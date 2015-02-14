//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".

#ifndef vpfHeaderSystemGlobalObjects_already_included
#define vpfHeaderSystemGlobalObjects_already_included
#include <iostream>
#include "vpfHeader1General0_General.h"
#include "vpfHeader3Calculator0_Interface.h"
#ifndef __DATE__
#define __DATE__ " unknown date "
#endif


static ProjectInformationInstance projectInfoInstanceCalculatorGlobalAndSystemHeader(__FILE__, "Global objects and system calls header");

extern GlobalVariables onePredefinedCopyOfGlobalVariables;
extern FormatExpressions consoleFormat;
extern Calculator theParser;

extern double GetElapsedTimeInSeconds();
extern void CallSystemWrapper(const std::string& theCommand);
extern void ignoreUserAbortSignal();

extern void SleepFunction(int nanoseconds);
extern void InitializeGlobalObjects();
extern void InitializeTimer();
extern void CreateTimerThread();

extern void* RunTimerVoidPtr(void* ptr);

#endif // vpfHeaderSystemGlobalObjects_already_included
