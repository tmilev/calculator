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

#ifndef WIN32
void* RunTimerVoidPtr(void* ptr)
{ double elapsedtime=-1;
  for (; ;)
  { SleepFunction(100000);
    elapsedtime=GetElapsedTimeInSeconds();
    if (onePredefinedCopyOfGlobalVariables.flagComputationComplete)
      break;
    if (onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit>0)
      if (elapsedtime>onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit)
        break;
    if (onePredefinedCopyOfGlobalVariables.MaxComputationTimeBeforeWeTakeAction>0)
      if (elapsedtime>onePredefinedCopyOfGlobalVariables.MaxComputationTimeBeforeWeTakeAction)
        if (onePredefinedCopyOfGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection!=0)
          if (!onePredefinedCopyOfGlobalVariables.flagOutputTimedOut!=0 &&
              !onePredefinedCopyOfGlobalVariables.flagDisplayTimeOutExplanation)
          { onePredefinedCopyOfGlobalVariables.flagDisplayTimeOutExplanation=true;
            onePredefinedCopyOfGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection();
          }
  }
  if (!onePredefinedCopyOfGlobalVariables.flagComputationComplete)
  { crash << "Your computation ran for " << elapsedtime << " seconds and exceeded the allowed run time of "
    << onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit
    << " seconds (twice the amount of time allowed for calculator interpretation). "
    << "<br>This safety limit is hard coded in "
    << " the variable onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit located in file "
    << __FILE__ << "<br><b>Signalling ungraceful exit...</b> ";
    crash << crash;
    return 0;
  } else
    pthread_exit(NULL);
}
#endif

void CallSystemWrapper(const std::string& theCommand)
{ system(theCommand.c_str());
}
