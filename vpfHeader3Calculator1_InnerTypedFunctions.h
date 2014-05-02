//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader6_1InnerTypedFns_h_already_included
#define vpfHeader6_1InnerTypedFns_h_already_included

static ProjectInformationInstance ProjectInfoVpf6_1Header(__FILE__, "Header, calculator inner binary typed functions. ");

class CalculatorFunctionsBinaryOps
{
public:
  template <class theType>
  static bool innerMultiplyTypeByType(Calculator& theCommands, const Expression& input, Expression& output);
  template <class theType>
  static bool innerAddTypeToType(Calculator& theCommands, const Expression& input, Expression& output);
  template <class theType>
  static bool innerDivideTypeByType(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerAddDoubleOrRatToDoubleOrRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddRatOrPolyOrEWAToRatOrPolyOrEWA(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddRatOrPolyOrRFToRatOrPolyOrRF(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddUEToAny(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddEltTensorToEltTensor(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddRatOrPolyToRatOrPoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddAlgNumPolyToAlgNumPoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddPlotToPlot(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddSequenceToSequence(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddMatrixRationalToMatrixRational(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddMatrixTensorToMatrixTensor(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddRatToRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddEltZmodPorRatToEltZmodPorRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddAlgebraicNumberToAlgebraicNumber(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddWeightToWeight(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyAlgNumPolyByAlgNumPoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyRatOrPolyByRatOrPoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyLRObyLRO(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyLRObyLSPath(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyEltZmodPorRatByEltZmodPorRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyAnyScalarBySequence(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplySequenceByAnyScalar(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplySequenceMatrixBySequenceMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyMatrixRationalOrRationalByMatrixRational(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyMatrixTensorOrRationalByMatrixTensor(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyAlgebraicNumberByAlgebraicNumber(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyRatByRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyDoubleOrRatByDoubleOrRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyCoxeterEltByCoxeterElt(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyCharacterByCharacter(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyCharSSLieAlgByCharSSLieAlg(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyAnyByUE(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyAnyByEltTensor(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyRatOrPolyByWeightPoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyWeylGroupEltByWeightPoly(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerLieBracketRatOrUEWithRatOrUE(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLieBracketRatPolyOrEWAWithRatPolyOrEWA(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerDivideRatByRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDivideDoubleByDouble(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDivideRFOrPolyOrRatByRFOrPoly(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerPowerMatRatBySmallInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerRatByRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerPolyBySmallInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerAlgNumPolyBySmallInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerAlgebraicNumberBySmallInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerEWABySmallInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerElementUEbyRatOrPolyOrRF(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerDoubleOrRatToDoubleOrRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerSequenceByT(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerSequenceMatrixByRat(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerNChooseK(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerTensorEltTensorByEltTensor(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTensorMatRatByMatRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTensorMatByMatTensor(Calculator& theCommands, const Expression& input, Expression& output);
};

template <class theType>
bool CalculatorFunctionsBinaryOps::innerMultiplyTypeByType(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyTypeByType");
  if (input.children.size!=3)
    return false;
  Expression inputContextsMerged;
  if (!input.MergeContextsMyArumentsAndConvertThem<theType>(inputContextsMerged))
    return false;
//  stOutput << "Merged contexts, ready for multiplication: " << inputContextsMerged.ToString();
  theType result=inputContextsMerged[1].GetValue<theType>();
  result*=inputContextsMerged[2].GetValue<theType>();
  return output.AssignValueWithContext(result, inputContextsMerged[1].GetContext(), theCommands);
}

template <class theType>
bool CalculatorFunctionsBinaryOps::innerAddTypeToType(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddTypeToType");
  if (input.children.size!=3)
    return false;
  Expression inputContextsMerged;
  if (!input.MergeContextsMyArumentsAndConvertThem<theType>(inputContextsMerged))
    return false;
  theType result=inputContextsMerged[1].GetValue<theType>();
  result+=inputContextsMerged[2].GetValue<theType>();
  return output.AssignValueWithContext(result, inputContextsMerged[1].GetContext(), theCommands);
}

template <class theType>
bool CalculatorFunctionsBinaryOps::innerDivideTypeByType(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddTypeToType");
  if (input.children.size!=3)
    return false;
  Expression inputContextsMerged;
  if (!input.MergeContextsMyArumentsAndConvertThem<theType>(inputContextsMerged))
    return false;
  if (inputContextsMerged[2].GetValue<theType>().IsEqualToZero())
    return output.SetError("Division by zero. ", theCommands);
  theType result=inputContextsMerged[1].GetValue<theType>();
  result/=inputContextsMerged[2].GetValue<theType>();
  return output.AssignValueWithContext(result, inputContextsMerged[1].GetContext(), theCommands);
}

template <class coefficient>
bool CalculatorSerialization::innerPolynomial(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorSerialization::innerPolynomial");
  RecursionDepthCounter theRecursionCounter(&theCommands.RecursionDeptH);
//  stOutput << "Extracting poly from: " << input.ToString();
  if (theCommands.RecursionDeptH>theCommands.MaxRecursionDeptH)
  { theCommands.Comments << "Max recursion depth of " << theCommands.MaxRecursionDeptH
    << " exceeded while trying to evaluate polynomial expression (i.e. your polynomial expression is too large).";
    return false;
  }
  if (input.IsOfType<Polynomial<coefficient> >())
  { output=input;
    return true;
  }
  if (input.IsOfType<coefficient>() || input.IsOfType<Rational>())
  { if (!input.ConvertToType<Polynomial<coefficient> >(output))
      crash << "This is a programming error: failed to convert coefficient to polynomial. " << crash;
    return true;
  }
  Expression theConverted, theComputed;
  if (input.IsListStartingWithAtom(theCommands.opTimes()) || input.IsListStartingWithAtom(theCommands.opPlus()))
  { theComputed.reset(theCommands, input.children.size);
    theComputed.AddChildOnTop(input[0]);
    for (int i=1; i<input.children.size; i++)
    { if (!CalculatorSerialization::innerPolynomial<coefficient>(theCommands, input[i], theConverted))
      { theCommands.Comments << "<hr>Failed to extract polynomial from " << input[i].ToString();
        return false;
      }
      theComputed.AddChildOnTop(theConverted);
    }
    if (input.IsListStartingWithAtom(theCommands.opTimes()))
      return CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyByRatOrPoly(theCommands, theComputed, output);
    if (input.IsListStartingWithAtom(theCommands.opPlus()))
      return CalculatorFunctionsBinaryOps::innerAddRatOrPolyToRatOrPoly(theCommands, theComputed, output);
    crash << "Error, this line of code should never be reached. " << crash;
  }
  if (input.StartsWith(theCommands.opThePower(), 3))
  { if(!CalculatorSerialization::innerPolynomial<coefficient>(theCommands, input[1], theConverted))
    { theCommands.Comments << "<hr>Failed to extract polynomial from " << input[1].ToString() << ".";
      return false;
    }
    int thePower=-1;
    if (!input[2].IsSmallInteger(&thePower))
    { theCommands.Comments << "<hr>Failed to extract polynomial from " << input.ToString() << " because the exponenet was not a small integer.";
      return false;
    }
    Polynomial<coefficient> resultP=theConverted.GetValue<Polynomial<coefficient> >();
    if (thePower<0)
      if (!resultP.IsAConstant())
      { theCommands.Comments << "<hr>Failed to extract polynomial from  " << input.ToString() << " because the exponent was negative. ";
        return false;
      }
    if (thePower==0)
      if (resultP.IsEqualToZero())
        return output.SetError("Error: 0^0 is undefined. ", theCommands);
    resultP.RaiseToPower(thePower);
    return output.AssignValueWithContext(resultP, theConverted.GetContext(), theCommands);
  }

  Polynomial<coefficient> JustAmonomial;
  JustAmonomial.MakeMonomiaL(0,1,1);
  Expression theContext;
  theContext.ContextMakeContextWithOnePolyVar(theCommands, input);
  return output.AssignValueWithContext(JustAmonomial, theContext, theCommands);
}


#endif
