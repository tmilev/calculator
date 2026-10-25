#include "calculator_interface.h"
#include "crypto_calculator.h"
#include "math_general.h"
#include "math_general_polynomial_computations_basic_implementation.h" // IWYU pragma: keep: breaks g++ -03 optimization build.
#include "math_rational_function.h"
#include "test.h"

namespace Testing {

bool LargeIntegerUnsignedTest::karatsubaMultiplication() {
  STACK_TRACE("LargeIntegerUnsignedTest::karatsubaMultiplication");
  LargeIntegerUnsignedTest::karatsubaMultiplicationOnce("0", "0", "0");
  LargeIntegerUnsignedTest::karatsubaMultiplicationOnce("1", "0", "0");
  LargeIntegerUnsignedTest::karatsubaMultiplicationOnce("0", "1", "0");
  LargeIntegerUnsignedTest::karatsubaMultiplicationOnce("1", "1", "1");
  LargeIntegerUnsignedTest::karatsubaMultiplicationOnce(
    "123456789", "987654321", "121932631112635269"
  );
  LargeIntegerUnsignedTest::multiplicationLargePowers();
  return true;
}

bool LargeIntegerUnsignedTest::multiplicationLargePowers() {
  STACK_TRACE(
    "LargeIntegerUnsignedTest::"
    "karatsubaMultiplicationLargePowers"
  );
  List<LargeIntegerUnsignedTest::LargePowerTest> results;
  for (int i = 1; i <= 10; i ++) {
    multiplicationLargePowerOnce(123, i, ALL, results);
  }
  for (int i = 11; i < 16; i ++) {
    multiplicationLargePowerOnce(123, i, KARATSUBA_ONLY, results);
  }
  return true;
}

bool LargeIntegerUnsignedTest::multiplicationLargePowerOnce(
  uint32_t base,
  int iterations,
  LargeIntegerUnsignedTest::LargePowerTestType testType,
  List<LargeIntegerUnsignedTest::LargePowerTest>& output
) {
  List<LargeIntegerUnsigned::MultiplicationAlgorithm> algorithms;
  algorithms.addOnTop(
    LargeIntegerUnsigned::MultiplicationAlgorithm::KARATSUBA
  );
  if (testType == LargeIntegerUnsignedTest::LargePowerTestType::ALL) {
    algorithms.addOnTop(
      LargeIntegerUnsigned::MultiplicationAlgorithm::DIGIT_BY_DIGIT
    );
  }
  LargeIntegerUnsignedTest::LargePowerTest result(base, iterations);
  for (
    const LargeIntegerUnsigned::MultiplicationAlgorithm& algorithm :
    algorithms
  ) {
    LargeIntegerUnsignedTest::multiplicationLargePowerOnceOneAlgorithm(
      result, algorithm
    );
  }
  result.checkPerformance();
  output.addOnTop(result);
  return true;
}

bool LargeIntegerUnsignedTest::division() {
  divisionOnce(
    "865824543292632094632409683200683659928970742272",
    "9956358115699982336",
    "86961972764652836093113913948",
    "7619180909546719744"
  );
  divisionOnce("1", "2", "0", "1");
  divisionOnce("1", "1", "1", "0");
  divisionOnce("2", "2", "1", "0");
  divisionOnce("10000000000000", "1", "10000000000000", "0");
  return true;
}

bool LargeIntegerUnsignedTest::divisionOnce(
  const std::string& dividend,
  const std::string& divisor,
  const std::string& expectedQuotient,
  const std::string& expectedRemainder
) {
  STACK_TRACE("LargeIntegerUnsignedTest::divisionOnce");
  LargeIntegerUnsigned dividendInteger(dividend);
  LargeIntegerUnsigned divisorInteger(divisor);
  LargeIntegerUnsigned quotient;
  LargeIntegerUnsigned remainder;
  dividendInteger.dividePositive(divisor, quotient, remainder);
  if (
    quotient.toString() != expectedQuotient ||
    remainder.toString() != expectedRemainder
  ) {
    global.fatal
    << "Division yielded unexpected results. "
    << "\nDividend: "
    << dividendInteger.toString()
    << "\nDivisor: "
    << divisorInteger.toString()
    << "\nActual   quotient: "
    << quotient.toString()
    << "\nExpected quotient: "
    << expectedQuotient
    << "\nActual   remainder: "
    << remainder.toString()
    << "\nExpected remainder: "
    << expectedRemainder
    << global.fatal;
  }
  return true;
}

bool LargeIntegerUnsignedTest::multiplicationLargePowerOnceOneAlgorithm(
  LargeIntegerUnsignedTest::LargePowerTest& testCase,
  LargeIntegerUnsigned::MultiplicationAlgorithm algorithm
) {
  LargeIntegerUnsigned result = testCase.base;
  int64_t start = global.getElapsedMilliseconds();
  for (int i = 0; i < testCase.iterations; i ++) {
    switch (algorithm) {
    case LargeIntegerUnsigned::MultiplicationAlgorithm::DIGIT_BY_DIGIT:
      result.multiplyByDigitByDigit(result, result);
      break;
    case LargeIntegerUnsigned::MultiplicationAlgorithm::KARATSUBA:
      LargeIntegerUnsigned::karatsubaProduct(result, result, result);
      break;
    }
  }
  LargeIntegerUnsignedTest::LargePowerTestDuration duration;
  duration.algorithm = algorithm;
  duration.duration = global.getElapsedMilliseconds() - start;
  testCase.durations.addOnTop(duration);
  if (testCase.durations.size == 1) {
    testCase.result = result;
  } else if (testCase.result != result) {
    global.fatal
    << "Rasing base^{2^iterations} with base="
    << testCase.base
    << ", iterations="
    << testCase.iterations
    << " gave different output for algorithm: "
    << LargeIntegerUnsigned::multiplicationAlgorithmName(algorithm)
    << global.fatal;
  }
  return true;
}

bool LargeIntegerUnsignedTest::LargePowerTest::checkPerformance() {
  int digits = result.digits.size;
  double minimumSpeed = 1;
  double recommendedSpeed = 10000;
  for (
    const LargeIntegerUnsignedTest::LargePowerTestDuration& duration :
    this->durations
  ) {
    double speed = static_cast<double>(digits) / duration.duration;
    std::string algorihtmName =
    LargeIntegerUnsigned::multiplicationAlgorithmName(duration.algorithm);
    if (speed < minimumSpeed) {
      global.fatal
      << "Rasing base^{2^iterations} with base="
      << this->base
      << ", iterations="
      << this->iterations
      << " using algorithm: "
      << algorihtmName
      << " results in "
      << digits
      << " digits which is too slow: "
      << "speed in digits per millisecond: "
      << FloatingPoint::doubleToString(speed)
      << ", minimum allowed speed: "
      << FloatingPoint::doubleToString(minimumSpeed)
      << " result digits per millisecond. "
      << global.fatal;
    }
    if (speed < recommendedSpeed) {
      global
      << Logger::yellow
      << "Rasing base^{2^iterations} with base="
      << this->base
      << ", iterations="
      << this->iterations
      << " using algorithm: "
      << algorihtmName
      << " results in "
      << digits
      << " digits which is too slow: "
      << "speed in digits per millisecond: "
      << FloatingPoint::doubleToString(speed)
      << ", recommended speed: "
      << FloatingPoint::doubleToString(recommendedSpeed)
      << " result digits per millisecond. "
      << Logger::endL;
    }
  }
  return true;
}

bool LargeIntegerUnsignedTest::karatsubaMultiplicationOnce(
  const std::string& leftInput,
  const std::string& rightInput,
  const std::string& desiredInput
) {
  STACK_TRACE("LargeIntegerUnsignedTest::karatsubaMultiplicationOnce");
  LargeIntegerUnsigned left(leftInput);
  LargeIntegerUnsigned right(rightInput);
  LargeIntegerUnsigned desired(desiredInput);
  LargeIntegerUnsigned actual;
  LargeIntegerUnsigned::karatsubaProduct(left, right, actual);
  if (desired != actual) {
    global.fatal
    << "Product of: "
    << left.toString()
    << " with: "
    << right.toString()
    << " equals:\n"
    << actual.toString()
    << "\nexpected:\n"
    << desired.toString()
    << global.fatal;
  }
  return true;
}

bool LargeIntegerUnsignedTest::serializationToHex(
  const LargeIntegerUnsigned& input
) {
  STACK_TRACE("LargeIntUnsigned::Test::serializationToHex");
  std::string resultCryptoHex;
  std::string resultByteSerializationHex;
  if (
    !Crypto::convertLargeUnsignedToHexSignificantDigitsFirst(
      input, 0, resultCryptoHex
    )
  ) {
    global.fatal
    << "Function Crypto::convertLargeUnsignedToHexSignificantDigitsFirst "
    << "is not supposed to return false. "
    << global.fatal;
  }
  List<unsigned char> serialization;
  input.writeBigEndianBytes(serialization, true);
  resultByteSerializationHex =
  Crypto::convertListUnsignedCharsToHex(serialization);
  if (resultByteSerializationHex != resultCryptoHex) {
    global.fatal
    << "Byte serialization hex: "
    << resultByteSerializationHex
    << " not equal to crypto hex conversion: "
    << resultCryptoHex
    << ". "
    << global.fatal;
  }
  return true;
}

bool LargeIntegerUnsignedTest::all() {
  STACK_TRACE("LargeIntegerUnsignedTest::all");
  LargeIntegerUnsignedTest::division();
  LargeIntegerUnsignedTest::serializationToHex(LargeIntegerUnsigned(100));
  LargeIntegerUnsignedTest::comparisons();
  LargeIntegerUnsignedTest::guaranteedPrime();
  LargeIntegerUnsignedTest::factor();
  LargeIntegerUnsignedTest::isPossiblyPrime();
  LargeIntegerUnsignedTest::karatsubaMultiplication();
  return true;
}

bool LargeIntegerUnsignedTest::comparisons() {
  List<LargeIntegerUnsigned> toTest =
  {
    LargeIntegerUnsigned(0),
    LargeIntegerUnsigned(1),
    LargeIntegerUnsigned(2),
    LargeIntegerUnsigned(3)
  };
  for (int i = 0; i < toTest.size; i ++) {
    LargeIntegerUnsigned& current = toTest[i];
    if (!current.isGreaterThanOrEqualTo(current)) {
      global.fatal
      << "Number: "
      << current
      << " not greater than or equal to itself. "
      << global.fatal;
    }
  }
  for (int i = 0; i < toTest.size; i ++) {
    for (int j = i + 1; j < toTest.size; j ++) {
      bool mustBeTrue = toTest[j] > toTest[i];
      if (!mustBeTrue) {
        global.fatal
        << "Number: "
        << toTest[j]
        << " not greater than "
        << toTest[i]
        << ". "
        << global.fatal;
      }
      mustBeTrue = toTest[i] < toTest[j];
      if (!mustBeTrue) {
        global.fatal
        << "Number: "
        << toTest[i]
        << " not less than "
        << toTest[j]
        << ". "
        << global.fatal;
      }
    }
  }
  return true;
}

bool LargeIntegerUnsignedTest::isPossiblyPrime() {
  List<LargeIntegerUnsigned> mustReturnTrue;
  List<LargeIntegerUnsigned> mustReturnFalse;
  mustReturnTrue.addOnTop(2);
  mustReturnTrue.addOnTop(3);
  mustReturnTrue.addOnTop(5);
  mustReturnTrue.addOnTop(7);
  mustReturnTrue.addOnTop(1009);
  mustReturnFalse.addOnTop(0);
  mustReturnFalse.addOnTop(1);
  mustReturnFalse.addOnTop(4);
  mustReturnFalse.addOnTop(6);
  mustReturnFalse.addOnTop(9989);
  LargeIntegerUnsignedTest::isPossiblyPrimeFast(mustReturnTrue, true, 0, 3);
  LargeIntegerUnsignedTest::isPossiblyPrimeFast(mustReturnTrue, true, 100, 3);
  LargeIntegerUnsignedTest::isPossiblyPrimeFast(mustReturnFalse, false, 0, 3);
  LargeIntegerUnsignedTest::isPossiblyPrimeFast(
    mustReturnFalse, false, 100, 3
  );
  LargeIntegerUnsignedTest::isPossiblyPrimeMillerRabinOnly(
    mustReturnTrue, true, 100
  );
  LargeIntegerUnsignedTest::isPossiblyPrimeMillerRabinOnly(
    mustReturnFalse, false, 100
  );
  return true;
}

bool LargeIntegerUnsignedTest::isPossiblyPrimeFast(
  const List<LargeIntegerUnsigned>& input,
  bool mustBeTrue,
  int millerRabinTries,
  int64_t maximumRunningTimeMilliseconds
) {
  for (int i = 0; i < input.size; i ++) {
    int64_t millisecondsStart = global.getElapsedMilliseconds();
    bool result = input[i].isPossiblyPrime(millerRabinTries, true, nullptr);
    if (result != mustBeTrue) {
      std::string compositeOrPrime = "composite";
      if (!mustBeTrue) {
        compositeOrPrime = "prime";
      }
      global.fatal
      << "Input "
      << input[i]
      << " is incorrectly computed to be "
      << compositeOrPrime
      << ". Miller-Rabin tries: "
      << millerRabinTries
      << ". "
      << global.fatal;
    }
    int64_t elapsed = global.getElapsedMilliseconds() - millisecondsStart;
    if (elapsed > maximumRunningTimeMilliseconds) {
      global.fatal
      << "It took longer than "
      << maximumRunningTimeMilliseconds
      << "ms to determine whether "
      << input[i]
      << " is prime."
      << global.fatal;
    }
  }
  return true;
}

bool LargeIntegerUnsignedTest::isPossiblyPrimeMillerRabinOnly(
  const List<LargeIntegerUnsigned>& input,
  bool mustBeTrue,
  int millerRabinTries
) {
  for (int i = 0; i < input.size; i ++) {
    bool result =
    input[i].isPossiblyPrimeMillerRabin(millerRabinTries, nullptr);
    if (result != mustBeTrue) {
      std::string compositeOrPrime = "composite";
      if (!mustBeTrue) {
        compositeOrPrime = "prime";
      }
      global.fatal
      << "Input "
      << input[i]
      << " is incorrectly computed via Miller-Rabin to be "
      << compositeOrPrime
      << ". Miller-Rabin tries: "
      << millerRabinTries
      << ". "
      << global.fatal;
    }
  }
  return true;
}

bool LargeIntegerUnsignedTest::guaranteedPrime() {
  List<LargeIntegerUnsigned> primes;
  primes.addOnTop(2);
  primes.addOnTop(3);
  primes.addOnTop(11);
  primes.addOnTop(17);
  primes.addOnTop(31);
  primes.addOnTop(1009);
  primes.addOnTop(65537);
  primes.addOnTop(999983);
  primes.addOnTop(1000003);
  LargeIntegerUnsigned guaranteedPrime;
  guaranteedPrime.assignString("9998200061");
  primes.addOnTop(guaranteedPrime);
  for (int i = 0; i < primes.size; i ++) {
    List<unsigned int> notUsed;
    bool mustBeTrue = false;
    bool mustBeFalse =
    primes[i].isCompositeUsingPrimeDivision(notUsed, mustBeTrue, nullptr);
    if (mustBeFalse) {
      global.fatal
      << "Prime number: "
      << primes[i]
      << " computed as composite."
      << global.fatal;
    }
    if (!mustBeTrue) {
      global.fatal
      << "Prime number: "
      << primes[i]
      << " not reported as guaranteed prime."
      << global.fatal;
    }
  }
  LargeIntegerUnsigned notGuaranteedPrime;
  notGuaranteedPrime.assignString("10000000019");
  bool guaranteed = true;
  List<unsigned int> notUsed;
  bool mustBeFalse =
  notGuaranteedPrime.isCompositeUsingPrimeDivision(
    notUsed, guaranteed, nullptr
  );
  if (mustBeFalse) {
    global.fatal
    << "Prime number: "
    << notGuaranteedPrime
    << " should be too large to be a guaranteed prime."
    << global.fatal;
  }
  return true;
}

bool LargeIntegerUnsignedTest::factor() {
  LargeIntegerUnsignedTest::factorSmall(
    120, "2, 3, 5", "3, 1, 1", 100, 100, 3
  );
  LargeIntegerUnsignedTest::factorSmall(120, "2, 3, 5", "3, 1, 1", 0, 100, 3);
  return true;
}

bool LargeIntegerUnsignedTest::factorSmall(
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
  input.factor(
    factors,
    multiplicitiesInt,
    maximumDivisorToTry,
    numberMillerRabinRuns,
    nullptr
  );
  int64_t elapsed = global.getElapsedMilliseconds() - millisecondsStart;
  if (elapsed > maximumRunningTime) {
    global.fatal
    << "Factoring "
    << input
    << " with maximumDivisorToTry: "
    << maximumDivisorToTry
    << " and "
    << numberMillerRabinRuns
    << " Miller-rabin runs took "
    << elapsed
    << " ms, maximum allowed: "
    << maximumRunningTime
    << " ms. "
    << global.fatal;
  }
  List<LargeInteger> multiplicities;
  multiplicities = multiplicitiesInt;
  std::stringstream out;
  out
  << "Factoring: "
  << input
  << ". Got factors: "
  << expectedFactors
  << " with multiplicities: "
  << multiplicities.toStringCommaDelimited()
  << ". Expected: "
  << expectedFactors
  << " with multiplicities: "
  << expectedMultiplicities
  << ". ";
  if (
    factors.toStringCommaDelimited() != expectedFactors ||
    multiplicities.toStringCommaDelimited() != expectedMultiplicities
  ) {
    global.fatal << "Bad factorization. " << out.str() << global.fatal;
  }
  return true;
}

bool RationalTest::all() {
  RationalTest::testScale();
  return true;
}

class TestRationalScale {
public:
  List<Rational> vector;
  int distinguishedIndex;
  std::string expected;
  TestRationalScale(
    const List<std::string>& inputVector,
    int inputIndex,
    const std::string& inputExpected
  );
  TestRationalScale() {}
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

bool RationalTest::testScale() {
  List<TestRationalScale> toTest =
  {
    TestRationalScale({"1/2", "2/3"}, 0, "3, 4"),
    TestRationalScale({"-1/2", "2/3"}, 0, "3, -4")
  };
  for (int i = 0; i < toTest.size; i ++) {
    TestRationalScale& current = toTest[i];
    List<Rational> copy = current.vector;
    Rational::scaleNormalizeIndex(copy, current.distinguishedIndex);
    if (copy.toStringCommaDelimited() != current.expected) {
      global.fatal
      << "Vector "
      << current.vector.toStringCommaDelimited()
      << " scaled to "
      << copy.toStringCommaDelimited()
      << " instead of the expected: "
      << current.expected
      << global.fatal;
    }
  }
  return true;
}

bool ElementZmodPTest::all() {
  ElementZmodPTest::basicOperations();
  ElementZmodPTest::scale();
  return true;
}

bool ElementZmodPTest::basicOperations() {
  ElementZmodP x, y;
  x.modulus = 5;
  y.modulus = 5;
  x.assignRational(Rational("-1/2"));
  y.assignRational(Rational("3"));
  ElementZmodP z = y;
  z *= x;
  if (!(z.value == 1)) {
    global.fatal
    << "Bad arithmetic: "
    << x.toString()
    << " * "
    << y.toString()
    << " equals: "
    << z.toString()
    << ". "
    << global.fatal;
  }
  return true;
}

bool ElementZmodPTest::scale() {
  ElementZmodP x, y, z;
  LargeIntegerUnsigned modulus = 7;
  x.makeOne(modulus);
  y.makeOne(modulus);
  z.makeOne(modulus);
  x = 2;
  y = 0;
  z = 3;
  List<ElementZmodP> elementList = {x, y, z};
  ElementZmodP scale = ElementZmodP::scaleNormalizeIndex(elementList, 0);
  std::string scaled = elementList.toStringCommaDelimited();
  std::string expected = "(1 mod (7)), (0 mod (7)), (5 mod (7))";
  if (scaled != expected) {
    global.fatal
    << "Bad scaling, got: "
    << scaled
    << ", expected: "
    << expected
    << ". "
    << global.fatal;
  }
  if (scale.value != 4) {
    global.fatal << "Bad scaling, scale expected to be 4. " << global.fatal;
  }
  return true;
}

RationalFraction<Rational> RationalFractionTest::fromString(
  const std::string& input
) {
  Calculator parser;
  std::string inputModified = "MakeRationalFunction(" + input + ")";
  parser.initialize(Calculator::Mode::full);
  parser.evaluate(inputModified);
  if (!parser.programExpression.startsWith(parser.opCommandSequence())) {
    global.fatal
    << "RationalFunction::fromString parsed: "
    << parser.programExpression.toString()
    << " which was not expected. This function is not allowed to fail. "
    << global.fatal;
  }
  RationalFraction<Rational> result;
  if (!parser.programExpression[1].isOfType(&result)) {
    global.fatal
    << "RationalFunction::fromString did not "
    << "produce a rational function, but instead: "
    << parser.programExpression.toString()
    << global.fatal;
  }
  return result;
}

bool RationalFractionTest::all() {
  RationalFractionTest::fromStringTest();
  RationalFractionTest::scaleNormalizeIndex();
  return true;
}

bool RationalFractionTest::fromStringTest() {
  STACK_TRACE("RationalFunction::Test::fromString");
  std::string input = "(a^2+7b)/(2+d*c)";
  std::string expected = "(x_{1}^2+7x_{2} )/(x_{3} x_{4} +2)";
  RationalFraction<Rational> underTest =
  RationalFractionTest::fromString(input);
  if (underTest.toString() != expected) {
    global.fatal
    << "Input: "
    << input
    << " parsed as: "
    << underTest.toString()
    << ", expected: "
    << expected
    << ". "
    << global.fatal;
  }
  return true;
}

bool RationalFractionTest::scaleNormalizeIndex() {
  RationalFraction<Rational> a =
  RationalFractionTest::fromString("(a+1/2)/(b+1/3)");
  RationalFraction<Rational> b = RationalFractionTest::fromString("2a/5");
  RationalFraction<Rational> c = RationalFractionTest::fromString("3a/(7x)");
  List<List<RationalFraction<Rational> > > toScale = {{a}, {a, b}, {a, b, c}};
  List<std::string> expected =
  {
    "(2x_{1} +1)/(3x_{2} +1)",
    "(30x_{1} +15)/(3x_{2} +1), 4x_{1} ",
    "(210x_{1} +105)/(3x_{2} +1), 28x_{1} , 30x_{1} /(x_{2} )"
  };
  for (int i = 0; i < toScale.size; i ++) {
    std::string atStart = toScale[i].toStringCommaDelimited();
    RationalFraction<Rational>::scaleNormalizeIndex(toScale[i], 0);
    if (toScale[i].toStringCommaDelimited() != expected[i]) {
      global.fatal
      << "Scaling rational functions: "
      << atStart
      << " produced: "
      << toScale[i].toStringCommaDelimited()
      << " instead of the expected: "
      << expected[i]
      << global.fatal;
    }
  }
  return true;
}

bool SelectionTest::all() {
  SelectionTest::testNElements(0);
  SelectionTest::testNElements(1);
  SelectionTest::testNElements(2);
  SelectionTest::testNElements(3);
  SelectionTest::testNElements(4);
  return true;
}

bool SelectionTest::testNElements(int n) {
  Selection selection;
  selection.initialize(n);
  int counter = 0;
  do {
    counter ++;
  } while (selection.incrementReturnFalseIfPastLast());
  LargeInteger twoToTheNth = 2;
  twoToTheNth.raiseToPower(n);
  if (twoToTheNth != counter) {
    global.fatal
    << "Got "
    << counter
    << " subsets of "
    << n
    << " elements, expected: "
    << twoToTheNth
    << global.fatal;
  }
  return true;
}
} // namespace: Testing.
