#include "transport_layer_security.h"
#include "general_logging_global_variables.h"
#include "general_file_operations_encodings.h"
#include "crypto.h"

#include <unistd.h> //<- close, open defined here

#ifdef MACRO_use_open_ssl
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

static ProjectInformationInstance projectInfoInstanceTransportLayerSecurityOpenSSLImplementation(__FILE__, "Contains all openSSL-related implementation.");

std::string TransportLayerSecurityOpenSSL::errors::errorWantRead = "SSL_ERROR_WANT_READ";
bool TransportLayerSecurityOpenSSL::flagSSLlibraryInitialized = false;

extern logger logServer ;
extern logger logOpenSSL;
extern logger logWorker ;

TransportLayerSecurityOpenSSL::~TransportLayerSecurityOpenSSL() {
  this->FreeSSL();
  this->FreeContext();
}

void TransportLayerSecurityOpenSSL::FreeSSL() {
#ifdef MACRO_use_open_ssl
  SSL_free(this->sslData);
  this->sslData = nullptr;
#endif
}

void TransportLayerSecurityOpenSSL::FreeEverythingShutdownSSL() {
  if (!theGlobalVariables.flagSSLIsAvailable) {
    return;
  }
  theGlobalVariables.flagSSLIsAvailable = false;
  this->FreeSSL();
  this->FreeContext();
}

void TransportLayerSecurityOpenSSL::FreeContext() {
#ifdef  MACRO_use_open_ssl

  if (this->context != nullptr && this->name != "") {
    logOpenSSL << logger::blue << "DEBUG: Freeing openSSL context: " << this->name << ". " << logger::endL;
  }
  SSL_CTX_free (this->context);
#endif // MACRO_use_open_ssl
  this->context = nullptr;
}

void TransportLayerSecurityOpenSSL::initSSLLibrary() {
  MacroRegisterFunctionWithName("TransportLayerSecurityOpenSSL::initSSLlibrary");
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
    logServer << logger::red << commentsOnError.str() << logger::endL;
    crash << "Failed to add ssl algorithms. " << crash;
  }
  if (commentsOnError.str() != "") {
    logServer << logger::red << "OpenSSL initialization comments: " << logger::blue << commentsOnError.str() << logger::endL;
  }
#endif // MACRO_use_open_ssl
}


bool TransportLayerSecurityOpenSSL::initSSLKeyFilesCreateOnDemand() {
  MacroRegisterFunctionWithName("TransportLayerSecurityOpenSSL::initSSLKeyFilesCreateOnDemand");
  if (!theGlobalVariables.flagSSLIsAvailable) {
    return false;
  }
  if (
    FileOperations::FileExistsVirtual(TransportLayerSecurity::fileCertificate, true, true) &&
    FileOperations::FileExistsVirtual(TransportLayerSecurity::fileKey, true, true)
  ) {
    return true;
  }
  logWorker << logger::red << "SSL is available but CERTIFICATE files are missing." << logger::endL;
  logWorker << logger::green << "Let me try to create those files for you." << logger::endL;
  std::stringstream theCommand;
  std::string certificatePhysicalName, keyPhysicalName;
  FileOperations::GetPhysicalFileNameFromVirtual(
    TransportLayerSecurity::fileCertificate, certificatePhysicalName, true, true, nullptr
  );
  FileOperations::GetPhysicalFileNameFromVirtual(
    TransportLayerSecurity::fileKey, keyPhysicalName, true, true, nullptr
  );
  theCommand <<  "openssl req -x509 -newkey rsa:2048 -nodes -keyout " << keyPhysicalName
  << " -out " << certificatePhysicalName
  << " -days 3001 ";
  if (
    theGlobalVariables.configuration["openSSLSubject"].theType != JSData::token::tokenUndefined &&
    theGlobalVariables.configuration["openSSLSubject"].theType == JSData::token::tokenString
  ) {
    theCommand << "-subj " << theGlobalVariables.configuration["openSSLSubject"].theString;
    // "/C=CA/ST=ON/L=MyTown/O=MyOrganization/OU=none/CN=localhost/emailAddress=myemail@gmail.com"
  }
  logServer << "About to generate key files with the following command. " << logger::endL;
  logServer << logger::green << theCommand.str() << logger::endL;
  theGlobalVariables.CallSystemNoOutput(theCommand.str(), &logServer);
  return true;
}

bool TransportLayerSecurityOpenSSL::initSSLKeyFiles() {
  MacroRegisterFunctionWithName("TransportLayerSecurityOpenSSL::initSSLKeyFiles");
  if (!TransportLayerSecurityOpenSSL::initSSLKeyFilesCreateOnDemand()) {
    return false;
  }
  if (
    !FileOperations::FileExistsVirtual(TransportLayerSecurity::fileCertificate, true, true) ||
    !FileOperations::FileExistsVirtual(TransportLayerSecurity::fileKey, true, true)
  ) {
    theGlobalVariables.flagSSLIsAvailable = false;
    return false;
  }
  return true;
}

void TransportLayerSecurityOpenSSL::initSSLClient() {
  this->initSSLCommon(false);
}

bool TransportLayerSecurityOpenSSL::CheckCanInitializeToClient() {
  return this->CheckCanInitialize(false);
}

bool TransportLayerSecurityOpenSSL::CheckCanInitializeToServer() {
  return this->CheckCanInitialize(true);
}

bool TransportLayerSecurityOpenSSL::CheckCanInitialize(bool toServer) {
  if (this->flagContextInitialized) {
    if (this->flagIsServer != toServer) {
      crash << "Attempt to initialize TLS as both server and client. " << crash;
      return false;
    }
  }
  return true;
}

void TransportLayerSecurityOpenSSL::initSSLCommon(bool isServer) {
  MacroRegisterFunctionWithName("TransportLayerSecurityOpenSSL::initSSLCommon");
  this->CheckCanInitialize(isServer);
  if (this->flagContextInitialized) {
    return;
  }
  this->flagIsServer = isServer;
  this->initSSLLibrary();
  std::stringstream commentsOnError;

#ifdef MACRO_use_open_ssl
  if (isServer) {
    this->theSSLMethod = SSLv23_server_method();
  } else {
    this->theSSLMethod = SSLv23_client_method();
  }
  this->context = SSL_CTX_new(this->theSSLMethod);

  if (this->context == nullptr) {
    logServer << logger::red << "Failed to create ssl context. " << logger::endL;
    ERR_print_errors_fp(stderr);
    crash << "Openssl context error.\n" << commentsOnError.str() << crash;
  }
#endif // MACRO_use_open_ssl
  this->flagContextInitialized = true;
  // Server does not verify client certificate:
  //SSL_CTX_set_verify(this->context, SSL_VERIFY_NONE, 0);
}

void TransportLayerSecurityOpenSSL::initSSLServer() {
  MacroRegisterFunctionWithName("TransportLayerSecurityOpenSSL::initSSLServer");
  this->initSSLCommon(true);
  if (this->owner->flagInitializedPrivateKey) {
    return;
  }
  this->owner->flagInitializedPrivateKey = true;
  std::string fileCertificatePhysical, fileKeyPhysical,
  singedFileCertificate1Physical, signedFileCertificate3Physical,
  signedFileKeyPhysical;
  FileOperations::GetPhysicalFileNameFromVirtual(
    TransportLayerSecurity::signedFileCertificate1,
    singedFileCertificate1Physical,
    true,
    true,
    nullptr
  );
  FileOperations::GetPhysicalFileNameFromVirtual(
    TransportLayerSecurity::signedFileCertificate3,
    signedFileCertificate3Physical,
    true,
    true,
    nullptr
  );
  FileOperations::GetPhysicalFileNameFromVirtual(
    TransportLayerSecurity::fileCertificate,
    fileCertificatePhysical,
    true,
    true,
    nullptr
  );
  FileOperations::GetPhysicalFileNameFromVirtual(
    TransportLayerSecurity::fileKey,
    fileKeyPhysical,
    true,
    true,
    nullptr
  );
  FileOperations::GetPhysicalFileNameFromVirtual(
    TransportLayerSecurity::signedFileKey,
    signedFileKeyPhysical,
    true,
    true,
    nullptr
  );
  //////////////////////////////////////////////////////////
  if (!this->initSSLKeyFiles()) {
    return;
  }
#ifdef MACRO_use_open_ssl
  logServer << logger::green << "SSL is available." << logger::endL;
  SSL_CTX_set_ecdh_auto(this->context, 1);
  if (SSL_CTX_use_certificate_chain_file(this->context, signedFileCertificate3Physical.c_str()) <= 0) {
    logServer << logger::purple << "Found no officially signed certificate, trying self-signed certificate. "
    << logger::endL;
    if (SSL_CTX_use_certificate_file(this->context, fileCertificatePhysical.c_str(), SSL_FILETYPE_PEM) <= 0) {
      ERR_print_errors_fp(stderr);
      exit(3);
    }
    if (SSL_CTX_use_PrivateKey_file(this->context, fileKeyPhysical.c_str(), SSL_FILETYPE_PEM) <= 0) {
      ERR_print_errors_fp(stderr);
      exit(4);
    }
  } else {
    logServer << logger::green << "Found officially signed certificate... " << logger::endL;
    //if (SSL_CTX_use_certificate_chain_file(theSSLdata.ctx, signedFileCertificate2.c_str()) <= 0)
    //{ ERR_print_errors_fp(stderr);
    //  exit(3);
    //}
    if (SSL_CTX_use_certificate_chain_file(this->context, singedFileCertificate1Physical.c_str()) <= 0) {
      ERR_print_errors_fp(stderr);
      crash << "Failed to user certificate file." << crash;
    }
    if (SSL_CTX_use_PrivateKey_file(this->context, signedFileKeyPhysical.c_str(), SSL_FILETYPE_PEM) <= 0) {
      ERR_print_errors_fp(stderr);
      crash << "Failed to use private key." << crash;
    }
    theGlobalVariables.flagCertificatesAreOfficiallySigned = true;
  }
  if (!SSL_CTX_check_private_key(this->context)) {
    logServer << "Private key does not match the certificate public key. ";
    crash << "Private key does not match the certificate public key. " << crash;
  }
#else
  logServer << logger::red << "Openssl not available." << logger::endL;
#endif // MACRO_use_open_ssl
}

void TransportLayerSecurityOpenSSL::ClearErrorQueue(
  int errorCode,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  bool includeNoErrorInComments
) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::ToStringError");
  (void) errorCode;
  (void) outputError;
  (void) commentsGeneral;
  (void) includeNoErrorInComments;

#ifdef MACRO_use_open_ssl
  int numErrors = 0;
  ERR_print_errors_fp(stderr);
  int theCode = SSL_get_error(this->sslData, errorCode);
  if (theCode == SSL_ERROR_NONE) {
    if (commentsGeneral != nullptr && includeNoErrorInComments) {
      *commentsGeneral << "\n<br>\nNo error.\n";
    }
    return;
  }
  numErrors ++;
  int extraErrorCode = 0;
  switch (theCode) {
  case SSL_ERROR_ZERO_RETURN:
    if (outputError != nullptr) {
      *outputError = "SSL_ERROR_ZERO_RETURN";
    }
    break;
  case SSL_ERROR_WANT_READ:
    if (outputError != nullptr) {
      *outputError = TransportLayerSecurityOpenSSL::errors::errorWantRead;
    }
    break;
  case SSL_ERROR_WANT_WRITE:
    if (outputError != nullptr) {
      *outputError = "SSL_ERROR_WANT_WRITE";
    }
    break;
  case SSL_ERROR_WANT_CONNECT:
    if (outputError != nullptr) {
      *outputError = "SSL_ERROR_WANT_CONNECT";
    }
    break;
  case SSL_ERROR_WANT_ACCEPT:
    if (outputError != nullptr) {
      *outputError = "SSL_ERROR_WANT_ACCEPT";
    }
    break;
  case SSL_ERROR_WANT_X509_LOOKUP:
    if (outputError != nullptr) {
      *outputError = "SSL_ERROR_WANT_X509_LOOKUP";
    }
    break;
  case SSL_ERROR_SYSCALL:
    if (outputError != nullptr) {
      *outputError = "SSL_ERROR_SYSCALL";
    }
    extraErrorCode = static_cast<int>(ERR_get_error());
    if (extraErrorCode == 0) {
      if (commentsGeneral != nullptr) {
        *commentsGeneral << "Bad eof. ";
      }
    } else if (extraErrorCode == - 1) {
      if (commentsGeneral != nullptr) {
        *commentsGeneral << "I/O error outside of ssl. ";
      }
    }
    break;
  case SSL_ERROR_SSL:
    if (outputError != nullptr) {
      *outputError = "SSL_ERROR_SSL";
    }
    break;
  default:
    if (outputError != nullptr) {
      *outputError = "SSL_ERROR_unknown";
    }
    break;
  }
#endif // MACRO_use_open_ssl
}

void TransportLayerSecurityOpenSSL::DoSetSocket(int theSocket) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::DoSetSocket");
  (void) theSocket;
#ifdef MACRO_use_open_ssl
  int result = 0;
  for (int i = 0; i < 10; i ++) {

    result = SSL_set_fd(this->sslData, theSocket);
    if (result != 0) {
      break;
    }
  }
  if (result == 0) {
    crash << "Failed to set socket of server. " << crash;
  }
#endif // MACRO_use_open_ssl
}

bool TransportLayerSecurityOpenSSL::HandShakeIamClientNoSocketCleanup(
  int inputSocketID,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  (void) inputSocketID;
  (void) commentsOnFailure;
  (void) commentsGeneral;
  this->FreeSSL();
  this->initSSLClient();
#ifdef MACRO_use_open_ssl

  this->sslData = SSL_new(this->context);
  if (this->sslData == nullptr) {
    this->flagSSLHandshakeSuccessful = false;
    logOpenSSL << logger::red << "Failed to allocate ssl. " << logger::endL;
    crash << "Failed to allocate ssl: not supposed to happen. " << crash;
  }
  this->SetSocketAddToStack(inputSocketID);
  int maxNumHandshakeTries = 4;
  for (int i = 0; i < maxNumHandshakeTries; i ++) {
    this->errorCode = SSL_connect(this->sslData);
    this->flagSSLHandshakeSuccessful = false;
    if (this->errorCode != 1) {
      if (this->errorCode == 0) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "Attempt " << i + 1
          << ": SSL handshake not successful in a "
          << "controlled fashion (errorCode = 0). <br>";
        }
      } else {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "Attempt " << i + 1
          << ": SSL handshake not successful with a fatal error with "
          << "errorCode: " << this->errorCode << ". <br>";
        }
      }
      switch(SSL_get_error(this->sslData, this->errorCode)) {
      case SSL_ERROR_NONE:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "No error reported, this shouldn't happen. <br>";
        }
        maxNumHandshakeTries = 1;
        break;
      case SSL_ERROR_ZERO_RETURN:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "The TLS/SSL connection has been closed (possibly cleanly). <br>";
        }
        maxNumHandshakeTries = 1;
        break;
      case SSL_ERROR_WANT_READ:
      case SSL_ERROR_WANT_WRITE:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << " During regular I/O: repeat needed (not implemented). <br>";
        }
        break;
      case SSL_ERROR_WANT_CONNECT:
      case SSL_ERROR_WANT_ACCEPT:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << " During handshake negotiations: repeat needed (not implemented). <br> ";
        }
        break;
      case SSL_ERROR_WANT_X509_LOOKUP:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << " Application callback set by SSL_CTX_set_client_cert_cb(): "
          << "repeat needed (not implemented).  <br>";
        }
        maxNumHandshakeTries = 1;
        break;
      //case SSL_ERROR_WANT_ASYNC:
      //  logOpenSSL << logger::red << "Asynchronous engine is still processing data. <br>"
      //  << logger::endL;
      //  break;
      case SSL_ERROR_SYSCALL:
        if (commentsOnFailure != nullptr)
          *commentsOnFailure << logger::red << "Error: some I/O error occurred. <br>"
          << logger::endL;
        maxNumHandshakeTries = 1;
        break;
      case SSL_ERROR_SSL:
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << "A failure in the SSL library occurred. <br>";
        }
        //theError = ERR_get_error(3ssl);
        //if (theError!=SSL_ERROR_WANT_READ && theError!=SSL_ERROR_WANT_WRITE)
        //  maxNumHandshakeTries =1;
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
      theGlobalVariables.FallAsleep(500000);
    } else {
      this->flagSSLHandshakeSuccessful = true;
      break;
    }
  }
  if (this->flagSSLHandshakeSuccessful) {
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "<span style =\"color:green\">SSL handshake successful.</span>\n<br>\n";
    }
  } else {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<span style =\"color:red\">SSL handshake failed.</span>\n<br>\n";
    }
    return false;
  }
  // CHK_SSL(err).
  // Get the cipher - opt.
  // Get client's certificate (note: beware of dynamic allocation) - opt.
  if ((false)) {
    this->InspectCertificates(commentsOnFailure, commentsGeneral);
  }
  return true;
#else
  return false;
#endif // MACRO_use_open_ssl
}

bool TransportLayerSecurityOpenSSL::InspectCertificates(
  std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::InspectCertificates");
  (void) commentsOnFailure;
  (void) commentsGeneral;
#ifdef MACRO_use_open_ssl
  this->peer_certificate = SSL_get_peer_certificate(this->sslData);
  if (this->peer_certificate != nullptr) {
    char* tempCharPtr = nullptr;
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "SSL connection using: "
      << SSL_get_cipher(this->sslData) << ".<br>\n";
    }
    tempCharPtr = X509_NAME_oneline(X509_get_subject_name(this->peer_certificate), nullptr, 0);
    if (tempCharPtr == nullptr) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "X509_NAME_oneline return null; this is not supposed to happen. <br>\n";
      }
      return false;
    }
    this->otherCertificateSubjectName = tempCharPtr;
    free(tempCharPtr);
    tempCharPtr = nullptr;
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "Peer certificate: "
      << "subject: " << this->otherCertificateSubjectName << "<br>\n";
    }
    tempCharPtr = X509_NAME_oneline(X509_get_issuer_name(this->peer_certificate), nullptr, 0);
    if (tempCharPtr == nullptr) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "X509_NAME_oneline return null; this is not supposed to happen. <br>\n";
      }
      return false;
    }
    this->otherCertificateIssuerName = tempCharPtr;
    free(tempCharPtr);
    tempCharPtr = nullptr;
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "Issuer name: "
      << this->otherCertificateIssuerName << "<br>\n";
    }
    X509_free(this->peer_certificate);
    this->peer_certificate = nullptr;
    return true;
  } else {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "SSL connection using: "
      << SSL_get_cipher(this->sslData) << ". "
      << "No client certificate.<br>\n";
    }
    return false;
  }
#else
  return false;
#endif
}

int TransportLayerSecurityOpenSSL::SSLRead(
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
  int result = SSL_read(this->sslData, readBuffer.TheObjects, readBuffer.size);
  this->ClearErrorQueue(
    result, outputError, commentsGeneral, includeNoErrorInComments
  );
  return result;
#else
  return - 1;
#endif // MACRO_use_open_ssl
}

int TransportLayerSecurityOpenSSL::SSLWrite(
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
    crash << "Write buffer size: " << writeBuffer.size
    << " must be positive." << crash;
  }
  if (this->sslData == nullptr) {
    crash << "Uninitialized ssl not allowed here. " << crash;
  }
  int result = SSL_write(this->sslData, writeBuffer.TheObjects, writeBuffer.size);
  this->ClearErrorQueue(
    result, outputError, commentsOnError, includeNoErrorInComments
  );
  return result;
#else
  return - 1;
#endif
}

bool TransportLayerSecurityOpenSSL::HandShakeIamServer(int inputSocketID, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("WebServer::HandShakeIamServer");
  (void) inputSocketID;
  (void) commentsOnFailure;
  if (this->sslData != nullptr) {
    crash << "SSL data expected to be zero. " << crash;
  }
#ifdef MACRO_use_open_ssl
  this->sslData = SSL_new(this->context);
  SSL_set_verify(this->sslData, SSL_VERIFY_NONE, nullptr);
  if (this->sslData == nullptr) {
    crash << "Failed to allocate ssl: not supposed to happen. " << crash;
  }
  this->SetSocketAddToStack(inputSocketID);
  int maxNumHandshakeTries = 3;
  this->flagSSLHandshakeSuccessful = false;
  for (int i = 0; i < maxNumHandshakeTries; i ++) {
    this->errorCode = SSL_accept(this->sslData);
    if (this->errorCode <= 0) {

      this->flagSSLHandshakeSuccessful = false;
      if (commentsOnFailure != nullptr) {
        if (this->errorCode == 0) {
          *commentsOnFailure << "OpenSSL handshake not successful in a controlled manner. ";
        } else {
          *commentsOnFailure << "OpenSSL handshake not successful with a fatal error. ";
        }
        *commentsOnFailure << "Attempt " << i + 1 << " out of " << maxNumHandshakeTries << " failed. ";
      }
      ERR_print_errors_fp(stderr);

      switch(SSL_get_error(this->sslData, this->errorCode)) {
        case SSL_ERROR_NONE:
          if (commentsOnFailure != nullptr) {
            *commentsOnFailure << "No error reported, this shouldn't happen. ";
          }
          maxNumHandshakeTries = 1;
          break;
        case SSL_ERROR_ZERO_RETURN:
          if (commentsOnFailure != nullptr) {
            *commentsOnFailure << "The TLS/SSL connection has been closed (possibly cleanly). ";
          }
          maxNumHandshakeTries = 1;
          break;
        case SSL_ERROR_WANT_READ:
        case SSL_ERROR_WANT_WRITE:
          if (commentsOnFailure != nullptr) {
            *commentsOnFailure << " During regular I/O: repeat needed (not implemented). ";
          }
          break;
        case SSL_ERROR_WANT_CONNECT:
        case SSL_ERROR_WANT_ACCEPT:
          if (commentsOnFailure != nullptr) {
            *commentsOnFailure << " During handshake negotiations: repeat needed (not implemented). ";
          }
          break;
        case SSL_ERROR_WANT_X509_LOOKUP:
          if (commentsOnFailure != nullptr) {
            *commentsOnFailure << "Application callback set by SSL_CTX_set_client_cert_cb(): "
            << "repeat needed (not implemented). ";
          }
          maxNumHandshakeTries = 1;
          break;
        // case SSL_ERROR_WANT_ASYNC:
        //  logOpenSSL << logger::red << "Asynchronous engine is still processing data. "
        //  << logger::endL;
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
        // theError = ERR_get_error(3ssl);
        // if (theError!=SSL_ERROR_WANT_READ && theError!=SSL_ERROR_WANT_WRITE)
        //   maxNumHandshakeTries =1;
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
      theGlobalVariables.FallAsleep(500000);
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
  this->theSSLMethod = nullptr;
  this->context = nullptr;
  this->owner = nullptr;
  this->flagSSLHandshakeSuccessful = false;
  this->flagContextInitialized = false;
  this->flagIsServer = true;
}
