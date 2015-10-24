#ifndef header_crypto_was_already_defined
#define header_crypto_was_already_defined
#include "vpfHeader1General0_General.h"
static ProjectInformationInstance projectInfoCryptoHeader(__FILE__, "Crypto class declaration.");

class Crypto
{
public:
  static std::string CharsToBase64String(const List<unsigned char>& input);
  static uint32_t GetUInt32FromCharBigendian(const List<unsigned char>& input);
  static void GetUInt32FromCharBigendianPadLastIntWithZeroes
  (const List<unsigned char>& input, List<uint32_t>& output);
  static void ConvertUint32ToUcharBigendian(const List<uint32_t>& input, List<unsigned char>& output);
  static bool StringBase64ToBitStream(const std::string& input, List<unsigned char>& output, std::stringstream* comments=0);

  static bool Get6bitFromChar(char input, uint32_t& output);
  static unsigned char GetCharFrom6bit(uint32_t input);
  static void ConvertBitStreamToString(const List<unsigned char>& input, std::string& output);
  static void ConvertStringToBitStream(const std::string& input, List<unsigned char>& output);
  static void convertUint32toBigendianStringAppendResult(uint32_t& input, std::string& outputAppend);
  static void convertUint64toBigendianStringAppendResult(uint64_t& input, std::string& outputAppend);
  static uint32_t leftRotateAsIfBigEndian(uint32_t input, int numBitsToRotate);
  static void computeSha1(const std::string& inputString, List<uint32_t>& output);
};
#endif // header_crypto_was_already_defined
