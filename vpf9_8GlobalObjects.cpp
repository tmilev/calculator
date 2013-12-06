//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".

#include "vpfHeader4SystemFunctionsGlobalObjects.h"
ProjectInformationInstance projectInfoInstanceCalculatorGlobal(__FILE__, "Global objects");


GlobalVariables theGlobalVariables;
Calculator theParser;
FormatExpressions consoleFormat;

void InitializeGlobalObjects()
{ //std::cout << "Content-Type: text/html\n\n";
  InitializeTimer();
  crash.theGlobalVariables=&theGlobalVariables;
  theGlobalVariables.SetStandardStringOutput(&CGI::MakeReportIndicatorFile);

  theGlobalVariables.SetTimerFunction(&GetElapsedTimeInSeconds);
  //std::cout << "address of get elapsed seconds: " << (int) &GetElapsedTimeInSeconds;
  //Change the below line to modify the computation time of the calculator.
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=-1;
  theGlobalVariables.SetCallSystem(&CallSystemWrapper);
  consoleFormat.flagUseHTML = false;
  consoleFormat.flagUseLatex = false;
}

void CGI::MakeReportIndicatorFile(const std::string& input)
{ static int counter =-1;
  counter++;
  //  if (counter%10!=0)
  //    return;
  std::fstream theFile;
  XML::OpenFileCreateIfNotPresent(theFile, theParser.indicatorFileNamE, false, true, false);
  std::stringstream outStream;
  theFile << " Elapsed calculator time: " << GetElapsedTimeInSeconds() << " second(s).";
  theFile << input;
  theFile.flush();
  theFile.close();
}

void CGI::MakeStdCoutReport(const std::string& input)
{ std::cout << input;
  CGI::MakeReportIndicatorFile(input);
}

bool ComputationComplete;

std::string IPAdressCaller;

