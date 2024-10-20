#include "calculator_interface.h"
#include "general_logging_global_variables.h"
#include "string_constants.h"
#include "system_functions_global_objects.h"
#include "web_assembly.h"
#include <iostream>
#include <string.h>

int WebAssembly::numberOfMainLoopCalls = 0;
extern "C" {
  char* callCalculator(char* inputBuffer) {
    global.runMode = GlobalVariables::RunMode::webAssembly;
    initializeGlobalObjects();
    std::string input(inputBuffer);
    Calculator calculator;
    calculator.initialize(Calculator::Mode::full);
    JSData result;
    calculator.evaluate(input);
    result = calculator.toJSONOutputAndSpecials();
    result[WebAPI::Result::commentsGlobal] = global.comments.getCurrentReset();
    std::string resultString = result.toString();
    char* buffer = new char[resultString.size() + 1];
    for (unsigned i = 0; i < resultString.size(); i ++) {
      buffer[i] = resultString[i];
    }
    buffer[resultString.size()] = '\0';
    return buffer;
  }
}

int WebAssembly::main(int argc, char** argv) {
  (void) argc;
  (void) argv;
  global.runMode = GlobalVariables::RunMode::webAssembly;
  std::cout
  << "Web assembly main function does nothing; exiting."
  << std::endl;
  return 0;
}

void WebAssembly::mainLoop() {
  WebAssembly::numberOfMainLoopCalls ++;
  std::cout
  << "Main loop calls: "
  << WebAssembly::numberOfMainLoopCalls
  << std::endl;
}
