#include "abstract_syntax_notation_one_decoder.h"
#include "crypto.h"
#include "math_extra_algebraic_numbers.h"
#include "math_general_implementation.h"
#include "serialization_basic.h"
#include "transport_layer_security.h"
#include "string_constants.h"

// Putting this in the header file currently breaks the linking for this
// particular tag.
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
  this->rawData = nullptr;
  this->decodedData = nullptr;
  this->dataPointer = 0;
}

AbstractSyntaxNotationOneSubsetDecoder::AbstractSyntaxNotationOneSubsetDecoder(
) {
  this->decodedData = nullptr;
  this->dataPointer = 0;
  this->recursionDepthGuard = 0;
  this->maxRecursionDepth = 20;
  this->flagMustDecodeAll = true;
}

AbstractSyntaxNotationOneSubsetDecoder::~AbstractSyntaxNotationOneSubsetDecoder
() {}

bool AbstractSyntaxNotationOneSubsetDecoder::checkInitialization() const {
  if (this->decodedData == nullptr) {
    global.fatal << "Uninitialized ASN1 output json. " << global.fatal;
  }
  if (this->rawData == nullptr) {
    global.fatal << "Uninitialized ASN1 raw data. " << global.fatal;
  }
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::decodeLengthIncrementDataPointer(
  ASNElement& output
) {
  if (!this->decodeLengthIncrementDataPointerNoCheck(output)) {
    return false;
  }
  if (output.lengthPromised + this->dataPointer > this->rawData->size) {
    std::stringstream errorStream;
    errorStream
    << "Element length: "
    << output.lengthPromised
    << " plus element body offset: "
    << this->dataPointer
    << " exceeds the total byte length: "
    << this->rawData->size
    << ". ";
    output.error = errorStream.str();
    return false;
  }
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::
decodeLengthIncrementDataPointerNoCheck(ASNElement& output) {
  output.lengthPromised = 0;
  int startDataPointer = this->dataPointer;
  unsigned char currentByte = (*this->rawData)[this->dataPointer];
  if (currentByte < 128) {
    // 128 = 0x80
    output.lengthPromised = currentByte;
    this->dataPointer ++;
    return true;
  }
  if (currentByte == 128) {
    // 128 = 0x80 signals varible-length encoding
    output.lengthPromised = - 1;
    this->dataPointer ++;
    return true;
  }
  int numberOfLengthBytes = currentByte - 128;
  LargeInteger length = 0;
  this->dataPointer ++;
  for (int i = 0; i < numberOfLengthBytes; i ++) {
    length *= 256;
    if (this->pointerIsBad(output)) {
      return false;
    }
    unsigned char nextByte = (*this->rawData)[this->dataPointer];
    length += static_cast<signed>(static_cast<unsigned int>(nextByte));
    this->dataPointer ++;
  }
  if (!length.isIntegerFittingInInt(&output.lengthPromised)) {
    std::stringstream errorStream;
    errorStream
    << "Variable length: "
    << length
    << " whose decoding started at position: "
    << startDataPointer
    << " with first excluded position: "
    << this->dataPointer
    << " is too large. ";
    output.error = errorStream.str();
    return false;
  }
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::pointerIsBad(
  ASNElement& outputError
) {
  if (this->dataPointer >= this->rawData->size || this->dataPointer < 0) {
    outputError.error =
    "Unexpected overflow error: data pointer is negative. ";
    return true;
  }
  return false;
}

bool AbstractSyntaxNotationOneSubsetDecoder::decodeSequenceLikeContent(
  ASNElement& output
) {
  int lastIndexPlusOne = this->dataPointer + output.lengthPromised;
  ASNElement nextElement;
  int numberOfDecoded = 0;
  // We reserve 32 bytes per object - a reasonable assumption.
  output.elements.reserve(output.lengthPromised / 32);
  while (this->dataPointer < lastIndexPlusOne) {
    int lastPointer = this->dataPointer;
    bool isGood = this->decodeCurrent(nextElement);
    if (!isGood) {
      std::stringstream errorStream;
      errorStream
      << "Failed to decode sequence element of index: "
      << numberOfDecoded
      << ". ";
      output.error = errorStream.str();
      return false;
    }
    if (lastPointer >= this->dataPointer) {
      global.fatal
      << "Decode current "
      << "did not increment the data pointer. "
      << global.fatal;
    }
    output.elements.addOnTop(nextElement);
    numberOfDecoded ++;
  }
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::decodeBitString(
  ASNElement& output
) {
  int offsetAtStart = this->dataPointer;
  this->decodeASNAtomContent(output);
  AbstractSyntaxNotationOneSubsetDecoder subDecoder;
  subDecoder.flagMustDecodeAll = false;
  ASNElement subDecoderResult;
  subDecoder.recursionDepthGuard = this->recursionDepthGuard + 1;
  bool insidePadded = false;
  if (offsetAtStart < this->rawData->size) {
    if ((*this->rawData)[offsetAtStart] == 0) {
      offsetAtStart ++;
      insidePadded = true;
    }
  }
  if (
    subDecoder.decode(
      *this->rawData, offsetAtStart, subDecoderResult, nullptr
    )
  ) {
    if (subDecoder.dataPointer == this->dataPointer) {
      output.elements.setSize(0);
      output.elements.addOnTop(subDecoderResult);
      if (insidePadded) {
        output.flagHeaderPadded = true;
      }
    }
  }
  return true;
}

void AbstractSyntaxNotationOneSubsetDecoder::decodeASNAtomContent(
  ASNElement& output
) {
  output.ASNAtom.setSize(output.lengthPromised);
  for (int i = 0; i < output.lengthPromised; i ++) {
    output.ASNAtom[i] = (*this->rawData)[this->dataPointer + i];
  }
  this->dataPointer += output.lengthPromised;
}

bool AbstractSyntaxNotationOneSubsetDecoder::decodeNull(ASNElement& output) {
  if (output.lengthPromised != 0) {
    output.error = "Length of null object is not zero. ";
    return false;
  }
  this->decodeASNAtomContent(output);
  return true;
}

LargeInteger AbstractSyntaxNotationOneSubsetDecoder::
variableLengthQuantityDecode(
  const List<unsigned char>& input, int& inputOutputDataPointer
) {
  LargeInteger result = 0;
  for (; inputOutputDataPointer < input.size; inputOutputDataPointer ++) {
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

std::string ASNElement::interpretAsObjectIdentifierGetNameAndId() const {
  std::stringstream out;
  out << "[" << this->interpretAsObjectIdentifier();
  out << "]: ";
  if (!ASNObject::objectIdsToNames().contains(this->ASNAtom)) {
    out << "[unknown]";
  } else {
    out
    << ASNObject::objectIdsToNames().getValueCreateEmpty(this->ASNAtom).name;
  }
  return out.str();
}

std::string ASNElement::interpretAsObjectIdentifier() const {
  STACK_TRACE("ASNElement::interpretAsObjectIdentifier");
  if (this->ASNAtom.size <= 0) {
    return "not an object id";
  }
  std::stringstream resultStream;
  unsigned char firstByte = this->ASNAtom[0];
  // first two entries are encoded in the first byte:
  unsigned char firstEntry = firstByte / 40;
  unsigned char secondEntry = firstByte % 40;
  resultStream << static_cast<int>(firstEntry) << "." << static_cast<int>(
    secondEntry
  );
  for (int i = 0; i < this->ASNAtom.size; i ++) {
    LargeInteger nextInt =
    AbstractSyntaxNotationOneSubsetDecoder::variableLengthQuantityDecode(
      this->ASNAtom, i
    );
    resultStream << "." << nextInt.toString();
  }
  return resultStream.str();
}

void ASNElement::writeBytesConst(List<unsigned char>& output) const {
  // WARNING: writer needs to be a named object to avoid
  // gcc immediately calling the object's destructor.
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength notAnonymous(
    this->startByte, this->lengthPromised, output, nullptr
  );
  if (this->flagHeaderPadded) {
    output.addOnTop(0);
  }
  if (this->ASNAtom.size > 0) {
    output.addListOnTop(this->ASNAtom);
  } else {
    for (int i = 0; i < this->elements.size; i ++) {
      this->elements[i].writeBytesConst(output);
    }
  }
}

void ASNElement::writeBytesUpdatePromisedLength(
  List<unsigned char>& output
) {
  this->offsetLastWrite = output.size;
  // WARNING: writer needs to be a named object to avoid
  // gcc immediately calling the object's destructor.
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength notAnonymous(
    this->startByte,
    this->lengthPromised,
    output,
    &this->lengthPromised
  );
  if (this->flagHeaderPadded) {
    output.addOnTop(0);
  }
  if (this->ASNAtom.size > 0) {
    output.addListOnTop(this->ASNAtom);
  } else {
    for (int i = 0; i < this->elements.size; i ++) {
      this->elements[i].writeBytesUpdatePromisedLength(output);
    }
  }
}

void ASNElement::writeBytesASNAtom(
  unsigned char inputStartByte,
  const List<unsigned char>& atomContent,
  List<unsigned char>& output
) {
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength notAnonymous(
    inputStartByte, atomContent.size, output, nullptr
  );
  output.addListOnTop(atomContent);
}

bool ASNElement::isComposite() const {
  return this->elements.size > 0;
}

bool ASNElement::isPureComposite() const {
  return this->elements.size > 0 && this->ASNAtom.size == 0;
}

bool ASNElement::isNonPureComposite() const {
  return this->elements.size > 0 && this->ASNAtom.size > 0;
}

ASNElement& ASNElement::operator[](int index) {
  return this->elements[index];
}

bool ASNElement::hasSubElementGetCopy(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  ASNElement& output,
  std::stringstream* commentsOnFailure
) const {
  const ASNElement* element = nullptr;
  if (
    !this->hasSubElementConst(
      desiredIndices, desiredTypes, &element, commentsOnFailure
    )
  ) {
    return false;
  }
  output = *element;
  return true;
}

bool ASNElement::hasSubElementConst(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  const ASNElement** whichElement,
  std::stringstream* commentsOnFailure
) const {
  return
  ASNElement::hasSubElementTemplate(
    this, desiredIndices, desiredTypes, whichElement, commentsOnFailure
  );
}

template <typename thisPointerType>
bool ASNElement::hasSubElementTemplate(
  thisPointerType* thisPointer,
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  thisPointerType** whichElement,
  std::stringstream* commentsOnFailure
) {
  thisPointerType* current = thisPointer;
  for (int i = 0; i < desiredIndices.size; i ++) {
    int currentIndex = desiredIndices[i];
    if (currentIndex >= current->elements.size) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "ASN element is missing composite index: "
        << desiredIndices.sliceCopy(0, i + 1).toStringCommaDelimited()
        << ". ";
      }
      return false;
    }
    current = &(current->elements[currentIndex]);
    if (i < desiredTypes.size) {
      unsigned char desiredTag = desiredTypes[i];
      if (
        current->tag != desiredTag &&
        desiredTag != AbstractSyntaxNotationOneSubsetDecoder::tags::anyType0xff
      ) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "ASN element with composite index: "
          << desiredIndices.sliceCopy(0, i + 1).toStringCommaDelimited()
          << ". "
          << " is not of the needed type "
          << static_cast<int>(desiredTag)
          << ". ";
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
bool ASNElement::hasSubElementOfType<ASNObject>(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  ASNObject& output,
  std::stringstream* commentsOnFailure
) const {
  const ASNElement* element = nullptr;
  if (
    !ASNElement::hasSubElementConst(
      desiredIndices, desiredTypes, &element, commentsOnFailure
    )
  ) {
    return false;
  }
  return output.loadFromASN(*element, commentsOnFailure);
}

template < >
bool ASNElement::hasSubElementOfType<LargeIntegerUnsigned>(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  LargeIntegerUnsigned& output,
  std::stringstream* commentsOnFailure
) const {
  const ASNElement* element = nullptr;
  if (
    !ASNElement::hasSubElementConst(
      desiredIndices, desiredTypes, &element, commentsOnFailure
    )
  ) {
    return false;
  }
  return element->isIntegerUnsigned(&output, commentsOnFailure);
}

template < >
bool ASNElement::hasSubElementOfType<LargeInteger>(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  LargeInteger& output,
  std::stringstream* commentsOnFailure
) const {
  const ASNElement* element = nullptr;
  if (
    !ASNElement::hasSubElementConst(
      desiredIndices, desiredTypes, &element, commentsOnFailure
    )
  ) {
    return false;
  }
  return element->isInteger(&output, commentsOnFailure);
}

template < >
bool ASNElement::hasSubElementOfType<int>(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  int& output,
  std::stringstream* commentsOnFailure
) const {
  LargeInteger outputLarge;
  if (
    !this->hasSubElementOfType(
      desiredIndices, desiredTypes, outputLarge, commentsOnFailure
    )
  ) {
    return false;
  }
  if (!outputLarge.isIntegerFittingInInt(&output)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "ASN is not a small integer because it is too large. ";
    }
    return false;
  }
  return true;
}

template < >
bool ASNElement::hasSubElementOfType<unsigned int>(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  unsigned int& output,
  std::stringstream* commentsOnFailure
) const {
  int outputSigned = 0;
  if (
    !this->hasSubElementOfType(
      desiredIndices, desiredTypes, outputSigned, commentsOnFailure
    )
  ) {
    return false;
  }
  if (outputSigned < 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "ASN is a negative integer, where a non-negative one is requested. ";
    }
    return false;
  }
  output = static_cast<unsigned int>(outputSigned);
  return true;
}

template < >
bool ASNElement::hasSubElementOfType<List<unsigned char> >(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  List<unsigned char>& output,
  std::stringstream* commentsOnFailure
) const {
  const ASNElement* element = nullptr;
  if (
    !ASNElement::hasSubElementConst(
      desiredIndices, desiredTypes, &element, commentsOnFailure
    )
  ) {
    return false;
  }
  if (element->isComposite()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Element with key: "
      << desiredIndices
      << " is not atomic. ";
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

std::string ASNElement::toString() const {
  return this->toJSON().toString();
}

void ASNElement::writeAnnotations(List<Serialization::Marker>& output) {
  std::stringstream bodyStream, tagStream, lengthStream;
  tagStream
  << "Type: "
  << AbstractSyntaxNotationOneSubsetDecoder::getType(this->tag);
  lengthStream << "Content length: " << this->lengthPromised;
  bodyStream << tagStream.str() << "<br>" << lengthStream.str();
  if (this->comment != "") {
    bodyStream << "<br>" << comment;
  }
  int lengthOfLengthEncoding = this->getLengthLengthEncoding();
  output.addOnTop(
    Serialization::Marker(
      this->offsetLastWrite,
      this->lengthPromised + lengthOfLengthEncoding + 1,
      bodyStream.str()
    )
  );
  output.addOnTop(
    Serialization::Marker(this->offsetLastWrite, 1, tagStream.str())
  );
  output.addOnTop(
    Serialization::Marker(
      this->offsetLastWrite + 1,
      lengthOfLengthEncoding,
      lengthStream.str()
    )
  );
  output.addOnTop(
    Serialization::Marker(
      this->offsetLastWrite + 1 + lengthOfLengthEncoding,
      this->lengthPromised,
      bodyStream.str()
    )
  );
  for (int i = 0; i < this->elements.size; i ++) {
    this->elements[i].writeAnnotations(output);
  }
}

JSData ASNElement::toJSON() const {
  JSData result;
  this->toJSON(result);
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
int ASNElement::getLengthLengthEncoding() {
  List<unsigned char> lengthEncoding;
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::writeLength(
    static_cast<unsigned>(this->lengthPromised), lengthEncoding, 0
  );
  return lengthEncoding.size;
}

void ASNElement::toJSON(JSData& output) const {
  output.reset();
  output[ASNElement::JSLabels::tag] =
  StringRoutines::convertByteToHex(this->tag);
  output[ASNElement::JSLabels::startByteOriginal] =
  StringRoutines::convertByteToHex(this->startByte);
  output[ASNElement::JSLabels::lengthPromised] = this->lengthPromised;
  output[ASNElement::JSLabels::type] =
  AbstractSyntaxNotationOneSubsetDecoder::getType(this->tag);
  if (this->offsetLastRead >= 0) {
    output[ASNElement::JSLabels::offsetLastRead] = this->offsetLastRead;
  }
  if (this->offsetLastWrite >= 0) {
    output[ASNElement::JSLabels::offsetLastWrite] = this->offsetLastWrite;
  }
  List<unsigned char> lengthEncoding;
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::writeLength(
    static_cast<unsigned>(this->lengthPromised), lengthEncoding, 0
  );
  output[ASNElement::JSLabels::lengthEncoding] =
  Crypto::convertListUnsignedCharsToHex(lengthEncoding);
  if (this->flagHeaderPadded) {
    output[ASNElement::JSLabels::lengthEncoding] =
    output[ASNElement::JSLabels::lengthEncoding].stringValue + "00";
  }
  output[ASNElement::JSLabels::isConstructed] = this->flagIsConstructed;
  output[ASNElement::JSLabels::numberOfChildren] = this->elements.size;
  if (this->comment != "") {
    output[ASNElement::JSLabels::comment] = this->comment;
  }
  if (this->isComposite()) {
    JSData children;
    children.elementType = JSData::token::tokenArray;
    children.listObjects.reserve(this->elements.size);
    for (int i = 0; i < this->elements.size; i ++) {
      JSData incoming;
      this->elements[i].toJSON(incoming);
      children.listObjects.addOnTop(incoming);
    }
    output[ASNElement::JSLabels::children] = children;
  }
  if (this->ASNAtom.size > 0 || !this->isComposite()) {
    output[ASNElement::JSLabels::body] =
    Crypto::convertListUnsignedCharsToHex(this->ASNAtom);
    if (
      this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::utf8String0x0c
      ||
      this->tag ==
      AbstractSyntaxNotationOneSubsetDecoder::tags::printableString0x13 ||
      this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::UTCTime0x17 ||
      this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::dateTime0x21
    ) {
      output[ASNElement::JSLabels::interpretation] =
      this->ASNAtom.toStringConcatenate();
    }
    if (
      this->tag ==
      AbstractSyntaxNotationOneSubsetDecoder::tags::objectIdentifier0x06
    ) {
      output[ASNElement::JSLabels::interpretation] =
      this->interpretAsObjectIdentifierGetNameAndId();
    }
    if (
      this->tag ==
      AbstractSyntaxNotationOneSubsetDecoder::tags::octetString0x04
    ) {
      output[ASNElement::JSLabels::interpretation] =
      Crypto::convertListUnsignedCharsToHex(this->ASNAtom);
    }
    if (
      this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::boolean0x01
    ) {
      if (this->ASNAtom.size > 0) {
        if (this->ASNAtom[0]) {
          output[ASNElement::JSLabels::interpretation] = "true";
        } else {
          output[ASNElement::JSLabels::interpretation] = "false";
        }
      }
    }
    if (
      this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::integer0x02
    ) {
      LargeInteger integer;
      this->isInteger(&integer, nullptr);
      output[ASNElement::JSLabels::interpretation] = integer.toString();
    }
  }
}

bool ASNElement::isIntegerUnsigned(
  LargeIntegerUnsigned* whichInteger, std::stringstream* commentsOnFalse
) const {
  STACK_TRACE("ASNElement::isIntegerUnsigned");
  if (
    this->tag != AbstractSyntaxNotationOneSubsetDecoder::tags::integer0x02
  ) {
    if (commentsOnFalse != nullptr) {
      *commentsOnFalse
      << "Element is not an unsigned integer but is of type: "
      << this->getType()
      << ". ";
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
    global.fatal
    << "ASNElement must be an integer at this point. "
    << global.fatal;
  }
  *whichInteger = result.value;
  return true;
}

bool ASNElement::isInteger(
  LargeInteger* whichInteger, std::stringstream* commentsOnFalse
) const {
  STACK_TRACE("ASNElement::isInteger");
  if (
    this->tag != AbstractSyntaxNotationOneSubsetDecoder::tags::integer0x02
  ) {
    if (commentsOnFalse != nullptr) {
      *commentsOnFalse
      << "Element is not an integer but is of type: "
      << this->getType()
      << ". ";
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

std::string AbstractSyntaxNotationOneSubsetDecoder::getType(
  unsigned char startByte
) {
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
  this->ASNAtom.setSize(0);
  this->elements.setSize(0);
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

void AbstractSyntaxNotationOneSubsetDecoder::writeNull(
  List<unsigned char>& output
) {
  STACK_TRACE("AbstractSyntaxNotationOneSubsetDecoder::writeNull");
  output.addOnTop(AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05);
  output.addOnTop(0);
}

void ASNElement::computeTag() {
  this->tag = this->startByte;
  this->flagIsConstructed = this->hasCostructedStartByte();
  if (this->flagIsConstructed) {
    if (
      this->tag != AbstractSyntaxNotationOneSubsetDecoder::tags::timeOfDay0x20
      &&
      this->tag != AbstractSyntaxNotationOneSubsetDecoder::tags::dateTime0x21
      &&
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

bool AbstractSyntaxNotationOneSubsetDecoder::decodeCurrent(ASNElement& output)
{
  STACK_TRACE("AbstractSyntaxNotationOneSubsetDecoder::decodeCurrent");
  RecursionDepthCounter recursionGuard(&this->recursionDepthGuard);
  if (this->recursionDepthGuard > this->maxRecursionDepth) {
    std::stringstream errorStream;
    errorStream << "Max depth of " << this->maxRecursionDepth << " exceeded. ";
    output.error = errorStream.str();
    return false;
  }
  output.reset();
  output.offsetLastRead = this->dataPointer;
  if (this->pointerIsBad(output)) {
    return false;
  }
  output.startByte = (*this->rawData)[this->dataPointer];
  output.computeTag();
  this->dataPointer ++;
  if (this->pointerIsBad(output)) {
    return false;
  }
  if (!this->decodeLengthIncrementDataPointer(output)) {
    return false;
  }
  std::stringstream errorStream;
  switch (output.tag) {
  // pure composite elements:
  case tags::reserved0:
    return this->decodeSequenceLikeContent(output);
  case tags::sequence0x10:
    return this->decodeSequenceLikeContent(output);
  case tags::set0x11:
    return this->decodeSequenceLikeContent(output);
    // either non-pure composite
    // or atom:
  case tags::bitString0x03:
    return this->decodeBitString(output);
    // atoms with extra
    // comments/interpretation:
  case tags::null0x05:
    return this->decodeNull(output);
    // atoms without additional interpretation:
  case tags::boolean0x01:
  case tags::integer0x02:
  case tags::objectIdentifier0x06:
  case tags::printableString0x13:
  case tags::utf8String0x0c:
  case tags::IA5String0x16:
  case tags::UTCTime0x17:
  case tags::octetString0x04:
    this->decodeASNAtomContent(output);
    return true;
  default:
    errorStream << "Unknown object tag: " << static_cast<int>(output.tag)
    << ", byte: "
    << static_cast<int>(output.startByte)
    << " at position: "
    << output.offsetLastRead
    << ". Length: "
    << output.lengthPromised
    << ". ";
    output.error = errorStream.str();
    return false;
  }
}

bool AbstractSyntaxNotationOneSubsetDecoder::decode(
  const List<unsigned char>& inputRawData,
  int inputOffset,
  ASNElement& output,
  std::stringstream* commentsOnError
) {
  STACK_TRACE("AbstractSyntaxNotationOneSubsetDecoder::decode");
  if (inputRawData.size == 0) {
    if (commentsOnError != nullptr) {
      *commentsOnError
      << "Empty sequences are not allowed in our ASN-1 decoder. ";
    }
    return false;
  }
  int maxAllowedSize = 1000000000;
  if (inputRawData.size >= maxAllowedSize) {
    if (commentsOnError != nullptr) {
      *commentsOnError
      << "Input size: "
      << this->rawData->size
      << " too large, max allowed: "
      << maxAllowedSize
      << ". ";
    }
    return false;
  }
  this->reset();
  this->dataPointer = inputOffset;
  this->decodedData = &output;
  this->rawData = &inputRawData;
  this->checkInitialization();
  this->decodedData->reset();
  if (!this->decodeCurrent(*this->decodedData)) {
    if (commentsOnError != nullptr) {
      *commentsOnError
      << "Failed to decode data with error: "
      << this->decodedData->error;
    }
    return false;
  }
  if (
    this->dataPointer < this->rawData->size && this->flagMustDecodeAll
  ) {
    if (commentsOnError != nullptr) {
      *commentsOnError
      << "Decoded "
      << this->dataPointer
      << " bytes but the input had: "
      << this->rawData->size
      << ". ";
    }
    return false;
  }
  return true;
}

void AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::
writeLength(unsigned int input, List<unsigned char>& output, int offset) {
  if (offset >= output.size) {
    int oldSize = output.size;
    output.setSize(offset + 1);
    for (int i = oldSize; i < offset; i ++) {
      output[i] = 0;
    }
  }
  if (input < 128) {
    unsigned char length = static_cast<unsigned char>(input);
    output[offset] = length;
    return;
  }
  int numBytes =
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::
  getReservedBytesForLength(static_cast<signed>(input)) -
  1;
  unsigned char lengthPlus128 = 128 + static_cast<unsigned char>(numBytes);
  output[offset] = lengthPlus128;
  offset ++;
  Serialization::writeNByteUnsigned(
    numBytes, static_cast<unsigned int>(input), output, offset
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

std::string AbstractSyntaxNotationOneSubsetDecoder::toStringAnnotateBinary() {
  if (!this->checkInitialization()) {
    return "ASN1 not initialized properly. ";
  }
  std::stringstream out;
  out
  << "<script "
  << WebAPI::result::scriptType
  << "='abstractSyntaxNotationAnnotate'>";
  out
  << "[\""
  << Crypto::convertListUnsignedCharsToHex(*this->rawData)
  << "\"";
  out << ", ";
  out << this->decodedData->toString();
  out << ", ";
  List<unsigned char> idNonHexed;
  Crypto::computeSha256(*this->rawData, idNonHexed);
  std::string id = Crypto::convertListUnsignedCharsToHex(idNonHexed);
  out << "\"" << id << "\"";
  out << "]";
  out << "</script>";
  out << "<span id='" << id << "'></span>";
  return out.str();
}

int AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::
getReservedBytesForLength(int length) {
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

AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::
WriterObjectFixedLength(
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
  this->outputPointer->addOnTop(startByte);
  this->totalByteLength = expectedTotalElementByteLength;
  this->reservedBytesForLength =
  this->getReservedBytesForLength(expectedTotalElementByteLength);
  for (int i = 0; i < this->reservedBytesForLength; i ++) {
    this->outputPointer->addOnTop(0);
  }
}

AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::~
WriterObjectFixedLength() {
  this->totalByteLength =
  this->outputPointer->size - this->offset - 1 - this->reservedBytesForLength;
  if (this->outputTotalByteLength != nullptr) {
    *this->outputTotalByteLength = this->totalByteLength;
  }
  int actualBytesNeededForLength =
  this->getReservedBytesForLength(this->totalByteLength);
  if (actualBytesNeededForLength > this->reservedBytesForLength) {
    global
    << Logger::red
    << "Wrong number of reserved bytes for sequence writer. "
    << "This is non-fatal but affects negatively performance. "
    << Logger::endL;
    this->outputPointer->shiftUpExpandOnTopRepeated(
      this->offset + 1,
      actualBytesNeededForLength - this->reservedBytesForLength
    );
  }
  if (actualBytesNeededForLength < this->reservedBytesForLength) {
    global
    << Logger::red
    << "Wrong number of reserved bytes for sequence writer. "
    << "This is non-fatal but affects negatively performance. "
    << Logger::endL;
    this->outputPointer->removeIndicesShiftDown(
      this->offset + 1,
      this->reservedBytesForLength - actualBytesNeededForLength
    );
  }
  this->reservedBytesForLength = actualBytesNeededForLength;
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::writeLength(
    static_cast<unsigned>(this->totalByteLength),
    *this->outputPointer,
    this->offset + 1
  );
}

void AbstractSyntaxNotationOneSubsetDecoder::writeUnsignedIntegerObject(
  const LargeIntegerUnsigned& input, List<unsigned char>& output
) {
  List<unsigned char> serialized;
  input.writeBigEndianBytes(serialized, true);
  ASNElement::writeBytesASNAtom(
    AbstractSyntaxNotationOneSubsetDecoder::tags::integer0x02,
    serialized,
    output
  );
}

void AbstractSyntaxNotationOneSubsetDecoder::writeObjectId(
  const List<unsigned char>& input, List<unsigned char>& output
) {
  ASNElement::writeBytesASNAtom(
    AbstractSyntaxNotationOneSubsetDecoder::tags::objectIdentifier0x06,
    input,
    output
  );
}

void ASNObject::initializeAddSample(
  MapList<
    std::string, ASNObject, HashFunctions::hashFunction<std::string>
  >& container,
  const std::string& inputName,
  const std::string& inputObjectIdHex,
  unsigned char inputContentTag
) {
  ASNObject incoming;
  incoming.name = inputName;
  incoming.content.startByte = inputContentTag;
  incoming.content.computeTag();
  std::stringstream commentsOnFailure;
  incoming.objectId.startByte =
  AbstractSyntaxNotationOneSubsetDecoder::tags::objectIdentifier0x06;
  incoming.objectId.computeTag();
  if (
    !Crypto::convertHexToListUnsignedChar(
      inputObjectIdHex, incoming.objectId.ASNAtom, &commentsOnFailure
    )
  ) {
    global.fatal
    << "Failure in certificate field initialization is not allowed. "
    << global.fatal;
  }
  container.setKeyValue(incoming.name, incoming);
}

MapList<
  List<unsigned char>, ASNObject, MathRoutines::hashListUnsignedChars
>& ASNObject::objectIdsToNames() {
  static MapList<
    List<unsigned char>, ASNObject, MathRoutines::hashListUnsignedChars
  > result;
  return result;
}

std::string ASNObject::names::sha1 = "sha1";
std::string ASNObject::names::sha256 = "sha256";
std::string ASNObject::names::sha384 = "sha384";
std::string ASNObject::names::sha512 = "sha512";
std::string ASNObject::names::sha256WithRSAEncryption =
"sha256WithRSAEncryption";
std::string ASNObject::names::RSAEncryption = "RSAEncryption";
std::string ASNObject::names::countryName = "countryName";
std::string ASNObject::names::stateOrProvinceName = "stateOrProvinceName";
std::string ASNObject::names::localityName = "localityName";
std::string ASNObject::names::organizationName = "organizationName";
std::string ASNObject::names::organizationalUnitName =
"organizationalUnitName";
std::string ASNObject::names::commonName = "commonName";
std::string ASNObject::names::emailAddress = "emailAddress";
std::string ASNObject::names::basicConstraints = "basicConstraints";
std::string ASNObject::names::subjectKeyIdentifier = "subjectKeyIdentifier";
std::string ASNObject::names::authorityKeyIdentifier =
"authorityKeyIdentifier";
MapList<
  std::string, ASNObject, HashFunctions::hashFunction<std::string>
>& ASNObject::namesToObjectIdsNonThreadSafe() {
  static MapList<
    std::string, ASNObject, HashFunctions::hashFunction<std::string>
  > container;
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
    ASNObject::names::sha256WithRSAEncryption,
    "2a864886f70d01010b",
    AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05
  );
  ASNObject::initializeAddSample(
    container,
    ASNObject::names::RSAEncryption,
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
  MapList<
    List<unsigned char>, ASNObject, MathRoutines::hashListUnsignedChars
  >& reverseMap =
  ASNObject::objectIdsToNames();
  for (int i = 0; i < container.values.size; i ++) {
    ASNObject& current = container.values[i];
    reverseMap.setKeyValue(current.objectId.ASNAtom, current);
  }
  return container;
}

int ASNObject::loadField(
  const MapList<
    std::string, ASNObject, HashFunctions::hashFunction<std::string>
  >& inputFields,
  const std::string& fieldName
) {
  if (!ASNObject::namesToObjectIdsNonThreadSafe().contains(fieldName)) {
    global.fatal
    << "Field "
    << fieldName
    << " is hard-coded but is yet unknown. "
    << global.fatal;
  }
  if (!inputFields.contains(fieldName)) {
    this->name = fieldName;
    this->objectId.ASNAtom.setSize(0);
    return 0;
  }
  *this = inputFields.getValueNoFail(fieldName);
  return 1;
}

std::string ASNObject::toString() const {
  std::stringstream out;
  out
  << "objectId: "
  << Crypto::convertListUnsignedCharsToHex(this->objectId.ASNAtom)
  << ", name: "
  << Crypto::convertStringToHex(this->name, 0, false);
  std::string content = this->content.ASNAtom.toStringConcatenate();
  out
  << ", content: "
  << StringRoutines::convertStringToHexIfNonReadable(content, 0, false);
  return out.str();
}

std::string ASNObject::toStringAllRecognizedObjectIds() {
  STACK_TRACE("ASNObject::toStringAllRecognizedObjectIds");
  // This function is safe after the first run, which should have already
  // happened, unless we are unit-testing.
  ASNObject::namesToObjectIdsNonThreadSafe();
  std::stringstream out;
  out << "<table>";
  for (int i = 0; i < ASNObject::objectIdsToNames().size(); i ++) {
    ASNObject& current = ASNObject::objectIdsToNames().values[i];
    const List<unsigned char>& currentId =
    ASNObject::objectIdsToNames().keys[i];
    out
    << "<tr>"
    << "<td><small>"
    << i + 1
    << "</small></td>"
    << "<td>"
    << Crypto::convertListUnsignedCharsToHexFormat(currentId, 50, false)
    << "</td>"
    << "<td>"
    << current.name
    << "</td>"
    << "</tr>";
  }
  out << "</table>";
  return out.str();
}

const List<unsigned char>& ASNObject::objectIdFromNameNoFail(
  const std::string& input
) {
  return
  ASNObject::namesToObjectIdsNonThreadSafe().getValueNoFail(input).objectId.
  ASNAtom;
}

bool ASNObject::loadFieldsFromASNSequence(
  const ASNElement& input,
  MapList<
    std::string, ASNObject, HashFunctions::hashFunction<std::string>
  >& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("ASNObject::loadFieldsFromASNSequence");
  output.clear();
  if (
    input.tag != AbstractSyntaxNotationOneSubsetDecoder::tags::sequence0x10
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Input is not array. ";
    }
    return false;
  }
  for (int i = 0; i < input.elements.size; i ++) {
    ASNObject current;
    if (!current.loadFromASN(input.elements[i], commentsOnFailure)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Failed to load certificate entry index: "
        << i
        << ". ";
      }
      return false;
    }
    output.setKeyValue(current.name, current);
  }
  return true;
}

std::string ASNElement::getType() const {
  return AbstractSyntaxNotationOneSubsetDecoder::getType(this->tag);
}

bool ASNObject::loadFromASN(
  const ASNElement& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("ASNObject::loadFromASN");
  if (
    input.tag != AbstractSyntaxNotationOneSubsetDecoder::tags::set0x11 ||
    input.elements.size != 1
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "ASNObject outer layer must "
      << "be a one-element set, instead is of type: "
      << input.getType()
      << " and has "
      << input.elements.size
      << " elements. ";
    }
    return false;
  }
  const ASNElement& internal = input.elements[0];
  if (
    internal.tag != AbstractSyntaxNotationOneSubsetDecoder::tags::sequence0x10
    ||
    internal.elements.size != 2
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "ASNObject JSON representation must be a two-element sequence. ";
    }
    return false;
  }
  this->objectId = internal.elements[0];
  if (
    this->objectId.tag !=
    AbstractSyntaxNotationOneSubsetDecoder::tags::objectIdentifier0x06
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Missing objectIdentifierBytes key. ";
    }
    return false;
  }
  if (
    !ASNObject::objectIdsToNames().contains(this->objectId.ASNAtom)
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Unrecognized object id. "
      << "Total known ids: "
      << ASNObject::objectIdsToNames().keys.size;
    }
    return false;
  }
  *this =
  ASNObject::objectIdsToNames().getValueCreateNoInitialization(
    this->objectId.ASNAtom
  );
  const ASNElement& second = internal.elements[1];
  if (second.isComposite()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "ASN object content is required to be non-composite. ";
    }
    return false;
  }
  this->content = second;
  return true;
}

void ASNObject::initializeNonThreadSafe() {
  STACK_TRACE("ASNObject::initializeNonThreadSafe");
  ASNObject::namesToObjectIdsNonThreadSafe();
}

void ASNElement::makeObjectId(const List<unsigned char>& input) {
  this->reset();
  this->tag =
  AbstractSyntaxNotationOneSubsetDecoder::tags::objectIdentifier0x06;
  this->startByte = this->tag;
  this->ASNAtom = input;
}

void ASNElement::makeInteger(const LargeIntegerUnsigned& input) {
  this->reset();
  this->tag = AbstractSyntaxNotationOneSubsetDecoder::tags::integer0x02;
  this->startByte = AbstractSyntaxNotationOneSubsetDecoder::tags::integer0x02;
  input.writeBigEndianBytes(this->ASNAtom, true);
}

void ASNElement::makeNull() {
  this->reset();
  this->tag = AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05;
  this->startByte = AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05;
}

void ASNElement::makeSet(
  int numberOfEmptyElements,
  bool setLeadingBit,
  bool setSecondMostSignificantBit,
  bool constructed
) {
  this->reset();
  this->tag = AbstractSyntaxNotationOneSubsetDecoder::tags::set0x11;
  this->startByte = this->tag;
  this->setStartByteFlags(
    setLeadingBit, setSecondMostSignificantBit, constructed
  );
  this->elements.setSize(numberOfEmptyElements);
}

void ASNElement::makeSequence(const List<ASNElement>& input) {
  this->makeSequence(input.size);
  this->elements = input;
}

void ASNElement::makeSequence(int numberOfEmptyElements) {
  this->reset();
  this->tag = AbstractSyntaxNotationOneSubsetDecoder::tags::sequence0x10;
  this->startByte = this->tag + 32;
  this->elements.setSize(numberOfEmptyElements);
}

void TBSCertificateInfo::computeASNSignatureAlgorithmIdentifier(
  ASNElement& output
) {
  output.makeSequence(2);
  output[0] = this->signatureAlgorithmIdentifier;
  output[1].makeNull();
}

void TBSCertificateInfo::computeASNValidityWrapper(ASNElement& output) {
  output.makeSequence(2);
  output[0] = this->validityNotBefore;
  output[0].comment = "Validity not before";
  output[1] = this->validityNotAfter;
  output[1].comment = "Validity not after";
}

void TBSCertificateInfo::computeASNVersionWrapper(ASNElement& output) {
  output.reset();
  output.tag = 0;
  output.startByte = 160;
  ASNElement versionInteger;
  versionInteger.makeInteger(this->version);
  output.elements.addOnTop(versionInteger);
}

void ASNElement::setStartByteFlags(
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

void ASNElement::makeBitStringEmpty(
  bool setLeadingBit, bool setSecondMostSignificantBit, bool setConstructed
) {
  this->reset();
  this->tag = AbstractSyntaxNotationOneSubsetDecoder::tags::bitString0x03;
  this->startByte = this->tag;
  this->setStartByteFlags(
    setLeadingBit, setSecondMostSignificantBit, setConstructed
  );
  this->ASNAtom.setSize(0);
  this->elements.setSize(0);
}

void ASNElement::makeBitString(const List<unsigned char>& input) {
  this->reset();
  this->startByte =
  AbstractSyntaxNotationOneSubsetDecoder::tags::bitString0x03;
  this->tag = this->startByte;
  this->ASNAtom = input;
}

void ASNElement::makeOctetString(const List<unsigned char>& input) {
  this->reset();
  this->startByte =
  AbstractSyntaxNotationOneSubsetDecoder::tags::octetString0x04;
  this->tag = this->startByte;
  this->ASNAtom = input;
}

void TBSCertificateInfo::computeASNSignature(ASNElement& output) {
  output.makeSequence(2);
  output.comment = "signature";
  output[0].makeSequence(2);
  output[0][0].makeObjectId(
    ASNObject::objectIdFromNameNoFail(ASNObject::names::RSAEncryption)
  );
  output[0][1].makeNull();
  output[1].makeBitStringEmpty(false, false, false);
  output[1].elements.setSize(1);
  ASNElement& signatureSerializer = output[1][0];
  signatureSerializer.makeSequence(2);
  signatureSerializer[0].makeInteger(this->subjectPublicKey.exponent);
  signatureSerializer[1].makeInteger(this->subjectPublicKey.modulus);
  signatureSerializer.writeBytesUpdatePromisedLength(output[1].ASNAtom);
  output[1].lengthPromised = output[1].ASNAtom.size;
  output[1].flagHeaderPadded = true;
}

void TBSCertificateInfo::computeASN(ASNElement& output) {
  output.makeSequence(8);
  this->computeASNVersionWrapper(output[0]);
  output[1].makeInteger(this->serialNumber);
  output[1].comment = "serial number";
  this->computeASNSignatureAlgorithmIdentifier(output[2]);
  output[2].comment = "algorithm identifier";
  this->issuer.computeASN(output[3]);
  output[3].comment = "Issuer [entity that signed this certificate]";
  this->computeASNValidityWrapper(output[4]);
  output[4].comment = "Validity";
  this->subject.computeASN(output[5]);
  output[5].comment = "Subject [entity advertising its public key identity]";
  this->computeASNSignature(output[6]);
  this->computeASNExtensions(output[7]);
}

void ASNElement::makeBitStringSequence(const List<ASNElement>& input) {
  this->makeSequence(input);
  this->tag = AbstractSyntaxNotationOneSubsetDecoder::tags::bitString0x03;
  this->startByte = this->tag + 128 + 32;
  for (int i = 0; i < this->elements.size; i ++) {
    this->elements[i].writeBytesUpdatePromisedLength(this->ASNAtom);
  }
  this->lengthPromised = this->ASNAtom.size;
}

void TBSCertificateInfo::computeASNExtensions(ASNElement& output) {
  output.makeBitStringSequence(this->extensions);
}

void ASNObject::computeASN(ASNElement& output) {
  output.makeSet(1, false, false, true);
  output[0].makeSequence(2);
  output[0][0] = this->objectId;
  output[0][1] = this->content;
  std::stringstream comment;
  comment
  << "Object: "
  << this->name
  << ", interpretation: "
  << this->content.ASNAtom.toStringConcatenate();
  output[0][0].comment = comment.str();
  output[0][1].comment = comment.str();
}

void TBSCertificateInfo::Organization::computeASN(ASNElement& output) {
  output.makeSequence(0);
  List<ASNObject*> serializationOrder({
      &this->countryName,
      &this->stateOrProvinceName,
      &this->localityName,
      &this->organizationName,
      &this->organizationalUnitName,
      &this->commonName,
      &this->emailAddress
    }
  );
  ASNElement next;
  for (int i = 0; i < serializationOrder.size; i ++) {
    ASNObject* current = serializationOrder[i];
    if (current->isEmpty()) {
      continue;
    }
    current->computeASN(next);
    output.elements.addOnTop(next);
  }
}

void X509Certificate::computeASN(ASNElement& output) {
  output.reset();
  output.makeSequence(3);
  output.comment = "X509 certificate";
  this->information.computeASN(output[0]);
  this->computeASNSignatureAlgorithm(output[1]);
  output[2] = this->signatureValue;
}

void X509Certificate::computeASNSignatureAlgorithm(ASNElement& output) {
  output.makeSequence(2);
  output[0] = this->signatureAlgorithmId;
  output[1].makeNull();
}

void X509Certificate::writeBytesASN1(
  List<unsigned char>& output,
  List<Serialization::Marker>* annotations
) {
  STACK_TRACE("X509Certificate::writeBytesASN1");
  this->computeASN(this->recodedASN);
  this->recodedASN.writeBytesUpdatePromisedLength(output);
  if (annotations != nullptr) {
    this->recodedASN.writeAnnotations(*annotations);
  }
}

std::string X509Certificate::toStringTestEncode() {
  std::stringstream out;
  std::string sourceHex =
  Crypto::convertListUnsignedCharsToHex(this->sourceBinary);
  List<unsigned char> recoded;
  this->writeBytesASN1(recoded, nullptr);
  std::string recodedHex = Crypto::convertListUnsignedCharsToHex(recoded);
  out
  << "Original, recoded binary source:<br>"
  << StringRoutines::Differ::differenceHTMLStatic(
    sourceHex, recodedHex, "sourceHex", "recodedHex"
  );
  return out.str();
}

std::string X509Certificate::toHex() {
  List<unsigned char> bytes;
  this->writeBytesASN1(bytes, nullptr);
  return Crypto::convertListUnsignedCharsToHex(bytes);
}

std::string X509Certificate::toString() {
  std::stringstream out;
  out
  << "Certificate pub key:<br>"
  << this->information.subjectPublicKey.toString();
  out << this->information.toString();
  AbstractSyntaxNotationOneSubsetDecoder decoderRecoded;
  decoderRecoded.decodedData = &this->recodedASN;
  List<unsigned char> recodedBytes;
  this->recodedASN.writeBytesUpdatePromisedLength(recodedBytes);
  decoderRecoded.rawData = &recodedBytes;
  out << "<br><b>Recoded (" << recodedBytes.size << " bytes).</b><br>";
  out << decoderRecoded.toStringAnnotateBinary();
  out << "<br><b>Source (" << this->sourceBinary.size << " bytes).</b><br>";
  if (this->sourceBinary == recodedBytes) {
    out
    << "<b style = 'color:green'>Recoded and original source coincide. </b>";
  } else {
    AbstractSyntaxNotationOneSubsetDecoder decoderSource;
    decoderSource.decodedData = &this->sourceASN;
    decoderSource.rawData = &this->sourceBinary;
    out << decoderSource.toStringAnnotateBinary();
  }
  return out.str();
}

std::string PrivateKeyRSA::toString() const {
  std::stringstream out;
  out
  << "Exponent: "
  << this->publicKey.exponent
  << "<br>Prime 1: "
  << this->primeOne
  << "<br>Prime 2: "
  << this->primeTwo;
  return out.str();
}

bool PrivateKeyRSA::basicChecks(std::stringstream* comments) {
  std::stringstream commentsContainer;
  if (comments == nullptr) {
    comments = &commentsContainer;
  }
  LargeInteger mustBeZero;
  LargeInteger primeOneLI = this->primeOne;
  LargeInteger primeTwoLI = this->primeTwo;
  mustBeZero = this->primeOne;
  mustBeZero *= this->primeTwo;
  mustBeZero -= this->publicKey.modulus;
  *comments << "Must be zero: " << mustBeZero.toString();
  LargeInteger EulerPhi = (primeOneLI - 1) *(primeTwoLI - 1);
  ElementZmodP mustBeZeroModP;
  // exponent.modulo = EulerPhi.value;
  // exponent.value = this->exponent;
  mustBeZeroModP.modulus = EulerPhi.value;
  mustBeZeroModP.value = 1;
  mustBeZeroModP /= this->publicKey.exponent;
  mustBeZeroModP -= this->privateExponent;
  *comments
  << "<br>Difference between private "
  << "exponent and computed one: "
  << mustBeZeroModP.value.toString();
  return true;
}

bool PrivateKeyRSA::loadFromPEMFile(
  const std::string& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("PrivateKeyRSA::loadFromPEMFile");
  std::string certificateContent;
  // No access to sensitive folders here, so this cannot be used for the
  // server's private key.
  // For server's certificate, use TransportLayerSecurity::loadPEMPrivateKey.
  if (
    !FileOperations::loadFileToStringVirtual(
      input, certificateContent, false, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load key file. ";
    }
    return false;
  }
  return this->loadFromPEM(certificateContent, commentsOnFailure);
}

bool PrivateKeyRSA::loadFromPEM(
  const std::string& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("PrivateKeyRSA::loadFromPEM");
  std::string certificateContentStripped =
  StringRoutines::stringTrimWhiteSpace(input);
  if (
    !StringRoutines::stringBeginsWith(
      certificateContentStripped,
      "-----BEGIN PRIVATE KEY-----",
      &certificateContentStripped
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad private key start. ";
    }
    return false;
  }
  if (
    !StringRoutines::stringEndsWith(
      certificateContentStripped,
      "-----END PRIVATE KEY-----",
      &certificateContentStripped
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad private key end. ";
    }
    return false;
  }
  certificateContentStripped =
  StringRoutines::stringTrimWhiteSpace(certificateContentStripped);
  if (
    !Crypto::convertBase64ToBitStream(
      certificateContentStripped,
      this->sourceBinary,
      commentsOnFailure,
      nullptr
    )
  ) {
    return false;
  }
  return this->loadFromASNEncoded(this->sourceBinary, commentsOnFailure);
}

// Notes:
// https://tools.ietf.org/html/rfc3447#page-23
// Algorithm:
// https://tools.ietf.org/html/rfc3447#page-29
void PrivateKeyRSA::signBytesPadPKCS1(
  List<unsigned char>& input, int hash, List<unsigned char>& output
) {
  STACK_TRACE("PrivateKeyRSA::signBytesPadPKCS1");
  List<unsigned char> inputHashedPadded;
  this->hashAndPadPKCS1(input, hash, inputHashedPadded);
  ElementZmodP element, one;
  if (this->publicKey.modulus.isEqualToZero()) {
    global.fatal << "Public key modulus is zero. " << global.fatal;
  }
  element.modulus = this->publicKey.modulus;
  one.makeOne(this->publicKey.modulus);
  Crypto::convertListUnsignedCharsToLargeUnsignedIntegerBigEndian(
    inputHashedPadded, element.value
  );
  MathRoutines::raiseToPower(element, this->privateExponent, one);
  output.setSize(0);
  element.value.writeBigEndianBytes(output, false);
}

PrivateKeyRSA::PrivateKeyRSA() {
  this->bitSize = 0;
  this->byteSize = 0;
}

void PrivateKeyRSA::computeBitSize() {
  this->bitSize = static_cast<signed>(
    this->publicKey.modulus.logarithmBaseNCeiling(2)
  );
  this->byteSize = this->bitSize / 8;
  if (this->byteSize * 8 < this->bitSize) {
    this->byteSize ++;
  }
}

bool PrivateKeyRSA::computeFromTwoPrimes(
  LargeIntegerUnsigned& inputPrimeOne,
  LargeIntegerUnsigned& inputPrimeTwo,
  bool verifyInputsArePrime,
  std::stringstream* commentsOnFailure
) {
  this->primeOne = inputPrimeOne;
  this->primeTwo = inputPrimeTwo;
  if (verifyInputsArePrime) {
    if (!this->primeOne.isPossiblyPrime(10, true, commentsOnFailure)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "First input: "
        << this->primeTwo.toString()
        << " is not prime. ";
      }
      return false;
    }
    if (!this->primeTwo.isPossiblyPrime(10, true, commentsOnFailure)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Second input: "
        << this->primeTwo.toString()
        << " is not prime. ";
      }
      return false;
    }
  }
  this->publicKey.exponent.assignUnsignedInt(
    this->publicKey.defaultExponent
  );
  this->publicKey.modulus = this->primeOne * this->primeTwo;
  this->carmichaelTotientOfModulus =
  MathRoutines::leastCommonMultiple(
    this->primeOne - 1, this->primeTwo - 1
  );
  ElementZmodP inverter;
  inverter.modulus = this->carmichaelTotientOfModulus;
  inverter.value = 1;
  inverter /= this->publicKey.exponent;
  this->privateExponent = inverter.value;
  this->computeBitSize();
  return true;
}

bool PrivateKeyRSA::generateRandom(
  std::stringstream* commentsOnFailure, int numberOfBytes
) {
  if (numberOfBytes < 1 || numberOfBytes > 1000) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Number of bytes must be "
      << "between 1 and 1000; you requested: "
      << numberOfBytes
      << ". ";
    }
    return false;
  }
  Crypto::Random::getRandomLargePrime(this->primeOne, numberOfBytes);
  Crypto::Random::getRandomLargePrime(this->primeTwo, numberOfBytes);
  return
  this->computeFromTwoPrimes(
    this->primeOne, this->primeTwo, false, commentsOnFailure
  );
}

void PrivateKeyRSA::hashAndPadPKCS1(
  List<unsigned char>& input, int hash, List<unsigned char>& output
) {
  List<unsigned char> inputHashed;
  ASNElement encoder;
  encoder.makeSequence(2);
  ASNObject hashObject;
  switch (hash) {
  case SignatureAlgorithmSpecification::HashAlgorithm::sha256:
    Crypto::computeSha256(input, inputHashed);
    hashObject =
    ASNObject::namesToObjectIdsNonThreadSafe().getValueNoFail(
      ASNObject::names::sha256
    );
    break;
  default:
    global.fatal
    << "Non-allowed or non-implemented value for the hash algorithm. "
    << global.fatal;
  }
  encoder.elements[1].makeOctetString(inputHashed);
  hashObject.computeASN(encoder.elements[0]);
  List<unsigned char> payload;
  encoder.writeBytesUpdatePromisedLength(payload);
  int numberOfFFs = this->byteSize - payload.size - 3;
  if (numberOfFFs < 0) {
    numberOfFFs = 0;
  }
  output.setSize(0);
  output.addOnTop(0x00);
  output.addOnTop(0x01);
  for (int i = 0; i < numberOfFFs; i ++) {
    output.addOnTop(0xff);
  }
  output.addOnTop(0x00);
  output.addListOnTop(payload);
}

bool PrivateKeyRSA::loadFromASNEncoded(
  List<unsigned char>& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("PrivateKeyRSA::loadFromASNEncoded");
  AbstractSyntaxNotationOneSubsetDecoder outerDecoder, innerDecoder;
  if (
    !outerDecoder.decode(
      input, 0, this->sourceASNOuter, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to asn-decode private key outer data. ";
    }
    return false;
  }
  const ASNElement* innerData;
  if (
    !this->sourceASNOuter.hasSubElementConst({2}, {}, &innerData,
      commentsOnFailure
    )
  ) {
    return false;
  }
  if (
    !innerDecoder.decode(
      innerData->ASNAtom, 0, this->sourceASNInner, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to asn-decode private key inner data. ";
    }
    return false;
  }
  if (
    !this->sourceASNInner.hasSubElementOfType({1}, {}, this->publicKey.modulus,
      commentsOnFailure
    )
  ) {
    return false;
  }
  if (
    !this->sourceASNInner.hasSubElementOfType({2}, {}, this->publicKey.exponent
      , commentsOnFailure
    )
  ) {
    return false;
  }
  if (
    !this->sourceASNInner.hasSubElementOfType({3}, {}, this->privateExponent,
      commentsOnFailure
    )
  ) {
    return false;
  }
  if (
    !this->sourceASNInner.hasSubElementOfType({4}, {}, this->primeOne,
      commentsOnFailure
    )
  ) {
    return false;
  }
  if (
    !this->sourceASNInner.hasSubElementOfType({5}, {}, this->primeTwo,
      commentsOnFailure
    )
  ) {
    return false;
  }
  LargeInteger exponent1, exponent2, coefficient;
  if (
    !this->sourceASNInner.hasSubElementOfType({6}, {}, exponent1,
      commentsOnFailure
    )
  ) {
    return false;
  }
  if (
    !this->sourceASNInner.hasSubElementOfType({7}, {}, exponent2,
      commentsOnFailure
    )
  ) {
    return false;
  }
  if (
    !this->sourceASNInner.hasSubElementOfType({8}, {}, coefficient,
      commentsOnFailure
    )
  ) {
    return false;
  }
  return true;
}

bool X509Certificate::loadFromPEM(
  const std::string& input, std::stringstream* commentsOnFailure
) {
  std::string certificateContentStripped;
  // see ASN1_item_d2i_bio for decoding.
  certificateContentStripped = StringRoutines::stringTrimWhiteSpace(input);
  std::string beginCertificate = "-----BEGIN CERTIFICATE-----";
  std::string endCertificate = "-----END CERTIFICATE-----";
  if (
    !StringRoutines::stringBeginsWith(
      certificateContentStripped,
      beginCertificate,
      &certificateContentStripped
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad certificate start. ";
    }
    return false;
  }
  if (
    !StringRoutines::stringEndsWith(
      certificateContentStripped,
      endCertificate,
      &certificateContentStripped
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad certificate end. ";
    }
    return false;
  }
  certificateContentStripped =
  StringRoutines::stringTrimWhiteSpace(certificateContentStripped);
  if (
    !Crypto::convertBase64ToBitStream(
      certificateContentStripped,
      this->sourceBinary,
      commentsOnFailure,
      nullptr
    )
  ) {
    return false;
  }
  return this->loadFromASNEncoded(this->sourceBinary, commentsOnFailure);
}

bool X509Certificate::loadFromPEMFile(
  const std::string& input, std::stringstream* commentsOnFailure
) {
  std::string certificateContent;
  // No access to sensitive folders here, so this cannot be used for the
  // server's certificate.
  // For server's certificate, use TransportLayerSecurity::loadPEMCertificate.
  if (
    !FileOperations::loadFileToStringVirtual(
      input, certificateContent, false, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load key file. ";
    }
    return false;
  }
  return this->loadFromPEM(certificateContent, commentsOnFailure);
}

std::string TBSCertificateInfo::Organization::toString() {
  std::stringstream out;
  out
  << "Country name: "
  << this->countryName.content.toString()
  << "\n<br>\n";
  out << "Common name: " << this->commonName.content.toString() << "\n<br>\n";
  out
  << "Email address: "
  << this->emailAddress.content.toString()
  << "\n<br>\n";
  out
  << "Locality name: "
  << this->localityName.content.toString()
  << "\n<br>\n";
  out
  << "Organizational unit name: "
  << this->organizationalUnitName.content.toString()
  << "\n<br>\n";
  out
  << "Organization name: "
  << this->organizationName.content.toString()
  << "\n<br>\n";
  out
  << "State or province name: "
  << this->stateOrProvinceName.content.toString()
  << "\n<br>\n";
  return out.str();
}

std::string TBSCertificateInfo::toString() {
  std::stringstream out;
  out << "<b>Issuer:</b><br>";
  out << this->issuer.toString();
  out << "<b>Subject (holder of the advertised public key):</b><br>";
  out << this->subject.toString();
  return out.str();
}

bool TBSCertificateInfo::loadExtensions(
  const ASNElement& input, std::stringstream* commentsOnFailure
) {
  this->extensions.setSize(0);
  if (
    input.tag != AbstractSyntaxNotationOneSubsetDecoder::tags::bitString0x03
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "TBS certificate extensions are expected to be of type bit string. ";
    }
    return false;
  }
  this->extensions = input.elements;
  return true;
}

bool TBSCertificateInfo::loadValidity(
  const ASNElement& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("TBSCertificateInfo::loadValidity");
  if (
    input.tag != AbstractSyntaxNotationOneSubsetDecoder::tags::sequence0x10 ||
    input.elements.size != 2
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Validity element must be a sequence of two elements. ";
    }
    return false;
  }
  if (
    !input.elements[0].isTime() || !input.elements[1].isTime()
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Validity elements must be of type time. ";
    }
    return false;
  }
  this->validityNotBefore = input.elements[0];
  this->validityNotAfter = input.elements[1];
  return true;
}

bool TBSCertificateInfo::Organization::loadFromASN(
  const ASNElement& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("TBSCertificateInfo::Organization::loadFromASN");
  MapList<
    std::string, ASNObject, HashFunctions::hashFunction<std::string>
  > fields;
  if (
    !ASNObject::loadFieldsFromASNSequence(input, fields, commentsOnFailure)
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to read certificate fields. Certificate fields decoded: "
      << fields.toStringHtml()
      << ". ";
    }
    return false;
  }
  return this->loadFields(fields, commentsOnFailure);
}

bool TBSCertificateInfo::Organization::loadFields(
  const MapList<
    std::string, ASNObject, HashFunctions::hashFunction<std::string>
  >& fields,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("TBSCertificateInfo::loadFields");
  int numberOfLoadedFields = 0;
  numberOfLoadedFields +=
  this->commonName.loadField(fields, ASNObject::names::commonName);
  numberOfLoadedFields +=
  this->countryName.loadField(fields, ASNObject::names::countryName);
  numberOfLoadedFields +=
  this->emailAddress.loadField(fields, ASNObject::names::emailAddress);
  numberOfLoadedFields +=
  this->localityName.loadField(fields, ASNObject::names::localityName);
  numberOfLoadedFields +=
  this->organizationalUnitName.loadField(
    fields, ASNObject::names::organizationalUnitName
  );
  numberOfLoadedFields +=
  this->organizationName.loadField(
    fields, ASNObject::names::organizationName
  );
  numberOfLoadedFields +=
  this->stateOrProvinceName.loadField(
    fields, ASNObject::names::stateOrProvinceName
  );
  if (numberOfLoadedFields < fields.size()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to recognize "
      << (fields.size() - numberOfLoadedFields)
      << " fields in your certificate. ";
    }
    return false;
  }
  return true;
}

bool TBSCertificateInfo::loadASNAlgorithmIdentifier(
  const ASNElement& input,
  ASNElement& output,
  std::stringstream* commentsOnFailure
) {
  if (
    input.tag != AbstractSyntaxNotationOneSubsetDecoder::tags::sequence0x10 ||
    input.elements.size != 2
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Algorithm identifier outer layer "
      << "must be a sequence of two elements. ";
    }
    return false;
  }
  if (
    !input.hasSubElementGetCopy({0},
      {AbstractSyntaxNotationOneSubsetDecoder::tags::objectIdentifier0x06},
      output,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to get signature algorithm id. ";
    }
    return false;
  }
  if (
    !input.hasSubElementConst({1},
      {AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05},
      nullptr,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Signature algorithm id must end in null. ";
    }
    return false;
  }
  return true;
}

bool TBSCertificateInfo::load(
  const ASNElement& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("TBSCertificateInfo::load");
  if (input.elements.size < 7) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Certificate ASN element needs at least 7 fields. ";
    }
    return false;
  }
  if (
    !input.hasSubElementOfType({0, 0}, {}, this->version, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load version. ";
    }
    return false;
  }
  if (
    !input.hasSubElementOfType({1}, {}, this->serialNumber, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read serial number. ";
    }
    return false;
  }
  if (
    !this->loadASNAlgorithmIdentifier(
      input.elements[2],
      this->signatureAlgorithmIdentifier,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to load certificate signature algorithm identifier. ";
    }
    return false;
  }
  if (
    !this->issuer.loadFromASN(input.elements[3], commentsOnFailure)
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to load issuer (authority that signed the certificate). ";
    }
    return false;
  }
  if (!this->loadValidity(input.elements[4], commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load validity. ";
    }
    return false;
  }
  if (
    !this->subject.loadFromASN(input.elements[5], commentsOnFailure)
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to load subject (owner of advertised public key). ";
    }
    return false;
  }
  if (
    !input.hasSubElementOfType({6, 1, 0, 1},
      {},
      this->subjectPublicKey.modulus,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read RSA modulus. ";
    }
    return false;
  }
  if (
    !input.hasSubElementOfType({6, 1, 0, 0},
      {},
      this->subjectPublicKey.exponent,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read public key. ";
    }
    return false;
  }
  if (
    this->subjectPublicKey.modulus == 0 || this->subjectPublicKey.exponent == 0
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Invalid RSA modulus or exponent. ";
    }
    return false;
  }
  if (input.elements.size >= 8) {
    return this->loadExtensions(input.elements[7], commentsOnFailure);
  }
  return true;
}

bool X509Certificate::loadFromASNEncoded(
  const List<unsigned char>& input,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("X509Certificate::loadFromASNEncoded");
  AbstractSyntaxNotationOneSubsetDecoder decoder;
  if (!decoder.decode(input, 0, this->sourceASN, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to asn-decode certificate input. ";
    }
    return false;
  }
  if (this->sourceASN.elements.size != 3) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Certificate ASN needs exactly three elements. ";
    }
    return false;
  }
  if (
    !this->information.load(this->sourceASN[0], commentsOnFailure)
  ) {
    return false;
  }
  if (
    this->sourceASN[2].tag !=
    AbstractSyntaxNotationOneSubsetDecoder::tags::bitString0x03
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Third element of certificate is required to be a bit string. ";
    }
    return false;
  }
  if (
    !this->information.loadASNAlgorithmIdentifier(
      this->sourceASN[1],
      this->signatureAlgorithmId,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load signature algorithm id. ";
    }
    return false;
  }
  this->signatureValue = this->sourceASN[2];
  return true;
}
