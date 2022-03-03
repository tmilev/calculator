 // The current file is licensed under the license terms found in the main header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_inner_functions.h"

bool CalculatorFunctionsTrigonometry::sin(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::sin");
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
        AlgebraicNumber algOutput;
        Rational ratOutput;
        if (
          algOutput.assignSinRationalTimesPi(
            piProportion, calculator.objectContainer.algebraicClosure
          )
        ) {
          if (algOutput.isRational(&ratOutput)) {
            return output.assignValue(calculator, ratOutput);
          }
          return output.assignValue(calculator, algOutput);
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
  MacroRegisterFunctionWithName("CalculatorFunctions::cos");
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
        AlgebraicNumber algOutput;
        Rational ratOutput;
        if (
          algOutput.assignCosRationalTimesPi(
            piProportion, calculator.objectContainer.algebraicClosure
          )
        ) {
          if (algOutput.isRational(&ratOutput)) {
            return output.assignValue(calculator, ratOutput);
          }
          return output.assignValue(calculator, algOutput);
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
  MacroRegisterFunctionWithName("CalculatorFunctionsTrigonometry::tan");
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
  MacroRegisterFunctionWithName("CalculatorFunctionsTrigonometry::cotangent");
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
  MacroRegisterFunctionWithName("CalculatorFunctionsTrigonometry::sec");
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
  MacroRegisterFunctionWithName("CalculatorFunctionsTrigonometry::csc");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression num, den;
  num.assignValue(calculator, 1);
  den.makeOX(calculator, calculator.opSin(), argument);
  return output.makeXOX(calculator, calculator.opDivide(), num, den);
}

bool CalculatorFunctionsTrigonometry::eulerFormulaAsLaw(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName(
    "CalculatorFunctionsTrigonometry::eulerFormulaAsLaw"
  );
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
  MacroRegisterFunctionWithName(
    "CalculatorFunctionsTrigonometry::exploitCosineEvenness"
  );
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression cfE, nonCFpart;
  argument.getCoefficientMultiplicandForm(cfE, nonCFpart);
  Rational rational;
  if (!cfE.isRational(&rational)) {
    return false;
  }
  if (rational >= 0) {
    return false;
  }
  Expression moneE;
  moneE.assignValue(calculator, - 1);
  return
  output.makeOX(calculator, calculator.opCos(), moneE * cfE * nonCFpart);
}

bool CalculatorFunctionsTrigonometry::exploitSineOddness(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName(
    "CalculatorFunctionsTrigonometry::exploitSineOddness"
  );
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression cfE, nonCFpart;
  argument.getCoefficientMultiplicandForm(cfE, nonCFpart);
  Rational rational;
  if (!cfE.isRational(&rational)) {
    return false;
  }
  if (rational >= 0) {
    return false;
  }
  Expression moneE, sinE;
  moneE.assignValue(calculator, - 1);
  sinE.makeOX(calculator, calculator.opSin(), moneE * cfE * nonCFpart);
  output = moneE * sinE;
  return true;
}

bool CalculatorFunctionsTrigonometry::convertSineToExponent(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName(
    "CalculatorFunctionsTrigonometry::convertSineToExponent"
  );
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
  MacroRegisterFunctionWithName(
    "CalculatorFunctionsTrigonometry::convertCosineToExponent"
  );
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
  MacroRegisterFunctionWithName("CalculatorFunctions::arccosAlgebraic");
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
  MacroRegisterFunctionWithName("CalculatorFunctions::arcsinAlgebraic");
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
  MacroRegisterFunctionWithName("CalculatorFunctions::arctan");
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
  MacroRegisterFunctionWithName("CalculatorFunctions::arccos");
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
  MacroRegisterFunctionWithName("CalculatorFunctions::arcsin");
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
  MacroRegisterFunctionWithName(
    "CalculatorFunctionsTrigonometry::sineOfAngleSumToTrigonometry"
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
  output = sinA * cosB + cosA * sinB;
  return true;
}

bool CalculatorFunctionsTrigonometry::trigonometricSumToTrigonometricProduct(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName(
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
  MacroRegisterFunctionWithName(
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
  Expression input;
  Calculator* owner;
  WithContext<RationalFraction<AlgebraicNumber> > inputFraction;
  std::string errorString;
  void initialize(Calculator& inputOwner, const Expression& incoming);
  bool reduce();
  std::string toString();
};

bool CalculatorFunctionsTrigonometry::fourierFractionForm(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName(
    "CalculatorFunctionsTrigonometry::fourierFractionForm"
  );
  if (input.size() != 2) {
    return false;
  }
  TrigonometricReduction trigonometricReduction;
  trigonometricReduction.initialize(calculator, input[1]);
  trigonometricReduction.reduce();
  return output.assignValue(calculator, trigonometricReduction.toString());
}

std::string TrigonometricReduction::toString() {
  std::stringstream out;
  out << this->inputFraction.toString();
  out << "not implemented yet";
  return out.str();
}

void TrigonometricReduction::initialize(
  Calculator& inputOwner, const Expression& incoming
) {
  this->owner = &inputOwner;
  this->input = incoming;
}

bool TrigonometricReduction::reduce() {
  HashedList<std::string> variables;
  std::string reductionRules;
  if (CalculatorConversions::functionRationalFraction(*this->owner, this->input, this->inputFraction, false)){
    this->errorString = "Failed to extract rational fraction.";
    return  false;
  }


  reductionRules = "";
  return false;
}
