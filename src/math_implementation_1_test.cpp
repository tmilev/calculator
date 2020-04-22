// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general.h"
#include "crypto.h"
#include "math_extra_algebraic_numbers.h"
#include "calculator.h"

bool LargeIntegerUnsigned::Test::SerializationToHex(const LargeIntegerUnsigned& input) {
  MacroRegisterFunctionWithName("LargeIntUnsigned::Test::SerializationToHex");
  std::string resultCryptoHex, resultByteSerializationHex;
  if (!Crypto::ConvertLargeUnsignedIntToHexSignificantDigitsFirst(input, 0, resultCryptoHex)) {
    global.fatal << "Function Crypto::ConvertLargeUnsignedIntToHexSignificantDigitsFirst is not supposed to return false. " << global.fatal;
  }
  List<unsigned char> serialization;
  input.WriteBigEndianBytes(serialization, true);
  resultByteSerializationHex = Crypto::ConvertListUnsignedCharsToHex(serialization);
  if (resultByteSerializationHex != resultCryptoHex) {
    global.fatal << "Byte serialization hex: " << resultByteSerializationHex
    << " not equal to crypto hex conversion: "
    << resultCryptoHex << ". "
    << global.fatal;
  }
  return true;
}

bool LargeIntegerUnsigned::Test::All() {
  MacroRegisterFunctionWithName("LargeIntegerUnsigned::Test::All");
  LargeIntegerUnsigned::Test::SerializationToHex(LargeIntegerUnsigned(100));
  LargeIntegerUnsigned::Test::Comparisons();
  return true;
}

bool LargeIntegerUnsigned::Test::Comparisons() {
  LargeIntegerUnsigned x = 1;
  LargeIntegerUnsigned y = 0;
  List<LargeIntegerUnsigned> toTest = {x, y};
  for (int i = 0; i < toTest.size; i ++) {
    LargeIntegerUnsigned& current = toTest[i];
    if (!current.IsGreaterThanOrEqualTo(current)) {
      global.fatal << "Number: " << current
      << " not greater than or equal to itself. " << global.fatal;
    }
  }
  return true;
}

bool Rational::Test::All() {
  Rational::Test::TestScale();
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

bool Rational::Test::TestScale() {
  List<TestRationalScale> toTest = {
    TestRationalScale({"1/2", "2/3"}, 0, "3, 4"),
    TestRationalScale({"-1/2", "2/3"}, 0, "3, -4")
  };
  for (int i = 0; i < toTest.size; i ++) {
    TestRationalScale& current = toTest[i];
    List<Rational> copy = current.vector;
    Rational::scaleNormalizeIndex(copy, current.distinguishedIndex);
    if (copy.ToStringCommaDelimited() != current.expected) {
      global.fatal << "Vector " << current.vector.ToStringCommaDelimited()
      << " scaled to " << copy.ToStringCommaDelimited()
      << " instead of the expected: " << current.expected
      << global.fatal;
    }
  }
  return true;
}

bool ElementZmodP::Test::all() {
  ElementZmodP::Test::basicOperations();
  return true;
}

bool ElementZmodP::Test::basicOperations() {
  ElementZmodP x, y;
  x.theModulus = 5;
  y.theModulus = 5;
  x.AssignRational(Rational("-1/2"));
  y.AssignRational(Rational("3"));
  ElementZmodP z = y;
  z *= x;
  if (!(z.theValue == 1)) {
    global.fatal << "Bad arithmetic: " << x.toString() << " * "
    << y.toString() << " equals: " << z.toString() << ". " << global.fatal;
  }
  return true;
}

RationalFunction RationalFunction::Test::fromString(const std::string& input) {
  Calculator parser;
  std::string inputModified = "MakeRationalFunction(" + input + ")";
  parser.initialize();
  parser.Evaluate(inputModified);
  if (!parser.theProgramExpression.startsWith(parser.opEndStatement())) {
    global.fatal
    << "RationalFunction::fromString parsed: "
    << parser.theProgramExpression.toString()
    << " which was not expected. This function is not allowed to fail. "
    << global.fatal;
  }
  RationalFunction result;
  if (!parser.theProgramExpression[1].IsOfType(&result)) {
    global.fatal << "RationalFunction::fromString did not "
    << "produce a rational function, but instead: "
    << parser.theProgramExpression.toString()
    << global.fatal;
  }
  return result;
}

bool RationalFunction::Test::all() {
  RationalFunction::Test::fromStringTest();
  RationalFunction::Test::scaleNormalizeIndex();
  return true;
}

bool RationalFunction::Test::fromStringTest() {
  MacroRegisterFunctionWithName("RationalFunction::Test::fromString");
  std::string input = "(a^2+7b)/(2+d*c)";
  std::string expected = "(x_{1}^{2}+7x_{2} )/(x_{3} x_{4} +2)";
  RationalFunction underTest = RationalFunction::Test::fromString(input);
  if (underTest.toString() != expected) {
    global.fatal << "Input: " << input << " parsed as: "
    << underTest.toString() << ", expected: "
    << expected << ". " << global.fatal;
  }
  return true;
}

bool RationalFunction::Test::scaleNormalizeIndex() {
  RationalFunction a = RationalFunction::Test::fromString("(a+1/2)/(b+1/3)");
  RationalFunction b = RationalFunction::Test::fromString("2a/5");
  RationalFunction c = RationalFunction::Test::fromString("3a/(7x)");
  List<List<RationalFunction> > toScale = {
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
    std::string atStart = toScale[i].ToStringCommaDelimited();
    RationalFunction::scaleNormalizeIndex(toScale[i], 0);
    if (toScale[i].ToStringCommaDelimited() != expected[i]) {
      global.fatal << "Scaling rational functions: " << atStart
      << " produced: " << toScale[i].ToStringCommaDelimited()
      << " instead of the expected: " << expected[i] << global.fatal;
    }
  }
  return true;
}
