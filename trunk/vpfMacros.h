//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef MacroVPFMacros_included
#define MacroVPFMacros_included

//External dependencies:
#include <assert.h>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <math.h>

#ifndef WIN32
#include <pthread.h>
//#include <execinfo.h>
#else
 #include<windows.h>
// #include <unistd.h>
// #include <Pthread.h>
#endif

#ifdef WIN32
//have to disable C4100 in VS because it warns me on passing non-used parameters to my functions.
//Those of course are passed to facilitate future extensions of functionality.
#pragma warning(disable:4100)//warning C4100: non-referenced formal parameter
//The below causes problems in VS with my debugging code (which I comment/uncomment often).
#pragma warning(disable:4189)//warning 4189: variable initialized but never used
#endif
//External dependencies end.


//-----------------------------------------------------------
//-----------------Preprocessor macros--------------
//-----------------------------------------------------------
//the following option turns on a RAM memory limit safeguard, as well as
// very limited memory allocation statistics. See
//ParallelComputing::CheckPointerCounters. The project should compile and link just fine with the following 3 lines commented out.
//If not, it's a bug.
#ifndef CGIversionLimitRAMuse
#define CGIversionLimitRAMuse
#endif

//the following option turns on counters for a number of mathematical operations.
//In general, it should not yield an essential slow-down.
//If you do not want to use these counters, turn them off by commenting the first definition and uncommenting
//the second, and vice versus.
#ifndef MacroIncrementCounter
#define MacroIncrementCounter(x) x++
//#define MacroIncrementCounter(x)
#endif

//the following option turns on a custom 100% portable by-hand stack tracer.
//The tracer does yield a slow-down, use with caution: it pushes two strings and an int on the stack (memory allocation is not an issue)
//If you want not to use the tracer, substitute use the commented lines to substitute the non-commented lines.
#ifndef MacroRegisterFunctionWithName
#define MacroRegisterFunctionWithName(FunctionName) RegisterFunctionCall theFunctionCallRegistration(__FILE__, __LINE__, FunctionName)
//#define MacroRegisterFunctionWithName(x)
#endif
#ifndef MacroRegisterFunction
#define MacroRegisterFunction RegisterFunctionCall theFunctionCallRegistration(__FILE__, __LINE__)
//#define MacroRegisterFunction
#endif

#define ANNOYINGSTATISTICS //std::cout << "<hr>" << "Time elapsed at file " << __FILE__ << " line " << __LINE__ << ": " << theGlobalVariables.GetElapsedSeconds()
//-----------------------------------------------------------
//-----------------Preprocessor macros end--------
//-----------------------------------------------------------

class ProjectInformationInstance
{
  public:
  ProjectInformationInstance(const char* fileName, const std::string& fileDescription)
  ;
};

static ProjectInformationInstance vpfMacrosHprojectInstance(__FILE__, "External includes + macros. Macro/build configuration.");

#endif
