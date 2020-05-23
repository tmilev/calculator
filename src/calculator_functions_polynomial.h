// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderCalculatorFunctionsPolynomial
#define vpfHeaderCalculatorFunctionsPolynomial
#include "calculator_interface.h"
class CalculatorFunctionsPolynomial {
public:
  static bool polynomialDivisionSlidesGrLex(Calculator& theCommands, const Expression& input, Expression& output);
  static bool polynomialDivisionRemainder(Calculator& theCommands, const Expression& input, Expression& output);
  static bool polynomialDivisionQuotient(Calculator& theCommands, const Expression& input, Expression& output);
  static bool polynomialDivisionVerbose(
    Calculator& theCommands,
    const Expression& input,
    Expression& output,
    List<MonomialP>::Comparator* monomialOrder
  );
  static bool polynomialDivisionVerboseGrLex(Calculator& theCommands, const Expression& input, Expression& output);
  static bool polynomialDivisionVerboseGradedReverseLexicographic(Calculator& theCommands, const Expression& input, Expression& output);
  static bool polynomialDivisionVerboseLexicographicOpposite(Calculator& theCommands, const Expression& input, Expression& output);
  static bool polynomialDivisionVerboseLexicographic(Calculator& theCommands, const Expression& input, Expression& output);
  static bool factorPolynomialModPrime(Calculator& theCommands, const Expression& input, Expression& output);
  static bool factorPolynomial(Calculator& theCommands, const Expression& input, Expression& output);
  static bool sylvesterMatrix(Calculator& theCommands, const Expression& input, Expression& output);
};

#endif // vpfHeaderCalculatorFunctionsPolynomial

