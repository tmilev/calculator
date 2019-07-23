//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfJson.h"

static ProjectInformationInstance ProjectInfoAbstractSyntaxNotationOneDecoderHeader(__FILE__, "Abstract syntax notation one (ASN-1) header file. ");

class AbstractSyntaxNotationOneSubsetDecoder;

// Object is used to hold arbitrary data in the context of
// cryptographic applications of ASN-1 encoding.
class AbstractSyntaxNotationOneSubsetObject {
  AbstractSyntaxNotationOneSubsetDecoder* owner;
  int indexInOwner;
  std::string theType;
  std::string data;
  AbstractSyntaxNotationOneSubsetObject();
};

// The following class (is supposed to) implement a sufficiently
// large subset of the ASN-1 so as to serve our cryptographic needs.

class AbstractSyntaxNotationOneSubsetDecoder {
public:
  int dataPointer;
  std::string rawData;
  JSData decodedData;
  List<AbstractSyntaxNotationOneSubsetObject> theObjects;
  bool Decode(std::stringstream* commentsOnError);
  static bool isCostructedByte(char input);
};
