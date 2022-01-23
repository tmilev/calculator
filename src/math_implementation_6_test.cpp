// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "calculator.h"

bool PolynomialFactorizationFiniteFields::Test::all() {
  PolynomialFactorizationFiniteFields::Test::test(
    "1176 x^14-7224x^13-10506x^12-7434x^11+1247x^10+6085x^9+6195x^8"
    "+2607x^7+11577x^6+32x^5+7265x^4-2841x^3-1794x^2-1320x-2880",
    "(14x^{7}-103x^{6}-76x^{4}+19x^{3}+18x^{2}+9x +24)"
    "(84x^{7}+102x^{6}-75x^{4}-23x^{3}+19x^{2}-10x -120)"
  );
  PolynomialFactorizationFiniteFields::Test::test("x+1", "(x +1)");
  PolynomialFactorizationFiniteFields::Test::test("x^2+1", "(x^{2}+1)");
  PolynomialFactorizationFiniteFields::Test::test("x^2-1", "(x -1)(x +1)");
  PolynomialFactorizationFiniteFields::Test::test("x^2+2x+1", "(x +1)(x +1)");
  PolynomialFactorizationFiniteFields::Test::test(
    "4507104x^15+7359384x^14+1298256x^13-5390778x^12-1915130x^11+2884723x^10"
    "+321265x^9-7734159x^8-2806758x^7+1609059x^6-90547x^5-1699161x^4"
    "+23910x^3-1608x^2-302400x+138240",
    "(76x^{4}+19x^{3}-18x^{2}-9x +24)"
    "(706x^{4}+119x^{3}-18x^{2}+91x -48)"
    "(84x^{7}+102x^{6}-75x^{4}-23x^{3}+19x^{2}-10x -120)"
  );
  PolynomialFactorizationFiniteFields::Test::test(
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
  Polynomial<Rational> input = this->parser.fromString(this->toBeFactored);
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
  global << color << "Finite field factorization: polynomial of degree: "
  << input.totalDegree() << " factored in "
  << duration << " ms. ";
  if (algorithm.numberOfRunsOfFactor > 1) {
    global << "Total runs of factor: " << algorithm.numberOfRunsOfFactor << ". ";
  }
  global << "Cantor-Zassenhaus algorithm took: "
  << algorithm.millisecondsCantorZassenhaus << " ms. "
  << "Henself lift took: " << algorithm.millisecondsLift
  << " ms. Factorization from lift took: "
  << algorithm.millisecondsFactorizationFromLift
  << "ms. Factorization total: "
  << algorithm.millisecondsTotal << " ms. " << Logger::endL;
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
  PolynomialUnivariateModular::Test::testOneDivision(
    5, "x^3-1", "x", "x^{2} (mod 5)", "4 (mod 5)"
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

std::string PolynomialUnivariateModular::Test::toStringPolynomialElementZModP(
  const Polynomial<ElementZmodP>& other
) {
  FormatExpressions format;
  format.polynomialAlphabet.addOnTop("x");
  format.flagSuppressModP = true;
  std::stringstream out;
  out << other.toString(&format);
  if (other.size() > 0) {
    out << "(mod " << other.coefficients[0].modulus << ")";
  }
  return out.str();
}

Polynomial<ElementZmodP> PolynomialUnivariateModular::Test::fromStringAndModulus(
  const std::string& input, int modulus
) {
  IntegerModulusSmall modulusData;
  modulusData.initializeModulusData(modulus);
  PolynomialUnivariateModular modulusPolynomialInternal =
  PolynomialUnivariateModular::Test::fromStringAndModulus(input, &modulusData);
  Polynomial<ElementZmodP> result;
  modulusPolynomialInternal.toPolynomialNonDense(result);
  return result;
}

bool PolynomialUnivariateModularAsModulus::Test::all() {
  PolynomialUnivariateModularAsModulus::Test::oneTest(
    5,
    "x^{6}+3x^{4}+2x^{3}+2x^{2}+3x +1",
    "4, 2, 3, 3, 2, 0\n"
    "x^6 -> 4, 2, 3, 3, 2, 0\n"
    "x^7 -> 0, 4, 2, 3, 3, 2\n"
    "x^8 -> 3, 4, 0, 3, 2, 3\n"
    "x^9 -> 2, 4, 3, 4, 4, 2\n"
    "x^10 -> 3, 1, 0, 4, 3, 4\n"
    "x^11 -> 1, 1, 3, 2, 2, 3"
  );
  return true;
}

bool PolynomialUnivariateModularAsModulus::Test::oneTest(
  int modulus,
  const std::string& modulusPolynomial,
  const std::string& expectedImagesOfX
) {
  IntegerModulusSmall modulusData;
  modulusData.initializeModulusData(modulus);
  PolynomialUnivariateModular modulusPolynomialInternal =
  PolynomialUnivariateModular::Test::fromStringAndModulus(modulusPolynomial, &modulusData);
  PolynomialUnivariateModularAsModulus result;
  result = modulusPolynomialInternal;
  if (result.toStringImagesOfX() != expectedImagesOfX) {
    global.fatal << "Images of x: got:\n"
    << result.toStringImagesOfX()
    << "\nexpected:\n"
    << expectedImagesOfX
    << global.fatal;
  }
  return true;
}

bool PolynomialModuloPolynomialModuloInteger::Test::all() {
  PolynomialModuloPolynomialModuloInteger::Test::product();
  return true;
}

bool PolynomialModuloPolynomialModuloInteger::Test::product() {
  PolynomialModuloPolynomialModuloInteger::Test::testOneProduct(
    5,
    "x^2",
    "x^3",
    "x^2+2x+3",
    "4x +3 (mod 5)"
  );
  PolynomialModuloPolynomialModuloInteger::Test::testOneProduct(
    5,
    "x +4",
    "x +4",
    "x^{6}+3x^{4}+2x^{3}+2x^{2}+3x +1",
    "x^{2}+3x +1 (mod 5)"
  );
  PolynomialModuloPolynomialModuloInteger::Test::testOneProduct(
    5,
    "x^3",
    "x^3",
    "x^{6}+3x^{4}+2x^{3}+2x^{2}+3x +1",
    "2x^{4}+3x^{3}+3x^{2}+2x +4 (mod 5)"
  );
  return true;
}

bool PolynomialModuloPolynomialModuloInteger::Test::testOneProduct(
  int modulus,
  const std::string& left,
  const std::string& right,
  const std::string& modulusPolynomial,
  const std::string& expected
) {
  IntegerModulusSmall modulusData;
  modulusData.initializeModulusData(modulus);
  Polynomial<ElementZmodP> leftPolynomial =
  PolynomialUnivariateModular::Test::fromStringAndModulus(left, modulus);
  Polynomial<ElementZmodP> rightPolynomial =
  PolynomialUnivariateModular::Test::fromStringAndModulus(right, modulus);
  PolynomialUnivariateModular modulusPolynomialInternal =
  PolynomialUnivariateModular::Test::fromStringAndModulus(modulusPolynomial, &modulusData);
  PolynomialUnivariateModularAsModulus modulusInternal;
  modulusInternal = modulusPolynomialInternal;
  PolynomialModuloPolynomialModuloInteger leftElement;
  PolynomialModuloPolynomialModuloInteger rightElement;
  leftElement.makeFromModulusAndValue(&modulusInternal, leftPolynomial);
  rightElement.makeFromModulusAndValue(&modulusInternal, rightPolynomial);
  leftElement *= rightElement;
  FormatExpressions format;
  format.flagSuppressModP = true;
  format.polynomialAlphabet.addOnTop("x");
  if (leftElement.value.toString(&format) != expected) {
    global.fatal << "Product of " << leftPolynomial.toString(&format)
    << " and " << rightPolynomial.toString(&format) << " mod "
    << modulusPolynomialInternal.toString(&format)
    << " computed as:\n" << leftElement.value.toString(&format)
    << "\nexpected:\n"
    << expected
    << global.fatal;
  }
  return true;
}

bool PolynomialConversions::Test::all() {
  PolynomialConversions::Test::univariateModularToDense();
  return true;
}

bool PolynomialConversions::Test::univariateModularToDense() {
  oneUnivariateModularToDense(5, "x^2+3x-1", "x^{2}+3x +4(mod 5)");
  return true;
}

bool PolynomialConversions::Test::oneUnivariateModularToDense(
  int modulus,
  const std::string& input,
  const std::string& expected
) {
  IntegerModulusSmall modulusData;
  modulusData.initializeModulusData(modulus);
  PolynomialUnivariateModular inputPolynomial =
  PolynomialUnivariateModular::Test::fromStringAndModulus(input, &modulusData);
  Polynomial<ElementZmodP> resultPolynomial;
  PolynomialConversions::convertToPolynomial(inputPolynomial, resultPolynomial);
  std::string result = PolynomialUnivariateModular::Test::toStringPolynomialElementZModP(resultPolynomial);
  if (result != expected) {
    global.fatal << "Converted "
    << inputPolynomial.toString() << " to: "
    << result
    << "\nexpected:\n" << expected << global.fatal;
  }
  return true;
}

template <>
bool PolynomialFactorizationCantorZassenhaus<
  PolynomialModuloPolynomialModuloInteger,
  PolynomialUnivariateModular,
  PolynomialUnivariateModularAsModulus
>
::Test::testOnce(
  int modulus,
  const std::string& input,
  const std::string& expected
) {
  Polynomial<ElementZmodP> polynomial =
  PolynomialUnivariateModular::Test::fromStringAndModulus(input, modulus);
  PolynomialFactorizationCantorZassenhaus<
    PolynomialModuloPolynomialModuloInteger,
    PolynomialUnivariateModular,
    PolynomialUnivariateModularAsModulus
  > algorithm;
  PolynomialUnivariateModular zero;
  IntegerModulusSmall modulusData;
  modulusData.initializeModulusData(modulus);
  zero.makeZero(&modulusData);
  algorithm.initialize(zero);
  PolynomialFactorizationUnivariate<ElementZmodP> factorization;
  std::stringstream comments;
  if (!factorization.factor(polynomial, algorithm, &comments, &comments)) {
    global.fatal << "Unexpected failure to factor: "
    << PolynomialUnivariateModular::Test::toStringPolynomialElementZModP(polynomial)
    << global.fatal;
  }
  std::string resultString = factorization.toStringResult();
  if (resultString != expected) {
    global.fatal
    << "Factorization of "
    << PolynomialUnivariateModular::Test::toStringPolynomialElementZModP(polynomial)
    << " mod "
    << modulus
    << ":\n"
    << resultString
    << "\nexpected:\n"
    << expected
    << global.fatal;
  }
  return true;
}

template <>
bool PolynomialFactorizationCantorZassenhaus<
  PolynomialModuloPolynomialModuloInteger,
  PolynomialUnivariateModular,
  PolynomialUnivariateModularAsModulus
>
::Test::testOneStartingPolynomial(
  int modulus,
  int constant,
  int currentDegree,
  const std::string& expected
) {
  MacroRegisterFunctionWithName("PolynomialFactorizationFiniteFields::testOneStartingPolynomial");
  IntegerModulusSmall modulusData;
  modulusData.initializeModulusData(modulus);
  PolynomialUnivariateModular one;
  one.makeZero(&modulusData);
  PolynomialFactorizationCantorZassenhaus<
    PolynomialModuloPolynomialModuloInteger,
    PolynomialUnivariateModular,
    PolynomialUnivariateModularAsModulus
  > algorithm;
  algorithm.current.makeZero(&modulusData);
  algorithm.current.addTerm(1, currentDegree);
  algorithm.one.makeFrom(modulus, 1);
  PolynomialModuloPolynomialModuloInteger result;
  result.value.makeZero(&modulusData);
  algorithm.initialize(algorithm.current);
  algorithm.initializeOneFactorComputation();
  algorithm.computeStartingPolynomial(constant, result);
  if (result.value.toString() != expected) {
    global.fatal << "Compute starting polynomial, modulus:\n"
    << modulus
    << "\nconstant:\n"
    << constant
    << "\ngot:\n"
    << result.value.toString()
    << "\nexpected:\n"
    << expected
    << global.fatal;
  }
  return true;
}

template <>
bool PolynomialFactorizationCantorZassenhaus<
  PolynomialModuloPolynomialModuloInteger,
  PolynomialUnivariateModular,
  PolynomialUnivariateModularAsModulus
>
::Test::constructStartingPolynomial() {
  Test::testOneStartingPolynomial(
    3,
    0,
    2,
    "x  (mod 3)"
  );
  Test::testOneStartingPolynomial(
    3,
    1,
    2,
    "x +1 (mod 3)"
  );
  Test::testOneStartingPolynomial(
    3,
    2,
    2,
    "x +2 (mod 3)"
  );
  Test::testOneStartingPolynomial(
    3,
    2,
    3,
    "x +2 (mod 3)"
  );
  Test::testOneStartingPolynomial(
    3,
    3,
    3,
    "x^{2}+1 (mod 3)"
  );
  Test::testOneStartingPolynomial(
    3,
    4,
    3,
    "x^{2}+2 (mod 3)"
  );
  Test::testOneStartingPolynomial(
    3,
    5,
    3,
    "x +1 (mod 3)"
  );
  Test::testOneStartingPolynomial(
    3,
    3,
    2,
    "x +1 (mod 3)"
  );
  Test::testOneStartingPolynomial(
    3,
    7,
    2,
    "x +1 (mod 3)"
  );
  return true;
}

template <>
bool PolynomialFactorizationCantorZassenhaus<
  PolynomialModuloPolynomialModuloInteger,
  PolynomialUnivariateModular,
  PolynomialUnivariateModularAsModulus
>
::Test::all() {
  Test::constructStartingPolynomial();
  Test::testOnce(
    11,
    "x^3+3x+7",
    "(x +10)(x^{2}+x +4)"
  );
  Test::testOnce(
    5,
    "x^{6}+3 x^{4}+2 x^{3}+2 x^{2}+3 x+1",
    "(x^{3}+x +1)(x^{3}+2x +1)"
  );
  Test::testOnce(
    17,
    "x^{6}+2 x^{5}+3 x^{4}+16 x^{3}+10 x^{2}+15 x+8",
    "(x^{3}+3x +5)(x^{3}+2x^{2}+5)"
  );
  Test::testOnce(
    11,
    "10x^{14}+3x^{13}+10x^{12}+2x^{11}+4x^{10}"
    "+2x^{9}+2x^{8}+5x^{6}+10x^{5}+5x^{4}+8x^{3}+10x^{2}+2",
    "10(x +4)(x^{2}+5x +2)"
    "(x^{2}+6x +2)"
    "(x^{4}+7x^{3}+3x^{2}+4x +1)"
    "(x^{5}+8x^{4}+2x^{3}+x^{2}+5x +4)"
  );
  return true;
}
