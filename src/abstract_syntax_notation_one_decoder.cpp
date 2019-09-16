//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".

#include "abstract_syntax_notation_one_decoder.h"
#include "crypto.h"
#include "math_extra_algebraic_numbers.h"
#include "serialization_basic.h"

extern logger logWorker;

extern ProjectInformationInstance ProjectInfoAbstractSyntaxNotationOneDecoderImplementation;

ProjectInformationInstance ProjectInfoAbstractSyntaxNotationOneDecoderImplementation(__FILE__, "Abstract syntax notation one (ASN-1) implementation");

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

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeConstructed(
  unsigned char startByte, int desiredLengthInBytes, JSData& output, JSData* interpretation
) {
  (void) startByte;
  output.reset(JSData::token::tokenObject);
  JSData objectContent;
  if (!this->DecodeSequenceContent(desiredLengthInBytes, objectContent, interpretation)) {
    return false;
  }
  return true;
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

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeSetContent(
  int desiredLengthInBytes, JSData& output, JSData* interpretation
) {
  output.reset(JSData::token::tokenObject);
  if (interpretation != nullptr) {
    interpretation->reset(JSData::token::tokenObject);
  }
  JSData theSet, theSetInterpretation;
  bool success = this->DecodeSequenceContent(desiredLengthInBytes, theSet, &theSetInterpretation);
  if (interpretation != nullptr) {
    (*interpretation)["set"] = theSetInterpretation;
  }
  if (!success) {
    return false;
  }
  output["set"] = theSet;
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeSequenceContent(
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
    (*interpretation)["body"] = Crypto::ConvertStringToHex(theSource, 0, false);
  }
  return true;
}

std::string AbstractSyntaxNotationOneSubsetDecoder::GetType(unsigned char startByte) {
  switch (startByte) {
  case AbstractSyntaxNotationOneSubsetDecoder::tags::sequence:
    return "sequence";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::integer:
    return "integer";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::zero:
    return "zero";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::bitString:
    return "bitString";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::octetString:
    return "octetString";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::tokenNull:
    return "null";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::objectIdentifier:
    return "objectID";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::utf8String:
    return "utf8 string";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::set:
    return "set";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::printableString:
    return "printable string";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::IA5String:
    return "IA5String";
  case AbstractSyntaxNotationOneSubsetDecoder::tags::UTCTime:
    return "UTCTime";
  default:
    return "unknown";
  }
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
    startByte -= 32;
    if (interpretation != nullptr) {
      (*interpretation)["constructed"] = true;
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
      (*interpretation)["constructed"] = true;
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
  case tags::zero:
    return this->DecodeConstructed(startByte, currentLength, output, interpretation);
  case tags::integer:
    return this->DecodeIntegerContent(currentLength, output, interpretation);
  case tags::octetString:
    return this->DecodeOctetString(currentLength, output, interpretation);
  case tags::objectIdentifier:
    return this->DecodeObjectIdentifier(currentLength, output, interpretation);
  case tags::tokenNull:
    return this->DecodeNull(currentLength, output, interpretation);
  case tags::sequence:
    return this->DecodeSequenceContent(currentLength, output, interpretation);
  case tags::set:
    return this->DecodeSetContent(currentLength, output, interpretation);
  case tags::printableString:
    return this->DecodePrintableString(currentLength, output, interpretation);
  case tags::utf8String:
    return this->DecodeUTF8String(currentLength, output, interpretation);
  case tags::IA5String:
    return this->DecodeIA5String(currentLength, output, interpretation);
  case tags::UTCTime:
    return this->DecodeUTCString(currentLength, output, interpretation);
  case tags::bitString:
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

int AbstractSyntaxNotationOneSubsetDecoder::WriterSequenceFixedLength::GetReservedBytesForLength(int length) {
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

AbstractSyntaxNotationOneSubsetDecoder::WriterSequenceFixedLength::WriterSequenceFixedLength(
  List<unsigned char>& output,
  int expectedTotalElementByteLength,
  bool isConstructed
) {
  if (expectedTotalElementByteLength < 0) {
    expectedTotalElementByteLength = 0;
  }
  this->outputPointer = &output;
  this->offset = output.size;
  unsigned char theTag = AbstractSyntaxNotationOneSubsetDecoder::tags::sequence;
  if (isConstructed) {
    theTag += 32;
  }
  this->outputPointer->AddOnTop(theTag);
  this->totalByteLength = expectedTotalElementByteLength;
  this->reservedBytesForLength = this->GetReservedBytesForLength(expectedTotalElementByteLength);
  for (int i = 0; i < this->reservedBytesForLength; i ++) {
    this->outputPointer->AddOnTop(0);
  }
}

AbstractSyntaxNotationOneSubsetDecoder::WriterSequenceFixedLength::~WriterSequenceFixedLength() {
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
  if (this->totalByteLength < 128) {
    unsigned char length = static_cast<unsigned char>(this->totalByteLength);
    (*this->outputPointer)[this->offset + 1] = length;
  } else {
    unsigned char lengthPlus128 = 128 + static_cast<unsigned char>(this->reservedBytesForLength);
    (*this->outputPointer)[this->offset + 1] = lengthPlus128;
    int offsetTemporary = this->offset + 2;
    Serialization::WriteNByteLength(
      this->reservedBytesForLength,
      static_cast<unsigned int>(this->totalByteLength),
      *this->outputPointer,
      offsetTemporary
    );
  }
}

void X509Certificate::WriteBytesASN1(List<unsigned char>& output) {
  AbstractSyntaxNotationOneSubsetDecoder::WriterSequenceFixedLength writeLength(output, 2000);
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
  JSData actualContent;
  if (!this->sourceJSONOuter.HasCompositeKeyOfType("[2]", &actualContent, JSData::token::tokenString, commentsOnFailure)) {
    return false;
  }
  ListZeroAfterUse<unsigned char> innerData;
  innerData.data = actualContent.theString;
  if (!innerDecoder.Decode(innerData.data, this->sourceJSONInner, nullptr, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to asn-decode private key inner data. ";
    }
    return false;
  }
  JSData exponent, modulus, privateExponentJS, prime1, prime2, exponent1, exponent2, coefficient;
  if (!this->sourceJSONInner.HasCompositeKeyOfType(
    "[1]", &modulus, JSData::token::tokenLargeInteger, commentsOnFailure
  )) {
    return false;
  }
  this->modulus = modulus.theInteger.GetElement().value;
  if (!this->sourceJSONInner.HasCompositeKeyOfType(
    "[2]", &exponent, JSData::token::tokenLargeInteger, commentsOnFailure
  )) {
    return false;
  }
  this->exponent = exponent.theInteger.GetElement().value;
  if (!this->sourceJSONInner.HasCompositeKey("[3]", &privateExponentJS, commentsOnFailure)) {
    return false;
  }
  this->privateExponent = privateExponentJS.theInteger.GetElement().value;
  if (!this->sourceJSONInner.HasCompositeKeyOfType(
    "[4]", &prime1, JSData::token::tokenLargeInteger, commentsOnFailure
  )) {
    return false;
  }
  this->primeOne = prime1.theInteger.GetElement().value;
  if (!this->sourceJSONInner.HasCompositeKeyOfType(
    "[5]", &prime2, JSData::token::tokenLargeInteger, commentsOnFailure
  )) {
    return false;
  }
  this->primeTwo = prime2.theInteger.GetElement().value;
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
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "decoded json: " << this->sourceJSON.ToString(false, true, true, true);
  }
  JSData rsaPublicKey;
  JSData rsaModulus;
  if (!this->sourceJSON.HasCompositeKey("[0][6][1].bitStringDecoded[1]", &rsaModulus, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read rsa modulus. JSON decoded: "
      << this->sourceJSON.ToString(false, true, true, true);
    }
    return false;
  }
  if (!this->sourceJSON.HasCompositeKey("[0][6][1].bitStringDecoded[0]", &rsaPublicKey, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read public key. ";
    }
    return false;
  }
  if (rsaModulus.theType != JSData::token::tokenLargeInteger) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "RSA modulus does not appear to be a large integer: " << rsaModulus.ToString(false);
    }
    return false;
  }
  if (rsaPublicKey.theType != JSData::token::tokenLargeInteger) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "RSA public key does not appear to be a large integer: " << rsaPublicKey.ToString(false);
    }
    return false;
  }
  this->theRSA.theModuluS = rsaModulus.theInteger.GetElement().value;
  this->theRSA.theExponenT = rsaPublicKey.theInteger.GetElement().value;
  return true;
}

