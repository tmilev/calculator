#include "json.h"
#include "math_general.h"

#include <sys/stat.h>
#include <cassert>

void JSData::operator=(const LargeInteger& other) {
  this->reset();
  this->theType = JSData::token::tokenLargeInteger;
  this->theInteger.getElement() = other;
}

void JSData::operator=(int other) {
  this->reset();
  this->theType = JSData::token::tokenLargeInteger;
  this->theInteger.getElement() = other;
}

void JSData::operator=(const bool other) {
  this->reset();
  this->theType = JSData::token::tokenBool;
  this->theBoolean = other;
}

void JSData::operator=(const double other) {
  this->reset();
  this->theType = JSData::token::tokenFloat;
  this->theFloat = other;
}

void JSData::operator=(const char* other) {
  std::string otherString(other);
  this->operator=(otherString);
}

void JSData::operator=(const std::string& other) {
  this->reset();
  this->theType = JSData::token::tokenString;
  this->theString = other;
}

void JSData::operator=(const JSData& other) {
  this->theType = other.theType;
  this->theBoolean = other.theBoolean;
  this->theFloat = other.theFloat;
  this->theInteger = other.theInteger;
  this->theString = other.theString;
  this->theList = other.theList;
  this->objects = other.objects;
}

JSData& JSData::operator[](int i) {
  this->theType = JSData::token::tokenArray;
  if (this->theList.size < i + 1) {
    this->theList.setSize(i + 1);
  }
  return this->theList[i];
}

JSData JSData::getValue(const std::string& key) {
  int theIndex = this->GetKeyIndex(key);
  if (theIndex != - 1) {
    return this->objects.theValues[theIndex];
  }
  JSData result;
  result.theType = JSData::token::tokenUndefined;
  return result;
}

bool JSData::HasKey(const std::string& key) const {
  return this->GetKeyIndex(key) != - 1;
}

bool JSData::HasCompositeKeyOfType(
  const std::string& key,
  LargeIntegerUnsigned& output,
  std::stringstream* commentsOnFailure
) const {
  JSData container;
  if (!this->HasCompositeKeyOfTokeN(
    key,
    &container,
    JSData::token::tokenLargeInteger,
    commentsOnFailure
  )) {
    return false;
  }
  if (container.theInteger.getElement() < 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Error: integer is negative. ";
    }
    return false;
  }
  output = container.theInteger.getElement().value;
  return true;
}

bool JSData::HasCompositeKeyOfType(
  const std::string& key, std::string& output, std::stringstream* commentsOnFailure
) const {
  JSData container;
  if (!this->HasCompositeKeyOfTokeN(key, &container, JSData::token::tokenString, commentsOnFailure)) {
    return false;
  }
  output = container.theString;
  return true;
}

bool JSData::HasCompositeKeyOfType(
  const std::string& key, List<unsigned char>& output, std::stringstream* commentsOnFailure
) const {
  JSData container;
  if (!this->HasCompositeKeyOfTokeN(key, &container, JSData::token::tokenString, commentsOnFailure)) {
    return false;
  }
  output = container.theString;
  return true;
}

bool JSData::HasCompositeKeyValueNull(const std::string &key, std::stringstream *commentsOnFailure) const {
  return this->HasCompositeKeyOfTokeN(key, nullptr, JSData::token::tokenNull, commentsOnFailure);
}

bool JSData::HasCompositeKeyOfTokeN(
  const std::string& key,
  JSData* whichValue,
  char targetType,
  std::stringstream* commentsOnFailure
) const {
  JSData container;
  if (whichValue == nullptr) {
    whichValue = &container;
  }
  if (!JSData::HasCompositeKey(key, whichValue, commentsOnFailure)) {
    return false;
  }
  if (whichValue->theType != targetType) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Key: " << key << ", value: "
      << whichValue->toString(nullptr) << " is of type "
      << static_cast<int>(whichValue->theType)
      << " instead of the target one: " << static_cast<int>(targetType)
      << ". ";
    }
    return false;
  }
  return true;
}

bool JSData::HasCompositeKey(const std::string& inputKeys, JSData* whichValue, std::stringstream* commentsOnFailure) const {
  List<char> delimiters;
  delimiters.addOnTop('.');
  delimiters.addOnTop('[');
  delimiters.addOnTop(']');
  List<std::string> keys;
  StringRoutines::StringSplitExcludeDelimiters(inputKeys, delimiters, keys);
  const JSData* currentData = this;
  if (keys.size == 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Could not extract any keys from your input: " << inputKeys;
    }
    return false;
  }
  for (int i = 0; i < keys.size; i ++) {
    // will not work for integer indices larger than 9
    int theDigit = - 1;
    if (MathRoutines::isADigit(keys[i], &theDigit)) {
      if (currentData->theType != JSData::token::tokenArray) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "The sub-object located before the key: " << keys[i] << " [all keys: "
          << keys.toStringCommaDelimited() << "] "
          << "is not an array, but is instead: "
          << currentData->toString(&JSData::PrintOptions::HTML())
          << ". ";
        }
        return false;
      }
      if (currentData->theList.size <= theDigit) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "Key index: " << theDigit
          << " is too large for current value: " << currentData->toString(&JSData::PrintOptions::HTML());
        }
        return false;
      }
      currentData = &currentData->theList[theDigit];
      continue;
    }
    if (currentData->theType != JSData::token::tokenObject) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Value preceding key: " << keys[i]
        << " is not of type object: " << currentData->toString(&JSData::PrintOptions::HTML());
      }
      return false;
    }
    if (!currentData->HasKey(keys[i])) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Key: " << keys[i]
        << " not present in: " << currentData->toString(&JSData::PrintOptions::HTML());
      }
      return false;
    }
    currentData = &currentData->objects.getValueNoFail(keys[i]);
  }
  if (whichValue != nullptr) {
    *whichValue = *currentData;
  }
  return true;
}

int JSData::GetKeyIndex(const std::string& key) const {
  return this->objects.getIndex(key);
}

void JSData::setKeyValue(const std::string& key, const JSData& value) {
  (*this)[key] = value;
}

JSData& JSData::operator[](const std::string& key) {
  this->theType = JSData::token::tokenObject;
  return this->objects.GetValueCreate(key);
}

void JSData::operator=(const List<unsigned char>& other) {
  this->theType = JSData::token::tokenString;
  this->theString = other.toStringConcatenate();
  this->theList.setSize(0); ;
  this->objects.clear();
}

void JSData::operator=(const List<JSData>& other) {
  this->theType = JSData::token::tokenArray;
  this->theList.setSize(other.size);
  for (int i = 0; i < other.size; i ++) {
    this->theList[i] = other[i];
  }
  this->objects.clear();
}

void JSData::operator=(int64_t input) {
  this->theType = JSData::token::tokenLargeInteger;
  this->theInteger.getElement().AssignInt64(input);
}

bool JSData::isIntegerFittingInInt(int* whichInteger) {
  if (this->theType == JSData::token::tokenLargeInteger) {
    return this->theInteger.getElement().IsIntegerFittingInInt(whichInteger);
  }
  if (this->theType == JSData::token::tokenFloat) {
    double floatRounded = static_cast<double>(static_cast<int>(this->theFloat));
    if (floatRounded - this->theFloat != 0.0) {
      return false;
    }
    if (whichInteger != nullptr) {
      *whichInteger = static_cast<int>(this->theFloat);
    }
    return true;
  }
  return false;
}

bool JSData::isEqualTo(const std::string& other) {
  if (this->theType != JSData::token::tokenString) {
    return false;
  }
  return this->theString == other;
}

bool JSData::isNullOrUndefined() {
  return
    this->theType == JSData::token::tokenNull ||
    this->theType == JSData::token::tokenUndefined;
}

bool JSData::isTrueRepresentationInJSON() {
  if (this->theType == JSData::token::tokenBool) {
    return this->theBoolean;
  }
  if (this->theType != JSData::token::tokenString) {
    return false;
  }
  return this->theString == "true";
}

bool JSData::isString(std::string* whichString) {
  if (this->theType != JSData::token::tokenString) {
    return false;
  }
  if (whichString != nullptr) {
    *whichString = this->theString;
  }
  return true;
}

bool JSData::isListOfStrings(List<std::string>* whichStrings) {
  if (this->theType != JSData::token::tokenArray) {
    return false;
  }
  for (int i = 0; i < this->theList.size; i ++) {
    if (this->theList[i].theType != JSData::token::tokenString) {
      return false;
    }
  }
  if (whichStrings != nullptr) {
    whichStrings->setSize(this->theList.size);
    for (int i = 0; i < this->theList.size; i ++) {
      (*whichStrings)[i] = this->theList[i].theString;
    }
  }
  return true;
}

bool JSData::IsValidElement() {
  return
  this->theType == JSData::token::tokenNull ||
  this->theType == JSData::token::tokenBool ||
  this->theType == JSData::token::tokenLargeInteger ||
  this->theType == JSData::token::tokenFloat ||
  this->theType == JSData::token::tokenString ||
  this->theType == JSData::token::tokenArray  ||
  this->theType == JSData::token::tokenObject;
}

bool JSData::TryToComputeType(std::stringstream* commentsOnFailure) {
  if (this->theType != JSData::token::tokenUnknown) {
    return true;
  }
  if (this->theString == "null") {
    this->reset();
    this->theType = JSData::token::tokenNull;
    return true;
  }
  if (this->theString == "true") {
    this->reset();
    this->theType = JSData::token::tokenBool;
    this->theBoolean = true;
    return true;
  }
  if (this->theString == "false") {
    this->reset();
    this->theType = JSData::token::tokenBool;
    this->theBoolean = false;
    return true;
  }
  if (this->theString.size() > 0) {
    if (this->theString[0] == '-' || MathRoutines::isADigit(this->theString[0])) {
      this->theInteger.FreeMemory();
      this->theType = JSData::token::tokenUndefined;
      Rational parser;
      if (parser.AssignStringFailureAllowed(this->theString)) {
        LargeInteger theInt;
        if (parser.isInteger(&theInt)) {
          this->theType = JSData::token::tokenLargeInteger;
          this->theInteger.getElement() = theInt;
          this->theString = "";
          return true;
        }
      }
      this->theFloat = std::stod(this->theString);
      this->theType = JSData::token::tokenFloat;
      this->theString = "";
      return true;
    }
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Failed to compute type of JSData element. ";
  }
  return false;
}

bool JSData::ConvertTwoByteHexToChar(
  char inputLeft, char inputRight, char& output, std::stringstream* commentsOnFailure
) {
  char leftHex = MathRoutines::ConvertHumanReadableHexToCharValue(inputLeft);
  if (leftHex < 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to interpret " << inputLeft << " as a hex digit. ";
    }
    return false;
  }
  char rightHex = MathRoutines::ConvertHumanReadableHexToCharValue(inputRight);
  if (rightHex < 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to interpret " << inputRight << " as a hex digit. ";
    }
    return false;
  }
  output = leftHex * 16 + rightHex;
  return true;
}

bool JSData::readstringConsumeFourHexAppendUnicode(
  std::string& output,
  unsigned int& currentIndex,
  const std::string& input,
  std::stringstream* commentsOnFailure
) {
  if (currentIndex + 3 >= input.size()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Unicode sequence \\u at position " << currentIndex << " exceeds json boundaries. ";
    }
    return false;
  }
  for (int i = 0; i < 2; i ++) {
    char left = input[currentIndex];
    currentIndex ++;
    char right = input[currentIndex];
    currentIndex ++;
    char next = 0;
    if (!JSData::ConvertTwoByteHexToChar(left, right, next, commentsOnFailure)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Failed to read hex characters at positions "
        << currentIndex - 2 << ", " << currentIndex - 1 << ". ";
      }
      return false;
    }
    output.push_back(next);
  }
  return true;
}

bool JSData::readstringConsumeNextCharacter(
  List<JSData>& readingStack,
  unsigned& currentIndex,
  const std::string& input,
  std::stringstream* commentsOnFailure
) {
  JSData& last = *readingStack.lastObject();
  char next = input[currentIndex];
  if (last.theType == JSData::token::tokenQuoteUnclosedStandard) {
    if (next == '\\') {
      last.theType = JSData::token::tokenQuoteUnclosedEscapeAtEnd;
      return true;
    }
    if (next == '"') {
      last.theType = JSData::token::tokenString;
      return true;
    }
    last.theString.push_back(next);
    return true;
  }
  if (last.theType == JSData::token::tokenQuoteUnclosedEscapeAtEnd) {
    bool result = true;
    switch (next) {
      case 'u':
        currentIndex ++;
        result = JSData::readstringConsumeFourHexAppendUnicode(last.theString, currentIndex, input, commentsOnFailure);
        break;
      case 'n':
        last.theString.push_back('\n');
        break;
      case '\\':
        last.theString.push_back('\\');
        break;
      case 't':
        last.theString.push_back('\t');
        break;
      case '"':
        last.theString.push_back('"');
        break;
      default:
        last.theString.push_back('\\');
        last.theString.push_back(next);
        break;
    }
    last.theType = JSData::token::tokenQuoteUnclosedStandard;
    return result;
  }
  if (next == ' ' || next == '\r' || next == '\n') {
    // empty space ignored
    return true;
  }
  JSData incoming;
  if (next == '"') {
    incoming.theType = JSData::token::tokenQuoteUnclosedStandard;
    readingStack.addOnTop(incoming);
    return true;
  }
  switch (next) {
    case '{':
      incoming.theType = JSData::token::tokenOpenBrace;
      break;
    case '}':
      incoming.theType = JSData::token::tokenCloseBrace;
      break;
    case '[':
      incoming.theType = JSData::token::tokenOpenBracket;
      break;
    case ']':
      incoming.theType = JSData::token::tokenCloseBracket;
      break;
    case ':':
      incoming.theType = JSData::token::tokenColon;
      break;
    case ',':
      incoming.theType = JSData::token::tokenComma;
      break;
    default:
      if (last.theType == JSData::token::tokenUnknown) {
        last.theString.push_back(next);
        return true;
      }
      incoming.theType = JSData::token::tokenUnknown;
      incoming.theString = next;
      break;
  }
  readingStack.addOnTop(incoming);
  return true;
}

bool JSData::TokenizePrependOneDummyElement(
  const std::string& input,
  List<JSData>& output,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("JSData::TokenizePrependOneDummyElement");
  output.setSize(0);
  output.setExpectedSize(static_cast<int>(input.size()));
  JSData emptyElt;
  output.addOnTop(emptyElt);
  for (unsigned i = 0; i < input.size(); i ++) {
    if (!this->readstringConsumeNextCharacter(output, i, input, commentsOnFailure)) {
      return false;
    }
  }
  for (int i = 1; i < output.size; i ++) {
    if (!output[i].TryToComputeType(commentsOnFailure)) {
      return false;
    }
  }
  return true;
}

bool JSData::MergeInMe(const JSData& input, std::stringstream* commentsOnFailure) {
  if (
    this->theType != JSData::token::tokenObject
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Can only merge in objects. ";
    }
    return false;
  }
  if (this->theType != input.theType) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Can only merge other objects.";
    }
    return false;
  }
  for (int i = 0; i < input.objects.size(); i ++) {
    const std::string& key = input.objects.theKeys[i];
    const JSData& value = input.objects.theValues[i];
    if (!this->objects.contains(key)) {
      (*this)[key] = value;
    } else {
      if (
        value.theType != JSData::token::tokenObject ||
        (*this)[key].theType != JSData::token::tokenObject
      ) {
        (*this)[key] = value;
      } else {
        if (!(*this)[key].MergeInMe(value, commentsOnFailure)) {
          return false;
        }
      }
    }
  }
  return true;
}

bool JSData::readstring(
  const std::string& json, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("JSData::readstring");
  this->reset();
  List<JSData> theTokenS;
  if (!JSData::TokenizePrependOneDummyElement(json, theTokenS, commentsOnFailure)) {
    return false;
  }
  if (theTokenS.size == 1) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "The empty string is not valid json.";
    }
    return false;
  }
  List<JSData> readingStack;
  JSData emptyElt;
  for (int i = 0; i < this->numEmptyTokensAtStart; i ++) {
    readingStack.addOnTop(emptyElt);
  }
  readingStack.addOnTop(theTokenS[1]);
  for (int i = 1;;) {
    int fourthToLastIndex = readingStack.size - 4; //<- used to avoid compiler warning
    JSData& last = readingStack[fourthToLastIndex + 3];
    JSData& secondToLast = readingStack[fourthToLastIndex + 2];
    JSData& thirdToLast  = readingStack[fourthToLastIndex + 1];
    JSData& fourthToLast = readingStack[fourthToLastIndex];
    //JSData& fifthToLast = theTokenS[i - 4];
    if (
      fourthToLast.theType == JSData::token::tokenOpenBrace && thirdToLast.theType == JSData::token::tokenString &&
      secondToLast.theType == JSData::token::tokenColon && last.IsValidElement()
    ) {
      fourthToLast.objects.setKeyValue(thirdToLast.theString, last);
      readingStack.setSize(readingStack.size - 3);
      continue;
    }
    if (secondToLast.theType == JSData::token::tokenOpenBracket && last.IsValidElement()) {
      secondToLast.theList.addOnTop(last);
      readingStack.removeLastObject();
      continue;
    }
    if (secondToLast.theType == JSData::token::tokenOpenBracket && last.theType == JSData::token::tokenComma) {
      readingStack.removeLastObject();
      continue;
    }
    if (
      (secondToLast.theType == JSData::token::tokenOpenBrace || secondToLast.theType == JSData::token::tokenOpenBracket) &&
      last.theType == JSData::token::tokenComma
    ) {
      readingStack.removeLastObject();
      continue;
    }
    if (secondToLast.theType == JSData::token::tokenOpenBrace && last.theType == JSData::token::tokenCloseBrace) {
      secondToLast.theType = JSData::token::tokenObject;
      readingStack.removeLastObject();
      continue;
    }
    if (secondToLast.theType == JSData::token::tokenOpenBracket && last.theType == JSData::token::tokenCloseBracket) {
      secondToLast.theType = JSData::token::tokenArray;
      readingStack.removeLastObject();
      continue;
    }
    i ++;
    if (i >= theTokenS.size) {
      break;
    }
    readingStack.addOnTop(theTokenS[i]);
  }
  if (readingStack.size != JSData::numEmptyTokensAtStart + 1) {
    if (commentsOnFailure != nullptr) {
      std::stringstream calculatorInput;
      calculatorInput
      << "TestJSON(\""
      << HtmlRoutines::ConvertStringEscapeNewLinesQuotesBackslashes(json)
      << "\")";
      *commentsOnFailure << "<hr>Failed to parse your JSON. Input:\n<br>\n "
      << "<a href=\""
      << global.DisplayNameExecutable
      << "?request=calculator&mainInput="
      << HtmlRoutines::convertStringToURLString(calculatorInput.str(), false)
      << "\">"
      << HtmlRoutines::ConvertStringToHtmlString(json, true)
      << "</a>"
      << "<br>Result:<br>\n ";
      for (int i = JSData::numEmptyTokensAtStart; i < readingStack.size; i ++) {
        *commentsOnFailure << i << ": "
        << readingStack[i].toString(&JSData::PrintOptions::HTML()) << "\n<br>\n";
      }
    }
    return false;
  }
  if (JSData::numEmptyTokensAtStart < readingStack.size) {
    *this = readingStack[JSData::numEmptyTokensAtStart];
  }
  bool result = this->IsValidElement();
  if (!result) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << this->toString(nullptr)
      << " does not appear to be valid json. ";
    }
  }
  return result;
}

JSData::PrintOptions::PrintOptions() {
  this->useHTML = false;
  this->useNewLine = false;
  this->indentation = 0;
  this->hexEncodeNonAsciiStrings = false;
}

const JSData::PrintOptions& JSData::PrintOptions::HexEncodeNonASCII() {
  static JSData::PrintOptions result;
  result.hexEncodeNonAsciiStrings = true;
  return result;
}

const JSData::PrintOptions& JSData::PrintOptions::NewLine() {
  static JSData::PrintOptions result;
  result.useNewLine = true;
  return result;
}

const JSData::PrintOptions& JSData::PrintOptions::HTML() {
  static JSData::PrintOptions result;
  result.useHTML = true;
  return result;
}

template <typename somestream>
somestream& JSData::IntoStream(
  somestream& out,
  const JSData::PrintOptions* optionsIncoming
) const {
  //MacroRegisterFunctionWithName("JSData::IntoStream");
  std::string whiteSpaceOuter = "";
  std::string whiteSpaceInner = "";
  JSData::PrintOptions options;
  if (optionsIncoming != nullptr) {
    options.useHTML = optionsIncoming->useHTML;
    options.useNewLine = optionsIncoming->useNewLine;
    options.indentation = optionsIncoming->indentation;
    options.hexEncodeNonAsciiStrings = optionsIncoming->hexEncodeNonAsciiStrings;
  }

  for (int i = 0; i < options.indentation; i ++) {
    if (options.useHTML) {
      whiteSpaceOuter += "&nbsp;";
    } else if (options.useNewLine) {
      whiteSpaceOuter += "  ";
    }
  }
  if (options.useHTML) {
    whiteSpaceInner = "&nbsp;";
  } else if (options.useNewLine){
    whiteSpaceInner = "  ";
  }
  std::string newLine = "";
  if (options.useHTML) {
    newLine = "\n<br>\n";
  } else if (options.useNewLine) {
    newLine = "\n";
  }
  options.indentation ++;
  switch (this->theType) {
    case JSData::token::tokenNull:
      out << "null";
      return out;
    case JSData::token::tokenFloat:
      out << this->theFloat;
      return out;
    case JSData::token::tokenLargeInteger:
      out << this->theInteger.GetElementConst().toString();
      return out;
    case JSData::token::tokenBool:
      if (this->theBoolean == true) {
        out << "true";
      } else {
        out << "false";
      }
      return out;
    case JSData::token::tokenString:
      if (!options.hexEncodeNonAsciiStrings) {
        out << '"' << StringRoutines::ConvertStringToJSONString(this->theString) << '"';
      } else {
        out << '"' << StringRoutines::ConvertStringToJSONString(
          StringRoutines::ConvertStringToHexIfNonReadable(this->theString, 0, false)
        ) << '"';
      }
      return out;
    case JSData::token::tokenArray:
      if (this->theList.size == 0) {
        out << "[]";
        return out;
      }
      out << "[" << newLine;
      for (int i = 0; i < this->theList.size; i ++) {
        out << whiteSpaceInner << whiteSpaceOuter;
        this->theList[i].IntoStream(out, &options);
        if (i != this->theList.size - 1) {
          out << "," << newLine;
        }
      }
      out << newLine << whiteSpaceOuter << ']';
      return out;
    case JSData::token::tokenObject:
      if (this->objects.size() == 0) {
        out << "{}";
        return out;
      }
      out << "{" << newLine;
      for (int i = 0; i < this->objects.size(); i ++) {
        out << whiteSpaceInner << whiteSpaceOuter;
        out << '"' << StringRoutines::ConvertStringToJSONString(this->objects.theKeys[i]) << '"';
        out << ':';
        this->objects.theValues[i].IntoStream(out, &options);
        if (i != this->objects.size() - 1) {
          out << "," << newLine;
        }
      }
      out << newLine << whiteSpaceOuter << '}';
      return out;
    case JSData::token::tokenOpenBrace:
      if (options.useHTML) {
        out << "<b>";
      }
      out << "{";
      if (options.useHTML) {
        out << "</b>";
      }
      return out;
    case JSData::token::tokenCloseBrace:
      if (options.useHTML) {
        out << "<b>";
      }
      out << "}";
      if (options.useHTML) {
        out << "</b>";
      }
      return out;
    case JSData::token::tokenOpenBracket:
      if (options.useHTML) {
        out << "<b>";
      }
      out << "[";
      if (options.useHTML) {
        out << "</b>";
      }
      return out;
    case JSData::token::tokenCloseBracket:
      if (options.useHTML) {
        out << "<b>";
      }
      out << "]";
      if (options.useHTML) {
        out << "</b>";
      }
      return out;
    case JSData::token::tokenColon:
      if (options.useHTML) {
        out << "<b>";
      }
      out << ":";
      if (options.useHTML) {
        out << "</b>";
      }
      return out;
    case JSData::token::tokenComma:
      if (options.useHTML) {
        out << "<b>";
      }
      out << ",";
      if (options.useHTML) {
        out << "</b>";
      }
      return out;
    case JSData::token::tokenUndefined:
      if (options.useHTML) {
        out << "<b>";
      }
      out << "null";
      if (options.useHTML) {
        out << "</b>";
      }
      return out;
    case JSData::token::tokenError:
      if (options.useHTML) {
        out << "<b>";
      }
      out << "error";
      if (options.useHTML) {
        out << "</b>";
      }
      return out;
    case JSData::token::tokenBackslash:
      if (options.useHTML) {
        out << "<b>";
      }
      out << "\\backslash";
      if (options.useHTML) {
        out << "</b>";
      }
      return out;
    case JSData::token::tokenQuoteUnclosedEscapeAtEnd:
      if (options.useHTML) {
        out << "<b>";
      }
      out << "\\QuoteUnclosedEscapeAtEnd";
      if (options.useHTML) {
        out << "</b>";
      }
      return out;
    case JSData::token::tokenQuoteUnclosedStandard:
      if (options.useHTML) {
        out << "<b>";
      }
      out << "\\tokenQuoteUnclosedStandard";
      if (options.useHTML) {
        out << "</b>";
      }
      return out;

    case JSData::token::tokenUnknown:
      if (options.useHTML) {
        out << "<b>";
      }
      out << "unknown";
      if (options.useHTML) {
        out << "</b>";
      }
      return out;
    default:
      break;
  }
  //supposed to be unreachable
  global.fatal << "Unhandled JSData case: " << this->theType << global.fatal;
  return out;
}

JSData::JSData(const JSData& other) {
  this->operator=(other);
}

JSData::JSData() {
  this->reset();
}

JSData::~JSData() {
}

void JSData::reset(char inputType) {
  this->theType = inputType;
  this->theBoolean = false;
  this->theFloat = 0;
  this->theString = "";
  this->theList.setSize(0);
  this->objects.clear();
  this->theInteger.FreeMemory();
  if (inputType == JSData::token::tokenLargeInteger) {
    this->theInteger.getElement().AssignInt(0);
  }
}

std::string JSData::toString(const JSData::PrintOptions* options) const {
  std::stringstream out;
  this->IntoStream(out, options);
  return out.str();
}

std::ostream& operator<<(std::ostream& out, const JSData& data) {
  return data.IntoStream(out, nullptr);
}

