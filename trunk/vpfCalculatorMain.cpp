//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader4SystemFunctionsGlobalObjects.h"
#include "vpfHeader6WebServer.h"

ProjectInformationInstance projectInfoInstanceCalculatorCpp(__FILE__, "Calculator web interface.");

extern WebServer theWebServer;
int main_apache_client();
int main_command_input(int argc, char **argv);

int main(int argc, char **argv)
{ MacroRegisterFunctionWithName("main");
  InitializeGlobalObjects();
  onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=500000;
  onePredefinedCopyOfGlobalVariables.MaxComputationTimeBeforeWeTakeAction=5;

  onePredefinedCopyOfGlobalVariables.flagReportLargeIntArithmetic=true;


  //	stOutput <<  "<br>" << (int) &theGlobalVariables.callSystem ;
  ParallelComputing::cgiLimitRAMuseNumPointersInList=4000000000;
  if (argc>=1)
    onePredefinedCopyOfGlobalVariables.initDefaultFolderAndFileNames(argv[0], "");
  theWebServer.flagUsingBuiltInServer=false;
  if (argc>=2)
  { std::string tempArgument=argv[1];
    theWebServer.flagUsingBuiltInServer=(tempArgument=="server");
    if (argc>2)
    { tempArgument=argv[2];
      theWebServer.flagTryToKillOlderProcesses=!(tempArgument=="nokill");
    }
    if (theWebServer.flagUsingBuiltInServer)
      if (theWebServer.Run()==0)
      { stOutput << "Server exit normal. ";
        return 0;
      }
  }
  //  stOutput << "input path: " << pathDisplayPath << "\n\n";
  theParser.init(onePredefinedCopyOfGlobalVariables);
	if (argc>1 && !theWebServer.flagUsingBuiltInServer)
    return main_command_input(argc, argv);
  if (argc<=1 && !theWebServer.flagUsingBuiltInServer)
    return main_apache_client();
  return theWebServer.GetActiveWorker().ServeClient();
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
  PredefinedStrings(theParser.inputStringRawestOfTheRaw);
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
