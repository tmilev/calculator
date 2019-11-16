//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#include "calculator.h"

extern ProjectInformationInstance ProjectInfoCalculatorExpressionsTest;
ProjectInformationInstance ProjectInfoCalculatorExpressionsTest(__FILE__, "Calculator expression test. ");

bool Expression::Test::All() {
  Expression::Test::ToStringTest();
  return true;
}

bool Expression::Test::ToStringTestRecode(const std::string& inputHardCodedMustParse, Calculator& owner) {
  Expression parsed;
  if (!owner.Parse(inputHardCodedMustParse, parsed)) {
    crash << "Failed to parse hard-coded input string. " << crash;
  }
  std::string recoded = parsed.ToString();
  if (recoded != inputHardCodedMustParse) {
    crash << "Recoded string: " << recoded
    << " does not coincide with the original: "
    << inputHardCodedMustParse << "." << crash;
  }
  return true;
}

bool Expression::Test::ToStringTest() {
  Calculator tester;
  tester.initialize();
  Expression::Test::ToStringTestRecode("1+1", tester);
  return true;
}
