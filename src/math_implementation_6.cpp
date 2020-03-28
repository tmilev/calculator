// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_polynomial_computations_advanced_implementation.h"

template <>
bool Polynomial<ElementZmodP>::FactorMeCantorZassenhaus(
  List<Polynomial<ElementZmodP> >& outputFactors, std::stringstream* comments
) const {
  return false;
}
