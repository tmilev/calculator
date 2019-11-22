//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#include <chrono>
#include <sys/time.h>
#include <unistd.h>
//#define cgiLimitRAMuseNumPointersInList

#include "system_functions_global_objects.h"
#include "general_logging_global_variables.h"

extern ProjectInformationInstance projectInfoInstanceCalculatorSystem;
ProjectInformationInstance projectInfoInstanceCalculatorSystem(__FILE__, "System functions, platform dependent code.");


static timeval ComputationStartGlobal, LastMeasureOfCurrentTime;

int64_t GlobalVariables::GetElapsedMilliseconds() {
  gettimeofday(&LastMeasureOfCurrentTime, nullptr);
  return (LastMeasureOfCurrentTime.tv_sec - ComputationStartGlobal.tv_sec) * 1000 +
  (LastMeasureOfCurrentTime.tv_usec - ComputationStartGlobal.tv_usec) / 1000 - theGlobalVariables.millisecondOffset;
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
  if (theGlobalVariables.flagComputationStarted) {
    if (theGlobalVariables.millisecondsComputationStart < 0) {
      theGlobalVariables.millisecondsComputationStart = theGlobalVariables.GetElapsedMilliseconds();
    }
  }
  this->elapsedTimeInMilliseconds = theGlobalVariables.GetElapsedMilliseconds();
  if (theGlobalVariables.millisecondsComputationStart > 0) {
    this->elapsedComputationTimeInMilliseconds = this->elapsedTimeInMilliseconds - theGlobalVariables.millisecondsComputationStart;
  }
  return false;
}

bool TimeoutThread::HandleComputationCompleteStandard() {
  //if (theGlobalVariables.flagComputationCompletE)
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
  if (theGlobalVariables.millisecondsMaxComputation <= 0) {
    return false;
  }
  if (this->elapsedComputationTimeInMilliseconds <= 0) {
    return false;
  }
  if (this->elapsedComputationTimeInMilliseconds <= theGlobalVariables.millisecondsMaxComputation) {
    return false;
  }
  if (theGlobalVariables.flagComputationCompletE) {
    return false;
  }
  //theReport2.SetStatus("Starting timer cycle break: computation time too long.");
  if (!theGlobalVariables.flagComputationStarted) {
    crash << "Something has gone wrong. Computation has not started, yet " << this->elapsedTimeInMilliseconds
    << " ms have already passed."
    << " This may be an error in the web-server routines of the calculator. " << crash;
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
  << theGlobalVariables. millisecondsMaxComputation
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
  << "theGlobalVariables.MaxComputationTimeMillisecondsNonPositiveForNoLimit. "
  << "<br>This may be a lot of work but will allow you to alter "
  << "time limits dynamically. You will need to modify file: "
  << __FILE__ << "<br>";
  crash << out.str() << crash;
  return true;
}

bool TimeoutThread::HandleComputationTimeout() {
  if (!theGlobalVariables.flagRunningBuiltInWebServer) {
    return false;
  }
  if (theGlobalVariables.millisecondsReplyAfterComputation <= 0) {
    return false;
  }
  if (this->elapsedComputationTimeInMilliseconds <= 0) {
    return false;
  }
  if (this->elapsedComputationTimeInMilliseconds <= theGlobalVariables.millisecondsReplyAfterComputation) {
    return false;
  }
  if (!theGlobalVariables.theProgress.ReportAlloweD()) {
    return false;
  }
  MacroRegisterFunctionWithName("TimerThreadData::HandleComputationTimeout");
  if (theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection == nullptr) {
    return false;
  }
  if (theGlobalVariables.theProgress.flagTimedOut) {
    return false;
  }
  theGlobalVariables.theProgress.flagTimedOut = true;
  // theReport2.SetStatus("Starting timer cycle displaying time out explanation.");
  // Possible values for theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection:
  // 1) nullptr
  // 2) WebServer::OutputShowIndicatorOnTimeoutStatic
  theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection("Triggered by timer thread.");
  //theReport2.SetStatus("Starting timer cycle displaying time out indicator done, continuing timer cycle.");
  return false;
}

bool TimeoutThread::HandleEverythingIsDone() {
  return theGlobalVariables.flagComputationFinishedAllOutputSentClosing;
}

bool TimeoutThread::HandlePingServerIamAlive() {
  if (theGlobalVariables.flagComputationFinishedAllOutputSentClosing) {
    return true;
  }
  if (theGlobalVariables.WebServerTimerPing == nullptr) {
    return false;
  }
  theGlobalVariables.WebServerTimerPing(this->elapsedTimeInMilliseconds);
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
    this->HandleComputationTimer();
    theGlobalVariables.FallAsleep(this->intervalBetweenChecksInMilliseconds);
    this->HandleComputationCompleteStandard();
    this->HandleTimerSignalToServer();
    this->HandleMaxComputationTime();
    this->HandleComputationTimeout();
    this->HandlePingServerIamAlive();
    if (this->HandleEverythingIsDone()) {
      break;
    }
  }
}

void RunTimerThread(int threadIndex) {
  theGlobalVariables.theThreadData[threadIndex].theId = std::this_thread::get_id();
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
    logWorker << logger::red << "Chdir command to directory: " << theDir << " exited with " << systemOutput
    << ". " << logger::endL;
  }
}
