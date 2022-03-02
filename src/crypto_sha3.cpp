#include "crypto.h"

// Reference:
// Wikipedia's page on SHA3:
// https://en.wikipedia.org/wiki/SHA-3
class Sha3 {
public:
  static const int numberOfSpongeWords = 25;
  static const int numberOfKeccakRounds = 24;
  static const unsigned triangularNumbers[24];
  static const uint64_t roundEndXORConstants[24];
  static const signed powersOfA[24];
  // The portion of the input message that we didn't consume yet.
  bool flagUseKeccak;
  uint64_t saved;
  union {
    // Keccak's state
    uint64_t stateStorage[Sha3::numberOfSpongeWords];
    uint8_t stateBytes[Sha3::numberOfSpongeWords * 8];
  };

  // 0..7--the next byte after the set one (starts from 0; 0--none are buffered)
  int byteIndex;
  // 0..24--the next word to integrate input (starts from 0)
  signed wordIndex;
  // The double size of the hash output in words (e.g. 16 for Keccak 512)
  signed capacityWords;
  std::string computeSha3_256(const std::string& input);
  void getResultVector(List<unsigned char>& output);
  std::string getResultString();
  void initialize();
  void update(const std::string& input);
  void update(List<unsigned char>& input);
  void finalize();
  Sha3();
  void transform();
  void transformIfFilledUp();
  void theta();
  void piOfRho();
  void chi();
  void reset();
  void sha3_Init256();
  void sha3_Init384();
  void sha3_Init512();
  std::string toStringState();
  static uint64_t rotateLeft(uint64_t x, unsigned y) {
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
const uint64_t Sha3::roundEndXORConstants[24] =
{
  0x0000000000000001UL,
  0x0000000000008082UL,
  0x800000000000808aUL,
  0x8000000080008000UL,
  0x000000000000808bUL,
  0x0000000080000001UL,
  0x8000000080008081UL,
  0x8000000000008009UL,
  0x000000000000008aUL,
  0x0000000000000088UL,
  0x0000000080008009UL,
  0x000000008000000aUL,
  0x000000008000808bUL,
  0x800000000000008bUL,
  0x8000000000008089UL,
  0x8000000000008003UL,
  0x8000000000008002UL,
  0x8000000000000080UL,
  0x000000000000800aUL,
  0x800000008000000aUL,
  0x8000000080008081UL,
  0x8000000000008080UL,
  0x0000000080000001UL,
  0x8000000080008008UL
};
const unsigned Sha3::triangularNumbers[24] =
{
  1,
  3,
  6,
  10,
  15,
  21,
  28,
  36,
  45,
  55,
  2,
  14,
  27,
  41,
  56,
  8,
  25,
  43,
  62,
  18,
  39,
  61,
  20,
  44
};
// Let A be the matrix:
// ( 3 2 )
// ( 1 0 )
// and let v be the vector-column:
// ( 0 )
// ( 1 ).
// Then powersOfA[i] equals 5w_0 + w_1 where
// w_0, w_1 are the entries of the vector-column w =
// ( w_0 )
// ( w_1 )
// computed by w = A^{i+1} * v.
const signed Sha3::powersOfA[24] =
{
  10,
  7,
  11,
  17,
  18,
  3,
  5,
  16,
  8,
  21,
  24,
  4,
  15,
  23,
  19,
  13,
  12,
  2,
  20,
  14,
  22,
  9,
  6,
  1
};
void Sha3::theta() {
  uint64_t columnXORed[5];
  for (int i = 0; i < 5; i ++) {
    columnXORed[i] = this->stateStorage[i] xor this->stateStorage[i + 5] xor
    this->stateStorage[i + 10] xor
    this->stateStorage[i + 15] xor
    this->stateStorage[i + 20];
  }
  for (int i = 0; i < 5; i ++) {
    uint64_t t = columnXORed[(i + 4) % 5] xor
    Sha3::rotateLeft(columnXORed[(i + 1) % 5], 1);
    for (int j = 0; j < 25; j += 5) {
      this->stateStorage[j + i] ^= t;
    }
  }
}

void Sha3::piOfRho() {
  uint64_t x, y;
  x = this->stateStorage[1];
  for (int i = 0; i < 24; i ++) {
    int j = Sha3::powersOfA[i];
    y = this->stateStorage[j];
    this->stateStorage[j] =
    Sha3::rotateLeft(x, Sha3::triangularNumbers[i]);
    x = y;
  }
}

void Sha3::chi() {
  uint64_t combined[5];
  for (int j = 0; j < 25; j += 5) {
    for (int i = 0; i < 5; i ++) {
      combined[i] = this->stateStorage[j + i];
    }
    for (int i = 0; i < 5; i ++) {
      this->stateStorage[j + i] ^= (compl combined[(i + 1) % 5]) &
      combined[(i + 2) % 5];
    }
  }
}

void Sha3::transform() {
  for (int round = 0; round < Sha3::numberOfKeccakRounds; round ++) {
    this->theta();
    this->piOfRho();
    this->chi();
    // Iota
    this->stateStorage[0] ^= Sha3::roundEndXORConstants[round];
  }
}

void Sha3::reset() {
  this->byteIndex = 0;
  this->capacityWords = 0;
  this->flagUseKeccak = false;
  int spongeWordsSizeInBytes = Sha3::numberOfSpongeWords * 8;
  for (int i = 0; i < spongeWordsSizeInBytes; i ++) {
    this->stateBytes[i] = 0;
  }
  this->wordIndex = 0;
  this->saved = 0;
}

// For Init or reset call these:
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

void Sha3::transformIfFilledUp() {
  if (this->byteIndex < 8) {
    return;
  }
  this->byteIndex = 0;
  this->stateStorage[this->wordIndex] ^= this->saved;
  this->saved = 0;
  this->wordIndex ++;
  if (this->wordIndex < Sha3::numberOfSpongeWords - this->capacityWords) {
    return;
  }
  this->wordIndex = 0;
  this->transform();
}

std::string Sha3::toStringState() {
  std::stringstream out;
  out << "SHA3 state:\n";
  int intermediate = Sha3::numberOfSpongeWords - this->capacityWords;
  for (int i = 0; i < Sha3::numberOfSpongeWords; i ++) {
    if (i == intermediate) {
      out << "----------------\n";
    }
    out << Crypto::convertUint64ToHex(this->stateStorage[i]) << "\n";
  }
  return out.str();
}

void Sha3::update(List<unsigned char>& input) {
  for (int i = 0; i < input.size; i ++) {
    int bitsToPad = this->byteIndex * 8;
    this->saved += (static_cast<uint64_t>(input[i]) << bitsToPad);
    this->byteIndex ++;
    this->transformIfFilledUp();
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
    this->stateStorage[this->wordIndex] ^= (
      this->saved ^ (
        static_cast<uint64_t>(
          static_cast<uint64_t>(0x02 | (1 << 2)) << ((this->byteIndex) * 8)
        )
      )
    );
  } else {
    // Original Keccak.
    this->stateStorage[this->wordIndex] ^= (
      this->saved ^ (
        static_cast<uint64_t>(
          static_cast<uint64_t>(1) << (this->byteIndex * 8)
        )
      )
    );
  }
  this->stateStorage[Sha3::numberOfSpongeWords - this->capacityWords - 1] ^=
  0x8000000000000000UL;
  this->transform();
  // Return first bytes. This conversion is not needed for
  // little-endian platforms.
  {
    unsigned i;
    for (i = 0; i < Sha3::numberOfSpongeWords; i ++) {
      const unsigned t1 = static_cast<uint32_t>(this->stateStorage[i]);
      const unsigned t2 = static_cast<uint32_t>((
          this->stateStorage[i] >> 16
        ) >>
        16
      );
      this->stateBytes[i * 8 + 0] = static_cast<uint8_t>(t1);
      this->stateBytes[i * 8 + 1] = static_cast<uint8_t>(t1 >> 8);
      this->stateBytes[i * 8 + 2] = static_cast<uint8_t>(t1 >> 16);
      this->stateBytes[i * 8 + 3] = static_cast<uint8_t>(t1 >> 24);
      this->stateBytes[i * 8 + 4] = static_cast<uint8_t>(t2);
      this->stateBytes[i * 8 + 5] = static_cast<uint8_t>(t2 >> 8);
      this->stateBytes[i * 8 + 6] = static_cast<uint8_t>(t2 >> 16);
      this->stateBytes[i * 8 + 7] = static_cast<uint8_t>(t2 >> 24);
    }
  }
}

void Sha3::initialize() {
  this->sha3_Init256();
}

void Sha3::update(const std::string& input) {
  List<unsigned char> inputChar;
  inputChar = input;
  this->update(inputChar);
}

std::string Sha3::computeSha3_256(const std::string& input) {
  this->flagUseKeccak = false;
  this->initialize();
  this->update(input);
  this->finalize();
  return this->getResultString();
}

std::string Sha3::getResultString() {
  std::string result;
  result.resize(32);
  for (unsigned i = 0; i < 32; i ++) {
    result[i] = static_cast<char>(this->stateBytes[i]);
  }
  return result;
}

void Sha3::getResultVector(List<unsigned char>& output) {
  output.setSize(32);
  for (int i = 0; i < 32; i ++) {
    output[i] = this->stateBytes[i];
  }
}

std::string Crypto::computeSha3_256OutputBase64URL(const std::string& input) {
  List<unsigned char> outputList;
  Crypto::computeSha3_256(input, outputList);
  return Crypto::convertListUnsignedCharsToBase64(outputList, true);
}

void Crypto::computeKeccak3_256(
  const std::string& input, List<unsigned char>& output
) {
  Sha3 hasher;
  hasher.flagUseKeccak = true;
  hasher.initialize();
  hasher.update(input);
  hasher.finalize();
  hasher.getResultVector(output);
}

void Crypto::computeSha3_256(
  const std::string& input, List<unsigned char>& output
) {
  Sha3 hasher;
  hasher.flagUseKeccak = false;
  hasher.initialize();
  hasher.update(input);
  hasher.finalize();
  hasher.getResultVector(output);
}
