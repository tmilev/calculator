//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader8HtmlSnippets.h"
#include <iomanip>

ProjectInformationInstance projectInfoInstanceHtmlSnippets
(__FILE__, "Html Snippets.");


std::string HtmlSnippets::GetJavascriptSubmitEmails(const std::string& homeFile)
{ std::stringstream out;
  (void) homeFile;
  out
  << "<script type=\"text/javascript\" id=\"scriptSubmitEmails\"> \n"
  << "function addEmailsOrUsers(idEmailList, problemCollectionName, idOutput, userRole, idExtraInfo, requestType){\n"
  << "  spanOutput = document.getElementById(idOutput);\n"
  << "  if (spanOutput==null){\n"
  << "    spanOutput = document.createElement('span');\n"
  << "    document.body.appendChild(spanOutput);\n"
  << "    spanOutput.innerHTML= \"<span style='color:red'> ERROR: span with id \" + idEmailList + \"MISSING! </span>\";\n"
  << "  }\n"
  << "  spanEmailList = document.getElementById(idEmailList);\n"
  << "  spanExtraInfo = document.getElementById(idExtraInfo);\n"
  << "  params='userRole='+userRole;\n"
  << "  params+='&mainInput=' + encodeURIComponent(spanEmailList.value);\n"
  << "  params+='&extraInfo=' + encodeURIComponent(spanExtraInfo.value);\n"
  << "  var https = new XMLHttpRequest();\n"
  << "  https.open(\"POST\", requestType, true);\n"
  << "  https.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\");\n"
  << "  https.onload = function() {\n"
  << "    spanOutput.innerHTML=https.responseText;\n"
//  << "    code=document.getElementById('progressReportJavascript').innerHTML;"
//  << "    eval.call(code);\n"
  //<< "    p();\n"
//  << "    eval(spanOutput.innerHTML);\n"
//  << "    if (typeof progressReport == 'function')\n"
//  << "      progressReport();\n"
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
  << "    c_value+=\"; secure;\"; \n"
  << "  document.cookie=theName + \"=\" + c_value;\n"
  << "}\n";
  return out.str();
}
