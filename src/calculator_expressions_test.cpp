// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"

bool Expression::Test::all() {
  Calculator tester;
  tester.initialize();
  Expression::Test::toStringTestRecode(tester);
  return true;
}

bool Expression::Test::toStringTestRecodeOnce(
  const std::string& inputHardCodedMustParse, Calculator& ownerInitialized
) {
  Expression parsed;
  if (!ownerInitialized.parse(inputHardCodedMustParse, parsed)) {
    global.fatal << "Failed to parse hard-coded input string. " << global.fatal;
  }
  std::string recoded = parsed.toString();
  if (recoded != inputHardCodedMustParse) {
    global.fatal << "Recoded string: " << recoded
    << " does not coincide with the original: "
    << inputHardCodedMustParse << "." << global.fatal;
  }
  return true;
}

bool Expression::Test::toStringTestRecode(Calculator& ownerInitialized) {
  Expression::Test::toStringTestRecodeOnce("1+1", ownerInitialized);
  return true;
}
