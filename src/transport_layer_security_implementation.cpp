#include "transport_layer_security.h"
#include "general_logging_global_variables.h"
#include "general_file_operations_encodings.h"
#include "crypto.h"
#include "abstract_syntax_notation_one_decoder.h"
#include <unistd.h> //<- close, open defined here

bool TransportLayerSecurityServer::loadSelfSignedPEMCertificate(std::stringstream *commentsOnFailure)
{
  std::string certificateContent;
  if (
    !FileOperations::
    loadFileToStringUnsecure(
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
  return
  this->certificate.loadFromPEM(certificateContent, commentsOnFailure);
}

bool TransportLayerSecurityServer::loadSelfSignedPEMPrivateKey(std::stringstream *commentsOnFailure)
 {
static  bool alreadyRan= false;
  if (alreadyRan){
    global.fatal << "Call TransportLayerSecurityServer::loadSelfSignedPEMPrivateKey only once please. "
    << global.fatal;
  }
  alreadyRan =true;
  std::string certificateContent;
  std::string certificateContentStripped;
  if (
    !FileOperations::
    loadFileToStringUnsecure(
      this->owner->selfSigned.privateKeyFileNamePhysical,
      certificateContent,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load key file. ";
    }
    return false;
  }
  return
  this->privateKey.loadFromPEM(certificateContent, commentsOnFailure);
}

