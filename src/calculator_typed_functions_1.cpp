// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "calculator_inner_typed_functions.h"
#include "math_general_implementation.h"
#include "math_extra_Weyl_algebras_implementation.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_extra_semisimple_Lie_algebras_implementation.h"
#include "math_extra_modules_semisimple_Lie_algebras_implementation.h"
#include "math_general_polynomial_computations_basic_implementation.h" //undefined reference to Polynomial<AlgebraicNumber>::makeOne(int)
#include "math_extra_finite_groups_implementation.h" // undefined reference to `void WeylGroup::raiseToDominantWeight<Rational>(Vector<Rational>&, int*, bool*, ElementWeylGroup<WeylGroup>*)
#include "math_rational_function_implementation.h"

template <>
bool Expression::convertInternally<RationalFunction<Rational> >(Expression& output) const;

template <>
bool Expression::convertInternally<ElementWeylAlgebra<Rational> >(Expression& output) const;

bool CalculatorFunctionsBinaryOps::innerAddElementZModPOrRationalToElementZModPOrRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddElementZModPOrRationalToElementZModPOrRational");
  if (input.size() != 3) {
    return false;
  }
  const Expression* leftE;
  const Expression* rightE;
  leftE = &input[1];
  rightE = &input[2];
  ElementZmodP element1, element2;
  for (int i = 0; i < 2; i ++, MathRoutines::swap(leftE, rightE)) {
    if (leftE->isOfType<ElementZmodP>(&element1)) {
      if (rightE->isOfType<ElementZmodP>(&element2)) {
        if (element1.modulus != element2.modulus) {
          return false;
        }
      } else {
        Rational tempRat;
        if (!rightE->isOfType<Rational>(&tempRat)) {
          return false;
        }
        element2.modulus = element1.modulus;
        if (!element2.assignRational(tempRat)) {
          return false;
        }
      }
      element1 += element2;
      ExpressionContext context(calculator);
      context.setDefaultModulus(element1.modulus);
      return output.assignValueWithContext(element1, context, calculator);
    }
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerMultiplyEltZmodPorRatByEltZmodPorRat(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyEltZmodPorRatByEltZmodPorRat");
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression* leftE;
  const Expression* rightE;
  leftE = &input[1];
  rightE = &input[2];
  ElementZmodP theElt1, theElt2;
  for (int i = 0; i < 2; i ++, MathRoutines::swap(leftE, rightE)) {
    if (leftE->isOfType<ElementZmodP>(&theElt1)) {
      if (rightE->isOfType<ElementZmodP>(&theElt2)) {
        if (theElt1.modulus != theElt2.modulus) {
          return false;
        }
      } else {
        Rational tempRat;
        if (!rightE->isOfType<Rational>(&tempRat)) {
          return false;
        }
        theElt2.modulus = theElt1.modulus;
        if (!theElt2.assignRational(tempRat)) {
          return false;
        }
      }
      theElt1 *= theElt2;
      ExpressionContext context(calculator);
      context.setDefaultModulus(theElt1.modulus);
      return output.assignValueWithContext(theElt1, context, calculator);
    }
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerDivideEltZmodPorRatByEltZmodPorRat(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyEltZmodPorRatByEltZmodPorRat");
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression* leftE;
  const Expression* rightE;
  leftE = &input[1];
  rightE = &input[2];
  ElementZmodP theElt1, theElt2;
  for (int i = 0; i < 2; i ++, MathRoutines::swap(leftE, rightE)) {
    if (leftE->isOfType<ElementZmodP>(&theElt1)) {
      if (rightE->isOfType<ElementZmodP>(&theElt2)) {
        if (theElt1.modulus != theElt2.modulus) {
          return false;
        }
      } else {
        Rational tempRat;
        if (!rightE->isOfType<Rational>(&tempRat)) {
          return false;
        }
        theElt2.modulus = theElt1.modulus;
        if (!theElt2.assignRational(tempRat)) {
          return false;
        }
      }
      if (i == 1) {
        MathRoutines::swap(theElt1, theElt2);
      }
      if (!(theElt1 /= theElt2)) {
        std::stringstream out;
        out << "Got division by zero while attempting to divide "
        << theElt1.toString() << " by " << theElt2.toString();
        return output.makeError(out.str(), calculator);
      }
      ExpressionContext context(calculator);
      context.setDefaultModulus(theElt1.modulus);
      return output.assignValueWithContext(theElt1, context, calculator);
    }
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerAddRatToRat(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddRatToRat");
  if (input.size() != 3) {
    return false;
  }
  Rational leftR, rightR;
  if (!input[1].isOfType(&leftR) || !input[2].isOfType(&rightR)) {
    return false;
  }
  return output.assignValue(leftR + rightR, calculator);
}

bool CalculatorFunctionsBinaryOps::innerAddStringToString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddStringToString");
  if (input.size() != 3) {
    return false;
  }
  std::string left, right;
  if (!input[1].isOfType(&left) || !input[2].isOfType(&right)) {
    return false;
  }
  return output.assignValue(left + right, calculator);
}

bool CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat");
  if (!input.isListNElements(3)) {
    return false;
  }
  AlgebraicNumber leftAN, rightAN;
  Rational tempRat;
  if (!input[1].isOfType(&leftAN)) {
    if (!input[2].isOfType(&rightAN)) {
      return false;
    }
    if (!input[1].isOfType<Rational>(&tempRat)) {
      return false;
    }
    leftAN.assignRational(tempRat, &calculator.theObjectContainer.theAlgebraicClosure);
  } else if (!input[2].isOfType(&rightAN)) {
    if (!input[2].isOfType(&tempRat)) {
      return false;
    }
    rightAN.assignRational(tempRat, &calculator.theObjectContainer.theAlgebraicClosure);
  }
  if (rightAN.isEqualToZero()) {
    return output.makeError("Division by zero. ", calculator);
  }
  leftAN /= rightAN;
  output.assignValue(leftAN, calculator);
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctionsBinaryOps::innerMultiplyEltHypOctByEltHypOct(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyEltHypOctByEltHypOct");
  if (!input.isListNElements(3)) {
    return false;
  }
  ElementHyperoctahedralGroupR2 outElt, left, right;
  if (
    !input[1].isOfType<ElementHyperoctahedralGroupR2>(&left) ||
    !input[2].isOfType<ElementHyperoctahedralGroupR2>(&right)
  ) {
    return false;
  }
  outElt = left * right;
  return output.assignValue(outElt, calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyAlgebraicNumberByAlgebraicNumber(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyAlgebraicNumberByAlgebraicNumber");
  if (!input.isListNElements(3)) {
    return false;
  }
  AlgebraicNumber leftAN, rightAN;
  Rational rationalValue;
  if (!input[1].isOfType(&leftAN)) {
    if (!input[2].isOfType(&rightAN)) {
      return false;
    }
    if (!input[1].isOfType<Rational>(&rationalValue)) {
      return false;
    }
    leftAN.assignRational(rationalValue, &calculator.theObjectContainer.theAlgebraicClosure);
  } else if (!input[2].isOfType(&rightAN)) {
    if (!input[2].isOfType(&rationalValue)) {
      return false;
    }
    rightAN.assignRational(rationalValue, &calculator.theObjectContainer.theAlgebraicClosure);
  }
  leftAN *= rightAN;
  return output.assignValue(leftAN, calculator);
}

bool CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber");
  if (!input.isListNElements(3)) {
    return false;
  }
  AlgebraicNumber leftAN, rightAN;
  Rational tempRat;
  if (!input[1].isOfType(&leftAN)) {
    if (!input[2].isOfType(&rightAN)) {
      return false;
    }
    if (!input[1].isOfType<Rational>(&tempRat)) {
      return false;
    }
    leftAN.assignRational(tempRat, &calculator.theObjectContainer.theAlgebraicClosure);
    leftAN.checkConsistency();
  } else if (!input[2].isOfType(&rightAN)) {
    if (!input[2].isOfType(&tempRat)) {
      return false;
    }
    rightAN.assignRational(tempRat, &calculator.theObjectContainer.theAlgebraicClosure);
    rightAN.checkConsistency();
  }
  leftAN.checkConsistency();
  rightAN.checkConsistency();
  leftAN += rightAN;
  leftAN.checkConsistency();
  return output.assignValue(leftAN, calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRationalByRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRationalByRational");
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational leftR, rightR;
  if (!input[1].isOfType(&leftR) || !input[2].isOfType(&rightR)) {
    return false;
  }
  return output.assignValue(leftR*rightR, calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyCoxeterEltByCoxeterElt(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyCoxeterEltByCoxeterElt");
  if (!input.isListNElements(3)) {
    return false;
  }
  ElementWeylGroup leftR, rightR;
  if (!input[1].isOfType(&leftR) || !input[2].isOfType(&rightR)) {
    return false;
  }
  if (leftR.owner != rightR.owner) {
    return calculator << "<hr>Attempting to multiply elements of different groups, possible user typo?";
  }
  leftR *= rightR;
  return output.assignValue(leftR, calculator);
}

bool CalculatorFunctionsBinaryOps::innerDivideRatByRat(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDivideRatByRat");
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational leftR, rightR;
  if (!input[1].isOfType(&leftR) || !input[2].isOfType(&rightR)) {
    return false;
  }
  if (rightR.isEqualToZero()) {
    return output.makeError("Division by zero.", calculator);
  }
  return output.assignValue(leftR / rightR, calculator);
}

bool CalculatorFunctionsBinaryOps::innerDivideDoubleByDouble(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDivideRatByRat");
  if (!input.isListNElements(3)) {
    return false;
  }
  double leftD, rightD;
  Rational leftR, rightR;
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
    return output.makeError("Division by zero.", calculator);
  }
  return output.assignValue(leftD / rightD, calculator);
}

bool CalculatorFunctionsBinaryOps::tensorElementTensorByElementTensor(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::tensorElementTensorByElementTensor");
  calculator.checkInputNotSameAsOutput(input, output);
  if (input.size() != 3) {
    return false;
  }
  Expression inputConverted;
  if (!input.mergeContextsMyArumentsAndConvertThem<ElementTensorsGeneralizedVermas<RationalFunction<Rational> > >(
    inputConverted, &calculator.comments
  )) {
    return false;
  }
  ElementTensorsGeneralizedVermas<RationalFunction<Rational> > resultTensor;
  resultTensor = inputConverted[1].getValue<ElementTensorsGeneralizedVermas<RationalFunction<Rational> > >();
  resultTensor.tensorOnTheRight(inputConverted[2].getValue<ElementTensorsGeneralizedVermas<RationalFunction<Rational> > >());
  return output.assignValueWithContext(resultTensor, inputConverted[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerAddWeightToWeight(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddWeightToWeight");
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<Weight<Polynomial<Rational> > >(calculator, input, output);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyByWeightPoly(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyByWeightPoly");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.mergeContextsMyAruments(inputConverted, &calculator.comments)) {
    return false;
  }
  Weight<Polynomial<Rational> > theWeight;
  Rational cfRat;
  Polynomial<Rational> theCoefficient;
  if (!inputConverted[1].isOfType<Polynomial<Rational> >(&theCoefficient)) {
    if (!inputConverted[1].isOfType<Rational>(&cfRat)) {
      return false;
    }
    theCoefficient = cfRat;
  }
  if (!inputConverted[2].isOfType<Weight<Polynomial<Rational> > >(&theWeight)) {
    return false;
  }
  theWeight *= theCoefficient;
  return output.assignValueWithContext(theWeight, inputConverted[2].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyWeylGroupEltByWeightPoly(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyWeylGroupEltByWeightPoly");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.mergeContextsMyAruments(inputConverted, &calculator.comments)) {
    return false;
  }
  Weight<Polynomial<Rational> > theWeight;
  if (!inputConverted[2].isOfType<Weight<Polynomial<Rational> > >(&theWeight)) {
    return false;
  }
  if (!inputConverted[1].isOfType<ElementWeylGroup>()) {
    return false;
  }
  ElementWeylGroup theElt = inputConverted[1].getValue<ElementWeylGroup>();
  if (theElt.owner != &theWeight.owner->theWeyl) {
    return calculator << "<hr>Possible user input error: attempting to apply Weyl group "
    << "element to weight corresponding to different Weyl group.";
  }
  Polynomial<Rational> zero;
  Vector<Polynomial<Rational> > theWeightSimpleCoords = theElt.owner->getSimpleCoordinatesFromFundamental(
    theWeight.weightFundamentalCoordS,
    zero
  );
  theElt.actOn(theWeightSimpleCoords);
  theWeight.weightFundamentalCoordS = theElt.owner->getFundamentalCoordinatesFromSimple(theWeightSimpleCoords);
  return output.assignValueWithContext(theWeight, inputConverted[2].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyEllipticCurveElements(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyEllipticCurveElements");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  ElementEllipticCurve<Rational> left, right;
  if (!input[1].isOfType(&left) || !input[2].isOfType(&right)) {
    return false;
  }
  if (!(left *= right)) {
    return false;
  }
  return output.assignValueWithContext(left, input[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyEllipticCurveElementsZmodP(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyEllipticCurveElements");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  ElementEllipticCurve<ElementZmodP> left, right;
  if (!input[1].isOfType(&left) || !input[2].isOfType(&right)) {
    return false;
  }
  if (!left.flagInfinity && !right.flagInfinity) {
    if (left.xCoordinate.modulus != right.xCoordinate.modulus) {
      return calculator << "Attempt to multiply elliptic curve elements over Z mod p "
      << "for different moduli p. Possible bad user input?";
    }
  }
  if (!(left *= right)) {
    return false;
  }
  return output.assignValueWithContext(left, input[1].getContext(), calculator);
}

template <>
bool Expression::convertInternally<ElementUniversalEnveloping<RationalFunction<Rational> > >(Expression& output) const;

bool CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.mergeContextsMyAruments(inputConverted, &calculator.comments)) {
    return false;
  }
  if (!inputConverted[2].isOfType<ElementTensorsGeneralizedVermas<RationalFunction<Rational> > >()) {
    return false;
  }
  SemisimpleLieAlgebra& theSSalg =
  inputConverted[2].getValue<ElementTensorsGeneralizedVermas<RationalFunction<Rational> > >().getOwnerSemisimple();
  Expression leftE;
  inputConverted[1].checkConsistency();
  input[1].checkConsistency();
  input[2].checkConsistency();
  if (!inputConverted[1].convertInternally<ElementUniversalEnveloping<RationalFunction<Rational> > >(leftE)) {
    return false;
  }
  const ElementTensorsGeneralizedVermas<RationalFunction<Rational> >& rightEltETGVM =
  inputConverted[2].getValue<ElementTensorsGeneralizedVermas<RationalFunction<Rational> > >();
  ElementTensorsGeneralizedVermas<RationalFunction<Rational> > outputElt;
  if (rightEltETGVM.isEqualToZero()) {
    output = inputConverted[2];
    return true;
  }
  theSSalg.orderNilradicalNilWeightAscending(rightEltETGVM.getOwnerModule().parabolicSelectionNonSelectedAreElementsLevi);
  theSSalg.flagHasNilradicalOrder = true;
  RationalFunction<Rational> one(Rational::one());
  if (!rightEltETGVM.multiplyOnTheLeft(
    leftE.getValue<ElementUniversalEnveloping<RationalFunction<Rational> > >(),
    outputElt,
    theSSalg,
    one
  )) {
    return false;
  }
  return output.assignValueWithContext(outputElt, inputConverted[2].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.mergeContextsMyArumentsAndConvertThem<ElementWeylAlgebra<Rational> >(
    inputContextsMerged, &calculator.comments
  )) {
    return false;
  }
  if (
    inputContextsMerged[1].getValue<ElementWeylAlgebra<Rational> >().hasNonSmallPositiveIntegerDerivation() ||
    inputContextsMerged[2].getValue<ElementWeylAlgebra<Rational> >().hasNonSmallPositiveIntegerDerivation()
  ) {
    return calculator << "<hr> Failed to multiply " << inputContextsMerged[1].toString()
    << " by " << inputContextsMerged[2].toString() << ": "
    << "one of the two differential operators has "
    << "differential operator exponent that is not a small integer. ";
  }
  ElementWeylAlgebra<Rational> result = inputContextsMerged[1].getValue<ElementWeylAlgebra<Rational> >();
  result *= inputContextsMerged[2].getValue<ElementWeylAlgebra<Rational> >();
  return output.assignValueWithContext(result, inputContextsMerged[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF");
  if (!CalculatorFunctionsBinaryOps::innerMultiplyTypeByType<RationalFunction<Rational> >(calculator, input, output)) {
    return false;
  }
  RationalFunction<Rational> simplified;
  if (!output.isOfType(&simplified)) {
    return true;
  }
  simplified.simplify();
  return output.assignValueWithContext(simplified, output.getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerDivideRationalFunctionOrPolynomialOrRationalByRationalFunctionOrPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDivideRationalFunctionOrPolynomialOrRationalByRationalFunctionOrPolynomial");
  if (!CalculatorFunctionsBinaryOps::innerDivideTypeByType<RationalFunction<Rational> >(calculator, input, output)) {
    return false;
  }
  RationalFunction<Rational> simplified;
  if (!output.isOfType(&simplified)) {
    return true;
  }
  simplified.simplify();
  return output.assignValueWithContext(simplified, output.getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrRFToRatOrPolyOrRF(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrRFToRatOrPolyOrRF");
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<RationalFunction<Rational> >(calculator, input, output);
}

bool CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrEWAToRatOrPolyOrEWA(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrEWAToRatOrPolyOrEWA");
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<ElementWeylAlgebra<Rational> >(calculator, input, output);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolynomialByNumberOrPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolynomialByNumberOrPolynomial");
  if (input.children.size < 3) {
    return false;
  }
  const Expression leftE = input[1];
  const Expression rightE = input[2];
  if (
    leftE.isOfType<AlgebraicNumber>() ||
    leftE.isOfType <Polynomial<AlgebraicNumber> >() ||
    rightE.isOfType<AlgebraicNumber>() ||
    rightE.isOfType<Polynomial<AlgebraicNumber> >()
  ) {
    return CalculatorFunctionsBinaryOps::innerMultiplyTypeByType<Polynomial<AlgebraicNumber> >(calculator, input, output);
  }
  return CalculatorFunctionsBinaryOps::innerMultiplyTypeByType<Polynomial<Rational> >(calculator, input, output);
}

bool CalculatorFunctionsBinaryOps::innerAddUEToAny(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddUEToAny");
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<ElementUniversalEnveloping<RationalFunction<Rational> > >(
    calculator, input, output
  );
}

bool CalculatorFunctionsBinaryOps::innerMultiplyAnyByUE(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyUEByAny");
  calculator.checkInputNotSameAsOutput(input, output);
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.mergeContextsMyArumentsAndConvertThem<ElementUniversalEnveloping<RationalFunction<Rational> > >(
    inputContextsMerged, &calculator.comments
  )) {
    return false;
  }
  ElementUniversalEnveloping<RationalFunction<Rational> > result = inputContextsMerged[1].getValue<ElementUniversalEnveloping<RationalFunction<Rational> > >();
  result *= inputContextsMerged[2].getValue<ElementUniversalEnveloping<RationalFunction<Rational> > >();
  result.simplify(RationalFunction<Rational>::oneRational());
  return output.assignValueWithContext(result, inputContextsMerged[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyLRObyLRO(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyLRObyLRO");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  output = input[1];
  Expression rightCopy = input[2];
  if (
    !output.isOfType<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >() ||
    !rightCopy.isOfType<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >()
  ) {
    return false;
  }
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& leftMon =
  output.getValue<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >();
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& rightMon =
  rightCopy.getValue<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >();
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> result;
  result = leftMon;
  result *= rightMon;
  for (int i = 0; i < result.generatorsIndices.size; i ++) {
    if (result.powers[i] > 100000 || result.powers[i] < 0) {
      return output.makeError(
        "The result of this operation is " + result.toString() + " which is outside of the allowed range. ",
        calculator
      );
    }
  }
  return output.assignValue(result, calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyLRObyLSPath(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyLRObyLSPath");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  output = input[1];
  Expression rightCopy = input[2];
  if (!output.mergeContexts(output, rightCopy)) {
    return false;
  }
  if (!output.isOfType<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >() || !rightCopy.isOfType<LittelmannPath>()) {
    return false;
  }
  LittelmannPath result = rightCopy.getValue<LittelmannPath>();
  WeylGroupData& theWeyl = *result.owner;
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> theLRO;
  theLRO = output.getValue<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >();
  for (int i = theLRO.generatorsIndices.size - 1; i >= 0; i --) {
    if (
      theLRO.generatorsIndices[i] == 0 ||
      theLRO.generatorsIndices[i] < - theWeyl.getDimension() ||
      theLRO.generatorsIndices[i] > theWeyl.getDimension()
    ) {
      std::stringstream out;
      out << " The Littelmann root operator must have an index whose absolute value "
      << "is between 1 and the rank of the ambient Lie algebra, instead I get index  "
      << theLRO.generatorsIndices[i];
      return output.makeError(out.str(), calculator);
    } else {
      for (int j = 0; j < theLRO.powers[i]; j ++) {
        result.actByEFDisplayIndex(theLRO.generatorsIndices[i]);
      }
    }
  }
  return output.assignValue(result, calculator);
}

bool CalculatorFunctionsBinaryOps::innerAddEltTensorToEltTensor(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddEltTensorToEltTensor");
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<ElementTensorsGeneralizedVermas<RationalFunction<Rational> > >(
    calculator, input, output
  );
}

bool CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial");
  if (input.size() < 3) {
    return false;
  }
  const Expression leftE = input[1];
  const Expression rightE = input[2];
  if (
    leftE.isOfType<AlgebraicNumber>() ||
    leftE.isOfType <Polynomial<AlgebraicNumber> >() ||
    rightE.isOfType<AlgebraicNumber>() ||
    rightE.isOfType<Polynomial<AlgebraicNumber> >()
  ) {
    return CalculatorFunctionsBinaryOps::innerAddTypeToType<Polynomial<AlgebraicNumber> >(calculator, input, output);
  }
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<Polynomial<Rational> >(calculator, input, output);
}

bool CalculatorFunctionsBinaryOps::innerAddPolynomialModuloPolynomialModuloIntegerToPolynomialModuloPolynomialModuloInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddPolynomialModuloPolynomialModuloIntegerToPolynomialModuloPolynomialModuloInteger");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.mergeContextsMyArumentsAndConvertThem<PolynomialModuloPolynomial<ElementZmodP> >(
    inputContextsMerged, &calculator.comments
  )) {
    return false;
  }
  PolynomialModuloPolynomial<ElementZmodP> left = inputContextsMerged[1].getValue<PolynomialModuloPolynomial<ElementZmodP> >();
  PolynomialModuloPolynomial<ElementZmodP> right = inputContextsMerged[2].getValue<PolynomialModuloPolynomial<ElementZmodP> >();
  if (!left.isEqualToZero() && !right.isEqualToZero()) {
    if (left.modulus != right.modulus) {
      return calculator << "Attempt to add polynomial quotients with different moduli. ";
    }
  }
  left += right;
  return output.assignValueWithContext(left, inputContextsMerged[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerAddPolynomialModuloIntegerToInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddPolynomialModuloIntegerToInteger");
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
      return calculator << "Attempt to add polynomial modulo "
      << modulus << " to integer modulo " << constant.modulus << ". ";
    }
  } else {
    return false;
  }
  polynomial += constant;
  return output.assignValueWithContext(polynomial, input[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerAddPolynomialModuloIntegerToPolynomialModuloInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddPolynomialModuloIntegerToPolynomialModuloInteger");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.mergeContextsMyArumentsAndConvertThem<Polynomial<ElementZmodP> >(
    inputContextsMerged, &calculator.comments
  )) {
    return false;
  }
  Polynomial<ElementZmodP> left = inputContextsMerged[1].getValue<Polynomial<ElementZmodP> >();
  Polynomial<ElementZmodP> right = inputContextsMerged[2].getValue<Polynomial<ElementZmodP> >();
  if (!left.isEqualToZero() && !right.isEqualToZero()) {
    if (left.coefficients[0].modulus != right.coefficients[0].modulus) {
      return calculator << "Attempt to add polynomials with different moduli. ";
    }
  }
  left += right;
  return output.assignValueWithContext(left, inputContextsMerged[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerDividePolynomialModuloIntegerByPolynomialModuloInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDividePolynomialModuloIntegerByPolynomialModuloInteger");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.mergeContextsMyArumentsAndConvertThem<Polynomial<ElementZmodP> >(
    inputContextsMerged, &calculator.comments
  )) {
    return false;
  }
  Polynomial<ElementZmodP> left = inputContextsMerged[1].getValue<Polynomial<ElementZmodP> >();
  Polynomial<ElementZmodP> right = inputContextsMerged[2].getValue<Polynomial<ElementZmodP> >();
  if (right.isEqualToZero()) {
    return output.makeError("Division by zero.", calculator);
  }
  LargeIntegerUnsigned modulus = right.coefficients[0].modulus;
  if (!left.isEqualToZero()) {
    if (left.coefficients[0].modulus != modulus) {
      return calculator
      << "Attempt to multiply polynomials with different moduli. ";
    }
  }
  if (modulus > ElementZmodP::maximumModulusForUserFacingPolynomialDivision) {
    return calculator << "Division of modular polynomials not "
    << "allowed for modulus larger than " << ElementZmodP::maximumModulusForUserFacingPolynomialDivision << ". ";
  }
  if (!modulus.isPossiblyPrime(0, true, &calculator.comments)) {
    return calculator
    << "Polynomial division is currently only implemented over prime moduli. ";
  }
  RationalFunction<ElementZmodP> result;
  result = left;
  result /= right;
  return output.assignValueWithContext(result, inputContextsMerged[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyPolynomialModPByPolynomialModP(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyPolynomialModPByPolynomialModP");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.mergeContextsMyArumentsAndConvertThem<Polynomial<ElementZmodP> >(
    inputContextsMerged, &calculator.comments
  )) {
    return false;
  }
  Polynomial<ElementZmodP> left = inputContextsMerged[1].getValue<Polynomial<ElementZmodP> >();
  Polynomial<ElementZmodP> right = inputContextsMerged[2].getValue<Polynomial<ElementZmodP> >();
  if (!left.isEqualToZero() && !right.isEqualToZero()) {
    if (left.coefficients[0].modulus != right.coefficients[0].modulus) {
      return calculator << "Attempt to multiply polynomials with different moduli. ";
    }
  }
  left *= right;
  return output.assignValueWithContext(left, inputContextsMerged[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyPolynomialModPolynomialModPToPolynomialModPolynomialModP(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyPolynomialModPolynomialModPToPolynomialModPolynomialModP");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.mergeContextsMyArumentsAndConvertThem<PolynomialModuloPolynomial<ElementZmodP> >(
    inputContextsMerged, &calculator.comments
  )) {
    return false;
  }
  PolynomialModuloPolynomial<ElementZmodP> left = inputContextsMerged[1].getValue<PolynomialModuloPolynomial<ElementZmodP> >();
  PolynomialModuloPolynomial<ElementZmodP> right = inputContextsMerged[2].getValue<PolynomialModuloPolynomial<ElementZmodP> >();
  if (left.modulus != right.modulus) {
    return calculator << "Attempt to multiply polynomial quotients with different moduli. ";
  }
  left *= right;
  return output.assignValueWithContext(left, inputContextsMerged[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerAddPlotToPlot(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddPlotToPlot");
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
    leftPlot. dimension != - 1 &&
    rightPlot.dimension != - 1 &&
    leftPlot. dimension != rightPlot.dimension
  ) {
    return false;
  }
  leftPlot += rightPlot;
  return output.assignValue(leftPlot, calculator);
}

bool CalculatorFunctionsBinaryOps::innerPowerPolynomialBySmallInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerPolynomialBySmallInteger");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Polynomial<Rational> base;
  int thePower = 0;
  if (!input[1].isOfType(&base) || !input[2].isSmallInteger(&thePower)) {
    return false;
  }
  if (base.isEqualToZero() && thePower <= 0) {
    return output.makeError("Division by zero: trying to raise 0 to negative power. ", calculator);
  }
  if (thePower < 0) {
    if (base.size() == 1) {
      Polynomial<Rational> outputPoly;
      MonomialP theMon = base[0];
      Rational theCF = base.coefficients[0];
      theCF.raiseToPower(thePower);
      theMon.raiseToPower(thePower);
      outputPoly.makeZero();
      outputPoly.addMonomial(theMon, theCF);
      return output.assignValueWithContext(outputPoly, input[1].getContext(), calculator);
    }
    base.raiseToPower(- thePower, 1);
    RationalFunction<Rational> theRF;
    theRF = base;
    theRF.invert();
    return output.assignValueWithContext(theRF, input[1].getContext(), calculator);
  }
  base.raiseToPower(thePower, 1);
  return output.assignValueWithContext(base, input[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerPowerMatrixNumbersByLargeIntegerIfPossible(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerMatrixNumbersByLargeIntegerIfPossible");
  if (!input.startsWith(calculator.opThePower(), 3)) {
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
  if (calculator.functionGetMatrix(matrixE, baseRat)) {
    if (!baseRat.isSquare() || baseRat.numberOfColumns == 0) {
      std::stringstream errorStream;
      errorStream << "Exponentiating non-square matrices or matrices with zero rows is not allowed. "
      << "Your matrix, " << baseRat.toString() << " is not square. ";
      return output.makeError(errorStream.str(), calculator);
    }
    Rational theDet = baseRat.getDeterminant();
    if (largePower <= 0) {
      if (theDet == 0) {
        return output.makeError("Division by zero: trying to raise 0 to negative power. ", calculator);
      }
    }
    if (theDet != 0 && theDet != - 1 && theDet != 1) {
      return calculator << "Matrix power too large.";
    }
    if (largePower < 0) {
      baseRat.invert();
      largePower *= - 1;
    }
    Matrix<Rational> idMat;
    idMat.makeIdentityMatrix(baseRat.numberOfRows, Rational::one(), Rational::zero());
    MathRoutines::raiseToPower(baseRat, largePower, idMat);
    return output.assignMatrix(baseRat, calculator);
  }
  Matrix<AlgebraicNumber> baseAlg;
  if (calculator.functionGetMatrix(matrixE, baseAlg)) {
    if (!baseAlg.isSquare() || baseAlg.numberOfColumns == 0) {
      std::stringstream errorStream;
      errorStream << "Exponentiating non-square matrices or matrices with zero rows is not allowed. "
      << "Your matrix, " << baseAlg.toString() << " is not square. ";
      return output.makeError(errorStream.str(), calculator);
    }
    AlgebraicNumber theDet = baseAlg.getDeterminant();
    if (largePower <= 0) {
      if (theDet == 0) {
        return output.makeError("Division by zero: trying to raise 0 to negative power. ", calculator);
      }
    }
    if (theDet != 0 && theDet != - 1 && theDet != 1) {
      return calculator << "Matrix power too large.";
    }
    if (largePower < 0) {
      baseAlg.invert();
      largePower *= - 1;
    }
    Matrix<AlgebraicNumber> idMat;
    AlgebraicNumber one = calculator.theObjectContainer.theAlgebraicClosure.one();
    AlgebraicNumber zero = calculator.theObjectContainer.theAlgebraicClosure.zero();

    idMat.makeIdentityMatrix(baseAlg.numberOfRows, one, zero);
    MathRoutines::raiseToPower(baseAlg, largePower, idMat);
    return output.assignMatrix(baseAlg, calculator);
  }  
  return false;
}

bool CalculatorFunctionsBinaryOps::innerPowerMatrixNumbersBySmallInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerMatrixNumbersBySmallInteger");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  const Expression& matrixE = input[1];
  if (!matrixE.isMatrix()) {
    return false;
  }
  int thePower = 0;
  bool powerIsSmall = input[2].isSmallInteger(&thePower);
  if (!powerIsSmall) {
    return false;
  }
  Matrix<Rational> baseRat;
  if (calculator.functionGetMatrix(matrixE, baseRat)) {
    if (!baseRat.isSquare() || baseRat.numberOfColumns == 0) {
      std::stringstream errorStream;
      errorStream << "Exponentiating non-square matrices or matrices with zero rows is not allowed. "
      << "Your matrix, " << baseRat.toString() << " is not square. ";
      return output.makeError(errorStream.str(), calculator);
    }
    if (thePower <= 0) {
      if (baseRat.getDeterminant() == 0) {
        return output.makeError("Division by zero: trying to raise 0 to negative power. ", calculator);
      }
    }
    if (thePower < 0) {
      baseRat.invert();
      thePower *= - 1;
    }
    Matrix<Rational> idMat;
    idMat.makeIdentityMatrix(baseRat.numberOfRows, Rational::one(), Rational::zero());
    MathRoutines::raiseToPower(baseRat, thePower, idMat);
    return output.assignMatrix(baseRat, calculator);
  }
  Matrix<AlgebraicNumber> baseAlg;
  AlgebraicNumber one = calculator.theObjectContainer.theAlgebraicClosure.one();
  AlgebraicNumber zero = calculator.theObjectContainer.theAlgebraicClosure.zero();
  if (calculator.functionGetMatrix(matrixE, baseAlg)) {
    if (!baseAlg.isSquare() || baseAlg.numberOfColumns == 0) {
      return output.makeError("Exponentiating non-square matrices or matrices with zero rows is not allowed.", calculator);
    }
    if (thePower <= 0) {
      if (baseAlg.getDeterminant() == 0) {
        return output.makeError("Division by zero: trying to raise 0 to negative power. ", calculator);
      }
    }
    if (thePower < 0) {
      baseAlg.invert();
      thePower *= - 1;
    }
    Matrix<AlgebraicNumber> idMat;
    idMat.makeIdentityMatrix(
      baseAlg.numberOfRows, one, zero
    );
    MathRoutines::raiseToPower(baseAlg, thePower, idMat);
    return output.assignMatrix(baseAlg, calculator);
  }
  Matrix<RationalFunction<Rational> > baseRF;
  ExpressionContext theContext(calculator);
  if (calculator.functionGetMatrix(matrixE, baseRF, &theContext)) {
    if (!baseRF.isSquare() || baseRF.numberOfColumns == 0) {
      return output.makeError(
        "Exponentiating non-square matrices or matrices "
        "with zero rows is not allowed.",
        calculator
      );
    }
    if (thePower <= 0) {
      if (baseRF.getDeterminant() == 0) {
        return output.makeError(
          "Division by zero: trying to raise 0 to negative power. ",
          calculator
        );
      }
    }
    if (thePower < 0) {
      return calculator
      << "Raising matrices of rational functions to "
      << "negative powers not implemented yet. ";
    }
    Matrix<RationalFunction<Rational> > idMat;
    idMat.makeIdentityMatrix(baseRF.numberOfRows, RationalFunction<Rational>::oneRational(), RationalFunction<Rational>::zeroRational());
    MathRoutines::raiseToPower(baseRF, thePower, idMat);
    return output.assignMatrix(baseRF, calculator, &theContext);
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberPolynomialBySmallInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberPolynomialBySmallInteger");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Polynomial<AlgebraicNumber> base;
  int thePower = 0;
  if (!input[1].isOfType(&base)|| !input[2].isSmallInteger(&thePower)) {
    return false;
  }
  if (thePower < 0) {
    return false;
  }
  if (base.isEqualToZero() && thePower <= 0) {
    return output.makeError("Division by zero: trying to raise 0 to negative power. ", calculator);
  }
  base.raiseToPower(thePower, 1);
  return output.assignValueWithContext(base, input[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerPowerPolynomialModuloIntegerBySmallInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerPolynomialModuloIntegerBySmallInteger");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Polynomial<ElementZmodP> base;
  int thePower = 0;
  if (!input[1].isOfType(&base)|| !input[2].isSmallInteger(&thePower)) {
    return false;
  }
  if (thePower < 0) {
    return false;
  }
  if (base.isEqualToZero()) {
    if (thePower < 0) {
      return output.makeError("Division by zero: trying to raise 0 to negative power. ", calculator);
    }
    if (thePower == 0) {
      return false;
    }
    return output.assignValueWithContext(base, input[1].getContext(), calculator);
  }
  base.raiseToPower(thePower, base.coefficients[0].one());
  return output.assignValueWithContext(base, input[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerPowerPolynomialModPModuloPolynomialModPBySmallInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerPolynomialModPModuloPolynomialModPBySmallInteger");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  PolynomialModuloPolynomial<ElementZmodP> base;
  LargeIntegerUnsigned thePower = 0;
  if (!input[1].isOfType(&base) || !input[2].isIntegerNonNegative(&thePower)) {
    return false;
  }
  if (base.isEqualToZero()) {
    if (thePower < 0) {
      return output.makeError("Division by zero: trying to raise 0 to negative power. ", calculator);
    }
    if (thePower == 0) {
      return false;
    }
    return output.assignValueWithContext(base, input[1].getContext(), calculator);
  }
  MathRoutines::raiseToPower(base, thePower, base.one());
  return output.assignValueWithContext(base, input[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerRadicalAlgebraicNumberPositiveDefault(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerRadicalAlgebraicNumberPositiveDefault");
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
    << maximumRadicalAllowed << ". ";
    return false;
  }
  if (powerIntegral > LargeIntegerUnsigned::SquareRootOfCarryOverBound) {
    // power too large
    return false;
  }
  AlgebraicClosureRationals fieldCopy;
  fieldCopy = calculator.theObjectContainer.theAlgebraicClosure;
  AlgebraicNumber baseCopy;
  baseCopy = base;
  baseCopy.owner = &fieldCopy;
  if (!baseCopy.radicalMeDefault(radical, &calculator.comments)) {
    return false;
  }
  if (fieldCopy.basisInjections.size != calculator.theObjectContainer.theAlgebraicClosure.basisInjections.size) {
    // the radical does not below to the base field;
    return false;
  }
  baseCopy.owner = &calculator.theObjectContainer.theAlgebraicClosure;
  MathRoutines::raiseToPower(baseCopy, powerIntegral, baseCopy.owner->one());
  return output.assignValue(baseCopy, calculator);
}

bool CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberBySmallInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberBySmallInteger");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  AlgebraicNumber base;
  if (!input[1].isOfType(&base)) {
    return false;
  }
  Rational powerRat, baseRat;
  if (input[2].isRational(&powerRat)) {
    if (powerRat.getDenominator() == 2) {
      if (base.isRational(&baseRat)) {
        if (base.assignRationalQuadraticRadical(
          baseRat,
          calculator.theObjectContainer.theAlgebraicClosure,
          &calculator.comments
        )) {
          base.checkConsistency();
          output = input;
          Expression newPower, newBase;
          newPower.assignValue(powerRat * 2, calculator);
          newBase.assignValue(base, calculator);
          return output.makeXOX(
            calculator,
            calculator.opThePower(),
            newBase,
            newPower
          );
        }
      }
    }
  }
  int thePower = 0;
  if (!input[2].isSmallInteger(&thePower)) {
    return false;
  }
  if (base.isEqualToZero() && thePower < 0) {
    return output.makeError("Division by zero: trying to raise 0 to negative power. ", calculator);
  }
  if (base.isEqualToZero() && thePower == 0) {
    return output.assignValue(1, calculator);
  }
  if (thePower < 0) {
    thePower *= - 1;
    base.invert();
  }
  MathRoutines::raiseToPower(base, thePower, AlgebraicNumber(1));
  return output.assignValueWithContext(base, input[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerPowerEWABySmallInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerEWABySmallInteger");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  ElementWeylAlgebra<Rational> base;
  int thePower = 0;
  if (!input[1].isOfType(&base)) {
    return false;
  }
  bool mustCheckForRationalPower = false;
  mustCheckForRationalPower = !input[2].isSmallInteger(&thePower);
  if (mustCheckForRationalPower || thePower < 0) {
    Rational powerRat;
    if (!input[2].isOfType<Rational>(&powerRat)) {
      return false;
    }
    bool isMon = true;
    if (base.size() != 1) {
      isMon = false;
    } else if (base.coefficients[0] != 1) {
      isMon = false;
    }
    if (!isMon) {
      return calculator << "<hr>Failed to raise " << base.toString() << " to power "
      << powerRat.toString() << ": the exponent is not a "
      << "small integer and the base is not a coefficient one monomial. ";
    }
    ElementWeylAlgebra<Rational> finalOutput;
    MonomialWeylAlgebra theMon = base[0];
    theMon.polynomialPart.raiseToPower(powerRat);
    theMon.differentialPart.raiseToPower(powerRat);
    for (int i = 0; i < theMon.polynomialPart.minimalNumberOfVariables(); i ++) {
      if (theMon.polynomialPart(i) != 0 && theMon.differentialPart(i) != 0) {
        return calculator << "<hr>Failed to raise " << base.toString() << " to power "
        << powerRat.toString() << ": the exponent is not a "
        << "small integer, the base is a monomial, however the monomial "
        << "contains derivative and polynomial with respect to the same variable. ";
      }
    }
    finalOutput.makeZero();
    finalOutput.addMonomial(theMon, 1);
    return output.assignValueWithContext(finalOutput, input[1].getContext(), calculator);
  }

  if (base.isEqualToZero()) {
    if (thePower < 0) {
      return output.makeError("Division by zero: trying to raise 0 to negative power. ", calculator);
    }
    if (thePower == 0) {
      return output.assignValue(1, calculator);
    }
  }
  base.raiseToPower(thePower);
  return output.assignValueWithContext(base, input[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerPowerRationalByInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerRationalByInteger");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational base, exp;
  if (!input[1].isRational(&base)) {
    return false;
  }
  if (base.isEqualToOne()) {
    return output.assignValue(1, calculator);
  }
  if (!input[2].isRational(&exp)) {
    return false;
  }
  int thePower;
  if (!exp.isSmallInteger(&thePower)) {
    return false;
  }
  if (base == 0 && thePower == 0) {
    return output.assignValue(1, calculator);
  }
  if (base == 0 && thePower < 0) {
    return output.makeError("Division by zero: trying to raise 0 to negative or zero power. ", calculator);
  }
  base.raiseToPower(thePower);
  return output.assignValue(base, calculator);
}

bool CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.mergeContextsMyAruments(inputConverted, &calculator.comments)) {
    return false;
  }
  ElementUniversalEnveloping<RationalFunction<Rational> > theUE;
  Expression copyExponent = inputConverted[2];
  Expression copyBase = inputConverted[1];
  if (!copyBase.isOfType<ElementUniversalEnveloping<RationalFunction<Rational> > >(&theUE)) {
    return false;
  }
  if (!theUE.isPowerOfSingleGenerator()) {
    int tempPower;
    if (!copyExponent.isSmallInteger(&tempPower)) {
      return false;
    }
    theUE.raiseToPower(tempPower);
    return output.assignValueWithContext(theUE, copyBase.getContext(), calculator);
  }
  Expression exponentConverted;
  if (!copyExponent.convertInternally<RationalFunction<Rational> >(exponentConverted)) {
    return false;
  }
  MonomialUniversalEnveloping<RationalFunction<Rational> > theMon;
  theMon = theUE[0];
  theMon.powers[0] *= exponentConverted.getValue<RationalFunction<Rational> >();
  ElementUniversalEnveloping<RationalFunction<Rational> > outputUE;
  outputUE.makeZero(*theUE.owner);
  outputUE.addMonomial(theMon, 1);
  return output.assignValueWithContext(outputUE, copyBase.getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerPowerMatrixExpressionsBySmallInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerMatrixExpressionsBySmallInteger");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  if (!input[1].isMatrix()) {
    return false;
  }
  int thePower = 0;
  if (!input[2].isSmallInteger(&thePower)) {
    return false;
  }
  if (thePower <= 0) {
    return false;
  }
  Matrix<Expression> theMat;
  if (!calculator.getMatrixExpressions(input[1], theMat)) {
    return false;
  }
  if (!theMat.isSquare()) {
    return output.makeError("Attempting to raise non-square matrix to power", calculator);
  }
  LargeInteger expectedNumTerms;
  expectedNumTerms = theMat.numberOfColumns;
  expectedNumTerms.raiseToPower(thePower);
  if (expectedNumTerms > 10000) {
    return calculator
    << "The expected number terms in the result of the exponentiation "
    << theMat.toString() << " to the power of "
    << thePower << " is approximately ("
    << theMat.numberOfColumns << ")^" << thePower
    << "=" << expectedNumTerms
    << ". I have been instructed to proceed only "
    << "if the expected number of terms is fewer than 10000. ";
  }
  Matrix<Expression> idMatE;
  idMatE.makeIdentityMatrix(theMat.numberOfRows, calculator.expressionOne(), calculator.expressionZero());
  MathRoutines::raiseToPower(theMat, thePower, idMatE);
  return output.assignMatrixExpressions(theMat, calculator, true, true);
}

bool CalculatorFunctionsBinaryOps::innerPowerRationalByRationalReducePrimeFactors(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerRationalByRationalReducePrimeFactors");
  if (!input.startsWith(calculator.opThePower(), 3)) {
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
      return output.makeError("Division by zero while evaluating " + input.toString(), calculator);
    }
    return output.assignValue(0, calculator);
  }
  if (!base.isInteger()) {
    if (base.getNumerator() == 1) {
      Expression theDenBase, theDenominator;
      theDenBase.assignValue(Rational(base.getDenominator()), calculator);
      theDenominator.makeXOX(calculator, calculator.opThePower(), theDenBase, input[2]);
      output = calculator.expressionOne() / theDenominator;
      return true;
    }
  }
  exponentWorking = exponentStarting;
  if (exponentWorking.getDenominator() == 2) {
    return false;
  }
  LargeIntegerUnsigned exponentDenominator = exponentWorking.getDenominator();
  LargeIntegerUnsigned exponentNumeratorNoSign = exponentWorking.getNumerator().value;
  List<LargeInteger> numeratorFactors, denominatorFactors;
  List<int> numeratorPowersInt, denominatorPowersInt;
  if (!base.getPrimeFactorsAbsoluteValue(
    numeratorFactors,
    numeratorPowersInt,
    denominatorFactors,
    denominatorPowersInt
  )) {
    return false;
  }
  List<LargeIntegerUnsigned> numeratorPowers, denominatorPowers;
  numeratorPowers = numeratorPowersInt;
  denominatorPowers = denominatorPowersInt;
  for (int i = 0; i < numeratorFactors.size; i ++) {
    numeratorPowers[i] *= exponentNumeratorNoSign;
  }
  for (int i = 0; i < denominatorFactors.size; i ++) {
    denominatorPowers[i] *= exponentNumeratorNoSign;
  }
  exponentWorking /= exponentNumeratorNoSign;
  Rational outsideOfTheRadical = 1;
  LargeInteger currentInsidePower, currentOutsidePower, currentOutside;
  LargeIntegerUnsigned currentPower;
  int currentInsidePowerInt = - 1, currentOutsidePowerInt = - 1;
  for (int k = 0; k < 2; k ++) {
    List<LargeIntegerUnsigned>& currentPowers = (k == 0) ? numeratorPowers : denominatorPowers;
    List<LargeInteger>& currentFactors = (k == 0) ? numeratorFactors : denominatorFactors;
    for (int i = 0; i < currentFactors.size; i ++) {
      currentPower = currentPowers[i];
      currentInsidePower = currentPower % exponentDenominator;
      currentOutsidePower = currentPower / exponentDenominator;
      if (!currentInsidePower.isIntegerFittingInInt(&currentInsidePowerInt)) {
        return false;
      }
      if (!currentOutsidePower.isIntegerFittingInInt(&currentOutsidePowerInt)) {
        return false;
      }
      if (
        currentInsidePowerInt > 1000 ||
        currentInsidePowerInt < - 1000 ||
        currentOutsidePowerInt > 1000 ||
        currentOutsidePowerInt < - 1000
      ) {
        return false;
      }
      currentPowers[i] = static_cast<unsigned>(currentInsidePowerInt);
      currentOutside = currentFactors[i];
      currentOutside.raiseToPower(currentOutsidePowerInt);
      if (k == 0) {
        outsideOfTheRadical *= currentOutside;
      } else {
        outsideOfTheRadical /= currentOutside;
      }
    }
  }
  LargeIntegerUnsigned theGCD = 1;
  if (numeratorPowers.size > 0) {
    theGCD = numeratorPowers[0];
  } else if (denominatorPowers.size > 0) {
    theGCD = denominatorPowers[0];
  }
  for (int i = 0; i < numeratorPowers.size; i ++) {
    theGCD = MathRoutines::greatestCommonDivisor(theGCD, numeratorPowers[i]);
  }
  for (int i = 0; i < denominatorPowers.size; i ++) {
    theGCD = MathRoutines::greatestCommonDivisor(theGCD, denominatorPowers[i]);
  }
  if (theGCD > 0) {
    for (int i = 0; i < numeratorPowers.size; i ++) {
      numeratorPowers[i] /= theGCD;
    }
    for (int i = 0; i < denominatorPowers.size; i ++) {
      denominatorPowers[i] /= theGCD;
    }
    exponentWorking *= theGCD;
    exponentDenominator = exponentWorking.getDenominator();
  }
  Rational insideTheRadical = 1;
  LargeInteger currentContribution, currentNumerator = 1, currentDenominator = 1;
  int currentExpSmallInt = - 1;
  for (int i = 0; i < numeratorPowers.size; i ++) {
    currentContribution = numeratorFactors[i];
    if (!numeratorPowers[i].isIntegerFittingInInt(&currentExpSmallInt)) {
      return false;
    }
    currentContribution.raiseToPower(currentExpSmallInt);
    currentNumerator *= currentContribution;
  }
  for (int i = 0; i < denominatorPowers.size; i ++) {
    currentContribution = denominatorFactors[i];
    if (!denominatorPowers[i].isIntegerFittingInInt(&currentExpSmallInt)) {
      return false;
    }
    currentContribution.raiseToPower(currentExpSmallInt);
    currentDenominator *= currentContribution;
  }
  insideTheRadical = currentNumerator;
  insideTheRadical /= currentDenominator;
  if (base < 0) {
    exponentDenominator = exponentWorking.getDenominator();//<-just in case
    if (exponentDenominator % 2 == 1) {
      outsideOfTheRadical *= - 1;
    } else {
      insideTheRadical *= - 1;
    }
  }
  if (
    exponentStarting == exponentWorking &&
    outsideOfTheRadical == 1 &&
    base == insideTheRadical
  ) {
    return false;
  }
  Expression insideTheRadicalE, theRadicalE, theRadicalCFE, exponentE;
  if (exponentWorking < 0) {
    outsideOfTheRadical.invert();
  }
  theRadicalCFE.assignValue(outsideOfTheRadical, calculator);
  insideTheRadicalE.assignValue(insideTheRadical, calculator);
  exponentE.assignValue(exponentWorking, calculator);
  theRadicalE.makeXOX(calculator, calculator.opThePower(), insideTheRadicalE, exponentE);
  return output.makeProduct(calculator, theRadicalCFE, theRadicalE);
}

bool CalculatorFunctionsBinaryOps::innerPowerDoubleOrRationalToDoubleOrRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerDoubleOrRationalToDoubleOrRational");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational base, exp;
  double baseDouble, expDouble;
  if (input[1].isRational(&base)) {
    baseDouble = base.getDoubleValue();
  } else if (!input[1].isOfType(&baseDouble)) {
    return false;
  }
  if (input[2].isRational(&exp)) {
    expDouble = exp.getDoubleValue();
  } else if (!input[2].isOfType(&expDouble)) {
    return false;
  }
  if (baseDouble < 0) {
    if (!input[2].isRational()) {
      return false;
    }
    int thePower = 0;
    if (exp.isEven()) {
      if (!exp.isSmallInteger(&thePower)) {
        return false;
      }
      return output.assignValue(FloatingPoint::power(- baseDouble, thePower), calculator);
    }
    baseDouble *= - 1;
    return output.assignValue(- FloatingPoint::power(baseDouble, expDouble), calculator);
  }
  if (baseDouble == 0.0) {
    if (expDouble > 0) {
      return output.assignValue<double>(0, calculator);
    }
  }
  return output.assignValue(FloatingPoint::power(baseDouble, expDouble), calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyDoubleOrRationalByDoubleOrRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyDoubleOrRationalByDoubleOrRational");
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
  return output.assignValue(leftD * rightD, calculator);
}

bool CalculatorFunctionsBinaryOps::innerAddDoubleOrRationalToDoubleOrRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddDoubleOrRationalToDoubleOrRational");
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
  return output.assignValue(leftD + rightD, calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyCharSSLieAlgByCharSSLieAlg(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyCharSSLieAlgByCharSSLieAlg");
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
    return calculator << "You asked me to multiply characters over "
    << "different semisimple Lie algebras. Could this be a typo?";
  }
  std::string successString = (leftC *= rightC);
  if (successString != "") {
    return calculator << "I tried to multiply character "
    << leftC.toString() << " by " << rightC.toString()
    << " but I failed with the following message: " << successString;
  }
  return output.assignValue(leftC, calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarByMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarByMatrix");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  const Expression& theScalarE = input[1];
  if (!theScalarE.isBuiltInScalar()) {
    return false;
  }
  const Expression& theMatE = input[2];
  if (!theMatE.isMatrix()) {
    return false;
  }
  Matrix<Expression> theMat;
  if (!calculator.getMatrixExpressions(theMatE, theMat)) {
    return false;
  }
  for (int i = 0; i < theMat.numberOfRows; i ++) {
    for (int j = 0; j < theMat.numberOfColumns; j ++) {
      theMat(i, j) = theScalarE * theMat(i, j);
    }
  }
  return output.assignMatrixExpressions(theMat, calculator, false, true);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence");
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
  output.children.reserve(input[2].size());
  output.addChildAtomOnTop(calculator.opSequence());
  Expression tempProduct;
  for (int i = 1; i < input[2].size(); i ++) {
    tempProduct.makeProduct(calculator, input[1], input[2][i]);
    output.addChildOnTop(tempProduct);
  }
  return true;
}

bool CalculatorFunctionsBinaryOps::innerMultiplyMatrixByMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyMatrixByMatrix");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  int numRowsLeft = - 1;
  int numColsLeft = - 1;
  int numRowsRight = - 1;
  int numColsRight = - 1;
  const Expression& left = input[1];
  const Expression& right = input[2];
  if (
    !left.isMatrix(&numRowsLeft, &numColsLeft) ||
    !input[2].isMatrix(&numRowsRight, &numColsRight)
  ) {
    return false;
  }
  if (numColsLeft != numRowsRight) {
    return calculator << "WARNING: I encountered a product of a matrix with " << numColsLeft
    << " columns by a matrix with " << numRowsRight << " rows. ";
  }
  if (
    left.isMatrixOfType<Rational>() &&
    right.isMatrixOfType<Rational>()
  ) {
    return CalculatorFunctionsBinaryOps::innerMultiplyMatrixRationalOrRationalByMatrixRational(
      calculator, input, output
    );
  }
  bool invokeAlgMatMultiplication =
  (left.isMatrixOfType<Rational>()        && right.isMatrixOfType<AlgebraicNumber>()) ||
  (left.isMatrixOfType<AlgebraicNumber>() && right.isMatrixOfType<Rational>()) ||
  (left.isMatrixOfType<AlgebraicNumber>() && right.isMatrixOfType<AlgebraicNumber>());
  if (invokeAlgMatMultiplication) {
    return CalculatorFunctionsBinaryOps::innerMultiplyMatRatOrMatAlgByMatRatOrMatAlg(calculator, input, output);
  }
  bool invokeRFMultiplication =
  (left.isMatrixOfType<RationalFunction<Rational> >() && right.isMatrixOfType<Rational>()) ||
  (left.isMatrixOfType<Rational>()         && right.isMatrixOfType<RationalFunction<Rational> >()) ||
  (left.isMatrixOfType<RationalFunction<Rational> >() && right.isMatrixOfType<RationalFunction<Rational> >() );
  if (invokeRFMultiplication) {
    return CalculatorFunctionsBinaryOps::innerMultiplyMatrixRFOrRFByMatrixRF(
      calculator, input, output
    );
  }
  Matrix<Expression> leftMatrix, rightMatrix;
  if (!calculator.getMatrixExpressions(left, leftMatrix)) {
    return false;
  }
  if (!calculator.getMatrixExpressions(input[2], rightMatrix, leftMatrix.numberOfColumns)) {
    return false;
  }
  if (leftMatrix.numberOfColumns != rightMatrix.numberOfRows) {
    return false;
  }
  Matrix<Expression> outputMat;
  outputMat.initialize(leftMatrix.numberOfRows, rightMatrix.numberOfColumns);
  Expression leftSummand, rightSummand;
  for (int i = 0; i < leftMatrix.numberOfRows; i ++) {
    for (int j = 0; j < rightMatrix.numberOfColumns; j ++) {
      for (int k = 0; k < leftMatrix.numberOfColumns; k ++) {
        if (k == 0) {
          outputMat(i, j).makeProduct(calculator, leftMatrix(i, k), rightMatrix(k, j));
        } else {
          rightSummand = outputMat(i, j);
          leftSummand.makeProduct(calculator, leftMatrix(i, k), rightMatrix(k, j));
          outputMat(i, j).makeXOX(calculator, calculator.opPlus(), leftSummand, rightSummand);
        }
      }
    }
  }
  return output.assignMatrixExpressions(outputMat, calculator, true, true);
}

bool CalculatorFunctionsBinaryOps::innerMultiplySequenceByMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyMatrixByMatrix");
  if (!input.startsWith(calculator.opTimes())) {
    return false;
  }
  int matRows = - 1, matCols = - 1;
  if (!input[1].isSequenceNElements() || !input[2].isMatrix(&matRows, &matCols)) {
    return false;
  }
  if (input[1].size() - 1 != matRows) {
    return false;
  }
  List<Expression> theResult;
  theResult.setSize(matCols);
  for (int i = 0; i < theResult.size; i ++) {
    for (int j = 0; j < matRows; j ++) {
      if (j == 0) {
        theResult[i] = input[1][j + 1] * input[2][j + 1][i + 1];
      } else {
        theResult[i] += input[1][j + 1] * input[2][j + 1][i + 1];
      }
    }
  }
  return output.makeSequence(calculator, &theResult);
}

bool CalculatorFunctionsBinaryOps::tensorMatrixByMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input.startsWith(calculator.opTensor(), 3)) {
    return false;
  }
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::tensorMatrixByMatrix");
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if ((!rightE.isMatrix() && !rightE.isSequenceNElements()) || (!leftE.isMatrix() && !leftE.isSequenceNElements())) {
    return false;
  }
  Matrix<Rational> leftMatRat, rightMatRat;
  if (
    calculator.functionGetMatrix(leftE, leftMatRat) &&
    calculator.functionGetMatrix(rightE, rightMatRat)
  ) {
    Matrix<Rational> resultMatRat;
    resultMatRat.assignTensorProduct(leftMatRat, rightMatRat);
    return output.assignMatrix(resultMatRat, calculator, nullptr, true);
  }
  Matrix<Expression> leftMatE, rightMatE, resultMatE;
  if (!calculator.getMatrixExpressions(input[1], leftMatE)) {
    return false;
  }
  if (!calculator.getMatrixExpressions(input[2], rightMatE)) {
    return false;
  }
  resultMatE.assignTensorProduct(leftMatE, rightMatE);
  return output.assignMatrixExpressions(resultMatE, calculator, true, true);
}

bool CalculatorFunctionsBinaryOps::tensorMatrixByMatrixTensor(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input.startsWith(calculator.opTensor())) {
    return false;
  }
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::tensorMatrixByMatrixTensor");
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!rightE.isOfType<MatrixTensor<Rational> >() || !leftE.isOfType<MatrixTensor<Rational> >()) {
    return false;
  }
  MatrixTensor<Rational> result;
  result.assignTensorProduct(leftE.getValue<MatrixTensor<Rational> >(), rightE.getValue<MatrixTensor<Rational> >());
  return output.assignValue(result, calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRatOrAlgebraicByMatRatOrMatAlg(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRatOrAlgebraicByMatRatOrMatAlg");
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression* matE;
  const Expression* scalarE;
  matE = &input[1];
  scalarE = &input[2];
  if (
    !matE->isMatrixOfType<Rational>() &&
    !matE->isMatrixOfType<AlgebraicNumber>()
  ) {
    MathRoutines::swap(matE, scalarE);
  }
  if (
    !matE->isMatrixOfType<Rational>() &&
    !matE->isMatrixOfType<AlgebraicNumber>()
  ) {
    return false;
  }
  Matrix<AlgebraicNumber> theMatAlg;
  AlgebraicNumber theScalar;
  Matrix<Rational> theMatRat;
  Rational theScalarRat;
  if (calculator.functionGetMatrix(*matE, theMatRat)) {
    theMatAlg = theMatRat;
  } else {
    if (!calculator.functionGetMatrix(*matE, theMatAlg)) {
      return false;
    }
  }
  if (scalarE->isOfType(&theScalarRat)) {
    theScalar = theScalarRat;
  } else {
    if (!scalarE->isOfType(&theScalar)) {
      return false;
    }
  }
  theMatAlg *= theScalar;
  return output.assignMatrix(theMatAlg, calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyMatRatOrMatAlgByMatRatOrMatAlg(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyMatRatOrMatAlgByMatRatOrMatAlg");
  if (!input.isListNElements(3)) {
    return false;
  }
  Matrix<AlgebraicNumber> matAlgLeft, matAlgRight;
  Matrix<Rational> matRatLeft, matRatRight;
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (leftE.isMatrixOfType<Rational>() && rightE.isMatrixOfType<Rational>()) {
    return false;
  }
  if (calculator.functionGetMatrix(leftE, matRatLeft)) {
    matAlgLeft = matRatLeft;
  } else {
    if (!calculator.functionGetMatrix(leftE, matAlgLeft)) {
      return false;
    }
  }
  if (calculator.functionGetMatrix(rightE, matRatRight)) {
    matAlgRight = matRatRight;
  } else {
    if (!calculator.functionGetMatrix(rightE, matAlgRight)) {
      return false;
    }
  }
  if (matAlgLeft.numberOfColumns != matAlgRight.numberOfRows) {
    std::stringstream errorStream;
    errorStream << "Error: attempting to multiply matrix with "
    << matAlgLeft.numberOfColumns << " columns by a "
    << "matrix with " << matAlgRight.numberOfRows << " rows. ";
    return output.makeError(errorStream.str(), calculator);
  }
  matAlgLeft *= matAlgRight;
  return output.assignMatrix(matAlgLeft, calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyMatrixRationalOrRationalByMatrixRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyMatrixRationalOrRationalByMatrixRational");
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!rightE.isMatrixOfType<Rational>()) {
    return false;
  }
  Matrix<Rational> leftMatrix, rightMatrix;
  if (!calculator.functionGetMatrix(rightE, rightMatrix)) {
    return false;
  }
  Rational theScalar;
  if (leftE.isOfType<Rational>(&theScalar)) {
    leftMatrix *= theScalar;
    return output.assignMatrix(leftMatrix, calculator);
  }
  if (!leftE.isMatrixOfType<Rational>()) {
    return false;
  }
  if (!calculator.functionGetMatrix(leftE, rightMatrix)) {
    return false;
  }
  if (leftMatrix.numberOfColumns != rightMatrix.numberOfRows) {
    return false;
  }
  leftMatrix.multiplyOnTheRight(rightMatrix);
  return output.assignMatrix(leftMatrix, calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyMatrixRFOrRFByMatrixRF(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyMatrixRFOrRFByMatrixRF");
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression leftE = input[1];
  Expression rightE = input[2];
  if (
    !rightE.isMatrixOfType<RationalFunction<Rational> >() || !(
      leftE.isOfType<Rational>() ||
      leftE.isOfType<Polynomial<Rational> >() ||
      leftE.isOfType<RationalFunction<Rational> >() ||
      leftE.isMatrixOfType<RationalFunction<Rational> >()
    )
  ) {
    return false;
  }
  if (!leftE.mergeContexts(leftE, rightE)) {
    return calculator << "Failed to convert "
    << leftE.toString() << " and " << rightE.toString()
    << " to common context. ";
  }
  Matrix<RationalFunction<Rational> > leftMatrix, rightMatrix;
  if (!calculator.functionGetMatrix(rightE, rightMatrix)) {
    return false;
  }
  if (!leftE.isMatrixOfType<RationalFunction<Rational> >()) {
    Expression leftErfForm;
    if (!leftE.convertInternally<RationalFunction<Rational> >(leftErfForm)) {
      return calculator << "Failed to convert "
      << leftE.toString() << " to rational function. ";
    }
    RationalFunction<Rational> theScalar = leftErfForm.getValue<RationalFunction<Rational> >();
    rightMatrix *= theScalar;
    ExpressionContext contextE = leftE.getContext();
    return output.assignMatrix(rightMatrix, calculator, &contextE);
  }
  if (!calculator.functionGetMatrix(leftE, leftMatrix)) {
    return false;
  }
  if (leftMatrix.numberOfColumns != rightMatrix.numberOfRows) {
    return false;
  }
  leftMatrix.multiplyOnTheRight(rightMatrix);
  ExpressionContext contextE = leftE.getContext();
  return output.assignMatrix(leftMatrix, calculator, &contextE);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyMatrixTensorOrRationalByMatrixTensor(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyMatrixTensorOrRationalByMatrixTensor");
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!rightE.isOfType<MatrixTensor<Rational> >()) {
    return false;
  }
  Rational theScalar;
  if (leftE.isOfType<Rational>(&theScalar)) {
    MatrixTensor<Rational> result = rightE.getValue<MatrixTensor<Rational> >();
    result *= theScalar;
    return output.assignValue(result, calculator);
  }
  if (!leftE.isOfType<MatrixTensor<Rational> >()) {
    return false;
  }
  const MatrixTensor<Rational>& rightMatrix = rightE.getValue<MatrixTensor<Rational> >();
  MatrixTensor<Rational> result = leftE.getValue<MatrixTensor<Rational> >();
  result *= rightMatrix;
  return output.assignValue(result, calculator);
}

bool CalculatorFunctionsBinaryOps::innerLieBracketExtractConstant(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerLieBracketExtractConstant");
  if (!input.startsWith(calculator.opLieBracket(), 3)) {
    return false;
  }
  Rational theCF = 1;
  Rational tempRat;
  Expression leftE = input[1];
  Expression rightE = input[2];
  bool found = false;
  if (input[1].startsWith(calculator.opTimes(), 3)) {
    if (input[1][1].isOfType(&tempRat)) {
      found = true;
      theCF *= tempRat;
      leftE = input[1][2];
    }
  }
  if (input[2].startsWith(calculator.opTimes(), 3)) {
    if (input[2][1].isOfType(&tempRat)) {
      found = true;
      theCF *= tempRat;
      rightE = input[2][2];
    }
  }
  if (!found) {
    return false;
  }
  Expression theBracket;
  theBracket.makeXOX(calculator, calculator.opLieBracket(), leftE, rightE);
  output.assignValue(theCF, calculator);
  output *= theBracket;
  return true;
}

bool CalculatorFunctionsBinaryOps::innerLieBracketDistribute(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerLieBracketDistribute");
  return CalculatorBasics::distribute(
    calculator, input, output, calculator.opPlus(), calculator.opLieBracket()
  );
}

bool CalculatorFunctionsBinaryOps::innerLieBracketRatOrUEWithRatOrUE(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerLieBracketRatOrUEWithRatOrUE");
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.mergeContextsMyAruments(inputConverted, nullptr)) {
    return false;
  }
  const Expression& leftE = inputConverted[1];
  const Expression& rightE = inputConverted[2];
  if (leftE.isOfType<Rational>() || rightE.isOfType<Rational>()) {
    return output.assignValueWithContext(0, leftE.getContext() , calculator);
  }
  if (
    leftE.isOfType<ElementUniversalEnveloping<RationalFunction<Rational> > >() &&
    rightE.isOfType<ElementUniversalEnveloping<RationalFunction<Rational> > >()
  ) {
    ElementUniversalEnveloping<RationalFunction<Rational> > result;
    leftE.getValue<ElementUniversalEnveloping<RationalFunction<Rational> > >().lieBracketOnTheRight(
      rightE.getValue<ElementUniversalEnveloping<RationalFunction<Rational> > >(), result
    );
    result.simplify(RationalFunction<Rational>::one());
    return output.assignValueWithContext(result, leftE.getContext(), calculator);
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerLieBracketJacobiIdentityIfNeeded(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerLieBracketJacobiIdentityIfNeeded");
  if (!input.startsWith(calculator.opLieBracket(), 3)) {
    return false;
  }
  if (!input[2].startsWith(calculator.opLieBracket())) {
    return false;
  }
  bool doContinue = (input[1] > input[2][1] && input[1] > input[2][2]);
  if (!doContinue) {
    return false;
  }
  Expression leftE, rightE;
  Expression lieBracket;
  lieBracket.makeXOX(calculator, calculator.opLieBracket(), input[2][2],input[1]);
  leftE.makeXOX(calculator, calculator.opLieBracket(), input[2][1], lieBracket);
  lieBracket.makeXOX(calculator, calculator.opLieBracket(), input[1],input[2][1]);
  rightE.makeXOX(calculator, calculator.opLieBracket(), input[2][2], lieBracket);
  leftE = calculator.expressionMinusOne() * leftE;
  rightE = calculator.expressionMinusOne() * rightE;
  output = leftE + rightE;
  return true;
}

bool CalculatorFunctionsBinaryOps::innerLieBracketSwapTermsIfNeeded(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerLieBracketSwapTermsIfNeeded");
  if (!input.startsWith(calculator.opLieBracket(), 3)) {
    return false;
  }
  if (input[1].isEqualToZero() || input[2].isEqualToZero()) {
    return output.assignValue(0, calculator);
  }
  if (input[2] > input[1]) {
    return false;
  }
  if (input[1] == input[2]) {
    return output.assignValue(0, calculator);
  }
  Expression theBracket;
  theBracket.makeXOX(calculator, calculator.opLieBracket(), input[2], input[1]);
  output = calculator.expressionMinusOne() * theBracket;
  return true;
}

bool CalculatorFunctionsBinaryOps::innerLieBracketRatPolyOrEWAWithRatPolyOrEWA(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerLieBracketRatPolyOrEWAWithRatPolyOrEWA");
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.mergeContextsMyAruments(inputConverted, nullptr)) {
    return false;
  }
  const Expression& leftE = inputConverted[1];
  const Expression& rightE = inputConverted[2];
  bool leftEisGood =
  leftE.isOfType<Rational>() ||
  leftE.isOfType<Polynomial<Rational> >() ||
  leftE.isOfType<ElementWeylAlgebra<Rational> >();
  bool rightEisGood =
  rightE.isOfType<Rational>() ||
  rightE.isOfType<Polynomial<Rational> >() ||
  rightE.isOfType<ElementWeylAlgebra<Rational> >();
  if (!leftEisGood || !rightEisGood) {
    return false;
  }
  if (
    !leftE.isOfType<ElementWeylAlgebra<Rational> >() &&
    !rightE.isOfType<ElementWeylAlgebra<Rational> >()
  ) {
    return output.assignValue(0, calculator);
  }
  Expression leftConverted, rightConverted;
  if (
    !leftE.convertInternally<ElementWeylAlgebra<Rational> >(leftConverted) ||
    !rightE.convertInternally<ElementWeylAlgebra<Rational> >(rightConverted)
  ) {
    calculator << "<hr>Failed with conversion to Element weyl algebra - possible programming error?";
    return false;
  }
  ElementWeylAlgebra<Rational> resultE = rightConverted.getValue<ElementWeylAlgebra<Rational> >();
  resultE.lieBracketOnTheLeft(leftConverted.getValue<ElementWeylAlgebra<Rational> >());
  return output.assignValueWithContext(resultE, leftConverted.getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerAddMatrixToMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddMatrixToMatrix");
  if (!input.startsWith(calculator.opPlus(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  int leftNumRows = - 1, leftNumCols = - 1, rightNumRows = - 1, rightNumCols = - 1;
  if (
    !leftE.isMatrix(&leftNumRows, &leftNumCols) ||
    !rightE.isMatrix(&rightNumRows, &rightNumCols)
  ) {
    return false;
  }
  if ((leftNumRows != rightNumRows) || (leftNumCols != rightNumCols)) {
    return false;
  }
  Matrix<Expression> leftMatrix, rightMatrix;
  if (
    !calculator.getMatrixExpressions(leftE, leftMatrix) ||
    !calculator.getMatrixExpressions(rightE, rightMatrix)
  ) {
    return false;
  }
  if (leftMatrix.numberOfColumns != rightMatrix.numberOfColumns || leftMatrix.numberOfRows != rightMatrix.numberOfRows) {
    return false;
  }
  for (int i = 0; i < leftMatrix.numberOfRows; i ++) {
    for (int j = 0; j < leftMatrix.numberOfColumns; j ++) {
      leftMatrix(i, j) += rightMatrix(i, j);
    }
  }
  return output.assignMatrixExpressions(leftMatrix, calculator, false, true);
}

bool CalculatorFunctionsBinaryOps::innerAugmentMatrixToTheRight(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAugmentMatrixToTheRight");
  if (input.size() != 3) {
    return false;
  }
  Matrix<Expression> leftMatrix, rightMatrix;
  if (
    !calculator.getMatrixExpressions(input[1], leftMatrix) ||
    !calculator.getMatrixExpressions(input[2], rightMatrix)
  ) {
    return false;
  }
  if (leftMatrix.numberOfRows != rightMatrix.numberOfRows) {
    return calculator << "Cannot augment the left matrix with: "
    << leftMatrix.numberOfRows
    << " rows to the right by a matrix with a different number of rows: "
    << rightMatrix.numberOfRows << ". ";
  }
  leftMatrix.appendMatrixOnTheRight(rightMatrix);
  return output.assignMatrixExpressions(leftMatrix, calculator, false, false);
}

bool CalculatorFunctionsBinaryOps::innerAugmentMatrixBelow(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAugmentMatrixBelow");
  if (input.size() != 3) {
    return false;
  }
  Matrix<Expression> leftMatrix, rightMatrix;
  if (
    !calculator.getMatrixExpressions(input[1], leftMatrix) ||
    !calculator.getMatrixExpressions(input[2], rightMatrix)
  ) {
    return false;
  }
  if (leftMatrix.numberOfRows != rightMatrix.numberOfRows) {
    return calculator
    << "Cannot augment the left matrix with: " << leftMatrix.numberOfRows
    << " rows to the right by a matrix with a different number of rows: "
    << rightMatrix.numberOfRows << ". ";
  }
  leftMatrix.appendMatrixToTheBottom(rightMatrix);
  return output.assignMatrixExpressions(leftMatrix, calculator, false, false);

}

bool CalculatorFunctionsBinaryOps::innerDirectSumMatrixWithMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDirectSumMatrixWithMatrix");
  if (!input.startsWith(calculator.opDirectSum(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!leftE.isMatrix() || !rightE.isMatrix()) {
    return false;
  }
  Matrix<Expression> leftMatrix, rightMatrix;
  if (
    !calculator.getMatrixExpressions(leftE, leftMatrix) ||
    !calculator.getMatrixExpressions(rightE, rightMatrix)
  ) {
    return false;
  }
  leftMatrix.directSumWith(rightMatrix,calculator.expressionZero());
  return output.assignMatrixExpressions(leftMatrix, calculator, false, true);
}

bool CalculatorFunctionsBinaryOps::innerAddMatrixRationalOrAlgebraicToMatrixRationalOrAlgebraic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddMatrixRationalOrAlgebraicToMatrixRationalOrAlgebraic");
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression leftE = input[1];
  Expression rightE = input[2];
  Matrix<Rational> leftMatRat, rightMatRat;
  Matrix<AlgebraicNumber> leftMatAlg, rightMatAlg;
  if (
    !rightE.isMatrixOfType<Rational>() ||
    !leftE.isMatrixOfType<Rational>()
  ) {
    if (rightE.isMatrixOfType<AlgebraicNumber>()) {
      MathRoutines::swap(leftE, rightE);
    }
    if (!leftE.isMatrixOfType<AlgebraicNumber>()) {
      return false;
    }
    if (rightE.isMatrixOfType<Rational>()) {
      if (!calculator.functionGetMatrix(leftE, leftMatAlg)) {
        return false;
      }
      if (!calculator.functionGetMatrix(rightE, rightMatAlg)) {
        return false;
      }
      if (
        rightMatAlg.numberOfRows != leftMatAlg.numberOfRows ||
        rightMatAlg.numberOfColumns != leftMatAlg.numberOfColumns
      ) {
        return false;
      }
      leftMatAlg += rightMatAlg;
      return output.assignMatrix(leftMatAlg, calculator);
    }
    if (!calculator.functionGetMatrix(rightE, rightMatAlg)) {
      return false;
    }
    if (!calculator.functionGetMatrix(leftE, leftMatAlg)) {
      return false;
    }
    if (
      rightMatAlg.numberOfRows != leftMatAlg.numberOfRows ||
      rightMatAlg.numberOfColumns != leftMatAlg.numberOfColumns
    ) {
      return false;
    }
    leftMatAlg += rightMatAlg;
    return output.assignMatrix(leftMatAlg, calculator);
  }
  if (!calculator.functionGetMatrix(leftE, leftMatRat)) {
    return false;
  }
  if (!calculator.functionGetMatrix(rightE, rightMatRat)) {
    return false;
  }
  if (
    leftMatRat.numberOfRows != rightMatRat.numberOfRows ||
    leftMatRat.numberOfColumns != rightMatRat.numberOfColumns
  ) {
    std::stringstream errorStream;
    errorStream << "Error: attempting to add a "
    << rightMatRat.numberOfRows << " by "
    << rightMatRat.numberOfColumns << " matrix to a "
    << leftMatRat.numberOfRows
    << " by " << leftMatRat.numberOfColumns << " matrix. ";
    return output.makeError(errorStream.str(), calculator);
  }
  leftMatRat += rightMatRat;
  return output.assignMatrix(leftMatRat, calculator);
}

bool CalculatorFunctionsBinaryOps::setMinus(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::setMinus");
  if (input.size() != 3) {
    return false;
  }
  const Expression& leftSetE = input[1];
  const Expression& rightSetE = input[2];
  if (!leftSetE.isSequenceNElements() || !rightSetE.isSequenceNElements()) {
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
  for (int i = 1; i < rightSetE.children.size; i ++) {
    if (resultEs.contains(rightSetE[i])) {
      resultEs.removeIndexSwapWithLast(resultEs.getIndex(rightSetE[i]));
    }
  }
  resultEs.quickSortAscending();
  return output.makeSequence(calculator, &resultEs);
}

bool CalculatorFunctionsBinaryOps::innerAddMatrixRFsToMatrixRFs(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddMatrixRFsToMatrixRFs");
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression leftE = input[1];
  Expression rightE = input[2];
  if (
    !rightE.isMatrixOfType<RationalFunction<Rational> >() ||
    !leftE.isMatrixOfType<RationalFunction<Rational> >()
  ) {
    return false;
  }
  if (!leftE.mergeContexts(leftE, rightE)) {
    return false;
  }
  Matrix<RationalFunction<Rational> > leftMatrix, rightMatrix;
  if (!calculator.functionGetMatrix(leftE, leftMatrix)) {
    return false;
  }
  if (!calculator.functionGetMatrix(rightE, rightMatrix)) {
    return false;
  }
  if (
    rightMatrix.numberOfRows != leftMatrix.numberOfRows ||
    rightMatrix.numberOfColumns != leftMatrix.numberOfColumns
  ) {
    return false;
  }
  leftMatrix += rightMatrix;
  ExpressionContext theContext = leftE.getContext();
  return output.assignMatrix(leftMatrix, calculator, &theContext);
}

bool CalculatorFunctionsBinaryOps::innerAddMatrixTensorToMatrixTensor(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!rightE.isOfType<MatrixTensor<Rational> >()|| !leftE.isOfType<MatrixTensor<Rational> >()) {
    return false;
  }
  const MatrixTensor<Rational>& rightMatrix = rightE.getValue<MatrixTensor<Rational> >();
  const MatrixTensor<Rational>& leftMatrix = leftE.getValue<MatrixTensor<Rational> >();
  MatrixTensor<Rational> result = leftMatrix;
  result += rightMatrix;
  return output.assignValue(result, calculator);
}

bool CalculatorFunctionsBinaryOps::innerMultiplySequenceByAnyScalar(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplySequenceByAnyScalar");
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression tempE = input;
  tempE.children.swapTwoIndices(1, 2);
  return CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence(calculator, tempE, output);
}

bool CalculatorFunctionsBinaryOps::innerAddSequenceToSequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddSequenceToSequence");
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
    return calculator << "<hr>Attempting to add a sequence of length "
    << input[1].size() - 1 << "  to a sequence of length "
    << input[2].size() - 1 << ", possible user typo?";
  }
  output.reset(calculator);
  output.children.reserve(input[1].size());
  output.addChildAtomOnTop(calculator.opSequence());
  Expression tempSum;
  for (int i = 1; i < input[2].size(); i ++) {
    tempSum.makeXOX(calculator, calculator.opPlus(), input[1][i], input[2][i]);
    output.addChildOnTop(tempSum);
  }
  return true;
}

bool CalculatorFunctionsBinaryOps::binomialCoefficient(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::binomialCoefficient");
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
    return output.assignValue(result, calculator);
  }
  if (K < 11) {
    output.assignValue(1, calculator);
    for (int i = 0; i < K; i ++) {
      output *= input[1] - i;
    }
    return true;
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerPowerElementZmodPByInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerElementZmodPByInteger");
  if (!input.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  ElementZmodP theElt;
  if (!leftE.isOfType(&theElt)) {
    return false;
  }
  LargeInteger thePower = 0;
  if (!rightE.isInteger(&thePower)) {
    return false;
  }
  if (thePower < 0) {
    ElementZmodP copy = theElt;
    theElt.makeOne(theElt.modulus);
    theElt /= copy;
    thePower *= - 1;
  }
  ElementZmodP unit;
  unit.makeOne(theElt.modulus);
  MathRoutines::raiseToPower(theElt, thePower, unit);
  ExpressionContext context(calculator);
  context.setDefaultModulus(theElt.modulus);
  return output.assignValueWithContext(theElt, context, calculator);
}

bool CalculatorFunctionsBinaryOps::innerPowerEllipticCurveRationalElementByInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerEllipticCurveRationalElementByInteger");
  if (!input.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  ElementEllipticCurve<Rational> theElt;
  if (!leftE.isOfType(&theElt)) {
    return false;
  }
  int thePower = 0;
  if (!rightE.isSmallInteger(& thePower)) {
    return false;
  }
  if (thePower < 0) {
    theElt.invert();
    thePower *= - 1;
  }
  ElementEllipticCurve<Rational> unit;
  unit.makeOne(theElt.owner);
  MathRoutines::raiseToPower(theElt, thePower, unit);
  return output.assignValueWithContext(theElt, input[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::innerPowerEllipticCurveZmodPElementByInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerEllipticCurveZmodPElementByInteger");
  if (!input.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  ElementEllipticCurve<ElementZmodP> theElt;
  if (!leftE.isOfType(&theElt)) {
    return false;
  }
  LargeInteger thePower = 0;
  if (!rightE.isInteger(& thePower)) {
    return false;
  }
  if (thePower < 0) {
    theElt.invert();
    thePower *= - 1;
  }
  ElementEllipticCurve<ElementZmodP> unit;
  unit.makeOne(theElt.owner);
  MathRoutines::raiseToPower(theElt, thePower, unit);
  return output.assignValueWithContext(theElt, input[1].getContext(), calculator);
}

bool CalculatorFunctionsBinaryOps::polynomialModPModuloPolynomialModP(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::polynomialModPModuloPolynomialModP");
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
  if (!input.mergeContextsMyArumentsAndConvertThem<Polynomial<ElementZmodP> >(
    inputConverted, &calculator.comments
  )) {
    return false;
  }
  WithContext<Polynomial<ElementZmodP> > left, right;
  if (
    !inputConverted[1].isOfTypeWithContext(&left) ||
    !inputConverted[2].isOfTypeWithContext(&right)
  ) {
    return false;
  }
  if (right.content.isEqualToZero()) {
    return output.makeError("Division by zero", calculator);
  }
  LargeIntegerUnsigned modulus = right.content.coefficients[0].modulus;
  if (!left.content.isEqualToZero()) {
    if (left.content.coefficients[0].modulus != modulus) {
      return calculator
      << "Attempt to perform operations with "
      << "modular polynomials with different moduli: "
      << modulus << " and "
      << left.content.coefficients[0].modulus;
    }
  }
  ElementZmodP rightLeadingCoefficient = right.content.getLeadingCoefficient(&MonomialP::orderDefault());
  if (!rightLeadingCoefficient.invert()) {
    return calculator
    << "Leading coefficient of quotient "
    << "polynomial is not invertible mod " << modulus << ".";
  }
  PolynomialModuloPolynomial<ElementZmodP> result;
  result.modulus = right.content;
  result.value = left.content;
  result.reduce();
  return output.assignValueWithContext(result, right.context, calculator);
}
