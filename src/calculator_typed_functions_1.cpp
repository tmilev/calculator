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
#include "math_extra_finite_groups_implementation.h" // undefined reference to `void WeylGroup::RaiseToDominantWeight<Rational>(Vector<Rational>&, int*, bool*, ElementWeylGroup<WeylGroup>*)

template <>
bool Expression::ConvertInternally<RationalFunction>(Expression& output) const;

template <>
bool Expression::ConvertInternally<ElementWeylAlgebra<Rational> >(Expression& output) const;

bool Calculator::innerOperationBinary(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  int theOp
) {
  MemorySaving<Calculator::OperationHandlers>& theOperation = theCommands.operations.theValues[theOp];
  if (theOperation.IsZeroPointer()) {
    return false;
  }
  List<Function>& handlers = theOperation.getElement().handlers;
  for (int i = 0; i < handlers.size; i ++) {
    if (handlers[i].inputFitsMyInnerType(input)) {
      if (handlers[i].theFunction(theCommands, input, output)) {
        return true;
      }
    }
  }
  return false;
}

bool Calculator::outerExtractBaseMultiplication(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::outerExtractBaseMultiplication");
  if (!input.startsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  bool result = false;
  // handle Anything * Rational = Rational * Anything
  output = input;
  if (output[2].IsOfType<Rational>()) {
    output.children.swapTwoIndices(1, 2);
    result = true;
  }
  if (output[2].IsOfType<double>() && !output[1].IsOfType<Rational>()) {
    output.children.swapTwoIndices(1, 2);
    result = true;
  }
  if (output[2].IsListStartingWithAtom(theCommands.opTimes())) {
    if (output[2].children.size != 3) {
      return result;
    }
    // handle Anything1 * (Rational * Anything2) = Rational * (Anything1 * Anything2)
    if (output[2][1].IsOfType<Rational>()) {
      Expression tempRight;
      tempRight.MakeXOX(theCommands, theCommands.opTimes(), output[1], output[2][2]);
      output.MakeXOX(theCommands, theCommands.opTimes(), output[2][1], tempRight);
      result = true;
    }
    // <- handle a * (b * anything)
    // on condition that a*b has an inner handler
    Expression tempExp, newExpr;
    tempExp.MakeXOX(theCommands, theCommands.opTimes(), output[1], output[2][1]);
    if (theCommands.innerTimes(theCommands, tempExp, newExpr)) {
      output.MakeProducT(theCommands, newExpr, output[2][2]);
      result = true;
    }
  }
  // handle 0 * anything = 0
  if (output[1].isEqualToZero()) {
    return output.assignValue(0, theCommands);
  }
  return result;
}

bool CalculatorFunctionsBinaryOps::innerAddEltZmodPorRatToEltZmodPorRat(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddEltZmodPorRatToEltZmodPorRat");
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression* leftE;
  const Expression* rightE;
  leftE = &input[1];
  rightE = &input[2];
  ElementZmodP theElt1, theElt2;
  for (int i = 0; i < 2; i ++, MathRoutines::swap(leftE, rightE)) {
    if (leftE->IsOfType<ElementZmodP>(&theElt1)) {
      if (rightE->IsOfType<ElementZmodP>(&theElt2)) {
        if (theElt1.theModulus != theElt2.theModulus) {
          return false;
        }
      } else {
        Rational tempRat;
        if (!rightE->IsOfType<Rational>(&tempRat)) {
          return false;
        }
        theElt2.theModulus = theElt1.theModulus;
        if (!theElt2.AssignRational(tempRat)) {
          return false;
        }
      }
      theElt1 += theElt2;
      return output.assignValue(theElt1, theCommands);
    }
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerMultiplyEltZmodPorRatByEltZmodPorRat(
  Calculator& theCommands, const Expression& input, Expression& output
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
    if (leftE->IsOfType<ElementZmodP>(&theElt1)) {
      if (rightE->IsOfType<ElementZmodP>(&theElt2)) {
        if (theElt1.theModulus != theElt2.theModulus) {
          return false;
        }
      } else {
        Rational tempRat;
        if (!rightE->IsOfType<Rational>(&tempRat)) {
          return false;
        }
        theElt2.theModulus = theElt1.theModulus;
        if (!theElt2.AssignRational(tempRat)) {
          return false;
        }
      }
      theElt1 *= theElt2;
      return output.assignValue(theElt1, theCommands);
    }
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerDivideEltZmodPorRatByEltZmodPorRat(
  Calculator& theCommands, const Expression& input, Expression& output
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
    if (leftE->IsOfType<ElementZmodP>(&theElt1)) {
      if (rightE->IsOfType<ElementZmodP>(&theElt2)) {
        if (theElt1.theModulus != theElt2.theModulus) {
          return false;
        }
      } else {
        Rational tempRat;
        if (!rightE->IsOfType<Rational>(&tempRat)) {
          return false;
        }
        theElt2.theModulus = theElt1.theModulus;
        if (!theElt2.AssignRational(tempRat)) {
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
        return output.makeError(out.str(), theCommands);
      }
      return output.assignValue(theElt1, theCommands);
    }
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerAddRatToRat(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddRatToRat");
  if (input.size() != 3) {
    return false;
  }
  Rational leftR, rightR;
  if (!input[1].IsOfType(&leftR) || !input[2].IsOfType(&rightR)) {
    return false;
  }
  return output.assignValue(leftR + rightR, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddStringToString(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddStringToString");
  if (input.size() != 3) {
    return false;
  }
  std::string left, right;
  if (!input[1].IsOfType(&left) || !input[2].IsOfType(&right)) {
    return false;
  }
  return output.assignValue(left + right, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat");
  if (!input.isListNElements(3)) {
    return false;
  }
  AlgebraicNumber leftAN, rightAN;
  Rational tempRat;
  if (!input[1].IsOfType(&leftAN)) {
    if (!input[2].IsOfType(&rightAN)) {
      return false;
    }
    if (!input[1].IsOfType<Rational>(&tempRat)) {
      return false;
    }
    leftAN.AssignRational(tempRat, theCommands.theObjectContainer.theAlgebraicClosure);
  } else if (!input[2].IsOfType(&rightAN)) {
    if (!input[2].IsOfType(&tempRat)) {
      return false;
    }
    rightAN.AssignRational(tempRat, theCommands.theObjectContainer.theAlgebraicClosure);
  }
  if (rightAN.isEqualToZero()) {
    return output.makeError("Division by zero. ", theCommands);
  }
  leftAN /= rightAN;
  output.assignValue(leftAN, theCommands);
  output.CheckInitializationRecursively();
  return true;
}

bool CalculatorFunctionsBinaryOps::innerMultiplyEltHypOctByEltHypOct(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyEltHypOctByEltHypOct");
  if (!input.isListNElements(3)) {
    return false;
  }
  ElementHyperoctahedralGroupR2 outElt, left, right;
  if (
    !input[1].IsOfType<ElementHyperoctahedralGroupR2>(&left) ||
    !input[2].IsOfType<ElementHyperoctahedralGroupR2>(&right)
  ) {
    return false;
  }
  outElt = left * right;
  return output.assignValue(outElt, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyAlgebraicNumberByAlgebraicNumber(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyAlgebraicNumberByAlgebraicNumber");
  if (!input.isListNElements(3)) {
    return false;
  }
  AlgebraicNumber leftAN, rightAN;
  Rational tempRat;
  if (!input[1].IsOfType(&leftAN)) {
    if (!input[2].IsOfType(&rightAN)) {
      return false;
    }
    if (!input[1].IsOfType<Rational>(&tempRat)) {
      return false;
    }
    leftAN.AssignRational(tempRat, theCommands.theObjectContainer.theAlgebraicClosure);
  } else if (!input[2].IsOfType(&rightAN)) {
    if (!input[2].IsOfType(&tempRat)) {
      return false;
    }
    rightAN.AssignRational(tempRat, theCommands.theObjectContainer.theAlgebraicClosure);
  }
  leftAN *= rightAN;
  return output.assignValue(leftAN, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber");
  if (!input.isListNElements(3)) {
    return false;
  }
  AlgebraicNumber leftAN, rightAN;
  Rational tempRat;
  if (!input[1].IsOfType(&leftAN)) {
    if (!input[2].IsOfType(&rightAN)) {
      return false;
    }
    if (!input[1].IsOfType<Rational>(&tempRat)) {
      return false;
    }
    leftAN.AssignRational(tempRat, theCommands.theObjectContainer.theAlgebraicClosure);
    leftAN.checkConsistency();
  } else if (!input[2].IsOfType(&rightAN)) {
    if (!input[2].IsOfType(&tempRat)) {
      return false;
    }
    rightAN.AssignRational(tempRat, theCommands.theObjectContainer.theAlgebraicClosure);
    rightAN.checkConsistency();
  }
  leftAN.checkConsistency();
  rightAN.checkConsistency();
  leftAN += rightAN;
  leftAN.checkConsistency();
  return output.assignValue(leftAN, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRatByRat(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRatByRat");
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational leftR, rightR;
  if (!input[1].IsOfType(&leftR) || !input[2].IsOfType(&rightR)) {
    return false;
  }
  return output.assignValue(leftR*rightR, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyCoxeterEltByCoxeterElt(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyCoxeterEltByCoxeterElt");
  if (!input.isListNElements(3)) {
    return false;
  }
  ElementWeylGroup leftR, rightR;
  if (!input[1].IsOfType(&leftR) || !input[2].IsOfType(&rightR)) {
    return false;
  }
  if (leftR.owner != rightR.owner) {
    return theCommands << "<hr>Attempting to multiply elements of different groups, possible user typo?";
  }
  leftR *= rightR;
  return output.assignValue(leftR, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerDivideRatByRat(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDivideRatByRat");
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational leftR, rightR;
  if (!input[1].IsOfType(&leftR) || !input[2].IsOfType(&rightR)) {
    return false;
  }
  if (rightR.isEqualToZero()) {
    return output.makeError("Division by zero.", theCommands);
  }
  return output.assignValue(leftR / rightR, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerDivideDoubleByDouble(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDivideRatByRat");
  if (!input.isListNElements(3)) {
    return false;
  }
  double leftD, rightD;
  Rational leftR, rightR;
  if (!input[1].IsOfType(&leftD)) {
    if (!input[1].IsOfType(&leftR)) {
      return false;
    } else {
      leftD = leftR.GetDoubleValue();
    }
  }
  if (!input[2].IsOfType(&rightD)) {
    if (!input[2].IsOfType(&rightR)) {
      return false;
    } else {
      rightD = rightR.GetDoubleValue();
    }
  }
  if (rightD == 0.0) {
    return output.makeError("Division by zero.", theCommands);
  }
  return output.assignValue(leftD / rightD, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerTensorEltTensorByEltTensor(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerTensorEltTensorByEltTensor");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (input.size() != 3) {
    return false;
  }
  Expression inputConverted;
  if (!input.MergeContextsMyArumentsAndConvertThem<ElementTensorsGeneralizedVermas<RationalFunction> >(
    inputConverted, &theCommands.comments
  )) {
    return false;
  }
  ElementTensorsGeneralizedVermas<RationalFunction> resultTensor;
  resultTensor = inputConverted[1].getValue<ElementTensorsGeneralizedVermas<RationalFunction> >();
  resultTensor.TensorOnTheRight(inputConverted[2].getValue<ElementTensorsGeneralizedVermas<RationalFunction> >());
  return output.assignValueWithContext(resultTensor, inputConverted[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddWeightToWeight(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddWeightToWeight");
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<Weight<Polynomial<Rational> > >(theCommands, input, output);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyByWeightPoly(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyByWeightPoly");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.MergeContextsMyAruments(inputConverted, &theCommands.comments)) {
    return false;
  }
  Weight<Polynomial<Rational> > theWeight;
  Rational cfRat;
  Polynomial<Rational> theCoefficient;
  if (!inputConverted[1].IsOfType<Polynomial<Rational> >(&theCoefficient)) {
    if (!inputConverted[1].IsOfType<Rational>(&cfRat)) {
      return false;
    }
    theCoefficient = cfRat;
  }
  if (!inputConverted[2].IsOfType<Weight<Polynomial<Rational> > >(&theWeight)) {
    return false;
  }
  theWeight *= theCoefficient;
  return output.assignValueWithContext(theWeight, inputConverted[2].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyWeylGroupEltByWeightPoly(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyWeylGroupEltByWeightPoly");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.MergeContextsMyAruments(inputConverted, &theCommands.comments)) {
    return false;
  }
  Weight<Polynomial<Rational> > theWeight;
  if (!inputConverted[2].IsOfType<Weight<Polynomial<Rational> > >(&theWeight)) {
    return false;
  }
  if (!inputConverted[1].IsOfType<ElementWeylGroup>()) {
    return false;
  }
  ElementWeylGroup theElt = inputConverted[1].getValue<ElementWeylGroup>();
  if (theElt.owner != &theWeight.owner->theWeyl) {
    return theCommands << "<hr>Possible user input error: attempting to apply Weyl group "
    << "element to weight corresponding to different Weyl group.";
  }
  Vector<Polynomial<Rational> > theWeightSimpleCoords = theElt.owner->GetSimpleCoordinatesFromFundamental(
    theWeight.weightFundamentalCoordS
  );
  theElt.actOn(theWeightSimpleCoords);
  theWeight.weightFundamentalCoordS = theElt.owner->GetFundamentalCoordinatesFromSimple(theWeightSimpleCoords);
  return output.assignValueWithContext(theWeight, inputConverted[2].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyEllipticCurveElements(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyEllipticCurveElements");
  if (!input.startsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  ElementEllipticCurve<Rational> left, right;
  if (!input[1].IsOfType(&left) || !input[2].IsOfType(&right)) {
    return false;
  }
  if (!(left *= right)) {
    return false;
  }
  return output.assignValueWithContext(left, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyEllipticCurveElementsZmodP(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyEllipticCurveElements");
  if (!input.startsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  ElementEllipticCurve<ElementZmodP> left, right;
  if (!input[1].IsOfType(&left) || !input[2].IsOfType(&right)) {
    return false;
  }
  if (!left.flagInfinity && !right.flagInfinity) {
    if (left.xCoordinate.theModulus != right.xCoordinate.theModulus) {
      return theCommands << "Attempt to multiply elliptic curve elements over Z mod p "
      << "for different moduli p. Possible bad user input?";
    }
  }
  if (!(left *= right)) {
    return false;
  }
  return output.assignValueWithContext(left, input[1].GetContext(), theCommands);
}

template <>
bool Expression::ConvertInternally<ElementUniversalEnveloping<RationalFunction> >(Expression& output) const;

bool CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.MergeContextsMyAruments(inputConverted, &theCommands.comments)) {
    return false;
  }
  if (!inputConverted[2].IsOfType<ElementTensorsGeneralizedVermas<RationalFunction> >()) {
    return false;
  }
  SemisimpleLieAlgebra& theSSalg =
  inputConverted[2].getValue<ElementTensorsGeneralizedVermas<RationalFunction> >().GetOwnerSS();
  Expression leftE;
  inputConverted[1].checkConsistency();
  input[1].checkConsistency();
  input[2].checkConsistency();
  if (!inputConverted[1].ConvertInternally<ElementUniversalEnveloping<RationalFunction> >(leftE)) {
    return false;
  }
  const ElementTensorsGeneralizedVermas<RationalFunction>& rightEltETGVM =
  inputConverted[2].getValue<ElementTensorsGeneralizedVermas<RationalFunction> >();
  ElementTensorsGeneralizedVermas<RationalFunction> outputElt;
  if (rightEltETGVM.isEqualToZero()) {
    output = inputConverted[2];
    return true;
  }
  theSSalg.OrderNilradicalNilWeightAscending(rightEltETGVM.GetOwnerModule().parabolicSelectionNonSelectedAreElementsLevi);
  theSSalg.flagHasNilradicalOrder = true;
  if (!rightEltETGVM.multiplyOnTheLeft(
    leftE.getValue<ElementUniversalEnveloping<RationalFunction> >(), outputElt, theSSalg, 1
  )) {
    return false;
  }
  return output.assignValueWithContext(outputElt, inputConverted[2].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.MergeContextsMyArumentsAndConvertThem<ElementWeylAlgebra<Rational> >(
    inputContextsMerged, &theCommands.comments
  )) {
    return false;
  }
  if (
    inputContextsMerged[1].getValue<ElementWeylAlgebra<Rational> >().HasNonSmallPositiveIntegerDerivation() ||
    inputContextsMerged[2].getValue<ElementWeylAlgebra<Rational> >().HasNonSmallPositiveIntegerDerivation()
  ) {
    return theCommands << "<hr> Failed to multiply " << inputContextsMerged[1].toString()
    << " by " << inputContextsMerged[2].toString() << ": "
    << "one of the two differential operators has differential operator exponent that is not a small integer. ";
  }
  ElementWeylAlgebra<Rational> result = inputContextsMerged[1].getValue<ElementWeylAlgebra<Rational> >();
  result *= inputContextsMerged[2].getValue<ElementWeylAlgebra<Rational> >();
  return output.assignValueWithContext(result, inputContextsMerged[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF");
  if (!CalculatorFunctionsBinaryOps::innerMultiplyTypeByType<RationalFunction>(theCommands, input, output)) {
    return false;
  }
  RationalFunction simplified;
  if (!output.IsOfType(&simplified)) {
    return true;
  }
  simplified.simplify();
  return output.assignValueWithContext(simplified, output.GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerDivideRFOrPolyOrRatByRFOrPoly(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDivideRFOrPolyOrRatByRFOrPoly");
  if (!CalculatorFunctionsBinaryOps::innerDivideTypeByType<RationalFunction>(theCommands, input, output)) {
    return false;
  }
  RationalFunction simplified;
  if (!output.IsOfType(&simplified)) {
    return true;
  }
  simplified.simplify();
  return output.assignValueWithContext(simplified, output.GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrRFToRatOrPolyOrRF(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrRFToRatOrPolyOrRF");
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<RationalFunction>(theCommands, input, output);
}

bool CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrEWAToRatOrPolyOrEWA(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrEWAToRatOrPolyOrEWA");
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<ElementWeylAlgebra<Rational> >(theCommands, input, output);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly");
  if (input.children.size < 3) {
    return false;
  }
  const Expression leftE = input[1];
  const Expression rightE = input[2];
  if (
    leftE.IsOfType<AlgebraicNumber>() ||
    leftE.IsOfType <Polynomial<AlgebraicNumber> >() ||
    rightE.IsOfType<AlgebraicNumber>() ||
    rightE.IsOfType<Polynomial<AlgebraicNumber> >()
  ) {
    return CalculatorFunctionsBinaryOps::innerMultiplyTypeByType<Polynomial<AlgebraicNumber> >(theCommands, input, output);
  }
  return CalculatorFunctionsBinaryOps::innerMultiplyTypeByType<Polynomial<Rational> >(theCommands, input, output);
}

bool CalculatorFunctionsBinaryOps::innerAddUEToAny(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddUEToAny");
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<ElementUniversalEnveloping<RationalFunction> >(
    theCommands, input, output
  );
}

bool CalculatorFunctionsBinaryOps::innerMultiplyAnyByUE(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyUEByAny");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.MergeContextsMyArumentsAndConvertThem<ElementUniversalEnveloping<RationalFunction> >(
    inputContextsMerged, &theCommands.comments
  )) {
    return false;
  }
  ElementUniversalEnveloping<RationalFunction> result = inputContextsMerged[1].getValue<ElementUniversalEnveloping<RationalFunction> >();
  result *= inputContextsMerged[2].getValue<ElementUniversalEnveloping<RationalFunction> >();
  result.simplify();
  return output.assignValueWithContext(result, inputContextsMerged[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyLRObyLRO(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyLRObyLRO");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  output = input[1];
  Expression rightCopy = input[2];
  if (
    !output.IsOfType<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >() ||
    !rightCopy.IsOfType<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >()
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
    if (result.Powers[i] > 100000 || result.Powers[i] < 0) {
      return output.makeError(
        "The result of this operation is " + result.toString() + " which is outside of the allowed range. ",
        theCommands
      );
    }
  }
  return output.assignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyLRObyLSPath(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyLRObyLSPath");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  output = input[1];
  Expression rightCopy = input[2];
  if (!output.MergeContexts(output, rightCopy)) {
    return false;
  }
  if (!output.IsOfType<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >() || !rightCopy.IsOfType<LittelmannPath>()) {
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
      return output.makeError(out.str(), theCommands);
    } else {
      for (int j = 0; j < theLRO.Powers[i]; j ++) {
        result.ActByEFDisplayIndex(theLRO.generatorsIndices[i]);
      }
    }
  }
  return output.assignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddEltTensorToEltTensor(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddEltTensorToEltTensor");
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<ElementTensorsGeneralizedVermas<RationalFunction> >(
    theCommands, input, output
  );
}

bool CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly");
  if (input.size() < 3) {
    return false;
  }
  const Expression leftE = input[1];
  const Expression rightE = input[2];
  if (
    leftE.IsOfType<AlgebraicNumber>() ||
    leftE.IsOfType <Polynomial<AlgebraicNumber> >() ||
    rightE.IsOfType<AlgebraicNumber>() ||
    rightE.IsOfType<Polynomial<AlgebraicNumber> >()
  ) {
    return CalculatorFunctionsBinaryOps::innerAddTypeToType<Polynomial<AlgebraicNumber> >(theCommands, input, output);
  }
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<Polynomial<Rational> >(theCommands, input, output);
}

bool CalculatorFunctionsBinaryOps::innerAddPlotToPlot(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddPlotToPlot");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Plot leftPlot, rightPlot;
  if (!input[1].IsOfType<Plot>(&leftPlot)) {
    return false;
  }
  if (!input[2].IsOfType<Plot>(&rightPlot)) {
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
  return output.assignValue(leftPlot, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerPolynomialBySmallInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerPolynomialBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Polynomial<Rational> base;
  int thePower = 0;
  if (!input[1].IsOfType(&base) || !input[2].IsSmallInteger(&thePower)) {
    return false;
  }
  if (base.isEqualToZero() && thePower <= 0) {
    return output.makeError("Division by zero: trying to raise 0 to negative power. ", theCommands);
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
      return output.assignValueWithContext(outputPoly, input[1].GetContext(), theCommands);
    }
    base.raiseToPower(- thePower, 1);
    RationalFunction theRF;
    theRF = base;
    theRF.invert();
    return output.assignValueWithContext(theRF, input[1].GetContext(), theCommands);
  }
  base.raiseToPower(thePower, 1);
  return output.assignValueWithContext(base, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerMatrixNumbersByLargeIntegerIfPossible(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerMatrixNumbersByLargeIntegerIfPossible");
  if (!input.startsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  if (!input[1].IsMatrix()) {
    return false;
  }
  LargeInteger largePower;
  if (!input[2].IsInteger(&largePower)) {
    return false;
  }
  const Expression& matrixE = input[1];
  Matrix<Rational> baseRat;
  if (theCommands.functionGetMatrix(matrixE, baseRat)) {
    if (!baseRat.IsSquare() || baseRat.numberOfColumns == 0) {
      std::stringstream errorStream;
      errorStream << "Exponentiating non-square matrices or matrices with zero rows is not allowed. "
      << "Your matrix, " << baseRat.toString() << " is not square. ";
      return output.makeError(errorStream.str(), theCommands);
    }
    Rational theDet = baseRat.GetDeterminant();
    if (largePower <= 0) {
      if (theDet == 0) {
        return output.makeError("Division by zero: trying to raise 0 to negative power. ", theCommands);
      }
    }
    if (theDet != 0 && theDet != - 1 && theDet != 1) {
      return theCommands << "Matrix power too large.";
    }
    if (largePower < 0) {
      baseRat.invert();
      largePower *= - 1;
    }
    Matrix<Rational> idMat;
    idMat.MakeIdMatrix(baseRat.numberOfRows);
    MathRoutines::raiseToPower(baseRat, largePower, idMat);
    return output.AssignMatrix(baseRat, theCommands);
  }
  Matrix<AlgebraicNumber> baseAlg;
  if (theCommands.functionGetMatrix(matrixE, baseAlg)) {
    if (!baseAlg.IsSquare() || baseAlg.numberOfColumns == 0) {
      std::stringstream errorStream;
      errorStream << "Exponentiating non-square matrices or matrices with zero rows is not allowed. "
      << "Your matrix, " << baseAlg.toString() << " is not square. ";
      return output.makeError(errorStream.str(), theCommands);
    }
    AlgebraicNumber theDet = baseAlg.GetDeterminant();
    if (largePower <= 0) {
      if (theDet == 0) {
        return output.makeError("Division by zero: trying to raise 0 to negative power. ", theCommands);
      }
    }
    if (theDet != 0 && theDet != - 1 && theDet != 1) {
      return theCommands << "Matrix power too large.";
    }
    if (largePower < 0) {
      baseAlg.invert();
      largePower *= - 1;
    }
    Matrix<AlgebraicNumber> idMat;
    idMat.MakeIdMatrix(baseAlg.numberOfRows);
    MathRoutines::raiseToPower(baseAlg, largePower, idMat);
    return output.AssignMatrix(baseAlg, theCommands);
  }  
  return false;
}

bool CalculatorFunctionsBinaryOps::innerPowerMatNumbersBySmallInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerMatNumbersBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.startsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  const Expression& matrixE = input[1];
  if (!matrixE.IsMatrix()) {
    return false;
  }
  int thePower = 0;
  bool powerIsSmall = input[2].IsSmallInteger(&thePower);
  if (!powerIsSmall) {
    return false;
  }
  Matrix<Rational> baseRat;
  if (theCommands.functionGetMatrix(matrixE, baseRat)) {
    if (!baseRat.IsSquare() || baseRat.numberOfColumns == 0) {
      std::stringstream errorStream;
      errorStream << "Exponentiating non-square matrices or matrices with zero rows is not allowed. "
      << "Your matrix, " << baseRat.toString() << " is not square. ";
      return output.makeError(errorStream.str(), theCommands);
    }
    if (thePower <= 0) {
      if (baseRat.GetDeterminant() == 0) {
        return output.makeError("Division by zero: trying to raise 0 to negative power. ", theCommands);
      }
    }
    if (thePower < 0) {
      baseRat.invert();
      thePower *= - 1;
    }
    Matrix<Rational> idMat;
    idMat.MakeIdMatrix(baseRat.numberOfRows);
    MathRoutines::raiseToPower(baseRat, thePower, idMat);
    return output.AssignMatrix(baseRat, theCommands);
  }
  Matrix<AlgebraicNumber> baseAlg;
  if (theCommands.functionGetMatrix(matrixE, baseAlg)) {
    if (!baseAlg.IsSquare() || baseAlg.numberOfColumns == 0) {
      return output.makeError("Exponentiating non-square matrices or matrices with zero rows is not allowed.", theCommands);
    }
    if (thePower <= 0) {
      if (baseAlg.GetDeterminant() == 0) {
        return output.makeError("Division by zero: trying to raise 0 to negative power. ", theCommands);
      }
    }
    if (thePower < 0) {
      baseAlg.invert();
      thePower *= - 1;
    }
    Matrix<AlgebraicNumber> idMat;
    idMat.MakeIdMatrix(baseAlg.numberOfRows);
    MathRoutines::raiseToPower(baseAlg, thePower, idMat);
    return output.AssignMatrix(baseAlg, theCommands);
  }
  Matrix<RationalFunction> baseRF;
  ExpressionContext theContext(theCommands);
  if (theCommands.functionGetMatrix(matrixE, baseRF, &theContext)) {
    if (!baseRF.IsSquare() || baseRF.numberOfColumns == 0) {
      return output.makeError(
        "Exponentiating non-square matrices or matrices "
        "with zero rows is not allowed.",
        theCommands
      );
    }
    if (thePower <= 0) {
      if (baseRF.GetDeterminant() == 0) {
        return output.makeError(
          "Division by zero: trying to raise 0 to negative power. ",
          theCommands
        );
      }
    }
    if (thePower < 0) {
      return theCommands
      << "Raising matrices of rational functions to "
      << "negative powers not implemented yet. ";
    }
    Matrix<RationalFunction> idMat;
    idMat.MakeIdMatrix(baseRF.numberOfRows);
    MathRoutines::raiseToPower(baseRF, thePower, idMat);
    return output.AssignMatrix(baseRF, theCommands, &theContext);
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberPolynomialBySmallInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberPolynomialBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Polynomial<AlgebraicNumber> base;
  int thePower = 0;
  if (!input[1].IsOfType(&base)|| !input[2].IsSmallInteger(&thePower)) {
    return false;
  }
  if (thePower < 0) {
    return false;
  }
  if (base.isEqualToZero() && thePower <= 0) {
    return output.makeError("Division by zero: trying to raise 0 to negative power. ", theCommands);
  }
  base.raiseToPower(thePower, 1);
  return output.assignValueWithContext(base, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerPolynomialModuloIntegerBySmallInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerPolynomialModuloIntegerBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Polynomial<ElementZmodP> base;
  int thePower = 0;
  if (!input[1].IsOfType(&base)|| !input[2].IsSmallInteger(&thePower)) {
    return false;
  }
  if (thePower < 0) {
    return false;
  }
  if (base.isEqualToZero()) {
    if (thePower < 0) {
      return output.makeError("Division by zero: trying to raise 0 to negative power. ", theCommands);
    }
    if (thePower == 0) {
      return false;
    }
    return output.assignValueWithContext(base, input[1].GetContext(), theCommands);
  }
  base.raiseToPower(thePower, base.coefficients[0].one());
  return output.assignValueWithContext(base, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerPolynomialModPModuloPolynomialModPBySmallInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerPolynomialModPModuloPolynomialModPBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  PolynomialModuloPolynomial<ElementZmodP> base;
  int thePower = 0;
  if (!input[1].IsOfType(&base)|| !input[2].IsSmallInteger(&thePower)) {
    return false;
  }
  if (thePower < 0) {
    return false;
  }
  if (base.isEqualToZero()) {
    if (thePower < 0) {
      return output.makeError("Division by zero: trying to raise 0 to negative power. ", theCommands);
    }
    if (thePower == 0) {
      return false;
    }
    return output.assignValueWithContext(base, input[1].GetContext(), theCommands);
  }
  MathRoutines::raiseToPower(base, thePower, base.one());
  return output.assignValueWithContext(base, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerRadicalAlgebraicNumberPositiveDefault(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerRadicalAlgebraicNumberPositiveDefault");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  AlgebraicNumber base;
  if (!input[1].IsOfType(&base)) {
    return false;
  }
  if (base.IsRational()) {
    // We let other rules handle the case of a rational argument.
    return false;
  }
  Rational power;
  if (!input[2].IsOfType(&power)) {
    // exponent is not rational.
    return false;
  }
  LargeInteger radicalLarge = power.GetDenominator();
  LargeInteger powerIntegral = power.GetNumerator();
  int radical = 0;
  int powerSmallInteger = 0;
  if (
    !radicalLarge.IsIntegerFittingInInt(&radical) ||
    !powerIntegral.IsIntegerFittingInInt(&powerSmallInteger)
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
    theCommands << "Not attempting to reduce radical: larger than maximum of "
    << maximumRadicalAllowed << ". ";
    return false;
  }
  if (powerIntegral > LargeIntegerUnsigned::SquareRootOfCarryOverBound) {
    // power too large
    return false;
  }
  AlgebraicClosureRationals fieldCopy;
  fieldCopy = theCommands.theObjectContainer.theAlgebraicClosure;
  AlgebraicNumber baseCopy;
  baseCopy = base;
  baseCopy.owner = &fieldCopy;
  if (!baseCopy.RadicalMeDefault(radical, &theCommands.comments)) {
    return false;
  }
  if (fieldCopy.basisInjections.size != theCommands.theObjectContainer.theAlgebraicClosure.basisInjections.size) {
    // the radical does not below to the base field;
    return false;
  }
  baseCopy.owner = &theCommands.theObjectContainer.theAlgebraicClosure;
  MathRoutines::raiseToPower(baseCopy, powerIntegral, baseCopy.owner->One());
  return output.assignValue(baseCopy, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberBySmallInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  AlgebraicNumber base;
  if (!input[1].IsOfType(&base)) {
    return false;
  }
  Rational powerRat, baseRat;
  if (input[2].IsRational(&powerRat)) {
    if (powerRat.GetDenominator() == 2) {
      if (base.IsRational(&baseRat)) {
        if (base.AssignRationalQuadraticRadical(
          baseRat,
          theCommands.theObjectContainer.theAlgebraicClosure,
          &theCommands.comments
        )) {
          base.checkConsistency();
          output = input;
          Expression newPower, newBase;
          newPower.assignValue(powerRat * 2, theCommands);
          newBase.assignValue(base, theCommands);
          return output.MakeXOX(
            theCommands,
            theCommands.opThePower(),
            newBase,
            newPower
          );
        }
      }
    }
  }
  int thePower = 0;
  if (!input[2].IsSmallInteger(&thePower)) {
    return false;
  }
  if (base.isEqualToZero() && thePower < 0) {
    return output.makeError("Division by zero: trying to raise 0 to negative power. ", theCommands);
  }
  if (base.isEqualToZero() && thePower == 0) {
    return output.assignValue(1, theCommands);
  }
  if (thePower < 0) {
    thePower *= - 1;
    base.invert();
  }
  MathRoutines::raiseToPower(base, thePower, AlgebraicNumber(1));
  return output.assignValueWithContext(base, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerEWABySmallInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerEWABySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  ElementWeylAlgebra<Rational> base;
  int thePower = 0;
  if (!input[1].IsOfType(&base)) {
    return false;
  }
  bool mustCheckForRationalPower = false;
  mustCheckForRationalPower = !input[2].IsSmallInteger(&thePower);
  if (mustCheckForRationalPower || thePower < 0) {
    Rational powerRat;
    if (!input[2].IsOfType<Rational>(&powerRat)) {
      return false;
    }
    bool isMon = true;
    if (base.size() != 1) {
      isMon = false;
    } else if (base.coefficients[0] != 1) {
      isMon = false;
    }
    if (!isMon) {
      return theCommands << "<hr>Failed to raise " << base.toString() << " to power "
      << powerRat.toString() << ": the exponent is not a "
      << " small integer and the base is not a coefficient one monomial. ";
    }
    ElementWeylAlgebra<Rational> finalOutput;
    MonomialWeylAlgebra theMon = base[0];
    theMon.polynomialPart.raiseToPower(powerRat);
    theMon.differentialPart.raiseToPower(powerRat);
    for (int i = 0; i < theMon.polynomialPart.minimalNumberOfVariables(); i ++) {
      if (theMon.polynomialPart(i) != 0 && theMon.differentialPart(i) != 0) {
        return theCommands << "<hr>Failed to raise " << base.toString() << " to power "
        << powerRat.toString() << ": the exponent is not a "
        << "small integer, the base is a monomial, however the monomial "
        << "contains derivative and polynomial with respect to the same variable. ";
      }
    }
    finalOutput.makeZero();
    finalOutput.addMonomial(theMon, 1);
    return output.assignValueWithContext(finalOutput, input[1].GetContext(), theCommands);
  }

  if (base.isEqualToZero()) {
    if (thePower < 0) {
      return output.makeError("Division by zero: trying to raise 0 to negative power. ", theCommands);
    }
    if (thePower == 0) {
      return output.assignValue(1, theCommands);
    }
  }
  base.raiseToPower(thePower);
  return output.assignValueWithContext(base, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerRationalByInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerRatByRat");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational base, exp;
  if (!input[1].IsRational(&base)) {
    return false;
  }
  if (base.IsEqualToOne()) {
    return output.assignValue(1, theCommands);
  }
  if (!input[2].IsRational(&exp)) {
    return false;
  }
  int thePower;
  if (!exp.IsSmallInteger(&thePower)) {
    return false;
  }
  if (base == 0 && thePower == 0) {
    return output.assignValue(1, theCommands);
  }
  if (base == 0 && thePower < 0) {
    return output.makeError("Division by zero: trying to raise 0 to negative or zero power. ", theCommands);
  }
  base.raiseToPower(thePower);
  return output.assignValue(base, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.MergeContextsMyAruments(inputConverted, &theCommands.comments)) {
    return false;
  }
  ElementUniversalEnveloping<RationalFunction> theUE;
  Expression copyExponent = inputConverted[2];
  Expression copyBase = inputConverted[1];
  if (!copyBase.IsOfType<ElementUniversalEnveloping<RationalFunction> >(&theUE)) {
    return false;
  }
  if (!theUE.IsAPowerOfASingleGenerator()) {
    int tempPower;
    if (!copyExponent.IsSmallInteger(&tempPower)) {
      return false;
    }
    theUE.raiseToPower(tempPower);
    return output.assignValueWithContext(theUE, copyBase.GetContext(), theCommands);
  }
  Expression exponentConverted;
  if (!copyExponent.ConvertInternally<RationalFunction>(exponentConverted)) {
    return false;
  }
  MonomialUniversalEnveloping<RationalFunction> theMon;
  theMon = theUE[0];
  theMon.Powers[0] *= exponentConverted.getValue<RationalFunction>();
  ElementUniversalEnveloping<RationalFunction> outputUE;
  outputUE.makeZero(*theUE.owner);
  outputUE.addMonomial(theMon, 1);
  return output.assignValueWithContext(outputUE, copyBase.GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerSequenceOrMatrixByT(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerSequenceOrMatrixByT");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.startsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  if (!input[1].IsSequenceNElementS() && !input[1].IsMatrix()) {
    return false;
  }
  if (!input[2].IsOperationGiven("t")  && !input[2].IsOperationGiven("T")) {
    return false;
  }
  return theCommands.innerTranspose(theCommands, input[1], output);
}

bool CalculatorFunctionsBinaryOps::innerPowerMatrixExpressionsBySmallInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerMatrixExpressionsBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.startsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  if (!input[1].IsMatrix()) {
    return false;
  }
  int thePower = 0;
  if (!input[2].IsSmallInteger(&thePower)) {
    return false;
  }
  if (thePower <= 0) {
    return false;
  }
  Matrix<Expression> theMat;
  if (!theCommands.GetMatrixExpressions(input[1], theMat)) {
    return false;
  }
  if (!theMat.IsSquare()) {
    return output.makeError("Attempting to raise non-square matrix to power", theCommands);
  }
  LargeInteger expectedNumTerms;
  expectedNumTerms = theMat.numberOfColumns;
  expectedNumTerms.raiseToPower(thePower);
  if (expectedNumTerms > 10000) {
    return theCommands << "The expected number terms in the result of the exponentiation "
    << theMat.toString() << " to the power of " << thePower << " is approximately ("
    << theMat.numberOfColumns << ")^" << thePower << "=" << expectedNumTerms
    << ". I have been instructed to proceed only if the expected number of terms is fewer than 10000. ";
  }
  Matrix<Expression> idMatE;
  idMatE.MakeIdMatrix(theMat.numberOfRows, theCommands.EOne(), theCommands.EZero());
  MathRoutines::raiseToPower(theMat, thePower, idMatE);
  return output.AssignMatrixExpressions(theMat, theCommands, true, true);
}

bool CalculatorFunctionsBinaryOps::innerPowerRationalByRationalReducePrimeFactors(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerRatByRatReducePrimeFactors");
  if (!input.startsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  Rational base, exponentWorking, exponentStarting;
  if (!input[1].IsOfType<Rational>(&base)) {
    return false;
  }
  if (!input[2].IsOfType<Rational>(&exponentStarting)) {
    return false;
  }
  if (base == 0) {
    if (exponentStarting < 0) {
      return output.makeError("Division by zero while evaluating " + input.toString(), theCommands);
    }
    return output.assignValue(0, theCommands);
  }
  if (!base.IsInteger()) {
    if (base.GetNumerator() == 1) {
      Expression theDenBase, theDenominator;
      theDenBase.assignValue(Rational(base.GetDenominator()), theCommands);
      theDenominator.MakeXOX(theCommands, theCommands.opThePower(), theDenBase, input[2]);
      output = theCommands.EOne() / theDenominator;
      return true;
    }
  }
  exponentWorking = exponentStarting;
  if (exponentWorking.GetDenominator() == 2) {
    return false;
  }
  LargeIntegerUnsigned exponentDenominator = exponentWorking.GetDenominator();
  LargeIntegerUnsigned exponentNumeratorNoSign = exponentWorking.GetNumerator().value;
  List<LargeInteger> numeratorFactors, denominatorFactors;
  List<int> numeratorPowersInt, denominatorPowersInt;
  if (!base.GetPrimeFactorsAbsoluteValue(numeratorFactors, numeratorPowersInt, denominatorFactors, denominatorPowersInt)) {
    return false;
  }
  List<LargeIntegerUnsigned> numeratorPowerS, denominatorPowerS;
  numeratorPowerS = numeratorPowersInt;
  denominatorPowerS = denominatorPowersInt;
  for (int i = 0; i < numeratorFactors.size; i ++) {
    numeratorPowerS[i] *= exponentNumeratorNoSign;
  }
  for (int i = 0; i < denominatorFactors.size; i ++) {
    denominatorPowerS[i] *= exponentNumeratorNoSign;
  }
  exponentWorking /= exponentNumeratorNoSign;
  Rational outsideOfTheRadical = 1;
  LargeInteger currentInsidePower, currentOutsidePower, currentOutside;
  LargeIntegerUnsigned currentPower;
  int currentInsidePowerInt = - 1, currentOutsidePowerInt = - 1;
  for (int k = 0; k < 2; k ++) {
    List<LargeIntegerUnsigned>& currentPowers = (k == 0) ? numeratorPowerS : denominatorPowerS;
    List<LargeInteger>& currentFactors = (k == 0) ? numeratorFactors : denominatorFactors;
    for (int i = 0; i < currentFactors.size; i ++) {
      currentPower = currentPowers[i];
      currentInsidePower = currentPower % exponentDenominator;
      currentOutsidePower = currentPower / exponentDenominator;
      if (!currentInsidePower.IsIntegerFittingInInt(&currentInsidePowerInt)) {
        return false;
      }
      if (!currentOutsidePower.IsIntegerFittingInInt(&currentOutsidePowerInt)) {
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
  if (numeratorPowerS.size > 0) {
    theGCD = numeratorPowerS[0];
  } else if (denominatorPowerS.size > 0) {
    theGCD = denominatorPowerS[0];
  }
  for (int i = 0; i < numeratorPowerS.size; i ++) {
    theGCD = MathRoutines::gcd(theGCD, numeratorPowerS[i]);
  }
  for (int i = 0; i < denominatorPowerS.size; i ++) {
    theGCD = MathRoutines::gcd(theGCD, denominatorPowerS[i]);
  }
  if (theGCD > 0) {
    for (int i = 0; i < numeratorPowerS.size; i ++) {
      numeratorPowerS[i] /= theGCD;
    }
    for (int i = 0; i < denominatorPowerS.size; i ++) {
      denominatorPowerS[i] /= theGCD;
    }
    exponentWorking *= theGCD;
    exponentDenominator = exponentWorking.GetDenominator();
  }
  Rational insideTheRadical = 1;
  LargeInteger currentContribution, currentNumerator = 1, currentDenominator = 1;
  int currentExpSmallInt = - 1;
  for (int i = 0; i < numeratorPowerS.size; i ++) {
    currentContribution = numeratorFactors[i];
    if (!numeratorPowerS[i].IsIntegerFittingInInt(&currentExpSmallInt)) {
      return false;
    }
    currentContribution.raiseToPower(currentExpSmallInt);
    currentNumerator *= currentContribution;
    if (!currentNumerator.IsIntegerFittingInInt(nullptr)) {
      return false;
    }
  }
  for (int i = 0; i < denominatorPowerS.size; i ++) {
    currentContribution = denominatorFactors[i];
    if (!denominatorPowerS[i].IsIntegerFittingInInt(&currentExpSmallInt)) {
      return false;
    }
    currentContribution.raiseToPower(currentExpSmallInt);
    currentDenominator *= currentContribution;
    if (!currentDenominator.IsIntegerFittingInInt(nullptr)) {
      return false;
    }
  }
  insideTheRadical = currentNumerator;
  insideTheRadical /= currentDenominator;
  if (base < 0) {
    exponentDenominator = exponentWorking.GetDenominator();//<-just in case
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
  theRadicalCFE.assignValue(outsideOfTheRadical, theCommands);
  insideTheRadicalE.assignValue(insideTheRadical, theCommands);
  exponentE.assignValue(exponentWorking, theCommands);
  theRadicalE.MakeXOX(theCommands, theCommands.opThePower(), insideTheRadicalE, exponentE);
  return output.MakeProducT(theCommands, theRadicalCFE, theRadicalE);
}

bool CalculatorFunctionsBinaryOps::innerPowerDoubleOrRatToDoubleOrRat(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerRatPowerRat");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational base, exp;
  double baseDouble, expDouble;
  if (input[1].IsRational(&base)) {
    baseDouble = base.GetDoubleValue();
  } else if (!input[1].IsOfType(&baseDouble)) {
    return false;
  }
  if (input[2].IsRational(&exp)) {
    expDouble = exp.GetDoubleValue();
  } else if (!input[2].IsOfType(&expDouble)) {
    return false;
  }
  if (baseDouble < 0) {
    if (!input[2].IsRational()) {
      return false;
    }
    int thePower = 0;
    if (exp.IsEven()) {
      if (!exp.IsSmallInteger(&thePower)) {
        return false;
      }
      return output.assignValue(FloatingPoint::Power(- baseDouble, thePower), theCommands);
    }
    baseDouble *= - 1;
    return output.assignValue(-FloatingPoint::Power(baseDouble, expDouble), theCommands);
  }
  if (baseDouble == 0.0) {
    if (expDouble > 0) {
      return output.assignValue<double>(0, theCommands);
    }
  }
  return output.assignValue(FloatingPoint::Power(baseDouble, expDouble), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyDoubleOrRatByDoubleOrRat(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerRatPowerRat");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational leftR, rightR;
  double leftD, rightD;
  if (input[1].IsOfType(&leftR)) {
    leftD = leftR.GetDoubleValue();
  } else if (!input[1].IsOfType(&leftD)) {
    return false;
  }
  if (input[2].IsOfType(&rightR)) {
    rightD = rightR.GetDoubleValue();
  } else if (!input[2].IsOfType(&rightD)) {
    return false;
  }
  return output.assignValue(leftD * rightD, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddDoubleOrRatToDoubleOrRat(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerRatPowerRat");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational leftR, rightR;
  double leftD, rightD;
  if (input[1].IsOfType(&leftR)) {
    leftD = leftR.GetDoubleValue();
  } else if (!input[1].IsOfType(&leftD)) {
    return false;
  }
  if (input[2].IsOfType(&rightR)) {
    rightD = rightR.GetDoubleValue();
  } else if (!input[2].IsOfType(&rightD)) {
    return false;
  }
  return output.assignValue(leftD + rightD, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyCharSSLieAlgByCharSSLieAlg(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyCharSSLieAlgByCharSSLieAlg");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.isListNElements(3)) {
    return false;
  }
  CharacterSemisimpleLieAlgebraModule<Rational> leftC, rightC;
  if (!input[1].IsOfType(&leftC)) {
    return false;
  }
  if (!input[2].IsOfType(&rightC)) {
    return false;
  }
  if (leftC.GetOwner() != rightC.GetOwner()) {
    return theCommands << "You asked me to multiply characters over "
    << "different semisimple Lie algebras. Could this be a typo?";
  }
  std::string successString = (leftC *= rightC);
  if (successString != "") {
    return theCommands << "I tried to multiply character " << leftC.toString() << " by " << rightC.toString()
    << " but I failed with the following message: " << successString;
  }
  return output.assignValue(leftC, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarByMatrix(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarByMatrix");
  if (!input.startsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  const Expression& theScalarE = input[1];
  if (!theScalarE.IsBuiltInScalar()) {
    return false;
  }
  const Expression& theMatE = input[2];
  if (!theMatE.IsMatrix()) {
    return false;
  }
  Matrix<Expression> theMat;
  if (!theCommands.GetMatrixExpressions(theMatE, theMat)) {
    return false;
  }
  for (int i = 0; i < theMat.numberOfRows; i ++) {
    for (int j = 0; j < theMat.numberOfColumns; j ++) {
      theMat(i, j) = theScalarE * theMat(i, j);
    }
  }
  return output.AssignMatrixExpressions(theMat, theCommands, false, true);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence");
  if (!input.startsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  if (!input[1].IsBuiltInScalar()) {
    return false;
  }
  if (!input[2].IsSequenceNElementS()) {
    return false;
  }
  output.reset(theCommands);
  output.children.reserve(input[2].size());
  output.addChildAtomOnTop(theCommands.opSequence());
  Expression tempProduct;
  for (int i = 1; i < input[2].size(); i ++) {
    tempProduct.MakeProducT(theCommands, input[1], input[2][i]);
    output.addChildOnTop(tempProduct);
  }
  return true;
}

bool CalculatorFunctionsBinaryOps::innerMultiplyMatrixByMatrix(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyMatrixByMatrix");
  if (!input.startsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  int numRowsLeft = - 1;
  int numColsLeft = - 1;
  int numRowsRight = - 1;
  int numColsRight = - 1;
  const Expression& left = input[1];
  const Expression& right = input[2];
  if (
    !left.IsMatrix(&numRowsLeft, &numColsLeft) ||
    !input[2].IsMatrix(&numRowsRight, &numColsRight)
  ) {
    return false;
  }
  if (numColsLeft != numRowsRight) {
    return theCommands << "WARNING: I encountered a product of a matrix with " << numColsLeft
    << " columns by a matrix with " << numRowsRight << " rows. ";
  }
  if (
    left.IsMatrixOfType<Rational>() &&
    right.IsMatrixOfType<Rational>()
  ) {
    return CalculatorFunctionsBinaryOps::innerMultiplyMatrixRationalOrRationalByMatrixRational(
      theCommands, input, output
    );
  }
  bool invokeAlgMatMultiplication =
  (left.IsMatrixOfType<Rational>()        && right.IsMatrixOfType<AlgebraicNumber>()) ||
  (left.IsMatrixOfType<AlgebraicNumber>() && right.IsMatrixOfType<Rational>()) ||
  (left.IsMatrixOfType<AlgebraicNumber>() && right.IsMatrixOfType<AlgebraicNumber>());
  if (invokeAlgMatMultiplication) {
    return CalculatorFunctionsBinaryOps::innerMultiplyMatRatOrMatAlgByMatRatOrMatAlg(theCommands, input, output);
  }
  bool invokeRFMultiplication =
  (left.IsMatrixOfType<RationalFunction>() && right.IsMatrixOfType<Rational>()) ||
  (left.IsMatrixOfType<Rational>()            && right.IsMatrixOfType<RationalFunction>()) ||
  (left.IsMatrixOfType<RationalFunction>() && right.IsMatrixOfType<RationalFunction>() );
  if (invokeRFMultiplication) {
    return CalculatorFunctionsBinaryOps::innerMultiplyMatrixRFOrRFByMatrixRF(
      theCommands, input, output
    );
  }
  Matrix<Expression> leftMat, rightMat;
  if (!theCommands.GetMatrixExpressions(left, leftMat)) {
    return false;
  }
  if (!theCommands.GetMatrixExpressions(input[2], rightMat, leftMat.numberOfColumns)) {
    return false;
  }
  if (leftMat.numberOfColumns != rightMat.numberOfRows) {
    return false;
  }
  Matrix<Expression> outputMat;
  outputMat.init(leftMat.numberOfRows, rightMat.numberOfColumns);
  Expression leftSummand, rightSummand;
  for (int i = 0; i < leftMat.numberOfRows; i ++) {
    for (int j = 0; j < rightMat.numberOfColumns; j ++) {
      for (int k = 0; k < leftMat.numberOfColumns; k ++) {
        if (k == 0) {
          outputMat(i, j).MakeProducT(theCommands, leftMat(i, k), rightMat(k, j));
        } else {
          rightSummand = outputMat(i, j);
          leftSummand.MakeProducT(theCommands, leftMat(i, k), rightMat(k, j));
          outputMat(i, j).MakeXOX(theCommands, theCommands.opPlus(), leftSummand, rightSummand);
        }
      }
    }
  }
  return output.AssignMatrixExpressions(outputMat, theCommands, true, true);
}

bool CalculatorFunctionsBinaryOps::innerMultiplySequenceByMatrix(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyMatrixByMatrix");
  if (!input.startsWith(theCommands.opTimes())) {
    return false;
  }
  int matRows = - 1, matCols = - 1;
  if (!input[1].IsSequenceNElementS() || !input[2].IsMatrix(&matRows, &matCols)) {
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
  return output.MakeSequence(theCommands, &theResult);
}

bool CalculatorFunctionsBinaryOps::innerTensorMatrixByMatrix(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  if (!input.startsWith(theCommands.opTensor(), 3)) {
    return false;
  }
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerTensorMatrixByMatrix");
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if ((!rightE.IsMatrix() && !rightE.IsSequenceNElementS()) || (!leftE.IsMatrix() && !leftE.IsSequenceNElementS())) {
    return false;
  }
  Matrix<Rational> leftMatRat, rightMatRat;
  if (
    theCommands.functionGetMatrix(leftE, leftMatRat) &&
    theCommands.functionGetMatrix(rightE, rightMatRat)
  ) {
    Matrix<Rational> resultMatRat;
    resultMatRat.assignTensorProduct(leftMatRat, rightMatRat);
    return output.AssignMatrix(resultMatRat, theCommands, nullptr, true);
  }
  Matrix<Expression> leftMatE, rightMatE, resultMatE;
  if (!theCommands.GetMatrixExpressions(input[1], leftMatE)) {
    return false;
  }
  if (!theCommands.GetMatrixExpressions(input[2], rightMatE)) {
    return false;
  }
  resultMatE.assignTensorProduct(leftMatE, rightMatE);
  return output.AssignMatrixExpressions(resultMatE, theCommands, true, true);
}

bool CalculatorFunctionsBinaryOps::innerTensorMatByMatTensor(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  if (!input.startsWith(theCommands.opTensor())) {
    return false;
  }
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerTensorMatByMatTensor");
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!rightE.IsOfType<MatrixTensor<Rational> >() || !leftE.IsOfType<MatrixTensor<Rational> >()) {
    return false;
  }
  MatrixTensor<Rational> result;
  result.assignTensorProduct(leftE.getValue<MatrixTensor<Rational> >(), rightE.getValue<MatrixTensor<Rational> >());
  return output.assignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRatOrAlgebraicByMatRatOrMatAlg(
  Calculator& theCommands, const Expression& input, Expression& output
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
    !matE->IsMatrixOfType<Rational>() &&
    !matE->IsMatrixOfType<AlgebraicNumber>()
  ) {
    MathRoutines::swap(matE, scalarE);
  }
  if (
    !matE->IsMatrixOfType<Rational>() &&
    !matE->IsMatrixOfType<AlgebraicNumber>()
  ) {
    return false;
  }
  Matrix<AlgebraicNumber> theMatAlg;
  AlgebraicNumber theScalar;
  Matrix<Rational> theMatRat;
  Rational theScalarRat;
  if (theCommands.functionGetMatrix(*matE, theMatRat)) {
    theMatAlg = theMatRat;
  } else {
    if (!theCommands.functionGetMatrix(*matE, theMatAlg)) {
      return false;
    }
  }
  if (scalarE->IsOfType(&theScalarRat)) {
    theScalar = theScalarRat;
  } else {
    if (!scalarE->IsOfType(&theScalar)) {
      return false;
    }
  }
  theMatAlg *= theScalar;
  return output.AssignMatrix(theMatAlg, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyMatRatOrMatAlgByMatRatOrMatAlg(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyMatRatOrMatAlgByMatRatOrMatAlg");
  if (!input.isListNElements(3)) {
    return false;
  }
  Matrix<AlgebraicNumber> matAlgLeft, matAlgRight;
  Matrix<Rational> matRatLeft, matRatRight;
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (leftE.IsMatrixOfType<Rational>() && rightE.IsMatrixOfType<Rational>()) {
    return false;
  }
  if (theCommands.functionGetMatrix(leftE, matRatLeft)) {
    matAlgLeft = matRatLeft;
  } else {
    if (!theCommands.functionGetMatrix(leftE, matAlgLeft)) {
      return false;
    }
  }
  if (theCommands.functionGetMatrix(rightE, matRatRight)) {
    matAlgRight = matRatRight;
  } else {
    if (!theCommands.functionGetMatrix(rightE, matAlgRight)) {
      return false;
    }
  }
  if (matAlgLeft.numberOfColumns != matAlgRight.numberOfRows) {
    std::stringstream errorStream;
    errorStream << "Error: attempting to multiply matrix with "
    << matAlgLeft.numberOfColumns << " columns by a "
    << "matrix with " << matAlgRight.numberOfRows << " rows. ";
    return output.makeError(errorStream.str(), theCommands);
  }
  matAlgLeft *= matAlgRight;
  return output.AssignMatrix(matAlgLeft, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyMatrixRationalOrRationalByMatrixRational(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyMatrixRationalOrRationalByMatrixRational");
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!rightE.IsMatrixOfType<Rational>()) {
    return false;
  }
  Matrix<Rational> leftMat, rightMat;
  if (!theCommands.functionGetMatrix(rightE, rightMat)) {
    return false;
  }
  Rational theScalar;
  if (leftE.IsOfType<Rational>(&theScalar)) {
    leftMat *= theScalar;
    return output.AssignMatrix(leftMat, theCommands);
  }
  if (!leftE.IsMatrixOfType<Rational>()) {
    return false;
  }
  if (!theCommands.functionGetMatrix(leftE, rightMat)) {
    return false;
  }
  if (leftMat.numberOfColumns != rightMat.numberOfRows) {
    return false;
  }
  leftMat.MultiplyOnTheRight(rightMat);
  return output.AssignMatrix(leftMat, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyMatrixRFOrRFByMatrixRF(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyMatrixRFOrRFByMatrixRF");
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression leftE = input[1];
  Expression rightE = input[2];
  if (
    !rightE.IsMatrixOfType<RationalFunction>() || !(
      leftE.IsOfType<Rational>() ||
      leftE.IsOfType<Polynomial<Rational> >() ||
      leftE.IsOfType<RationalFunction>() ||
      leftE.IsMatrixOfType<RationalFunction>()
    )
  ) {
    return false;
  }
  if (!leftE.MergeContexts(leftE, rightE)) {
    return theCommands << "Failed to convert "
    << leftE.toString() << " and " << rightE.toString()
    << " to common context. ";
  }
  Matrix<RationalFunction> leftMat, rightMat;
  if (!theCommands.functionGetMatrix(rightE, rightMat)) {
    return false;
  }
  if (!leftE.IsMatrixOfType<RationalFunction>()) {
    Expression leftErfForm;
    if (!leftE.ConvertInternally<RationalFunction>(leftErfForm)) {
      return theCommands << "Failed to convert " << leftE.toString() << " to rational function. ";
    }
    RationalFunction theScalar = leftErfForm.getValue<RationalFunction>();
    rightMat *= theScalar;
    ExpressionContext contextE = leftE.GetContext();
    return output.AssignMatrix(rightMat, theCommands, &contextE);
  }
  if (!theCommands.functionGetMatrix(leftE, leftMat)) {
    return false;
  }
  if (leftMat.numberOfColumns != rightMat.numberOfRows) {
    return false;
  }
  leftMat.MultiplyOnTheRight(rightMat);
  ExpressionContext contextE = leftE.GetContext();
  return output.AssignMatrix(leftMat, theCommands, &contextE);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyMatrixTensorOrRationalByMatrixTensor(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyMatrixTensorOrRationalByMatrixTensor");
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!rightE.IsOfType<MatrixTensor<Rational> >()) {
    return false;
  }
  Rational theScalar;
  if (leftE.IsOfType<Rational>(&theScalar)) {
    MatrixTensor<Rational> result = rightE.getValue<MatrixTensor<Rational> >();
    result *= theScalar;
    return output.assignValue(result, theCommands);
  }
  if (!leftE.IsOfType<MatrixTensor<Rational> >()) {
    return false;
  }
  const MatrixTensor<Rational>& rightMat = rightE.getValue<MatrixTensor<Rational> >();
  MatrixTensor<Rational> result = leftE.getValue<MatrixTensor<Rational> >();
  result *= rightMat;
  return output.assignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerLieBracketExtractConstant(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerLieBracketExtractConstant");
  if (!input.startsWith(theCommands.opLieBracket(), 3)) {
    return false;
  }
  Rational theCF = 1;
  Rational tempRat;
  Expression leftE = input[1], rightE = input[2];
  bool found = false;
  if (input[1].startsWith(theCommands.opTimes(), 3)) {
    if (input[1][1].IsOfType(&tempRat)) {
      found = true;
      theCF *= tempRat;
      leftE = input[1][2];
    }
  }
  if (input[2].startsWith(theCommands.opTimes(), 3)) {
    if (input[2][1].IsOfType(&tempRat)) {
      found = true;
      theCF *= tempRat;
      rightE = input[2][2];
    }
  }
  if (!found) {
    return false;
  }
  Expression theBracket;
  theBracket.MakeXOX(theCommands, theCommands.opLieBracket(), leftE, rightE);
  output.assignValue(theCF, theCommands);
  output *= theBracket;
  return true;
}

bool CalculatorFunctionsBinaryOps::innerLieBracketDistribute(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerLieBracketDistribute");
  return theCommands.outerDistribute(
    theCommands, input, output, theCommands.opPlus(),theCommands.opLieBracket()
  );
}

bool CalculatorFunctionsBinaryOps::innerLieBracketRatOrUEWithRatOrUE(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerLieBracketRatOrUEWithRatOrUE");
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.MergeContextsMyAruments(inputConverted, nullptr)) {
    return false;
  }
  const Expression& leftE = inputConverted[1];
  const Expression& rightE = inputConverted[2];
  if (leftE.IsOfType<Rational>() || rightE.IsOfType<Rational>()) {
    return output.assignValueWithContext(0, leftE.GetContext() , theCommands);
  }
  if (
    leftE.IsOfType<ElementUniversalEnveloping<RationalFunction> >() &&
    rightE.IsOfType<ElementUniversalEnveloping<RationalFunction> >()
  ) {
    ElementUniversalEnveloping<RationalFunction> result;
    leftE.getValue<ElementUniversalEnveloping<RationalFunction> >().LieBracketOnTheRight(
      rightE.getValue<ElementUniversalEnveloping<RationalFunction> >(), result
    );
    result.simplify();
    return output.assignValueWithContext(result, leftE.GetContext(), theCommands);
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerLieBracketJacobiIdentityIfNeeded(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerLieBracketJacobiIdentityIfNeeded");
  if (!input.startsWith(theCommands.opLieBracket(), 3)) {
    return false;
  }
  if (!input[2].startsWith(theCommands.opLieBracket())) {
    return false;
  }
  bool doContinue = (input[1] > input[2][1] && input[1] > input[2][2]);
  if (!doContinue) {
    return false;
  }
  Expression leftE, rightE;
  Expression lieBracket;
  lieBracket.MakeXOX(theCommands, theCommands.opLieBracket(), input[2][2],input[1]);
  leftE.MakeXOX(theCommands, theCommands.opLieBracket(), input[2][1], lieBracket);
  lieBracket.MakeXOX(theCommands, theCommands.opLieBracket(), input[1],input[2][1]);
  rightE.MakeXOX(theCommands, theCommands.opLieBracket(), input[2][2], lieBracket);
  leftE = theCommands.EMOne() * leftE;
  rightE = theCommands.EMOne() * rightE;
  output = leftE + rightE;
  return true;
}

bool CalculatorFunctionsBinaryOps::innerLieBracketSwapTermsIfNeeded(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerLieBracketSwapTermsIfNeeded");
  if (!input.startsWith(theCommands.opLieBracket(), 3)) {
    return false;
  }
  if (input[1].isEqualToZero() || input[2].isEqualToZero()) {
    return output.assignValue(0, theCommands);
  }
  if (input[2] > input[1]) {
    return false;
  }
  if (input[1] == input[2]) {
    return output.assignValue(0, theCommands);
  }
  Expression theBracket;
  theBracket.MakeXOX(theCommands, theCommands.opLieBracket(), input[2], input[1]);
  output = theCommands.EMOne() * theBracket;
  return true;
}

bool CalculatorFunctionsBinaryOps::innerLieBracketRatPolyOrEWAWithRatPolyOrEWA(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerLieBracketRatPolyOrEWAWithRatPolyOrEWA");
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.MergeContextsMyAruments(inputConverted, nullptr)) {
    return false;
  }
  const Expression& leftE = inputConverted[1];
  const Expression& rightE = inputConverted[2];
  bool leftEisGood =
  leftE.IsOfType<Rational>() ||
  leftE.IsOfType<Polynomial<Rational> >() ||
  leftE.IsOfType<ElementWeylAlgebra<Rational> >();
  bool rightEisGood =
  rightE.IsOfType<Rational>() ||
  rightE.IsOfType<Polynomial<Rational> >() ||
  rightE.IsOfType<ElementWeylAlgebra<Rational> >();
  if (!leftEisGood || !rightEisGood) {
    return false;
  }
  if (!leftE.IsOfType<ElementWeylAlgebra<Rational> >() && !rightE.IsOfType<ElementWeylAlgebra<Rational> >()) {
    return output.assignValue(0, theCommands);
  }
  Expression leftConverted, rightConverted;
  if (
    !leftE.ConvertInternally<ElementWeylAlgebra<Rational> >(leftConverted) ||
    !rightE.ConvertInternally<ElementWeylAlgebra<Rational> >(rightConverted)
  ) {
    theCommands << "<hr>Failed with conversion to Element weyl algebra - possible programming error?";
    return false;
  }
  ElementWeylAlgebra<Rational> resultE = rightConverted.getValue<ElementWeylAlgebra<Rational> >();
  resultE.LieBracketOnTheLeft(leftConverted.getValue<ElementWeylAlgebra<Rational> >());
  return output.assignValueWithContext(resultE, leftConverted.GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddMatrixToMatrix(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddMatrixToMatrix");
  if (!input.startsWith(theCommands.opPlus(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  int leftNumRows = - 1, leftNumCols = - 1, rightNumRows = - 1, rightNumCols = - 1;
  if (
    !leftE.IsMatrix(&leftNumRows, &leftNumCols) ||
    !rightE.IsMatrix(&rightNumRows, &rightNumCols)
  ) {
    return false;
  }
  if ((leftNumRows != rightNumRows) || (leftNumCols != rightNumCols)) {
    return false;
  }
  Matrix<Expression> leftMat, rightMat;
  if (
    !theCommands.GetMatrixExpressions(leftE, leftMat) ||
    !theCommands.GetMatrixExpressions(rightE, rightMat)
  ) {
    return false;
  }
  if (leftMat.numberOfColumns != rightMat.numberOfColumns || leftMat.numberOfRows != rightMat.numberOfRows) {
    return false;
  }
  for (int i = 0; i < leftMat.numberOfRows; i ++) {
    for (int j = 0; j < leftMat.numberOfColumns; j ++) {
      leftMat(i, j) += rightMat(i, j);
    }
  }
  return output.AssignMatrixExpressions(leftMat, theCommands, false, true);
}

bool CalculatorFunctionsBinaryOps::innerAugmentMatrixToTheRight(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAugmentMatrixToTheRight");
  if (input.size() != 3) {
    return false;
  }
  Matrix<Expression> leftMat, rightMat;
  if (
    !theCommands.GetMatrixExpressions(input[1], leftMat) ||
    !theCommands.GetMatrixExpressions(input[2], rightMat)
  ) {
    return false;
  }
  if (leftMat.numberOfRows != rightMat.numberOfRows) {
    return theCommands << "Cannot augment the left matrix with: " << leftMat.numberOfRows
    << " rows to the right by a matrix with a different number of rows: "
    << rightMat.numberOfRows << ". ";
  }
  leftMat.appendMatrixOnTheRight(rightMat);
  return output.AssignMatrixExpressions(leftMat, theCommands, false, false);
}

bool CalculatorFunctionsBinaryOps::innerAugmentMatrixBelow(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAugmentMatrixBelow");
  if (input.size() != 3) {
    return false;
  }
  Matrix<Expression> leftMat, rightMat;
  if (
    !theCommands.GetMatrixExpressions(input[1], leftMat) ||
    !theCommands.GetMatrixExpressions(input[2], rightMat)
  ) {
    return false;
  }
  if (leftMat.numberOfRows != rightMat.numberOfRows) {
    return theCommands << "Cannot augment the left matrix with: " << leftMat.numberOfRows
    << " rows to the right by a matrix with a different number of rows: "
    << rightMat.numberOfRows << ". ";
  }
  leftMat.appendMatrixToTheBottom(rightMat);
  return output.AssignMatrixExpressions(leftMat, theCommands, false, false);

}

bool CalculatorFunctionsBinaryOps::innerDirectSumMatrixWithMatrix(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDirectSumMatrixWithMatrix");
  if (!input.startsWith(theCommands.opDirectSum(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!leftE.IsMatrix() || !rightE.IsMatrix()) {
    return false;
  }
  Matrix<Expression> leftMat, rightMat;
  if (
    !theCommands.GetMatrixExpressions(leftE, leftMat) ||
    !theCommands.GetMatrixExpressions(rightE, rightMat)
  ) {
    return false;
  }
  leftMat.DirectSumWith(rightMat,theCommands.EZero());
  return output.AssignMatrixExpressions(leftMat, theCommands, false, true);
}

bool CalculatorFunctionsBinaryOps::innerAddMatrixRationalOrAlgebraicToMatrixRationalOrAlgebraic(
  Calculator& theCommands, const Expression& input, Expression& output
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
    !rightE.IsMatrixOfType<Rational>() ||
    !leftE.IsMatrixOfType<Rational>()
  ) {
    if (rightE.IsMatrixOfType<AlgebraicNumber>()) {
      MathRoutines::swap(leftE, rightE);
    }
    if (!leftE.IsMatrixOfType<AlgebraicNumber>()) {
      return false;
    }
    if (rightE.IsMatrixOfType<Rational>()) {
      if (!theCommands.functionGetMatrix(leftE, leftMatAlg)) {
        return false;
      }
      if (!theCommands.functionGetMatrix(rightE, rightMatAlg)) {
        return false;
      }
      if (
        rightMatAlg.numberOfRows != leftMatAlg.numberOfRows ||
        rightMatAlg.numberOfColumns != leftMatAlg.numberOfColumns
      ) {
        return false;
      }
      leftMatAlg += rightMatAlg;
      return output.AssignMatrix(leftMatAlg, theCommands);
    }
    if (!theCommands.functionGetMatrix(rightE, rightMatAlg)) {
      return false;
    }
    if (!theCommands.functionGetMatrix(leftE, leftMatAlg)) {
      return false;
    }
    if (
      rightMatAlg.numberOfRows != leftMatAlg.numberOfRows ||
      rightMatAlg.numberOfColumns != leftMatAlg.numberOfColumns
    ) {
      return false;
    }
    leftMatAlg += rightMatAlg;
    return output.AssignMatrix(leftMatAlg, theCommands);
  }
  if (!theCommands.functionGetMatrix(leftE, leftMatRat)) {
    return false;
  }
  if (!theCommands.functionGetMatrix(rightE, rightMatRat)) {
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
    return output.makeError(errorStream.str(), theCommands);
  }
  leftMatRat += rightMatRat;
  return output.AssignMatrix(leftMatRat, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerSetMinus(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerSetMinus");
  if (input.size() != 3) {
    return false;
  }
  const Expression& leftSetE = input[1];
  const Expression& rightSetE = input[2];
  if (!leftSetE.IsSequenceNElementS() || !rightSetE.IsSequenceNElementS()) {
    return false;
  }
  if (leftSetE.HasBoundVariables() || rightSetE.HasBoundVariables()) {
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
  return output.MakeSequence(theCommands, &resultEs);
}

bool CalculatorFunctionsBinaryOps::innerAddMatrixRFsToMatrixRFs(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddMatrixRFsToMatrixRFs");
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression leftE = input[1];
  Expression rightE = input[2];
  if (
    !rightE.IsMatrixOfType<RationalFunction>() ||
    !leftE.IsMatrixOfType<RationalFunction>()
  ) {
    return false;
  }
  if (!leftE.MergeContexts(leftE, rightE)) {
    return false;
  }
  Matrix<RationalFunction> leftMat, rightMat;
  if (!theCommands.functionGetMatrix(leftE, leftMat)) {
    return false;
  }
  if (!theCommands.functionGetMatrix(rightE, rightMat)) {
    return false;
  }
  if (
    rightMat.numberOfRows != leftMat.numberOfRows ||
    rightMat.numberOfColumns != leftMat.numberOfColumns
  ) {
    return false;
  }
  leftMat += rightMat;
  ExpressionContext theContext = leftE.GetContext();
  return output.AssignMatrix(leftMat, theCommands, &theContext);
}

bool CalculatorFunctionsBinaryOps::innerAddMatrixTensorToMatrixTensor(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!rightE.IsOfType<MatrixTensor<Rational> >()|| !leftE.IsOfType<MatrixTensor<Rational> >()) {
    return false;
  }
  const MatrixTensor<Rational>& rightMat = rightE.getValue<MatrixTensor<Rational> >();
  const MatrixTensor<Rational>& leftMat = leftE.getValue<MatrixTensor<Rational> >();
  MatrixTensor<Rational> result = leftMat;
  result += rightMat;
  return output.assignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplySequenceByAnyScalar(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplySequenceByAnyScalar");
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression tempE = input;
  tempE.children.swapTwoIndices(1, 2);
  return CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence(theCommands, tempE, output);
}

bool CalculatorFunctionsBinaryOps::innerAddSequenceToSequence(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddSequenceToSequence");
  if (!input.isListNElements(3)) {
    return false;
  }
  if (!input[1].IsSequenceNElementS()) {
    return false;
  }
  if (!input[2].IsSequenceNElementS()) {
    return false;
  }
  if (input[2].size() != input[1].size()) {
    return theCommands << "<hr>Attempting to add a sequence of length "
    << input[1].size() - 1 << "  to a sequence of length "
    << input[2].size() - 1 << ", possible user typo?";
  }
  output.reset(theCommands);
  output.children.reserve(input[1].size());
  output.addChildAtomOnTop(theCommands.opSequence());
  Expression tempSum;
  for (int i = 1; i < input[2].size(); i ++) {
    tempSum.MakeXOX(theCommands, theCommands.opPlus(), input[1][i], input[2][i]);
    output.addChildOnTop(tempSum);
  }
  return true;
}

bool CalculatorFunctionsBinaryOps::innerNChooseK(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerNChooseK");
  if (input.size() != 3) {
    return false;
  }
  Rational N;
  int K = - 1;
  if (!input[2].IsSmallInteger(&K)) {
    return false;
  }
  if (K < 0) {
    return false;
  }
  if (input[1].IsOfType<Rational>(&N)) {
    Rational result = result.NChooseK(N, K);
    return output.assignValue(result, theCommands);
  }
  if (K < 11) {
    output.assignValue(1, theCommands);
    for (int i = 0; i < K; i ++) {
      output *= input[1] - i;
    }
    return true;
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerPowerElementZmodPByInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerElementZmodPByInteger");
  if (!input.startsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  ElementZmodP theElt;
  if (!leftE.IsOfType(&theElt)) {
    return false;
  }
  LargeInteger thePower = 0;
  if (!rightE.IsInteger(&thePower)) {
    return false;
  }
  if (thePower < 0) {
    ElementZmodP copy = theElt;
    theElt.makeOne(theElt.theModulus);
    theElt /= copy;
    thePower *= - 1;
  }
  ElementZmodP unit;
  unit.makeOne(theElt.theModulus);
  MathRoutines::raiseToPower(theElt, thePower, unit);
  return output.assignValue(theElt, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerEllipticCurveRationalElementByInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerEllipticCurveRationalElementByInteger");
  if (!input.startsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  ElementEllipticCurve<Rational> theElt;
  if (!leftE.IsOfType(&theElt)) {
    return false;
  }
  int thePower = 0;
  if (!rightE.IsSmallInteger(& thePower)) {
    return false;
  }
  if (thePower < 0) {
    theElt.invert();
    thePower *= - 1;
  }
  ElementEllipticCurve<Rational> unit;
  unit.makeOne(theElt.owner);
  MathRoutines::raiseToPower(theElt, thePower, unit);
  return output.assignValueWithContext(theElt, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerEllipticCurveZmodPElementByInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerEllipticCurveZmodPElementByInteger");
  if (!input.startsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  ElementEllipticCurve<ElementZmodP> theElt;
  if (!leftE.IsOfType(&theElt)) {
    return false;
  }
  LargeInteger thePower = 0;
  if (!rightE.IsInteger(& thePower)) {
    return false;
  }
  if (thePower < 0) {
    theElt.invert();
    thePower *= - 1;
  }
  ElementEllipticCurve<ElementZmodP> unit;
  unit.makeOne(theElt.owner);
  MathRoutines::raiseToPower(theElt, thePower, unit);
  return output.assignValueWithContext(theElt, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPolynomialModPModuloPolynomialModP(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPolynomialModPModuloPolynomialModP");
  if (input.size() != 3) {
    return false;
  }
  if (
    !input[1].IsOfType<Polynomial<ElementZmodP> >() ||
    !input[2].IsOfType<Polynomial<ElementZmodP> >()
  ) {
    return false;
  }
  Expression inputConverted;
  if (!input.MergeContextsMyArumentsAndConvertThem<Polynomial<ElementZmodP> >(
    inputConverted, &theCommands.comments
  )) {
    return false;
  }
  WithContext<Polynomial<ElementZmodP> > left, right;
  if (
    !inputConverted[1].IsOfTypeWithContext(&left) ||
    !inputConverted[2].IsOfTypeWithContext(&right)
  ) {
    return false;
  }
  if (right.content.isEqualToZero()) {
    return output.makeError("Division by zero", theCommands);
  }
  LargeIntegerUnsigned modulus = right.content.coefficients[0].theModulus;
  if (!left.content.isEqualToZero()) {
    if (left.content.coefficients[0].theModulus != modulus) {
      return theCommands
      << "Attempt to perform operations with modular polynomials with different moduli: "
      << modulus << " and " << left.content.coefficients[0].theModulus;
    }
  }
  ElementZmodP rightLeadingCoefficient = right.content.GetLeadingCoefficient(&MonomialP::orderDefault());
  if (!rightLeadingCoefficient.invert()) {
    return theCommands
    << "Leading coefficient of quotient polynomial is not invertible mod " << modulus << ".";
  }
  PolynomialModuloPolynomial<ElementZmodP> result;
  result.modulus = right.content;
  result.value = left.content;
  result.reduce();
  return output.assignValueWithContext(result, right.context, theCommands);
}
