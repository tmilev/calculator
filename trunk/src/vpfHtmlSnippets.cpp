//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader8HtmlSnippets.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"
#include "vpfHeader1General7FileOperations_Encodings.h"
#include <iomanip>

ProjectInformationInstance projectInfoInstanceHtmlSnippets
(__FILE__, "Html Snippets.");

std::string HtmlSnippets::GetJavascriptSubmitEmails()
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

std::string HtmlSnippets::GetJavascriptHideHtml()
{ std::stringstream output;
  output << " <!>\n";
  output << " <script type=\"text/javascript\"> \n";
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

std::string HtmlSnippets::GetJavascriptCookieFunctions()
{std::stringstream out;
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

std::string HtmlSnippets::GetDatePickerStart(const std::string& theId)
{ std::stringstream out;
  out << "\n<script type=\"text/javascript\">\n"
  << "$(function() {\n"
  << "$('#" << theId << "').datepicker();\n"
  << "});\n"
  << "</script>\n";
  return out.str();
}

std::string HtmlSnippets::GetJavascriptCanvasGraphics()
{ MacroRegisterFunctionWithName("HtmlSnippets::GetJavascriptCanvasGraphics");
  std::stringstream out, commentsOnFailure;
  std::string fileReader;
  if (!FileOperations::LoadFileToStringVirtual("html-common-calculator/three-d.js", fileReader, commentsOnFailure))
    out << "Javascript file three-d.js not found. ";
  else
    out << "<script type=\"text/javascript\">" << fileReader << "</script>";
//  CGI::JavascriptAceEditorScript=out.str();
//  return CGI::JavascriptAceEditorScript;
  return out.str();
}

std::string HtmlSnippets::GetDatePickerJavascriptInit()
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
