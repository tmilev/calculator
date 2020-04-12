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
  std::string resultFactors = output.ToStringCommaDelimited(&this->format);
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
    "x+1", "x +1"
  );
  Polynomial<Rational>::Test::oneFactorization(
    "(2x+1)(-3x+1)", "-3x +1, 2x +1"
  );
  Polynomial<Rational>::Test::oneFactorization(
    "(1/2x+1/3)(-3/5x+1)", "-1/10x -1/15, 3x -5"
  );
  Polynomial<Rational>::Test::oneFactorization(
    "(x^2+3x+1)(-3x+1)", "3x -1, -x^{2}-3x -1"
  );
  Polynomial<Rational>::Test::oneFactorization(
    "(-3x^3-3x-5)(5x^3+x-7)", "-3x^{3}-3x -5, 5x^{3}+x -7"
  );
  Polynomial<Rational>::Test::oneFactorization(
    "(-3x^4+7x^3+2x^2-3x-5)(-1/2x^3+x^2+x-7)",
    "x^{3}-2x^{2}-2x +14, 3/2x^{4}-7/2x^{3}-x^{2}+3/2x +5/2"
  );

  Polynomial<Rational>::Test::oneFactorization(
    "10x^7+3x^6+5x^5-2x^4-x^3+x^2-4x+1",
    "10x^{7}+3x^{6}+5x^{5}-2x^{4}-x^{3}+x^{2}-4x +1"
  );
  return true;
}

template <>
bool Polynomial<Rational>::Test::all() {
  Polynomial<Rational>::Test tester;
  tester.initialize();

  tester.fromStringTest();
  tester.fromStringCommonContextTest();

  tester.leastCommonMultiple();
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
Vector<Polynomial<Rational> > Polynomial<Rational>::Test::fromStringCommonContext(
  const std::string& first, const std::string& second
) {
  List<std::string> input;
  input.AddOnTop(first);
  input.AddOnTop(second);
  return Polynomial<Rational>::Test::fromStringCommonContext(input);
}

template <>
Vector<Polynomial<Rational> > Polynomial<Rational>::Test::fromStringCommonContext(
  const List<std::string> & input
) {
  Vector<Polynomial<Rational> > result;
  if (input.size == 0) {
    return result;
  }
  Calculator parser;
  std::stringstream inputStream;
  inputStream << "(";
  for (int i = 0; i < input.size; i ++) {
    inputStream << "Polynomial(" + input[i] + ")";
    if (i != input.size - 1) {
      inputStream << ", ";
    }
  }
  inputStream << ")";
  parser.initialize();
  parser.Evaluate(inputStream.str());
  if (!parser.theProgramExpression.StartsWith(parser.opEndStatement())) {
    global.fatal
    << "Polynomial::fromString parsed: "
    << parser.theProgramExpression.toString()
    << " which was not expected. This function is not allowed to fail. "
    << global.fatal;
  }
  if (!parser.GetVectoR<Polynomial<Rational> >(
    parser.theProgramExpression[1], result
  )) {
    global.fatal
    << "Extracting vector from polynomial not allowed to fail. "
    << global.fatal;
  }
  return result;
}

template <>
bool Polynomial<Rational>::Test::fromStringCommonContextTest() {
  std::string expected = "x , y^{2}+y ";
  std::string inputFirst = "x";
  std::string inputSecond = "y^{2}+y";
  List<Polynomial<Rational> > underTest = Polynomial<Rational>::Test::fromStringCommonContext(inputFirst, inputSecond);
  std::string result = underTest.ToStringCommaDelimited(&this->format);
  if (result != expected) {
    global.fatal << "Polynomials with context from string: "
    << "input: " << inputFirst << ", " << inputSecond
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
  List<Polynomial<Rational> > converted = Polynomial<Rational>::Test::fromStringCommonContext(left, right);
  Polynomial<Rational> output;
  Polynomial<Rational>::leastCommonMultiple(
    converted[0], converted[1], output, Rational::one(), nullptr
  );
  std::string outputString = output.toString(&this->format);
  if (outputString != expected) {
    global.fatal << "Least common multiple of "
    << left << ", " << right
    << ", converted to: " << converted.ToStringCommaDelimited(&this->format)
    << " computed to be: [" << outputString
    << "], expected: [" << expected << "]. " << global.fatal;
  }
  return true;
}

template <>
bool Polynomial<Rational>::Test::leastCommonMultiple() {
  this->oneLeastCommonMultiple("x", "y", "x y ");
  return true;
}
