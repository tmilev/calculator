// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_math_extra_polynomial_factorization_ALREADY_INCLUDED
#define header_math_extra_polynomial_factorization_ALREADY_INCLUDED
#include "general_lists.h"

class IntegerModulusSmall {
public:
  int modulus;
  List<int> inverses;
};

// Dense univariate polynomial mod p for a small integer p.
// Intended for very fast computations.
class PolynomialUnivariateModular {
public:
  List<int> coefficients;
  IntegerModulusSmall* modulus;
  PolynomialUnivariateModular();
  PolynomialUnivariateModular(IntegerModulusSmall* modulus);
};

#endif // header_math_extra_polynomial_factorization_ALREADY_INCLUDED
