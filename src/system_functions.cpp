// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include <chrono>
#include <sys/time.h>
#include <unistd.h>
//#define cgiLimitRAMuseNumPointersInList

#include "system_functions_global_objects.h"
#include "general_logging_global_variables.h"

static ProjectInformationInstance projectInfoInstanceCalculatorSystem(__FILE__, "System functions, platform dependent code.");


static timeval ComputationStartGlobal, LastMeasureOfCurrentTime;

int64_t GlobalVariables::GetElapsedMilliseconds() {
  gettimeofday(&LastMeasureOfCurrentTime, nullptr);
  return (LastMeasureOfCurrentTime.tv_sec - ComputationStartGlobal.tv_sec) * 1000 +
  (LastMeasureOfCurrentTime.tv_usec - ComputationStartGlobal.tv_usec) / 1000 - global.millisecondOffset;
}

void InitializeTimeR() {
  gettimeofday(&ComputationStartGlobal, nullptr);
}

int GlobalVariables::GetGlobalTimeInSeconds() {
  time_t result = time(nullptr);
  return static_cast<int>(result);
}

void GlobalVariables::FallAsleep(int microseconds) {
  usleep(static_cast<useconds_t>(microseconds));
}

class TimeoutThread {
public:
  int64_t elapsedTimeInMilliseconds;
  int64_t elapsedComputationTimeInMilliseconds;
  int counter ;
  int intervalBetweenChecksInMilliseconds;
//  ThreadWrapper theThread;
  TimeoutThread();
  void reset();
  void Run();
  bool HandleComputationTimer();
  bool HandleComputationCompleteStandard();
  bool HandleTimerSignalToServer();
  bool HandleMaxComputationTime();
  bool HandleEverythingIsDone();
  bool HandlePingServerIamAlive();
  bool HandleComputationTimeout();
};

bool TimeoutThread::HandleComputationTimer() {
  if (global.flagComputationStarted) {
    if (global.millisecondsComputationStart < 0) {
      global.millisecondsComputationStart = global.GetElapsedMilliseconds();
    }
  }
  this->elapsedTimeInMilliseconds = global.GetElapsedMilliseconds();
  if (global.millisecondsComputationStart > 0) {
    this->elapsedComputationTimeInMilliseconds = this->elapsedTimeInMilliseconds - global.millisecondsComputationStart;
  }
  return false;
}

bool TimeoutThread::HandleComputationCompleteStandard() {
  //if (global.flagComputationCompletE)
  //  theReport1.SetStatus("Starting timer cycle break: computation is complete.");
  return false;
}

bool TimeoutThread::HandleTimerSignalToServer() {
  if (this->counter % 20 != 0) {
    return false;
  }
//  std::stringstream reportStream;
//  reportStream << "Timer: " << this->elapsedtime << " seconds elapsed since the timer was launched.";
//  theReport1.SetStatus(reportStream.str());
  return false;
}

bool TimeoutThread::HandleMaxComputationTime() {
  if (global.millisecondsMaxComputation <= 0) {
    return false;
  }
  if (this->elapsedComputationTimeInMilliseconds <= 0) {
    return false;
  }
  if (this->elapsedComputationTimeInMilliseconds <= global.millisecondsMaxComputation) {
    return false;
  }
  if (global.flagComputationCompletE) {
    return false;
  }
  //theReport2.SetStatus("Starting timer cycle break: computation time too long.");
  if (!global.flagComputationStarted) {
    global.fatal << "Something has gone wrong. Computation has not started, yet " << this->elapsedTimeInMilliseconds
    << " ms have already passed."
    << " This may be an error in the web-server routines of the calculator. " << global.fatal;
  }
  std::stringstream out;
  out << "<b>This is a safety time-out crash. You may have requested a computation that takes too long."
  << "</b> Your computation ran for ";
  if (this->elapsedComputationTimeInMilliseconds > 0) {
    out << this->elapsedComputationTimeInMilliseconds << " ms";
  } else {
    out << " (unknown amount of time)";
  }
  out << ". The allowed run time is "
  << global. millisecondsMaxComputation
  << " ms (twice the amount allowed for calculator interpretation). "
  << "<br>This restriction may be lifted "
  << "by restarting the server "
  << "with a larger timeout limit (requires admin access via ssh). "
  << "For example, the command:<br> "
  << "calculator/bin/calculator server 1000<br>"
  << "will restart the server with a timeout of 1000 seconds = 1000000 ms, "
  << "and the command:<br>"
  << "calculator/bin/calculator server 0"
  << "<br>will restart the server without any timeout limits whatsoever. "
  << "<br>Please note that running the server without time limits "
  << "is intended for private use only "
  << "(on computers not facing the internet). "
  << "<br>Finally, you may modify the timeout restrictions "
  << "by modifying the source code directly, increasing the variable: "
  << "global.MaxComputationTimeMillisecondsNonPositiveForNoLimit. "
  << "<br>This may be a lot of work but will allow you to alter "
  << "time limits dynamically. You will need to modify file: "
  << __FILE__ << "<br>";
  global.fatal << out.str() << global.fatal;
  return true;
}

bool TimeoutThread::HandleComputationTimeout() {
  if (!global.flagRunningBuiltInWebServer) {
    return false;
  }
  if (global.millisecondsReplyAfterComputation <= 0) {
    return false;
  }
  if (this->elapsedComputationTimeInMilliseconds <= 0) {
    return false;
  }
  if (this->elapsedComputationTimeInMilliseconds <= global.millisecondsReplyAfterComputation) {
    return false;
  }
  if (!global.theProgress.ReportAlloweD()) {
    return false;
  }
  MacroRegisterFunctionWithName("TimerThreadData::HandleComputationTimeout");
  if (global.WebServerReturnDisplayIndicatorCloseConnection == nullptr) {
    return false;
  }
  if (global.theProgress.flagTimedOut) {
    return false;
  }
  global.theProgress.flagTimedOut = true;
  // theReport2.SetStatus("Starting timer cycle displaying time out explanation.");
  // Possible values for global.WebServerReturnDisplayIndicatorCloseConnection:
  // 1) nullptr
  // 2) WebServer::OutputShowIndicatorOnTimeoutStatic
  global.WebServerReturnDisplayIndicatorCloseConnection("Triggered by timer thread.");
  //theReport2.SetStatus("Starting timer cycle displaying time out indicator done, continuing timer cycle.");
  return false;
}

bool TimeoutThread::HandleEverythingIsDone() {
  return global.flagComputationFinishedAllOutputSentClosing;
}

bool TimeoutThread::HandlePingServerIamAlive() {
  if (global.flagComputationFinishedAllOutputSentClosing) {
    return true;
  }
  if (global.WebServerTimerPing == nullptr) {
    return false;
  }
  global.WebServerTimerPing(this->elapsedTimeInMilliseconds);
  return false;
}

TimeoutThread::TimeoutThread() {
  this->reset();
}

void TimeoutThread::reset() {
  this->elapsedTimeInMilliseconds = - 1;
  this->elapsedComputationTimeInMilliseconds = - 1;
  this->counter = 0;
  this->intervalBetweenChecksInMilliseconds = 100000;
}

void TimeoutThread::Run() {
  MacroRegisterFunctionWithName("TimerThreadData::Run");
  this->reset();
  for (;;) {
    this->counter ++;
    global.CheckConsistency();
    this->HandleComputationTimer();
    global.FallAsleep(this->intervalBetweenChecksInMilliseconds);
    // global.CheckConsistency();
    this->HandleComputationCompleteStandard();
    // global.CheckConsistency();
    this->HandleTimerSignalToServer();
    // global.CheckConsistency();
    this->HandleMaxComputationTime();
    // global.CheckConsistency();
    // std::cout << "DEBUG: before handle timeout\n";
    this->HandleComputationTimeout();
    // std::cout << "DEBUG: before handle ping\n";
    // global.CheckConsistency();
    this->HandlePingServerIamAlive();
    // std::cout << "DEBUG: before handle every thing\n";
    // global.CheckConsistency();
    if (this->HandleEverythingIsDone()) {
      break;
    }
  }
  global.CheckConsistency();
}

void RunTimerThread(int threadIndex) {
  global.theThreadData[threadIndex].theId = std::this_thread::get_id();
  MacroRegisterFunctionWithName("RunTimerThread");
  TimeoutThread theThread;
  theThread.Run();
}

void CreateTimerThread() {
  ThreadData::CreateThread(RunTimerThread, "timer");
}

int CallSystemWrapperNoOutput(const std::string& theCommand) {
  return system(theCommand.c_str());
}

std::string CallSystemWrapperReturnStandardOutput(const std::string& inputCommand) {
  std::string inputCommandWithRedirection = inputCommand + " 2>&1";
  std::shared_ptr<FILE> pipe(popen(inputCommandWithRedirection.c_str(), "r"), pclose);
  if (!pipe) {
    return "ERROR";
  }
  const int bufferSize = 20000;
  char buffer[bufferSize];
  std::string result = "";
  while (!feof(pipe.get())) {
    if (fgets(buffer, bufferSize, pipe.get()) != nullptr) {
      result += buffer;
    }
  }
  return result;
}

void CallChDirWrapper(const std::string& theDir) {
  int systemOutput = chdir(theDir.c_str());
  if (systemOutput != 0) {
    global << logger::red << "Chdir command to directory: " << theDir << " exited with " << systemOutput
    << ". " << logger::endL;
  }
}
