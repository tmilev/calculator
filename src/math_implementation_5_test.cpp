// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "calculator.h"

bool MonomialP::Test::all() {
  return MonomialP::Test::TestMonomialOrdersSatisfyTheDefinition();
}

bool MonomialP::Test::TestMonomialOrdersSatisfyTheDefinitionOne(
  const MonomialP& mustBeSmaller,
  const MonomialP& mustBeLarger,
  List<MonomialP>::Comparator& order
) {
  if (!order.leftGreaterThanRight(mustBeLarger, mustBeSmaller)) {
    global.fatal << "Monomial: "
    << mustBeLarger.toString()
    << " is not larger than "
    << mustBeSmaller.toString()
    << " when using monomial: " << order.name
    << global.fatal;
  }
  return true;
}

bool MonomialP::Test::TestMonomialOrdersSatisfyTheDefinition() {
  MonomialP xOne, xTwo, xOneSquared, xTwoSquared, xOneXtwo, one;
  one.MakeOne();
  xOne.MakeEi(0, 1);
  xTwo.MakeEi(1, 1);
  xOneSquared.MakeEi(0, 2);
  xTwoSquared.MakeEi(1, 2);
  xOneXtwo = xOne;
  xOneXtwo *= xTwo;
  List<List<MonomialP>::Comparator> allOrders;
  allOrders.AddOnTop(MonomialP::greaterThan_leftLargerWins);
  allOrders.LastObject()->name = "lexicographic";
  allOrders.AddOnTop(MonomialP::greaterThan_rightLargerWins);
  allOrders.LastObject()->name = "lexicographic opposite";
  allOrders.AddOnTop(MonomialP::greaterThan_totalDegree_leftLargerWins);
  allOrders.LastObject()->name = "graded lexicographic";
  allOrders.AddOnTop(MonomialP::greaterThan_totalDegree_rightSmallerWins);
  allOrders.LastObject()->name = "graded reverse lexicographic";
  List<List<MonomialP> > elementsIncreasingOrder ({
    {one, xOne, xOneSquared},
    {one, xTwo, xTwoSquared},
    {one, xOne, xOneXtwo},
    {one, xTwo, xOneXtwo}
  });
  for (int i = 0; i < allOrders.size; i ++) {
    for (int j = 0; j < elementsIncreasingOrder.size; j ++) {
      for (int k = 0; k < elementsIncreasingOrder[i].size; k ++) {
        for (int l = k + 1; l < elementsIncreasingOrder[i].size; l ++) {
          MonomialP::Test::TestMonomialOrdersSatisfyTheDefinitionOne(
            elementsIncreasingOrder[j][k], elementsIncreasingOrder[j][l], allOrders[i]
          );
        }
      }
    }
  }
  return true;
}

template <>
bool Polynomial<Rational>::Test::oneFactorization(
  const std::string& input, const std::string& expectedFactors
) {
  Polynomial<Rational> toBeFactored = Polynomial<Rational>::Test::fromString(input);
  List<Polynomial<Rational> > output;
  bool success = toBeFactored.factorMe(output, nullptr);
  if (!success) {
    global.fatal << "Factorization of " << toBeFactored.toString() << " failed. " << global.fatal;
  }
  std::string resultFactors = output.ToStringCommaDelimited();
  if (resultFactors != expectedFactors) {
    global.fatal << "While factoring: "
    << input << "=" << toBeFactored.toString()
    << " got factors: " << resultFactors
    << ", expected: " << expectedFactors << ". " << global.fatal;
  }
  return true;
}

template <>
bool Polynomial<Rational>::Test::factorization() {
  Polynomial<Rational>::Test::oneFactorization(
    "x+1", "x_{1} +1"
  );
  Polynomial<Rational>::Test::oneFactorization(
    "(2x+1)(-3x+1)", "-3x_{1} +1, 2x_{1} +1"
  );
  Polynomial<Rational>::Test::oneFactorization(
    "(1/2x+1/3)(-3/5x+1)", "-1/10x_{1} -1/15, 3x_{1} -5"
  );
  Polynomial<Rational>::Test::oneFactorization(
    "(x^2+3x+1)(-3x+1)", "3x_{1} -1, -x_{1}^{2}-3x_{1} -1"
  );
  Polynomial<Rational>::Test::oneFactorization(
    "(-3x^3-3x-5)(5x^3+x-7)", "-3x_{1}^{3}-3x_{1} -5, 5x_{1}^{3}+x_{1} -7"
  );
  Polynomial<Rational>::Test::oneFactorization(
    "(-(1/2)x^4+7x^3+4x^2-3x-5)(11x^3+x^2+x-7)",
    "11x_{1}^{3}+x_{1}^{2}+x_{1} -7, -1/2x_{1}^{4}+7x_{1}^{3}+4x_{1}^{2}-3x_{1} -5"
  );

  Polynomial<Rational>::Test::oneFactorization(
    "10x^7+3x^6+5x^5-2x^4-x^3+x^2-4x+1",
    "10x_{1}^{7}+3x_{1}^{6}+5x_{1}^{5}-2x_{1}^{4}-x_{1}^{3}+x_{1}^{2}-4x_{1} +1"
  );
  return true;
}

template <>
bool Polynomial<Rational>::Test::all() {
  Polynomial<Rational>::Test tester;
  tester.initialize();

  tester.leastCommonMultiple();
  tester.fromStringTest();
  tester.factorization();
  return true;
}

template <>
void Polynomial<Rational>::Test::initialize() {
  this->format.polyAlphabeT.AddOnTop("x");
  this->format.polyAlphabeT.AddOnTop("y");
  this->format.polyAlphabeT.AddOnTop("z");
}

template <>
Polynomial<Rational> Polynomial<Rational>::Test::fromString(const std::string& input) {
  Calculator parser;
  std::string inputModified = "Polynomial(" + input + ")";
  parser.initialize();
  parser.Evaluate(inputModified);
  if (!parser.theProgramExpression.StartsWith(parser.opEndStatement())) {
    global.fatal
    << "Polynomial::fromString parsed: "
    << parser.theProgramExpression.toString()
    << " which was not expected. This function is not allowed to fail. "
    << global.fatal;
  }
  Polynomial<Rational> result;
  if (!parser.theProgramExpression[1].IsOfType(&result)) {
    global.fatal << "RationalFunction::fromString did not "
    << "produce a rational function, but instead: "
    << parser.theProgramExpression.toString()
    << global.fatal;
  }
  return result;
}

template <>
bool Polynomial<Rational>::Test::fromStringTest() {
  std::string expected = "x_{2}^{2}+x_{1} -x_{2} -1";
  std::string input = "x^2-1-x+b";
  Polynomial<Rational> underTest = Polynomial<Rational>::Test::fromString(input);
  std::string result = underTest.toString();
  if (result != expected) {
    global.fatal << "Polynomial from string: "
    << "input: " << input
    << ", result: "
    << result
    << ", expected: " << expected
    << "." << global.fatal;
  }
  return true;
}

template <>
bool Polynomial<Rational>::Test::oneLeastCommonMultiple(
  const std::string& left,
  const std::string& right,
  const std::string& expected
) {
  Polynomial<Rational> leftPolynomial = Polynomial<Rational>::Test::fromString(left);
  Polynomial<Rational> rightPolynomial = Polynomial<Rational>::Test::fromString(right);
  Polynomial<Rational> output;
  Polynomial<Rational>::leastCommonMultiple(
    leftPolynomial, rightPolynomial, output, Rational::one(), nullptr
  );
  if (output.toString() != expected) {
    global.fatal << "Least common multiple of "
    << left << ", " << right
    << " computed to be: " << output.toString(&this->format)
    << ", expected: " << expected << ". " << global.fatal;
  }
  return true;
}

template <>
bool Polynomial<Rational>::Test::leastCommonMultiple() {
  this->oneLeastCommonMultiple("x", "y", "x y");
  return true;
}
