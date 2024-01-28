#include "crypto_calculator.h"
#include "general_file_operations_encodings.h"
#include "general_logging_global_variables.h"
#include "transport_layer_security.h"

bool TransportLayerSecurityServer::loadSelfSignedPEMCertificate(
  std::stringstream* commentsOnFailure
) {
  std::string certificateContent;
  if (
    !this->loadSelfSignedPEMCertificateContent(
      commentsOnFailure, certificateContent
    )
  ) {
    return false;
  }
  return this->certificate.loadFromPEM(certificateContent, commentsOnFailure);
}

bool TransportLayerSecurityServer::loadSelfSignedPEMCertificateContent(
  std::stringstream* commentsOnFailure, std::string& certificateContent
) {
  if (
    !FileOperations::loadFileToStringUnsecure(
      this->owner->selfSigned.certificateFileNamePhysical,
      certificateContent,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load certificate file. ";
    }
    return false;
  }
  return true;
}

bool TransportLayerSecurityServer::loadSelfSignedPEMPrivateKeyContent(
  std::stringstream* commentsOnFailure,
  std::string& outputPrivateKeyContent
) {
  STACK_TRACE(
    "TransportLayerSecurityServer::loadSelfSignedPEMPrivateKeyContent"
  );
  static bool alreadyRan = false;
  if (alreadyRan) {
    global.fatal
    << "Call TransportLayerSecurityServer::loadSelfSignedPEMPrivateKeyContent "
    << "only once please. "
    << global.fatal;
  }
  if (this->owner == nullptr) {
    global.fatal << "Uninitialized TLS." << global.fatal;
  }
  alreadyRan = true;
  if (
    !FileOperations::loadFileToStringUnsecure(
      this->owner->selfSigned.privateKeyFileNamePhysical,
      outputPrivateKeyContent,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to load private key file: "
      << this->owner->selfSigned.privateKeyFileNamePhysical;
    }
    return false;
  }
  global
  << "Loaded private key from: "
  << this->owner->selfSigned.privateKeyFileNamePhysical
  << Logger::endL;
  return true;
}

bool TransportLayerSecurityServer::loadSelfSignedPEMPrivateKey(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("TransportLayerSecurityServer::loadSelfSignedPEMPrivateKey");
  std::string privateKeyContent;
  if (
    !this->loadSelfSignedPEMPrivateKeyContent(
      commentsOnFailure, privateKeyContent
    )
  ) {
    return false;
  }
  return this->privateKey.loadFromPEM(privateKeyContent, commentsOnFailure);
}
