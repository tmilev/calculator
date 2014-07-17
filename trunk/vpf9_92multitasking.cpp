//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader2Math0_General.h"
#include <assert.h> //Mutexes are way too basic infrastructure. We cannot crash using the standard crashing mechanism,
//as that mechanism might depend on mutexes.

List<MutexWrapper*>& theMutexWrappers() //<- if you are wondering why this code, google "static initialization order fiasco"
{ static List<MutexWrapper*> tempObjects;
  return tempObjects;
}

ProjectInformationInstance vpfGeneral2Mutexes(__FILE__, "Multitasking implementation.");
extern GlobalVariables onePredefinedCopyOfGlobalVariables;

void MutexWrapper::CheckConsistency()
{ if (this->flagDeallocated)
  { std::cout << "Use after free mutex index: " << this->indexInContainer << crash.GetStackTraceShort() << std::endl;
    assert(false);
  }
}

void MutexWrapper::initConstructorCallOnly()
{ //    std::cout << "MutexWrapper::MutexWrapper. theMutexesGlobal.size: " << theMutexesGlobal.size << std::endl;
//  static bool allowToRun=true;
//  cantfuckingbelievethisfuckingpieceofshitiscrashing=true;
//  while (!allowToRun)
//  {}
//  allowToRun=false;

  this->flagDeallocated=false;
  this->flagInitialized=false;
  this->flagUnsafeFlagForDebuggingIsLocked=false;
  this->theMutexImplementation=0;
  this->indexInContainer=theMutexWrappers().size;
  theMutexWrappers().AddOnTop(this);
  if (theMutexWrappers().size>1000)
  { std::cout << "Too many mutexes. " << std::endl;
    assert(false);
  }
  if (this->indexInContainer==7)
  { std::cout << " naughty mutex: " << this->indexInContainer << "name: " << this->mutexName << " stack: " << crash.GetStackTraceShort() << std::endl;
  }
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter++;
#endif
}

bool MutexWrapper::InitializeIfNeeded()
{ if (this->flagInitialized)
    return true;
  if (!onePredefinedCopyOfGlobalVariables.flagAllowUseOfThreadsAndMutexes)
    return false;
  this->theMutexImplementation= new pthread_mutex_t;
  pthread_mutex_init((pthread_mutex_t*) this->theMutexImplementation, NULL);
  this->flagInitialized=true;
  return true;
}

MutexWrapper::~MutexWrapper()
{ if (!onePredefinedCopyOfGlobalVariables.flagAllowUseOfThreadsAndMutexes)
    if (!theMutexWrappers().flagDeallocated)
      theMutexWrappers()[this->indexInContainer]=0; //this line of code might be unsafe when the master process exits, but is safe in all fork()-ed processes
  delete (pthread_mutex_t*)(this->theMutexImplementation);//whoever tells me this code is bad
//will have to submit to me a written essay of how to avoid the static initialization/deinitialization order fiasco
//AND write portable code, or will risk being verbally and possibly physically assaulted by myself.
//I am writing this after having lost 3 days of my precious and highly qualified time
//(Yes I do have a Ph.D. in mathematics and am a published scientist)
//on C++'s retarded design, because this retarded language doesn't have elementary means of
//specifying the order of destructors of objects.
//To give you a hint of the pain this retarded language caused me: I need static mutexes, initialized
//via user call after fork()-ing the process.
  this->theMutexImplementation=0;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter--;
#endif
  this->flagDeallocated=true;
}

bool MutexWrapper::isLockedUnsafeUseForWINguiOnly()
{ std::cout << "checking consistency from isLockedUnsafeUseForWINguiOnly";
  this->CheckConsistency();
  return this->flagUnsafeFlagForDebuggingIsLocked;
}

void MutexWrapper::LockMe()
{ //std::cout << "checking consistency from lockme";
  this->CheckConsistency();
  if (!this->InitializeIfNeeded())
    return;
#ifndef WIN32
  pthread_mutex_lock((pthread_mutex_t*)this->theMutexImplementation);
  this->flagUnsafeFlagForDebuggingIsLocked=true;
#else
  while(this->locked)
  {}
  this->locked=true;
#endif
}

void MutexWrapper::UnlockMe()
{ std::cout << "checking consistency from unlockme";
  this->CheckConsistency();
  if (!this->InitializeIfNeeded())
    return;
#ifndef WIN32
  pthread_mutex_unlock((pthread_mutex_t*)this->theMutexImplementation);
#endif
  this->flagUnsafeFlagForDebuggingIsLocked=false;
}

void MutexWrapper::InitializeAllAllocatedMutexesAllowMutexUse()
{ MacroRegisterFunctionWithName("MutexWrapper::InitializeAllAllocatedMutexesAllowMutexUse");
  static bool alreadyRan=false;
  onePredefinedCopyOfGlobalVariables.flagAllowUseOfThreadsAndMutexes=true;
  if (alreadyRan)
    return;
  alreadyRan=true;
  for(int i=0; i<theMutexWrappers().size; i++)
    if (theMutexWrappers()[i]!=0)
      theMutexWrappers()[i]->InitializeIfNeeded();
}

void Controller::SafePointDontCallMeFromDestructors()
{ this->mutexSignalMeWhenReachingSafePoint.UnlockMe();
  this->mutexLockMeToPauseCallersOfSafePoint.LockMe();
  this->mutexSignalMeWhenReachingSafePoint.LockMe();
  this->mutexLockMeToPauseCallersOfSafePoint.UnlockMe();
}

void Controller::SignalPauseToSafePointCallerAndPauseYourselfUntilOtherReachesSafePoint()
{ this->mutexHoldMeWhenReadingOrWritingInternalFlags.LockMe();
  if (this->flagIsPausedWhileRunning)
  { this->mutexHoldMeWhenReadingOrWritingInternalFlags.UnlockMe();
    return;
  }
  this->mutexHoldMeWhenReadingOrWritingInternalFlags.UnlockMe();
  this->mutexLockMeToPauseCallersOfSafePoint.LockMe();
  this->mutexSignalMeWhenReachingSafePoint.LockMe();
  this->flagIsPausedWhileRunning=true;
  this->mutexSignalMeWhenReachingSafePoint.UnlockMe();
}

void Controller::UnlockSafePoint()
{ this->flagIsPausedWhileRunning=false;
  this->mutexLockMeToPauseCallersOfSafePoint.UnlockMe();
}

void Controller::InitComputation()
{ this->mutexSignalMeWhenReachingSafePoint.LockMe();
  this->flagIsRunning=true;
}

void Controller::ExitComputation()
{ this->flagIsRunning=false;
  this->mutexSignalMeWhenReachingSafePoint.UnlockMe();
}

bool& Controller::GetFlagIsPausedWhileRunningUnsafeUseWithMutexHoldMe()
{ return this->flagIsPausedWhileRunning;
}

bool& Controller::GetFlagIsRunningUnsafeUseWithMutexHoldMe()
{ return this->flagIsRunning;
}

Controller::Controller()
{ this->flagIsRunning=false;
  this->flagIsPausedWhileRunning=false;
}

bool Controller::IsPausedWhileRunning()const
{ return this->flagIsPausedWhileRunning;
}

