// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_inner_functions.h"

bool CalculatorFunctionsTrigonometry::sin(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::sin");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argumentExpression = input[1];
  if (argumentExpression.isOperationGiven(calculator.opPi())) {
    return output.assignValue(calculator, 0);
  }
  if (argumentExpression.isEqualToZero()) {
    return output.assignValue(calculator, 0);
  }
  Rational piProportion;
  if (argumentExpression.startsWith(calculator.opTimes(), 3)) {
    if (argumentExpression[2].isOperationGiven(calculator.opPi())) {
      if (argumentExpression[1].isOfType<Rational>(&piProportion)) {
        AlgebraicNumber algebraicOutput;
        Rational ratOutput;
        if (
          algebraicOutput.assignSinRationalTimesPi(
            piProportion, calculator.objectContainer.algebraicClosure
          )
        ) {
          if (algebraicOutput.isRational(&ratOutput)) {
            return output.assignValue(calculator, ratOutput);
          }
          return output.assignValue(calculator, algebraicOutput);
        }
      }
    }
  }
  if (calculator.approximationsBanned()) {
    return false;
  }
  double argumentDouble = 0;
  if (!argumentExpression.evaluatesToDouble(&argumentDouble)) {
    return false;
  }
  return
  output.assignValue(
    calculator, FloatingPoint::sinFloating(argumentDouble)
  );
}

bool CalculatorFunctionsTrigonometry::cos(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::cos");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (argument.isOperationGiven(calculator.opPi())) {
    return output.assignValue(calculator, - 1);
  }
  if (argument.isEqualToZero()) {
    return output.assignValue(calculator, 1);
  }
  Rational piProportion;
  if (argument.startsWith(calculator.opTimes(), 3)) {
    if (argument[2].isOperationGiven(calculator.opPi())) {
      if (argument[1].isOfType<Rational>(&piProportion)) {
        AlgebraicNumber algebraicOutput;
        Rational ratOutput;
        if (
          algebraicOutput.assignCosRationalTimesPi(
            piProportion, calculator.objectContainer.algebraicClosure
          )
        ) {
          if (algebraicOutput.isRational(&ratOutput)) {
            return output.assignValue(calculator, ratOutput);
          }
          return output.assignValue(calculator, algebraicOutput);
        }
      }
    }
  }
  if (calculator.approximationsBanned()) {
    return false;
  }
  double argumentDouble = 0;
  if (!argument.evaluatesToDouble(&argumentDouble)) {
    return false;
  }
  return
  output.assignValue(
    calculator, FloatingPoint::cosFloating(argumentDouble)
  );
}

bool CalculatorFunctionsTrigonometry::tan(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::tan");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression num, den;
  num.makeOX(calculator, calculator.opSin(), argument);
  den.makeOX(calculator, calculator.opCos(), argument);
  return output.makeXOX(calculator, calculator.opDivide(), num, den);
}

bool CalculatorFunctionsTrigonometry::cotangent(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::cotangent");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression num, den;
  num.makeOX(calculator, calculator.opCos(), argument);
  den.makeOX(calculator, calculator.opSin(), argument);
  return output.makeXOX(calculator, calculator.opDivide(), num, den);
}

bool CalculatorFunctionsTrigonometry::sec(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::sec");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression num, den;
  num.assignValue(calculator, 1);
  den.makeOX(calculator, calculator.opCos(), argument);
  return output.makeXOX(calculator, calculator.opDivide(), num, den);
}

bool CalculatorFunctionsTrigonometry::csc(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::csc");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression numerator, denominator;
  numerator.assignValue(calculator, 1);
  denominator.makeOX(calculator, calculator.opSin(), argument);
  return
  output.makeXOX(
    calculator, calculator.opDivide(), numerator, denominator
  );
}

bool CalculatorFunctionsTrigonometry::eulerFormulaAsLaw(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::eulerFormulaAsLaw");
  if (!input.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  if (!input[1].isOperationGiven(calculator.opE())) {
    return false;
  }
  Expression coefficientOfI, currentE;
  Expression iE;
  iE.makeAtom(calculator, calculator.opImaginaryUnit());
  currentE.reset(calculator, 3);
  currentE.addChildAtomOnTop(calculator.opCoefficientOf());
  currentE.addChildOnTop(iE);
  currentE.addChildOnTop(input[2]);
  if (
    !CalculatorFunctions::coefficientOf(calculator, currentE, coefficientOfI)
  ) {
    return false;
  }
  if (coefficientOfI.isEqualToZero()) {
    return false;
  }
  Expression cosE, sinE;
  cosE.makeOX(calculator, calculator.opCos(), coefficientOfI);
  sinE.makeOX(calculator, calculator.opSin(), coefficientOfI);
  output = cosE + iE * sinE;
  return true;
}

bool CalculatorFunctionsTrigonometry::exploitCosineEvenness(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::exploitCosineEvenness");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression coefficientExpression, nonCoefficientPart;
  argument.getCoefficientMultiplicandForm(
    coefficientExpression, nonCoefficientPart
  );
  Rational rational;
  if (!coefficientExpression.isRational(&rational)) {
    return false;
  }
  if (rational >= 0) {
    return false;
  }
  Expression negativeOne;
  negativeOne.assignValue(calculator, - 1);
  return
  output.makeOX(
    calculator,
    calculator.opCos(),
    negativeOne * coefficientExpression * nonCoefficientPart
  );
}

bool CalculatorFunctionsTrigonometry::exploitSineOddness(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::exploitSineOddness");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression coefficientExpression, nonCFpart;
  argument.getCoefficientMultiplicandForm(coefficientExpression, nonCFpart);
  Rational rational;
  if (!coefficientExpression.isRational(&rational)) {
    return false;
  }
  if (rational >= 0) {
    return false;
  }
  Expression moneE, sinE;
  moneE.assignValue(calculator, - 1);
  sinE.makeOX(
    calculator,
    calculator.opSin(),
    moneE * coefficientExpression * nonCFpart
  );
  output = moneE * sinE;
  return true;
}

bool CalculatorFunctionsTrigonometry::convertSineToExponent(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::convertSineToExponent");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression eE, iE, exponentArgument, minusExponentArgument, leftE, rightE;
  eE.makeAtom(calculator, calculator.opE());
  iE.makeAtom(calculator, calculator.opImaginaryUnit());
  exponentArgument = iE * argument;
  minusExponentArgument = exponentArgument *(- 1);
  leftE.makeXOX(calculator, calculator.opPower(), eE, exponentArgument);
  rightE.makeXOX(
    calculator, calculator.opPower(), eE, minusExponentArgument
  );
  output = (iE *(- 1)) *(leftE - rightE) / 2;
  return true;
}

bool CalculatorFunctionsTrigonometry::convertCosineToExponent(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::convertCosineToExponent");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression eE, iE, exponentArgument, minusExponentArgument, leftE, rightE;
  eE.makeAtom(calculator, calculator.opE());
  iE.makeAtom(calculator, calculator.opImaginaryUnit());
  exponentArgument = iE * argument;
  minusExponentArgument = exponentArgument *(- 1);
  leftE.makeXOX(calculator, calculator.opPower(), eE, exponentArgument);
  rightE.makeXOX(
    calculator, calculator.opPower(), eE, minusExponentArgument
  );
  output = (leftE + rightE) / 2;
  return true;
}

bool CalculatorFunctionsTrigonometry::arccosAlgebraic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::arccosAlgebraic");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argumentE = input[1];
  Rational rational;
  if (argumentE.isRational(&rational)) {
    if (rational == 1) {
      return output.assignValue(calculator, 0);
    }
    if (rational == 0) {
      output.makeAtom(calculator, calculator.opPi());
      output /= 2;
      return true;
    }
    if (rational == - 1) {
      return output.makeAtom(calculator, calculator.opPi());
    }
    if (rational == Rational(1, 2)) {
      output.makeAtom(calculator, calculator.opPi());
      output /= 3;
      return true;
    }
    if (rational == Rational(- 1, 2)) {
      output.makeAtom(calculator, calculator.opPi());
      output *= 2;
      output /= 3;
      return true;
    }
  }
  AlgebraicNumber argument, candidate;
  if (argumentE.isOfType<AlgebraicNumber>(&argument)) {
    candidate.assignRationalQuadraticRadical(
      Rational(1, 2),
      calculator.objectContainer.algebraicClosure,
      nullptr
    );
    if (candidate == argument) {
      output.makeAtom(calculator, calculator.opPi());
      output /= 4;
      return true;
    }
    candidate *= - 1;
    if (candidate == argument) {
      output.makeAtom(calculator, calculator.opPi());
      output /= 4;
      output *= 3;
      return true;
    }
    candidate.assignRationalQuadraticRadical(
      Rational(3, 4),
      calculator.objectContainer.algebraicClosure,
      nullptr
    );
    if (candidate == argument) {
      output.makeAtom(calculator, calculator.opPi());
      output /= 6;
      return true;
    }
    candidate *= - 1;
    if (candidate == argument) {
      output.makeAtom(calculator, calculator.opPi());
      output /= 6;
      output *= 5;
      return true;
    }
  }
  return false;
}

bool CalculatorFunctionsTrigonometry::arcsinAlgebraic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::arcsinAlgebraic");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argumentE = input[1];
  Rational rational;
  if (argumentE.isRational(&rational)) {
    if (rational == 1) {
      output.makeAtom(calculator, calculator.opPi());
      output /= 2;
      return true;
    }
    if (rational == 0) {
      return output.assignValue(calculator, 0);
    }
    if (rational == - 1) {
      output.makeAtom(calculator, calculator.opPi());
      output /= - 2;
      return true;
    }
    if (rational == Rational(1, 2)) {
      output.makeAtom(calculator, calculator.opPi());
      output /= 6;
      return true;
    }
    if (rational == Rational(- 1, 2)) {
      output.makeAtom(calculator, calculator.opPi());
      output /= - 6;
      return true;
    }
  }
  AlgebraicNumber argument, candidate;
  if (argumentE.isOfType<AlgebraicNumber>(&argument)) {
    candidate.assignRationalQuadraticRadical(
      Rational(1, 2),
      calculator.objectContainer.algebraicClosure,
      nullptr
    );
    if (candidate == argument) {
      output.makeAtom(calculator, calculator.opPi());
      output /= 4;
      return true;
    }
    candidate *= - 1;
    if (candidate == argument) {
      output.makeAtom(calculator, calculator.opPi());
      output /= - 4;
      return true;
    }
    candidate.assignRationalQuadraticRadical(
      Rational(3, 4),
      calculator.objectContainer.algebraicClosure,
      nullptr
    );
    if (candidate == argument) {
      output.makeAtom(calculator, calculator.opPi());
      output /= 3;
      return true;
    }
    candidate *= - 1;
    if (candidate == argument) {
      output.makeAtom(calculator, calculator.opPi());
      output /= - 3;
      return true;
    }
  }
  return false;
}

bool CalculatorFunctionsTrigonometry::arctan(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::arctan");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (argument.isEqualToOne()) {
    output.makeAtom(calculator, calculator.opPi());
    output /= calculator.expressionFour();
    return true;
  }
  if (argument.isEqualToMOne()) {
    output.makeAtom(calculator, calculator.opPi());
    output /= calculator.expressionFour();
    output *= calculator.expressionMinusOne();
    return true;
  }
  if (calculator.approximationsBanned()) {
    return false;
  }
  double argumentDouble;
  if (!argument.evaluatesToDouble(&argumentDouble)) {
    return false;
  }
  return
  output.assignValue(calculator, FloatingPoint::arctan(argumentDouble));
}

bool CalculatorFunctionsTrigonometry::arccos(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::arccos");
  if (calculator.approximationsBanned()) {
    return false;
  }
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  double doubleArgument;
  if (!argument.evaluatesToDouble(&doubleArgument)) {
    return false;
  }
  return
  output.assignValue(calculator, FloatingPoint::arccos(doubleArgument));
}

bool CalculatorFunctionsTrigonometry::arcsin(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::arcsin");
  if (input.size() != 2) {
    return false;
  }
  if (calculator.approximationsBanned()) {
    return false;
  }
  double argument;
  if (!input[1].evaluatesToDouble(&argument)) {
    return false;
  }
  return output.assignValue(calculator, FloatingPoint::arcsin(argument));
}

bool CalculatorFunctionsTrigonometry::sineOfAngleSumToTrigonometry(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::sineOfAngleSumToTrigonometry");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (!argument.startsWith(calculator.opPlus(), 3)) {
    return false;
  }
  Expression sinA, sinB, cosA, cosB;
  sinA.makeOX(calculator, calculator.opSin(), argument[1]);
  sinB.makeOX(calculator, calculator.opSin(), argument[2]);
  cosA.makeOX(calculator, calculator.opCos(), argument[1]);
  cosB.makeOX(calculator, calculator.opCos(), argument[2]);
  output = sinA * cosB + cosA * sinB;
  return true;
}

bool CalculatorFunctionsTrigonometry::trigonometricSumToTrigonometricProduct(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsTrigonometry::trigonometricSumToTrigonometricProduct"
  );
  if (!input.startsWith(calculator.opPlus(), 3)) {
    return false;
  }
  Expression leftE = input[1], rightE = input[2];
  bool isGood = false;
  int leftSign = 1;
  int rightSign = 1;
  if (leftE.startsWith(calculator.opTimes(), 3)) {
    if (leftE[1].isEqualToMOne()) {
      leftE = leftE[2];
      leftSign = - 1;
    }
  }
  if (rightE.startsWith(calculator.opTimes(), 3)) {
    if (rightE[1].isEqualToMOne()) {
      rightE = rightE[2];
      rightSign = - 1;
    }
  }
  if (
    leftE.startsWith(calculator.opSin(), 2) &&
    rightE.startsWith(calculator.opSin(), 2)
  ) {
    isGood = true;
  } else if (
    leftE.startsWith(calculator.opCos(), 2) &&
    rightE.startsWith(calculator.opCos(), 2)
  ) {
    isGood = true;
  }
  if (!isGood) {
    return false;
  }
  Expression argSum, argDiff, leftMultiplicand, rightMultiplicand;
  if (leftE.startsWith(calculator.opSin(), 2)) {
    argSum = (leftE[1] * leftSign + rightE[1] * rightSign) / 2;
    argDiff = (leftE[1] * leftSign - rightE[1] * rightSign) / 2;
    leftMultiplicand.makeOX(calculator, calculator.opCos(), argDiff);
    rightMultiplicand.makeOX(calculator, calculator.opSin(), argSum);
    output = leftMultiplicand * rightMultiplicand * 2;
    return true;
  } else {
    if (leftSign == rightSign) {
      argSum = (leftE[1] + rightE[1]) / 2;
      argDiff = (leftE[1] - rightE[1]) / 2;
      leftMultiplicand.makeOX(calculator, calculator.opCos(), argDiff);
      rightMultiplicand.makeOX(calculator, calculator.opCos(), argSum);
      output = leftMultiplicand * rightMultiplicand * 2;
      if (leftSign == - 1) {
        output *= - 1;
      }
      return true;
    } else {
      argSum = (leftE[1] + rightE[1]) / 2;
      argDiff = (leftE[1] * leftSign + rightE[1] * rightSign) / 2;
      leftMultiplicand.makeOX(calculator, calculator.opSin(), argDiff);
      rightMultiplicand.makeOX(calculator, calculator.opSin(), argSum);
      output = leftMultiplicand * rightMultiplicand * 2;
      return true;
    }
  }
  // return false;
}

bool CalculatorFunctionsTrigonometry::cosineOfAngleSumToTrigonometry(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsTrigonometry::cosineOfAngleSumToTrigonometry"
  );
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (!argument.startsWith(calculator.opPlus(), 3)) {
    return false;
  }
  Expression sinA, sinB, cosA, cosB;
  sinA.makeOX(calculator, calculator.opSin(), argument[1]);
  sinB.makeOX(calculator, calculator.opSin(), argument[2]);
  cosA.makeOX(calculator, calculator.opCos(), argument[1]);
  cosB.makeOX(calculator, calculator.opCos(), argument[2]);
  output = cosA * cosB - sinA * sinB;
  return true;
}

class TrigonometricReduction {
public:
  class TrigonometricFunction {
  public:
    LinearCombination<Expression, Rational> arguments;
    bool isSine;
    Polynomial<AlgebraicNumber> eulerForm;
    Expression eulerFormExpression;
    TrigonometricReduction* owner;
    TrigonometricFunction();
    bool extractFrom(
      Expression& trigonometricExpression,
      std::stringstream* commentsOnFailure,
      TrigonometricReduction& inputOwner
    );
    std::string toString() const;
    void computeEulerFormAnonymous();
    void computeEulerFormExpression();
  };

  static const int maximumArguments = 5;
  static const int maximumTrigonometricPower = 20;
  Expression input;
  Calculator* owner;
  WithContext<RationalFraction<AlgebraicNumber> > inputFraction;
  WithContext<RationalFraction<AlgebraicNumber> > inputFractionSubstituted;
  WithContext<RationalFraction<AlgebraicNumber> >
  inputFractionSubstitutedAlgebraic;
  Expression eulerForm;
  Expression eulerFormAlgebraic;
  RationalFraction<AlgebraicNumber> eulerFormAlgebraicReduced;
  Expression fourierFractionForm;
  ExpressionContext variables;
  MapList<Expression, TrigonometricFunction> arguments;
  MapList<Expression, HashedList<Rational> > trigonometricBaseMonomials;
  MapList<Expression, Rational> trigonometricBaseScales;
  FormatExpressions formatAlgebraic;
  FormatExpressions formatTrigonometric;
  LinearCombination<Expression, AlgebraicNumber> numeratorBaseTrigonometric;
  LinearCombination<Expression, AlgebraicNumber> denominatorBaseTrigonometric;
  Expression baseTrigonometricForm;
  bool eulerFormToTrigonometryFourierForm(
    Polynomial<AlgebraicNumber>& input,
    Polynomial<AlgebraicNumber>& outputShifted,
    MonomialPolynomial& outputMonomialShift,
    LinearCombination<Expression, AlgebraicNumber>& output,
    std::stringstream* commentsOnFailure
  );
  bool shiftPolynomial(
    Polynomial<AlgebraicNumber>& input,
    Polynomial<AlgebraicNumber>& outputShifted,
    MonomialPolynomial& outputMonomialShift,
    std::stringstream* commentsOnFailure
  );
  void initialize(Calculator& inputOwner, const Expression& incoming);
  bool reduce(std::stringstream* commentsOnFailure);
  bool extractSinesAndCosines(std::stringstream* commentsOnFailure);
  Rational computeScaleOneBaseMonomial(HashedList<Rational>& coefficients);
  void computeBaseScales();
  void prepareFormatting();
  void computeEulerFormArguments();
  bool computeEulerFormReduced(std::stringstream* commentsOnFailure);
  bool computeBaseTrigonometricForm(std::stringstream* commentsOnFailure);
  bool computeEulerFormExpression(std::stringstream* commentsOnFailure);
  void normalizeNumeratorAndDenominator(
    LinearCombination<Expression, AlgebraicNumber>& numerator,
    LinearCombination<Expression, AlgebraicNumber>& denominator
  );
  bool computeSineCosineForm(
    const MonomialPolynomial& monomial,
    Polynomial<AlgebraicNumber>& output,
    int numberOfVariables,
    std::stringstream* commentsOnFailure
  );
  bool getSineCosineFormVariableOfPower(
    int letterIndex,
    const Rational& power,
    int numberOfVariables,
    Polynomial<AlgebraicNumber>& output,
    std::stringstream* commentsOnFailure
  );
  bool computeTrigonometricForm(
    Polynomial<AlgebraicNumber>& input,
    LinearCombination<Expression, AlgebraicNumber>& output,
    std::stringstream* commentsOnFailure
  );
  bool trigonometricFormFromSineCosineMonomial(
    const MonomialPolynomial& input,
    int numberOfVariables,
    Expression& output
  );
  bool sinePowerNAlgebraicForm(
    int power,
    int letterIndex,
    Polynomial<AlgebraicNumber>& output,
    std::stringstream* commentsOnFailure
  );
  void sineAlgebraicForm(
    int letterIndex, Polynomial<AlgebraicNumber>& output
  );
  void cosineAlgebraicForm(
    int letterIndex, Polynomial<AlgebraicNumber>& output
  );
  bool cosinePowerNAlgebraicForm(
    int power,
    int letterIndex,
    Polynomial<AlgebraicNumber>& output,
    std::stringstream* commentsOnFailure
  );
  void computeEulerFormExpressionPresentation();
  std::string toString();
  std::string toStringTrigonometry();
  std::string toStringTrigonometricArguments();
  AlgebraicClosureRationals* algebraicClosure();
  Expression argumentTermIndex(int index);
};

bool CalculatorFunctionsTrigonometry::fourierFractionForm(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::fourierFractionForm");
  if (input.size() != 2) {
    return false;
  }
  TrigonometricReduction trigonometricReduction;
  trigonometricReduction.initialize(calculator, input[1]);
  trigonometricReduction.reduce(&calculator.comments);
  return output.assignValue(calculator, trigonometricReduction.toString());
}

std::string TrigonometricReduction::toString() {
  std::stringstream out;
  out << "<b>Starting expression.</b><br>";
  out
  << "\\("
  << this->inputFraction.content.toString(&this->formatTrigonometric)
  << "\\)";
  out << "<br>";
  out << this->toStringTrigonometry();
  out << this->toStringTrigonometricArguments();
  out
  << "<br>Euler form:"
  << "<br>\\("
  << this->eulerForm.toString()
  << "\\)"
  << "<br>Euler form algebraic: "
  << "<br>\\("
  << this->eulerFormAlgebraic.toString()
  << "\\)<br>reduced:"
  << "<br>\\("
  << this->eulerFormAlgebraicReduced.toString(&this->formatAlgebraic)
  << "\\)";
  out
  << "<br><b>Fourier fraction form:</b>"
  << "<br>\\("
  << this->fourierFractionForm.toString()
  << "\\)<br>";
  out
  << "<br>Base trigonometric form:<br>"
  << "\\("
  << this->baseTrigonometricForm.toString()
  << "\\)";
  return out.str();
}

std::string TrigonometricReduction::toStringTrigonometricArguments() {
  std::stringstream out;
  out << "<b>Base arguments</b>: ";
  out << this->trigonometricBaseMonomials.toStringHtml();
  return out.str();
}

std::string TrigonometricReduction::toStringTrigonometry() {
  std::stringstream out;
  out << "\\(\\begin{array}{clclcl}\n";
  for (int i = 0; i < this->arguments.size(); i ++) {
    const TrigonometricReduction::TrigonometricFunction& trigonometricFunction
    =
    this->arguments.values[i];
    out
    << this->arguments.keys[i].toString()
    << "&\\to& "
    << trigonometricFunction.toString();
    out
    << "&=&"
    << trigonometricFunction.eulerFormExpression.toString()
    << "&=&"
    << trigonometricFunction.eulerForm.toString(&this->formatAlgebraic);
    out << "\\\\\n";
  }
  out << "\\end{array}\\)";
  out << "<br>";
  return out.str();
}

void TrigonometricReduction::initialize(
  Calculator& inputOwner, const Expression& incoming
) {
  this->owner = &inputOwner;
  this->input = incoming;
}

bool TrigonometricReduction::reduce(std::stringstream* commentsOnFailure) {
  STACK_TRACE("TrigonometricReduction::reduce");
  HashedList<std::string> variables;
  std::string reductionRules;
  if (
    !CalculatorConversions::functionRationalFraction(
      *this->owner, this->input, this->inputFraction, false
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to extract rational fraction.";
    }
    return false;
  }
  if (!this->extractSinesAndCosines(commentsOnFailure)) {
    return false;
  }
  this->computeBaseScales();
  this->prepareFormatting();
  this->computeEulerFormArguments();
  if (this->trigonometricBaseMonomials.size() > this->maximumArguments) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Too many base trig arguments: "
      << this->trigonometricBaseMonomials.size()
      << ", maximum: "
      << this->maximumArguments
      << ".";
    }
    return true;
  }
  if (!this->computeEulerFormReduced(commentsOnFailure)) {
    return false;
  }
  if (!this->computeEulerFormExpression(commentsOnFailure)) {
    return false;
  }
  if (!this->computeBaseTrigonometricForm(commentsOnFailure)) {
    return false;
  }
  this->computeEulerFormExpressionPresentation();
  reductionRules = "";
  return false;
}

bool TrigonometricReduction::extractSinesAndCosines(
  std::stringstream* commentsOnFailure
) {
  for (Expression& input : this->inputFraction.context.getVariables()) {
    TrigonometricReduction::TrigonometricFunction trigonometricFunction;
    if (
      !trigonometricFunction.extractFrom(input, commentsOnFailure, *this)
    ) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Failed to extract sine/cosine.";
      }
      return false;
    }
    this->arguments[input] = trigonometricFunction;
    for (int i = 0; i < trigonometricFunction.arguments.size(); i ++) {
      HashedList<Rational>& currentCoefficients =
      this->trigonometricBaseMonomials.getValueCreateEmpty(
        trigonometricFunction.arguments.monomials[i]
      );
      currentCoefficients.addOnTopNoRepetition(
        trigonometricFunction.arguments.coefficients[i]
      );
    }
  }
  return true;
}

void TrigonometricReduction::computeBaseScales() {
  for (int i = 0; i < this->trigonometricBaseMonomials.size(); i ++) {
    Rational scale =
    this->computeScaleOneBaseMonomial(
      this->trigonometricBaseMonomials.values[i]
    );
    this->trigonometricBaseScales.setKeyValue(
      this->trigonometricBaseMonomials.keys[i], scale
    );
  }
}

Rational TrigonometricReduction::computeScaleOneBaseMonomial(
  HashedList<Rational>& coefficients
) {
  LargeIntegerUnsigned denominator = 1;
  for (int i = 0; i < coefficients.size; i ++) {
    denominator =
    MathRoutines::leastCommonMultiple(
      denominator, coefficients[i].getDenominator()
    );
  }
  Rational result = 1;
  result /= denominator;
  return result;
}

void TrigonometricReduction::prepareFormatting() {
  for (int i = 0; i < this->trigonometricBaseMonomials.size(); i ++) {
    Expression newAtom = this->owner->getNewAtom();
    this->variables.addVariable(newAtom);
    this->formatAlgebraic.polynomialAlphabet.addOnTop(newAtom.toString());
  }
  this->formatAlgebraic.flagUseFrac = true;
  for (int i = 0; i < this->arguments.size(); i ++) {
    this->formatTrigonometric.polynomialAlphabet.addOnTop(
      this->arguments.keys[i].toString()
    );
  }
  this->formatTrigonometric.flagUseFrac = true;
}

void TrigonometricReduction::computeEulerFormArguments() {
  for (
    TrigonometricReduction::TrigonometricFunction& current :
    this->arguments.values
  ) {
    current.computeEulerFormAnonymous();
    current.computeEulerFormExpression();
  }
}

bool TrigonometricReduction::computeEulerFormReduced(
  std::stringstream* commentsOnFailure
) {
  PolynomialSubstitution<AlgebraicNumber> substitution;
  for (Expression& expression : this->arguments.keys) {
    substitution.addOnTop(
      this->arguments.getValueNoFail(expression).eulerForm
    );
  }
  this->eulerFormAlgebraicReduced = this->inputFraction.content;
  if (
    !this->eulerFormAlgebraicReduced.substitution(
      substitution,
      this->owner->objectContainer.algebraicClosure.one(),
      commentsOnFailure
    )
  ) {
    return false;
  }
  return true;
}

void TrigonometricReduction::normalizeNumeratorAndDenominator(
  LinearCombination<Expression, AlgebraicNumber>& numerator,
  LinearCombination<Expression, AlgebraicNumber>& denominator
) {
  AlgebraicNumber numeratorScale =
  numerator.scaleNormalizeLeadingMonomial(nullptr);
  AlgebraicNumber denominatorScale =
  denominator.scaleNormalizeLeadingMonomial(nullptr);
  AlgebraicNumber quotient = denominatorScale / numeratorScale;
  numerator *= quotient;
  if (!numerator.isEqualToZero()) {
    if (!numerator.getLeadingCoefficient(nullptr).isRational()) {
      numerator /=
      this->owner->objectContainer.algebraicClosure.imaginaryUnit();
      denominator /=
      this->owner->objectContainer.algebraicClosure.imaginaryUnit();
    }
  }
}

bool TrigonometricReduction::computeBaseTrigonometricForm(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("TrigonometricReduction::computeBaseTrigonometricForm");
  Polynomial<AlgebraicNumber> numerator, denominator;
  this->eulerFormAlgebraicReduced.getNumerator(numerator);
  this->eulerFormAlgebraicReduced.getDenominator(denominator);
  MonomialPolynomial unused;
  if (
    !this->shiftPolynomial(numerator, numerator, unused, commentsOnFailure)
  ) {
    return false;
  }
  if (
    !this->shiftPolynomial(
      denominator, denominator, unused, commentsOnFailure
    )
  ) {
    return false;
  }
  if (
    !this->computeTrigonometricForm(
      numerator, this->numeratorBaseTrigonometric, commentsOnFailure
    )
  ) {
    return false;
  }
  if (
    !this->computeTrigonometricForm(
      denominator, this->denominatorBaseTrigonometric, commentsOnFailure
    )
  ) {
    return false;
  }
  this->normalizeNumeratorAndDenominator(
    this->numeratorBaseTrigonometric, this->denominatorBaseTrigonometric
  );
  Expression numeratorExpression;
  numeratorExpression.makeSum(
    *this->owner, this->numeratorBaseTrigonometric
  );
  if (denominator.isEqualToOne()) {
    this->baseTrigonometricForm = numeratorExpression;
    return true;
  }
  Expression denominatorExpression;
  denominatorExpression.makeSum(
    *this->owner, this->denominatorBaseTrigonometric
  );
  this->baseTrigonometricForm.makeXOX(
    *this->owner,
    this->owner->opDivide(),
    numeratorExpression,
    denominatorExpression
  );
  return true;
}

AlgebraicClosureRationals* TrigonometricReduction::algebraicClosure() {
  return &this->owner->objectContainer.algebraicClosure;
}

bool TrigonometricReduction::sinePowerNAlgebraicForm(
  int power,
  int letterIndex,
  Polynomial<AlgebraicNumber>& output,
  std::stringstream* commentsOnFailure
) {
  if (power > TrigonometricReduction::maximumTrigonometricPower) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Sine power is too large.";
    }
    return false;
  }
  this->sineAlgebraicForm(letterIndex, output);
  output.raiseToPower(power, this->algebraicClosure()->one());
  return true;
}

bool TrigonometricReduction::cosinePowerNAlgebraicForm(
  int power,
  int letterIndex,
  Polynomial<AlgebraicNumber>& output,
  std::stringstream* commentsOnFailure
) {
  if (power > TrigonometricReduction::maximumTrigonometricPower) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Sine power is too large.";
    }
    return false;
  }
  this->cosineAlgebraicForm(letterIndex, output);
  output.raiseToPower(power, this->algebraicClosure()->one());
  return true;
}

void TrigonometricReduction::sineAlgebraicForm(
  int letterIndex, Polynomial<AlgebraicNumber>& output
) {
  AlgebraicNumber coefficient;
  coefficient.assignRational(Rational(1, 2), this->algebraicClosure());
  coefficient /= this->algebraicClosure()->imaginaryUnit();
  MonomialPolynomial monomial;
  monomial.makeEi(letterIndex, 1);
  output.addMonomial(monomial, coefficient);
  coefficient *= - 1;
  monomial.makeEi(letterIndex, - 1);
  output.addMonomial(monomial, coefficient);
}

void TrigonometricReduction::cosineAlgebraicForm(
  int letterIndex, Polynomial<AlgebraicNumber>& output
) {
  AlgebraicNumber coefficient;
  coefficient.assignRational(Rational(1, 2), this->algebraicClosure());
  MonomialPolynomial monomial;
  monomial.makeEi(letterIndex, 1);
  output.addMonomial(monomial, coefficient);
  monomial.makeEi(letterIndex, - 1);
  output.addMonomial(monomial, coefficient);
}

bool TrigonometricReduction::computeTrigonometricForm(
  Polynomial<AlgebraicNumber>& input,
  LinearCombination<Expression, AlgebraicNumber>& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("TrigonometricReduction::computeTrigonometricForm");
  Polynomial<AlgebraicNumber> sineCosineForm, current;
  int numberOfVariables = input.minimalNumberOfVariables();
  for (int i = 0; i < input.size(); i ++) {
    if (
      !this->computeSineCosineForm(
        input.monomials[i],
        current,
        numberOfVariables,
        commentsOnFailure
      )
    ) {
      return false;
    }
    sineCosineForm.addOtherTimesConst(current, input.coefficients[i]);
  }
  output.makeZero();
  Expression currentExpression;
  for (int i = 0; i < sineCosineForm.size(); i ++) {
    this->trigonometricFormFromSineCosineMonomial(
      sineCosineForm.monomials[i], numberOfVariables, currentExpression
    );
    output.addMonomial(currentExpression, sineCosineForm.coefficients[i]);
  }
  return true;
}

Expression TrigonometricReduction::argumentTermIndex(int index) {
  STACK_TRACE("TrigonometricReduction::argumentTermIndex");
  Expression result = this->trigonometricBaseScales.keys[index];
  if (this->trigonometricBaseScales.values[index] != 1) {
    Expression coefficient;
    coefficient.assignValue(
      *this->owner, this->trigonometricBaseScales.values[index]
    );
    result = coefficient * result;
  }
  return result;
}

bool TrigonometricReduction::trigonometricFormFromSineCosineMonomial(
  const MonomialPolynomial& input,
  int numberOfVariables,
  Expression& output
) {
  List<Expression> multiplicands;
  for (int i = 0; i < input.minimalNumberOfVariables(); i ++) {
    if (input(i).isEqualToZero()) {
      continue;
    }
    int index = i;
    if (index >= numberOfVariables) {
      index -= numberOfVariables;
    }
    Expression argument = this->argumentTermIndex(index);
    Expression trigonometricFunction;
    if (i >= numberOfVariables) {
      trigonometricFunction.makeOX(
        *this->owner, this->owner->opSin(), argument
      );
    } else {
      trigonometricFunction.makeOX(
        *this->owner, this->owner->opCos(), argument
      );
    }
    if (!input(i).isEqualToOne()) {
      Expression exponent;
      exponent.assignValue(*this->owner, input(i));
      trigonometricFunction.makeXOX(
        *this->owner,
        this->owner->opPower(),
        trigonometricFunction,
        exponent
      );
    }
    multiplicands.addOnTop(trigonometricFunction);
  }
  return output.makeProduct(*this->owner, multiplicands);
}

bool TrigonometricReduction::computeSineCosineForm(
  const MonomialPolynomial& monomial,
  Polynomial<AlgebraicNumber>& output,
  int numberOfVariables,
  std::stringstream* commentsOnFailure
) {
  output.makeConstant(this->algebraicClosure()->one());
  Polynomial<AlgebraicNumber> multiplicand;
  for (int i = 0; i < monomial.minimalNumberOfVariables(); i ++) {
    if (
      !this->getSineCosineFormVariableOfPower(
        i,
        monomial(i),
        numberOfVariables,
        multiplicand,
        commentsOnFailure
      )
    ) {
      return false;
    }
    output *= multiplicand;
  }
  return true;
}

bool TrigonometricReduction::getSineCosineFormVariableOfPower(
  int letterIndex,
  const Rational& power,
  int numberOfVariables,
  Polynomial<AlgebraicNumber>& output,
  std::stringstream* commentsOnFailure
) {
  int powerInteger = 0;
  if (!power.isSmallInteger(&powerInteger)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Power: "
      << power.toString()
      << " must be a small integer";
    }
    return false;
  }
  output.makeZero();
  MonomialPolynomial cosineMonomial, sineMonomial;
  cosineMonomial.makeEi(letterIndex);
  sineMonomial.makeEi(numberOfVariables + letterIndex);
  output.addMonomial(cosineMonomial, this->algebraicClosure()->one());
  if (powerInteger > 0) {
    output.addMonomial(
      sineMonomial, this->algebraicClosure()->imaginaryUnit()
    );
  } else {
    output.subtractMonomial(
      sineMonomial, this->algebraicClosure()->imaginaryUnit()
    );
    powerInteger *= - 1;
  }
  output.raiseToPower(powerInteger, this->algebraicClosure()->one());
  return true;
}

bool TrigonometricReduction::computeEulerFormExpression(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("TrigonometricReduction::computeEulerFormExpression");
  MonomialPolynomial shift;
  Polynomial<AlgebraicNumber> converted;
  Polynomial<AlgebraicNumber> shifted;
  this->eulerFormAlgebraicReduced.getNumerator(converted);
  LinearCombination<Expression, AlgebraicNumber> numerator, denominator;
  if (
    !this->eulerFormToTrigonometryFourierForm(
      converted, shifted, shift, numerator, commentsOnFailure
    )
  ) {
    return false;
  }
  this->eulerFormAlgebraicReduced.getDenominator(converted);
  if (
    !this->eulerFormToTrigonometryFourierForm(
      converted, shifted, shift, denominator, commentsOnFailure
    )
  ) {
    return false;
  }
  this->normalizeNumeratorAndDenominator(numerator, denominator);
  Expression numeratorExpression, denominatorExpression;
  numeratorExpression.makeSum(*this->owner, numerator);
  denominatorExpression.makeSum(*this->owner, denominator);
  this->fourierFractionForm.makeXOX(
    *this->owner,
    this->owner->opDivide(),
    numeratorExpression,
    denominatorExpression
  );
  return true;
}

void TrigonometricReduction::computeEulerFormExpressionPresentation() {
  STACK_TRACE("TrigonometricReduction::computeEulerFormExpressionPresentation")
  ;
  this->inputFractionSubstituted = this->inputFraction;
  this->inputFractionSubstitutedAlgebraic = this->inputFraction;
  List<Expression> eulerFormVariables;
  List<Expression> eulerFormVariablesAlgebraicForm;
  for (
    TrigonometricReduction::TrigonometricFunction& trigonometricFunction :
    this->arguments.values
  ) {
    eulerFormVariables.addOnTop(trigonometricFunction.eulerFormExpression);
    Expression currentEulerFormAlgebraic;
    CalculatorConversions::expressionFromPolynomial(
      *this->owner,
      trigonometricFunction.eulerForm,
      currentEulerFormAlgebraic,
      &this->variables
    );
    eulerFormVariablesAlgebraicForm.addOnTop(currentEulerFormAlgebraic);
  }
  this->inputFractionSubstituted.context.setVariables(eulerFormVariables);
  this->inputFractionSubstitutedAlgebraic.context.setVariables(
    eulerFormVariablesAlgebraicForm
  );
  CalculatorConversions::expressionFromRationalFraction(
    *this->owner,
    this->inputFractionSubstituted.content,
    this->eulerForm,
    &this->inputFractionSubstituted.context
  );
  CalculatorConversions::expressionFromRationalFraction(
    *this->owner,
    this->inputFractionSubstitutedAlgebraic.content,
    this->eulerFormAlgebraic,
    &this->inputFractionSubstitutedAlgebraic.context
  );
}

bool TrigonometricReduction::shiftPolynomial(
  Polynomial<AlgebraicNumber>& input,
  Polynomial<AlgebraicNumber>& outputShifted,
  MonomialPolynomial& outputMonomialShift,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("TrigonometricReduction::shiftPolynomial");
  MonomialPolynomial maximumMonomial;
  MonomialPolynomial minimumMonomial;
  int numberOfVariables = input.minimalNumberOfVariables();
  for (int j = 0; j < numberOfVariables; j ++) {
    for (int i = 0; i < input.size(); i ++) {
      const MonomialPolynomial& current = input.monomials[i];
      if (i == 0) {
        maximumMonomial.setVariable(j, current(j));
        minimumMonomial.setVariable(j, current(j));
        continue;
      }
      Rational power =
      MathRoutines::maximum(maximumMonomial(j), current(j));
      maximumMonomial.setVariable(j, power);
      power = MathRoutines::minimum(minimumMonomial(j), current(j));
      minimumMonomial.setVariable(j, power);
    }
  }
  outputMonomialShift.makeOne();
  for (int i = 0; i < numberOfVariables; i ++) {
    Rational shift = (maximumMonomial(i) + minimumMonomial(i)) / 2;
    if (!shift.isInteger()) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Quotient between maximum and minimum "
        << "monomials must have only even powers. ";
      }
      return false;
    }
    outputMonomialShift.setVariable(i, - shift);
  }
  outputShifted = input;
  outputShifted *= outputMonomialShift;
  return true;
}

bool TrigonometricReduction::eulerFormToTrigonometryFourierForm(
  Polynomial<AlgebraicNumber>& input,
  Polynomial<AlgebraicNumber>& outputShifted,
  MonomialPolynomial& outputMonomialShift,
  LinearCombination<Expression, AlgebraicNumber>& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("TrigonometricReduction::eulerFormToTrigonometryFourierForm");
  if (
    !this->shiftPolynomial(
      input, outputShifted, outputMonomialShift, commentsOnFailure
    )
  ) {
    return false;
  }
  int numberOfVariables = input.minimalNumberOfVariables();
  int startingMonomialCount = outputShifted.size();
  output.makeZero();
  Polynomial<AlgebraicNumber> remainder = outputShifted;
  for (int i = 0; i < startingMonomialCount; i ++) {
    if (remainder.isEqualToZero()) {
      break;
    }
    MonomialPolynomial leading = remainder.getLeadingMonomial(nullptr);
    MonomialPolynomial opposite = leading;
    opposite.invert();
    LinearCombination<Expression, Rational> currentArgument;
    for (int j = 0; j < numberOfVariables; j ++) {
      Expression term = this->argumentTermIndex(j);
      currentArgument.addMonomial(term, leading(j));
    }
    Expression currentArgumentSummed;
    currentArgumentSummed.makeSum(*this->owner, currentArgument);
    Expression sine;
    sine.makeOX(
      *this->owner, this->owner->opSin(), currentArgumentSummed
    );
    Expression cosine;
    cosine.makeOX(
      *this->owner, this->owner->opCos(), currentArgumentSummed
    );
    AlgebraicNumber leadingCoefficient = remainder.getCoefficientOf(leading);
    AlgebraicNumber oppositeCoefficient = remainder.getCoefficientOf(opposite);
    if (leading.isConstant()) {
      Expression one;
      one.assignValue(*this->owner, 1);
      remainder.subtractMonomial(leading, leadingCoefficient);
      output.addMonomial(one, leadingCoefficient);
      continue;
    }
    AlgebraicNumber sineCoefficient = leadingCoefficient - oppositeCoefficient;
    sineCoefficient *=
    this->owner->objectContainer.algebraicClosure.imaginaryUnit();
    AlgebraicNumber cosineCoefficient =
    leadingCoefficient + oppositeCoefficient;
    output.addMonomial(cosine, cosineCoefficient);
    output.addMonomial(sine, sineCoefficient);
    remainder.subtractMonomial(leading, leadingCoefficient);
    remainder.subtractMonomial(opposite, oppositeCoefficient);
  }
  return true;
}

TrigonometricReduction::TrigonometricFunction::TrigonometricFunction() {
  this->isSine = false;
  this->owner = nullptr;
}

bool TrigonometricReduction::TrigonometricFunction::extractFrom(
  Expression& trigonometricExpression,
  std::stringstream* commentsOnFailure,
  TrigonometricReduction& inputOwner
) {
  this->owner = &inputOwner;
  std::string trigonometricFunction;
  if (
    trigonometricExpression.startsWithGivenOperation(
      Calculator::Atoms::Trigonometry::sine
    )
  ) {
    trigonometricFunction = Calculator::Atoms::Trigonometry::sine;
    this->isSine = true;
  }
  if (
    trigonometricExpression.startsWithGivenOperation(
      Calculator::Atoms::Trigonometry::cosine
    )
  ) {
    trigonometricFunction = Calculator::Atoms::Trigonometry::cosine;
    this->isSine = false;
  }
  if (trigonometricFunction == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to extract sine or cosine from: "
      << trigonometricExpression.toString();
    }
    return false;
  }
  this->owner->owner->functionCollectSummandsCombine(
    *this->owner->owner,
    trigonometricExpression[1],
    this->arguments
  );
  return true;
}

void TrigonometricReduction::TrigonometricFunction::computeEulerFormAnonymous()
{
  STACK_TRACE(
    "TrigonometricReduction::"
    "TrigonometricFunction::computeEulerFormAnonymous"
  );
  this->eulerForm.makeZero();
  Polynomial<AlgebraicNumber> plusSummand, minusSummand;
  plusSummand.makeConstant(
    this->owner->owner->objectContainer.algebraicClosure.one()
  );
  minusSummand.makeConstant(
    this->owner->owner->objectContainer.algebraicClosure.one()
  );
  for (int i = 0; i < this->arguments.size(); i ++) {
    const Expression& monomial = this->arguments.monomials[i];
    int variableIndex =
    this->owner->trigonometricBaseMonomials.getIndex(monomial);
    MonomialPolynomial anonymousMonomial;
    Rational scale = this->arguments.coefficients[i] /
    this->owner->trigonometricBaseScales.values[variableIndex];
    if (!scale.isInteger()) {
      global.fatal
      << "The base monomial scale is supposed to be an integer. "
      << global.fatal;
    }
    anonymousMonomial.setVariable(variableIndex, scale);
    plusSummand *= anonymousMonomial;
    anonymousMonomial.setVariable(variableIndex, - scale);
    minusSummand *= anonymousMonomial;
  }
  this->eulerForm = plusSummand;
  if (this->isSine) {
    this->eulerForm -= minusSummand;
    this->eulerForm /=
    this->owner->owner->objectContainer.algebraicClosure.imaginaryUnit();
  } else {
    this->eulerForm += minusSummand;
  }
  this->eulerForm /= 2;
}

void TrigonometricReduction::TrigonometricFunction::computeEulerFormExpression(
) {
  STACK_TRACE(
    "TrigonometricReduction::"
    "TrigonometricFunction::"
    "computeEulerFormExpression"
  );
  Expression sum;
  Calculator& calculator = *this->owner->owner;
  sum.makeSum(calculator, this->arguments);
  Expression plusExponent = calculator.expressionSquareRootNegativeOne() * sum;
  Expression minusExponent = calculator.expressionMinusOne() * plusExponent;
  Expression plusSummand;
  Expression e;
  e.makeAtom(calculator, calculator.opE());
  plusSummand.makeXOX(calculator, calculator.opPower(), e, plusExponent);
  Expression minusSummand;
  minusSummand.makeXOX(
    calculator, calculator.opPower(), e, minusExponent
  );
  if (this->isSine) {
    Expression twoI =
    calculator.expressionTwo() * calculator.expressionSquareRootNegativeOne();
    this->eulerFormExpression = (
      plusSummand + calculator.expressionMinusOne() * minusSummand
    ) /
    twoI;
  } else {
    this->eulerFormExpression = (plusSummand + minusSummand) /
    calculator.expressionTwo();
  }
}

std::string TrigonometricReduction::TrigonometricFunction::toString() const {
  std::stringstream out;
  if (this->isSine) {
    out << "sin(";
  } else {
    out << "cos(";
  }
  Expression sum;
  sum.makeSum(*this->owner->owner, this->arguments);
  out << sum.toString();
  out << ")";
  return out.str();
}
