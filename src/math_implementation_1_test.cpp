// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general.h"
#include "crypto.h"
#include "math_extra_algebraic_numbers.h"

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
  this->vector.SetSize(inputVector.size);
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
