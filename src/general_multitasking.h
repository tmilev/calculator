// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_general_multitasking_ALREADY_INCLUDED
#define header_general_multitasking_ALREADY_INCLUDED
#include "macros.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The documentation of pthreads.h can be found at:
// https://computing.llnl.gov/tutorials/pthreads/#MutexOverview
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The below class is a wrapper for mutexes. All system dependent machinery for
// mutexes should be put here.
// MutexRecursiveWrapper specification:
// The mutex has two states: locked and unlocked.
// When the caller calls unlockMe() this unlocks the mutex if it were locked,
// otherwise does nothing, and immediately returns.
// When the caller calls lockMe() there are two cases.
// 1) First, if the mutex is unlocked, the mutex state changes to
// locked and execution of the caller continues.
// The preceding two operations are atomic: if the mutex happens to be unlocked,
// no other processor instruction
// can be executed before the mutex's state is changed to locked.
// 2) Second, if the mutex is locked, the calling thread must pause execution,
// without consuming computational/processor power.
// As soon as the mutex is unlocked (by another thread or by the system),
// the calling thread is allowed to wake up and perform the sequence described in 1).
// The wake-up time is unspecified/not guaranteed to be immediate: another thread
// might "jump in" and overtake, again locking the calling thread.
// In order to have guaranteed wake-up when coordinating two threads only, use the
// controller object (which uses two mutexes to achieve guaranteed wake-up).
// 3) Mutexes cannot be copied: once allocated, they stay in place.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MutexRecursiveWrapper {
private:
  bool flagUnsafeFlagForDebuggingIslocked;
  int lastlockerThread;
  bool flagDeallocated;
  bool flagInitialized;
  static std::string nolockId;
  void operator=(const MutexRecursiveWrapper& other);
  MutexRecursiveWrapper(const MutexRecursiveWrapper& other);
public:
  bool initializeIfNeeded();
  std::string mutexName;
  void* mutexImplementation;
  // note: the mutex implementation is not a named type for system portability.
  bool islockedUnsafeUseForWINguiOnly();
  // locks the mutex if the mutex is free. If not it suspends calling thread until
  // mutex becomes free and then locks it.
  void lockMe();
  // unlocks the mutex.
  void unlockMe();
  void checkConsistency();
  void initConstructorCallOnly();
  MutexRecursiveWrapper() {
    this->initConstructorCallOnly();
  }
  MutexRecursiveWrapper(const std::string& inputMutexName) {
    this->mutexName = inputMutexName;
    this->initConstructorCallOnly();
  }
  ~MutexRecursiveWrapper();
};

//this class uses RAII to lock MutexRecursiveWrapper's -> equivalent to std::lock_guard
class MutexlockGuard {
public:
  MutexRecursiveWrapper* mutexWrapper;
  MutexlockGuard(MutexRecursiveWrapper& inputMutex) {
    this->mutexWrapper = &inputMutex;
    this->mutexWrapper->lockMe();
  }
  ~MutexlockGuard() {
    this->mutexWrapper->unlockMe();
    this->mutexWrapper = nullptr;
  }
};

class GlobalStatistics {
public:
  static long long globalPointerCounter;
  static long long pointerCounterPeakRamUse;
  static long long cgiLimitRAMuseNumPointersInList;
  static bool flagUngracefulExitInitiated;
  static unsigned int numberOfListsCreated;
  static unsigned int numberOfListResizesTotal;
  static unsigned int numberOfHashResizes;

  static void checkPointerCounters();
};

#endif // header_general_multitasking_ALREADY_INCLUDED
