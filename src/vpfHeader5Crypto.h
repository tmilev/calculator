#ifndef header_crypto_was_already_defined
#define header_crypto_was_already_defined
#include "vpfHeader1General0_General.h"
static ProjectInformationInstance projectInfoCryptoHeader(__FILE__, "Crypto class declaration.");

class Certificate
{
public:


};

class Crypto
{
  //To do: make sure all crypto functions zero their buffers.
public:
  static List<Certificate> knownCertificates;
  static void LoadKnownCertificates(std::stringstream* comments);
  static void LoadOneKnownCertificate(const std::string& input, std::stringstream* comments);

  static std::string CharsToBase64String(const List<unsigned char>& input);
  static std::string CharsToBase64String(const std::string& input);
  static uint32_t GetUInt32FromCharBigendian(const List<unsigned char>& input);
  static void GetUInt32FromCharBigendianPadLastIntWithZeroes
  (const List<unsigned char>& input, List<uint32_t>& output);
  static void ConvertUint32ToUcharBigendian(const List<uint32_t>& input, List<unsigned char>& output);
  static bool StringBase64ToBitStream(const std::string& input, List<unsigned char>& output, std::stringstream* comments=0);
  static bool StringBase64ToString(const std::string& input, std::string& output, std::stringstream* comments=0);

  static bool Get6bitFromChar(char input, uint32_t& output);
  static unsigned char GetCharFrom6bit(uint32_t input);
  static void ConvertBitStreamToString(const List<unsigned char>& input, std::string& output);
  static void ConvertStringToBitStream(const std::string& input, List<unsigned char>& output);
  static void convertUint32toBigendianStringAppendResult(uint32_t& input, std::string& outputAppend);
  static void convertUint64toBigendianStringAppendResult(uint64_t& input, std::string& outputAppend);
  static uint32_t leftRotateAsIfBigEndian(uint32_t input, int numBitsToRotate);
  static void computeSha1(const std::string& inputString, List<uint32_t>& output);
  static std::string computeSha1outputBase64(const std::string& inputString);
};
#endif // header_crypto_was_already_defined
