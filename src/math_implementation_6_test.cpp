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
  PolynomialFactorizationUnivariate<Rational, PolynomialFactorizationFiniteFields> factorization;
  std::stringstream comments;
  Polynomial<Rational> input = this->parser.fromString(toBeFactored);
  int64_t start = global.getElapsedMilliseconds();
  if (!factorization.factor(input, &comments, &comments)) {
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
  << input.totalDegree() << " factored in " << duration << " ms." << Logger::endL;
  return true;
}
