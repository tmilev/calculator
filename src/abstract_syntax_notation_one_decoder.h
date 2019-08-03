//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfJson.h"

static ProjectInformationInstance ProjectInfoAbstractSyntaxNotationOneDecoderHeader(__FILE__, "Abstract syntax notation one (ASN-1) header file. ");

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
    static const unsigned char integer = 2;
    static const unsigned char sequence = 16;
    static const unsigned char octetString = 4;
    static const unsigned char objectIdentifier = 6;
    static const unsigned char tokenNull = 5;
  };
  int dataPointer;
  std::string rawData;
  JSData decodedData;
  bool flagLogByteInterpretation;
  List<std::string> byteInterpretationNotes;
  List<int> byteInterpretationDepth;
  typedef bool (*typeDecoder)(AbstractSyntaxNotationOneSubsetDecoder& thisPointer, std::stringstream* commentsOnError);
  List<typeDecoder> decodersByByteValue;
  bool Decode(std::stringstream* commentsOnError);
  bool PointerIsBad(std::stringstream* commentsOnError);
  bool DecodeCurrent(std::stringstream* commentsOnError, JSData& output);
  bool DecodeSequenceContent(std::stringstream* commentsOnError, int desiredLengthInBytes, JSData& output);
  bool DecodeIntegerContent(std::stringstream* commentsOnError, int desiredLengthInBytes, JSData& output);
  static LargeInt TryToDecode(const std::string& input, int& inputOutputDataPointer);
  bool DecodeOctetString(std::stringstream* commentsOnError, int desiredLengthInBytes, JSData& output);
  bool DecodeObjectIdentifier(std::stringstream* commentsOnError, int desiredLengthInBytes, JSData& output);
  bool DecodeNull(std::stringstream* commentsOnError, int desiredLengthInBytes, JSData& output);
  bool DecodeCurrentConstructed(std::stringstream* commentsOnError);
  bool DecodeCurrentBuiltInType(std::stringstream* commentsOnError);
  bool DecodeLengthIncrementDataPointer(int& outputLengthNegativeOneForVariable, std::stringstream* commentsOnError);
  std::string ToStringDebug() const;
  void initialize();
  AbstractSyntaxNotationOneSubsetDecoder();
  static bool isCostructedByte(unsigned char input);
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
