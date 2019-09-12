//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".

#include "transport_layer_security.h"
#include "general_4_logging_global_variables.h"
#include "general_7_file_operations_encodings.h"
#include "vpfHeader5Crypto.h"
#include "abstract_syntax_notation_one_decoder.h"

#include <unistd.h> //<- close, open defined here

extern ProjectInformationInstance projectInfoInstanceTransportLayerSecurityImplementationInternal;
ProjectInformationInstance projectInfoInstanceTransportLayerSecurityImplementationInternal(__FILE__, "TSL/ssl implementation playground.");

extern logger logServer   ;

bool TransportLayerSecurity::LoadPEMCertificate(std::stringstream* commentsOnFailure) {
  std::string certificateContent;
  if (!FileOperations::LoadFileToStringVirtual(TransportLayerSecurity::fileCertificate, certificateContent, true, true, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load certificate file. ";
    }
    return false;
  }
  return this->theServer.certificate.LoadFromPEM(certificateContent, commentsOnFailure);
}

bool TransportLayerSecurity::LoadPEMPrivateKey(std::stringstream *commentsOnFailure) {
  std::string certificateContent, certificateContentStripped;
  if (!FileOperations::LoadFileToStringVirtual(TransportLayerSecurity::fileKey, certificateContent, true, true, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load key file. ";
    }
    return false;
  }
  return this->theServer.privateKey.LoadFromPEM(certificateContent, commentsOnFailure);
}

bool TransportLayerSecurity::initSSLKeyFilesInternal(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::initSSLKeyFilesInternal");
  this->openSSLData.initSSLKeyFilesCreateOnDemand();
  logServer << logger::purple << "Using self-signed certificate. " << logger::endL;
  if (!this->LoadPEMCertificate(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load pem certificate. ";
    }
    return false;
  }
  if (!this->LoadPEMPrivateKey(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load pem certificate. ";
    }
  }
  return true;
}
