//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader2Math0_General.h"
#include "vpfHeader1General4_Logging.h"
#include "vpfHeader4SystemFunctionsGlobalObjects.h"
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
  { logBlock << logger::red << "Use after free of mutex. "
    << crash.GetStackTraceShort() << logger::endL;
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
{ this->index=0;
  this->threadPointer=0;
}

ThreadData::~ThreadData()
{ //this may be use after free depending on whether GetThreads is destroyed first
  logBlock << "joining threads ...";
  for (int i=1; i<theThreads.size; i++)
    theThreads[i].join();

  logBlock << "joining threads done.";
}

void ThreadData::RegisterCurrentThread(const std::string& inputName)
{ int threadID=ThreadData::getCurrentThreadId();
  ListReferences<ThreadData>& theThreads=theGlobalVariables.theThreadData;
  for (int i=0; i<theThreads.size; i++)
    if (threadID==theThreads[i].index)
      return;
  MutexLockGuard theLock(theGlobalVariables.MutexRegisterNewThread);
  theThreads.SetSize(theThreads.size+1);
  ThreadData& currentThread=theThreads.LastObject();
  currentThread.index=threadID;
  currentThread.name=inputName;
}

void ThreadData::CreateThread(void (*InputFunction)())
{ MutexLockGuard(theGlobalVariables.MutexRegisterNewThread);
  theThreads.SetSize(theThreadIds.size+1);
  theThreads.LastObject()=std::thread(InputFunction);
}

int ThreadData::getCurrentThreadId()
{ std::thread::id currentId= std::this_thread::get_id();
  int result=-1;
  for (int i=0; i<theThreadIds.size; i++)
    if (currentId==theThreadIds[i])
    { result=i;
      break;
    }
  if (result==-1)
  { MutexLockGuard theLock(theGlobalVariables.MutexRegisterNewThread);
    result=theThreadIds.size;
    theThreadIds.AddOnTop(currentId);
  }
  return result;
}

std::string ThreadData::ToString()const
{ std::stringstream out;
  out << "Thread ";
  if (this->name=="")
    out << "(thread name not set)";
  else
    out << this->name;
  out << ". Index: " << this->index << ", id: " << theThreadIds[this->index] << ".";
  return out.str();
}

std::string ThreadData::ToStringAllThreads()
{ std::stringstream out;
  for (int i=0; i<theGlobalVariables.theThreadData.size; i++)
    out << theGlobalVariables.theThreadData[i].ToString() << "<br>";
  return out.str();
}
