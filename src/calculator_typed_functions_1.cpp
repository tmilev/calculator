// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "calculator_inner_typed_functions.h"
#include "math_general_implementation.h"
#include "math_extra_Weyl_algebras_implementation.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_extra_semisimple_Lie_algebras_implementation.h"
#include "math_extra_modules_semisimple_Lie_algebras_implementation.h"
#include "math_general_polynomial_computations_basic_implementation.h" //undefined reference to Polynomial<AlgebraicNumber>::MakeOne(int)
#include "math_extra_finite_groups_implementation.h" // undefined reference to `void WeylGroup::RaiseToDominantWeight<Rational>(Vector<Rational>&, int*, bool*, ElementWeylGroup<WeylGroup>*)

static ProjectInformationInstance projectInfoCalculatorTypedFunctionsCPP(__FILE__, "Calculator inner binary typed functions. ");

bool Calculator::innerOperationBinary(Calculator& theCommands, const Expression& input, Expression& output, int theOp) {
  for (int i = 0; i < theCommands.FunctionHandlers[theOp].size; i ++) {
    if (theCommands.FunctionHandlers[theOp][i].inputFitsMyInnerType(input)) {
      if (theCommands.FunctionHandlers[theOp][i].theFunction(theCommands, input, output)) {
        return true;
      }
    }
  }
  return false;
}

bool Calculator::outerExtractBaseMultiplication(Calculator& theCommands, const Expression& input, Expression& output) {
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::outerExtractBaseMultiplication");
  if (!input.StartsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  bool result = false;
  //handle Anything * Rational = Rational * Anything
  output = input;
  if (output[2].IsOfType<Rational>()) {
    output.children.SwapTwoIndices(1, 2);
    result = true;
  }
  if (output[2].IsOfType<double>() && !output[1].IsOfType<Rational>()) {
    output.children.SwapTwoIndices(1, 2);
    result = true;
  }
  if (output[2].IsListStartingWithAtom(theCommands.opTimes())) {
    if (output[2].children.size != 3) {
      return result;
    }
    //handle Anything1 * (Rational * Anything2) = Rational * (Anything1 * Anything2)
    if (output[2][1].IsOfType<Rational>()) {
      Expression tempRight;
      tempRight.MakeXOX(theCommands, theCommands.opTimes(), output[1], output[2][2]);
      output.MakeXOX(theCommands, theCommands.opTimes(), output[2][1], tempRight);
      result = true;
    }
    //<- handle a * (b * anything)
    //on condition that a*b has an inner handler
    Expression tempExp, newExpr;
    tempExp.MakeXOX(theCommands, theCommands.opTimes(), output[1], output[2][1]);
    if (theCommands.innerTimes(theCommands, tempExp, newExpr)) {
      output.MakeProducT(theCommands, newExpr, output[2][2]);
      result = true;
    }
  }
  //handle 0 * anything = 0
  if (output[1].IsEqualToZero()) {
    return output.AssignValue(0, theCommands);
  }
  return result;
}

bool CalculatorFunctionsBinaryOps::innerAddEltZmodPorRatToEltZmodPorRat(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddEltZmodPorRatToEltZmodPorRat");
  if (!input.IsListNElements(3)) {
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
      return output.AssignValue(theElt1, theCommands);
    }
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerMultiplyEltZmodPorRatByEltZmodPorRat(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyEltZmodPorRatByEltZmodPorRat");
  if (!input.IsListNElements(3)) {
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
      return output.AssignValue(theElt1, theCommands);
    }
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerDivideEltZmodPorRatByEltZmodPorRat(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyEltZmodPorRatByEltZmodPorRat");
  if (!input.IsListNElements(3)) {
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
        << theElt1.ToString() << " by " << theElt2.ToString();
        return output.MakeError(out.str(), theCommands, true);
      }
      return output.AssignValue(theElt1, theCommands);
    }
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerAddRatToRat(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddRatToRat");
  if (input.size() != 3) {
    return false;
  }
  Rational leftR, rightR;
  if (!input[1].IsOfType(&leftR) || !input[2].IsOfType(&rightR)) {
    return false;
  }
  return output.AssignValue(leftR + rightR, theCommands);
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
  return output.AssignValue(left + right, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat");
  if (!input.IsListNElements(3)) {
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
  if (rightAN.IsEqualToZero()) {
    return output.MakeError("Division by zero. ", theCommands, true);
  }
  leftAN /= rightAN;
  output.AssignValue(leftAN, theCommands);
  output.CheckInitializationRecursively();
  return true;
}

bool CalculatorFunctionsBinaryOps::innerMultiplyEltHypOctByEltHypOct(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyEltHypOctByEltHypOct");
  if (!input.IsListNElements(3)) {
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
  return output.AssignValue(outElt, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyAlgebraicNumberByAlgebraicNumber(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyAlgebraicNumberByAlgebraicNumber");
  if (!input.IsListNElements(3)) {
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
  return output.AssignValue(leftAN, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber");
  if (!input.IsListNElements(3)) {
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
    leftAN.CheckConsistency();
  } else if (!input[2].IsOfType(&rightAN)) {
    if (!input[2].IsOfType(&tempRat)) {
      return false;
    }
    rightAN.AssignRational(tempRat, theCommands.theObjectContainer.theAlgebraicClosure);
    rightAN.CheckConsistency();
  }
  leftAN.CheckConsistency();
  rightAN.CheckConsistency();
  leftAN += rightAN;
  leftAN.CheckConsistency();
  return output.AssignValue(leftAN, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRatByRat(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRatByRat");
  if (!input.IsListNElements(3)) {
    return false;
  }
  Rational leftR, rightR;
  if (!input[1].IsOfType(&leftR) || !input[2].IsOfType(&rightR)) {
    return false;
  }
  return output.AssignValue(leftR*rightR, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyCoxeterEltByCoxeterElt(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyCoxeterEltByCoxeterElt");
  if (!input.IsListNElements(3)) {
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
  return output.AssignValue(leftR, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerDivideRatByRat(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDivideRatByRat");
  if (!input.IsListNElements(3)) {
    return false;
  }
  Rational leftR, rightR;
  if (!input[1].IsOfType(&leftR) || !input[2].IsOfType(&rightR)) {
    return false;
  }
  if (rightR.IsEqualToZero()) {
    return output.MakeError("Division by zero.", theCommands, true);
  }
  return output.AssignValue(leftR / rightR, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerDivideDoubleByDouble(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDivideRatByRat");
  if (!input.IsListNElements(3)) {
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
    return output.MakeError("Division by zero.", theCommands, true);
  }
  return output.AssignValue(leftD / rightD, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerTensorEltTensorByEltTensor(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerTensorEltTensorByEltTensor");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.MergeContextsMyArumentsAndConvertThem<ElementTensorsGeneralizedVermas<RationalFunctionOld> >(
    inputConverted, &theCommands.Comments
  )) {
    return false;
  }
  ElementTensorsGeneralizedVermas<RationalFunctionOld>
  resultTensor = inputConverted[1].GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
  resultTensor.TensorOnTheRight(inputConverted[2].GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >());
  return output.AssignValueWithContext(resultTensor, inputConverted[1].GetContext(), theCommands);
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
  if (!input.IsListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.MergeContextsMyAruments(inputConverted, &theCommands.Comments)) {
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
  return output.AssignValueWithContext(theWeight, inputConverted[2].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyWeylGroupEltByWeightPoly(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyWeylGroupEltByWeightPoly");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.MergeContextsMyAruments(inputConverted, &theCommands.Comments)) {
    return false;
  }
  Weight<Polynomial<Rational> > theWeight;
  if (!inputConverted[2].IsOfType<Weight<Polynomial<Rational> > >(&theWeight)) {
    return false;
  }
  if (!inputConverted[1].IsOfType<ElementWeylGroup>()) {
    return false;
  }
  ElementWeylGroup theElt = inputConverted[1].GetValue<ElementWeylGroup>();
  if (theElt.owner != &theWeight.owner->theWeyl) {
    return theCommands << "<hr>Possible user input error: attempting to apply Weyl group "
    << "element to weight corresponding to different Weyl group.";
  }
  Vector<Polynomial<Rational> > theWeightSimpleCoords = theElt.owner->GetSimpleCoordinatesFromFundamental(
    theWeight.weightFundamentalCoordS
  );
  theElt.ActOn(theWeightSimpleCoords);
  theWeight.weightFundamentalCoordS = theElt.owner->GetFundamentalCoordinatesFromSimple(theWeightSimpleCoords);
  return output.AssignValueWithContext(theWeight, inputConverted[2].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyEllipticCurveElements(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyEllipticCurveElements");
  if (!input.StartsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  ElementEllipticCurve<Rational> left, right;
  if (!input[1].IsOfType(&left) || !input[2].IsOfType(&right)) {
    return false;
  }
  if (!(left *= right)) {
    return false;
  }
  return output.AssignValueWithContext(left, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyEllipticCurveElementsZmodP(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyEllipticCurveElements");
  if (!input.StartsWith(theCommands.opTimes(), 3)) {
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
  return output.AssignValueWithContext(left, input[1].GetContext(), theCommands);
}

template <>
bool Expression::ConvertToType<ElementUniversalEnveloping<RationalFunctionOld> >(Expression& output) const;

bool CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.MergeContextsMyAruments(inputConverted, &theCommands.Comments)) {
    return false;
  }
  if (!inputConverted[2].IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >()) {
    return false;
  }
  SemisimpleLieAlgebra& theSSalg =
  inputConverted[2].GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >().GetOwnerSS();
  Expression leftE;
  inputConverted[1].CheckConsistency();
  input[1].CheckConsistency();
  input[2].CheckConsistency();
  if (!inputConverted[1].ConvertToType<ElementUniversalEnveloping<RationalFunctionOld> >(leftE)) {
    return false;
  }
  static bool theGhostHasAppeared = false;
  if (!theGhostHasAppeared) {
    theCommands << "Ere I am J.H. ... The ghost in the machine...<br>";
    theGhostHasAppeared = true;
  }
  const ElementTensorsGeneralizedVermas<RationalFunctionOld>& rightEltETGVM =
  inputConverted[2].GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
  ElementTensorsGeneralizedVermas<RationalFunctionOld> outputElt;
  if (rightEltETGVM.IsEqualToZero()) {
    output = inputConverted[2];
    return true;
  }
  theSSalg.OrderNilradicalNilWeightAscending(rightEltETGVM.GetOwnerModule().parabolicSelectionNonSelectedAreElementsLevi);
  theSSalg.flagHasNilradicalOrder = true;
  if (!rightEltETGVM.MultiplyOnTheLeft(
    leftE.GetValue<ElementUniversalEnveloping<RationalFunctionOld> >(), outputElt, theSSalg, 1
  )) {
    return false;
  }
  return output.AssignValueWithContext(outputElt, inputConverted[2].GetContext(), theCommands);
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
    inputContextsMerged, &theCommands.Comments
  )) {
    return false;
  }
  if (
    inputContextsMerged[1].GetValue<ElementWeylAlgebra<Rational> >().HasNonSmallPositiveIntegerDerivation() ||
    inputContextsMerged[2].GetValue<ElementWeylAlgebra<Rational> >().HasNonSmallPositiveIntegerDerivation()
  ) {
    return theCommands << "<hr> Failed to multiply " << inputContextsMerged[1].ToString()
    << " by " << inputContextsMerged[2].ToString() << ": "
    << " one of the two differential operators has differential operator exponent that is not a small integer. ";
  }
  ElementWeylAlgebra<Rational> result = inputContextsMerged[1].GetValue<ElementWeylAlgebra<Rational> >();
  result *= inputContextsMerged[2].GetValue<ElementWeylAlgebra<Rational> >();
  return output.AssignValueWithContext(result, inputContextsMerged[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF");
  if (!CalculatorFunctionsBinaryOps::innerMultiplyTypeByType<RationalFunctionOld>(theCommands, input, output)) {
    return false;
  }
  RationalFunctionOld simplified;
  if (!output.IsOfType(&simplified)) {
    return true;
  }
  simplified.Simplify();
  return output.AssignValueWithContext(simplified, output.GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerDivideRFOrPolyOrRatByRFOrPoly(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDivideRFOrPolyOrRatByRFOrPoly");
  if (!CalculatorFunctionsBinaryOps::innerDivideTypeByType<RationalFunctionOld>(theCommands, input, output)) {
    return false;
  }
  RationalFunctionOld simplified;
  if (!output.IsOfType(&simplified)) {
    return true;
  }
  simplified.Simplify();
  return output.AssignValueWithContext(simplified, output.GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrRFToRatOrPolyOrRF(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrRFToRatOrPolyOrRF");
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<RationalFunctionOld>(theCommands, input, output);
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
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<ElementUniversalEnveloping<RationalFunctionOld> >(
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
  if (!input.MergeContextsMyArumentsAndConvertThem<ElementUniversalEnveloping<RationalFunctionOld> >(
    inputContextsMerged, &theCommands.Comments
  )) {
    return false;
  }
  ElementUniversalEnveloping<RationalFunctionOld> result = inputContextsMerged[1].GetValue<ElementUniversalEnveloping<RationalFunctionOld> >();
  result *= inputContextsMerged[2].GetValue<ElementUniversalEnveloping<RationalFunctionOld> >();
  result.Simplify();
  return output.AssignValueWithContext(result, inputContextsMerged[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyLRObyLRO(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyLRObyLRO");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3)) {
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
  output.GetValue<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >();
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& rightMon =
  rightCopy.GetValue<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >();
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> result;
  result = leftMon;
  result *= rightMon;
  for (int i = 0; i < result.generatorsIndices.size; i ++) {
    if (result.Powers[i] > 100000 || result.Powers[i] < 0) {
      return output.MakeError(
        "The result of this operation is " + result.ToString() + " which is outside of the allowed range. ",
        theCommands
      );
    }
  }
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyLRObyLSPath(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyLRObyLSPath");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3)) {
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
  LittelmannPath result = rightCopy.GetValue<LittelmannPath>();
  WeylGroupData& theWeyl = *result.owner;
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> theLRO =
  output.GetValue<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >();
  for (int i = theLRO.generatorsIndices.size - 1; i >= 0; i --) {
    if (
      theLRO.generatorsIndices[i] == 0 ||
      theLRO.generatorsIndices[i] < - theWeyl.GetDim() ||
      theLRO.generatorsIndices[i] > theWeyl.GetDim()
    ) {
      std::stringstream out;
      out << " The Littelmann root operator must have an index whose absolute value "
      << "is between 1 and the rank of the ambient Lie algebra, instead I get index  "
      << theLRO.generatorsIndices[i];
      return output.MakeError(out.str(), theCommands);
    } else {
      for (int j = 0; j < theLRO.Powers[i]; j ++) {
        result.ActByEFDisplayIndex(theLRO.generatorsIndices[i]);
      }
    }
  }
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddEltTensorToEltTensor(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddEltTensorToEltTensor");
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >(
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
  if (!input.IsListNElements(3)) {
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
  return output.AssignValue(leftPlot, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerPolyBySmallInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerPolyBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3)) {
    return false;
  }
  Polynomial<Rational> base;
  int thePower = 0;
  if (!input[1].IsOfType(&base) || !input[2].IsSmallInteger(&thePower)) {
    return false;
  }
  if (base.IsEqualToZero() && thePower <= 0) {
    return output.MakeError("Division by zero: trying to raise 0 to negative power. ", theCommands, true);
  }
  if (thePower < 0) {
    if (base.size() == 1) {
      Polynomial<Rational> outputPoly;
      MonomialP theMon = base[0];
      Rational theCF = base.theCoeffs[0];
      theCF.RaiseToPower(thePower);
      theMon.RaiseToPower(thePower);
      outputPoly.MakeZero();
      outputPoly.AddMonomial(theMon, theCF);
      return output.AssignValueWithContext(outputPoly, input[1].GetContext(), theCommands);
    }
    base.RaiseToPower(- thePower);
    RationalFunctionOld theRF;
    theRF = base;
    theRF.Invert();
    return output.AssignValueWithContext(theRF, input[1].GetContext(), theCommands);
  }
  base.RaiseToPower(thePower);
  return output.AssignValueWithContext(base, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerMatNumbersByLargeIntegerIfPossible(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerMatNumbersByLargeIntegerIfPossible");
  if (!input.StartsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  LargeInteger largePower;
  if (!input[2].IsInteger(&largePower)) {
    return false;
  }
  Matrix<Rational> baseRat;
  if (input[1].IsMatrixGivenType(nullptr, nullptr, &baseRat)) {
    if (!baseRat.IsSquare() || baseRat.NumCols == 0) {
      std::stringstream errorStream;
      errorStream << "Exponentiating non-square matrices or matrices with zero rows is not allowed. "
      << "Your matrix, " << baseRat.ToString() << " is not square. ";
      return output.MakeError(errorStream.str(), theCommands, true);
    }
    Rational theDet = baseRat.GetDeterminant();
    if (largePower <= 0) {
      if (theDet == 0) {
        return output.MakeError("Division by zero: trying to raise 0 to negative power. ", theCommands, true);
      }
    }
    if (theDet != 0 && theDet != - 1 && theDet != 1) {
      return theCommands << "Matrix power too large.";
    }
    if (largePower < 0) {
      baseRat.Invert();
      largePower *= - 1;
    }
    Matrix<Rational> idMat;
    idMat.MakeIdMatrix(baseRat.NumRows);
    MathRoutines::RaiseToPower(baseRat, largePower, idMat);
    return output.AssignMatrix(baseRat, theCommands);
  }
  Matrix<AlgebraicNumber> baseAlg;
  if (input[1].IsMatrixGivenType(nullptr, nullptr, &baseAlg)) {
    if (!baseAlg.IsSquare() || baseAlg.NumCols == 0) {
      std::stringstream errorStream;
      errorStream << "Exponentiating non-square matrices or matrices with zero rows is not allowed. "
      << "Your matrix, " << baseAlg.ToString() << " is not square. ";
      return output.MakeError(errorStream.str(), theCommands);
    }
    AlgebraicNumber theDet = baseAlg.GetDeterminant();
    if (largePower <= 0) {
      if (theDet == 0) {
        return output.MakeError("Division by zero: trying to raise 0 to negative power. ", theCommands, true);
      }
    }
    if (theDet != 0 && theDet != - 1 && theDet != 1) {
      return theCommands << "Matrix power too large.";
    }
    if (largePower < 0) {
      baseAlg.Invert();
      largePower *= - 1;
    }
    Matrix<AlgebraicNumber> idMat;
    idMat.MakeIdMatrix(baseAlg.NumRows);
    MathRoutines::RaiseToPower(baseAlg, largePower, idMat);
    return output.AssignMatrix(baseAlg, theCommands);
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerPowerMatNumbersBySmallInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerMatNumbersBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.StartsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  int thePower = 0;
  bool powerIsSmall = input[2].IsSmallInteger(&thePower);
  if (!powerIsSmall) {
    return false;
  }
  Matrix<Rational> baseRat;
  if (input[1].IsMatrixGivenType<Rational>(nullptr, nullptr, &baseRat)) {
    if (!baseRat.IsSquare() || baseRat.NumCols == 0) {
      std::stringstream errorStream;
      errorStream << "Exponentiating non-square matrices or matrices with zero rows is not allowed. "
      << "Your matrix, " << baseRat.ToString() << " is not square. ";
      return output.MakeError(errorStream.str(), theCommands);
    }
    if (thePower <= 0) {
      if (baseRat.GetDeterminant() == 0) {
        return output.MakeError("Division by zero: trying to raise 0 to negative power. ", theCommands, true);
      }
    }
    if (thePower < 0) {
      baseRat.Invert();
      thePower *= - 1;
    }
    Matrix<Rational> idMat;
    idMat.MakeIdMatrix(baseRat.NumRows);
    MathRoutines::RaiseToPower(baseRat, thePower, idMat);
    return output.AssignMatrix(baseRat, theCommands);
  }
  Matrix<AlgebraicNumber> baseAlg;
  if (input[1].IsMatrixGivenType<AlgebraicNumber>(nullptr, nullptr, &baseAlg)) {
    if (!baseAlg.IsSquare() || baseAlg.NumCols == 0) {
      return output.MakeError("Exponentiating non-square matrices or matrices with zero rows is not allowed.", theCommands);
    }
    if (thePower <= 0) {
      if (baseAlg.GetDeterminant() == 0) {
        return output.MakeError("Division by zero: trying to raise 0 to negative power. ", theCommands, true);
      }
    }
    if (thePower < 0) {
      baseAlg.Invert();
      thePower *= - 1;
    }
    Matrix<AlgebraicNumber> idMat;
    idMat.MakeIdMatrix(baseAlg.NumRows);
    MathRoutines::RaiseToPower(baseAlg, thePower, idMat);
    return output.AssignMatrix(baseAlg, theCommands);
  }
  if (input[1].IsMatrixGivenType<RationalFunctionOld>()) {
    Matrix<RationalFunctionOld> baseRF;
    Expression theContext;
    if (!theCommands.GetMatrix(input[1], baseRF, &theContext)) {
      return false;
    }
    if (!baseRF.IsSquare() || baseRF.NumCols == 0) {
      return output.MakeError(
        "Exponentiating non-square matrices or matrices with zero rows is not allowed.", theCommands, true
      );
    }
    if (thePower <= 0) {
      if (baseRF.GetDeterminant() == 0) {
        return output.MakeError("Division by zero: trying to raise 0 to negative power. ", theCommands, true);
      }
    }
    if (thePower < 0) {
      return theCommands << "Raising matrices of rational functions to negative powers not implemented yet. ";
    }
    Matrix<RationalFunctionOld> idMat;
    idMat.MakeIdMatrix(baseRF.NumRows);
    MathRoutines::RaiseToPower(baseRF, thePower, idMat);
    return output.AssignMatrix(baseRF, theCommands, &theContext);
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerPowerAlgNumPolyBySmallInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerAlgNumPolyBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3)) {
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
  if (base.IsEqualToZero() && thePower <= 0) {
    return output.MakeError("Division by zero: trying to raise 0 to negative power. ", theCommands, true);
  }
  base.RaiseToPower(thePower);
  return output.AssignValueWithContext(base, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberBySmallInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3)) {
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
        if (base.AssignRationalQuadraticRadical(baseRat, theCommands.theObjectContainer.theAlgebraicClosure)) {
          base.CheckConsistency();
          output = input;
          Expression newPower, newBase;
          newPower.AssignValue(powerRat * 2, theCommands);
          newBase.AssignValue(base, theCommands);
          return output.MakeXOX(theCommands, theCommands.opThePower(), newBase, newPower);
        }
      }
    }
  }
  int thePower = 0;
  if (!input[2].IsSmallInteger(&thePower)) {
    return false;
  }
  if (base.IsEqualToZero() && thePower < 0) {
    return output.MakeError("Division by zero: trying to raise 0 to negative power. ", theCommands, true);
  }
  if (base.IsEqualToZero() && thePower == 0) {
    return output.AssignValue(1, theCommands);
  }
  if (thePower < 0) {
    thePower *= - 1;
    base.Invert();
  }
  MathRoutines::RaiseToPower(base, thePower, AlgebraicNumber(1));
  return output.AssignValueWithContext(base, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerEWABySmallInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerPolyBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3)) {
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
    } else if (base.theCoeffs[0] != 1) {
      isMon = false;
    }
    if (!isMon) {
      return theCommands << "<hr>Failed to raise " << base.ToString() << " to power "
      << powerRat.ToString() << ": the exponent is not a "
      << " small integer and the base is not a coefficient one monomial. ";
    }
    ElementWeylAlgebra<Rational> finalOutput;
    MonomialWeylAlgebra theMon = base[0];
    theMon.polynomialPart.RaiseToPower(powerRat);
    theMon.differentialPart.RaiseToPower(powerRat);
    for (int i = 0; i < theMon.polynomialPart.GetMinNumVars(); i ++) {
      if (theMon.polynomialPart[i] != 0 && theMon.differentialPart[i] != 0) {
        return theCommands << "<hr>Failed to raise " << base.ToString() << " to power "
        << powerRat.ToString() << ": the exponent is not a "
        << " small integer, the base is a monomial, however the monomial "
        << "contains derivative and polynomial with respect to the same variable. ";
      }
    }
    finalOutput.MakeZero();
    finalOutput.AddMonomial(theMon, 1);
    return output.AssignValueWithContext(finalOutput, input[1].GetContext(), theCommands);
  }

  if (base.IsEqualToZero()) {
    if (thePower < 0) {
      return output.MakeError("Division by zero: trying to raise 0 to negative power. ", theCommands, true);
    }
    if (thePower == 0) {
      return output.AssignValue(1, theCommands);
    }
  }
  base.RaiseToPower(thePower);
  return output.AssignValueWithContext(base, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerRationalByInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerRatByRat");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3)) {
    return false;
  }
  Rational base, exp;
  if (!input[1].IsRational(&base)) {
    return false;
  }
  if (base.IsEqualToOne()) {
    return output.AssignValue(1, theCommands);
  }
  if (!input[2].IsRational(&exp)) {
    return false;
  }
  int thePower;
  if (!exp.IsSmallInteger(&thePower)) {
    return false;
  }
  if (base == 0 && thePower == 0) {
    return output.AssignValue(1, theCommands);
  }
  if (base == 0 && thePower < 0) {
    return output.MakeError("Division by zero: trying to raise 0 to negative or zero power. ", theCommands, true);
  }
  base.RaiseToPower(thePower);
  return output.AssignValue(base, theCommands);
}

template <>
bool Expression::ConvertToType<RationalFunctionOld>(Expression& output) const;

bool CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.MergeContextsMyAruments(inputConverted, &theCommands.Comments)) {
    return false;
  }
  ElementUniversalEnveloping<RationalFunctionOld> theUE;
  Expression copyExponent = inputConverted[2];
  Expression copyBase = inputConverted[1];
  if (!copyBase.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >(&theUE)) {
    return false;
  }
  if (!theUE.IsAPowerOfASingleGenerator()) {
    int tempPower;
    if (!copyExponent.IsSmallInteger(&tempPower)) {
      return false;
    }
    theUE.RaiseToPower(tempPower);
    return output.AssignValueWithContext(theUE, copyBase.GetContext(), theCommands);
  }
  Expression exponentConverted;
  if (!copyExponent.ConvertToType<RationalFunctionOld>(exponentConverted)) {
    return false;
  }
  MonomialUniversalEnveloping<RationalFunctionOld> theMon;
  theMon = theUE[0];
  theMon.Powers[0] *= exponentConverted.GetValue<RationalFunctionOld>();
  ElementUniversalEnveloping<RationalFunctionOld> outputUE;
  outputUE.MakeZero(*theUE.owner);
  outputUE.AddMonomial(theMon, 1);
  return output.AssignValueWithContext(outputUE, copyBase.GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerSequenceOrMatrixByT(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerSequenceOrMatrixByT");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.StartsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  if (!input[1].IsSequenceNElementS() && !input[1].IsMatrix()) {
    return false;
  }
  if (!input[2].IsAtomGivenData("t")  && !input[2].IsAtomGivenData("T")) {
    return false;
  }
  return theCommands.innerTranspose(theCommands, input[1], output);
}

bool CalculatorFunctionsBinaryOps::innerPowerMatExpressionsBySmallInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerMatExpressionsBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.StartsWith(theCommands.opThePower(), 3)) {
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
    return output.MakeError("Attempting to raise non-square matrix to power", theCommands);
  }
  LargeInteger expectedNumTerms;
  expectedNumTerms = theMat.NumCols;
  expectedNumTerms.RaiseToPower(thePower);
  if (expectedNumTerms > 10000) {
    return theCommands << "The expected number terms in the result of the exponentiation "
    << theMat.ToString() << " to the power of " << thePower << " is approximately ("
    << theMat.NumCols << ")^" << thePower << "=" << expectedNumTerms
    << ". I have been instructed to proceed only if the expected number of terms is fewer than 10000. ";
  }
  Matrix<Expression> idMatE;
  idMatE.MakeIdMatrix(theMat.NumRows, theCommands.EOne(), theCommands.EZero());
  MathRoutines::RaiseToPower(theMat, thePower, idMatE);
  return output.AssignMatrixExpressions(theMat, theCommands, true, true);
}

bool CalculatorFunctionsBinaryOps::innerPowerRationalByRationalReducePrimeFactors(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerRatByRatReducePrimeFactors");
  if (!input.StartsWith(theCommands.opThePower(), 3)) {
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
      return output.MakeError("Division by zero while evaluating " + input.ToString(), theCommands, true);
    }
    return output.AssignValue(0, theCommands);
  }
  if (!base.IsInteger()) {
    if (base.GetNumerator() == 1) {
      Expression theDenBase, theDenominator;
      theDenBase.AssignValue(Rational(base.GetDenominator()), theCommands);
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
      currentOutside.RaiseToPower(currentOutsidePowerInt);
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
    currentContribution.RaiseToPower(currentExpSmallInt);
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
    currentContribution.RaiseToPower(currentExpSmallInt);
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
  theRadicalCFE.AssignValue(outsideOfTheRadical, theCommands);
  insideTheRadicalE.AssignValue(insideTheRadical, theCommands);
  exponentE.AssignValue(exponentWorking, theCommands);
  theRadicalE.MakeXOX(theCommands, theCommands.opThePower(), insideTheRadicalE, exponentE);
  return output.MakeProducT(theCommands, theRadicalCFE, theRadicalE);
}

bool CalculatorFunctionsBinaryOps::innerPowerDoubleOrRatToDoubleOrRat(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerRatPowerRat");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3)) {
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
      return output.AssignValue(FloatingPoint::Power(- baseDouble, thePower), theCommands);
    }
    baseDouble *= - 1;
    return output.AssignValue(-FloatingPoint::Power(baseDouble, expDouble), theCommands);
  }
  if (baseDouble == 0.0) {
    if (expDouble > 0) {
      return output.AssignValue<double>(0, theCommands);
    }
  }
  return output.AssignValue(FloatingPoint::Power(baseDouble, expDouble), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyDoubleOrRatByDoubleOrRat(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerRatPowerRat");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3)) {
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
  return output.AssignValue(leftD * rightD, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddDoubleOrRatToDoubleOrRat(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerRatPowerRat");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3)) {
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
  return output.AssignValue(leftD + rightD, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyCharSSLieAlgByCharSSLieAlg(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyCharSSLieAlgByCharSSLieAlg");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3)) {
    return false;
  }
  charSSAlgMod<Rational> leftC, rightC;
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
    return theCommands << "I tried to multiply character " << leftC.ToString() << " by " << rightC.ToString()
    << " but I failed with the following message: " << successString;
  }
  return output.AssignValue(leftC, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarByMatrix(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarByMatrix");
  if (!input.StartsWith(theCommands.opTimes(), 3)) {
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
  for (int i = 0; i < theMat.NumRows; i ++) {
    for (int j = 0; j < theMat.NumCols; j ++) {
      theMat(i, j) = theScalarE * theMat(i, j);
    }
  }
  return output.AssignMatrixExpressions(theMat, theCommands, false, true);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence");
  if (!input.StartsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  if (!input[1].IsBuiltInScalar()) {
    return false;
  }
  if (!input[2].IsSequenceNElementS()) {
    return false;
  }
  output.reset(theCommands);
  output.children.Reserve(input[2].size());
  output.AddChildAtomOnTop(theCommands.opSequence());
  Expression tempProduct;
  for (int i = 1; i < input[2].size(); i ++) {
    tempProduct.MakeProducT(theCommands, input[1], input[2][i]);
    output.AddChildOnTop(tempProduct);
  }
  return true;
}

bool CalculatorFunctionsBinaryOps::innerMultiplyMatrixByMatrix(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyMatrixByMatrix");
  if (!input.StartsWith(theCommands.opTimes())) {
    return false;
  }
  int numColsFirst = - 1;
  int numRowsSecond = - 1;
  if (!input[1].IsMatrix(nullptr, &numColsFirst) || !input[2].IsMatrix(&numRowsSecond, nullptr)) {
    return false;
  }
  if (numColsFirst != numRowsSecond) {
    return theCommands << "WARNING: I encountered a product of a matrix with " << numColsFirst
    << " columns by a matrix with " << numRowsSecond << " rows. ";
  }
  if (input[1].IsMatrixGivenType<Rational>() && input[2].IsMatrixGivenType<Rational>()) {
    return CalculatorFunctionsBinaryOps::innerMultiplyMatrixRationalOrRationalByMatrixRational(theCommands, input, output);
  }
  bool invokeAlgMatMultiplication =
  (input[1].IsMatrixGivenType<Rational>() && input[2].IsMatrixGivenType<AlgebraicNumber>()) ||
  (input[1].IsMatrixGivenType<AlgebraicNumber>() && input[2].IsMatrixGivenType<Rational>()) ||
  (input[1].IsMatrixGivenType<AlgebraicNumber>() && input[2].IsMatrixGivenType<AlgebraicNumber>());
  if (invokeAlgMatMultiplication) {
    return CalculatorFunctionsBinaryOps::innerMultiplyMatRatOrMatAlgByMatRatOrMatAlg(theCommands, input, output);
  }
  bool invokeRFMultiplication =
  (input[1].IsMatrixGivenType<RationalFunctionOld>() && input[2].IsMatrixGivenType<Rational>()) ||
  (input[1].IsMatrixGivenType<Rational>() && input[2].IsMatrixGivenType<RationalFunctionOld>()) ||
  (input[1].IsMatrixGivenType<RationalFunctionOld>() && input[2].IsMatrixGivenType<RationalFunctionOld>() );
  if (invokeRFMultiplication) {
    return CalculatorFunctionsBinaryOps::innerMultiplyMatrixRFOrRFByMatrixRF(theCommands, input, output);
  }
  Matrix<Expression> leftMat, rightMat;
  if (!theCommands.GetMatrixExpressions(input[1], leftMat)) {
    return false;
  }
  if (!theCommands.GetMatrixExpressions(input[2], rightMat, leftMat.NumCols)) {
    return false;
  }
  if (leftMat.NumCols != rightMat.NumRows) {
    return false;
  }
  Matrix<Expression> outputMat;
  outputMat.init(leftMat.NumRows, rightMat.NumCols);
  Expression leftSummand, rightSummand;
  for (int i = 0; i < leftMat.NumRows; i ++) {
    for (int j = 0; j < rightMat.NumCols; j ++) {
      for (int k = 0; k < leftMat.NumCols; k ++) {
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
  if (!input.StartsWith(theCommands.opTimes())) {
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
  theResult.SetSize(matCols);
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
  if (!input.StartsWith(theCommands.opTensor())) {
    return false;
  }
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerTensorMatrixByMatrix");
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if ((!rightE.IsMatrix() && !rightE.IsSequenceNElementS()) || (!leftE.IsMatrix() && !leftE.IsSequenceNElementS())) {
    return false;
  }
  Matrix<Rational> leftMatRat, rightMatRat;
  if (leftE.IsMatrixGivenType(nullptr, nullptr, &leftMatRat) && rightE.IsMatrixGivenType(nullptr, nullptr, &rightMatRat)) {
    Matrix<Rational> resultMatRat;
    resultMatRat.AssignTensorProduct(leftMatRat, rightMatRat);
    return output.AssignMatrix(resultMatRat, theCommands, nullptr, true);
  }
  Matrix<Expression> leftMatE, rightMatE, resultMatE;
  if (!theCommands.GetMatrixExpressions(input[1], leftMatE)) {
    return false;
  }
  if (!theCommands.GetMatrixExpressions(input[2], rightMatE)) {
    return false;
  }
  resultMatE.AssignTensorProduct(leftMatE, rightMatE);
  return output.AssignMatrixExpressions(resultMatE, theCommands, true, true);
}

bool CalculatorFunctionsBinaryOps::innerTensorMatByMatTensor(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  if (!input.StartsWith(theCommands.opTensor())) {
    return false;
  }
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerTensorMatByMatTensor");
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!rightE.IsOfType<MatrixTensor<Rational> >() || !leftE.IsOfType<MatrixTensor<Rational> >()) {
    return false;
  }
  MatrixTensor<Rational> result;
  result.AssignTensorProduct(leftE.GetValue<MatrixTensor<Rational> >(), rightE.GetValue<MatrixTensor<Rational> >());
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRatOrAlgebraicByMatRatOrMatAlg(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRatOrAlgebraicByMatRatOrMatAlg");
  if (!input.IsListNElements(3)) {
    return false;
  }
  const Expression* matE;
  const Expression* scalarE;
  matE = &input[1];
  scalarE = &input[2];
  if (!matE->IsMatrixGivenType<Rational>() && !matE->IsMatrixGivenType<AlgebraicNumber>()) {
    MathRoutines::swap(matE, scalarE);
  }
  if (!matE->IsMatrixGivenType<Rational>() && !matE->IsMatrixGivenType<AlgebraicNumber>()) {
    return false;
  }
  Matrix<AlgebraicNumber> theMatAlg;
  AlgebraicNumber theScalar;
  Matrix<Rational> theMatRat;
  Rational theScalarRat;
  if (matE->IsMatrixGivenType(nullptr, nullptr, &theMatRat)) {
    theMatAlg = theMatRat;
  } else {
    if (!matE->IsMatrixGivenType(nullptr, nullptr, &theMatAlg)) {
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
  if (!input.IsListNElements(3)) {
    return false;
  }
  Matrix<AlgebraicNumber> matAlgLeft, matAlgRight;
  Matrix<Rational> matRatLeft, matRatRight;
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (leftE.IsMatrixGivenType<Rational>() && rightE.IsMatrixGivenType<Rational>()) {
    return false;
  }
  if (leftE.IsMatrixGivenType(nullptr, nullptr, &matRatLeft)) {
    matAlgLeft = matRatLeft;
  } else {
    if (!leftE.IsMatrixGivenType(nullptr, nullptr, &matAlgLeft)) {
      return false;
    }
  }
  if (rightE.IsMatrixGivenType(nullptr, nullptr, &matRatRight)) {
    matAlgRight = matRatRight;
  } else {
    if (!rightE.IsMatrixGivenType(nullptr, nullptr, &matAlgRight)) {
      return false;
    }
  }
  if (matAlgLeft.NumCols != matAlgRight.NumRows) {
    std::stringstream errorStream;
    errorStream << "Error: attempting to multiply matrix with "
    << matAlgLeft.NumCols << " columns by a "
    << "matrix with " << matAlgRight.NumRows << " rows. ";
    return output.MakeError(errorStream.str(), theCommands, true);
  }
  matAlgLeft *= matAlgRight;
  return output.AssignMatrix(matAlgLeft, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyMatrixRationalOrRationalByMatrixRational(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyMatrixRationalOrRationalByMatrixRational");
  if (!input.IsListNElements(3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!rightE.IsMatrixGivenType<Rational>()) {
    return false;
  }
  Rational theScalar;
  if (leftE.IsOfType<Rational>(&theScalar)) {
    Matrix<Rational> result;
    rightE.IsMatrixGivenType(nullptr, nullptr, &result);
    result *= theScalar;
    return output.AssignMatrix(result, theCommands);
  }
  Matrix<Rational> leftMat, rightMat;
  if (!leftE.IsMatrixGivenType(nullptr, nullptr, &leftMat)) {
    return false;
  }
  if (!rightE.IsMatrixGivenType(nullptr, nullptr, &rightMat)) {
    return false;
  }
  if (leftMat.NumCols != rightMat.NumRows) {
    return false;
  }
  leftMat.MultiplyOnTheRight(rightMat);
  return output.AssignMatrix(leftMat, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyMatrixRFOrRFByMatrixRF(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyMatrixRFOrRFByMatrixRF");
  if (!input.IsListNElements(3)) {
    return false;
  }
  Expression leftE = input[1];
  Expression rightE = input[2];
  if (
    !rightE.IsMatrixGivenType<RationalFunctionOld>() || !(
      leftE.IsOfType<Rational>() ||
      leftE.IsOfType<Polynomial<Rational> >() ||
      leftE.IsOfType<RationalFunctionOld>() ||
      leftE.IsMatrixGivenType<RationalFunctionOld>()
    )
  ) {
    return false;
  }
  if (!leftE.MergeContexts(leftE, rightE)) {
    return theCommands << "Failed to convert " << leftE.ToString() << " and " << rightE.ToString()
    << " to common context";
  }
  if (!leftE.IsMatrixGivenType<RationalFunctionOld>()) {
    Expression leftErfForm;
    if (!leftE.ConvertToType<RationalFunctionOld>(leftErfForm)) {
      return theCommands << "Failed to convert " << leftE.ToString() << " to rational function. ";
    }
    Matrix<RationalFunctionOld> result;
    rightE.IsMatrixGivenType(nullptr, nullptr, &result);
    RationalFunctionOld theScalar = leftErfForm.GetValue<RationalFunctionOld>();
    result *= theScalar;
    Expression contextE = leftE.GetContext();
    return output.AssignMatrix(result, theCommands, &contextE);
  }
  Matrix<RationalFunctionOld> rightMat;
  rightE.IsMatrixGivenType(nullptr, nullptr, &rightMat);
  Matrix<RationalFunctionOld> result;
  leftE.IsMatrixGivenType(nullptr, nullptr, &result);
  if (result.NumCols != rightMat.NumRows) {
    return false;
  }
  result.MultiplyOnTheRight(rightMat);
  Expression contextE = leftE.GetContext();
  return output.AssignMatrix(result, theCommands, &contextE);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyMatrixTensorOrRationalByMatrixTensor(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyMatrixTensorOrRationalByMatrixTensor");
  if (!input.IsListNElements(3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!rightE.IsOfType<MatrixTensor<Rational> >()) {
    return false;
  }
  Rational theScalar;
  if (leftE.IsOfType<Rational>(&theScalar)) {
    MatrixTensor<Rational> result = rightE.GetValue<MatrixTensor<Rational> >();
    result *= theScalar;
    return output.AssignValue(result, theCommands);
  }
  if (!leftE.IsOfType<MatrixTensor<Rational> >()) {
    return false;
  }
  const MatrixTensor<Rational>& rightMat = rightE.GetValue<MatrixTensor<Rational> >();
  MatrixTensor<Rational> result = leftE.GetValue<MatrixTensor<Rational> >();
  result *= rightMat;
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerLieBracketExtractConstant(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerLieBracketExtractConstant");
  if (!input.StartsWith(theCommands.opLieBracket(), 3)) {
    return false;
  }
  Rational theCF = 1;
  Rational tempRat;
  Expression leftE = input[1], rightE = input[2];
  bool found = false;
  if (input[1].StartsWith(theCommands.opTimes(), 3)) {
    if (input[1][1].IsOfType(&tempRat)) {
      found = true;
      theCF *= tempRat;
      leftE = input[1][2];
    }
  }
  if (input[2].StartsWith(theCommands.opTimes(), 3)) {
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
  output.AssignValue(theCF, theCommands);
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
  if (!input.IsListNElements(3)) {
    return false;
  }
  Expression inputConverted;
  if (!input.MergeContextsMyAruments(inputConverted, nullptr)) {
    return false;
  }
  const Expression& leftE = inputConverted[1];
  const Expression& rightE = inputConverted[2];
  if (leftE.IsOfType<Rational>() || rightE.IsOfType<Rational>()) {
    return output.AssignValueWithContext(0, leftE.GetContext() , theCommands);
  }
  if (
    leftE.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >() &&
    rightE.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >()
  ) {
    ElementUniversalEnveloping<RationalFunctionOld> result;
    leftE.GetValue<ElementUniversalEnveloping<RationalFunctionOld> >().LieBracketOnTheRight(
      rightE.GetValue<ElementUniversalEnveloping<RationalFunctionOld> >(), result
    );
    result.Simplify();
    return output.AssignValueWithContext(result, leftE.GetContext(), theCommands);
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerLieBracketJacobiIdentityIfNeeded(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerLieBracketJacobiIdentityIfNeeded");
  if (!input.StartsWith(theCommands.opLieBracket(), 3)) {
    return false;
  }
  if (!input[2].StartsWith(theCommands.opLieBracket())) {
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
  if (!input.StartsWith(theCommands.opLieBracket(), 3)) {
    return false;
  }
  if (input[1].IsEqualToZero() || input[2].IsEqualToZero()) {
    return output.AssignValue(0, theCommands);
  }
  if (input[2] > input[1]) {
    return false;
  }
  if (input[1] == input[2]) {
    return output.AssignValue(0, theCommands);
  }
  Expression theBracket;
  theBracket.MakeXOX(theCommands, theCommands.opLieBracket(), input[2], input[1]);
  output = theCommands.EMOne() * theBracket;
  return true;
}

template <>
bool Expression::ConvertToType<ElementWeylAlgebra<Rational> >(Expression& output) const;

bool CalculatorFunctionsBinaryOps::innerLieBracketRatPolyOrEWAWithRatPolyOrEWA(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerLieBracketRatPolyOrEWAWithRatPolyOrEWA");
  if (!input.IsListNElements(3)) {
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
    return output.AssignValue(0, theCommands);
  }
  Expression leftConverted, rightConverted;
  if (
    !leftE.ConvertToType<ElementWeylAlgebra<Rational> >(leftConverted) ||
    !rightE.ConvertToType<ElementWeylAlgebra<Rational> >(rightConverted)
  ) {
    theCommands << "<hr>Failed with conversion to Element weyl algebra - possible programming error?";
    return false;
  }
  ElementWeylAlgebra<Rational> resultE = rightConverted.GetValue<ElementWeylAlgebra<Rational> >();
  resultE.LieBracketOnTheLeft(leftConverted.GetValue<ElementWeylAlgebra<Rational> >());
  return output.AssignValueWithContext(resultE, leftConverted.GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddMatrixToMatrix(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddMatrixToMatrix");
  if (!input.StartsWith(theCommands.opPlus(), 3)) {
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
  if (leftMat.NumCols != rightMat.NumCols || leftMat.NumRows != rightMat.NumRows) {
    return false;
  }
  for (int i = 0; i < leftMat.NumRows; i ++) {
    for (int j = 0; j < leftMat.NumCols; j ++) {
      leftMat(i, j) += rightMat(i, j);
    }
  }
  return output.AssignMatrixExpressions(leftMat, theCommands, false, true);
}

bool CalculatorFunctionsBinaryOps::innerDirectSumMatrixWithMatrix(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDirectSumMatrixWithMatrix");
  if (!input.StartsWith(theCommands.opDirectSum(), 3)) {
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
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddMatrixRationalToMatrixRational");
  if (!input.IsListNElements(3)) {
    return false;
  }
  Expression leftE = input[1];
  Expression rightE = input[2];
  if (!rightE.IsMatrixGivenType<Rational>()|| !leftE.IsMatrixGivenType<Rational>()) {
    if (rightE.IsMatrixGivenType<AlgebraicNumber>()) {
      MathRoutines::swap(leftE, rightE);
    }
    if (!leftE.IsMatrixGivenType<AlgebraicNumber>()) {
      return false;
    }
    if (rightE.IsMatrixGivenType<Rational>()) {
      Matrix<AlgebraicNumber> leftMat;
      leftE.IsMatrixGivenType(nullptr, nullptr, &leftMat);
      Matrix<AlgebraicNumber> rightMat;
      rightE.IsMatrixGivenType(nullptr, nullptr, &rightMat);
      if (rightMat.NumRows != leftMat.NumRows || rightMat.NumCols != leftMat.NumCols) {
        return false;
      }
      Matrix<AlgebraicNumber> result = leftMat;
      result += rightMat;
      return output.AssignMatrix(result, theCommands);
    }
    if (rightE.IsMatrixGivenType<AlgebraicNumber>()) {
      Matrix<AlgebraicNumber> rightMat;
      rightE.IsMatrixGivenType(nullptr, nullptr, &rightMat);
      Matrix<AlgebraicNumber> leftMat;
      leftE.IsMatrixGivenType(nullptr, nullptr, &leftMat);
      if (rightMat.NumRows != leftMat.NumRows || rightMat.NumCols != leftMat.NumCols) {
        return false;
      }
      Matrix<AlgebraicNumber> result = leftMat;
      result += rightMat;
      return output.AssignMatrix(result, theCommands);
    }
    return false;
  }
  Matrix<Rational> rightMat;
  rightE.IsMatrixGivenType(nullptr, nullptr, &rightMat);
  Matrix<Rational> leftMat;
  leftE.IsMatrixGivenType(nullptr, nullptr, &leftMat);
  if (rightMat.NumRows != leftMat.NumRows || rightMat.NumCols != leftMat.NumCols) {
    std::stringstream errorStream;
    errorStream << "Error: attempting to add a "
    << rightMat.NumRows << " by " << rightMat.NumCols << " matrix to a "
    << leftMat.NumRows
    << " by " << leftMat.NumCols << " matrix. ";
    return output.MakeError(errorStream.str(), theCommands, true);
  }
  Matrix<Rational> result = leftMat;
  result += rightMat;
  return output.AssignMatrix(result, theCommands);
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
  resultEs.SetExpectedSize(leftSetE.size() - 1);
  for (int i = 1; i < leftSetE.size(); i ++) {
    resultEs.AddOnTop(leftSetE[i]);
  }
  for (int i = 1; i < rightSetE.children.size; i ++) {
    if (resultEs.Contains(rightSetE[i])) {
      resultEs.RemoveIndexSwapWithLast(resultEs.GetIndex(rightSetE[i]));
    }
  }
  resultEs.QuickSortAscending();
  return output.MakeSequence(theCommands, &resultEs);
}

bool CalculatorFunctionsBinaryOps::innerAddMatrixRFsToMatrixRFs(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddMatrixRFsToMatrixRFs");
  if (!input.IsListNElements(3)) {
    return false;
  }
  Expression leftE = input[1];
  Expression rightE = input[2];
  if (!rightE.IsMatrixGivenType<RationalFunctionOld>() || !leftE.IsMatrixGivenType<RationalFunctionOld>()) {
    return false;
  }
  if (!leftE.MergeContexts(leftE, rightE)) {
    return false;
  }
  Matrix<RationalFunctionOld> rightMat;
  rightE.IsMatrixGivenType(nullptr, nullptr, &rightMat);
  Matrix<RationalFunctionOld> leftMat;
  leftE.IsMatrixGivenType(nullptr, nullptr, &leftMat);
  if (rightMat.NumRows != leftMat.NumRows || rightMat.NumCols != leftMat.NumCols) {
    return false;
  }
  Matrix<RationalFunctionOld> result = leftMat;
  result += rightMat;
  Expression theContext = leftE.GetContext();
  return output.AssignMatrix(result, theCommands, &theContext);
}

bool CalculatorFunctionsBinaryOps::innerAddMatrixTensorToMatrixTensor(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  if (!input.IsListNElements(3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!rightE.IsOfType<MatrixTensor<Rational> >()|| !leftE.IsOfType<MatrixTensor<Rational> >()) {
    return false;
  }
  const MatrixTensor<Rational>& rightMat = rightE.GetValue<MatrixTensor<Rational> >();
  const MatrixTensor<Rational>& leftMat = leftE.GetValue<MatrixTensor<Rational> >();
  MatrixTensor<Rational> result = leftMat;
  result += rightMat;
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplySequenceByAnyScalar(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplySequenceByAnyScalar");
  if (!input.IsListNElements(3)) {
    return false;
  }
  Expression tempE = input;
  tempE.children.SwapTwoIndices(1, 2);
  return CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence(theCommands, tempE, output);
}

bool CalculatorFunctionsBinaryOps::innerAddSequenceToSequence(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddSequenceToSequence");
  if (!input.IsListNElements(3)) {
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
  output.children.Reserve(input[1].size());
  output.AddChildAtomOnTop(theCommands.opSequence());
  Expression tempSum;
  for (int i = 1; i < input[2].size(); i ++) {
    tempSum.MakeXOX(theCommands, theCommands.opPlus(), input[1][i], input[2][i]);
    output.AddChildOnTop(tempSum);
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
    return output.AssignValue(result, theCommands);
  }
  if (K < 11) {
    output.AssignValue(1, theCommands);
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
  if (!input.StartsWith(theCommands.opThePower(), 3)) {
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
    theElt.MakeOne(theElt.theModulus);
    theElt /= copy;
    thePower *= - 1;
  }
  ElementZmodP unit;
  unit.MakeOne(theElt.theModulus);
  MathRoutines::RaiseToPower(theElt, thePower, unit);
  return output.AssignValue(theElt, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerEllipticCurveRationalElementByInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerEllipticCurveRationalElementByInteger");
  if (!input.StartsWith(theCommands.opThePower(), 3)) {
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
    theElt.Invert();
    thePower *= - 1;
  }
  ElementEllipticCurve<Rational> unit;
  unit.MakeOne(theElt.owner);
  MathRoutines::RaiseToPower(theElt, thePower, unit);
  return output.AssignValueWithContext(theElt, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerEllipticCurveZmodPElementByInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerEllipticCurveZmodPElementByInteger");
  if (!input.StartsWith(theCommands.opThePower(), 3)) {
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
    theElt.Invert();
    thePower *= - 1;
  }
  ElementEllipticCurve<ElementZmodP> unit;
  unit.MakeOne(theElt.owner);
  MathRoutines::RaiseToPower(theElt, thePower, unit);
  return output.AssignValueWithContext(theElt, input[1].GetContext(), theCommands);
}
