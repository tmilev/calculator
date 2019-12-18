// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "system_functions_global_objects.h"
#include "webserver.h"
#include "calculator_interface.h"
#include "general_logging_global_variables.h"
#include "general_time_date.h"

static ProjectInformationInstance projectInfoCalculatorGlobal(__FILE__, "Global objects");

GlobalVariables global;
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

logger logWorker("/LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/logCommon.html", nullptr, true, ProcessTypes::worker);
logger logServerMonitor("/LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/logServerMonitor.html", nullptr, false, ProcessTypes::serverMonitor);
logger logServer("/LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/logServer.html", nullptr, false, ProcessTypes::server);


logger logProcessStats("/LogFiles/" + GlobalVariables::GetDateForLogFiles() + "/LogWorkerProcessStats.html", &logServer, false, ProcessTypes::server);

Calculator* theParser = nullptr;
FormatExpressions consoleFormat;
Crasher crash;

void InitializeGlobalObjects() {
  global.processType = ProcessTypes::server;
  global.flagIsChildProcess = false;
  InitializeTimeR();
  global.IndicatorStringOutputFunction = nullptr;
  global.pointerCallSystemNoOutput = &CallSystemWrapperNoOutput;
  global.pointerCallSystemWithOutput = &CallSystemWrapperReturnStandardOutput;
  global.pointerCallChDir = &CallChDirWrapper;
  consoleFormat.flagUseHTML = false;
  consoleFormat.flagUseLatex = false;
}

void HtmlRoutines::MakeReportIndicatorFile(const std::string& input) {
  static int counter = - 1;
  counter ++;
  std::fstream theFile;
  FileOperations::OpenFileCreateIfNotPresentVirtual(
    theFile, "result/output.html", false, true, false
  );
  theFile << " Elapsed calculator time: " << global.GetElapsedSeconds() << " second(s).";
  theFile << input;
  theFile.flush();
  theFile.close();
}

void HtmlRoutines::MakeStdCoutReport(const std::string& input) {
  global.Comments << input;
  HtmlRoutines::MakeReportIndicatorFile(input);
}


