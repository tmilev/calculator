// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "system_functions_global_objects.h"
#include "webserver.h"
#include "calculator_interface.h"
#include "general_logging_global_variables.h"
#include "general_time_date.h"

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

void InitializeGlobalObjects() {
  global.logs.logType = GlobalVariables::LogData::type::server;
  global.flagIsChildProcess = false;
  InitializeTimeR();
  global.pointerExternalCommandNoOutput = &externalCommandNoOutput;
  global.pointerExternalCommandReturnOutput = &externalCommandReturnStandartOut;
  global.pointerExternalCommandStream = &externalCommandStreamOutput;
  global.pointerCallChDir = &CallChDirWrapper;
}

void HtmlRoutines::MakeReportIndicatorFile(const std::string& input) {
  static int counter = - 1;
  counter ++;
  std::fstream theFile;
  FileOperations::openFileCreateIfNotPresentVirtual(
    theFile, "result/output.html", false, true, false
  );
  theFile << " Elapsed calculator time: " << global.getElapsedSeconds() << " second(s).";
  theFile << input;
  theFile.flush();
  theFile.close();
}

void HtmlRoutines::MakeStdCoutReport(const std::string& input) {
  global.comments << input;
  HtmlRoutines::MakeReportIndicatorFile(input);
}


