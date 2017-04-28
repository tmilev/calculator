//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1General1_ListReferences.h"
#include "vpfHeader8HtmlSnippets.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"
#include "vpfHeader1General7FileOperations_Encodings.h"
#include <iomanip>

ProjectInformationInstance projectInfoInstanceHtmlSnippets
(__FILE__, "Html Snippets.");

std::string HtmlRoutines::GetJavascriptSubmitEmails()
{ std::stringstream out;
  std::string deadlineInfoRowId="defaultDeadlines";
  std::string problemInfoRowId="defaultProblemInfo";
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
  << "  params+='&" << DatabaseStrings::userGroupLabel << "=' + encodeURIComponent(spanUserGroup.value);\n"
  << "  params+='&passwordList=' + encodeURIComponent(spanPasswords.value);\n"
  << "  params+='&deadlineInfoRowId=' + encodeURIComponent('" << deadlineInfoRowId << "');\n"
  << "  params+='&problemInfoRowId=' + encodeURIComponent('" << problemInfoRowId << "');\n"
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

std::string HtmlRoutines::GetJavascriptHideHtml()
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

std::string HtmlRoutines::GetJavascriptCookieFunctions()
{ std::stringstream out;
  out
  << "function getCookie(c_name)\n"
  << "{ VPFcookie=document.cookie.split(\";\");\n"
  << "  for (i=0;i<VPFcookie.length;i++)\n"
  << "  { x=VPFcookie[i].substr(0,VPFcookie[i].indexOf(\"=\"));\n"
  << "  	y=VPFcookie[i].substr(VPFcookie[i].indexOf(\"=\")+1);\n"
  << "    x=x.replace(/^\\s+|\\s+$/g,\"\");\n"
  << "    if (x==c_name)\n"
  << "      return unescape(y);\n"
  << "  }\n"
  << "  return \"\";\n"
  << "}\n"
  << "\n"
  << "function addCookie(theName, theValue, exdays, secure)\n"
  << "{ exdate= new Date();\n"
  << "  exdate.setDate(exdate.getDate() + exdays);\n"
  << "  c_value=escape(theValue) + ((exdays==null) ? \"\" : \"; expires=\"+exdate.toUTCString());\n"
  << "  if(secure)\n"
  << "    c_value+=\"; Secure\"; \n"
  << "  document.cookie=theName + \"=\" + c_value+\"; Path=/;\";\n"
  << "}\n";
  return out.str();
}

std::string HtmlRoutines::GetJavascriptTopicListWithTags()
{ MacroRegisterFunctionWithName("HtmlRoutines::GetJavascriptTopicListWithTags");
  std::stringstream out, commentsOnFailure;
  std::string fileReader;
  if (!FileOperations::LoadFileToStringVirtual("html-common-calculator/topiclist.js", fileReader, commentsOnFailure))
    out << "Javascript file topiclist.js not found. ";
  else
    out << "<script type=\"text/javascript\">" << fileReader << "</script>";
//  HtmlRoutines::JavascriptAceEditorScript=out.str();
//  return HtmlRoutines::JavascriptAceEditorScript;
  return out.str();


}

std::string HtmlRoutines::GetJavascriptCanvasGraphicsWithTags()
{ MacroRegisterFunctionWithName("HtmlRoutines::GetJavascriptCanvasGraphicsWithTags");
  std::stringstream out, commentsOnFailure;
  std::string fileReader;
  if (!FileOperations::LoadFileToStringVirtual("html-common-calculator/three-d.js", fileReader, commentsOnFailure))
    out << "Javascript file three-d.js not found. ";
  else
    out << "<script type=\"text/javascript\">" << fileReader << "</script>";
//  HtmlRoutines::JavascriptAceEditorScript=out.str();
//  return HtmlRoutines::JavascriptAceEditorScript;
  return out.str();
}

std::string HtmlRoutines::GetJavascriptVariable(const std::string& theVar)
{ std::stringstream sanitizer;
  for (unsigned i=0; i<theVar.size(); i++)
  { if (MathRoutines::isALatinLetter(theVar[i]))
    { sanitizer << theVar[i];
      continue;
    }
    if (i==0)
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
    << "integrity=\"sha256-eGE6blurk5sHj+rmkfsGYeKyZx3M4bG+ZlFyA7Kns7E=\" crossorigin=\"anonymous\"></script>\n"
    ;
  else
    out
    << "<link rel=\"stylesheet\" href=\"/html-common-calculator/jquery-ui.css\">\n"
    << "<script src=\"/html-common-calculator/jquery.min.js\"></script>\n"
    << "<script src=\"/html-common-calculator/jquery-ui.min.js\"></script>\n"
;

  return out.str();
}

std::string HtmlRoutines::GetHtmlLinkFromProjectFileName(const std::string& fileName, const std::string& fileDesc, int line)
{ std::stringstream out;
  out << " <a href=\"https://sourceforge.net/p/vectorpartition/code/HEAD/tree/trunk/src/"
  << FileOperations::GetFileNameFromFileNameWithPath(fileName);
  if (line>0)
    out << "#l" << line;
  out << "\">" << FileOperations::GetFileNameFromFileNameWithPath(fileName);
  if (fileDesc!="")
    out << " (" << fileDesc << ")";
  out << "</a>\n";
  return out.str();
}

std::string HtmlRoutines::GetLatexEmbeddableLinkFromCalculatorInput(const std::string& address, const std::string& display)
{ std::stringstream out;
  out << "\\href{" << GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable << "?";
  for (unsigned i=0; i<address.size(); i++)
  { //if (input[i]=='&')
     // out << "\\&";
    //else
    if (address[i]=='%')
      out << "\\%";
    else if (address[i]=='_')
      out << "\\_";
    else
      out << address[i];
  }
  out << "}{";
  for (unsigned i=0; i<display.size(); i++)
  { //if (input[i]=='&')
     // out << "\\&";
    //else
    if (display[i]=='%')
      out << "\\%";
    else if (display[i]=='_')
      out << "\\_";
    else if (display[i]=='{')
      out << "\\{";
    else if (display[i]=='}')
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
  if (out.str().size()<(unsigned) upperNumChars && upperNumChars>0)
    return HtmlRoutines::GetMathMouseHover(out.str(), upperNumChars);
  else
    return HtmlRoutines::GetMathMouseHover(input, upperNumChars);
}

std::string HtmlRoutines::GetMathSpanBeginArrayL(const std::string& input, int upperNumChars)
{ std::stringstream out;
  out << "\\begin{array}{l} " << input << " \\end{array}";
  if (out.str().size()<(unsigned) upperNumChars && upperNumChars>0)
    return HtmlRoutines::GetMathSpanPure(out.str(), upperNumChars);
  else
    return HtmlRoutines::GetMathSpanPure(input, upperNumChars);
}

std::string HtmlRoutines::StyleSheetCalculatorWithTags;
std::string HtmlRoutines::StyleSheetMathQuillWithTags;
std::string HtmlRoutines::JavascriptAutoCompleteWithTags;
std::string HtmlRoutines::JavascriptSha1;
std::string HtmlRoutines::JavascriptMathjax;
std::string HtmlRoutines::JavascriptMathQuillMatrixSupport;
std::string HtmlRoutines::JavascriptMathQuillDefault;
std::string HtmlRoutines::JavascriptInitializeButtons;
std::string HtmlRoutines::JavascriptAceEditorScript;
std::string HtmlRoutines::JavascriptCalculatorPage;
std::string HtmlRoutines::JavascriptAccountManagement;

void HtmlRoutines::LoadStrings()
{ HtmlRoutines::GetMathQuillStyleSheetWithTags();
  HtmlRoutines::GetJavascriptInitializeButtons();
  HtmlRoutines::GetJavascriptAceEditorScript();
  HtmlRoutines::GetJavascriptCalculatorPage();
  HtmlRoutines::GetJavascriptAccountManagementWithTags();
  HtmlRoutines::GetCalculatorStyleSheetWithTags();
  HtmlRoutines::GetJavascriptAutocompleteWithTags();
  HtmlRoutines::GetJavascriptSha1();
  HtmlRoutines::GetJavascriptMathjax();
  HtmlRoutines::GetJavascriptMathQuillDefault();
  HtmlRoutines::GetJavascriptMathQuillMatrixSupport();
}

extern logger theLog;

std::string& HtmlRoutines::GetJavascriptCalculatorPage()
{ MacroRegisterFunctionWithName("HtmlRoutines::GetJavascriptCalculatorPage");
  if (theGlobalVariables.flagCachingInternalFilesOn)
    if (HtmlRoutines::JavascriptCalculatorPage!="")
      return HtmlRoutines::JavascriptCalculatorPage;
  std::stringstream out, commentsOnFailure;
  std::string fileReader;
  bool found=true;
  if (!FileOperations::LoadFileToStringVirtual("html-common-calculator/calculatorPage.js", fileReader, commentsOnFailure))
  { theLog << logger::red  << "Javascript file calculatorPage.js not found. " << logger::endL;
    found=false;
  }
  if (found)
    out << "<script type=\"text/javascript\">" << fileReader << "</script>";
  HtmlRoutines::JavascriptCalculatorPage=out.str();
  return HtmlRoutines::JavascriptCalculatorPage;
}

std::string& HtmlRoutines::GetJavascriptAccountManagementWithTags()
{ if (theGlobalVariables.flagCachingInternalFilesOn)
    if (HtmlRoutines::JavascriptAccountManagement!="")
      return HtmlRoutines::JavascriptAccountManagement;
  std::stringstream out, commentsOnFailure;
  std::string fileReader;
  bool found=true;
  if (!FileOperations::LoadFileToStringVirtual("html-common-calculator/account-management.js", fileReader, commentsOnFailure))
  { theLog << logger::red  << "Javascript file calculatorPage.js not found. " << logger::endL;
    found=false;
  }
  if (found)
    out << "<script type=\"text/javascript\">" << fileReader << "</script>";
  HtmlRoutines::JavascriptAccountManagement=out.str();
  return HtmlRoutines::JavascriptAccountManagement;
}

std::string& HtmlRoutines::GetJavascriptAceEditorScript()
{ if (theGlobalVariables.flagCachingInternalFilesOn)
    if (HtmlRoutines::JavascriptAceEditorScript!="")
      return HtmlRoutines::JavascriptAceEditorScript;
  std::stringstream out, commentsOnFailure;
  std::string fileReader;
  bool found=true;
  if (!FileOperations::LoadFileToStringVirtual("html-common-calculator/ace.min.js", fileReader, commentsOnFailure))
  { theLog << logger::red  << "Javascript file ace.min.js not found. " << logger::endL;
    found=false;
  }
  if (found)
    out << "<script type=\"text/javascript\" src=\"/html-common-calculator/ace/src-min/ace.js\" charset=\"utf-8\"></script>";
  else
    out << "<script src=\"https://cdn.jsdelivr.net/ace/1.2.3/min/ace.js\""
    << " type=\"text/javascript\" charset=\"utf-8\"></script>\n";
  HtmlRoutines::JavascriptAceEditorScript=out.str();
  return HtmlRoutines::JavascriptAceEditorScript;
}

std::string& HtmlRoutines::GetJavascriptInitializeButtons()
{ if (theGlobalVariables.flagCachingInternalFilesOn)
    if (HtmlRoutines::JavascriptInitializeButtons!="")
      return HtmlRoutines::JavascriptInitializeButtons;
  std::stringstream out, commentsOnFailure;
  std::string fileReader;
  out << "<script type=\"text/javascript\">";
  bool found=true;
  if (!FileOperations::LoadFileToStringVirtual("html-common-calculator/initializebuttons.js", fileReader, commentsOnFailure))
  { theLog << logger::red  << "Javascript file initializebuttons.js not found. " << logger::endL;
    found=false;
  }
  if (found)
    out << fileReader;
  out << "</script>";
  HtmlRoutines::JavascriptInitializeButtons=out.str();
  return HtmlRoutines::JavascriptInitializeButtons;
}

std::string& HtmlRoutines::GetMathQuillStyleSheetWithTags()
{ if (theGlobalVariables.flagCachingInternalFilesOn)
    if (HtmlRoutines::StyleSheetMathQuillWithTags!="")
      return HtmlRoutines::StyleSheetMathQuillWithTags;
  std::stringstream out, commentsOnFailure;
  std::string fileReader;
  out << "<style>";
  if (!FileOperations::LoadFileToStringVirtual("html-common-calculator/mathquill.css", fileReader, commentsOnFailure))
  { theLog << logger::red  << "Style file mathquill.css is missing. " << logger::endL;
    out << "Style file mathquill.css is missing. ";
  } else
    out << fileReader;
  out << "</style>";
  HtmlRoutines::StyleSheetMathQuillWithTags=out.str();
  return HtmlRoutines::StyleSheetMathQuillWithTags;
}

std::string& HtmlRoutines::GetCalculatorStyleSheetWithTags()
{ if (theGlobalVariables.flagCachingInternalFilesOn)
    if (HtmlRoutines::StyleSheetCalculatorWithTags!="")
      return HtmlRoutines::StyleSheetCalculatorWithTags;
  std::stringstream out, commentsOnFailure;
  std::string fileReader;
  out << "<style>";
  if (!FileOperations::LoadFileToStringVirtual("html-common-calculator/styleCalculator.css", fileReader, commentsOnFailure))
    out << commentsOnFailure.str();
  else
    out << fileReader;
  out << "</style>";
  HtmlRoutines::StyleSheetCalculatorWithTags=out.str();
  return HtmlRoutines::StyleSheetCalculatorWithTags;
}

std::string& HtmlRoutines::GetJavascriptMathQuillDefault()
{ if (theGlobalVariables.flagCachingInternalFilesOn)
    if (HtmlRoutines::JavascriptMathQuillDefault!="")
      return HtmlRoutines::JavascriptMathQuillDefault;
  std::stringstream commentsOnError;
  std::string theJS;
  if (!FileOperations::LoadFileToStringVirtual("html-common-calculator/mathquill.min.js", theJS, commentsOnError))
  { HtmlRoutines::JavascriptMathQuillDefault=commentsOnError.str();
    return HtmlRoutines::JavascriptMathQuillDefault;
  }
  std::stringstream out;
  out << "<script src=\"/html-common-calculator/jquery.min.js\"></script>\n"
  << "<script type=\"text/javascript\">" << theJS << "</script>\n"
  << "<script type=\"text/javascript\">var globalMQ = MathQuill.getInterface(2);</script>";
  HtmlRoutines::JavascriptMathQuillDefault=out.str();
  return HtmlRoutines::JavascriptMathQuillDefault;
}

std::string& HtmlRoutines::GetJavascriptMathQuillMatrixSupport()
{ if (theGlobalVariables.flagCachingInternalFilesOn)
    if (HtmlRoutines::JavascriptMathQuillMatrixSupport!="")
      return HtmlRoutines::JavascriptMathQuillMatrixSupport;
  std::stringstream commentsOnError;
  std::string theJS;
  if (!FileOperations::LoadFileToStringVirtual("html-common-calculator/mathquill.min-matrix.js", theJS, commentsOnError))
  { HtmlRoutines::JavascriptMathQuillMatrixSupport=commentsOnError.str();
    return HtmlRoutines::JavascriptMathQuillMatrixSupport;
  }
  std::stringstream out;
  out << "<script src=\"/html-common-calculator/jquery.min.js\"></script>\n"
  << "<script type=\"text/javascript\">" << theJS << "</script>\n"
  << "<script type=\"text/javascript\">var globalMQ = MathQuill.getInterface(2);</script>";
  HtmlRoutines::JavascriptMathQuillMatrixSupport=out.str();
  return HtmlRoutines::JavascriptMathQuillMatrixSupport;
}

std::string& HtmlRoutines::GetJavascriptAutocompleteWithTags()
{ if (theGlobalVariables.flagCachingInternalFilesOn)
    if (HtmlRoutines::JavascriptAutoCompleteWithTags!="")
      return HtmlRoutines::JavascriptAutoCompleteWithTags;
  std::stringstream out;
  std::string theJS;
  if (!FileOperations::LoadFileToStringVirtual("html-common-calculator/autocomplete.js", theJS, out))
    HtmlRoutines::JavascriptAutoCompleteWithTags=out.str();
  else
    HtmlRoutines::JavascriptAutoCompleteWithTags= "<script type=\"text/javascript\">" + theJS + "</script>\n";
  return HtmlRoutines::JavascriptAutoCompleteWithTags;
}

std::string& HtmlRoutines::GetJavascriptSha1()
{ if (theGlobalVariables.flagCachingInternalFilesOn)
    if (HtmlRoutines::JavascriptSha1!="")
      return HtmlRoutines::JavascriptSha1;
  std::stringstream out;
  std::string theJS;
  if (!FileOperations::LoadFileToStringVirtual("html-common-calculator/sha1.js", theJS, out))
    HtmlRoutines::JavascriptSha1=out.str();
  else
    HtmlRoutines::JavascriptSha1= "<script type=\"text/javascript\">" + theJS + "</script>\n";
  return HtmlRoutines::JavascriptSha1;
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
  static int globalLinkCounter=0;
  int linkCounter=++ globalLinkCounter; //<-Using linkCounter should be thread safer than using globalLinkCounter.
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
  if (input.size()> (unsigned) upperNumChars && upperNumChars>0)
  { out << "<b>LaTeX output is longer than " << upperNumChars << " characters and I dare not use mathjax. Here is the output as plain LaTeX.</b> " << input;
    return out.str();
  }
  out << "\\(\\displaystyle " << input << "\\)";
  return out.str();
}

std::string HtmlRoutines::GetMathMouseHover(const std::string& input, int upperNumChars)
{ std::stringstream out;
  if (input.size()> (unsigned) upperNumChars)
  { out << "<b>LaTeX output is longer than " << upperNumChars
    << " characters and I dare not process the LaTeX. Here is the output as plain (LaTeX-able) text.</b> " << input;
    return out.str();
  }
//  std::stringstream idSpanStream;
  HtmlRoutines::GlobalMathSpanID++;
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
  if (recursionDepth>50)
    return input;
  MapLisT<std::string, std::string, MathRoutines::hashString> currentMap;
  std::stringstream notUsed;
  if (!HtmlRoutines::ChopCGIString(input, currentMap, notUsed))
  { //stOutput << "oh no: " << notUsed.str();
    return input;
  }
  std::stringstream out;
  out << "<table border=\"1px solid black;\">";
  for (int i=0; i<currentMap.size(); i++)
  { out << "<tr>";
    out << "<td>"
    << HtmlRoutines::ConvertURLStringToNormal(currentMap.theKeys[i], false) << " </td>";
    if (currentMap[i]!="")
    { out << "<td>=</td><td>";
      if (currentMap[i]!="")
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
  int inputSize=(signed) input.size();
  for (int i=0; i<inputSize; i++)
  { readAhead="";
    for (int j=0; j<6; j++)
    { if (i+j<inputSize)
        readAhead.push_back(input[i+j]);
      if (HtmlRoutines::URLStringToNormalOneStep(readAhead, out, replacePlusBySpace))
      { i+=j;
        break;
      }
    }
  }
  output=out.str();
}

std::string& HtmlRoutines::GetJavascriptMathjax()
{ if (HtmlRoutines::JavascriptMathjax!="")
    return HtmlRoutines::JavascriptMathjax;
  std::stringstream out;
  if (theGlobalVariables.flagAceIsAvailable)
    out << "<script type=\"text/javascript\" async src=\"/MathJax-2.6-latest/MathJax.js?config=TeX-AMS_HTML-full,local/local\"></script>\n";
  else
  { out
    << "<script type=\"text/x-mathjax-config\">\n"
    << "function selectElementText(el, win) {\n"
    << "  win = win || window;\n"
    << "  var doc = win.document, sel, range;\n"
    << "  if (win.getSelection && doc.createRange) {\n"
    << "    sel = win.getSelection();\n"
    << "    range = doc.createRange();\n"
    << "    range.selectNodeContents(el);\n"
    << "    sel.removeAllRanges();\n"
    << "    sel.addRange(range);\n"
    << "  } else if (doc.body.createTextRange) {\n"
    << "    range = doc.body.createTextRange();\n"
    << "    range.moveToElementText(el);\n"
    << "    range.select();\n"
    << "  }\n"
    << "}"
    << "function showTex(originalTex, item, event){\n  "
    << "  if (item['calculatorTexShown']==undefined)\n"
    << "    item['calculatorTexShown']=false;\n"
    << "  item['calculatorTexShown']=!item['calculatorTexShown'];\n"
    << "  if (!item['calculatorTexShown']){\n"
    << "    item.parentNode.removeChild(item.nextSibling);\n"
    << "    return;\n"
    << "  }\n"
    << "  flaSpan=document.createElement('span');\n"
    << "  flaSpan.textContent=originalTex;\n"
    << "  item.parentNode.insertBefore(flaSpan, item.nextSibling);\n"
    << "  selectElementText(flaSpan);\n"
    //<< "      MathJax.Menu.ShowSource.Text(originalTex||\"No Original Source to Show\",event);\n"
    << "}\n  "
    << "//mathjax configuration comes before loading the mathjax script, as requested by the documentation.\n"
    << "MathJax.Hub.Config({\n"
    << "  extensions: [\"tex2jax.js\""
    << "],\n"
    << "  jax: [\"input/TeX\", \"output/HTML-CSS\"],\n"
    << "  tex2jax: {\n"
    << "    inlineMath: [ ['\\\\(','\\\\)'] ],\n"
    << "    displayMath: [ ['\\\\[','\\\\]'] ],\n"
    << "    processClass: \"calculator|calculatorStudentAnswer|calculatorAnswer\",\n"
    << "    processEscapes: true\n"
    << "  },\n"
    << "  \"HTML-CSS\": { availableFonts: [\"TeX\"] },\n"
    << " TeX: {\n"
    << "    Macros: {\n"
    << "      diff: \"{\\\\text {d}}\",\n"
    << "    },\n"
    << "    MAXBUFFER: 100000\n"
    << "  },\n"
    << "jsMath2jax: {\n"
    << "    preview: \"TeX\"\n"
    << "  },\n"
    << "  });\n"
    << "\n"
    << "MathJax.Hub.Register.StartupHook(\"MathEvents Ready\", function () {\n"
    << "  var HUB = MathJax.Hub, MENUSETTINGS = HUB.config.menuSettings;\n"
    << "  var EVENT = MathJax.Extension.MathEvents.Event;\n"
    << "  var MENU = MathJax.Menu;\n"
    << "  //\n"
    << "  //  Override the double-click function with our own.\n"
    << "  //  It calls on the MathJax.Menu code, so make sure that is loaded and\n"
    << "  //  if not, load it and call the double-click handler again afterward.\n"
    << "  //\n"
    << "  EVENT.DblClick = function(event) {\n"
    //<< "    alert('here be i');"
    << "    if (MENU) {\n"
    << "      var jax = HUB.getJaxFor(this);\n"
    << "      showTex(jax.originalText, this, event);\n"
    << "    } else {\n"
    << "      MathJax.Callback.Queue(\n"
    << "        [\"Require\",MathJax.Ajax,\"[MathJax]/extensions/MathMenu.js\"],\n"
    << "        [\"DblClick\",this,{}]\n"
    << "      );\n"
    << "    }\n"
    << "    return EVENT.False(event);\n"
    << "  }\n"
    //<< "  EVENT.Keydown = function(event) {\n"
    //<< "    alert('here be i');"
    //<< "    if (MENU) {\n"
    //<< "      if (event.ctrlKey!=true)\n"
    //<< "        return;\n"
    //  << "     return;\n"
    //<< "      if (event.keyCode!=67)\n"
    //<< "        showTex(jax.originalText, this, event);\n"
    //<< "    } else {\n"
    //<< "      MathJax.Callback.Queue(\n"
    //<< "        [\"Require\",MathJax.Ajax,\"[MathJax]/extensions/MathMenu.js\"],\n"
    //<< "        [\"onkeyup\",this,{}]\n"
    //<< "      );\n"
    //<< "    }\n"
    //<< "    return EVENT.False(event);\n"
    //<< "  }\n"
    << "  //\n"
    << "  //  Make sure we don't have double-click zoom menu item selected\n"
    << "  //  and disable the zoom trigger menu item.\n"
    << "  //  Also set the MENU item so that it will be available above.\n"
    << "  //\n"
    << "  if (MENUSETTINGS.zoom === \"Double-Click\") MENUSETTINGS.zoom = \"None\";\n"
    << "  MathJax.Hub.Register.StartupHook(\"MathMenu Ready\", function () {\n"
    << "    MENU = MathJax.Menu;\n"
    << "    MENU.menu.Find(\"Math Settings\", \"Zoom Trigger\",\"Double-Click\").disabled = true;\n"
    << "  });\n"
    << "});\n"
    << "</script>\n"
    << "<script src=\"//beta.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS_HTML-full\"></script>"
    ;
  }
  HtmlRoutines::JavascriptMathjax=out.str();
  return HtmlRoutines::JavascriptMathjax;
}

bool HtmlRoutines::AccountOneInputCGIString
(const std::string& fieldName, const std::string& fieldValue, MapLisT<std::string, std::string, MathRoutines::hashString>& outputMap,
 std::stringstream& commentsOnFailure)
{ MacroRegisterFunctionWithName("HtmlRoutines::AccountOneInputCGIString");
  if (fieldName=="")
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
  int inputLength= (signed) input.size();
  bool readingData=false;
  std::string currentFieldName="";
  std::string currentFieldValue="";
  currentFieldName.reserve(input.size());
  currentFieldValue.reserve(input.size());
  for (int i=0; i<inputLength; i++)
  { if (!readingData && input[i]=='=')
    { readingData=true;
      continue;
    }
    if (input[i]!='&')
    { if (readingData)
        currentFieldValue.push_back(input[i]);
      else
        currentFieldName.push_back(input[i]);
      continue;
    }
    if (!HtmlRoutines::AccountOneInputCGIString(currentFieldName, currentFieldValue, outputMap, commentsOnFailure))
      return false;
    currentFieldName="";
    currentFieldValue="";
    readingData=false;
  }
  return HtmlRoutines::AccountOneInputCGIString(currentFieldName, currentFieldValue, outputMap, commentsOnFailure);
}

bool HtmlRoutines::URLStringToNormalOneStep(std::string& readAhead, std::stringstream& out, bool replacePlusBySpace)
{ if (replacePlusBySpace)
    if (readAhead[0]=='+')
      { out << " ";
        return true;
      }
  bool isOK=readAhead[0]!='%' && readAhead[0]!='&';
  if (isOK)
  { out << readAhead[0];
    return true;
  }
  if (readAhead=="&")
  { out << " ";
    return true;
  }
  if (readAhead.size()==3)
    if (readAhead[0]=='%' && MathRoutines::IsAHexDigit(readAhead[1])
        && MathRoutines::IsAHexDigit(readAhead[2]))
    { out << (char)(MathRoutines::ConvertHumanReadableHexToCharValue(readAhead[1])*16+
      MathRoutines::ConvertHumanReadableHexToCharValue(readAhead[2]));
      return true;
    }
  return false;
}
