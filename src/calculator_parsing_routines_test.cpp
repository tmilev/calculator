// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "string_constants.h"

bool Calculator::Test::all() {
  Calculator tester;
  tester.initialize(Calculator::Mode::full);
  Calculator::Test::cacheWorks();
  Calculator::Test::loopDetection();
  Calculator::Test::checkBuiltInInitializations(tester);
  Calculator::Test::parseAllExamples(tester);
  Calculator::Test::numberOfTestFunctions(tester);
  Calculator::Test::parseDecimal(tester);
  Calculator::Test::builtInFunctionsABTest(tester);
  return true;
}

bool Calculator::Test::checkBuiltInInitializations(Calculator& ownerInitialized) {
  if (!ownerInitialized.checkPredefinedFunctionNameRepetitions()) {
    global.fatal << "Predefined function name repetitions." << global.fatal;
    return false;
  }
  if (!ownerInitialized.checkOperationHandlers()) {
    global.fatal << "Operation handler checks failed." << global.fatal;
    return false;
  }
  return true;
}

bool Calculator::Test::numberOfTestFunctions(Calculator& ownerInitialized) {
  int numberOfBuiltInFunction = ownerInitialized.getNumberOfBuiltInFunctions();
  if (numberOfBuiltInFunction <= 0) {
    global.fatal << "Calculator built in functions: " << numberOfBuiltInFunction << global.fatal;
  }
  return true;
}

bool Calculator::Test::cacheWorks() {
  Calculator calculator;
  calculator.initialize(Calculator::Mode::educational);
  int64_t startTime = global.getElapsedMilliseconds();
  calculator.evaluate("f{}0=0;f{}1=1;f{}{{x}}=f{}(x-1)+f{}(x-2);f{}300");
  int64_t duration = global.getElapsedMilliseconds() - startTime;
  int64_t maximumDuration = 3000;
  if (duration > maximumDuration) {
    global.fatal << "Large cacheable computation took too long: "
    << duration
    << " ms, maximum allowed " << maximumDuration << ". "
    << "Computed: "
    << calculator.programExpression.toString()
    << "Perhaps the caches are not functioning correctly?" << global.fatal;
  }
  return true;
}

bool Calculator::Test::loopDetection() {
  Calculator::Test::loopDetectionCycle();
  Calculator::Test::loopDetectionEverExpanding();
  return true;
}

bool Calculator::Test::loopDetectionEverExpanding() {
  Calculator calculator;
  calculator.initialize(Calculator::Mode::educational);
  calculator.evaluate("x=x+1;x");
  if (!calculator.flagAbortComputationASAP) {
    global.fatal
    << "Expanding cycle did not generate error as expected. "
    << "Instead, generated result was: " << calculator.programExpression.toString()
    << global.fatal;
  }
  return true;
}

bool Calculator::Test::loopDetectionCycle() {
  Calculator calculator;
  calculator.initialize(Calculator::Mode::educational);
  calculator.evaluate(
    "TurnOffRules(CheckAutoEquality);\n"
    "x=x;\n"
    "x"
  );
  if (!calculator.flagAbortComputationASAP) {
    global.fatal
    << "Simple infinite loop did not generate error as expected. Result: "
    << calculator.programExpression.toString()
    << global.fatal;
  }
  return true;
}

bool Calculator::Test::parseAllExamples(Calculator& ownerInitialized) {
  MacroRegisterFunctionWithName("Calculator::Test::parseAllExamples");
  for (int i = 0; i < ownerInitialized.numberOfPredefinedAtoms; i ++) {
    MemorySaving<Calculator::OperationHandlers>& operationPointer =
    ownerInitialized.operations.values[i];
    if (operationPointer.isZeroPointer()) {
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
      if (!ownerInitialized.parse(currentHandler.theExample, false, notUsed)) {
        global.fatal << "Failed to parse built-in example for rule: "
        << currentHandler.calculatorIdentifier
        << ". The example was: " << currentHandler.theExample << ". "
        << global.fatal;
      }
    }
  }
  return true;
}

bool Calculator::Test::parseDecimal(Calculator& ownerInitialized) {
  std::string mustEvaluateToZero = "2.01 - 201/100";
  ownerInitialized.evaluate(mustEvaluateToZero);
  if (ownerInitialized.programExpression.toString() != "0") {
    global.fatal << "Expression: " << mustEvaluateToZero << " evaluates to: "
    << ownerInitialized.programExpression.toString() << " instead of 0. " << global.fatal;
  }
  std::string mustEvaluateToOne =
  "100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 - 10^128 + 1";
  ownerInitialized.evaluate(mustEvaluateToOne);
  if (ownerInitialized.programExpression.toString() != "1") {
    std::cout << "Must crash!\n";
    global.fatal << "Expression: " << mustEvaluateToOne << " evaluates to: "
    << ownerInitialized.programExpression.toString() << " instead of 1. " << global.fatal;
  }

  return true;
}

bool Calculator::Test::builtInFunctionsABTest(Calculator& ownerInitialized) {
  Calculator::Test test(ownerInitialized);
  test.calculatorTestPrepare();
  if (!test.calculatorTestRun()) {
    std::stringstream crashFileWriteReport;
    std::stringstream crashFile;
    crashFile << "<html><link "
    << "type = 'text/css' "
    << "rel = 'stylesheet' "
    << "href = '" << WebAPI::request::calculatorCSS << "'>"
    << "<body>" << test.reportHtml << "</body></html>";

    FileOperations::writeFileVirual("output/crash_test.html", crashFile.str(), &crashFileWriteReport);
    global.fatal << crashFileWriteReport.str() << "Calculator AB test failed with " << test.inconsistencies
    << " inconsistencies. See report html in file:\n"
    << "https://localhost:8166/output/crash_test.html\n" << global.fatal;
  }
  return true;
}

bool Calculator::Examples::Test::all() {
  Calculator::Examples::Test::compose();
  return true;
}

bool Calculator::Examples::Test::compose() {
  Calculator calculator;
  calculator.initialize(Calculator::Mode::full);
  calculator.examples.writeExamplesReadme();
  return true;
}
