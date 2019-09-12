//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#include "vpfHeader3Calculator2_InnerFunctions.h"
//////////////////////////////////////
#include "webserver.h"
#include "vpfHeader4SystemFunctionsGlobalObjects.h"
///////////////////////////////////

extern ProjectInformationInstance ProjectInfoCalculatorWebserverFunctions;
ProjectInformationInstance ProjectInfoCalculatorWebserverFunctions(__FILE__, "Calculator functions related to the web server.");

bool CalculatorFunctionsGeneral::innerX509CertificateServer(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerX509CertificateServer");
  (void) input;
  List<unsigned char> outputChars;
  theWebServer.theTLS.theServer.certificate.WriteBytes(outputChars);
  outputChars = theWebServer.theTLS.theServer.certificate.sourceBinary;
  std::string result(reinterpret_cast<char*>(outputChars.TheObjects), static_cast<unsigned>(outputChars.size));
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsGeneral::innerX509CertificateDecode(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerX509CertificateDecode");
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
  out << theCertificate.ToStringTestEncode();
  out << theCertificate.ToString();
  return output.AssignValue(out.str(), theCommands);
}
