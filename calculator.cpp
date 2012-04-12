#include <iostream>
#include "vpf.h"
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

//#undef ANNOYINGSTATISTICS
//#define ANNOYINGSTATISTICS std::cout << "<hr>" << "Time elapsed at file " << __FILE__ << " line " << __LINE__ << ": " << theGlobalVariables.GetElapsedSeconds()


extern int GlobalPointerCounter;

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
Parser theParser;

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
  std::string& selectedType, std::string& selectedRank)
{ std::stringstream output;
  output << "  <select name=\"textType\" id=\"textType\">\n";
  for (int i=0; i<optionsType.size; i++)
  { output << "   <option";
    if (selectedType==optionsType[i])
      output << " selected=\"selected\"";
    output << ">" << optionsType[i] << "</option>\n";
  }
  output << "</select>";
  output << "  <select name=\"textDim\" id=\"textDim\">\n";
  for (int i=0; i<optionsRank.size; i++)
  { output << "   <option";
    if (selectedRank==optionsRank[i])
      output << " selected=\"selected\"";
    output << ">" << optionsRank[i] << "</option>\n";
  }
  output << " </select> Root system.\n";
  return output.str();
}
int main(int argc, char **argv)
{
#ifndef WIN32
  gettimeofday(&ComputationStartGlobal, NULL);
#endif
  theParser.DisplayNameCalculator="/vpf/cgi-bin/calculator";
  ParallelComputing::cgiLimitRAMuseNumPointersInList=60000000;
  HashedList<Monomial<Rational> >::PreferredHashSize=100;
  theGlobalVariables.MaxAllowedComputationTimeInSeconds=20000;
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
  std::cout << "<html><meta name=\"keywords\" content= \"root system, root system Lie algebra, "
  << "Vector partition function calculator, vector partition functions, Semisimple Lie algebras, "
  << "root subalgebras, sl(2)-triples\"> <head> <title>Vector partition calculator updated "
  << __DATE__ << ", " << __TIME__ << "</title>";
  //below follows a script for collapsing and expanding menus
  std::cout << "<script src=\"" << theParser.DisplayPathServerBase << "jsmath/easy/load.js\"></script> ";
  std::cout << "\n</head>\n<body onload=\"checkCookie();\">\n";
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
  CGI::functionCGIServerIgnoreUserAbort=&ignoreUserAbortSignal;
  List<std::string> inputStrings;
  CGI::ChopCGIInputStringToMultipleStrings(inputString, inputStrings);
  inputStrings.SetSize(3);
  std::string& civilizedInput= inputStrings.TheObjects[2];
  std::string& inputRankString = inputStrings.TheObjects[1];
  std::string& inputWeylString = inputStrings.TheObjects[0];
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
  optionsType.AddOnTop("Calculator");
  optionsRank.AddOnTop("1");
  optionsRank.AddOnTop("2");
  optionsRank.AddOnTop("3");
  optionsRank.AddOnTop("4");
  optionsRank.AddOnTop("5");
  optionsRank.AddOnTop("6");
  optionsRank.AddOnTop("7");
  optionsRank.AddOnTop("8");
  inputWeylString="Calculator";
//  civilizedInput="a{}b";
//  civilizedInput="Polynomial{}(a+a_1)";
//  civilizedInput="[a,b,c]";
//  civilizedInput="h_{{a}}:={\\bar h}_a; \nh_1h_2";
//  civilizedInput="g:=SemisimpleLieAlgebra{}A_1";//\ng_0";
//  civilizedInput="g:=SemisimpleLieAlgebra{}A_1;\nh_{{a}}:=g_(0,a);\ng_1+h_1";
  if (inputWeylString=="Calculator")
  { std::stringstream tempStreamXX;
    static_html4(tempStreamXX);
    std::cout << "<table>\n <tr valign=\"top\">\n <td>";
    std::cout << "\n<FORM method=\"POST\" name=\"formCalculator\" action=\"" <<
    theParser.DisplayNameCalculator << "\">\n" ;
    std::cout << GetSelectHTMLStringTEmp(optionsType, optionsRank, inputWeylString, inputRankString)
    <<  tempStreamXX.str();
     std::cout << "\n<br>\n";
    std::string civilizedInputSafish;
    if (CGI::GetHtmlStringSafeishReturnFalseIfIdentical(civilizedInput, civilizedInputSafish))
      std::cout << "Your input has been treated normally, however the return string of your input has been modified. More precisely, &lt; and &gt;  are modified due to a javascript hijack issue. ";
    std::cout << "<textarea rows=\"3\" cols=\"30\" name=\"textInput\" id=\"textInputID\" >";
    std::cout << civilizedInputSafish;
    std::cout << "</textarea>\n<br>\n";
    std::cout << "<input type=\"submit\" name=\"buttonGo\" value=\"Go\" onmousedown=\"storeSettings();\" > ";
  //  std::cout << "<a href=\"/tmp/indicator.html\" target=\"_blank\"> Indicator window  </a>";
    std::cout << "\n</FORM>";
    CommandList theComputation;
    theComputation.init(theGlobalVariables);
    if (civilizedInput!="")
    { std::cout << "<a href=\"/vpf/cgi-bin/calculator?" << inputString << "\">Link to your input.</a><br>";
      theComputation.Evaluate(civilizedInput);
      std::cout << "<hr><b>Result.</b><br>";
      std::cout <<  theComputation.output;
    }
    std::cout << "</td><td valign=\"top\">";
    std::cout << theComputation.theLog;
    std::cout << "</td></tr></table>";
    std::cout << "</body></html>";
    return 0;
  }
  if (inputWeylString!="")
    theParser.DefaultWeylLetter=inputWeylString[0];
  else
    theParser.DefaultWeylLetter='B';
  if (inputRankString!="")
  { std::stringstream tempStream;
    tempStream << inputRankString;
    tempStream.seekg(0);
    tempStream >> theParser.DefaultWeylRank;
  } else
    theParser.DefaultWeylRank=3;
  CGI::MakeSureWeylGroupIsSane(theParser.DefaultWeylLetter, theParser.DefaultWeylRank);
  ANNOYINGSTATISTICS;
//  civilizedInput="(x_1\\mapsto 0: g_{-1}g_{-2}g_{-3})";
//  civilizedInput="splitFDPartGenVermaOverLeviParabolic((0, 1,1),(1,0,0), (1,0,1))";
//  civilizedInput="h_2 v((1,0),(0,0))";
//  civilizedInput="g_{8}(g_{-8} v((x_1,1,0),(1,0,0)) )";
//  civilizedInput="v((x_1,1,0),(1,0,0))(x_2  v((x_1,1,0),(1,0,0)) )+v((x_1,1,0),(1,0,0))(  v((x_1,1,0),(1,0,0)) )";
//  civilizedInput="(h_1-x_2) v((0,1),(0,0))";

//  civilizedInput=" g_{-1} (  v((1),(0))* v((1),(1)) )";
//  civilizedInput="g_1 v((1,0),(0,0))";
//  theParser.DefaultWeylLetter='G';
//  theParser.DefaultWeylRank=2;
//  civilizedInput="g_1 v((0,0,1), (1,0,0))";
//  civilizedInput="v((0,x_1,1),(1,0,0))";
//  civilizedInput="allLittelmannPaths(0,1)";

//  civilizedInput="splitIrrepOverLeviParabolic((1,1),(0,1))";


//  civilizedInput="gTwoInBthreeMultsParabolic((2,0,0), (1,0,0) )";
//  civilizedInput="splitIrrepOverLeviParabolic((1,1),(0,1))";
//  theParser.DefaultWeylLetter='B';
//  theParser.DefaultWeylRank=2;

//  civilizedInput="splitCharOverLeviParabolic(char(1,1),(0,1))";
//  civilizedInput="irreducibleRep(1,0)";
//  civilizedInput="isInproperSubmodule(g_1,1)";
//  civilizedInput="(h_1)/3";
//  civilizedInput="irreducibleRep(2,2)";
//  civilizedInput="irreducibleRep(1,1,1)";
//  theParser.DefaultWeylLetter='A';
//  theParser.DefaultWeylRank=2;
//  civilizedInput="";
//  civilizedInput="vpfReport((1,0,0), (0,1,0), (0,0,1))";
//  civilizedInput="vpf (1,-2)";
//  civilizedInput="littelmann(2)";
//  theParser.DefaultWeylLetter='A';
//  theParser.DefaultWeylRank=1;

//  civilizedInput="vpf( (1, 0, 0),(0, 1, 0),(0, 0, 1),(1, 1, 0),(0, 1, 1),(1, 1, 1),(0, 1, 2),(1, 1, 2),(1, 2, 2) )";
//  civilizedInput="-char(1,1,1)";
//  civilizedInput="char(0,0,0,0,0,0,0,1)char(0,0,0,0,0,1,0,1)";
//civilizedInput="freudenthal( char(0,0,0,0,0,0,1,2) )";
//  civilizedInput="char (2) *char(3)";
//  theParser.DefaultWeylLetter='E';
//  theParser.DefaultWeylRank=8;
//civilizedInput="char(0,0,0,0,0,1,0)*char(0,0,0,0,0,0,1)";
//theParser.DefaultWeylLetter='E';
//theParser.DefaultWeylRank=7;
//  civilizedInput="freudenthal(char (2,2,2,2,2,2) )";
//  civilizedInput="freudenthal(char(5))";
//  theParser.DefaultWeylLetter='E';
//  theParser.DefaultWeylRank=6;
//  civilizedInput="freudenthal(char(2,2,2))";
//  civilizedInput="char(0,0,1)+char(1,0,0)+char(0,0,1)";
//  civilizedInput="parabolicsInfoBruhatGraph(1,0,0,1)";
//  theParser.DefaultWeylRank=4;
//  theParser.DefaultWeylLetter='D';
//  civilizedInput="drawRootSystemFixProjection ((11,2,8),(5,6,1))";
//  civilizedInput="drawRootSystemIncludeWeylChamberAndDynkinLabels";
//  civilizedInput="InvariantsExteriorPowerFundamentalsPlusTrivials(0,0)";
//  civilizedInput="gTwoInBthreeMultsParabolic((212323123,12344124123,1423451), (1,0,0) )";
//  civilizedInput="gTwoInBthreeMultsParabolic((2,0,0), (1,0,0) )";
//  civilizedInput="drawRootSystemInCoxeterPlaneOfRootSA(3)";
  //civilizedInput="drawRootSystem";
//  civilizedInput="animateRootSystem(100, (0,1,0),(0,0,1), (0,1,0),(1,0,2) )";
//  civilizedInput="animateRootSystemDefault(1,3,50)";
//  civilizedInput="vpf((1), (2))";
//  civilizedInput="vpf((1,0),(0,1),(1,1),(1,3))";
//  civilizedInput="weightSupportGtwoGenVermaModule((0,1),(0,2))";
//  civilizedInput=" actByWeylAlgebraElement(d_1,x_1)";
//  civilizedInput="modOutVermaRelations  ( g_{-1}h_1) +g_{-1}";
//  civilizedInput="gTwoInBthreeMultsParabolic((2,0,0), (1,0,0) )";
//civilizedInput="(x_1+2x_3\\mapsto x_2, 3x_2\\mapsto x_1 :    f_{-3}(512x_3^{6}+2304x_2x_3^{5}+1536x_1x_3^{5}+2432x_3^{5}+4000x_2^{2}x_3^{4}+5760x_1x_2x_3^{4}+8480x_2x_3^{4}+1920x_1^{2}x_3^{4}\\&& +6080x_1x_3^{4}+4480x_3^{4}+3360x_2^{3}x_3^{3}+8000x_1x_2^{2}x_3^{3}+10720x_2^{2}x_3^{3}+5760x_1^{2}x_2x_3^{3}\\&& +16960x_1x_2x_3^{3}+11360x_2x_3^{3}+1280x_1^{3}x_3^{3}+6080x_1^{2}x_3^{3}+8960x_1x_3^{3}+4000x_3^{3}+1368x_2^{4}x_3^{2}\\&& +5040x_1x_2^{3}x_3^{2}+5832x_2^{3}x_3^{2}+6000x_1^{2}x_2^{2}x_3^{2}+16080x_1x_2^{2}x_3^{2}+9288x_2^{2}x_3^{2}\\&& +2880x_1^{3}x_2x_3^{2}+12720x_1^{2}x_2x_3^{2}+17040x_1x_2x_3^{2}+6552x_2x_3^{2}+480x_1^{4}x_3^{2}+3040x_1^{3}x_3^{2}\\&& +6720x_1^{2}x_3^{2}+6000x_1x_3^{2}+1728x_3^{2}+216x_2^{5}x_3+1368x_1x_2^{4}x_3+1152x_2^{4}x_3+2520x_1^{2}x_2^{3}x_3\\&& +5832x_1x_2^{3}x_3+2448x_2^{3}x_3+2000x_1^{3}x_2^{2}x_3+8040x_1^{2}x_2^{2}x_3+9288x_1x_2^{2}x_3+2592x_2^{2}x_3\\&& +720x_1^{4}x_2x_3+4240x_1^{3}x_2x_3+8520x_1^{2}x_2x_3+6552x_1x_2x_3+1368x_2x_3+96x_1^{5}x_3+760x_1^{4}x_3+2240x_1^{3}x_3\\&& +3000x_1^{2}x_3+1728x_1x_3+288x_3+108x_1x_2^{5}+342x_1^{2}x_2^{4}+576x_1x_2^{4}+420x_1^{3}x_2^{3}+1458x_1^{2}x_2^{3}\\&& +1224x_1x_2^{3}+250x_1^{4}x_2^{2}+1340x_1^{3}x_2^{2}+2322x_1^{2}x_2^{2}+1296x_1x_2^{2}+72x_1^{5}x_2+530x_1^{4}x_2\\&& +1420x_1^{3}x_2+1638x_1^{2}x_2+684x_1x_2+8x_1^{6}+76x_1^{5}+280x_1^{4}+500x_1^{3}+432x_1^{2}+144x_1\\&&) v+f_{-2}((-256x_3^{5}-1152x_2x_3^{4}-640x_1x_3^{4}-1216x_3^{4}-2000x_2^{2}x_3^{3}-2304x_1x_2x_3^{3}-4240x_2x_3^{3}-640x_1^{2}x_3^{3}-2432x_1x_3^{3}-2240x_3^{3}-1680x_2^{3}x_3^{2}-3000x_1x_2^{2}x_3^{2}-5360x_2^{2}x_3^{2}-1728x_1^{2}x_2x_3^{2}-6360x_1x_2x_3^{2}-5680x_2x_3^{2}-320x_1^{3}x_3^{2}-1824x_1^{2}x_3^{2}-3360x_1x_3^{2}-2000x_3^{2}-684x_2^{4}x_3-1680x_1x_2^{3}x_3-2916x_2^{3}x_3-1500x_1^{2}x_2^{2}x_3-5360x_1x_2^{2}x_3-4644x_2^{2}x_3-576x_1^{3}x_2x_3-3180x_1^{2}x_2x_3-5680x_1x_2x_3-3276x_2x_3-80x_1^{4}x_3-608x_1^{3}x_3-1680x_1^{2}x_3-2000x_1x_3-864x_3-108x_2^{5}-342x_1x_2^{4}-576x_2^{4}-420x_1^{2}x_2^{3}-1458x_1x_2^{3}-1224x_2^{3}-250x_1^{3}x_2^{2}-1340x_1^{2}x_2^{2}-2322x_1x_2^{2}-1296x_2^{2}-72x_1^{4}x_2-530x_1^{3}x_2-1420x_1^{2}x_2-1638x_1x_2-684x_2-8x_1^{5}-76x_1^{4}-280x_1^{3}-500x_1^{2}-432x_1-144)f_{-5}\\&&) v+f_{-1}((((-256x_3^{5}-768x_2x_3^{4}-640x_1x_3^{4}-1216x_3^{4}-848x_2^{2}x_3^{3}-1536x_1x_2x_3^{3}-2992x_2x_3^{3}-640x_1^{2}x_3^{3}-2432x_1x_3^{3}-2240x_3^{3}-408x_2^{3}x_3^{2}-1272x_1x_2^{2}x_3^{2}-2600x_2^{2}x_3^{2}-1152x_1^{2}x_2x_3^{2}-4488x_1x_2x_3^{2}-4192x_2x_3^{2}-320x_1^{3}x_3^{2}-1824x_1^{2}x_3^{2}-3360x_1x_3^{2}-2000x_3^{2}-72x_2^{4}x_3-408x_1x_2^{3}x_3-924x_2^{3}x_3-636x_1^{2}x_2^{2}x_3-2600x_1x_2^{2}x_3-2496x_2^{2}x_3-384x_1^{3}x_2x_3-2244x_1^{2}x_2x_3-4192x_1x_2x_3-2508x_2x_3-80x_1^{4}x_3-608x_1^{3}x_3-1680x_1^{2}x_3-2000x_1x_3-864x_3-36x_1x_2^{4}-108x_2^{4}-102x_1^{2}x_2^{3}-462x_1x_2^{3}-468x_2^{3}-106x_1^{3}x_2^{2}-650x_1^{2}x_2^{2}-1248x_1x_2^{2}-756x_2^{2}-48x_1^{4}x_2-374x_1^{3}x_2-1048x_1^{2}x_2-1254x_1x_2-540x_2-8x_1^{5}-76x_1^{4}-280x_1^{3}-500x_1^{2}-432x_1-144)f_{-6}\\&&+(384x_3^{4}+1152x_2x_3^{3}+768x_1x_3^{3}+1248x_3^{3}+1272x_2^{2}x_3^{2}+1728x_1x_2x_3^{2}+2760x_2x_3^{2}+576x_1^{2}x_3^{2}+1872x_1x_3^{2}+1488x_3^{2}+612x_2^{3}x_3+1272x_1x_2^{2}x_3+1992x_2^{2}x_3+864x_1^{2}x_2x_3+2760x_1x_2x_3+2148x_2x_3+192x_1^{3}x_3+936x_1^{2}x_3+1488x_1x_3+768x_3+108x_2^{4}+306x_1x_2^{3}+468x_2^{3}+318x_1^{2}x_2^{2}+996x_1x_2^{2}+756x_2^{2}+144x_1^{3}x_2+690x_1^{2}x_2+1074x_1x_2+540x_2+24x_1^{4}+156x_1^{3}+372x_1^{2}+384x_1+144)f_{-5}f_{-4}\\&&)) v)+f_12((((-128x_3^{5}-480x_2x_3^{4}-320x_1x_3^{4}-608x_3^{4}-640x_2^{2}x_3^{3}-960x_1x_2x_3^{3}-1568x_2x_3^{3}-320x_1^{2}x_3^{3}-1216x_1x_3^{3}-1120x_3^{3}-360x_2^{3}x_3^{2}-960x_1x_2^{2}x_3^{2}-1288x_2^{2}x_3^{2}-720x_1^{2}x_2x_3^{2}-2352x_1x_2x_3^{2}-1928x_2x_3^{2}-160x_1^{3}x_3^{2}-912x_1^{2}x_3^{2}-1680x_1x_3^{2}-1000x_3^{2}-72x_2^{4}x_3-360x_1x_2^{3}x_3-336x_2^{3}x_3-480x_1^{2}x_2^{2}x_3-1288x_1x_2^{2}x_3-888x_2^{2}x_3-240x_1^{3}x_2x_3-1176x_1^{2}x_2x_3-1928x_1x_2x_3-1056x_2x_3-40x_1^{4}x_3-304x_1^{3}x_3-840x_1^{2}x_3-1000x_1x_3-432x_3-36x_1x_2^{4}-90x_1^{2}x_2^{3}-168x_1x_2^{3}-72x_2^{3}-80x_1^{3}x_2^{2}-322x_1^{2}x_2^{2}-444x_1x_2^{2}-216x_2^{2}-30x_1^{4}x_2-196x_1^{3}x_2-482x_1^{2}x_2-528x_1x_2-216x_2-4x_1^{5}-38x_1^{4}-140x_1^{3}-250x_1^{2}-216x_1-72)f_{-7}\\&&+(-96x_2x_3^{3}-216x_2^{2}x_3^{2}-144x_1x_2x_3^{2}-216x_2x_3^{2}-156x_2^{3}x_3-216x_1x_2^{2}x_3-312x_2^{2}x_3-72x_1^{2}x_2x_3-216x_1x_2x_3-156x_2x_3-36x_2^{4}-78x_1x_2^{3}-108x_2^{3}-54x_1^{2}x_2^{2}-156x_1x_2^{2}-108x_2^{2}-12x_1^{3}x_2-54x_1^{2}x_2-78x_1x_2-36x_2)f_{-6}f_{-5}\\&&+(-96x_3^{3}-216x_2x_3^{2}-144x_1x_3^{2}-216x_3^{2}-156x_2^{2}x_3-216x_1x_2x_3-312x_2x_3-72x_1^{2}x_3-216x_1x_3-156x_3-36x_2^{3}-78x_1x_2^{2}-108x_2^{2}-54x_1^{2}x_2-156x_1x_2-108x_2-12x_1^{3}-54x_1^{2}-78x_1-36)f_{-5}^{2}f_{-4}\\&&)) v)+f_{1}((((-128x_3^{5}-512x_2x_3^{4}-320x_1x_3^{4}-608x_3^{4}-744x_2^{2}x_3^{3}-1024x_1x_2x_3^{3}-1800x_2x_3^{3}-320x_1^{2}x_3^{3}-1216x_1x_3^{3}-1120x_3^{3}-468x_2^{3}x_3^{2}-1116x_1x_2^{2}x_3^{2}-1728x_2^{2}x_3^{2}-768x_1^{2}x_2x_3^{2}-2700x_1x_2x_3^{2}-2164x_2x_3^{2}-160x_1^{3}x_3^{2}-912x_1^{2}x_3^{2}-1680x_1x_3^{2}-1000x_3^{2}-108x_2^{4}x_3-468x_1x_2^{3}x_3-540x_2^{3}x_3-558x_1^{2}x_2^{2}x_3-1728x_1x_2^{2}x_3-1020x_2^{2}x_3-256x_1^{3}x_2x_3-1350x_1^{2}x_2x_3-2164x_1x_2x_3-1020x_2x_3-40x_1^{4}x_3-304x_1^{3}x_3-840x_1^{2}x_3-1000x_1x_3-432x_3-54x_1x_2^{4}-117x_1^{2}x_2^{3}-270x_1x_2^{3}-93x_1^{3}x_2^{2}-432x_1^{2}x_2^{2}-510x_1x_2^{2}-72x_2^{2}-32x_1^{4}x_2-225x_1^{3}x_2-541x_1^{2}x_2-510x_1x_2-144x_2-4x_1^{5}-38x_1^{4}-140x_1^{3}-250x_1^{2}-216x_1-72)f_{-8}\\&&+(-16x_2x_3^{3}-52x_2^{2}x_3^{2}-24x_1x_2x_3^{2}-116x_2x_3^{2}-54x_2^{3}x_3-52x_1x_2^{2}x_3-220x_2^{2}x_3-12x_1^{2}x_2x_3-116x_1x_2x_3-166x_2x_3-18x_2^{4}-27x_1x_2^{3}-102x_2^{3}-13x_1^{2}x_2^{2}-110x_1x_2^{2}-150x_2^{2}-2x_1^{3}x_2-29x_1^{2}x_2-83x_1x_2-66x_2)f_{-7}f_{-5}\\&&+(16x_2x_3^{2}+28x_2^{2}x_3+16x_1x_2x_3+28x_2x_3+12x_2^{3}+14x_1x_2^{2}+24x_2^{2}+4x_1^{2}x_2+14x_1x_2+12x_2)f_{-6}f_{-5}^{2}+(16x_3^{2}+28x_2x_3+16x_1x_3+28x_3+12x_2^{2}+14x_1x_2+24x_2+4x_1^{2}+14x_1+12)f_{-5}^{3}f_{-4}\\&&)) v)+f_{2}((((-128x_3^{5}-448x_2x_3^{4}-320x_1x_3^{4}-608x_3^{4}-552x_2^{2}x_3^{3}-896x_1x_2x_3^{3}-1560x_2x_3^{3}-320x_1^{2}x_3^{3}-1216x_1x_3^{3}-1120x_3^{3}-288x_2^{3}x_3^{2}-828x_1x_2^{2}x_3^{2}-1260x_2^{2}x_3^{2}-672x_1^{2}x_2x_3^{2}-2340x_1x_2x_3^{2}-1844x_2x_3^{2}-160x_1^{3}x_3^{2}-912x_1^{2}x_3^{2}-1680x_1x_3^{2}-1000x_3^{2}-54x_2^{4}x_3-288x_1x_2^{3}x_3-324x_2^{3}x_3-414x_1^{2}x_2^{2}x_3-1260x_1x_2^{2}x_3-726x_2^{2}x_3-224x_1^{3}x_2x_3-1170x_1^{2}x_2x_3-1844x_1x_2x_3-792x_2x_3-40x_1^{4}x_3-304x_1^{3}x_3-840x_1^{2}x_3-1000x_1x_3-432x_3-27x_1x_2^{4}-72x_1^{2}x_2^{3}-162x_1x_2^{3}-69x_1^{3}x_2^{2}-315x_1^{2}x_2^{2}-363x_1x_2^{2}-28x_1^{4}x_2-195x_1^{3}x_2-461x_1^{2}x_2-396x_1x_2-72x_2-4x_1^{5}-38x_1^{4}-140x_1^{3}-250x_1^{2}-216x_1-72)f_{-9}\\&&+(64x_3^{4}+192x_2x_3^{3}+128x_1x_3^{3}+240x_3^{3}+180x_2^{2}x_3^{2}+288x_1x_2x_3^{2}+468x_2x_3^{2}+96x_1^{2}x_3^{2}+360x_1x_3^{2}+320x_3^{2}+54x_2^{3}x_3+180x_1x_2^{2}x_3+216x_2^{2}x_3+144x_1^{2}x_2x_3+468x_1x_2x_3+294x_2x_3+32x_1^{3}x_3+180x_1^{2}x_3+320x_1x_3+228x_3+27x_1x_2^{3}+45x_1^{2}x_2^{2}+108x_1x_2^{2}+24x_1^{3}x_2+117x_1^{2}x_2+147x_1x_2+72x_2+4x_1^{4}+30x_1^{3}+80x_1^{2}+114x_1+72)f_{-8}f_{-4}\\&&+(16x_2x_3^{3}+20x_2^{2}x_3^{2}+24x_1x_2x_3^{2}-12x_2x_3^{2}+6x_2^{3}x_3+20x_1x_2^{2}x_3-36x_2^{2}x_3+12x_1^{2}x_2x_3-12x_1x_2x_3-90x_2x_3+3x_1x_2^{3}-18x_2^{3}+5x_1^{2}x_2^{2}-18x_1x_2^{2}-72x_2^{2}+2x_1^{3}x_2-3x_1^{2}x_2-45x_1x_2-54x_2)f_{-7}f_{-6}\\&&+(32x_3^{3}+72x_2x_3^{2}+48x_1x_3^{2}+72x_3^{2}+60x_2^{2}x_3+72x_1x_2x_3+144x_2x_3+24x_1^{2}x_3+72x_1x_3+4x_3+18x_2^{3}+30x_1x_2^{2}+72x_2^{2}+18x_1^{2}x_2+72x_1x_2+30x_2+4x_1^{3}+18x_1^{2}+2x_1-24)f_{-7}f_{-5}f_{-4}\\&&+(16x_2x_3^{2}+20x_2^{2}x_3+16x_1x_2x_3+36x_2x_3+6x_2^{3}+10x_1x_2^{2}+24x_2^{2}+4x_1^{2}x_2+18x_1x_2+18x_2)f_{-6}^{2}f_{-5}+(16x_3^{2}+12x_2x_3+16x_1x_3+44x_3+6x_1x_2+24x_2+4x_1^{2}+22x_1+24)f_{-6}f_{-5}^{2}f_{-4}\\&&+(-8x_3-6x_2-4x_1-6)f_{-5}^{3}f_{-4}^{2})) v)+f_{3}((((64x_3^{4}+224x_2x_3^{3}+128x_1x_3^{3}+256x_3^{3}+276x_2^{2}x_3^{2}+336x_1x_2x_3^{2}+636x_2x_3^{2}+96x_1^{2}x_3^{2}+384x_1x_3^{2}+368x_3^{2}+144x_2^{3}x_3+276x_1x_2^{2}x_3+498x_2^{2}x_3+168x_1^{2}x_2x_3+636x_1x_2x_3+574x_2x_3+32x_1^{3}x_3+192x_1^{2}x_3+368x_1x_3+224x_3+27x_2^{4}+72x_1x_2^{3}+126x_2^{3}+69x_1^{2}x_2^{2}+249x_1x_2^{2}+225x_2^{2}+28x_1^{3}x_2+159x_1^{2}x_2+287x_1x_2+150x_2+4x_1^{4}+32x_1^{3}+92x_1^{2}+112x_1+48)f_{-9}f_{-5}\\&&+(-64x_3^{4}-160x_2x_3^{3}-128x_1x_3^{3}-256x_3^{3}-132x_2^{2}x_3^{2}-240x_1x_2x_3^{2}-468x_2x_3^{2}-96x_1^{2}x_3^{2}-384x_1x_3^{2}-368x_3^{2}-36x_2^{3}x_3-132x_1x_2^{2}x_3-246x_2^{2}x_3-120x_1^{2}x_2x_3-468x_1x_2x_3-434x_2x_3-32x_1^{3}x_3-192x_1^{2}x_3-368x_1x_3-224x_3-18x_1x_2^{3}-27x_2^{3}-33x_1^{2}x_2^{2}-123x_1x_2^{2}-99x_2^{2}-20x_1^{3}x_2-117x_1^{2}x_2-217x_1x_2-138x_2-4x_1^{4}-32x_1^{3}-92x_1^{2}-112x_1-48)f_{-8}f_{-6}\\&&+(16x_3^{4}+48x_2x_3^{3}+32x_1x_3^{3}+64x_3^{3}+44x_2^{2}x_3^{2}+72x_1x_2x_3^{2}+116x_2x_3^{2}+24x_1^{2}x_3^{2}+96x_1x_3^{2}+92x_3^{2}+12x_2^{3}x_3+44x_1x_2^{2}x_3+46x_2^{2}x_3+36x_1^{2}x_2x_3+116x_1x_2x_3+82x_2x_3+8x_1^{3}x_3+48x_1^{2}x_3+92x_1x_3+56x_3+6x_1x_2^{3}+11x_1^{2}x_2^{2}+23x_1x_2^{2}+6x_2^{2}+6x_1^{3}x_2+29x_1^{2}x_2+41x_1x_2+6x_2+x_1^{4}+8x_1^{3}+23x_1^{2}+28x_1+12)f_{-7}^{2}\\&&+(8x_2x_3^{2}+12x_2^{2}x_3+8x_1x_2x_3+16x_2x_3+3x_2^{3}+6x_1x_2^{2}+5x_2^{2}+2x_1^{2}x_2+8x_1x_2+16x_2)f_{-7}f_{-6}f_{-5}+(8x_3^{2}+12x_2x_3+8x_1x_3+16x_3+3x_2^{2}+6x_1x_2+3x_2+2x_1^{2}+8x_1+18)f_{-7}f_{-5}^{2}f_{-4}\\&&+(64x_3^{3}+144x_2x_3^{2}+96x_1x_3^{2}+168x_3^{2}+108x_2^{2}x_3+144x_1x_2x_3+252x_2x_3+48x_1^{2}x_3+168x_1x_3+140x_3+27x_2^{3}+54x_1x_2^{2}+99x_2^{2}+36x_1^{2}x_2+126x_1x_2+126x_2+8x_1^{3}+42x_1^{2}+70x_1+12)f_{-8}f_{-5}f_{-4}\\&&+(x_2^{2}-x_2)f_{-6}^{2}f_{-5}^{2}+(2x_2-2)f_{-6}f_{-5}^{3}f_{-4}+f_{-5}^{4}f_{-4}^{2})) v) )";
 // civilizedInput="x_1f_1";
  //For debugging:
//  civilizedInput="(x_1+2x_3\\mapsto x_2, 3x_2\\mapsto x_1 :    modOutVermaRelations ( i(c) ) )";
//  civilizedInput="x_1^-1";
//  civilizedInput="(x_1+x_2\\mapsto x_1: x_1)";
//  civilizedInput="modOutSAVerma(f_1f_4)";
//  civilizedInput="adjointRepresentationAction(x_1g_1,g_2)";
//  civilizedInput="modOutVermaRelationsOrdered(c)";
//  civilizedInput="modOutVermaRelations(c)";
//  civilizedInput="decomposeXtimesVinGenericVerma(1)";
//  civilizedInput="sliceDirections( (1,0,0),(0,1,0),(0,0,1),(1,1,0), (0,1,1),(1,1,1) )";
//  civilizedInput="drawWeightSupport(0,5,2,0)";
//  theParser.DefaultWeylRank=4;
//  theParser.DefaultWeylLetter='F';
  //civilizedInput="drawConeAffine( coneFromNormals((1,0,0,0),(0,1,0,0),(0,-1,1,0), (-1,-1,-1,5/2), (0,0,0,1)))";
//  civilizedInput=" gTwoInBthreeMultsParabolic((2,0,0), (1,0,0) )";
//  civilizedInput="gTwoInBthreeMultsParabolic((3,0,0), (1,0,0) )";
  //civilizedInput="printSlTwosAndRootSAsFORCERecompute";
//  civilizedInput="actByWeylRho(x_1, x_2,x_3, x_4, x_5, x_6)";
//  theParser.DefaultWeylRank=6;
//  theParser.DefaultWeylLetter='E';
  //civilizedInput="parabolicsInfoBruhatGraph(0,0,0)";
  //civilizedInput="vpf((1, 0, 0),(0, 1, 0),(0, 0, 1),(1, 1, 0),(0, 1, 1),(1, 1, 1),(0, 1, 2),(1, 1, 2),(1, 2, 2))";
  //civilizedInput="parabolicsInfoBruhatGraph";
//  civilizedInput="coneFromVertices((1,0), (0,1)) ";
//  civilizedInput="coneFromNormals(0) ";
//  civilizedInput="coneFromVertices((1,0,0),(0,1,0), (0,0,1),(0,1,1), (1,1,0),(1,1,1)) ";
 //civilizedInput="cone( (3, -2, 1, -8), (-1, 1, 0, -3), (-1, 2, -1, 0), (-2, -2, 2, 3), (1, 0, -1, 5) )";
//  civilizedInput="cone ((2, -2, -2, 2, -21), (0, 1, 2, -1, -12), (-1, 2, -2, 2, 0), (0, -2, 1, 0, 2), (-1, 2, 0, -2, 24) )";
 // civilizedInput="drawConeProjective( cone((1,0),(-1,0), (1, -1), (1,1)) )";
//  civilizedInput="drawConeProjective( cone((1,0),(-1,0), (1, -1)) )";
 // civilizedInput="cone((1,0),(-1,0))";
//  civilizedInput="drawConeAffine( cone((1,0,0,0)))";
//  civilizedInput="RunGtwoInBthree";
//  civilizedInput="latticeImprecise((1, 0.000000001), (1.00000000001, 1.00000000001))";
//  civilizedInput= "animateRootSystemDefault(4,6,1)+animatePause(380)+animateRootSystemBlueDot(0,2,1, (1,0))+animatePause(49)
//    +animateRootSystemBlueDot(0,2,1, (1,1))+animatePause(19)+animateRootSystemBlueDot(0,2,1, (0,1))+animatePause(19)
//+animateRootSystemBlueDot(0,2,1, (-1,0))+animatePause(19)
//+animateRootSystemBlueDot(0,2,1, (-1,-1))
//    +animatePause(19)+animateRootSystemBlueDot(0,2,1, (0,-1))
//    +animatePause(19)+animateRootSystemDefault(0,2,1)+animatePause(79)+animateRootSystemDefault(1,3,100)+animatePause(20)
//    +animateRootSystemDefault(2,4,100)+animatePause(20)+animateRootSystemDefault(3,5,1)+animatePause(79)+animateRootSystemDefault(6,2,1)
//    +animatePause(29)+animateRootSystemDefault(5,4,1)+animatePause(59)+animateRootSystemDefault(4,6,1)+animatePause(59)
//    +animateRootSystemDefault(4,7,1)+animatePause(59)+animateRootSystemDefault(4,8,1)+animatePause(59)+animateRootSystemDefault(6,2,1)
//    +animatePause(99)+animateRootSystemDefault(4,8,1)+animatePause(119)+animateRootSystemDefault(4,7,1)+animatePause(119)
//    +animateRootSystemDefault(5,4,100)+animatePause(50)+animateRootSystemDefault(4,6,100)+animatePause(230)";
  //civilizedInput="(x_1\\mapsto factorial(10):x_1)";
  //civilizedInput="getWeylDim(0,0,(x_1\\mapsto 10: x_1))";
  //civilizedInput="factorial(100)/2";
  //civilizedInput=" actByWeyl(x_1, x_2, x_3)";
  //theParser.DefaultWeylRank=6;
  //theParser.DefaultWeylLetter='E';
  //civilizedInput="drawRootSystem";
  //civilizedInput="findMaximumInDirectionOverLatticeShifted(-2x_1+x_2+x_3, (0,0,0), lattice((1,0,0),(0,1,0),(0,0,1)), cone((1,0,0,0),(0,1,0,0), (1,1,1,-4), (0,1,-1,0), (0,0,0,1)),2)";
  //civilizedInput="findMaximumInDirectionOverLatticeShifted(x_1, (0,0), lattice((2,0),(1,1)), cone((1,0,0),(-1,1,0),(-2,-1,10)),1)";
//  civilizedInput="findExtremaInDirectionOverLatticeShifted(x_1, (0,0), lattice((2,0),(1,1)), cone((1,0,0),(-1,1,0),(-2,-1,10)),1)";
 //civilizedInput="drawConeAffine( cone((0,3,-10),(1,0,0),(-2,-1,10) ))";
 // civilizedInput="sliceConeInUniqueExitWall( cone((1,0,0),(-1,1,0),(-2,-1,10)), (-1,0,0) )";
  //civilizedInput="findExtremaInDirectionOverLatticeShifted(-2x_1+x_2+x_3, (0,0,0), lattice((1,0,0),(0,1,0),(0,0,1)), cone((1,0,0,0),(0,1,0,0), (1,1,1,-4), (0,1,-1,0), (0,0,0,1)),1)";
  //civilizedInput=" 1+cone(1)";
//  civilizedInput="drawConeAffine( cone((1,1,1),(1,0,1),(0,1,0)))";
  //civilizedInput=" findExtremaInDirectionOverLatticeShiftedOneNonParam(-2x_1+x_2, (0,0), lattice((1,0),(0,1)), cone((-1,1,0),(1,0,0), (-2,-1,4)))";
  //civilizedInput="sliceConeInUniqueExitWall( cone((1,0,0),(0,1,0),(0,0,1)), (1,1,1) )";
 // civilizedInput="factorial(20000)";
  //civilizedInput="10000000000";
  //civilizedInput="getPointOnLatticeClosestToWallInDirection( (2,3), (1,1/5,1/3), lattice((1,1), (0,2)))";
//  civilizedInput="intersectLatticeWithPreimageOfLattice(  lattice((1,1), (0,2)), lattice(26/5), (1,1/5) )";
  //civilizedInput="solveLPolyEqualsZeroOverCone(x_1+x_2-1/2,cone((1,0,0), (0,1,0), (1, 1, 1) ))";
  //civilizedInput="printSlTwos";
  //civilizedInput="invariantsSlTwoOfDegree(30,(2,3))";
  //civilizedInput="(x_1\\mapsto x_1, x_2\\mapsto x_2-1: 1/4x_1^2-1/4x_2^2+x_1+1)";
  //civilizedInput="printRootSubalgebras";
  //civilizedInput="getRelations(x_1^2, x_1x_2, x_2^2)";
  //civilizedInput="vpf((1,-3), (1,-1), (1,1), (1,3))";
  //civilizedInput="split(PartialFraction((1,0), (1,2), (1,-2)))";
//  civilizedInput="PartialFraction(1,1,(0,1))";
  //civilizedInput="slTwoInSlN(3)";
 // theParser.DefaultWeylLetter='A';
//  civilizedInput="(0,1)";
//  civilizedInput="Cone((1,0),(0,1), (1,1))";
//  civilizedInput="(i(c))^2";
  //civilizedInput="IntersectHyperplaneByCone(Cone((1,0), (0,1)), (1,-1))";
//  civilizedInput="QPSubAmbientLatticeZn(QuasiPolynomial(x_1+4x_2, (0,0), Lattice((1,0),(0,2))), ( (1,0),(0,1),(1,1),(1,2)), (0,0))";
  //civilizedInput="QPSubAmbientLatticeZn(QuasiPolynomial(x_1+4x_2, (0,0), Lattice((1,0),(0,2))), ( (1,0),(0,1),(1,1),(1,2)), (0,1))";
  //civilizedInput="Lattice((1,1/2), (1,-1/3))";
  //civilizedInput="IntersectLatticeWithPreimageOfLattice(Lattice((1,1,1,0), (0,2,1,1), (0,0,2,0), (0,0,0,2)),  Lattice((1,1,0),(0,2,0)), (1,2,0),(0,1,1),(1,0,1), (0,0,1) )";
  //civilizedInput="Lattice((1,0,1),(1,1,0))*Lattice((1,0,0),(1,2,-2))";
  //civilizedInput="IntersectLatticeWithSubspaces(Lattice((1/2,1/2,1/2), (0,1,2), (0,0,5)),(0,1,2),(1,0,0))";
  //civilizedInput="IntersectLatticeWithSubspaces(Lattice((1/2,1/2,1/2), (0,1,2), (0,0,5)),(0,1,2))";
  //civilizedInput="ReadFromFile";
  //civilizedInput="WriteToFile(Cone((0,1),(1,0)))";
  //civilizedInput= "(x_1\\mapsto x_1-3x_2, x_2\\mapsto 1/2x_2: Lattice((1,1),(0,2)))";
  //civilizedInput="(x_1\\mapsto x_1, x_2\\mapsto x_1: Lattice((1,0),(0,1)))";
  //civilizedInput="vpf(PartialFractions((1,0),(0,1),(1,1)),(1,2))";
  //civilizedInput="split(PartialFractions((1,0),(0,1),(1,1)))";
  //civilizedInput="PartialFractions((1,0),(0,1),(1,1))";
  //civilizedInput="MaximumQuasipolynomialsOverCone(QuasiPolynomial(x_1+4x_2, (0,0), Lattice((1,0),(0,2))),QuasiPolynomial(x_2, (0,0), Lattice((1,1),(0,2))),QuasiPolynomial(x_1+3x_2+1/2, (0,0), Lattice((0,1),(1,0))), Cone ((1,2,0),(1,-1,0),(0,0,1)))";
  //civilizedInput="GetAllRepresentatives(Lattice((7,4),(1,3)),Lattice((1,1),(1,0)))";
  //civilizedInput="Lattice((1,1),(1,0))Lattice((7,4),(1,3))";
  //civilizedInput="GetAllRepresentatives(Lattice((1,1),(1,0)),Lattice((7,4),(1,3)))";
  //civilizedInput="Lattice((1,2),(0,3))Lattice((1,0),(0,3))";
  //civilizedInput="QuasiPolynomial(x_1, (1,2), Lattice((1,2),(0,3)))+QuasiPolynomial(x_2, (0,0), Lattice((1,1),(1,0)))";
  //civilizedInput="mod([i(c),g_{-9}^{n_{12}}g_{-8}^{n_{11}}g_{-7}^{n_{10}}g_{-6}^{n_9}g_{-5}^{n_8}g_{-4}^{n_7}g_{-3}^{n_6}g_{-2}^{n_5}g_{-1}^{n_4}])";
  //civilizedInput= "QuasiPolynomial(x_1+x_2, (10101/71,10000/99), Lattice((2,1/8),(5,10)))";
  //civilizedInput="printEmbedding";
  //civilizedInput="Lattice((2,1,2), (3,2,1), (2,3,5))";
  //civilizedInput="(d_11\\mapsto 0 : secretSauceOrdered)";
  //civilizedInput="maximumLinearFunctionOverCone(x_1+x_2+x_3, x_1-x_3+2x_2, -x_1-x_3, Cone((1,2,3),(3,-5,0),(1,-1,2), (0,0,1), (0,1,0),(1,0,0)))";
  //civilizedInput="1/(x_1+1)+1/(-x_1+1)";
  //civilizedInput="combinatorialChamberParam(1, 2, (-1,1,0), (1,0,0), (-1,-1,1))";
  //civilizedInput="secretSauceOrdered";
  //civilizedInput="slTwoInSlN(2,2)";
  //theParser.DefaultWeylLetter='E';
  //theParser.DefaultWeylRank=6;
  //civilizedInput="actByWeylAffine(x_1,x_2,x_3,x_4,x_5,x_6)";
  //civilizedInput="(x_2\\mapsto 2, x_1\\mapsto 1:x_1x_2)";
  //theParser.DefaultWeylLetter='A';
  //theParser.DefaultWeylRank=2;
   // civilizedInput="gcd(x_1^3+x_2^3+x_3^3, x_1^{5}+x_2^{5}+x_3^{5})";
  //theParser.DefaultWeylRank=3;
  //theParser.DefaultWeylLetter='B';
  //SemisimpleSubalgebras theComputation;
  //theComputation.FindHCandidates('D', 4, theGlobalVariables);
  //theParser.theHmm.MakeGinGWithId('B', 3, theGlobalVariables);
  //  EigenVectorComputation theEigen;
  //  std::cout << theEigen.ComputeAndReturnString(theGlobalVariables, theParser);
  if (!CGI::FileExists(theParser.indicatorFileName))
  { std::fstream tempFile;
    CGI::OpenFileCreateIfNotPresent(tempFile, theParser.indicatorFileName, false, true, false);
    tempFile << "<html><body>" << theParser.javaScriptDisplayingIndicator << "</body></html>";
    tempFile.flush();
    tempFile.close();
  }
  ANNOYINGSTATISTICS;
  if (theParser.DefaultWeylLetter!='B' || theParser.DefaultWeylRank!=3)
  { HomomorphismSemisimpleLieAlgebra& theHmm=theParser.theHmm;
    theHmm.MakeGinGWithId(theParser.DefaultWeylLetter, theParser.DefaultWeylRank, theParser.theAlgebras, theGlobalVariables);
    theParser.initTestAlgebraNeedsToBeRewritteN(theGlobalVariables);
  }
  else
    theParser.initTestAlgebraNeedsToBeRewrittenG2InB3(theGlobalVariables);
  ANNOYINGSTATISTICS;
  double TimeParsing=0, TimeEvaluation=0;
  //std::cout << "before parsing numvars is: " << theParser.NumVariables;
  CGI::GetHtmlStringSafeishReturnFalseIfIdentical(civilizedInput, tempS);
  civilizedInput=tempS;

  PolynomialOutputFormat theOutputFormat;
  theOutputFormat.alphabetBases.SetSize(2);
  theOutputFormat.MakeAlphabetArbitraryWithIndex("x", "y");
  theOutputFormat.alphabetBases[0]="g";
  theOutputFormat.alphabetBases[1]="h";
  ANNOYINGSTATISTICS;
  theParser.ParseEvaluateAndSimplifyPart1(civilizedInput, theGlobalVariables);
  ANNOYINGSTATISTICS;
  TimeParsing=GetElapsedTimeInSeconds();
  theOutputFormat.flagUseCalculatorFormatForUEOrdered=true;
  std::string theResult = theParser.ParseEvaluateAndSimplifyPart2(civilizedInput, true, theGlobalVariables, theOutputFormat);
  TimeEvaluation=GetElapsedTimeInSeconds()-TimeParsing;
  theParser.DefaultWeylLetter=theParser.theHmm.theRange().theWeyl.WeylLetter;
  theParser.DefaultWeylRank=theParser.theHmm.theRange().theWeyl.CartanSymmetric.NumRows;
  //std::cout << "Computation done in " << GetElapsedTimeInSeconds() << "seconds <br>";
  std::cout.flush();
  theParser.ComputeDebugString(true, theGlobalVariables, theOutputFormat);

  std::string beginMath = "<div class=\"math\" scale=\"50\">";
  std::string endMath = "</div>";
  std::cout << "<table>\n <tr valign=\"top\">\n <td>";
  std::cout << "\n<FORM method=\"POST\" name=\"formCalculator\" action=\"" << theParser.DisplayNameCalculator << "\">\n" ;
  std::stringstream tempStream4;
  static_html4(tempStream4);
  std::cout << GetSelectHTMLStringTEmp(optionsType, optionsRank, inputWeylString, inputRankString) ;
  std:: cout << tempStream4.str();
//  std::cout << "<input type=\"text\" size =\"1\" name=\"weylLetterInput\" value=\"" << theParser.DefaultWeylLetter << "\"></input>";
//  std::cout << "<input type=\"text\" size =\"1\" name=\"weylRankInput\" value=\"" << theParser.DefaultWeylRank << "\"></input>";
  std::cout << "\n<br>\n";
  std::string civilizedInputSafish;
  if (CGI::GetHtmlStringSafeishReturnFalseIfIdentical(civilizedInput, civilizedInputSafish))
    std::cout << "Your input has been treated normally, however the return string of your input has been modified. More precisely, &lt; and &gt;  are modified due to a javascript hijack issue. ";
  std::cout << "<textarea rows=\"3\" cols=\"30\" name=\"textInput\" id=\"textInputID\" onkeypress=\"if (event.keyCode == 13) {storeSettings();  this.form.submit(); return false;}\">";
  std::cout << civilizedInputSafish;
  std::cout << "</textarea>\n<br>\n";
  std::cout << "<input type=\"submit\" name=\"buttonGo\" value=\"Go\" onmousedown=\"storeSettings();\" > ";
//  std::cout << "<a href=\"/tmp/indicator.html\" target=\"_blank\"> Indicator window  </a>";
  std::cout << "\n</FORM>";
  std::cout << "<script type=\"text/javascript\"> document.getElementById(\"textDim\").value=" << theParser.DefaultWeylRank << "; \n";
  std::cout << "document.getElementById(\"textType\").value=\"" << theParser.DefaultWeylLetter << "\"; </script>";
//  \n<FORM method=\"POST\" name=\"formCalculator\" action=\"/cgi-bin/calculator\">\n <input type=\"textarea\" rows=\"60\" cols=\"60\" name=\"textInput\" \"></textarea>\n<br>\n";
#ifndef WIN32
  double TimeTotalElapsed=GetElapsedTimeInSeconds();
  if (civilizedInput!="")
  { std::cout << "<a href=\"./calculator?" << inputString << "\">Link to your computation. </a>" ;
    std::cout << "<hr><b>Result.</b> "<< theResult
    << "<br><hr><br>Parsing+evaluation time: "
    << TimeTotalElapsed << " seconds<br> (" << TimeParsing << " parsing + "
    << TimeEvaluation << " evaluation + " << TimeTotalElapsed-TimeEvaluation-TimeParsing
    << " processing)<br>";
    std::cout << "#  pointers at peak RAM: " << ParallelComputing::PointerCounterPeakRamUse << "<br>(excluding std::string, std::stringstream)<br>";
  } //else
    //std::cout << "Empty input base boot time: " << TimeTotalElapsed;
  if (GetElapsedTimeInSeconds()>1)
    std::cout << "If your computation takes too long it is probably due to the weak algorithms used for your computation.<br> Feel free to email the author(s) with requests for speed improvement.";
  ComputationComplete=true;
#endif
  std::cout.flush();
  std::cout << "</td>";
  std::cout << " <td style=\"width:300\"> <b>List of available functions.</b><br> " << theParser.GetFunctionDescription() << "</td>\n";
  std::cout << "<td>";
  std::cout << "<b>Links. </b><br>"
  //<< "<button onclick=\"switchMenu('idLinksText');\">Show/hide links</button> <div id=\"idLinksText\" style=\"display: none\">"
  << "<a href=\"http://wwwmathlabo.univ-poitiers.fr/~maavl/LiE/form.html\"> LiE software online </a>"
  << "<br>"
  << "<a href=\"http://www.gap-system.org/Download/index.html\">GAP </a>, see also "
  << "<a href=\"http://www.science.unitn.it/~degraaf/sla.html\">Willem A. de Graaf's SLA package </a>"
  << "<br><a href=\"http://www.liegroups.org/\">Atlas of Lie groups </a> "
  // << " <br> <a href=\"http://www.cs.kuleuven.be/cgi-bin/dtai/barvinok.cgi\"> Barvinok program online</a>"
  //<< "</div>"
  << "";
  std::cout << "<hr><b>Notes.</b><br> Computation is limited to "
  << theGlobalVariables.MaxAllowedComputationTimeInSeconds
  << " seconds. The time limit can be changed if you install from source by editing (file) calculator.cpp (function) main() (variable) "
  << "theGlobalVariables.MaxAllowedComputationTimeInSeconds ."
  << "<br> Clicking \"Go\" + blank screen = calculator bug. <br> Clicking \"Go\" + \"Internal server error\"=  serious calculator bug.<br> Clicking \"Go\" + wrong result= <b>very serious calculator bug</b>.";
  std::cout << "<br>Bug reports = our wholehearted gratitude.<br><button " << CGI::GetStyleButtonLikeHtml() << " onclick=\"switchMenu('sourceDetails');\" >C++ source of the calculator</button>";
  std::cout << "<button " << CGI::GetStyleButtonLikeHtml() << " onclick=\"switchMenu('debugDetails');\">Debugging info</button>";
  std::cout << "<div id=\"sourceDetails\" style=\"display: none\">";
	std::cout << " <br>\n";
  std::cout << " <a href=\"http://vectorpartition.svn.sourceforge.net/viewvc/vectorpartition/trunk/vpf.h?view=markup\"> Vector partition c++(1 out of 4 files (header file))</a>\n";
  std::cout << " <br>\n";
  std::cout << " <a href=\"http://vectorpartition.svn.sourceforge.net/viewvc/vectorpartition/trunk/vpf9.cpp?view=markup\"> Vector partition c++(2 out of 4 files (.cpp))</a>\n";
  std::cout << " <br>\n";
  std::cout << " <a href=\"http://vectorpartition.svn.sourceforge.net/viewvc/vectorpartition/trunk/vpf8.cpp?view=markup\"> Vector partition c++(3 out of 4 files (.cpp))</a>\n";
  std::cout << " <br>\n";
  std::cout << " <a href=\"http://vectorpartition.svn.sourceforge.net/viewvc/vectorpartition/trunk/vpf7.cpp?view=markup\"> Vector partition c++(4 out of 4 files (.cpp))</a>\n";
  std::cout << " <br>\n";
  std::cout << " <a href=\"http://vectorpartition.svn.sourceforge.net/viewvc/vectorpartition/trunk/calculator.cpp?view=markup\"> Calculator interface c++ (1 out of 2 files)</a>\n";
  std::cout << " <br>\n";
  std::cout << " <a href=\"http://vectorpartition.svn.sourceforge.net/viewvc/vectorpartition/trunk/RootSystem.html.cpp?view=markup\"> Calculator interface c++ (2 out of 2 files)</a>\n";
  std::cout << " <br>\n";
  std::cout << " The calculator is a simple console application (like the C++ \"Hello world!\"). It is managed by an <a href=\"http://httpd.apache.org/\">Apache web server</a>. ";
  std::cout << " <br>The calculator errors get caught either by 1) in-line asserts() (blank screen), or 2) by Apache/the system (internal server error)."
  << "  \n<br> All precomputed data is stored <a href=\""
  << theParser.DisplayPathOutputFolder
  <<"\">here</a> (the precomputed data is stored as usual files). \
  <br> The file input/output is done via std::fstream. <br>The LaTeX'ing is called using std::system() \
  calls. The LaTeX logs can be found by viewing the calculator page's source. <br> \
  The html output is hardcoded: either by hand or transformed from a separate .html file using a \
  micro-tool written for the purpose. ";
  std::cout << " ";
  std::cout << " \n";
  std::cout << "<hr><b>Installing the calculator on your machine from c++ source. </b><br> \
  In order to get the calculator running on your machine you need to do the following. \
  Simplifying the installation procedure is on our to-do list. \
  <br>0) You need a Linux machine. Tested it only on Ubuntu and OpenSUSE. If you are interested in making the system run on Windows please write us an email. \
  <br>1) Download the c++ files in the links above. Alternatively you can check out the program's "
  << " svn repository. The svn check out command is  &nbsp&nbsp  svn co https://vectorpartition.svn.sourceforge.net/svnroot/vectorpartition vectorpartition &nbsp&nbsp."
  << "<br>2) Put them in a c++ project and make sure the following includes work:"
  << " #include &lt;sys/time.h&gt; #include &lt;unistd.h&gt; #include &lt;pthread.h&gt;. "
  << "They should work by default on almost any Linux distro. "
  << "<br>3) Build the project to a console application named  &nbsp&nbsp calculator &nbsp&nbsp with default console application settings.  "
  << "<br>4) Create a folder to contain the server files; assume without loss of generality the so created folder is called "
  << "&nbsp&nbsp ServerBase/    &nbsp&nbsp."
  << "<br>5) Create folders &nbsp&nbsp ServerBase/cgi-bin  &nbsp&nbsp and &nbsp&nbsp ServerBase/output &nbsp&nbsp. "
  << "<br>6) Enable full read/write access for every user in the folder &nbsp&nbsp ServerBase/output  &nbsp&nbsp. "
  << "<br>7) Copy the file &nbsp&nbsp calculator  &nbsp&nbsp into &nbsp&nbsp ServerBase/cgi-bin/ &nbsp&nbsp and allow read/execute access to every user."
  << "<br> 8) Install an <a href=\"http://httpd.apache.org/\">Apache web server</a> and enable cgi scripts "
  << "from folder &nbsp&nbsp ServerBase/cgi-bin/ &nbsp&nbsp."
  << "<br>9) Configure the Apache server so the adress of physical folder &nbsp&nbsp ServerBase/ &nbsp&nbsp is displayed as &nbsp&nbsp /vpf/ &nbsp&nbsp."
  << "<br>10) The basic installation is now complete; test the calculator by running it through your web browser."
  << "<br>11) To finish the installation install the jsmath in folder &nbsp&nbsp ServerBase/jsmath/ &nbsp&nbsp.";
	std::cout <<	"</div>";
  std::cout << "<div id=\"debugDetails\" style=\"display: none\">";
  std::cout << "<br>Debugging printouts follow.<br>Number of pointers used:" << ParallelComputing::GlobalPointerCounter << "<br>raw input string: " << inputString;
  std::cout << "<br>civilized input strings: " << civilizedInputSafish << "<br> chopped strings: <br>";
  for (int i=0; i<inputStrings.size; i++)
    std::cout << "string " << i << ": " << inputStrings.TheObjects[i] << "<br>";
  std::cout << "system commands: <br>";
  theGlobalVariables.MaxAllowedComputationTimeInSeconds=10000;
  for (int i=0; i< theParser.SystemCommands.size; i++)
    std::cout << theParser.SystemCommands.TheObjects[i] << "<br>";
  std::cout << "<br>Parser debug string:<br> " << theParser.DebugString;
//  theParser.theLieAlgebra.ComputeDebugString();
//  std::cout << "<br>details:<br> " << theParser.theLieAlgebra.ElementToStringLieBracketPairing();
  std::cout << "</div>";
  theGlobalVariables.SetCallSystem(&CallSystemWrapper);

  std::cout << theParser.CreateBasicStructureConstantInfoIfItDoesntExist(theGlobalVariables);
  //  std::cout << "<button onclick=\"switchMenu('rootSystem');\" >Root system</button>";
//  std::cout << "<div id=\"rootSystem\" style=\"display: none\">";
//  std::cout << "<hr><b> Root system visualization</b><br> The basis vectors (small red cirles) can be dragged with the mouse pointer." <<
  std::cout <<  " <br> <a href=\""
  << theParser.DisplayNameCalculator << "?%20textType=" << theParser.DefaultWeylLetter
  << "&textDim=" << theParser.DefaultWeylRank << "&textInput=drawRootSystem"
  << "\">Root system visualization (calculator generated).</a>";
  std::cout <<  " <br> <a href=\"/vpf/RootSystem_no_autocomplete.html\">"
  << "Root system visualization (self-contained javascript).</a>";
  std::cout <<  " <br> <a href=\""
  << theParser.DisplayNameCalculator << "?%20textType=" << theParser.DefaultWeylLetter
  << "&textDim=" << theParser.DefaultWeylRank << "&textInput=printRootSystem"
  << "\">Root system conventions used by the calculator</a>";
//  << "<br> At the moment the G_2 and F_4 long-short root convention is different from LiE/Humphreys/wikipedia."
//  << " Changing according to the preceding convention.";

//  << " uses javascript (+ dojo script from google for drawing lines and cirles).<br>";
  std::stringstream tempStream3;
  static_html5(tempStream3);
//  static_html6(tempStream3);
  std::cout << tempStream3.str();
//  std::cout << "</div>";

  std::cout << "\n\n<script language=\"javascript\">\n// List of words to show in drop down\n var functionNameArray =new Array(";
  bool isFirst=true;
  for (int i=0; i<theParser.theFunctionList.size; i++)
  { ParserFunction& currentFun=theParser.theFunctionList.TheObjects[i];
    if (currentFun.flagVisible)
    { if (!isFirst)
        std::cout << ",";
      isFirst=false;
      std::cout << "\"" << currentFun.functionName << "\"";
    }
  }
  std::cout << ");</script>";
  std::cout << "<script language=\"javascript\"> var obj = actb(document.getElementById('textInputID'), functionNameArray);</script>";
  std::cout << "\n</td>";
  std::cout
  << "<td>"
  << "<img src=\"" << theParser.DisplayPathServerBase << "/UMassBoston.png\" width=\"70\" height=\"81\"></img>"
  << "&nbsp"
  << "<img src=\"" << theParser.DisplayPathServerBase << "/karlin.png\" width=\"275\" height=\"48\"></img>"
  << "&nbsp"
  << "<img src=\"" << theParser.DisplayPathServerBase << "/jacobs_logo.png\" width=\"128\" height=\"44\"></img>";
 // std::cout << "<br>\n\nThe page <a href=\"http://www.google.com/search?btnG=1&pws=0&q=why+is+internet+explorer+bad\">does not support Internet Explorer </a>. To view this page properly you might need to install another browser.";
  std::cout << "<hr>Execution time before initiating system commands: " << TimeTotalElapsed << " seconds.";
  std::cout << "</td>";
  std::cout << "</tr>";
  std::cout << "</table>";
  std::cout << "</body></html>";
  std::string command1, command2;
  std::cout << "<!--";
  std::cout.flush();
  for(int i=0; i<theParser.SystemCommands.size; i++)
  { theGlobalVariables.ClearIndicatorVars();
    std::stringstream tempStream;
    tempStream << "Executing command " << i+1 << " out of " << theParser.SystemCommands.size;
    theGlobalVariables.theIndicatorVariables.StatusString1="Executing LaTeX/system commands.";
    theGlobalVariables.theIndicatorVariables.ProgressReportStrings[0]= tempStream.str();
    theGlobalVariables.theIndicatorVariables.ProgressReportStrings[1]= "Current command: " + theParser.SystemCommands.TheObjects[i];
    theGlobalVariables.MakeReport();
    system(theParser.SystemCommands.TheObjects[i].c_str());
  }
  std::cout << "-->";
  std::cout << theParser.afterSystemCommands;
	return 0;   // To avoid Apache errors.
}



