// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "transport_layer_security.h"
#include "general_logging_global_variables.h"
#include "general_file_operations_encodings.h"
#include "crypto.h"
#include "abstract_syntax_notation_one_decoder.h"
#include <unistd.h> //<- close, open defined here

bool TransportLayerSecurity::loadPEMCertificate(
  std::stringstream* commentsOnFailure
) {
  std::string certificateContent;
  if (
    !FileOperations::
    loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(
      this->certificateSelfSignedVirtual(),
      certificateContent,
      true,
      true,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load certificate file. ";
    }
    return false;
  }
  return
  this->server.certificate.loadFromPEM(certificateContent, commentsOnFailure);
}

bool TransportLayerSecurity::loadPEMPrivateKey(
  std::stringstream* commentsOnFailure
) {
  std::string certificateContent, certificateContentStripped;
  static bool accessULTRASensitiveFoldersAllowed = true;
  // Access to ultra sensitive folders is allowed only once, at the start of
  // the
  // program.
  // No further attempts to load allowed.
  if (
    !FileOperations::
    loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(
      this->keySelfSignedVirtual(),
      certificateContent,
      true,
      accessULTRASensitiveFoldersAllowed,
      commentsOnFailure
    )
  ) {
    accessULTRASensitiveFoldersAllowed = false;
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load key file. ";
    }
    return false;
  }
  accessULTRASensitiveFoldersAllowed = false;
  return
  this->server.privateKey.loadFromPEM(certificateContent, commentsOnFailure);
}

bool TransportLayerSecurity::initSSLKeyFilesInternal(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("TransportLayerSecurity::initSSLKeyFilesInternal");
  this->openSSLData.initSSLKeyFilesSelfSignedCreateOnDemand();
  global
  << Logger::purple
  << "Using self-signed certificate. "
  << Logger::endL;
  if (!this->loadPEMCertificate(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load pem certificate. ";
    }
    return false;
  }
  if (!this->loadPEMPrivateKey(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load pem private key. ";
    }
  }
  return true;
}
