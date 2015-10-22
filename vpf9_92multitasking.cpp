//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader2Math0_General.h"
#include <assert.h> //Mutexes are way too basic infrastructure. We cannot crash using the standard crashing mechanism,
//as that mechanism might depend on mutexes.
#include <mutex>

List<MutexWrapper*>& theMutexWrappers() //<- if you are wondering why this code, google "static initialization order fiasco"
{ static List<MutexWrapper*> tempObjects;
  return tempObjects;
}

ProjectInformationInstance vpfGeneral2Mutexes(__FILE__, "Multitasking implementation.");
extern GlobalVariables onePredefinedCopyOfGlobalVariables;

void ParallelComputing::CheckPointerCounters()
{ if (ParallelComputing::GlobalPointerCounter>::ParallelComputing::cgiLimitRAMuseNumPointersInList)
  { MutexWrapper& tempMutex=
    onePredefinedCopyOfGlobalVariables.MutexParallelComputingStaticFiasco
    ;
    tempMutex.LockMe();
    if (ParallelComputing::flagUngracefulExitInitiated)
    { tempMutex.UnlockMe();
      return;
    }
    ParallelComputing::flagUngracefulExitInitiated=true;
    tempMutex.UnlockMe();
    crash << "This may or may not be an error: the number of pointers "
    << "allocated by the program exceeded the allowed <b>limit of "
    << ParallelComputing::cgiLimitRAMuseNumPointersInList
    << ".</b>" << crash;
  }
  if (ParallelComputing::PointerCounterPeakRamUse<ParallelComputing::GlobalPointerCounter)
    ParallelComputing::PointerCounterPeakRamUse=ParallelComputing::GlobalPointerCounter;
}

void MutexWrapper::CheckConsistency()
{ if (this->flagDeallocated)
  { std::cout << "Use after free mutex index: " << this->indexInContainer
    << crash.GetStackTraceShort() << std::endl;
    assert(false);
  }
}

void MutexWrapper::initConstructorCallOnly()
{ //    std::cout << "MutexWrapper::MutexWrapper. theMutexesGlobal.size: " << theMutexesGlobal.size << std::endl;
//  static bool allowToRun=true;
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
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter++;
#endif
}

bool MutexWrapper::InitializeIfNeeded()
{ if (this->flagInitialized)
    return true;
  if (!onePredefinedCopyOfGlobalVariables.flagAllowUseOfThreadsAndMutexes)
    return false;
  this->theMutexImplementation= new std::mutex;
  this->flagInitialized=true;
  return true;
}

MutexWrapper::~MutexWrapper()
{ if (!onePredefinedCopyOfGlobalVariables.flagAllowUseOfThreadsAndMutexes)
    if (!theMutexWrappers().flagDeallocated)
      theMutexWrappers()[this->indexInContainer]=0; //this line of code might be unsafe when the master process exits, but is safe in all fork()-ed processes
  delete (std::mutex*)(this->theMutexImplementation);
    this->theMutexImplementation=0;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter--;
#endif
  this->flagDeallocated=true;
}

bool MutexWrapper::isLockedUnsafeUseForWINguiOnly()
{// std::cout << "checking consistency from isLockedUnsafeUseForWINguiOnly";
  this->CheckConsistency();
  return this->flagUnsafeFlagForDebuggingIsLocked;
}

void MutexWrapper::LockMe()
{ //std::cout << "checking consistency from lockme";
  this->CheckConsistency();
  if (!this->InitializeIfNeeded())
    return;
  try{
  ((std::mutex*)this->theMutexImplementation)->lock();
  }
  catch(int theException)
  { crash << "Fatal error: mutex lock failed." << crash;
  }
  this->flagUnsafeFlagForDebuggingIsLocked=true;
}

void MutexWrapper::UnlockMe()
{// std::cout << "checking consistency from unlockme";
  this->CheckConsistency();
  if (!this->InitializeIfNeeded())
    return;
  ((std::mutex*)this->theMutexImplementation)->unlock();
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

