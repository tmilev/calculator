// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader6_1InnerTypedFns_h_already_included
#define vpfHeader6_1InnerTypedFns_h_already_included

#include "calculator_interface.h"

class CalculatorFunctionsBinaryOps {
public:
  template <class theType>
  static bool innerMultiplyTypeByType(Calculator& calculator, const Expression& input, Expression& output);
  template <class theType>
  static bool innerAddTypeToType(Calculator& calculator, const Expression& input, Expression& output);
  template <class theType>
  static bool innerDivideTypeByType(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerAddDoubleOrRationalToDoubleOrRational(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddRatOrPolyOrEWAToRatOrPolyOrEWA(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddRatOrPolyOrRFToRatOrPolyOrRF(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddUEToAny(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddEltTensorToEltTensor(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddNumberOrPolynomialToNumberOrPolynomial(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddPlotToPlot(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddSequenceToSequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddMatrixToMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDirectSumMatrixWithMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAugmentMatrixToTheRight(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAugmentMatrixBelow(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerAddMatrixRationalOrAlgebraicToMatrixRationalOrAlgebraic(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerAddMatrixRFsToMatrixRFs(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddMatrixTensorToMatrixTensor(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddRatToRat(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddStringToString(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddElementZModPOrRationalToElementZModPOrRational(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddPolynomialModuloIntegerToPolynomialModuloInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerAddPolynomialModuloIntegerToInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerAddPolynomialModuloPolynomialModuloIntegerToPolynomialModuloPolynomialModuloInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );

  static bool innerAddAlgebraicNumberToAlgebraicNumber(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddWeightToWeight(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerMultiplyEltHypOctByEltHypOct(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyRatOrPolyOrEWAByRatOrPolyOrEWA(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyPolynomialModPByPolynomialModP(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyNumberOrPolynomialByNumberOrPolynomial(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyPolynomialModPolynomialModPToPolynomialModPolynomialModP(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerMultiplyLRObyLRO(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyLRObyLSPath(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyEltZmodPorRatByEltZmodPorRat(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyAnyScalarByMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyAnyScalarBySequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplySequenceByAnyScalar(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplySequenceByMatrix(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerMultiplyMatrixByMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyMatRatOrMatAlgByMatRatOrMatAlg(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyRatOrAlgebraicByMatRatOrMatAlg(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyMatrixRationalOrRationalByMatrixRational(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerMultiplyMatrixRFOrRFByMatrixRF(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyMatrixTensorOrRationalByMatrixTensor(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyAlgebraicNumberByAlgebraicNumber(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyRationalByRational(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyDoubleOrRationalByDoubleOrRational(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyCoxeterEltByCoxeterElt(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyCharacterByCharacter(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyCharSSLieAlgByCharSSLieAlg(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyAnyByUE(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyAnyByEltTensor(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyEllipticCurveElements(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyEllipticCurveElementsZmodP(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyRatOrPolyByWeightPoly(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyWeylGroupEltByWeightPoly(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerLieBracketExtractConstant(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLieBracketDistribute(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLieBracketRatOrUEWithRatOrUE(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLieBracketRatPolyOrEWAWithRatPolyOrEWA(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLieBracketSwapTermsIfNeeded(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLieBracketJacobiIdentityIfNeeded(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerDivideRatByRat(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDivideDoubleByDouble(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDivideAlgebraicNumberOrRatByAlgebraicNumberOrRat(
    Calculator& calculator, const Expression& input, Expression& output
  );

  static bool innerDividePolynomialModuloIntegerByPolynomialModuloInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerDivideRationalFunctionOrPolynomialOrRationalByRationalFunctionOrPolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerDivideEltZmodPorRatByEltZmodPorRat(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerPowerMatrixNumbersBySmallInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerMatrixExpressionsBySmallInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerMatrixNumbersByLargeIntegerIfPossible(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerPowerRationalByRationalOutputAlgebraic(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerRationalByRationalReducePrimeFactors(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerRationalByInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerPolynomialBySmallInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerAlgebraicNumberPolynomialBySmallInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerPolynomialModuloIntegerBySmallInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerPolynomialModPModuloPolynomialModPBySmallInteger(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerPowerAlgebraicNumberBySmallInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerRadicalAlgebraicNumberPositiveDefault(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerEWABySmallInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerElementUEbyRatOrPolyOrRF(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerDoubleOrRationalToDoubleOrRational(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerSequenceOrMatrixByT(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerPowerElementZmodPByInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerEllipticCurveRationalElementByInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerEllipticCurveZmodPElementByInteger(Calculator& calculator, const Expression& input, Expression& output);

  static bool binomialCoefficient(Calculator& calculator, const Expression& input, Expression& output);
  static bool setMinus(Calculator& calculator, const Expression& input, Expression& output);

  static bool tensorElementTensorByElementTensor(Calculator& calculator, const Expression& input, Expression& output);
  static bool tensorMatrixByMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool tensorMatrixByMatrixTensor(Calculator& calculator, const Expression& input, Expression& output);

  static bool polynomialModPModuloPolynomialModP(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

template <class theType>
bool CalculatorFunctionsBinaryOps::innerMultiplyTypeByType(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerMultiplyTypeByType");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.mergeContextsMyArumentsAndConvertThem<theType>(inputContextsMerged, &calculator.comments)) {
    return false;
  }
  theType result;
  result = inputContextsMerged[1].getValue<theType>();
  result *= inputContextsMerged[2].getValue<theType>();
  return output.assignValueWithContext(result, inputContextsMerged[1].getContext(), calculator);
}

template <class theType>
bool CalculatorFunctionsBinaryOps::innerAddTypeToType(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerAddTypeToType");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.mergeContextsMyArumentsAndConvertThem<theType>(inputContextsMerged, &calculator.comments)) {
    return false;
  }
  theType result;
  result = inputContextsMerged[1].getValue<theType>();
  result += inputContextsMerged[2].getValue<theType>();
  return output.assignValueWithContext(result, inputContextsMerged[1].getContext(), calculator);
}

template <class theType>
bool CalculatorFunctionsBinaryOps::innerDivideTypeByType(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerDivideTypeByType");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.mergeContextsMyArumentsAndConvertThem<theType>(inputContextsMerged, &calculator.comments)) {
    return false;
  }
  if (inputContextsMerged[2].getValue<theType>().isEqualToZero()) {
    return output.makeError("Division by zero. ", calculator);
  }
  theType result;
  result = inputContextsMerged[1].getValue<theType>();
  result /= inputContextsMerged[2].getValue<theType>();
  return output.assignValueWithContext(result, inputContextsMerged[1].getContext(), calculator);
}

template <class Coefficient>
bool CalculatorConversions::innerPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::innerPolynomial");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorConversions::functionPolynomial<Coefficient>(calculator, input[1], output);
}

template <class Coefficient, int MaximumPower, int MaximumVariables>
bool CalculatorConversions::functionPolynomialWithExponentLimit(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionPolynomialWithExponentLimit");
  RecursionDepthCounter theRecursionCounter(&calculator.recursionDepth);
  if (calculator.recursionDepth > calculator.maximumRecursionDepth) {
    return calculator << "Max recursion depth of " << calculator.maximumRecursionDepth
    << " exceeded while trying to evaluate polynomial expression (i.e. your polynomial expression is too large).";
  }
  if (input.isOfType<Polynomial<Coefficient> >()) {
    output = input;
    return true;
  }
  if (input.isOfType<Polynomial<Rational> >()) {
    Polynomial<Rational> thePolynomial;
    Polynomial<Coefficient> converted;
    input.isOfType(&thePolynomial);
    converted = thePolynomial;
    return output.assignValueWithContext(converted, input.getContext(), calculator);
  }
  if (input.isOfType<Coefficient>() || input.isOfType<Rational>()) {
    if (!input.convertInternally<Polynomial<Coefficient> >(output)) {
      global.fatal
      << "Failed to convert coefficient to polynomial. " << global.fatal;
    }
    return true;
  }
  Expression converted, computer, candidate;
  if (
    input.isListStartingWithAtom(calculator.opTimes()) ||
    input.isListStartingWithAtom(calculator.opPlus())
  ) {
    computer.reset(calculator, input.size());
    computer.addChildOnTop(input[0]);
    for (int i = 1; i < input.size(); i ++) {
      if (!CalculatorConversions::functionPolynomialWithExponentLimit<Coefficient, MaximumPower, MaximumVariables>(
        calculator, input[i], converted
      )) {
        return calculator << "<hr>Failed to extract polynomial from "
        << input[i].toString();
      }
      computer.addChildOnTop(converted);
    }
    if (input.isListStartingWithAtom(calculator.opTimes())) {
      if (!CalculatorFunctionsBinaryOps::innerMultiplyNumberOrPolynomialByNumberOrPolynomial(
        calculator, computer, candidate
      )) {
        return false;
      }
      if (candidate.getContext().numberOfVariables() > MaximumVariables && MaximumVariables >= 0) {
        return calculator << "Too many variables";
      }
      output = candidate;
      return true;
    }
    if (input.isListStartingWithAtom(calculator.opPlus())) {
      if (!CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial(
        calculator, computer, candidate
      )) {
        return false;
      }
      if (candidate.getContext().numberOfVariables() > MaximumVariables && MaximumVariables >= 0) {
        return calculator << "Too many variables";
      }
      output = candidate;
      return true;
    }
    global.fatal << "Error, this line of code should never be reached. " << global.fatal;
  }
  if (input.startsWith(calculator.opMinus(), 3)) {
    computer.reset(calculator, input.size());
    computer.addChildOnTop(input[0]);
    for (int i = 1; i < 3; i ++) {
      Expression summand = input[i];
      if (i == 2) {
        summand *= - 1;
      }
      if (!CalculatorConversions::functionPolynomialWithExponentLimit<Coefficient, MaximumPower, MaximumVariables>(
        calculator, summand, converted
      )) {
        return calculator << "<hr>Failed to extract polynomial from " << summand.toString();
      }
      computer.addChildOnTop(converted);
    }
    if (!CalculatorFunctionsBinaryOps::innerAddNumberOrPolynomialToNumberOrPolynomial(calculator, computer, candidate)) {
      return calculator << "Too many variables";
    }
    output = candidate;
    return true;
  }

  int power = - 1;
  if (input.startsWith(calculator.opThePower(), 3)) {
    if (input[2].isSmallInteger(&power)) {
      if (!CalculatorConversions::functionPolynomialWithExponentLimit<Coefficient, MaximumPower, MaximumVariables>(calculator, input[1], converted)) {
        return calculator
        << "<hr>Failed to extract polynomial from "
        << input[1].toString() << ".";
      }
      if (power > MaximumPower && MaximumPower >= 0) {
        return calculator
        << "Polynomial expression "
        << "has power larger than the maximum allowed: " << MaximumPower << ".";
      }
      if (converted.getContext().numberOfVariables() > MaximumVariables && MaximumVariables >= 0) {
        return calculator << "Too many variables";
      }
      Polynomial<Coefficient> resultP = converted.getValue<Polynomial<Coefficient> >();
      if (power < 0) {
        Coefficient theConst;
        if (!resultP.isConstant(&theConst)) {
          calculator << "<hr>Failed to extract polynomial from  "
          << input.toString() << " because the exponent was negative. "
          << "Please make sure that this is not a typo. "
          << "I am treating " << input.toString() << " as a single variable. ";
          Polynomial<Coefficient> monomial;
          monomial.makeMonomial(0, 1, 1);
          ExpressionContext theContext(calculator);
          theContext.makeOneVariable(input);
          return output.assignValueWithContext(monomial, theContext, calculator);
        }
        theConst.invert();
        power *= - 1;
        resultP = theConst;
      }
      resultP.raiseToPower(power, 1);
      return output.assignValueWithContext(resultP, converted.getContext(), calculator);
    }
  }
  Polynomial<Coefficient> monomial;
  monomial.makeMonomial(0, 1, 1);
  ExpressionContext theContext(calculator);
  theContext.makeOneVariable(input);
  return output.assignValueWithContext(monomial, theContext, calculator);
}

template <class Coefficient>
bool CalculatorConversions::functionRationalFunction(
  Calculator& calculator,
  const Expression& input,
  Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionRationalFunction");
  Expression intermediate(calculator);
  if (
    input.startsWith(calculator.opPlus(), 3) ||
    input.startsWith(calculator.opTimes(), 3) ||
    input.startsWith(calculator.opDivide(), 3)
  ) {
    Expression leftE, rightE;
    if (
      !CalculatorConversions::functionRationalFunction<Coefficient>(calculator, input[1], leftE) ||
      !CalculatorConversions::functionRationalFunction<Coefficient>(calculator, input[2], rightE)
    ) {
      return calculator << "<hr>Failed to convert " << input[1].toString()
      << " and " << input[2].toString() << " to rational function. ";
    }
    if (leftE.isError() || rightE.isError()) {
      return calculator << "<hr>Conversion of " << input[1].toString()
      << " and " << input[2].toString() << " returned error(s): "
      << leftE.toString() << " and " << rightE.toString();
    }
    intermediate.addChildOnTop(input[0]);
    intermediate.addChildOnTop(leftE);
    intermediate.addChildOnTop(rightE);
    if (input.startsWith(calculator.opPlus())) {
      return CalculatorFunctionsBinaryOps::innerAddRatOrPolyOrRFToRatOrPolyOrRF(
        calculator, intermediate, output
      );
    }
    if (input.startsWith(calculator.opTimes())) {
      return CalculatorFunctionsBinaryOps::innerMultiplyRatOrPolyOrRFByRatOrPolyOrRF(
        calculator, intermediate, output
      );
    }
    if (input.startsWith(calculator.opDivide())) {
      return CalculatorFunctionsBinaryOps::innerDivideRationalFunctionOrPolynomialOrRationalByRationalFunctionOrPolynomial(
        calculator, intermediate, output
      );
    }
    global.fatal << "This line of code should never be reached, something has gone wrong." << global.fatal;
  }
  int theSmallPower = - 1;
  if (input.startsWith(calculator.opThePower(), 3) ) {
    if (input[2].isSmallInteger(&theSmallPower)) {
      Expression leftE;
      if (!CalculatorConversions::functionRationalFunction<Coefficient>(calculator, input[1], leftE)) {
        return calculator << "<hr>CalculatorConversions::innerRationalFunction: failed to convert "
        << input[1].toString() << " to rational function. ";
      }
      if (leftE.isError()) {
        return calculator << "<hr>Conversion of " << input[1].toString() << " returned error: " << leftE.toString();
      }
      RationalFunction<Coefficient> rationalFunction;
      rationalFunction = leftE.getValue<RationalFunction<Coefficient> >();
      rationalFunction.raiseToPower(theSmallPower);
      return output.assignValueWithContext(rationalFunction, leftE.getContext(), calculator);
    }
    calculator << "<hr>Warning: failed to raise "
    << input[1].toString() << " to power " << input[2].toString()
    << ": failed to convert the power to small integer. "
    << "I am treating " << input.toString()
    << " as a single variable: please make sure that is what you want. ";
  }
  if (input.isOfType<RationalFunction<Coefficient> >()) {
    output = input;
    return true;
  }
  if (
    input.isOfType<Polynomial<Coefficient> >() ||
    input.isOfType<Coefficient>() ||
    input.isOfType<Rational>()
  ) {
    return input.convertInternally<RationalFunction<Coefficient> >(output);
  }
  if (input.isOfType<AlgebraicNumber>()) {
    Expression potentialOutput;
    if (input.convertInternally<RationalFunction<Coefficient> > (potentialOutput)) {
      output = potentialOutput;
      return true;
    }
  }
  ExpressionContext theContext(calculator);
  theContext.makeOneVariable(input);
  RationalFunction<Coefficient> rationalFunction;
  rationalFunction.makeOneLetterMonomial(0, Coefficient::oneStatic());
  return output.assignValueWithContext(rationalFunction, theContext, calculator);
}
#endif
