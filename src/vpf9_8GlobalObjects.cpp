//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".

#include "vpfHeader4SystemFunctionsGlobalObjects.h"
#include "webserver.h"
#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"
#include "vpfHeader1General5TimeDate.h"

ProjectInformationInstance projectInfoInstanceCalculatorGlobal(__FILE__, "Global objects");

GlobalVariables theGlobalVariables;
std::string GlobalVariables::GetDateForLogFiles()
{ static std::string tempDate;
  if (tempDate!="")
    return tempDate;
  TimeWrapper now;
  now.AssignLocalTime();
  tempDate=now.ToStringHumanReadable();
  for (unsigned i=0; i<tempDate.size(); i++)
    if (tempDate[i]==' ')
      tempDate[i]='_';
  return tempDate;
}

logger logHttpErrors("LogFiles/LogHttpErrors"+GlobalVariables::GetDateForLogFiles()+".html");
logger theLog("LogFiles/LogStandard"+GlobalVariables::GetDateForLogFiles()+".html");
logger logBlock( "LogFiles/LogBlockingEvents"+GlobalVariables::GetDateForLogFiles()+".html");
logger logIO( "LogFiles/LogIOErrorsEvents"+GlobalVariables::GetDateForLogFiles()+".html");
logger logProcessKills( "LogFiles/LogMultiprocessing"+GlobalVariables::GetDateForLogFiles()+".html");
logger logPlumbing( "LogFiles/LogServerPlumbing"+GlobalVariables::GetDateForLogFiles()+".html");
logger logSocketAccept( "LogFiles/LogSocketAccept"+GlobalVariables::GetDateForLogFiles()+".html");
logger logProcessStats( "LogFiles/LogWorkerProcessStats"+GlobalVariables::GetDateForLogFiles()+".html");
logger logOpenSSL( "LogFiles/LogOpenSSL"+GlobalVariables::GetDateForLogFiles()+".html");
Calculator theParser;
FormatExpressions consoleFormat;
Crasher crash;
StdoutClass stOutput;

void InitializeGlobalObjects()
{ //stOutput << "Content-Type: text/html\n\n";
  InitializeTimer();
  theGlobalVariables.IndicatorStringOutputFunction=&HtmlRoutines::MakeReportIndicatorFile;
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

void HtmlRoutines::MakeReportIndicatorFile(const std::string& input)
{ //calling stOutput forbidden! stOutput itself calls HtmlRoutines::MakeReportIndicatorFile.
  static int counter =-1;
  counter++;
  //  if (counter%10!=0)
  //    return;
  //std::cout << "Making report " << counter << " in file " << theGlobalVariables.PhysicalNameIndicatorWithPath << "<br>";
  std::fstream theFile;
  FileOperations::OpenFileCreateIfNotPresentVirtual
  (theFile, "output/" + theGlobalVariables.RelativePhysicalNameProgressReport, false, true, false);
  std::stringstream outStream;
  theFile << " Elapsed calculator time: " << GetElapsedTimeInSeconds() << " second(s).";
  theFile << input;
  theFile.flush();
  theFile.close();
}

void HtmlRoutines::MakeStdCoutReport(const std::string& input)
{ stOutput << input;
  HtmlRoutines::MakeReportIndicatorFile(input);
}


