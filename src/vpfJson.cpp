#include "vpfJson.h"
#include "vpfHeader2Math0_General.h"

#include <sys/stat.h>
#include <cassert>

static ProjectInformationInstance ProjectInfoVpfJSON(__FILE__, "Implementation JSON.");

void JSData::operator=(int other) {
  this->reset();
  this->theType = this->JSnumber;
  this->number = other;
}

void JSData::operator=(const bool other) {
  this->reset();
  this->theType = this->JSbool;
  this->theBoolean = other;
}

void JSData::operator=(const double other) {
  this->reset();
  this->theType = this->JSnumber;
  this->number = other;
}

void JSData::operator=(const char* other) {
  std::string otherString(other);
  this->operator=(otherString);
}

void JSData::operator=(const std::string& other) {
  this->reset();
  this->theType = this->JSstring;
  this->theString = other;
}

JSData& JSData::operator[](int i) {
  this->theType = this->JSarray;
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
  result.theType = JSData::JSUndefined;
  return result;
}

bool JSData::HasKey(const std::string& key) const {
  return this->GetKeyIndex(key) != - 1;
}

int JSData::GetKeyIndex(const std::string& key) const {
  return this->objects.GetIndex(key);
}

void JSData::SetKeyValue(const std::string& key, const JSData& value) {
  (*this)[key] = value;
}

JSData& JSData::operator[](const std::string& key) {
  this->theType = this->JSObject;
  return this->objects.GetValueCreate(key);
}

void JSData::operator=(const List<JSData>& other) {
  this->theType = this->JSarray;
  this->theList = other;
  this->objects.Clear();
}

void JSData::operator=(const Rational& other) {
  this->theType = this->JSnumber;
  this->number = other.GetDoubleValue();
}

void JSData::operator =(int64_t input) {
  this->theType = this->JSnumber;
  this->number = input;
}

bool JSData::isIntegerFittingInInt(int* whichInteger) {
  if (this->theType != JSData::JSnumber) {
    return false;
  }
  if (whichInteger != 0) {
    *whichInteger = (int) this->number;
  }
  return true;
}

bool JSData::isTrueRepresentationInJSON() {
  if (this->theType == JSData::JSbool) {
    return this->theBoolean;
  }
  if (this->theType != JSData::JSstring) {
    return false;
  }
  return this->theString == "true";
}

bool JSData::isListOfStrings(List<std::string>* whichStrings) {
  if (this->theType != this->JSarray) {
    return false;
  }
  for (int i = 0; i < this->theList.size; i ++) {
    if (this->theList[i].theType != this->JSstring) {
      return false;
    }
  }
  if (whichStrings != 0) {
    whichStrings->SetSize(this->theList.size);
    for (int i = 0; i < this->theList.size; i ++) {
      (*whichStrings)[i] = this->theList[i].theString;
    }
  }
  return true;
}

bool JSData::IsValidElement() {
  return
  this->theType == this->JSnull   ||
  this->theType == this->JSbool   ||
  this->theType == this->JSnumber ||
  this->theType == this->JSstring ||
  this->theType == this->JSarray  ||
  this->theType == this->JSObject;
}

void JSData::TryToComputeType() {
  if (this->theType != this->JSUndefined) {
    return;
  }
  if (this->theString == "") {
    return;
  }
  if (this->theString == "null") {
    this->reset();
    this->theType = this->JSnull;
    return;
  }
  if (this->theString == "true") {
    this->reset();
    this->theType = this->JSbool;
    this->theBoolean = true;
    return;
  }
  if (this->theString == "false") {
    this->reset();
    this->theType = this->JSbool;
    this->theBoolean = false;
    return;
  }
  if (this->theString.size() > 0) {
    if (this->theString[0] == '-' || MathRoutines::isADigit(this->theString[0])) {
      this->number = std::stod(this->theString);
      this->theString = "";
      this->theType = JSData::JSnumber;
      return;
    }
  }
  this->theType = JSData::JSstring;
}

bool JSData::Tokenize(const std::string& input, List<JSData>& output) {
  output.SetSize(0);
  output.SetExpectedSize(input.size());
  JSData currentElt;
  bool inQuotes = false;
  bool previousIsBackSlash = false;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (input[i] == '"') {
      if (currentElt.theType == currentElt.JSstring) {
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
        if (currentElt.theType != currentElt.JSUndefined) {
          output.AddOnTop(currentElt);
        }
        currentElt.reset();
        currentElt.theType = currentElt.JSstring;
        inQuotes = true;
        previousIsBackSlash = false;
      }
      continue;
    }
    if (inQuotes && currentElt.theType == currentElt.JSstring) {
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
    if (inQuotes && currentElt.theType == currentElt.JSstring) {
      currentElt.theString += input[i];
      continue;
    }
    if (input[i] == ' ' || input[i] == '\r' || input[i] == '\n') {
      currentElt.TryToComputeType();
      if (currentElt.theType != currentElt.JSUndefined) {
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
      if (currentElt.theType != JSData::JSUndefined) {
        output.AddOnTop(currentElt);
      }
      currentElt.reset();
      if (input[i] == '{') {
        currentElt.theType = currentElt.JSopenBrace;
      }
      if (input[i] == '[') {
        currentElt.theType = currentElt.JSopenBracket;
      }
      if (input[i] == '}') {
        currentElt.theType = currentElt.JScloseBrace;
      }
      if (input[i] == ']') {
        currentElt.theType = currentElt.JScloseBracket;
      }
      if (input[i] == ':') {
        currentElt.theType = currentElt.JScolon;
      }
      if (input[i] == ',') {
        currentElt.theType = currentElt.JScomma;
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
      if (theTokenS[i].theType == JSData::JSstring) {
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
      fourthToLast.theType == JSData::JSopenBrace && thirdToLast.theType == JSData::JSstring &&
      secondToLast.theType == JSData::JScolon && last.IsValidElement()
    ) {
      fourthToLast.objects.SetKeyValue(thirdToLast.theString, last);
      readingStack.SetSize(readingStack.size - 3);
      continue;
    }
    if (secondToLast.theType == JSData::JSopenBracket && last.IsValidElement()) {
      secondToLast.theList.AddOnTop(last);
      readingStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.theType == JSData::JSopenBracket && last.theType == JSData::JScomma) {
      readingStack.RemoveLastObject();
      continue;
    }
    if (
      (secondToLast.theType == JSData::JSopenBrace || secondToLast.theType == JSData::JSopenBracket) &&
      last.theType == JSData::JScomma
    ) {
      readingStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.theType == JSData::JSopenBrace && last.theType == JSData::JScloseBrace) {
      secondToLast.theType = JSData::JSObject;
      readingStack.RemoveLastObject();
      continue;
    }
    if (secondToLast.theType == JSData::JSopenBracket && last.theType == JSData::JScloseBracket) {
      secondToLast.theType = JSData::JSarray;
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
    if (commentsOnFailure != 0) {
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
      int x = (char) input[i];
      out << std::hex << ((x / 16) % 16) << (x % 16) << std::dec;
    }
  }
  return out.str();
}

template <typename somestream>
somestream& JSData::IntoStream(somestream& out, bool percentEncodeStrings, int indentation, bool useNewLine, bool useHTML) const {
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
    case JSnull:
      out << "null";
      return out;
    case JSnumber:
      out << this->number;
      return out;
    case JSbool:
      if (this->theBoolean == true) {
        out << "true";
      } else {
        out << "false";
      }
      return out;
    case JSstring:
      if (!percentEncodeStrings) {
        out << '"' << HtmlRoutines::ConvertStringEscapeNewLinesQuotesBackslashes(this->theString) << '"';
      } else {
        out << '"' << HtmlRoutines::ConvertStringToURLString(this->theString, false) << '"';
      }
      return out;
    case JSarray:
      if (this->theList.size == 0) {
        out << "[]";
        return out;
      }
      out << "[" << newLine;
      for (int i = 0; i < this->theList.size; i ++) {
        out << whiteSpaceInner << whiteSpaceOuter;
        this->theList[i].IntoStream(out, percentEncodeStrings, indentation, useNewLine, useHTML);
        if (i != this->theList.size - 1) {
          out << "," << newLine;
        }
      }
      out << newLine << whiteSpaceOuter << ']';
      return out;
    case JSObject:
      if (this->objects.size() == 0) {
        out << "{}";
        return out;
      }
      out << "{" << newLine;
      for (int i = 0; i < this->objects.size(); i ++) {
        out << whiteSpaceInner;
        if (!percentEncodeStrings) {
          out << '"' << HtmlRoutines::ConvertStringEscapeNewLinesQuotesBackslashes(this->objects.theKeys[i]) << '"';
        } else {
          out << '"' << JSData::EncodeKeyForMongo(this->objects.theKeys[i]) << '"';
        }
        out << ':';
        this->objects.theValues[i].IntoStream(out, percentEncodeStrings, indentation, useNewLine, useHTML);
        if (i != this->objects.size() - 1) {
          out << "," << newLine;
        }
      }
      out << newLine << '}';
      return out;
    case JSopenBrace:
      if (useHTML) {
        out << "<b>";
      }
      out << "{";
      if (useHTML) {
        out << "</b>";
      }
      return out;
    case JScloseBrace:
      if (useHTML) {
        out << "<b>";
      }
      out << "}";
      if (useHTML) {
        out << "</b>";
      }
      return out;
    case JSopenBracket:
      if (useHTML) {
        out << "<b>";
      }
      out << "[";
      if (useHTML) {
        out << "</b>";
      }
      return out;
    case JScloseBracket:
      if (useHTML) {
        out << "<b>";
      }
      out << "]";
      if (useHTML) {
        out << "</b>";
      }
      return out;
    case JScolon:
      if (useHTML) {
        out << "<b>";
      }
      out << ":";
      if (useHTML) {
        out << "</b>";
      }
      return out;
    case JScomma:
      if (useHTML) {
        out << "<b>";
      }
      out << ",";
      if (useHTML) {
        out << "</b>";
      }
      return out;
    case JSUndefined:
      if (useHTML) {
        out << "<b>";
      }
      out << "null";
      if (useHTML) {
        out << "</b>";
      }
      return out;
    case JSerror:
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

void JSData::reset(char inputType) {
  this->theType = inputType;
  this->theBoolean = false;
  this->number = 0;
  this->theString = "";
  this->theList.SetSize(0);
  this->objects.Clear();
}

std::string JSData::ToString(bool percentEncodeKeysIncludingDotsExcludingDollarSigns, bool useNewLine, bool useHTML) const {
  std::stringstream out;
  this->IntoStream(out, percentEncodeKeysIncludingDotsExcludingDollarSigns, 0, useNewLine, useHTML);
  return out.str();
}

std::ostream& operator<<(std::ostream& out, const JSData& data) {
  return data.IntoStream(out, false, false);
}

