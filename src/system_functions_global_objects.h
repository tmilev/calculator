#ifndef header_system_functions_global_objects_ALREADY_INCLUDED
#define header_system_functions_global_objects_ALREADY_INCLUDED

#include <string>

#ifndef __DATE__
#define __DATE__ " unknown date "
#endif

class Calculator;

extern int getElapsedMilliseconds();
extern int externalCommandNoOutput(const std::string& command);
extern std::string externalCommandReturnStandardOut(
  const std::string& command
);
extern int externalCommandStreamOutput(const std::string& command);
extern void callChDirWrapper(const std::string& command);
extern void ignoreUserAbortSignal();
extern void initializeGlobalObjects();
extern void createTimerThread();
extern void initializeTimer();
extern void* runTimerVoidPointer(void* ptr);
#endif // header_system_functions_global_objects_ALREADY_INCLUDED
