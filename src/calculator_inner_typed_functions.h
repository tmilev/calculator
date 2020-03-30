// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader6_1InnerTypedFns_h_already_included
#define vpfHeader6_1InnerTypedFns_h_already_included

#include "calculator_interface.h"

class CalculatorFunctionsBinaryOps {
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
  static bool innerAddMatrixToMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDirectSumMatrixWithMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAugmentMatrixToTheRight(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAugmentMatrixBelow(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerAddMatrixRationalOrAlgebraicToMatrixRationalOrAlgebraic(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  static bool innerAddMatrixRFsToMatrixRFs(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddMatrixTensorToMatrixTensor(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddRatToRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerAddStringToString(Calculator& theCommands, const Expression& input, Expression& output);
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
  static bool innerMultiplyAnyScalarByMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyAnyScalarBySequence(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplySequenceByAnyScalar(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplySequenceByMatrix(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerMultiplyMatrixByMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyMatRatOrMatAlgByMatRatOrMatAlg(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyRatOrAlgebraicByMatRatOrMatAlg(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyMatrixRationalOrRationalByMatrixRational(
    Calculator& theCommands, const Expression& input, Expression& output
  );
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
  static bool innerMultiplyEllipticCurveElements(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyEllipticCurveElementsZmodP(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyRatOrPolyByWeightPoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerMultiplyWeylGroupEltByWeightPoly(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerLieBracketExtractConstant(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLieBracketDistribute(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLieBracketRatOrUEWithRatOrUE(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLieBracketRatPolyOrEWAWithRatPolyOrEWA(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLieBracketSwapTermsIfNeeded(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerLieBracketJacobiIdentityIfNeeded(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerDivideRatByRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDivideDoubleByDouble(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat(
    Calculator& theCommands, const Expression& input, Expression& output
  );
  static bool innerDivideRFOrPolyOrRatByRFOrPoly(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerDivideEltZmodPorRatByEltZmodPorRat(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerPowerMatNumbersBySmallInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerMatExpressionsBySmallInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerMatNumbersByLargeIntegerIfPossible(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerPowerRationalByRationalOutputAlgebraic(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerRationalByRationalReducePrimeFactors(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerRationalByInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerPolyBySmallInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerAlgNumPolyBySmallInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerAlgebraicNumberBySmallInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerRadicalAlgebraicNumberPositiveDefault(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerEWABySmallInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerElementUEbyRatOrPolyOrRF(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerDoubleOrRatToDoubleOrRat(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerSequenceOrMatrixByT(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerPowerElementZmodPByInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerEllipticCurveRationalElementByInteger(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerPowerEllipticCurveZmodPElementByInteger(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerNChooseK(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerSetMinus(Calculator& theCommands, const Expression& input, Expression& output);

  static bool innerTensorEltTensorByEltTensor(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTensorMatrixByMatrix(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerTensorMatByMatTensor(Calculator& theCommands, const Expression& input, Expression& output);
};

template <class theType>
bool CalculatorFunctionsBinaryOps::innerMultiplyTypeByType(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyTypeByType");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.MergeContextsMyArumentsAndConvertThem<theType>(inputContextsMerged, &theCommands.Comments)) {
    return false;
  }
  theType result = inputContextsMerged[1].GetValue<theType>();
  result *= inputContextsMerged[2].GetValue<theType>();
  return output.AssignValueWithContext(result, inputContextsMerged[1].GetContext(), theCommands);
}

template <class theType>
bool CalculatorFunctionsBinaryOps::innerAddTypeToType(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddTypeToType");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.MergeContextsMyArumentsAndConvertThem<theType>(inputContextsMerged, &theCommands.Comments)) {
    return false;
  }
  theType result;
  result = inputContextsMerged[1].GetValue<theType>();
  result += inputContextsMerged[2].GetValue<theType>();
  return output.AssignValueWithContext(result, inputContextsMerged[1].GetContext(), theCommands);
}

template <class theType>
bool CalculatorFunctionsBinaryOps::innerDivideTypeByType(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddTypeToType");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.MergeContextsMyArumentsAndConvertThem<theType>(inputContextsMerged, &theCommands.Comments)) {
    return false;
  }
  if (inputContextsMerged[2].GetValue<theType>().IsEqualToZero()) {
    return output.MakeError("Division by zero. ", theCommands);
  }
  theType result = inputContextsMerged[1].GetValue<theType>();
  result /= inputContextsMerged[2].GetValue<theType>();
  return output.AssignValueWithContext(result, inputContextsMerged[1].GetContext(), theCommands);
}

template <class coefficient>
bool CalculatorConversions::innerPolynomial(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerPolynomial");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorConversions::functionPolynomial<coefficient>(theCommands, input[1], output);
}

template <class coefficient>
bool CalculatorConversions::functionPolynomial(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionPolynomial");
  RecursionDepthCounter theRecursionCounter(&theCommands.RecursionDeptH);
  if (theCommands.RecursionDeptH > theCommands.MaxRecursionDeptH) {
    return theCommands << "Max recursion depth of " << theCommands.MaxRecursionDeptH
    << " exceeded while trying to evaluate polynomial expression (i.e. your polynomial expression is too large).";
  }
  if (input.IsOfType<Polynomial<coefficient> >()) {
    output = input;
    return true;
  }
  if (input.IsOfType<Polynomial<Rational> >()) {
    Polynomial<Rational> thePolynomial;
    Polynomial<coefficient> converted;
    input.IsOfType(&thePolynomial);
    converted = thePolynomial;
    return output.AssignValueWithContext(converted, input.GetContext(), theCommands);
  }
  if (input.IsOfType<coefficient>() || input.IsOfType<Rational>()) {
    if (!input.ConvertInternally<Polynomial<coefficient> >(output)) {
      global.fatal << "This is a programming error: "
      << "failed to convert coefficient to polynomial. " << global.fatal;
    }
    return true;
  }
  Expression theConverted, theComputed;
  if (
    input.IsListStartingWithAtom(theCommands.opTimes()) ||
    input.IsListStartingWithAtom(theCommands.opPlus())
  ) {
    theComputed.reset(theCommands, input.size());
    theComputed.AddChildOnTop(input[0]);
    for (int i = 1; i < input.size(); i ++) {
      if (!CalculatorConversions::functionPolynomial<coefficient>(
        theCommands, input[i], theConverted
      )) {
        return theCommands << "<hr>Failed to extract polynomial from "
        << input[i].toString();
      }
      theComputed.AddChildOnTop(theConverted);
    }
    if (input.IsListStartingWithAtom(theCommands.opTimes())) {
      return CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolyByNumberOrPoly(
        theCommands, theComputed, output
      );
    }
    if (input.IsListStartingWithAtom(theCommands.opPlus())) {
      return CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly(theCommands, theComputed, output);
    }
    global.fatal << "Error, this line of code should never be reached. " << global.fatal;
  }
  if (input.StartsWith(theCommands.opMinus(), 3)) {
    theComputed.reset(theCommands, input.size());
    theComputed.AddChildOnTop(input[0]);
    for (int i = 1; i < 3; i ++) {
      Expression summand = input[i];
      if (i == 2) {
        summand *= - 1;
      }
      if (!CalculatorConversions::functionPolynomial<coefficient>(theCommands, summand, theConverted)) {
        return theCommands << "<hr>Failed to extract polynomial from " << summand.toString();
      }
      theComputed.AddChildOnTop(theConverted);
    }
    return CalculatorFunctionsBinaryOps::innerAddNumberOrPolyToNumberOrPoly(theCommands, theComputed, output);
  }

  int thePower = - 1;
  if (input.StartsWith(theCommands.opThePower(), 3)) {
    if (input[2].IsSmallInteger(&thePower)) {
      if (!CalculatorConversions::functionPolynomial<coefficient>(theCommands, input[1], theConverted)) {
        return theCommands << "<hr>Failed to extract polynomial from " << input[1].toString() << ".";
      }
      Polynomial<coefficient> resultP = theConverted.GetValue<Polynomial<coefficient> >();
      if (thePower < 0) {
        coefficient theConst;
        if (!resultP.IsConstant(&theConst)) {
          theCommands << "<hr>Failed to extract polynomial from  "
          << input.toString() << " because the exponent was negative. "
          << "Please make sure that this is not a typo. "
          << "I am treating " << input.toString() << " as a single variable. ";
          Polynomial<coefficient> JustAmonomial;
          JustAmonomial.MakeMonomiaL(0, 1, 1);
          Expression theContext;
          theContext.ContextMakeContextWithOnePolyVar(theCommands, input);
          return output.AssignValueWithContext(JustAmonomial, theContext, theCommands);
        }
        theConst.Invert();
        thePower *= - 1;
        resultP = theConst;
      }
      if (thePower == 0) {
        if (resultP.IsEqualToZero()) {
          return output.MakeError(
            "Error: 0^0 is undefined in the present version of the calculator. ",
            theCommands
          );
        }
      }
      resultP.RaiseToPower(thePower);
      return output.AssignValueWithContext(resultP, theConverted.GetContext(), theCommands);
    }
  }
  Polynomial<coefficient> JustAmonomial;
  JustAmonomial.MakeMonomiaL(0, 1, 1);
  Expression theContext;
  theContext.ContextMakeContextWithOnePolyVar(theCommands, input);
  return output.AssignValueWithContext(JustAmonomial, theContext, theCommands);
}
#endif
