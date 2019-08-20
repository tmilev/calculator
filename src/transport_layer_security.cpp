//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".

#include "transport_layer_security.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"
#include "vpfHeader1General7FileOperations_Encodings.h"
#include "vpfHeader5Crypto.h"
#include "abstract_syntax_notation_one_decoder.h"

#include <unistd.h> //<- close, open defined here
#include <netdb.h> //<-addrinfo and related data structures defined here


ProjectInformationInstance projectInfoInstanceTransportLayerSecurityImplementation(__FILE__, "TSL/ssl implementation.");

//http://stackoverflow.com/questions/10175812/how-to-create-a-self-signed-certificate-with-openssl
//openssl req -x509 -keyalg RSA -newkey rsa:2048 -nodes -keyout key.pem -out cert.pem -days 3001
//Alternatively:
//certificate with certificate signing request:
//openssl req -out CSR.csr -new -newkey rsa:2048 -nodes -keyout calculator-algebra.key
//then get the CSR.csr file to a signing authority,
//from where you get the signedFileCertificate1 and signedFileCertificate3
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
}

TransportLayerSecurity::~TransportLayerSecurity() {
}

bool TransportLayerSecurity::flagDontUseOpenSSL = false;

void TransportLayerSecurity::initialize(bool IamServer) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::initialize");
  if (this->flagInitialized) {
    return;
  }
  if (!this->flagDontUseOpenSSL) {
    if (IamServer) {
      this->openSSLData.initSSLServer();
    } else {
      this->openSSLData.initSSLClient();
    }
  } else {
    logServer << logger::red << "DEBUG: TODO: carry out any TLS initializations as needed. " << logger::endL;
  }
  this->flagInitialized = true;
}

void TransportLayerSecurity::FreeEverythingShutdown() {
  MacroRegisterFunctionWithName("TransportLayerSecurity::FreeEverythingShutdown");
  if (!this->flagDontUseOpenSSL) {
    return this->openSSLData.FreeEverythingShutdownSSL();
  } else {
    crash << "Free everything not implemented yet. " << crash;
  }
}

bool TransportLayerSecurity::initSSLKeyFiles() {
  MacroRegisterFunctionWithName("TransportLayerSecurity::initSSLKeyFiles");
  if (!this->flagDontUseOpenSSL) {
    return this->openSSLData.initSSLKeyFiles();
  } else {
    std::stringstream commentsOnFailure;
    this->initSSLKeyFilesInternal(&commentsOnFailure);
    return true;
  }
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
    TransportLayerSecurity::fileCertificate, certificatePhysicalName, true, true, 0
  );
  FileOperations::GetPhysicalFileNameFromVirtual(
    TransportLayerSecurity::fileKey, keyPhysicalName, true, true, 0
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
  } else {
    return true;
  }
  return false;
}

void TransportLayerSecurity::AddMoreEntropyFromTimer() {
  int fixMeAcquireEntropy;
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

  if (this->context == 0) {
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
    TransportLayerSecurity::signedFileCertificate1, singedFileCertificate1Physical, true, true, 0
  );
  FileOperations::GetPhysicalFileNameFromVirtual(
    TransportLayerSecurity::signedFileCertificate3, signedFileCertificate3Physical, true, true, 0
  );
  FileOperations::GetPhysicalFileNameFromVirtual(
    TransportLayerSecurity::fileCertificate, fileCertificatePhysical, true, true, 0
  );
  FileOperations::GetPhysicalFileNameFromVirtual(
    TransportLayerSecurity::fileKey, fileKeyPhysical, true, true, 0
  );
  FileOperations::GetPhysicalFileNameFromVirtual(
    TransportLayerSecurity::signedFileKey, signedFileKeyPhysical, true, true, 0
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
    logServer << "Private key does not match the certificate public key.";
    crash << "Private key does not match the certificate public key." << crash;
  }
}

bool TransportLayerSecurity::SSLReadLoop(int numTries,
  std::string& output,
  const LargeInt& expectedLength,
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
      next.assign(this->readBuffer.TheObjects, numBytes);
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
    if (commentsGeneral != 0) {
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
      *outputError = TransportLayerSecurityOpenSSL::errors::errorWantRead;
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
  if (!this->flagDontUseOpenSSL) {
    this->openSSLData.FreeSSL();
  } else {
    logWorker << logger::red << "DEBUG: free doesn't do anything yet, please fix as necesary as the code matures. " << logger::endL;
  }
  this->flagInitialized = false;
}

void TransportLayerSecurity::RemoveLastSocket() {
  if (!this->flagDontUseOpenSSL) {
    this->openSSLData.RemoveLastSocket();
  } else {
    crash << "Remove last socket not implemented yet. " << crash;
  }
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
  if (this->sslData == 0) {
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
        if (commentsOnFailure != 0) {
          *commentsOnFailure << "Attempt " << i + 1
          << ": SSL handshake not successful in a "
          << "controlled fashion (errorCode = 0). <br>";
        }
      } else {
        if (commentsOnFailure != 0) {
          *commentsOnFailure << "Attempt " << i + 1
          << ": SSL handshake not successful with a fatal error with "
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
      *commentsGeneral << "<span style =\"color:green\">SSL handshake successful.</span>\n<br>\n";
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

TransportLayerSecurityServer::TransportLayerSecurityServer() {
  this->socketId = - 1;
  this->millisecondsDefaultTimeOut = 5000;
  this->millisecondsTimeOut = this->millisecondsDefaultTimeOut;
  this->defaultBufferCapacity = 1000000;
}

bool TransportLayerSecurityServer::ReadBytesOnce() {
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 5;  // 5 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  setsockopt(this->socketId, SOL_SOCKET, SO_RCVTIMEO, (void*)(&tv), sizeof(timeval));
  this->lastRead.SetSize(this->defaultBufferCapacity);
  int numBytesInBuffer = recv(this->socketId, this->lastRead.TheObjects, this->lastRead.size - 1, 0);
  if (numBytesInBuffer >= 0) {
    this->lastRead.SetSize(numBytesInBuffer);
  }
  return numBytesInBuffer > 0;
}

SSLHello::SSLHello() {
  this->handshakeType = 0;
  this->version = 0;
}

SSLRecord::SSLRecord() {
  this->theType = SSLRecord::tokens::unknown;
  this->length = 0;
  this->version = 0;
}

std::string SSLRecord::ToStringType() const{
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
  out << "malformed_type_" << (int) this->theType;
  return out.str();
}

std::string SSLRecord::ToString() const {
  JSData result;
  result["length"] = this->length;
  result["body"] = Crypto::ConvertListCharsToHex(this->body, 50, false);
  result["type"] = this->ToStringType();
  std::string hexVersion;
  Crypto::ConvertLargeUnsignedIntToHexSignificantDigitsFirst(LargeIntUnsigned((unsigned) this->version), 0, hexVersion);
  result["version"] = hexVersion;
  return result.ToString(false, true, false, true);
}

bool SSLRecord::Decode(List<char>& input, int offset, std::stringstream *commentsOnFailure) {
  this->body.SetSize(0);
  if (input.size < 5 + offset) {
    if (commentsOnFailure != 0) {
      *commentsOnFailure << "SSL record needs to have at least 5 bytes, yours has: " << input.size << ".";
    }
    return false;
  }
  this->theType = input[offset];
  if (
    this->theType != SSLRecord::tokens::handshake
  //&&
//    this->theType != SSLRecord::tokens::alert &&
//    this->theType != SSLRecord::tokens::applicationData &&
//    this->theType != SSLRecord::tokens::changeCipherSpec
  ) {
    this->theType = SSLRecord::tokens::unknown;
    if (commentsOnFailure != 0) {
      *commentsOnFailure << "Unknown record type: " << (int) input[offset] << ". ";
    }
    return false;
  }
  logWorker << "DEBUG: got to length extraction. " << logger::endL;
  this->length = input[offset + 3] * 256 + input[offset + 4];
  this->version = input[offset + 1] * 256 + input[offset + 2];
  int highEnd = this->length + offset + 5;
  if (highEnd > input.size) {
    logWorker << "DEBUG: high end's too big. " << logger::endL;
    if (commentsOnFailure != 0) {
      *commentsOnFailure << "Insufficent record length: expected: 5 + " << this->length << ", got: " << input.size - offset;
    }
    return false;
  }
  this->body.SetSize(this->length);
  for (int i = 0; i < this->length; i ++) {
    this->body[i] = input[offset + i];
  }
  logWorker << "DEBUG: Got to return true ... " << logger::endL;
  return true;
}

bool TransportLayerSecurityServer::DecodeSSLRecord(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("TransportLayerSecurityServer::DecodeSSLRecord");
  SSLRecord oneRecord;
  if (!oneRecord.Decode(this->lastRead, 0, commentsOnFailure)) {
    logWorker << "DEBUG: one record decode error.\n" << oneRecord.ToString() << logger::endL;
    return false;
  }
  logWorker << "DEBUG: Decoded: " << oneRecord.ToString() << logger::endL;
  crash << "Decode not implemented yet. " << crash;

  return false;
}

bool TransportLayerSecurityServer::ReadBytesDecodeOnce(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("TransportLayerSecurityServer::ReadBytesDecodeOnce");
  if (!this->ReadBytesOnce()) {
    if (commentsOnFailure != 0) {
      *commentsOnFailure << "Failed to read bytes. ";
    }
    return false;
  }
  logWorker << "DEBUG: got to before decode ssl record. " << logger::endL;
  if (!this->DecodeSSLRecord(commentsOnFailure)) {
    if (commentsOnFailure != 0) {
      *commentsOnFailure << "Failed to decode ssl record. ";
    }
    return false;
  }
  crash << "read bytes decode Not implemented yet" << crash;
  return false;
}

bool TransportLayerSecurityServer::HandShakeIamServer(int inputSocketID, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("TransportLayerSecurityServer::HandShakeIamServer");
  logWorker << logger::red << "DEBUG: handshaking ...\n" << logger::endL;
  this->socketId = inputSocketID;
  bool success = this->ReadBytesDecodeOnce(commentsOnFailure);
  if (!success) {
    logWorker << logger::red << "DEBUG: commentson failure: \n" << commentsOnFailure->str() << logger::endL;

  }
  logWorker << logger::red << "DEBUG: after read bytes decode once. \n" << logger::endL;

  crash.CleanUpFunction = 0;
  crash << "Not implemented yet. " << crash;
  return false;
}

bool TransportLayerSecurity::HandShakeIamServer(int inputSocketID, std::stringstream* commentsOnFailure) {
  if (!this->flagDontUseOpenSSL) {
    logWorker << "DEBUG: AM using open ssl. " << logger::endL;
    return this->openSSLData.HandShakeIamServer(inputSocketID);
  } else {
    logWorker << "DEBUG: not using open ssl. " << logger::endL;

    return this->theServer.HandShakeIamServer(inputSocketID, commentsOnFailure);
  }
}

bool TransportLayerSecurity::HandShakeIamClientNoSocketCleanup(
  int inputSocketID,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  MacroRegisterFunctionWithName("WebServer::HandShakeIamClientNoSocketCleanup");
  this->openSSLData.CheckCanInitializeToClient();
  this->openSSLData.CheckCanInitializeToClient();
  this->initialize(false);
  if (!this->flagDontUseOpenSSL) {
    this->openSSLData.CheckCanInitializeToClient();
    return this->openSSLData.HandShakeIamClientNoSocketCleanup(inputSocketID, commentsOnFailure, commentsGeneral);
  } else {
    crash << "Handshake not implemented yet. " << crash;
    return false;
  }
}

bool TransportLayerSecurityOpenSSL::InspectCertificates(
  std::stringstream *commentsOnFailure, std::stringstream *commentsGeneral
) {
  MacroRegisterFunctionWithName("TransportLayerSecurity::InspectCertificates");
  this->peer_certificate = SSL_get_peer_certificate(this->sslData);
  if (this->peer_certificate != 0) {
    char* tempCharPtr = 0;
    if (commentsGeneral != 0) {
      *commentsGeneral << "SSL connection using: "
      << SSL_get_cipher(this->sslData) << ".<br>\n";
    }
    tempCharPtr = X509_NAME_oneline(X509_get_subject_name(this->peer_certificate), 0, 0);
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

int TransportLayerSecurityOpenSSL::SSLRead(
  List<char>& readBuffer, std::string *outputError, std::stringstream *commentsGeneral, bool includeNoErrorInComments
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
  if (!this->flagDontUseOpenSSL) {
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
  if (this->sslData == 0) {
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
  if (!this->flagDontUseOpenSSL) {
    return this->openSSLData.SSLWrite(writeBuffer, outputError, commentsGeneral, commentsOnError, includeNoErrorInComments);
  } else {
    crash << "Not imeplemented yet. " << crash;
    return - 1;
  }
}

bool TransportLayerSecurityOpenSSL::HandShakeIamServer(int inputSocketID) {
  MacroRegisterFunctionWithName("WebServer::HandShakeIamServer");
  std::stringstream commentsOnSSLNew;
  if (this->sslData != 0) {
    crash << "SSL data expected to be zero. " << crash;
  }
  this->sslData = SSL_new(this->context);
  SSL_set_verify(this->sslData, SSL_VERIFY_NONE, 0);
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
    if (this->errorCode <= 0) {

      this->flagSSLHandshakeSuccessful = false;
      if (this->errorCode == 0) {
        logOpenSSL << "OpenSSL handshake not successful in a controlled manner. ";
      } else {
        logOpenSSL << "OpenSSL handshake not successful with a fatal error. ";
      }
      logOpenSSL << commentsOnError.str() << logger::endL;
      logOpenSSL << "Attempt " << i + 1 << " out of " << maxNumHandshakeTries << " failed. Details (not logged in a file): ";
      ERR_print_errors_fp(stderr);

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

TransportLayerSecurityOpenSSL::TransportLayerSecurityOpenSSL() {
  this->peer_certificate = 0;
  this->sslData = 0;
  this->errorCode = - 1;
  this->theSSLMethod = 0;
  this->context = 0;
  this->owner = 0;
  this->flagSSLHandshakeSuccessful = false;
  this->flagContextInitialized = false;
  this->flagIsServer = true;
}
