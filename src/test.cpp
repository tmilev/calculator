// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "crypto.h"
#include "webserver.h"

static ProjectInformationInstance projectInfoTestRoutinesCPP(__FILE__, "Testing routines. ");

int mainTest(List<std::string>& inputArguments) {
  global << "Testing ..." << logger::endL;
  (void) inputArguments;
  // theWebServer.InitializeGlobalVariables();
  JSData::Test::All();
  ASNObject::initializeNonThreadSafe();
  PrivateKeyRSA::Test::All();
  Crypto::Test::All();
  X509Certificate::Test::All();
  Expression::Test::All();
  LargeIntegerUnsigned::Test::All();
  Calculator::Test::All();
  global << logger::green << "All tests passed. " << logger::endL;
  return 0;
}
