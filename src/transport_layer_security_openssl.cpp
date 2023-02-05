#include "transport_layer_security.h"
#include "general_logging_global_variables.h"
#include "general_file_operations_encodings.h"
#include "general_strings.h"
#include "crypto.h"
#include <unistd.h> //<- close, open defined here

#ifdef MACRO_use_open_ssl

// To install openssl:
// 1. Debian rodete: 
// sudo apt-get install libssl-dev
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#endif

std::string TransportLayerSecurityOpenSSL::errors::errorWantRead =
"SSL_ERROR_WANT_READ";
bool TransportLayerSecurityOpenSSL::flagSSLlibraryInitialized = false;
const SSL_METHOD* TransportLayerSecurityOpenSSL::methodGlobal = nullptr;
SSL_CTX* TransportLayerSecurityOpenSSL::contextGlobal;

TransportLayerSecurityOpenSSL::~TransportLayerSecurityOpenSSL() {
  this->peer_certificate = nullptr;
  this->owner = nullptr;
  this->freeSession();
}

void TransportLayerSecurityOpenSSL::freeSession() {
  if (this->sslData == nullptr) {
    return;
  }
#ifdef MACRO_use_open_ssl
  SSL_free(this->sslData);
  this->sslData = nullptr;
#endif
}

void TransportLayerSecurityOpenSSL::freeContextGlobal() {
#ifdef MACRO_use_open_ssl
  SSL_CTX_free(TransportLayerSecurityOpenSSL::contextGlobal);
#endif // MACRO_use_open_ssl
  TransportLayerSecurityOpenSSL::contextGlobal = nullptr;
}

void TransportLayerSecurityOpenSSL::initializeSSLLibrary() {
  STACK_TRACE("TransportLayerSecurityOpenSSL::initializeSSLLibrary");
  if (this->flagSSLlibraryInitialized) {
    return;
  }
  this->flagSSLlibraryInitialized = true;
  std::stringstream commentsOnError;
  // this command loads error strings and initializes openSSL.
#ifdef MACRO_use_open_ssl
  SSL_load_error_strings();
  int loadedSuccessfully = OpenSSL_add_ssl_algorithms();
  if (!loadedSuccessfully) {
    global << Logger::red << commentsOnError.str() << Logger::endL;
    global.fatal << "Failed to add ssl algorithms. " << global.fatal;
  }
  if (commentsOnError.str() != "") {
    global
    << Logger::red
    << "OpenSSL initialization comments: "
    << Logger::blue
    << commentsOnError.str()
    << Logger::endL;
  }
  TransportLayerSecurityOpenSSL::methodGlobal = SSLv23_method();
  TransportLayerSecurityOpenSSL::contextGlobal =
  SSL_CTX_new(TransportLayerSecurityOpenSSL::methodGlobal);
  if (TransportLayerSecurityOpenSSL::contextGlobal == nullptr) {
    global << Logger::red << "Failed to create ssl context. " << Logger::endL;
    ERR_print_errors_fp(stderr);
    global.fatal
    << "Openssl context error.\n"
    << commentsOnError.str()
    << global.fatal;
  }
#endif // MACRO_use_open_ssl
}

bool TransportLayerSecurityOpenSSL::initSSLKeyFilesSelfSignedCreateOnDemand() {
  STACK_TRACE(
    "TransportLayerSecurityOpenSSL::initSSLKeyFilesSelfSignedCreateOnDemand"
  );
  if (!global.flagSSLAvailable) {
    return false;
  }
  this->owner->selfSigned.makeSelfSignedKeyAndCertificate();
  if (this->owner->selfSigned.keysExist()){
    return true;
  }
  global
  << Logger::red
  << "SSL is available but CERTIFICATE files are missing."
  << Logger::endL;
  global << "Let me try to create those files for you." << Logger::endL;
  std::stringstream command;
  command
  << "openssl req -x509 -newkey rsa:2048 -nodes -keyout "
  << this->owner->selfSigned.privateKeyFileNamePhysical
  << " -out "
  << this->owner->selfSigned.certificateFileNamePhysical
  << " -days 3001 ";
  if (
    global.configuration["openSSLSubject"].elementType !=
    JSData::token::tokenUndefined &&
    global.configuration["openSSLSubject"].elementType ==
    JSData::token::tokenString
  ) {
    command << "-subj " << global.configuration["openSSLSubject"].stringValue;
    // "/C=CA/ST=ON/L=MyTown/O=MyOrganization/OU=none/CN=localhost/emailAddress=myemail@gmail.com"
  }
  global
  << Logger::green
  << "I am generating a local ssl certificate so you "
  << "can run your calculator over https. "
  << "The certificate will only be used by the calculator itself. "
  << "The certificate and its prive keys are located in the "
  << "certificates/ subfolder of your "
  << "calculator directory. "
  << "Please enter the requested information; you can fill "
  << "it in with bogus data if you so desire. "
  << Logger::purple
  << "Please use at least a single character for the name, else "
  << "the certificate won't generated. "
  << Logger::green
  << "The data you fill in will be visible in the `Security` "
  << "tab your web browser. Leaving the fields totally empty may "
  << "case security warning in your browser. "
  << Logger::endL;
  global << Logger::yellow << command.str() << Logger::endL;
  global.externalCommandNoOutput(command.str(), true);
  return true;
}


void TransportLayerSecurityOpenSSL::initializeSSLClient() {
  this->initializeSSLCommon(false);
}

bool TransportLayerSecurityOpenSSL::checkCanInitializeToClient() {
  return this->checkCanInitialize(false);
}

bool TransportLayerSecurityOpenSSL::checkCanInitializeToServer() {
  return this->checkCanInitialize(true);
}

bool TransportLayerSecurityOpenSSL::checkCanInitialize(bool toServer) {
  if (this->flagContextInitialized) {
    if (this->flagIsServer != toServer) {
      global.fatal
      << "Attempt to initialize TLS as both server and client. "
      << global.fatal;
      return false;
    }
  }
  return true;
}

void TransportLayerSecurityOpenSSL::initializeSSLCommon(bool isServer) {
  STACK_TRACE("TransportLayerSecurityOpenSSL::initializeSSLCommon");
  this->checkCanInitialize(isServer);
  if (this->flagContextInitialized) {
    return;
  }
  this->flagIsServer = isServer;
  this->initializeSSLLibrary();
  std::stringstream commentsOnError;
  this->flagContextInitialized = true;
}





void TransportLayerSecurityOpenSSL::initializeSSLServerCertificatesSelfSigned(){
  STACK_TRACE("TransportLayerSecurityOpenSSL::initializeSSLServerCertificatesSelfSigned");
  if (this->flagSelfSignedCertificatesAreInitialized){
    return;
  }
  this->flagSelfSignedCertificatesAreInitialized = true;
  TransportLayerSecurityConfiguration &selfSigned = this->owner->selfSigned;
  if (selfSigned.privateKeyFileNamePhysical == ""){
    selfSigned.makeSelfSignedKeyAndCertificate();
  }
  global
  << Logger::purple
  << "Using self-signed certificate: "
  << Logger::yellow
  << selfSigned.certificateFileNamePhysical
  << Logger::endL
  << "Private key: "
  << Logger::red
  << selfSigned.privateKeyFileNamePhysical
  << Logger::endL;
#ifdef MACRO_use_open_ssl
  if (
    SSL_CTX_use_certificate_file(
      this->contextGlobal, selfSigned.certificateFileNamePhysical.c_str(), SSL_FILETYPE_PEM
    ) <=
    0
  ) {
    ERR_print_errors_fp(stderr);
    exit(3);
  }
  if (
    SSL_CTX_use_PrivateKey_file(
      this->contextGlobal, selfSigned.privateKeyFileNamePhysical.c_str(), SSL_FILETYPE_PEM
    ) <=
    0
  ) {
    ERR_print_errors_fp(stderr);
    exit(4);
  }
#else
  global << Logger::red << "Open ssl not available." << Logger::endL;
#endif
}

void TransportLayerSecurityOpenSSL::initializeSSLServerCertificatesOfficial() {
  STACK_TRACE("TransportLayerSecurityOpenSSL::initializeSSLServerCertificatesOfficial");
  if (this->flagOfficialCertificatesAreInitialized) {
    return;
  }
  this->flagOfficialCertificatesAreInitialized = true;
  std::string certificateFileName = this->owner->official.certificateFileNamePhysical;
  std::string keyFileName = this->owner->official.privateKeyFileNamePhysical;
  if ( certificateFileName  == "" || keyFileName == ""){
    global << "Found no officially signed certificate. " << Logger::endL;
    return;
  }
#ifdef MACRO_use_open_ssl
  global << Logger::green << "SSL is available." << Logger::endL;
  SSL_CTX_set_ecdh_auto(this->contextGlobal, 1);
  if (
    SSL_CTX_use_certificate_chain_file(
      this->contextGlobal, certificateFileName.c_str()
    ) <=
    0
  ) {
    global.fatal << "Could not find certificate file: " << certificateFileName;
  }
  global
    << Logger::green
    << "Found officially signed certificate: "
    << Logger::endL
    << Logger::green
    << certificateFileName
    << Logger::endL;
    if (
      SSL_CTX_use_PrivateKey_file(
        this->contextGlobal,
        keyFileName.c_str(),
        SSL_FILETYPE_PEM
      ) <=
      0
    ) {
      ERR_print_errors_fp(stderr);
      global.fatal << "Failed to use private key." << global.fatal;
    }
    global.flagCertificatesAreOfficiallySigned = true;

#else
  global << Logger::red << "Openssl not available." << Logger::endL;
#endif // MACRO_use_open_ssl
}

void TransportLayerSecurityOpenSSL::initializeSSLServer() {
  STACK_TRACE("TransportLayerSecurityOpenSSL::initializeSSLServer");
  this->initializeSSLCommon(true);

}

void TransportLayerSecurityOpenSSL::clearErrorQueue(
  int numberOfTransferredBytes
) {
  STACK_TRACE("TransportLayerSecurityOpenSSL::clearErrorQueue");
  (void) numberOfTransferredBytes;
#ifdef MACRO_use_open_ssl
  char buffer[200];
  if (numberOfTransferredBytes < 0) {
    int errorCode = SSL_get_error(this->sslData, numberOfTransferredBytes);
    global
    << Logger::red
    << "Error: transferred bytes: "
    << numberOfTransferredBytes
    << "; error code: "
    << errorCode
    << ". "
    << strerror(errno)
    << ". "
    << Logger::endL;
    if (this->sslData == nullptr) {
      global
      << Logger::yellow
      << "Ssl data structure is null. "
      << Logger::endL;
    }
  }
  for (;;) {
    errorCode = ERR_get_error();
    if (errorCode == 0) {
      return;
    }
    ERR_error_string(errorCode, buffer);
    std::string errorString(buffer);
    global << Logger::red << errorString << Logger::endL;
  }
#endif // MACRO_use_open_ssl
}

void TransportLayerSecurityOpenSSL::doSetSocket(int socketFileDescriptor) {
  STACK_TRACE("TransportLayerSecurityOpenSSL::doSetSocket");
  (void) socketFileDescriptor;
#ifdef MACRO_use_open_ssl
  int result = 0;
  for (int i = 0; i < 10; i ++) {
    result = SSL_set_fd(this->sslData, socketFileDescriptor);
    if (result != 0) {
      break;
    }
  }
  if (result == 0) {
    global.fatal << "Failed to set socket of server. " << global.fatal;
  }
#endif // MACRO_use_open_ssl
}

bool TransportLayerSecurityOpenSSL::handShakeIAmClientNoSocketCleanup(
  int inputSocketID,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  (void) inputSocketID;
  (void) commentsOnFailure;
  (void) commentsGeneral;
  this->freeSession();
  this->initializeSSLClient();
#ifdef MACRO_use_open_ssl
  this->sslData = SSL_new(this->contextGlobal);
  if (this->sslData == nullptr) {
    this->flagSSLHandshakeSuccessful = false;
    global << Logger::red << "Failed to allocate ssl. " << Logger::endL;
    global.fatal
    << "Failed to allocate ssl: not supposed to happen. "
    << global.fatal;
  }
  this->setSocketAddToStack(inputSocketID);
  int maxNumHandshakeTries = 4;
  for (int i = 0; i < maxNumHandshakeTries; i ++) {
    this->errorCode = SSL_connect(this->sslData);
    this->flagSSLHandshakeSuccessful = false;
    if (this->errorCode != 1) {
      if (this->errorCode == 0) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "Attempt "
          << i + 1
          << ": SSL handshake not successful in a "
          << "controlled fashion (errorCode = 0). <br>";
        }
      } else {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "Attempt "
          << i + 1
          << ": SSL handshake not successful with a fatal error with "
          << "errorCode: "
          << this->errorCode
          << ". <br>";
        }
      }
      switch (SSL_get_error(this->sslData, this->errorCode)) {
      case SSL_ERROR_NONE:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "No error reported, this shouldn't happen. <br>";
        }
        maxNumHandshakeTries = 1;
        break;
      case SSL_ERROR_ZERO_RETURN:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "The TLS/SSL connection has been closed (possibly cleanly). <br>";
        }
        maxNumHandshakeTries = 1;
        break;
      case SSL_ERROR_WANT_READ:
      case SSL_ERROR_WANT_WRITE:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << " During regular I/O: repeat needed (not implemented). <br>";
        }
        break;
      case SSL_ERROR_WANT_CONNECT:
      case SSL_ERROR_WANT_ACCEPT:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          <<
          " During handshake negotiations: repeat needed (not implemented). <br> "
          ;
        }
        break;
      case SSL_ERROR_WANT_X509_LOOKUP:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << " Application callback set by SSL_CTX_set_client_cert_cb(): "
          << "repeat needed (not implemented).  <br>";
        }
        maxNumHandshakeTries = 1;
        break;
        // case SSL_ERROR_WANT_ASYNC:
        // logOpenSSL << Logger::red << "Asynchronous engine is still
        // processing
        // data. <br>"
        //  << Logger::endL;
        //  break;
      case SSL_ERROR_SYSCALL:
        if (commentsOnFailure != nullptr) *commentsOnFailure
        << Logger::red
        << "Error: some I/O error occurred. <br>"
        << Logger::endL;
        maxNumHandshakeTries = 1;
        break;
      case SSL_ERROR_SSL:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "A failure in the SSL library occurred. <br>";
        }
        break;
      default:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "Unknown error. <br>";
        }
        maxNumHandshakeTries = 1;
        break;
      }
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Retrying connection in 0.5 seconds... <br>";
      }
      global.fallAsleep(500000);
    } else {
      this->flagSSLHandshakeSuccessful = true;
      break;
    }
  }
  if (this->flagSSLHandshakeSuccessful) {
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "<span style ='color:green'>SSL handshake successful.</span>\n<br>\n";
    }
  } else {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "<span style ='color:red'>SSL handshake failed.</span>\n<br>\n";
    }
    return false;
  }
  // CHK_SSL(err).
  // Get the cipher - opt.
  // Get client's certificate (note: beware of dynamic allocation) - opt.
  if ((false)) {
    this->inspectCertificates(commentsOnFailure, commentsGeneral);
  }
  return true;
#else
  return false;
#endif // MACRO_use_open_ssl
}

bool TransportLayerSecurityOpenSSL::inspectCertificates(
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  STACK_TRACE("TransportLayerSecurity::inspectCertificates");
  (void) commentsOnFailure;
  (void) commentsGeneral;
#ifdef MACRO_use_open_ssl
  this->peer_certificate = SSL_get_peer_certificate(this->sslData);
  if (this->peer_certificate != nullptr) {
    char* tempCharPtr = nullptr;
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "SSL connection using: "
      << SSL_get_cipher(this->sslData)
      << ".<br>\n";
    }
    tempCharPtr =
    X509_NAME_oneline(
      X509_get_subject_name(this->peer_certificate), nullptr, 0
    );
    if (tempCharPtr == nullptr) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "X509_NAME_oneline return null; "
        << "this is not supposed to happen. <br>\n";
      }
      return false;
    }
    this->otherCertificateSubjectName = tempCharPtr;
    free(tempCharPtr);
    tempCharPtr = nullptr;
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "Peer certificate: "
      << "subject: "
      << this->otherCertificateSubjectName
      << "<br>\n";
    }
    tempCharPtr =
    X509_NAME_oneline(
      X509_get_issuer_name(this->peer_certificate), nullptr, 0
    );
    if (tempCharPtr == nullptr) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "X509_NAME_oneline return null; "
        << "this is not supposed to happen. <br>\n";
      }
      return false;
    }
    this->otherCertificateIssuerName = tempCharPtr;
    free(tempCharPtr);
    tempCharPtr = nullptr;
    if (commentsGeneral != nullptr) {
      *commentsGeneral
      << "Issuer name: "
      << this->otherCertificateIssuerName
      << "<br>\n";
    }
    X509_free(this->peer_certificate);
    this->peer_certificate = nullptr;
    return true;
  } else {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "SSL connection using: "
      << SSL_get_cipher(this->sslData)
      << ". "
      << "No client certificate.<br>\n";
    }
    return false;
  }
#else
  return false;
#endif
}

int TransportLayerSecurityOpenSSL::sslRead(
  List<char>& readBuffer,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  bool includeNoErrorInComments
) {
  (void) readBuffer;
  (void) outputError;
  (void) commentsGeneral;
  (void) includeNoErrorInComments;
#ifdef MACRO_use_open_ssl
  ERR_clear_error();
  int result =
  SSL_read(this->sslData, readBuffer.objects, readBuffer.size);
  if (result <= 0) {
    this->clearErrorQueue(result);
  }
  return result;
#else
  return - 1;
#endif // MACRO_use_open_ssl
}

int TransportLayerSecurityOpenSSL::sslWrite(
  const List<char>& writeBuffer,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  std::stringstream* commentsOnError,
  bool includeNoErrorInComments
) {
  (void) writeBuffer;
  (void) outputError;
  (void) commentsGeneral;
  (void) commentsOnError;
  (void) includeNoErrorInComments;
#ifdef MACRO_use_open_ssl
  ERR_clear_error();
  if (writeBuffer.size <= 0) {
    global.fatal
    << "Write buffer size: "
    << writeBuffer.size
    << " must be positive."
    << global.fatal;
  }
  if (this->sslData == nullptr) {
    global.fatal << "Uninitialized ssl not allowed here. " << global.fatal;
  }
  int result = 0;
  result =
  SSL_write(this->sslData, writeBuffer.objects, writeBuffer.size);
  this->clearErrorQueue(result);
  return result;
#else
  return - 1;
#endif
}

bool TransportLayerSecurityOpenSSL::handShakeIamServer(
  int inputSocketID, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("WebServer::handShakeIamServer");
  (void) inputSocketID;
  (void) commentsOnFailure;
  if (this->sslData != nullptr) {
    global.fatal << "SSL data expected to be zero. " << global.fatal;
  }
#ifdef MACRO_use_open_ssl
  this->sslData = SSL_new(this->contextGlobal);
  SSL_set_verify(this->sslData, SSL_VERIFY_NONE, nullptr);
  if (this->sslData == nullptr) {
    global.fatal
    << "Failed to allocate ssl: not supposed to happen. "
    << global.fatal;
  }
  this->setSocketAddToStack(inputSocketID);
  int maxNumHandshakeTries = 3;
  this->flagSSLHandshakeSuccessful = false;
  for (int i = 0; i < maxNumHandshakeTries; i ++) {
    this->errorCode = SSL_accept(this->sslData);
    if (this->errorCode <= 0) {
      this->flagSSLHandshakeSuccessful = false;
      if (commentsOnFailure != nullptr) {
        if (this->errorCode == 0) {
          *commentsOnFailure
          << "OpenSSL handshake not successful in a controlled manner. ";
        } else {
          *commentsOnFailure
          << "OpenSSL handshake not successful with a fatal error. ";
        }
        *commentsOnFailure
        << "Attempt "
        << i + 1
        << " out of "
        << maxNumHandshakeTries
        << " failed. ";
      }
      ERR_print_errors_fp(stderr);
      switch (SSL_get_error(this->sslData, this->errorCode)) {
      case SSL_ERROR_NONE:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "No error reported, this shouldn't happen. ";
        }
        maxNumHandshakeTries = 1;
        break;
      case SSL_ERROR_ZERO_RETURN:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "The TLS/SSL connection has been closed (possibly cleanly). ";
        }
        maxNumHandshakeTries = 1;
        break;
      case SSL_ERROR_WANT_READ:
      case SSL_ERROR_WANT_WRITE:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << " During regular I/O: repeat needed (not implemented). ";
        }
        break;
      case SSL_ERROR_WANT_CONNECT:
      case SSL_ERROR_WANT_ACCEPT:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << " During handshake negotiations: "
          << "repeat needed (not implemented). ";
        }
        break;
      case SSL_ERROR_WANT_X509_LOOKUP:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "Application callback set by SSL_CTX_set_client_cert_cb(): "
          << "repeat needed (not implemented). ";
        }
        maxNumHandshakeTries = 1;
        break;
        // case SSL_ERROR_WANT_ASYNC:
        // logOpenSSL << Logger::red << "Asynchronous engine is still
        // processing
        // data. "
        //  << Logger::endL;
        //  break;
      case SSL_ERROR_SYSCALL:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "Error: some I/O error occurred. ";
        }
        maxNumHandshakeTries = 1;
        break;
      case SSL_ERROR_SSL:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "A failure in the SSL library occurred. ";
        }
        break;
      default:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "Unknown error. ";
        }
        maxNumHandshakeTries = 1;
        break;
      }
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Retrying connection in 0.5 seconds... ";
      }
      global.fallAsleep(500000);
    } else {
      this->flagSSLHandshakeSuccessful = true;
      break;
    }
  }
#endif
  return this->flagSSLHandshakeSuccessful;
}

TransportLayerSecurityOpenSSL::TransportLayerSecurityOpenSSL() {
  this->peer_certificate = nullptr;
  this->sslData = nullptr;
  this->errorCode = - 1;
  this->owner = nullptr;
  this->flagSSLHandshakeSuccessful = false;
  this->flagContextInitialized = false;
  this->flagIsServer = true;
  this->flagOfficialCertificatesAreInitialized = false;
  this->flagSelfSignedCertificatesAreInitialized = false;
}



void TransportLayerSecurityConfiguration::makeOfficialKeyAndCertificate() {
   STACK_TRACE("TransportLayerSecurityConfiguration::makeOfficialKeyAndCertificate");
this->readCertificateFilename();
   this->readPrivateKeyFilename();
}

void TransportLayerSecurityConfiguration::readCertificateFilename(){
  List<std::string> filesInCertificateFolder;
  List<std::string> extensions;
  FileOperations::getFolderFileNamesVirtual(
    this->certificateFolder,
    filesInCertificateFolder,
    &extensions,
    true,
    true,
    nullptr
  );
  std::string authorityCertificate;
  for (int i = 0; i < filesInCertificateFolder.size; i ++) {
    if (extensions[i] != ".crt") {
      continue;
    }
    if (filesInCertificateFolder[i] == this->selfSignedCertificate) {
      continue;
    }
    if (authorityCertificate != "") {
      global.fatal
      << "Found more than one external "
      << "certificate file in certificates/ folder:\n"
      << authorityCertificate
      << "\nand\n"
      << filesInCertificateFolder[i]
      << "\n"
      << "The certificate folder must "
      << "contain one or two certificates (.crt files): "
      << "that of the server and that of an external signing authority. "
      << "The self-signed certificate, if present, "
      << "must be called "
      << this->selfSignedCertificate
      << ". The other file with .crt extension will be assumed to be"
      << " that of the signing authority. "
      << global.fatal;
    }
    authorityCertificate = filesInCertificateFolder[i];
  }
  if (authorityCertificate == "") {
    return;
  }
  std::string result;
  FileOperations::getPhysicalFileNameFromVirtual(
    this->certificateFolder + authorityCertificate,
    this->certificateFileNamePhysical,
    true,
    true,
    nullptr
  );
}

void TransportLayerSecurityConfiguration::readPrivateKeyFilename() {
  STACK_TRACE("TransportLayerSecurity::keyOfficialPhysical");
  List<std::string> filesInCertificateFolder;
  List<std::string> extensions;
  FileOperations::getFolderFileNamesVirtual(
    this->certificateFolder,
    filesInCertificateFolder,
    &extensions,
    true,
    true,
    nullptr
  );
  std::string keyFileName;
  for (int i = 0; i < filesInCertificateFolder.size; i ++) {
    if (extensions[i] != ".key") {
      continue;
    }
    if (filesInCertificateFolder[i] == this->selfSignedPrivateKey) {
      continue;
    }
    if (keyFileName != "") {
      global.fatal
      << "Found more than one private key "
      << "in certificates/ folder:\n"
      << keyFileName
      << "\nand\n"
      << filesInCertificateFolder[i]
      << "\n"
      << "The certificate folder must "
      << "contain one or two private .key files: "
      << "the official one and a self-signed one. "
      << "The self-signed one is for testing "
      << "and experimentation purposes. "
      << global.fatal;
    }
    keyFileName = filesInCertificateFolder[i];
  }
  FileOperations::getPhysicalFileNameFromVirtual(
    this->certificateFolder + keyFileName, this->privateKeyFileNamePhysical, true, true, nullptr
  );
}


bool TransportLayerSecurityConfiguration::makeSelfSignedKeyAndCertificate() {
  STACK_TRACE("TransportLayerSecurityOpenSSL::initSSLKeyFilesSelfSigned");
 return FileOperations::getPhysicalFileNameFromVirtual(
        this->selfSignedCertificate, this->certificateFileNamePhysical, true, true,nullptr
  ) &&
  FileOperations::getPhysicalFileNameFromVirtual(this->selfSignedPrivateKey, this->privateKeyFileNamePhysical, true, true,nullptr);


}

bool TransportLayerSecurityConfiguration::keysExist() const{
  return FileOperations::fileExistsUnsecure(this->privateKeyFileNamePhysical) && FileOperations::fileExistsUnsecure(this->certificateFileNamePhysical);
}