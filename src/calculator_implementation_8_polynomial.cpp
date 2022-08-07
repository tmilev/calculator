// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "calculator_inner_typed_functions.h"
#include "calculator_functions_polynomial.h"
#include "math_rational_function_implementation.h"
#include "math_extra_polynomial_factorization.h"
#include "math_extra_algebraic_numbers.h"
#include "math_general_implementation.h"

template < >
bool CalculatorConversions::getPolynomial<Rational>(
  Calculator& calculator, const Expression& input, Expression& output
);
bool CalculatorFunctionsPolynomial::polynomialDivisionVerboseGrLex(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return
  CalculatorFunctionsPolynomial::polynomialDivisionVerbose(
    calculator,
    input,
    output,
    &MonomialPolynomial::orderDegreeThenLeftLargerWins()
  );
}

bool CalculatorFunctionsPolynomial::
polynomialDivisionVerboseGradedReverseLexicographic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  List<MonomialPolynomial>::Comparator order(
    MonomialPolynomial::greaterThan_totalDegree_rightSmallerWins
  );
  return
  CalculatorFunctionsPolynomial::polynomialDivisionVerbose(
    calculator, input, output, &order
  );
}

bool CalculatorFunctionsPolynomial::polynomialDivisionVerboseLexicographic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsPolynomial::polynomialDivisionVerboseLexicographic"
  );
  List<MonomialPolynomial>::Comparator order(
    MonomialPolynomial::greaterThan_leftLargerWins
  );
  return
  CalculatorFunctionsPolynomial::polynomialDivisionVerbose(
    calculator, input, output, &order
  );
}

bool CalculatorFunctionsPolynomial::
polynomialDivisionVerboseLexicographicOpposite(
  Calculator& calculator, const Expression& input, Expression& output
) {
  List<MonomialPolynomial>::Comparator order(
    MonomialPolynomial::greaterThan_rightLargerWins
  );
  return
  CalculatorFunctionsPolynomial::polynomialDivisionVerbose(
    calculator, input, output, &order
  );
}

template <class Coefficient>
bool CalculatorFunctionsPolynomial::polynomialDivisionVerbosePart2(
  Calculator& calculator,
  List<Polynomial<Coefficient> >& input,
  ExpressionContext& context,
  Expression& output,
  List<MonomialPolynomial>::Comparator* monomialOrder
) {
  GroebnerBasisComputation<Coefficient> computation;
  context.getFormat(computation.format);
  computation.format.flagUseLatex = true;
  computation.format.flagUseFrac = true;
  computation.flagDoLogDivision = true;
  computation.flagStoreQuotients = true;
  computation.polynomialOrder.monomialOrder = *monomialOrder;
  for (int i = 1; i < input.size; i ++) {
    if (input[i].isEqualToZero()) {
      return output.assignError(calculator, "Division by zero.");
    }
    computation.addBasisElementNoReduction(input[i]);
  }
  if (monomialOrder != nullptr) {
    computation.polynomialOrder.monomialOrder = *monomialOrder;
  }
  computation.remainderDivisionByBasis(
    input[0], computation.remainderDivision, - 1
  );
  std::stringstream latexOutput;
  latexOutput
  << "<br>In latex: <br>"
  << "\\documentclass{article}\\usepackage{longtable}"
  << "\\usepackage{xcolor}\\usepackage{multicol}"
  << "\\begin{document} "
  << computation.divisionReport.getElement().getDivisionStringLaTeX()
  << "\\end{document}";
  std::stringstream out;
  out << computation.divisionReport.getElement().getDivisionStringHtml();
  out << latexOutput.str();
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsPolynomial::polynomialDivisionRemainderAlgebraic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsPolynomial::polynomialDivisionRemainderAlgebraic"
  );
  if (
    !CalculatorFunctionsPolynomial::polynomialDivisionRemainderBuiltIn<
      AlgebraicNumber
    >(calculator, input, output)
  ) {
    return false;
  }
  Expression result = output;
  output.reset(calculator);
  output.addChildAtomOnTop("MakeExpression");
  return output.addChildOnTop(result);
}

bool CalculatorFunctionsPolynomial::polynomialDivisionVerbose(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  List<MonomialPolynomial>::Comparator* monomialOrder
) {
  STACK_TRACE("CalculatorFunctionsPolynomial::polynomialDivisionVerbose");
  ExpressionContext contextAlgebraic(calculator);
  Vector<Polynomial<AlgebraicNumber> > polynomialsRational;
  if (
    CalculatorConversions::getListPolynomialVariableLabelsLexicographic(
      calculator, input, polynomialsRational, contextAlgebraic
    )
  ) {
    return
    CalculatorFunctionsPolynomial::polynomialDivisionVerbosePart2(
      calculator,
      polynomialsRational,
      contextAlgebraic,
      output,
      monomialOrder
    );
  }
  Vector<Polynomial<ElementZmodP> > polynomialsModP;
  ExpressionContext contextModular(calculator);
  if (
    calculator.getVectorFromFunctionArguments(
      input, polynomialsModP, &contextModular
    )
  ) {
    return
    CalculatorFunctionsPolynomial::polynomialDivisionVerbosePart2(
      calculator, polynomialsModP, contextModular, output, monomialOrder
    );
  }
  return
  output.assignError(calculator, "Failed to extract list of polynomials. ");
}

bool CalculatorFunctionsPolynomial::factorPolynomialModPrime(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPolynomial::factorPolynomialModPrime");
  if (input.size() != 2 && input.size() != 3) {
    return
    calculator
    << "Expected two arguments "
    << "(polynomial and prime) or one argument (modular polynomial).";
  }
  Expression converted;
  if (input.size() == 3) {
    if (
      !CalculatorConversions::polynomialModuloInteger(
        calculator, input, converted
      )
    ) {
      return
      calculator
      << "Failed to convert arguments to modular polynomial.";
    }
  } else {
    converted = input[1];
  }
  WithContext<Polynomial<ElementZmodP> > polynomial;
  if (!converted.isOfTypeWithContext(&polynomial)) {
    return calculator << "Failed to extract modular polynomial. ";
  }
  if (polynomial.content.isEqualToZero()) {
    return calculator << "Factoring zero not allowed. ";
  }
  LargeInteger prime = polynomial.content.coefficients[0].modulus;
  if (prime < 0) {
    return calculator << "The modulus: " << prime << " is not positive. ";
  }
  std::stringstream commentsOnFailure;
  if (!prime.value.isPossiblyPrime(2, true, &commentsOnFailure)) {
    return
    calculator
    << "The modulus: "
    << prime
    << " appears not to be prime. "
    << commentsOnFailure.str();
  }
  std::stringstream comments;
  std::stringstream out;
  PolynomialFactorizationUnivariate<ElementZmodP> result;
  PolynomialFactorizationCantorZassenhaus<
    PolynomialModuloPolynomial<ElementZmodP>,
    Polynomial<ElementZmodP>,
    Polynomial<ElementZmodP>
  > algorithm;
  polynomial.context.getFormat(result.format);
  result.format.flagSuppressModP = true;
  comments
  << "Converted polynomial: \\("
  << polynomial.content.toString(&result.format)
  << "\\)<br>";
  if (
    !result.factor(polynomial.content, algorithm, &comments, &comments)
  ) {
    out
    << "Failed to factor. "
    << comments.str()
    << "Factorization so far: "
    << result.toStringResult(&result.format);
    return output.assignValue(calculator, out.str());
  }
  calculator
  << "Factorization success: "
  << result.toStringResult(&result.format);
  List<Expression> factorsList;
  Expression constant;
  constant.assignValueWithContext(
    calculator, result.constantFactor, polynomial.context
  );
  factorsList.addOnTop(constant);
  for (int i = 0; i < result.reduced.size; i ++) {
    Expression next;
    next.assignValueWithContext(
      calculator, result.reduced[i], polynomial.context
    );
    factorsList.addOnTop(next);
  }
  return output.makeSequence(calculator, &factorsList);
}

bool CalculatorFunctionsPolynomial::polynomialDivisionQuotient(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPolynomial::polynomialDivisionQuotient");
  ExpressionContext context(calculator);
  Vector<Polynomial<AlgebraicNumber> > polynomialsRational;
  if (
    !CalculatorConversions::getListPolynomialVariableLabelsLexicographic(
      calculator, input, polynomialsRational, context
    )
  ) {
    return
    output.assignError(calculator, "Failed to extract list of polynomials. ");
  }
  GroebnerBasisComputation<AlgebraicNumber> computation;
  computation.polynomialOrder.monomialOrder =
  MonomialPolynomial::orderDefault();
  computation.flagStoreQuotients = true;
  for (int i = 1; i < polynomialsRational.size; i ++) {
    if (polynomialsRational[i].isEqualToZero()) {
      return output.assignError(calculator, "Division by zero.");
    }
    computation.addBasisElementNoReduction(polynomialsRational[i]);
  }
  Polynomial<AlgebraicNumber> outputRemainder;
  computation.remainderDivisionByBasis(
    polynomialsRational[0], outputRemainder, - 1
  );
  Expression currentE, polynomialExpression;
  List<Expression> quotients;
  for (int i = 0; i < computation.quotients.size; i ++) {
    currentE.reset(calculator);
    currentE.addChildAtomOnTop("MakeExpression");
    polynomialExpression.assignValueWithContext(
      calculator, computation.quotients[i], context
    );
    currentE.addChildOnTop(polynomialExpression);
    quotients.addOnTop(currentE);
  }
  if (quotients.size == 1) {
    output = quotients[0];
    return true;
  }
  return output.makeSequence(calculator, &quotients);
}

bool CalculatorFunctionsPolynomial::factorPolynomialFiniteFieldsWithComments(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool includeComments
) {
  STACK_TRACE("CalculatorFunctionsPolynomial::factorPolynomialFiniteFields");
  WithContext<Polynomial<Rational> > polynomial;
  if (
    !CalculatorConversions::convert(calculator, input[1], polynomial)
  ) {
    return false;
  }
  if (polynomial.content.minimalNumberOfVariables() > 1) {
    return
    output.assignError(
      calculator,
      "I have been taught to factor one variable polynomials only. "
    );
  }
  PolynomialFactorizationUnivariate<Rational> factorization;
  PolynomialFactorizationFiniteFields algorithm;
  std::stringstream comments;
  if (
    !factorization.factor(
      polynomial.content, algorithm, &comments, &comments
    )
  ) {
    return output.assignValue(calculator, comments.str());
  }
  if (includeComments) {
    calculator.comments << comments.str();
  }
  List<Expression> resultSequence;
  Expression constantFactor;
  constantFactor.assignValue(calculator, factorization.constantFactor);
  resultSequence.addOnTop(constantFactor);
  Expression polynomialE;
  for (int i = 0; i < factorization.reduced.size; i ++) {
    Expression expressionE(calculator);
    polynomialE.assignValueWithContext(
      calculator, factorization.reduced[i], polynomial.context
    );
    expressionE.addChildAtomOnTop("MakeExpression");
    expressionE.addChildOnTop(polynomialE);
    resultSequence.addOnTop(expressionE);
  }
  return output.makeSequence(calculator, &resultSequence);
}

bool CalculatorFunctionsPolynomial::factorPolynomialKronecker(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPolynomial::factorPolynomialKronecker");
  WithContext<Polynomial<Rational> > polynomial;
  if (
    !CalculatorConversions::convert(calculator, input[1], polynomial)
  ) {
    return false;
  }
  if (polynomial.content.minimalNumberOfVariables() > 1) {
    return
    output.assignError(
      calculator,
      "I have been taught to factor one variable polynomials only. "
    );
  }
  PolynomialFactorizationUnivariate<Rational> factorization;
  PolynomialFactorizationKronecker algorithm;
  if (
    !factorization.factor(
      polynomial.content,
      algorithm,
      &calculator.comments,
      &calculator.comments
    )
  ) {
    return false;
  }
  List<Expression> resultSequence;
  Expression constantFactor;
  constantFactor.assignValue(calculator, factorization.constantFactor);
  resultSequence.addOnTop(constantFactor);
  Expression polynomialE;
  for (int i = 0; i < factorization.reduced.size; i ++) {
    if (
      !CalculatorConversions::expressionFromPolynomial(
        calculator,
        factorization.reduced[i],
        polynomialE,
        &polynomial.context
      )
    ) {
      return
      calculator
      << "Unexpected failure to convert factor to expression. ";
    }
    resultSequence.addOnTop(polynomialE);
  }
  return output.makeSequence(calculator, &resultSequence);
}

bool CalculatorFunctionsPolynomial::factorPolynomialKroneckerThenFiniteFields(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsPolynomial::"
    "factorPolynomialKroneckerThenFiniteFields"
  );
  WithContext<Polynomial<Rational> > polynomial;
  if (
    !CalculatorConversions::functionPolynomial(
      calculator, input[1], polynomial, 1, 50, false
    )
  ) {
    return false;
  }
  if (polynomial.content.minimalNumberOfVariables() > 1) {
    return
    output.assignError(
      calculator,
      "I have been taught to factor one variable polynomials only. "
    );
  }
  PolynomialFactorizationUnivariate<Rational> factorizationKronecker;
  PolynomialFactorizationKronecker algorithmKronecker;
  factorizationKronecker.maximumComputations = 10000;
  if (
    factorizationKronecker.factor(
      polynomial.content,
      algorithmKronecker,
      &calculator.comments,
      &calculator.comments
    )
  ) {
    return
    CalculatorFunctionsPolynomial::factorPolynomialProcess(
      calculator,
      polynomial,
      factorizationKronecker.constantFactor,
      factorizationKronecker.reduced,
      output
    );
  }
  PolynomialFactorizationUnivariate<Rational> factorizationFiniteFields;
  PolynomialFactorizationFiniteFields algorithmFiniteFields;
  if (
    !factorizationFiniteFields.factor(
      polynomial.content,
      algorithmFiniteFields,
      &calculator.comments,
      &calculator.comments
    )
  ) {
    return false;
  }
  return
  CalculatorFunctionsPolynomial::factorPolynomialProcess(
    calculator,
    polynomial,
    factorizationFiniteFields.constantFactor,
    factorizationFiniteFields.reduced,
    output
  );
}

bool CalculatorFunctionsPolynomial::factorPolynomialProcess(
  Calculator& calculator,
  WithContext<Polynomial<Rational> >& originalPolynomial,
  const Rational& constantFactor,
  List<Polynomial<Rational> >& factors,
  Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPolynomial::factorPolynomialProcess");
  List<Expression> resultSequence;
  Expression constantFactorExpression;
  constantFactorExpression.assignValue(calculator, constantFactor);
  resultSequence.addOnTop(constantFactorExpression);
  Expression polynomialE;
  for (int i = 0; i < factors.size; i ++) {
    if (
      !CalculatorConversions::expressionFromPolynomial(
        calculator,
        factors[i],
        polynomialE,
        &originalPolynomial.context
      )
    ) {
      return
      calculator
      << "Unexpected failure to convert "
      << factors[i].toString()
      << " to expression. ";
    }
    resultSequence.addOnTop(polynomialE);
  }
  return output.makeSequence(calculator, &resultSequence);
}

template <class Coefficient>
bool CalculatorFunctionsPolynomial::sylvesterMatrixFromPolynomials(
  Calculator& calculator,
  const List<Polynomial<Coefficient> >& polynomials,
  ExpressionContext* context,
  Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPolynomial::sylvesterMatrixFromPolynomials");
  if (polynomials.size < 2) {
    return output.assignError(calculator, "Too few polynomials");
  }
  Matrix<Coefficient> result;
  std::stringstream commentsOnFailure;
  if (
    !SylvesterMatrix<Coefficient>::sylvesterMatrixProduct(
      polynomials, result, &commentsOnFailure
    )
  ) {
    return output.assignError(calculator, commentsOnFailure.str());
  }
  return output.makeMatrix(calculator, result, context, false);
}

bool CalculatorFunctionsPolynomial::sylvesterMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPolynomial::sylvesterMatrix");
  if (input.size() < 3) {
    return false;
  }
  bool isModular = true;
  for (int i = 1; i < input.size(); i ++) {
    if (!input[i].isOfType<Polynomial<ElementZmodP> >()) {
      isModular = false;
      break;
    }
  }
  if (isModular) {
    Expression inputMerged;
    if (
      !input.mergeContextsMyAruments(inputMerged, &calculator.comments)
    ) {
      return
      output.assignError(
        calculator, "Sylvester matrix: failed to merge polynomial contexts."
      );
    }
    List<Polynomial<ElementZmodP> > polynomials;
    polynomials.setSize(input.size() - 1);
    for (int i = 1; i < inputMerged.size(); i ++) {
      if (!inputMerged[i].isOfType(&polynomials[i - 1])) {
        return calculator << "Failed to extract polynomial from expression. ";
      }
    }
    ExpressionContext context = inputMerged[1].getContext();
    return
    CalculatorFunctionsPolynomial::sylvesterMatrixFromPolynomials(
      calculator, polynomials, &context, output
    );
  }
  Vector<Polynomial<Rational> > inputs;
  if (
    calculator.getVectorFromFunctionArguments(
      input,
      inputs,
      nullptr,
      - 1 // ,
      //    CalculatorConversions::functionPolynomial<Rational>
    )
  ) {
    return
    CalculatorFunctionsPolynomial::sylvesterMatrixFromPolynomials(
      calculator, inputs, nullptr, output
    );
  }
  Vector<Polynomial<AlgebraicNumber> > inputsAlgebraic;
  if (
    calculator.getVectorFromFunctionArguments(
      input,
      inputsAlgebraic,
      nullptr,
      - 1 // ,
      // CalculatorConversions::functionPolynomial<AlgebraicNumber>
    )
  ) {
    return
    CalculatorFunctionsPolynomial::sylvesterMatrixFromPolynomials(
      calculator, inputs, nullptr, output
    );
  }
  return false;
}

bool CalculatorFunctionsPolynomial::polynomialRelations(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPolynomial::groebner");
  Vector<Polynomial<Rational> > inputVector;
  if (input.size() < 3) {
    return
    output.assignError(calculator, "Function takes at least two arguments. ");
  }
  const Expression& numComputationsE = input[1];
  Rational upperBound = 0;
  if (!numComputationsE.isOfType(&upperBound)) {
    return
    output.assignError(
      calculator,
      "Failed to convert the first argument "
      "of the expression to rational number."
    );
  }
  if (upperBound > 1000000) {
    return
    output.assignError(
      calculator,
      "Error: your upper limit of polynomial "
      "operations exceeds 1000000, which is too large."
      "You may use negative or zero number give "
      "no computation bound, but please don't. "
    );
  }
  output.reset(calculator);
  for (int i = 1; i < input.size(); i ++) {
    output.addChildOnTop(input[i]);
  }
  ExpressionContext context(calculator);
  if (
    !calculator.getVectorFromFunctionArguments<Polynomial<Rational> >(
      output,
      inputVector,
      &context,
      - 1 // ,
      // CalculatorConversions::functionPolynomial<Rational>
    )
  ) {
    return
    output.assignError(calculator, "Failed to extract polynomial expressions");
  }
  Vector<Polynomial<Rational> > relations;
  Vector<Polynomial<Rational> > generators;
  FormatExpressions format;
  context.getFormat(format);
  for (char i = 0; i < 26; i ++) {
    char currentLetter = 'a' + i;
    std::string currentString;
    currentString = currentLetter;
    if (!format.polynomialAlphabet.contains(currentString)) {
      format.polynomialAlphabet.addOnTop(currentString);
    }
  }
  if (
    !RationalFraction<Rational>::getRelations(
      inputVector, generators, relations, calculator.comments
    )
  ) {
    return calculator << "Failed to extract relations. ";
  }
  std::stringstream out;
  out << "Polynomials:";
  for (int i = 0; i < generators.size; i ++) {
    out
    << "<br>"
    << generators[i].toString(&format)
    << "="
    << inputVector[i].toString(&format);
  }
  out << "<br>Relations: ";
  for (int i = 0; i < relations.size; i ++) {
    relations[i].scaleNormalizeLeadingMonomial(
      &MonomialPolynomial::orderDefault()
    );
    out << "<br>" << relations[i].toString(&format);
  }
  return output.assignValue(calculator, out.str());
}

template <class Coefficient>
bool CalculatorFunctionsPolynomial::
greatestCommonDivisorOrLeastCommonMultiplePolynomialTypePartTwo(
  Calculator& calculator,
  const Polynomial<Coefficient>& left,
  const Polynomial<Coefficient>& right,
  const ExpressionContext& context,
  Expression& output,
  bool doGCD
) {
  STACK_TRACE(
    "CalculatorFunctionsPolynomial::"
    "greatestCommonDivisorOrLeastCommonMultiplePolynomialTypePartTwo"
  );
  Polynomial<Coefficient> outputPolynomial;
  if (left.isEqualToZero()) {
    return calculator << "Not allowed to take gcd/lcm of zero. ";
  }
  Coefficient one = left.coefficients[0].one();
  if (doGCD) {
    Polynomial<Coefficient>::greatestCommonDivisor(
      left, right, outputPolynomial, one, &calculator.comments
    );
  } else {
    Polynomial<Coefficient>::leastCommonMultiple(
      left, right, outputPolynomial, one, &calculator.comments
    );
  }
  return output.assignValueWithContext(calculator, outputPolynomial, context);
}

bool CalculatorFunctionsPolynomial::
greatestCommonDivisorOrLeastCommonMultipleAlgebraic(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool doGCD
) {
  if (input.size() != 3) {
    return false;
  }
  Expression left = input[1];
  Expression right = input[2];
  if (!input.mergeContexts(left, right)) {
    return false;
  }
  Polynomial<AlgebraicNumber> leftPolynomial, rightPolynomial;
  if (
    !left.isOfType(&leftPolynomial) || !right.isOfType(&rightPolynomial)
  ) {
    return false;
  }
  return
  CalculatorFunctionsPolynomial::
  greatestCommonDivisorOrLeastCommonMultiplePolynomialTypePartTwo(
    calculator,
    leftPolynomial,
    rightPolynomial,
    left.getContext(),
    output,
    doGCD
  );
}

bool CalculatorFunctionsPolynomial::
greatestCommonDivisorOrLeastCommonMultipleModular(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool doGCD
) {
  if (input.size() != 3) {
    return false;
  }
  Expression left = input[1];
  Expression right = input[2];
  if (!input.mergeContexts(left, right)) {
    return false;
  }
  Polynomial<ElementZmodP> leftPolynomial, rightPolynomial;
  if (
    !left.isOfType(&leftPolynomial) || !right.isOfType(&rightPolynomial)
  ) {
    return false;
  }
  if (leftPolynomial.isEqualToZero() || rightPolynomial.isEqualToZero()) {
    calculator
    << "Greatest common divisor / "
    << "least common multiple with zero not allowed. ";
    return
    output.assignError(
      calculator, "Error in least common multiple / greatest common divisor."
    );
  }
  LargeIntegerUnsigned modulus = leftPolynomial.coefficients[0].modulus;
  if (
    modulus > static_cast<unsigned>(
      ElementZmodP::maximumModulusForUserFacingPolynomialDivision
    )
  ) {
    return
    calculator
    << "Polynomial modulus exceeds the maximum allowed "
    << "for user-facing polynomial division: "
    << ElementZmodP::maximumModulusForUserFacingPolynomialDivision
    << ". ";
  }
  if (!modulus.isPossiblyPrime(0, true)) {
    return
    calculator
    << "Cannot do GCD / lcm: modulus "
    << modulus
    << " is not prime. ";
  }
  return
  CalculatorFunctionsPolynomial::
  greatestCommonDivisorOrLeastCommonMultiplePolynomialTypePartTwo(
    calculator,
    leftPolynomial,
    rightPolynomial,
    left.getContext(),
    output,
    doGCD
  );
}

bool CalculatorFunctionsPolynomial::
greatestCommonDivisorOrLeastCommonMultiplePolynomial(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool doGCD
) {
  STACK_TRACE(
    "CalculatorFunctionsPolynomial::"
    "greatestCommonDivisorOrLeastCommonMultiplePolynomial"
  );
  if (input.size() != 3) {
    return false;
  }
  const Expression& left = input[1];
  if (left.isOfType<Polynomial<ElementZmodP> >()) {
    return
    CalculatorFunctionsPolynomial::
    greatestCommonDivisorOrLeastCommonMultipleModular(
      calculator, input, output, doGCD
    );
  }
  if (left.isOfType<Polynomial<AlgebraicNumber> >()) {
    return
    CalculatorFunctionsPolynomial::
    greatestCommonDivisorOrLeastCommonMultipleAlgebraic(
      calculator, input, output, doGCD
    );
  }
  Vector<Polynomial<Rational> > polynomials;
  ExpressionContext context(calculator);
  if (
    !calculator.getVectorFromFunctionArguments(
      input,
      polynomials,
      &context,
      2 // ,
      // CalculatorConversions::functionPolynomial<Rational>
    )
  ) {
    return
    output.assignError(
      calculator, "Failed to extract a list of 2 polynomials. "
    );
  }
  return
  CalculatorFunctionsPolynomial::
  greatestCommonDivisorOrLeastCommonMultiplePolynomialTypePartTwo(
    calculator,
    polynomials[0],
    polynomials[1],
    context,
    output,
    doGCD
  );
}

bool CalculatorFunctionsPolynomial::groebner(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  int order,
  bool useModZp
) {
  STACK_TRACE("CalculatorFunctionsPolynomial::groebner");
  Vector<Polynomial<Rational> > inputVector;
  Vector<Polynomial<ElementZmodP> > inputVectorZmodP;
  ExpressionContext context(calculator);
  if (input.size() < 3) {
    return
    output.assignError(calculator, "Function takes at least two arguments. ");
  }
  const Expression& numComputationsE = input[1];
  Rational upperBound = 0;
  if (!numComputationsE.isOfType(&upperBound)) {
    return
    output.assignError(
      calculator,
      "Failed to convert the first argument of "
      "the expression to rational number. "
    );
  }
  if (upperBound > 1000000) {
    return
    output.assignError(
      calculator,
      "Error: your upper limit of polynomial "
      "operations exceeds 1000000, which is too large. "
      "You may use negative or zero number "
      "give no computation bound. "
    );
  }
  int upperBoundComputations = int(upperBound.getDoubleValue());
  output.reset(calculator);
  output.checkInitialization();
  for (int i = 1; i < input.size(); i ++) {
    output.addChildOnTop(input[i]);
  }
  int modulus = 0;
  if (useModZp) {
    if (!output[1].isSmallInteger(&modulus)) {
      return
      output.assignError(
        calculator,
        "Error: failed to extract modulo from the second argument. "
      );
    }
    if (!MathRoutines::isPrimeSimple(modulus)) {
      return output.assignError(calculator, "Error: modulus not prime. ");
    }
  }
  if (
    !calculator.getVectorFromFunctionArguments<Polynomial<Rational> >(
      output,
      inputVector,
      &context,
      - 1 // ,
      // CalculatorConversions::functionPolynomial<Rational>
    )
  ) {
    return
    output.assignError(calculator, "Failed to extract polynomial expressions");
  }
  for (int i = 0; i < inputVector.size; i ++) {
    inputVector[i].scaleNormalizeLeadingMonomial(
      &MonomialPolynomial::orderDefault()
    );
  }
  GroebnerBasisComputation<AlgebraicNumber> groebnerComputation;
  context.getFormat(groebnerComputation.format);
  context.getFormat(global.defaultFormat.getElement());
  if (useModZp) {
    ElementZmodP element;
    element.makeMinusOne(static_cast<unsigned>(modulus));
    inputVectorZmodP.setSize(inputVector.size);
    for (int i = 0; i < inputVector.size; i ++) {
      inputVectorZmodP[i].makeZero();
      for (int j = 0; j < inputVector[i].size(); j ++) {
        element = inputVector[i].coefficients[j];
        inputVectorZmodP[i].addMonomial(inputVector[i][j], element);
      }
    }
  }
  List<Polynomial<AlgebraicNumber> > outputGroebner;
  outputGroebner = inputVector;
  if (order == MonomialPolynomial::Order::gradedLexicographic) {
    groebnerComputation.polynomialOrder.monomialOrder.setComparison(
      MonomialPolynomial::greaterThan_totalDegree_leftLargerWins
    );
  } else if (order == MonomialPolynomial::Order::gradedReverseLexicographic) {
    groebnerComputation.polynomialOrder.monomialOrder.setComparison(
      MonomialPolynomial::greaterThan_totalDegree_rightSmallerWins
    );
  } else if (order == MonomialPolynomial::Order::lexicographicOpposite) {
    groebnerComputation.polynomialOrder.monomialOrder.setComparison(
      MonomialPolynomial::greaterThan_rightLargerWins
    );
  } else if (order == MonomialPolynomial::Order::lexicographic) {
    groebnerComputation.polynomialOrder.monomialOrder.setComparison(
      MonomialPolynomial::greaterThan_leftLargerWins
    );
  } else {
    global.fatal << "Unexpected order value: " << order << global.fatal;
  }
  groebnerComputation.format.monomialOrder =
  groebnerComputation.polynomialOrder.monomialOrder;
  groebnerComputation.maximumMonomialOperations = upperBoundComputations;
  bool success =
  groebnerComputation.transformToReducedGroebnerBasis(outputGroebner);
  std::stringstream out;
  out << "<hr>" << groebnerComputation.toStringLetterOrder();
  out << "<br>Starting basis (" << inputVector.size << " elements): ";
  for (int i = 0; i < inputVector.size; i ++) {
    out
    << "<br>"
    << HtmlRoutines::getMathNoDisplay(
      inputVector[i].toString(&groebnerComputation.format)
    );
  }
  if (success) {
    out
    << "<br>Minimal Groebner basis with "
    << outputGroebner.size
    << " elements, computed using algorithm 1, using "
    << groebnerComputation.numberPolynomialDivisions
    << " polynomial divisions. ";
    for (int i = 0; i < outputGroebner.size; i ++) {
      out
      << "<br> "
      << HtmlRoutines::getMathNoDisplay(
        outputGroebner[i].toString(&groebnerComputation.format)
      );
    }
    out << "<br>Output in calculator-ready format: ";
    out << "<br>(";
    for (int i = 0; i < outputGroebner.size; i ++) {
      out << outputGroebner[i].toString(&groebnerComputation.format);
      if (i != outputGroebner.size - 1) {
        out << ", <br>";
      }
    }
    out << ")";
  } else {
    out
    << "<br>Minimal Groebner basis not computed: "
    << "exceeded the user-given limit of "
    << upperBoundComputations
    << " polynomial operations. ";
    out
    << "<br>An intermediate non-Groebner basis containing total "
    << groebnerComputation.basis.size
    << " basis elements: ";
    out << "<br>GroebnerLex{}(10000, <br>";
    for (int i = 0; i < groebnerComputation.basis.size; i ++) {
      out
      << groebnerComputation.basis[i].element.toString(
        &groebnerComputation.format
      );
      if (i != groebnerComputation.basis.size - 1) {
        out << ", <br>";
      }
    }
    out << ");";
  }
  calculator << out.str();
  List<Expression> elements;
  for (int i = 0; i < groebnerComputation.basis.size; i ++) {
    WithContext<Polynomial<AlgebraicNumber> > element;
    element.context = context;
    element.content = groebnerComputation.basis[i].element;
    Expression converted;
    element.toExpression(calculator, converted);
    elements.addOnTop(converted);
  }
  return output.makeSequence(calculator, &elements);
}

bool CalculatorFunctionsPolynomial::
combineFractionsCommutativeWithInternalLibrary(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsPolynomial::"
    "combineFractionsCommutativeWithInternalLibrary"
  );
  if (!input.startsWith(calculator.opPlus(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (
    !leftE.startsWith(calculator.opDivide(), 3) ||
    !rightE.startsWith(calculator.opDivide(), 3)
  ) {
    return false;
  }
  WithContext<RationalFraction<AlgebraicNumber> > rationalFunction;
  if (
    !CalculatorConversions::functionRationalFraction<AlgebraicNumber>(
      calculator, input, rationalFunction, true
    )
  ) {
    return false;
  }
  return
  CalculatorConversions::expressionFromRationalFraction(
    calculator,
    rationalFunction.content,
    output,
    &rationalFunction.context
  );
}

bool CalculatorFunctionsPolynomial::divideExpressionsAsIfPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsPolynomial::divideExpressionsAsIfPolynomial"
  );
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  WithContext<Polynomial<AlgebraicNumber> > numerator;
  WithContext<Polynomial<AlgebraicNumber> > denominator;
  if (
    !CalculatorConversions::functionPolynomial(
      calculator, input[2], denominator, 1, 6, true
    )
  ) {
    return false;
  }
  Rational denominatorDegree = denominator.content.totalDegree();
  if (denominatorDegree == 0) {
    // The denominator is a constant;
    return false;
  }
  if (
    !CalculatorConversions::functionPolynomial(
      calculator, input[1], numerator, 1, 20, true
    )
  ) {
    return false;
  }
  if (
    !WithContext<Polynomial<AlgebraicNumber> >::mergeContexts(
      numerator, denominator, nullptr
    )
  ) {
    return false;
  }
  if (!numerator.context.hasAtomicUserDefinedVariables()) {
    return false;
  }
  if (numerator.content.minimalNumberOfVariables() > 1) {
    if (
      numerator.content.totalDegree() > 4 ||
      denominator.content.totalDegree() > 4
    ) {
      return false;
    }
  }
  RationalFraction<AlgebraicNumber> result;
  result = numerator.content;
  result /= denominator.content;
  Polynomial<AlgebraicNumber> simplifiedNumerator;
  result.getNumerator(simplifiedNumerator);
  if (numerator.content == simplifiedNumerator) {
    // Nothing was cancelled.
    return false;
  }
  WithContext<RationalFraction<AlgebraicNumber> > simplified;
  simplified.context = numerator.context;
  simplified.content = result;
  Expression simplifiedExpression;
  simplifiedExpression.assignWithContext(calculator, simplified);
  Expression outputCandidate;
  if (
    !CalculatorConversions::functionExpressionFromBuiltInType(
      calculator, simplifiedExpression, outputCandidate
    )
  ) {
    return
    calculator
    << "Unexpected failure to convert "
    << simplifiedExpression.toString()
    << " to expression.";
  }
  output = outputCandidate;
  Polynomial<AlgebraicNumber> simplifiedDenominator;
  result.getDenominator(simplifiedDenominator);
  if (simplifiedDenominator.totalDegree() < denominatorDegree) {
    Polynomial<AlgebraicNumber> quotient, remainder;
    denominator.content.divideBy(
      simplifiedDenominator, quotient, remainder, nullptr
    );
    if (!remainder.isEqualToZero()) {
      global.fatal
      << "Remainder of quotient must not be zero. "
      << global.fatal;
    }
    WithContext<Polynomial<AlgebraicNumber> > quotientWithContext;
    quotientWithContext.context = numerator.context;
    quotientWithContext.content = quotient;
    Expression quotientExpression;
    quotientExpression.assignWithContext(calculator, quotientWithContext);
    Expression quotientNotZero;
    quotientNotZero.makeXOX(
      calculator,
      calculator.opNotEqual(),
      quotientExpression,
      calculator.expressionZero()
    );
    calculator.objectContainer.constraints.addOnTop(quotientNotZero);
  }
  return true;
}
