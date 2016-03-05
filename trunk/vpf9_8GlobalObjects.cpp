//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".

#include "vpfHeader4SystemFunctionsGlobalObjects.h"
#include "vpfHeader6WebServer.h"
#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"
ProjectInformationInstance projectInfoInstanceCalculatorGlobal(__FILE__, "Global objects");

GlobalVariables theGlobalVariables;
logger theLog( "./../output/LogStandard.html");
logger logBlock( "./../output/LogBlockingEvents.html");
logger logIO( "./../output/LogIOErrorsEvents.html");
logger logConnections( "./../output/LogConnections.html");
WebServer theWebServer;
Calculator theParser;
FormatExpressions consoleFormat;
Crasher crash;
StdoutClass stOutput;

void InitializeGlobalObjects()
{ //stOutput << "Content-Type: text/html\n\n";
  InitializeTimer();
  theGlobalVariables.IndicatorStringOutputFunction=&CGI::MakeReportIndicatorFile;
  theGlobalVariables.SetTimerFunction(&GetElapsedTimeInSeconds);
  theGlobalVariables.sleepFunction=SleepFunction;
  theGlobalVariables.pointerCallSystemNoOutput=&CallSystemWrapperNoOutput;
  theGlobalVariables.pointerCallSystemWithOutput=&CallSystemWrapperReturnStandardOutput;
  theGlobalVariables.pointerCallChDir=&CallChDirWrapper;

  //stOutput << "address of get elapsed seconds: " << (int) &GetElapsedTimeInSeconds;
  //Change the below line to modify the computation time of the calculator.
  consoleFormat.flagUseHTML = false;
  consoleFormat.flagUseLatex = false;
}

void CGI::MakeReportIndicatorFile(const std::string& input)
{ //calling stOutput forbidden! stOutput itself calls CGI::MakeReportIndicatorFile.
  static int counter =-1;
  counter++;
  //  if (counter%10!=0)
  //    return;
  //std::cout << "Making report " << counter << " in file " << theGlobalVariables.PhysicalNameIndicatorWithPath << "<br>";
  std::fstream theFile;
  FileOperations::OpenFileCreateIfNotPresentOnTopOfOutputFolder
  (theFile, theGlobalVariables.RelativePhysicalNameProgressReport, false, true, false);
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


