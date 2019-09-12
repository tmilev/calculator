//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".

#include "vpfHeader4SystemFunctionsGlobalObjects.h"
#include "webserver.h"
#include "calculator_1_interface.h"
#include "general_4_logging_global_variables.h"
#include "general_5_time_date.h"

ProjectInformationInstance projectInfoInstanceCalculatorGlobal(__FILE__, "Global objects");

GlobalVariables theGlobalVariables;
std::string GlobalVariables::GetDateForLogFiles() {
  static std::string tempDate;
  if (tempDate != "") {
    return tempDate;
  }
  TimeWrapper now;
  now.AssignLocalTime();
  std::stringstream out;
  out << "GM_" << now.ToStringGM();
  tempDate = out.str();
  for (unsigned i = 0; i < tempDate.size(); i ++) {
    if (tempDate[i] == ' ') {
      tempDate[i] = '_';
    }
  }
  return tempDate;
}

std::string GlobalVariables::GetTimeGM() {
  TimeWrapper now;
  now.AssignLocalTime();
  return now.ToStringGM();
}

std::string GlobalVariables::GetTimeLocal() {
  TimeWrapper now;
  now.AssignLocalTime();
  return now.ToStringLocal();
}

std::string ProcessTypes::worker = "worker";
std::string ProcessTypes::server = "server";
std::string ProcessTypes::serverMonitor = "serverMonitor";

logger logWorker("/LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/logCommon.html", 0, true, ProcessTypes::worker);
logger logServerMonitor("/LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/logServerMonitor.html", 0, false, ProcessTypes::serverMonitor);
logger logServer("/LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/logServer.html", 0, false, ProcessTypes::server);

logger logHttpErrors("/LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogHttpErrors.html", &logWorker, false, ProcessTypes::worker);
logger logBlock("/LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogBlockingEvents.html", &logWorker, false, ProcessTypes::worker);
logger logIO("/LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogIOErrorsEvents.html", &logWorker, false, ProcessTypes::worker);
logger logOpenSSL("/LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogOpenSSL.html", &logWorker, false, ProcessTypes::worker);
logger logEmail("/LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogEmail.html", &logWorker, false, ProcessTypes::worker);

logger logSocketAccept("/LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogSocketAccept.html", &logServer, false, ProcessTypes::server);
logger logProcessStats("/LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogWorkerProcessStats.html", &logServer, false, ProcessTypes::server);
logger logPlumbing("/LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogServerPlumbing.html", &logServer, false, ProcessTypes::server);
logger logProcessKills("/LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogMultiprocessing.html", &logServer, false, ProcessTypes::server);
logger logSuccessfulForks("/LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogForkSuccess.html", &logServer, false, ProcessTypes::server);

Calculator* theParser = 0;
FormatExpressions consoleFormat;
Crasher crash;
StdoutClass stOutput;

void InitializeGlobalObjects() {
  //stOutput << "Content-Type: text/html\n\n";
  theGlobalVariables.processType = ProcessTypes::server;
  theGlobalVariables.flagIsChildProcess = false;
  InitializeTimeR();
  theGlobalVariables.IndicatorStringOutputFunction = &HtmlRoutines::MakeReportIndicatorFile;
  theGlobalVariables.pointerCallSystemNoOutput = &CallSystemWrapperNoOutput;
  theGlobalVariables.pointerCallSystemWithOutput = &CallSystemWrapperReturnStandardOutput;
  theGlobalVariables.pointerCallChDir = &CallChDirWrapper;
  //stOutput << "address of get elapsed seconds: " << (int) &GetElapsedTimeInSeconds;
  //Change the below line to modify the computation time of the calculator.
  consoleFormat.flagUseHTML = false;
  consoleFormat.flagUseLatex = false;
}

void HtmlRoutines::MakeReportIndicatorFile(const std::string& input) {
  //calling stOutput forbidden! stOutput itself calls HtmlRoutines::MakeReportIndicatorFile.
  static int counter = - 1;
  counter ++;
  //  if (counter%10!= 0)
  //    return;
  //std::cout << "Making report " << counter << " in file " << theGlobalVariables.PhysicalNameIndicatorWithPath << "<br>";
  std::fstream theFile;
  FileOperations::OpenFileCreateIfNotPresentVirtual(
    theFile, "output/" + theGlobalVariables.RelativePhysicalNameProgressReport, false, true, false
  );
  theFile << " Elapsed calculator time: " << theGlobalVariables.GetElapsedSeconds() << " second(s).";
  theFile << input;
  theFile.flush();
  theFile.close();
}

void HtmlRoutines::MakeStdCoutReport(const std::string& input) {
  stOutput << input;
  HtmlRoutines::MakeReportIndicatorFile(input);
}


