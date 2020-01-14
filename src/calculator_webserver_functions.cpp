// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_inner_functions.h"
//////////////////////////////////////
#include "webserver.h"
#include "system_functions_global_objects.h"
///////////////////////////////////

bool CalculatorFunctions::innerX509CertificateServer(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerX509CertificateServer");
  (void) input;
  //List<unsigned char> outputChars;
  //theWebServer.theTLS.theServer.certificate.WriteBytesASN1(outputChars);
  //outputChars = theWebServer.theTLS.theServer.certificate.sourceBinary;
  std::string result = Crypto::ConvertListUnsignedCharsToBase64(global.server().theTLS.theServer.certificate.sourceBinary, false);
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctions::innerX509CertificateDecode(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerX509CertificateDecode");
  std::string binary;
  if (!input.IsOfType(&binary)) {
    return output.MakeError("Input is not a string. ", theCommands);
  }
  X509Certificate theCertificate;
  theCertificate.sourceBinary = binary;
  std::stringstream commentsOnError;
  if (!theCertificate.LoadFromASNEncoded(theCertificate.sourceBinary, &commentsOnError)) {
    return output.AssignValue(commentsOnError.str(), theCommands);
  }
  std::stringstream out;
  out << theCertificate.ToStringTestEncode() << "<br>";
  out << theCertificate.ToString();
  return output.AssignValue(out.str(), theCommands);
}
