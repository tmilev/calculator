//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".

#include "transport_layer_security.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"
#include "vpfHeader1General7FileOperations_Encodings.h"
#include "vpfHeader5Crypto.h"
#include "abstract_syntax_notation_one_decoder.h"

#include <unistd.h> //<- close, open defined here

ProjectInformationInstance projectInfoInstanceTransportLayerSecurityImplementationInternal(__FILE__, "TSL/ssl implementation playground.");

extern logger logServer   ;

bool TransportLayerSecurity::LoadPEMCertificate(std::stringstream* commentsOnFailure) {
  std::string certificateContent, certificateContentStripped;
  if (!FileOperations::LoadFileToStringVirtual(TransportLayerSecurity::fileCertificate, certificateContent, true, true, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load certificate file. ";
    }
    return false;
  }
  //see ASN1_item_d2i_bio for decoding.
  certificateContentStripped = MathRoutines::StringTrimWhiteSpace(certificateContent);
  if (!MathRoutines::StringBeginsWith(certificateContentStripped, "-----BEGIN CERTIFICATE-----", &certificateContentStripped)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad certificate start. ";
    }
    return false;
  }
  if (!MathRoutines::StringEndsWith(certificateContentStripped, "-----END CERTIFICATE-----", &certificateContentStripped)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad certificate end. ";
    }
    return false;
  }
  certificateContentStripped = MathRoutines::StringTrimWhiteSpace(certificateContentStripped);
  if (!Crypto::ConvertBase64ToString(certificateContentStripped, this->theServer.certificate.sourceBinary, commentsOnFailure, 0)) {
    return false;
  }
  if (!this->theServer.certificate.LoadFromASNEncoded(this->theServer.certificate.sourceBinary, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load certificate. ";
    }
    return false;
  }
  logServer << "Loaded certificate: " << this->theServer.certificate.theRSA.ToString() << logger::endL;
  //if (SSL_CTX_use_certificate_file(this->context, fileCertificatePhysical.c_str(), SSL_FILETYPE_PEM) <= 0) {
  //  ERR_print_errors_fp(stderr);
  //  exit(3);
  //}
  return false;
}

bool TransportLayerSecurity::LoadPEMPrivateKey(std::stringstream *commentsOnFailure) {
  //if (SSL_CTX_use_PrivateKey_file(this->context, fileKeyPhysical.c_str(), SSL_FILETYPE_PEM) <= 0) {
  //  ERR_print_errors_fp(stderr);
  //  exit(4);
  //}
  return false;
}

bool TransportLayerSecurity::initSSLKeyFilesInternal(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::initSSLKeyFilesInternal");
  this->openSSLData.initSSLKeyFilesCreateOnDemand();
  logServer << logger::purple << "Using self-signed certificate. " << logger::endL;
  if (!this->LoadPEMCertificate(commentsOnFailure)) {
    return false;
  }
  if (!this->LoadPEMPrivateKey(commentsOnFailure)) {
    crash << "Failed to load PEM private key. " << crash;
  }


  return false;
}
