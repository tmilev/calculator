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
  one.makeOne();
  xOne.MakeEi(0, 1);
  xTwo.MakeEi(1, 1);
  xOneSquared.MakeEi(0, 2);
  xTwoSquared.MakeEi(1, 2);
  xOneXtwo = xOne;
  xOneXtwo *= xTwo;
  List<List<MonomialP>::Comparator> allOrders;
  allOrders.addOnTop(MonomialP::greaterThan_leftLargerWins);
  allOrders.LastObject()->name = "lexicographic";
  allOrders.addOnTop(MonomialP::greaterThan_rightLargerWins);
  allOrders.LastObject()->name = "lexicographic opposite";
  allOrders.addOnTop(MonomialP::greaterThan_totalDegree_leftLargerWins);
  allOrders.LastObject()->name = "graded lexicographic";
  allOrders.addOnTop(MonomialP::greaterThan_totalDegree_rightSmallerWins);
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
bool Polynomial<Rational>::Test::oneFactorizationKronecker(
  const std::string& input, const std::string& expectedFactors
) {
  Polynomial<Rational> toBeFactored = Polynomial<Rational>::Test::fromString(input);
  PolynomialFactorization<Rational, PolynomialFactorizationKronecker> factorization;
  bool success = factorization.factor(
    toBeFactored,
    nullptr
  );
  if (!success) {
    global.fatal << "Factorization of " << toBeFactored.toString() << " failed. " << global.fatal;
  }
  std::string resultFactors = factorization.toStringResult(&this->format);
  if (resultFactors != expectedFactors) {
    global.fatal << "While factoring: "
    << input << "=" << toBeFactored.toString()
    << " got factors: " << resultFactors
    << ", expected: " << expectedFactors << ". " << global.fatal;
  }
  return true;
}

template <>
bool Polynomial<Rational>::Test::factorizationKronecker() {
  Polynomial<Rational>::Test::oneFactorizationKronecker(
    "x+1", "(x +1)"
  );
  Polynomial<Rational>::Test::oneFactorizationKronecker(
    "(2x+1)(-3x+1)", "-(2x +1)(3x -1)"
  );
  Polynomial<Rational>::Test::oneFactorizationKronecker(
    "(1/2x+1/3)(-3/5x+1)", "-1/30(3x -5)(3x +2)"
  );
  Polynomial<Rational>::Test::oneFactorizationKronecker(
    "(x^2+3x+1)(-3x+1)", "-(3x -1)(x^{2}+3x +1)"
  );
  Polynomial<Rational>::Test::oneFactorizationKronecker(
    "(-3x^3-3x-5)(5x^3+x-7)", "-(3x^{3}+3x +5)(5x^{3}+x -7)"
  );
  Polynomial<Rational>::Test::oneFactorizationKronecker(
    "(-3x^4+7x^3+2x^2-3x-5)(-1/2x^3+x^2+x-7)",
    "1/2(x^{3}-2x^{2}-2x +14)(3x^{4}-7x^{3}-2x^{2}+3x +5)"
  );
  Polynomial<Rational>::Test::oneFactorizationKronecker(
    "10x^7+3x^6+5x^5-2x^4-x^3+x^2-4x+1",
    "(10x^{7}+3x^{6}+5x^{5}-2x^{4}-x^{3}+x^{2}-4x +1)"
  );
  return true;
}

template <>
bool Polynomial<Rational>::Test::all() {
  Polynomial<Rational>::Test tester;
  tester.initialize();

  tester.fromStringTest();
  tester.fromStringCommonContextTest();

  tester.differential();
  tester.leastCommonMultiple();
  tester.factorizationKronecker();
  return true;
}

template <>
void Polynomial<Rational>::Test::initialize() {
  this->format.polyAlphabeT.addOnTop("x");
  this->format.polyAlphabeT.addOnTop("y");
  this->format.polyAlphabeT.addOnTop("z");
  this->format.flagUseHTML = false;
}

template <>
Polynomial<Rational> Polynomial<Rational>::Test::fromString(const std::string& input) {
  Calculator parser;
  std::string inputModified = "Polynomial(" + input + ")";
  parser.initialize();
  parser.Evaluate(inputModified);
  if (!parser.theProgramExpression.startsWith(parser.opEndStatement())) {
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
  std::string expected = "y^{2}+x -y -1";
  std::string input = "x^2-1-x+b";
  Polynomial<Rational> underTest = Polynomial<Rational>::Test::fromString(input);
  std::string result = underTest.toString(&this->format);
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
  input.addOnTop(first);
  input.addOnTop(second);
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
  if (!parser.theProgramExpression.startsWith(parser.opEndStatement())) {
    global.fatal
    << "Polynomial::fromString parsed: "
    << parser.theProgramExpression.toString()
    << " which was not expected. This function is not allowed to fail. "
    << global.fatal;
  }
  if (!parser.getVector<Polynomial<Rational> >(
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
    << ", converted to:\n" << converted.ToStringCommaDelimited(&this->format)
    << "\ncomputed to be:\n[" << outputString
    << "]\nexpected:\n[" << expected << "]" << global.fatal;
  }
  return true;
}

template <>
bool Polynomial<Rational>::Test::leastCommonMultiple() {
  this->oneLeastCommonMultiple("x", "y", "x y ");
  this->oneLeastCommonMultiple(
    "(x^2y + 3 x + z y^2 - 1 + 7x^3)(x^3 + 5x y z + x y - z - 2)",
    "(x^2y + 3 x + z y^2 - 1 + 7x^3)(x^3 + x y z - 3y^2)",
    "7x^{9}+x^{8}y +42x^{7}y z +7x^{6}y^{2}z +35x^{5}y^{2}z^{2}"
    "+11x^{4}y^{3}z^{2}+5x^{2}y^{4}z^{3}+7x^{7}y -20x^{6}y^{2}"
    "-3x^{5}y^{3}+7x^{5}y^{2}z -103x^{4}y^{3}z -18x^{3}y^{4}z "
    "+x^{2}y^{4}z^{2}-15x y^{5}z^{2}+3x^{7}-7x^{6}z "
    "+17x^{5}y z -21x^{4}y^{3}-7x^{4}y z^{2}-3x^{3}y^{4}"
    "+13x^{3}y^{2}z^{2}-3x y^{5}z -x y^{3}z^{3}-15x^{6}"
    "+x^{5}y -9x^{4}y^{2}-20x^{4}y z +20x^{3}y^{2}z -42x^{2}y^{3}z "
    "-5x^{2}y^{2}z^{2}-2x y^{3}z^{2}+3y^{4}z^{2}-x^{4}y -3x^{4}z "
    "+45x^{3}y^{2}-3x^{2}y^{3}-x^{2}y^{2}z -3x^{2}y z^{2}+15x y^{3}z "
    "+6y^{4}z -6x^{4}+x^{3}z -6x^{2}y z +3x y^{3}+9x y^{2}z "
    "+x y z^{2}+2x^{3}+18x y^{2}+2x y z -3y^{2}z -6y^{2}"
  );
  return true;
}

template <>
bool Polynomial<Rational>::Test::oneDifferential(
  const std::string& input, const std::string& expected
) {
  Polynomial<Rational> inputPolynomial = Polynomial<Rational>::Test::fromString(input);
  Polynomial<Rational> output;
  inputPolynomial.differential(output, nullptr);
  std::string outputString = output.toString(&this->formatDifferentials);
  if (outputString != expected) {
    global.fatal << "Differential of " << input << " was computed to be: "
    << outputString << ", expected: " << expected << ". " << global.fatal;
  }
  return true;
}

template <>
bool Polynomial<Rational>::Test::differential() {
  this->formatDifferentials.polyAlphabeT.addOnTop("x");
  this->formatDifferentials.polyAlphabeT.addOnTop("y");
  this->formatDifferentials.polyAlphabeT.addOnTop("z");
  this->formatDifferentials.polyAlphabeT.addOnTop("dx");
  this->formatDifferentials.polyAlphabeT.addOnTop("dy");
  this->formatDifferentials.polyAlphabeT.addOnTop("dz");
  this->oneDifferential("1", "0");
  this->oneDifferential("x+y+z+1", "dx +dy +dz ");
  this->oneDifferential("x^2y^3z^5 + x", "5x^{2}y^{3}z^{4}dz +3x^{2}y^{2}z^{5}dy +2x y^{3}z^{5}dx +dx ");
  return true;
}
