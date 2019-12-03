// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#ifndef vpfHeaderSystemGlobalObjects_already_included
#define vpfHeaderSystemGlobalObjects_already_included
#include <iostream>
#include "general_list_references.h"
#ifndef __DATE__
#define __DATE__ " unknown date "
#endif

static ProjectInformationInstance projectInfoInstanceCalculatorGlobalAndSystemHeader(
  __FILE__, "Global objects and system calls header"
);

class logger;
class Calculator;
extern logger logWorker  ;
extern logger logServerMonitor  ;
extern logger logHttpErrors;
extern logger logBlock;
extern logger logIO   ;
extern logger logOpenSSL   ;
extern logger logProcessKills   ;
extern logger logSocketAccept;
extern logger logProcessStats;
extern logger logPlumbing;
extern logger logEmail  ;
extern logger logServer  ;
extern logger logSuccessfulForks  ;
extern FormatExpressions consoleFormat;
extern Calculator* theParser;

extern int GetElapsedMilliseconds();
extern int CallSystemWrapperNoOutput(const std::string& theCommand);
extern std::string CallSystemWrapperReturnStandardOutput(const std::string& theCommand);
extern void CallChDirWrapper(const std::string& theCommand);
extern void ignoreUserAbortSignal();

extern void InitializeGlobalObjects();
extern void CreateTimerThread();
extern void InitializeTimeR();

extern void* RunTimerVoidPtr(void* ptr);

#endif // vpfHeaderSystemGlobalObjects_already_included
