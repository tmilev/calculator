 // The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_calculator_functions_polynomial_ALREADY_INCLUDED
#define header_calculator_functions_polynomial_ALREADY_INCLUDED

#include "calculator_interface.h"

class CalculatorFunctionsPolynomial {
public:
  static bool polynomialDivisionSlidesGrLex(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool polynomialDivisionRemainder(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool polynomialDivisionQuotient(
    Calculator& calculator, const Expression& input, Expression& output
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
  );
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
  static bool groebnerLexicographicOpposite(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsPolynomial::groebner(
      calculator,
      input,
      output,
      MonomialPolynomial::Order::lexicographicOpposite
    );
  }
  static bool groebnerModZpLexicographic(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsPolynomial::groebner(
      calculator,
      input,
      output,
      MonomialPolynomial::Order::lexicographic,
      true
    );
  }
  static bool groebner(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    int order,
    bool useModZp = false
  );
  static bool divideExpressionsAsIfPolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool combineFractionsCommutativeWithInternalLibrary(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

#endif // header_calculator_functions_polynomial_ALREADY_INCLUDED
