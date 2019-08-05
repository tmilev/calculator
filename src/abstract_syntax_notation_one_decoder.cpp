//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".

#include "abstract_syntax_notation_one_decoder.h"
#include "vpfHeader5Crypto.h"

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
  output.reset(JSData::token::tokenObject);
  JSData objectContent;
  if (!this->DecodeSequenceContent(desiredLengthInBytes, objectContent, interpretation)) {
    return false;
  }
  return true;
}

AbstractSyntaxNotationOneSubsetDecoder::AbstractSyntaxNotationOneSubsetDecoder() {
  this->flagLogByteInterpretation = false;
  this->recursionDepthGuard = 0;
  this->maxRecursionDepth = 10000;
}

void AbstractSyntaxNotationOneSubsetDecoder::initialize() {
  this->recursionDepthGuard = 0;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeLengthIncrementDataPointer(
  int& outputLengthNegativeOneForVariable, JSData* interpretation
) {
  outputLengthNegativeOneForVariable = 0;
  int startDataPointer = this->dataPointer;
  unsigned char currentByte = this->rawData[this->dataPointer];
  if (currentByte < 128) { // 128 = 0x80
    outputLengthNegativeOneForVariable = currentByte;
    this->dataPointer ++;
    return true;
  }
  if (currentByte == 128) { // 128 = 0x80 signals varible-length encoding
    outputLengthNegativeOneForVariable = - 1;
    this->dataPointer ++;
    return true;
  }
  int numberOfLengthBytes = currentByte - 128;
  if (interpretation != 0) {
    (*interpretation)["numberOfLengthBytes"] = numberOfLengthBytes;
  }
  LargeInt length = 0;
  this->dataPointer ++;
  for (int i = 0; i < numberOfLengthBytes; i ++) {
    length *= 256;
    if (this->PointerIsBad(interpretation)) {
      return false;
    }
    unsigned char nextByte = this->rawData[this->dataPointer];
    length += (int) nextByte;
    this->dataPointer ++;
  }
  if (!length.IsIntegerFittingInInt(&outputLengthNegativeOneForVariable)) {
    if (interpretation != 0) {
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
  if (this->dataPointer >= (signed) this->rawData.size() || this->dataPointer < 0) {
    if (interpretation != 0) {
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
  if (interpretation != 0) {
    interpretation->reset(JSData::token::tokenObject);
  }
  JSData theSet, theSetInterpretation;
  bool success = this->DecodeSequenceContent(desiredLengthInBytes, theSet, &theSetInterpretation);
  if (interpretation != 0) {
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
  if (interpretation != 0) {
    (*interpretation)["value"].reset(JSData::token::tokenArray);
  }
  int lastIndexPlusOne = this->dataPointer + desiredLengthInBytes;
  JSData nextElement, nextElementInterpretationContainer;
  JSData* nextElementInterpretation = 0;
  if (interpretation != 0) {
    nextElementInterpretation = &nextElementInterpretationContainer;
  }
  int numberOfDecoded = 0;
  while (this->dataPointer < lastIndexPlusOne) {
    int lastPointer = this->dataPointer;
    bool isGood = this->DecodeCurrent(nextElement, nextElementInterpretation);
    if (interpretation != 0) {
      (*interpretation)["value"].theList.AddOnTop(*nextElementInterpretation);
    }
    if (!isGood){
      if (interpretation != 0) {
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
  JSData* timeInterpretation = interpretation == 0 ? 0 : &timeInterpretationContainer;
  bool success = this->DecodeOctetString(desiredLengthInBytes, theTime, timeInterpretation);
  if (interpretation != 0) {
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
  JSData* theBitStringInterpretation = interpretation == 0 ? 0 : & theBitStringInterpretationContainer;
  bool success = this->DecodeOctetString(desiredLengthInBytes, theBitString, theBitStringInterpretation);
  if (interpretation != 0) {
    (*interpretation)["bitString"] = *theBitStringInterpretation;
  }
  if (!success) {
    return false;
  }
  output["bitString"] = theBitString;
  AbstractSyntaxNotationOneSubsetDecoder subDecoder;
  subDecoder.flagLogByteInterpretation = this->flagLogByteInterpretation;
  subDecoder.rawData = theBitString.theString;
  if (subDecoder.rawData.size() > 0) {
    subDecoder.rawData = subDecoder.rawData.substr(1);
  }
  subDecoder.recursionDepthGuard = this->recursionDepthGuard;
  if (subDecoder.Decode(0)) {
    output["bitStringDecoded"] = subDecoder.decodedData;
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
  output.theString.resize(desiredLengthInBytes);
  for (int i = 0; i < desiredLengthInBytes; i ++) {
    output.theString[i] = this->rawData[this->dataPointer + i];
  }
  this->dataPointer += desiredLengthInBytes;
  if (interpretation != 0) {
    (*interpretation)["value"] = output.theString;
  }
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeNull(
  int desiredLengthInBytes, JSData& output, JSData* interpretation
) {
  if (desiredLengthInBytes != 0) {
    if (interpretation != 0) {
      (*interpretation)["error"] = "Length of null object is not zero. ";
    }
    return false;
  }
  output.reset(JSData::token::tokenNull);
  if (interpretation != 0) {
    (*interpretation)["value"].reset(JSData::token::tokenNull);
  }
  return true;
}

LargeInt AbstractSyntaxNotationOneSubsetDecoder::VariableLengthQuantityDecode(
  const std::string& input, int& inputOutputDataPointer
) {
  LargeInt result = 0;
  for (; inputOutputDataPointer < (signed) input.size() ; inputOutputDataPointer ++) {
    unsigned char currentByte = input[inputOutputDataPointer];
    result *= 128;
    if (currentByte < 128) {
      result += (int) currentByte;
      inputOutputDataPointer ++;
      break;
    } else {
      currentByte -= 128;
      result += (int) currentByte;
    }
  }
  return result;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeObjectIdentifier(int desiredLengthInBytes, JSData& output
, JSData *interpretation) {
  if (this->PointerIsBad(interpretation)) {
    return false;
  }
  std::stringstream resultStream;
  unsigned char firstByte = this->rawData[this->dataPointer];
  int currentPointer = this->dataPointer + 1;
  this->dataPointer += desiredLengthInBytes;
  if (this->PointerIsBad(interpretation)) {
    if (interpretation != 0) {
      std::stringstream errorStream;
      errorStream << "Object identifier length overshoots the total raw data length. ";
      (*interpretation)["error"] = errorStream.str();
    }
    return false;
  }
  // first two entries are encoded in the first byte:
  unsigned char firstEntry = firstByte / 40;
  unsigned char secondEntry = firstByte % 40;
  resultStream << (int) firstEntry << "." << (int) secondEntry;
  for (; currentPointer < this->dataPointer; ) {
    LargeInt nextInt = this->VariableLengthQuantityDecode(this->rawData, currentPointer);
    resultStream << "." << nextInt.ToString();
  }
  if (interpretation != 0) {
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
    unsigned char unsignedContribution = (unsigned char) this->rawData[this->dataPointer];
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
  if (interpretation != 0) {
    interpretation->reset(JSData::token::tokenObject);
    (*interpretation)["value"] = reader;
    std::string theSource = this->rawData.substr(this->dataPointer - desiredLengthInBytes, desiredLengthInBytes);
    (*interpretation)["hex"] = Crypto::ConvertStringToHex(theSource, 0, false);
  }
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeCurrent(JSData& output, JSData* interpretation) {
  MacroRegisterFunctionWithName("AbstractSyntaxNotationOneSubsetDecoder::DecodeCurrent");
  RecursionDepthCounter recursionGuard(&this->recursionDepthGuard);
  if (this->recursionDepthGuard > this->maxRecursionDepth) {
    if (interpretation != 0) {
      std::stringstream errorStream;
      errorStream << "Max depth of " << this->maxRecursionDepth << " exceeded. ";
      (*interpretation)["error"] = errorStream.str();
    }
    return false;
  }
  output.reset(JSData::token::tokenUndefined);
  if (interpretation != 0) {
    interpretation->reset(JSData::token::tokenUndefined);
  }
  if (this->PointerIsBad(interpretation)) {
    return false;
  }
  unsigned char startByte = this->rawData[this->dataPointer];
  unsigned char startByteOriginal = startByte;
  int dataPointerAtStart = this->dataPointer;
  bool isConstructed = this->isCostructedByte(startByte);
  if (isConstructed) {
    startByte -= 32;
    if (interpretation != 0) {
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
  if (interpretation != 0) {
    (*interpretation)["offset"] = this->dataPointer;
    (*interpretation)["startByteOriginal"] = startByteOriginal;
    (*interpretation)["startByte"] = startByte;
  }
  this->dataPointer ++;
  int currentLength = 0;
  if (!this->DecodeLengthIncrementDataPointer(currentLength, interpretation)) {
    return false;
  }
  if (currentLength < 0) {
    if (interpretation != 0) {
      std::stringstream errorStream;
      errorStream << "Failed to decode variable length. ";
      (*interpretation)["error"] = errorStream.str();
    }
    return false;
  }
  if (interpretation != 0) {
    (*interpretation)["length"] = currentLength;
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
    if (interpretation != 0) {
      errorStream << "Unknown object tag: " << (int) startByte << ", byte: "
      << (int) startByteOriginal << " at position: " << dataPointerAtStart << ". Length: " << currentLength << ". ";
      (*interpretation)["error"] = errorStream.str();
    }
    return false;
  }
  return false;
}

bool AbstractSyntaxNotationOneSubsetDecoder::Decode(std::stringstream* commentsOnError) {
  MacroRegisterFunctionWithName("AbstractSyntaxNotationOneSubsetDecoder::Decode");
  unsigned int maxAllowedSize = 1000000000;
  if (this->rawData.size() >= maxAllowedSize) {
    if (commentsOnError != 0) {
      *commentsOnError << "Input size: " << this->rawData.size() << " too large, max allowed: " << maxAllowedSize << ". ";
    }
    return false;
  }
  this->initialize();
  this->dataPointer = 0;
  this->decodedData.reset();
  JSData* interpretation = 0;
  if (this->flagLogByteInterpretation) {
    interpretation = &this->dataInterpretation;
    this->dataInterpretation.reset();
  }
  this->DecodeCurrent(this->decodedData, interpretation);
  if (this->dataPointer < (signed) this->rawData.size()) {
    if (commentsOnError != 0) {
      *commentsOnError << "Decoded " << this->dataPointer << " bytes but the input had: " << this->rawData.size() << ". ";
    }
    return false;
  }
  return true;
}

std::string AbstractSyntaxNotationOneSubsetDecoder::ToStringDebug() const {
  std::stringstream out;
  out << "Decoded so far:<br>\n"
  << this->decodedData.ToString(false, false, true, true);
  if (this->flagLogByteInterpretation) {
    out << "<br>Interpretation status.<br>" << this->dataInterpretation.ToString(false, true, true, true);
  }
  out << "<br>Data: ";
  out << MathRoutines::StringShortenInsertDots(Crypto::ConvertStringToHex(this->rawData, 70, true), 4000);
  return out.str();
}
