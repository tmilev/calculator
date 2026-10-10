#include "calculator_interface.h"
#include "test.h"

namespace Testing {

bool CalculatorTest::all(bool updateABTestFile) {
  Calculator tester;
  tester.initialize(Calculator::Mode::full);
  CalculatorTest::parseQuotes(tester);
  CalculatorTest::cacheWorks();
  CalculatorTest::loopDetection();
  CalculatorTest::checkBuiltInInitializations(tester);
  CalculatorTest::parseAllExamples(tester);
  CalculatorTest::numberOfTestFunctions(tester);
  CalculatorTest::parseDecimal(tester);
  CalculatorTest::builtInFunctionsABTest(tester, updateABTestFile);
  return true;
}
}
