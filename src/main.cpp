// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "webserver.h"

static ProjectInformationInstance projectInfoInstanceMain(__FILE__, "Main function.");

int main(int argc, char **argv) {
  return WebServer::main(argc, argv);
}
