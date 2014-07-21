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

#ifndef WIN32
void* RunTimerVoidPtr(void* ptr)
{ MacroRegisterFunctionWithName("RunTimerVoidPtr");
//  std::cout << "Got thus far RunTimerVoidPtr" << std::endl;

  double elapsedtime=-1;
  double elapsedComputationTime=-1;
  double computationStartTime=-1;
  ProgressReportWebServer theReport;
//  std::cout << "Got thus far RunTimerVoidPtr - 2" << std::endl;
  int counter=0;
  for (; ;)
  { //std::stringstream tempStream;
    counter++;
    //tempStream << "Starting timer thread cycle " << counter << ". ";
//    std::cout << "Got thus far RunTimerVoidPtr - 4" << std::endl;
//    theReport.SetStatus(tempStream.str());
//    std::cout << "Got thus far RunTimerVoidPtr - 5" << std::endl;
    if (onePredefinedCopyOfGlobalVariables.flagComputationStarted)
      if (computationStartTime<0)
        computationStartTime=GetElapsedTimeInSeconds();
    SleepFunction(100000);
    elapsedtime=GetElapsedTimeInSeconds();
//    std::cout << "Got thus far RunTimerVoidPtr - 3" << std::endl;
    if (computationStartTime>0)
      elapsedComputationTime=elapsedtime-computationStartTime;
    if (onePredefinedCopyOfGlobalVariables.flagComputationCompletE)
    { theReport.SetStatus("Starting timer cycle break: computation is complete.");
//      std::cout << "Got thus far 2-1" << std::endl;
      break;
    }
//    if (onePredefinedCopyOfGlobalVariables.flagUsingBuiltInWebServer)
//      if (!onePredefinedCopyOfGlobalVariables.flagComputationStarted)
//        if (elapsedtime>onePredefinedCopyOfGlobalVariables.MaxWebWorkerRunTimeWithoutComputationStartedSecondsNonPositiveMeansNoLimit)
//          break;
    if (onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit>0)
      if (elapsedComputationTime>0)
        if (elapsedComputationTime>onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit)
        { theReport.SetStatus("Starting timer cycle break: computation time too long.");
//          std::cout << "Got thus far 2-2" << std::endl;
          break;
        }
    if (onePredefinedCopyOfGlobalVariables.flagUsingBuiltInWebServer)
      if (onePredefinedCopyOfGlobalVariables.MaxComputationTimeBeforeWeTakeAction>0)
        if (elapsedComputationTime>0)
          if (elapsedComputationTime>onePredefinedCopyOfGlobalVariables.MaxComputationTimeBeforeWeTakeAction)
            if (onePredefinedCopyOfGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection!=0)
              if (!onePredefinedCopyOfGlobalVariables.flagOutputTimedOut!=0 &&
                  !onePredefinedCopyOfGlobalVariables.flagDisplayTimeOutExplanation)
              { onePredefinedCopyOfGlobalVariables.flagDisplayTimeOutExplanation=true;
//                std::cout << "Got thus far 2-3" << std::endl;
                theReport.SetStatus("Starting timer cycle displaying time out explanation.");
                onePredefinedCopyOfGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection();
//                std::cout << "Got thus far 2-4" << std::endl;
                theReport.SetStatus("Starting timer cycle displaying time out indicator done, continuing timer cycle.");
//                std::cout << "Got thus far 2-5" << std::endl;
              }

  }
  if (!onePredefinedCopyOfGlobalVariables.flagComputationCompletE)
  { if (onePredefinedCopyOfGlobalVariables.flagComputationStarted)
    { crash << "<b>This is a safety time-out crash. You may have requested a computation that takes too long.</b> Your computation ran for ";
      if (elapsedComputationTime>0)
        crash << elapsedComputationTime << "seconds";
      else
        crash << " (unknown amount of time)";
      crash << ". The allowed run time is "
      << onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit
      << " seconds (twice the amount of time allowed for calculator interpretation). "
      << "<br>The present timeout limit may be extended by modifying onePredefinedCopyOfGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit;"
      << " the restriction can be altogether lifted by modifying source file "
      << __FILE__ << "<br>";
      crash << crash;
    } else
    { crash << "Something has gone wrong. Computation has not started, yet " << elapsedtime << " seconds have already passed."
//      << " allowed before a computation starts is " << onePredefinedCopyOfGlobalVariables.MaxWebWorkerRunTimeWithoutComputationStartedSecondsNonPositiveMeansNoLimit
      << " This may be an error in the web-server routines of the calculator!" << crash;
    }
  }
  RegisterFunctionCall(__FILE__, __LINE__, "exiting timer");
  theReport.SetStatus("Starting timer thread: exiting.");
  pthread_exit(NULL);
  return 0;
}
#endif

void CallSystemWrapper(const std::string& theCommand)
{ system(theCommand.c_str());
}
