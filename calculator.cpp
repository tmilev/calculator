#include <iostream>
#include "polyhedra.h"
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

double GetElapsedTimeInSeconds()
{ gettimeofday(&LastMeasureOfCurrentTime, NULL);
  int miliSeconds =(LastMeasureOfCurrentTime.tv_sec- ComputationStartGlobal.tv_sec)*1000+(LastMeasureOfCurrentTime.tv_usec- ComputationStartGlobal.tv_usec)/1000;
  return ((double) miliSeconds)/1000;
}

void* RunTimer(void* ptr)
{ for (; GetElapsedTimeInSeconds()<theGlobalVariables.MaxAllowedComputationTimeInSeconds || theGlobalVariables.MaxAllowedComputationTimeInSeconds<=0;)
  { usleep(10000);
    if (ComputationComplete)
      break;
  }
  if (!ComputationComplete)
  { std::cout << "</div><br><br><br>Your computation has taken " << GetElapsedTimeInSeconds() << " seconds so far.";
    std::cout << "<br>The maximum allowed computation time is <b>" << theGlobalVariables.MaxAllowedComputationTimeInSeconds << " seconds</b>. Please use an offline version of the calculator. ";
    std::exit(0);
  } else
    pthread_exit(NULL);
}
#endif

Parser theParser;
void makeReport(IndicatorWindowVariables& input)
{ static int counter =-1;
  counter++;
  if (counter%10!=0)
    return;
  std::fstream theFile;
  std::string reportFileName=theParser.outputFolderPath;
  reportFileName.append("report.txt");
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(theFile, reportFileName, false, true, false);
  std::stringstream outStream;
  theFile << " Elapsed seconds: " << GetElapsedTimeInSeconds();
  theFile << input.ProgressReportString1 << "<br>" << input.ProgressReportString2;
}

int main(int argc, char **argv)
{ std::string inputString, inputPath;
  std::string tempS;
	std::cin >> inputString;
#ifndef WIN32
  gettimeofday(&ComputationStartGlobal, NULL);
#endif
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
	}
	getPath(argv[0], inputPath);
//	inputString="textInput=+asf&buttonGo=Go";
//  inputString="weylLetterInput=B&weyRankInput=3&textInput=%2B&buttonGo=Go";

	std::cout << "Content-Type: text/html\n\n";
  std::cout << "<html><meta name=\"keywords\" content= \"root system, root system Lie algebra, Vector partition function calculator, vector partition functions, Semisimple Lie algebras, root subalgebras, sl(2)-triples\"> <head> <title>Vector partition calculator updated " << __DATE__ << "</title>";
  //below follows a script for collapsing and expanding menus
  std::cout << "<script src=\"/easy/load.js\"></script> ";
  std::cout << "\n</head>\n<body>\n";
//  std::stringstream tempStreamX;
//  static_html3(tempStreamX);
//  static_html5(tempStreamX);
//  std::cout << tempStreamX.str() << std::endl;
//  std::cout << inputString;
#ifndef WIN32
  pthread_t TimerThread;
  pthread_create(&TimerThread, NULL,*RunTimer, 0);
#endif
  CGIspecificRoutines::functionCGIServerIgnoreUserAbort=&ignoreUserAbortSignal;
  List<std::string> inputStrings;
  CGIspecificRoutines::ChopCGIInputStringToMultipleStrings(inputString, inputStrings);
  inputStrings.SetSize(3);
  std::string& civilizedInput= inputStrings.TheObjects[2];
  std::string& inputRankString = inputStrings.TheObjects[1];
  std::string& inputWeylString = inputStrings.TheObjects[0];
  theGlobalVariables.MaxAllowedComputationTimeInSeconds=10;
  CGIspecificRoutines::CivilizedStringTranslationFromCGI(civilizedInput, civilizedInput);
  theGlobalVariables.SetFeedDataToIndicatorWindowDefault(&makeReport);
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
  CGIspecificRoutines::MakeSureWeylGroupIsSane(theParser.DefaultWeylLetter, theParser.DefaultWeylRank);
  //For debugging:
  ParallelComputing::cgiLimitRAMuseNumPointersInList=60000000;
  HashedList<Monomial<Rational> >::PreferredHashSize=100;
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
  theParser.outputFolderPath.append(inputPath);
  theParser.outputFolderPath.append("../htdocs/tmp/");
  theParser.outputFolderDisplayPath="/tmp/";
  std::string indicatorFileString;
  indicatorFileString=theParser.outputFolderPath;
  indicatorFileString.append("indicator.html");
  if (!CGIspecificRoutines::FileExists(indicatorFileString))
  { std::stringstream tempStreamX;
    static_html3(tempStreamX);
    std::fstream tempFile;
    CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(tempFile, indicatorFileString, false, true, false);
    tempFile << tempStreamX.str();
  }
  if (theParser.DefaultWeylLetter=='B' && theParser.DefaultWeylRank==3)
  { theParser.theHmm.MakeG2InB3(theParser, theGlobalVariables);
    SSalgebraModule theModule;
    std::stringstream out;
    theModule.InduceFromEmbedding(out, theParser.theHmm, theGlobalVariables);
    List<ElementSimpleLieAlgebra> theBasis;
    theBasis.SetSize(theParser.theHmm.theRange.GetNumGenerators());
    /*int domainRank=theParser.theHmm.theDomain.GetRank();
    int rangeRank=theParser.theHmm.theRange.GetRank();
    int numDomainPosRoots=theParser.theHmm.theDomain.GetNumPosRoots();
    int numRangePosRoots=theParser.theHmm.theRange.GetNumPosRoots();*/
    for (int i=0; i<theParser.theHmm.imagesAllChevalleyGenerators.size; i++)
    { int theIndex=i;
      if (i>=6 && i<8)
        theIndex=3+i;
      if (i>=8)
        theIndex=i+7;
      /*
      if (i==0) theIndex=3;
      if (i==1) theIndex=4;
      if (i==2) theIndex=5;
      if (i==3) theIndex=6;
      if (i==4) theIndex=7;
      if (i==5) theIndex=8;
      if (i==6) theIndex=9;
      if (i==7) theIndex=10;
      if (i==8) theIndex=12;
      if (i==9) theIndex=13;
      if (i==10) theIndex=14;
      if (i==11) theIndex=15;
      if (i==12) theIndex=16;
      if (i==13) theIndex=17;
      */
      ElementSimpleLieAlgebra& currentElt=theBasis.TheObjects[theIndex];
      currentElt=theParser.theHmm.imagesAllChevalleyGenerators.TheObjects[i];
    }
    for (int i=0; i<theModule.moduleElementsEmbedded.size; i++)
    { int theIndex=i+6;
      if (i>=3)
        theIndex=8+i;
      /*
      if (i==0) theIndex=0;
      if (i==1) theIndex=1;
      if (i==2) theIndex=2;
      if (i==3) theIndex=11;
      if (i==4) theIndex=18;
      if (i==5) theIndex=19;
      if (i==6) theIndex=20;
      */
      ElementSimpleLieAlgebra& currentElt=theBasis.TheObjects[theIndex];
      currentElt=theModule.moduleElementsEmbedded.TheObjects[i];
    }
    for (int i=0; i<theBasis.size; i++)
    { int displayIndex=i-9;
      if (displayIndex>=0)
      { if (displayIndex<3)
          displayIndex+=10;
        else
          displayIndex-=2;
      }
      //std::cout << "<br>f_{" << displayIndex << "}=" << theBasis.TheObjects[i].ElementToStringNegativeRootSpacesFirst(false, false, theParser.theHmm.theRange);
    }
    theParser.testAlgebra.init(theBasis, theParser.theHmm.theRange, theGlobalVariables);
  } else
    theParser.testAlgebra.initDefaultOrder(theParser.theHmm.theRange, theGlobalVariables);
  double TimeParsing=0, TimeEvaluation=0;
  //std::cout << "before parsing numvars is: " << theParser.NumVariables;
  CGIspecificRoutines::GetHtmlStringSafeishReturnFalseIfIdentical(civilizedInput, tempS);
  civilizedInput=tempS;
/*  std::cout << "<div id=\"divProgressReport\"> <script type=\"text/javascript\"> window.setTimeout("Click()", 100);</script> </div>";

  <script  document.getElementById(\"textDim\").value=" << theParser.DefaultWeylRank << "; \n";
  std::cout << "document.getElementById(\"textType\").value=\"" << theParser.DefaultWeylLetter << "\"; </script>";
  */

  theParser.ParseEvaluateAndSimplifyPart1(civilizedInput, theGlobalVariables);
  TimeParsing=GetElapsedTimeInSeconds();
  std::string theResult = theParser.ParseEvaluateAndSimplifyPart2(civilizedInput, theGlobalVariables);
  TimeEvaluation=GetElapsedTimeInSeconds()-TimeParsing;
  theParser.DefaultWeylLetter=theParser.theHmm.theRange.theWeyl.WeylLetter;
  theParser.DefaultWeylRank=theParser.theHmm.theRange.theWeyl.CartanSymmetric.NumRows;
  //std::cout << "Computation done in " << GetElapsedTimeInSeconds() << "seconds <br>";
  std::cout.flush();
  theParser.ComputeDebugString(false, theGlobalVariables);

  std::string beginMath="<div class=\"math\" scale=\"50\">";
  std::string endMath ="</div>";
  std::cout << "<table>\n <tr valign=\"top\">\n <td>";
  std::cout << "\n<FORM method=\"POST\" name=\"formCalculator\" action=\"/cgi-bin/calculator\">\n" ;
  std::stringstream tempStream4;
  static_html4(tempStream4);
  std:: cout << tempStream4.str();
//  std::cout << "<input type=\"text\" size =\"1\" name=\"weylLetterInput\" value=\"" << theParser.DefaultWeylLetter << "\"></input>";
//  std::cout << "<input type=\"text\" size =\"1\" name=\"weylRankInput\" value=\"" << theParser.DefaultWeylRank << "\"></input>";
  std::cout << "\n<br>\n";
  std::string civilizedInputSafish;
  if (CGIspecificRoutines::GetHtmlStringSafeishReturnFalseIfIdentical(civilizedInput, civilizedInputSafish))
    std::cout << "Your input has been treated normally, however the return string of your input has been modified. More precisely, &lt; and &gt;  are modified due to a javascript hijack issue. ";
  std::cout << "<textarea rows=\"3\" cols=\"30\" name=\"textInput\" id=\"textInputID\" onkeypress=\"if (event.keyCode == 13) {this.form.submit(); return false;}\">";
  std::cout << civilizedInputSafish;
  std::cout << "</textarea>\n<br>\n";
  std::cout << "<input type=\"submit\" name=\"buttonGo\" value=\"Go\"	> ";
//  std::cout << "<a href=\"/tmp/indicator.html\" target=\"_blank\"> Indicator window  </a>";
  std::cout << "\n</FORM>";
  std::cout << "<script type=\"text/javascript\"> document.getElementById(\"textDim\").value=" << theParser.DefaultWeylRank << "; \n";
  std::cout << "document.getElementById(\"textType\").value=\"" << theParser.DefaultWeylLetter << "\"; </script>";
//  \n<FORM method=\"POST\" name=\"formCalculator\" action=\"/cgi-bin/calculator\">\n <input type=\"textarea\" rows=\"60\" cols=\"60\" name=\"textInput\" \"></textarea>\n<br>\n";
#ifndef WIN32
  double TimeTotalElapsed=GetElapsedTimeInSeconds();
  if (civilizedInput!="")
  { std::cout << "<hr><b>Result.</b> " << theResult << "<br><hr><br>Parsing+evaluation time: " << TimeTotalElapsed << " seconds<br> (" << TimeParsing << " parsing + " << TimeEvaluation <<  " evaluation + " << TimeTotalElapsed-TimeEvaluation-TimeParsing << " processing)<br>";
    std::cout << "#  pointers at peak RAM: " << ParallelComputing::PointerCounterPeakRamUse << "<br>(excluding std::string, std::stringstream)<br>";
  }
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
  std::cout << "<hr><b>Notes.</b><br> Computation is limited to " << theGlobalVariables.MaxAllowedComputationTimeInSeconds << " seconds. <br> Clicking \"Go\" + blank screen = calculator bug. <br> Clicking \"Go\" + \"Internal server error\"=  serious calculator bug.<br> Clicking \"Go\" + wrong result= <b>very serious calculator bug</b>.";
  std::cout << "<br>Bug reports = my wholehearted gratitude.<br><button " << CGIspecificRoutines::GetStyleButtonLikeHtml() << " onclick=\"switchMenu('sourceDetails');\" >C++ source of the calculator</button>";
  std::cout << "<button " << CGIspecificRoutines::GetStyleButtonLikeHtml() << " onclick=\"switchMenu('debugDetails');\">Debugging info</button>";
  std::cout << "<div id=\"sourceDetails\" style=\"display: none\">";
	std::cout << " <br>\n";
  std::cout << " <a href=\"http://vectorpartition.svn.sourceforge.net/viewvc/vectorpartition/trunk/polyhedra.h?view=markup\"> Vector partition c++(1 out of 3 files (header file))</a>\n";
  std::cout << " <br>\n";
  std::cout << " <a href=\"http://vectorpartition.svn.sourceforge.net/viewvc/vectorpartition/trunk/polyhedra.cpp?view=markup\"> Vector partition c++(2 out of 3 files (.cpp file that has passed minimal testing))</a>\n";
  std::cout << " <br>\n";
  std::cout << " <a href=\"http://vectorpartition.svn.sourceforge.net/viewvc/vectorpartition/trunk/custom_code_here.cpp?view=markup\"> Vector partition c++(3 out of 3 files (.cpp file for current development))</a>\n";
  std::cout << " <br>\n";
  std::cout << " <a href=\"http://vectorpartition.svn.sourceforge.net/viewvc/vectorpartition/trunk/calculator.cpp?view=markup\"> Calculator interface c++ (1 out of 2 files)</a>\n";
  std::cout << " <br>\n";
  std::cout << " <a href=\"http://vectorpartition.svn.sourceforge.net/viewvc/vectorpartition/trunk/RootSystem.html.cpp?view=markup\"> Calculator interface c++ (2 out of 2 files)</a>\n";
  std::cout << " <br>\n";
  std::cout << " The calculator is a simple console application (like the C++ \"Hello world!\"). It is managed by an <a href=\"http://httpd.apache.org/\">Apache web server</a>. ";
  std::cout << " <br>The calculator errors get caught either by 1) in-line asserts() left by me (blank screen), or 2) by Apache/the system (internal server error)."
  << "  \n<br> All precomputed data is stored in a \"database\" <a href=\"/tmp/\">here</a>. <br> The file input/output is done via std::fstream. <br>The LaTeX'ing is called using std::system() calls. The LaTeX logs can be found by viewing the calculator page's source. <br> The html output is hardcoded: either by hand or transformed from a separate .html file using a micro-tool written for the purpose. ";
  std::cout << " ";
  std::cout << " \n";
  std::cout << "<hr><b>Installing the calculator on your machine from c++ source. </b><br> In order to get the calculator running on your machine you need to do the following. I will work on simplifying the installation some time soon. <br>0) You need a Linux machine. I have tested it only on Ubuntu. <br>1) Download the c++ files in the links above. <br>2) Put them in a c++ project and make sure the following includes work:"
  << " #include &lt;sys/time.h&gt; #include &lt;unistd.h&gt; #include &lt;pthread.h&gt;. They should work by default on almost any Linux distro. <br>3) Build the project to a console application with default console application settings.  <br> 4) Install an <a href=\"http://httpd.apache.org/\">Apache web server</a> and enable cgi scripts (you might have to google how to, it's not easy).  Apache comes preinstalled on Ubuntu.\n"
  << "<br>5) Assume the location of your server's cgi-bin folder is some_folder_path/cgi-bin/ (you can google where this folder is located). Put the calculator executable file in some_folder_path/cgi-bin/  .  <br> 6) Create folders some_folder_path/cgi-bin/../htdocs, some_folder_path/cgi-bin/../htdocs/tmp/. "
  << "Enable read/write access to those two folders for every user. <br>7) The basic installation is now complete; test the calculator by running it through your web browser. The remaining steps are needed in order to get a nicely formatted output and to avoid broken links in the calculator. "
  << "<br>8) The calculator calls latex, so you need to install latex. <br>9) You need install the jsmath script in the base (root) folder of your apache server.";
	std::cout <<	"</div>";
  std::cout << "<div id=\"debugDetails\" style=\"display: none\">";
  std::cout << "<br>Debugging printouts follow.<br>Number of pointers used:" << ParallelComputing::GlobalPointerCounter << "<br>raw input string: " << inputString;
  std::cout << "<br>civilized input strings: " << civilizedInputSafish << "<br> chopped strings: <br>";
  for (int i=0; i<inputStrings.size; i++)
    std::cout << "string " << i << ": " << inputStrings.TheObjects[i] << "<br>";
  std::cout << "system commands: <br>";
  theGlobalVariables.MaxAllowedComputationTimeInSeconds=10000;
  for (int i=0; i< theParser.SystemCommands.size; i++)
    std::cout <<  theParser.SystemCommands.TheObjects[i] << "<br>";
  std::cout << "<br>Parser debug string:<br> " << theParser.DebugString;
//  theParser.theLieAlgebra.ComputeDebugString();
//  std::cout << "<br>details:<br> " << theParser.theLieAlgebra.ElementToStringLieBracketPairing();
  std::cout << "</div>";

  std::stringstream tempStream;
  inputPath.append("../htdocs/tmp/");
  std::stringstream tempStream2;
  tempStream2 << theParser.DefaultWeylLetter << theParser.DefaultWeylRank << "/";
  inputPath.append(tempStream2.str());
  tempStream << theParser.DefaultWeylLetter << theParser.DefaultWeylRank << "table";
  std::string fileNameLieBracketNoEnding=tempStream.str();
  std::string fileNameLieBracketFullPathNoEnding=inputPath;
  fileNameLieBracketFullPathNoEnding.append(fileNameLieBracketNoEnding);
  std::cout << " <hr><b>Chevalley-Weyl basis.</b><br> Simple Lie algebra of type " <<  theParser.DefaultWeylLetter << theParser.DefaultWeylRank << ". <a href=\"/tmp/"<< theParser.DefaultWeylLetter << theParser.DefaultWeylRank << "/" << fileNameLieBracketNoEnding << ".tex\">Latex source</a>. <br>\n<img src=\"/tmp/" << tempStream2.str() << fileNameLieBracketNoEnding << ".png\"></img>";
  std::string latexCommandTemp;
  std::string fileNameLieBracketFullPathPNGEnding;
  fileNameLieBracketFullPathPNGEnding=fileNameLieBracketFullPathNoEnding;
  fileNameLieBracketFullPathPNGEnding.append(".png");
  if (!CGIspecificRoutines::FileExists(fileNameLieBracketFullPathPNGEnding))
  { std::cout << "<br>the file: " << fileNameLieBracketFullPathPNGEnding << " does not exist<br>";
    std::fstream lieBracketFile;
    std::string tempFileName= fileNameLieBracketFullPathNoEnding;
    tempFileName.append(".tex");
    std::stringstream tempCommand;
    tempCommand << "mkdir " << inputPath;
    tempS=tempCommand.str();
    system(tempS.c_str());
    CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(lieBracketFile, tempFileName, false, true, false);
    theParser.theHmm.theRange.ElementToStringNegativeRootSpacesFirst(tempS, false, true, true, theGlobalVariables);
    lieBracketFile << "\\documentclass{article}\\begin{document}\\pagestyle{empty}\n" << tempS << "\n\\end{document}";
    lieBracketFile.close();
    std::stringstream tempStreamLatex, tempStreamPNG;
    tempStreamLatex << "latex  -output-directory=" << inputPath << " " << inputPath << fileNameLieBracketNoEnding << ".tex";
    latexCommandTemp=tempStreamLatex.str();
    theParser.SystemCommands.AddObjectOnTop(latexCommandTemp);
    tempStreamPNG << "dvipng " << fileNameLieBracketFullPathNoEnding << ".dvi -o " << fileNameLieBracketFullPathNoEnding << ".png -T tight";
    latexCommandTemp= tempStreamPNG.str();
    theParser.SystemCommands.AddObjectOnTop(latexCommandTemp);
  }
  //  std::cout << "<button onclick=\"switchMenu('rootSystem');\" >Root system</button>";
//  std::cout << "<div id=\"rootSystem\" style=\"display: none\">";
  std::cout << "<hr><b> Root system visualization</b><br> The basis vectors (small red cirles) can be dragged with the mouse pointer. <br> The <a href=\"/tmp/RootSystem_no_autocomplete.html\">visualization code</a>"
                  << " uses javascript (+ dojo script from google for drawing lines and cirles).<br>";
  std::stringstream tempStream3;
  static_html5(tempStream3);
  static_html6(tempStream3);
  std::cout << tempStream3.str();
//  std::cout << "</div>";

  std::cout << "\n\n<script language=\"javascript\">\n// List of words to show in drop down\n var functionNameArray =new Array(";
  for (int i=0; i<theParser.theFunctionList.size; i++)
  { ParserFunction& currentFun=theParser.theFunctionList.TheObjects[i];
    std::cout << "\"" << currentFun.functionName << "\"";
    if (i!=theParser.theFunctionList.size-1)
      std::cout << ",";
  }
  std::cout << ");</script>";
  std::cout << "<script language=\"javascript\"> var obj = actb(document.getElementById('textInputID'), functionNameArray);</script>";
  std::cout << "\n</td>";
  std::cout << "<td> <img src=\"../karlin.png\" width=\"275\" height=\"48\"></img>&nbsp<img src=\"../jacobs_logo.png\" width=\"128\" height=\"44\"></img>";
  std::cout << "<br>\n\nThe page <a href=\"http://www.google.com/search?btnG=1&pws=0&q=why+is+internet+explorer+bad\">does not support Internet Explorer </a>. To view this page properly you might need to install another browser.</td>";
  std::cout << "</tr></table>";
  std::cout << "</body></html>";
  std::string command1, command2;
  std::cout << "<!--";
  std::cout.flush();
  for(int i=0; i<theParser.SystemCommands.size; i++)
  { std::cout << "\n\ncommand: " << theParser.SystemCommands.TheObjects[i].c_str() << "\n" ;
    system(theParser.SystemCommands.TheObjects[i].c_str());
  }
  std::cout << "-->";
	return 0;   // To avoid Apache errors.
}



