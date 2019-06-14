//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".

#include "transport_security_layer.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"
#include "vpfHeader1General7FileOperations_Encodings.h"
#include "vpfHeader5Crypto.h"

#include <unistd.h> //<- close, open defined here

ProjectInformationInstance projectInfoInstanceTransportSecurityLayerImplementation(__FILE__, "TSL/ssl implementation.");

//http://stackoverflow.com/questions/10175812/how-to-create-a-self-signed-certificate-with-openssl
//openssl req -x509 -keyalg RSA -newkey rsa:2048 -nodes -keyout key.pem -out cert.pem -days 3001
//Alternatively:
//certificate with certificate signing request:
//openssl req -out CSR.csr -new -newkey rsa:2048 -nodes -keyout calculator-algebra.key
//then get the CSR.csr file to a signing authority,
//from where you get the signedFileCertificate1 and signedFileCertificate3
const std::string TransportSecurityLayer::fileCertificate = "certificates/cert.pem";
const std::string TransportSecurityLayer::fileCertificateConfiguration = "certificates/certificate_configuration.cnf";
const std::string TransportSecurityLayer::fileKey = "certificates/key.pem";
const std::string TransportSecurityLayer::signedFileCertificate1 = "certificates/calculator-algebra.crt";
const std::string TransportSecurityLayer::signedFileCertificate3 = "certificates/godaddy-signature.crt";
const std::string TransportSecurityLayer::signedFileKey = "certificates/calculator-algebra.key";

std::string TransportSecurityLayer::errors::errorWantRead = "SSL_ERROR_WANT_READ";

extern logger logWorker  ;
extern logger logHttpErrors;
extern logger logBlock;
extern logger logIO   ;
extern logger logOpenSSL   ;
extern logger logProcessKills   ;
extern logger logProcessStats;
extern logger logPlumbing;
extern logger logServer  ;

SSLServer::SSLServer(){
  this->theData = 0;
}

TransportSecurityLayer::TransportSecurityLayer() {
  this->errorCode = - 1;
  this->sslClient = 0;
  this->my_certificate = 0;
  this->peer_certificate = 0;
  this->theSSLMethod = 0;
  this->contextServer = 0;
  //this->contextClient = 0;
  this->flagSSLHandshakeSuccessful = false;
}

TransportSecurityLayer::~TransportSecurityLayer() {
  this->FreeSSL();
  this->FreeContext();
}

void SSLServer::Free() {
  SSL_free(this->theData);
  this->theData = 0;
}

void TransportSecurityLayer::FreeSSL() {
  SSL_free(this->sslClient);
  this->sslServeR.Free();
  this->sslClient = 0;
}

void TransportSecurityLayer::FreeClientSSL() {
  SSL_free(this->sslClient);
  this->sslClient = 0;
}

void TransportSecurityLayer::FreeEverythingShutdownSSL() {
  if (!theGlobalVariables.flagSSLisAvailable) {
    return;
  }
  theGlobalVariables.flagSSLisAvailable = false;
  this->FreeSSL();
  this->FreeContext();
}

void TransportSecurityLayer::FreeContext() {
  //SSL_CTX_free (this->contextClient);
  SSL_CTX_free (this->contextServer);
  //this->contextClient = 0;
  this->contextServer = 0;
}

bool TransportSecurityLayer::flagSSLlibraryInitialized = false;

bool TransportSecurityLayer::initSSLkeyFiles() {
  if (
    !FileOperations::FileExistsVirtual(fileCertificate, true, true) ||
    !FileOperations::FileExistsVirtual(fileKey, true, true)
  ) {
    logWorker << logger::red << "SSL is available but CERTIFICATE files are missing." << logger::endL;
    logWorker << logger::green << "Let me try to create those files for you." << logger::endL;
    std::stringstream theCommand;
    std::string certificatePhysicalName, keyPhysicalName, certificateConfiguration;
    FileOperations::GetPhysicalFileNameFromVirtual(fileCertificate, certificatePhysicalName, true, true, 0);
    FileOperations::GetPhysicalFileNameFromVirtual(fileKey, keyPhysicalName, true, true, 0);
    FileOperations::GetPhysicalFileNameFromVirtual(fileCertificateConfiguration, certificateConfiguration, true, true, 0);
    theCommand <<  "openssl req -x509 -newkey rsa:2048 -nodes -keyout " << keyPhysicalName
    << " -out " << certificatePhysicalName
    << " -days 3001 ";
    if (
      theGlobalVariables.configuration["openSSLSubject"].type != JSData::JSUndefined &&
      theGlobalVariables.configuration["openSSLSubject"].type == JSData::JSstring
    ) {
      theCommand << "-subj " << theGlobalVariables.configuration["openSSLSubject"].string;
    }
    theGlobalVariables.CallSystemNoOutput(theCommand.str(), false);
  }
  if (
    !FileOperations::FileExistsVirtual(fileCertificate, true, true) ||
    !FileOperations::FileExistsVirtual(fileKey, true, true)
  ) {
    theGlobalVariables.flagSSLisAvailable = false;
    return false;
  } else {
    return true;
  }
  return false;
}

void TransportSecurityLayer::initSSLlibrary() {
  if (this->flagSSLlibraryInitialized) {
    return;
  }
  this->flagSSLlibraryInitialized = true;
  std::stringstream commentsOnError;
  // this command loads error strings and initializes openSSL.
  SSL_load_error_strings();
  //loadedSuccessfully = OPENSSL_init_ssl(0, NULL, &commentsOnError);
  //if (loadedSuccessfully != 1) {
  //  logServer << logger::red << commentsOnError.str() << logger::endL;
  //  crash << "Failed to initialize ssl library. " << crash;
  //}
  if (commentsOnError.str() != "") {
    logServer << logger::red << "OpenSSL initialization comments: " << logger::blue << commentsOnError.str() << logger::endL;
  }
  logServer << logger::green << "DEBUG: Initialzation of ssl successfull." << logger::endL;
}

void TransportSecurityLayer::AddMoreEntropyFromTimer() {
  int fixMeAcquireEntropy;

}

void TransportSecurityLayer::initSSLserver() {
  this->initSSLlibrary();
  if (this->theSSLMethod != 0) {
    return;
  }
  std::string fileCertificatePhysical, fileKeyPhysical,
  singedFileCertificate1Physical, signedFileCertificate3Physical,
  signedFileKeyPhysical;
  FileOperations::GetPhysicalFileNameFromVirtual(signedFileCertificate1, singedFileCertificate1Physical, true, true, 0);
  FileOperations::GetPhysicalFileNameFromVirtual(signedFileCertificate3, signedFileCertificate3Physical, true, true, 0);
  FileOperations::GetPhysicalFileNameFromVirtual(fileCertificate, fileCertificatePhysical, true, true, 0);
  FileOperations::GetPhysicalFileNameFromVirtual(fileKey, fileKeyPhysical, true, true, 0);
  FileOperations::GetPhysicalFileNameFromVirtual(signedFileKey, signedFileKeyPhysical, true, true, 0);
  //////////////////////////////////////////////////////////
  if (!this->initSSLkeyFiles()) {
    return;
  }
  logServer << logger::green << "SSL is available." << logger::endL;
  this->theSSLMethod = SSLv23_method();
  std::stringstream commentsOnError;

  logServer << logger::blue << "DEBUG: about to init openssl. " << logger::endL;
  this->contextServer = SSL_CTX_new(this->theSSLMethod);
  logServer << logger::red << "DEBUG: comments: " << commentsOnError.str() << logger::endL;
  if (this->contextServer == 0) {
    logServer << logger::red << "Failed to create ssl context. " << logger::endL;
    ERR_print_errors_fp(stderr);
    crash << "Openssl context error.\n" << commentsOnError.str() << crash;
  }
  if (SSL_CTX_use_certificate_chain_file(this->contextServer, signedFileCertificate3Physical.c_str()) <= 0) {
    logServer << logger::purple << "Found no officially signed certificate, trying self-signed certificate. "
    << logger::endL;
    if (SSL_CTX_use_certificate_file(this->contextServer, fileCertificatePhysical.c_str(), SSL_FILETYPE_PEM) <= 0) {
      ERR_print_errors_fp(stderr);
      exit(3);
    }
    if (SSL_CTX_use_PrivateKey_file(this->contextServer, fileKeyPhysical.c_str(), SSL_FILETYPE_PEM) <= 0) {
      ERR_print_errors_fp(stderr);
      exit(4);
    }
  } else {
    logServer << logger::green << "Found officially signed certificate... " << logger::endL;
    //if (SSL_CTX_use_certificate_chain_file(theSSLdata.ctx, signedFileCertificate2.c_str()) <= 0)
    //{ ERR_print_errors_fp(stderr);
    //  exit(3);
    //}
    if (SSL_CTX_use_certificate_chain_file(this->contextServer, singedFileCertificate1Physical.c_str()) <= 0) {
      ERR_print_errors_fp(stderr);
      crash << "Failed to user certificate file." << crash;
    }
    if (SSL_CTX_use_PrivateKey_file(this->contextServer, signedFileKeyPhysical.c_str(), SSL_FILETYPE_PEM) <= 0) {
      ERR_print_errors_fp(stderr);
      crash << "Failed to use private key." << crash;
    }
    theGlobalVariables.flagCertificatesAreOfficiallySigned = true;
  }
  if (!SSL_CTX_check_private_key(this->contextServer)) {
    logServer << "Private key does not match the certificate public key.";
    crash << "Private key does not match the certificate public key." << crash;
  }
}

bool TransportSecurityLayer::SSLReadLoop(
  int numTries,
  SSL* theSSL,
  std::string& output,
  const LargeInt& expectedLength,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  bool includeNoErrorInComments
) {
  MacroRegisterFunctionWithName("TransportSecurityLayer::SSLreadLoop");
  this->buffer.SetSize(100000);
  output = "";
  int i = 0;
  std::string next;
  int numBytes = - 1;
  for (i = 0; i < numTries; i ++) {
    numBytes = this->SSLRead(
      theSSL, this->buffer.TheObjects, this->buffer.size,
      outputError, commentsGeneral, includeNoErrorInComments
    );
    if (numBytes > 0) {
      next.assign(this->buffer.TheObjects, numBytes);
      output += next;
      if (expectedLength <= 0) {
        break;
      } else {
        if (expectedLength <= output.size()) {
          break;
        }
      }
    }
    if (numBytes == 0) {
      break;
    }
  }
  if (numBytes < 0) {
    if (commentsGeneral != 0) {
      *commentsGeneral << "SSL-ERROR reading from socket. ";
    }
    return false;
  }
  return true;
}

bool TransportSecurityLayer::SSLWriteLoop(
  int numTries,
  SSL* theSSL,
  const std::string& input,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  bool includeNoErrorInComments
) {
  MacroRegisterFunctionWithName("TransportSecurityLayer::SSLwriteLoop");
  Crypto::ConvertStringToListBytesSigned(input, this->buffer);
  int i = 0;
  int numBytes = - 1;
  for (i = 0; i < numTries; i ++) {
    numBytes = this->SSLWrite(
      theSSL,
      this->buffer.TheObjects,
      this->buffer.size,
      outputError,
      commentsGeneral,
      commentsGeneral,
      includeNoErrorInComments
    );
    if (numBytes == this->buffer.size) {
      break;
    }
  }
  if (numBytes != this->buffer.size) {
    if (commentsGeneral != 0) {
      *commentsGeneral  << i << " errors writing to socket.\n NumBytes: " << numBytes << ". ";
    }
    return false;
  }
  return true;
}

void TransportSecurityLayer::ClearErrorQueue(
  int errorCode,
  SSL* theSSL,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  bool includeNoErrorInComments
) {
  MacroRegisterFunctionWithName("TransportSecurityLayer::ToStringError");
  int numErrors = 0;
  int theCode = SSL_get_error(theSSL, errorCode);
  if (theCode == SSL_ERROR_NONE) {
    if (commentsGeneral != 0 && includeNoErrorInComments) {
      *commentsGeneral << "\n<br>\nNo error.\n";
    }
    return;
  }
  numErrors ++;
  int extraErrorCode = 0;
  switch (theCode) {
  case SSL_ERROR_ZERO_RETURN:
    if (outputError != 0) {
      *outputError = "SSL_ERROR_ZERO_RETURN";
    }
    break;
  case SSL_ERROR_WANT_READ:
    if (outputError != 0) {
      *outputError = TransportSecurityLayer::errors::errorWantRead;
    }
    break;
  case SSL_ERROR_WANT_WRITE:
    if (outputError != 0) {
      *outputError = "SSL_ERROR_WANT_WRITE";
    }
    break;
  case SSL_ERROR_WANT_CONNECT:
    if (outputError != 0) {
      *outputError = "SSL_ERROR_WANT_CONNECT";
    }
    break;
  case SSL_ERROR_WANT_ACCEPT:
    if (outputError != 0) {
      *outputError = "SSL_ERROR_WANT_ACCEPT";
    }
    break;
  case SSL_ERROR_WANT_X509_LOOKUP:
    if (outputError != 0) {
      *outputError = "SSL_ERROR_WANT_X509_LOOKUP";
    }
    break;
  case SSL_ERROR_SYSCALL:
    if (outputError != 0) {
      *outputError = "SSL_ERROR_SYSCALL";
    }
    extraErrorCode = ERR_get_error();
    if (extraErrorCode == 0) {
      if (commentsGeneral != 0) {
        *commentsGeneral << "Bad eof. ";
      }
    } else if (extraErrorCode == - 1) {
      if (commentsGeneral != 0) {
        *commentsGeneral << "I/O error outside of ssl. ";
      }
    }
    break;
  case SSL_ERROR_SSL:
    if (outputError != 0) {
      *outputError = "SSL_ERROR_SSL";
    }
    break;
  default:
    if (outputError != 0) {
      *outputError = "SSL_ERROR_unknown";
    }
    break;
  }
}

void TransportSecurityLayer::DoSetSocket(int theSocket, SSL *theSSL) {
  MacroRegisterFunctionWithName("TransportSecurityLayer::DoSetSocket");
  int result = 0;
  for (int i = 0; i < 10; i ++) {
    result = SSL_set_fd(theSSL, theSocket);
    if (result != 0) {
      break;
    }
  }
  if (result == 0) {
    crash << "Failed to set socket of server. " << crash;
  }
}

void TransportSecurityLayer::SetSocketAddToStackServer(int theSocket) {
  MacroRegisterFunctionWithName("TransportSecurityLayer::SetSocketAddToStackServer");
  this->socketStackServer.AddOnTop(theSocket);
  this->DoSetSocket(theSocket, this->sslServeR.theData);
}

void TransportSecurityLayer::SetSocketAddToStackClient(int theSocket) {
  MacroRegisterFunctionWithName("TransportSecurityLayer::SetSocketAddToStackClient");
  this->socketStackClient.AddOnTop(theSocket);
  this->DoSetSocket(theSocket, this->sslClient);
}

void TransportSecurityLayer::RemoveLastSocketServer() {
  MacroRegisterFunctionWithName("TransportSecurityLayer::RemoveLastSocketServer");
  if (this->socketStackServer.size > 0) {
    int lastSocket = this->socketStackServer.PopLastObject();
    close(lastSocket);
  }
  if (this->socketStackServer.size <= 0) {
    return;
  }
  this->DoSetSocket(*this->socketStackServer.LastObject(), this->sslServeR.theData);
}

void TransportSecurityLayer::RemoveLastSocketClient() {
  MacroRegisterFunctionWithName("TransportSecurityLayer::RemoveLastSocketClient");
  if (this->socketStackClient.size > 0) {
    int lastSocket = this->socketStackClient.PopLastObject();
    close(lastSocket);
  }
  if (this->socketStackClient.size <= 0) {
    return;
  }
  this->DoSetSocket(*this->socketStackClient.LastObject(), this->sslClient);
}

bool TransportSecurityLayer::HandShakeIamClientNoSocketCleanup(
  int inputSocketID,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  MacroRegisterFunctionWithName("WebServer::HandShakeIamClientNoSocketCleanup");
  this->FreeClientSSL();
  this->sslClient = SSL_new(this->contextServer);
  if (this->sslClient == 0) {
    this->flagSSLHandshakeSuccessful = false;
    logOpenSSL << logger::red << "Failed to allocate ssl" << logger::endL;
    crash << "Failed to allocate ssl: not supposed to happen" << crash;
  }
  this->SetSocketAddToStackClient(inputSocketID);
  int maxNumHandshakeTries = 4;
  for (int i = 0; i < maxNumHandshakeTries; i ++) {
    this->errorCode = SSL_connect(this->sslClient);
    this->flagSSLHandshakeSuccessful = false;
    if (this->errorCode != 1) {
      if (this->errorCode == 0) {
        if (commentsOnFailure != 0) {
          *commentsOnFailure << "Attempt " << i + 1
          << ": SSL handshake not successfull in a "
          << "controlled fashion (errorCode = 0). <br>";
        }
      } else {
        if (commentsOnFailure != 0) {
          *commentsOnFailure << "Attempt " << i + 1
          << ": SSL handshake not successfull with a fatal error with "
          << "errorCode: " << this->errorCode << ". <br>";
        }
      }
      switch(SSL_get_error(this->sslClient, this->errorCode)) {
      case SSL_ERROR_NONE:
        if (commentsOnFailure != 0) {
          *commentsOnFailure << "No error reported, this shouldn't happen. <br>";
        }
        maxNumHandshakeTries = 1;
        break;
      case SSL_ERROR_ZERO_RETURN:
        if (commentsOnFailure != 0) {
          *commentsOnFailure << "The TLS/SSL connection has been closed (possibly cleanly).  <br>";
        }
        maxNumHandshakeTries = 1;
        break;
      case SSL_ERROR_WANT_READ:
      case SSL_ERROR_WANT_WRITE:
        if (commentsOnFailure != 0) {
          *commentsOnFailure << " During regular I/O: repeat needed (not implemented).  <br>";
        }
        break;
      case SSL_ERROR_WANT_CONNECT:
      case SSL_ERROR_WANT_ACCEPT:
        if (commentsOnFailure != 0) {
          *commentsOnFailure << " During handshake negotiations: repeat needed (not implemented). <br> ";
        }
        break;
      case SSL_ERROR_WANT_X509_LOOKUP:
        if (commentsOnFailure != 0) {
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
        if (commentsOnFailure != 0)
          *commentsOnFailure << logger::red << "Error: some I/O error occurred. <br>"
          << logger::endL;
        maxNumHandshakeTries = 1;
        break;
      case SSL_ERROR_SSL:
        if (commentsOnFailure != 0) {
          *commentsOnFailure << "A failure in the SSL library occurred. <br>";
        }
        //theError = ERR_get_error(3ssl);
        //if (theError!=SSL_ERROR_WANT_READ && theError!=SSL_ERROR_WANT_WRITE)
        //  maxNumHandshakeTries =1;
        break;
      default:
        if (commentsOnFailure != 0) {
          *commentsOnFailure << "Unknown error. <br>";
        }
        maxNumHandshakeTries = 1;
        break;
      }
      if (commentsOnFailure != 0) {
        *commentsOnFailure << "Retrying connection in 0.5 seconds... <br>";
      }
      theGlobalVariables.FallAsleep(500000);
    } else {
      this->flagSSLHandshakeSuccessful = true;
      break;
    }
  }
  if (this->flagSSLHandshakeSuccessful) {
    if (commentsGeneral != 0) {
      *commentsGeneral << "<span style =\"color:green\">SSL handshake successfull.</span>\n<br>\n";
    }
  } else {
    if (commentsOnFailure != 0) {
      *commentsOnFailure << "<span style =\"color:red\">SSL handshake failed.</span>\n<br>\n";
    }
    return false;
  }
  //CHK_SSL(err);
  /* Get the cipher - opt */
  /* Get client's certificate (note: beware of dynamic allocation) - opt */
  if (false) {
    this->InspectCertificates(commentsOnFailure, commentsGeneral);
  }
  return true;
}

bool TransportSecurityLayer::InspectCertificates(
  std::stringstream *commentsOnFailure, std::stringstream *commentsGeneral
) {
  MacroRegisterFunctionWithName("TransportSecurityLayer::InspectCertificates");
  this->peer_certificate = SSL_get_peer_certificate(this->sslClient);
  if (this->peer_certificate != 0) {
    char* tempCharPtr = 0;
    if (commentsGeneral != 0) {
      *commentsGeneral << "SSL connection using: "
      << SSL_get_cipher(this->sslClient) << ".<br>\n";
    }
    tempCharPtr = X509_NAME_oneline(X509_get_subject_name (this->peer_certificate), 0, 0);
    if (tempCharPtr == 0) {
      if (commentsOnFailure != 0) {
        *commentsOnFailure << "X509_NAME_oneline return null; this is not supposed to happen. <br>\n";
      }
      return false;
    }
    this->otherCertificateSubjectName = tempCharPtr;
    OPENSSL_free(tempCharPtr);
    if (commentsGeneral != 0) {
      *commentsGeneral << "Peer certificate: "
      << "subject: " << this->otherCertificateSubjectName << "<br>\n";
    }
    tempCharPtr = X509_NAME_oneline(X509_get_issuer_name(this->peer_certificate), 0, 0);
    if (tempCharPtr == 0) {
      if (commentsOnFailure != 0) {
        *commentsOnFailure << "X509_NAME_oneline return null; this is not supposed to happen. <br>\n";
      }
      return false;
    }
    this->otherCertificateIssuerName = tempCharPtr;
    OPENSSL_free(tempCharPtr);
    if (commentsGeneral != 0) {
      *commentsGeneral << "Issuer name: "
      << this->otherCertificateIssuerName << "<br>\n";
    }
    X509_free(this->peer_certificate);
    this->peer_certificate = 0;
    return true;
  } else {
    if (commentsOnFailure != 0) {
      *commentsOnFailure << "SSL connection using: "
      << SSL_get_cipher(this->sslClient) << ". "
      << "No client certificate.<br>\n";
    }
    return false;
  }
}

int TransportSecurityLayer::SSLRead(
  SSL* theSSL, void* buffer, int bufferSize, std::string* outputError,
  std::stringstream* commentsGeneral, bool includeNoErrorInComments
) {
  ERR_clear_error();
  int result = SSL_read(theSSL, buffer, bufferSize);
  this->ClearErrorQueue(
    result, theSSL, outputError, commentsGeneral, includeNoErrorInComments
  );
  return result;
}

int TransportSecurityLayer::SSLWrite(
  SSL* theSSL,
  void* buffer,
  int bufferSize,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  std::stringstream* commentsOnError,
  bool includeNoErrorInComments
) {
  (void) commentsGeneral;
  ERR_clear_error();
  int result = SSL_write(theSSL, buffer, bufferSize);
  this->ClearErrorQueue(
    result, theSSL, outputError, commentsOnError, includeNoErrorInComments
  );
  return result;
}

bool TransportSecurityLayer::HandShakeIamServer(int inputSocketID) {
  MacroRegisterFunctionWithName("WebServer::HandShakeIamServer");
  std::stringstream commentsOnSSLNew;
  this->sslServeR.theData = SSL_new(this->contextServer);
  if (this->sslServeR.theData == 0) {
    logOpenSSL << logger::red << "Failed to allocate ssl: " << commentsOnSSLNew.str() << logger::endL;
    crash << "Failed to allocate ssl: not supposed to happen" << crash;
  }
  this->SetSocketAddToStackServer(inputSocketID);
  int maxNumHandshakeTries = 3;
  this->flagSSLHandshakeSuccessful = false;
  for (int i = 0; i < maxNumHandshakeTries; i ++) {
    std::stringstream commentsOnError;
    this->errorCode = SSL_accept(this->sslServeR.theData);
    if (this->errorCode != 1) {
      this->flagSSLHandshakeSuccessful = false;
      if (this->errorCode == 0) {
        logOpenSSL << "OpenSSL handshake not successful in a controlled manner. ";
      } else {
        logOpenSSL << "OpenSSL handshake not successful with a fatal error. ";
      }
      logOpenSSL << commentsOnError.str() << logger::endL;
      logOpenSSL << "Attempt " << i + 1 << " out of " << maxNumHandshakeTries << " failed. ";
      switch(SSL_get_error(this->sslServeR.theData, this->errorCode)) {
      case SSL_ERROR_NONE:
        logOpenSSL << logger::red << "No error reported, this shouldn't happen. " << logger::endL;
        maxNumHandshakeTries = 1;
        break;
      case SSL_ERROR_ZERO_RETURN:
        logOpenSSL << logger::red << "The TLS/SSL connection has been closed (possibly cleanly). " << logger::endL;
        maxNumHandshakeTries = 1;
        break;
      case SSL_ERROR_WANT_READ:
      case SSL_ERROR_WANT_WRITE:
        logOpenSSL << logger::red << " During regular I/O: repeat needed (not implemented). " << logger::endL;
        break;
      case SSL_ERROR_WANT_CONNECT:
      case SSL_ERROR_WANT_ACCEPT:
        logOpenSSL << logger::red << " During handshake negotiations: repeat needed (not implemented). "
        << logger::endL;
        break;
      case SSL_ERROR_WANT_X509_LOOKUP:
        logOpenSSL << logger::red << " Application callback set by SSL_CTX_set_client_cert_cb(): "
        << "repeat needed (not implemented). "
        << logger::endL;
        maxNumHandshakeTries = 1;
        break;
      //case SSL_ERROR_WANT_ASYNC:
      //  logOpenSSL << logger::red << "Asynchronous engine is still processing data. "
      //  << logger::endL;
      //  break;
      case SSL_ERROR_SYSCALL:
        logOpenSSL << logger::red << "Error: some I/O error occurred. "
        << logger::endL;
        maxNumHandshakeTries = 1;
        break;
      case SSL_ERROR_SSL:
        logOpenSSL << logger::red << "A failure in the SSL library occurred. "
        << logger::endL;
      //theError = ERR_get_error(3ssl);
      //if (theError!=SSL_ERROR_WANT_READ && theError!=SSL_ERROR_WANT_WRITE)
      //  maxNumHandshakeTries =1;
        break;
      default:
        logOpenSSL << logger::red << "Unknown error. " << logger::endL;
        maxNumHandshakeTries = 1;
        break;
      }
      logOpenSSL << "Retrying connection in 0.5 seconds... ";
      theGlobalVariables.FallAsleep(500000);
    } else {
      this->flagSSLHandshakeSuccessful = true;
      break;
    }
  }
  return this->flagSSLHandshakeSuccessful;
}
