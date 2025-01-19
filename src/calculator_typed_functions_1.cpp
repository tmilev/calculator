#include "calculator_inner_typed_functions.h"
#include "math_extra_elliptic_curves.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_modules_semisimple_lie_algebras.h"
#include "math_extra_semisimple_lie_algebras.h"
#include "math_extra_universal_enveloping.h"
#include "math_general_implementation.h"
#include "math_general_polynomial_computations_advanced_implementation.h" // IWYU pragma: keep: breaks g++ -02 optimization build.
#include "math_rational_function.h"
#include "math_weyl_algebras.h"

bool CalculatorFunctionsBinaryOps::
addElementZModPOrRationalToElementZModPOrRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "addElementZModPOrRationalToElementZModPOrRational"
  );
  if (input.size() != 3) {
    return false;
  }
  const Expression* leftExpression;
  const Expression* rightExpression;
  leftExpression = &input[1];
  rightExpression = &input[2];
  ElementZmodP element1;
  ElementZmodP element2;
  for (
    int i = 0; i < 2;
    i ++,
    MathRoutines::swap(leftExpression, rightExpression)
  ) {
    if (leftExpression->isOfType<ElementZmodP>(&element1)) {
      if (rightExpression->isOfType<ElementZmodP>(&element2)) {
        if (element1.modulus != element2.modulus) {
          return false;
        }
      } else {
        Rational rationalValue;
        if (!rightExpression->isOfType<Rational>(&rationalValue)) {
          return false;
        }
        element2.modulus = element1.modulus;
        if (!element2.assignRational(rationalValue)) {
          return false;
        }
      }
      element1 += element2;
      ExpressionContext context(calculator);
      context.setDefaultModulus(element1.modulus);
      return output.assignValueWithContext(calculator, element1, context);
    }
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::
multiplyElementZmodPorRationalByElementZmodPorRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "multiplyElementZmodPorRationalByElementZmodPorRational"
  );
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression* leftExpression;
  const Expression* rightExpression;
  leftExpression = &input[1];
  rightExpression = &input[2];
  ElementZmodP element1;
  ElementZmodP element2;
  for (
    int i = 0; i < 2;
    i ++,
    MathRoutines::swap(leftExpression, rightExpression)
  ) {
    if (leftExpression->isOfType<ElementZmodP>(&element1)) {
      if (rightExpression->isOfType<ElementZmodP>(&element2)) {
        if (element1.modulus != element2.modulus) {
          return false;
        }
      } else {
        Rational rationalValue;
        if (!rightExpression->isOfType<Rational>(&rationalValue)) {
          return false;
        }
        element2.modulus = element1.modulus;
        if (!element2.assignRational(rationalValue)) {
          return false;
        }
      }
      element1 *= element2;
      ExpressionContext context(calculator);
      context.setDefaultModulus(element1.modulus);
      return output.assignValueWithContext(calculator, element1, context);
    }
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::
divideElementZmodPorRationalByElementZmodPorRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::divideElementZmodPorRationalByElementZmodPorRational"
  );
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression* leftExpression;
  const Expression* rightExpression;
  leftExpression = &input[1];
  rightExpression = &input[2];
  ElementZmodP element1;
  ElementZmodP element2;
  for (
    int i = 0; i < 2;
    i ++,
    MathRoutines::swap(leftExpression, rightExpression)
  ) {
    if (leftExpression->isOfType<ElementZmodP>(&element1)) {
      if (rightExpression->isOfType<ElementZmodP>(&element2)) {
        if (element1.modulus != element2.modulus) {
          return false;
        }
      } else {
        Rational rationalValue;
        if (!rightExpression->isOfType<Rational>(&rationalValue)) {
          return false;
        }
        element2.modulus = element1.modulus;
        if (!element2.assignRational(rationalValue)) {
          return false;
        }
      }
      if (i == 1) {
        MathRoutines::swap(element1, element2);
      }
      if (!(element1 /= element2)) {
        std::stringstream out;
        out
        << "Got division by zero while attempting to divide "
        << element1.toString()
        << " by "
        << element2.toString();
        return output.assignError(calculator, out.str());
      }
      ExpressionContext context(calculator);
      context.setDefaultModulus(element1.modulus);
      return output.assignValueWithContext(calculator, element1, context);
    }
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::addRationalToRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::addRationalToRational");
  if (input.size() != 3) {
    return false;
  }
  Rational leftRational;
  Rational rightRational;
  if (
    !input[1].isOfType(&leftRational) || !input[2].isOfType(&rightRational)
  ) {
    return false;
  }
  return output.assignValue(calculator, leftRational + rightRational);
}

bool CalculatorFunctionsBinaryOps::addStringToString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::addStringToString");
  if (input.size() != 3) {
    return false;
  }
  std::string left;
  std::string right;
  if (
    !input[1].isOfType(&left) || !input[2].isOfType(&right)
  ) {
    return false;
  }
  return output.assignValue(calculator, left + right);
}

bool CalculatorFunctionsBinaryOps::
divideAlgebraicNumberOrRationalByAlgebraicNumberOrRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "divideAlgebraicNumberOrRationalByAlgebraicNumberOrRational"
  );
  if (!input.isListNElements(3)) {
    return false;
  }
  AlgebraicNumber leftAlgebraicNumber;
  AlgebraicNumber rightAlgebraicNumber;
  Rational rationalValue;
  if (!input[1].isOfType(&leftAlgebraicNumber)) {
    if (!input[2].isOfType(&rightAlgebraicNumber)) {
      return false;
    }
    if (!input[1].isOfType<Rational>(&rationalValue)) {
      return false;
    }
    leftAlgebraicNumber.assignRational(
      rationalValue, &calculator.objectContainer.algebraicClosure
    );
  } else if (!input[2].isOfType(&rightAlgebraicNumber)) {
    if (!input[2].isOfType(&rationalValue)) {
      return false;
    }
    rightAlgebraicNumber.assignRational(
      rationalValue, &calculator.objectContainer.algebraicClosure
    );
  }
  if (rightAlgebraicNumber.isEqualToZero()) {
    return output.assignError(calculator, "Division by zero. ");
  }
  leftAlgebraicNumber /= rightAlgebraicNumber;
  output.assignValue(calculator, leftAlgebraicNumber);
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctionsBinaryOps::multiplyElementHypOctByElementHypOct(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::multiplyElementHypOctByElementHypOct"
  );
  if (!input.isListNElements(3)) {
    return false;
  }
  ElementHyperoctahedralGroupR2 outElement;
  ElementHyperoctahedralGroupR2 left;
  ElementHyperoctahedralGroupR2 right;
  if (
    !input[1].isOfType<ElementHyperoctahedralGroupR2>(&left) ||
    !input[2].isOfType<ElementHyperoctahedralGroupR2>(&right)
  ) {
    return false;
  }
  outElement = left * right;
  return output.assignValue(calculator, outElement);
}

bool CalculatorFunctionsBinaryOps::multiplyAlgebraicNumberByAlgebraicNumber(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::multiplyAlgebraicNumberByAlgebraicNumber"
  );
  if (!input.isListNElements(3)) {
    return false;
  }
  AlgebraicNumber leftAlgebraicNumber;
  AlgebraicNumber rightAlgebraicNumber;
  Rational rationalValue;
  if (!input[1].isOfType(&leftAlgebraicNumber)) {
    if (!input[2].isOfType(&rightAlgebraicNumber)) {
      return false;
    }
    if (!input[1].isOfType<Rational>(&rationalValue)) {
      return false;
    }
    leftAlgebraicNumber.assignRational(
      rationalValue, &calculator.objectContainer.algebraicClosure
    );
  } else if (!input[2].isOfType(&rightAlgebraicNumber)) {
    if (!input[2].isOfType(&rationalValue)) {
      return false;
    }
    rightAlgebraicNumber.assignRational(
      rationalValue, &calculator.objectContainer.algebraicClosure
    );
  }
  leftAlgebraicNumber *= rightAlgebraicNumber;
  return output.assignValue(calculator, leftAlgebraicNumber);
}

bool CalculatorFunctionsBinaryOps::addAlgebraicNumberToAlgebraicNumber(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::addAlgebraicNumberToAlgebraicNumber"
  );
  if (!input.isListNElements(3)) {
    return false;
  }
  AlgebraicNumber leftAN;
  AlgebraicNumber rightAN;
  Rational rationalValue;
  if (!input[1].isOfType(&leftAN)) {
    if (!input[2].isOfType(&rightAN)) {
      return false;
    }
    if (!input[1].isOfType<Rational>(&rationalValue)) {
      return false;
    }
    leftAN.assignRational(
      rationalValue, &calculator.objectContainer.algebraicClosure
    );
    leftAN.checkConsistency();
  } else if (!input[2].isOfType(&rightAN)) {
    if (!input[2].isOfType(&rationalValue)) {
      return false;
    }
    rightAN.assignRational(
      rationalValue, &calculator.objectContainer.algebraicClosure
    );
    rightAN.checkConsistency();
  }
  leftAN.checkConsistency();
  rightAN.checkConsistency();
  leftAN += rightAN;
  leftAN.checkConsistency();
  return output.assignValue(calculator, leftAN);
}

bool CalculatorFunctionsBinaryOps::multiplyRationalByRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::multiplyRationalByRational");
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational leftR;
  Rational rightR;
  if (
    !input[1].isOfType(&leftR) || !input[2].isOfType(&rightR)
  ) {
    return false;
  }
  return output.assignValue(calculator, leftR* rightR);
}

bool CalculatorFunctionsBinaryOps::multiplyCoxeterElementByCoxeterElement(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::multiplyCoxeterElementByCoxeterElement"
  );
  if (!input.isListNElements(3)) {
    return false;
  }
  ElementWeylGroup leftR;
  ElementWeylGroup rightR;
  if (
    !input[1].isOfType(&leftR) || !input[2].isOfType(&rightR)
  ) {
    return false;
  }
  if (leftR.owner != rightR.owner) {
    return
    calculator
    <<
    "<hr>Attempting to multiply elements of different groups, possible user typo?"
    ;
  }
  leftR *= rightR;
  return output.assignValue(calculator, leftR);
}

bool CalculatorFunctionsBinaryOps::powerWeylGroupElementByInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::powerWeylGroupElementByInteger");
  if (input.size() != 3) {
    return false;
  }
  ElementWeylGroup left;
  if (!input[1].isOfType(&left)) {
    return false;
  }
  LargeInteger power;
  if (!input[2].isInteger(&power)) {
    return false;
  }
  ElementWeylGroup identity;
  identity.makeIdentity(*left.owner);
  MathRoutines::raiseToPower(left, power, identity);
  return output.assignValue(calculator, left);
}

bool CalculatorFunctionsBinaryOps::divideRationalByRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::divideRationalByRational");
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational leftR;
  Rational rightR;
  if (
    !input[1].isOfType(&leftR) || !input[2].isOfType(&rightR)
  ) {
    return false;
  }
  if (rightR.isEqualToZero()) {
    return output.assignError(calculator, "Division by zero.");
  }
  return output.assignValue(calculator, leftR / rightR);
}

bool CalculatorFunctionsBinaryOps::divideDoubleByDouble(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::divideDoubleByDouble");
  if (!input.isListNElements(3)) {
    return false;
  }
  double leftD;
  double rightD;
  Rational leftR;
  Rational rightR;
  if (!input[1].isOfType(&leftD)) {
    if (!input[1].isOfType(&leftR)) {
      return false;
    } else {
      leftD = leftR.getDoubleValue();
    }
  }
  if (!input[2].isOfType(&rightD)) {
    if (!input[2].isOfType(&rightR)) {
      return false;
    } else {
      rightD = rightR.getDoubleValue();
    }
  }
  if (rightD == 0.0) {
    return output.assignError(calculator, "Division by zero.");
  }
  return output.assignValue(calculator, leftD / rightD);
}

bool CalculatorFunctionsBinaryOps::tensorElementTensorByElementTensor(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::tensorElementTensorByElementTensor"
  );
  calculator.checkInputNotSameAsOutput(input, output);
  if (input.size() != 3) {
    return false;
  }
  Expression inputConverted;
  if (
    !input.mergeContextsMyArumentsAndConvertThem<
      ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
    >(inputConverted, &calculator.comments)
  ) {
    return false;
  }
  ElementTensorsGeneralizedVermas<RationalFraction<Rational> > resultTensor;
  resultTensor =
  inputConverted[1].getValue<
    ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
  >();
  resultTensor.tensorOnTheRight(
    inputConverted[2].getValue<
      ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
    >()
  );
  return
  output.assignValueWithContext(
    calculator, resultTensor, inputConverted[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::addWeightToWeight(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::addWeightToWeight");
  return
  CalculatorFunctionsBinaryOps::addTypeToType<
    Weight<Polynomial<Rational> >
  >(calculator, input, output);
}

bool CalculatorFunctionsBinaryOps::
multiplyRationalOrPolynomialByWeightPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "multiplyRationalOrPolynomialByWeightPolynomial"
  );
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (
    !input.mergeContextsMyAruments(inputConverted, &calculator.comments)
  ) {
    return false;
  }
  Weight<Polynomial<Rational> > weight;
  Rational cfRat;
  Polynomial<Rational> coefficient;
  if (
    !inputConverted[1].isOfType<Polynomial<Rational> >(&coefficient)
  ) {
    if (!inputConverted[1].isOfType<Rational>(&cfRat)) {
      return false;
    }
    coefficient = cfRat;
  }
  if (
    !inputConverted[2].isOfType<Weight<Polynomial<Rational> > >(&weight)
  ) {
    return false;
  }
  weight *= coefficient;
  return
  output.assignValueWithContext(
    calculator, weight, inputConverted[2].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::multiplyWeylGroupElementByWeightPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::multiplyWeylGroupElementByWeightPolynomial"
  );
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (
    !input.mergeContextsMyAruments(inputConverted, &calculator.comments)
  ) {
    return false;
  }
  Weight<Polynomial<Rational> > weight;
  if (
    !inputConverted[2].isOfType<Weight<Polynomial<Rational> > >(&weight)
  ) {
    return false;
  }
  if (!inputConverted[1].isOfType<ElementWeylGroup>()) {
    return false;
  }
  ElementWeylGroup element = inputConverted[1].getValue<ElementWeylGroup>();
  if (element.owner != &weight.owner->weylGroup) {
    return
    calculator
    << "<hr>Possible user input error: attempting to apply Weyl group "
    << "element to weight corresponding to different Weyl group.";
  }
  Polynomial<Rational> zero;
  Vector<Polynomial<Rational> > weightSimpleCoordinates =
  element.owner->getSimpleCoordinatesFromFundamental(
    weight.weightFundamentalCoordinates, zero
  );
  element.actOn(weightSimpleCoordinates);
  weight.weightFundamentalCoordinates =
  element.owner->getFundamentalCoordinatesFromSimple(weightSimpleCoordinates);
  return
  output.assignValueWithContext(
    calculator, weight, inputConverted[2].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::multiplyEllipticCurveElements(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::multiplyEllipticCurveElements");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  ElementEllipticCurve<Rational> left;
  ElementEllipticCurve<Rational> right;
  if (
    !input[1].isOfType(&left) || !input[2].isOfType(&right)
  ) {
    return false;
  }
  if (!(left *= right)) {
    return false;
  }
  return
  output.assignValueWithContext(
    calculator, left, input[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::multiplyEllipticCurveElementsZmodP(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::multiplyEllipticCurveElements");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  ElementEllipticCurve<ElementZmodP> left;
  ElementEllipticCurve<ElementZmodP> right;
  if (
    !input[1].isOfType(&left) || !input[2].isOfType(&right)
  ) {
    return false;
  }
  if (!left.flagInfinity && !right.flagInfinity) {
    if (left.xCoordinate.modulus != right.xCoordinate.modulus) {
      return
      calculator
      << "Attempt to multiply elliptic curve elements over Z mod p "
      << "for different moduli p. Possible bad user input?";
    }
  }
  if (!(left *= right)) {
    return false;
  }
  return
  output.assignValueWithContext(
    calculator, left, input[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::multiplyAnyByElementTensor(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::multiplyAnyByElementTensor");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (
    !input.mergeContextsMyAruments(inputConverted, &calculator.comments)
  ) {
    return false;
  }
  if (
    !inputConverted[2].isOfType<
      ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
    >()
  ) {
    return false;
  }
  SemisimpleLieAlgebra& semisimpleLieAlgebra =
  inputConverted[2].getValue<
    ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
  >().getOwnerSemisimple();
  inputConverted[1].checkConsistency();
  input[1].checkConsistency();
  input[2].checkConsistency();
  WithContext<ElementUniversalEnveloping<RationalFraction<Rational> > >
  element;
  if (
    !CalculatorConversions::convertWithoutComputation(
      calculator, inputConverted[1], element
    )
  ) {
    return false;
  }
  const ElementTensorsGeneralizedVermas<RationalFraction<Rational> >&
  rightElementETGVM =
  inputConverted[2].getValue<
    ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
  >();
  ElementTensorsGeneralizedVermas<RationalFraction<Rational> > outputElement;
  if (rightElementETGVM.isEqualToZero()) {
    output = inputConverted[2];
    return true;
  }
  semisimpleLieAlgebra.orderNilradicalNilWeightAscending(
    rightElementETGVM.getOwnerModule().
    parabolicSelectionNonSelectedAreElementsLevi
  );
  semisimpleLieAlgebra.flagHasNilradicalOrder = true;
  RationalFraction<Rational> one(Rational::one());
  if (
    !rightElementETGVM.multiplyOnTheLeft(
      element.content, outputElement, semisimpleLieAlgebra, one
    )
  ) {
    return false;
  }
  return
  output.assignValueWithContext(
    calculator, outputElement, inputConverted[2].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::
multiplyRationalOrPolynomialOrElementWeylAlgebraByRationalOrPolynomialOrElementWeylAlgebra
(Calculator& calculator, const Expression& input, Expression& output) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "multiplyRationalOrPolynomialOrElementWeylAlgebraByRationalOrPolynomialOrElementWeylAlgebra"
  );
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (
    !input.mergeContextsMyArumentsAndConvertThem<
      ElementWeylAlgebra<Rational>
    >(inputContextsMerged, &calculator.comments)
  ) {
    return false;
  }
  if (
    inputContextsMerged[1].getValue<ElementWeylAlgebra<Rational> >().
    hasNonSmallPositiveIntegerDerivation() ||
    inputContextsMerged[2].getValue<ElementWeylAlgebra<Rational> >().
    hasNonSmallPositiveIntegerDerivation()
  ) {
    return
    calculator
    << "<hr> Failed to multiply "
    << inputContextsMerged[1].toString()
    << " by "
    << inputContextsMerged[2].toString()
    << ": "
    << "one of the two differential operators has "
    << "differential operator exponent that is not a small integer. ";
  }
  ElementWeylAlgebra<Rational> result =
  inputContextsMerged[1].getValue<ElementWeylAlgebra<Rational> >();
  result *= inputContextsMerged[2].getValue<ElementWeylAlgebra<Rational> >();
  return
  output.assignValueWithContext(
    calculator, result, inputContextsMerged[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::
multiplyRationalOrPolynomialOrRationalFunctionByRationalFraction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "multiplyRationalOrPolynomialOrRationalFunctionByRationalFunction"
  );
  if (
    !CalculatorFunctionsBinaryOps::multiplyTypeByType<
      RationalFraction<Rational>
    >(calculator, input, output)
  ) {
    if (
      !CalculatorFunctionsBinaryOps::multiplyTypeByType<
        RationalFraction<AlgebraicNumber>
      >(calculator, input, output)
    ) {
      return false;
    }
    RationalFraction<AlgebraicNumber> simplified;
    if (!output.isOfType(&simplified)) {
      return false;
    }
    simplified.simplify();
    return
    output.assignValueWithContext(
      calculator, simplified, output.getContext()
    );
  }
  RationalFraction<Rational> simplified;
  if (!output.isOfType(&simplified)) {
    return false;
  }
  simplified.simplify();
  return
  output.assignValueWithContext(
    calculator, simplified, output.getContext()
  );
}

bool CalculatorFunctionsBinaryOps::
divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial"
  );
  if (
    !CalculatorFunctionsBinaryOps::divideTypeByType<
      RationalFraction<Rational>
    >(calculator, input, output)
  ) {
    if (
      !CalculatorFunctionsBinaryOps::divideTypeByType<
        RationalFraction<AlgebraicNumber>
      >(calculator, input, output)
    ) {
      return false;
    }
    RationalFraction<AlgebraicNumber> simplified;
    if (!output.isOfType(&simplified)) {
      return false;
    }
    simplified.simplify();
    return
    output.assignValueWithContext(
      calculator, simplified, output.getContext()
    );
  }
  RationalFraction<Rational> simplified;
  if (!output.isOfType(&simplified)) {
    return false;
  }
  simplified.simplify();
  return
  output.assignValueWithContext(
    calculator, simplified, output.getContext()
  );
}

bool CalculatorFunctionsBinaryOps::
addRationalOrPolynomialOrRationalFunctionToRationalFunction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "addRationalOrPolynomialOrRationalFunctionToRationalFunction"
  );
  if (
    CalculatorFunctionsBinaryOps::addTypeToType<RationalFraction<Rational> >(
      calculator, input, output
    )
  ) {
    return true;
  }
  if (
    CalculatorFunctionsBinaryOps::addTypeToType<
      RationalFraction<AlgebraicNumber>
    >(calculator, input, output)
  ) {
    return true;
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::
addRationalOrPolynomialOrElementWeylAlgebraToRattionalOrPolynomialOrElementWeylAlgebra
(Calculator& calculator, const Expression& input, Expression& output) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "addRationalOrPolynomialOrElementWeylAlgebraToRattionalOrPolynomialOrElementWeylAlgebra"
  );
  return
  CalculatorFunctionsBinaryOps::addTypeToType<ElementWeylAlgebra<Rational> >(
    calculator, input, output
  );
}

bool CalculatorFunctionsBinaryOps::
multiplyNumberOrPolynomialByNumberOrPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::multiplyNumberOrPolynomialByNumberOrPolynomial"
  );
  if (input.size() < 3) {
    return false;
  }
  const Expression leftExpression = input[1];
  const Expression rightExpression = input[2];
  if (
    leftExpression.isOfType<AlgebraicNumber>() ||
    leftExpression.isOfType<Polynomial<AlgebraicNumber> >() ||
    rightExpression.isOfType<AlgebraicNumber>() ||
    rightExpression.isOfType<Polynomial<AlgebraicNumber> >()
  ) {
    return
    CalculatorFunctionsBinaryOps::multiplyTypeByType<
      Polynomial<AlgebraicNumber>
    >(calculator, input, output);
  }
  return
  CalculatorFunctionsBinaryOps::multiplyTypeByType<Polynomial<Rational> >(
    calculator, input, output
  );
}

bool CalculatorFunctionsBinaryOps::addUniversalEnvelopingAlgebraElementToAny(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::addUniversalEnvelopingAlgebraElementToAny"
  );
  return
  CalculatorFunctionsBinaryOps::addTypeToType<
    ElementUniversalEnveloping<RationalFraction<Rational> >
  >(calculator, input, output);
}

bool CalculatorFunctionsBinaryOps::multiplyAnyByElementUniversalEnveloping(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::multiplyAnyByElementUniversalEnveloping"
  );
  calculator.checkInputNotSameAsOutput(input, output);
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (
    !input.mergeContextsMyArumentsAndConvertThem<
      ElementUniversalEnveloping<RationalFraction<Rational> >
    >(inputContextsMerged, &calculator.comments)
  ) {
    return false;
  }
  ElementUniversalEnveloping<RationalFraction<Rational> > result =
  inputContextsMerged[1].getValue<
    ElementUniversalEnveloping<RationalFraction<Rational> >
  >();
  result *=
  inputContextsMerged[2].getValue<
    ElementUniversalEnveloping<RationalFraction<Rational> >
  >();
  result.simplify(RationalFraction<Rational>::oneRational());
  return
  output.assignValueWithContext(
    calculator, result, inputContextsMerged[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::multiplyLRObyLRO(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::multiplyLRObyLRO");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  output = input[1];
  Expression rightCopy = input[2];
  if (
    !output.isOfType<MonomialTensor<int, HashFunctions::hashFunction> >() ||
    !rightCopy.isOfType<MonomialTensor<int, HashFunctions::hashFunction> >()
  ) {
    return false;
  }
  const MonomialTensor<int, HashFunctions::hashFunction>& leftMonomial =
  output.getValue<MonomialTensor<int, HashFunctions::hashFunction> >();
  const MonomialTensor<int, HashFunctions::hashFunction>& rightMonomial =
  rightCopy.getValue<MonomialTensor<int, HashFunctions::hashFunction> >();
  MonomialTensor<int, HashFunctions::hashFunction> result;
  result = leftMonomial;
  result *= rightMonomial;
  for (int i = 0; i < result.generatorsIndices.size; i ++) {
    if (result.powers[i] > 100000 || result.powers[i] < 0) {
      return
      output.assignError(
        calculator,
        "The result of this operation is " +
        result.toString() +
        " which is outside of the allowed range. "
      );
    }
  }
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsBinaryOps::
multiplyLittlemannRootOperatorByLakshmibaiSeshadriPath(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::multiplyLRObyLSPath");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  output = input[1];
  Expression rightCopy = input[2];
  if (!output.mergeContexts(output, rightCopy)) {
    return false;
  }
  if (
    !output.isOfType<MonomialTensor<int, HashFunctions::hashFunction> >() ||
    !rightCopy.isOfType<LittelmannPath>()
  ) {
    return false;
  }
  LittelmannPath result = rightCopy.getValue<LittelmannPath>();
  WeylGroupData& weylGroup = *result.owner;
  MonomialTensor<int, HashFunctions::hashFunction> littlemannRootOperator;
  littlemannRootOperator =
  output.getValue<MonomialTensor<int, HashFunctions::hashFunction> >();
  for (
    int i = littlemannRootOperator.generatorsIndices.size - 1; i >= 0; i --
  ) {
    if (
      littlemannRootOperator.generatorsIndices[i] == 0 ||
      littlemannRootOperator.generatorsIndices[i] < - weylGroup.getDimension()
      ||
      littlemannRootOperator.generatorsIndices[i] > weylGroup.getDimension()
    ) {
      std::stringstream out;
      out
      << " The Littelmann root operator "
      << "must have an index whose absolute value "
      << "is between 1 and the rank "
      << "of the ambient Lie algebra, instead I get index  "
      << littlemannRootOperator.generatorsIndices[i];
      return output.assignError(calculator, out.str());
    } else {
      for (int j = 0; j < littlemannRootOperator.powers[i]; j ++) {
        result.actByEFDisplayIndex(
          littlemannRootOperator.generatorsIndices[i]
        );
      }
    }
  }
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsBinaryOps::addElementTensorToElementTensor(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::addElementTensorToElementTensor");
  return
  CalculatorFunctionsBinaryOps::addTypeToType<
    ElementTensorsGeneralizedVermas<RationalFraction<Rational> >
  >(calculator, input, output);
}

bool CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::addNumberOrPolynomialToNumberOrPolynomial"
  );
  if (input.size() < 3) {
    return false;
  }
  const Expression leftExpression = input[1];
  const Expression rightExpression = input[2];
  if (
    leftExpression.isOfType<AlgebraicNumber>() ||
    leftExpression.isOfType<Polynomial<AlgebraicNumber> >() ||
    rightExpression.isOfType<AlgebraicNumber>() ||
    rightExpression.isOfType<Polynomial<AlgebraicNumber> >()
  ) {
    return
    CalculatorFunctionsBinaryOps::addTypeToType<Polynomial<AlgebraicNumber> >(
      calculator, input, output
    );
  }
  return
  CalculatorFunctionsBinaryOps::addTypeToType<Polynomial<Rational> >(
    calculator, input, output
  );
}

bool CalculatorFunctionsBinaryOps::
addPolynomialModuloPolynomialModuloIntegerToAnother(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "addPolynomialModuloPolynomialModuloIntegerToAnother"
  );
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (
    !input.mergeContextsMyArumentsAndConvertThem<
      PolynomialModuloPolynomial<ElementZmodP>
    >(inputContextsMerged, &calculator.comments)
  ) {
    return false;
  }
  PolynomialModuloPolynomial<ElementZmodP> left =
  inputContextsMerged[1].getValue<PolynomialModuloPolynomial<ElementZmodP> >();
  PolynomialModuloPolynomial<ElementZmodP> right =
  inputContextsMerged[2].getValue<PolynomialModuloPolynomial<ElementZmodP> >();
  if (!left.isEqualToZero() && !right.isEqualToZero()) {
    if (left.modulusContainer != right.modulusContainer) {
      return
      calculator
      << "Attempt to add polynomial quotients with different moduli. ";
    }
  }
  left += right;
  return
  output.assignValueWithContext(
    calculator, left, inputContextsMerged[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::addPolynomialModuloIntegerToInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::addPolynomialModuloIntegerToInteger"
  );
  if (input.size() != 3) {
    return false;
  }
  Polynomial<ElementZmodP> polynomial;
  if (!input[1].isOfType(&polynomial)) {
    return false;
  }
  if (polynomial.isEqualToZero()) {
    return false;
  }
  LargeIntegerUnsigned modulus = polynomial.coefficients[0].modulus;
  LargeInteger constantInteger;
  ElementZmodP constant;
  constant.makeOne(modulus);
  if (input[2].isInteger(&constantInteger)) {
    constant = constantInteger;
  } else if (input.isOfType(&constant)) {
    if (constant.modulus != modulus) {
      return
      calculator
      << "Attempt to add polynomial modulo "
      << modulus
      << " to integer modulo "
      << constant.modulus
      << ". ";
    }
  } else {
    return false;
  }
  polynomial += constant;
  return
  output.assignValueWithContext(
    calculator, polynomial, input[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::
addPolynomialModuloIntegerToPolynomialModuloInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "addPolynomialModuloIntegerToPolynomialModuloInteger"
  );
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (
    !input.mergeContextsMyArumentsAndConvertThem<Polynomial<ElementZmodP> >(
      inputContextsMerged, &calculator.comments
    )
  ) {
    return false;
  }
  Polynomial<ElementZmodP> left =
  inputContextsMerged[1].getValue<Polynomial<ElementZmodP> >();
  Polynomial<ElementZmodP> right =
  inputContextsMerged[2].getValue<Polynomial<ElementZmodP> >();
  if (!left.isEqualToZero() && !right.isEqualToZero()) {
    if (
      left.coefficients[0].modulus != right.coefficients[0].modulus
    ) {
      return
      calculator
      << "Attempt to add polynomials with different moduli. ";
    }
  }
  left += right;
  return
  output.assignValueWithContext(
    calculator, left, inputContextsMerged[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::
dividePolynomialModuloIntegerByPolynomialModuloInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "dividePolynomialModuloIntegerByPolynomialModuloInteger"
  );
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (
    !input.mergeContextsMyArumentsAndConvertThem<Polynomial<ElementZmodP> >(
      inputContextsMerged, &calculator.comments
    )
  ) {
    return false;
  }
  Polynomial<ElementZmodP> left =
  inputContextsMerged[1].getValue<Polynomial<ElementZmodP> >();
  Polynomial<ElementZmodP> right =
  inputContextsMerged[2].getValue<Polynomial<ElementZmodP> >();
  if (right.isEqualToZero()) {
    return output.assignError(calculator, "Division by zero.");
  }
  LargeIntegerUnsigned modulus = right.coefficients[0].modulus;
  if (!left.isEqualToZero()) {
    if (left.coefficients[0].modulus != modulus) {
      return
      calculator
      << "Attempt to multiply polynomials with different moduli. ";
    }
  }
  if (modulus > ElementZmodP::maximumModulusForUserFacingPolynomialDivision) {
    return
    calculator
    << "Division of modular polynomials not "
    << "allowed for modulus larger than "
    << ElementZmodP::maximumModulusForUserFacingPolynomialDivision
    << ". ";
  }
  if (!modulus.isPossiblyPrime(0, true, &calculator.comments)) {
    return
    calculator
    << "Polynomial division is currently only implemented over prime moduli. ";
  }
  RationalFraction<ElementZmodP> result;
  result = left;
  result /= right;
  return
  output.assignValueWithContext(
    calculator, result, inputContextsMerged[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::multiplyPolynomialModPByPolynomialModP(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::multiplyPolynomialModPByPolynomialModP"
  );
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (
    !input.mergeContextsMyArumentsAndConvertThem<Polynomial<ElementZmodP> >(
      inputContextsMerged, &calculator.comments
    )
  ) {
    return false;
  }
  Polynomial<ElementZmodP> left =
  inputContextsMerged[1].getValue<Polynomial<ElementZmodP> >();
  Polynomial<ElementZmodP> right =
  inputContextsMerged[2].getValue<Polynomial<ElementZmodP> >();
  if (!left.isEqualToZero() && !right.isEqualToZero()) {
    if (
      left.coefficients[0].modulus != right.coefficients[0].modulus
    ) {
      return
      calculator
      << "Attempt to multiply polynomials with different moduli. ";
    }
  }
  left *= right;
  return
  output.assignValueWithContext(
    calculator, left, inputContextsMerged[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::
multiplyPolynomialModPolynomialModPToPolynomialModPolynomialModP(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "multiplyPolynomialModPolynomialModPToPolynomialModPolynomialModP"
  );
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (
    !input.mergeContextsMyArumentsAndConvertThem<
      PolynomialModuloPolynomial<ElementZmodP>
    >(inputContextsMerged, &calculator.comments)
  ) {
    return false;
  }
  PolynomialModuloPolynomial<ElementZmodP> left =
  inputContextsMerged[1].getValue<PolynomialModuloPolynomial<ElementZmodP> >();
  PolynomialModuloPolynomial<ElementZmodP> right =
  inputContextsMerged[2].getValue<PolynomialModuloPolynomial<ElementZmodP> >();
  if (left.modulusContainer != right.modulusContainer) {
    return
    calculator
    << "Attempt to multiply polynomial quotients with different moduli. ";
  }
  left *= right;
  return
  output.assignValueWithContext(
    calculator, left, inputContextsMerged[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::addPlotToPlot(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::addPlotToPlot");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Plot leftPlot, rightPlot;
  if (!input[1].isOfType<Plot>(&leftPlot)) {
    return false;
  }
  if (!input[2].isOfType<Plot>(&rightPlot)) {
    return false;
  }
  if (
    leftPlot.dimension != - 1 &&
    rightPlot.dimension != - 1 &&
    leftPlot.dimension != rightPlot.dimension
  ) {
    return false;
  }
  leftPlot += rightPlot;
  return output.assignValue(calculator, leftPlot);
}

bool CalculatorFunctionsBinaryOps::powerPolynomialBySmallInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::powerPolynomialBySmallInteger");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Polynomial<Rational> base;
  int power = 0;
  if (
    !input[1].isOfType(&base) || !input[2].isSmallInteger(&power)
  ) {
    return false;
  }
  if (base.isEqualToZero() && power <= 0) {
    return
    output.assignError(
      calculator, "Division by zero: trying to raise 0 to negative power. "
    );
  }
  if (power < 0) {
    if (base.size() == 1) {
      Polynomial<Rational> outputPoly;
      MonomialPolynomial monomial = base[0];
      Rational coefficient = base.coefficients[0];
      coefficient.raiseToPower(power);
      monomial.raiseToPower(power);
      outputPoly.makeZero();
      outputPoly.addMonomial(monomial, coefficient);
      return
      output.assignValueWithContext(
        calculator, outputPoly, input[1].getContext()
      );
    }
    base.raiseToPower(- power, 1);
    RationalFraction<Rational> rationalFraction;
    rationalFraction = base;
    rationalFraction.invert();
    return
    output.assignValueWithContext(
      calculator, rationalFraction, input[1].getContext()
    );
  }
  base.raiseToPower(power, 1);
  return
  output.assignValueWithContext(
    calculator, base, input[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::powerMatrixNumbersByLargeIntegerIfPossible(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::powerMatrixNumbersByLargeIntegerIfPossible"
  );
  if (!input.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  if (!input[1].isMatrix()) {
    return false;
  }
  LargeInteger largePower;
  if (!input[2].isInteger(&largePower)) {
    return false;
  }
  const Expression& matrixE = input[1];
  Matrix<Rational> baseRat;
  if (
    CalculatorConversions::functionGetMatrixNoComputation(
      calculator, matrixE, baseRat
    )
  ) {
    if (!baseRat.isSquare() || baseRat.numberOfColumns == 0) {
      std::stringstream errorStream;
      errorStream
      << "Exponentiating non-square matrices "
      << "or matrices with zero rows is not allowed. "
      << "Your matrix, "
      << baseRat.toString()
      << " is not square. ";
      return output.assignError(calculator, errorStream.str());
    }
    Rational determinant = baseRat.getDeterminant();
    if (largePower <= 0) {
      if (determinant == 0) {
        return
        output.assignError(
          calculator,
          "Division by zero: trying to raise 0 to negative power. "
        );
      }
    }
    if (determinant != 0 && determinant != - 1 && determinant != 1) {
      return calculator << "Matrix power too large.";
    }
    if (largePower < 0) {
      baseRat.invert();
      largePower *= - 1;
    }
    Matrix<Rational> idMat;
    idMat.makeIdentityMatrix(
      baseRat.numberOfRows, Rational::one(), Rational::zero()
    );
    MathRoutines::raiseToPower(baseRat, largePower, idMat);
    return output.makeMatrix(calculator, baseRat);
  }
  Matrix<AlgebraicNumber> baseAlg;
  if (
    CalculatorConversions::functionGetMatrixNoComputation(
      calculator, matrixE, baseAlg
    )
  ) {
    if (!baseAlg.isSquare() || baseAlg.numberOfColumns == 0) {
      std::stringstream errorStream;
      errorStream
      << "Exponentiating non-square matrices "
      << "or matrices with zero rows is not allowed. "
      << "Your matrix, "
      << baseAlg.toString()
      << " is not square. ";
      return output.assignError(calculator, errorStream.str());
    }
    AlgebraicNumber determinant = baseAlg.getDeterminant();
    if (largePower <= 0) {
      if (determinant == 0) {
        return
        output.assignError(
          calculator,
          "Division by zero: trying to raise 0 to negative power. "
        );
      }
    }
    if (determinant != 0 && determinant != - 1 && determinant != 1) {
      return calculator << "Matrix power too large.";
    }
    if (largePower < 0) {
      baseAlg.invert();
      largePower *= - 1;
    }
    Matrix<AlgebraicNumber> idMat;
    AlgebraicNumber one = calculator.objectContainer.algebraicClosure.one();
    AlgebraicNumber zero = calculator.objectContainer.algebraicClosure.zero();
    idMat.makeIdentityMatrix(baseAlg.numberOfRows, one, zero);
    MathRoutines::raiseToPower(baseAlg, largePower, idMat);
    return output.makeMatrix(calculator, baseAlg);
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::powerMatrixBuiltInBySmallInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::powerMatrixBuiltInBySmallInteger"
  );
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  const Expression& matrixE = input[1];
  if (!matrixE.isMatrix()) {
    return false;
  }
  int power = 0;
  bool powerIsSmall = input[2].isSmallInteger(&power);
  if (!powerIsSmall) {
    return false;
  }
  Matrix<Rational> baseRational;
  if (
    CalculatorConversions::functionGetMatrixNoComputation(
      calculator, matrixE, baseRational
    )
  ) {
    if (!baseRational.isSquare() || baseRational.numberOfColumns == 0) {
      std::stringstream errorStream;
      errorStream
      << "Exponentiating non-square matrices "
      << "or matrices with zero rows is not allowed. "
      << "Your matrix, "
      << baseRational.toString()
      << " is not square. ";
      return output.assignError(calculator, errorStream.str());
    }
    if (power <= 0) {
      if (baseRational.getDeterminant() == 0) {
        return
        output.assignError(
          calculator,
          "Division by zero: trying to raise 0 to negative power. "
        );
      }
    }
    if (power < 0) {
      baseRational.invert();
      power *= - 1;
    }
    Matrix<Rational> idMat;
    idMat.makeIdentityMatrix(
      baseRational.numberOfRows, Rational::one(), Rational::zero()
    );
    MathRoutines::raiseToPower(baseRational, power, idMat);
    return output.makeMatrix(calculator, baseRational);
  }
  Matrix<AlgebraicNumber> baseAlgebraic;
  AlgebraicNumber one = calculator.objectContainer.algebraicClosure.one();
  AlgebraicNumber zero = calculator.objectContainer.algebraicClosure.zero();
  if (
    CalculatorConversions::functionGetMatrixNoComputation(
      calculator, matrixE, baseAlgebraic
    )
  ) {
    if (!baseAlgebraic.isSquare() || baseAlgebraic.numberOfColumns == 0) {
      return
      output.assignError(
        calculator,
        "Exponentiating non-square matrices "
        "or matrices with zero rows is not allowed."
      );
    }
    if (power <= 0) {
      if (baseAlgebraic.getDeterminant() == 0) {
        return
        output.assignError(
          calculator,
          "Division by zero: trying to raise 0 to negative power. "
        );
      }
    }
    if (power < 0) {
      baseAlgebraic.invert();
      power *= - 1;
    }
    Matrix<AlgebraicNumber> idMat;
    idMat.makeIdentityMatrix(baseAlgebraic.numberOfRows, one, zero);
    MathRoutines::raiseToPower(baseAlgebraic, power, idMat);
    return output.makeMatrix(calculator, baseAlgebraic);
  }
  Matrix<RationalFraction<Rational> > baseRationalFunctionCoefficients;
  ExpressionContext context(calculator);
  if (
    CalculatorConversions::functionGetMatrix(
      calculator,
      matrixE,
      baseRationalFunctionCoefficients,
      false,
      &context
    )
  ) {
    if (
      !baseRationalFunctionCoefficients.isSquare() ||
      baseRationalFunctionCoefficients.numberOfColumns == 0
    ) {
      return
      output.assignError(
        calculator,
        "Exponentiating non-square matrices or matrices "
        "with zero rows is not allowed."
      );
    }
    int maximumDimensions = 4;
    if (power <= 0) {
      if (
        context.numberOfVariables() > 1 &&
        baseRationalFunctionCoefficients.numberOfColumns > maximumDimensions
      ) {
        return
        calculator
        << "Raising matrices of rational functions "
        << "with 2 or more variables and more than "
        << maximumDimensions
        << " rows to negative power not implemented yet. "
        << "Your matrix has "
        << context.numberOfVariables()
        << " variables.";
      }
    }
    if (power <= 0) {
      if (baseRationalFunctionCoefficients.getDeterminant() == 0) {
        return
        output.assignError(
          calculator,
          "Division by zero: request to invert matrix with zero determinant. "
        );
      }
    }
    if (power < 0) {
      power *= - 1;
      if (!baseRationalFunctionCoefficients.invert()) {
        return
        calculator
        << "Unexpected failure to invert your matrix: "
        << baseRationalFunctionCoefficients.toString();
      }
    }
    Matrix<RationalFraction<Rational> > identityMatrix;
    identityMatrix.makeIdentityMatrix(
      baseRationalFunctionCoefficients.numberOfRows,
      RationalFraction<Rational>::oneRational(),
      RationalFraction<Rational>::zeroRational()
    );
    MathRoutines::raiseToPower(
      baseRationalFunctionCoefficients, power, identityMatrix
    );
    return
    output.makeMatrix(
      calculator, baseRationalFunctionCoefficients, &context
    );
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::powerAlgebraicNumberPolynomialBySmallInteger
(Calculator& calculator, const Expression& input, Expression& output) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "powerAlgebraicNumberPolynomialBySmallInteger"
  );
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Polynomial<AlgebraicNumber> base;
  int power = 0;
  if (
    !input[1].isOfType(&base) || !input[2].isSmallInteger(&power)
  ) {
    return false;
  }
  if (power < 0) {
    return false;
  }
  if (base.isEqualToZero() && power <= 0) {
    return
    output.assignError(
      calculator, "Division by zero: trying to raise 0 to negative power. "
    );
  }
  base.raiseToPower(power, 1);
  return
  output.assignValueWithContext(
    calculator, base, input[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::powerRationalFractionBySmallInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::powerRationalFractionBySmallInteger"
  );
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  RationalFraction<Rational> base;
  int power = 0;
  if (
    !input[1].isOfType(&base) || !input[2].isSmallInteger(&power)
  ) {
    return false;
  }
  if (power < 0) {
    base.invert();
    power = - power;
  }
  if (base.isEqualToZero() && power <= 0) {
    return
    output.assignError(
      calculator, "Division by zero: trying to raise 0 to negative power. "
    );
  }
  base.raiseToPower(power);
  return
  output.assignValueWithContext(
    calculator, base, input[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::powerPolynomialModuloIntegerBySmallInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::powerPolynomialModuloIntegerBySmallInteger"
  );
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Polynomial<ElementZmodP> base;
  int power = 0;
  if (
    !input[1].isOfType(&base) || !input[2].isSmallInteger(&power)
  ) {
    return false;
  }
  if (power < 0) {
    return false;
  }
  if (base.isEqualToZero()) {
    if (power < 0) {
      return
      output.assignError(
        calculator, "Division by zero: trying to raise 0 to negative power. "
      );
    }
    if (power == 0) {
      return false;
    }
    return
    output.assignValueWithContext(
      calculator, base, input[1].getContext()
    );
  }
  base.raiseToPower(power, base.coefficients[0].one());
  return
  output.assignValueWithContext(
    calculator, base, input[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::
powerPolynomialModPModuloPolynomialModPBySmallInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "powerPolynomialModPModuloPolynomialModPBySmallInteger"
  );
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  PolynomialModuloPolynomial<ElementZmodP> base;
  LargeIntegerUnsigned power = 0;
  if (
    !input[1].isOfType(&base) || !input[2].isIntegerNonNegative(&power)
  ) {
    return false;
  }
  if (base.isEqualToZero()) {
    if (power < 0) {
      return
      output.assignError(
        calculator, "Division by zero: trying to raise 0 to negative power. "
      );
    }
    if (power == 0) {
      return false;
    }
    return
    output.assignValueWithContext(
      calculator, base, input[1].getContext()
    );
  }
  MathRoutines::raiseToPower(base, power, base.one());
  return
  output.assignValueWithContext(
    calculator, base, input[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::radicalAlgebraicNumberPositiveDefault(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::radicalAlgebraicNumberPositiveDefault"
  );
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  AlgebraicNumber base;
  if (!input[1].isOfType(&base)) {
    return false;
  }
  if (base.isRational()) {
    // We let other rules handle the case of a rational argument.
    return false;
  }
  Rational power;
  if (!input[2].isOfType(&power)) {
    // exponent is not rational.
    return false;
  }
  LargeInteger radicalLarge = power.getDenominator();
  LargeInteger powerIntegral = power.getNumerator();
  int radical = 0;
  int powerSmallInteger = 0;
  if (
    !radicalLarge.isIntegerFittingInInt(&radical) ||
    !powerIntegral.isIntegerFittingInInt(&powerSmallInteger)
  ) {
    // radical is too large.
    return false;
  }
  if (radical <= 1) {
    // not really a radical
    return false;
  }
  int maximumRadicalAllowed = 6;
  if (radical > maximumRadicalAllowed) {
    // max 6th radical allowed.
    calculator << "Not attempting to reduce radical: larger than maximum of "
    << maximumRadicalAllowed
    << ". ";
    return false;
  }
  if (powerIntegral > LargeIntegerUnsigned::squareRootOfCarryOverBound) {
    // power too large
    return false;
  }
  AlgebraicClosureRationals fieldCopy;
  fieldCopy = calculator.objectContainer.algebraicClosure;
  AlgebraicNumber baseCopy;
  baseCopy = base;
  baseCopy.owner = &fieldCopy;
  if (!baseCopy.radicalMeDefault(radical, &calculator.comments)) {
    return false;
  }
  if (
    fieldCopy.basisInjections.size !=
    calculator.objectContainer.algebraicClosure.basisInjections.size
  ) {
    // the radical does not below to the base field;
    return false;
  }
  baseCopy.owner = &calculator.objectContainer.algebraicClosure;
  MathRoutines::raiseToPower(
    baseCopy, powerIntegral, baseCopy.owner->one()
  );
  return output.assignValue(calculator, baseCopy);
}

bool CalculatorFunctionsBinaryOps::powerAlgebraicNumberBySmallInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::powerAlgebraicNumberBySmallInteger"
  );
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  AlgebraicNumber base;
  if (!input[1].isOfType(&base)) {
    return false;
  }
  Rational powerRational, baseRat;
  if (input[2].isRational(&powerRational)) {
    if (powerRational.getDenominator() == 2) {
      if (base.isRational(&baseRat)) {
        if (
          base.assignRationalQuadraticRadical(
            baseRat,
            calculator.objectContainer.algebraicClosure,
            &calculator.comments
          )
        ) {
          base.checkConsistency();
          output = input;
          Expression newPower, newBase;
          newPower.assignValue(calculator, powerRational* 2);
          newBase.assignValue(calculator, base);
          return
          output.makeXOX(
            calculator, calculator.opPower(), newBase, newPower
          );
        }
      }
    }
  }
  int power = 0;
  if (!input[2].isSmallInteger(&power)) {
    return false;
  }
  if (base.isEqualToZero() && power < 0) {
    return
    output.assignError(
      calculator, "Division by zero: trying to raise 0 to negative power. "
    );
  }
  if (base.isEqualToZero() && power == 0) {
    return output.assignValue(calculator, 1);
  }
  if (power < 0) {
    power *= - 1;
    base.invert();
  }
  MathRoutines::raiseToPower(base, power, AlgebraicNumber(1));
  return
  output.assignValueWithContext(
    calculator, base, input[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::powerElementWeylAlgebraBySmallInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::powerEWABySmallInteger");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  ElementWeylAlgebra<Rational> base;
  int power = 0;
  if (!input[1].isOfType(&base)) {
    return false;
  }
  bool mustCheckForRationalPower = false;
  mustCheckForRationalPower = !input[2].isSmallInteger(&power);
  if (mustCheckForRationalPower || power < 0) {
    Rational powerRational;
    if (!input[2].isOfType<Rational>(&powerRational)) {
      return false;
    }
    bool isMon = true;
    if (base.size() != 1) {
      isMon = false;
    } else if (base.coefficients[0] != 1) {
      isMon = false;
    }
    if (!isMon) {
      return
      calculator
      << "<hr>Failed to raise "
      << base.toString()
      << " to power "
      << powerRational.toString()
      << ": the exponent is not a "
      << "small integer and the base is not a coefficient one monomial. ";
    }
    ElementWeylAlgebra<Rational> finalOutput;
    MonomialWeylAlgebra monomial = base[0];
    monomial.polynomialPart.raiseToPower(powerRational);
    monomial.differentialPart.raiseToPower(powerRational);
    for (
      int i = 0; i < monomial.polynomialPart.minimalNumberOfVariables(); i ++
    ) {
      if (
        monomial.polynomialPart(i) != 0 && monomial.differentialPart(i) != 0
      ) {
        return
        calculator
        << "<hr>Failed to raise "
        << base.toString()
        << " to power "
        << powerRational.toString()
        << ": the exponent is not a "
        << "small integer, the base is a monomial, however the monomial "
        << "contains derivative and polynomial "
        << "with respect to the same variable. ";
      }
    }
    finalOutput.makeZero();
    finalOutput.addMonomial(monomial, 1);
    return
    output.assignValueWithContext(
      calculator, finalOutput, input[1].getContext()
    );
  }
  if (base.isEqualToZero()) {
    if (power < 0) {
      return
      output.assignError(
        calculator, "Division by zero: trying to raise 0 to negative power. "
      );
    }
    if (power == 0) {
      return output.assignValue(calculator, 1);
    }
  }
  base.raiseToPower(power);
  return
  output.assignValueWithContext(
    calculator, base, input[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::powerRationalByInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::powerRationalByInteger");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational base, exp;
  if (!input[1].isRational(&base)) {
    return false;
  }
  if (base.isEqualToOne()) {
    return output.assignValue(calculator, 1);
  }
  if (!input[2].isRational(&exp)) {
    return false;
  }
  int power;
  if (!exp.isSmallInteger(&power)) {
    return false;
  }
  if (base == 0 && power == 0) {
    return output.assignValue(calculator, 1);
  }
  if (base == 0 && power < 0) {
    return
    output.assignError(
      calculator,
      "Division by zero: trying to raise 0 to negative or zero power. "
    );
  }
  base.raiseToPower(power);
  return output.assignValue(calculator, base);
}

bool CalculatorFunctionsBinaryOps::
powerElementUniversalEnvelopingByRationalOrPolynomialOrRationalFraction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "powerElementUniversalEnvelopingByRationalOrPolynomialOrRationalFraction"
  );
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (
    !input.mergeContextsMyAruments(inputConverted, &calculator.comments)
  ) {
    return false;
  }
  ElementUniversalEnveloping<RationalFraction<Rational> >
  elementUniversalEnveloping;
  Expression copyExponent = inputConverted[2];
  Expression copyBase = inputConverted[1];
  if (
    !copyBase.isOfType<
      ElementUniversalEnveloping<RationalFraction<Rational> >
    >(&elementUniversalEnveloping)
  ) {
    return false;
  }
  if (!elementUniversalEnveloping.isPowerOfSingleGenerator()) {
    int tempPower;
    if (!copyExponent.isSmallInteger(&tempPower)) {
      return false;
    }
    elementUniversalEnveloping.raiseToPower(tempPower);
    return
    output.assignValueWithContext(
      calculator, elementUniversalEnveloping, copyBase.getContext()
    );
  }
  WithContext<RationalFraction<Rational> > exponentConverted;
  if (
    !CalculatorConversions::convertWithoutComputation(
      calculator, copyExponent, exponentConverted
    )
  ) {
    return false;
  }
  MonomialUniversalEnveloping<RationalFraction<Rational> > monomial;
  monomial = elementUniversalEnveloping[0];
  monomial.powers[0] *= exponentConverted.content;
  ElementUniversalEnveloping<RationalFraction<Rational> > outputUE;
  outputUE.makeZero(*elementUniversalEnveloping.owner);
  outputUE.addMonomial(monomial, 1);
  return
  output.assignValueWithContext(
    calculator, outputUE, copyBase.getContext()
  );
}

bool CalculatorFunctionsBinaryOps::powerMatrixExpressionsBySmallInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::powerMatrixExpressionsBySmallInteger"
  );
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  if (!input[1].isMatrix()) {
    return false;
  }
  int power = 0;
  if (!input[2].isSmallInteger(&power)) {
    return false;
  }
  if (power <= 0) {
    return false;
  }
  Matrix<Expression> matrix;
  if (!calculator.getMatrixExpressions(input[1], matrix)) {
    return false;
  }
  if (!matrix.isSquare()) {
    return
    output.assignError(
      calculator, "Attempting to raise non-square matrix to power"
    );
  }
  LargeInteger expectedNumberOfTerms;
  expectedNumberOfTerms = matrix.numberOfColumns;
  expectedNumberOfTerms.raiseToPower(power);
  if (expectedNumberOfTerms > 10000) {
    return
    calculator
    << "The expected number terms in the result of the exponentiation "
    << matrix.toString()
    << " to the power of "
    << power
    << " is approximately ("
    << matrix.numberOfColumns
    << ")^"
    << power
    << "="
    << expectedNumberOfTerms
    << ". I have been instructed to proceed only "
    << "if the expected number of terms is fewer than 10000. ";
  }
  Matrix<Expression> idMatE;
  idMatE.makeIdentityMatrix(
    matrix.numberOfRows,
    calculator.expressionOne(),
    calculator.expressionZero()
  );
  MathRoutines::raiseToPower(matrix, power, idMatE);
  return output.assignMatrixExpressions(matrix, calculator, true, true);
}

bool CalculatorFunctionsBinaryOps::powerRationalByRationalReducePrimeFactors(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::powerRationalByRationalReducePrimeFactors"
  );
  if (!input.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  Rational base, exponentWorking, exponentStarting;
  if (!input[1].isOfType<Rational>(&base)) {
    return false;
  }
  if (!input[2].isOfType<Rational>(&exponentStarting)) {
    return false;
  }
  if (base == 0) {
    if (exponentStarting < 0) {
      return
      output.assignError(
        calculator,
        "Division by zero while evaluating " + input.toString()
      );
    }
    return output.assignValue(calculator, 0);
  }
  if (!base.isInteger()) {
    if (base.getNumerator() == 1) {
      Expression denominatorBase, denominator;
      denominatorBase.assignValue(
        calculator, Rational(base.getDenominator())
      );
      denominator.makeXOX(
        calculator, calculator.opPower(), denominatorBase, input[2]
      );
      output = calculator.expressionOne() / denominator;
      return true;
    }
  }
  exponentWorking = exponentStarting;
  if (exponentWorking.getDenominator() == 2) {
    return false;
  }
  LargeIntegerUnsigned exponentDenominator = exponentWorking.getDenominator();
  LargeIntegerUnsigned exponentNumeratorNoSign =
  exponentWorking.getNumerator().value;
  List<LargeInteger> numeratorFactors, denominatorFactors;
  List<int> numeratorPowersInteger, denominatorPowersInteger;
  if (
    !base.getPrimeFactorsAbsoluteValue(
      numeratorFactors,
      numeratorPowersInteger,
      denominatorFactors,
      denominatorPowersInteger
    )
  ) {
    return false;
  }
  List<LargeIntegerUnsigned> numeratorPowers, denominatorPowers;
  numeratorPowers = numeratorPowersInteger;
  denominatorPowers = denominatorPowersInteger;
  for (int i = 0; i < numeratorFactors.size; i ++) {
    numeratorPowers[i] *= exponentNumeratorNoSign;
  }
  for (int i = 0; i < denominatorFactors.size; i ++) {
    denominatorPowers[i] *= exponentNumeratorNoSign;
  }
  exponentWorking /= exponentNumeratorNoSign;
  Rational outsideOfRadical = 1;
  LargeInteger currentInsidePower, currentOutsidePower, currentOutside;
  LargeIntegerUnsigned currentPower;
  int currentInsidePowerInteger = - 1, currentOutsidePowerInteger = - 1;
  for (int k = 0; k < 2; k ++) {
    List<LargeIntegerUnsigned>& currentPowers = (k == 0) ?
    numeratorPowers :
    denominatorPowers;
    List<LargeInteger>& currentFactors = (k == 0) ?
    numeratorFactors :
    denominatorFactors;
    for (int i = 0; i < currentFactors.size; i ++) {
      currentPower = currentPowers[i];
      currentInsidePower = currentPower % exponentDenominator;
      currentOutsidePower = currentPower / exponentDenominator;
      if (
        !currentInsidePower.isIntegerFittingInInt(&currentInsidePowerInteger)
      ) {
        return false;
      }
      if (
        !currentOutsidePower.isIntegerFittingInInt(
          &currentOutsidePowerInteger
        )
      ) {
        return false;
      }
      if (
        currentInsidePowerInteger > 1000 ||
        currentInsidePowerInteger < - 1000 ||
        currentOutsidePowerInteger > 1000 ||
        currentOutsidePowerInteger < - 1000
      ) {
        return false;
      }
      currentPowers[i] = static_cast<unsigned>(currentInsidePowerInteger);
      currentOutside = currentFactors[i];
      currentOutside.raiseToPower(currentOutsidePowerInteger);
      if (k == 0) {
        outsideOfRadical *= currentOutside;
      } else {
        outsideOfRadical /= currentOutside;
      }
    }
  }
  LargeIntegerUnsigned greatestCommonDivisor = 1;
  if (numeratorPowers.size > 0) {
    greatestCommonDivisor = numeratorPowers[0];
  } else if (denominatorPowers.size > 0) {
    greatestCommonDivisor = denominatorPowers[0];
  }
  for (int i = 0; i < numeratorPowers.size; i ++) {
    greatestCommonDivisor =
    MathRoutines::greatestCommonDivisor(
      greatestCommonDivisor, numeratorPowers[i]
    );
  }
  for (int i = 0; i < denominatorPowers.size; i ++) {
    greatestCommonDivisor =
    MathRoutines::greatestCommonDivisor(
      greatestCommonDivisor, denominatorPowers[i]
    );
  }
  if (greatestCommonDivisor > 0) {
    for (int i = 0; i < numeratorPowers.size; i ++) {
      numeratorPowers[i] /= greatestCommonDivisor;
    }
    for (int i = 0; i < denominatorPowers.size; i ++) {
      denominatorPowers[i] /= greatestCommonDivisor;
    }
    exponentWorking *= greatestCommonDivisor;
    exponentDenominator = exponentWorking.getDenominator();
  }
  Rational insideRadical = 1;
  LargeInteger
  currentContribution,
  currentNumerator = 1,
  currentDenominator = 1;
  int currentExponentSmallInteger = - 1;
  for (int i = 0; i < numeratorPowers.size; i ++) {
    currentContribution = numeratorFactors[i];
    if (
      !numeratorPowers[i].isIntegerFittingInInt(&currentExponentSmallInteger)
    ) {
      return false;
    }
    currentContribution.raiseToPower(currentExponentSmallInteger);
    currentNumerator *= currentContribution;
  }
  for (int i = 0; i < denominatorPowers.size; i ++) {
    currentContribution = denominatorFactors[i];
    if (
      !denominatorPowers[i].isIntegerFittingInInt(
        &currentExponentSmallInteger
      )
    ) {
      return false;
    }
    currentContribution.raiseToPower(currentExponentSmallInteger);
    currentDenominator *= currentContribution;
  }
  insideRadical = currentNumerator;
  insideRadical /= currentDenominator;
  if (base < 0) {
    exponentDenominator = exponentWorking.getDenominator();
    // <-just in case
    if (exponentDenominator % 2 == 1) {
      outsideOfRadical *= - 1;
    } else {
      insideRadical *= - 1;
    }
  }
  if (
    exponentStarting == exponentWorking &&
    outsideOfRadical == 1 &&
    base == insideRadical
  ) {
    return false;
  }
  Expression
  insideRadicalE,
  radicalExpression,
  radicalCoefficientExpression,
  exponentE;
  if (exponentWorking < 0) {
    outsideOfRadical.invert();
  }
  radicalCoefficientExpression.assignValue(calculator, outsideOfRadical);
  insideRadicalE.assignValue(calculator, insideRadical);
  exponentE.assignValue(calculator, exponentWorking);
  radicalExpression.makeXOX(
    calculator, calculator.opPower(), insideRadicalE, exponentE
  );
  return
  output.makeProduct(
    calculator, radicalCoefficientExpression, radicalExpression
  );
}

bool CalculatorFunctionsBinaryOps::powerDoubleOrRationalToDoubleOrRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::powerDoubleOrRationalToDoubleOrRational"
  );
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational base, exponent;
  double baseDouble, exponentDouble;
  if (input[1].isRational(&base)) {
    baseDouble = base.getDoubleValue();
  } else if (!input[1].isOfType(&baseDouble)) {
    return false;
  }
  if (input[2].isRational(&exponent)) {
    exponentDouble = exponent.getDoubleValue();
  } else if (!input[2].isOfType(&exponentDouble)) {
    return false;
  }
  if (baseDouble < 0) {
    if (!input[2].isRational()) {
      return false;
    }
    int power = 0;
    if (exponent.isEven()) {
      if (!exponent.isSmallInteger(&power)) {
        return false;
      }
      return
      output.assignValue(
        calculator, FloatingPoint::power(- baseDouble, power)
      );
    }
    baseDouble *= - 1;
    return
    output.assignValue(
      calculator, - FloatingPoint::power(baseDouble, exponentDouble)
    );
  }
  if (baseDouble == 0.0) {
    if (exponentDouble > 0) {
      return output.assignValue<double>(calculator, 0);
    }
  }
  return
  output.assignValue(
    calculator, FloatingPoint::power(baseDouble, exponentDouble)
  );
}

bool CalculatorFunctionsBinaryOps::multiplyDoubleOrRationalByDoubleOrRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::multiplyDoubleOrRationalByDoubleOrRational"
  );
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational leftR, rightR;
  double leftD, rightD;
  if (input[1].isOfType(&leftR)) {
    leftD = leftR.getDoubleValue();
  } else if (!input[1].isOfType(&leftD)) {
    return false;
  }
  if (input[2].isOfType(&rightR)) {
    rightD = rightR.getDoubleValue();
  } else if (!input[2].isOfType(&rightD)) {
    return false;
  }
  return output.assignValue(calculator, leftD* rightD);
}

bool CalculatorFunctionsBinaryOps::addDoubleOrRationalToDoubleOrRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::addDoubleOrRationalToDoubleOrRational"
  );
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational leftR, rightR;
  double leftD, rightD;
  if (input[1].isOfType(&leftR)) {
    leftD = leftR.getDoubleValue();
  } else if (!input[1].isOfType(&leftD)) {
    return false;
  }
  if (input[2].isOfType(&rightR)) {
    rightD = rightR.getDoubleValue();
  } else if (!input[2].isOfType(&rightD)) {
    return false;
  }
  return output.assignValue(calculator, leftD + rightD);
}

bool CalculatorFunctionsBinaryOps::multiplyCharSSLieAlgByCharSSLieAlg(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::multiplyCharSSLieAlgByCharSSLieAlg"
  );
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  CharacterSemisimpleLieAlgebraModule<Rational> leftC, rightC;
  if (!input[1].isOfType(&leftC)) {
    return false;
  }
  if (!input[2].isOfType(&rightC)) {
    return false;
  }
  if (leftC.getOwner() != rightC.getOwner()) {
    return
    calculator
    << "You asked me to multiply characters over "
    << "different semisimple Lie algebras. Could this be a typo?";
  }
  std::string successString = (leftC *= rightC);
  if (successString != "") {
    return
    calculator
    << "I tried to multiply character "
    << leftC.toString()
    << " by "
    << rightC.toString()
    << " but I failed with the following message: "
    << successString;
  }
  return output.assignValue(calculator, leftC);
}

bool CalculatorFunctionsBinaryOps::multiplyAnyScalarByMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::multiplyAnyScalarByMatrix");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  const Expression& scalarExpression = input[1];
  if (!scalarExpression.isBuiltInScalar()) {
    return false;
  }
  const Expression& matrixE = input[2];
  if (!matrixE.isMatrix()) {
    return false;
  }
  Matrix<Expression> matrix;
  if (!calculator.getMatrixExpressions(matrixE, matrix)) {
    return false;
  }
  for (int i = 0; i < matrix.numberOfRows; i ++) {
    for (int j = 0; j < matrix.numberOfColumns; j ++) {
      matrix(i, j) = scalarExpression * matrix(i, j);
    }
  }
  return output.assignMatrixExpressions(matrix, calculator, false, true);
}

bool CalculatorFunctionsBinaryOps::multiplyAnyScalarBySequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::multiplyAnyScalarBySequence");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (!input[1].isBuiltInScalar()) {
    return false;
  }
  if (!input[2].isSequenceNElements()) {
    return false;
  }
  output.reset(calculator);
  output.setExpectedSize(input[2].size());
  output.addChildAtomOnTop(calculator.opSequence());
  Expression product;
  for (int i = 1; i < input[2].size(); i ++) {
    product.makeProduct(calculator, input[1], input[2][i]);
    output.addChildOnTop(product);
  }
  return true;
}

bool CalculatorFunctionsBinaryOps::divideSequenceByScalar(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::divideSequenceByScalar");
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (!input[1].isSequenceNElements()) {
    return false;
  }
  if (!input[2].evaluatesToScalar()) {
    return false;
  }
  Expression result;
  result.reset(calculator);
  result.setExpectedSize(input[1].size());
  result.addChildAtomOnTop(calculator.opSequence());
  for (int i = 1; i < input[1].size(); i ++) {
    Expression quotient;
    quotient.makeQuotientReduce(calculator, input[1][i], input[2]);
    result.addChildOnTop(quotient);
  }
  output = result;
  return true;
}

bool CalculatorFunctionsBinaryOps::containsMatrixOrSequence(
  const Expression& input
) {
  if (input.isSequenceNElements() || input.isMatrix()) {
    return true;
  }
  if (input.owner == nullptr) {
    return false;
  }
  Calculator::GlobalCache& cache =
  input.owner->globalCache.getValueCreateEmpty(input);
  if (!cache.flagContainsMatrixOrSequence.isZeroPointer()) {
    return cache.flagContainsMatrixOrSequence.getElement();
  }
  bool result = false;
  for (int i = 1; i < input.size(); i ++) {
    if (
      CalculatorFunctionsBinaryOps::containsMatrixOrSequence(input[i])
    ) {
      result = true;
      break;
    }
  }
  cache.flagContainsMatrixOrSequence.getElement() = result;
  return result;
}

bool CalculatorFunctionsBinaryOps::isScalarLike(const Expression& input) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::isScalarLike");
  return !containsMatrixOrSequence(input);
}

bool CalculatorFunctionsBinaryOps::multiplySequenceByScalarLike(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::multiplySequenceByScalarLike");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  const Expression& left = input[1];
  const Expression& right = input[2];
  if (!left.isSequenceNElements()) {
    return false;
  }
  if (!CalculatorFunctionsBinaryOps::isScalarLike(right)) {
    return false;
  }
  List<Expression> newSequence;
  for (int i = 1; i < left.size(); i ++) {
    newSequence.addOnTop(left[i] * right);
  }
  return output.makeSequence(calculator, &newSequence);
}

bool CalculatorFunctionsBinaryOps::multiplyMatrixBySequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::multiplyMatrixBySequence");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  int numberOfRows = - 1;
  int numberOfColumns = - 1;
  const Expression& left = input[1];
  const Expression& right = input[2];
  if (
    !left.isMatrix(&numberOfRows, &numberOfColumns) ||
    !right.isSequenceNElements()
  ) {
    return false;
  }
  if (numberOfColumns != 1 || numberOfRows < 2) {
    return false;
  }
  Matrix<Expression> rightAsMatrix;
  Matrix<Expression> leftAsMatrix;
  if (
    !calculator.getMatrixExpressions(right, rightAsMatrix, 1, numberOfRows)
  ) {
    return false;
  }
  if (
    !calculator.getMatrixExpressions(
      left, leftAsMatrix, numberOfRows, numberOfColumns
    )
  ) {
    return false;
  }
  return
  CalculatorFunctionsBinaryOps::makeMatrixProduct(
    calculator, leftAsMatrix, rightAsMatrix, output
  );
}

bool CalculatorFunctionsBinaryOps::makeMatrixProduct(
  Calculator& calculator,
  const Matrix<Expression>& left,
  const Matrix<Expression>& right,
  Expression& output
) {
  Matrix<Expression> outputMatrix;
  if (left.numberOfColumns != right.numberOfRows) {
    std::stringstream out;
    out
    << "Error: multiplication of matrix with "
    << left.numberOfColumns
    << " columns by a matrix with "
    << right.numberOfRows
    << " rows.";
    return output.assignError(calculator, out.str());
  }
  outputMatrix.initialize(left.numberOfRows, right.numberOfColumns);
  Expression leftSummand;
  Expression rightSummand;
  for (int i = 0; i < left.numberOfRows; i ++) {
    for (int j = 0; j < right.numberOfColumns; j ++) {
      for (int k = 0; k < left.numberOfColumns; k ++) {
        if (k == 0) {
          outputMatrix(i, j).makeProduct(
            calculator, left(i, k), right(k, j)
          );
        } else {
          leftSummand = outputMatrix(i, j);
          rightSummand.makeProduct(
            calculator, left(i, k), right(k, j)
          );
          outputMatrix(i, j).makeXOX(
            calculator, calculator.opPlus(), leftSummand, rightSummand
          );
        }
      }
    }
  }
  return
  output.assignMatrixExpressions(outputMatrix, calculator, true, true);
}

bool CalculatorFunctionsBinaryOps::multiplyMatrixByMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::multiplyMatrixByMatrix");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  int totalRowsLeft = - 1;
  int totalColumnsLeft = - 1;
  int totalRowsRight = - 1;
  int totalColumnsRight = - 1;
  const Expression& left = input[1];
  const Expression& right = input[2];
  if (
    !left.isMatrix(&totalRowsLeft, &totalColumnsLeft) ||
    !input[2].isMatrix(&totalRowsRight, &totalColumnsRight)
  ) {
    return false;
  }
  if (totalColumnsLeft != totalRowsRight) {
    return
    calculator
    << "WARNING: I encountered a product of a matrix with "
    << totalColumnsLeft
    << " columns by a matrix with "
    << totalRowsRight
    << " rows. ";
  }
  if (
    left.isMatrixOfType<Rational>() && right.isMatrixOfType<Rational>()
  ) {
    return
    CalculatorFunctionsBinaryOps::
    multiplyMatrixRationalOrRationalByMatrixRational(
      calculator, input, output
    );
  }
  bool invokeAlgMatMultiplication = (
    left.isMatrixOfType<Rational>() && right.isMatrixOfType<AlgebraicNumber>()
  ) || (
    left.isMatrixOfType<AlgebraicNumber>() && right.isMatrixOfType<Rational>()
  ) || (
    left.isMatrixOfType<AlgebraicNumber>() &&
    right.isMatrixOfType<AlgebraicNumber>()
  );
  if (invokeAlgMatMultiplication) {
    return
    CalculatorFunctionsBinaryOps::multiplyMatRatOrMatAlgByMatRatOrMatAlg(
      calculator, input, output
    );
  }
  bool invokeRFMultiplication = (
    left.isMatrixOfType<RationalFraction<Rational> >() &&
    right.isMatrixOfType<Rational>()
  ) || (
    left.isMatrixOfType<Rational>() &&
    right.isMatrixOfType<RationalFraction<Rational> >()
  ) || (
    left.isMatrixOfType<RationalFraction<Rational> >() &&
    right.isMatrixOfType<RationalFraction<Rational> >()
  );
  if (invokeRFMultiplication) {
    return
    CalculatorFunctionsBinaryOps::
    multiplyMatrixRationalFractionOrRationalFractionByMatrixRationalFraction(
      calculator, input, output
    );
  }
  Matrix<Expression> leftMatrix;
  Matrix<Expression> rightMatrix;
  if (!calculator.getMatrixExpressions(left, leftMatrix)) {
    return false;
  }
  if (
    !calculator.getMatrixExpressions(
      input[2], rightMatrix, leftMatrix.numberOfColumns
    )
  ) {
    return false;
  }
  if (leftMatrix.numberOfColumns != rightMatrix.numberOfRows) {
    return false;
  }
  CalculatorFunctionsBinaryOps::makeMatrixProduct(
    calculator, leftMatrix, rightMatrix, output
  );
  return true;
}

bool CalculatorFunctionsBinaryOps::multiplySequenceByMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::multiplySequenceByMatrix");
  if (!input.startsWith(calculator.opTimes())) {
    return false;
  }
  int matrixRows = - 1;
  int matrixColumns = - 1;
  if (
    !input[1].isSequenceNElements() ||
    !input[2].isMatrix(&matrixRows, &matrixColumns)
  ) {
    return false;
  }
  if (input[1].size() - 1 != matrixRows) {
    return false;
  }
  List<Expression> result;
  result.setSize(matrixColumns);
  for (int i = 0; i < result.size; i ++) {
    for (int j = 0; j < matrixRows; j ++) {
      if (j == 0) {
        result[i] = input[1][j + 1] * input[2][j + 1][i + 1];
      } else {
        result[i] += input[1][j + 1] * input[2][j + 1][i + 1];
      }
    }
  }
  return output.makeSequence(calculator, &result);
}

bool CalculatorFunctionsBinaryOps::tensorMatrixByMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input.startsWith(calculator.opTensor(), 3)) {
    return false;
  }
  STACK_TRACE("CalculatorFunctionsBinaryOps::tensorMatrixByMatrix");
  const Expression& leftExpression = input[1];
  const Expression& rightExpression = input[2];
  if ((
      !rightExpression.isMatrix() && !rightExpression.isSequenceNElements()
    ) || (
      !leftExpression.isMatrix() && !leftExpression.isSequenceNElements()
    )
  ) {
    return false;
  }
  Matrix<Rational> leftMatrixRational;
  Matrix<Rational> rightMatrixRational;
  if (
    CalculatorConversions::functionGetMatrixNoComputation(
      calculator, leftExpression, leftMatrixRational
    ) &&
    CalculatorConversions::functionGetMatrixNoComputation(
      calculator, rightExpression, rightMatrixRational
    )
  ) {
    Matrix<Rational> resultMatrixRational;
    resultMatrixRational.assignTensorProduct(
      leftMatrixRational, rightMatrixRational
    );
    return
    output.makeMatrix(calculator, resultMatrixRational, nullptr, true);
  }
  Matrix<Expression> leftMatrixExpression;
  Matrix<Expression> rightMatrixExpression;
  Matrix<Expression> resultMatrixExpression;
  if (
    !calculator.getMatrixExpressions(input[1], leftMatrixExpression)
  ) {
    return false;
  }
  if (
    !calculator.getMatrixExpressions(input[2], rightMatrixExpression)
  ) {
    return false;
  }
  resultMatrixExpression.assignTensorProduct(
    leftMatrixExpression, rightMatrixExpression
  );
  return
  output.assignMatrixExpressions(
    resultMatrixExpression, calculator, true, true
  );
}

bool CalculatorFunctionsBinaryOps::tensorMatrixByMatrixTensor(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input.startsWith(calculator.opTensor())) {
    return false;
  }
  STACK_TRACE("CalculatorFunctionsBinaryOps::tensorMatrixByMatrixTensor");
  const Expression& leftExpression = input[1];
  const Expression& rightExpression = input[2];
  if (
    !rightExpression.isOfType<MatrixTensor<Rational> >() ||
    !leftExpression.isOfType<MatrixTensor<Rational> >()
  ) {
    return false;
  }
  MatrixTensor<Rational> result;
  result.assignTensorProduct(
    leftExpression.getValue<MatrixTensor<Rational> >(),
    rightExpression.getValue<MatrixTensor<Rational> >()
  );
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsBinaryOps::multiplyRatOrAlgebraicByMatRatOrMatAlg(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::multiplyRatOrAlgebraicByMatRatOrMatAlg"
  );
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression* matrixExpression;
  const Expression* scalarExpression;
  matrixExpression = &input[1];
  scalarExpression = &input[2];
  if (
    !matrixExpression->isMatrixOfType<Rational>() &&
    !matrixExpression->isMatrixOfType<AlgebraicNumber>()
  ) {
    MathRoutines::swap(matrixExpression, scalarExpression);
  }
  if (
    !matrixExpression->isMatrixOfType<Rational>() &&
    !matrixExpression->isMatrixOfType<AlgebraicNumber>()
  ) {
    return false;
  }
  Matrix<AlgebraicNumber> matrixAlgebraic;
  AlgebraicNumber scalar;
  Matrix<Rational> matrixRational;
  Rational scalarRational;
  if (
    CalculatorConversions::functionGetMatrixNoComputation(
      calculator, *matrixExpression, matrixRational
    )
  ) {
    matrixAlgebraic = matrixRational;
  } else {
    if (
      !CalculatorConversions::functionGetMatrixNoComputation(
        calculator, *matrixExpression, matrixAlgebraic
      )
    ) {
      return false;
    }
  }
  if (scalarExpression->isOfType(&scalarRational)) {
    scalar = scalarRational;
  } else {
    if (!scalarExpression->isOfType(&scalar)) {
      return false;
    }
  }
  matrixAlgebraic *= scalar;
  return output.makeMatrix(calculator, matrixAlgebraic);
}

bool CalculatorFunctionsBinaryOps::multiplyMatRatOrMatAlgByMatRatOrMatAlg(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::multiplyMatRatOrMatAlgByMatRatOrMatAlg"
  );
  if (!input.isListNElements(3)) {
    return false;
  }
  Matrix<AlgebraicNumber> matrixAlgebraicLeft;
  Matrix<AlgebraicNumber> matrixAlgebraicRight;
  Matrix<Rational> matrixRationalLeft;
  Matrix<Rational> matrixRationalRight;
  const Expression& leftExpression = input[1];
  const Expression& rightExpression = input[2];
  if (
    leftExpression.isMatrixOfType<Rational>() &&
    rightExpression.isMatrixOfType<Rational>()
  ) {
    return false;
  }
  if (
    CalculatorConversions::functionGetMatrixNoComputation(
      calculator, leftExpression, matrixRationalLeft
    )
  ) {
    matrixAlgebraicLeft = matrixRationalLeft;
  } else {
    if (
      !CalculatorConversions::functionGetMatrixNoComputation(
        calculator, leftExpression, matrixAlgebraicLeft
      )
    ) {
      return false;
    }
  }
  if (
    CalculatorConversions::functionGetMatrixNoComputation(
      calculator, rightExpression, matrixRationalRight
    )
  ) {
    matrixAlgebraicRight = matrixRationalRight;
  } else {
    if (
      !CalculatorConversions::functionGetMatrixNoComputation(
        calculator, rightExpression, matrixAlgebraicRight
      )
    ) {
      return false;
    }
  }
  if (
    matrixAlgebraicLeft.numberOfColumns != matrixAlgebraicRight.numberOfRows
  ) {
    std::stringstream errorStream;
    errorStream
    << "Error: attempting to multiply matrix with "
    << matrixAlgebraicLeft.numberOfColumns
    << " columns by a "
    << "matrix with "
    << matrixAlgebraicRight.numberOfRows
    << " rows. ";
    return output.assignError(calculator, errorStream.str());
  }
  matrixAlgebraicLeft *= matrixAlgebraicRight;
  return output.makeMatrix(calculator, matrixAlgebraicLeft);
}

bool CalculatorFunctionsBinaryOps::
multiplyMatrixRationalOrRationalByMatrixRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "multiplyMatrixRationalOrRationalByMatrixRational"
  );
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression& leftExpression = input[1];
  const Expression& rightExpression = input[2];
  if (!rightExpression.isMatrixOfType<Rational>()) {
    return false;
  }
  Matrix<Rational> leftMatrix;
  Matrix<Rational> rightMatrix;
  if (
    !CalculatorConversions::functionGetMatrixNoComputation(
      calculator, rightExpression, rightMatrix
    )
  ) {
    return false;
  }
  Rational scalar;
  if (leftExpression.isOfType<Rational>(&scalar)) {
    leftMatrix *= scalar;
    return output.makeMatrix(calculator, leftMatrix);
  }
  if (!leftExpression.isMatrixOfType<Rational>()) {
    return false;
  }
  if (
    !CalculatorConversions::functionGetMatrixNoComputation(
      calculator, leftExpression, rightMatrix
    )
  ) {
    return false;
  }
  if (leftMatrix.numberOfColumns != rightMatrix.numberOfRows) {
    return false;
  }
  leftMatrix.multiplyOnTheRight(rightMatrix);
  return output.makeMatrix(calculator, leftMatrix);
}

bool CalculatorFunctionsBinaryOps::
multiplyMatrixRationalFractionOrRationalFractionByMatrixRationalFraction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "multiplyMatrixRationalFractionOrRationalFractionByMatrixRationalFraction"
  );
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression leftExpression = input[1];
  Expression rightExpression = input[2];
  if (
    !rightExpression.isMatrixOfType<RationalFraction<Rational> >() ||
    !(
      leftExpression.isOfType<Rational>() ||
      leftExpression.isOfType<Polynomial<Rational> >() ||
      leftExpression.isOfType<RationalFraction<Rational> >() ||
      leftExpression.isMatrixOfType<RationalFraction<Rational> >()
    )
  ) {
    return false;
  }
  if (!leftExpression.mergeContexts(leftExpression, rightExpression)) {
    return
    calculator
    << "Failed to convert "
    << leftExpression.toString()
    << " and "
    << rightExpression.toString()
    << " to common context. ";
  }
  Matrix<RationalFraction<Rational> > leftMatrix;
  Matrix<RationalFraction<Rational> > rightMatrix;
  if (
    !CalculatorConversions::functionGetMatrixNoComputation(
      calculator, rightExpression, rightMatrix
    )
  ) {
    return false;
  }
  if (!leftExpression.isMatrixOfType<RationalFraction<Rational> >()) {
    WithContext<RationalFraction<Rational> >
    leftExpressionRationalFractionForm;
    if (
      !CalculatorConversions::convertWithoutComputation(
        calculator, leftExpression, leftExpressionRationalFractionForm
      )
    ) {
      return
      calculator
      << "Failed to convert "
      << leftExpression.toString()
      << " to rational function. ";
    }
    rightMatrix *= leftExpressionRationalFractionForm.content;
    ExpressionContext contextExpression = leftExpression.getContext();
    return output.makeMatrix(calculator, rightMatrix, &contextExpression);
  }
  if (
    !CalculatorConversions::functionGetMatrixNoComputation(
      calculator, leftExpression, leftMatrix
    )
  ) {
    return false;
  }
  if (leftMatrix.numberOfColumns != rightMatrix.numberOfRows) {
    return false;
  }
  leftMatrix.multiplyOnTheRight(rightMatrix);
  ExpressionContext contextExpression = leftExpression.getContext();
  return output.makeMatrix(calculator, leftMatrix, &contextExpression);
}

bool CalculatorFunctionsBinaryOps::multiplyMatrixTensorOrRationalByMatrixTensor
(Calculator& calculator, const Expression& input, Expression& output) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "multiplyMatrixTensorOrRationalByMatrixTensor"
  );
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression& leftExpression = input[1];
  const Expression& rightExpression = input[2];
  if (!rightExpression.isOfType<MatrixTensor<Rational> >()) {
    return false;
  }
  Rational scalar;
  if (leftExpression.isOfType<Rational>(&scalar)) {
    MatrixTensor<Rational> result =
    rightExpression.getValue<MatrixTensor<Rational> >();
    result *= scalar;
    return output.assignValue(calculator, result);
  }
  if (!leftExpression.isOfType<MatrixTensor<Rational> >()) {
    return false;
  }
  const MatrixTensor<Rational>& rightMatrix =
  rightExpression.getValue<MatrixTensor<Rational> >();
  MatrixTensor<Rational> result =
  leftExpression.getValue<MatrixTensor<Rational> >();
  result *= rightMatrix;
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsBinaryOps::lieBracketExtractConstant(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::lieBracketExtractConstant");
  if (!input.startsWith(calculator.opLieBracket(), 3)) {
    return false;
  }
  Rational coefficient = 1;
  Rational rationalValue;
  Expression leftExpression = input[1];
  Expression rightExpression = input[2];
  bool found = false;
  if (input[1].startsWith(calculator.opTimes(), 3)) {
    if (input[1][1].isOfType(&rationalValue)) {
      found = true;
      coefficient *= rationalValue;
      leftExpression = input[1][2];
    }
  }
  if (input[2].startsWith(calculator.opTimes(), 3)) {
    if (input[2][1].isOfType(&rationalValue)) {
      found = true;
      coefficient *= rationalValue;
      rightExpression = input[2][2];
    }
  }
  if (!found) {
    return false;
  }
  Expression bracket;
  bracket.makeXOX(
    calculator,
    calculator.opLieBracket(),
    leftExpression,
    rightExpression
  );
  output.assignValue(calculator, coefficient);
  output *= bracket;
  return true;
}

bool CalculatorFunctionsBinaryOps::lieBracketDistribute(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::lieBracketDistribute");
  return
  CalculatorBasics::distribute(
    calculator,
    input,
    output,
    calculator.opPlus(),
    calculator.opLieBracket()
  );
}

bool CalculatorFunctionsBinaryOps::lieBracketRatOrUEWithRatOrUE(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::lieBracketRatOrUEWithRatOrUE");
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.mergeContextsMyAruments(inputConverted, nullptr)) {
    return false;
  }
  const Expression& leftExpression = inputConverted[1];
  const Expression& rightExpression = inputConverted[2];
  if (
    leftExpression.isOfType<Rational>() || rightExpression.isOfType<Rational>()
  ) {
    return
    output.assignValueWithContext(
      calculator, 0, leftExpression.getContext()
    );
  }
  if (
    leftExpression.isOfType<
      ElementUniversalEnveloping<RationalFraction<Rational> >
    >() &&
    rightExpression.isOfType<
      ElementUniversalEnveloping<RationalFraction<Rational> >
    >()
  ) {
    ElementUniversalEnveloping<RationalFraction<Rational> > result;
    leftExpression.getValue<
      ElementUniversalEnveloping<RationalFraction<Rational> >
    >().lieBracketOnTheRight(
      rightExpression.getValue<
        ElementUniversalEnveloping<RationalFraction<Rational> >
      >(),
      result
    );
    result.simplify(RationalFraction<Rational>::one());
    return
    output.assignValueWithContext(
      calculator, result, leftExpression.getContext()
    );
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::lieBracketJacobiIdentityIfNeeded(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "lieBracketJacobiIdentityIfNeeded"
  );
  if (!input.startsWith(calculator.opLieBracket(), 3)) {
    return false;
  }
  if (!input[2].startsWith(calculator.opLieBracket())) {
    return false;
  }
  bool doContinue = (
    input[1] > input[2][1] && input[1] > input[2][2]
  );
  if (!doContinue) {
    return false;
  }
  Expression leftExpression;
  Expression rightExpression;
  Expression lieBracket;
  lieBracket.makeXOX(
    calculator,
    calculator.opLieBracket(),
    input[2][2],
    input[1]
  );
  leftExpression.makeXOX(
    calculator, calculator.opLieBracket(), input[2][1], lieBracket
  );
  lieBracket.makeXOX(
    calculator,
    calculator.opLieBracket(),
    input[1],
    input[2][1]
  );
  rightExpression.makeXOX(
    calculator, calculator.opLieBracket(), input[2][2], lieBracket
  );
  leftExpression = calculator.expressionMinusOne() * leftExpression;
  rightExpression = calculator.expressionMinusOne() * rightExpression;
  output = leftExpression + rightExpression;
  return true;
}

bool CalculatorFunctionsBinaryOps::lieBracketSwapTermsIfNeeded(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::lieBracketSwapTermsIfNeeded");
  if (!input.startsWith(calculator.opLieBracket(), 3)) {
    return false;
  }
  if (input[1].isEqualToZero() || input[2].isEqualToZero()) {
    return output.assignValue(calculator, 0);
  }
  if (input[2] > input[1]) {
    return false;
  }
  if (input[1] == input[2]) {
    return output.assignValue(calculator, 0);
  }
  Expression bracket;
  bracket.makeXOX(
    calculator, calculator.opLieBracket(), input[2], input[1]
  );
  output = calculator.expressionMinusOne() * bracket;
  return true;
}

bool CalculatorFunctionsBinaryOps::lieBracketOfMatrices(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::lieBracketOfMatrices");
  (void) calculator;
  if (input.size() != 3) {
    return false;
  }
  const Expression left = input[1];
  const Expression right = input[2];
  int leftRowCount = 0;
  int leftColumnCount = 0;
  int rightRowCount = 0;
  int rightColumnCount = 0;
  if (
    !left.isMatrix(&leftRowCount, &leftColumnCount) ||
    !right.isMatrix(&rightRowCount, &rightColumnCount)
  ) {
    return false;
  }
  if (
    leftRowCount < 2 ||
    leftRowCount != leftColumnCount ||
    leftRowCount != rightRowCount ||
    leftRowCount != rightColumnCount
  ) {
    return false;
  }
  output = left * right - right * left;
  return true;
}

bool CalculatorFunctionsBinaryOps::lieBracketRatPolyOrEWAWithRatPolyOrEWA(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::lieBracketRatPolyOrEWAWithRatPolyOrEWA"
  );
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.mergeContextsMyAruments(inputConverted, nullptr)) {
    return false;
  }
  const Expression& leftE = inputConverted[1];
  const Expression& rightExpression = inputConverted[2];
  bool leftEisGood = leftE.isOfType<Rational>() ||
  leftE.isOfType<Polynomial<Rational> >() ||
  leftE.isOfType<ElementWeylAlgebra<Rational> >();
  bool rightEisGood = rightExpression.isOfType<Rational>() ||
  rightExpression.isOfType<Polynomial<Rational> >() ||
  rightExpression.isOfType<ElementWeylAlgebra<Rational> >();
  if (!leftEisGood || !rightEisGood) {
    return false;
  }
  if (
    !leftE.isOfType<ElementWeylAlgebra<Rational> >() &&
    !rightExpression.isOfType<ElementWeylAlgebra<Rational> >()
  ) {
    return output.assignValue(calculator, 0);
  }
  WithContext<ElementWeylAlgebra<Rational> > leftConverted;
  WithContext<ElementWeylAlgebra<Rational> > rightConverted;
  if (
    !CalculatorConversions::convertWithoutComputation(
      calculator, leftE, leftConverted
    ) ||
    !CalculatorConversions::convertWithoutComputation(
      calculator, rightExpression, rightConverted
    )
  ) {
    calculator
    << "<hr>Failed with conversion to "
    << "Element weyl algebra - possible programming error?";
    return false;
  }
  ElementWeylAlgebra<Rational> resultE = rightConverted.content;
  resultE.lieBracketOnTheLeft(leftConverted.content);
  return
  output.assignValueWithContext(calculator, resultE, leftConverted.context);
}

bool CalculatorFunctionsBinaryOps::addMatrixToMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::addMatrixToMatrix");
  if (!input.startsWith(calculator.opPlus(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightExpression = input[2];
  int leftNumberOfRows = - 1;
  int leftNumberOfColumns = - 1;
  int rightNumberOfRows = - 1;
  int rightNumberOfColumns = - 1;
  if (
    !leftE.isMatrix(&leftNumberOfRows, &leftNumberOfColumns) ||
    !rightExpression.isMatrix(&rightNumberOfRows, &rightNumberOfColumns)
  ) {
    return false;
  }
  if ((leftNumberOfRows != rightNumberOfRows) || (
      leftNumberOfColumns != rightNumberOfColumns
    )
  ) {
    return false;
  }
  Matrix<Expression> leftMatrix;
  Matrix<Expression> rightMatrix;
  if (
    !calculator.getMatrixExpressions(leftE, leftMatrix) ||
    !calculator.getMatrixExpressions(rightExpression, rightMatrix)
  ) {
    return false;
  }
  if (
    leftMatrix.numberOfColumns != rightMatrix.numberOfColumns ||
    leftMatrix.numberOfRows != rightMatrix.numberOfRows
  ) {
    return false;
  }
  for (int i = 0; i < leftMatrix.numberOfRows; i ++) {
    for (int j = 0; j < leftMatrix.numberOfColumns; j ++) {
      leftMatrix(i, j) += rightMatrix(i, j);
    }
  }
  return output.assignMatrixExpressions(leftMatrix, calculator, false, true);
}

bool CalculatorFunctionsBinaryOps::augmentMatrixToTheRight(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::augmentMatrixToTheRight");
  if (input.size() != 3) {
    return false;
  }
  Matrix<Expression> leftMatrix;
  Matrix<Expression> rightMatrix;
  if (
    !calculator.getMatrixExpressions(input[1], leftMatrix) ||
    !calculator.getMatrixExpressions(input[2], rightMatrix)
  ) {
    return false;
  }
  if (leftMatrix.numberOfRows != rightMatrix.numberOfRows) {
    return
    calculator
    << "Cannot augment the left matrix with: "
    << leftMatrix.numberOfRows
    << " rows to the right by a matrix with a different number of rows: "
    << rightMatrix.numberOfRows
    << ". ";
  }
  leftMatrix.appendMatrixOnTheRight(rightMatrix);
  return
  output.assignMatrixExpressions(leftMatrix, calculator, false, false);
}

bool CalculatorFunctionsBinaryOps::augmentMatrixBelow(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::augmentMatrixBelow");
  if (input.size() != 3) {
    return false;
  }
  Matrix<Expression> leftMatrix;
  Matrix<Expression> rightMatrix;
  if (
    !calculator.getMatrixExpressions(input[1], leftMatrix) ||
    !calculator.getMatrixExpressions(input[2], rightMatrix)
  ) {
    return false;
  }
  if (leftMatrix.numberOfColumns != rightMatrix.numberOfColumns) {
    return
    calculator
    << "Cannot augment the left matrix with: "
    << leftMatrix.numberOfRows
    << " rows to the right by a matrix with a different number of rows: "
    << rightMatrix.numberOfRows
    << ". ";
  }
  leftMatrix.appendMatrixToTheBottom(rightMatrix);
  return
  output.assignMatrixExpressions(leftMatrix, calculator, false, false);
}

bool CalculatorFunctionsBinaryOps::directSumMatrixWithMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::directSumMatrixWithMatrix");
  if (!input.startsWith(calculator.opDirectSum(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightExpression = input[2];
  if (!leftE.isMatrix() || !rightExpression.isMatrix()) {
    return false;
  }
  Matrix<Expression> leftMatrix;
  Matrix<Expression> rightMatrix;
  if (
    !calculator.getMatrixExpressions(leftE, leftMatrix) ||
    !calculator.getMatrixExpressions(rightExpression, rightMatrix)
  ) {
    return false;
  }
  leftMatrix.directSumWith(rightMatrix, calculator.expressionZero());
  return output.assignMatrixExpressions(leftMatrix, calculator, false, true);
}

bool CalculatorFunctionsBinaryOps::
addMatrixRationalOrAlgebraicToMatrixRationalOrAlgebraic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "addMatrixRationalOrAlgebraicToMatrixRationalOrAlgebraic"
  );
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression leftE = input[1];
  Expression rightExpression = input[2];
  Matrix<Rational> leftMatrixRational;
  Matrix<Rational> rightMatrixRational;
  Matrix<AlgebraicNumber> leftMatrixAlgebraic;
  Matrix<AlgebraicNumber> rightMatrixAlgebraic;
  if (
    !rightExpression.isMatrixOfType<Rational>() ||
    !leftE.isMatrixOfType<Rational>()
  ) {
    if (rightExpression.isMatrixOfType<AlgebraicNumber>()) {
      MathRoutines::swap(leftE, rightExpression);
    }
    if (!leftE.isMatrixOfType<AlgebraicNumber>()) {
      return false;
    }
    if (rightExpression.isMatrixOfType<Rational>()) {
      if (
        !CalculatorConversions::functionGetMatrixNoComputation(
          calculator, leftE, leftMatrixAlgebraic
        )
      ) {
        return false;
      }
      if (
        !CalculatorConversions::functionGetMatrixNoComputation(
          calculator, rightExpression, rightMatrixAlgebraic
        )
      ) {
        return false;
      }
      if (
        rightMatrixAlgebraic.numberOfRows != leftMatrixAlgebraic.numberOfRows
        ||
        rightMatrixAlgebraic.numberOfColumns !=
        leftMatrixAlgebraic.numberOfColumns
      ) {
        return false;
      }
      leftMatrixAlgebraic += rightMatrixAlgebraic;
      return output.makeMatrix(calculator, leftMatrixAlgebraic);
    }
    if (
      !CalculatorConversions::functionGetMatrixNoComputation(
        calculator, rightExpression, rightMatrixAlgebraic
      )
    ) {
      return false;
    }
    if (
      !CalculatorConversions::functionGetMatrixNoComputation(
        calculator, leftE, leftMatrixAlgebraic
      )
    ) {
      return false;
    }
    if (
      rightMatrixAlgebraic.numberOfRows != leftMatrixAlgebraic.numberOfRows ||
      rightMatrixAlgebraic.numberOfColumns !=
      leftMatrixAlgebraic.numberOfColumns
    ) {
      return false;
    }
    leftMatrixAlgebraic += rightMatrixAlgebraic;
    return output.makeMatrix(calculator, leftMatrixAlgebraic);
  }
  if (
    !CalculatorConversions::functionGetMatrixNoComputation(
      calculator, leftE, leftMatrixRational
    )
  ) {
    return false;
  }
  if (
    !CalculatorConversions::functionGetMatrixNoComputation(
      calculator, rightExpression, rightMatrixRational
    )
  ) {
    return false;
  }
  if (
    leftMatrixRational.numberOfRows != rightMatrixRational.numberOfRows ||
    leftMatrixRational.numberOfColumns != rightMatrixRational.numberOfColumns
  ) {
    std::stringstream errorStream;
    errorStream
    << "Error: attempting to add a "
    << rightMatrixRational.numberOfRows
    << " by "
    << rightMatrixRational.numberOfColumns
    << " matrix to a "
    << leftMatrixRational.numberOfRows
    << " by "
    << leftMatrixRational.numberOfColumns
    << " matrix. ";
    return output.assignError(calculator, errorStream.str());
  }
  leftMatrixRational += rightMatrixRational;
  return output.makeMatrix(calculator, leftMatrixRational);
}

bool CalculatorFunctionsBinaryOps::setMinus(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::setMinus");
  if (input.size() != 3) {
    return false;
  }
  const Expression& leftSetE = input[1];
  const Expression& rightSetE = input[2];
  if (
    !leftSetE.isSequenceNElements() || !rightSetE.isSequenceNElements()
  ) {
    return false;
  }
  if (leftSetE.hasBoundVariables() || rightSetE.hasBoundVariables()) {
    return false;
  }
  HashedList<Expression> resultEs;
  resultEs.setExpectedSize(leftSetE.size() - 1);
  for (int i = 1; i < leftSetE.size(); i ++) {
    resultEs.addOnTop(leftSetE[i]);
  }
  for (int i = 1; i < rightSetE.size(); i ++) {
    if (resultEs.contains(rightSetE[i])) {
      resultEs.removeIndexSwapWithLast(resultEs.getIndex(rightSetE[i]));
    }
  }
  resultEs.quickSortAscending();
  return output.makeSequence(calculator, &resultEs);
}

bool CalculatorFunctionsBinaryOps::
addMatrixRationalFractionsToMatrixRationalFractions(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::"
    "addMatrixRationalFractionsToMatrixRationalFractions"
  );
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression leftE = input[1];
  Expression rightExpression = input[2];
  if (
    !rightExpression.isMatrixOfType<RationalFraction<Rational> >() ||
    !leftE.isMatrixOfType<RationalFraction<Rational> >()
  ) {
    return false;
  }
  if (!leftE.mergeContexts(leftE, rightExpression)) {
    return false;
  }
  Matrix<RationalFraction<Rational> > leftMatrix;
  Matrix<RationalFraction<Rational> > rightMatrix;
  if (
    !CalculatorConversions::functionGetMatrixNoComputation(
      calculator, leftE, leftMatrix
    )
  ) {
    return false;
  }
  if (
    !CalculatorConversions::functionGetMatrixNoComputation(
      calculator, rightExpression, rightMatrix
    )
  ) {
    return false;
  }
  if (
    rightMatrix.numberOfRows != leftMatrix.numberOfRows ||
    rightMatrix.numberOfColumns != leftMatrix.numberOfColumns
  ) {
    return false;
  }
  leftMatrix += rightMatrix;
  ExpressionContext context = leftE.getContext();
  return output.makeMatrix(calculator, leftMatrix, &context);
}

bool CalculatorFunctionsBinaryOps::addMatrixTensorToMatrixTensor(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightExpression = input[2];
  if (
    !rightExpression.isOfType<MatrixTensor<Rational> >() ||
    !leftE.isOfType<MatrixTensor<Rational> >()
  ) {
    return false;
  }
  const MatrixTensor<Rational>& rightMatrix =
  rightExpression.getValue<MatrixTensor<Rational> >();
  const MatrixTensor<Rational>& leftMatrix =
  leftE.getValue<MatrixTensor<Rational> >();
  MatrixTensor<Rational> result = leftMatrix;
  result += rightMatrix;
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsBinaryOps::multiplySequenceByAnyScalar(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::multiplySequenceByAnyScalar");
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression swapped = input;
  swapped.swapChildren(1, 2);
  return
  CalculatorFunctionsBinaryOps::multiplyAnyScalarBySequence(
    calculator, swapped, output
  );
}

bool CalculatorFunctionsBinaryOps::addSequenceToSequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::addSequenceToSequence");
  if (!input.isListNElements(3)) {
    return false;
  }
  if (!input[1].isSequenceNElements()) {
    return false;
  }
  if (!input[2].isSequenceNElements()) {
    return false;
  }
  if (input[2].size() != input[1].size()) {
    return
    calculator
    << "<hr>Attempting to add a sequence of length "
    << input[1].size() - 1
    << "  to a sequence of length "
    << input[2].size() - 1
    << ", possible user typo?";
  }
  output.reset(calculator);
  output.setExpectedSize(input[1].size());
  output.addChildAtomOnTop(calculator.opSequence());
  Expression summand;
  for (int i = 1; i < input[2].size(); i ++) {
    summand.makeXOX(
      calculator,
      calculator.opPlus(),
      input[1][i],
      input[2][i]
    );
    output.addChildOnTop(summand);
  }
  return true;
}

bool CalculatorFunctionsBinaryOps::binomialCoefficient(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::binomialCoefficient");
  if (input.size() != 3) {
    return false;
  }
  Rational N;
  int K = - 1;
  if (!input[2].isSmallInteger(&K)) {
    return false;
  }
  if (K < 0) {
    return false;
  }
  if (input[1].isOfType<Rational>(&N)) {
    Rational result = result.nChooseK(N, K);
    return output.assignValue(calculator, result);
  }
  if (K < 11) {
    output.assignValue(calculator, 1);
    for (int i = 0; i < K; i ++) {
      output *= input[1] - i;
    }
    Expression factorial;
    Expression kExpression;
    kExpression.assignValue(calculator, K);
    factorial.makeOX(calculator, calculator.opFactorial(), kExpression);
    output /= factorial;
    return true;
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::powerElementZmodPByInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::powerElementZmodPByInteger");
  if (!input.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightExpression = input[2];
  ElementZmodP element;
  if (!leftE.isOfType(&element)) {
    return false;
  }
  LargeInteger power = 0;
  if (!rightExpression.isInteger(&power)) {
    return false;
  }
  if (power < 0) {
    ElementZmodP copy = element;
    element.makeOne(element.modulus);
    element /= copy;
    power *= - 1;
  }
  ElementZmodP unit;
  unit.makeOne(element.modulus);
  MathRoutines::raiseToPower(element, power, unit);
  ExpressionContext context(calculator);
  context.setDefaultModulus(element.modulus);
  return output.assignValueWithContext(calculator, element, context);
}

bool CalculatorFunctionsBinaryOps::powerEllipticCurveRationalElementByInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::powerEllipticCurveRationalElementByInteger"
  );
  if (!input.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  const Expression& leftExpression = input[1];
  const Expression& rightExpression = input[2];
  ElementEllipticCurve<Rational> element;
  if (!leftExpression.isOfType(&element)) {
    return false;
  }
  int power = 0;
  if (!rightExpression.isSmallInteger(&power)) {
    return false;
  }
  if (power < 0) {
    element.invert();
    power *= - 1;
  }
  ElementEllipticCurve<Rational> unit;
  unit.makeOne(element.owner);
  MathRoutines::raiseToPower(element, power, unit);
  return
  output.assignValueWithContext(
    calculator, element, input[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::powerEllipticCurveZmodPElementByInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::powerEllipticCurveZmodPElementByInteger"
  );
  if (!input.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  const Expression& leftExpression = input[1];
  const Expression& rightExpression = input[2];
  ElementEllipticCurve<ElementZmodP> element;
  if (!leftExpression.isOfType(&element)) {
    return false;
  }
  LargeInteger power = 0;
  if (!rightExpression.isInteger(&power)) {
    return false;
  }
  if (power < 0) {
    element.invert();
    power *= - 1;
  }
  ElementEllipticCurve<ElementZmodP> unit;
  unit.makeOne(element.owner);
  MathRoutines::raiseToPower(element, power, unit);
  return
  output.assignValueWithContext(
    calculator, element, input[1].getContext()
  );
}

bool CalculatorFunctionsBinaryOps::polynomialModPModuloPolynomialModP(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsBinaryOps::polynomialModPModuloPolynomialModP"
  );
  if (input.size() != 3) {
    return false;
  }
  if (
    !input[1].isOfType<Polynomial<ElementZmodP> >() ||
    !input[2].isOfType<Polynomial<ElementZmodP> >()
  ) {
    return false;
  }
  Expression inputConverted;
  if (
    !input.mergeContextsMyArumentsAndConvertThem<Polynomial<ElementZmodP> >(
      inputConverted, &calculator.comments
    )
  ) {
    return false;
  }
  WithContext<Polynomial<ElementZmodP> > left;
  WithContext<Polynomial<ElementZmodP> > right;
  if (
    !inputConverted[1].isOfTypeWithContext(&left) ||
    !inputConverted[2].isOfTypeWithContext(&right)
  ) {
    return false;
  }
  if (right.content.isEqualToZero()) {
    return output.assignError(calculator, "Division by zero");
  }
  LargeIntegerUnsigned modulus = right.content.coefficients[0].modulus;
  if (!left.content.isEqualToZero()) {
    if (left.content.coefficients[0].modulus != modulus) {
      return
      calculator
      << "Attempt to perform operations with "
      << "modular polynomials with different moduli: "
      << modulus
      << " and "
      << left.content.coefficients[0].modulus;
    }
  }
  ElementZmodP rightLeadingCoefficient =
  right.content.getLeadingCoefficient(&MonomialPolynomial::orderDefault());
  if (!rightLeadingCoefficient.invert()) {
    return
    calculator
    << "Leading coefficient of quotient "
    << "polynomial is not invertible mod "
    << modulus
    << ".";
  }
  PolynomialModuloPolynomial<ElementZmodP> result;
  result.modulusContainer = right.content;
  result.value = left.content;
  result.reduce();
  return output.assignValueWithContext(calculator, result, right.context);
}
