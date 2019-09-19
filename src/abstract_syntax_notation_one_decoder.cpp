//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".

#include "abstract_syntax_notation_one_decoder.h"
#include "crypto.h"
#include "math_extra_algebraic_numbers.h"
#include "serialization_basic.h"

extern logger logWorker;

extern ProjectInformationInstance ProjectInfoAbstractSyntaxNotationOneDecoderImplementation;

ProjectInformationInstance ProjectInfoAbstractSyntaxNotationOneDecoderImplementation(__FILE__, "Abstract syntax notation one (ASN-1) implementation");

// Putting this in the header file currently breaks the linking for this particular tag.
// Appears to be a compiler/linker bug (?).
const unsigned char AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05 = 5;

bool AbstractSyntaxNotationOneSubsetDecoder::isCostructedByte(unsigned char input) {
  int sixthBit = input / 32;
  sixthBit %= 2;
  return sixthBit != 0;
}

bool AbstractSyntaxNotationOneSubsetDecoder::hasBit7Set(unsigned char input) {
  int seventhBit = input / 64;
  seventhBit %= 2;
  return seventhBit != 0;
}

bool AbstractSyntaxNotationOneSubsetDecoder::hasBit8Set(unsigned char input) {
  int eighthBit = input / 128;
  eighthBit %= 2;
  return eighthBit != 0;
}

AbstractSyntaxNotationOneSubsetDecoder::AbstractSyntaxNotationOneSubsetDecoder() {
  this->flagLogByteInterpretation = false;
  this->dataInterpretation = nullptr;
  this->decodedData = nullptr;
  this->dataPointer = 0;
  this->recursionDepthGuarD = 0;
  this->maxRecursionDepth = 20;
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

void AbstractSyntaxNotationOneSubsetDecoder::reset() {
  this->rawDatA = nullptr;
  this->dataInterpretation = nullptr;
  this->decodedData = nullptr;
  this->dataPointer = 0;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeLengthIncrementDataPointer(
  int& outputLengthNegativeOneForVariable,
  JSData* interpretation
) {
  outputLengthNegativeOneForVariable = 0;
  int startDataPointer = this->dataPointer;
  unsigned char currentByte = (*this->rawDatA)[this->dataPointer];
  if (currentByte < 128) { // 128 = 0x80
    outputLengthNegativeOneForVariable = currentByte;
    this->dataPointer ++;
    if (interpretation != nullptr) {
      (*interpretation)["lengthEncoding"] = StringRoutines::ConvertByteToHex(currentByte);
    }
    return true;
  }
  if (currentByte == 128) { // 128 = 0x80 signals varible-length encoding
    outputLengthNegativeOneForVariable = - 1;
    this->dataPointer ++;
    if (interpretation != nullptr) {
      (*interpretation)["lengthEncoding"] = StringRoutines::ConvertByteToHex(80);
    }
    return true;
  }
  int numberOfLengthBytes = currentByte - 128;
  if (interpretation != nullptr) {
    List<unsigned char> lengthBytes = this->rawDatA->SliceCopy(this->dataPointer, 1 + numberOfLengthBytes);
    (*interpretation)["lengthEncoding"] = Crypto::ConvertListUnsignedCharsToHex(lengthBytes, 0, false);
  }
  LargeInt length = 0;
  this->dataPointer ++;
  for (int i = 0; i < numberOfLengthBytes; i ++) {
    length *= 256;
    if (this->PointerIsBad(interpretation)) {
      return false;
    }
    unsigned char nextByte = (*this->rawDatA)[this->dataPointer];
    length += static_cast<signed>(static_cast<unsigned int>(nextByte));
    this->dataPointer ++;
  }
  if (!length.IsIntegerFittingInInt(&outputLengthNegativeOneForVariable)) {
    if (interpretation != nullptr) {
      std::stringstream errorStream;
      errorStream << "Variable length: " << length << " whose decoding started at position: "
      << startDataPointer << " with first excluded position: " << this->dataPointer << " is too large. ";
      (*interpretation)["error"] = errorStream.str();
    }
    return false;
  }
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::PointerIsBad(JSData *interpretation) {
  if (this->dataPointer >= this->rawDatA->size || this->dataPointer < 0) {
    if (interpretation != nullptr) {
      std::stringstream errorStream;
      errorStream << "Unexpected overflow error: data pointer is negative. ";
      (*interpretation)["error"] = errorStream.str();
    }
    return true;
  }
  return false;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeSequenceLikeContent(
  int desiredLengthInBytes, JSData& output, JSData* interpretation
) {
  output.reset(JSData::token::tokenArray);
  if (interpretation != nullptr) {
    (*interpretation)["value"].reset(JSData::token::tokenArray);
  }
  int lastIndexPlusOne = this->dataPointer + desiredLengthInBytes;
  JSData nextElement, nextElementInterpretationContainer;
  JSData* nextElementInterpretation = nullptr;
  if (interpretation != nullptr) {
    nextElementInterpretation = &nextElementInterpretationContainer;
  }
  int numberOfDecoded = 0;
  while (this->dataPointer < lastIndexPlusOne) {
    int lastPointer = this->dataPointer;
    bool isGood = this->DecodeCurrent(nextElement, nextElementInterpretation);
    if (interpretation != nullptr) {
      (*interpretation)["value"].theList.AddOnTop(*nextElementInterpretation);
    }
    if (!isGood){
      if (interpretation != nullptr) {
        std::stringstream errorStream;
        errorStream << "Failed to decode sequence element of index: " << numberOfDecoded << ". ";
        (*interpretation)["error"] = errorStream.str();
      }
      return false;
    }
    if (interpretation != nullptr) {
      (*interpretation)["numberOfElements"] = numberOfDecoded;
    }
    if (lastPointer >= this->dataPointer) {
      crash << "Programming error: decode current did not increment the data pointer. " << crash;
    }
    output.theList.AddOnTop(nextElement);
    numberOfDecoded ++;
  }
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeUTCString(
  int desiredLengthInBytes, JSData& output, JSData* interpretation
) {
  JSData theTime;
  JSData timeInterpretationContainer;
  JSData* timeInterpretation = interpretation == nullptr ? nullptr : &timeInterpretationContainer;
  bool success = this->DecodeOctetString(desiredLengthInBytes, theTime, timeInterpretation);
  if (interpretation != nullptr) {
    (*interpretation)["time"] = timeInterpretationContainer;
  }
  if (!success) {
    return false;
  }
  output["time"] = theTime;
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeBitString(
  int desiredLengthInBytes, JSData& output, JSData* interpretation
) {
  JSData theBitString, theBitStringInterpretationContainer;
  JSData* theBitStringInterpretation = interpretation == nullptr ? nullptr : & theBitStringInterpretationContainer;
  bool success = this->DecodeOctetString(desiredLengthInBytes, theBitString, theBitStringInterpretation);
  if (interpretation != nullptr) {
    (*interpretation)["bitString"] = *theBitStringInterpretation;
  }
  if (!success) {
    return false;
  }
  output["bitString"] = theBitString;
  AbstractSyntaxNotationOneSubsetDecoder subDecoder;
  JSData subDecoderInterpretationContainer, subDecoderResult;
  JSData* subDecoderInterpretation = nullptr;
  if (this->flagLogByteInterpretation) {
    subDecoderInterpretation = &subDecoderInterpretationContainer;
  }
  List<unsigned char> data;
  data = theBitString.theString;
  if (data.size > 0) {
    data.PopIndexShiftDown(0);
  }
  subDecoder.recursionDepthGuarD = this->recursionDepthGuarD + 1;
  if (subDecoder.Decode(data, subDecoderResult, subDecoderInterpretation, nullptr)) {
    output["bitStringDecoded"] = subDecoderResult;
    if (subDecoderInterpretation != nullptr) {
      output["bitStringDecodedInterpretation"] = *subDecoderInterpretation;
    }
  }
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeUTF8String(
  int desiredLengthInBytes, JSData& output, JSData* interpretation
) {
  return this->DecodeOctetString(desiredLengthInBytes, output, interpretation);
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeIA5String(
  int desiredLengthInBytes, JSData& output, JSData* interpretation
) {
  return this->DecodeOctetString(desiredLengthInBytes, output, interpretation);
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodePrintableString(
  int desiredLengthInBytes, JSData& output, JSData* interpretation
) {
  return this->DecodeOctetString(desiredLengthInBytes, output, interpretation);
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeOctetString(
  int desiredLengthInBytes, JSData& output, JSData* interpretation
) {
  output.reset(JSData::token::tokenString);
  output.theString.resize(static_cast<unsigned>(desiredLengthInBytes));
  for (int i = 0; i < desiredLengthInBytes; i ++) {
    output.theString[static_cast<unsigned>(i)] = static_cast<char>((*this->rawDatA)[this->dataPointer + i]);
  }
  this->dataPointer += desiredLengthInBytes;
  if (interpretation != nullptr) {
    (*interpretation)["value"] = output.theString;
  }
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeNull(
  int desiredLengthInBytes, JSData& output, JSData* interpretation
) {
  if (desiredLengthInBytes != 0) {
    if (interpretation != nullptr) {
      (*interpretation)["error"] = "Length of null object is not zero. ";
    }
    return false;
  }
  output.reset(JSData::token::tokenNull);
  if (interpretation != nullptr) {
    (*interpretation)["value"].reset(JSData::token::tokenNull);
  }
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

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeObjectIdentifier(
  int desiredLengthInBytes, JSData& output, JSData *interpretation
) {
  if (this->PointerIsBad(interpretation)) {
    return false;
  }
  std::stringstream resultStream;
  List<unsigned char> data = this->rawDatA->SliceCopy(this->dataPointer, desiredLengthInBytes);

  output["objectIdentifierBytes"] = std::string(
    reinterpret_cast<char*>(data.TheObjects),
    static_cast<unsigned>(data.size)
  );
  unsigned char firstByte = (*this->rawDatA)[this->dataPointer];
  int currentPointer = this->dataPointer + 1;
  this->dataPointer += desiredLengthInBytes;
  if (this->PointerIsBad(interpretation)) {
    if (interpretation != nullptr) {
      std::stringstream errorStream;
      errorStream << "Object identifier length overshoots the total raw data length. ";
      (*interpretation)["error"] = errorStream.str();
    }
    return false;
  }
  // first two entries are encoded in the first byte:
  unsigned char firstEntry = firstByte / 40;
  unsigned char secondEntry = firstByte % 40;
  resultStream << static_cast<int>(firstEntry) << "." << static_cast<int>(secondEntry);
  for (; currentPointer < this->dataPointer; ) {
    LargeInt nextInt = this->VariableLengthQuantityDecode(*this->rawDatA, currentPointer);
    resultStream << "." << nextInt.ToString();
  }
  if (interpretation != nullptr) {
    (*interpretation)["value"] = resultStream.str();
    if (ASNObject::ObjectIdsToNames().Contains(data)) {
      (*interpretation)["interpretation"] = ASNObject::ObjectIdsToNames().GetValueCreateNoInit(data).name;
    }
  }
  output["objectIdentifier"] = resultStream.str();
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeIntegerContent(
  int desiredLengthInBytes, JSData& output, JSData* interpretation
) {
  LargeInt reader = 0;
  int currentContribution = 0;

  for (int i = 0; i < desiredLengthInBytes; i ++) {
    unsigned char unsignedContribution = (*this->rawDatA)[this->dataPointer];
    currentContribution = unsignedContribution;
    if (i == 0) {
      if (currentContribution >= 128) {
        currentContribution -= 255;
      }
    }
    reader *= 256;
    reader += currentContribution;
    this->dataPointer ++;
  }
  output = reader;
  if (interpretation != nullptr) {
    (*interpretation)["value"] = reader.ToString();
    int startIndex = this->dataPointer - desiredLengthInBytes;
    std::string theSource(
      reinterpret_cast<char *>(this->rawDatA->TheObjects + startIndex),
      static_cast<unsigned>(desiredLengthInBytes)
    );
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

void AbstractSyntaxNotationOneSubsetDecoder::WriteNull(List<unsigned char>& output) {
  MacroRegisterFunctionWithName("AbstractSyntaxNotationOneSubsetDecoder::WriteNull");
  output.AddOnTop(AbstractSyntaxNotationOneSubsetDecoder::tags::null0x05);
  output.AddOnTop(0);
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeCurrent(JSData& output, JSData* interpretation) {
  MacroRegisterFunctionWithName("AbstractSyntaxNotationOneSubsetDecoder::DecodeCurrent");
  RecursionDepthCounter recursionGuard(&this->recursionDepthGuarD);
  if (this->recursionDepthGuarD > this->maxRecursionDepth) {
    if (interpretation != nullptr) {
      std::stringstream errorStream;
      errorStream << "Max depth of " << this->maxRecursionDepth << " exceeded. ";
      (*interpretation)["error"] = errorStream.str();
    }
    return false;
  }
  output.reset(JSData::token::tokenUndefined);
  if (interpretation != nullptr) {
    interpretation->reset(JSData::token::tokenUndefined);
  }
  if (this->PointerIsBad(interpretation)) {
    return false;
  }
  unsigned char startByte = (*this->rawDatA)[this->dataPointer];
  unsigned char startByteOriginal = startByte;
  int dataPointerAtStart = this->dataPointer;
  bool isConstructed = this->isCostructedByte(startByte);
  if (isConstructed) {
    if (interpretation != nullptr) {
      (*interpretation)["constructed"] = true;
    }
    if (
      startByte != AbstractSyntaxNotationOneSubsetDecoder::tags::timeOfDay0x20 &&
      startByte != AbstractSyntaxNotationOneSubsetDecoder::tags::dateTime0x21 &&
      startByte != AbstractSyntaxNotationOneSubsetDecoder::tags::duration0x22
    ) {
      startByte -= 32;
    }
  }
  bool hasBit7 = this->hasBit7Set(startByte);
  if (hasBit7) {
    startByte -= 64;
  }
  bool hasBit8 = this->hasBit8Set(startByte);
  if (hasBit8) {
    startByte -= 128;
  }
  if (interpretation != nullptr) {
    (*interpretation)["offset"] = this->dataPointer;
    (*interpretation)["startByteOriginal"] =  StringRoutines::ConvertByteToHex(startByteOriginal);
    (*interpretation)["startByte"] = StringRoutines::ConvertByteToHex(startByte);
    if (isConstructed) {
      (*interpretation)["constructed"] = true;
    }
    std::stringstream leadingBits;
    leadingBits << static_cast<int>(hasBit8) << static_cast<int>(hasBit7);
    (*interpretation)["leadingBits"] = leadingBits.str();
  }
  this->dataPointer ++;
  int currentLength = 0;
  if (!this->DecodeLengthIncrementDataPointer(currentLength, interpretation)) {
    return false;
  }
  if (currentLength < 0) {
    if (interpretation != nullptr) {
      std::stringstream errorStream;
      errorStream << "Failed to decode variable length. ";
      (*interpretation)["error"] = errorStream.str();
    }
    return false;
  }
  if (interpretation != nullptr) {
    (*interpretation)["length"] = currentLength;
    (*interpretation)["type"] = this->GetType(startByte);
    List<unsigned char> body = this->rawDatA->SliceCopy(this->dataPointer, currentLength);
    (*interpretation)["body"] = Crypto::ConvertListUnsignedCharsToHex(body, 0, false);
  }
  std::stringstream errorStream;
  switch (startByte) {
  case tags::reserved0:
    return this->DecodeSequenceLikeContent(currentLength, output, interpretation);
  case tags::integer0x02:
    return this->DecodeIntegerContent(currentLength, output, interpretation);
  case tags::octetString0x04:
    return this->DecodeOctetString(currentLength, output, interpretation);
  case tags::objectIdentifier0x06:
    return this->DecodeObjectIdentifier(currentLength, output, interpretation);
  case tags::null0x05:
    return this->DecodeNull(currentLength, output, interpretation);
  case tags::sequence0x10:
    return this->DecodeSequenceLikeContent(currentLength, output, interpretation);
  case tags::set0x11:
    return this->DecodeSequenceLikeContent(currentLength, output, interpretation);
  case tags::printableString0x13:
    return this->DecodePrintableString(currentLength, output, interpretation);
  case tags::utf8String0x0c:
    return this->DecodeUTF8String(currentLength, output, interpretation);
  case tags::IA5String0x16:
    return this->DecodeIA5String(currentLength, output, interpretation);
  case tags::UTCTime0x17:
    return this->DecodeUTCString(currentLength, output, interpretation);
  case tags::bitString0x03:
    return this->DecodeBitString(currentLength, output, interpretation);
  default:
    if (interpretation != nullptr) {
      errorStream << "Unknown object tag: " << static_cast<int>(startByte) << ", byte: "
      << static_cast<int>(startByteOriginal) << " at position: " << dataPointerAtStart << ". Length: " << currentLength << ". ";
      (*interpretation)["error"] = errorStream.str();
    }
    return false;
  }
}

bool AbstractSyntaxNotationOneSubsetDecoder::Decode(
  const List<unsigned char>& inputRawData,
  JSData &outputDecodedData,
  JSData *outputInterpretation,
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
      *commentsOnError << "Input size: " << this->rawDatA->size << " too large, max allowed: " << maxAllowedSize << ". ";
    }
    return false;
  }
  this->reset();

  this->decodedData = &outputDecodedData;
  this->dataInterpretation = outputInterpretation;
  this->rawDatA = &inputRawData;
  this->CheckInitialization();
  this->decodedData->reset();
  this->flagLogByteInterpretation = (this->dataInterpretation != nullptr);
  if (this->flagLogByteInterpretation) {
    this->dataInterpretation->reset();
  }
  this->DecodeCurrent(*this->decodedData, this->dataInterpretation);
  if (commentsOnError != nullptr) {
    *commentsOnError << "DEBUG: So far: decoded data pointer, raw size: "
    << this->dataPointer << ", " << this->rawDatA->size << ". ";
  }
  if (this->dataPointer < this->rawDatA->size) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Decoded " << this->dataPointer
      << " bytes but the input had: " << this->rawDatA->size << ". ";
      if (this->dataInterpretation != nullptr) {
        *commentsOnError << "DEBUG: intepretation: " << this->dataInterpretation->ToString(false, true, false, true);
      }
    }
    return false;
  }
  return true;
}

void AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::WriteLength(
  unsigned int input, List<unsigned char>& output, int offset
) {
 if (input < 128) {
    unsigned char length = static_cast<unsigned char>(input);
    output[offset] = length;
    return;
  }
  int numBytes = AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength::GetReservedBytesForLength(static_cast<signed>(input));
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
  if (this->dataInterpretation == nullptr) {
    return "Annotation not available. ";
  }
  if (!this->CheckInitialization()) {
    return "ASN1 not initialized properly. ";
  }
  std::stringstream out;
  out << "<script>";
  out << "window.calculator.crypto.abstractSyntaxNotationAnnotate(";
  out << "\"" << Crypto::ConvertListUnsignedCharsToHex(*this->rawDatA, 0, false) << "\"";
  out << ", ";
  out << this->dataInterpretation->ToString(false, false, false, true);
  out << ", ";
  out << this->decodedData->ToString(false, false, false, true);
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

std::string AbstractSyntaxNotationOneSubsetDecoder::ToStringDebug() const {
  MacroRegisterFunctionWithName("AbstractSyntaxNotationOneSubsetDecoder::ToStringDebug");
  this->CheckInitialization();
  std::stringstream out;
  out << "Decoded so far:<br>\n"
  << this->decodedData->ToString(false, false, true, true);
  if (this->flagLogByteInterpretation) {
    out << "<br>Interpretation status.<br>" << this->dataInterpretation->ToString(false, true, true, true);
  }
  out << "<br>Data: ";
  out << StringRoutines::StringShortenInsertDots(Crypto::ConvertListUnsignedCharsToHex(*this->rawDatA, 70, true), 4000);
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
  bool isConstructed
) {
  if (expectedTotalElementByteLength < 0) {
    expectedTotalElementByteLength = 0;
  }
  this->outputPointer = &output;
  this->offset = output.size;
  if (isConstructed) {
    startByte += 32;
  }
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
  List<unsigned char> serializedInput;
  input.WriteBigEndianBytes(serializedInput);
  AbstractSyntaxNotationOneSubsetDecoder::WriterInteger writeInteger(serializedInput.size, output);
  output.AddListOnTop(serializedInput);
}

void X509Certificate::WriteVersion(List<unsigned char>& output) {
  output.AddOnTop(0xa0);
  output.AddOnTop(0x03);
}

void AbstractSyntaxNotationOneSubsetDecoder::WriteObjectId(
  const List<unsigned char>& input, List<unsigned char>& output
) {
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectId(input.size, output);
  output.AddListOnTop(input);
}

void X509Certificate::WriteBytesAlgorithmIdentifier(List<unsigned char>& output) {
  AbstractSyntaxNotationOneSubsetDecoder::WriterSequence writeAlgorithmIdentifier(100, output);
  AbstractSyntaxNotationOneSubsetDecoder::WriteObjectId(this->signatureAlgorithmId, output);
  AbstractSyntaxNotationOneSubsetDecoder::WriteNull(output);
}

void X509Certificate::WriteBytesTBSCertificate(List<unsigned char>& output) {
  MacroRegisterFunctionWithName("X509Certificate::WriteBytesTBSCertificate");
  AbstractSyntaxNotationOneSubsetDecoder::WriterSequence writeTBSCertificateSequence(2000, output);
  this->WriteVersion(output);
  AbstractSyntaxNotationOneSubsetDecoder::WriteUnsignedIntegerObject(2, output);
  AbstractSyntaxNotationOneSubsetDecoder::WriteUnsignedIntegerObject(this->serialNumber, output);
  this->WriteBytesAlgorithmIdentifier(output);
  this->information.WriteBytesASN1(output);
}

void ASNObject::WriteBytesASNObject(List<unsigned char>& output) {
  if (this->objectId.size == 0) {
    // object id empty, field not initialized, do nothing
    return;
  }
  AbstractSyntaxNotationOneSubsetDecoder::WriterSet writeSet(100, output);
  AbstractSyntaxNotationOneSubsetDecoder::WriterSequence writeSequence(100, output);
  AbstractSyntaxNotationOneSubsetDecoder::WriteObjectId(this->objectId, output);
  AbstractSyntaxNotationOneSubsetDecoder::WriterObjectFixedLength contentWriter(this->contentTag, this->content.size, output, false);
  output.AddListOnTop(this->content);
}

void ASNObject::initializeAddSample(
  MapList<std::string, ASNObject, MathRoutines::HashString>& container,
  const std::string& inputName,
  const std::string& inputObjectIdHex,
  unsigned char inputContentTag
) {
  ASNObject incoming;
  incoming.name = inputName;
  incoming.contentTag = inputContentTag;
  std::stringstream commentsOnFailure;
  if (!Crypto::ConvertHexToListUnsignedChar(inputObjectIdHex, incoming.objectId, &commentsOnFailure)) {
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
      reverseMap.SetKeyValue(current.objectId, current);
    }
    logWorker << "DEBUG: Container size: " << container.theValues.size;
    logWorker << "DEBUG: reverse Container size: " << reverseMap.theValues.size;
  }
  return container;
}

int ASNObject::LoadField(const MapList<std::string, ASNObject, MathRoutines::HashString>& inputFields, const std::string& fieldName) {
  if (!ASNObject::NamesToObjectIdsNonThreadSafe().Contains(fieldName)) {
    crash << "Field " << fieldName << " is hard-coded but is yet unknown. " << crash;
  }
  if (!inputFields.Contains(fieldName)) {
    this->name = fieldName;
    this->objectId.SetSize(0);
    return 0;
  }
  *this = inputFields.GetValueConstCrashIfNotPresent(fieldName);
  return 1;
}

std::string ASNObject::ToStringAllRecognizedObjectIds() {
  std::stringstream out;
  out << "There are: " << ASNObject::ObjectIdsToNames().size() << " known fields. ";
  out << "DEBUG: There are: " << ASNObject::NamesToObjectIdsNonThreadSafe().size() << " known fields. ";
  for (int i = 0; i < ASNObject::ObjectIdsToNames().size(); i ++) {
    ASNObject& current = ASNObject::ObjectIdsToNames().theValues[i];
    const List<unsigned char>& currentId = ASNObject::ObjectIdsToNames().theKeys[i];
    out << Crypto::ConvertListUnsignedCharsToHex(currentId, 50, false) << ": " << current.name << "\n";
  }
  return out.str();
}

bool ASNObject::LoadFieldsFromJSArray(
  const JSData& jsonArray,
  MapList<std::string, ASNObject, MathRoutines::HashString>& output,
  std::stringstream *commentsOnFailure
) {
  output.Clear();
  if (jsonArray.theType != JSData::token::tokenArray) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Input is not array. ";
    }
    return false;
  }
  for (int i = 0; i < jsonArray.theList.size; i ++) {
    ASNObject current;
    if (!current.LoadFromJSON(jsonArray.theList[i], commentsOnFailure)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Failed to load certificate entry index: " << i << ". ";
      }
      return false;
    }
  }
  return true;

}

bool ASNObject::LoadFromJSON(const JSData& inputShell,
  std::stringstream* commentsOnFailure
) {
  if (inputShell.theType != JSData::token::tokenArray || inputShell.theList.size != 1) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "ASNObject JSON outer layer must be a single sequence. ";
    }
    return false;
  }
  const JSData& input = inputShell.theList[0];

  if (input.theType != JSData::token::tokenArray || input.theList.size != 2) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "ASNObject JSON representation must be a two-element array. ";
      *commentsOnFailure << "DEBUG: instead i got: " << input.ToString(false, true, false, true);
    }
    return false;
  }
  JSData& first = input.theList[0];
  if (!first.HasKey("objectIdentifierBytes")) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Missing objectIdentifierBytes key. ";
    }
    return false;
  }
  if (!first.HasKey("objectIdentifierBytes")) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "objectIdentifierBytes key missing. ";
    }
    return false;
  }
  JSData objectIdJSON = first.GetValue("objectIdentifierBytes");
  if (objectIdJSON.theType != JSData::token::tokenString) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "objectIdentifierBytes key required to have a string value. ";
    }
    return false;
  }
  this->objectId = objectIdJSON.theString;
  if (!ASNObject::ObjectIdsToNames().Contains(this->objectId)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Unrecognized object id. ";
      *commentsOnFailure << "DEBUG: and the id: " << Crypto::ConvertListUnsignedCharsToHex(this->objectId, 0, false)
      << ". ";
      *commentsOnFailure << "DEBUG: all ids: " << ASNObject::ToStringAllRecognizedObjectIds();
    }
    return false;
  }
  *this = ASNObject::ObjectIdsToNames().GetValueCreateNoInit(this->objectId);
  this->content.SetSize(0);
  JSData& second = input.theList[1];
  if (second.theType == JSData::token::tokenNull) {
    return true;
  }
  if (second.theType != JSData::token::tokenString) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Object id was OK but I failed to extract field value. ";
    }
    return false;
  }
  this->content = second.theString;
  return true;
}

void ASNObject::initializeNonThreadSafe() {
  MacroRegisterFunctionWithName("ASNObject::initializeNonThreadSafe");
  ASNObject::NamesToObjectIdsNonThreadSafe();
}

void X509Certificate::WriteBytesASN1(List<unsigned char>& output) {
  MacroRegisterFunctionWithName("X509Certificate::WriteBytesASN1");
  AbstractSyntaxNotationOneSubsetDecoder::WriterSequence writeTotalLength(2000, output);
  this->information.WriteBytesASN1(output);
}

std::string X509Certificate::ToStringTestEncode() {
  std::stringstream out;
  std::string sourceHex = Crypto::ConvertListUnsignedCharsToHex(this->sourceBinary, 80, true);
  List<unsigned char> recoded;
  this->WriteBytesASN1(recoded);
  std::string recodedHex = Crypto::ConvertListUnsignedCharsToHex(recoded, 80, true);
  out << "Original, recoded binary source:<br>"
  << StringRoutines::Differ::DifferenceHTMLStatic(sourceHex, recodedHex);
  return out.str();
}

std::string X509Certificate::ToString() {
  std::stringstream out;
  out << "Certificate RSA:<br>"
  << this->theRSA.ToString();
  out << "<br>Source binary: " << this->sourceBinary.size << " bytes.<br>";
  AbstractSyntaxNotationOneSubsetDecoder theDecoder;
  theDecoder.dataInterpretation = &this->sourceInterpretation;
  theDecoder.decodedData = &this->sourceJSON;
  theDecoder.rawDatA = &this->sourceBinary;
  out << theDecoder.ToStringAnnotateBinary();
  return out.str();
}

std::string PrivateKeyRSA::ToString() const {
  std::stringstream out;
  out << "Exponent: " << this->exponent << "<br>Prime 1: " << this->primeOne << "<br>Prime 2: " << this->primeTwo;
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

bool PrivateKeyRSA::LoadFromASNEncoded(List<unsigned char>& input, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("PrivateKeyRSA::LoadFromASNEncoded");
  AbstractSyntaxNotationOneSubsetDecoder outerDecoder, innerDecoder;
  if (!outerDecoder.Decode(input, this->sourceJSONOuter, nullptr, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to asn-decode private key outer data. ";
    }
    return false;
  }
  ListZeroAfterUse<unsigned char> innerData;
  if (!this->sourceJSONOuter.HasCompositeKeyOfType("[2]", innerData.data, commentsOnFailure)) {
    return false;
  }
  if (!innerDecoder.Decode(innerData.data, this->sourceJSONInner, nullptr, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to asn-decode private key inner data. ";
    }
    return false;
  }
  JSData exponent1, exponent2, coefficient;
  if (!this->sourceJSONInner.HasCompositeKeyOfType(
    "[1]", this->modulus, commentsOnFailure
  )) {
    return false;
  }
  if (!this->sourceJSONInner.HasCompositeKeyOfType(
    "[2]", this->exponent, commentsOnFailure
  )) {
    return false;
  }
  if (!this->sourceJSONInner.HasCompositeKeyOfType(
    "[3]", this->privateExponent, commentsOnFailure
  )) {
    return false;
  }
  if (!this->sourceJSONInner.HasCompositeKeyOfType(
    "[4]", this->primeOne, commentsOnFailure
  )) {
    return false;
  }
  if (!this->sourceJSONInner.HasCompositeKeyOfType(
    "[5]", this->primeTwo, commentsOnFailure
  )) {
    return false;
  }
  if (!this->sourceJSONInner.HasCompositeKey("[6]", &exponent1, commentsOnFailure)) {
    return false;
  }
  if (!this->sourceJSONInner.HasCompositeKey("[7]", &exponent2, commentsOnFailure)) {
    return false;
  }
  if (!this->sourceJSONInner.HasCompositeKey("[8]", &coefficient, commentsOnFailure)) {
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
  if (!StringRoutines::StringBeginsWith(certificateContentStripped, beginCertificate, &certificateContentStripped)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad certificate start. ";
    }
    return false;
  }
  if (!StringRoutines::StringEndsWith(certificateContentStripped, endCertificate, &certificateContentStripped)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad certificate end. ";
    }
    return false;
  }
  certificateContentStripped = StringRoutines::StringTrimWhiteSpace(certificateContentStripped);
  if (!Crypto::ConvertBase64ToBitStream(certificateContentStripped, this->sourceBinary, commentsOnFailure, nullptr)) {
    return false;
  }
  return this->LoadFromASNEncoded(this->sourceBinary, commentsOnFailure);
}

bool X509Certificate::LoadFromPEMFile(const std::string& input, std::stringstream* commentsOnFailure) {
  std::string certificateContent;
  // No access to sensitive folders here, so this cannot be used for the server's certificate.
  // For server's certificate, use TransportLayerSecurity::LoadPEMCertificate.
  if (!FileOperations::LoadFileToStringVirtual(input, certificateContent, false, false, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load key file. ";
    }
    return false;
  }
  return this->LoadFromPEM(certificateContent, commentsOnFailure);
}

void TBSCertificateInfo::WriteBytesASN1(List<unsigned char>& output) {
  AbstractSyntaxNotationOneSubsetDecoder::WriterSequence writeTBSCertificateContent(200, output);
  this->countryName           .WriteBytesASNObject(output);
  this->stateOrProvinceName   .WriteBytesASNObject(output);
  this->localityName          .WriteBytesASNObject(output);
  this->organizationName      .WriteBytesASNObject(output);
  this->organizationalUnitName.WriteBytesASNObject(output);
  this->commonName            .WriteBytesASNObject(output);
  this->emailAddress          .WriteBytesASNObject(output);
}

bool TBSCertificateInfo::LoadFromJSON(const JSData& input, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("TBSCertificateInfo::LoadFromJSON");
  MapList<std::string, ASNObject, MathRoutines::HashString> fields;
  if (!ASNObject::LoadFieldsFromJSArray(input, fields, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read certificate fields. Certificate fields decoded: "
      << input.ToString(false, true, true, true);
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
  if (!theDecoder.Decode(input, this->sourceJSON, &this->sourceInterpretation, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to asn-decode certificate input. ";
    }
    return false;
  }
  if (!this->sourceJSON.HasCompositeKeyOfType(
    "[0][1]", this->serialNumber, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read serial number. JSON decoded: "
      << this->sourceJSON.ToString(false, true, true, true);
    }
    return false;
  }
  if (!this->sourceJSON.HasCompositeKeyOfType("[0][2][0].objectIdentifierBytes", this->signatureAlgorithmId, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to get signature algorithm id. ";
    }
    return false;
  }
  JSData certificateFieldsJSON;
  if (!this->sourceJSON.HasCompositeKey("[0][3]", &certificateFieldsJSON, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to get sequence of certificate fields. ";
    }
    return false;
  }
  if (!this->information.LoadFromJSON(certificateFieldsJSON, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read certificate fields. JSON decoded: "
      << this->sourceJSON.ToString(false, true, true, true);
    }
    return false;
  }
  if (!this->sourceJSON.HasCompositeKeyOfType(
    "[0][6][1].bitStringDecoded[1]", this->theRSA.theModuluS, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read RSA modulus. JSON decoded: "
      << this->sourceJSON.ToString(false, true, true, true);
    }
    return false;
  }
  if (!this->sourceJSON.HasCompositeKeyOfType("[0][6][1].bitStringDecoded[0]", this->theRSA.theExponenT, commentsOnFailure)) {
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

