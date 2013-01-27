//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
ProjectInformationInstance ProjectInfoVpf6_1cpp
(__FILE__, "Implementation file for the calculator parser: implementation of inner binary typed functions. ");


bool CommandList::innerAddRatToRat
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerAddRatToRat");
  if (input.children.size!=2)
    return false;
  Rational leftR, rightR;
  if (!input[0].IsOfType(&leftR) || !input[1].IsOfType(&rightR))
    return false;
  return output.AssignValue(leftR+rightR, theCommands);
}

bool CommandList::innerMultiplyRatByRat
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerMultiplyRatByRat");
  if (input.children.size!=2)
    return false;
  Rational leftR, rightR;
  if (!input[0].IsOfType(&leftR) || !input[1].IsOfType(&rightR))
    return false;
  return output.AssignValue(leftR*rightR, theCommands);
}

bool CommandList::innerDivideRatByRat
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerDivideRatByRat");
  if (input.children.size!=2)
    return false;
  Rational leftR, rightR;
  if (!input[0].IsOfType(&leftR) || !input[1].IsOfType(&rightR))
    return false;
  if (rightR.IsEqualToZero())
    return output.SetError("Division by zero.", theCommands);
  return output.AssignValue(leftR/rightR, theCommands);
}

bool CommandList::innerTensorEltTensorByEltTensor
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerTensorEltTensorByEltTensor");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (input.children.size!=2)
    return false;
  Expression leftCopy=input[0];
  output=input[1];
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

bool CommandList::innerMultiplyAnyByEltTensor
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerMultiplyAnyByEltTensor");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (input.children.size!=2)
  { std::cout << "<br>input.children.size equals " << input.children.size << " instead of 2. ";
    return false;
  }
  if (!input[1].IsOfType<ElementTensorsGeneralizedVermas<RationalFunctionOld> >())
  { std::cout << "<br>input[1] is not tensor product, instead it is " << input[1].ToString() ;
    return false;
  }
  static bool theGhostHasAppeared=false;
  output=input[1];
  Expression leftCopy=input[0];
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
  { std::cout << "Error to convert: " << errorString;
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
//    std::cout << "<br>Multiplying " << leftCopy.GetUE().ToString() << " * " << output.ToString();
  if (!output.GetValuE<ElementTensorsGeneralizedVermas<RationalFunctionOld> >().MultiplyOnTheLeft
      (*leftUE, outputElt, theCommands.theObjectContainer.theCategoryOmodules,
       theSSalg, *theCommands.theGlobalVariableS, RFOne, RFZero))
  { //std::cout << "<br>failed to multiply on the left";
    return false;
  }
  return output.AssignValueWithContext(outputElt , output.GetContext(), theCommands);
}

bool CommandList::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (input.children.size!=2)
    return false;
  output=input[0];
  Expression rightCopy=input[1];
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

bool CommandList::innerAddRatOrPolyOrRFToRatOrPolyOrRF
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerAddRatOrPolyOrRFToRatOrPolyOrRF");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (input.children.size!=2)
    return false;
  output=input[0];
  Expression rightCopy=input[1];
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

bool CommandList::innerDivideRFOrPolyOrRatByRFOrPoly
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerDivideRFOrPolyOrRatByRFOrPoly");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (input.children.size!=2)
    return false;
  output=input[0];
  Expression rightCopy=input[1];
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
  if (right->IsEqualToZero())
    return output.SetError("Division error. ", theCommands);
  RationalFunctionOld result=*left;
//  std::cout << "dividing " << result.ToString() << " by " << rightCopy.GetValuE<RationalFunctionOld>().ToString();
  result/=*right;
//  std::cout << " to get " << result.ToString();
  return output.AssignValueWithContext(result, output.GetContext(), theCommands);
}

bool CommandList::innerMultiplyRatOrPolyByRatOrPoly
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerMultiplyRatOrPolyByRatOrPoly");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (input.children.size!=2)
    return false;
  output=input[0];
  Expression rightCopy=input[1];
  Polynomial<Rational>* left;
  Polynomial<Rational>* right;
  std::string errorString;
  if (!output.MergeContexts(output, rightCopy))
    return false;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerPolynomial, output, left, &errorString))
    return false;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerPolynomial, rightCopy, right, &errorString))
    return false;
  Polynomial<Rational> result=*left;
//  std::cout << "dividing " << result.ToString() << " by " << rightCopy.GetValuE<RationalFunctionOld>().ToString();
  result*=*right;
//  std::cout << " to get " << result.ToString();
  return output.AssignValueWithContext(result, output.GetContext(), theCommands);
}

bool CommandList::innerAddUEToAny
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerAddUEToAny");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (input.children.size!=2)
    return false;
  output=input[0];
  if (!output.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >())
    return false;
  Expression rightCopy=input[1];
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

bool CommandList::innerMultiplyUEByAny
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerMultiplyUEByAny");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (input.children.size!=2)
    return false;
  output=input[0];
  if (!output.IsOfType<ElementUniversalEnveloping<RationalFunctionOld> >())
    return false;
  Expression rightCopy=input[1];
  ElementUniversalEnveloping<RationalFunctionOld>* right;
  std::string errorString;
  if (!output.MergeContexts(output, rightCopy))
    return false;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerElementUniversalEnvelopingAlgebra, rightCopy, right, &errorString))
    return false;
  ElementUniversalEnveloping<RationalFunctionOld> result
  =output.GetValuE<ElementUniversalEnveloping<RationalFunctionOld> >();
  result*=*right;
//  std::cout << "dividing " << result.ToString() << " by " << rightCopy.GetValuE<RationalFunctionOld>().ToString();
//  std::cout << " to get " << result.ToString();
  return output.AssignValueWithContext(result, output.GetContext(), theCommands);
}

bool CommandList::innerMultiplyLRObyLRO
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerMultiplyLRObyLRO");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (input.children.size!=2)
    return false;
  output=input[0];
  Expression rightCopy=input[1];
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

bool CommandList::innerMultiplyLRObyLSPath
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerMultiplyLRObyLSPath");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (input.children.size!=2)
    return false;
  output=input[0];
  Expression rightCopy=input[1];
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

bool CommandList::innerAddEltTensorToEltTensor
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerAddEltTensorToEltTensor");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (input.children.size!=2)
    return false;
  output=input[0];
  Expression rightCopy=input[1];
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

bool CommandList::innerAddRatOrPolyToRatOrPoly
(CommandList& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CommandList::innerAddRatOrPolyToRatOrPoly");
  //std::cout << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  theCommands.CheckInputNotSameAsOutput(input, output);
  if (input.children.size!=2)
    return false;
  output=input[0];
  Expression rightCopy=input[1];
  Polynomial<Rational>* left;
  Polynomial<Rational>* right;
  std::string errorString;
  if (!output.MergeContexts(output, rightCopy))
    return false;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerPolynomial, output, left, &errorString))
    return false;
  if (!theCommands.CallConversionFunctionReturnsNonConstUseCarefully
      (theCommands.innerPolynomial, rightCopy, right, &errorString))
    return false;
  Polynomial<Rational> result=*left;
//  std::cout << "dividing " << result.ToString() << " by " << rightCopy.GetValuE<RationalFunctionOld>().ToString();
  result+=*right;
//  std::cout << " to get " << result.ToString();
  return output.AssignValueWithContext(result, output.GetContext(), theCommands);
}
