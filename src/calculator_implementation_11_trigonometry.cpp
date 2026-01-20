#include "calculator_inner_functions.h"
#include "math_general.h"
#include "math_general_polynomial_computations_advanced_implementation.h" // IWYU pragma: keep: breaks g++ -02 optimization build.
#include "math_general_polynomial_computations_basic_implementation.h" // IWYU pragma: keep: breaks g++ -02 optimization build.
#include "math_rational_function.h"

bool CalculatorFunctionsTrigonometry::sinExact(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::sinExact");
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
  if (!argumentExpression.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (!argumentExpression[2].isOperationGiven(calculator.opPi())) {
    return false;
  }
  if (!argumentExpression[1].isOfType<Rational>(&piProportion)) {
    return false;
  }
  AlgebraicNumber algebraicOutput;
  if (
    !algebraicOutput.assignSinRationalTimesPi(
      piProportion, calculator.objectContainer.algebraicClosure
    )
  ) {
    return false;
  }
  Rational rational;
  if (algebraicOutput.isRational(&rational)) {
    return output.assignValue(calculator, rational);
  }
  return output.assignValue(calculator, algebraicOutput);
}

bool CalculatorFunctionsTrigonometry::sinApproximate(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (input.size() != 2) {
    return false;
  }
  const Expression& argumentExpression = input[1];
  double argumentDouble = 0;
  if (!argumentExpression.evaluatesToDouble(&argumentDouble)) {
    return false;
  }
  return
  output.assignValue(calculator, FloatingPoint::sinFloating(argumentDouble));
}

bool CalculatorFunctionsTrigonometry::cosExact(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::cosExact");
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
  if (!argument.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (!argument[2].isOperationGiven(calculator.opPi())) {
    return false;
  }
  if (!argument[1].isOfType<Rational>(&piProportion)) {
    return false;
  }
  AlgebraicNumber algebraicOutput;
  if (
    !algebraicOutput.assignCosRationalTimesPi(
      piProportion, calculator.objectContainer.algebraicClosure
    )
  ) {
    return false;
  }
  Rational rational;
  if (algebraicOutput.isRational(&rational)) {
    return output.assignValue(calculator, rational);
  }
  return output.assignValue(calculator, algebraicOutput);
}

bool CalculatorFunctionsTrigonometry::cosApproximate(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  double argumentDouble = 0;
  if (!argument.evaluatesToDouble(&argumentDouble)) {
    return false;
  }
  return
  output.assignValue(calculator, FloatingPoint::cosFloating(argumentDouble));
}

bool CalculatorFunctionsTrigonometry::tan(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::tan");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression numerator;
  Expression denominator;
  numerator.makeOX(calculator, calculator.opSin(), argument);
  denominator.makeOX(calculator, calculator.opCos(), argument);
  return
  output.makeXOX(calculator, calculator.opDivide(), numerator, denominator);
}

bool CalculatorFunctionsTrigonometry::cotangent(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::cotangent");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression numerator;
  Expression denominator;
  numerator.makeOX(calculator, calculator.opCos(), argument);
  denominator.makeOX(calculator, calculator.opSin(), argument);
  return
  output.makeXOX(calculator, calculator.opDivide(), numerator, denominator);
}

bool CalculatorFunctionsTrigonometry::sec(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::sec");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression numerator;
  Expression denominator;
  numerator.assignValue(calculator, 1);
  denominator.makeOX(calculator, calculator.opCos(), argument);
  return
  output.makeXOX(calculator, calculator.opDivide(), numerator, denominator);
}

bool CalculatorFunctionsTrigonometry::csc(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::csc");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression numerator;
  Expression denominator;
  numerator.assignValue(calculator, 1);
  denominator.makeOX(calculator, calculator.opSin(), argument);
  return
  output.makeXOX(calculator, calculator.opDivide(), numerator, denominator);
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
  Expression coefficientOfI;
  Expression currentE;
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
  Expression cosE;
  Expression sinE;
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
  Expression coefficientExpression;
  Expression nonCoefficientPart;
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
  Expression coefficientExpression;
  Expression nonCFpart;
  argument.getCoefficientMultiplicandForm(coefficientExpression, nonCFpart);
  Rational rational;
  if (!coefficientExpression.isRational(&rational)) {
    return false;
  }
  if (rational >= 0) {
    return false;
  }
  Expression moneE;
  Expression sinE;
  moneE.assignValue(calculator, - 1);
  sinE.makeOX(
    calculator, calculator.opSin(), moneE * coefficientExpression * nonCFpart
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
  Expression eE;
  Expression iE;
  Expression exponentArgument;
  Expression minusExponentArgument;
  Expression leftE;
  Expression rightE;
  eE.makeAtom(calculator, calculator.opE());
  iE.makeAtom(calculator, calculator.opImaginaryUnit());
  exponentArgument = iE * argument;
  minusExponentArgument = exponentArgument *(- 1);
  leftE.makeXOX(calculator, calculator.opPower(), eE, exponentArgument);
  rightE.makeXOX(calculator, calculator.opPower(), eE, minusExponentArgument);
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
  Expression eE;
  Expression iE;
  Expression exponentArgument;
  Expression minusExponentArgument;
  Expression leftE;
  Expression rightE;
  eE.makeAtom(calculator, calculator.opE());
  iE.makeAtom(calculator, calculator.opImaginaryUnit());
  exponentArgument = iE * argument;
  minusExponentArgument = exponentArgument *(- 1);
  leftE.makeXOX(calculator, calculator.opPower(), eE, exponentArgument);
  rightE.makeXOX(calculator, calculator.opPower(), eE, minusExponentArgument);
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
  const Expression& argumentExpression = input[1];
  Rational rational;
  if (argumentExpression.isRational(&rational)) {
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
  AlgebraicNumber argument;
  AlgebraicNumber candidate;
  if (argumentExpression.isOfType<AlgebraicNumber>(&argument)) {
    candidate.assignRationalQuadraticRadical(
      Rational(1, 2), calculator.objectContainer.algebraicClosure, nullptr
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
      Rational(3, 4), calculator.objectContainer.algebraicClosure, nullptr
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
  const Expression& argumentExpression = input[1];
  Rational rational;
  if (argumentExpression.isRational(&rational)) {
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
  AlgebraicNumber argument;
  AlgebraicNumber candidate;
  if (argumentExpression.isOfType<AlgebraicNumber>(&argument)) {
    candidate.assignRationalQuadraticRadical(
      Rational(1, 2), calculator.objectContainer.algebraicClosure, nullptr
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
      Rational(3, 4), calculator.objectContainer.algebraicClosure, nullptr
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

bool CalculatorFunctionsTrigonometry::arctanExact(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::arctanExact");
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
  return false;
}

bool CalculatorFunctionsTrigonometry::arctanApproximate(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  double argumentDouble;
  if (!argument.evaluatesToDouble(&argumentDouble)) {
    return false;
  }
  return output.assignValue(calculator, FloatingPoint::arctan(argumentDouble));
}

bool CalculatorFunctionsTrigonometry::arctan2(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::arctan2");
  if (input.size() != 3) {
    return false;
  }
  const Expression& y = input[1];
  const Expression& x = input[2];
  double xDouble = 0;
  double yDouble = 0;
  if (!x.evaluatesToDouble(&xDouble) || !y.evaluatesToDouble(&yDouble)) {
    return false;
  }
  if (xDouble == 0 && yDouble == 0) {
    return false;
  }
  return
  output.assignValue(calculator, FloatingPoint::arctan2(yDouble, xDouble));
}

bool CalculatorFunctionsTrigonometry::arccosApproximate(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::arccosApproximate");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  double doubleArgument;
  if (!argument.evaluatesToDouble(&doubleArgument)) {
    return false;
  }
  return output.assignValue(calculator, FloatingPoint::arccos(doubleArgument));
}

bool CalculatorFunctionsTrigonometry::arcsinApproximate(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::arcsin");
  if (input.size() != 2) {
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
  STACK_TRACE(
    "CalculatorFunctionsTrigonometry::sineOfAngleSumToTrigonometry"
  );
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (!argument.startsWith(calculator.opPlus(), 3)) {
    return false;
  }
  Expression sinA;
  Expression sinB;
  Expression cosA;
  Expression cosB;
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
  Expression leftExpression = input[1];
  Expression rightExpression = input[2];
  bool isGood = false;
  int leftSign = 1;
  int rightSign = 1;
  if (leftExpression.startsWith(calculator.opTimes(), 3)) {
    if (leftExpression[1].isEqualToMOne()) {
      leftExpression = leftExpression[2];
      leftSign = - 1;
    }
  }
  if (rightExpression.startsWith(calculator.opTimes(), 3)) {
    if (rightExpression[1].isEqualToMOne()) {
      rightExpression = rightExpression[2];
      rightSign = - 1;
    }
  }
  if (
    leftExpression.startsWith(calculator.opSin(), 2) &&
    rightExpression.startsWith(calculator.opSin(), 2)
  ) {
    isGood = true;
  } else if (
    leftExpression.startsWith(calculator.opCos(), 2) &&
    rightExpression.startsWith(calculator.opCos(), 2)
  ) {
    isGood = true;
  }
  if (!isGood) {
    return false;
  }
  Expression argSum;
  Expression argDiff;
  Expression leftMultiplicand;
  Expression rightMultiplicand;
  if (leftExpression.startsWith(calculator.opSin(), 2)) {
    argSum = (leftExpression[1] * leftSign + rightExpression[1] * rightSign) /
    2;
    argDiff = (leftExpression[1] * leftSign - rightExpression[1] * rightSign) /
    2;
    leftMultiplicand.makeOX(calculator, calculator.opCos(), argDiff);
    rightMultiplicand.makeOX(calculator, calculator.opSin(), argSum);
    output = leftMultiplicand * rightMultiplicand * 2;
    return true;
  } else {
    if (leftSign == rightSign) {
      argSum = (leftExpression[1] + rightExpression[1]) / 2;
      argDiff = (leftExpression[1] - rightExpression[1]) / 2;
      leftMultiplicand.makeOX(calculator, calculator.opCos(), argDiff);
      rightMultiplicand.makeOX(calculator, calculator.opCos(), argSum);
      output = leftMultiplicand * rightMultiplicand * 2;
      if (leftSign == - 1) {
        output *= - 1;
      }
      return true;
    } else {
      argSum = (leftExpression[1] + rightExpression[1]) / 2;
      argDiff = (leftExpression[1] * leftSign + rightExpression[1] * rightSign)
      /
      2;
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
  Expression sinA;
  Expression sinB;
  Expression cosA;
  Expression cosB;
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
    void computeEulerFormAnonymousVariables();
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
  MonomialPolynomial startingScaleEulerForm;
  Polynomial<AlgebraicNumber> finalScaleEulerForm;
  // See the computation of finalScaleEulerForm to see how this is used.
  Polynomial<AlgebraicNumber> finalScaleEulerFormCosineFactorOrOne;
  MonomialPolynomial finalScaleEulerFormMonomialFactor;
  Vector<AlgebraicNumber> errorCheckInputs;
  AlgebraicNumber errorCheck;
  // The following polynomials need
  // argumentScalesFourier
  // to be converted to trigonometric form
  Polynomial<AlgebraicNumber> numeratorStarting;
  Polynomial<AlgebraicNumber> denominatorStarting;
  Polynomial<AlgebraicNumber> numeratorStartingNormalized;
  Polynomial<AlgebraicNumber> denominatorStartingNormalized;
  Polynomial<AlgebraicNumber> numeratorDividedByGCD;
  Polynomial<AlgebraicNumber> denominatorDividedByGCD;
  Polynomial<AlgebraicNumber> numeratorFinal;
  Polynomial<AlgebraicNumber> denominatorFinal;
  Polynomial<AlgebraicNumber> greatestCommonDivisor;
  Expression fourierFractionForm;
  ExpressionContext variables;
  MapList<Expression, TrigonometricFunction> arguments;
  MapList<Expression, HashedList<Rational> >
  startingTrigonometricBaseMonomials;
  MapList<Expression, Rational> argumentScales;
  //  MapList<Expression, Rational> argumentScalesBaseTrigonometric;
  FormatExpressions formatAlgebraic;
  FormatExpressions formatTrigonometric;
  LinearCombination<Expression, AlgebraicNumber> numeratorBaseTrigonometric;
  LinearCombination<Expression, AlgebraicNumber> denominatorBaseTrigonometric;
  Expression baseTrigonometricForm;
  bool success = false;
  std::string errorReason;
  bool eulerFormToFourierForm(
    const Polynomial<AlgebraicNumber>& input,
    LinearCombination<Expression, AlgebraicNumber>& output
  );
  void initialize(Calculator& inputOwner, const Expression& incoming);
  bool reduce(std::stringstream* commentsOnFailure);
  void reduceOrFail();
  bool extractSinesAndCosines(std::stringstream* commentsOnFailure);
  Rational computeScaleOneBaseMonomial(HashedList<Rational>& coefficients);
  void computeBaseScales();
  void prepareFormatting();
  void computeErrorCheck(
    const Polynomial<AlgebraicNumber>& numerator,
    const Polynomial<AlgebraicNumber>& denominator,
    bool firstRun
  );
  void computeFinalScaleEulerForm();
  void computeEulerFormArguments();
  int numberOfVariables();
  bool computeEulerFormReduced(std::stringstream* commentsOnFailure);
  bool computeBaseTrigonometricForm(std::stringstream* commentsOnFailure);
  bool computeFourierFractionForm();
  void normalizeNumeratorAndDenominator(
    LinearCombination<Expression, AlgebraicNumber>& numerator,
    LinearCombination<Expression, AlgebraicNumber>& denominator,
    Calculator& calculator
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
  bool computeBaseTrigonometricFormOfPolynomial(
    const Polynomial<AlgebraicNumber>& input,
    LinearCombination<Expression, AlgebraicNumber>& output,
    std::stringstream* commentsOnFailure
  );
  bool baseTrigonometricFormFromSineCosineMonomial(
    const MonomialPolynomial& input, int numberOfVariables, Expression& output
  );
  bool sinePowerNAlgebraicForm(
    int power,
    int letterIndex,
    Polynomial<AlgebraicNumber>& output,
    std::stringstream* commentsOnFailure
  );
  void sineAlgebraicForm(int letterIndex, Polynomial<AlgebraicNumber>& output);
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
  Expression argumentTermIndex(
    int index, const MapList<Expression, Rational>& baseScales
  );
};

bool CalculatorFunctionsTrigonometry::trigonometricReductionVerbose(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsTrigonometry::trigonometricReductionVerbose"
  );
  if (input.size() != 2) {
    return false;
  }
  TrigonometricReduction trigonometricReduction;
  trigonometricReduction.initialize(calculator, input[1]);
  trigonometricReduction.reduceOrFail();
  return output.assignValue(calculator, trigonometricReduction.toString());
}

bool CalculatorFunctionsTrigonometry::fourierFractionForm(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::fourierFractionForm");
  if (input.size() != 2) {
    return false;
  }
  TrigonometricReduction trigonometricReduction;
  trigonometricReduction.initialize(calculator, input[1]);
  trigonometricReduction.reduceOrFail();
  if (!trigonometricReduction.success) {
    return output.assignValue(calculator, trigonometricReduction.toString());
  }
  output = trigonometricReduction.fourierFractionForm;
  return true;
}

bool CalculatorFunctionsTrigonometry::baseTrigonometricForm(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsTrigonometry::baseTrigonometricForm");
  if (input.size() != 2) {
    return false;
  }
  TrigonometricReduction trigonometricReduction;
  trigonometricReduction.initialize(calculator, input[1]);
  trigonometricReduction.reduceOrFail();
  if (!trigonometricReduction.success) {
    return output.assignValue(calculator, trigonometricReduction.toString());
  }
  output = trigonometricReduction.baseTrigonometricForm;
  return true;
}

std::string TrigonometricReduction::toString() {
  std::stringstream out;
  out << "<b>Starting expression.</b><br>";
  out
  << "\\("
  << this->inputFraction.content.toString(&this->formatTrigonometric)
  << "\\)";
  out << "<br>";
  if (!this->success) {
    out
    << "<b style='color:red'>Failed to reduce the fraction. </b> Details.<br>"
    << this->errorReason;
    return out.str();
  }
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
  << "\\)"
  << "<br>Starting fraction: "
  << "<br>"
  << "\\("
  << "\\frac{"
  << this->numeratorStarting.toString(&this->formatAlgebraic)
  << "}{"
  << this->denominatorStarting.toString(&this->formatAlgebraic)
  << "}\\)";
  if (!this->numeratorStarting.isEqualToZero()) {
    out
    << "<br>Normalized by multiplying top and bottom by:  "
    << "\\("
    << this->startingScaleEulerForm.toString(&this->formatAlgebraic)
    << "\\)"
    << "<br>"
    << "\\("
    << "\\frac{"
    << this->numeratorStartingNormalized.toString(&this->formatAlgebraic)
    << "}{"
    << this->denominatorStartingNormalized.toString(&this->formatAlgebraic)
    << "}\\)"
    << "<br>Greatest common divisor of numerator and denominator: "
    << "<br>"
    << "\\("
    << this->greatestCommonDivisor.toString(&this->formatAlgebraic)
    << "\\)"
    << "<br>Euler form with canceled GCD: "
    << "<br>\\("
    << "\\frac{"
    << this->numeratorDividedByGCD.toString(&this->formatAlgebraic)
    << "}{"
    << this->denominatorDividedByGCD.toString(&this->formatAlgebraic)
    << "}"
    << "\\)"
    << "<br>Multiply back by: "
    << "\\("
    << this->finalScaleEulerForm.toString(&formatAlgebraic)
    << "\\)";
    if (!this->finalScaleEulerFormCosineFactorOrOne.isEqualToOne()) {
      out
      << "\\( = \\left("
      << this->finalScaleEulerFormCosineFactorOrOne.toString(
        &this->formatAlgebraic
      )
      << "\\right) \\cdot \\left("
      << this->finalScaleEulerFormMonomialFactor.toString(
        &this->formatAlgebraic
      )
      << "\\right)\\)"
      << ", where the two-term factor was chosen to "
      << "make the distance between the leading and "
      << "least monomials to be even-powered";
    }
    out
    << "<br> to get the final form: "
    << "<br>\\("
    << "\\frac{"
    << this->numeratorFinal.toString(&this->formatAlgebraic)
    << "}{"
    << this->denominatorFinal.toString(&this->formatAlgebraic)
    << "}"
    << "\\)";
  }
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
  out << this->startingTrigonometricBaseMonomials.toStringHtml();
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
    std::string eulerFormString1 =
    trigonometricFunction.eulerFormExpression.toString(
      &this->formatAlgebraic
    );
    std::string eulerFormString2 =
    trigonometricFunction.eulerForm.toString(&this->formatAlgebraic);
    out << "&=&" << eulerFormString1 << "&=&" << eulerFormString2;
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

void TrigonometricReduction::reduceOrFail() {
  STACK_TRACE("TrigonometricReduction::reduceOrFail");
  std::stringstream commentsOnFailure;
  this->success = this->reduce(&commentsOnFailure);
  if (!this->success) {
    this->errorReason = commentsOnFailure.str();
  }
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
  if (
    this->startingTrigonometricBaseMonomials.size() > this->maximumArguments
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Too many base trig arguments: "
      << this->startingTrigonometricBaseMonomials.size()
      << ", maximum: "
      << this->maximumArguments
      << ".";
    }
    return true;
  }
  if (!this->computeEulerFormReduced(commentsOnFailure)) {
    return false;
  }
  if (!this->computeFourierFractionForm()) {
    return false;
  }
  this->computeEulerFormExpressionPresentation();
  if (!this->computeBaseTrigonometricForm(commentsOnFailure)) {
    return false;
  }
  reductionRules = "";
  return true;
}

bool TrigonometricReduction::extractSinesAndCosines(
  std::stringstream* commentsOnFailure
) {
  for (Expression& input : this->inputFraction.context.getVariables()) {
    TrigonometricReduction::TrigonometricFunction trigonometricFunction;
    if (!trigonometricFunction.extractFrom(input, commentsOnFailure, *this)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Failed to extract sine/cosine.";
      }
      return false;
    }
    this->arguments[input] = trigonometricFunction;
    for (int i = 0; i < trigonometricFunction.arguments.size(); i ++) {
      HashedList<Rational>& currentCoefficients =
      this->startingTrigonometricBaseMonomials.getValueCreateEmpty(
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
  for (int i = 0; i < this->startingTrigonometricBaseMonomials.size(); i ++) {
    Rational scale =
    this->computeScaleOneBaseMonomial(
      this->startingTrigonometricBaseMonomials.values[i]
    );
    this->argumentScales.setKeyValue(
      this->startingTrigonometricBaseMonomials.keys[i], scale
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
  for (int i = 0; i < this->startingTrigonometricBaseMonomials.size(); i ++) {
    Expression newAtom = this->owner->getNewAtom();
    this->variables.addVariable(newAtom);
    this->formatAlgebraic.polynomialAlphabet.addOnTop(newAtom.toString());
  }
  this->formatAlgebraic.flagUseFrac = true;
  this->formatAlgebraic.flagSuppressOneIn1overXtimesY = true;
  for (int i = 0; i < this->arguments.size(); i ++) {
    this->formatTrigonometric.polynomialAlphabet.addOnTop(
      this->arguments.keys[i].toString()
    );
  }
  this->formatTrigonometric.flagUseFrac = true;
}

int TrigonometricReduction::numberOfVariables() {
  return this->argumentScales.size();
}

void TrigonometricReduction::computeEulerFormArguments() {
  for (
    TrigonometricReduction::TrigonometricFunction& current :
    this->arguments.values
  ) {
    current.computeEulerFormAnonymousVariables();
    current.computeEulerFormExpression();
  }
}

void TrigonometricReduction::computeErrorCheck(
  const Polynomial<AlgebraicNumber>& numerator,
  const Polynomial<AlgebraicNumber>& denominator,
  bool firstRun
) {
  AlgebraicNumber zero = this->algebraicClosure()->zero();
  AlgebraicNumber numeratorValue;
  AlgebraicNumber denominatorValue;
  if (!firstRun) {
    if (this->errorCheckInputs.size == 0) {
      // Couldn't find good error check inputs.
      return;
    }
    denominatorValue = denominator.evaluate(this->errorCheckInputs, zero);
    numeratorValue = numerator.evaluate(this->errorCheckInputs, zero);
    if (denominatorValue.isEqualToZero()) {
      global.fatal << "Unexpected zero denominator. " << global.fatal;
    }
    AlgebraicNumber currentValue = numeratorValue;
    currentValue /= denominatorValue;
    if (currentValue != this->errorCheck) {
      global.fatal
      << "Bad error check: want value: "
      << this->errorCheck
      << ", got: "
      << currentValue.toString()
      << global.fatal;
    }
    return;
  }
  int numberOfVariables =
  MathRoutines::maximum(
    numerator.minimalNumberOfVariables(),
    denominator.minimalNumberOfVariables()
  );
  this->errorCheckInputs.setSize(numberOfVariables);
  for (int firstValue = 7; firstValue < 15; firstValue ++) {
    this->errorCheckInputs[0] = firstValue;
    for (int i = 1; i < numberOfVariables; i ++) {
      this->errorCheckInputs[i] =
      this->errorCheckInputs[i - 1] * this->errorCheckInputs[i - 1] - 3;
    }
    denominatorValue = denominator.evaluate(this->errorCheckInputs, zero);
    numeratorValue = numerator.evaluate(this->errorCheckInputs, zero);
    if (denominatorValue.isEqualToZero()) {
      continue;
    }
    this->errorCheck = numeratorValue;
    this->errorCheck /= denominatorValue;
    return;
  }
  // Couldn't find good error check inputs
  this->errorCheckInputs.clear();
}

void TrigonometricReduction::computeFinalScaleEulerForm() {
  STACK_TRACE("TrigonometricReduction::computeFinalScaleEulerForm");
  AlgebraicNumber one = this->owner->objectContainer.algebraicClosure.one();
  this->finalScaleEulerFormMonomialFactor =
  this->greatestCommonDivisor.getLeadingMonomial(nullptr);
  this->finalScaleEulerFormMonomialFactor.raiseToPower(Rational(1, 2));
  int totalVariables = this->numberOfVariables();
  this->finalScaleEulerFormCosineFactorOrOne = one;
  for (int i = 0; i < totalVariables; i ++) {
    if (this->finalScaleEulerFormMonomialFactor(i).isInteger()) {
      continue;
    }
    this->finalScaleEulerFormMonomialFactor.multiplyByVariable(
      i, Rational(- 1, 2)
    );
    MonomialPolynomial monomial;
    monomial.makeEi(i, 1);
    // The powers of the monomial scale are not integral.
    this->finalScaleEulerFormCosineFactorOrOne.multiplyBy(monomial);
  }
  this->finalScaleEulerFormMonomialFactor.divideBy(
    this->startingScaleEulerForm
  );
  if (!this->finalScaleEulerFormCosineFactorOrOne.isEqualToOne()) {
    this->finalScaleEulerFormCosineFactorOrOne += one;
  }
  this->finalScaleEulerForm = this->finalScaleEulerFormCosineFactorOrOne;
  this->finalScaleEulerForm.multiplyBy(
    this->finalScaleEulerFormMonomialFactor
  );
}

bool TrigonometricReduction::computeEulerFormReduced(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("TrigonometricReduction::computeEulerFormReduced");
  PolynomialSubstitution<AlgebraicNumber> substitution;
  for (Expression& expression : this->arguments.keys) {
    substitution.addOnTop(
      this->arguments.getValueNoFail(expression).eulerForm
    );
  }
  this->inputFraction.content.getNumerator(this->numeratorStarting);
  this->inputFraction.content.getDenominator(this->denominatorStarting);
  AlgebraicNumber one = this->owner->objectContainer.algebraicClosure.one();
  if (
    !this->numeratorStarting.substitute(substitution, one) ||
    !this->denominatorStarting.substitute(substitution, one)
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Unexpected failure to substitute in fraction. ";
    }
    return false;
  }
  if (this->denominatorStarting.isEqualToZero()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Got division by zero while simplifying your fraction. "
      << "Perhaps your denominator equals zero?";
    }
    return false;
  }
  if (numeratorStarting.isEqualToZero()) {
    return true;
  }
  this->numeratorStartingNormalized = this->numeratorStarting;
  this->denominatorStartingNormalized = this->denominatorStarting;
  RationalFraction<AlgebraicNumber>::normalizeNegativeExponentsInQuotient(
    this->numeratorStartingNormalized,
    this->denominatorStartingNormalized,
    this->startingScaleEulerForm
  );
  this->computeErrorCheck(
    this->numeratorStartingNormalized,
    this->denominatorStartingNormalized,
    true
  );
  Polynomial<AlgebraicNumber> remainderUnused;
  if (
    !Polynomial<AlgebraicNumber>::greatestCommonDivisor(
      this->numeratorStartingNormalized,
      this->denominatorStartingNormalized,
      this->greatestCommonDivisor,
      one,
      commentsOnFailure
    )
  ) {
    return false;
  }
  this->greatestCommonDivisor /=
  this->greatestCommonDivisor.getLeadingCoefficient(nullptr);
  Polynomial<AlgebraicNumber> mustBeZero;
  this->numeratorStartingNormalized.divideBy(
    this->greatestCommonDivisor,
    this->numeratorDividedByGCD,
    mustBeZero,
    nullptr
  );
  if (!mustBeZero.isEqualToZero()) {
    global.fatal
    << "Numerator divided by the gcd did not yield remainder zero. "
    << global.fatal;
  }
  this->denominatorStartingNormalized.divideBy(
    this->greatestCommonDivisor,
    this->denominatorDividedByGCD,
    mustBeZero,
    nullptr
  );
  if (!mustBeZero.isEqualToZero()) {
    global.fatal
    << "Numerator divided by the gcd did not yield remainder zero. "
    << global.fatal;
  }
  this->numeratorFinal = this->numeratorDividedByGCD;
  this->denominatorFinal = this->denominatorDividedByGCD;
  this->computeErrorCheck(
    this->numeratorDividedByGCD, this->denominatorDividedByGCD, false
  );
  this->computeFinalScaleEulerForm();
  this->numeratorFinal *= this->finalScaleEulerForm;
  this->denominatorFinal *= this->finalScaleEulerForm;
  this->computeErrorCheck(this->numeratorFinal, this->denominatorFinal, false);
  return true;
}

void TrigonometricReduction::normalizeNumeratorAndDenominator(
  LinearCombination<Expression, AlgebraicNumber>& numerator,
  LinearCombination<Expression, AlgebraicNumber>& denominator,
  Calculator& calculator
) {
  STACK_TRACE("TrigonometricReduction::normalizeNumeratorAndDenominator");
  if (denominator.isEqualToZero()) {
    return;
  }
  if (numerator.isEqualToZero()) {
    AlgebraicNumber one = calculator.objectContainer.algebraicClosure.one();
    denominator.makeZero();
    denominator.addMonomial(calculator.expressionOne(), one);
    return;
  }
  AlgebraicNumber startingLeadingCoefficient =
  numerator.getLeadingCoefficient(nullptr);
  if (!startingLeadingCoefficient.isRational()) {
    numerator /= startingLeadingCoefficient;
    denominator /= startingLeadingCoefficient;
  }
  AlgebraicNumber numeratorScale =
  numerator.scaleNormalizeLeadingMonomial(nullptr);
  AlgebraicNumber denominatorScale =
  denominator.scaleNormalizeLeadingMonomial(nullptr);
  AlgebraicNumber quotient = denominatorScale / numeratorScale;
  numerator *= quotient;
}

bool TrigonometricReduction::computeBaseTrigonometricForm(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("TrigonometricReduction::computeBaseTrigonometricForm");
  if (this->numeratorStarting.isEqualToZero()) {
    this->baseTrigonometricForm = this->owner->expressionZero();
    return true;
  }
  if (
    !this->computeBaseTrigonometricFormOfPolynomial(
      this->numeratorFinal, this->numeratorBaseTrigonometric, commentsOnFailure
    )
  ) {
    return false;
  }
  if (
    !this->computeBaseTrigonometricFormOfPolynomial(
      this->denominatorFinal,
      this->denominatorBaseTrigonometric,
      commentsOnFailure
    )
  ) {
    return false;
  }
  this->normalizeNumeratorAndDenominator(
    this->numeratorBaseTrigonometric,
    this->denominatorBaseTrigonometric,
    *this->owner
  );
  Expression numeratorExpression;
  numeratorExpression.makeSumFromLinearCombination(
    *this->owner, this->numeratorBaseTrigonometric
  );
  if (this->numeratorFinal.isEqualToOne()) {
    this->baseTrigonometricForm = numeratorExpression;
    return true;
  }
  Expression denominatorExpression;
  denominatorExpression.makeSumFromLinearCombination(
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

bool TrigonometricReduction::computeBaseTrigonometricFormOfPolynomial(
  const Polynomial<AlgebraicNumber>& input,
  LinearCombination<Expression, AlgebraicNumber>& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE(
    "TrigonometricReduction::computeBaseTrigonometricFormOfPolynomial"
  );
  Polynomial<AlgebraicNumber> sineCosineForm;
  Polynomial<AlgebraicNumber> current;
  int numberOfVariables = input.minimalNumberOfVariables();
  for (int i = 0; i < input.size(); i ++) {
    if (
      !this->computeSineCosineForm(
        input.monomials[i], current, numberOfVariables, commentsOnFailure
      )
    ) {
      return false;
    }
    sineCosineForm.addOtherTimesConst(current, input.coefficients[i]);
  }
  output.makeZero();
  Expression currentExpression;
  for (int i = 0; i < sineCosineForm.size(); i ++) {
    this->baseTrigonometricFormFromSineCosineMonomial(
      sineCosineForm.monomials[i], numberOfVariables, currentExpression
    );
    output.addMonomial(currentExpression, sineCosineForm.coefficients[i]);
  }
  return true;
}

Expression TrigonometricReduction::argumentTermIndex(
  int index, const MapList<Expression, Rational>& baseScales
) {
  STACK_TRACE("TrigonometricReduction::argumentTermIndex");
  Expression result = baseScales.keys[index];
  if (baseScales.values[index] != 1) {
    Expression coefficient;
    coefficient.assignValue(*this->owner, baseScales.values[index]);
    result = coefficient * result;
  }
  return result;
}

bool TrigonometricReduction::baseTrigonometricFormFromSineCosineMonomial(
  const MonomialPolynomial& input, int numberOfVariables, Expression& output
) {
  STACK_TRACE(
    "TrigonometricReduction::baseTrigonometricFormFromSineCosineMonomial"
  );
  List<Expression> multiplicands;
  for (int i = 0; i < input.minimalNumberOfVariables(); i ++) {
    if (input(i).isEqualToZero()) {
      continue;
    }
    int index = i;
    if (index >= numberOfVariables) {
      index -= numberOfVariables;
    }
    Expression argument = this->argumentTermIndex(index, this->argumentScales);
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
        *this->owner, this->owner->opPower(), trigonometricFunction, exponent
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
  STACK_TRACE("TrigonometricReduction::computeSineCosineForm");
  output.makeConstant(this->algebraicClosure()->one());
  Polynomial<AlgebraicNumber> multiplicand;
  for (int i = 0; i < monomial.minimalNumberOfVariables(); i ++) {
    if (
      !this->getSineCosineFormVariableOfPower(
        i, monomial(i), numberOfVariables, multiplicand, commentsOnFailure
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
  MonomialPolynomial cosineMonomial;
  MonomialPolynomial sineMonomial;
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

bool TrigonometricReduction::computeFourierFractionForm() {
  STACK_TRACE("TrigonometricReduction::computeEulerFormExpression");
  if (this->numeratorStarting.isEqualToZero()) {
    this->fourierFractionForm = this->owner->expressionZero();
    return true;
  }
  LinearCombination<Expression, AlgebraicNumber> numerator;
  LinearCombination<Expression, AlgebraicNumber> denominator;
  this->eulerFormToFourierForm(this->numeratorFinal, numerator);
  this->eulerFormToFourierForm(this->denominatorFinal, denominator);
  this->normalizeNumeratorAndDenominator(numerator, denominator, *this->owner);
  Expression numeratorExpression;
  Expression denominatorExpression;
  numeratorExpression.makeSumFromLinearCombination(*this->owner, numerator);
  denominatorExpression.makeSumFromLinearCombination(
    *this->owner, denominator
  );
  this->fourierFractionForm.makeXOX(
    *this->owner,
    this->owner->opDivide(),
    numeratorExpression,
    denominatorExpression
  );
  return true;
}

void TrigonometricReduction::computeEulerFormExpressionPresentation() {
  STACK_TRACE(
    "TrigonometricReduction::computeEulerFormExpressionPresentation"
  );
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

bool TrigonometricReduction::eulerFormToFourierForm(
  const Polynomial<AlgebraicNumber>& input,
  LinearCombination<Expression, AlgebraicNumber>& output
) {
  STACK_TRACE("TrigonometricReduction::eulerFormToTrigonometryFourierForm");
  output.makeZero();
  int numberOfVariables = input.minimalNumberOfVariables();
  AlgebraicNumber imaginaryUnit =
  this->owner->objectContainer.algebraicClosure.imaginaryUnit();
  for (int i = 0; i < input.size(); i ++) {
    const MonomialPolynomial& monomial = input[i];
    AlgebraicNumber coefficient = input.coefficients[i];
    LinearCombination<Expression, Rational> currentArgument;
    for (int j = 0; j < numberOfVariables; j ++) {
      Expression term = this->argumentTermIndex(j, this->argumentScales);
      currentArgument.addMonomial(term, monomial(j));
    }
    if (currentArgument.isEqualToZero()) {
      output.addMonomial(this->owner->expressionOne(), coefficient);
      continue;
    }
    bool argumentNegated = currentArgument.coefficients[0] < 0;
    if (argumentNegated) {
      currentArgument *= - 1;
    }
    Expression currentArgumentSummed;
    currentArgumentSummed.makeSumFromLinearCombination(
      *this->owner, currentArgument
    );
    Expression sine;
    sine.makeOX(*this->owner, this->owner->opSin(), currentArgumentSummed);
    Expression cosine;
    cosine.makeOX(*this->owner, this->owner->opCos(), currentArgumentSummed);
    output.addMonomial(cosine, coefficient);
    AlgebraicNumber sineCoefficient = coefficient * imaginaryUnit;
    if (argumentNegated) {
      sineCoefficient *= - 1;
    }
    output.addMonomial(sine, sineCoefficient);
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
      Calculator::Functions::Names::Trigonometry::sine
    )
  ) {
    trigonometricFunction = Calculator::Functions::Names::Trigonometry::sine;
    this->isSine = true;
  }
  if (
    trigonometricExpression.startsWithGivenOperation(
      Calculator::Functions::Names::Trigonometry::cosine
    )
  ) {
    trigonometricFunction = Calculator::Functions::Names::Trigonometry::cosine;
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
    *this->owner->owner, trigonometricExpression[1], this->arguments
  );
  return true;
}

void TrigonometricReduction::TrigonometricFunction::
computeEulerFormAnonymousVariables() {
  STACK_TRACE(
    "TrigonometricReduction::"
    "TrigonometricFunction::computeEulerFormAnonymous"
  );
  this->eulerForm.makeZero();
  Polynomial<AlgebraicNumber> plusSummand;
  Polynomial<AlgebraicNumber> minusSummand;
  plusSummand.makeConstant(
    this->owner->owner->objectContainer.algebraicClosure.one()
  );
  minusSummand.makeConstant(
    this->owner->owner->objectContainer.algebraicClosure.one()
  );
  for (int i = 0; i < this->arguments.size(); i ++) {
    const Expression& monomial = this->arguments.monomials[i];
    int variableIndex =
    this->owner->startingTrigonometricBaseMonomials.getIndex(monomial);
    MonomialPolynomial anonymousMonomial;
    Rational scale = this->arguments.coefficients[i] /
    this->owner->argumentScales.values[variableIndex];
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
  sum.makeSumFromLinearCombination(calculator, this->arguments);
  Expression plusExponent = calculator.expressionSquareRootNegativeOne() * sum;
  Expression minusExponent = calculator.expressionMinusOne() * plusExponent;
  Expression plusSummand;
  Expression e;
  e.makeAtom(calculator, calculator.opE());
  plusSummand.makeXOX(calculator, calculator.opPower(), e, plusExponent);
  Expression minusSummand;
  minusSummand.makeXOX(calculator, calculator.opPower(), e, minusExponent);
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
  sum.makeSumFromLinearCombination(*this->owner->owner, this->arguments);
  out << sum.toString();
  out << ")";
  return out.str();
}
