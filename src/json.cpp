#include "json.h"
#include "math_general.h"

#include <sys/stat.h>
#include <cassert>

void JSData::operator=(const LargeInteger& other) {
  this->reset();
  this->elementType = JSData::token::tokenLargeInteger;
  this->integerValue.getElement() = other;
}

void JSData::operator=(int other) {
  this->reset();
  this->elementType = JSData::token::tokenLargeInteger;
  this->integerValue.getElement() = other;
}

void JSData::operator=(const bool other) {
  this->reset();
  this->elementType = JSData::token::tokenBool;
  this->booleanValue = other;
}

void JSData::operator=(const double other) {
  this->reset();
  this->elementType = JSData::token::tokenFloat;
  this->floatValue = other;
}

void JSData::operator=(const char* other) {
  std::string otherString(other);
  this->operator=(otherString);
}

void JSData::operator=(const std::string& other) {
  this->reset();
  this->elementType = JSData::token::tokenString;
  this->stringValue = other;
}

void JSData::operator=(const JSData& other) {
  this->elementType = other.elementType;
  this->booleanValue = other.booleanValue;
  this->floatValue = other.floatValue;
  this->integerValue = other.integerValue;
  this->stringValue = other.stringValue;
  this->listObjects = other.listObjects;
  this->objects = other.objects;
}

JSData& JSData::operator[](int i) {
  this->elementType = JSData::token::tokenArray;
  if (this->listObjects.size < i + 1) {
    this->listObjects.setSizeAndExpectedSize(i + 1);
  }
  return this->listObjects[i];
}

JSData JSData::getValue(const std::string& key) {
  int index = this->getKeyIndex(key);
  if (index != - 1) {
    return this->objects.values[index];
  }
  JSData result;
  result.elementType = JSData::token::tokenUndefined;
  return result;
}

bool JSData::hasKey(const std::string& key) const {
  return this->getKeyIndex(key) != - 1;
}

bool JSData::hasCompositeKeyOfType(
  const std::string& key,
  LargeIntegerUnsigned& output,
  std::stringstream* commentsOnFailure
) const {
  JSData container;
  if (!this->hasCompositeKeyOfToken(
    key,
    &container,
    JSData::token::tokenLargeInteger,
    commentsOnFailure
  )) {
    return false;
  }
  if (container.integerValue.getElement() < 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Error: integer is negative. ";
    }
    return false;
  }
  output = container.integerValue.getElement().value;
  return true;
}

bool JSData::hasCompositeKeyOfType(
  const std::string& key, std::string& output, std::stringstream* commentsOnFailure
) const {
  JSData container;
  if (!this->hasCompositeKeyOfToken(key, &container, JSData::token::tokenString, commentsOnFailure)) {
    return false;
  }
  output = container.stringValue;
  return true;
}

bool JSData::hasCompositeKeyOfType(
  const std::string& key, List<unsigned char>& output, std::stringstream* commentsOnFailure
) const {
  JSData container;
  if (!this->hasCompositeKeyOfToken(key, &container, JSData::token::tokenString, commentsOnFailure)) {
    return false;
  }
  output = container.stringValue;
  return true;
}

bool JSData::hasCompositeKeyValueNull(const std::string &key, std::stringstream *commentsOnFailure) const {
  return this->hasCompositeKeyOfToken(key, nullptr, JSData::token::tokenNull, commentsOnFailure);
}

bool JSData::hasCompositeKeyOfToken(
  const std::string& key,
  JSData* whichValue,
  char targetType,
  std::stringstream* commentsOnFailure
) const {
  JSData container;
  if (whichValue == nullptr) {
    whichValue = &container;
  }
  if (!JSData::hasCompositeKey(key, whichValue, commentsOnFailure)) {
    return false;
  }
  if (whichValue->elementType != targetType) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Key: " << key << ", value: "
      << whichValue->toString(nullptr) << " is of type "
      << static_cast<int>(whichValue->elementType)
      << " instead of the target one: " << static_cast<int>(targetType)
      << ". ";
    }
    return false;
  }
  return true;
}

bool JSData::hasCompositeKey(const std::string& inputKeys, JSData* whichValue, std::stringstream* commentsOnFailure) const {
  List<char> delimiters;
  delimiters.addOnTop('.');
  delimiters.addOnTop('[');
  delimiters.addOnTop(']');
  List<std::string> keys;
  StringRoutines::stringSplitExcludeDelimiters(inputKeys, delimiters, keys);
  const JSData* currentData = this;
  if (keys.size == 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Could not extract any keys from your input: " << inputKeys;
    }
    return false;
  }
  for (int i = 0; i < keys.size; i ++) {
    // will not work for integer indices larger than 9
    int digit = - 1;
    if (MathRoutines::isADigit(keys[i], &digit)) {
      if (currentData->elementType != JSData::token::tokenArray) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "The sub-object located before the key: " << keys[i] << " [all keys: "
          << keys.toStringCommaDelimited() << "] "
          << "is not an array, but is instead: "
          << currentData->toString(&JSData::PrintOptions::HTML())
          << ". ";
        }
        return false;
      }
      if (currentData->listObjects.size <= digit) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "Key index: " << digit
          << " is too large for current value: " << currentData->toString(&JSData::PrintOptions::HTML());
        }
        return false;
      }
      currentData = &currentData->listObjects[digit];
      continue;
    }
    if (currentData->elementType != JSData::token::tokenObject) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Value preceding key: " << keys[i]
        << " is not of type object: " << currentData->toString(&JSData::PrintOptions::HTML());
      }
      return false;
    }
    if (!currentData->hasKey(keys[i])) {
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

int JSData::getKeyIndex(const std::string& key) const {
  return this->objects.getIndex(key);
}

void JSData::setKeyValue(const std::string& key, const JSData& value) {
  (*this)[key] = value;
}

JSData& JSData::operator[](const std::string& key) {
  this->elementType = JSData::token::tokenObject;
  return this->objects.getValueCreateEmpty(key);
}

void JSData::operator=(const List<unsigned char>& other) {
  this->elementType = JSData::token::tokenString;
  this->stringValue = other.toStringConcatenate();
  this->listObjects.setSize(0); ;
  this->objects.clear();
}

void JSData::operator=(const List<JSData>& other) {
  this->elementType = JSData::token::tokenArray;
  this->listObjects.setSize(other.size);
  for (int i = 0; i < other.size; i ++) {
    this->listObjects[i] = other[i];
  }
  this->objects.clear();
}

void JSData::operator=(int64_t input) {
  this->elementType = JSData::token::tokenLargeInteger;
  this->integerValue.getElement().assignInt64(input);
}

bool JSData::isIntegerFittingInInt(int* whichInteger) {
  if (this->elementType == JSData::token::tokenLargeInteger) {
    return this->integerValue.getElement().isIntegerFittingInInt(whichInteger);
  }
  if (this->elementType == JSData::token::tokenFloat) {
    double floatRounded = static_cast<double>(static_cast<int>(this->floatValue));
    if (floatRounded - this->floatValue != 0.0) {
      return false;
    }
    if (whichInteger != nullptr) {
      *whichInteger = static_cast<int>(this->floatValue);
    }
    return true;
  }
  return false;
}

bool JSData::isEqualTo(const std::string& other) {
  if (this->elementType != JSData::token::tokenString) {
    return false;
  }
  return this->stringValue == other;
}

bool JSData::isNullOrUndefined() {
  return
    this->elementType == JSData::token::tokenNull ||
    this->elementType == JSData::token::tokenUndefined;
}

bool JSData::isTrueRepresentationInJSON() {
  if (this->elementType == JSData::token::tokenBool) {
    return this->booleanValue;
  }
  if (this->elementType != JSData::token::tokenString) {
    return false;
  }
  return this->stringValue == "true";
}

JSData JSData::makeEmptyArray() {
  JSData result;
  result.elementType = JSData::token::tokenArray;
  return result;
}

bool JSData::isString(std::string* whichString) {
  if (this->elementType != JSData::token::tokenString) {
    return false;
  }
  if (whichString != nullptr) {
    *whichString = this->stringValue;
  }
  return true;
}

bool JSData::isListOfStrings(List<std::string>* whichStrings) {
  if (this->elementType != JSData::token::tokenArray) {
    return false;
  }
  for (int i = 0; i < this->listObjects.size; i ++) {
    if (this->listObjects[i].elementType != JSData::token::tokenString) {
      return false;
    }
  }
  if (whichStrings != nullptr) {
    whichStrings->setSize(this->listObjects.size);
    for (int i = 0; i < this->listObjects.size; i ++) {
      (*whichStrings)[i] = this->listObjects[i].stringValue;
    }
  }
  return true;
}

bool JSData::isValidElement() {
  return
  this->elementType == JSData::token::tokenNull ||
  this->elementType == JSData::token::tokenBool ||
  this->elementType == JSData::token::tokenLargeInteger ||
  this->elementType == JSData::token::tokenFloat ||
  this->elementType == JSData::token::tokenString ||
  this->elementType == JSData::token::tokenArray  ||
  this->elementType == JSData::token::tokenObject;
}

bool JSData::tryToComputeType(std::stringstream* commentsOnFailure) {
  if (this->elementType != JSData::token::tokenUnknown) {
    return true;
  }
  if (this->stringValue == "null") {
    this->reset();
    this->elementType = JSData::token::tokenNull;
    return true;
  }
  if (this->stringValue == "true") {
    this->reset();
    this->elementType = JSData::token::tokenBool;
    this->booleanValue = true;
    return true;
  }
  if (this->stringValue == "false") {
    this->reset();
    this->elementType = JSData::token::tokenBool;
    this->booleanValue = false;
    return true;
  }
  if (this->stringValue.size() > 0) {
    if (this->stringValue[0] == '-' || MathRoutines::isADigit(this->stringValue[0])) {
      this->integerValue.freeMemory();
      this->elementType = JSData::token::tokenUndefined;
      Rational parser;
      if (parser.assignStringFailureAllowed(this->stringValue)) {
        LargeInteger integerValue;
        if (parser.isInteger(&integerValue)) {
          this->elementType = JSData::token::tokenLargeInteger;
          this->integerValue.getElement() = integerValue;
          this->stringValue = "";
          return true;
        }
      }
      this->floatValue = std::stod(this->stringValue);
      this->elementType = JSData::token::tokenFloat;
      this->stringValue = "";
      return true;
    }
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "Failed to compute the type of the JSData element: "
    << this->toString() << ". ";
  }
  return false;
}

bool JSData::convertTwoByteHexToUnsignedChar(
  unsigned char inputLeft,
  unsigned char inputRight,
  unsigned char& output,
  std::stringstream* commentsOnFailure
) {
  char leftHex = MathRoutines::convertHumanReadableHexToCharValue(
    static_cast<char>(inputLeft)
  );
  if (leftHex < 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to interpret value " << static_cast<unsigned int>(inputLeft)
      << " [" << inputLeft << "] as a hex digit. ";
    }
    return false;
  }
  char rightHex = MathRoutines::convertHumanReadableHexToCharValue(inputRight);
  if (rightHex < 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to interpret " << inputRight << " as a hex digit. ";
    }
    return false;
  }
  output = leftHex * 16 + rightHex;
  return true;
}

bool JSData::readstringConsumeUnicodeFourHexAppendUtf8(
  std::string& output,
  unsigned int& currentIndex,
  const std::string& input,
  std::stringstream* commentsOnFailure
) {
  if ((currentIndex + 4) >= input.size()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Unicode sequence \\u at position "
      << currentIndex << " exceeds json boundaries. ";
    }
    return false;
  }
  uint32_t codePoint = 0;
  for (int i = 0; i < 2; i ++) {
    int leftIndex = currentIndex + 2 * i + 1;
    int rightIndex = currentIndex + 2 * i + 2;
    unsigned char left = input[leftIndex];
    unsigned char right = input[rightIndex];
    unsigned char next = 0;
    if (!JSData::convertTwoByteHexToUnsignedChar(left, right, next, commentsOnFailure)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Failed to read hex characters with value: "
        << static_cast<unsigned int>(left) << ", " << static_cast<unsigned int>(right)
        << ", ["
        << left << ", " << right << "] at positions "
        << leftIndex << ", " << rightIndex << " (currentIndex: "
        << currentIndex << "). ";
      }
      return false;
    }
    codePoint *= 256;
    codePoint += static_cast<uint32_t>(next);
  }
  std::string utf8Conversion = StringRoutines::Conversions::codePointToUtf8(codePoint);
  output += utf8Conversion;
  currentIndex += 4;
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
  if (last.elementType == JSData::token::tokenQuoteUnclosedStandard) {
    if (next == '\\') {
      last.elementType = JSData::token::tokenQuoteUnclosedEscapeAtEnd;
      return true;
    }
    if (next == '"') {
      last.elementType = JSData::token::tokenString;
      return true;
    }
    last.stringValue.push_back(next);
    return true;
  }
  if (last.elementType == JSData::token::tokenQuoteUnclosedEscapeAtEnd) {
    bool result = true;
    switch (next) {
      case 'u':
        result = JSData::readstringConsumeUnicodeFourHexAppendUtf8(
          last.stringValue, currentIndex, input, commentsOnFailure
        );
        break;
      case 'n':
        last.stringValue.push_back('\n');
        break;
      case '\\':
        last.stringValue.push_back('\\');
        break;
      case 't':
        last.stringValue.push_back('\t');
        break;
      case '"':
        last.stringValue.push_back('"');
        break;
      default:
        last.stringValue.push_back('\\');
        last.stringValue.push_back(next);
        break;
    }
    last.elementType = JSData::token::tokenQuoteUnclosedStandard;
    return result;
  }
  if (next == ' ' || next == '\r' || next == '\n') {
    // empty space ignored
    return true;
  }
  JSData incoming;
  if (next == '"') {
    incoming.elementType = JSData::token::tokenQuoteUnclosedStandard;
    readingStack.addOnTop(incoming);
    return true;
  }
  switch (next) {
    case '{':
      incoming.elementType = JSData::token::tokenOpenBrace;
      break;
    case '}':
      incoming.elementType = JSData::token::tokenCloseBrace;
      break;
    case '[':
      incoming.elementType = JSData::token::tokenOpenBracket;
      break;
    case ']':
      incoming.elementType = JSData::token::tokenCloseBracket;
      break;
    case ':':
      incoming.elementType = JSData::token::tokenColon;
      break;
    case ',':
      incoming.elementType = JSData::token::tokenComma;
      break;
    default:
      if (last.elementType == JSData::token::tokenUnknown) {
        last.stringValue.push_back(next);
        return true;
      }
      incoming.elementType = JSData::token::tokenUnknown;
      incoming.stringValue = next;
      break;
  }
  readingStack.addOnTop(incoming);
  return true;
}

bool JSData::tokenizePrependOneDummyElement(
  const std::string& input,
  List<JSData>& output,
  bool relaxedInput,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("JSData::tokenizePrependOneDummyElement");
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
    if (!output[i].tryToComputeType(commentsOnFailure)) {
      if (!relaxedInput) {
        return false;
      }
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Will attempt to auto-correct your json input. ";
      }
      // Do not generate further comments.
      commentsOnFailure = nullptr;
    }
  }
  return true;
}

bool JSData::mergeInMe(const JSData& input, std::stringstream* commentsOnFailure) {
  if (
    this->elementType != JSData::token::tokenObject
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Can only merge in objects. ";
    }
    return false;
  }
  if (this->elementType != input.elementType) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Can only merge other objects.";
    }
    return false;
  }
  for (int i = 0; i < input.objects.size(); i ++) {
    const std::string& key = input.objects.keys[i];
    const JSData& value = input.objects.values[i];
    if (!this->objects.contains(key)) {
      (*this)[key] = value;
    } else {
      if (
        value.elementType != JSData::token::tokenObject ||
        (*this)[key].elementType != JSData::token::tokenObject
      ) {
        (*this)[key] = value;
      } else {
        if (!(*this)[key].mergeInMe(value, commentsOnFailure)) {
          return false;
        }
      }
    }
  }
  return true;
}

bool JSData::parse(
  const std::string& json,
  bool relaxedInput,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("JSData::readstring");
  this->reset();
  List<JSData> inputTokens;
  if (!JSData::tokenizePrependOneDummyElement(
    json, inputTokens, relaxedInput, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to tokenize your input: " << json << ".";
    }
    if (!relaxedInput) {
      return false;
    }
  }
  if (inputTokens.size == 1) {
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
  readingStack.addOnTop(inputTokens[1]);
  for (int i = 1;;) {
    int fourthToLastIndex = readingStack.size - 4; //<- used to avoid compiler warning
    JSData& last = readingStack[fourthToLastIndex + 3];
    JSData& secondToLast = readingStack[fourthToLastIndex + 2];
    JSData& thirdToLast  = readingStack[fourthToLastIndex + 1];
    JSData& fourthToLast = readingStack[fourthToLastIndex];
    //JSData& fifthToLast = inputTokens[i - 4];
    if (
      fourthToLast.elementType == JSData::token::tokenOpenBrace &&
      thirdToLast.elementType == JSData::token::tokenString &&
      secondToLast.elementType == JSData::token::tokenColon &&
      last.isValidElement()
    ) {
      fourthToLast.objects.setKeyValue(thirdToLast.stringValue, last);
      readingStack.setSize(readingStack.size - 3);
      continue;
    }
    if (
      relaxedInput &&
      fourthToLast.elementType == JSData::token::tokenOpenBrace &&
      thirdToLast.elementType == JSData::token::tokenUnknown &&
      secondToLast.elementType == JSData::token::tokenColon &&
      last.isValidElement()
    ) {
      fourthToLast.objects.setKeyValue(thirdToLast.stringValue, last);
      readingStack.setSize(readingStack.size - 3);
      continue;
    }
    if (
      secondToLast.elementType == JSData::token::tokenOpenBracket &&
      last.isValidElement()
    ) {
      secondToLast.listObjects.addOnTop(last);
      readingStack.removeLastObject();
      continue;
    }
    if (secondToLast.elementType == JSData::token::tokenOpenBracket && last.elementType == JSData::token::tokenComma) {
      readingStack.removeLastObject();
      continue;
    }
    if (
      (secondToLast.elementType == JSData::token::tokenOpenBrace || secondToLast.elementType == JSData::token::tokenOpenBracket) &&
      last.elementType == JSData::token::tokenComma
    ) {
      readingStack.removeLastObject();
      continue;
    }
    if (
      secondToLast.elementType == JSData::token::tokenOpenBrace &&
      last.elementType == JSData::token::tokenCloseBrace
    ) {
      secondToLast.elementType = JSData::token::tokenObject;
      readingStack.removeLastObject();
      continue;
    }
    if (secondToLast.elementType == JSData::token::tokenOpenBracket && last.elementType == JSData::token::tokenCloseBracket) {
      secondToLast.elementType = JSData::token::tokenArray;
      readingStack.removeLastObject();
      continue;
    }
    i ++;
    if (i >= inputTokens.size) {
      break;
    }
    readingStack.addOnTop(inputTokens[i]);
  }
  if (readingStack.size != JSData::numEmptyTokensAtStart + 1) {
    if (commentsOnFailure != nullptr) {
      std::stringstream calculatorInput;
      calculatorInput
      << "TestJSON(\""
      << HtmlRoutines::convertStringEscapeNewLinesQuotesBackslashes(json)
      << "\")";
      *commentsOnFailure << "<hr>Failed to parse your JSON. Input:\n<br>\n "
      << "<a href=\""
      << global.displayNameExecutable
      << "?request=calculator&mainInput="
      << HtmlRoutines::convertStringToURLString(calculatorInput.str(), false)
      << "\">"
      << HtmlRoutines::convertStringToHtmlString(json, true)
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
  bool result = this->isValidElement();
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
}

const JSData::PrintOptions& JSData::PrintOptions::newLine() {
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
somestream& JSData::intoStream(
  somestream& out,
  const JSData::PrintOptions* optionsIncoming
) const {
  std::string whiteSpaceOuter = "";
  std::string whiteSpaceInner = "";
  JSData::PrintOptions options;
  if (optionsIncoming != nullptr) {
    options.useHTML = optionsIncoming->useHTML;
    options.useNewLine = optionsIncoming->useNewLine;
    options.indentation = optionsIncoming->indentation;
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
  switch (this->elementType) {
    case JSData::token::tokenNull:
      out << "null";
      return out;
    case JSData::token::tokenFloat:
      out << this->floatValue;
      return out;
    case JSData::token::tokenLargeInteger:
      out << this->integerValue.getElementConst().toString();
      return out;
    case JSData::token::tokenBool:
      if (this->booleanValue == true) {
        out << "true";
      } else {
        out << "false";
      }
      return out;
    case JSData::token::tokenString:
      out << '"' << StringRoutines::Conversions::stringToJSONStringEscaped(this->stringValue) << '"';
      return out;
    case JSData::token::tokenArray:
      if (this->listObjects.size == 0) {
        out << "[]";
        return out;
      }
      out << "[" << newLine;
      for (int i = 0; i < this->listObjects.size; i ++) {
        out << whiteSpaceInner << whiteSpaceOuter;
        this->listObjects[i].intoStream(out, &options);
        if (i != this->listObjects.size - 1) {
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
        out << '"' << StringRoutines::Conversions::stringToJSONStringEscaped(this->objects.keys[i]) << '"';
        out << ':';
        this->objects.values[i].intoStream(out, &options);
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
  global.fatal << "Unhandled JSData case: " << this->elementType << global.fatal;
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
  this->elementType = inputType;
  this->booleanValue = false;
  this->floatValue = 0;
  this->stringValue = "";
  this->listObjects.setSize(0);
  this->objects.clear();
  this->integerValue.freeMemory();
  if (inputType == JSData::token::tokenLargeInteger) {
    this->integerValue.getElement().assignInteger(0);
  }
}

std::string JSData::toString(const JSData::PrintOptions* options) const {
  std::stringstream out;
  this->intoStream(out, options);
  return out.str();
}

unsigned int JSData::hashFunction() const {
  switch (this->elementType) {
  case JSData::token::tokenLargeInteger:
    return this->integerValue.getElementConst().hashFunction();
  case JSData::token::tokenArray:
    return this->listObjects.hashFunction();
  case JSData::token::tokenObject:
    return this->objects.hashFunction();
  case JSData::token::tokenString:
    return HashFunctions::hashFunction(this->stringValue);
  case JSData::token::tokenBool:
    return HashFunctions::hashFunction(this->booleanValue);
  case JSData::token::tokenFloat:
    return HashFunctions::hashFunction(this->floatValue);
  default:
    return 0;
  }
}

bool JSData::operator==(const JSData& other) const {
  return this->toString() == other.toString();
}

std::ostream& operator<<(std::ostream& out, const JSData& data) {
  return data.intoStream(out, nullptr);
}

