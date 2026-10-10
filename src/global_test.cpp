#include "general_file_operations_encodings.h"
#include "general_logging_global_variables.h"
#include "test.h"

namespace Testing {

bool GlobalVariablesTest::all() {
  GlobalVariablesTest::builds();
  return true;
}

bool GlobalVariablesTest::webAssemblyBuild() {
  int wasmResult =
  global.externalCommandNoOutput("make -j20 wasm=1 optimize=1", true);
  if (wasmResult != 0) {
    global << Logger::red << "Wasm make was not successful. " << Logger::endL;
  }
  return true;
}

bool GlobalVariablesTest::oneMakeBuild(const std::string& buildCommand) {
  global.externalCommandNoOutput("make clean", true);
  if (FileOperations::fileExistsVirtual("calculator")) {
    global.fatal
    << "Command make clean did not remove the calculator executable. "
    << global.fatal;
  }
  // This is the most restrictive build allowed:
  int result = global.externalCommandNoOutput(buildCommand, true);
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
  return true;
}

bool GlobalVariablesTest::builds() {
  GlobalVariablesTest::oneMakeBuild(
    "make -j20 noMongo=1 nossl=1 noPublicDomain=1 optimize=1"
  );
  GlobalVariablesTest::oneMakeBuild("make -j20 llvm=1");
  return true;
}
} // namespace Testing.
