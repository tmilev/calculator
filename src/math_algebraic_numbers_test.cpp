#include "calculator_interface.h"
#include "math_extra_algebraic_numbers.h"

bool AlgebraicNumber::Test::all() {
  AlgebraicNumber::Test::constantValues();
  AlgebraicNumber::Test::hashFunction();
  AlgebraicNumber::Test::evaluatesToComplex();
  return true;
}

bool AlgebraicNumber::Test::constantValues() {
  if (AlgebraicNumber::oneStatic().toString() != "1") {
    global.fatal
    << "Algebraic number 1 has unexpected value. "
    << global.fatal;
  }
  if (AlgebraicNumber::zeroStatic().toString() != "0") {
    global.fatal
    << "Algebraic number 0 has unexpected value. "
    << global.fatal;
  }
  return true;
}

struct AlgebraicNumberWithValue {
  std::string algebraicNumber;
  Complex<double> expectedValue;
  AlgebraicNumberWithValue() {}
  AlgebraicNumberWithValue(
    const std::string& inputAlgebraicNumber,
    double inputRealPart,
    double inputImaginaryPart
  ):
  algebraicNumber(inputAlgebraicNumber) {
    this->expectedValue.realPart = inputRealPart;
    this->expectedValue.imaginaryPart = inputImaginaryPart;
  }
};

bool AlgebraicNumber::Test::fromString(
  const std::string& input, Calculator& inputOwner, AlgebraicNumber& output
) {
  STACK_TRACE("AlgebraicNumber::fromString");
  inputOwner.evaluate(input);
  Expression target = inputOwner.programExpression;
  if (target.startsWith(inputOwner.opCommandSequence())) {
    target = target[1];
  }
  WithContext<AlgebraicNumber> result;
  if (!CalculatorConversions::convert(inputOwner, target, result)) {
    return false;
  }
  output = result.content;
  return true;
}

AlgebraicNumber AlgebraicNumber::Test::fromStringWithoutFailure(
  const std::string& input, Calculator& inputOwner
) {
  STACK_TRACE("AlgebraicNumber::fromStringWithoutFailure");
  AlgebraicNumber result;
  bool mustBeTrue =
  AlgebraicNumber::Test::fromString(input, inputOwner, result);
  if (!mustBeTrue) {
    global.fatal << "Attempt to create algebraic number failed for: " << input;
  }
  return result;
}

bool AlgebraicNumber::Test::evaluatesToComplex() {
  List<AlgebraicNumberWithValue> examples = List<AlgebraicNumberWithValue>({
      AlgebraicNumberWithValue("sqrt(-1)", 0, 1),
      AlgebraicNumberWithValue(
        "sqrt(-1)+sqrt(-2)+sqrt(-3)",
        0,
        1 + FloatingPoint::sqrtFloating(2) + FloatingPoint::sqrtFloating(3)
      ),
      AlgebraicNumberWithValue(
        "1/(sqrt(-1)+sqrt(-2)+sqrt(-3))",
        0,
        - 1 / (
          1 + FloatingPoint::sqrtFloating(2) + FloatingPoint::sqrtFloating(3)
        )
      ),
    });
  for (const AlgebraicNumberWithValue& example : examples) {
    Calculator calculator;
    calculator.initialize(Calculator::Mode::full);
    AlgebraicClosureRationals algebraicClosure;
    AlgebraicNumber number =
    AlgebraicNumber::Test::fromStringWithoutFailure(
      example.algebraicNumber, calculator
    );
    Complex<double> value;
    bool mustBeTrue = number.evaluatesToComplex(&value);
    if (!mustBeTrue) {
      global.fatal
      << "Failed to evaluate "
      << number.toString()
      << " ["
      << example.algebraicNumber
      << "] to complex number. "
      << global.fatal;
    }
    Complex<double> mustBeZero = value;
    mustBeZero -= example.expectedValue;
    if (mustBeZero.magnitudeSquared() > 0.001) {
      global.fatal
      << "The value of "
      << number.toString()
      << " ["
      << example.algebraicNumber
      << "] is: "
      << value.toString()
      << ", expected: "
      << example.expectedValue.toString()
      << global.fatal;
    }
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
    << "They are expected to coincide with the "
    << "current implementation of the hash function. "
    << global.fatal;
  }
  return true;
}
