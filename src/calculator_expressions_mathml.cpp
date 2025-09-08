#include "calculator_interface.h"

template < >
bool Expression::toMathMLBuiltIn<Rational>(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  out << input.getValue<Rational>().toMathML(format, nullptr);
  return true;
}

void Expression::toMathML(std::stringstream& out, FormatExpressions* format)
const {
  STACK_TRACE("Expression::toMathML");
  if (this->owner != nullptr) {
    if (this->owner->recursionDepth + 1 > this->owner->maximumRecursionDepth) {
      out << "<ms>(...)</ms>";
      return;
    }
  } else {
    out << "<ms>(Error:NoOwner)</ms>";
    return;
  }
  RecursionDepthCounter recursionCounter;
  recursionCounter.initialize(&this->owner->recursionDepth);
  this->checkConsistency();
  int notationIndex =
  owner->objectContainer.expressionWithNotation.getIndex(*this);
  if (notationIndex != - 1) {
    out << owner->objectContainer.expressionNotation[notationIndex];
    return;
  }
  if (this->toMathMLData(out)) {} else if (this->toMathMLWithAtomHandler(out)) {}
 else if (this->toMathMLWithCompositeHandler(out)) {} else if (
    this->toStringEndStatement(out, nullptr, nullptr, nullptr)
  ) {} else if (this->size() == 1) {
    (*this)[0].toMathML(out, format);
  } else if (this->toMathMLGeneral(out, format)) {} else {
    out << "<ms>(ProgrammingError:NotDocumented)</ms>";
  }
}
bool Expression::toMathMLData(std::stringstream& out) const {
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
    FormatExpressions format;
    this->getContext().getFormat(format);
    format.flagUseHTML = false;
    format.flagUseLatex = true;
    Matrix<RationalFraction<Rational> > matrix;
    CalculatorConversions::functionGetMatrix(
        *this->owner, *this, matrix, false
        );
    out << matrix.toMathML(&format);
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
  return handler(*this, out, nullptr);
}

bool Expression::toMathMLGeneral(std::stringstream& out, FormatExpressions* format) const {
  if (this->size() < 2) {
    return false;
  }
   (*this)[0].toMathML(out, format);
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
    out << (*this)[i].toMathML();
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
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.isListStartingWithAtom(input.owner->opSequence())) {
    return false;
  }
  (void) format;
  out << "<mrow>" << MathML::leftParenthesis;
  for (int i = 1; i < input.size(); i ++) {
    std::string currentChildString = input[i].toMathML();
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
  const Expression& input, std::stringstream& out, FormatExpressions* format
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
    firstE[1].toMathML(out, format);
    firstE[2].toMathML(out, format);
    secondE.toMathML(out, format);
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
      newFunE.toMathML(out, format);
      if (
        firstE[1].needsParenthesisForMultiplicationWhenSittingOnTheRightMost()
        ||
        firstE[1].startsWith(commands.opTimes())
      ) {
        out << MathML::leftParenthesis;
        firstE[1].toMathML(out, format);
        out << MathML::rightParenthesis;
      } else {
        firstE[1].toMathML(out, format);
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
      input[1].toMathML(out, format);
      out << "</msqrt>";
    } else {
      out << "<msup>";
      bool needsParentheses = input[1].needsParenthesisForBaseOfExponent();
      if (needsParentheses) {
        out << MathML::leftParenthesis;
      }
      input[1].toMathML(out, format);
      if (needsParentheses) {
        out << MathML::rightParenthesis;
      }
      input[2].toMathML(out, format);
      out << "</msup>";
    }
  }
  return true;
}

bool Expression::toMathMLUnderscore(
  const Expression& input, std::stringstream& out, FormatExpressions* format
) {
  if (!input.startsWith(input.owner->opUnderscore())) {
    return false;
  }
  if (!input[1].startsWith(input.owner->opPower())){
    out << "<msub>";
    input[1].toMathML(out, format);
    input[2].toMathML(out, format);
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
   input[1][1].toMathML(out, format);
  // The subscript:
  input[2].toMathML(out, format);
   // The power of the exponent:
  input[1][2].toMathML(out, format);
      out << "</msubsup>";
  return true;
}
