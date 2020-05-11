// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_crypto_was_already_defined
#define header_crypto_was_already_defined
#include "general_lists.h"
#include "general_logging_global_variables.h"
#include "math_large_integers.h"
#include "json.h"
#include "abstract_syntax_notation_one_decoder.h"

class PublicKeyRSA {
public:
  static const unsigned int defaultExponent = 65537;
  std::string algorithm;
  std::string keyid;
  std::string theModulusString;
  std::string theExponentString;
  LargeIntegerUnsigned theModulus;
  LargeIntegerUnsigned theExponent;
  bool loadFromJSON(
    JSData& input,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  bool loadFromModulusAndExponentStrings(std::stringstream* commentsOnFailure);
  std::string toString();
};

// Reference:
// https://en.wikipedia.org/wiki/RSA_(cryptosystem)
class PrivateKeyRSA {
public:
  LargeIntegerUnsigned primeOne;
  LargeIntegerUnsigned primeTwo;
  LargeIntegerUnsigned carmichaelTotientOfModulus;
  LargeIntegerUnsigned privateExponent;
  PublicKeyRSA thePublicKey;
  int bitSize; // = ceiling(log_2 (pimeOne * primeTwo))
  int byteSize; // = ceiling (bitSize / 8)
  List<unsigned char> sourceBinary;
  ASNElement sourceASNOuter;
  ASNElement sourceASNInner;
  PrivateKeyRSA();
  void computeBitSize();
  class Test {
    public:
    static bool all();
    static bool loadFromPEMFile();
    static bool loadFromPEM();
  };
  bool basicChecks(std::stringstream* comments);
  bool loadFromPEMFile(const std::string& input, std::stringstream* commentsOnFailure);
  bool loadFromPEM(const std::string& input, std::stringstream* commentsOnFailure);
  bool loadFromASNEncoded(List<unsigned char>& input, std::stringstream* commentsOnFailure);
  bool generateRandom(std::stringstream* commentsOnFailure, int numberOfBytes);
  bool computeFromTwoPrimes(
    LargeIntegerUnsigned& inputPrimeOne,
    LargeIntegerUnsigned& inputPrimeTwo,
    bool verifyInputsArePrime,
    std::stringstream* commentsOnFailure
  );
  void signBytesPadPKCS1(List<unsigned char>& input, int hash, List<unsigned char>& output);
  void hashAndPadPKCS1(List<unsigned char>& input, int hash, List<unsigned char>& output);
  std::string toString() const;
};

class TBSCertificateInfo {
public:
  class Organization {
  public:
    ASNObject countryName;
    ASNObject stateOrProvinceName;
    ASNObject localityName;
    ASNObject organizationName;
    ASNObject organizationalUnitName;
    ASNObject commonName;
    ASNObject emailAddress;
    void computeASN(ASNElement& output);
    std::string toString();
    bool loadFields(
      const MapList<std::string, ASNObject, MathRoutines::hashString>& fields,
      std::stringstream* commentsOnFailure
    );
    bool loadFromASN(
      const ASNElement &input,
      std::stringstream* commentsOnFailure
    );
  };
  unsigned int version;
  LargeIntegerUnsigned serialNumber;

  // Public key advertised by the certificate:
  // signature type:
  ASNElement subjectPublicKeyAlgorithmId;
  // public key bytes:
  ASNElement subjectPublicKeyASN;
  PublicKeyRSA subjectPublicKey;
  // The authority that signed the certificate,
  // usually a large institution with a
  // well-known public key:
  TBSCertificateInfo::Organization issuer;

  // The holder of the public key
  // advertised by this certificate:
  TBSCertificateInfo::Organization subject;
  ASNElement signatureAlgorithmIdentifier;
  ASNElement signature;
  ASNElement validityNotBefore;
  ASNElement validityNotAfter;
  List<ASNElement> extensions;
  bool loadValidity(
    const ASNElement& input,
    std::stringstream* commentsOnFailure
  );
  bool load(const ASNElement& input, std::stringstream* commentsOnFailure);
  bool loadExtensions(
    const ASNElement& input,
    std::stringstream* commentsOnFailure
  );
  static bool loadASNAlgorithmIdentifier(
    const ASNElement& input,
    ASNElement& output,
    std::stringstream* commentsOnFailure
  );
  std::string toString();
  void computeASN(ASNElement& output);
  void computeASNVersionWrapper(ASNElement& output);
  void computeASNValidityWrapper(ASNElement& output);
  void computeASNSignatureAlgorithmIdentifier(ASNElement& output);
  void computeASNSignature(ASNElement& output);
  void computeASNExtensions(ASNElement& output);
};

// For definition, google RFC 5280
// https://lapo.it/asn1js [decodes messages and highlights and annotates individual bytes.]
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
    static bool all();
    static bool loadFromPEMFile();
    static bool loadFromPEM();
  };
  bool loadFromPEMFile(const std::string& input, std::stringstream* commentsOnFailure);
  bool loadFromPEM(const std::string& input, std::stringstream* commentsOnFailure);
  bool loadFromASNEncoded(
    const List<unsigned char>& input,
    std::stringstream* commentsOnFailure
  );
  bool loadFromJSON(
    JSData& input,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  std::string toString();
  std::string toHex();
  std::string toStringTestEncode();
  void computeASN(ASNElement& output);
  void computeASNSignatureAlgorithm(ASNElement& output);
  void writeBytesASN1(List<unsigned char>& output, List<serialization::Marker>* annotations);
};

class Crypto {
  //To do: make sure all crypto functions zero their buffers.
public:

  static const int RIPEMD160LengthInBytes = 20;
  static const int LengthSha3DefaultInBytes = 32;
  static bool flagRIPEMDBigEndian;

  static List<PublicKeyRSA> knownCertificates;
  static List<uint32_t> kArraySha2xx;
  static List<uint64_t> kArraySha512;
  class Test {
  public:
    static bool sha256();
    static bool all();
  };
  static bool loadKnownCertificates(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  static bool loadOneKnownCertificate(
    const std::string& input,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  static std::string convertStringToBase64Standard(const std::string& input);
  static std::string convertStringToBase64URL(const std::string& input);
  static uint32_t getUInt32FromCharBigendian(const List<unsigned char>& input);
  static void convertStringToListUInt32BigendianZeroPad(
    const List<unsigned char>& input, List<uint32_t>& output
  );
  static void convertStringToListUInt32BigendianZeroPad(const std::string& input, List<uint32_t>& output);
  static void convertUint32ToUcharBigendian(const List<uint32_t>& input, List<unsigned char>& output);
  static void convertUint32ToString(const List<uint32_t>& input, std::string& output);
  static bool convertBase64ToBitStream(
    const std::string& input,
    List<unsigned char>& output,
    std::stringstream* commentsOnFailure = nullptr,
    std::stringstream* commentsGeneral = nullptr
  );
  static bool convertBase64ToString(
    const std::string& input,
    std::string& output,
    std::stringstream* commentsOnFailure = nullptr,
    std::stringstream* commentsGeneral = nullptr
  );

  static void convertStringToLargeIntUnsigned(
    const std::string& input, LargeIntegerUnsigned& output
  );
  static void convertLargeIntUnsignedToBase58SignificantDigitsLAST(
    const LargeIntegerUnsigned& input, std::string& output
  );
  static void convertLargeIntUnsignedToBase58SignificantDigitsFIRST(
    const LargeIntegerUnsigned& input, std::string& output, int numberOfOnesToPrepend
  );
  static bool convertBase58SignificantDigitsFIRSTToLargeIntUnsigned(
    const std::string& inputSignificantDigitsFirst,
    LargeIntegerUnsigned& output,
    int& numberOfLeadingZeroes,
    std::stringstream* commentsOnFailure
  );
  static bool convertBase58ToHexSignificantDigitsFirst(
    const std::string& input, std::string& output, std::stringstream* commentsOnFailure
  );
  static std::string convertIntToHex(int input, int significantBytes);
  static std::string convertUintToHex(unsigned int input, int significantBytes);
  static std::string convertUint64ToHex(uint64_t input);
  static bool convertHexToString(
    const std::string& input, std::string& output, std::stringstream* commentsOnFailure
  );
  static bool convertHexToListUnsignedChar(
    const std::string& input, List<unsigned char>& output, std::stringstream* commentsOnFailure
  );
  static bool convertHexToInteger(
    const std::string& input,
    LargeIntegerUnsigned& output,
    int& outputNumLeadingZeroPairs
  );
  static std::string convertListUnsignedCharsToBase64(
    const List<unsigned char>& input, bool useBase64URL
  );
  static std::string convertListUnsignedCharsToHex(
    const List<unsigned char>& input
  );
  static std::string convertListUnsignedCharsToHexFormat(
    const List<unsigned char>& input,
    int byteWidthLineBreakZeroForNone,
    bool useHtml
  );
  static bool convertListUnsignedCharsToHexFormat(
    const List<unsigned char>& input,
    std::string& output,
    int byteWidthLineBreakZeroForNone,
    bool useHtml
  );
  static std::string convertListCharsToHex(
    const List<char>& input,
    int byteWidthLineBreakZeroForNone,
    bool useHtml
  );
  static bool convertListCharsToHex(
    const List<char>& input, std::string& output, int byteWidthLineBreakZeroForNone, bool useHtml
  );
  static bool convertStringToHex(
    const std::string& input, std::string& output, int byteWidthLineBreakZeroForNone, bool useHtml
  );
  static void appendDoubleSha256Check(const std::string& input, std::string& output);
  static std::string convertStringToHex(
    const std::string& input, int byteWidthLineBreakZeroForNone, bool useHtml
  );
  static bool getBase58FromChar(char input, uint32_t& output);
  static bool getCharFromBase58(uint32_t input, char& output);
  static bool get6BitFromChar(unsigned char input, uint32_t& output);
  static char getCharFrom6Bit(uint32_t input, bool useBase64URL);
  static void convertBitStreamToString(const List<unsigned char>& input, std::string& output);
  static void convertStringToListBytes(const std::string& input, List<unsigned char>& output);
  static void convertStringToListBytesSigned(const std::string& input, List<char>& output);
  static void convertUint64toBigendianStringAppendResult(uint64_t input, std::string& outputAppend);
  static void convertUint64toBigendianListUnsignedCharAppendResult(
    uint64_t input, List<unsigned char>& outputAppend
  );
  static void convertUint128toBigendianListUnsignedCharAppendResult(
    uint64_t input, List<unsigned char>& outputAppend
  );
  static uint32_t leftRotateAsIfBigEndian(uint32_t input, int numBitsToRotate);
  static uint32_t rightRotateAsIfBigEndian(uint32_t input, int numBitsToRotate);
  static uint32_t rightShiftAsIfBigEndian(uint32_t input, int numBitsToShift);
  static uint32_t leftShiftAsIfBigEndian(uint32_t input, int numBitsToShift);

  static uint64_t leftRotateAsIfBigEndian64(uint64_t input, int numBitsToRotate);
  static uint64_t rightRotateAsIfBigEndian64(uint64_t input, int numBitsToRotate);
  static uint64_t rightShiftAsIfBigEndian64(uint64_t input, int numBitsToShift);
  static uint64_t leftShiftAsIfBigEndian64(uint64_t input, int numBitsToShift);

  static void initSha256();
  static void initSha512();
  static void computeSha1(const std::string& inputString, List<uint32_t>& output);
  static void computeSha224(const List<unsigned char>& input, List<uint32_t>& output);
  static void computeSha2xx(const List<unsigned char>& input, List<uint32_t>& output, bool is224);
  static void computeSha256(const List<unsigned char>& input, List<uint32_t>& output);
  static void computeSha256(const List<unsigned char>& input, List<unsigned char>& output);
  static void computeSha256(const std::string& input, std::string& output);
  static std::string computeSha256(const std::string& input);
  static void computeSha512(const List<unsigned char>& input, List<uint64_t>& output);
  static void computeSha512(const List<unsigned char>& input, List<unsigned char>& output);
  static std::string computeSha3_256OutputBase64URL(const std::string& input);
  static void computeKeccak3_256(const std::string& input, List<unsigned char>& output);
  static void computeSha3_256(const std::string& input, List<unsigned char>& output);
  static void convertListUint32ToLargeIntegerUnsignedLittleEndian(List<uint32_t>& input, LargeIntegerUnsigned& output);
  static LargeIntegerUnsigned rsaEncrypt(
    const LargeIntegerUnsigned& theModulus, const LargeInteger& theExponent, const LargeInteger& theMessage
  );
  static void convertListUnsignedCharsToLargeUnsignedIntegerBigEndian(
    const List<unsigned char>& input,
    LargeIntegerUnsigned& output
  );
  static bool convertBase64ToLargeUnsigned(
    const std::string& inputBase64,
    LargeIntegerUnsigned& output,
    std::stringstream* commentsOnFailure
  );
  static bool convertLargeUnsignedToBase64SignificantDigitsFirst(
    const LargeIntegerUnsigned& input,
    std::string& outputBase64
  );
  static bool convertLargeUnsignedToHexSignificantDigitsFirst(
    const LargeIntegerUnsigned& input,
    int numberOfLeadingZeroesToPadWith,
    std::string& outputHex
  );
  static bool convertLargeUnsignedToStringSignificantDigitsLast(
    const LargeIntegerUnsigned& input,
    std::string& output
  );
  static bool convertLargeUnsignedToStringSignificantDigitsFirst(
    const LargeIntegerUnsigned& input,
    int numberOfLeadingZeroesToPadWith,
    std::string& output
  );
  static bool verifyJWTagainstKnownKeys(
    const std::string& inputToken,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
  class Random {
    public:
    static void initializeRandomBytes();
    // Every call of this function acquires additional randomness from the system's timer.
    static void getRandomBytesSecureInternal(ListZeroAfterUse<unsigned char>& output, int numberOfBytesMax32);
    static void getRandomBytesSecureInternalMayLeaveTracesInMemory(List<unsigned char>& output, int numberOfBytesMax32);
    // Forget previous random bytes and the entropy of additionalRandomness.
    // We expect the extra entropy from sources such as the system timer, so
    // additionalRandomness is expected to contain considerably less
    // entropy than 64 bits.
    static void acquireAdditionalRandomness(int64_t additionalRandomness);

    static void getRandomLargeIntegerSecure(LargeIntegerUnsigned& output, int numBytes);
    static void getRandomLargePrime(LargeIntegerUnsigned& output, int numBytes);
  };
  // Shorter strings should hash faster, so
  // timing attacks on this should reveal differences in string lengths.
  static bool haveEqualHashes(const std::string& left, const std::string& right);
  class External {
  public:
    void (*computeRIPEMD160) (const std::string& input, List<unsigned char>& output);
    bool (*encryptAES_CBC_256) (
      const std::string& inputKey,
      const std::string& inputPlainText,
      List<unsigned char>& output,
      std::stringstream *commentsOnFailure
    );
    bool (*decryptAES_CBC_256) (
      const std::string& inputKey,
      const std::string& inputCipherText,
      List<unsigned char>& output,
      std::stringstream* commentsOnFailure
    );
    bool encryptAES_CBC_256_string (
      const std::string& inputKey,
      const std::string& inputPlainText,
      std::string& output,
      std::stringstream *commentsOnFailure
    );
    bool decryptAES_CBC_256_string (
      const std::string& inputKey,
      const std::string& inputCipherText,
      std::string& output,
      std::stringstream* commentsOnFailure
    );
  };
  static Crypto::External& externalCrypto();
};

class JSONWebToken {
public:
  std::string signatureScheme;
  std::string headerBase64;
  std::string claimsBase64;
  std::string signatureBase64;
  std::string headerJSON;
  std::string claimsJSON;
  MapList<std::string, std::string, MathRoutines::hashString> payloadKeys;

  bool assignString(const std::string& other, std::stringstream* commentsOnFailure);
  std::string toString();
  bool verifyRSA256(
    const LargeIntegerUnsigned& theModulus,
    const LargeIntegerUnsigned& theExponent,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral
  );
};
#endif // header_crypto_was_already_defined
