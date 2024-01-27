#include "json.h"
#include "math_general.h"
#include "html_routines.h"
#include <sys/stat.h>
#include <cassert>

bool JSData::parse(
  const std::string& json,
  bool relaxedInput,
  std::stringstream* commentsOnFailure,
  bool useHTMLInComments
) {
  STACK_TRACE("JSData::parse");
  this->reset();
  List<JSONWithTokens> inputTokens;
  if (
    !JSONWithTokens::tokenize(
      json, inputTokens, relaxedInput, commentsOnFailure
    )
  ) {
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
  List<JSONWithTokens> readingStack;
  JSONWithTokens emptyElement;
  emptyElement.token = JSONWithTokens::tokenParsingStart;
  for (int i = 0; i < JSONWithTokens::numberOfEmptyTokensAtStart; i ++) {
    readingStack.addOnTop(emptyElement);
  }
  readingStack.addOnTop(inputTokens[1]);
  for (int i = 1;;) {
    int fourthToLastIndex = readingStack.size - 4;
    // <- used to avoid compiler warning
    JSONWithTokens& last = readingStack[fourthToLastIndex + 3];
    JSONWithTokens& secondToLast = readingStack[fourthToLastIndex + 2];
    JSONWithTokens& thirdToLast = readingStack[fourthToLastIndex + 1];
    JSONWithTokens& fourthToLast = readingStack[fourthToLastIndex];
    if (
      fourthToLast.token == JSONWithTokens::Token::tokenOpenBrace && (
        thirdToLast.token == JSONWithTokens::Token::tokenJSON &&
        thirdToLast.content().isString()
      ) &&
      secondToLast.token == JSONWithTokens::Token::tokenColon &&
      last.isJSON()
    ) {
      fourthToLast.content().objects.setKeyValue(
        thirdToLast.content().stringValue, last.content()
      );
      readingStack.setSize(readingStack.size - 3);
      continue;
    }
    if (
      relaxedInput &&
      fourthToLast.token == JSONWithTokens::Token::tokenOpenBrace &&
      thirdToLast.token == JSONWithTokens::Token::tokenUnknown &&
      secondToLast.token == JSONWithTokens::Token::tokenColon &&
      last.isJSON()
    ) {
      fourthToLast.content().objects.setKeyValue(
        thirdToLast.source, last.content()
      );
      readingStack.setSize(readingStack.size - 3);
      continue;
    }
    if (
      secondToLast.token == JSONWithTokens::Token::tokenOpenBracket &&
      last.isJSON()
    ) {
      secondToLast.content().listObjects.addOnTop(last.content());
      readingStack.removeLastObject();
      continue;
    }
    if (
      secondToLast.token == JSONWithTokens::Token::tokenOpenBracket &&
      last.token == JSONWithTokens::Token::tokenComma
    ) {
      readingStack.removeLastObject();
      continue;
    }
    if ((
        secondToLast.token == JSONWithTokens::Token::tokenOpenBrace ||
        secondToLast.token == JSONWithTokens::Token::tokenOpenBracket
      ) &&
      last.token == JSONWithTokens::Token::tokenComma
    ) {
      readingStack.removeLastObject();
      continue;
    }
    if (
      secondToLast.token == JSONWithTokens::Token::tokenOpenBrace &&
      last.token == JSONWithTokens::Token::tokenCloseBrace
    ) {
      secondToLast.token = JSONWithTokens::Token::tokenJSON;
      secondToLast.content().elementType = JSData::Type::tokenObject;
      readingStack.removeLastObject();
      continue;
    }
    if (
      secondToLast.token == JSONWithTokens::Token::tokenOpenBracket &&
      last.token == JSONWithTokens::Token::tokenCloseBracket
    ) {
      secondToLast.token = JSONWithTokens::Token::tokenJSON;
      secondToLast.content().elementType = JSData::Type::tokenArray;
      readingStack.removeLastObject();
      continue;
    }
    if (
      thirdToLast.token == JSONWithTokens::Token::tokenParsingStart &&
      secondToLast.token == JSONWithTokens::Token::tokenJSON &&
      last.token == JSONWithTokens::Token::tokenParsingEnd
    ) {
      readingStack.removeLastObject();
      continue;
    }
    i ++;
    if (i >= inputTokens.size) {
      break;
    }
    readingStack.addOnTop(inputTokens[i]);
  }
  if (readingStack.size != JSONWithTokens::numberOfEmptyTokensAtStart + 1) {
    if (commentsOnFailure != nullptr && useHTMLInComments) {
      std::stringstream calculatorInput;
      calculatorInput
      << "TestJSON(\""
      << HtmlRoutines::convertStringEscapeNewLinesQuotesBackslashes(json)
      << "\")";
      *commentsOnFailure
      << "<hr>Failed to parse your JSON. Input:\n<br>\n "
      << "<a href=\""
      << global.displayNameExecutable
      << "?request=calculator&mainInput="
      << HtmlRoutines::convertStringToURLString(
        calculatorInput.str(), false
      )
      << "\">"
      << HtmlRoutines::convertStringToHtmlString(json, true)
      << "</a>"
      << "<br>Result:<br>\n ";
      for (
        int i = JSONWithTokens::numberOfEmptyTokensAtStart; i < readingStack.
        size; i ++
      ) {
        *commentsOnFailure
        << i
        << ": "
        << readingStack[i].toString(true)
        << "\n<br>\n";
      }
    }
    if (commentsOnFailure != nullptr && !useHTMLInComments) {
      *commentsOnFailure
      << "Failed to parse your input. Stack state: "
      << JSONWithTokens::toStringReadingStack(readingStack);
    }
    return false;
  }
  JSONWithTokens& remaining =
  readingStack[JSONWithTokens::numberOfEmptyTokensAtStart];
  *this = remaining.content();
  bool result = remaining.token == JSONWithTokens::Token::tokenJSON;
  if (!result) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << this->toString(nullptr)
      << " does not appear to be valid json. ";
    }
  }
  return result;
}

bool JSData::convertTwoByteHexToUnsignedChar(
  unsigned char inputLeft,
  unsigned char inputRight,
  unsigned char& output,
  std::stringstream* commentsOnFailure
) {
  char leftHex =
  MathRoutines::convertHumanReadableHexToCharValue(
    static_cast<char>(inputLeft)
  );
  if (leftHex < 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to interpret value "
      << static_cast<unsigned int>(inputLeft)
      << " ["
      << inputLeft
      << "] as a hex digit. ";
    }
    return false;
  }
  char rightHex = MathRoutines::convertHumanReadableHexToCharValue(inputRight);
  if (rightHex < 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to interpret "
      << inputRight
      << " as a hex digit. ";
    }
    return false;
  }
  output = leftHex * 16 + rightHex;
  return true;
}

bool JSData::mergeInMe(
  const JSData& input, std::stringstream* commentsOnFailure
) {
  if (this->elementType != JSData::Type::tokenObject) {
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
        value.elementType != JSData::Type::tokenObject || (*this)[key].
        elementType !=
        JSData::Type::tokenObject
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

void JSData::parseNoFail(const std::string& json, bool relaxedInput) {
  STACK_TRACE("JSData::parseNoFail");
  std::stringstream commentsOnFailure;
  bool mustBeTrue = this->parse(json, relaxedInput, &commentsOnFailure);
  if (!mustBeTrue) {
    global.fatal
    << "Parsing failed on input: "
    << json
    << " which does not allow failure. "
    << commentsOnFailure.str()
    << global.fatal;
  }
  return;
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

std::stringstream& JSData::intoStream(
  std::stringstream& out, const JSData::PrintOptions* optionsIncoming
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
  } else if (options.useNewLine) {
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
  case JSData::Type::tokenNull:
    out << "null";
    return out;
  case JSData::Type::tokenFloat:
    out << this->floatValue;
    return out;
  case JSData::Type::tokenLargeInteger:
    out << this->integerValue.getElementConst().toString();
    return out;
  case JSData::Type::tokenBool:
    if (this->booleanValue == true) {
      out << "true";
    } else {
      out << "false";
    }
    return out;
  case JSData::Type::tokenString:
    out
    << '"'
    << StringRoutines::Conversions::stringToJSONStringEscaped(
      this->stringValue
    )
    << '"';
    return out;
  case JSData::Type::tokenArray:
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
  case JSData::Type::tokenObject:
    if (this->objects.size() == 0) {
      out << "{}";
      return out;
    }
    out << "{" << newLine;
    for (int i = 0; i < this->objects.size(); i ++) {
      out << whiteSpaceInner << whiteSpaceOuter;
      out
      << '"'
      << StringRoutines::Conversions::stringToJSONStringEscaped(
        this->objects.keys[i]
      )
      << '"';
      out << ':';
      this->objects.values[i].intoStream(out, &options);
      if (i != this->objects.size() - 1) {
        out << "," << newLine;
      }
    }
    out << newLine << whiteSpaceOuter << "}";
    return out;
  case JSData::Type::tokenUndefined:
    if (options.useHTML) {
      out << "<b>";
    }
    out << "null";
    if (options.useHTML) {
      out << "</b>";
    }
    return out;
  default:
    break;
  }
  // supposed to be unreachable
  global.fatal
  << "Unhandled JSData case: "
  << this->elementType
  << global.fatal;
  return out;
}

JSData::JSData(const JSData& other) {
  this->operator=(other);
}

JSData::JSData() {
  this->reset();
}

JSData::~JSData() {}

void JSData::reset(JSData::Type inputType) {
  this->elementType = inputType;
  this->booleanValue = false;
  this->floatValue = 0;
  this->stringValue = "";
  this->listObjects.setSize(0);
  this->objects.clear();
  this->integerValue.freeMemory();
  if (inputType == JSData::Type::tokenLargeInteger) {
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
  case JSData::Type::tokenLargeInteger:
    return this->integerValue.getElementConst().hashFunction();
  case JSData::Type::tokenArray:
    return this->listObjects.hashFunction();
  case JSData::Type::tokenObject:
    return this->objects.hashFunction();
  case JSData::Type::tokenString:
    return HashFunctions::hashFunction(this->stringValue);
  case JSData::Type::tokenBool:
    return HashFunctions::hashFunction(this->booleanValue);
  case JSData::Type::tokenFloat:
    return HashFunctions::hashFunction(this->floatValue);
  default:
    return 0;
  }
}

bool JSData::operator==(const JSData& other) const {
  return this->toString() == other.toString();
}

bool JSData::operator!=(const JSData& other) const {
  return !(*this == other);
}

std::ostream& operator<<(std::ostream& out, const JSData& data) {
  return out << data.toString();
}

JSONWithTokens::JSONWithTokens() {
  this->token = JSONWithTokens::Token::tokenUnknown;
}

bool JSONWithTokens::isJSON() const {
  return this->token == JSONWithTokens::Token::tokenJSON;
}

JSData& JSONWithTokens::content() {
  return this->container.getElement();
}

bool JSONWithTokens::tokenize(
  const std::string& input,
  List<JSONWithTokens>& output,
  bool relaxedInput,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("JSData::tokenizePrependOneDummyElement");
  output.setSize(0);
  int stringSize = static_cast<int>(input.size());
  // 1 million tokens maximum.
  int expectedSize = MathRoutines::minimum(stringSize, 1000000);
  output.setExpectedSize(expectedSize);
  JSONWithTokens startToken;
  startToken.token = JSONWithTokens::Token::tokenParsingStart;
  output.addOnTop(startToken);
  for (unsigned i = 0; i < input.size(); i ++) {
    if (
      !JSONWithTokens::readstringConsumeNextCharacter(
        output, i, input, commentsOnFailure
      )
    ) {
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
  JSONWithTokens endToken;
  endToken.token = JSONWithTokens::Token::tokenParsingEnd;
  output.addOnTop(endToken);
  return true;
}

std::string JSONWithTokens::toString() const {
  return this->toString(false);
}

std::string JSONWithTokens::toString(bool useHTML) const {
  if (this->isJSON()) {
    return this->container.getElementConst().toString();
  }
  return this->toStringToken(useHTML);
}

std::string JSONWithTokens::toStringToken(bool useHTML) const {
  std::stringstream out;
  switch (this->token) {
  case JSONWithTokens::Token::tokenOpenBrace:
    if (useHTML) {
      out << "<b>";
    }
    out << "{";
    if (useHTML) {
      out << "</b>";
    }
    break;
  case JSONWithTokens::Token::tokenCloseBrace:
    if (useHTML) {
      out << "<b>";
    }
    out << "}";
    if (useHTML) {
      out << "</b>";
    }
    break;
  case JSONWithTokens::Token::tokenOpenBracket:
    if (useHTML) {
      out << "<b>";
    }
    out << "[";
    if (useHTML) {
      out << "</b>";
    }
    break;
  case JSONWithTokens::Token::tokenCloseBracket:
    if (useHTML) {
      out << "<b>";
    }
    out << "]";
    if (useHTML) {
      out << "</b>";
    }
    break;
  case JSONWithTokens::Token::tokenColon:
    if (useHTML) {
      out << "<b>";
    }
    out << ":";
    if (useHTML) {
      out << "</b>";
    }
    break;
  case JSONWithTokens::Token::tokenComma:
    if (useHTML) {
      out << "<b>";
    }
    out << ",";
    if (useHTML) {
      out << "</b>";
    }
    break;
  case JSONWithTokens::Token::tokenError:
    if (useHTML) {
      out << "<b>";
    }
    out << "error";
    if (useHTML) {
      out << "</b>";
    }
    break;
  case JSONWithTokens::Token::tokenBackslash:
    if (useHTML) {
      out << "<b>";
    }
    out << "\\backslash";
    if (useHTML) {
      out << "</b>";
    }
    break;
  case JSONWithTokens::Token::tokenQuoteUnclosedEscapeAtEnd:
    if (useHTML) {
      out << "<b>";
    }
    out << "\\QuoteUnclosedEscapeAtEnd";
    if (useHTML) {
      out << "</b>";
    }
    break;
  case JSONWithTokens::Token::tokenQuoteUnclosedStandard:
    if (useHTML) {
      out << "<b>";
    }
    out << "\\tokenQuoteUnclosedStandard";
    if (useHTML) {
      out << "</b>";
    }
    break;
  case JSONWithTokens::Token::tokenUnknown:
    if (useHTML) {
      out << "<b>";
    }
    out << "unknown";
    out << "[" << this->source << "]";
    if (useHTML) {
      out << "</b>";
    }
    break;
  case JSONWithTokens::Token::tokenJSON:
    out << "{JSON}";
    break;
  case JSONWithTokens::Token::tokenParsingStart:
    out << "start";
    break;
  case JSONWithTokens::Token::tokenParsingEnd:
    out << "end";
    break;
  }
  return out.str();
}

bool JSONWithTokens::readstringConsumeNextCharacter(
  List<JSONWithTokens>& readingStack,
  unsigned& currentIndex,
  const std::string& input,
  std::stringstream* commentsOnFailure
) {
  JSONWithTokens& last = *readingStack.lastObject();
  char next = input[currentIndex];
  if (last.token == JSONWithTokens::Token::tokenQuoteUnclosedStandard) {
    if (next == '\\') {
      last.token = JSONWithTokens::Token::tokenQuoteUnclosedEscapeAtEnd;
      return true;
    }
    if (next == '"') {
      last.token = JSONWithTokens::Token::tokenJSON;
      last.content() = last.source;
      return true;
    }
    last.source.push_back(next);
    return true;
  }
  if (last.token == JSONWithTokens::Token::tokenQuoteUnclosedEscapeAtEnd) {
    bool result = true;
    switch (next) {
    case 'u':
      result =
      JSONWithTokens::readstringConsumeUnicodeFourHexAppendUtf8(
        last.source, currentIndex, input, commentsOnFailure
      );
      break;
    case 'n':
      last.source.push_back('\n');
      break;
    case '\\':
      last.source.push_back('\\');
      break;
    case 't':
      last.source.push_back('\t');
      break;
    case '"':
      last.source.push_back('"');
      break;
    default:
      last.source.push_back('\\');
      last.source.push_back(next);
      break;
    }
    last.token = JSONWithTokens::Token::tokenQuoteUnclosedStandard;
    return result;
  }
  if (next == ' ' || next == '\r' || next == '\n') {
    // empty space ignored
    return true;
  }
  JSONWithTokens incoming;
  if (next == '"') {
    incoming.token = JSONWithTokens::Token::tokenQuoteUnclosedStandard;
    readingStack.addOnTop(incoming);
    return true;
  }
  switch (next) {
  case '{':
    incoming.token = JSONWithTokens::Token::tokenOpenBrace;
    break;
  case '}':
    incoming.token = JSONWithTokens::Token::tokenCloseBrace;
    break;
  case '[':
    incoming.token = JSONWithTokens::Token::tokenOpenBracket;
    break;
  case ']':
    incoming.token = JSONWithTokens::Token::tokenCloseBracket;
    break;
  case ':':
    incoming.token = JSONWithTokens::Token::tokenColon;
    break;
  case ',':
    incoming.token = JSONWithTokens::Token::tokenComma;
    break;
  default:
    if (last.token == JSONWithTokens::Token::tokenUnknown) {
      last.source.push_back(next);
      return true;
    }
    incoming.token = JSONWithTokens::Token::tokenUnknown;
    incoming.source = next;
    break;
  }
  readingStack.addOnTop(incoming);
  return true;
}

bool JSONWithTokens::tryToComputeType(std::stringstream* commentsOnFailure) {
  if (this->token != JSONWithTokens::Token::tokenUnknown) {
    return true;
  }
  if (this->source == "null") {
    this->content().reset();
    this->content().elementType = JSData::Type::tokenNull;
    this->token = JSONWithTokens::tokenJSON;
    return true;
  }
  if (this->source == "true") {
    this->content().reset();
    this->content().elementType = JSData::Type::tokenBool;
    this->content().booleanValue = true;
    this->token = JSONWithTokens::tokenJSON;
    return true;
  }
  if (this->source == "false") {
    this->content().reset();
    this->content().elementType = JSData::Type::tokenBool;
    this->content().booleanValue = false;
    this->token = JSONWithTokens::tokenJSON;
    return true;
  }
  if (this->source.size() > 0) {
    if (
      this->source[0] == '-' || MathRoutines::isDigit(this->source[0])
    ) {
      this->content().integerValue.freeMemory();
      this->content().elementType = JSData::Type::tokenUndefined;
      Rational parser;
      if (parser.assignStringFailureAllowed(this->source)) {
        LargeInteger integerValue;
        if (parser.isInteger(&integerValue)) {
          this->content().elementType = JSData::Type::tokenLargeInteger;
          this->content().integerValue.getElement() = integerValue;
          this->content().stringValue = "";
          this->token = JSONWithTokens::tokenJSON;
          return true;
        }
      }
      this->content().floatValue = std::stod(this->source);
      this->content().elementType = JSData::Type::tokenFloat;
      this->token = JSONWithTokens::tokenJSON;
      return true;
    }
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "Failed to compute the type of the JSData element: "
    << this->toString()
    << ". ";
  }
  return false;
}

bool JSONWithTokens::readstringConsumeUnicodeFourHexAppendUtf8(
  std::string& output,
  unsigned int& currentIndex,
  const std::string& input,
  std::stringstream* commentsOnFailure
) {
  if ((currentIndex + 4) >= input.size()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Unicode sequence \\u at position "
      << currentIndex
      << " exceeds json boundaries. ";
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
    if (
      !JSData::convertTwoByteHexToUnsignedChar(
        left, right, next, commentsOnFailure
      )
    ) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Failed to read hex characters with value: "
        << static_cast<unsigned int>(left)
        << ", "
        << static_cast<unsigned int>(right)
        << ", ["
        << left
        << ", "
        << right
        << "] at positions "
        << leftIndex
        << ", "
        << rightIndex
        << " (currentIndex: "
        << currentIndex
        << "). ";
      }
      return false;
    }
    codePoint *= 256;
    codePoint += static_cast<uint32_t>(next);
  }
  std::string utf8Conversion =
  StringRoutines::Conversions::codePointToUtf8(codePoint);
  output += utf8Conversion;
  currentIndex += 4;
  return true;
}

std::string JSONWithTokens::toStringTokens(
  const List<JSONWithTokens>& stack
) {
  std::stringstream out;
  for (JSONWithTokens& element : stack) {
    out << element.toString();
  }
  return out.str();
}

std::string JSONWithTokens::toStringReadingStack(
  const List<JSONWithTokens>& stack
) {
  return
  toStringTokens(
    stack.sliceCopy(
      JSONWithTokens::numberOfEmptyTokensAtStart, stack.size
    )
  );
}

std::ostream& operator<<(std::ostream& out, const JSONWithTokens& data) {
  return out << data.toString();
}
