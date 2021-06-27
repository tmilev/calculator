// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "web_assembly.h"
#include "general_logging_global_variables.h"
#ifdef MACRO_use_wasm
#include "../emsdk/upstream/emscripten/system/include/emscripten.h"
#endif

int WebAssembly::numberOfMainLoopCalls = 0;

extern "C" {
  int callCalculator(char* inputBuffer) {
    std::string input(inputBuffer);
    std::cout << "Hello world: input: " << input << std::endl;
    return - 1;
  }
}

int WebAssembly::main(int argc, char **argv) {
  (void) argc;
  (void) argv;
  global.flagRunningWebAssembly = true;
#ifdef MACRO_use_wasm
  emscripten_set_main_loop(WebAssembly::mainLoop, 4, false);
#endif
  return 0;
}

void WebAssembly::mainLoop() {
  WebAssembly::numberOfMainLoopCalls ++;
  // std::string input;
  // std::cin >> input;
  std::cout << "Main loop calls: " << WebAssembly::numberOfMainLoopCalls << std::endl;
  // std::cout << "Input: " << input << std::endl;
}


