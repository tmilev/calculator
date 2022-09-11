
#ifndef header_calculator_functions_polynomial_ALREADY_INCLUDED
#define header_calculator_functions_polynomial_ALREADY_INCLUDED

#include "calculator_interface.h"
#include "math_general_polynomial_computations_advanced_implementation.h"

class CalculatorFunctionsPolynomial {
public:
  template <class Type>
  static bool polynomialDivisionSlidesGrLex(
    Calculator& calculator, const Expression& input, Expression& output
  );
  template <class Type>
  static bool polynomialDivisionRemainderBuiltIn(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool polynomialDivisionRemainderAlgebraic(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool polynomialDivisionQuotient(
    Calculator& calculator, const Expression& input, Expression& output
  );
  template <class Coefficient>
  static bool polynomialDivisionVerbosePart2(
    Calculator& calculator,
    List<Polynomial<Coefficient> >& input,
    ExpressionContext& context,
    Expression& output,
    List<MonomialPolynomial>::Comparator* monomialOrder
  );
  static bool polynomialDivisionVerbose(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    List<MonomialPolynomial>::Comparator* monomialOrder
  );
  static bool polynomialDivisionVerboseGrLex(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool polynomialDivisionVerboseGradedReverseLexicographic(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool polynomialDivisionVerboseLexicographicOpposite(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool polynomialDivisionVerboseLexicographic(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool factorPolynomialModPrime(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool factorPolynomialFiniteFields(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsPolynomial::factorPolynomialFiniteFieldsWithComments(
      calculator, input, output, false
    );
  }
  static bool factorPolynomialFiniteFieldsWithComments(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool includeComments
  );
  static bool factorPolynomialFiniteFieldsDetailed(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsPolynomial::factorPolynomialFiniteFieldsWithComments(
      calculator, input, output, true
    );
  }
  static bool factorPolynomialKronecker(
    Calculator& calculator, const Expression& input, Expression& output
  );
  // Chooses between Kronecker / finite field factorization.
  static bool factorPolynomialKroneckerThenFiniteFields(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool factorPolynomialProcess(
    Calculator& calculator,
    WithContext<Polynomial<Rational> >& originalPolynomial,
    const Rational& constantFactor,
    List<Polynomial<Rational> >& factors,
    Expression& output
  );
  static bool sylvesterMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool polynomialRelations(
    Calculator& calculator, const Expression& input, Expression& output
  );
  template <class Coefficient>
  static bool sylvesterMatrixFromPolynomials(
    Calculator& calculator,
    const List<Polynomial<Coefficient> >& polynomials,
    ExpressionContext* context,
    Expression& output
  );
  template <class Coefficient>
  static bool greatestCommonDivisorOrLeastCommonMultiplePolynomialTypePartTwo(
    Calculator& calculator,
    const Polynomial<Coefficient>& left,
    const Polynomial<Coefficient>& right,
    const ExpressionContext& context,
    Expression& output,
    bool doGCD
  );
  static bool greatestCommonDivisorOrLeastCommonMultiplePolynomial(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool doGCD
  );
  static bool leastCommonMultiplePolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsPolynomial::
    greatestCommonDivisorOrLeastCommonMultiplePolynomial(
      calculator, input, output, false
    );
  }
  static bool greatestCommonDivisorPolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsPolynomial::
    greatestCommonDivisorOrLeastCommonMultiplePolynomial(
      calculator, input, output, true
    );
  }
  static bool greatestCommonDivisorOrLeastCommonMultipleAlgebraic(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool doGCD
  );
  static bool greatestCommonDivisorOrLeastCommonMultipleModular(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool doGCD
  );
  static bool groebnerGradedLexicographic(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsPolynomial::groebner(
      calculator,
      input,
      output,
      MonomialPolynomial::Order::gradedLexicographic
    );
  }
  static bool groebnerGradedReverseLexicographic(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsPolynomial::groebner(
      calculator,
      input,
      output,
      MonomialPolynomial::Order::gradedReverseLexicographic
    );
  }
  static bool groebnerLexicographic(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsPolynomial::groebner(
      calculator,
      input,
      output,
      MonomialPolynomial::Order::lexicographic
    );
  }
  static bool groebner(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    int order
  );
  static bool divideExpressionsAsIfPolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool combineFractionsCommutativeWithInternalLibrary(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

template <class Type>
bool CalculatorFunctionsPolynomial::polynomialDivisionRemainderBuiltIn(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsPolynomial::polynomialDivisionRemainderBuiltIn"
  );
  ExpressionContext context(calculator);
  Vector<Polynomial<Type> > polynomials;
  if (
    !CalculatorConversions::getListPolynomialVariableLabelsLexicographic(
      calculator, input, polynomials, context
    )
  ) {
    return false;
  }
  GroebnerBasisComputation<Type> computation;
  computation.polynomialOrder.monomialOrder =
  MonomialPolynomial::orderDefault();
  computation.flagStoreQuotients = true;
  for (int i = 1; i < polynomials.size; i ++) {
    if (polynomials[i].isEqualToZero()) {
      return output.assignError(calculator, "Division by zero.");
    }
    computation.addBasisElementNoReduction(polynomials[i]);
  }
  Polynomial<Type> outputRemainder;
  computation.remainderDivisionByBasis(
    polynomials[0], outputRemainder, - 1
  );
  return output.assignValueWithContext(calculator, outputRemainder, context);
}

template <class Type>
bool CalculatorFunctionsPolynomial::polynomialDivisionSlidesGrLex(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPolynomial::polynomialDivisionSlidesGrLex");
  ExpressionContext context(calculator);
  if (input.size() < 3) {
    return
    calculator
    << "Function takes at least 3 inputs: "
    << "index of first slide, dividend, divisor(s).";
  }
  int firstIndexLatexSlide = 0;
  if (!input[1].isSmallInteger(&firstIndexLatexSlide)) {
    return calculator << "Failed to extract integer from first argument";
  }
  Expression inputShifted = input;
  inputShifted.removeChildShiftDown(1);
  Vector<Polynomial<Type> > polynomialsRational;
  if (
    !CalculatorConversions::getListPolynomialVariableLabelsLexicographic(
      calculator, inputShifted, polynomialsRational, context
    )
  ) {
    return false;
  }
  GroebnerBasisComputation<Type> computation;
  computation.flagDoLogDivision = true;
  computation.flagStoreQuotients = true;
  computation.polynomialOrder.monomialOrder.setComparison(
    MonomialPolynomial::greaterThan_totalDegree_rightSmallerWins
  );
  for (int i = 1; i < polynomialsRational.size; i ++) {
    if (polynomialsRational[i].isEqualToZero()) {
      return output.assignError(calculator, "Division by zero.");
    }
    computation.addBasisElementNoReduction(polynomialsRational[i]);
  }
  computation.divisionReport.getElement().firstIndexLatexSlide =
  firstIndexLatexSlide;
  computation.remainderDivisionByBasis(
    polynomialsRational[0], computation.remainderDivision, - 1
  );
  context.getFormat(computation.format);
  computation.format.flagUseLatex = true;
  computation.format.flagUseFrac = true;
  computation.format.flagSuppressModP = true;
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
  return
  output.assignValue(
    calculator,
    HtmlRoutines::convertStringToHtmlString(latexOutput.str(), true)
  );
}

#endif // header_calculator_functions_polynomial_ALREADY_INCLUDED
