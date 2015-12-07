//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader4SystemFunctionsGlobalObjects.h"
#include "vpfHeader6WebServer.h"
#include "vpfHeader1General3_Test.h"
#include "vpfHeader3Calculator0_Interface.h"

ProjectInformationInstance projectInfoInstanceCalculatorCpp(__FILE__, "Calculator web interface.");

int main_apache_client();
int main_command_input(int argc, char **argv);

int main(int argc, char **argv)
{ //theGlobalVariables.InitializeThreadData();
  ThreadData::RegisterCurrentThread("main");
  theGlobalVariables.theThreads.SetSize(1);
  //for (int i=0; i<argc; i++)
  //  std::cout << "argument " << i << ": " << argv[i] << "\n";
  MacroRegisterFunctionWithName("main");
  try {
  InitializeGlobalObjects();
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=5000000;
  theGlobalVariables.MaxComputationTimeBeforeWeTakeAction=5;
  theGlobalVariables.flagReportEverything=true;
  //	stOutput <<  "<br>" << (int) &theGlobalVariables.callSystem ;
  ParallelComputing::cgiLimitRAMuseNumPointersInList=4000000000;

  if (argc>=1)
  { std::string firstArg;
    //std::cout << "argc: " << argc << " argv[0]: " << argv[0] << "\n";
    firstArg=argv[0];
    theGlobalVariables.initDefaultFolderAndFileNames(firstArg, "");
  }
  theGlobalVariables.flagUsingBuiltInWebServer=false;
  if (argc>=2)
  { std::string tempArgument=argv[1];
    theGlobalVariables.flagUsingBuiltInWebServer=
    (tempArgument=="server" || tempArgument=="server8155" || tempArgument=="serverSSL");
    if (tempArgument=="server8155")
      theWebServer.flagPort8155=true;
    if (tempArgument=="serverSSL")
      theWebServer.flagUsESSL=true;
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
        theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=timeLimitInt;
    }
    if (theGlobalVariables.flagUsingBuiltInWebServer)
    { int result=-1;
      result=theWebServer.Run();
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
  if (!theGlobalVariables.flagUsingBuiltInWebServer)
    theGlobalVariables.flagAllowUseOfThreadsAndMutexes=true;
  //  stOutput << "input path: " << pathDisplayPath << "\n\n";
  theParser.init();
	if (argc>1 && !theGlobalVariables.flagUsingBuiltInWebServer)
    return main_command_input(argc, argv);
  if (argc<=1 && !theGlobalVariables.flagUsingBuiltInWebServer)
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
  theGlobalVariables.IndicatorStringOutputFunction=CGI::MakeStdCoutReport;
  for (int i=1; i<argc; i++)
  { theInputStream << argv[i];
    if (i<argc-1)
      theInputStream << " ";
  }
  //  stOutput << "\n\n\n" << theParser.DisplayPathServerBase << "\n\n";
  //  return 0;
  theParser.inputStringRawestOfTheRaw =theInputStream.str();
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
  theGlobalVariables.IndicatorStringOutputFunction=CGI::MakeReportIndicatorFile;
  std::cin >> theParser.inputStringRawestOfTheRaw;

	if (theParser.inputStringRawestOfTheRaw=="")
	{ theParser.inputStringRawestOfTheRaw=getenv("QUERY_STRING");
    std::string& IPAdressCaller=theGlobalVariables.IPAdressCaller;
    IPAdressCaller=getenv("REMOTE_ADDR");
    for (int i=0; i<MathRoutines::Minimum((int)IPAdressCaller.size(), SomeRandomPrimesSize); i++)
      IPAdressCaller[i]='A'+(IPAdressCaller[i]*SomeRandomPrimes[i])%26;
	}
	theParser.javaScriptDisplayingIndicator=WebWorker::GetJavaScriptIndicatorFromHD();
  return WebWorker::OutputWeb();
}
