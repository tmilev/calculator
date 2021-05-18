// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "calculator_inner_typed_functions.h"
#include "calculator_functions_polynomial.h"
#include "math_rational_function_implementation.h"

template <>
bool CalculatorConversions::innerPolynomial<Rational>(Calculator& calculator, const Expression& input, Expression& output);
template <>
bool CalculatorConversions::functionPolynomial<Rational>(Calculator& calculator, const Expression& input, Expression& output);

bool CalculatorFunctionsPolynomial::polynomialDivisionRemainder(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::polynomialDivisionRemainder");
  ExpressionContext theContext(calculator);
  Vector<Polynomial<AlgebraicNumber> > polynomials;
  if (!calculator.getListPolynomialVariableLabelsLexicographic(
    input, polynomials, theContext
  )) {
    return output.makeError("Failed to extract list of polynomials. ", calculator);
  }
  GroebnerBasisComputation<AlgebraicNumber> computation;
  computation.polynomialOrder.monomialOrder = MonomialPolynomial::orderDefault();
  computation.flagStoreQuotients = true;
  for (int i = 1; i < polynomials.size; i ++) {
    if (polynomials[i].isEqualToZero()) {
      return output.makeError("Division by zero.", calculator);
    }
    computation.addBasisElementNoReduction(polynomials[i]);
  }
  Polynomial<AlgebraicNumber> outputRemainder;
  computation.remainderDivisionByBasis(polynomials[0], outputRemainder, - 1);
  Expression thePolyE;
  thePolyE.assignValueWithContext(outputRemainder, theContext, calculator);
  output.reset(calculator);
  output.addChildAtomOnTop("MakeExpression");
  output.addChildOnTop(thePolyE);
  return true;
}

bool CalculatorFunctionsPolynomial::polynomialDivisionVerboseGrLex(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return CalculatorFunctionsPolynomial::polynomialDivisionVerbose(
    calculator, input, output, &MonomialPolynomial::orderDegreeThenLeftLargerWins()
  );
}

bool CalculatorFunctionsPolynomial::polynomialDivisionVerboseGradedReverseLexicographic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  List<MonomialPolynomial>::Comparator order(
    MonomialPolynomial::greaterThan_totalDegree_rightSmallerWins
  );
  return CalculatorFunctionsPolynomial::polynomialDivisionVerbose(
    calculator, input, output, &order
  );
}

bool CalculatorFunctionsPolynomial::polynomialDivisionVerboseLexicographic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPolynomialDivisionVerboseLexicographic");
  List<MonomialPolynomial>::Comparator order(
    MonomialPolynomial::greaterThan_leftLargerWins
  );
  return CalculatorFunctionsPolynomial::polynomialDivisionVerbose(
    calculator,
    input,
    output,
    &order
  );
}

bool CalculatorFunctionsPolynomial::polynomialDivisionVerboseLexicographicOpposite(
  Calculator& calculator, const Expression& input, Expression& output
) {
  List<MonomialPolynomial>::Comparator order(
    MonomialPolynomial::greaterThan_rightLargerWins
  );
  return CalculatorFunctionsPolynomial::polynomialDivisionVerbose(
    calculator, input, output, &order
  );
}

bool CalculatorFunctionsPolynomial::polynomialDivisionVerbose(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  List<MonomialPolynomial>::Comparator* monomialOrder
) {
  MacroRegisterFunctionWithName("Calculator::innerPolynomialDivisionVerbose");
  ExpressionContext theContext(calculator);
  Vector<Polynomial<AlgebraicNumber> > polynomialsRational;
  if (!calculator.getListPolynomialVariableLabelsLexicographic(
    input, polynomialsRational, theContext
  )) {
    return output.makeError(
      "Failed to extract list of polynomials. ",
      calculator
    );
  }
  GroebnerBasisComputation<AlgebraicNumber> computation;
  computation.flagDoLogDivision = true;
  computation.flagStoreQuotients = true;
  computation.polynomialOrder.monomialOrder = *monomialOrder;
  for (int i = 1; i < polynomialsRational.size; i ++) {
    if (polynomialsRational[i].isEqualToZero()) {
      return output.makeError("Division by zero.", calculator);
    }
    computation.addBasisElementNoReduction(polynomialsRational[i]);
  }
  if (monomialOrder != nullptr) {
    computation.polynomialOrder.monomialOrder = *monomialOrder;
  }
  computation.remainderDivisionByBasis(polynomialsRational[0], computation.remainderDivision, - 1);
  theContext.getFormat(computation.format);
  computation.format.flagUseLatex = true;
  computation.format.flagUseFrac = true;
  std::stringstream latexOutput;
  latexOutput << "<br>In latex: <br>"
  << "\\documentclass{article}\\usepackage{longtable}"
  << "\\usepackage{xcolor}\\usepackage{multicol}"
  << "\\begin{document} "
  << computation.divisionReport.getElement().getDivisionStringLaTeX()
  << "\\end{document}";
  std::stringstream out;
  out << computation.divisionReport.getElement().getDivisionStringHtml();
  out << latexOutput.str();
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsPolynomial::polynomialDivisionSlidesGrLex(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerPolynomialDivisionSlidesGrLex");
  ExpressionContext theContext(calculator);
  Vector<Polynomial<AlgebraicNumber> > polynomialsRational;
  if (!calculator.getListPolynomialVariableLabelsLexicographic(
    input, polynomialsRational, theContext
  )) {
    return output.makeError("Failed to extract list of polynomials. ", calculator);
  }
  if (polynomialsRational.size < 3) {
    return calculator
    << "Function takes at least 3 inputs: "
    << "index of first slide, dividend, divisor(s).";
  }
  GroebnerBasisComputation<AlgebraicNumber> computation;
  computation.flagDoLogDivision = true;
  computation.flagStoreQuotients = true;
  computation.polynomialOrder.monomialOrder.setComparison(
    MonomialPolynomial::greaterThan_totalDegree_rightSmallerWins
  );
  for (int i = 2; i < polynomialsRational.size; i ++) {
    if (polynomialsRational[i].isEqualToZero()) {
      return output.makeError("Division by zero.", calculator);
    }
    computation.addBasisElementNoReduction(polynomialsRational[i]);
  }
  if (!polynomialsRational[0].isSmallInteger(&computation.divisionReport.getElement().firstIndexLatexSlide)) {
    return calculator << "Failed to extract integer from first argument";
  }
  computation.remainderDivisionByBasis(polynomialsRational[1], computation.remainderDivision, - 1);
  theContext.getFormat(computation.format);
  computation.format.flagUseLatex = true;
  computation.format.flagUseFrac = true;
  std::stringstream latexOutput;
  latexOutput
  << "In latex: \r\n \\documentclass{beamer}\n"
  << "\\usepackage{longtable}\\usepackage{xcolor}\\usepackage{multicol}\n"
  << "\\newcommand{\\alertNoH}[2]{\\alert<handout:0|#1>{#2}}\n"
  << "\\newcommand{\\fcAnswer}[2]{"
  << "\\uncover<handout:0|\\the\\numexpr#1- 1\\relax>{"
  << "\\alertNoH{\\the\\numexpr#1- 1\\relax}{\\textbf{?}}}"
  << "{\\uncover<#1->{\\alertNoH{#1}{\\!\\!\\!#2}}}}\r\n"
  << "\\begin{document} "
  << "\\begin{frame}"
  << computation.divisionReport.getElement().getDivisionLaTeXSlide()
  << "\\end{frame}"
  << "\\end{document}\r\n";
  return output.assignValue(
    HtmlRoutines::convertStringToHtmlString(latexOutput.str(), true), calculator
  );
}

bool CalculatorFunctionsPolynomial::factorPolynomialModPrime(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::factorPolynomialModPrime");
  if (input.size() != 2 && input.size() != 3) {
    return calculator << "Expected two arguments "
    << "(polynomial and prime) or one argument (modular polynomial).";
  }
  Expression converted;
  if (input.size() == 3) {
    if (!CalculatorConversions::innerPolynomialModuloInteger(
      calculator, input, converted
    )) {
      return calculator
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
    return calculator << "The modulus: " << prime
    << " appears not to be prime. " << commentsOnFailure.str();
  }
  std::stringstream comments, out;
  PolynomialFactorizationUnivariate<ElementZmodP, PolynomialFactorizationCantorZassenhaus> result;
  polynomial.context.getFormat(result.format);
  result.format.flagSuppressModP = true;
  comments << "Converted polynomial: \\("
  << polynomial.content.toString(&result.format) << "\\)<br>";
  if (!result.factor(polynomial.content, &comments, &comments)) {
    out << "Failed to factor. " << comments.str()
    << "Factorization so far: " << result.toStringResult(&result.format);
    return output.assignValue(out.str(), calculator);
  }
  calculator << "Factorization success: " << result.toStringResult(&result.format);
  List<Expression> factorsList;
  Expression constant;
  constant.assignValueWithContext(result.constantFactor, polynomial.context, calculator);
  factorsList.addOnTop(constant);
  for (int i = 0; i < result.reduced.size; i ++) {
    Expression next;
    next.assignValueWithContext(result.reduced[i], polynomial.context, calculator);
    factorsList.addOnTop(next);
  }
  return output.makeSequence(calculator, &factorsList);
}

bool CalculatorFunctionsPolynomial::polynomialDivisionQuotient(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerPolynomialDivisionQuotient");
  ExpressionContext theContext(calculator);
  Vector<Polynomial<AlgebraicNumber> > polynomialsRational;
  if (!calculator.getListPolynomialVariableLabelsLexicographic(input, polynomialsRational, theContext)) {
    return output.makeError("Failed to extract list of polynomials. ", calculator);
  }
  GroebnerBasisComputation<AlgebraicNumber> computation;
  computation.polynomialOrder.monomialOrder = MonomialPolynomial::orderDefault();
  computation.flagStoreQuotients = true;
  for (int i = 1; i < polynomialsRational.size; i ++) {
    if (polynomialsRational[i].isEqualToZero()) {
      return output.makeError("Division by zero.", calculator);
    }
    computation.addBasisElementNoReduction(polynomialsRational[i]);
  }
  Polynomial<AlgebraicNumber> outputRemainder;
  computation.remainderDivisionByBasis(polynomialsRational[0], outputRemainder, - 1);
  Expression currentE, thePolyE;
  List<Expression> theList;
  for (int i = 0; i < computation.quotients.size; i ++) {
    currentE.reset(calculator);
    currentE.addChildAtomOnTop("MakeExpression");
    thePolyE.assignValueWithContext(computation.quotients[i], theContext, calculator);
    currentE.addChildOnTop(thePolyE);
    theList.addOnTop(currentE);
  }
  if (theList.size == 1) {
    output = theList[0];
    return true;
  }
  return output.makeSequence(calculator, &theList);
}

bool CalculatorFunctionsPolynomial::factorPolynomialFiniteFields(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPolynomial::factorPolynomialFiniteFields");
  WithContext<Polynomial<Rational> > polynomial;
  if (!CalculatorConversions::convert(
    input, CalculatorConversions::innerPolynomial<Rational>, polynomial
  )) {
    return false;
  }
  if (polynomial.content.minimalNumberOfVariables() > 1) {
    return output.makeError(
      "I have been taught to factor one variable polynomials only. ",
      calculator
    );
  }
  PolynomialFactorizationUnivariate<Rational, PolynomialFactorizationFiniteFields> factorization;
  std::stringstream comments;
  if (!factorization.factor(
    polynomial.content,
    &comments,
    &comments
  )) {
    return output.assignValue(comments.str(), calculator);
  }
  List<Expression> resultSequence;
  Expression constantFactor;
  constantFactor.assignValue(factorization.constantFactor, calculator);
  resultSequence.addOnTop(constantFactor);
  Expression polynomialE;

  for (int i = 0; i < factorization.reduced.size; i ++) {
    Expression expressionE(calculator);
    polynomialE.assignValueWithContext(
      factorization.reduced[i], polynomial.context, calculator
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
  MacroRegisterFunctionWithName("Calculator::factorPolynomialKronecker");
  WithContext<Polynomial<Rational> > polynomial;
  if (!CalculatorConversions::convert(
    input, CalculatorConversions::innerPolynomial<Rational>, polynomial
  )) {
    return false;
  }
  if (polynomial.content.minimalNumberOfVariables() > 1) {
    return output.makeError(
      "I have been taught to factor one variable polynomials only. ",
      calculator
    );
  }
  PolynomialFactorizationUnivariate<Rational, PolynomialFactorizationKronecker> factorization;
  if (!factorization.factor(
    polynomial.content,
    &calculator.comments,
    &calculator.comments
  )) {
    return false;
  }
  List<Expression> resultSequence;
  Expression constantFactor;
  constantFactor.assignValue(factorization.constantFactor, calculator);
  resultSequence.addOnTop(constantFactor);
  Expression polynomialE;
  for (int i = 0; i < factorization.reduced.size; i ++) {
    Expression expressionE(calculator);
    polynomialE.assignValueWithContext(
      factorization.reduced[i], polynomial.context, calculator
    );
    expressionE.addChildAtomOnTop("MakeExpression");
    expressionE.addChildOnTop(polynomialE);
    resultSequence.addOnTop(expressionE);
  }
  return output.makeSequence(calculator, &resultSequence);
}

bool CalculatorFunctionsPolynomial::factorPolynomialRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::factorPolynomialRational");
  WithContext<Polynomial<Rational> > polynomial;
  if (!CalculatorConversions::convert(
    input, CalculatorConversions::innerPolynomial<Rational>, polynomial
  )) {
    return false;
  }
  if (polynomial.content.minimalNumberOfVariables() > 1) {
    return output.makeError(
      "I have been taught to factor one variable polynomials only. ",
      calculator
    );
  }
  PolynomialFactorizationUnivariate<Rational, PolynomialFactorizationKronecker> factorizationKronecker;
  factorizationKronecker.maximumComputations = 10000;
  if (factorizationKronecker.factor(
    polynomial.content,
    &calculator.comments,
    &calculator.comments
  )) {
    return CalculatorFunctionsPolynomial::factorPolynomialProcess(
      calculator,
      polynomial,
      factorizationKronecker.constantFactor,
      factorizationKronecker.reduced,
      output
    );
  }
  PolynomialFactorizationUnivariate<Rational, PolynomialFactorizationFiniteFields> factorizationFiniteFields;
  if (!factorizationFiniteFields.factor(polynomial.content, &calculator.comments, & calculator.comments)) {
    return false;
  }
  return CalculatorFunctionsPolynomial::factorPolynomialProcess(
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
  MacroRegisterFunctionWithName("CalculatorFunctionsPolynomial::factorPolynomialProcess");
  List<Expression> resultSequence;
  Expression constantFactorExpression;
  constantFactorExpression.assignValue(constantFactor, calculator);
  resultSequence.addOnTop(constantFactorExpression);
  Expression polynomialE;
  for (int i = 0; i < factors.size; i ++) {
    Expression expressionE(calculator);
    polynomialE.assignValueWithContext(
      factors[i], originalPolynomial.context, calculator
    );
    expressionE.addChildAtomOnTop("MakeExpression");
    expressionE.addChildOnTop(polynomialE);
    resultSequence.addOnTop(expressionE);
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
  MacroRegisterFunctionWithName("CalculatorFunctionsPolynomial::sylvesterMatrixFromPolynomials");
  if (polynomials.size < 2) {
    return output.makeError("Too few polynomials", calculator);
  }
  Matrix<Coefficient> result;
  std::stringstream commentsOnFailure;
  if (!SylvesterMatrix<Coefficient>::sylvesterMatrixProduct(
    polynomials, result, &commentsOnFailure
  )) {
    return output.makeError(commentsOnFailure.str(), calculator);
  }
  return output.assignMatrix(result, calculator, context, false);
}

bool CalculatorFunctionsPolynomial::sylvesterMatrix(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPolynomial::sylvesterMatrix");
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
    if (!input.mergeContextsMyAruments(inputMerged, &calculator.comments)) {
      return output.makeError(
        "Sylvester matrix: failed to merge polynomial contexts.",
        calculator
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
    return CalculatorFunctionsPolynomial::sylvesterMatrixFromPolynomials(
      calculator, polynomials, &context, output
    );
  }
  Vector<Polynomial<Rational> > inputs;
  if (calculator.getVectorFromFunctionArguments(
    input,
    inputs,
    nullptr,
    - 1,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return CalculatorFunctionsPolynomial::sylvesterMatrixFromPolynomials(
      calculator, inputs, nullptr, output
    );
  }
  Vector<Polynomial<AlgebraicNumber> > inputsAlgebraic;
  if (calculator.getVectorFromFunctionArguments(
    input,
    inputsAlgebraic,
    nullptr,
    - 1,
    CalculatorConversions::functionPolynomial<AlgebraicNumber>
  )) {
    return CalculatorFunctionsPolynomial::sylvesterMatrixFromPolynomials(
      calculator, inputs, nullptr, output
    );
  }
  return false;
}

template <class Coefficient>
std::string PolynomialDivisionReport<Coefficient>::getDivisionLaTeXSlide() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::getDivisionLaTeXSlide");
  this->checkInitialization();
  std::stringstream out;
  List<Polynomial<Coefficient> >& theRemainders = this->intermediateRemainders;
  List<Polynomial<Coefficient> >& theSubtracands = this->intermediateSubtractands;
  this->owner->format.monomialOrder = this->owner->polynomialOrder.monomialOrder;
  bool oneDivisor = (this->owner->basis.size == 1);
  this->allMonomials.clear();
  this->allMonomials.addOnTopNoRepetition(this->startingPolynomial.monomials);
  for (int i = 0; i < theRemainders.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theRemainders[i].monomials);
  }
  for (int i = 0; i < theSubtracands.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theSubtracands[i].monomials);
  }
  auto& basis = this->owner->basis;
  for (int i = 0; i < basis.size; i ++) {
    Polynomial<Coefficient>& current = basis[i].element;
    this->allMonomials.addOnTopNoRepetition(current.monomials);
  }

  for (int i = 0; i < this->owner->quotients.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(this->owner->quotients[i].monomials);
  }
  if (this->owner->remainderDivision.isEqualToZero()) {
    MonomialPolynomial constMon;
    constMon.makeOne();
    this->allMonomials.addOnTopNoRepetition(constMon);
  }
  this->allMonomials.quickSortDescending(&this->owner->polynomialOrder.monomialOrder);
  List<List<int> > dummyListList;
  List<int> dummyList;
  dummyListList.setSize(this->allMonomials.size);
  dummyList.initializeFillInObject(this->allMonomials.size, - 1);
  this->firstNonZeroIndicesPerIntermediateSubtracand.initializeFillInObject(theSubtracands.size, 0);
  this->highlightMonsRemainders.initializeFillInObject(theRemainders.size,   dummyListList);
  this->highlightMonsSubtracands.initializeFillInObject(theSubtracands.size, dummyListList);
  this->highlightMonsQuotients.initializeFillInObject(basis.size, dummyListList);
  this->highlightMonsDivisors.initializeFillInObject (basis.size, dummyListList);
  this->fcAnswerMonsRemainders.initializeFillInObject(theRemainders.size, dummyList);
  this->fcAnswerMonsSubtracands.initializeFillInObject(theSubtracands.size, dummyList);
  this->fcAnswerMonsQuotients.initializeFillInObject(basis.size, dummyList);
  this->fcAnswerMonsDivisors.initializeFillInObject(basis.size, dummyList);
  this->uncoverAllMonsRemainders.initializeFillInObject(theRemainders.size, 1);
  this->uncoverAllMonsSubtracands.initializeFillInObject(theSubtracands.size, 1);
  this->uncoverAllMonsQuotients.initializeFillInObject(basis.size, 1);
  this->uncoverAllMonsDivisors.initializeFillInObject(basis.size, 1);
  this->uncoverMonsFinalRemainder.initializeFillInObject(this->allMonomials.size, - 1);
  this->additionalHighlightFinalRemainder.initializeFillInObject(this->allMonomials.size,- 1);
  this->additionalHighlightRemainders.initializeFillInObject(this->allMonomials.size, dummyList);
  this->highlightAllMonsFinalRemainder = - 1;
  int currentSlideNumer = this->firstIndexLatexSlide + 1;
  for (int i = 0; i < theRemainders.size; i ++) {
    this->computeHighLightsFromRemainder(i, currentSlideNumer);
  }
  for (int i = 0; i < theSubtracands.size; i ++) {
    this->firstNonZeroIndicesPerIntermediateSubtracand[i] = theSubtracands[i].getIndexLeadingMonomial(
      nullptr,
      nullptr,
      &this->owner->polynomialOrder.monomialOrder
    );
  }
  this->owner->format.flagUseLatex = true;
  out << "\\renewcommand{\\arraystretch}{1.2}";
  out << "\\begin{longtable}{@{}c";
  for (int i = 0; i < this->allMonomials.size * 2; i ++) {
    out << "@{}c";
  }
  out << "}";
  if (!oneDivisor) {
    out << "{\\color{orange}\\textbf{Remainder:} }&"
    << this->getSpacedMonomialsWithHighlightLaTeX(
      this->owner->remainderDivision,
      nullptr,
      nullptr,
      &this->uncoverMonsFinalRemainder,
      &this->additionalHighlightFinalRemainder,
      - 1,
      true
    )
    << "\\\\\\hline";
  }
  if (!oneDivisor) {
    out << "\\textbf{Divisor(s)} &" << "\\multicolumn{"
    << this->allMonomials.size * 2 << "}{c}{"
    << "\\alertNoH{" << this->uncoverAllMonsQuotients[0] << "}{"
    << "\\textbf{Quotient(s)}"
    << "}"
    << "}"
    << "\\\\";
  }
  for (int i = 0; i < this->owner->basis.size; i ++) {
    if (!oneDivisor) {
      out << this->getSpacedMonomialsWithHighlightLaTeX(
        this->owner->basis[i].element,
        &this->highlightMonsDivisors[i],
        &this->fcAnswerMonsDivisors[i],
        nullptr,
        nullptr,
        this->uncoverAllMonsDivisors[i],
        false
      );
    } else {
      out << "\\uncover<" << this->uncoverAllMonsQuotients[0] << "->{"
      << "\\alertNoH{" << this->uncoverAllMonsQuotients[0] << "}{"
      << "\\textbf{Quotient: }"
      << "}"
      << "}";
    }
    out << "&";
    out << "\\multicolumn{" << this->allMonomials.size * 2 << "}{c}{";
    out << this->getSpacedMonomialsWithHighlightLaTeX(
      this->owner->quotients[i],
      &this->highlightMonsQuotients[i],
      &this->fcAnswerMonsQuotients[i],
      nullptr,
      nullptr,
      this->uncoverAllMonsQuotients[i],
      false
    );
    out << "}";
    out << "\\\\";
    if (!oneDivisor) {
      out << "\\hline";
    }
  }
  out << " \\cline{2-" << this->allMonomials.size * 2 + 1 << "}"
  << " \\cline{2-" << this->allMonomials.size * 2 + 1 << "}";
  for (int i = 0; i < theRemainders.size; i ++) {
    if (i == 0) {
      if (oneDivisor) {
        out << "\\multicolumn{1}{c|}{"
        << this->getSpacedMonomialsWithHighlightLaTeX(
          this->owner->basis[0].element,
          &this->highlightMonsDivisors[0],
          &this->fcAnswerMonsDivisors[i],
          nullptr,
          nullptr,
          0,
          false
        )
        << "}";
      }
    } else {
      out << "\\uncover<" << this->uncoverAllMonsRemainders[i] << "->{";
    }
    if (i == theRemainders.size - 1 && i != 0 && oneDivisor) {
      out << "\\uncover<" << this->highlightAllMonsFinalRemainder << "->{"
      << "\\textbf{\\color{orange}Remainder: }"
      << "}";
    }
    out << "&";
    out << this->getSpacedMonomialsWithHighlightLaTeX(
      theRemainders[i],
      &this->highlightMonsRemainders[i],
      &this->fcAnswerMonsRemainders[i],
      nullptr,
      &this->additionalHighlightRemainders[i],
      this->uncoverAllMonsRemainders[i],
      true
    )
    << "\\\\";
    if (i < theSubtracands.size) {
      out << "\\uncover<" << this->uncoverAllMonsSubtracands[i] << "->{";
      out << "\\uncover<" << this->uncoverAllMonsSubtracands[i] + 2
      << "->{\\alertNoH{" << this->uncoverAllMonsSubtracands[i] + 2
      << ", " << this->uncoverAllMonsSubtracands[i] + 3
      << "}{"
      << "$\\overline{\\phantom{A}}$"
      << "}}";
      out << "&";
      out << this->getSpacedMonomialsWithHighlightLaTeX(
        theSubtracands[i],
        &this->highlightMonsSubtracands[i],
        &this->fcAnswerMonsSubtracands[i],
        nullptr,
        nullptr,
        this->uncoverAllMonsSubtracands[i],
        true
      );
      out << "\\\\\\cline{"
      << this->firstNonZeroIndicesPerIntermediateSubtracand[i] * 2 + 2
      << "-" << this->allMonomials.size * 2 + 1 << "}";
      out << "}";
    }
    if (i != 0) {
      out << "}";
    }
  }
  out << "\\end{longtable}";
  out << "\r\n";
  out << this->divisionLog.str();
  return out.str();
}

template <class Coefficient>
std::string PolynomialDivisionReport<Coefficient>::getSpacedMonomialsWithHighlightLaTeX(
  const Polynomial<Coefficient>& thePoly,
  List<List<int> >* slidesToHighlightMon,
  List<int>* slidesToFcAnswer,
  List<int>* slidesToUncover,
  List<int>* slidesAdditionalHighlight,
  int slidesToUncoverAllMons,
  bool useColumnSeparator
) {
  MacroRegisterFunctionWithName("PolynomialDivisionReport::getSpacedMonomialsWithHighlightLaTeX");
  (void) slidesToUncoverAllMons;
  std::stringstream out;
  bool found = false;
  int countMons = 0;
  if (thePoly.isEqualToZero()) {
    if (useColumnSeparator) {
      for (int i = 0; i < this->allMonomials.size * 2 - 1; i ++) {
        out << "&";
      }
    }
    std::stringstream highlightHeadStream;
    std::stringstream highlightTailStream;
    MonomialPolynomial tempM;
    tempM.makeOne();
    int monIndex = this->allMonomials.getIndex(tempM);
    if (slidesAdditionalHighlight != nullptr && monIndex != - 1) {
      if ((*slidesAdditionalHighlight)[monIndex] > 0) {
        highlightHeadStream << "{ \\only<"
        << (*slidesAdditionalHighlight)[monIndex]
        << "->{\\color{orange}}";
        highlightTailStream << "}";
      }
    }
    if (slidesToFcAnswer != nullptr && monIndex != - 1) {
      if ((*slidesToFcAnswer)[monIndex] > 1) {
        highlightHeadStream << "\\fcAnswer{" << (*slidesToFcAnswer)[monIndex] << "}{";
        highlightTailStream << "}";
      }
    }
    out << "$ " << highlightHeadStream.str() << "0"
    << highlightTailStream.str() << "$";
    return out.str();
  }
  for (int i = 0; i < this->allMonomials.size; i ++) {
    int theIndex = thePoly.monomials.getIndex(this->allMonomials[i]);
    if (theIndex == - 1) {
      if (useColumnSeparator) {
        if (i != this->allMonomials.size - 1) {
          out << "&&";
        }
      }
      continue;
    }
    std::string highlightHead;
    std::string highlightTail;
    int fcAnswerSlide = - 1;
    if (slidesToFcAnswer != nullptr) {
      if (i < (*slidesToFcAnswer).size) {
        if ((*slidesToFcAnswer)[i] > 1) {
          fcAnswerSlide = (*slidesToFcAnswer)[i];
          if (slidesToHighlightMon != nullptr) {
            (*slidesToHighlightMon)[i].addOnTop(fcAnswerSlide);
          }
        }
      }
    }
    if (slidesToUncover != nullptr) {
      if ((*slidesToUncover)[i] > 1) {
        std::stringstream highlightStream;
        highlightStream << "\\uncover<" << (*slidesToUncover)[i] << "->{";
        highlightHead += highlightStream.str();
        highlightTail = "}" + highlightTail;
      }
    }
    if (slidesToHighlightMon != nullptr) {
      if ((*slidesToHighlightMon)[i].size > 0) {
        highlightHead += "\\alertNoH{" + (*slidesToHighlightMon)[i].toStringCommaDelimited() + "}{";
        highlightTail = "}" + highlightTail;
      }
    }
    if (slidesAdditionalHighlight != nullptr) {
      if ((*slidesAdditionalHighlight)[i] > 0) {
        std::stringstream highlightStream;
        highlightStream << "{\\only<" << (*slidesAdditionalHighlight)[i] << "->{\\color{orange}}";
        highlightHead += highlightStream.str();
        highlightTail = "}" + highlightTail;
      }
    }
    countMons ++;
    std::string monWithSign =
    Polynomial<Coefficient>::getBlendCoefficientAndMonomial(
      thePoly[theIndex], thePoly.coefficients[theIndex], true, &this->owner->format
    );
    std::string sign = monWithSign.substr(0, 1);
    std::string monNoSign = monWithSign.substr(1);
    if (sign == "-" || found) {
      if (fcAnswerSlide != - 1) {
        out << "\\uncover<" << fcAnswerSlide << "->{";
      }
      out << highlightHead;
      out << "$" << sign << "$";
      out << highlightTail;
      if (fcAnswerSlide != - 1) {
        out << "}";
      }
    }
    found = true;
    if (useColumnSeparator) {
      out << "&";
    } else {
      out << " ";
    }
    if (fcAnswerSlide != - 1) {
      out << "\\fcAnswer{" << fcAnswerSlide << "}{";
    }
    out << highlightHead << "$" << monNoSign << "$" << highlightTail;
    if (fcAnswerSlide != - 1) {
      out << "}";
    }
    if (!useColumnSeparator) {
      out << " ";
    }
    if (useColumnSeparator) {
      if (i != this->allMonomials.size - 1) {
        out << "&";
      }
    }
  }
  if (countMons != thePoly.size()) {
    out << " Programming ERROR!";
  }
  return out.str();
}

template <class Coefficient>
void PolynomialDivisionReport<Coefficient>::computeHighLightsFromRemainder(
  int remainderIndex, int& currentSlideNumber
) {
  MacroRegisterFunctionWithName("PolynomialDivisionReport::computeHighLightsFromRemainder");
  this->checkInitialization();
  auto& basis = this->owner->basis;
  if (remainderIndex == 0) {
    for (int i = 0; i < this->allMonomials.size; i ++) {
      this->highlightMonsRemainders[remainderIndex][i].addOnTop(currentSlideNumber);
    }
    currentSlideNumber ++;
    for (int j = 0; j < basis.size; j ++) {
      for (int i = 0; i < this->allMonomials.size; i ++) {
        this->highlightMonsDivisors[j][i].addOnTop(currentSlideNumber);
      }
      currentSlideNumber ++;
    }
  }
  for (int i = 0; i < this->intermediateHighlightedMons[remainderIndex].size; i ++) {
    int monomialIndex = this->allMonomials.getIndex(
      this->intermediateHighlightedMons[remainderIndex][i]
    );
    this->additionalHighlightRemainders[remainderIndex][monomialIndex] = currentSlideNumber;
    this->additionalHighlightFinalRemainder[monomialIndex] = currentSlideNumber;
    this->uncoverMonsFinalRemainder[monomialIndex] = currentSlideNumber;
    currentSlideNumber ++;
  }
  MonomialPolynomial constMon;
  constMon.makeOne();
  int zeroMonIndex = this->allMonomials.getIndex(constMon);
  if (this->intermediateRemainders[remainderIndex].isEqualToZero()) {
    this->additionalHighlightRemainders[remainderIndex][zeroMonIndex] = currentSlideNumber;
    this->additionalHighlightFinalRemainder[zeroMonIndex] = currentSlideNumber;
    currentSlideNumber ++;
  }
  if (remainderIndex == this->intermediateRemainders.size - 1) {
    for (int i = 0; i < basis.size; i ++) {
      this->uncoverAllMonsQuotients[i] = currentSlideNumber;
      for (int j = 0; j < this->allMonomials.size; j ++) {
        this->highlightMonsQuotients[i][j].addOnTop(currentSlideNumber);
      }
    }
    currentSlideNumber ++;
    this->highlightAllMonsFinalRemainder = currentSlideNumber;
  }
  if (remainderIndex >= this->intermediateSelectedDivisors.size) {
    return;
  }
  Polynomial<Coefficient>& currentRemainder = this->intermediateRemainders[remainderIndex];
  int indexCurrentDivisor = this->intermediateSelectedDivisors[remainderIndex];
  Polynomial<Coefficient>& currentDivisor = basis[indexCurrentDivisor].element;
  MonomialPolynomial divisorLeadingMonomial;
  int indexCurrentDivisorLeadingMoN = currentDivisor.getIndexLeadingMonomial(
    &divisorLeadingMonomial, nullptr, &this->owner->polynomialOrder.monomialOrder
  );
  int indexCurrentDivisorLeadingMonInAllMons = this->allMonomials.getIndex(
    divisorLeadingMonomial
  );
  MonomialPolynomial maxMonCurrentRemainder;
  Coefficient leadingCFCurrentRemainder;
  currentRemainder.getIndexLeadingMonomial(
    &maxMonCurrentRemainder, &leadingCFCurrentRemainder, &this->owner->polynomialOrder.monomialOrder
  );
  int indexCurrentRemainderLeadingMonInAllMons = this->allMonomials.getIndex(maxMonCurrentRemainder);
  this->highlightMonsDivisors[indexCurrentDivisor][indexCurrentDivisorLeadingMonInAllMons].addOnTop(currentSlideNumber);
  this->highlightMonsRemainders[remainderIndex][indexCurrentRemainderLeadingMonInAllMons].addOnTop(currentSlideNumber);

  if (remainderIndex == 0) {
    this->divisionLog << "$\\vphantom"
    << "{\\frac{x^1}{x^1}}$";
  }
  FormatExpressions& format = this->owner->format;
  this->divisionLog << "\\only<" << currentSlideNumber << ","
  << currentSlideNumber + 1 << "| handout:0>{Divide "
  << "\\alertNoH{"
  << currentSlideNumber << ","
  << currentSlideNumber + 1 << "}{"
  << "$" << currentRemainder.getBlendCoefficientAndMonomial(
    maxMonCurrentRemainder, leadingCFCurrentRemainder, false, &format
  )
  << "$ "
  << "}"
  << " by "
  << "\\alertNoH{"
  << currentSlideNumber << ","
  << currentSlideNumber + 1 << "}{"
  << "$" << currentRemainder.getBlendCoefficientAndMonomial(
    currentDivisor.monomials[indexCurrentDivisorLeadingMoN],
    currentDivisor.coefficients[indexCurrentDivisorLeadingMoN],
    false,
    &format
  )
  << "$"
  << "}."
  << "}";
  currentSlideNumber ++;
  this->highlightMonsDivisors[indexCurrentDivisor][indexCurrentDivisorLeadingMonInAllMons].
  addOnTop(currentSlideNumber);
  this->highlightMonsRemainders[remainderIndex][indexCurrentRemainderLeadingMonInAllMons].addOnTop(currentSlideNumber);
  int indexCurrentQuotientMonInAllMons =
  this->allMonomials.getIndex(this->intermediateHighestMonDivHighestMon[remainderIndex]);
  Polynomial<Coefficient>& currentQuotient = this->owner->quotients[indexCurrentDivisor];
  int indexCurrentQuotientMoN = currentQuotient.monomials.getIndex(
    this->intermediateHighestMonDivHighestMon[remainderIndex]
  );
  this->fcAnswerMonsQuotients[indexCurrentDivisor][indexCurrentQuotientMonInAllMons] = currentSlideNumber;
  currentSlideNumber ++;
  this->highlightMonsQuotients[indexCurrentDivisor][indexCurrentQuotientMonInAllMons].addOnTop(currentSlideNumber);
  for (int i = 0; i < currentDivisor.size(); i ++) {
    this->highlightMonsDivisors[indexCurrentDivisor][
      this->allMonomials.getIndex(currentDivisor[i])
    ].addOnTop(currentSlideNumber);
  }
  this->uncoverAllMonsSubtracands[remainderIndex] = currentSlideNumber;
  this->divisionLog << "\\only<"
  << currentSlideNumber << ", "
  << currentSlideNumber + 1
  << "| handout:0>{Multiply "
  << "\\alertNoH{"
  << currentSlideNumber << ", "
  << currentSlideNumber + 1
  << "}{$"
  << currentQuotient.getBlendCoefficientAndMonomial(
    currentQuotient.monomials[indexCurrentQuotientMoN],
    currentQuotient.coefficients[indexCurrentQuotientMoN],
    false,
    &format
  )
  << "$}"
  << " by divisor. "
  << "}";
  currentSlideNumber ++;
  this->highlightMonsQuotients[indexCurrentDivisor][indexCurrentQuotientMonInAllMons].addOnTop(currentSlideNumber);
  for (int i = 0; i < currentDivisor.size(); i ++) {
    this->highlightMonsDivisors[indexCurrentDivisor][
      this->allMonomials.getIndex(currentDivisor[i])
    ].addOnTop(currentSlideNumber);
  }
  if (this->fcAnswerMonsSubtracands[remainderIndex].size != this->allMonomials.size) {
    this->fcAnswerMonsSubtracands[remainderIndex].initializeFillInObject(this->allMonomials.size, - 1);
  }
  for (int i = 0; i < this->intermediateSubtractands[remainderIndex].size(); i ++) {
    this->fcAnswerMonsSubtracands[remainderIndex][
      this->allMonomials.getIndex(this->intermediateSubtractands[remainderIndex][i])
    ] = currentSlideNumber;
  }
  currentSlideNumber ++;
  for (int i = 0; i < this->intermediateRemainders[remainderIndex].size(); i ++) {
    this->highlightMonsRemainders[remainderIndex][
      this->allMonomials.getIndex(this->intermediateRemainders[remainderIndex][i])
    ].addOnTop(currentSlideNumber);
  }
  for (int i = 0; i < this->intermediateSubtractands[remainderIndex].size(); i ++) {
    this->highlightMonsSubtracands[remainderIndex][
      this->allMonomials.getIndex(this->intermediateSubtractands[remainderIndex][i])
    ].addOnTop(currentSlideNumber);
  }
  this->uncoverAllMonsRemainders[remainderIndex + 1] = currentSlideNumber;
  this->divisionLog << "\\only<" << currentSlideNumber << ", "
  << currentSlideNumber + 1 << "| handout:0>{subtract last two polynomials.}";
  currentSlideNumber ++;
  for (int i = 0; i < this->intermediateRemainders[remainderIndex].size(); i ++) {
    this->highlightMonsRemainders[remainderIndex][
      this->allMonomials.getIndex(this->intermediateRemainders[remainderIndex][i])
    ].addOnTop(currentSlideNumber);
  }
  for (int i = 0; i < this->intermediateSubtractands[remainderIndex].size(); i ++) {
    this->highlightMonsSubtracands[remainderIndex][
      this->allMonomials.getIndex(this->intermediateSubtractands[remainderIndex][i])
    ].addOnTop(currentSlideNumber);
  }
  if (remainderIndex + 1 >= this->intermediateRemainders.size) {
    global.fatal << "Something is wrong: not enough intermediate remainders. " << global.fatal;
  }
  for (int i = 0; i < this->intermediateRemainders[remainderIndex + 1].size(); i ++) {
    this->fcAnswerMonsRemainders[remainderIndex + 1][
      this->allMonomials.getIndex(this->intermediateRemainders[remainderIndex + 1][i])
    ] = currentSlideNumber;
  }
  if (this->intermediateRemainders[remainderIndex + 1].isEqualToZero()) {
    this->fcAnswerMonsRemainders[remainderIndex + 1][zeroMonIndex] = currentSlideNumber;
  }
  currentSlideNumber ++;
}

bool CalculatorFunctionsPolynomial::polynomialRelations(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::groebner");
  Vector<Polynomial<Rational> > inputVector;
  if (input.size() < 3) {
    return output.makeError("Function takes at least two arguments. ", calculator);
  }
  const Expression& numComputationsE = input[1];
  Rational upperBound = 0;
  if (!numComputationsE.isOfType(&upperBound)) {
    return output.makeError("Failed to convert the first argument of the expression to rational number.", calculator);
  }
  if (upperBound > 1000000) {
    return output.makeError(
      "Error: your upper limit of polynomial operations exceeds 1000000, which is too large."
      "You may use negative or zero number give no computation bound, but please don't. ",
      calculator
    );
  }
  output.reset(calculator);
  for (int i = 1; i < input.size(); i ++) {
    output.addChildOnTop(input[i]);
  }
  ExpressionContext theContext(calculator);
  if (!calculator.getVectorFromFunctionArguments<Polynomial<Rational> >(
    output,
    inputVector,
    &theContext,
    - 1,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return output.makeError("Failed to extract polynomial expressions", calculator);
  }
  Vector<Polynomial<Rational> > relations, theGens;
  FormatExpressions theFormat;
  theContext.getFormat(theFormat);
  for (char i = 0; i < 26; i ++) {
    char currentLetter = 'a' + i;
    std::string currentStr;
    currentStr = currentLetter;
    if (!theFormat.polynomialAlphabet.contains(currentStr)) {
      theFormat.polynomialAlphabet.addOnTop(currentStr);
    }
  }
  if (!RationalFraction<Rational>::getRelations(inputVector, theGens, relations, calculator.comments)) {
    return calculator << "Failed to extract relations. ";
  }
  std::stringstream out;
  out << "Polynomials:";
  for (int i = 0; i < theGens.size; i ++) {
    out << "<br>" << theGens[i].toString(&theFormat) << "=" << inputVector[i].toString(&theFormat);
  }
  out << "<br>Relations: ";
  for (int i = 0; i < relations.size; i ++) {
    relations[i].scaleNormalizeLeadingMonomial(&MonomialPolynomial::orderDefault());
    out << "<br>" << relations[i].toString(&theFormat);
  }
  return output.assignValue(out.str(), calculator);
}

template <class Coefficient>
bool CalculatorFunctionsPolynomial::greatestCommonDivisorOrLeastCommonMultiplePolynomialTypePartTwo(
  Calculator& calculator,
  const Polynomial<Coefficient>& left,
  const Polynomial<Coefficient>& right,
  const ExpressionContext &context,
  Expression& output,
  bool doGCD
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPolynomial::greatestCommonDivisorOrLeastCommonMultiplePolynomialTypePartTwo");
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
  return output.assignValueWithContext(outputPolynomial, context, calculator);
}

bool CalculatorFunctionsPolynomial::greatestCommonDivisorOrLeastCommonMultipleAlgebraic(
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
  if (!left.isOfType(&leftPolynomial) || !right.isOfType(&rightPolynomial)) {
    return false;
  }
  return CalculatorFunctionsPolynomial::greatestCommonDivisorOrLeastCommonMultiplePolynomialTypePartTwo(
    calculator, leftPolynomial, rightPolynomial, left.getContext(), output, doGCD
  );
}

bool CalculatorFunctionsPolynomial::greatestCommonDivisorOrLeastCommonMultipleModular(
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
  if (!left.isOfType(&leftPolynomial) || !right.isOfType(&rightPolynomial)) {
    return false;
  }
  if (leftPolynomial.isEqualToZero() || rightPolynomial.isEqualToZero()) {
    calculator
    << "Greatest common divisor / "
    << "least common multiple with zero not allowed. ";
    return output.makeError("Error in least common multiple / greatest common divisor.", calculator);
  }
  LargeIntegerUnsigned modulus = leftPolynomial.coefficients[0].modulus;

  if (modulus > static_cast<unsigned>(ElementZmodP::maximumModulusForUserFacingPolynomialDivision)) {
    return calculator
    << "Polynomial modulus exceeds the maximum allowed "
    << "for user-facing polynomial division: "
    << ElementZmodP::maximumModulusForUserFacingPolynomialDivision << ". ";
  }
  if (!modulus.isPossiblyPrime(0, true)) {
    return calculator << "Cannot do GCD / lcm: modulus "
    << modulus << " is not prime. ";
  }
  return CalculatorFunctionsPolynomial::greatestCommonDivisorOrLeastCommonMultiplePolynomialTypePartTwo(
    calculator, leftPolynomial, rightPolynomial, left.getContext(), output, doGCD
  );
}

bool CalculatorFunctionsPolynomial::greatestCommonDivisorOrLeastCommonMultiplePolynomial(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool doGCD
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPolynomial::greatestCommonDivisorOrLeastCommonMultiplePolynomial");
  if (input.size() != 3) {
    return false;
  }
  const Expression& left = input[1];
  if (left.isOfType<Polynomial<ElementZmodP> >()) {
    return CalculatorFunctionsPolynomial::greatestCommonDivisorOrLeastCommonMultipleModular(calculator, input, output, doGCD);
  }
  if (left.isOfType<Polynomial<AlgebraicNumber> >()) {
    return CalculatorFunctionsPolynomial::greatestCommonDivisorOrLeastCommonMultipleAlgebraic(calculator, input, output, doGCD);
  }
  Vector<Polynomial<Rational> > polynomials;
  ExpressionContext theContext(calculator);
  if (!calculator.getVectorFromFunctionArguments(
    input,
    polynomials,
    &theContext,
    2,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return output.makeError("Failed to extract a list of 2 polynomials. ", calculator);
  }
  return CalculatorFunctionsPolynomial::greatestCommonDivisorOrLeastCommonMultiplePolynomialTypePartTwo(
    calculator, polynomials[0], polynomials[1], theContext, output, doGCD
  );
}

bool CalculatorFunctionsPolynomial::groebner(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  int order,
  bool useModZp
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPolynomial::groebner");
  Vector<Polynomial<Rational> > inputVector;
  Vector<Polynomial<ElementZmodP> > inputVectorZmodP;
  ExpressionContext context(calculator);
  if (input.size() < 3) {
    return output.makeError("Function takes at least two arguments. ", calculator);
  }
  const Expression& numComputationsE = input[1];
  Rational upperBound = 0;
  if (!numComputationsE.isOfType(&upperBound)) {
    return output.makeError(
      "Failed to convert the first argument of "
      "the expression to rational number. ",
      calculator
    );
  }
  if (upperBound > 1000000) {
    return output.makeError(
      "Error: your upper limit of polynomial "
      "operations exceeds 1000000, which is too large. "
      "You may use negative or zero number "
      "give no computation bound. ",
      calculator
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
      return output.makeError(
        "Error: failed to extract modulo from the second argument. ",
        calculator
      );
    }
    if (!MathRoutines::isPrimeSimple(modulus)) {
      return output.makeError("Error: modulus not prime. ", calculator);
    }
  }
  if (!calculator.getVectorFromFunctionArguments<Polynomial<Rational> >(
    output,
    inputVector,
    &context,
    - 1,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return output.makeError(
      "Failed to extract polynomial expressions",
      calculator
    );
  }
  for (int i = 0; i < inputVector.size; i ++) {
    inputVector[i].scaleNormalizeLeadingMonomial(&MonomialPolynomial::orderDefault());
  }
  GroebnerBasisComputation<AlgebraicNumber> groebnerComputation;
  context.getFormat(groebnerComputation.format);
  context.getFormat(global.defaultFormat.getElement());
  if (useModZp) {
    ElementZmodP tempElt;
    tempElt.makeMinusOne(static_cast<unsigned>(modulus));
    inputVectorZmodP.setSize(inputVector.size);
    for (int i = 0; i < inputVector.size; i ++) {
      inputVectorZmodP[i].makeZero();
      for (int j = 0; j < inputVector[i].size(); j ++) {
        tempElt = inputVector[i].coefficients[j];
        inputVectorZmodP[i].addMonomial(inputVector[i][j], tempElt);
      }
    }
  }
  List<Polynomial<AlgebraicNumber> > outputGroebner, outputGroebner2;
  outputGroebner = inputVector;
  outputGroebner2 = inputVector;
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
  } else if (order == MonomialPolynomial::Order::lexicographic){
    groebnerComputation.polynomialOrder.monomialOrder.setComparison(
      MonomialPolynomial::greaterThan_leftLargerWins
    );
  } else {
    global.fatal << "Unexpected order value: " << order << global.fatal;
  }
  groebnerComputation.format.monomialOrder = groebnerComputation.polynomialOrder.monomialOrder;
  groebnerComputation.maximumMonomialOperations = upperBoundComputations;
  bool success = groebnerComputation.transformToReducedGroebnerBasis(outputGroebner);
  std::stringstream out;
  out << groebnerComputation.toStringLetterOrder(false);
  out << "Letter/expression order: ";
  int numberOfVariables = context.numberOfVariables();
  for (int i = 0; i < numberOfVariables; i ++) {
    out << context.getVariable(i).toString();
    if (i != numberOfVariables - 1) {
      out << "&lt;";
    }
  }
  out << "<br>Starting basis (" << inputVector.size  << " elements): ";
  for (int i = 0; i < inputVector.size; i ++) {
    out << "<br>"
    << HtmlRoutines::getMathNoDisplay(
      inputVector[i].toString(&groebnerComputation.format)
    );
  }
  if (success) {
    out << "<br>Minimal Groebner basis with "
    << outputGroebner.size
    << " elements, computed using algorithm 1, using "
    << groebnerComputation.numberPolynomialDivisions
    << " polynomial divisions. ";
    for (int i = 0; i < outputGroebner.size; i ++) {
      out << "<br> " << HtmlRoutines::getMathNoDisplay(
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
    out << "<br>Minimal Groebner basis not computed: "
    << "exceeded the user-given limit of "
    << upperBoundComputations << " polynomial operations. ";
    out << "<br>An intermediate non-Groebner basis containing total "
    << groebnerComputation.basis.size
    << " basis elements: ";
    out << "<br>GroebnerLexUpperLimit{}(10000, <br>";
    for (int i = 0; i < groebnerComputation.basis.size; i ++) {
      out << groebnerComputation.basis[i].element.toString(&groebnerComputation.format);
      if (i != groebnerComputation.basis.size - 1) {
        out << ", <br>";
      }
    }
    out << ");";
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsPolynomial::combineFractionsCommutativeWithInternalLibrary(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::combineFractionsCommutativeWithInternalLibrary");
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
  Expression converted(calculator);
  if (!CalculatorConversions::functionRationalFunction<AlgebraicNumber>(calculator, input, converted)) {
    return false;
  }
  WithContext<RationalFraction<AlgebraicNumber> > rationalFunction;
  if (!converted.isOfTypeWithContext(&rationalFunction)) {
    return false;
  }
  return CalculatorConversions::expressionFromRationalFraction(calculator, rationalFunction.content, output, &rationalFunction.context);
}

bool CalculatorFunctionsPolynomial::divideExpressionsAsIfPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPolynomial::divideExpressionsAsIfPolynomial");
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  WithContext<Polynomial<AlgebraicNumber> > numerator;
  WithContext<Polynomial<AlgebraicNumber> > denominator;
  if (!CalculatorConversions::convert(
    input[2],
    CalculatorConversions::functionPolynomialWithExponentLimit<AlgebraicNumber, 6, 1>,
    denominator
  )) {
    return false;
  }
  Rational denominatorDegree = denominator.content.totalDegree();
  if (denominatorDegree == 0) {
    // The denominator is a constant;
    return false;
  }
  if (!CalculatorConversions::convert(
    input[1],
    CalculatorConversions::functionPolynomialWithExponentLimit<AlgebraicNumber, 6, 1>,
    numerator
  )) {
    return false;
  }
  if (!WithContext<Polynomial<AlgebraicNumber> >::mergeContexts(numerator, denominator, nullptr)) {
    return false;
  }
  if (!numerator.context.hasAtomicUserDefinedVariables()) {
    return false;
  }
  if (numerator.content.minimalNumberOfVariables() > 1) {
    if (numerator.content.totalDegree() > 4 || denominator.content.totalDegree() > 4) {
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
  simplifiedExpression.assignWithContext(simplified, calculator);
  Expression outputCandidate;
  if (!CalculatorConversions::functionExpressionFromBuiltInType(
    calculator, simplifiedExpression, outputCandidate
  )) {
    return calculator
    << "Unexpected failure to convert "
    << simplifiedExpression.toString()
    << " to expression.";
  }
  output = outputCandidate;
  Polynomial<AlgebraicNumber> simplifiedDenominator;
  result.getDenominator(simplifiedDenominator);
  if (simplifiedDenominator.totalDegree() < denominatorDegree) {
    Polynomial<AlgebraicNumber> quotient, remainder;
    denominator.content.divideBy(simplifiedDenominator, quotient, remainder, nullptr);
    if (!remainder.isEqualToZero()) {
      global.fatal << "Remainder of quotient must not be zero. " << global.fatal;
    }
    WithContext<Polynomial<AlgebraicNumber> > quotientWithContext;
    quotientWithContext.context = numerator.context;
    quotientWithContext.content = quotient;
    Expression quotientExpression;
    quotientExpression.assignWithContext(quotientWithContext, calculator);
    Expression quotientNotZero;
    quotientNotZero.makeXOX(calculator, calculator.opNotEqual(), quotientExpression, calculator.expressionZero());
    calculator.objectContainer.constraints.addOnTop(quotientNotZero);
  }
  return true;
}
