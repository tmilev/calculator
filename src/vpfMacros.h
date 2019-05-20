//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef MacroVPFMacros_included
#define MacroVPFMacros_included

//External dependencies:
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <fstream>

#ifdef WIN32
//have to disable C4100 in VS because it warns me on passing non-used parameters to my functions.
//Those of course are passed to facilitate future extensions of functionality.
#pragma warning(disable:4100)//warning C4100: non-referenced formal parameter
//The below causes problems in VS with my debugging code (which I comment/uncomment often).
#pragma warning(disable:4189)//warning 4189: variable initialized but never used
#endif
//External dependencies end.


//-----------------------------------------------------------
//-----------------Preprocessor macros-----------------------
//-----------------------------------------------------------
//The following option turns on a RAM memory limit safeguard, as well as
// very limited memory allocation statistics. See
//ParallelComputing::CheckPointerCounters. The project should compile and link just fine with
//the following 3 lines commented out.
//If not, it's a bug.
#ifndef AllocationLimitsSafeguard
#define AllocationLimitsSafeguard
#endif


//////////////////////Inline macros.
//Important: Inline macros are expected to follow the same syntactical rules/semantics as
//regular C++ code.
//It should be possible to substitute all macros present here with global C++ functions.
//If not, the macros are criminal and need to be fixed.
////////////////////////////////////////////
//The following option turns on counters for a number of mathematical operations.
//In general, it should not yield an essential slow-down.
//If you do not want to use these counters, turn them off by commenting the first definition
//and uncommenting the second, and vice versus.
#ifndef MacroIncrementCounter
#define MacroIncrementCounter(x) x ++
//#define MacroIncrementCounter(x)
#endif

//The following turns on a custom, one-line-per-function-opt-in 100% portable stack tracer.
//The tracer does yield a slow-down, use with caution: it pushes two strings and an int in
//and out of a custom defined stack
//(however, memory allocation of the stack storage is not an issue).
//If you want want to turn the tracer on/off, use the commented lines to substitute the
//non-commented lines.
//The below code might end up being substituted with a regular class definition.
#ifndef MacroRegisterFunctionWithName
#define MacroRegisterFunctionWithName(FunctionName) RegisterFunctionCall theFunctionCallRegistration(__FILE__, __LINE__, FunctionName)
//#define MacroRegisterFunctionWithName(x)
#endif

//-----------------------------------------------------------
//-----------------Preprocessor macros end-------------------
//-----------------------------------------------------------

class ProjectInformationInstance {
  public:
  ProjectInformationInstance(const char* fileName, const std::string& fileDescription);
};
class GlobalVariables;
extern GlobalVariables theGlobalVariables;

static ProjectInformationInstance vpfMacrosHprojectInstance(
  __FILE__, "Header, External includes + macros. Macro/build configuration."
);

class Crasher {
  public:
  std::stringstream crashReport;
  std::stringstream crashReportHtml;
  std::stringstream crashReportConsolE;
  std::stringstream crashReportFile;
  void (*CleanUpFunction)();
  bool flagCrashInitiateD; //<-we crash only once, and we do not resume execution after a crash
  bool flagFinishingCrash; //<-we crash only once, and we do not resume execution after a crash
  Crasher();
  void FirstRun();
  static std::string GetStackTraceEtcErrorMessageHTML();
  static std::string GetStackTraceEtcErrorMessageConsole();
  Crasher& operator<<(const Crasher& dummyCrasherSignalsActualCrash);
  template <class AnyObject>
  Crasher& operator<<(const AnyObject& input) {
    this->FirstRun();
    this->crashReport << input;
    return *this;
  }
};

class StdoutClass {
public:
  template <typename anyType>
  StdoutClass& operator<<(const anyType& toBePiped) {
    if (this->theOutputFunction == 0) {
      std::cout << toBePiped;
    } else {
      std::stringstream out;
      out << toBePiped;
      this->theOutputFunction(out.str());
    }
    return *this;
  }
  template <typename anyType>
  StdoutClass& operator<<(anyType& toBePiped) {
    if (this->theOutputFunction == 0) {
      std::cout << toBePiped;
    } else {
      std::stringstream out;
      out << toBePiped;
      this->theOutputFunction(out.str());
    }
    return *this;
  }
  void (*theOutputFunction)(const std::string& stringToOutput);
  void (*flushOutputFunction)();
  StdoutClass(): theOutputFunction(0), flushOutputFunction(0){}
  void Flush() {
    if (this->flushOutputFunction == 0) {
      std::cout.flush();
      return;
    }
    this->flushOutputFunction();
  }
};

extern Crasher crash;
extern StdoutClass stOutput;
#endif
