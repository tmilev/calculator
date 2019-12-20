// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"

static ProjectInformationInstance projectInfoCalculatorExpressionsTestCPP(__FILE__, "Calculator expression test. ");

bool Expression::Test::All() {
  Calculator tester;
  tester.initialize();
  Expression::Test::ToStringTest(tester);
  return true;
}

bool Expression::Test::ToStringTestRecode(const std::string& inputHardCodedMustParse, Calculator& ownerInitialized) {
  Expression parsed;
  if (!ownerInitialized.Parse(inputHardCodedMustParse, parsed)) {
    global.fatal << "Failed to parse hard-coded input string. " << global.fatal;
  }
  std::string recoded = parsed.ToString();
  if (recoded != inputHardCodedMustParse) {
    global.fatal << "Recoded string: " << recoded
    << " does not coincide with the original: "
    << inputHardCodedMustParse << "." << global.fatal;
  }
  return true;
}

bool Expression::Test::ToStringTest(Calculator& ownerInitialized) {
  Expression::Test::ToStringTestRecode("1+1", ownerInitialized);
  return true;
}
