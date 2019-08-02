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

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeCurrentConstructed(std::stringstream* commentsOnError) {
  unsigned char startByte = this->rawData[this->dataPointer];
  if (!this->isCostructedByte(startByte)) {
    crash << "Byte " << (int) startByte << " must be constructed. " << crash;
  }
  unsigned char nonConstructedPart = startByte - 32;
  AbstractSyntaxNotationOneSubsetDecoder::typeDecoder handler = this->decodersByByteValue[nonConstructedPart];
  if (handler == 0) {
    if (commentsOnError != 0) {
      *commentsOnError << "No handler for byte: " << (int) startByte << ". ";
    }
    return false;
  }
  this->dataPointer ++;
  return handler(*this, commentsOnError);
}

AbstractSyntaxNotationOneSubsetDecoder::AbstractSyntaxNotationOneSubsetDecoder() {
  this->flagLogByteInterpretation = false;
}

void AbstractSyntaxNotationOneSubsetDecoder::initialize() {
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeLengthIncrementDataPointer(
  int& outputLengthNegativeOneForVariable, std::stringstream* commentsOnError
) {
  outputLengthNegativeOneForVariable = 0;
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
  LargeInt length = 0;
  this->dataPointer ++;
  for (int i = 0; i < numberOfLengthBytes; i ++) {
    length *= 256;
    if (this->PointerIsBad(commentsOnError)) {
      return false;
    }
    length += this->rawData[this->dataPointer];
    this->dataPointer ++;
  }
  if (!length.IsIntegerFittingInInt(&outputLengthNegativeOneForVariable)) {
    if (commentsOnError != 0) {
      *commentsOnError << "Variable length is too large. ";
    }
    return false;
  }
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::PointerIsBad(std::stringstream* commentsOnError) {
  if (this->dataPointer >= (signed) this->rawData.size() || this->dataPointer < 0) {
    if (commentsOnError != 0) {
      *commentsOnError << "Unexpected overflow error: data pointer is negative. ";
    }
    return true;
  }
  return false;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeSequenceContent(
  std::stringstream* commentsOnError, int desiredLengthInBytes, JSData& output
) {
  output.reset(JSData::token::tokenArray);
  int lastIndexPlusOne = this->dataPointer + desiredLengthInBytes;
  JSData nextElement;
  int numberOfDecoded = 0;
  while (this->dataPointer < lastIndexPlusOne) {
    int lastPointer = this->dataPointer;
    if (!this->DecodeCurrent(commentsOnError, nextElement)) {
      if (commentsOnError != 0) {
        *commentsOnError << "Failed to decode sequence element of index: " << numberOfDecoded << ". ";
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

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeOctetString(
  std::stringstream* commentsOnError, int desiredLengthInBytes, JSData& output
) {
  if (commentsOnError != 0) {
    *commentsOnError << "Not implemented yet.";
  }
  return false;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeObjectIdentifier(
  std::stringstream* commentsOnError, int desiredLengthInBytes, JSData& output
) {
  return this->DecodeSequenceContent(commentsOnError, desiredLengthInBytes, output);
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeIntegerContent(
  std::stringstream* commentsOnError, int desiredLengthInBytes, JSData& output
) {
  LargeInt reader = 0;
  int currentContribution;
  for (int i = 0; i < desiredLengthInBytes; i ++) {
    currentContribution = this->rawData[this->dataPointer];
    if (i == 0) {
      if (currentContribution >= 128) {
        currentContribution = 256 - currentContribution;
      }
    }
    reader *= 256;
    reader += currentContribution;
    this->dataPointer ++;
  }
  output = reader;
  return true;
}

bool AbstractSyntaxNotationOneSubsetDecoder::DecodeCurrent(std::stringstream* commentsOnError, JSData& output) {
  if (this->PointerIsBad(commentsOnError)) {
    return false;
  }
  unsigned char startByte = this->rawData[this->dataPointer];
  int dataPointerAtStart = this->dataPointer;
  bool isConstructed = this->isCostructedByte(startByte);
  if (isConstructed) {
    startByte -= 32;
  }
  this->dataPointer ++;
  int currentLength = 0;
  if (!this->DecodeLengthIncrementDataPointer(currentLength, commentsOnError)) {
    return false;
  }
  if (currentLength < 0) {
    if (commentsOnError != 0) {
      *commentsOnError << "Failed to decode variable length. ";
    }
    return false;
  }
  switch (startByte) {
  case tags::integer:
    return this->DecodeIntegerContent(commentsOnError, currentLength, output);
  case tags::octet_string:
    return this->DecodeOctetString(commentsOnError, currentLength, output);
  case tags::object_identifier:
    return this->DecodeObjectIdentifier(commentsOnError, currentLength, output);
  case tags::sequence:
    return this->DecodeSequenceContent(commentsOnError, currentLength, output);
  default:
    if (commentsOnError != 0) {
      *commentsOnError << "Unknown object tag: " << (int) startByte << " at position: " << dataPointerAtStart << ". ";
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
  this->DecodeCurrent(commentsOnError, this->decodedData);
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
  << this->decodedData.ToString(false, false, true);
  out << "<br>Data: ";
  out << MathRoutines::StringShortenInsertDots(Crypto::ConvertStringToHex(this->rawData, 70, true), 4000);
  return out.str();
}
