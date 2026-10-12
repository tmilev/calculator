#include "calculator_interface.h"

bool Expression::toStringMinus3(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opMinus(), 3)) {
    return false;
  }
  if (input.children.size != 3) {
    global.fatal
    << "The minus function expects 1 or 2 arguments, "
    << "instead there are "
    << input.children.size - 1
    << ". "
    << global.fatal;
  }
  out << input[1].toString(format) << "-";
  if (
    input[2].startsWith(input.owner->opPlus()) ||
    input[2].startsWith(input.owner->opMinus())
  ) {
    out << "\\left(" << input[2].toString(format) << "\\right)";
  } else {
    out << input[2].toString(format);
  }
  return true;
}

bool Expression::toStringMinus(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  if (!input.startsWith(input.owner->opMinus())) {
    return false;
  }
  if (outputProperties != nullptr) {
    outputProperties->needsParenthesesForMultiplicationOnTheRight = true;
  }
  if (input.size() == 2) {
    return Expression::toStringMinus2(input, out, format);
  }
  return Expression::toStringMinus3(input, out, format);
}

bool Expression::toStringMinus2(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opMinus(), 2)) {
    return false;
  }
  if (
    input[1].startsWith(input.owner->opPlus()) ||
    input[1].startsWith(input.owner->opMinus())
  ) {
    out << "-\\left(" << input[1].toString(format) << "\\right)";
  } else {
    out << "-" << input[1].toString(format);
  }
  return true;
}

bool Expression::toStringOr(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  if (!input.startsWith(input.owner->opOr(), 3)) {
    return false;
  }
  if (!input[1].isSequenceNElements()) {
    out << "\\left(" << input[1] << "\\right)";
  } else {
    out << input[1];
  }
  out << " ~or~ ";
  if (!input[2].isSequenceNElements()) {
    out << "\\left(" << input[2] << "\\right)";
  } else {
    out << input[2];
  }
  return true;
}

bool Expression::toStringIntervalOpen(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opIntervalOpen(), 3)) {
    return false;
  }
  if (!input.owner->flagUseBracketsForIntervals) {
    out << "IntervalOpen{}";
  }
  out
  << "\\left("
  << input[1].toString(format)
  << ", "
  << input[2].toString(format)
  << "\\right)";
  return true;
}

bool Expression::toStringIntervalLeftClosed(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (input.startsWith(input.owner->opIntervalLeftClosed(), 2)) {
    if (input[1].isSequenceNElements(2)) {
      out << "\\left[" << input[1][1] << ", " << input[1][2] << "\\right)";
    } else {
      out << input[0].toString(format) << "{}" << input[1].toString(format);
    }
    return true;
  }
  if (input.startsWith(input.owner->opIntervalLeftClosed(), 3)) {
    out
    << "\\left["
    << input[1].toString(format)
    << ", "
    << input[2].toString(format)
    << "\\right)";
    return true;
  }
  return false;
}

bool Expression::toStringIntervalRightClosed(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (input.startsWith(input.owner->opIntervalRightClosed(), 3)) {
    out
    << "\\left("
    << input[1].toString(format)
    << ", "
    << input[2].toString(format)
    << "\\right]";
    return true;
  }
  if (input.startsWith(input.owner->opIntervalRightClosed(), 2)) {
    if (input[1].isSequenceNElements(2)) {
      out << "\\left(" << input[1][1] << ", " << input[1][2] << "\\right]";
    } else {
      out << input[0].toString(format) << "{}" << input[1].toString(format);
    }
    return true;
  }
  return false;
}

bool Expression::toStringIntervalClosed(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (input.startsWith(input.owner->opIntervalClosed(), 2)) {
    if (input[1].isSequenceNElements(2)) {
      out << "\\left[" << input[1][1] << ", " << input[1][2] << "\\right]";
    } else {
      out << input[0].toString(format) << "{}" << input[1].toString(format);
    }
    return true;
  }
  if (input.startsWith(input.owner->opIntervalClosed(), 3)) {
    out
    << "\\left["
    << input[1].toString(format)
    << ", "
    << input[2].toString(format)
    << "\\right]";
    return true;
  }
  return false;
}

bool Expression::toStringQuote(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) format;
  (void) outputProperties;
  if (!input.startsWith(input.owner->opQuote(), 2)) {
    return false;
  }
  std::string content;
  if (input[1].isOperation(&content)) {
    out << "\"" << content << "\"";
  } else {
    out << "(Corrupt string)";
  }
  return true;
}

bool Expression::toStringLogBase(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opLogBase(), 3)) {
    return false;
  }
  out
  << "\\log_{"
  << input[1].toString(format)
  << "}"
  << "\\left("
  << input[2].toString(format)
  << "\\right)";
  return true;
}

bool Expression::toStringIsDenotedBy(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.isListStartingWithAtom(input.owner->opIsDenotedBy())) {
    return false;
  }
  out << input[1].toString(format) << "=:" << input[2].toString(format);
  return true;
}

bool Expression::toStringDefineConditional(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opDefineConditional(), 4)) {
    return false;
  }
  out
  << input[1].toString(format)
  << " :if \\left("
  << input[2].toString(format)
  << "\\right)="
  << input[3].toString(format);
  return true;
}

bool Expression::toStringTensor(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opTensor(), 3)) {
    return false;
  }
  input.toStringOpMultiplicative(out, "\\otimes", format);
  return true;
}

bool Expression::toStringIn(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opIn(), 3)) {
    return false;
  }
  out << input[1].toString(format) << "\\in " << input[2].toString(format);
  return true;
}

bool Expression::toStringAnd(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  (void) format;
  if (!input.startsWith(input.owner->opAnd(), 3)) {
    return false;
  }
  out
  << "\\left("
  << input[1]
  << "\\right)"
  << " and "
  << "\\left("
  << input[2]
  << "\\right)";
  return true;
}

bool Expression::toStringBinom(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opBinom(), 3)) {
    return false;
  }
  out
  << "\\binom{"
  << input[1].toString(format)
  << "}{ "
  << input[2].toString(format)
  << "}";
  return true;
}

bool Expression::toStringUnderscore(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  if (!input.startsWith(input.owner->opUnderscore(), 3)) {
    return false;
  }
  if (outputProperties != nullptr) {
    outputProperties->needsParenthesesForMultiplicationOnTheRight = false;
    outputProperties->needsParenthesesWhenLastAndMultipliedOnTheLeft = false;
  }
  input.checkInitialization();
  const Expression& firstE = input[1];
  int opLimit = input.owner->opLimitBoundary();
  if (firstE.startsWith(opLimit) || firstE.isOperationGiven(opLimit)) {
    out
    << "(\\ "
    << firstE.toString(format)
    << ")_{"
    << input[2].toString(format)
    << "}";
    return true;
  }
  out
  << "{"
  << firstE.toString(format)
  << "}_{"
  << input[2].toString(format)
  << "}";
  return true;
}

bool Expression::toStringSetMinus(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opSetMinus(), 3)) {
    return false;
  }
  out
  << input[1].toString(format)
  << "\\setminus "
  << input[2].toString(format);
  return true;
}

bool Expression::toStringLimitBoundary(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opLimitBoundary(), 3)) {
    return false;
  }
  out
  << "\\limits_{"
  << input[1].toString(format)
  << "}^{"
  << input[2].toString(format)
  << "}";
  return true;
}

bool Expression::toStringCrossProduct(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opCrossProduct())) {
    return false;
  }
  input.toStringOpMultiplicative(out, "\\times", format);
  return true;
}

bool Expression::toStringAbsoluteValue(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  if (!input.startsWith(input.owner->opAbsoluteValue(), 2)) {
    return false;
  }
  if (outputProperties != nullptr) {
    outputProperties->needsParenthesesForMultiplicationOnTheRight = false;
  }
  out << "\\left|" << input[1].toString(format) << "\\right|";
  return true;
}

bool Expression::toStringBind(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  if (!input.startsWith(input.owner->opBind(), 2)) {
    return false;
  }
  out << "{{" << input[1].toString(format, outputProperties) << "}}";
  return true;
}

bool Expression::toStringMod(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.isListStartingWithAtom(input.owner->opMod())) {
    return false;
  }
  if (input.size() == 2) {
    out << " mod " << input[1].toString(format);
    return true;
  }
  if (input.size() == 3) {
    out << input[1].toString(format) << " mod " << input[2].toString(format);
    return true;
  }
  return false;
}

bool Expression::toStringLieBracket(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.isListStartingWithAtom(input.owner->opLieBracket())) {
    return false;
  }
  out
  << "["
  << input[1].toString(format)
  << ","
  << input[2].toString(format)
  << "]";
  return true;
}

bool Expression::toStringEqualEqual(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.isListStartingWithAtom(input.owner->opEqualEqual())) {
    return false;
  }
  out << input[1].toString(format) << "==" << input[2].toString(format);
  return true;
}

bool Expression::toStringEqualEqualEqual(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.isListStartingWithAtom(input.owner->opEqualEqualEqual())) {
    return false;
  }
  out << input[1].toString(format) << "===" << input[2].toString(format);
  return true;
}

bool Expression::toStringDefine(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  if (!input.startsWith(input.owner->opDefine(), 3)) {
    return false;
  }
  if (outputProperties != nullptr) {
    outputProperties->needsParenthesesForMultiplicationOnTheRight = true;
  }
  FormatExpressions formatForSubexpressions;
  if (format != nullptr) {
    formatForSubexpressions = *format;
  }
  formatForSubexpressions.flagExpressionIsTopLevel = false;
  std::string firstString = input[1].toString(&formatForSubexpressions);
  std::string secondString = input[2].toString(&formatForSubexpressions);
  if (
    input[1].isListStartingWithAtom(input.owner->opDefine()) ||
    input[1].isListStartingWithAtom(input.owner->opGreaterThan()) ||
    input[1].isListStartingWithAtom(input.owner->opGreaterThanOrEqualTo()) ||
    input[1].isListStartingWithAtom(input.owner->opLessThan()) ||
    input[1].isListStartingWithAtom(input.owner->opLessThanOrEqualTo())
  ) {
    out << "\\left(" << firstString << "\\right)";
  } else {
    out << firstString;
  }
  out << "=";
  if (input[2].isListStartingWithAtom(input.owner->opDefine())) {
    out << "\\left(" << secondString << "\\right)";
  } else {
    out << secondString;
  }
  return true;
}

bool Expression::toStringLnAbsoluteInsteadOfLogarithm(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (
    !input.owner->flagUseLnAbsInsteadOfLogForIntegrationNotation ||
    !input.startsWith(input.owner->opLog(), 2)
  ) {
    return false;
  }
  std::string argument = input[1].toString(format);
  if (!StringRoutines::stringBeginsWith(argument, "\\left|")) {
    out << "\\ln \\left|" << argument << "\\right|";
  } else {
    out << "\\ln " << argument;
  }
  return true;
}

bool Expression::toStringDifferential(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opDifferential())) {
    return false;
  }
  if (input.size() == 2) {
    return Expression::toStringDifferential2(input, out, format);
  }
  return
  Expression::toStringDifferential3(input, out, format, outputProperties);
}

bool Expression::toStringDifferential3(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  STACK_TRACE("Expression::toStringDifferential3");
  if (!input.startsWith(input.owner->opDifferential(), 3)) {
    return false;
  }
  MathExpressionFormattingProperties coefficientProperties;
  std::string coefficient = input[2].toString(format, &coefficientProperties);
  bool coefficientNeedsParentheses =
  coefficientProperties.needsParenthesesForMultiplicationOnTheRight ||
  coefficientProperties.startsWithMinus;
  if (input[2].startsWith(input.owner->opDivide())) {
    coefficientNeedsParentheses = false;
  }
  bool argumentOfDifferentialNeedsParentheses = (!input[1].isAtom()) && (
    !input[1].isBuiltInType()
  );
  if (coefficient == "1") {
    coefficientNeedsParentheses = false;
    coefficient = "";
  }
  if (coefficientNeedsParentheses) {
    out << "\\left(";
  }
  out << coefficient;
  if (coefficientNeedsParentheses) {
    out << "\\right)";
  }
  out << " {\\text{d}} ";
  if (argumentOfDifferentialNeedsParentheses) {
    out << "\\left(";
  }
  out << input[1].toString(format);
  if (argumentOfDifferentialNeedsParentheses) {
    out << "\\right)";
  }
  if (coefficientNeedsParentheses && outputProperties != nullptr) {
    outputProperties->startsWithDigit = false;
    outputProperties->startsWithMinus = false;
    outputProperties->startsWithFraction = false;
  }
  return true;
}

bool Expression::toStringDifferentiate(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opDifferentiate(), 3)) {
    return false;
  }
  out << "\\frac{\\text{d} ";
  MathExpressionFormattingProperties differentialProperties;
  std::string differential =
  input[2].toString(format, &differentialProperties);
  if (differentialProperties.needsParenthesesForMultiplicationOnTheRight) {
    out << "\\left(" << differential << "\\right)";
  } else {
    out << differential;
  }
  out << "}{\\text{d} " << input[1].toString(format) << "}";
  return true;
}

bool Expression::toStringDifferential2(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opDifferential(), 2)) {
    return false;
  }
  bool needsParen = (!input[1].isAtom()) && (!input[1].isBuiltInType());
  out << "{\\text{d}}{}";
  if (needsParen) {
    out << "\\left(";
  }
  out << input[1].toString(format);
  if (needsParen) {
    out << "\\right)";
  }
  return true;
}

bool Expression::toStringFactorial(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opFactorial(), 2)) {
    return false;
  }
  if (input[1].needsParenthesisForBaseOfExponent()) {
    out << "\\left(" << input[1].toString(format) << "\\right) !";
  } else {
    out << input[1].toString(format) << "!";
  }
  return true;
}

bool Expression::toStringSqrt(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opSqrt())) {
    return false;
  }
  if (input.size() == 2) {
    return Expression::toStringSqrt2(input, out, format);
  }
  return Expression::toStringSqrt3(input, out, format);
}

bool Expression::toStringSqrt2(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opSqrt(), 2)) {
    return false;
  }
  out << "\\sqrt{" << input[1].toString(format) << "}";
  return true;
}

bool Expression::toStringSqrt3(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opSqrt(), 3)) {
    return false;
  }
  int power = 0;
  bool hasPowerTwo = input[1].isSmallInteger(&power);
  if (hasPowerTwo) {
    hasPowerTwo = (power == 2);
  }
  if (hasPowerTwo) {
    out << "\\sqrt{" << input[2].toString(format) << "}";
  } else {
    out
    << "\\sqrt["
    << input[1].toString(format)
    << "]{"
    << input[2].toString(format)
    << "}";
  }
  return true;
}

bool Expression::toStringGreaterThan(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.isListStartingWithAtom(input.owner->opGreaterThan())) {
    return false;
  }
  out << input[1].toString(format) << "&gt;" << input[2].toString(format);
  return true;
}

bool Expression::toStringLessThanOrEqualTo(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.isListStartingWithAtom(input.owner->opLessThanOrEqualTo())) {
    return false;
  }
  out << input[1].toString(format) << "\\leq " << input[2].toString(format);
  return true;
}

bool Expression::toStringLessThan(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.isListStartingWithAtom(input.owner->opLessThan())) {
    return false;
  }
  out << input[1].toString(format) << "&lt;" << input[2].toString(format);
  return true;
}

bool Expression::toStringLimitProcess(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.isListStartingWithAtom(input.owner->opLimitProcess())) {
    return false;
  }
  out << input[1].toString(format) << " \\to " << input[2].toString(format);
  return true;
}

bool Expression::toStringGreaterThanOrEqualTo(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.isListStartingWithAtom(input.owner->opGreaterThanOrEqualTo())) {
    return false;
  }
  out << input[1].toString(format) << "\\geq " << input[2].toString(format);
  return true;
}

bool Expression::toStringError(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opError(), 2)) {
    return false;
  }
  input.checkInitialization();
  out << "\\text{Error:~" << input[1].toString(format) << "}";
  return true;
}

bool Expression::toStringSumOrIntegral(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  STACK_TRACE("Expression::toStringSumOrIntegral");
  (void) outputProperties;
  if (
    !input.startsWith(input.owner->opSum()) &&
    !input.startsWith(input.owner->opIntegral())
  ) {
    return false;
  }
  std::string opString = input[0].toString(format);
  out << opString << " ";
  int firstIndex = 2;
  if (input.size() >= 2) {
    if (input[1].startsWith(input.owner->opLimitBoundary(), 3)) {
      out
      << "_{"
      << input[1][1].toString(format)
      << "}"
      << "^{"
      << input[1][2].toString(format)
      << "}";
    } else if (
      input[1].isOperationGiven(input.owner->opIndefiniteIndicator())
    ) {
      firstIndex = 2;
    } else {
      firstIndex = 1;
    }
  }
  if (input.size() < firstIndex + 1) {
    return true;
  }
  if (input.size() > firstIndex + 1) {
    out << "(";
    for (int i = firstIndex; i < input.size(); i ++) {
      out << input[i].toString(format);
      if (i != input.size() - 1) {
        out << ", ";
      }
    }
    out << ")";
    return true;
  }
  const Expression& differential = input[firstIndex];
  if (!differential.startsWith(input.owner->opTimes(), 3)) {
    MathExpressionFormattingProperties differentialProperties;
    std::string differentialString =
    differential.toString(format, &differentialProperties);
    if (differentialProperties.startsWithMinus) {
      out << "\\left(" << differentialString << "\\right)";
    } else {
      out << differentialString;
    }
    return true;
  }
  return
  Expression::toStringTimesInContext(differential, out, format, nullptr, true);
}

bool Expression::toStringLimit(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  if (!input.startsWith(input.owner->opLimit(), 3)) {
    return false;
  }
  if (outputProperties != nullptr) {
    outputProperties->needsParenthesesForMultiplicationOnTheRight = true;
  }
  out << "\\lim_{";
  if (!input[1].isSequenceNElements()) {
    out << input[1].toString(format);
  } else {
    for (int i = 1; i < input[1].size(); i ++) {
      out << input[1][i].toString(format);
      if (i != input[1].size() - 1) {
        out << ", ";
      }
    }
  }
  out << "}" << input[2].toString(format);
  return true;
}

bool Expression::toStringUnion(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (
    !input.isListStartingWithAtom(input.owner->opUnion()) || input.size() != 3
  ) {
    return false;
  }
  if (input[1].startsWith(input.owner->opIntersection())) {
    out
    << "\\left("
    << input[1].toString(format)
    << "\\right)"
    << "\\cup "
    << input[2].toString(format);
  } else {
    out << input[1].toString(format) << "\\cup " << input[2].toString(format);
  }
  return true;
}

bool Expression::toStringUnionNoRepetition(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.isListStartingWithAtom(input.owner->opUnionNoRepetition())) {
    return false;
  }
  out << input[1].toString(format) << "\\sqcup " << input[2].toString(format);
  return true;
}

bool Expression::toStringIntersection(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (
    !input.isListStartingWithAtom(input.owner->opIntersection()) ||
    input.size() != 3
  ) {
    return false;
  }
  if (input[1].startsWith(input.owner->opUnion())) {
    out
    << "\\left("
    << input[1].toString(format)
    << "\\right)"
    << "\\cap "
    << input[2].toString(format);
  } else {
    out << input[1].toString(format) << "\\cap " << input[2].toString(format);
  }
  return true;
}

void Expression::initializeToStringHandlers(Calculator& toBeInitialized) {
  STACK_TRACE("Calculator::initializeToStringHandlers");
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opDefine(), Expression::toStringDefine
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opIsDenotedBy(), Expression::toStringIsDenotedBy
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opLog(), Expression::toStringLnAbsoluteInsteadOfLogarithm
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opLogBase(), Expression::toStringLogBase
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opIntervalOpen(), Expression::toStringIntervalOpen
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opIntervalRightClosed(),
    Expression::toStringIntervalRightClosed
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opIntervalLeftClosed(),
    Expression::toStringIntervalLeftClosed
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opIntervalClosed(), Expression::toStringIntervalClosed
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opQuote(), Expression::toStringQuote
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opDefineConditional(),
    Expression::toStringDefineConditional
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opDivide(), Expression::toStringDivide
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opTensor(), Expression::toStringTensor
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opIn(), Expression::toStringIn
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opOr(), Expression::toStringOr
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opAnd(), Expression::toStringAnd
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opBinom(), Expression::toStringBinom
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opUnderscore(), Expression::toStringUnderscore
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opSetMinus(), Expression::toStringSetMinus
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opLimitBoundary(), Expression::toStringLimitBoundary
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opTimes(), Expression::toStringTimes
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opCrossProduct(), Expression::toStringCrossProduct
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opSqrt(), Expression::toStringSqrt
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opFactorial(), Expression::toStringFactorial
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opAbsoluteValue(), Expression::toStringAbsoluteValue
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opPower(), Expression::toStringPower
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opPlus(), Expression::toStringPlus
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opDirectSum(), Expression::toStringDirectSum
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opMinus(), Expression::toStringMinus
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opBind(), Expression::toStringBind
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opEqualEqual(), Expression::toStringEqualEqual
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opEqualEqualEqual(), Expression::toStringEqualEqualEqual
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opDifferentiate(), Expression::toStringDifferentiate
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opDifferential(), Expression::toStringDifferential
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opSum(), Expression::toStringSumOrIntegral
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opIntegral(), Expression::toStringSumOrIntegral
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opGreaterThan(), Expression::toStringGreaterThan
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opGreaterThanOrEqualTo(),
    Expression::toStringGreaterThanOrEqualTo
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opLessThanOrEqualTo(),
    Expression::toStringLessThanOrEqualTo
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opLimit(), Expression::toStringLimit
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opLimitProcess(), Expression::toStringLimitProcess
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opLessThan(), Expression::toStringLessThan
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opSequence(), Expression::toStringSequence
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opLieBracket(), Expression::toStringLieBracket
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opMod(), Expression::toStringMod
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opUnion(), Expression::toStringUnion
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opIntersection(), Expression::toStringIntersection
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opUnionNoRepetition(),
    Expression::toStringUnionNoRepetition
  );
  toBeInitialized.addOneStringAtomHandler(
    toBeInitialized.opError(), Expression::toStringError
  );
  toBeInitialized.addOneStringCompositeHandler(
    toBeInitialized.opMatrix(), Expression::toStringMatrix
  );
}
