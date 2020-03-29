// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general.h"
#include "crypto.h"

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
