// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_calculator_inner_typed_functions_ALREADY_INCLUDED
#define header_calculator_inner_typed_functions_ALREADY_INCLUDED

#include "calculator_interface.h"

class CalculatorFunctionsBinaryOps {
public:
  template <class Type>
  static bool multiplyTypeByType(
    Calculator& calculator, const Expression& input, Expression& output
  );
  template <class Type>
  static bool addTypeToType(
    Calculator& calculator, const Expression& input, Expression& output
  );
  template <class Type>
  static bool divideTypeByType(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addDoubleOrRationalToDoubleOrRational(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool
  addRationalOrPolynomialOrElementWeylAlgebraToRattionalOrPolynomialOrElementWeylAlgebra
  (Calculator& calculator, const Expression& input, Expression& output);
  static bool addRationalOrPolynomialOrRationalFunctionToRationalFunction(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addUniversalEnvelopingAlgebraElementToAny(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addElementTensorToElementTensor(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addNumberOrPolynomialToNumberOrPolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addPlotToPlot(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addSequenceToSequence(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addMatrixToMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool directSumMatrixWithMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool augmentMatrixToTheRight(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool augmentMatrixBelow(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addMatrixRationalOrAlgebraicToMatrixRationalOrAlgebraic(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addMatrixRationalFractionsToMatrixRationalFractions(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addMatrixTensorToMatrixTensor(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addRationalToRational(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addStringToString(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addElementZModPOrRationalToElementZModPOrRational(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addPolynomialModuloIntegerToPolynomialModuloInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addPolynomialModuloIntegerToInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool
  addPolynomialModuloPolynomialModuloIntegerToPolynomialModuloPolynomialModuloInteger
  (Calculator& calculator, const Expression& input, Expression& output);
  static bool addAlgebraicNumberToAlgebraicNumber(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addWeightToWeight(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyEltHypOctByEltHypOct(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyRationalOrPolynomialOrRationalFunctionByRationalFraction(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool
  multiplyRationalOrPolynomialOrElementWeylAlgebraByRationalOrPolynomialOrElementWeylAlgebra
  (Calculator& calculator, const Expression& input, Expression& output);
  static bool multiplyPolynomialModPByPolynomialModP(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyNumberOrPolynomialByNumberOrPolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyPolynomialModPolynomialModPToPolynomialModPolynomialModP(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyLRObyLRO(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyLittlemannRootOperatorByLakshmibaiSeshadriPath(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyEltZmodPorRatByEltZmodPorRat(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyAnyScalarByMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyAnyScalarBySequence(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplySequenceByAnyScalar(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplySequenceByMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyMatrixByMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool makeMatrixProduct(
    Calculator& calculator,
    const Matrix<Expression>& left,
    const Matrix<Expression>& right,
    Expression& output
  );
  static bool multiplyMatrixBySequence(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyMatRatOrMatAlgByMatRatOrMatAlg(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyRatOrAlgebraicByMatRatOrMatAlg(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyMatrixRationalOrRationalByMatrixRational(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool
  multiplyMatrixRationalFractionOrRationalFractionByMatrixRationalFraction(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyMatrixTensorOrRationalByMatrixTensor(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyAlgebraicNumberByAlgebraicNumber(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyRationalByRational(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyDoubleOrRationalByDoubleOrRational(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyCoxeterEltByCoxeterElt(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerWeylGroupElementByInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyCharacterByCharacter(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyCharSSLieAlgByCharSSLieAlg(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyAnyByElementUniversalEnveloping(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyAnyByElementTensor(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyEllipticCurveElements(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyEllipticCurveElementsZmodP(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyRationalOrPolynomialByWeightPolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplyWeylGroupEltByWeightPoly(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool lieBracketOfMatrices(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool lieBracketExtractConstant(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool lieBracketDistribute(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool lieBracketRatOrUEWithRatOrUE(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool lieBracketRatPolyOrEWAWithRatPolyOrEWA(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool lieBracketSwapTermsIfNeeded(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool lieBracketJacobiIdentityIfNeeded(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool divideRationalByRational(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool divideDoubleByDouble(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool divideAlgebraicNumberOrRationalByAlgebraicNumberOrRational(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool dividePolynomialModuloIntegerByPolynomialModuloInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool
  divideRationalFractionOrPolynomialOrRationalByRationalFractionOrPolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool divideEltZmodPorRatByEltZmodPorRat(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerMatrixBuiltInBySmallInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerMatrixExpressionsBySmallInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerMatrixNumbersByLargeIntegerIfPossible(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerRationalByRationalOutputAlgebraic(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerRationalByRationalReducePrimeFactors(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerRationalByInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerPolynomialBySmallInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerAlgebraicNumberPolynomialBySmallInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerRationalFractionBySmallInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerPolynomialModuloIntegerBySmallInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerPolynomialModPModuloPolynomialModPBySmallInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerAlgebraicNumberBySmallInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool radicalAlgebraicNumberPositiveDefault(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerElementWeylAlgebraBySmallInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool
  powerElementUniversalEnvelopingByRationalOrPolynomialOrRationalFraction(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerDoubleOrRationalToDoubleOrRational(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerSequenceOrMatrixByT(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerElementZmodPByInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerEllipticCurveRationalElementByInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerEllipticCurveZmodPElementByInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool binomialCoefficient(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool setMinus(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool tensorElementTensorByElementTensor(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool tensorMatrixByMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool tensorMatrixByMatrixTensor(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool polynomialModPModuloPolynomialModP(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

template <class Type>
bool CalculatorFunctionsBinaryOps::multiplyTypeByType(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::multiplyTypeByType");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (
    !input.mergeContextsMyArumentsAndConvertThem<Type>(
      inputContextsMerged, nullptr
    )
  ) {
    return false;
  }
  Type result;
  result = inputContextsMerged[1].getValue<Type>();
  result *= inputContextsMerged[2].getValue<Type>();
  return
  output.assignValueWithContext(
    calculator, result, inputContextsMerged[1].getContext()
  );
}

template <class Type>
bool CalculatorFunctionsBinaryOps::addTypeToType(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::addTypeToType");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (
    !input.mergeContextsMyArumentsAndConvertThem<Type>(
      inputContextsMerged, nullptr
    )
  ) {
    return false;
  }
  Type result;
  result = inputContextsMerged[1].getValue<Type>();
  result += inputContextsMerged[2].getValue<Type>();
  return
  output.assignValueWithContext(
    calculator, result, inputContextsMerged[1].getContext()
  );
}

template <class Type>
bool CalculatorFunctionsBinaryOps::divideTypeByType(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::divideTypeByType");
  if (input.size() != 3) {
    return false;
  }
  Expression inputContextsMerged;
  if (
    !input.mergeContextsMyArumentsAndConvertThem<Type>(
      inputContextsMerged, nullptr
    )
  ) {
    return false;
  }
  if (inputContextsMerged[2].getValue<Type>().isEqualToZero()) {
    return output.assignError(calculator, "Division by zero. ");
  }
  Type result;
  result = inputContextsMerged[1].getValue<Type>();
  result /= inputContextsMerged[2].getValue<Type>();
  return
  output.assignValueWithContext(
    calculator, result, inputContextsMerged[1].getContext()
  );
}

template <class Coefficient>
bool CalculatorConversions::getPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::getPolynomial");
  if (input.size() != 2) {
    return false;
  }
  WithContext<Polynomial<Coefficient> > outputWithContext;
  if (
    !CalculatorConversions::functionPolynomial<Coefficient>(
      calculator, input[1], outputWithContext, - 1, - 1, false
    )
  ) {
    return false;
  }
  return
  output.assignValueWithContext(
    calculator, outputWithContext.content, outputWithContext.context
  );
}

template <class Coefficient>
bool CalculatorConversions::extractPolynomialFromSumDifferenceOrProduct(
  Calculator& calculator,
  const Expression& input,
  WithContext<Polynomial<Coefficient> >& output,
  int maximumVariables,
  int maximumPowerToExpand,
  bool acceptNonPositiveOrNonIntegerPowers
) {
  STACK_TRACE(
    "CalculatorConversions::extractPolynomialSumDifferenceOrProduct"
  );
  if (
    !input.isListStartingWithAtom(calculator.opTimes()) &&
    !input.isListStartingWithAtom(calculator.opPlus()) &&
    !input.isListStartingWithAtom(calculator.opMinus())
  ) {
    global.fatal
    << "Unexpected input for extractPolynomialSumDifferenceOrProduct. "
    << global.fatal;
  }
  WithContext<Polynomial<Coefficient> > converted;
  if (input.isListStartingWithAtom(calculator.opTimes())) {
    output.content.makeOne();
  } else {
    output.content.makeZero();
  }
  for (int i = 1; i < input.size(); i ++) {
    if (
      !CalculatorConversions::functionPolynomial<Coefficient>(
        calculator,
        input[i],
        converted,
        maximumVariables,
        maximumPowerToExpand,
        acceptNonPositiveOrNonIntegerPowers
      )
    ) {
      return
      calculator
      << "<hr>Failed to extract polynomial from "
      << input[i].toString()
      << ". ";
    }
    if (
      !output.mergeContexts(output, converted, &calculator.comments)
    ) {
      return
      calculator
      << "Failed to merge contexts in polynomial conversion. ";
    }
    if (
      maximumVariables >= 0 &&
      output.context.numberOfVariables() > maximumVariables
    ) {
      return
      calculator
      << "Maximum number of variables exceeded: "
      << maximumVariables
      << ". ";
    }
    if (input.isListStartingWithAtom(calculator.opPlus())) {
      output.content += converted.content;
    } else if (input.isListStartingWithAtom(calculator.opMinus())) {
      if (i == 1 && input.size() == 2) {
        output.content -= converted.content;
      } else if (i == 1 && input.size() > 2) {
        output.content += converted.content;
      } else {
        output.content -= converted.content;
      }
    } else if (input.isListStartingWithAtom(calculator.opTimes())) {
      output.content *= converted.content;
    } else {
      global.fatal
      << "While extracting input polynomial, got unexpected input type: "
      << input.toString()
      << global.fatal;
    }
  }
  return true;
}

template <class Coefficient>
bool CalculatorConversions::extractPolynomialFromPower(
  Calculator& calculator,
  const Expression& input,
  WithContext<Polynomial<Coefficient> >& output,
  int maximumVariables,
  int maximumPowerToExpand,
  bool acceptNonPositiveOrNonIntegerPowers
) {
  STACK_TRACE("CalculatorConversions::extractPolynomialFromPower");
  if (!input.startsWith(calculator.opPower(), 3)) {
    global.fatal
    << "Incorrect call of extractPolynomialFromPower with input: "
    << input.toString()
    << global.fatal;
  }
  int power = - 1;
  if (!input[2].isSmallInteger(&power)) {
    if (acceptNonPositiveOrNonIntegerPowers) {
      return
      CalculatorConversions::extractPolynomialMakeAtom(
        calculator, input, output
      );
    } else {
      return
      calculator
      << "Expression: "
      << input.toString()
      << " has non-integer exponent. ";
    }
  }
  if (
    !CalculatorConversions::functionPolynomial<Coefficient>(
      calculator,
      input[1],
      output,
      maximumVariables,
      maximumPowerToExpand,
      acceptNonPositiveOrNonIntegerPowers
    )
  ) {
    return
    calculator
    << "<hr>Failed to extract polynomial from "
    << input[1].toString()
    << ". ";
  }
  if (maximumPowerToExpand >= 0 && power > maximumPowerToExpand) {
    return
    calculator
    << "Polynomial expression "
    << "has power larger than the maximum allowed: "
    << maximumPowerToExpand
    << ". ";
  }
  if (power < 0) {
    Coefficient inverted;
    Polynomial<Coefficient>& content = output.content;
    if (content.isConstant(&inverted)) {
      inverted.invert();
      power *= - 1;
      content = inverted;
      return true;
    }
    if (content.size() == 1 && acceptNonPositiveOrNonIntegerPowers) {
      MonomialPolynomial monomial = content.monomials[0];
      inverted = content.coefficients[0];
      monomial.invert();
      inverted.invert();
      content.makeZero();
      content.addMonomial(monomial, inverted);
      return true;
    }
    return
    calculator
    << "<hr>Failed to extract polynomial from "
    << input.toString()
    << " because the exponent was negative. "
    << "Please make sure that this is not a typo. ";
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
  int maximumPowerToExpand,
  bool acceptNonPositiveOrNonIntegerPowers
) {
  STACK_TRACE("CalculatorConversions::functionPolynomial");
  RecursionDepthCounter recursionCounter(&calculator.recursionDepth);
  if (calculator.recursionDepth > calculator.maximumRecursionDepth) {
    return
    calculator
    << "Max recursion depth of "
    << calculator.maximumRecursionDepth
    << " exceeded while trying to evaluate polynomial "
    << "expression (i.e. your polynomial expression is too large).";
  }
  bool noConversionDesired = false;
  if (
    CalculatorConversions::convertWithoutComputation(
      calculator, input, output, &noConversionDesired
    )
  ) {
    return true;
  }
  if (noConversionDesired){
    return false;
  }
  WithContext<Polynomial<Coefficient> > converted, candidate;
  if (
    input.isListStartingWithAtom(calculator.opTimes()) ||
    input.isListStartingWithAtom(calculator.opPlus()) ||
    input.isListStartingWithAtom(calculator.opMinus())
  ) {
    return
    CalculatorConversions::extractPolynomialFromSumDifferenceOrProduct(
      calculator,
      input,
      output,
      maximumVariables,
      maximumPowerToExpand,
      acceptNonPositiveOrNonIntegerPowers
    );
  }
  if (input.startsWith(calculator.opPower(), 3)) {
    return
    CalculatorConversions::extractPolynomialFromPower(
      calculator,
      input,
      output,
      maximumVariables,
      maximumPowerToExpand,
      acceptNonPositiveOrNonIntegerPowers
    );
  }
  return
  CalculatorConversions::extractPolynomialMakeAtom(calculator, input, output);
}

template <class Coefficient>
bool CalculatorConversions::extractPolynomialMakeAtom(
  Calculator& calculator,
  const Expression& input,
  WithContext<Polynomial<Coefficient> >& output
) {
  output.context.initialize(calculator);
  output.context.makeOneVariable(input);
  Polynomial<Coefficient> monomial;
  monomial.makeMonomial(0, 1, 1);
  output.content = monomial;
  return true;
}

template <class Coefficient>
bool CalculatorConversions::functionRationalFraction(
  Calculator& calculator,
  const Expression& input,
  WithContext<RationalFraction<Coefficient> >& output,
  bool allowNonIntegerPowers
) {
  STACK_TRACE("CalculatorConversions::functionRationalFraction");
  if (
    CalculatorConversions::convertWithoutComputation(
      calculator, input, output
    )
  ) {
    return true;
  }
  if (
    input.startsWith(calculator.opPlus(), 3) ||
    input.startsWith(calculator.opTimes(), 3) ||
    input.startsWith(calculator.opDivide(), 3)
  ) {
    WithContext<RationalFraction<Coefficient> > left, right;
    if (
      !CalculatorConversions::functionRationalFraction<Coefficient>(
        calculator, input[1], left, allowNonIntegerPowers
      )
    ) {
      return
      calculator
      << "<hr>Failed to convert "
      << input[1].toString()
      << " to rational function. ";
    }
    if (
      !CalculatorConversions::functionRationalFraction<Coefficient>(
        calculator, input[2], right, allowNonIntegerPowers
      )
    ) {
      return
      calculator
      << "<hr>Failed to convert "
      << input[2].toString()
      << " to rational function. ";
    }
    if (!left.mergeContexts(left, right, &calculator.comments)) {
      return false;
    }
    output.content = left.content;
    output.context = left.context;
    if (input.startsWith(calculator.opPlus())) {
      output.content += right.content;
      return true;
    }
    if (input.startsWith(calculator.opTimes())) {
      output.content *= right.content;
      return true;
    }
    if (input.startsWith(calculator.opDivide())) {
      if (right.content.isEqualToZero()) {
        return
        calculator
        << "Division by zero in rational fraction conversion";
      }
      output.content /= right.content;
      return true;
    }
    global.fatal
    << "This line of code should never be reached, something has gone wrong."
    << global.fatal;
  }
  int smallPower = - 1;
  if (input.startsWith(calculator.opPower(), 3)) {
    if (input[2].isSmallInteger(&smallPower)) {
      WithContext<RationalFraction<Coefficient> > base;
      if (
        !CalculatorConversions::functionRationalFraction<Coefficient>(
          calculator, input[1], base, allowNonIntegerPowers
        )
      ) {
        return
        calculator
        << "<hr>CalculatorConversions::functionRationalFraction: "
        << "failed to convert "
        << input[1].toString()
        << " to rational fraction. ";
      }
      output.context = base.context;
      output.content = base.content;
      output.content.raiseToPower(smallPower);
      return true;
    }
    if (!allowNonIntegerPowers) {
      return
      calculator
      << "<hr>Warning: failed to raise "
      << input[1].toString()
      << " to power "
      << input[2].toString();
    }
  }
  output.context.initialize(calculator);
  output.context.makeOneVariable(input);
  output.content.makeOneLetterMonomial(0, Coefficient::oneStatic());
  return true;
}

#endif // header_calculator_inner_typed_functions_ALREADY_INCLUDED
