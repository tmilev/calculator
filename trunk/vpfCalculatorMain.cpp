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

void PredefinedStrings(std::string& inputOutputCivilizedString)
{ if (inputOutputCivilizedString=="PenkovProject")
    inputOutputCivilizedString="experimentalPrintSemisimpleSubalgebras{}(F_4)";
  if(inputOutputCivilizedString=="PenkovProjectRecomputeTest")
    inputOutputCivilizedString="%HideLHS \
    experimentalPrintSemisimpleSubalgebrasForceRecompute{}Load{}Serialization{}(LoadSemisimpleSubalgebras, (F)_{4}, \
    ( \n\
Serialization{}(LoadCandidateSubalgebra, A^{12}_{1}, {Sequence{}((4, 8, 12, 6))}, (\n\
g_{-9}+g_{-17}+g_{-18}, 2 (g_{18})+2 (g_{9})+g_{17}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 2 A^{12}_{1}, ((4, 8, 12, 6), (2, 2, 0, 2)), (\n\
g_{-15}+2 g_{-16}+g_{-14}, g_{15}+2 g_{14}+g_{16}, g_{-1}- g_{-2}- g_{-4}, -2 g_{2}+2 g_{1}- g_{4}\n\
))\n\
))";
  if (inputOutputCivilizedString=="PenkovProjectRecompute")
    inputOutputCivilizedString= "\
%HideLHS \n\
experimentalPrintSemisimpleSubalgebrasForceRecompute{}Load{}(\n\
Serialization{}(LoadSemisimpleSubalgebras, F^{2}_{4}, (\n\
Serialization{}(LoadCandidateSubalgebra, A^{312}_{1}, {Sequence{}((22, 42, 60, 32))}, (\n\
g_{-1}+g_{-2}+g_{-3}+g_{-4}, 42 (g_{2})+30 (g_{3})+22 (g_{1})+16 (g_{4})\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{120}_{1}, {Sequence{}((14, 26, 36, 20))}, (\n\
g_{-1}+g_{-2}+g_{-7}+g_{-9}, 18 (g_{2})+14 (g_{1})+10 (g_{7})+8 (g_{9})\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{72}_{1}, {Sequence{}((10, 20, 28, 16))}, (\n\
g_{-2}+g_{-4}+g_{-6}+g_{-11}, 10 (g_{11})+8 (g_{4})+4 (g_{7})+4 (g_{6})+2 (g_{8})+2 (g_{2})-4 (g_{5})\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{24}_{1}, {Sequence{}((6, 12, 16, 8))}, (\n\
g_{-2}+g_{-9}+g_{-11}+g_{-18}, 4 (g_{18})+4 (g_{2})+2 (g_{11})+2 (g_{9})-2 (g_{16})-2 (g_{5})\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{56}_{1}, {Sequence{}((10, 18, 24, 12))}, (\n\
2 g_{-16}+2 g_{-1}+2 g_{-6}, 3 g_{6}+5 g_{1}+3 g_{16}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{8}_{1}, {Sequence{}((4, 6, 8, 4))}, (\n\
g_{-14}+g_{-18}, 2(g_{14}+g_{18})\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{70}_{1}, {Sequence{}((10, 19, 28, 16))}, (\n\
g_{-4}+g_{-8}+g_{-9}, 9 (g_{9})+8 (g_{4})+5 (g_{8})\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{22}_{1}, {Sequence{}((6, 11, 16, 8))}, (\n\
g_{-8}+g_{-9}+g_{-16}, 4 (g_{16})+3 (g_{8})+g_{9}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{20}_{1}, {Sequence{}((6, 10, 14, 8))}, (\n\
2 g_{-16}+g_{-8}, 3 g_{8}+2 g_{16}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{18}_{1}, {Sequence{}((5, 10, 14, 8))}, (\n\
g_{-10}+g_{-14}+g_{-15}, 2 (g_{15})+2 (g_{10})+g_{14}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{12}_{1}, {Sequence{}((4, 8, 12, 6))}, (\n\
g_{-9}+g_{-17}+g_{-18}, 2 (g_{18})+2 (g_{9})+g_{17}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{6}_{1}, {Sequence{}((3, 6, 8, 4))}, (\n\
g_{-21}+g_{-14}, g_21+g_{14}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{16}_{1}, {Sequence{}((4, 8, 12, 8))}, (\n\
g_{-7}+g_{-17}, 2 (g_{17})+2 (g_{7})\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{4}_{1}, {Sequence{}((2, 4, 6, 4))}, (\n\
g_{-21}, g_{21}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}, {Sequence{}((2, 3, 4, 2))}, (\n\
g_{-24}, g_{24}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 2 A^{2}_{1}, ((2, 3, 4, 2), (0, 1, 2, 2)), (\n\
g_{-24}, g_{24}, g_{-16}, g_{16}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+A^{4}_{1}, ((2, 3, 4, 2), (0, 2, 4, 2)), (\n\
g_{-24}, g_{24}, g_{-13}, g_{13}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 2 A^{4}_{1}, ((2, 4, 6, 4), (2, 2, 2, 0)), (\n\
g_{-21}, g_{21}, g_{-5}+g_{-11}, g_{11}+g_{5}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+A^{6}_{1}, ((2, 3, 4, 2), (0, 3, 4, 2)), (\n\
g_{-24}, g_{24}, g_{-9}+g_{-10}, g_{10}+g_{9}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{4}_{1}+A^{6}_{1}, ((2, 4, 6, 4), (2, 3, 4, 0))),\n\
Serialization{}(LoadCandidateSubalgebra, 2 A^{6}_{1}, ((3, 6, 8, 4), (1, 0, 2, 2))),\n\
Serialization{}(LoadCandidateSubalgebra, A^{4}_{1}+A^{8}_{1}, ((2, 4, 6, 4), (2, 4, 4, 0)), (\n\
g_{-21}, g_{21}, g_{-2}+g_{-11}, 2 g_{11}+2 g_{2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 2 A^{12}_{1}, ((4, 8, 12, 6), (2, 2, 0, 2)), (\n\
g_{-15}+2 g_{-16}+g_{-14}, g_{15}+2 g_{14}+g_{16}, g_{-1}- g_{-2}- g_{-4}, -2 g_{2}+2 g_{1}- g_{4}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+A^{16}_{1}, ((2, 3, 4, 2), (0, 4, 8, 4)), (\n\
g_{-24}, g_{24}, g_{-6}+g_{-7}, 2 g_{7}+2 g_{6}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{6}_{1}+A^{16}_{1}, ((3, 6, 8, 4), (0, 0, 4, 4)), (\n\
g_{-19}+g_{-20}, g_{20}+g_{19}, -g_{-4}+g_{-3}, 2 g_{3}-2 g_{4}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{8}_{1}+A^{16}_{1}, ((4, 6, 8, 4), (0, 4, 8, 4)), (\n\
g_{-18}- g_{-20}- g_{-11}- g_{-14}, g_{18}- g_{11}- g_{20}- g_{14}, g_{-10}+g_{-7}- g_{-6}+g_{-3}, g_{3}+g_{7}- g_{6}+g_{10}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{6}_{1}+A^{18}_{1}, ((3, 6, 8, 4), (1, 0, 4, 4)), (\n\
g_{-19}+g_{-20}, g_{20}+g_{19}, g_{-3}- g_{-4}+g_{-1}, g_{1}+2 g_{3}-2 g_{4}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+A^{20}_{1}, ((2, 3, 4, 2), (0, 4, 8, 6)), (\n\
g_{-24}, g_{24}, 2g_{-9}+g_{-4}, 3g_{4}+2g_{9}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{4}_{1}+A^{20}_{1}, ((2, 4, 6, 4), (4, 6, 6, 0)), (\n\
g_{-20}+g_{-21}, -g_{22}+g_{21}, g_{-6}+g_{-2}+2g_{-9}+g_{-1}, g_{6}+g_{9}+4g_{1}+2g_{2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+A^{22}_{1}, ((2, 3, 4, 2), (0, 5, 8, 6)), (\n\
g_{-24}, g_{24}, g_{-6}+g_{-2}+3 g_{-7}-3/4 g_{-9}, g_{4}+g_{7}+4g_{9}+4 g_{6}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{4}_{1}+A^{56}_{1}, ((2, 4, 6, 4), (6, 10, 12, 0))),\n\
Serialization{}(LoadCandidateSubalgebra, A^{16}_{1}+A^{56}_{1}, ((4, 8, 12, 8), (6, 10, 12, 0)), (\n\
g_{-12}+g_{-13}, 2 g_{13}+2 g_{12}, g_{-2}- g_{-3}+g_{-1}, -6 g_{3}+10 g_{2}+6 g_{1}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+A^{70}_{1}, ((2, 3, 4, 2), (0, 9, 16, 10)), (\n\
g_{-24}, g_{24}, g_{-4}+3g_{-2}+2g_{-3}, 3g_{2}+5 g_{4}+4g_{3}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{2}, ((2, 3, 4, 2), (-1, 0, 0, 0)), (\n\
g_{-24}, g_24, g_1, g_-1\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{4}_{2}, ((2, 4, 6, 4), (0, 0, 0, -2)), (\n\
g_{-21}, g_21, g_4, g_{-4}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{6}_{2}, ((3, 6, 8, 4), (-1, -3, -2, 0)), (\n\
g_{-19}+g_{-20}, g_{20}+g_{19}, g_{6}+g_{5}, g_{-5}+g_{-6}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{6}_{2}, ((3, 6, 8, 4), (0, -3, -4, -2)), (\n\
g_{-21}+g_{-19}+g_{-20}, g_{19}+g_{20}- g_{17}- g_{14}, g_{9}+g_{16}- g_{6}-2 g_{2}, g_{-16}+g_{-6}+2 g_{-9}- g_{-2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, B^{2}_{2}, ((2, 3, 4, 2), (-1, -1, -1, 0)), (\n\
g_{-24}, g_{24}, g_{8}, g_{-8}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, B^{6}_{2}, ((3, 6, 8, 4), (-1, -3, -3, -1))),\n\
Serialization{}(LoadCandidateSubalgebra, G^{6}_{2}, ((3, 6, 8, 4), (-3, -9, -12, -6)), (\n\
g_{-19}+g_{-20}, g_{20}+g_{19}, g_{23}, g_{-23}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 3 A^{2}_{1}, ((2, 3, 4, 2), (0, 1, 2, 2), (0, 1, 2, 0)), (\n\
g_{-24}, g_{24}, g_{-16}, g_{16}, g_{-9}, g_{9}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 2 A^{2}_{1}+A^{4}_{1}, ((2, 3, 4, 2), (0, 1, 2, 2), (0, 2, 2, 0)), (\n\
g_{-24}, g_{24}, g_{-16}, g_{16}, g_{-6}, g_{6}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 3 A^{4}_{1}, ((2, 4, 6, 4), (2, 2, 2, 0), (0, 2, 2, 0)), (\n\
g_{-21}, g_{21}, g_{-5}+g_{-11}, g_{11}+g_{5}, -g_{-9}+g_{-2}, -g_{9}+g_{2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+A^{4}_{1}+A^{6}_{1}, ((2, 3, 4, 2), (0, 2, 4, 2), (0, 1, 0, 2))),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+A^{6}_{1}+A^{16}_{1}, ((2, 3, 4, 2), (0, 3, 4, 2), (0, 0, 4, 4)), (\n\
g_{-24}, g_{24}, g_{-9}+g_{-10}, g_{10}+g_{9}, g_{-3}+g_{-4}, 2 g_{4}+2 g_{3}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 2A^{2}_{1}+A^{20}_{1}, ((2, 3, 4, 2), (0, 1, 2, 2), (0, 4, 6, 0)), (\n\
g_{-24}, g_{24}, g_{-16}, g_{16}, 2g_{-2}+g_{-3}, 3g_{3}+2 g_{2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{4}_{1}+A^{2}_{2}, ((2, 4, 6, 4), (1, 2, 2, 0), (0, -1, 0, 0)), (\n\
g_{-21}, g_{21}, g_{-14}, g_{14}, g_{2}, g_{-2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{16}_{1}+A^{2}_{2}, ((4, 8, 12, 8), (1, 2, 2, 0), (0, -1, 0, 0)), (\n\
g_{-12}+g_{-13}, 2 g_{13}+2 g_{12}, g_{-14}, g_{14}, g_{2}, g_{-2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+A^{4}_{2}, ((2, 3, 4, 2), (0, 2, 4, 2), (0, 0, -2, 0)), (\n\
g_{-24}, g_{24}, g_{-13}, g_{13}, g_{3}, g_{-3}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{8}_{1}+A^{4}_{2}, ((4, 6, 8, 4), (0, 2, 4, 2), (0, 0, -2, 0)), (\n\
g_{-14}+g_{-18}, 2 g_{18}+2 g_{14}, g_{-13}, g_{13}, g_{3}, g_{-3}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+B^{2}_{2}, ((2, 3, 4, 2), (0, 1, 2, 2), (0, 0, 0, -1)), (\n\
g_{-24}, g_{24}, g_{-16}, g_{16}, g_{4}, g_{-4}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{4}_{1}+B^{2}_{2}, ((2, 4, 6, 4), (1, 2, 2, 0), (0, -1, -1, 0)), (\n\
g_{-21}, g_{21}, g_{-14}, g_{14}, g_{9}+g_{2}, g_{-2}+g_{-9}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{4}_{1}+G^{6}_{2}, ((2, 4, 6, 4), (2, 3, 4, 0), (-3, -3, -6, 0))),\n\
Serialization{}(LoadCandidateSubalgebra, A^{16}_{1}+G^{6}_{2}, ((4, 8, 12, 8), (2, 3, 4, 0), (-3, -3, -6, 0)), (\n\
g_{-12}+g_{-13}, 2g_{13}+2g_{12}, -g_{-9}+g_{-8}, -g_{9}+g_{8}, g_{11}, g_{-11}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{3}, ((2, 3, 4, 2), (-1, 0, 0, 0), (0, -1, 0, 0)), (\n\
g_{-24},  g_{24}, g_{1}, g_{-1}, g_{2}, g_{-2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, B^{2}_{3}, ((2, 3, 4, 2), (-1, 0, 0, 0), (0, -1, -1, 0)), (\n\
g_{-24}, g_{24}, g_{1}, g_{-1}, g_{6}, g_{-6}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, C^{4}_{3}, ((2, 4, 6, 4), (0, 0, 0, -2), (0, -2, -4, 0)), (\n\
g_{-21}, g_{21}, g_{4}, g_{-4}, g_{9}, g_{-9}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 4A^{2}_{1}, ((2, 3, 4, 2), (0, 1, 2, 2), (0, 1, 2, 0), (0, 1, 0, 0)), (\n\
g_{-24}, g_{24}, g_{-16}, g_{16}, g_{-9}, g_{9}, g_{-2}, g_{2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{2}+A^{4}_{2}, ((2, 3, 4, 2), (-1, 0, 0, 0), (0, 0, 2, 2), (0, 0, 0, -2)), (\n\
g_{-24}, g_{24}, g_{1}, g_{-1}, g_{-7}, g_{7}, g_{4}, g_{-4}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, 2A^{2}_{1}+B^{2}_{2}, ((2, 3, 4, 2), (0, 1, 2, 2), (0, 1, 2, 0), (0, 0, -1, 0)), (\n\
g_{-24}, g_{24}, g_{-16}, g_{16}, g_{-9}, g_{9}, g_{3}, g_{-3}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{4}_{1}+A^{2}_{3}, ((2, 4, 6, 4), (1, 2, 2, 0), (0, -1, 0, 0), (-1, 0, 0, 0)), (\n\
g_{-21}, g_{21}, g_{-14}, g_{14}, g_{2}, g_{-2}, g_{1}, g_{-1}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, A^{2}_{1}+(C)^{4}_{3}, ((2, 3, 4, 2), (0, 2, 4, 2), (0, 0, -2, 0), (0, -2, 0, 0)), (\n\
g_{-24}, g_{24}, g_{-13}, g_{13}, g_{3}, g_{-3}, g_{2}, g_{-2}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, D^{2}_{4}, ((2, 3, 4, 2), (-1, 0, 0, 0), (0, -1, 0, 0), (0, -1, -2, 0)), (\n\
g_{-24}, g_{24}, g_{1}, g_{-1}, g_{2}, g_{-2}, g_{9}, g_{-9}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, B^{2}_{4}, ((2, 3, 4, 2), (-1, 0, 0, 0), (0, -1, 0, 0), (0, 0, -1, 0)), (\n\
g_{-24}, g_{24}, g_{1}, g_{-1}, g_{2}, g_{-2}, g_{3}, g_{-3}\n\
)),\n\
Serialization{}(LoadCandidateSubalgebra, F^{2}_{4}, ((2, 3, 4, 2), (-1, 0, 0, 0), (0, -1, -1, 0), (0, 0, 0, -1)), (\n\
g_{-24}, g_{24}, g_{1}, g_{-1}, g_{6}, g_{-6}, g_{4}, g_{-4}\n\
))\n\
\n\
))\n\
)\n\
  ";

}

int main_command_input(int argc, char **argv)
{ std::stringstream theInputStream;
  theParser.theGlobalVariableS->SetFeedDataToIndicatorWindowDefault(&CGI::makeStdCoutReport);
  for (int i=1; i<argc; i++)
  { theInputStream << argv[i];
    if (i<argc-1)
      theInputStream << " ";
  }
//  std::cout << "\n\n\n" << theParser.DisplayPathServerBase << "\n\n";
//  return 0;
  theParser.inputStringRawestOfTheRaw =theInputStream.str();
  PredefinedStrings(theParser.inputStringRawestOfTheRaw);
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
//    std::cout << "input path: " << inputPatH << "\n\n";
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
//  std::cout << "input path: " << inputDisplayPath << "\n\n";
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
  //std::cout << "Chopped strings: ";
  //for (int i=0; i<inputStrings.size; i++)
  //  std::cout << inputStringNames[i] << " = " << inputStrings[i] << "<br>";
  //std::cout << "<hr>";
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

  //civilizedInput="experimentalPrintSemisimpleSubalgebrasForceRecompute{}(d_4)";
//  civilizedInput="WeylGroupIrrepsAndCharTable{}(b_3);";
  //civilizedInput= "experimentalPrintSemisimpleSubalgebrasForceRecompute{}(A_2)";
 // civilizedInput="WeylGroupIrrepsAndCharTable{}(a_2);";
 // civilizedInput="experimentalPrintSemisimpleSubalgebrasForceRecompute{}(g_2)";
// civilizedInput="experimentalPrintSemisimpleSubalgebras{}(d_4)";
  //civilizedInput="PenkovProjectTest";
//  civilizedInput="WeylGroupIrrepsAndCharTable{}(b_3);";
 // civilizedInput="WeylGroupIrrepsAndCharTable{}(c_3);";
  //civilizedInput="Y:=Polynomial{}((a+b)^2);Store{}Y";
//  civilizedInput="PenkovProjectRecompute";
//  civilizedInput="experimentalPrintSemisimpleSubalgebrasFull{}a_2";
//  civilizedInput="experimentalPrintSemisimpleSubalgebrasFull{}A_2";
//  civilizedInput="SplitFDpartB3overG2CharsOnly{}(x_1,2,0)";
//  civilizedInput="WeylGroupGetOuterAutoGenerators{}(2D_4)";
//  civilizedInput="experimentalPrintSemisimpleSubalgebrasForceRecompute{}(D_4)";
//  civilizedInput="\\sqrt {3}+\\sqrt 2";
  //civilizedInput="s_1:=MatrixRationalsTensorForm{}((-1,-1,0,0), (0,1,0,0), (0,0,1,0), (0,0,0,1)); ";
//  civilizedInput="\\sqrt 4";
//  civilizedInput="\\sqrt 2+\\sqrt{}3";
//civilizedInput="FactorOneVarPolyOverRationals{}(x^2-7x+12)";
  PredefinedStrings(civilizedInput);

  std::stringstream tempStreamXX;
  static_html4(tempStreamXX);
  std::cout << "<table>\n <tr valign=\"top\">\n <td>";
  std::cout << "\n<FORM method=\"POST\" name=\"formCalculator\" action=\"" << theParser.DisplayNameCalculator << "\">\n" ;
  std::cout
  << GetSelectHTMLStringTEmp(optionsType, optionsRank, inputWeylString, inputRankString,inputStringNames.Contains
  ("checkUsePreamble"))
  << tempStreamXX.str();
  std::cout << "\n<br>\n";
  std::string civilizedInputSafish;
  if (CGI::GetHtmlStringSafeishReturnFalseIfIdentical(civilizedInput, civilizedInputSafish))
    std::cout
    << "Your input has been treated normally, however the return string"
    << " of your input has been modified. More precisely, &lt; and &gt;  are "
    << " modified due to a javascript hijack issue. <br>";
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
      tempStream << "g_{{i}}:=getChevalleyGenerator{}(" << inputWeylString << "_" << inputRankString
      << ", i); h_{{i}}:=getCartanGenerator{}( " << inputWeylString << "_" << inputRankString << ", i) ;";
      civilizedInput=tempStream.str()+civilizedInput;
    }
    theParser.Evaluate(civilizedInput);
    ComputationComplete=true;
    if (theParser.flagProduceLatexLink)
      std::cout << "<br>LaTeX link (\\usepackage{hyperref}):<br> "
      << CGI::GetLatexEmbeddableLinkFromCalculatorInput(theParser.inputStringRawestOfTheRaw, civilizedInput);

//    std::cout << "<hr>";
    std::cout << theParser.outputString;
    if (theParser.parsingLog!="")
      std::cout << "<b> As requested, here is a calculator parsing log</b><br>" << theParser.parsingLog;
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


