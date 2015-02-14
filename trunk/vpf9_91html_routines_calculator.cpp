//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".

#include "vpfHeader1General0_General.h"
ProjectInformationInstance projectInfoInstanceCalculatorHTML(__FILE__, "Calculator html routines. ");


void static_html4(std::stringstream& output)
{ output << " <!>\n";
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
  output << " <script type=\"text/javascript\"> \n";
  output << " function getCookie(c_name)\n";
  output << " { VPFcookie=document.cookie.split(\";\");\n";
  output << "   for (i=0;i<VPFcookie.length;i++)\n";
  output << "   { x=VPFcookie[i].substr(0,VPFcookie[i].indexOf(\"=\"));\n";
  output << "   	y=VPFcookie[i].substr(VPFcookie[i].indexOf(\"=\")+1);\n";
  output << "     x=x.replace(/^\\s+|\\s+$/g,\"\");\n";
  output << "     if (x==c_name)\n";
  output << "       return unescape(y);\n";
  output << "   }\n";
  output << " }\n";
  output << " function storeSettings()\n";
  output << " { theCalculatorForm=document.getElementById(\"textInputID\");  \n";
  output << "   //alert(theCalculatorForm.style.width);\n";
  output << "   addCookie(\"widthCalculatorText\", theCalculatorForm.style.width, 100);  \n";
  output << "   addCookie(\"heightCalculatorText\", theCalculatorForm.style.height, 100);\n";
  output << "   //alert(document.cookie);\n";
  output << " }\n";
  output << " \n";
  output << " function addCookie(theName, theValue, exdays)\n";
  output << " { exdate= new Date();\n";
  output << "   exdate.setDate(exdate.getDate() + exdays);\n";
  output << "   c_value=escape(theValue) + ((exdays==null) ? \"\" : \"; expires=\"+exdate.toUTCString());\n";
  output << "   document.cookie=theName + \"=\" + c_value;\n";
  output << " }\n";
  output << " \n";
  output << " function checkCookie()\n";
  output << " { theCalculatorForm=document.getElementById(\"textInputID\");  \n";
  output << "   theOldWidth=getCookie(\"widthCalculatorText\");\n";
  output << "   theOldHeight=getCookie(\"heightCalculatorText\");\n";
  output << "   //alert(\"height: \" + theOldHeight +\" width: \" + theOldWidth);\n";
  output << "   //theCalculatorForm.setStyle(\"width:\"+ theOldWidth);\n";
  output << " //  theCalculatorForm.style.height=theOldHeight;\n";
  output << "   theCalculatorForm.style.width  = theOldWidth;\n";
  output << "   theCalculatorForm.style.height = theOldHeight;\n";
  output << " }\n";
  output << " </script>\n";
}

