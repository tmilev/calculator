//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include <chrono>
#include <sys/time.h>
#include <unistd.h>
//#define cgiLimitRAMuseNumPointersInList

#include "vpfHeader4SystemFunctionsGlobalObjects.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"
ProjectInformationInstance projectInfoInstanceCalculatorSystem(__FILE__, "System functions, platform dependent code.");


timeval ComputationStartGlobal, LastMeasureOfCurrentTime;

int64_t GlobalVariables::GetElapsedMilliseconds() {
  gettimeofday(&LastMeasureOfCurrentTime, NULL);
  return (LastMeasureOfCurrentTime.tv_sec - ComputationStartGlobal.tv_sec) * 1000 +
    (LastMeasureOfCurrentTime.tv_usec - ComputationStartGlobal.tv_usec) / 1000 - theGlobalVariables.millisecondOffset;
}

void InitializeTimeR() {
  gettimeofday(&ComputationStartGlobal, NULL);
}

int GlobalVariables::GetGlobalTimeInSeconds() {
  time_t result = time(0);
  return (int) result;
}

void GlobalVariables::FallAsleep(int microseconds) {
  usleep(microseconds);
}

struct TimerThreadData{
  int elapsedTimeInMilliseconds;
  int elapsedComputationTimeInMilliseconds;
  int computationStartTimeInMilliseconds;
  int counter ;
  int intervalBetweenChecksInMilliseconds;
//  ThreadWrapper theThread;
  TimerThreadData();
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

bool TimerThreadData::HandleComputationTimer() {
  if (theGlobalVariables.flagComputationStarted)
    if (this->computationStartTimeInMilliseconds < 0)
      this->computationStartTimeInMilliseconds = theGlobalVariables.GetElapsedMilliseconds();
  this->elapsedTimeInMilliseconds = theGlobalVariables.GetElapsedMilliseconds();
  if (this->computationStartTimeInMilliseconds > 0)
    this->elapsedComputationTimeInMilliseconds = this->elapsedTimeInMilliseconds - this->computationStartTimeInMilliseconds;
  return false;
}

bool TimerThreadData::HandleComputationCompleteStandard() {
  //if (theGlobalVariables.flagComputationCompletE)
  //  theReport1.SetStatus("Starting timer cycle break: computation is complete.");
  return false;
}

bool TimerThreadData::HandleTimerSignalToServer() {
  if (this->counter % 20 != 0)
    return false;
//  std::stringstream reportStream;
//  reportStream << "Timer: " << this->elapsedtime << " seconds elapsed since the timer was launched.";
//  theReport1.SetStatus(reportStream.str());
  return false;
}

bool TimerThreadData::HandleMaxComputationTime() {
  if (theGlobalVariables.MaxComputationMilliseconds <= 0)
    return false;
  if (this->elapsedComputationTimeInMilliseconds <= 0)
    return false;
  if (this->elapsedComputationTimeInMilliseconds <= theGlobalVariables.MaxComputationMilliseconds)
    return false;
  if (theGlobalVariables.flagComputationCompletE) {
    //theReport2.SetStatus((std::string)"The allowed clock time has ran out, but it seems the computation is already done. "+
    //                     (std::string)"Continuing to run (in order to wrap it up)...");
    return false;
  }
  //theReport2.SetStatus("Starting timer cycle break: computation time too long.");
  if (!theGlobalVariables.flagComputationStarted)
    crash << "Something has gone wrong. Computation has not started, yet " << this->elapsedTimeInMilliseconds
    << " ms have already passed."
    << " This may be an error in the web-server routines of the calculator. " << crash;
  std::stringstream out;
  out << "<b>This is a safety time-out crash. You may have requested a computation that takes too long."
  << "</b> Your computation ran for ";
  if (this->elapsedComputationTimeInMilliseconds > 0)
    out << this->elapsedComputationTimeInMilliseconds << " ms";
  else
    out << " (unknown amount of time)";
  out << ". The allowed run time is "
  << theGlobalVariables.MaxComputationMilliseconds
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
  << "<br>This may be a lot of work but will allow you to alter"
  << " time limits dynamically. "
  << __FILE__ << "<br>";
  crash << out.str() << crash;
  return true;
}

bool TimerThreadData::HandleComputationTimeout() {
  if (!theGlobalVariables.flagRunningBuiltInWebServer)
    return false;
  if (theGlobalVariables.replyAfterComputationMilliseconds <= 0)
    return false;
  if (this->elapsedComputationTimeInMilliseconds <= 0)
    return false;
  if (this->elapsedComputationTimeInMilliseconds <= theGlobalVariables.replyAfterComputationMilliseconds)
    return false;
  MacroRegisterFunctionWithName("TimerThreadData::HandleComputationTimeout");
  if (theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection == 0)
    return false;
  if (theGlobalVariables.flagOutputTimedOut)
    return false;
  theGlobalVariables.flagTimeOutExplanationAlreadyDisplayed = true;
  //theReport2.SetStatus("Starting timer cycle displaying time out explanation.");
  theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection();
  //theReport2.SetStatus("Starting timer cycle displaying time out indicator done, continuing timer cycle.");
  return false;
}

bool TimerThreadData::HandleEverythingIsDone() {
  return theGlobalVariables.flagComputationFinishedAllOutputSentClosing;
}

bool TimerThreadData::HandlePingServerIamAlive() {
  if (theGlobalVariables.flagComputationFinishedAllOutputSentClosing)
    return true;
  if (theGlobalVariables.WebServerTimerPing == 0)
    return false;
  theGlobalVariables.WebServerTimerPing(this->elapsedTimeInMilliseconds);
  return false;
}

TimerThreadData::TimerThreadData() {
  this->reset();
}

void TimerThreadData::reset() {
  this->elapsedTimeInMilliseconds = - 1;
  this->elapsedComputationTimeInMilliseconds = - 1;
  this->computationStartTimeInMilliseconds = - 1;
  this->counter = 0;
  this->intervalBetweenChecksInMilliseconds = 100000;
}

void TimerThreadData::Run() {
  MacroRegisterFunctionWithName("TimerThreadData::Run");
  this->reset();
//  ProgressReport theReport;
//  std::cout << "Got thus far RunTimerVoidPtr - 2" << std::endl;
  for (;;) {
    this->counter ++;
    this->HandleComputationTimer();
    theGlobalVariables.FallAsleep(this->intervalBetweenChecksInMilliseconds);
    this->HandleComputationCompleteStandard();
    this->HandleTimerSignalToServer();
    this->HandleMaxComputationTime();
    this->HandleComputationTimeout();
    this->HandlePingServerIamAlive();
    if (this->HandleEverythingIsDone())
      break;
  }
}

void RunTimerThread(int threadIndex) {
  theGlobalVariables.theThreadData[threadIndex].theId = std::this_thread::get_id();
  MacroRegisterFunctionWithName("RunTimerThread");
//  std::cout << "Got thus far RunTimerVoidPtr" << std::endl;
  TimerThreadData theThread;
  theThread.Run();
}

void CreateTimerThread() {
  ThreadData::CreateThread(RunTimerThread, "timer");
}

void CallSystemWrapperNoOutput(const std::string& theCommand, bool ignoreNonZeroReturn) {
  int systemOutput = system(theCommand.c_str());
  if (systemOutput != 0 && !ignoreNonZeroReturn)
    logWorker << logger::red << "System command: " << theCommand << " exited with " << systemOutput << ". " << logger::endL;
}

std::string CallSystemWrapperReturnStandardOutput(const std::string& inputCommand) {
  std::string inputCommandWithRedirection = inputCommand + " 2>&1";
  std::shared_ptr<FILE> pipe(popen(inputCommandWithRedirection.c_str(), "r"), pclose);
  if (!pipe)
    return "ERROR";
  const int bufferSize = 20000;
  char buffer[bufferSize];
  std::string result = "";
  while (!feof(pipe.get()))
    if (fgets(buffer, bufferSize, pipe.get()) != 0)
      result += buffer;
  return result;
}

void CallChDirWrapper(const std::string& theDir) {
  int systemOutput = chdir(theDir.c_str());
  if (systemOutput != 0)
    logWorker << logger::red << "Chdir command to directory: " << theDir << " exited with " << systemOutput
    << ". " << logger::endL;
}
