 // The current file is licensed under the license terms found in the main header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "web_assembly.h"
#include "general_logging_global_variables.h"
#include "calculator_interface.h"
#include "string_constants.h"
#include "system_functions_global_objects.h"
#include <string.h>

#ifdef MACRO_use_wasm

#include "../emsdk/upstream/emscripten/system/include/emscripten.h"

#endif

int WebAssembly::numberOfMainLoopCalls = 0;
extern "C" {
  char* callCalculator(char* inputBuffer) {
    global.flagRunningWebAssembly = true;
    initializeGlobalObjects();
    std::string input(inputBuffer);
    Calculator calculator;
    calculator.initialize(Calculator::Mode::full);
    JSData result;
    calculator.evaluate(input);
    result = calculator.toJSONOutputAndSpecials();
    result[WebAPI::result::commentsGlobal] = global.comments.getCurrentReset();
    std::string resultString = result.toString();
    char* buffer = new char[resultString.size() + 1];
    strcpy(buffer, resultString.c_str());
    return buffer;
  }
}
int WebAssembly::main(int argc, char** argv) {
  (void) argc;
  (void) argv;
  global.flagRunningWebAssembly = true;
  std::cout << "Web assembly main function does nothing; exiting." << std::endl
  ;
  return 0;
}

void WebAssembly::mainLoop() {
  WebAssembly::numberOfMainLoopCalls ++;
  std::cout
  << "Main loop calls: "
  << WebAssembly::numberOfMainLoopCalls
  << std::endl;
}
