//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
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
  std::string result(reinterpret_cast<char*>(outputChars.TheObjects), outputChars.size);
  return output.AssignValue(result, theCommands);
}

bool CalculatorFunctionsGeneral::innerX509CertificateDecode(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerX509CertificateDecode");
  std::string certificateBytes;
  if (!input.IsOfType(&certificateBytes)) {
    return output.MakeError("Input is not a string. ", theCommands);
  }
  X509Certificate theCertificate;
  std::stringstream commentsOnError;
  if (!theCertificate.LoadFromASNEncoded(certificateBytes, &commentsOnError)) {
    return output.AssignValue(commentsOnError.str(), theCommands);
  }
  return output.AssignValue(theCertificate.ToString(), theCommands);
}
