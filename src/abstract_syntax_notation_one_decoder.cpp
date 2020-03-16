// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "abstract_syntax_notation_one_decoder.h"
#include "crypto.h"
#include "math_extra_algebraic_numbers.h"
#include "math_general_implementation.h"
#include "serialization_basic.h"
#include "transport_layer_security.h"

// Putting this in the header file currently breaks the linking for this particular tag.
// Appears to be a compiler/linker bug (?).
const unsigned char AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05 = 5;

bool ASNElement::hasCostructedStartByte() const {
  int sixthBit = this->startByte / 32;
  sixthBit %= 2;
  return sixthBit != 0;
}

bool ASNElement::hasBit7Set() const {
  int seventhBit = this->startByte / 64;
  seventhBit %= 2;
  return seventhBit != 0;
}

bool ASNElement::hasBit8Set() const {
  int eighthBit = this->startByte / 128;
  eighthBit %= 2;
  return eighthBit != 0;
}

void AbstractSyntaxNotationOneSubsetDecoder::reset() {
  this->rawDatA = nullptr;
  this->decodedData = nullptr;
  this->dataPointer = 0;
}

AbstractSyntaxNotationOneSubsetDecoder::AbstractSyntaxNotationOneSubsetDecoder() {
  this->decodedData = nullptr;
  this->dataPointer = 0;
  this->recursionDepthGuarD = 0;
  this->maxRecursionDepth = 20;
  this->flagMustDecodeAll = true;
}

AbstractSyntaxNotationOneSubsetDecoder::~AbstractSyntaxNotationOneSubsetDecoder() {
}

bool AbstractSyntaxNotationOneSubsetDecoder::CheckInitialization() const {
  if (this->decodedData == nullptr) {
    global.fatal << "Uninitialized ASN1 output json. " << global.fatal;
  }
  if (this->rawDatA == nullptr) {
    global.fatal << "Uninitialized ASN1 raw data. " << global.fatal;
  }
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeLengthIncrementDataPointer(
  ASNElement& output
) {
  if (!this->DecodeLengthIncrementDataPointerNoCheck(output)) {
    return false;
  }
  if (output.lengthPromised + this->dataPointer > this->rawDatA->size) {
    std::stringstream errorStream;
    errorStream << "Element length: " << output.lengthPromised << " plus element body offset: "
    << this->dataPointer << " exceeds the total byte length: " << this->rawDatA->size << ". ";
    output.error = errorStream.str();
    return false;
  }
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeLengthIncrementDataPointerNoCheck(
  ASNElement& output
) {
  output.lengthPromised = 0;
  int startDataPointer = this->dataPointer;
  unsigned char currentByte = (*this->rawDatA)[this->dataPointer];
  if (currentByte < 128) { // 128 = 0x80
    output.lengthPromised = currentByte;
    this->dataPointer ++;
    return true;
  }
  if (currentByte == 128) { // 128 = 0x80 signals varible-length encoding
    output.lengthPromised = - 1;
    this->dataPointer ++;
    return true;
  }
  int numberOfLengthBytes = currentByte - 128;
  LargeInteger length = 0;
  this->dataPointer ++;
  for (int i = 0; i < numberOfLengthBytes; i ++) {
    length *= 256;
    if (this->PointerIsBad(output)) {
      return false;
    }
    unsigned char nextByte = (*this->rawDatA)[this->dataPointer];
    length += static_cast<signed>(static_cast<unsigned int>(nextByte));
    this->dataPointer ++;
  }
  if (!length.IsIntegerFittingInInt(&output.lengthPromised)) {
    std::stringstream errorStream;
    errorStream << "Variable length: " << length << " whose decoding started at position: "
    << startDataPointer << " with first excluded position: " << this->dataPointer << " is too large. ";
    output.error = errorStream.str();
    return false;
  }
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::PointerIsBad(
  ASNElement& outputError
) {
  if (this->dataPointer >= this->rawDatA->size || this->dataPointer < 0) {
    outputError.error = "Unexpected overflow error: data pointer is negative. ";
    return true;
  }
  return false;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeSequenceLikeContent(
  ASNElement& output
) {
  int lastIndexPlusOne = this->dataPointer + output.lengthPromised;
  ASNElement nextElement;
  int numberOfDecoded = 0;
  // We reserve 32 bytes per object - a reasonable assumption.
  output.theElements.Reserve(output.lengthPromised / 32);
  while (this->dataPointer < lastIndexPlusOne) {
    int lastPointer = this->dataPointer;
    bool isGood = this->DecodeCurrent(nextElement);
    if (!isGood){
      std::stringstream errorStream;
      errorStream << "Failed to decode sequence element of index: " << numberOfDecoded << ". ";
      output.error = errorStream.str();
      return false;
    }
    if (lastPointer >= this->dataPointer) {
      global.fatal << "Programming error: decode current "
      << "did not increment the data pointer. " << global.fatal;
    }
    output.theElements.AddOnTop(nextElement);
    numberOfDecoded ++;
  }
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeBitString(
  ASNElement& output
) {
  int offsetAtStart = this->dataPointer;
  this->DecodeASNAtomContent(output);
  AbstractSyntaxNotationOneSubsetDecoder subDecoder;
  subDecoder.flagMustDecodeAll = false;
  ASNElement subDecoderResult;
  subDecoder.recursionDepthGuarD = this->recursionDepthGuarD + 1;
  bool insidePadded = false;
  if (offsetAtStart < this->rawDatA->size) {
    if ((*this->rawDatA)[offsetAtStart] == 0) {
      offsetAtStart ++;
      insidePadded = true;
    }
  }
  if (subDecoder.Decode(*this->rawDatA, offsetAtStart, subDecoderResult, nullptr)) {
    if (subDecoder.dataPointer == this->dataPointer) {
      output.theElements.SetSize(0);
      output.theElements.AddOnTop(subDecoderResult);
      if (insidePadded) {
        output.flagHeaderPadded = true;
      }
    }
  }
  return true;
}

void AbstractSyntaxNotationOneSubsetDecoder::DecodeASNAtomContent(
  ASNElement& output
) {
  output.ASNAtom.SetSize(output.lengthPromised);
  for (int i = 0; i < output.lengthPromised; i ++) {
    output.ASNAtom[i] = (*this->rawDatA)[this->dataPointer + i];
  }
  this->dataPointer += output.lengthPromised;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeNull(
  ASNElement& output
) {
  if (output.lengthPromised != 0) {
    output.error = "Length of null object is not zero. ";
    return false;
  }
  this->DecodeASNAtomContent(output);
  return true;
}

LargeInteger AbstractSyntaxNotationOneSubsetDecoder::VariableLengthQuantityDecode(
  const List<unsigned char>& input, int& inputOutputDataPointer
) {
  LargeInteger result = 0;
  for (; inputOutputDataPointer < input.size ; inputOutputDataPointer ++) {
    unsigned char currentByte = input[inputOutputDataPointer];
    result *= 128;
    if (currentByte < 128) {
      result += static_cast<int>(currentByte);
      inputOutputDataPointer ++;
      break;
    } else {
      currentByte -= 128;
      result += static_cast<int>(currentByte);
    }
  }
  return result;
}

std::string ASNElement::InterpretAsObjectIdentifierGetNameAndId() const {
  std::stringstream out;
  out << "[" << this->InterpretAsObjectIdentifier();
  out << "]: ";
  if (!ASNObject::ObjectIdsToNames().Contains(this->ASNAtom)) {
    out << "[unknown]";
  } else {
    out << ASNObject::ObjectIdsToNames().GetValueCreate(this->ASNAtom).name;
  }
  return out.str();
}

std::string ASNElement::InterpretAsObjectIdentifier() const {
  MacroRegisterFunctionWithName("ASNElement::InterpretAsObjectIdentifier");
  if (this->ASNAtom.size <= 0) {
    return "not an object id";
  }
  std::stringstream resultStream;
  unsigned char firstByte = this->ASNAtom[0];
  // first two entries are encoded in the first byte:
  unsigned char firstEntry = firstByte / 40;
  unsigned char secondEntry = firstByte % 40;
  resultStream << static_cast<int>(firstEntry) << "." << static_cast<int>(secondEntry);
  for (int i = 0; i < this->ASNAtom.size; i ++) {
    LargeInteger nextInt = AbstractSyntaxNotationOneSubsetDecoder::VariableLengthQuantityDecode(this->ASNAtom, i);
    resultStream << "." << nextInt.ToString();
  }
  return resultStream.str();
}

void ASNElement::WriteBytesConst(List<unsigned char>& output) const {
  // WARNING: writer needs to be a named object to avoid
  // gcc immediately calling the object's destructor.
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength
  notAnonymous(this->startByte, this->lengthPromised, output, nullptr);
  if (this->flagHeaderPadded) {
    output.AddOnTop(0);
  }
  if (this->ASNAtom.size > 0) {
    output.AddListOnTop(this->ASNAtom);
  } else {
    for (int i = 0; i < this->theElements.size; i ++) {
      this->theElements[i].WriteBytesConst(output);
    }
  }
}

void ASNElement::WriteBytesUpdatePromisedLength(List<unsigned char>& output) {
  this->offsetLastWrite = output.size;
  // WARNING: writer needs to be a named object to avoid
  // gcc immediately calling the object's destructor.
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength
  notAnonymous(this->startByte, this->lengthPromised, output, &this->lengthPromised);
  if (this->flagHeaderPadded) {
    output.AddOnTop(0);
  }
  if (this->ASNAtom.size > 0) {
    output.AddListOnTop(this->ASNAtom);
  } else {
    for (int i = 0; i < this->theElements.size; i ++) {
      this->theElements[i].WriteBytesUpdatePromisedLength(output);
    }
  }
}

void ASNElement::WriteBytesASNAtom(
  unsigned char inputStartByte,
  const List<unsigned char> &atomContent,
  List<unsigned char>& output
) {
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength
  notAnonymous(inputStartByte, atomContent.size, output, nullptr);
  output.AddListOnTop(atomContent);
}

bool ASNElement::isComposite() const {
  return this->theElements.size > 0;
}

bool ASNElement::isPureComposite() const {
  return this->theElements.size > 0 && this->ASNAtom.size == 0;
}

bool ASNElement::isNonPureComposite() const {
  return this->theElements.size > 0 && this->ASNAtom.size > 0;
}

ASNElement& ASNElement::operator[](int index) {
  return this->theElements[index];
}

bool ASNElement::HasSubElementGetCopy(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  ASNElement& output,
  std::stringstream* commentsOnFailure
) const {
  const ASNElement* element = nullptr;
  if (!this->HasSubElementConst(
    desiredIndices, desiredTypes, &element, commentsOnFailure
  )) {
    return false;
  }
  output = *element;
  return true;
}

bool ASNElement::HasSubElementConst(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  const ASNElement** whichElement,
  std::stringstream* commentsOnFailure
) const {
  return ASNElement::HasSubElementTemplate(
    this, desiredIndices, desiredTypes, whichElement, commentsOnFailure
  );
}

template <typename thisPointerType>
bool ASNElement::HasSubElementTemplate(
  thisPointerType* thisPointer,
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  thisPointerType **whichElement,
  std::stringstream* commentsOnFailure
) {
  thisPointerType* current = thisPointer;
  for (int i = 0; i < desiredIndices.size; i ++) {
    int currentIndex = desiredIndices[i];
    if (currentIndex >= current->theElements.size) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "ASN element is missing composite index: "
        << desiredIndices.SliceCopy(0, i + 1).ToStringCommaDelimited() << ". ";
      }
      return false;
    }
    current = &(current->theElements[currentIndex]);
    if (i < desiredTypes.size) {
      unsigned char desiredTag = desiredTypes[i];
      if (
        current->tag != desiredTag &&
        desiredTag != AbstractSyntaxNotationOneSubsetDecoder::tags::anyType0xff
      ) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "ASN element with composite index: "
          << desiredIndices.SliceCopy(0, i + 1).ToStringCommaDelimited() << ". "
          << " is not of the needed type " << static_cast<int>(desiredTag) << ". ";
        }
      }
    }
  }
  if (whichElement != nullptr) {
    *whichElement = current;
  }
  return true;
}

template < >
bool ASNElement::HasSubElementOfType<ASNObject>(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  ASNObject& output,
  std::stringstream* commentsOnFailure
) const {
  const ASNElement* element = nullptr;
  if (!ASNElement::HasSubElementConst(desiredIndices, desiredTypes, &element, commentsOnFailure)) {
    return false;
  }
  return output.LoadFromASN(*element, commentsOnFailure);
}

template < >
bool ASNElement::HasSubElementOfType<LargeIntegerUnsigned>(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  LargeIntegerUnsigned& output,
  std::stringstream* commentsOnFailure
) const {
  const ASNElement* element = nullptr;
  if (!ASNElement::HasSubElementConst(desiredIndices, desiredTypes, &element, commentsOnFailure)) {
    return false;
  }
  return element->isIntegerUnsigned(&output, commentsOnFailure);
}

template < >
bool ASNElement::HasSubElementOfType<LargeInteger>(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  LargeInteger& output,
  std::stringstream* commentsOnFailure
) const {
  const ASNElement* element = nullptr;
  if (!ASNElement::HasSubElementConst(desiredIndices, desiredTypes, &element, commentsOnFailure)) {
    return false;
  }
  return element->isInteger(&output, commentsOnFailure);
}

template < >
bool ASNElement::HasSubElementOfType<int>(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  int& output,
  std::stringstream* commentsOnFailure
) const {
  LargeInteger outputLarge;
  if (!this->HasSubElementOfType(desiredIndices, desiredTypes, outputLarge, commentsOnFailure)) {
    return false;
  }
  if (!outputLarge.IsIntegerFittingInInt(&output)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "ASN is not a small integer because it is too large. ";
    }
    return false;
  }
  return true;
}

template < >
bool ASNElement::HasSubElementOfType<unsigned int>(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  unsigned int& output,
  std::stringstream* commentsOnFailure
) const {
  int outputSigned = 0;
  if (!this->HasSubElementOfType(desiredIndices, desiredTypes, outputSigned, commentsOnFailure)) {
    return false;
  }
  if (outputSigned < 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "ASN is a negative integer, where a non-negative one is requested. ";
    }
    return false;
  }
  output = static_cast<unsigned int>(outputSigned);
  return true;
}

template < >
bool ASNElement::HasSubElementOfType<List<unsigned char> >(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  List<unsigned char>& output,
  std::stringstream* commentsOnFailure
) const {
  const ASNElement* element = nullptr;
  if (!ASNElement::HasSubElementConst(desiredIndices, desiredTypes, &element, commentsOnFailure)) {
    return false;
  }
  if (element->isComposite()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Element with key: " << desiredIndices << " is not atomic. ";
    }
    return false;
  }
  output = element->ASNAtom;
  return true;
}

bool ASNElement::isTime() const {
  return
  this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::timeOfDay0x20 ||
  this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::UTCTime0x17;
}

std::string ASNElement::ToString() const {
  return this->ToJSON().ToString(&JSData::PrintOptions::HexEncodeNonASCII());
}

void ASNElement::WriteAnnotations(List<Serialization::Marker>& output) {
  std::stringstream bodyStream, tagStream, lengthStream;
  tagStream << "Type: " << AbstractSyntaxNotationOneSubsetDecoder::GetType(this->tag);
  lengthStream << "Content length: " << this->lengthPromised;
  bodyStream << tagStream.str() << "<br>" << lengthStream.str();
  if (this->comment != "") {
    bodyStream << "<br>" << comment;
  }
  int lengthOfLengthEncoding = this->GetLengthLengthEncoding();
  output.AddOnTop(Serialization::Marker(
    this->offsetLastWrite, this->lengthPromised + lengthOfLengthEncoding + 1, bodyStream.str()
  ));
  output.AddOnTop(Serialization::Marker(
    this->offsetLastWrite, 1, tagStream.str()
  ));
  output.AddOnTop(Serialization::Marker(
    this->offsetLastWrite + 1, lengthOfLengthEncoding, lengthStream.str()
  ));
  output.AddOnTop(Serialization::Marker(
    this->offsetLastWrite + 1 + lengthOfLengthEncoding, this->lengthPromised, bodyStream.str()
  ));
  for (int i = 0; i < this->theElements.size; i ++) {
    this->theElements[i].WriteAnnotations(output);
  }
}

JSData ASNElement::ToJSON() const {
  JSData result;
  this->ToJSON(result);
  return result;
}

std::string ASNElement::JSLabels::body = "body";
std::string ASNElement::JSLabels::children = "children";
std::string ASNElement::JSLabels::error = "error";
std::string ASNElement::JSLabels::isConstructed = "isConstructed";
std::string ASNElement::JSLabels::lengthEncoding = "lengthEncoding";
std::string ASNElement::JSLabels::lengthPromised = "lengthPromised";
std::string ASNElement::JSLabels::offsetLastRead = "offsetLastRead";
std::string ASNElement::JSLabels::offsetLastWrite = "offsetLastWrite";
std::string ASNElement::JSLabels::startByteOriginal = "startByteOriginal";
std::string ASNElement::JSLabels::tag = "tag";
std::string ASNElement::JSLabels::type = "type";
std::string ASNElement::JSLabels::interpretation = "interpretation";
std::string ASNElement::JSLabels::numberOfChildren = "numberOfChildren";
std::string ASNElement::JSLabels::comment = "comment";

int ASNElement::GetLengthLengthEncoding() {
  List<unsigned char> lengthEncoding;
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::WriteLength(
    static_cast<unsigned>(this->lengthPromised), lengthEncoding, 0
  );
  return lengthEncoding.size;
}

void ASNElement::ToJSON(JSData& output) const {
  output.reset();
  output[ASNElement::JSLabels::tag] = StringRoutines::ConvertByteToHex(this->tag);
  output[ASNElement::JSLabels::startByteOriginal] = StringRoutines::ConvertByteToHex(this->startByte);
  output[ASNElement::JSLabels::lengthPromised] = this->lengthPromised;
  output[ASNElement::JSLabels::type] = AbstractSyntaxNotationOneSubsetDecoder::GetType(this->tag);
  if (this->offsetLastRead >= 0) {
    output[ASNElement::JSLabels::offsetLastRead] = this->offsetLastRead;
  }
  if (this->offsetLastWrite >= 0) {
    output[ASNElement::JSLabels::offsetLastWrite] = this->offsetLastWrite;
  }
  List<unsigned char> lengthEncoding;
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::WriteLength(
    static_cast<unsigned>(this->lengthPromised), lengthEncoding, 0
  );
  output[ASNElement::JSLabels::lengthEncoding] = Crypto::ConvertListUnsignedCharsToHex(lengthEncoding);
  if (this->flagHeaderPadded) {
    output[ASNElement::JSLabels::lengthEncoding] = output[ASNElement::JSLabels::lengthEncoding].theString + "00";
  }
  output[ASNElement::JSLabels::isConstructed] = this->flagIsConstructed;
  output[ASNElement::JSLabels::numberOfChildren] = this->theElements.size;
  if (this->comment != "") {
    output[ASNElement::JSLabels::comment] = this->comment;
  }
  if (this->isComposite()) {
    JSData children;
    children.theType = JSData::token::tokenArray;
    children.theList.Reserve(this->theElements.size);
    for (int i = 0; i < this->theElements.size; i ++) {
      JSData incoming;
      this->theElements[i].ToJSON(incoming);
      children.theList.AddOnTop(incoming);
    }
    output[ASNElement::JSLabels::children] = children;
  }
  if (this->ASNAtom.size > 0 || !this->isComposite()) {
    output[ASNElement::JSLabels::body] = Crypto::ConvertListUnsignedCharsToHex(this->ASNAtom);
    if (
      this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::utf8String0x0c ||
      this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::printableString0x13 ||
      this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::UTCTime0x17 ||
      this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::dateTime0x21
    ) {
      output[ASNElement::JSLabels::interpretation] = this->ASNAtom.ToStringConcatenate();
    }
    if (this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::objectIdentifier0x06) {
      output[ASNElement::JSLabels::interpretation] = this->InterpretAsObjectIdentifierGetNameAndId();
    }
    if (this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::octetString0x04) {
      output[ASNElement::JSLabels::interpretation] = Crypto::ConvertListUnsignedCharsToHex(this->ASNAtom);
    }
    if (this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::boolean0x01) {
      if (this->ASNAtom.size > 0) {
        if (this->ASNAtom[0]) {
          output[ASNElement::JSLabels::interpretation] = "true";
        } else {
          output[ASNElement::JSLabels::interpretation] = "false";
        }
      }
    }
    if (this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::integer0x02) {
      LargeInteger theInt;
      this->isInteger(&theInt, nullptr);
      output[ASNElement::JSLabels::interpretation] = theInt.ToString();
    }
  }
}

bool ASNElement::isIntegerUnsigned(
  LargeIntegerUnsigned* whichInteger,
  std::stringstream* commentsOnFalse
) const {
  MacroRegisterFunctionWithName("ASNElement::isIntegerUnsigned");
  if (this->tag != AbstractSyntaxNotationOneSubsetDecoder::tags::integer0x02) {
    if (commentsOnFalse != nullptr) {
      *commentsOnFalse << "Element is not an unsigned integer but is of type: " << this->GetType() << ". ";
    }
    return false;
  }
  if (this->ASNAtom.size > 0) {
    if (this->ASNAtom[0] >= 128) {
      if (commentsOnFalse != nullptr) {
        *commentsOnFalse << "Integer is negative. ";
      }
      return false;
    }
  }
  if (whichInteger == nullptr) {
    return true;
  }
  LargeInteger result;
  if (!this->isInteger(&result, commentsOnFalse)) {
    global.fatal << "ASNElement must be an integer at this point. " << global.fatal;
  }
  *whichInteger = result.value;
  return true;
}

bool ASNElement::isInteger(LargeInteger *whichInteger, std::stringstream *commentsOnFalse) const {
  MacroRegisterFunctionWithName("ASNElement::isInteger");
  if (this->tag != AbstractSyntaxNotationOneSubsetDecoder::tags::integer0x02) {
    if (commentsOnFalse != nullptr) {
      *commentsOnFalse << "Element is not an integer but is of type: " << this->GetType() << ". ";
    }
    return false;
  }
  if (whichInteger == nullptr) {
    return true;
  }
  *whichInteger = 0;
  int currentContribution = 0;
  for (int i = 0; i < this->ASNAtom.size; i ++) {
    unsigned char unsignedContribution = this->ASNAtom[i];
    currentContribution = unsignedContribution;
    if (i == 0) {
      if (currentContribution >= 128) {
        currentContribution -= 255;
      }
    }
    *whichInteger *= 256;
    *whichInteger += currentContribution;
  }
  return true;
}

std::string AbstractSyntaxNotationOneSubsetDecoder::GetType(unsigned char startByte) {
  switch (startByte) {
  case AbstractSyntaxNotationOneSubsetDecoder::tags::reserved0:
    return "reserved";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::boolean0x01:
    return "boolean";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::integer0x02:
    return "integer";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::bitString0x03:
    return "bitString";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::octetString0x04:
    return "octetString";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05:
    return "null";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::objectIdentifier0x06:
    return "objectID";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::utf8String0x0c:
    return "utf8 string";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::sequence0x10:
    return "sequence";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::set0x11:
    return "set";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::printableString0x13:
    return "printable string";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::IA5String0x16:
    return "IA5String";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::UTCTime0x17:
    return "UTCTime";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::date0x1f:
    return "date";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::timeOfDay0x20:
    return "timeOfDay";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::dateTime0x21:
    return "dateTime";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::duration0x22:
    return "duration";
  default:
    return "unknown";
  }
}

ASNElement::ASNElement() {
  this->reset();
}

void ASNElement::reset() {
  this->resetExceptContent();
  this->ASNAtom.SetSize(0);
  this->theElements.SetSize(0);
}

void ASNElement::resetExceptContent() {
  this->flagIsConstructed = false;
  this->lengthPromised = 0;
  this->tag = 0;
  this->startByte = 0;
  this->error = "";
  this->offsetLastRead = - 1;
  this->offsetLastWrite = - 1;
  this->flagHeaderPadded = false;
}

void AbstractSyntaxNotationOneSubsetDecoder::WriteNull(List<unsigned char>& output) {
  MacroRegisterFunctionWithName("AbstractSyntaxNotationOneSubsetDecoder::WriteNull");
  output.AddOnTop(AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05);
  output.AddOnTop(0);
}

void ASNElement::ComputeTag() {
  this->tag = this->startByte;
  this->flagIsConstructed = this->hasCostructedStartByte();
  if (this->flagIsConstructed) {
    if (
      this->tag != AbstractSyntaxNotationOneSubsetDecoder::tags::timeOfDay0x20 &&
      this->tag != AbstractSyntaxNotationOneSubsetDecoder::tags::dateTime0x21 &&
      this->tag != AbstractSyntaxNotationOneSubsetDecoder::tags::duration0x22
    ) {
      this->tag -= 32;
    }
  }
  bool hasBit7 = this->hasBit7Set();
  if (hasBit7) {
    this->tag -= 64;
  }
  bool hasBit8 = this->hasBit8Set();
  if (hasBit8) {
    this->tag -= 128;
  }
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeCurrent(
  ASNElement& output
) {
  MacroRegisterFunctionWithName("AbstractSyntaxNotationOneSubsetDecoder::DecodeCurrent");
  RecursionDepthCounter recursionGuard(&this->recursionDepthGuarD);
  if (this->recursionDepthGuarD > this->maxRecursionDepth) {
    std::stringstream errorStream;
    errorStream << "Max depth of " << this->maxRecursionDepth << " exceeded. ";
    output.error = errorStream.str();
    return false;
  }
  output.reset();
  output.offsetLastRead = this->dataPointer;
  if (this->PointerIsBad(output)) {
    return false;
  }
  output.startByte = (*this->rawDatA)[this->dataPointer];
  output.ComputeTag();
  this->dataPointer ++;
  if (this->PointerIsBad(output)) {
    return false;
  }
  if (!this->DecodeLengthIncrementDataPointer(output)) {
    return false;
  }
  std::stringstream errorStream;
  switch (output.tag) {
  // pure composite elements:
  case tags::reserved0:
    return this->DecodeSequenceLikeContent(output);
  case tags::sequence0x10:
    return this->DecodeSequenceLikeContent(output);
  case tags::set0x11:
    return this->DecodeSequenceLikeContent(output);
  // either non-pure composite or atom:
  case tags::bitString0x03:
    return this->DecodeBitString(output);
  // atoms with extra comments/interpretation:
  case tags::null0x05:
    return this->DecodeNull(output);
  // atoms without additional interpretation:
  case tags::boolean0x01:
  case tags::integer0x02:
  case tags::objectIdentifier0x06:
  case tags::printableString0x13:
  case tags::utf8String0x0c:
  case tags::IA5String0x16:
  case tags::UTCTime0x17:
  case tags::octetString0x04:
    this->DecodeASNAtomContent(output);
    return true;
  default:
    errorStream << "Unknown object tag: " << static_cast<int>(output.tag) << ", byte: "
    << static_cast<int>(output.startByte) << " at position: " << output.offsetLastRead
    << ". Length: " << output.lengthPromised << ". ";
    output.error = errorStream.str();
    return false;
  }
}

bool AbstractSyntaxNotationOneSubsetDecoder::Decode(
  const List<unsigned char>& inputRawData,
  int inputOffset,
  ASNElement& output,
  std::stringstream* commentsOnError
) {
  MacroRegisterFunctionWithName("AbstractSyntaxNotationOneSubsetDecoder::Decode");
  if (inputRawData.size == 0) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Empty sequences are not allowed in our ASN-1 decoder. ";
    }
    return false;
  }
  int maxAllowedSize = 1000000000;
  if (inputRawData.size >= maxAllowedSize) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Input size: " << this->rawDatA->size
      << " too large, max allowed: " << maxAllowedSize << ". ";
    }
    return false;
  }
  this->reset();
  this->dataPointer = inputOffset;
  this->decodedData = &output;
  this->rawDatA = &inputRawData;
  this->CheckInitialization();
  this->decodedData->reset();
  if (!this->DecodeCurrent(*this->decodedData)) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Failed to decode data with error: " << this->decodedData->error;
    }
    return false;
  }
  if (this->dataPointer < this->rawDatA->size && this->flagMustDecodeAll) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Decoded " << this->dataPointer
      << " bytes but the input had: " << this->rawDatA->size << ". ";
    }
    return false;
  }
  return true;
}

void AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::WriteLength(
  unsigned int input, List<unsigned char>& output, int offset
) {
  if (offset >= output.size) {
    int oldSize = output.size;
    output.SetSize(offset + 1);
    for (int i = oldSize; i < offset; i ++) {
      output[i] = 0;
    }
  }
 if (input < 128) {
    unsigned char length = static_cast<unsigned char>(input);
    output[offset] = length;
    return;
  }
  int numBytes = AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength
  ::GetReservedBytesForLength(static_cast<signed>(input)) - 1;
  unsigned char lengthPlus128 = 128 + static_cast<unsigned char>(numBytes);
  output[offset] = lengthPlus128;
  offset ++;
  Serialization::WriteNByteUnsigned(
    numBytes,
    static_cast<unsigned int>(input),
    output,
    offset
  );
}

bool ASNElement::isEmpty() const {
  return
    this->startByte == AbstractSyntaxNotationOneSubsetDecoder::tags::reserved0 &&
    this->ASNAtom.size == 0;
}

bool ASNObject::isEmpty() const {
  return this->objectId.isEmpty() && this->content.isEmpty();
}

std::string AbstractSyntaxNotationOneSubsetDecoder::ToStringAnnotateBinary() {
  if (!this->CheckInitialization()) {
    return "ASN1 not initialized properly. ";
  }
  std::stringstream out;
  out << "<script>";
  out << "window.calculator.crypto.abstractSyntaxNotationAnnotate(";
  out << "\"" << Crypto::ConvertListUnsignedCharsToHex(*this->rawDatA) << "\"";
  out << ", ";
  out << this->decodedData->ToString();
  out << ", ";
  List<unsigned char> idNonHexed;
  Crypto::computeSha256(*this->rawDatA, idNonHexed);
  std::string theID = Crypto::ConvertListUnsignedCharsToHex(idNonHexed);
  out << "\"" << theID << "\"";
  out << ");";
  out << "</script>";
  out << "<span id=\"" << theID << "\"></span>";
  return out.str();
}

int AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::GetReservedBytesForLength(int length) {
  int result = 1;
  if (length < 128) {
    return result;
  }
  while (length > 0) {
    length /= 256;
    result ++;
  }
  return result;
}

AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::WriterObjectFixedLength(
  unsigned char startByte,
  int expectedTotalElementByteLength,
  List<unsigned char>& output,
  int* outputTotalElementByteLength
) {
  if (expectedTotalElementByteLength < 0) {
    expectedTotalElementByteLength = 0;
  }
  this->outputPointer = &output;
  this->outputTotalByteLength = outputTotalElementByteLength;
  this->offset = output.size;
  this->outputPointer->AddOnTop(startByte);
  this->totalByteLength = expectedTotalElementByteLength;
  this->reservedBytesForLength = this->GetReservedBytesForLength(expectedTotalElementByteLength);
  for (int i = 0; i < this->reservedBytesForLength; i ++) {
    this->outputPointer->AddOnTop(0);
  }

}

AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::~WriterObjectFixedLength() {
  this->totalByteLength = this->outputPointer->size - this->offset - 1 - this->reservedBytesForLength;
  if (this->outputTotalByteLength != nullptr) {
    *this->outputTotalByteLength = this->totalByteLength;
  }
  int actualBytesNeededForLength = this->GetReservedBytesForLength(this->totalByteLength);
  if (actualBytesNeededForLength > this->reservedBytesForLength) {
    global << logger::red << "Wrong number of reserved bytes for sequence writer. "
    << "This is non-fatal but affects negatively performance. " << logger::endL;
    this->outputPointer->ShiftUpExpandOnTopRepeated(
      this->offset + 1, actualBytesNeededForLength - this->reservedBytesForLength
    );
  }
  if (actualBytesNeededForLength < this->reservedBytesForLength) {
    global << logger::red << "Wrong number of reserved bytes for sequence writer. "
    << "This is non-fatal but affects negatively performance. " << logger::endL;
    this->outputPointer->RemoveIndicesShiftDown(
      this->offset + 1, this->reservedBytesForLength - actualBytesNeededForLength
    );
  }
  this->reservedBytesForLength = actualBytesNeededForLength;
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::WriteLength(
    static_cast<unsigned>(this->totalByteLength),
    *this->outputPointer,
    this->offset + 1
  );
}

void AbstractSyntaxNotationOneSubsetDecoder::WriteUnsignedIntegerObject(
  const LargeIntegerUnsigned& input, List<unsigned char>& output
) {
  List<unsigned char> serialized;
  input.WriteBigEndianBytes(serialized, true);
  ASNElement::WriteBytesASNAtom(
    AbstractSyntaxNotationOneSubsetDecoder::tags::integer0x02,
    serialized,
    output
  );
}

void AbstractSyntaxNotationOneSubsetDecoder::WriteObjectId(
  const List<unsigned char>& input, List<unsigned char>& output
) {
  ASNElement::WriteBytesASNAtom(
    AbstractSyntaxNotationOneSubsetDecoder::tags::objectIdentifier0x06,
    input,
    output
  );
}

void ASNObject::initializeAddSample(
  MapList<std::string, ASNObject, MathRoutines::HashString>& container,
  const std::string& inputName,
  const std::string& inputObjectIdHex,
  unsigned char inputContentTag
) {
  ASNObject incoming;
  incoming.name = inputName;
  incoming.content.startByte = inputContentTag;
  incoming.content.ComputeTag();
  std::stringstream commentsOnFailure;
  incoming.objectId.startByte = AbstractSyntaxNotationOneSubsetDecoder::tags::objectIdentifier0x06;
  incoming.objectId.ComputeTag();
  if (!Crypto::ConvertHexToListUnsignedChar(
    inputObjectIdHex, incoming.objectId.ASNAtom, &commentsOnFailure
  )) {
    global.fatal << "Failure in certificate field initialization is not allowed. " << global.fatal;
  }
  container.SetKeyValue(incoming.name, incoming);
}

MapList<List<unsigned char>, ASNObject, MathRoutines::HashListUnsignedChars>& ASNObject::ObjectIdsToNames() {
  static MapList<List<unsigned char>, ASNObject, MathRoutines::HashListUnsignedChars> result;
  return result;
}

std::string ASNObject::names::sha1                    = "sha1"                   ;
std::string ASNObject::names::sha256                  = "sha256"                 ;
std::string ASNObject::names::sha384                  = "sha384"                 ;
std::string ASNObject::names::sha512                  = "sha512"                 ;
std::string ASNObject::names::sha256WithRSAEncryption = "sha256WithRSAEncryption";
std::string ASNObject::names::RSAEncryption           = "RSAEncryption"          ;
std::string ASNObject::names::countryName             = "countryName"            ;
std::string ASNObject::names::stateOrProvinceName     = "stateOrProvinceName"    ;
std::string ASNObject::names::localityName            = "localityName"           ;
std::string ASNObject::names::organizationName        = "organizationName"       ;
std::string ASNObject::names::organizationalUnitName  = "organizationalUnitName" ;
std::string ASNObject::names::commonName              = "commonName"             ;
std::string ASNObject::names::emailAddress            = "emailAddress"           ;
std::string ASNObject::names::basicConstraints        = "basicConstraints"       ;
std::string ASNObject::names::subjectKeyIdentifier    = "subjectKeyIdentifier"   ;
std::string ASNObject::names::authorityKeyIdentifier  = "authorityKeyIdentifier" ;

MapList<std::string, ASNObject, MathRoutines::HashString>& ASNObject::NamesToObjectIdsNonThreadSafe() {
  static MapList<std::string, ASNObject, MathRoutines::HashString> container;
  // Object ids of some hash functions are given in RFC4055.
  // Object ids of hash functions may be deduced from [Page 42, RFC 3447]
  if (container.size() != 0) {
    return container;
  }
  ASNObject::initializeAddSample(
    container,
    ASNObject::names::sha1,
    "2b0e03021a",
    AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05
  );
  ASNObject::initializeAddSample(
    container,
    ASNObject::names::sha256,
    "608648016503040201",
    AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05
  );
  ASNObject::initializeAddSample(
    container,
    ASNObject::names::sha384,
    "608648016503040202",
    AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05
  );
  ASNObject::initializeAddSample(
    container,
    ASNObject::names::sha512,
    "608648016503040203",
    AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05
  );


  ASNObject::initializeAddSample(
    container,
    ASNObject::names::sha256WithRSAEncryption ,
    "2a864886f70d01010b",
    AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05
  );
  ASNObject::initializeAddSample(
    container,
    ASNObject::names::RSAEncryption ,
    "2a864886f70d010101",
    AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05
  );
  ASNObject::initializeAddSample(
    container,
    ASNObject::names::countryName,
    "550406",
    AbstractSyntaxNotationOneSubsetDecoder::tags::printableString0x13
  );
  ASNObject::initializeAddSample(
    container,
    ASNObject::names::stateOrProvinceName,
    "550408",
    AbstractSyntaxNotationOneSubsetDecoder::tags::utf8String0x0c
  );
  ASNObject::initializeAddSample(
    container,
    ASNObject::names::localityName,
    "550407",
    AbstractSyntaxNotationOneSubsetDecoder::tags::utf8String0x0c
  );
  ASNObject::initializeAddSample(
    container,
    ASNObject::names::organizationName,
    "55040a",
    AbstractSyntaxNotationOneSubsetDecoder::tags::utf8String0x0c
  );
  ASNObject::initializeAddSample(
    container,
    ASNObject::names::organizationalUnitName,
    "55040b",
    AbstractSyntaxNotationOneSubsetDecoder::tags::utf8String0x0c
  );
  ASNObject::initializeAddSample(
    container,
    ASNObject::names::commonName,
    "550403",
    AbstractSyntaxNotationOneSubsetDecoder::tags::utf8String0x0c
  );
  ASNObject::initializeAddSample(
    container,
    ASNObject::names::emailAddress,
    "2a864886f70d010901",
    AbstractSyntaxNotationOneSubsetDecoder::tags::utf8String0x0c
  );
  ASNObject::initializeAddSample(
    container,
    ASNObject::names::subjectKeyIdentifier,
    "551d0e",
    AbstractSyntaxNotationOneSubsetDecoder::tags::octetString0x04
  );
  ASNObject::initializeAddSample(
    container,
    ASNObject::names::authorityKeyIdentifier,
    "551d23",
    AbstractSyntaxNotationOneSubsetDecoder::tags::octetString0x04
  );
  ASNObject::initializeAddSample(
    container,
    ASNObject::names::basicConstraints,
    "551d13",
    AbstractSyntaxNotationOneSubsetDecoder::tags::boolean0x01
  );
  MapList<List<unsigned char>, ASNObject, MathRoutines::HashListUnsignedChars>& reverseMap =
  ASNObject::ObjectIdsToNames();
  for (int i = 0; i < container.theValues.size; i ++) {
    ASNObject& current = container.theValues[i];
    reverseMap.SetKeyValue(current.objectId.ASNAtom, current);
  }
  return container;
}

int ASNObject::LoadField(
  const MapList<std::string, ASNObject, MathRoutines::HashString>& inputFields, const std::string& fieldName
) {
  if (!ASNObject::NamesToObjectIdsNonThreadSafe().Contains(fieldName)) {
    global.fatal << "Field " << fieldName << " is hard-coded but is yet unknown. " << global.fatal;
  }
  if (!inputFields.Contains(fieldName)) {
    this->name = fieldName;
    this->objectId.ASNAtom.SetSize(0);
    return 0;
  }
  *this = inputFields.GetValueConstCrashIfNotPresent(fieldName);
  return 1;
}

std::string ASNObject::ToString() const {
  std::stringstream out;
  out << "objectId: " << Crypto::ConvertListUnsignedCharsToHex(this->objectId.ASNAtom)
  << ", name: " << Crypto::ConvertStringToHex(this->name, 0, false);
  std::string content = this->content.ASNAtom.ToStringConcatenate();
  out << ", content: " << StringRoutines::ConvertStringToHexIfNonReadable(content, 0, false);
  return out.str();
}

std::string ASNObject::ToStringAllRecognizedObjectIds() {
  MacroRegisterFunctionWithName("ASNObject::ToStringAllRecognizedObjectIds");
  // This function is safe after the first run, which should have already happened, unless we are unit-testing.
  ASNObject::NamesToObjectIdsNonThreadSafe();
  std::stringstream out;
  out << "<table>";
  for (int i = 0; i < ASNObject::ObjectIdsToNames().size(); i ++) {
    ASNObject& current = ASNObject::ObjectIdsToNames().theValues[i];
    const List<unsigned char>& currentId = ASNObject::ObjectIdsToNames().theKeys[i];
    out << "<tr>"
    << "<td><small>" << i + 1 << "</small></td>"
    << "<td>" << Crypto::ConvertListUnsignedCharsToHexFormat(currentId, 50, false) << "</td>"
    << "<td>" << current.name << "</td>"
    << "</tr>";
  }
  out << "</table>";
  return out.str();
}

const List<unsigned char>& ASNObject::ObjectIdFromNameNoFail(const std::string& input) {
  return ASNObject::NamesToObjectIdsNonThreadSafe().GetValueConstCrashIfNotPresent(input).objectId.ASNAtom;
}

bool ASNObject::LoadFieldsFromASNSequence(
  const ASNElement& input,
  MapList<std::string, ASNObject, MathRoutines::HashString>& output,
  std::stringstream *commentsOnFailure
) {
  MacroRegisterFunctionWithName("ASNObject::LoadFieldsFromASNSequence");
  output.Clear();
  if (input.tag != AbstractSyntaxNotationOneSubsetDecoder::tags::sequence0x10) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Input is not array. ";
    }
    return false;
  }
  for (int i = 0; i < input.theElements.size; i ++) {
    ASNObject current;
    if (!current.LoadFromASN(input.theElements[i], commentsOnFailure)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Failed to load certificate entry index: " << i << ". ";
      }
      return false;
    }
    output.SetKeyValue(current.name, current);
  }
  return true;

}

std::string ASNElement::GetType() const {
  return AbstractSyntaxNotationOneSubsetDecoder::GetType(this->tag);
}

bool ASNObject::LoadFromASN(
  const ASNElement& input,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("ASNObject::LoadFromASN");
  if (
    input.tag != AbstractSyntaxNotationOneSubsetDecoder::tags::set0x11 ||
    input.theElements.size != 1
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "ASNObject outer layer must be a one-element set, instead is of type: "
      << input.GetType() << " and has " << input.theElements.size << " elements. ";
    }
    return false;
  }
  const ASNElement& internal = input.theElements[0];

  if (
    internal.tag != AbstractSyntaxNotationOneSubsetDecoder::tags::sequence0x10 ||
    internal.theElements.size != 2
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "ASNObject JSON representation must be a two-element sequence. ";
    }
    return false;
  }
  this->objectId = internal.theElements[0];
  if (this->objectId.tag != AbstractSyntaxNotationOneSubsetDecoder::tags::objectIdentifier0x06) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Missing objectIdentifierBytes key. ";
    }
    return false;
  }
  if (!ASNObject::ObjectIdsToNames().Contains(this->objectId.ASNAtom)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Unrecognized object id. " << "Total known ids: " << ASNObject::ObjectIdsToNames().theKeys.size;
    }
    return false;
  }
  *this = ASNObject::ObjectIdsToNames().GetValueCreateNoInit(this->objectId.ASNAtom);
  const ASNElement& second = internal.theElements[1];
  if (second.isComposite()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "ASN object content is required to be non-composite. ";
    }
    return false;
  }
  this->content = second;
  return true;
}

void ASNObject::initializeNonThreadSafe() {
  MacroRegisterFunctionWithName("ASNObject::initializeNonThreadSafe");
  ASNObject::NamesToObjectIdsNonThreadSafe();
}

void ASNElement::MakeObjectId(const List<unsigned char>& input) {
  this->reset();
  this->tag = AbstractSyntaxNotationOneSubsetDecoder::tags::objectIdentifier0x06;
  this->startByte = this->tag;
  this->ASNAtom = input;
}

void ASNElement::MakeInteger(const LargeIntegerUnsigned& input) {
  this->reset();
  this->tag = AbstractSyntaxNotationOneSubsetDecoder::tags::integer0x02;
  this->startByte = AbstractSyntaxNotationOneSubsetDecoder::tags::integer0x02;
  input.WriteBigEndianBytes(this->ASNAtom, true);
}

void ASNElement::MakeNull() {
  this->reset();
  this->tag = AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05;
  this->startByte = AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05;
}

void ASNElement::MakeSet(
  int numberOfEmptyElements, bool setLeadingBit, bool setSecondMostSignificantBit, bool constructed
) {
  this->reset();
  this->tag = AbstractSyntaxNotationOneSubsetDecoder::tags::set0x11;
  this->startByte = this->tag;
  this->SetStartByteFlags(setLeadingBit, setSecondMostSignificantBit, constructed);
  this->theElements.SetSize(numberOfEmptyElements);
}

void ASNElement::MakeSequence(const List<ASNElement>& input) {
  this->MakeSequence(input.size);
  this->theElements = input;
}

void ASNElement::MakeSequence(int numberOfEmptyElements) {
  this->reset();
  this->tag = AbstractSyntaxNotationOneSubsetDecoder::tags::sequence0x10;
  this->startByte = this->tag + 32;
  this->theElements.SetSize(numberOfEmptyElements);
}

void TBSCertificateInfo::ComputeASNSignatureAlgorithmIdentifier(ASNElement& output) {
  output.MakeSequence(2);
  output[0] = this->signatureAlgorithmIdentifier;
  output[1].MakeNull();
}

void TBSCertificateInfo::ComputeASNValidityWrapper(ASNElement& output) {
  output.MakeSequence(2);
  output[0] = this->validityNotBefore;
  output[0].comment = "Validity not before";
  output[1] = this->validityNotAfter;
  output[1].comment = "Validity not after";
}

void TBSCertificateInfo::ComputeASNVersionWrapper(ASNElement& output) {
  output.reset();
  output.tag = 0;
  output.startByte = 160;
  ASNElement versionInteger;
  versionInteger.MakeInteger(this->version);
  output.theElements.AddOnTop(versionInteger);
}

void ASNElement::SetStartByteFlags(
  bool setLeadingBit, bool setSecondMostSignificantBit, bool setConstructed
) {
  if (setLeadingBit) {
    this->startByte += 128;
  }
  if (setSecondMostSignificantBit) {
    this->startByte += 64;
  }
  if (setConstructed) {
    this->startByte += 32;
  }
}

void ASNElement::MakeBitStringEmpty(
  bool setLeadingBit, bool setSecondMostSignificantBit, bool setConstructed
) {
  this->reset();
  this->tag = AbstractSyntaxNotationOneSubsetDecoder::tags::bitString0x03;
  this->startByte = this->tag;
  this->SetStartByteFlags(setLeadingBit, setSecondMostSignificantBit, setConstructed);
  this->ASNAtom.SetSize(0);
  this->theElements.SetSize(0);
}

void ASNElement::MakeBitStrinG(const List<unsigned char>& input) {
  this->reset();
  this->startByte = AbstractSyntaxNotationOneSubsetDecoder::tags::bitString0x03;
  this->tag = this->startByte;
  this->ASNAtom = input;
}

void ASNElement::MakeOctetString(const List<unsigned char>& input) {
  this->reset();
  this->startByte = AbstractSyntaxNotationOneSubsetDecoder::tags::octetString0x04;
  this->tag = this->startByte;
  this->ASNAtom = input;
}

void TBSCertificateInfo::ComputeASNSignature(ASNElement& output) {
  output.MakeSequence(2);
  output.comment = "signature";
  output[0].MakeSequence(2);
  output[0][0].MakeObjectId(ASNObject::ObjectIdFromNameNoFail(ASNObject::names::RSAEncryption));
  output[0][1].MakeNull();
  output[1].MakeBitStringEmpty(false, false, false);
  output[1].theElements.SetSize(1);
  ASNElement& signatureSerializer = output[1][0];
  signatureSerializer.MakeSequence(2);
  signatureSerializer[0].MakeInteger(this->subjectPublicKey.theExponent);
  signatureSerializer[1].MakeInteger(this->subjectPublicKey.theModulus);
  signatureSerializer.WriteBytesUpdatePromisedLength(output[1].ASNAtom);
  output[1].lengthPromised = output[1].ASNAtom.size;
  output[1].flagHeaderPadded = true;
}

void TBSCertificateInfo::ComputeASN(ASNElement& output) {
  output.MakeSequence(8);
  this->ComputeASNVersionWrapper(output[0]);
  output[1].MakeInteger(this->serialNumber);
  output[1].comment = "serial number";
  this->ComputeASNSignatureAlgorithmIdentifier(output[2]);
  output[2].comment = "algorithm identifier";
  this->issuer.ComputeASN(output[3]);
  output[3].comment = "Issuer [entity that signed this certificate]";
  this->ComputeASNValidityWrapper(output[4]);
  output[4].comment = "Validity";
  this->subject.ComputeASN(output[5]);
  output[5].comment = "Subject [entity advertising its public key identity]";
  this->ComputeASNSignature(output[6]);
  this->ComputeASNExtensions(output[7]);
}

void ASNElement::MakeBitStringSequence(const List<ASNElement>& input) {
  this->MakeSequence(input);
  this->tag = AbstractSyntaxNotationOneSubsetDecoder::tags::bitString0x03;
  this->startByte = this->tag + 128 + 32;
  for (int i = 0; i < this->theElements.size; i ++) {
    this->theElements[i].WriteBytesUpdatePromisedLength(this->ASNAtom);
  }
  this->lengthPromised = this->ASNAtom.size;
}

void TBSCertificateInfo::ComputeASNExtensions(ASNElement& output) {
  output.MakeBitStringSequence(this->extensions);
}

void ASNObject::ComputeASN(ASNElement& output) {
  output.MakeSet(1, false, false, true);
  output[0].MakeSequence(2);
  output[0][0] = this->objectId;
  output[0][1] = this->content;
  std::stringstream comment;
  comment << "Object: " << this->name << ", interpretation: " << this->content.ASNAtom.ToStringConcatenate();
  output[0][0].comment = comment.str();
  output[0][1].comment = comment.str();
}

void TBSCertificateInfo::Organization::ComputeASN(ASNElement& output) {
  output.MakeSequence(0);
  List<ASNObject*> serializationOrder({
    &this->countryName,
    &this->stateOrProvinceName,
    &this->localityName,
    &this->organizationName,
    &this->organizationalUnitName,
    &this->commonName,
    &this->emailAddress
  });
  ASNElement next;
  for (int i = 0; i < serializationOrder.size; i ++) {
    ASNObject* current = serializationOrder[i];
    if (current->isEmpty()) {
      continue;
    }
    current->ComputeASN(next);
    output.theElements.AddOnTop(next);
  }
}

void X509Certificate::ComputeASN(ASNElement& output) {
  output.reset();
  output.MakeSequence(3);
  output.comment = "X509 certificate";
  this->information.ComputeASN(output[0]);
  this->ComputeASNSignatureAlgorithm(output[1]);
  output[2] = this->signatureValue;
}

void X509Certificate::ComputeASNSignatureAlgorithm(ASNElement& output) {
  output.MakeSequence(2);
  output[0] = this->signatureAlgorithmId;
  output[1].MakeNull();
}

void X509Certificate::WriteBytesASN1(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) {
  MacroRegisterFunctionWithName("X509Certificate::WriteBytesASN1");
  this->ComputeASN(this->recodedASN);
  this->recodedASN.WriteBytesUpdatePromisedLength(output);
  if (annotations != nullptr) {
    this->recodedASN.WriteAnnotations(*annotations);
  }
}

std::string X509Certificate::ToStringTestEncode() {
  std::stringstream out;
  std::string sourceHex = Crypto::ConvertListUnsignedCharsToHex(this->sourceBinary);
  List<unsigned char> recoded;
  this->WriteBytesASN1(recoded, nullptr);
  std::string recodedHex = Crypto::ConvertListUnsignedCharsToHex(recoded);
  out << "Original, recoded binary source:<br>"
  << StringRoutines::Differ::DifferenceHTMLStatic(sourceHex, recodedHex, "sourceHex", "recodedHex");
  return out.str();
}

std::string X509Certificate::ToHex() {
  List<unsigned char> bytes;
  this->WriteBytesASN1(bytes, nullptr);
  return Crypto::ConvertListUnsignedCharsToHex(bytes);
}

std::string X509Certificate::ToString() {
  std::stringstream out;
  out << "Certificate pub key:<br>"
  << this->information.subjectPublicKey.ToString();
  out << this->information.ToString();
  AbstractSyntaxNotationOneSubsetDecoder decoderRecoded;
  decoderRecoded.decodedData = &this->recodedASN;
  List<unsigned char> recodedBytes;
  this->recodedASN.WriteBytesUpdatePromisedLength(recodedBytes);
  decoderRecoded.rawDatA = &recodedBytes;
  out << "<br><b>Recoded (" << recodedBytes.size << " bytes).</b><br>";
  out << decoderRecoded.ToStringAnnotateBinary();

  out << "<br><b>Source (" << this->sourceBinary.size << " bytes).</b><br>";
  if (this->sourceBinary == recodedBytes) {
    out << "<b style = 'color:green'>Recoded and original source coincide. </b>";
  } else {
    AbstractSyntaxNotationOneSubsetDecoder decoderSource;
    decoderSource.decodedData = &this->sourceASN;
    decoderSource.rawDatA = &this->sourceBinary;
    out << decoderSource.ToStringAnnotateBinary();
  }
  return out.str();
}

std::string PrivateKeyRSA::ToString() const {
  std::stringstream out;
  out << "Exponent: " << this->thePublicKey.theExponent << "<br>Prime 1: "
  << this->primeOne << "<br>Prime 2: " << this->primeTwo;
  return out.str();
}

bool PrivateKeyRSA::BasicChecks(std::stringstream* comments) {
  std::stringstream commentsContainer;
  if (comments == nullptr) {
    comments = &commentsContainer;
  }
  LargeInteger mustBeZero;
  LargeInteger primeOneLI = this->primeOne;
  LargeInteger primeTwoLI = this->primeTwo;
  mustBeZero = this->primeOne;
  mustBeZero *= this->primeTwo;
  mustBeZero -= this->thePublicKey.theModulus;
  *comments << "Must be zero: " << mustBeZero.ToString();
  LargeInteger EulerPhi = (primeOneLI - 1) * (primeTwoLI - 1);
  ElementZmodP mustBeZeroModP;
  //theExponent.theModulo = EulerPhi.value;
  //theExponent.theValue = this->exponent;
  mustBeZeroModP.theModulus = EulerPhi.value;
  mustBeZeroModP.theValue = 1;
  mustBeZeroModP /= this->thePublicKey.theExponent;
  mustBeZeroModP -= this->privateExponent;
  *comments << "<br>Difference between private exponent and computed one: " << mustBeZeroModP.theValue.ToString();
  return true;
}

bool PrivateKeyRSA::LoadFromPEMFile(const std::string& input, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("PrivateKeyRSA::LoadFromPEMFile");
  std::string certificateContent;
  // No access to sensitive folders here, so this cannot be used for the server's private key.
  // For server's certificate, use TransportLayerSecurity::LoadPEMPrivateKey.
  if (!FileOperations::LoadFileToStringVirtual(input, certificateContent, false, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load key file. ";
    }
    return false;
  }
  return this->LoadFromPEM(certificateContent, commentsOnFailure);
}

bool PrivateKeyRSA::LoadFromPEM(const std::string& input, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("PrivateKeyRSA::LoadFromPEM");
  std::string certificateContentStripped = StringRoutines::StringTrimWhiteSpace(input);
  if (!StringRoutines::StringBeginsWith(
    certificateContentStripped,
    "-----BEGIN PRIVATE KEY-----",
    &certificateContentStripped
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad private key start. ";
    }
    return false;
  }
  if (!StringRoutines::StringEndsWith(
    certificateContentStripped,
    "-----END PRIVATE KEY-----",
    &certificateContentStripped
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad private key end. ";
    }
    return false;
  }
  certificateContentStripped = StringRoutines::StringTrimWhiteSpace(certificateContentStripped);
  if (!Crypto::ConvertBase64ToBitStream(
    certificateContentStripped,
    this->sourceBinary,
    commentsOnFailure,
    nullptr
  )) {
    return false;
  }
  return this->LoadFromASNEncoded(this->sourceBinary, commentsOnFailure);
}

// Notes:
// https://tools.ietf.org/html/rfc3447#page-23
// Algorithm:
// https://tools.ietf.org/html/rfc3447#page-29
void PrivateKeyRSA::SignBytesPadPKCS1(
  List<unsigned char>& input,
  int hash,
  List<unsigned char>& output
) {
  MacroRegisterFunctionWithName("PrivateKeyRSA::SignBytesPadPKCS1");
  List<unsigned char> inputHashedPadded;
  this->HashAndPadPKCS1(input, hash, inputHashedPadded);
  ElementZmodP theElement, theOne;
  if (this->thePublicKey.theModulus.IsEqualToZero()) {
    global.fatal << "Public key modulus is zero. " << global.fatal;
  }
  theElement.theModulus = this->thePublicKey.theModulus;
  theOne.MakeOne(this->thePublicKey.theModulus);
  Crypto::ConvertListUnsignedCharsToLargeUnsignedIntegerBigEndian(
    inputHashedPadded, theElement.theValue
  );
  MathRoutines::RaiseToPower(theElement, this->privateExponent, theOne);
  output.SetSize(0);
  theElement.theValue.WriteBigEndianBytes(output, false);
}

PrivateKeyRSA::PrivateKeyRSA() {
  this->bitSize = 0;
  this->byteSize = 0;
}

void PrivateKeyRSA::ComputeBitSize() {
  this->bitSize = static_cast<signed>(this->thePublicKey.theModulus.LogarithmBaseNCeiling(2));
  this->byteSize = this->bitSize / 8;
  if (this->byteSize * 8 < this->bitSize) {
    this->byteSize ++;
  }
}

bool PrivateKeyRSA::ComputeFromTwoPrimes(
  LargeIntegerUnsigned& inputPrimeOne,
  LargeIntegerUnsigned& inputPrimeTwo,
  bool verifyInputsArePrime,
  std::stringstream* commentsOnFailure
) {
  this->primeOne = inputPrimeOne;
  this->primeTwo = inputPrimeTwo;
  if (verifyInputsArePrime) {
    if (!this->primeOne.IsPossiblyPrime(10, true, commentsOnFailure)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "First input: " << this->primeTwo.ToString() << " is not prime. ";
      }
      return false;
    }
    if (!this->primeTwo.IsPossiblyPrime(10, true, commentsOnFailure)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Second input: " << this->primeTwo.ToString() << " is not prime. ";
      }
      return false;
    }
  }
  this->thePublicKey.theExponent.AssignUnsignedInt(this->thePublicKey.defaultExponent);
  this->thePublicKey.theModulus = this->primeOne * this->primeTwo;
  this->CarmichaelTotientOfModulus = MathRoutines::lcm(this->primeOne - 1, this->primeTwo - 1);
  ElementZmodP inverter;
  inverter.theModulus = this->CarmichaelTotientOfModulus;
  inverter.theValue = 1;
  inverter /= this->thePublicKey.theExponent;
  this->privateExponent = inverter.theValue;
  this->ComputeBitSize();
  return true;
}

bool PrivateKeyRSA::GenerateRandom(std::stringstream* commentsOnFailure, int numberOfBytes) {
  if (numberOfBytes < 1 || numberOfBytes > 1000) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Number of bytes must be "
      << "between 1 and 1000; you requested: " << numberOfBytes << ". ";
    }
    return false;
  }
  Crypto::Random::GetRandomLargePrime(this->primeOne, numberOfBytes);
  Crypto::Random::GetRandomLargePrime(this->primeTwo, numberOfBytes);
  return this->ComputeFromTwoPrimes(this->primeOne, this->primeTwo, false, commentsOnFailure);
}

void PrivateKeyRSA::HashAndPadPKCS1(
  List<unsigned char>& input, int hash, List<unsigned char>& output
) {
  List<unsigned char> inputHashed;
  ASNElement encoder;
  encoder.MakeSequence(2);
  ASNObject hashObject;
  switch (hash) {
  case SignatureAlgorithmSpecification::HashAlgorithm::sha256:
    Crypto::computeSha256(input, inputHashed);
    hashObject = ASNObject::NamesToObjectIdsNonThreadSafe().GetValueConstCrashIfNotPresent(ASNObject::names::sha256);
    break;
  default:
    global.fatal << "Non-allowed or non-implemented value for the hash algorithm. " << global.fatal;
  }
  encoder.theElements[1].MakeOctetString(inputHashed);
  hashObject.ComputeASN(encoder.theElements[0]);
  List<unsigned char> payload;
  encoder.WriteBytesUpdatePromisedLength(payload);
  int numberOfFFs = this->byteSize - payload.size - 3;
  if (numberOfFFs < 0) {
    numberOfFFs = 0;
  }
  output.SetSize(0);
  output.AddOnTop(0x00);
  output.AddOnTop(0x01);
  for (int i = 0; i < numberOfFFs; i ++) {
    output.AddOnTop(0xff);
  }
  output.AddOnTop(0x00);
  output.AddListOnTop(payload);
}

bool PrivateKeyRSA::LoadFromASNEncoded(
  List<unsigned char>& input, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("PrivateKeyRSA::LoadFromASNEncoded");
  AbstractSyntaxNotationOneSubsetDecoder outerDecoder, innerDecoder;
  if (!outerDecoder.Decode(
    input, 0, this->sourceASNOuter, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to asn-decode private key outer data. ";
    }
    return false;
  }
  const ASNElement* innerData;
  if (!this->sourceASNOuter.HasSubElementConst(
    {2}, {}, &innerData, commentsOnFailure
  )) {
    return false;
  }
  if (!innerDecoder.Decode(innerData->ASNAtom, 0, this->sourceASNInner, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to asn-decode private key inner data. ";
    }
    return false;
  }
  if (!this->sourceASNInner.HasSubElementOfType(
    {1}, {}, this->thePublicKey.theModulus, commentsOnFailure
  )) {
    return false;
  }
  if (!this->sourceASNInner.HasSubElementOfType(
    {2}, {}, this->thePublicKey.theExponent, commentsOnFailure
  )) {
    return false;
  }
  if (!this->sourceASNInner.HasSubElementOfType(
    {3}, {}, this->privateExponent, commentsOnFailure
  )) {
    return false;
  }
  if (!this->sourceASNInner.HasSubElementOfType(
    {4}, {}, this->primeOne, commentsOnFailure
  )) {
    return false;
  }
  if (!this->sourceASNInner.HasSubElementOfType(
    {5}, {}, this->primeTwo, commentsOnFailure
  )) {
    return false;
  }
  LargeInteger exponent1, exponent2, coefficient;
  if (!this->sourceASNInner.HasSubElementOfType(
    {6}, {}, exponent1, commentsOnFailure
  )) {
    return false;
  }
  if (!this->sourceASNInner.HasSubElementOfType(
    {7}, {}, exponent2, commentsOnFailure
  )) {
    return false;
  }
  if (!this->sourceASNInner.HasSubElementOfType(
    {8}, {}, coefficient, commentsOnFailure
  )) {
    return false;
  }
  return true;
}

bool X509Certificate::LoadFromPEM(const std::string& input, std::stringstream *commentsOnFailure) {
  std::string certificateContentStripped;
  //see ASN1_item_d2i_bio for decoding.
  certificateContentStripped = StringRoutines::StringTrimWhiteSpace(input);
  std::string beginCertificate = "-----BEGIN CERTIFICATE-----";
  std::string endCertificate = "-----END CERTIFICATE-----";
  if (!StringRoutines::StringBeginsWith(
    certificateContentStripped, beginCertificate, &certificateContentStripped
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad certificate start. ";
    }
    return false;
  }
  if (!StringRoutines::StringEndsWith(
    certificateContentStripped, endCertificate, &certificateContentStripped
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad certificate end. ";
    }
    return false;
  }
  certificateContentStripped = StringRoutines::StringTrimWhiteSpace(certificateContentStripped);
  if (!Crypto::ConvertBase64ToBitStream(
    certificateContentStripped, this->sourceBinary, commentsOnFailure, nullptr
  )) {
    return false;
  }
  return this->LoadFromASNEncoded(this->sourceBinary, commentsOnFailure);
}

bool X509Certificate::LoadFromPEMFile(const std::string& input, std::stringstream* commentsOnFailure) {
  std::string certificateContent;
  // No access to sensitive folders here, so this cannot be used for the server's certificate.
  // For server's certificate, use TransportLayerSecurity::LoadPEMCertificate.
  if (!FileOperations::LoadFileToStringVirtual(
    input, certificateContent, false, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load key file. ";
    }
    return false;
  }
  return this->LoadFromPEM(certificateContent, commentsOnFailure);
}

std::string TBSCertificateInfo::Organization::ToString() {
  std::stringstream out;
  out << "Country name: " << this->countryName.content.ToString() << "\n<br>\n";
  out << "Common name: " << this->commonName.content.ToString() << "\n<br>\n";
  out << "Email address: " << this->emailAddress.content.ToString() << "\n<br>\n";
  out << "Locality name: " << this->localityName.content.ToString() << "\n<br>\n";
  out << "Organizational unit name: " << this->organizationalUnitName.content.ToString() << "\n<br>\n";
  out << "Organization name: " << this->organizationName.content.ToString() << "\n<br>\n";
  out << "State or province name: " << this->stateOrProvinceName.content.ToString() << "\n<br>\n";
  return out.str();
}

std::string TBSCertificateInfo::ToString() {
  std::stringstream out;
  out << "<b>Issuer:</b><br>";
  out << this->issuer.ToString();
  out << "<b>Subject (holder of the advertised public key):</b><br>";
  out << this->subject.ToString();
  return out.str();
}

bool TBSCertificateInfo::LoadExtensions(
  const ASNElement& input, std::stringstream* commentsOnFailure
) {
  this->extensions.SetSize(0);
  if (input.tag != AbstractSyntaxNotationOneSubsetDecoder::tags::bitString0x03) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "TBS certificate extensions are expected to be of type bit string. ";
    }
    return false;
  }
  this->extensions = input.theElements;
  return true;
}

bool TBSCertificateInfo::LoadValidity(
  const ASNElement& input, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("TBSCertificateInfo::LoadValidity");
  if (
    input.tag != AbstractSyntaxNotationOneSubsetDecoder::tags::sequence0x10 ||
    input.theElements.size != 2
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Validity element must be a sequence of two elements. ";
    }
    return false;
  }
  if (!input.theElements[0].isTime() || !input.theElements[1].isTime()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Validity elements must be of type time. ";
    }
    return false;
  }
  this->validityNotBefore = input.theElements[0];
  this->validityNotAfter = input.theElements[1];
  return true;
}

bool TBSCertificateInfo::Organization::LoadFromASN(
  const ASNElement& input, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("TBSCertificateInfo::LoadFieldsFromASN");
  MapList<std::string, ASNObject, MathRoutines::HashString> fields;
  if (!ASNObject::LoadFieldsFromASNSequence(input, fields, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read certificate fields. Certificate fields decoded: "
      << fields.ToStringHtml() << ". ";
    }
    return false;
  }
  return this->LoadFields(fields, commentsOnFailure);
}

bool TBSCertificateInfo::Organization::LoadFields(
  const MapList<std::string, ASNObject, MathRoutines::HashString>& fields,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("TBSCertificateInfo::LoadFields");
  int numberOfLoadedFields = 0;
  numberOfLoadedFields += this->commonName.LoadField(
    fields, ASNObject::names::commonName
  );
  numberOfLoadedFields += this->countryName.LoadField(
    fields, ASNObject::names::countryName
  );
  numberOfLoadedFields += this->emailAddress.LoadField(
    fields, ASNObject::names::emailAddress
  );
  numberOfLoadedFields += this->localityName.LoadField(
    fields, ASNObject::names::localityName
  );
  numberOfLoadedFields += this->organizationalUnitName. LoadField(
    fields, ASNObject::names::organizationalUnitName
  );
  numberOfLoadedFields += this->organizationName.LoadField(
    fields, ASNObject::names::organizationName
  );
  numberOfLoadedFields += this->stateOrProvinceName.LoadField(
    fields, ASNObject::names::stateOrProvinceName
  );
  if (numberOfLoadedFields < fields.size()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to recognize "
      << (fields.size() - numberOfLoadedFields)
      << " fields in your certificate. ";
    }
    return false;
  }
  return true;
}

bool TBSCertificateInfo::LoadASNAlgorithmIdentifier(
  const ASNElement &input,
  ASNElement& output,
  std::stringstream* commentsOnFailure
) {
  if (
    input.tag != AbstractSyntaxNotationOneSubsetDecoder::tags::sequence0x10 ||
    input.theElements.size != 2
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Algorithm identifier outer layer must be a sequence of two elements. ";
    }
    return false;
  }
  if (!input.HasSubElementGetCopy(
    {0}, {
      AbstractSyntaxNotationOneSubsetDecoder::tags::objectIdentifier0x06
    },
    output,
    commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to get signature algorithm id. ";
    }
    return false;
  }
  if (!input.HasSubElementConst(
    {1}, {
      AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05
    },
    nullptr,
    commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Signature algorithm id must end in null. ";
    }
    return false;
  }
  return true;
}

bool TBSCertificateInfo::Load(const ASNElement& input, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("TBSCertificateInfo::Load");
  if (input.theElements.size < 7) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Certificate ASN element needs at least 7 fields. ";
    }
    return false;
  }
  if (!input.HasSubElementOfType(
    {0, 0}, {}, this->version, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load version. ";
    }
    return false;
  }
  if (!input.HasSubElementOfType(
    {1}, {}, this->serialNumber, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read serial number. ";
    }
    return false;
  }
  if (!this->LoadASNAlgorithmIdentifier(
    input.theElements[2], this->signatureAlgorithmIdentifier, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load certificate signature algorithm identifier. ";
    }
    return false;
  }
  if (!this->issuer.LoadFromASN(input.theElements[3], commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load issuer (authority that signed the certificate). ";
    }
    return false;
  }
  if (!this->LoadValidity(input.theElements[4], commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load validity. ";
    }
    return false;
  }
  if (!this->subject.LoadFromASN(input.theElements[5], commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load subject (owner of advertised public key). ";
    }
    return false;
  }

  if (!input.HasSubElementOfType(
    {6, 1, 0, 1}, {}, this->subjectPublicKey.theModulus, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read RSA modulus. ";
    }
    return false;
  }
  if (!input.HasSubElementOfType(
    {6, 1, 0, 0}, {}, this->subjectPublicKey.theExponent, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read public key. ";
    }
    return false;
  }
  if (
    this->subjectPublicKey.theModulus == 0 ||
    this->subjectPublicKey.theExponent == 0
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Invalid RSA modulus or exponent. ";
    }
    return false;
  }
  if (input.theElements.size >= 8) {
    return this->LoadExtensions(input.theElements[7], commentsOnFailure);
  }
  return true;
}

bool X509Certificate::LoadFromASNEncoded(
  const List<unsigned char>& input, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("X509Certificate::LoadFromASNEncoded");
  AbstractSyntaxNotationOneSubsetDecoder theDecoder;
  if (!theDecoder.Decode(input, 0, this->sourceASN, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to asn-decode certificate input. ";
    }
    return false;
  }
  if (this->sourceASN.theElements.size != 3) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Certificate ASN needs exactly three elements. ";
    }
    return false;
  }
  if (!this->information.Load(this->sourceASN[0], commentsOnFailure)) {
    return false;
  }
  if (this->sourceASN[2].tag != AbstractSyntaxNotationOneSubsetDecoder::tags::bitString0x03) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Third element of certificate is required to be a bit string. ";
    }
    return false;
  }
  if (!this->information.LoadASNAlgorithmIdentifier(
    this->sourceASN[1], this->signatureAlgorithmId, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load signature algorithm id. ";
    }
    return false;
  }
  this->signatureValue = this->sourceASN[2];
  return true;
}

