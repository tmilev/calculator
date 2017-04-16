#ifndef header_crypto_was_already_defined
#define header_crypto_was_already_defined
#include "vpfHeader1General0_General.h"
#include "vpfHeader2Math01_LargeIntArithmetic.h"
#include "vpfJson.h"
static ProjectInformationInstance projectInfoCryptoHeader(__FILE__, "Crypto class declaration.");

class Certificate
{
public:
  std::string algorithm;
  std::string keyid;
  std::string theModulusString;
  std::string theExponentString;
  LargeIntUnsigned theModuluS;
  LargeIntUnsigned theExponenT;
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

  static std::string ConvertStringToBase64(const List<unsigned char>& input);
  static std::string ConvertStringToBase64(const std::string& input);
  static uint32_t GetUInt32FromCharBigendian(const List<unsigned char>& input);
  static void ConvertStringToListUInt32BigendianZeroPad
  (const List<unsigned char>& input, List<uint32_t>& output);
  static void ConvertStringToListUInt32BigendianZeroPad
  (const std::string& input, List<uint32_t>& output);
  static void ConvertUint32ToUcharBigendian(const List<uint32_t>& input, List<unsigned char>& output);
  static bool ConvertBase64ToBitStream(const std::string& input, List<unsigned char>& output, std::stringstream* comments=0);
  static bool ConvertBase64ToString(const std::string& input, std::string& output, std::stringstream* comments=0);

  static void ConvertStringToLargeIntUnsigned(const std::string& input, LargeIntUnsigned& output);
  static bool ConvertHexToString(const std::string& input, std::string& output);
  static bool ConvertHexToInteger(const std::string& input, LargeInt& output);
  static bool ConvertStringToHex(const std::string& input, std::string& output);
  static std::string ConvertStringToHex(const std::string& input);
  static bool Get6bitFromChar(unsigned char input, uint32_t& output);
  static unsigned char GetCharFrom6bit(uint32_t input);
  static void ConvertBitStreamToString(const List<unsigned char>& input, std::string& output);
  static void ConvertStringToListBytes(const std::string& input, List<unsigned char>& output);
  static void ConvertStringToListBytesSigned(const std::string& input, List<char>& output);
  static void ConvertUint32toBigendianStringAppendResult(uint32_t& input, std::string& outputAppend);
  static void ConvertUint64toBigendianStringAppendResult(uint64_t& input, std::string& outputAppend);
  static List<int> ConvertStringToListInt(const std::string& input);
  static uint32_t leftRotateAsIfBigEndian(uint32_t input, int numBitsToRotate);
  static uint32_t rightRotateAsIfBigEndian(uint32_t input, int numBitsToRotate);
  static uint32_t rightShiftAsIfBigEndian(uint32_t input, int numBitsToShift);
  static uint32_t leftShiftAsIfBigEndian(uint32_t input, int numBitsToShift);

  static void initSha256();
  static void computeSha1(const std::string& inputString, List<uint32_t>& output);
  static void computeSha224(const std::string& inputString, List<uint32_t>& output);
  static void computeSha2xx(const std::string& inputString, List<uint32_t>& output, bool is224);
  static void computeSha256(const std::string& inputString, List<uint32_t>& output);
  static void ConvertListUintToLargeUInt(List<uint32_t>& input, LargeIntUnsigned& output);
  static LargeIntUnsigned RSAencrypt(const LargeIntUnsigned& theModulus, const LargeInt& theExponent, const LargeInt& theMessage);
  static void ConvertBitStreamToLargeUnsignedInt
  (const List<unsigned char>& input, LargeIntUnsigned& output);
  static bool ConvertBase64ToLargeUnsignedInt
  (const std::string& inputBase64, LargeIntUnsigned& output, std::stringstream* comments);
  static bool ConvertLargeUnsignedIntToBase64
  (const LargeIntUnsigned& input, std::string& outputBase64);
  static bool ConvertLargeUnsignedIntToHex
  (const LargeIntUnsigned& input, std::string& outputHex);
  static bool ConvertLargeUnsignedIntToString
  (const LargeIntUnsigned& input, std::string& output);
  static std::string computeSha1outputBase64(const std::string& inputString);
  static bool VerifyJWTagainstKnownKeys
  (const std::string& inputToken, std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
};

class JSONWebToken
{
public:
  std::string signatureScheme;
  std::string headerBase64;
  std::string claimsBase64;
  std::string signatureBase64;
  std::string headerJSON;
  std::string claimsJSON;
  MapLisT<std::string, std::string, MathRoutines::hashString> payloadKeys;

  bool AssignString(const std::string& other, std::stringstream* commentsOnFailure);
  std::string ToString();
  bool VerifyRSA256
  (const LargeIntUnsigned& theModulus, const LargeIntUnsigned& theExponent,
   std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
  );
};
#endif // header_crypto_was_already_defined
