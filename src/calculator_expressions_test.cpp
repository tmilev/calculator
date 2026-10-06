#include "calculator_interface.h"
#include "test.h"

bool ExpressionTest::all() {
  STACK_TRACE("Expression::Test::all");
  Calculator tester;
  tester.initialize(Calculator::Mode::full);
  ExpressionTest::builtInAtomValues(tester);
  ExpressionTest::largeNestedExpression1(tester);
  ExpressionTest::toStringTestRecode(tester);
  ExpressionTest::isUserDefinedAtom(tester);
  return true;
}

bool ExpressionTest::toStringTestRecodeOnce(
  const std::string& inputHardCodedMustParse, Calculator& owner
) {
  STACK_TRACE("Expression::Test::toStringTestRecodeOnce");
  Expression parsed = owner.parseOrCrash(inputHardCodedMustParse, true);
  std::string recoded = parsed.toString();
  if (recoded != inputHardCodedMustParse) {
    global.fatal
    << "Recoded string: "
    << recoded
    << " does not coincide with the original: "
    << inputHardCodedMustParse
    << "."
    << global.fatal;
  }
  return true;
}

bool ExpressionTest::toStringTestRecode(Calculator& owner) {
  STACK_TRACE("Expression::Test::toStringTestRecode");
  ExpressionTest::toStringTestRecodeOnce("1+1", owner);
  ExpressionTest::toStringTestRecodeOnce("\"\u00B0\"", owner);
  ExpressionTest::toStringTestRecodeOnce("\u00B0", owner);
  ExpressionTest::toStringTestRecodeOnce("\"\\\\\\\"\"", owner);
  return true;
}

bool ExpressionTest::isUserDefinedAtomOnce(
  Calculator& owner, const std::string& input, bool isUserDefinedAtom
) {
  STACK_TRACE("Expression::Test::isUserDefinedAtomOnce");
  Expression expression = owner.parseOrCrash(input, true);
  if (expression.isAtomUserDefined() != isUserDefinedAtom) {
    global.fatal
    << "Expression::isAtomUserDefined did not return "
    << isUserDefinedAtom
    << " for input: "
    << expression.toString()
    << " with lispification: "
    << expression.toStringFull()
    << ", parsed from: "
    << input
    << global.fatal;
  }
  return true;
}

bool ExpressionTest::isUserDefinedAtom(Calculator& owner) {
  STACK_TRACE("Expression::Test::isUserDefinedAtom");
  ExpressionTest::isUserDefinedAtomOnce(owner, "x", true);
  ExpressionTest::isUserDefinedAtomOnce(owner, "x+y", false);
  ExpressionTest::isUserDefinedAtomOnce(owner, "x+1", false);
  ExpressionTest::isUserDefinedAtomOnce(owner, "1", false);
  return true;
}

bool ExpressionTest::builtInAtomValues(Calculator& owner) {
  STACK_TRACE("Expression::Test::builtInAtomValues");
  if (Calculator::BuiltInAtomValues::list != owner.opList()) {
    global.fatal << "Bad value of BuiltInAtomValues::list. " << global.fatal;
  }
  return true;
}

bool ExpressionTest::largeNestedExpression1(Calculator& owner) {
  STACK_TRACE("Expression::Test::largeNestedExpression1");
  int iterations = 1 * 1000 * 1000;
  Expression current;
  Expression one;
  one.assignValue(owner, 1);
  int operatorPlus = owner.opPlus();
  int64_t start = global.getElapsedMilliseconds();
  int startHashResizes = GlobalStatistics::numberOfHashResizes;
  for (int i = 0; i < iterations; i ++) {
    current.makeXOX(owner, operatorPlus, current, one);
  }
  int64_t duration = global.getElapsedMilliseconds() - start;
  int64_t maximumDuration = 6000;
  int64_t recommendedDuration = 300;
  int totalHashResizes =
  GlobalStatistics::numberOfHashResizes - startHashResizes;
  if (duration > recommendedDuration) {
    global
    << Logger::red
    << "Constructing a large nested expression: "
    << iterations
    << " iterations took "
    << Logger::red
    << duration
    << " ms, maximum allowed: "
    << maximumDuration
    << " ms, recommended: "
    << recommendedDuration
    << "ms."
    << Logger::endL
    << "All children: "
    << owner.allChildExpressions.getReport()
    << "\nTotal hash resizes: "
    << totalHashResizes
    << ".\n"
    << "Ram: "
    << RamUsageComputation::byteSize(owner.allChildExpressions)
    << Logger::endL;
  }
  if (duration > maximumDuration) {
    global.fatal << global.fatal;
  }
  return true;
}
