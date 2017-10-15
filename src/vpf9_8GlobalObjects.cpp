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

logger theLog("LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/logCommon.html", 0, true, "worker");
logger logServerMonitor("LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/logServerMonitor.html", 0, false, "serverMonitor");
logger logServer("LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/logServer.html", 0, false, "server");

logger logHttpErrors("LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogHttpErrors.html", &theLog, false, "worker");
logger logBlock("LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogBlockingEvents.html", &theLog, false, "worker");
logger logIO("LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogIOErrorsEvents.html", &theLog, false, "worker");
logger logOpenSSL("LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogOpenSSL.html", &theLog, false, "worker");
logger logEmail("LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogEmail.html", &theLog, false, "worker");

logger logSocketAccept("LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogSocketAccept.html", &logServer, false, "server");
logger logProcessStats("LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogWorkerProcessStats.html", &logServer, false, "server");
logger logPlumbing("LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogServerPlumbing.html", &logServer, false, "server");
logger logProcessKills("LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogMultiprocessing.html", &logServer, false, "server");
logger logSuccessfulForks("LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogForkSuccess.html", &logServer, false, "server");

Calculator theParser;
FormatExpressions consoleFormat;
Crasher crash;
StdoutClass stOutput;

void InitializeGlobalObjects()
{ //stOutput << "Content-Type: text/html\n\n";
  theGlobalVariables.processType="server";
  theGlobalVariables.flagIsChildProcess=false;
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


