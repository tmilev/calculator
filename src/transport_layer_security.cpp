//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".

#include "transport_layer_security.h"
#include "general_logging_global_variables.h"
#include "general_file_operations_encodings.h"
#include "crypto.h"
#include "abstract_syntax_notation_one_decoder.h"
#include "serialization_basic.h"

#include <unistd.h> //<- close, open defined here
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <iomanip>

extern ProjectInformationInstance projectInfoInstanceTransportLayerSecurityImplementation;
ProjectInformationInstance projectInfoInstanceTransportLayerSecurityImplementation(__FILE__, "TSL/ssl implementation.");

// http://stackoverflow.com/questions/10175812/how-to-create-a-self-signed-certificate-with-openssl
// openssl req -x509 -keyalg RSA -newkey rsa:2048 -nodes -keyout key.pem -out cert.pem -days 3001
// Alternatively:
// certificate with certificate signing request:
// openssl req -out CSR.csr -new -newkey rsa:2048 -nodes -keyout calculator-algebra.key
// then get the CSR.csr file to a signing authority,
// from where you get the signedFileCertificate1 and signedFileCertificate3
const std::string TransportLayerSecurity::fileCertificate = "certificates/cert.pem";
const std::string TransportLayerSecurity::fileKey = "certificates/key.pem";
const std::string TransportLayerSecurity::signedFileCertificate1 = "certificates/calculator-algebra.crt";
const std::string TransportLayerSecurity::signedFileCertificate3 = "certificates/godaddy-signature.crt";
const std::string TransportLayerSecurity::signedFileKey = "certificates/calculator-algebra.key";

extern logger logWorker  ;
extern logger logHttpErrors;
extern logger logBlock;
extern logger logIO   ;
extern logger logOpenSSL   ;
extern logger logProcessKills   ;
extern logger logProcessStats;
extern logger logPlumbing;
extern logger logServer  ;

TransportLayerSecurity::TransportLayerSecurity() {
  this->flagIsServer = true;
  this->flagInitializedPrivateKey = false;
  this->flagInitialized = false;
  this->openSSLData.owner = this;
  this->readBufferStandardSize = 100000;
  this->flagUseBuiltInTlS = false;
}

TransportLayerSecurity::~TransportLayerSecurity() {
}

bool TransportLayerSecurity::flagBuiltInTLSAvailable = false;

void TransportLayerSecurity::initializeNonThreadSafePartsCommon() {
  ASNObject::initializeNonThreadSafe();
}

void TransportLayerSecurity::initializeNonThreadSafeOnFirstCall(bool IamServer) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::initializeNonThreadSafeOnFirstCall");
  if (this->flagInitialized) {
    return;
  }
  this->initializeNonThreadSafePartsCommon();
  if (IamServer) {
    this->openSSLData.initSSLServer();
    if (this->flagBuiltInTLSAvailable) {
      this->theServer.initialize();
      if (theGlobalVariables.flagAutoUnitTest) {
        logServer << logger::yellow
        << "Testing cryptographic functions ..." << logger::endL;
        SSLRecord::Test::Serialization();
        Crypto::Test::Sha256();
        PrivateKeyRSA::Test::All();
        X509Certificate::Test::All();
        logServer << logger::green << "Cryptographic tests done." << logger::endL;
      }
    }
  } else {
    this->openSSLData.initSSLClient();
  }
  this->flagInitialized = true;
}

void TransportLayerSecurity::FreeEverythingShutdown() {
  MacroRegisterFunctionWithName("TransportLayerSecurity::FreeEverythingShutdown");
  this->openSSLData.FreeEverythingShutdownSSL();
}

bool TransportLayerSecurity::initSSLKeyFiles(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::initSSLKeyFiles");
  if (!this->openSSLData.initSSLKeyFiles()) {
    return false;
  }
  if (!this->flagBuiltInTLSAvailable) {
    return true;
  }
  if (!this->initSSLKeyFilesInternal(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      crash << commentsOnFailure->str();
    }
    crash << "Failed to initialize ssl keys of built-in tls server. " << crash;
  }
  return true;
}

bool TransportLayerSecurityOpenSSL::initSSLKeyFilesCreateOnDemand() {
  MacroRegisterFunctionWithName("TransportLayerSecurityOpenSSL::initSSLKeyFilesCreateOnDemand");
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
}

bool TransportLayerSecurity::SSLReadLoop(
  int numTries,
  std::string& output,
  const LargeInteger& expectedLength,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  bool includeNoErrorInComments
) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::SSLReadLoop");
  this->readBuffer.SetSize(this->readBufferStandardSize);
  output = "";
  int i = 0;
  std::string next;
  int numBytes = - 1;
  for (i = 0; i < numTries; i ++) {
    numBytes = this->SSLRead(
      outputError,
      commentsGeneral,
      includeNoErrorInComments
    );
    if (numBytes > 0) {
      next.assign(this->readBuffer.TheObjects, static_cast<unsigned>(numBytes));
      output += next;
      if (expectedLength <= 0) {
        break;
      } else {
        if (expectedLength <= static_cast<int>(output.size())) {
          break;
        }
      }
    }
    if (numBytes == 0) {
      break;
    }
  }
  if (numBytes < 0) {
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "SSL-ERROR reading from socket. ";
    }
    return false;
  }
  return true;
}

bool TransportLayerSecurity::SSLWriteLoop(
  int numTries,
  const std::string& input,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  bool includeNoErrorInComments
) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::SSLWriteLoop");
  Crypto::ConvertStringToListBytesSigned(input, this->writeBuffer);
  int i = 0;
  int numBytes = - 1;
  for (i = 0; i < numTries; i ++) {
    numBytes = this->SSLWrite(
      this->writeBuffer,
      outputError,
      commentsGeneral,
      commentsGeneral,
      includeNoErrorInComments
    );
    if (numBytes == this->writeBuffer.size) {
      break;
    }
  }
  if (numBytes != this->writeBuffer.size) {
    if (commentsGeneral != nullptr) {
      *commentsGeneral  << i << " errors writing to socket.\n NumBytes: " << numBytes << ". ";
    }
    return false;
  }
  return true;
}

void TransportLayerSecurityOpenSSL::ClearErrorQueue(
  int errorCode,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  bool includeNoErrorInComments
) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::ToStringError");

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
}

void TransportLayerSecurityOpenSSL::DoSetSocket(int theSocket) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::DoSetSocket");
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

void TransportLayerSecurityOpenSSL::SetSocketAddToStack(int theSocket) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::SetSocketAddToStack");
  this->socketStack.AddOnTop(theSocket);
  this->DoSetSocket(theSocket);
}

void TransportLayerSecurity::Free() {
  this->openSSLData.FreeSSL();
  this->flagInitialized = false;
}

void TransportLayerSecurity::RemoveLastSocket() {
  this->openSSLData.RemoveLastSocket();
}

void TransportLayerSecurityOpenSSL::RemoveLastSocket() {
  MacroRegisterFunctionWithName("TransportLayerSecurity::RemoveLastSocket");
  if (this->socketStack.size > 0) {
    int lastSocket = this->socketStack.PopLastObject();
    close(lastSocket);
  }
  if (this->socketStack.size <= 0) {
    return;
  }
  this->DoSetSocket(*this->socketStack.LastObject());
}

bool TransportLayerSecurityOpenSSL::HandShakeIamClientNoSocketCleanup(
  int inputSocketID,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  this->FreeSSL();
  this->initSSLClient();
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
}

CipherSuiteSpecification TransportLayerSecurityServer::GetCipherCrashIfUnknown(int inputId) {
  if (inputId == 0) {
    crash << "Zero cipher suite specification not allowed here. " << crash;
  }
  if (!this->cipherSuiteNames.Contains(inputId)) {
    crash << "Unknown cipher suite specification not allowed here. " << crash;
  }
  CipherSuiteSpecification result;
  result.id = inputId;
  result.owner = this;
  result.ComputeName();
  return result;
}

void TransportLayerSecurityServer::AddSupportedCipher(int inputId) {
  this->supportedCiphers.SetKeyValue(inputId, this->GetCipherCrashIfUnknown(inputId));
}

void TransportLayerSecurityServer::initializeExtensions() {
  // Extension specification should be available here:
  // https://www.iana.org/assignments/tls-extensiontype-values/tls-extensiontype-values.xhtml
  this->extensionNames.SetKeyValue(
    SSLContent::tokensExtension::signatureAlgorithms,
    "signature algorithms"
  );
  this->extensionNames.SetKeyValue(
    SSLContent::tokensExtension::renegotiateConnection, "renegotiate"
  );
  this->extensionNames.SetKeyValue(
    SSLContent::tokensExtension::ellipticCurvePointFormat, "elliptic curve point format"
  );
  this->extensionNames.SetKeyValue(
    SSLContent::tokensExtension::requestOnlineCertificateStatus, "request online certificate status"
  );
  this->extensionNames.SetKeyValue(
    SSLContent::tokensExtension::serverName, "server name"
  );
  this->extensionNames.SetKeyValue(
    SSLContent::tokensExtension::extendedMasterSecret, "extended master secret"
  );
}

void TransportLayerSecurityServer::initializeCipherSuites() {
  if (this->cipherSuiteNames.size() != 0) {
    return;
  }
  this->cipherSuiteNames.SetKeyValue(0xc02f, "ECDHE/RSA/AES 128/GCM/SHA2"          ); // (RFC 5289)
  this->AddSupportedCipher(0xc02f);
  this->cipherSuiteNames.SetKeyValue(0x1301, "TLS_AES_128_GCM_SHA256"              ); //
  this->cipherSuiteNames.SetKeyValue(0x1302, "TLS_AES_256_GCM_SHA384"              ); //
  this->cipherSuiteNames.SetKeyValue(0x1303, "TLS_CHACHA20_POLY1305_SHA256"        ); //
  this->cipherSuiteNames.SetKeyValue(0xc02b, "ECDHE/ECDSA/AES 128/GCM/SHA2"        ); // (RFC 5289)
  this->cipherSuiteNames.SetKeyValue(0xc02c, "ECDHE/ECDSA/AES 128/GCM/SHA3"        ); // (RFC 5289)
  this->cipherSuiteNames.SetKeyValue(0xc030, "ECDHE/RSA/AES 256/GCM/SHA3"          ); // (RFC 5289)
  this->cipherSuiteNames.SetKeyValue(0xcca9, "ECDHE/ECDSA/CHACHA 20/POLY 1305/SHA2"); // (RFC 7905)
  this->cipherSuiteNames.SetKeyValue(0xcca8, "ECDHE/RSA/CHACHA 20/POLY 1305/SHA2"  ); // (RFC 7905)
  this->cipherSuiteNames.SetKeyValue(0xc013, "ECDHE/RSA/AES 128/CBC/SHA"           ); // (RFC 4492)
  this->cipherSuiteNames.SetKeyValue(0xc014, "ECDHE/RSA/AES 256/CBC/SHA"           ); // (RFC 4492)
  this->cipherSuiteNames.SetKeyValue(0x009c, "RSA/RSA/AES 128/GCM/SHA2"            ); // (RFC 5288)
  this->cipherSuiteNames.SetKeyValue(0x009d, "RSA/RSA/AES 256/GCM/SHA2"            ); // (RFC 5288)
  this->cipherSuiteNames.SetKeyValue(0x002f, "RSA/RSA/AES 128/CBC/SHA"             ); // (RFC 5246)
  this->cipherSuiteNames.SetKeyValue(0x0035, "RSA/RSA/AES 256/CBC/SHA"             ); // (RFC 5246)
  this->cipherSuiteNames.SetKeyValue(0x000a, "RSA/RSA/3DES/CBC/SHA"                ); // (RFC 5246)
  this->cipherSuiteNames.SetKeyValue(0,      "unknown"                             );
}

bool TransportLayerSecurityServer::initializeAll(
  List<unsigned char>& inputServerCertificate,
  std::stringstream* commentsOnError
) {
  this->initialize();
  return this->certificate.LoadFromASNEncoded(inputServerCertificate, commentsOnError);
}

void TransportLayerSecurityServer::initialize() {
  this->initializeCipherSuites();
  this->initializeExtensions();
}

TransportLayerSecurityServer::NetworkSpoofer::NetworkSpoofer() {
  this->flagDoSpoof = false;
  this->currentInputMessageIndex = - 1;
  this->owner = nullptr;
}

TransportLayerSecurityServer::TransportLayerSecurityServer() {
  this->session.owner = this;
  this->millisecondsDefaultTimeOut = 5000;
  this->millisecondsTimeOut = this->millisecondsDefaultTimeOut;
  this->defaultBufferCapacity = 1000000;
  this->lastReaD.owner = this;
  this->serverHelloCertificate.owner = this;
  this->serverHelloKeyExchange.owner = this;
  this->serverHelloStart.owner = this;
  this->spoofer.owner = this;
}

bool TransportLayerSecurityServer::NetworkSpoofer::WriteSSLRecords(List<SSLRecord>& input) {
  this->outgoingMessages.AddOnTop(input);
  return true;
}

bool TransportLayerSecurityServer::WriteSSLRecords(
  List<SSLRecord>& input, std::stringstream* commentsOnFailure
) {
  if (this->spoofer.flagDoSpoof) {
    return this->spoofer.WriteSSLRecords(input);
  }
  this->outgoingBytes.SetSize(0);
  for (int i = 0; i < input.size; i ++) {
    input[i].WriteBytes(this->outgoingBytes, nullptr);
  }
  return this->WriteBytesOnce(this->outgoingBytes, commentsOnFailure);
}

bool TransportLayerSecurityServer::WriteBytesOnce(
  List<unsigned char>& input, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("TransportLayerSecurityServer::WriteBytesOnce");
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 5;  // 5 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  setsockopt(this->session.socketId, SOL_SOCKET, SO_RCVTIMEO, static_cast<void*>(&tv), sizeof(timeval));
  ssize_t numBytesSent = send(
    this->session.socketId,
    input.TheObjects,
    static_cast<size_t>(input.size),
    0
  );
  if (numBytesSent < 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Error receiving bytes. " << strerror(errno);
    }
  }
  return numBytesSent >= 0;
}

std::string TransportLayerSecurityServer::NetworkSpoofer::JSLabels::inputMessages = "inputMessages";
std::string TransportLayerSecurityServer::NetworkSpoofer::JSLabels::outputMessages = "outputMessages";
std::string TransportLayerSecurityServer::NetworkSpoofer::JSLabels::errorsOnInput = "errorsOnInput";
std::string TransportLayerSecurityServer::NetworkSpoofer::JSLabels::errorsOnOutput = "errorsOnOutput";

std::string TransportLayerSecurityServer::JSLabels::spoofer = "spoofer";
std::string TransportLayerSecurityServer::JSLabels::session = "session";

JSData TransportLayerSecurityServer::ToJSON() {
  JSData result;
  if (this->spoofer.flagDoSpoof) {
    result[TransportLayerSecurityServer::JSLabels::spoofer] = this->spoofer.ToJSON();
  }
  result[TransportLayerSecurityServer::JSLabels::session] = this->session.ToJSON();
  return result;
}

JSData TransportLayerSecurityServer::NetworkSpoofer::ToJSON() {
  JSData result, inputMessage, outputMessages, inErrors, outErrors;
  result.theType = JSData::token::tokenArray;
  inputMessage.theType = JSData::token::tokenArray;
  outputMessages.theType = JSData::token::tokenArray;
  inErrors.theType = JSData::token::tokenArray;
  outErrors.theType = JSData::token::tokenArray;
  for (int i = 0; i < this->incomingMessages.size; i ++) {
    inputMessage[i] = this->incomingMessages[i].ToJSON();
  }
  inErrors = this->errorsOnIncoming;
  outErrors = this->errorsOnOutgoing;
  for (int i = 0; i < this->outgoingMessages.size; i ++) {
    JSData currentOutputMessages;
    currentOutputMessages.theType = JSData::token::tokenArray;
    for (int j = 0; j < this->outgoingMessages[i].size; j ++) {
      currentOutputMessages[j] = this->outgoingMessages[i][j].ToJSON();
    }
    outputMessages[i] = currentOutputMessages;
  }
  result[TransportLayerSecurityServer::NetworkSpoofer::JSLabels::errorsOnInput] = inErrors;
  result[TransportLayerSecurityServer::NetworkSpoofer::JSLabels::errorsOnOutput] = outErrors;
  result[TransportLayerSecurityServer::NetworkSpoofer::JSLabels::inputMessages] = inputMessage;
  result[TransportLayerSecurityServer::NetworkSpoofer::JSLabels::outputMessages] = outputMessages;
  return result;
}

bool TransportLayerSecurityServer::NetworkSpoofer::ReadBytesOnce(
  std::stringstream* commentsOnError
) {
  logWorker << logger::red
  << "Transport layer security server is spoofing the network. " << logger::endL;
  if (
    this->currentInputMessageIndex < 0 ||
    this->currentInputMessageIndex >= this->incomingMessages.size
  ) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "No available spoofed messages. ";
    }
    return false;
  }
  this->owner->incomingBytes = this->incomingMessages[
    this->currentInputMessageIndex
  ].incomingBytes;
  this->currentInputMessageIndex ++;
  return true;
}

bool TransportLayerSecurityServer::ReadBytesOnce(std::stringstream* commentsOnError) {
  MacroRegisterFunctionWithName("TransportLayerSecurityServer::ReadBytesOnce");
  if (this->spoofer.flagDoSpoof) {
    return this->spoofer.ReadBytesOnce(commentsOnError);
  }
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 5;  // 5 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  setsockopt(this->session.socketId, SOL_SOCKET, SO_RCVTIMEO, static_cast<void*>(&tv), sizeof(timeval));
  this->incomingBytes.SetSize(this->defaultBufferCapacity);
  int numBytesInBuffer = static_cast<int>(recv(
    this->session.socketId,
    this->incomingBytes.TheObjects,
    static_cast<unsigned>(this->incomingBytes.size - 1),
    0
  ));
  if (numBytesInBuffer >= 0) {
    this->incomingBytes.SetSize(numBytesInBuffer);
  }
  logWorker << "Read bytes:\n"
  << Crypto::ConvertListUnsignedCharsToHexFormat(this->incomingBytes, 40, false) << logger::endL;
  return numBytesInBuffer > 0;
}

TransportLayerSecurityServer& SSLContent::GetServer() const {
  return *this->owner->owner;
}

void SSLContent::resetExceptOwner() {
  this->theType           = 0;
  this->length            = 0;
  this->version           = 0;
  this->cipherSpecLength  = 0;
  this->challengeLength   = 0;
  this->compressionMethod = 0;
  this->flagRenegotiate            = false;
  this->flagIncomingRandomIncluded = false;
  this->flagOutgoingRandomIncluded = false;
  this->extensions             .SetSize(0);
  this->sessionId              .SetSize(0);
  this->challenge              .SetSize(0);
}

SSLContent::SSLContent() {
  this->owner = nullptr;
  this->resetExceptOwner();
}

logger::StringHighligher SSLContent::getStringHighlighter() {
  logger::StringHighligher result("2,4,4,2,6,4,64,2,64,4,||");
  for (int i = 0; i < this->owner->owner->session.incomingCiphers.size; i ++) {
    result.sections.AddOnTop(4);
  }
  result.sections.AddOnTop(std::string("||"));
  result.sections.AddOnTop(4);
  result.sections.AddOnTop(4);
  result.sections.AddOnTop(std::string("|"));
  for (int i = 0; i < this->extensions.size; i ++) {
    result.sections.AddOnTop(4);
    result.sections.AddOnTop(4);
    if (this->extensions[i].content.size > 0) {
      result.sections.AddOnTop(this->extensions[i].content.size * 2);
    }
  }
  return result;
}

std::string SSLContent::ToStringVersion() const {
  List<char> twoBytes;
  twoBytes.SetSize(2);
  twoBytes[0] = static_cast<char> (this->version / 256);
  twoBytes[1] = static_cast<char> (this->version % 256);
  std::stringstream out;
  out << Crypto::ConvertListCharsToHex(twoBytes, 0, false);
  return out.str();
}


std::string SSLContent::JSLabels::incomingRandom                    = "incomingRandom"                   ;
std::string SSLContent::JSLabels::outgoingRandom                    = "outgoingRandom"                   ;
std::string SSLContent::JSLabels::version                           = "version"                          ;
std::string SSLContent::JSLabels::length                            = "length"                           ;
std::string SSLContent::JSLabels::cipherSpecLength                  = "cipherSpecLength"                 ;
std::string SSLContent::JSLabels::sessionId                         = "sessionId"                        ;
std::string SSLContent::JSLabels::extensionsLength                  = "extensionsLength"                 ;
std::string SSLContent::JSLabels::compressionMethod                 = "compressionMethod"                ;
std::string SSLContent::JSLabels::extensions                        = "extensions"                       ;
std::string SSLContent::JSLabels::renegotiate                       = "renegotiate"                      ;
std::string SSLContent::JSLabels::OCSPrequest                       = "OCSPrequest"                      ;
std::string SSLContent::JSLabels::signedCertificateTimestampRequest = "signedCertificateTimestampRequest";

JSData SSLContent::ToJSON() const {
  MacroRegisterFunctionWithName("SSLHello::ToJSON");
  JSData result;
  result[SSLContent::JSLabels::version] = this->ToStringVersion();
  result[SSLContent::JSLabels::length] = this->length;
  result[SSLContent::JSLabels::cipherSpecLength] = this->cipherSpecLength;
  result[SSLContent::JSLabels::sessionId] = Crypto::ConvertListUnsignedCharsToHex(this->sessionId);
  result[SSLContent::JSLabels::extensionsLength ] = this->extensionsLength;
  result[SSLContent::JSLabels::compressionMethod] = Crypto::ConvertIntToHex(this->compressionMethod, 2);
  JSData extensionsObject;
  extensionsObject.theType = JSData::token::tokenArray;
  for (int i = 0; i < this->extensions.size; i ++) {
    extensionsObject.theList.AddOnTop(this->extensions[i].ToJSON());
  }
  result[SSLContent::JSLabels::extensions] = extensionsObject;
  result[SSLContent::JSLabels::renegotiate                      ] = this->flagRenegotiate;
  return result;
}

unsigned int CipherSuiteSpecification::HashFunction(const CipherSuiteSpecification &input) {
  return input.HashFunction();
}

unsigned int CipherSuiteSpecification::HashFunction() const {
  return static_cast<unsigned>(this->id);
}

CipherSuiteSpecification::CipherSuiteSpecification() {
  this->owner = nullptr;
}

bool CipherSuiteSpecification::CheckInitialization() const {
  if (this->owner == nullptr) {
    crash << "CipherSuiteSpecification not initialized correctly. " << crash;
  }
  return true;
}

bool CipherSuiteSpecification::ComputeName() {
  MacroRegisterFunctionWithName("CipherSuiteSpecification::ComputeName");
  this->CheckInitialization();
  if (!this->owner->cipherSuiteNames.Contains(this->id)) {
    // GREASE = deliberately invalid cipher suite code.
    // [Generate Random Extensions and Sustain Extensibility]
    this->name = "unknown/GREASE";
    return false;
  }
  this->name = this->owner->cipherSuiteNames.GetValueConstCrashIfNotPresent(this->id);
  return true;
}

void SSLRecord::WriteBytes(List<unsigned char>& output, List<Serialization::Marker>* annotations) const {
  MacroRegisterFunctionWithName("SSLRecord::WriteBytes");
  Serialization::WriterOneByteInteger recordTypeWriter(
    SSLRecord::tokens::handshake, output, annotations, "SSL handshake tag"
  );
  Serialization::WriterTwoByteInteger writerVersion(this->version, output, annotations, "SSL record version");
  Serialization::LengthWriterTwoBytes writerLength(output, annotations, "SSL handshake body");
  this->content.WriteBytes(output, annotations);
}

void SSLContent::WriteBytes(List<unsigned char>& output, List<Serialization::Marker>* annotations) const {
  switch (this->theType) {
    case SSLContent::tokens::clientHello:
      return SSLContent::WriteBytesHandshakeClientHello(output, annotations);
    case SSLContent::tokens::serverHello:
      return SSLContent::WriteBytesHandshakeServerHello(output, annotations);
    case SSLContent::tokens::certificate:
      return SSLContent::WriteBytesHandshakeCertificate(output, annotations);
    case SSLContent::tokens::serverKeyExchange:
      return SSLContent::WriteBytesHandshakeSecretExchange(output, annotations);
  }
}

void SSLContent::WriteBytesHandshakeServerHello(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLHello::WriteBytesHandshakeServerHello");
  if (this->theType != SSLContent::tokens::serverHello) {
    crash << "Not allowed to serialize non server-hello content as server hello. " << crash;
  }
  this->WriteType(output, annotations);
  Serialization::LengthWriterThreeBytes writeLength(output, annotations, "SSL content body");
  this->WriteVersion(output, annotations);
  this->WriteBytesMyRandomAndSessionId(output, annotations);
  // 256 = 0x0100 = no compression:
  Serialization::WriterTwoByteInteger compressionWriter(256, output, annotations, "compression: none");
  this->WriteBytesExtensionsOnly(output, annotations);
}

std::string SSLContent::GetType(unsigned char theToken) {
  switch (theToken) {
  case SSLContent::tokens::certificate:
    return "certificate";
  case SSLContent::tokens::serverKeyExchange:
    return "server key exchange";
  default:
    return "unknown";
  }
}

void TransportLayerSecurityServer::Session::WriteNamedCurveAndPublicKey(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLContent::WriteNamedCurveAndPublicKey");
  Serialization::WriterOneByteInteger curveName(SSLContent::namedCurve, output, annotations, "named curve");
  Serialization::WriteTwoByteUnsignedAnnotated(
    this->chosenEllipticCurve,
    output,
    annotations,
    this->chosenEllipticCurveName
  );
  std::stringstream shouldNotBeNeeded;
  List<unsigned char> publicKeyBytes;
  bool mustBeTrue =
  this->ephemerealPublicKey.xCoordinate.theValue.WriteBigEndianFixedNumberOfBytes(
    publicKeyBytes, 32, &shouldNotBeNeeded
  );
  Serialization::WriteOneByteLengthFollowedByBytes(publicKeyBytes, output, annotations, "public key");
  if (!mustBeTrue) {
    crash << "Internally generated public key does not serialize properly. "
    << shouldNotBeNeeded.str()
    << crash;
  }
}

void SSLContent::WriteType(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  if (annotations != nullptr) {
    annotations->AddOnTop(Serialization::Marker(output.size, 1, this->GetType(this->theType)));
  }
  output.AddOnTop(this->theType);
}

void SSLContent::WriteVersion(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  Serialization::WriterTwoByteInteger(this->version, output, annotations, "SSL content version");
}

void SSLContent::WriteBytesHandshakeCertificate(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLHello::WriteBytesHandshakeCertificate");
  if (this->theType != SSLContent::tokens::certificate) {
    crash << "Not allowed to serialize non-certificate content as certificate. " << crash;
  }
  this->WriteType(output, annotations);
  Serialization::LengthWriterThreeBytes writeLength(output, annotations, "certificateCollection");
  Serialization::LengthWriterThreeBytes writeLengthFirstCertificate(output, annotations, "certificateBody");
  this->GetServer().certificate.WriteBytesASN1(output, annotations);
}

void SSLContent::WriteBytesHandshakeSecretExchange(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLContent::WriteBytesHandshakeSecretExchange");
  this->CheckInitialization();
  if (this->theType != SSLContent::tokens::serverKeyExchange) {
    crash << "Not allowed to serialize non-server key exchange as such. " << crash;
  }
  this->WriteType(output, annotations);
  Serialization::LengthWriterThreeBytes writeLength(output, annotations, "server key exchange");
  this->owner->owner->session.WriteNamedCurveAndPublicKey(output, annotations);
}

void SSLContent::WriteBytesHandshakeClientHello(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLHello::WriteBytesHandshakeClientHello");
  if (this->theType != SSLContent::tokens::clientHello) {
    crash << "Not allowed to serialize non client-hello content as client hello. " << crash;
  }
  this->WriteType(output, annotations);
  Serialization::LengthWriterThreeBytes writeLength(output, annotations, "Handshake clientHello body");
  this->WriteVersion(output, annotations);
  this->WriteBytesIncomingRandomAndSessionId(output, annotations);
  this->WriteBytesSupportedCiphers(output, annotations);
  // 256 = 0x0100 = no compression:
  Serialization::WriterTwoByteInteger noCompression(256, output, annotations, "compression: none");
  this->WriteBytesExtensionsOnly(output, annotations);
}

List<unsigned char>& SSLContent::GetMyRandomBytes() const {
  if (
    this->owner->owner->session.myRandomBytes.size !=
    this->LengthRandomBytesInSSLHello
  ) {
    crash << "Writing non-initialized SSL hello forbidden. " << crash;
  }
  return this->owner->owner->session.myRandomBytes;
}

void SSLContent::WriteBytesIncomingRandomAndSessionId(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLHello::WriteBytesIncomingRandomAndSessionId");
  Serialization::WriteBytesAnnotated(
    this->owner->owner->session.incomingRandomBytes, output, annotations, "client random"
  );
  Serialization::WriteOneByteLengthFollowedByBytes(this->sessionId, output, annotations, "session id");
}

void SSLContent::WriteBytesMyRandomAndSessionId(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLHello::WriteBytesMyRandomAndSessionId");
  Serialization::WriteBytesAnnotated(this->GetMyRandomBytes(), output, annotations, "server random");
  Serialization::WriteOneByteLengthFollowedByBytes(this->sessionId, output, annotations, "session id");
}

void SSLContent::WriteBytesSupportedCiphers(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLHello::WriteBytesSupportedCiphers");
  List<CipherSuiteSpecification>& ciphers = this->owner->owner->session.incomingCiphers;
  Serialization::WriterTwoByteInteger cipherNumberWriter(
    ciphers.size * 2,
    output,
    annotations,
    "number of ciphers"
  );
  for (int i = 0; i < ciphers.size; i ++) {
    Serialization::WriterTwoByteInteger currentCipher(
      ciphers[i].id, output, annotations, "cipher id"
    );
  }
}

bool SSLContent::DecodeSupportedCiphers(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("SSLHello::DecodeSupportedCiphers");
  if (!Serialization::ReadTwoByteInt(
    this->owner->incomingBytes,
    this->owner->offsetDecoded,
    this->cipherSpecLength,
    commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read cipher suite length. ";
    }
    return false;
  }
  int halfCipherLength = this->cipherSpecLength / 2;
  this->cipherSpecLength = halfCipherLength * 2; // make sure cipher spec length is even.
  if (this->cipherSpecLength + this->owner->offsetDecoded >= this->owner->incomingBytes.size) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Cipher suite length: " << this->cipherSpecLength << " exceeds message length. ";
    }
    return false;
  }
  List<CipherSuiteSpecification>& ciphers = this->owner->owner->session.incomingCiphers;
  if (ciphers.size > 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Ciphers already declared. ";
    }
    return false;
  }
  ciphers.SetSize(halfCipherLength);
  for (int i = 0; i < halfCipherLength; i ++) {
    ciphers[i].owner = this->owner->owner;
    if (!Serialization::ReadTwoByteInt(
      this->owner->incomingBytes,
      this->owner->offsetDecoded,
      ciphers[i].id,
      commentsOnFailure
    )) {
      // this should not happen.
      return false;
    }
    ciphers[i].ComputeName();
  }
  if (!this->owner->owner->session.ChooseCipher(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to choose a cipher. ";
    }
    return false;
  }
  return true;
}

void SSLContent::WriteBytesExtensionsOnly(
  List<unsigned char>& output, List<Serialization::Marker>* annotations
) const {
  MacroRegisterFunctionWithName("SSLHello::WriteBytesExtensionsOnly");
  Serialization::LengthWriterTwoBytes extensionsLength(output, annotations, "extensions");
  for (int i = 0; i < this->extensions.size; i ++) {
    this->extensions[i].WriteBytes(output, annotations);
  }
}

bool SSLContent::Decode(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("SSLHello::Decode");
  if (this->owner->incomingBytes.size == 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Empty message. ";
    }
    return false;
  }
  this->theType = this->owner->incomingBytes[this->owner->offsetDecoded];
  this->owner->offsetDecoded ++;
  if (
    this->theType != SSLContent::tokens::clientHello &&
    this->theType != SSLContent::tokens::serverHello
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Message does not appear to be a client/server hello. ";
    }
    return false;
  }
  if (!Serialization::ReadThreeByteInt(
    this->owner->incomingBytes,
    this->owner->offsetDecoded,
    this->length,
    commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read ssl record length. ";
    }
    return false;
  }
  if (this->length + this->owner->offsetDecoded > this->owner->incomingBytes.size) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Client hello length is too big. ";
    }
    return false;
  }
  if (!Serialization::ReadTwoByteInt(
    this->owner->incomingBytes,
    this->owner->offsetDecoded,
    this->version,
    commentsOnFailure
  )) {
    return false;
  }
  List<unsigned char> incomingRandomBytes;
  if (!Serialization::ReadBytesFixedLength(
    this->owner->incomingBytes,
    this->LengthRandomBytesInSSLHello,
    this->owner->offsetDecoded,
    incomingRandomBytes,
    commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to fetch session random bytes. ";
    }
    return false;
  }
  if (!this->owner->owner->session.SetIncomingRandomBytes(
    incomingRandomBytes, commentsOnFailure
  )) {
    return false;
  }
  this->flagIncomingRandomIncluded = true;

  if (!Serialization::ReadOneByteLengthFollowedByBytes(
    this->owner->incomingBytes,
    this->owner->offsetDecoded,
    nullptr,
    &this->sessionId,
    commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read session id. ";
    }
    return false;
  }
  if (!this->DecodeSupportedCiphers(commentsOnFailure)){
    return false;
  }
  if (!Serialization::ReadTwoByteInt(
    this->owner->incomingBytes, this->owner->offsetDecoded, this->compressionMethod, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Compression bytes not set. ";
    }
    return false;
  }
  // compression is ignored.
  return this->DecodeExtensions(commentsOnFailure);
}

bool SSLContent::DecodeExtensions(std::stringstream *commentsOnFailure) {
  MacroRegisterFunctionWithName("SSLHello::DecodeExtensions");
  if (this->owner->offsetDecoded + 2 >= this->owner->incomingBytes.size) {
    // No extensions
    return true;
  }
  if (!Serialization::ReadTwoByteInt(
    this->owner->incomingBytes,
    this->owner->offsetDecoded,
    this->extensionsLength,
    commentsOnFailure
  )) {
    return false;
  }
  int extensionsLimit = this->owner->offsetDecoded + this->extensionsLength;
  if (extensionsLimit > this->owner->incomingBytes.size) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Extension length exceeds message size. ";
    }
    return false;
  }
  this->extensions.SetSize(0);

  while (this->owner->offsetDecoded < extensionsLimit) {
    SSLHelloExtension incoming;
    incoming.owner = this;
    if (!Serialization::ReadTwoByteInt(
      this->owner->incomingBytes,
      this->owner->offsetDecoded,
      incoming.theType,
      commentsOnFailure
    )) {
      return false;
    }
    if (!Serialization::ReadTwoByteLengthFollowedByBytes(
      this->owner->incomingBytes,
      this->owner->offsetDecoded,
      nullptr,
      &incoming.content,
      commentsOnFailure
    )) {
      return false;
    }
    this->extensions.AddOnTop(incoming);
  }
  return this->ProcessExtensions(commentsOnFailure);
}

SSLHelloExtension::SSLHelloExtension() {
  this->owner = nullptr;
  this->theType = 0;
}

void SSLHelloExtension::WriteBytes(
  List<unsigned char>& output,
  List<Serialization::Marker>* annotations
) {
  Serialization::WriterTwoByteInteger writer(this->theType, output, annotations, "ssl hello extension type");
  Serialization::WriteTwoByteLengthFollowedByBytes(this->content, output, annotations, "ssl hello extension content");
}

void SSLHelloExtension::MakeGrease(SSLContent* inputOwner) {
  this->owner = inputOwner;
  this->content.SetSize(0);
  this->theType = (13 * 16 + 10) * 256 + (13 * 16 + 10); // 0xdada;
}

void SSLHelloExtension::MakeExtendedMasterSecret(SSLContent* inputOwner) {
  this->owner = inputOwner;
  this->content.SetSize(0);
  this->theType = SSLContent::tokensExtension::extendedMasterSecret;
}

void SSLHelloExtension::MakeEllipticCurvePointFormat(SSLContent* inputOwner) {
  this->owner = inputOwner;
  this->theType = SSLContent::tokensExtension::ellipticCurvePointFormat;
  this->content.SetSize(0);
  this->content.AddOnTop(0);
  this->content.AddOnTop(4);
  this->content.AddOnTop(3);
  this->content.AddOnTop(0);
  this->content.AddOnTop(1);
  this->content.AddOnTop(2);
}

bool SSLHelloExtension::CheckInitialization() {
  if (this->owner == nullptr) {
    crash << "Non-initialized owner of ssl hello extension. " << crash;
  }
  return true;
}

JSData SSLHelloExtension::ToJSON() {
  JSData result;
  result.theType = JSData::token::tokenObject;
  result["name"] = this->Name();
  std::stringstream hex;
  hex << std::hex << std::setfill('0') << std::setw(4) << this->theType;
  result["type"] = hex.str();
  result["data"] = Crypto::ConvertListUnsignedCharsToHex(this->content);
  return result;
}

std::string SSLHelloExtension::Name() {
  this->CheckInitialization();
  if (this->owner->owner->owner->extensionNames.Contains(this->theType)) {
    return this->owner->owner->owner->extensionNames.GetValueConstCrashIfNotPresent(this->theType);
  }
  return "unknown";
}

bool SSLHelloExtension::ProcessRenegotiateConnection(std::stringstream *commentsOnError) {
  if (this->content.size == 1) {
    if (this->content[0] == 0) {
      this->owner->flagRenegotiate = true;
      return true;
    }
  }
  if (commentsOnError != nullptr) {
    *commentsOnError << "Unexpected renegotiate connection extension content. ";
  }
  return false;
}

bool SSLHelloExtension::ProcessServerName(std::stringstream *commentsOnError) {
  TransportLayerSecurityServer::Session& session = this->owner->owner->owner->session;
  if (session.serverName.size > 0) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Server name extension can be set only once. ";
    }
    return false;
  }
  if (!Serialization::ReadTwoByteLengthFollowedByBytesDontOutputOffset(
    this->content, 0, nullptr, &session.serverName, commentsOnError
  )) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Failed to read server name extension. ";
    }
    return false;
  }
  return true;
}

bool SSLHelloExtension::ProcessRequestOnlineCertificateStatus(std::stringstream* commentsOnError) {
  (void) commentsOnError;
  this->owner->owner->owner->session.flagRequestOnlineCertificateStatusProtocol = true;
  return true;
}

bool SSLHelloExtension::ProcessRequestSignedCertificateTimestamp(std::stringstream* commentsOnError) {
  (void) commentsOnError;
  this->owner->owner->owner->session.flagRequestSignedCertificateTimestamp = true;
  return true;
}

SignatureAlgorithmSpecification::SignatureAlgorithmSpecification() {
  this->ownerServer = nullptr;
  this->signatureAlgorithm = 0;
  this->hash = 0;
  this->value = 0;
}

void SignatureAlgorithmSpecification::processValue() {
  // Works for all RSA and ECDSA-based algorithms:
  this->hash = this->value / 256;
  this->signatureAlgorithm = this->value % 256;
}

bool SSLHelloExtension::ProcessSignatureAlgorithms(std::stringstream* commentsOnError) {
  // See:
  // 1. [7.4.1.4.1., RFC5246]
  // 2. [page 41, RFC 8446].
  TransportLayerSecurityServer::Session& session = this->owner->owner->owner->session;
  int offset = 0;
  List<unsigned char> specifications;
  if (!Serialization::ReadTwoByteLengthFollowedByBytes(
    this->content, offset, 0, &specifications, commentsOnError
  )) {
    return false;
  }
  int numberOfPairs = specifications.size / 2;
  for (int i = 0; i < numberOfPairs; i ++) {
    SignatureAlgorithmSpecification incoming;
    incoming.ownerServer = this->owner->owner->owner;
    incoming.value = specifications[i * 2] * 256 + specifications[i * 2 + 1];
    incoming.processValue();
    session.incomingAlgorithmSpecifications.AddOnTop(incoming);
  }
  if (session.incomingAlgorithmSpecifications.size == 0) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Failed to recognize any hash / algorithm pair supported by the client. ";
    }
    return false;
  }
  return true;
}

bool SSLHelloExtension::ProcessMe(std::stringstream* commentsOnError) {
  switch (this->theType) {
  case SSLContent::tokensExtension::renegotiateConnection:
    return this->ProcessRenegotiateConnection(commentsOnError);
  case SSLContent::tokensExtension::serverName:
    return this->ProcessServerName(commentsOnError);
  case SSLContent::tokensExtension::requestOnlineCertificateStatus:
    return this->ProcessRequestOnlineCertificateStatus(commentsOnError);
  case SSLContent::tokensExtension::requestSignedCertificateTimestamp:
    return this->ProcessRequestSignedCertificateTimestamp(commentsOnError);
  case SSLContent::tokensExtension::signatureAlgorithms:
    return this->ProcessSignatureAlgorithms(commentsOnError);
  default:
    break;
  }
  return true;
}

bool SSLContent::ProcessExtensions(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("SSLHello::ProcessExtensions");
  for (int i = 0; i < this->extensions.size; i ++) {
    if (!this->extensions[i].ProcessMe(commentsOnFailure)) {
      return false;
    }
  }
  return true;
}

bool Serialization::ReadTwoByteInt(
  const List<unsigned char>& input, int& inputOutputOffset, int& result, std::stringstream* commentsOnFailure
) {
  return Serialization::ReadNByteInt(2, input, inputOutputOffset, result, commentsOnFailure);
}

bool Serialization::ReadThreeByteInt(
  const List<unsigned char>& input, int& inputOutputOffset, int& result, std::stringstream* commentsOnFailure
) {
  return Serialization::ReadNByteInt(3, input, inputOutputOffset, result, commentsOnFailure);
}

bool Serialization::ReadNByteInt(
  int numBytes,
  const List<unsigned char>& input,
  int& inputOutputOffset,
  int& result,
  std::stringstream* commentsOnFailure
) {
  if (numBytes > 4) {
    crash << "Not allowed to read more than 4 bytes into an integer. " << crash;
  }
  if (numBytes + inputOutputOffset > input.size) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "At least " << numBytes
      << " bytes required to read integer. Input has: " << input.size
      << " bytes and offset is: " << inputOutputOffset << ". ";
    }
    return false;
  }
  result = 0;
  for (int i = 0; i < numBytes; i ++) {
    result *= 256;
    result += static_cast<unsigned>(input[inputOutputOffset + i]);
  }
  inputOutputOffset += numBytes;
  return true;
}

bool Serialization::ReadBytesFixedLength(
  const List<unsigned char>& input,
  int desiredNumberOfBytes,
  int& inputOutputOffset,
  List<unsigned char>& output,
  std::stringstream* commentsOnError
) {
  if (desiredNumberOfBytes < 0) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Cannot read " << desiredNumberOfBytes << " bytes. ";
    }
    return false;
  }
  if (inputOutputOffset + desiredNumberOfBytes >= input.size) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Failed to read " << desiredNumberOfBytes
      << " bytes at offset: " << inputOutputOffset
      << ": this would exceed the input buffer size: " << input.size << ". ";
    }
    return false;
  }
  output.SetSize(desiredNumberOfBytes);
  for (int i = 0; i < desiredNumberOfBytes; i ++) {
    output[i] = input[inputOutputOffset + i];
  }
  inputOutputOffset += desiredNumberOfBytes;
  return true;
}

bool Serialization::ReadOneByteLengthFollowedByBytes(
  const List<unsigned char>& input,
  int& outputOffset,
  int* resultLength,
  List<unsigned char>* output,
  std::stringstream* commentsOnError
) {
  return Serialization::ReadNByteLengthFollowedByBytes(
    1, input, outputOffset, resultLength, output, commentsOnError
  );
}

bool Serialization::ReadTwoByteLengthFollowedByBytes(
  const List<unsigned char>& input,
  int& outputOffset,
  int* resultLength,
  List<unsigned char>* output,
  std::stringstream* commentsOnError
) {
  return Serialization::ReadNByteLengthFollowedByBytes(
    2, input, outputOffset, resultLength, output, commentsOnError
  );
}

bool Serialization::ReadTwoByteLengthFollowedByBytesDontOutputOffset(
  const List<unsigned char>& input,
  int offset,
  int* resultLength,
  List<unsigned char>* output,
  std::stringstream* commentsOnError
) {
  int outputOffset = offset;
  return Serialization::ReadNByteLengthFollowedByBytes(
    2, input, outputOffset, resultLength, output, commentsOnError
  );
}

bool Serialization::ReadNByteLengthFollowedByBytes(
  int numBytesLength,
  const List<unsigned char>& input,
  int& outputOffset,
  int* resultLength,
  List<unsigned char>* output,
  std::stringstream* commentsOnError
) {
  int temp = 0;
  if (resultLength == nullptr) {
    resultLength = &temp;
  }
  if (!Serialization::ReadNByteInt(
    numBytesLength, input, outputOffset, *resultLength, commentsOnError
  )) {
    return false;
  }
  if (*resultLength + outputOffset > input.size) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Indicated byte sequence length exceeds input size. ";
    }
    return false;
  }
  if (output != nullptr) {
    output->SetSize(*resultLength);
    for (int i = 0; i < *resultLength; i ++) {
      (*output)[i] = input[outputOffset + i];
    }
  }
  outputOffset += *resultLength;
  return true;
}

void Serialization::WriteTwoByteInt(
  int input,
  List<unsigned char>& output
) {
  int inputOutputOffset = output.size;
  return Serialization::WriteNByteUnsigned(2, static_cast<unsigned>(input), output, inputOutputOffset);
}

void Serialization::WriteThreeByteInt(
  int input,
  List<unsigned char>& output
) {
  int inputOutputOffset = output.size;
  return Serialization::WriteNByteUnsigned(3, static_cast<unsigned>(input), output, inputOutputOffset);
}

void Serialization::WriteTwoByteUnsignedAnnotated(
  unsigned int input,
  List<unsigned char>& output,
  List<Serialization::Marker>* annotations,
  const std::string& label
) {
  if (annotations != nullptr) {
    annotations->AddOnTop(Serialization::Marker(output.size, 2, label));
  }
  Serialization::WriteTwoByteUnsigned(input, output);
}

void Serialization::WriteTwoByteUnsigned(
  unsigned int input,
  List<unsigned char>& output
) {
  int inputOutputOffset = output.size;
  return Serialization::WriteNByteUnsigned(2, input, output, inputOutputOffset);
}

void Serialization::WriteThreeByteUnsigned(
  unsigned int input,
  List<unsigned char>& output
) {
  int inputOutputOffset = output.size;
  return Serialization::WriteNByteUnsigned(3, input, output, inputOutputOffset);
}

void Serialization::WriteNByteUnsigned(
  int byteCountOfLength,
  unsigned int input,
  List<unsigned char>& output,
  int& inputOutputOffset
) {
  if (byteCountOfLength < 0 || byteCountOfLength > 4) {
    crash << "Invalid byteCountOfLength: " << byteCountOfLength << crash;
  }
  if (inputOutputOffset + byteCountOfLength > output.size) {
    output.SetSize(inputOutputOffset + byteCountOfLength);
  }
  for (int i = byteCountOfLength - 1 + inputOutputOffset; i >= inputOutputOffset; i --) {
    output[i] = static_cast<unsigned char>(input % 256);
    input /= 256;
  }
  if (input > 0) {
    crash << "Input has more significant digits than the number of bytes allow. " << crash;
  }
  inputOutputOffset += byteCountOfLength;
}

void Serialization::WriteOneByteLengthFollowedByBytes(
  const List<unsigned char>& input,
  List<unsigned char>& output,
  List<Serialization::Marker>* annotations,
  const std::string& label
) {
  Serialization::WriteNByteLengthFollowedByBytes(
    1, input, output, annotations, label
  );
}

void Serialization::WriteTwoByteLengthFollowedByBytes(
  const List<unsigned char>& input,
  List<unsigned char>& output,
  List<Serialization::Marker>* annotations,
  const std::string& label
) {
  Serialization::WriteNByteLengthFollowedByBytes(
    2, input, output, annotations, label
  );
}

std::string Serialization::ConvertListUnsignedCharsToHex(const List<unsigned char>& input) {
  return Crypto::ConvertListUnsignedCharsToHexFormat(input, 0, false);
}

void Serialization::WriteBytesAnnotated(
  const List<unsigned char>& input,
  List<unsigned char>& output,
  List<Serialization::Marker>* annotations,
  const std::string& label
) {
  if (annotations != nullptr) {
    annotations->AddOnTop(Serialization::Marker(output.size, input.size, label));
  }
  output.AddListOnTop(input);
}

void Serialization::WriteNByteLengthFollowedByBytes(
  int byteCountOfLength,
  const List<unsigned char>& input,
  List<unsigned char>& output,
  List<Serialization::Marker>* annotations,
  const std::string& label
) {
  Serialization::WriterIntegerWithMarker writer(
    byteCountOfLength, static_cast<unsigned>(input.size), output, annotations, label
  );
  output.AddListOnTop(input);
}

bool SSLRecord::CheckInitialization() const {
  if (this->owner == nullptr) {
    crash << "Uninitialized ssl record. " << crash;
  }
  return true;
}

SSLRecord::SSLRecord() {
  this->theType = SSLRecord::tokens::unknown;
  this->length = 0;
  this->version = 0;
  this->offsetDecoded = 0;
  this->content.owner = this;
  this->owner = nullptr;
}

std::string Serialization::JSLabels::markers = "markers";
std::string Serialization::JSLabels::body = "body";
std::string Serialization::JSLabels::length = "length";
std::string Serialization::JSLabels::offset = "offset";
std::string Serialization::JSLabels::label = "label";
std::string Serialization::JSLabels::serialization = "serialization";

std::string Serialization::Marker::ToString() const {
  std::stringstream out;
  out << "[" << this->offset << ", " << this->length << ", '" << this->label << "']";
  return out.str();
}

JSData Serialization::Marker::ToJSON() {
  JSData result;
  result.theType = JSData::token::tokenObject;
  result[Serialization::JSLabels::offset] = this->offset;
  result[Serialization::JSLabels::length] = this->length;
  result[Serialization::JSLabels::label] = this->label;
  return result;
}

std::string SSLRecord::ToHtml() {
  std::stringstream out, spanId;
  spanId << "spanSSLRecord_" << "_" << theGlobalVariables.GetElapsedMilliseconds();
  out << "<div id = '" << spanId.str() << "'></div>";
  out << "<script>"
  << "window.calculator.crypto.displaySSLRecord('"
  << spanId.str() << "', " << this->ToJSON() << ");"
  << "</script>";
  return out.str();
}

std::string SSLRecord::JSLabels::type = "type";
std::string SSLRecord::JSLabels::content = "content";
std::string SSLRecord::JSLabels::session = "session";

JSData SSLRecord::ToJSON() {
  MacroRegisterFunctionWithName("SSLRecord::ToJSON");
  JSData result;
  result[Serialization::JSLabels::serialization] = this->ToJSONSerialization();
  result[SSLRecord::JSLabels::type] = this->ToStringType();
  result[SSLRecord::JSLabels::content] = this->content.ToJSON();
  result[SSLRecord::JSLabels::session] = this->owner->session.ToJSON();
  return result;
}

std::string TransportLayerSecurityServer::Session::ToStringChosenCipher() {
  if (this->chosenCipher == 0) {
    return "unknown";
  }
  return this->owner->cipherSuiteNames.GetValueConstCrashIfNotPresent(this->chosenCipher);
}

std::string SignatureAlgorithmSpecification::JSLabels::hash = "hash";
std::string SignatureAlgorithmSpecification::JSLabels::hashName = "hashName";
std::string SignatureAlgorithmSpecification::JSLabels::signatureAlgorithm = "signatureAlgorithm";
std::string SignatureAlgorithmSpecification::JSLabels::signatureAlgorithmName = "signatureAlgorithmName";
std::string SignatureAlgorithmSpecification::JSLabels::valueHex = "valueHex";

std::string SignatureAlgorithmSpecification::GetSignatureAlgorithmName() {
  switch (this->signatureAlgorithm) {
  case SignatureAlgorithmSpecification::SignatureAlgorithm::anonymous:
    return "anonymous";
  case SignatureAlgorithmSpecification::SignatureAlgorithm::DSA:
    return "DSA";
  case SignatureAlgorithmSpecification::SignatureAlgorithm::ECDSA:
    return "ECDSA";
  case SignatureAlgorithmSpecification::SignatureAlgorithm::RSA:
    return "RSA";
  }
  return "unknown";
}

std::string SignatureAlgorithmSpecification::GetHashName() {
  switch (this->hash) {
  case SignatureAlgorithmSpecification::HashAlgorithm::md5:
    return "md5";
  case SignatureAlgorithmSpecification::HashAlgorithm::none:
    return "none";
  case SignatureAlgorithmSpecification::HashAlgorithm::sha1:
    return "sha1";
  case SignatureAlgorithmSpecification::HashAlgorithm::sha224:
    return "sha224";
  case SignatureAlgorithmSpecification::HashAlgorithm::sha256:
    return "sha256";
  case SignatureAlgorithmSpecification::HashAlgorithm::sha384:
    return "sha384";
  case SignatureAlgorithmSpecification::HashAlgorithm::sha512:
    return "sha512";
  }
  return "unknown";
}

std::string SignatureAlgorithmSpecification::ToHex() {
  return Crypto::ConvertIntToHex(this->value, 2);
}

JSData SignatureAlgorithmSpecification::ToJSON() {
  JSData result;
  result[SignatureAlgorithmSpecification::JSLabels::valueHex] = this->ToHex();
  result[SignatureAlgorithmSpecification::JSLabels::hash] = static_cast<int>(this->hash);
  result[SignatureAlgorithmSpecification::JSLabels::hashName] = this->GetHashName();
  result[SignatureAlgorithmSpecification::JSLabels::signatureAlgorithm] = static_cast<int>(this->signatureAlgorithm);
  result[SignatureAlgorithmSpecification::JSLabels::signatureAlgorithmName] = this->GetSignatureAlgorithmName();
  return result;
}

std::string TransportLayerSecurityServer::Session::JSLabels::chosenCipher                      = "chosenCipher"                     ;
std::string TransportLayerSecurityServer::Session::JSLabels::chosenCipherName                  = "chosenCipherName"                 ;
std::string TransportLayerSecurityServer::Session::JSLabels::incomingRandomBytes               = "incomingRandomBytes"              ;
std::string TransportLayerSecurityServer::Session::JSLabels::myRandomBytes                     = "myRandomBytes"                    ;
std::string TransportLayerSecurityServer::Session::JSLabels::OCSPrequest                       = "OnlineCertificateStatusProtocol"  ;
std::string TransportLayerSecurityServer::Session::JSLabels::signedCertificateTimestampRequest = "RequestSignedCertificateTimestamp";
std::string TransportLayerSecurityServer::Session::JSLabels::cipherSuites                      = "cipherSuites"                     ;
std::string TransportLayerSecurityServer::Session::JSLabels::serverName                        = "serverName"                       ;
std::string TransportLayerSecurityServer::Session::JSLabels::algorithmSpecifications           = "algorithmSpecifications"          ;
std::string TransportLayerSecurityServer::Session::JSLabels::ellipticCurveId                   = "ellipticCurveId"                  ;
std::string TransportLayerSecurityServer::Session::JSLabels::ellipticCurveName                 = "ellipticCurveName"                ;
std::string TransportLayerSecurityServer::Session::JSLabels::bytesToSign                       = "bytesToSign"                      ;

JSData TransportLayerSecurityServer::Session::ToJSON() {
  JSData result;
  result.theType = JSData::token::tokenArray;
  result[TransportLayerSecurityServer::Session::JSLabels::chosenCipher                     ] = Crypto::ConvertIntToHex(this->chosenCipher, 2);
  result[TransportLayerSecurityServer::Session::JSLabels::chosenCipherName                 ] = this->ToStringChosenCipher();
  result[TransportLayerSecurityServer::Session::JSLabels::incomingRandomBytes              ] = Crypto::ConvertListUnsignedCharsToHex(this->incomingRandomBytes);
  result[TransportLayerSecurityServer::Session::JSLabels::myRandomBytes                    ] = Crypto::ConvertListUnsignedCharsToHex(this->myRandomBytes);
  result[TransportLayerSecurityServer::Session::JSLabels::OCSPrequest                      ] = this->flagRequestOnlineCertificateStatusProtocol;
  result[TransportLayerSecurityServer::Session::JSLabels::signedCertificateTimestampRequest] = this->flagRequestSignedCertificateTimestamp;
  result[TransportLayerSecurityServer::Session::JSLabels::ellipticCurveId                  ] = this->chosenEllipticCurve;
  result[TransportLayerSecurityServer::Session::JSLabels::ellipticCurveName                ] = this->chosenEllipticCurveName;
  result[TransportLayerSecurityServer::Session::JSLabels::bytesToSign                      ] = Crypto::ConvertListUnsignedCharsToHex(this->bytesToSign);
  JSData ciphers;
  ciphers.theType = JSData::token::tokenObject;
  ciphers.theList.SetSize(this->incomingCiphers.size);
  for (int i = 0; i < this->incomingCiphers.size; i ++) {
    CipherSuiteSpecification& current = this->incomingCiphers[i];
    ciphers[Crypto::ConvertIntToHex(current.id, 2)] = current.name;
  }
  JSData algorithmSpecifications;
  algorithmSpecifications.theType = JSData::token::tokenArray;
  for (int i = 0; i < this->incomingAlgorithmSpecifications.size; i ++) {
    algorithmSpecifications.theList.AddOnTop(this->incomingAlgorithmSpecifications[i].ToJSON());
  }
  result[TransportLayerSecurityServer::Session::JSLabels::algorithmSpecifications] = algorithmSpecifications;
  result[TransportLayerSecurityServer::Session::JSLabels::cipherSuites] = ciphers;
  if (this->serverName.size > 0) {
    result[
      TransportLayerSecurityServer::Session::JSLabels::serverName
    ] = Crypto::ConvertListUnsignedCharsToHex(
      this->serverName
    );
  }
  return result;
}

JSData SSLRecord::ToJSONSerialization() {
  MacroRegisterFunctionWithName("SSLRecord::ToJSONSerialization");
  JSData result;
  List<unsigned char> serialization;
  List<Serialization::Marker> markers;
  this->WriteBytes(serialization, &markers);
  JSData jsMarkers;
  jsMarkers.theType = JSData::token::tokenArray;
  for (int i = 0; i < markers.size; i ++) {
    jsMarkers.theList.AddOnTop(markers[i].ToJSON());
  }
  result[Serialization::JSLabels::markers] = jsMarkers;
  result[Serialization::JSLabels::body] = Crypto::ConvertListUnsignedCharsToHex(serialization);
  return result;
}

std::string SSLRecord::ToStringType() const {
  switch (this->theType) {
  case SSLRecord::tokens::alert:
    return "alert";
  case SSLRecord::tokens::applicationData:
    return "applicationData";
  case SSLRecord::tokens::changeCipherSpec:
    return "changeCipherSpec";
  case SSLRecord::tokens::handshake:
    return "handshake";
  case SSLRecord::tokens::unknown:
    return "unknown";
  default:
    break;
  }
  std::stringstream out;
  out << "unknown type: " << static_cast<int>(this->theType);
  return out.str();
}

std::string SSLRecord::ToString() const {
  JSData result;
  result["length"] = this->length;
  result["incomingBytes"] = Crypto::ConvertListUnsignedCharsToHexFormat(this->incomingBytes, 50, false);
  result["type"] = this->ToStringType();
  if (this->theType == SSLRecord::tokens::handshake) {
    result["hello"] = this->content.ToJSON();
  }
  std::string hexVersion;
  Crypto::ConvertLargeUnsignedIntToHexSignificantDigitsFirst(
    LargeIntegerUnsigned(static_cast<unsigned>(this->version)), 0, hexVersion
  );
  result["version"] = hexVersion;
  return result.ToString(false, true, false, true);
}

bool SSLRecord::Decode(std::stringstream *commentsOnFailure) {
  MacroRegisterFunctionWithName("SSLRecord::Decode");
  this->CheckInitialization();
  if (this->incomingBytes.size < 5 + this->offsetDecoded) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "SSL record needs to have at least 5 bytes, yours has: "
      << this->incomingBytes.size << ". ";
    }
    return false;
  }
  this->theType = this->incomingBytes[this->offsetDecoded];
  this->offsetDecoded ++;
  if (
    this->theType != SSLRecord::tokens::handshake
  //&&
//    this->theType != SSLRecord::tokens::alert &&
//    this->theType != SSLRecord::tokens::applicationData &&
//    this->theType != SSLRecord::tokens::changeCipherSpec
  ) {
    this->theType = SSLRecord::tokens::unknown;
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Unknown record type: "
      << static_cast<int>(this->incomingBytes[this->offsetDecoded]) << ". ";
    }
    return false;
  }
  if (!Serialization::ReadTwoByteInt(
    this->incomingBytes, this->offsetDecoded, this->version, commentsOnFailure
  )) {
    return false;
  }
  if (!Serialization::ReadTwoByteInt(
    this->incomingBytes, this->offsetDecoded, this->length, commentsOnFailure
  )) {
    return false;
  }
  int highEnd = this->length + this->offsetDecoded;
  if (highEnd > this->incomingBytes.size) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Insufficient record length: expected: 5 + " << this->length
      << ", got: " << this->incomingBytes.size - this->offsetDecoded;
    }
    return false;
  }
  return this->DecodeBody(commentsOnFailure);
}

bool SSLRecord::DecodeBody(std::stringstream *commentsOnFailure) {
  switch (this->theType) {
    case SSLRecord::tokens::handshake:
      return this->content.Decode(commentsOnFailure);
    default:
      break;
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Unrecognized ssl record type: " << static_cast<int>(this->theType) << ". ";
  }
  return false;
}

bool TransportLayerSecurityServer::DecodeSSLRecord(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("TransportLayerSecurityServer::DecodeSSLRecord");
  if (!this->lastReaD.Decode(commentsOnFailure)) {
    logWorker << "DEBUG: one record decode error.\n" << this->lastReaD.ToString() << logger::endL;
    return false;
  }
  return true;
}

bool TransportLayerSecurityServer::ReadBytesDecodeOnce(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("TransportLayerSecurityServer::ReadBytesDecodeOnce");
  if (!this->ReadBytesOnce(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read bytes. ";
    }
    return false;
  }
  this->lastReaD.incomingBytes = this->incomingBytes;
  if (!this->DecodeSSLRecord(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to decode ssl record. ";
    }
    return false;
  }
  return true;
}

bool SSLContent::CheckInitialization() const {
  if (this->owner == nullptr) {
    crash << "Uninitialized ssl content. " << crash;
  }
  this->owner->CheckInitialization();
  return true;
}

std::string CipherSuiteSpecification::ToString() const {
  std::stringstream out;
  std::string hexVersion;
  Crypto::ConvertLargeUnsignedIntToHexSignificantDigitsFirst(
    LargeIntegerUnsigned(static_cast<unsigned>(this->id)), 0, hexVersion
  );
  out << hexVersion << "[" << this->name << "]";
  return out.str();
}

// https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art061
void SSLContent::PrepareServerHello2Certificate() {
  MacroRegisterFunctionWithName("SSLContent::PrepareServerHello2Certificate");
  this->CheckInitialization();
  this->version = 3 * 256 + 3;
  this->theType = SSLContent::tokens::certificate;
}

bool TransportLayerSecurityServer::Session::ComputeAndSignEphemerealKey(std::stringstream* commentOnError) {
  MacroRegisterFunctionWithName("TransportLayerSecurityServer::Session::ComputeAndSignEphemerealKey");
  Crypto::GetRandomLargeIntegerSecure(this->ephemerealPrivateKey, 32);
  this->chosenEllipticCurve = CipherSuiteSpecification::EllipticCurveSpecification::secp256k1;
  this->chosenEllipticCurveName = "secp256k1";
  this->bytesToSign.SetSize(0);
  this->bytesToSign.AddListOnTop(this->incomingRandomBytes);
  this->bytesToSign.AddListOnTop(this->myRandomBytes);
  //
  this->WriteNamedCurveAndPublicKey(this->bytesToSign, nullptr);
  this->owner->privateKey.SignBytes(this->bytesToSign, this->signature);
  return true;
}

// https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art060
bool SSLContent::PrepareServerHello3ServerKeyExchange(std::stringstream* commentsOnError) {
  MacroRegisterFunctionWithName("SSLContent::PrepareServerHello3SecretNegotiation");
  this->theType = SSLContent::tokens::serverKeyExchange;
  if (this->owner->owner->session.ephemerealPrivateKey != 0) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Private key already generated for this session. "
      << "No further modifications allowed in this session. ";
    }
    return false;
  }
  if (!this->owner->owner->session.ComputeAndSignEphemerealKey(commentsOnError)) {
    return false;
  }
  // this->owner->owner->session.ephemerealPublicKey.
  return true;
}

bool TransportLayerSecurityServer::Session::ChooseCipher(std::stringstream* commentsOnFailure) {
  (void) commentsOnFailure;
  MapList<int, CipherSuiteSpecification, MathRoutines::IntUnsignIdentity>& suites =
  this->owner->supportedCiphers;
  int bestIndex = suites.size();
  for (int i = 0; i < this->incomingCiphers.size; i ++) {
    CipherSuiteSpecification& cipher = this->incomingCiphers[i];
    if (cipher.id == 0) {
      continue;
    }
    int candidateIndex = suites.GetIndex(cipher.id);
    if (candidateIndex < 0 || candidateIndex > bestIndex) {
      continue;
    }
    bestIndex = candidateIndex;
  }
  this->chosenCipher = - 1;
  if (bestIndex < suites.size()) {
    this->chosenCipher = suites.theKeys[bestIndex];
  }
  return true;
}

void SSLContent::PrepareServerHello1Start(SSLContent& clientHello) {
  MacroRegisterFunctionWithName("SSLContent::PrepareServerHello1Start");
  this->CheckInitialization();
  this->version = 3 * 256 + 3;
  this->theType = SSLContent::tokens::serverHello;
  this->compressionMethod = clientHello.compressionMethod;
  Crypto::computeSha256(
    this->owner->owner->session.myRandomBytes, this->sessionId
  );
  this->extensions.SetSize(0);
  SSLHelloExtension newExtension;
  newExtension.MakeGrease(this);
  this->extensions.AddOnTop(newExtension);
  newExtension.MakeExtendedMasterSecret(this);
  this->extensions.AddOnTop(newExtension);
  newExtension.MakeEllipticCurvePointFormat(this);
  this->extensions.AddOnTop(newExtension);

}

void SSLRecord::resetExceptOwner() {
  this->content.resetExceptOwner();
  this->incomingBytes.SetSize(0);
}

void SSLRecord::PrepareServerHello1Start(
  SSLRecord& clientHello
) {
  this->resetExceptOwner();
  this->theType = SSLRecord::tokens::handshake;
  this->version = 3 * 256 + 1;
  this->content.PrepareServerHello1Start(clientHello.content);
  this->owner->outgoingRecords.AddOnTop(*this);
}

void SSLRecord::PrepareServerHello2Certificate() {
  this->resetExceptOwner();
  this->theType = SSLRecord::tokens::handshake;
  this->version = 3 * 256 + 1;
  this->content.PrepareServerHello2Certificate();
  this->owner->outgoingRecords.AddOnTop(*this);
}

bool SSLRecord::PrepareServerHello3SecretExchange(std::stringstream* commentsOnFailure) {
  this->resetExceptOwner();
  this->theType = SSLRecord::tokens::handshake;
  this->version = 3 * 256 + 1;
  bool success = this->content.PrepareServerHello3ServerKeyExchange(commentsOnFailure);
  this->owner->outgoingRecords.AddOnTop(*this);
  return success;
}

bool TransportLayerSecurityServer::ReplyToClientHello(int inputSocketID, std::stringstream* commentsOnFailure) {
  (void) commentsOnFailure;
  (void) inputSocketID;
  this->outgoingRecords.SetSize(0);
  this->serverHelloStart.PrepareServerHello1Start(this->lastReaD);
  this->serverHelloCertificate.PrepareServerHello2Certificate();
  if (!this->serverHelloKeyExchange.PrepareServerHello3SecretExchange(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to prepare server secret exchange message. ";
    }
    return false;
  }
  this->outgoingBytes.SetSize(0);
  if (!this->WriteSSLRecords(this->outgoingRecords, commentsOnFailure)) {
    logWorker << "Error replying to client hello. ";
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure  << "Error replying to client hello. ";
    }
    return false;
  }
  logWorker << "Wrote ssl records successfully. " << logger::endL;
  return true;
}

TransportLayerSecurityServer::Session::Session() {
  this->socketId = - 1;
  this->owner = nullptr;
  this->chosenCipher = 0;
  this->flagRequestOnlineCertificateStatusProtocol = false;
  this->flagRequestSignedCertificateTimestamp = false;
}

bool TransportLayerSecurityServer::Session::SetIncomingRandomBytes(
  List<unsigned char>& input, std::stringstream* commentsOnFailure
) {
  if (this->incomingRandomBytes.size != 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Session random bytes already set. ";
    }
    return false;
  }
  this->incomingRandomBytes = input;
  return true;
}

void TransportLayerSecurityServer::Session::initialize() {
  this->socketId = - 1;
  this->incomingRandomBytes.SetSize(0);
  this->incomingAlgorithmSpecifications.SetSize(0);
  this->chosenCipher = 0;
  this->chosenSignatureAlgorithm = 0;
  this->chosenEllipticCurve = 0;
  this->chosenEllipticCurveName = "";
  Crypto::GetRandomBytesSecureOutputMayLeaveTraceInFreedMemory(
    this->myRandomBytes, SSLContent::LengthRandomBytesInSSLHello
  );
  this->ephemerealPrivateKey = 0;
  this->serverName.SetSize(0);
  this->bytesToSign.SetSize(0);
  this->signature.SetSize(0);
  this->incomingCiphers.SetSize(0);
}

bool TransportLayerSecurityServer::HandShakeIamServer(
  int inputSocketID, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("TransportLayerSecurityServer::HandShakeIamServer");
  this->session.initialize();
  this->session.socketId = inputSocketID;
  bool success = this->ReadBytesDecodeOnce(commentsOnFailure);
  if (this->spoofer.flagDoSpoof && this->spoofer.currentInputMessageIndex > 0) {
    this->spoofer.incomingMessages[
      this->spoofer.currentInputMessageIndex - 1
    ] = this->lastReaD;
  }
  if (!success) {
    logWorker << logger::red << commentsOnFailure->str() << logger::endL;
    if (this->spoofer.flagDoSpoof) {
      this->spoofer.errorsOnIncoming.AddOnTop(commentsOnFailure->str());
    }
    return false;
  }
  success = TransportLayerSecurityServer::ReplyToClientHello(inputSocketID, commentsOnFailure);
  if (!success) {
    logWorker << logger::red << commentsOnFailure->str() << logger::endL;
    if (this->spoofer.flagDoSpoof) {
      this->spoofer.errorsOnOutgoing.AddOnTop(commentsOnFailure->str());
    }
    return false;
  }
  //crash << "Handshake i am server not implemented yet. " << crash;
  return false;
}

bool TransportLayerSecurity::HandShakeIamServer(int inputSocketID, std::stringstream* commentsOnFailure) {
  if (!this->flagUseBuiltInTlS) {
    return this->openSSLData.HandShakeIamServer(inputSocketID, commentsOnFailure);
  }
  return this->theServer.HandShakeIamServer(inputSocketID, commentsOnFailure);
}

bool TransportLayerSecurity::HandShakeIamClientNoSocketCleanup(
  int inputSocketID,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  MacroRegisterFunctionWithName("WebServer::HandShakeIamClientNoSocketCleanup");
  this->openSSLData.CheckCanInitializeToClient();
  this->initializeNonThreadSafeOnFirstCall(false);
  this->openSSLData.CheckCanInitializeToClient();
  return this->openSSLData.HandShakeIamClientNoSocketCleanup(inputSocketID, commentsOnFailure, commentsGeneral);
}

bool TransportLayerSecurityOpenSSL::InspectCertificates(
  std::stringstream *commentsOnFailure, std::stringstream *commentsGeneral
) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::InspectCertificates");
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
}

int TransportLayerSecurityOpenSSL::SSLRead(
  List<char>& readBuffer,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  bool includeNoErrorInComments
) {
  ERR_clear_error();
  int result = SSL_read(this->sslData, readBuffer.TheObjects, readBuffer.size);
  this->ClearErrorQueue(
    result, outputError, commentsGeneral, includeNoErrorInComments
  );
  return result;

}

int TransportLayerSecurity::SSLRead(
  std::string* outputError,
  std::stringstream* commentsGeneral,
  bool includeNoErrorInComments
) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::SSLRead");
  this->readBuffer.SetSize(this->readBufferStandardSize);
  if (!this->flagUseBuiltInTlS) {
    return this->openSSLData.SSLRead(readBuffer, outputError, commentsGeneral, includeNoErrorInComments);
  } else {
    crash << "SSL read not implemented yet. " << crash;
    return - 1;
  }
}

int TransportLayerSecurityOpenSSL::SSLWrite(
  const List<char>& writeBuffer,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  std::stringstream* commentsOnError,
  bool includeNoErrorInComments
) {
  (void) commentsGeneral;
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
}

int TransportLayerSecurity::SSLWrite(
  List<char>& writeBuffer,
  std::string* outputError,
  std::stringstream* commentsGeneral,
  std::stringstream* commentsOnError,
  bool includeNoErrorInComments
) {
  if (!this->flagUseBuiltInTlS) {
    return this->openSSLData.SSLWrite(
      writeBuffer,
      outputError,
      commentsGeneral,
      commentsOnError,
      includeNoErrorInComments
    );
  } else {
    crash << "Not implemented yet. " << crash;
    return - 1;
  }
}

bool TransportLayerSecurityOpenSSL::HandShakeIamServer(int inputSocketID, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("WebServer::HandShakeIamServer");
  if (this->sslData != nullptr) {
    crash << "SSL data expected to be zero. " << crash;
  }
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

void TransportLayerSecurity::AddMoreEntropyFromTimer() {
  int fixMeAcquireEntropy;
}
