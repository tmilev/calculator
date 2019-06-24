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

extern logger logWorker  ;
extern logger logHttpErrors;
extern logger logBlock;
extern logger logIO   ;
extern logger logOpenSSL   ;
extern logger logProcessKills   ;
extern logger logProcessStats;
extern logger logPlumbing;
extern logger logServer  ;

TransportSecurityLayer::TransportSecurityLayer() {
  this->flagIsServer = true;
  this->flagInitializedPrivateKey = false;
  this->standardBufferSize = 10000;
}

TransportSecurityLayer::~TransportSecurityLayer() {
}

bool TransportSecurityLayer::flagDontUseOpenSSL = false;

void TransportSecurityLayer::initialize(bool IamServer) {
  if (!this->flagDontUseOpenSSL) {
    if (IamServer) {
      this->openSSLData.initSSLserver();
    } else {
      this->openSSLData.initSSLClient();
    }
  } else {
    crash << "Not implemented yet. " << crash;
  }
}

void TransportSecurityLayer::FreeEverythingShutdown() {
  if (!this->flagDontUseOpenSSL) {
    return this->openSSLData.FreeEverythingShutdownSSL();
  } else {
    crash << "not implemented yet. " << crash;
  }
}

bool TransportSecurityLayer::initSSLKeyFiles() {
  if (!this->flagDontUseOpenSSL) {
    return this->openSSLData.initSSLkeyFiles();
  } else {
    crash << "not implemented yet. " << crash;
    return false;
  }
}

bool TransportSecurityLayerOpenSSL::initSSLkeyFiles() {
  if (
    !FileOperations::FileExistsVirtual(TransportSecurityLayer::fileCertificate, true, true) ||
    !FileOperations::FileExistsVirtual(TransportSecurityLayer::fileKey, true, true)
  ) {
    logWorker << logger::red << "SSL is available but CERTIFICATE files are missing." << logger::endL;
    logWorker << logger::green << "Let me try to create those files for you." << logger::endL;
    std::stringstream theCommand;
    std::string certificatePhysicalName, keyPhysicalName, certificateConfiguration;
    FileOperations::GetPhysicalFileNameFromVirtual(
      TransportSecurityLayer::fileCertificate, certificatePhysicalName, true, true, 0
    );
    FileOperations::GetPhysicalFileNameFromVirtual(
      TransportSecurityLayer::fileKey, keyPhysicalName, true, true, 0
    );
    FileOperations::GetPhysicalFileNameFromVirtual(
      TransportSecurityLayer::fileCertificateConfiguration, certificateConfiguration, true, true, 0
    );
    theCommand <<  "openssl req -x509 -newkey rsa:2048 -nodes -keyout " << keyPhysicalName
    << " -out " << certificatePhysicalName
    << " -days 3001 ";
    if (
      theGlobalVariables.configuration["openSSLSubject"].type != JSData::JSUndefined &&
      theGlobalVariables.configuration["openSSLSubject"].type == JSData::JSstring
    ) {
      theCommand << "-subj " << theGlobalVariables.configuration["openSSLSubject"].string;
    }
    theGlobalVariables.CallSystemNoOutput(theCommand.str(), &logServer);
  }
  if (
    !FileOperations::FileExistsVirtual(TransportSecurityLayer::fileCertificate, true, true) ||
    !FileOperations::FileExistsVirtual(TransportSecurityLayer::fileKey, true, true)
  ) {
    theGlobalVariables.flagSSLIsAvailable = false;
    return false;
  } else {
    return true;
  }
  return false;
}



void TransportSecurityLayer::AddMoreEntropyFromTimer() {
  int fixMeAcquireEntropy;

}

void TransportSecurityLayerOpenSSL::initSSLClient() {
  this->initSSLCommon();
}

void TransportSecurityLayerOpenSSL::initSSLCommon() {
  this->initSSLlibrary();
  std::stringstream commentsOnError;
  this->theSSLMethod = SSLv23_method();
  this->contextServer = SSL_CTX_new(this->theSSLMethod);
  if (this->contextServer == 0) {
    logServer << logger::red << "Failed to create ssl context. " << logger::endL;
    ERR_print_errors_fp(stderr);
    crash << "Openssl context error.\n" << commentsOnError.str() << crash;
  }

}

void TransportSecurityLayerOpenSSL::initSSLserver() {
  this->initSSLCommon();
  if (this->owner->flagInitializedPrivateKey) {
    return;
  }
  this->owner->flagInitializedPrivateKey = true;
  std::string fileCertificatePhysical, fileKeyPhysical,
  singedFileCertificate1Physical, signedFileCertificate3Physical,
  signedFileKeyPhysical;
  FileOperations::GetPhysicalFileNameFromVirtual(
    TransportSecurityLayer::signedFileCertificate1, singedFileCertificate1Physical, true, true, 0
  );
  FileOperations::GetPhysicalFileNameFromVirtual(
    TransportSecurityLayer::signedFileCertificate3, signedFileCertificate3Physical, true, true, 0
  );
  FileOperations::GetPhysicalFileNameFromVirtual(
    TransportSecurityLayer::fileCertificate, fileCertificatePhysical, true, true, 0
  );
  FileOperations::GetPhysicalFileNameFromVirtual(
    TransportSecurityLayer::fileKey, fileKeyPhysical, true, true, 0
  );
  FileOperations::GetPhysicalFileNameFromVirtual(
    TransportSecurityLayer::signedFileKey, signedFileKeyPhysical, true, true, 0
  );
  //////////////////////////////////////////////////////////
  if (!this->initSSLkeyFiles()) {
    return;
  }
  logServer << logger::green << "SSL is available." << logger::endL;
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

bool TransportSecurityLayer::SSLReadLoop(int numTries,
  std::string& output,
  const LargeInt& expectedLength,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  bool includeNoErrorInComments
) {
  MacroRegisterFunctionWithName("TransportSecurityLayer::SSLReadLoop");
  output = "";
  int i = 0;
  std::string next;
  int numBytes = - 1;
  for (i = 0; i < numTries; i ++) {
    numBytes = this->SSLRead(
      this->buffer,
      outputError,
      commentsGeneral,
      includeNoErrorInComments
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
      this->buffer,
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

void TransportSecurityLayerOpenSSL::ClearErrorQueue(
  int errorCode,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  bool includeNoErrorInComments
) {
  MacroRegisterFunctionWithName("TransportSecurityLayer::ToStringError");
  int numErrors = 0;
  int theCode = SSL_get_error(this->sslData, errorCode);
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
      *outputError = TransportSecurityLayerOpenSSL::errors::errorWantRead;
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

void TransportSecurityLayerOpenSSL::DoSetSocket(int theSocket) {
  MacroRegisterFunctionWithName("TransportSecurityLayer::DoSetSocket");
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
}

void TransportSecurityLayerOpenSSL::SetSocketAddToStack(int theSocket) {
  MacroRegisterFunctionWithName("TransportSecurityLayer::SetSocketAddToStack");
  this->socketStack.AddOnTop(theSocket);
  this->DoSetSocket(theSocket);
}

void TransportSecurityLayer::Free() {
  if (!this->flagDontUseOpenSSL) {
    this->openSSLData.FreeSSL();
  } else {
    crash << "Remove last socket not implemented yet. " << crash;
  }
}

void TransportSecurityLayer::RemoveLastSocket() {
  if (!this->flagDontUseOpenSSL) {
    this->openSSLData.RemoveLastSocket();
  } else {
    crash << "Remove last socket not implemented yet. " << crash;
  }
}

void TransportSecurityLayerOpenSSL::RemoveLastSocket() {
  MacroRegisterFunctionWithName("TransportSecurityLayer::RemoveLastSocket");
  if (this->socketStack.size > 0) {
    int lastSocket = this->socketStack.PopLastObject();
    close(lastSocket);
  }
  if (this->socketStack.size <= 0) {
    return;
  }
  this->DoSetSocket(*this->socketStack.LastObject());
}

bool TransportSecurityLayerOpenSSL::HandShakeIamClientNoSocketCleanup(
  int inputSocketID,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  this->FreeSSL();
  this->sslData = SSL_new(this->contextServer);
  if (this->sslData == 0) {
    this->flagSSLHandshakeSuccessful = false;
    logOpenSSL << logger::red << "Failed to allocate ssl" << logger::endL;
    crash << "Failed to allocate ssl: not supposed to happen" << crash;
  }
  this->SetSocketAddToStack(inputSocketID);
  int maxNumHandshakeTries = 4;
  for (int i = 0; i < maxNumHandshakeTries; i ++) {
    this->errorCode = SSL_connect(this->sslData);
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
      switch(SSL_get_error(this->sslData, this->errorCode)) {
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

bool TransportSecurityLayer::HandShakeIamServer(int inputSocketID) {
  if (!this->flagDontUseOpenSSL) {
    return this->openSSLData.HandShakeIamServer(inputSocketID);
  } else {
    crash << "Not implemented yet. " << crash;
    return false;
  }
}

bool TransportSecurityLayer::HandShakeIamClientNoSocketCleanup(
  int inputSocketID,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  MacroRegisterFunctionWithName("WebServer::HandShakeIamClientNoSocketCleanup");
  if (!this->flagDontUseOpenSSL) {
    return this->HandShakeIamClientNoSocketCleanup(inputSocketID, commentsOnFailure, commentsGeneral);
  } else {
    crash << "Handshake not implemented yet. " << crash;
    return false;
  }
}

bool TransportSecurityLayerOpenSSL::InspectCertificates(
  std::stringstream *commentsOnFailure, std::stringstream *commentsGeneral
) {
  MacroRegisterFunctionWithName("TransportSecurityLayer::InspectCertificates");
  this->peer_certificate = SSL_get_peer_certificate(this->sslData);
  if (this->peer_certificate != 0) {
    char* tempCharPtr = 0;
    if (commentsGeneral != 0) {
      *commentsGeneral << "SSL connection using: "
      << SSL_get_cipher(this->sslData) << ".<br>\n";
    }
    tempCharPtr = X509_NAME_oneline(X509_get_subject_name (this->peer_certificate), 0, 0);
    if (tempCharPtr == 0) {
      if (commentsOnFailure != 0) {
        *commentsOnFailure << "X509_NAME_oneline return null; this is not supposed to happen. <br>\n";
      }
      return false;
    }
    this->otherCertificateSubjectName = tempCharPtr;
    free(tempCharPtr);
    tempCharPtr = 0;
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
    free(tempCharPtr);
    tempCharPtr = 0;
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
      << SSL_get_cipher(this->sslData) << ". "
      << "No client certificate.<br>\n";
    }
    return false;
  }
}

int TransportSecurityLayerOpenSSL::SSLRead(
  List<char> &readBuffer, std::string *outputError, std::stringstream *commentsGeneral, bool includeNoErrorInComments
) {
  ERR_clear_error();
  int result = SSL_read(this->sslData, readBuffer.TheObjects, readBuffer.size);
  this->ClearErrorQueue(
    result, outputError, commentsGeneral, includeNoErrorInComments
  );
  return result;

}

int TransportSecurityLayer::SSLRead(
  List<char>& readBuffer,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  bool includeNoErrorInComments
) {
  this->buffer.SetSize(100000);
  if (!this->flagDontUseOpenSSL) {
    return this->openSSLData.SSLRead(readBuffer, outputError, commentsGeneral, includeNoErrorInComments);
  } else {
    crash << "SSL read not implemented yet. " << crash;
    return - 1;
  }
}

int TransportSecurityLayerOpenSSL::SSLWrite(
  List<char>& writeBuffer,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  std::stringstream* commentsOnError,
  bool includeNoErrorInComments
) {
  (void) commentsGeneral;
  ERR_clear_error();
  if (this->owner->standardBufferSize > 0 && writeBuffer.size < this->owner->standardBufferSize) {
    writeBuffer.SetSize(this->owner->standardBufferSize);
  }
  int result = SSL_write(this->sslData, writeBuffer.TheObjects, writeBuffer.size);
  this->ClearErrorQueue(
    result, outputError, commentsOnError, includeNoErrorInComments
  );
  return result;
}

int TransportSecurityLayer::SSLWrite(
  List<char>& writeBuffer,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  std::stringstream* commentsOnError,
  bool includeNoErrorInComments
) {
  if (!this->flagDontUseOpenSSL) {
    return this->SSLWrite(writeBuffer, outputError, commentsGeneral, commentsOnError, includeNoErrorInComments);
  } else {
    crash << "Not imeplemented yet. " << crash;
    return - 1;
  }
}

bool TransportSecurityLayerOpenSSL::HandShakeIamServer(int inputSocketID) {
  MacroRegisterFunctionWithName("WebServer::HandShakeIamServer");
  std::stringstream commentsOnSSLNew;
  this->sslData = SSL_new(this->contextServer);
  if (this->sslData == 0) {
    logOpenSSL << logger::red << "Failed to allocate ssl: " << commentsOnSSLNew.str() << logger::endL;
    crash << "Failed to allocate ssl: not supposed to happen" << crash;
  }
  this->SetSocketAddToStack(inputSocketID);
  int maxNumHandshakeTries = 3;
  this->flagSSLHandshakeSuccessful = false;
  for (int i = 0; i < maxNumHandshakeTries; i ++) {
    std::stringstream commentsOnError;
    this->errorCode = SSL_accept(this->sslData);
    if (this->errorCode != 1) {
      this->flagSSLHandshakeSuccessful = false;
      if (this->errorCode == 0) {
        logOpenSSL << "OpenSSL handshake not successful in a controlled manner. ";
      } else {
        logOpenSSL << "OpenSSL handshake not successful with a fatal error. ";
      }
      logOpenSSL << commentsOnError.str() << logger::endL;
      logOpenSSL << "Attempt " << i + 1 << " out of " << maxNumHandshakeTries << " failed. ";
      switch(SSL_get_error(this->sslData, this->errorCode)) {
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

TransportSecurityLayerOpenSSL::TransportSecurityLayerOpenSSL() {
  this->peer_certificate = 0;
  this->sslData = 0;
  this->errorCode = - 1;
  this->theSSLMethod = 0;
  this->contextServer = 0;
  this->owner = 0;
  this->flagSSLHandshakeSuccessful = false;
}
