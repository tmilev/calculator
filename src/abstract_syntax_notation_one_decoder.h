#ifndef ABSTRACT_SYNTAX_NOTATION_ONE_HEADER_ALREADY_INCLUDED
#define ABSTRACT_SYNTAX_NOTATION_ONE_HEADER_ALREADY_INCLUDED
//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#include "json.h"

static ProjectInformationInstance ProjectInfoAbstractSyntaxNotationOneDecoderHeader(__FILE__, "Abstract syntax notation one (ASN-1) header file. ");

class ASNObject {
private:
  // the map below records samples for each known objectId
  static MapList<std::string, ASNObject, MathRoutines::HashString> objectIdSamples;
public:
  class names {
  public:
    static std::string sha256WithRSAEncryption;
    static std::string countryName            ;
    static std::string stateOrProvinceName    ;
    static std::string localityName           ;
    static std::string organizationName       ;
    static std::string organizationalUnitName ;
    static std::string commonName             ;
    static std::string emailAddress           ;
  };
  std::string name;
  unsigned char contentTag;
  List<unsigned char> content;
  List<unsigned char> objectId;
  void MakeLookupId(const std::string& inputName, List<unsigned char>& inputContent);
  void Make(List<unsigned char>& inputObjectId, List<unsigned char>& inputContent);
  static void initializeAddSample(
    MapList<std::string, ASNObject, MathRoutines::HashString>& container,
    const std::string& inputName,
    const std::string& inputObjectIdHex,
    unsigned char inputContentTag
  );
  // static initialization order fiasco guard:
  static MapList<std::string, ASNObject, MathRoutines::HashString>& NamesToObjectIdsNonThreadSafe();
  static MapList<List<unsigned char>, ASNObject, MathRoutines::HashListUnsignedChars>& ObjectIdsToNames();
  static void initializeNonThreadSafe();
  bool LoadFromJSON(
    const JSData& inputShell,
    std::stringstream* commentsOnFailure
  );
  static bool LoadFieldsFromJSArray(
    const JSData &jsonArray,
    MapList<std::string, ASNObject, MathRoutines::HashString>& output,
    std::stringstream* commentsOnFailure
  );
  // Returns 1 if field was found, 0 otherwise.
  int LoadField(
    const MapList<std::string, ASNObject, MathRoutines::HashString> &inputFields,
    const std::string& fieldName
  );
  static std::string ToStringAllRecognizedObjectIds();
  void WriteBytesASNObject(List<unsigned char>& output);
  std::string ToString();
};

// The following class (is supposed to) implement a sufficiently
// large subset of the ASN-1 so as to serve our cryptographic needs.
// Materials used, in no particular order:
// https://www.w3.org/Protocols/HTTP-NG/asn1.html [brief philosophy of the how of the encoding, must read]
// http://luca.ntop.org/Teaching/Appunti/asn1.html [long technical discussion and tables, no proper explanations, use for reference]
// https://en.wikipedia.org/wiki/Abstract_Syntax_Notation_One
// https://lapo.it/asn1js [decodes messages and highlights and annotates individual bytes.]

class AbstractSyntaxNotationOneSubsetDecoder {
  // BER (basic encoding rules) concepts.
  // A piece of data is identified via a tag-length-value encoding.
  // First comes a tag which indicates the type,
  // then comes the length, and then the actual value.
  // The 6th-most significant bit, when set, is used to indicate
  // that the type is constructed, rather than built-in.
  // When the type is built-in, the low 5 bits are used to indicate the
  // data type.
  // The top 2 bits are reserved as follows:
  // 00 - universal data type.
  // 01 - application-specific type.
  // 10 - context specific type (interpret as you wish: it's not self-explanatory and the standards do not give a proper definition).
  // 11 - private type (interpret as you wish: it's not self-explanatory and the standards do not give a proper definition).
public:
  struct tags {
    static const unsigned char reserved0 = 0;
    static const unsigned char integer0x02 = 2;
    static const unsigned char bitString0x03 = 3;
    static const unsigned char octetString0x04 = 4;
    static const unsigned char objectIdentifier0x06 = 6;
    static const unsigned char utf8String0x0c = 12; // 0x0c
    static const unsigned char sequence0x10 = 16; //0x10
    static const unsigned char set0x11 = 17; //0x11
    static const unsigned char printableString0x13 = 19;
    static const unsigned char IA5String0x16 = 22;
    static const unsigned char UTCTime0x17 = 23;
    static const unsigned char date0x1f = 31;
    static const unsigned char timeOfDay0x20 = 32;
    static const unsigned char dateTime0x21 = 33;
    static const unsigned char duration0x22 = 34;
    static const unsigned char null0x05; //0x05
  };
  // writes fixed lenght encodings.
  class WriterObjectFixedLength {
  public:
    List<unsigned char>* outputPointer;
    int offset;
    int totalByteLength;
    int reservedBytesForLength;
    static int GetReservedBytesForLength(int length);
    static void WriteLength(unsigned int input, List<unsigned char>& output, int offset);
    WriterObjectFixedLength(
      unsigned char startByte,
      int expectedTotalElementByteLength,
      List<unsigned char>& output,
      bool isConstructed
    );
    ~WriterObjectFixedLength();
  };
  class WriterSequence : public WriterObjectFixedLength {
  public:
    WriterSequence(
      int expectedTotalElementByteLength,
      List<unsigned char>& output
    ) : WriterObjectFixedLength(
      AbstractSyntaxNotationOneSubsetDecoder::tags::sequence0x10,
      expectedTotalElementByteLength,
      output,
      true
      ) {}
  };
  class WriterInteger: public WriterObjectFixedLength {
  public:
    WriterInteger(
      int expectedTotalElementByteLength,
      List<unsigned char>& output
    ) : WriterObjectFixedLength(
      AbstractSyntaxNotationOneSubsetDecoder::tags::integer0x02,
      expectedTotalElementByteLength,
      output,
      false
      ) {}
  };
  class WriterBitString: public WriterObjectFixedLength {
  public:
    WriterBitString(
      int expectedTotalElementByteLength,
      List<unsigned char>& output
    ) : WriterObjectFixedLength(
      AbstractSyntaxNotationOneSubsetDecoder::tags::bitString0x03,
      expectedTotalElementByteLength,
      output,
      false
      ) {}
  };
  class WriterObjectId: public WriterObjectFixedLength {
  public:
    WriterObjectId(
      int expectedTotalElementByteLength,
      List<unsigned char>& output
    ) : WriterObjectFixedLength(
      AbstractSyntaxNotationOneSubsetDecoder::tags::objectIdentifier0x06,
      expectedTotalElementByteLength,
      output,
      false
      ) {}
  };
  class WriterSet: public WriterObjectFixedLength {
  public:
    WriterSet(
      int expectedTotalElementByteLength,
      List<unsigned char>& output
    ) : WriterObjectFixedLength(
      AbstractSyntaxNotationOneSubsetDecoder::tags::set0x11,
      expectedTotalElementByteLength,
      output,
      true
      ) {}
  };
  int recursionDepthGuarD;
  int maxRecursionDepth;
  int dataPointer;
  const List<unsigned char>* rawDatA;
  JSData* decodedData;
  JSData* dataInterpretation;
  bool flagLogByteInterpretation;
  List<std::string> byteInterpretationNotes;
  List<int> byteInterpretationDepth;
  typedef bool (*typeDecoder)(AbstractSyntaxNotationOneSubsetDecoder& thisPointer, std::stringstream* commentsOnError);
  List<typeDecoder> decodersByByteValue;
  bool Decode(
    const List<unsigned char>& inputRawData,
    JSData& outputDecodedData,
    JSData* outputInterpretation,
    std::stringstream* commentsOnError
  );
  std::string GetType(unsigned char startByte);
  bool PointerIsBad(JSData* interpretation);
  bool DecodeCurrent(JSData& output, JSData *interpretation);
  bool DecodeSequenceLikeContent(int desiredLengthInBytes, JSData& output, JSData* interpretation);
  bool DecodePrintableString(int desiredLengthInBytes, JSData& output, JSData* interpretation);
  bool DecodeBitString(int desiredLengthInBytes, JSData& output, JSData* interpretation);
  bool DecodeUTF8String(int desiredLengthInBytes, JSData& output, JSData* interpretation);
  bool DecodeIA5String(int desiredLengthInBytes, JSData& output, JSData* interpretation);
  bool DecodeUTCString(int desiredLengthInBytes, JSData& output, JSData* interpretation);
  bool DecodeIntegerContent(int desiredLengthInBytes, JSData& output, JSData* interpretation);
  static LargeInt VariableLengthQuantityDecode(const List<unsigned char> &input, int& inputOutputDataPointer);
  bool DecodeOctetString(int desiredLengthInBytes, JSData& output, JSData* interpretation);
  bool DecodeObjectIdentifier(int desiredLengthInBytes, JSData& output, JSData* interpretation);
  bool DecodeNull(int desiredLengthInBytes, JSData& output, JSData* interpretation);
  bool DecodeCurrentBuiltInType(std::stringstream* commentsOnError);
  bool DecodeLengthIncrementDataPointer(int& outputLengthNegativeOneForVariable, JSData* interpretation);

  static void WriteUnsignedIntegerObject(const LargeIntUnsigned& input, List<unsigned char>& output);
  static void WriteObjectId(const List<unsigned char>& input, List<unsigned char>& output);
  static void WriteNull(List<unsigned char>& output);

  std::string ToStringAnnotateBinary();
  std::string ToStringDebug() const;
  void reset();
  bool CheckInitialization() const;
  AbstractSyntaxNotationOneSubsetDecoder();
  ~AbstractSyntaxNotationOneSubsetDecoder();
  static bool isCostructedByte(unsigned char input);
  static bool hasBit7Set(unsigned char input);
  static bool hasBit8Set(unsigned char input);
  // Pointers to the following functions will be used.
  // These functions would suitably be member functions, however
  // I chose to make them static members for the following reasons.
  // Taking pointers of member functions has either
  // 1. confusing syntax - as of 2019, something along the lines of:
  //    (this->(*handler))(commentsOnError)
  //    - or,
  // 2. requires use of std::invoke, which in turn requires C++17 standard, which may not
  //    be fully suppported yet
  //    [say, by your favorite compiler on your favorite out-of-date linux distro].
};

#endif // ABSTRACT_SYNTAX_NOTATION_ONE_HEADER_ALREADY_INCLUDED
