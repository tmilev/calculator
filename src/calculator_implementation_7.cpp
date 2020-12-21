// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "calculator_inner_functions.h"
#include "calculator_inner_typed_functions.h"
#include "math_general_implementation.h"
#include "math_extra_Weyl_algebras_implementation.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_modules_semisimple_Lie_algebras_implementation.h"
#include "crypto.h"
#include "math_extra_semisimple_Lie_algebras_implementation.h" // undefined reference to `CharacterSemisimpleLieAlgebraModule<RationalFunctionOld>::splitCharacterOverReductiveSubalgebra(std::string*, CharacterSemisimpleLieAlgebraModule<RationalFunctionOld>&, BranchingData&, GlobalVariables&)'
#include "web_api.h"
#include "math_extra_latex_routines.h"
#include "source_code_formatter.h"
#include <cmath>
#include "transport_layer_security.h"
#include "string_constants.h"
#include <vector>
#include "math_rational_function_implementation.h"

template <>
bool Expression::convertInternally<ElementWeylAlgebra<Rational> >(Expression& output) const;
template <>
bool Expression::convertInternally<Polynomial<Rational> >(Expression& output) const;
template <>
bool Expression::convertInternally<ElementUniversalEnveloping<RationalFunction<Rational> > >(Expression& output) const;

bool CalculatorFunctionsCrypto::testLoadPEMCertificates(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::testLoadPEMCertificates");
  if (input.size() != 2) {
    return false;
  }
  std::string binaryString;
  if (!input[1].isOfType(&binaryString)) {
    return false;
  }
  X509Certificate theCertificate;
  std::stringstream errorStream, resultStream;
  // May not be initialized if unit testing. Safe after the first run.
  ASNObject::namesToObjectIdsNonThreadSafe();
  bool success = theCertificate.loadFromASNEncoded(binaryString, &errorStream);
  if (!success) {
    resultStream << "Failed to load asn encoded certificate.<br>";
    resultStream << errorStream.str();
  } else {
    resultStream << "<br>Loaded certificate successfully.<br>" << theCertificate.toString();
  }
  return output.assignValue(resultStream.str(), calculator);
}

bool CalculatorFunctionsCrypto::testTLSDecodeSSLRecord(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::testTLSDecodeSSLRecord");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  TransportLayerSecurityServer testServer;
  std::stringstream commentsOnFailure;
  if (!Crypto::convertHexToListUnsignedChar(
    inputString, testServer.lastReaD.incomingBytes, &commentsOnFailure
  )) {
    return calculator << commentsOnFailure.str();
  }
  testServer.initialize();
  bool success = testServer.decodeSSLRecord(&commentsOnFailure);
  std::stringstream out;
  if (!success) {
    out << "<b style = 'color:red'>Failed to decode the record.</b> "
    << commentsOnFailure.str();
  }
  out << testServer.lastReaD.toHtml(1);

  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsCrypto::testTLSMessageSequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::testTLSMessageSequence");
  Vector<std::string> inputMessages;
  if (!calculator.getVectorFromFunctionArguments(input, inputMessages)) {
    return calculator << "Failed to extract input vector of strings. ";
  }
  if (inputMessages.size < 3) {
    return calculator << "At least 3 inputs required: "
    << "private key, public key, and client hello message. ";
  }
  std::stringstream commentsOnFailure;
  TransportLayerSecurityServer spoofServer;
  if (!spoofServer.initializeAll(
    inputMessages[0],
    inputMessages[1],
    // TransportLayerSecurity::DefaultTLS_READ_ONLY().theServer.certificate.sourceBinary,
    &commentsOnFailure
  )) {
    commentsOnFailure << "Unexpected failure while initializing TLS server. ";
    return output.assignValue(commentsOnFailure.str(), calculator);
  }
  spoofServer.spoofer.flagDoSpoof = true;
  for (int i = 2; i < inputMessages.size; i ++) {
    spoofServer.spoofer.incomingMessages.addOnTop(SSLRecord());
    spoofServer.spoofer.incomingMessages.lastObject()->incomingBytes = inputMessages[i];
  }
  spoofServer.spoofer.currentInputMessageIndex = 0;
  std::stringstream errorStream;
  spoofServer.handShakeIamServer(- 1, &errorStream);

  std::stringstream out;
  std::stringstream spanId;
  spanId << "spanServerSpoofer_" << "_" << global.getElapsedMilliseconds();
  out << "<div id = '" << spanId.str() << "'></div>";
  out << "<script>"
  << "window.calculator.crypto.displayTransportLayerSecurity('"
  << spanId.str() << "', " << spoofServer.toJSON() << ");"
  << "</script>";
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsCrypto::testLoadPEMPrivateKey(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::testLoadPEMPrivateKey");
  if (input.size() != 2) {
    return  false;
  }
  std::string privateKeyString;
  if (!input[1].isOfType(&privateKeyString)) {
    return false;
  }
  List<unsigned char> privateKeyBytes;
  privateKeyBytes = privateKeyString;
  std::stringstream errorStream, resultStream;
  PrivateKeyRSA thePrivateKey;
  bool success = thePrivateKey.loadFromASNEncoded(privateKeyBytes, &errorStream);
  if (!thePrivateKey.basicChecks(&resultStream)) {
    resultStream << "<b style = 'color:red'>Private key failed basic checks. </b>";
  }
  if (!success) {
    resultStream << "<br>Failed to load asn encoded certificate.<br>";
    resultStream << errorStream.str();
  } else {
    resultStream << "<br>Loaded private key.<br>" << thePrivateKey.toString();
  }
  return output.assignValue(resultStream.str(), calculator);
}

bool CalculatorFunctionsCrypto::loadKnownCertificates(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::loadKnownCertificates");
  (void) input;
  std::stringstream out;
  Crypto::loadKnownCertificates(&out, &out);
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsCrypto::sha1OfString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::sha1OfString");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctionsCrypto::functionHashString(
    calculator, input[1], output, "SHA1", false
  );
}

bool CalculatorFunctionsCrypto::sha256OfString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::sha256OfString");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctionsCrypto::functionHashString(calculator, input[1], output, "SHA256", false);
}

bool CalculatorFunctionsCrypto::sha512(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSha512String");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctionsCrypto::functionHashString(calculator, input[1], output, "SHA512", false);
}

bool CalculatorFunctionsCrypto::sha256OfStringVerbose(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::sha256OfString");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctionsCrypto::functionHashString(calculator, input[1], output, "SHA256", true);
}

bool CalculatorFunctionsCrypto::sha224OfString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::sha224OfString");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctionsCrypto::functionHashString(calculator, input[1], output, "SHA224", false);
}

bool CalculatorFunctionsCrypto::ripemd160OfString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::ripemd160OfString");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctionsCrypto::functionHashString(calculator, input[1], output, "RIPEMD160", false);
}

bool CalculatorFunctionsCrypto::sha3_256OfString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::sha3_256OfString");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctionsCrypto::functionHashString(calculator, input[1], output, "SHA3_256", false);
}

bool CalculatorFunctionsCrypto::keccak256OfString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::keccak256OfString");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctionsCrypto::functionHashString(calculator, input[1], output, "KECCAK256", false);
}

bool CalculatorFunctionsCrypto::functionHashString(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  const std::string& hashId,
  bool verbose
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::functionHashString");
  std::string inputString;
  if (!input.isOfType(&inputString)) {
    return false;
  }
  List<unsigned char> theBitStream;
  theBitStream = inputString;
  std::stringstream out;
  if (verbose) {
    out << "<br>Input: " << inputString;
    out << "<br>Base64 conversion: " << Crypto::convertListUnsignedCharsToBase64(theBitStream, false);
    out << "<br>Base64url conversion: " << Crypto::convertListUnsignedCharsToBase64(theBitStream, true);
  }
  List<unsigned char> hashUChar;
  List<uint32_t> theSha1Uint;
  Crypto::External& externalCrypto = Crypto::externalCrypto();
  if (hashId == "SHA1" || hashId == "SHA256" || hashId == "SHA224") {
    if (hashId == "SHA1") {
      Crypto::computeSha1(inputString, theSha1Uint);
    } else if (hashId == "SHA256") {
      Crypto::computeSha256(inputString, theSha1Uint);
    } else if (hashId == "SHA224") {
      Crypto::computeSha224(inputString, theSha1Uint);
    }
    Crypto::convertUint32ToUcharBigendian(theSha1Uint, hashUChar);
  } else if (hashId == "SHA3_256") {
    Crypto::computeSha3_256(inputString, hashUChar);
  } else if (hashId == "KECCAK256") {
    Crypto::computeKeccak3_256(inputString, hashUChar);
  } else if (hashId == "SHA512") {
    Crypto::computeSha512(inputString, hashUChar);
  } else if (hashId == "RIPEMD160") {
    if (externalCrypto.computeRIPEMD160 == nullptr) {
      return calculator
      << "You are running a non-vanilla version of the calculator "
      << "that is missing ripemd160 support. "
      << "check out the vanilla version of the calculator at github.com.";
    }
    externalCrypto.computeRIPEMD160(inputString, hashUChar);
  }
  if (verbose) {
    std::string theSha1base64string, theSha1base64URLstring;
    theSha1base64string = Crypto::convertListUnsignedCharsToBase64(hashUChar, true);
    theSha1base64URLstring = Crypto::convertListUnsignedCharsToBase64(hashUChar, false);
    out << "<br>" << hashId << " in base64: " << theSha1base64string;
    out << "<br>" << hashId << " in base64url: " << theSha1base64URLstring;
    out << "<br>" << hashId << " hex: ";
    for (int i = 0; i < theSha1Uint.size; i ++) {
      out << std::hex << theSha1Uint[i];
    }
    out << "<br>bitstream: ";
  }
  for (int i = 0; i < hashUChar.size; i ++) {
    out << hashUChar[i];
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsEncoding::convertBase64ToString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsEncoding::convertBase64ToString");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString, result;
  if (!input[1].isOfType(&inputString)) {
    inputString = input[1].toString();
  }
  if (!Crypto::convertBase64ToString(inputString, result, &calculator.comments)) {
    return false;
  }
  return output.assignValue(result, calculator);
}

bool CalculatorFunctions::nistEllipticCurveOrder(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::nistEllipticCurveGenerator");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return calculator
    << "Function NISTEllipticCurveGenerator takes as input a string with an EC curve name. "
    << "Available curve names: secp256k1";
  }
  LargeIntegerUnsigned result;
  if (!EllipticCurveWeierstrassNormalForm::getOrderNISTCurve(
    inputString, result, &calculator.comments
  )) {
    return false;
  }
  return output.assignValue(result, calculator);
}

template < >
bool ElementEllipticCurve<ElementZmodP>::makeGeneratorNISTCurve(
  const std::string& input, std::stringstream* commentsOnFailure
);

bool CalculatorFunctions::nistEllipticCurveGenerator(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::nistEllipticCurveGenerator");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return calculator
    << "Function NISTEllipticCurveGenerator takes as input a string with an EC curve name. "
    << "Available curve names: secp256k1";
  }
  ElementEllipticCurve<ElementZmodP> generator;
  if (!generator.makeGeneratorNISTCurve(inputString, &calculator.comments)) {
    return false;
  }
  ExpressionContext theContext(calculator);
  List<std::string> variables;
  variables.addOnTop("x");
  variables.addOnTop("y");
  theContext.setVariablesFromStrings(variables);
  return output.assignValueWithContext(generator, theContext, calculator);
}

bool CalculatorFunctions::stringDifference(
  Calculator& calculator,
  const Expression& input,
  Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::stringDifference");
  if (input.size() != 3) {
    return false;
  }
  std::string left, right;
  if (!input[1].isOfType(&left) || !input[2].isOfType(&right)) {
    return false;
  }
  return output.assignValue(
    StringRoutines::Differ::differenceHTMLStatic(
      left, right, "left", "right"
    ),
    calculator
  );
}

bool CalculatorFunctions::sliceString(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::sliceString");
  if (input.size() < 3) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType<std::string>(&inputString)) {
    return false;
  }
  int leftIndex = 0;
  if (!input[2].isSmallInteger(&leftIndex)) {
    return calculator << "Failed to convert slice input " << input[2] << " to small integer. ";
  }
  if (leftIndex < 0) {
    return calculator << "slice input " << input[2] << " appears to be negative. ";
  }
  if (leftIndex >= static_cast<signed>(inputString.size())) {
    return output.assignValue(std::string(""), calculator);
  }
  if (input.size() <= 3) {
    return output.assignValue(inputString.substr(static_cast<unsigned>(leftIndex)), calculator);
  }
  int rightIndex = 0;
  if (!input[3].isSmallInteger(&rightIndex)) {
    return calculator << "Failed to convert slice input " << input[3] << " to small integer. ";
  }
  if (rightIndex < 0) {
    rightIndex += static_cast<signed>(inputString.size());
  }
  int size = rightIndex - leftIndex ;
  if (size <= 0) {
    return output.assignValue(std::string(""), calculator);
  }
  return output.assignValue(
    inputString.substr(static_cast<unsigned>(leftIndex), static_cast<unsigned>(size)),
    calculator
  );
}

bool CalculatorFunctionsEncoding::convertIntegerUnsignedToBase58(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsEncoding::convertIntegerUnsignedToBase58");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger theInt;
  if (!input[1].isInteger(&theInt)) {
    return false;
  }
  if (theInt < 0) {
    return calculator << "Conversion from negative integer to base58 not allowed. ";
  }
  std::string result;
  Crypto::convertLargeIntUnsignedToBase58SignificantDigitsFIRST(theInt.value, result, 0);
  return output.assignValue(result, calculator);
}

bool CalculatorFunctionsCrypto::appendDoubleSha256Check(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::innerAppendSha256Check");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  std::string outputString;
  Crypto::appendDoubleSha256Check(inputString, outputString);
  return output.assignValue(outputString, calculator);
}

bool CalculatorFunctionsCrypto::aes_cbc_256_decrypt(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::aes_cbc_256_decrypt");
  if (input.size() != 3) {
    return calculator << "AES decrypt function expects two arguments: key and plain text. ";
  }
  std::string text, key;
  if (!input[1].isOfType<std::string>(&key)) {
    return false;
  }
  if (!input[2].isOfType<std::string>(&text)) {
    return false;
  }
  std::string cipherText;
  std::stringstream comments;
  bool result = Crypto::externalCrypto().decryptAES_CBC_256_string(
    key, text, cipherText, &comments
  );
  if (!result) {
    return calculator << comments.str();
  }
  return output.assignValue(cipherText, calculator);
}

bool CalculatorFunctionsCrypto::aes_cbc_256_encrypt(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::aes_cbc_256_encrypt");
  if (input.size() != 3) {
    return calculator << "AES function expects two arguments: key and plain text. ";
  }
  std::string text, key;
  if (!input[1].isOfType<std::string>(&key)) {
    return false;
  }
  if (!input[2].isOfType<std::string>(&text)) {
    return false;
  }
  std::string cipherText;
  std::stringstream comments;
  bool result = Crypto::externalCrypto().encryptAES_CBC_256_string(
    key, text, cipherText, &comments
  );
  if (!result) {
    return calculator << comments.str();
  }
  return output.assignValue(cipherText, calculator);
}

bool CalculatorFunctionsEncoding::convertBase58ToHex(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsEncoding::convertBase58ToHex");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  std::string outputString;
  if (!Crypto::convertBase58ToHexSignificantDigitsFirst(
    inputString, outputString, &calculator.comments
  )) {
    return calculator << "Failed to convert " << inputString << " to hex. ";
  }
  return output.assignValue(outputString, calculator);
}

bool CalculatorFunctionsEncoding::convertHexToBase58(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsEncoding::convertHexToBase58");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  LargeIntegerUnsigned outputInteger;
  std::string outputString;
  int numLeadingZeroBytes = 0;
  if (!Crypto::convertHexToInteger(inputString, outputInteger, numLeadingZeroBytes)) {
    return calculator << "Failed to convert " << inputString << " from hex to integer. ";
  }
  Crypto::convertLargeIntUnsignedToBase58SignificantDigitsFIRST(outputInteger, outputString, numLeadingZeroBytes);
  return output.assignValue(outputString, calculator);
}

bool CalculatorFunctionsEncoding::convertStringToHex(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsEncoding::convertStringToHex");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  return output.assignValue(Crypto::convertStringToHex(inputString, 0, false), calculator);
}

bool CalculatorFunctionsEncoding::convertCharToBase64(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsEncoding::convertCharToBase64");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  List<unsigned char> theBitStream;
  theBitStream = inputString;
  return output.assignValue(Crypto::convertListUnsignedCharsToBase64(theBitStream, false), calculator);
}

bool CalculatorFunctionsEncoding::base64ToCharToBase64Test(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsEncoding::base64ToCharToBase64Test");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  List<unsigned char> theBitStream;
  if (!Crypto::convertBase64ToBitStream(inputString, theBitStream, &calculator.comments)) {
    return false;
  }
  std::stringstream out;
  std::string theConvertedBack = Crypto::convertListUnsignedCharsToBase64(theBitStream, false);
  out << "Original string: " << inputString
  << "<br>Converted to bitstream and back: " << theConvertedBack;
  if (theConvertedBack != inputString) {
    out << "<br><b>The input is not the same as the output!</b>";
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::urlKeyValuePairsToNormalRecursive(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::urlKeyValuePairsToNormalRecursive");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string theString;
  if (!argument.isOfType<std::string>(&theString)) {
    return false;
  }
  return output.assignValue(HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(theString), calculator);
}

bool CalculatorFunctions::convertElementZmodPToInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::convertElementZmodPToInteger");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  ElementZmodP theElement;
  if (!argument.isOfType<ElementZmodP>(&theElement)) {
    return false;
  }
  return output.assignValue(theElement.value, calculator);
}

bool CalculatorFunctions::urlStringToNormalString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::urlStringToNormalString");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string theString;
  if (!argument.isOfType<std::string>(&theString)) {
    return false;
  }
  return output.assignValue(HtmlRoutines::convertURLStringToNormal(theString, false), calculator);
}

bool CalculatorFunctions::stringToURL(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::stringToURL");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string theString;
  if (!argument.isOfType<std::string>(&theString)) {
    return false;
  }
  return output.assignValue(HtmlRoutines::convertStringToURLString(theString, false), calculator);
}

bool CalculatorFunctions::stringToAtom(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::stringToAtom");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string theString;
  if (!argument.isOfType(&theString)) {
    return false;
  }
  return output.makeAtom(theString, calculator);
}

bool CalculatorFunctions::expressionToString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::expressionToString");
  if (input.size() != 2) {
    return false;
  }
  return output.assignValue(input[1].toString(), calculator);
}

bool CalculatorFunctions::quoteToString(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::quoteToString");
  std::string operation;
  if (input.size() != 2) {
    calculator << "<b>Warning: this shouldn't happen: "
    << "quote operation is applied to an expression "
    << "with wrong number of arguments. "
    << "This may be a bug with the function "
    << "Calculator::parseFillDictionary.</b>";
    return output.assignValue(input.toString(), calculator);
  }
  if (input[1].isOperation(&operation)) {
    return output.assignValue(operation, calculator);
  }
  calculator
  << "<b>Warning: this shouldn't happen: quote operation "
  << "is applied to the non-atomic expression: "
  << input.toString() << ". "
  << "This may be a bug with the function Calculator::parseFillDictionary. </b>";
  return output.assignValue(input.toString(), calculator);
}

bool CalculatorFunctions::innerFourierTransformEWA(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFourierTransformEWA");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (!argument.isOfType<ElementWeylAlgebra<Rational> >()) {
    return false;
  }
  ElementWeylAlgebra<Rational> theElt;
  argument.getValue<ElementWeylAlgebra<Rational> >().fourierTransform(theElt);
  return output.assignValueWithContext(theElt, argument.getContext(), calculator);
}

bool CalculatorFunctionsBasic::logarithmBase(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBasic::logarithmBase");
  if (!input.startsWith(calculator.opLogBase(), 3)) {
    return false;
  }
  Expression numE, denE;
  numE.makeOX(calculator, calculator.opLog(), input[2]);
  denE.makeOX(calculator, calculator.opLog(), input[1]);
  return output.makeXOX(calculator, calculator.opDivide(), numE, denE);
}

bool CalculatorFunctionsBasic::logarithm(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBasic::logarithm");
  if (input.size() != 2) {
    return false;
  }
  if (input[1].isEqualToZero()) {
    return output.makeError("Attempting to compute logarithm of zero.", calculator);
  }
  if (input[1].isEqualToOne()) {
    return output.assignValue(0, calculator);
  }
  if (calculator.flagNoApproximations) {
    return false;
  }
  double theArgument;
  if (!input[1].evaluatesToDouble(&theArgument)) {
    if (input[1].isOperationGiven(calculator.opE())) {
      return output.assignValue(Rational(1), calculator);
    }
    return false;
  }
  if (theArgument > 0) {
    if (input[1].isOperationGiven(calculator.opE())) {
      return output.assignValue(Rational(1), calculator);
    }
    return output.assignValue(FloatingPoint::logFloating(theArgument), calculator);
  }
  theArgument *= - 1;
  Expression iE, ipiE, piE, lnPart;
  iE.makeSqrt(calculator, Rational(- 1), 2);
  piE.makeAtom(calculator.opPi(), calculator);
  ipiE.makeXOX(calculator, calculator.opTimes(), piE, iE);
  lnPart.assignValue(FloatingPoint::logFloating(theArgument), calculator);
  return output.makeXOX(calculator, calculator.opPlus(), lnPart, ipiE);
}

bool CalculatorFunctionsBasic::factorial(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBasic::factorial");
  if (input.size() != 2) {
    return false;
  }
  int inputInt = - 1;
  if (!input[1].isIntegerFittingInInt(&inputInt)) {
    return false;
  }
  if (inputInt < 0) {
    return false;
  }
  if (inputInt > 30000) {
    return calculator << "Computing large factorials is disabled "
    << "(can be changed in the source code by modifying CalculatorFunctions::factorial).";
  }
  Rational result;
  return output.assignValue(result.factorial(inputInt), calculator);
}

bool CalculatorFunctionsTrigonometry::arctan(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::arctan");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (argument.isEqualToOne()) {
    output.makeAtom(calculator.opPi(), calculator);
    output /= calculator.expressionFour();
    return true;
  }
  if (argument.isEqualToMOne()) {
    output.makeAtom(calculator.opPi(), calculator);
    output /= calculator.expressionFour();
    output *= calculator.expressionMinusOne();
    return true;
  }
  if (calculator.flagNoApproximations) {
    return false;
  }
  double theArgument;
  if (!argument.evaluatesToDouble(&theArgument)) {
    return false;
  }
  return output.assignValue(FloatingPoint::arctan(theArgument), calculator);
}

bool CalculatorFunctionsTrigonometry::arccos(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::arccos");
  if (calculator.flagNoApproximations) {
    return false;
  }
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  double doubleArgument;
  if (!argument.evaluatesToDouble(&doubleArgument)) {
    return false;
  }
  return output.assignValue(FloatingPoint::arccos(doubleArgument), calculator);
}

bool CalculatorFunctionsTrigonometry::arcsin(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::arcsin");
  if (input.size() != 2) {
    return false;
  }
  if (calculator.flagNoApproximations) {
    return false;
  }
  double theArgument;
  if (!input[1].evaluatesToDouble(&theArgument)) {
    return false;
  }
  return output.assignValue(FloatingPoint::arcsin(theArgument), calculator);
}

bool CalculatorFunctionsBasic::absoluteValue(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBasic::absoluteValue");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Rational theRat;
  if (argument.isRational(&theRat)) {
    if (theRat < 0) {
      return output.assignValue(- theRat, calculator);
    }
    return output.assignValue(theRat, calculator);
  }
  double theDouble = 0;
  if (argument.evaluatesToDouble(&theDouble)) {
    if (theDouble < 0) {
      Expression moneE;
      moneE.assignValue(- 1, calculator);
      output = argument;
      output *= moneE;
      return true;
    }
    output = argument;
    return true;
  }
  return false;
}

bool CalculatorFunctionsTrigonometry::sin(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::sin");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (argument.isOperationGiven(calculator.opPi())) {
    return output.assignValue(0, calculator);
  }
  if (argument.isEqualToZero()) {
    return output.assignValue(0, calculator);
  }
  Rational piProportion;
  if (argument.startsWith(calculator.opTimes(), 3)) {
    if (argument[2].isOperationGiven(calculator.opPi())) {
      if (argument[1].isOfType<Rational>(&piProportion)) {
        AlgebraicNumber algOutput;
        Rational ratOutput;
        if (algOutput.assignSinRationalTimesPi(piProportion, calculator.theObjectContainer.theAlgebraicClosure)) {
          if (algOutput.isRational(&ratOutput)) {
            return output.assignValue(ratOutput, calculator);
          }
          return output.assignValue(algOutput, calculator);
        }
      }
    }
  }
  if (calculator.flagNoApproximations) {
    return false;
  }
  double theArgument = 0;
  if (!argument.evaluatesToDouble(&theArgument)) {
    return false;
  }
  return output.assignValue(FloatingPoint::sinFloating(theArgument), calculator);
}

bool CalculatorFunctionsTrigonometry::cos(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::cos");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (argument.isOperationGiven(calculator.opPi())) {
    return output.assignValue(- 1, calculator);
  }
  if (argument.isEqualToZero()) {
    return output.assignValue(1, calculator);
  }
  Rational piProportion;
  if (argument.startsWith(calculator.opTimes(), 3)) {
    if (argument[2].isOperationGiven(calculator.opPi())) {
      if (argument[1].isOfType<Rational>(&piProportion)) {
        AlgebraicNumber algOutput;
        Rational ratOutput;
        if (algOutput.assignCosRationalTimesPi(piProportion, calculator.theObjectContainer.theAlgebraicClosure)) {
          if (algOutput.isRational(&ratOutput)) {
            return output.assignValue(ratOutput, calculator);
          }
          return output.assignValue(algOutput, calculator);
        }
      }
    }
  }
  if (calculator.flagNoApproximations) {
    return false;
  }
  double theArgument = 0;
  if (!argument.evaluatesToDouble(&theArgument)) {
    return false;
  }
  return output.assignValue(FloatingPoint::cosFloating(theArgument), calculator );
}

bool CalculatorFunctionsTrigonometry::tan(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::tan");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression num, den;
  num.makeOX(calculator, calculator.opSin(), argument);
  den.makeOX(calculator, calculator.opCos(), argument);
  return output.makeXOX(calculator, calculator.opDivide(), num, den);
}

bool CalculatorFunctionsTrigonometry::cotangent(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::tan");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression num, den;
  num.makeOX(calculator, calculator.opCos(), argument);
  den.makeOX(calculator, calculator.opSin(), argument);
  return output.makeXOX(calculator, calculator.opDivide(), num, den);
}

bool CalculatorFunctionsTrigonometry::sec(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::sec");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression num, den;
  num.assignValue(1, calculator);
  den.makeOX(calculator, calculator.opCos(), argument);
  return output.makeXOX(calculator, calculator.opDivide(), num, den);
}

bool CalculatorFunctionsTrigonometry::csc(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::csc");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression num, den;
  num.assignValue(1, calculator);
  den.makeOX(calculator, calculator.opSin(), argument);
  return output.makeXOX(calculator, calculator.opDivide(), num, den);
}

bool Calculator::getSumProductsExpressions(const Expression& inputSum, List<List<Expression> >& outputSumMultiplicands) {
  MacroRegisterFunctionWithName("Calculator::getSumProductsExpressions");
  List<Expression> theSummands, currentMultiplicands;
  outputSumMultiplicands.setSize(0);
  if (!this->collectOpands(inputSum, this->opPlus(), theSummands)) {
    return false;
  }
  for (int i = 0; i < theSummands.size; i ++) {
    this->collectOpands(theSummands[i], this->opTimes(), currentMultiplicands);
    outputSumMultiplicands.addOnTop(currentMultiplicands);
  }
  return true;
}

bool CalculatorFunctions::innerCoefficientOf(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCoefficientOf");
  if (input.size() != 3) {
    return false;
  }
  List<List<Expression> > theSummands;
  List<Expression> currentListMultiplicands, survivingSummands;
  Expression currentMultiplicand;
  if (input[2].startsWith(calculator.opDivide())) {
    Expression coefficientNumerator = input;
    coefficientNumerator.setChild(2, input[2][1]);
    if (!CalculatorFunctions::innerCoefficientOf(calculator, coefficientNumerator, output)) {
      return false;
    }
    return output.makeXOX(calculator, calculator.opDivide(), output, input[2][2]);
  }
  if (!calculator.getSumProductsExpressions(input[2], theSummands)) {
    return calculator << "Failed to extract product of expressions from "
    << input[2].toString();
  }
  for (int i = 0; i < theSummands.size; i ++) {
    bool isGood = false;
    for (int j = 0; j < theSummands[i].size; j ++) {
      if (theSummands[i][j] == input[1]) {
        if (isGood) {
          isGood = false;
          break;
        }
        isGood = true;
      }
    }
    if (!isGood) {
      continue;
    }
    currentListMultiplicands.setSize(0);
    for (int j = 0; j < theSummands[i].size; j ++) {
      if (theSummands[i][j] != input[1]) {
        currentListMultiplicands.addOnTop(theSummands[i][j]);
      }
    }
    currentMultiplicand.makeProduct(calculator, currentListMultiplicands);
    survivingSummands.addOnTop(currentMultiplicand);
  }
  return output.makeSum(calculator, survivingSummands);
}

bool CalculatorFunctions::innerChildExpression(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerChildExpression");
  (void) calculator;
  if (input.size() != 3) {
    return false;
  }
  int theIndex = 0;
  if (!input[2].isSmallInteger(&theIndex)) {
    return false;
  }
  theIndex --;
  if (theIndex < 0 || theIndex >= input[1].size()) {
    return false;
  }
  output = input[1][theIndex];
  return true;
}

bool CalculatorFunctions::innerDereferenceInterval(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDereferenceInterval");
  (void) calculator;
  if (!input.startsWith(calculator.opUnderscore(), 3)) {
    return false;
  }
  if (!input[1].isIntervalRealLine()) {
    return false;
  }
  if (input[2].isEqualToOne()) {
    output = input[1][1];
    return true;
  }
  if (input[2].isEqualToTwo()) {
    output = input[1][2];
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerDereferenceSequenceOrMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDereferenceSequenceOrMatrix");
  (void) calculator;
  if (!input.startsWith(calculator.opUnderscore(), 3)) {
    return false;
  }
  if (
    !input[1].startsWith(calculator.opSequence()) &&
    !input[1].isMatrix()
  ) {
    return false;
  }
  int theIndex;
  if (!input[2].isSmallInteger(&theIndex)) {
    return false;
  }
  if (theIndex > 0 && theIndex < input[1].size()) {
    output = input[1][theIndex];
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerDereferenceSequenceStatements(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDereferenceSequenceStatements");
  (void) calculator;
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].startsWith(calculator.opCommandSequence())) {
    return false;
  }
  int theIndex;
  if (!input[2].isSmallInteger(&theIndex)) {
    return false;
  }
  if (theIndex > 0 && theIndex < input[1].size()) {
    output = input[1][theIndex];
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerSolveSerreLikeSystem(
  Calculator& calculator, const Expression& input, Expression& output, bool useUpperLimit, bool startWithAlgebraicClosure
) {
  MacroRegisterFunctionWithName("Calculator::innerSolveSerreLikeSystem");
  Vector<Polynomial<Rational> > polynomialsRational;
  ExpressionContext theContext(calculator);
  bool useArguments =
  input.startsWith(calculator.getOperations().getIndexNoFail("FindOneSolutionSerreLikePolynomialSystem")) ||
  input.startsWith(calculator.getOperations().getIndexNoFail("FindOneSolutionSerreLikePolynomialSystemAlgebraic")) ||
  input.startsWith(calculator.getOperations().getIndexNoFail("FindOneSolutionSerreLikePolynomialSystemUpperLimit")) ||
  input.startsWith(calculator.getOperations().getIndexNoFail("FindOneSolutionSerreLikePolynomialSystemAlgebraicUpperLimit"));

  if (useArguments) {
    if (!calculator.getVectorFromFunctionArguments(
      input,
      polynomialsRational,
      &theContext,
      0,
      CalculatorConversions::functionPolynomial<Rational>
    )) {
      return output.makeError("Failed to extract list of polynomials. ", calculator);
    }
  } else {
    if (!calculator.getVector(
      input,
      polynomialsRational,
      &theContext,
      0,
      CalculatorConversions::functionPolynomial<Rational>
    )) {
      return output.makeError("Failed to extract list of polynomials. ", calculator);
    }
  }
  PolynomialSystem<AlgebraicNumber> system;
  theContext.getFormat(system.groebner.theFormat);
  int upperLimit = 501;
  if (useUpperLimit) {
    Rational upperLimitRat;
    if (!polynomialsRational[0].isConstant(&upperLimitRat)) {
      return calculator << "Failed to extract a constant from the first argument "
      << polynomialsRational[0].toString(&system.format()) << ". ";
    }
    if (!upperLimitRat.isIntegerFittingInInt(&upperLimit)) {
      return calculator << "Failed to extract a small integer from the first argument "
      << upperLimitRat.toString(&system.groebner.theFormat) << ". ";
    }
    polynomialsRational.popIndexShiftDown(0);
  }
  Vector<Polynomial<AlgebraicNumber> > polynomials;
  polynomials = polynomialsRational;
  system.groebner.maximumPolynomialComputations = upperLimit;
  system.maximumSerreSystemComputationsPreferred = upperLimit;
  system.groebner.polynomialOrder.monomialOrder = MonomialP::orderDefault();
  system.theAlgebraicClosure = &calculator.theObjectContainer.theAlgebraicClosure;
  system.flagTryDirectlySolutionOverAlgebraicClosure = startWithAlgebraicClosure;
  global.theDefaultFormat.getElement() = system.groebner.theFormat;
  system.flagUseTheMonomialBranchingOptimization = true;
  system.solveSerreLikeSystem(polynomials);
  std::stringstream out;
  out << "<br>The context vars:<br>" << theContext.toString();
  out << "<br>The polynomials: " << polynomials.toString(&system.groebner.theFormat);
  out << "<br>Total number of polynomial computations: "
  << system.numberOfSerreSystemComputations;
  if (system.flagSystemProvenToHaveNoSolution) {
    out << "<br>The system does not have a solution. ";
  } else if (system.flagSystemProvenToHaveSolution) {
    out << "<br>System proven to have solution. ";
  }
  if (!system.flagSystemProvenToHaveNoSolution) {
    if (system.flagSystemSolvedOverBaseField) {
      out << "<br>One solution follows. " << system.toStringSerreLikeSolution();
    } else {
      out << "However, I was unable to find such a solution: my heuristics are not good enough.";
    }
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsAlgebraic::convertAlgebraicNumberToMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsAlgebraic::convertAlgebraicNumberToMatrix");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  AlgebraicNumber theNumber;
  if (!argument.isOfType(&theNumber)) {
    return calculator << "Failed to convert "
    << argument.toString() << " to algebraic number. ";
  }
  int dimension = theNumber.owner->getDimensionOverTheRationals();
  MatrixTensor<Rational> numberMatrixTensor;
  theNumber.owner->getMultiplicationBy(theNumber, numberMatrixTensor);
  Matrix<Rational> result;
  numberMatrixTensor.getMatrix(result, dimension);
  return output.assignMatrix(result, calculator);
}

bool CalculatorFunctionsAlgebraic::printAlgebraicClosureStatus(
  Calculator& calculator, const Expression& input, Expression& output
) {
  (void) input;
  FormatExpressions theFormat;
  theFormat.flagUseHTML = false;
  theFormat.flagUseLatex = true;
  return output.assignValue(
    calculator.theObjectContainer.theAlgebraicClosure.toStringFull(&theFormat),
    calculator
  );
}

bool CalculatorFunctionsAlgebraic::getAlgebraicNumberFromMinPoly(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsAlgebraic::getAlgebraicNumberFromMinPoly");
  Expression polynomialExpression;
  if (!CalculatorConversions::innerPolynomial<AlgebraicNumber>(
    calculator, input, polynomialExpression
  )) {
    return calculator << "<hr>Failed to convert "
    << input.toString() << " to polynomial. ";
  }
  WithContext<Polynomial<AlgebraicNumber> > polynomial;
  if (!polynomialExpression.isOfTypeWithContext(&polynomial)) {
    return calculator << "<hr>Failed to convert " << input.toString()
    << " to polynomial, instead got " << polynomialExpression.toString();
  }
  if (polynomial.context.numberOfVariables() != 1) {
    return calculator
    << "<hr>After conversion, I got the polynomial "
    << polynomialExpression.toString()
    << ", which is not in one variable.";
  }
  AlgebraicNumber theAN;
  std::stringstream commentsOnFailure;
  if (!theAN.constructFromMinimalPolynomial(
    polynomial.content,
    calculator.theObjectContainer.theAlgebraicClosure,
    &commentsOnFailure
  )) {
    return calculator << "Failed to construct minimal polynomial. " << commentsOnFailure.str();
  }
  return output.assignValue(theAN, calculator);
}

bool CalculatorFunctions::innerCompositeApowerBevaluatedAtC(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompositeApowerBevaluatedAtC");
  if (!input.isListNElements()) {
    return false;
  }
  const Expression& firstE = input[0];
  if (!firstE.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  if (firstE[1].isSequenceNElements()) {
    return false;
  }
  calculator.checkInputNotSameAsOutput(input, output);
  Expression finalBase;
  finalBase.reset(calculator, input.children.size);
  finalBase.addChildOnTop(input[0][1]);
  for (int i = 1; i < input.children.size; i ++) {
    finalBase.addChildOnTop(input[i]);
  }
  return output.makeXOX(calculator, calculator.opThePower(), finalBase, input[0][2]);
}

bool CalculatorFunctions::innerConstantFunction(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerConstantFunction");
  (void) calculator;//portable way of avoiding unused parameter warning
  if (!input.isListNElements()) {
    return false;
  }
  if (!input[0].isConstantNumber()) {
    return false;
  }
  output = input[0];
  return true;
}

bool CalculatorFunctions::combineFractionsCommutativeWithInternalLibrary(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::combineFractionsCommutativeWithInternalLibrary");
  if (!input.startsWith(calculator.opPlus(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (
    !leftE.startsWith(calculator.opDivide(), 3) ||
    !rightE.startsWith(calculator.opDivide(), 3)
  ) {
    return false;
  }
  Expression converted(calculator);
  if (!CalculatorConversions::functionRationalFunction<AlgebraicNumber>(calculator, input, converted)) {
    return false;
  }
  WithContext<RationalFunction<AlgebraicNumber> > rationalFunction;
  if (!converted.isOfTypeWithContext(&rationalFunction)) {
    return false;
  }
  return CalculatorConversions::innerExpressionFromRationalFunction(calculator, rationalFunction.content, output, &rationalFunction.context);
}

bool CalculatorFunctions::combineFractionsCommutative(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::combineFractionsCommutative");
  if (!input.startsWith(calculator.opPlus(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (
    !leftE.startsWith(calculator.opDivide(), 3) ||
    !rightE.startsWith(calculator.opDivide(), 3)
  ) {
    return false;
  }
  // Failed to extract rational function.
  if (leftE[2] == rightE[2]) {
    output = (leftE[1] + rightE[1]) / leftE[2];
    return true;
  }
  output = (leftE[1] * rightE[2] + rightE[1] * leftE[2]) / (leftE[2] * rightE[2]);
  return true;
}

template<class Coefficient>
void Polynomial<Coefficient>::getPolynomialWithPolynomialCoefficient(
  Selection& theNonCoefficientVariables, Polynomial<Polynomial<Coefficient> >& output
) const {
  MacroRegisterFunctionWithName("Polynomial::getPolynomialWithPolynomialCoefficient");
  if (theNonCoefficientVariables.numberOfElements != this->minimalNumberOfVariables()) {
    global.fatal << "getPolynomialWithPolynomialCoefficient called with selection which has "
    << "selects the wrong number of variables. " << global.fatal;
  }
  output.makeZero();
  MonomialP coeffPart, polyPart;
  Polynomial<Coefficient> currentCF;
  for (int i = 0; i < this->size(); i ++) {
    coeffPart.makeOne();
    polyPart.makeOne();
    for (int j = 0; j < (*this)[i].minimalNumberOfVariables(); j ++) {
      if (theNonCoefficientVariables.selected[j]) {
        polyPart.setVariable(j, (*this)[i](j));
      } else {
        coeffPart.setVariable(j, (*this)[i](j));
      }
    }
    currentCF.makeZero();
    currentCF.addMonomial(coeffPart, this->coefficients[i]);
    output.addMonomial(polyPart, currentCF);
  }
}

template<class Coefficient>
void Polynomial<Coefficient>::getPolynomialUnivariateWithPolynomialCoefficients(
  int theVar, Polynomial<Polynomial<Coefficient> >& output
) const {
  Selection theVars;
  theVars.initialize(this->minimalNumberOfVariables());
  theVars.addSelectionAppendNewIndex(theVar);
  this->getPolynomialWithPolynomialCoefficient(theVars, output);
}

template <class Coefficient>
bool Polynomial<Coefficient>::getLinearSystemFromLinearPolynomials(
  const List<Polynomial<Coefficient> >& theLinPolys,
  Matrix<Coefficient>& homogenousPart,
  Matrix<Coefficient>& constTerms
) {
  MacroRegisterFunctionWithName("Polynomial::getLinearSystemFromLinearPolynomials");
  int theLetter;
  int numVars = 0;
  for (int i = 0; i < theLinPolys.size; i ++) {
    numVars = MathRoutines::maximum(theLinPolys[i].minimalNumberOfVariables(), numVars);
  }
  homogenousPart.initialize(theLinPolys.size, numVars);
  homogenousPart.makeZero();
  constTerms.initialize(theLinPolys.size, 1);
  constTerms.makeZero();
  for (int i = 0; i < theLinPolys.size; i ++) {
    for (int j = 0; j < theLinPolys[i].size(); j ++) {
      if (theLinPolys[i][j].isLinearNoConstantTerm(&theLetter)) {
        homogenousPart(i, theLetter) = theLinPolys[i].coefficients[j];
      } else if (theLinPolys[i][j].isConstant()) {
        constTerms(i, 0) = theLinPolys[i].coefficients[j];
        constTerms(i, 0) *= - 1;
      } else {
        return false;
      }
    }
  }
  return true;
}

class IntegralRationalFunctionComputation {
public:
  RationalFunction<Rational> theRF;
  Polynomial<Rational> theDen, theNum;
  Polynomial<Rational> quotientRat, remainderRat;
  List<Polynomial<Rational> > theFactors;
  Polynomial<AlgebraicNumber> thePolyThatMustVanish;
  Polynomial<AlgebraicNumber> remainderRescaledAlgebraic;
  RationalFunction<Rational> transformedRF;
  FormatExpressions currentFormaT;
  Expression integrationSetE;
  ExpressionContext context;
  Expression inpuTE;
  Expression outputIntegralE;
  AlgebraicNumber additionalMultiple;
  Calculator* owner;
  bool allFactorsAreOfDegree2orless;
  bool needPolyDivision;
  int numberOfSystemVariables;
  std::stringstream printoutPFsHtml;
  std::stringstream printoutPFsLatex;
  std::stringstream printoutIntegration;
  std::string stringPolyIndentityNonSimplifiedLatex;
  std::string stringRationalFunctionLatex;
  std::string stringRationalFunctionPartialFractionLatex;
  std::string stringFinalAnswer;
  std::string stringDenominatorFactors;
  LinearCombination<Polynomial<AlgebraicNumber>, Rational> theDenominatorFactorsWithMults;
  List<List<Polynomial<AlgebraicNumber> > > theNumerators;
  List<Expression> thePFSummands;
  List<Expression> theIntegralSummands;
  Expression theIntegralSum;
  bool computePartialFractionDecomposition();
  void prepareFormatExpressions();
  void prepareDenominatorFactors();
  void prepareNumerators();
  void prepareFinalAnswer();
  bool preparePartialFractionExpressionSummands();
  std::string toStringRationalFunctionLatex();
  bool integrateRationalFunction();
  IntegralRationalFunctionComputation(Calculator* inputOwner): owner(inputOwner) {}
  bool checkConsistency() const;
};

bool IntegralRationalFunctionComputation::checkConsistency() const {
  if (this->owner == nullptr) {
    global.fatal << "Non-initialized rf computation" << global.fatal;
  }
  return true;
}

bool IntegralRationalFunctionComputation::preparePartialFractionExpressionSummands() {
  MacroRegisterFunctionWithName("IntegralRationalFunctionComputation::preparePartialFractionExpressionSummands");
  this->checkConsistency();
  Expression polyE, currentNum, denExpE, currentDenNoPowerMonic,
  currentDen, currentPFnoCoeff, currentPFWithCoeff,
  coeffE;
  this->thePFSummands.setSize(0);
  Polynomial<AlgebraicNumber> denominatorRescaled, numeratorRescaled;
  AlgebraicNumber currentCoefficient, numScale;
  List<MonomialP>::Comparator* monomialOrder = &MonomialP::orderDefault();
  for (int i = 0; i < this->theNumerators.size; i ++) {
    for (int j = 0; j < this->theNumerators[i].size; j ++) {
      if (this->theNumerators[i][j].isEqualToZero()) {
        continue;
      }
      denominatorRescaled = this->theDenominatorFactorsWithMults[i];
      numeratorRescaled = this->theNumerators[i][j];
      denominatorRescaled.getIndexLeadingMonomial(nullptr, &currentCoefficient, monomialOrder);
      currentCoefficient.invert();
      denominatorRescaled *= currentCoefficient;
      MathRoutines::raiseToPower(currentCoefficient, j + 1, AlgebraicNumber(1));
      numeratorRescaled.getIndexLeadingMonomial(nullptr, &numScale, monomialOrder);
      numeratorRescaled /= numScale;
      currentCoefficient *= numScale;
      polyE.assignValueWithContext(numeratorRescaled, this->context, *this->owner);
      if (!CalculatorConversions::functionExpressionFromBuiltInType(*this->owner, polyE, currentNum)) {
        return false;
      }
      polyE.assignValueWithContext(denominatorRescaled, this->context, *this->owner);
      if (!CalculatorConversions::functionExpressionFromBuiltInType(*this->owner, polyE, currentDenNoPowerMonic)) {
        return false;
      }
      if (j != 0) {
        denExpE.assignValue(j + 1, *this->owner);
        currentDen.makeXOX(*this->owner, this->owner->opThePower(), currentDenNoPowerMonic, denExpE);
      } else {
        currentDen = currentDenNoPowerMonic;
      }
      currentPFnoCoeff = currentNum;
      currentPFnoCoeff /= currentDen;
      coeffE.assignValue(currentCoefficient, *this->owner);
      currentPFWithCoeff = coeffE * currentPFnoCoeff;
      currentPFWithCoeff.checkConsistencyRecursively();
      this->thePFSummands.addOnTop(currentPFWithCoeff);
    }
  }
  if (!this->quotientRat.isEqualToZero()) {
    Expression currentPFpolyForm;
    currentPFpolyForm.assignValueWithContext(
      this->quotientRat, this->context, *this->owner
    );
    if (!CalculatorConversions::functionExpressionFromPoly<Rational>(
      *this->owner, currentPFpolyForm, currentPFWithCoeff
    )) {
      *this->owner
      << "<br>Something is wrong: failed to convert polynomial "
      << currentPFpolyForm.toString()
      << " to expression. This shouldn't happen. ";
      return false;
    }
    currentPFWithCoeff.checkConsistencyRecursively();
    this->thePFSummands.addOnTop(currentPFWithCoeff);
  }
  return true;
}

bool IntegralRationalFunctionComputation::integrateRationalFunction() {
  MacroRegisterFunctionWithName("IntegralRationalFunctionComputation::integrateRationalFunction");
  this->checkConsistency();
  if (!this->computePartialFractionDecomposition()) {
    printoutIntegration
    << "Failed to decompose rational function into partial fractions. "
    << this->printoutPFsHtml.str();
    return false;
  }
  printoutIntegration << this->printoutPFsHtml.str();
  Expression polyE, currentNum, denExpE, currentDenNoPowerMonic, currentDen, currentIntegrand,
  currentIntegralNoCoeff, currentIntegralWithCoeff, coeffE;
  this->theIntegralSummands.setSize(0);
  Polynomial<AlgebraicNumber> denRescaled, numRescaled;
  AlgebraicNumber currentCoefficient, numScale;
  List<MonomialP>::Comparator* monomialOrder = &MonomialP::orderDefault();
  for (int i = 0; i < this->theNumerators.size; i ++) {
    for (int j = 0; j < this->theNumerators[i].size; j ++) {
      if (this->theNumerators[i][j].isEqualToZero()) {
        continue;
      }
      denRescaled = this->theDenominatorFactorsWithMults[i];
      numRescaled = this->theNumerators[i][j];
      currentCoefficient = denRescaled.getLeadingCoefficient(monomialOrder);
      currentCoefficient.invert();
      denRescaled *= currentCoefficient;
      MathRoutines::raiseToPower(currentCoefficient, j + 1, AlgebraicNumber(1));
      numScale = numRescaled.getLeadingCoefficient(monomialOrder);
      numRescaled /= numScale;
      currentCoefficient *= numScale;
      polyE.assignValueWithContext(numRescaled, this->context, *this->owner);
      if (!CalculatorConversions::functionExpressionFromBuiltInType(*this->owner, polyE, currentNum)) {
        return false;
      }
      polyE.assignValueWithContext(denRescaled, this->context, *this->owner);
      if (!CalculatorConversions::functionExpressionFromBuiltInType(*this->owner, polyE, currentDenNoPowerMonic)) {
        return false;
      }
      if (j != 0) {
        denExpE.assignValue(j + 1, *this->owner);
        currentDen.makeXOX(*this->owner, this->owner->opThePower(), currentDenNoPowerMonic, denExpE);
      } else {
        currentDen = currentDenNoPowerMonic;
      }
      currentIntegrand = currentNum;
      currentIntegrand /= currentDen;
      currentIntegralNoCoeff.makeIntegral(
        *this->owner,
        this->integrationSetE,
        currentIntegrand,
        this->context.getVariable(0)
      );
      coeffE.assignValue(currentCoefficient, *this->owner);
      currentIntegralWithCoeff = coeffE * currentIntegralNoCoeff;
      currentIntegralWithCoeff.checkConsistencyRecursively();
      this->theIntegralSummands.addOnTop(currentIntegralWithCoeff);
    }
  }
  if (!this->quotientRat.isEqualToZero()) {
    Expression currentIntegrandPolyForm;
    currentIntegrandPolyForm.assignValueWithContext(
      this->quotientRat, this->context, *this->owner
    );
    if (!CalculatorConversions::functionExpressionFromPoly<Rational>(
      *this->owner, currentIntegrandPolyForm, currentIntegrand
    )) {
      *this->owner << "<br>Something is wrong: failed to convert polynomial "
      << currentIntegrandPolyForm.toString()
      << " to expression. This shouldn't happen. ";
      return false;
    }
    currentIntegralWithCoeff.makeIntegral(
      *this->owner,
      this->integrationSetE,
      currentIntegrand,
      this->context.getVariable(0)
    );
    currentIntegralWithCoeff.checkConsistencyRecursively();
    this->theIntegralSummands.addOnTop(currentIntegralWithCoeff);
  }
  this->theIntegralSum.makeSum(*this->owner, this->theIntegralSummands);
  this->theIntegralSum.checkConsistencyRecursively();
  return true;
}

void IntegralRationalFunctionComputation::prepareFormatExpressions() {
  MacroRegisterFunctionWithName("IntegralRationalFunctionComputation::prepareFormatExpressions");
  std::stringstream rfStream, polyStream;
  rfStream << transformedRF.toString(&this->currentFormaT) << " = ";
  polyStream << remainderRescaledAlgebraic.toString(&this->currentFormaT) << " = ";
  int varCounter = 0;
  for (int i = 0; i < this->theDenominatorFactorsWithMults.size(); i ++) {
    int tempSize = - 1;
    this->theDenominatorFactorsWithMults.coefficients[i].isSmallInteger(&tempSize);
    for (int k = 0; k < this->theDenominatorFactorsWithMults.coefficients[i]; k ++) {
      rfStream << "\\frac{";
      if (this->theDenominatorFactorsWithMults[i].totalDegree() > 1) {
        polyStream << "(";
      }
      for (int j = 0; j < this->theDenominatorFactorsWithMults[i].totalDegree(); j ++) {
        varCounter ++;
        std::stringstream varNameStream;
        varNameStream << "A_{" << varCounter << "} ";
        rfStream << varNameStream.str();
        polyStream << varNameStream.str();
        this->currentFormaT.polynomialAlphabet.addOnTop(varNameStream.str());
        if (j > 0) {
          rfStream << "x";
          polyStream << "x";
        }
        if (j > 1) {
          rfStream << "^{" << j << "}";
          polyStream << "^{" << j << "}";
        }
        if ((this->theDenominatorFactorsWithMults[i].totalDegree() - 1) != j) {
          rfStream << " + ";
          polyStream << " + ";
        }
      }
      if (this->theDenominatorFactorsWithMults[i].totalDegree() > 1) {
        polyStream << ")";
      }
      for (int j = 0; j < this->theDenominatorFactorsWithMults.size(); j ++) {
        Rational theExp = this->theDenominatorFactorsWithMults.coefficients[j];
        if (j == i) {
          theExp -= k + 1;
        }
        if (theExp == 0) {
          continue;
        }
        polyStream << "(" << this->theDenominatorFactorsWithMults[j].toString(&this->currentFormaT) << ")";
        if (theExp > 1) {
          polyStream << "^{" << theExp << "}";
        }
      }
      rfStream << "}{";
      if (k > 0) {
        rfStream << "(";
      }
      rfStream << this->theDenominatorFactorsWithMults[i].toString(&this->currentFormaT);
      if (k > 0) {
        rfStream << ")^{" << k + 1 << "}";
      }
      rfStream << "}";
      if (
        ((this->theDenominatorFactorsWithMults.coefficients[i] - 1) != k) ||
        (i != this->theDenominatorFactorsWithMults.size() - 1)
      ) {
        rfStream << "+";
        polyStream << "+";
      }
    }
  }
  this->stringRationalFunctionLatex = rfStream.str();
  this->stringPolyIndentityNonSimplifiedLatex = polyStream.str();
}

void IntegralRationalFunctionComputation::prepareNumerators() {
  MacroRegisterFunctionWithName("IntegralRationalFunctionComputation::prepareNumerators");
  this->transformedRF = this->remainderRat;
  this->transformedRF /= this->theDen;
  this->remainderRescaledAlgebraic = this->remainderRat;
  this->remainderRescaledAlgebraic /= additionalMultiple;
  this->numberOfSystemVariables = 0;
  Polynomial<AlgebraicNumber> currentSummand;
  MonomialP currentMon;
  this->thePolyThatMustVanish.makeZero();
  this->thePolyThatMustVanish -= remainderRescaledAlgebraic;
  this->theNumerators.setSize(this->theDenominatorFactorsWithMults.size());
  for (int i = 0; i < this->theDenominatorFactorsWithMults.size(); i ++) {
    int tempSize = - 1;
    this->theDenominatorFactorsWithMults.coefficients[i].isSmallInteger(&tempSize);
    this->theNumerators[i].setSize(tempSize);
    for (int k = 0; k < this->theDenominatorFactorsWithMults.coefficients[i]; k ++) {
      currentSummand.makeZero();
      this->theNumerators[i][k].makeZero();
      for (int j = 0; j < this->theDenominatorFactorsWithMults[i].totalDegree(); j ++) {
        this->numberOfSystemVariables ++;
        currentMon.makeEi(this->numberOfSystemVariables);
        currentMon.setVariable(0, j);
        this->theNumerators[i][k].addMonomial(currentMon, 1);
        currentSummand.addMonomial(currentMon, 1);
      }
      for (int j = 0; j < this->theDenominatorFactorsWithMults.size(); j ++) {
        Rational theExp = this->theDenominatorFactorsWithMults.coefficients[j];
        if (j == i) {
          theExp -= k + 1;
        }
        if (theExp == 0) {
          continue;
        }
        for (int p = 0; p < theExp; p ++) {
          currentSummand *= this->theDenominatorFactorsWithMults[j];
        }
      }
      this->thePolyThatMustVanish += currentSummand;
    }
  }
}

void IntegralRationalFunctionComputation::prepareFinalAnswer() {
  MacroRegisterFunctionWithName("IntegralRationalFunctionComputation::prepareFinalAnswer");
  std::stringstream rfComputedStream, answerFinalStream;
  for (int i = 0; i < theDenominatorFactorsWithMults.size(); i ++) {
    for (int k = 0; k < theDenominatorFactorsWithMults.coefficients[i]; k ++) {
      rfComputedStream << "\\frac{"
      << this->theNumerators[i][k].toString(&this->currentFormaT) << "}";
      rfComputedStream << "{";
      rfComputedStream << "("
      << theDenominatorFactorsWithMults[i].toString(&this->currentFormaT) << ")";
      if (k > 0) {
        rfComputedStream << "^{" << k + 1 << "}";
      }
      rfComputedStream << "}";
      if (
        ((theDenominatorFactorsWithMults.coefficients[i] - 1) != k) ||
        (i != theDenominatorFactorsWithMults.size() - 1)
      ) {
        rfComputedStream << "+";
      }
    }
  }
  this->stringRationalFunctionPartialFractionLatex = rfComputedStream.str();
  answerFinalStream << this->theRF.toString(&this->currentFormaT) << "=";
  if (!this->quotientRat.isEqualToZero()) {
    answerFinalStream << this->quotientRat.toString(&this->currentFormaT) << "+ ";
    answerFinalStream << this->transformedRF.toString(&this->currentFormaT) << "=";
  }
  if (!this->quotientRat.isEqualToZero()) {
    answerFinalStream << this->quotientRat.toString(&this->currentFormaT) << "+ ";
  }
  answerFinalStream << this->stringRationalFunctionPartialFractionLatex;
  this->stringFinalAnswer = answerFinalStream.str();
}

void IntegralRationalFunctionComputation::prepareDenominatorFactors() {
  MacroRegisterFunctionWithName("IntegralRationalFunctionComputation::prepareDenominatorFactors");
  this->printoutPFsHtml << "The rational function is: " << HtmlRoutines::getMathNoDisplay(
    "\\frac{" + this->theNum.toString(&this->currentFormaT) + "}{" + this->theDen.toString(&this->currentFormaT) + "}"
  )
  << ".";
  this->printoutPFsHtml << "<br>The denominator factors are: ";
  this->printoutPFsLatex << "We aim to decompose into partial fractions the following function "
  << "(the denominator has been factored). \\[\\frac{"
  << this->theNum.toString(&this->currentFormaT) << "}{" << this->theDen.toString(&this->currentFormaT) << "}="
  << "\\frac{" << this->theNum.toString(&this->currentFormaT)  << "}{ ";
  this->allFactorsAreOfDegree2orless = true;
  for (int i = 0; i < this->theFactors.size; i ++) {
    this->printoutPFsHtml << HtmlRoutines::getMathNoDisplay(this->theFactors[i].toString(&this->currentFormaT));
    bool needsParenthesis = this->theFactors[i].needsParenthesisForMultiplication();
    if (needsParenthesis) {
      this->printoutPFsLatex << "\\left(";
    }
    this->printoutPFsLatex << this->theFactors[i].toString(&this->currentFormaT);
    if (needsParenthesis) {
      this->printoutPFsLatex << "\\right)";
    }
    if (i != this->theFactors.size - 1) {
      this->printoutPFsHtml << ", ";
    }
    if (this->theFactors[i].totalDegree() > 2) {
      allFactorsAreOfDegree2orless = false;
    }
  }
  this->printoutPFsLatex << "}";
  this->printoutPFsLatex << "\\]";
  this->printoutPFsHtml << ". <br>";
}

bool IntegralRationalFunctionComputation::computePartialFractionDecomposition() {
  MacroRegisterFunctionWithName("IntegralRationalFunctionComputation::computePartialFractionDecomposition");
  this->checkConsistency();
  this->context = this->inpuTE.getContext();
  this->context.getFormat(this->currentFormaT);
  if (
    this->theRF.minimalNumberOfVariables() < 1 ||
    this->theRF.expressionType == this->theRF.typeConstant ||
    this->theRF.expressionType == this->theRF.typePolynomial
  ) {
    this->printoutPFsHtml << this->theRF.toString(&this->currentFormaT)
    << " is already split into partial fractions. ";
    return true;
  }
  this->theRF.getDenominator(this->theDen);
  this->theRF.getNumerator(this->theNum);
  this->theNum *= this->theDen.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  PolynomialFactorizationUnivariate<Rational, PolynomialFactorizationKronecker> factorization;
  if (!factorization.factor(
    this->theDen,
    &this->printoutPFsHtml,
    &this->printoutPFsHtml
  )) {
    this->printoutPFsHtml
    << "<hr>Failed to factor the denominator "
    << "of the rational function, I surrender. ";
    return false;
  }
  this->theNum /= factorization.constantFactor;
  this->theDen /= factorization.constantFactor;
  this->theFactors = factorization.reduced;
  Polynomial<Rational> tempP;
  tempP.makeConstant(1);
  for (int i = 0; i < this->theFactors.size; i ++) {
    tempP *= this->theFactors[i];
  }
  if (tempP != this->theDen) {
    global.fatal
    << "Something is very wrong: product of denominator factors is "
    << tempP.toString(&this->currentFormaT)
    << ", but the denominator equals: "
    << this->theDen.toString(&this->currentFormaT) << ". " << global.fatal;
  }
  this->printoutPFsLatex
  << "\\documentclass{article}\\usepackage{longtable}\\usepackage{xcolor}\\usepackage{multicol} "
  << "\\begin{document}";
  this->prepareDenominatorFactors();
  if (!allFactorsAreOfDegree2orless) {
    this->printoutPFsHtml
    << "There were factors (over the rationals) of "
    << "degree greater than 2. I surrender. ";
    return false;
  }
  List<MonomialP>::Comparator monomialOrder = MonomialP::orderDefault();
  this->currentFormaT.flagUseFrac = true;
  this->theNum.divideBy(
    this->theDen,
    this->quotientRat,
    this->remainderRat,
   & monomialOrder
  );
  needPolyDivision = !this->quotientRat.isEqualToZero();
  if (needPolyDivision) {
    this->printoutPFsHtml << "<br>The numerator "
    << HtmlRoutines::getMathNoDisplay(this->theNum.toString(&this->currentFormaT))
    << " divided by the denominator "
    << HtmlRoutines::getMathNoDisplay(theDen.toString(&this->currentFormaT))
    << " yields "
    << HtmlRoutines::getMathNoDisplay(this->quotientRat.toString(&this->currentFormaT))
    << " with remainder "
    << HtmlRoutines::getMathNoDisplay(this->remainderRat.toString(&this->currentFormaT)) << ". ";
    GroebnerBasisComputation<Rational> computation;
    computation.flagDoLogDivision = true;
    computation.flagStoreQuotients = true;
    computation.polynomialOrder.monomialOrder = MonomialP::orderDefault();
    computation.addBasisElementNoReduction(this->theDen);
    computation.theFormat = this->currentFormaT;
    computation.polynomialOrder.monomialOrder = monomialOrder;
    Polynomial<Rational> theNumCopy = this->theNum;
    computation.remainderDivisionByBasis(theNumCopy, computation.remainderDivision, - 1);
    this->printoutPFsLatex << "Here is a detailed long polynomial division. ";
    this->printoutPFsLatex << computation.divisionReport.getElement().getDivisionStringLaTeX();
    this->printoutPFsHtml << "<br>Here is a detailed long polynomial division:<br> ";
    this->printoutPFsHtml << computation.divisionReport.getElement().getDivisionStringHtml();
  }
  LinearCombination<Polynomial<Rational>, Rational> theDenominatorFactorsWithMultsCopy;
  theDenominatorFactorsWithMultsCopy.makeZero();
  for (int i = 0; i < this->theFactors.size; i ++) {
    theDenominatorFactorsWithMultsCopy.addMonomial(this->theFactors[i], 1);
  }
  theDenominatorFactorsWithMultsCopy.quickSortAscending();
  Polynomial<Rational> currentSecondDegreePoly;
  this->theDenominatorFactorsWithMults.makeZero();
  Polynomial<AlgebraicNumber> currentLinPoly, currentSecondDegreePolyAlgebraic;
  this->additionalMultiple = 1;
  for (int i = 0; i < theDenominatorFactorsWithMultsCopy.size(); i ++) {
    currentSecondDegreePoly = theDenominatorFactorsWithMultsCopy[i];
    currentSecondDegreePolyAlgebraic = currentSecondDegreePoly;
    if (currentSecondDegreePoly.totalDegree() != 2) {
      this->theDenominatorFactorsWithMults.addMonomial(
        currentSecondDegreePolyAlgebraic, theDenominatorFactorsWithMultsCopy.coefficients[i]
      );
      continue;
    }
    Rational theDiscriminant = currentSecondDegreePoly.getDiscriminant();
    if (theDiscriminant < 0) {
      this->theDenominatorFactorsWithMults.addMonomial(
        currentSecondDegreePolyAlgebraic, theDenominatorFactorsWithMultsCopy.coefficients[i]
      );
      continue;
    }
    AlgebraicNumber theDiscriminantSqrt;
    if (!theDiscriminantSqrt.assignRationalQuadraticRadical(
      theDiscriminant,
      this->owner->theObjectContainer.theAlgebraicClosure,
      &this->printoutPFsHtml
    )) {
      this->printoutPFsHtml
      << "Failed to take radical of "
      << theDiscriminant.toString()
      << " (radical too large?).";
      return false;
    }
    theDiscriminantSqrt.checkConsistency();
    AlgebraicNumber a = currentSecondDegreePoly.getCoefficientOf(MonomialP(0, 2));
    AlgebraicNumber b = currentSecondDegreePoly.getCoefficientOf(MonomialP(0, 1));
    a.checkConsistency();
    b.checkConsistency();
    currentLinPoly.makeMonomial(0, 1);
    currentLinPoly -= (- b + theDiscriminantSqrt) / (a * 2);
    this->theDenominatorFactorsWithMults.addMonomial(currentLinPoly, theDenominatorFactorsWithMultsCopy.coefficients[i]);
    currentLinPoly.makeMonomial(0, 1);
    currentLinPoly -= (- b - theDiscriminantSqrt) / (a * 2);
    this->theDenominatorFactorsWithMults.addMonomial(currentLinPoly, theDenominatorFactorsWithMultsCopy.coefficients[i]);
    additionalMultiple *= a;
  }
  this->theDenominatorFactorsWithMults.quickSortAscending();
  this->printoutPFsHtml << "<br><br>I need to find "
  << HtmlRoutines::getMathNoDisplay("A_i")
  << "'s so that I have the equality of rational functions: ";
  this->printoutPFsLatex
  << "We need to find $A_i$'s so that we have the following equality of rational functions. ";
  this->prepareNumerators();
  this->prepareFormatExpressions();
  this->printoutPFsHtml << HtmlRoutines::getMathNoDisplay(this->stringRationalFunctionLatex, - 1);
  this->printoutPFsHtml << "<br><br>After clearing denominators, we get the equality: ";
  this->printoutPFsLatex << "After clearing denominators, we get the following equality. ";
  this->printoutPFsHtml << "<br><br>" << HtmlRoutines::getMathNoDisplay(this->stringPolyIndentityNonSimplifiedLatex, - 1);
  this->printoutPFsLatex << "\\[" << this->stringPolyIndentityNonSimplifiedLatex << "\\]";
  Polynomial<Polynomial<AlgebraicNumber> > univariateThatMustDie;
  thePolyThatMustVanish.getPolynomialUnivariateWithPolynomialCoefficients(0, univariateThatMustDie);
  this->printoutPFsHtml << "<br><br>After rearranging we get that the following polynomial must vanish: "
  << HtmlRoutines::getMathNoDisplay(univariateThatMustDie.toString(&this->currentFormaT));
  this->printoutPFsLatex << "After rearranging we get that the following polynomial must vanish. Here, by ``vanish'' "
  << "we mean that the coefficients of the powers of $x$ must be equal to zero."
  << "\\[" << univariateThatMustDie.toString(&this->currentFormaT) << "\\]";
  this->printoutPFsHtml << "<br>Here, by ``vanish'', we mean that the coefficients in front of the powers of x must vanish.";
  Matrix<AlgebraicNumber> theSystemHomogeneous, theSystemHomogeneousForLaTeX, theConstTerms, theConstTermsForLaTeX;
  Polynomial<AlgebraicNumber>::getLinearSystemFromLinearPolynomials(univariateThatMustDie.coefficients, theSystemHomogeneous, theConstTerms);
  theSystemHomogeneousForLaTeX = theSystemHomogeneous;
  theConstTermsForLaTeX = theConstTerms;
  this->currentFormaT.flagFormatMatrixAsLinearSystem = true;
  this->printoutPFsHtml << "<br>In other words, we need to solve the system: "
  << HtmlRoutines::getMathNoDisplay(theSystemHomogeneous.toStringSystemLatex(&theConstTerms, &this->currentFormaT), - 1);
  this->printoutPFsLatex << "In other words, we need to solve the following system. "
  << "\\[" << theSystemHomogeneous.toStringSystemLatex(&theConstTerms, &this->currentFormaT) << "\\]";
  this->currentFormaT.flagUseHTML = true;
  theSystemHomogeneous.gaussianEliminationByRows(&theConstTerms, nullptr, nullptr, &this->printoutPFsHtml, &this->currentFormaT);
  this->currentFormaT.flagUseHTML = false;
  theSystemHomogeneousForLaTeX.gaussianEliminationByRows(&theConstTermsForLaTeX, nullptr, nullptr, &this->printoutPFsLatex, &this->currentFormaT);
  PolynomialSubstitution<AlgebraicNumber> substitution;
  substitution.makeIdentitySubstitution(this->numberOfSystemVariables + 1);
  for (int i = 1; i < substitution.size; i ++) {
    substitution[i].makeConstant(theConstTerms(i - 1, 0));
  }
  for (int i = 0; i < theDenominatorFactorsWithMults.size(); i ++) {
    for (int k = 0; k < theDenominatorFactorsWithMults.coefficients[i]; k ++) {
      this->theNumerators[i][k].substitution(
        substitution,
        this->owner->theObjectContainer.theAlgebraicClosure.one()
      );
    }
  }
  this->prepareFinalAnswer();
  this->printoutPFsHtml << "<br>Therefore, the final partial fraction decomposition is: "
  << HtmlRoutines::getMathNoDisplay(this->stringFinalAnswer);
  this->printoutPFsLatex << "Therefore, the final partial fraction decomposition is the following. "
  << "\\[" << this->stringFinalAnswer << "\\]";
  this->printoutPFsLatex << "\\end{document}";
  this->printoutPFsHtml << "<hr>The present printout, in latex format, in ready form "
  << "for copy + paste to your latex editor, follows<hr> ";
  this->printoutPFsHtml << this->printoutPFsLatex.str();
  return true;
}

bool CalculatorFunctions::innerSplitToPartialFractionsOverAlgebraicRealS(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctions::functionSplitToPartialFractionsOverAlgebraicReals(
    calculator, input[1], output
  );
}

bool CalculatorFunctions::functionSplitToPartialFractionsOverAlgebraicReals(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSplitToPartialFractionsOverAlgebraicReals");
  IntegralRationalFunctionComputation theComputation(&calculator);
  bool isGood = CalculatorConversions::functionRationalFunction<Rational>(calculator, input, theComputation.inpuTE);
  if (isGood) {
    isGood = theComputation.inpuTE.isOfType<RationalFunction<Rational> >();
  }
  if (!isGood) {
    return calculator << "CalculatorFunctions::innerSplitToPartialFractionsOverAlgebraicReals: "
    << "Failed to convert "
    << input.toString() << " to rational function. ";
  }
  theComputation.theRF = theComputation.inpuTE.getValue<RationalFunction<Rational> >();
  if (theComputation.theRF.minimalNumberOfVariables() > 1) {
    return calculator << "The input rational function is of "
    << theComputation.theRF.minimalNumberOfVariables() << " variables and "
    << " I can handle only 1.";
  }
  if (!theComputation.computePartialFractionDecomposition()) {
    return calculator << "Did not manage do decompose "
    << input.toString() << " into partial fractions. ";
  }
  if (!theComputation.preparePartialFractionExpressionSummands()) {
    return calculator << "Something went wrong while collecting summands "
    << "while splitting "
    << input.toString() << " into partial fractions. ";
  }
  return output.makeSequence(calculator, &theComputation.thePFSummands);
}

bool CalculatorFunctions::innerSplitToPartialFractionsOverAlgebraicRealsAlgorithm(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSplitToPartialFractionsOverAlgebraicReals");
  IntegralRationalFunctionComputation theComputation(&calculator);
  bool isGood = CalculatorConversions::innerRationalFunction(calculator, input, theComputation.inpuTE);
  if (isGood) {
    isGood = theComputation.inpuTE.isOfType<RationalFunction<Rational> >();
  }
  if (!isGood) {
    return calculator << "CalculatorFunctions::innerSplitToPartialFractionsOverAlgebraicReals: "
    << "Failed to convert "
    << input.toString() << " to rational function. ";
  }
  theComputation.theRF = theComputation.inpuTE.getValue<RationalFunction<Rational> >();
  if (theComputation.theRF.minimalNumberOfVariables() > 1) {
    return calculator << "The input rational function is of "
    << theComputation.theRF.minimalNumberOfVariables() << " variables and "
    << " I can handle only 1. ";
  }
  theComputation.computePartialFractionDecomposition();
  return output.assignValue(theComputation.printoutPFsHtml.str(), calculator);
}

bool CalculatorFunctions::innerGaussianEliminationMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("innerGaussianEliminationMatrix");
  Expression theConverted;
  if (!CalculatorConversions::innerMakeMatrix(
    calculator, input, theConverted
  )) {
    return calculator
    << "<hr>Failed to extract algebraic number matrix from "
    << input.toString() << ". ";
  }
  Matrix<AlgebraicNumber> theMat;
  if (!calculator.functionGetMatrix(theConverted, theMat)) {
    return calculator
    << "<hr>Failed to extract algebraic number matrix, "
    << "got intermediate conversion to: "
    << theConverted.toString();
  }
  if (theMat.numberOfRows < 2) {
    return calculator
    << "<hr>The matrix I got as input had only 1 row. Possible user typo?";
  }
  std::stringstream out;
  FormatExpressions theFormat;
  theFormat.flagUseFrac = true;
  theMat.gaussianEliminationByRows(nullptr, nullptr, nullptr, &out, &theFormat);
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerCompositeConstTimesAnyActOn(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompositeConstTimesAnyActOn");
  if (!input.isListNElements()) {
    return false;
  }
  if (!input[0].startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (!input[0][1].isConstantNumber()) {
    return false;
  }
  Expression functionActsOnE;
  functionActsOnE.reset(calculator);
  functionActsOnE.addChildOnTop(input[0][2]);
  functionActsOnE.addChildOnTop(input[1]);
  calculator.checkInputNotSameAsOutput(input, output);
  return output.makeXOX(calculator, calculator.opTimes(), input[0][1], functionActsOnE);
}

bool CalculatorFunctions::innerCompositeEWAactOnPoly(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompositeEWAactOnPoly");
  if (input.size() != 2) {
    return false;
  }
  Expression theEWAE = input[0];
  Expression theArgument = input[1];
  if (!theEWAE.isListStartingWithAtom(calculator.opElementWeylAlgebra())) {
    return false;
  }
  if (!theArgument.isBuiltInType()) {
    return false;
  }
  if (!theEWAE.mergeContexts(theEWAE, theArgument)) {
    return false;
  }
  Polynomial<Rational> theArgumentPoly;
  Expression theArgumentConverted;
  if (theArgument.convertInternally<Polynomial<Rational> >(theArgumentConverted)) {
    theArgumentPoly = theArgumentConverted.getValue<Polynomial<Rational> >();
  } else if (theArgument.convertInternally<ElementWeylAlgebra<Rational> >(theArgumentConverted)) {
    if (!theArgumentConverted.getValue<ElementWeylAlgebra<Rational> >().isPolynomial(&theArgumentPoly)) {
      return false;
    }
  } else {
    return false;
  }
  const ElementWeylAlgebra<Rational>& theEWA = theEWAE.getValue<ElementWeylAlgebra<Rational> >();
  if (theEWA.hasNonSmallPositiveIntegerDerivation()) {
    return calculator << "<hr> I cannot apply " << theEWA.toString()
    << " onto " << theArgumentPoly.toString() << " as "
    << "the differential operator contains non-integral differential operator exponents. ";
  }
  if (!theEWA.actOnPolynomial(theArgumentPoly)) {
    std::stringstream out;
    out << "Failed to act by operator " << theEWA.toString() << " on polynomial " << theArgumentPoly.toString()
    << " (possibly the weyl algebra element has non-integral exponents)";
    return output.makeError(out.str(), calculator);
  }
  return output.assignValueWithContext(theArgumentPoly, theEWAE.getContext(), calculator);
}

bool CalculatorFunctions::innerFormatCPPSourceCode(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!global.userDefaultHasAdminRights()) {
    return calculator << "Cpp code formatting available only to logged-in admins. ";
  }
  std::string fileName;
  if (!input.isOfType(&fileName)) {
    return false;
  }
  std::stringstream report;
  CodeFormatter formatter;
  formatter.formatCPPSourceCode(fileName, "", &report);
  return output.assignValue(report.str(), calculator);
}

bool CalculatorFunctions::innerformatCPPDirectory(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!global.userDefaultHasAdminRights()) {
    return calculator << "Cpp code formatting available only to logged-in admins. ";
  }
  std::string fileName;
  if (!input.isOfType(&fileName)) {
    return false;
  }
  std::stringstream report;
  CodeFormatter::formatCPPDirectory(fileName, &report);
  return output.assignValue(report.str(), calculator);
}

bool CalculatorFunctionsListsAndSets::intersection(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsListsAndSets::intersection");
  if (!input.startsWith(calculator.opIntersection())) {
    return false;
  }
  int numElts = 1;
  for (int i = 1; i < input.size(); i ++) {
    if (!input[i].isSequenceNElements()) {
      return false;
    } else {
      numElts += input[i].size() - 1;
    }
  }
  List<List<Expression> > theListsToBeIntersected;
  theListsToBeIntersected.setSize(input.size() - 1);
  for (int i = 1; i < input.size(); i ++) {
    theListsToBeIntersected[i - 1].reserve(input[i].size());
    for (int j = 1; j < input[i].size(); j ++) {
      theListsToBeIntersected[i - 1].addOnTop(input[i][j]);
    }
  }
  if (theListsToBeIntersected.size == 0) {
    return false;
  }
  List<Expression> outputList = theListsToBeIntersected[0];
  for (int i = 1; i < theListsToBeIntersected.size; i ++) {
    outputList.intersectWith(theListsToBeIntersected[i], outputList);
  }
  return output.makeSequence(calculator, &outputList);
}


bool CalculatorFunctionsListsAndSets::listUnion(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsListsAndSets::listUnion");
  if (!input.startsWith(calculator.opUnion())) {
    return false;
  }
  int numElts = 1;
  for (int i = 1; i < input.size(); i ++) {
    if (!input[i].isSequenceNElements()) {
      return false;
    }
  }
  output.reset(calculator, numElts);
  output.addChildAtomOnTop(calculator.opSequence());
  for (int i = 1; i < input.size(); i ++) {
    for (int j = 1; j < input[i].size(); j ++) {
      output.addChildOnTop(input[i][j]);
    }
  }
  return true;
}

bool CalculatorFunctionsListsAndSets::unionNoRepetition(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsListsAndSets::unionNoRepetition");
  if (!input.isList()) {
    return false;
  }
  int numElts = 1;
  for (int i = 1; i < input.size(); i ++) {
    if (!input[i].isListStartingWithAtom(calculator.opSequence())) {
      return false;
    } else {
      numElts += input[i].size() - 1;
    }
  }
  HashedList<Expression> theList;
  List<int> theIndices;
  theList.setExpectedSize(numElts);
  for (int i = 1; i < input.size(); i ++) {
    for (int j = 1; j < input[i].size(); j ++) {
      theList.addOnTopNoRepetition(input[i][j]);
    }
  }
  theIndices.setSize(theList.size);
  for (int i = 0; i < theList.size; i ++) {
    theIndices[i] = calculator.addChildExpression(theList[i]);
  }
  output.children.reserve(numElts);
  output.reset(calculator, theIndices.size + 1);
  output.addChildAtomOnTop(calculator.opSequence());
  output.children.addListOnTop(theIndices);
  return true;
}

bool CalculatorFunctions::innerCrossProduct(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCrossProduct");
  if (!input.isListStartingWithAtom(calculator.opCrossProduct()) || input.children.size != 3) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!leftE.isSequenceNElements(3) || !rightE.isSequenceNElements(3)) {
    std::stringstream out;
    out << "Can't compute cross product of the non-3d vectors " << input[1].toString() << " and "
    << input[2].toString() << ". ";
    return output.makeError(out.str(), calculator);
  }
  List<Expression> outputSequence;
  outputSequence.setSize(3);
  outputSequence[0] = leftE[2] * rightE[3] - leftE[3] * rightE[2];
  outputSequence[1] = leftE[3] * rightE[1] - leftE[1] * rightE[3];
  outputSequence[2] = leftE[1] * rightE[2] - leftE[2] * rightE[1];
  return output.makeSequence(calculator, &outputSequence);
}

bool CalculatorFunctionsDifferentiation::differentiateConstPower(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsDifferentiation::differentiateConstPower");
  //////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& theDOvar = input[1];
  const Expression& theArgument = input[2];
  //////////////////////
  if (!theArgument.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  if (!theArgument[2].isConstantNumber()) {
    return false;
  }
  Expression theMonomial, theTerm, theExponent, basePrime, minusOne;
  minusOne.assignValue<Rational>(- 1, calculator);
  theExponent.makeXOX(calculator, calculator.opPlus(), theArgument[2], minusOne);
  theMonomial.makeXOX(calculator, calculator.opThePower(), theArgument[1], theExponent);
  basePrime.makeXOX(calculator, calculator.opDifferentiate(), theDOvar, theArgument[1]);
  theTerm.makeXOX(calculator, calculator.opTimes(), theArgument[2], theMonomial);
  return output.makeXOX(calculator, calculator.opTimes(), theTerm, basePrime);
}

bool CalculatorFunctionsDifferentiation::differentiateAPowerB(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsDifferentiation::differentiateAPowerB");
  //////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& theDOvar = input[1];
  const Expression& theArgument = input[2];
  //////////////////////
  // d/dx a^b= d/dx(e^{b\\ln a}) = a^b d/dx(b\\log a)
  if (!theArgument.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  Expression logarithmBase, exponentTimesLogBase, derivativeExponentTimesLogBase;
  logarithmBase.reset(calculator, 2);
  logarithmBase.addChildAtomOnTop(calculator.opLog());
  logarithmBase.addChildOnTop(theArgument[1]);
  exponentTimesLogBase.makeXOX(calculator, calculator.opTimes(), theArgument[2], logarithmBase);
  derivativeExponentTimesLogBase.makeXOX(calculator, calculator.opDifferentiate(), theDOvar, exponentTimesLogBase);
  return output.makeXOX(calculator, calculator.opTimes(), theArgument, derivativeExponentTimesLogBase);
}

bool CalculatorFunctionsDifferentiation::differentiateConstant(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsDifferentiation::differentiateConstant");
  //////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& theArgument = input[2];
  //////////////////////
  if (!theArgument.isConstantNumber()) {
    return false;
  }
  return output.assignValue<Rational>(0, calculator);
}

bool CalculatorFunctionsDifferentiation::differentiateX(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsDifferentiation::differentiateX");
  //////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& theDOvar = input[1];
  const Expression& theArgument = input[2];
  //////////////////////
  if (theArgument != theDOvar)
    return false;
  return output.assignValue<Rational>(1, calculator);
}

bool CalculatorFunctionsDifferentiation::differentiateTrigAndInverseTrig(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsDifferentiation::differentiateTrigAndInverseTrig");
  /////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator << "<hr>Warning: differentiating with respect to the non-atomic expression "
    << input[1].toString() << " - possible user typo?";
  }
  const Expression& theArgument = input[2];
  //////////////////////
  if (theArgument.isOperationGiven(calculator.opSin())) {
    return output.makeAtom(calculator.opCos(), calculator);
  }
  if (theArgument.isOperationGiven(calculator.opCos())) {
    Expression mOneE, sinE;
    mOneE.assignValue<Rational>(- 1, calculator);
    sinE.makeAtom(calculator.opSin(), calculator);
    return output.makeXOX(calculator, calculator.opTimes(), mOneE, sinE);
  }
  if (theArgument.isOperationGiven(calculator.opTan())) {
    Expression secE, twoE;
    secE.makeAtom(calculator.opSec(), calculator);
    twoE.assignValue(2, calculator);
    return output.makeXOX(calculator, calculator.opThePower(), secE, twoE);
  }
  if (theArgument.isOperationGiven(calculator.opCot())) {
    Expression cscE, twoE, cscSquared, mOneE;
    cscE.makeAtom(calculator.opCsc(), calculator);
    twoE.assignValue(2, calculator);
    cscSquared.makeXOX(calculator, calculator.opThePower(), cscE, twoE);
    mOneE.assignValue(- 1, calculator);
    return output.makeXOX(calculator, calculator.opTimes(), mOneE, cscSquared);
  }
  if (theArgument.isOperationGiven(calculator.opSec())) {
    Expression tanE, secE;
    tanE.makeAtom(calculator.opTan(), calculator);
    secE.makeAtom(calculator.opSec(), calculator);
    return output.makeXOX(calculator, calculator.opTimes(), tanE, secE);
  }
  if (theArgument.isOperationGiven(calculator.opCsc())) {
    Expression cotE, cscE, mOneE, cotTimesCscE;
    cotE.makeAtom(calculator.opCot(), calculator);
    cscE.makeAtom(calculator.opCsc(), calculator);
    mOneE.assignValue(- 1, calculator);
    cotTimesCscE.makeXOX(calculator, calculator.opTimes(), cotE, cscE);
    return output.makeXOX(calculator, calculator.opTimes(), mOneE, cotTimesCscE);
  }
  if (theArgument.isOperationGiven(calculator.opArcTan())) {
    Polynomial<Rational> onePlusXsquared;
    RationalFunction<Rational> oneOverOnePlusXsquared;
    onePlusXsquared.makeMonomial(0, 2);
    onePlusXsquared += Rational::one();
    oneOverOnePlusXsquared.makeOne();
    oneOverOnePlusXsquared /= onePlusXsquared;
    ExpressionContext context(calculator);
    context.makeOneVariableFromString("x");
    return output.assignValueWithContext(oneOverOnePlusXsquared, context, calculator);
  }
  if (theArgument.isOperationGiven(calculator.opArcSin())) {
    Expression denE;
    ExpressionContext context(calculator);
    context.makeOneVariableFromString("x");
    RationalFunction<Rational> oneMinusXsquared;
    oneMinusXsquared.makeMonomial(0, 2);
    oneMinusXsquared *= - 1;
    oneMinusXsquared += 1;
    denE.assignValueWithContext(oneMinusXsquared, context, calculator);
    return output.makeXOX(calculator, calculator.opThePower(), denE, calculator.expressionMinusHalf());
  }
  if (theArgument.isOperationGiven(calculator.opArcCos())) {
    Expression denE;
    ExpressionContext context;
    context.makeOneVariableFromString("x");
    RationalFunction<Rational> oneMinusXsquared;
    oneMinusXsquared.makeMonomial(0, 2);
    oneMinusXsquared *= - 1;
    oneMinusXsquared += 1;
    denE.assignValueWithContext(oneMinusXsquared, context, calculator);
    output.makeXOX(calculator, calculator.opThePower(), denE, calculator.expressionMinusHalf());
    output *= - 1;
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerCompositeDifferentiateLog(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompositeDifferentiateLog");
  /////////////////////
  if (input.size() != 2) {
    return false;
  }
  if (!input[0].startsWith(calculator.opDifferentiate(), 3)) {
    return false;
  }
  if (!input[0][2].isOperationGiven(calculator.opLog())) {
    return false;
  }
  calculator.checkInputNotSameAsOutput(input, output);
  Expression OneE;
  OneE.assignValue(1, calculator);
  output.reset(calculator, 2);
  return output.makeXOX(calculator, calculator.opDivide(), OneE, input[1]);
}

bool CalculatorFunctions::outerDivideByNumber(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::outerDivide");
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (input[2].isEqualToZero()) {
    return output.makeError("Division by zero. ", calculator);
  }
  Rational theRatValue;
  AlgebraicNumber theAlgValue;
  double theDoubleValue;
  Expression theInvertedE;
  bool result = false;
  if (input[2].isOfType<Rational>(&theRatValue)) {
    theRatValue.invert();
    theInvertedE.assignValue(theRatValue, calculator);
    result = true;
  }
  if (input[2].isOfType<AlgebraicNumber>(&theAlgValue)) {
    theAlgValue.invert();
    theInvertedE.assignValue(theAlgValue, calculator);
    result = true;
  }
  if (input[2].isOfType<double>(&theDoubleValue)) {
    theDoubleValue = 1 / theDoubleValue;
    theInvertedE.assignValue(theDoubleValue, calculator);
    result = true;
  }
  if (!result) {
    return false;
  }
  output = theInvertedE * input[1];
  return true;
}

bool CalculatorFunctions::innerMax(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerMin");
  (void) calculator;
  if (input.size() < 3) {
    return false;
  }
  double theMax = 0, current = 0;
  int bestIndex = 1;
  for (int i = 1; i < input.size(); i ++) {
    if (!input[i].evaluatesToDouble(&current)) {
      return false;
    }
    if (i == 1) {
      theMax = current;
    } else if (theMax < current) {
      theMax = current;
      bestIndex = i;
    }
  }
  output = input[bestIndex];
  return true;
}

bool CalculatorFunctions::innerMin(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerMin");
  (void) calculator;
  if (input.size() < 3) {
    return false;
  }
  double theMin = 0, current = 0;
  int bestIndex = 1;
  for (int i = 1; i < input.size(); i ++) {
    if (!input[i].evaluatesToDouble(& current)) {
      return false;
    }
    if (i == 1) {
      theMin = current;
    } else if (theMin > current) {
      theMin = current;
      bestIndex = i;
    }
  }
  output = input[bestIndex];
  return true;
}

bool CalculatorFunctions::innerEqualEqualEqual(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerEqualEqualEqual");
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression& left  = input[1];
  const Expression& right = input[2];
  int result = 0;
  if (left == right) {
    result = 1;
  }
  return output.assignValue(result, calculator);
}

bool CalculatorFunctions::outerEqualEqual(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerEqualEqual");
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression& left = input[1];
  const Expression& right = input[2];
  if (left.hasBoundVariables() || right.hasBoundVariables()) {
    return false;
  }
  if (left == right) {
    return output.assignValue(1, calculator);
  }
  if (left.isEqualToMathematically(right)) {
    return output.assignValue(1, calculator);
  } else {
    return output.assignValue(0, calculator);
  }
}

bool CalculatorFunctions::outerAssociateAdivBdivCpowerD(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerAssociateAdivBdivCpowerD");
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (!input[2].startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  if (!input[2][1].startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  calculator.checkInputNotSameAsOutput(input, output);
  Expression numeratorE, numeratorLeftE, denominatorE;
  output.reset(calculator, 3);
  numeratorLeftE.makeXOX(calculator, calculator.opThePower(), input[2][1][2], input[2][2]);
  numeratorE.makeXOX(calculator, calculator.opTimes(), input[1], numeratorLeftE);
  denominatorE.makeXOX(calculator, calculator.opThePower(), input[2][1][1], input[2][2]);
  return output.makeXOX(calculator, calculator.opDivide(), numeratorE, denominatorE);
}

bool CalculatorFunctions::outerDivCancellations(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerDivCancellations");
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (!input[1].startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (!input[2].startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (input[1][2] == input[2][2]) {
    return output.makeXOX(calculator, calculator.opDivide(), input[1][1], input[2][1]);
  }
  if (input[1][1] == input[2][1]) {
    return output.makeXOX(calculator, calculator.opDivide(), input[2][2], input[1][2]);
  }
  return false;
}

bool CalculatorFunctions::outerAssociateDivisionDivision(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerAssociateDivisionDivision");
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (input[1].startsWith(calculator.opDivide(), 3)) {
    Expression newRightE;
    newRightE.makeXOX(calculator, calculator.opTimes(), input[2], input[1][2]);
    return output.makeXOX(calculator, calculator.opDivide(), input[1][1], newRightE);
  }
  if (input[2].startsWith(calculator.opDivide(), 3)) {
    Expression newLeftE;
    newLeftE.makeXOX(calculator, calculator.opTimes(), input[1], input[2][2]);
    return output.makeXOX(calculator, calculator.opDivide(), newLeftE, input[2][1]);
  }
  return false;
}

bool CalculatorFunctionsDifferentiation::differentiateChainRule(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsDifferentiation::differentiateChainRule");
  /////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& theDOvar = input[1], theArgument = input[2];
  //////////////////////
  if (!theArgument.startsWith(- 1, 2)) {
    return false;
  }
  if (!theArgument.isGoodForChainRuleFunction() && !theArgument[0].isGoodForChainRuleFunction()) {
    return false;
  }
  calculator.checkInputNotSameAsOutput(input, output);
  output.reset(calculator);
  Expression multiplicandleft, multiplicandleftFunction, multiplicandright;
  multiplicandleftFunction.makeXOX(calculator, calculator.opDifferentiate(), theDOvar, theArgument[0]);
  multiplicandleft.reset(calculator);
  multiplicandleft.addChildOnTop(multiplicandleftFunction);
  multiplicandleft.addChildOnTop(theArgument[1]);
  multiplicandright.makeXOX(calculator, calculator.opDifferentiate(), theDOvar, theArgument[1]);
  return output.makeXOX(calculator, calculator.opTimes(), multiplicandleft, multiplicandright);
}

bool CalculatorFunctionsDifferentiation::differentiateAplusB(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsDifferentiation::differentiateAplusB");
  /////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator << "<hr>Warning: differentiating with respect to the non-atomic expression "
    << input[1].toString() << " - possible user typo?";
  }
  const Expression& theDOvar = input[1], theArgument = input[2];
  //////////////////////
  if (!theArgument.startsWith(calculator.opPlus(), 3)) {
    return false;
  }
  calculator.checkInputNotSameAsOutput(input, output);
  output.reset(calculator);
  Expression leftSummand, rightSummand;
  leftSummand.makeXOX(calculator, calculator.opDifferentiate(), theDOvar, theArgument[1]);
  rightSummand.makeXOX(calculator, calculator.opDifferentiate(), theDOvar, theArgument[2]);
  return output.makeXOX(calculator, calculator.opPlus(), leftSummand, rightSummand);
}

bool CalculatorFunctionsDifferentiation::differentiateAtimesB(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsDifferentiation::differentiateAtimesB");
    //////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& theDOvar = input[1], theArgument = input[2];
  //////////////////////
  if (!theArgument.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  calculator.checkInputNotSameAsOutput(input, output);
  output.reset(calculator);
  Expression changedMultiplicand, leftSummand, rightSummand;
  changedMultiplicand.makeXOX(calculator, calculator.opDifferentiate(), theDOvar, theArgument[1]);
  leftSummand.makeXOX(calculator, calculator.opTimes(), changedMultiplicand, theArgument[2]);
  changedMultiplicand.makeXOX(calculator, calculator.opDifferentiate(), theDOvar, theArgument[2]);
  rightSummand.makeXOX(calculator, calculator.opTimes(), theArgument[1], changedMultiplicand );
  return output.makeXOX(calculator, calculator.opPlus(), leftSummand, rightSummand);
}

bool CalculatorFunctions::innerPowerAnyToZero(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPowerAnyToZero");
  if (!input.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  if (!input[2].isEqualToZero()) {
    return false;
  }
  if (input[1].startsWith(calculator.opIntegral())) {
    if (input[1].size() != 3) { //<- an expression of the form \int_a^b (without f*dx).
      return false;
    }
  }
  if (input[1].isEqualToZero()) {
    return output.makeError("Error: expression of the form 0^0 is illegal.", calculator);
  }
  return output.assignValue<Rational>(1, calculator);
}

bool CalculatorFunctionsDifferentiation::differentiateAdivideBCommutative(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsDifferentiation::differentiateAdivideBCommutative");
    //////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator << "<hr>Warning: differentiating with respect to the non-atomic expression " << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& theDOvar = input[1], theArgument = input[2];
  //////////////////////
  //Quotient rule (commutative): (a/b^n)'= (a'b-n a b')/b^{n + 1}
  if (!theArgument.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  const Expression& numeratorE = theArgument[1];
  const Expression& denominatorE = theArgument[2];
  if (numeratorE.startsWith(calculator.opPlus())) {
    Expression leftE(calculator), rightE(calculator);
    leftE.addChildAtomOnTop(calculator.opDifferentiate());
    leftE.addChildOnTop(theDOvar);
    rightE = leftE;
    leftE.addChildOnTop(numeratorE[1] / denominatorE);
    rightE.addChildOnTop(numeratorE[2] / denominatorE);
    output = leftE + rightE;
    return true;
  }
  calculator.checkInputNotSameAsOutput(input, output);
  output.reset(calculator);
  Expression theDenominatorBase, eOne, theDenominatorExponentPlusOne, theDenominatorExponent, changedMultiplicand,
  leftSummand, rightSummand, theDenominatorFinal, numerator;
  eOne.assignValue(1, calculator);
  bool denBaseFound = false;
  if (theArgument[2].startsWith(calculator.opThePower(), 3)) {
    if (theArgument[2][2].isConstantNumber()) {
      denBaseFound = true;
      theDenominatorBase = theArgument[2][1];
      theDenominatorExponent = theArgument[2][2];
      theDenominatorExponentPlusOne.makeXOX(calculator, calculator.opPlus(), theDenominatorExponent, eOne);
    }
  }
  if (!denBaseFound) {
    theDenominatorBase = theArgument[2];
    theDenominatorExponentPlusOne.assignValue(2, calculator);
    theDenominatorExponent.assignValue(1, calculator);
  }
  theDenominatorFinal.makeXOX(calculator, calculator.opThePower(), theDenominatorBase, theDenominatorExponentPlusOne);
  changedMultiplicand.makeXOX(calculator, calculator.opDifferentiate(), theDOvar, theArgument[1]);
  leftSummand.makeXOX(calculator, calculator.opTimes(), changedMultiplicand, theDenominatorBase);
  rightSummand.makeXOX(calculator, calculator.opDifferentiate(), theDOvar, theDenominatorBase);
  changedMultiplicand.makeXOX(calculator, calculator.opTimes(), theArgument[1], rightSummand);
  rightSummand.makeXOX(calculator, calculator.opTimes(), theDenominatorExponent, changedMultiplicand);
  numerator.makeXOX(calculator, calculator.opMinus(), leftSummand, rightSummand);
  return output.makeXOX(calculator, calculator.opDivide(), numerator, theDenominatorFinal);
}

bool CalculatorFunctionsDifferentiation::differentiateAdivideBNONCommutative(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsDifferentiation::differentiateAdivideBNONCommutative");
    //////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator << "<hr>Warning: differentiating with respect to the non-atomic expression " << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& theDOvar = input[1], theArgument = input[2];
  //////////////////////
  //Quotient rule (non-commutative): (a/b)'= (ab^{- 1})'=a' b - a b^{- 1} b' b^{- 1}
  if (!theArgument.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  calculator.checkInputNotSameAsOutput(input, output);
  output.reset(calculator);
  Expression changedMultiplicand, leftSummand, rightSummand;
  Expression bInverse, bPrime, eMOne;
  eMOne.assignValue<Rational>(- 1, calculator);
  changedMultiplicand.makeXOX(calculator, calculator.opDifferentiate(), theDOvar, theArgument[1]);
  leftSummand.makeXOX(calculator, calculator.opDivide(), changedMultiplicand, theArgument[2]);
  bPrime.makeXOX(calculator, calculator.opDifferentiate(), theDOvar, theArgument[2]);
  bInverse.makeXOX(calculator, calculator.opThePower(), theArgument[2], eMOne);
  rightSummand.makeXOX(calculator, calculator.opTimes(), bPrime, bInverse); //rightSummand = b' b^{- 1}
  changedMultiplicand.makeXOX(calculator, calculator.opTimes(), bInverse, rightSummand); //changedMultiplicand = b^- 1 b' b^- 1
  rightSummand.makeXOX(calculator, calculator.opTimes(), theArgument[1], changedMultiplicand);
  return output.makeXOX(calculator, calculator.opMinus(), leftSummand, rightSummand);
}

bool CalculatorFunctions::outerCommuteAtimesBifUnivariate(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerCommuteAtimesBifUnivariate");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (input[1].isConstantNumber()) {
    return false;
  }
  HashedListSpecialized<Expression> theList;
  input.getBlocksOfCommutativity(theList);
  if (theList.size != 1) {
    return false;
  }
  if (input[2] > input[1] || input[2] == input[1]) {
    return false;
  }
  output = input;
  output.children.swapTwoIndices(1, 2);
  return true;
}

bool CalculatorFunctions::outerCommuteAtimesBtimesCifUnivariate(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerCommuteAtimesBtimesCifUnivariate");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  const Expression& leftExpression = input[1];
  if (leftExpression.isConstantNumber()) {
    return false;
  }
  if (!input[2].startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  const Expression& middleExpression = input[2][1];
  HashedListSpecialized<Expression> theList;

  leftExpression.getBlocksOfCommutativity(theList);
  middleExpression.getBlocksOfCommutativity(theList);
  if (theList.size != 1) {
    return false;
  }
  if (middleExpression > leftExpression || middleExpression == leftExpression) {
    return false;
  }
  Expression rightMultiplicand;
  rightMultiplicand.makeProduct(calculator,  leftExpression, input[2][2]);
  return output.makeXOX(calculator, calculator.opTimes(), middleExpression, rightMultiplicand);
}

bool CalculatorFunctions::innerGetFreeVariablesIncludeNamedConstants(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGetFreeVariables");
  if (input.size() != 2) {
    return false;
  }
  HashedList<Expression> outputList;
  if (!input[1].getFreeVariables(outputList, false)) {
    return calculator << "Function getFreeVariables failed, this shouldn't happen. ";
  }
  return output.makeSequence(calculator, &outputList);
}

bool CalculatorFunctions::innerGetFreeVariablesExcludeNamedConstants(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGetFreeVariables");
  if (input.size() != 2) {
    return false;
  }
  HashedList<Expression> outputList;
  if (!input[1].getFreeVariables(outputList, true)) {
    return calculator << "Function getFreeVariables failed, this shouldn't happen.";
  }
  return output.makeSequence(calculator, &outputList);
}

bool CalculatorFunctions::innerCompareFunctionsNumerically(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompareFunctionsNumerically");
  if (input.size() < 5) {
    return calculator << "Comparing functions takes as input at least 4 arguments "
    << "(two functions to compare and interval to compare over).";
  }
  Expression theFunE = input[1];
  theFunE -= input[2];
  HashedList<Expression> theVars;
  if (!theFunE.getFreeVariables(theVars, true)) {
    return calculator << "Was not able to extract the function argument of your function. " ;
  }
  if (theVars.size <= 0) {
    Expression zeroE;
    zeroE.assignValue(0, calculator);
    return output.makeXOX(calculator, calculator.opEqualEqual(), theFunE, zeroE);
  }
  if (theVars.size > 1) {
    return calculator << "I cannot compare the functions as they appear to depend on more than one variable, namely, on: "
    << theVars.toStringCommaDelimited();
  }
  double leftBoundary = 0;
  double rightBoundary = 0;
  if (!input[3].evaluatesToDouble(&leftBoundary)) {
    return calculator << "Failed to extract the left endpoint of the comparison interval. ";
  }
  if (!input[4].evaluatesToDouble(&rightBoundary)) {
    return calculator << "Failed to extract the right endpoint of the comparison interval. ";
  }
  int numPoints = 50;
  if (input.size() > 5) {
    if (!input[5].isSmallInteger(&numPoints)) {
      return calculator << "Failed to convert argument: " << input[5].toString() << " to a small integer. ";
    }
  }
  double minDiff = 0, maxDiff = 0;
  if (!theFunE.evaluatesToDoubleInRange(
    theVars[0].toString(), leftBoundary, rightBoundary, numPoints, &minDiff, &maxDiff, nullptr
  )) {
    return calculator << "Failed to evaluate your function to a number. "
    << "The sampling interval may be outside of the domain of the function. ";
  }
  double tolerance = 0.0001;
  if (input.size() > 6) {
    if (!input[6].evaluatesToDouble(&tolerance)) {
      return calculator << "Failed to evaluate the argument " << input[6].toString() << " to a floating point number. ";
    }
  }
  if (minDiff < - tolerance || maxDiff > tolerance) {
    return output.assignValue(0, calculator);
  }
  return output.assignValue(1, calculator);
}

bool CalculatorFunctions::innerCompareExpressionsNumericallyAtPoints(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompareExpressionsNumericallyAtPoints");
  if (input.size() < 5) {
    return calculator << "Comparing functions at points takes as input at least 5 arguments "
    << "- two functions to compare, precision, variable belonging to an interval and number of sampling points).";
  }
  Expression theFunE = input[1];
  theFunE -= input[2];
  HashedList<Expression> theFreeVars;
  if (!theFunE.getFreeVariables(theFreeVars, true)) {
    return calculator << "Was not able to extract the function argument of your function. " ;
  }
  if (theFreeVars.size <= 0) {
    Expression zeroE;
    zeroE.assignValue(0, calculator);
    return output.makeXOX(calculator, calculator.opEqualEqual(), theFunE, zeroE);
  }
  double tolerance = 0.0001;
  if (!input[3].evaluatesToDouble(&tolerance)) {
    return calculator << "Failed to evaluate tolerance limit from " << input[3].toString()
    << " to a floating point number. ";
  }
  if (tolerance < 0) {
    tolerance *= - 1;
  }
  if (!input[4].startsWith(calculator.opIn(), 3)) {
    return calculator << "The fourth argument " << input[4].toString() << " needs to be "
    << " of the form (x,y,...)\\in (...). ";
  }
  const Expression& theVarsE = input[4][1];
  HashedList<Expression> varsGiven;
  if (!theVarsE.isSequenceNElements()) {
    varsGiven.addOnTop(theVarsE);
  } else {
    for (int i = 1; i < theVarsE.size(); i ++) {
      if (varsGiven.contains(theVarsE[i])) {
        return calculator << theVarsE[i] << " given more than once. ";
      }
      varsGiven.addOnTop(theVarsE[i]);
    }
  }
  for (int i = 0; i < theFreeVars.size; i ++) {
    if (!varsGiven.contains(theFreeVars[i])) {
      return calculator << "The expression depends on "
      << theFreeVars[i].toString()
      << " but I no value given for that expression. ";
    }
  }
  const Expression& thePointsE = input[4][2];
  Matrix<double> thePoints;
  if (!calculator.functionGetMatrix(
    thePointsE,
    thePoints,
    nullptr,
    varsGiven.size,
    CalculatorFunctions::functionEvaluateToDouble
  )) {
    return calculator
    << "Failed to extract list of points from: "
    << thePointsE.toString();
  }
  HashedList<Expression> knownEs = calculator.knownDoubleConstants;
  List<double> knownValues = calculator.knownDoubleConstantValues;
  for (int i = 0; i < varsGiven.size; i ++) {
    if (knownEs.contains(varsGiven[i])) {
      return calculator << varsGiven[i]
      << " is an already known constant and cannot be used as a variable in this context. ";
    } else {
      knownEs.addOnTop(varsGiven[i]);
    }
  }
  knownValues.setSize(knownEs.size);
  int numFailedSamples = 0;
  int totalSamples = thePoints.numberOfRows;
  for (int i = 0; i < thePoints.numberOfRows; i ++) {
    for (int j = 0; j < thePoints.numberOfColumns; j ++) {
      knownValues[j + calculator.knownDoubleConstants.size] = thePoints(i, j);
    }
    double floatingResult = 0;
    if (!theFunE.evaluatesToDoubleUnderSubstitutions(knownEs, knownValues, &floatingResult)) {
      numFailedSamples ++;
      if ((numFailedSamples * 100) / totalSamples > 20) {
        calculator << "Failed to evaluate at least one of the functions in more than 20% of the sampling points. ";
        return output.assignValue(0, calculator);
      }
    }
    if (floatingResult > tolerance || floatingResult < - tolerance) {
      return output.assignValue(0, calculator);
    }
  }
  return output.assignValue(1, calculator);
}

bool CalculatorFunctions::innerCompareExpressionsNumerically(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompareFunctionsNumerically");
  if (input.size() < 6) {
    return calculator << "Comparing functions takes as input at least 5 arguments "
    << "- two functions to compare, precision, variable belonging to an interval and number of sampling points).";
  }
  Expression theFunE = input[1];
  theFunE -= input[2];
  HashedList<Expression> theVars;
  if (!theFunE.getFreeVariables(theVars, true)) {
    return calculator << "Was not able to extract the function argument of your function. " ;
  }
  if (theVars.size <= 0) {
    Expression zeroE;
    zeroE.assignValue(0, calculator);
    return output.makeXOX(calculator, calculator.opEqualEqual(), theFunE, zeroE);
  }
  List<double> leftBoundaries;
  List<double> rightBoundaries;
  HashedList<Expression> theBoundaryVars;
  List<int> numSamples;
  for (int i = 4; i < input.size(); i += 2) {
    const Expression& currentIntervalWithVariable = input[i];
    if (!currentIntervalWithVariable.startsWith(calculator.opIn(), 3)) {
      return calculator << "Expression " << currentIntervalWithVariable.toString()
      << " is not a belonging relation (does not use the \\in symbol). ";
    }
    if (!currentIntervalWithVariable[2].isSequenceNElements(2)) {
      return calculator << "Could not get a two-element sequence from " << currentIntervalWithVariable[2].toString();
    }
    double currentLeft, currentRight;
    if (!currentIntervalWithVariable[2][1].evaluatesToDouble(&currentLeft)) {
      return calculator << "Could not get a double from " << currentIntervalWithVariable[2][1].toString();
    }
    if (!currentIntervalWithVariable[2][2].evaluatesToDouble(&currentRight)) {
      return calculator << "Could not get a double from " << currentIntervalWithVariable[2][2].toString();
    }
    if (theBoundaryVars.contains(currentIntervalWithVariable[1])) {
      return calculator << "Expression " << currentIntervalWithVariable[1].toString()
      << " specified an interval range more than once. ";
    }
    theBoundaryVars.addOnTop(currentIntervalWithVariable[1]);
    leftBoundaries.addOnTop(currentLeft);
    rightBoundaries.addOnTop(currentRight);
    numSamples.addOnTop(10);
    int currentNumSamplingPoints = 0;
    if (i + 1 < input.size()) {
      if (input[i + 1].isSmallInteger(&currentNumSamplingPoints)) {
        *numSamples.lastObject() = currentNumSamplingPoints;
      } else {
        return calculator << "Failed to extract a small integer for the number of sampling points from: "
        << input[i + 1].toString();
      }
      if (*numSamples.lastObject() > 1000) {
        return calculator << *numSamples.lastObject() << " sampling points requested for variable/expression "
        << theBoundaryVars.lastObject()->toString() << "; this exceeds the hard-coded limit of 1000. ";
      }
      if (*numSamples.lastObject() < 1) {
        return calculator << *numSamples.lastObject() << " sampling points requested for variable/expression "
        << theBoundaryVars.lastObject()->toString() << "; this is not allowed. ";
      }
    }
  }
  for (int i = 0; i < theVars.size; i ++) {
    if (!theBoundaryVars.contains(theVars[i])) {
      return calculator << "Could not find an interval for variable/expression: " << theVars[i].toString();
    }
  }
  HashedList<Expression> knownEs = calculator.knownDoubleConstants;
  List<double> knownValues = calculator.knownDoubleConstantValues;
  for (int i = 0; i < theBoundaryVars.size; i ++) {
    if (knownEs.contains(theBoundaryVars[i])) {
      return calculator << theBoundaryVars[i]
      << " is an already known constant and cannot be used as a variable in this context. ";
    } else {
      knownEs.addOnTop(theBoundaryVars[i]);
    }
  }
  knownValues.setSize(knownEs.size);
  SelectionWithDifferentMaxMultiplicities theSamplingSelector;
  theSamplingSelector.initFromInts(numSamples);
  if (theSamplingSelector.totalNumberOfSubsets() > 1000000) {
    return calculator << "The total number of sampling points, "
    << theSamplingSelector.totalNumberOfSubsets().toString() << " exceeds "
    << "the hard-coded limit of 1000000. ";
  }
  double tolerance = 0.0001;
  if (!input[3].evaluatesToDouble(&tolerance)) {
    return calculator << "Failed to evaluate tolerance limit from " << input[3].toString()
    << " to a floating point number. ";
  }
  if (tolerance < 0) {
    tolerance *= - 1;
  }
  Rational totalSamples = theSamplingSelector.totalNumberSubsetsSmallInt();
  Rational numFailedSamples = 0;
  do {
    for (int i = 0; i < theSamplingSelector.multiplicities.size; i ++) {
      double& currentValue = knownValues[i + calculator.knownDoubleConstants.size];
      double& lBound = leftBoundaries[i];
      double& rBound = rightBoundaries[i];
      if (theSamplingSelector.capacities[i] == 1) {
        currentValue = (lBound + rBound) / 2;
        continue;
      }
      double paramBnZeroAndOne = theSamplingSelector.multiplicities[i];
      paramBnZeroAndOne /= theSamplingSelector.capacities[i] - 1;
      currentValue = lBound *(1 - paramBnZeroAndOne) + rBound * paramBnZeroAndOne;
    }
    double floatingResult = 0;
    if (!theFunE.evaluatesToDoubleUnderSubstitutions(knownEs, knownValues, &floatingResult)) {
      numFailedSamples ++;
      if ((numFailedSamples * 100) / totalSamples > 20) {
        calculator << "Failed to evaluate at least one of the functions in more than 20% of the sampling points. ";
        return output.assignValue(0, calculator);
      }
    }
    if (floatingResult > tolerance || floatingResult < - tolerance) {
      return output.assignValue(0, calculator);
    }
  } while (theSamplingSelector.incrementReturnFalseIfPastLast());
  return output.assignValue(1, calculator);
}

bool CalculatorFunctions::innerCompositeArithmeticOperationEvaluatedOnArgument(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompositeArithmeticOperationEvaluatedOnArgument");
  if (input.size() != 2) {
    return false;
  }
  if (input[0].size() != 3) {
    return false;
  }
  if (!input[0][0].isArithmeticOperation()) {
    return false;
  }
  Expression leftE(calculator);
  Expression rightE(calculator);
  leftE.addChildOnTop(input[0][1]);
  leftE.addChildOnTop(input[1]);
  rightE.addChildOnTop(input[0][2]);
  rightE.addChildOnTop(input[1]);
  return output.makeXOX(calculator, input[0][0].data, leftE, rightE);
}

bool CalculatorFunctions::innerIsEven(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsEven");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (argument.hasBoundVariables()) {
    return false;
  }
  LargeInteger theInt;
  if (!argument.isInteger(&theInt)) {
    return output.assignValue(0, calculator);
  }
  if (theInt.isEven()) {
    return output.assignValue(1, calculator);
  }
  return output.assignValue(0, calculator);
}

bool CalculatorFunctions::innerIsConstant(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsConstant");
  if (input.size() != 2) {
    return false;
  }
  int result = static_cast<int>(input[1].isConstantNumber());
  return output.assignValue(result, calculator);
}

bool CalculatorFunctions::innerIsNonEmptySequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsNonEmptySequence");
  if (input.hasBoundVariables()) {
    return false;
  }
  if (input.size() < 2) {
    return output.assignValue(0, calculator);
  }
  if (input.size() == 2) {
    if (input[1].isSequenceNElements() && input[1].size() > 0) {
      return output.assignValue(1, calculator);

    }
    return output.assignValue(0, calculator);
  }
  return output.assignValue(1, calculator);
}

bool CalculatorFunctions::innerDifferentialStandardHandler(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDifferentialStandardHandler");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  output.reset(calculator);
  output.addChildAtomOnTop(calculator.opDifferential());
  output.addChildOnTop(argument);
  return output.addChildRationalOnTop(1);
}

bool CalculatorFunctions::innerDifferentialOfPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDifferentialOfPolynomial");
  if (input.size() != 3) {
    return false;
  }
  WithContext<Polynomial<Rational> > polynomial;
  if (!input[1].isOfTypeWithContext(&polynomial)) {
    return false;
  }
  std::stringstream comments;
  Vector<Polynomial<Rational> > differentials;
  if (!polynomial.content.differential(differentials, &comments)) {
    return false;
  }
  List<Expression> outputSummands;
  for (int i = 0; i < differentials.size; i ++) {
    Expression incoming(calculator);
    Expression variable = polynomial.context.getVariable(i);
    incoming.addChildAtomOnTop(calculator.opDifferential());
    incoming.addChildOnTop(variable);
    Expression polynomialWrapper;
    polynomialWrapper.assignValueWithContext(
      differentials[i], polynomial.context, calculator
    );
    incoming.addChildOnTop(polynomialWrapper);
    outputSummands.addOnTop(incoming);
  }
  return output.makeSum(calculator, outputSummands);
}

bool CalculatorFunctions::innerIsDifferentialOneFormOneVariable(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsDifferentialOneFormOneVariable");
  if (input.size() != 2) {
    return false;
  }
  return output.assignValue(
    static_cast<int>(input[1].isDifferentialOneFormOneVariable()),
    calculator
  );
}

bool CalculatorFunctionsDifferentiation::interpretAsDifferential(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::interpretAsDifferential");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (!input[1].startsWith(calculator.opIntegral())) {
    return false;
  }
  const Expression& theDiffFormCandidateE = input[2];
  std::string theDiff;
  if (theDiffFormCandidateE.isOperation(&theDiff)) {
    if (theDiff.size() > 1) {
      if (theDiff[0] == 'd') {
        Expression variableE, diffFormE;
        variableE.makeAtom(theDiff.substr(1, std::string::npos), calculator);
        diffFormE.reset(calculator);
        diffFormE.addChildAtomOnTop(calculator.opDifferential());
        diffFormE.addChildOnTop(variableE);
        diffFormE.addChildRationalOnTop(1);
        output = input;
        return output.setChild(2, diffFormE);
      }
    }
  }
  return false;
}

bool CalculatorFunctionsIntegration::integralOperator(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::innerCompositeMultiplyIntegralFbyDx");
  int theOp = calculator.opTimes();
  if (!input.startsWith(theOp, 3)) {
    theOp = calculator.opDivide();
    if (!input.startsWith(theOp, 3)) {
      return false;
    }
  }
  const Expression& integralE = input[1];
  if (integralE.isOperationGiven(calculator.opIntegral())) {
    Expression integralOperatorE;
    integralOperatorE.reset(calculator, 2);
    integralOperatorE.addChildAtomOnTop(calculator.opIntegral());
    integralOperatorE.addChildAtomOnTop(calculator.opIndefiniteIndicator());
    output = input;
    return output.setChild(1, integralOperatorE);
  }
  if (!integralE.startsWith(calculator.opIntegral())) {
    return false;
  }
  if (integralE.size() < 3) {
    output = integralE;
    return output.addChildOnTop(input[2]);
  }
  if (integralE.size() != 3) {
    return false;
  }
  const Expression& startingIntegrand = integralE[2];
  if (startingIntegrand.isDifferentialOneFormOneVariable()) {
    return false;
  }
  const Expression& incomingIntegrand = input[2];
  Expression newIntegrand;
  Expression newFun;
  if (incomingIntegrand.isDifferentialOneFormOneVariable()) {
    newFun.makeXOX(calculator, theOp, startingIntegrand, incomingIntegrand[2]);
    newIntegrand.makeXOX(calculator, calculator.opDifferential(), incomingIntegrand[1], newFun);
    output = integralE;
    return output.setChild(2, newIntegrand);
  }
  newIntegrand.makeXOX(calculator, theOp, startingIntegrand, incomingIntegrand);
  output = integralE;
  return output.setChild(2, newIntegrand);
}

bool CalculatorFunctions::innerRationalFunctionSubstitution(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerRationalFunctionSubstitution");
  if (input.size() != 2) {
    return false;
  }
  if (!input[0].isOfType<RationalFunction<Rational> >()) {
    return false;
  }
  if (input[0].getValue<RationalFunction<Rational> >().minimalNumberOfVariables() > 1) {
    return false;
  }
  Expression resultRationalForm;
  ExpressionContext finalContext(calculator);
  finalContext.makeOneVariable(input[1]);
  resultRationalForm.assignValueWithContext(input[0].getValue<RationalFunction<Rational> >(), finalContext, calculator);
  return CalculatorConversions::innerExpressionFromRationalFunction<Rational>(calculator, resultRationalForm, output);
}

bool CalculatorFunctions::innerInvertMatrixRFsVerbose(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerInvertMatrixVerbose");
  Matrix<RationalFunction<Rational> > theMatrix, outputMat, extendedMatrix;
  Expression converted;
  if (!CalculatorConversions::innerMatrixRationalFunction(
    calculator, input, converted
  )) {
    return output.makeError("Failed to extract matrix. ", calculator);
  }
  if (calculator.functionGetMatrix(converted, theMatrix)) {
    return calculator << "Failed to get matrix of rational functions. ";
  }
  ExpressionContext theContext = converted.getContext();
  if (theMatrix.numberOfRows != theMatrix.numberOfColumns || theMatrix.numberOfColumns < 1) {
    std::stringstream out;
    out << "The matrix " << theMatrix.toString( ) << " has "
    << theMatrix.numberOfColumns << " columns and " << theMatrix.numberOfRows << " rows. "
    << "The matrix is not square.";
    return output.makeError(out.str(), calculator);
  }
  outputMat.makeIdentityMatrix(
  theMatrix.numberOfRows,RationalFunction<Rational>::oneRational(), RationalFunction<Rational>::zeroRational());
  int tempI;
  int NumFoundPivots = 0;
  std::stringstream out, outLaTeX;

  RationalFunction<Rational> tempElement;
  FormatExpressions theFormat;
  theContext.getFormat(theFormat);
  theFormat.flagUseLatex = true;
  theFormat.flagUseHTML = false;
  theFormat.flagUseFrac = true;
  theFormat.matrixColumnVerticalLineIndex = theMatrix.numberOfColumns - 1;
  out << "Computing "
  << HtmlRoutines::getMathNoDisplay(theMatrix.toString(&theFormat) + "^{- 1}");
  extendedMatrix = theMatrix;
  extendedMatrix.appendMatrixOnTheRight(outputMat);
  out << "<br>" << HtmlRoutines::getMathNoDisplay(extendedMatrix.toString(&theFormat)) ;
  outLaTeX << "\\begin{tabular}{ll}";
  outLaTeX << "$" << extendedMatrix.toString(& theFormat) << "$";

  for (int i = 0; i < theMatrix.numberOfColumns; i ++) {
    tempI = theMatrix.findPivot(i, NumFoundPivots);
    if (tempI != - 1) {
      if (tempI != NumFoundPivots) {
        theMatrix.switchRows(NumFoundPivots, tempI);
        outputMat.switchRows (NumFoundPivots, tempI);
        out << "<br>Swap row " << NumFoundPivots + 1 << " and row " << tempI + 1 << ": ";
        outLaTeX << "& Swap row " << NumFoundPivots + 1 << " and row " << tempI + 1 << ". ";
        extendedMatrix = theMatrix;
        extendedMatrix.appendMatrixOnTheRight(outputMat);
        out << "<br>" << HtmlRoutines::getMathNoDisplay(outputMat.toString(&theFormat));
        outLaTeX << "\\\\" << "$" << outputMat.toString(&theFormat) << "$";
      }
      tempElement = theMatrix(NumFoundPivots, i);
      tempElement.invert();
      if (tempElement != 1) {
        out << "<br> multiply row number " << NumFoundPivots + 1 << " by "
        << tempElement.toString(&theFormat) << ": ";
        outLaTeX << "& multiply row number " << NumFoundPivots + 1 << " by $"
        << tempElement.toString(&theFormat) << "$. \\\\";
      }
      theMatrix.rowTimesScalar(NumFoundPivots, tempElement);
      outputMat.rowTimesScalar(NumFoundPivots, tempElement);
      if (tempElement != 1) {
        extendedMatrix = theMatrix;
        extendedMatrix.appendMatrixOnTheRight(outputMat);
        out << HtmlRoutines::getMathNoDisplay(extendedMatrix.toString(&theFormat));
        outLaTeX << "$" << extendedMatrix.toString(&theFormat) << "$";
      }
      bool found = false;
      for (int j = 0; j < theMatrix.numberOfRows; j ++) {
        if (j != NumFoundPivots) {
          if (!theMatrix.elements[j][i].isEqualToZero()) {
            tempElement = theMatrix.elements[j][i];
            tempElement.negate();
            theMatrix.addTwoRows(NumFoundPivots, j, i, tempElement);
            outputMat.addTwoRows(NumFoundPivots, j, 0, tempElement);
            if (!found) {
              out << "<br>";
              outLaTeX << "&";
              outLaTeX << "\\begin{tabular}{l}";
            } else {
              out << ", ";
              outLaTeX << ", ";
            }
            found = true;
            out << " Row index " << NumFoundPivots + 1 << " times "
            << tempElement.toString(&theFormat) << " added to row index " << j + 1;
            outLaTeX << " Row index " << NumFoundPivots + 1 << " times $"
            << tempElement.toString(&theFormat) << "$ added to row index " << j + 1 << "\\\\";
          }
        }
      }
      if (found) {
        out << ": <br> ";
        outLaTeX << "\\end{tabular}";
        outLaTeX << "\\\\";
        extendedMatrix = theMatrix;
        extendedMatrix.appendMatrixOnTheRight(outputMat);
        out << HtmlRoutines::getMathNoDisplay(extendedMatrix.toString(&theFormat));
        outLaTeX << "$" << extendedMatrix.toString(&theFormat) << "$";
      }
      NumFoundPivots ++;
    }
  }
  outLaTeX << "\\end{tabular}";
  theFormat.matrixColumnVerticalLineIndex = - 1;
  if (NumFoundPivots < theMatrix.numberOfRows) {
    out << "<br>Matrix to the right of the vertical line not "
    << "transformed to the identity matrix => "
    << "starting matrix is not invertible. ";
    outLaTeX << "Matrix to the right of the vertical line "
    << "not transformed to the identity matrix => "
    << "starting matrix is not invertible. ";
  } else {
    out << "<br>The inverse of the starting matrix "
    << "can be read off on the matrix to the left of the id matrix: "
    << HtmlRoutines::getMathNoDisplay(outputMat.toString(&theFormat));
    outLaTeX << " The inverse matrix can now be read off as the matrix "
    << "to the left of the identity matrix: $"
    << outputMat.toString(&theFormat) << "$";
  }
  out << "Output in LaTeX: <br><br>" << outLaTeX.str();
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerInvertMatrixVerbose(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerInvertMatrixVerbose");
  Expression converted;
  if (!CalculatorConversions::innerMakeMatrix(calculator, input, converted)) {
    return calculator << "Failed to get matrix from: " << input.toString();
  }

  Matrix<Rational> mat, outputMat, tempMat;
  if (!calculator.functionGetMatrix(
    converted, mat
  )) {
    return CalculatorFunctions::innerInvertMatrixRFsVerbose(calculator, input, output);
  }
  if (mat.numberOfRows != mat.numberOfColumns || mat.numberOfColumns < 1) {
    return output.makeError("The matrix is not square", calculator);
  }
  outputMat.makeIdentityMatrix(mat.numberOfRows);
  int tempI;
  int NumFoundPivots = 0;
  std::stringstream out;
  Rational tempElement;
  FormatExpressions theFormat;
  theFormat.flagUseLatex = true;
  theFormat.flagUseHTML = false;
  theFormat.matrixColumnVerticalLineIndex = mat.numberOfColumns - 1;
  out << "Computing " << HtmlRoutines::getMathNoDisplay(mat.toString(&theFormat) + "^{- 1}");
  tempMat = mat;
  tempMat.appendMatrixOnTheRight(outputMat);
  out << "<br>" << HtmlRoutines::getMathNoDisplay(tempMat.toString(&theFormat));
  for (int i = 0; i < mat.numberOfColumns; i ++) {
    tempI = mat.findPivot(i, NumFoundPivots);
    if (tempI != - 1) {
      if (tempI != NumFoundPivots) {
        mat.switchRows(NumFoundPivots, tempI);
        outputMat.switchRows (NumFoundPivots, tempI);
        out << "<br>switch row " << NumFoundPivots + 1
        << " and row " << tempI + 1 << ": ";
        tempMat = mat;
        tempMat.appendMatrixOnTheRight(outputMat);
        out << "<br>"
        << HtmlRoutines::getMathNoDisplay(outputMat.toString(&theFormat));
      }
      tempElement = mat.elements[NumFoundPivots][i];
      tempElement.invert();
      if (tempElement != 1) {
        out << "<br> multiply row " << NumFoundPivots + 1
        << " by " << tempElement << ": ";
      }
      mat.rowTimesScalar(NumFoundPivots, tempElement);
      outputMat.rowTimesScalar(NumFoundPivots, tempElement);
      if (tempElement != 1) {
        tempMat = mat;
        tempMat.appendMatrixOnTheRight(outputMat);
        out << HtmlRoutines::getMathNoDisplay(tempMat.toString(&theFormat));
      }
      bool found = false;
      for (int j = 0; j < mat.numberOfRows; j ++) {
        if (j != NumFoundPivots) {
          if (!mat.elements[j][i].isEqualToZero()) {
            tempElement = mat.elements[j][i];
            tempElement.negate();
            mat.addTwoRows(NumFoundPivots, j, i, tempElement);
            outputMat.addTwoRows(NumFoundPivots, j, 0, tempElement);
            if (!found) {
              out << "<br>";
            } else {
              out << ", ";
            }
            found = true;
            out << " Row index " << NumFoundPivots + 1 << " times "
            << tempElement << " added to row index " << j + 1;
          }
        }
      }
      if (found) {
        out << ": <br> ";
        tempMat = mat;
        tempMat.appendMatrixOnTheRight(outputMat);
        out << HtmlRoutines::getMathNoDisplay(tempMat.toString(&theFormat));
      }
      NumFoundPivots ++;
    }
  }
  if (NumFoundPivots < mat.numberOfRows) {
    out << "<br>Matrix to the right of the vertical line not "
    << "transformed to the identity matrix => "
    << "starting matrix is not invertible. ";
  } else {
    out << "<br>The inverse of the starting matrix can "
    << "be read off on the matrix to the left of the id matrix: "
    << HtmlRoutines::getMathNoDisplay(outputMat.toString(&theFormat));
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::functionPolynomialize(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::functionPolynomialize");
  Expression thePolyE;
  if (input.hasBoundVariables()) {
    return false;
  }
  if (!CalculatorConversions::functionPolynomial<Rational>(
    calculator, input, thePolyE
  )) {
    return false;
  }
  if (!CalculatorConversions::functionExpressionFromBuiltInType(
    calculator, thePolyE, output
  )) {
    return false;
  }
  return true;
}

bool CalculatorFunctions::outerPolynomializE(Calculator& calculator, const Expression& input, Expression& output) {
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctions::functionPolynomialize(calculator, input[1], output);
}

bool CalculatorFunctionsIntegration::integrateRationalFunctionSplitToBuidingBlocks(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::functionIntegrateRationalFunctionSplitToBuidingBlocks");
  Expression theFunctionE, theVariableE, integrationSetE;
  if (!input.isIndefiniteIntegralFdx(&theVariableE, &theFunctionE, &integrationSetE)) {
    return false;
  }
  IntegralRationalFunctionComputation theComputation(&calculator);
  if (!CalculatorConversions::functionRationalFunction<Rational>(
    calculator, theFunctionE, theComputation.inpuTE
  )) {
    return calculator
    << "<hr>Call of function CalculatorConversions::innerRationalFunction<Rational> "
    << "failed, input was: "
    << theFunctionE.toString();
  }
  if (!theComputation.inpuTE.isOfType<RationalFunction<Rational> >()) {
    return calculator
    << "<hr>CalculatorFunctions::integrateRationalFunctionSplitToBuidingBlocks: "
    << "failed to convert "
    << theFunctionE.toString() << " to rational function. "
    << "Attempt to converted expression yielded: " << theComputation.inpuTE.toString();
  }
  ExpressionContext context = theComputation.inpuTE.getContext();
  if (context.numberOfVariables() > 1) {
    return calculator << "<hr>I converted " << theFunctionE.toString()
    << " to rational function, but it is of "
    << context.numberOfVariables()
    << " variables. I have been taught to work with 1 variable only. "
    << "<br>The context of the rational function is: "
    << theComputation.inpuTE.getContext().toString();
  }
  if (context.numberOfVariables() == 1) {
    if (context.getVariable(0) != theVariableE) {
      return calculator << "<hr>The univariate rational function was in variable "
      << theComputation.inpuTE.getContext().toString()
      << " but the variable of integration is " << theVariableE.toString();
    }
  }
  theComputation.integrationSetE = integrationSetE;
  theComputation.theRF = theComputation.inpuTE.getValue<RationalFunction<Rational> >();
  theComputation.theRF.getDenominator(theComputation.theDen);
  theComputation.theRF.getNumerator(theComputation.theNum);
  if (theComputation.theDen.totalDegree() < 1) {
    return false;
  }
  if (!theComputation.integrateRationalFunction()) {
    return calculator << theComputation.printoutIntegration.str();
  }
  theComputation.theIntegralSum.checkConsistencyRecursively();
  output = theComputation.theIntegralSum;
  if (output.startsWith(calculator.opIntegral())) {
    if (output[1] == input) {
      return false;
    }
  }
  output.checkConsistencyRecursively();
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctions::innerCoefficientsPowersOf(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCoefficientsPowersOf");
  if (input.size() != 3) {
    return false;
  }
  const Expression& theVarE = input[1];
  const Expression& theExpressionE = input[2];
  VectorSparse<Expression> theCFs;
  if (!calculator.collectCoefficientsPowersVariables(theExpressionE, theVarE, theCFs)) {
    return calculator << "<hr>Failed to evaluate Calculator::collectCoefficientsPowersVariables";
  }
  int highestPowerPlus1 = theCFs.getLargestParticipatingBasisIndex() + 1;
  List<Expression> theCFsIncludingZeros;
  Expression currentCF;
  for (int i = 0; i < highestPowerPlus1; i ++) {
    int theIndex = theCFs.monomials.getIndex(MonomialVector(i));
    if (theIndex == - 1) {
      currentCF.assignValue(0, calculator);
    } else {
      currentCF = theCFs.coefficients[theIndex];
    }
    theCFsIncludingZeros.addOnTop(currentCF);
  }
  return output.makeSequence(calculator, &theCFsIncludingZeros);
}

bool CalculatorFunctions::innerConstTermRelative(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerConstTermRelative");
  if (!input.startsWithGivenOperation("ConstantTerm")) {
    return false;
  }
  Expression coeffExtractor, theCoeffs;
  coeffExtractor = input;
  coeffExtractor.setChildAtomValue(0, "ConstantTerm");
  if (!CalculatorFunctions::innerCoefficientsPowersOf(calculator, coeffExtractor, theCoeffs)) {
    return false;
  }
  if (theCoeffs.isSequenceNElements()) {
    if (theCoeffs.size() > 1) {
      output = theCoeffs[1];
      return true;
    }
  }
  return false;
}

bool CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIa(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIa");
  Expression theFunctionE, theVariableE;
  if (!input.isIndefiniteIntegralFdx(&theVariableE, &theFunctionE)) {
    return false;
  }
  if (!theFunctionE.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  const Expression& A = theFunctionE[1];
  Expression a, b;
  const Expression& axPlusb = theFunctionE[2];
  if (!A.isConstantNumber()) {
    return false;
  }
  if (!CalculatorFunctions::extractLinearCoeffsWRTvariable(axPlusb, theVariableE, a, b)) {
    return false;
  }
  if (!a.isConstantNumber() || !b.isConstantNumber()) {
    return false;
  }
  Expression logaxPlusb;
  logaxPlusb.reset(calculator);
  logaxPlusb.addChildAtomOnTop(calculator.opLog());
  logaxPlusb.addChildOnTop(axPlusb);
  output = A;
  output /= a;
  output *= logaxPlusb;
  output.checkConsistencyRecursively();
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIb(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIa");
  Expression theFunctionE, theVariableE;
  if (!input.isIndefiniteIntegralFdx(&theVariableE, &theFunctionE)) {
    return false;
  }
  if (!theFunctionE.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  const Expression& A = theFunctionE[1];
  const Expression& axPlusbPowerN = theFunctionE[2];
  if (!A.isConstantNumber()) {
    return false;
  }
  if (!axPlusbPowerN.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  Expression N = axPlusbPowerN[2];
  if (!N.isConstantNumber()) {
    return false;
  }
  Expression a, b;
  const Expression& axPlusb = axPlusbPowerN[1];
  if (!CalculatorFunctions::extractLinearCoeffsWRTvariable(axPlusb, theVariableE, a, b)) {
    return false;
  }
  if (!a.isConstantNumber() || !b.isConstantNumber()) {
    return false;
  }
  Expression base, OneMinusN;
  OneMinusN = N;
  OneMinusN += - 1;
  OneMinusN *= - 1;
  base.makeXOX(calculator, calculator.opThePower(), axPlusb, OneMinusN);
  output = A;
  output /= a;
  output /= OneMinusN;
  output *= base;
  output.checkConsistencyRecursively();
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctions::extractQuadraticCoeffsWRTvariable(
  const Expression& theQuadratic,
  const Expression& theVariable,
  Expression& outputCoeffVarSquared,
  Expression& outputCoeffLinTerm,
  Expression& outputConstTerm
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::extractQuadraticCoeffsWRTvariable");
  VectorSparse<Expression> theCoeffs;
  if (theQuadratic.owner == nullptr) {
    return false;
  }
  Calculator& calculator = *theQuadratic.owner;
  if (!calculator.collectCoefficientsPowersVariables(theQuadratic, theVariable, theCoeffs)) {
    return false;
  }
  if (theCoeffs.getLargestParticipatingBasisIndex() != 2) {
    return false;
  }
  outputCoeffVarSquared.assignValue(0, calculator);
  outputCoeffLinTerm.assignValue(0, calculator);
  outputConstTerm.assignValue(0, calculator);
  if (theCoeffs.monomials.contains(MonomialVector(0))) {
    outputConstTerm = theCoeffs.getCoefficientOf(MonomialVector(0));
  }
  if (theCoeffs.monomials.contains(MonomialVector(1))) {
    outputCoeffLinTerm = theCoeffs.getCoefficientOf(MonomialVector(1));
  }
  if (theCoeffs.monomials.contains(MonomialVector(2))) {
    outputCoeffVarSquared = theCoeffs.getCoefficientOf(MonomialVector(2));
  }
  return true;
}

bool CalculatorFunctions::extractLinearCoeffsWRTvariable(
  const Expression& theLinearExpression,
  const Expression& theVariable,
  Expression& outputCoeffLinTerm,
  Expression& outputConstTerm
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::extractLinearCoeffsWRTvariable");
  VectorSparse<Expression> theCoeffs;
  if (theLinearExpression.owner == nullptr) {
    return false;
  }
  Calculator& calculator = *theLinearExpression.owner;
  calculator.collectCoefficientsPowersVariables(theLinearExpression, theVariable, theCoeffs);
  if (theCoeffs.getLargestParticipatingBasisIndex() > 1) {
    return false;
  }
  outputCoeffLinTerm.assignValue(0, calculator);
  outputConstTerm.assignValue(0, calculator);
  if (theCoeffs.monomials.contains(MonomialVector(1))) {
    outputCoeffLinTerm = theCoeffs.getCoefficientOf(MonomialVector(1));
  }
  if (theCoeffs.monomials.contains(MonomialVector(0))) {
    outputConstTerm = theCoeffs.getCoefficientOf(MonomialVector(0));
  }
  return true;
}

bool CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIaandIIIa(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIaandIIIa");
  Expression theFunctionE, x;
  if (!input.isIndefiniteIntegralFdx(&x, &theFunctionE)) {
    return false;
  }
  if (!theFunctionE.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  Expression denNoPower = theFunctionE[2];
  Expression a, b, c, A, B;
  if (!CalculatorFunctions::extractQuadraticCoeffsWRTvariable(denNoPower, x, a, b, c)) {
    return false;
  }
  if (!CalculatorFunctions::extractLinearCoeffsWRTvariable(theFunctionE[1], x, A, B)) {
    return false;
  }
  if (
    !a.isConstantNumber() || !b.isConstantNumber() || !c.isConstantNumber()||
    !A.isConstantNumber() || !B.isConstantNumber()
  ) {
    return calculator << "<hr>Failed to evaluate to constant "
    << "the coefficients of the block IIa and IIIa integral. "
    << "The coefficients are: " << a.toString()
    << ", " << b.toString() << ", " << c.toString() << ", "
    << A.toString() << ", " << B.toString() << ". ";
  }
  double approxa, approxb, approxc;
  if (!a.evaluatesToDouble(&approxa) || !b.evaluatesToDouble(&approxb) || !c.evaluatesToDouble(&approxc)) {
    return calculator
    << "<hr>Failed to evaluate "
    << "variable coefficients in denominator " << denNoPower.toString()
    << " to double. Possible user typo. ";
  }
  if (approxb*approxb >= approxa * approxc * 4) {
    return false;
  }
  Expression xSquared, bSquared, aSquared;
  Expression twoE, oneE, fourE;
  oneE.assignValue(1, calculator);
  twoE.assignValue(2, calculator);
  fourE.assignValue(4, calculator);
  xSquared.makeXOX(calculator, calculator.opThePower(), x, twoE);
  bSquared.makeXOX(calculator, calculator.opThePower(), b, twoE);
  aSquared.makeXOX(calculator, calculator.opThePower(), a, twoE);

  Expression theQuadraticDiva = xSquared + (b / a) * x + c / a;
  Expression xplusbdiv2a = x + b / (twoE * a);
  Expression F = (fourE * a * c - bSquared) / (fourE * aSquared);
  Expression sqrtF;
  sqrtF.makeSqrt(calculator, F, 2);
  Expression arcTanArgument = xplusbdiv2a / sqrtF;
  Expression theArcTan(calculator);
  theArcTan.addChildAtomOnTop(calculator.opArcTan());
  theArcTan.addChildOnTop(arcTanArgument);
  Expression theLog(calculator);
  theLog.addChildAtomOnTop(calculator.opLog());
  theLog.addChildOnTop(theQuadraticDiva);
  Expression C = B - (A * b) / (twoE * a);
  output = (oneE / a) * ((A / twoE) * theLog + (C / sqrtF) * theArcTan);
  output.checkConsistencyRecursively();
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIIb(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIIb");
  Expression theFunctionE, x, integrationSetE;
  if (!input.isIndefiniteIntegralFdx(&x, &theFunctionE, &integrationSetE)) {
    return false;
  }
  if (!theFunctionE.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (!theFunctionE[2].startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  Expression numPowerE = theFunctionE[2][2];
  int numPower = 0;
  if (!numPowerE.isSmallInteger(&numPower)) {
    return false;
  }
  if (numPower <= 1) {
    return false;
  }
  Expression denNoPower = theFunctionE[2][1];
  Expression a, b, c;
  if (!theFunctionE[1].isEqualToOne()) {
    return false;
  }
  if (!CalculatorFunctions::extractQuadraticCoeffsWRTvariable(denNoPower, x, a, b, c)) {
    return false;
  }
  if (!a.isEqualToOne() || !b.isConstantNumber() || !c.isConstantNumber()) {
    calculator << "<hr>Failed to evaluate to constant the coefficients of the block IIIb integral."
    << "The coefficients are: " << a.toString() << ", " << b.toString() << ", " << c.toString() << ". ";
    return false;
  }
  double approxa, approxb, approxc;
  if (!a.evaluatesToDouble(&approxa) || !b.evaluatesToDouble(&approxb) || !c.evaluatesToDouble(&approxc)) {
    calculator << "Failed to evaluate variable coefficients in denominator " << denNoPower.toString()
    << " to double. Possible user typo. ";
    return false;
  }
  if (approxb * approxb >= approxa * approxc * 4) {
    return false;
  }
  Expression xSquared, bSquared;
  Expression twoE, oneE, threeE, fourE;
  oneE.assignValue(1, calculator);
  twoE.assignValue(2, calculator);
  threeE.assignValue(3, calculator);
  fourE.assignValue(4, calculator);

  xSquared.makeXOX(calculator, calculator.opThePower(), x, twoE);
  bSquared.makeXOX(calculator, calculator.opThePower(), b, twoE);

  Expression theMonicQuadratic = xSquared + b * x + c;
  Expression D = c - bSquared / fourE;
  Expression remainingIntegral, functionRemainingToIntegrate, theQuadraticPowerOneMinusN, theQuadraticPowerNMinusOne;
  theQuadraticPowerOneMinusN.makeXOX(calculator, calculator.opThePower(), theMonicQuadratic, oneE - numPowerE);
  theQuadraticPowerNMinusOne.makeXOX(calculator, calculator.opThePower(), theMonicQuadratic, numPowerE - oneE);
  functionRemainingToIntegrate = oneE / theQuadraticPowerNMinusOne;
  remainingIntegral.makeIntegral(calculator, integrationSetE, functionRemainingToIntegrate, x);
  output = oneE / D *
  ((x + b / twoE) / (twoE * numPowerE - twoE) * theQuadraticPowerOneMinusN +
  (twoE * numPowerE - threeE) / (twoE * numPowerE - twoE) * remainingIntegral);
  output.checkConsistencyRecursively();
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIb(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIb");
  Expression theFunctionE, x, integrationSetE;
  if (!input.isIndefiniteIntegralFdx(&x, &theFunctionE, &integrationSetE)) {
    return false;
  }
  if (!theFunctionE.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (!theFunctionE[2].startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  Expression nE = theFunctionE[2][2];
  int numPower = 0;
  if (!nE.isSmallInteger(&numPower)) {
    return false;
  }
  if (numPower <= 1) {
    return false;
  }
  Expression denNoPower = theFunctionE[2][1];
  Expression a, b, c, A, B;
  if (!CalculatorFunctions::extractQuadraticCoeffsWRTvariable(denNoPower, x, a, b, c)) {
    return false;
  }
  if (!CalculatorFunctions::extractLinearCoeffsWRTvariable(theFunctionE[1], x, A, B)) {
    return false;
  }
  if (A.isEqualToZero()) { //building block is of type IIIb
    return false;
  }
  if (
    !a.isConstantNumber() || !b.isConstantNumber() || !c.isConstantNumber()||
    !A.isConstantNumber() || !B.isConstantNumber()
  ) {
    calculator << "<hr>Failed to evaluate to constant the coefficients of the block IIb integral."
    << "The coefficients are: " << a.toString() << ", " << b.toString() << ", " << c.toString() << ", "
    << A.toString() << ", " << B.toString() << ". ";
    return false;
  }
  double approxa, approxb, approxc;
  if (!a.evaluatesToDouble(&approxa) || !b.evaluatesToDouble(&approxb) || !c.evaluatesToDouble(&approxc)) {
    calculator << "Failed to evaluate variable coefficients in denominator " << denNoPower.toString()
    << " to double. Possible user typo. ";
    return false;
  }
  if (approxb * approxb >= approxa * approxc * 4) {
    return false;
  }
  Expression xSquared, bSquared, aSquared, apowerN;
  Expression twoE, oneE, fourE;
  Expression remainingIntegral, remainingFunctionToIntegrate, quadraticPowerN, quadraticPowerOneMinusN;
  oneE.assignValue(1, calculator);
  twoE.assignValue(2, calculator);
  fourE.assignValue(4, calculator);
  apowerN.makeXOX(calculator, calculator.opThePower(), a, nE);
  xSquared.makeXOX(calculator, calculator.opThePower(), x, twoE);
  bSquared.makeXOX(calculator, calculator.opThePower(), b, twoE);
  aSquared.makeXOX(calculator, calculator.opThePower(), a, twoE);
  Expression theQuadraticDiva = xSquared + (b / a) * x + c / a;
  quadraticPowerN.makeXOX(calculator, calculator.opThePower(), theQuadraticDiva, nE);
  quadraticPowerOneMinusN.makeXOX(calculator, calculator.opThePower(), theQuadraticDiva, oneE - nE);
  remainingFunctionToIntegrate = oneE / quadraticPowerN;
  remainingIntegral.makeIntegral(calculator, integrationSetE, remainingFunctionToIntegrate, x);
  Expression C = B - (A * b) / (twoE * a);
  oneE.checkInitializationRecursively();
  apowerN.checkInitializationRecursively();
  A.checkInitializationRecursively();
  twoE.checkInitializationRecursively();
  nE.checkInitializationRecursively();
  quadraticPowerOneMinusN.checkInitializationRecursively();
  C.checkInitializationRecursively();
  remainingIntegral.checkInitializationRecursively();
  output = (oneE / apowerN) * (A / (twoE * (oneE - nE)) * quadraticPowerOneMinusN + C * remainingIntegral);
  output.checkConsistencyRecursively();
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctionsIntegration::integratePowerByUncoveringParenthesisFirst(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::integratePowerByUncoveringParenthesisFirst");
  Expression theFunctionE, integrandE, newIntegralE, theVariableE, integrationSetE;
  if (!input.isIndefiniteIntegralFdx(&theVariableE, &theFunctionE, &integrationSetE)) {
    return false;
  }
  if (!theFunctionE.startsWith(calculator.opThePower())) {
    return false;
  }
  if (!CalculatorFunctions::functionPolynomialize(calculator, theFunctionE, integrandE)) {
    return false;
  }
  if (integrandE == theFunctionE) {
    return false;
  }
  newIntegralE.makeIntegral(calculator, integrationSetE, integrandE, theVariableE);
  if (!calculator.evaluateExpression(calculator, newIntegralE, output)) {
    return false;
  }
  if (output.containsAsSubExpressionNoBuiltInTypes(calculator.opIntegral())) {
    return false;
  }
  output.checkConsistencyRecursively();
  output.checkInitializationRecursively();
  return true;
}

bool Expression::makeIntegral(
  Calculator& calculator,
  const Expression& integrationSet,
  const Expression& theFunction,
  const Expression& theVariable
) {
  MacroRegisterFunctionWithName("Expression::makeIntegral");
  if (this == &theFunction || this == &theVariable || this == &integrationSet) {
    Expression theFunCopy = theFunction;
    Expression theVarCopy = theVariable;
    Expression integrationSetCopy = integrationSet;
    return this->makeIntegral(calculator, integrationSetCopy, theFunCopy, theVarCopy);
  }
  Expression theDiffForm;
  theDiffForm.makeXOX(calculator, calculator.opDifferential(), theVariable, theFunction);
  this->reset(calculator);
  this->addChildAtomOnTop(calculator.opIntegral());
  this->addChildOnTop(integrationSet);
  return this->addChildOnTop(theDiffForm);
}

bool CalculatorFunctionsIntegration::integratePullImaginaryUnit(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::integratePullImaginaryUnit");
  Expression theFunctionE, theVariableE, integrationSetE;
  if (!input.isIndefiniteIntegralFdx(&theVariableE, &theFunctionE, &integrationSetE)) {
    return false;
  }
  Expression iE;
  iE.makeAtom(calculator.opImaginaryUnit(), calculator);
  if (theVariableE == iE) {
    return false;
  }
  Expression theCF, theNoCFintegrand, theNoImIntegrand, outputIntegralNoCF;
  theFunctionE.getCoefficientMultiplicandForm(theCF, theNoCFintegrand);

  if (theNoCFintegrand == iE) {
    theNoImIntegrand.assignValue(1, calculator);
  } else if (theNoCFintegrand.startsWith(calculator.opTimes(), 3)) {
    if (theNoCFintegrand[1] != iE) {
      return false;
    }
    theNoImIntegrand = theNoCFintegrand[2];
  } else {
    return false;
  }
  theCF *= iE;
  outputIntegralNoCF.makeIntegral(calculator, integrationSetE,theNoImIntegrand,theVariableE);
  output = theCF * outputIntegralNoCF;
  return true;
}

bool CalculatorFunctionsIntegration::integrateSum(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::integrateSum");
  Expression theFunctionE, theVariableE, integrationSetE;
  if (!input.isIndefiniteIntegralFdx(&theVariableE, &theFunctionE, &integrationSetE)) {
    return false;
  }
  if (!theFunctionE.startsWith(calculator.opPlus())) {
    return false;
  }
  List<Expression> integralsOfSummands;
  integralsOfSummands.setSize(theFunctionE.size() - 1);
  Expression newIntegralE, result, newSummand;
  for (int i = 1; i < theFunctionE.size(); i ++) {
    newIntegralE.makeIntegral(calculator, integrationSetE, theFunctionE[i], theVariableE);
    if (!calculator.evaluateExpression(calculator, newIntegralE, newSummand)) {
      return false;
    }
    if (newSummand.containsAsSubExpressionNoBuiltInTypes(calculator.opIntegral())) {
      return false;
    }
    if (i == 1) {
      result = newSummand;
    } else {
      result += newSummand;
    }
  }
  output = result;
  output.checkConsistencyRecursively();
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctionsIntegration::integrateXnDiffX(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::integrateXnDiffX");
  Expression theFunctionE, theVariableE;
  if (!input.isIndefiniteIntegralFdx(&theVariableE, &theFunctionE)) {
    return false;
  }
  Expression theFunCoeff, theFunNoCoeff, outputNoCoeff;
  if (theFunctionE.isConstantNumber()) {
    output = theFunctionE * theVariableE;
    return true;
  }
  theFunctionE.getCoefficientMultiplicandForm(theFunCoeff, theFunNoCoeff);
  if (theFunNoCoeff == theVariableE) {
    output = theFunCoeff * theVariableE * theVariableE;
    output /= 2;
    return true;
  }
  if (!theFunNoCoeff.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  if (theFunNoCoeff[1] != theVariableE) {
    return false;
  }
  if (!theFunNoCoeff[2].isConstantNumber()) {
    return false;
  }
  if (theFunNoCoeff[2].isEqualToMOne()) {
    outputNoCoeff.reset(calculator, 2);
    outputNoCoeff.addChildAtomOnTop(calculator.opLog());
    outputNoCoeff.addChildOnTop(theVariableE);
    output = theFunCoeff * outputNoCoeff;
    return true;
  }
  Expression outputPower = theFunNoCoeff[2];
  outputPower += 1;
  outputNoCoeff.makeXOX(calculator, calculator.opThePower(), theVariableE, outputPower);
  outputNoCoeff /= outputPower;
  output = theFunCoeff * outputNoCoeff;
  output.checkConsistency();
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctionsIntegration::integrateSinPowerNCosPowerM(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::integrateSinPowerNCosPowerM");
  Expression theFunctionE, theVariableE, integrationSet;
  if (!input.isIndefiniteIntegralFdx(&theVariableE, &theFunctionE, &integrationSet)) {
    return false;
  }
  Expression polynomializedFunctionE;
  if (!CalculatorConversions::functionPolynomial<Rational>(
    calculator, theFunctionE, polynomializedFunctionE
  )) {
    return false;
  }
  if (!polynomializedFunctionE.isOfType<Polynomial<Rational> >()) {
    return false;
  }
  ExpressionContext contextE = polynomializedFunctionE.getContext();
  int numVars = contextE.numberOfVariables();
  if (numVars > 2) {
    return false;
  }
  if (numVars == 0) {
    return false;
  }
  Expression sinPowerE, theTrigArgument, cosPowerE;
  sinPowerE.assignValue(1, calculator);
  cosPowerE.assignValue(1, calculator);
  bool firstIsSine = false;
  for (int i = 0; i < numVars; i ++) {
    Expression currentE = contextE.getVariable(i);
    if (
      !currentE.startsWith(calculator.opSin(), 2) &&
      !currentE.startsWith(calculator.opCos(), 2)
    ) {
      return false;
    }
    if (i == 0 && currentE.startsWith(calculator.opSin())) {
      firstIsSine = true;
    }
    if (i == 0) {
      theTrigArgument = currentE[1];
    } else {
      if (theTrigArgument != currentE[1]) {
        return false;
      }
    }
  }
  Expression theTrigArgumentNoCoeff, theTrigArgCoeff;
  theTrigArgument.getCoefficientMultiplicandForm(theTrigArgCoeff, theTrigArgumentNoCoeff);
  if (theTrigArgumentNoCoeff != theVariableE) {
    return false;
  }
  const Polynomial<Rational>& theTrigPoly = polynomializedFunctionE.getValue<Polynomial<Rational> >();
  Expression theCosE, theSinE, theCosDoubleE, theTrigArgumentDouble;
  theTrigArgumentDouble = theTrigArgument;
  theTrigArgumentDouble *= 2;
  theCosE.makeOX(calculator, calculator.opCos(), theTrigArgument);
  theSinE.makeOX(calculator, calculator.opSin(), theTrigArgument);
  theCosDoubleE.makeOX(calculator, calculator.opCos(), theTrigArgumentDouble);
  Expression outputCandidate, currentSummandE, currentCommandListE,
  currentSubE, currentIntegrandE, currentIntegrandNonPolynomializedE,
  currentIntegral, currentIntegralComputation,
  currentIntegrandSinePart, currentIntegrandCosinePart, newVarE, newResultE, currentE, currentCF;
  Expression oneE, twoE, threeE, powerE;
  oneE.assignValue(1, calculator);
  twoE.assignValue(2, calculator);
  threeE.assignValue(3, calculator);
  outputCandidate.assignValue(0,calculator);
  newVarE = calculator.getNewAtom();
  newResultE = calculator.getNewAtom();
  for (int i = 0; i < theTrigPoly.size(); i ++) {
    const MonomialP& currentMon = theTrigPoly[i];
    int powerSine = - 1, powerCosine = - 1;
    if (!currentMon(0).isSmallInteger(&powerSine) || !currentMon(1).isSmallInteger(&powerCosine)) {
      return false;
    }
    if (!firstIsSine) {
      MathRoutines::swap(powerSine, powerCosine);
    }
    if (powerSine < 0 || powerCosine < 0) {
      return false;
    }
    if (powerSine % 2 == 1) {
      currentE = oneE - newVarE * newVarE;
      powerE.assignValue((powerSine - 1) / 2, calculator);
      currentIntegrandSinePart.makeXOX(calculator, calculator.opThePower(), currentE, powerE);
      powerE.assignValue(powerCosine, calculator);
      currentIntegrandCosinePart.makeXOX(calculator, calculator.opThePower(), newVarE, powerE);
      currentCF.assignValue(- theTrigPoly.coefficients[i], calculator);
      currentCF /= theTrigArgCoeff;
      currentSubE.makeXOX(calculator, calculator.opDefine(), newVarE, theCosE);
    } else if (powerCosine % 2 == 1) {
      currentE = oneE - newVarE * newVarE;
      powerE.assignValue((powerCosine - 1) / 2, calculator);
      currentIntegrandCosinePart.makeXOX(calculator, calculator.opThePower(), currentE, powerE);
      powerE.assignValue(powerSine, calculator);
      currentIntegrandSinePart.makeXOX(calculator, calculator.opThePower(), newVarE, powerE);
      currentCF.assignValue(theTrigPoly.coefficients[i], calculator);
      currentCF /= theTrigArgCoeff;
      currentSubE.makeXOX(calculator, calculator.opDefine(), newVarE, theSinE);
    } else {
      currentE = (oneE - theCosDoubleE) / twoE;
      powerE.assignValue(powerSine / 2, calculator);
      currentIntegrandSinePart.makeXOX(calculator, calculator.opThePower(), currentE, powerE);
      currentE = (oneE + theCosDoubleE) / twoE;
      powerE.assignValue(powerCosine / 2, calculator);
      currentIntegrandCosinePart.makeXOX(calculator, calculator.opThePower(), currentE, powerE);
      currentCF.assignValue(theTrigPoly.coefficients[i], calculator);
      currentIntegrandNonPolynomializedE = currentCF * currentIntegrandSinePart * currentIntegrandCosinePart;
      currentIntegrandE.reset(calculator);
      currentIntegrandE.addChildAtomOnTop("Polynomialize");
      currentIntegrandE.addChildOnTop(currentIntegrandNonPolynomializedE);
      currentIntegral.makeIntegral(calculator, integrationSet, currentIntegrandE, theVariableE);
      outputCandidate += currentIntegral;
      continue;
    }
    currentIntegrandNonPolynomializedE = currentCF * currentIntegrandSinePart * currentIntegrandCosinePart;
    currentIntegrandE.reset(calculator);
    currentIntegrandE.addChildAtomOnTop("Polynomialize");
    currentIntegrandE.addChildOnTop(currentIntegrandNonPolynomializedE);
    currentIntegral.makeIntegral(calculator, integrationSet, currentIntegrandE, newVarE);

    currentIntegralComputation.makeXOX(calculator, calculator.opDefine(), newResultE, currentIntegral);
    currentCommandListE.reset(calculator);
    currentCommandListE.addChildAtomOnTop(calculator.opCommandSequence());
    currentCommandListE.addChildOnTop(currentIntegralComputation);
    currentCommandListE.addChildOnTop(currentSubE);
    currentCommandListE.addChildOnTop(newResultE);
    currentSummandE.reset(calculator);
    currentSummandE.addChildAtomOnTop(calculator.opUnderscore());
    currentSummandE.addChildOnTop(currentCommandListE);
    currentSummandE.addChildOnTop(threeE);

    outputCandidate += currentSummandE;
  }
  output = outputCandidate;
  return true;
}

bool CalculatorFunctionsIntegration::integrateTanPowerNSecPowerM(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::integrateTanPowerNSecPowerM");
  Expression theFunctionE, theVariableE, integrationSet;
  if (!input.isIndefiniteIntegralFdx(&theVariableE, &theFunctionE, &integrationSet)) {
    return false;
  }
  Expression polynomializedFunctionE;
  if (!CalculatorConversions::functionPolynomial<Rational>(
    calculator, theFunctionE, polynomializedFunctionE
  )) {
    return false;
  }
  if (!polynomializedFunctionE.isOfType<Polynomial<Rational> >()) {
    return false;
  }
  ExpressionContext context = polynomializedFunctionE.getContext();
  int numVars = context.numberOfVariables();
  if (numVars > 2) {
    return false;
  }
  if (numVars == 0) {
    return false;
  }
  Expression sinPowerE, theTrigArgument, cosPowerE;
  sinPowerE.assignValue(1, calculator);
  cosPowerE.assignValue(1, calculator);
  bool firstIsTan = false;
  for (int i = 0; i < numVars; i ++) {
    Expression currentE = context.getVariable(i);
    if (
      !currentE.startsWith(calculator.opTan(), 2) &&
      !currentE.startsWith(calculator.opSec(), 2)
    ) {
      return false;
    }
    if (i == 0 && currentE.startsWith(calculator.opTan())) {
      firstIsTan = true;
    }
    if (i == 0) {
      theTrigArgument = currentE[1];
    } else {
      if (theTrigArgument != currentE[1]) {
        return false;
      }
    }
  }
  Expression theTrigArgumentNoCoeff, theTrigArgCoeff;
  theTrigArgument.getCoefficientMultiplicandForm(theTrigArgCoeff, theTrigArgumentNoCoeff);
  if (theTrigArgumentNoCoeff != theVariableE) {
    return false;
  }
  const Polynomial<Rational>& theTrigPoly = polynomializedFunctionE.getValue<Polynomial<Rational> >();
  Expression theTanE, theSecE;
  theTanE.makeOX(calculator, calculator.opTan(), theTrigArgument);
  theSecE.makeOX(calculator, calculator.opSec(), theTrigArgument);
  Expression outputCandidate, currentSummandE, currentCommandListE,
  currentSubE, currentIntegrandE, currentIntegrandNonPolynomializedE,
  currentIntegral, currentIntegralComputation,
  currentIntegrandTanPart, currentIntegrandSecPart, newVarE, newResultE, currentE, currentCF;
  Expression oneE, twoE, threeE, powerE;
  oneE.assignValue(1, calculator);
  twoE.assignValue(2, calculator);
  threeE.assignValue(3, calculator);
  outputCandidate.assignValue(0,calculator);
  newVarE = calculator.getNewAtom();
  newResultE = calculator.getNewAtom();
  for (int i = 0; i < theTrigPoly.size(); i ++) {
    const MonomialP& currentMon = theTrigPoly[i];
    int powerTan = - 1, powerSec = - 1;
    if (!currentMon(0).isSmallInteger(&powerTan) || !currentMon(1).isSmallInteger(&powerSec)) {
      return false;
    }
    if (!firstIsTan) {
      MathRoutines::swap(powerTan, powerSec);
    }
    if (powerTan < 0 || powerSec < 0) {
      return false;
    }
    if (powerTan % 2 == 1 && powerSec > 0) {
      currentE = newVarE * newVarE - oneE;
      powerE.assignValue((powerTan - 1) / 2, calculator);
      currentIntegrandTanPart.makeXOX(calculator, calculator.opThePower(), currentE, powerE);
      powerE.assignValue(powerSec - 1, calculator);
      currentIntegrandSecPart.makeXOX(calculator, calculator.opThePower(), newVarE, powerE);
      currentCF.assignValue(theTrigPoly.coefficients[i], calculator);
      currentCF /= theTrigArgCoeff;
      currentSubE.makeXOX(calculator, calculator.opDefine(), newVarE, theSecE);
    } else if (powerSec % 2 == 0) {
      currentE = oneE + newVarE * newVarE;
      powerE.assignValue((powerSec - 2) / 2, calculator);
      currentIntegrandSecPart.makeXOX(calculator, calculator.opThePower(), currentE, powerE);
      powerE.assignValue(powerTan, calculator);
      currentIntegrandTanPart.makeXOX(calculator, calculator.opThePower(), newVarE, powerE);
      currentCF.assignValue(theTrigPoly.coefficients[i], calculator);
      currentCF /= theTrigArgCoeff;
      currentSubE.makeXOX(calculator, calculator.opDefine(), newVarE, theTanE);
    } else {
      return false;
      /*currentE= (oneE-theCosDoubleE)/twoE;
      powerE.assignValue(powerSine/2, calculator);
      currentIntegrandSinePart.makeXOX
      (calculator, calculator.opThePower(),currentE, powerE);
      currentE= (oneE+theCosDoubleE)/twoE;
      powerE.assignValue(powerCosine/2, calculator);
      currentIntegrandCosinePart.makeXOX
      (calculator, calculator.opThePower(),currentE, powerE);
      currentCF.assignValue(theTrigPoly.theCoeffs[i], calculator);
      currentIntegrandNonPolynomializedE=
      currentCF*currentIntegrandSinePart*currentIntegrandCosinePart;
      currentIntegrandE.reset(calculator);
      currentIntegrandE.addChildAtomOnTop("Polynomialize");
      currentIntegrandE.addChildOnTop(currentIntegrandNonPolynomializedE);
      currentIntegral.makeIntegral(calculator, integrationSet, currentIntegrandE, theVariableE);
      outputCandidate += currentIntegral;
      continue;*/
    }
    currentIntegrandNonPolynomializedE = currentCF * currentIntegrandTanPart*currentIntegrandSecPart;
    currentIntegrandE.reset(calculator);
    currentIntegrandE.addChildAtomOnTop("Polynomialize");
    currentIntegrandE.addChildOnTop(currentIntegrandNonPolynomializedE);
    currentIntegral.makeIntegral(calculator, integrationSet, currentIntegrandE, newVarE);

    currentIntegralComputation.makeXOX(calculator, calculator.opDefine(), newResultE, currentIntegral);
    currentCommandListE.reset(calculator);
    currentCommandListE.addChildAtomOnTop(calculator.opCommandSequence());
    currentCommandListE.addChildOnTop(currentIntegralComputation);
    currentCommandListE.addChildOnTop(currentSubE);
    currentCommandListE.addChildOnTop(newResultE);
    currentSummandE.reset(calculator);
    currentSummandE.addChildAtomOnTop(calculator.opUnderscore());
    currentSummandE.addChildOnTop(currentCommandListE);
    currentSummandE.addChildOnTop(threeE);

    outputCandidate += currentSummandE;
  }
  output = outputCandidate;
  return true;
}

bool CalculatorFunctionsTrigonometry::exploitCosineEvenness(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsTrigonometry::exploitCosineEvenness");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression cfE, nonCFpart;
  argument.getCoefficientMultiplicandForm(cfE, nonCFpart);
  Rational theRat;
  if (!cfE.isRational(&theRat)) {
    return false;
  }
  if (theRat >= 0) {
    return false;
  }
  Expression moneE;
  moneE.assignValue(- 1, calculator);
  return output.makeOX(calculator, calculator.opCos(), moneE * cfE * nonCFpart);
}

bool CalculatorFunctionsTrigonometry::exploitSineOddness(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsTrigonometry::exploitSineOddness");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression cfE, nonCFpart;
  argument.getCoefficientMultiplicandForm(cfE, nonCFpart);
  Rational theRat;
  if (!cfE.isRational(&theRat)) {
    return false;
  }
  if (theRat >= 0) {
    return false;
  }
  Expression moneE, sinE;
  moneE.assignValue(- 1, calculator);
  sinE.makeOX(calculator, calculator.opSin(), moneE * cfE * nonCFpart);
  output = moneE * sinE;
  return true;
}

bool CalculatorFunctionsTrigonometry::convertSineToExponent(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsTrigonometry::convertSineToExponent");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression eE, iE, exponentArgument, minusExponentArgument, leftE, rightE;
  eE.makeAtom(calculator.opE(), calculator);
  iE.makeAtom(calculator.opImaginaryUnit(), calculator);
  exponentArgument = iE * argument;
  minusExponentArgument = exponentArgument * (- 1);
  leftE.makeXOX(calculator, calculator.opThePower(), eE, exponentArgument);
  rightE.makeXOX(calculator, calculator.opThePower(), eE, minusExponentArgument);
  output = (iE * (- 1)) * (leftE - rightE) / 2;
  return true;
}

bool CalculatorFunctionsTrigonometry::convertCosineToExponent(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsTrigonometry::convertCosineToExponent");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression eE, iE, exponentArgument, minusExponentArgument, leftE, rightE;
  eE.makeAtom(calculator.opE(), calculator);
  iE.makeAtom(calculator.opImaginaryUnit(), calculator);
  exponentArgument = iE * argument;
  minusExponentArgument = exponentArgument * (- 1);
  leftE.makeXOX(calculator, calculator.opThePower(), eE, exponentArgument);
  rightE.makeXOX(calculator, calculator.opThePower(), eE, minusExponentArgument);
  output = (leftE + rightE) / 2;
  return true;
}

bool CalculatorFunctions::innerPowerImaginaryUnit(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPowerImaginaryUnit");
  if (!input.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  if (!input[1].isOperationGiven(calculator.opImaginaryUnit())) {
    return false;
  }
  LargeInteger thePower;
  if (!input[2].isInteger(&thePower)) {
    return false;
  }
  Expression iE;
  iE.makeAtom(calculator.opImaginaryUnit(), calculator);
  if (thePower % 4 == 0) {
    return output.assignValue(1, calculator);
  }
  if (thePower % 4 == 1) {
    output = iE;
    return true;
  }
  if (thePower % 4 == 2) {
    return output.assignValue(- 1, calculator);
  }
  if (thePower % 4 == 3) {
    output = iE * (- 1);
    return true;
  }
  return false; //<-this shouldn't happen
}

bool CalculatorFunctionsTrigonometry::eulerFormulaAsLaw(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsTrigonometry::eulerFormulaAsLaw");
  if (!input.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  if (!input[1].isOperationGiven(calculator.opE())) {
    return false;
  }
  Expression coefficientOfI, currentE;
  Expression iE;
  iE.makeAtom(calculator.opImaginaryUnit(), calculator);
  currentE.reset(calculator, 3);
  currentE.addChildAtomOnTop(calculator.opCoefficientOf());
  currentE.addChildOnTop(iE);
  currentE.addChildOnTop(input[2]);
  if (!CalculatorFunctions::innerCoefficientOf(calculator, currentE, coefficientOfI)) {
    return false;
  }
  if (coefficientOfI.isEqualToZero()) {
    return false;
  }
  Expression cosE, sinE;
  cosE.makeOX(calculator, calculator.opCos(), coefficientOfI);
  sinE.makeOX(calculator, calculator.opSin(), coefficientOfI);
  output = cosE + iE * sinE;
  return true;
}

bool CalculatorFunctionsIntegration::integrateEpowerAxDiffX(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::integrateEpowerAxDiffX");
  Expression theFunctionE, theVariableE;
  if (!input.isIndefiniteIntegralFdx(&theVariableE, &theFunctionE)) {
    return false;
  }
  Expression theFunCoeff, theFunNoCoeff;
  theFunctionE.getCoefficientMultiplicandForm(theFunCoeff, theFunNoCoeff);
  if (!theFunNoCoeff.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  if (!theFunNoCoeff[1].isOperationGiven(calculator.opE())) {
    return false;
  }
  Expression thePowerCoeff, thePowerNoCoeff;
  theFunNoCoeff[2].getCoefficientMultiplicandForm(thePowerCoeff, thePowerNoCoeff);
  if (thePowerNoCoeff != theVariableE) {
    if (thePowerNoCoeff.startsWith(calculator.opTimes(), 3)) {
      if (
        thePowerNoCoeff[1].isOperationGiven(calculator.opImaginaryUnit()) &&
        thePowerNoCoeff[2] == theVariableE
      ) {
        output = thePowerNoCoeff[1] * (- 1) * theFunctionE / thePowerCoeff;
        output.checkConsistency();
        output.checkInitializationRecursively();
        return true;
      }
    }
    return false;
  }
  output = theFunctionE;
  output /= thePowerCoeff;
  output.checkConsistency();
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctionsDifferentiation::differentiateSqrt(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsDifferentiation::differentiateSqrt");
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator << "<hr>Warning: differentiating with respect to the non-atomic expression "
    << input[1].toString() << " - possible user typo?";
  }
  const Expression& theArgument = input[2];
  if (!theArgument.isOperationGiven(calculator.opSqrt())) {
    return false;
  }
  Expression twoE(calculator);
  Expression oneOverSqrtE(calculator);
  twoE = Rational(1, 2);
  oneOverSqrtE = 1;
  oneOverSqrtE /= theArgument;
  return output.makeXOX(calculator, calculator.opTimes(), twoE, oneOverSqrtE);
}

bool CalculatorFunctionsDifferentiation::differentiateWithRespectToXTimesAny(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsDifferentiation::differentiateWithRespectToXTimesAny");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (!input[1].startsWith(calculator.opDifferentiate(), 2)) {
    return false;
  }
  if (input[2].isBuiltInAtom()) {
    return false;
  }
  calculator.checkInputNotSameAsOutput(input, output);
  output = input[1];
  return output.addChildOnTop(input[2]);
}

bool CalculatorFunctionsDifferentiation::diffdivDiffxToDifferentiation(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::diffdivDiffxToDifferentiation");
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  bool hasArgument = false;
  bool hasExtraCF = false;
  Expression theArgument, extraCoeff;
  if (input[1] != "Differential" && input[1] != "d") {
    if (!input[1].startsWith(calculator.opDifferential())) {
      return false;
    }
    if (input[1].size() > 3) {
      return false;
    }
    theArgument = input[1][1];
    if (input[1].size() == 3) {
      extraCoeff = input[1][2];
      hasExtraCF = true;
    }
    hasArgument = true;
  }
  if (input[2].size() < 2) {
    return false;
  }
  if (input[2][0] != "Differential") {
    return false;
  }
  output.reset(calculator, 2);
  output.addChildAtomOnTop(calculator.opDifferentiate());
  output.addChildOnTop(input[2][1]);
  if (hasArgument) {
    output.addChildOnTop(theArgument);
  }
  if (hasExtraCF) {
    Expression outputCopy = output;
    output = extraCoeff * outputCopy;
  }
  return true;
}

bool CalculatorFunctionsDifferentiation::ddivDxToDiffDivDiffx(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDdivDxToDifferentiation");
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  std::string denominatorString,numeratorString;
  if (!input[1].isOperation(&numeratorString)) {
    return false;
  }
  if (!input[2].isOperation(&denominatorString)) {
    return false;
  }
  if (numeratorString != "d") {
    return false;
  }
  if (denominatorString.size() < 2) {
    return false;
  }
  if (denominatorString[0] != 'd') {
    return false;
  }
  for (int i = 0; i < static_cast<signed>(denominatorString.size()) - 1; i ++) {
    unsigned k = static_cast<unsigned>(i);
    denominatorString[k] = denominatorString[k + 1];
  }
  denominatorString.resize(denominatorString.size() - 1);
  Expression numeratorE, denominatorE(calculator), rightDenE;
  numeratorE.makeAtom(calculator.opDifferential(), calculator);
  rightDenE.makeAtom(calculator.addOperationNoRepetitionOrReturnIndexFirst(denominatorString), calculator);
  denominatorE.addChildOnTop(numeratorE);
  denominatorE.addChildOnTop(rightDenE);
  return output.makeXOX(calculator, calculator.opDivide(), numeratorE, denominatorE);
}

bool CalculatorFunctions::outerMergeConstantRadicals(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerMergeConstantRadicals");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (
    !input[1].startsWith(calculator.opThePower(), 3) ||
    !input[2].startsWith(calculator.opThePower(), 3)
  ) {
    return false;
  }
  if (input[1][2] != input[2][2]) {
    return false;
  }
  if (
    !input[1][1].isOfType<Rational>() && !input[1][1].isOfType<AlgebraicNumber>() &&
    !input[1][1].isOfType<double>()
  ) {
    return false;
  }
  if (
    !input[2][1].isOfType<Rational>() &&
    !input[2][1].isOfType<AlgebraicNumber>() &&
    !input[2][1].isOfType<double>()
  ) {
    return false;
  }
  Expression theProduct;
  theProduct.makeProduct(calculator, input[1][1], input[2][1]);
  return output.makeXOX(calculator, calculator.opThePower(), theProduct, input[1][2]);
}

bool CalculatorFunctions::outerCommuteConstants(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerCommuteConstants");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (!input[2].isConstantNumber()) {
    return false;
  }
  if (input[1].isConstantNumber()) {
    return false;
  }
  output.makeProduct(calculator, input[2], input[1]);
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctions::outerDivideReplaceAdivBpowerItimesBpowerJ(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::outerDivideReplaceAdivBpowerItimesBpowerJ");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (!input[1].startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  Expression denominatorBase, denominatorExponent;
  Expression numeratorBase, numeratorExponent;
  input[1][2].getBaseExponentForm(denominatorBase, denominatorExponent);
  input[2].getBaseExponentForm(numeratorBase, numeratorExponent);
  if (denominatorBase != numeratorBase) {
    return false;
  }
  calculator.checkInputNotSameAsOutput(input, output);
  Expression rightMultiplicand, rightMultiplicandExponent;
  rightMultiplicandExponent.makeXOX(calculator, calculator.opMinus(), numeratorExponent, denominatorExponent);
  rightMultiplicand.makeXOX(calculator, calculator.opThePower(), denominatorBase, rightMultiplicandExponent);
  return output.makeXOX(calculator, calculator.opTimes(), input[1][1], rightMultiplicand);
}

bool Expression::splitProduct(
  int numDesiredMultiplicandsLeft,
  Expression& outputLeftMultiplicand,
  Expression& outputRightMultiplicand
) const {
  MacroRegisterFunctionWithName("Expression::splitProduct");
  if (numDesiredMultiplicandsLeft <= 0) {
    return false;
  }
  this->checkInitialization();
  List<Expression> theMultiplicandsLeft, theMultiplicandsRight;
  this->owner->appendOpandsReturnTrueIfOrderNonCanonical(*this, theMultiplicandsLeft, this->owner->opTimes());
  if (theMultiplicandsLeft.size <= numDesiredMultiplicandsLeft) {
    return false;
  }
  theMultiplicandsRight.setExpectedSize(theMultiplicandsLeft.size-numDesiredMultiplicandsLeft);
  for (int i = numDesiredMultiplicandsLeft; i < theMultiplicandsLeft.size; i ++) {
    theMultiplicandsRight.addOnTop(theMultiplicandsLeft[i]);
  }
  theMultiplicandsLeft.setSize(numDesiredMultiplicandsLeft);
  outputLeftMultiplicand.makeOXdotsX(*this->owner, this->owner->opTimes(), theMultiplicandsLeft);
  return outputRightMultiplicand.makeOXdotsX(*this->owner, this->owner->opTimes(), theMultiplicandsRight);
}

bool CalculatorFunctions::outerAtimesBpowerJplusEtcDivBpowerI(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::outerAtimesBpowerJplusEtcDivBpowerI");
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  Expression denominatorBase, denominatorExponent;
  input[2].getBaseExponentForm(denominatorBase, denominatorExponent);
  if (!denominatorBase.divisionByMeShouldBeWrittenInExponentForm()) {
    return false;
  }
  LinearCombination<Expression, Rational> numerators, numeratorsNew;
  calculator.functionCollectSummands(calculator, input[1], numerators);
  numeratorsNew.setExpectedSize(numerators.size());
  numeratorsNew.makeZero();
  Expression numeratorMultiplicandLeft, numeratorMultiplicandRight, numeratorBaseRight, numeratorExponentRight;
  Expression newNumSummand, newNumSummandRightPart, newNumExponent, mOneE;
  mOneE.assignValue(- 1, calculator);
  for (int i = 0; i < numerators.size(); i ++) {
    if (numerators[i].isConstantNumber()) {
      newNumSummandRightPart.makeXOX(
        calculator,
        calculator.opThePower(),
        denominatorBase,
        mOneE * denominatorExponent
      );
      newNumSummand = numerators[i] * newNumSummandRightPart;
      numeratorsNew.addMonomial(newNumSummand, numerators.coefficients[i]);
      continue;
    }
    numerators[i].getBaseExponentForm(numeratorBaseRight, numeratorExponentRight);
    if (numeratorBaseRight == denominatorBase) {
      newNumExponent.makeXOX(calculator, calculator.opMinus(), numeratorExponentRight, denominatorExponent);
      newNumSummand.makeXOX(calculator, calculator.opThePower(), denominatorBase, newNumExponent);
      numeratorsNew.addMonomial(newNumSummand, numerators.coefficients[i]);
      continue;
    }
    bool isGood = false;
    for (int j = 1; numerators[i].splitProduct(j, numeratorMultiplicandLeft, numeratorMultiplicandRight); j ++) {
      numeratorMultiplicandRight.getBaseExponentForm(numeratorBaseRight, numeratorExponentRight);
      if (numeratorBaseRight != denominatorBase) {
        continue;
      }
      newNumExponent.makeXOX(calculator, calculator.opMinus(), numeratorExponentRight, denominatorExponent);
      newNumSummandRightPart.makeXOX(calculator, calculator.opThePower(), denominatorBase, newNumExponent);
      newNumSummand.makeXOX(calculator, calculator.opTimes(), numeratorMultiplicandLeft, newNumSummandRightPart);
      numeratorsNew.addMonomial(newNumSummand, numerators.coefficients[i]);
      isGood = true;
      break;
    }
    if (!isGood) {
      return false;
    }
  }
  return output.makeSum(calculator, numeratorsNew);
}


void Expression::getBlocksOfCommutativity(HashedListSpecialized<Expression>& inputOutputList) const {
  MacroRegisterFunctionWithName("Expression::getBlocksOfCommutativity");
  this->checkInitialization();
  if (this->isAtom()) {
    inputOutputList.addOnTopNoRepetition(*this);
    return;
  }
  if (this->isConstantNumber()) {
    return;
  }
  std::string whichOperation;
  if ((*this)[0].isOperation(&whichOperation)) {
    if (this->owner->atomsThatAllowCommutingOfCompositesStartingWithThem.contains(whichOperation)) {
      for (int i = 1; i < this->size(); i ++) {
        (*this)[i].getBlocksOfCommutativity(inputOutputList);
      }
      return;
    }
  }
  inputOutputList.addOnTopNoRepetition(*this);
}

bool Expression::makeMatrix(Calculator& owner, Matrix<Expression>* inputMat) {
  MacroRegisterFunctionWithName("Expression::makeMatrix");
  if (inputMat == nullptr) {
    this->reset(owner);
    Expression theMatID(owner);
    theMatID.addChildAtomOnTop(owner.opMatrix());
    this->addChildOnTop(theMatID);
    return true;
  }
  return this->assignMatrixExpressions(*inputMat, owner, true, true);
}

bool Expression::makeSequence(Calculator& owner, List<Expression>* inputSequence) {
  MacroRegisterFunctionWithName("Expression::makeSequence");
  this->reset(owner, inputSequence == nullptr ? 1 : inputSequence->size + 1);
  this->addChildAtomOnTop(owner.opSequence());
  if (inputSequence != nullptr) {
    for (int i = 0; i < inputSequence->size; i ++) {
      this->addChildOnTop((*inputSequence)[i]);
    }
  }
  return true;
}

bool Expression::makeSequenceCommands(Calculator& owner, List<std::string>& inputKeys, List<Expression>& inputValues) {
  MacroRegisterFunctionWithName("Expression::makeSequenceCommands");
  List<Expression> theStatements;
  Expression currentStatement, currentKey;
  if (inputValues.size != inputKeys.size) {
    global.fatal << "I am asked to create a "
    << "sequence of statements but I was given different "
    << "number of keys and expressions." << global.fatal;
  }
  for (int i = 0; i < inputValues.size; i ++) {
    currentKey.makeAtom(inputKeys[i], owner);
    currentStatement.makeXOX(owner, owner.opDefine(), currentKey, inputValues[i]);
    theStatements.addOnTop(currentStatement);
  }
  return this->makeSequenceStatements(owner, &theStatements);
}

bool Expression::makeSequenceStatements(Calculator& owner, List<Expression>* inputStatements) {
  MacroRegisterFunctionWithName("Expression::makeSequenceStatements");
  this->reset(owner, inputStatements == nullptr ? 1 : inputStatements->size + 1);
  this->addChildAtomOnTop(owner.opCommandSequence());
  if (inputStatements != nullptr) {
    for (int i = 0; i < inputStatements->size; i ++) {
      this->addChildOnTop((*inputStatements)[i]);
    }
  }
  return true;
}

bool CalculatorFunctions::innerGetUserDefinedSubExpressions(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGetUserDefinedSubExpressions");
  if (input.size() != 2) {
    return false;
  }
  HashedListSpecialized<Expression> theList;
  input[1].getBlocksOfCommutativity(theList);
  return output.makeSequence(calculator, &theList);
}

bool CalculatorFunctions::innerLispify(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerLispify");
  if (input.size() != 2) {
    return false;
  }
  return output.assignValue(input[1].toStringSemiFull(), calculator);
}

bool CalculatorFunctions::innerLispifyFull(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerLispifyFull");
  if (input.size() != 2) {
    return false;
  }
  return output.assignValue(input[1].toStringFull(), calculator);
}

bool CalculatorFunctions::innerMinPolyMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerMinPolyMatrix");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (!CalculatorConversions::functionMatrixRational(
    calculator, argument, output
  )) {
    return false;
  }
  Matrix<Rational> theMat;
  if (!calculator.functionGetMatrix(argument, theMat)) {
    return calculator
    << "<hr>Minimal poly computation: could not convert "
    << argument.toString() << " to rational matrix.";
  }
  if (theMat.numberOfRows != theMat.numberOfColumns || theMat.numberOfRows <= 0) {
    return output.makeError("Error: matrix is not square.", calculator);
  }
  FormatExpressions tempF;
  tempF.polynomialAlphabet.setSize(1);
  tempF.polynomialAlphabet[0] = "q";
  Polynomial<Rational> theMinPoly;
  theMinPoly.assignMinimalPolynomial(theMat);
  return output.assignValue(theMinPoly.toString(&tempF), calculator);
}

bool CalculatorFunctions::innerCharPolyMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerMinPolyMatrix");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Matrix<Rational> theMat;
  if (!calculator.functionGetMatrix(argument, theMat)) {
    return calculator
    << "<hr>Characteristic poly computation: could not convert "
    << input.toString() << " to rational matrix.";
  }
  if (theMat.numberOfRows != theMat.numberOfColumns || theMat.numberOfRows <= 0) {
    return output.makeError("Error: matrix is not square.", calculator);
  }
  FormatExpressions tempF;
  tempF.polynomialAlphabet.setSize(1);
  tempF.polynomialAlphabet[0] = "q";
  Polynomial<Rational> theCharPoly;
  theCharPoly.assignCharacteristicPolynomial(theMat);
  return output.assignValue(theCharPoly.toString(&tempF), calculator);
}

bool CalculatorFunctions::innerReverseBytes(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerReverseBytes");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  std::string result;
  result.resize(inputString.size());
  for (unsigned i = 0; i < inputString.size(); i ++) {
    result[i] = inputString[inputString.size() - i - 1];
  }
  return output.assignValue(result, calculator);
}

bool CalculatorFunctions::innerTrace(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTrace");
  if (input.size() != 2) {
    return false;
  }
  Matrix<Rational> theMat;
  if (calculator.functionGetMatrix(input[1], theMat)) {
    if (!theMat.isSquare()) {
      return output.makeError(
        "Error: attempting to get trace of non-square matrix. ",
        calculator
      );
    }
    return output.assignValue(theMat.getTrace(), calculator);
  }
  Matrix<RationalFunction<Rational> > theMatRF;
  if (calculator.functionGetMatrix(input[1], theMatRF)) {
    if (!theMatRF.isSquare()) {
      return output.makeError(
        "Error: attempting to get trace of non-square matrix. ",
        calculator
      );
    }
    return output.assignValue(theMatRF.getTrace(), calculator);
  }
  Matrix<Expression> theMatExp;
  if (!calculator.getMatrixExpressionsFromArguments(input[1], theMatExp)) {
    return false;
  }
  if (!theMat.isSquare()) {
    return output.makeError(
      "Error: attempting to get trace of non-square matrix. ",
      calculator
    );
  }
  if (theMat.numberOfRows == 1) {
    calculator << "Requested trace of 1x1 matrix: possible "
    << "interpretation of a scalar as a 1x1 matrix. Trace not taken. ";
    return false;
  }
  output = theMat.getTrace();
  return true;
}

bool CalculatorFunctions::innerContains(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerContains");
  if (input.size() != 3) {
    return false;
  }
  if (input[1].containsAsSubExpressionNoBuiltInTypes(input[2])) {
    return output.assignValue(1, calculator);
  } else {
    return output.assignValue(0, calculator);
  }
}

bool CalculatorFunctions::innerExpressionLeafs(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerExpressionLeafs");
  HashedList<Expression> theLeafs;
  if (input.startsWithGivenOperation("ExpressionLeafs")) {
    for (int i = 1; i < input.size(); i ++) {
      if (!input[i].getExpressionLeafs(theLeafs)) {
        return calculator << "Failed to extract expression leafs from " << input.toString() << ".";
      }
    }
  } else if (!input.getExpressionLeafs(theLeafs)) {
    return calculator << "Failed to extract expression leafs from " << input.toString() << ".";
  }
  return output.makeSequence(calculator, &theLeafs);
}

bool CalculatorFunctions::innerLastElement(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerLastElement");
  if (input.hasBoundVariables()) {
    return false;
  }
  if (input.isAtom()) {
    std::stringstream out;
    out << "Error: requesting the last element of the atom " << input.toString();
    return output.makeError(out.str(), calculator);
  }
  if (input.size() == 2) {
    const Expression& sequenceCandidate = input[1];
    if (sequenceCandidate.isSequenceNElements()) {
      output = sequenceCandidate[sequenceCandidate.size() - 1];
      return true;
    }
  }
  output = input[input.size() - 1];
  return true;
}

bool CalculatorFunctions::innerRemoveLastElement(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerRemoveLastElement");
  if (input.hasBoundVariables()) {
    return false;
  }
  if (input.isAtom() || input.size() == 1) {
    std::stringstream out;
    if (input.isAtom()) {
      out << "Error: requesting to remove the last element of the atom " << input.toString() << ". ";
    } else {
      out << "Error: requesting to remove the last element of the zero-element list " << input.toString() << ". ";
    }
    return output.makeError(out.str(), calculator);
  }
  if (input.size() == 2) {
    const Expression& sequenceCandidate = input[1];
    if (sequenceCandidate.isSequenceNElements() && sequenceCandidate.size() > 1) {
      output = sequenceCandidate;
      output.children.removeLastObject();
      return true;
    }
  }
  output = input;
  output.children.removeLastObject();
  return output.setChildAtomValue(0, calculator.opSequence());
}

bool ElementZmodP::operator==(int other) const {
  this->checkIamInitialized();
  ElementZmodP tempElt;
  tempElt.modulus = this->modulus;
  tempElt = LargeInteger(other);
  return *this == tempElt;
}

bool ElementZmodP::operator==(const ElementZmodP& other) const {
  this->checkIamInitialized();
  other.checkIamInitialized();
  return this->modulus == other.modulus && this->value == other.value;
}

void ElementZmodP::operator*=(const ElementZmodP& other) {
  if (this == &other) {
    ElementZmodP other = *this;
    *this *= other;
    return;
  }
  this->checkEqualModuli(other);
  this->value *= other.value;
  this->value %= this->modulus;
}

bool CalculatorFunctions::innerIsPossiblyPrime(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsPossiblyPrime");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger theInt;
  if (!input[1].isInteger(&theInt)) {
    return false;
  }
  bool resultBool = theInt.value.isPossiblyPrime(10, true, &calculator.comments);
  Rational result = 1;
  if (!resultBool) {
    result = 0;
  }
  return output.assignValue(result, calculator);
}

bool CalculatorFunctions::innerIsPrimeMillerRabin(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsPrimeMillerRabin");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger theInt;
  if (!input[1].isInteger(&theInt)) {
    return false;
  }
  bool resultBool = theInt.value.isPossiblyPrimeMillerRabin(10, &calculator.comments);
  Rational result = 1;
  if (!resultBool) {
    result = 0;
  }
  return output.assignValue(result, calculator);
}

bool CalculatorFunctions::innerIsNilpotent(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsNilpotent");
  Expression converted;
  if (!CalculatorConversions::innerMakeMatrix(calculator, input, converted)) {
    return false;
  }
  bool found = false;
  Matrix<Rational> theMat;
  MatrixTensor<Rational> theMatTensor;
  if (calculator.functionGetMatrix(converted, theMat)) {
    found = true;
    theMatTensor = theMat;
  } else if (input.isOfType<MatrixTensor<Rational> >(&theMatTensor)) {
    found = true;
  }
  if (!found) {
    return output.makeError(
      "Failed to extract matrix with rational coefficients", calculator
    );
  }
  if (theMatTensor.isNilpotent()) {
    return output.assignValue(1, calculator);
  }
  return output.assignValue(0, calculator);
}

bool CalculatorFunctions::innerInvertMatrix(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerInvertMatrix");
  Matrix<Rational> theMat;
  Expression converted;
  if (!CalculatorConversions::innerMakeMatrix(
    calculator, input, converted
  )) {
    return calculator << "Failed to extract matrix from input. ";
  }
  if (calculator.functionGetMatrix(converted, theMat)) {
    if (theMat.numberOfRows != theMat.numberOfColumns || theMat.numberOfColumns < 1) {
      return output.makeError("The matrix is not square", calculator);
    }
    if (theMat.getDeterminant() == 0) {
      return output.makeError("Matrix determinant is zero.", calculator);
    }
    theMat.invert();
    return output.assignMatrix(theMat, calculator);
  }
  Matrix<AlgebraicNumber> theMatAlg;
  if (calculator.functionGetMatrix(input, theMatAlg)) {
    return calculator << "<hr>Failed to extract algebraic number matrix from: "
    << input.toString();
  }
  if (theMatAlg.numberOfRows != theMatAlg.numberOfColumns || theMatAlg.numberOfColumns < 1) {
    return output.makeError("The matrix is not square", calculator);
  }
  if (theMatAlg.getDeterminant() == 0) {
    return output.makeError("Matrix determinant is zero.", calculator);
  }
  theMatAlg.invert();
  return output.assignMatrix(theMatAlg, calculator);
}

bool CalculatorFunctions::innerDFQsEulersMethod(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDFQsEulersMethod");
  if (input.size() != 7) {
    return calculator << "Euler method function takes 6 arguments";
  }
  double xInitial, yInitial, leftEndpoint, rightEndpoint;
  int numPoints;
  if (!input[2].evaluatesToDouble(&xInitial) || !input[3].evaluatesToDouble(&yInitial)) {
    return calculator << "Failed to extract initial x,y values from " << input.toString();
  }
  if (!input[4].isSmallInteger(&numPoints)) {
    return calculator << "Failed to extract number of points from " << input.toString();
  }
  if (!input[5].evaluatesToDouble(&leftEndpoint) || !input[6].evaluatesToDouble(&rightEndpoint)) {
    return calculator << "Failed to extract left and right endpoints from " << input.toString();
  }
  if (leftEndpoint > rightEndpoint) {
    MathRoutines::swap(leftEndpoint, rightEndpoint);
  }
  if (xInitial > rightEndpoint || xInitial < leftEndpoint) {
    return calculator << "The initial value for x, " << xInitial
    << ", does not lie between the left and right endpoints "
    << leftEndpoint << ", " << rightEndpoint << ". I am aborting Euler's method. ";
  }
  HashedList<Expression> knownConsts;
  List<double> knownValues;
  knownConsts.addListOnTop(calculator.knownDoubleConstants);
  knownValues.addListOnTop(calculator.knownDoubleConstantValues);
  Expression xE, yE;
  xE.makeAtom("x", calculator);
  yE.makeAtom("y", calculator);
  if (knownConsts.contains(xE) || knownConsts.contains(yE)) {
    return calculator << "The letters x, y appear to be already used to "
    << "denote known constants, I cannot run Euler's method.";
  }
  knownConsts.addOnTop(xE);
  knownConsts.addOnTop(yE);
  knownValues.addOnTop(0);
  knownValues.addOnTop(0);
  if (numPoints < 2) {
    return calculator << "The number of points for Euler's method is " << numPoints << ", too few. ";
  }
  if (numPoints > 10001) {
    return calculator << "The number of points for Euler's method is " << numPoints
    << ", I am not allowed to handle that many. ";
  }
  if (leftEndpoint - rightEndpoint == 0.0) {
    return calculator << "Whlie doing Euler's method: right endpoint equals left! ";
  }
  double delta = (rightEndpoint - leftEndpoint) / numPoints;
  List<double> XValues, YValues;
  XValues.setExpectedSize(numPoints + 5);
  YValues.setExpectedSize(numPoints + 5);
  int pointsCounter = 0;
  for (double currentX = xInitial; currentX > leftEndpoint - delta; currentX -= delta) {
    XValues.addOnTop(currentX);
    pointsCounter ++;
    if (pointsCounter > numPoints) {
      break; //<-in case floating point arithmetic is misbehaving
    }
  }
  XValues.reverseElements();
  int indexXinitial = XValues.size - 1;
  pointsCounter = 0;
  for (double currentX = xInitial + delta; currentX < rightEndpoint + delta; currentX += delta) {
    XValues.addOnTop(currentX);
    pointsCounter ++;
    if (pointsCounter > numPoints) {
      break; //<-in case floating point arithmetic is misbehaving
    }
  }
  YValues.initializeFillInObject(XValues.size, 0);
  YValues[indexXinitial] = yInitial;
  Expression functionE = input[1];
  double currentYprimeApprox = 0;
  int lastGoodXIndex = indexXinitial;
  int firstGoodXIndex = indexXinitial;
  double maxYallowed = (rightEndpoint - leftEndpoint) * 2 + yInitial;
  double minYallowed = - (rightEndpoint - leftEndpoint) * 2 + yInitial;
  for (int direction = - 1; direction < 2; direction += 2) {
    for (int i = indexXinitial + direction; i >= 0 && i < XValues.size; i += direction) {
      knownValues[knownValues.size - 2] = XValues[i];
      bool isGood = true;
      for (int counter = 0; ; counter ++) {
        knownValues[knownValues.size - 1] = YValues[i];
        if (!functionE.evaluatesToDoubleUnderSubstitutions(knownConsts, knownValues, &currentYprimeApprox)) {
          return calculator << "Failed to evaluate yPrime approximation at x =" << XValues[i];
        }
        double adjustment = delta * direction * currentYprimeApprox;
        double ynew = YValues[i - direction] + adjustment;
        double difference = ynew - YValues[i];
        YValues[i] = ynew;
        if (counter > 20) {
          //<- we run Euler algorithm at least 20 times.
          if (difference > - 0.01 && difference < 0.01) { //<-if changes are smaller than 0.01 we assume success.
            break;
          }
        }
        if (counter > 200) {
          //<- we ran Euler algorithm 100 times, but the difference is still greater than 0.01. Something is wrong, we abort
          calculator << "Euler method: no convergence. At x =" << XValues[i] << ", y ="
          << YValues[i] << ", the change in y is: " << difference << " after " << counter << " iterations. ";
          if (counter > 205) {
            isGood = false;
            break;
          }
        }
      }
      if (isGood) {
        isGood = (YValues[i] <= maxYallowed && YValues[i] >= minYallowed);
        if (!isGood) {
          calculator << "Euler method: y values outside of the bounding box. At x =" << XValues[i]
          << ", the y value is: " << YValues[i] << ". Max y is  " << maxYallowed << " and min y is " << minYallowed << ". ";
        }
      }
      if (direction == - 1) {
        firstGoodXIndex = i + 1;
      } else {
        lastGoodXIndex = i - 1;
      }
      if (!isGood) {
        break;
      }
    }
  }
  PlotObject thePlot;
  Vector<double> currentPt;
  currentPt.setSize(2);
  for (int i = firstGoodXIndex; i <= lastGoodXIndex; i ++) {
    currentPt[0] = XValues[i];
    currentPt[1] = YValues[i];
    thePlot.thePointsDouble.addOnTop(currentPt);
  }
  thePlot.xLow = XValues[0];
  thePlot.xHigh = *XValues.lastObject();
  thePlot.yLow = - 0.5;
  thePlot.yHigh = 0.5;
  thePlot.coordinateFunctionsE.addOnTop(input);
  std::stringstream outLatex, outHtml;
  outLatex << "\n\n%calculator input:" << input.toString() << "\n\n"
  << "\\psline[linecolor =\\fcColorGraph]";
  outHtml << "\n<br>\n%calculator input:" << input.toString() << "\n<br>\n"
  << "\\psline[linecolor =\\fcColorGraph]";
  for (int i = firstGoodXIndex; i <= lastGoodXIndex; i ++) {
    outLatex << std::fixed << "(" << XValues[i] << ", " << YValues[i] << ")";
    outHtml << std::fixed << "(" << XValues[i] << ", " << YValues[i] << ")";
    thePlot.yLow = MathRoutines::minimum(YValues[i], thePlot.yLow);
    thePlot.yHigh = MathRoutines::maximum(YValues[i], thePlot.yHigh);
  }
  thePlot.thePlotString = outLatex.str();
  thePlot.thePlotStringWithHtml = outHtml.str();
  return output.assignValue(thePlot, calculator);
}

bool CalculatorFunctionsPlot::plotViewWindow(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPlot::plotViewWindow");
  if (input.size() < 3) {
    return false;
  }
  Vector<double> widthHeight;
  Plot emptyPlot;
  emptyPlot.priorityWindow = 1;
  bool isGood = false;
  if (calculator.getVectorDoublesFromFunctionArguments(input, widthHeight, 2)) {
    isGood = true;
  } else {
    widthHeight.setSize(2);
    widthHeight[0] = 100;
    widthHeight[1] = 100;
    MapList<std::string, Expression, MathRoutines::hashString> theMap;
    if (!CalculatorConversions::innerLoadKeysFromStatementList(
      calculator, input, theMap, nullptr, false
    )) {
      isGood = false;
    } else {
      if (theMap.contains("width")) {
        if (!theMap.getValueCreateNoInit("width").evaluatesToDouble(&widthHeight[0])) {
          isGood = false;
        }
      }
      if (theMap.contains("height")) {
        if (!theMap.getValueCreateNoInit("height").evaluatesToDouble(&widthHeight[1])) {
          isGood = false;
        }
      }
    }
  }
  if (!isGood) {
    return calculator << "Failed to extract a pair of positive numbers from: "
    << input.toString();
  }
  emptyPlot.desiredHtmlWidthInPixels = static_cast<int>(widthHeight[0]);
  emptyPlot.desiredHtmlHeightInPixels = static_cast<int>(widthHeight[1]);
  return output.assignValue(emptyPlot, calculator);
}

bool CalculatorFunctionsPlot::plotSetId(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPlot::plotSetId");
  Plot emptyPlot;
  emptyPlot.priorityCanvasName = 1;
  emptyPlot.desiredHtmlHeightInPixels = 100;
  emptyPlot.desiredHtmlWidthInPixels = 100;
  std::string incomingName;
  if (!input.isOfType(&incomingName)) {
    incomingName = input.toString();
  }
  emptyPlot.setCanvasName(incomingName);
  return output.assignValue(emptyPlot, calculator);
}

bool CalculatorFunctionsPlot::plotViewRectangle(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPlot::plotViewRectangle");
  if (input.size() < 3) {
    return false;
  }
  Vector<double> lowerLeft, upperRight;
  if (
    !calculator.getVectorDoubles(input[1], lowerLeft, 2) ||
    !calculator.getVectorDoubles(input[2], upperRight, 2)
  ) {
    return calculator << "Failed to extract two pairs of floating point numbers from: "
    << input[1].toString() << " and " << input[2].toString();
  }
  Plot emptyPlot;
  emptyPlot.dimension = 2;
  emptyPlot.theLowerBoundAxes = lowerLeft[0];
  emptyPlot.lowBoundY = lowerLeft[1];
  emptyPlot.theUpperBoundAxes = upperRight[0];
  emptyPlot.highBoundY = upperRight[1];
  emptyPlot.priorityViewRectangle = 1;
  emptyPlot.desiredHtmlHeightInPixels = 100;
  emptyPlot.desiredHtmlWidthInPixels = 100;
  return output.assignValue(emptyPlot, calculator);
}

bool CalculatorFunctionsPlot::plotFill(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPlot::plotFill");
  if (input.size() < 3) {
    return false;
  }
  const Expression& thePlotE = input[1];
  const Expression& colorE = input[2];
  Plot outputPlot, startPlot;
  outputPlot.dimension = 2;
  PlotObject theFilledPlot;
  if (!thePlotE.isOfType<Plot>(&startPlot)) {
    return false;
  }
  std::string colorString;
  if (!colorE.isOfType<std::string>(&colorString)) {
    colorString = colorE.toString();
  }
  if (!DrawingVariables::getColorIntFromColorString(colorString, theFilledPlot.colorFillRGB)) {
    calculator << "Failed to extract color from: " << colorE.toString() << "; using default color value. ";
  }
  theFilledPlot.colorFillJS = colorString;
  for (int i = 0; i < startPlot.thePlots.size; i ++) {
    theFilledPlot.thePointsDouble.addListOnTop(startPlot.thePlots[i].thePointsDouble);
  }
  theFilledPlot.fillStyle = "filled";
  theFilledPlot.thePlotType = "plotFillStart";
  outputPlot.desiredHtmlHeightInPixels = startPlot.desiredHtmlHeightInPixels;
  outputPlot.desiredHtmlWidthInPixels = startPlot.desiredHtmlWidthInPixels;
  outputPlot.thePlots.addOnTop(theFilledPlot);
  outputPlot += startPlot;
  theFilledPlot.thePlotType = "plotFillFinish";
  outputPlot.thePlots.addOnTop(theFilledPlot);
  return output.assignValue(outputPlot, calculator);
}

bool CalculatorFunctionsPlot::isPlot(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPlot::isPlot");
  if (input.size() != 2) {
    return false;
  }
  if (input[1].isOfType<Plot>()) {
    return output.assignValue(1, calculator);
  }
  return output.assignValue(0, calculator);
}

bool CalculatorFunctionsPlot::plot2DOverIntervals(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPlot::plot2DOverIntervals");
  if (input.size() < 3) {
    return false;
  }
  List<Expression> theIntervalCandidates;
  if (!calculator.collectOpands(input[2], calculator.opUnion(),theIntervalCandidates)) {
    return false;
  }
  if (theIntervalCandidates.size < 1) {
    return false;
  }
  for (int i = 0; i < theIntervalCandidates.size; i ++) {
    if (!theIntervalCandidates[i].isIntervalRealLine()) {
      return false;
    }
  }
  Expression summandE;
  List<Expression> finalSummands;
  for (int i = 0; i < theIntervalCandidates.size; i ++) {
    summandE.reset(calculator);
    summandE.addChildOnTop(input[0]);
    summandE.addChildOnTop(input[1]);
    summandE.addChildOnTop(theIntervalCandidates[i][1]);
    summandE.addChildOnTop(theIntervalCandidates[i][2]);
    for (int j = 3; j < input.size(); j ++) {
      summandE.addChildOnTop(input[j]);
    }
    finalSummands.addOnTop(summandE);
  }
  return output.makeSum(calculator, finalSummands);
}

bool CalculatorFunctionsPlot::plot2D(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPlot::plot2D");
  if (input.size() < 4) {
    return output.makeError(
      "Plotting coordinates takes at least three arguments: function, lower and upper bound. ",
      calculator
    );
  }
  if (input.hasBoundVariables()) {
    return false;
  }
  Plot thePlot;
  thePlot.dimension = 2;
  PlotObject thePlotObj;
  thePlotObj.leftPtE = input[2];
  thePlotObj.rightPtE = input[3];
  if (input.size() >= 5) {
    if (!input[4].isOfType<std::string>(&thePlotObj.colorJS)) {
      thePlotObj.colorJS = input[4].toString();
    }
  } else {
    thePlotObj.colorJS = "red";
  }
  thePlotObj.colorRGB = static_cast<int>(HtmlRoutines::redGreenBlue(255, 0, 0));
  DrawingVariables::getColorIntFromColorString(thePlotObj.colorJS, thePlotObj.colorRGB);
  thePlotObj.lineWidth = 1;
  if (input.size() >= 6) {
    input[5].evaluatesToDouble(&thePlotObj.lineWidth);
  }
  if (input.size() >= 7) {
    thePlotObj.numSegmentsE = input[6];
  } else {
    thePlotObj.numSegmentsE.assignValue(500, calculator);
  }
  int numIntervals = - 1;
  if (!thePlotObj.numSegmentsE.isSmallInteger(&numIntervals)) {
    numIntervals = 500;
  }
  if (numIntervals < 2) {
    numIntervals = 2;
  }
  bool leftIsDouble = thePlotObj.leftPtE.evaluatesToDouble(&thePlotObj.xLow);
  bool rightIsDouble = thePlotObj.rightPtE.evaluatesToDouble(&thePlotObj.xHigh);
  if (!leftIsDouble) {
    if (thePlotObj.leftPtE != calculator.expressionMinusInfinity()) {
      return calculator
      << "Couldn't convert left boundary "
      << thePlotObj.leftPtE.toString() << " to floating point number. ";
    } else {
      thePlotObj.leftBoundaryIsMinusInfinity = true;
    }
  }
  if (!rightIsDouble) {
    if (thePlotObj.rightPtE != calculator.expressionInfinity()) {
      return calculator
      << "Couldn't convert right boundary "
      << thePlotObj.rightPtE.toString() << " to floating point number. ";
    } else {
      thePlotObj.rightBoundaryIsMinusInfinity = true;
    }
  }
  thePlotObj.coordinateFunctionsE.addOnTop(input[1]);
  thePlotObj.coordinateFunctionsJS.setSize(1);
  thePlotObj.coordinateFunctionsE[0].getFreeVariables(thePlotObj.variablesInPlay, true);
  if (thePlotObj.variablesInPlay.size > 1) {
    return calculator << "Got a function with "
    << thePlotObj.variablesInPlay.size
    << " variables, namely: "
    << thePlotObj.variablesInPlay.toStringCommaDelimited()
    << ". I was expecting a single variable. ";
  }
  if (thePlotObj.variablesInPlay.size == 0) {
    Expression xE;
    xE.makeAtom("x", calculator);
    thePlotObj.variablesInPlay.addOnTop(xE);
  }
  thePlotObj.variablesInPlayJS.setSize(thePlotObj.variablesInPlay.size);
  thePlotObj.variablesInPlayJS[0] = thePlotObj.variablesInPlay[0].toString();
  std::string theVarString = thePlotObj.variablesInPlayJS[0];
  Expression jsConverterE;
  thePlotObj.thePlotType = "plotFunction";
  if (CalculatorFunctions::functionMakeJavascriptExpression(
    calculator, thePlotObj.coordinateFunctionsE[0], jsConverterE
  )) {
    thePlotObj.coordinateFunctionsJS[0] = jsConverterE.toString();
    thePlotObj.coordinateFunctionsE[0].hasInputBoxVariables(&thePlot.boxesThatUpdateMe);
  } else {
    thePlotObj.thePlotType = "plotFunctionPrecomputed";
  }
  if (CalculatorFunctions::functionMakeJavascriptExpression(
    calculator, thePlotObj.leftPtE, jsConverterE
  )) {
    thePlotObj.leftPtJS = jsConverterE.toString();
    thePlotObj.leftPtE.hasInputBoxVariables(&thePlot.boxesThatUpdateMe);
  } else {
    thePlotObj.thePlotType = "plotFunctionPrecomputed";
  }
  if (CalculatorFunctions::functionMakeJavascriptExpression(
    calculator, thePlotObj.rightPtE, jsConverterE
  )) {
    thePlotObj.rightPtJS = jsConverterE.toString();
    thePlotObj.rightPtE.hasInputBoxVariables(&thePlot.boxesThatUpdateMe);
  } else {
    thePlotObj.thePlotType = "plotFunctionPrecomputed";
  }
  thePlotObj.numSegmenTsJS.setSize(1);
  thePlotObj.numSegmenTsJS[0] = "200";
  if (CalculatorFunctions::functionMakeJavascriptExpression(
    calculator, thePlotObj.numSegmentsE, jsConverterE
  )) {
    thePlotObj.numSegmenTsJS[0] = jsConverterE.toString();
    thePlotObj.numSegmentsE.hasInputBoxVariables(&thePlot.boxesThatUpdateMe);
  } else {
    thePlotObj.thePlotType = "plotFunctionPrecomputed";
  }
  Vectors<double>& thePointsDouble = thePlotObj.thePointsDouble;
  if (thePlot.boxesThatUpdateMe.size == 0) {
    if (!input[1].evaluatesToDoubleInRange(
      theVarString,
      thePlotObj.xLow,
      thePlotObj.xHigh,
      numIntervals,
      &thePlotObj.yLow,
      &thePlotObj.yHigh,
      &thePointsDouble
    )) {
      bool hasOneGoodPoint = false;
      for (int i = 0; i < thePointsDouble.size; i ++) {
        if (!std::isnan(thePointsDouble[i][1])) {
          hasOneGoodPoint = true;
          break;
        }
      }
      if (!hasOneGoodPoint) {
        return calculator << "<hr>I failed to evaluate the input function at all points, "
        << "perhaps your expression is not a function of x.";
      }
      calculator << "<hr>I failed to evaluate your function in a number of points. ";
    }
  }
  Expression functionSuffixNotationE;
  if (!calculator.callCalculatorFunction(
    CalculatorFunctions::innerSuffixNotationForPostScript, input[1], functionSuffixNotationE
  )) {
    calculator << "No LaTeX version: failed to convert: "
    << input[1].toString() << " to postfix notation. ";
  } else {
    thePlotObj.thePlotString = thePlotObj.
    getPlotStringFromFunctionStringAndRanges(
      false,
      functionSuffixNotationE.toString(),
      thePlotObj.coordinateFunctionsE[0].toString(),
      thePlotObj.xLow,
      thePlotObj.xHigh
    );
    thePlotObj.thePlotStringWithHtml = thePlotObj.thePlotString;
  }
  thePlot.thePlots.addOnTop(thePlotObj);
  return output.assignValue(thePlot, calculator);
}

bool CalculatorFunctionsPlot::plotPoint(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPlot::plotPoint");
  if (input.size() != 3) {
    return output.makeError(
      "Plotting a point takes at least two arguments, location and color. ",
      calculator
    );
  }
  Plot theFinalPlot;
  PlotObject thePlot;
  if (!calculator.getMatrixExpressions(input[1], thePlot.thePointS)) {
    return calculator << "The first argument of PlotPoint is "
    << "expected to be a sequence, instead I had: " << input[1].toString();
  }
  theFinalPlot.dimension = thePlot.thePointS.numberOfColumns;
  thePlot.dimension = theFinalPlot.dimension;
  thePlot.coordinateFunctionsE.setSize(thePlot.dimension);
  thePlot.coordinateFunctionsJS.setSize(thePlot.dimension);
  Expression jsConverterE;
  thePlot.thePointsJS.initialize(thePlot.thePointS.numberOfRows, thePlot.thePointS.numberOfColumns);
  for (int i = 0; i < thePlot.thePointS.numberOfRows; i ++) {
    for (int j = 0; j < thePlot.thePointS.numberOfColumns; j ++) {
      if (!CalculatorFunctions::functionMakeJavascriptExpression(
        calculator, thePlot.thePointS(i, j), jsConverterE
      )) {
        return calculator << "Failed to extract coordinate " << i + 1 << " from: "
        << thePlot.coordinateFunctionsE[i].toString();
      }
      thePlot.thePointsJS(i, j) = jsConverterE.toString();
      thePlot.thePointS(i, j).hasInputBoxVariables(&theFinalPlot.boxesThatUpdateMe);
    }
  }
  thePlot.dimension = theFinalPlot.dimension;
  thePlot.colorRGB =static_cast<int>(HtmlRoutines::redGreenBlue(0, 0, 0));
  if (input[2].isOfType<std::string>()) {
    DrawingVariables::getColorIntFromColorString(input[2].getValue<std::string>(), thePlot.colorRGB);
  }
  thePlot.colorJS = input[2].toString();
  thePlot.thePlotType = "points";
  theFinalPlot.thePlots.addOnTop(thePlot);
  theFinalPlot.desiredHtmlHeightInPixels = 100;
  theFinalPlot.desiredHtmlWidthInPixels = 100;
  return output.assignValue(theFinalPlot, calculator);
}

bool CalculatorFunctionsPlot::plot2DWithBars(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPlot::plot2DWithBars");
  if (input.size() < 6) {
    return output.makeError(
      "Plotting coordinates takes the following arguments: lower function, "
      "upper function, lower and upper bound, delta x. ",
      calculator
    );
  }
  Expression lowerEplot = input, upperEplot = input;
  lowerEplot.children.removeIndexShiftDown(2);
  upperEplot.children.removeIndexShiftDown(1);
  Plot outputPlot;
  outputPlot.dimension = 2;
  bool tempB = CalculatorFunctionsPlot::plot2D(calculator, lowerEplot, output);
  if (!tempB || !output.isOfType<Plot>(&outputPlot)) {
    return calculator << "<hr>Failed to get a plot from "
    << lowerEplot.toString() << ", not proceding with bar plot.";
  }
  tempB = CalculatorFunctionsPlot::plot2D(calculator, upperEplot, output);
  if (!tempB || !output.isOfType<Plot>()) {
    return calculator << "<hr>Failed to get a plot from "
    << upperEplot.toString() << ", not proceding with bar plot.";
  }
  outputPlot += output.getValue<Plot>();
  const Expression& lowerFunctionE = input[1];
  const Expression& upperFunctionE = input[2];
  const Expression& lowerE = input[3];
  const Expression& upperE = input[4];
  const Expression& deltaE = input[5];
  double theDeltaNoSign, theDeltaWithSign;
  if (!deltaE.evaluatesToDouble(&theDeltaWithSign)) {
    return false;
  }
  theDeltaNoSign = theDeltaWithSign;
  if (theDeltaNoSign < 0) {
    theDeltaNoSign *= - 1;
  }
  if (theDeltaNoSign == 0.0) {
    theDeltaNoSign = 1;
  }
  double upperBound, lowerBound;
  if (!lowerE.evaluatesToDouble(&lowerBound) || !upperE.evaluatesToDouble(&upperBound)) {
    return calculator
    << "Couldn't convert bounds of drawing function to floating point numbers. ";
  }
  if (upperBound < lowerBound) {
    MathRoutines::swap(upperBound, lowerBound);
  }
  Expression xValueE, xExpression, theFunValueEnonEvaluated, theFunValueFinal;
  xExpression.makeAtom(calculator.addOperationNoRepetitionOrReturnIndexFirst("x"), calculator);
  List<double> xValues;
  List<double> fValuesLower;
  List<double> fValuesUpper;
  if (theDeltaNoSign == 0.0) {
    return output.makeError("Delta equal to zero is not allowed", calculator);
  }
  if ((upperBound - lowerBound) / theDeltaNoSign > 10000) {
    return output.makeError("More than 10000 intervals needed for the plot, this is not allowed.", calculator);
  }
  List<Rational> rValues;
  Rational lowerBoundRat, upperBoundRat, deltaRat;
  if (
    lowerE.isOfType<Rational>(&lowerBoundRat) &&
    upperE.isOfType<Rational>(&upperBoundRat) &&
    deltaE.isOfType<Rational>(&deltaRat)
  ) {
    if (upperBoundRat<lowerBoundRat) {
      MathRoutines::swap(upperBoundRat, lowerBoundRat);
    }
    if (deltaRat < 0) {
      deltaRat *= - 1;
    }
    if (deltaRat == 0) {
      return output.makeError("Delta equal to zero is not allowed", calculator);
    }
    for (Rational i = lowerBoundRat; i <= upperBoundRat; i += deltaRat) {
      rValues.addOnTop(i);
    }
  }
  double yMax = - 0.5, yMin = 0.5;
  for (double i = lowerBound; i <= upperBound; i += theDeltaNoSign) {
    for (int j = 0; j < 2; j ++) {
      if (theDeltaWithSign < 0 && (i - lowerBound == 0.0)) {
        continue;
      }
      if (theDeltaWithSign > 0 && (i - upperBound >= 0.0)) {
        continue;
      }
      xValueE.assignValue(i, calculator);
      theFunValueEnonEvaluated = (j == 0) ? lowerFunctionE : upperFunctionE;
      theFunValueEnonEvaluated.substituteRecursively(xExpression, xValueE);
      if (!calculator.evaluateExpression(calculator, theFunValueEnonEvaluated, theFunValueFinal)) {
        return false;
      }
      double finalResultDouble;
      if (!theFunValueFinal.evaluatesToDouble(&finalResultDouble)) {
        return calculator << "<hr>Failed to evaluate your function at point " << i
        << ", instead " << "I evaluated to " << theFunValueFinal.toString();
      }
      if (j == 0) {
        xValues.addOnTop(i);
        fValuesLower.addOnTop(finalResultDouble);
      } else {
        fValuesUpper.addOnTop(finalResultDouble);
      }
      yMin = MathRoutines::minimum(yMin, finalResultDouble);
      yMax = MathRoutines::maximum(yMax, finalResultDouble);
    }
  }
  std::stringstream outTex, outHtml;
  for (int k = 0; k < 2; k ++) {
    for (int i = 0; i < xValues.size; i ++) {
      bool useNegativePattern = (fValuesLower[i] > fValuesUpper[i]);
      if (k == 0 && useNegativePattern) {
        outTex << "\\psline*[linecolor =\\fcColorNegativeAreaUnderGraph, linewidth = 0.1pt]";
        outHtml << "<br>\\psline*[linecolor =\\fcColorNegativeAreaUnderGraph, linewidth = 0.1pt]";
      }
      if (k == 0 && !useNegativePattern) {
        outTex << "\\psline*[linecolor =\\fcColorAreaUnderGraph, linewidth = 0.1pt]";
        outHtml << "<br>\\psline*[linecolor =\\fcColorAreaUnderGraph, linewidth = 0.1pt]";
      }
      if (k > 0 && useNegativePattern) {
        outTex << "\\psline[linecolor =brown, linewidth = 0.1pt]";
        outHtml << "<br>\\psline[linecolor =brown, linewidth = 0.1pt]";
      }
      if (k > 0 && !useNegativePattern) {
        outTex << "\\psline[linecolor =blue, linewidth = 0.1pt]";
        outHtml << "<br>\\psline[linecolor =blue, linewidth = 0.1pt]";
      }
      outTex << "(" << std::fixed << xValues[i] << ", " << std::fixed << fValuesLower[i] << ")("
      << std::fixed << xValues[i]  << ", "
      << std::fixed << fValuesUpper[i] << ")" << "(" << std::fixed << xValues[i] + theDeltaWithSign
      << ", " << std::fixed
      << fValuesUpper[i] << ")(" << std::fixed << xValues[i] + theDeltaWithSign << ", "
      << std::fixed << fValuesLower[i] << ")"
      << "(" << std::fixed << xValues[i] << ", " << std::fixed << fValuesLower[i] << ")";
      outHtml << "(" << std::fixed << xValues[i] << ", " << std::fixed << fValuesLower[i] << ")("
      << std::fixed << xValues[i] << ", "
      << std::fixed << fValuesUpper[i] << ")" << "(" << std::fixed << xValues[i] + theDeltaWithSign
      << ", " << std::fixed
      << fValuesUpper[i] << ")(" << std::fixed << xValues[i] + theDeltaWithSign << ", " << std::fixed
      << fValuesLower[i] << ")"
      << "(" << std::fixed << xValues[i] << ", " << std::fixed << fValuesLower[i] << ")";
    }
  }
  outHtml << "<br>";
  for (int i = 0; i < rValues.size; i ++) {
    std::stringstream tempStream;
    tempStream << "\\rput[t](" << std::fixed << rValues[i].getDoubleValue() << ",-0.03)" << "{$";
    if (rValues[i].isInteger()) {
      tempStream << rValues[i].toString();
    } else {
      tempStream << "\\frac{" << rValues[i].getNumerator().toString() << "}"
      << "{" << rValues[i].getDenominator().toString() << "}";
    }
    tempStream << "$}";
    outHtml << tempStream.str();
    outTex << tempStream.str();
  }
  outHtml << "<br>";
  PlotObject thePlot;
  thePlot.thePlotString = outTex.str();
  thePlot.thePlotStringWithHtml = outHtml.str();
  thePlot.xLow = lowerBound;
  thePlot.xHigh = upperBound;
  thePlot.yLow = yMin;
  thePlot.yHigh = yMax;

  thePlot.coordinateFunctionsE.addOnTop(input[1]);
  Plot plotFinal;
  plotFinal += thePlot;
  plotFinal += outputPlot;
  return output.assignValue(plotFinal, calculator);
}

bool CalculatorFunctionsPlot::plotPolarRfunctionTheta(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPlot::plotPolarRfunctionTheta");
  if (input.size() < 4) {
    return output.makeError(
      "Drawing polar coordinates takes at least three arguments: "
      "function, lower angle bound and upper angle bound. ",
      calculator
    );
  }
  if (input.size() < 4) {
    return calculator
    << "Parametric curve plots take 3+ arguments. The first argument gives "
    << "the coordinate functions in the format (f_1, f_2) or (f_1, f_2,f_3), "
    << "the next two arguments stands for the variable range.";
  }
  if (input.hasBoundVariables()) {
    return false;
  }
  const Expression& polarE = input[1];
  HashedList<Expression> theVars;
  if (!polarE.getFreeVariables(theVars, true)) {
    return false;
  }
  if (theVars.size > 1) {
    return calculator << "Polar radius must depend on a single variable. ";
  }
  if (theVars.size == 0) {
    theVars.addOnTop(calculator.getNewAtom());
  }
  Expression theSine(calculator), theCosine(calculator);
  theSine.addChildAtomOnTop(calculator.opSin());
  theCosine.addChildAtomOnTop(calculator.opCos());
  theSine.addChildOnTop(theVars[0]);
  theCosine.addChildOnTop(theVars[0]);
  Expression theX = theCosine * polarE;
  Expression theY = theSine * polarE;
  Expression newArg;
  newArg.makeSequence(calculator);
  newArg.addChildOnTop(theX);
  newArg.addChildOnTop(theY);
  output.reset(calculator);
  output.addChildAtomOnTop("PlotCurve");
  output.addChildOnTop(newArg);
  for (int i = 2; i < input.size(); i ++) {
    output.addChildOnTop(input[i]);
  }
  return true;
}

bool CalculatorFunctionsPlot::plotPolarRfunctionThetaExtended(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPlot::plotPolarRfunctionThetaExtended");
  if (input.size() < 4) {
    return output.makeError(
      "Drawing polar coordinates takes three arguments: "
      "function, lower angle bound and upper angle bound. ",
      calculator
    );
  }
  Expression plotXYE, plotRthetaE;
  if (!calculator.callCalculatorFunction(CalculatorFunctionsPlot::plotPolarRfunctionTheta, input, plotXYE)) {
    return false;
  }
  if (!calculator.callCalculatorFunction(CalculatorFunctionsPlot::plot2D, input, plotRthetaE)) {
    return false;
  }
  return output.makeXOX(calculator, calculator.opSequence(), plotXYE, plotRthetaE);
}

bool CalculatorFunctionsPlot::plotParametricCurve(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPlot::plotParametricCurve");
  if (input.size() < 4) {
    return calculator
    << "Parametric curve plots take 3+ arguments. The first argument gives "
    << "the coordinate functions in the format (f_1, f_2) or (f_1, f_2,f_3), "
    << " the next two arguments stands for the variable range. ";
  }
  if (input.hasBoundVariables()) {
    return false;
  }
  PlotObject thePlot;
  if (!input[1].isSequenceNElements()) {
    return calculator
    << "The first argument of parametric curve must be a sequence, instead I got: "
    << input[1].toString();
  }
  thePlot.dimension = input[1].size() - 1;
  for (int i = 0; i < thePlot.dimension; i ++) {
    thePlot.coordinateFunctionsE.addOnTop(input[1][i + 1]);
    thePlot.coordinateFunctionsE[i].getFreeVariables(thePlot.variablesInPlay, true);
  }
  if (thePlot.variablesInPlay.size > 1) {
    return calculator << "Curve is allowed to depend on at most 1 parameter. "
    << "Instead, your curve: " << input.toString()
    << " depends on "
    << thePlot.variablesInPlay.size << ", namely: "
    << thePlot.variablesInPlay.toStringCommaDelimited() << ". ";
  }
  if (thePlot.variablesInPlay.size == 0) {
    Expression tempE;
    tempE.makeAtom("t", calculator);
    thePlot.variablesInPlay.addOnTop(tempE);
  }
  thePlot.variablesInPlayJS.addOnTop(
    HtmlRoutines::getJavascriptVariable(thePlot.variablesInPlay[0].toString())
  );
  thePlot.colorJS = "red";
  thePlot.colorRGB = static_cast<int>(HtmlRoutines::redGreenBlue(255, 0, 0));
  if (input.size() >= 5) {
    if (!input[4].isOfType<std::string>(&thePlot.colorJS)) {
      thePlot.colorJS = input[4].toString();
    }
  }
  DrawingVariables::getColorIntFromColorString(thePlot.colorJS, thePlot.colorRGB);
  thePlot.lineWidth = 1;
  if (input.size() >= 6) {
    if (!input[5].evaluatesToDouble(&thePlot.lineWidth)) {
      thePlot.lineWidth = 1;
    }
  }
  int numPoints = 1000;
  if (input.size() >= 7) {
    if (!input[6].isSmallInteger(&numPoints)) {
      numPoints = 1000;
      calculator << "<hr>Could not extract number of points from "
      << input[6].toString();
    }
    thePlot.numSegmentsE = input[6];
  }
  if (numPoints < 2 || numPoints > 30000) {
    numPoints = 1000;
    calculator << "<hr>Extracted " << numPoints
    << " point but that is not valid. Changing to 1000. ";
  }
  if (input.size() < 7) {
    thePlot.numSegmentsE.assignValue(numPoints, calculator);
  }
  List<Expression> theConvertedExpressions;
  theConvertedExpressions.setSize(thePlot.dimension);
  thePlot.paramLowE = input[2];
  thePlot.paramHighE = input[3];
  if (
    !thePlot.paramLowE.evaluatesToDouble(&thePlot.paramLow) ||
    !thePlot.paramHighE.evaluatesToDouble(&thePlot.paramHigh)
  ) {
    calculator << "Failed to convert "
    << thePlot.paramLowE.toString() << " and "
    << thePlot.paramHighE.toString()
    << " to left and right endpoint of parameter interval. ";
  }
  Vectors<double> theXs, theYs;

  bool isGoodLatexWise = true;
  for (int i = 0; i < thePlot.dimension; i ++) {
    if (!calculator.callCalculatorFunction(
      CalculatorFunctions::innerSuffixNotationForPostScript,
      thePlot.coordinateFunctionsE[i],
      theConvertedExpressions[i]
    )) {
      calculator << "Failed to extract suffix notation from argument "
      << thePlot.coordinateFunctionsE[i].toString();
      isGoodLatexWise = false;
      break;
    }
  }
  if (isGoodLatexWise && thePlot.dimension == 2) {
    std::stringstream outLatex, outHtml;
    outLatex << "\\parametricplot[linecolor =\\fcColorGraph, plotpoints =" << numPoints << "]{"
    << thePlot.paramLow << "}{" << thePlot.paramHigh << "}{"
    << theConvertedExpressions[0].getValue<std::string>()
    << theConvertedExpressions[1].getValue<std::string>() << "}";
    outHtml << "<br>%Calculator input: " << input.toString()
    << "<br>\\parametricplot[linecolor =\\fcColorGraph, plotpoints =" << numPoints << "]{"
    << thePlot.paramLow << "}{" << thePlot.paramHigh << "}{"
    << theConvertedExpressions[0].getValue<std::string>()
    << theConvertedExpressions[1].getValue<std::string>() << "}";
    thePlot.thePlotString= outLatex.str();
    thePlot.thePlotStringWithHtml = outHtml.str();
  }
  Expression converterE;
  thePlot.thePlotType = "parametricCurve";
  thePlot.coordinateFunctionsJS.setSize(thePlot.dimension);
  for (int i = 0; i < thePlot.dimension; i ++) {
    if (CalculatorFunctions::functionMakeJavascriptExpression(
      calculator,
      thePlot.coordinateFunctionsE[i],
      converterE
    )) {
      thePlot.coordinateFunctionsJS[i] = converterE.toString();
    } else {
      thePlot.thePlotType = "parametricCurvePrecomputed";
      calculator << "Failed to convert: "
      << thePlot.coordinateFunctionsE[i] << " to js. ";
    }
  }
  thePlot.numSegmenTsJS.setSize(1);
  thePlot.numSegmenTsJS[0] = "200";
  if (CalculatorFunctions::functionMakeJavascriptExpression(
    calculator, thePlot.numSegmentsE, converterE
  )) {
    thePlot.numSegmenTsJS[0] = converterE.toString();
  } else {
    thePlot.thePlotType = "parametricCurvePrecomputMakeBoxed";
    calculator << "Failed to convert: "
    << thePlot.numSegmentsE << " to js. ";
  }
  if (CalculatorFunctions::functionMakeJavascriptExpression(
    calculator, thePlot.paramLowE, converterE
  )) {
    thePlot.paramLowJS = converterE.toString();
  } else {
    thePlot.thePlotType = "parametricCurvePrecomputed";
    calculator << "Failed to convert: " << thePlot.paramLowE << " to js. ";
  }
  if (CalculatorFunctions::functionMakeJavascriptExpression(
    calculator, thePlot.paramHighE, converterE
  )) {
    thePlot.paramHighJS = converterE.toString();
  } else {
    thePlot.thePlotType = "parametricCurvePrecomputed";
    calculator << "Failed to convert: " << thePlot.paramHighE << " to js. ";
  }
  if (thePlot.dimension == 2) {
    if (!thePlot.coordinateFunctionsE[0].evaluatesToDoubleInRange(
      thePlot.variablesInPlay[0].toString(),
      thePlot.paramLow,
      thePlot.paramHigh,
      numPoints,
      &thePlot.xLow,
      &thePlot.xHigh,
      &theXs
    )) {
      calculator << "<hr>Failed to evaluate curve function. ";
    }
    if (!thePlot.coordinateFunctionsE[1].evaluatesToDoubleInRange(
      thePlot.variablesInPlay[0].toString(),
      thePlot.paramLow,
      thePlot.paramHigh,
      numPoints,
      &thePlot.yLow,
      &thePlot.yHigh,
      &theYs
    )) {
      calculator << "<hr>Failed to evaluate curve function. ";
    }
    thePlot.thePointsDouble.setSize(theXs.size);
    for (int i = 0; i < theXs.size; i ++) {
      thePlot.thePointsDouble[i].setSize(2);
      thePlot.thePointsDouble[i][0] = theXs[i][1];
      thePlot.thePointsDouble[i][1] = theYs[i][1];
    }
  }
  Plot outputPlot;
  outputPlot += thePlot;
  input.hasInputBoxVariables(&outputPlot.boxesThatUpdateMe);
  return output.assignValue(outputPlot, calculator);
}

bool CalculatorFunctions::innerEvaluateToDoublE(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Expression::innerEvaluateToDoublE");
  if (input.size() != 2) {
    // one argument expected.
    return false;
  }
  double theValue = 0;
  if (!input[1].evaluatesToDouble(&theValue)) {
    return false;
  }
  return output.assignValue(theValue, calculator);
}

bool CalculatorFunctions::functionEvaluateToDouble(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Expression::functionEvaluateToDouble");
  double theValue = 0;
  if (!input.evaluatesToDouble(&theValue)) {
    return false;
  }
  return output.assignValue(theValue, calculator);
}

bool CalculatorFunctions::innerTestMathMouseHover(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestMathMouseHover");
  std::stringstream out;
  out << "Hover mouse to render: " << HtmlRoutines::getMathNoDisplay(input.toString());
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerEmbedSemisimpleAlgebraInSemisimpleAlgebra(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerEmbedSemisimpleAlgebraInSemisimpleAlgebra");
  if (input.size() != 3) {
    return output.makeError("I expect two arguments - the two semisimple subalgebras.", calculator);
  }
  const Expression& EsmallSA = input[1];
  const Expression& ElargeSA = input[2];
  WithContext<SemisimpleLieAlgebra*> smallSubalgebraPointer;
  if (!calculator.convert(
    EsmallSA,
    CalculatorConversions::functionSemisimpleLieAlgebra,
    smallSubalgebraPointer
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  WithContext<SemisimpleLieAlgebra*> largeSubalgebraPointer;
  if (!calculator.convert(
    ElargeSA,
    CalculatorConversions::functionSemisimpleLieAlgebra,
    largeSubalgebraPointer
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra& ownerSS = *largeSubalgebraPointer.content;
  std::stringstream out;
  if (ownerSS.getRank() > 8) {
    out << "<b>This code is has been set to run up to ambient Lie algebra of rank 8. </b>";
    return output.assignValue(out.str(), calculator);
  } else {
    out << "<b>This code is completely experimental. Use the following printouts on your own risk</b>";
  }
  SemisimpleSubalgebras& theSSsubalgebras =
  calculator.theObjectContainer.getSemisimpleSubalgebrasCreateIfNotPresent(ownerSS.theWeyl.theDynkinType);
  theSSsubalgebras.ToStringExpressionString = CalculatorConversions::innerStringFromSemisimpleSubalgebras;

  out << "Attempting to embed "
  << smallSubalgebraPointer.content->theWeyl.theDynkinType.toString()
  << " in " << ownerSS.toStringLieAlgebraName();
  theSSsubalgebras.findTheSemisimpleSubalgebrasFromScratch(
    ownerSS,
    calculator.theObjectContainer.theAlgebraicClosure,
    calculator.theObjectContainer.semisimpleLieAlgebras,
    calculator.theObjectContainer.theSltwoSAs,
    &smallSubalgebraPointer.content->theWeyl.theDynkinType
  );
  return output.assignValue(theSSsubalgebras, calculator);
}

bool CalculatorFunctions::innerAllPartitions(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerAllPartitions");
  if (input.size() != 2) {
    return false;
  }
  int theRank = - 1;
  if (!input[1].isSmallInteger(&theRank)) {
    return false;
  }
  if (theRank > 33 || theRank < 0) {
    return calculator
    << "Partitions printouts are limited "
    << "from n = 0 to n = 33, your input was: "
    << input[1].toString();
  }
  List<Partition> thePartitions;
  Partition::GetPartitions(thePartitions, theRank);
  std::stringstream out;
  out << "The partitions of " << theRank << " (total: " << thePartitions.size << ")"
  << ": ";
  for (int i = 0; i < thePartitions.size; i ++) {
    out << "<br>" << thePartitions[i].toString();
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerAllVectorPartitions(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerAllVectorPartitions");
  if (input.size() != 3) {
    return calculator << "<hr>AllVectorPartitions function takes 3 arguments.";
  }
  VectorPartition thePartition;
  const Expression& theVectorE = input[1];
  const Expression& thePartitioningVectorsE = input[2];
  if (!calculator.getVector(theVectorE, thePartition.goalVector)) {
    return calculator << "<hr>Failed to extract vector from " << theVectorE.toString();
  }
  Matrix<Rational> vectorsMatForm;
  if (!calculator.functionGetMatrix(
    thePartitioningVectorsE,
    vectorsMatForm,
    nullptr,
    thePartition.goalVector.size
  )) {
    return calculator << "<hr>Failed to extract list of vectors from "
    << thePartitioningVectorsE.toString();
  }
  Vectors<Rational> theInputVectors;
  theInputVectors.assignMatrixRows(vectorsMatForm);
  for (int i = 0; i < theInputVectors.size; i ++) {
    if (!theInputVectors[i].isPositive()) {
      return calculator << "<hr>Input vector "
      << theInputVectors[i].toString() << " is non-positive";
    }
  }
  if (!thePartition.initialize(theInputVectors, thePartition.goalVector)) {
    return calculator << "<hr>Failed to initialize vector partition object";
  }
  std::stringstream out;
  int numFound = 0;
  ProgressReport theReport;
  out << thePartition.toStringPartitioningVectors();
  while (thePartition.incrementReturnFalseIfPastLast()) {
    out << "<br>" << thePartition.toStringOnePartition(thePartition.currentPartition);
    numFound ++;
    if (numFound % 1000 == 0) {
      std::stringstream reportStream;
      reportStream << "Found " << numFound << " partitions of " << thePartition.goalVector.toString()
      << "<br>Current partition: " << thePartition.currentPartition;
      theReport.report(reportStream.str());
    }
  }
  return output.assignValue(out.str(), calculator);
}

template <class Coefficient>
bool CalculatorFunctions::functionDeterminant(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  Expression::FunctionAddress conversionFunction,
  int maxiumDimension
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::functionDeterminant");
  Matrix<Coefficient> matrix;
  ExpressionContext context;
  if (!calculator.functionGetMatrix<Coefficient>(
    input, matrix, &context, - 1, conversionFunction, &calculator.comments
  )) {
    return false;
  }
  if (matrix.numberOfRows == 0) {
    calculator << "Matrix has zero rows: " << input.toString() << ".";
    return output.makeError(
      "Request to compute determinant of emptry matrix. ", calculator
    );
  }
  if (matrix.numberOfRows != matrix.numberOfColumns) {
    calculator << "Request to compute determinant of the non-square "
    << matrix.numberOfRows << " by "
    << matrix.numberOfColumns << " matrix: " << input.toString();
    return output.makeError(
      "Request to compute determinant of non-square matrix. ", calculator
    );
  }
  if (matrix.numberOfRows > maxiumDimension) {
    return calculator
    << "<hr>Maximum dimension for "
    << "determinant computation of the given type is "
    << maxiumDimension << " by " << maxiumDimension
    << ". Your matrix is " << matrix.numberOfRows
    << " by " << matrix.numberOfColumns << ". "
    << "The restrictions are set around file: "
    << __FILE__ << ", line " << __LINE__ << ". ";
  }
  Coefficient result;
  const Coefficient& sample = matrix(0, 0);
  matrix.computeDeterminantOverwriteMatrix(result, sample.one(), sample.zero());
  return output.assignValueWithContext(result, context, calculator);
}

bool CalculatorFunctions::innerDeterminant(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDeterminant");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (CalculatorFunctions::functionDeterminant<Rational>(
    calculator, argument, output, nullptr, 100
  )) {
    return true;
  }
  if (CalculatorFunctions::functionDeterminant<ElementZmodP>(
    calculator, argument, output, nullptr, 100
  )) {
    return true;
  }
  if (CalculatorFunctions::functionDeterminant<AlgebraicNumber>(
    calculator, argument, output, nullptr, 20
  )) {
    return true;
  }
  if (CalculatorFunctions::functionDeterminant<RationalFunction<Rational> >(
    calculator, argument, output, CalculatorConversions::functionRationalFunction<Rational>, 10
  )) {
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerHighestWeightTransposeAntiAutomorphismBilinearForm(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerHighestWeightTransposeAntiAutomorphismBilinearForm");
  RecursionDepthCounter theRecursionCounter(&calculator.recursionDepth);
  if (!input.isListNElements(4)) {
    return output.makeError("Function expects three arguments.", calculator);
  }
  Expression leftMerged = input[1];
  Expression rightMerged = input[2];
  if (!Expression::mergeContexts(leftMerged, rightMerged)) {
    return calculator << "Could not get elements of universal enveloping algebra from inputs: " << input[1].toString()
    << " and " << input[2].toString();
  }
  ExpressionContext finalContext = rightMerged.getContext();
  int algebraIndex = finalContext.indexAmbientSemisimpleLieAlgebra;
  if (algebraIndex == - 1) {
    return output.makeError("I couldn't extract a Lie algebra to compute hwtaabf.", calculator);
  }
  SemisimpleLieAlgebra& constSSalg = calculator.theObjectContainer.semisimpleLieAlgebras.theValues[algebraIndex];
  const Expression& weightExpression = input[3];
  Vector<RationalFunction<Rational> > weight;
  if (!calculator.getVector<RationalFunction<Rational> >(
    weightExpression,
    weight,
    &finalContext,
    constSSalg.getRank(),
    CalculatorConversions::functionRationalFunction<Rational>
  )) {
    return calculator
    << "<hr>Failed to obtain highest weight from the third argument which is "
    << weightExpression.toString();
  }
  if (
    !leftMerged.setContextAtLeastEqualTo(finalContext, &calculator.comments) ||
    !rightMerged.setContextAtLeastEqualTo(finalContext, &calculator.comments)
  ) {
    return output.makeError(
      "Failed to merge the contexts of the highest "
      "weight and the elements of the Universal enveloping. ",
      calculator
    );
  }
  Expression leftConverted, rightConverted;
  if (!leftMerged.convertInternally<ElementUniversalEnveloping<RationalFunction<Rational> > >(leftConverted)) {
    return false;
  }
  if (!rightMerged.convertInternally<ElementUniversalEnveloping<RationalFunction<Rational> > >(rightConverted)) {
    return false;
  }
  const ElementUniversalEnveloping<RationalFunction<Rational> >& leftUE = leftConverted.getValue<ElementUniversalEnveloping<RationalFunction<Rational> > >();
  const ElementUniversalEnveloping<RationalFunction<Rational> >& rightUE = rightConverted.getValue<ElementUniversalEnveloping<RationalFunction<Rational> > >();
  WeylGroupData& theWeyl = constSSalg.theWeyl;
  Vector<RationalFunction<Rational> > hwDualCoords;
  constSSalg.orderSSalgebraForHWbfComputation();
  hwDualCoords = theWeyl.getDualCoordinatesFromFundamental(weight);
  RationalFunction<Rational> outputRF;
  if (!leftUE.highestWeightTransposeAntiAutomorphismBilinearForm(rightUE, outputRF, &hwDualCoords, 1, 0, &calculator.comments)) {
    return output.makeError("Error: couldn't compute Shapovalov form, see comments.", calculator);
  }
  constSSalg.orderStandardAscending();
  return output.assignValueWithContext(outputRF, finalContext, calculator);
}

bool Expression::evaluatesToDoubleInRange(
  const std::string& varName,
  double lowBound,
  double highBound,
  int numIntervals,
  double* outputYmin,
  double* outputYmax,
  Vectors<double>* outputPoints
) const {
  MacroRegisterFunctionWithName("Expression::evaluatesToDoubleInRange");
  if (numIntervals < 1 || this->owner == nullptr) {
    return false;
  }
  HashedList<Expression> knownEs = this->owner->knownDoubleConstants;
  List<double> knownValues = this->owner->knownDoubleConstantValues;
  Expression theVarNameE;
  theVarNameE.makeAtom(varName, *this->owner);
  if (knownEs.contains(theVarNameE)) {
    return *(this->owner) << "Variable name is an already known constant, variable name is bad.";
  }
  knownEs.addOnTop(theVarNameE);
  knownValues.addOnTop(0);
  int numPoints = numIntervals + 1;
  double delta = (highBound - lowBound) / (numIntervals);
  *knownValues.lastObject() = lowBound;
  double currentValue = 0;
  if (outputPoints != nullptr) {
    outputPoints->setSize(numPoints);
  }
  bool result = true;
  int numFailedEvaluations = 0;
  for (int i = 0; i < numPoints; i ++) {
    if (i == numPoints - 1) {
      *knownValues.lastObject() = highBound; //correcting for floating point errors.
    }
    if (!this->evaluatesToDoubleUnderSubstitutions(knownEs, knownValues, &currentValue)) {
      numFailedEvaluations ++;
      if (numFailedEvaluations < 5) {
        *(this->owner) << "<br>Failed to evaluate " << this->toString() << " at " << varName << "="
        << *knownValues.lastObject() << ". ";
      }
      if (numFailedEvaluations == 5) {
        *(this->owner) << "<br>...";
      }
      result = false;
      if (outputPoints != nullptr) {
        (*outputPoints)[i].setSize(2);
        (*outputPoints)[i][0] = *knownValues.lastObject();
        (*outputPoints)[i][1] = std::nan("");
      }
      *knownValues.lastObject() += delta;
      continue;
    }
    if (outputPoints != nullptr) {
      (*outputPoints)[i].setSize(2);
      (*outputPoints)[i][0] = *knownValues.lastObject();
      (*outputPoints)[i][1] = currentValue;
    }
    *knownValues.lastObject() += delta;
    if (outputYmin != nullptr) {
      if (i == 0) {
        *outputYmin = currentValue;
      } else {
        *outputYmin = MathRoutines::minimum(currentValue, *outputYmin);
      }
    }
    if (outputYmax != nullptr) {
      if (i == 0) {
        *outputYmax = currentValue;
      } else {
        *outputYmax = MathRoutines::maximum(currentValue, *outputYmax);
      }
    }
  }
  if (numFailedEvaluations >= 5) {
    (*this->owner) << "<br>" << numFailedEvaluations << " evaluations total failed. ";
  }
  return result;
}

bool Expression::evaluatesToDouble(double* whichDouble) const {
  return this->evaluatesToDoubleUnderSubstitutions(
    this->owner->knownDoubleConstants, this->owner->knownDoubleConstantValues, whichDouble
  );
}

bool Expression::evaluatesToDoubleUnderSubstitutions(
  const HashedList<Expression>& knownEs,
  const List<double>& valuesKnownEs,
  double* whichDouble
) const {
  MacroRegisterFunctionWithName("Expression::evaluatesToDoubleUnderSubstitutions");
  if (this->owner == nullptr) {
    return false;
  }
  Calculator& calculator = *this->owner;
  if (this->isOfType<double>(whichDouble)) {
    return true;
  }
  if (this->isOfType<Rational>()) {
    if (whichDouble != nullptr) {
      *whichDouble = this->getValue<Rational>().getDoubleValue();
    }
    return true;
  }
  if (this->isOfType<AlgebraicNumber>()) {
    if (this->getValue<AlgebraicNumber>().evaluatesToDouble(whichDouble)) {
      return true;
    }
  }
  RecursionDepthCounter theCounter(&this->owner->recursionDepth);
  if (this->owner->recursionDepth >this->owner->maximumRecursionDepth) {
    return *(this->owner) << "<hr>Recursion depth exceeded while evaluating innerEvaluateToDouble."
    << " This may be a programming error. ";
  }
  if (knownEs.contains(*this)) {
    if (whichDouble != nullptr) {
      *whichDouble = valuesKnownEs[knownEs.getIndex(*this)];
    }
    return true;
  }
  bool isArithmeticOperationTwoArguments =
  this->startsWith(calculator.opTimes(), 3) ||
  this->startsWith(calculator.opPlus(), 3) ||
  this->startsWith(calculator.opMinus(), 3) ||
  this->startsWith(calculator.opThePower(), 3) ||
  this->startsWith(calculator.opDivide(), 3) ||
  this->startsWith(calculator.opSqrt(), 3) ||
  this->startsWith(calculator.opLogBase(), 3);
  if (isArithmeticOperationTwoArguments) {
    double leftD, rightD;
    if (
      !(*this)[1].evaluatesToDoubleUnderSubstitutions(knownEs, valuesKnownEs, &leftD) ||
      !(*this)[2].evaluatesToDoubleUnderSubstitutions(knownEs, valuesKnownEs, &rightD)
    ) {
      return false;
    }
    if ((*this).startsWith(calculator.opTimes(), 3)) {
      if (whichDouble != nullptr) {
        *whichDouble = leftD * rightD;
      }
      return true;
    }
    if ((*this).startsWith(calculator.opPlus(), 3)) {
      if (whichDouble != nullptr) {
        *whichDouble = leftD + rightD;
      }
      return true;
    }
    if ((*this).startsWith(calculator.opMinus(), 3)) {
      if (whichDouble != nullptr) {
        *whichDouble = leftD - rightD;
      }
      return true;
    }
    if ((*this).startsWith(calculator.opLogBase(), 3)) {
      if (leftD <= 0 || rightD <= 0) {
        return false;
      }
      if (leftD - 1 == 0.0) {
        return false;
      }
      if (whichDouble != nullptr) {
        *whichDouble = (FloatingPoint::logFloating(rightD)) / (FloatingPoint::logFloating(leftD));
      }
      return true;
    }
    if ((*this).startsWith(calculator.opThePower(), 3)) {
      bool signChange = false;
      if (leftD < 0) {
        Rational theRat;
        if ((*this)[2].isRational(&theRat)) {
          if (!theRat.getDenominator().isEven()) {
            if (!theRat.getNumerator().isEven()) {
              signChange = true;
            }
            leftD *= - 1;
          }
        }
      }
      double tempDouble = 0;
      if (whichDouble == nullptr) {
        whichDouble = &tempDouble;
      }
      if (leftD == 0.0 && rightD < 0) {
        return false;
      }
      if (leftD == 0.0 && rightD > 0) {
        *whichDouble = 0;
      } else {
        *whichDouble = FloatingPoint::power(leftD, rightD);
      }
      if (signChange) {
        *whichDouble *= - 1;
      }
      return !std::isnan(*whichDouble) && !std::isinf(*whichDouble);
    }
    if ((*this).startsWith(calculator.opSqrt(), 3)) {
      bool signChange = false;
      if (rightD < 0) {
        Rational theRat;
        if ((*this)[1].isRational(&theRat)) {
          if (!theRat.getNumerator().isEven()) {
            if (!theRat.getDenominator().isEven()) {
              signChange = true;
            }
            rightD *= - 1;
          }
        }
      }
      if (leftD == 0.0 && rightD < 0) {
        return false;
      }
      double tempDouble = 0;
      if (whichDouble == nullptr) {
        whichDouble = &tempDouble;
      }
      if (rightD == 0.0 && leftD > 0) {
        *whichDouble = 0;
      } else {
        *whichDouble = FloatingPoint::power(rightD, 1 / leftD);
      }
      if (signChange) {
        *whichDouble *= - 1;
      }
      return !std::isnan(*whichDouble) && !std::isinf(*whichDouble);
    }
    if ((*this).startsWith(calculator.opDivide(), 3)) {
      if (rightD == 0.0) {
        if (whichDouble != nullptr) {
          *whichDouble = std::nan("");
        }
        return false;
      }
      if (whichDouble != nullptr) {
        *whichDouble = leftD / rightD;
      }
      return true;
    }
    global.fatal << "This is a piece of code which should never be reached. " << global.fatal;
  }
  bool isKnownFunctionOneArgument =
  this->startsWith(calculator.opSin(), 2) ||
  this->startsWith(calculator.opCos(), 2) ||
  this->startsWith(calculator.opTan(), 2) ||
  this->startsWith(calculator.opCot(), 2) ||
  this->startsWith(calculator.opCsc(), 2) ||
  this->startsWith(calculator.opSec(), 2) ||
  this->startsWith(calculator.opArcTan(), 2) ||
  this->startsWith(calculator.opArcCos(), 2) ||
  this->startsWith(calculator.opArcSin(), 2) ||
  this->startsWith(calculator.opSqrt(), 2) ||
  this->startsWith(calculator.opLog(), 2) ||
  this->startsWith(calculator.opAbsoluteValue(), 2);

  if (isKnownFunctionOneArgument) {
    double argumentD;
    if (!(*this)[1].evaluatesToDoubleUnderSubstitutions(knownEs, valuesKnownEs, &argumentD)) {
      return false;
    }
    if (this->startsWith(calculator.opSqrt())) {
      if (argumentD < 0) {
        return false;
      }
      if (whichDouble != nullptr) {
        *whichDouble = FloatingPoint::sqrtFloating(argumentD);
      }
    }
    if (this->startsWith(calculator.opAbsoluteValue())) {
      if (whichDouble != nullptr) {
        if (argumentD < 0) {
          *whichDouble = - argumentD;
        } else {
          *whichDouble = argumentD;
        }
      }
    }
    if (this->startsWith(calculator.opArcCos())) {
      if (argumentD > 1 || argumentD < - 1) {
        return false;
      }
      if (whichDouble != nullptr) {
        *whichDouble = FloatingPoint::arccos(argumentD);
      }
    }
    if (this->startsWith(calculator.opArcSin())) {
      if (argumentD > 1 || argumentD < - 1) {
        return false;
      }
      if (whichDouble != nullptr) {
        *whichDouble = FloatingPoint::arcsin(argumentD);
      }
    }
    if (this->startsWith(calculator.opSin())) {
      if (whichDouble != nullptr) {
        *whichDouble = FloatingPoint::sinFloating(argumentD);
      }
    }
    if (this->startsWith(calculator.opCos())) {
      if (whichDouble != nullptr) {
        *whichDouble = FloatingPoint::cosFloating(argumentD);
      }
    }
    if (this->startsWith(calculator.opTan())) {
      if (whichDouble != nullptr) {
        double denominator = FloatingPoint::cosFloating(argumentD);
        if (denominator == 0.0) {
          return false;
        }
        *whichDouble = FloatingPoint::sinFloating(argumentD) / denominator;
      }
    }
    if (this->startsWith(calculator.opCot())) {
      if (whichDouble != nullptr) {
        double denominator = FloatingPoint::sinFloating(argumentD);
        if (denominator == 0.0) {
          return false;
        }
        *whichDouble = FloatingPoint::cosFloating(argumentD) / denominator;
      }
    }
    if (this->startsWith(calculator.opCsc())) {
      if (whichDouble != nullptr) {
        double denominator = FloatingPoint::sinFloating(argumentD);
        if (denominator == 0.0) {
          return false;
        }
        *whichDouble = 1 / denominator;
      }
    }
    if (this->startsWith(calculator.opSec())) {
      if (whichDouble != nullptr) {
        double denominator = FloatingPoint::cosFloating(argumentD);
        if (denominator == 0.0) {
          return false;
        }
        *whichDouble = 1 / denominator;
      }
    }
    if (this->startsWith(calculator.opArcTan())) {
      if (whichDouble != nullptr) {
        *whichDouble = FloatingPoint::arctan(argumentD);
      }
    }
    if (this->startsWith(calculator.opLog())) {
      if (argumentD <= 0) {
        return false;
      }
      if (whichDouble != nullptr) {
        *whichDouble = FloatingPoint::logFloating(argumentD);
      }
    }
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerTestTopCommand(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestTopCommand");
  (void) input;
  (void) calculator;//portable way of avoiding unused parameter warning
  if (!global.userDefaultHasAdminRights()) {
    return calculator << "Test top command available only to logged-in admins. ";
  }
  return output.assignValue(global.toHTMLTopCommandLinuxSystem(), calculator);
}

bool CalculatorFunctions::innerTestIndicator(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestIndicator");
  if (global.theResponse.flagBanProcessMonitorinG) {
    std::stringstream out;
    out << "The server's admins have explicitly banned monitoring. ";
    return output.assignValue(out.str(), calculator);
  }
  if (!global.theResponse.flagReportDesired) {
    std::stringstream out;
    out << "Process monitoring turned off by user. ";
    return output.assignValue(out.str(), calculator);
  }
  if (input.size() < 3) {
    return calculator
    << "Testing indicator requires two arguments: "
    << "number of iterations and size of dummy comment. ";
  }
  int numRuns = - 1;
  if (!input[1].isIntegerFittingInInt(&numRuns)) {
    return calculator << "Argument of CalculatorFunctions::innerTestIndicator "
    << "is not a small integer, instead it equals: " << input[1] << ".";
  }
  if (numRuns > 200000) {
    calculator << "The argument " << numRuns
    << " of CalculatorFunctions::innerTestIndicator larger than 200000, trimming down to 200000.";
    numRuns = 200000;
  }
  if (numRuns < 0) {
    numRuns = 0;
  }
  int dummyCommentSize = 0;
  if (!input[2].isIntegerFittingInInt(&dummyCommentSize)) {
    return calculator << "Second argument needs to be an integer. ";
  }
  if (dummyCommentSize > 200000) {
    calculator << "Dummy comment size of " << dummyCommentSize << " reduced to 200000.";
    dummyCommentSize = 200000;
  }
  if (dummyCommentSize < 0) {
    dummyCommentSize = 0;
  }
  std::string dummyComment;
  dummyComment.resize(static_cast<unsigned>(dummyCommentSize));
  for (unsigned i = 0; i < static_cast<unsigned>(dummyCommentSize); i ++) {
    dummyComment[i] = 'a';
  }
  global.theResponse.initiate("Triggered by test indicator. ");
  ProgressReport theReport;
  for (int i = 0; i < numRuns; i ++) {
    std::stringstream reportStream;
    reportStream << " Running indicator test, " << i + 1 << " out of " << numRuns << ".";
    theReport.report(reportStream.str());
    global.fallAsleep(4000);
  }
  std::stringstream out;
  out << numRuns << " iterations of the indicator test executed. ";
  if (dummyCommentSize > 0) {
    calculator << "Dummy comment:<br>" << dummyComment;
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerCrawlTexFile(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCrawlTexFile");
  if (!global.userDefaultHasAdminRights()) {
    std::stringstream out;
    out << "Command available to logged-in admins only. ";
    return output.assignValue(out.str(), calculator);
  }
  if (!input.isOfType<std::string>()) {
    return calculator << "<hr>Input " << input.toString() << " is not of type string. ";
  }
  LaTeXCrawler theCrawler;
  theCrawler.ownerCalculator = &calculator;
  theCrawler.theFileNameToCrawlRelative = input.getValue<std::string>();
  theCrawler.crawl();
  return output.assignValue(theCrawler.displayResult.str(), calculator);
}

bool CalculatorFunctions::innerBuildFreecalcSlidesOnTopic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerBuildFreecalcSlidesOnTopic");
  if (!global.userDefaultHasAdminRights()) {
    std::stringstream out;
    out << "Command available to logged-in admins only. ";
    return output.assignValue(out.str(), calculator);
  }
  (void) input;
  LaTeXCrawler theCrawler;
  theCrawler.flagBuildSingleSlides = true;
  theCrawler.ownerCalculator = &calculator;
  //theCrawler.topicListToBuild = input.getValue<std::string>();
  std::stringstream out;
  theCrawler.buildTopicList(&out, &out);
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerBuildFreecalcSingleSlides(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerBuildFreecalcSingleSlides");
  if (!global.userDefaultHasAdminRights()) {
    std::stringstream out;
    out << "Command available to logged-in admins only. ";
    return output.assignValue(out.str(), calculator);
  }
  if (!input.isOfType<std::string>()) {
    return calculator << "<hr>Input " << input.toString() << " is not of type string. ";
  }
  LaTeXCrawler theCrawler;
  theCrawler.flagBuildSingleSlides = true;
  theCrawler.ownerCalculator = &calculator;
  theCrawler.theFileNameToCrawlRelative = input.getValue<std::string>();
  std::string startingFolder = FileOperations::getCurrentFolder();
  theCrawler.buildFreecalc();
  global.changeDirectory(startingFolder);
  return output.assignValue(theCrawler.displayResult.str(), calculator);
}

bool CalculatorFunctions::innerBuildFreecalc(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerBuildFreecalc");
  if (!global.userDefaultHasAdminRights()) {
    std::stringstream out;
    out << "Command available to logged-in admins only. ";
    return output.assignValue(out.str(), calculator);
  }
  if (input.size() != 2) {
    return calculator << "<hr>Building freecalc requires exactly one argument. ";
  }
  if (!input[1].isOfType<std::string>()) {
    return calculator << "<hr>Input " << input.toString() << " is not of type string. ";
  }
  LaTeXCrawler theCrawler;
  theCrawler.flagBuildSingleSlides = false;
  theCrawler.ownerCalculator = &calculator;
  theCrawler.theFileNameToCrawlRelative = input[1].getValue<std::string>();
  std::string startingFolder = FileOperations::getCurrentFolder();
  theCrawler.buildFreecalc();
  global.changeDirectory(startingFolder);
  return output.assignValue(theCrawler.displayResult.str(), calculator);
}

bool CalculatorFunctions::innerFindProductDistanceModN(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFindProductDistanceModN");
  if (input.children.size != 3) {
    return calculator << "<hr>Product distance f-n takes as input 2 arguments, modulo and a list of integers";
  }
  const Expression& theModuloE = input[1];
  const Expression& theIntegersE = input[2];
  int theSize;
  if (!theModuloE.isIntegerFittingInInt(&theSize)) {
    return calculator << " <hr> Failed to extract modulus from " << theModuloE.toString();
  }
  if (theSize == 0) {
    return calculator << "<hr>zero modulus not allowed.";
  }
  if (theSize < 0) {
    theSize *= - 1;
  }
  if (theSize > 10000000) {
    return calculator << "<hr>I've been instructed to compute with moduli no larger than 10000000.";
  }
  List<int> theInts, theIntsReduced;
  if (!calculator.getVectorInt(theIntegersE, theInts)) {
    return calculator << "<hr>Failed to extract integer list from " << theIntegersE.toString();
  }
  theIntsReduced.setSize(theInts.size);
  for (int i = 0; i < theInts.size; i ++) {
    if (theInts[i] <= 0) {
      return calculator << "<hr>The integer list " << theInts << " contains non-positive numbers. ";
    }
    theIntsReduced[i] = theInts[i] % theSize;
  }
  List<LargeIntegerUnsigned> theList;
  List<int> theIndexStack;
  theList.initializeFillInObject(theSize, 0);
  theIndexStack.reserve(theSize);
  LargeIntegerUnsigned theMod;
  theMod = static_cast<unsigned>(theSize);
  int numElementsCovered = 0;
  for (int i = 0; i < theInts.size; i ++) {
    if (theList[theIntsReduced[i]] == 0) {
      numElementsCovered ++;
    }
    theList[theIntsReduced[i]] = static_cast<unsigned>(theInts[i]);
    theIndexStack.addOnTop(theIntsReduced[i]);
  }
  LargeIntegerUnsigned currentIndexLarge, currentDistance, maxDistanceGenerated;
  int currentIndex;
  ProgressReport theReport;
  std::stringstream reportstream;
  reportstream << "Finding product distance mod " << theMod.toString() << " w.r.t. elements "
  << theInts;
  int numElementsNotAddedToStack = 0;
  maxDistanceGenerated = 0;
  for (int i = 0; i < theIndexStack.size; i ++) {
    for (int j = 0; j < theIntsReduced.size; j ++) {
      currentIndexLarge = static_cast<unsigned>(theIndexStack[i]);
      currentIndexLarge *= static_cast<unsigned>(theIntsReduced[j]);
      currentIndexLarge %= theMod;
      if (!currentIndexLarge.isIntegerFittingInInt(&currentIndex)) {
        return calculator << "An internal check has failed. "
        << "This shouldn't happen, this is possibly a programming bug.";
      }
      currentDistance = theList[theIndexStack[i]];
      currentDistance += static_cast<unsigned>(theIntsReduced[j]);
      if (theList[currentIndex] > 0) {
        if (theList[currentIndex] < currentDistance) {
          numElementsNotAddedToStack ++;
          if (numElementsNotAddedToStack % 50000 == 0) {
            std::stringstream out;
            out << "While computing product distance, explored " << i + 1 << " out of "
            << theIndexStack.size << " indices. " << numElementsNotAddedToStack
            << " candidates were not added to the stack. "
            << "Number of elements reached: " << numElementsCovered << ". "
            << "Max distance generated while searching: " << maxDistanceGenerated.toString();
            theReport.report(out.str());
          }
          continue;
        }
      }
      if (theList[currentIndex] == 0) {
        numElementsCovered ++;
      }
      theList[currentIndex] = currentDistance;
      if (currentDistance > maxDistanceGenerated) {
        maxDistanceGenerated = currentDistance;
      }
      theIndexStack.addOnTop(currentIndex);
      if (theIndexStack.size % 10000 == 0) {
        std::stringstream out;
        out << "While computing product distance, explored " << i + 1 << " out of "
        << theIndexStack.size << " indices. " << numElementsNotAddedToStack << " candidates were not added to the stack. "
        << "Number of elements reached: " << numElementsCovered << ". "
        << "Max distance generated while searching: " << maxDistanceGenerated.toString();
        theReport.report(out.str());
      }
    }
    if (theIndexStack.size > 1000000000) {
      return calculator << "While computing product distance, exceeded allowed stack size of 1000000000";
    }
  }
  std::stringstream out;
  for (int i = 0; i < theList.size; i ++) {
    out << "<br>" << i << ": " << theList[i].toString();
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerSolveProductSumEquationOverSetModN(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSolveProductSumEquationOverSetModN");
  Expression theModuloE, theIntegersE, theProductE, theSumE;
  if (!CalculatorConversions::innerLoadKey(calculator, input, "theMod", theModuloE)) {
    return calculator << "<hr>Value theMod not found.";
  }
  int theMod;
  if (!theModuloE.isIntegerFittingInInt(&theMod)) {
    return calculator << " <hr> Failed to extract modulus from " << theModuloE.toString();
  }
  if (theMod == 0) {
    return calculator << "<hr>zero modulus not allowed.";
  }
  if (theMod < 0) {
    theMod *= - 1;
  }
  if (theMod > 10000000) {
    return calculator << "<hr>I've been instructed to compute with moduli no larger than 10000000.";
  }
  if (!CalculatorConversions::innerLoadKey(calculator, input, "theSet", theIntegersE)) {
    return calculator << "<hr>Value theSet not found.";
  }
  List<int> theInts;
  if (!calculator.getVectorInt(theIntegersE, theInts)) {
    return calculator << "<hr>Failed to extract integer list from " << theIntegersE.toString();
  }
  for (int i = 0; i < theInts.size; i ++) {
    if (theInts[i] <= 0) {
      return calculator << "<hr>The integer list " << theInts << " contains non-positive numbers.";
    }
  }
  if (!CalculatorConversions::innerLoadKey(calculator, input, "theProduct", theProductE)) {
    return calculator << "<hr>Value theProduct not found. ";
  }
  LargeInteger goalProduct;
  if (!theProductE.isInteger(&goalProduct)) {
    return calculator << "<hr>Failed to extract integer from " << theProductE.toString();
  }
  if (goalProduct.isNegative()) {
    return calculator << "<hr>I am expecting a positive product as input. ";
  }
  if (!CalculatorConversions::innerLoadKey(calculator, input, "theSum", theSumE)) {
    return calculator << "<hr>Value theSum not found. ";
  }
  int theSum = - 1;
  if (!theSumE.isSmallInteger(&theSum)) {
    return calculator << "Failed to extract small integer from " << theSumE.toString();
  }
  VectorPartition thePartition;
  Vectors<Rational> theOneDimVectors;
  theOneDimVectors.setSize(theInts.size);
  for (int i = 0; i < theInts.size; i ++) {
    theOneDimVectors[i].makeZero(1);
    theOneDimVectors[i][0] = theInts[i];
  }
  thePartition.goalVector.makeZero(1);
  thePartition.goalVector[0] = theSum;
  if (!thePartition.initialize(theOneDimVectors, thePartition.goalVector)) {
    return calculator << "Failed to initialize the computation. ";
  }
  LargeIntegerUnsigned theModLarge;
  theModLarge = static_cast<unsigned>(theMod);
  int numTestedSoFar = 0;
  ProgressReport theReport;
  LargeIntegerUnsigned oneUI = 1;
  while (thePartition.incrementReturnFalseIfPastLast()) {
    LargeIntegerUnsigned theProduct = 1;
    for (int i = 0; i < thePartition.currentPartition.size; i ++) {
      LargeIntegerUnsigned theNumber = static_cast<unsigned>(theInts[i]);
      MathRoutines::raiseToPower(theNumber, thePartition.currentPartition[i], oneUI);
      theProduct *= theNumber;
      theProduct %= theModLarge;
    }
    if (theProduct == goalProduct.value) {
      std::stringstream out;
      out << "Found one solution: ";
      for (int i = 0; i < thePartition.currentPartition.size; i ++) {
        if (thePartition.currentPartition[i] > 0) {
          out << theInts[i];
          if (thePartition.currentPartition[i] > 1) {
            out << "^{" << thePartition.currentPartition[i] << "}";
          }
          out << " ";
        }
      }
      return output.assignValue(out.str(), calculator);
    }
    numTestedSoFar ++;
    std::stringstream reportStream;
    reportStream << numTestedSoFar << " tested so far ...";
    theReport.report(reportStream.str());
  }
  return output.assignValue(std::string("Couldn't find solution"), calculator);
}

void Calculator::Test::calculatorTestPrepare() {
  MacroRegisterFunctionWithName("Calculator::Test::calculatorTestPrepare");
  if (this->owner == nullptr) {
    global.fatal << "Non-initialized calculator test. " << global.fatal;
  }
  this->commands.clear();
  for (int i = 0; i < this->owner->numberOfPredefinedAtoms; i ++) {
    MemorySaving<Calculator::OperationHandlers>& currentPointer = this->owner->operations.theValues[i];
    if (currentPointer.isZeroPointer()) {
      continue;
    }
    Calculator::OperationHandlers& current = currentPointer.getElement();
    List<Function>* currentHandler = &current.handlers;
    for (int j = 0; j < 2; j ++) {
      for (int k = 0; k < currentHandler->size; k ++) {
        Function& currentFunction = (*currentHandler)[k];
        if (currentFunction.options.dontTestAutomatically) {
          this->noTestSkips ++;
          continue;
        }
        Calculator::Test::OneTest oneTest;
        oneTest.command = currentFunction.theExample;
        oneTest.atom = this->owner->operations.theKeys[i];
        oneTest.functionAdditionalIdentifier = currentFunction.additionalIdentifier;
        this->commands.setKeyValue(oneTest.command, oneTest);
      }
      currentHandler = &current.compositeHandlers;
    }
  }
}

bool Calculator::Test::calculatorTestRun() {
  MacroRegisterFunctionWithName("Calculator::Test::calculatorTestRun");
  this->debugFlagAtStart = global.getWebInput(WebAPI::request::debugFlag);
  global.setWebInput(WebAPI::request::debugFlag, "false");
  this->calculatorTestPrepare();
  Calculator theTester;
  ProgressReport theReport;
  FormatExpressions theFormat;
  theFormat.flagExpressionIsFinal = true;
  if (this->numberOfTests <= 0) {
    this->numberOfTests = this->commands.size() - this->startIndex;
    if (this->numberOfTests < 0) {
      this->numberOfTests = 0;
    }
  }
  this->lastIndexNotTested = this->startIndex + this->numberOfTests;
  if (this->lastIndexNotTested > this->commands.size()) {
    this->lastIndexNotTested = this->commands.size();
  }
  for (int i = this->startIndex; i < this->lastIndexNotTested; i ++) {
    std::stringstream reportStream;
    Calculator::Test::OneTest& currentTest = this->commands.theValues[i];
    reportStream << "<br>Test progress: testing " << i + 1
    << " out of " << this->commands.size() << ". ";
    reportStream << "<br>Testing expression:<br> " << currentTest.command;
    global << Logger::green << "Automated test: " << i << " out of "
    << this->commands.size() << ", atom: " << currentTest.atom
    << ", command:\n"
    << currentTest.command << Logger::endL;
    theReport.report(reportStream.str());
    theTester.initialize();
    theTester.checkConsistencyAfterInitialization();
    theTester.evaluate(currentTest.command);
    currentTest.actualResult = theTester.programExpression.toString(&theFormat);
    reportStream << "<br>Result: " << theTester.programExpression.toString();
    reportStream << "<br>Done in: " << global.getElapsedSeconds() - this->startTime << " seconds. ";
    theReport.report(reportStream.str());
  }
  global.setWebInput(WebAPI::request::debugFlag, this->debugFlagAtStart);
  return this->processResults();
}

bool CalculatorFunctions::printRuleStack(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::printRuleStack");
  (void) input;//portable way of avoiding unused parameter warning
  return output.assignValue(calculator.ruleStack.toString(), calculator);
}

bool CalculatorFunctions::crash(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::crash");
  (void) input;
  global.fatal << "This is a test of the crashing mechanism. "
  << "A file report must have been written. "
  << "The crash file report is not accessible through the calculator's webserver. "
  << "It can be reached locally in the results/crashes folder of the calculator or, "
  << "if running remotely, through an administrator ssh connection. "
  << global.fatal;
  return output.assignValue(std::string("Crashed succesfully"), calculator);
}

bool CalculatorFunctions::crashByListOutOfBounds(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::crashByListOutOfBounds");
  (void) input;//portable way of avoiding unused parameter warning
  List<int> theList;
  std::vector<int> theVector;
  for (int i = 0; i < 5; i ++) {
    theList.addOnTop(0);
    theVector.push_back(0);
  }
  theList.setSize(0);
  theVector.resize(0);
  theVector[1] = 1;
  theList[1] = 1;
  return output.assignValue(std::string("Crashing: list out of bounds."), calculator);
}

bool CalculatorFunctions::crashByVectorOutOfBounds(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::crashByVectorOutOfBounds");
  (void) input;//portable way of avoiding unused parameter warning
  std::vector<int> theVector;
  theVector[1] = 1;
  return output.assignValue(std::string("Crashing: std::vector out of bounds."), calculator);
}

bool CalculatorFunctions::transpose(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerTranspose");
  if (
    !input.isSequenceNElements() &&
    !input.isMatrix() &&
    !input.startsWithGivenOperation(Calculator::Atoms::transpose)
  ) {
    return false;
  }
  Matrix<Expression> theMat;
  if (input.startsWithGivenOperation(Calculator::Atoms::transpose)) {
    calculator.getMatrixExpressionsFromArguments(input, theMat);
  } else {
    calculator.getMatrixExpressions(input, theMat);
  }
  // The commented code used to be here. I don't remember why I added it, perhaps there was a solid reason?
  // If the code is uncommented, then ((1,2),(3,5))^t will not be transposed according to expectation.
  // If the commented code needs to be restored, please document why.
  // if (input.isSequenceNElements())
  //   if (theMat.numberOfRows !=1)
  //     return false;
  theMat.transpose();
  return output.assignMatrixExpressions(theMat, calculator, true, true);
}

bool CalculatorFunctionsBinaryOps::innerPowerSequenceOrMatrixByT(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerSequenceOrMatrixByT");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  if (!input[1].isSequenceNElements() && !input[1].isMatrix()) {
    return false;
  }
  if (!input[2].isOperationGiven("t")  && !input[2].isOperationGiven("T")) {
    return false;
  }
  return CalculatorFunctions::transpose(calculator, input[1], output);
}

class ExpressionTreeDrawer {
public:
  int maximumRecursionDepth;
  int maximumAllowedWidth;
  int maximumDisplayedNodes;
  int indexInCurrentLayer;
  int indexCurrentChild;
  Rational widthMaxLayer;
  int numLayers;
  int maxNumCharsInString;
  bool flagUseFullTree;
  Expression baseExpression;
  HashedList<std::string, MathRoutines::hashString> displayedExpressionStrings;
  List<bool> displayedStringIsLeaf;
  List<Vector<Rational> > nodePositions;
  List<Vector<double> > nodePositionsDouble;
  List<int> layerSizes;
  List<int> layerFirstIndices;
  List<List<int> > arrows;
  Plot thePlot;
  List<Expression> currentLayer;
  List<Expression> nextLayer;
  List<Expression> currentEchildrenTruncated;
  Calculator* owner;
  Rational charWidth, padding, layerHeight, charHeight;
  ExpressionTreeDrawer() {
    this->maximumRecursionDepth = 10;
    this->maximumAllowedWidth = 10;
    this->maximumDisplayedNodes = 1000;
    this->flagUseFullTree = false;
    this->indexInCurrentLayer = - 1;
    this->indexCurrentChild = - 1;
    this->maxNumCharsInString = 100;
    this->numLayers = 0;
    this->owner = nullptr;
    this->charWidth.assignNumeratorAndDenominator(1, 20);
    this->padding = 1;
    this->layerHeight = 2;
    this->widthMaxLayer = 0;
    this->charHeight.assignNumeratorAndDenominator(1, 5);
  }
  Expression& getCurrentExpression() {
    MacroRegisterFunctionWithName("ExpressionTreeDrawer::getCurrentExpression");
    return this->currentLayer[this->indexInCurrentLayer];
  }
  void computeCurrentChildrenTruncated() {
    MacroRegisterFunctionWithName("ExpressionTreeDrawer::computeCurrentChildrenTruncated");
    this->currentEchildrenTruncated.setSize(0);
    if (!this->flagUseFullTree) {
      if (this->getCurrentExpression().isBuiltInType()) {
        return;
      }
    }
    for (int i = 0; i < this->getCurrentExpression().children.size; i ++) {
      this->currentEchildrenTruncated.addOnTop(this->getCurrentExpression()[i]);
      if (i + 1 + this->indexCurrentChild > this->maximumDisplayedNodes || i > this->maximumAllowedWidth) {
        Expression dotsAtom;
        dotsAtom.makeAtom(std::string("..."), *this->owner);
        this->currentEchildrenTruncated.addOnTop(dotsAtom);
        break;
      }
    }
  }
  bool isLeaf(const Expression& input) {
    if (this->flagUseFullTree) {
      return input.isAtom();
    }
    if (input.isAtom() || input.isBuiltInType()) {
      return true;
    }
    return false;
  }
  std::string getDisplayString(const Expression& input) {
    MacroRegisterFunctionWithName("ExpressionTreeDrawer::getDisplayString");
    std::stringstream out;
    if (this->flagUseFullTree) {
      std::string atomName;
       if (input.isOperation(&atomName)) {
        if (atomName != "...") {
          out << input.data;
        } else {
          out << "...";
        }
      } else {
        out << input.toString();
      }
    } else {
      out << input.toString();
    }
    return out.str();
  }
  void computeCurrentEContributionToNextLayer() {
    MacroRegisterFunctionWithName("ExpressionTreeDrawer::computeCurrentEContributionToNextLayer");
    this->computeCurrentChildrenTruncated();
    this->nextLayer.addListOnTop(this->currentEchildrenTruncated);
    List<int> emptyArrows;
    for (int i = 0; i < this->currentEchildrenTruncated.size; i ++) {
      this->arrows[this->indexCurrentChild].addOnTop(this->displayedExpressionStrings.size);
      this->addStringTruncate(
        this->getDisplayString(this->currentEchildrenTruncated[i]),
        this->isLeaf(this->currentEchildrenTruncated[i])
      );
      this->arrows.addOnTop(emptyArrows);
    }
  }
  void initialize() {
    this->indexInCurrentLayer = 0;
    this->indexCurrentChild = 0;
    this->currentLayer.setSize(1);
    this->layerFirstIndices.setSize(1);
    this->layerFirstIndices[0] = 0;
    this->layerSizes.setSize(1);
    this->layerSizes[0] = 1;
    List<int> emptyArrows;
    this->arrows.addOnTop(emptyArrows);
    this->currentLayer[0] = this->baseExpression;
    this->displayedExpressionStrings.clear();
    this->addStringTruncate(this->baseExpression.toString(), this->isLeaf(this->baseExpression));
    this->computeCurrentEContributionToNextLayer();
    this->displayedExpressionStrings.setExpectedSize(this->maximumDisplayedNodes);
    this->arrows.setExpectedSize(this->maximumDisplayedNodes);
  }
  std::string toString() {
    std::stringstream out;
    out << "<br>Index in current layer: " << this->indexInCurrentLayer;
    out << "<br>Index in displayed strings: " << this->indexCurrentChild;
    out << "<br>Current layer: " << this->currentLayer.toStringCommaDelimited();
    out << "<br>Next layer: " << this->nextLayer.toStringCommaDelimited();
    out << "<br>Displayed strings: " << this->displayedExpressionStrings.toStringCommaDelimited() ;
    out << "<br>Node positions: " << this->nodePositions.toStringCommaDelimited() ;
    out << "<br>Arrows: " << this->arrows.toStringCommaDelimited() ;
    out << "<br>Width max layer: " << this->widthMaxLayer;
    return out.str();
  }
  void addStringTruncate(const std::string& input, bool isLeaf) {
    this->displayedStringIsLeaf.addOnTop(isLeaf);
    if (input.size() <= static_cast<unsigned>(this->maxNumCharsInString)) {
      this->displayedExpressionStrings.addOnTop(input);
      return;
    }
    std::string truncatedInput = input;
    truncatedInput.resize(static_cast<unsigned>(this->maxNumCharsInString) - 3);
    truncatedInput += "...";
    this->displayedExpressionStrings.addOnTop(truncatedInput);
  }
  bool incrementReturnFalseIfPastLast() {
    MacroRegisterFunctionWithName("ExpressionTreeDrawer::incrementReturnFalseIfPastLast");
    this->indexInCurrentLayer ++;
    this->indexCurrentChild ++;
    if (this->indexInCurrentLayer >= this->currentLayer.size) {
      this->indexInCurrentLayer = 0;
      this->currentLayer = this->nextLayer;
      if (this->currentLayer.size == 0) {
        return false;
      }
      this->layerFirstIndices.addOnTop(this->indexCurrentChild);
      this->layerSizes.addOnTop(this->nextLayer.size);
      this->nextLayer.setSize(0);
      this->computeCurrentEContributionToNextLayer();
      return this->currentLayer.size > 0;
    }
    this->computeCurrentEContributionToNextLayer();
    return true;
  }
  Rational getStringWidthTruncated(int theIndex) {
    return this->charWidth *
    MathRoutines::minimum(
      this->maxNumCharsInString,
      static_cast<signed>(this->displayedExpressionStrings[theIndex].size())
    );
  }
  Rational getLayerWidth(int layerIndex) {
    MacroRegisterFunctionWithName("ExpressionTreeDrawer::getLayerWidth");
    Rational result = 0;
    for (
      int i = this->layerFirstIndices[layerIndex];
      i < this->layerFirstIndices[layerIndex] + this->layerSizes[layerIndex];
      i ++
    ) {
      result += this->getStringWidthTruncated(i) + this->padding;
    }
    result -= this->padding;
    if (result > this->widthMaxLayer) {
      this->widthMaxLayer = result;
    }
    return result;
  }
  void computeLayerPositions(int layerIndex) {
    MacroRegisterFunctionWithName("ExpressionTreeDrawer::computeLayerPositions");
    Rational currentX = - this->getLayerWidth(layerIndex) / 2;
    for (
      int i = this->layerFirstIndices[layerIndex];
      i < this->layerFirstIndices[layerIndex] + this->layerSizes[layerIndex];
      i ++
    ) {
      this->nodePositions[i].setSize(2);
      this->nodePositions[i][0] = currentX + this->getStringWidthTruncated(i) / 2;
      this->nodePositions[i][1] = this->layerHeight * layerIndex * (- 1);
      currentX += this->charWidth * static_cast<int>(this->displayedExpressionStrings[i].size()) + this->padding;
    }
  }
  void drawToDrawingVariables() {
    MacroRegisterFunctionWithName("ExpressionTreeDrawer::drawToDrawingVariables");
    this->initialize();
    while (this->incrementReturnFalseIfPastLast()) {
    }
    this->thePlot.dimension = 2;
    this->thePlot.flagIncludeCoordinateSystem = false;
    this->nodePositions.setSize(this->displayedExpressionStrings.size);
    for (int i = 0; i < this->layerFirstIndices.size; i ++) {
      this->computeLayerPositions(i);
    }
    this->nodePositionsDouble.setSize(this->nodePositions.size);
    for (int i = 0; i < this->nodePositionsDouble.size; i ++) {
      this->nodePositionsDouble[i] = MathRoutines::getVectorDouble(this->nodePositions[i]);
    }
    Vector<double> arrowBase, arrowHead;
    for (int i = 0; i < this->displayedExpressionStrings.size; i ++) {
      for (int j = 0; j < this->arrows[i].size; j ++) {
        arrowBase = this->nodePositionsDouble[i];
        arrowHead = this->nodePositionsDouble[this->arrows[i][j]];
        arrowHead[1] += this->charHeight.getDoubleValue() / 2;
        PlotObject theSegment;
        theSegment.thePlotString = "segment";
        theSegment.thePointsDouble.addOnTop(arrowBase);
        theSegment.thePointsDouble.addOnTop(arrowHead);
        theSegment.colorJS = "black";
        this->thePlot += theSegment;
      }
      if (this->displayedExpressionStrings[i] != "") {
        PlotObject theText;
        theText.thePlotType = "label";
        theText.thePointsDouble.addOnTop(this->nodePositionsDouble[i]);
        theText.colorJS =
        this->displayedStringIsLeaf[i] ? "red" : "gray";
        theText.thePlotString =
        HtmlRoutines::clearNewLines
        (HtmlRoutines::backslashQuotes(HtmlRoutines::doubleBackslashes(this->displayedExpressionStrings[i]) ));
        thePlot += theText;
      } else {
        PlotObject thePoint;
        thePoint.thePlotType = "point";
        thePoint.colorJS = "blue";
        thePoint.thePointsDouble.addOnTop(this->nodePositionsDouble[i]);
        this->thePlot += thePoint;
      }
    }
  }
};

bool CalculatorFunctions::innerDrawExpressionGraphWithOptions(
  Calculator& calculator, const Expression& input, Expression& output, bool useFullTree
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDrawExpressionGraph");
  ExpressionTreeDrawer theEdrawer;
  theEdrawer.flagUseFullTree = useFullTree;
  theEdrawer.owner = &calculator;
  theEdrawer.baseExpression = input;
  theEdrawer.drawToDrawingVariables();
  return output.assignValue(theEdrawer.thePlot, calculator);
}

bool CalculatorFunctions::setRandomSeed(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::setRandomSeed");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  LargeInteger seedOriginal;
  if (!argument.isInteger(&seedOriginal)) {
    return calculator << "Failed to extract integer random seed. ";
  }
  std::stringstream out;
  calculator.theObjectContainer.pseudoRandom.setRandomSeedLarge(seedOriginal, &out);
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerAnd(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerAnd");
  if (input.size() != 3) {
    return false;
  }
  if (input[1].isEqualToZero()) {
    return output.assignValue(0, calculator);
  }
  if (input[2].isEqualToZero()) {
    return output.assignValue(0, calculator);
  }
  if (input[1].isEqualToOne() && input[2].isEqualToOne()) {
    return output.assignValue(1, calculator);
  }
  return false;
}

bool CalculatorFunctions::innerOr(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerOr");
  if (input.size() != 3) {
    return false;
  }
  if (input[1].isEqualToOne()) {
    return output.assignValue(1, calculator);
  }
  if (input[2].isEqualToOne()) {
    return output.assignValue(1, calculator);
  }
  if (input[1].isEqualToZero() && input[2].isEqualToZero()) {
    return output.assignValue(0, calculator);
  }
  return false;
}

bool CalculatorFunctions::innerIfStandard(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIfStandard");
  (void) calculator;
  if (input.size() != 4) {
    return false;
  }
  if (input[1].isEqualToOne()) {
    output = input[2];
    return true;
  }
  if (input[1].isEqualToZero()) {
    output = input[3];
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerIfFrozen(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIfFrozen");
  (void) calculator;
  if (input.size() != 4) {
    return false;
  }
  Expression firstArgument;
  if (!calculator.evaluateExpression(calculator, input[1], firstArgument)) {
    return false;
  }
  if (input[1] != firstArgument) {
    output.reset(calculator);
    output.addChildAtomOnTop("if");
    output.addChildOnTop(firstArgument);
    output.addChildOnTop(input[2]);
    output.addChildOnTop(input[3]);
    return true;
  }
  if (firstArgument.isEqualToOne()) {
    output = input[2];
    return true;
  }
  if (firstArgument.isEqualToZero()) {
    output = input[3];
    return true;
  }
  return false;
}

bool CalculatorFunctions::turnRulesOnOff(
  Calculator& calculator,
  const Expression& input,
  Expression& output, bool turnOff
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::turnRulesOnOff");
  List<std::string> rulesToConsider;
  std::string currentRule;
  if (
    !input.startsWith(calculator.opTurnOffRules()) &&
    !input.startsWith(calculator.opTurnOnRules())
  ) {
    if (input.isOfType<std::string>(&currentRule)) {
      rulesToConsider.addOnTop(currentRule);
    } else if (input.isOperation(&currentRule)) {
      rulesToConsider.addOnTop(currentRule);
    } else {
      return calculator << "Could not extract rule to turn off from "
      << input.toString() << ". ";
    }
  } else {
    for (int i = 1; i < input.size(); i ++) {
      if (input[i].isOfType<std::string>(&currentRule)) {
        rulesToConsider.addOnTop(currentRule);
      } else if (input[i].isOperation(&currentRule)) {
        rulesToConsider.addOnTop(currentRule);
      } else {
        return calculator
        << "Could not extract rule to turn off from "
        << input[i].toString() << ". ";
      }
    }
  }
  HashedList<std::string, MathRoutines::hashString> rulesToSwitch;
  rulesToSwitch.reserve(rulesToConsider.size);
  for (int i = 0; i < rulesToConsider.size; i ++) {
    if (!calculator.namedRules.contains(rulesToConsider[i])) {
      return calculator << "Can't find named rule: " << rulesToConsider[i]
      << ". Turn-off rules command failed. "
      ;
    } else {
      rulesToSwitch.addOnTopNoRepetition(rulesToConsider[i]);
    }
  }
  output.reset(calculator, rulesToSwitch.size + 1);
  if (turnOff) {
    output.addChildAtomOnTop(calculator.opRulesOff());
  } else {
    output.addChildAtomOnTop(calculator.opRulesOn());
  }
  Expression currentRuleE;
  for (int i = 0; i < rulesToSwitch.size; i ++) {
    currentRuleE.assignValue(rulesToSwitch[i], calculator);
    output.addChildOnTop(currentRuleE);
  }
  return true;
}

bool CalculatorFunctions::turnOnApproximations(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::turnOnApproximations");
  (void) input;
  calculator.flagNoApproximations = false;
  return output.assignValue(std::string("Approximations have been turned on. "), calculator);
}

bool CalculatorFunctions::turnOffApproximations(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::turnOffApproximations");
  (void) input;
  calculator.flagNoApproximations = true;
  return output.assignValue(std::string("Approximations have been turned off. "), calculator);
}

bool CalculatorFunctions::turnOffRules(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::turnOffRules");
  return CalculatorFunctions::turnRulesOnOff(calculator, input, output, true);
}

bool CalculatorFunctions::turnOnRules(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::turnOnRules");
  return CalculatorFunctions::turnRulesOnOff(calculator, input, output, false);
}

bool CalculatorFunctions::equalityToArithmeticExpression(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::equalityToArithmeticExpression");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  return CalculatorFunctions::functionEqualityToArithmeticExpression(calculator, argument, output);
}

bool CalculatorFunctions::functionEqualityToArithmeticExpression(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::functionEqualityToArithmeticExpression");
  if (!input.startsWith(calculator.opDefine(), 3)) {
    return false;
  }
  return output.makeXOX(calculator, calculator.opMinus(), input[1], input[2]);
}

bool CalculatorFunctions::randomInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::randomInteger");
  Matrix<Expression> theMat;
  if (!calculator.getMatrixExpressionsFromArguments(input, theMat, - 1, 2)) {
    return calculator << "<hr>Failed to extract a Nx2 matrix giving the integer intervals";
  }
  if (theMat.numberOfRows == 0) {
    return calculator << "<hr>Failed to extract a Nx2 matrix giving the integer intervals";
  }
  List<List<int> > theIntervals;
  theIntervals.setSize(theMat.numberOfRows);
  for (int i = 0; i < theMat.numberOfRows; i ++) {
    theIntervals[i].setSize(theMat.numberOfColumns);
    for (int j = 0; j < theMat.numberOfColumns; j ++) {
      if (!theMat(i, j).isIntegerFittingInInt(&theIntervals[i][j])) {
        return calculator << "<hr>Failed to convert "
        << theMat(i, j).toString() << " to an integer. ";
      }
    }
  }
  int accum = 0;
  for (int i = 0; i < theIntervals.size; i ++) {
    int currentContribution = theIntervals[i][1] - theIntervals[i][0];
    if (currentContribution < 0) {
      currentContribution *= - 1;
    }
    accum += currentContribution + 1;
  }
  if (accum == 0) {
    global.fatal << "This shouldn't happen: accum should not be zero at this point. " << global.fatal;
  }
  int generatedRandomInt = static_cast<signed>(
    calculator.theObjectContainer.pseudoRandom.getRandomNonNegativeLessThanMaximumSeed()
  );
  generatedRandomInt %= accum;
  int resultRandomValue = theIntervals[0][0];
  bool found = false;
  accum = 0;
  for (int i = 0; i < theIntervals.size; i ++) {
    int currentContribution = theIntervals[i][1] - theIntervals[i][0];
    int sign = 1;
    if (currentContribution < 0) {
      currentContribution *= - 1;
      sign = - 1;
    }
    currentContribution ++;
    int nextAccum = accum + currentContribution;
    if (accum <= generatedRandomInt && generatedRandomInt < nextAccum) {
      resultRandomValue = theIntervals[i][0] + sign * (generatedRandomInt - accum);
      found = true;
    }
    accum = nextAccum;
  }
  if (!found) {
    return calculator << "<hr>Failed to generate a random number: "
    << "this shouldn't happen - perhaps the requested "
    << "interval(s) were too large. The generated random integer was: "
    << generatedRandomInt << " and the intervals were: " << theIntervals;
  }
  return output.assignValue(resultRandomValue, calculator);
}

bool CalculatorFunctions::selectAtRandom(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::selectAtRandom");
  if (!input.startsWith(calculator.operations.getIndex("SelectAtRandom"))) {
    output = input; // only one item to select from: returning the item
    return true;
  }
  if (input.size() < 2) {
    return false;
  }
  if (input.size() == 2) {
    output = input[1]; // only one item to select from: return that item
    return true;
  }
  int randomIndex = static_cast<signed>(
    (calculator.theObjectContainer.pseudoRandom.getRandomNonNegativeLessThanMaximumSeed()
  )) % (input.size() - 1)  + 1;
  if (randomIndex < 0 || randomIndex > input.size() - 1) {
    randomIndex = input.size() - 1;
  }
  // <-the line above should never be executed
  // if the % operator works as it should,
  // but having an extra check never hurts
  // (may be a life saver if I change the code above).
  output = input[randomIndex];
  return true;
}
