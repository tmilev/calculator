//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#include "math_general.h"
#include "general_logging_global_variables.h"
#include "system_functions_global_objects.h"
#include <assert.h>
#include <mutex>

extern ProjectInformationInstance vpfGeneral2Mutexes;
ProjectInformationInstance vpfGeneral2Mutexes(__FILE__, "Multitasking implementation.");

void ParallelComputing::CheckPointerCounters() {
  if (ParallelComputing::GlobalPointerCounter>::ParallelComputing::cgiLimitRAMuseNumPointersInList) {
    MutexRecursiveWrapper& tempMutex =
    theGlobalVariables.MutexParallelComputingStaticFiasco;
    tempMutex.LockMe();
    if (ParallelComputing::flagUngracefulExitInitiated) {
      tempMutex.UnlockMe();
      return;
    }
    ParallelComputing::flagUngracefulExitInitiated = true;
    tempMutex.UnlockMe();
    crash << "This may or may not be an error: the number of pointers "
    << "allocated by the program exceeded the allowed <b>limit of "
    << ParallelComputing::cgiLimitRAMuseNumPointersInList
    << ".</b>" << crash;
  }
  if (ParallelComputing::PointerCounterPeakRamUse < ParallelComputing::GlobalPointerCounter) {
    ParallelComputing::PointerCounterPeakRamUse = ParallelComputing::GlobalPointerCounter;
  }
}

void MutexRecursiveWrapper::CheckConsistency() {
  if (this->flagDeallocated) {
    logBlock << logger::red << "Use after free of mutex. "
    << crash.GetStackTraceEtcErrorMessageConsole() << logger::endL;
    assert(false);
  }
}

void MutexRecursiveWrapper::initConstructorCallOnly() {
  //  static bool allowToRun = true;
  //  while (!allowToRun)
  //  {}
  //  allowToRun = false;

  this->flagDeallocated = false;
  this->flagInitialized = false;
  this->flagUnsafeFlagForDebuggingIsLocked = false;
  this->theMutexImplementation = nullptr;
  this->lastLockerThread = - 1;
#ifdef AllocationLimitsSafeguard
ParallelComputing::GlobalPointerCounter ++;
#endif
}

bool MutexRecursiveWrapper::InitializeIfNeeded() {
  if (this->flagInitialized) {
    return true;
  }
  if (!theGlobalVariables.flagServerForkedIntoWorker) {
    return false;
  }
  this->theMutexImplementation = new std::mutex;
  this->flagInitialized = true;
  return true;
}

MutexRecursiveWrapper::~MutexRecursiveWrapper() {
  delete static_cast<std::mutex*>(this->theMutexImplementation);
  this->theMutexImplementation = nullptr;
  this->flagDeallocated = true;
  this->flagInitialized = false;
}

bool MutexRecursiveWrapper::isLockedUnsafeUseForWINguiOnly() {
 // std::cout << "checking consistency from isLockedUnsafeUseForWINguiOnly";
  this->CheckConsistency();
  return this->flagUnsafeFlagForDebuggingIsLocked;
}

void MutexRecursiveWrapper::LockMe() {
  this->CheckConsistency();
  if (!this->InitializeIfNeeded()) {
    return;
  }
  try {
    if (this->flagUnsafeFlagForDebuggingIsLocked) {
      int currentThreadId = ThreadData::getCurrentThreadId();
      if (currentThreadId == this->lastLockerThread) {
        logBlock << logger::red << "Fatal: about to self-lock: ["
        << this->mutexName << "] thread: "
        << currentThreadId
        << "." << logger::endL
        << theGlobalVariables.ToStringProgressReportConsole() << logger::endL;
      }
    }
    static_cast<std::mutex*>(this->theMutexImplementation)->lock();
  } catch (...) {
    crash << "Fatal error: mutex lock failed. " << crash;
  }
  this->flagUnsafeFlagForDebuggingIsLocked = true;
  this->lastLockerThread = ThreadData::getCurrentThreadId();
}

void MutexRecursiveWrapper::UnlockMe() {
  this->CheckConsistency();
  if (!this->InitializeIfNeeded()) {
    return;
  }
  this->flagUnsafeFlagForDebuggingIsLocked = false;
  this->lastLockerThread = - 1;
  static_cast<std::mutex*>(this->theMutexImplementation)->unlock();
}

void PauseThread::SafePointDontCallMeFromDestructors() {
  this->mutexSignalMeWhenReachingSafePoint.UnlockMe();
  this->mutexLockMeToPauseCallersOfSafePoint.LockMe();
  this->mutexSignalMeWhenReachingSafePoint.LockMe();
  this->mutexLockMeToPauseCallersOfSafePoint.UnlockMe();
}

void PauseThread::SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint() {
  this->mutexHoldMeWhenReadingOrWritingInternalFlags.LockMe();
  if (this->flagIsPausedWhileRunning) {
    this->mutexHoldMeWhenReadingOrWritingInternalFlags.UnlockMe();
    return;
  }
  this->mutexHoldMeWhenReadingOrWritingInternalFlags.UnlockMe();
  this->mutexLockMeToPauseCallersOfSafePoint.LockMe();
  this->mutexSignalMeWhenReachingSafePoint.LockMe();
  this->flagIsPausedWhileRunning = true;
  this->mutexSignalMeWhenReachingSafePoint.UnlockMe();
}

void PauseThread::UnlockSafePoint() {
  this->flagIsPausedWhileRunning = false;
  this->mutexLockMeToPauseCallersOfSafePoint.UnlockMe();
}

void PauseThread::InitComputation() {
  this->mutexSignalMeWhenReachingSafePoint.LockMe();
  this->flagIsRunning = true;
}

void PauseThread::ExitComputation() {
  this->flagIsRunning = false;
  this->mutexSignalMeWhenReachingSafePoint.UnlockMe();
}

bool& PauseThread::GetFlagIsPausedWhileRunningUnsafeUseWithMutexHoldMe() {
  return this->flagIsPausedWhileRunning;
}

bool& PauseThread::GetFlagIsRunningUnsafeUseWithMutexHoldMe() {
  return this->flagIsRunning;
}

PauseThread::PauseThread() {
  this->flagIsRunning = false;
  this->flagIsPausedWhileRunning = false;
}

bool PauseThread::IsPausedWhileRunning() const {
  return this->flagIsPausedWhileRunning;
}

ThreadData::ThreadData() {
  this->index = 0;
//  this->theId = 0;
}

ThreadData::~ThreadData() {

}

GlobalVariables::~GlobalVariables() {
  this->flagComputationFinishedAllOutputSentClosing = true;
  for (int i = 1; i < this->theThreads.size; i ++) {
    this->theThreads[i].join();
  }
}

void ThreadData::RegisterFirstThread(const std::string& inputName) {
  ThreadData::RegisterNewThread(inputName).theId = std::this_thread::get_id();
}

ThreadData& ThreadData::RegisterNewThread(const std::string& inputName) {
  ListReferences<ThreadData>& theThreadData =
  theGlobalVariables.theThreadData;
  ThreadData newThreadData;
  newThreadData.name = inputName;
  newThreadData.index = theThreadData.size;
  theThreadData.AddOnTop(newThreadData);
  theGlobalVariables.theThreads.SetSize(theThreadData.size);
  theGlobalVariables.CustomStackTrace.Reserve(2);
  theGlobalVariables.ProgressReportStringS.Reserve(2);
  theGlobalVariables.CustomStackTrace.SetSize(theThreadData.size);
  theGlobalVariables.ProgressReportStringS.SetSize(theThreadData.size);
  theGlobalVariables.CustomStackTrace.LastObject().Reserve(30);
  theGlobalVariables.ProgressReportStringS.LastObject().Reserve(30);
  return theGlobalVariables.theThreadData.LastObject();
}

void ThreadData::CreateThread(void (*InputFunction)(int), const std::string& inputName) {
  MutexLockGuard(theGlobalVariables.MutexRegisterNewThread);
  ThreadData& theData = ThreadData::RegisterNewThread(inputName);
  std::thread newThread(InputFunction, theData.index);
  theGlobalVariables.theThreads.LastObject().swap(newThread);
}

int ThreadData::getCurrentThreadId() {
  std::thread::id currentId = std::this_thread::get_id();
  ListReferences<ThreadData>& theThreadData = theGlobalVariables.theThreadData;
  for (int i = 0; i < theThreadData.size; i ++) {
    if (currentId == theThreadData[i].theId) {
      return i;
    }
  }
  return - 1;
}

std::string ThreadData::ToStringHtml() const {
  std::stringstream out;
  out << "Process type: <b>" << theGlobalVariables.processType << "</b>. ";
  if (this->getCurrentThreadId() == this->index) {
    out << "<b style = 'color:green'>Current thread</b> ";
  } else {
    out << "Thread ";
  }
  out << "<span style =\"color:red\">";
  if (this->name == "") {
    out << "(thread name not set)";
  } else {
    out << this->name;
  }
  out << "</span>.";
  out << " Index: " << this->index << ", id: " << this->theId << ".";
  return out.str();
}

std::string ThreadData::ToStringConsole() const {
  std::stringstream out;
  out << "Thread ";
  if (this->name == "") {
    out << "(thread name not set)";
  } else {
    out << this->name;
  }
  out << ". Index: " << this->index << ", id: " << this->theId << ".";
  return out.str();
}

std::string ThreadData::ToStringAllThreadsHtml() {
  std::stringstream out;
  out << theGlobalVariables.theThreadData.size << " threads registered. <br> "
  << theGlobalVariables.theThreads.size << " total threads.<br>";
  for (int i = 0; i < theGlobalVariables.theThreadData.size; i ++) {
    out << theGlobalVariables.theThreadData[i].ToStringHtml() << "<br>";
  }
  return out.str();
}

std::string ThreadData::ToStringAllThreadsConsole() {
  std::stringstream out;
  out << theGlobalVariables.theThreadData.size << " threads registered. "
  << theGlobalVariables.theThreads.size << " total threads.\n";
  for (int i = 0; i < theGlobalVariables.theThreadData.size; i ++) {
    out << theGlobalVariables.theThreadData[i].ToStringConsole() << "\n";
  }
  return out.str();
}