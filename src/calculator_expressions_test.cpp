#include "calculator_interface.h"
#include "test.h"

bool ExpressionTest::all() {
  STACK_TRACE("ExpressionTest::all");
  Calculator tester;
  tester.initialize(Calculator::Mode::full);
  ExpressionTest::smallStringConversion(
    tester, false, ExpressionTestType::Sum, 2, "1+1+1"
  );
  ExpressionTest::smallStringConversion(
    tester,
    true,
    ExpressionTestType::Sum,
    2,
    "<mrow><mn>1</mn><mo>+</mo><mn>1</mn><mo>+</mo><mn>1</mn></mrow>"
  );
  ExpressionTest::smallStringConversion(
    tester,
    false,
    ExpressionTestType::SumProduct,
    2,
    "2 \\left(2 \\left(2+2\\right)+2\\right)"
  );
  ExpressionTest::smallStringConversion(
    tester,
    false,
    ExpressionTestType::Arithmetic,
    1,
    "\\frac{4}{3 \\left(2+2\\right)}"
  );
  ExpressionTest::smallStringConversion(
    tester,
    false,
    ExpressionTestType::NestedList,
    3,
    "\\left(0, 1, 2, \\left(0, 1, \\left(0, 0\\right)\\right)\\right)"
  );
  List<int> iterationsToTest = List<int>({200, 400, 800});
  List<ExpressionTestType> types = List<ExpressionTestType>({
      ExpressionTestType::Sum,
      ExpressionTestType::SumProduct,
      ExpressionTestType::Arithmetic,
      ExpressionTestType::NestedList
    });
  for (int i = 0; i < 2; i ++) {
    for (ExpressionTestType& oneType : types) {
      for (int iterations : iterationsToTest) {
        ExpressionTest::largeStringConversion(
          tester, i % 2 == 1, oneType, iterations
        );
      }
    }
  }
  ExpressionTest::builtInAtomValues(tester);
  ExpressionTest::largeNestedExpressionCreationSpeed1(tester);
  ExpressionTest::toStringTestRecode(tester);
  ExpressionTest::isUserDefinedAtom(tester);
  return true;
}

bool ExpressionTest::toStringTestRecodeOnce(
  const std::string& inputHardCodedMustParse, Calculator& owner
) {
  STACK_TRACE("ExpressionTest::toStringTestRecodeOnce");
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
  STACK_TRACE("ExpressionTest::toStringTestRecode");
  ExpressionTest::toStringTestRecodeOnce("1+1", owner);
  ExpressionTest::toStringTestRecodeOnce("\"\u00B0\"", owner);
  ExpressionTest::toStringTestRecodeOnce("\u00B0", owner);
  ExpressionTest::toStringTestRecodeOnce("\"\\\\\\\"\"", owner);
  return true;
}

bool ExpressionTest::isUserDefinedAtomOnce(
  Calculator& owner, const std::string& input, bool isUserDefinedAtom
) {
  STACK_TRACE("ExpressionTest::isUserDefinedAtomOnce");
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
  STACK_TRACE("ExpressionTest::isUserDefinedAtom");
  ExpressionTest::isUserDefinedAtomOnce(owner, "x", true);
  ExpressionTest::isUserDefinedAtomOnce(owner, "x+y", false);
  ExpressionTest::isUserDefinedAtomOnce(owner, "x+1", false);
  ExpressionTest::isUserDefinedAtomOnce(owner, "1", false);
  return true;
}

bool ExpressionTest::builtInAtomValues(Calculator& owner) {
  STACK_TRACE("ExpressionTest::builtInAtomValues");
  if (Calculator::BuiltInAtomValues::list != owner.opList()) {
    global.fatal << "Bad value of BuiltInAtomValues::list. " << global.fatal;
  }
  return true;
}

void ExpressionTest::makeLargeAdditionSumNested(
  Calculator& owner, Expression& output, int iterations
) {
  Expression one;
  output.assignValue(owner, 1);
  one.assignValue(owner, 1);
  int operatorPlus = owner.opPlus();
  for (int i = 0; i < iterations; i ++) {
    output.makeXOX(owner, operatorPlus, output, one);
  }
}

void ExpressionTest::makeLargeAdditionSumProductNested(
  Calculator& owner, Expression& output, int iterations
) {
  Expression two;
  output.assignValue(owner, 2);
  two.assignValue(owner, 2);
  int operatorPlus = owner.opPlus();
  int operatorTimes = owner.opTimes();
  for (int i = 0; i < iterations; i ++) {
    output.makeXOX(owner, operatorPlus, output, two);
    output.makeXOX(owner, operatorTimes, two, output);
  }
}

void ExpressionTest::makeLargeNestedList(
  Calculator& owner, Expression& output, int iterations
) {
  STACK_TRACE("ExpressionTest::makeLargeNestedList");
  output = owner.expressionInteger(0);
  for (int i = 0; i < iterations; i ++) {
    Expression copy = output;
    output.makeSequence(owner);
    for (int j = 0; j < i + 1; j ++) {
      output.addChildRationalOnTop(j);
    }
    output.addChildOnTop(copy);
  }
}

void ExpressionTest::makeLargeExpressionArithmeticNested(
  Calculator& owner, Expression& output, int iterations
) {
  Expression two = owner.expressionInteger(2);
  Expression three = owner.expressionInteger(3);
  Expression four = owner.expressionInteger(4);
  output.assignValue(owner, 2);
  int operatorPlus = owner.opPlus();
  int operatorTimes = owner.opTimes();
  int operatorDivide = owner.opDivide();
  for (int i = 0; i < iterations; i ++) {
    output.makeXOX(owner, operatorPlus, output, two);
    output.makeXOX(owner, operatorTimes, three, output);
    output.makeXOX(owner, operatorDivide, four, output);
  }
}

bool ExpressionTest::largeStringConversion(
  Calculator& owner,
  bool useMathML,
  ExpressionTestType expressionType,
  int iterations
) {
  STACK_TRACE("ExpressionTest::largeStringConversion");
  owner.initialize(Calculator::Mode::full);
  Expression current;
  ExpressionTest::makeLarge(owner, current, expressionType, iterations);
  int64_t start = global.getElapsedMilliseconds();
  std::string result = useMathML ? current.toMathML() : current.toString();
  int64_t duration = global.getElapsedMilliseconds() - start;
  double minimumSpeed = 100;
  double recommendedSpeed = 10000;
  std::string label = useMathML ? "toMathML" : "toString";
  std::string expressionTypeString =
  ExpressionTest::expressionTypeToString(expressionType);
  if (duration == 0) {
    duration = 1;
  }
  double speed = static_cast<double>(result.size()) / duration;
  if (speed < minimumSpeed) {
    global.fatal
    << "Large "
    << expressionTypeString
    << " "
    << label
    << " ["
    << iterations
    << " iterations] "
    << "took "
    << duration
    << " milliseconds to convert, at speed: "
    << FloatingPoint::doubleToString(speed)
    << " chars/ms. This is too slow, the minimum speed is: "
    << minimumSpeed
    << ". "
    << global.fatal;
  }
  if (speed < recommendedSpeed) {
    global
    << Logger::yellow
    << "Large "
    << expressionTypeString
    << " "
    << label
    << " ["
    << iterations
    << " iterations] "
    << "took "
    << duration
    << " milliseconds to convert, at speed: "
    << FloatingPoint::doubleToString(speed, 1)
    << " chars/ms. This is too slow, the recommended speed is: "
    << recommendedSpeed
    << ". "
    << Logger::endL;
  }
  return true;
}

bool ExpressionTest::smallStringConversion(
  Calculator& owner,
  bool mathML,
  ExpressionTestType expressionType,
  int iterations,
  const std::string& expected
) {
  STACK_TRACE("ExpressionTest::smallStringConversion");
  owner.initialize(Calculator::Mode::full);
  Expression current;
  ExpressionTest::makeLarge(owner, current, expressionType, iterations);
  std::string actual = mathML ? current.toMathML() : current.toString();
  if (actual != expected) {
    global.fatal
    << "Expression printout is:\n"
    << actual
    << "\nexpected:\n"
    << expected
    << global.fatal;
  }
  return true;
}

void ExpressionTest::makeLarge(
  Calculator& owner,
  Expression& output,
  ExpressionTestType expressionType,
  int iterations
) {
  switch (expressionType) {
  case ExpressionTestType::Sum:
    ExpressionTest::makeLargeAdditionSumNested(owner, output, iterations);
    break;
  case ExpressionTestType::SumProduct:
    ExpressionTest::makeLargeAdditionSumProductNested(
      owner, output, iterations
    );
    break;
  case ExpressionTestType::Arithmetic:
    ExpressionTest::makeLargeExpressionArithmeticNested(
      owner, output, iterations
    );
    break;
  case ExpressionTestType::NestedList:
    ExpressionTest::makeLargeNestedList(owner, output, iterations);
    break;
  }
}

std::string ExpressionTest::expressionTypeToString(
  ExpressionTestType expressionType
) {
  switch (expressionType) {
  case ExpressionTestType::Sum:
    return "sum";
  case ExpressionTestType::SumProduct:
    return "sum-product";
  case ExpressionTestType::Arithmetic:
    return "aritmetic expression";
  case ExpressionTestType::NestedList:
    return "nested list";
  }
  return "unknown";
}

bool ExpressionTest::largeNestedExpressionCreationSpeed1(Calculator& owner) {
  STACK_TRACE("ExpressionTest::largeNestedExpressionCreationSpeed1");
  owner.initialize(Calculator::Mode::full);
  int iterations = 1 * 1000 * 1000;
  int64_t start = global.getElapsedMilliseconds();
  Expression current;
  ExpressionTest::makeLargeAdditionSumNested(owner, current, iterations);
  int64_t duration = global.getElapsedMilliseconds() - start;
  int startHashResizes = GlobalStatistics::numberOfHashResizes;
  int totalHashResizes =
  GlobalStatistics::numberOfHashResizes - startHashResizes;
  int64_t maximumDuration = 6000;
  int64_t recommendedDuration = 300;
  if (duration > recommendedDuration) {
    global
    << Logger::yellow
    << "Constructing a large nested expression: "
    << iterations
    << " iterations took "
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
