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

//#undef ANNOYINGSTATISTICS
//#define ANNOYINGSTATISTICS std::cout << "<hr>" << "Time elapsed at file " << __FILE__ << " line " << __LINE__ << ": " << theGlobalVariables.GetElapsedSeconds()

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

extern void static_html6(std::stringstream& output);
extern void static_html5(std::stringstream& output);
extern void static_html4(std::stringstream& output);
extern void static_html3(std::stringstream& output);

bool ComputationComplete;

#ifndef WIN32
timeval ComputationStartGlobal, LastMeasureOfCurrentTime;

GlobalVariables theGlobalVariables;
CommandList theParser;

double GetElapsedTimeInSeconds()
{ gettimeofday(&LastMeasureOfCurrentTime, NULL);
  int miliSeconds =(LastMeasureOfCurrentTime.tv_sec- ComputationStartGlobal.tv_sec)*1000+(LastMeasureOfCurrentTime.tv_usec- ComputationStartGlobal.tv_usec)/1000;
  return ((double) miliSeconds)/1000;
}

void* RunTimer(void* ptr)
{ for (; GetElapsedTimeInSeconds()<theGlobalVariables.MaxAllowedComputationTimeInSeconds || theGlobalVariables.MaxAllowedComputationTimeInSeconds<=0;)
  { usleep(10);
    if (ComputationComplete)
      break;
  }
  if (!ComputationComplete)
  { std::cout << "</div><br><br><br>Your computation has taken " << GetElapsedTimeInSeconds() << " seconds so far.";
    std::cout << "<br>The maximum allowed computation time is <b>" << theGlobalVariables.MaxAllowedComputationTimeInSeconds << " seconds</b>. Please use an offline version of the calculator. <br><b>Signalling ungraceful exit...</b> ";
    ParallelComputing::SafePointDontCallMeFromDestructors();
    ParallelComputing::controllerSignalPauseUseForNonGraciousExitOnly.SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint();
    std::exit(0);
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
  CGI::OpenFileCreateIfNotPresent(theFile, theParser.indicatorReportFileName, false, true, false);
  std::stringstream outStream;
  theFile << " Elapsed seconds: " << GetElapsedTimeInSeconds();
  theFile << "<hr>\n\n" << input.StatusString1 << "<hr>\n\n";
  for (int i=0; i<input.ProgressReportStrings.size; i++)
    theFile << "\n" << input.ProgressReportStrings[i] << "\n<br>\n";
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
  << "  { document.getElementById(\"textPreamble\").innerHTML=\"g:= SemisimpleLieAlgebra{}\"+"
  << " document.getElementById(\"textTypeID\").value+\"_\"+ document.getElementById(\"textDimID\").value+\""
  << "; h_{{i}}:=g_{0, i}; \"; \n"
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
int main(int argc, char **argv)
{
#ifndef WIN32
  gettimeofday(&ComputationStartGlobal, NULL);
#endif
  theParser.DisplayNameCalculator="/vpf/cgi-bin/calculator";
  ParallelComputing::cgiLimitRAMuseNumPointersInList=60000000;
  HashedList<MonomialP>::PreferredHashSize=100;
  theGlobalVariables.MaxAllowedComputationTimeInSeconds=200;
  std::string inputString, inputPatH;
  std::string tempS;
	std::cin >> inputString;
  ComputationComplete=false;
	if (inputString=="")
	{
#ifdef WIN32
    char buffer[2000];
		size_t tempI=1500;
		::getenv_s(&tempI, buffer, 1500, "QUERY_STRING");
		inputString=buffer;
#else
		inputString=getenv("QUERY_STRING");
//		inputString=::getenv("QUERY_STRING");
#endif
    IPAdressCaller=getenv("REMOTE_ADDR");
    for (int i=0; i<MathRoutines::Minimum((int)IPAdressCaller.size(), SomeRandomPrimesSize); i++)
      IPAdressCaller[i]='A'+(IPAdressCaller[i]*SomeRandomPrimes[i])%26;
	}
	getPath(argv[0], inputPatH);

  theParser.initDefaultFolderAndFileNames(inputPatH, "/vpf/", IPAdressCaller);
  theParser.InitJavaScriptDisplayIndicator();

//	inputString="textInput=+asf&buttonGo=Go";
//  inputString="weylLetterInput=B&weyRankInput=3&textInput=%2B&buttonGo=Go";
	std::cout << "Content-Type: text/html\n\n";
  std::cout << "<html><meta name=\"keywords\" content= \"Root system, Root system Lie algebra, "
  << "Vector partition function calculator, vector partition functions, Semisimple Lie algebras, "
  << "Root subalgebras, sl(2)-triples\"> <head> <title>Vector partition calculator updated "
  << __DATE__ << ", " << __TIME__ << "</title>";
  //below follows a script for collapsing and expanding menus
  std::cout << "<script src=\"" << theParser.DisplayPathServerBase << "jsmath/easy/load.js\"></script> ";
  std::cout << "\n</head>\n<body onload=\"checkCookie(); updatePreamble();\">\n";
  ANNOYINGSTATISTICS;
  //std::cout << IPAdressCaller;
//  std::stringstream tempStreamX;
//  static_html3(tempStreamX);
//  static_html5(tempStreamX);
//  std::cout << tempStreamX.str() << std::endl;
//  std::cout << inputString;
#ifndef WIN32
  pthread_t TimerThread;
  pthread_create(&TimerThread, NULL,*RunTimer, 0);
#endif
//  std::cout << "Raw input: " << inputString << "<hr>";
  CGI::functionCGIServerIgnoreUserAbort=&ignoreUserAbortSignal;
  List<std::string> inputStrings, inputStringNames;
  CGI::ChopCGIInputStringToMultipleStrings(inputString, inputStrings, inputStringNames);
//  std::cout << "Chopped strings: ";
//  for (int i=0; i<inputStrings.size; i++)
//    std::cout << inputStringNames[i] << " = " << inputStrings[i] << "<br>";
//  std::cout << "<hr>";
  std::string civilizedInput, inputRankString, inputWeylString;
  if (inputStringNames.ContainsObject("textInput"))
    civilizedInput= inputStrings[inputStringNames.IndexOfObject("textInput")];
  if (inputStringNames.ContainsObject("textDim"))
    inputRankString= inputStrings[inputStringNames.IndexOfObject("textDim")];
  if (inputStringNames.ContainsObject("textType"))
    inputWeylString= inputStrings[inputStringNames.IndexOfObject("textType")];
  CGI::CivilizedStringTranslationFromCGI(civilizedInput, civilizedInput);
  theGlobalVariables.SetFeedDataToIndicatorWindowDefault(&makeReport);
  theGlobalVariables.SetTimerFunction(&GetElapsedTimeInSeconds);
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
  if (inputWeylString!="")
  { theParser.DefaultWeylLetter=inputWeylString[0];
  }
  else
  { theParser.DefaultWeylLetter='B';
    inputWeylString="Calculator";
  }
  if (inputRankString!="")
  { std::stringstream tempStream;
    tempStream << inputRankString;
    tempStream.seekg(0);
    tempStream >> theParser.DefaultWeylRank;
  } else
    theParser.DefaultWeylRank=3;

//  civilizedInput="a{}b";
//  civilizedInput="Polynomial{}(a+a_1)";
//  civilizedInput="[a,b,c]";
//  civilizedInput="h_{{a}}:={\\bar h}_a; \nh_1h_2";
//  civilizedInput="g:=SemisimpleLieAlgebra{}A_1";//\ng_0";
//  civilizedInput="g:=SemisimpleLieAlgebra{}A_1;\nh_{{a}}:=g_(0,a);\ng_1+h_1";
  //civilizedInput="SemisimpleLieAlgebra{} A_1";
//  civilizedInput="f{}0:=1;\nf{}1:=f{}2;\nf{}{{x}}:=f{}(x-1)+f{}(x-2);\nf{}1";
//  civilizedInput="1+1";
//  civilizedInput="{{a}}*a:=a^2;\nx*x";
  //civilizedInput="Matrix{}(A,5,6)";
//  civilizedInput="\\begin{array}{cc} a &a \\end{array}";
//  civilizedInput="\\begin{array}{cc}x&((2,3),a,a,((b,a),a,a,b,(a,b)))\\\\ 1\\end{array}";
//  civilizedInput="fib{}1:=1;\nfib{}0:=1;\nfib{}{{n}}:=fib{}(n-1)+fib{}(n-2);\nfib{}10";
//  civilizedInput="hwv{}(B_3,(x_1,0,1),(1,0,0))";
//  civilizedInput="Polynomial{} (1)";
//  civilizedInput="g:= SemisimpleLieAlgebra{}B_3:=g_1 hwv{}(B_3, (x_1,0,1),(1,0,0));\n hwv{}(B_3, (x_1,0,1),(1,0,0)):=v_\\lambda;\n result \n ";
//  inputWeylString="Calculator";
//  civilizedInput="g:=SemisimpleLieAlgebra{} B_3;\ng_{-1} hwv{}(B_3, (x_1,0,1),(1,0,0))";
//  civilizedInput="g:=SemisimpleLieAlgebra{} B_3;\nv:=hwv{}(B_3, (x_1,0,1),(1,0,0));\ng_{0,1}g_{-1} {}v";
//civilizedInput="g:=SemisimpleLieAlgebra{} B_3";
//  civilizedInput="hwv{}(B_3, (x_1,0,1),(1,0,0))";
//  inputWeylString="B";
//  civilizedInput="g:=SemisimpleLieAlgebra{}G_2;\nUE{}(g_1+2g_2)";
  /*civilizedInput="A_1:=1/12x_1^{2}+1/2x_1+5/12;\n\
A_2:=1/12x_1^{2}+7/12x_1+1/2;\n\
A_3:=1/12x_1^{2}+7/12x_1+1;\n\
A_4:=1/12x_1^{2}+2/3x_1+1;\n\
A_5:=1/12x_1^{2}+2/3x_1+1 ;\n\
A_6:=1/12x_1^{2}+3/4x_1+7/6;\n\
A_7:=1/12x_1^{2}+3/4x_1+5/3;\n\
A_8:=1/12x_1^{2}+5/6x_1+7/4;\n\
Solve{}({{a}}x_1+{{b}}):=-b/a;\n\
Solve{}({{a}}x_1):=0;\n\
B{}({{a}},{{b}}):=Solve{}(A_a-A_b);\n\
UnionNoRepetition{}\n\
FunctionToMatrix{}(B,8,8)";*/
//  civilizedInput="g:=SemisimpleLieAlgebra{} G_2;\nv:=hwvbf{}(g_{-1} g_{-2}, g_{-1}g_{-2}, (2,2))";
//  civilizedInput="SemisimpleLieAlgebra{} G_2;\nUE{}(g_1 )";
//  civilizedInput="g:=SemisimpleLieAlgebra{} G_2;\nUE{}(g_2 g_1+g_1)";
//  civilizedInput="g:=SemisimpleLieAlgebra{} G_2;\nUE{}(2x_1 g_2 g_1+3g_1)";
//  civilizedInput="g:=SemisimpleLieAlgebra{} G_2;\nhwTAAbf{}(g_-1, g_-1, (1,0))";
//  civilizedInput="g:=SemisimpleLieAlgebra{} B_3;\nv:=hwv{}(B_3, (x_1,0,1),(1,0,0));\ng_{0,1}g_{-1} v";
//civilizedInput="g:=SemisimpleLieAlgebra{} B_3;\nv:=hwv{}(B_3, (x_1,0,1),(1,0,0));\ng_{0,1}g_{-1} v";
//  civilizedInput="g:= SemisimpleLieAlgebra{}B_3;UE{}(g_{-1}^{2}g_{-2}g_{-3})";
//inputWeylString="A";
//inputRankString="2";
//civilizedInput=" irreducibleRep(1,1)";
//  civilizedInput="a";
//  civilizedInput="g:=SemisimpleLieAlgebra{} B_3;\nv:=hwv{}(B_3, (x_1,0,1),(1,0,0));\ng_{0,1}g_{-1} v";
//  civilizedInput="g:=SemisimpleLieAlgebra{}G_2; 2 h_1";
//  civilizedInput="g:=SemisimpleLieAlgebra{}G_2; 2 g_{0,1}";
//civilizedInput="g_{1}^2g_{1}";
  std::stringstream tempStreamXX;
  static_html4(tempStreamXX);
  std::cout << "<table>\n <tr valign=\"top\">\n <td>";
  std::cout << "\n<FORM method=\"POST\" name=\"formCalculator\" action=\"" <<
  theParser.DisplayNameCalculator << "\">\n" ;
  std::cout << GetSelectHTMLStringTEmp(optionsType, optionsRank, inputWeylString, inputRankString, inputStringNames.ContainsObject("checkUsePreamble"))
  <<  tempStreamXX.str();
   std::cout << "\n<br>\n";
  std::string civilizedInputSafish;
  if (CGI::GetHtmlStringSafeishReturnFalseIfIdentical(civilizedInput, civilizedInputSafish))
    std::cout << "Your input has been treated normally, however the return string of your input has been modified. More precisely, &lt; and &gt;  are modified due to a javascript hijack issue. ";
  std::cout << "<textarea rows=\"3\" cols=\"30\" name=\"textInput\" id=\"textInputID\""
  << " onkeypress=\"if (event.keyCode == 13 && event.shiftKey) {storeSettings();  this.form.submit(); return false;}\" >";
  std::cout << civilizedInputSafish;
  std::cout << "</textarea>\n<br>\n";
  std::cout << "<input type=\"submit\" title=\"Shift+Enter=shortcut from input text box. \" name=\"buttonGo\" "
  << "value=\"Go\" onmousedown=\"storeSettings();\" > ";
  if (civilizedInput!="")
    std::cout << "<a href=\"/vpf/cgi-bin/calculator?" << inputString << "\">Link to your input.</a><br>";
  std::cout << "\n</FORM>";
  theParser.DisplayNameCalculator="/vpf/cgi-bin/calculator";
  theParser.init(theGlobalVariables);
  if (civilizedInput!="")
  { if (inputStringNames.ContainsObject("checkUsePreamble"))
    { std::stringstream tempStream;
      tempStream << "g:=SemisimpleLieAlgebra{}" << inputWeylString << "_" << inputRankString << "; h_{{i}}:=g_{0, i};";
      civilizedInput=tempStream.str()+civilizedInput;
    }
    theParser.Evaluate(civilizedInput);
    std::cout << "<hr>";
    std::cout <<  theParser.outputString;
  }
//  theComputation.flagLogSyntaxRules=inputRankString=="2";

  std::cout << "</td><td valign=\"top\">";
  std::cout << ProjectInformation::GetMainProjectInfo().ElementToString();
  std::cout << theParser.theLog;
  std::cout << "<hr><b>CommandList status. </b><br>";
  std::cout << theParser.ElementToString();

  std::cout << "</td></tr></table>";
  std::stringstream tempStream3;
  static_html5(tempStream3);
//  static_html6(tempStream3);
  std::cout << tempStream3.str();
//  std::cout << "</div>";
  std::cout << "\n\n<script language=\"javascript\">\n// List of words to show in drop down\n var functionNameArray =new Array(";
  bool isFirst=true;
  for (int i=0; i<theParser.theNonBoundVars.size; i++)
  { VariableNonBound& currentVar=theParser.theNonBoundVars[i];
    if (true)
    { if (!isFirst)
        std::cout << ",";
      isFirst=false;
      std::cout << "\"" << currentVar.theName << "\"";
    }
  }
  std::cout << ");\n  //var obj = actb(document.getElementById('textInputID'), functionNameArray);\n</script>\n";
  std::cout << "</body></html>";
  return 0;
}



