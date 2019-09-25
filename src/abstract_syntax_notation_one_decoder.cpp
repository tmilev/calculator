//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".

#include "abstract_syntax_notation_one_decoder.h"
#include "crypto.h"
#include "math_extra_algebraic_numbers.h"
#include "serialization_basic.h"

extern logger logWorker, logServer;

extern ProjectInformationInstance ProjectInfoAbstractSyntaxNotationOneDecoderImplementation;

ProjectInformationInstance ProjectInfoAbstractSyntaxNotationOneDecoderImplementation(
  __FILE__, "Abstract syntax notation one (ASN-1) implementation. "
);

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
    crash << "Uninitialized ASN1 output json. " << crash;
  }
  if (this->rawDatA == nullptr) {
    crash << "Uninitialized ASN1 raw data. " << crash;
  }
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeLengthIncrementDataPointer(
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
  LargeInt length = 0;
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
      crash << "Programming error: decode current "
      << "did not increment the data pointer. " << crash;
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
  if (subDecoder.Decode(*this->rawDatA, offsetAtStart + 1, subDecoderResult, nullptr)) {
    std::cout << "DEBUG: sub-decoder data pointer: "
    << subDecoder.dataPointer << " and this->datapointer: "
    << this->dataPointer << std::endl;
    std::cout << "DEBUG: sub-decoder result: "
    << subDecoderResult.ToString() << std::endl;
    if (subDecoder.dataPointer == this->dataPointer) {
      output.theElements.SetSize(0);
      output.theElements.AddOnTop(subDecoderResult);
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

LargeInt AbstractSyntaxNotationOneSubsetDecoder::VariableLengthQuantityDecode(
  const List<unsigned char>& input, int& inputOutputDataPointer
) {
  LargeInt result = 0;
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
  out << "objectId: " << this->InterpretAsObjectIdentifier();
  out << " meaning: ";
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
    LargeInt nextInt = AbstractSyntaxNotationOneSubsetDecoder::VariableLengthQuantityDecode(this->ASNAtom, i);
    resultStream << "." << nextInt.ToString();
  }
  return resultStream.str();
}

void ASNElement::WriteBytes(List<unsigned char>& output) const {
  // WARNING: writer needs to be a named object to avoid
  // gcc immediately calling the object's destructor.
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength
  notAnonymous(this->startByte, this->lengthPromised, output);
  if (this->ASNAtom.size > 0) {
    output.AddListOnTop(this->ASNAtom);
  } else {
    for (int i = 0; i < this->theElements.size; i ++) {
      this->theElements[i].WriteBytes(output);
    }
  }
}

void ASNElement::WriteBytesASNAtom(
  unsigned char inputStartByte,
  const List<unsigned char> &atomContent,
  List<unsigned char>& output
) {
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength
  notAnonymous(inputStartByte, atomContent.size, output);
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

const ASNElement& ASNElement::operator[](int index) const {
  return this->theElements[index];
}

bool ASNElement::HasSubElement(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  const ASNElement** whichElement,
  std::stringstream* commentsOnFailure
) const {
  const ASNElement* current = this;
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
bool ASNElement::HasSubElementOfType<LargeIntUnsigned>(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  LargeIntUnsigned& output,
  std::stringstream* commentsOnFailure
) const {
  const ASNElement* element = nullptr;
  if (!ASNElement::HasSubElement(desiredIndices, desiredTypes, &element, commentsOnFailure)) {
    return false;
  }
  return element->isIntegerUnsigned(&output, commentsOnFailure);
}

template < >
bool ASNElement::HasSubElementOfType<LargeInt>(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  LargeInt& output,
  std::stringstream* commentsOnFailure
) const {
  const ASNElement* element = nullptr;
  if (!ASNElement::HasSubElement(desiredIndices, desiredTypes, &element, commentsOnFailure)) {
    return false;
  }
  return element->isInteger(&output, commentsOnFailure);
}

template < >
bool ASNElement::HasSubElementOfType<List<unsigned char> >(
  const List<int>& desiredIndices,
  const List<unsigned char>& desiredTypes,
  List<unsigned char>& output,
  std::stringstream* commentsOnFailure
) const {
  const ASNElement* element = nullptr;
  if (!ASNElement::HasSubElement(desiredIndices, desiredTypes, &element, commentsOnFailure)) {
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
  return this->ToJSON().ToString(false, false, false, true);
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
std::string ASNElement::JSLabels::offset = "offset";
std::string ASNElement::JSLabels::startByteOriginal = "startByteOriginal";
std::string ASNElement::JSLabels::tag = "tag";
std::string ASNElement::JSLabels::type = "type";
std::string ASNElement::JSLabels::interpretation = "interpretation";
std::string ASNElement::JSLabels::numberOfChildren = "numberOfChildren";

void ASNElement::ToJSON(JSData& output) const {
  output.reset();
  output[ASNElement::JSLabels::tag] = StringRoutines::ConvertByteToHex(this->tag);
  output[ASNElement::JSLabels::startByteOriginal] = StringRoutines::ConvertByteToHex(this->startByte);
  output[ASNElement::JSLabels::lengthPromised] = this->lengthPromised;
  output[ASNElement::JSLabels::type] = AbstractSyntaxNotationOneSubsetDecoder::GetType(this->tag);
  output[ASNElement::JSLabels::offset] = this->offset;
  List<unsigned char> lengthEncoding;
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::WriteLength(
    static_cast<unsigned>(this->lengthPromised), lengthEncoding, 0
  );
  output[ASNElement::JSLabels::lengthEncoding] = Crypto::ConvertListUnsignedCharsToHex(lengthEncoding, 0, false);
  output[ASNElement::JSLabels::isConstructed] = this->flagIsConstructed;
  output[ASNElement::JSLabels::numberOfChildren] = this->theElements.size;
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
    ASNObject attemptToLoad;
    if (attemptToLoad.LoadFromASN(*this, nullptr)) {
      output[ASNElement::JSLabels::interpretation] = attemptToLoad.ToString();
    }
  }
  if (this->ASNAtom.size > 0 || !this->isComposite()) {
    output[ASNElement::JSLabels::body] = Crypto::ConvertListUnsignedCharsToHex(this->ASNAtom, 0, false);
    if (
      this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::utf8String0x0c ||
      this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::printableString0x13
    ) {
      output[ASNElement::JSLabels::interpretation] = this->ASNAtom.ToStringConcatenate();
    }
    if (this->tag == AbstractSyntaxNotationOneSubsetDecoder::tags::objectIdentifier0x06) {
      output[ASNElement::JSLabels::interpretation] = this->InterpretAsObjectIdentifierGetNameAndId();
    }
  }
}

bool ASNElement::isIntegerUnsigned(
  LargeIntUnsigned* whichInteger,
  std::stringstream* commentsOnFalse
) const {
  MacroRegisterFunctionWithName("ASNElement::isIntegerUnsigned");
  if (this->tag != AbstractSyntaxNotationOneSubsetDecoder::tags::integer0x02) {
    if (commentsOnFalse != nullptr) {
      *commentsOnFalse << "Element is not an integer. ";
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
  LargeInt result;
  if (!this->isInteger(&result, commentsOnFalse)) {
    crash << "ASNElement must be an integer at this point. " << crash;
  }
  *whichInteger = result.value;
  return true;
}

bool ASNElement::isInteger(LargeInt *whichInteger, std::stringstream *commentsOnFalse) const {
  MacroRegisterFunctionWithName("ASNElement::isInteger");
  if (this->tag != AbstractSyntaxNotationOneSubsetDecoder::tags::integer0x02) {
    if (commentsOnFalse != nullptr) {
      *commentsOnFalse << "Element is not an integer. ";
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
  this->flagIsConstructed = false;
  this->ASNAtom.SetSize(0);
  this->theElements.SetSize(0);
  this->lengthPromised = 0;
  this->tag = 0;
  this->startByte = 0;
  this->error = "";
  this->offset = 0;
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
  output.offset = this->dataPointer;
  if (this->PointerIsBad(output)) {
    return false;
  }
  output.startByte= (*this->rawDatA)[this->dataPointer];
  output.ComputeTag();
  this->dataPointer ++;
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
    << static_cast<int>(output.startByte) << " at position: " << output.offset
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
  int numBytes = AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::GetReservedBytesForLength(static_cast<signed>(input)) - 1;
  unsigned char lengthPlus128 = 128 + static_cast<unsigned char>(numBytes);
  output[offset] = lengthPlus128;
  offset ++;
  Serialization::WriteNByteUnsignedInt(
    numBytes,
    static_cast<unsigned int>(input),
    output,
    offset
  );
}

std::string AbstractSyntaxNotationOneSubsetDecoder::ToStringAnnotateBinary() {
  if (!this->CheckInitialization()) {
    return "ASN1 not initialized properly. ";
  }
  std::stringstream out;
  out << "<script>";
  out << "window.calculator.crypto.abstractSyntaxNotationAnnotate(";
  out << "\"" << Crypto::ConvertListUnsignedCharsToHex(*this->rawDatA, 0, false) << "\"";
  out << ", ";
  out << this->decodedData->ToString();
  out << ", ";
  List<unsigned char> idNonHexed;
  Crypto::computeSha256(*this->rawDatA, idNonHexed);
  std::string theID = Crypto::ConvertListUnsignedCharsToHex(idNonHexed, 0, false);
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
  List<unsigned char>& output) {
  if (expectedTotalElementByteLength < 0) {
    expectedTotalElementByteLength = 0;
  }
  this->outputPointer = &output;
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
  int actualBytesNeededForLength = this->GetReservedBytesForLength(this->totalByteLength);
  if (actualBytesNeededForLength > this->reservedBytesForLength) {
    logWorker << logger::red << "Wrong number of reserved bytes for sequence writer. "
    << "This is non-fatal but affects negatively performance. " << logger::endL;
    this->outputPointer->ShiftUpExpandOnTopRepeated(
      this->offset + 1, actualBytesNeededForLength - this->reservedBytesForLength
    );
  }
  if (actualBytesNeededForLength < this->reservedBytesForLength) {
    logWorker << logger::red << "Wrong number of reserved bytes for sequence writer. "
    << "This is non-fatal but affects negatively performance. " << logger::endL;
    this->outputPointer->RemoveIndicesShiftDown(
      this->offset + 1, this->reservedBytesForLength - actualBytesNeededForLength
    );
  }
  this->reservedBytesForLength = actualBytesNeededForLength;
  AbstractSyntaxNotationOneSubsetDecoder::WriterSequence::WriteLength(
    static_cast<unsigned>(this->totalByteLength),
    *this->outputPointer,
    this->offset + 1
  );
}

void AbstractSyntaxNotationOneSubsetDecoder::WriteUnsignedIntegerObject(
  const LargeIntUnsigned& input, List<unsigned char>& output
) {
  List<unsigned char> serialized;
  input.WriteBigEndianBytes(serialized);
  ASNElement::WriteBytesASNAtom(
    AbstractSyntaxNotationOneSubsetDecoder::tags::integer0x02,
    serialized,
    output
  );
}

void X509Certificate::WriteVersion(List<unsigned char>& output) {
  output.AddOnTop(0xa0);
  output.AddOnTop(0x03);
}

void TBSCertificateInfo::WriteBytesContentAndExpiry(List<unsigned char>& output) {
  this->WriteBytesContent(output);
  this->WriteBytesExpiry(output);
}

void TBSCertificateInfo::WriteBytesExpiry(List<unsigned char>& output) {
  // Anonymity breaks destructor generation in gcc!
  AbstractSyntaxNotationOneSubsetDecoder::WriterSequence notAnonymous(100, output);
  this->validityNotBefore.WriteBytes(output);
  this->validityNotAfter.WriteBytes(output);
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

void X509Certificate::WriteBytesAlgorithmIdentifier(List<unsigned char>& output) {
  // Anonymity breaks destructor generation in gcc!
  AbstractSyntaxNotationOneSubsetDecoder::WriterSequence notAnonymous(100, output);
  AbstractSyntaxNotationOneSubsetDecoder::WriteObjectId(this->signatureAlgorithmId, output);
  AbstractSyntaxNotationOneSubsetDecoder::WriteNull(output);
}

void ASNObject::WriteBytesASNObject(List<unsigned char>& output) {
  if (this->objectId.ASNAtom.size == 0) {
    // object id empty, field not initialized, do nothing
    return;
  }
  // Anonymity breaks destructor generation in gcc!
  AbstractSyntaxNotationOneSubsetDecoder::WriterSet notAnonymous1(100, output);
  AbstractSyntaxNotationOneSubsetDecoder::WriterSequence notAnonymous2(100, output);
  this->objectId.WriteBytes(output);
  this->content.WriteBytes(output);
}

void ASNObject::initializeAddSample(
  MapList<std::string, ASNObject, MathRoutines::HashString>& container,
  const std::string& inputName,
  const std::string& inputObjectIdHex,
  unsigned char inputContentTag
) {
  ASNObject incoming;
  incoming.name = inputName;
  incoming.content.tag = inputContentTag;
  std::stringstream commentsOnFailure;
  if (!Crypto::ConvertHexToListUnsignedChar(
    inputObjectIdHex, incoming.objectId.ASNAtom, &commentsOnFailure
  )) {
    crash << "Failure in certificate field initialization is not allowed. " << crash;
  }
  container.SetKeyValue(incoming.name, incoming);
}

MapList<List<unsigned char>, ASNObject, MathRoutines::HashListUnsignedChars>& ASNObject::ObjectIdsToNames() {
  static MapList<List<unsigned char>, ASNObject, MathRoutines::HashListUnsignedChars> result;
  return result;
}

std::string ASNObject::names::sha256WithRSAEncryption = "sha256WithRSAEncryption";
std::string ASNObject::names::countryName             = "countryName"            ;
std::string ASNObject::names::stateOrProvinceName     = "stateOrProvinceName"    ;
std::string ASNObject::names::localityName            = "localityName"           ;
std::string ASNObject::names::organizationName        = "organizationName"       ;
std::string ASNObject::names::organizationalUnitName  = "organizationalUnitName" ;
std::string ASNObject::names::commonName              = "commonName"             ;
std::string ASNObject::names::emailAddress            = "emailAddress"           ;

MapList<std::string, ASNObject, MathRoutines::HashString>& ASNObject::NamesToObjectIdsNonThreadSafe() {
  static MapList<std::string, ASNObject, MathRoutines::HashString> container;
  if (container.size() == 0) {
    ASNObject::initializeAddSample(
      container,
      ASNObject::names::sha256WithRSAEncryption ,
      "2a864886f70d01010b",
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
    MapList<List<unsigned char>, ASNObject, MathRoutines::HashListUnsignedChars>& reverseMap = ASNObject::ObjectIdsToNames();
    for (int i = 0; i < container.theValues.size; i ++) {
      ASNObject& current = container.theValues[i];
      reverseMap.SetKeyValue(current.objectId.ASNAtom, current);
    }
  }
  return container;
}

int ASNObject::LoadField(const MapList<std::string, ASNObject, MathRoutines::HashString>& inputFields, const std::string& fieldName) {
  if (!ASNObject::NamesToObjectIdsNonThreadSafe().Contains(fieldName)) {
    crash << "Field " << fieldName << " is hard-coded but is yet unknown. " << crash;
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
  out << "objectId: " << Crypto::ConvertListUnsignedCharsToHex(this->objectId.ASNAtom, 0, false)
  << ", name: " << Crypto::ConvertStringToHex(this->name, 0, false);
  std::string content = this->content.ASNAtom.ToStringConcatenate();
  out << ", content: " << StringRoutines::ConvertStringToHexIfNonReadable(content, 0, false);
  return out.str();
}

std::string ASNObject::ToStringAllRecognizedObjectIds() {
  std::stringstream out;
  out << "There are: " << ASNObject::ObjectIdsToNames().size() << " known fields. ";
  for (int i = 0; i < ASNObject::ObjectIdsToNames().size(); i ++) {
    ASNObject& current = ASNObject::ObjectIdsToNames().theValues[i];
    const List<unsigned char>& currentId = ASNObject::ObjectIdsToNames().theKeys[i];
    out << Crypto::ConvertListUnsignedCharsToHex(currentId, 50, false) << ": " << current.name << "\n";
  }
  return out.str();
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
  const ASNElement& internal = input[0];

  if (
    internal.tag != AbstractSyntaxNotationOneSubsetDecoder::tags::sequence0x10 ||
    internal.theElements.size != 2
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "ASNObject JSON representation must be a two-element sequence. ";
    }
    return false;
  }
  this->objectId = internal[0];
  if (this->objectId.tag != AbstractSyntaxNotationOneSubsetDecoder::tags::objectIdentifier0x06) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Missing objectIdentifierBytes key. ";
    }
    return false;
  }
  if (!ASNObject::ObjectIdsToNames().Contains(this->objectId.ASNAtom)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Unrecognized object id. ";
    }
    return false;
  }
  *this = ASNObject::ObjectIdsToNames().GetValueCreateNoInit(this->objectId.ASNAtom);
  const ASNElement& second = internal[1];
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

void X509Certificate::WriteBytesASN1(List<unsigned char>& output) {
  MacroRegisterFunctionWithName("X509Certificate::WriteBytesASN1");
  // Anonymity breaks destructor generation in gcc!
  AbstractSyntaxNotationOneSubsetDecoder::WriterSequence writeTotalLength(2000, output);
  AbstractSyntaxNotationOneSubsetDecoder::WriterSequence writeMetadata(2000, output);
  this->WriteVersion(output);
  AbstractSyntaxNotationOneSubsetDecoder::WriteUnsignedIntegerObject(2, output);
  AbstractSyntaxNotationOneSubsetDecoder::WriteUnsignedIntegerObject(this->serialNumber, output);
  this->WriteBytesAlgorithmIdentifier(output);
  this->information.WriteBytesContentAndExpiry(output);
}

std::string X509Certificate::ToStringTestEncode() {
  std::stringstream out;
  std::string sourceHex = Crypto::ConvertListUnsignedCharsToHex(this->sourceBinary, 0, false);
  List<unsigned char> recoded;
  this->WriteBytesASN1(recoded);
  std::string recodedHex = Crypto::ConvertListUnsignedCharsToHex(recoded, 0, false);
  out << "Original, recoded binary source:<br>"
  << StringRoutines::Differ::DifferenceHTMLStatic(sourceHex, recodedHex);
  return out.str();
}

std::string X509Certificate::ToString() {
  std::stringstream out;
  out << "Certificate RSA:<br>"
  << this->theRSA.ToString();
  out << "<br>Source binary: " << this->sourceBinary.size << " bytes.<br>";
  out << this->information.ToString();
  AbstractSyntaxNotationOneSubsetDecoder theDecoder;
  theDecoder.decodedData = &this->sourceASN;
  theDecoder.rawDatA = &this->sourceBinary;
  out << theDecoder.ToStringAnnotateBinary();
  return out.str();
}

std::string PrivateKeyRSA::ToString() const {
  std::stringstream out;
  out << "Exponent: " << this->exponent << "<br>Prime 1: "
  << this->primeOne << "<br>Prime 2: " << this->primeTwo;
  return out.str();
}

bool PrivateKeyRSA::BasicChecks(std::stringstream* comments) {
  std::stringstream commentsContainer;
  if (comments == nullptr) {
    comments = &commentsContainer;
  }
  LargeInt mustBeZero;
  LargeInt primeOneLI = this->primeOne;
  LargeInt primeTwoLI = this->primeTwo;
  mustBeZero = this->primeOne;
  mustBeZero *= this->primeTwo;
  mustBeZero -= this->modulus;
  *comments << "Must be zero: " << mustBeZero.ToString();
  LargeInt EulerPhi = (primeOneLI - 1) * (primeTwoLI - 1);
  ElementZmodP mustBeZeroModP;
  //theExponent.theModulo = EulerPhi.value;
  //theExponent.theValue = this->exponent;
  mustBeZeroModP.theModulo = EulerPhi.value;
  mustBeZeroModP.theValue = 1;
  mustBeZeroModP /= this->exponent;
  mustBeZeroModP -= this->privateExponent;
  *comments << "<br>Difference between private exponent and computed one: " << mustBeZeroModP.theValue.ToString();
  return true;
}

bool PrivateKeyRSA::LoadFromPEMFile(const std::string& input, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("PrivateKeyRSA::LoadFromPEMFile");
  std::string certificateContent;
  // No access to sensitive folders here, so this cannot be used for the server's private key.
  // For server's certificate, use TransportLayerSecurity::LoadPEMPrivateKey.
  if (!FileOperations::LoadFileToStringVirtual(input, certificateContent, false, false, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load key file. ";
    }
    return false;
  }
  return this->LoadFromPEM(certificateContent, commentsOnFailure);
}

bool PrivateKeyRSA::LoadFromPEM(const std::string& input, std::stringstream* commentsOnFailure){
  MacroRegisterFunctionWithName("PrivateKeyRSA::LoadFromPEM");
  std::string certificateContentStripped = StringRoutines::StringTrimWhiteSpace(input);
  if (!StringRoutines::StringBeginsWith(certificateContentStripped, "-----BEGIN PRIVATE KEY-----", &certificateContentStripped)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad private key start. ";
    }
    return false;
  }
  if (!StringRoutines::StringEndsWith(certificateContentStripped, "-----END PRIVATE KEY-----", &certificateContentStripped)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad private key end. ";
    }
    return false;
  }
  certificateContentStripped = StringRoutines::StringTrimWhiteSpace(certificateContentStripped);
  if (!Crypto::ConvertBase64ToBitStream(certificateContentStripped, this->sourceBinary, commentsOnFailure, nullptr)) {
    return false;
  }
  return this->LoadFromASNEncoded(this->sourceBinary, commentsOnFailure);
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
  if (!this->sourceASNOuter.HasSubElement(
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
    {1}, {}, this->modulus, commentsOnFailure
  )) {
    return false;
  }
  if (!this->sourceASNInner.HasSubElementOfType(
    {2}, {}, this->exponent, commentsOnFailure
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
  LargeInt exponent1, exponent2, coefficient;
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
    input, certificateContent, false, false, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load key file. ";
    }
    return false;
  }
  return this->LoadFromPEM(certificateContent, commentsOnFailure);
}

std::string TBSCertificateInfo::ToString() {
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

void TBSCertificateInfo::WriteBytesContent(List<unsigned char>& output) {
  // Anonymity breaks destructor generation in gcc!
  AbstractSyntaxNotationOneSubsetDecoder::WriterSequence writeTBSCertificateContent(200, output);
  this->countryName           .WriteBytesASNObject(output);
  this->stateOrProvinceName   .WriteBytesASNObject(output);
  this->localityName          .WriteBytesASNObject(output);
  this->organizationName      .WriteBytesASNObject(output);
  this->organizationalUnitName.WriteBytesASNObject(output);
  this->commonName            .WriteBytesASNObject(output);
  this->emailAddress          .WriteBytesASNObject(output);
}

bool TBSCertificateInfo::LoadValidityFromASN(
  const ASNElement& input, std::stringstream* commentsOnFailure
) {
  if (
    input.tag != AbstractSyntaxNotationOneSubsetDecoder::tags::sequence0x10 ||
    input.theElements.size != 2
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Validity element must be a sequence of two elements. ";
    }
    return false;
  }
  if (!input[0].isTime() || !input[1].isTime()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Validity elements must be of type time. ";
    }
    return false;
  }
  this->validityNotAfter = input[0];
  this->validityNotBefore = input[1];
  return true;
}

bool TBSCertificateInfo::LoadFieldsFromASN(const ASNElement& input, std::stringstream* commentsOnFailure) {
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

bool TBSCertificateInfo::LoadFields(
  const MapList<std::string, ASNObject, MathRoutines::HashString>& fields,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("TBSCertificateInfo::LoadFields");
  int numberOfLoadedFields = 0;
  numberOfLoadedFields += this->commonName.LoadField(
    fields, ASNObject::names::commonName
  );
  numberOfLoadedFields += this->commonName.LoadField(
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

bool X509Certificate::LoadFromASNEncoded(
  const List<unsigned char>& input, std::stringstream* commentsOnFailure
) {
  AbstractSyntaxNotationOneSubsetDecoder theDecoder;
  if (!theDecoder.Decode(input, 0, this->sourceASN, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to asn-decode certificate input. ";
    }
    return false;
  }
  if (!this->sourceASN.HasSubElementOfType(
    {0, 1}, {}, this->serialNumber, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read serial number. ";
    }
    return false;
  }
  if (!this->sourceASN.HasSubElementOfType(
    {0, 2, 0}, {}, this->signatureAlgorithmId, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to get signature algorithm id. ";
    }
    return false;
  }
  const ASNElement* certificateFieldsASN = nullptr;
  if (!this->sourceASN.HasSubElement({0, 3}, {}, &certificateFieldsASN, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to get sequence of certificate fields. ";
    }
    return false;
  }
  if (!this->information.LoadFieldsFromASN(*certificateFieldsASN, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read certificate fields. ";
    }
    return false;
  }
  const ASNElement* validityASN = nullptr;
  if (!this->sourceASN.HasSubElement({0, 4}, {}, &validityASN, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to extract certificate validity. ";
    }
    return false;
  }
  if (!this->information.LoadValidityFromASN(*validityASN, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load validity. ";
    }
    return false;
  }
  if (!this->sourceASN.HasSubElementOfType(
    {0, 6, 1, 0, 1}, {}, this->theRSA.theModuluS, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read RSA modulus. ";
    }
    return false;
  }
  if (!this->sourceASN.HasSubElementOfType(
    {0, 6, 1, 0, 0}, {}, this->theRSA.theExponenT, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read public key. ";
    }
    return false;
  }
  if (this->theRSA.theModuluS == 0 || this->theRSA.theExponenT == 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Invalid RSA modulus or exponent. ";
    }
    return false;
  }
  return true;
}

