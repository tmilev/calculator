//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpfHeader3Calculator1_InnerTypedFunctions.h"
#include "vpfImplementationHeader2Math3_WeylAlgebra.h"
#include "vpfImplementationHeader2Math15_UniversalEnveloping.h"

ProjectInformationInstance ProjectInfoVpf6_1cpp(__FILE__, "Calculator inner binary typed functions. ");

bool CommandList::innerOperationBinary(CommandList& theCommands, const Expression& input, Expression& output, int theOp)
{ for (int i=0; i<theCommands.FunctionHandlers[theOp].size; i++)
    if (theCommands.FunctionHandlers[theOp][i].inputFitsMyInnerType(input))
      if (theCommands.FunctionHandlers[theOp][i].theFunction(theCommands, input, output))
        return true;
  return false;
}

bool CommandList::outerExtractBaseMultiplication(CommandList& theCommands, const Expression& input, Expression& output)
{ RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("CommandList::outerExtractBaseMultiplication");
  if (!input.IsListNElementsStartingWithAtom(theCommands.opTimes(), 3))
    return false;
  bool result=false;
  //  std::cout << "<br>handling base extraction of: " << input.ToString();
  //handle Anything*Rational:=Rational*Anything
  output=input;
  if (output[2].IsOfType<Rational>())
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

bool CommandListInnerTypedFunctions::innerAddEltZmodPorRatToEltZmodPorRat(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddEltZmodPorRatToEltZmodPorRat");
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

bool CommandListInnerTypedFunctions::innerMultiplyEltZmodPorRatByEltZmodPorRat(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyEltZmodPorRatByEltZmodPorRat");
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

bool CommandListInnerTypedFunctions::innerAddRatToRat(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddRatToRat");
  if (!input.IsListNElements(3))
    return false;
  Rational leftR, rightR;
  if (!input[1].IsOfType(&leftR) || !input[2].IsOfType(&rightR))
    return false;
  return output.AssignValue(leftR+rightR, theCommands);
}

bool CommandListInnerTypedFunctions::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat");
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

bool CommandListInnerTypedFunctions::innerMultiplyAlgebraicNumberByAlgebraicNumber(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyAlgebraicNumberByAlgebraicNumber");
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

bool CommandListInnerTypedFunctions::innerAddAlgebraicNumberToAlgebraicNumber(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddAlgebraicNumberToAlgebraicNumber");
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

bool CommandListInnerTypedFunctions::innerMultiplyRatByRat(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyRatByRat");
  if (!input.IsListNElements(3))
    return false;
  Rational leftR, rightR;
  if (!input[1].IsOfType(&leftR) || !input[2].IsOfType(&rightR))
    return false;
  return output.AssignValue(leftR*rightR, theCommands);
}

bool CommandListInnerTypedFunctions::innerMultiplyCoxeterEltByCoxeterElt(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyCoxeterEltByCoxeterElt");
  if (!input.IsListNElements(3))
    return false;
  //std::cout << "<br>Here i am!";
  //std::cout << "Multiplying: " << input[1].ToString() << " by " << input[2].ToString();
  ElementWeylGroup leftR, rightR;
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

bool CommandListInnerTypedFunctions::innerDivideRatByRat(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerDivideRatByRat");
  if (!input.IsListNElements(3))
    return false;
  Rational leftR, rightR;
  if (!input[1].IsOfType(&leftR) || !input[2].IsOfType(&rightR))
    return false;
  if (rightR.IsEqualToZero())
    return output.SetError("Division by zero.", theCommands);
  return output.AssignValue(leftR/rightR, theCommands);
}

bool CommandListInnerTypedFunctions::innerDivideDoubleByDouble(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerDivideRatByRat");
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

bool CommandListInnerTypedFunctions::innerTensorEltTensorByEltTensor(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerTensorEltTensorByEltTensor");
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

bool CommandListInnerTypedFunctions::innerMultiplyAnyByEltTensor(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyAnyByEltTensor");
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

bool CommandListInnerTypedFunctions::innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA");
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

bool CommandListInnerTypedFunctions::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF");
  return CommandListInnerTypedFunctions::innerMultiplyTypeByType<RationalFunctionOld>(theCommands, input, output);
}

bool CommandListInnerTypedFunctions::innerAddRatOrPolyOrRFToRatOrPolyOrRF(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddRatOrPolyOrRFToRatOrPolyOrRF");
  return CommandListInnerTypedFunctions::innerAddTypeToType<RationalFunctionOld>(theCommands, input, output);
}

bool CommandListInnerTypedFunctions::innerAddRatOrPolyOrEWAToRatOrPolyOrEWA(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddRatOrPolyOrEWAToRatOrPolyOrEWA");
  return CommandListInnerTypedFunctions::innerAddTypeToType<ElementWeylAlgebra<Rational> >(theCommands, input, output);
}

bool CommandListInnerTypedFunctions::innerDivideRFOrPolyOrRatByRFOrPoly(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerDivideRFOrPolyOrRatByRFOrPoly");
  return CommandListInnerTypedFunctions::innerDivideTypeByType<RationalFunctionOld>(theCommands, input, output);
}

bool CommandListInnerTypedFunctions::innerMultiplyRatOrPolyByRatOrPoly(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyRatOrPolyByRatOrPoly");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  return CommandListInnerTypedFunctions::innerMultiplyTypeByType<Polynomial<Rational> >(theCommands, input, output);
}

bool CommandListInnerTypedFunctions::innerMultiplyAlgNumPolyByAlgNumPoly(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyAlgNumPolyByAlgNumPoly");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  return CommandListInnerTypedFunctions::innerMultiplyTypeByType<Polynomial<AlgebraicNumber> >(theCommands, input, output);
}

bool CommandListInnerTypedFunctions::innerAddUEToAny(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddUEToAny");
  //std::cout << "<br>adding ue to any";
  return CommandListInnerTypedFunctions::innerAddTypeToType<ElementUniversalEnveloping<RationalFunctionOld> >(theCommands, input, output);
}

bool CommandListInnerTypedFunctions::innerMultiplyAnyByUE(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyUEByAny");
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

bool CommandListInnerTypedFunctions::innerMultiplyLRObyLRO(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyLRObyLRO");
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

bool CommandListInnerTypedFunctions::innerMultiplyLRObyLSPath(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyLRObyLSPath");
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

bool CommandListInnerTypedFunctions::innerAddEltTensorToEltTensor(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddEltTensorToEltTensor");
//  std::cout << "<hr>HERE I am!";
  return CommandListInnerTypedFunctions::innerAddTypeToType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >(theCommands, input, output);
}

bool CommandListInnerTypedFunctions::innerAddRatOrPolyToRatOrPoly(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddRatOrPolyToRatOrPoly");
  return CommandListInnerTypedFunctions::innerAddTypeToType<Polynomial<Rational> >(theCommands, input, output);
}

bool CommandListInnerTypedFunctions::innerAddAlgNumPolyToAlgNumPoly(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddAlgNumPolyToAlgNumPoly");
  return CommandListInnerTypedFunctions::innerAddTypeToType<Polynomial<AlgebraicNumber> >(theCommands, input, output);
}

bool CommandListInnerTypedFunctions::innerAddPlotToPlot(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddPlotToPlot");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  CalculusFunctionPlot leftPlot, rightPlot;
  if(!input[1].IsOfType<CalculusFunctionPlot>(&leftPlot))
    return false;
  if(!input[2].IsOfType<CalculusFunctionPlot>(&rightPlot))
    return false;
  leftPlot+=rightPlot;
  return output.AssignValue(leftPlot, theCommands);
}

bool CommandListInnerTypedFunctions::innerPowerPolyBySmallInteger(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerPowerPolyBySmallInteger");
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

bool CommandListInnerTypedFunctions::innerPowerAlgNumPolyBySmallInteger(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerPowerAlgNumPolyBySmallInteger");
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

bool CommandListInnerTypedFunctions::innerPowerAlgebraicNumberBySmallInteger(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerPowerAlgebraicNumberBySmallInteger");
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

bool CommandListInnerTypedFunctions::innerPowerEWABySmallInteger(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerPowerPolyBySmallInteger");
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

bool CommandListInnerTypedFunctions::innerPowerRatByRat(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerPowerRatByRat");
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

bool CommandListInnerTypedFunctions::innerPowerElementUEbyRatOrPolyOrRF(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerPowerElementUEbyRatOrPolyOrRF");
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

bool CommandListInnerTypedFunctions::innerPowerSequenceByT(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerPowerSequenceByT");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  if (!input[1].IsSequenceNElementS())
    return false;
  if (!(input[2]=="t") && !(input[2]=="T"))
    return false;
  return theCommands.innerTranspose(theCommands, input[1], output);
}

bool CommandListInnerTypedFunctions::innerPowerDoubleOrRatToDoubleOrRat(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerRatPowerRat");
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

bool CommandListInnerTypedFunctions::innerMultiplyDoubleOrRatByDoubleOrRat(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerRatPowerRat");
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

bool CommandListInnerTypedFunctions::innerAddDoubleOrRatToDoubleOrRat(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerRatPowerRat");
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

bool CommandListInnerTypedFunctions::innerMultiplyCharSSLieAlgByCharSSLieAlg(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyCharSSLieAlgByCharSSLieAlg");
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

bool CommandListInnerTypedFunctions::innerMultiplyRationalBySequence(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyRationalBySequence");
  //std::cout << "<br>here be trouble! input is a sequence of " << input.children.size << " elmeents.";
  if (!input.IsListNElements(3))
    return false;
  //std::cout << "<br>trouble be double!";
  if (!input[1].IsOfType<Rational>())
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

bool CommandListInnerTypedFunctions::innerMultiplyMatrixSequenceByMatrixSequence(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyRationalBySequence");
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

bool CommandListInnerTypedFunctions::innerTensorMatRatByMatRat(CommandList& theCommands, const Expression& input, Expression& output)
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

bool CommandListInnerTypedFunctions::innerTensorMatByMatTensor(CommandList& theCommands, const Expression& input, Expression& output)
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

bool CommandListInnerTypedFunctions::innerMultiplyMatrixRationalOrRationalByMatrixRational(CommandList& theCommands, const Expression& input, Expression& output)
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

bool CommandListInnerTypedFunctions::innerMultiplyMatrixTensorOrRationalByMatrixTensor(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyMatrixTensorOrRationalByMatrixTensor");
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

bool CommandListInnerTypedFunctions::innerLieBracketRatOrUEWithRatOrUE(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerLieBracketUEbyUE");
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

bool CommandListInnerTypedFunctions::innerLieBracketRatPolyOrEWAWithRatPolyOrEWA(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerLieBracketRatPolyOrEWAWithRatPolyOrEWA");
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

bool CommandListInnerTypedFunctions::innerAddMatrixRationalToMatrixRational(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddMatrixRationalToMatrixRational");
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

bool CommandListInnerTypedFunctions::innerAddMatrixTensorToMatrixTensor(CommandList& theCommands, const Expression& input, Expression& output)
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

bool CommandListInnerTypedFunctions::innerMultiplySequenceByRational(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyRationalBySequence");
  //std::cout << "<br>here be trouble! input is a sequence of " << input.children.size << " elmeents.";
  if (!input.IsListNElements(3))
    return false;
  Expression tempE=input;
  tempE.children.SwapTwoIndices(1,2);
  return CommandListInnerTypedFunctions::innerMultiplyRationalBySequence(theCommands, tempE, output);
}

bool CommandListInnerTypedFunctions::innerAddSequenceToSequence(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyRationalBySequence");
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
    << input[2].children.size-1 << ", possible user error?";
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

bool CommandListInnerTypedFunctions::innerNChooseK(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerNChooseK");
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

bool CommandList::innerHWTAABF(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerHWTAABF");
  RecursionDepthCounter theRecursionCounter(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(4))
    return output.SetError("Function expects three arguments.", theCommands);
  Expression leftMerged=input[1];
  Expression rightMerged=input[2];
  if (!Expression::MergeContexts(leftMerged, rightMerged))
    return output.SetError("Failed to merge the contexts of the two UE elements ", theCommands);
  Expression finalContext=rightMerged.GetContext();
  int algebraIndex=finalContext.ContextGetIndexAmbientSSalg();
  if (algebraIndex==-1)
    return output.SetError("I couldn't extract a Lie algebra to compute hwtaabf.", theCommands);
  SemisimpleLieAlgebra* constSSalg= &theCommands.theObjectContainer.theLieAlgebras.GetElement(algebraIndex);
  const Expression& weightExpression=input[3];
  Vector<RationalFunctionOld> weight;
  if (!theCommands.GetVectoR<RationalFunctionOld>(weightExpression, weight, &finalContext, constSSalg->GetRank(), theCommands.innerRationalFunction))
  { theCommands.Comments << "<hr>Failed to obtain highest weight from the third argument which is " << weightExpression.ToString();
    return false;
  }
  if (!leftMerged.SetContextAtLeastEqualTo(finalContext) || !rightMerged.SetContextAtLeastEqualTo(finalContext))
    return output.SetError("Failed to merge the contexts of the highest weight and the elements of the Universal enveloping. ", theCommands);
  Expression leftConverted, rightConverted;
  if (!leftMerged.ConvertToType<ElementUniversalEnveloping<RationalFunctionOld> >(leftConverted))
    return false;
  if (!rightMerged.ConvertToType<ElementUniversalEnveloping<RationalFunctionOld> >(rightConverted))
    return false;
  const ElementUniversalEnveloping<RationalFunctionOld>& leftUE=leftConverted.GetValue<ElementUniversalEnveloping<RationalFunctionOld> >();
  const ElementUniversalEnveloping<RationalFunctionOld>& rightUE=rightConverted.GetValue<ElementUniversalEnveloping<RationalFunctionOld> >();
  SemisimpleLieAlgebra theSSalgebra;
  theSSalgebra=*constSSalg;
  WeylGroup& theWeyl=theSSalgebra.theWeyl;
  std::stringstream out;
  Vector<RationalFunctionOld> hwDualCoords;
  theSSalgebra.OrderSSalgebraForHWbfComputation();
  hwDualCoords=theWeyl.GetDualCoordinatesFromFundamental(weight);
  RationalFunctionOld outputRF;
  //std::cout << "<br>The highest weight in dual coordinates, as I understand it:" << hwDualCoords.ToString();
  if(!leftUE.HWTAAbilinearForm(rightUE, outputRF, &hwDualCoords, *theCommands.theGlobalVariableS, 1, 0, &theCommands.Comments))
    return output.SetError("Error: couldn't compute Shapovalov form, see comments.", theCommands);
  return output.AssignValueWithContext(outputRF, finalContext, theCommands);
}
