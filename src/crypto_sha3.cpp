#include "crypto.h"


// Reference:
// Wikipedia's page on SHA3:
// https://en.wikipedia.org/wiki/SHA-3
class Sha3 {
public:
  static const int numberOfSpongeWords = 25;
  static const int numberOfKeccakRounds = 24;
  // The portion of the input message that we didn't consume yet.
  bool flagUseKeccak;
  uint64_t saved;
  union {
    // Keccak's state
    uint64_t stateStorage[Sha3::numberOfSpongeWords];
    uint8_t sb[Sha3::numberOfSpongeWords * 8];
  };
  // 0..7--the next byte after the set one (starts from 0; 0--none are buffered)
  unsigned byteIndex;

  // 0..24--the next word to integrate input (starts from 0)
  unsigned wordIndex;
  // The double size of the hash output in words (e.g. 16 for Keccak 512)
  unsigned capacityWords;
  std::string computeSha3_256(const std::string& input);
  void getResultVector(List<unsigned char>& output);
  std::string getResultString();
  void init();
  void update(List<unsigned char>& input);
  void update(const std::string& input);
  void update(void const* inputBuffer, size_t length);
  void finalize();
  Sha3();
  void transform(uint64_t* state25Elements);
  void OneRoundTheta(uint64_t* state25Elements);
  void reset();
  void sha3_Init256();
  void sha3_Init384();
  void sha3_Init512();
  void sha3_Update(void const *bufIn, size_t len);
  static uint64_t RotateLeft(uint64_t x, unsigned y) {
    return ((x << y) | (x >> ((sizeof(uint64_t) * 8) - y)));
  }
};

Sha3::Sha3() {
  this->flagUseKeccak = false;
}

// Define SHA3_USE_KECCAK to run "pure" Keccak, as opposed to SHA3.
// The tests that this macro enables use the input and output from [Keccak]
// (see the reference below). The used test vectors aren't correct for SHA3,
// however, they are helpful to verify the implementation.
// SHA3_USE_KECCAK only changes one line of code in Finalize.

static const uint64_t keccakf_rndc[24] = {
  0x0000000000000001UL, 0x0000000000008082UL,
  0x800000000000808aUL, 0x8000000080008000UL,
  0x000000000000808bUL, 0x0000000080000001UL,
  0x8000000080008081UL, 0x8000000000008009UL,
  0x000000000000008aUL, 0x0000000000000088UL,
  0x0000000080008009UL, 0x000000008000000aUL,
  0x000000008000808bUL, 0x800000000000008bUL,
  0x8000000000008089UL, 0x8000000000008003UL,
  0x8000000000008002UL, 0x8000000000000080UL,
  0x000000000000800aUL, 0x800000008000000aUL,
  0x8000000080008081UL, 0x8000000000008080UL,
  0x0000000080000001UL, 0x8000000080008008UL
};

static const unsigned keccakf_rotc[24] ={
  1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 2, 14, 27, 41, 56, 8, 25, 43, 62,
  18, 39, 61, 20, 44
};

static const unsigned keccakf_piln[24] = {
  10, 7, 11, 17, 18, 3, 5, 16, 8, 21, 24, 4, 15, 23, 19, 13, 12, 2, 20,
  14, 22, 9, 6, 1
};

void Sha3::transform(uint64_t *state25Elements) {
  uint64_t t, bc[5];
  for (int round = 0; round < Sha3::numberOfKeccakRounds; round ++) {
    // Theta
    for (int i = 0; i < 5; i ++) {
      bc[i] = state25Elements[i] xor state25Elements[i + 5] xor state25Elements[i + 10] xor state25Elements[i + 15] xor state25Elements[i + 20];
    }
    for (int i = 0; i < 5; i ++) {
      t = bc[(i + 4) % 5] ^ Sha3::RotateLeft(bc[(i + 1) % 5], 1);
      for (int j = 0; j < 25; j += 5) {
        state25Elements[j + i] ^= t;
      }
    }
    // Rho Pi
    t = state25Elements[1];
    for (int i = 0; i < 24; i ++) {
      int j = static_cast<int>(keccakf_piln[i]);
      bc[0] = state25Elements[j];
      state25Elements[j] = Sha3::RotateLeft(t, keccakf_rotc[i]);
      t = bc[0];
    }
    // Chi
    for (int j = 0; j < 25; j += 5) {
      for(int i = 0; i < 5; i ++) {
        bc[i] = state25Elements[j + i];
      }
      for(int i = 0; i < 5; i ++) {
        state25Elements[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
      }
    }
    // Iota
    state25Elements[0] ^= keccakf_rndc[round];
  }
}

void Sha3::reset() {
  this->byteIndex = 0;
  this->capacityWords = 0;
  this->flagUseKeccak = false;
  int spongeWordsSizeInBytes = Sha3::numberOfSpongeWords * 8;
  for (int i = 0; i < spongeWordsSizeInBytes; i ++) {
    this->sb[i] = 0;
  }
  this->wordIndex = 0;
  this->saved = 0;
}

// For Init or Reset call these:
void Sha3::sha3_Init256() {
  bool flagOldUseKeccak = this->flagUseKeccak;
  this->reset();
  this->flagUseKeccak = flagOldUseKeccak;
  this->capacityWords = 2 * 256 / (8 * sizeof(uint64_t));
}

void Sha3::sha3_Init384() {
  this->reset();
  this->capacityWords = 2 * 384 / (8 * sizeof(uint64_t));
}

void Sha3::sha3_Init512() {
  this->reset();
  this->capacityWords = 2 * 512 / (8 * sizeof(uint64_t));
}

void Sha3::sha3_Update(void const* bufIn, size_t len) {
  /* 0...7 -- how much is needed to have a word */
  unsigned old_tail = (8 - this->byteIndex) & 7;
  size_t words;
  unsigned tail;
  size_t i;
  const uint8_t* buf = static_cast<const uint8_t *>(bufIn);
  if (this->byteIndex >= 8) {
    global.fatal << "Internal sha3 computation error: byteIndex too large: " << this->byteIndex << global.fatal;
  }
  if (this->wordIndex >= (sizeof(this->stateStorage) / sizeof(this->stateStorage[0]))) {
    global.fatal << "Internal sha3 computation error: wordIndex too large: " << this->wordIndex << global.fatal;
  }
  if (len < old_tail) {
    // Have no complete word or haven't started the word yet.
    // Endian-independent code follows:
    while (len --) {
      this->saved |= static_cast<uint64_t>(*(buf ++)) << ((this->byteIndex ++) * 8);
    }
    if (this->byteIndex >= 8) {
      global.fatal << "Internal sha3 computation error. " << global.fatal;
    }
    return;
  }
  if (old_tail) {
    // will have one word to process
    // endian-independent code follows:
    len -= old_tail;
    while (old_tail --) {
      this->saved |= static_cast<uint64_t>(*(buf ++)) << ((this->byteIndex ++) * 8);
    }

    // now ready to add saved to the sponge
    this->stateStorage[this->wordIndex] ^= this->saved;
    if (this->byteIndex != 8) {
      global.fatal << "Internal sha3 computation error. " << global.fatal;
    }
    this->byteIndex = 0;
    this->saved = 0;
    if (++ this->wordIndex == (Sha3::numberOfSpongeWords - this->capacityWords)) {
      this->transform(this->stateStorage);
      this->wordIndex = 0;
    }
  }
  // now work in full words directly from input
  if (this->byteIndex != 0) {
    global.fatal << "Internal sha3 computation error. " << global.fatal;
  }
  words = len / sizeof(uint64_t);
  tail = static_cast<unsigned>(len - words * sizeof(uint64_t));
  for (i = 0; i < words; i ++, buf += sizeof(uint64_t)) {
    const uint64_t t =
     static_cast<uint64_t>(buf[0]) |
    (static_cast<uint64_t>(buf[1]) << 8 * 1) |
    (static_cast<uint64_t>(buf[2]) << 8 * 2) |
    (static_cast<uint64_t>(buf[3]) << 8 * 3) |
    (static_cast<uint64_t>(buf[4]) << 8 * 4) |
    (static_cast<uint64_t>(buf[5]) << 8 * 5) |
    (static_cast<uint64_t>(buf[6]) << 8 * 6) |
    (static_cast<uint64_t>(buf[7]) << 8 * 7);
    this->stateStorage[this->wordIndex] ^= t;
    if (++ this->wordIndex == (Sha3::numberOfSpongeWords - this->capacityWords)) {
      this->transform(this->stateStorage);
      this->wordIndex = 0;
    }
  }
  // finally, save the partial word
  if (!(this->byteIndex == 0 && tail < 8)) {
    global.fatal << "Internal sha3 computation error. " << global.fatal;
  }
  while (tail --) {
    this->saved |= static_cast<uint64_t>(*(buf ++)) << ((this->byteIndex ++) * 8);
  }
  if (this->byteIndex >= 8) {
    global.fatal << "Internal sha3 computation error. " << global.fatal;
  }
}

void Sha3::finalize() {
  // The padding consists of
  // bit 1 followed by r 0 bits and ending with bit 1.
  // We assume here that all sequences are divisible in bytes,
  // therefore there are two types of padding:
  // 1) Padding whose hex looks like:
  // 01 00...00 80,
  // with zero or more 0x00 bytes sandwitched between byte 0x01 and byte 0x80.
  // 2) Padding with the single byte 0x81 = binary(10000001) = 129.

  if (!this->flagUseKeccak) {
  // Append 2-bit suffix 01, per SHA-3 spec. Instead of 1 for padding we
  // use 1 << 2 below. The 0x02 below corresponds to the suffix 01.
  // Overall, we feed 0, then 1, and finally 1 to start padding. Without
  // M || 01, we would simply use 1 to start padding.
    // SHA3 version
    this->stateStorage[this->wordIndex] ^= (this->saved ^ (static_cast<uint64_t>(static_cast<uint64_t>(0x02 | (1 << 2)) << ((this->byteIndex) * 8))));
  } else {
    // For testing the "pure" Keccak version
    this->stateStorage[this->wordIndex] ^= (this->saved ^ (static_cast<uint64_t>(static_cast<uint64_t>(1) << (this->byteIndex * 8))));
  }
  this->stateStorage[Sha3::numberOfSpongeWords - this->capacityWords - 1] ^= 0x8000000000000000UL;
  this->transform(this->stateStorage);
  // Return first bytes of the ctx->s. This conversion is not needed for
  // little-endian platforms e.g. wrap with #if !defined(__BYTE_ORDER__)
  // || !defined(__ORDER_LITTLE_ENDIAN__) || __BYTE_ORDER__!=__ORDER_LITTLE_ENDIAN__
  //    ... the conversion below ...
  //
  {
    unsigned i;
    for (i = 0; i < Sha3::numberOfSpongeWords; i ++) {
      const unsigned t1  = static_cast<uint32_t>(this->stateStorage[i]);
      const unsigned t2  = static_cast<uint32_t>((this->stateStorage[i] >> 16) >> 16);
      this->sb[i * 8 + 0] = static_cast<uint8_t>(t1);
      this->sb[i * 8 + 1] = static_cast<uint8_t>(t1 >> 8);
      this->sb[i * 8 + 2] = static_cast<uint8_t>(t1 >> 16);
      this->sb[i * 8 + 3] = static_cast<uint8_t>(t1 >> 24);
      this->sb[i * 8 + 4] = static_cast<uint8_t>(t2);
      this->sb[i * 8 + 5] = static_cast<uint8_t>(t2 >> 8);
      this->sb[i * 8 + 6] = static_cast<uint8_t>(t2 >> 16);
      this->sb[i * 8 + 7] = static_cast<uint8_t>(t2 >> 24);
    }
  }
}

void Sha3::init() {
  this->sha3_Init256();
}

void Sha3::update(const std::string& input) {
  this->update(input.c_str(), input.size());
}

void Sha3::update(List<unsigned char>& input) {
  this->update(input.TheObjects, static_cast<unsigned>(input.size));
}

void Sha3::update(const void* inputBuffer, size_t length) {
  this->sha3_Update(inputBuffer, length);
}

std::string Sha3::computeSha3_256(const std::string& input) {
  this->flagUseKeccak = false;
  this->init();
  this->update(input.c_str(), input.size());
  this->finalize();
  return this->getResultString();
}

std::string Sha3::getResultString() {
  std::string result;
  result.resize(32);
  for (unsigned i = 0; i < 32; i ++) {
    result[i] = static_cast<char>(this->sb[i]);
  }
  return result;
}

void Sha3::getResultVector(List<unsigned char>& output) {
  output.SetSize(32);
  for (int i = 0; i < 32; i ++) {
    output[i] = this->sb[i];
  }
}

std::string Crypto::computeSha3_256OutputBase64URL(const std::string& input) {
  List<unsigned char> outputList;
  Crypto::computeSha3_256(input, outputList);
  return Crypto::ConvertListUnsignedCharsToBase64(outputList, true);
}

void Crypto::computeKeccak3_256(const std::string& input, List<unsigned char>& output) {
  Sha3 theHasher;
  theHasher.flagUseKeccak = true;
  theHasher.init();
  theHasher.update(input.c_str(), input.size());
  theHasher.finalize();
  theHasher.getResultVector(output);
}

void Crypto::computeSha3_256(const std::string& input, List<unsigned char>& output) {
  Sha3 theHasher;
  theHasher.flagUseKeccak = false;
  theHasher.init();
  theHasher.update(input.c_str(), input.size());
  theHasher.finalize();
  theHasher.getResultVector(output);
}
