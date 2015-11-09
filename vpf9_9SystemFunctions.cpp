//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#endif
//#define cgiLimitRAMuseNumPointersInList

#include "vpfHeader4SystemFunctionsGlobalObjects.h"
#include "vpfHeader6WebServer.h"
ProjectInformationInstance projectInfoInstanceCalculatorSystem(__FILE__, "System functions, platform dependent code.");


timeval ComputationStartGlobal, LastMeasureOfCurrentTime;
pthread_t TimerThread;

double GetElapsedTimeInSeconds()
{ gettimeofday(&LastMeasureOfCurrentTime, NULL);
  int miliSeconds =(LastMeasureOfCurrentTime.tv_sec- ComputationStartGlobal.tv_sec)*1000+(LastMeasureOfCurrentTime.tv_usec- ComputationStartGlobal.tv_usec)/1000;
  return ((double) miliSeconds)/1000;
}

void InitializeTimer()
{
#ifndef WIN32
  gettimeofday(&ComputationStartGlobal, NULL);
#endif
}

void ignoreUserAbortSignal()
{
#ifndef WIN32
  signal(SIGABRT,SIG_IGN);
  signal(SIGTERM,SIG_IGN);
#endif
}

void CreateTimerThread()
{
#ifndef WIN32
  pthread_create(&TimerThread, NULL,*RunTimerVoidPtr, 0);
#endif
}

void SleepFunction(int microseconds)
{ usleep(microseconds);
}

struct TimerThreadData{
  double elapsedtime;
  double elapsedComputationTime;
  double computationStartTime;
  int counter=0;
  int microsecondsleep=100000;
  ProgressReportWebServer theReport1, theReport2;
  void Run();
  bool HandleComputationTimer();
  bool HandleComputationCompleteStandard();
  bool HandleTimerSignalToServer();
  bool HandleMaxComputationTime();
  bool HandlePingServerIamAlive();
  bool HandleComputationTimeout();
};

bool TimerThreadData::HandleComputationTimer()
{ if (onePredefinedCopyOfGlobalVariables.flagComputationStarted)
    if (this->computationStartTime<0)
      this->computationStartTime=GetElapsedTimeInSeconds();
  this->elapsedtime=GetElapsedTimeInSeconds();
  if (this->computationStartTime>0)
    this->elapsedComputationTime=this->elapsedtime-this->computationStartTime;
  return false;
}

bool TimerThreadData::HandleComputationCompleteStandard()
{ if (onePredefinedCopyOfGlobalVariables.flagComputationCompletE)
    theReport1.SetStatus("Starting timer cycle break: computation is complete.");
  return false;
}

bool TimerThreadData::HandleTimerSignalToServer()
{ if (this->counter%20!=0)
    return false;
  std::stringstream reportStream;
  reportStream << "Timer: " << this->elapsedtime << " seconds elapsed since the timer was launched.";
  theReport1.SetStatus(reportStream.str());
}

bool TimerThreadData::HandleMaxComputationTime()
{ if (onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit<=0)
    return false;
  if (elapsedComputationTime<=0)
    return false;
  if (elapsedComputationTime<=onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit)
    return false;
  if (onePredefinedCopyOfGlobalVariables.flagComputationCompletE)
  { theReport2.SetStatus((std::string)"The allowed clock time has ran out, but it seems the computation is already done. "+
                         (std::string)"Continuing to run (in order to wrap it up)...");
    return false;
  }
  theReport2.SetStatus("Starting timer cycle break: computation time too long.");
  if (!onePredefinedCopyOfGlobalVariables.flagComputationStarted)
    crash << "Something has gone wrong. Computation has not started, yet " << elapsedtime
    << " seconds have already passed."
    << " This may be an error in the web-server routines of the calculator!" << crash;
  std::stringstream out;
  out << "<b>This is a safety time-out crash. You may have requested a computation that takes too long."
  << "</b> Your computation ran for ";
  if (elapsedComputationTime>0)
    out << elapsedComputationTime << "seconds";
  else
    out << " (unknown amount of time)";
  out << ". The allowed run time is "
  << onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit
  << " seconds (twice the amount of time allowed for calculator interpretation). "
  << "<br>The present timeout limit may be extended by modifying onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit;"
  << " the restriction can be altogether lifted by modifying source file "
  << __FILE__ << "<br>";
  crash << out.str() << crash;
  return true;
}

bool TimerThreadData::HandleComputationTimeout()
{ if (!onePredefinedCopyOfGlobalVariables.flagUsingBuiltInWebServer)
    return false;
  if (onePredefinedCopyOfGlobalVariables.MaxComputationTimeBeforeWeTakeAction<=0)
    return false;
  if (elapsedComputationTime<=0)
    return false;
  if (elapsedComputationTime<=onePredefinedCopyOfGlobalVariables.MaxComputationTimeBeforeWeTakeAction)
    return false;
  if (onePredefinedCopyOfGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection==0)
    return false;
  if (! onePredefinedCopyOfGlobalVariables.flagOutputTimedOut ||
      onePredefinedCopyOfGlobalVariables.flagTimeOutExplanationAlreadyDisplayed)
    return false;
  onePredefinedCopyOfGlobalVariables.flagTimeOutExplanationAlreadyDisplayed=true;
  theReport2.SetStatus("Starting timer cycle displaying time out explanation.");
  onePredefinedCopyOfGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection();
  theReport2.SetStatus("Starting timer cycle displaying time out indicator done, continuing timer cycle.");
}

bool TimerThreadData::HandlePingServerIamAlive()
{ if (onePredefinedCopyOfGlobalVariables.flagComputationFinishedAllOutputSentClosing)
    return true;
  if (onePredefinedCopyOfGlobalVariables.WebServerTimerPing==0)
    return false;
  onePredefinedCopyOfGlobalVariables.WebServerTimerPing(this->elapsedtime);
  return false;
}

void TimerThreadData::Run()
{ this->elapsedtime=-1;
  this->elapsedComputationTime=-1;
  this->computationStartTime=-1;
  this->counter=0;
  this->microsecondsleep=100000;
//  std::cout << "Got thus far RunTimerVoidPtr - 2" << std::endl;
  for (; ;)
  { this->counter++;
    this->HandleComputationTimer();
    SleepFunction(microsecondsleep);
    this->HandleComputationCompleteStandard();
    this->HandleTimerSignalToServer();
    this->HandleMaxComputationTime();
    this->HandleComputationTimeout();
    if (this->HandlePingServerIamAlive())
      break;
  }
  RegisterFunctionCall(__FILE__, __LINE__, "exiting timer");
  theReport2.SetStatus("Timer thread: finished.");
}


#ifndef WIN32
void* RunTimerVoidPtr(void* ptr)
{ MacroRegisterFunctionWithName("RunTimerVoidPtr");
//  std::cout << "Got thus far RunTimerVoidPtr" << std::endl;
  TimerThreadData theThread;
  theThread.Run();


  pthread_exit(NULL);
  return 0;
}
#endif

void CallSystemWrapper(const std::string& theCommand)
{ system(theCommand.c_str());
}

void CallChDirWrapper(const std::string& theDir)
{ chdir(theDir.c_str());
}
