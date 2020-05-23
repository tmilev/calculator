// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "calculator_functions_polynomial.h"

template <>
bool CalculatorConversions::innerPolynomial<Rational>(Calculator& calculator, const Expression& input, Expression& output);
template <>
bool CalculatorConversions::functionPolynomial<Rational>(Calculator& calculator, const Expression& input, Expression& output);

bool CalculatorFunctionsPolynomial::polynomialDivisionRemainder(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerPolynomialDivisionRemainder");
  ExpressionContext theContext(calculator);
  Vector<Polynomial<AlgebraicNumber> > polynomials;
  if (!calculator.getListPolynomialVariableLabelsLexicographic(
    input, polynomials, theContext
  )) {
    return output.makeError("Failed to extract list of polynomials. ", calculator);
  }
  GroebnerBasisComputation<AlgebraicNumber> computation;
  computation.thePolynomialOrder.monomialOrder = MonomialP::orderDefault();
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
    calculator, input, output, &MonomialP::orderDegreeThenLeftLargerWins()
  );
}

bool CalculatorFunctionsPolynomial::polynomialDivisionVerboseGradedReverseLexicographic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  List<MonomialP>::Comparator order(
    MonomialP::greaterThan_totalDegree_rightSmallerWins
  );
  return CalculatorFunctionsPolynomial::polynomialDivisionVerbose(
    calculator, input, output, &order
  );
}

bool CalculatorFunctionsPolynomial::polynomialDivisionVerboseLexicographic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPolynomialDivisionVerboseLexicographic");
  List<MonomialP>::Comparator order(
    MonomialP::greaterThan_leftLargerWins
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
  List<MonomialP>::Comparator order(
    MonomialP::greaterThan_rightLargerWins
  );
  return CalculatorFunctionsPolynomial::polynomialDivisionVerbose(
    calculator, input, output, &order
  );
}

bool CalculatorFunctionsPolynomial::polynomialDivisionVerbose(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  List<MonomialP>::Comparator* monomialOrder
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
  computation.thePolynomialOrder.monomialOrder = *monomialOrder;
  for (int i = 1; i < polynomialsRational.size; i ++) {
    if (polynomialsRational[i].isEqualToZero()) {
      return output.makeError("Division by zero.", calculator);
    }
    computation.addBasisElementNoReduction(polynomialsRational[i]);
  }
  if (monomialOrder != nullptr) {
    computation.thePolynomialOrder.monomialOrder = *monomialOrder;
  }
  computation.remainderDivisionByBasis(polynomialsRational[0], computation.remainderDivision, - 1);
  theContext.getFormat(computation.theFormat);
  computation.theFormat.flagUseLatex = true;
  computation.theFormat.flagUseFrac = true;
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
  computation.thePolynomialOrder.monomialOrder.setComparison(
    MonomialP::greaterThan_totalDegree_rightSmallerWins
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
  theContext.getFormat(computation.theFormat);
  computation.theFormat.flagUseLatex = true;
  computation.theFormat.flagUseFrac = true;
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
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFactorPolynomialModPrime");
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
  LargeInteger thePrime = polynomial.content.coefficients[0].modulus;
  if (thePrime < 0) {
    return calculator << "The modulus: " << thePrime << " is not positive. ";
  }
  std::stringstream commentsOnFailure;
  if (!thePrime.value.isPossiblyPrime(2, true, &commentsOnFailure)) {
    calculator << "The modulus: " << thePrime
    << " appears not to be prime. " << commentsOnFailure.str();
  }
  std::stringstream comments, out;
  PolynomialFactorization<ElementZmodP, PolynomialFactorizationCantorZassenhaus> result;
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
  constant.assignValue(result.constantFactor, calculator);
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
  computation.thePolynomialOrder.monomialOrder = MonomialP::orderDefault();
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
  for (int i = 0; i < computation.theQuotients.size; i ++) {
    currentE.reset(calculator);
    currentE.addChildAtomOnTop("MakeExpression");
    thePolyE.assignValueWithContext(computation.theQuotients[i], theContext, calculator);
    currentE.addChildOnTop(thePolyE);
    theList.addOnTop(currentE);
  }
  if (theList.size == 1) {
    output = theList[0];
    return true;
  }
  return output.makeSequence(calculator, &theList);
}

bool CalculatorFunctionsPolynomial::factorPolynomial(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerFactorPolynomial");
  WithContext<Polynomial<Rational> > polynomial;
  if (!calculator.convert(
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
  PolynomialFactorization<Rational, PolynomialFactorizationKronecker> factorization;
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
  Expression polynomialE, expressionE(calculator);

  for (int i = 0; i < factorization.reduced.size; i ++) {
    polynomialE.assignValueWithContext(
      factorization.reduced[i], polynomial.context, calculator
    );
    expressionE.children.clear();
    expressionE.addChildAtomOnTop("MakeExpression");
    expressionE.addChildOnTop(polynomialE);
    resultSequence.addOnTop(expressionE);
  }
  return output.makeSequence(calculator, &resultSequence);
}

bool CalculatorFunctionsPolynomial::sylvesterMatrix(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPolynomial::sylvesterMatrix");
  if (input.size() != 3) {
    return false;
  }
  const Expression& left = input[1];
  const Expression& right = input[2];
  WithContext<Polynomial<ElementZmodP> > leftPolynomialModular, rightPolynomialModular;
  if (
    left.isOfTypeWithContext(&leftPolynomialModular) &&
    right.isOfTypeWithContext(&rightPolynomialModular)
  ) {
    if (!leftPolynomialModular.mergeContexts(
      leftPolynomialModular, rightPolynomialModular, &calculator.comments
    )) {
      return false;
    }

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
  this->owner->theFormat.monomialOrder = this->owner->thePolynomialOrder.monomialOrder;
  bool oneDivisor = (this->owner->theBasis.size == 1);
  this->allMonomials.clear();
  this->allMonomials.addOnTopNoRepetition(this->startingPolynomial.monomials);
  for (int i = 0; i < theRemainders.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theRemainders[i].monomials);
  }
  for (int i = 0; i < theSubtracands.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theSubtracands[i].monomials);
  }
  auto& basis = this->owner->theBasis;
  for (int i = 0; i < basis.size; i ++) {
    Polynomial<Coefficient>& current = basis[i].element;
    this->allMonomials.addOnTopNoRepetition(current.monomials);
  }

  for (int i = 0; i < this->owner->theQuotients.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(this->owner->theQuotients[i].monomials);
  }
  if (this->owner->remainderDivision.isEqualToZero()) {
    MonomialP constMon;
    constMon.makeOne();
    this->allMonomials.addOnTopNoRepetition(constMon);
  }
  this->allMonomials.quickSortDescending(&this->owner->thePolynomialOrder.monomialOrder);
  List<List<int> > dummyListList;
  List<int> dummyList;
  dummyListList.setSize(this->allMonomials.size);
  dummyList.initializeFillInObject(this->allMonomials.size, - 1);
  this->firstNonZeroIndicesPerIntermediateSubtracand.initializeFillInObject(theSubtracands.size, 0);
  this->highlightMonsRemainders.initializeFillInObject(theRemainders.size,   dummyListList);
  this->highlightMonsSubtracands.initializeFillInObject(theSubtracands.size, dummyListList);
  this->highlightMonsQuotients.initializeFillInObject(basis.size,   dummyListList);
  this->highlightMonsDivisors.initializeFillInObject (basis.size,   dummyListList);
  this->fcAnswerMonsRemainders.initializeFillInObject(theRemainders.size,    dummyList);
  this->fcAnswerMonsSubtracands.initializeFillInObject(theSubtracands.size,  dummyList);
  this->fcAnswerMonsQuotients.initializeFillInObject(basis.size,    dummyList);
  this->fcAnswerMonsDivisors.initializeFillInObject (basis.size,    dummyList);
  this->uncoverAllMonsRemainders.initializeFillInObject (theRemainders.size, 1);
  this->uncoverAllMonsSubtracands.initializeFillInObject(theSubtracands.size, 1);
  this->uncoverAllMonsQuotients.initializeFillInObject  (basis.size, 1);
  this->uncoverAllMonsDivisors.initializeFillInObject   (basis.size, 1);
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
      &this->owner->thePolynomialOrder.monomialOrder
    );
  }
  this->owner->theFormat.flagUseLatex = true;
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
  for (int i = 0; i < this->owner->theBasis.size; i ++) {
    if (!oneDivisor) {
      out << this->getSpacedMonomialsWithHighlightLaTeX(
        this->owner->theBasis[i].element,
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
      this->owner->theQuotients[i],
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
          this->owner->theBasis[0].element,
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
    MonomialP tempM;
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
      thePoly[theIndex], thePoly.coefficients[theIndex], true, &this->owner->theFormat
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
  MacroRegisterFunctionWithName("GroebnerBasisComputation::computeHighLightsFromRemainder");
  this->checkInitialization();
  auto& basis = this->owner->theBasis;
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
    int theMonIndex = this->allMonomials.getIndex(
      this->intermediateHighlightedMons[remainderIndex][i]
    );
    this->additionalHighlightRemainders[remainderIndex][theMonIndex] = currentSlideNumber;
    this->additionalHighlightFinalRemainder[theMonIndex] = currentSlideNumber;
    this->uncoverMonsFinalRemainder[theMonIndex] = currentSlideNumber;
    currentSlideNumber ++;
  }
  MonomialP constMon;
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
  MonomialP divisorLeadingMonomial;
  int indexCurrentDivisorLeadingMoN = currentDivisor.getIndexLeadingMonomial(
    &divisorLeadingMonomial, nullptr, &this->owner->thePolynomialOrder.monomialOrder
  );
  int indexCurrentDivisorLeadingMonInAllMons = this->allMonomials.getIndex(
    divisorLeadingMonomial
  );
  MonomialP maxMonCurrentRemainder;
  Coefficient leadingCFCurrentRemainder;
  currentRemainder.getIndexLeadingMonomial(
    &maxMonCurrentRemainder, &leadingCFCurrentRemainder, &this->owner->thePolynomialOrder.monomialOrder
  );
  int indexCurrentRemainderLeadingMonInAllMons = this->allMonomials.getIndex(maxMonCurrentRemainder);
  this->highlightMonsDivisors[indexCurrentDivisor][indexCurrentDivisorLeadingMonInAllMons].addOnTop(currentSlideNumber);
  this->highlightMonsRemainders[remainderIndex][indexCurrentRemainderLeadingMonInAllMons].addOnTop(currentSlideNumber);

  if (remainderIndex == 0) {
    this->divisionLog << "$\\vphantom"
    << "{\\frac{x^1}{x^1}}$";
  }
  FormatExpressions& format = this->owner->theFormat;
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
  Polynomial<Coefficient>& currentQuotient = this->owner->theQuotients[indexCurrentDivisor];
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
