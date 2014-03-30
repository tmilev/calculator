//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".

#include "vpfHeader1General0_General.h"
ProjectInformationInstance projectInfoInstanceCalculatorHTML(__FILE__, "Calculator html routines. ");

void PredefinedStrings(std::string& inputOutputCivilizedString)
{ if (inputOutputCivilizedString=="PenkovProject")
    inputOutputCivilizedString="experimentalPrintSemisimpleSubalgebras{}(F_4)";
  if(inputOutputCivilizedString=="PenkovProjectRecomputeTest")
    inputOutputCivilizedString="%HideLHS \
    experimentalPrintSemisimpleSubalgebrasFull{}Load{}Serialization{}(LoadSemisimpleSubalgebras, (F)_{4}, \
    ( \n\
Serialization{}(LoadCandidateSubalgebra, A^{6}_{1}, {Sequence{}((4, 8, 12, 6))}, (\n\
g_{-9}+g_{-17}+g_{-18}, 2 (g_{18})+2 (g_{9})+g_{17}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 2 A^{6}_{1}, ((4, 8, 12, 6), (2, 2, 0, 2)), (\n\
g_{-15}+2 g_{-16}+g_{-14}, g_{15}+2 g_{14}+g_{16}, g_{-1}- g_{-2}- g_{-4}, -2 g_{2}+2 g_{1}- g_{4}\n\
))\n\
))";
  if (inputOutputCivilizedString=="PenkovProjectRecompute")
    inputOutputCivilizedString= "\
%HideLHS \n\
experimentalPrintSemisimpleSubalgebrasFull{}Load{}(\n\
Serialization{}(LoadSemisimpleSubalgebras, F_{4}, (\n\
Serialization{}(LoadCandidateSubalgebra, A^{156}_{1}, {Sequence{}((22, 42, 60, 32))}, (\n\
g_{-1}+g_{-2}+g_{-3}+g_{-4}, 42 (g_{2})+30 (g_{3})+22 (g_{1})+16 (g_{4})\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{60}_{1}, {Sequence{}((14, 26, 36, 20))}, (\n\
g_{-1}+g_{-2}+g_{-7}+g_{-9}, 18 (g_{2})+14 (g_{1})+10 (g_{7})+8 (g_{9})\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{36}_{1}, {Sequence{}((10, 20, 28, 16))}, (\n\
g_{-2}+g_{-4}+g_{-6}+g_{-11}, 10 (g_{11})+8 (g_{4})+4 (g_{7})+4 (g_{6})+2 (g_{8})+2 (g_{2})-4 (g_{5})\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{12}_{1}, {Sequence{}((6, 12, 16, 8))}, (\n\
g_{-2}+g_{-9}+g_{-11}+g_{-18}, 4 (g_{18})+4 (g_{2})+2 (g_{11})+2 (g_{9})-2 (g_{16})-2 (g_{5})\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{28}_{1}, {Sequence{}((10, 18, 24, 12))}, (\n\
2 g_{-16}+2 g_{-1}+2 g_{-6}, 3 g_{6}+5 g_{1}+3 g_{16}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{4}_{1}, {Sequence{}((4, 6, 8, 4))}, (\n\
g_{-14}+g_{-18}, 2(g_{14}+g_{18})\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{35}_{1}, {Sequence{}((10, 19, 28, 16))}, (\n\
g_{-4}+g_{-8}+g_{-9}, 9 (g_{9})+8 (g_{4})+5 (g_{8})\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{11}_{1}, {Sequence{}((6, 11, 16, 8))}, (\n\
g_{-8}+g_{-9}+g_{-16}, 4 (g_{16})+3 (g_{8})+g_{9}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{10}_{1}, {Sequence{}((6, 10, 14, 8))}, (\n\
2 g_{-16}+g_{-8}, 3 g_{8}+2 g_{16}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{9}_{1}, {Sequence{}((5, 10, 14, 8))}, (\n\
g_{-10}+g_{-14}+g_{-15}, 2 (g_{15})+2 (g_{10})+g_{14}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{6}_{1}, {Sequence{}((4, 8, 12, 6))}, (\n\
g_{-9}+g_{-17}+g_{-18}, 2 (g_{18})+2 (g_{9})+g_{17}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{3}_{1}, {Sequence{}((3, 6, 8, 4))}, (\n\
g_{-21}+g_{-14}, g_21+g_{14}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{8}_{1}, {Sequence{}((4, 8, 12, 8))}, (\n\
g_{-7}+g_{-17}, 2 (g_{17})+2 (g_{7})\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}, {Sequence{}((2, 4, 6, 4))}, (\n\
g_{-21}, g_{21}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{1}_{1}, {Sequence{}((2, 3, 4, 2))}, (\n\
g_{-24}, g_{24}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 2 A^{1}_{1}, ((2, 3, 4, 2), (0, 1, 2, 2)), (\n\
g_{-24}, g_{24}, g_{-16}, g_{16}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{1}_{1}+A^{2}_{1}, ((2, 3, 4, 2), (0, 2, 4, 2)), (\n\
g_{-24}, g_{24}, g_{-13}, g_{13}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 2 A^{2}_{1}, ((2, 4, 6, 4), (2, 2, 2, 0)), (\n\
g_{-21}, g_{21}, g_{-5}+g_{-11}, g_{11}+g_{5}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{1}_{1}+A^{3}_{1}, ((2, 3, 4, 2), (0, 3, 4, 2)), (\n\
g_{-24}, g_{24}, g_{-9}+g_{-10}, g_{10}+g_{9}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+A^{3}_{1}, ((2, 4, 6, 4), (2, 3, 4, 0))),\n\
Serialization{}(LoadCandidateSubalgebra, 2 A^{3}_{1}, ((3, 6, 8, 4), (1, 0, 2, 2))),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+A^{4}_{1}, ((2, 4, 6, 4), (2, 4, 4, 0)), (\n\
g_{-21}, g_{21}, g_{-2}+g_{-11}, 2 g_{11}+2 g_{2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 2 A^{6}_{1}, ((4, 8, 12, 6), (2, 2, 0, 2)), (\n\
g_{-15}+2 g_{-16}+g_{-14}, g_{15}+2 g_{14}+g_{16}, g_{-1}- g_{-2}- g_{-4}, -2 g_{2}+2 g_{1}- g_{4}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{1}_{1}+A^{8}_{1}, ((2, 3, 4, 2), (0, 4, 8, 4)), (\n\
g_{-24}, g_{24}, g_{-6}+g_{-7}, 2 g_{7}+2 g_{6}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{3}_{1}+A^{8}_{1}, ((3, 6, 8, 4), (0, 0, 4, 4)), (\n\
g_{-19}+g_{-20}, g_{20}+g_{19}, -g_{-4}+g_{-3}, 2 g_{3}-2 g_{4}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{4}_{1}+A^{8}_{1}, ((4, 6, 8, 4), (0, 4, 8, 4)), (\n\
g_{-18}- g_{-20}- g_{-11}- g_{-14}, g_{18}- g_{11}- g_{20}- g_{14}, g_{-10}+g_{-7}- g_{-6}+g_{-3}, g_{3}+g_{7}- g_{6}+g_{10}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{3}_{1}+A^{9}_{1}, ((3, 6, 8, 4), (1, 0, 4, 4)), (\n\
g_{-19}+g_{-20}, g_{20}+g_{19}, g_{-3}- g_{-4}+g_{-1}, g_{1}+2 g_{3}-2 g_{4}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{1}_{1}+A^{10}_{1}, ((2, 3, 4, 2), (0, 4, 8, 6)), (\n\
g_{-24}, g_{24}, 2g_{-9}+g_{-4}, 3g_{4}+2g_{9}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+A^{10}_{1}, ((2, 4, 6, 4), (4, 6, 6, 0)), (\n\
g_{-20}+g_{-21}, -g_{22}+g_{21}, g_{-6}+g_{-2}+2g_{-9}+g_{-1}, g_{6}+g_{9}+4g_{1}+2g_{2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{1}_{1}+A^{11}_{1}, ((2, 3, 4, 2), (0, 5, 8, 6)), (\n\
g_{-24}, g_{24}, g_{-6}+g_{-2}+3 g_{-7}-3/4 g_{-9}, g_{4}+g_{7}+4g_{9}+4 g_{6}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+A^{28}_{1}, ((2, 4, 6, 4), (6, 10, 12, 0))),\n\
Serialization{}(LoadCandidateSubalgebra, A^{8}_{1}+A^{28}_{1}, ((4, 8, 12, 8), (6, 10, 12, 0)), (\n\
g_{-12}+g_{-13}, 2 g_{13}+2 g_{12}, g_{-2}- g_{-3}+g_{-1}, -6 g_{3}+10 g_{2}+6 g_{1}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{1}_{1}+A^{35}_{1}, ((2, 3, 4, 2), (0, 9, 16, 10)), (\n\
g_{-24}, g_{24}, g_{-4}+3g_{-2}+2g_{-3}, 3g_{2}+5 g_{4}+4g_{3}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{1}_{2}, ((2, 3, 4, 2), (-1, 0, 0, 0)), (\n\
g_{-24}, g_24, g_1, g_-1\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{2}, ((2, 4, 6, 4), (0, 0, 0, -2)), (\n\
g_{-21}, g_21, g_4, g_{-4}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{3}_{2}, ((3, 6, 8, 4), (-1, -3, -2, 0)), (\n\
g_{-19}+g_{-20}, g_{20}+g_{19}, g_{6}+g_{5}, g_{-5}+g_{-6}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{3}_{2}, ((3, 6, 8, 4), (0, -3, -4, -2)), (\n\
g_{-21}+g_{-19}+g_{-20}, g_{19}+g_{20}- g_{17}- g_{14}, g_{9}+g_{16}- g_{6}-2 g_{2}, g_{-16}+g_{-6}+2 g_{-9}- g_{-2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, B^{1}_{2}, ((2, 3, 4, 2), (-1, -1, -1, 0)), (\n\
g_{-24}, g_{24}, g_{8}, g_{-8}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, B^{3}_{2}, ((3, 6, 8, 4), (-1, -3, -3, -1))),\n\
Serialization{}(LoadCandidateSubalgebra, G^{3}_{2}, ((3, 6, 8, 4), (-3, -9, -12, -6)), (\n\
g_{-19}+g_{-20}, g_{20}+g_{19}, g_{23}, g_{-23}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 3 A^{1}_{1}, ((2, 3, 4, 2), (0, 1, 2, 2), (0, 1, 2, 0)), (\n\
g_{-24}, g_{24}, g_{-16}, g_{16}, g_{-9}, g_{9}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 2 A^{1}_{1}+A^{2}_{1}, ((2, 3, 4, 2), (0, 1, 2, 2), (0, 2, 2, 0)), (\n\
g_{-24}, g_{24}, g_{-16}, g_{16}, g_{-6}, g_{6}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 3 A^{2}_{1}, ((2, 4, 6, 4), (2, 2, 2, 0), (0, 2, 2, 0)), (\n\
g_{-21}, g_{21}, g_{-5}+g_{-11}, g_{11}+g_{5}, -g_{-9}+g_{-2}, -g_{9}+g_{2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{1}_{1}+A^{2}_{1}+A^{3}_{1}, ((2, 3, 4, 2), (0, 2, 4, 2), (0, 1, 0, 2))),\n\
Serialization{}(LoadCandidateSubalgebra, A^{1}_{1}+A^{3}_{1}+A^{8}_{1}, ((2, 3, 4, 2), (0, 3, 4, 2), (0, 0, 4, 4)), (\n\
g_{-24}, g_{24}, g_{-9}+g_{-10}, g_{10}+g_{9}, g_{-3}+g_{-4}, 2 g_{4}+2 g_{3}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 2A^{1}_{1}+A^{10}_{1}, ((2, 3, 4, 2), (0, 1, 2, 2), (0, 4, 6, 0)), (\n\
g_{-24}, g_{24}, g_{-16}, g_{16}, 2g_{-2}+g_{-3}, 3g_{3}+2 g_{2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+A^{1}_{2}, ((2, 4, 6, 4), (1, 2, 2, 0), (0, -1, 0, 0)), (\n\
g_{-21}, g_{21}, g_{-14}, g_{14}, g_{2}, g_{-2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{8}_{1}+A^{1}_{2}, ((4, 8, 12, 8), (1, 2, 2, 0), (0, -1, 0, 0)), (\n\
g_{-12}+g_{-13}, 2 g_{13}+2 g_{12}, g_{-14}, g_{14}, g_{2}, g_{-2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{1}_{1}+A^{2}_{2}, ((2, 3, 4, 2), (0, 2, 4, 2), (0, 0, -2, 0)), (\n\
g_{-24}, g_{24}, g_{-13}, g_{13}, g_{3}, g_{-3}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{4}_{1}+A^{2}_{2}, ((4, 6, 8, 4), (0, 2, 4, 2), (0, 0, -2, 0)), (\n\
g_{-14}+g_{-18}, 2 g_{18}+2 g_{14}, g_{-13}, g_{13}, g_{3}, g_{-3}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{1}_{1}+B^{1}_{2}, ((2, 3, 4, 2), (0, 1, 2, 2), (0, 0, 0, -1)), (\n\
g_{-24}, g_{24}, g_{-16}, g_{16}, g_{4}, g_{-4}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+B^{1}_{2}, ((2, 4, 6, 4), (1, 2, 2, 0), (0, -1, -1, 0)), (\n\
g_{-21}, g_{21}, g_{-14}, g_{14}, g_{9}+g_{2}, g_{-2}+g_{-9}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+G^{3}_{2}, ((2, 4, 6, 4), (2, 3, 4, 0), (-3, -3, -6, 0))),\n\
Serialization{}(LoadCandidateSubalgebra, A^{8}_{1}+G^{3}_{2}, ((4, 8, 12, 8), (2, 3, 4, 0), (-3, -3, -6, 0)), (\n\
g_{-12}+g_{-13}, 2g_{13}+2g_{12}, -g_{-9}+g_{-8}, -g_{9}+g_{8}, g_{11}, g_{-11}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{1}_{3}, ((2, 3, 4, 2), (-1, 0, 0, 0), (0, -1, 0, 0)), (\n\
g_{-24},  g_{24}, g_{1}, g_{-1}, g_{2}, g_{-2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, B^{1}_{3}, ((2, 3, 4, 2), (-1, 0, 0, 0), (0, -1, -1, 0)), (\n\
g_{-24}, g_{24}, g_{1}, g_{-1}, g_{6}, g_{-6}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, C^{2}_{3}, ((2, 4, 6, 4), (0, 0, 0, -2), (0, -2, -4, 0)), (\n\
g_{-21}, g_{21}, g_{4}, g_{-4}, g_{9}, g_{-9}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 4A^{1}_{1}, ((2, 3, 4, 2), (0, 1, 2, 2), (0, 1, 2, 0), (0, 1, 0, 0)), (\n\
g_{-24}, g_{24}, g_{-16}, g_{16}, g_{-9}, g_{9}, g_{-2}, g_{2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{1}_{2}+A^{2}_{2}, ((2, 3, 4, 2), (-1, 0, 0, 0), (0, 0, 2, 2), (0, 0, 0, -2)), (\n\
g_{-24}, g_{24}, g_{1}, g_{-1}, g_{-7}, g_{7}, g_{4}, g_{-4}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 2A^{1}_{1}+B^{1}_{2}, ((2, 3, 4, 2), (0, 1, 2, 2), (0, 1, 2, 0), (0, 0, -1, 0)), (\n\
g_{-24}, g_{24}, g_{-16}, g_{16}, g_{-9}, g_{9}, g_{3}, g_{-3}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+A^{1}_{3}, ((2, 4, 6, 4), (1, 2, 2, 0), (0, -1, 0, 0), (-1, 0, 0, 0)), (\n\
g_{-21}, g_{21}, g_{-14}, g_{14}, g_{2}, g_{-2}, g_{1}, g_{-1}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{1}_{1}+(C)^{2}_{3}, ((2, 3, 4, 2), (0, 2, 4, 2), (0, 0, -2, 0), (0, -2, 0, 0)), (\n\
g_{-24}, g_{24}, g_{-13}, g_{13}, g_{3}, g_{-3}, g_{2}, g_{-2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, D^{1}_{4}, ((2, 3, 4, 2), (-1, 0, 0, 0), (0, -1, 0, 0), (0, -1, -2, 0)), (\n\
g_{-24}, g_{24}, g_{1}, g_{-1}, g_{2}, g_{-2}, g_{9}, g_{-9}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, B^{1}_{4}, ((2, 3, 4, 2), (-1, 0, 0, 0), (0, -1, 0, 0), (0, 0, -1, 0)), (\n\
g_{-24}, g_{24}, g_{1}, g_{-1}, g_{2}, g_{-2}, g_{3}, g_{-3}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, F^{1}_{4}, ((2, 3, 4, 2), (-1, 0, 0, 0), (0, -1, -1, 0), (0, 0, 0, -1)), (\n\
g_{-24}, g_{24}, g_{1}, g_{-1}, g_{6}, g_{-6}, g_{4}, g_{-4}\n\
))\n\
\n\
))\n\
)\n\
  ";

}

void static_html3(std::stringstream& output)
{ output << " <!>\n";
  output << " <script type=\"text/javascript\"> \n";
  output << " var timeOutCounter=0;\n";
  output << " window.setTimeout(\"progressReport()\",1000);\n";
  output << " var newReportString=\"\";\n";
  output << " function progressReport()\n";
  output << " { timeOutCounter++;\n";
  output << "   var oRequest = new XMLHttpRequest();\n";
  output << "   var sURL  = \"/tmp/report.txt\";\n";
  output << "   oRequest.open(\"GET\",sURL,false);\n";
  output << "   oRequest.setRequestHeader(\"User-Agent\",navigator.userAgent);\n";
  output << "   oRequest.send(null)\n";
  output << "   if (oRequest.status==200)\n";
  output << "   { newReportString= oRequest.responseText;\n";
  output << "     if (newReportString!=\"\")\n";
  output << "       el = document.getElementById(\"idProgressReport\").innerHTML= \"Attempting indicator refresh each second. Number of seconds: \"+ timeOutCounter+\"<br>Status file content:<br>\" +newReportString;\n";
  output << "   }\n";
  output << "   window.setTimeout(\"progressReport()\",1000);\n";
  output << " }\n";
  output << " </script>\n";
  output << " <div id=\"idProgressReport\">\n";
  output << " </div>\n";
  output << " \n";
  output << " \n";
}

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

