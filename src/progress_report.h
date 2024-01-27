#ifndef header_progress_report_ALREADY_INCLUDED
#define header_progress_report_ALREADY_INCLUDED

#include "globals.h"
#include "general_multitasking.h"

class ProgressReport {
private:
  void initialize();
public:
  int currentLevel;
  int threadIndex;
  int ticks;
  int ticksPerReport;
  bool flagInitialized;
  std::string name;
  static MutexRecursiveWrapper reportMutex;
  // Call tickAndWantReport before generating report.
  // If a report is not wanted, it is wise to not generate one:
  // the string operations required to generate some progress reports
  // are expected to cost more than the mathematical computations
  // they are reporting.
  void report(const std::string& stringToReport);
  bool tickAndWantReport();
  ProgressReport(const std::string& inputName = "") {
    this->name = inputName;
    this->initialize();
  }
  ProgressReport(int inputTicksPerReport) {
    this->initialize();
    this->ticksPerReport = inputTicksPerReport;
  }
  ~ProgressReport();
};

#endif
