 // The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_extra_algebraic_numbers.h"

bool AlgebraicNumber::Test::all() {
  AlgebraicNumber::Test::constantValues();
  AlgebraicNumber::Test::hashFunction();
  return true;
}

bool AlgebraicNumber::Test::constantValues() {
  if (AlgebraicNumber::oneStatic().toString() != "1") {
    global.fatal << "Algebraic number 1 has unexpected value. " << global.fatal
    ;
  }
  if (AlgebraicNumber::zeroStatic().toString() != "0") {
    global.fatal << "Algebraic number 0 has unexpected value. " << global.fatal
    ;
  }
  return true;
}

bool AlgebraicNumber::Test::hashFunction() {
  AlgebraicClosureRationals closure;
  AlgebraicNumber one = closure.one();
  unsigned int hashOfOne = one.hashFunction();
  if (hashOfOne != Rational::one().hashFunction()) {
    global.fatal
    << "Rational numbers and their algebraic "
    << "counterparts must have the same hash function."
    << global.fatal;
  }
  AlgebraicNumber squareRootOfTwo;
  squareRootOfTwo.assignRationalQuadraticRadical(2, closure, nullptr);
  one.expressViaLatestBasis();
  if (hashOfOne != one.hashFunction()) {
    global.fatal
    << "The algebraic number field extension changed the hash function of 1. "
    << global.fatal;
  }
  AlgebraicNumber squareRootOfTwoPlusOne = one + squareRootOfTwo;
  if (hashOfOne != squareRootOfTwoPlusOne.hashFunction()) {
    global.fatal
    << "The hash functions of 1+sqrt(2) and 1 do not coincide "
    <<
    "They are expected to coincide with the current implementation of the hash function. "
    << global.fatal;
  }
  return true;
}
