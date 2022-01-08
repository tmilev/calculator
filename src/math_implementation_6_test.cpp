// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "calculator.h"

bool PolynomialFactorizationFiniteFields::Test::all() {
  Test::test("x^2-1", "(x -1)(x +1)");
  return true;
}

bool PolynomialFactorizationFiniteFields::Test::test(
  const std::string& toFactor, const std::string& desiredResult
) {
  PolynomialFactorizationFiniteFields::Test::TestCase testCase;
  testCase.toBeFactored = toFactor;
  testCase.desiredFactorization = desiredResult;
  testCase.run();
  return true;
}

bool PolynomialFactorizationFiniteFields::Test::TestCase::run() {
  this->parser.initialize();
  Polynomial<Rational> input = this->parser.fromString(this->toBeFactored);
  PolynomialFactorizationUnivariate<Rational, PolynomialFactorizationFiniteFields> factorization;
  std::stringstream comments;
  if (!factorization.factor(input, &comments, &comments)) {
    global.fatal << "Factorization failed: "
    << this->toBeFactored << "\nComments:\n"
    << comments.str()
    << global.fatal;
  }
  std::string result = factorization.toStringResult(&this->parser.format);
  if (result != this->desiredFactorization) {
    global.fatal
    << "Factorization succeeded but results dont match. Got:\n"
    << result
    << "\nExpected:\n"
    << this->desiredFactorization
    << global.fatal;
  }
  return true;
}
