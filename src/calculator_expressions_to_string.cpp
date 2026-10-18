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

bool Expression::toStringExpressionPrintoutMinus2(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opMinus(), 2)) {
    return false;
  }
  if (
    input[1].startsWith(input.owner->opPlus()) ||
    input[1].startsWith(input.owner->opMinus())
  ) {
    output.addString("-\\left(").addChildIndex(childrenIndices[1]).addString(
      "\\right)"
    );
  } else {
    output.addString("-").addChildIndex(childrenIndices[1]);
  }
  output.properties.startsWithMinus = true;
  return true;
}

bool Expression::toStringExpressionPrintoutMinus3(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
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
  output.addChildIndex(childrenIndices[1]).addString("-");
  if (
    input[2].startsWith(input.owner->opPlus()) ||
    input[2].startsWith(input.owner->opMinus())
  ) {
    output.addString("\\left(").addChildIndex(childrenIndices[2]).addString(
      "\\right)"
    );
  } else {
    output.addChildIndex(childrenIndices[2]);
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

bool Expression::toStringDivide(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  Calculator& commands = *input.owner;
  if (!input.startsWith(commands.opDivide(), 3)) {
    return false;
  }
  if (outputProperties != nullptr) {
    if (input[1] == "Differential") {
      outputProperties->needsParenthesesForMultiplicationOnTheRight = false;
    }
  }
  bool doUseFrac = !commands.flagDontUseFracInRational;
  if (
    doUseFrac && (
      input[1].startsWith(commands.opTimes()) ||
      input[1].startsWith(commands.opDivide())
    )
  ) {
    List<Expression> multiplicandsLeft;
    input.getMultiplicandsDivisorsRecursive(multiplicandsLeft, 0);
    for (int i = 0; i < multiplicandsLeft.size; i ++) {
      if (
        multiplicandsLeft[i].startsWith(commands.opIntegral()) ||
        multiplicandsLeft[i].isOperationGiven(commands.opIntegral())
      ) {
        doUseFrac = false;
        break;
      }
    }
  }
  if (!doUseFrac) {
    std::string firstE = input[1].toString(format);
    std::string secondE = input[2].toString(format);
    bool firstNeedsBrackets =
    !(
      input[1].isListStartingWithAtom(commands.opTimes()) ||
      input[1].isListStartingWithAtom(commands.opDivide())
    );
    bool secondNeedsBrackets = true;
    if (input[2].isOfType<Rational>()) {
      if (input[2].getValue<Rational>().isInteger()) {
        secondNeedsBrackets = false;
      }
    }
    if (firstNeedsBrackets) {
      out << "(" << firstE << ")";
    } else {
      out << firstE;
    }
    out << "/";
    if (secondNeedsBrackets) {
      out << "(" << secondE << ")";
    } else {
      out << secondE;
    }
  } else {
    StateMaintainer<bool> maintain;
    if (format != nullptr) {
      maintain.initialize(format->flagExpressionNewLineAllowed);
      format->flagExpressionNewLineAllowed = false;
    } else {
      // Compilers will complain at certain optimization levels about
      // maintain not being initialized.
      maintain.toMaintain = nullptr;
      maintain.contentAtStart = false;
    }
    std::string firstE = input[1].toString(format);
    std::string secondE = input[2].toString(format);
    out << "\\frac{" << firstE << "}{" << secondE << "}";
  }
  return true;
}

bool Expression::toStringPower(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  input.checkInitialization();
  Calculator& commands = *input.owner;
  if (!input.startsWith(commands.opPower(), 3)) {
    return false;
  }
  const Expression& baseExpression = input[1];
  const Expression& exponentExpression = input[2];
  bool isSuperScriptOfUnderscoredOperator = false;
  if (baseExpression.startsWith(commands.opUnderscore(), 3)) {
    if (baseExpression[1].isOperationGiven(commands.opIntegral())) {
      isSuperScriptOfUnderscoredOperator = true;
    }
  }
  if (outputProperties != nullptr) {
    if (
      baseExpression.children.size > 0 &&
      baseExpression[0].isAtomWhoseExponentsAreInterpretedAsFunction()
    ) {
      outputProperties->needsParenthesesForMultiplicationOnTheRight = true;
      outputProperties->needsParenthesesWhenLastAndMultipliedOnTheLeft = false;
    } else {
      outputProperties->needsParenthesesForMultiplicationOnTheRight = false;
      outputProperties->needsParenthesesWhenLastAndMultipliedOnTheLeft = false;
    }
  }
  bool involvesExponentsInterpretedAsFunctions = false;
  if (baseExpression.startsWith(- 1, 2)) {
    involvesExponentsInterpretedAsFunctions =
    baseExpression[0].isAtomWhoseExponentsAreInterpretedAsFunction() &&
    !exponentExpression.isEqualToMOne() &&
    exponentExpression.isRational();
    if (
      involvesExponentsInterpretedAsFunctions &&
      baseExpression[0].isOperationGiven(commands.opLog()) &&
      commands.flagUseLnAbsInsteadOfLogForIntegrationNotation
    ) {
      involvesExponentsInterpretedAsFunctions = false;
    }
  }
  if (involvesExponentsInterpretedAsFunctions) {
    Expression newFunctionExpression;
    newFunctionExpression.makeXOX(
      *input.owner, commands.opPower(), baseExpression[0], input[2]
    );
    newFunctionExpression.checkConsistency();
    out << "{" << newFunctionExpression.toString(format) << "}{}";
    if (
      baseExpression[1].
      needsParenthesisForMultiplicationWhenSittingOnTheRightMost() ||
      baseExpression[1].startsWith(commands.opTimes())
    ) {
      out << "\\left(" << baseExpression[1].toString(format) << "\\right)";
    } else {
      out << baseExpression[1].toString(format);
    }
    return true;
  }
  if (isSuperScriptOfUnderscoredOperator) {
    out
    << baseExpression[1].toString(format)
    << "_{"
    << baseExpression[2].toString(format)
    << "}^{"
    << exponentExpression.toString(format)
    << "}";
    return true;
  }
  int opLimit = commands.opLimitBoundary();
  if (
    baseExpression.startsWith(opLimit) ||
    baseExpression.isOperationGiven(opLimit)
  ) {
    out
    << "(\\ "
    << baseExpression.toString(format)
    << ")^{"
    << exponentExpression.toString(format)
    << "}";
    return true;
  }
  bool isSqrt = false;
  if (exponentExpression.isOfType<Rational>()) {
    if (exponentExpression.getValue<Rational>().isEqualTo(Rational(1, 2))) {
      isSqrt = true;
    }
  }
  if (isSqrt) {
    out << "\\sqrt{" << baseExpression.toString(format) << "}";
    return true;
  }
  MathExpressionFormattingProperties baseProperties;
  std::string baseString = baseExpression.toString(format, &baseProperties);
  std::string secondExpressionString = exponentExpression.toString(format);
  if (baseExpression.needsParenthesisForBaseOfExponent()) {
    bool useBigParenthesis = true;
    if (baseExpression.startsWith(commands.opDivide())) {
      useBigParenthesis = true;
    }
    if (useBigParenthesis) {
      out << "\\left(";
    } else {
      out << "(";
    }
    out << baseString;
    if (useBigParenthesis) {
      out << "\\right)";
    } else {
      out << ")";
    }
  } else {
    out << baseString;
  }
  out << "^{" << secondExpressionString << "}";
  return true;
}

bool Expression::toStringExpressionPrintoutDefine(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opDefine(), 3)) {
    return false;
  }
  output.properties.needsParenthesesForMultiplicationOnTheRight = true;
  FormatExpressions formatForSubexpressions;
  if (format != nullptr) {
    formatForSubexpressions = *format;
  }
  formatForSubexpressions.flagExpressionIsTopLevel = false;
  if (
    input[1].isListStartingWithAtom(input.owner->opDefine()) ||
    input[1].isListStartingWithAtom(input.owner->opGreaterThan()) ||
    input[1].isListStartingWithAtom(input.owner->opGreaterThanOrEqualTo()) ||
    input[1].isListStartingWithAtom(input.owner->opLessThan()) ||
    input[1].isListStartingWithAtom(input.owner->opLessThanOrEqualTo())
  ) {
    output.addString("\\left(").addChildIndex(childrenIndices[1]).addString(
      "\\right)"
    );
  } else {
    output.addChildIndex(childrenIndices[1]);
  }
  output.addString("=");
  if (input[2].isListStartingWithAtom(input.owner->opDefine())) {
    output.addString("\\left(").addChildIndex(childrenIndices[2]).addString(
      "\\right)"
    );
  } else {
    output.addChildIndex(childrenIndices[2]);
  }
  return true;
}

bool Expression::toStringPlus(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  if (!input.startsWith(input.owner->opPlus())) {
    return false;
  }
  if (outputProperties != nullptr) {
    outputProperties->needsParenthesesForMultiplicationOnTheRight = true;
  }
  if (input.children.size < 3) {
    global.fatal
    << "Plus operation takes at least 2 arguments, "
    << "whereas this expression has "
    << input.children.size - 1
    << " arguments. "
    << global.fatal;
  }
  const Expression& left = input[1];
  const Expression& right = input[2];
  std::string leftString;
  if (left.needsParenthesisForAddition()) {
    leftString = "\\left(" + left.toString(format) + "\\right)";
  } else {
    leftString = left.toString(format);
  }
  out << leftString;
  bool allowNewLine = false;
  if (format != nullptr) {
    allowNewLine = format->flagExpressionNewLineAllowed;
  }
  bool useFrac = !input.owner->flagDontUseFracInRational;
  if (
    allowNewLine &&
    !useFrac &&
    leftString.size() > static_cast<unsigned>(
      FormatExpressions::expressionLineBreak
    )
  ) {
    out << "\\\\\n";
  }
  std::string rightString = right.toString(format);
  if (right.needsParenthesisForAddition()) {
    rightString = "\\left(" + right.toString(format) + "\\right)";
  }
  if (rightString.size() > 0) {
    if (rightString[0] != '-') {
      out << "+";
    }
  }
  out << rightString;
  return true;
}

bool Expression::toStringDirectSum(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.isListStartingWithAtom(input.owner->opDirectSum())) {
    return false;
  }
  if (input.children.size < 3) {
    global.fatal
    << "Direct sum operation takes at least 2 arguments, "
    << "whereas this expression has "
    << input.children.size
    << " arguments. "
    << global.fatal;
  }
  const Expression& left = input[1];
  const Expression& right = input[2];
  std::string leftString;
  if (left.needsParenthesisForAddition()) {
    leftString = "\\left(" + left.toString(format) + "\\right)";
  } else {
    leftString = left.toString(format);
  }
  out << leftString;
  bool allowNewLine = false;
  if (format != nullptr) {
    allowNewLine = format->flagExpressionNewLineAllowed;
  }
  bool useFrac = !input.owner->flagDontUseFracInRational;
  if (
    allowNewLine &&
    !useFrac &&
    leftString.size() > static_cast<unsigned>(
      FormatExpressions::expressionLineBreak
    )
  ) {
    out << "\\\\\n";
  }
  std::string rightString =
  right.needsParenthesisForAddition() ? (
    "\\left(" + right.toString(format) + "\\right)"
  ) : right.toString(format);
  if (rightString.size() > 0) {
    if (rightString[0] != '-') {
      out << "\\oplus ";
    }
  }
  out << rightString;
  return true;
}

bool Expression::toStringTimes(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  return
  Expression::toStringTimesInContext(
    input, out, format, outputProperties, false
  );
}

bool Expression::toStringTimesInContext(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties,
  bool outputMustStartMultiplicatively
) {
  if (!input.startsWith(input.owner->opTimes(), 3)) {
    return false;
  }
  if (outputProperties != nullptr) {
    outputProperties->needsParenthesesForMultiplicationOnTheRight = false;
  }
  MathExpressionFormattingProperties leftProperties;
  MathExpressionFormattingProperties rightProperties;
  std::string secondString = input[2].toString(format, &rightProperties);
  if (input[1].isOperationGiven(input.owner->opSqrt())) {
    // A malformed expression such as: "\sqrt 3" will be parsed as "sqrt * 3"
    // and later corrected to "\sqrt{3}".
    out << "\\sqrt{" << secondString << "}";
    return true;
  }
  std::string firstString = input[1].toString(format, &leftProperties);
  bool firstNeedsParentheses =
  leftProperties.needsParenthesesForMultiplicationOnTheRight;
  bool secondNeedsParentheses =
  input[2].needsParenthesisForMultiplicationWhenSittingOnTheRightMost(
    &(input[1])
  );
  if (secondString.size() > 0) {
    if (secondString[0] == '-') {
      secondNeedsParentheses = true;
    }
  }
  bool mustHaveTimes = false;
  bool collapseUnits = true;
  if (format != nullptr) {
    collapseUnits = !format->flagDontCollalpseProductsByUnits;
  }
  if (collapseUnits) {
    if (firstString == "-1" || firstString == "- 1") {
      firstString = "-";
      firstNeedsParentheses = false;
    }
    if (firstString == "1") {
      firstString = "";
    }
  }
  if (outputMustStartMultiplicatively && leftProperties.startsWithMinus) {
    firstNeedsParentheses = true;
  }
  if (firstNeedsParentheses) {
    out << "\\left(" << firstString << "\\right)";
  } else {
    out << firstString;
  }
  if (!firstNeedsParentheses && outputProperties != nullptr) {
    outputProperties->startsWithMinus = leftProperties.startsWithMinus;
    outputProperties->startsWithDigit = leftProperties.startsWithDigit;
    outputProperties->startsWithFraction = leftProperties.startsWithFraction;
  }
  bool mustHaveSpace = true;
  if (
    !firstNeedsParentheses &&
    !secondNeedsParentheses &&
    firstString != "" &&
    firstString != "-"
  ) {
    if (MathRoutines::isDigit(secondString[0])) {
      mustHaveTimes = true;
    }
    if (MathRoutines::isDigit(firstString[firstString.size() - 1])) {
      if (StringRoutines::stringBeginsWith(secondString, "\\frac")) {
        mustHaveTimes = true;
      }
    }
    if (rightProperties.needsProductSignForMultiplication) {
      mustHaveTimes = true;
    }
  }
  if (
    firstNeedsParentheses ||
    mustHaveTimes ||
    firstString == "" ||
    firstString == "-"
  ) {
    mustHaveSpace = false;
  }
  if (input[1].isSumLikeOperatorAtom()) {
    mustHaveTimes = false;
  }
  if (mustHaveTimes) {
    out << "\\cdot ";
  } else if (mustHaveSpace) {
    out << " ";
  }
  if (secondNeedsParentheses) {
    out << "\\left(" << secondString << "\\right)";
  } else {
    out << secondString;
  }
  return true;
}

void Expression::toStringOpMultiplicative(
  std::stringstream& out,
  const std::string& operation,
  FormatExpressions* format
) const {
  MathExpressionFormattingProperties secondProperties;
  MathExpressionFormattingProperties firstProperties;
  std::string secondE = (*this)[2].toString(format, &secondProperties);
  std::string firstE = (*this)[1].toString(format, &firstProperties);
  bool firstNeedsBrackets =
  firstProperties.needsParenthesesForMultiplicationOnTheRight;
  bool secondNeedsBrackets =
  secondProperties.needsParenthesesForMultiplicationOnTheRight;
  if (firstNeedsBrackets) {
    out << "\\left(" << firstE << "\\right)";
  } else {
    out << firstE;
  }
  out << operation << " ";
  if (secondNeedsBrackets) {
    out << "\\left(" << secondE << "\\right)";
  } else {
    out << secondE;
  }
}

bool Expression::toStringSequence(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  if (!input.isListStartingWithAtom(input.owner->opSequence())) {
    return false;
  }
  if (outputProperties != nullptr) {
    outputProperties->needsParenthesesForMultiplicationOnTheRight = false;
  }
  if (input.size() == 2) {
    out << "{Sequence{}";
  }
  out << "\\left(";
  int charCounter = 0;
  bool allowNewLine = false;
  if (format != nullptr) {
    allowNewLine = format->flagExpressionNewLineAllowed;
  }
  for (int i = 1; i < input.size(); i ++) {
    std::string currentChildString = input[i].toString(format);
    out << currentChildString;
    charCounter += currentChildString.size();
    if (i != input.children.size - 1) {
      out << ", ";
      if (format != nullptr) {
        if (allowNewLine && charCounter > 50) {
          if (format->flagUseLatex) {
            out << "\\\\\n";
          }
          if (format->flagUseHTML) {
            out << "\n<br>\n";
          }
        }
      }
    }
    charCounter %= 50;
  }
  out << "\\right)";
  if (input.size() == 2) {
    out << "}";
  }
  return true;
}

bool Expression::toStringMatrix(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) {
  if (outputProperties != nullptr) {
    outputProperties->needsParenthesesForMultiplicationOnTheRight = false;
  }
  if (!input.isMatrix()) {
    return false;
  }
  if (format->flagUseLatex && !format->flagUsePmatrix) {
    out << "\\left(";
  }
  if (!format->flagUsePmatrix) {
    int numberOfColumns = input.getNumberOfColumns();
    out << "\\begin{array}{";
    for (int i = 0; i < numberOfColumns; i ++) {
      out << "c";
    }
    out << "} ";
  } else {
    out << "\\begin{pmatrix}";
  }
  for (int i = 1; i < input.size(); i ++) {
    for (int j = 1; j < input[i].size(); j ++) {
      out << input[i][j].toString(format);
      if (j != input[i].size() - 1) {
        out << " & ";
      }
    }
    if (i != input.size() - 1) {
      out << "\\\\ \n";
    }
  }
  if (format->flagUsePmatrix) {
    out << " \\end{pmatrix}";
  } else {
    out << " \\end{array}";
  }
  if (format->flagUseLatex && !format->flagUsePmatrix) {
    out << "\\right)";
  }
  return true;
}

bool Expression::toStringExpressionPrintoutIsDenotedBy(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.isListStartingWithAtom(input.owner->opIsDenotedBy())) {
    return false;
  }
  output.addChildIndex(childrenIndices[1]).addString("=:").addChildIndex(
    childrenIndices[2]
  );
  return true;
}

bool Expression::toStringExpressionPrintoutLnAbsoluteInsteadOfLogarithm(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (
    !input.owner->flagUseLnAbsInsteadOfLogForIntegrationNotation ||
    !input.startsWith(input.owner->opLog(), 2)
  ) {
    return false;
  }
  ExpressionPrintout& printout = output.getOwner();
  int argumentIndex = childrenIndices[1];
  ExpressionPrintoutNode argument = printout.allPrintouts[argumentIndex];
  if (!argument.properties.startsWithAbsoluteValue) {
    output.addString("\\ln \\left|").addChildIndex(argumentIndex).addString(
      "\\right|"
    );
  } else {
    output.addString("\\ln ").addChildIndex(argumentIndex);
  }
  return true;
}

bool Expression::toStringExpressionPrintoutLogBase(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opLogBase(), 3)) {
    return false;
  }
  output.addString("\\log_{").addChildIndex(childrenIndices[1]).addString(
    "}"
  ).addString("\\left(").addChildIndex(childrenIndices[2]).addString(
    "\\right)"
  );
  return true;
}

bool Expression::toStringExpressionPrintoutIntervalOpen(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opIntervalOpen(), 3)) {
    return false;
  }
  if (!input.owner->flagUseBracketsForIntervals) {
    output.addString("IntervalOpen{}");
  }
  output.addString("\\left(").addChildIndex(childrenIndices[1]).addString(
    ", "
  ).addChildIndex(childrenIndices[2]).addString("\\right)");
  return true;
}

bool Expression::toStringExpressionPrintoutIntervalRightClosed(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (input.startsWith(input.owner->opIntervalRightClosed(), 3)) {
    output.addString("\\left(").addChildIndex(childrenIndices[1]).addString(
      ", "
    ).addChildIndex(childrenIndices[2]).addString("\\right]");
    return true;
  }
  if (input.startsWith(input.owner->opIntervalRightClosed(), 2)) {
    if (input[1].isSequenceNElements(2)) {
      ExpressionPrintoutNode& firstChild =
      output.getOwner().allPrintouts[childrenIndices[1]];
      output.addString("\\left(").addChildIndex(
        firstChild.childrenExpressionIndices[1]
      ).addString(", ").addChildIndex(
        firstChild.childrenExpressionIndices[2]
      ).addString("\\right]");
    } else {
      output.addString("IntervalRightClosed{}").addChildIndex(
        childrenIndices[1]
      );
    }
    return true;
  }
  return false;
}

bool Expression::toStringExpressionPrintoutIntervalLeftClosed(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (input.startsWith(input.owner->opIntervalLeftClosed(), 2)) {
    if (input[1].isSequenceNElements(2)) {
      ExpressionPrintoutNode& firstChild =
      output.getOwner().allPrintouts[childrenIndices[1]];
      output.addString("\\left[").addChildIndex(
        firstChild.childrenExpressionIndices[1]
      ).addString(", ").addChildIndex(
        firstChild.childrenExpressionIndices[2]
      ).addString("\\right)");
    } else {
      output.addString("IntervalLeftClosed{}").addChildIndex(
        childrenIndices[1]
      );
    }
    return true;
  }
  if (input.startsWith(input.owner->opIntervalLeftClosed(), 3)) {
    output.addString("\\left[").addChildIndex(childrenIndices[1]).addString(
      ", "
    ).addChildIndex(childrenIndices[2]).addString("\\right)");
    return true;
  }
  return false;
}

bool Expression::toStringExpressionPrintoutIntervalClosed(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (input.startsWith(input.owner->opIntervalClosed(), 2)) {
    if (input[1].isSequenceNElements(2)) {
      ExpressionPrintoutNode& firstChild =
      output.getOwner().allPrintouts[childrenIndices[1]];
      output.addString("\\left[").addChildIndex(
        firstChild.childrenExpressionIndices[1]
      ).addString(", ").addChildIndex(
        firstChild.childrenExpressionIndices[2]
      ).addString("\\right]");
    } else {
      output.addString("IntervalClosed{}").addChildIndex(childrenIndices[1]);
    }
    return true;
  }
  if (input.startsWith(input.owner->opIntervalClosed(), 3)) {
    output.addString("\\left[").addChildIndex(childrenIndices[1]).addString(
      ", "
    ).addChildIndex(childrenIndices[2]).addString("\\right]");
    return true;
  }
  return false;
}

bool Expression::toStringExpressionPrintoutQuote(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opQuote(), 2)) {
    return false;
  }
  std::string content;
  if (input[1].isOperation(&content)) {
    output.addString("\"").addChildIndex(childrenIndices[1]).addString("\"");
  } else {
    output.addString("(Corrupt string)");
  }
  return true;
}

bool Expression::toStringExpressionPrintoutDefineConditional(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opDefineConditional(), 4)) {
    return false;
  }
  output.addChildIndex(childrenIndices[1]).addString(" :if \\left(").
  addChildIndex(childrenIndices[2]).addString("\\right)=").addChildIndex(
    childrenIndices[3]
  );
  return true;
}

bool Expression::toStringExpressionPrintoutDivide(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  Calculator& commands = *input.owner;
  if (!input.startsWith(commands.opDivide(), 3)) {
    return false;
  }
  if (input[1] == "Differential") {
    output.properties.needsParenthesesForMultiplicationOnTheRight = false;
  }
  bool doUseFrac = !commands.flagDontUseFracInRational;
  if (
    doUseFrac && (
      input[1].startsWith(commands.opTimes()) ||
      input[1].startsWith(commands.opDivide())
    )
  ) {
    List<Expression> multiplicandsLeft;
    input.getMultiplicandsDivisorsRecursive(multiplicandsLeft, 0);
    for (int i = 0; i < multiplicandsLeft.size; i ++) {
      if (
        multiplicandsLeft[i].startsWith(commands.opIntegral()) ||
        multiplicandsLeft[i].isOperationGiven(commands.opIntegral())
      ) {
        doUseFrac = false;
        break;
      }
    }
  }
  if (!doUseFrac) {
    bool firstNeedsBrackets =
    !(
      input[1].isListStartingWithAtom(commands.opTimes()) ||
      input[1].isListStartingWithAtom(commands.opDivide())
    );
    bool secondNeedsBrackets = true;
    if (input[2].isOfType<Rational>()) {
      if (input[2].getValue<Rational>().isInteger()) {
        secondNeedsBrackets = false;
      }
    }
    if (firstNeedsBrackets) {
      output.addString("(").addChildIndex(childrenIndices[1]).addString(")");
    } else {
      output.addChildIndex(childrenIndices[1]);
    }
    output.addString("/");
    if (secondNeedsBrackets) {
      output.addString("(").addChildIndex(childrenIndices[2]).addString(")");
    } else {
      output.addChildIndex(childrenIndices[2]);
    }
  } else {
    StateMaintainer<bool> maintain;
    if (format != nullptr) {
      maintain.initialize(format->flagExpressionNewLineAllowed);
      format->flagExpressionNewLineAllowed = false;
    } else {
      // Compilers will complain at certain optimization levels about
      // maintain not being initialized.
      maintain.toMaintain = nullptr;
      maintain.contentAtStart = false;
    }
    output.addString("\\frac{").addChildIndex(childrenIndices[1]).addString(
      "}{"
    ).addChildIndex(childrenIndices[2]).addString("}");
  }
  return true;
}

bool Expression::toStringExpressionPrintoutOpMultiplicative(
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  const std::string& operation,
  FormatExpressions* format
) const {
  MathExpressionFormattingProperties secondProperties;
  MathExpressionFormattingProperties firstProperties;
  std::string secondE = (*this)[2].toString(format, &secondProperties);
  std::string firstE = (*this)[1].toString(format, &firstProperties);
  bool firstNeedsBrackets =
  firstProperties.needsParenthesesForMultiplicationOnTheRight;
  bool secondNeedsBrackets =
  secondProperties.needsParenthesesForMultiplicationOnTheRight;
  if (firstNeedsBrackets) {
    output.addString("\\left(").addChildIndex(childrenIndices[1]).addString(
      "\\right)"
    );
  } else {
    output.addChildIndex(childrenIndices[1]);
  }
  output.addString(operation).addString(" ");
  if (secondNeedsBrackets) {
    output.addString("\\left(").addChildIndex(childrenIndices[2]).addString(
      "\\right)"
    );
  } else {
    output.addChildIndex(childrenIndices[2]);
  }
  return true;
}

bool Expression::toStringExpressionPrintoutTensor(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opTensor(), 3)) {
    return false;
  }
  input.toStringExpressionPrintoutOpMultiplicative(
    output, childrenIndices, "\\otimes", format
  );
  return true;
}

bool Expression::toStringExpressionPrintoutIn(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opIn(), 3)) {
    return false;
  }
  output.addChildIndex(childrenIndices[1]).addString("\\in ").addChildIndex(
    childrenIndices[2]
  );
  return true;
}

bool Expression::toStringExpressionPrintoutOr(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opOr(), 3)) {
    return false;
  }
  if (!input[1].isSequenceNElements()) {
    output.addString("\\left(").addChildIndex(childrenIndices[1]).addString(
      "\\right)"
    );
  } else {
    output.addChildIndex(childrenIndices[1]);
  }
  output.addString(" ~or~ ");
  if (!input[2].isSequenceNElements()) {
    output.addString("\\left(").addChildIndex(childrenIndices[2]).addString(
      "\\right)"
    );
  } else {
    output.addChildIndex(childrenIndices[2]);
  }
  return true;
}

bool Expression::toStringExpressionPrintoutAnd(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opAnd(), 3)) {
    return false;
  }
  output.addString("\\left(").addChildIndex(childrenIndices[1]).addString(
    "\\right) and \\left("
  ).addChildIndex(childrenIndices[2]).addString("\\right)");
  return true;
}

bool Expression::toStringExpressionPrintoutBinom(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opBinom(), 3)) {
    return false;
  }
  output.addString("\\binom{").addChildIndex(childrenIndices[1]).addString(
    "}{ "
  ).addChildIndex(childrenIndices[2]).addString("}");
  return true;
}

bool Expression::toStringExpressionPrintoutUnderscore(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opUnderscore(), 3)) {
    return false;
  }
  output.properties.needsParenthesesForMultiplicationOnTheRight = false;
  output.properties.needsParenthesesWhenLastAndMultipliedOnTheLeft = false;
  input.checkInitialization();
  const Expression& firstE = input[1];
  int opLimit = input.owner->opLimitBoundary();
  if (firstE.startsWith(opLimit) || firstE.isOperationGiven(opLimit)) {
    output.addString("(\\ ").addChildIndex(childrenIndices[1]).addString(
      ")_{"
    ).addChildIndex(childrenIndices[2]).addString("}");
    return true;
  }
  output.addString("{").addChildIndex(childrenIndices[1]).addString("}_{").
  addChildIndex(childrenIndices[2]).addString("}");
  return true;
}

bool Expression::toStringExpressionPrintoutSetMinus(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opSetMinus(), 3)) {
    return false;
  }
  output.addChildIndex(childrenIndices[1]).addString("\\setminus ").
  addChildIndex(childrenIndices[2]);
  return true;
}

bool Expression::toStringExpressionPrintoutLimitBoundary(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opLimitBoundary(), 3)) {
    return false;
  }
  output.addString("\\limits_{").addChildIndex(childrenIndices[1]).addString(
    "}^{"
  ).addChildIndex(childrenIndices[2]).addString("}");
  return true;
}

bool Expression::toStringExpressionPrintoutTimes(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  bool outputMustStartMultiplicatively = false;
  if (!input.startsWith(input.owner->opTimes(), 3)) {
    return false;
  }
  output.properties.needsParenthesesForMultiplicationOnTheRight = false;
  MathExpressionFormattingProperties& leftProperties =
  output.getOwner().allPrintouts[childrenIndices[1]].properties;
  MathExpressionFormattingProperties& rightProperties =
  output.getOwner().allPrintouts[childrenIndices[2]].properties;
  if (input[1].isOperationGiven(input.owner->opSqrt())) {
    // A malformed expression such as: "\sqrt 3" will be parsed as "sqrt * 3"
    // and later corrected to "\sqrt{3}".
    output.addString("\\sqrt{").addChildIndex(childrenIndices[2]).addString(
      "}"
    );
    return true;
  }
  bool firstNeedsParentheses =
  leftProperties.needsParenthesesForMultiplicationOnTheRight;
  bool secondNeedsParentheses =
  input[2].needsParenthesisForMultiplicationWhenSittingOnTheRightMost(
    &(input[1])
  );
  if (rightProperties.startsWithMinus) {
    secondNeedsParentheses = true;
  }
  bool mustHaveTimes = false;
  bool collapseUnits = true;
  if (format != nullptr) {
    collapseUnits = !format->flagDontCollalpseProductsByUnits;
  }
  bool isCollapsed = false;
  std::string collapsedString;
  if (collapseUnits) {
    if (leftProperties.isNegativeOne) {
      collapsedString = "-";
      firstNeedsParentheses = false;
      isCollapsed = true;
    }
    if (leftProperties.isOne) {
      collapsedString = "";
      isCollapsed = true;
      firstNeedsParentheses = false;
    }
  }
  if (outputMustStartMultiplicatively && leftProperties.startsWithMinus) {
    firstNeedsParentheses = true;
  }
  if (firstNeedsParentheses) {
    output.addString("\\left(").addChildIndex(childrenIndices[1]).addString(
      "\\right)"
    );
  } else if (isCollapsed) {
    output.addString(collapsedString);
  } else {
    output.addChildIndex(childrenIndices[1]);
  }
  if (!firstNeedsParentheses) {
    output.properties.startsWithMinus = leftProperties.startsWithMinus;
    output.properties.startsWithDigit = leftProperties.startsWithDigit;
    output.properties.startsWithFraction = leftProperties.startsWithFraction;
  }
  bool mustHaveSpace = true;
  if (
    !isCollapsed && !secondNeedsParentheses && rightProperties.startsWithDigit
  ) {
    mustHaveTimes = true;
  }
  if (!firstNeedsParentheses && !secondNeedsParentheses && !isCollapsed) {
    if (leftProperties.endsWithDigit && rightProperties.startsWithFraction) {
      mustHaveTimes = true;
    }
    if (rightProperties.needsProductSignForMultiplication) {
      mustHaveTimes = true;
    }
  }
  if (firstNeedsParentheses || mustHaveTimes || isCollapsed) {
    mustHaveSpace = false;
  }
  if (input[1].isSumLikeOperatorAtom()) {
    mustHaveTimes = false;
  }
  if (mustHaveTimes) {
    output.addString("\\cdot ");
  } else if (mustHaveSpace) {
    output.addString(" ");
  }
  if (secondNeedsParentheses) {
    output.addString("\\left(").addChildIndex(childrenIndices[2]).addString(
      "\\right)"
    );
  } else {
    output.addChildIndex(childrenIndices[2]);
  }
  return true;
}

bool Expression::toStringExpressionPrintoutCrossProduct(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opCrossProduct())) {
    return false;
  }
  input.toStringExpressionPrintoutOpMultiplicative(
    output, childrenIndices, "\\times", format
  );
  return true;
}

bool Expression::toStringExpressionPrintoutSqrt(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opSqrt())) {
    return false;
  }
  if (input.size() == 2) {
    return
    Expression::toStringExpressionPrintoutSqrt2(
      input, output, childrenIndices, format
    );
  }
  return
  Expression::toStringExpressionPrintoutSqrt3(
    input, output, childrenIndices, format
  );
}

bool Expression::toStringExpressionPrintoutSqrt2(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opSqrt(), 2)) {
    return false;
  }
  output.addString("\\sqrt{").addChildIndex(childrenIndices[1]).addString(
    "}"
  );
  return true;
}

bool Expression::toStringExpressionPrintoutSqrt3(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opSqrt(), 3)) {
    return false;
  }
  int power = 0;
  bool hasPowerTwo = input[1].isSmallInteger(&power);
  if (hasPowerTwo) {
    hasPowerTwo = (power == 2);
  }
  if (hasPowerTwo) {
    output.addString("\\sqrt{").addChildIndex(childrenIndices[2]).addString(
      "}"
    );
  } else {
    output.addString("\\sqrt[").addChildIndex(childrenIndices[1]).addString(
      "]{"
    ).addChildIndex(childrenIndices[2]).addString("}");
  }
  return true;
}

bool Expression::toStringExpressionPrintoutFactorial(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opFactorial(), 2)) {
    return false;
  }
  if (input[1].needsParenthesisForBaseOfExponent()) {
    output.addString("\\left(").addChildIndex(childrenIndices[1]).addString(
      "\\right) !"
    );
  } else {
    output.addChildIndex(childrenIndices[1]).addString("!");
  }
  return true;
}

bool Expression::toStringExpressionPrintoutAbsoluteValue(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opAbsoluteValue(), 2)) {
    return false;
  }
  output.properties.needsParenthesesForMultiplicationOnTheRight = false;
  output.addString("\\left|").addChildIndex(childrenIndices[1]).addString(
    "\\right|"
  );
  return true;
}

bool Expression::toStringExpressionPrintoutPower(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  STACK_TRACE("Expression::toStringExpressionPrintoutPower");
  (void) format;
  input.checkInitialization();
  Calculator& calculator = *input.owner;
  if (!input.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  const Expression& baseExpression = input[1];
  const Expression& exponentExpression = input[2];
  bool isSuperScriptOfUnderscoredOperator = false;
  if (baseExpression.startsWith(calculator.opUnderscore(), 3)) {
    if (baseExpression[1].isOperationGiven(calculator.opIntegral())) {
      isSuperScriptOfUnderscoredOperator = true;
    }
  }
  if (
    baseExpression.children.size > 0 &&
    baseExpression[0].isAtomWhoseExponentsAreInterpretedAsFunction()
  ) {
    output.properties.needsParenthesesForMultiplicationOnTheRight = true;
    output.properties.needsParenthesesWhenLastAndMultipliedOnTheLeft = false;
  } else {
    output.properties.needsParenthesesForMultiplicationOnTheRight = false;
    output.properties.needsParenthesesWhenLastAndMultipliedOnTheLeft = false;
  }
  bool involvesExponentsInterpretedAsFunctions = false;
  if (baseExpression.startsWith(- 1, 2)) {
    involvesExponentsInterpretedAsFunctions =
    baseExpression[0].isAtomWhoseExponentsAreInterpretedAsFunction() &&
    !exponentExpression.isEqualToMOne() &&
    exponentExpression.isRational();
    if (
      involvesExponentsInterpretedAsFunctions &&
      baseExpression[0].isOperationGiven(calculator.opLog()) &&
      calculator.flagUseLnAbsInsteadOfLogForIntegrationNotation
    ) {
      involvesExponentsInterpretedAsFunctions = false;
    }
  }
  if (involvesExponentsInterpretedAsFunctions) {
    ExpressionPrintoutNode& basePrintout =
    output.getOwner().allPrintouts[childrenIndices[1]];
    output.addString("{").addChildIndex(
      basePrintout.childrenExpressionIndices[0]
    ).addString("^{").addChildIndex(childrenIndices[2]).addString("}}{}");
    bool needsParentheses = baseExpression.size() != 2 ||
    !baseExpression[1].isAtom();
    if (needsParentheses) {
      output.addString("\\left(");
    }
    for (int i = 1; i < baseExpression.children.size; i ++) {
      output.addChildIndex(basePrintout.childrenExpressionIndices[i]);
    }
    if (needsParentheses) {
      output.addString("\\right)");
    }
    return true;
  }
  if (isSuperScriptOfUnderscoredOperator) {
    ExpressionPrintoutNode& basePrintout =
    output.getOwner().allPrintouts[childrenIndices[1]];
    output.addString("{").addChildIndex(
      basePrintout.childrenExpressionIndices[1]
    ).addString("}_{").addChildIndex(
      basePrintout.childrenExpressionIndices[2]
    ).addString("}^{").addChildIndex(childrenIndices[2]).addString("}");
    return true;
  }
  int opLimit = calculator.opLimitBoundary();
  if (
    baseExpression.startsWith(opLimit) ||
    baseExpression.isOperationGiven(opLimit)
  ) {
    output.addString("(\\ ").addChildIndex(childrenIndices[1]).addString(
      ")^{"
    ).addChildIndex(childrenIndices[2]).addString("}");
    return true;
  }
  bool isSqrt = false;
  if (exponentExpression.isOfType<Rational>()) {
    if (exponentExpression.getValue<Rational>().isEqualTo(Rational(1, 2))) {
      isSqrt = true;
    }
  }
  if (isSqrt) {
    output.addString("\\sqrt{").addChildIndex(childrenIndices[1]).addString(
      "}"
    );
    return true;
  }
  ExpressionPrintoutNode& basePrintout =
  output.getOwner().allPrintouts[childrenIndices[1]];
  if (baseExpression.needsParenthesisForBaseOfExponent()) {
    bool useBigParenthesis = true;
    if (baseExpression.startsWith(calculator.opDivide())) {
      useBigParenthesis = true;
    }
    if (useBigParenthesis) {
      output.addString("\\left(");
    } else {
      output.addString("(");
    }
    output.addChildIndex(childrenIndices[1]);
    if (useBigParenthesis) {
      output.addString("\\right)");
    } else {
      output.addString(")");
    }
  } else {
    output.addChildIndex(childrenIndices[1]);
    output.properties.startsWithDigit =
    basePrintout.properties.startsWithDigit;
  }
  output.addString("^{").addChildIndex(childrenIndices[2]).addString("}");
  return true;
}

bool Expression::toStringExpressionPrintoutPlus(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opPlus())) {
    return false;
  }
  output.properties.needsParenthesesForMultiplicationOnTheRight = true;
  if (input.children.size < 3) {
    global.fatal
    << "Plus operation takes at least 2 arguments, "
    << "whereas this expression has "
    << input.children.size - 1
    << " arguments. "
    << global.fatal;
  }
  for (int i = 1; i < input.size(); i ++) {
    if (i >= childrenIndices.size) {
      output.addString("...");
      return true;
    }
    int printoutIndex = childrenIndices[i];
    ExpressionPrintoutNode& childPrintout =
    output.getOwner().allPrintouts[printoutIndex];
    if (
      i > 1 && (
        childPrintout.properties.needsParenthesesForAddition ||
        !childPrintout.properties.startsWithMinus
      )
    ) {
      output.addString("+");
    }
    if (childPrintout.properties.needsParenthesesForAddition) {
      output.addString("\\left(").addChildIndex(printoutIndex).addString(
        "\\right)"
      );
    } else {
      output.addChildIndex(printoutIndex);
    }
  }
  return true;
}

bool Expression::toStringExpressionPrintoutDirectSum(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  if (!input.isListStartingWithAtom(input.owner->opDirectSum())) {
    return false;
  }
  if (input.children.size < 3) {
    global.fatal
    << "Direct sum operation takes at least 2 arguments, "
    << "whereas this expression has "
    << input.children.size
    << " arguments. "
    << global.fatal;
  }
  ExpressionPrintoutNode& leftPrintout =
  output.getOwner().allPrintouts[childrenIndices[1]];
  ExpressionPrintoutNode& rightPrintout =
  output.getOwner().allPrintouts[childrenIndices[2]];
  std::string leftString;
  if (leftPrintout.properties.needsParenthesesForAddition) {
    output.addString("\\left(").addChildIndex(childrenIndices[1]).addString(
      "\\right)"
    );
  } else {
    output.addChildIndex(childrenIndices[1]);
  }
  bool allowNewLine = false;
  if (format != nullptr) {
    allowNewLine = format->flagExpressionNewLineAllowed;
  }
  bool useFrac = !input.owner->flagDontUseFracInRational;
  if (
    allowNewLine &&
    !useFrac &&
    leftPrintout.stringSize > FormatExpressions::expressionLineBreak
  ) {
    output.addString("\\\\\n");
  }
  if (
    rightPrintout.properties.needsParenthesesForAddition ||
    !rightPrintout.properties.startsWithMinus
  ) {
    output.addString("\\oplus");
  }
  if (rightPrintout.properties.needsParenthesesForAddition) {
    output.addString("\\left(").addChildIndex(childrenIndices[2]).addString(
      "\\right)"
    );
  } else {
    output.addChildIndex(childrenIndices[2]);
  }
  return true;
}

bool Expression::toStringExpressionPrintoutMinus(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opMinus())) {
    return false;
  }
  output.properties.needsParenthesesForMultiplicationOnTheRight = true;
  if (input.size() == 2) {
    return
    Expression::toStringExpressionPrintoutMinus2(
      input, output, childrenIndices, format
    );
  }
  return
  Expression::toStringExpressionPrintoutMinus3(
    input, output, childrenIndices, format
  );
}

bool Expression::toStringExpressionPrintoutBind(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opBind(), 2)) {
    return false;
  }
  output.addString("{{").addChildIndex(childrenIndices[1]).addString("}}");
  output.properties =
  output.getOwner().allPrintouts[childrenIndices[1]].properties;
  return true;
}

bool Expression::toStringExpressionPrintoutEqualEqual(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.isListStartingWithAtom(input.owner->opEqualEqual())) {
    return false;
  }
  output.addChildIndex(childrenIndices[1]).addString("==").addChildIndex(
    childrenIndices[2]
  );
  return true;
}

bool Expression::toStringExpressionPrintoutEqualEqualEqual(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.isListStartingWithAtom(input.owner->opEqualEqualEqual())) {
    return false;
  }
  output.addChildIndex(childrenIndices[1]).addString("===").addChildIndex(
    childrenIndices[2]
  );
  return true;
}

bool Expression::toStringExpressionPrintoutDifferentiate(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opDifferentiate(), 3)) {
    return false;
  }
  output.addString("\\frac{\\text{d} ");
  ExpressionPrintoutNode& rightPrintout =
  output.getOwner().allPrintouts[childrenIndices[2]];
  if (rightPrintout.properties.needsParenthesesForMultiplicationOnTheRight) {
    output.addString("\\left(").addChildIndex(childrenIndices[2]).addString(
      "\\right)"
    );
  } else {
    output.addChildIndex(childrenIndices[2]);
  }
  output.addString("}{\\text{d} ");
  output.addChildIndex(childrenIndices[1]);
  output.addString("}");
  return true;
}

bool Expression::toStringExpressionPrintoutDifferential(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opDifferential())) {
    return false;
  }
  if (input.size() == 2) {
    return
    Expression::toStringExpressionPrintoutDifferential2(
      input, output, childrenIndices, format
    );
  }
  return
  Expression::toStringExpressionPrintoutDifferential3(
    input, output, childrenIndices, format
  );
}

bool Expression::toStringExpressionPrintoutSumOrIntegral(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  STACK_TRACE("Expression::toStringSumOrIntegral");
  if (
    !input.startsWith(input.owner->opSum()) &&
    !input.startsWith(input.owner->opIntegral())
  ) {
    return false;
  }
  output.addChildIndex(childrenIndices[0]).addString(" ");
  ExpressionPrintoutNode& limits =
  output.getOwner().allPrintouts[childrenIndices[1]];
  int firstIndex = 2;
  if (input.size() >= 2) {
    if (input[1].startsWith(input.owner->opLimitBoundary(), 3)) {
      output.addString("_{").addChildIndex(
        limits.childrenExpressionIndices[1]
      ).addString("}^{").addChildIndex(limits.childrenExpressionIndices[2]).
      addString("}");
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
    output.addString("(");
    for (int i = firstIndex; i < input.size(); i ++) {
      output.addChildIndex(childrenIndices[i]);
      if (i != input.size() - 1) {
        output.addString(", ");
      }
    }
    output.addString(")");
    return true;
  }
  const Expression& differential = input[firstIndex];
  if (!differential.startsWith(input.owner->opTimes(), 3)) {
    MathExpressionFormattingProperties differentialProperties;
    std::string differentialString =
    differential.toString(format, &differentialProperties);
    if (differentialProperties.startsWithMinus) {
      output.addString("\\left(").addChildIndex(childrenIndices[firstIndex]).
      addString("\\right)");
    } else {
      output.addChildIndex(childrenIndices[firstIndex]);
    }
    return true;
  }
  output.addString("\\left(").addChildIndex(childrenIndices[firstIndex]).
  addString("\\right)");
  return true;
}

bool Expression::toStringExpressionPrintoutGreaterThan(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.isListStartingWithAtom(input.owner->opGreaterThan())) {
    return false;
  }
  output.addChildIndex(childrenIndices[1]).addString("&gt;").addChildIndex(
    childrenIndices[2]
  );
  return true;
}

bool Expression::toStringExpressionPrintoutGreaterThanOrEqualTo(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.isListStartingWithAtom(input.owner->opGreaterThanOrEqualTo())) {
    return false;
  }
  output.addChildIndex(childrenIndices[1]).addString("\\geq ").addChildIndex(
    childrenIndices[2]
  );
  return true;
}

bool Expression::toStringExpressionPrintoutLessThanOrEqualTo(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.isListStartingWithAtom(input.owner->opLessThanOrEqualTo())) {
    return false;
  }
  output.addChildIndex(childrenIndices[1]).addString("\\leq ").addChildIndex(
    childrenIndices[2]
  );
  return true;
}

bool Expression::toStringExpressionPrintoutLimit(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opLimit(), 3)) {
    return false;
  }
  output.properties.needsParenthesesForMultiplicationOnTheRight = true;
  output.addString("\\lim_{");
  if (!input[1].isSequenceNElements()) {
    output.addChildIndex(childrenIndices[1]);
  } else {
    ExpressionPrintoutNode& child =
    output.getOwner().allPrintouts[childrenIndices[1]];
    for (int i = 1; i < input[1].size(); i ++) {
      output.addChildIndex(child.childrenExpressionIndices[i]);
      if (i != input[1].size() - 1) {
        output.addString(", ");
      }
    }
  }
  output.addString("}");
  output.addChildIndex(childrenIndices[2]);
  return true;
}

bool Expression::toStringExpressionPrintoutLimitProcess(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.isListStartingWithAtom(input.owner->opLimitProcess())) {
    return false;
  }
  output.addChildIndex(childrenIndices[1]).addString(" \\to ").addChildIndex(
    childrenIndices[2]
  );
  return true;
}

bool Expression::toStringExpressionPrintoutLessThan(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.isListStartingWithAtom(input.owner->opLessThan())) {
    return false;
  }
  output.addChildIndex(childrenIndices[1]).addString("&lt;").addChildIndex(
    childrenIndices[2]
  );
  return true;
}

bool Expression::toStringExpressionPrintoutSequence(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  if (!input.isListStartingWithAtom(input.owner->opSequence())) {
    return false;
  }
  output.properties.needsParenthesesForMultiplicationOnTheRight = false;
  if (input.size() == 2) {
    output.addString("{Sequence{}");
  }
  output.addString("\\left(");
  int charCounter = 0;
  bool allowNewLine = false;
  if (format != nullptr) {
    allowNewLine = format->flagExpressionNewLineAllowed;
  }
  for (int i = 1; i < input.size(); i ++) {
    ExpressionPrintoutNode& current =
    output.getOwner().allPrintouts[childrenIndices[i]];
    output.addChildIndex(childrenIndices[i]);
    charCounter += current.stringSize;
    if (i != input.children.size - 1) {
      output.addString(", ");
      if (format != nullptr) {
        if (allowNewLine && charCounter > 50) {
          if (format->flagUseLatex) {
            output.addString("\\\\\n");
          }
          if (format->flagUseHTML) {
            output.addString("\n<br>\n");
          }
        }
      }
    }
    charCounter %= 50;
  }
  output.addString("\\right)");
  if (input.size() == 2) {
    output.addString("}");
  }
  return true;
}

bool Expression::toStringExpressionPrintoutLieBracket(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.isListStartingWithAtom(input.owner->opLieBracket())) {
    return false;
  }
  output.addString("[").addChildIndex(childrenIndices[1]).addString(",").
  addChildIndex(childrenIndices[2]).addString("]");
  return true;
}

bool Expression::toStringExpressionPrintoutMod(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.isListStartingWithAtom(input.owner->opMod())) {
    return false;
  }
  if (input.size() == 2) {
    output.addString(" mod ").addChildIndex(childrenIndices[1]);
    return true;
  }
  if (input.size() == 3) {
    output.addChildIndex(childrenIndices[1]).addString(" mod ").addChildIndex(
      childrenIndices[2]
    );
    return true;
  }
  return false;
}

bool Expression::toStringExpressionPrintoutUnion(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (
    !input.isListStartingWithAtom(input.owner->opUnion()) || input.size() != 3
  ) {
    return false;
  }
  if (input[1].startsWith(input.owner->opIntersection())) {
    output.addString("\\left(").addChildIndex(childrenIndices[1]).addString(
      "\\right)"
    ).addString("\\cup ").addChildIndex(childrenIndices[2]);
  } else {
    output.addChildIndex(childrenIndices[1]).addString("\\cup ").addChildIndex(
      childrenIndices[2]
    );
  }
  return true;
}

bool Expression::toStringExpressionPrintoutIntersection(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (
    !input.isListStartingWithAtom(input.owner->opIntersection()) ||
    input.size() != 3
  ) {
    return false;
  }
  if (input[1].startsWith(input.owner->opUnion())) {
    output.addString("\\left(").addChildIndex(childrenIndices[1]).addString(
      "\\right)"
    ).addString("\\cap ").addChildIndex(childrenIndices[2]);
  } else {
    output.addChildIndex(childrenIndices[1]).addString("\\cap ").addChildIndex(
      childrenIndices[2]
    );
  }
  return true;
}

bool Expression::toStringExpressionPrintoutUnionNoRepetition(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.isListStartingWithAtom(input.owner->opUnionNoRepetition())) {
    return false;
  }
  output.addChildIndex(childrenIndices[1]).addString("\\sqcup ").addChildIndex(
    childrenIndices[2]
  );
  return true;
}

bool Expression::toStringExpressionPrintoutError(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.startsWith(input.owner->opError(), 2)) {
    return false;
  }
  input.checkInitialization();
  output.addString("\\text{Error:~").addChildIndex(childrenIndices[1]).
  addString("}");
  return true;
}

bool Expression::toStringExpressionPrintoutMatrix(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  if (!input.isMatrix()) {
    return false;
  }
  output.properties.needsParenthesesForMultiplicationOnTheRight = false;
  if (format->flagUseLatex && !format->flagUsePmatrix) {
    output.addString("\\left(");
  }
  if (!format->flagUsePmatrix) {
    int numberOfColumns = input.getNumberOfColumns();
    output.addString("\\begin{array}{");
    for (int i = 0; i < numberOfColumns; i ++) {
      output.addString("c");
    }
    output.addString("} ");
  } else {
    output.addString("\\begin{pmatrix}");
  }
  for (int i = 1; i < input.size(); i ++) {
    ExpressionPrintoutNode& rowPrintout =
    output.getOwner().allPrintouts[childrenIndices[i]];
    for (int j = 1; j < input[i].size(); j ++) {
      output.addChildIndex(rowPrintout.childrenExpressionIndices[j]);
      if (j != input[i].size() - 1) {
        output.addString(" & ");
      }
    }
    if (i != input.size() - 1) {
      output.addString("\\\\ \n");
    }
  }
  if (format->flagUsePmatrix) {
    output.addString(" \\end{pmatrix}");
  } else {
    output.addString(" \\end{array}");
  }
  if (format->flagUseLatex && !format->flagUsePmatrix) {
    output.addString("\\right)");
  }
  return true;
}

bool Expression::toStringExpressionPrintoutEndStatementNested(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  if (!input.isListStartingWithAtom(input.owner->opCommandSequence())) {
    return false;
  }
  if (input.size() > 2) {
    output.addString("(");
  }
  std::string currentOutput;
  for (int i = 1; i < input.size(); i ++) {
    output.addChildIndex(childrenIndices[i]);
    if (i != input.size() - 1) {
      output.addString(";");
    }
  }
  if (input.size() > 2) {
    output.addString(")");
  }
  return true;
}

bool Expression::toStringExpressionPrintoutDifferential2(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  STACK_TRACE("Expression::toStringExpressionPrintoutDifferential2");
  if (!input.startsWith(input.owner->opDifferential(), 2)) {
    return false;
  }
  bool needsParentheses = (!input[1].isAtom()) && (!input[1].isBuiltInType());
  output.addString("{\\text{d}}{}");
  if (needsParentheses) {
    output.addString("\\left(");
  }
  output.addChildIndex(childrenIndices[1]);
  if (needsParentheses) {
    output.addString("\\right)");
  }
  return true;
}

bool Expression::toStringExpressionPrintoutDifferential3(
  const Expression& input,
  ExpressionPrintoutNode& output,
  const List<int>& childrenIndices,
  FormatExpressions* format
) {
  (void) format;
  STACK_TRACE("Expression::toStringExpressionPrintoutDifferential3");
  if (!input.startsWith(input.owner->opDifferential(), 3)) {
    return false;
  }
  ExpressionPrintoutNode& coefficientPrintout =
  output.getOwner().allPrintouts[childrenIndices[2]];
  bool coefficientNeedsParentheses =
  coefficientPrintout.properties.needsParenthesesForMultiplicationOnTheRight ||
  coefficientPrintout.properties.startsWithMinus;
  if (input[2].startsWith(input.owner->opDivide())) {
    coefficientNeedsParentheses = false;
  }
  bool argumentOfDifferentialNeedsParentheses = (!input[1].isAtom()) && (
    !input[1].isBuiltInType()
  );
  if (coefficientPrintout.properties.isOne) {
    coefficientNeedsParentheses = false;
  }
  if (coefficientNeedsParentheses) {
    output.addString("\\left(");
  }
  if (!coefficientPrintout.properties.isOne) {
    output.addChildIndex(childrenIndices[2]);
  }
  if (coefficientNeedsParentheses) {
    output.addString("\\right)");
  }
  output.addString(" {\\text{d}} ");
  if (argumentOfDifferentialNeedsParentheses) {
    output.addString("\\left(");
  }
  output.addChildIndex(childrenIndices[1]);
  if (argumentOfDifferentialNeedsParentheses) {
    output.addString("\\right)");
  }
  if (coefficientNeedsParentheses) {
    output.properties.startsWithDigit = false;
    output.properties.startsWithMinus = false;
    output.properties.startsWithFraction = false;
  }
  return true;
}

std::string ExpressionPrintoutNode::toString() const {
  std::stringstream out;
  if (this->children.size == 0) {
    out << "[" << this->content << "]";
    return out.str();
  }
  out << "(";
  for (int i = 0; i < this->children.size; i ++) {
    out << this->getOwner().allPrintouts[this->children[i]].toString();
  }
  out << ")";
  return out.str();
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
