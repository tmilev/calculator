//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".

#include "vpfHeader4SystemFunctionsGlobalObjects.h"
ProjectInformationInstance projectInfoInstanceCalculatorGlobal(__FILE__, "Global objects");


GlobalVariables onePredefinedCopyOfGlobalVariables;
Calculator theParser;
FormatExpressions consoleFormat;


void InitializeGlobalObjects()
{ //stOutput << "Content-Type: text/html\n\n";
  InitializeTimer();
  onePredefinedCopyOfGlobalVariables.IndicatorStringOutputFunction=&CGI::MakeReportIndicatorFile;
  onePredefinedCopyOfGlobalVariables.SetTimerFunction(&GetElapsedTimeInSeconds);
  onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=-1;
  onePredefinedCopyOfGlobalVariables.sleepFunction=SleepFunction;
  onePredefinedCopyOfGlobalVariables.SetCallSystem(&CallSystemWrapper);
  crash.theGlobalVariables=&onePredefinedCopyOfGlobalVariables;
  //stOutput << "address of get elapsed seconds: " << (int) &GetElapsedTimeInSeconds;
  //Change the below line to modify the computation time of the calculator.
  consoleFormat.flagUseHTML = false;
  consoleFormat.flagUseLatex = false;
}

void CGI::MakeReportIndicatorFile(const std::string& input)
{ static int counter =-1;
  counter++;
  //  if (counter%10!=0)
  //    return;
  std::fstream theFile;
  FileOperations::OpenFileCreateIfNotPresent(theFile, theParser.theGlobalVariableS->PhysicalNameIndicatorWithPath, false, true, false);
  std::stringstream outStream;
  theFile << " Elapsed calculator time: " << GetElapsedTimeInSeconds() << " second(s).";
  theFile << input;
  theFile.flush();
  theFile.close();
}

void CGI::MakeStdCoutReport(const std::string& input)
{ stOutput << input;
  CGI::MakeReportIndicatorFile(input);
}


