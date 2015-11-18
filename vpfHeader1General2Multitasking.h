#ifndef header_vpfHeader1General2Mutexes_already_Defined
#define header_vpfHeader1General2Mutexes_already_Defined
#include "vpfMacros.h"
static ProjectInformationInstance vpfHeader1General2Mutexes(__FILE__, "Header, mutexes. ");

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//The documentation of pthreads.h can be found at:
// https://computing.llnl.gov/tutorials/pthreads/#MutexOverview
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//The below class is a wrapper for mutexes. All system dependent machinery for
//mutexes should be put here.
//MutexRecursiveWrapper specification:
//The mutex has two states: locked and unlocked.
//When the caller calls UnlockMe() this unlocks the mutex if it were locked,
//otherwise does nothing, and immediately returns.
//When the caller calls LockMe() there are two cases.
//1) First, If the mutex is unlocked, the mutex state changes to
//locked and execution of the caller continues.
//The preceding two operations are atomic: if the mutex happens to be unlocked,
//no other processor instruction
//can be executed before the mutex's state is changed to locked.
//2) Second, if the mutex is locked, the calling thread must pause execution,
//without consuming computational/processor power.
// As soon as the mutex is unlocked (by another thread or by the system),
//the calling thread is allowed to wake up and perform the sequence described in 1).
// The wake-up time is unspecified/not guaranteed to be immediate: another thread
//might "jump in" and overtake, again locking the calling thread.
// In order to have guaranteed wake-up when coordinating two threads only, use the
//controller object (which uses two mutexes to achieve guaranteed wake-up).
//3) Mutexes cannot be copied: once allocated, they stay in place.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MutexRecursiveWrapper
{
private:
  bool flagUnsafeFlagForDebuggingIsLocked;
  bool flagDeallocated;
  bool flagInitialized;
  void operator=(const MutexRecursiveWrapper& other);
  MutexRecursiveWrapper(const MutexRecursiveWrapper& other);
  bool InitializeIfNeeded();
public:
  std::string mutexName;
  void* theMutexImplementation;
  //note: the mutex implementation is not a named type for system portability.
  bool isLockedUnsafeUseForWINguiOnly();
  //locks the mutex if the mutex is free. If not it suspends calling thread until
  //mutex becomes free and then locks it.
  void LockMe();
  //unlocks the mutex.
  void UnlockMe();
  void CheckConsistency();
  void initConstructorCallOnly();
  MutexRecursiveWrapper()
  { this->initConstructorCallOnly();
  }
  MutexRecursiveWrapper(const std::string& inputMutexName)
  { this->mutexName=inputMutexName;
    this->initConstructorCallOnly();
  }
  ~MutexRecursiveWrapper();
};

//this class uses RAII to lock MutexRecursiveWrapper's -> equivalent to std::lock_guard
class MutexLockGuard{
public:
  MutexRecursiveWrapper* theMutex;
  MutexLockGuard(MutexRecursiveWrapper& inputMutex)
  { this->theMutex=&inputMutex;
    this->theMutex->LockMe();
  }
  ~MutexLockGuard()
  { this->theMutex->UnlockMe();
    this->theMutex=0;
  }
};

class ThreadData
{
public:
  int index;
  std::string name;
  void* threadPointer;
  std::string ToString()const;
  static int getCurrentThreadId();
  static void RegisterCurrentThread(const std::string& inputName="");
  static void CreateThread(void (*InputFunction)());
  static std::string ToStringAllThreads();
  ThreadData();
  ~ThreadData();
};

class Controller
{
  private:
  MutexRecursiveWrapper mutexLockMeToPauseCallersOfSafePoint;
  MutexRecursiveWrapper mutexSignalMeWhenReachingSafePoint;
  bool flagIsRunning;
  bool flagIsPausedWhileRunning;
  bool IsPausedWhileRunning()const;
  void operator=(const Controller& other);
  Controller(const Controller& other);
public:
  MutexRecursiveWrapper mutexHoldMeWhenReadingOrWritingInternalFlags;
  void SafePointDontCallMeFromDestructors();
  void SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint();
  void UnlockSafePoint();
  void InitComputation();
  void ExitComputation();
  bool& GetFlagIsPausedWhileRunningUnsafeUseWithMutexHoldMe();
  bool& GetFlagIsRunningUnsafeUseWithMutexHoldMe();
  Controller();
};

class ControllerStartsRunning: public Controller
{
  public:
    ControllerStartsRunning()
    { this->InitComputation();
    }
};
#endif // header_vpfHeader1General2Mutexes_already_Defined
