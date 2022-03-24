 // The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include <chrono>
#include <sys/time.h>
#include <unistd.h>
#include <memory>

// #define cgiLimitRAMuseNumPointersInList
#include "system_functions_global_objects.h"
#include "general_logging_global_variables.h"

class SytemFunctionsGlobal {
public:
  static timeval computationStartGlobal;
  static timeval lastTimeMeasure;
};

timeval SytemFunctionsGlobal::computationStartGlobal;
timeval SytemFunctionsGlobal::lastTimeMeasure;
int64_t GlobalVariables::getElapsedMilliseconds() {
  gettimeofday(&SytemFunctionsGlobal::lastTimeMeasure, nullptr);
  return (
    SytemFunctionsGlobal::lastTimeMeasure.tv_sec -
    SytemFunctionsGlobal::computationStartGlobal.tv_sec
  ) *
  1000 + (
    SytemFunctionsGlobal::lastTimeMeasure.tv_usec -
    SytemFunctionsGlobal::computationStartGlobal.tv_usec
  ) /
  1000 - global.millisecondOffset;
}

void initializeTimer() {
  gettimeofday(&SytemFunctionsGlobal::computationStartGlobal, nullptr);
}

int GlobalVariables::getGlobalTimeInSeconds() {
  time_t result = time(nullptr);
  return static_cast<int>(result);
}

void GlobalVariables::fallAsleep(int microseconds) {
  usleep(static_cast<useconds_t>(microseconds));
}

class TimeoutThread {
public:
  int64_t elapsedTimeInMilliseconds;
  int64_t elapsedComputationTimeInMilliseconds;
  int counter;
  int intervalBetweenChecksInMilliseconds;
  TimeoutThread();
  void reset();
  void run();
  bool HandleComputationTimer();
  bool HandleMaxComputationTime();
  bool HandleEverythingIsDone();
  bool HandleComputationTimeout();
};

bool TimeoutThread::HandleComputationTimer() {
  if (global.flagComputationStarted) {
    if (global.millisecondsComputationStart < 0) {
      global.millisecondsComputationStart = global.getElapsedMilliseconds();
    }
  }
  this->elapsedTimeInMilliseconds = global.getElapsedMilliseconds();
  if (global.millisecondsComputationStart > 0) {
    this->elapsedComputationTimeInMilliseconds =
    this->elapsedTimeInMilliseconds - global.millisecondsComputationStart;
  }
  return false;
}

bool TimeoutThread::HandleMaxComputationTime() {
  if (global.millisecondsMaxComputation <= 0) {
    return false;
  }
  if (this->elapsedComputationTimeInMilliseconds <= 0) {
    return false;
  }
  if (
    this->elapsedComputationTimeInMilliseconds <=
    global.millisecondsMaxComputation
  ) {
    return false;
  }
  if (global.flagComputationComplete) {
    return false;
  }
  if (!global.flagComputationStarted) {
    global.fatal
    << "Something has gone wrong. Computation has not started, yet "
    << this->elapsedTimeInMilliseconds
    << " ms have already passed."
    << " This may be an error in the web-server routines of the calculator. "
    << global.fatal;
  }
  std::stringstream out;
  out
  << "<b>This is a safety time-out crash. "
  << "You may have requested a computation that takes too long."
  << "</b> Your computation ran for ";
  if (this->elapsedComputationTimeInMilliseconds > 0) {
    out << this->elapsedComputationTimeInMilliseconds << " ms";
  } else {
    out << " (unknown amount of time)";
  }
  out
  << ". The allowed run time is "
  << global.millisecondsMaxComputation
  << " ms (twice the amount allowed for calculator interpretation). "
  << "<br>This restriction may be lifted "
  << "by restarting the server "
  << "with a larger timeout limit (requires administrator access via ssh). "
  << "For example, the command:<br> "
  << "calculator server 1000<br>"
  << "will restart the server with a timeout of 1000 seconds = 1000000 ms, "
  << "and the command:<br>"
  << "calculator server 0"
  << "<br>will restart the server without any timeout limits whatsoever. "
  << "<br>Please note that running the server without time limits "
  << "is intended for private use only "
  << "(on computers not facing the internet). "
  << "<br>Finally, you may modify the timeout restrictions "
  << "by modifying the source code directly, increasing the variable: "
  << "global.MaxComputationTimeMillisecondsNonPositiveForNoLimit. "
  << "<br>This may be a lot of work but will allow you to alter "
  << "time limits dynamically. You will need to modify file: "
  << __FILE__
  << "<br>";
  global.fatal << out.str() << global.fatal;
  return true;
}

bool TimeoutThread::HandleComputationTimeout() {
  STACK_TRACE("TimerThreadData::HandleComputationTimeout");
  if (!global.flagRunningBuiltInWebServer) {
    return false;
  }
  if (global.millisecondsReplyAfterComputation <= 0) {
    return false;
  }
  if (this->elapsedComputationTimeInMilliseconds <= 0) {
    return false;
  }
  if (
    this->elapsedComputationTimeInMilliseconds <=
    global.millisecondsReplyAfterComputation
  ) {
    return false;
  }
  global.response.initiate("Triggered by timer thread.");
  return false;
}

bool TimeoutThread::HandleEverythingIsDone() {
  return global.flagComputationFinishedAllOutputSentClosing;
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

void TimeoutThread::run() {
  STACK_TRACE("TimerThreadData::run");
  this->reset();
  for (;;) {
    this->counter ++;
    global.checkConsistency();
    this->HandleComputationTimer();
    global.fallAsleep(this->intervalBetweenChecksInMilliseconds);
    this->HandleComputationTimeout();
    // Will crash if max computation time is exceeded.
    // This loop will never be exited;
    // the program will terminate wit
    this->HandleMaxComputationTime();
    // Only standard exit of the loop.
    if (this->HandleEverythingIsDone()) {
      break;
    }
  }
  global.checkConsistency();
}

void ThreadData::runTimerThread(int threadIndex) {
  global.threadData[threadIndex].id = std::this_thread::get_id();
  STACK_TRACE("RunTimerThread");
  TimeoutThread timeoutThread;
  timeoutThread.run();
}

void createTimerThread() {
  ThreadData::createThread(ThreadData::runTimerThread, "timer");
}

int externalCommandNoOutput(const std::string& command) {
  return system(command.c_str());
}

#ifndef MACRO_use_wasm

std::string externalCommandReturnStandardOut(
  const std::string& inputCommand
) {
  std::string inputCommandWithRedirection = inputCommand + " 2>&1";
  std::shared_ptr<FILE> pipe(
    popen(inputCommandWithRedirection.c_str(), "r"), pclose
  );
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

int externalCommandStreamOutput(const std::string& inputCommand) {
  std::string inputCommandWithRedirection = inputCommand + " 2>&1";
  std::shared_ptr<FILE> reader(
    popen(inputCommandWithRedirection.c_str(), "r"), pclose
  );
  if (!reader) {
    global << Logger::red << "Failed to create pipe. " << Logger::endL;
    return - 1;
  }
  const int bufferSize = 20000;
  char buffer[bufferSize];
  while (!feof(reader.get())) {
    char* incoming = fgets(buffer, bufferSize, reader.get());
    if (incoming != nullptr) {
      std::string result(buffer);
      std::cout << result;
    }
  }
  int result = pclose(reader.get());
  if (result != 0) {
    global << Logger::red << strerror(errno) << Logger::endL;
  }
  return result;
}

#else

std::string externalCommandReturnStandardOut(const std::string& command) {
  global.fatal
  << "In externalCommandReturnStandardOut(...): external commands such as: "
  << command
  << " are not allowed. "
  << global.fatal;
  return "";
}

int externalCommandStreamOutput(const std::string& command) {
  global.fatal
  << "In externalCommandStreamOutput(...): external commands such as: "
  << command
  << " are not allowed. "
  << global.fatal;
  return - 1;
}

#endif

void callChDirWrapper(const std::string& command) {
  int systemOutput = chdir(command.c_str());
  if (systemOutput != 0) {
    global
    << Logger::red
    << "Chdir command to directory: "
    << command
    << " exited with "
    << systemOutput
    << ". "
    << Logger::endL;
  }
}
