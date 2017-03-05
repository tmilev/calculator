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
  static bool innerAddNumberOrPolyToNumberOrPoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddPlotToPlot(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddSequenceToSequence(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddMatrixRationalOrAlgebraicToMatrixRationalOrAlgebraic(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddMatrixRFsToMatrixRFs(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddMatrixTensorToMatrixTensor(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddRatToRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddEltZmodPorRatToEltZmodPorRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddAlgebraicNumberToAlgebraicNumber(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddWeightToWeight(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerMultiplyEltHypOctByEltHypOct(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyNumberOrPolyByNumberOrPoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyLRObyLRO(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyLRObyLSPath(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyEltZmodPorRatByEltZmodPorRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyAnyScalarBySequence(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplySequenceByAnyScalar(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplySequenceMatrixBySequenceMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyMatRatOrMatAlgByMatRatOrMatAlg(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyRatOrAlgebraicByMatRatOrMatAlg(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyMatrixRationalOrRationalByMatrixRational(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyMatrixRFOrRFByMatrixRF(Calculator& theCommands, const Expression& input, Expression& output);
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
  static bool innerDivideEltZmodPorRatByEltZmodPorRat(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerPowerMatBySmallInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerRatByRatReducePrimeFactors(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerRatByRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerRatByRatGetAlgebraicNumber(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerPolyBySmallInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerAlgNumPolyBySmallInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerAlgebraicNumberBySmallInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerEWABySmallInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerElementUEbyRatOrPolyOrRF(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerDoubleOrRatToDoubleOrRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerSequenceByT(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerSequenceMatrixByRat(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerNChooseK(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSetMinus(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerTensorEltTensorByEltTensor(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTensorMatRatByMatRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTensorMatByMatTensor(Calculator& theCommands, const Expression& input, Expression& output);
};

template <class theType>
bool CalculatorFunctionsBinaryOps::innerMultiplyTypeByType(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyTypeByType");
  if (input.size()!=3)
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
  if (input.size()!=3)
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
  if (input.size()!=3)
    return false;
  Expression inputContextsMerged;
  if (!input.MergeContextsMyArumentsAndConvertThem<theType>(inputContextsMerged))
    return false;
  if (inputContextsMerged[2].GetValue<theType>().IsEqualToZero())
    return output.MakeError("Division by zero. ", theCommands, true);
  theType result=inputContextsMerged[1].GetValue<theType>();
  result/=inputContextsMerged[2].GetValue<theType>();
  return output.AssignValueWithContext(result, inputContextsMerged[1].GetContext(), theCommands);
}

template <class coefficient>
bool CalculatorConversions::innerPolynomial(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerPolynomial");
  RecursionDepthCounter theRecursionCounter(&theCommands.RecursionDeptH);
//  stOutput << "Extracting poly from: " << input.ToString();
  if (theCommands.RecursionDeptH>theCommands.MaxRecursionDeptH)
    return theCommands << "Max recursion depth of " << theCommands.MaxRecursionDeptH
    << " exceeded while trying to evaluate polynomial expression (i.e. your polynomial expression is too large).";
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
    { if (!CalculatorConversions::innerPolynomial<coefficient>(theCommands, input[i], theConverted))
        return theCommands << "<hr>Failed to extract polynomial from " << input[i].ToString();
      theComputed.AddChildOnTop(theConverted);
    }
    if (input.IsListStartingWithAtom(theCommands.opTimes()))
      return CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly(theCommands, theComputed, output);
    if (input.IsListStartingWithAtom(theCommands.opPlus()))
      return CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly(theCommands, theComputed, output);
    crash << "Error, this line of code should never be reached. " << crash;
  }
  int thePower=-1;
  if (input.StartsWith(theCommands.opThePower(), 3))
    if (input[2].IsSmallInteger(&thePower))
    { if(!CalculatorConversions::innerPolynomial<coefficient>(theCommands, input[1], theConverted))
        return theCommands << "<hr>Failed to extract polynomial from " << input[1].ToString() << ".";
      Polynomial<coefficient> resultP=theConverted.GetValue<Polynomial<coefficient> >();
      if (thePower<0)
      { coefficient theConst;
        if (!resultP.IsConstant(&theConst))
        { theCommands << "<hr>Failed to extract polynomial from  "
          << input.ToString() << " because the exponent was negative. "
          << "Please make sure that this is not a typo."
          << " I am treating " << input.ToString() << " as a single variable. "
          ;
          Polynomial<coefficient> JustAmonomial;
          JustAmonomial.MakeMonomiaL(0,1,1);
          Expression theContext;
          theContext.ContextMakeContextWithOnePolyVar(theCommands, input);
          return output.AssignValueWithContext(JustAmonomial, theContext, theCommands);
        }
        theConst.Invert();
        thePower*=-1;
        resultP=theConst;
      }
      if (thePower==0)
        if (resultP.IsEqualToZero())
          return output.MakeError("Error: 0^0 is undefined in the present version of the calculator. ", theCommands);
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
