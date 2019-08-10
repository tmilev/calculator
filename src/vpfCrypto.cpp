#include "vpfHeader5Crypto.h"
#include "vpfImplementationHeader2Math0_General.h"
#include "vpfHeader1General1_ListReferences.h"
#include "vpfHeader1General7FileOperations_Encodings.h"
#include "vpfJson.h"
#include "vpfHeader2Math2_AlgebraicNumbers.h"
ProjectInformationInstance projectInfoCryptoFile1(__FILE__, "SHA- 1 and base64 implementation.");

unsigned char Crypto::GetCharFrom6bit(uint32_t input, bool useBase64URL) {
  switch (input) {
    case 0: return  'A';
    case 1: return  'B';
    case 2: return  'C';
    case 3: return  'D';
    case 4: return  'E';
    case 5: return  'F';
    case 6: return  'G';
    case 7: return  'H';
    case 8: return  'I';
    case 9: return  'J';
    case 10: return 'K';
    case 11: return 'L';
    case 12: return 'M';
    case 13: return 'N';
    case 14: return 'O';
    case 15: return 'P';
    case 16: return 'Q';
    case 17: return 'R';
    case 18: return 'S';
    case 19: return 'T';
    case 20: return 'U';
    case 21: return 'V';
    case 22: return 'W';
    case 23: return 'X';
    case 24: return 'Y';
    case 25: return 'Z';
    case 26: return 'a';
    case 27: return 'b';
    case 28: return 'c';
    case 29: return 'd';
    case 30: return 'e';
    case 31: return 'f';
    case 32: return 'g';
    case 33: return 'h';
    case 34: return 'i';
    case 35: return 'j';
    case 36: return 'k';
    case 37: return 'l';
    case 38: return 'm';
    case 39: return 'n';
    case 40: return 'o';
    case 41: return 'p';
    case 42: return 'q';
    case 43: return 'r';
    case 44: return 's';
    case 45: return 't';
    case 46: return 'u';
    case 47: return 'v';
    case 48: return 'w';
    case 49: return 'x';
    case 50: return 'y';
    case 51: return 'z';
    case 52: return '0';
    case 53: return '1';
    case 54: return '2';
    case 55: return '3';
    case 56: return '4';
    case 57: return '5';
    case 58: return '6';
    case 59: return '7';
    case 60: return '8';
    case 61: return '9';
    case 62:
      if (useBase64URL) {
        return '-'; //RFC 4648, base64url.
      } else {
        return '+'; //RFC 4648.
      }
    case 63:
      if (useBase64URL) {
        return '_'; //RFC 4648.
      } else {
        return '/'; // RFC 4648, base64url.
      }
    default: crash << "Requesting character from a purported 6 bit integer, which in fact has more significant bits. " << crash;
      break;
  }
  return - 1;
}

bool Crypto::GetCharFromBase58(uint32_t input, char& output) {
  switch (input) {
    case 0:  output = '1'; return true;
    case 1:  output = '2'; return true;
    case 2:  output = '3'; return true;
    case 3:  output = '4'; return true;
    case 4:  output = '5'; return true;
    case 5:  output = '6'; return true;
    case 6:  output = '7'; return true;
    case 7:  output = '8'; return true;
    case 8:  output = '9'; return true;
    case 9:  output = 'A'; return true;
    case 10: output = 'B'; return true;
    case 11: output = 'C'; return true;
    case 12: output = 'D'; return true;
    case 13: output = 'E'; return true;
    case 14: output = 'F'; return true;
    case 15: output = 'G'; return true;
    case 16: output = 'H'; return true;
    case 17: output = 'J'; return true;
    case 18: output = 'K'; return true;
    case 19: output = 'L'; return true;
    case 20: output = 'M'; return true;
    case 21: output = 'N'; return true;
    case 22: output = 'P'; return true;
    case 23: output = 'Q'; return true;
    case 24: output = 'R'; return true;
    case 25: output = 'S'; return true;
    case 26: output = 'T'; return true;
    case 27: output = 'U'; return true;
    case 28: output = 'V'; return true;
    case 29: output = 'W'; return true;
    case 30: output = 'X'; return true;
    case 31: output = 'Y'; return true;
    case 32: output = 'Z'; return true;
    case 33: output = 'a'; return true;
    case 34: output = 'b'; return true;
    case 35: output = 'c'; return true;
    case 36: output = 'd'; return true;
    case 37: output = 'e'; return true;
    case 38: output = 'f'; return true;
    case 39: output = 'g'; return true;
    case 40: output = 'h'; return true;
    case 41: output = 'i'; return true;
    case 42: output = 'j'; return true;
    case 43: output = 'k'; return true;
    case 44: output = 'm'; return true;
    case 45: output = 'n'; return true;
    case 46: output = 'o'; return true;
    case 47: output = 'p'; return true;
    case 48: output = 'q'; return true;
    case 49: output = 'r'; return true;
    case 50: output = 's'; return true;
    case 51: output = 't'; return true;
    case 52: output = 'u'; return true;
    case 53: output = 'v'; return true;
    case 54: output = 'w'; return true;
    case 55: output = 'x'; return true;
    case 56: output = 'y'; return true;
    case 57: output = 'z'; return true;
    default: return false;
  }
  return false;
}


bool Crypto::GetBase58FromChar(unsigned char input, uint32_t& output) {
  switch (input) {
    case '1': output = 0;  return true;
    case '2': output = 1;  return true;
    case '3': output = 2;  return true;
    case '4': output = 3;  return true;
    case '5': output = 4;  return true;
    case '6': output = 5;  return true;
    case '7': output = 6;  return true;
    case '8': output = 7;  return true;
    case '9': output = 8;  return true;
    case 'A': output = 9;  return true;
    case 'B': output = 10; return true;
    case 'C': output = 11; return true;
    case 'D': output = 12; return true;
    case 'E': output = 13; return true;
    case 'F': output = 14; return true;
    case 'G': output = 15; return true;
    case 'H': output = 16; return true;
    case 'J': output = 17; return true;
    case 'K': output = 18; return true;
    case 'L': output = 19; return true;
    case 'M': output = 20; return true;
    case 'N': output = 21; return true;
    case 'P': output = 22; return true;
    case 'Q': output = 23; return true;
    case 'R': output = 24; return true;
    case 'S': output = 25; return true;
    case 'T': output = 26; return true;
    case 'U': output = 27; return true;
    case 'V': output = 28; return true;
    case 'W': output = 29; return true;
    case 'X': output = 30; return true;
    case 'Y': output = 31; return true;
    case 'Z': output = 32; return true;
    case 'a': output = 33; return true;
    case 'b': output = 34; return true;
    case 'c': output = 35; return true;
    case 'd': output = 36; return true;
    case 'e': output = 37; return true;
    case 'f': output = 38; return true;
    case 'g': output = 39; return true;
    case 'h': output = 40; return true;
    case 'i': output = 41; return true;
    case 'j': output = 42; return true;
    case 'k': output = 43; return true;
    case 'm': output = 44; return true;
    case 'n': output = 45; return true;
    case 'o': output = 46; return true;
    case 'p': output = 47; return true;
    case 'q': output = 48; return true;
    case 'r': output = 49; return true;
    case 's': output = 50; return true;
    case 't': output = 51; return true;
    case 'u': output = 52; return true;
    case 'v': output = 53; return true;
    case 'w': output = 54; return true;
    case 'x': output = 55; return true;
    case 'y': output = 56; return true;
    case 'z': output = 57; return true;
    default: return false;
  }
  return false;
}


bool Crypto::Get6bitFromChar(unsigned char input, uint32_t& output) {
  switch (input) {
    case 'A': output = 0;  return true;
    case 'B': output = 1;  return true;
    case 'C': output = 2;  return true;
    case 'D': output = 3;  return true;
    case 'E': output = 4;  return true;
    case 'F': output = 5;  return true;
    case 'G': output = 6;  return true;
    case 'H': output = 7;  return true;
    case 'I': output = 8;  return true;
    case 'J': output = 9;  return true;
    case 'K': output = 10; return true;
    case 'L': output = 11; return true;
    case 'M': output = 12; return true;
    case 'N': output = 13; return true;
    case 'O': output = 14; return true;
    case 'P': output = 15; return true;
    case 'Q': output = 16; return true;
    case 'R': output = 17; return true;
    case 'S': output = 18; return true;
    case 'T': output = 19; return true;
    case 'U': output = 20; return true;
    case 'V': output = 21; return true;
    case 'W': output = 22; return true;
    case 'X': output = 23; return true;
    case 'Y': output = 24; return true;
    case 'Z': output = 25; return true;
    case 'a': output = 26; return true;
    case 'b': output = 27; return true;
    case 'c': output = 28; return true;
    case 'd': output = 29; return true;
    case 'e': output = 30; return true;
    case 'f': output = 31; return true;
    case 'g': output = 32; return true;
    case 'h': output = 33; return true;
    case 'i': output = 34; return true;
    case 'j': output = 35; return true;
    case 'k': output = 36; return true;
    case 'l': output = 37; return true;
    case 'm': output = 38; return true;
    case 'n': output = 39; return true;
    case 'o': output = 40; return true;
    case 'p': output = 41; return true;
    case 'q': output = 42; return true;
    case 'r': output = 43; return true;
    case 's': output = 44; return true;
    case 't': output = 45; return true;
    case 'u': output = 46; return true;
    case 'v': output = 47; return true;
    case 'w': output = 48; return true;
    case 'x': output = 49; return true;
    case 'y': output = 50; return true;
    case 'z': output = 51; return true;
    case '0': output = 52; return true;
    case '1': output = 53; return true;
    case '2': output = 54; return true;
    case '3': output = 55; return true;
    case '4': output = 56; return true;
    case '5': output = 57; return true;
    case '6': output = 58; return true;
    case '7': output = 59; return true;
    case '8': output = 60; return true;
    case '9': output = 61; return true;
    case '-': output = 62; return true;//RFC 4648 base64url.
    case '_': output = 63; return true;//RFC 4648 base64url.
    case '+': output = 62; return true;//RFC 4648: "standard" base64.
    case '/': output = 63; return true;//RFC 4648: "standard" base64.
    //Note: there is no collision between the original base64 and RFC 7515,
    //both can be supported for input.
    case '=': return false;
    default: return false;
  }
  return false;
}

bool Crypto::ConvertBase64ToString(
  const std::string& input, std::string& output, std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
) {
  List<unsigned char> byteList;
  if (!Crypto::ConvertBase64ToBitStream(input, byteList, commentsOnFailure, commentsGeneral)) {
    return false;
  }
  Crypto::ConvertBitStreamToString(byteList, output);
  return true;
}

bool Crypto::ConvertBase64ToBitStream(
  const std::string& input,
  List<unsigned char>& output,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  MacroRegisterFunctionWithName("Crypto::Base64ToBitStream");
  output.Reserve((3 * input.size()) / 4 + 1);
  output.SetSize(0);
  uint32_t theStack = 0, sixBitDigit = 0;
  int numBitsInStack = 0;
  for (unsigned i = 0; i < input.size(); i ++) {
    bool isGood = Crypto::Get6bitFromChar(input[i], sixBitDigit);
    if (!isGood && input[i] == '\n') {
      continue;
    }
    if (!isGood && input[i] == '=' ) {
      theStack = 0;
      numBitsInStack += 6;
      numBitsInStack %= 8;
      continue;
    }
    if (!isGood) {
      if (commentsOnFailure != 0) {
        *commentsOnFailure << "<hr>Error: the input string: <br>\n"
        << input << "\n<br>had characters outside of base64";
      }
      return false;
    }
    theStack *= 64;
    theStack += sixBitDigit;
    numBitsInStack += 6;
    if (numBitsInStack == 12) {
      output.AddOnTop(theStack / 16);
      numBitsInStack = 4;
      theStack = theStack % 16;
    }
    if (numBitsInStack == 8) {
      output.AddOnTop(theStack);
      numBitsInStack = 0;
      theStack = 0;
    }
    if (numBitsInStack == 10) {
      output.AddOnTop(theStack / 4);
      numBitsInStack = 2;
      theStack = theStack % 4;
    }
  }
  if (commentsGeneral != 0 && numBitsInStack != 0) {
    *commentsGeneral << "<br>Base64: input corresponds modulo 8 to " << numBitsInStack
    << " bits. Perhaps the input was not padded correctly with = signs. The input: "
    << input;
  }
  return true;
}

void Crypto::ConvertBitStreamToString(const List<unsigned char>& input, std::string& output) {
  MacroRegisterFunctionWithName("Crypto::ConvertBitStreamToString");
  output.clear();
  output.reserve(input.size);
  for (int i = 0; i < input.size; i ++) {
    output.push_back(input[i]);
  }
}

void Crypto::ConvertStringToListBytes(const std::string& input, List<unsigned char>& output) {
  MacroRegisterFunctionWithName("Crypto::ConvertStringToListBytes");
  output.SetSize(input.size());
  for (unsigned i = 0; i < input.size(); i ++) {
    output[i] = input[i];
  }
}

void Crypto::ConvertStringToListBytesSigned(const std::string& input, List<char>& output) {
  MacroRegisterFunctionWithName("Crypto::ConvertStringToListBytesSigned");
  output.SetSize(input.size());
  for (unsigned i = 0; i < input.size(); i ++) {
    output[i] = input[i];
  }
}

List<int> Crypto::ConvertStringToListInt(const std::string& input) {
  List<int> result;
  result.SetSize(input.size());
  for (unsigned i = 0; i < input.size(); i ++) {
    result[i] = input[i];
  }
  return result;
}

std::string Crypto::ConvertStringToBase64Standard(const std::string& input) {
  List<unsigned char> inputChar;
  inputChar.SetSize(input.size());
  for (unsigned i = 0; i < input.size(); i ++) {
    inputChar[i] = input[i];
  }
  return Crypto::ConvertStringToBase64(inputChar, false);
}

std::string Crypto::ConvertStringToBase64URL(const std::string& input) {
  List<unsigned char> inputChar;
  inputChar.SetSize(input.size());
  for (unsigned i = 0; i < input.size(); i ++) {
    inputChar[i] = input[i];
  }
  return Crypto::ConvertStringToBase64(inputChar, true);
}

std::string Crypto::ConvertStringToBase64(const List<unsigned char>& input, bool useBase64URL) {
  MacroRegisterFunctionWithName("Crypto::ConvertStringToBase64");
  uint32_t theStack = 0;
  int numBitsInTheStack = 0;
  std::string result;
  result.reserve((input.size * 4) / 3 + 1);
  for (int i = 0; i < input.size; i ++) {
    theStack *= 256;
    theStack += input[i];
    numBitsInTheStack += 8;
    if (numBitsInTheStack == 8) {
      result.push_back(Crypto::GetCharFrom6bit(theStack / 4, useBase64URL));
      numBitsInTheStack = 2;
      theStack %= 4;
    }
    if (numBitsInTheStack == 10) {
      result.push_back(Crypto::GetCharFrom6bit(theStack / 16, useBase64URL));
      numBitsInTheStack = 4;
      theStack %= 16;
    }
    if (numBitsInTheStack == 12) {
      result.push_back(Crypto::GetCharFrom6bit(theStack / 64, useBase64URL));
      result.push_back(Crypto::GetCharFrom6bit(theStack % 64, useBase64URL));
      numBitsInTheStack = 0;
      theStack = 0;
    }
  }
  if (numBitsInTheStack == 2) {
    result.push_back(Crypto::GetCharFrom6bit(theStack * 16, useBase64URL));
    result.push_back('=');
    result.push_back('=');
  }
  if (numBitsInTheStack == 4) {
    result.push_back(Crypto::GetCharFrom6bit(theStack * 4, useBase64URL));
    result.push_back('=');
  }
  return result;
}

uint32_t Crypto::leftRotateAsIfBigEndian(uint32_t input, int numBitsToRotate) {
  uint32_t result = input;
  //Can be improved for speed but since not using bit operators,
  //this should be ultra portable
  for (int i = 0; i < numBitsToRotate; i ++) {
    uint32_t last_bit = result / 2147483648;
    result *= 2;
    result += last_bit;
  }
  //stOutput << "<hr>DEBUG: right rotate: " << numBitsToRotate << " bits maps: "
  //<< input << " to " << result;
  return result;
}

uint32_t Crypto::rightShiftAsIfBigEndian(uint32_t input, int numBitsToShift) {
  uint32_t result = input;
  //Can be improved for speed but since not using bit operators,
  //this should be ultra portable
  for (int i = 0; i < numBitsToShift; i ++) {
    result /= 2;
  }
  return result;
}

uint32_t Crypto::leftShiftAsIfBigEndian(uint32_t input, int numBitsToShift) {
  uint32_t result = input;
  //Can be improved for speed but since not using bit operators,
  //this should be ultra portable
  for (int i = 0; i < numBitsToShift; i ++) {
    result *= 2;
  }
  return result;
}

uint32_t Crypto::rightRotateAsIfBigEndian(uint32_t input, int numBitsToRotate) {
  uint32_t result = input;
  //Can be improved for speed but since not using bit operators,
  //this should be ultra portable
  for (int i = 0; i < numBitsToRotate; i ++) {
    uint32_t rightBit = result % 2;
    result /= 2;
    if (rightBit > 0) {
      result += 2147483648;
    }
  }
  return result;
}

void Crypto::ConvertUint32ToUcharBigendian(const List<uint32_t>& input, List<unsigned char>& output) {
  MacroRegisterFunctionWithName("Crypto::ConvertUint32ToUcharBigendian");
  output.SetSize(input.size * 4);
  for (int i = 0; i < input.size; i ++) {
    output[i * 4 + 0] = input[i] / 16777216;
    output[i * 4 + 1] = (input[i] / 65536) % 256;
    output[i * 4 + 2] = (input[i] / 256) % 256;
    output[i * 4 + 3] = input[i] % 256;
  }
}

void Crypto::ConvertUint32ToString(const List<uint32_t>& input, std::string& output) {
  MacroRegisterFunctionWithName("Crypto::ConvertUint32ToString");
  output.resize(input.size * 4);
  for (int i = 0; i < input.size; i ++) {
    output[i * 4 + 0] = (char) (input[i] / 16777216);
    output[i * 4 + 1] = (char) ((input[i] / 65536) % 256);
    output[i * 4 + 2] = (char) ((input[i] / 256) % 256);
    output[i * 4 + 3] = (char) (input[i] % 256);
  }
}

bool Crypto::ConvertHexToString(const std::string& input, std::string& output, std::stringstream* commentsOnFailure) {
  output.reserve(input.size() / 2);
  output.clear();
  bool result = true;
  for (unsigned i = 0; i < input.size(); i += 2) {
    // Skip single new line
    // characters appearing at even positions:
    if (input[i] == '\n') {
      i -= 1;
      continue;
    }
    unsigned char nextByte = 0;
    for (unsigned j = 0; j < 2; j ++) {
      if (i + j >= input.size()) {
        result = false;
        if (commentsOnFailure != 0) {
          *commentsOnFailure << "Input has an odd number of symbols. ";
        }
        break;
      }
      char currentChar = input[i + j];
      unsigned char theDigit = 255;
      if (currentChar >= 'A' && currentChar <= 'F') {
        theDigit = 10 + currentChar - 'A';
      }
      if (currentChar >= 'a' && currentChar <= 'f') {
        theDigit = 10 + currentChar - 'a';
      }
      if (currentChar >= '0' && currentChar <= '9') {
        theDigit = currentChar - '0';
      }
      if (theDigit != 255) {
        nextByte *= 16;
        nextByte += theDigit;
      } else {
        if (commentsOnFailure != 0 && result) {
          *commentsOnFailure << "Found unexpected character: " << currentChar;
        }
        result = false;
      }
    }
    output.push_back(nextByte);
  }
  return result;
}

bool Crypto::ConvertHexToInteger(const std::string& input, LargeIntUnsigned& output, int& outputNumLeadingZeroPairs) {
  output.MakeZero();
  bool foundNonZero = false;
  outputNumLeadingZeroPairs = 0;
  for (unsigned i = 0; i < input.size(); i ++) {
    int theDigit = - 1;
    if (input[i] >= 'A' && input[i] <= 'F') {
      theDigit = 10 + input[i] - 'A';
    }
    if (input[i] >= 'a' && input[i] <= 'f') {
      theDigit = 10 + input[i] - 'a';
    }
    if (input[i] >= '0' && input[i] <= '9') {
      theDigit = input[i] - '0';
    }
    if (theDigit > 0) {
      foundNonZero = true;
    }
    //stOutput << "DEBUG: got digit: " << theDigit;
    if (theDigit != - 1) {
      output *= 16;
      output += theDigit;
    }
    if (!foundNonZero) {
      outputNumLeadingZeroPairs ++;
    }
  }
  if (outputNumLeadingZeroPairs % 2 != 0) {
    outputNumLeadingZeroPairs --;
  }
  outputNumLeadingZeroPairs /= 2;
  return true;
}

std::string Crypto::ConvertStringToHex(const std::string& input, int byteWidthLineBreakZeroForNone, bool useHtml) {
  std::string result;
  Crypto::ConvertStringToHex(input, result, byteWidthLineBreakZeroForNone, useHtml);
  return result;
}

void Crypto::AppendDoubleSha256Check(const std::string& input, std::string& output) {
  std::string theSha256, doubleSha256;
  Crypto::computeSha256(input, theSha256);
  Crypto::computeSha256(theSha256, doubleSha256);
  output.reserve(input.size() + 4);
  output = input;
  for (int i = 0; i < 4; i ++) {
    output.push_back(doubleSha256[i]);
  }
}

bool Crypto::ConvertStringToHex(
  const std::string& input, std::string& output, int byteWidthLineBreakZeroForNone, bool useHtml
) {
  std::stringstream out;
  int lineBreakCounter = 0;
  for (unsigned i = 0; i < input.size(); i ++, lineBreakCounter ++) {
    if (byteWidthLineBreakZeroForNone > 0) {
      if (lineBreakCounter >= byteWidthLineBreakZeroForNone) {
        out << "\n";
        if (useHtml) {
          out << "<br>";
        }
        lineBreakCounter = 0;
      }
    }
    char high = ((unsigned char) input[i]) / 16;
    char low  = ((unsigned char) input[i]) % 16;
    if (high < 10) {
      out << (int) high;
    } else {
      out << ((unsigned char) ('a' + high - 10));
    }
    if (low < 10) {
      out << (int) low;
    } else {
      out << ((unsigned char) ('a' + low - 10));
    }
  }
  output = out.str();
  return true;
}

void Crypto::ConvertUint64toBigendianStringAppendResult(uint64_t& input, std::string& outputAppend) {
  //the following code should work on both big- and little-endian systems:
  outputAppend.push_back((char)  (input / 72057594037927936));
  outputAppend.push_back((char) ((input / 281474976710656) % 256));
  outputAppend.push_back((char) ((input / 1099511627776) % 256));
  outputAppend.push_back((char) ((input / 4294967296) % 256));
  outputAppend.push_back((char) ((input / 16777216) % 256));
  outputAppend.push_back((char) ((input / 65536) % 256));
  outputAppend.push_back((char) ((input / 256) % 256));
  outputAppend.push_back((char) (input % 256));
}

void Crypto::ConvertStringToListUInt32BigendianZeroPad(const std::string& input, List<uint32_t>& output) {
  List<unsigned char> resultChar;
  Crypto::ConvertStringToListBytes(input, resultChar);
  Crypto::ConvertStringToListUInt32BigendianZeroPad(resultChar, output);
}

void Crypto::ConvertStringToListUInt32BigendianZeroPad(const List<unsigned char>& input, List<uint32_t>& output) {
  MacroRegisterFunctionWithName("Crypto::ConvertStringToListUInt32BigendianZeroPad");
  List<unsigned char> theConvertor;
  theConvertor.SetSize(4);
  int finalSize = input.size / 4;
  if (input.size % 4 != 0) {
    finalSize ++;
  }
  output.SetSize(finalSize);
  for (int i = 0; i < output.size; i ++) {
    for (int j = 0; j < 4; j ++) {
      theConvertor[j] = (i * 4 + j < input.size) ? input[i * 4 + j] : 0;
    }
    output[i] = Crypto::GetUInt32FromCharBigendian(theConvertor);
  }
}

uint32_t Crypto::GetUInt32FromCharBigendian(const List<unsigned char>& input) {
  uint32_t result = input[0];
  for (int i = 1; i < 4; i ++) {
    result *= 256;
    result += input[i];
  }
  return result;
}

void Crypto::computeSha1(const std::string& inputString, List<uint32_t>& output) {
  MacroRegisterFunctionWithName("Crypto::computeSha1");
  //Reference: wikipedia page on sha1.
  //the Algorithm here is a direct implementation of the Wikipedia pseudocode.
  uint32_t h0 = 0x67452301;
  uint32_t h1 = 0xEFCDAB89;
  uint32_t h2 = 0x98BADCFE;
  uint32_t h3 = 0x10325476;
  uint32_t h4 = 0xC3D2E1F0;
  uint64_t messageLength = inputString.size() * 8;//*sizeof(char);
  std::string inputStringPreprocessed = inputString;
  //My reading of Wikipedia appears to suggest that if the message
  //is a multiple of 8 bits, padding with the bit 1
  //is not necessary. This appears to be false:
  //adding: if (messageLength % 256 == 0)
  //appears to produce wrong results.
  inputStringPreprocessed.push_back(0x80);
  unsigned numbytesMod64 = inputStringPreprocessed.size() % 64;
  if (numbytesMod64 > 56) {
    for (unsigned i = numbytesMod64; i < 64; i ++) {
      inputStringPreprocessed.push_back(0);
    }
    numbytesMod64 = 0;
  }
  for (int i = numbytesMod64; i < 56; i ++) {
    inputStringPreprocessed.push_back(0);
  }
  Crypto::ConvertUint64toBigendianStringAppendResult(messageLength, inputStringPreprocessed);
  List<unsigned char> convertorToUint32;
  List<uint32_t> inputStringUint32;
  inputStringUint32.Reserve(inputStringPreprocessed.size() / 4);
  convertorToUint32.SetSize(4);
  for (unsigned i = 0; i < inputStringPreprocessed.size() / 4; i ++) {
    convertorToUint32[0] = inputStringPreprocessed[i * 4];
    convertorToUint32[1] = inputStringPreprocessed[i * 4 + 1];
    convertorToUint32[2] = inputStringPreprocessed[i * 4 + 2];
    convertorToUint32[3] = inputStringPreprocessed[i * 4 + 3];
    inputStringUint32.AddOnTop(Crypto::GetUInt32FromCharBigendian(convertorToUint32));
  }
  List<uint32_t> currentChunk;
  currentChunk.SetSize(80);
  uint32_t a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, k = 0, temp = 0;
  for (int chunkCounter = 0; chunkCounter < inputStringUint32.size; chunkCounter += 16) {
    for (int j = 0; j < 16; j ++) {
      currentChunk[j] = inputStringUint32[chunkCounter + j];
    }
    for (int j = 16; j < 80; j ++) {
      currentChunk[j] = Crypto::leftRotateAsIfBigEndian((
          currentChunk[j - 3] xor
          currentChunk[j - 8] xor
          currentChunk[j - 14] xor
          currentChunk[j - 16]
        ), 1
      );
    }
    a = h0;
    b = h1;
    c = h2;
    d = h3;
    e = h4;
    for (int j = 0; j < 80; j ++) {
      if (j < 20) {
        f = (b bitand c) bitor ((compl b) bitand d);
        k = 0x5A827999;
      } else if (20 <= j and j < 40) {
        f = b xor c xor d;
        k = 0x6ED9EBA1;
      } else if (40 <= j and j < 60) {
        f = (b bitand c) bitor (b bitand d) bitor (c bitand d);
        k = 0x8F1BBCDC;
      } else {
        //60<= j
        f = b xor c xor d;
        k = 0xCA62C1D6;
      }
      temp = Crypto::leftRotateAsIfBigEndian(a, 5) + f + e + k + currentChunk[j];
      e = d;
      d = c;
      c = Crypto::leftRotateAsIfBigEndian(b, 30);
      b = a;
      a = temp;
    }
    h0 += a;
    h1 += b;
    h2 += c;
    h3 += d;
    h4 += e;
  }
  output.SetSize(5);
  output[0] = h0;
  output[1] = h1;
  output[2] = h2;
  output[3] = h3;
  output[4] = h4;
}

void Crypto::initSha256() {
  MacroRegisterFunctionWithName("Crypto::initSha256");
  if (Crypto::kArraySha2xx.size != 0) {
    return;
  }
  Crypto::kArraySha2xx.SetSize(64);
  Crypto::kArraySha2xx[0]  = 0x428a2f98;
  Crypto::kArraySha2xx[1]  = 0x71374491;
  Crypto::kArraySha2xx[2]  = 0xb5c0fbcf;
  Crypto::kArraySha2xx[3]  = 0xe9b5dba5;
  Crypto::kArraySha2xx[4]  = 0x3956c25b;
  Crypto::kArraySha2xx[5]  = 0x59f111f1;
  Crypto::kArraySha2xx[6]  = 0x923f82a4;
  Crypto::kArraySha2xx[7]  = 0xab1c5ed5;
  Crypto::kArraySha2xx[8]  = 0xd807aa98;
  Crypto::kArraySha2xx[9]  = 0x12835b01;
  Crypto::kArraySha2xx[10] = 0x243185be;
  Crypto::kArraySha2xx[11] = 0x550c7dc3;
  Crypto::kArraySha2xx[12] = 0x72be5d74;
  Crypto::kArraySha2xx[13] = 0x80deb1fe;
  Crypto::kArraySha2xx[14] = 0x9bdc06a7;
  Crypto::kArraySha2xx[15] = 0xc19bf174;
  Crypto::kArraySha2xx[16] = 0xe49b69c1;
  Crypto::kArraySha2xx[17] = 0xefbe4786;
  Crypto::kArraySha2xx[18] = 0x0fc19dc6;
  Crypto::kArraySha2xx[19] = 0x240ca1cc;
  Crypto::kArraySha2xx[20] = 0x2de92c6f;
  Crypto::kArraySha2xx[21] = 0x4a7484aa;
  Crypto::kArraySha2xx[22] = 0x5cb0a9dc;
  Crypto::kArraySha2xx[23] = 0x76f988da;
  Crypto::kArraySha2xx[24] = 0x983e5152;
  Crypto::kArraySha2xx[25] = 0xa831c66d;
  Crypto::kArraySha2xx[26] = 0xb00327c8;
  Crypto::kArraySha2xx[27] = 0xbf597fc7;
  Crypto::kArraySha2xx[28] = 0xc6e00bf3;
  Crypto::kArraySha2xx[29] = 0xd5a79147;
  Crypto::kArraySha2xx[30] = 0x06ca6351;
  Crypto::kArraySha2xx[31] = 0x14292967;
  Crypto::kArraySha2xx[32] = 0x27b70a85;
  Crypto::kArraySha2xx[33] = 0x2e1b2138;
  Crypto::kArraySha2xx[34] = 0x4d2c6dfc;
  Crypto::kArraySha2xx[35] = 0x53380d13;
  Crypto::kArraySha2xx[36] = 0x650a7354;
  Crypto::kArraySha2xx[37] = 0x766a0abb;
  Crypto::kArraySha2xx[38] = 0x81c2c92e;
  Crypto::kArraySha2xx[39] = 0x92722c85;
  Crypto::kArraySha2xx[40] = 0xa2bfe8a1;
  Crypto::kArraySha2xx[41] = 0xa81a664b;
  Crypto::kArraySha2xx[42] = 0xc24b8b70;
  Crypto::kArraySha2xx[43] = 0xc76c51a3;
  Crypto::kArraySha2xx[44] = 0xd192e819;
  Crypto::kArraySha2xx[45] = 0xd6990624;
  Crypto::kArraySha2xx[46] = 0xf40e3585;
  Crypto::kArraySha2xx[47] = 0x106aa070;
  Crypto::kArraySha2xx[48] = 0x19a4c116;
  Crypto::kArraySha2xx[49] = 0x1e376c08;
  Crypto::kArraySha2xx[50] = 0x2748774c;
  Crypto::kArraySha2xx[51] = 0x34b0bcb5;
  Crypto::kArraySha2xx[52] = 0x391c0cb3;
  Crypto::kArraySha2xx[53] = 0x4ed8aa4a;
  Crypto::kArraySha2xx[54] = 0x5b9cca4f;
  Crypto::kArraySha2xx[55] = 0x682e6ff3;
  Crypto::kArraySha2xx[56] = 0x748f82ee;
  Crypto::kArraySha2xx[57] = 0x78a5636f;
  Crypto::kArraySha2xx[58] = 0x84c87814;
  Crypto::kArraySha2xx[59] = 0x8cc70208;
  Crypto::kArraySha2xx[60] = 0x90befffa;
  Crypto::kArraySha2xx[61] = 0xa4506ceb;
  Crypto::kArraySha2xx[62] = 0xbef9a3f7;
  Crypto::kArraySha2xx[63] = 0xc67178f2;
}

bool Crypto::ConvertLargeUnsignedIntToHexSignificantDigitsFirst(
  const LargeIntUnsigned& input, int numberOfLeadingZeroesToPadWith, std::string& output
) {
  std::string outputString;
  Crypto::ConvertLargeUnsignedIntToStringSignificantDigitsFirst(input, numberOfLeadingZeroesToPadWith, outputString);
  Crypto::ConvertStringToHex(outputString, output, 0, false);
  return true;
}

bool Crypto::ConvertLargeUnsignedIntToStringSignificantDigitsFirst(
  const LargeIntUnsigned& input, int numberOfLeadingZeroesToPadWith, std::string& output
) {
  List<char> result;
  LargeIntUnsigned digit, inputCopy = input;
  while (inputCopy > 0) {
    digit = inputCopy % 256;
    inputCopy /= 256;
    int digitInt = 0;
    digit.IsIntegerFittingInInt(&digitInt);
    char digitChar = (char) digitInt;
    result.AddOnTop(digitChar);
  }
  for (int i = 0; i < numberOfLeadingZeroesToPadWith; i ++) {
    result.AddOnTop(0);
  }
  result.ReverseOrderElements();
  output.assign(result.TheObjects, result.size);
  return true;
}

bool Crypto::ConvertLargeUnsignedIntToBase64SignificantDigitsFirst(
  const LargeIntUnsigned& input, std::string& outputBase64
) {
  std::string theString;
  Crypto::ConvertLargeUnsignedIntToStringSignificantDigitsFirst(input, 0, theString);
  outputBase64 = Crypto::ConvertStringToBase64(theString, false);
  return true;
}

bool Crypto::ConvertBase64ToLargeUnsignedInt(
  const std::string& inputBase64, LargeIntUnsigned& output, std::stringstream* commentsOnFailure
) {
  List<unsigned char> theBitStream;
  if (!Crypto::ConvertBase64ToBitStream(inputBase64, theBitStream, commentsOnFailure)) {
    return false;
  }
  Crypto::ConvertBitStreamToLargeUnsignedInt(theBitStream, output);
  return true;
}

void Crypto::ConvertListUintToLargeUInt(List<uint32_t>& input, LargeIntUnsigned& output) {
  output = 0;
  LargeIntUnsigned twoPower32;
  twoPower32 = 65536;
  twoPower32 *= 65536;
  for (int i = input.size - 1; i >= 0; i --) {
    output *= twoPower32;
    output += input[i];
  }
}

void Crypto::ConvertBitStreamToLargeUnsignedInt(const List<unsigned char>& input, LargeIntUnsigned& output) {
  output = 0;
  for (int i = 0; i < input.size; i ++) {
    output *= 256;
    output += ((unsigned int) input[i]);
  }
}

void Crypto::ConvertLargeIntUnsignedToBase58SignificantDigitsLAST(
  const LargeIntUnsigned& input, std::string& output
) {
  MacroRegisterFunctionWithName("Crypto::ConvertLargeIntUnsignedToBase58SignificantDigitsLAST");
  output.clear();
  LargeIntUnsigned copy = input;
  while (copy > 0) {
    LargeIntUnsigned nextDigitLIU = copy % 58;
    copy /= 58;
    int32_t nextDigitInt = nextDigitLIU.theDigits[0];
    char next = '#';
    Crypto::GetCharFromBase58(nextDigitInt, next);
    output.push_back(next);
  }
}

void Crypto::ConvertLargeIntUnsignedToBase58SignificantDigitsFIRST(
  const LargeIntUnsigned& input, std::string& output, int numberOfOnesToPrepend
) {
  std::string outputReversed;
  Crypto::ConvertLargeIntUnsignedToBase58SignificantDigitsLAST(input, outputReversed);
  output.clear();
  output.reserve(outputReversed.size() + numberOfOnesToPrepend);
  for (int i = 0; i < numberOfOnesToPrepend; i ++) {
    output.push_back('1');
  }
  for (unsigned i = 0; i < outputReversed.size(); i ++) {
    output.push_back(outputReversed[outputReversed.size() - 1 - i]);
  }
}

bool Crypto::ConvertBase58SignificantDigitsFIRSTToLargeIntUnsigned(
  const std::string& input, LargeIntUnsigned& output, int& numberOfLeadingZeroes, std::stringstream* commentsOnFailure
) {
  output = 0;
  bool result = true;
  numberOfLeadingZeroes = 0;
  for (unsigned i = 0; i < input.size(); i ++) {
    output *= 58;
    uint32_t currentChar;
    if (!Crypto::GetBase58FromChar(input[i], currentChar)) {
      currentChar = 0;
      if (result) {
        if (commentsOnFailure != 0) {
          *commentsOnFailure << "Failed to extract base 58 digit from: " << input[i] << " (position " << i << "). ";
        }
      }
      result = false;
    }
    output += currentChar;
    if (output.IsEqualToZero()) {
      numberOfLeadingZeroes ++;
    }
  }
  return result;
}

bool Crypto::ConvertBase58ToHexSignificantDigitsFirst(
  const std::string& input, std::string& output, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Crypto::ConvertBase58ToHexSignificantDigitsFirst");
  LargeIntUnsigned outputLIU;
  bool result = true;
  int numberOfLeadingZeroes = 0;
  if (!Crypto::ConvertBase58SignificantDigitsFIRSTToLargeIntUnsigned(
    input, outputLIU, numberOfLeadingZeroes, commentsOnFailure
  )) {
    result = false;
  }
  if (!Crypto::ConvertLargeUnsignedIntToHexSignificantDigitsFirst(outputLIU, numberOfLeadingZeroes, output)) {
    return false;
  }
  return result;
}

void Crypto::ConvertStringToLargeIntUnsigned(const std::string& input, LargeIntUnsigned& output) {
  output = 0;
  for (unsigned i = 0; i < input.size(); i ++) {
    output *= 256;
    output += ((unsigned int) input[i]);
  }
}

void Crypto::computeSha224(const std::string& inputString, List<uint32_t>& output) {
  MacroRegisterFunctionWithName("Crypto::computeSha224");
  return Crypto::computeSha2xx(inputString, output, true);
}

void Crypto::computeSha256(const std::string& inputString, std::string& output) {
 
  List<uint32_t> theSha256Uint;
  Crypto::computeSha256(inputString, theSha256Uint);
  Crypto::ConvertUint32ToString(theSha256Uint, output);
}

void Crypto::computeSha256(const std::string& inputString, List<uint32_t>& output) {
  MacroRegisterFunctionWithName("Crypto::computeSha256");
  return Crypto::computeSha2xx(inputString, output, false);
}

List<uint32_t> Crypto::kArraySha2xx;

void Crypto::computeSha2xx(const std::string& inputString, List<uint32_t>& output, bool is224) {
  MacroRegisterFunctionWithName("Crypto::computeSha2xx");
  //Reference: wikipedia page on sha256.
  //the Algorithm here is a direct implementation of the Wikipedia pseudocode.
  uint32_t h0 = 0x6a09e667;
  uint32_t h1 = 0xbb67ae85;
  uint32_t h2 = 0x3c6ef372;
  uint32_t h3 = 0xa54ff53a;
  uint32_t h4 = 0x510e527f;
  uint32_t h5 = 0x9b05688c;
  uint32_t h6 = 0x1f83d9ab;
  uint32_t h7 = 0x5be0cd19;
  if (is224) {
    h0 = 0xc1059ed8;
    h1 = 0x367cd507;
    h2 = 0x3070dd17;
    h3 = 0xf70e5939;
    h4 = 0xffc00b31;
    h5 = 0x68581511;
    h6 = 0x64f98fa7;
    h7 = 0xbefa4fa4;
  }
  Crypto::initSha256();
  //stOutput << "DEBUG: start string length: " << inputString.size();
  uint64_t messageLength = inputString.size() * 8;//*sizeof(char);
  std::string inputStringPreprocessed = inputString;
  inputStringPreprocessed.push_back(0x80);
  unsigned numbytesMod64 = inputStringPreprocessed.size() % 64;
  if (numbytesMod64 > 56) {
    for (unsigned i = numbytesMod64; i < 64; i ++) {
      inputStringPreprocessed.push_back(0);
    }
    numbytesMod64 = 0;
  }
  for (int i = numbytesMod64; i < 56; i ++) {
    inputStringPreprocessed.push_back(0);
  }
  Crypto::ConvertUint64toBigendianStringAppendResult(messageLength, inputStringPreprocessed);
////////////////////////
//  std::stringstream tempSTream;
//  for (unsigned i = 0; i < inputStringPreprocessed.size(); i ++)
//  {
//    tempSTream << std::hex << inputStringPreprocessed[i];
//  }
//  stOutput << "<hr>DEBUG: first stream: " << tempSTream.str();
///////////////////////
  List<unsigned char> convertorToUint32;
  List<uint32_t> inputStringUint32;
  inputStringUint32.Reserve(1 + inputStringPreprocessed.size() / 4);
  convertorToUint32.SetSize(4);
  for (unsigned i = 0; i < inputStringPreprocessed.size() / 4; i ++) {
    convertorToUint32[0] = inputStringPreprocessed[i * 4];
    convertorToUint32[1] = inputStringPreprocessed[i * 4 + 1];
    convertorToUint32[2] = inputStringPreprocessed[i * 4 + 2];
    convertorToUint32[3] = inputStringPreprocessed[i * 4 + 3];
    inputStringUint32.AddOnTop(Crypto::GetUInt32FromCharBigendian(convertorToUint32));
  }
  List<uint32_t> currentChunk;
  currentChunk.SetSize(64);
  uint32_t a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, g = 0, h = 0, maj = 0, temp1 = 0, temp2 = 0;
  uint32_t s0 = 0, s1 = 0, ch = 0;
  List<uint32_t>& kArray = Crypto::kArraySha2xx;
  for (int chunkCounter = 0; chunkCounter < inputStringUint32.size; chunkCounter += 16) {
    for (int j = 0; j < 16; j ++) {
      currentChunk[j] = inputStringUint32[chunkCounter + j];
    }
    for (int j = 16; j < 64; j ++) {
      s0 =
      Crypto::rightRotateAsIfBigEndian(currentChunk[j - 15], 7) xor
      Crypto::rightRotateAsIfBigEndian(currentChunk[j - 15], 18) xor
      Crypto::rightShiftAsIfBigEndian(currentChunk[j - 15], 3);
      s1 =
      Crypto::rightRotateAsIfBigEndian(currentChunk[j - 2], 17) xor
      Crypto::rightRotateAsIfBigEndian(currentChunk[j - 2], 19) xor
      Crypto::rightShiftAsIfBigEndian(currentChunk[j - 2], 10);
      currentChunk[j] = currentChunk[j - 16] + s0 + currentChunk[j - 7] + s1;
    }
    a = h0;
    b = h1;
    c = h2;
    d = h3;
    e = h4;
    f = h5;
    g = h6;
    h = h7;
    for (int j = 0; j < 64; j ++) {
      s1 =
      Crypto::rightRotateAsIfBigEndian(e, 6) xor
      Crypto::rightRotateAsIfBigEndian(e, 11) xor
      Crypto::rightRotateAsIfBigEndian(e, 25);
      ch = (e bitand f) xor ((compl e) bitand g);
      temp1 = h + s1 + ch + kArray[j] + currentChunk[j];
      s0 =
      Crypto::rightRotateAsIfBigEndian(a, 2) xor
      Crypto::rightRotateAsIfBigEndian(a, 13) xor
      Crypto::rightRotateAsIfBigEndian(a, 22);
      maj = (a bitand b) xor (a bitand c) xor (b bitand c);
      temp2 = s0 + maj;
      h = g            ;
      g = f            ;
      f = e            ;
      e = d + temp1    ;
      d = c            ;
      c = b            ;
      b = a            ;
      a = temp1 + temp2;
    }
    h0 += a;
    h1 += b;
    h2 += c;
    h3 += d;
    h4 += e;
    h5 += f;
    h6 += g;
    h7 += h;
  }
  if (is224) {
    output.SetSize(7);
  } else {
    output.SetSize(8);
  }
  output[0] = h0;
  output[1] = h1;
  output[2] = h2;
  output[3] = h3;
  output[4] = h4;
  output[5] = h5;
  output[6] = h6;
  if (!is224) {
    output[7] = h7;
  }
}

bool CertificateRSA::LoadFromJSON(JSData& input, std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral) {
  MacroRegisterFunctionWithName("Certificate::LoadFromJSON");
  if (commentsGeneral != 0) {
    *commentsGeneral << "<hr>Loading certificate from: "
    << input.ToString(true);
  }
  if (input.theType != JSData::token::tokenObject) {
    if (commentsOnFailure != 0) {
      *commentsOnFailure << "Can't load certificate: JSON not of type object. ";
    }
    return false;
  }
  this->algorithm = "";
  this->keyid = "";
  this->theModulusString = "";
  this->theExponentString = "";
  if (input.HasKey("alg")) {
    this->algorithm = input.GetValue("alg").theString;
  }
  if (input.HasKey("kid")) {
    this->keyid = input.GetValue("kid").theString;
  }
  if (input.HasKey("n")) {
    this->theModulusString = input.GetValue("n").theString;
    if (!Crypto::ConvertBase64ToLargeUnsignedInt(this->theModulusString, this->theModuluS, commentsOnFailure)) {
      return false;
    }
  }
  if (input.HasKey("e")) {
    this->theExponentString = input.GetValue("e").theString;
    if (!Crypto::ConvertBase64ToLargeUnsignedInt(this->theExponentString, this->theExponenT, commentsOnFailure)) {
      return false;
    }
  }
  return true;
}

List<CertificateRSA> Crypto::knownCertificates;

bool Crypto::LoadOneKnownCertificate(
  const std::string& input, std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
) {
  MacroRegisterFunctionWithName("Crypto::LoadOneKnownCertificate");
  if (commentsGeneral != 0) {
    *commentsGeneral << "Loading from: " << input;
  }
  JSData certificateJSON;
  if (!certificateJSON.readstring(input, false, commentsOnFailure)) {
    return false;
  }
  CertificateRSA currentCert;
  bool isGood = false;
  if (certificateJSON.theType == JSData::token::tokenObject) {
    if (certificateJSON.HasKey("keys")) {
      JSData theKeys = certificateJSON.GetValue("keys");
      if (theKeys.theType == JSData::token::tokenArray) {
        isGood = true;
        for (int i = 0; i < theKeys.theList.size; i ++) {
          if (!currentCert.LoadFromJSON(theKeys.theList[i], commentsOnFailure, commentsGeneral)) {
            return false;
          }
          Crypto::knownCertificates.AddOnTop(currentCert);
        }
      }
    }
  }
  if (!isGood) {
    if (commentsOnFailure != 0) {
      *commentsOnFailure << "I expected an object with key 'keys'"
      << " consisting of an array of public keys. Instead, I got: "
      << certificateJSON.ToString(true);
    }
    return false;
  }
  return true;
}

std::string CertificateRSA::ToString() {
  std::stringstream out;
  out << "Algorithm: " << this->algorithm << "\n<br>\n";
  out << "Keyid: "     << this->keyid << "\n<br>\n";
  out << "Modulus [base64, math]: [" << this->theModulusString
  << ", " << this->theModuluS.ToString() << "]" << "\n<br>\n";
  out << "Exponent [base64, math]: " << "["
  << this->theExponentString << ", "
  << this->theExponenT.ToString()
  << "\n<br>\n";
  return out.str();
}

bool Crypto::LoadKnownCertificates(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral) {
  MacroRegisterFunctionWithName("Crypto::LoadKnownCertificates");
  Crypto::knownCertificates.SetSize(0);
  List<std::string> theFileNames;
  if (! FileOperations::GetFolderFileNamesVirtual("certificates-public/", theFileNames, 0)) {
    if (commentsOnFailure != 0) {
      *commentsOnFailure << "Could not open folder certificates-public/, no certificates loaded.";
    }
    return false;
  }
  if (commentsGeneral != 0) {
    *commentsGeneral << "<br>Certificates: ";
  }
  for (int i = 0; i < theFileNames.size; i ++) {
    if (theFileNames[i] == "." || theFileNames[i] == "..") {
      continue;
    }
    if (MathRoutines::StringBeginsWith(theFileNames[i], "debug")) {
      continue;
    }
    if (MathRoutines::StringEndsWith(theFileNames[i], "readme.md")) {
      continue;
    }
    if (MathRoutines::StringEndsWith(theFileNames[i], "README.md")) {
      continue;
    }
    std::string currentCert;
    if (!FileOperations::LoadFileToStringVirtual(
      "certificates-public/" + theFileNames[i], currentCert, false, false, commentsOnFailure
    )) {
      continue;
    }
    if (!Crypto::LoadOneKnownCertificate(currentCert, commentsOnFailure, commentsGeneral)) {
      return false;
    }
  }
  for (int i = 0; i < Crypto::knownCertificates.size; i ++) {
    if (commentsGeneral != 0) {
      *commentsGeneral << "\n<hr>\nLoaded: " << Crypto::knownCertificates[i].ToString();
    }
  }
  return true;
}

LargeIntUnsigned Crypto::RSAencrypt(
  const LargeIntUnsigned& theModulus, const LargeInt& theExponent, const LargeInt& theMessage
) {
  MacroRegisterFunctionWithName("Crypto::RSAencrypt");
  if (theModulus == 0 || theExponent == 0) {
    crash << "The modulus and the exponent are not allowed to be zero while running RSA. "
    << "The modulus: " << theModulus.ToString() << "; the exponent: " << theExponent.ToString() << crash;
  }
  ElementZmodP theElt, theOne;
  theElt.theModulo = theModulus;
  theOne.theValue = 1;
  theOne.theModulo = theModulus;
  theElt.AssignRational(theMessage);
  MathRoutines::RaiseToPower(theElt, theExponent, theOne);
  return theElt.theValue;
}

std::string JSONWebToken::ToString() {
  std::stringstream out;
  out << "JWT token.";
  out << "<br>Header: " << this->headerJSON;
  out << "<br>Claims: " << this->claimsJSON;
  out << "<br>Signature base64: " << this->signatureBase64;
  return out.str();
}

bool JSONWebToken::VerifyRSA256(
  const LargeIntUnsigned& theModulus,
  const LargeIntUnsigned& theExponent,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  std::string payload = this->headerBase64 + '.' + this->claimsBase64;
  if (commentsGeneral != 0) {
    *commentsGeneral << "<br>Payload: " << payload;
  }
  List<uint32_t> outputSha, RSAresultInts;
  Crypto::computeSha256(payload, outputSha);
  if (commentsGeneral != 0) {
    LargeIntUnsigned theSha;
    Crypto::ConvertListUintToLargeUInt(outputSha, theSha);
    *commentsGeneral << "<br>Sha256 of payload: " << theSha.ToString();
  }
  LargeIntUnsigned theSignatureInt;
  if (!Crypto::ConvertBase64ToLargeUnsignedInt(this->signatureBase64, theSignatureInt, commentsOnFailure)) {
    return false;
  }
  double timeStart = - 1;
  if (commentsGeneral != 0) {
    timeStart = theGlobalVariables.GetElapsedSeconds();
  }
  if (theModulus == 0 || theExponent == 0) {
    if (commentsOnFailure != 0) {
      *commentsOnFailure << "The modulus and the exponent must be non-zero. "
      << "The mod is: " << theModulus << "; the exponent is: " << theExponent;
    }
    return false;
  }
  LargeIntUnsigned RSAresult = Crypto::RSAencrypt(theModulus, theExponent, theSignatureInt);
  if (commentsGeneral != 0) {
    *commentsGeneral << "<br>RSA encryption took: "
    << theGlobalVariables.GetElapsedSeconds() - timeStart << " second(s).<br>";
  }
  std::string RSAresultBitstream, RSAresultLast32bytes;
  Crypto::ConvertLargeUnsignedIntToStringSignificantDigitsFirst(RSAresult, 0, RSAresultBitstream);
  if (RSAresultBitstream.size() > 32) {
    RSAresultLast32bytes = RSAresultBitstream.substr(RSAresultBitstream.size() - 32, 32);
  }
  Crypto::ConvertStringToListUInt32BigendianZeroPad(RSAresultLast32bytes, RSAresultInts);
  bool result = (RSAresultInts == outputSha);
  if (!result) {
    RSAresultInts.ReverseOrderElements();
    result = (RSAresultInts == outputSha);
  }
  if ((!result && commentsOnFailure != 0) || commentsGeneral != 0) {
    std::string RSAresultTrimmedHex, theShaHex, RSAresultHex, RSAresultBase64;
    LargeIntUnsigned theShaUI;
    Crypto::ConvertListUintToLargeUInt(outputSha, theShaUI);
    RSAresultBase64 = Crypto::ConvertStringToBase64Standard(RSAresultBitstream);
    Crypto::ConvertStringToHex(RSAresultBitstream, RSAresultHex, 0, false);
    Crypto::ConvertStringToHex(RSAresultLast32bytes, RSAresultTrimmedHex, 0, false);
    Crypto::ConvertLargeUnsignedIntToHexSignificantDigitsFirst(theShaUI, 0, theShaHex);
    if (!result && commentsOnFailure != 0) {
      *commentsOnFailure << "<br><b style =\"color:red\">Token invalid: the "
      << "SHA does not match the RSA result. </b>";
    } else if (commentsGeneral != 0) {
      *commentsGeneral << "<b style =\"color:green\">Validated. </b>";
    }
    if (commentsGeneral != 0) {
      *commentsGeneral
      << "<br>Sha integer: " << theShaUI.ToString()
      << "<br>Encrypted signature: " << RSAresult.ToString()
      << "<br>sha hex: " << theShaHex
      << "<br>RSAresult hex: " << RSAresultHex
      << "<br>trimmed: " << RSAresultTrimmedHex
      << "<br>RSAresult base64: " << RSAresultBase64;
    }
  }
  return result;
}

bool JSONWebToken::AssignString(const std::string& other, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("JSONWebToken::AssignString");
  List<std::string> theStrings;
  MathRoutines::StringSplitExcludeDelimiter(other, '.', theStrings);
  if (theStrings.size != 3) {
    if (commentsOnFailure != 0) {
      *commentsOnFailure << "Expected 3 strings separated by two dots, got: "
      << theStrings.size << " strings, obtained from: " << other;
    }
    return false;
  }
  this->headerBase64    = theStrings[0];
  this->claimsBase64    = theStrings[1];
  this->signatureBase64 = theStrings[2];
  if (!Crypto::ConvertBase64ToString(this->headerBase64, this->headerJSON, commentsOnFailure)) {
    return false;
  }
  if (!Crypto::ConvertBase64ToString(this->claimsBase64, this->claimsJSON, commentsOnFailure)) {
    return false;
  }
  return true;
}
