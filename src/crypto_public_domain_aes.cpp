
/*

This is an implementation of the AES algorithm, specifically ECB, CTR and CBC mode.
Block size can be chosen in aes.h - available choices are AES128, AES192, AES256.

The implementation is verified against the test vectors in:
  National Institute of Standards and Technology Special Publication 800-38A 2001 ED

ECB-AES128
----------

  plain-text:
    6bc1bee22e409f96e93d7e117393172a
    ae2d8a571e03ac9c9eb76fac45af8e51
    30c81c46a35ce411e5fbc1191a0a52ef
    f69f2445df4f9b17ad2b417be66c3710

  key:
    2b7e151628aed2a6abf7158809cf4f3c

  resulting cipher
    3ad77bb40d7a3660a89ecaf32466ef97
    f5d3d58503b9699de785895a96fdbaaf
    43b1cd7f598ece23881b00e3ed030688
    7b0c785e27e8ad3f8223207104725dd4


NOTE:   String length must be evenly divisible by 16byte (str_len % 16 == 0)
        You should pad the end of the string with zeros if this is not the case.
        For AES192/256 the key size is proportionally larger.

*/
#include "crypto_public_domain.h"

class AESContext {
public:
  static const int blockLength = 16;
  static const int maxKeyExpSize = 240;
  static const uint8_t sbox[256];
  static const uint8_t rsbox[256];
  static const uint8_t rCon[11];
  // CBC enables AES encryption in CBC-mode of operation.
  // CTR enables encryption in counter-mode.
  // ECB enables the basic ECB 16-byte block algorithm. All can be enabled
  // simultaneously.
  bool flagUseCBC;
  bool flagUseECB;
  bool flagUseCTR;
  int numBits;
  // NumBits can be 256, 192 or 128.
  int keyExpSize;
  int keyLength;
  uint8_t RoundKey[maxKeyExpSize];
  uint8_t initializationVector[blockLength];
  // The number of columns comprising a state in AES. This is a constant in
  // AES.
  // Value=4
  const static unsigned Nb = 4;
  unsigned Nk;
  // The number of 32 bit words in a key.
  unsigned Nr;
  // The number of rounds in AES cipher.
  /**************************************************************************/
  /* Private variables:                                                     */
  /**************************************************************************/
  // state - array holding the intermediate results during decryption.
  typedef uint8_t state_t[4][4];
  AESContext();
  void setNumberOfBits(int inputNumBits);
  // buffer size MUST be multiple of blockLength;
  // Suggest https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for
  // padding scheme
  // NOTES: you need to set IV in ctx via initializeVector() or
  // setInitializationVector()
  //        no IV should ever be reused with the same key
  void initializeContext(const uint8_t* key);
  void initializeVector(const uint8_t* key, const uint8_t* iv);
  void setInitializationVector(const uint8_t* iv);
  void AES_ECB_encrypt(uint8_t* buf);
  void AES_ECB_decrypt(uint8_t* buf);
  void AES_CBC_encrypt_buffer(uint8_t* buf, uint32_t length);
  void AES_CBC_decrypt_buffer(uint8_t* buf, uint32_t length);
  // Same function for encrypting as for decrypting.
  // IV is incremented for every block, and used after encryption as
  // XOR-compliment for output
  // Suggesting https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for
  // padding scheme
  // NOTES: you need to set IV in ctx with initializeVector() or
  // setInitializationVector()
  //        no IV should ever be reused with the same key
  void AES_CTR_xcrypt_buffer(uint8_t* buf, uint32_t length);
  static inline uint8_t xtime(uint8_t x) {
    return static_cast<uint8_t>(((x << 1) ^ (((x >> 7) & 1) * 0x1b)));
  }
  // Multiply is used to multiply numbers in the field GF(2^8)
  // Note: The last call to xtime() is unneeded, but often ends up generating a
  // smaller binary
  // The compiler seems to be able to vectorize the operation better this way.
  // See https://github.com/kokke/tiny-AES-c/pull/34
  static inline uint8_t multiply(uint8_t x, uint8_t y) {
    return (((y& 1) * x) ^ ((y >> 1 & 1) * xtime(x)) ^ ((y >> 2 & 1) *
      xtime(xtime(x))
    ) ^ ((y >> 3 & 1) * xtime(xtime(xtime(x)))) ^ ((y >> 4 & 1) *
      xtime(xtime(xtime(xtime(x))))
    ));
    /* this last call to xtime() can be omitted */
  }
  static void mixColumns(state_t* state);
  void addRoundKey(uint8_t round, state_t* state, uint8_t* RoundKey);
  static void substituteBytes(state_t* state);
  static void shiftRows(state_t* state);
  static void invMixColumns(state_t* state);
  static void invSubstitutionBytes(state_t* state);
  static void invShiftRows(state_t* state);
  void cipher(state_t* state, uint8_t* RoundKey);
  void invCipher(state_t* state, uint8_t* RoundKey);
  void keyExpansion(uint8_t* RoundKey, const uint8_t* Key);
  static uint8_t getSBoxValue(uint8_t num);
  static uint8_t getSBoxInvert(uint8_t num);
  static void xOrWithIv(uint8_t* buf, uint8_t* Iv);
};

void AESContext::setNumberOfBits(int inputNumBits) {
  this->numBits = inputNumBits;
  if (
    this->numBits != 256 && this->numBits != 192 && this->numBits != 128
  ) {
    global.fatal
    << "Bad number of bits: "
    << inputNumBits
    << " for AES cipher. Allowed inputs: 256, 192, 128. "
    << global.fatal;
  }
  if (this->numBits == 256) {
    this->keyExpSize = 240;
    this->keyLength = 32;
    this->Nk = 8;
    // The number of 32 bit words in a key.
    this->Nr = 14;
    // The number of rounds in AES cipher.
  }
  if (this->numBits == 192) {
    this->keyExpSize = 208;
    this->keyLength = 24;
    this->Nk = 6;
    this->Nr = 12;
  }
  if (this->numBits == 128) {
    this->keyExpSize = 176;
    this->keyLength = 16;
    this->Nk = 4;
    this->Nr = 10;
  }
}

AESContext::AESContext() {
  this->flagUseCBC = true;
  this->flagUseCTR = false;
  this->flagUseECB = false;
  for (int i = 0; i < 16; i ++) {
    this->initializationVector[i] = static_cast<uint8_t>(i);
  }
  this->setNumberOfBits(256);
}

// The lookup-tables are marked const so they can be placed in read-only
// storage
// instead of RAM
// The numbers below can be computed dynamically trading ROM for RAM -
// This can be useful in (embedded) bootloader applications, where ROM is often
// limited.
const uint8_t AESContext::sbox[256] =
{
    // 0     1    2      3     4    5     6     7
  // 8     9     A      B    C     D     E     F
  0x63,
  0x7c,
  0x77,
  0x7b,
  0xf2,
  0x6b,
  0x6f,
  0xc5,
  0x30,
  0x01,
  0x67,
  0x2b,
  0xfe,
  0xd7,
  0xab,
  0x76,
  0xca,
  0x82,
  0xc9,
  0x7d,
  0xfa,
  0x59,
  0x47,
  0xf0,
  0xad,
  0xd4,
  0xa2,
  0xaf,
  0x9c,
  0xa4,
  0x72,
  0xc0,
  0xb7,
  0xfd,
  0x93,
  0x26,
  0x36,
  0x3f,
  0xf7,
  0xcc,
  0x34,
  0xa5,
  0xe5,
  0xf1,
  0x71,
  0xd8,
  0x31,
  0x15,
  0x04,
  0xc7,
  0x23,
  0xc3,
  0x18,
  0x96,
  0x05,
  0x9a,
  0x07,
  0x12,
  0x80,
  0xe2,
  0xeb,
  0x27,
  0xb2,
  0x75,
  0x09,
  0x83,
  0x2c,
  0x1a,
  0x1b,
  0x6e,
  0x5a,
  0xa0,
  0x52,
  0x3b,
  0xd6,
  0xb3,
  0x29,
  0xe3,
  0x2f,
  0x84,
  0x53,
  0xd1,
  0x00,
  0xed,
  0x20,
  0xfc,
  0xb1,
  0x5b,
  0x6a,
  0xcb,
  0xbe,
  0x39,
  0x4a,
  0x4c,
  0x58,
  0xcf,
  0xd0,
  0xef,
  0xaa,
  0xfb,
  0x43,
  0x4d,
  0x33,
  0x85,
  0x45,
  0xf9,
  0x02,
  0x7f,
  0x50,
  0x3c,
  0x9f,
  0xa8,
  0x51,
  0xa3,
  0x40,
  0x8f,
  0x92,
  0x9d,
  0x38,
  0xf5,
  0xbc,
  0xb6,
  0xda,
  0x21,
  0x10,
  0xff,
  0xf3,
  0xd2,
  0xcd,
  0x0c,
  0x13,
  0xec,
  0x5f,
  0x97,
  0x44,
  0x17,
  0xc4,
  0xa7,
  0x7e,
  0x3d,
  0x64,
  0x5d,
  0x19,
  0x73,
  0x60,
  0x81,
  0x4f,
  0xdc,
  0x22,
  0x2a,
  0x90,
  0x88,
  0x46,
  0xee,
  0xb8,
  0x14,
  0xde,
  0x5e,
  0x0b,
  0xdb,
  0xe0,
  0x32,
  0x3a,
  0x0a,
  0x49,
  0x06,
  0x24,
  0x5c,
  0xc2,
  0xd3,
  0xac,
  0x62,
  0x91,
  0x95,
  0xe4,
  0x79,
  0xe7,
  0xc8,
  0x37,
  0x6d,
  0x8d,
  0xd5,
  0x4e,
  0xa9,
  0x6c,
  0x56,
  0xf4,
  0xea,
  0x65,
  0x7a,
  0xae,
  0x08,
  0xba,
  0x78,
  0x25,
  0x2e,
  0x1c,
  0xa6,
  0xb4,
  0xc6,
  0xe8,
  0xdd,
  0x74,
  0x1f,
  0x4b,
  0xbd,
  0x8b,
  0x8a,
  0x70,
  0x3e,
  0xb5,
  0x66,
  0x48,
  0x03,
  0xf6,
  0x0e,
  0x61,
  0x35,
  0x57,
  0xb9,
  0x86,
  0xc1,
  0x1d,
  0x9e,
  0xe1,
  0xf8,
  0x98,
  0x11,
  0x69,
  0xd9,
  0x8e,
  0x94,
  0x9b,
  0x1e,
  0x87,
  0xe9,
  0xce,
  0x55,
  0x28,
  0xdf,
  0x8c,
  0xa1,
  0x89,
  0x0d,
  0xbf,
  0xe6,
  0x42,
  0x68,
  0x41,
  0x99,
  0x2d,
  0x0f,
  0xb0,
  0x54,
  0xbb,
  0x16
};
const uint8_t AESContext::rsbox[256] =
{
  0x52,
  0x09,
  0x6a,
  0xd5,
  0x30,
  0x36,
  0xa5,
  0x38,
  0xbf,
  0x40,
  0xa3,
  0x9e,
  0x81,
  0xf3,
  0xd7,
  0xfb,
  0x7c,
  0xe3,
  0x39,
  0x82,
  0x9b,
  0x2f,
  0xff,
  0x87,
  0x34,
  0x8e,
  0x43,
  0x44,
  0xc4,
  0xde,
  0xe9,
  0xcb,
  0x54,
  0x7b,
  0x94,
  0x32,
  0xa6,
  0xc2,
  0x23,
  0x3d,
  0xee,
  0x4c,
  0x95,
  0x0b,
  0x42,
  0xfa,
  0xc3,
  0x4e,
  0x08,
  0x2e,
  0xa1,
  0x66,
  0x28,
  0xd9,
  0x24,
  0xb2,
  0x76,
  0x5b,
  0xa2,
  0x49,
  0x6d,
  0x8b,
  0xd1,
  0x25,
  0x72,
  0xf8,
  0xf6,
  0x64,
  0x86,
  0x68,
  0x98,
  0x16,
  0xd4,
  0xa4,
  0x5c,
  0xcc,
  0x5d,
  0x65,
  0xb6,
  0x92,
  0x6c,
  0x70,
  0x48,
  0x50,
  0xfd,
  0xed,
  0xb9,
  0xda,
  0x5e,
  0x15,
  0x46,
  0x57,
  0xa7,
  0x8d,
  0x9d,
  0x84,
  0x90,
  0xd8,
  0xab,
  0x00,
  0x8c,
  0xbc,
  0xd3,
  0x0a,
  0xf7,
  0xe4,
  0x58,
  0x05,
  0xb8,
  0xb3,
  0x45,
  0x06,
  0xd0,
  0x2c,
  0x1e,
  0x8f,
  0xca,
  0x3f,
  0x0f,
  0x02,
  0xc1,
  0xaf,
  0xbd,
  0x03,
  0x01,
  0x13,
  0x8a,
  0x6b,
  0x3a,
  0x91,
  0x11,
  0x41,
  0x4f,
  0x67,
  0xdc,
  0xea,
  0x97,
  0xf2,
  0xcf,
  0xce,
  0xf0,
  0xb4,
  0xe6,
  0x73,
  0x96,
  0xac,
  0x74,
  0x22,
  0xe7,
  0xad,
  0x35,
  0x85,
  0xe2,
  0xf9,
  0x37,
  0xe8,
  0x1c,
  0x75,
  0xdf,
  0x6e,
  0x47,
  0xf1,
  0x1a,
  0x71,
  0x1d,
  0x29,
  0xc5,
  0x89,
  0x6f,
  0xb7,
  0x62,
  0x0e,
  0xaa,
  0x18,
  0xbe,
  0x1b,
  0xfc,
  0x56,
  0x3e,
  0x4b,
  0xc6,
  0xd2,
  0x79,
  0x20,
  0x9a,
  0xdb,
  0xc0,
  0xfe,
  0x78,
  0xcd,
  0x5a,
  0xf4,
  0x1f,
  0xdd,
  0xa8,
  0x33,
  0x88,
  0x07,
  0xc7,
  0x31,
  0xb1,
  0x12,
  0x10,
  0x59,
  0x27,
  0x80,
  0xec,
  0x5f,
  0x60,
  0x51,
  0x7f,
  0xa9,
  0x19,
  0xb5,
  0x4a,
  0x0d,
  0x2d,
  0xe5,
  0x7a,
  0x9f,
  0x93,
  0xc9,
  0x9c,
  0xef,
  0xa0,
  0xe0,
  0x3b,
  0x4d,
  0xae,
  0x2a,
  0xf5,
  0xb0,
  0xc8,
  0xeb,
  0xbb,
  0x3c,
  0x83,
  0x53,
  0x99,
  0x61,
  0x17,
  0x2b,
  0x04,
  0x7e,
  0xba,
  0x77,
  0xd6,
  0x26,
  0xe1,
  0x69,
  0x14,
  0x63,
  0x55,
  0x21,
  0x0c,
  0x7d
};
// The round constant word array, rCon[i], contains the values given by
// x to the power (i - 1) being powers of x (x is denoted as {02}) in the field
// GF(2^8)
const uint8_t AESContext::rCon[11] =
{0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};
/*
 * Jordan Goulder points out in PR #12
 * (https://github.com/kokke/tiny-AES-C/pull/12),
 * that you can remove most of the elements
 * in the Rcon array, because they are unused.
 *
 * From Wikipedia's article on the Rijndael key schedule
 * @ https://en.wikipedia.org/wiki/Rijndael_key_schedule#Rcon
 *
 * "Only the first some of these constants
 * are actually used – up to rcon[10] for AES-128
 * (as 11 round keys are needed),
 * up to rcon[8] for AES-192, up to rcon[7] for AES-256.
 * rcon[0] is not used in AES algorithm."
 */
uint8_t AESContext::getSBoxValue(uint8_t num) {
  return AESContext::sbox[num];
}

uint8_t AESContext::getSBoxInvert(uint8_t num) {
  return AESContext::rsbox[num];
}

// This function produces Nb(Nr+1) round keys. The round keys are used in each
// round to decrypt the states.
void AESContext::keyExpansion(uint8_t* RoundKey, const uint8_t* Key) {
  unsigned i, j, k;
  uint8_t tempa[4];
  // Used for the column/row operations
  // The first round key is the key itself.
  for (i = 0; i < Nk; ++ i) {
    RoundKey[(i* 4) + 0] = Key[(i* 4) + 0];
    RoundKey[(i* 4) + 1] = Key[(i* 4) + 1];
    RoundKey[(i* 4) + 2] = Key[(i* 4) + 2];
    RoundKey[(i* 4) + 3] = Key[(i* 4) + 3];
  }
  // All other round keys are found from the previous round keys.
  for (i = Nk; i < Nb *(Nr + 1); ++ i) {
    k = (i - 1) * 4;
    tempa[0] = RoundKey[k + 0];
    tempa[1] = RoundKey[k + 1];
    tempa[2] = RoundKey[k + 2];
    tempa[3] = RoundKey[k + 3];
    if (i % Nk == 0) {
      // This function shifts the 4 bytes in a word to the left once.
      // [a0, a1, a2, a3] becomes [a1, a2, a3, a0]
      // Function RotWord()
      k = tempa[0];
      tempa[0] = tempa[1];
      tempa[1] = tempa[2];
      tempa[2] = tempa[3];
      tempa[3] = static_cast<uint8_t>(k);
      // SubWord() is a function that takes a four-byte input word and
      // applies the S-box to each of the four bytes to produce an output word.
      // Function Subword()
      tempa[0] = getSBoxValue(tempa[0]);
      tempa[1] = getSBoxValue(tempa[1]);
      tempa[2] = getSBoxValue(tempa[2]);
      tempa[3] = getSBoxValue(tempa[3]);
      tempa[0] = tempa[0] ^ rCon[i / Nk];
    }
    if (this->numBits == 256) {
      if (i % Nk == 4) {
        tempa[0] = getSBoxValue(tempa[0]);
        tempa[1] = getSBoxValue(tempa[1]);
        tempa[2] = getSBoxValue(tempa[2]);
        tempa[3] = getSBoxValue(tempa[3]);
      }
    }
    j = i * 4;
    k = (i - Nk) * 4;
    RoundKey[j + 0] = RoundKey[k + 0] ^ tempa[0];
    RoundKey[j + 1] = RoundKey[k + 1] ^ tempa[1];
    RoundKey[j + 2] = RoundKey[k + 2] ^ tempa[2];
    RoundKey[j + 3] = RoundKey[k + 3] ^ tempa[3];
  }
}

void AESContext::initializeContext(const uint8_t* key) {
  keyExpansion(this->RoundKey, key);
}

void AESContext::initializeVector(const uint8_t* key, const uint8_t* iv) {
  keyExpansion(this->RoundKey, key);
  memcpy(this->initializationVector, iv, this->blockLength);
}

void AESContext::setInitializationVector(const uint8_t* iv) {
  memcpy(this->initializationVector, iv, this->blockLength);
}

// This function adds the round key to state.
// The round key is added to the state by an XOR function.
void AESContext::addRoundKey(
  uint8_t round, state_t* state, uint8_t* RoundKey
) {
  uint8_t i, j;
  for (i = 0; i < 4; ++ i) {
    for (j = 0; j < 4; ++ j) {
      (*state)[i][j] ^= RoundKey[(round * Nb * 4) + (i* Nb) + j];
    }
  }
}

// The substituteBytes Function Substitutes the values in the
// state matrix with values in an S-box.
void AESContext::substituteBytes(state_t* state) {
  uint8_t i, j;
  for (i = 0; i < 4; ++ i) {
    for (j = 0; j < 4; ++ j) {
      (*state)[j][i] = getSBoxValue((*state)[j][i]);
    }
  }
}

// The shiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
void AESContext::shiftRows(state_t* state) {
  uint8_t temp;
  // rotate first row 1 columns to left
  temp = (*state)[0][1];
  (*state)[0][1] = (*state)[1][1];
  (*state)[1][1] = (*state)[2][1];
  (*state)[2][1] = (*state)[3][1];
  (*state)[3][1] = temp;
  // rotate second row 2 columns to left
  temp = (*state)[0][2];
  (*state)[0][2] = (*state)[2][2];
  (*state)[2][2] = temp;
  temp = (*state)[1][2];
  (*state)[1][2] = (*state)[3][2];
  (*state)[3][2] = temp;
  // rotate third row 3 columns to left
  temp = (*state)[0][3];
  (*state)[0][3] = (*state)[3][3];
  (*state)[3][3] = (*state)[2][3];
  (*state)[2][3] = (*state)[1][3];
  (*state)[1][3] = temp;
}

// mixColumns function mixes the columns of the state matrix
void AESContext::mixColumns(state_t* state) {
  uint8_t i;
  uint8_t Tmp, Tm, t;
  for (i = 0; i < 4; ++ i) {
    t = (*state)[i][0];
    Tmp = (*state)[i][0] ^ (*state)[i][1] ^ (*state)[i][2] ^ (*state)[i][3];
    Tm = (*state)[i][0] ^ (*state)[i][1];
    Tm = AESContext::xtime(Tm);
    (*state)[i][0] ^= Tm ^ Tmp;
    Tm = (*state)[i][1] ^ (*state)[i][2];
    Tm = AESContext::xtime(Tm);
    (*state)[i][1] ^= Tm ^ Tmp;
    Tm = (*state)[i][2] ^ (*state)[i][3];
    Tm = AESContext::xtime(Tm);
    (*state)[i][2] ^= Tm ^ Tmp;
    Tm = (*state)[i][3] ^ t;
    Tm = AESContext::xtime(Tm);
    (*state)[i][3] ^= Tm ^ Tmp;
  }
}

// mixColumns function mixes the columns of the state matrix.
// The method used to multiply may be difficult to understand for the
// inexperienced.
// Please use the references to gain more information.
void AESContext::invMixColumns(state_t* state) {
  int i;
  uint8_t a, b, c, d;
  for (i = 0; i < 4; ++ i) {
    a = (*state)[i][0];
    b = (*state)[i][1];
    c = (*state)[i][2];
    d = (*state)[i][3];
    (*state)[i][0] = multiply(a, 0x0e) ^ multiply(b, 0x0b) ^ multiply(c, 0x0d)
    ^
    multiply(d, 0x09);
    (*state)[i][1] = multiply(a, 0x09) ^ multiply(b, 0x0e) ^ multiply(c, 0x0b)
    ^
    multiply(d, 0x0d);
    (*state)[i][2] = multiply(a, 0x0d) ^ multiply(b, 0x09) ^ multiply(c, 0x0e)
    ^
    multiply(d, 0x0b);
    (*state)[i][3] = multiply(a, 0x0b) ^ multiply(b, 0x0d) ^ multiply(c, 0x09)
    ^
    multiply(d, 0x0e);
  }
}

// The substituteBytes Function Substitutes the values in the
// state matrix with values in an S-box.
void AESContext::invSubstitutionBytes(state_t* state) {
  uint8_t i, j;
  for (i = 0; i < 4; ++ i) {
    for (j = 0; j < 4; ++ j) {
      (*state)[j][i] = getSBoxInvert((*state)[j][i]);
    }
  }
}

void AESContext::invShiftRows(state_t* state) {
  uint8_t temp;
  // rotate first row 1 columns to right
  temp = (*state)[3][1];
  (*state)[3][1] = (*state)[2][1];
  (*state)[2][1] = (*state)[1][1];
  (*state)[1][1] = (*state)[0][1];
  (*state)[0][1] = temp;
  // rotate second row 2 columns to right
  temp = (*state)[0][2];
  (*state)[0][2] = (*state)[2][2];
  (*state)[2][2] = temp;
  temp = (*state)[1][2];
  (*state)[1][2] = (*state)[3][2];
  (*state)[3][2] = temp;
  // rotate third row 3 columns to right
  temp = (*state)[0][3];
  (*state)[0][3] = (*state)[1][3];
  (*state)[1][3] = (*state)[2][3];
  (*state)[2][3] = (*state)[3][3];
  (*state)[3][3] = temp;
}

// cipher is the main function that encrypts the PlainText.
void AESContext::cipher(state_t* state, uint8_t* RoundKey) {
  uint8_t round = 0;
  // Add the First round key to the state before starting the rounds.
  addRoundKey(0, state, RoundKey);
  // There will be Nr rounds.
  // The first Nr-1 rounds are identical.
  // These Nr-1 rounds are executed in the loop below.
  for (round = 1; round < this->Nr; ++ round) {
    substituteBytes(state);
    shiftRows(state);
    mixColumns(state);
    addRoundKey(round, state, RoundKey);
  }
  // The last round is given below.
  // The mixColumns function is not here in the last round.
  substituteBytes(state);
  shiftRows(state);
  addRoundKey(static_cast<uint8_t>(this->Nr), state, RoundKey);
}

void AESContext::invCipher(state_t* state, uint8_t* RoundKey) {
  uint8_t round = 0;
  // Add the First round key to the state before starting the rounds.
  addRoundKey(static_cast<uint8_t>(Nr), state, RoundKey);
  // There will be Nr rounds.
  // The first Nr-1 rounds are identical.
  // These Nr-1 rounds are executed in the loop below.
  for (round = (static_cast<uint8_t>(Nr) - 1); round > 0; -- round) {
    invShiftRows(state);
    invSubstitutionBytes(state);
    addRoundKey(round, state, RoundKey);
    invMixColumns(state);
  }
  // The last round is given below.
  // The mixColumns function is not here in the last round.
  invShiftRows(state);
  invSubstitutionBytes(state);
  addRoundKey(0, state, RoundKey);
}

void AESContext::AES_ECB_encrypt(uint8_t* buf) {
  // The next function call encrypts the PlainText with the Key using AES
  // algorithm.
  cipher(reinterpret_cast<state_t*>(buf), this->RoundKey);
}

void AESContext::AES_ECB_decrypt(uint8_t* buf) {
  // The next function call decrypts the PlainText with the Key using AES
  // algorithm.
  invCipher(reinterpret_cast<state_t*>(buf), this->RoundKey);
}

void AESContext::xOrWithIv(uint8_t* buf, uint8_t* Iv) {
  uint8_t i;
  for (i = 0; i < AESContext::blockLength; ++ i) {
    // The block in AES is always 128-bit no matter the key size
    buf[i] ^=
    Iv[i];
  }
}

void AESContext::AES_CBC_encrypt_buffer(uint8_t* buf, uint32_t length) {
  uintptr_t i;
  uint8_t* Iv = this->initializationVector;
  for (i = 0; i < length; i += AESContext::blockLength) {
    xOrWithIv(buf, Iv);
    cipher(reinterpret_cast<state_t*>(buf), this->RoundKey);
    Iv = buf;
    buf += AESContext::blockLength;
    // printf("Step %d - %d", i/16, i);
  }
  /* store Iv in ctx for next call */
  memcpy(this->initializationVector, Iv, AESContext::blockLength);
}

void AESContext::AES_CBC_decrypt_buffer(uint8_t* buf, uint32_t length) {
  uintptr_t i;
  uint8_t storeNextIv[AESContext::blockLength];
  for (i = 0; i < length; i += this->blockLength) {
    memcpy(storeNextIv, buf, this->blockLength);
    invCipher(reinterpret_cast<state_t*>(buf), this->RoundKey);
    xOrWithIv(buf, this->initializationVector);
    memcpy(this->initializationVector, storeNextIv, this->blockLength);
    buf += this->blockLength;
  }
}

// Symmetrical operation: same function for encrypting as for decrypting.
// Note any IV / nonce should never be reused with the same key.
void AESContext::AES_CTR_xcrypt_buffer(uint8_t* buf, uint32_t length) {
  uint8_t buffer[AESContext::blockLength];
  unsigned i;
  int bi;
  for (i = 0, bi = this->blockLength; i < length; ++ i, ++ bi) {
    if (bi == this->blockLength) {
      /* we need to regen xor compliment in buffer */ memcpy(
        buffer, this->initializationVector, this->blockLength
      );
      cipher(reinterpret_cast<state_t*>(buffer), this->RoundKey);
      /* Increment Iv and handle overflow */
      for (bi = (this->blockLength - 1); bi >= 0; -- bi) {
        /* inc will owerflow */
        if (this->initializationVector[bi] == 255) {
          this->initializationVector[bi] = 0;
          continue;
        }
        this->initializationVector[bi] += 1;
        break;
      }
      bi = 0;
    }
    buf[i] = buf[i] ^ buffer[bi];
  }
}

bool CryptoPublicDomain::decryptAES_CBC_256(
  const std::string& inputKey,
  const std::string& inputCipherText,
  List<unsigned char>& output,
  std::stringstream* commentsOnFailure
) {
  output.setSize(static_cast<signed>(inputCipherText.size()));
  if (inputKey.size() != 32) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Input key: "
      << inputKey
      << " is of length: "
      << inputKey.size()
      << " (expected: 32). ";
    }
    return false;
  }
  int remainderMod16 = inputCipherText.size() % 16;
  if (remainderMod16 > 0) {
    int numberOfBytesToPad = 16 - remainderMod16;
    std::string newInput;
    newInput.reserve(
      inputCipherText.size() + static_cast<unsigned>(numberOfBytesToPad)
    );
    newInput = inputCipherText;
    for (int i = 0; i < numberOfBytesToPad; i ++) {
      newInput.push_back(0);
    }
    return
    CryptoPublicDomain::decryptAES_CBC_256(
      inputKey, inputCipherText, output, commentsOnFailure
    );
  }
  AESContext context;
  context.initializeContext(
    reinterpret_cast<const uint8_t*>(inputKey.c_str())
  );
  output = inputCipherText;
  context.AES_CBC_decrypt_buffer(
    static_cast<uint8_t*>(output.objects),
    static_cast<unsigned>(output.size)
  );
  return true;
}

bool CryptoPublicDomain::encryptAES_CBC_256(
  const std::string& inputKey,
  const std::string& inputPlainText,
  List<unsigned char>& output,
  std::stringstream* commentsOnFailure
) {
  output.setSize(static_cast<signed>(inputPlainText.size()));
  if (inputKey.size() != 32) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Input key: "
      << inputKey
      << " is of length: "
      << inputKey.size()
      << " (expected: 32). ";
    }
    return false;
  }
  unsigned remainderMod16 = inputPlainText.size() % 16;
  if (remainderMod16 > 0) {
    unsigned numberOfBytesToPad = 16 - remainderMod16;
    std::string newInput;
    newInput.reserve(inputPlainText.size() + numberOfBytesToPad);
    newInput = inputPlainText;
    for (unsigned i = 0; i < numberOfBytesToPad; i ++) {
      newInput.push_back(static_cast<char>(numberOfBytesToPad));
    }
    return
    CryptoPublicDomain::encryptAES_CBC_256(
      inputKey, newInput, output, commentsOnFailure
    );
  }
  AESContext context;
  context.initializeContext(
    reinterpret_cast<const uint8_t*>(inputKey.c_str())
  );
  output = inputPlainText;
  context.AES_CBC_encrypt_buffer(
    reinterpret_cast<uint8_t*>(output.objects),
    static_cast<unsigned>(output.size)
  );
  return true;
}
