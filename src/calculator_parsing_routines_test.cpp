// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"

bool Calculator::Test::all() {
  Calculator tester;
  tester.initialize();
  Calculator::Test::CheckBuiltInInitializations(tester);
  Calculator::Test::ParseAllExamples(tester);
  Calculator::Test::NumberOfTestFunctions(tester);
  Calculator::Test::ParseDecimal(tester);
  Calculator::Test::BuiltInFunctionsABTest(tester);
  return true;
}

bool Calculator::Test::CheckBuiltInInitializations(Calculator& ownerInitialized) {
  if (!ownerInitialized.CheckPredefinedFunctionNameRepetitions()) {
    global.fatal << "Predefined function name repetitions." << global.fatal;
    return false;
  }
  if (!ownerInitialized.CheckOperationHandlers()) {
    global.fatal << "Operation handler checks failed." << global.fatal;
    return false;
  }
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
  for (int i = 0; i < ownerInitialized.NumPredefinedAtoms; i ++) {
    MemorySaving<Calculator::OperationHandlers>& operationPointer =
    ownerInitialized.operations.theValues[i];
    if (operationPointer.IsZeroPointer()) {
      continue;
    }
    Calculator::OperationHandlers& operationHandlers = operationPointer.getElement();
    List<Function> handlers = operationHandlers.mergeHandlers();
    for (int i = 0; i < handlers.size; i ++) {
      Function& currentHandler = handlers[i];
      if (currentHandler.theExample == "") {
        global.fatal << "Empty example for operation: "
        << currentHandler.calculatorIdentifier << global.fatal;
      }
      Expression notUsed;
      if (!ownerInitialized.parse(currentHandler.theExample, notUsed)) {
        global.fatal << "Failed to parse built-in example for rule: "
        << currentHandler.calculatorIdentifier
        << ". The example was: " << currentHandler.theExample << ". "
        << global.fatal;
      }
    }
  }
  return true;
}

bool Calculator::Test::ParseDecimal(Calculator& ownerInitialized) {
  std::string mustEvaluateToZero = "2.01 - 201/100";
  ownerInitialized.evaluate(mustEvaluateToZero);
  if (ownerInitialized.theProgramExpression.toString() != "0") {
    global.fatal << "Expression: " << mustEvaluateToZero << " evaluates to: "
    << ownerInitialized.theProgramExpression.toString() << " instead of 0. " << global.fatal;
  }
  std::string mustEvaluateToOne =
  "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 - 10^128 + 1";
  ownerInitialized.evaluate(mustEvaluateToOne);
  if (ownerInitialized.theProgramExpression.toString() != "1") {
    std::cout << "Must crash!\n";
    global.fatal << "Expression: " << mustEvaluateToOne << " evaluates to: "
    << ownerInitialized.theProgramExpression.toString() << " instead of 1. " << global.fatal;
  }

  return true;
}

bool Calculator::Test::BuiltInFunctionsABTest(Calculator& ownerInitialized) {
  Calculator::Test test(ownerInitialized);
  test.CalculatorTestPrepare();
  if (!test.CalculatorTestRun()) {
    std::stringstream crashFileWriteReport;
    std::stringstream crashFile;
    crashFile << "<html><link "
    << "type = 'text/css' "
    << "rel = 'stylesheet' "
    << "href = '../calculator-html/styleCalculator.css'>"
    << "<body>" << test.reportHtml << "</body></html>";

    FileOperations::WriteFileVirual("output/crash_test.html", crashFile.str(), &crashFileWriteReport);
    global.fatal << crashFileWriteReport.str() << "Calculator AB test failed with " << test.inconsistencies
    << " inconsistencies. See report html in file output/crash_test.html. " << global.fatal;
  }
  return true;
}
