// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_extra_algebraic_numbers.h"

bool AlgebraicNumber::Test::all() {
  AlgebraicNumber::Test::constantValues();
  return true;
}

bool AlgebraicNumber::Test::constantValues() {
  if (AlgebraicNumber::oneStatic().toString() != "1") {
    global.fatal << "Algebraic number 1 has unexpected value. " << global.fatal;
  }
  if (AlgebraicNumber::zeroStatic().toString() != "0") {
    global.fatal << "Algebraic number 0 has unexpected value. " << global.fatal;
  }
  return true;
}
