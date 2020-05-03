// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "transport_layer_security.h"
#include "general_logging_global_variables.h"
#include "general_file_operations_encodings.h"
#include "crypto.h"
#include "abstract_syntax_notation_one_decoder.h"

#include <unistd.h> //<- close, open defined here

bool TransportLayerSecurity::LoadPEMCertificate(std::stringstream* commentsOnFailure) {
  std::string certificateContent;
  if (!FileOperations::loadFiletoStringVirtual_AccessUltraSensitiveFoldersIfNeeded(
    TransportLayerSecurity::fileCertificate, certificateContent, true, true, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load certificate file. ";
    }
    return false;
  }
  return this->theServer.certificate.LoadFromPEM(certificateContent, commentsOnFailure);
}

bool TransportLayerSecurity::LoadPEMPrivateKey(
  std::stringstream* commentsOnFailure
) {
  std::string certificateContent, certificateContentStripped;
  static bool accessULTRASensitiveFoldersAllowed = true;
  // Access to ultra sensitive folders is allowed only once, at the start of the program.
  // No further attempts to load allowed.
  if (!FileOperations::loadFiletoStringVirtual_AccessUltraSensitiveFoldersIfNeeded(
    TransportLayerSecurity::fileKey,
    certificateContent,
    true,
    accessULTRASensitiveFoldersAllowed,
    commentsOnFailure
  )) {
    accessULTRASensitiveFoldersAllowed = false;
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load key file. ";
    }
    return false;
  }
  accessULTRASensitiveFoldersAllowed = false;
  return this->theServer.privateKey.LoadFromPEM(certificateContent, commentsOnFailure);
}

bool TransportLayerSecurity::initSSLKeyFilesInternal(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::initSSLKeyFilesInternal");
  this->openSSLData.initSSLKeyFilesCreateOnDemand();
  global << Logger::purple << "Using self-signed certificate. " << Logger::endL;
  if (!this->LoadPEMCertificate(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load pem certificate. ";
    }
    return false;
  }
  if (!this->LoadPEMPrivateKey(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load pem private key. ";
    }
  }
  return true;
}
