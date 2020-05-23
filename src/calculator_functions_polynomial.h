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
  static bool factorPolynomial(Calculator& calculator, const Expression& input, Expression& output);
  static bool sylvesterMatrix(Calculator& calculator, const Expression& input, Expression& output);
};

#endif // vpfHeaderCalculatorFunctionsPolynomial

