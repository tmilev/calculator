// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "calculator.h"

bool PolynomialFactorizationFiniteFields::Test::all() {
  Test::test("x^2-1", "(x -1)(x +1)");
  Test::test(
    "1176 x^14-7224x^13-10506x^12-7434x^11+1247x^10+6085x^9+6195x^8"
    "+2607x^7+11577x^6+32x^5+7265x^4-2841x^3-1794x^2-1320x-2880",
    "(14x^{7}-103x^{6}-76x^{4}+19x^{3}+18x^{2}+9x +24)"
    "(84x^{7}+102x^{6}-75x^{4}-23x^{3}+19x^{2}-10x -120)"
  );
  Test::test(
    "4507104x^15+7359384x^14+1298256x^13-5390778x^12-1915130x^11+2884723x^10"
    "+321265x^9-7734159x^8-2806758x^7+1609059x^6-90547x^5-1699161x^4"
    "+23910x^3-1608x^2-302400x+138240",
    "(76x^{4}+19x^{3}-18x^{2}-9x +24)"
    "(706x^{4}+119x^{3}-18x^{2}+91x -48)"
    "(84x^{7}+102x^{6}-75x^{4}-23x^{3}+19x^{2}-10x -120)"
  );
  Test::test(
    "109x^13+9x^12+100x^11-98x^10+x^8 84x^7+12x^6-75x^4-23x^3+19x^2-10x-120",
    "(84x^{15}+109x^{13}+9x^{12}+100x^{11}-98x^{10}"
    "+12x^{6}-75x^{4}-23x^{3}+19x^{2}-10x -120)"
  );
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
  PolynomialFactorizationUnivariate<Rational> factorization;
  PolynomialFactorizationFiniteFields algorithm;
  std::stringstream comments;
  Polynomial<Rational> input = this->parser.fromString(toBeFactored);
  int64_t start = global.getElapsedMilliseconds();
  if (!factorization.factor(input, algorithm, &comments, &comments)) {
    global.fatal << "Factorization failed: "
    << this->toBeFactored << "\nComments:\n"
    << comments.str()
    << global.fatal;
  }
  int64_t duration = global.getElapsedMilliseconds() - start;
  std::string result = factorization.toStringResult(&this->parser.format);
  if (result != this->desiredFactorization) {
    global.fatal
    << "Factorization succeeded but results dont match. Got:\n"
    << result
    << "\nExpected:\n"
    << this->desiredFactorization
    << global.fatal;
  }
  Logger::LoggerSpecialSymbols color = Logger::green;
  if (duration > 100) {
    color = Logger::orange;
  }
  if (duration > 200) {
    color = Logger::red;
  }
  global << color << "Cantor-Zassenhaus: polynomial of degree: "
  << input.totalDegree() << " factored in "
  << duration << " ms." << Logger::endL;
  return true;
}

bool PolynomialUnivariateModular::Test::all() {
  PolynomialUnivariateModular::Test::division();
  PolynomialUnivariateModular::Test::greatestCommonDivisor();
  PolynomialUnivariateModular::Test::derivative();
  return true;
}

bool PolynomialUnivariateModular::Test::greatestCommonDivisor() {
  PolynomialUnivariateModular::Test::testOneGreatestCommonDivisor(
    5, "x^2-1", "x^3-1", "x +4 (mod 5)"
  );
  return true;
}

bool PolynomialUnivariateModular::Test::division() {
  PolynomialUnivariateModular::Test::testOneDivision(
    5, "x^3-1", "x-1", "x^{2}+x +1 (mod 5)", "0 (mod 5)"
  );
  return true;
}

bool PolynomialUnivariateModular::Test::derivative() {
  PolynomialUnivariateModular::Test::testOneDerivative(
    3, "x^2-1", "2x^{2} (mod 3)"
  );
  return true;
}

bool PolynomialUnivariateModular::Test::testOneGreatestCommonDivisor(
  int modulus,
  const std::string& left,
  const std::string& right,
  const std::string& expected
) {
  IntegerModulusSmall modulusData;
  modulusData.initializeModulusData(modulus);
  PolynomialUnivariateModular leftPolynomial =
  PolynomialUnivariateModular::Test::fromStringAndModulus(left, &modulusData);
  PolynomialUnivariateModular rightPolynomial =
  PolynomialUnivariateModular::Test::fromStringAndModulus(right, &modulusData);
  PolynomialUnivariateModular result;
  leftPolynomial.greatestCommonDivisor(leftPolynomial, rightPolynomial, result, nullptr);
  if (result.toString() != expected) {
    global.fatal << "Greatest common divisor of " << leftPolynomial.toString()
    << " and "
    << rightPolynomial.toString()
    << " equals:\n"
    << result.toString()
    << "\nexpected:\n"
    << expected
    << global.fatal;
  }
  return true;
}

bool PolynomialUnivariateModular::Test::testOneDivision(
  int modulusData,
  const std::string& dividend,
  const std::string& divisor,
  const std::string& expectedQuotient,
  const std::string& expectedRemainder
) {
  IntegerModulusSmall modulus;
  modulus.initializeModulusData(modulusData);
  PolynomialUnivariateModular dividendPolynomial =
  PolynomialUnivariateModular::Test::fromStringAndModulus(dividend, &modulus);
  PolynomialUnivariateModular divisorPolynomial =
  PolynomialUnivariateModular::Test::fromStringAndModulus(divisor, &modulus);
  PolynomialUnivariateModular remainder;
  PolynomialUnivariateModular quotient;
  dividendPolynomial.divideBy(divisorPolynomial, quotient, remainder);
  if (
    quotient.toString() != expectedQuotient ||
    remainder.toString() != expectedRemainder
  ) {
    global.fatal << "Dividing " << divisorPolynomial.toString()
    << " by "
    << divisorPolynomial.toString()
    << " gave quotient:\n"
    << quotient.toString()
    << "\nremainder:\n"
    << remainder.toString()
    << "\nexpected quotient:\n"
    << expectedQuotient
    << "\nexpected remainder:\n"
    << expectedRemainder
    << global.fatal;
  }
  return true;
}

bool PolynomialUnivariateModular::Test::testOneDerivative(
  int modulusData,
  const std::string& toBeDifferentiated,
  const std::string& expected
) {
  IntegerModulusSmall modulus;
  modulus.initializeModulusData(modulusData);
  PolynomialUnivariateModular input =
  PolynomialUnivariateModular::Test::fromStringAndModulus(toBeDifferentiated, &modulus);
  PolynomialUnivariateModular result;
  input.derivative(result);
  if (result.toString() != expected) {
    global.fatal << "Derivative of:\n" << input.toString()
    << "\ncomputed to be:\n"
    << result.toString()
    << "\nexpected:\n"
    << expected
    << global.fatal;
  }
  return true;
}

PolynomialUnivariateModular PolynomialUnivariateModular::Test::fromStringAndModulus(
  const std::string& input, IntegerModulusSmall* modulus
) {
  Polynomial<Rational> polynomialRational = Polynomial<Rational>::Test::fromString(input);
  PolynomialUnivariateModular result;
  result.makeZero(modulus);
  for (int i = 0; i < polynomialRational.size(); i ++) {
    ElementZmodP element;
    element.modulus = modulus->modulus;
    element.assignRational(polynomialRational.coefficients[i]);
    int coefficient = 0;
    element.value.isIntegerFittingInInt(&coefficient);
    result.addTerm(coefficient, polynomialRational.monomials[i].totalDegreeInt());
  }
  return result;
}
