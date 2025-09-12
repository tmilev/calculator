#include "calculator_interface.h"

template < >
bool Expression::toMathMLBuiltIn<Rational>(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  out << input.getValue<Rational>().toMathML(format, nullptr);
  return true;
}

void Expression::initializeToMathMLHandlers(Calculator& toBeInitialized) {
  STACK_TRACE("Expression::initializeToMathMLHandlers");
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opDefine(), Expression::toStringDefine
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opIsDenotedBy(), Expression::toStringIsDenotedBy
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opLog(), Expression::toStringLnAbsoluteInsteadOfLogarithm
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opLogBase(), Expression::toStringLogBase
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opIntervalOpen(), Expression::toStringIntervalOpen
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opIntervalRightClosed(),
    Expression::toStringIntervalRightClosed
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opIntervalLeftClosed(),
    Expression::toStringIntervalLeftClosed
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opIntervalClosed(), Expression::toStringIntervalClosed
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opQuote(), Expression::toStringQuote
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opDefineConditional(),
    Expression::toStringDefineConditional
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opDivide(), Expression::toStringDivide
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opTensor(), Expression::toStringTensor
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opIn(), Expression::toStringIn
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opOr(), Expression::toStringOr
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opAnd(), Expression::toStringAnd
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opBinom(), Expression::toStringBinom
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opUnderscore(), Expression::toMathMLUnderscore
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opSetMinus(), Expression::toStringSetMinus
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opLimitBoundary(), Expression::toStringLimitBoundary
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opTimes(), Expression::toMathMLTimes
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opCrossProduct(), Expression::toStringCrossProduct
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opSqrt(), Expression::toStringSqrt
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opFactorial(), Expression::toStringFactorial
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opAbsoluteValue(), Expression::toStringAbsoluteValue
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opPower(), Expression::toMathMLPower
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opPlus(), Expression::toMathMLPlus
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opDirectSum(), Expression::toStringDirectSum
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opMinus(), Expression::toStringMinus
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opBind(), Expression::toStringBind
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opEqualEqual(), Expression::toStringEqualEqual
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opEqualEqualEqual(), Expression::toStringEqualEqualEqual
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opDifferentiate(), Expression::toStringDifferentiate
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opDifferential(), Expression::toStringDifferential
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opSum(), Expression::toStringSumOrIntegral
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opIntegral(), Expression::toStringSumOrIntegral
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opGreaterThan(), Expression::toStringGreaterThan
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opGreaterThanOrEqualTo(),
    Expression::toStringGreaterThanOrEqualTo
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opLessThanOrEqualTo(),
    Expression::toStringLessThanOrEqualTo
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opLimit(), Expression::toStringLimit
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opLimitProcess(), Expression::toStringLimitProcess
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opLessThan(), Expression::toStringLessThan
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opSequence(), Expression::toMathMLSequence
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opLieBracket(), Expression::toStringLieBracket
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opMod(), Expression::toStringMod
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opUnion(), Expression::toStringUnion
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opIntersection(), Expression::toStringIntersection
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opUnionNoRepetition(),
    Expression::toStringUnionNoRepetition
  );
  toBeInitialized.addOneMathMLAtomHandler(
    toBeInitialized.opError(), Expression::toStringError
  );
  toBeInitialized.addOneMathMLCompositeHandler(
    toBeInitialized.opMatrix(), Expression::toMathMLMatrix
  );
}

std::string Expression::toMathMLFinal() const {
  return MathML::toMathMLFinal(this->toMathML(), this->toString());
}

void Expression::toMathML(
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties,
  Expression* startingExpression,
  bool unfoldCommandEnclosures,
  JSData* outputJS
) const {
  STACK_TRACE("Expression::toMathML");
  RecursionDepthCounter recursionCounter;
  recursionCounter.initialize(&this->owner->recursionDepth);
  this->checkConsistency();
  if (this->owner != nullptr) {
    if (this->owner->recursionDepth + 1 > this->owner->maximumRecursionDepth) {
      out << "<ms>(...)</ms>";
      return;
    }
  } else {
    out << "<ms>(Error:NoOwner)</ms>";
    return;
  }
  MemorySaving<FormatExpressions> formatContainer;
  if (format == nullptr) {
    format = &formatContainer.getElement();
    format->flagUseQuotes = false;
  }
  if (startingExpression != nullptr && unfoldCommandEnclosures) {
    Expression newStart;
    Expression newMe;
    if (
      CalculatorBasics::functionFlattenCommandEnclosuresOneLayer(
        *this->owner, *this, newMe
      ) &&
      CalculatorBasics::functionFlattenCommandEnclosuresOneLayer(
        *this->owner, *startingExpression, newStart
      )
    ) {
      newMe.toMathML(
        out, format, outputProperties, &newStart, false, outputJS
      );
      return;
    }
  }
  if (
    !this->isOfType<std::string>() &&
    !this->startsWith(this->owner->opCommandSequence())
  ) {
    if (startingExpression == nullptr) {
      format->flagUseQuotes = true;
    } else {
      format->flagUseQuotes = false;
    }
  }
  if (outputJS != nullptr) {
    outputJS->reset();
  }
  int notationIndex =
  owner->objectContainer.expressionWithNotation.getIndex(*this);
  if (notationIndex != - 1) {
    out << owner->objectContainer.expressionNotation[notationIndex];
    return;
  }
  if (this->toMathMLData(out, format, outputProperties)) {} else if (
    this->toMathMLWithAtomHandler(out, outputProperties)
  ) {} else if (this->toMathMLWithCompositeHandler(out, outputProperties)) {}
 else if (
    this->toMathMLEndStatement(out, startingExpression, outputJS, format)
  ) {} else if (this->size() == 1) {
    (*this)[0].toMathML(
      out, format, outputProperties, nullptr, false, nullptr
    );
  } else if (this->toMathMLGeneral(out, format, outputProperties)) {} else {
    out << "<ms>(ProgrammingError:NotDocumented)</ms>";
  }
}

bool Expression::toMathMLEndStatement(
  std::stringstream& out,
  Expression* startingExpression,
  JSData* outputJS,
  FormatExpressions* format
) const {
  STACK_TRACE("Expression::toMathMLEndStatement");
  if (!this->isListStartingWithAtom(this->owner->opCommandSequence())) {
    return false;
  }
  if (startingExpression != nullptr) {
    return
    this->toMathMLEndStatementTopLevel(
      out, *startingExpression, outputJS, format
    );
  }
  return this->toMathMLEndStatementNested(out, format);
}

bool Expression::toMathMLEndStatementTopLevel(
  std::stringstream& out,
  Expression& startingExpression,
  JSData* outputJS,
  FormatExpressions* format
) const {
  STACK_TRACE("Expression::toMathMLEndStatementTopLevel");
  MemorySaving<FormatExpressions> temporaryFormat;
  if (format == nullptr) {
    format = &temporaryFormat.getElement();
    format->flagExpressionIsTopLevel = true;
  }
  bool isFinal = format->flagExpressionIsTopLevel;
  bool createTable = true;
  bool createSingleTable = true;
  if (
    createTable == false &&
    format != nullptr &&
    global.runMode != GlobalVariables::RunMode::consoleRegular
  ) {
    createSingleTable = format->flagMakingExpressionTableWithLatex;
    format->flagMakingExpressionTableWithLatex = false;
  }
  if (!createSingleTable && !createTable && this->size() > 2) {
    out << "(";
  }
  if (createSingleTable) {
    out << "<table class='tableCalculatorOutput'>";
  }
  std::string currentOutput;
  if (outputJS != nullptr) {
    (*outputJS)["input"].elementType = JSData::Type::tokenArray;
    (*outputJS)["output"].elementType = JSData::Type::tokenArray;
  }
  for (int i = 1; i < this->size(); i ++) {
    const Expression expression = (*this)[i];
    this->toMathMLEndStatementOneRowTopLevel(
      out, &startingExpression, outputJS, i, *format
    );
    if (format != nullptr) {
      format->flagExpressionIsTopLevel = isFinal;
    }
  }
  if (createSingleTable) {
    out << "</table>";
  }
  if (!createSingleTable && !createTable && this->size() > 2) {
    out << ")";
  }
  return true;
}

bool Expression::toMathMLEndStatementOneRowTopLevel(
  std::stringstream& out,
  Expression* startingExpression,
  JSData* outputJS,
  int index,
  FormatExpressions& format
) const {
  STACK_TRACE("Expression::toMathMLEndStatementOneRowTopLevel");
  std::string currentInput, currentOutput;
  const Expression currentE = (*this)[index];
  out << "<tr><td class='cellCalculatorInput'>";
  if (!this->owner->flagHideLHS) {
    if (index < (*startingExpression).size()) {
      format.flagDontCollalpseProductsByUnits = true;
      currentInput =
      HtmlRoutines::getMathNoDisplay((*startingExpression)[index].toString(
          &format
        )
      );
    } else {
      currentInput =
      "No matching starting expression - "
      "possible use of the Melt keyword.";
    }
  } else {
    currentInput = "...";
  }
  out << currentInput;
  if (outputJS != nullptr) {
    (*outputJS)["input"][index - 1] = currentInput;
  }
  if (index != this->size() - 1) {
    out << ";";
  }
  out << "</td><td class='cellCalculatorResult'>";
  if (currentE.isOfType<std::string>()) {
    currentOutput =
    StringRoutines::Conversions::stringToCalculatorDisplay(
      currentE.getValue<std::string>()
    );
  } else if (currentE.requiresNoMathTags()) {
    format.flagDontCollalpseProductsByUnits = false;
    currentOutput = currentE.toString(&format);
  } else {
    format.flagDontCollalpseProductsByUnits = false;
    std::string childString = currentE.toString(&format);
    if (StringRoutines::stringContains(childString, "\\(")) {
      // The string contains the math tag \(. We assume the childString
      // has embedded descriptive latex strings. We should not generate math
      // tags; the expression is either too complicated to be formatted
      // correctly,
      // or the childString has already got all it's tags.
      currentOutput = childString;
    } else {
      currentOutput = HtmlRoutines::getMathNoDisplay(childString);
    }
  }
  currentOutput += currentE.toStringAllSlidersInExpression();
  if (outputJS != nullptr) {
    (*outputJS)["output"][index - 1] = currentOutput;
  }
  out << currentOutput;
  out << "</td></tr>";
  return true;
}

bool Expression::toMathMLEndStatementNested(
  std::stringstream& out, FormatExpressions* format
) const {
  STACK_TRACE("Expression::toMathMLEndStatementNested");
  out << "<mtable>";
  for (int i = 1; i < this->size(); i ++) {
    out << "<mtr><mtd>";
    (*this)[i].toMathML(format, nullptr, nullptr, false, nullptr);
    out << "</mtd></mtr>";
  }
  out << "</mtable>";
  return true;
}

bool Expression::toMathMLData(
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) const {
  STACK_TRACE("Expression::toMathMLData");
  if (this->owner == nullptr) {
    out << "<ms>(non-initialized)</ms>";
    return true;
  }
  if (this->isAtom()) {
    if (this->isOperationGiven(this->owner->opDifferential())) {
      out << "<mtext>d</mtext>";
    } else if (this->isOperationGiven(this->owner->opPhantom())) {
      out << "";
    } else if (
      this->owner->flagUseLnInsteadOfLog &&
      this->isOperationGiven(this->owner->opLog())
    ) {
      out << "<ln/>";
    } else if (
      this->data < this->owner->getOperations().size && this->data >= 0
    ) {
      std::string latex = this->owner->getOperations()[this->data];
      out << MathML::latexCommandToMathMLEquivalent(latex);
    } else {
      out << "[unknown atom of value " << this->data << "]";
    }
    return true;
  }
  if (this->isMatrixOfType<RationalFraction<Rational> >()) {
    FormatExpressions currentFormat;
    this->getContext().getFormat(currentFormat);
    currentFormat.flagUseHTML = false;
    currentFormat.flagUseLatex = true;
    Matrix<RationalFraction<Rational> > matrix;
    CalculatorConversions::functionGetMatrix(
      *this->owner, *this, matrix, false
    );
    out << matrix.toMathML(&currentFormat);
    return true;
  }
  int typeIndex = - 1;
  if (!this->isBuiltInType(&typeIndex)) {
    return false;
  }
  Calculator& commands = *this->owner;
  // The following handlers are initialized by
  // calling Calculator::addBuiltInType
  // in Calculator::initialize.
  if (!commands.toMathMLDataHandlers.contains(typeIndex)) {
    return false;
  }
  // handler must hold the function pointer:
  // Expression::toStringBuiltIn<builtInType>,
  // where builtInType is one of the types registered in
  // Calculator::initializeToStringHandlers.
  Expression::ToStringHandler handler =
  commands.toMathMLDataHandlers.getValueNoFail(typeIndex);
  return handler(*this, out, format, outputProperties);
}

bool Expression::toMathMLGeneral(
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) const {
  if (this->size() < 2) {
    return false;
  }
  (*this)[0].toMathML(out, format, outputProperties, nullptr, false, nullptr);
  bool needParenthesis = true;
  if (this->size() == 2) {
    if ((*this)[0].isAtomWhoseExponentsAreInterpretedAsFunction()) {
      needParenthesis = !(*this)[1].isAtom();
    }
    if ((*this)[0].isPowerOfAtomWhoseExponentsAreInterpretedAsFunction()) {
      needParenthesis = !(*this)[1].isAtom();
    }
  }
  if (needParenthesis) {
    out << MathML::leftParenthesis;
  }
  for (int i = 1; i < this->children.size; i ++) {
    (*this)[i].toMathML(
      out, format, outputProperties, nullptr, false, nullptr
    );
    if (i != this->children.size - 1) {
      out << "<mo>,</mo>";
    }
  }
  if (needParenthesis) {
    out << MathML::rightParenthesis;
  }
  return true;
}

bool Expression::toMathMLSequence(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  if (!input.isListStartingWithAtom(input.owner->opSequence())) {
    return false;
  }
  (void) format;
  out << "<mrow>" << MathML::leftParenthesis;
  for (int i = 1; i < input.size(); i ++) {
    std::string currentChildString =
    input[i].toMathML(format, outputProperties);
    out << currentChildString;
    if (i != input.children.size - 1) {
      out << "<mo>,</mo>";
    }
  }
  out << MathML::rightParenthesis;
  if (input.size() == 2) {
    out << "}";
  }
  return true;
}

bool Expression::toMathMLPower(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  Calculator& commands = *input.owner;
  if (!input.startsWith(commands.opPower(), 3)) {
    return false;
  }
  bool involvesExponentsInterpretedAsFunctions = false;
  const Expression& firstE = input[1];
  const Expression& secondE = input[2];
  bool isSuperScriptOfUnderscoredOperator = false;
  if (firstE.startsWith(commands.opUnderscore(), 3)) {
    if (firstE[1].isOperationGiven(commands.opIntegral())) {
      isSuperScriptOfUnderscoredOperator = true;
    }
  }
  if (isSuperScriptOfUnderscoredOperator) {
    out << "<msubsup>";
    firstE[1].toMathML(out, format, outputProperties);
    firstE[2].toMathML(out, format, outputProperties);
    secondE.toMathML(out, format, outputProperties);
    return true;
  }
  if (firstE.startsWith(- 1, 2)) {
    bool shouldProceed =
    firstE[0].isAtomWhoseExponentsAreInterpretedAsFunction() &&
    !secondE.isEqualToMOne() &&
    secondE.isRational();
    if (
      shouldProceed &&
      firstE[0].isOperationGiven(commands.opLog()) &&
      commands.flagUseLnAbsInsteadOfLogForIntegrationNotation
    ) {
      shouldProceed = false;
    }
    if (shouldProceed) {
      involvesExponentsInterpretedAsFunctions = true;
      Expression newFunE;
      newFunE.makeXOX(*input.owner, commands.opPower(), firstE[0], input[2]);
      newFunE.checkConsistency();
      out << "<mrow>";
      newFunE.toMathML(out, format, outputProperties);
      if (
        firstE[1].needsParenthesisForMultiplicationWhenSittingOnTheRightMost()
        ||
        firstE[1].startsWith(commands.opTimes())
      ) {
        out << MathML::leftParenthesis;
        firstE[1].toMathML(out, format, outputProperties);
        out << MathML::rightParenthesis;
      } else {
        firstE[1].toMathML(out, format, outputProperties);
      }
    }
  }
  if (!involvesExponentsInterpretedAsFunctions) {
    bool isSqrt = false;
    if (input[2].isOfType<Rational>()) {
      if (input[2].getValue<Rational>().isEqualTo(Rational(1, 2))) {
        isSqrt = true;
      }
    }
    if (isSqrt) {
      out << "<msqrt>";
      input[1].toMathML(out, format, outputProperties);
      out << "</msqrt>";
    } else {
      out << "<msup>";
      bool needsParentheses = input[1].needsParenthesisForBaseOfExponent();
      if (needsParentheses) {
        out << MathML::leftParenthesis;
      }
      input[1].toMathML(out, format, outputProperties);
      if (needsParentheses) {
        out << MathML::rightParenthesis;
      }
      input[2].toMathML(out, format, outputProperties);
      out << "</msup>";
    }
  }
  return true;
}

bool Expression::toMathMLUnderscore(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opUnderscore())) {
    return false;
  }
  if (!input[1].startsWith(input.owner->opPower())) {
    out << "<msub>";
    input[1].toMathML(out, format, nullptr);
    input[2].toMathML(out, format, nullptr);
    out << "</msub>";
    return true;
  }
  // We have an expression along the lines of {x^2}_1.
  // In latex, this would render with the subscript
  // being to the right of the superscript.
  // We shall render that as the latex of x^2_1 which
  // looks identical as the latex of x_1^2 and has
  // the super- and sub- scripts with the same indentation levels.
  out << "<msubsup>";
  // The base of the exponent:
  input[1][1].toMathML(out, format, nullptr);
  // The subscript:
  input[2].toMathML(out, format, nullptr);
  // The power of the exponent:
  input[1][2].toMathML(out, format, nullptr);
  out << "</msubsup>";
  return true;
}

bool Expression::toMathMLTimes(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opTimes(), 3)) {
    return false;
  }
  MathExpressionProperties secondExpressionProperties;
  MathExpressionProperties firstExpressionProperties;
  std::string secondExpressionMathML =
  input[2].toMathML(format, &secondExpressionProperties);
  if (input[1].isOperationGiven(input.owner->opSqrt())) {
    // A malformed expression such as: "\sqrt 3" will be parsed as "sqrt * 3"
    // and later corrected to "\sqrt{3}".
    out << "<msqrt>" << secondExpressionMathML << "</msqrt>";
    return true;
  }
  std::string firstExpression =
  input[1].toMathML(format, &firstExpressionProperties);
  bool firstNeedsBrackets = input[1].needsParenthesisForMultiplication();
  bool secondNeedsBrackets =
  input[2].needsParenthesisForMultiplicationWhenSittingOnTheRightMost(
    &(input[1])
  );
  if (secondExpressionProperties.startsWithMinus) {
    secondNeedsBrackets = true;
  }
  bool mustHaveTimes = false;
  bool collapseUnits = true;
  if (format != nullptr) {
    collapseUnits = !format->flagDontCollalpseProductsByUnits;
  }
  if (collapseUnits) {
    if (firstExpressionProperties.isNegativeOne) {
      firstExpression = MathML::negativeSign;
      firstNeedsBrackets = false;
    }
    if (firstExpressionProperties.isOne) {
      firstExpression = "";
    }
  }
  bool needsRow = !firstExpressionProperties.isOne &&
  !secondExpressionProperties.isOne;
  if (needsRow) {
    out << "<mrow>";
  }
  if (firstNeedsBrackets) {
    out
    << MathML::leftParenthesis
    << firstExpression
    << MathML::rightParenthesis;
  } else {
    out << firstExpression;
  }
  if (
    !firstNeedsBrackets &&
    !secondNeedsBrackets &&
    firstExpression != "" &&
    firstExpressionProperties.isNegativeOne
  ) {
    if (secondExpressionProperties.startsWithDigit) {
      mustHaveTimes = true;
    }
    if (firstExpressionProperties.endsWithDigit) {
      if (secondExpressionProperties.startsWithFraction) {
        mustHaveTimes = true;
      }
    }
  }
  if (mustHaveTimes) {
    out << "<mo>&sdot;</mo>";
  }
  if (secondNeedsBrackets) {
    out
    << MathML::leftParenthesis
    << secondExpressionMathML
    << MathML::rightParenthesis;
  } else {
    out << secondExpressionMathML;
  }
  if (needsRow) {
    out << "</mrow>";
  }
  return true;
}

bool Expression::toMathMLPlus(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.startsWith(input.owner->opPlus())) {
    return false;
  }
  if (input.size() < 3) {
    global.fatal
    << "Plus operation takes at least 2 arguments, "
    << "whereas this expression has "
    << input.children.size - 1
    << " arguments. "
    << global.fatal;
  }
  input.checkInitialization();
  List<Expression> summands;
  input.owner->collectOpands(input, input.owner->opPlus(), summands);
  out << "<mrow>";
  for (int i = 0; i < summands.size; i ++) {
    const Expression& summand = summands[i];
    bool needsParenthesesForAddition = summand.needsParenthesisForAddition();
    MathExpressionProperties properties;
    std::string summandString = summand.toMathML(format, &properties);
    if (i > 0) {
      if (needsParenthesesForAddition || !properties.startsWithMinus) {
        out << "<mo>+</mo>";
      }
    }
    if (needsParenthesesForAddition) {
      out << MathML::leftParenthesis;
    }
    out << summandString;
    if (needsParenthesesForAddition) {
      out << MathML::rightParenthesis;
    }
  }
  out << "</mrow>";
  return true;
}

bool Expression::toMathMLMatrix(
  const Expression& input,
  std::stringstream& out,
  FormatExpressions* format,
  MathExpressionProperties* outputProperties
) {
  (void) outputProperties;
  if (!input.isMatrix()) {
    return false;
  }
  out << MathML::leftParenthesis;
  out << "<mtable>";
  for (int i = 1; i < input.size(); i ++) {
    out << "<mtr>";
    for (int j = 1; j < input[i].size(); j ++) {
      out << "<mtd>" << input[i][j].toMathML(format, nullptr) << "</mtd>";
    }
    out << "</mtr>";
  }
  out << "</mtable>";
  out << MathML::rightParenthesis;
  return true;
}
