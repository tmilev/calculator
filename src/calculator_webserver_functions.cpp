#include "calculator_inner_functions.h"
#include "webserver.h"

bool CalculatorFunctionsCrypto::x509CertificateServer(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::x509CertificateServer");
  (void) input;
  return
  output.assignValue(
    calculator, global.server().transportLayerSecurity.server.certificate.
    source
  );
}

bool CalculatorFunctionsCrypto::x509CertificateDecode(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::x509CertificateDecode");
  std::string base64;
  if (!input[1].isOfType(&base64)) {
    return output.assignError(calculator, "Input is not a string. ");
  }
  X509Certificate certificate;
  std::stringstream commentsOnError;
  if (!certificate.loadFromPEM(base64, &commentsOnError)) {
    return output.assignValue(calculator, commentsOnError.str());
  }
  std::stringstream out;
  out << certificate.toStringTestEncode() << "<br>";
  out << certificate.toString();
  return output.assignValue(calculator, out.str());
}
