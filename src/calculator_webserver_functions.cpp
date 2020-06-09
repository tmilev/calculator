// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_inner_functions.h"
//////////////////////////////////////
#include "webserver.h"
#include "system_functions_global_objects.h"
///////////////////////////////////

bool CalculatorFunctions::innerX509CertificateServer(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerX509CertificateServer");
  (void) input;
  //List<unsigned char> outputChars;
  //theWebServer.theTLS.theServer.certificate.writeBytesASN1(outputChars);
  //outputChars = theWebServer.theTLS.theServer.certificate.sourceBinary;
  std::string result = Crypto::convertListUnsignedCharsToBase64(global.server().theTLS.theServer.certificate.sourceBinary, false);
  return output.assignValue(result, calculator);
}

bool CalculatorFunctions::innerX509CertificateDecode(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerX509CertificateDecode");
  std::string binary;
  if (!input.isOfType(&binary)) {
    return output.makeError("Input is not a string. ", calculator);
  }
  X509Certificate theCertificate;
  theCertificate.sourceBinary = binary;
  std::stringstream commentsOnError;
  if (!theCertificate.loadFromASNEncoded(theCertificate.sourceBinary, &commentsOnError)) {
    return output.assignValue(commentsOnError.str(), calculator);
  }
  std::stringstream out;
  out << theCertificate.toStringTestEncode() << "<br>";
  out << theCertificate.toString();
  return output.assignValue(out.str(), calculator);
}
