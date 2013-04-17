//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpf6_1innerTypedFunctions.h"
ProjectInformationInstance ProjectInfoVpf6_1cpp
(__FILE__, "Implementation file for the calculator parser: implementation of inner binary typed functions. ");

bool CommandListInnerTypedFunctions::innerAddRatToRat
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddRatToRat");
  if (!input.IsListNElements(3))
    return false;
  Rational leftR, rightR;
  if (!input[1].IsOfType(&leftR) || !input[2].IsOfType(&rightR))
    return false;
  return output.AssignValue(leftR+rightR, theCommands);
}

bool CommandListInnerTypedFunctions::innerMultiplyRatByRat
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyRatByRat");
  if (!input.IsListNElements(3))
    return false;
  Rational leftR, rightR;
  if (!input[1].IsOfType(&leftR) || !input[2].IsOfType(&rightR))
    return false;
  return output.AssignValue(leftR*rightR, theCommands);
}

bool CommandListInnerTypedFunctions::innerMultiplyCoxeterEltByCoxeterElt
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyCoxeterEltByCoxeterElt");
  if (!input.IsListNElements(3))
    return false;
  //std::cout << "<br>Here i am!";
  //std::cout << "Multiplying: " << input[1].ToString() << " by " << input[2].ToString();
  CoxeterElement leftR, rightR;
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

bool CommandListInnerTypedFunctions::innerDivideRatByRat
(CommandList& theCommands, const Expression& input, Expression& output)
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

bool CommandListInnerTypedFunctions::innerTensorEltTensorByEltTensor
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerTensorEltTensorByEltTensor");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
//  std::cout << "<br>Attempting to tensor " << input[1].ToString() << " and " << input[2].ToString();

  Expression leftCopy=input[1];
  output=input[2];
  if (!output.IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
    return false;
  if (!leftCopy.IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
    return false;
  if (!output.MergeContexts(leftCopy, output))
    return false;
  ElementTensorsGeneralizedVermas<RationalFunctionOld>
  resultTensor=leftCopy.GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
  resultTensor.TensorOnTheRight
  (output.GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >(),
   *theCommands.theGlobalVariableS);
  return output.AssignValueWithContext(resultTensor, output.GetContext(), theCommands);
}

bool CommandListInnerTypedFunctions::innerMultiplyAnyByEltTensor
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyAnyByEltTensor");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  //std:: cout << "grrrrrrrrrrr!!!!!!!!!!!!1";
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
  { //std::cout << "<br>input.children.size equals " << input.children.size << " instead of 2. ";
    return false;
  }
  if (!input[2].IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
  { //std::cout << "<br>input[2] is not tensor product, instead it is " << input[2].ToString()
    //<< "."; //",stack trace:  "  << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    return false;
  }
  static bool theGhostHasAppeared=false;
  output=input[2];
  Expression leftCopy=input[1];
  std::cout << "<br>before merge left and right are: " << leftCopy.ToString() << " and " << output.ToString();
  if (!output.MergeContexts(leftCopy, output))
  { std::cout << "<br>failed context merge of " << leftCopy.ToString() << " and " << output.ToString();
    return false;
  }
  std::cout << "<br>after merge left and right are: " << leftCopy.ToString() << " and " << output.ToString();
  ElementUniversalEnveloping<RationalFunctionOld>* leftUE;
  std::string errorString;
//  std::cout << "<br>after merge left and right are: " << leftCopy.ToString() << " and " << output.ToString();
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerElementUniversalEnvelopingAlgebra, leftCopy, leftUE, &errorString))
  { //std::cout << "Error to convert: " << errorString;
    return false;
  }
//  std::cout << "<br>after conversion, before multiplying the tensor, left copy is: " << leftCopy.ToString();
  if (!theGhostHasAppeared)
  { std::cout << "Ere I am J.H. ... The ghost in the machine...<br>";
    theGhostHasAppeared=true;
  }
  RationalFunctionOld RFOne, RFZero;
  RFZero.MakeZero(theCommands.theGlobalVariableS);
  RFOne.MakeOne(theCommands.theGlobalVariableS);
  ElementTensorsGeneralizedVermas<RationalFunctionOld> outputElt;
  SemisimpleLieAlgebra& theSSalg=*output.GetAmbientSSAlgebraNonConstUseWithCaution();
  std::cout << "<br>Multiplying "
  << leftCopy.GetValuE<ElementUniversalEnveloping<RationalFunctionOld> >().ToString()
  << " * " << output.ToString();
  if (!output.GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >().MultiplyOnTheLeft
      (*leftUE, outputElt, theSSalg, *theCommands.theGlobalVariableS, RFOne, RFZero))
  { //std::cout << "<br>failed to multiply on the left";
    return false;
  }
  return output.AssignValueWithContext(outputElt , output.GetContext(), theCommands);
}

bool CommandListInnerTypedFunctions::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  output=input[1];
  Expression rightCopy=input[2];
  RationalFunctionOld* left;
  RationalFunctionOld* right;
  std::string errorString;
  if (!output.MergeContexts(output, rightCopy))
    return false;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerRationalFunction, output, left, &errorString))
    return false;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerRationalFunction, rightCopy, right, &errorString))
    return false;
  RationalFunctionOld result;
  result=*left;
  result*=*right;
  return output.AssignValueWithContext(result, output.GetContext(), theCommands);
}

bool CommandListInnerTypedFunctions::innerAddRatOrPolyOrRFToRatOrPolyOrRF
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddRatOrPolyOrRFToRatOrPolyOrRF");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  output=input[1];
  Expression rightCopy=input[2];
  RationalFunctionOld* left;
  RationalFunctionOld* right;
  std::string errorString;
  if (!output.MergeContexts(output, rightCopy))
    return false;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerRationalFunction, output, left, &errorString))
    return false;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerRationalFunction, rightCopy, right, &errorString))
    return false;
  RationalFunctionOld result;
  result=*left;
  result+=*right;
  return output.AssignValueWithContext(result, output.GetContext(), theCommands);
}

bool CommandListInnerTypedFunctions::innerDivideRFOrPolyOrRatByRFOrPoly
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerDivideRFOrPolyOrRatByRFOrPoly");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
//  std::cout << "<br>attempting to divide " << input[1].ToString() << " by " << input[2].ToString();
  output=input[1];
  Expression rightCopy=input[2];
  RationalFunctionOld* left;
  RationalFunctionOld* right;
  std::string errorString;
  if (!output.MergeContexts(output, rightCopy))
    return false;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerRationalFunction, output, left, &errorString))
    return false;
  left->checkConsistency();
//  std::cout << "<br>numerical value of left: " << left;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerRationalFunction, rightCopy, right, &errorString))
    return false;
  right->checkConsistency();
  if (right->IsEqualToZero())
    return output.SetError("Division error. ", theCommands);
//  std::cout << "<br>The numerical value of left " << left << ", the numberical value of right "
//  << right << "<br>";
  //for (int i=0; i<theCommands.theObjectContainer.theRFs.size; i++)
  //{ //std::cout << "Address of entry " << i+1 << ": "
    //<< &theCommands.theObjectContainer.theRFs[i] << "<br>";
  //}
  left->checkConsistency();
  RationalFunctionOld result=*left;
  result.checkConsistency();
  std::cout << "<br>dividing " << result.ToString() << " by " << right->ToString();
  result/=*right;
  std::cout << " to get " << result.ToString();
  return output.AssignValueWithContext(result, output.GetContext(), theCommands);
}

bool CommandListInnerTypedFunctions::innerMultiplyRatOrPolyByRatOrPoly
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyRatOrPolyByRatOrPoly");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  output=input[1];
  Expression rightCopy=input[2];
  Polynomial<Rational>* left;
  Polynomial<Rational>* right;
  std::string errorString;
  if (!output.MergeContexts(output, rightCopy))
    return false;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerPolynomial, output, left, &errorString))
    return false;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerPolynomial, rightCopy, right, &errorString))
    return false;
  Polynomial<Rational> result=*left;
//  std::cout << "dividing " << result.ToString() << " by " << rightCopy.GetValuE<RationalFunctionOld>().ToString();
  result*=*right;
//  std::cout << " to get " << result.ToString();
  return output.AssignValueWithContext(result, output.GetContext(), theCommands);
}

bool CommandListInnerTypedFunctions::innerAddUEToAny
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddUEToAny");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  output=input[1];
  if (!output.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >())
    return false;
  Expression rightCopy=input[2];
  ElementUniversalEnveloping<RationalFunctionOld>* right;
  std::string errorString;
  if (!output.MergeContexts(output, rightCopy))
    return false;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerElementUniversalEnvelopingAlgebra, rightCopy, right, &errorString))
    return false;
  ElementUniversalEnveloping<RationalFunctionOld> result
  =output.GetValuE<ElementUniversalEnveloping<RationalFunctionOld> >();
  result+=*right;
//  std::cout << "dividing " << result.ToString() << " by " << rightCopy.GetValuE<RationalFunctionOld>().ToString();
//  std::cout << " to get " << result.ToString();
  return output.AssignValueWithContext(result, output.GetContext(), theCommands);
}

bool CommandListInnerTypedFunctions::innerMultiplyAnyByUE
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyUEByAny");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  Expression leftCopy=input[1];
  Expression rightCopy=input[2];
  if (!rightCopy.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >())
    return false;
  ElementUniversalEnveloping<RationalFunctionOld>* left;
  std::string errorString;
  if (!leftCopy.MergeContexts(leftCopy, rightCopy))
    return false;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerElementUniversalEnvelopingAlgebra, leftCopy, left, &errorString))
    return false;
  ElementUniversalEnveloping<RationalFunctionOld> result=*left;
  result*=
  rightCopy.GetValuE<ElementUniversalEnveloping<RationalFunctionOld> >();
//  std::cout << "dividing " << result.ToString() << " by " << rightCopy.GetValuE<RationalFunctionOld>().ToString();
//  std::cout << " to get " << result.ToString();
  return output.AssignValueWithContext(result, leftCopy.GetContext(), theCommands);
}

bool CommandListInnerTypedFunctions::innerMultiplyLRObyLRO
(CommandList& theCommands, const Expression& input, Expression& output)
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
  =output.GetValuE<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >();
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& rightMon
  =rightCopy.GetValuE<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >();
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> result;
  result=leftMon;
  result*=rightMon;
  for (int i=0; i<result.generatorsIndices.size; i++)
    if (result.Powers[i]>100000 || result.Powers[i]<0)
      return output.SetError
      ("The result of this operation is " + result.ToString() +
       " which is outside of the allowed range. ", theCommands);
  return output.AssignValueWithContext(result, output.GetContext(), theCommands);
}

bool CommandListInnerTypedFunctions::innerMultiplyLRObyLSPath
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyLRObyLSPath");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  output=input[1];
  Expression rightCopy=input[2];
  if (!output.MergeContexts(output, rightCopy))
    return false;
  if (!output.IsOfType<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >() ||
      !rightCopy.IsOfType<LittelmannPath>())
    return false;
  LittelmannPath result=rightCopy.GetValuE<LittelmannPath>();
  WeylGroup& theWeyl=*result.owner;
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> theLRO=
  output.GetValuE<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >();
  for (int i=theLRO.generatorsIndices.size-1; i>=0; i--)
    if (theLRO.generatorsIndices[i]==0 || theLRO.generatorsIndices[i]< -theWeyl.GetDim()
        || theLRO.generatorsIndices[i]> theWeyl.GetDim())
    { std::stringstream out;
      out << " The Littelmann root operator must have an index whose absolute value is "
      << "between 1 and the rank of the ambient Lie algebra, instead I get index  "
      << theLRO.generatorsIndices[i];
      return output.SetError(out.str(), theCommands);
    } else
      for (int j=0; j<theLRO.Powers[i]; j++)
        result.ActByEFDisplayIndex(theLRO.generatorsIndices[i]);
  return output.AssignValueWithContext(result, output.GetContext(), theCommands);
}

bool CommandListInnerTypedFunctions::innerAddEltTensorToEltTensor
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddEltTensorToEltTensor");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
//  std::cout << "<br>adding element gvm and element gvm. ";
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  output=input[1];
  Expression rightCopy=input[2];
  if (!output.MergeContexts(output, rightCopy))
    return false;
  if (!output.IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >() ||
      !rightCopy.IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
    return false;
  ElementTensorsGeneralizedVermas<RationalFunctionOld> result;
  result=output.GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
  result+=rightCopy.GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >();
  //std::cout << "result of addition: " << result.ToString();
  return output.AssignValueWithContext(result, output.GetContext(), theCommands);
}

bool CommandListInnerTypedFunctions::innerAddRatOrPolyToRatOrPoly
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerAddRatOrPolyToRatOrPoly");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  output=input[1];
  Expression rightCopy=input[2];
  Polynomial<Rational>* left;
  Polynomial<Rational>* right;
  std::string errorString;
  if (!output.MergeContexts(output, rightCopy))
    return false;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerPolynomial, output, left, &errorString))
    return false;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (Serialization::innerPolynomial, rightCopy, right, &errorString))
    return false;
  Polynomial<Rational> result=*left;
//  std::cout << "dividing " << result.ToString() << " by " << rightCopy.GetValuE<RationalFunctionOld>().ToString();
  result+=*right;
//  std::cout << " to get " << result.ToString();
  return output.AssignValueWithContext(result, output.GetContext(), theCommands);
}

bool CommandListInnerTypedFunctions::innerAddPlotToPlot
(CommandList& theCommands, const Expression& input, Expression& output)
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

bool CommandListInnerTypedFunctions::innerRatPowerRat
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerRatPowerRat");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  Rational base, exp;
  if(!input[1].IsOfType(&base))
    return false;
  if(!input[2].IsOfType(&exp))
    return false;
  int thePower;
  if (!exp.IsSmallInteger(&thePower))
    return false;
  if (base==0 && thePower<0)
    return output.SetError("Division by zero: trying to raise 0 to negative power. ", theCommands);
  base.RaiseToPower(thePower);
  return output.AssignValue(base, theCommands);
}

bool CommandListInnerTypedFunctions::innerElementUEPowerRatOrPolyOrRF
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerElementUEPowerRatOrPolyOrRF");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  ElementUniversalEnveloping<RationalFunctionOld> theUE;
  RationalFunctionOld theExponent;
  Expression copyExponent=input[2];
  Expression copyBase=input[1];
  if (!copyExponent.MergeContexts(copyExponent, copyBase))
    return false;
  if (!copyBase.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> > (&theUE))
    return false;
  if (!theUE.IsAPowerOfASingleGenerator())
  { int tempPower;
    if (!copyExponent.IsSmallInteger(&tempPower))
      return false;
    theUE.RaiseToPower(tempPower);
    return output.AssignValueWithContext(theUE, copyExponent.GetContext(), theCommands);
  }
  if (copyExponent.IsOfType<Rational>())
    theExponent=copyExponent.GetValuE<Rational>();
  else if (copyExponent.IsOfType<Polynomial<Rational> >())
    theExponent=copyExponent.GetValuE<Polynomial<Rational> >();
  else if (!copyExponent.IsOfType<RationalFunctionOld>(&theExponent))
    return false;
  MonomialUniversalEnveloping<RationalFunctionOld> theMon;
  theMon=theUE[0];
  theMon.Powers[0]*= theExponent;
  ElementUniversalEnveloping<RationalFunctionOld> outputUE;
  outputUE.MakeZero(*theUE.owneR);
  outputUE.AddMonomial(theMon, 1);
  return output.AssignValueWithContext(outputUE, copyExponent.GetContext(), theCommands);
}

bool CommandListInnerTypedFunctions::innerDoubleOrRatPowerDoubleOrRat
(CommandList& theCommands, const Expression& input, Expression& output)
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
    if (exp.IsEven())
      return false;
    baseDouble*=-1;
    return output.AssignValue(-pow (baseDouble, expDouble), theCommands);
  }
  if (baseDouble==0)
    if (expDouble>0)
      return output.AssignValue<double>(0, theCommands);
  return output.AssignValue(pow(baseDouble, expDouble), theCommands);
}

bool CommandListInnerTypedFunctions::innerDoubleOrRatTimesDoubleOrRat
(CommandList& theCommands, const Expression& input, Expression& output)
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

bool CommandListInnerTypedFunctions::innerDoubleOrRatPlusDoubleOrRat
(CommandList& theCommands, const Expression& input, Expression& output)
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
  return output.AssignValue(leftD+rightD, theCommands);
}

bool CommandListInnerTypedFunctions::innerMultiplyCharSSLieAlgByCharSSLieAlg
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandListInnerTypedFunctions::innerMultiplyCharSSLieAlgByCharSSLieAlg");
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (!input.IsListNElements(3))
    return false;
  charSSAlgMod<Rational> leftC, rightC;
  if (!input[1].IsOfType(& leftC))
    return false;
  if (!input[2].IsOfType(&rightC))
    return false;
  if (leftC.owner!=rightC.owner)
  { theCommands.Comments
    << "You asked me to multiply characters over different semisimple Lie algebras. "
    << "Could this be a typo?";
    return false;
  }
  std::string successString=(leftC*=rightC);
  if (successString!="")
  { theCommands.Comments << "I tried to multiply character " << leftC.ToString() << " by "
    << rightC.ToString() << " but I failed with the following message: "
    << successString;
    return false;
  }
  return output.AssignValue(leftC, theCommands);
}

bool CommandListInnerTypedFunctions::innerMultiplyRationalBySequence
(CommandList& theCommands, const Expression& input, Expression& output)
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
  output.AddAtomOnTop(theCommands.opSequence());
  Expression tempProduct;
  for (int i=1; i<input[2].children.size; i++)
  { tempProduct.MakeProducT(theCommands, input[1], input[2][i]);
    output.AddChildOnTop(tempProduct);
  }
  return true;
}
