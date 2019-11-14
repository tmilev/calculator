#include "json.h"
#include "math_general.h"

#include <sys/stat.h>
#include <cassert>

static ProjectInformationInstance ProjectInfoVpfJSON(__FILE__, "Implementation JSON.");

void JSData::operator=(const LargeInteger& other) {
  this->reset();
  this->theType = JSData::token::tokenLargeInteger;
  this->theInteger.GetElement() = other;
}

void JSData::operator=(int other) {
  this->reset();
  this->theType = JSData::token::tokenLargeInteger;
  this->theInteger.GetElement() = other;
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
    this->theList.SetSize(i + 1);
  }
  return this->theList[i];
}

JSData JSData::GetValue(const std::string& key) {
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

bool JSData::HasCompositeKeyOfType(const std::string& key, LargeIntegerUnsigned& output, std::stringstream* commentsOnFailure) const {
  JSData container;
  if (!this->HasCompositeKeyOfTokeN(key, &container, JSData::token::tokenLargeInteger, commentsOnFailure)) {
    return false;
  }
  if (container.theInteger.GetElement() < 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Error: integer is negative. ";
    }
    return false;
  }
  output = container.theInteger.GetElement().value;
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
      *commentsOnFailure << "Key: " << key << ", value: " << whichValue->ToString(false)
      << " is of type " << static_cast<int>(whichValue->theType)
      << " instead of the target one: " << static_cast<int>(targetType) << ". ";
    }
    return false;
  }
  return true;
}

bool JSData::HasCompositeKey(const std::string& inputKeys, JSData* whichValue, std::stringstream* commentsOnFailure) const {
  List<char> delimiters;
  delimiters.AddOnTop('.');
  delimiters.AddOnTop('[');
  delimiters.AddOnTop(']');
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
          << keys.ToStringCommaDelimited() << "] "
          << "is not an array, but is instead: " << currentData->ToString(false, true, true, true) << ". ";
        }
        return false;
      }
      if (currentData->theList.size <= theDigit) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "Key index: " << theDigit
          << " is too large for current value: " << currentData->ToString(false, true, true, true);
        }
        return false;
      }
      currentData = &currentData->theList[theDigit];
      continue;
    }
    if (currentData->theType != JSData::token::tokenObject) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Value preceding key: " << keys[i]
        << " is not of type object: " << currentData->ToString(false, true, true, true);
      }
      return false;
    }
    if (!currentData->HasKey(keys[i])) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Key: " << keys[i]
        << " not present in: " << currentData->ToString(false, true, true, true);
      }
      return false;
    }
    currentData = &currentData->objects.GetValueConstCrashIfNotPresent(keys[i]);
  }
  if (whichValue != nullptr) {
    *whichValue = *currentData;
  }
  return true;
}

int JSData::GetKeyIndex(const std::string& key) const {
  return this->objects.GetIndex(key);
}

void JSData::SetKeyValue(const std::string& key, const JSData& value) {
  (*this)[key] = value;
}

JSData& JSData::operator[](const std::string& key) {
  this->theType = JSData::token::tokenObject;
  return this->objects.GetValueCreate(key);
}

void JSData::operator=(const List<unsigned char>& other) {
  this->theType = JSData::token::tokenString;
  this->theString = other.ToStringConcatenate();
  this->theList.SetSize(0); ;
  this->objects.Clear();
}

void JSData::operator=(const List<JSData>& other) {
  this->theType = JSData::token::tokenArray;
  this->theList = other;
  this->objects.Clear();
}

void JSData::operator=(int64_t input) {
  this->theType = JSData::token::tokenLargeInteger;
  this->theInteger.GetElement().AssignInt64(input);
}

bool JSData::isIntegerFittingInInt(int* whichInteger) {
  if (this->theType == JSData::token::tokenLargeInteger) {
    return this->theInteger.GetElement().IsIntegerFittingInInt(whichInteger);
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

bool JSData::isTrueRepresentationInJSON() {
  if (this->theType == JSData::token::tokenBool) {
    return this->theBoolean;
  }
  if (this->theType != JSData::token::tokenString) {
    return false;
  }
  return this->theString == "true";
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
    whichStrings->SetSize(this->theList.size);
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

void JSData::TryToComputeType() {
  if (this->theType != JSData::token::tokenUndefined) {
    return;
  }
  if (this->theString == "") {
    return;
  }
  if (this->theString == "null") {
    this->reset();
    this->theType = JSData::token::tokenNull;
    return;
  }
  if (this->theString == "true") {
    this->reset();
    this->theType = JSData::token::tokenBool;
    this->theBoolean = true;
    return;
  }
  if (this->theString == "false") {
    this->reset();
    this->theType = JSData::token::tokenBool;
    this->theBoolean = false;
    return;
  }
  if (this->theString.size() > 0) {
    if (this->theString[0] == '-' || MathRoutines::isADigit(this->theString[0])) {
      this->theInteger.FreeMemory();
      this->theType = JSData::token::tokenUndefined;
      Rational parser;
      if (parser.AssignStringFailureAllowed(this->theString)) {
        LargeInteger theInt;
        if (parser.IsInteger(&theInt)) {
          this->theType = JSData::token::tokenLargeInteger;
          this->theInteger.GetElement() = theInt;
          this->theString = "";
          return;
        }
      }
      this->theFloat = std::stod(this->theString);
      this->theType = JSData::token::tokenFloat;
      this->theString = "";
      return;
    }
  }
  this->theType = JSData::token::tokenString;
}

bool JSData::Tokenize(const std::string& input, List<JSData>& output) {
  output.SetSize(0);
  output.SetExpectedSize(static_cast<int>(input.size()));
  JSData currentElt;
  bool inQuotes = false;
  bool previousIsBackSlash = false;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (input[i] == '"') {
      if (currentElt.theType == JSData::token::tokenString) {
        if (previousIsBackSlash) {
          currentElt.theString[currentElt.theString.size() - 1] = '\"';
          previousIsBackSlash = false;
        } else {
          output.AddOnTop(currentElt);
          currentElt.reset();
          inQuotes = false;
        }
      } else {
        currentElt.TryToComputeType();
        if (currentElt.theType != JSData::token::tokenUndefined) {
          output.AddOnTop(currentElt);
        }
        currentElt.reset();
        currentElt.theType = JSData::token::tokenString;
        inQuotes = true;
        previousIsBackSlash = false;
      }
      continue;
    }
    if (inQuotes && currentElt.theType == JSData::token::tokenString) {
      if (input[i] == '\\') {
        if (previousIsBackSlash) {
          previousIsBackSlash = false;
        } else {
          previousIsBackSlash = true;
          currentElt.theString += '\\';
        }
        continue;
      }
    }
    previousIsBackSlash = false;
    if (inQuotes && currentElt.theType == JSData::token::tokenString) {
      currentElt.theString += input[i];
      continue;
    }
    if (input[i] == ' ' || input[i] == '\r' || input[i] == '\n') {
      currentElt.TryToComputeType();
      if (currentElt.theType != JSData::token::tokenUndefined) {
        output.AddOnTop(currentElt);
        currentElt.reset();
      }
      continue;
    }
    if (
      input[i] == '{' || input[i] == '}' ||
      input[i] == '[' || input[i] == ']' ||
      input[i] == ':' || input[i] ==  ','
    ) {
      currentElt.TryToComputeType();
      if (currentElt.theType != JSData::token::tokenUndefined) {
        output.AddOnTop(currentElt);
      }
      currentElt.reset();
      if (input[i] == '{') {
        currentElt.theType = JSData::token::tokenOpenBrace;
      }
      if (input[i] == '[') {
        currentElt.theType = JSData::token::tokenOpenBracket;
      }
      if (input[i] == '}') {
        currentElt.theType = JSData::token::tokenCloseBrace;
      }
      if (input[i] == ']') {
        currentElt.theType = JSData::token::tokenCloseBracket;
      }
      if (input[i] == ':') {
        currentElt.theType = JSData::token::tokenColon;
      }
      if (input[i] == ',') {
        currentElt.theType = JSData::token::tokenComma;
      }
      output.AddOnTop(currentElt);
      currentElt.reset();
      continue;
    }
    currentElt.theString += input[i];
  }
  return true;
}

bool JSData::readstring(
  const std::string& json, bool stringsWerePercentEncoded, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("JSData::readstring");
  this->reset();
  List<JSData> theTokenS;
  JSData::Tokenize(json, theTokenS);
  if (theTokenS.size == 0) {
    return false;
  }
  if (stringsWerePercentEncoded) {
    for (int i = 0; i < theTokenS.size; i ++) {
      if (theTokenS[i].theType == JSData::token::tokenString) {
        theTokenS[i].theString = HtmlRoutines::ConvertURLStringToNormal(theTokenS[i].theString, false);
      }
    }
  }
  List<JSData> readingStack;
  JSData emptyElt;
  for (int i = 0; i < JSData::numEmptyTokensAtStart; i ++) {
    readingStack.AddOnTop(emptyElt);
  }
  readingStack.AddOnTop(theTokenS[0]);
  for (int i = 0;;) {
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
      fourthToLast.objects.SetKeyValue(thirdToLast.theString, last);
      readingStack.SetSize(readingStack.size - 3);
      continue;
    }
    if (secondToLast.theType == JSData::token::tokenOpenBracket && last.IsValidElement()) {
      secondToLast.theList.AddOnTop(last);
      readingStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.theType == JSData::token::tokenOpenBracket && last.theType == JSData::token::tokenComma) {
      readingStack.RemoveLastObject();
      continue;
    }
    if (
      (secondToLast.theType == JSData::token::tokenOpenBrace || secondToLast.theType == JSData::token::tokenOpenBracket) &&
      last.theType == JSData::token::tokenComma
    ) {
      readingStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.theType == JSData::token::tokenOpenBrace && last.theType == JSData::token::tokenCloseBrace) {
      secondToLast.theType = JSData::token::tokenObject;
      readingStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.theType == JSData::token::tokenOpenBracket && last.theType == JSData::token::tokenCloseBracket) {
      secondToLast.theType = JSData::token::tokenArray;
      readingStack.RemoveLastObject();
      continue;
    }
    i ++;
    if (i >= theTokenS.size) {
      break;
    }
    readingStack.AddOnTop(theTokenS[i]);
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
      << theGlobalVariables.DisplayNameExecutable
      << "?request=calculator&mainInput="
      << HtmlRoutines::ConvertStringToURLString(calculatorInput.str(), false)
      << "\">"
      << HtmlRoutines::ConvertStringToHtmlString(json, true)
      << "</a>"
      << "<br>Result:<br>\n ";
      for (int i = JSData::numEmptyTokensAtStart; i < readingStack.size; i ++) {
        *commentsOnFailure << i << ": " << readingStack[i].ToString(true) << "\n<br>\n";
      }
    }
    return false;
  }
  if (JSData::numEmptyTokensAtStart < readingStack.size) {
    *this = readingStack[JSData::numEmptyTokensAtStart];
  }
  return true;
}

std::string JSData::EncodeKeyForMongo(const std::string& input) {
  std::stringstream out;
  for (unsigned int i = 0; i < input.size(); i ++) {
    if (HtmlRoutines::IsRepresentedByItselfInURLs(input[i]) && input[i] != '.') {
      out << input[i];
    } else if (input[i] == '$') {
      out << input[i];
    } else {
      out << "%";
      int x = static_cast<signed>(input[i]);
      out << std::hex << ((x / 16) % 16) << (x % 16) << std::dec;
    }
  }
  return out.str();
}

template <typename somestream>
somestream& JSData::IntoStream(
  somestream& out, bool percentEncodeStrings, int indentation, bool useNewLine, bool useHTML, bool convertNonASCIIStringsToHex
) const {
  //MacroRegisterFunctionWithName("JSData::IntoStream");
  std::string whiteSpaceOuter = "";
  std::string whiteSpaceInner = "";
  for (int i = 0; i < indentation; i ++) {
    if (useHTML) {
      whiteSpaceOuter += "&nbsp;";
    } else if (useNewLine) {
      whiteSpaceOuter += "  ";
    }
  }
  if (useHTML) {
    whiteSpaceInner = "&nbsp;";
  } else if (useNewLine){
    whiteSpaceInner = "  ";
  }
  std::string newLine = "";
  if (useHTML) {
    newLine = "\n<br>\n";
  } else if (useNewLine) {
    newLine = "\n";
  }
  indentation ++;
  switch (this->theType) {
    case JSData::token::tokenNull:
      out << "null";
      return out;
    case JSData::token::tokenFloat:
      out << this->theFloat;
      return out;
    case JSData::token::tokenLargeInteger:
      out << this->theInteger.GetElementConst().ToString();
      return out;
    case JSData::token::tokenBool:
      if (this->theBoolean == true) {
        out << "true";
      } else {
        out << "false";
      }
      return out;
    case JSData::token::tokenString:
      if (convertNonASCIIStringsToHex) {
        out << '"' << StringRoutines::ConvertStringToHexIfNonReadable(this->theString, 0, useHTML) << '"';
      } else if (!percentEncodeStrings) {
        out << '"' << StringRoutines::ConvertStringToJSONString(this->theString) << '"';
      } else {
        out << '"' << HtmlRoutines::ConvertStringToURLString(this->theString, false) << '"';
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
        this->theList[i].IntoStream(
          out, percentEncodeStrings, indentation, useNewLine, useHTML, convertNonASCIIStringsToHex
        );
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
        if (!percentEncodeStrings) {
          out << '"' << StringRoutines::ConvertStringToJSONString(this->objects.theKeys[i]) << '"';
        } else {
          out << '"' << JSData::EncodeKeyForMongo(this->objects.theKeys[i]) << '"';
        }
        out << ':';
        this->objects.theValues[i].IntoStream(
          out, percentEncodeStrings, indentation, useNewLine, useHTML, convertNonASCIIStringsToHex
        );
        if (i != this->objects.size() - 1) {
          out << "," << newLine;
        }
      }
      out << newLine << whiteSpaceOuter << '}';
      return out;
    case JSData::token::tokenOpenBrace:
      if (useHTML) {
        out << "<b>";
      }
      out << "{";
      if (useHTML) {
        out << "</b>";
      }
      return out;
    case JSData::token::tokenCloseBrace:
      if (useHTML) {
        out << "<b>";
      }
      out << "}";
      if (useHTML) {
        out << "</b>";
      }
      return out;
    case JSData::token::tokenOpenBracket:
      if (useHTML) {
        out << "<b>";
      }
      out << "[";
      if (useHTML) {
        out << "</b>";
      }
      return out;
    case JSData::token::tokenCloseBracket:
      if (useHTML) {
        out << "<b>";
      }
      out << "]";
      if (useHTML) {
        out << "</b>";
      }
      return out;
    case JSData::token::tokenColon:
      if (useHTML) {
        out << "<b>";
      }
      out << ":";
      if (useHTML) {
        out << "</b>";
      }
      return out;
    case JSData::token::tokenComma:
      if (useHTML) {
        out << "<b>";
      }
      out << ",";
      if (useHTML) {
        out << "</b>";
      }
      return out;
    case JSData::token::tokenUndefined:
      if (useHTML) {
        out << "<b>";
      }
      out << "null";
      if (useHTML) {
        out << "</b>";
      }
      return out;
    case JSData::token::tokenError:
      if (useHTML) {
        out << "<b>";
      }
      out << "error";
      if (useHTML) {
        out << "</b>";
      }
      return out;
    default:
      break;
  }
  //supposed to be unreachable
  crash << "Unhandled JSData case. " << crash;
  return out;
}

JSData::JSData(const char* other) {
  this->reset();
  this->operator=(other);
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
  this->theList.SetSize(0);
  this->objects.Clear();
  this->theInteger.FreeMemory();
  if (inputType == JSData::token::tokenLargeInteger) {
    this->theInteger.GetElement().AssignInt(0);
  }
}

std::string JSData::ToString(
  bool percentEncodeKeysIncludingDotsExcludingDollarSigns, bool useNewLine, bool useHTML, bool convertNonASCIIStringsToHex
) const {
  std::stringstream out;
  this->IntoStream(
    out, percentEncodeKeysIncludingDotsExcludingDollarSigns, 0, useNewLine, useHTML, convertNonASCIIStringsToHex
  );
  return out.str();
}

std::ostream& operator<<(std::ostream& out, const JSData& data) {
  return data.IntoStream(out, false, false);
}

