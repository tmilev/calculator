// The current file is licensed under the license terms found in the main header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "general_logging_global_variables.h"
#include "general_file_operations_encodings.h"

bool GlobalVariables::Test::all() {
  GlobalVariables::Test::builds();
  return true;
}

bool GlobalVariables::Test::builds() {
  global.externalCommandNoOutput("make clean", true);
  if (FileOperations::fileExistsVirtual("calculator")) {
    global.fatal
    << "Command make clean did not remove the calculator executable. "
    << global.fatal;
  }
  // This is the most restrictive build allowed:
  int result =
  global.externalCommandNoOutput(
    "make -j20 noMongo=1 nossl=1 noPublicDomain=1", true
  );
  if (result != 0) {
    global
    << Logger::red
    << "Calculator most restrictive build generated errors. "
    << Logger::endL;
  }
  std::stringstream commentsOnFailure;
  if (
    !FileOperations::fileExistsVirtual(
      "calculator", true, true, &commentsOnFailure
    )
  ) {
    global.fatal
    << "Failed to build the calculator executable. "
    << commentsOnFailure.str()
    << global.fatal;
  }
  global.externalCommandNoOutput("make clean", true);
  return true;
}

