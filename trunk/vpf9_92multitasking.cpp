//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader2Math0_General.h"
#include <assert.h>
#include <thread>
#include <mutex>

ProjectInformationInstance vpfGeneral2Mutexes(__FILE__, "Multitasking implementation.");

void ParallelComputing::CheckPointerCounters()
{ if (ParallelComputing::GlobalPointerCounter>::ParallelComputing::cgiLimitRAMuseNumPointersInList)
  { MutexRecursiveWrapper& tempMutex=
    theGlobalVariables.MutexParallelComputingStaticFiasco
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

void MutexRecursiveWrapper::CheckConsistency()
{ if (this->flagDeallocated)
  { std::cout << "Use after free of mutex. " << crash.GetStackTraceShort() << std::endl;
    assert(false);
  }
}

void MutexRecursiveWrapper::initConstructorCallOnly()
{ //    std::cout << "MutexRecursiveWrapper::MutexRecursiveWrapper. theMutexesGlobal.size: " << theMutexesGlobal.size << std::endl;
//  static bool allowToRun=true;
//  while (!allowToRun)
//  {}
//  allowToRun=false;

  this->flagDeallocated=false;
  this->flagInitialized=false;
  this->flagUnsafeFlagForDebuggingIsLocked=false;
  this->theMutexImplementation=0;
#ifdef CGIversionLimitRAMuse
ParallelComputing::GlobalPointerCounter++;
#endif
}

bool MutexRecursiveWrapper::InitializeIfNeeded()
{ if (this->flagInitialized)
    return true;
  if (!theGlobalVariables.flagAllowUseOfThreadsAndMutexes)
    return false;
  this->theMutexImplementation= new std::mutex;
  this->flagInitialized=true;
  return true;
}

MutexRecursiveWrapper::~MutexRecursiveWrapper()
{ delete (std::mutex*)(this->theMutexImplementation);
  this->theMutexImplementation=0;
  this->flagDeallocated=true;
  this->flagInitialized=false;
}

bool MutexRecursiveWrapper::isLockedUnsafeUseForWINguiOnly()
{// std::cout << "checking consistency from isLockedUnsafeUseForWINguiOnly";
  this->CheckConsistency();
  return this->flagUnsafeFlagForDebuggingIsLocked;
}

void MutexRecursiveWrapper::LockMe()
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

void MutexRecursiveWrapper::UnlockMe()
{// std::cout << "checking consistency from unlockme";
  this->CheckConsistency();
  if (!this->InitializeIfNeeded())
    return;
  ((std::mutex*)this->theMutexImplementation)->unlock();
  this->flagUnsafeFlagForDebuggingIsLocked=false;
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

ThreadData::ThreadData()
{ this->id=0;
  this->threadPointer=0;
}

ThreadData::~ThreadData()
{
}

void GlobalVariables::InitializeThreadData()
{

}
