 // The current file is licensed under the license terms found in the main header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_macros_ALREADY_INCLUDED
#define header_macros_ALREADY_INCLUDED

#include <sstream>
#include <cstdlib>
#include <iostream>
#include <fstream>

// -----------------------------------------------------------
// -----------------Preprocessor macros-----------------------
// -----------------------------------------------------------
// The following option turns on a RAM memory limit safeguard, as well as
// very limited memory allocation statistics. See
// GlobalStatistics::checkPointerCounters.
// The project should compile and link just fine with
// the following 3 lines commented out.
// If not, it's a bug.
#ifndef AllocationLimitsSafeguard
#define AllocationLimitsSafeguard
#endif

// ////////////////////Inline macros.
// Important: Inline macros are expected to follow the same syntactical
// rules/semantics as regular C++ code.
// It should be possible to substitute all macros present here with global C++
// functions. If not, the macros are criminal and need to be fixed.
//
// The following option turns on counters for a number of mathematical
// operations.
// In general, it should not yield an essential slow-down.
// If you do not want to use these counters, turn them off by commenting the
// first definition
// and uncommenting the second, and vice versus.
#ifndef MacroIncrementCounter
#define MacroIncrementCounter(x) x ++

// #define MacroIncrementCounter(x)
#endif // header_macros_ALREADY_INCLUDED

// The following turns on a custom, one-line-per-function-opt-in 100% portable
// stack tracer.
// The tracer does yield a slow-down, use with caution: it pushes two strings
// and an int in and out of a custom defined stack
// (however, memory allocation of the stack storage is not an issue).
// If you want want to turn the tracer on/off, use the commented lines to
// substitute the non-commented lines.
// The below code might end up being substituted with a regular class
// definition.
#ifndef STACK_TRACE
#define STACK_TRACE(FunctionName) RegisterFunctionCall functionCallRegistration(__FILE__, __LINE__, FunctionName)

// #define STACK_TRACE(x)
#endif

// -----------------------------------------------------------
// -----------------Preprocessor macros end-------------------
// -----------------------------------------------------------
class GlobalVariables;

extern GlobalVariables global;
void fatalCrash(const std::string& input);
#endif
