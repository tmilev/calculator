//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#include "math_general.h"
#include "crypto.h"

extern ProjectInformationInstance ProjectInfoMathImplementationTest;
ProjectInformationInstance ProjectInfoMathImplementationTest(__FILE__, "Math routines tests. ");

bool LargeIntegerUnsigned::Test::SerializationToHex(const LargeIntegerUnsigned& input) {
  MacroRegisterFunctionWithName("LargeIntUnsigned::Test::SerializationToHex");
  std::string resultCryptoHex, resultByteSerializationHex;
  if (!Crypto::ConvertLargeUnsignedIntToHexSignificantDigitsFirst(input, 0, resultCryptoHex)) {
    crash << "Function Crypto::ConvertLargeUnsignedIntToHexSignificantDigitsFirst is not supposed to return false. " << crash;
  }
  List<unsigned char> serialization;
  input.WriteBigEndianBytes(serialization, true);
  resultByteSerializationHex = Crypto::ConvertListUnsignedCharsToHex(serialization);
  if (resultByteSerializationHex != resultCryptoHex) {
    crash << "Byte serialization hex: " << resultByteSerializationHex
    << " not equal to crypto hex conversion: "
    << resultCryptoHex << ". "
    << crash;
  }
  return true;
}

bool LargeIntegerUnsigned::Test::All() {
  MacroRegisterFunctionWithName("LargeIntegerUnsigned::Test::All");
  LargeIntegerUnsigned::Test::SerializationToHex(LargeIntegerUnsigned(100));
  return true;
}
