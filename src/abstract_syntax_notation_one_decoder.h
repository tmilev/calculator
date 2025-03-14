#ifndef header_abstract_syntax_notation_one_decoder_ALREADY_INCLUDED
#define header_abstract_syntax_notation_one_decoder_ALREADY_INCLUDED

#include "json.h"
#include "serialization_basic.h"

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
// in elements array.
//
// Non-pure composite elements have both non-empty
// ASNAtom and their children ASNElements are recorded
// in elements array.
//
// Finally, non-composite elements have their
// elements array of length zero and have their
// ASNAtom filled-in.
//
class ASNElement {
public:
  class JSLabels {
  public:
    static std::string offsetLastRead;
    static std::string offsetLastWrite;
    static std::string tag;
    static std::string lengthPromised;
    static std::string lengthEncoding;
    static std::string startByteOriginal;
    static std::string interpretation;
    static std::string error;
    static std::string isConstructed;
    static std::string type;
    static std::string children;
    static std::string body;
    static std::string numberOfChildren;
    static std::string comment;
  };

  friend std::ostream& operator<<(
    std::ostream& output, const ASNElement& element
  ) {
    output << element.toString();
    return output;
  }
  unsigned char startByte;
  unsigned char tag;
  int lengthPromised;
  // Offset relative to start of deserialization byte stream.
  int offsetLastRead;
  // Offset relative to start of serialization byte stream.
  int offsetLastWrite;
  bool flagIsConstructed;
  bool flagHeaderPadded;
  std::string error;
  std::string comment;
  List<unsigned char> ASNAtom;
  List<ASNElement> elements;
  ASNElement();
  void computeTag();
  bool hasBit7Set() const;
  bool hasBit8Set() const;
  bool hasCostructedStartByte() const;
  void resetExceptContent();
  void reset();
  bool isIntegerUnsigned(
    LargeIntegerUnsigned* whichInteger, std::stringstream* commentsOnFalse
  ) const;
  bool isInteger(
    LargeInteger* whichInteger, std::stringstream* commentsOnFalse
  ) const;
  std::string interpretAsObjectIdentifier() const;
  std::string interpretAsObjectIdentifierGetNameAndId() const;
  void toJSON(JSData& output) const;
  JSData toJSON() const;
  void writeAnnotations(List<Serialization::Marker>& output);
  std::string toString() const;
  bool isComposite() const;
  bool isPureComposite() const;
  bool isNonPureComposite() const;
  std::string getType() const;
  bool isTime() const;
  int getLengthLengthEncoding();
  void makeSequence(int numberOfEmptyElements);
  void makeSequence(const List<ASNElement>& input);
  // The following method
  // serialize an object like a sequence but
  // changes the tag to a bitstring.
  // While this is awkward,
  // it is in fact used for public key and
  // certificate extension serializations.
  void makeBitStringSequence(const List<ASNElement>& input);
  void makeSet(
    int numberOfEmptyElements,
    bool setLeadingBit,
    bool setSecondMostSignificantBit,
    bool constructed
  );
  void makeNull();
  void makeInteger(const LargeIntegerUnsigned& input);
  void makeBitString(const List<unsigned char>& input);
  void makeOctetString(const List<unsigned char>& input);
  void setStartByteFlags(
    bool setLeadingBit, bool setSecondMostSignificantBit, bool setConstructed
  );
  void makeBitStringEmpty(
    bool setLeadingBit, bool setSecondMostSignificantBit, bool setConstructed
  );
  void makeObjectId(const List<unsigned char>& input);
  template <typename thisPointerType>
  static bool hasSubElementTemplate(
    thisPointerType* thisPointer,
    const List<int>& desiredIndices,
    const List<unsigned char>& desiredTypes,
    thisPointerType** whichElement,
    std::stringstream* commentsOnFailure
  );
  bool hasSubElementConst(
    const List<int>& desiredIndices,
    const List<unsigned char>& desiredTypes,
    const ASNElement** whichElement,
    std::stringstream* commentsOnFailure
  ) const;
  bool hasSubElementNonConst(
    const List<int>& desiredIndices,
    const List<unsigned char>& desiredTypes,
    ASNElement** whichElement,
    std::stringstream* commentsOnFailure
  );
  bool hasSubElementGetCopy(
    const List<int>& desiredIndices,
    const List<unsigned char>& desiredTypes,
    ASNElement& output,
    std::stringstream* commentsOnFailure
  ) const;
  template <typename desiredType>
  bool hasSubElementOfType(
    const List<int>& desiredIndices,
    const List<unsigned char>& desiredTypes,
    desiredType& output,
    std::stringstream* commentsOnFailure
  ) const;
  void writeBytesConst(List<unsigned char>& output) const;
  void writeBytesUpdatePromisedLength(List<unsigned char>& output);
  static void writeBytesASNAtom(
    unsigned char startByte,
    const List<unsigned char>& content,
    List<unsigned char>& output
  );
  bool isEmpty() const;
  ASNElement& operator[](int index);
};

class ASNObject {
private:
  // the map below records samples for each known objectId
  static MapList<
    std::string, ASNObject, HashFunctions::hashFunction<std::string>
  > objectIdSamples;
public:
  class names {
  public:
    static std::string sha1;
    static std::string sha256;
    static std::string sha384;
    static std::string sha512;
    static std::string sha256WithRSAEncryption;
    static std::string rsaEncryption;
    static std::string subjectKeyIdentifier;
    static std::string authorityKeyIdentifier;
    static std::string basicConstraints;
    static std::string countryName;
    static std::string stateOrProvinceName;
    static std::string localityName;
    static std::string organizationName;
    static std::string organizationalUnitName;
    static std::string commonName;
    static std::string emailAddress;
  };

  friend std::ostream& operator<<(
    std::ostream& output, const ASNObject& element
  ) {
    output << element.toString();
    return output;
  }
  std::string name;
  ASNElement objectId;
  ASNElement content;
  static void initializeAddSample(
    MapList<std::string, ASNObject, HashFunctions::hashFunction<std::string> >&
    container,
    const std::string& inputName,
    const std::string& inputObjectIdHex,
    unsigned char inputContentTag
  );
  // static initialization order fiasco guard:
  // The function is thread-safe after returning once.
  static MapList<
    std::string, ASNObject, HashFunctions::hashFunction<std::string>
  >& namesToObjectIdsNonThreadSafe();
  static MapList<List<unsigned char>, ASNObject>& objectIdsToNames();
  static void initializeNonThreadSafe();
  bool loadFromASN(
    const ASNElement& input, std::stringstream* commentsOnFailure
  );
  static bool loadFieldsFromASNSequence(
    const ASNElement& input,
    MapList<std::string, ASNObject, HashFunctions::hashFunction<std::string> >&
    output,
    std::stringstream* commentsOnFailure
  );
  // Returns 1 if field was found, 0 otherwise.
  int loadField(
    const MapList<
      std::string, ASNObject, HashFunctions::hashFunction<std::string>
    >& inputFields,
    const std::string& fieldName
  );
  static const List<unsigned char>& objectIdFromNameNoFail(
    const std::string& input
  );
  static std::string toStringAllRecognizedObjectIds();
  void computeASN(ASNElement& output);
  std::string toString() const;
  bool isEmpty() const;
};

// The following class (is supposed to) implement a sufficiently
// large subset of the ASN-1 so as to serve our cryptographic needs.
// Materials used, in no particular order:
// https://www.w3.org/Protocols/HTTP-NG/asn1.html [brief philosophy of the how
// of the encoding, must read]
// http://luca.ntop.org/Teaching/Appunti/asn1.html [long technical discussion
// and tables, no proper explanations, use for reference]
// https://en.wikipedia.org/wiki/Abstract_Syntax_Notation_One
// https://lapo.it/asn1js [decodes messages and highlights and annotates
// individual bytes.]
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
  // 10 - context specific type (interpret as you wish: it's not
  // self-explanatory and the standards do not give a proper definition).
  // 11 - private type (interpret as you wish: it's not self-explanatory and
  // the
  // standards do not give a proper definition).
public:
  struct tags {
    static const unsigned char reserved0 = 0;
    static const unsigned char boolean0x01 = 1;
    static const unsigned char integer0x02 = 2;
    static const unsigned char bitString0x03 = 3;
    static const unsigned char octetString0x04 = 4;
    static const unsigned char null0x05;
    // cannot initialize here: gcc linker bug?
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
    static const unsigned char anyType0xff = 255;
    // Used exclusively for type matching.
  };

  // A class to write fixed length encodings.
  // The class carries out all useful work in its destructor.
  class WriterObjectFixedLength {
  private:
    // forbidden:
    void operator=(WriterObjectFixedLength&);
  public:
    List<unsigned char>* outputPointer;
    int* outputTotalByteLength;
    int offset;
    int totalByteLength;
    int reservedBytesForLength;
    static int getReservedBytesForLength(int length);
    static void writeLength(
      unsigned int input, List<unsigned char>& output, int offset
    );
    WriterObjectFixedLength(
      unsigned char startByte,
      int expectedTotalElementByteLength,
      List<unsigned char>& output,
      int* outputTotalElementByteLength
    );
    ~WriterObjectFixedLength();
  };

  int recursionDepthGuard;
  int maxRecursionDepth;
  int dataPointer;
  bool flagMustDecodeAll;
  const List<unsigned char>* rawData;
  ASNElement* decodedData;
  typedef bool(*typeDecoder)(
    AbstractSyntaxNotationOneSubsetDecoder& thisPointer,
    std::stringstream* commentsOnError
  );
  List<typeDecoder> decodersByByteValue;
  bool decode(
    const List<unsigned char>& inputRawData,
    int inputOffset,
    ASNElement& output,
    std::stringstream* commentsOnError
  );
  static std::string getType(unsigned char startByte);
  bool pointerIsBad(ASNElement& outputError);
  bool decodeCurrent(ASNElement& output);
  bool decodeSequenceLikeContent(ASNElement& output);
  bool decodeBitString(ASNElement& output);
  static LargeInteger variableLengthQuantityDecode(
    const List<unsigned char>& input, int& inputOutputDataPointer
  );
  void decodeASNAtomContent(ASNElement& output);
  bool decodeNull(ASNElement& output);
  bool decodeLengthIncrementDataPointerNoCheck(ASNElement& output);
  bool decodeLengthIncrementDataPointer(ASNElement& output);
  static void writeUnsignedIntegerObject(
    const LargeIntegerUnsigned& input, List<unsigned char>& output
  );
  static void writeObjectId(
    const List<unsigned char>& input, List<unsigned char>& output
  );
  static void writeNull(List<unsigned char>& output);
  std::string toStringAnnotateBinary();
  bool checkInitialization() const;
  void reset();
  AbstractSyntaxNotationOneSubsetDecoder();
  ~AbstractSyntaxNotationOneSubsetDecoder();
};

#endif // header_abstract_syntax_notation_one_decoder_ALREADY_INCLUDED
