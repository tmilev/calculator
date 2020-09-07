// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderCalculatorFunctionsPolynomial
#define vpfHeaderCalculatorFunctionsPolynomial
#include "calculator_interface.h"
class CalculatorFunctionsPolynomial {
public:
  static bool polynomialDivisionSlidesGrLex(Calculator& calculator, const Expression& input, Expression& output);
  static bool polynomialDivisionRemainder(Calculator& calculator, const Expression& input, Expression& output);
  static bool polynomialDivisionQuotient(Calculator& calculator, const Expression& input, Expression& output);
  static bool polynomialDivisionVerbose(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    List<MonomialP>::Comparator* monomialOrder
  );
  static bool polynomialDivisionVerboseGrLex(Calculator& calculator, const Expression& input, Expression& output);
  static bool polynomialDivisionVerboseGradedReverseLexicographic(Calculator& calculator, const Expression& input, Expression& output);
  static bool polynomialDivisionVerboseLexicographicOpposite(Calculator& calculator, const Expression& input, Expression& output);
  static bool polynomialDivisionVerboseLexicographic(Calculator& calculator, const Expression& input, Expression& output);
  static bool factorPolynomialModPrime(Calculator& calculator, const Expression& input, Expression& output);
  static bool factorPolynomialKronecker(Calculator& calculator, const Expression& input, Expression& output);
  // Chooses between Kronecker / finite field factorization.
  static bool factorPolynomialRational(Calculator& calculator, const Expression& input, Expression& output);
  static bool factorPolynomialProcess(
    Calculator& calculator,
    WithContext<Polynomial<Rational> >& originalPolynomial,
    const Rational& constantFactor,
    List<Polynomial<Rational> >& factors,
    Expression& output
  );
  static bool factorPolynomialFiniteFields(Calculator& calculator, const Expression& input, Expression& output);
  static bool sylvesterMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool polynomialRelations(Calculator& calculator, const Expression& input, Expression& output);
  template<class Coefficient>
  static bool sylvesterMatrixFromPolynomials(
    Calculator& calculator,
    const List<Polynomial<Coefficient>> & polynomials,
    ExpressionContext* context,
    Expression& output
  );
};

#endif // vpfHeaderCalculatorFunctionsPolynomial

