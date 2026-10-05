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
  int iterations = 1000;
  Expression current;
  Expression one;
  one.assignValue(owner, 1);
  int operatorPlus = owner.opPlus();
  int64_t start = global.getElapsedMilliseconds();
  int DEBUG_DO_NOT_SUBMIT;
  int counter = 0;
  int childExpressionsAtStart = 0;
  int startHashResizes = GlobalStatistics::numberOfHashResizes;
  for (int i = 0; i < iterations; i ++) {
    current.makeXOX(owner, operatorPlus, current, one);
    counter ++;
    if (counter >= 10000) {
      counter = 0;
      int64_t elapsedSoFar = global.getElapsedMilliseconds() - start;
      int hashResizesSoFar =
      GlobalStatistics::numberOfHashResizes - startHashResizes;
      global
      << "Iteration: "
      << i
      << ", milliseconds: "
      << elapsedSoFar
      << ". "
      << "Hash resizes: "
      << hashResizesSoFar
      << Logger::endL;
      int64_t expressionRam =
      owner.allChildExpressions.byteSizeOwnedThroughPointers();
      expressionRam +=
      owner.allChildExpressionHashes.byteSizeOwnedThroughPointers();
      int childrenMaximumBucketSize =
      owner.allChildExpressions.largestBucketSize();
      double bytesPerIteration = 0;
      double bytesPerExpression = 0;
      double expressionsPerIteration = 0;
      int expressionsCreated =
      owner.allChildExpressions.size - childExpressionsAtStart;
      if (i > 0) {
        bytesPerIteration = static_cast<double>(expressionRam) /
        static_cast<double>(i);
        bytesPerExpression = static_cast<double>(expressionRam) /
        static_cast<double>(owner.allChildExpressions.size);
        expressionsPerIteration = static_cast<double>(expressionsCreated) /
        static_cast<double>(i);
      }
      global
      << "Total expression RAM: "
      << expressionRam
      << ", "
      << FloatingPoint::doubleToString(bytesPerExpression)
      << " bytes per expression, "
      << FloatingPoint::doubleToString(bytesPerIteration)
      << " bytes per iteration. "
      << Logger::endL;
      global
      << "Expressions created: "
      << expressionsCreated
      << ". "
      << "Expressions per iteration: "
      << FloatingPoint::doubleToString(expressionsPerIteration)
      << Logger::endL;
      global
      << "Current expression RAM in bytes: "
      << RamUsageComputation::byteSize(current)
      << Logger::endL;
      double expressionRAMPerMillisecond = static_cast<double>(expressionRam) /
      elapsedSoFar;
      global
      << "Ram per millisecond: "
      << expressionRAMPerMillisecond
      << Logger::endL;
      global
      << "Maximum hash bucket size: "
      << childrenMaximumBucketSize
      << ". "
      << Logger::endL;
    }
  }
  int64_t duration = global.getElapsedMilliseconds() - start;
  int64_t maximumDuration = 10;
  int totalHashResizes =
  GlobalStatistics::numberOfHashResizes - startHashResizes;
  if (duration > maximumDuration) {
    global.fatal
    << "Constructing a large nested expression: "
    << iterations
    << " iterations "
    << "took "
    << duration
    << " ms, maximum allowed: "
    << maximumDuration
    << " ms.\nAll children: "
    << owner.allChildExpressions.getReport()
    << "\nTotal hash resizes: "
    << totalHashResizes
    << ".\n"
    << global.fatal;
  }
  return true;
}
