#include "calculator_interface.h"
#include "general_file_operations_encodings.h"
#include "test.h"
#include <iostream>


bool CalculatorTest::checkBuiltInInitializations(
  Calculator& ownerInitialized
) {
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

bool CalculatorTest::numberOfTestFunctions(Calculator& ownerInitialized) {
  int numberOfBuiltInFunction = ownerInitialized.getNumberOfBuiltInFunctions();
  if (numberOfBuiltInFunction <= 0) {
    global.fatal
    << "Calculator built in functions: "
    << numberOfBuiltInFunction
    << global.fatal;
  }
  return true;
}

bool CalculatorTest::cacheWorks() {
  Calculator calculator;
  calculator.initialize(Calculator::Mode::educational);
  int64_t startTime = global.getElapsedMilliseconds();
  calculator.evaluate("f{}0=0;f{}1=1;f{}{{x}}=f{}(x-1)+f{}(x-2);f{}300");
  int64_t duration = global.getElapsedMilliseconds() - startTime;
  int64_t maximumDuration = 3000;
  if (duration > maximumDuration) {
    global.fatal
    << "Large cacheable computation took too long: "
    << duration
    << " ms, maximum allowed "
    << maximumDuration
    << ". "
    << "Computed: "
    << calculator.programExpression.toString()
    << "Perhaps the caches are not functioning correctly?"
    << global.fatal;
  }
  return true;
}

bool CalculatorTest::loopDetection() {
  CalculatorTest::loopDetectionCycle();
  CalculatorTest::loopDetectionEverExpanding();
  return true;
}

bool CalculatorTest::loopDetectionEverExpanding() {
  Calculator calculator;
  calculator.initialize(Calculator::Mode::educational);
  calculator.evaluate("x=x+1;x");
  if (!calculator.flagAbortComputationASAP) {
    global.fatal
    << "Expanding cycle did not generate error as expected. "
    << "Instead, generated result was: "
    << calculator.programExpression.toString()
    << global.fatal;
  }
  return true;
}

bool CalculatorTest::loopDetectionCycle() {
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

bool CalculatorTest::parseAllExamples(Calculator& ownerInitialized) {
  STACK_TRACE("Calculator::Test::parseAllExamples");
  for (int i = 0; i < ownerInitialized.numberOfPredefinedAtoms; i ++) {
    MemorySaving<Calculator::OperationHandlers>& operationPointer =
    ownerInitialized.operations.values[i];
    if (operationPointer.isZeroPointer()) {
      continue;
    }
    Calculator::OperationHandlers& operationHandlers =
    operationPointer.getElement();
    List<Function> handlers = operationHandlers.mergeHandlers();
    for (int i = 0; i < handlers.size; i ++) {
      Function& currentHandler = handlers[i];
      if (currentHandler.example == "") {
        global.fatal
        << "Empty example for operation: "
        << currentHandler.calculatorIdentifier
        << global.fatal;
      }
      Expression notUsed;
      if (
        !ownerInitialized.parser.parse(currentHandler.example, false, notUsed)
      ) {
        global.fatal
        << "Failed to parse built-in example for rule: "
        << currentHandler.calculatorIdentifier
        << ". The example was:\n"
        << currentHandler.example
        << "\nError string:\n"
        << ownerInitialized.parser.toStringSyntacticStackHTMLSimple()
        << global.fatal;
      }
    }
  }
  return true;
}

bool CalculatorTest::parseQuotes(Calculator& ownerInitialized) {
  std::string input = "\"\\\"\\\\\\\"\"";
  std::string expected = "\"\\\"\\\\\\\"\"";
  Expression output;
  if (!ownerInitialized.parser.parseNoEmbeddingInCommand(input, output)) {
    global.fatal << "Failed to parse: " << input << global.fatal;
  }
  std::string result = output.toString();
  if (result != expected) {
    global.fatal
    << "Parsing and re-coding of: "
    << input
    << " resulted in: "
    << result
    << " instead of the expected: "
    << expected
    << global.fatal;
  }
  return true;
}

bool CalculatorTest::parseDecimal(Calculator& ownerInitialized) {
  std::string mustEvaluateToZero = "2.01 - 201/100";
  ownerInitialized.evaluate(mustEvaluateToZero);
  if (ownerInitialized.programExpression.toString() != "0") {
    global.fatal
    << "Expression: "
    << mustEvaluateToZero
    << " evaluates to: "
    << ownerInitialized.programExpression.toString()
    << " instead of 0. "
    << global.fatal;
  }
  std::string mustEvaluateToOne =
  "10000000000000000000000000000000000000000000000"
  "00000000000000000000000000000000000000000000000"
  "00000000000000000000000000000000000 - 10^128 + 1";
  ownerInitialized.evaluate(mustEvaluateToOne);
  if (ownerInitialized.programExpression.toString() != "1") {
    global.fatal
    << "Expression: "
    << mustEvaluateToOne
    << " evaluates to: "
    << ownerInitialized.programExpression.toString()
    << " instead of 1. "
    << global.fatal;
  }
  return true;
}

Calculator::Test::OneTest::OneTest() {
  this->requresAdminAccess = false;
  this->maximumRuntimeMilliseconds = 0;
  this->actualDuration = 0;
}

bool CalculatorTest::builtInFunctionsABTest(
  Calculator& ownerInitialized, bool updateABTestFile
) {
  Calculator::Test test(ownerInitialized);
  test.initialize();
  global.userInputStringIfAvailable = "";
  bool success = test.calculatorTestRun();
  // For verification, we only use the golden json.
  // Therefore we can immediately write the golden output files.
  // These are used so as to track via our repository the evolution of
  // the test cases and are not used to verify AB behavior.
  test.writeGoldenOutputFiles();
  if (!success && !updateABTestFile) {
    std::stringstream crashFileWriteReport;
    std::stringstream crashFile;
    crashFile
    << "<!DOCTYPE html>"
    << "<link rel='stylesheet' href='../calculator_html/crash_report.css'>"
    << "<body><div style='overflow:auto;height:100%'>"
    << test.reportHtml
    << "</div></body></html>";
    FileOperations::writeFileVirtual(
      "output/crash_test.html", crashFile.str(), &crashFileWriteReport
    );
    global.fatal
    << crashFileWriteReport.str()
    << "Calculator AB test failed with "
    << test.inconsistencies
    << " inconsistencies,  "
    << test.inconsistenciesMathML
    << " mathML inconsistencies and "
    << test.totalSpeedRegressions
    << " performance regressions. "
    << "See report html in file:\n"
    << "https://localhost:8166/output/crash_test.html\n"
    << global.fatal;
  }
  return true;
}

bool CalculatorExamplesTest::all() {
  CalculatorExamplesTest::compose();
  return true;
}

bool CalculatorExamplesTest::compose() {
  Calculator calculator;
  calculator.initialize(Calculator::Mode::full);
  calculator.examples.writeExamplesReadme();
  return true;
}

bool CalculatorParserTest::all() {
  STACK_TRACE("CalculatorParserTest::all");
  global << "CalculatorParserTest started." << Logger::endL;
  Calculator tester;
  tester.initialize(Calculator::Mode::full);
  CalculatorParserTest::largeExample1(tester);
  CalculatorParserTest::whitespace();
  global << Logger::green << "CalculatorParserTest completed." << Logger::endL;
  return true;
}

bool CalculatorParserTest::largeExample1(Calculator& initializedTester) {
  // A large example. Intended to catch non-linear parsing speed.
  std::stringstream out;
  int iterations = 100 * 1000;
  int64_t start = global.getElapsedMilliseconds();
  for (int i = 0; i < iterations; i ++) {
    out << "(1+5 *";
  }
  out << "1";
  for (int i = 0; i < iterations; i ++) {
    out << ")";
  }
  std::string input = out.str();
  Expression output;
  if (!initializedTester.parser.parseNoEmbeddingInCommand(input, output)) {
    global.fatal << "Failed to parse: " << input << global.fatal;
  }
  int64_t duration = global.getElapsedMilliseconds() - start;
  int64_t maximumDuration = iterations / 20;
  if (duration > maximumDuration) {
    global.fatal
    << "Parsing "
    << iterations
    << " iterations is too slow, took: "
    << duration
    << " milliseconds, maximum allowed: "
    << maximumDuration
    << ". "
    << global.fatal;
  }
  global
  << "Parsing expression of size: "
  << input.size()
  << " took: "
  << duration
  << " milliseconds. "
  << Logger::endL;
  return true;
}

bool CalculatorParserTest::whitespace() {
  STACK_TRACE("CalculatorParserTest::whitespace");
  std::string expression;
  Calculator owner;
  owner.initialize(Calculator::Mode::educational);
  Expression output(owner);
  std::string input =
  "\t\n\v\f\r\u0085\u00a0\u2000\u2001"
  "\u2002\u2003\u2004\u2005\u2006\u2007"
  "\u2008\u2009\u200A\u2028\u2029\u202f"
  "\u205f\u3000A";
  bool mustBeTrue = owner.parser.parse(input, false, output);
  if (!mustBeTrue) {
    global.fatal << "Failed to parse:\n" << input << global.fatal;
  }
  std::string result = output.toString();
  std::string expected = "A";
  if (result != expected) {
    global.fatal
    << "Parsed white space: got:\n"
    << HtmlRoutines::convertStringToURLString(result, false)
    << "\nexpected:\n"
    << HtmlRoutines::convertStringToURLString(expected, false)
    << output.toStringSemiFull()
    << global.fatal;
  }
  return true;
}
