#ifndef header_progress_report_ALREADY_INCLUDED
#define header_progress_report_ALREADY_INCLUDED

#include "general_multitasking.h"

// A class that prepares a progress report of a computation.
// The progress report is end-user visible.
// The progress report is stored by calling the report function.
// Each progress report overwrites the previous report done by the same
// ProgressReport object.
// ProgressReports done by different objects are independent of one another
// and are pushed in the order of allocation onto a global stack.
// When an end user polls the progress report, the end user receives
// all current progress reports on the stack, in the order of
// ProgressReport allocation.
// ProgressReports that were allocated earlier
// can make reports without changes in subsequent ProgressReports on the stack.
// The end user application is then expected to poll
// the stack of reported strings.
// The poll may be less more more frequent than the
// actual frequency of report generations.
class ProgressReport {
private:
  void initialize();
public:
  int currentLevel;
  int threadIndex;
  // How many times the report was needed.
  int ticks;
  // An optional throttle used by tickAndWantReport().
  // If ticksPerReport is larger than the ticks, then tickAndWantReport
  // will return false.
  // If that happens, then the user of the report
  // should abstain from generating a report,
  // unless there's a valid reason for an exception.
  // See the comments before the report() function.
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
  ProgressReport(
    int inputTicksPerReport = 1, const std::string& inputName = ""
  ) {
    this->name = inputName;
    this->ticksPerReport = inputTicksPerReport;
    this->initialize();
  }
  ~ProgressReport();
};

#endif
