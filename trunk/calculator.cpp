#include <iostream>
#include "polyhedra.h"
#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
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
  for (int i=length-1;i>=0;i--)
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

extern void static_html4(std::stringstream& output);
extern void static_html3(std::stringstream& output);

const double MaxAllowedComputationTimeInSeconds=20;
bool ComputationComplete;

#ifndef WIN32
timeval ComputationStartGlobal, LastMeasureOfCurrentTime;

double GetElapsedTimeInSeconds()
{ gettimeofday(&LastMeasureOfCurrentTime, NULL);
  int miliSeconds =(LastMeasureOfCurrentTime.tv_sec- ComputationStartGlobal.tv_sec)*1000+(LastMeasureOfCurrentTime.tv_usec- ComputationStartGlobal.tv_usec)/1000;
  return ((double) miliSeconds)/1000;
}

void* RunTimer(void* ptr)
{ for (; GetElapsedTimeInSeconds()<MaxAllowedComputationTimeInSeconds || MaxAllowedComputationTimeInSeconds<=0;)
  { usleep(10000);
    if (ComputationComplete)
      break;
  }
  if (!ComputationComplete)
  { std::cout << "</div><br><br><br>Your computation has taken " << GetElapsedTimeInSeconds() << " seconds so far.";
    std::cout << "<br>The maximum allowed computation time is <b>" << MaxAllowedComputationTimeInSeconds << " seconds</b>. Please use the offline version of the calculator. ";
    std::exit(0);
  } else
    pthread_exit(NULL);
}
#endif

int main(int argc, char **argv)
{ std::string inputString, inputPath, tempS;
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
  std::cout << "<html><head><title>Vector partition calculator updated " << __DATE__ << "</title>";
  //below follows a script for collapsing and expanding menus
  std::cout << "<script src=\"/easy/load.js\"></script> ";
  std::cout << "\n</head>\n<body>\n";
//  std::cout << inputString;
#ifndef WIN32
  pthread_t TimerThread;
  pthread_create(&TimerThread, NULL,*RunTimer, 0);
#endif

  List<std::string> inputStrings;
  CGIspecificRoutines::ChopCGIInputStringToMultipleStrings(inputString, inputStrings);
  inputStrings.SetSize(3);
  std::string& civilizedInput= inputStrings.TheObjects[2];
  std::string& inputRankString = inputStrings.TheObjects[1];
  std::string& inputWeylString = inputStrings.TheObjects[0];
  CGIspecificRoutines::CivilizedStringTranslationFromCGI(civilizedInput, civilizedInput);
  Parser theParser;
  GlobalVariables theGlobalVariables;
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
  HashedList<Monomial<Rational> >::PreferredHashSize=10;
//  civilizedInput="GetRelations(x_1^2, x_1x_2, x_2^2)";
  //civilizedInput="vpf((1,-3), (1,-1), (1,1), (1,3))";
  //civilizedInput="split(PartialFraction((1,0), (1,2), (1,-2)))";
//  civilizedInput="PartialFraction(1,1,(0,1))";
  //civilizedInput="slTwoInSlN(3)";
  //theParser.DefaultWeylLetter='A';
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
  //civilizedInput="GetAllRepresentatitves(Lattice((1,1),(1,0)),Lattice((7,4),(1,3)))";
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
  theParser.ParseEvaluateAndSimplifyPart1(civilizedInput, theGlobalVariables);
  TimeParsing=GetElapsedTimeInSeconds();
  std::string theResult = theParser.ParseEvaluateAndSimplifyPart2(civilizedInput, theGlobalVariables);
  TimeEvaluation=GetElapsedTimeInSeconds()-TimeParsing;
  theParser.DefaultWeylLetter=theParser.theHmm.theRange.theWeyl.WeylLetter;
  theParser.DefaultWeylRank=theParser.theHmm.theRange.theWeyl.CartanSymmetric.NumRows;
  //std::cout << "Computation done in " << GetElapsedTimeInSeconds() << "seconds <br>";
  std::cout.flush();
  theParser.ComputeDebugString(theGlobalVariables);

  std::string beginMath="<div class=\"math\" scale=\"50\">";
  std::string endMath ="</div>";
  std::cout << "<table>\n <tr valign=\"top\">\n <td>";
  std::cout << "\n<FORM method=\"POST\" name=\"formCalculator\" action=\"/cgi-bin/calculator\">\n" ;
  std::stringstream tempStream4;
  static_html3(tempStream4);
  std:: cout << tempStream4.str();
//  std::cout << "<input type=\"text\" size =\"1\" name=\"weylLetterInput\" value=\"" << theParser.DefaultWeylLetter << "\"></input>";
//  std::cout << "<input type=\"text\" size =\"1\" name=\"weylRankInput\" value=\"" << theParser.DefaultWeylRank << "\"></input>";
  std::cout << "\n<br>\n";
  std::cout << "<textarea rows=\"3\" cols=\"30\" name=\"textInput\" id=\"textInputID\" onkeypress=\"if (event.keyCode == 13) {this.form.submit(); return false;}\">" << civilizedInput  << "</textarea>\n<br>\n";
  std::cout << "<input type=\"submit\" name=\"buttonGo\" value=\"Go\"	> ";
  std::cout << "\n</FORM>";
  std::cout << "<script type=\"text/javascript\"> document.getElementById(\"textDim\").value=" << theParser.DefaultWeylRank << "; \n";
  std::cout << "document.getElementById(\"textType\").value=\"" << theParser.DefaultWeylLetter << "\"; </script>";
//  \n<FORM method=\"POST\" name=\"formCalculator\" action=\"/cgi-bin/calculator\">\n <input type=\"textarea\" rows=\"60\" cols=\"60\" name=\"textInput\" \"></textarea>\n<br>\n";
#ifndef WIN32
  double TimeTotalElapsed=GetElapsedTimeInSeconds();
  if (civilizedInput!="")
  { std::cout << "<br>result: " << theResult << "<br>Parsing+evaluation time: " <<  TimeTotalElapsed<< " seconds<br> (" << TimeParsing << " parsing + " << TimeEvaluation <<  " evaluation + " << TimeTotalElapsed-TimeEvaluation-TimeParsing << " processing)<br>";
    std::cout << "#  pointers at peak RAM: " << ParallelComputing::PointerCounterPeakRamUse << "<br>(excluding std::string, std::stringstream)<br>";
  }
  if (GetElapsedTimeInSeconds()>1)
    std::cout << "If your computation takes too long it is probably due to the weak algorithms used for your computation.<br> Feel free to email the author(s) with requests for speed improvement.";
  ComputationComplete=true;
#endif
  std::cout.flush();
  std::cout << "</td>";
  std::cout << " <td width=\"300\">" << theParser.GetFunctionDescription() << "</td>\n";
  std::cout << "<td>";
  std::cout << " <img src=\"../karlin.png\" width=\"275\" height=\"48\"></img>&nbsp<img src=\"../jacobs_logo.png\" width=\"128\" height=\"44\"></img><br>";
  std::cout << "<button onclick=\"switchMenu('idLinksText');\">Show/hide links</button> <div id=\"idLinksText\" style=\"display: none\">"
                  << "<a href=\"http://wwwmathlabo.univ-poitiers.fr/~maavl/LiE/form.html\"> LiE software online </a>"
                  << "<br><br><a href=\"http://www.google.com/search?btnG=1&pws=0&q=why+is+internet+explorer+bad\"> IE not suppoted.</a> "
                 // << " <br> <a href=\"http://www.cs.kuleuven.be/cgi-bin/dtai/barvinok.cgi\"> Barvinok program online</a>"
                  << "</div>";
  std::cout << "<br>";
  std::stringstream tempStream;
  inputPath.append("../htdocs/tmp/");
  std::stringstream tempStream2;
  tempStream2 << theParser.DefaultWeylLetter << theParser.DefaultWeylRank << "/";
  inputPath.append(tempStream2.str());
  tempStream << theParser.DefaultWeylLetter << theParser.DefaultWeylRank << "table";
  std::string fileNameLieBracketNoEnding=tempStream.str();
  std::string fileNameLieBracketFullPathNoEnding=inputPath;
  fileNameLieBracketFullPathNoEnding.append(fileNameLieBracketNoEnding);
  std::cout << "<img src=\"/tmp/" << tempStream2.str() << fileNameLieBracketNoEnding << ".png\"></img>";
  List<std::string> LatexCommands;
  std::string latexCommandTemp;
  std::string fileNameLieBracketFullPathPNGEnding;
  fileNameLieBracketFullPathPNGEnding=fileNameLieBracketFullPathNoEnding;
  fileNameLieBracketFullPathPNGEnding.append(".png");
  LatexCommands.size=0;
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
    LatexCommands.AddObjectOnTop(latexCommandTemp);
    tempStreamPNG << "dvipng " << fileNameLieBracketFullPathNoEnding << ".dvi -o " << fileNameLieBracketFullPathNoEnding << ".png -T tight";
    latexCommandTemp= tempStreamPNG.str();
    LatexCommands.AddObjectOnTop(latexCommandTemp);
  }
  std::cout << "<br>";
  std::cout << "<button onclick=\"switchMenu('rootSystem');\" >Root system</button>";
  std::cout << "<button onclick=\"switchMenu('sourceDetails');\" >C++ source</button>";
  std::cout << "<button onclick=\"switchMenu('debugDetails');\">Debugging</button>";
  std::cout << "<div id=\"rootSystem\" style=\"display: none\">";
  std::stringstream tempStream3;
  static_html4(tempStream3);
  std::cout << tempStream3.str();
  std::cout << "</div>";

  std::cout << "<script language=\"javascript\">\n// List of words to show in drop down\n var functionNameArray =new Array(";
  for (int i=0; i<theParser.theFunctionList.size; i++)
  { ParserFunction& currentFun=theParser.theFunctionList.TheObjects[i];
    std::cout << "\"" << currentFun.functionName << "\"";
    if (i!=theParser.theFunctionList.size-1)
      std::cout << ",";
  }
  std::cout << ");</script>";
  std::cout << "<script language=\"javascript\"> var obj = actb(document.getElementById('textInputID'), functionNameArray);</script>";


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
  std::cout << " To run the calculator on your own web server you might want to install the <a href=\"http://httpd.apache.org/\">Apache web server</a> (comes preinstalled on <a href=\"http://www.ubuntu.com/\">Ubuntu</a>)";
  std::cout << " <br>\n";
	std::cout <<	"</div>";
  std::cout << "<div id=\"debugDetails\" style=\"display: none\">";
  std::cout << "<br>Debugging printouts follow.<br>Number of pointers used:" << ParallelComputing::GlobalPointerCounter << "<br>raw input string: " << inputString;
  std::cout << "<br>civilized input strings: " << civilizedInput << "<br> chopped strings: <br>";
  for (int i=0; i<inputStrings.size; i++)
    std::cout << "string " << i << ": " << inputStrings.TheObjects[i] << "<br>";
  std::cout << "system commands: <br>";
  for (int i=0; i<LatexCommands.size; i++)
    std::cout << LatexCommands.TheObjects[i] << "<br>";
  std::cout << "<br>Parser debug string:<br> " << theParser.DebugString;
//  theParser.theLieAlgebra.ComputeDebugString();
//  std::cout << "<br>details:<br> " << theParser.theLieAlgebra.ElementToStringLieBracketPairing();
  std::cout << "</div>";
  std::cout << "\n</td></tr></table>";
  std::cout << "</body></html>";
  std::string command1, command2;
  std::cout << "<!--";
  std::cout.flush();
  for (int i=0; i<LatexCommands.size; i++)
    system(LatexCommands.TheObjects[i].c_str());
  for(int i=0; i<theParser.SystemCommands.size; i++)
  { std::cout << "\n\ncommand: " << theParser.SystemCommands.TheObjects[i].c_str() << "\n" ;
    system(theParser.SystemCommands.TheObjects[i].c_str());
  }
  std::cout << "-->";
	return 0;   // Toavoid Apache errors.
}

void CGIspecificRoutines::CivilizedStringTranslationFromCGI(std::string& input, std::string& output)
{ std::string readAhead;
  std::stringstream out;
  int inputSize=(signed) input.size();
  for (int i=0; i<inputSize; i++)
  { readAhead="";
    for (int j=0; j<6; j++)
    { if (i+j<inputSize)
        readAhead.push_back(input[i+j]);
      if (CGIspecificRoutines::AttemptToCivilize(readAhead, out))
      { i+=j;
        break;
      }
    }
  }
  output=out.str();
}

void CGIspecificRoutines::ChopCGIInputStringToMultipleStrings(const std::string& input, List<std::string>& output)
{ int inputLength= (signed) input.size();
  bool reading=false;
  output.SetSize(1);
  for (int i=0; i<inputLength; i++)
  { if (input[i]=='=')
      reading=!reading;
    if (input[i]=='&')
    { output.SetSize(output.size+1);
      output.LastObject()->reserve(1000);
      *output.LastObject()="";
      reading=false;
    }
    if (input[i]!='=' && input[i]!='&' && reading)
      output.LastObject()->push_back(input[i]);
  }
}

bool CGIspecificRoutines::AttemptToCivilize(std::string& readAhead, std::stringstream& out)
{ if (readAhead[0]!='%' && readAhead[0]!='&' && readAhead[0]!='+')
  { out << readAhead[0];
    return true;
  }
  if (readAhead=="&")
  { out << " ";
    return true;
  }
  if (readAhead=="+")
  { out << " ";
    return true;
  }
  if (readAhead=="%2B")
  { out << "+";
    return true;
  }
  if (readAhead=="%28")
  { out << "(";
    return true;
  }
  if (readAhead=="%29")
  { out << ")";
    return true;
  }
  if (readAhead=="%5B")
  { out << "[";
    return true;
  }
  if (readAhead=="%5D")
  { out << "]";
    return true;
  }
  if (readAhead=="%2C")
  { out << ",";
    return true;
  }
  if (readAhead=="%7B")
  { out << "{";
    return true;
  }
  if (readAhead=="%3B")
  { out << ";";
    return true;
  }
  if (readAhead=="%2F")
  { out << "/";
    return true;
  }
  if (readAhead=="%3A")
  { out << ":";
    return true;
  }
  if (readAhead=="%5E")
  { out << "^";
    return true;
  }
  if (readAhead=="%5C")
  { out << "\\";
    return true;
  }
  if (readAhead=="%7D")
  { out << "}";
    return true;
  }
  if (readAhead=="%0D%0A")
  { return true;
  }
  return false;
}


