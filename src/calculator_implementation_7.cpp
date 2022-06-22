// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "calculator_inner_functions.h"
#include "calculator_inner_typed_functions.h"
#include "math_general_implementation.h"
#include "math_extra_weyl_algebras_implementation.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_modules_semisimple_lie_algebras_implementation.h"
#include "crypto.h"
#include "math_extra_semisimple_lie_algebras_implementation.h"
#include "web_api.h"
#include "math_extra_latex_routines.h"
#include "source_code_formatter.h"
#include <cmath>
#include "transport_layer_security.h"
#include "string_constants.h"
#include <vector>
#include "math_rational_function_implementation.h"

template < >
List<Polynomial<Rational> >::Comparator* FormatExpressions::getMonomialOrder<
  Polynomial<Rational>
>() {
  return nullptr;
}

bool CalculatorFunctionsCrypto::testLoadPEMCertificates(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::testLoadPEMCertificates");
  if (input.size() != 2) {
    return false;
  }
  std::string binaryString;
  if (!input[1].isOfType(&binaryString)) {
    return false;
  }
  X509Certificate certificate;
  std::stringstream errorStream, resultStream;
  // May not be initialized if unit testing. Safe after the first run.
  ASNObject::namesToObjectIdsNonThreadSafe();
  bool success = certificate.loadFromASNEncoded(binaryString, &errorStream);
  if (!success) {
    resultStream << "Failed to load asn encoded certificate.<br>";
    resultStream << errorStream.str();
  } else {
    resultStream
    << "<br>Loaded certificate successfully.<br>"
    << certificate.toString();
  }
  return output.assignValue(calculator, resultStream.str());
}

bool CalculatorFunctionsCrypto::testTLSDecodeSSLRecord(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::testTLSDecodeSSLRecord");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  TransportLayerSecurityServer testServer;
  std::stringstream commentsOnFailure;
  if (
    !Crypto::convertHexToListUnsignedChar(
      inputString, testServer.lastReaD.incomingBytes, &commentsOnFailure
    )
  ) {
    return calculator << commentsOnFailure.str();
  }
  testServer.initialize();
  bool success = testServer.decodeSSLRecord(&commentsOnFailure);
  std::stringstream out;
  if (!success) {
    out
    << "<b style = 'color:red'>Failed to decode the record.</b> "
    << commentsOnFailure.str();
  }
  out << testServer.lastReaD.toHtml(1);
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsCrypto::testTLSMessageSequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::testTLSMessageSequence");
  Vector<std::string> inputMessages;
  if (!calculator.getVectorFromFunctionArguments(input, inputMessages)) {
    return calculator << "Failed to extract input vector of strings. ";
  }
  if (inputMessages.size < 3) {
    return
    calculator
    << "At least 3 inputs required: "
    << "private key, public key, and client hello message. ";
  }
  std::stringstream commentsOnFailure;
  TransportLayerSecurityServer spoofServer;
  if (
    !spoofServer.initializeAll(
      inputMessages[0], inputMessages[1], &commentsOnFailure
    )
  ) {
    commentsOnFailure << "Unexpected failure while initializing TLS server. ";
    return output.assignValue(calculator, commentsOnFailure.str());
  }
  spoofServer.spoofer.flagDoSpoof = true;
  for (int i = 2; i < inputMessages.size; i ++) {
    spoofServer.spoofer.incomingMessages.addOnTop(SSLRecord());
    spoofServer.spoofer.incomingMessages.lastObject()->incomingBytes =
    inputMessages[i];
  }
  spoofServer.spoofer.currentInputMessageIndex = 0;
  std::stringstream errorStream;
  spoofServer.handShakeIamServer(- 1, &errorStream);
  std::stringstream out;
  std::stringstream spanId;
  spanId << "spanServerSpoofer_" << "_" << global.getElapsedMilliseconds();
  out << "<div id = '" << spanId.str() << "'></div>";
  JSData script;
  script["id"] = spanId.str();
  script["content"] = spoofServer.toJSON();
  out << HtmlRoutines::scriptFromJSON("displayTransportLayerSecurity", script);
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsCrypto::testLoadPEMPrivateKey(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::testLoadPEMPrivateKey");
  if (input.size() != 2) {
    return false;
  }
  std::string privateKeyString;
  if (!input[1].isOfType(&privateKeyString)) {
    return false;
  }
  List<unsigned char> privateKeyBytes;
  privateKeyBytes = privateKeyString;
  std::stringstream errorStream, resultStream;
  PrivateKeyRSA privateKey;
  bool success = privateKey.loadFromASNEncoded(privateKeyBytes, &errorStream);
  if (!privateKey.basicChecks(&resultStream)) {
    resultStream
    << "<b style = 'color:red'>Private key failed basic checks. </b>";
  }
  if (!success) {
    resultStream << "<br>Failed to load asn encoded certificate.<br>";
    resultStream << errorStream.str();
  } else {
    resultStream << "<br>Loaded private key.<br>" << privateKey.toString();
  }
  return output.assignValue(calculator, resultStream.str());
}

bool CalculatorFunctionsCrypto::loadKnownCertificates(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::loadKnownCertificates");
  (void) input;
  std::stringstream out;
  Crypto::loadKnownCertificates(&out, &out);
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsCrypto::sha1OfString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::sha1OfString");
  if (input.size() != 2) {
    return false;
  }
  return
  CalculatorFunctionsCrypto::functionHashString(
    calculator, input[1], output, "SHA1", false
  );
}

bool CalculatorFunctionsCrypto::sha256OfString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::sha256OfString");
  if (input.size() != 2) {
    return false;
  }
  return
  CalculatorFunctionsCrypto::functionHashString(
    calculator, input[1], output, "SHA256", false
  );
}

bool CalculatorFunctionsCrypto::sha512(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::sha512");
  if (input.size() != 2) {
    return false;
  }
  return
  CalculatorFunctionsCrypto::functionHashString(
    calculator, input[1], output, "SHA512", false
  );
}

bool CalculatorFunctionsCrypto::sha256OfStringVerbose(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::sha256OfString");
  if (input.size() != 2) {
    return false;
  }
  return
  CalculatorFunctionsCrypto::functionHashString(
    calculator, input[1], output, "SHA256", true
  );
}

bool CalculatorFunctionsCrypto::sha224OfString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::sha224OfString");
  if (input.size() != 2) {
    return false;
  }
  return
  CalculatorFunctionsCrypto::functionHashString(
    calculator, input[1], output, "SHA224", false
  );
}

bool CalculatorFunctionsCrypto::ripemd160OfString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::ripemd160OfString");
  if (input.size() != 2) {
    return false;
  }
  return
  CalculatorFunctionsCrypto::functionHashString(
    calculator, input[1], output, "RIPEMD160", false
  );
}

bool CalculatorFunctionsCrypto::sha3_256OfString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::sha3_256OfString");
  if (input.size() != 2) {
    return false;
  }
  return
  CalculatorFunctionsCrypto::functionHashString(
    calculator, input[1], output, "SHA3_256", false
  );
}

bool CalculatorFunctionsCrypto::keccak256OfString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::keccak256OfString");
  if (input.size() != 2) {
    return false;
  }
  return
  CalculatorFunctionsCrypto::functionHashString(
    calculator, input[1], output, "KECCAK256", false
  );
}

bool CalculatorFunctionsCrypto::functionHashString(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  const std::string& hashId,
  bool verbose
) {
  STACK_TRACE("CalculatorFunctionsCrypto::functionHashString");
  std::string inputString;
  if (!input.isOfType(&inputString)) {
    return false;
  }
  List<unsigned char> bitStream;
  bitStream = inputString;
  std::stringstream out;
  if (verbose) {
    out << "<br>Input: " << inputString;
    out
    << "<br>Base64 conversion: "
    << Crypto::convertListUnsignedCharsToBase64(bitStream, false);
    out
    << "<br>Base64url conversion: "
    << Crypto::convertListUnsignedCharsToBase64(bitStream, true);
  }
  List<unsigned char> hashUChar;
  List<uint32_t> sha1Uint;
  Crypto::External& externalCrypto = Crypto::externalCrypto();
  if (hashId == "SHA1" || hashId == "SHA256" || hashId == "SHA224") {
    if (hashId == "SHA1") {
      Crypto::computeSha1(inputString, sha1Uint);
    } else if (hashId == "SHA256") {
      Crypto::computeSha256(inputString, sha1Uint);
    } else if (hashId == "SHA224") {
      Crypto::computeSha224(inputString, sha1Uint);
    }
    Crypto::convertListUint32ToListUcharBigendian(sha1Uint, hashUChar);
  } else if (hashId == "SHA3_256") {
    Crypto::computeSha3_256(inputString, hashUChar);
  } else if (hashId == "KECCAK256") {
    Crypto::computeKeccak3_256(inputString, hashUChar);
  } else if (hashId == "SHA512") {
    Crypto::computeSha512(inputString, hashUChar);
  } else if (hashId == "RIPEMD160") {
    if (externalCrypto.computeRIPEMD160 == nullptr) {
      return
      calculator
      << "You are running a non-vanilla version of the calculator "
      << "that is missing ripemd160 support. "
      << "check out the vanilla version of the calculator at github.com.";
    }
    externalCrypto.computeRIPEMD160(inputString, hashUChar);
  }
  if (verbose) {
    std::string sha1base64string;
    std::string sha1base64URLstring;
    sha1base64string =
    Crypto::convertListUnsignedCharsToBase64(hashUChar, true);
    sha1base64URLstring =
    Crypto::convertListUnsignedCharsToBase64(hashUChar, false);
    out << "<br>" << hashId << " in base64: " << sha1base64string;
    out << "<br>" << hashId << " in base64url: " << sha1base64URLstring;
    out << "<br>" << hashId << " hex: ";
    for (int i = 0; i < sha1Uint.size; i ++) {
      out << std::hex << sha1Uint[i];
    }
    out << "<br>bitstream: ";
  }
  for (int i = 0; i < hashUChar.size; i ++) {
    out << hashUChar[i];
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsEncoding::convertBase64ToString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsEncoding::convertBase64ToString");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString, result;
  if (!input[1].isOfType(&inputString)) {
    inputString = input[1].toString();
  }
  if (
    !Crypto::convertBase64ToString(
      inputString, result, &calculator.comments
    )
  ) {
    return false;
  }
  return output.assignValue(calculator, result);
}

bool CalculatorFunctions::nistEllipticCurveOrder(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::nistEllipticCurveGenerator");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return
    calculator
    << "Function NISTEllipticCurveGenerator "
    << "takes as input a string with an EC curve name. "
    << "Available curve names: secp256k1";
  }
  LargeIntegerUnsigned result;
  if (
    !EllipticCurveWeierstrassNormalForm::getOrderNISTCurve(
      inputString, result, &calculator.comments
    )
  ) {
    return false;
  }
  return output.assignValue(calculator, result);
}

template < >
bool ElementEllipticCurve<ElementZmodP>::makeGeneratorNISTCurve(
  const std::string& input, std::stringstream* commentsOnFailure
);
bool CalculatorFunctions::nistEllipticCurveGenerator(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::nistEllipticCurveGenerator");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return
    calculator
    << "Function NISTEllipticCurveGenerator "
    << "takes as input a string with an EC curve name. "
    << "Available curve names: secp256k1";
  }
  ElementEllipticCurve<ElementZmodP> generator;
  if (
    !generator.makeGeneratorNISTCurve(inputString, &calculator.comments)
  ) {
    return false;
  }
  ExpressionContext context(calculator);
  List<std::string> variables;
  variables.addOnTop("x");
  variables.addOnTop("y");
  context.setVariablesFromStrings(variables);
  return output.assignValueWithContext(calculator, generator, context);
}

bool CalculatorFunctions::stringDifference(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::stringDifference");
  if (input.size() != 3) {
    return false;
  }
  std::string left, right;
  if (
    !input[1].isOfType(&left) || !input[2].isOfType(&right)
  ) {
    return false;
  }
  return
  output.assignValue(
    calculator,
    StringRoutines::Differ::differenceHTMLStatic(
      left, right, "left", "right"
    )
  );
}

bool CalculatorFunctions::sliceString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::sliceString");
  if (input.size() < 3) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType<std::string>(&inputString)) {
    return false;
  }
  int leftIndex = 0;
  if (!input[2].isSmallInteger(&leftIndex)) {
    return
    calculator
    << "Failed to convert slice input "
    << input[2]
    << " to small integer. ";
  }
  if (leftIndex < 0) {
    return
    calculator
    << "slice input "
    << input[2]
    << " appears to be negative. ";
  }
  if (leftIndex >= static_cast<signed>(inputString.size())) {
    return output.assignValue(calculator, std::string(""));
  }
  if (input.size() <= 3) {
    return
    output.assignValue(
      calculator,
      inputString.substr(static_cast<unsigned>(leftIndex))
    );
  }
  int rightIndex = 0;
  if (!input[3].isSmallInteger(&rightIndex)) {
    return
    calculator
    << "Failed to convert slice input "
    << input[3]
    << " to small integer. ";
  }
  if (rightIndex < 0) {
    rightIndex += static_cast<signed>(inputString.size());
  }
  int size = rightIndex - leftIndex;
  if (size <= 0) {
    return output.assignValue(calculator, std::string(""));
  }
  return
  output.assignValue(
    calculator,
    inputString.substr(
      static_cast<unsigned>(leftIndex),
      static_cast<unsigned>(size)
    )
  );
}

bool CalculatorFunctionsEncoding::convertIntegerUnsignedToBase58(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsEncoding::convertIntegerUnsignedToBase58");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger integerValue;
  if (!input[1].isInteger(&integerValue)) {
    return false;
  }
  if (integerValue < 0) {
    return
    calculator
    << "Conversion from negative integer to base58 not allowed. ";
  }
  std::string result;
  Crypto::convertLargeIntUnsignedToBase58SignificantDigitsFIRST(
    integerValue.value, result, 0
  );
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsCrypto::appendDoubleSha256Check(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::appendDoubleSha256Check");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  std::string outputString;
  Crypto::appendDoubleSha256Check(inputString, outputString);
  return output.assignValue(calculator, outputString);
}

bool CalculatorFunctionsCrypto::aes_cbc_256_decrypt(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::aes_cbc_256_decrypt");
  if (input.size() != 3) {
    return
    calculator
    << "AES decrypt function expects two arguments: key and plain text. ";
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
  bool result =
  Crypto::externalCrypto().decryptAES_CBC_256_string(
    key, text, cipherText, &comments
  );
  if (!result) {
    return calculator << comments.str();
  }
  return output.assignValue(calculator, cipherText);
}

bool CalculatorFunctionsCrypto::aes_cbc_256_encrypt(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::aes_cbc_256_encrypt");
  if (input.size() != 3) {
    return
    calculator
    << "AES function expects two arguments: key and plain text. ";
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
  bool result =
  Crypto::externalCrypto().encryptAES_CBC_256_string(
    key, text, cipherText, &comments
  );
  if (!result) {
    return calculator << comments.str();
  }
  return output.assignValue(calculator, cipherText);
}

bool CalculatorFunctionsEncoding::convertBase58ToHex(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsEncoding::convertBase58ToHex");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  std::string outputString;
  if (
    !Crypto::convertBase58ToHexSignificantDigitsFirst(
      inputString, outputString, &calculator.comments
    )
  ) {
    return calculator << "Failed to convert " << inputString << " to hex. ";
  }
  return output.assignValue(calculator, outputString);
}

bool CalculatorFunctionsEncoding::convertHexToBase58(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsEncoding::convertHexToBase58");
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
  if (
    !Crypto::convertHexToInteger(
      inputString, outputInteger, numLeadingZeroBytes
    )
  ) {
    return
    calculator
    << "Failed to convert "
    << inputString
    << " from hex to integer. ";
  }
  Crypto::convertLargeIntUnsignedToBase58SignificantDigitsFIRST(
    outputInteger, outputString, numLeadingZeroBytes
  );
  return output.assignValue(calculator, outputString);
}

bool CalculatorFunctionsEncoding::convertStringToHex(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsEncoding::convertStringToHex");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  return
  output.assignValue(
    calculator, Crypto::convertStringToHex(inputString, 0, false)
  );
}

bool CalculatorFunctionsEncoding::convertCharToBase64(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsEncoding::convertCharToBase64");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  List<unsigned char> bitStream;
  bitStream = inputString;
  return
  output.assignValue(
    calculator,
    Crypto::convertListUnsignedCharsToBase64(bitStream, false)
  );
}

bool CalculatorFunctionsEncoding::base64ToCharToBase64Test(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsEncoding::base64ToCharToBase64Test");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  List<unsigned char> bitStream;
  if (
    !Crypto::convertBase64ToBitStream(
      inputString, bitStream, &calculator.comments
    )
  ) {
    return false;
  }
  std::stringstream out;
  std::string convertedBack =
  Crypto::convertListUnsignedCharsToBase64(bitStream, false);
  out
  << "Original string: "
  << inputString
  << "<br>Converted to bitstream and back: "
  << convertedBack;
  if (convertedBack != inputString) {
    out << "<br><b>The input is not the same as the output!</b>";
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctions::urlKeyValuePairsToNormalRecursive(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::urlKeyValuePairsToNormalRecursive");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string urlString;
  if (!argument.isOfType<std::string>(&urlString)) {
    return false;
  }
  return
  output.assignValue(
    calculator,
    HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(urlString)
  );
}

bool CalculatorFunctions::convertElementZmodPToInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::convertElementZmodPToInteger");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  ElementZmodP element;
  if (!argument.isOfType<ElementZmodP>(&element)) {
    return false;
  }
  return output.assignValue(calculator, element.value);
}

bool CalculatorFunctions::urlStringToNormalString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::urlStringToNormalString");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string argumentString;
  if (!argument.isOfType<std::string>(&argumentString)) {
    return false;
  }
  return
  output.assignValue(
    calculator,
    HtmlRoutines::convertURLStringToNormal(argumentString, false)
  );
}

bool CalculatorFunctions::stringToURL(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::stringToURL");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string argumentString;
  if (!argument.isOfType<std::string>(&argumentString)) {
    return false;
  }
  return
  output.assignValue(
    calculator,
    HtmlRoutines::convertStringToURLString(argumentString, false)
  );
}

bool CalculatorFunctions::stringToAtom(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::stringToAtom");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string argumentString;
  if (!argument.isOfType(&argumentString)) {
    return false;
  }
  return output.makeAtom(calculator, argumentString);
}

bool CalculatorFunctions::expressionToString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::expressionToString");
  if (input.size() != 2) {
    return false;
  }
  return output.assignValue(calculator, input[1].toString());
}

bool CalculatorFunctions::quoteToString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::quoteToString");
  std::string operation;
  if (input.size() != 2) {
    calculator
    << "<b>Warning: this shouldn't happen: "
    << "quote operation is applied to an expression "
    << "with wrong number of arguments. "
    << "This may be a bug with the function "
    << "Calculator::parseFillDictionary.</b>";
    return output.assignValue(calculator, input.toString());
  }
  if (input[1].isOperation(&operation)) {
    std::string converted =
    StringRoutines::Conversions::unescapeJavascriptLike(operation);
    return output.assignValue(calculator, converted);
  }
  calculator
  << "<b>Warning: this shouldn't happen: quote operation "
  << "is applied to the non-atomic expression: "
  << input.toString()
  << ". "
  << "This may be a bug with the function "
  << "Calculator::parseFillDictionary. </b>";
  return output.assignValue(calculator, input.toString());
}

bool CalculatorFunctions::fourierTransformElementWeylAlgebra(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::fourierTransformElementWeylAlgebra");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (!argument.isOfType<ElementWeylAlgebra<Rational> >()) {
    return false;
  }
  ElementWeylAlgebra<Rational> element;
  argument.getValue<ElementWeylAlgebra<Rational> >().fourierTransform(element);
  return
  output.assignValueWithContext(
    calculator, element, argument.getContext()
  );
}

bool CalculatorFunctionsBasic::logarithmBase(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBasic::logarithmBase");
  if (!input.startsWith(calculator.opLogBase(), 3)) {
    return false;
  }
  Expression numE, denE;
  numE.makeOX(calculator, calculator.opLog(), input[2]);
  denE.makeOX(calculator, calculator.opLog(), input[1]);
  return output.makeXOX(calculator, calculator.opDivide(), numE, denE);
}

bool CalculatorFunctionsBasic::logarithm(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBasic::logarithm");
  if (input.size() != 2) {
    return false;
  }
  if (input[1].isEqualToZero()) {
    return
    output.assignError(calculator, "Attempting to compute logarithm of zero.");
  }
  if (input[1].isEqualToOne()) {
    return output.assignValue(calculator, 0);
  }
  if (calculator.approximationsBanned()) {
    return false;
  }
  double argument;
  if (!input[1].evaluatesToDouble(&argument)) {
    if (input[1].isOperationGiven(calculator.opE())) {
      return output.assignValue(calculator, Rational(1));
    }
    return false;
  }
  if (argument > 0) {
    if (input[1].isOperationGiven(calculator.opE())) {
      return output.assignValue(calculator, Rational(1));
    }
    return
    output.assignValue(calculator, FloatingPoint::logFloating(argument));
  }
  argument *= - 1;
  Expression iE, ipiE, piE, lnPart;
  iE.makeSqrt(calculator, Rational(- 1), 2);
  piE.makeAtom(calculator, calculator.opPi());
  ipiE.makeXOX(calculator, calculator.opTimes(), piE, iE);
  lnPart.assignValue(calculator, FloatingPoint::logFloating(argument));
  return output.makeXOX(calculator, calculator.opPlus(), lnPart, ipiE);
}

bool CalculatorFunctionsBasic::factorial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBasic::factorial");
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
    return
    calculator
    << "Computing large factorials is disabled "
    << "(can be changed in the source code "
    << "by modifying CalculatorFunctions::factorial).";
  }
  Rational result;
  return output.assignValue(calculator, result.factorial(inputInt));
}

bool CalculatorFunctionsBasic::absoluteValue(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBasic::absoluteValue");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Rational rational;
  if (argument.isRational(&rational)) {
    if (rational < 0) {
      return output.assignValue(calculator, - rational);
    }
    return output.assignValue(calculator, rational);
  }
  double value = 0;
  if (argument.evaluatesToDouble(&value)) {
    if (value < 0) {
      Expression moneE;
      moneE.assignValue(calculator, - 1);
      output = argument;
      output *= moneE;
      return true;
    }
    output = argument;
    return true;
  }
  return false;
}

bool Calculator::getSumProductsExpressions(
  const Expression& inputSum,
  List<List<Expression> >& outputSumMultiplicands
) {
  STACK_TRACE("Calculator::getSumProductsExpressions");
  List<Expression> summands, currentMultiplicands;
  outputSumMultiplicands.setSize(0);
  if (!this->collectOpands(inputSum, this->opPlus(), summands)) {
    return false;
  }
  for (int i = 0; i < summands.size; i ++) {
    this->collectOpands(
      summands[i], this->opTimes(), currentMultiplicands
    );
    outputSumMultiplicands.addOnTop(currentMultiplicands);
  }
  return true;
}

bool CalculatorFunctions::coefficientOf(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::coefficientOf");
  if (input.size() != 3) {
    return false;
  }
  List<List<Expression> > summands;
  List<Expression> currentListMultiplicands, survivingSummands;
  Expression currentMultiplicand;
  if (input[2].startsWith(calculator.opDivide())) {
    Expression coefficientNumerator = input;
    coefficientNumerator.setChild(2, input[2][1]);
    if (
      !CalculatorFunctions::coefficientOf(
        calculator, coefficientNumerator, output
      )
    ) {
      return false;
    }
    return
    output.makeXOX(
      calculator, calculator.opDivide(), output, input[2][2]
    );
  }
  if (!calculator.getSumProductsExpressions(input[2], summands)) {
    return
    calculator
    << "Failed to extract product of expressions from "
    << input[2].toString();
  }
  for (int i = 0; i < summands.size; i ++) {
    bool isGood = false;
    for (int j = 0; j < summands[i].size; j ++) {
      if (summands[i][j] == input[1]) {
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
    for (int j = 0; j < summands[i].size; j ++) {
      if (summands[i][j] != input[1]) {
        currentListMultiplicands.addOnTop(summands[i][j]);
      }
    }
    currentMultiplicand.makeProduct(calculator, currentListMultiplicands);
    survivingSummands.addOnTop(currentMultiplicand);
  }
  return output.makeSum(calculator, survivingSummands);
}

bool CalculatorFunctions::childExpression(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::childExpression");
  (void) calculator;
  if (input.size() != 3) {
    return false;
  }
  int index = 0;
  if (!input[2].isSmallInteger(&index)) {
    return false;
  }
  index --;
  if (index < 0 || index >= input[1].size()) {
    return false;
  }
  output = input[1][index];
  return true;
}

bool CalculatorFunctions::dereferenceInterval(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::dereferenceInterval");
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

bool CalculatorFunctions::dereferenceSequenceOrMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::dereferenceSequenceOrMatrix");
  (void) calculator;
  if (!input.startsWith(calculator.opUnderscore(), 3)) {
    return false;
  }
  if (
    !input[1].startsWith(calculator.opSequence()) && !input[1].isMatrix()
  ) {
    return false;
  }
  int index;
  if (!input[2].isSmallInteger(&index)) {
    return false;
  }
  if (index > 0 && index < input[1].size()) {
    output = input[1][index];
    return true;
  }
  return false;
}

bool CalculatorFunctions::dereferenceSequenceStatements(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::dereferenceSequenceStatements");
  (void) calculator;
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].startsWith(calculator.opCommandSequence())) {
    return false;
  }
  int index;
  if (!input[2].isSmallInteger(&index)) {
    return false;
  }
  if (index > 0 && index < input[1].size()) {
    output = input[1][index];
    return true;
  }
  return false;
}

bool CalculatorFunctions::solveSerreLikeSystem(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool useUpperLimit,
  bool startWithAlgebraicClosure
) {
  STACK_TRACE("Calculator::solveSerreLikeSystem");
  Vector<Polynomial<Rational> > polynomialsRational;
  ExpressionContext context(calculator);
  bool useArguments =
  input.startsWith(
    calculator.getOperations().getIndexNoFail(
      "FindOneSolutionSerreLikePolynomialSystem"
    )
  ) ||
  input.startsWith(
    calculator.getOperations().getIndexNoFail(
      "FindOneSolutionSerreLikePolynomialSystemAlgebraic"
    )
  ) ||
  input.startsWith(
    calculator.getOperations().getIndexNoFail(
      "FindOneSolutionSerreLikePolynomialSystemUpperLimit"
    )
  ) ||
  input.startsWith(
    calculator.getOperations().getIndexNoFail(
      "FindOneSolutionSerreLikePolynomialSystemAlgebraicUpperLimit"
    )
  );
  if (useArguments) {
    if (
      !calculator.getVectorFromFunctionArguments(
        input, polynomialsRational, &context, 0
      )
    ) {
      return
      output.assignError(
        calculator, "Failed to extract list of polynomials. "
      );
    }
  } else {
    if (
      !calculator.getVector(input, polynomialsRational, &context, 0)
    ) {
      return
      output.assignError(
        calculator, "Failed to extract list of polynomials. "
      );
    }
  }
  PolynomialSystem<AlgebraicNumber> system;
  context.getFormat(system.groebner.format);
  List<int> upperLimits = List<int>({201, 1000});
  if (useUpperLimit) {
    for (int i = 0; i < 2; i ++) {
      Rational upperLimitRat;
      if (!polynomialsRational[0].isConstant(&upperLimitRat)) {
        return
        calculator
        << "Failed to extract a constant from the first argument "
        << polynomialsRational[0].toString(&system.format())
        << ". ";
      }
      if (!upperLimitRat.isIntegerFittingInInt(&upperLimits[i])) {
        return
        calculator
        << "Failed to extract a small integer from the first argument "
        << upperLimitRat.toString(&system.groebner.format)
        << ". ";
      }
      polynomialsRational.popIndexShiftDown(0);
    }
  }
  Vector<Polynomial<AlgebraicNumber> > polynomials;
  polynomials = polynomialsRational;
  system.groebner.maximumPolynomialDivisions = upperLimits[0];
  system.groebner.maximumMonomialOperations = upperLimits[1];
  system.groebner.polynomialOrder.monomialOrder =
  MonomialPolynomial::orderDefault();
  system.algebraicClosure = &calculator.objectContainer.algebraicClosure;
  system.flagTryDirectlySolutionOverAlgebraicClosure =
  startWithAlgebraicClosure;
  global.defaultFormat.getElement() = system.groebner.format;
  system.flagUseTheMonomialBranchingOptimization = true;
  system.solveSerreLikeSystem(polynomials);
  std::stringstream out;
  out << "<br>The context vars:<br>" << context.toString();
  out
  << "<br>The polynomials: "
  << polynomials.toString(&system.groebner.format);
  out
  << "<br>Total number of polynomial computations: "
  << system.numberOfSerreSystemComputations;
  if (system.flagSystemProvenToHaveNoSolution) {
    out << "<br>The system does not have a solution. ";
  } else if (system.flagSystemProvenToHaveSolution) {
    out << "<br>System proven to have solution. ";
  }
  if (!system.flagSystemProvenToHaveNoSolution) {
    if (system.flagSystemSolvedOverBaseField) {
      out
      << "<br>One solution follows. "
      << system.toStringSerreLikeSolution();
    } else {
      out
      << "However, I was unable to find such a solution: "
      << "my heuristics are not good enough.";
    }
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsAlgebraic::convertAlgebraicNumberToMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsAlgebraic::convertAlgebraicNumberToMatrix");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  AlgebraicNumber algebraicNumber;
  if (!argument.isOfType(&algebraicNumber)) {
    return
    calculator
    << "Failed to convert "
    << argument.toString()
    << " to algebraic number. ";
  }
  int dimension = algebraicNumber.owner->getDimensionOverRationals();
  MatrixTensor<Rational> numberMatrixTensor;
  algebraicNumber.owner->getMultiplicationBy(
    algebraicNumber, numberMatrixTensor
  );
  Matrix<Rational> result;
  numberMatrixTensor.getMatrix(result, dimension);
  return output.makeMatrix(calculator, result);
}

bool CalculatorFunctionsAlgebraic::printAlgebraicClosureStatus(
  Calculator& calculator, const Expression& input, Expression& output
) {
  (void) input;
  FormatExpressions format;
  format.flagUseHTML = false;
  format.flagUseLatex = true;
  return
  output.assignValue(
    calculator,
    calculator.objectContainer.algebraicClosure.toStringFull(&format)
  );
}

bool CalculatorFunctionsAlgebraic::getAlgebraicNumberFromMinimalPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsAlgebraic::getAlgebraicNumberFromMinimalPolynomial"
  );
  WithContext<Polynomial<AlgebraicNumber> > polynomial;
  if (
    !CalculatorConversions::functionPolynomial(
      calculator, input[1], polynomial, - 1, - 1, false
    )
  ) {
    return
    calculator
    << "<hr>Failed to convert "
    << input.toString()
    << " to polynomial. ";
  }
  if (polynomial.context.numberOfVariables() != 1) {
    return
    calculator
    << "<hr>After conversion, I got the polynomial "
    << polynomial.toStringContentWithFormat()
    << ", which is not in one variable.";
  }
  AlgebraicNumber algebraicNumber;
  std::stringstream commentsOnFailure;
  if (
    !algebraicNumber.constructFromMinimalPolynomial(
      polynomial.content,
      calculator.objectContainer.algebraicClosure,
      &commentsOnFailure
    )
  ) {
    return
    calculator
    << "Failed to construct minimal polynomial. "
    << commentsOnFailure.str();
  }
  return output.assignValue(calculator, algebraicNumber);
}

bool CalculatorFunctions::compositeAPowerBEvaluatedAtC(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::compositeAPowerBEvaluatedAtC");
  if (!input.isListNElements()) {
    return false;
  }
  const Expression& firstE = input[0];
  if (!firstE.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  if (firstE[1].isSequenceNElements()) {
    return false;
  }
  calculator.checkInputNotSameAsOutput(input, output);
  Expression finalBase;
  finalBase.reset(calculator, input.size());
  finalBase.addChildOnTop(input[0][1]);
  for (int i = 1; i < input.size(); i ++) {
    finalBase.addChildOnTop(input[i]);
  }
  return
  output.makeXOX(
    calculator, calculator.opPower(), finalBase, input[0][2]
  );
}

bool CalculatorFunctions::constantFunction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::constantFunction");
  (void) calculator;
  // portable way of avoiding unused parameter warning
  if (!input.isListNElements()) {
    return false;
  }
  if (!input[0].isConstantNumber()) {
    return false;
  }
  output = input[0];
  return true;
}

bool CalculatorFunctions::combineFractionsCommutative(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::combineFractionsCommutative");
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
  output = (leftE[1] * rightE[2] + rightE[1] * leftE[2]) / (
    leftE[2] * rightE[2]
  );
  return true;
}

template <class Coefficient>
void Polynomial<Coefficient>::getPolynomialUnivariateWithPolynomialCoefficients
(int variableIndex, Polynomial<Polynomial<Coefficient> >& output) const {
  Selection variables;
  variables.initialize(this->minimalNumberOfVariables());
  variables.addSelectionAppendNewIndex(variableIndex);
  this->getPolynomialWithPolynomialCoefficient(variables, output);
}

class IntegralRationalFunctionComputation {
public:
  WithContext<RationalFraction<Rational> > rationalFraction;
  Polynomial<Rational> denominator, numerator;
  Polynomial<Rational> quotient, remainder;
  List<Polynomial<Rational> > allFactors;
  Polynomial<AlgebraicNumber> polynomialThatMustVanish;
  Polynomial<AlgebraicNumber> remainderRescaledAlgebraic;
  RationalFraction<Rational> transformedRationalFraction;
  FormatExpressions currentFormat;
  Expression integrationSetE;
  ExpressionContext context;
  Expression inputIntegrand;
  Expression outputIntegralE;
  AlgebraicNumber additionalMultiple;
  Calculator* owner;
  bool allFactorsAreOfDegree2orless;
  bool needPolynomialDivision;
  int numberOfSystemVariables;
  std::stringstream printoutPartialFractionsHtml;
  std::stringstream printoutPartialFractionsLatex;
  std::stringstream printoutIntegration;
  std::string stringPolyIndentityNonSimplifiedLatex;
  std::string stringRationalFunctionLatex;
  std::string stringRationalFunctionPartialFractionLatex;
  std::string stringFinalAnswer;
  std::string stringDenominatorFactors;
  LinearCombination<Polynomial<AlgebraicNumber>, Rational>
  denominatorFactorsAlgebraicWithMultiplicities;
  LinearCombination<Polynomial<Rational>, Rational>
  denominatorFactorsRationalWithMultiplicities;
  List<List<Polynomial<AlgebraicNumber> > > numerators;
  List<Expression> partialFractionSummands;
  List<Expression> integralSummands;
  Expression integralSum;
  bool computePartialFractionDecomposition();
  void prepareFormatExpressions();
  void prepareDenominatorFactors();
  void prepareNumerators();
  void prepareFinalAnswer();
  bool preparePartialFractionExpressionSummands();
  std::string toStringRationalFunctionLatex();
  bool integrateRationalFunction();
  IntegralRationalFunctionComputation(Calculator* inputOwner):
  owner(inputOwner) {}
  bool checkConsistency() const;
};

bool IntegralRationalFunctionComputation::checkConsistency() const {
  if (this->owner == nullptr) {
    global.fatal << "Non-initialized rf computation" << global.fatal;
  }
  return true;
}

bool IntegralRationalFunctionComputation::
preparePartialFractionExpressionSummands() {
  STACK_TRACE(
    "IntegralRationalFunctionComputation::preparePartialFractionExpressionSummands"
  );
  this->checkConsistency();
  Expression
  polyE,
  currentNumerator,
  denominatorExponent,
  currentDenominatorNoPowerMonic,
  currentDen,
  currentPartialFractionNoCoefficient,
  currentPFWithCoeff,
  coeffE;
  this->partialFractionSummands.setSize(0);
  Polynomial<AlgebraicNumber> denominatorRescaled, numeratorRescaled;
  AlgebraicNumber currentCoefficient, numScale;
  List<MonomialPolynomial>::Comparator* monomialOrder =
  &MonomialPolynomial::orderDefault();
  for (int i = 0; i < this->numerators.size; i ++) {
    for (int j = 0; j < this->numerators[i].size; j ++) {
      if (this->numerators[i][j].isEqualToZero()) {
        continue;
      }
      denominatorRescaled =
      this->denominatorFactorsAlgebraicWithMultiplicities[i];
      numeratorRescaled = this->numerators[i][j];
      denominatorRescaled.getIndexLeadingMonomial(
        nullptr, &currentCoefficient, monomialOrder
      );
      currentCoefficient.invert();
      denominatorRescaled *= currentCoefficient;
      MathRoutines::raiseToPower(
        currentCoefficient, j + 1, AlgebraicNumber(1)
      );
      numeratorRescaled.getIndexLeadingMonomial(
        nullptr, &numScale, monomialOrder
      );
      numeratorRescaled /= numScale;
      currentCoefficient *= numScale;
      polyE.assignValueWithContext(
        *this->owner, numeratorRescaled, this->context
      );
      if (
        !CalculatorConversions::functionExpressionFromBuiltInType(
          *this->owner, polyE, currentNumerator
        )
      ) {
        return false;
      }
      polyE.assignValueWithContext(
        *this->owner, denominatorRescaled, this->context
      );
      if (
        !CalculatorConversions::functionExpressionFromBuiltInType(
          *this->owner, polyE, currentDenominatorNoPowerMonic
        )
      ) {
        return false;
      }
      if (j != 0) {
        denominatorExponent.assignValue(*this->owner, j + 1);
        currentDen.makeXOX(
          *this->owner,
          this->owner->opPower(),
          currentDenominatorNoPowerMonic,
          denominatorExponent
        );
      } else {
        currentDen = currentDenominatorNoPowerMonic;
      }
      currentPartialFractionNoCoefficient = currentNumerator;
      currentPartialFractionNoCoefficient /= currentDen;
      coeffE.assignValue(*this->owner, currentCoefficient);
      currentPFWithCoeff = coeffE * currentPartialFractionNoCoefficient;
      currentPFWithCoeff.checkConsistencyRecursively();
      this->partialFractionSummands.addOnTop(currentPFWithCoeff);
    }
  }
  if (!this->quotient.isEqualToZero()) {
    Expression currentPFpolyForm;
    currentPFpolyForm.assignValueWithContext(
      *this->owner, this->quotient, this->context
    );
    if (
      !CalculatorConversions::functionExpressionFromPolynomial<Rational>(
        *this->owner, currentPFpolyForm, currentPFWithCoeff
      )
    ) {
      *this->owner
      << "<br>Something is wrong: failed to convert polynomial "
      << currentPFpolyForm.toString()
      << " to expression. This shouldn't happen. ";
      return false;
    }
    currentPFWithCoeff.checkConsistencyRecursively();
    this->partialFractionSummands.addOnTop(currentPFWithCoeff);
  }
  return true;
}

bool IntegralRationalFunctionComputation::integrateRationalFunction() {
  STACK_TRACE(
    "IntegralRationalFunctionComputation::integrateRationalFunction"
  );
  this->checkConsistency();
  if (!this->computePartialFractionDecomposition()) {
    printoutIntegration
    << "Failed to decompose rational function into partial fractions. "
    << this->printoutPartialFractionsHtml.str();
    return false;
  }
  printoutIntegration << this->printoutPartialFractionsHtml.str();
  Expression
  polyE,
  currentNumerator,
  denExpE,
  currentDenominatorNoPowerMonic,
  currentDen,
  currentIntegrand,
  currentIntegralNoCoeff,
  currentIntegralWithCoeff,
  coeffE;
  this->integralSummands.setSize(0);
  Polynomial<AlgebraicNumber> denRescaled, numRescaled;
  AlgebraicNumber currentCoefficient, numScale;
  List<MonomialPolynomial>::Comparator* monomialOrder =
  &MonomialPolynomial::orderDefault();
  for (int i = 0; i < this->numerators.size; i ++) {
    for (int j = 0; j < this->numerators[i].size; j ++) {
      if (this->numerators[i][j].isEqualToZero()) {
        continue;
      }
      denRescaled = this->denominatorFactorsAlgebraicWithMultiplicities[i];
      numRescaled = this->numerators[i][j];
      currentCoefficient = denRescaled.getLeadingCoefficient(monomialOrder);
      currentCoefficient.invert();
      denRescaled *= currentCoefficient;
      MathRoutines::raiseToPower(
        currentCoefficient, j + 1, AlgebraicNumber(1)
      );
      numScale = numRescaled.getLeadingCoefficient(monomialOrder);
      numRescaled /= numScale;
      currentCoefficient *= numScale;
      polyE.assignValueWithContext(
        *this->owner, numRescaled, this->context
      );
      if (
        !CalculatorConversions::functionExpressionFromBuiltInType(
          *this->owner, polyE, currentNumerator
        )
      ) {
        return false;
      }
      polyE.assignValueWithContext(
        *this->owner, denRescaled, this->context
      );
      if (
        !CalculatorConversions::functionExpressionFromBuiltInType(
          *this->owner, polyE, currentDenominatorNoPowerMonic
        )
      ) {
        return false;
      }
      if (j != 0) {
        denExpE.assignValue(*this->owner, j + 1);
        currentDen.makeXOX(
          *this->owner,
          this->owner->opPower(),
          currentDenominatorNoPowerMonic,
          denExpE
        );
      } else {
        currentDen = currentDenominatorNoPowerMonic;
      }
      currentIntegrand = currentNumerator;
      currentIntegrand /= currentDen;
      currentIntegralNoCoeff.makeIntegral(
        *this->owner,
        this->integrationSetE,
        currentIntegrand,
        this->context.getVariable(0)
      );
      coeffE.assignValue(*this->owner, currentCoefficient);
      currentIntegralWithCoeff = coeffE * currentIntegralNoCoeff;
      currentIntegralWithCoeff.checkConsistencyRecursively();
      this->integralSummands.addOnTop(currentIntegralWithCoeff);
    }
  }
  if (!this->quotient.isEqualToZero()) {
    Expression currentIntegrandPolyForm;
    currentIntegrandPolyForm.assignValueWithContext(
      *this->owner, this->quotient, this->context
    );
    if (
      !CalculatorConversions::functionExpressionFromPolynomial<Rational>(
        *this->owner, currentIntegrandPolyForm, currentIntegrand
      )
    ) {
      *this->owner
      << "<br>Something is wrong: failed to convert polynomial "
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
    this->integralSummands.addOnTop(currentIntegralWithCoeff);
  }
  this->integralSum.makeSum(*this->owner, this->integralSummands);
  this->integralSum.checkConsistencyRecursively();
  return true;
}

void IntegralRationalFunctionComputation::prepareFormatExpressions() {
  STACK_TRACE("IntegralRationalFunctionComputation::prepareFormatExpressions");
  std::stringstream rationalFractionStream, polyStream;
  rationalFractionStream
  << transformedRationalFraction.toString(&this->currentFormat)
  << " = ";
  polyStream
  << remainderRescaledAlgebraic.toString(&this->currentFormat)
  << " = ";
  int variableCounter = 0;
  for (
    int i = 0; i < this->denominatorFactorsAlgebraicWithMultiplicities.size();
    i ++
  ) {
    int tempSize = - 1;
    this->denominatorFactorsAlgebraicWithMultiplicities.coefficients[i].
    isSmallInteger(&tempSize);
    for (
      int k = 0; k < this->denominatorFactorsAlgebraicWithMultiplicities.
      coefficients[i]; k ++
    ) {
      rationalFractionStream << "\\frac{";
      if (
        this->denominatorFactorsAlgebraicWithMultiplicities[i].totalDegree() >
        1
      ) {
        polyStream << "(";
      }
      for (
        int j = 0; j < this->denominatorFactorsAlgebraicWithMultiplicities[i].
        totalDegree(); j ++
      ) {
        variableCounter ++;
        std::stringstream varNameStream;
        varNameStream << "A_{" << variableCounter << "} ";
        rationalFractionStream << varNameStream.str();
        polyStream << varNameStream.str();
        this->currentFormat.polynomialAlphabet.addOnTop(
          varNameStream.str()
        );
        if (j > 0) {
          rationalFractionStream << "x";
          polyStream << "x";
        }
        if (j > 1) {
          rationalFractionStream << "^{" << j << "}";
          polyStream << "^{" << j << "}";
        }
        if ((
            this->denominatorFactorsAlgebraicWithMultiplicities[i].totalDegree(
            ) -
            1
          ) !=
          j
        ) {
          rationalFractionStream << " + ";
          polyStream << " + ";
        }
      }
      if (
        this->denominatorFactorsAlgebraicWithMultiplicities[i].totalDegree() >
        1
      ) {
        polyStream << ")";
      }
      for (
        int j = 0; j < this->denominatorFactorsAlgebraicWithMultiplicities.size
        (); j ++
      ) {
        Rational exponent =
        this->denominatorFactorsAlgebraicWithMultiplicities.coefficients[j];
        if (j == i) {
          exponent -= k + 1;
        }
        if (exponent == 0) {
          continue;
        }
        polyStream
        << "("
        << this->denominatorFactorsAlgebraicWithMultiplicities[j].toString(
          &this->currentFormat
        )
        << ")";
        if (exponent > 1) {
          polyStream << "^{" << exponent << "}";
        }
      }
      rationalFractionStream << "}{";
      if (k > 0) {
        rationalFractionStream << "(";
      }
      rationalFractionStream
      << this->denominatorFactorsAlgebraicWithMultiplicities[i].toString(
        &this->currentFormat
      );
      if (k > 0) {
        rationalFractionStream << ")^{" << k + 1 << "}";
      }
      rationalFractionStream << "}";
      if (((
            this->denominatorFactorsAlgebraicWithMultiplicities.coefficients[i]
            -
            1
          ) !=
          k
        ) || (
          i != this->denominatorFactorsAlgebraicWithMultiplicities.size() - 1
        )
      ) {
        rationalFractionStream << "+";
        polyStream << "+";
      }
    }
  }
  this->stringRationalFunctionLatex = rationalFractionStream.str();
  this->stringPolyIndentityNonSimplifiedLatex = polyStream.str();
}

void IntegralRationalFunctionComputation::prepareNumerators() {
  STACK_TRACE("IntegralRationalFunctionComputation::prepareNumerators");
  this->transformedRationalFraction = this->remainder;
  this->transformedRationalFraction /= this->denominator;
  this->remainderRescaledAlgebraic = this->remainder;
  this->remainderRescaledAlgebraic /= additionalMultiple;
  this->numberOfSystemVariables = 0;
  Polynomial<AlgebraicNumber> currentSummand;
  MonomialPolynomial currentMon;
  this->polynomialThatMustVanish.makeZero();
  this->polynomialThatMustVanish -= remainderRescaledAlgebraic;
  this->numerators.setSize(
    this->denominatorFactorsAlgebraicWithMultiplicities.size()
  );
  for (
    int i = 0; i < this->denominatorFactorsAlgebraicWithMultiplicities.size();
    i ++
  ) {
    int tempSize = - 1;
    this->denominatorFactorsAlgebraicWithMultiplicities.coefficients[i].
    isSmallInteger(&tempSize);
    this->numerators[i].setSize(tempSize);
    for (
      int k = 0; k < this->denominatorFactorsAlgebraicWithMultiplicities.
      coefficients[i]; k ++
    ) {
      currentSummand.makeZero();
      this->numerators[i][k].makeZero();
      for (
        int j = 0; j < this->denominatorFactorsAlgebraicWithMultiplicities[i].
        totalDegree(); j ++
      ) {
        this->numberOfSystemVariables ++;
        currentMon.makeEi(this->numberOfSystemVariables);
        currentMon.setVariable(0, j);
        this->numerators[i][k].addMonomial(currentMon, 1);
        currentSummand.addMonomial(currentMon, 1);
      }
      for (
        int j = 0; j < this->denominatorFactorsAlgebraicWithMultiplicities.size
        (); j ++
      ) {
        Rational exponent =
        this->denominatorFactorsAlgebraicWithMultiplicities.coefficients[j];
        if (j == i) {
          exponent -= k + 1;
        }
        if (exponent == 0) {
          continue;
        }
        for (int p = 0; p < exponent; p ++) {
          currentSummand *=
          this->denominatorFactorsAlgebraicWithMultiplicities[j];
        }
      }
      this->polynomialThatMustVanish += currentSummand;
    }
  }
}

void IntegralRationalFunctionComputation::prepareFinalAnswer() {
  STACK_TRACE("IntegralRationalFunctionComputation::prepareFinalAnswer");
  std::stringstream rationalFractionStream, answerFinalStream;
  for (
    int i = 0; i < this->denominatorFactorsAlgebraicWithMultiplicities.size();
    i ++
  ) {
    for (
      int k = 0; k < this->denominatorFactorsAlgebraicWithMultiplicities.
      coefficients[i]; k ++
    ) {
      rationalFractionStream
      << "\\frac{"
      << this->numerators[i][k].toString(&this->currentFormat)
      << "}";
      rationalFractionStream << "{";
      rationalFractionStream
      << "("
      << this->denominatorFactorsAlgebraicWithMultiplicities[i].toString(
        &this->currentFormat
      )
      << ")";
      if (k > 0) {
        rationalFractionStream << "^{" << k + 1 << "}";
      }
      rationalFractionStream << "}";
      if (((
            this->denominatorFactorsAlgebraicWithMultiplicities.coefficients[i]
            -
            1
          ) !=
          k
        ) || (
          i != this->denominatorFactorsAlgebraicWithMultiplicities.size() - 1
        )
      ) {
        rationalFractionStream << "+";
      }
    }
  }
  this->stringRationalFunctionPartialFractionLatex =
  rationalFractionStream.str();
  answerFinalStream
  << this->rationalFraction.content.toString(&this->currentFormat)
  << "=";
  if (!this->quotient.isEqualToZero()) {
    answerFinalStream << this->quotient.toString(&this->currentFormat) << "+ ";
    answerFinalStream
    << this->transformedRationalFraction.toString(&this->currentFormat)
    << "=";
  }
  if (!this->quotient.isEqualToZero()) {
    answerFinalStream << this->quotient.toString(&this->currentFormat) << "+ ";
  }
  answerFinalStream << this->stringRationalFunctionPartialFractionLatex;
  this->stringFinalAnswer = answerFinalStream.str();
}

void IntegralRationalFunctionComputation::prepareDenominatorFactors() {
  STACK_TRACE(
    "IntegralRationalFunctionComputation::prepareDenominatorFactors"
  );
  this->printoutPartialFractionsHtml
  << "The rational function is: "
  << HtmlRoutines::getMathNoDisplay(
    "\\frac{" +
    this->numerator.toString(&this->currentFormat) +
    "}{" +
    this->denominator.toString(&this->currentFormat) +
    "}"
  )
  << ".";
  this->printoutPartialFractionsHtml << "<br>The denominator factors are: ";
  this->printoutPartialFractionsLatex
  << "We aim to decompose into partial fractions the following function "
  << "(the denominator has been factored). \\[\\frac{"
  << this->numerator.toString(&this->currentFormat)
  << "}{"
  << this->denominator.toString(&this->currentFormat)
  << "}="
  << "\\frac{"
  << this->numerator.toString(&this->currentFormat)
  << "}{ ";
  this->allFactorsAreOfDegree2orless = true;
  for (int i = 0; i < this->allFactors.size; i ++) {
    this->printoutPartialFractionsHtml
    << HtmlRoutines::getMathNoDisplay(
      this->allFactors[i].toString(&this->currentFormat)
    );
    bool needsParenthesis =
    this->allFactors[i].needsParenthesisForMultiplication();
    if (needsParenthesis) {
      this->printoutPartialFractionsLatex << "\\left(";
    }
    this->printoutPartialFractionsLatex
    << this->allFactors[i].toString(&this->currentFormat);
    if (needsParenthesis) {
      this->printoutPartialFractionsLatex << "\\right)";
    }
    if (i != this->allFactors.size - 1) {
      this->printoutPartialFractionsHtml << ", ";
    }
    if (this->allFactors[i].totalDegree() > 2) {
      allFactorsAreOfDegree2orless = false;
    }
  }
  this->printoutPartialFractionsLatex << "}";
  this->printoutPartialFractionsLatex << "\\]";
  this->printoutPartialFractionsHtml << ". <br>";
}

bool IntegralRationalFunctionComputation::computePartialFractionDecomposition()
{
  STACK_TRACE(
    "IntegralRationalFunctionComputation::computePartialFractionDecomposition"
  );
  this->checkConsistency();
  this->context = this->rationalFraction.context;
  this->context.getFormat(this->currentFormat);
  if (
    this->rationalFraction.content.minimalNumberOfVariables() < 1 ||
    this->rationalFraction.content.expressionType ==
    RationalFraction<Rational>::TypeExpression::typeConstant ||
    this->rationalFraction.content.expressionType ==
    RationalFraction<Rational>::TypeExpression::typePolynomial
  ) {
    this->printoutPartialFractionsHtml
    << this->rationalFraction.content.toString(&this->currentFormat)
    << " is already split into partial fractions. ";
    return true;
  }
  this->rationalFraction.content.getDenominator(this->denominator);
  this->rationalFraction.content.getNumerator(this->numerator);
  this->numerator *=
  this->denominator.scaleNormalizeLeadingMonomial(
    &MonomialPolynomial::orderDefault()
  );
  PolynomialFactorizationUnivariate<Rational> factorization;
  PolynomialFactorizationKronecker algorithm;
  if (
    !factorization.factor(
      this->denominator,
      algorithm,
      &this->printoutPartialFractionsHtml,
      &this->printoutPartialFractionsHtml
    )
  ) {
    this->printoutPartialFractionsHtml
    << "<hr>Failed to factor the denominator "
    << "of the rational function, I surrender. ";
    return false;
  }
  this->numerator /= factorization.constantFactor;
  this->denominator /= factorization.constantFactor;
  this->allFactors = factorization.reduced;
  Polynomial<Rational> denominatorProduct;
  denominatorProduct.makeConstant(1);
  for (int i = 0; i < this->allFactors.size; i ++) {
    denominatorProduct *= this->allFactors[i];
  }
  if (denominatorProduct != this->denominator) {
    global.fatal
    << "Something is very wrong: product of denominator factors is "
    << denominatorProduct.toString(&this->currentFormat)
    << ", but the denominator equals: "
    << this->denominator.toString(&this->currentFormat)
    << ". "
    << global.fatal;
  }
  this->printoutPartialFractionsLatex
  << "\\documentclass{article}"
  << "\\usepackage{longtable}"
  << "\\usepackage{xcolor}"
  << "\\usepackage{multicol} "
  << "\\begin{document}";
  this->prepareDenominatorFactors();
  if (!allFactorsAreOfDegree2orless) {
    this->printoutPartialFractionsHtml
    << "There were factors (over the rationals) of "
    << "degree greater than 2. I surrender. ";
    return false;
  }
  List<MonomialPolynomial>::Comparator monomialOrder =
  MonomialPolynomial::orderDefault();
  this->currentFormat.flagUseFrac = true;
  this->numerator.divideBy(
    this->denominator,
    this->quotient,
    this->remainder,
    &monomialOrder
  );
  this->needPolynomialDivision = !this->quotient.isEqualToZero();
  if (this->needPolynomialDivision) {
    this->printoutPartialFractionsHtml
    << "<br>The numerator "
    << HtmlRoutines::getMathNoDisplay(
      this->numerator.toString(&this->currentFormat)
    )
    << " divided by the denominator "
    << HtmlRoutines::getMathNoDisplay(
      denominator.toString(&this->currentFormat)
    )
    << " yields "
    << HtmlRoutines::getMathNoDisplay(
      this->quotient.toString(&this->currentFormat)
    )
    << " with remainder "
    << HtmlRoutines::getMathNoDisplay(
      this->remainder.toString(&this->currentFormat)
    )
    << ". ";
    GroebnerBasisComputation<Rational> computation;
    computation.flagDoLogDivision = true;
    computation.flagStoreQuotients = true;
    computation.polynomialOrder.monomialOrder =
    MonomialPolynomial::orderDefault();
    computation.addBasisElementNoReduction(this->denominator);
    computation.format = this->currentFormat;
    computation.polynomialOrder.monomialOrder = monomialOrder;
    Polynomial<Rational> numeratorCopy = this->numerator;
    computation.remainderDivisionByBasis(
      numeratorCopy, computation.remainderDivision, - 1
    );
    this->printoutPartialFractionsLatex
    << "Here is a detailed long polynomial division. ";
    this->printoutPartialFractionsLatex
    << computation.divisionReport.getElement().getDivisionStringLaTeX();
    this->printoutPartialFractionsHtml
    << "<br>Here is a detailed long polynomial division:<br> ";
    this->printoutPartialFractionsHtml
    << computation.divisionReport.getElement().getDivisionStringHtml();
  }
  this->denominatorFactorsRationalWithMultiplicities.makeZero();
  for (int i = 0; i < this->allFactors.size; i ++) {
    this->denominatorFactorsRationalWithMultiplicities.addMonomial(
      this->allFactors[i], 1
    );
  }
  this->denominatorFactorsRationalWithMultiplicities.quickSortAscending();
  Polynomial<Rational> currentSecondDegreePolynomial;
  this->denominatorFactorsAlgebraicWithMultiplicities.makeZero();
  Polynomial<AlgebraicNumber> currentLinPoly, currentSecondDegreePolyAlgebraic;
  this->additionalMultiple = 1;
  for (
    int i = 0; i < this->denominatorFactorsRationalWithMultiplicities.size(); i
    ++
  ) {
    currentSecondDegreePolynomial =
    this->denominatorFactorsRationalWithMultiplicities[i];
    currentSecondDegreePolyAlgebraic = currentSecondDegreePolynomial;
    if (currentSecondDegreePolynomial.totalDegree() != 2) {
      this->denominatorFactorsAlgebraicWithMultiplicities.addMonomial(
        currentSecondDegreePolyAlgebraic,
        this->denominatorFactorsRationalWithMultiplicities.coefficients[i]
      );
      continue;
    }
    Rational discriminant = currentSecondDegreePolynomial.getDiscriminant();
    if (discriminant < 0) {
      this->denominatorFactorsAlgebraicWithMultiplicities.addMonomial(
        currentSecondDegreePolyAlgebraic,
        this->denominatorFactorsRationalWithMultiplicities.coefficients[i]
      );
      continue;
    }
    AlgebraicNumber discriminantSqrt;
    if (
      !discriminantSqrt.assignRationalQuadraticRadical(
        discriminant,
        this->owner->objectContainer.algebraicClosure,
        &this->printoutPartialFractionsHtml
      )
    ) {
      this->printoutPartialFractionsHtml
      << "Failed to take radical of "
      << discriminant.toString()
      << " (radical too large?).";
      return false;
    }
    discriminantSqrt.checkConsistency();
    AlgebraicNumber a =
    currentSecondDegreePolynomial.getCoefficientOf(
      MonomialPolynomial(0, 2)
    );
    AlgebraicNumber b =
    currentSecondDegreePolynomial.getCoefficientOf(
      MonomialPolynomial(0, 1)
    );
    a.checkConsistency();
    b.checkConsistency();
    currentLinPoly.makeMonomial(0, 1);
    currentLinPoly -= (- b + discriminantSqrt) / (a* 2);
    Rational currentCoefficient =
    this->denominatorFactorsRationalWithMultiplicities.coefficients[i];
    this->denominatorFactorsAlgebraicWithMultiplicities.addMonomial(
      currentLinPoly, currentCoefficient
    );
    currentLinPoly.makeMonomial(0, 1);
    currentLinPoly -= (- b - discriminantSqrt) / (a* 2);
    this->denominatorFactorsAlgebraicWithMultiplicities.addMonomial(
      currentLinPoly, currentCoefficient
    );
    this->additionalMultiple *= a;
  }
  this->denominatorFactorsAlgebraicWithMultiplicities.quickSortAscending();
  this->printoutPartialFractionsHtml
  << "<br><br>I need to find "
  << HtmlRoutines::getMathNoDisplay("A_i")
  << "'s so that I have the equality of rational functions: ";
  this->printoutPartialFractionsLatex
  << "We need to find $A_i$'s so that "
  << "we have the following equality of rational functions. ";
  this->prepareNumerators();
  this->prepareFormatExpressions();
  this->printoutPartialFractionsHtml
  << HtmlRoutines::getMathNoDisplay(this->stringRationalFunctionLatex, - 1);
  this->printoutPartialFractionsHtml
  << "<br><br>After clearing denominators, we get the equality: ";
  this->printoutPartialFractionsLatex
  << "After clearing denominators, we get the following equality. ";
  this->printoutPartialFractionsHtml
  << "<br><br>"
  << HtmlRoutines::getMathNoDisplay(
    this->stringPolyIndentityNonSimplifiedLatex, - 1
  );
  this->printoutPartialFractionsLatex
  << "\\["
  << this->stringPolyIndentityNonSimplifiedLatex
  << "\\]";
  Polynomial<Polynomial<AlgebraicNumber> > univariateThatMustVanish;
  polynomialThatMustVanish.getPolynomialUnivariateWithPolynomialCoefficients(
    0, univariateThatMustVanish
  );
  this->printoutPartialFractionsHtml
  << "<br><br>After rearranging we get "
  << "that the following polynomial must vanish: "
  << HtmlRoutines::getMathNoDisplay(
    univariateThatMustVanish.toString(&this->currentFormat)
  );
  this->printoutPartialFractionsLatex
  << "After rearranging we get that the "
  << "following polynomial must vanish. Here, by ``vanish'' "
  << "we mean that the coefficients of "
  << "the powers of $x$ must be equal to zero."
  << "\\["
  << univariateThatMustVanish.toString(&this->currentFormat)
  << "\\]";
  this->printoutPartialFractionsHtml
  << "<br>Here, by ``vanish'', we mean that "
  << "the coefficients in front of the powers of x must vanish.";
  Matrix<AlgebraicNumber>
  homogenousSystem,
  systemHomogeneousForLaTeX,
  constantTerms,
  constantTermsForLaTeX;
  Polynomial<AlgebraicNumber>::getLinearSystemFromLinearPolynomials(
    univariateThatMustVanish.coefficients, homogenousSystem, constantTerms
  );
  systemHomogeneousForLaTeX = homogenousSystem;
  constantTermsForLaTeX = constantTerms;
  this->currentFormat.flagFormatMatrixAsLinearSystem = true;
  this->printoutPartialFractionsHtml
  << "<br>In other words, we need to solve the system: "
  << HtmlRoutines::getMathNoDisplay(
    homogenousSystem.toStringSystemLatex(
      &constantTerms, &this->currentFormat
    ),
    - 1
  );
  this->printoutPartialFractionsLatex
  << "In other words, we need to solve the following system. "
  << "\\["
  << homogenousSystem.toStringSystemLatex(
    &constantTerms, &this->currentFormat
  )
  << "\\]";
  this->currentFormat.flagUseHTML = true;
  homogenousSystem.gaussianEliminationByRows(
    &constantTerms,
    nullptr,
    nullptr,
    &this->printoutPartialFractionsHtml,
    &this->currentFormat
  );
  this->currentFormat.flagUseHTML = false;
  systemHomogeneousForLaTeX.gaussianEliminationByRows(
    &constantTermsForLaTeX,
    nullptr,
    nullptr,
    &this->printoutPartialFractionsLatex,
    &this->currentFormat
  );
  PolynomialSubstitution<AlgebraicNumber> substitution;
  substitution.makeIdentitySubstitution(this->numberOfSystemVariables + 1);
  for (int i = 1; i < substitution.size; i ++) {
    substitution[i].makeConstant(constantTerms(i - 1, 0));
  }
  for (
    int i = 0; i < this->denominatorFactorsAlgebraicWithMultiplicities.size();
    i ++
  ) {
    for (
      int k = 0; k < this->denominatorFactorsAlgebraicWithMultiplicities.
      coefficients[i]; k ++
    ) {
      this->numerators[i][k].substitution(
        substitution,
        this->owner->objectContainer.algebraicClosure.one()
      );
    }
  }
  this->prepareFinalAnswer();
  this->printoutPartialFractionsHtml
  << "<br>Therefore, the final partial fraction decomposition is: "
  << HtmlRoutines::getMathNoDisplay(this->stringFinalAnswer);
  this->printoutPartialFractionsLatex
  << "Therefore, the final partial fraction decomposition is the following. "
  << "\\["
  << this->stringFinalAnswer
  << "\\]";
  this->printoutPartialFractionsLatex << "\\end{document}";
  this->printoutPartialFractionsHtml
  << "<hr>The present printout, in latex format, in ready form "
  << "for copy + paste to your latex editor, follows<hr> ";
  this->printoutPartialFractionsHtml
  << this->printoutPartialFractionsLatex.str();
  return true;
}

bool CalculatorFunctions::splitToPartialFractionsOverAlgebraicReals(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (input.size() != 2) {
    return false;
  }
  return
  CalculatorFunctions::functionSplitToPartialFractionsOverAlgebraicReals(
    calculator, input[1], output
  );
}

bool CalculatorFunctions::functionSplitToPartialFractionsOverAlgebraicReals(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctions::functionSplitToPartialFractionsOverAlgebraicReals"
  );
  IntegralRationalFunctionComputation computation(&calculator);
  computation.inputIntegrand = input;
  if (
    !CalculatorConversions::functionRationalFraction<Rational>(
      calculator, input, computation.rationalFraction, false
    )
  ) {
    return
    calculator
    << "CalculatorFunctions::"
    << "functionSplitToPartialFractionsOverAlgebraicReals: "
    << "Failed to convert "
    << input.toString()
    << " to rational function. ";
  }
  if (
    computation.rationalFraction.content.minimalNumberOfVariables() > 1
  ) {
    return
    calculator
    << "The input rational function is of "
    << computation.rationalFraction.content.minimalNumberOfVariables()
    << " variables and "
    << " I can handle only 1.";
  }
  if (!computation.computePartialFractionDecomposition()) {
    return
    calculator
    << "Did not manage do decompose "
    << input.toString()
    << " into partial fractions. ";
  }
  if (!computation.preparePartialFractionExpressionSummands()) {
    return
    calculator
    << "Something went wrong while collecting summands "
    << "while splitting "
    << input.toString()
    << " into partial fractions. ";
  }
  return
  output.makeSequence(calculator, &computation.partialFractionSummands);
}

bool CalculatorFunctions::splitToPartialFractionsOverAlgebraicRealsAlgorithm(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctions::splitToPartialFractionsOverAlgebraicRealsAlgorithm"
  );
  IntegralRationalFunctionComputation computation(&calculator);
  if (input.size() < 2) {
    return false;
  }
  if (
    !CalculatorConversions::functionRationalFraction(
      calculator, input[1], computation.rationalFraction, false
    )
  ) {
    return
    calculator
    << "CalculatorFunctions::"
    << "splitToPartialFractionsOverAlgebraicRealsAlgorithm: "
    << "Failed to convert "
    << input[1].toString()
    << " to rational function. ";
  }
  if (
    computation.rationalFraction.content.minimalNumberOfVariables() > 1
  ) {
    return
    calculator
    << "The input rational function is of "
    << computation.rationalFraction.content.minimalNumberOfVariables()
    << " variables and "
    << " I can handle only 1. ";
  }
  computation.computePartialFractionDecomposition();
  return
  output.assignValue(
    calculator, computation.printoutPartialFractionsHtml.str()
  );
}

bool CalculatorFunctions::gaussianEliminationMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("gaussianEliminationMatrix");
  Expression converted;
  if (!CalculatorConversions::makeMatrix(calculator, input, converted)) {
    return
    calculator
    << "<hr>Failed to extract algebraic number matrix from "
    << input.toString()
    << ". ";
  }
  Matrix<AlgebraicNumber> matrix;
  if (
    !CalculatorConversions::functionGetMatrixNoComputation(
      calculator, converted, matrix
    )
  ) {
    return
    calculator
    << "<hr>Failed to extract algebraic number matrix, "
    << "got intermediate conversion to: "
    << converted.toString();
  }
  if (matrix.numberOfRows < 2) {
    return
    calculator
    << "<hr>The matrix I got as input had only 1 row. Possible user typo?";
  }
  std::stringstream out;
  FormatExpressions format;
  format.flagUseFrac = true;
  format.flagUseHTML = true;
  matrix.gaussianEliminationByRows(
    nullptr, nullptr, nullptr, &out, &format
  );
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctions::compositeConstTimesAnyActOn(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::compositeConstTimesAnyActOn");
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
  return
  output.makeXOX(
    calculator, calculator.opTimes(), input[0][1], functionActsOnE
  );
}

bool CalculatorFunctions::compositeElementWeylAlgebraActOnPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctions::compositeElementWeylAlgebraActOnPolynomial"
  );
  if (input.size() != 2) {
    return false;
  }
  Expression elementWeylAlgebra = input[0];
  Expression argument = input[1];
  if (
    !elementWeylAlgebra.isListStartingWithAtom(
      calculator.opElementWeylAlgebra()
    )
  ) {
    return false;
  }
  if (!argument.isBuiltInType()) {
    return false;
  }
  if (!elementWeylAlgebra.mergeContexts(elementWeylAlgebra, argument)) {
    return false;
  }
  WithContext<Polynomial<Rational> > polynomial;
  if (
    !CalculatorConversions::convertWithoutComputation(
      calculator, argument, polynomial
    )
  ) {
    return false;
  }
  const ElementWeylAlgebra<Rational>& elementWeylAlgebraInternal =
  elementWeylAlgebra.getValue<ElementWeylAlgebra<Rational> >();
  if (elementWeylAlgebraInternal.hasNonSmallPositiveIntegerDerivation()) {
    return
    calculator
    << "<hr> I cannot apply "
    << elementWeylAlgebraInternal.toString()
    << " onto "
    << polynomial.content.toString()
    << " as "
    << "the differential operator contains "
    << "non-integral differential operator exponents. ";
  }
  if (!elementWeylAlgebraInternal.actOnPolynomial(polynomial.content)) {
    std::stringstream out;
    out
    << "Failed to act by operator "
    << elementWeylAlgebraInternal.toString()
    << " on polynomial "
    << polynomial.content.toString()
    << " (possibly the weyl algebra "
    << "element has non-integral exponents)";
    return output.assignError(calculator, out.str());
  }
  return
  output.assignValueWithContext(
    calculator, polynomial.content, polynomial.context
  );
}

bool CalculatorFunctions::formatCPPSourceCodeDebug(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return formatCPPSourceCodeInternal(calculator, input, output, true);
}

bool CalculatorFunctions::formatCPPSourceCode(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return formatCPPSourceCodeInternal(calculator, input, output, false);
}

bool CalculatorFunctions::formatCPPSourceCodeInternal(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool logDebugData
) {
  STACK_TRACE("CalculatorFunctions::formatCPPSourceCode");
  if (!global.userDefaultHasAdminRights()) {
    return
    calculator
    << "Cpp code formatting available only to logged-in admins. ";
  }
  if (input.size() != 2) {
    return false;
  }
  std::string fileName;
  if (!input[1].isOfType(&fileName)) {
    return false;
  }
  std::stringstream report;
  CodeFormatter formatter;
  formatter.formatCPPSourceCode(
    fileName, fileName + ".new", fileName + ".new", &report, logDebugData
  );
  return output.assignValue(calculator, report.str());
}

bool CalculatorFunctions::formatCPPDirectory(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!global.userDefaultHasAdminRights()) {
    return
    calculator
    << "Cpp code formatting available only to logged-in admins. ";
  }
  if (input.size() < 2) {
    return false;
  }
  std::string fileName;
  if (!input[1].isOfType(&fileName)) {
    return false;
  }
  std::stringstream report;
  CodeFormatter::formatCPPDirectory(fileName, false, &report);
  return output.assignValue(calculator, report.str());
}

bool CalculatorFunctionsListsAndSets::intersection(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsListsAndSets::intersection");
  if (!input.startsWith(calculator.opIntersection())) {
    return false;
  }
  for (int i = 1; i < input.size(); i ++) {
    if (!input[i].isSequenceNElements()) {
      return false;
    }
  }
  List<List<Expression> > toBeIntersected;
  toBeIntersected.setSize(input.size() - 1);
  for (int i = 1; i < input.size(); i ++) {
    toBeIntersected[i - 1].reserve(input[i].size());
    for (int j = 1; j < input[i].size(); j ++) {
      toBeIntersected[i - 1].addOnTop(input[i][j]);
    }
  }
  if (toBeIntersected.size == 0) {
    return false;
  }
  List<Expression> outputList = toBeIntersected[0];
  for (int i = 1; i < toBeIntersected.size; i ++) {
    outputList.intersectWith(toBeIntersected[i], outputList);
  }
  return output.makeSequence(calculator, &outputList);
}

bool CalculatorFunctionsListsAndSets::listUnion(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsListsAndSets::listUnion");
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

bool CalculatorFunctionsListsAndSets::belongsTo(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsListsAndSets::listUnion");
  if (input.size() != 3) {
    return false;
  }
  if (!input[2].isSequenceNElements()) {
    return false;
  }
  for (int i = 1; i < input[2].size(); i ++) {
    if (input[1] == input[2][i]) {
      return output.assignValue(calculator, 1);
    }
  }
  return output.assignValue(calculator, 0);
}

bool CalculatorFunctionsListsAndSets::unionNoRepetition(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsListsAndSets::unionNoRepetition");
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
  HashedList<Expression> unionHashedList;
  List<int> indices;
  unionHashedList.setExpectedSize(numElts);
  for (int i = 1; i < input.size(); i ++) {
    for (int j = 1; j < input[i].size(); j ++) {
      unionHashedList.addOnTopNoRepetition(input[i][j]);
    }
  }
  indices.setSize(unionHashedList.size);
  for (int i = 0; i < unionHashedList.size; i ++) {
    indices[i] = calculator.addChildExpression(unionHashedList[i]);
  }
  output.setExpectedSize(numElts);
  output.reset(calculator, indices.size + 1);
  output.addChildAtomOnTop(calculator.opSequence());
  output.addChildIndices(indices);
  return true;
}

bool CalculatorFunctions::crossProduct(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::crossProduct");
  if (
    !input.isListStartingWithAtom(calculator.opCrossProduct()) ||
    input.size() != 3
  ) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (
    !leftE.isSequenceNElements(3) || !rightE.isSequenceNElements(3)
  ) {
    std::stringstream out;
    out
    << "Can't compute cross product of the non-3d vectors "
    << input[1].toString()
    << " and "
    << input[2].toString()
    << ". ";
    return output.assignError(calculator, out.str());
  }
  List<Expression> outputSequence;
  outputSequence.setSize(3);
  outputSequence[0] = leftE[2] * rightE[3] - leftE[3] * rightE[2];
  outputSequence[1] = leftE[3] * rightE[1] - leftE[1] * rightE[3];
  outputSequence[2] = leftE[1] * rightE[2] - leftE[2] * rightE[1];
  return output.makeSequence(calculator, &outputSequence);
}

bool CalculatorFunctionsDifferentiation::differentiateConstPower(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsDifferentiation::differentiateConstPower");
  // ////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator
    << "<hr>Warning: differentiating with "
    << "respect to the non-atomic expression"
    << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& differentialOperatorVariable = input[1];
  const Expression& argument = input[2];
  // ////////////////////
  if (!argument.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  if (!argument[2].isConstantNumber()) {
    return false;
  }
  Expression monomial, term, exponent, basePrime, minusOne;
  minusOne.assignValue<Rational>(calculator, - 1);
  exponent.makeXOX(
    calculator, calculator.opPlus(), argument[2], minusOne
  );
  monomial.makeXOX(
    calculator, calculator.opPower(), argument[1], exponent
  );
  basePrime.makeXOX(
    calculator,
    calculator.opDifferentiate(),
    differentialOperatorVariable,
    argument[1]
  );
  term.makeXOX(
    calculator, calculator.opTimes(), argument[2], monomial
  );
  return
  output.makeXOX(calculator, calculator.opTimes(), term, basePrime);
}

bool CalculatorFunctionsDifferentiation::differentiateAPowerB(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsDifferentiation::differentiateAPowerB");
  // ////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator
    << "<hr>Warning: differentiating with "
    << "respect to the non-atomic expression "
    << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& differentialOperatorVariable = input[1];
  const Expression& argument = input[2];
  // ////////////////////
  // d/dx a^b= d/dx(e^{b\\ln a}) = a^b d/dx(b\\log a)
  if (!argument.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  Expression
  logarithmBase,
  exponentTimesLogBase,
  derivativeExponentTimesLogBase;
  logarithmBase.reset(calculator, 2);
  logarithmBase.addChildAtomOnTop(calculator.opLog());
  logarithmBase.addChildOnTop(argument[1]);
  exponentTimesLogBase.makeXOX(
    calculator, calculator.opTimes(), argument[2], logarithmBase
  );
  derivativeExponentTimesLogBase.makeXOX(
    calculator,
    calculator.opDifferentiate(),
    differentialOperatorVariable,
    exponentTimesLogBase
  );
  return
  output.makeXOX(
    calculator,
    calculator.opTimes(),
    argument,
    derivativeExponentTimesLogBase
  );
}

bool CalculatorFunctionsDifferentiation::differentiateConstant(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsDifferentiation::differentiateConstant");
  // ////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator
    << "<hr>Warning: differentiating with "
    << "respect to the non-atomic expression"
    << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& argument = input[2];
  // ////////////////////
  if (!argument.isConstantNumber()) {
    return false;
  }
  return output.assignValue<Rational>(calculator, 0);
}

bool CalculatorFunctionsDifferentiation::differentiateX(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsDifferentiation::differentiateX");
  // ////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator
    << "<hr>Warning: differentiating with "
    << "respect to the non-atomic expression"
    << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& differentialOperatorVariable = input[1];
  const Expression& argument = input[2];
  // ////////////////////
  if (argument != differentialOperatorVariable) return false;
  return output.assignValue<Rational>(calculator, 1);
}

bool CalculatorFunctionsDifferentiation::differentiateTrigAndInverseTrig(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsDifferentiation::differentiateTrigAndInverseTrig"
  );
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator
    << "<hr>Warning: differentiating with "
    << "respect to the non-atomic expression "
    << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& argument = input[2];
  if (argument.isOperationGiven(calculator.opSin())) {
    return output.makeAtom(calculator, calculator.opCos());
  }
  if (argument.isOperationGiven(calculator.opCos())) {
    Expression mOneE, sinE;
    mOneE.assignValue<Rational>(calculator, - 1);
    sinE.makeAtom(calculator, calculator.opSin());
    return output.makeXOX(calculator, calculator.opTimes(), mOneE, sinE);
  }
  if (argument.isOperationGiven(calculator.opTan())) {
    Expression secE, twoE;
    secE.makeAtom(calculator, calculator.opSec());
    twoE.assignValue(calculator, 2);
    return output.makeXOX(calculator, calculator.opPower(), secE, twoE);
  }
  if (argument.isOperationGiven(calculator.opCot())) {
    Expression cscE, twoE, cscSquared, mOneE;
    cscE.makeAtom(calculator, calculator.opCsc());
    twoE.assignValue(calculator, 2);
    cscSquared.makeXOX(calculator, calculator.opPower(), cscE, twoE);
    mOneE.assignValue(calculator, - 1);
    return
    output.makeXOX(calculator, calculator.opTimes(), mOneE, cscSquared);
  }
  if (argument.isOperationGiven(calculator.opSec())) {
    Expression tanE, secE;
    tanE.makeAtom(calculator, calculator.opTan());
    secE.makeAtom(calculator, calculator.opSec());
    return output.makeXOX(calculator, calculator.opTimes(), tanE, secE);
  }
  if (argument.isOperationGiven(calculator.opCsc())) {
    Expression cotE, cscE, mOneE, cotTimesCscE;
    cotE.makeAtom(calculator, calculator.opCot());
    cscE.makeAtom(calculator, calculator.opCsc());
    mOneE.assignValue(calculator, - 1);
    cotTimesCscE.makeXOX(calculator, calculator.opTimes(), cotE, cscE);
    return
    output.makeXOX(
      calculator, calculator.opTimes(), mOneE, cotTimesCscE
    );
  }
  if (argument.isOperationGiven(calculator.opArcTan())) {
    Polynomial<Rational> onePlusXsquared;
    RationalFraction<Rational> oneOverOnePlusXsquared;
    onePlusXsquared.makeMonomial(0, 2);
    onePlusXsquared += Rational::one();
    oneOverOnePlusXsquared.makeOne();
    oneOverOnePlusXsquared /= onePlusXsquared;
    ExpressionContext context(calculator);
    context.makeOneVariableFromString("x");
    return
    output.assignValueWithContext(
      calculator, oneOverOnePlusXsquared, context
    );
  }
  if (argument.isOperationGiven(calculator.opArcSin())) {
    Expression denE;
    ExpressionContext context(calculator);
    context.makeOneVariableFromString("x");
    RationalFraction<Rational> oneMinusXsquared;
    oneMinusXsquared.makeMonomial(0, 2);
    oneMinusXsquared *= - 1;
    oneMinusXsquared += 1;
    denE.assignValueWithContext(calculator, oneMinusXsquared, context);
    return
    output.makeXOX(
      calculator,
      calculator.opPower(),
      denE,
      calculator.expressionMinusHalf()
    );
  }
  if (argument.isOperationGiven(calculator.opArcCos())) {
    Expression denE;
    ExpressionContext context;
    context.makeOneVariableFromString("x");
    RationalFraction<Rational> oneMinusXsquared;
    oneMinusXsquared.makeMonomial(0, 2);
    oneMinusXsquared *= - 1;
    oneMinusXsquared += 1;
    denE.assignValueWithContext(calculator, oneMinusXsquared, context);
    output.makeXOX(
      calculator,
      calculator.opPower(),
      denE,
      calculator.expressionMinusHalf()
    );
    output *= - 1;
    return true;
  }
  return false;
}

bool CalculatorFunctions::compositeDifferentiateLog(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::compositeDifferentiateLog");
  // ///////////////////
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
  OneE.assignValue(calculator, 1);
  output.reset(calculator, 2);
  return
  output.makeXOX(calculator, calculator.opDivide(), OneE, input[1]);
}

bool CalculatorFunctions::divideByNumber(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::divideByNumber");
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (input[2].isEqualToZero()) {
    return output.assignError(calculator, "Division by zero. ");
  }
  Rational rationalValue;
  AlgebraicNumber algebraicValue;
  double doubleValue;
  Expression invertedExpression;
  bool result = false;
  if (input[2].isOfType<Rational>(&rationalValue)) {
    rationalValue.invert();
    invertedExpression.assignValue(calculator, rationalValue);
    result = true;
  }
  if (input[2].isOfType<AlgebraicNumber>(&algebraicValue)) {
    algebraicValue.invert();
    invertedExpression.assignValue(calculator, algebraicValue);
    result = true;
  }
  if (input[2].isOfType<double>(&doubleValue)) {
    doubleValue = 1 / doubleValue;
    invertedExpression.assignValue(calculator, doubleValue);
    result = true;
  }
  if (!result) {
    return false;
  }
  output = invertedExpression * input[1];
  return true;
}

bool CalculatorFunctions::maximum(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::maximum");
  (void) calculator;
  if (input.size() < 3) {
    return false;
  }
  double maximumValue = 0;
  double current = 0;
  int bestIndex = 1;
  for (int i = 1; i < input.size(); i ++) {
    if (!input[i].evaluatesToDouble(&current)) {
      return false;
    }
    if (i == 1) {
      maximumValue = current;
    } else if (maximumValue < current) {
      maximumValue = current;
      bestIndex = i;
    }
  }
  output = input[bestIndex];
  return true;
}

bool CalculatorFunctions::minimum(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::minimum");
  (void) calculator;
  if (input.size() < 3) {
    return false;
  }
  double minimalValue = 0;
  double current = 0;
  int bestIndex = 1;
  for (int i = 1; i < input.size(); i ++) {
    if (!input[i].evaluatesToDouble(&current)) {
      return false;
    }
    if (i == 1) {
      minimalValue = current;
    } else if (minimalValue > current) {
      minimalValue = current;
      bestIndex = i;
    }
  }
  output = input[bestIndex];
  return true;
}

bool CalculatorFunctions::equalEqualEqual(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::equalEqualEqual");
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression& left = input[1];
  const Expression& right = input[2];
  int result = 0;
  if (left == right) {
    result = 1;
  }
  return output.assignValue(calculator, result);
}

bool CalculatorFunctions::outerEqualEqual(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::outerEqualEqual");
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression& left = input[1];
  const Expression& right = input[2];
  if (left.hasBoundVariables() || right.hasBoundVariables()) {
    return false;
  }
  if (left == right) {
    return output.assignValue(calculator, 1);
  }
  if (left.isEqualToMathematically(right)) {
    return output.assignValue(calculator, 1);
  } else {
    return output.assignValue(calculator, 0);
  }
}

bool CalculatorFunctions::associateAdivBdivCPowerD(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::associateAdivBdivCPowerD");
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (!input[2].startsWith(calculator.opPower(), 3)) {
    return false;
  }
  if (!input[2][1].startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  calculator.checkInputNotSameAsOutput(input, output);
  Expression numeratorE, numeratorLeftE, denominatorE;
  output.reset(calculator, 3);
  numeratorLeftE.makeXOX(
    calculator,
    calculator.opPower(),
    input[2][1][2],
    input[2][2]
  );
  numeratorE.makeXOX(
    calculator, calculator.opTimes(), input[1], numeratorLeftE
  );
  denominatorE.makeXOX(
    calculator,
    calculator.opPower(),
    input[2][1][1],
    input[2][2]
  );
  return
  output.makeXOX(
    calculator, calculator.opDivide(), numeratorE, denominatorE
  );
}

bool CalculatorFunctions::divisionCancellations(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::divisionCancellations");
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
    return
    output.makeXOX(
      calculator,
      calculator.opDivide(),
      input[1][1],
      input[2][1]
    );
  }
  if (input[1][1] == input[2][1]) {
    return
    output.makeXOX(
      calculator,
      calculator.opDivide(),
      input[2][2],
      input[1][2]
    );
  }
  return false;
}

bool CalculatorFunctions::associateDivisionDivision(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::associateDivisionDivision");
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (input[1].startsWith(calculator.opDivide(), 3)) {
    Expression newRightE;
    newRightE.makeXOX(
      calculator, calculator.opTimes(), input[2], input[1][2]
    );
    return
    output.makeXOX(
      calculator, calculator.opDivide(), input[1][1], newRightE
    );
  }
  if (input[2].startsWith(calculator.opDivide(), 3)) {
    Expression newLeftE;
    newLeftE.makeXOX(
      calculator, calculator.opTimes(), input[1], input[2][2]
    );
    return
    output.makeXOX(
      calculator, calculator.opDivide(), newLeftE, input[2][1]
    );
  }
  return false;
}

bool CalculatorFunctionsDifferentiation::differentiateChainRule(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsDifferentiation::differentiateChainRule");
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator
    << "<hr>Warning: differentiating with "
    << "respect to the non-atomic expression"
    << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& differentialOperatorVariable = input[1],
  argument = input[2];
  if (!argument.startsWith(- 1, 2)) {
    return false;
  }
  if (
    !argument.isGoodForChainRuleFunction() &&
    !argument[0].isGoodForChainRuleFunction()
  ) {
    return false;
  }
  calculator.checkInputNotSameAsOutput(input, output);
  output.reset(calculator);
  Expression multiplicandleft, multiplicandleftFunction, multiplicandright;
  multiplicandleftFunction.makeXOX(
    calculator,
    calculator.opDifferentiate(),
    differentialOperatorVariable,
    argument[0]
  );
  multiplicandleft.reset(calculator);
  multiplicandleft.addChildOnTop(multiplicandleftFunction);
  multiplicandleft.addChildOnTop(argument[1]);
  multiplicandright.makeXOX(
    calculator,
    calculator.opDifferentiate(),
    differentialOperatorVariable,
    argument[1]
  );
  return
  output.makeXOX(
    calculator,
    calculator.opTimes(),
    multiplicandleft,
    multiplicandright
  );
}

bool CalculatorFunctionsDifferentiation::differentiateAplusB(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsDifferentiation::differentiateAplusB");
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator
    << "<hr>Warning: differentiating with "
    << "respect to the non-atomic expression "
    << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& differentialOperatorVariable = input[1],
  argument = input[2];
  // ////////////////////
  if (!argument.startsWith(calculator.opPlus(), 3)) {
    return false;
  }
  calculator.checkInputNotSameAsOutput(input, output);
  output.reset(calculator);
  Expression leftSummand, rightSummand;
  leftSummand.makeXOX(
    calculator,
    calculator.opDifferentiate(),
    differentialOperatorVariable,
    argument[1]
  );
  rightSummand.makeXOX(
    calculator,
    calculator.opDifferentiate(),
    differentialOperatorVariable,
    argument[2]
  );
  return
  output.makeXOX(
    calculator, calculator.opPlus(), leftSummand, rightSummand
  );
}

bool CalculatorFunctionsDifferentiation::differentiateAtimesB(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsDifferentiation::differentiateAtimesB");
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator
    << "<hr>Warning: differentiating with "
    << "respect to the non-atomic expression"
    << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& differentialOperatorVariable = input[1],
  argument = input[2];
  // ////////////////////
  if (!argument.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  calculator.checkInputNotSameAsOutput(input, output);
  output.reset(calculator);
  Expression changedMultiplicand, leftSummand, rightSummand;
  changedMultiplicand.makeXOX(
    calculator,
    calculator.opDifferentiate(),
    differentialOperatorVariable,
    argument[1]
  );
  leftSummand.makeXOX(
    calculator,
    calculator.opTimes(),
    changedMultiplicand,
    argument[2]
  );
  changedMultiplicand.makeXOX(
    calculator,
    calculator.opDifferentiate(),
    differentialOperatorVariable,
    argument[2]
  );
  rightSummand.makeXOX(
    calculator,
    calculator.opTimes(),
    argument[1],
    changedMultiplicand
  );
  return
  output.makeXOX(
    calculator, calculator.opPlus(), leftSummand, rightSummand
  );
}

bool CalculatorFunctions::powerAnyToZero(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::powerAnyToZero");
  if (!input.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  if (!input[2].isEqualToZero()) {
    return false;
  }
  if (input[1].startsWith(calculator.opIntegral())) {
    if (input[1].size() != 3) {
      // <- an expression of the form \int_a^b (without f*dx).
      return false;
    }
  }
  if (input[1].isEqualToZero()) {
    // TODO(tmilev): 0^0 is 1 by definition, fix this.
    return
    output.assignError(
      calculator, "Error: expression of the form 0^0 is illegal."
    );
  }
  return output.assignValue<Rational>(calculator, 1);
}

bool CalculatorFunctionsDifferentiation::differentiateAdivideBCommutative(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsDifferentiation::differentiateAdivideBCommutative"
  );
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator
    << "<hr>Warning: differentiating "
    << "with respect to the non-atomic expression "
    << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& differentialOperatorVariable = input[1],
  argument = input[2];
  // Quotient rule (commutative): (a/b^n)'= (a'b-n a b')/b^{n + 1}
  if (!argument.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  const Expression& numeratorE = argument[1];
  const Expression& denominatorE = argument[2];
  if (numeratorE.startsWith(calculator.opPlus())) {
    Expression leftE(calculator), rightE(calculator);
    leftE.addChildAtomOnTop(calculator.opDifferentiate());
    leftE.addChildOnTop(differentialOperatorVariable);
    rightE = leftE;
    leftE.addChildOnTop(numeratorE[1] / denominatorE);
    rightE.addChildOnTop(numeratorE[2] / denominatorE);
    output = leftE + rightE;
    return true;
  }
  calculator.checkInputNotSameAsOutput(input, output);
  output.reset(calculator);
  Expression
  denominatorBase,
  eOne,
  denominatorExponentPlusOne,
  denominatorExponent,
  changedMultiplicand,
  leftSummand,
  rightSummand,
  denominatorFinal,
  numerator;
  eOne.assignValue(calculator, 1);
  bool denBaseFound = false;
  if (argument[2].startsWith(calculator.opPower(), 3)) {
    if (argument[2][2].isConstantNumber()) {
      denBaseFound = true;
      denominatorBase = argument[2][1];
      denominatorExponent = argument[2][2];
      denominatorExponentPlusOne.makeXOX(
        calculator, calculator.opPlus(), denominatorExponent, eOne
      );
    }
  }
  if (!denBaseFound) {
    denominatorBase = argument[2];
    denominatorExponentPlusOne.assignValue(calculator, 2);
    denominatorExponent.assignValue(calculator, 1);
  }
  denominatorFinal.makeXOX(
    calculator,
    calculator.opPower(),
    denominatorBase,
    denominatorExponentPlusOne
  );
  changedMultiplicand.makeXOX(
    calculator,
    calculator.opDifferentiate(),
    differentialOperatorVariable,
    argument[1]
  );
  leftSummand.makeXOX(
    calculator,
    calculator.opTimes(),
    changedMultiplicand,
    denominatorBase
  );
  rightSummand.makeXOX(
    calculator,
    calculator.opDifferentiate(),
    differentialOperatorVariable,
    denominatorBase
  );
  changedMultiplicand.makeXOX(
    calculator, calculator.opTimes(), argument[1], rightSummand
  );
  rightSummand.makeXOX(
    calculator,
    calculator.opTimes(),
    denominatorExponent,
    changedMultiplicand
  );
  numerator.makeXOX(
    calculator, calculator.opMinus(), leftSummand, rightSummand
  );
  return
  output.makeXOX(
    calculator, calculator.opDivide(), numerator, denominatorFinal
  );
}

bool CalculatorFunctionsDifferentiation::differentiateAdivideBNONCommutative(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsDifferentiation::differentiateAdivideBNONCommutative"
  );
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator
    << "<hr>Warning: differentiating with "
    << "respect to the non-atomic expression "
    << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& differentialOperatorVariable = input[1],
  argument = input[2];
  // Quotient rule (non-commutative): (a/b)'= (ab^{- 1})'=a' b - a b^{- 1} b'
  // b^{- 1}
  if (!argument.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  calculator.checkInputNotSameAsOutput(input, output);
  output.reset(calculator);
  Expression changedMultiplicand, leftSummand, rightSummand;
  Expression bInverse, bPrime, eMOne;
  eMOne.assignValue<Rational>(calculator, - 1);
  changedMultiplicand.makeXOX(
    calculator,
    calculator.opDifferentiate(),
    differentialOperatorVariable,
    argument[1]
  );
  leftSummand.makeXOX(
    calculator,
    calculator.opDivide(),
    changedMultiplicand,
    argument[2]
  );
  bPrime.makeXOX(
    calculator,
    calculator.opDifferentiate(),
    differentialOperatorVariable,
    argument[2]
  );
  bInverse.makeXOX(
    calculator, calculator.opPower(), argument[2], eMOne
  );
  rightSummand.makeXOX(
    calculator, calculator.opTimes(), bPrime, bInverse
  );
  // rightSummand = b' b^{- 1}
  changedMultiplicand.makeXOX(
    calculator, calculator.opTimes(), bInverse, rightSummand
  );
  // changedMultiplicand = b^- 1 b' b^- 1
  rightSummand.makeXOX(
    calculator,
    calculator.opTimes(),
    argument[1],
    changedMultiplicand
  );
  return
  output.makeXOX(
    calculator, calculator.opMinus(), leftSummand, rightSummand
  );
}

bool CalculatorFunctions::outerCommuteAtimesBifUnivariate(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::outerCommuteAtimesBifUnivariate");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (input[1].isConstantNumber()) {
    return false;
  }
  HashedList<Expression> commutativityBlocks;
  input.getBlocksOfCommutativity(commutativityBlocks);
  if (commutativityBlocks.size != 1) {
    return false;
  }
  if (input[2] > input[1] || input[2] == input[1]) {
    return false;
  }
  output = input;
  output.swapChildren(1, 2);
  return true;
}

bool CalculatorFunctions::outerCommuteAtimesBtimesCifUnivariate(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::outerCommuteAtimesBtimesCifUnivariate");
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
  HashedList<Expression> commutativityBlocks;
  leftExpression.getBlocksOfCommutativity(commutativityBlocks);
  middleExpression.getBlocksOfCommutativity(commutativityBlocks);
  if (commutativityBlocks.size != 1) {
    return false;
  }
  if (middleExpression > leftExpression || middleExpression == leftExpression) {
    return false;
  }
  Expression rightMultiplicand;
  rightMultiplicand.makeProduct(
    calculator, leftExpression, input[2][2]
  );
  return
  output.makeXOX(
    calculator,
    calculator.opTimes(),
    middleExpression,
    rightMultiplicand
  );
}

bool CalculatorFunctions::getFreeVariablesIncludeNamedConstants(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::getFreeVariablesIncludeNamedConstants");
  if (input.size() != 2) {
    return false;
  }
  HashedList<Expression> outputList;
  if (!input[1].getFreeVariables(outputList, false)) {
    return
    calculator
    << "Function getFreeVariables failed, this shouldn't happen. ";
  }
  return output.makeSequence(calculator, &outputList);
}

bool CalculatorFunctions::getFreeVariablesExcludeNamedConstants(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::getFreeVariablesExcludeNamedConstants");
  if (input.size() != 2) {
    return false;
  }
  HashedList<Expression> outputList;
  if (!input[1].getFreeVariables(outputList, true)) {
    return
    calculator
    << "Function getFreeVariables failed, this shouldn't happen.";
  }
  return output.makeSequence(calculator, &outputList);
}

bool CalculatorFunctions::Test::checkSorting(
  const List<Expression>& mustBeSorted
) {
  if (mustBeSorted.size < 5) {
    // too many elements, bail out.
    return true;
  }
  for (int i = 0; i < mustBeSorted.size; i ++) {
    for (int j = i; j < mustBeSorted.size; j ++) {
      if (
        mustBeSorted[i] > mustBeSorted[j] && mustBeSorted[j] > mustBeSorted[i]
      ) {
        global.fatal
        << "Faulty comparison: "
        << mustBeSorted[i].toString()
        << " and "
        << mustBeSorted[j].toString()
        << " are mutually greater than one another. "
        << global.fatal;
      }
    }
  }
  return true;
}

bool CalculatorFunctions::addTerms(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("Calculator::addTerms");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.startsWith(calculator.opPlus())) {
    return false;
  }
  LinearCombination<Expression, Rational> sum;
  if (
    !calculator.functionCollectSummandsCombine(calculator, input, sum)
  ) {
    return false;
  }
  sum.quickSortDescending();
  CalculatorFunctions::Test::checkSorting(sum.monomials);
  output.makeSum(calculator, sum);
  if (output == input) {
    return false;
  }
  return true;
}

bool CalculatorFunctions::sortTerms(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("Calculator::sortTerms");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.startsWith(calculator.opPlus())) {
    return false;
  }
  HashedList<Expression> monomials;
  List<Rational> coefficients;
  List<Expression> summands;
  bool orderNonCanonical =
  calculator.functionCollectSummandsSeparatelyTrueIfOrderNonCanonical(
    calculator, input, summands, monomials, coefficients
  );
  List<Expression> sortedMonomials = monomials;
  // Not guaranteed to be stable. Consider fixing.
  sortedMonomials.quickSortDescending(nullptr, &summands);
  if (!orderNonCanonical) {
    if (monomials == sortedMonomials) {
      // Possible when monomials repeat,
      // for example 2x+x will have the monomial
      // x appear twice.
      // Since sorting is not guaranteed to
      // be stable at present, the two terms
      // may have been swapped.
      return false;
    }
  }
  CalculatorFunctions::Test::checkSorting(summands);
  output.makeSum(calculator, summands);
  if (output == input) {
    return false;
  }
  return true;
}

bool CalculatorFunctions::compareFunctionsNumerically(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::compareFunctionsNumerically");
  if (input.size() < 5) {
    return
    calculator
    << "Comparing functions takes as input at least 4 arguments "
    << "(two functions to compare and interval to compare over).";
  }
  Expression functionExpression = input[1];
  functionExpression -= input[2];
  HashedList<Expression> variables;
  if (!functionExpression.getFreeVariables(variables, true)) {
    return
    calculator
    << "Was not able to extract the function argument of your function. ";
  }
  if (variables.size <= 0) {
    Expression zeroE;
    zeroE.assignValue(calculator, 0);
    return
    output.makeXOX(
      calculator, calculator.opEqualEqual(), functionExpression, zeroE
    );
  }
  if (variables.size > 1) {
    return
    calculator
    << "I cannot compare the functions as they appear "
    << "to depend on more than one variable, namely, on: "
    << variables.toStringCommaDelimited();
  }
  double leftBoundary = 0;
  double rightBoundary = 0;
  if (!input[3].evaluatesToDouble(&leftBoundary)) {
    return
    calculator
    << "Failed to extract the left endpoint of the comparison interval. ";
  }
  if (!input[4].evaluatesToDouble(&rightBoundary)) {
    return
    calculator
    << "Failed to extract the right endpoint of the comparison interval. ";
  }
  int numPoints = 50;
  if (input.size() > 5) {
    if (!input[5].isSmallInteger(&numPoints)) {
      return
      calculator
      << "Failed to convert argument: "
      << input[5].toString()
      << " to a small integer. ";
    }
  }
  double minDiff = 0, maxDiff = 0;
  if (
    !functionExpression.evaluatesToDoubleInRange(
      variables[0].toString(),
      leftBoundary,
      rightBoundary,
      numPoints,
      &minDiff,
      &maxDiff,
      nullptr
    )
  ) {
    return
    calculator
    << "Failed to evaluate your function to a number. "
    << "The sampling interval may be outside "
    << "of the domain of the function. ";
  }
  double tolerance = 0.0001;
  if (input.size() > 6) {
    if (!input[6].evaluatesToDouble(&tolerance)) {
      return
      calculator
      << "Failed to evaluate the argument "
      << input[6].toString()
      << " to a floating point number. ";
    }
  }
  if (minDiff < - tolerance || maxDiff > tolerance) {
    return output.assignValue(calculator, 0);
  }
  return output.assignValue(calculator, 1);
}

bool CalculatorFunctions::compareExpressionsNumericallyAtPoints(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::compareExpressionsNumericallyAtPoints");
  if (input.size() < 5) {
    return
    calculator
    << "Comparing functions at points takes as input at least 5 arguments "
    << "- two functions to compare, precision, variable belonging "
    << "to an interval and number of sampling points).";
  }
  Expression functionExpression = input[1];
  functionExpression -= input[2];
  HashedList<Expression> freeVariables;
  if (!functionExpression.getFreeVariables(freeVariables, true)) {
    return
    calculator
    << "Was not able to extract the function argument of your function. ";
  }
  if (freeVariables.size <= 0) {
    Expression zeroE;
    zeroE.assignValue(calculator, 0);
    return
    output.makeXOX(
      calculator, calculator.opEqualEqual(), functionExpression, zeroE
    );
  }
  double tolerance = 0.0001;
  if (!input[3].evaluatesToDouble(&tolerance)) {
    return
    calculator
    << "Failed to evaluate tolerance limit from "
    << input[3].toString()
    << " to a floating point number. ";
  }
  if (tolerance < 0) {
    tolerance *= - 1;
  }
  if (!input[4].startsWith(calculator.opIn(), 3)) {
    return
    calculator
    << "The fourth argument "
    << input[4].toString()
    << " needs to be "
    << " of the form (x,y,...)\\in (...). ";
  }
  const Expression& variableExpressions = input[4][1];
  HashedList<Expression> varsGiven;
  if (!variableExpressions.isSequenceNElements()) {
    varsGiven.addOnTop(variableExpressions);
  } else {
    for (int i = 1; i < variableExpressions.size(); i ++) {
      if (varsGiven.contains(variableExpressions[i])) {
        return
        calculator
        << variableExpressions[i]
        << " given more than once. ";
      }
      varsGiven.addOnTop(variableExpressions[i]);
    }
  }
  for (int i = 0; i < freeVariables.size; i ++) {
    if (!varsGiven.contains(freeVariables[i])) {
      return
      calculator
      << "The expression depends on "
      << freeVariables[i].toString()
      << " but I no value given for that expression. ";
    }
  }
  const Expression& pointsExpressions = input[4][2];
  Matrix<double> points;
  if (
    !CalculatorConversions::functionGetMatrix(
      calculator,
      pointsExpressions,
      points,
      true,
      nullptr,
      varsGiven.size
    )
  ) {
    return
    calculator
    << "Failed to extract list of points from: "
    << pointsExpressions.toString();
  }
  HashedList<Expression> knownEs = calculator.knownDoubleConstants;
  List<double> knownValues = calculator.knownDoubleConstantValues;
  for (int i = 0; i < varsGiven.size; i ++) {
    if (knownEs.contains(varsGiven[i])) {
      return
      calculator
      << varsGiven[i]
      << " is an already known constant and "
      << "cannot be used as a variable in this context. ";
    } else {
      knownEs.addOnTop(varsGiven[i]);
    }
  }
  knownValues.setSize(knownEs.size);
  int numFailedSamples = 0;
  int totalSamples = points.numberOfRows;
  for (int i = 0; i < points.numberOfRows; i ++) {
    for (int j = 0; j < points.numberOfColumns; j ++) {
      knownValues[j + calculator.knownDoubleConstants.size] = points(i, j);
    }
    double floatingResult = 0;
    if (
      !functionExpression.evaluatesToDoubleUnderSubstitutions(
        knownEs, knownValues, &floatingResult
      )
    ) {
      numFailedSamples ++;
      if ((numFailedSamples* 100) / totalSamples > 20) {
        calculator
        << "Failed to evaluate at least one "
        << "of the functions in more "
        << "than 20% of the sampling points. ";
        return output.assignValue(calculator, 0);
      }
    }
    if (floatingResult > tolerance || floatingResult < - tolerance) {
      return output.assignValue(calculator, 0);
    }
  }
  return output.assignValue(calculator, 1);
}

bool CalculatorFunctions::compareExpressionsNumerically(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::compareExpressionsNumerically");
  if (input.size() < 6) {
    return
    calculator
    << "Comparing functions takes as input at least 5 arguments "
    << "- two functions to compare, precision, "
    << "variable belonging to an interval "
    << "and number of sampling points).";
  }
  Expression functionExpression = input[1];
  functionExpression -= input[2];
  HashedList<Expression> variables;
  if (!functionExpression.getFreeVariables(variables, true)) {
    return
    calculator
    << "Was not able to extract the "
    << "function argument of your function. ";
  }
  if (variables.size <= 0) {
    Expression zeroE;
    zeroE.assignValue(calculator, 0);
    return
    output.makeXOX(
      calculator, calculator.opEqualEqual(), functionExpression, zeroE
    );
  }
  List<double> leftBoundaries;
  List<double> rightBoundaries;
  HashedList<Expression> boundaryVariables;
  List<int> numSamples;
  for (int i = 4; i < input.size(); i += 2) {
    const Expression& currentIntervalWithVariable = input[i];
    if (
      !currentIntervalWithVariable.startsWith(calculator.opIn(), 3)
    ) {
      return
      calculator
      << "Expression "
      << currentIntervalWithVariable.toString()
      << " is not a belonging relation (does not use the \\in symbol). ";
    }
    if (!currentIntervalWithVariable[2].isSequenceNElements(2)) {
      return
      calculator
      << "Could not get a two-element sequence from "
      << currentIntervalWithVariable[2].toString();
    }
    double currentLeft, currentRight;
    if (
      !currentIntervalWithVariable[2][1].evaluatesToDouble(&currentLeft)
    ) {
      return
      calculator
      << "Could not get a double from "
      << currentIntervalWithVariable[2][1].toString();
    }
    if (
      !currentIntervalWithVariable[2][2].evaluatesToDouble(&currentRight)
    ) {
      return
      calculator
      << "Could not get a double from "
      << currentIntervalWithVariable[2][2].toString();
    }
    if (boundaryVariables.contains(currentIntervalWithVariable[1])) {
      return
      calculator
      << "Expression "
      << currentIntervalWithVariable[1].toString()
      << " specified an interval range more than once. ";
    }
    boundaryVariables.addOnTop(currentIntervalWithVariable[1]);
    leftBoundaries.addOnTop(currentLeft);
    rightBoundaries.addOnTop(currentRight);
    numSamples.addOnTop(10);
    int currentNumberOfSamplingPoints = 0;
    if (i + 1 < input.size()) {
      if (input[i + 1].isSmallInteger(&currentNumberOfSamplingPoints)) {
        *numSamples.lastObject() = currentNumberOfSamplingPoints;
      } else {
        return
        calculator
        << "Failed to extract a small integer "
        << "for the number of sampling points from: "
        << input[i + 1].toString();
      }
      if (*numSamples.lastObject() > 1000) {
        return
        calculator
        << *numSamples.lastObject()
        << " sampling points requested for variable/expression "
        << boundaryVariables.lastObject()->toString()
        << "; this exceeds the hard-coded limit of 1000. ";
      }
      if (*numSamples.lastObject() < 1) {
        return
        calculator
        << *numSamples.lastObject()
        << " sampling points requested for variable/expression "
        << boundaryVariables.lastObject()->toString()
        << "; this is not allowed. ";
      }
    }
  }
  for (int i = 0; i < variables.size; i ++) {
    if (!boundaryVariables.contains(variables[i])) {
      return
      calculator
      << "Could not find an interval for variable/expression: "
      << variables[i].toString();
    }
  }
  HashedList<Expression> knownEs = calculator.knownDoubleConstants;
  List<double> knownValues = calculator.knownDoubleConstantValues;
  for (int i = 0; i < boundaryVariables.size; i ++) {
    if (knownEs.contains(boundaryVariables[i])) {
      return
      calculator
      << boundaryVariables[i]
      << " is an already known constant and "
      << "cannot be used as a variable in this context. ";
    } else {
      knownEs.addOnTop(boundaryVariables[i]);
    }
  }
  knownValues.setSize(knownEs.size);
  SelectionWithDifferentMaxMultiplicities samplingSelector;
  samplingSelector.initializeFromIntegers(numSamples);
  if (samplingSelector.totalNumberOfSubsets() > 1000000) {
    return
    calculator
    << "The total number of sampling points, "
    << samplingSelector.totalNumberOfSubsets().toString()
    << " exceeds "
    << "the hard-coded limit of 1000000. ";
  }
  double tolerance = 0.0001;
  if (!input[3].evaluatesToDouble(&tolerance)) {
    return
    calculator
    << "Failed to evaluate tolerance limit from "
    << input[3].toString()
    << " to a floating point number. ";
  }
  if (tolerance < 0) {
    tolerance *= - 1;
  }
  Rational totalSamples = samplingSelector.totalNumberSubsetsSmallInt();
  Rational numFailedSamples = 0;
  do {
    for (int i = 0; i < samplingSelector.multiplicities.size; i ++) {
      double& currentValue =
      knownValues[i + calculator.knownDoubleConstants.size];
      double& lBound = leftBoundaries[i];
      double& rBound = rightBoundaries[i];
      if (samplingSelector.capacities[i] == 1) {
        currentValue = (lBound + rBound) / 2;
        continue;
      }
      double paramBnZeroAndOne = samplingSelector.multiplicities[i];
      paramBnZeroAndOne /= samplingSelector.capacities[i] - 1;
      currentValue =
      lBound *(1 - paramBnZeroAndOne) + rBound * paramBnZeroAndOne;
    }
    double floatingResult = 0;
    if (
      !functionExpression.evaluatesToDoubleUnderSubstitutions(
        knownEs, knownValues, &floatingResult
      )
    ) {
      numFailedSamples ++;
      if ((numFailedSamples* 100) / totalSamples > 20) {
        calculator
        << "Failed to evaluate at least "
        << "one of the functions in more than "
        << "20% of the sampling points. ";
        return output.assignValue(calculator, 0);
      }
    }
    if (floatingResult > tolerance || floatingResult < - tolerance) {
      return output.assignValue(calculator, 0);
    }
  } while (samplingSelector.incrementReturnFalseIfPastLast());
  return output.assignValue(calculator, 1);
}

bool CalculatorFunctions::compositeArithmeticOperationEvaluatedOnArgument(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctions::compositeArithmeticOperationEvaluatedOnArgument"
  );
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

bool CalculatorFunctions::isEven(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::isEven");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (argument.hasBoundVariables()) {
    return false;
  }
  LargeInteger integerValue;
  if (!argument.isInteger(&integerValue)) {
    return output.assignValue(calculator, 0);
  }
  if (integerValue.isEven()) {
    return output.assignValue(calculator, 1);
  }
  return output.assignValue(calculator, 0);
}

bool CalculatorFunctions::isConstant(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::isConstant");
  if (input.size() != 2) {
    return false;
  }
  int result = static_cast<int>(input[1].isConstantNumber());
  return output.assignValue(calculator, result);
}

bool CalculatorFunctions::isNonEmptySequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::isNonEmptySequence");
  if (input.hasBoundVariables()) {
    return false;
  }
  if (input.size() < 2) {
    return output.assignValue(calculator, 0);
  }
  if (input.size() == 2) {
    if (input[1].isSequenceNElements() && input[1].size() > 0) {
      return output.assignValue(calculator, 1);
    }
    return output.assignValue(calculator, 0);
  }
  return output.assignValue(calculator, 1);
}

bool CalculatorFunctions::differentialStandardHandler(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::differentialStandardHandler");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  output.reset(calculator);
  output.addChildAtomOnTop(calculator.opDifferential());
  output.addChildOnTop(argument);
  return output.addChildRationalOnTop(1);
}

bool CalculatorFunctions::differentialOfPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::differentialOfPolynomial");
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
      calculator, differentials[i], polynomial.context
    );
    incoming.addChildOnTop(polynomialWrapper);
    outputSummands.addOnTop(incoming);
  }
  return output.makeSum(calculator, outputSummands);
}

bool CalculatorFunctions::isDifferentialOneFormOneVariable(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::isDifferentialOneFormOneVariable");
  if (input.size() != 2) {
    return false;
  }
  return
  output.assignValue(
    calculator,
    static_cast<int>(input[1].isDifferentialOneFormOneVariable())
  );
}

bool CalculatorFunctionsDifferentiation::atomToDifferential(
  Calculator& calculator, const Expression& input, Expression& output
) {
  std::string differentialString;
  if (!input.isOperation(&differentialString)) {
    return false;
  }
  if (differentialString.size() <= 1) {
    return false;
  }
  if (differentialString[0] != 'd') {
    return false;
  }
  Expression underTheDifferential;
  underTheDifferential.makeAtom(
    calculator, differentialString.substr(1, std::string::npos)
  );
  return
  output.makeXOX(
    calculator,
    calculator.opDifferential(),
    underTheDifferential,
    calculator.expressionOne()
  );
}

bool CalculatorFunctionsDifferentiation::interpretAsDifferential(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::interpretAsDifferential");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (!input[1].startsWith(calculator.opIntegral())) {
    return false;
  }
  const Expression& differentialFormCandidate = input[2];
  Expression differentialForm;
  if (
    CalculatorFunctionsDifferentiation::atomToDifferential(
      calculator, differentialFormCandidate, differentialForm
    )
  ) {
    output = input;
    output.setChild(2, differentialForm);
    return true;
  }
  if (!differentialFormCandidate.startsWithGivenOperation("*", 3)) {
    return false;
  }
  if (!differentialFormCandidate[1].isOperationGiven("d")) {
    return false;
  }
  differentialForm.makeXOX(
    calculator,
    calculator.opDifferential(),
    differentialFormCandidate[2],
    calculator.expressionOne()
  );
  output = input;
  output.setChild(2, differentialForm);
  return true;
}

bool CalculatorFunctionsIntegration::integralOperator(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsIntegration::integralOperator");
  int operatorIndex = calculator.opTimes();
  if (!input.startsWith(operatorIndex, 3)) {
    operatorIndex = calculator.opDivide();
    if (!input.startsWith(operatorIndex, 3)) {
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
    output.setChild(1, integralOperatorE);
    return true;
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
  if (incomingIntegrand.isOperationGiven("d")) {
    // We have an expression of the form \int x d x.
    return false;
  }
  Expression newIntegrand;
  Expression newFun;
  if (incomingIntegrand.isDifferentialOneFormOneVariable()) {
    newFun.makeXOX(
      calculator, operatorIndex, startingIntegrand, incomingIntegrand[2]
    );
    newIntegrand.makeXOX(
      calculator,
      calculator.opDifferential(),
      incomingIntegrand[1],
      newFun
    );
    output = integralE;
    output.setChild(2, newIntegrand);
    return true;
  }
  newIntegrand.makeXOX(
    calculator, operatorIndex, startingIntegrand, incomingIntegrand
  );
  output = integralE;
  output.setChild(2, newIntegrand);
  return true;
}

bool CalculatorFunctions::rationalFunctionSubstitution(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::rationalFunctionSubstitution");
  if (input.size() != 2) {
    return false;
  }
  if (!input[0].isOfType<RationalFraction<Rational> >()) {
    return false;
  }
  if (
    input[0].getValue<RationalFraction<Rational> >().minimalNumberOfVariables()
    >
    1
  ) {
    return false;
  }
  Expression resultRationalForm;
  ExpressionContext finalContext(calculator);
  finalContext.makeOneVariable(input[1]);
  resultRationalForm.assignValueWithContext(
    calculator,
    input[0].getValue<RationalFraction<Rational> >(),
    finalContext
  );
  return
  CalculatorConversions::expressionFromRationalFraction<Rational>(
    calculator, resultRationalForm, output
  );
}

bool CalculatorFunctions::invertMatrixRationalFractionsVerbose(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("Calculator::invertMatrixVerbose");
  Matrix<RationalFraction<Rational> > matrix, outputMatrix, extendedMatrix;
  Expression converted;
  if (
    !CalculatorConversions::matrixRationalFunction(
      calculator, input, converted
    )
  ) {
    return output.assignError(calculator, "Failed to extract matrix. ");
  }
  if (
    CalculatorConversions::functionGetMatrixNoComputation(
      calculator, converted, matrix
    )
  ) {
    return calculator << "Failed to get matrix of rational functions. ";
  }
  ExpressionContext context = converted.getContext();
  if (
    matrix.numberOfRows != matrix.numberOfColumns || matrix.numberOfColumns < 1
  ) {
    std::stringstream out;
    out
    << "The matrix "
    << matrix.toString()
    << " has "
    << matrix.numberOfColumns
    << " columns and "
    << matrix.numberOfRows
    << " rows. "
    << "The matrix is not square.";
    return output.assignError(calculator, out.str());
  }
  outputMatrix.makeIdentityMatrix(
    matrix.numberOfRows,
    RationalFraction<Rational>::oneRational(),
    RationalFraction<Rational>::zeroRational()
  );
  int numberOfFoundPivots = 0;
  std::stringstream out, outLaTeX;
  RationalFraction<Rational> tempElement;
  FormatExpressions format;
  context.getFormat(format);
  format.flagUseLatex = true;
  format.flagUseHTML = false;
  format.flagUseFrac = true;
  format.matrixColumnVerticalLineIndex = matrix.numberOfColumns - 1;
  out
  << "Computing "
  << HtmlRoutines::getMathNoDisplay(matrix.toString(&format) + "^{- 1}");
  extendedMatrix = matrix;
  extendedMatrix.appendMatrixOnTheRight(outputMatrix);
  out
  << "<br>"
  << HtmlRoutines::getMathNoDisplay(extendedMatrix.toString(&format));
  outLaTeX << "\\begin{tabular}{ll}";
  outLaTeX << "$" << extendedMatrix.toString(&format) << "$";
  int pivotIndex = 0;
  for (int i = 0; i < matrix.numberOfColumns; i ++) {
    pivotIndex = matrix.findPivot(i, numberOfFoundPivots);
    if (pivotIndex != - 1) {
      if (pivotIndex != numberOfFoundPivots) {
        matrix.switchRows(numberOfFoundPivots, pivotIndex);
        outputMatrix.switchRows(numberOfFoundPivots, pivotIndex);
        out
        << "<br>Swap row "
        << numberOfFoundPivots + 1
        << " and row "
        << pivotIndex + 1
        << ": ";
        outLaTeX
        << "& Swap row "
        << numberOfFoundPivots + 1
        << " and row "
        << pivotIndex + 1
        << ". ";
        extendedMatrix = matrix;
        extendedMatrix.appendMatrixOnTheRight(outputMatrix);
        out
        << "<br>"
        << HtmlRoutines::getMathNoDisplay(outputMatrix.toString(&format));
        outLaTeX << "\\\\" << "$" << outputMatrix.toString(&format) << "$";
      }
      tempElement = matrix(numberOfFoundPivots, i);
      tempElement.invert();
      if (tempElement != 1) {
        out
        << "<br> multiply row number "
        << numberOfFoundPivots + 1
        << " by "
        << tempElement.toString(&format)
        << ": ";
        outLaTeX
        << "& multiply row number "
        << numberOfFoundPivots + 1
        << " by $"
        << tempElement.toString(&format)
        << "$. \\\\";
      }
      matrix.rowTimesScalar(numberOfFoundPivots, tempElement);
      outputMatrix.rowTimesScalar(numberOfFoundPivots, tempElement);
      if (tempElement != 1) {
        extendedMatrix = matrix;
        extendedMatrix.appendMatrixOnTheRight(outputMatrix);
        out
        << HtmlRoutines::getMathNoDisplay(
          extendedMatrix.toString(&format)
        );
        outLaTeX << "$" << extendedMatrix.toString(&format) << "$";
      }
      bool found = false;
      for (int j = 0; j < matrix.numberOfRows; j ++) {
        if (j != numberOfFoundPivots) {
          if (!matrix.elements[j][i].isEqualToZero()) {
            tempElement = matrix.elements[j][i];
            tempElement.negate();
            matrix.addTwoRows(numberOfFoundPivots, j, i, tempElement);
            outputMatrix.addTwoRows(numberOfFoundPivots, j, 0, tempElement);
            if (!found) {
              out << "<br>";
              outLaTeX << "&";
              outLaTeX << "\\begin{tabular}{l}";
            } else {
              out << ", ";
              outLaTeX << ", ";
            }
            found = true;
            out
            << " Row index "
            << numberOfFoundPivots + 1
            << " times "
            << tempElement.toString(&format)
            << " added to row index "
            << j + 1;
            outLaTeX
            << " Row index "
            << numberOfFoundPivots + 1
            << " times $"
            << tempElement.toString(&format)
            << "$ added to row index "
            << j + 1
            << "\\\\";
          }
        }
      }
      if (found) {
        out << ": <br> ";
        outLaTeX << "\\end{tabular}";
        outLaTeX << "\\\\";
        extendedMatrix = matrix;
        extendedMatrix.appendMatrixOnTheRight(outputMatrix);
        out
        << HtmlRoutines::getMathNoDisplay(
          extendedMatrix.toString(&format)
        );
        outLaTeX << "$" << extendedMatrix.toString(&format) << "$";
      }
      numberOfFoundPivots ++;
    }
  }
  outLaTeX << "\\end{tabular}";
  format.matrixColumnVerticalLineIndex = - 1;
  if (numberOfFoundPivots < matrix.numberOfRows) {
    out
    << "<br>Matrix to the right of the vertical line not "
    << "transformed to the identity matrix => "
    << "starting matrix is not invertible. ";
    outLaTeX
    << "Matrix to the right of the vertical line "
    << "not transformed to the identity matrix => "
    << "starting matrix is not invertible. ";
  } else {
    out
    << "<br>The inverse of the starting matrix "
    << "can be read off on the matrix to the left of the id matrix: "
    << HtmlRoutines::getMathNoDisplay(outputMatrix.toString(&format));
    outLaTeX
    << " The inverse matrix can now be read off as the matrix "
    << "to the left of the identity matrix: $"
    << outputMatrix.toString(&format)
    << "$";
  }
  out << "Output in LaTeX: <br><br>" << outLaTeX.str();
  return output.assignValue<std::string>(calculator, out.str());
}

bool CalculatorFunctions::invertMatrixVerbose(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::invertMatrixVerbose");
  Expression converted;
  if (!CalculatorConversions::makeMatrix(calculator, input, converted)) {
    return calculator << "Failed to get matrix from: " << input.toString();
  }
  Matrix<Rational> matrix, outputMatrix, augmentedMatrix;
  if (
    !CalculatorConversions::functionGetMatrixNoComputation(
      calculator, converted, matrix
    )
  ) {
    return
    CalculatorFunctions::invertMatrixRationalFractionsVerbose(
      calculator, input, output
    );
  }
  if (
    matrix.numberOfRows != matrix.numberOfColumns || matrix.numberOfColumns < 1
  ) {
    return output.assignError(calculator, "The matrix is not square");
  }
  outputMatrix.makeIdentityMatrix(matrix.numberOfRows);
  int numberOfFoundPivots = 0;
  std::stringstream out;
  Rational tempElement;
  FormatExpressions format;
  format.flagUseLatex = true;
  format.flagUseHTML = false;
  format.matrixColumnVerticalLineIndex = matrix.numberOfColumns - 1;
  out
  << "Computing "
  << HtmlRoutines::getMathNoDisplay(matrix.toString(&format) + "^{- 1}");
  augmentedMatrix = matrix;
  augmentedMatrix.appendMatrixOnTheRight(outputMatrix);
  out
  << "<br>"
  << HtmlRoutines::getMathNoDisplay(augmentedMatrix.toString(&format));
  int pivotIndex = 0;
  for (int i = 0; i < matrix.numberOfColumns; i ++) {
    pivotIndex = matrix.findPivot(i, numberOfFoundPivots);
    if (pivotIndex != - 1) {
      if (pivotIndex != numberOfFoundPivots) {
        matrix.switchRows(numberOfFoundPivots, pivotIndex);
        outputMatrix.switchRows(numberOfFoundPivots, pivotIndex);
        out
        << "<br>switch row "
        << numberOfFoundPivots + 1
        << " and row "
        << pivotIndex + 1
        << ": ";
        augmentedMatrix = matrix;
        augmentedMatrix.appendMatrixOnTheRight(outputMatrix);
        out
        << "<br>"
        << HtmlRoutines::getMathNoDisplay(outputMatrix.toString(&format));
      }
      tempElement = matrix.elements[numberOfFoundPivots][i];
      tempElement.invert();
      if (tempElement != 1) {
        out
        << "<br> multiply row "
        << numberOfFoundPivots + 1
        << " by "
        << tempElement
        << ": ";
      }
      matrix.rowTimesScalar(numberOfFoundPivots, tempElement);
      outputMatrix.rowTimesScalar(numberOfFoundPivots, tempElement);
      if (tempElement != 1) {
        augmentedMatrix = matrix;
        augmentedMatrix.appendMatrixOnTheRight(outputMatrix);
        out
        << HtmlRoutines::getMathNoDisplay(
          augmentedMatrix.toString(&format)
        );
      }
      bool found = false;
      for (int j = 0; j < matrix.numberOfRows; j ++) {
        if (j != numberOfFoundPivots) {
          if (!matrix.elements[j][i].isEqualToZero()) {
            tempElement = matrix.elements[j][i];
            tempElement.negate();
            matrix.addTwoRows(numberOfFoundPivots, j, i, tempElement);
            outputMatrix.addTwoRows(numberOfFoundPivots, j, 0, tempElement);
            if (!found) {
              out << "<br>";
            } else {
              out << ", ";
            }
            found = true;
            out
            << " Row index "
            << numberOfFoundPivots + 1
            << " times "
            << tempElement
            << " added to row index "
            << j + 1;
          }
        }
      }
      if (found) {
        out << ": <br> ";
        augmentedMatrix = matrix;
        augmentedMatrix.appendMatrixOnTheRight(outputMatrix);
        out
        << HtmlRoutines::getMathNoDisplay(
          augmentedMatrix.toString(&format)
        );
      }
      numberOfFoundPivots ++;
    }
  }
  if (numberOfFoundPivots < matrix.numberOfRows) {
    out
    << "<br>Matrix to the right of the vertical line not "
    << "transformed to the identity matrix => "
    << "starting matrix is not invertible. ";
  } else {
    out
    << "<br>The inverse of the starting matrix can "
    << "be read off on the matrix to the left of the id matrix: "
    << HtmlRoutines::getMathNoDisplay(outputMatrix.toString(&format));
  }
  return output.assignValue<std::string>(calculator, out.str());
}

bool CalculatorFunctions::functionPolynomialize(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::functionPolynomialize");
  Expression polynomialExpression;
  if (input.hasBoundVariables()) {
    return false;
  }
  WithContext<Polynomial<AlgebraicNumber> > outputWithContext;
  if (
    !CalculatorConversions::functionPolynomial<AlgebraicNumber>(
      calculator, input, outputWithContext, - 1, - 1, true
    )
  ) {
    return false;
  }
  return
  CalculatorConversions::expressionFromPolynomial(
    calculator,
    outputWithContext.content,
    output,
    &outputWithContext.context
  );
}

bool CalculatorFunctions::polynomialize(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (input.size() != 2) {
    return false;
  }
  if (
    CalculatorFunctions::functionPolynomialize(
      calculator, input[1], output
    )
  ) {
    return true;
  }
  output = input[1];
  return true;
}

bool CalculatorFunctionsIntegration::
integrateRationalFunctionSplitToBuidingBlocks(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsIntegration::"
    "integrateRationalFunctionSplitToBuidingBlocks"
  );
  Expression functionExpression, variableExpression, integrationSetE;
  if (
    !input.isIndefiniteIntegralFdx(
      &variableExpression, &functionExpression, &integrationSetE
    )
  ) {
    return false;
  }
  IntegralRationalFunctionComputation computation(&calculator);
  computation.inputIntegrand = functionExpression;
  if (
    !CalculatorConversions::functionRationalFraction<Rational>(
      calculator, functionExpression, computation.rationalFraction, false
    )
  ) {
    return
    calculator
    << "<hr>Call of function CalculatorConversions::functionRationalFraction "
    << "failed, input was: "
    << functionExpression.toString();
  }
  ExpressionContext context = computation.rationalFraction.context;
  if (context.numberOfVariables() > 1) {
    return
    calculator
    << "<hr>I converted "
    << functionExpression.toString()
    << " to rational function, but it is of "
    << context.numberOfVariables()
    << " variables. I have been taught to work with 1 variable only. "
    << "<br>The context of the rational function is: "
    << context.toString();
  }
  if (context.numberOfVariables() == 1) {
    if (context.getVariable(0) != variableExpression) {
      return
      calculator
      << "<hr>The univariate rational function uses context "
      << context.toString()
      << " but the variable of integration is "
      << variableExpression.toString();
    }
  }
  computation.integrationSetE = integrationSetE;
  computation.rationalFraction.content.getDenominator(
    computation.denominator
  );
  computation.rationalFraction.content.getNumerator(computation.numerator);
  if (computation.denominator.totalDegree() < 1) {
    return false;
  }
  if (!computation.integrateRationalFunction()) {
    return calculator << computation.printoutIntegration.str();
  }
  computation.integralSum.checkConsistencyRecursively();
  output = computation.integralSum;
  if (output.startsWith(calculator.opIntegral())) {
    if (output[1] == input) {
      return false;
    }
  }
  output.checkConsistencyRecursively();
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctions::coefficientsPowersOf(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::coefficientsPowersOf");
  if (input.size() != 3) {
    return false;
  }
  const Expression& variable = input[1];
  const Expression& processed = input[2];
  VectorSparse<Expression> coefficients;
  if (
    !calculator.collectCoefficientsPowersVariables(
      processed, variable, coefficients
    )
  ) {
    return
    calculator
    << "<hr>Failed to evaluate Calculator::collectCoefficientsPowersVariables";
  }
  int highestPowerPlus1 = coefficients.getLargestParticipatingBasisIndex() + 1;
  List<Expression> coefficientExpressionsIncludingZeros;
  Expression currentCF;
  for (int i = 0; i < highestPowerPlus1; i ++) {
    int index = coefficients.monomials.getIndex(MonomialVector(i));
    if (index == - 1) {
      currentCF.assignValue(calculator, 0);
    } else {
      currentCF = coefficients.coefficients[index];
    }
    coefficientExpressionsIncludingZeros.addOnTop(currentCF);
  }
  return
  output.makeSequence(calculator, &coefficientExpressionsIncludingZeros);
}

bool CalculatorFunctions::constantTermRelative(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::constantTermRelative");
  if (!input.startsWithGivenOperation("ConstantTerm")) {
    return false;
  }
  Expression coefficientExtractor, coefficients;
  coefficientExtractor = input;
  coefficientExtractor.setChildAtomValue(0, "ConstantTerm");
  if (
    !CalculatorFunctions::coefficientsPowersOf(
      calculator, coefficientExtractor, coefficients
    )
  ) {
    return false;
  }
  if (coefficients.isSequenceNElements()) {
    if (coefficients.size() > 1) {
      output = coefficients[1];
      return true;
    }
  }
  return false;
}

bool CalculatorFunctionsIntegration::integrateRationalFunctionBuildingBlockIa(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsIntegration::integrateRationalFunctionBuildingBlockIa"
  );
  Expression functionExpression, variable;
  if (!input.isIndefiniteIntegralFdx(&variable, &functionExpression)) {
    return false;
  }
  if (!functionExpression.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  const Expression& A = functionExpression[1];
  Expression a, b;
  const Expression& axPlusb = functionExpression[2];
  if (!A.isConstantNumber()) {
    return false;
  }
  if (
    !CalculatorFunctions::extractLinearCoefficientsWithRespectToVariable(
      axPlusb, variable, a, b
    )
  ) {
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
  STACK_TRACE(
    "CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIb"
  );
  Expression functionExpression, variable;
  if (!input.isIndefiniteIntegralFdx(&variable, &functionExpression)) {
    return false;
  }
  if (!functionExpression.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  const Expression& A = functionExpression[1];
  const Expression& axPlusbPowerN = functionExpression[2];
  if (!A.isConstantNumber()) {
    return false;
  }
  if (!axPlusbPowerN.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  Expression N = axPlusbPowerN[2];
  if (!N.isConstantNumber()) {
    return false;
  }
  Expression a, b;
  const Expression& axPlusb = axPlusbPowerN[1];
  if (
    !CalculatorFunctions::extractLinearCoefficientsWithRespectToVariable(
      axPlusb, variable, a, b
    )
  ) {
    return false;
  }
  if (!a.isConstantNumber() || !b.isConstantNumber()) {
    return false;
  }
  Expression base, OneMinusN;
  OneMinusN = N;
  OneMinusN += - 1;
  OneMinusN *= - 1;
  base.makeXOX(calculator, calculator.opPower(), axPlusb, OneMinusN);
  output = A;
  output /= a;
  output /= OneMinusN;
  output *= base;
  output.checkConsistencyRecursively();
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctions::extractQuadraticCoefficientsWithRespectToVariable(
  const Expression& quadratic,
  const Expression& variable,
  Expression& outputCoeffVarSquared,
  Expression& outputCoeffLinTerm,
  Expression& outputConstTerm
) {
  STACK_TRACE("CalculatorFunctions::extractQuadraticCoeffsWRTvariable");
  VectorSparse<Expression> coefficients;
  if (quadratic.owner == nullptr) {
    return false;
  }
  Calculator& calculator = *quadratic.owner;
  if (
    !calculator.collectCoefficientsPowersVariables(
      quadratic, variable, coefficients
    )
  ) {
    return false;
  }
  if (coefficients.getLargestParticipatingBasisIndex() != 2) {
    return false;
  }
  outputCoeffVarSquared.assignValue(calculator, 0);
  outputCoeffLinTerm.assignValue(calculator, 0);
  outputConstTerm.assignValue(calculator, 0);
  if (coefficients.monomials.contains(MonomialVector(0))) {
    outputConstTerm = coefficients.getCoefficientOf(MonomialVector(0));
  }
  if (coefficients.monomials.contains(MonomialVector(1))) {
    outputCoeffLinTerm = coefficients.getCoefficientOf(MonomialVector(1));
  }
  if (coefficients.monomials.contains(MonomialVector(2))) {
    outputCoeffVarSquared = coefficients.getCoefficientOf(MonomialVector(2));
  }
  return true;
}

bool CalculatorFunctions::extractLinearCoefficientsWithRespectToVariable(
  const Expression& linearExpression,
  const Expression& variable,
  Expression& outputCoeffLinTerm,
  Expression& outputConstTerm
) {
  STACK_TRACE(
    "CalculatorFunctions::extractLinearCoefficientsWithRespectToVariable"
  );
  VectorSparse<Expression> coefficients;
  if (linearExpression.owner == nullptr) {
    return false;
  }
  Calculator& calculator = *linearExpression.owner;
  calculator.collectCoefficientsPowersVariables(
    linearExpression, variable, coefficients
  );
  if (coefficients.getLargestParticipatingBasisIndex() > 1) {
    return false;
  }
  outputCoeffLinTerm.assignValue(calculator, 0);
  outputConstTerm.assignValue(calculator, 0);
  if (coefficients.monomials.contains(MonomialVector(1))) {
    outputCoeffLinTerm = coefficients.getCoefficientOf(MonomialVector(1));
  }
  if (coefficients.monomials.contains(MonomialVector(0))) {
    outputConstTerm = coefficients.getCoefficientOf(MonomialVector(0));
  }
  return true;
}

bool CalculatorFunctionsIntegration::
integrateRationalFunctionBuidingBlockIIaandIIIa(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsIntegration::"
    "integrateRationalFunctionBuidingBlockIIaandIIIa"
  );
  Expression functionExpression, x;
  if (!input.isIndefiniteIntegralFdx(&x, &functionExpression)) {
    return false;
  }
  if (!functionExpression.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  Expression denNoPower = functionExpression[2];
  Expression a, b, c, A, B;
  if (
    !CalculatorFunctions::extractQuadraticCoefficientsWithRespectToVariable(
      denNoPower, x, a, b, c
    )
  ) {
    return false;
  }
  if (
    !CalculatorFunctions::extractLinearCoefficientsWithRespectToVariable(
      functionExpression[1], x, A, B
    )
  ) {
    return false;
  }
  if (
    !a.isConstantNumber() ||
    !b.isConstantNumber() ||
    !c.isConstantNumber() ||
    !A.isConstantNumber() ||
    !B.isConstantNumber()
  ) {
    return
    calculator
    << "<hr>Failed to evaluate to constant "
    << "the coefficients of the block IIa and IIIa integral. "
    << "The coefficients are: "
    << a.toString()
    << ", "
    << b.toString()
    << ", "
    << c.toString()
    << ", "
    << A.toString()
    << ", "
    << B.toString()
    << ". ";
  }
  double approxa, approxb, approxc;
  if (
    !a.evaluatesToDouble(&approxa) ||
    !b.evaluatesToDouble(&approxb) ||
    !c.evaluatesToDouble(&approxc)
  ) {
    return
    calculator
    << "<hr>Failed to evaluate "
    << "variable coefficients in denominator "
    << denNoPower.toString()
    << " to double. Possible user typo. ";
  }
  if (approxb * approxb >= approxa * approxc * 4) {
    return false;
  }
  Expression xSquared, bSquared, aSquared;
  Expression twoE, oneE, fourE;
  oneE.assignValue(calculator, 1);
  twoE.assignValue(calculator, 2);
  fourE.assignValue(calculator, 4);
  xSquared.makeXOX(calculator, calculator.opPower(), x, twoE);
  bSquared.makeXOX(calculator, calculator.opPower(), b, twoE);
  aSquared.makeXOX(calculator, calculator.opPower(), a, twoE);
  Expression quadraticDividedByA = xSquared + (b / a) * x + c / a;
  Expression xplusbdiv2a = x + b / (twoE* a);
  Expression F = (fourE * a * c - bSquared) / (fourE* aSquared);
  Expression sqrtF;
  sqrtF.makeSqrt(calculator, F, 2);
  Expression arcTanArgument = xplusbdiv2a / sqrtF;
  Expression arctangentExpressino(calculator);
  arctangentExpressino.addChildAtomOnTop(calculator.opArcTan());
  arctangentExpressino.addChildOnTop(arcTanArgument);
  Expression logarithmExpression(calculator);
  logarithmExpression.addChildAtomOnTop(calculator.opLog());
  logarithmExpression.addChildOnTop(quadraticDividedByA);
  Expression C = B -(A* b) / (twoE* a);
  output = (oneE / a) *((A / twoE) * logarithmExpression + (C / sqrtF) *
    arctangentExpressino
  );
  output.checkConsistencyRecursively();
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIIb(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIIb"
  );
  Expression functionExpression, x, integrationSetE;
  if (
    !input.isIndefiniteIntegralFdx(
      &x, &functionExpression, &integrationSetE
    )
  ) {
    return false;
  }
  if (!functionExpression.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (!functionExpression[2].startsWith(calculator.opPower(), 3)) {
    return false;
  }
  Expression numPowerE = functionExpression[2][2];
  int numPower = 0;
  if (!numPowerE.isSmallInteger(&numPower)) {
    return false;
  }
  if (numPower <= 1) {
    return false;
  }
  Expression denNoPower = functionExpression[2][1];
  Expression a, b, c;
  if (!functionExpression[1].isEqualToOne()) {
    return false;
  }
  if (
    !CalculatorFunctions::extractQuadraticCoefficientsWithRespectToVariable(
      denNoPower, x, a, b, c
    )
  ) {
    return false;
  }
  if (
    !a.isEqualToOne() || !b.isConstantNumber() || !c.isConstantNumber()
  ) {
    calculator
    << "<hr>Failed to evaluate to constant the "
    << "coefficients of the block IIIb integral."
    << "The coefficients are: "
    << a.toString()
    << ", "
    << b.toString()
    << ", "
    << c.toString()
    << ". ";
    return false;
  }
  double approxa, approxb, approxc;
  if (
    !a.evaluatesToDouble(&approxa) ||
    !b.evaluatesToDouble(&approxb) ||
    !c.evaluatesToDouble(&approxc)
  ) {
    calculator
    << "Failed to evaluate variable coefficients in denominator "
    << denNoPower.toString()
    << " to double. Possible user typo. ";
    return false;
  }
  if (approxb * approxb >= approxa * approxc * 4) {
    return false;
  }
  Expression xSquared, bSquared;
  Expression twoE, oneE, threeE, fourE;
  oneE.assignValue(calculator, 1);
  twoE.assignValue(calculator, 2);
  threeE.assignValue(calculator, 3);
  fourE.assignValue(calculator, 4);
  xSquared.makeXOX(calculator, calculator.opPower(), x, twoE);
  bSquared.makeXOX(calculator, calculator.opPower(), b, twoE);
  Expression monicQuadratic = xSquared + b * x + c;
  Expression D = c - bSquared / fourE;
  Expression
  remainingIntegral,
  functionRemainingToIntegrate,
  quadraticPowerOneMinusN,
  quadraticPowerNMinusOne;
  quadraticPowerOneMinusN.makeXOX(
    calculator, calculator.opPower(), monicQuadratic, oneE - numPowerE
  );
  quadraticPowerNMinusOne.makeXOX(
    calculator, calculator.opPower(), monicQuadratic, numPowerE - oneE
  );
  functionRemainingToIntegrate = oneE / quadraticPowerNMinusOne;
  remainingIntegral.makeIntegral(
    calculator, integrationSetE, functionRemainingToIntegrate, x
  );
  output = oneE / D *((x + b / twoE) / (twoE * numPowerE - twoE) *
    quadraticPowerOneMinusN + (twoE * numPowerE - threeE) / (
      twoE * numPowerE - twoE
    ) *
    remainingIntegral
  );
  output.checkConsistencyRecursively();
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIb(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsIntegration::integrateRationalFunctionBuidingBlockIIb"
  );
  Expression functionExpression, x, integrationSetE;
  if (
    !input.isIndefiniteIntegralFdx(
      &x, &functionExpression, &integrationSetE
    )
  ) {
    return false;
  }
  if (!functionExpression.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  if (!functionExpression[2].startsWith(calculator.opPower(), 3)) {
    return false;
  }
  Expression nE = functionExpression[2][2];
  int numPower = 0;
  if (!nE.isSmallInteger(&numPower)) {
    return false;
  }
  if (numPower <= 1) {
    return false;
  }
  Expression denNoPower = functionExpression[2][1];
  Expression a, b, c, A, B;
  if (
    !CalculatorFunctions::extractQuadraticCoefficientsWithRespectToVariable(
      denNoPower, x, a, b, c
    )
  ) {
    return false;
  }
  if (
    !CalculatorFunctions::extractLinearCoefficientsWithRespectToVariable(
      functionExpression[1], x, A, B
    )
  ) {
    return false;
  }
  if (A.isEqualToZero()) {
    // building block is of type IIIb
    return false;
  }
  if (
    !a.isConstantNumber() ||
    !b.isConstantNumber() ||
    !c.isConstantNumber() ||
    !A.isConstantNumber() ||
    !B.isConstantNumber()
  ) {
    calculator
    << "<hr>Failed to evaluate to constant "
    << "the coefficients of the block IIb integral."
    << "The coefficients are: "
    << a.toString()
    << ", "
    << b.toString()
    << ", "
    << c.toString()
    << ", "
    << A.toString()
    << ", "
    << B.toString()
    << ". ";
    return false;
  }
  double approxa, approxb, approxc;
  if (
    !a.evaluatesToDouble(&approxa) ||
    !b.evaluatesToDouble(&approxb) ||
    !c.evaluatesToDouble(&approxc)
  ) {
    calculator
    << "Failed to evaluate variable coefficients in denominator "
    << denNoPower.toString()
    << " to double. Possible user typo. ";
    return false;
  }
  if (approxb * approxb >= approxa * approxc * 4) {
    return false;
  }
  Expression xSquared, bSquared, aSquared, apowerN;
  Expression twoE, oneE, fourE;
  Expression
  remainingIntegral,
  remainingFunctionToIntegrate,
  quadraticPowerN,
  quadraticPowerOneMinusN;
  oneE.assignValue(calculator, 1);
  twoE.assignValue(calculator, 2);
  fourE.assignValue(calculator, 4);
  apowerN.makeXOX(calculator, calculator.opPower(), a, nE);
  xSquared.makeXOX(calculator, calculator.opPower(), x, twoE);
  bSquared.makeXOX(calculator, calculator.opPower(), b, twoE);
  aSquared.makeXOX(calculator, calculator.opPower(), a, twoE);
  Expression quadraticDividedByA = xSquared + (b / a) * x + c / a;
  quadraticPowerN.makeXOX(
    calculator, calculator.opPower(), quadraticDividedByA, nE
  );
  quadraticPowerOneMinusN.makeXOX(
    calculator, calculator.opPower(), quadraticDividedByA, oneE - nE
  );
  remainingFunctionToIntegrate = oneE / quadraticPowerN;
  remainingIntegral.makeIntegral(
    calculator, integrationSetE, remainingFunctionToIntegrate, x
  );
  Expression C = B -(A* b) / (twoE* a);
  oneE.checkInitializationRecursively();
  apowerN.checkInitializationRecursively();
  A.checkInitializationRecursively();
  twoE.checkInitializationRecursively();
  nE.checkInitializationRecursively();
  quadraticPowerOneMinusN.checkInitializationRecursively();
  C.checkInitializationRecursively();
  remainingIntegral.checkInitializationRecursively();
  output = (oneE / apowerN) *(
    A / (twoE *(oneE - nE)) * quadraticPowerOneMinusN + C * remainingIntegral
  );
  output.checkConsistencyRecursively();
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctionsIntegration::integratePowerByUncoveringParenthesisFirst
(Calculator& calculator, const Expression& input, Expression& output) {
  STACK_TRACE(
    "CalculatorFunctionsIntegration::integratePowerByUncoveringParenthesisFirst"
  );
  Expression
  functionExpression,
  integrandE,
  newIntegralE,
  variableExpression,
  integrationSetE;
  if (
    !input.isIndefiniteIntegralFdx(
      &variableExpression, &functionExpression, &integrationSetE
    )
  ) {
    return false;
  }
  if (!functionExpression.startsWith(calculator.opPower())) {
    return false;
  }
  if (
    !CalculatorFunctions::functionPolynomialize(
      calculator, functionExpression, integrandE
    )
  ) {
    return false;
  }
  if (integrandE == functionExpression) {
    return false;
  }
  newIntegralE.makeIntegral(
    calculator, integrationSetE, integrandE, variableExpression
  );
  if (!calculator.evaluateExpression(calculator, newIntegralE, output)) {
    return false;
  }
  if (
    output.containsAsSubExpressionNoBuiltInTypes(calculator.opIntegral())
  ) {
    return false;
  }
  output.checkConsistencyRecursively();
  output.checkInitializationRecursively();
  return true;
}

bool Expression::makeIntegral(
  Calculator& calculator,
  const Expression& integrationSet,
  const Expression& function,
  const Expression& variable
) {
  STACK_TRACE("Expression::makeIntegral");
  if (this == &function || this == &variable || this == &integrationSet) {
    Expression functionCopy = function;
    Expression variableCopy = variable;
    Expression integrationSetCopy = integrationSet;
    return
    this->makeIntegral(
      calculator, integrationSetCopy, functionCopy, variableCopy
    );
  }
  Expression differentialForm;
  differentialForm.makeXOX(
    calculator, calculator.opDifferential(), variable, function
  );
  this->reset(calculator);
  this->addChildAtomOnTop(calculator.opIntegral());
  this->addChildOnTop(integrationSet);
  return this->addChildOnTop(differentialForm);
}

bool CalculatorFunctionsIntegration::integratePullImaginaryUnit(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsIntegration::integratePullImaginaryUnit");
  Expression functionExpression, variableExpression, integrationSetE;
  if (
    !input.isIndefiniteIntegralFdx(
      &variableExpression, &functionExpression, &integrationSetE
    )
  ) {
    return false;
  }
  Expression iE;
  iE.makeAtom(calculator, calculator.opImaginaryUnit());
  if (variableExpression == iE) {
    return false;
  }
  Expression
  coefficientExpression,
  noCoefficientIntegrand,
  noImaginaryIntegrand,
  outputIntegralNoCF;
  functionExpression.getCoefficientMultiplicandForm(
    coefficientExpression, noCoefficientIntegrand
  );
  if (noCoefficientIntegrand == iE) {
    noImaginaryIntegrand.assignValue(calculator, 1);
  } else if (
    noCoefficientIntegrand.startsWith(calculator.opTimes(), 3)
  ) {
    if (noCoefficientIntegrand[1] != iE) {
      return false;
    }
    noImaginaryIntegrand = noCoefficientIntegrand[2];
  } else {
    return false;
  }
  coefficientExpression *= iE;
  outputIntegralNoCF.makeIntegral(
    calculator, integrationSetE, noImaginaryIntegrand, variableExpression
  );
  output = coefficientExpression * outputIntegralNoCF;
  return true;
}

bool CalculatorFunctionsIntegration::integrateSum(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsIntegration::integrateSum");
  Expression functionExpression, variableExpression, integrationSetE;
  if (
    !input.isIndefiniteIntegralFdx(
      &variableExpression, &functionExpression, &integrationSetE
    )
  ) {
    return false;
  }
  if (!functionExpression.startsWith(calculator.opPlus())) {
    return false;
  }
  List<Expression> integralsOfSummands;
  integralsOfSummands.setSize(functionExpression.size() - 1);
  Expression newIntegralE, result, newSummand;
  for (int i = 1; i < functionExpression.size(); i ++) {
    newIntegralE.makeIntegral(
      calculator,
      integrationSetE,
      functionExpression[i],
      variableExpression
    );
    if (
      !calculator.evaluateExpression(calculator, newIntegralE, newSummand)
    ) {
      return false;
    }
    if (
      newSummand.containsAsSubExpressionNoBuiltInTypes(
        calculator.opIntegral()
      )
    ) {
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

bool CalculatorFunctionsIntegration::integrateXnDiffX(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsIntegration::integrateXnDiffX");
  Expression functionExpression, variableExpression;
  if (
    !input.isIndefiniteIntegralFdx(&variableExpression, &functionExpression)
  ) {
    return false;
  }
  Expression functionCoefficient, functionNoCoefficient, outputNoCoeff;
  if (functionExpression.isConstantNumber()) {
    output = functionExpression * variableExpression;
    return true;
  }
  functionExpression.getCoefficientMultiplicandForm(
    functionCoefficient, functionNoCoefficient
  );
  if (functionNoCoefficient == variableExpression) {
    output = functionCoefficient * variableExpression * variableExpression;
    output /= 2;
    return true;
  }
  if (!functionNoCoefficient.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  if (functionNoCoefficient[1] != variableExpression) {
    return false;
  }
  if (!functionNoCoefficient[2].isConstantNumber()) {
    return false;
  }
  if (functionNoCoefficient[2].isEqualToMOne()) {
    outputNoCoeff.reset(calculator, 2);
    outputNoCoeff.addChildAtomOnTop(calculator.opLog());
    outputNoCoeff.addChildOnTop(variableExpression);
    output = functionCoefficient * outputNoCoeff;
    return true;
  }
  Expression outputPower = functionNoCoefficient[2];
  outputPower += 1;
  outputNoCoeff.makeXOX(
    calculator, calculator.opPower(), variableExpression, outputPower
  );
  outputNoCoeff /= outputPower;
  output = functionCoefficient * outputNoCoeff;
  output.checkConsistency();
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctionsIntegration::integrateSinPowerNCosPowerM(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsIntegration::integrateSinPowerNCosPowerM");
  Expression functionExpression, variableExpression, integrationSet;
  if (
    !input.isIndefiniteIntegralFdx(
      &variableExpression, &functionExpression, &integrationSet
    )
  ) {
    return false;
  }
  WithContext<Polynomial<Rational> > polynomial;
  if (
    !CalculatorConversions::functionPolynomial<Rational>(
      calculator, functionExpression, polynomial, - 1, - 1, true
    )
  ) {
    return false;
  }
  int numberOfVariables = polynomial.context.numberOfVariables();
  if (numberOfVariables > 2) {
    return false;
  }
  if (numberOfVariables == 0) {
    return false;
  }
  Expression sinPowerE, trigonometricArgument, cosPowerE;
  sinPowerE.assignValue(calculator, 1);
  cosPowerE.assignValue(calculator, 1);
  bool firstIsSine = false;
  for (int i = 0; i < numberOfVariables; i ++) {
    Expression currentE = polynomial.context.getVariable(i);
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
      trigonometricArgument = currentE[1];
    } else {
      if (trigonometricArgument != currentE[1]) {
        return false;
      }
    }
  }
  Expression
  trigonometricArgumentNoCoefficient,
  trigonometricArgumentCoefficient;
  trigonometricArgument.getCoefficientMultiplicandForm(
    trigonometricArgumentCoefficient, trigonometricArgumentNoCoefficient
  );
  if (trigonometricArgumentNoCoefficient != variableExpression) {
    return false;
  }
  const Polynomial<Rational>& trigonometricPolynomial = polynomial.content;
  Expression cosineExpression;
  Expression sineExpression;
  Expression cosineDoubleExpression;
  Expression trigonometricArgumentDouble;
  trigonometricArgumentDouble = trigonometricArgument;
  trigonometricArgumentDouble *= 2;
  cosineExpression.makeOX(
    calculator, calculator.opCos(), trigonometricArgument
  );
  sineExpression.makeOX(
    calculator, calculator.opSin(), trigonometricArgument
  );
  cosineDoubleExpression.makeOX(
    calculator, calculator.opCos(), trigonometricArgumentDouble
  );
  Expression
  outputCandidate,
  currentSummandE,
  currentCommandListE,
  currentSubE,
  currentIntegrandE,
  currentIntegrandNonPolynomializedE,
  currentIntegral,
  currentIntegralComputation,
  currentIntegrandSinePart,
  currentIntegrandCosinePart,
  newVarE,
  newResultE,
  currentE,
  currentCF;
  Expression oneE, twoE, threeE, powerE;
  oneE.assignValue(calculator, 1);
  twoE.assignValue(calculator, 2);
  threeE.assignValue(calculator, 3);
  outputCandidate.assignValue(calculator, 0);
  newVarE = calculator.getNewAtom();
  newResultE = calculator.getNewAtom();
  for (int i = 0; i < trigonometricPolynomial.size(); i ++) {
    const MonomialPolynomial& currentMon = trigonometricPolynomial[i];
    int powerSine = - 1, powerCosine = - 1;
    if (
      !currentMon(0).isSmallInteger(&powerSine) ||
      !currentMon(1).isSmallInteger(&powerCosine)
    ) {
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
      powerE.assignValue(calculator, (powerSine - 1) / 2);
      currentIntegrandSinePart.makeXOX(
        calculator, calculator.opPower(), currentE, powerE
      );
      powerE.assignValue(calculator, powerCosine);
      currentIntegrandCosinePart.makeXOX(
        calculator, calculator.opPower(), newVarE, powerE
      );
      currentCF.assignValue(
        calculator, - trigonometricPolynomial.coefficients[i]
      );
      currentCF /= trigonometricArgumentCoefficient;
      currentSubE.makeXOX(
        calculator, calculator.opDefine(), newVarE, cosineExpression
      );
    } else if (powerCosine % 2 == 1) {
      currentE = oneE - newVarE * newVarE;
      powerE.assignValue(calculator, (powerCosine - 1) / 2);
      currentIntegrandCosinePart.makeXOX(
        calculator, calculator.opPower(), currentE, powerE
      );
      powerE.assignValue(calculator, powerSine);
      currentIntegrandSinePart.makeXOX(
        calculator, calculator.opPower(), newVarE, powerE
      );
      currentCF.assignValue(
        calculator, trigonometricPolynomial.coefficients[i]
      );
      currentCF /= trigonometricArgumentCoefficient;
      currentSubE.makeXOX(
        calculator, calculator.opDefine(), newVarE, sineExpression
      );
    } else {
      currentE = (oneE - cosineDoubleExpression) / twoE;
      powerE.assignValue(calculator, powerSine / 2);
      currentIntegrandSinePart.makeXOX(
        calculator, calculator.opPower(), currentE, powerE
      );
      currentE = (oneE + cosineDoubleExpression) / twoE;
      powerE.assignValue(calculator, powerCosine / 2);
      currentIntegrandCosinePart.makeXOX(
        calculator, calculator.opPower(), currentE, powerE
      );
      currentCF.assignValue(
        calculator, trigonometricPolynomial.coefficients[i]
      );
      currentIntegrandNonPolynomializedE =
      currentCF * currentIntegrandSinePart * currentIntegrandCosinePart;
      currentIntegrandE.reset(calculator);
      currentIntegrandE.addChildAtomOnTop("Polynomialize");
      currentIntegrandE.addChildOnTop(currentIntegrandNonPolynomializedE);
      currentIntegral.makeIntegral(
        calculator, integrationSet, currentIntegrandE, variableExpression
      );
      outputCandidate += currentIntegral;
      continue;
    }
    currentIntegrandNonPolynomializedE =
    currentCF * currentIntegrandSinePart * currentIntegrandCosinePart;
    currentIntegrandE.reset(calculator);
    currentIntegrandE.addChildAtomOnTop("Polynomialize");
    currentIntegrandE.addChildOnTop(currentIntegrandNonPolynomializedE);
    currentIntegral.makeIntegral(
      calculator, integrationSet, currentIntegrandE, newVarE
    );
    currentIntegralComputation.makeXOX(
      calculator, calculator.opDefine(), newResultE, currentIntegral
    );
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
  STACK_TRACE("CalculatorFunctionsIntegration::integrateTanPowerNSecPowerM");
  Expression functionExpression, variableExpression, integrationSet;
  if (
    !input.isIndefiniteIntegralFdx(
      &variableExpression, &functionExpression, &integrationSet
    )
  ) {
    return false;
  }
  WithContext<Polynomial<Rational> > polynomial;
  if (
    !CalculatorConversions::functionPolynomial<Rational>(
      calculator, functionExpression, polynomial, - 1, - 1, true
    )
  ) {
    return false;
  }
  ExpressionContext context = polynomial.context;
  int numberOfVariables = context.numberOfVariables();
  if (numberOfVariables > 2) {
    return false;
  }
  if (numberOfVariables == 0) {
    return false;
  }
  Expression sinPowerE, trigonometricArgument, cosPowerE;
  sinPowerE.assignValue(calculator, 1);
  cosPowerE.assignValue(calculator, 1);
  bool firstIsTan = false;
  for (int i = 0; i < numberOfVariables; i ++) {
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
      trigonometricArgument = currentE[1];
    } else {
      if (trigonometricArgument != currentE[1]) {
        return false;
      }
    }
  }
  Expression
  trigonometricArgumentNoCoefficient,
  trigonometricArgumentCoefficient;
  trigonometricArgument.getCoefficientMultiplicandForm(
    trigonometricArgumentCoefficient, trigonometricArgumentNoCoefficient
  );
  if (trigonometricArgumentNoCoefficient != variableExpression) {
    return false;
  }
  const Polynomial<Rational>& trigonometrixPolynomial = polynomial.content;
  Expression tangentExpression, secantExpression;
  tangentExpression.makeOX(
    calculator, calculator.opTan(), trigonometricArgument
  );
  secantExpression.makeOX(
    calculator, calculator.opSec(), trigonometricArgument
  );
  Expression
  outputCandidate,
  currentSummandE,
  currentCommandListE,
  currentSubE,
  currentIntegrandE,
  currentIntegrandNonPolynomializedE,
  currentIntegral,
  currentIntegralComputation,
  currentIntegrandTanPart,
  currentIntegrandSecPart,
  newVarE,
  newResultE,
  currentE,
  currentCF;
  Expression oneE, twoE, threeE, powerE;
  oneE.assignValue(calculator, 1);
  twoE.assignValue(calculator, 2);
  threeE.assignValue(calculator, 3);
  outputCandidate.assignValue(calculator, 0);
  newVarE = calculator.getNewAtom();
  newResultE = calculator.getNewAtom();
  for (int i = 0; i < trigonometrixPolynomial.size(); i ++) {
    const MonomialPolynomial& currentMon = trigonometrixPolynomial[i];
    int powerTan = - 1, powerSec = - 1;
    if (
      !currentMon(0).isSmallInteger(&powerTan) ||
      !currentMon(1).isSmallInteger(&powerSec)
    ) {
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
      powerE.assignValue(calculator, (powerTan - 1) / 2);
      currentIntegrandTanPart.makeXOX(
        calculator, calculator.opPower(), currentE, powerE
      );
      powerE.assignValue(calculator, powerSec - 1);
      currentIntegrandSecPart.makeXOX(
        calculator, calculator.opPower(), newVarE, powerE
      );
      currentCF.assignValue(
        calculator, trigonometrixPolynomial.coefficients[i]
      );
      currentCF /= trigonometricArgumentCoefficient;
      currentSubE.makeXOX(
        calculator, calculator.opDefine(), newVarE, secantExpression
      );
    } else if (powerSec % 2 == 0) {
      currentE = oneE + newVarE * newVarE;
      powerE.assignValue(calculator, (powerSec - 2) / 2);
      currentIntegrandSecPart.makeXOX(
        calculator, calculator.opPower(), currentE, powerE
      );
      powerE.assignValue(calculator, powerTan);
      currentIntegrandTanPart.makeXOX(
        calculator, calculator.opPower(), newVarE, powerE
      );
      currentCF.assignValue(
        calculator, trigonometrixPolynomial.coefficients[i]
      );
      currentCF /= trigonometricArgumentCoefficient;
      currentSubE.makeXOX(
        calculator, calculator.opDefine(), newVarE, tangentExpression
      );
    } else {
      return false;
      /*currentE= (oneE-cosDoubleE)/twoE;
      powerE.assignValue(powerSine/2, calculator);
      currentIntegrandSinePart.makeXOX
      (calculator, calculator.opThePower(),currentE, powerE);
      currentE= (oneE+cosDoubleE)/twoE;
      powerE.assignValue(powerCosine/2, calculator);
      currentIntegrandCosinePart.makeXOX
      (calculator, calculator.opThePower(),currentE, powerE);
      currentCF.assignValue(trigPoly.coeffs[i], calculator);
      currentIntegrandNonPolynomializedE=
      currentCF*currentIntegrandSinePart*currentIntegrandCosinePart;
      currentIntegrandE.reset(calculator);
      currentIntegrandE.addChildAtomOnTop("Polynomialize");
      currentIntegrandE.addChildOnTop(currentIntegrandNonPolynomializedE);
      currentIntegral.makeIntegral(calculator,
      integrationSet, currentIntegrandE, variableExpression);
      outputCandidate += currentIntegral;
      continue;*/
    }
    currentIntegrandNonPolynomializedE =
    currentCF * currentIntegrandTanPart * currentIntegrandSecPart;
    currentIntegrandE.reset(calculator);
    currentIntegrandE.addChildAtomOnTop("Polynomialize");
    currentIntegrandE.addChildOnTop(currentIntegrandNonPolynomializedE);
    currentIntegral.makeIntegral(
      calculator, integrationSet, currentIntegrandE, newVarE
    );
    currentIntegralComputation.makeXOX(
      calculator, calculator.opDefine(), newResultE, currentIntegral
    );
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

bool CalculatorFunctions::powerImaginaryUnit(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::powerImaginaryUnit");
  if (!input.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  if (!input[1].isOperationGiven(calculator.opImaginaryUnit())) {
    return false;
  }
  LargeInteger power;
  if (!input[2].isInteger(&power)) {
    return false;
  }
  Expression iE;
  iE.makeAtom(calculator, calculator.opImaginaryUnit());
  if (power % 4 == 0) {
    return output.assignValue(calculator, 1);
  }
  if (power % 4 == 1) {
    output = iE;
    return true;
  }
  if (power % 4 == 2) {
    return output.assignValue(calculator, - 1);
  }
  if (power % 4 == 3) {
    output = iE *(- 1);
    return true;
  }
  return false;
  // <-this shouldn't happen
}

bool CalculatorFunctionsIntegration::integrateEpowerAxDiffX(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsIntegration::integrateEpowerAxDiffX");
  Expression functionExpression, variableExpression;
  if (
    !input.isIndefiniteIntegralFdx(&variableExpression, &functionExpression)
  ) {
    return false;
  }
  Expression functionCoefficient, functionWithoutCoefficient;
  functionExpression.getCoefficientMultiplicandForm(
    functionCoefficient, functionWithoutCoefficient
  );
  if (
    !functionWithoutCoefficient.startsWith(calculator.opPower(), 3)
  ) {
    return false;
  }
  if (
    !functionWithoutCoefficient[1].isOperationGiven(calculator.opE())
  ) {
    return false;
  }
  Expression powerCoefficient, powerNoCoefficient;
  functionWithoutCoefficient[2].getCoefficientMultiplicandForm(
    powerCoefficient, powerNoCoefficient
  );
  if (powerNoCoefficient != variableExpression) {
    if (powerNoCoefficient.startsWith(calculator.opTimes(), 3)) {
      if (
        powerNoCoefficient[1].isOperationGiven(
          calculator.opImaginaryUnit()
        ) &&
        powerNoCoefficient[2] == variableExpression
      ) {
        output = powerNoCoefficient[1] *(- 1) * functionExpression /
        powerCoefficient;
        output.checkConsistency();
        output.checkInitializationRecursively();
        return true;
      }
    }
    return false;
  }
  output = functionExpression;
  output /= powerCoefficient;
  output.checkConsistency();
  output.checkInitializationRecursively();
  return true;
}

bool CalculatorFunctionsDifferentiation::differentiateSqrt(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsDifferentiation::differentiateSqrt");
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    calculator
    << "<hr>Warning: differentiating with "
    << "respect to the non-atomic expression "
    << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& argument = input[2];
  if (!argument.isOperationGiven(calculator.opSqrt())) {
    return false;
  }
  Expression twoE(calculator);
  Expression oneOverSqrtE(calculator);
  twoE = Rational(1, 2);
  oneOverSqrtE = 1;
  oneOverSqrtE /= argument;
  return
  output.makeXOX(calculator, calculator.opTimes(), twoE, oneOverSqrtE);
}

bool CalculatorFunctionsDifferentiation::differentiateWithRespectToXTimesAny(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsDifferentiation::differentiateWithRespectToXTimesAny"
  );
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
  STACK_TRACE(
    "CalculatorFunctionsDifferentiation::"
    "diffdivDiffxToDifferentiation"
  );
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  bool hasArgument = false;
  bool hasExtraCF = false;
  Expression argument, extraCoeff;
  if (input[1] != "Differential" && input[1] != "d") {
    if (!input[1].startsWith(calculator.opDifferential())) {
      return false;
    }
    if (input[1].size() > 3) {
      return false;
    }
    argument = input[1][1];
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
    output.addChildOnTop(argument);
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
  STACK_TRACE("CalculatorFunctions::ddivDxToDifferentiation");
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  std::string denominatorString, numeratorString;
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
  for (
    int i = 0; i < static_cast<signed>(denominatorString.size()) - 1; i ++
  ) {
    unsigned k = static_cast<unsigned>(i);
    denominatorString[k] = denominatorString[k + 1];
  }
  denominatorString.resize(denominatorString.size() - 1);
  Expression numeratorE, denominatorE(calculator), rightDenE;
  numeratorE.makeAtom(calculator, calculator.opDifferential());
  rightDenE.makeAtom(
    calculator,
    calculator.addOperationNoRepetitionOrReturnIndexFirst(denominatorString)
  );
  denominatorE.addChildOnTop(numeratorE);
  denominatorE.addChildOnTop(rightDenE);
  return
  output.makeXOX(
    calculator, calculator.opDivide(), numeratorE, denominatorE
  );
}

bool CalculatorFunctions::outerMergeConstantRadicals(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::outerMergeConstantRadicals");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (
    !input[1].startsWith(calculator.opPower(), 3) ||
    !input[2].startsWith(calculator.opPower(), 3)
  ) {
    return false;
  }
  if (input[1][2] != input[2][2]) {
    return false;
  }
  if (
    !input[1][1].isOfType<Rational>() &&
    !input[1][1].isOfType<AlgebraicNumber>() &&
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
  Expression product;
  product.makeProduct(calculator, input[1][1], input[2][1]);
  return
  output.makeXOX(
    calculator, calculator.opPower(), product, input[1][2]
  );
}

bool CalculatorFunctions::outerCommuteConstants(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::outerCommuteConstants");
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
  STACK_TRACE(
    "CalculatorFunctions::outerDivideReplaceAdivBpowerItimesBpowerJ"
  );
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
  rightMultiplicandExponent.makeXOX(
    calculator,
    calculator.opMinus(),
    numeratorExponent,
    denominatorExponent
  );
  rightMultiplicand.makeXOX(
    calculator,
    calculator.opPower(),
    denominatorBase,
    rightMultiplicandExponent
  );
  return
  output.makeXOX(
    calculator,
    calculator.opTimes(),
    input[1][1],
    rightMultiplicand
  );
}

bool Expression::splitProduct(
  int numDesiredMultiplicandsLeft,
  Expression& outputLeftMultiplicand,
  Expression& outputRightMultiplicand
) const {
  STACK_TRACE("Expression::splitProduct");
  if (numDesiredMultiplicandsLeft <= 0) {
    return false;
  }
  this->checkInitialization();
  List<Expression> multiplicandsLeft;
  List<Expression> multiplicandsRight;
  this->owner->appendOpandsReturnTrueIfOrderNonCanonical(
    *this, multiplicandsLeft, this->owner->opTimes()
  );
  if (multiplicandsLeft.size <= numDesiredMultiplicandsLeft) {
    return false;
  }
  multiplicandsRight.setExpectedSize(
    multiplicandsLeft.size - numDesiredMultiplicandsLeft
  );
  for (
    int i = numDesiredMultiplicandsLeft; i < multiplicandsLeft.size; i ++
  ) {
    multiplicandsRight.addOnTop(multiplicandsLeft[i]);
  }
  multiplicandsLeft.setSize(numDesiredMultiplicandsLeft);
  outputLeftMultiplicand.makeOXdotsX(
    *this->owner, this->owner->opTimes(), multiplicandsLeft
  );
  return
  outputRightMultiplicand.makeOXdotsX(
    *this->owner, this->owner->opTimes(), multiplicandsRight
  );
}

bool CalculatorFunctions::atimesBpowerJplusEtcDivBpowerI(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::atimesBpowerJplusEtcDivBpowerI");
  if (!input.startsWith(calculator.opDivide(), 3)) {
    return false;
  }
  Expression denominatorBase, denominatorExponent;
  input[2].getBaseExponentForm(denominatorBase, denominatorExponent);
  if (!denominatorBase.divisionByMeShouldBeWrittenInExponentForm()) {
    return false;
  }
  LinearCombination<Expression, Rational> numerators, numeratorsNew;
  calculator.functionCollectSummandsCombine(
    calculator, input[1], numerators
  );
  numeratorsNew.setExpectedSize(numerators.size());
  numeratorsNew.makeZero();
  Expression
  numeratorMultiplicandLeft,
  numeratorMultiplicandRight,
  numeratorBaseRight,
  numeratorExponentRight;
  Expression newNumSummand, newNumSummandRightPart, newNumExponent, mOneE;
  mOneE.assignValue(calculator, - 1);
  for (int i = 0; i < numerators.size(); i ++) {
    if (numerators[i].isConstantNumber()) {
      newNumSummandRightPart.makeXOX(
        calculator,
        calculator.opPower(),
        denominatorBase,
        mOneE* denominatorExponent
      );
      newNumSummand = numerators[i] * newNumSummandRightPart;
      numeratorsNew.addMonomial(
        newNumSummand, numerators.coefficients[i]
      );
      continue;
    }
    numerators[i].getBaseExponentForm(
      numeratorBaseRight, numeratorExponentRight
    );
    if (numeratorBaseRight == denominatorBase) {
      newNumExponent.makeXOX(
        calculator,
        calculator.opMinus(),
        numeratorExponentRight,
        denominatorExponent
      );
      newNumSummand.makeXOX(
        calculator,
        calculator.opPower(),
        denominatorBase,
        newNumExponent
      );
      numeratorsNew.addMonomial(
        newNumSummand, numerators.coefficients[i]
      );
      continue;
    }
    bool isGood = false;
    for (
      int j = 1; numerators[i].splitProduct(
        j, numeratorMultiplicandLeft, numeratorMultiplicandRight
      ); j ++
    ) {
      numeratorMultiplicandRight.getBaseExponentForm(
        numeratorBaseRight, numeratorExponentRight
      );
      if (numeratorBaseRight != denominatorBase) {
        continue;
      }
      newNumExponent.makeXOX(
        calculator,
        calculator.opMinus(),
        numeratorExponentRight,
        denominatorExponent
      );
      newNumSummandRightPart.makeXOX(
        calculator,
        calculator.opPower(),
        denominatorBase,
        newNumExponent
      );
      newNumSummand.makeXOX(
        calculator,
        calculator.opTimes(),
        numeratorMultiplicandLeft,
        newNumSummandRightPart
      );
      numeratorsNew.addMonomial(
        newNumSummand, numerators.coefficients[i]
      );
      isGood = true;
      break;
    }
    if (!isGood) {
      return false;
    }
  }
  return output.makeSum(calculator, numeratorsNew);
}

void Expression::getBlocksOfCommutativity(
  HashedList<Expression>& inputOutputList
) const {
  STACK_TRACE("Expression::getBlocksOfCommutativity");
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
    if (
      this->owner->atomsThatAllowCommutingOfCompositesStartingWithThem.contains
      (whichOperation)
    ) {
      for (int i = 1; i < this->size(); i ++) {
        (*this)[i].getBlocksOfCommutativity(inputOutputList);
      }
      return;
    }
  }
  inputOutputList.addOnTopNoRepetition(*this);
}

bool Expression::makeMatrix(
  Matrix<Expression>* inputMatrix, Calculator& owner
) {
  STACK_TRACE("Expression::makeMatrix");
  if (inputMatrix == nullptr) {
    this->reset(owner);
    Expression emptyMatrix(owner);
    emptyMatrix.addChildAtomOnTop(owner.opMatrix());
    this->addChildOnTop(emptyMatrix);
    return true;
  }
  return this->assignMatrixExpressions(*inputMatrix, owner, true, true);
}

bool Expression::makeSequence(
  Calculator& owner, List<Expression>* inputSequence
) {
  STACK_TRACE("Expression::makeSequence");
  this->reset(owner, inputSequence == nullptr ? 1 : inputSequence->size + 1);
  this->addChildAtomOnTop(owner.opSequence());
  if (inputSequence != nullptr) {
    for (int i = 0; i < inputSequence->size; i ++) {
      this->addChildOnTop((*inputSequence)[i]);
    }
  }
  return true;
}

bool Expression::makeSequenceCommands(
  Calculator& owner,
  List<std::string>& inputKeys,
  List<Expression>& inputValues
) {
  STACK_TRACE("Expression::makeSequenceCommands");
  List<Expression> statements;
  Expression currentStatement, currentKey;
  if (inputValues.size != inputKeys.size) {
    global.fatal
    << "I am asked to create a "
    << "sequence of statements but I was given different "
    << "number of keys and expressions."
    << global.fatal;
  }
  for (int i = 0; i < inputValues.size; i ++) {
    currentKey.makeAtom(owner, inputKeys[i]);
    currentStatement.makeXOX(
      owner, owner.opDefine(), currentKey, inputValues[i]
    );
    statements.addOnTop(currentStatement);
  }
  return this->makeSequenceStatements(owner, &statements);
}

bool Expression::makeSequenceStatements(
  Calculator& owner, List<Expression>* inputStatements
) {
  STACK_TRACE("Expression::makeSequenceStatements");
  this->reset(
    owner, inputStatements == nullptr ? 1 : inputStatements->size + 1
  );
  this->addChildAtomOnTop(owner.opCommandSequence());
  if (inputStatements != nullptr) {
    for (int i = 0; i < inputStatements->size; i ++) {
      this->addChildOnTop((*inputStatements)[i]);
    }
  }
  return true;
}

bool CalculatorFunctions::getUserDefinedSubExpressions(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::getUserDefinedSubExpressions");
  if (input.size() != 2) {
    return false;
  }
  HashedList<Expression> commutativityBlocks;
  input[1].getBlocksOfCommutativity(commutativityBlocks);
  return output.makeSequence(calculator, &commutativityBlocks);
}

bool CalculatorFunctions::lispify(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::lispify");
  if (input.size() != 2) {
    return false;
  }
  return output.assignValue(calculator, input[1].toStringSemiFull());
}

bool CalculatorFunctions::lispifyFull(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::lispifyFull");
  if (input.size() != 2) {
    return false;
  }
  return
  output.assignValue(
    calculator, input[1].toStringFullWithHints(false)
  );
}

bool CalculatorFunctionsLinearAlgebra::minimalPolynomialMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsLinearAlgebra::minimalPolynomialMatrix");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (
    !CalculatorConversions::functionMatrixRational(
      calculator, argument, output
    )
  ) {
    return false;
  }
  Matrix<Rational> matrix;
  if (
    !CalculatorConversions::functionGetMatrixNoComputation(
      calculator, argument, matrix
    )
  ) {
    return
    calculator
    << "<hr>Minimal poly computation: could not convert "
    << argument.toString()
    << " to rational matrix.";
  }
  if (
    matrix.numberOfRows != matrix.numberOfColumns || matrix.numberOfRows <= 0
  ) {
    return output.assignError(calculator, "Error: matrix is not square.");
  }
  FormatExpressions tempF;
  tempF.polynomialAlphabet.setSize(1);
  tempF.polynomialAlphabet[0] = "q";
  Polynomial<Rational> minimalPolynomial;
  minimalPolynomial.assignMinimalPolynomial(matrix);
  return
  output.assignValue(calculator, minimalPolynomial.toString(&tempF));
}

bool CalculatorFunctionsLinearAlgebra::characteristicPolynomialMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::characteristicPolynomialMatrix");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Matrix<Rational> matrix;
  if (
    !CalculatorConversions::functionGetMatrixNoComputation(
      calculator, argument, matrix
    )
  ) {
    return
    calculator
    << "<hr>Characteristic poly computation: could not convert "
    << input.toString()
    << " to rational matrix.";
  }
  if (
    matrix.numberOfRows != matrix.numberOfColumns || matrix.numberOfRows <= 0
  ) {
    return output.assignError(calculator, "Error: matrix is not square.");
  }
  FormatExpressions format;
  format.polynomialAlphabet.setSize(1);
  format.polynomialAlphabet[0] = "q";
  Polynomial<Rational> characterPolynomial;
  characterPolynomial.assignCharacteristicPolynomial(matrix);
  return
  output.assignValue(calculator, characterPolynomial.toString(&format));
}

bool CalculatorFunctions::reverseBytes(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::reverseBytes");
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
  return output.assignValue(calculator, result);
}

bool CalculatorFunctions::matrixTrace(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::matrixTrace");
  if (input.size() != 2) {
    return false;
  }
  Matrix<Rational> matrix;
  if (
    CalculatorConversions::functionGetMatrixNoComputation(
      calculator, input[1], matrix
    )
  ) {
    if (!matrix.isSquare()) {
      return
      output.assignError(
        calculator, "Error: attempting to get trace of non-square matrix. "
      );
    }
    return output.assignValue(calculator, matrix.getTrace());
  }
  Matrix<RationalFraction<Rational> > matrixRationalFunction;
  if (
    CalculatorConversions::functionGetMatrixNoComputation(
      calculator, input[1], matrixRationalFunction
    )
  ) {
    if (!matrixRationalFunction.isSquare()) {
      return
      output.assignError(
        calculator, "Error: attempting to get trace of non-square matrix. "
      );
    }
    return
    output.assignValue(calculator, matrixRationalFunction.getTrace());
  }
  Matrix<Expression> matrixExpression;
  if (
    !calculator.getMatrixExpressionsFromArguments(
      input[1], matrixExpression
    )
  ) {
    return false;
  }
  if (!matrixExpression.isSquare()) {
    return
    output.assignError(
      calculator, "Error: attempting to get trace of non-square matrix. "
    );
  }
  if (matrixExpression.numberOfRows == 1) {
    calculator
    << "Requested trace of 1x1 matrix: possible "
    << "interpretation of a scalar as a 1x1 matrix. Trace not taken. ";
    return false;
  }
  output = matrixExpression.getTrace();
  return true;
}

bool CalculatorFunctions::contains(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::contains");
  if (input.size() != 3) {
    return false;
  }
  if (input[1].containsAsSubExpressionNoBuiltInTypes(input[2])) {
    return output.assignValue(calculator, 1);
  } else {
    return output.assignValue(calculator, 0);
  }
}

bool CalculatorFunctions::expressionLeafs(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::expressionLeafs");
  HashedList<Expression> leafs;
  if (input.startsWithGivenOperation("ExpressionLeafs")) {
    for (int i = 1; i < input.size(); i ++) {
      if (!input[i].getExpressionLeafs(leafs)) {
        return
        calculator
        << "Failed to extract expression leafs from "
        << input.toString()
        << ".";
      }
    }
  } else if (!input.getExpressionLeafs(leafs)) {
    return
    calculator
    << "Failed to extract expression leafs from "
    << input.toString()
    << ".";
  }
  return output.makeSequence(calculator, &leafs);
}

bool CalculatorFunctionsListsAndSets::lastElement(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsListsAndSets::lastElement");
  if (input.hasBoundVariables()) {
    return false;
  }
  if (input.isAtom()) {
    std::stringstream out;
    out
    << "Error: requesting the last element of the atom "
    << input.toString();
    return output.assignError(calculator, out.str());
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

bool CalculatorFunctionsListsAndSets::removeLastElement(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsListsAndSets::removeLastElement");
  if (input.hasBoundVariables()) {
    return false;
  }
  if (input.isAtom() || input.size() == 1) {
    std::stringstream out;
    if (input.isAtom()) {
      out
      << "Error: requesting to remove the last element of the atom "
      << input.toString()
      << ". ";
    } else {
      out
      << "Error: requesting to remove the last "
      << "element of the zero-element list "
      << input.toString()
      << ". ";
    }
    return output.assignError(calculator, out.str());
  }
  if (input.size() == 2) {
    const Expression& sequenceCandidate = input[1];
    if (
      sequenceCandidate.isSequenceNElements() && sequenceCandidate.size() > 1
    ) {
      output = sequenceCandidate;
      output.removeLastChild();
      return true;
    }
  }
  output = input;
  output.removeLastChild();
  return output.setChildAtomValue(0, calculator.opSequence());
}

bool ElementZmodP::operator==(int other) const {
  this->checkIamInitialized();
  ElementZmodP element;
  element.modulus = this->modulus;
  element = LargeInteger(other);
  return *this == element;
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

bool CalculatorFunctions::isPossiblyPrime(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::isPossiblyPrime");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger integerValue;
  if (!input[1].isInteger(&integerValue)) {
    return false;
  }
  bool resultBool =
  integerValue.value.isPossiblyPrime(10, true, &calculator.comments);
  Rational result = 1;
  if (!resultBool) {
    result = 0;
  }
  return output.assignValue(calculator, result);
}

bool CalculatorFunctions::isPrimeMillerRabin(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::isPrimeMillerRabin");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger integerValue;
  if (!input[1].isInteger(&integerValue)) {
    return false;
  }
  bool resultBool =
  integerValue.value.isPossiblyPrimeMillerRabin(10, &calculator.comments);
  Rational result = 1;
  if (!resultBool) {
    result = 0;
  }
  return output.assignValue(calculator, result);
}

bool CalculatorFunctions::isNilpotent(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::isNilpotent");
  Expression converted;
  if (!CalculatorConversions::makeMatrix(calculator, input, converted)) {
    return false;
  }
  bool found = false;
  Matrix<Rational> matrix;
  MatrixTensor<Rational> matrixTensor;
  if (
    CalculatorConversions::functionGetMatrixNoComputation(
      calculator, converted, matrix
    )
  ) {
    found = true;
    matrixTensor = matrix;
  } else if (input.isOfType<MatrixTensor<Rational> >(&matrixTensor)) {
    found = true;
  }
  if (!found) {
    return
    output.assignError(
      calculator, "Failed to extract matrix with rational coefficients"
    );
  }
  if (matrixTensor.isNilpotent()) {
    return output.assignValue(calculator, 1);
  }
  return output.assignValue(calculator, 0);
}

bool CalculatorFunctions::invertMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::invertMatrix");
  Matrix<Rational> matrix;
  Expression converted;
  if (!CalculatorConversions::makeMatrix(calculator, input, converted)) {
    return calculator << "Failed to extract matrix from input. ";
  }
  if (
    CalculatorConversions::functionGetMatrixNoComputation(
      calculator, converted, matrix
    )
  ) {
    if (
      matrix.numberOfRows != matrix.numberOfColumns ||
      matrix.numberOfColumns < 1
    ) {
      return output.assignError(calculator, "The matrix is not square");
    }
    if (matrix.getDeterminant() == 0) {
      return output.assignError(calculator, "Matrix determinant is zero.");
    }
    matrix.invert();
    return output.makeMatrix(calculator, matrix);
  }
  Matrix<AlgebraicNumber> matrixAlgebraic;
  if (
    CalculatorConversions::functionGetMatrixNoComputation(
      calculator, input, matrixAlgebraic
    )
  ) {
    return
    calculator
    << "<hr>Failed to extract algebraic number matrix from: "
    << input.toString();
  }
  if (
    matrixAlgebraic.numberOfRows != matrixAlgebraic.numberOfColumns ||
    matrixAlgebraic.numberOfColumns < 1
  ) {
    return output.assignError(calculator, "The matrix is not square");
  }
  if (matrixAlgebraic.getDeterminant() == 0) {
    return output.assignError(calculator, "Matrix determinant is zero.");
  }
  matrixAlgebraic.invert();
  return output.makeMatrix(calculator, matrixAlgebraic);
}

bool CalculatorFunctions::differentialEquationsEulersMethod(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::dFQsEulersMethod");
  if (input.size() != 7) {
    return calculator << "Euler method function takes 6 arguments";
  }
  double xInitial, yInitial, leftEndpoint, rightEndpoint;
  int numPoints;
  if (
    !input[2].evaluatesToDouble(&xInitial) ||
    !input[3].evaluatesToDouble(&yInitial)
  ) {
    return
    calculator
    << "Failed to extract initial x,y values from "
    << input.toString();
  }
  if (!input[4].isSmallInteger(&numPoints)) {
    return
    calculator
    << "Failed to extract number of points from "
    << input.toString();
  }
  if (
    !input[5].evaluatesToDouble(&leftEndpoint) ||
    !input[6].evaluatesToDouble(&rightEndpoint)
  ) {
    return
    calculator
    << "Failed to extract left and right endpoints from "
    << input.toString();
  }
  if (leftEndpoint > rightEndpoint) {
    MathRoutines::swap(leftEndpoint, rightEndpoint);
  }
  if (xInitial > rightEndpoint || xInitial < leftEndpoint) {
    return
    calculator
    << "The initial value for x, "
    << xInitial
    << ", does not lie between the left and right endpoints "
    << leftEndpoint
    << ", "
    << rightEndpoint
    << ". I am aborting Euler's method. ";
  }
  HashedList<Expression> knownConsts;
  List<double> knownValues;
  knownConsts.addListOnTop(calculator.knownDoubleConstants);
  knownValues.addListOnTop(calculator.knownDoubleConstantValues);
  Expression xE, yE;
  xE.makeAtom(calculator, "x");
  yE.makeAtom(calculator, "y");
  if (knownConsts.contains(xE) || knownConsts.contains(yE)) {
    return
    calculator
    << "The letters x, y appear to be already used to "
    << "denote known constants, I cannot run Euler's method.";
  }
  knownConsts.addOnTop(xE);
  knownConsts.addOnTop(yE);
  knownValues.addOnTop(0);
  knownValues.addOnTop(0);
  if (numPoints < 2) {
    return
    calculator
    << "The number of points for Euler's method is "
    << numPoints
    << ", too few. ";
  }
  if (numPoints > 10001) {
    return
    calculator
    << "The number of points for Euler's method is "
    << numPoints
    << ", I am not allowed to handle that many. ";
  }
  if (leftEndpoint - rightEndpoint == 0.0) {
    return
    calculator
    << "Whlie doing Euler's method: right endpoint equals left! ";
  }
  double delta = (rightEndpoint - leftEndpoint) / numPoints;
  List<double> XValues, YValues;
  XValues.setExpectedSize(numPoints + 5);
  YValues.setExpectedSize(numPoints + 5);
  int pointsCounter = 0;
  for (
    double currentX = xInitial; currentX > leftEndpoint - delta; currentX -=
    delta
  ) {
    XValues.addOnTop(currentX);
    pointsCounter ++;
    if (pointsCounter > numPoints) {
      break;
      // <-in case floating point arithmetic is misbehaving
    }
  }
  XValues.reverseElements();
  int indexXinitial = XValues.size - 1;
  pointsCounter = 0;
  for (
    double currentX = xInitial + delta; currentX < rightEndpoint + delta;
    currentX += delta
  ) {
    XValues.addOnTop(currentX);
    pointsCounter ++;
    if (pointsCounter > numPoints) {
      break;
      // <-in case floating point arithmetic is misbehaving
    }
  }
  YValues.initializeFillInObject(XValues.size, 0);
  YValues[indexXinitial] = yInitial;
  Expression functionE = input[1];
  double currentYprimeApprox = 0;
  int lastGoodXIndex = indexXinitial;
  int firstGoodXIndex = indexXinitial;
  double maxYallowed = (rightEndpoint - leftEndpoint) * 2 + yInitial;
  double minYallowed = -(rightEndpoint - leftEndpoint) * 2 + yInitial;
  for (int direction = - 1; direction < 2; direction += 2) {
    for (
      int i = indexXinitial + direction; i >= 0 && i < XValues.size; i +=
      direction
    ) {
      knownValues[knownValues.size - 2] = XValues[i];
      bool isGood = true;
      for (int counter = 0;; counter ++) {
        knownValues[knownValues.size - 1] = YValues[i];
        if (
          !functionE.evaluatesToDoubleUnderSubstitutions(
            knownConsts, knownValues, &currentYprimeApprox
          )
        ) {
          return
          calculator
          << "Failed to evaluate yPrime approximation at x ="
          << XValues[i];
        }
        double adjustment = delta * direction * currentYprimeApprox;
        double ynew = YValues[i - direction] + adjustment;
        double difference = ynew - YValues[i];
        YValues[i] = ynew;
        if (counter > 20) {
          // <- we run Euler algorithm at least 20 times.
          if (difference > - 0.01 && difference < 0.01) {
            // <-if changes are smaller than 0.01 we assume success.
            break;
          }
        }
        if (counter > 200) {
          // <- we ran Euler algorithm 100 times, but the difference is still
          // greater than 0.01. Something is wrong, we abort
          calculator << "Euler method: no convergence. At x ="
          << XValues[i]
          << ", y ="
          << YValues[i]
          << ", the change in y is: "
          << difference
          << " after "
          << counter
          << " iterations. ";
          if (counter > 205) {
            isGood = false;
            break;
          }
        }
      }
      if (isGood) {
        isGood = (YValues[i] <= maxYallowed && YValues[i] >= minYallowed);
        if (!isGood) {
          calculator
          << "Euler method: y values outside of the bounding box. At x ="
          << XValues[i]
          << ", the y value is: "
          << YValues[i]
          << ". Max y is  "
          << maxYallowed
          << " and min y is "
          << minYallowed
          << ". ";
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
  PlotObject plot;
  Vector<double> currentPt;
  currentPt.setSize(2);
  for (int i = firstGoodXIndex; i <= lastGoodXIndex; i ++) {
    currentPt[0] = XValues[i];
    currentPt[1] = YValues[i];
    plot.pointsDouble.addOnTop(currentPt);
  }
  plot.xLow = XValues[0];
  plot.xHigh = *XValues.lastObject();
  plot.yLow = - 0.5;
  plot.yHigh = 0.5;
  plot.coordinateFunctionsE.addOnTop(input);
  std::stringstream outLatex, outHtml;
  outLatex
  << "\n\n%calculator input:"
  << input.toString()
  << "\n\n"
  << "\\psline[linecolor =\\fcColorGraph]";
  outHtml
  << "\n<br>\n%calculator input:"
  << input.toString()
  << "\n<br>\n"
  << "\\psline[linecolor =\\fcColorGraph]";
  for (int i = firstGoodXIndex; i <= lastGoodXIndex; i ++) {
    outLatex << std::fixed << "(" << XValues[i] << ", " << YValues[i] << ")";
    outHtml << std::fixed << "(" << XValues[i] << ", " << YValues[i] << ")";
    plot.yLow = MathRoutines::minimum(YValues[i], plot.yLow);
    plot.yHigh = MathRoutines::maximum(YValues[i], plot.yHigh);
  }
  plot.plotString = outLatex.str();
  plot.plotStringWithHtml = outHtml.str();
  plot.dimension = 2;
  return output.assignValue(calculator, plot);
}

bool CalculatorFunctionsPlot::plotViewWindow(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPlot::plotViewWindow");
  if (input.size() < 3) {
    return false;
  }
  Vector<double> widthHeight;
  Plot emptyPlot;
  emptyPlot.priorityWindow = 1;
  bool isGood = false;
  if (
    calculator.getVectorDoublesFromFunctionArguments(input, widthHeight, 2)
  ) {
    isGood = true;
  } else {
    widthHeight.setSize(2);
    widthHeight[0] = 100;
    widthHeight[1] = 100;
    MapList<
      std::string,
      Expression,
      HashFunctions::hashFunction<std::string>
    > map;
    if (
      !CalculatorConversions::loadKeysFromStatementList(
        calculator, input, map, nullptr, false
      )
    ) {
      isGood = false;
    } else {
      if (map.contains("width")) {
        if (
          !map.getValueCreateNoInitialization("width").evaluatesToDouble(
            &widthHeight[0]
          )
        ) {
          isGood = false;
        }
      }
      if (map.contains("height")) {
        if (
          !map.getValueCreateNoInitialization("height").evaluatesToDouble(
            &widthHeight[1]
          )
        ) {
          isGood = false;
        }
      }
    }
  }
  if (!isGood) {
    return
    calculator
    << "Failed to extract a pair of positive numbers from: "
    << input.toString();
  }
  emptyPlot.desiredHtmlWidthInPixels = static_cast<int>(widthHeight[0]);
  emptyPlot.desiredHtmlHeightInPixels = static_cast<int>(widthHeight[1]);
  return output.assignValue(calculator, emptyPlot);
}

bool CalculatorFunctionsPlot::plotSetId(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPlot::plotSetId");
  Plot emptyPlot;
  emptyPlot.priorityCanvasName = 1;
  emptyPlot.desiredHtmlHeightInPixels = 100;
  emptyPlot.desiredHtmlWidthInPixels = 100;
  std::string incomingName;
  if (!input.isOfType(&incomingName)) {
    incomingName = input.toString();
  }
  emptyPlot.setCanvasName(incomingName);
  return output.assignValue(calculator, emptyPlot);
}

bool CalculatorFunctionsPlot::plotViewRectangle(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPlot::plotViewRectangle");
  if (input.size() < 3) {
    return false;
  }
  Vector<double> lowerLeft, upperRight;
  if (
    !calculator.getVectorDoubles(input[1], lowerLeft, 2) ||
    !calculator.getVectorDoubles(input[2], upperRight, 2)
  ) {
    return
    calculator
    << "Failed to extract two pairs of floating point numbers from: "
    << input[1].toString()
    << " and "
    << input[2].toString();
  }
  Plot emptyPlot;
  emptyPlot.dimension = 2;
  emptyPlot.lowerBoundAxes = lowerLeft[0];
  emptyPlot.lowBoundY = lowerLeft[1];
  emptyPlot.upperBoundAxes = upperRight[0];
  emptyPlot.highBoundY = upperRight[1];
  emptyPlot.priorityViewRectangle = 1;
  emptyPlot.desiredHtmlHeightInPixels = 100;
  emptyPlot.desiredHtmlWidthInPixels = 100;
  return output.assignValue(calculator, emptyPlot);
}

bool CalculatorFunctionsPlot::plotFill(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPlot::plotFill");
  if (input.size() < 3) {
    return false;
  }
  const Expression& plotExpression = input[1];
  const Expression& colorE = input[2];
  Plot outputPlot, startPlot;
  outputPlot.dimension = 2;
  PlotObject filledPlot;
  if (!plotExpression.isOfType<Plot>(&startPlot)) {
    return false;
  }
  std::string colorString;
  if (!colorE.isOfType<std::string>(&colorString)) {
    colorString = colorE.toString();
  }
  if (
    !DrawingVariables::getColorIntFromColorString(
      colorString, filledPlot.colorFillRGB
    )
  ) {
    calculator
    << "Failed to extract color from: "
    << colorE.toString()
    << "; using default color value. ";
  }
  filledPlot.colorFillJS = colorString;
  for (int i = 0; i < startPlot.getPlots().size; i ++) {
    filledPlot.pointsDouble.addListOnTop(
      startPlot.getPlots()[i].pointsDouble
    );
  }
  filledPlot.fillStyle = "filled";
  filledPlot.plotType = "plotFillStart";
  outputPlot.desiredHtmlHeightInPixels = startPlot.desiredHtmlHeightInPixels;
  outputPlot.desiredHtmlWidthInPixels = startPlot.desiredHtmlWidthInPixels;
  outputPlot += filledPlot;
  outputPlot += startPlot;
  filledPlot.plotType = "plotFillFinish";
  outputPlot += filledPlot;
  return output.assignValue(calculator, outputPlot);
}

bool CalculatorFunctionsPlot::isPlot(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPlot::isPlot");
  if (input.size() != 2) {
    return false;
  }
  if (input[1].isOfType<Plot>()) {
    return output.assignValue(calculator, 1);
  }
  return output.assignValue(calculator, 0);
}

bool CalculatorFunctionsPlot::plot2DOverIntervals(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPlot::plot2DOverIntervals");
  if (input.size() < 3) {
    return false;
  }
  List<Expression> intervalCandidates;
  if (
    !calculator.collectOpands(
      input[2], calculator.opUnion(), intervalCandidates
    )
  ) {
    return false;
  }
  if (intervalCandidates.size < 1) {
    return false;
  }
  for (int i = 0; i < intervalCandidates.size; i ++) {
    if (!intervalCandidates[i].isIntervalRealLine()) {
      return false;
    }
  }
  Expression summandE;
  List<Expression> finalSummands;
  for (int i = 0; i < intervalCandidates.size; i ++) {
    summandE.reset(calculator);
    summandE.addChildOnTop(input[0]);
    summandE.addChildOnTop(input[1]);
    summandE.addChildOnTop(intervalCandidates[i][1]);
    summandE.addChildOnTop(intervalCandidates[i][2]);
    for (int j = 3; j < input.size(); j ++) {
      summandE.addChildOnTop(input[j]);
    }
    finalSummands.addOnTop(summandE);
  }
  return output.makeSum(calculator, finalSummands);
}

bool CalculatorFunctionsPlot::plot2D(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPlot::plot2D");
  if (input.size() < 4) {
    return
    output.assignError(
      calculator,
      "Plotting coordinates takes at least "
      "three arguments: function, lower and upper bound. "
    );
  }
  if (input.hasBoundVariables()) {
    return false;
  }
  Plot plot;
  plot.dimension = 2;
  PlotObject plotObject;
  plotObject.leftPoint = input[2];
  plotObject.rightPoint = input[3];
  if (input.size() >= 5) {
    if (!input[4].isOfType<std::string>(&plotObject.colorJS)) {
      plotObject.colorJS = input[4].toString();
    }
  } else {
    plotObject.colorJS = "red";
  }
  plotObject.colorRGB = static_cast<int>(
    HtmlRoutines::redGreenBlue(255, 0, 0)
  );
  DrawingVariables::getColorIntFromColorString(
    plotObject.colorJS, plotObject.colorRGB
  );
  plotObject.lineWidth = 1;
  if (input.size() >= 6) {
    input[5].evaluatesToDouble(&plotObject.lineWidth);
  }
  if (input.size() >= 7) {
    plotObject.numSegmentsE = input[6];
  } else {
    plotObject.numSegmentsE.assignValue(calculator, 500);
  }
  int numberOfIntervals = - 1;
  if (!plotObject.numSegmentsE.isSmallInteger(&numberOfIntervals)) {
    numberOfIntervals = 500;
  }
  if (numberOfIntervals < 2) {
    numberOfIntervals = 2;
  }
  bool leftIsDouble =
  plotObject.leftPoint.evaluatesToDouble(&plotObject.xLow);
  bool rightIsDouble =
  plotObject.rightPoint.evaluatesToDouble(&plotObject.xHigh);
  if (!leftIsDouble) {
    if (plotObject.leftPoint != calculator.expressionMinusInfinity()) {
      return
      calculator
      << "Couldn't convert left boundary "
      << plotObject.leftPoint.toString()
      << " to floating point number. ";
    } else {
      plotObject.leftBoundaryIsMinusInfinity = true;
    }
  }
  if (!rightIsDouble) {
    if (plotObject.rightPoint != calculator.expressionInfinity()) {
      return
      calculator
      << "Couldn't convert right boundary "
      << plotObject.rightPoint.toString()
      << " to floating point number. ";
    } else {
      plotObject.rightBoundaryIsMinusInfinity = true;
    }
  }
  plotObject.coordinateFunctionsE.addOnTop(input[1]);
  plotObject.coordinateFunctionsJS.setSize(1);
  plotObject.coordinateFunctionsE[0].getFreeVariables(
    plotObject.variablesInPlay, true
  );
  if (plotObject.variablesInPlay.size > 1) {
    return
    calculator
    << "Got a function with "
    << plotObject.variablesInPlay.size
    << " variables, namely: "
    << plotObject.variablesInPlay.toStringCommaDelimited()
    << ". I was expecting a single variable. ";
  }
  if (plotObject.variablesInPlay.size == 0) {
    Expression xE;
    xE.makeAtom(calculator, "x");
    plotObject.variablesInPlay.addOnTop(xE);
  }
  plotObject.variablesInPlayJS.setSize(plotObject.variablesInPlay.size);
  plotObject.variablesInPlayJS[0] = plotObject.variablesInPlay[0].toString();
  std::string variableString = plotObject.variablesInPlayJS[0];
  Expression jsConverterE;
  plotObject.plotType = PlotObject::PlotTypes::plotFunction;
  if (
    CalculatorFunctions::functionMakeJavascriptExpression(
      calculator, plotObject.coordinateFunctionsE[0], jsConverterE
    )
  ) {
    plotObject.coordinateFunctionsJS[0] = jsConverterE.toString();
    plotObject.coordinateFunctionsE[0].hasInputBoxVariables(
      &plotObject.parametersInPlay, &plotObject.parametersInPlayJS
    );
  } else {
    plotObject.plotType = "plotFunctionPrecomputed";
  }
  if (
    CalculatorFunctions::functionMakeJavascriptExpression(
      calculator, plotObject.leftPoint, jsConverterE
    )
  ) {
    plotObject.leftPtJS = jsConverterE.toString();
    plotObject.leftPoint.hasInputBoxVariables(
      &plotObject.parametersInPlay, &plotObject.parametersInPlayJS
    );
  } else {
    plotObject.plotType = "plotFunctionPrecomputed";
  }
  if (
    CalculatorFunctions::functionMakeJavascriptExpression(
      calculator, plotObject.rightPoint, jsConverterE
    )
  ) {
    plotObject.rightPtJS = jsConverterE.toString();
    plotObject.rightPoint.hasInputBoxVariables(
      &plotObject.parametersInPlay, &plotObject.parametersInPlayJS
    );
  } else {
    plotObject.plotType = "plotFunctionPrecomputed";
  }
  plotObject.numberOfSegmentsJS.setSize(1);
  plotObject.numberOfSegmentsJS[0] = "200";
  if (
    CalculatorFunctions::functionMakeJavascriptExpression(
      calculator, plotObject.numSegmentsE, jsConverterE
    )
  ) {
    plotObject.numberOfSegmentsJS[0] = jsConverterE.toString();
    plotObject.numSegmentsE.hasInputBoxVariables(
      &plotObject.parametersInPlay, &plotObject.parametersInPlayJS
    );
  } else {
    plotObject.plotType = "plotFunctionPrecomputed";
  }
  Vectors<double>& pointsDouble = plotObject.pointsDouble;
  if (plotObject.parametersInPlay.size == 0) {
    if (
      !input[1].evaluatesToDoubleInRange(
        variableString,
        plotObject.xLow,
        plotObject.xHigh,
        numberOfIntervals,
        &plotObject.yLow,
        &plotObject.yHigh,
        &pointsDouble
      )
    ) {
      bool hasOneGoodPoint = false;
      for (int i = 0; i < pointsDouble.size; i ++) {
        if (!FloatingPoint::isNaN(pointsDouble[i][1])) {
          hasOneGoodPoint = true;
          break;
        }
      }
      if (!hasOneGoodPoint) {
        return
        calculator
        << "<hr>I failed to evaluate the input function at all points, "
        << "perhaps your expression is not a function of x.";
      }
      calculator
      << "<hr>I failed to evaluate your "
      << "function in a number of points. ";
    }
  }
  Expression functionSuffixNotationE;
  if (
    !calculator.callCalculatorFunction(
      CalculatorFunctions::suffixNotationForPostScript,
      input[1],
      functionSuffixNotationE
    )
  ) {
    calculator
    << "No LaTeX version: failed to convert: "
    << input[1].toString()
    << " to postfix notation. ";
  } else {
    plotObject.plotString =
    plotObject.getPlotStringFromFunctionStringAndRanges(
      false,
      functionSuffixNotationE.toString(),
      plotObject.coordinateFunctionsE[0].toString(),
      plotObject.xLow,
      plotObject.xHigh
    );
    plotObject.plotStringWithHtml = plotObject.plotString;
  }
  plot += plotObject;
  return output.assignValue(calculator, plot);
}

bool CalculatorFunctionsPlot::plotPoint(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPlot::plotPoint");
  if (input.size() != 3) {
    return
    output.assignError(
      calculator,
      "Plotting a point takes at least "
      "two arguments, location and color. "
    );
  }
  PlotObject plot;
  if (!calculator.getMatrixExpressions(input[1], plot.points)) {
    return
    calculator
    << "The first argument of PlotPoint is "
    << "expected to be a sequence, instead I had: "
    << input[1].toString();
  }
  plot.dimension = plot.points.numberOfColumns;
  plot.coordinateFunctionsE.setSize(plot.dimension);
  plot.coordinateFunctionsJS.setSize(plot.dimension);
  Expression jsConverterE;
  plot.pointsJS.initialize(
    plot.points.numberOfRows, plot.points.numberOfColumns
  );
  for (int i = 0; i < plot.points.numberOfRows; i ++) {
    for (int j = 0; j < plot.points.numberOfColumns; j ++) {
      if (
        !CalculatorFunctions::functionMakeJavascriptExpression(
          calculator, plot.points(i, j), jsConverterE
        )
      ) {
        return
        calculator
        << "Failed to extract coordinate "
        << i + 1
        << " from: "
        << plot.coordinateFunctionsE[i].toString();
      }
      plot.pointsJS(i, j) = jsConverterE.toString();
      plot.points(i, j).hasInputBoxVariables(
        &plot.parametersInPlay, &plot.parametersInPlayJS
      );
    }
  }
  plot.dimension = plot.dimension;
  plot.colorRGB = static_cast<int>(HtmlRoutines::redGreenBlue(0, 0, 0));
  if (input[2].isOfType<std::string>()) {
    DrawingVariables::getColorIntFromColorString(
      input[2].getValue<std::string>(), plot.colorRGB
    );
  }
  plot.colorJS = input[2].toString();
  plot.plotType = "points";
  Plot finalPlot;
  finalPlot.dimension = plot.dimension;
  finalPlot += plot;
  finalPlot.desiredHtmlHeightInPixels = 100;
  finalPlot.desiredHtmlWidthInPixels = 100;
  return output.assignValue(calculator, finalPlot);
}

bool CalculatorFunctionsPlot::plot2DWithBars(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPlot::plot2DWithBars");
  if (input.size() < 6) {
    return
    output.assignError(
      calculator,
      "Plotting coordinates takes the "
      "following arguments: lower function, "
      "upper function, lower and upper bound, delta x. "
    );
  }
  Expression lowerEplot = input;
  Expression upperEplot = input;
  lowerEplot.removeChildShiftDown(2);
  upperEplot.removeChildShiftDown(1);
  Plot outputPlot;
  outputPlot.dimension = 2;
  bool tempB =
  CalculatorFunctionsPlot::plot2D(calculator, lowerEplot, output);
  if (!tempB || !output.isOfType<Plot>(&outputPlot)) {
    return
    calculator
    << "<hr>Failed to get a plot from "
    << lowerEplot.toString()
    << ", not proceding with bar plot.";
  }
  tempB = CalculatorFunctionsPlot::plot2D(calculator, upperEplot, output);
  if (!tempB || !output.isOfType<Plot>()) {
    return
    calculator
    << "<hr>Failed to get a plot from "
    << upperEplot.toString()
    << ", not proceding with bar plot.";
  }
  outputPlot += output.getValue<Plot>();
  const Expression& lowerFunctionE = input[1];
  const Expression& upperFunctionE = input[2];
  const Expression& lowerE = input[3];
  const Expression& upperE = input[4];
  const Expression& deltaE = input[5];
  double deltaNoSign;
  double deltaWithSign;
  if (!deltaE.evaluatesToDouble(&deltaWithSign)) {
    return false;
  }
  deltaNoSign = deltaWithSign;
  if (deltaNoSign < 0) {
    deltaNoSign *= - 1;
  }
  if (deltaNoSign == 0.0) {
    deltaNoSign = 1;
  }
  double upperBound, lowerBound;
  if (
    !lowerE.evaluatesToDouble(&lowerBound) ||
    !upperE.evaluatesToDouble(&upperBound)
  ) {
    return
    calculator
    << "Couldn't convert bounds of drawing "
    << "function to floating point numbers. ";
  }
  if (upperBound < lowerBound) {
    MathRoutines::swap(upperBound, lowerBound);
  }
  Expression
  xValueE,
  xExpression,
  functionValueExpressioNonEvaluated,
  functionValueFinal;
  xExpression.makeAtom(
    calculator,
    calculator.addOperationNoRepetitionOrReturnIndexFirst("x")
  );
  List<double> xValues;
  List<double> fValuesLower;
  List<double> fValuesUpper;
  if (deltaNoSign == 0.0) {
    return
    output.assignError(calculator, "Delta equal to zero is not allowed");
  }
  if ((upperBound - lowerBound) / deltaNoSign > 10000) {
    return
    output.assignError(
      calculator,
      "More than 10000 intervals "
      "needed for the plot, this is not allowed."
    );
  }
  List<Rational> rValues;
  Rational lowerBoundRational;
  Rational upperBoundRational;
  Rational deltaRational;
  if (
    lowerE.isOfType<Rational>(&lowerBoundRational) &&
    upperE.isOfType<Rational>(&upperBoundRational) &&
    deltaE.isOfType<Rational>(&deltaRational)
  ) {
    if (upperBoundRational < lowerBoundRational) {
      MathRoutines::swap(upperBoundRational, lowerBoundRational);
    }
    if (deltaRational < 0) {
      deltaRational *= - 1;
    }
    if (deltaRational == 0) {
      return
      output.assignError(calculator, "Delta equal to zero is not allowed");
    }
    for (
      Rational i = lowerBoundRational; i <= upperBoundRational; i +=
      deltaRational
    ) {
      rValues.addOnTop(i);
    }
  }
  double yMax = - 0.5, yMin = 0.5;
  for (double i = lowerBound; i <= upperBound; i += deltaNoSign) {
    for (int j = 0; j < 2; j ++) {
      if (deltaWithSign < 0 && (i - lowerBound == 0.0)) {
        continue;
      }
      if (deltaWithSign > 0 && (i - upperBound >= 0.0)) {
        continue;
      }
      xValueE.assignValue(calculator, i);
      functionValueExpressioNonEvaluated = (j == 0) ?
      lowerFunctionE :
      upperFunctionE;
      functionValueExpressioNonEvaluated.substituteRecursively(
        xExpression, xValueE
      );
      if (
        !calculator.evaluateExpression(
          calculator, functionValueExpressioNonEvaluated, functionValueFinal
        )
      ) {
        return false;
      }
      double finalResultDouble;
      if (!functionValueFinal.evaluatesToDouble(&finalResultDouble)) {
        return
        calculator
        << "<hr>Failed to evaluate your function at point "
        << i
        << ", instead "
        << "I evaluated to "
        << functionValueFinal.toString();
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
        outTex
        << "\\psline*[linecolor ="
        << "\\fcColorNegativeAreaUnderGraph, linewidth = 0.1pt]";
        outHtml
        << "<br>\\psline*[linecolor ="
        << "\\fcColorNegativeAreaUnderGraph, linewidth = 0.1pt]";
      }
      if (k == 0 && !useNegativePattern) {
        outTex
        << "\\psline*[linecolor ="
        << "\\fcColorAreaUnderGraph, linewidth = 0.1pt]";
        outHtml
        << "<br>\\psline*[linecolor ="
        << "\\fcColorAreaUnderGraph, linewidth = 0.1pt]";
      }
      if (k > 0 && useNegativePattern) {
        outTex << "\\psline[linecolor =brown, linewidth = 0.1pt]";
        outHtml << "<br>\\psline[linecolor =brown, linewidth = 0.1pt]";
      }
      if (k > 0 && !useNegativePattern) {
        outTex << "\\psline[linecolor =blue, linewidth = 0.1pt]";
        outHtml << "<br>\\psline[linecolor =blue, linewidth = 0.1pt]";
      }
      outTex
      << "("
      << std::fixed
      << xValues[i]
      << ", "
      << std::fixed
      << fValuesLower[i]
      << ")("
      << std::fixed
      << xValues[i]
      << ", "
      << std::fixed
      << fValuesUpper[i]
      << ")"
      << "("
      << std::fixed
      << xValues[i] + deltaWithSign
      << ", "
      << std::fixed
      << fValuesUpper[i]
      << ")("
      << std::fixed
      << xValues[i] + deltaWithSign
      << ", "
      << std::fixed
      << fValuesLower[i]
      << ")"
      << "("
      << std::fixed
      << xValues[i]
      << ", "
      << std::fixed
      << fValuesLower[i]
      << ")";
      outHtml
      << "("
      << std::fixed
      << xValues[i]
      << ", "
      << std::fixed
      << fValuesLower[i]
      << ")("
      << std::fixed
      << xValues[i]
      << ", "
      << std::fixed
      << fValuesUpper[i]
      << ")"
      << "("
      << std::fixed
      << xValues[i] + deltaWithSign
      << ", "
      << std::fixed
      << fValuesUpper[i]
      << ")("
      << std::fixed
      << xValues[i] + deltaWithSign
      << ", "
      << std::fixed
      << fValuesLower[i]
      << ")"
      << "("
      << std::fixed
      << xValues[i]
      << ", "
      << std::fixed
      << fValuesLower[i]
      << ")";
    }
  }
  outHtml << "<br>";
  for (int i = 0; i < rValues.size; i ++) {
    std::stringstream tempStream;
    tempStream
    << "\\rput[t]("
    << std::fixed
    << rValues[i].getDoubleValue()
    << ",-0.03)"
    << "{$";
    if (rValues[i].isInteger()) {
      tempStream << rValues[i].toString();
    } else {
      tempStream
      << "\\frac{"
      << rValues[i].getNumerator().toString()
      << "}"
      << "{"
      << rValues[i].getDenominator().toString()
      << "}";
    }
    tempStream << "$}";
    outHtml << tempStream.str();
    outTex << tempStream.str();
  }
  outHtml << "<br>";
  PlotObject plot;
  plot.plotString = outTex.str();
  plot.plotStringWithHtml = outHtml.str();
  plot.xLow = lowerBound;
  plot.xHigh = upperBound;
  plot.yLow = yMin;
  plot.yHigh = yMax;
  plot.coordinateFunctionsE.addOnTop(input[1]);
  Plot plotFinal;
  plotFinal += plot;
  plotFinal += outputPlot;
  return output.assignValue(calculator, plotFinal);
}

bool CalculatorFunctionsPlot::plotPolarRfunctionTheta(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPlot::plotPolarRfunctionTheta");
  if (input.size() < 4) {
    return
    output.assignError(
      calculator,
      "Drawing polar coordinates takes at least three arguments: "
      "function, lower angle bound and upper angle bound. "
    );
  }
  if (input.size() < 4) {
    return
    calculator
    << "Parametric curve plots take 3+ arguments. The first argument gives "
    << "the coordinate functions in the format (f_1, f_2) or (f_1, f_2,f_3), "
    << "the next two arguments stands for the variable range.";
  }
  if (input.hasBoundVariables()) {
    return false;
  }
  const Expression& polarE = input[1];
  HashedList<Expression> variables;
  if (!polarE.getFreeVariables(variables, true)) {
    return false;
  }
  if (variables.size > 1) {
    return calculator << "Polar radius must depend on a single variable. ";
  }
  if (variables.size == 0) {
    variables.addOnTop(calculator.getNewAtom());
  }
  Expression sine(calculator);
  Expression cosine(calculator);
  sine.addChildAtomOnTop(calculator.opSin());
  cosine.addChildAtomOnTop(calculator.opCos());
  sine.addChildOnTop(variables[0]);
  cosine.addChildOnTop(variables[0]);
  Expression xCoordinate = cosine * polarE;
  Expression yCoordinate = sine * polarE;
  Expression newArg;
  newArg.makeSequence(calculator);
  newArg.addChildOnTop(xCoordinate);
  newArg.addChildOnTop(yCoordinate);
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
  STACK_TRACE("CalculatorFunctionsPlot::plotPolarRfunctionThetaExtended");
  if (input.size() < 4) {
    return
    output.assignError(
      calculator,
      "Drawing polar coordinates takes three arguments: "
      "function, lower angle bound and upper angle bound. "
    );
  }
  Expression plotXYE, plotRthetaE;
  if (
    !calculator.callCalculatorFunction(
      CalculatorFunctionsPlot::plotPolarRfunctionTheta, input, plotXYE
    )
  ) {
    return false;
  }
  if (
    !calculator.callCalculatorFunction(
      CalculatorFunctionsPlot::plot2D, input, plotRthetaE
    )
  ) {
    return false;
  }
  return
  output.makeXOX(
    calculator, calculator.opSequence(), plotXYE, plotRthetaE
  );
}

bool CalculatorFunctionsPlot::plotParametricCurve(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPlot::plotParametricCurve");
  if (input.size() < 4) {
    return
    calculator
    << "Parametric curve plots take 3+ arguments. "
    << "The first argument gives "
    << "the coordinate functions in the format "
    << "(f_1, f_2) or (f_1, f_2, f_3), "
    << "the next two arguments stands for the variable range. ";
  }
  if (input.hasBoundVariables()) {
    return false;
  }
  PlotObject plot;
  if (!input[1].isSequenceNElements()) {
    return
    calculator
    << "The first argument of parametric curve "
    << "must be a sequence, instead I got: "
    << input[1].toString();
  }
  plot.dimension = input[1].size() - 1;
  for (int i = 0; i < plot.dimension; i ++) {
    plot.coordinateFunctionsE.addOnTop(input[1][i + 1]);
    plot.coordinateFunctionsE[i].getFreeVariables(
      plot.variablesInPlay, true
    );
  }
  if (plot.variablesInPlay.size > 1) {
    return
    calculator
    << "Curve is allowed to depend on at most 1 parameter. "
    << "Instead, your curve: "
    << input.toString()
    << " depends on "
    << plot.variablesInPlay.size
    << ", namely: "
    << plot.variablesInPlay.toStringCommaDelimited()
    << ". ";
  }
  if (plot.variablesInPlay.size == 0) {
    Expression tempE;
    tempE.makeAtom(calculator, "t");
    plot.variablesInPlay.addOnTop(tempE);
  }
  plot.variablesInPlayJS.addOnTop(
    HtmlRoutines::getJavascriptVariable(
      plot.variablesInPlay[0].toString()
    )
  );
  plot.colorJS = "red";
  plot.colorRGB = static_cast<int>(
    HtmlRoutines::redGreenBlue(255, 0, 0)
  );
  if (input.size() >= 5) {
    if (!input[4].isOfType<std::string>(&plot.colorJS)) {
      plot.colorJS = input[4].toString();
    }
  }
  DrawingVariables::getColorIntFromColorString(
    plot.colorJS, plot.colorRGB
  );
  plot.lineWidth = 1;
  if (input.size() >= 6) {
    if (!input[5].evaluatesToDouble(&plot.lineWidth)) {
      plot.lineWidth = 1;
    }
  }
  int numberOfPoints = 1000;
  if (input.size() >= 7) {
    if (!input[6].isSmallInteger(&numberOfPoints)) {
      numberOfPoints = 1000;
      calculator
      << "<hr>Could not extract number of points from "
      << input[6].toString();
    }
    plot.numSegmentsE = input[6];
  }
  if (numberOfPoints < 2 || numberOfPoints > 30000) {
    numberOfPoints = 1000;
    calculator
    << "<hr>Extracted "
    << numberOfPoints
    << " point but that is not valid. Changing to 1000. ";
  }
  if (input.size() < 7) {
    plot.numSegmentsE.assignValue(calculator, numberOfPoints);
  }
  List<Expression> convertedExpressions;
  convertedExpressions.setSize(plot.dimension);
  plot.paramLowE = input[2];
  plot.paramHighE = input[3];
  if (
    !plot.paramLowE.evaluatesToDouble(&plot.paramLow) ||
    !plot.paramHighE.evaluatesToDouble(&plot.paramHigh)
  ) {
    calculator
    << "Failed to convert "
    << plot.paramLowE.toString()
    << " and "
    << plot.paramHighE.toString()
    << " to left and right endpoint of parameter interval. ";
  }
  Vectors<double> xCoordinates, yCoordinates;
  bool isGoodLatexWise = true;
  for (int i = 0; i < plot.dimension; i ++) {
    if (
      !calculator.callCalculatorFunction(
        CalculatorFunctions::suffixNotationForPostScript,
        plot.coordinateFunctionsE[i],
        convertedExpressions[i]
      )
    ) {
      calculator
      << "Failed to extract suffix notation from argument "
      << plot.coordinateFunctionsE[i].toString();
      isGoodLatexWise = false;
      break;
    }
  }
  if (isGoodLatexWise && plot.dimension == 2) {
    std::stringstream outLatex, outHtml;
    outLatex
    << "\\parametricplot[linecolor =\\fcColorGraph, plotpoints ="
    << numberOfPoints
    << "]{"
    << plot.paramLow
    << "}{"
    << plot.paramHigh
    << "}{"
    << convertedExpressions[0].getValue<std::string>()
    << convertedExpressions[1].getValue<std::string>()
    << "}";
    outHtml
    << "<br>%Calculator input: "
    << input.toString()
    << "<br>\\parametricplot[linecolor =\\fcColorGraph, plotpoints ="
    << numberOfPoints
    << "]{"
    << plot.paramLow
    << "}{"
    << plot.paramHigh
    << "}{"
    << convertedExpressions[0].getValue<std::string>()
    << convertedExpressions[1].getValue<std::string>()
    << "}";
    plot.plotString = outLatex.str();
    plot.plotStringWithHtml = outHtml.str();
  }
  Expression converterE;
  plot.plotType = PlotObject::PlotTypes::parametricCurve;
  plot.coordinateFunctionsJS.setSize(plot.dimension);
  for (int i = 0; i < plot.dimension; i ++) {
    if (
      CalculatorFunctions::functionMakeJavascriptExpression(
        calculator, plot.coordinateFunctionsE[i], converterE
      )
    ) {
      plot.coordinateFunctionsJS[i] = converterE.toString();
    } else {
      plot.plotType = "parametricCurvePrecomputed";
      calculator
      << "Failed to convert: "
      << plot.coordinateFunctionsE[i]
      << " to js. ";
    }
  }
  plot.numberOfSegmentsJS.setSize(1);
  plot.numberOfSegmentsJS[0] = "200";
  if (
    CalculatorFunctions::functionMakeJavascriptExpression(
      calculator, plot.numSegmentsE, converterE
    )
  ) {
    plot.numberOfSegmentsJS[0] = converterE.toString();
  } else {
    plot.plotType = "parametricCurvePrecomputMakeBoxed";
    calculator << "Failed to convert: " << plot.numSegmentsE << " to js. ";
  }
  if (
    CalculatorFunctions::functionMakeJavascriptExpression(
      calculator, plot.paramLowE, converterE
    )
  ) {
    plot.paramLowJS = converterE.toString();
  } else {
    plot.plotType = "parametricCurvePrecomputed";
    calculator << "Failed to convert: " << plot.paramLowE << " to js. ";
  }
  if (
    CalculatorFunctions::functionMakeJavascriptExpression(
      calculator, plot.paramHighE, converterE
    )
  ) {
    plot.paramHighJS = converterE.toString();
  } else {
    plot.plotType = "parametricCurvePrecomputed";
    calculator << "Failed to convert: " << plot.paramHighE << " to js. ";
  }
  if (plot.dimension == 2) {
    if (
      !plot.coordinateFunctionsE[0].evaluatesToDoubleInRange(
        plot.variablesInPlay[0].toString(),
        plot.paramLow,
        plot.paramHigh,
        numberOfPoints,
        &plot.xLow,
        &plot.xHigh,
        &xCoordinates
      )
    ) {
      calculator << "<hr>Failed to evaluate curve function. ";
    }
    if (
      !plot.coordinateFunctionsE[1].evaluatesToDoubleInRange(
        plot.variablesInPlay[0].toString(),
        plot.paramLow,
        plot.paramHigh,
        numberOfPoints,
        &plot.yLow,
        &plot.yHigh,
        &yCoordinates
      )
    ) {
      calculator << "<hr>Failed to evaluate curve function. ";
    }
    plot.pointsDouble.setSize(xCoordinates.size);
    for (int i = 0; i < xCoordinates.size; i ++) {
      plot.pointsDouble[i].setSize(2);
      plot.pointsDouble[i][0] = xCoordinates[i][1];
      plot.pointsDouble[i][1] = yCoordinates[i][1];
    }
  }
  input.hasInputBoxVariables(
    &plot.parametersInPlay, &plot.parametersInPlayJS
  );
  Plot outputPlot;
  outputPlot += plot;
  return output.assignValue(calculator, outputPlot);
}

bool CalculatorFunctions::evaluateToDouble(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("Expression::evaluateToDouble");
  if (input.size() != 2) {
    // one argument expected.
    return false;
  }
  double value = 0;
  if (!input[1].evaluatesToDouble(&value)) {
    return false;
  }
  return output.assignValue(calculator, value);
}

bool CalculatorFunctions::functionEvaluateToDouble(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("Expression::functionEvaluateToDouble");
  double value = 0;
  if (!input.evaluatesToDouble(&value)) {
    return false;
  }
  return output.assignValue(calculator, value);
}

bool CalculatorFunctions::embedSemisimpleAlgebraInSemisimpleAlgebra(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctions::embedSemisimpleAlgebraInSemisimpleAlgebra"
  );
  if (input.size() != 3) {
    return
    output.assignError(
      calculator, "I expect two arguments - the two semisimple subalgebras."
    );
  }
  const Expression& eSmallSA = input[1];
  const Expression& eLargeSA = input[2];
  WithContext<SemisimpleLieAlgebra*> smallSubalgebraPointer;
  if (
    !CalculatorConversions::convert(
      calculator, eSmallSA, smallSubalgebraPointer
    )
  ) {
    return output.assignError(calculator, "Error extracting Lie algebra.");
  }
  WithContext<SemisimpleLieAlgebra*> largeSubalgebraPointer;
  if (
    !CalculatorConversions::convert(
      calculator, eLargeSA, largeSubalgebraPointer
    )
  ) {
    return output.assignError(calculator, "Error extracting Lie algebra.");
  }
  SemisimpleLieAlgebra& semisimpleLieAlgebra = *largeSubalgebraPointer.content;
  std::stringstream out;
  if (semisimpleLieAlgebra.getRank() > 8) {
    out
    << "<b>This code is has been set to run "
    << "up to ambient Lie algebra of rank 8. </b>";
    return output.assignValue(calculator, out.str());
  }
  SemisimpleSubalgebras& semisimpleSubalgebras =
  calculator.objectContainer.getSemisimpleSubalgebrasCreateIfNotPresent(
    semisimpleLieAlgebra.weylGroup.dynkinType
  );
  semisimpleSubalgebras.toStringExpressionString =
  CalculatorConversions::stringFromSemisimpleSubalgebras;
  out
  << "Attempting to embed "
  << smallSubalgebraPointer.content->weylGroup.dynkinType.toString()
  << " in "
  << semisimpleLieAlgebra.toStringLieAlgebraName();
  semisimpleSubalgebras.findTheSemisimpleSubalgebrasFromScratch(
    semisimpleLieAlgebra,
    calculator.objectContainer.algebraicClosure,
    calculator.objectContainer.semisimpleLieAlgebras,
    calculator.objectContainer.slTwoSubalgebras,
    &smallSubalgebraPointer.content->weylGroup.dynkinType
  );
  return output.assignValue(calculator, semisimpleSubalgebras);
}

bool CalculatorFunctions::allPartitions(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::allPartitions");
  if (input.size() != 2) {
    return false;
  }
  int rank = - 1;
  if (!input[1].isSmallInteger(&rank)) {
    return false;
  }
  if (rank > 33 || rank < 0) {
    return
    calculator
    << "Partitions printouts are limited "
    << "from n = 0 to n = 33, your input was: "
    << input[1].toString();
  }
  List<Partition> partitions;
  Partition::GetPartitions(partitions, rank);
  std::stringstream out;
  out
  << "The partitions of "
  << rank
  << " (total: "
  << partitions.size
  << ")"
  << ": ";
  for (int i = 0; i < partitions.size; i ++) {
    out << "<br>" << partitions[i].toString();
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctions::allVectorPartitions(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::allVectorPartitions");
  if (input.size() != 3) {
    return calculator << "<hr>AllVectorPartitions function takes 3 arguments.";
  }
  VectorPartition partition;
  const Expression& vectorExpression = input[1];
  const Expression& partitioningVectorsExpression = input[2];
  if (!calculator.getVector(vectorExpression, partition.targetSum)) {
    return
    calculator
    << "<hr>Failed to extract vector from "
    << vectorExpression.toString();
  }
  Matrix<Rational> vectorsMatForm;
  if (
    !CalculatorConversions::functionGetMatrix(
      calculator,
      partitioningVectorsExpression,
      vectorsMatForm,
      false,
      nullptr,
      partition.targetSum.size
    )
  ) {
    return
    calculator
    << "<hr>Failed to extract list of vectors from "
    << partitioningVectorsExpression.toString();
  }
  Vectors<Rational> inputVectors;
  inputVectors.assignMatrixRows(vectorsMatForm);
  for (int i = 0; i < inputVectors.size; i ++) {
    if (!inputVectors[i].isPositive()) {
      return
      calculator
      << "<hr>Input vector "
      << inputVectors[i].toString()
      << " is non-positive";
    }
  }
  if (!partition.initialize(inputVectors, partition.targetSum)) {
    return calculator << "<hr>Failed to initialize vector partition object";
  }
  std::stringstream outFinal;
  int numFound = 0;
  ProgressReport report;
  outFinal << partition.toStringPartitioningVectors();
  std::stringstream outVectors;
  int count = 0;
  while (partition.incrementReturnFalseIfPastLast()) {
    count ++;
    outVectors
    << "<br>"
    << partition.toStringOnePartition(partition.currentPartition);
    numFound ++;
    if (numFound % 1000 == 0) {
      std::stringstream reportStream;
      reportStream
      << "Found "
      << numFound
      << " partitions of "
      << partition.targetSum.toString()
      << "<br>Current partition: "
      << partition.currentPartition;
      report.report(reportStream.str());
    }
  }
  outFinal << count << " partition(s) total." << outVectors.str();
  return output.assignValue(calculator, outFinal.str());
}

template <class Coefficient>
bool CalculatorFunctions::functionDeterminant(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  int maxiumDimension,
  bool convertByComputation
) {
  STACK_TRACE("CalculatorFunctions::functionDeterminant");
  Matrix<Coefficient> matrix;
  ExpressionContext context;
  if (
    !CalculatorConversions::functionGetMatrix<Coefficient>(
      calculator,
      input,
      matrix,
      convertByComputation,
      &context,
      - 1,
      &calculator.comments
    )
  ) {
    return false;
  }
  if (matrix.numberOfRows == 0) {
    calculator << "Matrix has zero rows: " << input.toString() << ".";
    return
    output.assignError(
      calculator, "Request to compute determinant of emptry matrix. "
    );
  }
  if (matrix.numberOfRows != matrix.numberOfColumns) {
    calculator
    << "Request to compute determinant of the non-square "
    << matrix.numberOfRows
    << " by "
    << matrix.numberOfColumns
    << " matrix: "
    << input.toString();
    return
    output.assignError(
      calculator, "Request to compute determinant of non-square matrix. "
    );
  }
  if (matrix.numberOfRows > maxiumDimension) {
    return
    calculator
    << "<hr>Maximum dimension for "
    << "determinant computation of the given type is "
    << maxiumDimension
    << " by "
    << maxiumDimension
    << ". Your matrix is "
    << matrix.numberOfRows
    << " by "
    << matrix.numberOfColumns
    << ". "
    << "The restrictions are set around file: "
    << __FILE__
    << ", line "
    << __LINE__
    << ". ";
  }
  Coefficient result;
  const Coefficient& sample = matrix(0, 0);
  matrix.computeDeterminantOverwriteMatrix(
    result, sample.one(), sample.zero()
  );
  return output.assignValueWithContext(calculator, result, context);
}

bool CalculatorFunctions::determinant(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::determinant");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (
    CalculatorFunctions::functionDeterminant<Rational>(
      calculator, argument, output, 100, false
    )
  ) {
    return true;
  }
  if (
    CalculatorFunctions::functionDeterminant<ElementZmodP>(
      calculator, argument, output, 100, false
    )
  ) {
    return true;
  }
  if (
    CalculatorFunctions::functionDeterminant<AlgebraicNumber>(
      calculator, argument, output, 20, true
    )
  ) {
    return true;
  }
  if (
    CalculatorFunctions::functionDeterminant<RationalFraction<Rational> >(
      calculator, argument, output, 10, true
    )
  ) {
    return true;
  }
  return false;
}

bool CalculatorFunctions::highestWeightTransposeAntiAutomorphismBilinearForm(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctions::highestWeightTransposeAntiAutomorphismBilinearForm"
  );
  RecursionDepthCounter recursionCounter(&calculator.recursionDepth);
  if (!input.isListNElements(4)) {
    return output.assignError(calculator, "Function expects three arguments.");
  }
  Expression leftMerged = input[1];
  Expression rightMerged = input[2];
  if (!Expression::mergeContexts(leftMerged, rightMerged)) {
    return
    calculator
    << "Could not get elements of universal enveloping algebra from inputs: "
    << input[1].toString()
    << " and "
    << input[2].toString();
  }
  ExpressionContext finalContext = rightMerged.getContext();
  int algebraIndex = finalContext.indexAmbientSemisimpleLieAlgebra;
  if (algebraIndex == - 1) {
    return
    output.assignError(
      calculator, "I couldn't extract a Lie algebra to compute hwtaabf."
    );
  }
  SemisimpleLieAlgebra& constantSemisimpleLieAlgebra =
  calculator.objectContainer.semisimpleLieAlgebras.values[algebraIndex];
  const Expression& weightExpression = input[3];
  Vector<RationalFraction<Rational> > weight;
  if (
    !calculator.getVector<RationalFraction<Rational> >(
      weightExpression,
      weight,
      &finalContext,
      constantSemisimpleLieAlgebra.getRank()
    )
  ) {
    return
    calculator
    << "<hr>Failed to obtain highest weight from the third argument which is "
    << weightExpression.toString();
  }
  if (
    !leftMerged.setContextAtLeastEqualTo(
      finalContext, &calculator.comments
    ) ||
    !rightMerged.setContextAtLeastEqualTo(
      finalContext, &calculator.comments
    )
  ) {
    return
    output.assignError(
      calculator,
      "Failed to merge the contexts of the highest "
      "weight and the elements of the Universal enveloping. "
    );
  }
  WithContext<ElementUniversalEnveloping<RationalFraction<Rational> > >
  leftConverted,
  rightConverted;
  if (
    !CalculatorConversions::convertWithoutComputation(
      calculator, leftMerged, leftConverted
    )
  ) {
    return false;
  }
  if (
    !CalculatorConversions::convertWithoutComputation(
      calculator, rightMerged, rightConverted
    )
  ) {
    return false;
  }
  WeylGroupData& weylGroup = constantSemisimpleLieAlgebra.weylGroup;
  Vector<RationalFraction<Rational> > highestWeightDualCoordinates;
  constantSemisimpleLieAlgebra.orderSSalgebraForHWbfComputation();
  highestWeightDualCoordinates =
  weylGroup.getDualCoordinatesFromFundamental(weight);
  RationalFraction<Rational> outputRationalFraction;
  if (
    !leftConverted.content.highestWeightTransposeAntiAutomorphismBilinearForm(
      rightConverted.content,
      outputRationalFraction,
      &highestWeightDualCoordinates,
      1,
      0,
      &calculator.comments
    )
  ) {
    return
    output.assignError(
      calculator, "Error: couldn't compute Shapovalov form, see comments."
    );
  }
  constantSemisimpleLieAlgebra.orderStandardAscending();
  return
  output.assignValueWithContext(
    calculator, outputRationalFraction, finalContext
  );
}

bool Expression::evaluatesToDoubleInRange(
  const std::string& variableName,
  double lowBound,
  double highBound,
  int numberOfPoints,
  double* outputYmin,
  double* outputYmax,
  Vectors<double>* outputPoints
) const {
  STACK_TRACE("Expression::evaluatesToDoubleInRange");
  if (numberOfPoints < 1 || this->owner == nullptr) {
    return false;
  }
  HashedList<Expression> knownEs = this->owner->knownDoubleConstants;
  List<double> knownValues = this->owner->knownDoubleConstantValues;
  Expression variableExpression;
  variableExpression.makeAtom(*this->owner, variableName);
  if (knownEs.contains(variableExpression)) {
    return
    *(this->owner)
    << "Variable name is an already known constant, variable name is bad.";
  }
  knownEs.addOnTop(variableExpression);
  knownValues.addOnTop(0);
  int numPoints = numberOfPoints + 1;
  double delta = (highBound - lowBound) / (numberOfPoints);
  *knownValues.lastObject() = lowBound;
  double currentValue = 0;
  if (outputPoints != nullptr) {
    outputPoints->setSize(numPoints);
  }
  bool result = true;
  int numberOfFailedEvaluations = 0;
  for (int i = 0; i < numPoints; i ++) {
    if (i == numPoints - 1) {
      *knownValues.lastObject() = highBound;
      // correcting for floating point errors.
    }
    if (
      !this->evaluatesToDoubleUnderSubstitutions(
        knownEs, knownValues, &currentValue
      )
    ) {
      numberOfFailedEvaluations ++;
      if (numberOfFailedEvaluations < 5) {
        *(this->owner)
        << "<br>Failed to evaluate "
        << this->toString()
        << " at "
        << variableName
        << "="
        << *knownValues.lastObject()
        << ". ";
      }
      if (numberOfFailedEvaluations == 5) {
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
  if (numberOfFailedEvaluations >= 5) {
    (*this->owner)
    << "<br>"
    << numberOfFailedEvaluations
    << " evaluations total failed. ";
  }
  return result;
}

bool Expression::evaluatesToDouble(double* whichDouble) const {
  return
  this->evaluatesToDoubleUnderSubstitutions(
    this->owner->knownDoubleConstants,
    this->owner->knownDoubleConstantValues,
    whichDouble
  );
}

bool Expression::evaluatesToDoubleUnderSubstitutions(
  const HashedList<Expression>& knownEs,
  const List<double>& valuesKnownEs,
  double* whichDouble
) const {
  STACK_TRACE("Expression::evaluatesToDoubleUnderSubstitutions");
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
    if (
      this->getValue<AlgebraicNumber>().evaluatesToDouble(whichDouble)
    ) {
      return true;
    }
  }
  RecursionDepthCounter counter(&this->owner->recursionDepth);
  if (this->owner->recursionDepth > this->owner->maximumRecursionDepth) {
    return
    *(this->owner)
    << "<hr>Recursion depth exceeded while "
    << "evaluating innerEvaluateToDouble. "
    << "This may be a programming error. ";
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
  this->startsWith(calculator.opPower(), 3) ||
  this->startsWith(calculator.opDivide(), 3) ||
  this->startsWith(calculator.opSqrt(), 3) ||
  this->startsWith(calculator.opLogBase(), 3);
  if (isArithmeticOperationTwoArguments) {
    double leftD, rightD;
    if (
      !(*this)[1].evaluatesToDoubleUnderSubstitutions(
        knownEs, valuesKnownEs, &leftD
      ) ||
      !(*this)[2].evaluatesToDoubleUnderSubstitutions(
        knownEs, valuesKnownEs, &rightD
      )
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
        *whichDouble = (FloatingPoint::logFloating(rightD)) / (
          FloatingPoint::logFloating(leftD)
        );
      }
      return true;
    }
    if ((*this).startsWith(calculator.opPower(), 3)) {
      bool signChange = false;
      if (leftD < 0) {
        Rational rational;
        if ((*this)[2].isRational(&rational)) {
          if (!rational.getDenominator().isEven()) {
            if (!rational.getNumerator().isEven()) {
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
      return !FloatingPoint::isNaN(*whichDouble) && !std::isinf(*whichDouble);
    }
    if ((*this).startsWith(calculator.opSqrt(), 3)) {
      bool signChange = false;
      if (rightD < 0) {
        Rational rational;
        if ((*this)[1].isRational(&rational)) {
          if (!rational.getNumerator().isEven()) {
            if (!rational.getDenominator().isEven()) {
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
      return !FloatingPoint::isNaN(*whichDouble) && !std::isinf(*whichDouble);
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
    global.fatal
    << "This is a piece of code which should never be reached. "
    << global.fatal;
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
    if (
      !(*this)[1].evaluatesToDoubleUnderSubstitutions(
        knownEs, valuesKnownEs, &argumentD
      )
    ) {
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

bool CalculatorFunctions::testTopCommand(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::testTopCommand");
  (void) input;
  (void) calculator;
  // portable way of avoiding unused parameter warning
  if (!global.userDefaultHasAdminRights()) {
    return
    calculator
    << "Test top command available only to logged-in admins. ";
  }
  return
  output.assignValue(calculator, global.toHTMLTopCommandLinuxSystem());
}

bool CalculatorFunctions::testIndicator(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::testIndicator");
  if (global.response.flagBanProcessMonitoring) {
    std::stringstream out;
    out << "The server's admins have explicitly banned monitoring. ";
    return output.assignValue(calculator, out.str());
  }
  if (!global.response.flagReportDesired) {
    std::stringstream out;
    out << "Process monitoring turned off by user. ";
    return output.assignValue(calculator, out.str());
  }
  if (input.size() < 3) {
    return
    calculator
    << "Testing indicator requires two arguments: "
    << "number of iterations and size of dummy comment. ";
  }
  int numRuns = - 1;
  if (!input[1].isIntegerFittingInInt(&numRuns)) {
    return
    calculator
    << "Argument of CalculatorFunctions::testIndicator "
    << "is not a small integer, instead it equals: "
    << input[1]
    << ".";
  }
  if (numRuns > 200000) {
    calculator
    << "The argument "
    << numRuns
    << " of CalculatorFunctions::testIndicator "
    << "larger than 200000, trimming down to 200000.";
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
    calculator
    << "Dummy comment size of "
    << dummyCommentSize
    << " reduced to 200000.";
    dummyCommentSize = 200000;
  }
  if (dummyCommentSize < 0) {
    dummyCommentSize = 0;
  }
  std::string dummyComment;
  dummyComment.resize(static_cast<unsigned>(dummyCommentSize));
  for (
    unsigned i = 0; i < static_cast<unsigned>(dummyCommentSize); i ++
  ) {
    dummyComment[i] = 'a';
  }
  global.response.initiate("Triggered by test indicator. ");
  ProgressReport report;
  for (int i = 0; i < numRuns; i ++) {
    std::stringstream reportStream;
    reportStream
    << " Running indicator test, "
    << i + 1
    << " out of "
    << numRuns
    << ".";
    report.report(reportStream.str());
    global.fallAsleep(4000);
  }
  std::stringstream out;
  out << numRuns << " iterations of the indicator test executed. ";
  if (dummyCommentSize > 0) {
    calculator << "Dummy comment:<br>" << dummyComment;
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsFreecalc::crawlTexFile(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsFreecalc::crawlTexFile");
  if (!global.userDefaultHasAdminRights()) {
    std::stringstream out;
    out << "Command available to logged-in admins only. ";
    return output.assignValue(calculator, out.str());
  }
  if (input.size() != 2) {
    return
    calculator
    << "<hr>Input "
    << input.toString()
    << " requires a single argument. ";
  }
  std::string argument;
  if (!input[1].isOfType<std::string>(&argument)) {
    return
    calculator
    << "<hr>Input "
    << input.toString()
    << " is not of type string. ";
  }
  LaTeXCrawler crawler;
  crawler.ownerCalculator = &calculator;
  crawler.fileNameToCrawlRelative = argument;
  crawler.crawl();
  return output.assignValue(calculator, crawler.displayResult.str());
}

bool CalculatorFunctionsFreecalc::buildFreecalcSlidesOnTopic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsFreecalc::buildFreecalcSlidesOnTopic");
  if (!global.userDefaultHasAdminRights()) {
    std::stringstream out;
    out << "Command available to logged-in admins only. ";
    return output.assignValue(calculator, out.str());
  }
  if (input.size() < 2) {
    return false;
  }
  int topicNumber = 0;
  if (!input[1].isSmallInteger(&topicNumber)) {
    return
    calculator
    << "Expected a topic number, got: "
    << input[1].toString();
  }
  LaTeXCrawler crawler;
  crawler.flagBuildSingleSlides = true;
  crawler.ownerCalculator = &calculator;
  std::stringstream out;
  crawler.buildTopicList(topicNumber, &out, &out);
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsFreecalc::buildFreecalcSingleSlides(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsFreecalc::buildFreecalcSingleSlides");
  if (!global.userDefaultHasAdminRights()) {
    std::stringstream out;
    out << "Command available to logged-in admins only. ";
    return output.assignValue(calculator, out.str());
  }
  if (input.size() != 2) {
    return false;
  }
  if (!input[1].isOfType<std::string>()) {
    return
    calculator
    << "<hr>Input "
    << input.toString()
    << " is not of type string. ";
  }
  LaTeXCrawler crawler;
  crawler.flagBuildSingleSlides = true;
  crawler.ownerCalculator = &calculator;
  crawler.fileNameToCrawlRelative = input[1].getValue<std::string>();
  StateMaintainerCurrentFolder maintainFolder;
  crawler.buildFreecalc();
  return
  output.assignValue(
    calculator,
    crawler.displayResult.str() + crawler.errorStream.str()
  );
}

bool CalculatorFunctionsFreecalc::buildFreecalc(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsFreecalc::buildFreecalc");
  if (!global.userDefaultHasAdminRights()) {
    std::stringstream out;
    out << "Command available to logged-in admins only. ";
    return output.assignValue(calculator, out.str());
  }
  if (input.size() != 2) {
    return
    calculator
    << "<hr>Building freecalc requires exactly one argument. ";
  }
  if (!input[1].isOfType<std::string>()) {
    return
    calculator
    << "<hr>Input "
    << input.toString()
    << " is not of type string. ";
  }
  LaTeXCrawler crawler;
  crawler.flagBuildSingleSlides = false;
  crawler.ownerCalculator = &calculator;
  crawler.fileNameToCrawlRelative = input[1].getValue<std::string>();
  std::string startingFolder = FileOperations::getCurrentFolder();
  crawler.buildFreecalc();
  global.changeDirectory(startingFolder);
  return output.assignValue(calculator, crawler.displayResult.str());
}

bool CalculatorFunctions::findProductDistanceModN(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::findProductDistanceModN");
  if (input.size() != 3) {
    return
    calculator
    << "<hr>Product distance f-n takes "
    << "as input 2 arguments, modulo and a list of integers. ";
  }
  const Expression& moduloExpression = input[1];
  const Expression& integersExpression = input[2];
  int inputSize;
  if (!moduloExpression.isIntegerFittingInInt(&inputSize)) {
    return
    calculator
    << " <hr> Failed to extract modulus from "
    << moduloExpression.toString();
  }
  if (inputSize == 0) {
    return calculator << "<hr>zero modulus not allowed.";
  }
  if (inputSize < 0) {
    inputSize *= - 1;
  }
  if (inputSize > 10000000) {
    return
    calculator
    << "<hr>I've been instructed to compute "
    << "with moduli no larger than 10000000.";
  }
  List<int> integerList, integersReduced;
  if (!calculator.getVectorInt(integersExpression, integerList)) {
    return
    calculator
    << "<hr>Failed to extract integer list from "
    << integersExpression.toString();
  }
  integersReduced.setSize(integerList.size);
  for (int i = 0; i < integerList.size; i ++) {
    if (integerList[i] <= 0) {
      return
      calculator
      << "<hr>The integer list "
      << integerList
      << " contains non-positive numbers. ";
    }
    integersReduced[i] = integerList[i] % inputSize;
  }
  List<LargeIntegerUnsigned> inputList;
  List<int> indexStack;
  inputList.initializeFillInObject(inputSize, 0);
  indexStack.reserve(inputSize);
  LargeIntegerUnsigned modulus;
  modulus = static_cast<unsigned>(inputSize);
  int numberOfElementsCovered = 0;
  for (int i = 0; i < integerList.size; i ++) {
    if (inputList[integersReduced[i]] == 0) {
      numberOfElementsCovered ++;
    }
    inputList[integersReduced[i]] = static_cast<unsigned>(integerList[i]);
    indexStack.addOnTop(integersReduced[i]);
  }
  LargeIntegerUnsigned
  currentIndexLarge,
  currentDistance,
  maxDistanceGenerated;
  int currentIndex;
  ProgressReport report;
  std::stringstream reportstream;
  reportstream
  << "Finding product distance mod "
  << modulus.toString()
  << " w.r.t. elements "
  << integerList;
  int numberOfElementsNotAddedToStack = 0;
  maxDistanceGenerated = 0;
  for (int i = 0; i < indexStack.size; i ++) {
    for (int j = 0; j < integersReduced.size; j ++) {
      currentIndexLarge = static_cast<unsigned>(indexStack[i]);
      currentIndexLarge *= static_cast<unsigned>(integersReduced[j]);
      currentIndexLarge %= modulus;
      if (!currentIndexLarge.isIntegerFittingInInt(&currentIndex)) {
        return
        calculator
        << "An internal check has failed. "
        << "This shouldn't happen, this is possibly a programming bug.";
      }
      currentDistance = inputList[indexStack[i]];
      currentDistance += static_cast<unsigned>(integersReduced[j]);
      if (inputList[currentIndex] > 0) {
        if (inputList[currentIndex] < currentDistance) {
          numberOfElementsNotAddedToStack ++;
          if (numberOfElementsNotAddedToStack % 50000 == 0) {
            std::stringstream out;
            out
            << "While computing product distance, explored "
            << i + 1
            << " out of "
            << indexStack.size
            << " indices. "
            << numberOfElementsNotAddedToStack
            << " candidates were not added to the stack. "
            << "Number of elements reached: "
            << numberOfElementsCovered
            << ". "
            << "Max distance generated while searching: "
            << maxDistanceGenerated.toString();
            report.report(out.str());
          }
          continue;
        }
      }
      if (inputList[currentIndex] == 0) {
        numberOfElementsCovered ++;
      }
      inputList[currentIndex] = currentDistance;
      if (currentDistance > maxDistanceGenerated) {
        maxDistanceGenerated = currentDistance;
      }
      indexStack.addOnTop(currentIndex);
      if (indexStack.size % 10000 == 0) {
        std::stringstream out;
        out
        << "While computing product distance, explored "
        << i + 1
        << " out of "
        << indexStack.size
        << " indices. "
        << numberOfElementsNotAddedToStack
        << " candidates were not added to the stack. "
        << "Number of elements reached: "
        << numberOfElementsCovered
        << ". "
        << "Max distance generated while searching: "
        << maxDistanceGenerated.toString();
        report.report(out.str());
      }
    }
    if (indexStack.size > 1000000000) {
      return
      calculator
      << "While computing product distance, "
      << "exceeded allowed stack size of 1000000000";
    }
  }
  std::stringstream out;
  for (int i = 0; i < inputList.size; i ++) {
    out << "<br>" << i << ": " << inputList[i].toString();
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctions::solveProductSumEquationOverSetModN(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::solveProductSumEquationOverSetModN");
  Expression
  modulusExpression,
  integersExpression,
  productExpression,
  sumExpression;
  if (
    !CalculatorConversions::loadKey(
      calculator, input, "theMod", modulusExpression
    )
  ) {
    return calculator << "<hr>Value theMod not found.";
  }
  int modulusSmall;
  if (!modulusExpression.isIntegerFittingInInt(&modulusSmall)) {
    return
    calculator
    << " <hr> Failed to extract modulus from "
    << modulusExpression.toString();
  }
  if (modulusSmall == 0) {
    return calculator << "<hr>zero modulus not allowed.";
  }
  if (modulusSmall < 0) {
    modulusSmall *= - 1;
  }
  if (modulusSmall > 10000000) {
    return
    calculator
    << "<hr>I've been instructed to compute "
    << "with moduli no larger than 10000000.";
  }
  if (
    !CalculatorConversions::loadKey(
      calculator, input, "theSet", integersExpression
    )
  ) {
    return calculator << "<hr>Value theSet not found.";
  }
  List<int> integerList;
  if (!calculator.getVectorInt(integersExpression, integerList)) {
    return
    calculator
    << "<hr>Failed to extract integer list from "
    << integersExpression.toString();
  }
  for (int i = 0; i < integerList.size; i ++) {
    if (integerList[i] <= 0) {
      return
      calculator
      << "<hr>The integer list "
      << integerList
      << " contains non-positive numbers.";
    }
  }
  if (
    !CalculatorConversions::loadKey(
      calculator, input, "theProduct", productExpression
    )
  ) {
    return calculator << "<hr>Value theProduct not found. ";
  }
  LargeInteger goalProduct;
  if (!productExpression.isInteger(&goalProduct)) {
    return
    calculator
    << "<hr>Failed to extract integer from "
    << productExpression.toString();
  }
  if (goalProduct.isNegative()) {
    return calculator << "<hr>I am expecting a positive product as input. ";
  }
  if (
    !CalculatorConversions::loadKey(
      calculator, input, "theSum", sumExpression
    )
  ) {
    return calculator << "<hr>Value theSum not found. ";
  }
  int integerSum = - 1;
  if (!sumExpression.isSmallInteger(&integerSum)) {
    return
    calculator
    << "Failed to extract small integer from "
    << sumExpression.toString();
  }
  VectorPartition vectorPartition;
  Vectors<Rational> oneDimensionalVectors;
  oneDimensionalVectors.setSize(integerList.size);
  for (int i = 0; i < integerList.size; i ++) {
    oneDimensionalVectors[i].makeZero(1);
    oneDimensionalVectors[i][0] = integerList[i];
  }
  vectorPartition.targetSum.makeZero(1);
  vectorPartition.targetSum[0] = integerSum;
  if (
    !vectorPartition.initialize(
      oneDimensionalVectors, vectorPartition.targetSum
    )
  ) {
    return calculator << "Failed to initialize the computation. ";
  }
  LargeIntegerUnsigned modulusLarge;
  modulusLarge = static_cast<unsigned>(modulusSmall);
  int numTestedSoFar = 0;
  ProgressReport report;
  LargeIntegerUnsigned oneUI = 1;
  while (vectorPartition.incrementReturnFalseIfPastLast()) {
    LargeIntegerUnsigned integerProduct = 1;
    for (int i = 0; i < vectorPartition.currentPartition.size; i ++) {
      LargeIntegerUnsigned number = static_cast<unsigned>(integerList[i]);
      MathRoutines::raiseToPower(
        number, vectorPartition.currentPartition[i], oneUI
      );
      integerProduct *= number;
      integerProduct %= modulusLarge;
    }
    if (integerProduct == goalProduct.value) {
      std::stringstream out;
      out << "Found one solution: ";
      for (int i = 0; i < vectorPartition.currentPartition.size; i ++) {
        if (vectorPartition.currentPartition[i] > 0) {
          out << integerList[i];
          if (vectorPartition.currentPartition[i] > 1) {
            out << "^{" << vectorPartition.currentPartition[i] << "}";
          }
          out << " ";
        }
      }
      return output.assignValue(calculator, out.str());
    }
    numTestedSoFar ++;
    std::stringstream reportStream;
    reportStream << numTestedSoFar << " tested so far ...";
    report.report(reportStream.str());
  }
  return
  output.assignValue(calculator, std::string("Couldn't find solution"));
}

void Calculator::Test::calculatorTestPrepare() {
  STACK_TRACE("Calculator::Test::calculatorTestPrepare");
  if (this->owner == nullptr) {
    global.fatal << "Non-initialized calculator test. " << global.fatal;
  }
  this->commands.clear();
  for (int i = 0; i < this->owner->numberOfPredefinedAtoms; i ++) {
    MemorySaving<Calculator::OperationHandlers>& currentPointer =
    this->owner->operations.values[i];
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
        oneTest.requresAdminAccess = currentFunction.options.adminOnly;
        oneTest.command = currentFunction.example;
        oneTest.atom = this->owner->operations.keys[i];
        oneTest.functionAdditionalIdentifier =
        currentFunction.additionalIdentifier;
        this->commands.setKeyValue(oneTest.command, oneTest);
      }
      currentHandler = &current.compositeHandlers;
    }
  }
}

bool Calculator::Test::calculatorTestRun() {
  STACK_TRACE("Calculator::Test::calculatorTestRun");
  this->debugFlagAtStart = global.getWebInput(WebAPI::request::debugFlag);
  global.setWebInput(WebAPI::request::debugFlag, "false");
  this->calculatorTestPrepare();
  Calculator tester;
  ProgressReport report;
  FormatExpressions format;
  format.flagExpressionIsFinal = true;
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
    Calculator::Test::OneTest& currentTest = this->commands.values[i];
    reportStream
    << "<br>Test progress: testing "
    << i + 1
    << " out of "
    << this->commands.size()
    << ". ";
    reportStream << "<br>Testing expression:<br> " << currentTest.command;
    global
    << Logger::green
    << "Automated test: "
    << i
    << " out of "
    << this->commands.size()
    << ", atom: "
    << currentTest.atom
    << ", command:\n"
    << currentTest.command
    << Logger::endL;
    StateMaintainer<bool> maintainOriginalValueDatabaseFlag;
    maintainOriginalValueDatabaseFlag.initialize(
      global.flagDisableDatabaseLogEveryoneAsAdmin
    );
    if (currentTest.requresAdminAccess) {
      global.flagDisableDatabaseLogEveryoneAsAdmin = true;
    }
    report.report(reportStream.str());
    tester.initialize(Calculator::Mode::full);
    tester.checkConsistencyAfterInitialization();
    tester.evaluate(currentTest.command);
    currentTest.actualResult = tester.programExpression.toString(&format);
    reportStream << "<br>Result: " << tester.programExpression.toString();
    reportStream
    << "<br>Done in: "
    << global.getElapsedSeconds() - this->startTime
    << " seconds. ";
    report.report(reportStream.str());
  }
  global.setWebInput(WebAPI::request::debugFlag, this->debugFlagAtStart);
  return this->processResults();
}

bool CalculatorFunctions::printRuleStack(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::printRuleStack");
  (void) input;
  // portable way of avoiding unused parameter warning
  return output.assignValue(calculator, calculator.ruleStack.toString());
}

bool CalculatorFunctions::crash(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::crash");
  (void) input;
  global.fatal
  << "This is a test of the crashing mechanism. "
  << "A file report must have been written, unless running web assembly. "
  << "The crash file report is not "
  << "accessible through the calculator's webserver. "
  << "It can be reached locally in the "
  << "results/crashes folder of the calculator or, "
  << "if running remotely, through an administrator ssh connection. "
  << global.fatal;
  return
  output.assignValue(calculator, std::string("Crashed succesfully"));
}

bool CalculatorFunctions::crashByListOutOfBounds(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::crashByListOutOfBounds");
  (void) input;
  // portable way of avoiding unused parameter warning
  List<int> list;
  std::vector<int> array;
  for (int i = 0; i < 5; i ++) {
    list.addOnTop(0);
    array.push_back(0);
  }
  list.setSize(0);
  array.resize(0);
  array[1] = 1;
  list[1] = 1;
  return
  output.assignValue(
    calculator, std::string("Crashing: list out of bounds.")
  );
}

bool CalculatorFunctions::crashByVectorOutOfBounds(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::crashByVectorOutOfBounds");
  (void) input;
  // portable way of avoiding unused parameter warning
  std::vector<int> array;
  array[1] = 1;
  return
  output.assignValue(
    calculator, std::string("Crashing: std::vector out of bounds.")
  );
}

bool CalculatorFunctions::transpose(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("Calculator::transpose");
  if (
    !input.isSequenceNElements() &&
    !input.isMatrix() &&
    !input.startsWithGivenOperation(
      Calculator::Functions::Names::transpose
    )
  ) {
    return false;
  }
  Matrix<Expression> matrix;
  if (
    input.startsWithGivenOperation(
      Calculator::Functions::Names::transpose
    )
  ) {
    calculator.getMatrixExpressionsFromArguments(input, matrix);
  } else {
    calculator.getMatrixExpressions(input, matrix);
  }
  // The commented code used to be here. I don't remember why I added it,
  // perhaps there was a solid reason?
  // If the code is uncommented, then ((1,2),(3,5))^t will not be transposed
  // according to expectation.
  // If the commented code needs to be restored, please document why.
  // if (input.isSequenceNElements())
  //   if (matrix.numberOfRows !=1)
  //     return false;
  matrix.transpose();
  return output.assignMatrixExpressions(matrix, calculator, true, true);
}

bool CalculatorFunctionsBinaryOps::powerSequenceOrMatrixByT(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::powerSequenceOrMatrixByT");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!input.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  if (!input[1].isSequenceNElements() && !input[1].isMatrix()) {
    return false;
  }
  if (
    !input[2].isOperationGiven("t") && !input[2].isOperationGiven("T")
  ) {
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
  HashedList<std::string> displayedExpressionStrings;
  List<bool> displayedStringIsLeaf;
  List<Vector<Rational> > nodePositions;
  List<Vector<double> > nodePositionsDouble;
  List<int> layerSizes;
  List<int> layerFirstIndices;
  List<List<int> > arrows;
  Plot plot;
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
    STACK_TRACE("ExpressionTreeDrawer::getCurrentExpression");
    return this->currentLayer[this->indexInCurrentLayer];
  }
  void computeCurrentChildrenTruncated() {
    STACK_TRACE("ExpressionTreeDrawer::computeCurrentChildrenTruncated");
    this->currentEchildrenTruncated.setSize(0);
    if (!this->flagUseFullTree) {
      if (this->getCurrentExpression().isBuiltInType()) {
        return;
      }
    }
    for (int i = 0; i < this->getCurrentExpression().size(); i ++) {
      this->currentEchildrenTruncated.addOnTop(
        this->getCurrentExpression()[i]
      );
      if (
        i + 1 + this->indexCurrentChild > this->maximumDisplayedNodes ||
        i > this->maximumAllowedWidth
      ) {
        Expression dotsAtom;
        dotsAtom.makeAtom(*this->owner, std::string("..."));
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
    STACK_TRACE("ExpressionTreeDrawer::getDisplayString");
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
    STACK_TRACE(
      "ExpressionTreeDrawer::computeCurrentEContributionToNextLayer"
    );
    this->computeCurrentChildrenTruncated();
    this->nextLayer.addListOnTop(this->currentEchildrenTruncated);
    List<int> emptyArrows;
    for (int i = 0; i < this->currentEchildrenTruncated.size; i ++) {
      this->arrows[this->indexCurrentChild].addOnTop(
        this->displayedExpressionStrings.size
      );
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
    this->addStringTruncate(
      this->baseExpression.toString(),
      this->isLeaf(this->baseExpression)
    );
    this->computeCurrentEContributionToNextLayer();
    this->displayedExpressionStrings.setExpectedSize(
      this->maximumDisplayedNodes
    );
    this->arrows.setExpectedSize(this->maximumDisplayedNodes);
  }
  std::string toString() {
    std::stringstream out;
    out << "<br>Index in current layer: " << this->indexInCurrentLayer;
    out << "<br>Index in displayed strings: " << this->indexCurrentChild;
    out
    << "<br>Current layer: "
    << this->currentLayer.toStringCommaDelimited();
    out << "<br>Next layer: " << this->nextLayer.toStringCommaDelimited();
    out
    << "<br>Displayed strings: "
    << this->displayedExpressionStrings.toStringCommaDelimited();
    out
    << "<br>Node positions: "
    << this->nodePositions.toStringCommaDelimited();
    out << "<br>Arrows: " << this->arrows.toStringCommaDelimited();
    out << "<br>Width max layer: " << this->widthMaxLayer;
    return out.str();
  }
  void addStringTruncate(const std::string& input, bool isLeaf) {
    this->displayedStringIsLeaf.addOnTop(isLeaf);
    if (
      input.size() <= static_cast<unsigned>(this->maxNumCharsInString)
    ) {
      this->displayedExpressionStrings.addOnTop(input);
      return;
    }
    std::string truncatedInput = input;
    truncatedInput.resize(
      static_cast<unsigned>(this->maxNumCharsInString) - 3
    );
    truncatedInput += "...";
    this->displayedExpressionStrings.addOnTop(truncatedInput);
  }
  bool incrementReturnFalseIfPastLast() {
    STACK_TRACE("ExpressionTreeDrawer::incrementReturnFalseIfPastLast");
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
  Rational getStringWidthTruncated(int index) {
    return
    this->charWidth *
    MathRoutines::minimum(
      this->maxNumCharsInString,
      static_cast<signed>(
        this->displayedExpressionStrings[index].size()
      )
    );
  }
  Rational getLayerWidth(int layerIndex) {
    STACK_TRACE("ExpressionTreeDrawer::getLayerWidth");
    Rational result = 0;
    for (
      int i = this->layerFirstIndices[layerIndex]; i < this->layerFirstIndices[
        layerIndex
      ] +
      this->layerSizes[layerIndex]; i ++
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
    STACK_TRACE("ExpressionTreeDrawer::computeLayerPositions");
    Rational currentX = - this->getLayerWidth(layerIndex) / 2;
    for (
      int i = this->layerFirstIndices[layerIndex]; i < this->layerFirstIndices[
        layerIndex
      ] +
      this->layerSizes[layerIndex]; i ++
    ) {
      this->nodePositions[i].setSize(2);
      this->nodePositions[i][0] = currentX + this->getStringWidthTruncated(i) /
      2;
      this->nodePositions[i][1] = this->layerHeight * layerIndex *(- 1);
      currentX +=
      this->charWidth * static_cast<int>(
        this->displayedExpressionStrings[i].size()
      ) +
      this->padding;
    }
  }
  void drawToDrawingVariables() {
    STACK_TRACE("ExpressionTreeDrawer::drawToDrawingVariables");
    this->initialize();
    while (this->incrementReturnFalseIfPastLast()) {}
    this->plot.dimension = 2;
    this->plot.flagIncludeCoordinateSystem = false;
    this->nodePositions.setSize(this->displayedExpressionStrings.size);
    for (int i = 0; i < this->layerFirstIndices.size; i ++) {
      this->computeLayerPositions(i);
    }
    this->nodePositionsDouble.setSize(this->nodePositions.size);
    for (int i = 0; i < this->nodePositionsDouble.size; i ++) {
      this->nodePositionsDouble[i] =
      MathRoutines::getVectorDouble(this->nodePositions[i]);
    }
    Vector<double> arrowBase, arrowHead;
    for (int i = 0; i < this->displayedExpressionStrings.size; i ++) {
      for (int j = 0; j < this->arrows[i].size; j ++) {
        arrowBase = this->nodePositionsDouble[i];
        arrowHead = this->nodePositionsDouble[this->arrows[i][j]];
        arrowHead[1] += this->charHeight.getDoubleValue() / 2;
        PlotObject plotSegment;
        plotSegment.plotString = "segment";
        plotSegment.pointsDouble.addOnTop(arrowBase);
        plotSegment.pointsDouble.addOnTop(arrowHead);
        plotSegment.colorJS = "black";
        this->plot += plotSegment;
      }
      if (this->displayedExpressionStrings[i] != "") {
        PlotObject plotText;
        plotText.plotType = "label";
        plotText.pointsDouble.addOnTop(this->nodePositionsDouble[i]);
        plotText.colorJS = this->displayedStringIsLeaf[i] ? "red" : "gray";
        std::string doubledBackslashes =
        HtmlRoutines::doubleBackslashes(
          this->displayedExpressionStrings[i]
        );
        std::string backslashQuotes =
        HtmlRoutines::backslashQuotes(doubledBackslashes);
        plotText.plotString = HtmlRoutines::clearNewLines(backslashQuotes);
        plot += plotText;
      } else {
        PlotObject point;
        point.plotType = "point";
        point.colorJS = "blue";
        point.pointsDouble.addOnTop(this->nodePositionsDouble[i]);
        this->plot += point;
      }
    }
  }
};

bool CalculatorFunctionsPlot::drawExpressionGraphWithOptions(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool useFullTree
) {
  STACK_TRACE("CalculatorFunctionsPlot::drawExpressionGraphWithOptions");
  ExpressionTreeDrawer expressionDrawer;
  expressionDrawer.flagUseFullTree = useFullTree;
  expressionDrawer.owner = &calculator;
  expressionDrawer.baseExpression = input;
  expressionDrawer.drawToDrawingVariables();
  return output.assignValue(calculator, expressionDrawer.plot);
}

bool CalculatorFunctions::setRandomSeed(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::setRandomSeed");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  LargeInteger seedOriginal;
  if (!argument.isInteger(&seedOriginal)) {
    return calculator << "Failed to extract integer random seed. ";
  }
  std::stringstream out;
  calculator.objectContainer.pseudoRandom.setRandomSeedLarge(
    seedOriginal, &out
  );
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctions::andFunction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::andFunction");
  if (input.size() != 3) {
    return false;
  }
  if (input[1].isEqualToZero()) {
    return output.assignValue(calculator, 0);
  }
  if (input[2].isEqualToZero()) {
    return output.assignValue(calculator, 0);
  }
  if (input[1].isEqualToOne() && input[2].isEqualToOne()) {
    return output.assignValue(calculator, 1);
  }
  return false;
}

bool CalculatorFunctions::orFunction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::orFunction");
  if (input.size() != 3) {
    return false;
  }
  if (input[1].isEqualToOne()) {
    return output.assignValue(calculator, 1);
  }
  if (input[2].isEqualToOne()) {
    return output.assignValue(calculator, 1);
  }
  if (input[1].isEqualToZero() && input[2].isEqualToZero()) {
    return output.assignValue(calculator, 0);
  }
  return false;
}

bool CalculatorFunctions::ifStandard(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::ifStandard");
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

bool CalculatorFunctions::ifFrozen(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::ifFrozen");
  (void) calculator;
  if (input.size() != 4) {
    return false;
  }
  Expression firstArgument;
  if (
    !calculator.evaluateExpression(calculator, input[1], firstArgument)
  ) {
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
  Expression& output,
  bool turnOff
) {
  STACK_TRACE("CalculatorFunctions::turnRulesOnOff");
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
      return
      calculator
      << "Could not extract rule to turn off from "
      << input.toString()
      << ". ";
    }
  } else {
    for (int i = 1; i < input.size(); i ++) {
      if (input[i].isOfType<std::string>(&currentRule)) {
        rulesToConsider.addOnTop(currentRule);
      } else if (input[i].isOperation(&currentRule)) {
        rulesToConsider.addOnTop(currentRule);
      } else {
        return
        calculator
        << "Could not extract rule to turn off from "
        << input[i].toString()
        << ". ";
      }
    }
  }
  HashedList<std::string> rulesToSwitch;
  rulesToSwitch.reserve(rulesToConsider.size);
  for (int i = 0; i < rulesToConsider.size; i ++) {
    if (!calculator.namedRules.contains(rulesToConsider[i])) {
      return
      calculator
      << "Can't find named rule: "
      << rulesToConsider[i]
      << ". Turn-off rules command failed. ";
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
    currentRuleE.assignValue(calculator, rulesToSwitch[i]);
    output.addChildOnTop(currentRuleE);
  }
  return true;
}

bool CalculatorFunctions::approximationsDummy(
  Calculator& calculator, const Expression& input, Expression& output
) {
  (void) calculator;
  (void) input;
  (void) output;
  return false;
}

bool CalculatorFunctions::turnOnApproximations(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::turnOnApproximations");
  (void) input;
  Expression approximations;
  approximations.makeAtom(calculator, calculator.opApproximations());
  Expression onSwitch;
  onSwitch.makeOX(
    calculator, calculator.opTurnOffRules(), approximations
  );
  return CalculatorFunctions::turnOnRules(calculator, onSwitch, output);
}

bool CalculatorFunctions::turnOffApproximations(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::turnOffApproximations");
  (void) input;
  Expression approximations;
  approximations.makeAtom(calculator, calculator.opApproximations());
  Expression offSwitch;
  offSwitch.makeOX(
    calculator, calculator.opTurnOffRules(), approximations
  );
  return CalculatorFunctions::turnOffRules(calculator, offSwitch, output);
}

bool CalculatorFunctions::turnOffRules(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::turnOffRules");
  return
  CalculatorFunctions::turnRulesOnOff(calculator, input, output, true);
}

bool CalculatorFunctions::turnOnRules(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::turnOnRules");
  return
  CalculatorFunctions::turnRulesOnOff(calculator, input, output, false);
}

bool CalculatorFunctions::equalityToArithmeticExpression(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::equalityToArithmeticExpression");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  return
  CalculatorFunctions::functionEqualityToArithmeticExpression(
    calculator, argument, output
  );
}

bool CalculatorFunctions::functionEqualityToArithmeticExpression(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::functionEqualityToArithmeticExpression");
  if (!input.startsWith(calculator.opDefine(), 3)) {
    return false;
  }
  return
  output.makeXOX(
    calculator, calculator.opMinus(), input[1], input[2]
  );
}

bool CalculatorFunctions::randomInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::randomInteger");
  Matrix<Expression> matrix;
  if (
    !calculator.getMatrixExpressionsFromArguments(input, matrix, - 1, 2)
  ) {
    return
    calculator
    << "<hr>Failed to extract a Nx2 matrix giving the integer intervals";
  }
  if (matrix.numberOfRows == 0) {
    return
    calculator
    << "<hr>Failed to extract a Nx2 matrix giving the integer intervals";
  }
  List<List<int> > intervals;
  intervals.setSize(matrix.numberOfRows);
  for (int i = 0; i < matrix.numberOfRows; i ++) {
    intervals[i].setSize(matrix.numberOfColumns);
    for (int j = 0; j < matrix.numberOfColumns; j ++) {
      if (
        !matrix(i, j).isIntegerFittingInInt(&intervals[i][j])
      ) {
        return
        calculator
        << "<hr>Failed to convert "
        << matrix(i, j).toString()
        << " to an integer. ";
      }
    }
  }
  int accumulator = 0;
  for (int i = 0; i < intervals.size; i ++) {
    int currentContribution = intervals[i][1] - intervals[i][0];
    if (currentContribution < 0) {
      currentContribution *= - 1;
    }
    accumulator += currentContribution + 1;
  }
  if (accumulator == 0) {
    global.fatal
    << "This shouldn't happen: accumulator should not be zero at this point. "
    << global.fatal;
  }
  int generatedRandomInt = static_cast<signed>(
    calculator.objectContainer.pseudoRandom.
    getRandomNonNegativeLessThanMaximumSeed()
  );
  generatedRandomInt %= accumulator;
  int resultRandomValue = intervals[0][0];
  bool found = false;
  accumulator = 0;
  for (int i = 0; i < intervals.size; i ++) {
    int currentContribution = intervals[i][1] - intervals[i][0];
    int sign = 1;
    if (currentContribution < 0) {
      currentContribution *= - 1;
      sign = - 1;
    }
    currentContribution ++;
    int nextAccumulator = accumulator + currentContribution;
    if (
      accumulator <= generatedRandomInt && generatedRandomInt < nextAccumulator
    ) {
      resultRandomValue =
      intervals[i][0] + sign *(generatedRandomInt - accumulator);
      found = true;
    }
    accumulator = nextAccumulator;
  }
  if (!found) {
    return
    calculator
    << "<hr>Failed to generate a random number: "
    << "this shouldn't happen - perhaps the requested "
    << "interval(s) were too large. The generated random integer was: "
    << generatedRandomInt
    << " and the intervals were: "
    << intervals;
  }
  return output.assignValue(calculator, resultRandomValue);
}

bool CalculatorFunctions::selectAtRandom(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::selectAtRandom");
  if (
    !input.startsWith(calculator.operations.getIndex("SelectAtRandom"))
  ) {
    output = input;
    // only one item to select from: returning the item
    return true;
  }
  if (input.size() < 2) {
    return false;
  }
  if (input.size() == 2) {
    output = input[1];
    // only one item to select from: return that item
    return true;
  }
  int randomIndex = static_cast<signed>((
      calculator.objectContainer.pseudoRandom.
      getRandomNonNegativeLessThanMaximumSeed()
    )
  ) % (input.size() - 1) +
  1;
  if (randomIndex < 0 || randomIndex > input.size() - 1) {
    randomIndex = input.size() - 1;
  }
  // <-The line above should never be executed
  // if the % operator works as it should,
  // but having an extra check never hurts
  // (may be a life saver if I change the code above).
  output = input[randomIndex];
  return true;
}

bool CalculatorFunctions::convertPolynomialModulotIntegerToInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::convertPolynomialModulotIntegerToInteger");
  if (input.size() < 2) {
    return false;
  }
  const Expression& argument = input[1];
  WithContext<Polynomial<ElementZmodP> > polynomial;
  if (!argument.isOfTypeWithContext(&polynomial)) {
    return false;
  }
  if (polynomial.content.isEqualToZero()) {
    return output.assignValue(calculator, Rational::zero());
  }
  ElementZmodP zero = polynomial.content.coefficients[0].zero();
  WithContext<Polynomial<Rational> > result;
  result.context = polynomial.context;
  zero.convertPolynomialModularToPolynomialIntegral(
    polynomial.content, result.content, true
  );
  return output.assignWithContext(calculator, result);
}
