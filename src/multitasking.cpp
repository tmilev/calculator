// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general.h"
#include "general_logging_global_variables.h"
#include "system_functions_global_objects.h"
#include <assert.h>
#include <mutex>

void GlobalStatistics::checkPointerCounters() {
  if (GlobalStatistics::globalPointerCounter > GlobalStatistics::cgiLimitRAMuseNumPointersInList) {
    /////////////////////////////////////////////////
    // *** Deadlock alert: critical section start ***
    global.mutexParallelComputingCrash.lockMe();
    if (GlobalStatistics::flagUngracefulExitInitiated) {
      global.mutexParallelComputingCrash.unlockMe();
      return;
    }
    GlobalStatistics::flagUngracefulExitInitiated = true;
    global.mutexParallelComputingCrash.unlockMe();
    // *** Deadlock alert: critical section end ***
    /////////////////////////////////////////////////
    global.fatal << "This may or may not be an error: the number of pointers "
    << "allocated by the program exceeded the allowed <b>limit of "
    << GlobalStatistics::cgiLimitRAMuseNumPointersInList
    << ".</b>" << global.fatal;
  }
  if (GlobalStatistics::pointerCounterPeakRamUse < GlobalStatistics::globalPointerCounter) {
    GlobalStatistics::pointerCounterPeakRamUse = GlobalStatistics::globalPointerCounter;
  }
}

void MutexRecursiveWrapper::checkConsistency() {
  if (this->flagDeallocated) {
    global << Logger::red << "Use after free of mutex. "
    << global.fatal.getStackTraceEtcErrorMessageConsole() << Logger::endL;
    // We are in a highly corrupt state, standard crashing mecahnisms may not work.
    global.fatal << "Use after free of mutex. " << global.fatal;
  }
}

void MutexRecursiveWrapper::initConstructorCallOnly() {
  this->flagDeallocated = false;
  this->flagInitialized = false;
  this->flagUnsafeFlagForDebuggingIslocked = false;
  this->mutexImplementation = nullptr;
  this->lastlockerThread = - 1;
#ifdef AllocationLimitsSafeguard
GlobalStatistics::globalPointerCounter ++;
#endif
}

bool MutexRecursiveWrapper::initializeIfNeeded() {
  if (this->flagInitialized) {
    return true;
  }
  if (!global.flagServerforkedIntoWorker) {
    return false;
  }
  this->mutexImplementation = new std::mutex;
  this->flagInitialized = true;
  return true;
}

MutexRecursiveWrapper::~MutexRecursiveWrapper() {
  delete static_cast<std::mutex*>(this->mutexImplementation);
  this->mutexImplementation = nullptr;
  this->flagDeallocated = true;
  this->flagInitialized = false;
}

bool MutexRecursiveWrapper::islockedUnsafeUseForWINguiOnly() {
 // std::cout << "checking consistency from islockedUnsafeUseForWINguiOnly";
  this->checkConsistency();
  return this->flagUnsafeFlagForDebuggingIslocked;
}

void MutexRecursiveWrapper::lockMe() {
  this->checkConsistency();
  if (!this->initializeIfNeeded()) {
    return;
  }
  try {
    if (this->flagUnsafeFlagForDebuggingIslocked) {
      int currentThreadId = ThreadData::getCurrentThreadId();
      if (currentThreadId == this->lastlockerThread) {
        global << Logger::red << "Fatal: about to self-lock: ["
        << this->mutexName << "] thread: "
        << currentThreadId
        << "." << Logger::endL
        << global.toStringProgressReportConsole() << Logger::endL;
      }
    }
    static_cast<std::mutex*>(this->mutexImplementation)->lock();
  } catch (...) {
    global.fatal << "Fatal error: mutex lock failed. " << global.fatal;
  }
  this->flagUnsafeFlagForDebuggingIslocked = true;
  this->lastlockerThread = ThreadData::getCurrentThreadId();
}

void MutexRecursiveWrapper::unlockMe() {
  this->checkConsistency();
  if (!this->initializeIfNeeded()) {
    return;
  }
  this->flagUnsafeFlagForDebuggingIslocked = false;
  this->lastlockerThread = - 1;
  static_cast<std::mutex*>(this->mutexImplementation)->unlock();
}

ThreadData::ThreadData() {
  this->index = 0;
//  this->theId = 0;
}

ThreadData::~ThreadData() {

}

void GlobalVariables::joinAllThreads() {
  this->flagComputationFinishedAllOutputSentClosing = true;
  for (int i = 1; i < this->allThreads.size; i ++) {
    this->allThreads[i].join();
  }
}

GlobalVariables::~GlobalVariables() {
  this->flagDeallocated = true;
}

void ThreadData::registerFirstThread(const std::string& inputName) {
  ThreadData::registerNewThread(inputName).id = std::this_thread::get_id();
}

ThreadData& ThreadData::registerNewThread(const std::string& inputName) {
  ListReferences<ThreadData>& theThreadData =
  global.threadData;
  ThreadData newThreadData;
  newThreadData.name = inputName;
  newThreadData.index = theThreadData.size;
  theThreadData.addOnTop(newThreadData);
  global.allThreads.setSize(theThreadData.size);
  global.customStackTrace.reserve(2);
  global.progressReportStrings.reserve(2);
  global.customStackTrace.setSize(theThreadData.size);
  global.progressReportStrings.setSize(theThreadData.size);
  global.customStackTrace.lastObject().reserve(30);
  global.progressReportStrings.lastObject().reserve(30);
  return global.threadData.lastObject();
}

void ThreadData::createThread(void (*InputFunction)(int), const std::string& inputName) {
  MutexlockGuard(global.mutexRegisterNewThread);
  ThreadData& threadData = ThreadData::registerNewThread(inputName);
  std::thread newThread(InputFunction, threadData.index);
  global.allThreads.lastObject().swap(newThread);
}

int ThreadData::getCurrentThreadId() {
  std::thread::id currentId = std::this_thread::get_id();
  ListReferences<ThreadData>& theThreadData = global.threadData;
  for (int i = 0; i < theThreadData.size; i ++) {
    if (currentId == theThreadData[i].id) {
      return i;
    }
  }
  return - 1;
}

std::string ThreadData::toStringHtml() const {
  std::stringstream out;
  out << "Process type: <b>" << global.logs.toStringProcessType() << "</b>. ";
  if (this->getCurrentThreadId() == this->index) {
    out << "<b style = 'color:green'>Current thread</b> ";
  } else {
    out << "Thread ";
  }
  out << "<span style = 'color:red'>";
  if (this->name == "") {
    out << "(thread name not set)";
  } else {
    out << this->name;
  }
  out << "</span>.";
  out << " Index: " << this->index << ", id: " << this->id << ".";
  return out.str();
}

std::string ThreadData::toStringConsole() const {
  std::stringstream out;
  out << "Thread ";
  if (this->name == "") {
    out << "(thread name not set)";
  } else {
    out << this->name;
  }
  out << ". Index: " << this->index << ", id: " << this->id << ".";
  return out.str();
}

std::string ThreadData::toStringAllThreadsHtml() {
  std::stringstream out;
  out << global.threadData.size << " threads registered. <br> "
  << global.allThreads.size << " total threads.<br>";
  for (int i = 0; i < global.threadData.size; i ++) {
    out << global.threadData[i].toStringHtml() << "<br>";
  }
  return out.str();
}

std::string ThreadData::toStringAllThreadsConsole() {
  std::stringstream out;
  out << global.threadData.size << " threads registered. "
  << global.allThreads.size << " total threads.\n";
  for (int i = 0; i < global.threadData.size; i ++) {
    out << global.threadData[i].toStringConsole() << "\n";
  }
  return out.str();
}
