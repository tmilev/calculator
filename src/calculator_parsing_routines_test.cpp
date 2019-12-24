// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"

static ProjectInformationInstance projectInfoCalculatorParsingRoutinesTestCPP(__FILE__, "Test of calculator input parsing routines. ");

bool Calculator::Test::All() {
  Calculator tester;
  tester.initialize();
  Calculator::Test::ParseAllExamples(tester);
  Calculator::Test::NumberOfTestFunctions(tester);
  Calculator::Test::ParseDecimal(tester);
  Calculator::Test::BuiltInFunctionsABTest(tester);
  return true;
}

bool Calculator::Test::NumberOfTestFunctions(Calculator& ownerInitialized) {
  int numberOfBuiltInFunction = ownerInitialized.GetNumBuiltInFunctions();
  if (numberOfBuiltInFunction <= 0) {
    global.fatal << "Calculator built in functions: " << numberOfBuiltInFunction << global.fatal;
  }
  return true;
}

bool Calculator::Test::ParseAllExamples(Calculator& ownerInitialized) {
  MacroRegisterFunctionWithName("Calculator::Test::ParseAllExamples");
  List<std::string> toBeParsed;
  for (int i = 0; i < ownerInitialized.NumPredefinedAtoms; i ++) {
    MemorySaving<Calculator::AtomHandler>& operationPointer = ownerInitialized.operations.theValues[i];
  }
  return true;
}

bool Calculator::Test::ParseDecimal(Calculator& ownerInitialized) {
  std::string mustEvaluateToZero = "2.01 - 201/100";
  ownerInitialized.Evaluate(mustEvaluateToZero);
  if (ownerInitialized.theProgramExpression.ToString() != "0") {
    global.fatal << "Expression: " << mustEvaluateToZero << " evaluates to: "
    << ownerInitialized.theProgramExpression.ToString() << " instead of 0. " << global.fatal;
  }
  std::string mustEvaluateToOne =
  "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 - 10^128 + 1";
  ownerInitialized.Evaluate(mustEvaluateToOne);
  if (ownerInitialized.theProgramExpression.ToString() != "1") {
    std::cout << "Must crash!\n";
    global.fatal << "Expression: " << mustEvaluateToOne << " evaluates to: "
    << ownerInitialized.theProgramExpression.ToString() << " instead of 1. " << global.fatal;
  }

  return true;
}

bool Calculator::Test::BuiltInFunctionsABTest(Calculator& ownerInitialized) {
  Calculator::Test test(ownerInitialized);
  if (!test.CalculatorTestRun()) {
    global.fatal << "Calculator AB test failed. " << global.fatal;
  }
  return true;
}
