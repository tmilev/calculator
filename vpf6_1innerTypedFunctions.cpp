//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpfHeader3Calculator1_InnerTypedFunctions.h"
#include "vpfImplementationHeader2Math3_WeylAlgebra.h"
#include "vpfImplementationHeader2Math15_UniversalEnveloping.h"
#include "vpfImplementationHeader2Math1_SemisimpleLieAlgebras.h"
#include "vpfImplementationHeader2Math6_ModulesSSLieAlgebras.h"

ProjectInformationInstance ProjectInfoVpf6_1cpp(__FILE__, "Calculator inner binary typed functions. ");

bool Calculator::innerOperationBinary(Calculator& theCommands, const Expression& input, Expression& output, int theOp)
{ for (int i=0; i<theCommands.FunctionHandlers[theOp].size; i++)
    if (theCommands.FunctionHandlers[theOp][i].inputFitsMyInnerType(input))
      if (theCommands.FunctionHandlers[theOp][i].theFunction(theCommands, input, output))
        return true;
  return false;
}

bool Calculator::outerExtractBaseMultiplication(Calculator& theCommands, const Expression& input, Expression& output)
{ RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("Calculator::outerExtractBaseMultiplication");
 // bool hereBeTrouble = input.ToString()=="e^{-2 x} -2";
//  if (hereBeTrouble)
//    std::cout << "<hr>Here be trouble";
  if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  bool result=false;
  //  std::cout << "<br>handling base extraction of: " << input.ToString();
  //handle Anything*Rational:=Rational*Anything
  output=input;
//  if (hereBeTrouble)
//    std::cout << "handling: " << input.ToString() << ", semilisp: " << input.ToStringSemiFull() << ", lisp: " << input.ToStringFull();
  if (output[2].IsOfType<Rational>())
  { output.children.SwapTwoIndices(1, 2);
    result=true;
//    std::cout << "swapped " << leftE.ToString() << " and " << rightE.ToString();
  }
  if (output[2].IsOfType<double>() && !output[1].IsOfType<Rational>())
  { output.children.SwapTwoIndices(1, 2);
    result=true;
//    std::cout << "swapped " << leftE.ToString() << " and " << rightE.ToString();
  }
//  Expression leftE=output[1];
//  Expression rightE=output[2];
  if (output[2].IsListStartingWithAtom(theCommands.opTimes()))
  { if (output[2].children.size!=3)
      return result;
//    Expression rightLeftE=rightE[1];
//    Expression rightRightE=rightE[2];
    //handle Anything1*(Rational*Anything2):=Rational*(Anything1*Anything2)
    if (output[2][1].IsOfType<Rational>())
    { Expression tempRight;
      tempRight.MakeXOX(theCommands, theCommands.opTimes(), output[1], output[2][2]);
      output.MakeXOX(theCommands, theCommands.opTimes(), output[2][1], tempRight);
      result=true;
//      std::cout << " swapped " << rightLeftE.ToString() << " and " << leftE.ToString();
    }
    //<- handle a*(b*anything)
    //on condition that a*b has an inner handler
    Expression tempExp, newExpr;
    tempExp.MakeXOX(theCommands, theCommands.opTimes(), output[1], output[2][1]);
    if (theCommands.innerTimes(theCommands, tempExp, newExpr))
    { output.MakeProducT(theCommands, newExpr, output[2][2]);
      result=true;
    }
  }
  //handle 0*anything=0
  if (output[1].IsEqualToZero())
    return output.AssignValue(0, theCommands);
  return result;
}

bool CalculatorFunctionsBinaryOps::innerAddEltZmodPorRatToEltZmodPorRat(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddEltZmodPorRatToEltZmodPorRat");
  //std::cout << "<hr>here i am ";
  if (!input.IsListNElements(3))
    return false;
  const Expression* leftE;
  const Expression* rightE;
  leftE=&input[1];
  rightE=&input[2];
  ElementZmodP theElt1, theElt2;
  for (int i=0; i<2; i++, MathRoutines::swap(leftE, rightE))
    if (leftE->IsOfType<ElementZmodP>(&theElt1))
    { if (rightE->IsOfType<ElementZmodP>(&theElt2))
      { if (theElt1.theModulo!=theElt2.theModulo)
          return false;
      } else
      { Rational tempRat;
        if (!rightE->IsOfType<Rational>(&tempRat))
          return false;
        theElt2.theModulo=theElt1.theModulo;
        if (!theElt2.AssignRational(tempRat))
          return false;
      }
      theElt1+=theElt2;
      return output.AssignValue(theElt1, theCommands);
    }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerMultiplyEltZmodPorRatByEltZmodPorRat(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyEltZmodPorRatByEltZmodPorRat");
  if (!input.IsListNElements(3))
    return false;
  const Expression* leftE;
  const Expression* rightE;
  leftE=&input[1];
  rightE=&input[2];
  ElementZmodP theElt1, theElt2;
  for (int i=0; i<2; i++, MathRoutines::swap(leftE, rightE))
    if (leftE->IsOfType<ElementZmodP>(&theElt1))
    { if (rightE->IsOfType<ElementZmodP>(&theElt2))
      { if (theElt1.theModulo!=theElt2.theModulo)
          return false;
      } else
      { Rational tempRat;
        if (!rightE->IsOfType<Rational>(&tempRat))
          return false;
        theElt2.theModulo=theElt1.theModulo;
        if (!theElt2.AssignRational(tempRat))
          return false;
      }
      theElt1*=theElt2;
      return output.AssignValue(theElt1, theCommands);
    }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerAddRatToRat(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddRatToRat");
  if (!input.IsListNElements(3))
    return false;
  Rational leftR, rightR;
  if (!input[1].IsOfType(&leftR) || !input[2].IsOfType(&rightR))
    return false;
  return output.AssignValue(leftR+rightR, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat");
  if (!input.IsListNElements(3))
    return false;
  AlgebraicNumber leftAN, rightAN;
  Rational tempRat;
  if (!input[1].IsOfType(&leftAN))
  { if (!input[2].IsOfType(&rightAN))
      return false;
    if (!input[1].IsOfType<Rational>(&tempRat))
      return false;
    leftAN.AssignRational(tempRat, theCommands.theObjectContainer.theAlgebraicClosure);
  } else if (!input[2].IsOfType(&rightAN))
  { if (!input[2].IsOfType(&tempRat))
      return false;
    rightAN.AssignRational(tempRat, theCommands.theObjectContainer.theAlgebraicClosure);
  }
  if (rightAN.IsEqualToZero())
    return output.SetError("Division by zero. ", theCommands);
  leftAN/=rightAN;
  return output.AssignValue(leftAN, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyAlgebraicNumberByAlgebraicNumber(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyAlgebraicNumberByAlgebraicNumber");
  if (!input.IsListNElements(3))
    return false;
  AlgebraicNumber leftAN, rightAN;
  Rational tempRat;
  if (!input[1].IsOfType(&leftAN))
  { if (!input[2].IsOfType(&rightAN))
      return false;
    if (!input[1].IsOfType<Rational>(&tempRat))
      return false;
    leftAN.AssignRational(tempRat, theCommands.theObjectContainer.theAlgebraicClosure);
  } else if (!input[2].IsOfType(&rightAN))
  { if (!input[2].IsOfType(&tempRat))
      return false;
    rightAN.AssignRational(tempRat, theCommands.theObjectContainer.theAlgebraicClosure);
  }
  leftAN*=rightAN;
  return output.AssignValue(leftAN, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddAlgebraicNumberToAlgebraicNumber");
  if (!input.IsListNElements(3))
    return false;
  AlgebraicNumber leftAN, rightAN;
  Rational tempRat;
  if (!input[1].IsOfType(&leftAN))
  { if (!input[2].IsOfType(&rightAN))
      return false;
    if (!input[1].IsOfType<Rational>(&tempRat))
      return false;
    leftAN.AssignRational(tempRat, theCommands.theObjectContainer.theAlgebraicClosure);
  } else if (!input[2].IsOfType(&rightAN))
  { if (!input[2].IsOfType(&tempRat))
      return false;
    rightAN.AssignRational(tempRat, theCommands.theObjectContainer.theAlgebraicClosure);
  }
  leftAN+=rightAN;
  return output.AssignValue(leftAN, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRatByRat(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRatByRat");
  if (!input.IsListNElements(3))
    return false;
  Rational leftR, rightR;
  if (!input[1].IsOfType(&leftR) || !input[2].IsOfType(&rightR))
    return false;
  return output.AssignValue(leftR*rightR, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyCoxeterEltByCoxeterElt(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyCoxeterEltByCoxeterElt");
  if (!input.IsListNElements(3))
    return false;
  //std::cout << "<br>Here i am!";
  //std::cout << "Multiplying: " << input[1].ToString() << " by " << input[2].ToString();
  ElementWeylGroup<WeylGroup> leftR, rightR;
  if (!input[1].IsOfType(&leftR) || !input[2].IsOfType(&rightR))
    return false;
  //std::cout << "<br>leftR= " << leftR.ToString();
  //std::cout << "<br>rightR=" << rightR.ToString();
  //for (int i=0; i<theCommands.theObjectContainer.theCoxeterElements.size; i++)
  //{ std::cout << "<br>Coxeter element " << i+1 << ": "
  //  << theCommands.theObjectContainer.theCoxeterElements[i].ToString();
  //}
  if (leftR.owner!=rightR.owner)
  { theCommands.Comments << "<hr>Attempting to multiply elements of different groups, possible user typo?";
    return false;
  }
  leftR*=rightR;
  //std::cout << "<br>final output: " << leftR.ToString();
  return output.AssignValue(leftR, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerDivideRatByRat(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDivideRatByRat");
  if (!input.IsListNElements(3))
    return false;
  Rational leftR, rightR;
  if (!input[1].IsOfType(&leftR) || !input[2].IsOfType(&rightR))
    return false;
  if (rightR.IsEqualToZero())
    return output.SetError("Division by zero.", theCommands);
  return output.AssignValue(leftR/rightR, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerDivideDoubleByDouble(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDivideRatByRat");
  if (!input.IsListNElements(3))
    return false;
  double leftD, rightD;
  Rational leftR, rightR;
  if (!input[1].IsOfType(&leftD))
  { if (!input[1].IsOfType(&leftR))
      return false;
    else
      leftD=leftR.DoubleValue();
  }
  if (!input[2].IsOfType(&rightD))
  { if (!input[2].IsOfType(&rightR))
      return false;
    else
      rightD=rightR.DoubleValue();
  }
  if (rightD==0)
    return output.SetError("Division by zero.", theCommands);
  return output.AssignValue(leftD/rightD, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerTensorEltTensorByEltTensor(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerTensorEltTensorByEltTensor");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
//  std::cout << "<br>Attempting to tensor " << input[1].ToString() << " and " << input[2].ToString();
  Expression inputConverted;
  if (!input.MergeContextsMyArumentsAndConvertThem<ElementTensorsGeneralizedVermas<RationalFunctionOld> >(inputConverted))
    return false;
  ElementTensorsGeneralizedVermas<RationalFunctionOld>
  resultTensor=inputConverted[1].GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
  resultTensor.TensorOnTheRight(inputConverted[2].GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >(), *theCommands.theGlobalVariableS);
  return output.AssignValueWithContext(resultTensor, inputConverted[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddWeightToWeight(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddWeightToWeight");
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<Weight<Polynomial<Rational> > >(theCommands, input, output);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyByWeightPoly(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyByWeightPoly");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  //std:: cout << "grrrrrrrrrrr!!!!!!!!!!!!1";
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
  { //std::cout << "<br>input.children.size equals " << input.children.size << " instead of 2. ";
    return false;
  }
  Expression inputConverted;
  if (!input.MergeContextsMyAruments(inputConverted))
    return false;
  Weight<Polynomial<Rational> > theWeight;
  Rational cfRat;
  Polynomial<Rational> theCoefficient;
  if (!inputConverted[1].IsOfType<Polynomial<Rational> >(&theCoefficient))
  { if (!inputConverted[1].IsOfType<Rational>(&cfRat))
      return false;
    theCoefficient=cfRat;
  }
  if (!inputConverted[2].IsOfType<Weight<Polynomial<Rational> > >(&theWeight))
    return false;
  theWeight*=theCoefficient;
  return output.AssignValueWithContext(theWeight,inputConverted[2].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyWeylGroupEltByWeightPoly(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyWeylGroupEltByWeightPoly");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  //std:: cout << "grrrrrrrrrrr!!!!!!!!!!!!1";
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
  { //std::cout << "<br>input.children.size equals " << input.children.size << " instead of 2. ";
    return false;
  }
  Expression inputConverted;
  if (!input.MergeContextsMyAruments(inputConverted))
    return false;
  Weight<Polynomial<Rational> > theWeight;
  if (!inputConverted[2].IsOfType<Weight<Polynomial<Rational> > >(&theWeight))
    return false;
  if (!inputConverted[1].IsOfType<ElementWeylGroup<WeylGroup> >())
    return false;
  ElementWeylGroup<WeylGroup> theElt=inputConverted[1].GetValue<ElementWeylGroup<WeylGroup> >();
  if (theElt.owner!=&theWeight.owner->theWeyl)
  { theCommands.Comments << "<hr>Possible user input error: attempting to apply Weyl group element to weight corresponding to different Weyl group.";
    return false;
  }
  Vector<Polynomial<Rational> > theWeightSimpleCoords=theElt.owner->GetSimpleCoordinatesFromFundamental(theWeight.weightFundamentalCoordS);
  theElt.ActOn(theWeightSimpleCoords);
  theWeight.weightFundamentalCoordS=theElt.owner->GetFundamentalCoordinatesFromSimple(theWeightSimpleCoords);
  return output.AssignValueWithContext(theWeight, inputConverted[2].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyAnyByEltTensor");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  //std:: cout << "grrrrrrrrrrr!!!!!!!!!!!!1";
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
  { //std::cout << "<br>input.children.size equals " << input.children.size << " instead of 2. ";
    return false;
  }
  Expression inputConverted;
  if (!input.MergeContextsMyAruments(inputConverted))
    return false;
  if (!inputConverted[2].IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
    return false;
//  std::cout << "<br>before merge left and right are: " << leftCopy.ToString() << " and " << output.ToString();
//  std::cout << "<br>after merge left and right are: " << leftCopy.ToString() << " and " << output.ToString();
//  std::cout << "<br>after conversion, before multiplying the tensor, left copy is: " << leftCopy.ToString();
  SemisimpleLieAlgebra& theSSalg=inputConverted[2].GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >().GetOwnerSS();
  Expression leftE;
//  std::cout << "<hr>Got where i needa be";
  inputConverted[1].CheckConsistency();
  input[1].CheckConsistency();
  input[2].CheckConsistency();
  if (!inputConverted[1].ConvertToType<ElementUniversalEnveloping<RationalFunctionOld> >(leftE))
  { //theCommands.Comments << "<hr>Failed to convert " << inputConverted[1].ToString() << " to element of Universal enveloping algebra. ";
    return false;
  }
//  std::cout << " ... and got my ue out!";
  static bool theGhostHasAppeared=false;
  if (!theGhostHasAppeared)
  { std::cout << "Ere I am J.H. ... The ghost in the machine...<br>";
    theGhostHasAppeared=true;
  }
  ElementTensorsGeneralizedVermas<RationalFunctionOld> outputElt;
  //std::cout << "<br>Multiplying " << leftUE->ToString() << " * " << output.ToString();
  if (!inputConverted[2].GetValue<ElementTensorsGeneralizedVermas<RationalFunctionOld> >().MultiplyOnTheLeft
      (leftE.GetValue<ElementUniversalEnveloping<RationalFunctionOld> >(), outputElt, theSSalg, *theCommands.theGlobalVariableS, 1, 0))
  { //std::cout << "<br>failed to multiply on the left";
    return false;
  }
  return output.AssignValueWithContext(outputElt, inputConverted[2].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA");
  if (input.children.size!=3)
    return false;
  Expression inputContextsMerged;
//  std::cout << "<hr>Merging contexts: " << input.ToString();
  if (!input.MergeContextsMyArumentsAndConvertThem<ElementWeylAlgebra<Rational> >(inputContextsMerged))
    return false;
//  std::cout << "<hr>Merged contexts, ready for multiplication: " << inputContextsMerged.ToString();
  if (inputContextsMerged[1].GetValue<ElementWeylAlgebra<Rational> >().HasNonSmallPositiveIntegerDerivation() ||
      inputContextsMerged[2].GetValue<ElementWeylAlgebra<Rational> >().HasNonSmallPositiveIntegerDerivation())
  { theCommands.Comments << "<hr> Failed to multiply " << inputContextsMerged[1].ToString() << " by " << inputContextsMerged[2].ToString() << ": "
    << " one of the two differential operators has differential operator exponent that is not a small integer. ";
    return false;
  }
//  std::cout << "<hr>Multiplying " << inputContextsMerged[1].GetValue<ElementWeylAlgebra<Rational> >().ToString()
//  << " by " << inputContextsMerged[2].GetValue<ElementWeylAlgebra<Rational> >().ToString();
  ElementWeylAlgebra<Rational> result=inputContextsMerged[1].GetValue<ElementWeylAlgebra<Rational> >();
  result*=inputContextsMerged[2].GetValue<ElementWeylAlgebra<Rational> >();
  return output.AssignValueWithContext(result, inputContextsMerged[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF");
  return CalculatorFunctionsBinaryOps::innerMultiplyTypeByType<RationalFunctionOld>(theCommands, input, output);
}

bool CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrRFToRatOrPolyOrRF(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrRFToRatOrPolyOrRF");
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<RationalFunctionOld>(theCommands, input, output);
}

bool CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrEWAToRatOrPolyOrEWA(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrEWAToRatOrPolyOrEWA");
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<ElementWeylAlgebra<Rational> >(theCommands, input, output);
}

bool CalculatorFunctionsBinaryOps::innerDivideRFOrPolyOrRatByRFOrPoly(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDivideRFOrPolyOrRatByRFOrPoly");
  return CalculatorFunctionsBinaryOps::innerDivideTypeByType<RationalFunctionOld>(theCommands, input, output);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyByRatOrPoly(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyByRatOrPoly");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  return CalculatorFunctionsBinaryOps::innerMultiplyTypeByType<Polynomial<Rational> >(theCommands, input, output);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyAlgNumPolyByAlgNumPoly(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyAlgNumPolyByAlgNumPoly");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  return CalculatorFunctionsBinaryOps::innerMultiplyTypeByType<Polynomial<AlgebraicNumber> >(theCommands, input, output);
}

bool CalculatorFunctionsBinaryOps::innerAddUEToAny(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddUEToAny");
  //std::cout << "<br>adding ue to any";
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<ElementUniversalEnveloping<RationalFunctionOld> >(theCommands, input, output);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyAnyByUE(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyUEByAny");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (input.children.size!=3)
    return false;
  Expression inputContextsMerged;
  if (!input.MergeContextsMyArumentsAndConvertThem<ElementUniversalEnveloping<RationalFunctionOld> >(inputContextsMerged))
    return false;
  ElementUniversalEnveloping<RationalFunctionOld> result=inputContextsMerged[1].GetValue<ElementUniversalEnveloping<RationalFunctionOld> >();
  result*=inputContextsMerged[2].GetValue<ElementUniversalEnveloping<RationalFunctionOld> >();
  //std::cout << "before simplification: " << result.ToString() << " and after: " << result.ToString();
  result.Simplify(theCommands.theGlobalVariableS);
  return output.AssignValueWithContext(result, inputContextsMerged[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyLRObyLRO(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyLRObyLRO");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  output=input[1];
  Expression rightCopy=input[2];
  if (!output.IsOfType<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >() ||
      !rightCopy.IsOfType<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >())
    return false;
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& leftMon
  =output.GetValue<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >();
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& rightMon
  =rightCopy.GetValue<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >();
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> result;
  result=leftMon;
  result*=rightMon;
  for (int i=0; i<result.generatorsIndices.size; i++)
    if (result.Powers[i]>100000 || result.Powers[i]<0)
      return output.SetError
      ("The result of this operation is " + result.ToString() + " which is outside of the allowed range. ", theCommands);
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyLRObyLSPath(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyLRObyLSPath");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  output=input[1];
  Expression rightCopy=input[2];
  if (!output.MergeContexts(output, rightCopy))
    return false;
  if (!output.IsOfType<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >() || !rightCopy.IsOfType<LittelmannPath>())
    return false;
  LittelmannPath result=rightCopy.GetValue<LittelmannPath>();
  WeylGroup& theWeyl=*result.owner;
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> theLRO=
  output.GetValue<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >();
  for (int i=theLRO.generatorsIndices.size-1; i>=0; i--)
    if (theLRO.generatorsIndices[i]==0 || theLRO.generatorsIndices[i]< -theWeyl.GetDim() || theLRO.generatorsIndices[i]> theWeyl.GetDim())
    { std::stringstream out;
      out << " The Littelmann root operator must have an index whose absolute value is between 1 and the rank of the ambient Lie algebra, instead I get index  "
      << theLRO.generatorsIndices[i];
      return output.SetError(out.str(), theCommands);
    } else
      for (int j=0; j<theLRO.Powers[i]; j++)
        result.ActByEFDisplayIndex(theLRO.generatorsIndices[i]);
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddEltTensorToEltTensor(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddEltTensorToEltTensor");
//  std::cout << "<hr>HERE I am!";
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >(theCommands, input, output);
}

bool CalculatorFunctionsBinaryOps::innerAddRatOrPolyToRatOrPoly(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddRatOrPolyToRatOrPoly");
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<Polynomial<Rational> >(theCommands, input, output);
}

bool CalculatorFunctionsBinaryOps::innerAddAlgNumPolyToAlgNumPoly(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddAlgNumPolyToAlgNumPoly");
  return CalculatorFunctionsBinaryOps::innerAddTypeToType<Polynomial<AlgebraicNumber> >(theCommands, input, output);
}

bool CalculatorFunctionsBinaryOps::innerAddPlotToPlot(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddPlotToPlot");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  Plot leftPlot, rightPlot;
  if(!input[1].IsOfType<Plot>(&leftPlot))
    return false;
  if(!input[2].IsOfType<Plot>(&rightPlot))
    return false;
  leftPlot+=rightPlot;
  return output.AssignValue(leftPlot, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerPolyBySmallInteger(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerPolyBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  Polynomial<Rational> base;
  int thePower=0;
  if(!input[1].IsOfType(&base)|| !input[2].IsSmallInteger(&thePower))
    return false;
  if (base.IsEqualToZero() && thePower<=0)
    return output.SetError("Division by zero: trying to raise 0 to negative power. ", theCommands);
  if (thePower<0)
  { if (base.size()==1)
    { Polynomial<Rational> outputPoly;
      MonomialP theMon=base[0];
      Rational theCF=base.theCoeffs[0];
      theCF.RaiseToPower(thePower);
      theMon.RaiseToPower(thePower);
      outputPoly.MakeZero();
      outputPoly.AddMonomial(theMon, theCF);
      return output.AssignValueWithContext(outputPoly, input[1].GetContext(), theCommands);
    }
    base.RaiseToPower(-thePower);
    RationalFunctionOld theRF;
    theRF=base;
    theRF.Invert();
    return output.AssignValueWithContext(theRF, input[1].GetContext(), theCommands);
  }
  base.RaiseToPower(thePower);
  return output.AssignValueWithContext(base, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerMatRatBySmallInteger(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerMatRatBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  Matrix<Rational> base;
  int thePower=0;
  if(!input[1].IsOfType(&base)|| !input[2].IsSmallInteger(&thePower))
    return false;
  if (!base.IsSquare() || base.NumCols==0)
    return output.SetError("Exponentiating non-square matrices or matrices with zero rows is not allowed.", theCommands);
  if (thePower<=0)
    if (base.GetDeterminant()==0 )
      return output.SetError("Division by zero: trying to raise 0 to negative power. ", theCommands);
  if (thePower<0)
  { base.Invert();
    thePower*=-1;
  }
  Matrix<Rational> idMat;
  idMat.MakeIdMatrix(base.NumRows);
  MathRoutines::RaiseToPower(base, thePower, idMat);
  return output.AssignValue(base, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerAlgNumPolyBySmallInteger(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerAlgNumPolyBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
//  std::cout << "evaluating ..." << input.ToString();
  if (!input.IsListNElements(3))
    return false;
//  std::cout << "evaluating ..." << input.ToString();
  Polynomial<AlgebraicNumber> base;
  int thePower=0;
  if (!input[1].IsOfType(&base)|| !input[2].IsSmallInteger(&thePower))
    return false;
  if (thePower<0)
    return false;
  if (base.IsEqualToZero() && thePower<=0)
    return output.SetError("Division by zero: trying to raise 0 to negative power. ", theCommands);
  base.RaiseToPower(thePower);
  return output.AssignValueWithContext(base, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberBySmallInteger(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerAlgebraicNumberBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  AlgebraicNumber base;
  int thePower=0;
  if (!input[1].IsOfType(&base)|| !input[2].IsSmallInteger(&thePower))
    return false;
  if (thePower<0)
    return false;
  if (base.IsEqualToZero() && thePower<=0)
    return output.SetError("Division by zero: trying to raise 0 to negative power. ", theCommands);
  MathRoutines::RaiseToPower(base, thePower, (AlgebraicNumber) 1);
  return output.AssignValueWithContext(base, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerEWABySmallInteger(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerPolyBySmallInteger");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
//  std::cout << "raising " << input[1].ToString() << " to power " << input[2].ToString();
  ElementWeylAlgebra<Rational> base;
  int thePower=0;
  if(!input[1].IsOfType(&base))
    return false;
  bool mustCheckForRationalPower=false;
  mustCheckForRationalPower=!input[2].IsSmallInteger(&thePower);
  if (mustCheckForRationalPower || thePower<0)
  { Rational powerRat;
    if (!input[2].IsOfType<Rational>(&powerRat))
      return false;
    bool isMon=true;
    if (base.size()!=1)
      isMon=false;
    else if (base.theCoeffs[0]!=1)
      isMon=false;
    if (!isMon)
    { theCommands.Comments << "<hr>Failed to raise " << base.ToString() << " to power " << powerRat.ToString() << ": the exponent is not a "
      << " small integer and the base is not a coefficient one monomial. ";
      return false;
    }
    ElementWeylAlgebra<Rational> finalOutput;
    MonomialWeylAlgebra theMon=base[0];
    theMon.polynomialPart.RaiseToPower(powerRat);
    theMon.differentialPart.RaiseToPower(powerRat);
    for (int i=0; i<theMon.polynomialPart.GetMinNumVars(); i++)
      if (theMon.polynomialPart[i]!=0 && theMon.differentialPart[i]!=0)
      { theCommands.Comments << "<hr>Failed to raise " << base.ToString() << " to power " << powerRat.ToString() << ": the exponent is not a "
        << " small integer, the base is a monomial, however the monomial contains derivative and polynomial with respect to the same variable. ";
        return false;
      }
    finalOutput.MakeZero();
    finalOutput.AddMonomial(theMon, 1);
    return output.AssignValueWithContext(finalOutput, input[1].GetContext(), theCommands);
  }
  if (base.IsEqualToZero() && thePower<=0)
    return output.SetError("Division by zero: trying to raise 0 to negative power. ", theCommands);
  base.RaiseToPower(thePower);
  return output.AssignValueWithContext(base, input[1].GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerRatByRat(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerRatByRat");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
//  std::cout << "raising " << input[1].ToString() << " to power " << input[2].ToString();
  Rational base, exp;
  if(!input[1].IsOfType(&base))
    return false;
  if(!input[2].IsOfType(&exp))
    return false;
  int thePower;
  if (!exp.IsSmallInteger(&thePower))
    return false;
  if (base==0 && thePower<=0)
    return output.SetError("Division by zero: trying to raise 0 to negative or zero power. ", theCommands);
  base.RaiseToPower(thePower);
  return output.AssignValue(base, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerElementUEbyRatOrPolyOrRF");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  Expression inputConverted;
  if (!input.MergeContextsMyAruments(inputConverted))
    return false;
  ElementUniversalEnveloping<RationalFunctionOld> theUE;
  Expression copyExponent=inputConverted[2];
  Expression copyBase=inputConverted[1];
  if (!copyBase.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >(&theUE))
    return false;
  if (!theUE.IsAPowerOfASingleGenerator())
  { int tempPower;
    if (!copyExponent.IsSmallInteger(&tempPower))
      return false;
    theUE.RaiseToPower(tempPower);
    return output.AssignValueWithContext(theUE, copyBase.GetContext(), theCommands);
  }
  Expression exponentConverted;
  if (!copyExponent.ConvertToType<RationalFunctionOld>(exponentConverted))
    return false;
  MonomialUniversalEnveloping<RationalFunctionOld> theMon;
  theMon=theUE[0];
  theMon.Powers[0]*= exponentConverted.GetValue<RationalFunctionOld>();
  ElementUniversalEnveloping<RationalFunctionOld> outputUE;
  outputUE.MakeZero(*theUE.owneR);
  outputUE.AddMonomial(theMon, 1);
  return output.AssignValueWithContext(outputUE, copyBase.GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerSequenceByT(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerSequenceByT");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  if (!input[1].IsSequenceNElementS())
    return false;
  if (!(input[2]=="t") && !(input[2]=="T"))
    return false;
  return theCommands.innerTranspose(theCommands, input[1], output);
}

template <class coefficient>
bool Expression::AssignMatrix(const Matrix<coefficient>& input, Calculator& owner)
{ MacroRegisterFunctionWithName("Expression::AssignMatrix");
  this->reset(owner, input.NumRows+1);
  this->AddChildAtomOnTop(owner.opSequence());
  Expression currentRow, currentElt;
  for (int i=0; i<input.NumRows; i++)
  { currentRow.reset(owner);
    currentRow.children.ReservE(input.NumCols+1);
    currentRow.AddChildAtomOnTop(owner.opSequence());
    for (int j=0; j<input.NumCols; j++)
    { currentElt.AssignValue(input(i,j), owner);
      currentRow.AddChildOnTop(currentElt);
    }
    currentRow.format=this->formatMatrixRow;
    this->AddChildOnTop(currentRow);
  }
  this->format=this->formatMatrix;
  return true;
}

bool CalculatorFunctionsBinaryOps::innerPowerSequenceMatrixByRat(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerSequenceMatrixByRat");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  if (!input[1].IsSequenceNElementS())
    return false;
  Matrix<Rational> theMatRat;
  std::cout << "raising " << input[1].ToString() << " to " << input[2].ToString();
  if (theCommands.GetMatrix<Rational>(input[1], theMatRat))
  { Expression inputCopy, baseRatMat, outputMatRat;
    baseRatMat.AssignValue(theMatRat, theCommands);
    inputCopy.reset(theCommands);
    inputCopy.AddChildOnTop(input[0]);
    inputCopy.AddChildOnTop(baseRatMat);
    inputCopy.AddChildOnTop(input[2]);
    if (!CalculatorFunctionsBinaryOps::innerPowerMatRatBySmallInteger(theCommands, inputCopy, outputMatRat))
      return false;
    if (!outputMatRat.IsOfType<Matrix<Rational> >(&theMatRat))//<- probably outputMatRat is of type Error.
    { output=outputMatRat;
      return true;
    }
    return output.AssignMatrix(theMatRat, theCommands);
  }
  int thePower=0;
  if (!input[2].IsSmallInteger(&thePower))
    return false;
  if (thePower<=0)
    return false;
  Matrix<Expression> theMat;
  if (!theCommands.GetMatrixExpressions(input[1], theMat))
    return false;
  if (!theMat.IsSquare())
    return output.SetError("Attempting to raise non-square matrix to power", theCommands);
  if (theMat.NumRows>10)
  { theCommands.Comments << "I've been instructed not to exponentiate non-ratinoal matrices of dimension >10. ";
    return false;
  }
  Matrix<Expression> idMatE;
  Expression oneE, zeroE;
  oneE.AssignValue(1, theCommands);
  zeroE.AssignValue(0, theCommands);
  idMatE.MakeIdMatrix(theMat.NumRows, oneE, zeroE);
  MathRoutines::RaiseToPower(theMat, thePower, idMatE);
  return output.AssignMatrixExpressions(theMat, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerPowerDoubleOrRatToDoubleOrRat(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerRatPowerRat");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
//  std::cout << "<br>attempting to exponentiate: " << input.ToString();
  Rational base, exp;
  double baseDouble, expDouble;
  if(input[1].IsOfType(&base))
    baseDouble=base.DoubleValue();
  else if (!input[1].IsOfType(&baseDouble))
    return false;
  if(input[2].IsOfType(&exp))
    expDouble=exp.DoubleValue();
  else if (!input[2].IsOfType(&expDouble))
    return false;
  if (baseDouble<0)
  { if(!input[2].IsOfType<Rational>())
      return false;
    int thePower=0;
    if (exp.IsEven())
    { if (!exp.IsSmallInteger(&thePower))
        return false;
      return output.AssignValue(FloatingPoint::power(-baseDouble, thePower), theCommands);
    }
    baseDouble*=-1;
    return output.AssignValue(-FloatingPoint::power (baseDouble, expDouble), theCommands);
  }
  if (baseDouble==0)
    if (expDouble>0)
      return output.AssignValue<double>(0, theCommands);
  return output.AssignValue(FloatingPoint::power(baseDouble, expDouble), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyDoubleOrRatByDoubleOrRat(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerRatPowerRat");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  Rational leftR, rightR;
  double leftD, rightD;
  if(input[1].IsOfType(&leftR))
    leftD=leftR.DoubleValue();
  else if (!input[1].IsOfType(&leftD))
    return false;
  if(input[2].IsOfType(&rightR))
    rightD=rightR.DoubleValue();
  else if (!input[2].IsOfType(&rightD))
    return false;
  return output.AssignValue(leftD*rightD, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddDoubleOrRatToDoubleOrRat(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerRatPowerRat");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
//  std::cout << "Adding " << input[1].ToString() << " 'n " << input[2].ToString();
  Rational leftR, rightR;
  double leftD, rightD;
  if(input[1].IsOfType(&leftR))
    leftD=leftR.DoubleValue();
  else if (!input[1].IsOfType(&leftD))
    return false;
  if(input[2].IsOfType(&rightR))
    rightD=rightR.DoubleValue();
  else if (!input[2].IsOfType(&rightD))
    return false;
  return output.AssignValue(leftD+rightD, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyCharSSLieAlgByCharSSLieAlg(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyCharSSLieAlgByCharSSLieAlg");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  charSSAlgMod<Rational> leftC, rightC;
  if (!input[1].IsOfType(&leftC))
    return false;
  if (!input[2].IsOfType(&rightC))
    return false;
  if (leftC.GetOwner()!=rightC.GetOwner())
  { theCommands.Comments << "You asked me to multiply characters over different semisimple Lie algebras. Could this be a typo?";
    return false;
  }
  std::string successString=(leftC*=rightC);
  if (successString!="")
  { theCommands.Comments << "I tried to multiply character " << leftC.ToString() << " by " << rightC.ToString()
    << " but I failed with the following message: " << successString;
    return false;
  }
  return output.AssignValue(leftC, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRationalBySequence");
  //std::cout << "<br>here be trouble! input is a sequence of " << input.children.size << " elmeents.";
  if (!input.IsListNElements(3))
    return false;
  //std::cout << "<br>trouble be double!";
  if (!input[1].IsBuiltInScalar())
    return false;
  //std::cout << "<br>trouble be triple!";
  if (!input[2].IsSequenceNElementS())
    return false;
  output.reset(theCommands);
  output.children.ReservE(input[2].children.size);
  output.AddChildAtomOnTop(theCommands.opSequence());
  Expression tempProduct;
  for (int i=1; i<input[2].children.size; i++)
  { tempProduct.MakeProducT(theCommands, input[1], input[2][i]);
    output.AddChildOnTop(tempProduct);
  }
  return true;
}

bool CalculatorFunctionsBinaryOps::innerMultiplySequenceMatrixBySequenceMatrix(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRationalBySequence");
  //std::cout << "<br>here be trouble! input is a sequence of " << input.children.size << " elmeents.";
  if (!input.IsListNElements(3))
    return false;
  Matrix<Expression> leftMat, rightMat;
  if (!theCommands.GetMatrixExpressions(input[1], leftMat))
    return false;
  if (!theCommands.GetMatrixExpressions(input[2], rightMat, leftMat.NumCols))
    return false;
  if (leftMat.NumCols!=rightMat.NumRows)
    return false;
  Matrix<Expression> outputMat;
  outputMat.init(leftMat.NumRows, rightMat.NumCols);
  Expression leftSummand, rightSummand;
  for (int i=0; i<leftMat.NumRows; i++)
    for (int j=0; j<rightMat.NumCols; j++)
      for (int k=0; k<leftMat.NumCols; k++)
      { if (k==0)
          outputMat(i,j).MakeProducT(theCommands, leftMat(i,k), rightMat(k,j) );
        else
        { rightSummand=outputMat(i,j);
          leftSummand.MakeProducT(theCommands, leftMat(i,k), rightMat(k,j));
          outputMat(i,j).MakeXOX(theCommands, theCommands.opPlus(), leftSummand, rightSummand);
        }
      }
  return output.AssignMatrixExpressions(outputMat, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerTensorMatRatByMatRat(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return false;
  const Expression& leftE=input[1];
  const Expression& rightE=input[2];
  if (!rightE.IsOfType<Matrix<Rational> >() || !leftE.IsOfType<Matrix<Rational> >())
    return false;
//  if (leftE.GetValue<Matrix<Rational> >().NumCols!=rightMat.NumRows)
//    return false;
  Matrix<Rational> result;
  result.AssignTensorProduct(leftE.GetValue<Matrix<Rational> >(), rightE.GetValue<Matrix<Rational> >());
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerTensorMatByMatTensor(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return false;
  const Expression& leftE=input[1];
  const Expression& rightE=input[2];
  if (!rightE.IsOfType<MatrixTensor<Rational> >() || !leftE.IsOfType<MatrixTensor<Rational> >())
    return false;
//  if (leftE.GetValue<Matrix<Rational> >().NumCols!=rightMat.NumRows)
//    return false;
  MatrixTensor<Rational> result;
  result.AssignTensorProduct(leftE.GetValue<MatrixTensor<Rational> >(), rightE.GetValue<MatrixTensor<Rational> >());
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyMatrixRationalOrRationalByMatrixRational(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return false;
  const Expression& leftE=input[1];
  const Expression& rightE=input[2];
  if (!rightE.IsOfType<Matrix<Rational> >())
    return false;
  Rational theScalar;
  if (leftE.IsOfType<Rational>(&theScalar))
  { Matrix<Rational> result=rightE.GetValue<Matrix<Rational> >();
    result*=theScalar;
    return output.AssignValue(result, theCommands);
  }
  if (!leftE.IsOfType<Matrix<Rational> >())
    return false;
  const Matrix<Rational>& rightMat=rightE.GetValue<Matrix<Rational> >();
  if (leftE.GetValue<Matrix<Rational> >().NumCols!=rightMat.NumRows)
    return false;
  Matrix<Rational> result=leftE.GetValue<Matrix<Rational> >();
  result.MultiplyOnTheRight(rightMat);
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplyMatrixTensorOrRationalByMatrixTensor(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyMatrixTensorOrRationalByMatrixTensor");
  if (!input.IsListNElements(3))
    return false;
  const Expression& leftE=input[1];
  const Expression& rightE=input[2];
  if (!rightE.IsOfType<MatrixTensor<Rational> >())
    return false;
  Rational theScalar;
  if (leftE.IsOfType<Rational>(&theScalar))
  { MatrixTensor<Rational> result=rightE.GetValue<MatrixTensor<Rational> >();
    result*=theScalar;
    return output.AssignValue(result, theCommands);
  }
  if (!leftE.IsOfType<MatrixTensor<Rational> >())
    return false;
  const MatrixTensor<Rational>& rightMat=rightE.GetValue<MatrixTensor<Rational> >();
  MatrixTensor<Rational> result=leftE.GetValue<MatrixTensor<Rational> >();
  result*=rightMat;
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerLieBracketRatOrUEWithRatOrUE(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerLieBracketUEbyUE");
  if (!input.IsListNElements(3))
    return false;
  Expression inputConverted;
  if (!input.MergeContextsMyAruments(inputConverted))
    return false;
  const Expression& leftE=inputConverted[1];
  const Expression& rightE=inputConverted[2];
  if (leftE.IsOfType<Rational>() || rightE.IsOfType<Rational>())
    return output.AssignValueWithContext(0, leftE.GetContext() , theCommands);
  if (leftE.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >() && rightE.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >() )
  { ElementUniversalEnveloping<RationalFunctionOld> result;
    leftE.GetValue<ElementUniversalEnveloping<RationalFunctionOld> >().LieBracketOnTheRight(rightE.GetValue<ElementUniversalEnveloping<RationalFunctionOld> >(), result);
    //std::cout << "before simplification: " << result.ToString();
    result.Simplify(theCommands.theGlobalVariableS);
    //std::cout << " after: " << result.ToString();
    return output.AssignValueWithContext(result, leftE.GetContext(), theCommands);
  }
  return false;
}

bool CalculatorFunctionsBinaryOps::innerLieBracketRatPolyOrEWAWithRatPolyOrEWA(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerLieBracketRatPolyOrEWAWithRatPolyOrEWA");
  if (!input.IsListNElements(3))
    return false;
  Expression inputConverted;
  if (!input.MergeContextsMyAruments(inputConverted))
    return false;
  const Expression& leftE=inputConverted[1];
  const Expression& rightE=inputConverted[2];
  bool leftEisGood =leftE.IsOfType<Rational>()  || leftE.IsOfType<Polynomial<Rational> >()  || leftE.IsOfType<ElementWeylAlgebra<Rational> >();
  bool rightEisGood=rightE.IsOfType<Rational>() || rightE.IsOfType<Polynomial<Rational> >() || rightE.IsOfType<ElementWeylAlgebra<Rational> >();
  if (!leftEisGood || !rightEisGood)
    return false;
  if (!leftE.IsOfType<ElementWeylAlgebra<Rational> >() && !rightE.IsOfType<ElementWeylAlgebra<Rational> >())
    return output.AssignValue(0, theCommands);
  Expression leftConverted, rightConverted;
  if (!leftE.ConvertToType<ElementWeylAlgebra<Rational> >(leftConverted) || !rightE.ConvertToType<ElementWeylAlgebra<Rational> >(rightConverted))
  { theCommands.Comments << "<hr>Failed with conversion to Element weyl algebra - possible programming error?";
    return false;
  }
  ElementWeylAlgebra<Rational> resultE=rightConverted.GetValue<ElementWeylAlgebra<Rational> >();
  resultE.LieBracketOnTheLeft(leftConverted.GetValue<ElementWeylAlgebra<Rational> >(), theCommands.theGlobalVariableS);
  return output.AssignValueWithContext(resultE, leftConverted.GetContext(), theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddMatrixRationalToMatrixRational(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddMatrixRationalToMatrixRational");
  if (!input.IsListNElements(3))
    return false;
  const Expression& leftE=input[1];
  const Expression& rightE=input[2];
  if (!rightE.IsOfType<Matrix<Rational> >()|| !leftE.IsOfType<Matrix<Rational> >())
    return false;
  const Matrix<Rational>& rightMat=rightE.GetValue<Matrix<Rational> >();
  const Matrix<Rational>& leftMat=leftE.GetValue<Matrix<Rational> >();
  if (rightMat.NumRows!=leftMat.NumRows || rightMat.NumCols!=leftMat.NumCols)
    return false;
  Matrix<Rational> result=leftMat;
  result+=rightMat;
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerAddMatrixTensorToMatrixTensor(Calculator& theCommands, const Expression& input, Expression& output)
{ if (!input.IsListNElements(3))
    return false;
  const Expression& leftE=input[1];
  const Expression& rightE=input[2];
  if (!rightE.IsOfType<MatrixTensor<Rational> >()|| !leftE.IsOfType<MatrixTensor<Rational> >())
    return false;
  const MatrixTensor<Rational>& rightMat=rightE.GetValue<MatrixTensor<Rational> >();
  const MatrixTensor<Rational>& leftMat=leftE.GetValue<MatrixTensor<Rational> >();
  MatrixTensor<Rational> result=leftMat;
  result+=rightMat;
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsBinaryOps::innerMultiplySequenceByAnyScalar(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRationalBySequence");
  //std::cout << "<br>here be trouble! input is a sequence of " << input.children.size << " elmeents.";
  if (!input.IsListNElements(3))
    return false;
  Expression tempE=input;
  tempE.children.SwapTwoIndices(1,2);
  return CalculatorFunctionsBinaryOps::innerMultiplyAnyScalarBySequence(theCommands, tempE, output);
}

bool CalculatorFunctionsBinaryOps::innerAddSequenceToSequence(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyRationalBySequence");
  //std::cout << "<br>here be trouble! input is a sequence of " << input.children.size << " elmeents.";
  if (!input.IsListNElements(3))
    return false;
  //std::cout << "<br>trouble be double!";
  if (!input[1].IsSequenceNElementS())
    return false;
  //std::cout << "<br>trouble be triple!";
  if (!input[2].IsSequenceNElementS())
    return false;
  if (input[2].children.size!=input[1].children.size)
  { theCommands.Comments << "<hr>Attempting to add a sequence of length " << input[1].children.size-1 << "  to a sequence of length "
    << input[2].children.size-1 << ", possible user typo?";
    return false;
  }
  output.reset(theCommands);
  output.children.ReservE(input[1].children.size);
  output.AddChildAtomOnTop(theCommands.opSequence());
  if (input[1].format==Expression::formatMatrixRow || input[2].format==Expression::formatMatrixRow)
    output.format=Expression::formatMatrixRow;
  if (input[1].format==Expression::formatMatrix || input[2].format==Expression::formatMatrix)
    output.format=Expression::formatMatrix;
  Expression tempSum;
  for (int i=1; i<input[2].children.size; i++)
  { tempSum.MakeXOX(theCommands, theCommands.opPlus(), input[1][i], input[2][i]);
    output.AddChildOnTop(tempSum);
  }
  return true;
}

bool CalculatorFunctionsBinaryOps::innerNChooseK(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerNChooseK");
  if (!input.IsListNElements(3))
    return false;
  Rational N;
  int K;
  if (!input[1].IsOfType<Rational>(&N) || !input[2].IsSmallInteger(&K))
    return false;
//  std::cout << N.ToString();
  if (K<0)
    return output.AssignValue(0, theCommands);
  Rational result= result.NChooseK(N, K);
  return output.AssignValue(result, theCommands);
}
