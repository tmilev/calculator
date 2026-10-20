#include "calculator_interface.h"

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
  STACK_TRACE("Expression::toStringExpressionPrintoutOpMultiplicative");
  (void) format;
  ExpressionPrintoutNode& secondPrintout =
  output.getOwner().allPrintouts[childrenIndices[2]];
  ExpressionPrintoutNode& firstPrintout =
  output.getOwner().allPrintouts[childrenIndices[1]];
  bool firstNeedsBrackets =
  firstPrintout.properties.needsParenthesesForMultiplicationOnTheRight;
  bool secondNeedsBrackets =
  secondPrintout.properties.needsParenthesesForMultiplicationOnTheRight;
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
  if (firstNeedsParentheses || isCollapsed) {
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
    bool needsParentheses = true;
    if (baseExpression.size() == 2) {
      if (baseExpression[1].isAtom()) {
        needsParentheses = false;
      }
      if (
        baseExpression[1].startsWith(input.owner->opBind(), 2) &&
        baseExpression[1][1].isAtom()
      ) {
        needsParentheses = false;
      }
    }
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
  STACK_TRACE("Expression::toStringExpressionPrintoutSumOrIntegral");
  (void) format;
  if (
    !input.startsWith(input.owner->opSum()) &&
    !input.startsWith(input.owner->opIntegral())
  ) {
    return false;
  }
  output.addChildIndex(childrenIndices[0]).addString(" ");
  if (childrenIndices.size < 2) {
    output.addString("()");
    return true;
  }
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
    ExpressionPrintoutNode& differentialPrintout =
    output.getOwner().allPrintouts[childrenIndices[firstIndex]];
    if (differentialPrintout.properties.startsWithMinus) {
      output.addString("\\left(").addChildIndex(childrenIndices[firstIndex]).
      addString("\\right)");
    } else {
      output.addChildIndex(childrenIndices[firstIndex]);
    }
    return true;
  }
  output.addChildIndex(childrenIndices[firstIndex]);
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
  for (int i = 1; i < childrenIndices.size; i ++) {
    output.addChildIndex(childrenIndices[i]);
    if (i != childrenIndices.size - 1) {
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
