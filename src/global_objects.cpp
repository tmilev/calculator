#include "system_functions_global_objects.h"
#include "webserver.h"
#include "calculator_interface.h"
#include "general_logging_global_variables.h"
#include "general_time_date.h"
#include "general_file_operations_encodings.h"

GlobalVariables global;

std::string GlobalVariables::getDateForLogFiles() {
  static std::string date;
  if (date != "") {
    return date;
  }
  TimeWrapper now;
  now.assignLocalTime();
  std::stringstream out;
  out << "GM_" << now.toStringGM();
  date = out.str();
  for (unsigned i = 0; i < date.size(); i ++) {
    if (date[i] == ' ') {
      date[i] = '_';
    }
  }
  return date;
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
  global.logs.logType = GlobalVariables::LogData::type::initialization;
  global.flagIsChildProcess = false;
  initializeTimer();
  global.pointerExternalCommandNoOutput = &externalCommandNoOutput;
  global.pointerExternalCommandReturnOutput =
  &externalCommandReturnStandardOut;
  global.pointerExternalCommandStream = &externalCommandStreamOutput;
  global.pointerCallChDir = &callChDirWrapper;
}

void HtmlRoutines::makeReportIndicatorFile(const std::string& input) {
  std::fstream file;
  FileOperations::openFileCreateIfNotPresentVirtual(
    file, "result/output.html", false, true, false
  );
  file
  << " Elapsed calculator time: "
  << global.getElapsedSeconds()
  << " second(s).";
  file << input;
  file.flush();
  file.close();
}

void HtmlRoutines::makeStdCoutReport(const std::string& input) {
  global.comments << input;
  HtmlRoutines::makeReportIndicatorFile(input);
}
