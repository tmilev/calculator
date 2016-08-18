//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".

#ifndef vpfHeaderSystemGlobalObjects_already_included
#define vpfHeaderSystemGlobalObjects_already_included
#include <iostream>
#include "vpfHeader1General1_ListReferences.h"
#ifndef __DATE__
#define __DATE__ " unknown date "
#endif

static ProjectInformationInstance projectInfoInstanceCalculatorGlobalAndSystemHeader(__FILE__, "Global objects and system calls header");

class logger;
class Calculator;
extern logger theLog  ;
extern logger logBlock;
extern logger logIO   ;
extern logger logOpenSSL   ;
extern logger logProcessKills   ;
extern logger logSocketAccept;
extern logger logProcessStats;
extern logger logPlumbing;
extern FormatExpressions consoleFormat;
extern Calculator theParser;

extern double GetElapsedTimeInSeconds();
extern void CallSystemWrapperNoOutput(const std::string& theCommand);
extern std::string CallSystemWrapperReturnStandardOutput(const std::string& theCommand);
extern void CallChDirWrapper(const std::string& theCommand);
extern void ignoreUserAbortSignal();

extern void SleepFunction(int nanoseconds);
extern void InitializeGlobalObjects();
extern void InitializeTimer(void* desiredStartTime=0);
extern void CreateTimerThread();

extern void* RunTimerVoidPtr(void* ptr);

#endif // vpfHeaderSystemGlobalObjects_already_included
