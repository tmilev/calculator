#ifndef header_crypto_was_already_defined
#define header_crypto_was_already_defined
#include "vpfHeader1General0_General.h"
#include "vpfJson.h"
static ProjectInformationInstance projectInfoCryptoHeader(__FILE__, "Crypto class declaration.");

class Certificate
{
public:
  std::string algorithm;
  std::string keyid;
  std::string theModulus;
  std::string theExponent;
  bool LoadFromJSON(JSData& input, std::stringstream* comments);
  std::string ToString();
};

class Crypto
{
  //To do: make sure all crypto functions zero their buffers.
public:
  static List<Certificate> knownCertificates;
  static List<uint32_t> kArraySha2xx;
  static bool LoadKnownCertificates(std::stringstream* comments);
  static bool LoadOneKnownCertificate(const std::string& input, std::stringstream* comments);

  static std::string CharsToBase64String(const List<unsigned char>& input);
  static std::string CharsToBase64String(const std::string& input);
  static uint32_t GetUInt32FromCharBigendian(const List<unsigned char>& input);
  static void GetUInt32FromCharBigendianPadLastIntWithZeroes
  (const List<unsigned char>& input, List<uint32_t>& output);
  static void ConvertUint32ToUcharBigendian(const List<uint32_t>& input, List<unsigned char>& output);
  static bool ConvertStringBase64ToBitStream(const std::string& input, List<unsigned char>& output, std::stringstream* comments=0);
  static bool ConvertStringBase64ToString(const std::string& input, std::string& output, std::stringstream* comments=0);

  static bool ConvertHexToInteger(const std::string& input, LargeInt& output);
  static bool ConvertBitStreamToHexString(const std::string& input, std::string& output);
  static bool Get6bitFromChar(char input, uint32_t& output);
  static unsigned char GetCharFrom6bit(uint32_t input);
  static void ConvertBitStreamToString(const List<unsigned char>& input, std::string& output);
  static void ConvertStringToListBytes(const std::string& input, List<unsigned char>& output);
  static void ConvertUint32toBigendianStringAppendResult(uint32_t& input, std::string& outputAppend);
  static void ConvertUint64toBigendianStringAppendResult(uint64_t& input, std::string& outputAppend);
  static uint32_t leftRotateAsIfBigEndian(uint32_t input, int numBitsToRotate);
  static uint32_t rightRotateAsIfBigEndian(uint32_t input, int numBitsToRotate);
  static uint32_t rightShiftAsIfBigEndian(uint32_t input, int numBitsToShift);
  static uint32_t leftShiftAsIfBigEndian(uint32_t input, int numBitsToShift);

  static void initSha256();
  static void computeSha1(const std::string& inputString, List<uint32_t>& output);
  static void computeSha224(const std::string& inputString, List<uint32_t>& output);
  static void computeSha2xx(const std::string& inputString, List<uint32_t>& output, bool is224);
  static void computeSha256(const std::string& inputString, List<uint32_t>& output);
  static std::string computeSha1outputBase64(const std::string& inputString);
};
#endif // header_crypto_was_already_defined
