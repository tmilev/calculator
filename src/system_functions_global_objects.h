// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#ifndef vpfHeaderSystemGlobalObjects_already_included
#define vpfHeaderSystemGlobalObjects_already_included
#include <iostream>
#include "general_list_references.h"
#ifndef __DATE__
#define __DATE__ " unknown date "
#endif

class Calculator;

extern int getElapsedMilliseconds();
extern int externalCommandNoOutput(const std::string& theCommand);
extern std::string externalCommandReturnStandartOut(const std::string& theCommand);
extern int externalCommandStreamOutput(const std::string& theCommand);
extern void callChDirWrapper(const std::string& theCommand);
extern void ignoreUserAbortSignal();

extern void initializeGlobalObjects();
extern void createTimerThread();
extern void initializeTimer();

extern void* runTimerVoidPointer(void* ptr);

#endif // vpfHeaderSystemGlobalObjects_already_included
