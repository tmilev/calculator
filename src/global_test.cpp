// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "general_logging_global_variables.h"

bool GlobalVariables::Test::All() {
  GlobalVariables::Test::Builds();
  return true;
}

bool GlobalVariables::Test::Builds() {
  global.CallSystemNoOutput("make clean", true);
  // This is the most restrictive build allowed:
  int result = global.CallSystemNoOutput("make -j20 noMongo=1 nossl=1 noPublicDomain=1", true);
  if (result != 0) {
    global.fatal << "Calculator noMongo nossl build generated errors. " << global.fatal;
  }
  global.CallSystemNoOutput("make clean", true);
  return true;
}
