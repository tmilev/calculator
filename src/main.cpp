#include "webserver.h"
#include "web_assembly.h"

int main(int argc, char** argv) {
  bool useWasm = false;
#ifdef MACRO_use_wasm
  useWasm = true;
#endif
  if (useWasm) {
    return WebAssembly::main(argc, argv);
  }
  return WebServer::main(argc, argv);
}
