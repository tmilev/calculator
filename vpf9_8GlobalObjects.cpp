//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".

#include "vpfHeader4SystemFunctionsGlobalObjects.h"
ProjectInformationInstance projectInfoInstanceCalculatorGlobal(__FILE__, "Global objects");


GlobalVariables theGlobalVariables;
CommandList theParser;
FormatExpressions consoleFormat;

void InitializeGlobalObjects()
{ //std::cout << "Content-Type: text/html\n\n";
  InitializeTimer();
  crash.theGlobalVariables=&theGlobalVariables;
  theGlobalVariables.SetFeedDataToIndicatorWindowDefault(&CGI::makeReportIndicatorFile);

  theGlobalVariables.SetTimerFunction(&GetElapsedTimeInSeconds);
  //std::cout << "address of get elapsed seconds: " << (int) &GetElapsedTimeInSeconds;
  //Change the below line to modify the computation time of the calculator.
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=-1;
  theGlobalVariables.SetCallSystem(&CallSystemWrapper);
  consoleFormat.flagUseHTML = false;
  consoleFormat.flagUseLatex = false;
}

void CGI::makeReportIndicatorFile(IndicatorWindowVariables& input)
{ static int counter =-1;
  counter++;
  //  if (counter%10!=0)
  //    return;
  std::fstream theFile;
  XML::OpenFileCreateIfNotPresent(theFile, theParser.indicatorFileNamE, false, true, false);
  std::stringstream outStream;
  theFile << " Elapsed calculator time: " << GetElapsedTimeInSeconds() << " second(s).";
  for (int i=input.ProgressReportStringS.size-1; i>=0; i--)
    theFile << "\n" << input.ProgressReportStringS[i] << "\n<br>\n";
  theFile.flush();
  theFile.close();
}

void CGI::makeStdCoutReport(IndicatorWindowVariables& input)
{ static int counter =-1;
  counter++;
  std::cout << "\nLast progress string: " << *input.ProgressReportStringS.LastObject() << "\n<br>\n";
  CGI::makeReportIndicatorFile(input);
}

bool ComputationComplete;

std::string IPAdressCaller;

