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

#ifndef WIN32
void* RunTimerVoidPtr(void* ptr)
{ double elapsedtime=-1;
  for (; ;)
  { usleep(100);
    elapsedtime=GetElapsedTimeInSeconds();
    if (ComputationComplete)
      break;
    if (elapsedtime >=onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit && !
        (onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit<=0))
      break;
  }
  if (!ComputationComplete)
  { stOutput << "</div><br><br><br>Your computation has taken " << elapsedtime << " seconds so far.";
    stOutput << "<br>The maximum allowed run time for " << " the entire system is  " << onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit
    << " seconds (twice the amount of time allowed for calculator interpretation). <br>This safety limit is hard coded in this particular server. "
    << "<br>However, if you install the calculator on your own machine you may <br>allow arbitrarily large execution time by modifying "
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

void getPath(char* path, std::string& output)
{ if (path==0)
    return;
  int length=0;
  output= path;
  while (output[length]!=0 && length<150)
    length++;
  for (int i=length-1; i>=0; i--)
  {
#ifdef WIN32
	if (output[i]=='\\' )
    { output.resize(i+1);
      return;
    }
#else
    if (output[i]=='/')
    { output.resize(i+1);
      return;
    }
#endif
  }
}
