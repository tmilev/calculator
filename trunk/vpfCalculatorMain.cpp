//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include <iostream>
#include "vpfHeader1.h"
#include "vpfHeader2.h"

#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#endif
//#define cgiLimitRAMuseNumPointersInList
#ifndef __DATE__
#define __DATE__ " unknown date "
#endif
ProjectInformationInstance projectInfoInstanceCalculatorCpp(__FILE__, "Calculator web interface.");

void getPath(char* path, std::string& output)
{ if (path==0) return;
  int length=0;
  output= path;
  while (output[length]!=0 && length<150)
    length++;
  for (int i=length-1; i>=0; i--)
  {
#ifdef WIN32
		if (output[i]=='\\' )
    { output.resize(i+1);
      return;
    }
#else
		if (output[i]=='/')
    { output.resize(i+1);
      return;
    }
#endif
  }
}

void ignoreUserAbortSignal()
{
#ifndef WIN32
  signal(SIGABRT,SIG_IGN);
  signal(SIGTERM,SIG_IGN);
#endif
}

void static_html4(std::stringstream& output);
void static_html3(std::stringstream& output);

bool ComputationComplete;

#ifndef WIN32
timeval ComputationStartGlobal, LastMeasureOfCurrentTime;
pthread_t TimerThread;

GlobalVariables theGlobalVariables;
CommandList theParser;

double GetElapsedTimeInSeconds()
{ gettimeofday(&LastMeasureOfCurrentTime, NULL);
  int miliSeconds =(LastMeasureOfCurrentTime.tv_sec- ComputationStartGlobal.tv_sec)*1000+(LastMeasureOfCurrentTime.tv_usec- ComputationStartGlobal.tv_usec)/1000;
  return ((double) miliSeconds)/1000;
}

void* RunTimer(void* ptr)
{ double elapsedtime=-1;
  for (; ;)
  { usleep(100);
    elapsedtime=GetElapsedTimeInSeconds();
    if (ComputationComplete)
      break;
    if (elapsedtime >=theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit && !
        (theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit<=0))
      break;
  }
  if (!ComputationComplete)
  { std::cout << "</div><br><br><br>Your computation has taken "
    << elapsedtime << " seconds so far.";
    std::cout << "<br>The maximum allowed run time for "
    << " the entire system is  "
    << theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit
    << " seconds (twice the amount of time allowed for calculator interpretation). "
    << "<br>This safety limit is hard coded in this particular server. "
    << "<br>However, if you install the calculator on your own machine you may "
    << "<br>allow arbitrarily large execution time by modifying "
    << " the variable theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit"
    << " located in file " << __FILE__ << "<br><b>Signalling ungraceful exit...</b> ";
    std::cout.flush();
    assert(false);
  } else
    pthread_exit(NULL);
}
#endif

std::string IPAdressCaller;

void makeReport(IndicatorWindowVariables& input)
{ static int counter =-1;
  counter++;
//  if (counter%10!=0)
//    return;
  std::fstream theFile;
  CGI::OpenFileCreateIfNotPresent(theFile, theParser.indicatorFileNamE, false, true, false);
  std::stringstream outStream;
  theFile << " Elapsed calculator time: " << GetElapsedTimeInSeconds() << " second(s).";
  for (int i=0; i<input.ProgressReportStringS.size; i++)
    theFile << "\n" << input.ProgressReportStringS[i] << "\n<br>\n";
  theFile.flush();
  theFile.close();
}

void CallSystemWrapper(const std::string& theCommand)
{ system(theCommand.c_str());
}

std::string GetSelectHTMLStringTEmp
(List<std::string>& optionsType, List<std::string>& optionsRank,
  std::string& selectedType, std::string& selectedRank, bool usePreamble)
{ std::stringstream out;
  out << "\n<script language=\"javascript\">\n"
  << "function updatePreamble(){\n"
  << "  if (! document.getElementById(\"checkUsePreambleID\").checked)\n"
  << "  { document.getElementById(\"textPreamble\").style.display = \"none\";\n"
  << "    document.getElementById(\"textTypeID\").disabled= \"disabled\";\n"
  << "    document.getElementById(\"textDimID\").disabled = \"disabled\";\n"
  << "  }"
  << "  else\n"
  << "  { document.getElementById(\"textPreamble\").innerHTML=\"g_{{i}}:= getChevalleyGenerator{}(\"+"
  << " document.getElementById(\"textTypeID\").value+\"_\""
  << "+ document.getElementById(\"textDimID\").value+\", i)"
  << "; h_{{i}}:=getCartanGenerator{}(\""
  << "+document.getElementById(\"textTypeID\").value+\"_\""
  << "+ document.getElementById(\"textDimID\").value+ \", i) ; \"; \n"
  << "    document.getElementById(\"textPreamble\").style.display = '';\n"
  << "    document.getElementById(\"textTypeID\").disabled = '';\n"
  << "    document.getElementById(\"textDimID\").disabled = '';\n"
  << "  }\n"
  << "}\n"
  << "\n</script>\n";
  out << "Use predefined preamble";
  out << "<input type=\"checkbox\"";
  if (usePreamble)
    out << " checked=\"checked\"";
  out << " name=\"checkUsePreamble\" id= \"checkUsePreambleID\"onchange=\"updatePreamble();\"></input>";
  out << "  <select name=\"textType\" id=\"textTypeID\" onchange=\"updatePreamble();\">\n";
  for (int i=0; i<optionsType.size; i++)
  { out << "   <option";
    if (selectedType==optionsType[i])
      out << " selected=\"selected\"";
    out << ">" << optionsType[i] << "</option>\n";
  }
  out << "</select>";
  out << "  <select name=\"textDim\" id=\"textDimID\" onchange=\"updatePreamble();\">\n";
  for (int i=0; i<optionsRank.size; i++)
  { out << "   <option";
    if (selectedRank==optionsRank[i])
      out << " selected=\"selected\"";
    out << ">" << optionsRank[i] << "</option>\n";
  }
  out << " </select>\n";
  out << "<span name=\"textPreamble\" id=\"textPreamble\"> </span>";
  return out.str();
}

int main_command_input(int argc, char **argv)
{ std::stringstream theInputStream;
  for (int i=1; i<argc; i++)
    theInputStream << argv[i] << " ";
  theParser.inputStringRawestOfTheRaw =theInputStream.str();
  theParser.Evaluate(theParser.inputStringRawestOfTheRaw);
  std::cout << theParser.outputString;
  std::cout << "\nTotal running time: " << GetElapsedTimeInSeconds() << " seconds.";
  return 0;
}

int main(int argc, char **argv)
{
#ifndef WIN32
  gettimeofday(&ComputationStartGlobal, NULL);
#endif
  theGlobalVariables.SetFeedDataToIndicatorWindowDefault(&makeReport);
  theGlobalVariables.SetTimerFunction(&GetElapsedTimeInSeconds);
  //Change the below line to modify the computation time of the calculator.
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=500;
  theGlobalVariables.SetCallSystem(&CallSystemWrapper);


	std::cout << "Content-Type: text/html\n\n";

  theParser.init(theGlobalVariables);
  MacroRegisterFunctionWithName("main");

  ParallelComputing::cgiLimitRAMuseNumPointersInList=60000000;
  std::string inputPatH;
  std::string inputDisplayPath="vpf/";
  if (argc>=1)
  { getPath(argv[0], inputPatH);
//    std::cout << "input path: " << inputPatH;

    bool found=false;
    for (int j=inputPatH.size()-2; j>=0; j--)
    { if (inputPatH[j]=='/')
      { if (found)
          break;
        inputDisplayPath="";
        found=true;
      }
      if (found)
        inputDisplayPath.push_back(inputPatH[j]);
    }
    if (found)
      for (unsigned j=0; j<inputDisplayPath.size()/2; j++)
        MathRoutines::swap(inputDisplayPath[j], inputDisplayPath[inputDisplayPath.size()-1-j]);
//    std::cout << "<br>input display path: " << inputDisplayPath;
    std::string::size_type foundExperimental=inputDisplayPath.find("experimental");
    if (foundExperimental!=std::string::npos)
      std::cout << "<b>This is an entirely experimental version of the calculator. </b>\n";

  }
  std::string tempS;
  theParser.initDefaultFolderAndFileNames(inputPatH, inputDisplayPath, IPAdressCaller);
  theParser.InitJavaScriptDisplayIndicator();

	if (argc>1)
    return main_command_input(argc, argv);
  else
    std::cin >> theParser.inputStringRawestOfTheRaw;
  ComputationComplete=false;
	if (theParser.inputStringRawestOfTheRaw=="")
	{
#ifdef WIN32
    char buffer[2000];
		size_t tempI=1500;
		::getenv_s(&tempI, buffer, 1500, "QUERY_STRING");
		theParser.inputStringRawestOfTheRaw=buffer;
#else
		theParser.inputStringRawestOfTheRaw=getenv("QUERY_STRING");
//		inputString=::getenv("QUERY_STRING");
#endif
    IPAdressCaller=getenv("REMOTE_ADDR");
    for (int i=0; i<MathRoutines::Minimum((int)IPAdressCaller.size(), SomeRandomPrimesSize); i++)
      IPAdressCaller[i]='A'+(IPAdressCaller[i]*SomeRandomPrimes[i])%26;
	}
//	inputString="textInput=+asf&buttonGo=Go";
//  inputString="weylLetterInput=B&weyRankInput=3&textInput=%2B&buttonGo=Go";
  std::cout << "<html><meta name=\"keywords\" content= \"Root system, Root system Lie algebra, "
  << "Vector partition function calculator, vector partition functions, Semisimple Lie algebras, "
  << "Root subalgebras, sl(2)-triples\"> <head> <title>vpf version  "
  << __DATE__ << ", " << __TIME__ << "</title>";
  //below follows a script for collapsing and expanding menus
//  std::cout << "<script type=\"text/javascript\" src=\"http://ajax.googleapis.com/ajax/libs/dojo/1.6.1/dojo/dojo.xd.js\""
//  << "></script>";
//  << " djConfig = \"parseOnLoad: true\"></script>";

  std::cout << "<script src=\"" << theParser.DisplayPathServerBase
  << "jsmath/easy/load.js\"></script> ";
  std::cout << "\n"
//  << CGI::GetAnimateShowHideJavascriptMustBEPutInHTMLHead()
  << "</head>\n<body onload=\"checkCookie(); updatePreamble();\">\n";
  //std::cout << IPAdressCaller;
//  std::stringstream tempStreamX;
//  static_html3(tempStreamX);
//  static_html5(tempStreamX);
//  std::cout << tempStreamX.str() << std::endl;
//  std::cout << inputString;
#ifndef WIN32
  pthread_create(&TimerThread, NULL,*RunTimer, 0);
#endif
//  std::cout << "Raw input: " << inputString << "<hr>";
  CGI::functionCGIServerIgnoreUserAbort=&ignoreUserAbortSignal;
  List<std::string> inputStrings, inputStringNames;
  CGI::ChopCGIInputStringToMultipleStrings
  (theParser.inputStringRawestOfTheRaw, inputStrings, inputStringNames);
//  std::cout << "Chopped strings: ";
//  for (int i=0; i<inputStrings.size; i++)
//    std::cout << inputStringNames[i] << " = " << inputStrings[i] << "<br>";
//  std::cout << "<hr>";
  std::string civilizedInput, inputRankString, inputWeylString;
  if (inputStringNames.Contains("textInput"))
    civilizedInput= inputStrings[inputStringNames.GetIndex("textInput")];
  if (inputStringNames.Contains("textDim"))
    inputRankString= inputStrings[inputStringNames.GetIndex("textDim")];
  if (inputStringNames.Contains("textType"))
    inputWeylString= inputStrings[inputStringNames.GetIndex("textType")];
  CGI::CivilizedStringTranslationFromCGI(civilizedInput, civilizedInput);
  List<std::string> optionsType, optionsRank;
  optionsType.AddOnTop("A");
  optionsType.AddOnTop("B");
  optionsType.AddOnTop("C");
  optionsType.AddOnTop("D");
  optionsType.AddOnTop("E");
  optionsType.AddOnTop("F");
  optionsType.AddOnTop("G");
//  optionsType.AddOnTop("NoPreamble");
  optionsRank.AddOnTop("1");
  optionsRank.AddOnTop("2");
  optionsRank.AddOnTop("3");
  optionsRank.AddOnTop("4");
  optionsRank.AddOnTop("5");
  optionsRank.AddOnTop("6");
  optionsRank.AddOnTop("7");
  optionsRank.AddOnTop("8");
  //civilizedInput=
  //"g_{{i}}:= getChevalleyGenerator{}(F_1, i); h_{{i}}:=getCartanGenerator{}(F_1, i) ; [5 g_{9},g_{-1}]";
//  civilizedInput="PolyDivRemainder{}(x^3+6xy+5xy^2+y^3, x^2+y^2-1, x^3+y^3-xy) ;";
//  civilizedInput="plot2D{}(sin{}(x), -5, 5)+ plot2D{}(1/sin{}(x ), 0.01, 3.14)";
//  civilizedInput="MakeCoxeterElement{}(B_3,2,1,1,1,1,2,2,1,2,1,2,1,2,1);";
//  civilizedInput="Deserialize{}(Serialize{}(experimentalEmbedSemisimpleInSemisimple{}(G_2, B_3)))";
  //civilizedInput="1/2";
//  civilizedInput="MakeCharacter{}(B_3, 1);";
  //civilizedInput="g:=SemisimpleLieAlgebra{} G_2;\nhwTAAbf{}(g_{-1} g_{-2}, g_{-1}g_{-2}, (2,2))";
  //civilizedInput="Load{}Serialization{}(LoadSltwoSubalgebra, 2 (Serialization{}(getChevalleyGenerator, (B)^{2}_{3}, -3))\\\\+Serialization{}(getChevalleyGenerator, (B)^{2}_{3}, -1)\\\\+3 (Serialization{}(getChevalleyGenerator, (B)^{2}_{3}, -2)), 3 (Serialization{}(getChevalleyGenerator, (B)^{2}_{3}, 3))\\\\+6 (Serialization{}(getChevalleyGenerator, (B)^{2}_{3}, 1))\\\\+10/3 (Serialization{}(getChevalleyGenerator, (B)^{2}_{3}, 2)))";
  //civilizedInput="Melt{}(a);";
  //civilizedInput="Melt{}(a;b);";
//  civilizedInput="a; b";
//  civilizedInput=" ";
//  civilizedInput="c:=(a:=b);c;a;";
//  civilizedInput="experimentalPrintSemisimpleSubalgebras{}(A_2)";
//  civilizedInput="experimentalEmbedSemisimpleInSemisimple{}(G_2, B_3)";
  //civilizedInput="Load{}Serialization{}(LoadSemisimpleSubalgebras, (B)^{2}_{3}, (Serialization{}(LoadCandidateSubalgebra, (G)^{6}_{2}, ((2, 3, 4), (-3, -3, -6)))))";
  //civilizedInput="Load{}Serialization{}(LoadSemisimpleSubalgebras, (B)^{2}_{3}, (Serialization{}(LoadCandidateSubalgebra, (G)^{6}_{2}, ((2, 3, 4), (-3, -3, -6) ),( g_-1+g_-3, g_1+g_3, g_-2, g_2 ) ) ))";
//  civilizedInput="Load{}Serialization{}(LoadSemisimpleSubalgebras, (B)^{2}_{3}, (Serialization{}(LoadCandidateSubalgebra, (G)^{6}_{2}, ((2, 3, 4), (-3, -3, -6) ),( g_-6+g_-7, g_6+g_7, g_-8, g_8 ) ) ))";
  /*civilizedInput=
  "%HideLHS Load{}Serialization{}(LoadSemisimpleSubalgebras, (F)^{1}_{4}, \\\\\
  (Serialization{}(LoadCandidateSubalgebra, (A)^{156}_{1}, ((11, 21, 30, 16)), \\\\\
(g_{-1}+3g_{-2}+2g_{-3}+4g_{-4},4g_{4}+15g_{3}+14g_{2}+22g_{1})\
)))";*/
//civilizedInput="g_{{i}}:= getChevalleyGenerator{}(G_1, i); h_{{i}}:=getCartanGenerator{}(G_1, i) ; \
//x:=Polynomial{} \\lambda;\
//v:= hwv{}(G_2, (x, 0));\
//g_{-1}^{x +1}   v";

  if (civilizedInput=="PenkovProject")
  {civilizedInput= "\
%HideLHS \n\
Load{}Serialization{}(LoadSemisimpleSubalgebras, (F)^{1}_{4}, \\\\\n\
(\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{156}_{1}, ((11, 21, 30, 16)), \\\\\n\
(\n\
g_{-1}+3g_{-2}+2g_{-3}+4g_{-4},\n\
4g_{4}+15g_{3}+14g_{2}+22g_{1}\n\
)\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{60}_{1}, ((7, 13, 18, 10)),\n\
(\n\
g_{-1}+2g_{-2}+3g_{-4}+4g_{-6}+5g_{-7}+6g_{-9},\n\
-83/406g_{9}+55/29g_{7}+989/406g_{6}+5/29g_{4}+1571/406g_{2}+14g_{1}\n\
)\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{36}_{1}, ((5, 10, 14, 8)),\n\
(\n\
g_{-2}+2g_{-4}+3g_{-5}+4g_{-6}+5g_{-7}+6g_{-8}+7g_{-9}+8g_{-11},\n\
-21878/2045g_{11}+45282/2045g_{9}+21997/2045g_{8}-34/5g_{7}-43943/2045g_{6}-4566/409g_{5}+21g_{4}+11004/409g_{2}\n\
)\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{12}_{1}, ((3, 6, 8, 4)),\n\
(\n\
g_{-2}+2g_{-5}+3g_{-6}+4g_{-8}+5g_{-9}+6g_{-10}+7g_{-11}+8g_{-12}+9g_{-13}+10g_{-15}+11g_{-16}+12g_{-18} ,\n\
-2115041/3639963g_{18}+2627072/3639963g_{16}-2435210/3639963g_{15}+3684764/3639963g_{13}+3149078/3639963g_{12}\n\
-49952/3639963g_{11}-3826808/3639963g_{10}+388076/3639963g_{9}+1102602/1213321g_{8}-1008096/1213321g_{6}\n\
+9713219/3639963g_{5}-11256722/3639963g_{2}\n\
)), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{28}_{1}, ((5, 9, 12, 6)),\n\
(\n\
g_{-1}+3g_{-2}+4g_{-9}+2g_{-16},\n\
3g_{16}+3/2g_{9}+2g_{2}+10g_{1}\n\
)), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{4}_{1}, ((2, 3, 4, 2)),\n\
(\n\
g_{-1}+2g_{-14}+3g_{-20}+4g_{-22},\n\
1/4g_{22}+1/3g_{20}+1/2g_{14}+g_{1}\n\
)), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{8}_{1}+(A)^{4}_{1}, ((2, 3, 4, 2), (0, 2, 4, 2)),\n\
(g_{-18}- (g_{-20})- (g_{-11})- (g_{-14}), g_{18}- (g_{11})- (g_{20})- (g_{14}), \\\\\n\
g_{-10}+g_{-7}- (g_{-6})+g_{-3}, \\\\\n\
g_{3}+g_{7}- (g_{6})+g_{10})\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{4}_{1}+(A)^{2}_{2}, ((2, 3, 4, 2), (0, 1, 2, 1), (0, 0, -1, 0)),\n\
(g_{-14}+g_{-18}, 2 (g_{18})+2 (g_{14}), (g_{-13}), (g_{13}), \\\\\n\
(g_{3}), (g_{-3}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{35}_{1}, ((5, 19/2, 14, 8)),\n\
(3g_{-4}+2g_{-8}+g_{-9},9g_{9}+5/2g_{8}+8/3g_{4})\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{11}_{1}, ((3, 11/2, 8, 4)),\n\
(2g_{-8}+g_{-9}+4g_{-12}+3g_{-13}+5g_{-16},\n\
55/182g_{16}+123/182g_{13}+3/26g_{12}-103/182g_{9}+33/26g_{8}\n\
)\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{10}_{1}, ((3, 5, 7, 4)),\n\
(\n\
3g_{-5}+2g_{-11}+g_{-16}, 4g_{16}+3/2g_{11}+g_{5}\n\
)\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{9}_{1}, ((5/2, 5, 7, 4)),\n\
(3g_{-12}+2g_{-13}+g_{-14},\n\
g_{14}+g_{13}+2/3g_{12}\n\
)\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{6}_{1}, ((2, 4, 6, 3)),\n\
(3g_{-14}+2g_{-15}+g_{-16}, 2g_{16}+1/2g_{15}+2/3g_{14}\n\
)\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, 2 ((A)^{6}_{1}), ((2, 4, 6, 3), (1, 1, 0, 1)),\n\
(g_{-15}+2 (g_{-16})+g_{-14}, g_{15}+2 (g_{14})+g_{16}, \\\\\n\
g_{-1}- (g_{-2})- (g_{-4}), -2 (g_{2})+2 (g_{1})- (g_{4}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{3}_{1}, ((3/2, 3, 4, 2)),\n\
(g_{-21}+g_{-20}+g_{-17}- (g_{-14})- (g_{-22}), g_{20}+g_{21}-2 (g_{14})- (g_{19})- (g_{17}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{9}_{1}+(A)^{3}_{1}, ((3/2, 3, 4, 2), (1/2, 0, 2, 2)),\n\
(g_{-19}+g_{-20}, g_{20}+g_{19}, g_{-3}- (g_{-4})+g_{-1}, \\\\\n\
g_{1}+2 (g_{3})-2 (g_{4}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, 2 ((A)^{3}_{1}), ((3/2, 3, 4, 2), (1/2, 0, 1, 1))), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{8}_{1}+(A)^{3}_{1}, ((3/2, 3, 4, 2), (0, 0, 2, 2)),\n\
(g_{-19}+g_{-20}, g_{20}+g_{19}, - (g_{-4})+g_{-3}, 2 (g_{3})-2 (g_{4}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{8}_{1}, ((2, 4, 6, 4)),\n\
(g_{-12}+2g_{-13}, g_{13}+2g_{12}\n\
)\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{28}_{1}+(A)^{8}_{1}, ((2, 4, 6, 4), (3, 5, 6, 0)),\n\
(g_{-12}+g_{-13}, 2 (g_{13})+2 (g_{12}), g_{-2}- (g_{-3})+g_{-1}, \\\\\n\
-6 (g_{3})+10 (g_{2})+6 (g_{1}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{8}_{1}+(A)^{1}_{2}, ((2, 4, 6, 4), (1/2, 1, 1, 0), (0, -1/2, 0, 0)),\n\
(g_{-12}+g_{-13}, 2 (g_{13})+2 (g_{12}), (g_{-14}), (g_{14}), \\\\\n\
(g_{2}), (g_{-2}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (G)^{3}_{2}+(A)^{8}_{1}, ((2, 4, 6, 4), (1, 3/2, 2, 0), (-3/2, -3/2, -3, 0)),\n\
(g_{-12}+g_{-13}, 2 (g_{13})+2 (g_{12}), - (g_{-9})+g_{-8}, \\\\\n\
- (g_{9})+g_{8}, (g_{11}), (g_{-11}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{2}_{1}, ((1, 2, 3, 2)),\n\
(g_{-18}+2g_{-23}, 1/2g_{23}+g_{18}\n\
)\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{28}_{1}+(A)^{2}_{1}, ((1, 2, 3, 2), (3, 5, 6, 0))), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{4}_{1}+(A)^{2}_{1}, ((1, 2, 3, 2), (1, 2, 2, 0)),\n\
(g_{-21}+2 (g_{-22})+g_{-20}, g_{22}+2 (g_{20})- (g_{21}), \\\\\n\
g_{-8}+g_{-6}+2 (g_{-11})- (g_{-2}), g_{6}+g_{8}+2 (g_{5})- (g_{9}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{10}_{1}+(A)^{2}_{1}, ((1, 2, 3, 2), (2, 3, 3, 0)),\n\
(g_{-20}+g_{-21}, - (g_{22})+g_{21}, g_{-6}+g_{-2}+2 (g_{-9})+g_{-1}, \\\\\n\
g_{6}+g_{9}+4 (g_{1})+2 (g_{2}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{3}_{1}+(A)^{2}_{1}, ((1, 2, 3, 2), (1, 3/2, 2, 0))), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, 2 ((A)^{2}_{1}), ((1, 2, 3, 2), (1, 1, 1, 0)),\n\
(g_{-21}, g_{21}, g_{-5}+g_{-11}, g_{11}+g_{5})\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, 3 ((A)^{2}_{1}), ((1, 2, 3, 2), (1, 1, 1, 0), (0, 1, 1, 0)),\n\
(g_{-21}, g_{21}, g_{-5}+g_{-11}, g_{11}+g_{5}, - (g_{-9})+g_{-2}, \\\\\n\
- (g_{9})+g_{2})\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{2}_{1}+(A)^{1}_{2}, ((1, 2, 3, 2), (1/2, 1, 1, 0), (0, -1/2, 0, 0)),\n\
( (g_{-21}), (g_{21}), (g_{-14}), (g_{14}), (g_{2}), (g_{-2}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (B)^{1}_{2}+(A)^{2}_{1}, ((1, 2, 3, 2), (1/2, 1, 1, 0), (0, -1/2, -1/2, 0)),\n\
(g_{-21}, g_{21}, (g_{-14}), (g_{14}), g_{9}+g_{2}, g_{-2}+g_{-9})\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (G)^{3}_{2}+(A)^{2}_{1}, ((1, 2, 3, 2), (1, 3/2, 2, 0), (-3/2, -3/2, -3, 0))), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{2}_{1}+(A)^{1}_{3}, ((1, 2, 3, 2), (1/2, 1, 1, 0), (0, -1/2, 0, 0), (-1/2, 0, 0, 0)),\n\
( (g_{-21}), (g_{21}), (g_{-14}), (g_{14}), (g_{2}), (g_{-2}), \\\\\n\
(g_{1}), (g_{-1}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{1}_{1}, ((1, 3/2, 2, 1)),\n\
(\n\
 g_{-24}, g_{24})\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{35}_{1}+(A)^{1}_{1}, ((1, 3/2, 2, 1), (0, 9/2, 8, 5)),\n\
( (g_{-24}), (g_{24}), g_{-4}+3 (g_{-2})+2 (g_{-3}), 3 (g_{2})+5 (g_{4})+4 (g_{3}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{11}_{1}+(A)^{1}_{1}, ((1, 3/2, 2, 1), (0, 5/2, 4, 3)),\n\
( (g_{-24}), (g_{24}), g_{-6}+g_{-2}+3 (g_{-7})-3/4 (g_{-9}), \\\\\n\
g_{4}+g_{7}+4 (g_{9})+4 (g_{6}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{10}_{1}+(A)^{1}_{1}, ((1, 3/2, 2, 1), (0, 2, 4, 3)),\n\
( (g_{-24}), (g_{24}), 2 (g_{-9})+g_{-4}, 3 (g_{4})+2 (g_{9}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{3}_{1}+(A)^{1}_{1}, ((1, 3/2, 2, 1), (0, 3/2, 2, 1)),\n\
( (g_{-24}), (g_{24}), g_{-13}+g_{-10}+g_{-9}-2 (g_{-16})+g_{-6}, \\\\\n\
g_{2}+g_{6}+g_{9}- (g_{16})- (g_{13}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{1}_{1}+(A)^{8}_{1}+(A)^{3}_{1}, ((1, 3/2, 2, 1), (0, 3/2, 2, 1), (0, 0, 2, 2)),\n\
( (g_{-24}), (g_{24}), g_{-9}+g_{-10}, g_{10}+g_{9}, g_{-3}+g_{-4}, \\\\\n\
2 (g_{4})+2 (g_{3}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{8}_{1}+(A)^{1}_{1}, ((1, 3/2, 2, 1), (0, 2, 4, 2)),\n\
( (g_{-24}), (g_{24}), g_{-10}-2 (g_{-7})+g_{-6}, g_{3}+2 (g_{6})- (g_{7}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{2}_{1}+(A)^{1}_{1}, ((1, 3/2, 2, 1), (0, 1, 2, 1)),\n\
( (g_{-24}), (g_{24}), g_{-13}, g_{13})\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{1}_{1}+(A)^{3}_{1}+(A)^{2}_{1}, ((1, 3/2, 2, 1), (0, 1, 2, 1), (0, 1/2, 0, 1))), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, 2 ((A)^{1}_{1}), ((1, 3/2, 2, 1), (0, 1/2, 1, 1)),\n\
(g_{-24}, g_{24}, g_{-16}, g_{16})\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, 2 ((A)^{1}_{1})+(A)^{10}_{1}, ((1, 3/2, 2, 1), (0, 1/2, 1, 1), (0, 2, 3, 0)),\n\
( (g_{-24}), (g_{24}), (g_{-16}), (g_{16}), 2 (g_{-2})+g_{-3}, \\\\\n\
3 (g_{3})+2 (g_{2}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, 2 ((A)^{1}_{1})+(A)^{2}_{1}, ((1, 3/2, 2, 1), (0, 1/2, 1, 1), (0, 1, 1, 0)),\n\
( (g_{-24}), (g_{24}), (g_{-16}), (g_{16}), g_{-6}, g_{6})\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, 3 ((A)^{1}_{1}), ((1, 3/2, 2, 1), (0, 1/2, 1, 1), (0, 1/2, 1, 0)),\n\
( (g_{-24}), (g_{24}), (g_{-16}), (g_{16}), (g_{-9}), (g_{9}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, 4 ((A)^{1}_{1}), ((1, 3/2, 2, 1), (0, 1/2, 1, 1), (0, 1/2, 1, 0), (0, 1/2, 0, 0)),\n\
( (g_{-24}), (g_{24}), (g_{-16}), (g_{16}), (g_{-9}), (g_{9}), \\\\\n\
(g_{-2}), (g_{2}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, 2 ((A)^{1}_{1})+(B)^{1}_{2}, ((1, 3/2, 2, 1), (0, 1/2, 1, 1), (0, 1/2, 1, 0), (0, 0, -1/2, 0)),\n\
( (g_{-24}), (g_{24}), (g_{-16}), (g_{16}), (g_{-9}), (g_{9}), \\\\\n\
(g_{3}), (g_{-3}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{2}_{2}+(A)^{1}_{1}, ((1, 3/2, 2, 1), (0, 1, 2, 1), (0, 0, -1, 0)),\n\
( (g_{-24}), (g_{24}), (g_{-13}), (g_{13}), (g_{3}), (g_{-3}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (B)^{1}_{2}+(A)^{1}_{1}, ((1, 3/2, 2, 1), (0, 1/2, 1, 1), (0, 0, 0, -1/2)),\n\
( (g_{-24}), (g_{24}), (g_{-16}), (g_{16}), (g_{4}), (g_{-4}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (C)^{2}_{3}+(A)^{1}_{1}, ((1, 3/2, 2, 1), (0, 1, 2, 1), (0, 0, -1, 0), (0, -1, 0, 0)),\n\
( (g_{-24}), (g_{24}), (g_{-13}), (g_{13}), (g_{3}), (g_{-3}), \\\\\n\
(g_{2}), (g_{-2}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{3}_{2}, ((3/2, 3, 4, 2), (-1/2, -3/2, -1, 0)),\n\
(\n\
g_{-19}+g_{-20}, g_{20}+g_{19}, g_{6}+g_{5}, g_{-5}+g_{-6}\n\
)\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{3}_{2}, ((3/2, 3, 4, 2), (0, -3/2, -2, -1)),\n\
(\n\
g_{-21}+g_{-19}+g_{-20}, g_{19}+g_{20}- (g_{17})- (g_{14}), \\\\\n\
g_{9}+g_{16}- (g_{6})-2 (g_{2}), g_{-16}+g_{-6}+2 (g_{-9})- (g_{-2})\n\
)\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{2}_{2}, ((1, 2, 3, 2), (0, 0, 0, -1)),\n\
(\n\
g_{-21},  g_{21}, g_{-4},  g_{4}\n\
)\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{1}_{2}, ((1, 3/2, 2, 1), (-1/2, 0, 0, 0)),\n\
(\n\
g_{-24}, g_24, g_-1, g_1\n\
)\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{2}_{2}+(A)^{1}_{2}, ((1, 3/2, 2, 1), (-1/2, 0, 0, 0), (0, 0, 1, 1), (0, 0, 0, -1)),\n\
( (g_{-24}), (g_{24}), (g_{1}), (g_{-1}), (g_{-7}), (g_{7}), \\\\\n\
(g_{4}), (g_{-4}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (B)^{3}_{2}, ((3/2, 3, 4, 2), (-1/2, -3/2, -3/2, -1/2))), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (B)^{1}_{2}, ((1, 3/2, 2, 1), (-1/2, -1/2, -1/2, 0)),\n\
( (g_{-24}), (g_{24}), g_{8}, g_{-8})\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (G)^{3}_{2}, ((3/2, 3, 4, 2), (-3/2, -9/2, -6, -3)),\n\
(g_{-19}+g_{-20}, g_{20}+g_{19}, (g_{23}), (g_{-23}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (A)^{1}_{3}, ((1, 3/2, 2, 1), (-1/2, 0, 0, 0), (0, -1/2, 0, 0)),\n\
( g_{-24},  g_{24},\n\
g_{1}, g_{-1},\n\
g_{2}, g_{-2})\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (B)^{1}_{3}, ((1, 3/2, 2, 1), (-1/2, 0, 0, 0), (0, -1/2, -1/2, 0)),\n\
( (g_{-24}), (g_{24}), (g_{1}), (g_{-1}), g_{6}, g_{-6})\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (C)^{2}_{3}, ((1, 2, 3, 2), (0, 0, 0, -1), (0, -1, -2, 0)),\n\
( (g_{-21}), (g_{21}), (g_{4}), (g_{-4}), (g_{9}), (g_{-9}))\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (D)^{1}_{4}, ((1, 3/2, 2, 1), (-1/2, 0, 0, 0), (0, -1/2, 0, 0), (0, -1/2, -1, 0)),\n\
(\n\
 g_{-24},  g_{24},\n\
 g_{1},  g_{-1},\n\
 g_{2},  g_{-2},\n\
 g_{9},  g_{-9})\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (B)^{1}_{4}, ((1, 3/2, 2, 1), (-1/2, 0, 0, 0), (0, -1/2, 0, 0), (0, 0, -1/2, 0)),\n\
(\n\
 g_{-24},  g_{24},\n\
 g_{1},  g_{-1},\n\
 g_{2},  g_{-2},\n\
 g_{3},  g_{-3})\n\
), \\\\\n\
Serialization{}(LoadCandidateSubalgebra, (F)^{1}_{4}, ((2, 3, 4, 2), (-1, 0, 0, 0), (0, -1, -1, 0), (0, 0, 0, -1)),\n\
(\n\
 g_{-24},  g_{24},\n\
 g_{1},  g_{-1},\n\
 g_{6},  g_{-6},\n\
 g_{4},  g_{-4})\n\
)))\n\
  ";
  }

  std::stringstream tempStreamXX;
  static_html4(tempStreamXX);
  std::cout << "<table>\n <tr valign=\"top\">\n <td>";
  std::cout << "\n<FORM method=\"POST\" name=\"formCalculator\" action=\""
  << theParser.DisplayNameCalculator << "\">\n" ;
  std::cout << GetSelectHTMLStringTEmp
  (optionsType, optionsRank, inputWeylString, inputRankString,
   inputStringNames.Contains("checkUsePreamble"))
  << tempStreamXX.str();
  std::cout << "\n<br>\n";
  std::string civilizedInputSafish;
  if (CGI::GetHtmlStringSafeishReturnFalseIfIdentical(civilizedInput, civilizedInputSafish))
    std::cout
    << "Your input has been treated normally, however the return string"
    << " of your input has been modified. More precisely, &lt; and &gt;  are "
    << " modified due to a javascript hijack issue. ";
  std::cout << "<textarea rows=\"3\" cols=\"30\" name=\"textInput\" id=\"textInputID\""
  << " onkeypress=\"if (event.keyCode == 13 && event.shiftKey) {storeSettings(); "
  << " this.form.submit(); return false;}\" >";
  std::cout << civilizedInputSafish;
  std::cout << "</textarea>\n<br>\n";
  std::cout << "<input type=\"submit\" title=\"Shift+Enter=shortcut from input text box. \" name=\"buttonGo\" "
  << "value=\"Go\" onmousedown=\"storeSettings();\" > ";
  if (civilizedInput!="")
    std::cout << "<a href=\"" << theParser.DisplayNameCalculator << "?"
    << theParser.inputStringRawestOfTheRaw
    << "\">Link to your input.</a>";
  std::cout << "\n</FORM>";
  std::cout << theParser.javaScriptDisplayingIndicator;
  std::cout.flush();
//  std::cout << "<br>Number of lists created before evaluation: " << NumListsCreated;
  if (civilizedInput!="")
  { if (inputStringNames.Contains("checkUsePreamble"))
    { std::stringstream tempStream;
      tempStream << "g_{{i}}:=getChevalleyGenerator{}(" << inputWeylString
      << "_" << inputRankString << ", i); h_{{i}}:=getCartanGenerator{}( "
      << inputWeylString << "_" << inputRankString << ", i) ;";
      civilizedInput=tempStream.str()+civilizedInput;
    }
    theParser.Evaluate(civilizedInput);
    ComputationComplete=true;
    if (theParser.flagProduceLatexLink)
      std::cout << "<br>LaTeX link (\\usepackage{hyperref}):<br> "
      << CGI::GetLatexEmbeddableLinkFromCalculatorInput
      (theParser.inputStringRawestOfTheRaw, civilizedInput);

//    std::cout << "<hr>";
    std::cout << theParser.outputString;
    if (theParser.parsingLog!="")
      std::cout << "<b> As requested, here is a calculator parsing log</b><br>"
      << theParser.parsingLog;
  }
//  theComputation.flagLogSyntaxRules=inputRankString=="2";
  std::cout << "\n\n</td>\n\n";
  if (theParser.outputCommentsString!="")
  { std::cout << "<td valign=\"top\">";
    if (theParser.outputCommentsString.size()<10000)
      std::cout << theParser.outputCommentsString;
    else
      std::cout << "The comments generated by your computation are a bit too long."
      << " If you want to see them click the show/hide button below.<br>"
      << CGI::GetHtmlSpanHidableStartsHiddeN(theParser.outputCommentsString);
    std::cout << "</td>";
  }
  std::cout << "<td valign=\"top\">";
  std::cout << ProjectInformation::GetMainProjectInfo().ToString();
  std::cout << "<hr><b>CommandList status. </b><br>";
  std::cout << theParser.ToString();

  std::cout << "</td></tr></table>";
  std::stringstream tempStream3;
//  static_html5(tempStream3);
//  static_html6(tempStream3);
//  std::cout << tempStream3.str();
//  std::cout << "</div>";
  std::cout << "\n\n<script language=\"javascript\">\n// List of words to show in drop down\n var functionNameArray = new Array(";
 /* bool isFirst=true;
  for (int i=0; i<theParser.theObjectContainer.theVariablesNonBound.size; i++)
  { VariableNonBound& currentVar=theParser.theObjectContainer.theVariablesNonBound[i];
    if (true)
    { if (!isFirst)
        std::cout << ",";
      isFirst=false;
      std::cout << "\"" << currentVar.theName << "\"";
    }
  }*/
  std::cout << ");\n  //var obj = actb(document.getElementById('textInputID'), functionNameArray);\n</script>\n";
//  for(int i=0; i<theParser.SystemCommands.size; i++)
//  { std::cout << "<br>About to execute: " << theParser.SystemCommands.TheObjects[i].c_str() << "\n" ;
 //   system(theParser.SystemCommands[i].c_str());
 // }

  std::cout << "</body></html>";
  std::cout << "<!--";
  std::cout.flush();
  ProgressReport theReport(theParser.theGlobalVariableS);
  for(int i=0; i<theParser.SystemCommands.size; i++)
  { std::stringstream out;
    out << "\n\ncommand: " << theParser.SystemCommands[i] << "\n" ;
    theReport.Report(out.str());
    system(theParser.SystemCommands[i].c_str());
  }
  std::cout << "-->";
  return 0;
}

void static_html3(std::stringstream& output){
output << " <!>\n";
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
void static_html4(std::stringstream& output){
output << " <!>\n";
output << " <script src=\"http://ajax.googleapis.com/ajax/libs/dojo/1.6.1/dojo/dojo.xd.js\" type=\"text/javascript\"></script>\n";
output << " \n";
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


