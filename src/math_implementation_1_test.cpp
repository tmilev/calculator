// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general.h"
#include "crypto.h"
#include "math_extra_algebraic_numbers.h"
#include "calculator.h"

bool LargeIntegerUnsigned::Test::serializationToHex(const LargeIntegerUnsigned& input) {
  MacroRegisterFunctionWithName("LargeIntUnsigned::Test::serializationToHex");
  std::string resultCryptoHex, resultByteSerializationHex;
  if (!Crypto::convertLargeUnsignedToHexSignificantDigitsFirst(input, 0, resultCryptoHex)) {
    global.fatal << "Function Crypto::convertLargeUnsignedToHexSignificantDigitsFirst is not supposed to return false. " << global.fatal;
  }
  List<unsigned char> serialization;
  input.writeBigEndianBytes(serialization, true);
  resultByteSerializationHex = Crypto::convertListUnsignedCharsToHex(serialization);
  if (resultByteSerializationHex != resultCryptoHex) {
    global.fatal << "Byte serialization hex: " << resultByteSerializationHex
    << " not equal to crypto hex conversion: "
    << resultCryptoHex << ". "
    << global.fatal;
  }
  return true;
}

bool LargeIntegerUnsigned::Test::all() {
  MacroRegisterFunctionWithName("LargeIntegerUnsigned::Test::all");
  LargeIntegerUnsigned::Test::serializationToHex(LargeIntegerUnsigned(100));
  LargeIntegerUnsigned::Test::comparisons();
  LargeIntegerUnsigned::Test::factor();
  return true;
}

bool LargeIntegerUnsigned::Test::comparisons() {
  LargeIntegerUnsigned x = 1;
  LargeIntegerUnsigned y = 0;
  List<LargeIntegerUnsigned> toTest = {x, y};
  for (int i = 0; i < toTest.size; i ++) {
    LargeIntegerUnsigned& current = toTest[i];
    if (!current.isGreaterThanOrEqualTo(current)) {
      global.fatal << "Number: " << current
      << " not greater than or equal to itself. " << global.fatal;
    }
  }
  return true;
}

bool LargeIntegerUnsigned::Test::factor() {
  LargeIntegerUnsigned::Test::factorSmall(120, "2, 3, 5", "3, 1, 1", 100, 100, 3);
  LargeIntegerUnsigned::Test::factorSmall(120, "2, 3, 5", "3, 1, 1", 0, 100, 3);
  return true;
}

bool LargeIntegerUnsigned::Test::factorSmall(
  const LargeIntegerUnsigned& input,
  const std::string& expectedFactors,
  const std::string& expectedMultiplicities,
  int maximumDivisorToTry,
  int numberMillerRabinRuns,
  int64_t maximumRunningTime
) {
  List<LargeInteger> factors;
  List<int> multiplicitiesInt;
  int64_t millisecondsStart = global.getElapsedMilliseconds();
  input.factor(factors, multiplicitiesInt, maximumDivisorToTry, numberMillerRabinRuns, nullptr);
  int64_t elapsed = global.getElapsedMilliseconds() - millisecondsStart;
  if (elapsed > maximumRunningTime) {
    global.fatal << "Factoring " << input << " with maximumDivisorToTry: " << maximumDivisorToTry
    << " and " << numberMillerRabinRuns << " Miller-rabin runs took "
    << elapsed << " ms, maximum allowed: "
    << maximumRunningTime << " ms. " << global.fatal;
  }
  List<LargeInteger> multiplicities;
  multiplicities = multiplicitiesInt;
  std::stringstream out;
  out << "Factoring: " << input << ". Got factors: "
  << expectedFactors << " with multiplicities: "
  << multiplicities.toStringCommaDelimited() << ". Expected: "
  << expectedFactors << " with multiplicities: " << expectedMultiplicities << ". ";
  if (
    factors.toStringCommaDelimited() != expectedFactors ||
    multiplicities.toStringCommaDelimited() != expectedMultiplicities
  ) {
    global.fatal << "Bad factorization. " << out.str() << global.fatal;
  }
  return true;
}


bool Rational::Test::all() {
  Rational::Test::testScale();
  return true;
}

class TestRationalScale {
public:
  List<Rational> vector;
  int distinguishedIndex;
  std::string expected;
  TestRationalScale(
    const List<std::string>& inputVector, int inputIndex, const std::string& inputExpected
  );
  TestRationalScale() {
  }
};

TestRationalScale::TestRationalScale(
  const List<std::string>& inputVector,
  int inputIndex,
  const std::string& inputExpected
) {
  this->vector.setSize(inputVector.size);
  for (int i = 0; i < inputVector.size; i ++) {
    this->vector[i] = Rational(inputVector[i]);
  }
  this->distinguishedIndex = inputIndex;
  this->expected = inputExpected;
}

bool Rational::Test::testScale() {
  List<TestRationalScale> toTest = {
    TestRationalScale({"1/2", "2/3"}, 0, "3, 4"),
    TestRationalScale({"-1/2", "2/3"}, 0, "3, -4")
  };
  for (int i = 0; i < toTest.size; i ++) {
    TestRationalScale& current = toTest[i];
    List<Rational> copy = current.vector;
    Rational::scaleNormalizeIndex(copy, current.distinguishedIndex);
    if (copy.toStringCommaDelimited() != current.expected) {
      global.fatal << "Vector " << current.vector.toStringCommaDelimited()
      << " scaled to " << copy.toStringCommaDelimited()
      << " instead of the expected: " << current.expected
      << global.fatal;
    }
  }
  return true;
}

bool ElementZmodP::Test::all() {
  ElementZmodP::Test::basicOperations();
  ElementZmodP::Test::scale();
  return true;
}

bool ElementZmodP::Test::basicOperations() {
  ElementZmodP x, y;
  x.theModulus = 5;
  y.theModulus = 5;
  x.assignRational(Rational("-1/2"));
  y.assignRational(Rational("3"));
  ElementZmodP z = y;
  z *= x;
  if (!(z.theValue == 1)) {
    global.fatal << "Bad arithmetic: " << x.toString() << " * "
    << y.toString() << " equals: " << z.toString() << ". " << global.fatal;
  }
  return true;
}

bool ElementZmodP::Test::scale() {
  ElementZmodP x, y, z;
  LargeIntegerUnsigned modulus = 7;
  x.makeOne(modulus);
  y.makeOne(modulus);
  z.makeOne(modulus);
  x = 2;
  y = 0;
  z = 3;
  List<ElementZmodP> theList = {x, y, z};
  ElementZmodP scale = ElementZmodP::scaleNormalizeIndex(theList, 0);
  std::string scaled = theList.toStringCommaDelimited();
  std::string expected = "(1 mod (7)), (0 mod (7)), (5 mod (7))";
  if (scaled != expected) {
    global.fatal << "Bad scaling, got: " << scaled << ", expected: " << expected << ". " << global.fatal;
  }
  if (scale.theModulus != 4) {
    global.fatal << "Bad scaling, scale expected to be 4. ";
  }
  return true;
}

template<>
RationalFunction<Rational> RationalFunction<Rational>::Test::fromString(const std::string& input) {
  Calculator parser;
  std::string inputModified = "MakeRationalFunction(" + input + ")";
  parser.initialize();
  parser.evaluate(inputModified);
  if (!parser.theProgramExpression.startsWith(parser.opEndStatement())) {
    global.fatal
    << "RationalFunction::fromString parsed: "
    << parser.theProgramExpression.toString()
    << " which was not expected. This function is not allowed to fail. "
    << global.fatal;
  }
  RationalFunction<Rational> result;
  if (!parser.theProgramExpression[1].isOfType(&result)) {
    global.fatal << "RationalFunction::fromString did not "
    << "produce a rational function, but instead: "
    << parser.theProgramExpression.toString()
    << global.fatal;
  }
  return result;
}

template<>
bool RationalFunction<Rational>::Test::all() {
  RationalFunction<Rational>::Test::fromStringTest();
  RationalFunction<Rational>::Test::scaleNormalizeIndex();
  return true;
}

template<>
bool RationalFunction<Rational>::Test::fromStringTest() {
  MacroRegisterFunctionWithName("RationalFunction::Test::fromString");
  std::string input = "(a^2+7b)/(2+d*c)";
  std::string expected = "(x_{1}^{2}+7x_{2} )/(x_{3} x_{4} +2)";
  RationalFunction<Rational> underTest = RationalFunction<Rational>::Test::fromString(input);
  if (underTest.toString() != expected) {
    global.fatal << "Input: " << input << " parsed as: "
    << underTest.toString() << ", expected: "
    << expected << ". " << global.fatal;
  }
  return true;
}

template<>
bool RationalFunction<Rational>::Test::scaleNormalizeIndex() {
  RationalFunction<Rational> a = RationalFunction<Rational>::Test::fromString("(a+1/2)/(b+1/3)");
  RationalFunction<Rational> b = RationalFunction<Rational>::Test::fromString("2a/5");
  RationalFunction<Rational> c = RationalFunction<Rational>::Test::fromString("3a/(7x)");
  List<List<RationalFunction<Rational> > > toScale = {
    {a},
    {a, b},
    {a, b, c}
  };
  List<std::string> expected = {
  "(2x_{1} +1)/(3x_{2} +1)",
  "(30x_{1} +15)/(3x_{2} +1), 4x_{1} ",
  "(210x_{1} +105)/(3x_{2} +1), 28x_{1} , 30x_{1} /(x_{2} )"
  };
  for (int i = 0; i < toScale.size; i ++) {
    std::string atStart = toScale[i].toStringCommaDelimited();
    RationalFunction::scaleNormalizeIndex(toScale[i], 0);
    if (toScale[i].toStringCommaDelimited() != expected[i]) {
      global.fatal << "Scaling rational functions: " << atStart
      << " produced: " << toScale[i].toStringCommaDelimited()
      << " instead of the expected: " << expected[i] << global.fatal;
    }
  }
  return true;
}
