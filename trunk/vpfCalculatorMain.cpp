//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader4SystemFunctionsGlobalObjects.h"
#include "vpfHeader6WebServer.h"
#include "vpfHeader1General3_Test.h"

ProjectInformationInstance projectInfoInstanceCalculatorCpp(__FILE__, "Calculator web interface.");

extern WebServer theWebServer;
int main_apache_client();
int main_command_input(int argc, char **argv);

int main(int argc, char **argv)
{ //for (int i=0; i<argc; i++)
  //  std::cout << "argument " << i << ": " << argv[i] << "\n";
  MacroRegisterFunctionWithName("main");
  try {
  InitializeGlobalObjects();
  onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=500000;
  onePredefinedCopyOfGlobalVariables.MaxComputationTimeBeforeWeTakeAction=5;
  onePredefinedCopyOfGlobalVariables.flagReportEverything=true;
  //	stOutput <<  "<br>" << (int) &theGlobalVariables.callSystem ;
  ParallelComputing::cgiLimitRAMuseNumPointersInList=4000000000;
  if (argc>=1)
    onePredefinedCopyOfGlobalVariables.initDefaultFolderAndFileNames(argv[0], "");
  onePredefinedCopyOfGlobalVariables.flagUsingBuiltInWebServer=false;
  if (argc>=2)
  { std::string tempArgument=argv[1];
    onePredefinedCopyOfGlobalVariables.flagUsingBuiltInWebServer=
    (tempArgument=="server" || tempArgument=="server8155");
    if (tempArgument=="server8155")
      theWebServer.flagPort8155=true;
    if (tempArgument=="test")
    { List<std::string> remainingArguments;
      remainingArguments.SetSize(argc-2);
      for (int i=2; i<argc; i++)
        remainingArguments[i-2]=argv[i];
      return mainTest(remainingArguments);
    }
    if (argc>2)
    { tempArgument=argv[2];
      theWebServer.flagTryToKillOlderProcesses=!(tempArgument=="nokill");
      if (tempArgument=="nokill")
      { if (argc>3)
          tempArgument=argv[3];
        else
          tempArgument="100";
      }
      Rational timeLimit;
      timeLimit.AssignString(tempArgument);
      int timeLimitInt=0;
      if (timeLimit.IsIntegerFittingInInt(& timeLimitInt))
        onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=timeLimitInt;
    }
    if (onePredefinedCopyOfGlobalVariables.flagUsingBuiltInWebServer)
    { int result=theWebServer.Run();
      if (result==0)
      { stOutput << "Server exit normal. ";
        return 0;
      }
      if (result==-1)
      { stOutput << "Server process error. ";
        return 0;
      }
    }
  }
  if (!onePredefinedCopyOfGlobalVariables.flagUsingBuiltInWebServer)
    onePredefinedCopyOfGlobalVariables.flagAllowUseOfThreadsAndMutexes=true;
  //  stOutput << "input path: " << pathDisplayPath << "\n\n";
  theParser.init(onePredefinedCopyOfGlobalVariables);
	if (argc>1 && !onePredefinedCopyOfGlobalVariables.flagUsingBuiltInWebServer)
    return main_command_input(argc, argv);
  if (argc<=1 && !onePredefinedCopyOfGlobalVariables.flagUsingBuiltInWebServer)
    return main_apache_client();
  RegisterFunctionCall theReg(__FILE__, __LINE__, "About to serve Client");
  return theWebServer.GetActiveWorker().ServeClient();
  }
  catch (...)
  { crash << "Something very wrong has happened. " << crash;
  }
  return -1;
}

int main_command_input(int argc, char **argv)
{ MacroRegisterFunctionWithName("main_command_input");
  std::stringstream theInputStream;
  theParser.theGlobalVariableS->IndicatorStringOutputFunction=CGI::MakeStdCoutReport;
  for (int i=1; i<argc; i++)
  { theInputStream << argv[i];
    if (i<argc-1)
      theInputStream << " ";
  }
  //  stOutput << "\n\n\n" << theParser.DisplayPathServerBase << "\n\n";
  //  return 0;
  theParser.inputStringRawestOfTheRaw =theInputStream.str();
/*  theParser.inputStringRawestOfTheRaw="\
  SetOutputFile(\"subalgebras_E^{1}_8\");\
  LoadSemisimpleSubalgebras {}(AmbientDynkinType=E^{1}_{8};CurrentChain={Sequence{}(-1)};NumExploredTypes={Sequence{}(22)};NumExploredHs={Sequence{}(0)};Subalgebras=((DynkinType=A^{1240}_{1};ElementsCartan=\\begin{array}{cccccccc} 92& 136& 182& 270& 220& 168& 114& 58 \\end{array};generators=(g_{-1}+g_{-2}+g_{-3}+g_{-4}+g_{-5}+g_{-6}+g_{-7}+g_{-8},\
58 g_{8}+114 g_{7}+168 g_{6}+220 g_{5}+270 g_{4}+182 g_{3}+136 g_{2}+92 g_{1})),\
(DynkinType=A^{760}_{1};ElementsCartan=\\begin{array}{cccccccc} 72& 106& 142& 210& 172& 132& 90& 46 \\end{array};generators=(g_{-1}+g_{-2}+g_{-3}+g_{-6}+g_{-7}+g_{-8}+g_{-11}+g_{-12},\
172 g_{12}+38 g_{11}-38 g_{10}+46 g_{8}+90 g_{7}+132 g_{6}+104 g_{5}+104 g_{3}+106 g_{2}+72 g_{1})),\
(DynkinType=A^{520}_{1};ElementsCartan=\\begin{array}{cccccccc} 60& 88& 118& 174& 142& 108& 74& 38 \\end{array};generators=(g_{-1}+g_{-2}+g_{-5}+g_{-7}+g_{-8}+g_{-11}+g_{-14}+g_{-20},\
56 g_{20}+52 g_{14}+52 g_{13}+22 g_{12}+118 g_{11}+22 g_{10}+38 g_{8}+22 g_{7}+86 g_{5}+52 g_{3}+88 g_{2}+60 g_{1})),\
(DynkinType=A^{400}_{1};ElementsCartan=\\begin{array}{cccccccc} 52& 76& 102& 152& 124& 96& 66& 34 \\end{array};generators=(g_{-1}+g_{-6}+g_{-7}+g_{-8}+g_{-10}+g_{-11}+g_{-12}+g_{-25},\
75 g_{25}+49 g_{12}+27 g_{11}+g_{10}+34 g_{8}+66 g_{7}+96 g_{6}+52 g_{1})),\
(DynkinType=A^{399}_{1};ElementsCartan=\\begin{array}{cccccccc} 52& 76& 102& 151& 124& 96& 66& 34 \\end{array};generators=(g_{-1}+g_{-6}+g_{-7}+g_{-8}+g_{-17}+g_{-18}+g_{-19},\
75 g_{19}+49 g_{18}+27 g_{17}+34 g_{8}+66 g_{7}+96 g_{6}+52 g_{1})),\
(DynkinType=A^{399}_{1}+A^{1}_{1};ElementsCartan=\\begin{array}{cccccccc} 52& 76& 102& 151& 124& 96& 66& 34\\\\ 0& 0& 0& 1& 0& 0& 0& 0 \\end{array};generators=(g_{-1}+g_{-6}+g_{-7}+g_{-8}+g_{-17}+g_{-18}+g_{-19},\
75 g_{19}+49 g_{18}+27 g_{17}+34 g_{8}+66 g_{7}+96 g_{6}+52 g_{1},\
g_{-4}, g_{4})),\
(DynkinType=A^{280}_{1};ElementsCartan=\\begin{array}{cccccccc} 44& 64& 86& 128& 104& 80& 54& 28 \\end{array};generators=(g_{-1}+g_{-6}+g_{-10}+g_{-11}+g_{-12}+g_{-15}+g_{-21}+g_{-25},\
50 g_{25}+26 g_{21}+28 g_{15}+28 g_{12}+36 g_{11}+14 g_{10}+54 g_{6}+44 g_{1})),\
(DynkinType=A^{232}_{1};ElementsCartan=\\begin{array}{cccccccc} 40& 58& 78& 116& 94& 72& 50& 26 \\end{array};generators=(g_{-1}+g_{-4}+g_{-7}+g_{-8}+g_{-18}+g_{-19}+15 g_{-20}+g_{-33},\
57 g_{33}+g_{21}+g_{20}+21 g_{19}+g_{18}-330 g_{17}+26 g_{8}+50 g_{7}+22 g_{4}+40 g_{1})),\
(DynkinType=A^{231}_{1};ElementsCartan=\\begin{array}{cccccccc} 40& 58& 78& 115& 94& 72& 50& 26 \\end{array};generators=(g_{-1}+g_{-7}+g_{-8}+g_{-17}+g_{-18}-\\frac{1}{22} g_{-26}+g_{-27},\
57 g_{27}-330 g_{26}+g_{19}+22 g_{18}+21 g_{17}-330 g_{14}+26 g_{8}+50 g_{7}+40 g_{1})),\
(DynkinType=A^{231}_{1}+A^{1}_{1};ElementsCartan=\\begin{array}{cccccccc} 40& 58& 78& 115& 94& 72& 50& 26\\\\ 0& 0& 0& 1& 0& 0& 0& 0 \\end{array};generators=(g_{-1}+g_{-7}+g_{-8}+g_{-17}+g_{-18}-\\frac{1}{22} g_{-26}+g_{-27},\
57 g_{27}-330 g_{26}+g_{19}+22 g_{18}+21 g_{17}-330 g_{14}+26 g_{8}+50 g_{7}+40 g_{1},\
g_{-4}, g_{4})),\
(DynkinType=A^{184}_{1};ElementsCartan=\\begin{array}{cccccccc} 36& 52& 70& 104& 84& 64& 44& 22 \\end{array};generators=(g_{-1}+g_{-4}+g_{-17}+g_{-18}+g_{-19}+g_{-20}+g_{-21}+g_{-22},\
-20 g_{33}+22 g_{22}+22 g_{21}+20 g_{20}+30 g_{19}+12 g_{18}+40 g_{17}+2 g_{7}+2 g_{4}+36 g_{1})),\
(DynkinType=A^{182}_{1};ElementsCartan=\\begin{array}{cccccccc} 36& 52& 70& 103& 84& 64& 43& 22 \\end{array};generators=(g_{-1}+g_{-13}+g_{-17}+g_{-18}+g_{-19}+g_{-22}+g_{-28},\
21 g_{28}+22 g_{22}+30 g_{19}+12 g_{18}+40 g_{17}+21 g_{13}+36 g_{1})),\
(DynkinType=A^{182}_{1}+A^{2}_{1};ElementsCartan=\\begin{array}{cccccccc} 36& 52& 70& 103& 84& 64& 43& 22\\\\ 0& 0& 0& 1& 0& 0& 1& 0 \\end{array};generators=(g_{-1}+g_{-17}+g_{-18}+g_{-19}+g_{-20}+g_{-21}+g_{-22},\
22 g_{22}+21 g_{21}+21 g_{20}+30 g_{19}+12 g_{18}+40 g_{17}+36 g_{1},\
g_{-4}+g_{-7}, g_{7}+g_{4})),\
(DynkinType=A^{160}_{1};ElementsCartan=\\begin{array}{cccccccc} 32& 48& 64& 96& 78& 60& 42& 22 \\end{array};generators=(g_{-4}+g_{-7}+g_{-8}-72 g_{-12}+\\frac{32}{3} g_{-18}-\\frac{10}{3} g_{-20}+g_{-25}+g_{-27}+36 g_{-32}+g_{-38},\
30 g_{38}-888 g_{33}+\\frac{1}{18} g_{32}-\\frac{1}{18} g_{30}+\\frac{74}{3} g_{27}+36 g_{26}+\\frac{22}{3} g_{25}+\\frac{1}{324} g_{24}+24 g_{23}- g_{21}- g_{20}-\\frac{1}{18} g_{19}+g_{18}+1296 g_{17}+g_{16}-432 g_{14}+22 g_{8}+42 g_{7}+18 g_{4})),\
(DynkinType=A^{159}_{1};ElementsCartan=\\begin{array}{cccccccc} 32& 48& 64& 95& 78& 60& 42& 22 \\end{array};generators=(g_{-7}+g_{-8}+g_{-17}+g_{-18}+g_{-19}+g_{-26}+g_{-32},\
32 g_{32}+28 g_{26}-3 g_{24}+3 g_{23}+15 g_{19}+3 g_{18}+17 g_{17}+28 g_{14}+22 g_{8}+42 g_{7})),\
(DynkinType=A^{159}_{1}+A^{1}_{1};ElementsCartan=\\begin{array}{cccccccc} 32& 48& 64& 95& 78& 60& 42& 22\\\\ 0& 0& 0& 1& 0& 0& 0& 0 \\end{array};generators=(g_{-7}+g_{-8}+g_{-17}+g_{-18}+g_{-19}+g_{-26}+g_{-32},\
32 g_{32}+28 g_{26}-3 g_{24}+3 g_{23}+15 g_{19}+3 g_{18}+17 g_{17}+28 g_{14}+22 g_{8}+42 g_{7},\
g_{-4},\
g_{4})),\
(DynkinType=A^{157}_{1};ElementsCartan=\\begin{array}{cccccccc} 32& 47& 63& 94& 77& 60& 42& 22 \\end{array};generators=(g_{-7}+g_{-8}+g_{-16}+g_{-20}+g_{-30}+g_{-31}+g_{-33},\
30 g_{33}+g_{31}+16 g_{30}+30 g_{20}+16 g_{16}+22 g_{8}+42 g_{7})),\
(DynkinType=A^{157}_{1}+A^{3}_{1};ElementsCartan=\\begin{array}{cccccccc} 32& 47& 63& 94& 77& 60& 42& 22\\\\ 0& 1& 1& 0& 1& 0& 0& 0 \\end{array};generators=(g_{-7}+g_{-8}+g_{-23}+g_{-24}+g_{-26}+g_{-27}+g_{-31},\
g_{31}+30 g_{27}+30 g_{26}+16 g_{24}+16 g_{23}+22 g_{8}+42 g_{7},\
g_{-2}- g_{-3}+g_{-5}, g_{5}- g_{3}+g_{2})),\
(DynkinType=A^{156}_{1};ElementsCartan=\\begin{array}{cccccccc} 32& 46& 62& 92& 76& 60& 42& 22 \\end{array};generators=(g_{-7}+g_{-8}+g_{-9}+g_{-13}+g_{-37}+g_{-39}, 30 g_{39}+16 g_{37}+30 g_{13}+16 g_{9}+22 g_{8}+42 g_{7})),\
(DynkinType=A^{156}_{1}+A^{28}_{1};ElementsCartan=\\begin{array}{cccccccc} 32& 46& 62& 92& 76& 60& 42& 22\\\\ 0& 6& 6& 10& 6& 0& 0& 0 \\end{array};generators=(g_{-7}+g_{-8}+g_{-23}+g_{-24}+g_{-26}+g_{-27}, 30 g_{27}+30 g_{26}+16 g_{24}+16 g_{23}+22 g_{8}+42 g_{7},\
g_{-2}- g_{-3}+g_{-4}+g_{-5}, 6 g_{5}+10 g_{4}-6 g_{3}+6 g_{2})),\
(DynkinType=A^{156}_{1}+A^{4}_{1};ElementsCartan=\\begin{array}{cccccccc} 32& 46& 62& 92& 76& 60& 42& 22\\\\ 0& 2& 2& 4& 2& 0& 0& 0 \\end{array};generators=(g_{-7}+g_{-8}+g_{-23}+g_{-24}+g_{-26}+g_{-27}, 30 g_{27}+30 g_{26}+16 g_{24}+16 g_{23}+22 g_{8}+42 g_{7},\
g_{-4}+g_{-25}, 2 g_{25}+2 g_{4})),\
(DynkinType=A^{156}_{1}+A^{3}_{1};ElementsCartan=\\begin{array}{cccccccc} 32& 46& 62& 92& 76& 60& 42& 22\\\\ 0& 2& 2& 3& 2& 0& 0& 0 \\end{array};generators=(g_{-7}+g_{-8}+g_{-23}+g_{-24}+g_{-26}+g_{-27}, 30 g_{27}+30 g_{26}+16 g_{24}+16 g_{23}+22 g_{8}+42 g_{7},\
g_{-17}+g_{-18}- g_{-19}, - g_{19}+g_{18}+g_{17})),\
(DynkinType=A^{156}_{1}+A^{3}_{1}+A^{1}_{1};ElementsCartan=\\begin{array}{cccccccc} 32& 46& 62& 92& 76& 60& 42& 22\\\\ 0& 2& 2& 3& 2& 0& 0& 0\\\\ 0& 0& 0& 1& 0& 0& 0& 0 \\end{array};generators=(g_{-7}+g_{-8}+g_{-23}+g_{-24}+g_{-26}+g_{-27}, 30 g_{27}+30 g_{26}+16 g_{24}+16 g_{23}+22 g_{8}+42 g_{7},\
g_{-17}+g_{-18}- g_{-19}, - g_{19}+g_{18}+g_{17}, g_{-4},\
g_{4})),\
(DynkinType=A^{156}_{1}+A^{1}_{1};ElementsCartan=\\begin{array}{cccccccc} 32& 46& 62& 92& 76& 60& 42& 22\\\\ 0& 1& 1& 2& 1& 0& 0& 0 \\end{array};generators=(g_{-7}+g_{-8}+g_{-16}+g_{-20}+g_{-30}+g_{-33}, 30 g_{33}+16 g_{30}+30 g_{20}+16 g_{16}+22 g_{8}+42 g_{7},\
g_{-31}, g_{31})),\
(DynkinType=A^{120}_{1};ElementsCartan=\\begin{array}{cccccccc} 28& 42& 56& 84& 68& 52& 36& 18 \\end{array};generators=(g_{-7}+g_{-10}+g_{-11}+g_{-12}+g_{-25}+g_{-29}+g_{-32}+g_{-33},\
6 g_{33}+28 g_{32}+18 g_{29}+12 g_{25}-12 g_{24}+12 g_{23}+6 g_{14}+4 g_{12}+10 g_{11}+24 g_{10}+18 g_{7})),\
(DynkinType=A^{112}_{1};ElementsCartan=\\begin{array}{cccccccc} 28& 40& 54& 80& 66& 50& 34& 18 \\end{array};generators=(g_{-1}+g_{-5}+g_{-8}+g_{-20}+g_{-31}+g_{-33}+g_{-34}+g_{-35},\
24 g_{35}+10 g_{34}+15 g_{33}+15 g_{31}+g_{20}+18 g_{8}+g_{5}+28 g_{1})),\
(DynkinType=A^{111}_{1};ElementsCartan=\\begin{array}{cccccccc} 28& 40& 54& 80& 65& 50& 34& 18 \\end{array};generators=(g_{-1}+g_{-8}+g_{-26}+g_{-27}+g_{-28}+g_{-31}+g_{-41},\
24 g_{41}+15 g_{31}+10 g_{28}+15 g_{27}+g_{26}+18 g_{8}+28 g_{1})),\
(DynkinType=A^{111}_{1}+A^{1}_{1};ElementsCartan=\\begin{array}{cccccccc} 28& 40& 54& 80& 65& 50& 34& 18\\\\ 0& 0& 0& 0& 1& 0& 0& 0 \\end{array};generators=(g_{-1}+g_{-8}+g_{-26}+g_{-27}+g_{-28}+g_{-31}+g_{-41},\
24 g_{41}+15 g_{31}+10 g_{28}+15 g_{27}+g_{26}+18 g_{8}+28 g_{1},\
g_{-5}, g_{5})),\
(DynkinType=A^{110}_{1};ElementsCartan=\\begin{array}{cccccccc} 28& 40& 54& 79& 64& 49& 34& 18 \\end{array};generators=(g_{-1}+g_{-8}+g_{-25}+g_{-34}+g_{-35}+g_{-39}, 15 g_{39}+24 g_{35}+10 g_{34}+15 g_{25}+18 g_{8}+28 g_{1})),\
(DynkinType=A^{110}_{1}+A^{10}_{1};ElementsCartan=\\begin{array}{cccccccc} 28& 40& 54& 79& 64& 49& 34& 18\\\\ 0& 0& 0& 3& 4& 3& 0& 0 \\end{array};generators=(g_{-1}+g_{-8}+g_{-31}+g_{-33}+g_{-34}+g_{-35}, 24 g_{35}+10 g_{34}+15 g_{33}+15 g_{31}+18 g_{8}+28 g_{1},\
g_{-4}+g_{-5}+g_{-6}, 3 g_{6}+4 g_{5}+3 g_{4})),\
(DynkinType=A^{110}_{1}+A^{2}_{1};ElementsCartan=\\begin{array}{cccccccc} 28& 40& 54& 79& 64& 49& 34& 18\\\\ 0& 0& 0& 1& 2& 1& 0& 0 \\end{array};generators=(g_{-1}+g_{-8}+g_{-25}+g_{-34}+g_{-35}+g_{-39}, 15 g_{39}+24 g_{35}+10 g_{34}+15 g_{25}+18 g_{8}+28 g_{1},\
- g_{-5}+g_{-20}, g_{20}- g_{5})),\
(DynkinType=A^{110}_{1}+A^{1}_{1};ElementsCartan=\\begin{array}{cccccccc} 28& 40& 54& 79& 64& 49& 34& 18\\\\ 0& 0& 0& 1& 1& 1& 0& 0 \\end{array};generators=(g_{-1}+g_{-8}+g_{-31}+g_{-33}+g_{-34}+g_{-35}, 24 g_{35}+10 g_{34}+15 g_{33}+15 g_{31}+18 g_{8}+28 g_{1},\
g_{-20}, g_{20})),\
(DynkinType=A^{110}_{1}+2 A^{1}_{1};ElementsCartan=\\begin{array}{cccccccc} 28& 40& 54& 79& 64& 49& 34& 18\\\\ 0& 0& 0& 1& 1& 1& 0& 0\\\\ 0& 0& 0& 0& 1& 0& 0& 0 \\end{array};generators=(g_{-1}+g_{-8}+g_{-31}+g_{-33}+g_{-34}+g_{-35}, 24 g_{35}+10 g_{34}+15 g_{33}+15 g_{31}+18 g_{8}+28 g_{1},\
g_{-20}, g_{20}, g_{-5}, g_{5})),\
(DynkinType=A^{88}_{1};ElementsCartan=\\begin{array}{cccccccc} 24& 36& 48& 72& 58& 44& 30& 16 \\end{array};generators=(g_{-4}+g_{-8}+g_{-18}+g_{-19}-\\frac{1}{2} g_{-20}+g_{-32}+g_{-33}+g_{-35}+g_{-47},\
18 g_{47}+12 g_{35}+8 g_{33}+6 g_{32}-4 g_{30}+12 g_{22}+4 g_{19}+10 g_{18}+7 g_{16}+16 g_{8}+14 g_{4})),\
(DynkinType=A^{85}_{1};ElementsCartan=\\begin{array}{cccccccc} 24& 35& 47& 70& 57& 44& 30& 16 \\end{array};generators=(g_{-8}+g_{-16}+g_{-20}+8 g_{-28}+g_{-30}+g_{-31}+g_{-41},\
22 g_{41}-112 g_{33}+g_{31}+12 g_{30}+g_{28}+14 g_{20}+12 g_{16}+g_{15}+16 g_{8})),\
(DynkinType=A^{84}_{1};ElementsCartan=\\begin{array}{cccccccc} 24& 34& 46& 68& 56& 44& 30& 16 \\end{array};generators=(g_{-8}+g_{-16}+g_{-27}+g_{-30}+g_{-34}+g_{-35}, 8 g_{35}+22 g_{34}+12 g_{30}+14 g_{27}-14 g_{26}+12 g_{16}+8 g_{15}+16 g_{8})),\
(DynkinType=A^{84}_{1}+A^{4}_{1};ElementsCartan=\\begin{array}{cccccccc} 24& 34& 46& 68& 56& 44& 30& 16\\\\ 0& 2& 2& 4& 2& 0& 0& 0 \\end{array};generators=(g_{-8}+g_{-16}+g_{-27}+g_{-30}+g_{-34}+g_{-35}, 8 g_{35}+22 g_{34}+12 g_{30}+14 g_{27}-14 g_{26}+12 g_{16}+8 g_{15}+16 g_{8},\
g_{-12}+g_{-17}, 2 g_{17}+2 g_{12})),\
(DynkinType=A^{84}_{1}+A^{1}_{1};ElementsCartan=\\begin{array}{cccccccc} 24& 34& 46& 68& 56& 44& 30& 16\\\\ 0& 1& 1& 2& 1& 0& 0& 0 \\end{array};generators=(g_{-8}+g_{-16}+g_{-27}+g_{-30}+g_{-34}+g_{-35}, 8 g_{35}+22 g_{34}+12 g_{30}+14 g_{27}-14 g_{26}+12 g_{16}+8 g_{15}+16 g_{8},\
g_{-31}, g_{31})),\
(DynkinType=A^{84}_{1};ElementsCartan=\\begin{array}{cccccccc} 24& 35& 47& 70& 57& 44& 30& 15 \\end{array};generators=(g_{-14}+g_{-16}+g_{-20}+g_{-29}+g_{-30}+g_{-31}+g_{-33},\
7 g_{33}+16 g_{31}+12 g_{30}+15 g_{29}+7 g_{20}+12 g_{16}+15 g_{14})),\
(DynkinType=A^{84}_{1}+A^{4}_{1};ElementsCartan=\\begin{array}{cccccccc} 24& 35& 47& 70& 57& 44& 30& 15\\\\ 0& 1& 1& 0& 1& 0& 0& 1 \\end{array};generators=(g_{-21}+g_{-22}+g_{-23}+g_{-24}+g_{-26}+g_{-27}+g_{-31},\
16 g_{31}+7 g_{27}+7 g_{26}+12 g_{24}+12 g_{23}+15 g_{22}+15 g_{21},\
g_{-2}- g_{-3}+g_{-5}+g_{-8}, g_{8}+g_{5}- g_{3}+g_{2})),\
(DynkinType=A^{70}_{1};ElementsCartan=\\begin{array}{cccccccc} 22& 32& 43& 64& 52& 40& 27& 14 \\end{array};generators=(g_{-16}-6 g_{-20}+g_{-41}+g_{-31}-3 g_{-24}+g_{-26}+g_{-30}+g_{-29},\
18 g_{16}- g_{20}+13 g_{41}+54 g_{23}+g_{35}+7 g_{26}+8 g_{31}+4 g_{30}+14 g_{29})),\
 (DynkinType=(A{}\\left(1\\right))^{64};ElementsCartan=(20, 30, 40, 60, 50, 38, 26, 14);generators=(g_{-5}+g_{-22}- g_{-25}+g_{-26}+g_{-27}+g_{-30}+g_{-31}+g_{-32}- g_{-33}+g_{-47}+g_{-48}+2 g_{-51}+g_{-58}, \\\\\
2 g_{58}+2 g_{48}+8 g_{47}+8 g_{45}-2 g_{44}-4 g_{40}-10 g_{37}+10 g_{32}+16 g_{31}+2 g_{26}+10 g_{24}+14 g_{22}-10 g_{21}-6 g_{20}+8 g_{18}+14 g_{15}+26 g_{12}+14 g_{8}+10 g_{5}))\
))";
*/


  theParser.flagUseHtml=false;
  theParser.Evaluate(theParser.inputStringRawestOfTheRaw);
  std::fstream outputFile;
  FileOperations::OpenFileCreateIfNotPresent(outputFile, "./outputFileCommandLine.html", false, true, false);
  stOutput << theParser.outputString;
  outputFile << theParser.outputString;
  stOutput << "\nTotal running time: " << GetElapsedTimeInSeconds() << " seconds. \nOutput written in file ./outputFileCommandLine.html\n";
  return 0;
}

int main_apache_client()
{ MacroRegisterFunctionWithName("main_apache_client");
  stOutput << "Content-Type: text/html\n\n";
  theParser.theGlobalVariableS->IndicatorStringOutputFunction=CGI::MakeReportIndicatorFile;
  std::cin >> theParser.inputStringRawestOfTheRaw;

	if (theParser.inputStringRawestOfTheRaw=="")
	{ theParser.inputStringRawestOfTheRaw=getenv("QUERY_STRING");
    std::string& IPAdressCaller=onePredefinedCopyOfGlobalVariables.IPAdressCaller;
    IPAdressCaller=getenv("REMOTE_ADDR");
    for (int i=0; i<MathRoutines::Minimum((int)IPAdressCaller.size(), SomeRandomPrimesSize); i++)
      IPAdressCaller[i]='A'+(IPAdressCaller[i]*SomeRandomPrimes[i])%26;
	}
	theParser.javaScriptDisplayingIndicator=WebWorker::GetJavaScriptIndicatorFromHD();
  return WebWorker::OutputWeb();
}
