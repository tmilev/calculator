#ifndef header_crypto_was_already_defined
#define header_crypto_was_already_defined
#include "general_lists.h"
#include "math_large_integers.h"
#include "json.h"
#include "abstract_syntax_notation_one_decoder.h"

static ProjectInformationInstance projectInfoCryptoHeader(__FILE__, "Crypto class declaration.");

class CertificateRSA {
public:
  std::string algorithm;
  std::string keyid;
  std::string theModulusString;
  std::string theExponentString;
  LargeIntUnsigned theModuluS;
  LargeIntUnsigned theExponenT;
  bool LoadFromJSON(JSData& input, std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  bool LoadFromModulusAndExponentStrings(std::stringstream* commentsOnFailure);
  std::string ToString();
};

class PrivateKeyRSA {
public:
  LargeIntUnsigned primeOne;
  LargeIntUnsigned primeTwo;
  LargeIntUnsigned modulus;
  LargeIntUnsigned exponent;
  LargeIntUnsigned privateExponent;
  LargeIntUnsigned coefficient;
  LargeIntUnsigned exponentOne;
  LargeIntUnsigned exponentTwo;
  CertificateRSA theCertificate;
  List<unsigned char> sourceBinary;
  ASNElement sourceASNOuter;
  ASNElement sourceASNInner;
  class Test {
    public:
    static bool All();
    static bool LoadFromPEMFile();
    static bool LoadFromPEM();
  };
  bool BasicChecks(std::stringstream* comments);
  bool LoadFromPEMFile(const std::string& input, std::stringstream* commentsOnFailure);
  bool LoadFromPEM(const std::string& input, std::stringstream* commentsOnFailure);
  bool LoadFromASNEncoded(List<unsigned char>& input, std::stringstream* commentsOnFailure);
  std::string ToString() const;
};

class TBSCertificateInfo {
public:
  unsigned int version;
  LargeIntUnsigned serialNumber;
  CertificateRSA theRSA;
  ASNElement signatureAlgorithmIdentifier;
  ASNObject countryName;
  ASNObject stateOrProvinceName;
  ASNObject localityName;
  ASNObject organizationName;
  ASNObject organizationalUnitName;
  ASNObject commonName;
  ASNObject emailAddress;
  ASNElement validityNotBefore;
  ASNElement validityNotAfter;
  bool LoadValidityFromASN(
    const ASNElement& input,
    std::stringstream* commentsOnFailure
  );
  bool LoadFieldsFromASN(
    const ASNElement &input,
    std::stringstream* commentsOnFailure
  );
  bool LoadFields(
    const MapList<std::string, ASNObject, MathRoutines::HashString>& fields,
    std::stringstream* commentsOnFailure
  );
  bool LoadFromASNEncoded(const ASNElement& input, std::stringstream* commentsOnFailure);
  void WriteBytesContentAndExpiry(List<unsigned char>& output);
  void WriteBytesContent(List<unsigned char>& output);
  void WriteBytesExpiry(List<unsigned char>& output);
  std::string ToString();
  void ComputeASN(ASNElement& output);
  void ComputeASNVersionWrapper(ASNElement& output);
};

// For definition, google RFC 5280
class X509Certificate {
public:
  ASNElement signatureAlgorithmId;
  TBSCertificateInfo information;
  ASNElement signatureValue;
  List<unsigned char> sourceBinary;
  ASNElement sourceASN;
  ASNElement recodedASN;
  class Test {
  public:
    static bool All();
    static bool LoadFromPEMFile();
    static bool LoadFromPEM();
  };
  bool LoadFromPEMFile(const std::string& input, std::stringstream* commentsOnFailure);
  bool LoadFromPEM(const std::string& input, std::stringstream* commentsOnFailure);
  bool LoadFromASNEncoded(
    const List<unsigned char>& input,
    std::stringstream* commentsOnFailure
  );
  bool LoadFromJSON(
    JSData& input,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  std::string ToString();
  std::string ToStringTestEncode();
  void ComputeASN(ASNElement& output);
  void ComputeASNSignatureAlgorithm(ASNElement& output);
  void WriteBytesASN1(List<unsigned char>& output);
};

class Crypto {
  //To do: make sure all crypto functions zero their buffers.
public:

  static const int RIPEMD160LengthInBytes = 20;
  static const int LengthSha3DefaultInBytes = 32;
  static bool flagRIPEMDBigEndian;

  static List<CertificateRSA> knownCertificates;
  static List<uint32_t> kArraySha2xx;
  class Test {
  public:
    static bool Sha256();
  };
  static bool LoadKnownCertificates(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  static bool LoadOneKnownCertificate(
    const std::string& input, std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
  );

  static std::string ConvertStringToBase64(const List<unsigned char>& input, bool useBase64URL);
  static std::string ConvertStringToBase64Standard(const std::string& input);
  static std::string ConvertStringToBase64URL(const std::string& input);
  static uint32_t GetUInt32FromCharBigendian(const List<unsigned char>& input);
  static void ConvertStringToListUInt32BigendianZeroPad(
    const List<unsigned char>& input, List<uint32_t>& output
  );
  static void ConvertStringToListUInt32BigendianZeroPad(const std::string& input, List<uint32_t>& output);
  static void ConvertUint32ToUcharBigendian(const List<uint32_t>& input, List<unsigned char>& output);
  static void ConvertUint32ToString(const List<uint32_t>& input, std::string& output);
  static bool ConvertBase64ToBitStream(
    const std::string& input,
    List<unsigned char>& output,
    std::stringstream* commentsOnFailure = nullptr,
    std::stringstream* commentsGeneral = nullptr
  );
  static bool ConvertBase64ToString(
    const std::string& input,
    std::string& output,
    std::stringstream* commentsOnFailure = nullptr,
    std::stringstream* commentsGeneral = nullptr
  );

  static void ConvertStringToLargeIntUnsigned(const std::string& input, LargeIntUnsigned& output);
  static void ConvertLargeIntUnsignedToBase58SignificantDigitsLAST(const LargeIntUnsigned& input, std::string& output);
  static void ConvertLargeIntUnsignedToBase58SignificantDigitsFIRST(
    const LargeIntUnsigned& input, std::string& output, int numberOfOnesToPrepend
  );
  static bool ConvertBase58SignificantDigitsFIRSTToLargeIntUnsigned(
    const std::string& inputSignificantDigitsFirst,
    LargeIntUnsigned& output,
    int &numberOfLeadingZeroes,
    std::stringstream *commentsOnFailure
  );
  static bool ConvertBase58ToHexSignificantDigitsFirst(
    const std::string& input, std::string& output, std::stringstream* commentsOnFailure
  );
  static std::string ConvertIntToHex(int input, int significantBytes);
  static std::string ConvertUintToHex(unsigned int input, int significantBytes);
  static bool ConvertHexToString(const std::string& input, std::string& output, std::stringstream* commentsOnFailure);
  static bool ConvertHexToListUnsignedChar(const std::string& input, List<unsigned char>& output, std::stringstream* commentsOnFailure);
  static bool ConvertHexToInteger(const std::string& input, LargeIntUnsigned &output, int &outputNumLeadingZeroPairs);
  static std::string ConvertListUnsignedCharsToHex(
    const List<unsigned char>& input, int byteWidthLineBreakZeroForNone, bool useHtml
  );
  static bool ConvertListUnsignedCharsToHex(
    const List<unsigned char>& input, std::string& output, int byteWidthLineBreakZeroForNone, bool useHtml
  );
  static std::string ConvertListCharsToHex(
    const List<char>& input, int byteWidthLineBreakZeroForNone, bool useHtml
  );
  static bool ConvertListCharsToHex(
    const List<char>& input, std::string& output, int byteWidthLineBreakZeroForNone, bool useHtml
  );
  static bool ConvertStringToHex(
    const std::string& input, std::string& output, int byteWidthLineBreakZeroForNone, bool useHtml
  );
  static void AppendDoubleSha256Check(const std::string& input, std::string& output);
  static std::string ConvertStringToHex(const std::string& input, int byteWidthLineBreakZeroForNone, bool useHtml);
  static bool GetBase58FromChar(char input, uint32_t& output);
  static bool GetCharFromBase58(uint32_t input, char& output);
  static bool Get6bitFromChar(unsigned char input, uint32_t& output);
  static char GetCharFrom6bit(uint32_t input, bool useBase64URL);
  static void ConvertBitStreamToString(const List<unsigned char>& input, std::string& output);
  static void ConvertStringToListBytes(const std::string& input, List<unsigned char>& output);
  static void ConvertStringToListBytesSigned(const std::string& input, List<char>& output);
  static void ConvertUint32toBigendianStringAppendResult(uint32_t& input, std::string& outputAppend);
  static void ConvertUint64toBigendianStringAppendResult(uint64_t input, std::string& outputAppend);
  static void ConvertUint64toBigendianListUnsignedCharAppendResult(uint64_t input, List<unsigned char>& outputAppend);
  static uint32_t leftRotateAsIfBigEndian(uint32_t input, int numBitsToRotate);
  static uint32_t rightRotateAsIfBigEndian(uint32_t input, int numBitsToRotate);
  static uint32_t rightShiftAsIfBigEndian(uint32_t input, int numBitsToShift);
  static uint32_t leftShiftAsIfBigEndian(uint32_t input, int numBitsToShift);

  static void initSha256();
  static void computeSha1(const std::string& inputString, List<uint32_t>& output);
  static void computeSha224(const List<unsigned char>& input, List<uint32_t>& output);
  static void computeSha2xx(const List<unsigned char>& input, List<uint32_t>& output, bool is224);
  static void computeSha256(const List<unsigned char>& input, List<uint32_t>& output);
  static void computeSha256(const List<unsigned char>& input, List<unsigned char>& output);
  static void computeSha256(const std::string& input, std::string& output);
  static void ConvertListUintToLargeUInt(List<uint32_t>& input, LargeIntUnsigned& output);
  static LargeIntUnsigned RSAencrypt(const LargeIntUnsigned& theModulus, const LargeInt& theExponent, const LargeInt& theMessage);
  static void ConvertBitStreamToLargeUnsignedInt(const List<unsigned char>& input, LargeIntUnsigned& output);
  static bool ConvertBase64ToLargeUnsignedInt(
    const std::string& inputBase64, LargeIntUnsigned& output, std::stringstream* commentsOnFailure
  );
  static bool ConvertLargeUnsignedIntToBase64SignificantDigitsFirst(const LargeIntUnsigned& input, std::string& outputBase64);
  static bool ConvertLargeUnsignedIntToHexSignificantDigitsFirst(
    const LargeIntUnsigned& input, int numberOfLeadingZeroesToPadWith, std::string& outputHex
  );
  static bool ConvertLargeUnsignedIntToStringSignificantDigitsLast(const LargeIntUnsigned& input, std::string& output);
  static bool ConvertLargeUnsignedIntToStringSignificantDigitsFirst(
    const LargeIntUnsigned& input, int numberOfLeadingZeroesToPadWith, std::string& output
  );
  static bool VerifyJWTagainstKnownKeys(
    const std::string& inputToken, std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
  );
  static void computeRIPEMD160(const std::string& input, List<unsigned char>& output);
  static std::string computeSha3_256OutputBase64URL(const std::string& input);
  static void computeSha3_256(const std::string& input, List<unsigned char>& output);
  static void computeKeccak3_256(const std::string& input, List<unsigned char>& output);
  static bool encryptAES_CBC_256(
    const std::string& inputKey,
    const std::string& inputPlainText,
    List<unsigned char>& output,
    std::stringstream *commentsOnFailure
  );
  static bool encryptAES_CBC_256(
    const std::string& inputKey,
    const std::string& inputPlainText,
    std::string& output,
    std::stringstream *commentsOnFailure
  );
  static bool decryptAES_CBC_256(
    const std::string& inputKey,
    const std::string& inputCipherText,
    List<unsigned char>& output,
    std::stringstream* commentsOnFailure
  );
  static bool decryptAES_CBC_256(
    const std::string& inputKey,
    const std::string& inputCipherText,
    std::string& output,
    std::stringstream* commentsOnFailure
  );
  static void GetRandomBytesSecure(List<unsigned char>& output, int numBytes);
};

class JSONWebToken {
public:
  std::string signatureScheme;
  std::string headerBase64;
  std::string claimsBase64;
  std::string signatureBase64;
  std::string headerJSON;
  std::string claimsJSON;
  MapList<std::string, std::string, MathRoutines::HashString> payloadKeys;

  bool AssignString(const std::string& other, std::stringstream* commentsOnFailure);
  std::string ToString();
  bool VerifyRSA256(
    const LargeIntUnsigned& theModulus,
    const LargeIntUnsigned& theExponent,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
};
#endif // header_crypto_was_already_defined