//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1General1_ListReferences.h"
#include "vpfHeader8HtmlSnippets.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"
#include "vpfHeader1General7FileOperations_Encodings.h"
#include <iomanip>

ProjectInformationInstance projectInfoInstanceHtmlSnippets
(__FILE__, "Html Snippets.");

MapLisT<std::string, std::string, MathRoutines::hashString> HtmlRoutines::preLoadedFiles;

std::string HtmlRoutines::GetJavascriptSubmitEmails()
{ std::stringstream out;
  std::string deadlineInfoRowId = "defaultDeadlines";
  std::string problemInfoRowId = "defaultProblemInfo";
  out
  << "<script type=\"text/javascript\" id=\"scriptSubmitEmails\"> \n"
  << "function addEmailsOrUsers(idEmailList, problemCollectionName, idOutput, userRole, idUserGroup, idPasswords, requestType){\n"
  << "  spanOutput = document.getElementById(idOutput);\n"
  << "  if (spanOutput==null){\n"
  << "    spanOutput = document.createElement('span');\n"
  << "    document.body.appendChild(spanOutput);\n"
  << "    spanOutput.innerHTML= \"<span style='color:red'> ERROR: span with id \" + idEmailList + \"MISSING! </span>\";\n"
  << "  }\n"
  << "  spanEmailList = document.getElementById(idEmailList);\n"
  << "  spanUserGroup = document.getElementById(idUserGroup);\n"
  << "  spanPasswords = document.getElementById(idPasswords);\n"
  << "  params='request='+requestType+'&';\n"
  << "  params+='userRole='+userRole;\n"
  << "  params+='&userList=' + encodeURIComponent(spanEmailList.value);\n"
  << "  params+='&" << DatabaseStrings::labelSection << "=' + encodeURIComponent(spanUserGroup.value);\n"
  << "  params+='&passwordList=' + encodeURIComponent(spanPasswords.value);\n"
  << "  params+='&deadlineInfoRowId=' + encodeURIComponent('" << deadlineInfoRowId << "');\n"
  << "  params+='&problemInfoRowId=' + encodeURIComponent('" << problemInfoRowId << "');\n"
  << "  params+='&filterAccounts=' + '" << theGlobalVariables.GetWebInput("filterAccounts") << "';\n"
  << "  var https = new XMLHttpRequest();\n"
  << "  https.open(\"POST\", '" << theGlobalVariables.DisplayNameExecutable << "', true);\n"
  << "  https.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\");\n"
  << "  https.onload = function() {\n"
  << "    spanOutput.innerHTML=https.responseText;\n"
  << "  }\n"
  << "  https.send(params);\n"
  << "}\n"
  << "</script>";
  return out.str();
}

std::string HtmlRoutines::GetJavascriptHideHtmlWithTags()
{ std::stringstream output;
  output << " <!>\n";
  output << " <script type=\"text/javascript\"> \n";
  output << "\"use strict\";\n";
  output << " function switchMenu(obj)\n";
  output << " { var el = document.getElementById(obj);	\n";
  output << "   if ( el.style.display != \"none\" ) \n";
  output << "     el.style.display = 'none';\n";
  output << "   else \n";
  output << "     el.style.display = '';\n";
  output << " }\n";
  output << " function hideItem(obj)\n";
  output << " { document.getElementById(obj).style.display=\"none\";\n";
  output << " }\n";
  output << " function showItem(obj)\n";
  output << " { document.getElementById(obj).style.display=\"\";\n";
  output << " }\n";
  output << " </script>\n";
  return output.str();
}

const std::string HtmlRoutines::GetJavascriptCookieFunctionsLink()
{ return HtmlRoutines::GetJavascriptLink("/calculator-html/cookie-functions.js");
}

const std::string& HtmlRoutines::GetJavascriptCookieFunctionSNoTags()
{ return HtmlRoutines::GetFile("/calculator-html/cookie-functions.js");
}

const std::string HtmlRoutines::GetJavascriptForgotLogin()
{ return HtmlRoutines::GetJavascriptLink("/calculator-html/forgot-login.js");
}

const std::string HtmlRoutines::GetJavascriptProblemLinksLink()
{ return HtmlRoutines::GetJavascriptLink("/calculator-html/problemlinkstyles.js");
}

const std::string& HtmlRoutines::GetJavascriptProblemLinkSWithTags()
{ MacroRegisterFunctionWithName("HtmlRoutines::GetJavascriptProblemLinkSWithTags");
  return HtmlRoutines::GetJavascriptAddScriptTags("/calculator-html/problemlinkstyles.js");
}

const std::string& HtmlRoutines::GetJavascriptTopicLisTWithTags()
{ MacroRegisterFunctionWithName("HtmlRoutines::GetJavascriptTopicListWithTags");
  return HtmlRoutines::GetJavascriptAddScriptTags("/calculator-html/topiclist.js");
}

const std::string HtmlRoutines::GetJavascriptTopicListLink()
{ MacroRegisterFunctionWithName("HtmlRoutines::GetJavascriptTopicListLink");
  return HtmlRoutines::GetJavascriptLink("/calculator-html/topiclist.js");
}

const std::string& HtmlRoutines::GetJavascriptCanvasGraphicSWithTags()
{ return HtmlRoutines::GetJavascriptAddScriptTags("/calculator-html/three-d.js");
}

const std::string HtmlRoutines::GetJavascriptCanvasGraphicsLink()
{ return HtmlRoutines::GetJavascriptLink("/calculator-html/three-d.js");
}

const std::string& HtmlRoutines::GetJavascriptDatabaseRoutineSWithTags()
{ MacroRegisterFunctionWithName("HtmlRoutines::GetJavascriptDatabaseRoutinesWithTags");
  return HtmlRoutines::GetJavascriptAddScriptTags("/calculator-html/database-routines.js");
}

const std::string HtmlRoutines::GetJavascriptDatabaseRoutinesLink()
{ MacroRegisterFunctionWithName("HtmlRoutines::GetJavascriptDatabaseRoutinesWithTags");
  return HtmlRoutines::GetJavascriptLink("/calculator-html/database-routines.js");
}

std::string HtmlRoutines::GetJavascriptVariable(const std::string& theVar)
{ std::stringstream sanitizer;
  for (unsigned i = 0; i < theVar.size(); i ++)
  { if (MathRoutines::isALatinLetter(theVar[i]))
    { sanitizer << theVar[i];
      continue;
    }
    if (i == 0)
      sanitizer << "_";
    sanitizer << ((int) theVar[i]);
  }
  return sanitizer.str();
}

std::string HtmlRoutines::GetDatePickerJavascriptInit()
{ std::stringstream out;
  if (!theGlobalVariables.flagAceIsAvailable)
    out
    << "<link rel=\"stylesheet\" href=\"//code.jquery.com/ui/1.11.4/themes/smoothness/jquery-ui.css\">\n"
    << "<script src=\"https://code.jquery.com/jquery-3.1.0.min.js\" "
    << "integrity=\"sha256-cCueBR6CsyA4/9szpPfrX3s49M9vUU5BgtiJj06wt/s=\" "
    << "crossorigin=\"anonymous\"></script>\n"
    << "<script src=\"https://code.jquery.com/ui/1.12.0/jquery-ui.min.js\"   "
    << "integrity=\"sha256-eGE6blurk5sHj+rmkfsGYeKyZx3M4bG+ZlFyA7Kns7E=\" crossorigin=\"anonymous\"></script>\n";
  else
    out
    << "<link rel=\"stylesheet\" href=\"/html-common/jquery-ui.css\">\n"
    << "<script src=\"/html-common/jquery.min.js\"></script>\n"
    << "<script src=\"/html-common/jquery-ui.min.js\"></script>\n";

  return out.str();
}

std::string HtmlRoutines::GetHtmlLinkFromProjectFileName(const std::string& fileName, const std::string& fileDesc, int line)
{ std::stringstream out;
  out << " <a href=\"https://github.com/tmilev/calculator/blob/master/src/"
  << FileOperations::GetFileNameFromFileNameWithPath(fileName);
  if (line > 0)
    out << "#L" << line;
  out << "\">" << FileOperations::GetFileNameFromFileNameWithPath(fileName);
  if (fileDesc != "")
    out << " (" << fileDesc << ")";
  out << "</a>\n";
  return out.str();
}

std::string HtmlRoutines::GetLatexEmbeddableLinkFromCalculatorInput(const std::string& address, const std::string& display)
{ std::stringstream out;
  out << "\\href{" << GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable << "?";
  for (unsigned i = 0; i < address.size(); i ++)
  { //if (input[i]=='&')
     // out << "\\&";
    //else
    if (address[i] == '%')
      out << "\\%";
    else if (address[i] == '_')
      out << "\\_";
    else
      out << address[i];
  }
  out << "}{";
  for (unsigned i = 0; i < display.size(); i ++)
  { //if (input[i]=='&')
     // out << "\\&";
    //else
    if (display[i] == '%')
      out << "\\%";
    else if (display[i] == '_')
      out << "\\_";
    else if (display[i] == '{')
      out << "\\{";
    else if (display[i] == '}')
      out << "\\}";
    else
      out << display[i];
  }
  out << "}";
  return out.str();
}

std::string HtmlRoutines::GetMathMouseHoverBeginArrayL(const std::string& input, int upperNumChars)
{ std::stringstream out;
  out << "\\begin{array}{l}" << input << "\\end{array}";
  if (out.str().size() < (unsigned) upperNumChars && upperNumChars > 0)
    return HtmlRoutines::GetMathMouseHover(out.str(), upperNumChars);
  else
    return HtmlRoutines::GetMathMouseHover(input, upperNumChars);
}

std::string HtmlRoutines::GetMathSpanBeginArrayL(const std::string& input, int upperNumChars)
{ std::stringstream out;
  out << "\\begin{array}{l} " << input << " \\end{array}";
  if (out.str().size() < (unsigned) upperNumChars && upperNumChars > 0)
    return HtmlRoutines::GetMathSpanPure(out.str(), upperNumChars);
  else
    return HtmlRoutines::GetMathSpanPure(input, upperNumChars);
}

void HtmlRoutines::LoadStrings()
{ if (HtmlRoutines::preLoadedFiles.size() > 0)
    return;
  HtmlRoutines::GetJavascriptCookieFunctionSNoTags();
  HtmlRoutines::GetMathQuillStyleSheeTWithTags();
  HtmlRoutines::GetJavascriptInitializeButtonSWithTags();
  HtmlRoutines::GetJavascriptAceEditorScriptWithTags();
  HtmlRoutines::GetJavascriptCalculatorPageWithTags();
  HtmlRoutines::GetJavascriptAccountManagemenTWithTags();
  HtmlRoutines::GetCSSLinkCalculator();
  HtmlRoutines::GetJavascriptAutocompletEWithTags();
  HtmlRoutines::GetJavascriptSha1();
  HtmlRoutines::GetJavascriptMathjax();
  HtmlRoutines::GetJavascriptMathQuillDefaulTWithTags();
  HtmlRoutines::GetJavascriptMathQuillMatrixSupporTWithTags();
  HtmlRoutines::GetJavascriptProblemLinkSWithTags();
  HtmlRoutines::GetJavascriptDatabaseRoutineSWithTags();
  HtmlRoutines::GetJavascriptCanvasGraphicSWithTags();
  HtmlRoutines::GetJavascriptTopicLisTWithTags();
}

extern logger logWorker;
extern logger logServer;

const std::string& HtmlRoutines::GetJavascriptCalculatorPageWithTags()
{ MacroRegisterFunctionWithName("HtmlRoutines::GetJavascriptCalculatorPage");
  return HtmlRoutines::GetJavascriptAddScriptTags("/calculator-html/calculatorPage.js");
}

const std::string HtmlRoutines::GetJavascriptCalculatorPageLink()
{ MacroRegisterFunctionWithName("HtmlRoutines::GetJavascriptCalculatorPageLink");
  return HtmlRoutines::GetJavascriptLink("/calculator-html/calculatorPage.js");
}

const std::string& HtmlRoutines::GetJavascriptAccountManagemenTWithTags()
{ return HtmlRoutines::GetJavascriptAddScriptTags("/calculator-html/account-management.js");
}

const std::string HtmlRoutines::GetJavascriptAccountManagementLink()
{ return HtmlRoutines::GetJavascriptLink("/calculator-html/account-management.js");
}

const std::string& HtmlRoutines::GetJavascriptAceEditorScriptWithTags()
{ if (HtmlRoutines::preLoadedFiles.Contains("AceEditor"))
    return HtmlRoutines::preLoadedFiles.GetValueCreateNoInit("AceEditor");
  std::stringstream out;
  out << "<script type=\"text/javascript\" src=\""
  << FileOperations::GetVirtualNameWithHash("/html-common/ace/src-min/ace.js")
  << "\" charset=\"utf-8\"></script>";
  out << HtmlRoutines::GetJavascriptAddScriptTags("/html-common/ace-editor-settings.js");
  HtmlRoutines::preLoadedFiles.SetKeyValue("AceEditor", out.str());
  return HtmlRoutines::preLoadedFiles.GetValueCreateNoInit("AceEditor");
}

const std::string& HtmlRoutines::GetFile
(const std::string& fileNameVirtual, const std::string& additionalBeginTag,
 const std::string& additionalEndTag)
{ MacroRegisterFunctionWithName("HtmlRoutines::GetFile");
  std::string theID = fileNameVirtual + additionalBeginTag + additionalEndTag;
  if (theGlobalVariables.flagCachingInternalFilesOn)
    if (HtmlRoutines::preLoadedFiles.GetValueCreate(theID) != "")
      return HtmlRoutines::preLoadedFiles.GetValueCreate(theID);
  std::stringstream out, commentsOnFailure;
  std::string fileReader;
  if (FileOperations::LoadFileToStringVirtual(fileNameVirtual, fileReader, false, false, &commentsOnFailure))
    out << additionalBeginTag << fileReader << additionalEndTag;
  else
  { if (theGlobalVariables.processType == ProcessTypes::worker)
      logWorker << logger::red << "File: "
      << fileNameVirtual << " not found. " << commentsOnFailure.str() << logger::endL;
    if (theGlobalVariables.processType == ProcessTypes::server)
      logServer << logger::red << "File: "
      << fileNameVirtual << " not found. " << commentsOnFailure.str() << logger::endL;
    out << "<b style=\"color:red\">Failed to load file: " << fileNameVirtual
    << ". Comments: " << commentsOnFailure.str() << "</b>";
  }
  HtmlRoutines::preLoadedFiles.SetKeyValue(theID, out.str());
  return HtmlRoutines::preLoadedFiles.GetValueCreateNoInit(theID);
}

const std::string& HtmlRoutines::GetJavascriptAddScriptTags(const std::string& fileNameVirtual)
{ MacroRegisterFunctionWithName("HtmlRoutines::GetJavascriptAddScriptTags");
  return HtmlRoutines::GetFile(fileNameVirtual, "<script>", "</script>");
}

std::string HtmlRoutines::GetJavascriptLink(const std::string& fileNameVirtual)
{ MacroRegisterFunctionWithName("HtmlRoutines::GetJavascriptLink");
  std::stringstream out;
  std::string theFileName = FileOperations::GetVirtualNameWithHash(fileNameVirtual);
  out << "<script src=\"" << theFileName << "\"></script>\n";
  return out.str();
}

std::string HtmlRoutines::GetCSSLink(const std::string& fileNameVirtual)
{ MacroRegisterFunctionWithName("HtmlRoutines::GetCSSLink");
  std::stringstream out;
  std::string theFileName = FileOperations::GetVirtualNameWithHash(fileNameVirtual);
  out << "<link rel=\"stylesheet\" href= \"" << theFileName << "\">\n";
  return out.str();
}

const std::string& HtmlRoutines::GetCSSAddStyleTags(const std::string& fileNameVirtual)
{ MacroRegisterFunctionWithName("HtmlRoutines::GetCSSAddStyleTags");
  return HtmlRoutines::GetFile(fileNameVirtual, "<style>", "</style>");
}

const std::string& HtmlRoutines::GetJavascriptInitializeButtonSWithTags()
{ return HtmlRoutines::GetJavascriptAddScriptTags("/calculator-html/initializebuttons.js");
}

const std::string HtmlRoutines::GetJavascriptInitializeButtonsLink()
{ return HtmlRoutines::GetJavascriptLink("/calculator-html/initializebuttons.js");
}

const std::string& HtmlRoutines::GetMathQuillStyleSheeTWithTags()
{ return HtmlRoutines::GetCSSAddStyleTags("/html-common/mathquill.css");
}

const std::string HtmlRoutines::GetMathQuillStyleSheetLink()
{ return HtmlRoutines::GetCSSLink("/html-common/mathquill.css");
}

const std::string HtmlRoutines::GetCSSLinkCalculator()
{ return HtmlRoutines::GetCSSLink("/calculator-html/styleCalculator.css");
}

const std::string& HtmlRoutines::GetJavascriptMathQuillDefaulTWithTags()
{ return  HtmlRoutines::GetJavascriptAddScriptTags("/html-common/mathquill.min.js");
}

const std::string HtmlRoutines::GetJavascriptMathQuillDefaultLink()
{ return  HtmlRoutines::GetJavascriptLink("/html-common/mathquill.min.js");
}

const std::string& HtmlRoutines::GetJavascriptMathQuillMatrixSupporTWithTags()
{ return  HtmlRoutines::GetJavascriptAddScriptTags("/html-common/mathquill.min-matrix.js");
}

const std::string HtmlRoutines::GetJavascriptMathQuillMatrixSupportLink()
{ return  HtmlRoutines::GetJavascriptLink("/html-common/mathquill.min-matrix.js");
}

std::string HtmlRoutines::GetJavascriptMathQuillDefaultFull()
{ std::stringstream out;
  out << "<script src=\""
  << FileOperations::GetVirtualNameWithHash("/html-common/jquery.min.js")
  << "\"></script>\n"
  << HtmlRoutines::GetJavascriptMathQuillDefaultLink() << "\n"
  << "<script type=\"text/javascript\">var globalMQ = MathQuill.getInterface(2); var MathQuillHasMatrixSupport=false;</script>";
  return out.str();
}

std::string HtmlRoutines::GetJavascriptMathQuillMatrixSupportFull()
{ std::stringstream out;
  out << "<script src=\"/html-common/jquery.min.js\"></script>\n"
  << HtmlRoutines::GetJavascriptMathQuillMatrixSupportLink() << "\n"
  << "<script type=\"text/javascript\">var globalMQ = MathQuill.getInterface(2); var MathQuillHasMatrixSupport=true;</script>";
  return out.str();
}

const std::string& HtmlRoutines::GetJavascriptAutocompletEWithTags()
{ return HtmlRoutines::GetJavascriptAddScriptTags("/calculator-html/autocomplete.js");
}

const std::string HtmlRoutines::GetJavascriptAutocompleteLink()
{ return HtmlRoutines::GetJavascriptLink("/calculator-html/autocomplete.js");
}

const std::string& HtmlRoutines::GetJavascriptSha1()
{ return HtmlRoutines::GetJavascriptAddScriptTags("/html-common/sha1.js");
}

std::string HtmlRoutines::GetCalculatorLink(const std::string& DisplayNameCalculator, const std::string& input)
{ std::stringstream out;
  out << "<a href=\"" << DisplayNameCalculator << "?request=calculator&mainInput="
  << HtmlRoutines::ConvertStringToURLString(input, false) << "\"> " << input << "</a>";
  return out.str();
}

std::string HtmlRoutines::GetCalculatorLinkUnclosedPostArguments(const std::string& input)
{ return HtmlRoutines::GetCalculatorLinkUnclosedPostArguments(theGlobalVariables.DisplayNameExecutable, input);
}

std::string HtmlRoutines::GetCalculatorLinkUnclosedPostArguments(const std::string& DisplayNameCalculator, const std::string& input)
{ std::stringstream out;
  static int globalLinkCounter = 0;
  int linkCounter = ++ globalLinkCounter; //<-Using linkCounter should be thread safer than using globalLinkCounter.
  //No need to make it really thread safe as this function is not critical and it is unlikely it will ever get
  //used by more than one thread.
  out << "<form id=\"submissionForm" << linkCounter << "\" method=\"POST\" action=\"" << DisplayNameCalculator << "\">";
  out << "<input type=\"hidden\" name=\"doubleURLencodedInput\" value=\""
  << HtmlRoutines::ConvertStringToURLString(input, false) << "\">";
  out << "</form>";
  out << "<a href=\"document.getElementById('submissionForm" << linkCounter << "').submit();\"> ";
  return out.str();
}

std::string HtmlRoutines::GetMathSpanPure(const std::string& input, int upperNumChars)
{ std::stringstream out;
//  int dirtylittleHAckHEre;
//  upperNumChars=1;
  if (input.size() > (unsigned) upperNumChars && upperNumChars > 0)
  { out << "<b>LaTeX output is longer than " << upperNumChars << " characters and I dare not use mathjax. Here is the output as plain LaTeX.</b> " << input;
    return out.str();
  }
  out << "\\(\\displaystyle " << input << "\\)";
  return out.str();
}

std::string HtmlRoutines::GetMathMouseHover(const std::string& input, int upperNumChars)
{ std::stringstream out;
  if (input.size() > (unsigned) upperNumChars)
  { out << "<b>LaTeX output is longer than " << upperNumChars
    << " characters and I dare not process the LaTeX. Here is the output as plain (LaTeX-able) text.</b> " << input;
    return out.str();
  }
//  std::stringstream idSpanStream;
  HtmlRoutines::GlobalMathSpanID ++;
//  if (HtmlRoutines::GlobalMathSpanID==1)
//    out << "<span class=\"math\"></span>"; //<- empty math span class forces jsMath to load.
//  idSpanStream << "mathFormula" << HtmlRoutines::GlobalMathSpanID;
//  std::string containterString="container"+idSpanStream.str();
//  out << "<span id=\"" << containterString << "\">"  << "<span id=\"" << idSpanStream.str()
//  out << "<span><span onmouseover=\"if (this.parentNode.className=='math') return; "
//  << "this.className='math'; this.parentNode.className='math';"
//  << "window.alert('Calling jsmath.Process'); "
//  << "jsMath.Process(this.parentNode);\" >"
  out << "\\(" << input << "\\)";// << "</span></span>";
  return out.str();
}

std::string HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(const std::string& input, int recursionDepth)
{ MacroRegisterFunctionWithName("HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml");
//  stOutput << "I'm here<br>";
  if (recursionDepth > 50)
    return input;
  MapLisT<std::string, std::string, MathRoutines::hashString> currentMap;
  std::stringstream notUsed;
  if (!HtmlRoutines::ChopCGIString(input, currentMap, notUsed))
  { //stOutput << "oh no: " << notUsed.str();
    return input;
  }
  if (currentMap.size() == 0)
    return "";
  if (currentMap.size() == 1)
    if (currentMap[0] == "")
      return HtmlRoutines::ConvertURLStringToNormal(currentMap.theKeys[0], false);
  std::stringstream out;
  out << "<table border=\"1px solid black;\">";
  for (int i = 0; i < currentMap.size(); i ++)
  { out << "<tr>";
    out << "<td>"
    << HtmlRoutines::ConvertURLStringToNormal(currentMap.theKeys[i], false) << " </td>";
    if (currentMap[i] != "")
    { out << "<td>=</td><td>";
      if (currentMap[i] != "")
        out << HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(HtmlRoutines::ConvertURLStringToNormal(currentMap.theValues[i], true), recursionDepth+1);
      out << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

std::string HtmlRoutines::ConvertURLStringToNormal(const std::string& input, bool replacePlusBySpace)
{ std::string output;
  HtmlRoutines::ConvertURLStringToNormal(input, output, replacePlusBySpace);
  return output;
}

void HtmlRoutines::ConvertURLStringToNormal(const std::string& input, std::string& output, bool replacePlusBySpace)
{ std::string readAhead;
  std::stringstream out;
  int inputSize = (signed) input.size();
  for (int i = 0; i < inputSize; i ++)
  { readAhead = "";
    for (int j = 0; j < 6; j ++)
    { if (i + j < inputSize)
        readAhead.push_back(input[i + j]);
      if (HtmlRoutines::URLStringToNormalOneStep(readAhead, out, replacePlusBySpace))
      { i += j;
        break;
      }
    }
  }
  output = out.str();
}

const std::string& HtmlRoutines::GetJavascriptMathjax()
{ MacroRegisterFunctionWithName("HtmlRoutines::GetJavascriptMathjax");
  if (HtmlRoutines::preLoadedFiles.Contains("MathJax"))
    return HtmlRoutines::preLoadedFiles.GetValueCreateNoInit("MathJax");
  std::stringstream out;
  std::string mathjaxSetupScript = FileOperations::GetVirtualNameWithHash
  ("/calculator-html/new/mathjax-calculator-setup.js");
  out << "<script type=\"text/javascript\">MathJaxSetupScriptURL=\"" << mathjaxSetupScript << "\"</script>";
  out << "<script type=\"text/javascript\" src=\"/MathJax-2.7-latest/MathJax.js?config=TeX-AMS_HTML-full"
//  << ","
//  << mathjaxSetupScript
  << "\"></script>\n";
  out << "<script src=\"" << mathjaxSetupScript << "\"></script>";
  HtmlRoutines::preLoadedFiles.SetKeyValue("MathJax", out.str());
  return HtmlRoutines::preLoadedFiles.GetValueCreateNoInit("MathJax");
}

bool HtmlRoutines::AccountOneInputCGIString
(const std::string& fieldName, const std::string& fieldValue, MapLisT<std::string, std::string, MathRoutines::hashString>& outputMap,
 std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("HtmlRoutines::AccountOneInputCGIString");
  if (fieldName == "")
    return true;
  (void) commentsOnFailure;
//  if (fieldValue!="" && outputMap.Contains(fieldName))
//    if (outputMap.GetValueCreateIfNotPresent(fieldName)!=fieldValue &&
//        outputMap.GetValueCreateIfNotPresent(fieldName)!="")
//    { commentsOnFailure << "More than one value specified for input field " << fieldName << ": "
//      << fieldValue << " and " << outputMap.GetValueCreateIfNotPresent(fieldName) << ". ";
//      return false;
//    }
  outputMap.SetKeyValue(fieldName, fieldValue);
  return true;
}

bool HtmlRoutines::ChopCGIString(const std::string& input, MapLisT<std::string, std::string, MathRoutines::hashString>& outputMap,
 std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("HtmlRoutines::ChopCGIString");
  outputMap.Clear();
  outputMap.SetExpectedSize(15);
  return HtmlRoutines::ChopCGIStringAppend(input, outputMap, commentsOnFailure);
}

bool HtmlRoutines::ChopCGIStringAppend(const std::string& input, MapLisT<std::string, std::string, MathRoutines::hashString>& outputMap,
 std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("HtmlRoutines::ChopCGIStringAppend");
  int inputLength = (signed) input.size();
  bool readingData = false;
  std::string currentFieldName = "";
  std::string currentFieldValue = "";
  currentFieldName.reserve(input.size());
  currentFieldValue.reserve(input.size());
  for (int i = 0; i < inputLength; i ++)
  { if (!readingData && input[i] == '=')
    { readingData = true;
      continue;
    }
    if (input[i] != '&')
    { if (readingData)
        currentFieldValue.push_back(input[i]);
      else
        currentFieldName.push_back(input[i]);
      continue;
    }
    if (!HtmlRoutines::AccountOneInputCGIString(currentFieldName, currentFieldValue, outputMap, commentsOnFailure))
      return false;
    currentFieldName = "";
    currentFieldValue = "";
    readingData = false;
  }
  return HtmlRoutines::AccountOneInputCGIString(currentFieldName, currentFieldValue, outputMap, commentsOnFailure);
}

bool HtmlRoutines::URLStringToNormalOneStep(std::string& readAhead, std::stringstream& out, bool replacePlusBySpace)
{ if (replacePlusBySpace)
    if (readAhead[0] == '+')
      { out << " ";
        return true;
      }
  bool isOK = readAhead[0] != '%' && readAhead[0] != '&';
  if (isOK)
  { out << readAhead[0];
    return true;
  }
  if (readAhead == "&")
  { out << " ";
    return true;
  }
  if (readAhead.size() == 3)
    if (readAhead[0] == '%' && MathRoutines::IsAHexDigit(readAhead[1])
        && MathRoutines::IsAHexDigit(readAhead[2]))
    { out << (char)(MathRoutines::ConvertHumanReadableHexToCharValue(readAhead[1]) * 16 +
      MathRoutines::ConvertHumanReadableHexToCharValue(readAhead[2]));
      return true;
    }
  return false;
}
