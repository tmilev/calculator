/*
 * Adapted by Pieter Wuille in 2012; all changes are in the public domain
 *
 * Written in 2008 by Dwayne C. Litzenberger <dlitz@dlitz.net>
 *
 * ===================================================================
 * The contents of this file are dedicated to the public domain.  To
 * the extent that dedication to the public domain is not available,
 * everyone is granted a worldwide, perpetual, royalty-free,
 * non-exclusive license to exercise all rights associated with the
 * contents of this file for any purpose whatsoever.
 * No rights are reserved.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ===================================================================
 *
 * Country of origin: Canada
 *
 * This implementation (written in C) is based on an implementation the author
 * wrote in Python.
 *
 * This implementation was written with reference to the RIPEMD-160
 * specification, which is available at:
 * http://homes.esat.kuleuven.be/~cosicart/pdf/AB-9601/
 *
 * It is also documented in the _Handbook of Applied Cryptography_, as
 * Algorithm 9.55.  It's on page 30 of the following PDF file:
 * http://www.cacr.math.uwaterloo.ca/hac/about/chap9.pdf
 *
 * The RIPEMD-160 specification doesn't really tell us how to do padding, but
 * since RIPEMD-160 is inspired by MD4, you can use the padding algorithm from
 * RFC 1320.
 *
 * According to http://www.users.zetnet.co.uk/hopwood/crypto/scan/md.html:
 *   "RIPEMD-160 is big-bit-endian, little-byte-endian, and left-justified."
 */
#include "crypto_calculator.h"
#include "crypto_public_domain.h"
#include "general_logging_global_variables.h"

bool Crypto::flagRIPEMDBigEndian = false;
class Ripemd160State {
public:
  uint64_t length;
  union {
    uint32_t w[16];
    uint8_t b[64];
  } buf;

  uint32_t h[5];
  uint8_t bufpos;
};

class RIPEMD160Internal {
public:
  static const uint32_t initial_h[5];
  static const uint8_t RL[5][16];
  static const uint8_t RR[5][16];
  static const uint8_t SL[5][16];
  static const uint8_t SR[5][16];
  static const uint32_t KL[5];
  static const uint32_t KR[5];
  /* cyclic left-shift the 32-bit word n left by s bits */
  inline static uint32_t ROL(uint8_t s, uint32_t n) {
    return (((n) << (s)) | ((n) >> (32 -(s))));
  }
  /* Boolean functions */
  inline static uint32_t F1(uint32_t x, uint32_t y, uint32_t z) {
    return ((x) ^ (y) ^ (z));
  }
  inline static uint32_t F2(uint32_t x, uint32_t y, uint32_t z) {
    return (((x) &(y)) | (~(x) &(z)));
  }
  inline static uint32_t F3(uint32_t x, uint32_t y, uint32_t z) {
    return (((x) | ~(y)) ^ (z));
  }
  inline static uint32_t F4(uint32_t x, uint32_t y, uint32_t z) {
    return (((x) &(z)) | ((y) & ~(z)));
  }
  inline static uint32_t F5(uint32_t x, uint32_t y, uint32_t z) {
    return ((x) ^ ((y) | ~(z)));
  }
  static void ripemd160Initialize(Ripemd160State* self);
  static void ripemd160Done(Ripemd160State* self, unsigned char* out);
  static void ripemd160Process(
    Ripemd160State* self, const unsigned char* p, unsigned long length
  );
  static void ripemd160Compress(Ripemd160State* self);
  static inline void byteswapDigest(uint32_t* p);
  static inline void byteswap32(uint32_t* v);
};

/* Initial values for the chaining variables.
 * This is just 0123456789ABCDEFFEDCBA9876543210F0E1D2C3 in little-endian. */
const uint32_t RIPEMD160Internal::initial_h[5] =
{0x67452301u, 0xEFCDAB89u, 0x98BADCFEu, 0x10325476u, 0xC3D2E1F0u};
/* Ordering of message words.
 * Based on the permutations rho(i) and pi(i), defined as follows:
 *
 * rho(i) := { 7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8 }[i]
 * 0 <= i <= 15
 *
 *  pi(i) := 9*i + 5 (mod 16)
 *
 *  Line  |  Round 1  |  Round 2  |  Round 3  |  Round 4  |  Round 5
 * -------+-----------+-----------+-----------+-----------+-----------
 *  left  |    id     |    rho    |   rho^2   |   rho^3   |   rho^4
 *  right |    pi     |   rho pi  |  rho^2 pi |  rho^3 pi |  rho^4 pi
 */
/* Left line */
const uint8_t RIPEMD160Internal::RL[5][16] =
{
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  /* Round 1: id */
  {7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8},
  /* Round 2: rho */
  {3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12},
  /* Round 3: rho^2 */
  {1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2},
  /* Round 4: rho^3 */
  {4, 0, 5, 9, 7, 12, 2, 10, 14, 1, 3, 8, 11, 6, 15, 13} /* Round 5: rho^4 */
};
/* Right line */
const uint8_t RIPEMD160Internal::RR[5][16] =
{
  {5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12},
  /* Round 1: pi */
  {6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2},
  /* Round 2: rho pi */
  {15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13},
  /* Round 3: rho^2 pi */
  {8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14},
  /* Round 4: rho^3 pi */
  {12, 15, 10, 4, 1, 5, 8, 7, 6, 2, 13, 14, 0, 3, 9, 11}
  /* Round 5: rho^4 pi */
};
/*
 * Shifts - Since we don't actually re-order the message words according to
 * the permutations above (we could, but it would be slower), these tables
 * come with the permutations pre-applied.
 */
/* Shifts, left line */
const uint8_t RIPEMD160Internal::SL[5][16] =
{
  {11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8},
  /* Round 1 */
  {7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12},
  /* Round 2 */
  {11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5},
  /* Round 3 */
  {11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12},
  /* Round 4 */
  {9, 15, 5, 11, 6, 8, 13, 12, 5, 12, 13, 14, 11, 8, 5, 6} /* Round 5 */
};
/* Shifts, right line */
const uint8_t RIPEMD160Internal::SR[5][16] =
{
  {8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6},
  /* Round 1 */
  {9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11},
  /* Round 2 */
  {9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5},
  /* Round 3 */
  {15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8},
  /* Round 4 */
  {8, 5, 12, 9, 12, 5, 14, 6, 8, 13, 6, 5, 15, 13, 11, 11} /* Round 5 */
};
/* Round constants, left line */
const uint32_t RIPEMD160Internal::KL[5] =
{
  0x00000000u,
  /* Round 1: 0 */
  0x5A827999u,
  /* Round 2: floor(2**30 * sqrt(2)) */
  0x6ED9EBA1u,
  /* Round 3: floor(2**30 * sqrt(3)) */
  0x8F1BBCDCu,
  /* Round 4: floor(2**30 * sqrt(5)) */
  0xA953FD4Eu /* Round 5: floor(2**30 * sqrt(7)) */
};
/* Round constants, right line */
const uint32_t RIPEMD160Internal::KR[5] =
{
  0x50A28BE6u,
  /* Round 1: floor(2**30 * cubert(2)) */
  0x5C4DD124u,
  /* Round 2: floor(2**30 * cubert(3)) */
  0x6D703EF3u,
  /* Round 3: floor(2**30 * cubert(5)) */
  0x7A6D76E9u,
  /* Round 4: floor(2**30 * cubert(7)) */
  0x00000000u /* Round 5: 0 */
};

void RIPEMD160Internal::ripemd160Initialize(Ripemd160State* self) {
  memcpy(
    self->h, RIPEMD160Internal::initial_h, Crypto::RIPEMD160LengthInBytes
  );
  memset(&self->buf, 0, sizeof(self->buf));
  self->length = 0;
  self->bufpos = 0;
}

inline void RIPEMD160Internal::byteswap32(uint32_t* v) {
  union {
    uint32_t w;
    uint8_t b[4];
  } x, y;

  x.w = *v;
  y.b[0] = x.b[3];
  y.b[1] = x.b[2];
  y.b[2] = x.b[1];
  y.b[3] = x.b[0];
  *v = y.w;
  /* Wipe temporary variables */
  x.w = y.w = 0;
}

void RIPEMD160Internal::byteswapDigest(uint32_t* p) {
  unsigned int i;
  for (i = 0; i < 4; i ++) {
    byteswap32(p ++);
    byteswap32(p ++);
    byteswap32(p ++);
    byteswap32(p ++);
  }
}

/* The RIPEMD160 compression function.  Operates on self->buf */
void RIPEMD160Internal::ripemd160Compress(Ripemd160State* self) {
  uint8_t w;
  uint8_t round;
  uint32_t T;
  uint32_t AL;
  uint32_t BL;
  uint32_t CL;
  uint32_t DL;
  uint32_t EL;
  /* left line */
  uint32_t AR;
  uint32_t BR;
  uint32_t CR;
  uint32_t DR;
  uint32_t ER;
  /* right line */
  /* Sanity check */
  if (self->bufpos != 64) {
    global.fatal
    << "Internal error in RIPEMD160 implementation. "
    << global.fatal;
  }
  /* Byte-swap the buffer if we're on a big-endian machine */
  if (Crypto::flagRIPEMDBigEndian) {
    RIPEMD160Internal::byteswapDigest(self->buf.w);
  }
  /* load the left and right lines with the initial state */
  AL = AR = self->h[0];
  BL = BR = self->h[1];
  CL = CR = self->h[2];
  DL = DR = self->h[3];
  EL = ER = self->h[4];
  /* Round 1 */
  round = 0;
  for (w = 0; w < 16; w ++) {
    /* left line */ T =
    RIPEMD160Internal::ROL(
      RIPEMD160Internal::SL[round][w],
      AL +
      RIPEMD160Internal::F1(BL, CL, DL) +
      self->buf.w[RIPEMD160Internal::RL[round][w]] +
      RIPEMD160Internal::KL[round]
    ) +
    EL;
    AL = EL;
    EL = DL;
    DL = RIPEMD160Internal::ROL(10, CL);
    CL = BL;
    BL = T;
  }
  for (w = 0; w < 16; w ++) {
    /* right line */ T =
    RIPEMD160Internal::ROL(
      RIPEMD160Internal::SR[round][w],
      AR +
      RIPEMD160Internal::F5(BR, CR, DR) +
      self->buf.w[RIPEMD160Internal::RR[round][w]] +
      RIPEMD160Internal::KR[round]
    ) +
    ER;
    AR = ER;
    ER = DR;
    DR = RIPEMD160Internal::ROL(10, CR);
    CR = BR;
    BR = T;
  }
  /* Round 2 */
  round ++;
  for (w = 0; w < 16; w ++) {
    /* left line */ T =
    RIPEMD160Internal::ROL(
      RIPEMD160Internal::SL[round][w],
      AL +
      RIPEMD160Internal::F2(BL, CL, DL) +
      self->buf.w[RIPEMD160Internal::RL[round][w]] +
      RIPEMD160Internal::KL[round]
    ) +
    EL;
    AL = EL;
    EL = DL;
    DL = RIPEMD160Internal::ROL(10, CL);
    CL = BL;
    BL = T;
  }
  for (w = 0; w < 16; w ++) {
    /* right line */ T =
    RIPEMD160Internal::ROL(
      RIPEMD160Internal::SR[round][w],
      AR +
      RIPEMD160Internal::F4(BR, CR, DR) +
      self->buf.w[RIPEMD160Internal::RR[round][w]] +
      RIPEMD160Internal::KR[round]
    ) +
    ER;
    AR = ER;
    ER = DR;
    DR = RIPEMD160Internal::ROL(10, CR);
    CR = BR;
    BR = T;
  }
  /* Round 3 */
  round ++;
  for (w = 0; w < 16; w ++) {
    /* left line */ T =
    RIPEMD160Internal::ROL(
      RIPEMD160Internal::SL[round][w],
      AL +
      RIPEMD160Internal::F3(BL, CL, DL) +
      self->buf.w[RIPEMD160Internal::RL[round][w]] +
      RIPEMD160Internal::KL[round]
    ) +
    EL;
    AL = EL;
    EL = DL;
    DL = RIPEMD160Internal::ROL(10, CL);
    CL = BL;
    BL = T;
  }
  for (w = 0; w < 16; w ++) {
    /* right line */ T =
    RIPEMD160Internal::ROL(
      RIPEMD160Internal::SR[round][w],
      AR +
      RIPEMD160Internal::F3(BR, CR, DR) +
      self->buf.w[RIPEMD160Internal::RR[round][w]] +
      RIPEMD160Internal::KR[round]
    ) +
    ER;
    AR = ER;
    ER = DR;
    DR = RIPEMD160Internal::ROL(10, CR);
    CR = BR;
    BR = T;
  }
  /* Round 4 */
  round ++;
  for (w = 0; w < 16; w ++) {
    /* left line */ T =
    RIPEMD160Internal::ROL(
      RIPEMD160Internal::SL[round][w],
      AL +
      RIPEMD160Internal::F4(BL, CL, DL) +
      self->buf.w[RIPEMD160Internal::RL[round][w]] +
      RIPEMD160Internal::KL[round]
    ) +
    EL;
    AL = EL;
    EL = DL;
    DL = RIPEMD160Internal::ROL(10, CL);
    CL = BL;
    BL = T;
  }
  for (w = 0; w < 16; w ++) {
    /* right line */ T =
    RIPEMD160Internal::ROL(
      RIPEMD160Internal::SR[round][w],
      AR +
      RIPEMD160Internal::F2(BR, CR, DR) +
      self->buf.w[RIPEMD160Internal::RR[round][w]] +
      RIPEMD160Internal::KR[round]
    ) +
    ER;
    AR = ER;
    ER = DR;
    DR = RIPEMD160Internal::ROL(10, CR);
    CR = BR;
    BR = T;
  }
  /* Round 5 */
  round ++;
  for (w = 0; w < 16; w ++) {
    /* left line */ T =
    RIPEMD160Internal::ROL(
      RIPEMD160Internal::SL[round][w],
      AL +
      RIPEMD160Internal::F5(BL, CL, DL) +
      self->buf.w[RIPEMD160Internal::RL[round][w]] +
      RIPEMD160Internal::KL[round]
    ) +
    EL;
    AL = EL;
    EL = DL;
    DL = RIPEMD160Internal::ROL(10, CL);
    CL = BL;
    BL = T;
  }
  for (w = 0; w < 16; w ++) {
    /* right line */ T =
    RIPEMD160Internal::ROL(
      RIPEMD160Internal::SR[round][w],
      AR +
      RIPEMD160Internal::F1(BR, CR, DR) +
      self->buf.w[RIPEMD160Internal::RR[round][w]] +
      RIPEMD160Internal::KR[round]
    ) +
    ER;
    AR = ER;
    ER = DR;
    DR = RIPEMD160Internal::ROL(10, CR);
    CR = BR;
    BR = T;
  }
  /* Final mixing stage */
  T = self->h[1] + CL + DR;
  self->h[1] = self->h[2] + DL + ER;
  self->h[2] = self->h[3] + EL + AR;
  self->h[3] = self->h[4] + AL + BR;
  self->h[4] = self->h[0] + BL + CR;
  self->h[0] = T;
  /* clear the buffer and wipe the temporary variables */
  T = AL = BL = CL = DL = EL = AR = BR = CR = DR = ER = 0;
  memset(&self->buf, 0, sizeof(self->buf));
  self->bufpos = 0;
}

void RIPEMD160Internal::ripemd160Process(
  Ripemd160State* self, const unsigned char* p, unsigned long length
) {
  unsigned long bytes_needed;
  /* We never leave a full buffer */
  if (self->bufpos >= 64) {
    global.fatal
    << "Internal error in RIPEMD160 computation. "
    << global.fatal;
  }
  while (length > 0) {
    /* Figure out how many bytes we need to fill the internal buffer. */
    bytes_needed = 64 - self->bufpos;
    if (static_cast<unsigned long>(length) >= bytes_needed) {
      /* We have enough bytes, so copy them into the internal buffer and run
       * the compression function. */
      memcpy(&self->buf.b[self->bufpos], p, bytes_needed);
      self->bufpos += bytes_needed;
      self->length += bytes_needed << 3;
      /* length is in bits */
      p += bytes_needed;
      RIPEMD160Internal::ripemd160Compress(self);
      length -= bytes_needed;
      continue;
    }
    // We do not have enough bytes to fill the internal buffer.
    // Copy what's there and return.
    memcpy(&self->buf.b[self->bufpos], p, length);
    self->bufpos += length;
    self->length += length << 3;
    // length is in bits
    return;
  }
}

void RIPEMD160Internal::ripemd160Done(Ripemd160State* self, unsigned char* out)
{
  /* Append the padding */ self->buf.b[self->bufpos ++] = 0x80;
  if (self->bufpos > 56) {
    self->bufpos = 64;
    RIPEMD160Internal::ripemd160Compress(self);
  }
  /* Append the length */
  self->buf.w[14] = static_cast<uint32_t>(self->length & 0xFFFFffffu);
  self->buf.w[15] = static_cast<uint32_t>((self->length >> 32) & 0xFFFFffffu);
  if (Crypto::flagRIPEMDBigEndian) {
    byteswap32(&self->buf.w[14]);
    byteswap32(&self->buf.w[15]);
  }
  self->bufpos = 64;
  RIPEMD160Internal::ripemd160Compress(self);
  /* Copy the final state into the output buffer */
  if (Crypto::flagRIPEMDBigEndian) {
    RIPEMD160Internal::byteswapDigest(self->h);
  }
  memcpy(out, &self->h, Crypto::RIPEMD160LengthInBytes);
}

void CryptoPublicDomain::computeRIPEMD160(
  const std::string& input, List<unsigned char>& output
) {
  STACK_TRACE("Crypto::computeRIPEMD160");
  Ripemd160State md;
  RIPEMD160Internal::ripemd160Initialize(&md);
  output.setSize(20);
  RIPEMD160Internal::ripemd160Process(
    &md, reinterpret_cast<const unsigned char*>(input.c_str()), input.size()
  );
  RIPEMD160Internal::ripemd160Done(&md, output.objects);
}
