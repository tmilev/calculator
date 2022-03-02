 // The current file is licensed under the license terms found in the main header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_inner_functions.h"

// ////////////////////////////////////
#include "webserver.h"
#include "system_functions_global_objects.h"

// /////////////////////////////////
bool CalculatorFunctionsCrypto::x509CertificateServer(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::x509CertificateServer");
  (void) input;
  std::string result =
  Crypto::convertListUnsignedCharsToBase64(
    global.server().transportLayerSecurity.server.certificate.sourceBinary,
    false
  );
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsCrypto::x509CertificateDecode(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::x509CertificateDecode");
  std::string binary;
  if (!input[1].isOfType(&binary)) {
    return output.assignError(calculator, "Input is not a string. ");
  }
  X509Certificate certificate;
  certificate.sourceBinary = binary;
  std::stringstream commentsOnError;
  if (
    !certificate.loadFromASNEncoded(
      certificate.sourceBinary, &commentsOnError
    )
  ) {
    return output.assignValue(calculator, commentsOnError.str());
  }
  std::stringstream out;
  out << certificate.toStringTestEncode() << "<br>";
  out << certificate.toString();
  return output.assignValue(calculator, out.str());
}
