#ifndef ABSTRACT_SYNTAX_NOTATION_ONE_HEADER_ALREADY_INCLUDED
#define ABSTRACT_SYNTAX_NOTATION_ONE_HEADER_ALREADY_INCLUDED
//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#include "json.h"

static ProjectInformationInstance ProjectInfoAbstractSyntaxNotationOneDecoderHeader(__FILE__, "Abstract syntax notation one (ASN-1) header file. ");

// ASNElements can be either composites or atoms.
//
// 1) The composites are:
// -- pure: all sequences and sets;
// -- non-pure: bitStrings and reserved (tag = 0) objects
//    whose byte content is a valid encoding.
// 2) Atoms: all non-composite elements.
//
// Pure composite elements have empty ASNAtoms,
// and their children ASNElements are recorded
// in theElements array.
//
// Non-pure composite elements have both non-empty
// ASNAtom and their children ASNElements are recorded
// in theElements array.
//
// Finally, non-composite elements have their
// theElemetns array of length zero and have their
// ASNAtom filled-in.
//
class ASNElement {
public:
  class JSLabels{
  public:
    static std::string offset;
    static std::string tag;
    static std::string lengthPromised;
    static std::string lengthBytes;
    static std::string startByteOriginal;
    static std::string value;
    static std::string error;
    static std::string isConstructed;
    static std::string type;
    static std::string children;
    static std::string body;
  };

  friend std::ostream& operator<<(std::ostream& output, const ASNElement& element) {
    output << element.ToString();
    return output;
  }
  unsigned char startByte;
  unsigned char tag;
  int lengthPromised;
  int offset;
  bool flagIsConstructed;
  std::string error;
  List<unsigned char> ASNAtom;
  List<ASNElement> theElements;
  ASNElement();
  void ComputeTag();
  bool hasBit7Set() const;
  bool hasBit8Set() const;
  bool hasCostructedStartByte() const;
  void reset();
  bool isIntegerUnsigned(LargeIntUnsigned* whichInteger, std::stringstream* commentsOnFalse) const;
  bool isInteger(LargeInt* whichInteger, std::stringstream* commentsOnFalse) const;
  std::string InterpretAsObjectIdentifier() const;
  void ToJSON(JSData& output) const;
  JSData ToJSON() const;
  std::string ToString() const;
  bool isComposite() const;
  bool isPureComposite() const;
  bool isNonPureComposite() const;
  bool isTime() const;
  bool HasSubElement(
    const List<int>& desiredIndices,
    const List<unsigned char>& desiredTypes,
    const ASNElement** whichElement,
    std::stringstream* commentsOnFailure
  ) const;
  bool HasSubElementGetCopy(
    const List<int>& desiredIndices,
    const List<unsigned char>& desiredTypes,
    ASNElement& output,
    std::stringstream* commentsOnFailure
  ) const;
  template <typename desiredType>
  bool HasSubElementOfType(
    const List<int>& desiredIndices,
    const List<unsigned char>& desiredTypes,
    desiredType& output,
    std::stringstream* commentsOnFailure
  ) const;
  const ASNElement& operator[](int index) const;
};

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
  friend std::ostream& operator<<(std::ostream& output, const ASNObject& element) {
    output << element.ToString();
    return output;
  }
  std::string name;
  ASNElement content;
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
  bool LoadFromASN(
    const ASNElement& input,
    std::stringstream* commentsOnFailure
  );
  static bool LoadFieldsFromASNSequence(
    const ASNElement& input,
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
  std::string ToString() const;
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
    static const unsigned char utf8String0x0c = 12;
    static const unsigned char sequence0x10 = 16;
    static const unsigned char set0x11 = 17;
    static const unsigned char printableString0x13 = 19;
    static const unsigned char IA5String0x16 = 22;
    static const unsigned char UTCTime0x17 = 23;
    static const unsigned char date0x1f = 31;
    static const unsigned char timeOfDay0x20 = 32;
    static const unsigned char dateTime0x21 = 33;
    static const unsigned char duration0x22 = 34;
    static const unsigned char null0x05;
    static const unsigned char anyType0xff = 255; // Used exclusively for type matching.
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
  bool flagMustDecodeAll;
  const List<unsigned char>* rawDatA;
  ASNElement* decodedData;
  typedef bool (*typeDecoder)(AbstractSyntaxNotationOneSubsetDecoder& thisPointer, std::stringstream* commentsOnError);
  List<typeDecoder> decodersByByteValue;
  bool Decode(
    const List<unsigned char>& inputRawData,
    int inputOffset,
    ASNElement& output,
    std::stringstream* commentsOnError
  );
  static std::string GetType(unsigned char startByte);
  bool PointerIsBad(ASNElement& outputError);
  bool DecodeCurrent(ASNElement& output);
  bool DecodeSequenceLikeContent(ASNElement& output);
  bool DecodeBitString(ASNElement& output);
  static LargeInt VariableLengthQuantityDecode(const List<unsigned char>& input, int& inputOutputDataPointer);
  void DecodeASNAtomContent(ASNElement& output);
  bool DecodeNull(ASNElement& output);
  bool DecodeCurrentBuiltInType(std::stringstream* commentsOnError);
  bool DecodeLengthIncrementDataPointer(ASNElement& output);

  static void WriteASNAtom(const ASNElement& input, List<unsigned char>& output);
  static void WriteListUnsignedCharsWithTag(
    const List<unsigned char>& input,
    unsigned char theTag,
    List<unsigned char>& output,
    bool constructed
  );
  static void WriteUnsignedIntegerObject(const LargeIntUnsigned& input, List<unsigned char>& output);
  static void WriteObjectId(const List<unsigned char>& input, List<unsigned char>& output);
  static void WriteNull(List<unsigned char>& output);

  std::string ToStringAnnotateBinary();
  std::string ToStringDebug() const;
  bool CheckInitialization() const;
  void reset();
  AbstractSyntaxNotationOneSubsetDecoder();
  ~AbstractSyntaxNotationOneSubsetDecoder();
};

#endif // ABSTRACT_SYNTAX_NOTATION_ONE_HEADER_ALREADY_INCLUDED
