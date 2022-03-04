 // The current file is licensed under the license terms found in the main header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "general_list_references.h"
#include "web_api.h"
#include "general_logging_global_variables.h"
#include "general_file_operations_encodings.h"
#include <iomanip>
#include "string_constants.h"
#include "general_strings.h"

MapList<
  std::string,
  std::string,
  HashFunctions::hashFunction<std::string>
>& HtmlRoutines::preLoadedFiles() {
  static MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > result;
  return result;
}

std::string HtmlRoutines::gitRepository =
"https://github.com/tmilev/calculator";
std::string HtmlRoutines::getJavascriptVariable(
  const std::string& variableName
) {
  return StringRoutines::convertStringToJavascriptVariable(variableName);
}

std::string HtmlRoutines::getHtmlLinkToGithubRepository(
  const std::string& displayString
) {
  std::stringstream out;
  out
  << "<a href='"
  << HtmlRoutines::gitRepository
  << "'>"
  << displayString
  << "</a>";
  return out.str();
}

std::string HtmlRoutines::getHtmlLinkFromProjectFileName(
  const std::string& fileName, const std::string& fileDesc, int line
) {
  std::stringstream out;
  out
  << " <a href=\""
  << HtmlRoutines::gitRepository
  << "/blob/master/src/"
  << FileOperations::getFileNameFromFileNameWithPath(fileName);
  if (line > 0) {
    out << "#L" << line;
  }
  out << "\">" << FileOperations::getFileNameFromFileNameWithPath(fileName);
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
  HtmlRoutines::getJavascriptAceEditorScriptWithTags();
}

const std::string& HtmlRoutines::getJavascriptAceEditorScriptWithTags() {
  if (HtmlRoutines::preLoadedFiles().contains("AceEditor")) {
    return
    HtmlRoutines::preLoadedFiles().getValueCreateNoInitialization("AceEditor");
  }
  std::stringstream out;
  out
  << "<script type =\"text/javascript\" src =\""
  << FileOperations::GetVirtualNameWithHash("/html-common/ace/src-min/ace.js")
  << "\" charset =\"utf-8\"></script>";
  out
  << HtmlRoutines::getJavascriptAddScriptTags(
    "/html-common/ace-editor-settings.js"
  );
  HtmlRoutines::preLoadedFiles().setKeyValue("AceEditor", out.str());
  return
  HtmlRoutines::preLoadedFiles().getValueCreateNoInitialization("AceEditor");
}

const std::string& HtmlRoutines::getFile(
  const std::string& fileNameVirtual,
  const std::string& additionalBeginTag,
  const std::string& additionalEndTag
) {
  MacroRegisterFunctionWithName("HtmlRoutines::getFile");
  std::string fileId = fileNameVirtual + additionalBeginTag + additionalEndTag;
  if (global.flagCachingInternalFilesOn) {
    if (
      HtmlRoutines::preLoadedFiles().getValueCreateEmpty(fileId) != ""
    ) {
      return HtmlRoutines::preLoadedFiles().getValueCreateEmpty(fileId);
    }
  }
  std::stringstream out, commentsOnFailure;
  std::string fileReader;
  if (
    FileOperations::loadFileToStringVirtual(
      fileNameVirtual, fileReader, false, &commentsOnFailure
    )
  ) {
    out << additionalBeginTag << fileReader << additionalEndTag;
  } else {
    global
    << Logger::red
    << "File: "
    << fileNameVirtual
    << " not found. "
    << commentsOnFailure.str()
    << Logger::endL;
    out
    << "<b style='color:red'>Failed to load file: ["
    << fileNameVirtual
    << "]. Comments: "
    << commentsOnFailure.str()
    << "</b>";
  }
  HtmlRoutines::preLoadedFiles().setKeyValue(fileId, out.str());
  return HtmlRoutines::preLoadedFiles().getValueCreateNoInitialization(fileId);
}

const std::string& HtmlRoutines::getJavascriptAddScriptTags(
  const std::string& fileNameVirtual
) {
  MacroRegisterFunctionWithName("HtmlRoutines::getJavascriptAddScriptTags");
  return HtmlRoutines::getFile(fileNameVirtual, "<script>", "</script>");
}

std::string HtmlRoutines::getJavascriptLink(
  const std::string& fileNameVirtual
) {
  MacroRegisterFunctionWithName("HtmlRoutines::getJavascriptLink");
  std::stringstream out;
  std::string fileName =
  FileOperations::GetVirtualNameWithHash(fileNameVirtual);
  out << "<script src =\"" << fileName << "\"></script>\n";
  return out.str();
}

std::string HtmlRoutines::getCSSLink(const std::string& fileNameVirtual) {
  MacroRegisterFunctionWithName("HtmlRoutines::getCSSLink");
  std::stringstream out;
  std::string fileName =
  FileOperations::GetVirtualNameWithHash(fileNameVirtual);
  out << "<link rel = \"stylesheet\" href= \"" << fileName << "\">\n";
  return out.str();
}

const std::string& HtmlRoutines::getCSSAddStyleTags(
  const std::string& fileNameVirtual
) {
  MacroRegisterFunctionWithName("HtmlRoutines::getCSSAddStyleTags");
  return HtmlRoutines::getFile(fileNameVirtual, "<style>", "</style>");
}

std::string HtmlRoutines::getCalculatorComputationURL(
  const std::string& inputNoEncoding
) {
  std::stringstream out;
  JSData request;
  request[DatabaseStrings::labelCalculatorInput] = inputNoEncoding;
  request[DatabaseStrings::labelCurrentPage] = WebAPI::frontend::calculatorPage
  ;
  out
  << "#"
  << HtmlRoutines::convertStringToURLString(
    request.toString(nullptr), false
  );
  return out.str();
}

std::string HtmlRoutines::getProblemURLRelative(
  const std::string& problemName
) {
  std::stringstream out;
  JSData anchor;
  anchor[WebAPI::frontend::currentPage] = WebAPI::frontend::problemPage;
  anchor[WebAPI::frontend::problemFileName] = problemName;
  out
  << "/"
  << WebAPI::appNoCache
  << "#"
  << HtmlRoutines::convertStringToURLString(anchor.toString(), false);
  return out.str();
}

std::string HtmlRoutines::getCalculatorComputationAnchorSamePage(
  const std::string& inputNoEncoding
) {
  std::stringstream out;
  out
  << "<a href = '"
  << HtmlRoutines::getCalculatorComputationURL(inputNoEncoding)
  <<
  "' onclick = 'window.calculator.calculator.calculatorLinkClickHandler(this);'>"
  << HtmlRoutines::convertStringToHtmlString(inputNoEncoding, false)
  << "</a>";
  return out.str();
}

std::string HtmlRoutines::getCalculatorComputationAnchor(
  const std::string& urlApplication,
  const std::string& inputNoEncoding,
  const std::string& desiredAnchorTextEmptyForDefault
) {
  std::stringstream out;
  std::string displayText = desiredAnchorTextEmptyForDefault;
  if (displayText == "") {
    displayText =
    HtmlRoutines::convertStringToHtmlString(inputNoEncoding, false);
  }
  out
  << "<a href = '"
  << urlApplication
  << HtmlRoutines::getCalculatorComputationURL(inputNoEncoding)
  << "' target='_blank'>"
  << displayText
  << "</a>";
  return out.str();
}

std::string HtmlRoutines::getCalculatorComputationAnchorSameURL(
  const std::string& inputNoEncoding,
  const std::string& desiredAnchorTextEmptyForDefault
) {
  return
  HtmlRoutines::getCalculatorComputationAnchor(
    "", inputNoEncoding, desiredAnchorTextEmptyForDefault
  );
}

std::string HtmlRoutines::getCalculatorComputationAnchorThisServer(
  const std::string& inputNoEncoding,
  const std::string& desiredAnchorTextEmptyForDefault
) {
  return
  HtmlRoutines::getCalculatorComputationAnchor(
    global.displayApplication,
    inputNoEncoding,
    desiredAnchorTextEmptyForDefault
  );
}

std::string HtmlRoutines::getMathSpan(
  const std::string& input, int upperNumChars, bool useDisplayStyle
) {
  std::stringstream out;
  if (
    input.size() > static_cast<unsigned>(upperNumChars) && upperNumChars > 0
  ) {
    out
    << "<b>LaTeX output is longer than "
    << upperNumChars
    << " characters and I dare not use the equation editor. "
    << "Here is the output as plain LaTeX.</b> "
    << input;
    return out.str();
  }
  if (global.flagUseMathTags) {
    out << "<div class='mathcalculator'>";
  } else {
    out << "\\(";
  }
  if (useDisplayStyle) {
    out << "\\displaystyle ";
  }
  out << input;
  if (global.flagUseMathTags) {
    out << "</div>";
  } else {
    out << "\\)";
  }
  return out.str();
}

std::string HtmlRoutines::getMathNoDisplay(
  const std::string& input, int upperNumChars
) {
  return HtmlRoutines::getMathSpan(input, upperNumChars, false);
}

std::string HtmlRoutines::getMathDisplayStyle(
  const std::string& input, int upperNumChars
) {
  return HtmlRoutines::getMathSpan(input, upperNumChars, true);
}

std::string HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(
  const std::string& input, int recursionDepth
) {
  MacroRegisterFunctionWithName(
    "HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml"
  );
  if (recursionDepth > 50) {
    return input;
  }
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > currentMap;
  std::stringstream notUsed;
  if (
    !HtmlRoutines::chopPercentEncodedString(input, currentMap, notUsed)
  ) {
    return input;
  }
  if (currentMap.size() == 0) {
    return "";
  }
  if (currentMap.size() == 1) {
    if (currentMap.values[0] == "") {
      return
      HtmlRoutines::convertURLStringToNormal(currentMap.keys[0], false);
    }
  }
  std::stringstream out;
  out << "<table border =\"1px solid black;\">";
  for (int i = 0; i < currentMap.size(); i ++) {
    out << "<tr>";
    out
    << "<td>"
    << HtmlRoutines::convertURLStringToNormal(currentMap.keys[i], false)
    << " </td>";
    if (currentMap.values[i] != "") {
      out << "<td>=</td><td>";
      if (currentMap.values[i] != "") {
        out
        << HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(
          HtmlRoutines::convertURLStringToNormal(
            currentMap.values[i], true
          ),
          recursionDepth + 1
        );
      }
      out << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

std::string HtmlRoutines::scriptFromJSON(
  const std::string& scriptType, const JSData& scriptContent
) {
  std::stringstream out;
  out
  << "<script "
  << WebAPI::result::scriptType
  << "='"
  << scriptType
  << "'>\n"
  << scriptContent.toString()
  << "</script>";
  return out.str();
}

std::string HtmlRoutines::jsonContainer(
  const std::string& scriptType, const JSData& scriptContent
) {
  std::stringstream out;
  out
  << "<span name='script' style='display:none' "
  << WebAPI::result::scriptType
  << "='"
  << scriptType
  << "'>\n"
  << scriptContent.toString()
  << "</span>";
  return out.str();
}

std::string HtmlRoutines::convertURLStringToNormal(
  const std::string& input, bool replacePlusBySpace
) {
  std::string output;
  HtmlRoutines::convertURLStringToNormal(input, output, replacePlusBySpace);
  return output;
}

void HtmlRoutines::convertURLStringToNormal(
  const std::string& input,
  std::string& output,
  bool replacePlusBySpace
) {
  std::string readAhead;
  std::stringstream out;
  unsigned inputSize = static_cast<unsigned>(input.size());
  for (unsigned i = 0; i < inputSize; i ++) {
    readAhead = "";
    for (unsigned j = 0; j < 6; j ++) {
      if (i + j < inputSize) {
        readAhead.push_back(input[i + j]);
      }
      if (
        HtmlRoutines::URLStringToNormalOneStep(
          readAhead, out, replacePlusBySpace
        )
      ) {
        i += j;
        break;
      }
    }
  }
  output = out.str();
}

bool HtmlRoutines::accountOneInputPercentEncodedString(
  const std::string& fieldName,
  const std::string& fieldValue,
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  >& outputMap,
  std::stringstream& commentsOnFailure
) {
  MacroRegisterFunctionWithName(
    "HtmlRoutines::accountOneInputPercentEncodedString"
  );
  if (fieldName == "") {
    return true;
  }
  (void) commentsOnFailure;
  //  if (fieldValue != "" && outputMap.contains(fieldName))
  //    if (outputMap.GetValueCreateIfNotPresent(fieldName) != fieldValue &&
  //        outputMap.GetValueCreateIfNotPresent(fieldName) != "")
  // { commentsOnFailure << "More than one value specified for input field " <<
  // fieldName << ": "
  // << fieldValue << " and " << outputMap.GetValueCreateIfNotPresent(fieldName)
  // << ". ";
  //      return false;
  //    }
  outputMap.setKeyValue(fieldName, fieldValue);
  return true;
}

bool HtmlRoutines::chopPercentEncodedString(
  const std::string& input,
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  >& outputMap,
  std::stringstream& commentsOnFailure
) {
  MacroRegisterFunctionWithName("HtmlRoutines::chopPercentEncodedString");
  outputMap.clear();
  outputMap.setExpectedSize(15);
  return
  HtmlRoutines::chopPercentEncodedStringAppend(
    input, outputMap, commentsOnFailure
  );
}

bool HtmlRoutines::chopPercentEncodedStringAppend(
  const std::string& input,
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  >& outputMap,
  std::stringstream& commentsOnFailure
) {
  MacroRegisterFunctionWithName("HtmlRoutines::chopPercentEncodedStringAppend")
  ;
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
    if (
      !HtmlRoutines::accountOneInputPercentEncodedString(
        currentFieldName, currentFieldValue, outputMap, commentsOnFailure
      )
    ) {
      return false;
    }
    currentFieldName = "";
    currentFieldValue = "";
    readingData = false;
  }
  return
  HtmlRoutines::accountOneInputPercentEncodedString(
    currentFieldName, currentFieldValue, outputMap, commentsOnFailure
  );
}

bool HtmlRoutines::URLStringToNormalOneStep(
  std::string& readAhead,
  std::stringstream& out,
  bool replacePlusBySpace
) {
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
      MathRoutines::isHexDigit(readAhead[1]) &&
      MathRoutines::isHexDigit(readAhead[2])
    ) {
      char next =
      MathRoutines::convertHumanReadableHexToCharValue(readAhead[1]) * 16 +
      MathRoutines::convertHumanReadableHexToCharValue(readAhead[2]);
      out << next;
      return true;
    }
  }
  return false;
}
