// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "webserver.h"
#include "web_assembly.h"
int main(int argc, char **argv) {
#ifdef MACRO_use_wasm
  return WebAssembly::main(argc, argv);
#else
  return WebServer::main(argc, argv);
#endif
}
