// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "web_assembly.h"
#include "general_logging_global_variables.h"

int WebAssembly::main(int argc, char **argv) {
  (void) argc;
  (void) argv;
  global.flagRunningWebAssembly = true;
  global << "Hello world" << Logger::endL;
  std::string command;
  global << "Your command: " << command << Logger::endL;
  return 0;
}

extern "C" {
int myFunction(char* stringInput, int length) {
  std::cout << "Hello world";
  std::string input(stringInput, length);
    std::cout << "MyFunction was called. Length: " << length << "; input: " << input;
    return -15;
}
}
