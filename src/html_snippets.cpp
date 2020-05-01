// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "general_list_references.h"
#include "web_api.h"
#include "general_logging_global_variables.h"
#include "general_file_operations_encodings.h"
#include <iomanip>
#include "string_constants.h"

MapList<std::string, std::string, MathRoutines::HashString>& HtmlRoutines::preLoadedFiles() {
  static MapList<std::string, std::string, MathRoutines::HashString> result;
  return result;
}

std::string HtmlRoutines::gitRepository = "https://github.com/tmilev/calculator";

std::string HtmlRoutines::getJavascriptVariable(const std::string& theVar) {
  std::stringstream sanitizer;
  for (unsigned i = 0; i < theVar.size(); i ++) {
    if (MathRoutines::isALatinLetter(theVar[i])) {
      sanitizer << theVar[i];
      continue;
    }
    if (i == 0) {
      sanitizer << "_";
    }
    sanitizer << static_cast<int>(theVar[i]);
  }
  return sanitizer.str();
}

std::string HtmlRoutines::getHtmlLinkToGithubRepository(const std::string& displayString) {
  std::stringstream out;
  out << "<a href=\"" << HtmlRoutines::gitRepository << "\">" << displayString << "</a>";
  return out.str();
}

std::string HtmlRoutines::getHtmlLinkFromProjectFileName(
  const std::string& fileName, const std::string& fileDesc, int line
) {
  std::stringstream out;
  out << " <a href=\"" << HtmlRoutines::gitRepository << "/blob/master/src/"
  << FileOperations::GetFileNameFromFileNameWithPath(fileName);
  if (line > 0) {
    out << "#L" << line;
  }
  out << "\">" << FileOperations::GetFileNameFromFileNameWithPath(fileName);
  if (fileDesc != "") {
    out << " (" << fileDesc << ")";
  }
  out << "</a>\n";
  return out.str();
}

void HtmlRoutines::loadStrings() {
  if (HtmlRoutines::preLoadedFiles().size() > 0) {
    return;
  }
  HtmlRoutines::getMathQuillStyleSheeTWithTags();
  HtmlRoutines::getJavascriptAceEditorScriptWithTags();
  HtmlRoutines::getJavascriptMathQuillDefaultWithTags();
  HtmlRoutines::getJavascriptMathQuillMatrixSupporTWithTags();
  HtmlRoutines::getJavascriptBrowserifier();
}

const std::string& HtmlRoutines::getJavascriptAceEditorScriptWithTags() {
  if (HtmlRoutines::preLoadedFiles().contains("AceEditor")) {
    return HtmlRoutines::preLoadedFiles().GetValueCreateNoInit("AceEditor");
  }
  std::stringstream out;
  out << "<script type =\"text/javascript\" src =\""
  << FileOperations::GetVirtualNameWithHash("/html-common/ace/src-min/ace.js")
  << "\" charset =\"utf-8\"></script>";
  out << HtmlRoutines::getJavascriptAddScriptTags("/html-common/ace-editor-settings.js");
  HtmlRoutines::preLoadedFiles().setKeyValue("AceEditor", out.str());
  return HtmlRoutines::preLoadedFiles().GetValueCreateNoInit("AceEditor");
}

const std::string& HtmlRoutines::getFile(
  const std::string& fileNameVirtual,
  const std::string& additionalBeginTag,
  const std::string& additionalEndTag
) {
  MacroRegisterFunctionWithName("HtmlRoutines::getFile");
  std::string theID = fileNameVirtual + additionalBeginTag + additionalEndTag;
  if (global.flagCachingInternalFilesOn) {
    if (HtmlRoutines::preLoadedFiles().GetValueCreate(theID) != "") {
      return HtmlRoutines::preLoadedFiles().GetValueCreate(theID);
    }
  }
  std::stringstream out, commentsOnFailure;
  std::string fileReader;
  if (FileOperations::LoadFileToStringVirtual(fileNameVirtual, fileReader, false, &commentsOnFailure)) {
    out << additionalBeginTag << fileReader << additionalEndTag;
  } else {
    global << Logger::red << "File: "
    << fileNameVirtual << " not found. " << commentsOnFailure.str() << Logger::endL;
    out << "<b style =\"color:red\">Failed to load file: [" << fileNameVirtual
    << "]. Comments: " << commentsOnFailure.str() << "</b>";
  }
  HtmlRoutines::preLoadedFiles().setKeyValue(theID, out.str());
  return HtmlRoutines::preLoadedFiles().GetValueCreateNoInit(theID);
}

const std::string& HtmlRoutines::getJavascriptAddScriptTags(const std::string& fileNameVirtual) {
  MacroRegisterFunctionWithName("HtmlRoutines::getJavascriptAddScriptTags");
  return HtmlRoutines::getFile(fileNameVirtual, "<script>", "</script>");
}

std::string HtmlRoutines::getJavascriptLink(const std::string& fileNameVirtual) {
  MacroRegisterFunctionWithName("HtmlRoutines::getJavascriptLink");
  std::stringstream out;
  std::string theFileName = FileOperations::GetVirtualNameWithHash(fileNameVirtual);
  out << "<script src =\"" << theFileName << "\"></script>\n";
  return out.str();
}

std::string HtmlRoutines::getCSSLink(const std::string& fileNameVirtual) {
  MacroRegisterFunctionWithName("HtmlRoutines::getCSSLink");
  std::stringstream out;
  std::string theFileName = FileOperations::GetVirtualNameWithHash(fileNameVirtual);
  out << "<link rel = \"stylesheet\" href= \"" << theFileName << "\">\n";
  return out.str();
}

const std::string& HtmlRoutines::getCSSAddStyleTags(const std::string& fileNameVirtual) {
  MacroRegisterFunctionWithName("HtmlRoutines::getCSSAddStyleTags");
  return HtmlRoutines::getFile(fileNameVirtual, "<style>", "</style>");
}

const std::string& HtmlRoutines::getMathQuillStyleSheeTWithTags() {
  return HtmlRoutines::getCSSAddStyleTags("/html-common/mathquill/mathquill.css");
}

const std::string HtmlRoutines::getCSSLinkLieAlgebrasAndCalculator(const std::string& relativeTo) {
  return
  HtmlRoutines::getCSSLink(relativeTo + "calculator-html/styleCalculator.css") +
  HtmlRoutines::getCSSLink(relativeTo + "calculator-html/style_lie_algebras.css") ;
}

const std::string HtmlRoutines::getJavascriptLinkGraphicsNDimensionsWithPanels(const std::string& relativeTo) {
  return
  HtmlRoutines::getJavascriptLink(relativeTo + "calculator-html/panels.js") +
  HtmlRoutines::getJavascriptLink(relativeTo + "calculator-html/graphics_n_dimensions.js")
  ;
}

const std::string& HtmlRoutines::getJavascriptMathQuillDefaultWithTags() {
  return  HtmlRoutines::getJavascriptAddScriptTags("/html-common/mathquill/mathquill.min.js");
}

const std::string& HtmlRoutines::getJavascriptMathQuillMatrixSupporTWithTags() {
  return  HtmlRoutines::getJavascriptAddScriptTags("/html-common/mathquill/mathquill.min.js");
}

const std::string& HtmlRoutines::getJavascriptBrowserifier() {
  return HtmlRoutines::getJavascriptAddScriptTags("/calculator-html/browserifier.js");
}

std::string HtmlRoutines::getCalculatorComputationURL(const std::string& inputNoEncoding) {
  std::stringstream out;
  JSData theRequest;
  theRequest[DatabaseStrings::labelCalculatorInput] = inputNoEncoding;
  theRequest[DatabaseStrings::labelCurrentPage] = WebAPI::request::calculatorPage;
  out << "#" << HtmlRoutines::convertStringToURLString(theRequest.toString(nullptr), false);
  return out.str();
}

std::string HtmlRoutines::getCalculatorComputationAnchor(const std::string& inputNoEncoding) {
  std::stringstream out;
  out << "<a href = \"" << HtmlRoutines::getCalculatorComputationURL(inputNoEncoding)
  << "\" onclick = \"window.calculator.calculator.calculatorLinkClickHandler(this);\">"
  << HtmlRoutines::convertStringToHtmlString(inputNoEncoding, false) << "</a>";
  return out.str();
}

std::string HtmlRoutines::getMathSpanPure(const std::string& input, int upperNumChars) {
  std::stringstream out;
  if (input.size() > static_cast<unsigned>(upperNumChars) && upperNumChars > 0) {
    out << "<b>LaTeX output is longer than " << upperNumChars
    << " characters and I dare not use mathjax. Here is the output as plain LaTeX.</b> " << input;
    return out.str();
  }
  out << "\\(\\displaystyle " << input << "\\)";
  return out.str();
}

std::string HtmlRoutines::getMathMouseHover(const std::string& input, int upperNumChars) {
  std::stringstream out;
  if (input.size() > static_cast<unsigned>(upperNumChars)) {
    out << "<b>LaTeX output is longer than " << upperNumChars
    << " characters and I dare not process the LaTeX. Here is the output as plain (LaTeX-able) text.</b> " << input;
    return out.str();
  }
//  std::stringstream idSpanStream;
  HtmlRoutines::GlobalMathSpanID ++;
//  if (HtmlRoutines::GlobalMathSpanID==1)
//    out << "<span class =\"math\"></span>"; //<- empty math span class forces jsMath to load.
//  idSpanStream << "mathFormula" << HtmlRoutines::GlobalMathSpanID;
//  std::string containterString="container"+ idSpanStream.str();
//  out << "<span id =\"" << containterString << "\">"  << "<span id =\"" << idSpanStream.str()
//  out << "<span><span onmouseover =\"if (this.parentNode.className == 'math') return; "
//  << "this.className ='math'; this.parentNode.className ='math';"
//  << "window.alert('Calling jsmath.Process'); "
//  << "jsMath.Process(this.parentNode);\" >"
  out << "\\(" << input << "\\)";// << "</span></span>";
  return out.str();
}

std::string HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(const std::string& input, int recursionDepth) {
  MacroRegisterFunctionWithName("HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml");
  if (recursionDepth > 50) {
    return input;
  }
  MapList<std::string, std::string, MathRoutines::HashString> currentMap;
  std::stringstream notUsed;
  if (!HtmlRoutines::chopPercentEncodedString(input, currentMap, notUsed)) {
    return input;
  }
  if (currentMap.size() == 0) {
    return "";
  }
  if (currentMap.size() == 1) {
    if (currentMap.theValues[0] == "") {
      return HtmlRoutines::convertURLStringToNormal(currentMap.theKeys[0], false);
    }
  }
  std::stringstream out;
  out << "<table border =\"1px solid black;\">";
  for (int i = 0; i < currentMap.size(); i ++) {
    out << "<tr>";
    out << "<td>"
    << HtmlRoutines::convertURLStringToNormal(currentMap.theKeys[i], false) << " </td>";
    if (currentMap.theValues[i] != "") {
      out << "<td>=</td><td>";
      if (currentMap.theValues[i] != "") {
        out << HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(
          HtmlRoutines::convertURLStringToNormal(currentMap.theValues[i], true), recursionDepth + 1
        );
      }
      out << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

std::string HtmlRoutines::convertURLStringToNormal(const std::string& input, bool replacePlusBySpace) {
  std::string output;
  HtmlRoutines::convertURLStringToNormal(input, output, replacePlusBySpace);
  return output;
}

void HtmlRoutines::convertURLStringToNormal(const std::string& input, std::string& output, bool replacePlusBySpace) {
  std::string readAhead;
  std::stringstream out;
  unsigned inputSize = static_cast<unsigned>(input.size());
  for (unsigned i = 0; i < inputSize; i ++) {
    readAhead = "";
    for (unsigned j = 0; j < 6; j ++) {
      if (i + j < inputSize) {
        readAhead.push_back(input[i + j]);
      }
      if (HtmlRoutines::URLStringToNormalOneStep(readAhead, out, replacePlusBySpace)) {
        i += j;
        break;
      }
    }
  }
  output = out.str();
}

std::string HtmlRoutines::getJavascriptMathjax(const std::string& baseFolder) {
  MacroRegisterFunctionWithName("HtmlRoutines::getJavascriptMathjax");
  std::stringstream out;
  std::string mathjaxSetupScript = FileOperations::GetVirtualNameWithHash(
    "calculator-html/mathjax-calculator-setup.js"
  );
  out << "<script type = 'text/javascript'>"
  << "var MathJaxSetupScriptURL = '" << baseFolder << mathjaxSetupScript << "';\n"
  << "var calculatorHtmlBaseFolder = '" << baseFolder << "calculator-html/'; </script>";
  out << "<script type =\"text/javascript\" src = '" << baseFolder << "MathJax-2.7-latest/MathJax.js?config=TeX-AMS_HTML-full'>"
  << "</script>\n";
  out << "<script src =\"" << baseFolder << mathjaxSetupScript << "\"></script>";
  HtmlRoutines::preLoadedFiles().setKeyValue("MathJax", out.str());
  return HtmlRoutines::preLoadedFiles().GetValueCreateNoInit("MathJax");
}

bool HtmlRoutines::accountOneInputPercentEncodedString(
  const std::string& fieldName,
  const std::string& fieldValue,
  MapList<std::string, std::string, MathRoutines::HashString>& outputMap,
  std::stringstream& commentsOnFailure
) {
  MacroRegisterFunctionWithName("HtmlRoutines::accountOneInputPercentEncodedString");
  if (fieldName == "") {
    return true;
  }
  (void) commentsOnFailure;
//  if (fieldValue != "" && outputMap.contains(fieldName))
//    if (outputMap.GetValueCreateIfNotPresent(fieldName) != fieldValue &&
//        outputMap.GetValueCreateIfNotPresent(fieldName) != "")
//    { commentsOnFailure << "More than one value specified for input field " << fieldName << ": "
//      << fieldValue << " and " << outputMap.GetValueCreateIfNotPresent(fieldName) << ". ";
//      return false;
//    }
  outputMap.setKeyValue(fieldName, fieldValue);
  return true;
}

bool HtmlRoutines::chopPercentEncodedString(
  const std::string& input,
  MapList<std::string, std::string, MathRoutines::HashString>& outputMap,
  std::stringstream& commentsOnFailure
) {
  MacroRegisterFunctionWithName("HtmlRoutines::chopPercentEncodedString");
  outputMap.clear();
  outputMap.setExpectedSize(15);
  return HtmlRoutines::chopPercentEncodedStringAppend(input, outputMap, commentsOnFailure);
}

bool HtmlRoutines::chopPercentEncodedStringAppend(
  const std::string& input,
  MapList<std::string, std::string, MathRoutines::HashString>& outputMap,
  std::stringstream& commentsOnFailure
) {
  MacroRegisterFunctionWithName("HtmlRoutines::chopPercentEncodedStringAppend");
  unsigned inputLength = static_cast<unsigned>(input.size());
  bool readingData = false;
  std::string currentFieldName = "";
  std::string currentFieldValue = "";
  currentFieldName.reserve(input.size());
  currentFieldValue.reserve(input.size());
  for (unsigned i = 0; i < inputLength; i ++) {
    if (!readingData && input[i] == '=') {
      readingData = true;
      continue;
    }
    if (input[i] != '&') {
      if (readingData) {
        currentFieldValue.push_back(input[i]);
      } else {
        currentFieldName.push_back(input[i]);
      }
      continue;
    }
    if (!HtmlRoutines::accountOneInputPercentEncodedString(currentFieldName, currentFieldValue, outputMap, commentsOnFailure)) {
      return false;
    }
    currentFieldName = "";
    currentFieldValue = "";
    readingData = false;
  }
  return HtmlRoutines::accountOneInputPercentEncodedString(currentFieldName, currentFieldValue, outputMap, commentsOnFailure);
}

bool HtmlRoutines::URLStringToNormalOneStep(std::string& readAhead, std::stringstream& out, bool replacePlusBySpace) {
  if (replacePlusBySpace) {
    if (readAhead[0] == '+') {
      out << " ";
      return true;
    }
  }
  bool isOK = readAhead[0] != '%' && readAhead[0] != '&';
  if (isOK) {
    out << readAhead[0];
    return true;
  }
  if (readAhead == "&") {
    out << " ";
    return true;
  }
  if (readAhead.size() == 3) {
    if (
      readAhead[0] == '%' &&
      MathRoutines::IsAHexDigit(readAhead[1]) &&
      MathRoutines::IsAHexDigit(readAhead[2])
    ) {
      char next =
      MathRoutines::ConvertHumanReadableHexToCharValue(readAhead[1]) * 16 +
      MathRoutines::ConvertHumanReadableHexToCharValue(readAhead[2]);
      out << next;
      return true;
    }
  }
  return false;
}
