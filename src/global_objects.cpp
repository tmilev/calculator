// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "system_functions_global_objects.h"
#include "webserver.h"
#include "calculator_interface.h"
#include "general_logging_global_variables.h"
#include "general_time_date.h"

GlobalVariables global;

std::string GlobalVariables::getDateForLogFiles() {
  static std::string tempDate;
  if (tempDate != "") {
    return tempDate;
  }
  TimeWrapper now;
  now.assignLocalTime();
  std::stringstream out;
  out << "GM_" << now.toStringGM();
  tempDate = out.str();
  for (unsigned i = 0; i < tempDate.size(); i ++) {
    if (tempDate[i] == ' ') {
      tempDate[i] = '_';
    }
  }
  return tempDate;
}

std::string GlobalVariables::getTimeGM() {
  TimeWrapper now;
  now.assignLocalTime();
  return now.toStringGM();
}

std::string GlobalVariables::getTimeLocal() {
  TimeWrapper now;
  now.assignLocalTime();
  return now.toStringLocal();
}

void initializeGlobalObjects() {
  global.logs.logType = GlobalVariables::LogData::type::server;
  global.flagIsChildProcess = false;
  initializeTimer();
  global.pointerExternalCommandNoOutput = &externalCommandNoOutput;
  global.pointerExternalCommandReturnOutput = &externalCommandReturnStandartOut;
  global.pointerExternalCommandStream = &externalCommandStreamOutput;
  global.pointerCallChDir = &callChDirWrapper;
}

void HtmlRoutines::makeReportIndicatorFile(const std::string& input) {
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

void HtmlRoutines::makeStdCoutReport(const std::string& input) {
  global.comments << input;
  HtmlRoutines::makeReportIndicatorFile(input);
}


