// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader6_1InnerTypedFns_h_already_included
#define vpfHeader6_1InnerTypedFns_h_already_included

#include "calculator_interface.h"

class CalculatorFunctionsBinaryOps {
public:
  template <class Type>
  static bool multiplyTypeByType(Calculator& calculator, const Expression& input, Expression& output);
  template <class Type>
  static bool addTypeToType(Calculator& calculator, const Expression& input, Expression& output);
  template <class Type>
  static bool divideTypeByType(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerAddDoubleOrRationalToDoubleOrRational(Calculator& calculator, const Expression& input, Expression& output);
  static bool addRationalOrPolynomialOrElementWeylAlgebraToRattionalOrPolynomialOrElementWeylAlgebra(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addRationalOrPolynomialOrRationalFunctionToRationalFunction(Calculator& calculator, const Expression& input, Expression& output);
  static bool addUniversalEnvelopingAlgebraElementToAny(Calculator& calculator, const Expression& input, Expression& output);
  static bool addElementTensorToElementTensor(Calculator& calculator, const Expression& input, Expression& output);
  static bool addNumberOrPolynomialToNumberOrPolynomial(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddPlotToPlot(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddSequenceToSequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddMatrixToMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDirectSumMatrixWithMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAugmentMatrixToTheRight(Calculator& calculator, const Expression& input, Expression& output);
  static bool augmentMatrixBelow(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerAddMatrixRationalOrAlgebraicToMatrixRationalOrAlgebraic(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addMatrixRationalFractionsToMatrixRationalFractions(Calculator& calculator, const Expression& input, Expression& output);
  static bool addMatrixTensorToMatrixTensor(Calculator& calculator, const Expression& input, Expression& output);
  static bool addRationalToRational(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddStringToString(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAddElementZModPOrRationalToElementZModPOrRational(Calculator& calculator, const Expression& input, Expression& output);
  static bool addPolynomialModuloIntegerToPolynomialModuloInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addPolynomialModuloIntegerToInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addPolynomialModuloPolynomialModuloIntegerToPolynomialModuloPolynomialModuloInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );

  static bool addAlgebraicNumberToAlgebraicNumber(Calculator& calculator, const Expression& input, Expression& output);
  static bool addWeightToWeight(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerMultiplyEltHypOctByEltHypOct(Calculator& calculator, const Expression& input, Expression& output);

  static bool multiplyRationalOrPolynomialOrRationalFunctionByRationalFraction(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyRationalOrPolynomialOrElementWeylAlgebraByRationalOrPolynomialOrElementWeylAlgebra(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyPolynomialModPByPolynomialModP(Calculator& calculator, const Expression& input, Expression& output);
  static bool multiplyNumberOrPolynomialByNumberOrPolynomial(Calculator& calculator, const Expression& input, Expression& output);
  static bool multiplyPolynomialModPolynomialModPToPolynomialModPolynomialModP(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerMultiplyLRObyLRO(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyLRObyLSPath(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyEltZmodPorRatByEltZmodPorRat(Calculator& calculator, const Expression& input, Expression& output);
  static bool multiplyAnyScalarByMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyAnyScalarBySequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplySequenceByAnyScalar(Calculator& calculator, const Expression& input, Expression& output);
  static bool multiplySequenceByMatrix(Calculator& calculator, const Expression& input, Expression& output);

  static bool multiplyMatrixByMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool makeMatrixProduct(
    Calculator& calculator,
    const Matrix<Expression>& left,
    const Matrix<Expression>& right,
    Expression& output
  );
  static bool multiplyMatrixBySequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyMatRatOrMatAlgByMatRatOrMatAlg(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyRatOrAlgebraicByMatRatOrMatAlg(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyMatrixRationalOrRationalByMatrixRational(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyMatrixRationalFractionOrRationalFractionByMatrixRationalFraction(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerMultiplyMatrixTensorOrRationalByMatrixTensor(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyAlgebraicNumberByAlgebraicNumber(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyRationalByRational(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyDoubleOrRationalByDoubleOrRational(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyCoxeterEltByCoxeterElt(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerWeylGroupElementByInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyCharacterByCharacter(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyCharSSLieAlgByCharSSLieAlg(Calculator& calculator, const Expression& input, Expression& output);
  static bool multiplyAnyByElementUniversalEnveloping(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyAnyByEltTensor(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyEllipticCurveElements(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyEllipticCurveElementsZmodP(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyRatOrPolyByWeightPoly(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMultiplyWeylGroupEltByWeightPoly(Calculator& calculator, const Expression& input, Expression& output);

  static bool lieBracketOfMatrices(Calculator& calculator, const Expression& input, Expression& output);
  static bool lieBracketExtractConstant(Calculator& calculator, const Expression& input, Expression& output);
  static bool lieBracketDistribute(Calculator& calculator, const Expression& input, Expression& output);
  static bool lieBracketRatOrUEWithRatOrUE(Calculator& calculator, const Expression& input, Expression& output);
  static bool lieBracketRatPolyOrEWAWithRatPolyOrEWA(Calculator& calculator, const Expression& input, Expression& output);
  static bool lieBracketSwapTermsIfNeeded(Calculator& calculator, const Expression& input, Expression& output);
  static bool lieBracketJacobiIdentityIfNeeded(Calculator& calculator, const Expression& input, Expression& output);

  static bool divideRationalByRational(Calculator& calculator, const Expression& input, Expression& output);
  static bool divideDoubleByDouble(Calculator& calculator, const Expression& input, Expression& output);
  static bool divideAlgebraicNumberOrRationalByAlgebraicNumberOrRational(
    Calculator& calculator, const Expression& input, Expression& output
  );

  static bool dividePolynomialModuloIntegerByPolynomialModuloInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool divideEltZmodPorRatByEltZmodPorRat(Calculator& calculator, const Expression& input, Expression& output);

  static bool powerMatrixBuiltInBySmallInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerMatrixExpressionsBySmallInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool powerMatrixNumbersByLargeIntegerIfPossible(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerPowerRationalByRationalOutputAlgebraic(Calculator& calculator, const Expression& input, Expression& output);
  static bool powerRationalByRationalReducePrimeFactors(Calculator& calculator, const Expression& input, Expression& output);
  static bool powerRationalByInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool powerPolynomialBySmallInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool powerAlgebraicNumberPolynomialBySmallInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool powerRationalFractionBySmallInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool powerPolynomialModuloIntegerBySmallInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool powerPolynomialModPModuloPolynomialModPBySmallInteger(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerPowerAlgebraicNumberBySmallInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerRadicalAlgebraicNumberPositiveDefault(Calculator& calculator, const Expression& input, Expression& output);
  static bool powerElementWeylAlgebraBySmallInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool powerElementUniversalEnvelopingByRationalOrPolynomialOrRationalFraction(Calculator& calculator, const Expression& input, Expression& output);
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

template <class Type>
bool CalculatorFunctionsBinaryOps::multiplyTypeByType(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::multiplyTypeByType");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.mergeContextsMyArumentsAndConvertThem<Type>(inputContextsMerged, &calculator.comments)) {
    return false;
  }
  Type result;
  result = inputContextsMerged[1].getValue<Type>();
  result *= inputContextsMerged[2].getValue<Type>();
  return output.assignValueWithContextOLD(result, inputContextsMerged[1].getContext(), calculator);
}

template <class Type>
bool CalculatorFunctionsBinaryOps::addTypeToType(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::addTypeToType");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.mergeContextsMyArumentsAndConvertThem<Type>(inputContextsMerged, &calculator.comments)) {
    return false;
  }
  Type result;
  result = inputContextsMerged[1].getValue<Type>();
  result += inputContextsMerged[2].getValue<Type>();
  return output.assignValueWithContextOLD(result, inputContextsMerged[1].getContext(), calculator);
}

template <class Type>
bool CalculatorFunctionsBinaryOps::divideTypeByType(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::divideTypeByType");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (!input.mergeContextsMyArumentsAndConvertThem<Type>(inputContextsMerged, &calculator.comments)) {
    return false;
  }
  if (inputContextsMerged[2].getValue<Type>().isEqualToZero()) {
    return output.assignError(calculator, "Division by zero. ");
  }
  Type result;
  result = inputContextsMerged[1].getValue<Type>();
  result /= inputContextsMerged[2].getValue<Type>();
  return output.assignValueWithContextOLD(result, inputContextsMerged[1].getContext(), calculator);
}

template <class Coefficient>
bool CalculatorConversions::getPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::getPolynomial");
  if (input.size() != 2) {
    return false;
  }
  WithContext<Polynomial<Coefficient> > outputWithContext;
  if (!CalculatorConversions::functionPolynomial<Coefficient>(
    calculator, input[1], outputWithContext, - 1, - 1
  )) {
    return false;
  }
  return output.assignValueWithContext(
    calculator,
    outputWithContext.content,
    outputWithContext.context
  );
}

template <class Coefficient>
bool CalculatorConversions::extractPolynomialFromSumDifferenceOrProduct(
  Calculator& calculator,
  const Expression& input,
  WithContext<Polynomial<Coefficient> >& output,
  int maximumVariables,
  int maximumPowerToExpand
) {
  MacroRegisterFunctionWithName("CalculatorConversions::extractPolynomialSumDifferenceOrProduct");
  if (
    !input.isListStartingWithAtom(calculator.opTimes()) &&
    !input.isListStartingWithAtom(calculator.opPlus()) &&
    !input.isListStartingWithAtom(calculator.opMinus())
  ) {
    global.fatal << "Unexpected input for extractPolynomialSumDifferenceOrProduct. " << global.fatal;
  }
  WithContext<Polynomial<Coefficient> > converted;
  if (input.isListStartingWithAtom(calculator.opTimes())) {
    output.content.makeOne();
  } else {
    output.content.makeZero();
  }
  output.content.makeZero();
  for (int i = 1; i < input.size(); i ++) {
    if (!CalculatorConversions::functionPolynomial<Coefficient>(
      calculator,
      input[i],
      converted,
      maximumVariables,
      maximumPowerToExpand
    )) {
      return calculator << "<hr>Failed to extract polynomial from "
      << input[i].toString();
    }
    if (!output.mergeContexts(output, converted, &calculator.comments)) {
      return calculator << "Failed to merge contexts in polynomial conversion. ";
    }
    if (
      maximumVariables >= 0 &&
      output.context.numberOfVariables() > maximumVariables
    ) {
      return calculator << "Maximum number of variables exceeded: " << maximumVariables << ". ";
    }
    if (input.isListStartingWithAtom(calculator.opPlus())) {
      output.content += converted.content;
    } else if (input.isListStartingWithAtom(calculator.opMinus())){
      if (i == 1 && input.size() == 2) {
        output.content -= converted.content;
      } else if (i == 1 && input.size() > 2) {
        output.content += converted.content;
      } else {
        output.content -= converted.content;
      }
    } else {
      output.content *= converted.content;
    }
    return true;
  }
  return true;
}

template <class Coefficient>
bool CalculatorConversions::extractPolynomialFromPower(
  Calculator& calculator,
  const Expression& input,
  WithContext<Polynomial<Coefficient> >& output,
  int maximumVariables,
  int maximumPowerToExpand
) {
  MacroRegisterFunctionWithName("CalculatorConversions::extractPolynomialFromPower");
  if (!input.startsWith(calculator.opPower(), 3)) {
    global.fatal << "Incorrect call of extractPolynomialFromPower with input: "
    << input.toString() << global.fatal;
  }
  int power = - 1;
  if (!input[2].isSmallInteger(&power)) {
    return calculator << "Expression: " << input.toString() << " has non-integer exponent. ";
  }
  if (!CalculatorConversions::functionPolynomial<Coefficient>(
    calculator, input[1], output, maximumVariables, maximumPowerToExpand
  )) {
    return calculator
    << "<hr>Failed to extract polynomial from "
    << input[1].toString() << ".";
  }
  if (maximumPowerToExpand >= 0 && power > maximumPowerToExpand) {
    return calculator
    << "Polynomial expression "
    << "has power larger than the maximum allowed: " << maximumPowerToExpand << ".";
  }
  if (power < 0) {
    Coefficient inverted;
    if (!output.content.isConstant(&inverted)) {
      return calculator << "<hr>Failed to extract polynomial from "
      << input.toString() << " because the exponent was negative. "
      << "Please make sure that this is not a typo. ";
    }
    inverted.invert();
    power *= - 1;
    output.content = inverted;
  }
  output.content.raiseToPower(power, 1);
  return true;
}

template <class Coefficient>
bool CalculatorConversions::functionPolynomial(
  Calculator& calculator,
  const Expression& input,
  WithContext<Polynomial<Coefficient> >& output,
  int maximumVariables,
  int maximumPowerToExpand
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionPolynomial");
  RecursionDepthCounter recursionCounter(&calculator.recursionDepth);
  if (calculator.recursionDepth > calculator.maximumRecursionDepth) {
    return calculator << "Max recursion depth of "
    << calculator.maximumRecursionDepth
    << " exceeded while trying to evaluate polynomial "
    << "expression (i.e. your polynomial expression is too large).";
  }
  if (input.isOfTypeWithContext<Polynomial<Coefficient> >(&output)) {
    return true;
  }
  WithContext<Polynomial<Rational> > polynomialRational;
  if (input.isOfTypeWithContext<Polynomial<Rational> >(&polynomialRational)) {
    output.content = polynomialRational.content;
    output.context = polynomialRational.context;
    return true;
  }
  WithContext<Coefficient> coefficient;
  if (input.isOfTypeWithContext(&coefficient)) {
    output.content = coefficient.content;
    output.context = coefficient.context;
    return true;
  }
  WithContext<Rational> coefficientRational;
  if (input.isOfTypeWithContext(&coefficientRational)) {
    output.content = coefficientRational.content;
    output.context = coefficientRational.context;
    return true;
  }
  WithContext<Polynomial<Coefficient> > converted, candidate;
  if (
    input.isListStartingWithAtom(calculator.opTimes()) ||
    input.isListStartingWithAtom(calculator.opPlus()) ||
    input.isListStartingWithAtom(calculator.opMinus())
  ) {
    return CalculatorConversions::extractPolynomialFromSumDifferenceOrProduct(
      calculator, input, output, maximumVariables, maximumPowerToExpand
    );
  }
  if (input.startsWith(calculator.opPower(), 3)) {
    return CalculatorConversions::extractPolynomialFromPower(
      calculator, input, output, maximumVariables, maximumPowerToExpand
    );
  }
  output.context.makeOneVariable(input);
  Polynomial<Coefficient> monomial;
  monomial.makeMonomial(0, 1, 1);
  output.content = monomial;
  return true;
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
      return CalculatorFunctionsBinaryOps::addRationalOrPolynomialOrRationalFunctionToRationalFunction(
        calculator, intermediate, output
      );
    }
    if (input.startsWith(calculator.opTimes())) {
      return CalculatorFunctionsBinaryOps::multiplyRationalOrPolynomialOrRationalFunctionByRationalFraction(
        calculator, intermediate, output
      );
    }
    if (input.startsWith(calculator.opDivide())) {
      return CalculatorFunctionsBinaryOps::divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial(
        calculator, intermediate, output
      );
    }
    global.fatal << "This line of code should never be reached, something has gone wrong." << global.fatal;
  }
  int theSmallPower = - 1;
  if (input.startsWith(calculator.opPower(), 3) ) {
    if (input[2].isSmallInteger(&theSmallPower)) {
      Expression leftE;
      if (!CalculatorConversions::functionRationalFunction<Coefficient>(calculator, input[1], leftE)) {
        return calculator << "<hr>CalculatorConversions::innerRationalFunction: failed to convert "
        << input[1].toString() << " to rational function. ";
      }
      if (leftE.isError()) {
        return calculator << "<hr>Conversion of " << input[1].toString() << " returned error: " << leftE.toString();
      }
      RationalFraction<Coefficient> rationalFunction;
      rationalFunction = leftE.getValue<RationalFraction<Coefficient> >();
      rationalFunction.raiseToPower(theSmallPower);
      return output.assignValueWithContextOLD(rationalFunction, leftE.getContext(), calculator);
    }
    calculator << "<hr>Warning: failed to raise "
    << input[1].toString() << " to power " << input[2].toString()
    << ": failed to convert the power to small integer. "
    << "I am treating " << input.toString()
    << " as a single variable: please make sure that is what you want. ";
  }
  if (input.isOfType<RationalFraction<Coefficient> >()) {
    output = input;
    return true;
  }
  WithContext<RationalFraction<Coefficient> > potentialOutput;
  if (
    input.isOfType<Polynomial<Coefficient> >() ||
    input.isOfType<Coefficient>() ||
    input.isOfType<Rational>()
  ) {
    if (!CalculatorConversions::convertWithoutComputation(calculator, input, potentialOutput)) {
      return false;
    }
    return output.assignWithContext(calculator, potentialOutput);
  }
  if (input.isOfType<AlgebraicNumber>()) {
    if (CalculatorConversions::convertWithoutComputation(calculator, input, potentialOutput)) {
      return output.assignWithContext(calculator, potentialOutput);
    }
  }
  ExpressionContext context(calculator);
  context.makeOneVariable(input);
  RationalFraction<Coefficient> rationalFunction;
  rationalFunction.makeOneLetterMonomial(0, Coefficient::oneStatic());
  return output.assignValueWithContext(calculator, rationalFunction, context);
}
#endif
