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
#include "math_extra_semisimple_Lie_algebras_implementation.h" // undefined reference to `charSSAlgMod<RationalFunctionOld>::SplitCharOverRedSubalg(std::string*, charSSAlgMod<RationalFunctionOld>&, branchingData&, GlobalVariables&)'
#include "web_api.h"
#include "math_extra_latex_routines.h"
#include "source_code_formatter.h"
#include <cmath>
#include "transport_layer_security.h"
#include "string_constants.h"
#include <vector>

template <>
bool Expression::ConvertsInternally<ElementSemisimpleLieAlgebra<AlgebraicNumber> >(
  WithContext<ElementSemisimpleLieAlgebra<AlgebraicNumber> >* whichElement
) const;

template <>
bool Expression::ConvertInternally<ElementWeylAlgebra<Rational> >(Expression& output) const;
template <>
bool Expression::ConvertInternally<Polynomial<Rational> >(Expression& output) const;
template <>
bool Expression::ConvertInternally<ElementUniversalEnveloping<RationalFunction> >(Expression& output) const;

template <class theType>
bool MathRoutines::GenerateVectorSpaceClosedWRTOperation(
  List<theType>& inputOutputElts,
  int upperDimensionBound,
  void (*theBinaryOperation)(const theType& left, const theType& right, theType& output)
) {
  MacroRegisterFunctionWithName("MathRoutines::GenerateVectorSpaceClosedWRTOperation");
  inputOutputElts[0].GaussianEliminationByRowsDeleteZeroRows(inputOutputElts);
  theType theOpResult;
  ProgressReport theReport1(1, GlobalVariables::Response::ReportType::gaussianElimination);
  ProgressReport theReport2(20, GlobalVariables::Response::ReportType::gaussianElimination);
  if (theReport1.TickAndWantReport()) {
    theReport1.Report("Extending vector space to closed with respect to binary operation. ");
  }
  List<theType> theEltsForGaussianElimination = inputOutputElts;
  for (int i = 0; i < inputOutputElts.size; i ++) {
    for (int j = i; j < inputOutputElts.size; j ++) {
      theBinaryOperation(inputOutputElts[i], inputOutputElts[j], theOpResult);
      //int oldNumElts = inputOutputElts.size;
      theEltsForGaussianElimination.addOnTop(theOpResult);
      theEltsForGaussianElimination[0].GaussianEliminationByRowsDeleteZeroRows(theEltsForGaussianElimination);
      if (theEltsForGaussianElimination.size > inputOutputElts.size) {
        inputOutputElts.addOnTop(theOpResult);
      }
      if (upperDimensionBound > 0 && inputOutputElts.size > upperDimensionBound) {
        return false;
      }
      if (theReport2.TickAndWantReport()) {
        std::stringstream reportStream;
        reportStream << "Accounted operation between elements " << i + 1
        << " and " << j + 1 << " out of " << inputOutputElts.size;
        theReport2.Report(reportStream.str());
      }
    }
  }
  inputOutputElts[0].GaussianEliminationByRowsDeleteZeroRows(inputOutputElts);
  return true;
}

bool CalculatorFunctions::innerConstructCartanSA(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerConstructCartanSA");
  SubalgebraSemisimpleLieAlgebra theSA;
  WithContext<ElementSemisimpleLieAlgebra<AlgebraicNumber> > element;
  if (input.ConvertsInternally(&element)) {
    theSA.theGenerators.addOnTop(element.content);
  } else {
    for (int i = 1; i < input.size(); i ++) {
      if (input[i].ConvertsInternally(&element)) {
        theSA.theGenerators.addOnTop(element.content);
      } else {
        return theCommands
        << "Failed to extract element of a semisimple Lie algebra from "
        << input[i].toString();
      }
    }
  }
  for (int i = 0; i < theSA.theGenerators.size; i ++) {
    if (!theSA.theGenerators[i].IsEqualToZero()) {
      if (theSA.owner != nullptr) {
        if (theSA.owner != theSA.theGenerators[i].GetOwner()) {
          return theCommands << "The input elements in " << input.toString()
          << " belong to different semisimple Lie algebras";
        }
      }
      theSA.owner = theSA.theGenerators[i].GetOwner();
    }
  }
  if (theSA.owner == nullptr) {
    return theCommands << "Failed to extract input semisimple Lie algebra "
    << "elements from the inputs of " << input.toString();
  }
  theSA.ComputeBasis();
  theSA.ComputeCartanSA();
  return output.AssignValue(theSA.toString(), theCommands);
}

bool CalculatorFunctions::innerGenerateVectorSpaceClosedWRTLieBracket(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGenerateVectorSpaceClosedWRTLieBracket");
  Vector<ElementWeylAlgebra<Rational> > theOps;
  Expression theContext;
  if (input.size() <= 1) {
    return false;
  }
  int upperBound = - 1;
  if (!input[1].IsSmallInteger(&upperBound)) {
    return theCommands << "<hr>Failed to extract upper bound "
    << "for the vector space dimension from the first argument. ";
  }
  Expression inputModded = input;
  inputModded.children.RemoveIndexShiftDown(1);

  if (!theCommands.GetVectorFromFunctionArguments(inputModded, theOps, &theContext)) {
    Vector<ElementUniversalEnveloping<RationalFunction> > theLieAlgElts;
    theContext.MakeEmptyContext(theCommands);
    if (!theCommands.GetVectorFromFunctionArguments(inputModded, theLieAlgElts, &theContext)) {
      return theCommands << "<hr>Failed to extract elements of Weyl algebra and "
      << "failed to extract elements of UE algebra from input "
      << input.toString();
    }
    FormatExpressions theFormat;
    theContext.ContextGetFormatExpressions(theFormat);
    std::stringstream out;
    out << "Starting elements: <br>";
    for (int i = 0; i < theLieAlgElts.size; i ++) {
      out << HtmlRoutines::GetMathSpanPure(theLieAlgElts[i].toString(&theFormat)) << "<br>";
    }
    bool success = MathRoutines::GenerateVectorSpaceClosedWRTLieBracket(theLieAlgElts, upperBound);
    if (!success) {
      out << "<br>Did not succeed with generating vector space, "
      << "instead got a vector space with basis " << theLieAlgElts.size << " exceeding the limit. "
      << "The basis generated before exceeding the limit was: " << theLieAlgElts.toString();
    } else {
      out << "<br>Lie bracket generates vector space of dimension " << theLieAlgElts.size << " with basis:";
      for (int i = 0; i < theLieAlgElts.size; i ++) {
        out << "<br>";
        if (theLieAlgElts.size > 50) {
          out << theLieAlgElts[i].toString(&theFormat);
        } else {
          out << HtmlRoutines::GetMathSpanPure(theLieAlgElts[i].toString(&theFormat));
        }
      }
    }
    return output.AssignValue(out.str(), theCommands);
  }
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  std::stringstream out;
  out << "Starting elements: <br>";
  for (int i = 0; i < theOps.size; i ++) {
    out << HtmlRoutines::GetMathSpanPure(theOps[i].toString(&theFormat)) << "<br>";
  }
  bool success = MathRoutines::GenerateVectorSpaceClosedWRTLieBracket(theOps, upperBound);
  if (!success) {
    out << "<br>Did not succeed with generating vector space, "
    << "instead got a vector space with basis "
    << theOps.size << " exceeding the limit. "
    << "The basis generated before exceeding the limit was: "
    << theOps.toString();
  } else {
    out << "<br>Lie bracket generates vector space of dimension "
    << theOps.size << " with basis:";
    for (int i = 0; i < theOps.size; i ++) {
      out << "<br>";
      if (theOps.size > 50) {
        out << theOps[i].toString(&theFormat);
      } else {
        out << HtmlRoutines::GetMathSpanPure(theOps[i].toString(&theFormat));
      }
    }
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerTestLoadPEMCertificates(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestLoadPEMCertificates");
  if (input.size() != 2) {
    return false;
  }
  std::string binaryString;
  if (!input[1].IsOfType(&binaryString)) {
    return false;
  }
  X509Certificate theCertificate;
  std::stringstream errorStream, resultStream;
  // May not be initialized if unit testing. Safe after the first run.
  ASNObject::NamesToObjectIdsNonThreadSafe();
  bool success = theCertificate.LoadFromASNEncoded(binaryString, &errorStream);
  if (!success) {
    resultStream << "Failed to load asn encoded certificate.<br>";
    resultStream << errorStream.str();
  } else {
    resultStream << "<br>Loaded certificate successfully.<br>" << theCertificate.toString();
  }
  return output.AssignValue(resultStream.str(), theCommands);
}

bool CalculatorFunctions::innerTestTLSDecodeSSLRecord(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestTLSDecodeSSLRecord");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].IsOfType(&inputString)) {
    return false;
  }
  TransportLayerSecurityServer testServer;
  std::stringstream commentsOnFailure;
  if (!Crypto::ConvertHexToListUnsignedChar(
    inputString, testServer.lastReaD.incomingBytes, &commentsOnFailure
  )) {
    return theCommands << commentsOnFailure.str();
  }
  testServer.initialize();
  bool success = testServer.DecodeSSLRecord(&commentsOnFailure);
  std::stringstream out;
  if (!success) {
    out << "<b style = 'color:red'>Failed to decode the record.</b> "
    << commentsOnFailure.str();
  }
  out << testServer.lastReaD.ToHtml(1);

  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerTestTLSMessageSequence(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestTLSMessageSequence");
  Vector<std::string> inputMessages;
  if (!theCommands.GetVectorFromFunctionArguments(input, inputMessages)) {
    return theCommands << "Failed to extract input vector of strings. ";
  }
  if (inputMessages.size < 3) {
    return theCommands << "At least 3 inputs required: "
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
    return output.AssignValue(commentsOnFailure.str(), theCommands);
  }
  spoofServer.spoofer.flagDoSpoof = true;
  for (int i = 2; i < inputMessages.size; i ++) {
    spoofServer.spoofer.incomingMessages.addOnTop(SSLRecord());
    spoofServer.spoofer.incomingMessages.LastObject()->incomingBytes = inputMessages[i];
  }
  spoofServer.spoofer.currentInputMessageIndex = 0;
  std::stringstream errorStream;
  spoofServer.HandShakeIamServer(- 1, &errorStream);

  std::stringstream out;
  std::stringstream spanId;
  spanId << "spanServerSpoofer_" << "_" << global.GetElapsedMilliseconds();
  out << "<div id = '" << spanId.str() << "'></div>";
  out << "<script>"
  << "window.calculator.crypto.displayTransportLayerSecurity('"
  << spanId.str() << "', " << spoofServer.ToJSON() << ");"
  << "</script>";
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerTestLoadPEMPrivateKey(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestLoadPEMPrivateKey");
  if (input.size() != 2) {
    return  false;
  }
  std::string privateKeyString;
  if (!input[1].IsOfType(&privateKeyString)) {
    return false;
  }
  List<unsigned char> privateKeyBytes;
  privateKeyBytes = privateKeyString;
  std::stringstream errorStream, resultStream;
  PrivateKeyRSA thePrivateKey;
  bool success = thePrivateKey.LoadFromASNEncoded(privateKeyBytes, &errorStream);
  if (!thePrivateKey.BasicChecks(&resultStream)) {
    resultStream << "<b style = 'color:red'>Private key failed basic checks. </b>";
  }
  if (!success) {
    resultStream << "<br>Failed to load asn encoded certificate.<br>";
    resultStream << errorStream.str();
  } else {
    resultStream << "<br>Loaded private key.<br>" << thePrivateKey.toString();
  }
  return output.AssignValue(resultStream.str(), theCommands);
}

bool CalculatorFunctions::innerLoadKnownCertificates(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerLoadKnownCertificates");
  (void) input;
  std::stringstream out;
  Crypto::LoadKnownCertificates(&out, &out);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerSha1OfString(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSha1OfString");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctions::functionHashString(
    theCommands, input[1], output, "SHA1", false
  );
}

bool CalculatorFunctions::innerSha256OfString(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSha256OfString");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctions::functionHashString(theCommands, input[1], output, "SHA256", false);
}

bool CalculatorFunctions::innerSha512(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSha512String");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctions::functionHashString(theCommands, input[1], output, "SHA512", false);
}

bool CalculatorFunctions::innerSha256OfStringVerbose(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSha256OfString");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctions::functionHashString(theCommands, input[1], output, "SHA256", true);
}

bool CalculatorFunctions::innerSha224OfString(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSha224OfString");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctions::functionHashString(theCommands, input[1], output, "SHA224", false);
}

bool CalculatorFunctions::innerRIPEMD160OfString(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerRIPEMD160OfString");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctions::functionHashString(theCommands, input[1], output, "RIPEMD160", false);
}

bool CalculatorFunctions::innerSha3_256OfString(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSha3_256OfString");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctions::functionHashString(theCommands, input[1], output, "SHA3_256", false);
}

bool CalculatorFunctions::innerKeccak256OfString(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerKeccak256OfString");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctions::functionHashString(theCommands, input[1], output, "KECCAK256", false);
}

bool CalculatorFunctions::functionHashString(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  const std::string& hashId,
  bool verbose
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::functionHashString");
  std::string inputString;
  if (!input.IsOfType(&inputString)) {
    return false;
  }
  List<unsigned char> theBitStream;
  theBitStream = inputString;
  std::stringstream out;
  if (verbose) {
    out << "<br>Input: " << inputString;
    out << "<br>Base64 conversion: " << Crypto::ConvertListUnsignedCharsToBase64(theBitStream, false);
    out << "<br>Base64url conversion: " << Crypto::ConvertListUnsignedCharsToBase64(theBitStream, true);
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
    Crypto::ConvertUint32ToUcharBigendian(theSha1Uint, hashUChar);
  } else if (hashId == "SHA3_256") {
    Crypto::computeSha3_256(inputString, hashUChar);
  } else if (hashId == "KECCAK256") {
    Crypto::computeKeccak3_256(inputString, hashUChar);
  } else if (hashId == "SHA512") {
    Crypto::computeSha512(inputString, hashUChar);
  } else if (hashId == "RIPEMD160") {
    if (externalCrypto.computeRIPEMD160 == nullptr) {
      return theCommands
      << "You are running a non-vanilla version of the calculator "
      << "that is missing ripemd160 support. "
      << "Check out the vanilla version of the calculator at github.com.";
    }
    externalCrypto.computeRIPEMD160(inputString, hashUChar);
  }
  if (verbose) {
    std::string theSha1base64string, theSha1base64URLstring;
    theSha1base64string = Crypto::ConvertListUnsignedCharsToBase64(hashUChar, true);
    theSha1base64URLstring = Crypto::ConvertListUnsignedCharsToBase64(hashUChar, false);
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
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerConvertBase64ToString(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerConvertBase64ToString");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString, result;
  if (!input[1].IsOfType(&inputString)) {
    inputString = input[1].toString();
  }
  if (!Crypto::ConvertBase64ToString(inputString, result, &theCommands.Comments)) {
    return false;
  }
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctions::innerNISTEllipticCurveOrder(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerNISTEllipticCurveGenerator");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].IsOfType(&inputString)) {
    return theCommands
    << "Function NISTEllipticCurveGenerator takes as input a string with an EC curve name. "
    << "Available curve names: secp256k1";
  }
  LargeIntegerUnsigned result;
  if (!EllipticCurveWeierstrassNormalForm::GetOrderNISTCurve(
    inputString, result, &theCommands.Comments
  )) {
    return false;
  }
  return output.AssignValue(result, theCommands);
}

template < >
bool ElementEllipticCurve<ElementZmodP>::MakeGeneratorNISTCurve(
  const std::string& input, std::stringstream* commentsOnFailure
);

bool CalculatorFunctions::innerNISTEllipticCurveGenerator(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerNISTEllipticCurveGenerator");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].IsOfType(&inputString)) {
    return theCommands
    << "Function NISTEllipticCurveGenerator takes as input a string with an EC curve name. "
    << "Available curve names: secp256k1";
  }
  ElementEllipticCurve<ElementZmodP> generator;
  if (!generator.MakeGeneratorNISTCurve(inputString, &theCommands.Comments)) {
    return false;
  }
  Expression theContext;
  List<std::string> theVars;
  theVars.addOnTop("x");
  theVars.addOnTop("y");
  theContext.ContextMakeContextWithPolyVars(theCommands, theVars);
  return output.AssignValueWithContext(generator, theContext, theCommands);
}

bool CalculatorFunctions::innerStringDifference(
  Calculator& theCommands,
  const Expression& input,
  Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerStringDifference");
  if (input.size() != 3) {
    return false;
  }
  std::string left, right;
  if (!input[1].IsOfType(&left) || !input[2].IsOfType(&right)) {
    return false;
  }
  return output.AssignValue(
    StringRoutines::Differ::DifferenceHTMLStatic(
      left, right, "left", "right"
    ),
    theCommands
  );
}

bool CalculatorFunctions::innerSliceString(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSliceString");
  if (input.size() < 3) {
    return false;
  }
  std::string inputString;
  if (!input[1].IsOfType<std::string>(&inputString)) {
    return false;
  }
  int leftIndex = 0;
  if (!input[2].IsSmallInteger(&leftIndex)) {
    return theCommands << "Failed to convert slice input " << input[2] << " to small integer. ";
  }
  if (leftIndex < 0) {
    return theCommands << "Slice input " << input[2] << " appears to be negative. ";
  }
  if (leftIndex >= static_cast<signed>(inputString.size())) {
    return output.AssignValue(std::string(""), theCommands);
  }
  if (input.size() <= 3) {
    return output.AssignValue(inputString.substr(static_cast<unsigned>(leftIndex)), theCommands);
  }
  int rightIndex = 0;
  if (!input[3].IsSmallInteger(&rightIndex)) {
    return theCommands << "Failed to convert slice input " << input[3] << " to small integer. ";
  }
  if (rightIndex < 0) {
    rightIndex += static_cast<signed>(inputString.size());
  }
  int size = rightIndex - leftIndex ;
  if (size <= 0) {
    return output.AssignValue(std::string(""), theCommands);
  }
  return output.AssignValue(
    inputString.substr(static_cast<unsigned>(leftIndex), static_cast<unsigned>(size)),
    theCommands
  );
}

bool CalculatorFunctions::innerConvertIntegerUnsignedToBase58(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerConvertIntegerUnsignedToBase58");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger theInt;
  if (!input[1].IsInteger(&theInt)) {
    return false;
  }
  if (theInt < 0) {
    return theCommands << "Conversion from negative integer to base58 not allowed. ";
  }
  std::string result;
  Crypto::ConvertLargeIntUnsignedToBase58SignificantDigitsFIRST(theInt.value, result, 0);
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctions::innerAppendDoubleSha256Check(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerAppendSha256Check");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].IsOfType(&inputString)) {
    return false;
  }
  std::string outputString;
  Crypto::AppendDoubleSha256Check(inputString, outputString);
  return output.AssignValue(outputString, theCommands);
}

bool CalculatorFunctionsCrypto::innerAES_CBC_256_Decrypt(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::innerAES_CBC_256_Decrypt");
  if (input.size() != 3) {
    return theCommands << "AES decrypt function expects two arguments: key and plain text. ";
  }
  std::string text, key;
  if (!input[1].IsOfType<std::string>(&key)) {
    return false;
  }
  if (!input[2].IsOfType<std::string>(&text)) {
    return false;
  }
  std::string cipherText;
  std::stringstream comments;
  bool result = Crypto::externalCrypto().decryptAES_CBC_256_string(
    key, text, cipherText, &comments
  );
  if (!result) {
    return theCommands << comments.str();
  }
  return output.AssignValue(cipherText, theCommands);
}

bool CalculatorFunctionsCrypto::innerAES_CBC_256_Encrypt(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsCrypto::innerAES_CBC_256_Encrypt");
  if (input.size() != 3) {
    return theCommands << "AES function expects two arguments: key and plain text. ";
  }
  std::string text, key;
  if (!input[1].IsOfType<std::string>(&key)) {
    return false;
  }
  if (!input[2].IsOfType<std::string>(&text)) {
    return false;
  }
  std::string cipherText;
  std::stringstream comments;
  bool result = Crypto::externalCrypto().encryptAES_CBC_256_string(
    key, text, cipherText, &comments
  );
  if (!result) {
    return theCommands << comments.str();
  }
  return output.AssignValue(cipherText, theCommands);
}

bool CalculatorFunctions::innerConvertBase58ToHex(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerConvertBase58ToHex");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].IsOfType(&inputString)) {
    return false;
  }
  std::string outputString;
  if (!Crypto::ConvertBase58ToHexSignificantDigitsFirst(
    inputString, outputString, &theCommands.Comments
  )) {
    return theCommands << "Failed to convert " << inputString << " to hex. ";
  }
  return output.AssignValue(outputString, theCommands);
}

bool CalculatorFunctions::innerConvertHexToBase58(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerConvertHexToBase58");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].IsOfType(&inputString)) {
    return false;
  }
  LargeIntegerUnsigned outputInteger;
  std::string outputString;
  int numLeadingZeroBytes = 0;
  if (!Crypto::ConvertHexToInteger(inputString, outputInteger, numLeadingZeroBytes)) {
    return theCommands << "Failed to convert " << inputString << " from hex to integer. ";
  }
  Crypto::ConvertLargeIntUnsignedToBase58SignificantDigitsFIRST(outputInteger, outputString, numLeadingZeroBytes);
  return output.AssignValue(outputString, theCommands);
}

bool CalculatorFunctions::innerConvertStringToHex(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerConvertStringToHex");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].IsOfType(&inputString)) {
    return false;
  }
  return output.AssignValue(Crypto::ConvertStringToHex(inputString, 0, false), theCommands);
}

bool CalculatorFunctions::innerCharToBase64(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCharToBase64");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].IsOfType(&inputString)) {
    return false;
  }
  List<unsigned char> theBitStream;
  theBitStream = inputString;
  return output.AssignValue(Crypto::ConvertListUnsignedCharsToBase64(theBitStream, false), theCommands);
}

bool CalculatorFunctions::innerBase64ToCharToBase64Test(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerBase64ToCharToBase64Test");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].IsOfType(&inputString)) {
    return false;
  }
  List<unsigned char> theBitStream;
  if (!Crypto::ConvertBase64ToBitStream(inputString, theBitStream, &theCommands.Comments)) {
    return false;
  }
  std::stringstream out;
  std::string theConvertedBack = Crypto::ConvertListUnsignedCharsToBase64(theBitStream, false);
  out << "Original string: " << inputString
  << "<br>Converted to bitstream and back: " << theConvertedBack;
  if (theConvertedBack != inputString) {
    out << "<br><b>The input is not the same as the output!</b>";
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerURLKeyValuePairsToNormalRecursive(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerURLKeyValuePairsToNormalRecursive");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string theString;
  if (!argument.IsOfType<std::string>(&theString)) {
    return false;
  }
  return output.AssignValue(HtmlRoutines::URLKeyValuePairsToNormalRecursiveHtml(theString), theCommands);
}

bool CalculatorFunctions::innerConvertElementZmodPToInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerConvertElementZmodPToInteger");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  ElementZmodP theElement;
  if (!argument.IsOfType<ElementZmodP>(&theElement)) {
    return false;
  }
  return output.AssignValue(theElement.theValue, theCommands);
}

bool CalculatorFunctions::innerUrlStringToNormalString(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerUrlStringToNormalString");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string theString;
  if (!argument.IsOfType<std::string>(&theString)) {
    return false;
  }
  return output.AssignValue(HtmlRoutines::ConvertURLStringToNormal(theString, false), theCommands);
}

bool CalculatorFunctions::innerStringToURL(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerStringToURL");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string theString;
  if (!argument.IsOfType<std::string>(&theString)) {
    return false;
  }
  return output.AssignValue(HtmlRoutines::ConvertStringToURLString(theString, false), theCommands);
}

bool CalculatorFunctions::innerStringToAtom(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerStringToAtom");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  std::string theString;
  if (!argument.IsOfType(&theString)) {
    return false;
  }
  return output.MakeAtom(theString, theCommands);
}

bool CalculatorFunctions::innerExpressionToString(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerExpressionToString");
  if (input.size() != 2) {
    return false;
  }
  return output.AssignValue(input[1].toString(), theCommands);
}

bool CalculatorFunctions::innerQuoteToString(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerQuoteToString");
  std::string operation;
  if (input.size() != 2) {
    theCommands << "<b>Warning: this shouldn't happen: "
    << "quote operation is applied to an expression "
    << "with wrong number of arguments. "
    << "This may be a bug with the function "
    << "Calculator::ParseFillDictionary.</b>";
    return output.AssignValue(input.toString(), theCommands);
  }
  if (input[1].IsOperation(&operation)) {
    return output.AssignValue(operation, theCommands);
  }
  theCommands
  << "<b>Warning: this shouldn't happen: quote operation "
  << "is applied to the non-atomic expression: "
  << input.toString() << ". "
  << "This may be a bug with the function Calculator::ParseFillDictionary. </b>";
  return output.AssignValue(input.toString(), theCommands);
}

bool CalculatorFunctions::innerFourierTransformEWA(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFourierTransformEWA");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (!argument.IsOfType<ElementWeylAlgebra<Rational> >()) {
    return false;
  }
  ElementWeylAlgebra<Rational> theElt;
  argument.GetValue<ElementWeylAlgebra<Rational> >().FourierTransform(theElt);
  return output.AssignValueWithContext(theElt, argument.GetContext(), theCommands);
}

bool CalculatorFunctions::innerCasimirWRTlevi(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCasimir");
  RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(3)) {
    return false;
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!theCommands.Convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, algebra
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra* theSSalg = algebra.content;
  Vector<Rational> leviSelection;
  if (!theCommands.GetVectoR(input[2], leviSelection, nullptr, theSSalg->GetRank())) {
    return theCommands << "<hr>Failed to extract parabolic selection. ";
  }
  Selection theParSel;
  theParSel = leviSelection;
  theParSel.InvertSelection();
  ElementUniversalEnveloping<RationalFunction> theCasimir;
  theCasimir.MakeCasimirWRTLeviParabolic(*theSSalg, theParSel);
  Expression contextE;
  contextE.MakeContextSSLieAlg(theCommands, *theSSalg);
  return output.AssignValueWithContext(theCasimir, contextE, theCommands);
}

bool CalculatorFunctions::innerLogBase(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerLogBase");
  if (!input.StartsWith(theCommands.opLogBase(), 3)) {
    return false;
  }
  Expression numE, denE;
  numE.MakeOX(theCommands, theCommands.opLog(), input[2]);
  denE.MakeOX(theCommands, theCommands.opLog(), input[1]);
  return output.MakeXOX(theCommands, theCommands.opDivide(), numE, denE);
}

bool CalculatorFunctions::innerLog(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerLog");
  if (input.size() != 2) {
    return false;
  }
  if (input[1].IsEqualToZero()) {
    return output.MakeError("Attempting to compute logarithm of zero.", theCommands);
  }
  if (input[1].IsEqualToOne()) {
    return output.AssignValue(0, theCommands);
  }
  if (theCommands.flagNoApproximationS) {
    return false;
  }
  double theArgument;
  if (!input[1].EvaluatesToDouble(&theArgument)) {
    if (input[1].IsOperationGiven(theCommands.opE())) {
      return output.AssignValue(Rational(1), theCommands);
    }
    return false;
  }
  if (theArgument > 0) {
    if (input[1].IsOperationGiven(theCommands.opE())) {
      return output.AssignValue(Rational(1), theCommands);
    }
    return output.AssignValue(FloatingPoint::Log(theArgument), theCommands);
  }
  theArgument *= - 1;
  Expression iE, ipiE, piE, lnPart;
  iE.MakeSqrt(theCommands, Rational(- 1), 2);
  piE.MakeAtom(theCommands.opPi(), theCommands);
  ipiE.MakeXOX(theCommands, theCommands.opTimes(), piE, iE);
  lnPart.AssignValue(FloatingPoint::Log(theArgument), theCommands);
  return output.MakeXOX(theCommands, theCommands.opPlus(), lnPart, ipiE);
}

bool CalculatorFunctions::innerFactorial(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFactorial");
  if (input.size() != 2) {
    return false;
  }
  int inputInt = - 1;
  if (!input[1].IsIntegerFittingInInt(&inputInt)) {
    return false;
  }
  if (inputInt < 0) {
    return false;
  }
  if (inputInt > 30000) {
    return theCommands << "Computing large factorials is disabled "
    << "(can be changed in the source code by modifying CalculatorFunctions::innerFactorial).";
  }
  Rational result;
  return output.AssignValue(result.Factorial(inputInt), theCommands);
}

bool CalculatorFunctions::innerArctan(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerArctan");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (argument.IsEqualToOne()) {
    output.MakeAtom(theCommands.opPi(), theCommands);
    output /= theCommands.EFour();
    return true;
  }
  if (argument.IsEqualToMOne()) {
    output.MakeAtom(theCommands.opPi(), theCommands);
    output /= theCommands.EFour();
    output *= theCommands.EMOne();
    return true;
  }
  if (theCommands.flagNoApproximationS) {
    return false;
  }
  double theArgument;
  if (!argument.EvaluatesToDouble(&theArgument)) {
    return false;
  }
  return output.AssignValue(FloatingPoint::Arctan(theArgument), theCommands);
}

bool CalculatorFunctions::innerArccos(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerArccos");
  if (theCommands.flagNoApproximationS) {
    return false;
  }
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  double doubleArgument;
  if (!argument.EvaluatesToDouble(&doubleArgument)) {
    return false;
  }
  return output.AssignValue(FloatingPoint::Arccos(doubleArgument), theCommands);
}

bool CalculatorFunctions::innerArcsin(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerArcsin");
  if (input.size() != 2) {
    return false;
  }
  if (theCommands.flagNoApproximationS) {
    return false;
  }
  double theArgument;
  if (!input[1].EvaluatesToDouble(&theArgument)) {
    return false;
  }
  return output.AssignValue(FloatingPoint::Arcsin(theArgument), theCommands);
}

bool CalculatorFunctions::innerAbs(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerAbs");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Rational theRat;
  if (argument.IsRational(&theRat)) {
    if (theRat < 0) {
      return output.AssignValue(- theRat, theCommands);
    }
    return output.AssignValue(theRat, theCommands);
  }
  double theDouble = 0;
  if (argument.EvaluatesToDouble(&theDouble)) {
    if (theDouble < 0) {
      Expression moneE;
      moneE.AssignValue(- 1, theCommands);
      output = argument;
      output *= moneE;
      return true;
    }
    output = argument;
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerSin(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSin");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (argument.IsOperationGiven(theCommands.opPi())) {
    return output.AssignValue(0, theCommands);
  }
  if (argument.IsEqualToZero()) {
    return output.AssignValue(0, theCommands);
  }
  Rational piProportion;
  if (argument.StartsWith(theCommands.opTimes(), 3)) {
    if (argument[2].IsOperationGiven(theCommands.opPi())) {
      if (argument[1].IsOfType<Rational>(&piProportion)) {
        AlgebraicNumber algOutput;
        Rational ratOutput;
        if (algOutput.AssignSinRationalTimesPi(piProportion, theCommands.theObjectContainer.theAlgebraicClosure)) {
          if (algOutput.IsRational(&ratOutput)) {
            return output.AssignValue(ratOutput, theCommands);
          }
          return output.AssignValue(algOutput, theCommands);
        }
      }
    }
  }
  if (theCommands.flagNoApproximationS) {
    return false;
  }
  double theArgument = 0;
  if (!argument.EvaluatesToDouble(&theArgument)) {
    return false;
  }
  return output.AssignValue(FloatingPoint::Sin(theArgument), theCommands);
}

bool CalculatorFunctions::innerCos(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCos");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (argument.IsOperationGiven(theCommands.opPi())) {
    return output.AssignValue(- 1, theCommands);
  }
  if (argument.IsEqualToZero()) {
    return output.AssignValue(1, theCommands);
  }
  Rational piProportion;
  if (argument.StartsWith(theCommands.opTimes(), 3)) {
    if (argument[2].IsOperationGiven(theCommands.opPi())) {
      if (argument[1].IsOfType<Rational>(&piProportion)) {
        AlgebraicNumber algOutput;
        Rational ratOutput;
        if (algOutput.AssignCosRationalTimesPi(piProportion, theCommands.theObjectContainer.theAlgebraicClosure)) {
          if (algOutput.IsRational(&ratOutput)) {
            return output.AssignValue(ratOutput, theCommands);
          }
          return output.AssignValue(algOutput, theCommands);
        }
      }
    }
  }
  if (theCommands.flagNoApproximationS) {
    return false;
  }
  double theArgument = 0;
  if (!argument.EvaluatesToDouble(&theArgument)) {
    return false;
  }
  return output.AssignValue(FloatingPoint::Cos(theArgument), theCommands );
}

bool CalculatorFunctions::innerTan(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTan");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression num, den;
  num.MakeOX(theCommands, theCommands.opSin(), argument);
  den.MakeOX(theCommands, theCommands.opCos(), argument);
  return output.MakeXOX(theCommands, theCommands.opDivide(), num, den);
}

bool CalculatorFunctions::innerCot(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTan");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression num, den;
  num.MakeOX(theCommands, theCommands.opCos(), argument);
  den.MakeOX(theCommands, theCommands.opSin(), argument);
  return output.MakeXOX(theCommands, theCommands.opDivide(), num, den);
}

bool CalculatorFunctions::innerSec(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSec");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression num, den;
  num.AssignValue(1, theCommands);
  den.MakeOX(theCommands, theCommands.opCos(), argument);
  return output.MakeXOX(theCommands, theCommands.opDivide(), num, den);
}

bool CalculatorFunctions::innerCsc(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCsc");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression num, den;
  num.AssignValue(1, theCommands);
  den.MakeOX(theCommands, theCommands.opSin(), argument);
  return output.MakeXOX(theCommands, theCommands.opDivide(), num, den);
}

bool Calculator::GetSumProductsExpressions(const Expression& inputSum, List<List<Expression> >& outputSumMultiplicands) {
  MacroRegisterFunctionWithName("Calculator::GetSumProductsExpressions");
  List<Expression> theSummands, currentMultiplicands;
  outputSumMultiplicands.setSize(0);
  if (!this->CollectOpands(inputSum, this->opPlus(), theSummands)) {
    return false;
  }
  for (int i = 0; i < theSummands.size; i ++) {
    this->CollectOpands(theSummands[i], this->opTimes(), currentMultiplicands);
    outputSumMultiplicands.addOnTop(currentMultiplicands);
  }
  return true;
}

bool CalculatorFunctions::innerCoefficientOf(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCoefficientOf");
  if (input.size() != 3) {
    return false;
  }
  List<List<Expression> > theSummands;
  List<Expression> currentListMultiplicands, survivingSummands;
  Expression currentMultiplicand;
  if (input[2].StartsWith(theCommands.opDivide())) {
    Expression coefficientNumerator = input;
    coefficientNumerator.SetChilD(2, input[2][1]);
    if (!CalculatorFunctions::innerCoefficientOf(theCommands, coefficientNumerator, output)) {
      return false;
    }
    return output.MakeXOX(theCommands, theCommands.opDivide(), output, input[2][2]);
  }
  if (!theCommands.GetSumProductsExpressions(input[2], theSummands)) {
    return theCommands << "Failed to extract product of expressions from "
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
    currentMultiplicand.MakeProducT(theCommands, currentListMultiplicands);
    survivingSummands.addOnTop(currentMultiplicand);
  }
  return output.makeSum(theCommands, survivingSummands);
}

bool CalculatorFunctions::innerChildExpression(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerChildExpression");
  (void) theCommands;
  if (input.size() != 3) {
    return false;
  }
  int theIndex = 0;
  if (!input[2].IsSmallInteger(&theIndex)) {
    return false;
  }
  theIndex --;
  if (theIndex < 0 || theIndex >= input[1].size()) {
    return false;
  }
  output = input[1][theIndex];
  return true;
}

bool CalculatorFunctions::innerDereferenceInterval(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDereferenceInterval");
  (void) theCommands;
  if (!input.StartsWith(theCommands.opUnderscore(), 3)) {
    return false;
  }
  if (!input[1].IsIntervalRealLine()) {
    return false;
  }
  if (input[2].IsEqualToOne()) {
    output = input[1][1];
    return true;
  }
  if (input[2].IsEqualToTwo()) {
    output = input[1][2];
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerDereferenceSequenceOrMatrix(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDereferenceSequenceOrMatrix");
  (void) theCommands;
  if (!input.StartsWith(theCommands.opUnderscore(), 3)) {
    return false;
  }
  if (
    !input[1].StartsWith(theCommands.opSequence()) &&
    !input[1].IsMatrix()
  ) {
    return false;
  }
  int theIndex;
  if (!input[2].IsSmallInteger(&theIndex)) {
    return false;
  }
  if (theIndex > 0 && theIndex < input[1].size()) {
    output = input[1][theIndex];
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerDereferenceSequenceStatements(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDereferenceSequenceStatements");
  (void) theCommands;
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].StartsWith(theCommands.opEndStatement())) {
    return false;
  }
  int theIndex;
  if (!input[2].IsSmallInteger(&theIndex)) {
    return false;
  }
  if (theIndex > 0 && theIndex < input[1].size()) {
    output = input[1][theIndex];
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerSolveSerreLikeSystem(
  Calculator& theCommands, const Expression& input, Expression& output, bool useUpperLimit, bool startWithAlgebraicClosure
) {
  MacroRegisterFunctionWithName("Calculator::innerSolveSerreLikeSystem");
  Vector<Polynomial<Rational> > thePolysRational;
  Expression theContext(theCommands);
  bool useArguments =
  input.StartsWith(theCommands.GetOperations().GetIndexIMustContainTheObject("FindOneSolutionSerreLikePolynomialSystem")) ||
  input.StartsWith(theCommands.GetOperations().GetIndexIMustContainTheObject("FindOneSolutionSerreLikePolynomialSystemAlgebraic")) ||
  input.StartsWith(theCommands.GetOperations().GetIndexIMustContainTheObject("FindOneSolutionSerreLikePolynomialSystemUpperLimit")) ||
  input.StartsWith(theCommands.GetOperations().GetIndexIMustContainTheObject("FindOneSolutionSerreLikePolynomialSystemAlgebraicUpperLimit"));

  if (useArguments) {
    if (!theCommands.GetVectorFromFunctionArguments(
      input,
      thePolysRational,
      &theContext,
      0,
      CalculatorConversions::functionPolynomial<Rational>
    )) {
      return output.MakeError("Failed to extract list of polynomials. ", theCommands);
    }
  } else {
    if (!theCommands.GetVectoR(
      input,
      thePolysRational,
      &theContext,
      0,
      CalculatorConversions::functionPolynomial<Rational>
    )) {
      return output.MakeError("Failed to extract list of polynomials. ", theCommands);
    }
  }
  GroebnerBasisComputation<AlgebraicNumber> theComputation;
  theContext.ContextGetFormatExpressions(theComputation.theFormat);
  int upperLimit = 2001;
  if (useUpperLimit) {
    Rational upperLimitRat;
    if (!thePolysRational[0].IsConstant(&upperLimitRat)) {
      return theCommands << "Failed to extract a constant from the first argument "
      << thePolysRational[0].toString(&theComputation.theFormat) << ". ";
    }
    if (!upperLimitRat.IsIntegerFittingInInt(&upperLimit)) {
      return theCommands << "Failed to extract a small integer from the first argument "
      << upperLimitRat.toString(&theComputation.theFormat) << ". ";
    }
    thePolysRational.PopIndexShiftDown(0);
  }
  Vector<Polynomial<AlgebraicNumber> > thePolysAlgebraic;
  thePolysAlgebraic = thePolysRational;
  //int numVars = theContext.GetNumContextVariables();
  theComputation.MaxNumGBComputations = upperLimit;
  theComputation.MaxNumSerreSystemComputationsPreferred = upperLimit;
  theComputation.thePolynomialOrder.theMonOrder.setComparison(MonomialP::greaterThan_totalDegree_rightSmallerWins);
  theComputation.theAlgebraicClosurE = &theCommands.theObjectContainer.theAlgebraicClosure;
  theComputation.flagTryDirectlySolutionOverAlgebraicClosure = startWithAlgebraicClosure;
  global.theDefaultFormat.GetElement() = theComputation.theFormat;
  theComputation.flagUseTheMonomialBranchingOptimization = true;
  theComputation.SolveSerreLikeSystem(thePolysAlgebraic);
  std::stringstream out;
  out << "<br>The context vars:<br>" << theContext.toString();
  out << "<br>The polynomials: " << thePolysAlgebraic.toString(&theComputation.theFormat);
  out << "<br>Total number of polynomial computations: " << theComputation.NumberSerreSystemComputations;
  if (theComputation.flagSystemProvenToHaveNoSolution) {
    out << "<br>The system does not have a solution. ";
  } else if (theComputation.flagSystemProvenToHaveSolution) {
    out << "<br>System proven to have solution. ";
  }
  if (!theComputation.flagSystemProvenToHaveNoSolution) {
    if (theComputation.flagSystemSolvedOverBaseField) {
      out << "<br>One solution follows. " << theComputation.ToStringSerreLikeSolution();
    } else {
      out << "However, I was unable to find such a solution: my heuristics are not good enough.";
    }
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerConvertAlgebraicNumberToMatrix(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerConvertAlgebraicNumberToMatrix");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  AlgebraicNumber theNumber;
  if (!argument.IsOfType(&theNumber)) {
    return theCommands << "Failed to convert "
    << argument.toString() << " to algebraic number. ";
  }
  int dimension = theNumber.owner->getDimensionOverTheRationals();
  MatrixTensor<Rational> numberMatrixTensor;
  theNumber.owner->GetMultiplicationBy(theNumber, numberMatrixTensor);
  Matrix<Rational> result;
  numberMatrixTensor.GetMatrix(result, dimension);
  return output.AssignMatrix(result, theCommands);
}

bool CalculatorFunctions::innerPrintAlgebraicClosureStatus(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  (void) input;
  FormatExpressions theFormat;
  theFormat.flagUseHTML = false;
  theFormat.flagUseLatex = true;
  return output.AssignValue(
    theCommands.theObjectContainer.theAlgebraicClosure.ToStringFull(&theFormat),
    theCommands
  );
}

bool CalculatorFunctions::innerGetAlgebraicNumberFromMinPoly(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGetAlgebraicNumberFromMinPoly");
  Expression polyE;
  if (!CalculatorConversions::innerPolynomial<AlgebraicNumber>(
    theCommands, input, polyE
  )) {
    return theCommands << "<hr>Failed to convert "
    << input.toString() << " to polynomial. ";
  }
  Polynomial<AlgebraicNumber> thePoly;
  if (!polyE.IsOfType<Polynomial<AlgebraicNumber> >(&thePoly)) {
    return theCommands << "<hr>Failed to convert " << input.toString()
    << " to polynomial, instead got " << polyE.toString();
  }
  if (polyE.GetNumContextVariables() != 1) {
    return theCommands << "<hr>After conversion, I got the polynomial " << polyE.toString()
    << ", which is not in one variable.";
  }
  AlgebraicNumber theAN;
  std::stringstream commentsOnFailure;
  if (!theAN.ConstructFromMinPoly(
    thePoly, theCommands.theObjectContainer.theAlgebraicClosure, &commentsOnFailure
  )) {
    return theCommands << "Failed to construct minimal polynomial. " << commentsOnFailure.str();
  }
  return output.AssignValue(theAN, theCommands);
}

bool CalculatorFunctions::innerCompositeApowerBevaluatedAtC(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompositeApowerBevaluatedAtC");
  if (!input.IsListNElements()) {
    return false;
  }
  const Expression& firstE = input[0];
  if (!firstE.StartsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  if (firstE[1].IsSequenceNElementS()) {
    return false;
  }
  theCommands.CheckInputNotSameAsOutput(input, output);
  Expression finalBase;
  finalBase.reset(theCommands, input.children.size);
  finalBase.AddChildOnTop(input[0][1]);
  for (int i = 1; i < input.children.size; i ++) {
    finalBase.AddChildOnTop(input[i]);
  }
  return output.MakeXOX(theCommands, theCommands.opThePower(), finalBase, input[0][2]);
}

bool CalculatorFunctions::innerConstantFunction(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerConstantFunction");
  (void) theCommands;//portable way of avoiding unused parameter warning
  if (!input.IsListNElements()) {
    return false;
  }
  if (!input[0].IsConstantNumber()) {
    return false;
  }
  output = input[0];
  return true;
}

bool CalculatorFunctions::outerCombineFractionsCommutative(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerCombineFractionsCommutative");
  if (!input.StartsWith(theCommands.opPlus(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (
    !leftE.StartsWith(theCommands.opDivide(), 3) ||
    !rightE.StartsWith(theCommands.opDivide(), 3)
  ) {
    return false;
  }
  theCommands << "<br><b>To do: make function outerCombineFractionsCommutative work much better.</b>";
  if (leftE[2] == rightE[2]) {
    output = (leftE[1] + rightE[1]) / leftE[2];
    return true;
  }
  output = (leftE[1] * rightE[2] + rightE[1] * leftE[2]) / (leftE[2] * rightE[2]);
  return true;
}

template<class coefficient>
void Polynomial<coefficient>::GetPolyWithPolyCoeff(
  Selection& theNonCoefficientVariables, Polynomial<Polynomial<coefficient> >& output
) const {
  MacroRegisterFunctionWithName("Polynomial::GetPolyWithPolyCoeff");
  if (theNonCoefficientVariables.MaxSize != this->minimalNumberOfVariables()) {
    global.fatal << "GetPolyWithPolyCoeff called with selection which has "
    << "selects the wrong number of variables. " << global.fatal;
  }
  output.makeZero();
  MonomialP coeffPart, polyPart;
  Polynomial<coefficient> currentCF;
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
    currentCF.AddMonomial(coeffPart, this->coefficients[i]);
    output.AddMonomial(polyPart, currentCF);
  }
}

template<class coefficient>
void Polynomial<coefficient>::GetPolyUnivariateWithPolyCoeffs(
  int theVar, Polynomial<Polynomial<coefficient> >& output
) const {
  Selection theVars;
  theVars.init(this->minimalNumberOfVariables());
  theVars.AddSelectionAppendNewIndex(theVar);
  this->GetPolyWithPolyCoeff(theVars, output);
}

template <class coefficient>
bool Polynomial<coefficient>::GetLinearSystemFromLinearPolys(
  const List<Polynomial<coefficient> >& theLinPolys,
  Matrix<coefficient>& homogenousPart,
  Matrix<coefficient>& constTerms
) {
  MacroRegisterFunctionWithName("Polynomial::GetLinearSystemFromLinearPolys");
  int theLetter;
  int numVars = 0;
  for (int i = 0; i < theLinPolys.size; i ++) {
    numVars = MathRoutines::Maximum(theLinPolys[i].minimalNumberOfVariables(), numVars);
  }
  homogenousPart.init(theLinPolys.size, numVars);
  homogenousPart.makeZero();
  constTerms.init(theLinPolys.size, 1);
  constTerms.makeZero();
  for (int i = 0; i < theLinPolys.size; i ++) {
    for (int j = 0; j < theLinPolys[i].size(); j ++) {
      if (theLinPolys[i][j].IsLinearNoConstantTerm(&theLetter)) {
        homogenousPart(i, theLetter) = theLinPolys[i].coefficients[j];
      } else if (theLinPolys[i][j].IsConstant()) {
        constTerms(i, 0) = theLinPolys[i].coefficients[j];
        constTerms(i, 0) *= - 1;
      } else {
        return false;
      }
    }
  }
  return true;
}

template <class coefficient>
coefficient Polynomial<coefficient>::GetDiscriminant() {
  MacroRegisterFunctionWithName("Polynomial::GetDiscriminant");
  if (this->minimalNumberOfVariables() > 1) {
    global.fatal
    << "I do not have a definition of discriminant "
    << "for more than one variable. "
    << global.fatal;
  }
  if (this->totalDegree() != 2) {
    global.fatal
    << "Discriminant not implemented for polynomial "
    << "of degree not equal to 1."
    << global.fatal;
  }
  coefficient a = this->GetMonomialCoefficient(MonomialP(0, 2));
  coefficient b = this->GetMonomialCoefficient(MonomialP(0, 1));
  coefficient c = this->GetMonomialCoefficient(MonomialP(0, 0));
  return b * b - a * c * 4;
}

class IntegralRFComputation {
public:
  RationalFunction theRF;
  Polynomial<Rational> theDen, theNum;
  Polynomial<Rational> quotientRat, remainderRat;
  List<Polynomial<Rational> > theFactors;
  Polynomial<AlgebraicNumber> thePolyThatMustVanish;
  Polynomial<AlgebraicNumber> remainderRescaledAlgebraic;
  RationalFunction transformedRF;
  FormatExpressions currentFormaT;
  Expression integrationSetE;
  Expression contextE;
  Expression inpuTE;
  Expression outputIntegralE;
  AlgebraicNumber additionalMultiple;
  Calculator* owner;
  bool allFactorsAreOfDegree2orless;
  bool needPolyDivision;
  int NumberOfSystemVariables;
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
  bool ComputePartialFractionDecomposition();
  void PrepareFormatExpressions();
  void PrepareDenominatorFactors();
  void PrepareNumerators();
  void PrepareFinalAnswer();
  bool PreparePFExpressionSummands();
  std::string ToStringRationalFunctionLatex();
  bool IntegrateRF();
  IntegralRFComputation(Calculator* inputOwner): owner(inputOwner) {}
  bool CheckConsistency() const;
};

bool IntegralRFComputation::CheckConsistency() const {
  if (this->owner == nullptr) {
    global.fatal << "Non-initialized rf computation" << global.fatal;
  }
  return true;
}

bool IntegralRFComputation::PreparePFExpressionSummands() {
  MacroRegisterFunctionWithName("IntegralRFComputation::PreparePFExpressionSummands");
  this->CheckConsistency();
  Expression polyE, currentNum, denExpE, currentDenNoPowerMonic,
  currentDen, currentPFnoCoeff, currentPFWithCoeff,
  coeffE;
  this->thePFSummands.setSize(0);
  Polynomial<AlgebraicNumber> denominatorRescaled, numeratorRescaled;
  AlgebraicNumber currentCoefficient, numScale;
  List<MonomialP>::Comparator* monomialOrder = &MonomialP::orderDefault();
  for (int i = 0; i < this->theNumerators.size; i ++) {
    for (int j = 0; j < this->theNumerators[i].size; j ++) {
      if (this->theNumerators[i][j].IsEqualToZero()) {
        continue;
      }
      denominatorRescaled = this->theDenominatorFactorsWithMults[i];
      numeratorRescaled = this->theNumerators[i][j];
      denominatorRescaled.GetIndexLeadingMonomial(nullptr, &currentCoefficient, monomialOrder);
      currentCoefficient.invert();
      denominatorRescaled *= currentCoefficient;
      MathRoutines::RaiseToPower(currentCoefficient, j + 1, AlgebraicNumber(1));
      numeratorRescaled.GetIndexLeadingMonomial(nullptr, &numScale, monomialOrder);
      numeratorRescaled /= numScale;
      currentCoefficient *= numScale;
      polyE.AssignValueWithContext(numeratorRescaled, this->contextE, *this->owner);
      if (!CalculatorConversions::functionExpressionFromBuiltInType(*this->owner, polyE, currentNum)) {
        return false;
      }
      polyE.AssignValueWithContext(denominatorRescaled, this->contextE, *this->owner);
      if (!CalculatorConversions::functionExpressionFromBuiltInType(*this->owner, polyE, currentDenNoPowerMonic)) {
        return false;
      }
      if (j != 0) {
        denExpE.AssignValue(j + 1, *this->owner);
        currentDen.MakeXOX(*this->owner, this->owner->opThePower(), currentDenNoPowerMonic, denExpE);
      } else {
        currentDen = currentDenNoPowerMonic;
      }
      currentPFnoCoeff = currentNum;
      currentPFnoCoeff /= currentDen;
      coeffE.AssignValue(currentCoefficient, *this->owner);
      currentPFWithCoeff = coeffE * currentPFnoCoeff;
      currentPFWithCoeff.CheckConsistencyRecursively();
      this->thePFSummands.addOnTop(currentPFWithCoeff);
    }
  }
  if (!this->quotientRat.IsEqualToZero()) {
    Expression currentPFpolyForm;
    currentPFpolyForm.AssignValueWithContext(
      this->quotientRat, this->contextE, *this->owner
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
    currentPFWithCoeff.CheckConsistencyRecursively();
    this->thePFSummands.addOnTop(currentPFWithCoeff);
  }
  return true;
}

bool IntegralRFComputation::IntegrateRF() {
  MacroRegisterFunctionWithName("IntegralRFComputation::IntegrateRF");
  this->CheckConsistency();
  if (!this->ComputePartialFractionDecomposition()) {
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
      if (this->theNumerators[i][j].IsEqualToZero()) {
        continue;
      }
      denRescaled = this->theDenominatorFactorsWithMults[i];
      numRescaled = this->theNumerators[i][j];
      currentCoefficient = denRescaled.GetLeadingCoefficient(monomialOrder);
      currentCoefficient.invert();
      denRescaled *= currentCoefficient;
      MathRoutines::RaiseToPower(currentCoefficient, j + 1, AlgebraicNumber(1));
      numScale = numRescaled.GetLeadingCoefficient(monomialOrder);
      numRescaled /= numScale;
      currentCoefficient *= numScale;
      polyE.AssignValueWithContext(numRescaled, this->contextE, *this->owner);
      if (!CalculatorConversions::functionExpressionFromBuiltInType(*this->owner, polyE, currentNum)) {
        return false;
      }
      polyE.AssignValueWithContext(denRescaled, this->contextE, *this->owner);
      if (!CalculatorConversions::functionExpressionFromBuiltInType(*this->owner, polyE, currentDenNoPowerMonic)) {
        return false;
      }
      if (j != 0) {
        denExpE.AssignValue(j + 1, *this->owner);
        currentDen.MakeXOX(*this->owner, this->owner->opThePower(), currentDenNoPowerMonic, denExpE);
      } else {
        currentDen = currentDenNoPowerMonic;
      }
      currentIntegrand = currentNum;
      currentIntegrand /= currentDen;
      currentIntegralNoCoeff.MakeIntegral(
        *this->owner,
        this->integrationSetE,
        currentIntegrand,
        this->contextE.ContextGetContextVariable(0)
      );
      coeffE.AssignValue(currentCoefficient, *this->owner);
      currentIntegralWithCoeff = coeffE * currentIntegralNoCoeff;
      currentIntegralWithCoeff.CheckConsistencyRecursively();
      this->theIntegralSummands.addOnTop(currentIntegralWithCoeff);
    }
  }
  if (!this->quotientRat.IsEqualToZero()) {
    Expression currentIntegrandPolyForm;
    currentIntegrandPolyForm.AssignValueWithContext(
      this->quotientRat, this->contextE, *this->owner
    );
    if (!CalculatorConversions::functionExpressionFromPoly<Rational>(
      *this->owner, currentIntegrandPolyForm, currentIntegrand
    )) {
      *this->owner << "<br>Something is wrong: failed to convert polynomial "
      << currentIntegrandPolyForm.toString()
      << " to expression. This shouldn't happen. ";
      return false;
    }
    currentIntegralWithCoeff.MakeIntegral(
      *this->owner,
      this->integrationSetE,
      currentIntegrand,
      this->contextE.ContextGetContextVariable(0)
    );
    currentIntegralWithCoeff.CheckConsistencyRecursively();
    this->theIntegralSummands.addOnTop(currentIntegralWithCoeff);
  }
  this->theIntegralSum.makeSum(*this->owner, this->theIntegralSummands);
  this->theIntegralSum.CheckConsistencyRecursively();
  return true;
}

void IntegralRFComputation::PrepareFormatExpressions() {
  MacroRegisterFunctionWithName("IntegralRFComputation::PrepareFormatExpressions");
  std::stringstream rfStream, polyStream;
  rfStream << transformedRF.toString(&this->currentFormaT) << " = ";
  polyStream << remainderRescaledAlgebraic.toString(&this->currentFormaT) << " = ";
  int varCounter = 0;
  for (int i = 0; i < this->theDenominatorFactorsWithMults.size(); i ++) {
    int tempSize = - 1;
    this->theDenominatorFactorsWithMults.coefficients[i].IsSmallInteger(&tempSize);
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
        this->currentFormaT.polyAlphabeT.addOnTop(varNameStream.str());
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

void IntegralRFComputation::PrepareNumerators() {
  MacroRegisterFunctionWithName("IntegralRFComputation::PrepareNumerators");
  this->transformedRF = this->remainderRat;
  this->transformedRF /= this->theDen;
  this->remainderRescaledAlgebraic = this->remainderRat;
  this->remainderRescaledAlgebraic /= additionalMultiple;
  this->NumberOfSystemVariables = 0;
  Polynomial<AlgebraicNumber> currentSummand;
  MonomialP currentMon;
  this->thePolyThatMustVanish.makeZero();
  this->thePolyThatMustVanish -= remainderRescaledAlgebraic;
  this->theNumerators.setSize(this->theDenominatorFactorsWithMults.size());
  for (int i = 0; i < this->theDenominatorFactorsWithMults.size(); i ++) {
    int tempSize = - 1;
    this->theDenominatorFactorsWithMults.coefficients[i].IsSmallInteger(&tempSize);
    this->theNumerators[i].setSize(tempSize);
    for (int k = 0; k < this->theDenominatorFactorsWithMults.coefficients[i]; k ++) {
      currentSummand.makeZero();
      this->theNumerators[i][k].makeZero();
      for (int j = 0; j < this->theDenominatorFactorsWithMults[i].totalDegree(); j ++) {
        this->NumberOfSystemVariables ++;
        currentMon.MakeEi(this->NumberOfSystemVariables);
        currentMon.setVariable(0, j);
        this->theNumerators[i][k].AddMonomial(currentMon, 1);
        currentSummand.AddMonomial(currentMon, 1);
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

void IntegralRFComputation::PrepareFinalAnswer() {
  MacroRegisterFunctionWithName("IntegralRFComputation::PrepareFinalAnswer");
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
  if (!this->quotientRat.IsEqualToZero()) {
    answerFinalStream << this->quotientRat.toString(&this->currentFormaT) << "+ ";
    answerFinalStream << this->transformedRF.toString(&this->currentFormaT) << "=";
  }
  if (!this->quotientRat.IsEqualToZero()) {
    answerFinalStream << this->quotientRat.toString(&this->currentFormaT) << "+ ";
  }
  answerFinalStream << this->stringRationalFunctionPartialFractionLatex;
  this->stringFinalAnswer = answerFinalStream.str();
}

void IntegralRFComputation::PrepareDenominatorFactors() {
  MacroRegisterFunctionWithName("IntegralRFComputation::PrepareDenominatorFactors");
  this->printoutPFsHtml << "The rational function is: " << HtmlRoutines::GetMathSpanPure(
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
    this->printoutPFsHtml << HtmlRoutines::GetMathSpanPure(this->theFactors[i].toString(&this->currentFormaT));
    bool needsParenthesis = this->theFactors[i].NeedsParenthesisForMultiplication();
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

bool IntegralRFComputation::ComputePartialFractionDecomposition() {
  MacroRegisterFunctionWithName("IntegralRFComputation::ComputePartialFractionDecomposition");
  this->CheckConsistency();
  this->contextE = this->inpuTE.GetContext();
  this->contextE.ContextGetFormatExpressions(this->currentFormaT);
  if (
    this->theRF.minimalNumberOfVariables() < 1 ||
    this->theRF.expressionType == this->theRF.typeRational ||
    this->theRF.expressionType == this->theRF.typePoly
  ) {
    this->printoutPFsHtml << this->theRF.toString(&this->currentFormaT) << " is already split into partial fractions. ";
    return true;
  }
  this->theRF.GetDenominator(this->theDen);
  this->theRF.GetNumerator(this->theNum);
  this->theNum *= this->theDen.scaleNormalizeLeadingMonomial();
  PolynomialFactorization<Rational, PolynomialFactorizationKronecker> factorization;
  if (!factorization.factor(
    this->theDen,
    &this->printoutPFsHtml
  )) {
    this->printoutPFsHtml << "<hr>Failed to factor the denominator of the rational function, I surrender. ";
    return false;
  }
  this->theNum /= factorization.constantFactor;
  this->theDen /= factorization.constantFactor;
  this->theFactors = factorization.reduced;
  Polynomial<Rational> tempP;
  tempP.MakeConst(1);
  for (int i = 0; i < this->theFactors.size; i ++) {
    tempP *= this->theFactors[i];
  }
  if (tempP != this->theDen) {
    global.fatal << "Something is very wrong: product of denominator factors is "
    << tempP.toString(&this->currentFormaT)
    << ", but the denominator equals: "
    << this->theDen.toString(&this->currentFormaT) << ". " << global.fatal;
  }
  this->printoutPFsLatex
  << "\\documentclass{article}\\usepackage{longtable}\\usepackage{xcolor}\\usepackage{multicol} "
  << "\\begin{document}";
  this->PrepareDenominatorFactors();
  if (!allFactorsAreOfDegree2orless) {
    this->printoutPFsHtml
    << "There were factors (over the rationals) of "
    << "degree greater than 2. I surrender. ";
    return false;
  }
  List<MonomialP>::Comparator monomialOrder = MonomialP::orderDefault();
  this->currentFormaT.flagUseFrac = true;
  this->theNum.DivideBy(
    this->theDen,
    this->quotientRat,
    this->remainderRat,
   & monomialOrder
  );
  needPolyDivision = !this->quotientRat.IsEqualToZero();
  if (needPolyDivision) {
    this->printoutPFsHtml << "<br>The numerator "
    << HtmlRoutines::GetMathSpanPure(this->theNum.toString(&this->currentFormaT))
    << " divided by the denominator "
    << HtmlRoutines::GetMathSpanPure(theDen.toString(&this->currentFormaT))
    << " yields "
    << HtmlRoutines::GetMathSpanPure(this->quotientRat.toString(&this->currentFormaT))
    << " with remainder "
    << HtmlRoutines::GetMathSpanPure(this->remainderRat.toString(&this->currentFormaT)) << ". ";
    GroebnerBasisComputation<Rational> theGB;
    theGB.flagDoLogDivision = true;
    theGB.flagStoreQuotients = true;
    theGB.theBasiS.setSize(1);
    theGB.theBasiS[0] = this->theDen;
    theGB.theFormat = this->currentFormaT;
    theGB.thePolynomialOrder.theMonOrder = monomialOrder;
    theGB.initializeForDivision(theGB.theBasiS);
    Polynomial<Rational> theNumCopy = this->theNum;
    theGB.RemainderDivisionByBasis(theNumCopy, &theGB.remainderDivision, - 1);
    this->printoutPFsLatex << "Here is a detailed long polynomial division. ";
    this->printoutPFsLatex << theGB.GetDivisionStringLaTeX();
    this->printoutPFsHtml << "<br>Here is a detailed long polynomial division:<br> ";
    this->printoutPFsHtml << theGB.GetDivisionStringHtml();
  }
  LinearCombination<Polynomial<Rational>, Rational> theDenominatorFactorsWithMultsCopy;
  theDenominatorFactorsWithMultsCopy.makeZero();
  for (int i = 0; i < this->theFactors.size; i ++) {
    theDenominatorFactorsWithMultsCopy.AddMonomial(this->theFactors[i], 1);
  }
  theDenominatorFactorsWithMultsCopy.QuickSortAscending();
  Polynomial<Rational> currentSecondDegreePoly;
  this->theDenominatorFactorsWithMults.makeZero();
  Polynomial<AlgebraicNumber> currentLinPoly, currentSecondDegreePolyAlgebraic;
  this->additionalMultiple = 1;
  for (int i = 0; i < theDenominatorFactorsWithMultsCopy.size(); i ++) {
    currentSecondDegreePoly = theDenominatorFactorsWithMultsCopy[i];
    currentSecondDegreePolyAlgebraic = currentSecondDegreePoly;
    if (currentSecondDegreePoly.totalDegree() != 2) {
      this->theDenominatorFactorsWithMults.AddMonomial(
        currentSecondDegreePolyAlgebraic, theDenominatorFactorsWithMultsCopy.coefficients[i]
      );
      continue;
    }
    Rational theDiscriminant = currentSecondDegreePoly.GetDiscriminant();
    if (theDiscriminant < 0) {
      this->theDenominatorFactorsWithMults.AddMonomial(
        currentSecondDegreePolyAlgebraic, theDenominatorFactorsWithMultsCopy.coefficients[i]
      );
      continue;
    }
    AlgebraicNumber theDiscriminantSqrt;
    if (!theDiscriminantSqrt.AssignRationalQuadraticRadical(
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
    theDiscriminantSqrt.CheckConsistency();
    AlgebraicNumber a = currentSecondDegreePoly.GetMonomialCoefficient(MonomialP(0, 2));
    AlgebraicNumber b = currentSecondDegreePoly.GetMonomialCoefficient(MonomialP(0, 1));
    a.CheckConsistency();
    b.CheckConsistency();
    currentLinPoly.makeMonomial(0, 1);
    currentLinPoly -= (- b + theDiscriminantSqrt) / (a * 2);
    this->theDenominatorFactorsWithMults.AddMonomial(currentLinPoly, theDenominatorFactorsWithMultsCopy.coefficients[i]);
    currentLinPoly.makeMonomial(0, 1);
    currentLinPoly -= (- b - theDiscriminantSqrt) / (a * 2);
    this->theDenominatorFactorsWithMults.AddMonomial(currentLinPoly, theDenominatorFactorsWithMultsCopy.coefficients[i]);
    additionalMultiple *= a;
  }
  this->theDenominatorFactorsWithMults.QuickSortAscending();
  this->printoutPFsHtml << "<br><br>I need to find "
  << HtmlRoutines::GetMathSpanPure("A_i")
  << "'s so that I have the equality of rational functions: ";
  this->printoutPFsLatex
  << "We need to find $A_i$'s so that we have the following equality of rational functions. ";
  this->PrepareNumerators();
  this->PrepareFormatExpressions();
  this->printoutPFsHtml << HtmlRoutines::GetMathSpanPure(this->stringRationalFunctionLatex, - 1);
  this->printoutPFsHtml << "<br><br>After clearing denominators, we get the equality: ";
  this->printoutPFsLatex << "After clearing denominators, we get the following equality. ";
  this->printoutPFsHtml << "<br><br>" << HtmlRoutines::GetMathSpanPure(this->stringPolyIndentityNonSimplifiedLatex, - 1);
  this->printoutPFsLatex << "\\[" << this->stringPolyIndentityNonSimplifiedLatex << "\\]";
  Polynomial<Polynomial<AlgebraicNumber> > univariateThatMustDie;
  thePolyThatMustVanish.GetPolyUnivariateWithPolyCoeffs(0, univariateThatMustDie);
  this->printoutPFsHtml << "<br><br>After rearranging we get that the following polynomial must vanish: "
  << HtmlRoutines::GetMathSpanPure(univariateThatMustDie.toString(&this->currentFormaT));
  this->printoutPFsLatex << "After rearranging we get that the following polynomial must vanish. Here, by ``vanish'' "
  << "we mean that the coefficients of the powers of $x$ must be equal to zero."
  << "\\[" << univariateThatMustDie.toString(&this->currentFormaT) << "\\]";
  this->printoutPFsHtml << "<br>Here, by ``vanish'', we mean that the coefficients in front of the powers of x must vanish.";
  Matrix<AlgebraicNumber> theSystemHomogeneous, theSystemHomogeneousForLaTeX, theConstTerms, theConstTermsForLaTeX;
  Polynomial<AlgebraicNumber>::GetLinearSystemFromLinearPolys(univariateThatMustDie.coefficients, theSystemHomogeneous, theConstTerms);
  theSystemHomogeneousForLaTeX = theSystemHomogeneous;
  theConstTermsForLaTeX = theConstTerms;
  this->currentFormaT.flagFormatMatrixAsLinearSystem = true;
  this->printoutPFsHtml << "<br>In other words, we need to solve the system: "
  << HtmlRoutines::GetMathSpanPure(theSystemHomogeneous.ToStringSystemLatex(&theConstTerms, &this->currentFormaT), - 1);
  this->printoutPFsLatex << "In other words, we need to solve the following system. "
  << "\\[" << theSystemHomogeneous.ToStringSystemLatex(&theConstTerms, &this->currentFormaT) << "\\]";
  this->currentFormaT.flagUseHTML = true;
  theSystemHomogeneous.GaussianEliminationByRows(&theConstTerms, nullptr, nullptr, &this->printoutPFsHtml, &this->currentFormaT);
  this->currentFormaT.flagUseHTML = false;
  theSystemHomogeneousForLaTeX.GaussianEliminationByRows(&theConstTermsForLaTeX, nullptr, nullptr, &this->printoutPFsLatex, &this->currentFormaT);
  PolynomialSubstitution<AlgebraicNumber> theSub;
  theSub.MakeIdSubstitution(this->NumberOfSystemVariables + 1);
  for (int i = 1; i < theSub.size; i ++) {
    theSub[i].MakeConst(theConstTerms(i - 1, 0));
  }
  for (int i = 0; i < theDenominatorFactorsWithMults.size(); i ++) {
    for (int k = 0; k < theDenominatorFactorsWithMults.coefficients[i]; k ++) {
      this->theNumerators[i][k].Substitution(theSub);
    }
  }
  this->PrepareFinalAnswer();
  this->printoutPFsHtml << "<br>Therefore, the final partial fraction decomposition is: "
  << HtmlRoutines::GetMathSpanPure(this->stringFinalAnswer);
  this->printoutPFsLatex << "Therefore, the final partial fraction decomposition is the following. "
  << "\\[" << this->stringFinalAnswer << "\\]";
  this->printoutPFsLatex << "\\end{document}";
  this->printoutPFsHtml << "<hr>The present printout, in latex format, in ready form "
  << "for copy + paste to your latex editor, follows<hr> ";
  this->printoutPFsHtml << this->printoutPFsLatex.str();
  return true;
}

bool CalculatorFunctions::innerSplitToPartialFractionsOverAlgebraicRealS(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctions::functionSplitToPartialFractionsOverAlgebraicReals(
    theCommands, input[1], output
  );
}

bool CalculatorFunctions::functionSplitToPartialFractionsOverAlgebraicReals(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSplitToPartialFractionsOverAlgebraicReals");
  IntegralRFComputation theComputation(&theCommands);
  bool isGood = CalculatorConversions::functionRationalFunction(theCommands, input, theComputation.inpuTE);
  if (isGood) {
    isGood = theComputation.inpuTE.IsOfType<RationalFunction>();
  }
  if (!isGood) {
    return theCommands << "CalculatorFunctions::innerSplitToPartialFractionsOverAlgebraicReals: "
    << "Failed to convert "
    << input.toString() << " to rational function. ";
  }
  theComputation.theRF = theComputation.inpuTE.GetValue<RationalFunction>();
  if (theComputation.theRF.minimalNumberOfVariables() > 1) {
    return theCommands << "The input rational function is of "
    << theComputation.theRF.minimalNumberOfVariables() << " variables and "
    << " I can handle only 1.";
  }
  if (!theComputation.ComputePartialFractionDecomposition()) {
    return theCommands << "Did not manage do decompose "
    << input.toString() << " into partial fractions. ";
  }
  if (!theComputation.PreparePFExpressionSummands()) {
    return theCommands << "Something went wrong while collecting summands "
    << "while splitting "
    << input.toString() << " into partial fractions. ";
  }
  return output.MakeSequence(theCommands, &theComputation.thePFSummands);
}

bool CalculatorFunctions::innerSplitToPartialFractionsOverAlgebraicRealsAlgorithm(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSplitToPartialFractionsOverAlgebraicReals");
  IntegralRFComputation theComputation(&theCommands);
  bool isGood = CalculatorConversions::innerRationalFunctioN(theCommands, input, theComputation.inpuTE);
  if (isGood) {
    isGood = theComputation.inpuTE.IsOfType<RationalFunction>();
  }
  if (!isGood) {
    return theCommands << "CalculatorFunctions::innerSplitToPartialFractionsOverAlgebraicReals: "
    << "Failed to convert "
    << input.toString() << " to rational function. ";
  }
  theComputation.theRF = theComputation.inpuTE.GetValue<RationalFunction>();
  if (theComputation.theRF.minimalNumberOfVariables() > 1) {
    return theCommands << "The input rational function is of " << theComputation.theRF.minimalNumberOfVariables() << " variables and "
    << " I can handle only 1. ";
  }
  theComputation.ComputePartialFractionDecomposition();
  return output.AssignValue(theComputation.printoutPFsHtml.str(), theCommands);
}

bool CalculatorFunctions::innerGaussianEliminationMatrix(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("innerGaussianEliminationMatrix");
  Expression theConverted;
  if (!CalculatorConversions::innerMakeMatrix(
    theCommands, input, theConverted
  )) {
    return theCommands
    << "<hr>Failed to extract algebraic number matrix from "
    << input.toString() << ". ";
  }
  Matrix<AlgebraicNumber> theMat;
  if (!theCommands.functionGetMatrix(theConverted, theMat)) {
    return theCommands
    << "<hr>Failed to extract algebraic number matrix, "
    << "got intermediate conversion to: "
    << theConverted.toString();
  }
  if (theMat.NumRows < 2) {
    return theCommands
    << "<hr>The matrix I got as input had only 1 row. Possible user typo?";
  }
  std::stringstream out;
  FormatExpressions theFormat;
  theFormat.flagUseFrac = true;
  theMat.GaussianEliminationByRows(nullptr, nullptr, nullptr, &out, &theFormat);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerCompositeConstTimesAnyActOn(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompositeConstTimesAnyActOn");
  if (!input.IsListNElements()) {
    return false;
  }
  if (!input[0].StartsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  if (!input[0][1].IsConstantNumber()) {
    return false;
  }
  Expression functionActsOnE;
  functionActsOnE.reset(theCommands);
  functionActsOnE.AddChildOnTop(input[0][2]);
  functionActsOnE.AddChildOnTop(input[1]);
  theCommands.CheckInputNotSameAsOutput(input, output);
  return output.MakeXOX(theCommands, theCommands.opTimes(), input[0][1], functionActsOnE);
}

bool CalculatorFunctions::innerCompositeEWAactOnPoly(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompositeEWAactOnPoly");
  if (input.size() != 2) {
    return false;
  }
  Expression theEWAE = input[0];
  Expression theArgument = input[1];
  if (!theEWAE.IsListStartingWithAtom(theCommands.opElementWeylAlgebra())) {
    return false;
  }
  if (!theArgument.IsBuiltInTypE()) {
    return false;
  }
  if (!theEWAE.MergeContexts(theEWAE, theArgument)) {
    return false;
  }
  Polynomial<Rational> theArgumentPoly;
  Expression theArgumentConverted;
  if (theArgument.ConvertInternally<Polynomial<Rational> >(theArgumentConverted)) {
    theArgumentPoly = theArgumentConverted.GetValue<Polynomial<Rational> >();
  } else if (theArgument.ConvertInternally<ElementWeylAlgebra<Rational> >(theArgumentConverted)) {
    if (!theArgumentConverted.GetValue<ElementWeylAlgebra<Rational> >().IsPolynomial(&theArgumentPoly)) {
      return false;
    }
  } else {
    return false;
  }
  const ElementWeylAlgebra<Rational>& theEWA = theEWAE.GetValue<ElementWeylAlgebra<Rational> >();
  if (theEWA.HasNonSmallPositiveIntegerDerivation()) {
    return theCommands << "<hr> I cannot apply " << theEWA.toString()
    << " onto " << theArgumentPoly.toString() << " as "
    << "the differential operator contains non-integral differential operator exponents. ";
  }
  if (!theEWA.ActOnPolynomial(theArgumentPoly)) {
    std::stringstream out;
    out << "Failed to act by operator " << theEWA.toString() << " on polynomial " << theArgumentPoly.toString()
    << " (possibly the weyl algebra element has non-integral exponents)";
    return output.MakeError(out.str(), theCommands);
  }
  return output.AssignValueWithContext(theArgumentPoly, theEWAE.GetContext(), theCommands);
}

bool CalculatorFunctions::innerFormatCPPSourceCode(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  if (!global.UserDefaultHasAdminRights()) {
    return theCommands << "Cpp code formatting available only to logged-in admins. ";
  }
  std::string fileName;
  if (!input.IsOfType(&fileName)) {
    return false;
  }
  std::stringstream report;
  CodeFormatter formatter;
  formatter.FormatCPPSourceCode(fileName, "", &report);
  return output.AssignValue(report.str(), theCommands);
}

bool CalculatorFunctions::innerFormatCPPDirectory(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  if (!global.UserDefaultHasAdminRights()) {
    return theCommands << "Cpp code formatting available only to logged-in admins. ";
  }
  std::string fileName;
  if (!input.IsOfType(&fileName)) {
    return false;
  }
  std::stringstream report;
  CodeFormatter::FormatCPPDirectory(fileName, &report);
  return output.AssignValue(report.str(), theCommands);
}

bool CalculatorFunctions::innerIntersection(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntersection");
  if (!input.StartsWith(theCommands.opIntersection())) {
    return false;
  }
  int numElts = 1;
  for (int i = 1; i < input.size(); i ++) {
    if (!input[i].IsSequenceNElementS()) {
      return false;
    } else {
      numElts += input[i].size() - 1;
    }
  }
  List<List<Expression> > theListsToBeIntersected;
  theListsToBeIntersected.setSize(input.size() - 1);
  for (int i = 1; i < input.size(); i ++) {
    theListsToBeIntersected[i - 1].Reserve(input[i].size());
    for (int j = 1; j < input[i].size(); j ++) {
      theListsToBeIntersected[i - 1].addOnTop(input[i][j]);
    }
  }
  if (theListsToBeIntersected.size == 0) {
    return false;
  }
  List<Expression> outputList = theListsToBeIntersected[0];
  for (int i = 1; i < theListsToBeIntersected.size; i ++) {
    outputList.IntersectWith(theListsToBeIntersected[i], outputList);
  }
  return output.MakeSequence(theCommands, &outputList);
}


bool CalculatorFunctions::innerUnion(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerUnion");
  if (!input.StartsWith(theCommands.opUnion())) {
    return false;
  }
  int numElts = 1;
  for (int i = 1; i < input.size(); i ++) {
    if (!input[i].IsSequenceNElementS()) {
      return false;
    }
  }
  output.reset(theCommands, numElts);
  output.AddChildAtomOnTop(theCommands.opSequence());
  for (int i = 1; i < input.size(); i ++) {
    for (int j = 1; j < input[i].size(); j ++) {
      output.AddChildOnTop(input[i][j]);
    }
  }
  return true;
}

bool CalculatorFunctions::innerUnionNoRepetition(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerUnionNoRepetition");
  if (!input.IsLisT()) {
    return false;
  }
  int numElts = 1;
  for (int i = 1; i < input.size(); i ++) {
    if (!input[i].IsListStartingWithAtom(theCommands.opSequence())) {
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
    theIndices[i] = theCommands.theExpressionContainer.AddNoRepetitionOrReturnIndexFirst(theList[i]);
  }
  output.children.Reserve(numElts);
  output.reset(theCommands, theIndices.size + 1);
  output.AddChildAtomOnTop(theCommands.opSequence());
  output.children.addOnTop(theIndices);
  return true;
}

bool CalculatorFunctions::innerCrossProduct(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCrossProduct");
  if (!input.IsListStartingWithAtom(theCommands.opCrossProduct()) || input.children.size != 3) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!leftE.IsSequenceNElementS(3) || !rightE.IsSequenceNElementS(3)) {
    std::stringstream out;
    out << "Can't compute cross product of the non-3d vectors " << input[1].toString() << " and "
    << input[2].toString() << ". ";
    return output.MakeError(out.str(), theCommands);
  }
  List<Expression> outputSequence;
  outputSequence.setSize(3);
  outputSequence[0] = leftE[2] * rightE[3] - leftE[3] * rightE[2];
  outputSequence[1] = leftE[3] * rightE[1] - leftE[1] * rightE[3];
  outputSequence[2] = leftE[1] * rightE[2] - leftE[2] * rightE[1];
  return output.MakeSequence(theCommands, &outputSequence);
}

bool CalculatorFunctions::innerDifferentiateConstPower(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDifferentiateConstPower");
  //////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].IsAtom()) {
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& theDOvar = input[1];
  const Expression& theArgument = input[2];
  //////////////////////
  if (!theArgument.StartsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  if (!theArgument[2].IsConstantNumber()) {
    return false;
  }
  Expression theMonomial, theTerm, theExponent, basePrime, minusOne;
  minusOne.AssignValue<Rational>(- 1, theCommands);
  theExponent.MakeXOX(theCommands, theCommands.opPlus(), theArgument[2], minusOne);
  theMonomial.MakeXOX(theCommands, theCommands.opThePower(), theArgument[1], theExponent);
  basePrime.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  theTerm.MakeXOX(theCommands, theCommands.opTimes(), theArgument[2], theMonomial);
  return output.MakeXOX(theCommands, theCommands.opTimes(), theTerm, basePrime);
}

bool CalculatorFunctions::innerDifferentiateAPowerB(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDifferentiateAPowerB");
  //////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].IsAtom()) {
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& theDOvar = input[1];
  const Expression& theArgument = input[2];
  //////////////////////
  //d/dx a^b= d/dx(e^{b\\ln a}) = a^b d/dx(b\\log a)
  if (!theArgument.StartsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  Expression logBase, exponentTimesLogBase, derivativeExponentTimesLogBase;
  logBase.reset(theCommands, 2);
  logBase.AddChildAtomOnTop(theCommands.opLog());
  logBase.AddChildOnTop(theArgument[1]);
  exponentTimesLogBase.MakeXOX(theCommands, theCommands.opTimes(), theArgument[2], logBase);
  derivativeExponentTimesLogBase.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, exponentTimesLogBase);
  return output.MakeXOX(theCommands, theCommands.opTimes(), theArgument, derivativeExponentTimesLogBase);
}

bool CalculatorFunctions::innerDifferentiateConstant(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDifferentiateConstant");
  //////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].IsAtom()) {
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& theArgument = input[2];
  //////////////////////
  if (!theArgument.IsConstantNumber()) {
    return false;
  }
  return output.AssignValue<Rational>(0, theCommands);
}

bool CalculatorFunctions::innerDifferentiateX(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDifferentiateX");
  //////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].IsAtom()) {
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& theDOvar = input[1];
  const Expression& theArgument = input[2];
  //////////////////////
  if (theArgument != theDOvar)
    return false;
  return output.AssignValue<Rational>(1, theCommands);
}

bool CalculatorFunctions::innerDifferentiateTrigAndInverseTrig(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDifferentiateTrigAndInverseTrig");
  /////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].IsAtom()) {
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression "
    << input[1].toString() << " - possible user typo?";
  }
  const Expression& theArgument = input[2];
  //////////////////////
  if (theArgument.IsOperationGiven(theCommands.opSin())) {
    return output.MakeAtom(theCommands.opCos(), theCommands);
  }
  if (theArgument.IsOperationGiven(theCommands.opCos())) {
    Expression mOneE, sinE;
    mOneE.AssignValue<Rational>(- 1, theCommands);
    sinE.MakeAtom(theCommands.opSin(), theCommands);
    return output.MakeXOX(theCommands, theCommands.opTimes(), mOneE, sinE);
  }
  if (theArgument.IsOperationGiven(theCommands.opTan())) {
    Expression secE, twoE;
    secE.MakeAtom(theCommands.opSec(), theCommands);
    twoE.AssignValue(2, theCommands);
    return output.MakeXOX(theCommands, theCommands.opThePower(), secE, twoE);
  }
  if (theArgument.IsOperationGiven(theCommands.opCot())) {
    Expression cscE, twoE, cscSquared, mOneE;
    cscE.MakeAtom(theCommands.opCsc(), theCommands);
    twoE.AssignValue(2, theCommands);
    cscSquared.MakeXOX(theCommands, theCommands.opThePower(), cscE, twoE);
    mOneE.AssignValue(- 1, theCommands);
    return output.MakeXOX(theCommands, theCommands.opTimes(), mOneE, cscSquared);
  }
  if (theArgument.IsOperationGiven(theCommands.opSec())) {
    Expression tanE, secE;
    tanE.MakeAtom(theCommands.opTan(), theCommands);
    secE.MakeAtom(theCommands.opSec(), theCommands);
    return output.MakeXOX(theCommands, theCommands.opTimes(), tanE, secE);
  }
  if (theArgument.IsOperationGiven(theCommands.opCsc())) {
    Expression cotE, cscE, mOneE, cotTimesCscE;
    cotE.MakeAtom(theCommands.opCot(), theCommands);
    cscE.MakeAtom(theCommands.opCsc(), theCommands);
    mOneE.AssignValue(- 1, theCommands);
    cotTimesCscE.MakeXOX(theCommands, theCommands.opTimes(), cotE, cscE);
    return output.MakeXOX(theCommands, theCommands.opTimes(), mOneE, cotTimesCscE);
  }
  if (theArgument.IsOperationGiven(theCommands.opArcTan())) {
    Polynomial<Rational> onePlusXsquared;
    RationalFunction oneOverOnePlusXsquared;
    onePlusXsquared.makeMonomial(0, 2);
    onePlusXsquared += Rational::one();
    oneOverOnePlusXsquared.makeOne();
    oneOverOnePlusXsquared /= onePlusXsquared;
    Expression theContext;
    theContext.ContextMakeContextWithOnePolyVar(theCommands, "x");
    return output.AssignValueWithContext(oneOverOnePlusXsquared, theContext, theCommands);
  }
  if (theArgument.IsOperationGiven(theCommands.opArcSin())) {
    Expression denE, theContext;
    theContext.ContextMakeContextWithOnePolyVar(theCommands, "x");
    RationalFunction oneMinusXsquared;
    oneMinusXsquared.makeMonomial(0, 2);
    oneMinusXsquared *= - 1;
    oneMinusXsquared += 1;
    denE.AssignValueWithContext(oneMinusXsquared, theContext, theCommands);
    return output.MakeXOX(theCommands, theCommands.opThePower(), denE, theCommands.EMHalf());
  }
  if (theArgument.IsOperationGiven(theCommands.opArcCos())) {
    Expression denE, theContext;
    theContext.ContextMakeContextWithOnePolyVar(theCommands, "x");
    RationalFunction oneMinusXsquared;
    oneMinusXsquared.makeMonomial(0, 2);
    oneMinusXsquared *= - 1;
    oneMinusXsquared += 1;
    denE.AssignValueWithContext(oneMinusXsquared, theContext, theCommands);
    output.MakeXOX(theCommands, theCommands.opThePower(), denE, theCommands.EMHalf());
    output *= - 1;
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerCompositeDifferentiateLog(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompositeDifferentiateLog");
  /////////////////////
  if (input.size() != 2) {
    return false;
  }
  if (!input[0].StartsWith(theCommands.opDifferentiate(), 3)) {
    return false;
  }
  if (!input[0][2].IsOperationGiven(theCommands.opLog())) {
    return false;
  }
  theCommands.CheckInputNotSameAsOutput(input, output);
  Expression OneE;
  OneE.AssignValue(1, theCommands);
  output.reset(theCommands, 2);
  return output.MakeXOX(theCommands, theCommands.opDivide(), OneE, input[1]);
}

bool CalculatorFunctions::outerDivideByNumber(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::outerDivide");
  if (!input.StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  if (input[2].IsEqualToZero()) {
    return output.MakeError("Division by zero. ", theCommands);
  }
  Rational theRatValue;
  AlgebraicNumber theAlgValue;
  double theDoubleValue;
  Expression theInvertedE;
  bool result = false;
  if (input[2].IsOfType<Rational>(&theRatValue)) {
    theRatValue.invert();
    theInvertedE.AssignValue(theRatValue, theCommands);
    result = true;
  }
  if (input[2].IsOfType<AlgebraicNumber>(&theAlgValue)) {
    theAlgValue.invert();
    theInvertedE.AssignValue(theAlgValue, theCommands);
    result = true;
  }
  if (input[2].IsOfType<double>(&theDoubleValue)) {
    theDoubleValue = 1 / theDoubleValue;
    theInvertedE.AssignValue(theDoubleValue, theCommands);
    result = true;
  }
  if (!result) {
    return false;
  }
  output = theInvertedE * input[1];
  return true;
}

bool CalculatorFunctions::innerMax(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerMin");
  (void) theCommands;
  if (input.size() < 3) {
    return false;
  }
  double theMax = 0, current = 0;
  int bestIndex = 1;
  for (int i = 1; i < input.size(); i ++) {
    if (!input[i].EvaluatesToDouble(&current)) {
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

bool CalculatorFunctions::innerMin(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerMin");
  (void) theCommands;
  if (input.size() < 3) {
    return false;
  }
  double theMin = 0, current = 0;
  int bestIndex = 1;
  for (int i = 1; i < input.size(); i ++) {
    if (!input[i].EvaluatesToDouble(& current)) {
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
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerEqualEqualEqual");
  if (!input.IsListNElements(3)) {
    return false;
  }
  const Expression& left  = input[1];
  const Expression& right = input[2];
  int result = 0;
  if (left == right) {
    result = 1;
  }
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctions::outerEqualEqual(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerEqualEqual");
  if (!input.IsListNElements(3)) {
    return false;
  }
  const Expression& left = input[1];
  const Expression& right = input[2];
  if (left.HasBoundVariables() || right.HasBoundVariables()) {
    return false;
  }
  if (left == right) {
    return output.AssignValue(1, theCommands);
  }
  if (left.IsEqualToMathematically(right)) {
    return output.AssignValue(1, theCommands);
  } else {
    return output.AssignValue(0, theCommands);
  }
}

bool CalculatorFunctions::outerAssociateAdivBdivCpowerD(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerAssociateAdivBdivCpowerD");
  if (!input.StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  if (!input[2].StartsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  if (!input[2][1].StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  theCommands.CheckInputNotSameAsOutput(input, output);
  Expression numeratorE, numeratorLeftE, denominatorE;
  output.reset(theCommands, 3);
  numeratorLeftE.MakeXOX(theCommands, theCommands.opThePower(), input[2][1][2], input[2][2]);
  numeratorE.MakeXOX(theCommands, theCommands.opTimes(), input[1], numeratorLeftE);
  denominatorE.MakeXOX(theCommands, theCommands.opThePower(), input[2][1][1], input[2][2]);
  return output.MakeXOX(theCommands, theCommands.opDivide(), numeratorE, denominatorE);
}

bool CalculatorFunctions::outerDivCancellations(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerDivCancellations");
  if (!input.StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  if (!input[1].StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  if (!input[2].StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  if (input[1][2] == input[2][2]) {
    return output.MakeXOX(theCommands, theCommands.opDivide(), input[1][1], input[2][1]);
  }
  if (input[1][1] == input[2][1]) {
    return output.MakeXOX(theCommands, theCommands.opDivide(), input[2][2], input[1][2]);
  }
  return false;
}

bool CalculatorFunctions::outerAssociateDivisionDivision(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerAssociateDivisionDivision");
  if (!input.StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  if (input[1].StartsWith(theCommands.opDivide(), 3)) {
    Expression newRightE;
    newRightE.MakeXOX(theCommands, theCommands.opTimes(), input[2], input[1][2]);
    return output.MakeXOX(theCommands, theCommands.opDivide(), input[1][1], newRightE);
  }
  if (input[2].StartsWith(theCommands.opDivide(), 3)) {
    Expression newLeftE;
    newLeftE.MakeXOX(theCommands, theCommands.opTimes(), input[1], input[2][2]);
    return output.MakeXOX(theCommands, theCommands.opDivide(), newLeftE, input[2][1]);
  }
  return false;
}

bool CalculatorFunctions::innerDifferentiateChainRule(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDifferentiateChainRule");
  /////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].IsAtom()) {
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& theDOvar = input[1], theArgument = input[2];
  //////////////////////
  if (!theArgument.StartsWith(- 1, 2)) {
    return false;
  }
  if (!theArgument.IsGoodForChainRuleFunction() && !theArgument[0].IsGoodForChainRuleFunction()) {
    return false;
  }
  theCommands.CheckInputNotSameAsOutput(input, output);
  output.reset(theCommands);
  Expression multiplicandleft, multiplicandleftFunction, multiplicandright;
  multiplicandleftFunction.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[0]);
  multiplicandleft.reset(theCommands);
  multiplicandleft.AddChildOnTop(multiplicandleftFunction);
  multiplicandleft.AddChildOnTop(theArgument[1]);
  multiplicandright.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  return output.MakeXOX(theCommands, theCommands.opTimes(), multiplicandleft, multiplicandright);
}

bool CalculatorFunctions::innerDifferentiateAplusB(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDifferentiateAplusB");
  /////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].IsAtom()) {
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression "
    << input[1].toString() << " - possible user typo?";
  }
  const Expression& theDOvar = input[1], theArgument = input[2];
  //////////////////////
  if (!theArgument.StartsWith(theCommands.opPlus(), 3)) {
    return false;
  }
  theCommands.CheckInputNotSameAsOutput(input, output);
  output.reset(theCommands);
  Expression leftSummand, rightSummand;
  leftSummand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  rightSummand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[2]);
  return output.MakeXOX(theCommands, theCommands.opPlus(), leftSummand, rightSummand);
}

bool CalculatorFunctions::innerDifferentiateAtimesB(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDifferentiateAtimesB");
    //////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].IsAtom()) {
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression" << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& theDOvar = input[1], theArgument = input[2];
  //////////////////////
  if (!theArgument.StartsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  theCommands.CheckInputNotSameAsOutput(input, output);
  output.reset(theCommands);
  Expression changedMultiplicand, leftSummand, rightSummand;
  changedMultiplicand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  leftSummand.MakeXOX(theCommands, theCommands.opTimes(), changedMultiplicand, theArgument[2]);
  changedMultiplicand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[2]);
  rightSummand.MakeXOX(theCommands, theCommands.opTimes(), theArgument[1], changedMultiplicand );
  return output.MakeXOX(theCommands, theCommands.opPlus(), leftSummand, rightSummand);
}

bool CalculatorFunctions::innerPowerAnyToZero(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPowerAnyToZero");
  if (!input.StartsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  if (!input[2].IsEqualToZero()) {
    return false;
  }
  if (input[1].StartsWith(theCommands.opIntegral())) {
    if (input[1].size() != 3) { //<- an expression of the form \int_a^b (without f*dx).
      return false;
    }
  }
  if (input[1].IsEqualToZero()) {
    return output.MakeError("Error: expression of the form 0^0 is illegal.", theCommands);
  }
  return output.AssignValue<Rational>(1, theCommands);
}

bool CalculatorFunctions::innerDifferentiateAdivideBCommutative(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDifferentiateAdivideBCommutative");
    //////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].IsAtom()) {
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression " << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& theDOvar = input[1], theArgument = input[2];
  //////////////////////
  //Quotient rule (commutative): (a/b^n)'= (a'b-n a b')/b^{n + 1}
  if (!theArgument.StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  const Expression& numeratorE = theArgument[1];
  const Expression& denominatorE = theArgument[2];
  if (numeratorE.StartsWith(theCommands.opPlus())) {
    Expression leftE(theCommands), rightE(theCommands);
    leftE.AddChildAtomOnTop(theCommands.opDifferentiate());
    leftE.AddChildOnTop(theDOvar);
    rightE = leftE;
    leftE.AddChildOnTop(numeratorE[1] / denominatorE);
    rightE.AddChildOnTop(numeratorE[2] / denominatorE);
    output = leftE + rightE;
    return true;
  }
  theCommands.CheckInputNotSameAsOutput(input, output);
  output.reset(theCommands);
  Expression theDenominatorBase, eOne, theDenominatorExponentPlusOne, theDenominatorExponent, changedMultiplicand,
  leftSummand, rightSummand, theDenominatorFinal, numerator;
  eOne.AssignValue(1, theCommands);
  bool denBaseFound = false;
  if (theArgument[2].StartsWith(theCommands.opThePower(), 3)) {
    if (theArgument[2][2].IsConstantNumber()) {
      denBaseFound = true;
      theDenominatorBase = theArgument[2][1];
      theDenominatorExponent = theArgument[2][2];
      theDenominatorExponentPlusOne.MakeXOX(theCommands, theCommands.opPlus(), theDenominatorExponent, eOne);
    }
  }
  if (!denBaseFound) {
    theDenominatorBase = theArgument[2];
    theDenominatorExponentPlusOne.AssignValue(2, theCommands);
    theDenominatorExponent.AssignValue(1, theCommands);
  }
  theDenominatorFinal.MakeXOX(theCommands, theCommands.opThePower(), theDenominatorBase, theDenominatorExponentPlusOne);
  changedMultiplicand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  leftSummand.MakeXOX(theCommands, theCommands.opTimes(), changedMultiplicand, theDenominatorBase);
  rightSummand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theDenominatorBase);
  changedMultiplicand.MakeXOX(theCommands, theCommands.opTimes(), theArgument[1], rightSummand);
  rightSummand.MakeXOX(theCommands, theCommands.opTimes(), theDenominatorExponent, changedMultiplicand);
  numerator.MakeXOX(theCommands, theCommands.opMinus(), leftSummand, rightSummand);
  return output.MakeXOX(theCommands, theCommands.opDivide(), numerator, theDenominatorFinal);
}

bool CalculatorFunctions::innerDifferentiateAdivideBNONCommutative(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDifferentiateAdivideBNONCommutative");
    //////////////////////
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].IsAtom()) {
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression " << input[1].toString()
    << " - possible user typo?";
  }
  const Expression& theDOvar = input[1], theArgument = input[2];
  //////////////////////
  //Quotient rule (non-commutative): (a/b)'= (ab^{- 1})'=a' b - a b^{- 1} b' b^{- 1}
  if (!theArgument.StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  theCommands.CheckInputNotSameAsOutput(input, output);
  output.reset(theCommands);
  Expression changedMultiplicand, leftSummand, rightSummand;
  Expression bInverse, bPrime, eMOne;
  eMOne.AssignValue<Rational>(- 1, theCommands);
  changedMultiplicand.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[1]);
  leftSummand.MakeXOX(theCommands, theCommands.opDivide(), changedMultiplicand, theArgument[2]);
  bPrime.MakeXOX(theCommands, theCommands.opDifferentiate(), theDOvar, theArgument[2]);
  bInverse.MakeXOX(theCommands, theCommands.opThePower(), theArgument[2], eMOne);
  rightSummand.MakeXOX(theCommands, theCommands.opTimes(), bPrime, bInverse); //rightSummand = b' b^{- 1}
  changedMultiplicand.MakeXOX(theCommands, theCommands.opTimes(), bInverse, rightSummand); //changedMultiplicand = b^- 1 b' b^- 1
  rightSummand.MakeXOX(theCommands, theCommands.opTimes(), theArgument[1], changedMultiplicand);
  return output.MakeXOX(theCommands, theCommands.opMinus(), leftSummand, rightSummand);
}

bool CalculatorFunctions::outerCommuteAtimesBifUnivariate(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerCommuteAtimesBifUnivariate");
  if (!input.StartsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  if (input[1].IsConstantNumber()) {
    return false;
  }
  HashedListSpecialized<Expression> theList;
  input.GetBlocksOfCommutativity(theList);
  if (theList.size != 1) {
    return false;
  }
  if (input[2] > input[1] || input[2] == input[1]) {
    return false;
  }
  output = input;
  output.children.SwapTwoIndices(1, 2);
  return true;
}

bool CalculatorFunctions::outerCommuteAtimesBtimesCifUnivariate(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerCommuteAtimesBtimesCifUnivariate");
  if (!input.StartsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  if (leftE.IsConstantNumber()) {
    return false;
  }
  if (!input[2].StartsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  const Expression& rightE = input[2][1];
  HashedListSpecialized<Expression> theList;

  leftE.GetBlocksOfCommutativity(theList);
  rightE.GetBlocksOfCommutativity(theList);
  if (theList.size != 1) {
    return false;
  }
  if (rightE > leftE || leftE == rightE) {
    return false;
  }
  Expression leftMultiplicand;
  leftMultiplicand.MakeXOX(theCommands, theCommands.opTimes(), rightE, leftE);
  return output.MakeXOX(theCommands, theCommands.opTimes(), leftMultiplicand, input[2][2]);
}

bool CalculatorFunctions::innerGetFreeVariablesIncludeNamedConstants(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGetFreeVariables");
  if (input.size() != 2) {
    return false;
  }
  HashedList<Expression> outputList;
  if (!input[1].GetFreeVariables(outputList, false)) {
    return theCommands << "Function GetFreeVariables failed, this shouldn't happen. ";
  }
  return output.MakeSequence(theCommands, &outputList);
}

bool CalculatorFunctions::innerGetFreeVariablesExcludeNamedConstants(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGetFreeVariables");
  if (input.size() != 2) {
    return false;
  }
  HashedList<Expression> outputList;
  if (!input[1].GetFreeVariables(outputList, true)) {
    return theCommands << "Function GetFreeVariables failed, this shouldn't happen.";
  }
  return output.MakeSequence(theCommands, &outputList);
}

bool CalculatorFunctions::innerCompareFunctionsNumerically(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompareFunctionsNumerically");
  if (input.size() < 5) {
    return theCommands << "Comparing functions takes as input at least 4 arguments "
    << "(two functions to compare and interval to compare over).";
  }
  Expression theFunE = input[1];
  theFunE -= input[2];
  HashedList<Expression> theVars;
  if (!theFunE.GetFreeVariables(theVars, true)) {
    return theCommands << "Was not able to extract the function argument of your function. " ;
  }
  if (theVars.size <= 0) {
    Expression zeroE;
    zeroE.AssignValue(0, theCommands);
    return output.MakeXOX(theCommands, theCommands.opEqualEqual(), theFunE, zeroE);
  }
  if (theVars.size > 1) {
    return theCommands << "I cannot compare the functions as they appear to depend on more than one variable, namely, on: "
    << theVars.ToStringCommaDelimited();
  }
  double leftBoundary = 0;
  double rightBoundary = 0;
  if (!input[3].EvaluatesToDouble(&leftBoundary)) {
    return theCommands << "Failed to extract the left endpoint of the comparison interval. ";
  }
  if (!input[4].EvaluatesToDouble(&rightBoundary)) {
    return theCommands << "Failed to extract the right endpoint of the comparison interval. ";
  }
  int numPoints = 50;
  if (input.size() > 5) {
    if (!input[5].IsSmallInteger(&numPoints)) {
      return theCommands << "Failed to convert argument: " << input[5].toString() << " to a small integer. ";
    }
  }
  double minDiff = 0, maxDiff = 0;
  if (!theFunE.EvaluatesToDoubleInRange(
    theVars[0].toString(), leftBoundary, rightBoundary, numPoints, &minDiff, &maxDiff, nullptr
  )) {
    return theCommands << "Failed to evaluate your function to a number. "
    << "The sampling interval may be outside of the domain of the function. ";
  }
  double tolerance = 0.0001;
  if (input.size() > 6) {
    if (!input[6].EvaluatesToDouble(&tolerance)) {
      return theCommands << "Failed to evaluate the argument " << input[6].toString() << " to a floating point number. ";
    }
  }
  if (minDiff < - tolerance || maxDiff > tolerance) {
    return output.AssignValue(0, theCommands);
  }
  return output.AssignValue(1, theCommands);
}

bool CalculatorFunctions::innerCompareExpressionsNumericallyAtPoints(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompareExpressionsNumericallyAtPoints");
  if (input.size() < 5) {
    return theCommands << "Comparing functions at points takes as input at least 5 arguments "
    << "- two functions to compare, precision, variable belonging to an interval and number of sampling points).";
  }
  Expression theFunE = input[1];
  theFunE -= input[2];
  HashedList<Expression> theFreeVars;
  if (!theFunE.GetFreeVariables(theFreeVars, true)) {
    return theCommands << "Was not able to extract the function argument of your function. " ;
  }
  if (theFreeVars.size <= 0) {
    Expression zeroE;
    zeroE.AssignValue(0, theCommands);
    return output.MakeXOX(theCommands, theCommands.opEqualEqual(), theFunE, zeroE);
  }
  double tolerance = 0.0001;
  if (!input[3].EvaluatesToDouble(&tolerance)) {
    return theCommands << "Failed to evaluate tolerance limit from " << input[3].toString()
    << " to a floating point number. ";
  }
  if (tolerance < 0) {
    tolerance *= - 1;
  }
  if (!input[4].StartsWith(theCommands.opIn(), 3)) {
    return theCommands << "The fourth argument " << input[4].toString() << " needs to be "
    << " of the form (x,y,...)\\in (...). ";
  }
  const Expression& theVarsE = input[4][1];
  HashedList<Expression> varsGiven;
  if (!theVarsE.IsSequenceNElementS()) {
    varsGiven.addOnTop(theVarsE);
  } else {
    for (int i = 1; i < theVarsE.size(); i ++) {
      if (varsGiven.Contains(theVarsE[i])) {
        return theCommands << theVarsE[i] << " given more than once. ";
      }
      varsGiven.addOnTop(theVarsE[i]);
    }
  }
  for (int i = 0; i < theFreeVars.size; i ++) {
    if (!varsGiven.Contains(theFreeVars[i])) {
      return theCommands << "The expression depends on "
      << theFreeVars[i].toString()
      << " but I no value given for that expression. ";
    }
  }
  const Expression& thePointsE = input[4][2];
  Matrix<double> thePoints;
  if (!theCommands.functionGetMatrix(
    thePointsE,
    thePoints,
    nullptr,
    varsGiven.size,
    CalculatorFunctions::functionEvaluateToDouble
  )) {
    return theCommands
    << "Failed to extract list of points from: "
    << thePointsE.toString();
  }
  HashedList<Expression> knownEs = theCommands.knownDoubleConstants;
  List<double> knownValues = theCommands.knownDoubleConstantValues;
  for (int i = 0; i < varsGiven.size; i ++) {
    if (knownEs.Contains(varsGiven[i])) {
      return theCommands << varsGiven[i]
      << " is an already known constant and cannot be used as a variable in this context. ";
    } else {
      knownEs.addOnTop(varsGiven[i]);
    }
  }
  knownValues.setSize(knownEs.size);
  int numFailedSamples = 0;
  int totalSamples = thePoints.NumRows;
  for (int i = 0; i < thePoints.NumRows; i ++) {
    for (int j = 0; j < thePoints.NumCols; j ++) {
      knownValues[j + theCommands.knownDoubleConstants.size] = thePoints(i, j);
    }
    double floatingResult = 0;
    if (!theFunE.EvaluatesToDoubleUnderSubstitutions(knownEs, knownValues, &floatingResult)) {
      numFailedSamples ++;
      if ((numFailedSamples * 100) / totalSamples > 20) {
        theCommands << "Failed to evaluate at least one of the functions in more than 20% of the sampling points. ";
        return output.AssignValue(0, theCommands);
      }
    }
    if (floatingResult > tolerance || floatingResult < - tolerance) {
      return output.AssignValue(0, theCommands);
    }
  }
  return output.AssignValue(1, theCommands);
}

bool CalculatorFunctions::innerCompareExpressionsNumerically(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompareFunctionsNumerically");
  if (input.size() < 6) {
    return theCommands << "Comparing functions takes as input at least 5 arguments "
    << "- two functions to compare, precision, variable belonging to an interval and number of sampling points).";
  }
  Expression theFunE = input[1];
  theFunE -= input[2];
  HashedList<Expression> theVars;
  if (!theFunE.GetFreeVariables(theVars, true)) {
    return theCommands << "Was not able to extract the function argument of your function. " ;
  }
  if (theVars.size <= 0) {
    Expression zeroE;
    zeroE.AssignValue(0, theCommands);
    return output.MakeXOX(theCommands, theCommands.opEqualEqual(), theFunE, zeroE);
  }
  List<double> leftBoundaries;
  List<double> rightBoundaries;
  HashedList<Expression> theBoundaryVars;
  List<int> numSamples;
  for (int i = 4; i < input.size(); i += 2) {
    const Expression& currentIntervalWithVariable = input[i];
    if (!currentIntervalWithVariable.StartsWith(theCommands.opIn(), 3)) {
      return theCommands << "Expression " << currentIntervalWithVariable.toString()
      << " is not a belonging relation (does not use the \\in symbol). ";
    }
    if (!currentIntervalWithVariable[2].IsSequenceNElementS(2)) {
      return theCommands << "Could not get a two-element sequence from " << currentIntervalWithVariable[2].toString();
    }
    double currentLeft, currentRight;
    if (!currentIntervalWithVariable[2][1].EvaluatesToDouble(&currentLeft)) {
      return theCommands << "Could not get a double from " << currentIntervalWithVariable[2][1].toString();
    }
    if (!currentIntervalWithVariable[2][2].EvaluatesToDouble(&currentRight)) {
      return theCommands << "Could not get a double from " << currentIntervalWithVariable[2][2].toString();
    }
    if (theBoundaryVars.Contains(currentIntervalWithVariable[1])) {
      return theCommands << "Expression " << currentIntervalWithVariable[1].toString()
      << " specified an interval range more than once. ";
    }
    theBoundaryVars.addOnTop(currentIntervalWithVariable[1]);
    leftBoundaries.addOnTop(currentLeft);
    rightBoundaries.addOnTop(currentRight);
    numSamples.addOnTop(10);
    int currentNumSamplingPoints = 0;
    if (i + 1 < input.size()) {
      if (input[i + 1].IsSmallInteger(&currentNumSamplingPoints)) {
        *numSamples.LastObject() = currentNumSamplingPoints;
      } else {
        return theCommands << "Failed to extract a small integer for the number of sampling points from: "
        << input[i + 1].toString();
      }
      if (*numSamples.LastObject() > 1000) {
        return theCommands << *numSamples.LastObject() << " sampling points requested for variable/expression "
        << theBoundaryVars.LastObject()->toString() << "; this exceeds the hard-coded limit of 1000. ";
      }
      if (*numSamples.LastObject() < 1) {
        return theCommands << *numSamples.LastObject() << " sampling points requested for variable/expression "
        << theBoundaryVars.LastObject()->toString() << "; this is not allowed. ";
      }
    }
  }
  for (int i = 0; i < theVars.size; i ++) {
    if (!theBoundaryVars.Contains(theVars[i])) {
      return theCommands << "Could not find an interval for variable/expression: " << theVars[i].toString();
    }
  }
  HashedList<Expression> knownEs = theCommands.knownDoubleConstants;
  List<double> knownValues = theCommands.knownDoubleConstantValues;
  for (int i = 0; i < theBoundaryVars.size; i ++) {
    if (knownEs.Contains(theBoundaryVars[i])) {
      return theCommands << theBoundaryVars[i]
      << " is an already known constant and cannot be used as a variable in this context. ";
    } else {
      knownEs.addOnTop(theBoundaryVars[i]);
    }
  }
  knownValues.setSize(knownEs.size);
  SelectionWithDifferentMaxMultiplicities theSamplingSelector;
  theSamplingSelector.initFromInts(numSamples);
  if (theSamplingSelector.TotalNumSubsets() > 1000000) {
    return theCommands << "The total number of sampling points, "
    << theSamplingSelector.TotalNumSubsets().toString() << " exceeds "
    << "the hard-coded limit of 1000000. ";
  }
  double tolerance = 0.0001;
  if (!input[3].EvaluatesToDouble(&tolerance)) {
    return theCommands << "Failed to evaluate tolerance limit from " << input[3].toString()
    << " to a floating point number. ";
  }
  if (tolerance < 0) {
    tolerance *= - 1;
  }
  Rational totalSamples = theSamplingSelector.TotalNumSubsetsMustBeSmalInt();
  Rational numFailedSamples = 0;
  do {
    for (int i = 0; i < theSamplingSelector.Multiplicities.size; i ++) {
      double& currentValue = knownValues[i + theCommands.knownDoubleConstants.size];
      double& lBound = leftBoundaries[i];
      double& rBound = rightBoundaries[i];
      if (theSamplingSelector.MaxMultiplicities[i] == 1) {
        currentValue = (lBound + rBound) / 2;
        continue;
      }
      double paramBnZeroAndOne = theSamplingSelector.Multiplicities[i];
      paramBnZeroAndOne /= theSamplingSelector.MaxMultiplicities[i] - 1;
      currentValue = lBound *(1 - paramBnZeroAndOne) + rBound * paramBnZeroAndOne;
    }
    double floatingResult = 0;
    if (!theFunE.EvaluatesToDoubleUnderSubstitutions(knownEs, knownValues, &floatingResult)) {
      numFailedSamples ++;
      if ((numFailedSamples * 100) / totalSamples > 20) {
        theCommands << "Failed to evaluate at least one of the functions in more than 20% of the sampling points. ";
        return output.AssignValue(0, theCommands);
      }
    }
    if (floatingResult > tolerance || floatingResult < - tolerance) {
      return output.AssignValue(0, theCommands);
    }
  } while (theSamplingSelector.IncrementReturnFalseIfPastLast());
  return output.AssignValue(1, theCommands);
}

bool CalculatorFunctions::innerCompositeArithmeticOperationEvaluatedOnArgument(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompositeArithmeticOperationEvaluatedOnArgument");
  if (input.size() != 2) {
    return false;
  }
  if (input[0].size() != 3) {
    return false;
  }
  if (!input[0][0].IsArithmeticOperation()) {
    return false;
  }
  Expression leftE(theCommands);
  Expression rightE(theCommands);
  leftE.AddChildOnTop(input[0][1]);
  leftE.AddChildOnTop(input[1]);
  rightE.AddChildOnTop(input[0][2]);
  rightE.AddChildOnTop(input[1]);
  return output.MakeXOX(theCommands, input[0][0].theData, leftE, rightE);
}

bool CalculatorFunctions::innerIsEven(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsEven");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (argument.HasBoundVariables()) {
    return false;
  }
  LargeInteger theInt;
  if (!argument.IsInteger(&theInt)) {
    return output.AssignValue(0, theCommands);
  }
  if (theInt.IsEven()) {
    return output.AssignValue(1, theCommands);
  }
  return output.AssignValue(0, theCommands);
}

bool CalculatorFunctions::innerIsConstant(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsConstant");
  if (input.size() != 2) {
    return false;
  }
  int result = static_cast<int>(input[1].IsConstantNumber());
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctions::innerIsNonEmptySequence(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsNonEmptySequence");
  if (input.HasBoundVariables()) {
    return false;
  }
  if (!input.IsSequenceNElementS() || input.size() < 2) {
    return output.AssignValue(0, theCommands);
  }
  return output.AssignValue(1, theCommands);
}

bool CalculatorFunctions::innerDifferentialStandardHandler(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDifferentialStandardHandler");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  output.reset(theCommands);
  output.AddChildAtomOnTop(theCommands.opDifferential());
  output.AddChildOnTop(argument);
  return output.AddChildRationalOnTop(1);
}

bool CalculatorFunctions::innerDifferentialOfPolynomial(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDifferentialOfPolynomial");
  if (input.size() != 3) {
    return false;
  }
  WithContext<Polynomial<Rational> > polynomial;
  if (!input[1].IsOfTypeWithContext(&polynomial)) {
    return false;
  }
  std::stringstream comments;
  Vector<Polynomial<Rational> > differentials;
  if (!polynomial.content.differential(differentials, &comments)) {
    return false;
  }
  List<Expression> outputSummands;
  for (int i = 0; i < differentials.size; i ++) {
    Expression incoming(theCommands);
    Expression variable = polynomial.context.context.ContextGetContextVariable(i);
    incoming.AddChildAtomOnTop(theCommands.opDifferential());
    incoming.AddChildOnTop(variable);
    Expression polynomialWrapper;
    polynomialWrapper.AssignValueWithContext(differentials[i], polynomial.context.context, theCommands);
    incoming.AddChildOnTop(polynomialWrapper);
    outputSummands.addOnTop(incoming);
  }
  return output.makeSum(theCommands, outputSummands);
}

bool CalculatorFunctions::innerIsDifferentialOneFormOneVariable(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsDifferentialOneFormOneVariable");
  if (input.size() != 2) {
    return false;
  }
  return output.AssignValue(
    static_cast<int>(input[1].IsDifferentialOneFormOneVariablE()),
    theCommands
  );
}

bool CalculatorFunctions::innerInterpretAsDifferential(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerInterpretAsDifferential");
  if (!input.StartsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  if (!input[1].StartsWith(theCommands.opIntegral())) {
    return false;
  }
  const Expression& theDiffFormCandidateE = input[2];
  std::string theDiff;
  if (theDiffFormCandidateE.IsOperation(&theDiff)) {
    if (theDiff.size() > 1) {
      if (theDiff[0] == 'd') {
        Expression variableE, diffFormE;
        variableE.MakeAtom(theDiff.substr(1, std::string::npos), theCommands);
        diffFormE.reset(theCommands);
        diffFormE.AddChildAtomOnTop(theCommands.opDifferential());
        diffFormE.AddChildOnTop(variableE);
        diffFormE.AddChildRationalOnTop(1);
        output = input;
        return output.SetChilD(2, diffFormE);
      }
    }
  }
  return false;
}

bool CalculatorFunctions::innerIntegralOperator(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompositeMultiplyIntegralFbyDx");
  int theOp = theCommands.opTimes();
  if (!input.StartsWith(theOp, 3)) {
    theOp = theCommands.opDivide();
    if (!input.StartsWith(theOp, 3)) {
      return false;
    }
  }
  const Expression& integralE = input[1];
  if (integralE.IsOperationGiven(theCommands.opIntegral())) {
    Expression integralOperatorE;
    integralOperatorE.reset(theCommands, 2);
    integralOperatorE.AddChildAtomOnTop(theCommands.opIntegral());
    integralOperatorE.AddChildAtomOnTop(theCommands.opIndefiniteIndicator());
    output = input;
    return output.SetChilD(1, integralOperatorE);
  }
  if (!integralE.StartsWith(theCommands.opIntegral())) {
    return false;
  }
  if (integralE.size() < 3) {
    output = integralE;
    return output.AddChildOnTop(input[2]);
  }
  if (integralE.size() != 3) {
    return false;
  }
  const Expression& startingIntegrand = integralE[2];
  if (startingIntegrand.IsDifferentialOneFormOneVariablE()) {
    return false;
  }
  const Expression& incomingIntegrand = input[2];
  Expression newIntegrand;
  Expression newFun;
  if (incomingIntegrand.IsDifferentialOneFormOneVariablE()) {
    newFun.MakeXOX(theCommands, theOp, startingIntegrand, incomingIntegrand[2]);
    newIntegrand.MakeXOX(theCommands, theCommands.opDifferential(), incomingIntegrand[1], newFun);
    output = integralE;
    return output.SetChilD(2, newIntegrand);
  }
  newIntegrand.MakeXOX(theCommands, theOp, startingIntegrand, incomingIntegrand);
  output = integralE;
  return output.SetChilD(2, newIntegrand);
}

bool CalculatorFunctions::innerRationalFunctionSubstitution(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerRationalFunctionSubstitution");
  if (input.size() != 2) {
    return false;
  }
  if (!input[0].IsOfType<RationalFunction>()) {
    return false;
  }
  if (input[0].GetValue<RationalFunction>().minimalNumberOfVariables() > 1) {
    return false;
  }
  Expression ResultRationalForm;
  Expression finalContext;
  finalContext.ContextMakeContextWithOnePolyVar(theCommands, input[1]);
  ResultRationalForm.AssignValueWithContext(input[0].GetValue<RationalFunction>(), finalContext, theCommands);
  return CalculatorConversions::innerExpressionFromRF(theCommands, ResultRationalForm, output);
}

bool CalculatorFunctions::innerInvertMatrixRFsVerbose(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerInvertMatrixVerbose");
  Matrix<RationalFunction> theMatrix, outputMat, extendedMatrix;
  Expression theContext, converted;
  if (!CalculatorConversions::innerMatrixRationalFunction(
    theCommands, input, converted
  )) {
    return output.MakeError("Failed to extract matrix. ", theCommands);
  }
  if (theCommands.functionGetMatrix(converted, theMatrix)) {
    return theCommands << "Failed to get matrix of rational functions. ";
  }
  theContext = converted.GetContext();
  if (theMatrix.NumRows != theMatrix.NumCols || theMatrix.NumCols < 1) {
    std::stringstream out;
    out << "The matrix " << theMatrix.toString( ) << " has "
    << theMatrix.NumCols << " columns and " << theMatrix.NumRows << " rows. "
    << "The matrix is not square.";
    return output.MakeError(out.str(), theCommands);
  }
  outputMat.MakeIdMatrix(theMatrix.NumRows);
  int tempI;
  int NumFoundPivots = 0;
  std::stringstream out, outLaTeX;

  RationalFunction tempElement;
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  theFormat.flagUseLatex = true;
  theFormat.flagUseHTML = false;
  theFormat.flagUseFrac = true;
  theFormat.MatrixColumnVerticalLineIndex = theMatrix.NumCols - 1;
  out << "Computing "
  << HtmlRoutines::GetMathSpanPure(theMatrix.toString(&theFormat) + "^{- 1}");
  extendedMatrix = theMatrix;
  extendedMatrix.AppendMatrixOnTheRight(outputMat);
  out << "<br>" << HtmlRoutines::GetMathSpanPure(extendedMatrix.toString(&theFormat)) ;
  outLaTeX << "\\begin{tabular}{ll}";
  outLaTeX << "$" << extendedMatrix.toString(& theFormat) << "$";

  for (int i = 0; i < theMatrix.NumCols; i ++) {
    tempI = theMatrix.FindPivot(i, NumFoundPivots);
    if (tempI != - 1) {
      if (tempI != NumFoundPivots) {
        theMatrix.SwitchTwoRows(NumFoundPivots, tempI);
        outputMat.SwitchTwoRows (NumFoundPivots, tempI);
        out << "<br>Swap row " << NumFoundPivots + 1 << " and row " << tempI + 1 << ": ";
        outLaTeX << "& Swap row " << NumFoundPivots + 1 << " and row " << tempI + 1 << ". ";
        extendedMatrix = theMatrix;
        extendedMatrix.AppendMatrixOnTheRight(outputMat);
        out << "<br>" << HtmlRoutines::GetMathSpanPure(outputMat.toString(&theFormat));
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
      theMatrix.RowTimesScalar(NumFoundPivots, tempElement);
      outputMat.RowTimesScalar(NumFoundPivots, tempElement);
      if (tempElement != 1) {
        extendedMatrix = theMatrix;
        extendedMatrix.AppendMatrixOnTheRight(outputMat);
        out << HtmlRoutines::GetMathSpanPure(extendedMatrix.toString(&theFormat));
        outLaTeX << "$" << extendedMatrix.toString(&theFormat) << "$";
      }
      bool found = false;
      for (int j = 0; j < theMatrix.NumRows; j ++) {
        if (j != NumFoundPivots) {
          if (!theMatrix.elements[j][i].IsEqualToZero()) {
            tempElement = theMatrix.elements[j][i];
            tempElement.Minus();
            theMatrix.AddTwoRows(NumFoundPivots, j, i, tempElement);
            outputMat.AddTwoRows(NumFoundPivots, j, 0, tempElement);
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
        extendedMatrix.AppendMatrixOnTheRight(outputMat);
        out << HtmlRoutines::GetMathSpanPure(extendedMatrix.toString(&theFormat));
        outLaTeX << "$" << extendedMatrix.toString(&theFormat) << "$";
      }
      NumFoundPivots ++;
    }
  }
  outLaTeX << "\\end{tabular}";
  theFormat.MatrixColumnVerticalLineIndex = - 1;
  if (NumFoundPivots < theMatrix.NumRows) {
    out << "<br>Matrix to the right of the vertical line not "
    << "transformed to the identity matrix => "
    << "starting matrix is not invertible. ";
    outLaTeX << "Matrix to the right of the vertical line "
    << "not transformed to the identity matrix => "
    << "starting matrix is not invertible. ";
  } else {
    out << "<br>The inverse of the starting matrix "
    << "can be read off on the matrix to the left of the id matrix: "
    << HtmlRoutines::GetMathSpanPure(outputMat.toString(&theFormat));
    outLaTeX << " The inverse matrix can now be read off as the matrix "
    << "to the left of the identity matrix: $"
    << outputMat.toString(&theFormat) << "$";
  }
  out << "Output in LaTeX: <br><br>" << outLaTeX.str();
  return output.AssignValue(out.str(), theCommands);
}

bool Calculator::innerInvertMatrixVerbose(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerInvertMatrixVerbose");
  Expression converted;
  if (!CalculatorConversions::innerMakeMatrix(theCommands, input, converted)) {
    return theCommands << "Failed to get matrix from: " << input.toString();
  }

  Matrix<Rational> mat, outputMat, tempMat;
  if (!theCommands.functionGetMatrix(
    converted, mat
  )) {
    return CalculatorFunctions::innerInvertMatrixRFsVerbose(theCommands, input, output);
  }
  if (mat.NumRows != mat.NumCols || mat.NumCols < 1) {
    return output.MakeError("The matrix is not square", theCommands);
  }
  outputMat.MakeIdMatrix(mat.NumRows);
  int tempI;
  int NumFoundPivots = 0;
  std::stringstream out;
  Rational tempElement;
  FormatExpressions theFormat;
  theFormat.flagUseLatex = true;
  theFormat.flagUseHTML = false;
  theFormat.MatrixColumnVerticalLineIndex = mat.NumCols - 1;
  out << "Computing " << HtmlRoutines::GetMathSpanPure(mat.toString(&theFormat) + "^{- 1}");
  tempMat = mat;
  tempMat.AppendMatrixOnTheRight(outputMat);
  out << "<br>" << HtmlRoutines::GetMathSpanPure(tempMat.toString(&theFormat));
  for (int i = 0; i < mat.NumCols; i ++) {
    tempI = mat.FindPivot(i, NumFoundPivots);
    if (tempI != - 1) {
      if (tempI != NumFoundPivots) {
        mat.SwitchTwoRows(NumFoundPivots, tempI);
        outputMat.SwitchTwoRows (NumFoundPivots, tempI);
        out << "<br>switch row " << NumFoundPivots + 1
        << " and row " << tempI + 1 << ": ";
        tempMat = mat;
        tempMat.AppendMatrixOnTheRight(outputMat);
        out << "<br>"
        << HtmlRoutines::GetMathSpanPure(outputMat.toString(&theFormat));
      }
      tempElement = mat.elements[NumFoundPivots][i];
      tempElement.invert();
      if (tempElement != 1) {
        out << "<br> multiply row " << NumFoundPivots + 1
        << " by " << tempElement << ": ";
      }
      mat.RowTimesScalar(NumFoundPivots, tempElement);
      outputMat.RowTimesScalar(NumFoundPivots, tempElement);
      if (tempElement != 1) {
        tempMat = mat;
        tempMat.AppendMatrixOnTheRight(outputMat);
        out << HtmlRoutines::GetMathSpanPure(tempMat.toString(&theFormat));
      }
      bool found = false;
      for (int j = 0; j < mat.NumRows; j ++) {
        if (j != NumFoundPivots) {
          if (!mat.elements[j][i].IsEqualToZero()) {
            tempElement = mat.elements[j][i];
            tempElement.Minus();
            mat.AddTwoRows(NumFoundPivots, j, i, tempElement);
            outputMat.AddTwoRows(NumFoundPivots, j, 0, tempElement);
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
        tempMat.AppendMatrixOnTheRight(outputMat);
        out << HtmlRoutines::GetMathSpanPure(tempMat.toString(&theFormat));
      }
      NumFoundPivots ++;
    }
  }
  if (NumFoundPivots < mat.NumRows) {
    out << "<br>Matrix to the right of the vertical line not "
    << "transformed to the identity matrix => "
    << "starting matrix is not invertible. ";
  } else {
    out << "<br>The inverse of the starting matrix can "
    << "be read off on the matrix to the left of the id matrix: "
    << HtmlRoutines::GetMathSpanPure(outputMat.toString(&theFormat));
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerPolynomialRelations(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerGroebner");
  Vector<Polynomial<Rational> > inputVector;
  Expression theContext;
  if (input.size() < 3) {
    return output.MakeError("Function takes at least two arguments. ", theCommands);
  }
  const Expression& numComputationsE = input[1];
  Rational upperBound = 0;
  if (!numComputationsE.IsOfType(&upperBound)) {
    return output.MakeError("Failed to convert the first argument of the expression to rational number.", theCommands);
  }
  if (upperBound > 1000000) {
    return output.MakeError(
      "Error: your upper limit of polynomial operations exceeds 1000000, which is too large."
      "You may use negative or zero number give no computation bound, but please don't. ",
      theCommands
    );
  }
  output.reset(theCommands);
  for (int i = 1; i < input.size(); i ++) {
    output.children.addOnTop(input.children[i]);
  }
  if (!theCommands.GetVectorFromFunctionArguments<Polynomial<Rational> >(
    output,
    inputVector,
    &theContext,
    - 1,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return output.MakeError("Failed to extract polynomial expressions", theCommands);
  }
  Vector<Polynomial<Rational> > theRels, theGens;
  FormatExpressions theFormat;
  theContext.ContextGetFormatExpressions(theFormat);
  for (char i = 0; i < 26; i ++) {
    char currentLetter = 'a' + i;
    std::string currentStr;
    currentStr = currentLetter;
    if (!theFormat.polyAlphabeT.Contains(currentStr)) {
      theFormat.polyAlphabeT.addOnTop(currentStr);
    }
  }
  if (!RationalFunction::GetRelations(inputVector, theGens, theRels, theCommands.Comments)) {
    return theCommands << "Failed to extract relations. ";
  }
  std::stringstream out;
  out << "Polynomials:";
  for (int i = 0; i < theGens.size; i ++) {
    out << "<br>" << theGens[i].toString(&theFormat) << "=" << inputVector[i].toString(&theFormat);
  }
  out << "<br>Relations: ";
  for (int i = 0; i < theRels.size; i ++) {
    theRels[i].scaleNormalizeLeadingMonomial();
    out << "<br>" << theRels[i].toString(&theFormat);
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::functionPolynomialize(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::functionPolynomialize");
  Expression thePolyE;
  if (input.HasBoundVariables()) {
    return false;
  }
  if (!CalculatorConversions::functionPolynomial<Rational>(
    theCommands, input, thePolyE
  )) {
    return false;
  }
  if (!CalculatorConversions::functionExpressionFromBuiltInType(
    theCommands, thePolyE, output
  )) {
    return false;
  }
  return true;
}

bool CalculatorFunctions::outerPolynomializE(Calculator& theCommands, const Expression& input, Expression& output) {
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctions::functionPolynomialize(theCommands, input[1], output);
}

bool CalculatorFunctions::innerIntegrateRationalFunctionSplitToBuidingBlocks(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::functionIntegrateRationalFunctionSplitToBuidingBlocks");
  Expression theFunctionE, theVariableE, integrationSetE;
  if (!input.IsIndefiniteIntegralfdx(&theVariableE, &theFunctionE, &integrationSetE)) {
    return false;
  }
  IntegralRFComputation theComputation(&theCommands);
  if (!CalculatorConversions::functionRationalFunction(
    theCommands, theFunctionE, theComputation.inpuTE
  )) {
    return theCommands
    << "<hr>Call of function CalculatorConversions::innerRationalFunction "
    << "failed, input was: "
    << theFunctionE.toString();
  }
  if (!theComputation.inpuTE.IsOfType<RationalFunction>()) {
    return theCommands
    << "<hr>CalculatorFunctions::innerIntegrateRationalFunctionSplitToBuidingBlocks: "
    << "failed to convert "
    << theFunctionE.toString() << " to rational function. "
    << "Attempt to converted expression yielded: " << theComputation.inpuTE.toString();
  }
  if (theComputation.inpuTE.GetNumContextVariables() > 1) {
    return theCommands << "<hr>I converted " << theFunctionE.toString()
    << " to rational function, but it is of "
    << theComputation.inpuTE.GetNumContextVariables()
    << " variables. I have been taught to work with 1 variable only. "
    << "<br>The context of the rational function is: "
    << theComputation.inpuTE.GetContext().toString();
  }
  if (theComputation.inpuTE.GetNumContextVariables() == 1) {
    if (theComputation.inpuTE.GetContext().ContextGetContextVariable(0) != theVariableE) {
      return theCommands << "<hr>The univariate rational function was in variable "
      << theComputation.inpuTE.GetContext().toString()
      << " but the variable of integration is " << theVariableE.toString();
    }
  }
  theComputation.integrationSetE = integrationSetE;
  theComputation.theRF = theComputation.inpuTE.GetValue<RationalFunction>();
  theComputation.theRF.GetDenominator(theComputation.theDen);
  theComputation.theRF.GetNumerator(theComputation.theNum);
  if (theComputation.theDen.totalDegree() < 1) {
    return false;
  }
  if (!theComputation.IntegrateRF()) {
    return theCommands << theComputation.printoutIntegration.str();
  }
  theComputation.theIntegralSum.CheckConsistencyRecursively();
  output = theComputation.theIntegralSum;
  if (output.StartsWith(theCommands.opIntegral())) {
    if (output[1] == input) {
      return false;
    }
  }
  output.CheckConsistencyRecursively();
  output.CheckInitializationRecursively();
  return true;
}

bool CalculatorFunctions::innerCoefficientsPowersOf(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCoefficientsPowersOf");
  if (input.size() != 3) {
    return false;
  }
  const Expression& theVarE = input[1];
  const Expression& theExpressionE = input[2];
  VectorSparse<Expression> theCFs;
  if (!theCommands.CollectCoefficientsPowersVar(theExpressionE, theVarE, theCFs)) {
    return theCommands << "<hr>Failed to evaluate Calculator::CollectCoefficientsPowersVar";
  }
  int highestPowerPlus1 = theCFs.GetLargestParticipatingBasisIndex() + 1;
  List<Expression> theCFsIncludingZeros;
  Expression currentCF;
  for (int i = 0; i < highestPowerPlus1; i ++) {
    int theIndex = theCFs.theMonomials.GetIndex(MonomialVector(i));
    if (theIndex == - 1) {
      currentCF.AssignValue(0, theCommands);
    } else {
      currentCF = theCFs.coefficients[theIndex];
    }
    theCFsIncludingZeros.addOnTop(currentCF);
  }
  return output.MakeSequence(theCommands, &theCFsIncludingZeros);
}

bool CalculatorFunctions::innerConstTermRelative(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerConstTermRelative");
  if (!input.StartsWithGivenOperation("ConstantTerm")) {
    return false;
  }
  Expression coeffExtractor, theCoeffs;
  coeffExtractor = input;
  coeffExtractor.SetChildAtomValue(0, "ConstantTerm");
  if (!CalculatorFunctions::innerCoefficientsPowersOf(theCommands, coeffExtractor, theCoeffs)) {
    return false;
  }
  if (theCoeffs.IsSequenceNElementS()) {
    if (theCoeffs.size() > 1) {
      output = theCoeffs[1];
      return true;
    }
  }
  return false;
}

bool CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIa(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIa");
  Expression theFunctionE, theVariableE;
  if (!input.IsIndefiniteIntegralfdx(&theVariableE, &theFunctionE)) {
    return false;
  }
  if (!theFunctionE.StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  const Expression& A = theFunctionE[1];
  Expression a, b;
  const Expression& axPlusb = theFunctionE[2];
  if (!A.IsConstantNumber()) {
    return false;
  }
  if (!CalculatorFunctions::extractLinearCoeffsWRTvariable(axPlusb, theVariableE, a, b)) {
    return false;
  }
  if (!a.IsConstantNumber() || !b.IsConstantNumber()) {
    return false;
  }
  Expression logaxPlusb;
  logaxPlusb.reset(theCommands);
  logaxPlusb.AddChildAtomOnTop(theCommands.opLog());
  logaxPlusb.AddChildOnTop(axPlusb);
  output = A;
  output /= a;
  output *= logaxPlusb;
  output.CheckConsistencyRecursively();
  output.CheckInitializationRecursively();
  return true;
}

bool CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIb(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIa");
  Expression theFunctionE, theVariableE;
  if (!input.IsIndefiniteIntegralfdx(&theVariableE, &theFunctionE)) {
    return false;
  }
  if (!theFunctionE.StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  const Expression& A = theFunctionE[1];
  const Expression& axPlusbPowerN = theFunctionE[2];
  if (!A.IsConstantNumber()) {
    return false;
  }
  if (!axPlusbPowerN.StartsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  Expression N = axPlusbPowerN[2];
  if (!N.IsConstantNumber()) {
    return false;
  }
  Expression a, b;
  const Expression& axPlusb = axPlusbPowerN[1];
  if (!CalculatorFunctions::extractLinearCoeffsWRTvariable(axPlusb, theVariableE, a, b)) {
    return false;
  }
  if (!a.IsConstantNumber() || !b.IsConstantNumber()) {
    return false;
  }
  Expression base, OneMinusN;
  OneMinusN = N;
  OneMinusN += - 1;
  OneMinusN *= - 1;
  base.MakeXOX(theCommands, theCommands.opThePower(), axPlusb, OneMinusN);
  output = A;
  output /= a;
  output /= OneMinusN;
  output *= base;
  output.CheckConsistencyRecursively();
  output.CheckInitializationRecursively();
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
  Calculator& theCommands = *theQuadratic.owner;
  if (!theCommands.CollectCoefficientsPowersVar(theQuadratic, theVariable, theCoeffs)) {
    return false;
  }
  if (theCoeffs.GetLargestParticipatingBasisIndex() != 2) {
    return false;
  }
  outputCoeffVarSquared.AssignValue(0, theCommands);
  outputCoeffLinTerm.AssignValue(0, theCommands);
  outputConstTerm.AssignValue(0, theCommands);
  if (theCoeffs.theMonomials.Contains(MonomialVector(0))) {
    outputConstTerm = theCoeffs.GetMonomialCoefficient(MonomialVector(0));
  }
  if (theCoeffs.theMonomials.Contains(MonomialVector(1))) {
    outputCoeffLinTerm = theCoeffs.GetMonomialCoefficient(MonomialVector(1));
  }
  if (theCoeffs.theMonomials.Contains(MonomialVector(2))) {
    outputCoeffVarSquared = theCoeffs.GetMonomialCoefficient(MonomialVector(2));
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
  Calculator& theCommands = *theLinearExpression.owner;
  theCommands.CollectCoefficientsPowersVar(theLinearExpression, theVariable, theCoeffs);
  if (theCoeffs.GetLargestParticipatingBasisIndex() > 1) {
    return false;
  }
  outputCoeffLinTerm.AssignValue(0, theCommands);
  outputConstTerm.AssignValue(0, theCommands);
  if (theCoeffs.theMonomials.Contains(MonomialVector(1))) {
    outputCoeffLinTerm = theCoeffs.GetMonomialCoefficient(MonomialVector(1));
  }
  if (theCoeffs.theMonomials.Contains(MonomialVector(0))) {
    outputConstTerm = theCoeffs.GetMonomialCoefficient(MonomialVector(0));
  }
  return true;
}

bool CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIIaandIIIa(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIIaandIIIa");
  Expression theFunctionE, x;
  if (!input.IsIndefiniteIntegralfdx(&x, &theFunctionE)) {
    return false;
  }
  if (!theFunctionE.StartsWith(theCommands.opDivide(), 3)) {
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
    !a.IsConstantNumber() || !b.IsConstantNumber() || !c.IsConstantNumber()||
    !A.IsConstantNumber() || !B.IsConstantNumber()
  ) {
    return theCommands << "<hr>Failed to evaluate to constant the coefficients of the block IIa and IIIa integral."
    << "The coefficients are: " << a.toString() << ", " << b.toString() << ", " << c.toString() << ", "
    << A.toString() << ", " << B.toString() << ". ";
  }
  double approxa, approxb, approxc;
  if (!a.EvaluatesToDouble(&approxa) || !b.EvaluatesToDouble(&approxb) || !c.EvaluatesToDouble(&approxc)) {
    return theCommands << "<hr>Failed to evaluate variable coefficients in denominator " << denNoPower.toString()
    << " to double. Possible user typo. ";
  }
  if (approxb*approxb >= approxa * approxc * 4) {
    return false;
  }
  Expression xSquared, bSquared, aSquared;
  Expression twoE, oneE, fourE;
  oneE.AssignValue(1, theCommands);
  twoE.AssignValue(2, theCommands);
  fourE.AssignValue(4, theCommands);
  xSquared.MakeXOX(theCommands, theCommands.opThePower(), x, twoE);
  bSquared.MakeXOX(theCommands, theCommands.opThePower(), b, twoE);
  aSquared.MakeXOX(theCommands, theCommands.opThePower(), a, twoE);

  Expression theQuadraticDiva = xSquared + (b / a) * x + c / a;
  Expression xplusbdiv2a = x + b / (twoE * a);
  Expression D = (fourE * a * c - bSquared) / (fourE * aSquared);
  Expression sqrtD;
  sqrtD.MakeSqrt(theCommands, D, 2);
  Expression arcTanArgument = xplusbdiv2a / sqrtD;
  Expression theArcTan(theCommands);
  theArcTan.AddChildAtomOnTop(theCommands.opArcTan());
  theArcTan.AddChildOnTop(arcTanArgument);
  Expression theLog(theCommands);
  theLog.AddChildAtomOnTop(theCommands.opLog());
  theLog.AddChildOnTop(theQuadraticDiva);
  Expression C = B - (A * b) / (twoE * a);
  output = (oneE / a) * ((A / twoE) * theLog + (C / sqrtD) * theArcTan);
  output.CheckConsistencyRecursively();
  output.CheckInitializationRecursively();
  return true;
}

bool CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIIIb(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIIIb");
  Expression theFunctionE, x, integrationSetE;
  if (!input.IsIndefiniteIntegralfdx(&x, &theFunctionE, &integrationSetE)) {
    return false;
  }
  if (!theFunctionE.StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  if (!theFunctionE[2].StartsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  Expression numPowerE = theFunctionE[2][2];
  int numPower = 0;
  if (!numPowerE.IsSmallInteger(&numPower)) {
    return false;
  }
  if (numPower <= 1) {
    return false;
  }
  Expression denNoPower = theFunctionE[2][1];
  Expression a, b, c;
  if (!theFunctionE[1].IsEqualToOne()) {
    return false;
  }
  if (!CalculatorFunctions::extractQuadraticCoeffsWRTvariable(denNoPower, x, a, b, c)) {
    return false;
  }
  if (!a.IsEqualToOne() || !b.IsConstantNumber() || !c.IsConstantNumber()) {
    theCommands << "<hr>Failed to evaluate to constant the coefficients of the block IIIb integral."
    << "The coefficients are: " << a.toString() << ", " << b.toString() << ", " << c.toString() << ". ";
    return false;
  }
  double approxa, approxb, approxc;
  if (!a.EvaluatesToDouble(&approxa) || !b.EvaluatesToDouble(&approxb) || !c.EvaluatesToDouble(&approxc)) {
    theCommands << "Failed to evaluate variable coefficients in denominator " << denNoPower.toString()
    << " to double. Possible user typo. ";
    return false;
  }
  if (approxb * approxb >= approxa * approxc * 4) {
    return false;
  }
  Expression xSquared, bSquared;
  Expression twoE, oneE, threeE, fourE;
  oneE.AssignValue(1, theCommands);
  twoE.AssignValue(2, theCommands);
  threeE.AssignValue(3, theCommands);
  fourE.AssignValue(4, theCommands);

  xSquared.MakeXOX(theCommands, theCommands.opThePower(), x, twoE);
  bSquared.MakeXOX(theCommands, theCommands.opThePower(), b, twoE);

  Expression theMonicQuadratic = xSquared + b * x + c;
  Expression D = c - bSquared / fourE;
  Expression remainingIntegral, functionRemainingToIntegrate, theQuadraticPowerOneMinusN, theQuadraticPowerNMinusOne;
  theQuadraticPowerOneMinusN.MakeXOX(theCommands, theCommands.opThePower(), theMonicQuadratic, oneE - numPowerE);
  theQuadraticPowerNMinusOne.MakeXOX(theCommands, theCommands.opThePower(), theMonicQuadratic, numPowerE - oneE);
  functionRemainingToIntegrate = oneE / theQuadraticPowerNMinusOne;
  remainingIntegral.MakeIntegral(theCommands, integrationSetE, functionRemainingToIntegrate, x);
  output = oneE / D *
  ((x + b / twoE) / (twoE * numPowerE - twoE) * theQuadraticPowerOneMinusN +
  (twoE * numPowerE - threeE) / (twoE * numPowerE - twoE) * remainingIntegral);
  output.CheckConsistencyRecursively();
  output.CheckInitializationRecursively();
  return true;
}

bool CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIIb(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegrateRationalFunctionBuidingBlockIIb");
  Expression theFunctionE, x, integrationSetE;
  if (!input.IsIndefiniteIntegralfdx(&x, &theFunctionE, &integrationSetE)) {
    return false;
  }
  if (!theFunctionE.StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  if (!theFunctionE[2].StartsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  Expression nE = theFunctionE[2][2];
  int numPower = 0;
  if (!nE.IsSmallInteger(&numPower)) {
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
  if (A.IsEqualToZero()) { //building block is of type IIIb
    return false;
  }
  if (
    !a.IsConstantNumber() || !b.IsConstantNumber() || !c.IsConstantNumber()||
    !A.IsConstantNumber() || !B.IsConstantNumber()
  ) {
    theCommands << "<hr>Failed to evaluate to constant the coefficients of the block IIb integral."
    << "The coefficients are: " << a.toString() << ", " << b.toString() << ", " << c.toString() << ", "
    << A.toString() << ", " << B.toString() << ". ";
    return false;
  }
  double approxa, approxb, approxc;
  if (!a.EvaluatesToDouble(&approxa) || !b.EvaluatesToDouble(&approxb) || !c.EvaluatesToDouble(&approxc)) {
    theCommands << "Failed to evaluate variable coefficients in denominator " << denNoPower.toString()
    << " to double. Possible user typo. ";
    return false;
  }
  if (approxb * approxb >= approxa * approxc * 4) {
    return false;
  }
  Expression xSquared, bSquared, aSquared, apowerN;
  Expression twoE, oneE, fourE;
  Expression remainingIntegral, remainingFunctionToIntegrate, quadraticPowerN, quadraticPowerOneMinusN;
  oneE.AssignValue(1, theCommands);
  twoE.AssignValue(2, theCommands);
  fourE.AssignValue(4, theCommands);
  apowerN.MakeXOX(theCommands, theCommands.opThePower(), a, nE);
  xSquared.MakeXOX(theCommands, theCommands.opThePower(), x, twoE);
  bSquared.MakeXOX(theCommands, theCommands.opThePower(), b, twoE);
  aSquared.MakeXOX(theCommands, theCommands.opThePower(), a, twoE);
  Expression theQuadraticDiva = xSquared + (b / a) * x + c / a;
  quadraticPowerN.MakeXOX(theCommands, theCommands.opThePower(), theQuadraticDiva, nE);
  quadraticPowerOneMinusN.MakeXOX(theCommands, theCommands.opThePower(), theQuadraticDiva, oneE - nE);
  remainingFunctionToIntegrate = oneE / quadraticPowerN;
  remainingIntegral.MakeIntegral(theCommands, integrationSetE, remainingFunctionToIntegrate, x);
  Expression C = B - (A * b) / (twoE * a);
  oneE.CheckInitializationRecursively();
  apowerN.CheckInitializationRecursively();
  A.CheckInitializationRecursively();
  twoE.CheckInitializationRecursively();
  nE.CheckInitializationRecursively();
  quadraticPowerOneMinusN.CheckInitializationRecursively();
  C.CheckInitializationRecursively();
  remainingIntegral.CheckInitializationRecursively();
  output = (oneE / apowerN) * (A / (twoE * (oneE - nE)) * quadraticPowerOneMinusN + C * remainingIntegral);
  output.CheckConsistencyRecursively();
  output.CheckInitializationRecursively();
  return true;
}

bool CalculatorFunctions::innerIntegratePowerByUncoveringParenthesisFirst(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegratePowerByUncoveringParenthesisFirst");
  Expression theFunctionE, integrandE, newIntegralE, theVariableE, integrationSetE;
  if (!input.IsIndefiniteIntegralfdx(&theVariableE, &theFunctionE, &integrationSetE)) {
    return false;
  }
  if (!theFunctionE.StartsWith(theCommands.opThePower())) {
    return false;
  }
  if (!CalculatorFunctions::functionPolynomialize(theCommands, theFunctionE, integrandE)) {
    return false;
  }
  if (integrandE == theFunctionE) {
    return false;
  }
  newIntegralE.MakeIntegral(theCommands, integrationSetE, integrandE, theVariableE);
  if (!theCommands.EvaluateExpression(theCommands, newIntegralE, output)) {
    return false;
  }
  if (output.ContainsAsSubExpressionNoBuiltInTypes(theCommands.opIntegral())) {
    return false;
  }
  output.CheckConsistencyRecursively();
  output.CheckInitializationRecursively();
  return true;
}

bool Expression::MakeIntegral(
  Calculator& theCommands,
  const Expression& integrationSet,
  const Expression& theFunction,
  const Expression& theVariable
) {
  MacroRegisterFunctionWithName("Expression::MakeIntegral");
  if (this == &theFunction || this == &theVariable || this == &integrationSet) {
    Expression theFunCopy = theFunction;
    Expression theVarCopy = theVariable;
    Expression integrationSetCopy = integrationSet;
    return this->MakeIntegral(theCommands, integrationSetCopy, theFunCopy, theVarCopy);
  }
  Expression theDiffForm;
  theDiffForm.MakeXOX(theCommands, theCommands.opDifferential(), theVariable, theFunction);
  this->reset(theCommands);
  this->AddChildAtomOnTop(theCommands.opIntegral());
  this->AddChildOnTop(integrationSet);
  return this->AddChildOnTop(theDiffForm);
}

bool CalculatorFunctions::innerIntegratePullImaginaryUnit(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegratePullImaginaryUnit");
  Expression theFunctionE, theVariableE, integrationSetE;
  if (!input.IsIndefiniteIntegralfdx(&theVariableE, &theFunctionE, &integrationSetE)) {
    return false;
  }
  Expression iE;
  iE.MakeAtom(theCommands.opImaginaryUnit(), theCommands);
  if (theVariableE == iE) {
    return false;
  }
  Expression theCF, theNoCFintegrand, theNoImIntegrand, outputIntegralNoCF;
  theFunctionE.GetCoefficientMultiplicandForm(theCF, theNoCFintegrand);

  if (theNoCFintegrand == iE) {
    theNoImIntegrand.AssignValue(1, theCommands);
  } else if (theNoCFintegrand.StartsWith(theCommands.opTimes(), 3)) {
    if (theNoCFintegrand[1] != iE) {
      return false;
    }
    theNoImIntegrand = theNoCFintegrand[2];
  } else {
    return false;
  }
  theCF *= iE;
  outputIntegralNoCF.MakeIntegral(theCommands, integrationSetE,theNoImIntegrand,theVariableE);
  output = theCF * outputIntegralNoCF;
  return true;
}

bool CalculatorFunctions::innerIntegrateSum(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegrateSum");
  Expression theFunctionE, theVariableE, integrationSetE;
  if (!input.IsIndefiniteIntegralfdx(&theVariableE, &theFunctionE, &integrationSetE)) {
    return false;
  }
  if (!theFunctionE.StartsWith(theCommands.opPlus())) {
    return false;
  }
  List<Expression> integralsOfSummands;
  integralsOfSummands.setSize(theFunctionE.size() - 1);
  Expression newIntegralE, result, newSummand;
  for (int i = 1; i < theFunctionE.size(); i ++) {
    newIntegralE.MakeIntegral(theCommands, integrationSetE, theFunctionE[i], theVariableE);
    if (!theCommands.EvaluateExpression(theCommands, newIntegralE, newSummand)) {
      return false;
    }
    if (newSummand.ContainsAsSubExpressionNoBuiltInTypes(theCommands.opIntegral())) {
      return false;
    }
    if (i == 1) {
      result = newSummand;
    } else {
      result += newSummand;
    }
  }
  output = result;
  output.CheckConsistencyRecursively();
  output.CheckInitializationRecursively();
  return true;
}

bool CalculatorFunctions::innerIntegrateXnDiffX(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegrateXnDiffX");
  Expression theFunctionE, theVariableE;
  if (!input.IsIndefiniteIntegralfdx(&theVariableE, &theFunctionE)) {
    return false;
  }
  Expression theFunCoeff, theFunNoCoeff, outputNoCoeff;
  if (theFunctionE.IsConstantNumber()) {
    output = theFunctionE * theVariableE;
    return true;
  }
  theFunctionE.GetCoefficientMultiplicandForm(theFunCoeff, theFunNoCoeff);
  if (theFunNoCoeff == theVariableE) {
    output = theFunCoeff * theVariableE * theVariableE;
    output /= 2;
    return true;
  }
  if (!theFunNoCoeff.StartsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  if (theFunNoCoeff[1] != theVariableE) {
    return false;
  }
  if (!theFunNoCoeff[2].IsConstantNumber()) {
    return false;
  }
  if (theFunNoCoeff[2].IsEqualToMOne()) {
    outputNoCoeff.reset(theCommands, 2);
    outputNoCoeff.AddChildAtomOnTop(theCommands.opLog());
    outputNoCoeff.AddChildOnTop(theVariableE);
    output = theFunCoeff * outputNoCoeff;
    return true;
  }
  Expression outputPower = theFunNoCoeff[2];
  outputPower += 1;
  outputNoCoeff.MakeXOX(theCommands, theCommands.opThePower(), theVariableE, outputPower);
  outputNoCoeff /= outputPower;
  output = theFunCoeff * outputNoCoeff;
  output.CheckConsistency();
  output.CheckInitializationRecursively();
  return true;
}

bool CalculatorFunctions::innerIntegrateSinPowerNCosPowerM(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegrateSinPowerNCosPowerM");
  Expression theFunctionE, theVariableE, integrationSet;
  if (!input.IsIndefiniteIntegralfdx(&theVariableE, &theFunctionE, &integrationSet)) {
    return false;
  }
  Expression polynomializedFunctionE;
  if (!CalculatorConversions::functionPolynomial<Rational>(
    theCommands, theFunctionE, polynomializedFunctionE
  )) {
    return false;
  }
  if (!polynomializedFunctionE.IsOfType<Polynomial<Rational> >()) {
    return false;
  }
  Expression contextE = polynomializedFunctionE.GetContext();
  int numVars = contextE.ContextGetNumContextVariables();
  if (numVars > 2) {
    return false;
  }
  if (numVars == 0) {
    return false;
  }
  Expression sinPowerE, theTrigArgument, cosPowerE;
  sinPowerE.AssignValue(1, theCommands);
  cosPowerE.AssignValue(1, theCommands);
  bool firstIsSine = false;
  for (int i = 0; i < numVars; i ++) {
    Expression currentE = contextE.ContextGetContextVariable(i);
    if (
      !currentE.StartsWith(theCommands.opSin(), 2) &&
      !currentE.StartsWith(theCommands.opCos(), 2)
    ) {
      return false;
    }
    if (i == 0 && currentE.StartsWith(theCommands.opSin())) {
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
  theTrigArgument.GetCoefficientMultiplicandForm(theTrigArgCoeff, theTrigArgumentNoCoeff);
  if (theTrigArgumentNoCoeff != theVariableE) {
    return false;
  }
  const Polynomial<Rational>& theTrigPoly = polynomializedFunctionE.GetValue<Polynomial<Rational> >();
  Expression theCosE, theSinE, theCosDoubleE, theTrigArgumentDouble;
  theTrigArgumentDouble = theTrigArgument;
  theTrigArgumentDouble *= 2;
  theCosE.MakeOX(theCommands, theCommands.opCos(), theTrigArgument);
  theSinE.MakeOX(theCommands, theCommands.opSin(), theTrigArgument);
  theCosDoubleE.MakeOX(theCommands, theCommands.opCos(), theTrigArgumentDouble);
  Expression outputCandidate, currentSummandE, currentCommandListE,
  currentSubE, currentIntegrandE, currentIntegrandNonPolynomializedE,
  currentIntegral, currentIntegralComputation,
  currentIntegrandSinePart, currentIntegrandCosinePart, newVarE, newResultE, currentE, currentCF;
  Expression oneE, twoE, threeE, powerE;
  oneE.AssignValue(1, theCommands);
  twoE.AssignValue(2, theCommands);
  threeE.AssignValue(3, theCommands);
  outputCandidate.AssignValue(0,theCommands);
  newVarE = theCommands.GetNewAtom();
  newResultE = theCommands.GetNewAtom();
  for (int i = 0; i < theTrigPoly.size(); i ++) {
    const MonomialP& currentMon = theTrigPoly[i];
    int powerSine = - 1, powerCosine = - 1;
    if (!currentMon(0).IsSmallInteger(&powerSine) || !currentMon(1).IsSmallInteger(&powerCosine)) {
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
      powerE.AssignValue((powerSine - 1) / 2, theCommands);
      currentIntegrandSinePart.MakeXOX(theCommands, theCommands.opThePower(), currentE, powerE);
      powerE.AssignValue(powerCosine, theCommands);
      currentIntegrandCosinePart.MakeXOX(theCommands, theCommands.opThePower(), newVarE, powerE);
      currentCF.AssignValue(- theTrigPoly.coefficients[i], theCommands);
      currentCF /= theTrigArgCoeff;
      currentSubE.MakeXOX(theCommands, theCommands.opDefine(), newVarE, theCosE);
    } else if (powerCosine % 2 == 1) {
      currentE = oneE - newVarE * newVarE;
      powerE.AssignValue((powerCosine - 1) / 2, theCommands);
      currentIntegrandCosinePart.MakeXOX(theCommands, theCommands.opThePower(), currentE, powerE);
      powerE.AssignValue(powerSine, theCommands);
      currentIntegrandSinePart.MakeXOX(theCommands, theCommands.opThePower(), newVarE, powerE);
      currentCF.AssignValue(theTrigPoly.coefficients[i], theCommands);
      currentCF /= theTrigArgCoeff;
      currentSubE.MakeXOX(theCommands, theCommands.opDefine(), newVarE, theSinE);
    } else {
      currentE = (oneE - theCosDoubleE) / twoE;
      powerE.AssignValue(powerSine / 2, theCommands);
      currentIntegrandSinePart.MakeXOX(theCommands, theCommands.opThePower(), currentE, powerE);
      currentE = (oneE + theCosDoubleE) / twoE;
      powerE.AssignValue(powerCosine / 2, theCommands);
      currentIntegrandCosinePart.MakeXOX(theCommands, theCommands.opThePower(), currentE, powerE);
      currentCF.AssignValue(theTrigPoly.coefficients[i], theCommands);
      currentIntegrandNonPolynomializedE = currentCF * currentIntegrandSinePart * currentIntegrandCosinePart;
      currentIntegrandE.reset(theCommands);
      currentIntegrandE.AddChildAtomOnTop("Polynomialize");
      currentIntegrandE.AddChildOnTop(currentIntegrandNonPolynomializedE);
      currentIntegral.MakeIntegral(theCommands, integrationSet, currentIntegrandE, theVariableE);
      outputCandidate += currentIntegral;
      continue;
    }
    currentIntegrandNonPolynomializedE = currentCF * currentIntegrandSinePart * currentIntegrandCosinePart;
    currentIntegrandE.reset(theCommands);
    currentIntegrandE.AddChildAtomOnTop("Polynomialize");
    currentIntegrandE.AddChildOnTop(currentIntegrandNonPolynomializedE);
    currentIntegral.MakeIntegral(theCommands, integrationSet, currentIntegrandE, newVarE);

    currentIntegralComputation.MakeXOX(theCommands, theCommands.opDefine(), newResultE, currentIntegral);
    currentCommandListE.reset(theCommands);
    currentCommandListE.AddChildAtomOnTop(theCommands.opEndStatement());
    currentCommandListE.AddChildOnTop(currentIntegralComputation);
    currentCommandListE.AddChildOnTop(currentSubE);
    currentCommandListE.AddChildOnTop(newResultE);
    currentSummandE.reset(theCommands);
    currentSummandE.AddChildAtomOnTop(theCommands.opUnderscore());
    currentSummandE.AddChildOnTop(currentCommandListE);
    currentSummandE.AddChildOnTop(threeE);

    outputCandidate += currentSummandE;
  }
  output = outputCandidate;
  return true;
}

bool CalculatorFunctions::innerIntegrateTanPowerNSecPowerM(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegrateTanPowerNSecPowerM");
  Expression theFunctionE, theVariableE, integrationSet;
  if (!input.IsIndefiniteIntegralfdx(&theVariableE, &theFunctionE, &integrationSet)) {
    return false;
  }
  Expression polynomializedFunctionE;
  if (!CalculatorConversions::functionPolynomial<Rational>(
    theCommands, theFunctionE, polynomializedFunctionE
  )) {
    return false;
  }
  if (!polynomializedFunctionE.IsOfType<Polynomial<Rational> >()) {
    return false;
  }
  Expression contextE = polynomializedFunctionE.GetContext();
  int numVars = contextE.ContextGetNumContextVariables();
  if (numVars > 2) {
    return false;
  }
  if (numVars == 0) {
    return false;
  }
  Expression sinPowerE, theTrigArgument, cosPowerE;
  sinPowerE.AssignValue(1, theCommands);
  cosPowerE.AssignValue(1, theCommands);
  bool firstIsTan = false;
  for (int i = 0; i < numVars; i ++) {
    Expression currentE = contextE.ContextGetContextVariable(i);
    if (
      !currentE.StartsWith(theCommands.opTan(), 2) &&
      !currentE.StartsWith(theCommands.opSec(), 2)
    ) {
      return false;
    }
    if (i == 0 && currentE.StartsWith(theCommands.opTan())) {
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
  theTrigArgument.GetCoefficientMultiplicandForm(theTrigArgCoeff, theTrigArgumentNoCoeff);
  if (theTrigArgumentNoCoeff != theVariableE) {
    return false;
  }
  const Polynomial<Rational>& theTrigPoly = polynomializedFunctionE.GetValue<Polynomial<Rational> >();
  Expression theTanE, theSecE;
  theTanE.MakeOX(theCommands, theCommands.opTan(), theTrigArgument);
  theSecE.MakeOX(theCommands, theCommands.opSec(), theTrigArgument);
  Expression outputCandidate, currentSummandE, currentCommandListE,
  currentSubE, currentIntegrandE, currentIntegrandNonPolynomializedE,
  currentIntegral, currentIntegralComputation,
  currentIntegrandTanPart, currentIntegrandSecPart, newVarE, newResultE, currentE, currentCF;
  Expression oneE, twoE, threeE, powerE;
  oneE.AssignValue(1, theCommands);
  twoE.AssignValue(2, theCommands);
  threeE.AssignValue(3, theCommands);
  outputCandidate.AssignValue(0,theCommands);
  newVarE = theCommands.GetNewAtom();
  newResultE = theCommands.GetNewAtom();
  for (int i = 0; i < theTrigPoly.size(); i ++) {
    const MonomialP& currentMon = theTrigPoly[i];
    int powerTan = - 1, powerSec = - 1;
    if (!currentMon(0).IsSmallInteger(&powerTan) || !currentMon(1).IsSmallInteger(&powerSec)) {
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
      powerE.AssignValue((powerTan - 1) / 2, theCommands);
      currentIntegrandTanPart.MakeXOX(theCommands, theCommands.opThePower(), currentE, powerE);
      powerE.AssignValue(powerSec - 1, theCommands);
      currentIntegrandSecPart.MakeXOX(theCommands, theCommands.opThePower(), newVarE, powerE);
      currentCF.AssignValue(theTrigPoly.coefficients[i], theCommands);
      currentCF /= theTrigArgCoeff;
      currentSubE.MakeXOX(theCommands, theCommands.opDefine(), newVarE, theSecE);
    } else if (powerSec % 2 == 0) {
      currentE = oneE + newVarE * newVarE;
      powerE.AssignValue((powerSec - 2) / 2, theCommands);
      currentIntegrandSecPart.MakeXOX(theCommands, theCommands.opThePower(), currentE, powerE);
      powerE.AssignValue(powerTan, theCommands);
      currentIntegrandTanPart.MakeXOX(theCommands, theCommands.opThePower(), newVarE, powerE);
      currentCF.AssignValue(theTrigPoly.coefficients[i], theCommands);
      currentCF /= theTrigArgCoeff;
      currentSubE.MakeXOX(theCommands, theCommands.opDefine(), newVarE, theTanE);
    } else {
      return false;
      /*currentE= (oneE-theCosDoubleE)/twoE;
      powerE.AssignValue(powerSine/2, theCommands);
      currentIntegrandSinePart.MakeXOX
      (theCommands, theCommands.opThePower(),currentE, powerE);
      currentE= (oneE+theCosDoubleE)/twoE;
      powerE.AssignValue(powerCosine/2, theCommands);
      currentIntegrandCosinePart.MakeXOX
      (theCommands, theCommands.opThePower(),currentE, powerE);
      currentCF.AssignValue(theTrigPoly.theCoeffs[i], theCommands);
      currentIntegrandNonPolynomializedE=
      currentCF*currentIntegrandSinePart*currentIntegrandCosinePart;
      currentIntegrandE.reset(theCommands);
      currentIntegrandE.AddChildAtomOnTop("Polynomialize");
      currentIntegrandE.AddChildOnTop(currentIntegrandNonPolynomializedE);
      currentIntegral.MakeIntegral(theCommands, integrationSet, currentIntegrandE, theVariableE);
      outputCandidate += currentIntegral;
      continue;*/
    }
    currentIntegrandNonPolynomializedE = currentCF * currentIntegrandTanPart*currentIntegrandSecPart;
    currentIntegrandE.reset(theCommands);
    currentIntegrandE.AddChildAtomOnTop("Polynomialize");
    currentIntegrandE.AddChildOnTop(currentIntegrandNonPolynomializedE);
    currentIntegral.MakeIntegral(theCommands, integrationSet, currentIntegrandE, newVarE);

    currentIntegralComputation.MakeXOX(theCommands, theCommands.opDefine(), newResultE, currentIntegral);
    currentCommandListE.reset(theCommands);
    currentCommandListE.AddChildAtomOnTop(theCommands.opEndStatement());
    currentCommandListE.AddChildOnTop(currentIntegralComputation);
    currentCommandListE.AddChildOnTop(currentSubE);
    currentCommandListE.AddChildOnTop(newResultE);
    currentSummandE.reset(theCommands);
    currentSummandE.AddChildAtomOnTop(theCommands.opUnderscore());
    currentSummandE.AddChildOnTop(currentCommandListE);
    currentSummandE.AddChildOnTop(threeE);

    outputCandidate += currentSummandE;
  }
  output = outputCandidate;
  return true;
}

bool CalculatorFunctions::innerExploitCosEvenness(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerExploitCosEvenness");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression cfE, nonCFpart;
  argument.GetCoefficientMultiplicandForm(cfE, nonCFpart);
  Rational theRat;
  if (!cfE.IsRational(&theRat)) {
    return false;
  }
  if (theRat >= 0) {
    return false;
  }
  Expression moneE;
  moneE.AssignValue(- 1, theCommands);
  return output.MakeOX(theCommands, theCommands.opCos(), moneE * cfE * nonCFpart);
}

bool CalculatorFunctions::innerExploitSinOddness(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerExploitSinOddness");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression cfE, nonCFpart;
  argument.GetCoefficientMultiplicandForm(cfE, nonCFpart);
  Rational theRat;
  if (!cfE.IsRational(&theRat)) {
    return false;
  }
  if (theRat >= 0) {
    return false;
  }
  Expression moneE, sinE;
  moneE.AssignValue(- 1, theCommands);
  sinE.MakeOX(theCommands, theCommands.opSin(), moneE * cfE * nonCFpart);
  output = moneE * sinE;
  return true;
}

bool CalculatorFunctions::innerConvertSinToExponent(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerConvertSinToExponent");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression eE, iE, exponentArgument, minusExponentArgument, leftE, rightE;
  eE.MakeAtom(theCommands.opE(), theCommands);
  iE.MakeAtom(theCommands.opImaginaryUnit(), theCommands);
  exponentArgument = iE * argument;
  minusExponentArgument = exponentArgument * (- 1);
  leftE.MakeXOX(theCommands, theCommands.opThePower(), eE, exponentArgument);
  rightE.MakeXOX(theCommands, theCommands.opThePower(), eE, minusExponentArgument);
  output = (iE * (- 1)) * (leftE - rightE) / 2;
  return true;
}

bool CalculatorFunctions::innerConvertCosToExponent(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerConvertCosToExponent");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Expression eE, iE, exponentArgument, minusExponentArgument, leftE, rightE;
  eE.MakeAtom(theCommands.opE(), theCommands);
  iE.MakeAtom(theCommands.opImaginaryUnit(), theCommands);
  exponentArgument = iE * argument;
  minusExponentArgument = exponentArgument * (- 1);
  leftE.MakeXOX(theCommands, theCommands.opThePower(), eE, exponentArgument);
  rightE.MakeXOX(theCommands, theCommands.opThePower(), eE, minusExponentArgument);
  output = (leftE + rightE) / 2;
  return true;
}

bool CalculatorFunctions::innerPowerImaginaryUnit(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPowerImaginaryUnit");
  if (!input.StartsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  if (!input[1].IsOperationGiven(theCommands.opImaginaryUnit())) {
    return false;
  }
  LargeInteger thePower;
  if (!input[2].IsInteger(&thePower)) {
    return false;
  }
  Expression iE;
  iE.MakeAtom(theCommands.opImaginaryUnit(), theCommands);
  if (thePower % 4 == 0) {
    return output.AssignValue(1, theCommands);
  }
  if (thePower % 4 == 1) {
    output = iE;
    return true;
  }
  if (thePower % 4 == 2) {
    return output.AssignValue(- 1, theCommands);
  }
  if (thePower % 4 == 3) {
    output = iE * (- 1);
    return true;
  }
  return false; //<-this shouldn't happen
}

bool CalculatorFunctions::innerEulerFlaAsALaw(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerEulerFlaAsALaw");
  if (!input.StartsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  if (!input[1].IsOperationGiven(theCommands.opE())) {
    return false;
  }
  Expression coefficientOfI, currentE;
  Expression iE;
  iE.MakeAtom(theCommands.opImaginaryUnit(), theCommands);
  currentE.reset(theCommands, 3);
  currentE.AddChildAtomOnTop(theCommands.opCoefficientOf());
  currentE.AddChildOnTop(iE);
  currentE.AddChildOnTop(input[2]);
  if (!CalculatorFunctions::innerCoefficientOf(theCommands, currentE, coefficientOfI)) {
    return false;
  }
  if (coefficientOfI.IsEqualToZero()) {
    return false;
  }
  Expression cosE, sinE;
  cosE.MakeOX(theCommands, theCommands.opCos(), coefficientOfI);
  sinE.MakeOX(theCommands, theCommands.opSin(), coefficientOfI);
  output = cosE + iE * sinE;
  return true;
}

bool CalculatorFunctions::innerIntegrateEpowerAxDiffX(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegrateEpowerAxDiffX");
  Expression theFunctionE, theVariableE;
  if (!input.IsIndefiniteIntegralfdx(&theVariableE, &theFunctionE)) {
    return false;
  }
  Expression theFunCoeff, theFunNoCoeff;
  theFunctionE.GetCoefficientMultiplicandForm(theFunCoeff, theFunNoCoeff);
  if (!theFunNoCoeff.StartsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  if (!theFunNoCoeff[1].IsOperationGiven(theCommands.opE())) {
    return false;
  }
  Expression thePowerCoeff, thePowerNoCoeff;
  theFunNoCoeff[2].GetCoefficientMultiplicandForm(thePowerCoeff, thePowerNoCoeff);
  if (thePowerNoCoeff != theVariableE) {
    if (thePowerNoCoeff.StartsWith(theCommands.opTimes(), 3)) {
      if (
        thePowerNoCoeff[1].IsOperationGiven(theCommands.opImaginaryUnit()) &&
        thePowerNoCoeff[2] == theVariableE
      ) {
        output = thePowerNoCoeff[1] * (- 1) * theFunctionE / thePowerCoeff;
        output.CheckConsistency();
        output.CheckInitializationRecursively();
        return true;
      }
    }
    return false;
  }
  output = theFunctionE;
  output /= thePowerCoeff;
  output.CheckConsistency();
  output.CheckInitializationRecursively();
  return true;
}

bool CalculatorFunctions::innerDifferentiateSqrt(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDifferentiateSqrt");
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].IsAtom()) {
    theCommands << "<hr>Warning: differentiating with respect to the non-atomic expression "
    << input[1].toString() << " - possible user typo?";
  }
  const Expression& theArgument = input[2];
  if (!theArgument.IsOperationGiven(theCommands.opSqrt())) {
    return false;
  }
  Expression twoE(theCommands);
  Expression oneOverSqrtE(theCommands);
  twoE = Rational(1, 2);
  oneOverSqrtE = 1;
  oneOverSqrtE /= theArgument;
  return output.MakeXOX(theCommands, theCommands.opTimes(), twoE, oneOverSqrtE);
}

bool CalculatorFunctions::outerDifferentiateWRTxTimesAny(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerDifferentiateWRTxTimesAny");
  if (!input.StartsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  if (!input[1].StartsWith(theCommands.opDifferentiate(), 2)) {
    return false;
  }
  if (input[2].IsBuiltInAtom()) {
    return false;
  }
  theCommands.CheckInputNotSameAsOutput(input, output);
  output = input[1];
  return output.AddChildOnTop(input[2]);
}

bool CalculatorFunctions::innerDiffdivDiffxToDifferentiation(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDiffdivDiffxToDifferentiation");
  if (!input.StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  bool hasArgument = false;
  bool hasExtraCF = false;
  Expression theArgument, extraCoeff;
  if (input[1] != "Differential" && input[1] != "d") {
    if (!input[1].StartsWith(theCommands.opDifferential())) {
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
  output.reset(theCommands, 2);
  output.AddChildAtomOnTop(theCommands.opDifferentiate());
  output.AddChildOnTop(input[2][1]);
  if (hasArgument) {
    output.AddChildOnTop(theArgument);
  }
  if (hasExtraCF) {
    Expression outputCopy = output;
    output = extraCoeff * outputCopy;
  }
  return true;
}

bool CalculatorFunctions::innerDdivDxToDiffDivDiffx(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDdivDxToDifferentiation");
  if (!input.StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  std::string denominatorString,numeratorString;
  if (!input[1].IsOperation(&numeratorString)) {
    return false;
  }
  if (!input[2].IsOperation(&denominatorString)) {
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
  Expression numeratorE, denominatorE(theCommands), rightDenE;
  numeratorE.MakeAtom(theCommands.opDifferential(), theCommands);
  rightDenE.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst(denominatorString), theCommands);
  denominatorE.AddChildOnTop(numeratorE);
  denominatorE.AddChildOnTop(rightDenE);
  return output.MakeXOX(theCommands, theCommands.opDivide(), numeratorE, denominatorE);
}

bool CalculatorFunctions::outerMergeConstantRadicals(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerMergeConstantRadicals");
  if (!input.StartsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  if (
    !input[1].StartsWith(theCommands.opThePower(), 3) ||
    !input[2].StartsWith(theCommands.opThePower(), 3)
  ) {
    return false;
  }
  if (input[1][2] != input[2][2]) {
    return false;
  }
  if (
    !input[1][1].IsOfType<Rational>() && !input[1][1].IsOfType<AlgebraicNumber>() &&
    !input[1][1].IsOfType<double>()
  ) {
    return false;
  }
  if (
    !input[2][1].IsOfType<Rational>() &&
    !input[2][1].IsOfType<AlgebraicNumber>() &&
    !input[2][1].IsOfType<double>()
  ) {
    return false;
  }
  Expression theProduct;
  theProduct.MakeProducT(theCommands, input[1][1], input[2][1]);
  return output.MakeXOX(theCommands, theCommands.opThePower(), theProduct, input[1][2]);
}

bool CalculatorFunctions::outerCommuteConstants(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerCommuteConstants");
  if (!input.StartsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  if (!input[2].IsConstantNumber()) {
    return false;
  }
  if (input[1].IsConstantNumber()) {
    return false;
  }
  output.MakeProducT(theCommands, input[2], input[1]);
  output.CheckInitializationRecursively();
  return true;
}

bool CalculatorFunctions::outerDivideReplaceAdivBpowerItimesBpowerJ(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::outerDivideReplaceAdivBpowerItimesBpowerJ");
  if (!input.StartsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  if (!input[1].StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  Expression denominatorBase, denominatorExponent;
  Expression numeratorBase, numeratorExponent;
  input[1][2].GetBaseExponentForm(denominatorBase, denominatorExponent);
  input[2].GetBaseExponentForm(numeratorBase, numeratorExponent);
  if (denominatorBase != numeratorBase) {
    return false;
  }
  theCommands.CheckInputNotSameAsOutput(input, output);
  Expression rightMultiplicand, rightMultiplicandExponent;
  rightMultiplicandExponent.MakeXOX(theCommands, theCommands.opMinus(), numeratorExponent, denominatorExponent);
  rightMultiplicand.MakeXOX(theCommands, theCommands.opThePower(), denominatorBase, rightMultiplicandExponent);
  return output.MakeXOX(theCommands, theCommands.opTimes(), input[1][1], rightMultiplicand);
}

bool Expression::SplitProduct(
  int numDesiredMultiplicandsLeft,
  Expression& outputLeftMultiplicand,
  Expression& outputRightMultiplicand
) const {
  MacroRegisterFunctionWithName("Expression::SplitProduct");
  if (numDesiredMultiplicandsLeft <= 0) {
    return false;
  }
  this->CheckInitialization();
  List<Expression> theMultiplicandsLeft, theMultiplicandsRight;
  this->owner->AppendOpandsReturnTrueIfOrderNonCanonical(*this, theMultiplicandsLeft, this->owner->opTimes());
  if (theMultiplicandsLeft.size <= numDesiredMultiplicandsLeft) {
    return false;
  }
  theMultiplicandsRight.setExpectedSize(theMultiplicandsLeft.size-numDesiredMultiplicandsLeft);
  for (int i = numDesiredMultiplicandsLeft; i < theMultiplicandsLeft.size; i ++) {
    theMultiplicandsRight.addOnTop(theMultiplicandsLeft[i]);
  }
  theMultiplicandsLeft.setSize(numDesiredMultiplicandsLeft);
  outputLeftMultiplicand.MakeOXdotsX(*this->owner, this->owner->opTimes(), theMultiplicandsLeft);
  return outputRightMultiplicand.MakeOXdotsX(*this->owner, this->owner->opTimes(), theMultiplicandsRight);
}

bool CalculatorFunctions::outerAtimesBpowerJplusEtcDivBpowerI(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::outerAtimesBpowerJplusEtcDivBpowerI");
  if (!input.StartsWith(theCommands.opDivide(), 3)) {
    return false;
  }
  Expression denominatorBase, denominatorExponent;
  input[2].GetBaseExponentForm(denominatorBase, denominatorExponent);
  if (!denominatorBase.DivisionByMeShouldBeWrittenInExponentForm()) {
    return false;
  }
  LinearCombination<Expression, Rational> numerators, numeratorsNew;
  theCommands.functionCollectSummands(theCommands, input[1], numerators);
  numeratorsNew.setExpectedSize(numerators.size());
  numeratorsNew.makeZero();
  Expression numeratorMultiplicandLeft, numeratorMultiplicandRight, numeratorBaseRight, numeratorExponentRight;
  Expression newNumSummand, newNumSummandRightPart, newNumExponent, mOneE;
  mOneE.AssignValue(- 1, theCommands);
  for (int i = 0; i < numerators.size(); i ++) {
    if (numerators[i].IsConstantNumber()) {
      newNumSummandRightPart.MakeXOX(
        theCommands,
        theCommands.opThePower(),
        denominatorBase,
        mOneE * denominatorExponent
      );
      newNumSummand = numerators[i] * newNumSummandRightPart;
      numeratorsNew.AddMonomial(newNumSummand, numerators.coefficients[i]);
      continue;
    }
    numerators[i].GetBaseExponentForm(numeratorBaseRight, numeratorExponentRight);
    if (numeratorBaseRight == denominatorBase) {
      newNumExponent.MakeXOX(theCommands, theCommands.opMinus(), numeratorExponentRight, denominatorExponent);
      newNumSummand.MakeXOX(theCommands, theCommands.opThePower(), denominatorBase, newNumExponent);
      numeratorsNew.AddMonomial(newNumSummand, numerators.coefficients[i]);
      continue;
    }
    bool isGood = false;
    for (int j = 1; numerators[i].SplitProduct(j, numeratorMultiplicandLeft, numeratorMultiplicandRight); j ++) {
      numeratorMultiplicandRight.GetBaseExponentForm(numeratorBaseRight, numeratorExponentRight);
      if (numeratorBaseRight != denominatorBase) {
        continue;
      }
      newNumExponent.MakeXOX(theCommands, theCommands.opMinus(), numeratorExponentRight, denominatorExponent);
      newNumSummandRightPart.MakeXOX(theCommands, theCommands.opThePower(), denominatorBase, newNumExponent);
      newNumSummand.MakeXOX(theCommands, theCommands.opTimes(), numeratorMultiplicandLeft, newNumSummandRightPart);
      numeratorsNew.AddMonomial(newNumSummand, numerators.coefficients[i]);
      isGood = true;
      break;
    }
    if (!isGood) {
      return false;
    }
  }
  return output.makeSum(theCommands, numeratorsNew);
}

bool CalculatorFunctions::innerGrowDynkinType(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGrowDynkinType");
  if (input.size() != 3) {
    return false;
  }
  const Expression& theSmallerTypeE = input[1];
  DynkinType theSmallDynkinType;
  if (!CalculatorConversions::functionDynkinType(
    theCommands, theSmallerTypeE, theSmallDynkinType
  )) {
    return false;
  }
  WithContext<SemisimpleLieAlgebra*> theSSalg;
  if (!theCommands.Convert(
    input[2], CalculatorConversions::functionSemisimpleLieAlgebra, theSSalg
  )) {
    return output.MakeError("Error extracting ambient Lie algebra.", theCommands);
  }
  SemisimpleSubalgebras tempSas;
  tempSas.initHookUpPointers(
    *theSSalg.content,
    &theCommands.theObjectContainer.theAlgebraicClosure,
    &theCommands.theObjectContainer.semisimpleLieAlgebras,
    &theCommands.theObjectContainer.theSltwoSAs
  );
  tempSas.ComputeSl2sInitOrbitsForComputationOnDemand();
  if (!tempSas.RanksAndIndicesFit(theSmallDynkinType)) {
    return output.MakeError(
      "Error: type " + theSmallDynkinType.toString() + " does not fit inside " + theSSalg.content->theWeyl.theDynkinType.toString(),
      theCommands
    );
  }
  List<DynkinType> largerTypes;
  List<List<int> > imagesSimpleRoots;
  if (!tempSas.GrowDynkinType(theSmallDynkinType, largerTypes, &imagesSimpleRoots)) {
    return output.MakeError(
      "Error: growing type " + theSmallDynkinType.toString() + " inside " + theSSalg.content->theWeyl.theDynkinType.toString() + " failed. ",
      theCommands
    );
  }
  std::stringstream out;
  out << "Inside " << theSSalg.content->theWeyl.theDynkinType.toString()
  << ", input type " << theSmallDynkinType.toString();
  if (largerTypes.size == 0) {
    out << " cannot grow any further. ";
  } else {
    CandidateSSSubalgebra tempCandidate;
    out << " can grow to the following types. <br>";
    out << "<table border =\"1\">"
    << "<td>Larger type</td>"
    << "<td>Root injection</td>"
    << "<td>Highest weight module containing new simple generator</td></tr>";
    for (int i = 0; i < largerTypes.size; i ++) {
      out << "<tr><td>" << largerTypes[i].toString() << "</td>";
      out << "<td>";
      for (int j = 0; j < imagesSimpleRoots[i].size; j ++) {
        out << "r_" << j + 1 << " -> " << "r_" << imagesSimpleRoots[i][j] + 1;
        if (j != imagesSimpleRoots[i].size) {
          out << ", ";
        }
      }
      out << "</td><td>";
      Vector<Rational> currentHighestWeight = tempSas.GetHighestWeightFundNewComponentFromImagesOldSimpleRootsAndNewRoot(
        largerTypes[i], imagesSimpleRoots[i], tempCandidate
      );
      out << HtmlRoutines::GetMathSpanPure(
        currentHighestWeight.ToStringLetterFormat("\\omega")
      );
      out << "</td></tr>";
    }
    out << "</table>";
  }
  return output.AssignValue(out.str(), theCommands);
}

void Expression::GetBlocksOfCommutativity(HashedListSpecialized<Expression>& inputOutputList) const {
  MacroRegisterFunctionWithName("Expression::GetBlocksOfCommutativity");
  this->CheckInitialization();
  if (this->IsAtom()) {
    inputOutputList.addOnTopNoRepetition(*this);
    return;
  }
  if (this->IsConstantNumber()) {
    return;
  }
  std::string whichOperation;
  if ((*this)[0].IsOperation(&whichOperation)) {
    if (this->owner->atomsThatAllowCommutingOfCompositesStartingWithThem.Contains(whichOperation)) {
      for (int i = 1; i < this->size(); i ++) {
        (*this)[i].GetBlocksOfCommutativity(inputOutputList);
      }
      return;
    }
  }
  inputOutputList.addOnTopNoRepetition(*this);
}

bool Expression::MakeMatrix(Calculator& owner, Matrix<Expression>* inputMat) {
  MacroRegisterFunctionWithName("Expression::MakeMatrix");
  if (inputMat == nullptr) {
    this->reset(owner);
    Expression theMatID(owner);
    theMatID.AddChildAtomOnTop(owner.opMatriX());
    this->AddChildOnTop(theMatID);
    return true;
  }
  return this->AssignMatrixExpressions(*inputMat, owner, true, true);
}

bool Expression::MakeSequence(Calculator& owner, List<Expression>* inputSequence) {
  MacroRegisterFunctionWithName("Expression::MakeSequence");
  this->reset(owner, inputSequence == nullptr ? 1 : inputSequence->size + 1);
  this->AddChildAtomOnTop(owner.opSequence());
  if (inputSequence != nullptr) {
    for (int i = 0; i < inputSequence->size; i ++) {
      this->AddChildOnTop((*inputSequence)[i]);
    }
  }
  return true;
}

bool Expression::MakeSequenceCommands(Calculator& owner, List<std::string>& inputKeys, List<Expression>& inputValues) {
  MacroRegisterFunctionWithName("Expression::MakeSequenceCommands");
  List<Expression> theStatements;
  Expression currentStatement, currentKey;
  if (inputValues.size != inputKeys.size) {
    global.fatal << "This is a programming error: I am asked to create a "
    << "sequence of statements but I was given different "
    << "number of keys and expressions." << global.fatal;
  }
  for (int i = 0; i < inputValues.size; i ++) {
    currentKey.MakeAtom(inputKeys[i], owner);
    currentStatement.MakeXOX(owner, owner.opDefine(), currentKey, inputValues[i]);
    theStatements.addOnTop(currentStatement);
  }
  return this->MakeSequenceStatements(owner, &theStatements);
}

bool Expression::MakeSequenceStatements(Calculator& owner, List<Expression>* inputStatements) {
  MacroRegisterFunctionWithName("Expression::MakeSequenceStatements");
  this->reset(owner, inputStatements == nullptr ? 1 : inputStatements->size + 1);
  this->AddChildAtomOnTop(owner.opEndStatement());
  if (inputStatements != nullptr) {
    for (int i = 0; i < inputStatements->size; i ++) {
      this->AddChildOnTop((*inputStatements)[i]);
    }
  }
  return true;
}

bool CalculatorFunctions::innerGetUserDefinedSubExpressions(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGetUserDefinedSubExpressions");
  if (input.size() != 2) {
    return false;
  }
  HashedListSpecialized<Expression> theList;
  input[1].GetBlocksOfCommutativity(theList);
  return output.MakeSequence(theCommands, &theList);
}

bool CalculatorFunctions::innerComputeSemisimpleSubalgebras(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerComputeSemisimpleSubalgebras");
  if (input.size() != 2) {
    return theCommands << "Semisimple subalgebras function expects 1 argument. ";
  }
  WithContext<SemisimpleLieAlgebra*> ownerSSPointer;
  if (!theCommands.Convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, ownerSSPointer
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra& ownerSS = *ownerSSPointer.content;
  std::stringstream out;
  if (ownerSS.GetRank() > 6) {
    out << "<b>This code is completely experimental and has been set to run up to rank 6. "
    << "As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
    return output.AssignValue(out.str(), theCommands);
  } else {
    out << "<b>This code is completely experimental. Use the following printouts on your own risk</b>";
  }
  SemisimpleSubalgebras& theSSsubalgebras =
  theCommands.theObjectContainer.GetSemisimpleSubalgebrasCreateIfNotPresent(ownerSS.theWeyl.theDynkinType);
  theSSsubalgebras.flagComputePairingTable = false;
  theSSsubalgebras.flagComputeNilradicals = false;
  theSSsubalgebras.FindTheSSSubalgebrasFromScratch(
    ownerSS,
    theCommands.theObjectContainer.theAlgebraicClosure,
    theCommands.theObjectContainer.semisimpleLieAlgebras,
    theCommands.theObjectContainer.theSltwoSAs,
    nullptr
  );
  return output.AssignValue(theSSsubalgebras, theCommands);
}

bool CalculatorFunctions::innerLispify(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerLispify");
  if (input.size() != 2) {
    return false;
  }
  return output.AssignValue(input[1].ToStringSemiFull(), theCommands);
}

bool CalculatorFunctions::innerLispifyFull(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerLispifyFull");
  if (input.size() != 2) {
    return false;
  }
  return output.AssignValue(input[1].ToStringFull(), theCommands);
}

bool CalculatorFunctions::innerMinPolyMatrix(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerMinPolyMatrix");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (!CalculatorConversions::functionMatrixRational(
    theCommands, argument, output
  )) {
    return false;
  }
  Matrix<Rational> theMat;
  if (!theCommands.functionGetMatrix(argument, theMat)) {
    return theCommands
    << "<hr>Minimal poly computation: could not convert "
    << argument.toString() << " to rational matrix.";
  }
  if (theMat.NumRows != theMat.NumCols || theMat.NumRows <= 0) {
    return output.MakeError("Error: matrix is not square.", theCommands);
  }
  FormatExpressions tempF;
  tempF.polyAlphabeT.setSize(1);
  tempF.polyAlphabeT[0] = "q";
  Polynomial<Rational> theMinPoly;
  theMinPoly.AssignMinPoly(theMat);
  return output.AssignValue(theMinPoly.toString(&tempF), theCommands);
}

bool CalculatorFunctions::innerCharPolyMatrix(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerMinPolyMatrix");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Matrix<Rational> theMat;
  if (!theCommands.functionGetMatrix(argument, theMat)) {
    return theCommands
    << "<hr>Characteristic poly computation: could not convert "
    << input.toString() << " to rational matrix.";
  }
  if (theMat.NumRows != theMat.NumCols || theMat.NumRows <= 0) {
    return output.MakeError("Error: matrix is not square.", theCommands);
  }
  FormatExpressions tempF;
  tempF.polyAlphabeT.setSize(1);
  tempF.polyAlphabeT[0] = "q";
  Polynomial<Rational> theCharPoly;
  theCharPoly.AssignCharPoly(theMat);
  return output.AssignValue(theCharPoly.toString(&tempF), theCommands);
}

bool CalculatorFunctions::innerReverseBytes(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerReverseBytes");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].IsOfType(&inputString)) {
    return false;
  }
  std::string result;
  result.resize(inputString.size());
  for (unsigned i = 0; i < inputString.size(); i ++) {
    result[i] = inputString[inputString.size() - i - 1];
  }
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctions::innerTrace(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTrace");
  if (input.size() != 2) {
    return false;
  }
  Matrix<Rational> theMat;
  if (theCommands.functionGetMatrix(input[1], theMat)) {
    if (!theMat.IsSquare()) {
      return output.MakeError(
        "Error: attempting to get trace of non-square matrix. ",
        theCommands
      );
    }
    return output.AssignValue(theMat.GetTrace(), theCommands);
  }
  Matrix<RationalFunction> theMatRF;
  if (theCommands.functionGetMatrix(input[1], theMatRF)) {
    if (!theMatRF.IsSquare()) {
      return output.MakeError(
        "Error: attempting to get trace of non-square matrix. ",
        theCommands
      );
    }
    return output.AssignValue(theMatRF.GetTrace(), theCommands);
  }
  Matrix<Expression> theMatExp;
  if (!theCommands.GetMatrixExpressionsFromArguments(input[1], theMatExp)) {
    return false;
  }
  if (!theMat.IsSquare()) {
    return output.MakeError(
      "Error: attempting to get trace of non-square matrix. ",
      theCommands
    );
  }
  if (theMat.NumRows == 1) {
    theCommands << "Requested trace of 1x1 matrix: possible "
    << "interpretation of a scalar as a 1x1 matrix. Trace not taken. ";
    return false;
  }
  output = theMat.GetTrace();
  return true;
}

bool CalculatorFunctions::innerContains(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerContains");
  if (input.size() != 3) {
    return false;
  }
  if (input[1].ContainsAsSubExpressionNoBuiltInTypes(input[2])) {
    return output.AssignValue(1, theCommands);
  } else {
    return output.AssignValue(0, theCommands);
  }
}

bool CalculatorFunctions::innerExpressionLeafs(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerExpressionLeafs");
  HashedList<Expression> theLeafs;
  if (input.StartsWithGivenOperation("ExpressionLeafs")) {
    for (int i = 1; i < input.size(); i ++) {
      if (!input[i].GetExpressionLeafs(theLeafs)) {
        return theCommands << "Failed to extract expression leafs from " << input.toString() << ".";
      }
    }
  } else if (!input.GetExpressionLeafs(theLeafs)) {
    return theCommands << "Failed to extract expression leafs from " << input.toString() << ".";
  }
  return output.MakeSequence(theCommands, &theLeafs);
}

bool CalculatorFunctions::innerLastElement(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerLastElement");
  if (input.HasBoundVariables()) {
    return false;
  }
  if (input.IsAtom()) {
    std::stringstream out;
    out << "Error: requesting the last element of the atom " << input.toString();
    return output.MakeError(out.str(), theCommands);
  }
  if (input.size() == 2) {
    const Expression& sequenceCandidate = input[1];
    if (sequenceCandidate.IsSequenceNElementS()) {
      output = sequenceCandidate[sequenceCandidate.size() - 1];
      return true;
    }
  }
  output = input[input.size() - 1];
  return true;
}

bool CalculatorFunctions::innerRemoveLastElement(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerRemoveLastElement");
  if (input.HasBoundVariables()) {
    return false;
  }
  if (input.IsAtom() || input.size() == 1) {
    std::stringstream out;
    if (input.IsAtom()) {
      out << "Error: requesting to remove the last element of the atom " << input.toString() << ". ";
    } else {
      out << "Error: requesting to remove the last element of the zero-element list " << input.toString() << ". ";
    }
    return output.MakeError(out.str(), theCommands);
  }
  if (input.size() == 2) {
    const Expression& sequenceCandidate = input[1];
    if (sequenceCandidate.IsSequenceNElementS() && sequenceCandidate.size() > 1) {
      output = sequenceCandidate;
      output.children.RemoveLastObject();
      return true;
    }
  }
  output = input;
  output.children.RemoveLastObject();
  return output.SetChildAtomValue(0, theCommands.opSequence());
}

bool ElementZmodP::operator==(int other) const {
  this->CheckIamInitialized();
  ElementZmodP tempElt;
  tempElt.theModulus = this->theModulus;
  tempElt = LargeInteger(other);
  return *this == tempElt;
}

bool ElementZmodP::operator==(const ElementZmodP& other) const {
  this->CheckIamInitialized();
  other.CheckIamInitialized();
  return this->theModulus == other.theModulus && this->theValue == other.theValue;
}

void ElementZmodP::operator*=(const ElementZmodP& other) {
  if (this == &other) {
    ElementZmodP other = *this;
    *this *= other;
    return;
  }
  this->CheckEqualModuli(other);
  this->theValue *= other.theValue;
  this->theValue %= this->theModulus;
}

bool CalculatorFunctions::innerIsPossiblyPrime(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsPossiblyPrime");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger theInt;
  if (!input[1].IsInteger(&theInt)) {
    return false;
  }
  bool resultBool = theInt.value.IsPossiblyPrime(10, true, &theCommands.Comments);
  Rational result = 1;
  if (!resultBool) {
    result = 0;
  }
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctions::innerIsPrimeMillerRabin(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsPrimeMillerRabin");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger theInt;
  if (!input[1].IsInteger(&theInt)) {
    return false;
  }
  bool resultBool = theInt.value.IsPossiblyPrimeMillerRabiN(10, &theCommands.Comments);
  Rational result = 1;
  if (!resultBool) {
    result = 0;
  }
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctions::innerIsNilpotent(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsNilpotent");
  Expression converted;
  if (!CalculatorConversions::innerMakeMatrix(theCommands, input, converted)) {
    return false;
  }
  bool found = false;
  Matrix<Rational> theMat;
  MatrixTensor<Rational> theMatTensor;
  if (theCommands.functionGetMatrix(converted, theMat)) {
    found = true;
    theMatTensor = theMat;
  } else if (input.IsOfType<MatrixTensor<Rational> >(&theMatTensor)) {
    found = true;
  }
  if (!found) {
    return output.MakeError(
      "Failed to extract matrix with rational coefficients", theCommands
    );
  }
  if (theMatTensor.IsNilpotent()) {
    return output.AssignValue(1, theCommands);
  }
  return output.AssignValue(0, theCommands);
}

bool CalculatorFunctions::innerInvertMatrix(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerInvertMatrix");
  Matrix<Rational> theMat;
  Expression converted;
  if (!CalculatorConversions::innerMakeMatrix(
    theCommands, input, converted
  )) {
    return theCommands << "Failed to extract matrix from input. ";
  }
  if (theCommands.functionGetMatrix(converted, theMat)) {
    if (theMat.NumRows != theMat.NumCols || theMat.NumCols < 1) {
      return output.MakeError("The matrix is not square", theCommands);
    }
    if (theMat.GetDeterminant() == 0) {
      return output.MakeError("Matrix determinant is zero.", theCommands);
    }
    theMat.Invert();
    return output.AssignMatrix(theMat, theCommands);
  }
  Matrix<AlgebraicNumber> theMatAlg;
  if (theCommands.functionGetMatrix(input, theMatAlg)) {
    return theCommands << "<hr>Failed to extract algebraic number matrix from: "
    << input.toString();
  }
  if (theMatAlg.NumRows != theMatAlg.NumCols || theMatAlg.NumCols < 1) {
    return output.MakeError("The matrix is not square", theCommands);
  }
  if (theMatAlg.GetDeterminant() == 0) {
    return output.MakeError("Matrix determinant is zero.", theCommands);
  }
  theMatAlg.Invert();
  return output.AssignMatrix(theMatAlg, theCommands);
}

bool CalculatorFunctions::innerDFQsEulersMethod(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDFQsEulersMethod");
  if (input.size() != 7) {
    return theCommands << "Euler method function takes 6 arguments";
  }
  double xInitial, yInitial, leftEndpoint, rightEndpoint;
  int numPoints;
  if (!input[2].EvaluatesToDouble(&xInitial) || !input[3].EvaluatesToDouble(&yInitial)) {
    return theCommands << "Failed to extract initial x,y values from " << input.toString();
  }
  if (!input[4].IsSmallInteger(&numPoints)) {
    return theCommands << "Failed to extract number of points from " << input.toString();
  }
  if (!input[5].EvaluatesToDouble(&leftEndpoint) || !input[6].EvaluatesToDouble(&rightEndpoint)) {
    return theCommands << "Failed to extract left and right endpoints from " << input.toString();
  }
  if (leftEndpoint > rightEndpoint) {
    MathRoutines::swap(leftEndpoint, rightEndpoint);
  }
  if (xInitial > rightEndpoint || xInitial < leftEndpoint) {
    return theCommands << "The initial value for x, " << xInitial
    << ", does not lie between the left and right endpoints "
    << leftEndpoint << ", " << rightEndpoint << ". I am aborting Euler's method. ";
  }
  HashedList<Expression> knownConsts;
  List<double> knownValues;
  knownConsts.addOnTop(theCommands.knownDoubleConstants);
  knownValues.addListOnTop(theCommands.knownDoubleConstantValues);
  Expression xE, yE;
  xE.MakeAtom("x", theCommands);
  yE.MakeAtom("y", theCommands);
  if (knownConsts.Contains(xE) || knownConsts.Contains(yE)) {
    return theCommands << "The letters x, y appear to be already used to "
    << "denote known constants, I cannot run Euler's method.";
  }
  knownConsts.addOnTop(xE);
  knownConsts.addOnTop(yE);
  knownValues.addOnTop(0);
  knownValues.addOnTop(0);
  if (numPoints < 2) {
    return theCommands << "The number of points for Euler's method is " << numPoints << ", too few. ";
  }
  if (numPoints > 10001) {
    return theCommands << "The number of points for Euler's method is " << numPoints
    << ", I am not allowed to handle that many. ";
  }
  if (leftEndpoint - rightEndpoint == 0.0) {
    return theCommands << "Whlie doing Euler's method: right endpoint equals left! ";
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
  XValues.ReverseOrderElements();
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
        if (!functionE.EvaluatesToDoubleUnderSubstitutions(knownConsts, knownValues, &currentYprimeApprox)) {
          return theCommands << "Failed to evaluate yPrime approximation at x =" << XValues[i];
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
          theCommands << "Euler method: no convergence. At x =" << XValues[i] << ", y ="
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
          theCommands << "Euler method: y values outside of the bounding box. At x =" << XValues[i]
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
  thePlot.xHigh = *XValues.LastObject();
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
    thePlot.yLow = MathRoutines::Minimum(YValues[i], thePlot.yLow);
    thePlot.yHigh = MathRoutines::Maximum(YValues[i], thePlot.yHigh);
  }
  thePlot.thePlotString = outLatex.str();
  thePlot.thePlotStringWithHtml = outHtml.str();
  return output.AssignValue(thePlot, theCommands);
}

bool CalculatorFunctions::innerPlotViewWindow(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotViewWindow");
  if (input.size() < 3) {
    return false;
  }
  Vector<double> widthHeight;
  Plot emptyPlot;
  emptyPlot.priorityWindow = 1;
  bool isGood = false;
  if (theCommands.GetVectorDoublesFromFunctionArguments(input, widthHeight, 2)) {
    isGood = true;
  } else {
    widthHeight.setSize(2);
    widthHeight[0] = 100;
    widthHeight[1] = 100;
    MapList<std::string, Expression, MathRoutines::HashString> theMap;
    if (!CalculatorConversions::innerLoadKeysFromStatementLisT(
      theCommands, input, theMap, nullptr, false
    )) {
      isGood = false;
    } else {
      if (theMap.Contains("width")) {
        if (!theMap.GetValueCreateNoInit("width").EvaluatesToDouble(&widthHeight[0])) {
          isGood = false;
        }
      }
      if (theMap.Contains("height")) {
        if (!theMap.GetValueCreateNoInit("height").EvaluatesToDouble(&widthHeight[1])) {
          isGood = false;
        }
      }
    }
  }
  if (!isGood) {
    return theCommands << "Failed to extract a pair of positive numbers from: "
    << input.toString();
  }
  emptyPlot.DesiredHtmlWidthInPixels = static_cast<int>(widthHeight[0]);
  emptyPlot.DesiredHtmlHeightInPixels = static_cast<int>(widthHeight[1]);
  return output.AssignValue(emptyPlot, theCommands);
}

bool CalculatorFunctions::innerPlotSetId(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotSetId");
  Plot emptyPlot;
  emptyPlot.priorityCanvasName = 1;
  emptyPlot.DesiredHtmlHeightInPixels = 100;
  emptyPlot.DesiredHtmlWidthInPixels = 100;
  std::string incomingName;
  if (!input.IsOfType(&incomingName)) {
   incomingName = input.toString();
  }
  emptyPlot.SetCanvasName(incomingName);
  return output.AssignValue(emptyPlot, theCommands);
}

bool CalculatorFunctions::innerPlotViewRectangle(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotViewRectangle");
  if (input.size() < 3) {
    return false;
  }
  Vector<double> lowerLeft, upperRight;
  if (
    !theCommands.GetVectorDoubles(input[1], lowerLeft, 2) ||
    !theCommands.GetVectorDoubles(input[2], upperRight, 2)
  ) {
    return theCommands << "Failed to extract two pairs of floating point numbers from: "
    << input[1].toString() << " and " << input[2].toString();
  }
  Plot emptyPlot;
  emptyPlot.dimension = 2;
  emptyPlot.theLowerBoundAxes = lowerLeft[0];
  emptyPlot.lowBoundY = lowerLeft[1];
  emptyPlot.theUpperBoundAxes = upperRight[0];
  emptyPlot.highBoundY = upperRight[1];
  emptyPlot.priorityViewRectangle = 1;
  emptyPlot.DesiredHtmlHeightInPixels = 100;
  emptyPlot.DesiredHtmlWidthInPixels = 100;
  return output.AssignValue(emptyPlot, theCommands);
}

bool CalculatorFunctions::innerPlotFill(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotFill");
  if (input.size() < 3) {
    return false;
  }
  const Expression& thePlotE = input[1];
  const Expression& colorE = input[2];
  Plot outputPlot, startPlot;
  outputPlot.dimension = 2;
  PlotObject theFilledPlot;
  if (!thePlotE.IsOfType<Plot>(&startPlot)) {
    return false;
  }
  std::string colorString;
  if (!colorE.IsOfType<std::string>(&colorString)) {
    colorString = colorE.toString();
  }
  if (!DrawingVariables::GetColorIntFromColorString(colorString, theFilledPlot.colorFillRGB)) {
    theCommands << "Failed to extract color from: " << colorE.toString() << "; using default color value. ";
  }
  theFilledPlot.colorFillJS = colorString;
  for (int i = 0; i < startPlot.thePlots.size; i ++) {
    theFilledPlot.thePointsDouble.addListOnTop(startPlot.thePlots[i].thePointsDouble);
  }
  theFilledPlot.fillStyle = "filled";
  theFilledPlot.thePlotType = "plotFillStart";
  outputPlot.DesiredHtmlHeightInPixels = startPlot.DesiredHtmlHeightInPixels;
  outputPlot.DesiredHtmlWidthInPixels = startPlot.DesiredHtmlWidthInPixels;
  outputPlot.thePlots.addOnTop(theFilledPlot);
  outputPlot += startPlot;
  theFilledPlot.thePlotType = "plotFillFinish";
  outputPlot.thePlots.addOnTop(theFilledPlot);
  return output.AssignValue(outputPlot, theCommands);
}

bool CalculatorFunctions::innerIsPlot(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsPlot");
  if (input.size() != 2) {
    return false;
  }
  if (input[1].IsOfType<Plot>()) {
    return output.AssignValue(1, theCommands);
  }
  return output.AssignValue(0, theCommands);
}

bool CalculatorFunctions::innerPlot2DoverIntervals(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlot2DoverIntervals");
  if (input.size() < 3) {
    return false;
  }
  List<Expression> theIntervalCandidates;
  if (!theCommands.CollectOpands(input[2], theCommands.opUnion(),theIntervalCandidates)) {
    return false;
  }
  if (theIntervalCandidates.size < 1) {
    return false;
  }
  for (int i = 0; i < theIntervalCandidates.size; i ++) {
    if (!theIntervalCandidates[i].IsIntervalRealLine()) {
      return false;
    }
  }
  Expression summandE;
  List<Expression> finalSummands;
  for (int i = 0; i < theIntervalCandidates.size; i ++) {
    summandE.reset(theCommands);
    summandE.AddChildOnTop(input[0]);
    summandE.AddChildOnTop(input[1]);
    summandE.AddChildOnTop(theIntervalCandidates[i][1]);
    summandE.AddChildOnTop(theIntervalCandidates[i][2]);
    for (int j = 3; j < input.size(); j ++) {
      summandE.AddChildOnTop(input[j]);
    }
    finalSummands.addOnTop(summandE);
  }
  return output.makeSum(theCommands, finalSummands);
}

bool CalculatorFunctions::innerPlot2D(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlot2D");
  if (input.size() < 4) {
    return output.MakeError(
      "Plotting coordinates takes at least three arguments: function, lower and upper bound. ",
      theCommands
    );
  }
  if (input.HasBoundVariables()) {
    return false;
  }
  Plot thePlot;
  thePlot.dimension = 2;
  PlotObject thePlotObj;
  thePlotObj.leftPtE = input[2];
  thePlotObj.rightPtE = input[3];
  if (input.size() >= 5) {
    if (!input[4].IsOfType<std::string>(&thePlotObj.colorJS)) {
      thePlotObj.colorJS = input[4].toString();
    }
  } else {
    thePlotObj.colorJS = "red";
  }
  thePlotObj.colorRGB = static_cast<int>(HtmlRoutines::RedGreenBlue(255, 0, 0));
  DrawingVariables::GetColorIntFromColorString(thePlotObj.colorJS, thePlotObj.colorRGB);
  thePlotObj.lineWidth = 1;
  if (input.size() >= 6) {
    input[5].EvaluatesToDouble(&thePlotObj.lineWidth);
  }
  if (input.size() >= 7) {
    thePlotObj.numSegmentsE = input[6];
  } else {
    thePlotObj.numSegmentsE.AssignValue(500, theCommands);
  }
  int numIntervals = - 1;
  if (!thePlotObj.numSegmentsE.IsSmallInteger(&numIntervals)) {
    numIntervals = 500;
  }
  if (numIntervals < 2) {
    numIntervals = 2;
  }
  bool leftIsDouble = thePlotObj.leftPtE.EvaluatesToDouble(&thePlotObj.xLow);
  bool rightIsDouble = thePlotObj.rightPtE.EvaluatesToDouble(&thePlotObj.xHigh);
  if (!leftIsDouble) {
    if (thePlotObj.leftPtE != theCommands.EMInfinity()) {
      return theCommands
      << "Couldn't convert left boundary "
      << thePlotObj.leftPtE.toString() << " to floating point number. ";
    } else {
      thePlotObj.leftBoundaryIsMinusInfinity = true;
    }
  }
  if (!rightIsDouble) {
    if (thePlotObj.rightPtE != theCommands.EInfinity()) {
      return theCommands
      << "Couldn't convert right boundary "
      << thePlotObj.rightPtE.toString() << " to floating point number. ";
    } else {
      thePlotObj.rightBoundaryIsMinusInfinity = true;
    }
  }
  thePlotObj.coordinateFunctionsE.addOnTop(input[1]);
  thePlotObj.coordinateFunctionsJS.setSize(1);
  thePlotObj.coordinateFunctionsE[0].GetFreeVariables(thePlotObj.variablesInPlay, true);
  if (thePlotObj.variablesInPlay.size > 1) {
    return theCommands << "Got a function with "
    << thePlotObj.variablesInPlay.size
    << " variables, namely: "
    << thePlotObj.variablesInPlay.ToStringCommaDelimited()
    << ". I was expecting a single variable. ";
  }
  if (thePlotObj.variablesInPlay.size == 0) {
    Expression xE;
    xE.MakeAtom("x", theCommands);
    thePlotObj.variablesInPlay.addOnTop(xE);
  }
  thePlotObj.variablesInPlayJS.setSize(thePlotObj.variablesInPlay.size);
  thePlotObj.variablesInPlayJS[0] = thePlotObj.variablesInPlay[0].toString();
  std::string theVarString = thePlotObj.variablesInPlayJS[0];
  Expression jsConverterE;
  thePlotObj.thePlotType = "plotFunction";
  if (CalculatorFunctions::functionMakeJavascriptExpression(
    theCommands, thePlotObj.coordinateFunctionsE[0], jsConverterE
  )) {
    thePlotObj.coordinateFunctionsJS[0] = jsConverterE.toString();
    thePlotObj.coordinateFunctionsE[0].HasInputBoxVariables(&thePlot.boxesThatUpdateMe);
  } else {
    thePlotObj.thePlotType = "plotFunctionPrecomputed";
  }
  if (CalculatorFunctions::functionMakeJavascriptExpression(
    theCommands, thePlotObj.leftPtE, jsConverterE
  )) {
    thePlotObj.leftPtJS = jsConverterE.toString();
    thePlotObj.leftPtE.HasInputBoxVariables(&thePlot.boxesThatUpdateMe);
  } else {
    thePlotObj.thePlotType = "plotFunctionPrecomputed";
  }
  if (CalculatorFunctions::functionMakeJavascriptExpression(
    theCommands, thePlotObj.rightPtE, jsConverterE
  )) {
    thePlotObj.rightPtJS = jsConverterE.toString();
    thePlotObj.rightPtE.HasInputBoxVariables(&thePlot.boxesThatUpdateMe);
  } else {
    thePlotObj.thePlotType = "plotFunctionPrecomputed";
  }
  thePlotObj.numSegmenTsJS.setSize(1);
  thePlotObj.numSegmenTsJS[0] = "200";
  if (CalculatorFunctions::functionMakeJavascriptExpression(
    theCommands, thePlotObj.numSegmentsE, jsConverterE
  )) {
    thePlotObj.numSegmenTsJS[0] = jsConverterE.toString();
    thePlotObj.numSegmentsE.HasInputBoxVariables(&thePlot.boxesThatUpdateMe);
  } else {
    thePlotObj.thePlotType = "plotFunctionPrecomputed";
  }
  Vectors<double>& thePointsDouble = thePlotObj.thePointsDouble;
  if (thePlot.boxesThatUpdateMe.size == 0) {
    if (!input[1].EvaluatesToDoubleInRange(
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
        return theCommands << "<hr>I failed to evaluate the input function at all points, "
        << "perhaps your expression is not a function of x.";
      }
      theCommands << "<hr>I failed to evaluate your function in a number of points. ";
    }
  }
  Expression functionSuffixNotationE;
  if (!theCommands.CallCalculatorFunction(
    theCommands.innerSuffixNotationForPostScript, input[1], functionSuffixNotationE
  )) {
    theCommands << "No LaTeX version: failed to convert: "
    << input[1].toString() << " to postfix notation. ";
  } else {
    thePlotObj.thePlotString = thePlotObj.
    GetPlotStringFromFunctionStringAndRanges(
      false,
      functionSuffixNotationE.toString(),
      thePlotObj.coordinateFunctionsE[0].toString(),
      thePlotObj.xLow,
      thePlotObj.xHigh
    );
    thePlotObj.thePlotStringWithHtml = thePlotObj.thePlotString;
  }
  thePlot.thePlots.addOnTop(thePlotObj);
  return output.AssignValue(thePlot, theCommands);
}

bool CalculatorFunctions::innerPlotPoint(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotPoint");
  if (input.size() != 3) {
    return output.MakeError(
      "Plotting a point takes at least two arguments, location and color. ",
      theCommands
    );
  }
  Plot theFinalPlot;
  PlotObject thePlot;
  if (!theCommands.GetMatrixExpressions(input[1], thePlot.thePointS)) {
    return theCommands << "The first argument of PlotPoint is "
    << "expected to be a sequence, instead I had: " << input[1].toString();
  }
  theFinalPlot.dimension = thePlot.thePointS.NumCols;
  thePlot.dimension = theFinalPlot.dimension;
  thePlot.coordinateFunctionsE.setSize(thePlot.dimension);
  thePlot.coordinateFunctionsJS.setSize(thePlot.dimension);
  Expression jsConverterE;
  thePlot.thePointsJS.init(thePlot.thePointS.NumRows, thePlot.thePointS.NumCols);
  for (int i = 0; i < thePlot.thePointS.NumRows; i ++) {
    for (int j = 0; j < thePlot.thePointS.NumCols; j ++) {
      if (!CalculatorFunctions::functionMakeJavascriptExpression(
        theCommands, thePlot.thePointS(i, j), jsConverterE
      )) {
        return theCommands << "Failed to extract coordinate " << i + 1 << " from: "
        << thePlot.coordinateFunctionsE[i].toString();
      }
      thePlot.thePointsJS(i, j) = jsConverterE.toString();
      thePlot.thePointS(i, j).HasInputBoxVariables(&theFinalPlot.boxesThatUpdateMe);
    }
  }
  thePlot.dimension = theFinalPlot.dimension;
  thePlot.colorRGB =static_cast<int>(HtmlRoutines::RedGreenBlue(0, 0, 0));
  if (input[2].IsOfType<std::string>()) {
    DrawingVariables::GetColorIntFromColorString(input[2].GetValue<std::string>(), thePlot.colorRGB);
  }
  thePlot.colorJS = input[2].toString();
  thePlot.thePlotType = "points";
  theFinalPlot.thePlots.addOnTop(thePlot);
  theFinalPlot.DesiredHtmlHeightInPixels = 100;
  theFinalPlot.DesiredHtmlWidthInPixels = 100;
  return output.AssignValue(theFinalPlot, theCommands);
}

bool CalculatorFunctions::innerPlot2DWithBars(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlot2DWithBars");
  if (input.size() < 6) {
    return output.MakeError(
      "Plotting coordinates takes the following arguments: lower function, "
      "upper function, lower and upper bound, delta x. ",
      theCommands
    );
  }
  Expression lowerEplot = input, upperEplot = input;
  lowerEplot.children.RemoveIndexShiftDown(2);
  upperEplot.children.RemoveIndexShiftDown(1);
  Plot outputPlot;
  outputPlot.dimension = 2;
  bool tempB = CalculatorFunctions::innerPlot2D(theCommands, lowerEplot, output);
  if (!tempB || !output.IsOfType<Plot>(&outputPlot)) {
    return theCommands << "<hr>Failed to get a plot from "
    << lowerEplot.toString() << ", not proceding with bar plot.";
  }
  tempB = CalculatorFunctions::innerPlot2D(theCommands, upperEplot, output);
  if (!tempB || !output.IsOfType<Plot>()) {
    return theCommands << "<hr>Failed to get a plot from "
    << upperEplot.toString() << ", not proceding with bar plot.";
  }
  outputPlot += output.GetValue<Plot>();
  const Expression& lowerFunctionE = input[1];
  const Expression& upperFunctionE = input[2];
  const Expression& lowerE = input[3];
  const Expression& upperE = input[4];
  const Expression& deltaE = input[5];
  double theDeltaNoSign, theDeltaWithSign;
  if (!deltaE.EvaluatesToDouble(&theDeltaWithSign)) {
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
  if (!lowerE.EvaluatesToDouble(&lowerBound) || !upperE.EvaluatesToDouble(&upperBound)) {
    return theCommands
    << "Couldn't convert bounds of drawing function to floating point numbers. ";
  }
  if (upperBound < lowerBound) {
    MathRoutines::swap(upperBound, lowerBound);
  }
  Expression xValueE, xExpression, theFunValueEnonEvaluated, theFunValueFinal;
  xExpression.MakeAtom(theCommands.AddOperationNoRepetitionOrReturnIndexFirst("x"), theCommands);
  List<double> xValues;
  List<double> fValuesLower;
  List<double> fValuesUpper;
  if (theDeltaNoSign == 0.0) {
    return output.MakeError("Delta equal to zero is not allowed", theCommands);
  }
  if ((upperBound - lowerBound) / theDeltaNoSign > 10000) {
    return output.MakeError("More than 10000 intervals needed for the plot, this is not allowed.", theCommands);
  }
  List<Rational> rValues;
  Rational lowerBoundRat, upperBoundRat, deltaRat;
  if (
    lowerE.IsOfType<Rational>(&lowerBoundRat) &&
    upperE.IsOfType<Rational>(&upperBoundRat) &&
    deltaE.IsOfType<Rational>(&deltaRat)
  ) {
    if (upperBoundRat<lowerBoundRat) {
      MathRoutines::swap(upperBoundRat, lowerBoundRat);
    }
    if (deltaRat < 0) {
      deltaRat *= - 1;
    }
    if (deltaRat == 0) {
      return output.MakeError("Delta equal to zero is not allowed", theCommands);
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
      xValueE.AssignValue(i, theCommands);
      theFunValueEnonEvaluated = (j == 0) ? lowerFunctionE : upperFunctionE;
      theFunValueEnonEvaluated.SubstituteRecursively(xExpression, xValueE);
      if (!theCommands.EvaluateExpression(theCommands, theFunValueEnonEvaluated, theFunValueFinal)) {
        return false;
      }
      double finalResultDouble;
      if (!theFunValueFinal.EvaluatesToDouble(&finalResultDouble)) {
        return theCommands << "<hr>Failed to evaluate your function at point " << i
        << ", instead " << "I evaluated to " << theFunValueFinal.toString();
      }
      if (j == 0) {
        xValues.addOnTop(i);
        fValuesLower.addOnTop(finalResultDouble);
      } else {
        fValuesUpper.addOnTop(finalResultDouble);
      }
      yMin = MathRoutines::Minimum(yMin, finalResultDouble);
      yMax = MathRoutines::Maximum(yMax, finalResultDouble);
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
    tempStream << "\\rput[t](" << std::fixed << rValues[i].GetDoubleValue() << ",-0.03)" << "{$";
    if (rValues[i].IsInteger()) {
      tempStream << rValues[i].toString();
    } else {
      tempStream << "\\frac{" << rValues[i].GetNumerator().toString() << "}"
      << "{" << rValues[i].GetDenominator().toString() << "}";
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
  return output.AssignValue(plotFinal, theCommands);
}

bool CalculatorFunctions::innerPlotPolarRfunctionTheta(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotPolarRfunctionTheta");
  if (input.size() < 4) {
    return output.MakeError(
      "Drawing polar coordinates takes at least three arguments: "
      "function, lower angle bound and upper angle bound. ",
      theCommands
    );
  }
  if (input.size() < 4) {
    return theCommands
    << "Parametric curve plots take 3+ arguments. The first argument gives "
    << "the coordinate functions in the format (f_1, f_2) or (f_1, f_2,f_3), "
    << " the next two arguments stands for the variable range.";
  }
  if (input.HasBoundVariables()) {
    return false;
  }
  const Expression& polarE = input[1];
  HashedList<Expression> theVars;
  if (!polarE.GetFreeVariables(theVars, true)) {
    return false;
  }
  if (theVars.size > 1) {
    return theCommands << "Polar radius must depend on a single variable. ";
  }
  if (theVars.size == 0) {
    theVars.addOnTop(theCommands.GetNewAtom());
  }
  Expression theSine(theCommands), theCosine(theCommands);
  theSine.AddChildAtomOnTop(theCommands.opSin());
  theCosine.AddChildAtomOnTop(theCommands.opCos());
  theSine.AddChildOnTop(theVars[0]);
  theCosine.AddChildOnTop(theVars[0]);
  Expression theX = theCosine * polarE;
  Expression theY = theSine * polarE;
  Expression newArg;
  newArg.MakeSequence(theCommands);
  newArg.AddChildOnTop(theX);
  newArg.AddChildOnTop(theY);
  output.reset(theCommands);
  output.AddChildAtomOnTop("PlotCurve");
  output.AddChildOnTop(newArg);
  for (int i = 2; i < input.size(); i ++) {
    output.AddChildOnTop(input[i]);
  }
  return true;
}

bool CalculatorFunctions::innerPlotPolarRfunctionThetaExtended(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotPolarRfunctionThetaExtended");
  if (input.size() < 4) {
    return output.MakeError(
      "Drawing polar coordinates takes three arguments: "
      "function, lower angle bound and upper angle bound. ",
      theCommands
    );
  }
  Expression plotXYE, plotRthetaE;
  if (!theCommands.CallCalculatorFunction(CalculatorFunctions::innerPlotPolarRfunctionTheta, input, plotXYE)) {
    return false;
  }
  if (!theCommands.CallCalculatorFunction(CalculatorFunctions::innerPlot2D, input, plotRthetaE)) {
    return false;
  }
  return output.MakeXOX(theCommands, theCommands.opSequence(), plotXYE, plotRthetaE);
}

bool CalculatorFunctions::innerPlotParametricCurve(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotParametricCurve");
  if (input.size() < 4) {
    return theCommands
    << "Parametric curve plots take 3+ arguments. The first argument gives "
    << "the coordinate functions in the format (f_1, f_2) or (f_1, f_2,f_3), "
    << " the next two arguments stands for the variable range. ";
  }
  if (input.HasBoundVariables()) {
    return false;
  }
  PlotObject thePlot;
  if (!input[1].IsSequenceNElementS()) {
    return theCommands
    << "The first argument of parametric curve must be a sequence, instead I got: "
    << input[1].toString();
  }
  thePlot.dimension = input[1].size() - 1;
  for (int i = 0; i < thePlot.dimension; i ++) {
    thePlot.coordinateFunctionsE.addOnTop(input[1][i + 1]);
    thePlot.coordinateFunctionsE[i].GetFreeVariables(thePlot.variablesInPlay, true);
  }
  if (thePlot.variablesInPlay.size > 1) {
    return theCommands << "Curve is allowed to depend on at most 1 parameter. "
    << "Instead, your curve: " << input.toString()
    << " depends on "
    << thePlot.variablesInPlay.size << ", namely: "
    << thePlot.variablesInPlay.ToStringCommaDelimited() << ". ";
  }
  if (thePlot.variablesInPlay.size == 0) {
    Expression tempE;
    tempE.MakeAtom("t", theCommands);
    thePlot.variablesInPlay.addOnTop(tempE);
  }
  thePlot.variablesInPlayJS.addOnTop(
    HtmlRoutines::GetJavascriptVariable(thePlot.variablesInPlay[0].toString())
  );
  thePlot.colorJS = "red";
  thePlot.colorRGB = static_cast<int>(HtmlRoutines::RedGreenBlue(255, 0, 0));
  if (input.size() >= 5) {
    if (!input[4].IsOfType<std::string>(&thePlot.colorJS)) {
      thePlot.colorJS = input[4].toString();
    }
  }
  DrawingVariables::GetColorIntFromColorString(thePlot.colorJS, thePlot.colorRGB);
  thePlot.lineWidth = 1;
  if (input.size() >= 6) {
    if (!input[5].EvaluatesToDouble(&thePlot.lineWidth)) {
      thePlot.lineWidth = 1;
    }
  }
  int numPoints = 1000;
  if (input.size() >= 7) {
    if (!input[6].IsSmallInteger(&numPoints)) {
      numPoints = 1000;
      theCommands << "<hr>Could not extract number of points from "
      << input[6].toString();
    }
    thePlot.numSegmentsE = input[6];
  }
  if (numPoints < 2 || numPoints > 30000) {
    numPoints = 1000;
    theCommands << "<hr>Extracted " << numPoints
    << " point but that is not valid. Changing to 1000. ";
  }
  if (input.size() < 7) {
    thePlot.numSegmentsE.AssignValue(numPoints, theCommands);
  }
  List<Expression> theConvertedExpressions;
  theConvertedExpressions.setSize(thePlot.dimension);
  thePlot.paramLowE = input[2];
  thePlot.paramHighE = input[3];
  if (
    !thePlot.paramLowE.EvaluatesToDouble(&thePlot.paramLow) ||
    !thePlot.paramHighE.EvaluatesToDouble(&thePlot.paramHigh)
  ) {
    theCommands << "Failed to convert "
    << thePlot.paramLowE.toString() << " and "
    << thePlot.paramHighE.toString()
    << " to left and right endpoint of parameter interval. ";
  }
  Vectors<double> theXs, theYs;

  bool isGoodLatexWise = true;
  for (int i = 0; i < thePlot.dimension; i ++) {
    if (!theCommands.CallCalculatorFunction(
      Calculator::innerSuffixNotationForPostScript,
      thePlot.coordinateFunctionsE[i],
      theConvertedExpressions[i]
    )) {
      theCommands << "Failed to extract suffix notation from argument "
      << thePlot.coordinateFunctionsE[i].toString();
      isGoodLatexWise = false;
      break;
    }
  }
  if (isGoodLatexWise && thePlot.dimension == 2) {
    std::stringstream outLatex, outHtml;
    outLatex << "\\parametricplot[linecolor =\\fcColorGraph, plotpoints =" << numPoints << "]{"
    << thePlot.paramLow << "}{" << thePlot.paramHigh << "}{"
    << theConvertedExpressions[0].GetValue<std::string>()
    << theConvertedExpressions[1].GetValue<std::string>() << "}";
    outHtml << "<br>%Calculator input: " << input.toString()
    << "<br>\\parametricplot[linecolor =\\fcColorGraph, plotpoints =" << numPoints << "]{"
    << thePlot.paramLow << "}{" << thePlot.paramHigh << "}{"
    << theConvertedExpressions[0].GetValue<std::string>()
    << theConvertedExpressions[1].GetValue<std::string>() << "}";
    thePlot.thePlotString= outLatex.str();
    thePlot.thePlotStringWithHtml = outHtml.str();
  }
  Expression converterE;
  thePlot.thePlotType = "parametricCurve";
  thePlot.coordinateFunctionsJS.setSize(thePlot.dimension);
  for (int i = 0; i < thePlot.dimension; i ++) {
    if (CalculatorFunctions::functionMakeJavascriptExpression(
      theCommands,
      thePlot.coordinateFunctionsE[i],
      converterE
    )) {
      thePlot.coordinateFunctionsJS[i] = converterE.toString();
    } else {
      thePlot.thePlotType = "parametricCurvePrecomputed";
      theCommands << "Failed to convert: "
      << thePlot.coordinateFunctionsE[i] << " to js. ";
    }
  }
  thePlot.numSegmenTsJS.setSize(1);
  thePlot.numSegmenTsJS[0] = "200";
  if (CalculatorFunctions::functionMakeJavascriptExpression(
    theCommands, thePlot.numSegmentsE, converterE
  )) {
    thePlot.numSegmenTsJS[0] = converterE.toString();
  } else {
    thePlot.thePlotType = "parametricCurvePrecomputMakeBoxed";
    theCommands << "Failed to convert: "
    << thePlot.numSegmentsE << " to js. ";
  }
  if (CalculatorFunctions::functionMakeJavascriptExpression(
    theCommands, thePlot.paramLowE, converterE
  )) {
    thePlot.paramLowJS = converterE.toString();
  } else {
    thePlot.thePlotType = "parametricCurvePrecomputed";
    theCommands << "Failed to convert: " << thePlot.paramLowE << " to js. ";
  }
  if (CalculatorFunctions::functionMakeJavascriptExpression(
    theCommands, thePlot.paramHighE, converterE
  )) {
    thePlot.paramHighJS = converterE.toString();
  } else {
    thePlot.thePlotType = "parametricCurvePrecomputed";
    theCommands << "Failed to convert: " << thePlot.paramHighE << " to js. ";
  }
  if (thePlot.dimension == 2) {
    if (!thePlot.coordinateFunctionsE[0].EvaluatesToDoubleInRange(
      thePlot.variablesInPlay[0].toString(),
      thePlot.paramLow,
      thePlot.paramHigh,
      numPoints,
      &thePlot.xLow,
      &thePlot.xHigh,
      &theXs
    )) {
      theCommands << "<hr>Failed to evaluate curve function. ";
    }
    if (!thePlot.coordinateFunctionsE[1].EvaluatesToDoubleInRange(
      thePlot.variablesInPlay[0].toString(),
      thePlot.paramLow,
      thePlot.paramHigh,
      numPoints,
      &thePlot.yLow,
      &thePlot.yHigh,
      &theYs
    )) {
      theCommands << "<hr>Failed to evaluate curve function. ";
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
  input.HasInputBoxVariables(&outputPlot.boxesThatUpdateMe);
  return output.AssignValue(outputPlot, theCommands);
}

template < >
SemisimpleSubalgebras& Expression::GetValueNonConst() const;

bool CalculatorFunctions::innerComputePairingTablesAndFKFTsubalgebras(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerComputePairingTablesAndFKFTsubalgebras");
  if (!input.IsOfType<SemisimpleSubalgebras>()) {
    return theCommands << "<hr>Input of ComputeFKFT must be of type semisimple subalgebras. ";
  }
  SemisimpleSubalgebras& theSAs = input.GetValueNonConst<SemisimpleSubalgebras>();
  theSAs.flagComputePairingTable = true;
  theSAs.flagComputeNilradicals = true;
  theSAs.ComputePairingTablesAndFKFTtypes();
  output = input;
  std::fstream theFile;
  std::string theFileName;
  theFileName = "FKFTcomputation.html";
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML = true;
  tempFormat.flagUseLatex = true;
  tempFormat.flagUseHTML = true;
  tempFormat.flagCandidateSubalgebraShortReportOnly = false;
  FileOperations::OpenFileCreateIfNotPresentVirtual(theFile, "output/" + theFileName, false, true, false);
  theFile << theSAs.toString(&tempFormat);
  std::stringstream out;
  out << "<a href=\"" << global.DisplayPathOutputFolder << "FKFTcomputation.html\">FKFTcomputation.html</a>";
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerGetCentralizerChainsSemisimpleSubalgebras(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGetCentralizerChainsSemisimpleSubalgebras");
  if (!input.IsOfType<SemisimpleSubalgebras>()) {
    return theCommands << "<hr>Input of GetCentralizerChains must be of type semisimple subalgebras. ";
  }
  SemisimpleSubalgebras& theSAs = input.GetValueNonConst<SemisimpleSubalgebras>();
  List<List<int> > theChains;
  std::stringstream out;
  theSAs.GetCentralizerChains(theChains);
  Expression currentChainE;
  out << theChains.size << " chains total. <br>";
  for (int i = 0; i < theChains.size; i ++) {
    out << "<br>Chain " << i + 1 << ": LoadSemisimpleSubalgebras{}( "
    << theSAs.owner->theWeyl.theDynkinType.toString() << ", (";
    for (int j = 0; j < theChains[i].size; j ++) {
      CalculatorConversions::innerStoreCandidateSA(
        theCommands, theSAs.theSubalgebras.theValues[theChains[i][j]], currentChainE
      );
      out << currentChainE.toString();
      if (j != theChains[i].size - 1) {
        out << ", ";
      }
    }
    out << ")  )";
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerEvaluateToDoublE(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Expression::innerEvaluateToDoublE");
  if (input.size() != 2) {
    // One argument expected.
    return false;
  }
  double theValue = 0;
  if (!input[1].EvaluatesToDouble(&theValue)) {
    return false;
  }
  return output.AssignValue(theValue, theCommands);
}

bool CalculatorFunctions::functionEvaluateToDouble(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Expression::functionEvaluateToDouble");
  double theValue = 0;
  if (!input.EvaluatesToDouble(&theValue)) {
    return false;
  }
  return output.AssignValue(theValue, theCommands);
}

bool CalculatorFunctions::innerTestMathMouseHover(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestMathMouseHover");
  std::stringstream out;
  out << "Hover mouse to render: " << HtmlRoutines::GetMathMouseHover(input.toString());
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerCanBeExtendedParabolicallyTo(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCanBeExtendedParabolicallyTo");
  if (!input.IsListNElements(3)) {
    return false;
  }
  DynkinType smallType, targetType;
  if (
    !CalculatorConversions::functionDynkinType(theCommands, input[1], smallType) ||
    !CalculatorConversions::functionDynkinType(theCommands, input[2], targetType)
  ) {
    return theCommands << "Failed to convert arguments of " << input.toString() << " to two DynkinType's.";
  }
  return output.AssignValue(static_cast<int>(smallType.CanBeExtendedParabolicallyTo(targetType)), theCommands);
}

bool CalculatorFunctions::innerGetSymmetricCartan(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGetSymmetricCartan");
  DynkinType theType;
  if (!CalculatorConversions::innerDynkinTypE(theCommands, input, theType)) {
    return theCommands << "Failed to convert " << input.toString() << " to DynkinType.";
  }
  std::stringstream out;
  Matrix<Rational> outputMat, outputCoMat;
  theType.GetCartanSymmetric(outputMat);
  theType.GetCoCartanSymmetric(outputCoMat);
  out << "Symmetric Cartan matrix: " << HtmlRoutines::GetMathMouseHover(outputMat.ToStringLatex(), 10000)
  << "<br>Co-symmetric Cartan matrix: " << HtmlRoutines::GetMathMouseHover(outputCoMat.ToStringLatex(), 10000);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerGetPrincipalSl2Index(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGetPrincipalSl2Index");
  DynkinType theType;
  if (!CalculatorConversions::innerDynkinTypE(theCommands, input, theType)) {
    return theCommands << "Failed to convert "
    << input.toString() << " to DynkinType.";
  }
  return output.AssignValue(theType.GetPrincipalSlTwoCSInverseScale(), theCommands);
}

bool CalculatorFunctions::innerGetDynkinIndicesSlTwoSubalgebras(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGetDynkinIndicesSlTwoSubalgebras");
  DynkinType theType;
  if (!CalculatorConversions::innerDynkinTypE(theCommands, input, theType)) {
    return theCommands << "Failed to convert "
    << input.toString() << " to DynkinType.";
  }
  if (theType.GetRank() > 20) {
    return theCommands
    << "Getting absolute Dynkin indices of sl(2)-subalgebras "
    << "is restricted up to rank 20 "
    << "(for computational feasibility reasons). ";
  }
  List<List<Rational> > bufferIndices;
  HashedList<DynkinSimpleType> bufferTypes;
  HashedList<Rational> theIndices;
  theType.GetDynkinIndicesSl2Subalgebras(bufferIndices, bufferTypes, theIndices);
  std::stringstream out;
  out << "There are " << theIndices.size << " absolute Dynkin indices. The indices are: "
  << theIndices.ToStringCommaDelimited();
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerEmbedSSalgInSSalg(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerEmbedSSalgInSSalg");
  if (input.size() != 3) {
    return output.MakeError("I expect two arguments - the two semisimple subalgebras.", theCommands);
  }
  const Expression& EsmallSA = input[1];
  const Expression& ElargeSA = input[2];
  WithContext<SemisimpleLieAlgebra*> smallSubalgebraPointer;
  if (!theCommands.Convert(
    EsmallSA,
    CalculatorConversions::functionSemisimpleLieAlgebra,
    smallSubalgebraPointer
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  WithContext<SemisimpleLieAlgebra*> largeSubalgebraPointer;
  if (!theCommands.Convert(
    ElargeSA,
    CalculatorConversions::functionSemisimpleLieAlgebra,
    largeSubalgebraPointer
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra& ownerSS = *largeSubalgebraPointer.content;
  std::stringstream out;
  if (ownerSS.GetRank() > 8) {
    out << "<b>This code is has been set to run up to ambient Lie algebra of rank 8. </b>";
    return output.AssignValue(out.str(), theCommands);
  } else {
    out << "<b>This code is completely experimental. Use the following printouts on your own risk</b>";
  }
  SemisimpleSubalgebras& theSSsubalgebras =
  theCommands.theObjectContainer.GetSemisimpleSubalgebrasCreateIfNotPresent(ownerSS.theWeyl.theDynkinType);
  theSSsubalgebras.ToStringExpressionString = CalculatorConversions::innerStringFromSemisimpleSubalgebras;

  out << "Attempting to embed "
  << smallSubalgebraPointer.content->theWeyl.theDynkinType.toString()
  << " in " << ownerSS.ToStringLieAlgebraName();
  theSSsubalgebras.FindTheSSSubalgebrasFromScratch(
    ownerSS,
    theCommands.theObjectContainer.theAlgebraicClosure,
    theCommands.theObjectContainer.semisimpleLieAlgebras,
    theCommands.theObjectContainer.theSltwoSAs,
    &smallSubalgebraPointer.content->theWeyl.theDynkinType
  );
  return output.AssignValue(theSSsubalgebras, theCommands);
}

bool CalculatorFunctions::innerWeylDimFormula(Calculator& theCommands, const Expression& input, Expression& output) {
  RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  if (input.size() != 3) {
    return output.MakeError("This function takes 2 arguments", theCommands);
  }
  WithContext<SemisimpleLieAlgebra*> theSSowner;
  if (!theCommands.Convert(
    input[1],
    CalculatorConversions::functionSemisimpleLieAlgebra,
    theSSowner
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  Vector<RationalFunction> theWeight;
  if (!theCommands.GetVectoR<RationalFunction>(
    input[2],
    theWeight,
    &theSSowner.context.context,
    theSSowner.content->GetRank(),
    CalculatorConversions::functionRationalFunction
  )) {
    return output.MakeError(
      "Failed to convert the argument of the function to a highest weight vector",
      theCommands
    );
  }
  RationalFunction rfOne;
  rfOne.makeOne();
  Vector<RationalFunction> theWeightInSimpleCoords;
  FormatExpressions theFormat;
  theSSowner.context.context.ContextGetFormatExpressions(theFormat);
  theWeightInSimpleCoords = theSSowner.content->theWeyl.GetSimpleCoordinatesFromFundamental(theWeight);
  theCommands << "<br>Weyl dim formula input: simple coords: "
  << theWeightInSimpleCoords.toString(&theFormat)
  << ", fundamental coords: " << theWeight.toString(&theFormat);
  RationalFunction tempRF = theSSowner.content->theWeyl.WeylDimFormulaSimpleCoords(theWeightInSimpleCoords);
  return output.AssignValueWithContext(tempRF, theSSowner.context.context, theCommands);
}

bool CalculatorFunctions::innerDecomposeFDPartGeneralizedVermaModuleOverLeviPart(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  RecursionDepthCounter recursionCounter(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(5)) {
    return output.MakeError(
      "Function decompose finite-dimensional part of "
      "generalized Verma over Levi expects 4 arguments.",
      theCommands
    );
  }
  const Expression& typeNode = input[1];
  const Expression& weightNode = input[2];
  const Expression& inducingParNode = input[3];
  const Expression& splittingParNode = input[4];
  WithContext<SemisimpleLieAlgebra*> ownerSSPointer;
  if (!theCommands.Convert(
    typeNode,
    CalculatorConversions::functionSemisimpleLieAlgebra,
    ownerSSPointer
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  Vector<RationalFunction> theWeightFundCoords;
  Vector<Rational> inducingParSel, splittingParSel;
  SemisimpleLieAlgebra& ownerSS = *ownerSSPointer.content;
  WeylGroupData& theWeyl = ownerSS.theWeyl;
  int theDim = ownerSS.GetRank();
  Expression finalContext;
  if (!theCommands.GetVectoR<RationalFunction>(
    weightNode,
    theWeightFundCoords,
    &finalContext,
    theDim,
    CalculatorConversions::functionRationalFunction
  )) {
    return output.MakeError("Failed to extract highest weight from the second argument.", theCommands);
  }
  if (!theCommands.GetVectoR<Rational>(inducingParNode, inducingParSel, &finalContext, theDim, nullptr)) {
    return output.MakeError("Failed to extract parabolic selection from the third argument", theCommands);
  }
  if (!theCommands.GetVectoR<Rational>(splittingParNode, splittingParSel, &finalContext, theDim, nullptr)) {
    return output.MakeError("Failed to extract parabolic selection from the fourth argument", theCommands);
  }
  theCommands << "Your input weight in fundamental coordinates: " << theWeightFundCoords.toString();
  theCommands << "<br>Your input weight in simple coordinates: "
  << theWeyl.GetSimpleCoordinatesFromFundamental(theWeightFundCoords).toString()
  << "<br>Your inducing parabolic subalgebra: " << inducingParSel.toString() << "."
  << "<br>The parabolic subalgebra I should split over: " << splittingParSel.toString() << ".";
  ModuleSSalgebra<RationalFunction> theMod;
  Selection selInducing = inducingParSel;
  Selection selSplittingParSel = splittingParSel;
  theMod.MakeFromHW(ownerSS, theWeightFundCoords, selInducing, 1, 0, nullptr, false);
  std::string report;
  theMod.SplitOverLevi(&report, selSplittingParSel);
  return output.AssignValue(report, theCommands);
}

bool CalculatorFunctions::innerSplitFDpartB3overG2Init(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  branchingData& theG2B3Data,
  Expression& outputContext
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSplitFDpartB3overG2Init");
  if (!input.IsListNElements(4)) {
    return output.MakeError(
      "Splitting the f.d. part of a B_3-representation "
      "over G_2 requires 3 arguments",
      theCommands
    );
  }
  if (!theCommands.GetVectorFromFunctionArguments<RationalFunction>(
    input,
    theG2B3Data.theWeightFundCoords,
    &outputContext,
    3,
    CalculatorConversions::functionRationalFunction
  )) {
    output.MakeError(
      "Failed to extract highest weight in fundamental coordinates. ",
      theCommands
    );
  }
  theCommands.MakeHmmG2InB3(theG2B3Data.theHmm);
  theG2B3Data.selInducing.init(3);
  for (int i = 0; i < theG2B3Data.theWeightFundCoords.size; i ++) {
    if (!theG2B3Data.theWeightFundCoords[i].IsSmallInteger()) {
      theG2B3Data.selInducing.AddSelectionAppendNewIndex(i);
    }
  }
  theG2B3Data.initAssumingParSelAndHmmInittedPart1NoSubgroups();
  return true;
}

bool CalculatorFunctions::innerParabolicWeylGroups(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerParabolicWeylGroups");
  if (input.size() != 2) {
    return false;
  }
  Selection selectionParSel;
  WithContext<SemisimpleLieAlgebra*> theSSPointer;
  if (!theCommands.Convert(
    input[1],
    CalculatorConversions::functionSemisimpleLieAlgebra,
    theSSPointer
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra& theSSalgebra = *theSSPointer.content;
  int numCycles = MathRoutines::TwoToTheNth(selectionParSel.MaxSize);
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms theSubgroup;
  std::stringstream out;
  for (int i = 0; i < numCycles; i ++, selectionParSel.incrementSelection()) {
    theSubgroup.MakeParabolicFromSelectionSimpleRoots(theSSalgebra.theWeyl, selectionParSel, 2000);
    out << "<hr>" << HtmlRoutines::GetMathSpanPure(theSubgroup.toString());
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerParabolicWeylGroupsBruhatGraph(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerParabolicWeylGroupsBruhatGraph");
  RecursionDepthCounter theRecursion(&theCommands.RecursionDeptH);
  Selection parabolicSel;
  Vector<RationalFunction> theHWfundcoords, tempRoot, theHWsimplecoords;
  WithContext<SemisimpleLieAlgebra*> theSSalgPointer;
  if (!theCommands.GetTypeHighestWeightParabolic(
    theCommands,
    input,
    output,
    theHWfundcoords,
    parabolicSel,
    theSSalgPointer,
    CalculatorConversions::functionRationalFunction
  )) {
    return output.MakeError("Failed to extract highest weight vector data", theCommands);
  } else {
    if (output.IsError()) {
      return true;
    }
  }
  SemisimpleLieAlgebra& theSSalgebra = *theSSalgPointer.content;

  WeylGroupData& theAmbientWeyl = theSSalgebra.theWeyl;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms theSubgroup;
  std::stringstream out;
  if (!theSubgroup.MakeParabolicFromSelectionSimpleRoots(theAmbientWeyl, parabolicSel, 500)) {
    return output.MakeError("<br><br>Failed to generate Weyl subgroup, 500 elements is the limit", theCommands);
  }
  theSubgroup.FindQuotientRepresentatives(2000);
  out << "<br>Number elements of the coset: "
  << theSubgroup.RepresentativesQuotientAmbientOrder.size;
  out << "<br>Number of elements of the Weyl group of the Levi part: " << theSubgroup.allElements.size;
  out << "<br>Number of elements of the ambient Weyl: "
  << theSubgroup.AmbientWeyl->theGroup.theElements.size;
  FormatExpressions theFormat;
  theSSalgPointer.context.context.ContextGetFormatExpressions(theFormat);
  if (theSubgroup.allElements.size > 498) {
    if (theSubgroup.AmbientWeyl->SizeByFormulaOrNeg1('E', 6) <= theSubgroup.AmbientWeyl->theGroup.GetSize()) {
      out << "Weyl group is too large. <br>";
    } else {
      out << "Weyl group is too large for LaTeX. <br>";
    }
  } else {
    std::stringstream outputFileContent, outputFileContent2;
    std::string fileHasse, fileCosetGraph;
    bool useJavascript = (theSubgroup.allElements.size < 100);
    outputFileContent << "\\documentclass{article}\\usepackage[all,cmtip]{xy}\\begin{document}\n";
    outputFileContent << "\\[" << theSubgroup.ElementToStringBruhatGraph() << "\\]";
    outputFileContent << "\n\\end{document}";
    outputFileContent2 << "\\documentclass{article}\\usepackage[all,cmtip]{xy}\\begin{document}\n";
    outputFileContent2 << "\\[" << theSubgroup.ElementToStringCosetGraph() << "\\]";
    outputFileContent2 << "\n\\end{document}";
    theCommands.WriteDefaultLatexFileReturnHtmlLink(outputFileContent.str(), &fileHasse, true);
    theCommands.WriteDefaultLatexFileReturnHtmlLink(outputFileContent2.str(), &fileCosetGraph, true);
    out << "<hr>"
    << " The Hasse graph of the Weyl group of the Levi part follows. <a href=\""
    << fileHasse << ".tex\"> "
    << fileHasse << ".tex</a>";
    out << ", <iframe src =\"" << fileHasse
    << ".png\" width =\"800\" height =\"600\">"
    << fileHasse << ".png</iframe>";
    out << "<hr> The coset graph of the Weyl group of the Levi part follows. "
    << "The cosets are right, i.e. a coset "
    << " of the subgroup X is written in the form Xw, where w is one of the elements below. "
    << "<a href=\""
    << fileCosetGraph
    << ".tex\"> " << fileCosetGraph << ".tex</a>";
    out << ", <iframe src =\"" << fileCosetGraph
    << ".png\" width =\"800\" height =\"600\"> " << fileCosetGraph << ".png</iframe>";
    out << "<b>The .png file might be bad if LaTeX crashed while trying to process it; "
    << "please check whether the .tex corresponds to the .png.</b>";
    out << "<hr>Additional printout follows.<br> ";
    out << "<br>Representatives of the coset follow. Below them you can find "
    << "the elements of the subgroup. <br>";
    out << "<table><tr><td>Element</td><td>weight simple coords</td>"
    << "<td>weight fund. coords</td></tr>";
    theFormat.fundamentalWeightLetter = "\\omega";
    for (int i = 0; i < theSubgroup.RepresentativesQuotientAmbientOrder.size; i ++) {
      ElementWeylGroup& current = theSubgroup.RepresentativesQuotientAmbientOrder[i];
      out << "<tr><td>"
      << (useJavascript ? HtmlRoutines::GetMathSpanPure(current.toString()) : current.toString())
      << "</td>";
      theHWsimplecoords = theSSalgebra.theWeyl.GetSimpleCoordinatesFromFundamental(theHWfundcoords);
      theSSalgebra.theWeyl.ActOnRhoModified(theSubgroup.RepresentativesQuotientAmbientOrder[i], theHWsimplecoords);
      out << "<td>"
      << (useJavascript ? HtmlRoutines::GetMathSpanPure(theHWsimplecoords.toString(&theFormat))
      : theHWsimplecoords.toString(&theFormat))
      << "</td>";
      tempRoot = theSSalgebra.theWeyl.GetFundamentalCoordinatesFromSimple(theHWsimplecoords);
      std::string theFundString = tempRoot.ToStringLetterFormat(theFormat.fundamentalWeightLetter, &theFormat);
      out << "<td>" << (useJavascript ? HtmlRoutines::GetMathSpanPure(theFundString): theFundString)
      << "</td>";
      out << "</tr>";
    }
    out << "</table><hr>";
    out << theSubgroup.toString();
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerAllPartitions(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerAllPartitions");
  if (input.size() != 2) {
    return false;
  }
  int theRank = - 1;
  if (!input[1].IsSmallInteger(&theRank)) {
    return false;
  }
  if (theRank > 33 || theRank < 0) {
    return theCommands
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
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerAllVectorPartitions(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerAllVectorPartitions");
  if (input.size() != 3) {
    return theCommands << "<hr>AllVectorPartitions function takes 3 arguments.";
  }
  VectorPartition thePartition;
  const Expression& theVectorE = input[1];
  const Expression& thePartitioningVectorsE = input[2];
  if (!theCommands.GetVectoR(theVectorE, thePartition.goalVector)) {
    return theCommands << "<hr>Failed to extract vector from " << theVectorE.toString();
  }
  Matrix<Rational> vectorsMatForm;
  if (!theCommands.functionGetMatrix(
    thePartitioningVectorsE,
    vectorsMatForm,
    nullptr,
    thePartition.goalVector.size
  )) {
    return theCommands << "<hr>Failed to extract list of vectors from "
    << thePartitioningVectorsE.toString();
  }
  Vectors<Rational> theInputVectors;
  theInputVectors.AssignMatrixRows(vectorsMatForm);
  for (int i = 0; i < theInputVectors.size; i ++) {
    if (!theInputVectors[i].IsPositive()) {
      return theCommands << "<hr>Input vector "
      << theInputVectors[i].toString() << " is non-positive";
    }
  }
  if (!thePartition.init(theInputVectors, thePartition.goalVector)) {
    return theCommands << "<hr>Failed to initialize vector partition object";
  }
  std::stringstream out;
  int numFound = 0;
  ProgressReport theReport;
  out << thePartition.ToStringPartitioningVectors();
  while (thePartition.IncrementReturnFalseIfPastLast()) {
    out << "<br>" << thePartition.ToStringOnePartition(thePartition.currentPartition);
    numFound ++;
    if (numFound % 1000 == 0) {
      std::stringstream reportStream;
      reportStream << "Found " << numFound << " partitions of " << thePartition.goalVector.toString()
      << "<br>Current partition: " << thePartition.currentPartition;
      theReport.Report(reportStream.str());
    }
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerDeterminant(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDeterminant");
  Matrix<Rational> matRat;
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (theCommands.functionGetMatrix(argument, matRat)) {
    if (matRat.NumRows == matRat.NumCols) {
      if (matRat.NumRows > 100) {
        return theCommands << "<hr>I have been instructed not to compute "
        << "determinants of rational matrices larger than 100 x 100 "
        << ", and your matrix had " << matRat.NumRows << " rows. "
        << "To lift the restriction "
        << "edit function located in file " << __FILE__ << ", line " << __LINE__ << ". ";
      }
      return output.AssignValue(matRat.GetDeterminant(), theCommands);
    } else {
      theCommands << "Requesting to compute determinant of the non-square "
      << matRat.NumRows << " by "
      << matRat.NumCols << " matrix: " << argument.toString();
      return output.MakeError(
        "Requesting to compute determinant of non-square matrix. ", theCommands
      );
    }
  }
  Matrix<AlgebraicNumber> matAlg;
  if (theCommands.functionGetMatrix(argument, matAlg)) {
    if (matAlg.NumRows == matAlg.NumCols) {
      if (matAlg.NumRows > 100) {
        return theCommands
        << "<hr>I have been instructed not to compute determinants "
        << "of algebraic number matrices larger than 100 x 100 "
        << ", and your matrix had " << matAlg.NumRows
        << " rows. " << "To lift the restriction "
        << "edit function located in file "
        << __FILE__ << ", line " << __LINE__ << ". ";
      }
      return output.AssignValue(matAlg.GetDeterminant(), theCommands);
    } else {
      theCommands << "Requesting to compute determinant of the non-square "
      << matRat.NumRows << " by "
      << matRat.NumCols << " matrix: " << argument.toString();
      return output.MakeError(
        "Requesting to compute determinant of non-square matrix. ",
        theCommands
      );
    }
  }
  Matrix<RationalFunction> matRF;
  Expression theContext;
  if (!theCommands.functionGetMatrix(
      argument,
      matRF,
      &theContext,
      - 1,
      CalculatorConversions::functionRationalFunction
  )) {
    return theCommands
    << "<hr>I have been instructed to only compute "
    << "determinants of matrices whose entries are "
    << "rational functions or rationals, and "
    << "I failed to convert your matrix to either type. "
    << "If this is not how you expect this function to act, "
    << "correct it: the code is located in "
    << "file " << __FILE__ << ", line " << __LINE__ << ". ";
  }
  if (matRF.NumRows != matRF.NumCols) {
    theCommands << "Requesting to compute determinant of the non-square "
    << matRat.NumRows << " by "
    << matRat.NumCols << " matrix: " << argument.toString();
    return output.MakeError(
      "Requesting to compute determinant of non-square matrix given by: " + argument.toString(),
      theCommands
    );
  }
  if (matRF.NumRows > 20) {
    return theCommands << "I have been instructed not to "
    << "compute determinants of matrices of rational functions larger than "
    << "20 x 20, and your matrix had " << matRF.NumRows
    << " rows. To lift the restriction edit function located in file "
    << __FILE__ << ", line " << __LINE__ << ". ";
  }
  RationalFunction theDet = matRF.GetDeterminant();
  return output.AssignValueWithContext(theDet, theContext, theCommands);
}

bool CalculatorFunctions::innerDecomposeCharGenVerma(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDecomposeCharGenVerma");
  Vector<RationalFunction> theHWfundcoords, theHWsimpCoords, theHWFundCoordsFDPart, theHWSimpCoordsFDPart;
  Selection parSel, invertedParSel;
  WithContext<SemisimpleLieAlgebra*> theSSlieAlg;
  output.reset(theCommands);
  if (!theCommands.GetTypeHighestWeightParabolic<RationalFunction>(
    theCommands,
    input,
    output,
    theHWfundcoords,
    parSel,
    theSSlieAlg,
    CalculatorConversions::functionRationalFunction
  )) {
   return false;
  }
  if (output.IsError()) {
    return true;
  }
  std::stringstream out;
  FormatExpressions theFormat;
  theSSlieAlg.context.context.ContextGetFormatExpressions(theFormat);
  out << "<br>Highest weight: " << theHWfundcoords.toString(&theFormat)
  << "<br>Parabolic selection: " << parSel.toString();
  theHWFundCoordsFDPart = theHWfundcoords;
  for (int i = 0; i < parSel.CardinalitySelection; i ++) {
    theHWFundCoordsFDPart[parSel.elements[i]] = 0;
  }
  KLpolys theKLpolys;
  WeylGroupData& theWeyl = theSSlieAlg.content->theWeyl;
  if (!theKLpolys.ComputeKLPolys(&theWeyl)) {
    return output.MakeError("failed to generate Kazhdan-Lusztig polynomials (output too large?)", theCommands);
  }
  theHWSimpCoordsFDPart = theWeyl.GetSimpleCoordinatesFromFundamental(theHWFundCoordsFDPart);
  theHWSimpCoordsFDPart += theWeyl.rho;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms theSub;
  if (!theSub.MakeParabolicFromSelectionSimpleRoots(theWeyl, parSel, 1000)) {
    return output.MakeError(
      "Failed to generate Weyl subgroup of Levi part (possibly too large? element limit is 1000).",
      theCommands
    );
  }
  theHWsimpCoords = theWeyl.GetSimpleCoordinatesFromFundamental(theHWfundcoords);
  List<ElementWeylGroup> theWeylElements;
  theSub.GetGroupElementsIndexedAsAmbientGroup(theWeylElements);
  Vector<RationalFunction> currentHW;
  out << "<br>Orbit modified with small rho: "
  << "<table><tr><td>Simple coords</td><td>Fund coords</td></tr>";
  for (int i = 0; i < theWeyl.theGroup.theElements.size; i ++) {
    currentHW = theHWsimpCoords;
    currentHW += theSub.GetRho();
    theWeyl.ActOn(i, currentHW);
    currentHW -= theSub.GetRho();
    out << "<tr><td>" << currentHW.toString() << "</td><td>"
    << theWeyl.GetFundamentalCoordinatesFromSimple(currentHW).toString() << "</td></tr>";
  }
  out << "</table>";
  out << "<br>The rho of the Levi part is: "
  << theSub.GetRho().toString() << "<br>Weyl group of Levi part follows. "
  << "<br><table><tr><td>Weyl element</td>"
  << "<td>Image hw under small rho modified action fund coords</td>"
  << "<td>Character Verma given h.w.</td></tr>";
  invertedParSel = parSel;
  invertedParSel.InvertSelection();
  charSSAlgMod<RationalFunction> theChar, currentChar;
  Weight<RationalFunction> theMon;
  theChar.makeZero();
  FormatExpressions formatChars;
  formatChars.FDrepLetter = "L";
  formatChars.fundamentalWeightLetter = "\\omega";
  formatChars.flagUseLatex = true;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms theSubgroup;
  theSubgroup.AmbientWeyl = theKLpolys.TheWeylGroup;
  for (int i = 0; i < theWeylElements.size; i ++) {
    ElementWeylGroup& currentElement = theWeylElements[i];
    out << "<tr><td>" << currentElement.toString() << "</td>";

    int indexInWeyl = theKLpolys.TheWeylGroup->theGroup.theElements.GetIndex(currentElement);
    if (indexInWeyl == - 1) {
      global.fatal << "This is a programming error. Something is wrong: "
      << "I am getting that an element of the Weyl group of the Levi part of "
      << "the parabolic does not lie in the ambient Weyl group, which is impossible. "
      << "There is a bug somewhere; crashing in accordance. " << global.fatal;
    }
    currentChar.makeZero();
    theMon.owner = theSSlieAlg.content;
    for (int j = 0; j < theKLpolys.theKLcoeffs[indexInWeyl].size; j ++) {
      if (!theKLpolys.theKLcoeffs[indexInWeyl][j].IsEqualToZero()) {
        currentHW = theHWsimpCoords;
        theWeyl.ActOnRhoModified(j, currentHW);
        theMon.weightFundamentalCoordS = theWeyl.GetFundamentalCoordinatesFromSimple(currentHW);
        int sign = (currentElement.generatorsLastAppliedFirst.size - theWeyl.theGroup.theElements[j].generatorsLastAppliedFirst.size) % 2 == 0 ? 1 : - 1;
        currentChar.AddMonomial(theMon, theKLpolys.theKLcoeffs[indexInWeyl][j] * sign);
      }
    }
    currentHW = theHWsimpCoords;
    currentHW += theSub.GetRho();
    theWeyl.ActOn(indexInWeyl, currentHW);
    currentHW -= theSub.GetRho();
    out << "<td>" << theWeyl.GetFundamentalCoordinatesFromSimple(currentHW).ToStringLetterFormat("\\omega") << "</td>";
    out << "<td>" << HtmlRoutines::GetMathMouseHover(currentChar.toString(&formatChars)) << "</td>";
    if (currentElement.generatorsLastAppliedFirst.size % 2 == 1) {
      currentChar *= - 1;
    }
    theChar += currentChar;
    out << "</tr>";
  }
  out << "</table>";
  out << "Final char: " << HtmlRoutines::GetMathMouseHover(theChar.toString(&formatChars));
  return output.AssignValue<std::string>(out.str(), theCommands);
}

bool CalculatorFunctions::innerPrintGenVermaModule(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPrintGenVermaModule");
  Selection selectionParSel;
  Vector<RationalFunction> theHWfundcoords;
  WithContext<SemisimpleLieAlgebra*> theSSalgebra;
  if (!theCommands.GetTypeHighestWeightParabolic(
    theCommands,
    input,
    output,
    theHWfundcoords,
    selectionParSel,
    theSSalgebra,
    CalculatorConversions::functionRationalFunction
  )) {
    return output.MakeError("Failed to extract highest weight vector data", theCommands);
  } else {
    if (output.IsError()) {
      return true;
    }
  }
  if (!theCommands.innerHWVCommon(
    theCommands,
    output,
    theHWfundcoords,
    selectionParSel,
    theSSalgebra.context.context,
    theSSalgebra.content,
    false
  )) {
    return output.MakeError("Failed to create Generalized Verma module", theCommands);
  }
  if (output.IsError()) {
    return true;
  }
  ElementTensorsGeneralizedVermas<RationalFunction> theElt;
  theElt = output.GetValue<ElementTensorsGeneralizedVermas<RationalFunction> >();
  ModuleSSalgebra<RationalFunction>& theModule = *theElt[0].theMons[0].owner;
  return output.AssignValue(theModule.toString(), theCommands);
}

bool CalculatorFunctions::innerWriteGenVermaModAsDiffOperatorUpToLevel(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerWriteGenVermaModAsDiffOperatorUpToLevel");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(4)) {
    return output.MakeError(
      "Function innerSplitGenericGenVermaTensorFD is expected "
      "to have three arguments: SS algebra type, Number, List{}. ",
      theCommands
    );
  }
  const Expression& leftE = input[1];
  const Expression& genVemaWeightNode = input[3];
  const Expression& levelNode = input[2];
  Expression resultSSalgebraE;
  resultSSalgebraE = leftE;
  WithContext<SemisimpleLieAlgebra*> theSSalgebra;
  if (!theCommands.Convert(
    leftE, CalculatorConversions::functionSemisimpleLieAlgebra, theSSalgebra
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  int theRank = theSSalgebra.content->GetRank();
  Vector<Polynomial<Rational> > highestWeightFundCoords;
  Expression hwContext(theCommands);
  if (!theCommands.GetVectoR(
    genVemaWeightNode,
    highestWeightFundCoords,
    &hwContext,
    theRank,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return theCommands
    << "Failed to convert the third argument of innerSplitGenericGenVermaTensorFD to a list of " << theRank
    << " polynomials. The second argument you gave is " << genVemaWeightNode.toString() << ".";
  }
  int desiredHeight;
  if (!levelNode.IsSmallInteger(&desiredHeight)) {
    return output.MakeError("second argument of " + input.toString() + " must be a small integer", theCommands);
  }
  RationalFunction RFOne, RFZero;
  RFOne.makeOne();
  RFZero.makeZero();
  Selection selInducing;
  selInducing.MakeFullSelection(theRank);
  int theCoeff;
  for (int i = 0; i < theRank; i ++) {
    if (highestWeightFundCoords[i].IsSmallInteger(&theCoeff)) {
      if (theCoeff == 0) {
        selInducing.RemoveSelection(i);
      }
    }
  }
  Vectors<Polynomial<Rational> > theHws;
  Selection invertedSelInducing = selInducing;
  invertedSelInducing.InvertSelection();
  theHws.setSize(0);
  SelectionWithMaxMultiplicity theHWenumerator;
  Vector<Polynomial<Rational> > theHWrf;
  for (int j = 0; j <= desiredHeight; j ++) {
    theHWenumerator.initMaxMultiplicity(theRank - selInducing.CardinalitySelection, j);
    theHWenumerator.IncrementSubsetFixedCardinality(j);
    int numCycles = theHWenumerator.NumCombinationsOfCardinality(j);
    for (int i = 0; i < numCycles; i ++, theHWenumerator.IncrementSubsetFixedCardinality(j)) {
      theHWrf = highestWeightFundCoords;
      for (int k = 0; k < invertedSelInducing.CardinalitySelection; k ++) {
        theHWrf[invertedSelInducing.elements[k]] += Rational(theHWenumerator.Multiplicities[k]);
      }
      theHws.addOnTop(theHWrf);
    }
  }
  FormatExpressions theFormat;
  hwContext.ContextGetFormatExpressions(theFormat);
  return theCommands.innerWriteGenVermaModAsDiffOperatorInner(
    theCommands,
    input,
    output,
    theHws,
    hwContext,
    selInducing,
    theSSalgebra.content,
    false,
    nullptr,
    nullptr,
    nullptr,
    true,
    true
  );
}

bool CalculatorFunctions::innerHWV(Calculator& theCommands, const Expression& input, Expression& output) {
  Selection selectionParSel;
  Vector<RationalFunction> theHWfundcoords;
  WithContext<SemisimpleLieAlgebra*> theSSalgebra;
  if (!theCommands.GetTypeHighestWeightParabolic(
    theCommands,
    input,
    output,
    theHWfundcoords,
    selectionParSel,
    theSSalgebra,
    CalculatorConversions::functionRationalFunction
  )) {
    return output.MakeError("Failed to extract highest weight vector data", theCommands);
  } else {
    if (output.IsError()) {
      return true;
    }
  }
  return theCommands.innerHWVCommon(
    theCommands,
    output,
    theHWfundcoords,
    selectionParSel,
    theSSalgebra.context.context,
    theSSalgebra.content
  );
}

bool CalculatorFunctions::innerSplitGenericGenVermaTensorFD(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSplitGenericGenVermaTensorFD");
  RecursionDepthCounter theRecursionIncrementer(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(4))
    return output.MakeError(
      "Function innerSplitGenericGenVermaTensorFD is expected to "
      "have three arguments: SS algebra type, weight, weight. ",
      theCommands
    );
  const Expression& leftE = input[1];
  const Expression& genVemaWeightNode = input[3];
  const Expression& fdWeightNode = input[2];
  WithContext<SemisimpleLieAlgebra*> theSSalgebra;
  if (!theCommands.Convert(
    leftE, CalculatorConversions::functionSemisimpleLieAlgebra, theSSalgebra
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  int theRank = theSSalgebra.content->GetRank();
  Vector<RationalFunction> highestWeightFundCoords;
  Expression hwContext(theCommands);
  if (!theCommands.GetVectoR<RationalFunction>(
    genVemaWeightNode,
    highestWeightFundCoords,
    &hwContext,
    theRank,
    CalculatorConversions::functionRationalFunction
  )) {
    return theCommands
    << "Failed to convert the third argument of "
    << "innerSplitGenericGenVermaTensorFD to a list of " << theRank
    << " polynomials. The second argument you gave is "
    << genVemaWeightNode.toString() << ".";
  }
  Vector<Rational> theFDhw;
  if (!theCommands.GetVectoR<Rational>(fdWeightNode, theFDhw, nullptr, theRank, nullptr)) {
    return theCommands
    << "Failed to convert the second argument of "
    << "innerSplitGenericGenVermaTensorFD to a list of "
    << theRank
    << " rationals. The second argument you gave is "
    << fdWeightNode.toString() << ".";
  }
  int theNumVars = hwContext.ContextGetNumContextVariables();
  RationalFunction RFOne, RFZero;
  RFOne.makeOne();
  RFZero.makeZero();
  ElementTensorsGeneralizedVermas<RationalFunction> theElt;
  //= theElementData.theElementTensorGenVermas.GetElement();
  Selection selParSel1, selFD;
  Expression hwvFD, hwvGenVerma;
  selParSel1.MakeFullSelection(theRank);
  selFD.init(theRank);
  int theCoeff;
  for (int i = 0; i < theRank; i ++) {
    if (highestWeightFundCoords[i].IsSmallInteger(&theCoeff)) {
      if (theCoeff >= 0) {
        selParSel1.RemoveSelection(i);
      }
    }
    bool isGood = false;
    if (theFDhw[i].IsSmallInteger(&theCoeff)) {
      if (theCoeff >= 0) {
        isGood = true;
      }
    }
    if (!isGood) {
      return output.MakeError(
        "Error: the third argument of innerSplitGenericGenVermaTensorFD "
        "must be a list of small non-negative integers.",
        theCommands
      );
    }
  }
  theSSalgebra.content->flagHasNilradicalOrder = true;
  if (!theCommands.innerHWVCommon(
    theCommands, hwvGenVerma, highestWeightFundCoords, selParSel1, hwContext, theSSalgebra.content
  )) {
    return false;
  }
  if (hwvGenVerma.IsError()) {
    output = hwvGenVerma;
    return true;
  }
  Vector<RationalFunction> theFDhwRF;
  theFDhwRF = theFDhw;
  if (!theCommands.innerHWVCommon(theCommands, hwvFD, theFDhwRF, selFD, hwContext, theSSalgebra.content)) {
    return false;
  }
  if (hwvFD.IsError()) {
    output = hwvFD;
    return true;
  }
  std::stringstream out;
  out << "hwv par sel: " << hwvGenVerma.toString() << "hwv fd: " << hwvFD.toString();
  const ElementTensorsGeneralizedVermas<RationalFunction>& theHWgenVerma =
  hwvGenVerma.GetValue<ElementTensorsGeneralizedVermas<RationalFunction> >();
  const ElementTensorsGeneralizedVermas<RationalFunction>& theHWfd =
  hwvFD.GetValue<ElementTensorsGeneralizedVermas<RationalFunction> >();

  ModuleSSalgebra<RationalFunction>& theGenMod = theHWgenVerma[0].theMons[0].GetOwner();
  ModuleSSalgebra<RationalFunction>& theFDMod = theHWfd[0].theMons[0].GetOwner();
  if (
    theGenMod.owner != theFDMod.owner ||
    theGenMod.GetOwner().GetRank() != theGenMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize ||
    theFDMod.GetOwner().GetRank() != theFDMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize
  ) {
    global.fatal << "This is a programming error: the two modules have owners, "
    << theFDMod.GetOwner().theWeyl.theDynkinType.toString()
    << " and " << theGenMod.GetOwner().theWeyl.theDynkinType.toString() << ", and parabolic selections of max size "
    << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize
    << " and " << theFDMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize << global.fatal;
  }
  ElementUniversalEnveloping<RationalFunction> theCasimir, theCasimirMinusChar;
  charSSAlgMod<RationalFunction> theHWchar, theFDLeviSplit, theFDChaR, theFDLeviSplitShifteD;
  theHWchar.MakeFromWeight(theFDMod.theHWSimpleCoordSBaseField, theSSalgebra.content);
  List<ElementUniversalEnveloping<RationalFunction> > theLeviEigenVectors;
  Vectors<RationalFunction> theEigenVectorWeightsFund;
  if (theGenMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize != theGenMod.GetOwner().GetRank()) {
    global.fatal << "This is a programming error: module has parabolic selection with max size "
    << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.MaxSize << " but the ambient semisimple Lie algebra is of rank "
    << theGenMod.GetOwner().GetRank() << ". " << global.fatal;
  }
  std::string report;
  theFDMod.SplitOverLevi(
    &report,
    theGenMod.parabolicSelectionNonSelectedAreElementsLevi,
    &theLeviEigenVectors,
    &theEigenVectorWeightsFund,
    nullptr,
    &theFDLeviSplit
  );
  theFDMod.GetFDchar(theFDChaR);
  List<ElementUniversalEnveloping<RationalFunction> > theCentralCharacters;
  theCasimir.MakeCasimir(*theSSalgebra.content);
  Vector<RationalFunction> currentHWsimplecoords, currentHWdualcoords;
  FormatExpressions tempFormat;
  tempFormat.MaxLineLength = 60;
  tempFormat.flagUseLatex = true;
  tempFormat.fundamentalWeightLetter = "\\psi";
  tempFormat.CustomPlusSign = "\\oplus ";
  hwContext.ContextGetFormatExpressions(tempFormat);
  out << "<br>Character of finite dimensional module:" << HtmlRoutines::GetMathMouseHover(theFDChaR.toString());
  if (theGenMod.parabolicSelectionSelectedAreElementsLevi.CardinalitySelection > 0) {
    out << "<br>theFDChar split over levi:" << HtmlRoutines::GetMathMouseHover(theFDLeviSplit.toString(&tempFormat));
  }
  std::stringstream latexReport1;
  out << "<br><table><tr><td>weight in fundamental coords</td><td>Character</td></tr>";
  latexReport1 << " \\begin{longtable}{rl}\\caption{\\label{table" << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.toString()
  << "GenVermatimes7DimCentralCharacters} $" << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.toString()
  << "$- parabolic $\\bar{\\mathfrak{p}}$} \\\\ $\\mu+\\gamma$ & Action of $\\bar c$\\\\\\hline";
  tempFormat.CustomPlusSign = "";
  tempFormat.chevalleyGgeneratorLetter = "\\bar{g}";
  tempFormat.chevalleyHgeneratorLetter = "\\bar{h}";
  theFDLeviSplitShifteD.makeZero();
  Weight<RationalFunction> tempMon;
  tempMon.owner = theSSalgebra.content;
  ElementUniversalEnveloping<RationalFunction> currentChar;
  for (int i = 0; i < theLeviEigenVectors.size; i ++) {
    tempMon.weightFundamentalCoordS = theEigenVectorWeightsFund[i];
    tempMon.weightFundamentalCoordS += theGenMod.theHWFundamentalCoordsBaseField;
    theFDLeviSplitShifteD.AddMonomial(tempMon, RFOne);
    currentHWdualcoords = theSSalgebra.content->theWeyl.GetDualCoordinatesFromFundamental(tempMon.weightFundamentalCoordS);
    currentChar = theCasimir;
    currentChar.ModOutVermaRelations(& currentHWdualcoords, RFOne, RFZero);
    theCentralCharacters.addOnTop(currentChar);
    out << "<tr><td>" << theFDLeviSplitShifteD[i].weightFundamentalCoordS.ToStringLetterFormat("\\psi") << "</td><td>"
    << currentChar.toString(&tempFormat) << "</td></tr>";
    latexReport1 << " $" << theFDLeviSplitShifteD[i].weightFundamentalCoordS.ToStringLetterFormat("\\psi", &tempFormat) << "$"
    << "&$p_{" << i + 1 << "}=$ $" << currentChar.toString(&tempFormat) << "$\\\\<br>";
  }
  out << "</table>";
  latexReport1 << "\\end{longtable}<br>";
  ElementTensorsGeneralizedVermas<RationalFunction> tempElt, tempElt2;
  theFDMod.highestWeightVectorNotation = "v";
  theGenMod.highestWeightVectorNotation = "w";
  out << "Let w be the highest weight vector of the generalized Verma component, and let v be the highest weight vector of the finite dimensional component";
  out << "<br><table><tr><td>weight in fundamental coords</td><td>Algebraic expression</td><td>Additional multiplier</td>";
  if (theNumVars == 1) {
    out << "<td>gcd polynomial coeffs</td>";
  }
  out << "<td>the hwv</td>";
  if (theNumVars == 1) {
    out << "<td>gcd divided out</td>";
  }
  out << "</tr>";
  std::stringstream latexReport2;
  latexReport2 << "\\begin{longtable}{p{2.5cm}p{2.5cm}p{1.5cm}l}\\caption{\\label{tableDecompo"
  << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.toString() << "times7dim}Decomposition for the $"
  << theGenMod.parabolicSelectionNonSelectedAreElementsLevi.toString()
  << "$-parabolic subalgebra $\\bar{\\mathfrak{p}}$ } \\\\ Weight & Projector applied to &"
  << " Extra multiplier & Resulting $\\bar {\\mathfrak b}$-singular vector \\endhead\\hline";
  for (int i = 0; i < theCentralCharacters.size; i ++) {
    Vector<RationalFunction> currentWeightSimpleCoords =
    theSSalgebra.content->theWeyl.GetSimpleCoordinatesFromFundamental(theEigenVectorWeightsFund[i]);
    tempElt.MakeHWV(theFDMod, RFOne);
    tempElt.MultiplyOnTheLeft(theLeviEigenVectors[i], theElt, *theSSalgebra.content, RFOne);
    tempElt.MakeHWV(theGenMod, RFOne);
    theElt.TensorOnTheRight(tempElt);
    theElt *= - 1;
    std::string startingEltString = theElt.toString(&tempFormat);
    std::stringstream tempStream, tempStream2;
    tempStream << "\\begin{array}{l}";
    bool found = false;
    for (int j = 0; j < theCentralCharacters.size; j ++) {
      Vector<RationalFunction> otherWeightSimpleCoords =
      theSSalgebra.content->theWeyl.GetSimpleCoordinatesFromFundamental(theEigenVectorWeightsFund[j]);
      if ((otherWeightSimpleCoords - currentWeightSimpleCoords).IsPositive()) {
        theCasimirMinusChar = theCasimir;
        theCasimirMinusChar -= theCentralCharacters[j];
        theElt.MultiplyOnTheLeft(theCasimirMinusChar, tempElt2, *theSSalgebra.content, RFOne);
        theElt = tempElt2;
        tempStream << "(i(\\bar c)- (" << theCentralCharacters[j].toString() << ") )\\\\";
        tempStream2 << " $(\\bar c-p_" << j + 1 << ") $ ";
        found = true;
      }
    }
    if (found) {
      tempStream << "\\cdot";
      tempStream2 << " $\\cdot$ ";
    }
    tempStream << "(" << startingEltString << ")";
    tempStream << "\\end{array}";
    tempStream2 << " $(" << startingEltString << ")$ ";
    RationalFunction scale = theElt.scaleNormalizeLeadingMonomial();
    Rational tempRat;
    if (!scale.IsConstant(&tempRat)) {
      global.fatal << "Unexpected: scale not rational" << global.fatal;
    }
    currentHWsimplecoords = theGenMod.theHWSimpleCoordSBaseField;
    currentHWsimplecoords += theFDMod.theModuleWeightsSimpleCoords[i];
    out << "<tr><td>"
    << theSSalgebra.content->theWeyl.GetFundamentalCoordinatesFromSimple(currentHWsimplecoords).ToStringLetterFormat("\\psi")
    << "</td><td>" << HtmlRoutines::GetMathMouseHover(tempStream.str()) << "</td><td>" << tempRat.toString() << "</td>";
    latexReport2
    << "$" << theSSalgebra.content->theWeyl.GetFundamentalCoordinatesFromSimple(currentHWsimplecoords).ToStringLetterFormat("\\psi")
    << "$ &  " << tempStream2.str() << " &" << tempRat.toString();
    Polynomial<Rational> tmpGCD, tmpRF;
    tempFormat.MaxLineLength = 80;
    if (theNumVars == 1) {
      scale = theElt.scaleNormalizeLeadingMonomial();
      scale.GetNumerator(tmpGCD);
      tmpGCD.scaleNormalizeLeadingMonomial();
      out << "<td>" << HtmlRoutines::GetMathMouseHover(tmpGCD.toString(&tempFormat)) << "</td>";
    }
    out << "<td>" << HtmlRoutines::GetMathMouseHover(theElt.toString(&tempFormat)) << "</td>";
    latexReport2 << "&$\\begin{array}{l}" << theElt.toString(&tempFormat) << "\\end{array}$\\\\<br>";
    if (theNumVars == 1) {
      tmpRF = tmpGCD;
      theElt /= tmpRF;
      out << "<td>" << HtmlRoutines::GetMathMouseHover("\\begin{array}{l}" + theElt.toString(&tempFormat) + "\\end{array}") << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  latexReport2 << "\\end{longtable}";
  out << "<br>Ready LaTeX (table 1 and 2): <br><br><br>" << latexReport1.str() << "<br><br><br>" << latexReport2.str() << "<br>";
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerHWTAABF(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerHWTAABF");
  RecursionDepthCounter theRecursionCounter(&theCommands.RecursionDeptH);
  if (!input.IsListNElements(4)) {
    return output.MakeError("Function expects three arguments.", theCommands);
  }
  Expression leftMerged = input[1];
  Expression rightMerged = input[2];
  if (!Expression::MergeContexts(leftMerged, rightMerged)) {
    return theCommands << "Could not get elements of universal enveloping algebra from inputs: " << input[1].toString()
    << " and " << input[2].toString();
  }
  Expression finalContext = rightMerged.GetContext();
  int algebraIndex = finalContext.ContextGetIndexAmbientSSalg();
  if (algebraIndex == - 1) {
    return output.MakeError("I couldn't extract a Lie algebra to compute hwtaabf.", theCommands);
  }
  SemisimpleLieAlgebra& constSSalg = theCommands.theObjectContainer.semisimpleLieAlgebras.theValues[algebraIndex];
  const Expression& weightExpression = input[3];
  Vector<RationalFunction> weight;
  if (!theCommands.GetVectoR<RationalFunction>(
    weightExpression,
    weight,
    &finalContext,
    constSSalg.GetRank(),
    CalculatorConversions::functionRationalFunction
  )) {
    return theCommands
    << "<hr>Failed to obtain highest weight from the third argument which is "
    << weightExpression.toString();
  }
  if (!leftMerged.SetContextAtLeastEqualTo(finalContext) || !rightMerged.SetContextAtLeastEqualTo(finalContext)) {
    return output.MakeError(
      "Failed to merge the contexts of the highest weight and the elements of the Universal enveloping. ",
      theCommands
    );
  }
  Expression leftConverted, rightConverted;
  if (!leftMerged.ConvertInternally<ElementUniversalEnveloping<RationalFunction> >(leftConverted)) {
    return false;
  }
  if (!rightMerged.ConvertInternally<ElementUniversalEnveloping<RationalFunction> >(rightConverted)) {
    return false;
  }
  const ElementUniversalEnveloping<RationalFunction>& leftUE = leftConverted.GetValue<ElementUniversalEnveloping<RationalFunction> >();
  const ElementUniversalEnveloping<RationalFunction>& rightUE = rightConverted.GetValue<ElementUniversalEnveloping<RationalFunction> >();
  WeylGroupData& theWeyl = constSSalg.theWeyl;
  Vector<RationalFunction> hwDualCoords;
  constSSalg.OrderSSalgebraForHWbfComputation();
  hwDualCoords = theWeyl.GetDualCoordinatesFromFundamental(weight);
  RationalFunction outputRF;
  if (!leftUE.HWTAAbilinearForm(rightUE, outputRF, &hwDualCoords, 1, 0, &theCommands.Comments)) {
    return output.MakeError("Error: couldn't compute Shapovalov form, see comments.", theCommands);
  }
  constSSalg.OrderStandardAscending();
  return output.AssignValueWithContext(outputRF, finalContext, theCommands);
}

bool CalculatorFunctions::innerSplitFDpartB3overG2CharsOutput(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  branchingData& theG2B3Data
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSplitFDpartB3overG2CharsOutput");
  Expression theContext(theCommands);
  CalculatorFunctions::innerSplitFDpartB3overG2Init(theCommands, input, output, theG2B3Data, theContext);
  if (output.IsError()) {
    return true;
  }
  std::stringstream out;
  out << "<br>Highest weight: " << theG2B3Data.theWeightFundCoords.toString() << "<br>Parabolic selection: "
  << theG2B3Data.selInducing.toString();
  std::string report;
  charSSAlgMod<RationalFunction> tempChar;
  charSSAlgMod<RationalFunction> startingChar;
  Vector<RationalFunction> simpleWeight;
  simpleWeight = theG2B3Data.theHmm.theRange().theWeyl.GetSimpleCoordinatesFromFundamental(theG2B3Data.theWeightFundCoords);
  startingChar.MakeFromWeight(simpleWeight, &theG2B3Data.theHmm.theRange());
  startingChar.SplitCharOverRedSubalg(&report, tempChar, theG2B3Data);
  out << report;
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerPrintB3G2branchingTableInit(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  branchingData& theG2B3data,
  int& desiredHeight,
  Expression& outputContext
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPrintB3G2branchingTableInit");
  if (input.size() != 3) {
    return output.MakeError("I need two arguments: first is height, second is parabolic selection. ", theCommands);
  }
  desiredHeight = 0;
  if (!input[1].IsSmallInteger(&desiredHeight)) {
    return output.MakeError("the first argument must be a small integer", theCommands);
  }
  if (desiredHeight < 0) {
    desiredHeight = 0;
  }
  const Expression& weightNode = input[2];
  CalculatorFunctions::innerSplitFDpartB3overG2Init(theCommands, weightNode, output, theG2B3data, outputContext);
  if (output.IsError()) {
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerSplitFDpartB3overG2(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSplitFDpartB3overG2");
  branchingData theG2B3Data;
  Expression theContext(theCommands);
  CalculatorFunctions::innerSplitFDpartB3overG2Init(theCommands, input, output, theG2B3Data, theContext);
  if (output.IsError()) {
    return true;
  }
  Vectors<RationalFunction> theHWs;
  theHWs.addOnTop(theG2B3Data.theWeightFundCoords);
  return theCommands.innerPrintB3G2branchingIntermediate(theCommands, input, output, theHWs, theG2B3Data, theContext);
}

bool Calculator::innerSplitFDpartB3overG2CharsOnly(Calculator& theCommands, const Expression& input, Expression& output) {
  branchingData theG2B3Data;
  return CalculatorFunctions::innerSplitFDpartB3overG2CharsOutput(theCommands, input, output, theG2B3Data);
}

bool Calculator::innerPrintB3G2branchingTableCommon(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  Vectors<RationalFunction>& outputHWs,
  branchingData& theG2B3Data,
  Expression& theContext
) {
  MacroRegisterFunctionWithName("Calculator::innerPrintB3G2branchingTableCommon");
  Vector<RationalFunction> theHWrf;
  SelectionWithMaxMultiplicity theHWenumerator;
  int desiredHeight = 0;
  if (!CalculatorFunctions::innerPrintB3G2branchingTableInit(
    theCommands, input, output, theG2B3Data, desiredHeight, theContext
  )) {
    return false;
  }
  if (output.IsError()) {
    return true;
  }
  Selection invertedSelInducing = theG2B3Data.selInducing;
  theContext.ContextGetFormatExpressions(theG2B3Data.theFormat);
  invertedSelInducing.InvertSelection();
  outputHWs.setSize(0);
  for (int j = 0; j <= desiredHeight; j ++) {
    theHWenumerator.initMaxMultiplicity(3 - theG2B3Data.selInducing.CardinalitySelection, j);
    theHWenumerator.IncrementSubsetFixedCardinality(j);
    int numCycles = theHWenumerator.NumCombinationsOfCardinality(j);
    for (int i = 0; i < numCycles; i ++, theHWenumerator.IncrementSubsetFixedCardinality(j)) {
      theHWrf = theG2B3Data.theWeightFundCoords;
      for (int k = 0; k < invertedSelInducing.CardinalitySelection; k ++)
        theHWrf[invertedSelInducing.elements[k]] += theHWenumerator.Multiplicities[k];
      outputHWs.addOnTop(theHWrf);
    }
  }
  return true;
}

bool Calculator::innerSplitFDpartB3overG2old(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerSplitFDpartB3overG2old");
  branchingData theG2B3Data;
  CalculatorFunctions::innerSplitFDpartB3overG2CharsOutput(theCommands, input, output, theG2B3Data);
  if (output.IsError()) {
    return true;
  }
  std::stringstream out;
  theCommands.innerSplitFDpartB3overG2inner(theCommands, theG2B3Data, output);
  out << "<br>Highest weight: " << theG2B3Data.theWeightFundCoords.toString() << "<br>Parabolic selection: "
  << theG2B3Data.selInducing.toString() << "<br>common Levi part of G_2 and B_3: "
  << theG2B3Data.selSmallParSel.toString();
  out
  << "<table border =\"1\"><tr><td>word</td><td>B_3-weight simple coords</td><td>B_3-weight fund. coords </td>"
  << "<td>G_2 simple coordinates</td><td>G2-fund. coords</td><td>G2-dual coordinates</td><td>character</td></tr>";
  std::stringstream readyForLatexConsumptionTable1;

  readyForLatexConsumptionTable1 << "\\hline\\multicolumn{3}{|c|}{Highest weight $ "
  <<  theG2B3Data.theWeightFundCoords.ToStringLetterFormat("\\omega")
  << "$}\\\\ weight fund. coord.& singular vector& weight proj. $\\bar h^*$ \\\\\\hline\n<br> ";
  for (int i = 0; i < theG2B3Data.outputWeightsSimpleCoords.size; i ++) {
    Vector<RationalFunction>& currentWeightSimpleB3coords = theG2B3Data.outputWeightsSimpleCoords[i];
    Vector<RationalFunction>& currentWeightFundB3coords = theG2B3Data.outputWeightsFundCoordS[i];
    Vector<RationalFunction>& currentG2Weight = theG2B3Data.g2Weights[i];
    Vector<RationalFunction>& currentG2DualWeight = theG2B3Data.g2DualWeights[i];
    readyForLatexConsumptionTable1 << "$" << currentWeightFundB3coords.ToStringLetterFormat("\\omega")
    << " $ & $" << theG2B3Data.leviEigenSpace[i].ToStringLetterFormat("m")
    << " $ & $ " << currentG2Weight.ToStringLetterFormat("\\alpha") << " $ \\\\\n<br>";
    out << "<tr><td>" << theG2B3Data.outputEigenWords[i].toString() << "</td><td> "
    << currentWeightSimpleB3coords.toString() << "</td><td> " << currentWeightFundB3coords.toString()
    << "</td><td>" << currentG2Weight.ToStringLetterFormat("\\alpha") << "</td><td> "
    << theG2B3Data.theHmm.theDomain().theWeyl.GetFundamentalCoordinatesFromSimple(currentG2Weight).toString()
    << "</td><td> " << currentG2DualWeight.toString() << "</td>";
    out << "<td>" << HtmlRoutines::GetMathSpanPure(theG2B3Data.theChars[i].toString()) << "</td>";
    out << "</tr>";
  }
  readyForLatexConsumptionTable1 << "\\hline \n";
  out << "</table>";
  out << "<br>Ready for LaTeX consumption: ";
  out << "<br><br>" << readyForLatexConsumptionTable1.str() << "<br><br>";
  out << "<table border =\"1\"><tr><td>weight</td><td>the elt closed form</td><td>the elt</td></tr>";
  Vector<RationalFunction> weightDifference;
  std::stringstream formulaStream1;
  for (int k = 0; k < theG2B3Data.g2Weights.size; k ++) {
    out << "<tr><td>" << theG2B3Data.g2Weights[k].toString() << "</td><td>";
    for (int j = 0; j < theG2B3Data.g2Weights.size; j ++) {
      weightDifference = theG2B3Data.g2Weights[j] - theG2B3Data.g2Weights[k];
      if (weightDifference.IsPositive()) {
        formulaStream1 << "(12(i(\\bar c) - " << theG2B3Data.theChars[j].toString() <<  "))";
      }
    }
    formulaStream1 << "v_\\lambda";
    out << HtmlRoutines::GetMathSpanPure(formulaStream1.str())
    << "</td><td>" << HtmlRoutines::GetMathSpanPure(theG2B3Data.theEigenVectorS[k].toString()) << "</td></tr>";
  }
  out << "</table>";
  out << "<br>Time final: " << global.GetElapsedSeconds();
  return output.AssignValue(out.str(), theCommands);
}

bool Expression::EvaluatesToDoubleInRange(
  const std::string& varName,
  double lowBound,
  double highBound,
  int numIntervals,
  double* outputYmin,
  double* outputYmax,
  Vectors<double>* outputPoints
) const {
  MacroRegisterFunctionWithName("Expression::EvaluatesToDoubleInRange");
  if (numIntervals < 1 || this->owner == nullptr) {
    return false;
  }
  HashedList<Expression> knownEs = this->owner->knownDoubleConstants;
  List<double> knownValues = this->owner->knownDoubleConstantValues;
  Expression theVarNameE;
  theVarNameE.MakeAtom(varName, *this->owner);
  if (knownEs.Contains(theVarNameE)) {
    return *(this->owner) << "Variable name is an already known constant, variable name is bad.";
  }
  knownEs.addOnTop(theVarNameE);
  knownValues.addOnTop(0);
  int numPoints = numIntervals + 1;
  double delta = (highBound - lowBound) / (numIntervals);
  *knownValues.LastObject() = lowBound;
  double currentValue = 0;
  if (outputPoints != nullptr) {
    outputPoints->setSize(numPoints);
  }
  bool result = true;
  int numFailedEvaluations = 0;
  for (int i = 0; i < numPoints; i ++) {
    if (i == numPoints - 1) {
      *knownValues.LastObject() = highBound; //correcting for floating point errors.
    }
    if (!this->EvaluatesToDoubleUnderSubstitutions(knownEs, knownValues, &currentValue)) {
      numFailedEvaluations ++;
      if (numFailedEvaluations < 5) {
        *(this->owner) << "<br>Failed to evaluate " << this->toString() << " at " << varName << "="
        << *knownValues.LastObject() << ". ";
      }
      if (numFailedEvaluations == 5) {
        *(this->owner) << "<br>...";
      }
      result = false;
      if (outputPoints != nullptr) {
        (*outputPoints)[i].setSize(2);
        (*outputPoints)[i][0] = *knownValues.LastObject();
        (*outputPoints)[i][1] = std::nan("");
      }
      *knownValues.LastObject() += delta;
      continue;
    }
    if (outputPoints != nullptr) {
      (*outputPoints)[i].setSize(2);
      (*outputPoints)[i][0] = *knownValues.LastObject();
      (*outputPoints)[i][1] = currentValue;
    }
    *knownValues.LastObject() += delta;
    if (outputYmin != nullptr) {
      if (i == 0) {
        *outputYmin = currentValue;
      } else {
        *outputYmin = MathRoutines::Minimum(currentValue, *outputYmin);
      }
    }
    if (outputYmax != nullptr) {
      if (i == 0) {
        *outputYmax = currentValue;
      } else {
        *outputYmax = MathRoutines::Maximum(currentValue, *outputYmax);
      }
    }
  }
  if (numFailedEvaluations >= 5) {
    (*this->owner) << "<br>" << numFailedEvaluations << " evaluations total failed. ";
  }
  return result;
}

bool Expression::EvaluatesToDouble(double* whichDouble) const {
  return this->EvaluatesToDoubleUnderSubstitutions(
    this->owner->knownDoubleConstants, this->owner->knownDoubleConstantValues, whichDouble
  );
}

bool Expression::EvaluatesToDoubleUnderSubstitutions(
  const HashedList<Expression>& knownEs,
  const List<double>& valuesKnownEs,
  double* whichDouble
) const {
  MacroRegisterFunctionWithName("Expression::EvaluatesToDoubleUnderSubstitutions");
  if (this->owner == nullptr) {
    return false;
  }
  Calculator& theCommands = *this->owner;
  if (this->IsOfType<double>(whichDouble)) {
    return true;
  }
  if (this->IsOfType<Rational>()) {
    if (whichDouble != nullptr) {
      *whichDouble = this->GetValue<Rational>().GetDoubleValue();
    }
    return true;
  }
  if (this->IsOfType<AlgebraicNumber>()) {
    if (this->GetValue<AlgebraicNumber>().EvaluatesToDouble(whichDouble)) {
      return true;
    }
  }
  RecursionDepthCounter theCounter(&this->owner->RecursionDeptH);
  if (this->owner->RecursionDeptH >this->owner->MaxRecursionDeptH) {
    return *(this->owner) << "<hr>Recursion depth exceeded while evaluating innerEvaluateToDouble."
    << " This may be a programming error. ";
  }
  if (knownEs.Contains(*this)) {
    if (whichDouble != nullptr) {
      *whichDouble = valuesKnownEs[knownEs.GetIndex(*this)];
    }
    return true;
  }
  bool isArithmeticOperationTwoArguments =
  this->StartsWith(theCommands.opTimes(), 3) ||
  this->StartsWith(theCommands.opPlus(), 3) ||
  this->StartsWith(theCommands.opMinus(), 3) ||
  this->StartsWith(theCommands.opThePower(), 3) ||
  this->StartsWith(theCommands.opDivide(), 3) ||
  this->StartsWith(theCommands.opSqrt(), 3) ||
  this->StartsWith(theCommands.opLogBase(), 3);
  if (isArithmeticOperationTwoArguments) {
    double leftD, rightD;
    if (
      !(*this)[1].EvaluatesToDoubleUnderSubstitutions(knownEs, valuesKnownEs, &leftD) ||
      !(*this)[2].EvaluatesToDoubleUnderSubstitutions(knownEs, valuesKnownEs, &rightD)
    ) {
      return false;
    }
    if ((*this).StartsWith(theCommands.opTimes(), 3)) {
      if (whichDouble != nullptr) {
        *whichDouble = leftD * rightD;
      }
      return true;
    }
    if ((*this).StartsWith(theCommands.opPlus(), 3)) {
      if (whichDouble != nullptr) {
        *whichDouble = leftD + rightD;
      }
      return true;
    }
    if ((*this).StartsWith(theCommands.opMinus(), 3)) {
      if (whichDouble != nullptr) {
        *whichDouble = leftD - rightD;
      }
      return true;
    }
    if ((*this).StartsWith(theCommands.opLogBase(), 3)) {
      if (leftD <= 0 || rightD <= 0) {
        return false;
      }
      if (leftD - 1 == 0.0) {
        return false;
      }
      if (whichDouble != nullptr) {
        *whichDouble = (FloatingPoint::Log(rightD)) / (FloatingPoint::Log(leftD));
      }
      return true;
    }
    if ((*this).StartsWith(theCommands.opThePower(), 3)) {
      bool signChange = false;
      if (leftD < 0) {
        Rational theRat;
        if ((*this)[2].IsRational(&theRat)) {
          if (!theRat.GetDenominator().IsEven()) {
            if (!theRat.GetNumerator().IsEven()) {
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
        *whichDouble = FloatingPoint::Power(leftD, rightD);
      }
      if (signChange) {
        *whichDouble *= - 1;
      }
      return !std::isnan(*whichDouble) && !std::isinf(*whichDouble);
    }
    if ((*this).StartsWith(theCommands.opSqrt(), 3)) {
      bool signChange = false;
      if (rightD < 0) {
        Rational theRat;
        if ((*this)[1].IsRational(&theRat)) {
          if (!theRat.GetNumerator().IsEven()) {
            if (!theRat.GetDenominator().IsEven()) {
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
        *whichDouble = FloatingPoint::Power(rightD, 1 / leftD);
      }
      if (signChange) {
        *whichDouble *= - 1;
      }
      return !std::isnan(*whichDouble) && !std::isinf(*whichDouble);
    }
    if ((*this).StartsWith(theCommands.opDivide(), 3)) {
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
  this->StartsWith(theCommands.opSin(), 2) ||
  this->StartsWith(theCommands.opCos(), 2) ||
  this->StartsWith(theCommands.opTan(), 2) ||
  this->StartsWith(theCommands.opCot(), 2) ||
  this->StartsWith(theCommands.opCsc(), 2) ||
  this->StartsWith(theCommands.opSec(), 2) ||
  this->StartsWith(theCommands.opArcTan(), 2) ||
  this->StartsWith(theCommands.opArcCos(), 2) ||
  this->StartsWith(theCommands.opArcSin(), 2) ||
  this->StartsWith(theCommands.opSqrt(), 2) ||
  this->StartsWith(theCommands.opLog(), 2) ||
  this->StartsWith(theCommands.opAbsoluteValue(), 2);

  if (isKnownFunctionOneArgument) {
    double argumentD;
    if (!(*this)[1].EvaluatesToDoubleUnderSubstitutions(knownEs, valuesKnownEs, &argumentD)) {
      return false;
    }
    if (this->StartsWith(theCommands.opSqrt())) {
      if (argumentD < 0) {
        return false;
      }
      if (whichDouble != nullptr) {
        *whichDouble = FloatingPoint::Sqrt(argumentD);
      }
    }
    if (this->StartsWith(theCommands.opAbsoluteValue())) {
      if (whichDouble != nullptr) {
        if (argumentD < 0) {
          *whichDouble = - argumentD;
        } else {
          *whichDouble = argumentD;
        }
      }
    }
    if (this->StartsWith(theCommands.opArcCos())) {
      if (argumentD > 1 || argumentD < - 1) {
        return false;
      }
      if (whichDouble != nullptr) {
        *whichDouble = FloatingPoint::Arccos(argumentD);
      }
    }
    if (this->StartsWith(theCommands.opArcSin())) {
      if (argumentD > 1 || argumentD < - 1) {
        return false;
      }
      if (whichDouble != nullptr) {
        *whichDouble = FloatingPoint::Arcsin(argumentD);
      }
    }
    if (this->StartsWith(theCommands.opSin())) {
      if (whichDouble != nullptr) {
        *whichDouble = FloatingPoint::Sin(argumentD);
      }
    }
    if (this->StartsWith(theCommands.opCos())) {
      if (whichDouble != nullptr) {
        *whichDouble = FloatingPoint::Cos(argumentD);
      }
    }
    if (this->StartsWith(theCommands.opTan())) {
      if (whichDouble != nullptr) {
        double denominator = FloatingPoint::Cos(argumentD);
        if (denominator == 0.0) {
          return false;
        }
        *whichDouble = FloatingPoint::Sin(argumentD) / denominator;
      }
    }
    if (this->StartsWith(theCommands.opCot())) {
      if (whichDouble != nullptr) {
        double denominator = FloatingPoint::Sin(argumentD);
        if (denominator == 0.0) {
          return false;
        }
        *whichDouble = FloatingPoint::Cos(argumentD) / denominator;
      }
    }
    if (this->StartsWith(theCommands.opCsc())) {
      if (whichDouble != nullptr) {
        double denominator = FloatingPoint::Sin(argumentD);
        if (denominator == 0.0) {
          return false;
        }
        *whichDouble = 1 / denominator;
      }
    }
    if (this->StartsWith(theCommands.opSec())) {
      if (whichDouble != nullptr) {
        double denominator = FloatingPoint::Cos(argumentD);
        if (denominator == 0.0) {
          return false;
        }
        *whichDouble = 1 / denominator;
      }
    }
    if (this->StartsWith(theCommands.opArcTan())) {
      if (whichDouble != nullptr) {
        *whichDouble = FloatingPoint::Arctan(argumentD);
      }
    }
    if (this->StartsWith(theCommands.opLog())) {
      if (argumentD <= 0) {
        return false;
      }
      if (whichDouble != nullptr) {
        *whichDouble = FloatingPoint::Log(argumentD);
      }
    }
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerTestTopCommand(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestTopCommand");
  (void) input;
  (void) theCommands;//portable way of avoiding unused parameter warning
  if (!global.UserDefaultHasAdminRights()) {
    return theCommands << "Test top command available only to logged-in admins. ";
  }
  return output.AssignValue(global.ToStringHTMLTopCommandLinuxSystem(), theCommands);
}

bool CalculatorFunctions::innerTestIndicator(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestIndicator");
  if (global.theResponse.flagBanProcessMonitorinG) {
    std::stringstream out;
    out << "The server's admins have explicitly banned monitoring. ";
    return output.AssignValue(out.str(), theCommands);
  }
  if (!global.theResponse.flagReportDesired) {
    std::stringstream out;
    out << "Process monitoring turned off by user. ";
    return output.AssignValue(out.str(), theCommands);
  }
  if (input.size() < 3) {
    return theCommands
    << "Testing indicator requires two arguments: "
    << "number of iterations and size of dummy comment. ";
  }
  int numRuns = - 1;
  if (!input[1].IsIntegerFittingInInt(&numRuns)) {
    return theCommands << "Argument of CalculatorFunctions::innerTestIndicator "
    << "is not a small integer, instead it equals: " << input[1] << ".";
  }
  if (numRuns > 200000) {
    theCommands << "The argument " << numRuns
    << " of CalculatorFunctions::innerTestIndicator larger than 200000, trimming down to 200000.";
    numRuns = 200000;
  }
  if (numRuns < 0) {
    numRuns = 0;
  }
  int dummyCommentSize = 0;
  if (!input[2].IsIntegerFittingInInt(&dummyCommentSize)) {
    return theCommands << "Second argument needs to be an integer. ";
  }
  if (dummyCommentSize > 200000) {
    theCommands << "Dummy comment size of " << dummyCommentSize << " reduced to 200000.";
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
  global.theResponse.Initiate("Triggered by test indicator. ");
  ProgressReport theReport;
  for (int i = 0; i < numRuns; i ++) {
    std::stringstream reportStream;
    reportStream << " Running indicator test, " << i + 1 << " out of " << numRuns << ".";
    theReport.Report(reportStream.str());
    global.FallAsleep(4000);
  }
  std::stringstream out;
  out << numRuns << " iterations of the indicator test executed. ";
  if (dummyCommentSize > 0) {
    theCommands << "Dummy comment:<br>" << dummyComment;
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerRootSAsAndSltwos(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  bool showSLtwos,
  bool MustRecompute
) {
  MacroRegisterFunctionWithName("Calculator::innerRootSAsAndSltwos");
  if (input.size() != 2) {
    return theCommands << "Root subalgebra / sl(2)-subalgebras function expects 1 argument. ";
  }
  WithContext<SemisimpleLieAlgebra*> ownerSS;
  if (!theCommands.Convert(
    input[1],
    CalculatorConversions::functionSemisimpleLieAlgebra,
    ownerSS
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  FormatExpressions theFormat;
  theFormat.flagUseHTML = true;
  theFormat.flagUseLatex = false;
  theFormat.flagUsePNG = true;

  std::stringstream outRootHtmlFileName, outRootHtmlDisplayName, outSltwoMainFile, outSltwoFileDisplayName;

  std::string displayFolder = ownerSS.content->ToStringDisplayFolderName("");

  outSltwoMainFile << displayFolder << ownerSS.content->ToStringFileNameRelativePathSlTwoSubalgebras();
  outRootHtmlFileName << displayFolder << ownerSS.content->ToStringFileNameNoPathRootSubalgebras();
  outRootHtmlDisplayName << displayFolder << ownerSS.content->ToStringFileNameNoPathRootSubalgebras();
  if (
    !FileOperations::FileExistsVirtual(outSltwoMainFile.str()) ||
    !FileOperations::FileExistsVirtual(outRootHtmlFileName.str())
  ) {
    MustRecompute = true;
  }
  std::stringstream out;
  if (MustRecompute) {
    SltwoSubalgebras theSl2s(*ownerSS.content);
    theSl2s.theRootSAs.flagPrintParabolicPseudoParabolicInfo = true;
    ownerSS.content->FindSl2Subalgebras(*ownerSS.content, theSl2s);
    theSl2s.ToHTML(&theFormat);
  } else {
    out << "The table is precomputed and served from the hard disk. <br>";
  }
  out << "<a href=\""
  << (showSLtwos ? outSltwoFileDisplayName.str() : outRootHtmlDisplayName.str())
  << "\" target = \"_blank\">"
  << (showSLtwos ? outSltwoFileDisplayName.str() : outRootHtmlDisplayName.str()) << " </a>";
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerCrawlTexFile(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCrawlTexFile");
  if (!global.UserDefaultHasAdminRights()) {
    std::stringstream out;
    out << "Command available to logged-in admins only. ";
    return output.AssignValue(out.str(), theCommands);
  }
  if (!input.IsOfType<std::string>()) {
    return theCommands << "<hr>Input " << input.toString() << " is not of type string. ";
  }
  LaTeXCrawler theCrawler;
  theCrawler.ownerCalculator = &theCommands;
  theCrawler.theFileNameToCrawlRelative = input.GetValue<std::string>();
  theCrawler.Crawl();
  return output.AssignValue(theCrawler.displayResult.str(), theCommands);
}

bool CalculatorFunctions::innerBuildFreecalcSlidesOnTopic(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerBuildFreecalcSlidesOnTopic");
  if (!global.UserDefaultHasAdminRights()) {
    std::stringstream out;
    out << "Command available to logged-in admins only. ";
    return output.AssignValue(out.str(), theCommands);
  }
  (void) input;
  LaTeXCrawler theCrawler;
  theCrawler.flagBuildSingleSlides = true;
  theCrawler.ownerCalculator = &theCommands;
  //theCrawler.topicListToBuild = input.GetValue<std::string>();
  std::stringstream out;
  theCrawler.BuildTopicList(&out, &out);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerBuildFreecalcSingleSlides(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerBuildFreecalcSingleSlides");
  if (!global.UserDefaultHasAdminRights()) {
    std::stringstream out;
    out << "Command available to logged-in admins only. ";
    return output.AssignValue(out.str(), theCommands);
  }
  if (!input.IsOfType<std::string>()) {
    return theCommands << "<hr>Input " << input.toString() << " is not of type string. ";
  }
  LaTeXCrawler theCrawler;
  theCrawler.flagBuildSingleSlides = true;
  theCrawler.ownerCalculator = &theCommands;
  theCrawler.theFileNameToCrawlRelative = input.GetValue<std::string>();
  std::string startingFolder = FileOperations::GetCurrentFolder();
  theCrawler.BuildFreecalC();
  global.ChDir(startingFolder);
  return output.AssignValue(theCrawler.displayResult.str(), theCommands);
}

bool CalculatorFunctions::innerBuildFreecalc(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerBuildFreecalc");
  if (!global.UserDefaultHasAdminRights()) {
    std::stringstream out;
    out << "Command available to logged-in admins only. ";
    return output.AssignValue(out.str(), theCommands);
  }
  if (!input.IsOfType<std::string>()) {
    return theCommands << "<hr>Input " << input.toString() << " is not of type string. ";
  }
  LaTeXCrawler theCrawler;
  theCrawler.flagBuildSingleSlides = false;
  theCrawler.ownerCalculator = &theCommands;
  theCrawler.theFileNameToCrawlRelative = input.GetValue<std::string>();
  std::string startingFolder = FileOperations::GetCurrentFolder();
  theCrawler.BuildFreecalC();
  global.ChDir(startingFolder);
  return output.AssignValue(theCrawler.displayResult.str(), theCommands);
}

bool CalculatorFunctions::innerFindProductDistanceModN(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFindProductDistanceModN");
  if (input.children.size != 3) {
    return theCommands << "<hr>Product distance f-n takes as input 2 arguments, modulo and a list of integers";
  }
  const Expression& theModuloE = input[1];
  const Expression& theIntegersE = input[2];
  int theSize;
  if (!theModuloE.IsIntegerFittingInInt(&theSize)) {
    return theCommands << " <hr> Failed to extract modulus from " << theModuloE.toString();
  }
  if (theSize == 0) {
    return theCommands << "<hr>Zero modulus not allowed.";
  }
  if (theSize < 0) {
    theSize *= - 1;
  }
  if (theSize > 10000000) {
    return theCommands << "<hr>I've been instructed to compute with moduli no larger than 10000000.";
  }
  List<int> theInts, theIntsReduced;
  if (!theCommands.GetVectoRInt(theIntegersE, theInts)) {
    return theCommands << "<hr>Failed to extract integer list from " << theIntegersE.toString();
  }
  theIntsReduced.setSize(theInts.size);
  for (int i = 0; i < theInts.size; i ++) {
    if (theInts[i] <= 0) {
      return theCommands << "<hr>The integer list " << theInts << " contains non-positive numbers. ";
    }
    theIntsReduced[i] = theInts[i] % theSize;
  }
  List<LargeIntegerUnsigned> theList;
  List<int> theIndexStack;
  theList.initializeFillInObject(theSize, 0);
  theIndexStack.Reserve(theSize);
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
      if (!currentIndexLarge.IsIntegerFittingInInt(&currentIndex)) {
        return theCommands << "An internal check has failed. "
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
            theReport.Report(out.str());
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
        theReport.Report(out.str());
      }
    }
    if (theIndexStack.size > 1000000000) {
      return theCommands << "While computing product distance, exceeded allowed stack size of 1000000000";
    }
  }
  std::stringstream out;
  for (int i = 0; i < theList.size; i ++) {
    out << "<br>" << i << ": " << theList[i].toString();
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerSolveProductSumEquationOverSetModN(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSolveProductSumEquationOverSetModN");
  Expression theModuloE, theIntegersE, theProductE, theSumE;
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "theMod", theModuloE)) {
    return theCommands << "<hr>Value theMod not found.";
  }
  int theMod;
  if (!theModuloE.IsIntegerFittingInInt(&theMod)) {
    return theCommands << " <hr> Failed to extract modulus from " << theModuloE.toString();
  }
  if (theMod == 0) {
    return theCommands << "<hr>Zero modulus not allowed.";
  }
  if (theMod < 0) {
    theMod *= - 1;
  }
  if (theMod > 10000000) {
    return theCommands << "<hr>I've been instructed to compute with moduli no larger than 10000000.";
  }
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "theSet", theIntegersE)) {
    return theCommands << "<hr>Value theSet not found.";
  }
  List<int> theInts;
  if (!theCommands.GetVectoRInt(theIntegersE, theInts)) {
    return theCommands << "<hr>Failed to extract integer list from " << theIntegersE.toString();
  }
  for (int i = 0; i < theInts.size; i ++) {
    if (theInts[i] <= 0) {
      return theCommands << "<hr>The integer list " << theInts << " contains non-positive numbers.";
    }
  }
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "theProduct", theProductE)) {
    return theCommands << "<hr>Value theProduct not found. ";
  }
  LargeInteger goalProduct;
  if (!theProductE.IsInteger(&goalProduct)) {
    return theCommands << "<hr>Failed to extract integer from " << theProductE.toString();
  }
  if (goalProduct.IsNegative()) {
    return theCommands << "<hr>I am expecting a positive product as input. ";
  }
  if (!CalculatorConversions::innerLoadKey(theCommands, input, "theSum", theSumE)) {
    return theCommands << "<hr>Value theSum not found. ";
  }
  int theSum = - 1;
  if (!theSumE.IsSmallInteger(&theSum)) {
    return theCommands << "Failed to extract small integer from " << theSumE.toString();
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
  if (!thePartition.init(theOneDimVectors, thePartition.goalVector)) {
    return theCommands << "Failed to initialize the computation. ";
  }
  LargeIntegerUnsigned theModLarge;
  theModLarge = static_cast<unsigned>(theMod);
  int numTestedSoFar = 0;
  ProgressReport theReport;
  LargeIntegerUnsigned oneUI = 1;
  while (thePartition.IncrementReturnFalseIfPastLast()) {
    LargeIntegerUnsigned theProduct = 1;
    for (int i = 0; i < thePartition.currentPartition.size; i ++) {
      LargeIntegerUnsigned theNumber = static_cast<unsigned>(theInts[i]);
      MathRoutines::RaiseToPower(theNumber, thePartition.currentPartition[i], oneUI);
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
      return output.AssignValue(out.str(), theCommands);
    }
    numTestedSoFar ++;
    std::stringstream reportStream;
    reportStream << numTestedSoFar << " tested so far ...";
    theReport.Report(reportStream.str());
  }
  return output.AssignValue(std::string("Couldn't find solution"), theCommands);
}

void Calculator::Test::CalculatorTestPrepare() {
  MacroRegisterFunctionWithName("Calculator::Test::CalculatorTestPrepare");
  if (this->owner == nullptr) {
    global.fatal << "Non-initialized calculator test. " << global.fatal;
  }
  this->commands.Clear();
  for (int i = 0; i < this->owner->NumPredefinedAtoms; i ++) {
    MemorySaving<Calculator::OperationHandlers>& currentPointer = this->owner->operations.theValues[i];
    if (currentPointer.IsZeroPointer()) {
      continue;
    }
    Calculator::OperationHandlers& current = currentPointer.GetElement();
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
        this->commands.SetKeyValue(oneTest.command, oneTest);
      }
      currentHandler = &current.compositeHandlers;
    }
  }
}

bool Calculator::Test::CalculatorTestRun() {
  MacroRegisterFunctionWithName("Calculator::Test::CalculatorTestRun");
  this->debugFlagAtStart = global.GetWebInput(WebAPI::request::debugFlag);
  global.SetWebInpuT(WebAPI::request::debugFlag, "false");
  this->CalculatorTestPrepare();
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
    reportStream << "<br>Test progress: testing " << i + 1 << " out of " << this->commands.size() << ". ";
    reportStream << "<br>Testing expression:<br> " << currentTest.command;
    global << logger::green << "Automated test: " << i << " out of "
    << this->commands.size() << ", atom: " << currentTest.command << logger::endL;
    theReport.Report(reportStream.str());
    theTester.initialize();
    theTester.CheckConsistencyAfterInitialization();
    theTester.Evaluate(currentTest.command);
    currentTest.actualResult = theTester.theProgramExpression.toString(&theFormat);
    reportStream << "<br>Result: " << theTester.theProgramExpression.toString();
    reportStream << "<br>Done in: " << global.GetElapsedSeconds() - this->startTime << " seconds. ";
    theReport.Report(reportStream.str());
  }
  global.SetWebInpuT(WebAPI::request::debugFlag, this->debugFlagAtStart);
  return this->ProcessResults();
}

bool CalculatorFunctions::innerPrintRuleStack(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPrintRuleStack");
  (void) input;//portable way of avoiding unused parameter warning
  return output.AssignValue(theCommands.RuleStack.toString(), theCommands);
}

bool CalculatorFunctions::innerCrash(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCrash");
  (void) input;
  global.fatal << "This is a test of the crashing mechanism. "
  << "A file report must have been written. "
  << "The crash file report is not accessible through the calculator's webserver. "
  << "It can be reached locally in the results/crashes folder of the calculator or, "
  << "if running remotely, through an administrator ssh connection. "
  << global.fatal;
  return output.AssignValue(std::string("Crashed succesfully"), theCommands);
}

bool CalculatorFunctions::innerCrashByListOutOfBounds(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCrashByListOutOfBounds");
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
  return output.AssignValue(std::string("Crashing: list out of bounds."), theCommands);
}

bool CalculatorFunctions::innerCrashByVectorOutOfBounds(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCrashByVectorOutOfBounds");
  (void) input;//portable way of avoiding unused parameter warning
  std::vector<int> theVector;
  theVector[1] = 1;
  return output.AssignValue(std::string("Crashing: std::vector out of bounds."), theCommands);
}

bool CalculatorFunctions::innerDrawWeightSupportWithMults(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  if (!input.IsListNElements(3)) {
    return output.MakeError(
      "Error: the function for drawing weight support takes two arguments (type and highest weight)",
      theCommands
    );
  }
  const Expression& typeNode = input[1];
  const Expression& hwNode = input[2];
  WithContext<SemisimpleLieAlgebra*> theSSalgpointer;
  if (!theCommands.Convert(
    typeNode,
    CalculatorConversions::functionSemisimpleLieAlgebra,
    theSSalgpointer
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  Vector<Rational> highestWeightFundCoords;
  Expression theContext;
  if (!theCommands.GetVectoR<Rational>(
    hwNode, highestWeightFundCoords, &theContext, theSSalgpointer.content->GetRank(), nullptr
  )) {
    return output.MakeError("Failed to extract highest weight vector", theCommands);
  }
  Vector<Rational> highestWeightSimpleCoords;
  WeylGroupData& theWeyl = theSSalgpointer.content->theWeyl;
  highestWeightSimpleCoords = theWeyl.GetSimpleCoordinatesFromFundamental(highestWeightFundCoords);
  std::stringstream out;
  charSSAlgMod<Rational> theChar;
  theChar.MakeFromWeight(highestWeightSimpleCoords, theSSalgpointer.content);
  DrawingVariables theDV;
  std::string report;
  theChar.DrawMeWithMults(report, theDV, 10000);
  out << report << theDV.GetHtmlDiv(theWeyl.GetDim());
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerDrawRootSystem(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDrawRootSystem");
  if (input.size() < 2) {
    return theCommands << "DrawRootSystem expects at least 1 argument. ";
  }
  bool hasPreferredProjectionPlane = input.IsListNElements(4);
  WithContext<SemisimpleLieAlgebra*> theAlgPointer;
  if (!theCommands.Convert(
    input[1],
    CalculatorConversions::functionSemisimpleLieAlgebra,
    theAlgPointer
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra& theAlg = *theAlgPointer.content;
  WeylGroupData& theWeyl = theAlg.theWeyl;
  Vectors<Rational> preferredProjectionPlane;
  if (hasPreferredProjectionPlane) {
    preferredProjectionPlane.setSize(2);
    bool isGood = theCommands.GetVectoR(
      input[2],
      preferredProjectionPlane[0],
      nullptr,
      theWeyl.GetDim(),
      nullptr
    ) && theCommands.GetVectoR(
      input[3],
      preferredProjectionPlane[1],
      nullptr,
      theWeyl.GetDim(),
      nullptr
    );
    if (!isGood) {
      return output.MakeError("Failed to convert second or third argument to vector of desired dimension", theCommands);
    }
  }
  std::stringstream out;
  DrawingVariables theDV;
  theWeyl.DrawRootSystem(theDV, true, false, nullptr, true, nullptr);
  if (hasPreferredProjectionPlane) {
    theDV.flagFillUserDefinedProjection = true;
    theDV.FillUserDefinedProjection = preferredProjectionPlane;
  }
  out << theDV.GetHtmlDiv(theWeyl.GetDim());
  return output.AssignValue(out.str(), theCommands);
}

template <class coefficient>
int charSSAlgMod<coefficient>::GetPosNstringSuchThatWeightMinusNalphaIsWeight(
  const Weight<coefficient>& theWeightInFundCoords, const Vector<coefficient>& theAlphaInFundCoords
) {
  MacroRegisterFunctionWithName("charSSAlgMod_coefficient::GetMaxNSuchThatWeightMinusNalphaIsAWeight");
  int result = - 1;
  Weight<coefficient> currentWeight;
  currentWeight = theWeightInFundCoords;
  for (
    ;
    this->theMonomials.Contains(currentWeight);
    result ++, currentWeight.weightFundamentalCoordS -= theAlphaInFundCoords
  ) {
  }
  return result;
}

template <class coefficient>
std::string charSSAlgMod<coefficient>::ToStringFullCharacterWeightsTable() {
  MacroRegisterFunctionWithName("charSSAlgMod_CoefficientType::ToStringFullCharacterWeightsTable");
  std::stringstream out;
  charSSAlgMod<coefficient> outputChar;
  if (!this->FreudenthalEvalMeFullCharacter(outputChar, 10000, nullptr)) {
    out << "Failed to compute the character with highest weight " << this->toString()
    << " I used Fredenthal's formula; likely the computation was too large. ";
    return out.str();
  }
  out << "<table><tr><td>Weight in fund. coords</td><td>simple coords.</td>"
  << "<td>Simple strings</td><td>Simple half-strings</td></tr>";
  Vector<coefficient> outputSimpleStringCoords, outputSimpleHalfStringCoords;
  Vector<coefficient> theSimpleRoot;
  Vector<coefficient> theSimpleRootFundCoords;
  for (int k = 0; k < outputChar.size(); k ++) {
    out << "<tr>";
    out << "<td>" << outputChar[k].weightFundamentalCoordS.toString() << "</td>";
    Vector<coefficient> weightSimple = this->GetOwner()->theWeyl.GetSimpleCoordinatesFromFundamental
    (outputChar[k].weightFundamentalCoordS);
    out << "<td>" << weightSimple.toString() << "</td>";
    outputSimpleStringCoords.makeZero(this->GetOwner()->GetRank());
    outputSimpleHalfStringCoords.makeZero(this->GetOwner()->GetRank());
    for (int j = 0; j < this->GetOwner()->GetRank(); j ++) {
      theSimpleRoot.MakeEi(this->GetOwner()->GetRank(), j);
      theSimpleRootFundCoords =
      this->GetOwner()->theWeyl.GetFundamentalCoordinatesFromSimple(theSimpleRoot);
      outputSimpleStringCoords[j] = outputChar.GetPosNstringSuchThatWeightMinusNalphaIsWeight(
        outputChar[k], theSimpleRootFundCoords
      ) - outputChar.GetPosNstringSuchThatWeightMinusNalphaIsWeight(
        outputChar[k], - theSimpleRootFundCoords
      );
      outputSimpleHalfStringCoords[j] = outputChar.GetPosNstringSuchThatWeightMinusNalphaIsWeight(
        outputChar[k], theSimpleRootFundCoords
      );
    }
    if (outputSimpleStringCoords != outputChar[k].weightFundamentalCoordS) {
      out << "<td><b style =\"color:red\">" << outputSimpleStringCoords.toString() << "</b></td>" ;
    } else {
      out << "<td>" << outputSimpleStringCoords.toString() << "</td>";
    }
    if (outputSimpleHalfStringCoords != outputChar[k].weightFundamentalCoordS) {
      out << "<td><b style =\"color:red\">" << outputSimpleHalfStringCoords.toString() << "</b></td>" ;
    } else {
      out << "<td>" << outputSimpleHalfStringCoords.toString() << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

class ExpressionTreeDrawer {
public:
  int MaxDepth;
  int MaxAllowedWidth;
  int MaxDisplayedNodes;
  int indexInCurrentLayer;
  int indexCurrentChild;
  Rational widthMaxLayer;
  int numLayers;
  int maxNumCharsInString;
  bool flagUseFullTree;
  Expression baseExpression;
  HashedList<std::string, MathRoutines::HashString> DisplayedEstrings;
  List<bool> DisplayedStringIsLeaf;
  List<Vector<Rational> > NodePositions;
  List<Vector<double> > NodePositionsDouble;
  List<int> LayerSizes;
  List<int> LayerFirstIndices;
  List<List<int> > arrows;
  Plot thePlot;
  List<Expression> currentLayer;
  List<Expression> nextLayer;
  List<Expression> currentEchildrenTruncated;
  Calculator* owner;
  Rational charWidth, padding, layerHeight, charHeight;
  ExpressionTreeDrawer() {
    this->MaxDepth = 10;
    this->MaxAllowedWidth = 10;
    this->MaxDisplayedNodes = 1000;
    this->flagUseFullTree = false;
    this->indexInCurrentLayer = - 1;
    this->indexCurrentChild = - 1;
    this->maxNumCharsInString = 100;
    this->numLayers = 0;
    this->owner = nullptr;
    this->charWidth.AssignNumeratorAndDenominator(1, 20);
    this->padding = 1;
    this->layerHeight = 2;
    this->widthMaxLayer = 0;
    this->charHeight.AssignNumeratorAndDenominator(1, 5);
  }
  Expression& GetCurrentE() {
    MacroRegisterFunctionWithName("ExpressionTreeDrawer::GetCurrentE");
    return this->currentLayer[this->indexInCurrentLayer];
  }
  void ComputeCurrentEchildrenTruncated() {
    MacroRegisterFunctionWithName("ExpressionTreeDrawer::ComputeCurrentEchildrenTruncated");
    this->currentEchildrenTruncated.setSize(0);
    if (!this->flagUseFullTree) {
      if (this->GetCurrentE().IsBuiltInTypE()) {
        return;
      }
    }
    for (int i = 0; i < this->GetCurrentE().children.size; i ++) {
      this->currentEchildrenTruncated.addOnTop(this->GetCurrentE()[i]);
      if (i + 1 + this->indexCurrentChild > this->MaxDisplayedNodes || i > this->MaxAllowedWidth) {
        Expression dotsAtom;
        dotsAtom.MakeAtom(std::string("..."), *this->owner);
        this->currentEchildrenTruncated.addOnTop(dotsAtom);
        break;
      }
    }
  }
  bool isLeaf(const Expression& input) {
    if (this->flagUseFullTree) {
      return input.IsAtom();
    }
    if (input.IsAtom() || input.IsBuiltInTypE()) {
      return true;
    }
    return false;
  }
  std::string GetDisplayString(const Expression& input) {
    MacroRegisterFunctionWithName("ExpressionTreeDrawer::GetDisplayString");
    std::stringstream out;
    if (this->flagUseFullTree) {
      std::string atomName;
      if (input.IsOperation(&atomName)) {
        if (atomName != "...") {
          out << input.theData;
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
  void ComputeCurrentEContributionToNextLayer() {
    MacroRegisterFunctionWithName("ExpressionTreeDrawer::ComputeCurrentEContributionToNextLayer");
    this->ComputeCurrentEchildrenTruncated();
    this->nextLayer.addListOnTop(this->currentEchildrenTruncated);
    List<int> emptyArrows;
    for (int i = 0; i < this->currentEchildrenTruncated.size; i ++) {
      this->arrows[this->indexCurrentChild].addOnTop(this->DisplayedEstrings.size);
      this->AddStringTruncate(
        this->GetDisplayString(this->currentEchildrenTruncated[i]),
        this->isLeaf(this->currentEchildrenTruncated[i])
      );
      this->arrows.addOnTop(emptyArrows);
    }
  }
  void init() {
    this->indexInCurrentLayer = 0;
    this->indexCurrentChild = 0;
    this->currentLayer.setSize(1);
    this->LayerFirstIndices.setSize(1);
    this->LayerFirstIndices[0] = 0;
    this->LayerSizes.setSize(1);
    this->LayerSizes[0] = 1;
    List<int> emptyArrows;
    this->arrows.addOnTop(emptyArrows);
    this->currentLayer[0] = this->baseExpression;
    this->DisplayedEstrings.Clear();
    this->AddStringTruncate(this->baseExpression.toString(), this->isLeaf(this->baseExpression));
    this->ComputeCurrentEContributionToNextLayer();
    this->DisplayedEstrings.setExpectedSize(this->MaxDisplayedNodes);
    this->arrows.setExpectedSize(this->MaxDisplayedNodes);
  }
  std::string toString() {
    std::stringstream out;
    out << "<br>Index in current layer: " << this->indexInCurrentLayer;
    out << "<br>Index in displayed strings: " << this->indexCurrentChild;
    out << "<br>Current layer: " << this->currentLayer.ToStringCommaDelimited();
    out << "<br>Next layer: " << this->nextLayer.ToStringCommaDelimited();
    out << "<br>Displayed strings: " << this->DisplayedEstrings.ToStringCommaDelimited() ;
    out << "<br>Node positions: " << this->NodePositions.ToStringCommaDelimited() ;
    out << "<br>Arrows: " << this->arrows.ToStringCommaDelimited() ;
    out << "<br>Width max layer: " << this->widthMaxLayer;
    return out.str();
  }
  void AddStringTruncate(const std::string& input, bool isLeaf) {
    this->DisplayedStringIsLeaf.addOnTop(isLeaf);
    if (input.size() <= static_cast<unsigned>(this->maxNumCharsInString)) {
      this->DisplayedEstrings.addOnTop(input);
      return;
    }
    std::string truncatedInput = input;
    truncatedInput.resize(static_cast<unsigned>(this->maxNumCharsInString) - 3);
    truncatedInput += "...";
    this->DisplayedEstrings.addOnTop(truncatedInput);
  }
  bool IncrementReturnFalseIfPastLast() {
    MacroRegisterFunctionWithName("ExpressionTreeDrawer::IncrementReturnFalseIfPastLast");
    this->indexInCurrentLayer ++;
    this->indexCurrentChild ++;
    if (this->indexInCurrentLayer >= this->currentLayer.size) {
      this->indexInCurrentLayer = 0;
      this->currentLayer = this->nextLayer;
      if (this->currentLayer.size == 0) {
        return false;
      }
      this->LayerFirstIndices.addOnTop(this->indexCurrentChild);
      this->LayerSizes.addOnTop(this->nextLayer.size);
      this->nextLayer.setSize(0);
      this->ComputeCurrentEContributionToNextLayer();
      return this->currentLayer.size > 0;
    }
    this->ComputeCurrentEContributionToNextLayer();
    return true;
  }
  Rational GetStringWidthTruncated(int theIndex) {
    return this->charWidth *
    MathRoutines::Minimum(
      this->maxNumCharsInString,
      static_cast<signed>(this->DisplayedEstrings[theIndex].size())
    );
  }
  Rational GetLayerWidth(int layerIndex) {
    MacroRegisterFunctionWithName("ExpressionTreeDrawer::GetLayerWidth");
    Rational result = 0;
    for (
      int i = this->LayerFirstIndices[layerIndex];
      i < this->LayerFirstIndices[layerIndex] + this->LayerSizes[layerIndex];
      i ++
    ) {
      result += this->GetStringWidthTruncated(i) + this->padding;
    }
    result -= this->padding;
    if (result > this->widthMaxLayer) {
      this->widthMaxLayer = result;
    }
    return result;
  }
  void ComputeLayerPositions(int layerIndex) {
    MacroRegisterFunctionWithName("ExpressionTreeDrawer::ComputeLayerPositions");
    Rational currentX = - this->GetLayerWidth(layerIndex) / 2;
    for (
      int i = this->LayerFirstIndices[layerIndex];
      i < this->LayerFirstIndices[layerIndex] + this->LayerSizes[layerIndex];
      i ++
    ) {
      this->NodePositions[i].setSize(2);
      this->NodePositions[i][0] = currentX + this->GetStringWidthTruncated(i) / 2;
      this->NodePositions[i][1] = this->layerHeight * layerIndex * (- 1);
      currentX += this->charWidth * static_cast<int>(this->DisplayedEstrings[i].size()) + this->padding;
    }
  }
  void DrawToDV() {
    MacroRegisterFunctionWithName("ExpressionTreeDrawer::ExtractDisplayedExpressions");
    this->init();
    while (this->IncrementReturnFalseIfPastLast()) {
    }
    this->thePlot.dimension = 2;
    this->thePlot.flagIncludeCoordinateSystem = false;
    this->NodePositions.setSize(this->DisplayedEstrings.size);
    for (int i = 0; i < this->LayerFirstIndices.size; i ++) {
      this->ComputeLayerPositions(i);
    }
    this->NodePositionsDouble.setSize(this->NodePositions.size);
    for (int i = 0; i < this->NodePositionsDouble.size; i ++) {
      this->NodePositionsDouble[i] = MathRoutines::GetVectorDouble(this->NodePositions[i]);
    }
    Vector<double> arrowBase, arrowHead;
    for (int i = 0; i < this->DisplayedEstrings.size; i ++) {
      for (int j = 0; j < this->arrows[i].size; j ++) {
        arrowBase = this->NodePositionsDouble[i];
        arrowHead = this->NodePositionsDouble[this->arrows[i][j]];
        arrowHead[1] += this->charHeight.GetDoubleValue() / 2;
        PlotObject theSegment;
        theSegment.thePlotString = "segment";
        theSegment.thePointsDouble.addOnTop(arrowBase);
        theSegment.thePointsDouble.addOnTop(arrowHead);
        theSegment.colorJS = "black";
        this->thePlot += theSegment;
      }
      if (this->DisplayedEstrings[i] != "") {
        PlotObject theText;
        theText.thePlotType = "label";
        theText.thePointsDouble.addOnTop(this->NodePositionsDouble[i]);
        theText.colorJS =
        this->DisplayedStringIsLeaf[i] ? "red" : "gray";
        theText.thePlotString =
        HtmlRoutines::clearNewLines
        (HtmlRoutines::backslashQuotes(HtmlRoutines::DoubleBackslashes(this->DisplayedEstrings[i]) ));
        thePlot += theText;
      } else {
        PlotObject thePoint;
        thePoint.thePlotType = "point";
        thePoint.colorJS = "blue";
        thePoint.thePointsDouble.addOnTop(this->NodePositionsDouble[i]);
        this->thePlot += thePoint;
      }
    }
  }
};

bool CalculatorFunctions::innerDrawExpressionGraphWithOptions(
  Calculator& theCommands, const Expression& input, Expression& output, bool useFullTree
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDrawExpressionGraph");
  ExpressionTreeDrawer theEdrawer;
  theEdrawer.flagUseFullTree = useFullTree;
  theEdrawer.owner = &theCommands;
  theEdrawer.baseExpression = input;
  theEdrawer.DrawToDV();
  return output.AssignValue(theEdrawer.thePlot, theCommands);
}

bool CalculatorFunctions::innerDrawWeightSupport(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDrawWeightSupport");
  // theNode.owner->theHmm.MakeG2InB3(theParser);
  if (!input.IsListNElements(3)) {
    return output.MakeError(
      "Wrong number of arguments, must be 2. ",
      theCommands
    );
  }
  const Expression& typeNode = input[1];
  const Expression& hwNode = input[2];
  WithContext<SemisimpleLieAlgebra*> theAlgPointer;
  if (!theCommands.Convert(
    typeNode,
    CalculatorConversions::functionSemisimpleLieAlgebra,
    theAlgPointer
  )) {
    return output.MakeError("Error extracting Lie algebra.", theCommands);
  }
  SemisimpleLieAlgebra& theAlg = *theAlgPointer.content;
  Vector<Rational> highestWeightFundCoords;
  Expression tempContext;
  if (!theCommands.GetVectoR<Rational>(
    hwNode,
    highestWeightFundCoords,
    &tempContext,
    theAlg.GetRank(),
    nullptr
  )) {
    return false;
  }
  Vector<Rational> highestWeightSimpleCoords;
  WeylGroupData& theWeyl = theAlg.theWeyl;
  highestWeightSimpleCoords = theWeyl.GetSimpleCoordinatesFromFundamental(highestWeightFundCoords);
  // Vectors<Rational> theWeightsToBeDrawn;
  std::stringstream out;
  charSSAlgMod<Rational> theChar;
  theChar.MakeFromWeight(highestWeightSimpleCoords, theAlgPointer.content);
  DrawingVariables theDV;
  std::string report;
  theChar.DrawMeNoMults(report, theDV, 10000);
  out << report << theDV.GetHtmlDiv(theWeyl.GetDim());
  out << "<br>A table with the weights of the character follows. <br>";
  out << theChar.ToStringFullCharacterWeightsTable();
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerSetRandomSeed(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerRandomInteger");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  int theInt = - 1;
  if (!argument.IsIntegerFittingInInt(& theInt)) {
    return theCommands << "Failed to extract small integer";
  }
  std::stringstream out;
  theCommands.theObjectContainer.CurrentRandomSeed = theInt;
  global.unsecurePseudoRandomGenerator.SetRandomSeed(theInt);
  out << "Successfully set random seed to: " << static_cast<unsigned>(theInt);
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerAnd(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerAnd");
  if (input.size() != 3) {
    return false;
  }
  if (input[1].IsEqualToZero()) {
    return output.AssignValue(0, theCommands);
  }
  if (input[2].IsEqualToZero()) {
    return output.AssignValue(0, theCommands);
  }
  if (input[1].IsEqualToOne() && input[2].IsEqualToOne()) {
    return output.AssignValue(1, theCommands);
  }
  return false;
}

bool CalculatorFunctions::innerOr(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerOr");
  if (input.size() != 3) {
    return false;
  }
  if (input[1].IsEqualToOne()) {
    return output.AssignValue(1, theCommands);
  }
  if (input[2].IsEqualToOne()) {
    return output.AssignValue(1, theCommands);
  }
  if (input[1].IsEqualToZero() && input[2].IsEqualToZero()) {
    return output.AssignValue(0, theCommands);
  }
  return false;
}

bool CalculatorFunctions::innerIfStandard(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIfStandard");
  (void) theCommands; //portable way of avoiding unused parameter warning
  if (input.size() != 4) {
    return false;
  }
  if (input[1].IsEqualToOne()) {
    output = input[2];
    return true;
  }
  if (input[1].IsEqualToZero()) {
    output = input[3];
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerIfFrozen(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIfFrozen");
  (void) theCommands; // portable way of avoiding unused parameter warning
  if (input.size() != 4) {
    return false;
  }
  Expression firstArgument;
  if (!theCommands.EvaluateExpression(theCommands, input[1], firstArgument)) {
    return false;
  }
  if (input[1] != firstArgument) {
    output.reset(theCommands);
    output.AddChildAtomOnTop("if");
    output.AddChildOnTop(firstArgument);
    output.AddChildOnTop(input[2]);
    output.AddChildOnTop(input[3]);
    return true;
  }
  if (firstArgument.IsEqualToOne()) {
    output = input[2];
    return true;
  }
  if (firstArgument.IsEqualToZero()) {
    output = input[3];
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerTurnRulesOnOff(
  Calculator& theCommands,
  const Expression& input,
  Expression& output, bool turnOff
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTurnRulesOnOff");
  List<std::string> rulesToConsider;
  std::string currentRule;
  if (
    !input.StartsWith(theCommands.opTurnOffRules()) &&
    !input.StartsWith(theCommands.opTurnOnRules())
  ) {
    if (input.IsOfType<std::string>(&currentRule)) {
      rulesToConsider.addOnTop(currentRule);
    } else if (input.IsOperation(&currentRule)) {
      rulesToConsider.addOnTop(currentRule);
    } else {
      return theCommands << "Could not extract rule to turn off from "
      << input.toString() << ". ";
    }
  } else {
    for (int i = 1; i < input.size(); i ++) {
      if (input[i].IsOfType<std::string>(&currentRule)) {
        rulesToConsider.addOnTop(currentRule);
      } else if (input[i].IsOperation(&currentRule)) {
        rulesToConsider.addOnTop(currentRule);
      } else {
        return theCommands
        << "Could not extract rule to turn off from "
        << input[i].toString() << ". ";
      }
    }
  }
  HashedList<std::string, MathRoutines::HashString> rulesToSwitch;
  rulesToSwitch.Reserve(rulesToConsider.size);
  for (int i = 0; i < rulesToConsider.size; i ++) {
    if (!theCommands.namedRules.Contains(rulesToConsider[i])) {
      return theCommands << "Can't find named rule: " << rulesToConsider[i]
      << ". Turn-off rules command failed. "
      ;
    } else {
      rulesToSwitch.addOnTopNoRepetition(rulesToConsider[i]);
    }
  }
  output.reset(theCommands, rulesToSwitch.size + 1);
  if (turnOff) {
    output.AddChildAtomOnTop(theCommands.opRulesOff());
  } else {
    output.AddChildAtomOnTop(theCommands.opRulesOn());
  }
  Expression currentRuleE;
  for (int i = 0; i < rulesToSwitch.size; i ++) {
    currentRuleE.AssignValue(rulesToSwitch[i], theCommands);
    output.AddChildOnTop(currentRuleE);
  }
  return true;
}

bool CalculatorFunctions::innerTurnOnApproximations(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTurnOnApproximations");
  (void) input;
  theCommands.flagNoApproximationS = false;
  return output.AssignValue(std::string("Approximations have been turned on. "), theCommands);
}

bool CalculatorFunctions::innerTurnOffApproximations(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTurnOffApproximations");
  (void) input;
  theCommands.flagNoApproximationS = true;
  return output.AssignValue(std::string("Approximations have been turned off. "), theCommands);
}

bool CalculatorFunctions::innerTurnOffRules(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTurnOffRules");
  return CalculatorFunctions::innerTurnRulesOnOff(theCommands, input, output, true);
}

bool CalculatorFunctions::innerTurnOnRules(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTurnOnRules");
  return CalculatorFunctions::innerTurnRulesOnOff(theCommands, input, output, false);
}

bool CalculatorFunctions::innerEqualityToArithmeticExpression(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerEqualityToArithmeticExpression");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  return CalculatorFunctions::functionEqualityToArithmeticExpression(theCommands, argument, output);
}

bool CalculatorFunctions::functionEqualityToArithmeticExpression(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::functionEqualityToArithmeticExpression");
  if (!input.StartsWith(theCommands.opDefine(), 3)) {
    return false;
  }
  return output.MakeXOX(theCommands, theCommands.opMinus(), input[1], input[2]);
}

bool CalculatorFunctions::innerRandomInteger(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerRandomInteger");
  Matrix<Expression> theMat;
  if (!theCommands.GetMatrixExpressionsFromArguments(input, theMat, - 1, 2)) {
    return theCommands << "<hr>Failed to extract a Nx2 matrix giving the integer intervals";
  }
  if (theMat.NumRows == 0) {
    return theCommands << "<hr>Failed to extract a Nx2 matrix giving the integer intervals";
  }
  List<List<int> > theIntervals;
  theIntervals.setSize(theMat.NumRows);
  for (int i = 0; i < theMat.NumRows; i ++) {
    theIntervals[i].setSize(theMat.NumCols);
    for (int j = 0; j < theMat.NumCols; j ++) {
      if (!theMat(i, j).IsIntegerFittingInInt(&theIntervals[i][j])) {
        return theCommands << "<hr>Failed to convert "
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
  int generatedRandomInt = static_cast<signed>(global.unsecurePseudoRandomGenerator.GetRandomLessThanBillion()) % accum;
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
    return theCommands << "<hr>Failed to generate a random number: "
    << "this shouldn't happen - perhaps the requested "
    << "interval(s) were too large. The generated random integer was: "
    << generatedRandomInt << " and the intervals were: " << theIntervals;
  }
  return output.AssignValue(resultRandomValue, theCommands);
}

bool CalculatorFunctions::innerSelectAtRandom(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSelectAtRandom");
  if (!input.StartsWith(theCommands.operations.GetIndex("SelectAtRandom"))) {
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
  int randomIndex = (global.unsecurePseudoRandomGenerator.GetRandomPositiveLessThanBillion() % (input.size() - 1)) + 1;
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
