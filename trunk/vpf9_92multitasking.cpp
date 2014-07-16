//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader2Math0_General.h"
#include <assert.h> //Mutexes are way too basic infrastructure. We cannot crash using the standard crashing mechanism,
//as that mechanism might depend on mutexes.

ProjectInformationInstance vpfGeneral2Mutexes(__FILE__, "Multitasking implementation.");
extern GlobalVariables onePredefinedCopyOfGlobalVariables;

//Why the code below? Because of the static initialization order fiasco!!!!!!!!!
static ListReferences<pthread_mutex_t>& theMutexesGlobal()
{ static ListReferences<pthread_mutex_t> theMutexesGlobalHolder;
  return theMutexesGlobalHolder;
}

//Why the code below? Because of the static initialization order fiasco!!!!!!!!!
static List<MutexWrapper*>& theMutexWrappersGlobal()
{ static List<MutexWrapper*> theMutexWrappersGlobalHolder;
  return theMutexWrappersGlobalHolder;
}

void MutexWrapper::CheckConsistency()
{ if (this->theMutexIndex<0 || this->theMutexIndex>=theMutexesGlobal().size)
  { std::cout << "Bad mutex with this->theMutexIndex= " << this->theMutexIndex << std::endl;
    assert(false);
  }
  if (theMutexesGlobal().flagDeallocated)
  { std::cout << "theMutexesGlobal has been deallocated but a mutex is still using it. " << std::endl;
    assert(false);
  }
  if (theMutexWrappersGlobal().flagDeallocated)
  { std::cout << "theMutexWrappersGlobal has been deallocated but a mutex is still using it. " << std::endl;
    assert(false);
  }
}

MutexWrapper::MutexWrapper()
{ //    std::cout << "MutexWrapper::MutexWrapper. theMutexesGlobal.size: " << theMutexesGlobal.size << std::endl;
//  static bool allowToRun=true;
//  cantfuckingbelievethisfuckingpieceofshitiscrashing=true;
//  while (!allowToRun)
//  {}
//  allowToRun=false;
  this->theMutexIndex=-1;
  int oldSize=theMutexesGlobal().size;
  theMutexesGlobal().SetSize(theMutexesGlobal().size+1);
  if (oldSize==theMutexesGlobal().size)
    assert(false);
  this->theMutexIndex=theMutexesGlobal().size-1;
  if (this->theMutexIndex>=theMutexesGlobal().size)
    assert(false);

  //    std::cout << "MutexWrapper::MutexWrapper - 2." << std::endl;
  this->flagUnsafeFlagForDebuggingIsLocked=false;
  this->flagInitialized=false;
  theMutexWrappersGlobal().AddOnTop(this);
  if (theMutexWrappersGlobal().size!=theMutexesGlobal().size)
  { std::cout << "theMutexWrappersGlobal.size!=theMutexesGlobal.size is not supposed to happen!" << std::endl;
    assert(false);
  }
  if (theMutexWrappersGlobal().size>10000)
  { std::cout << "This is a programming error. Created total more than the hard-coded limit of 1000 mutexes. "
    << "The number of mutexes is supposed to be bounded and much smaller. " << std::endl;
    assert(false);
  }
  this->InitializeIfNeeded();
//  std::cout << "Mutexes created so far: " << theMutexesGlobal().size << std::endl;
//  allowToRun=true;
}

MutexWrapper::~MutexWrapper()
{ if (this->theMutexIndex<0)
    return;
  if (theMutexesGlobal().flagDeallocated)
  { std::cout << "Use after free of theMutexesGlobal()!" << std::endl;
    assert(false);
  }
  std::cout << "Running mutex wrapper destructor, this->theMutexIndex= " << this->theMutexIndex << std::endl;
//  if (theMutexWrappersGlobal()[this->theMutexIndex])
  theMutexWrappersGlobal()[this->theMutexIndex]=0;
  pthread_mutex_destroy(&theMutexesGlobal()[this->theMutexIndex]);
  this->theMutexIndex=-1;
}

bool MutexWrapper::isLockedUnsafeUseForWINguiOnly()
{ this->CheckConsistency();
  return this->flagUnsafeFlagForDebuggingIsLocked;
}

void MutexWrapper::LockMe()
{ this->CheckConsistency();
  if (!this->InitializeIfNeeded())
    return;
  if (this->theMutexIndex<0)
  { std::cout << "this->theMutesIndex = " << this->theMutexIndex << " impossible!" << std::endl;
    std::cout << crash.GetStackTraceShort();
    assert(false);
  }
#ifndef WIN32
  pthread_mutex_lock(&theMutexesGlobal()[this->theMutexIndex]);
  this->flagUnsafeFlagForDebuggingIsLocked=true;
#else
  while(this->locked)
  {}
  this->locked=true;
#endif
}

void MutexWrapper::UnlockMe()
{ this->CheckConsistency();
  if (!this->InitializeIfNeeded())
    return;
  if (this->theMutexIndex<0)
  { std::cout << crash.GetStackTraceShort() << std::endl;
    std::cout << "this->theMutesIndex = " << this->theMutexIndex << " impossible! " << std::endl;
    assert(false);
  }

#ifndef WIN32
  pthread_mutex_unlock(&theMutexesGlobal()[this->theMutexIndex]);
#endif
  this->flagUnsafeFlagForDebuggingIsLocked=false;
}

bool MutexWrapper::InitializeIfNeeded()
{ this->CheckConsistency();
  if (this->flagInitialized)
    return true;
  if (!onePredefinedCopyOfGlobalVariables.flagAllowUseOfThreadsAndMutexes)
    return false;
  if (this->theMutexIndex>=theMutexesGlobal().size || this->theMutexIndex<0)
  { std::cout << "this->theMutexIndex: " << this->theMutexIndex << ", theMutexesGlobal.size: " << theMutexesGlobal().size
    << ", theMutexWrappersGlobal.size: " << theMutexWrappersGlobal().size;
    assert(false);
  }
#ifndef WIN32
  pthread_mutex_init(&theMutexesGlobal()[this->theMutexIndex], NULL);
#endif
  this->flagInitialized=true;
  return true;
}

void MutexWrapper::InitializeAllAllocatedMutexes()
{ for (int i=0; i<theMutexWrappersGlobal().size; i++)
    if (theMutexWrappersGlobal()[i]!=0)
      theMutexWrappersGlobal()[i]->InitializeIfNeeded();
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

