//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "webserver.h"
#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader7DatabaseInterface.h"
#include "vpfHeader8HtmlInterpretationInterface.h"
#include "vpfHeader8HtmlSnippets.h"
#include "vpfWebAPI.h"

std::string WebAPI::appNoCache = "appNoCache";
std::string WebAPI::app = "app";
std::string WebAPI::request::calculatorHTML = "/calculator-html";
std::string WebAPI::request::onePageJS = "/calculator-html/javascript_all_in_one.js";
std::string WebAPI::request::onePageJSWithHash = "/calculator-html/javascript_all_in_one.js";
std::string WebAPI::request::calculatorPage = "calculator";
std::string WebAPI::request::setProblemData = "setProblemData";
std::string WebAPI::request::changePassword = "changePassword";
std::string WebAPI::request::activateAccount = "activateAccount";
std::string WebAPI::request::activateAccountJSON = "activateAccountJSON";
std::string WebAPI::request::serverStatusJSON = "serverStatusJSON";
std::string WebAPI::request::selectCourseJSON = "selectCourseJSON";
std::string WebAPI::HeaderCacheControl = "Cache-Control: max-age=129600000, public";

std::string WebAPI::problemContent = "problemContent";
std::string WebAPI::problemFileName = "fileName";
std::string WebAPI::problemId = "id";
std::string WebAPI::commentsProblem = "commentsProblem";
std::string WebAPI::commentsServer = "commentsServer";

std::string WebAPI::request::userInfoJSON = "userInfoJSON";
std::string WebAPI::request::examplesJSON = "calculatorExamplesJSON";
std::string WebAPI::request::editPage = "editPageJSON";
std::string WebAPI::databaseParameters::entryPoint = "database";
std::string WebAPI::databaseParameters::labels = "databaseLabels";
std::string WebAPI::databaseParameters::operation = "databaseOperation";
std::string WebAPI::databaseParameters::fetch = "databaseFetch";

std::string WebAPI::problemSingleDeadline = "deadline";

ProjectInformationInstance projectInfoInstanceWebServer(__FILE__, "Web server implementation.");
WebServer theWebServer;

#include <sys/wait.h>//<-waitpid f-n here
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <arpa/inet.h> // <- inet_ntop declared here (ntop = network to presentation)
#include <unistd.h>
#include <sys/stat.h>//<-for file statistics
#include <fcntl.h>//<-setting flags of file descriptors
#include "vpfHeader7DatabaseInterface_Mongodb.h"

struct sigaction SignalSEGV, SignalFPE, SignalChild, SignalINT;
//sigset_t SignalSetToBlockWhileHandlingSIGCHLD;

bool WebWorker::CheckConsistency()
{ auto oldOutputFn = stOutput.theOutputFunction;
  stOutput.theOutputFunction = 0;
  if (this->flagDeallocated)
    crash << "Use after free of webworker." << crash;
  if (this->parent == 0)
    crash << "Parent of web worker is not initialized." << crash;
  if (this->indexInParent == - 1)
    crash << "Index in parent is bad. " << crash;
  if (this->connectionID == - 1)
    crash << "Connection id is bad. " << crash;
  stOutput.theOutputFunction = oldOutputFn;
  return true;
}

bool WebWorker::ReceiveAll()
{ MacroRegisterFunctionWithName("WebWorker::ReceiveAll");
  if (this->connectedSocketID == - 1)
    crash << "Attempting to receive on a socket with ID equal to - 1. " << crash;
  if (theGlobalVariables.flagUsingSSLinCurrentConnection)
    return this->ReceiveAllHttpSSL();
  return this->ReceiveAllHttp();
}

std::string WebWorker::openIndentTag(const std::string& theTag)
{ std::stringstream out;
  for (int i = 0; i < this->indentationLevelHTML; i ++)
    out << "  ";
  out << theTag << "\n";
  this->indentationLevelHTML ++;
  return out.str();
}

std::string WebWorker::closeIndentTag(const std::string& theTag)
{ std::stringstream out;
  this->indentationLevelHTML --;
  for (int i = 0; i < this->indentationLevelHTML; i ++)
    out << "  ";
  out << theTag << "\n";
  return out.str();
}

bool WebWorker::IsAllowedAsRequestCookie(const std::string& input)
{ return
  input != "login" && input != "logout" && input != WebAPI::request::changePassword &&
  input != "changePasswordPage" && input != WebAPI::request::activateAccount &&
  input != WebAPI::request::activateAccountJSON;
}

#ifdef MACRO_use_open_ssl
void SSL_write_Wrapper(SSL* inputSSL, const std::string& theString)
{ MacroRegisterFunctionWithName("SSL_write_Wrapper");
  int err = SSL_write (inputSSL, theString.c_str(), theString.size());
  if ((err) == - 1)
  { ERR_print_errors_fp(stderr);
    crash << "Errors while calling SSL_write. " << crash;
  }
}

#endif // MACRO_use_open_ssl
//http://stackoverflow.com/questions/10175812/how-to-create-a-self-signed-certificate-with-openssl
//openssl req -x509 -newkey rsa:2048 -nodes -keyout key.pem -out cert.pem -days 3001
//Alternatively:
//certificate with certificate signing request:
//openssl req -out CSR.csr -new -newkey rsa:2048 -nodes -keyout calculator-algebra.key
//then get the CSR.csr file to a signing authority,
//from where you get the signedFileCertificate1 and signedFileCertificate3
const std::string fileCertificate = "certificates/cert.pem";
const std::string fileCertificateConfiguration = "certificates/certificate_configuration.cnf";
const std::string fileKey = "certificates/key.pem";
const std::string signedFileCertificate1 = "certificates/calculator-algebra.crt";
const std::string signedFileCertificate3 = "certificates/godaddy-signature.crt";
const std::string signedFileKey = "certificates/calculator-algebra.key";

void WebServer::initSSL()
{ MacroRegisterFunctionWithName("WebServer::initSSL");
  if (!theGlobalVariables.flagSSLisAvailable)
  { logWorker << logger::red << "SSL is DISABLED." << logger::endL;
    return;
  }
#ifdef MACRO_use_open_ssl
  //////////////////////////////////////////////////////////////////////////
  this->theSSLdata.initSSLserver();

#endif // MACRO_use_open_ssl
}

#ifdef MACRO_use_open_ssl
SSLdata::SSLdata()
{ this->errorCode = - 1;
  this->sslClient = 0;
  this->sslServeR = 0;
  this->my_certificate = 0;
  this->peer_certificate = 0;
  this->theSSLMethod = 0;
  this->contextServer = 0;
  //this->contextClient = 0;
  this->flagSSLHandshakeSuccessful = false;
}

SSLdata::~SSLdata()
{ this->FreeSSL();
  this->FreeContext();
}

void SSLdata::FreeSSL()
{ SSL_free(this->sslClient);
  SSL_free(this->sslServeR);
  this->sslClient = 0;
  this->sslServeR = 0;
}

void SSLdata::FreeClientSSL()
{ SSL_free(this->sslClient);
  this->sslClient = 0;
}

void SSLdata::FreeEverythingShutdownSSL()
{ if (!theGlobalVariables.flagSSLisAvailable)
    return;
  theGlobalVariables.flagSSLisAvailable = false;
  this->FreeSSL();
  this->FreeContext();
}

void SSLdata::FreeContext()
{
#ifdef MACRO_use_open_ssl
  //SSL_CTX_free (this->contextClient);
  SSL_CTX_free (this->contextServer);
  //this->contextClient = 0;
  this->contextServer = 0;
#endif // MACRO_use_open_ssl
}

void WebServer::SSLServerSideHandShake()
{ if (!theGlobalVariables.flagSSLisAvailable)
    return;
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    return;
//  logOpenSSL << "Initiating SSL handshake. " << logger::endL;
#ifdef MACRO_use_open_ssl
  this->theSSLdata.HandShakeIamServer(this->GetActiveWorker().connectedSocketID);
#endif
}

#ifdef MACRO_use_open_ssl
bool SSLdata::flagSSLlibraryInitialized = false;

bool SSLdata::initSSLkeyFiles()
{
#ifdef MACRO_use_open_ssl
  if (!FileOperations::FileExistsVirtual(fileCertificate, true, true) ||
      !FileOperations::FileExistsVirtual(fileKey, true, true))
  { logWorker << logger::red << "SSL is available but CERTIFICATE files are missing." << logger::endL;
    logWorker << logger::green << "Let me try to create those files for you." << logger::endL;
    std::stringstream theCommand;
    std::string certificatePhysicalName, keyPhysicalName, certificateConfiguration;
    FileOperations::GetPhysicalFileNameFromVirtual(fileCertificate, certificatePhysicalName, true, true, 0);
    FileOperations::GetPhysicalFileNameFromVirtual(fileKey, keyPhysicalName, true, true, 0);
    FileOperations::GetPhysicalFileNameFromVirtual(fileCertificateConfiguration, certificateConfiguration, true, true, 0);
    theCommand <<  "openssl req -x509 -newkey rsa:2048 -nodes -keyout " << keyPhysicalName
    << " -out " << certificatePhysicalName
    << " -days 3001 ";
    if (theGlobalVariables.configuration["openSSLSubject"].type != JSData::JSUndefined &&
        theGlobalVariables.configuration["openSSLSubject"].type == JSData::JSstring)
      theCommand
      << "-subj " << theGlobalVariables.configuration["openSSLSubject"].string;
    theGlobalVariables.CallSystemNoOutput(theCommand.str(), false);
  }
  if (!FileOperations::FileExistsVirtual(fileCertificate, true, true) ||
      !FileOperations::FileExistsVirtual(fileKey, true, true))
  { theGlobalVariables.flagSSLisAvailable = false;
    return false;
  } else
    return true;
#endif
  return false;
}

void SSLdata::initSSLlibrary()
{ if (this->flagSSLlibraryInitialized)
    return;
  this->flagSSLlibraryInitialized = true;
  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();
}

void SSLdata::initSSLserver()
{ this->initSSLlibrary();
  if (this->theSSLMethod != 0)
    return;
  std::string fileCertificatePhysical, fileKeyPhysical,
  singedFileCertificate1Physical, signedFileCertificate3Physical,
  signedFileKeyPhysical;
  FileOperations::GetPhysicalFileNameFromVirtual(signedFileCertificate1, singedFileCertificate1Physical, true, true, 0);
  FileOperations::GetPhysicalFileNameFromVirtual(signedFileCertificate3, signedFileCertificate3Physical, true, true, 0);
  FileOperations::GetPhysicalFileNameFromVirtual(fileCertificate, fileCertificatePhysical, true, true, 0);
  FileOperations::GetPhysicalFileNameFromVirtual(fileKey, fileKeyPhysical, true, true, 0);
  FileOperations::GetPhysicalFileNameFromVirtual(signedFileKey, signedFileKeyPhysical, true, true, 0);
  //////////////////////////////////////////////////////////
  if (!this->initSSLkeyFiles())
    return;
  logServer << logger::green << "SSL is available." << logger::endL;
  this->theSSLMethod = SSLv23_method();
  this->contextServer = SSL_CTX_new(this->theSSLMethod);
  if (!this->contextServer)
  { ERR_print_errors_fp(stderr);
    crash << "openssl error: failed to create CTX object." << crash;
  }
  //////////Safari web browser: no further use of foul language necessary
  if (SSL_CTX_use_certificate_chain_file(this->contextServer, signedFileCertificate3Physical.c_str()) <= 0)
  { logServer << logger::purple << "Found no officially signed certificate, trying self-signed certificate. "
    << logger::endL;
    if (SSL_CTX_use_certificate_file(this->contextServer, fileCertificatePhysical.c_str(), SSL_FILETYPE_PEM) <= 0)
    { ERR_print_errors_fp(stderr);
      exit(3);
    }
    if (SSL_CTX_use_PrivateKey_file(this->contextServer, fileKeyPhysical.c_str(), SSL_FILETYPE_PEM) <= 0)
    { ERR_print_errors_fp(stderr);
      exit(4);
    }
  } else
  { logServer << logger::green << "Found officially signed certificate... " << logger::endL;
    //if (SSL_CTX_use_certificate_chain_file(theSSLdata.ctx, signedFileCertificate2.c_str()) <= 0)
    //{ ERR_print_errors_fp(stderr);
    //  exit(3);
    //}
    if (SSL_CTX_use_certificate_chain_file(this->contextServer, singedFileCertificate1Physical.c_str()) <= 0)
    { ERR_print_errors_fp(stderr);
      crash << "Failed to user certificate file." << crash;
    }
    if (SSL_CTX_use_PrivateKey_file(this->contextServer, signedFileKeyPhysical.c_str(), SSL_FILETYPE_PEM) <= 0)
    { ERR_print_errors_fp(stderr);
      crash << "Failed to use private key." << crash;
    }
    theGlobalVariables.flagCertificatesAreOfficiallySigned = true;
  }
  if (!SSL_CTX_check_private_key(this->contextServer))
  { logServer << "Private key does not match the certificate public key.";
    crash << "Private key does not match the certificate public key." << crash;
  }
  ////////Safari web browser: no further use of foul language necessary.
  //  SSL_CTX_set_options(theSSLdata.ctx, SSL_OP_SAFARI_ECDHE_ECDSA_BUG);
  ////////
}
#endif

//void SSLdata::initSSLclient()
//{ this->initSSLlibrary();
//  if (this->theSSLClientMethod!= 0)
//    return;
//  this->theSSLClientMethod = SSLv23_client_method();
//  this->contextClient = SSL_CTX_new(this->theSSLClientMethod);   /* Create new context */
//  if (this->contextClient == 0)
//  { ERR_print_errors_fp(stderr);
//    crash << "openssl error: failed to create CTX object." << crash;
//  }
//}

#ifdef MACRO_use_open_ssl
void SSLdata::HandShakeIamServer(int inputSocketID)
{
  MacroRegisterFunctionWithName("WebServer::HandShakeIamServer");
  this->sslServeR = SSL_new(this->contextServer);
  if (this->sslServeR == 0)
  { logOpenSSL << logger::red << "Failed to allocate ssl" << logger::endL;
    crash << "Failed to allocate ssl: not supposed to happen" << crash;
  }
  this->SetSocketAddToStackServer(inputSocketID);
  int maxNumHandshakeTries = 3;
  for (int i = 0; i < maxNumHandshakeTries; i ++)
  { this->errorCode = SSL_accept(this->sslServeR);
    this->flagSSLHandshakeSuccessful = false;
    if (this->errorCode != 1)
    { if (this->errorCode == 0)
        logOpenSSL << "OpenSSL handshake not successful in a controlled manner. ";
      else
        logOpenSSL << "OpenSSL handshake not successful with a fatal error. ";
      logOpenSSL << "Attempt " << i + 1 << " out of " << maxNumHandshakeTries << " failed. ";
      switch(SSL_get_error(this->sslServeR, this->errorCode))
      {
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
      logOpenSSL << "Retrying connection in 0.5 seconds...";
      theGlobalVariables.FallAsleep(500000);
    } else
    { this->flagSSLHandshakeSuccessful = true;
      break;
    }
  }
}
#endif // MACRO_use_open_ssl

void SSLdata::DoSetSocket(int theSocket, SSL *theSSL)
{ MacroRegisterFunctionWithName("SSLdata::DoSetSocket");
  int result = 0;
  for (int i = 0; i < 10; i ++)
  { result = SSL_set_fd(theSSL, theSocket);
    if (result != 0)
      break;
  }
  if (result == 0)
    crash << "Failed to set socket of server. " << crash;
}

void SSLdata::SetSocketAddToStackServer(int theSocket)
{ MacroRegisterFunctionWithName("SSLdata::SetSocketAddToStackServer");
  this->socketStackServer.AddOnTop(theSocket);
  this->DoSetSocket(theSocket, this->sslServeR);
}

void SSLdata::SetSocketAddToStackClient(int theSocket)
{ MacroRegisterFunctionWithName("SSLdata::SetSocketAddToStackClient");
  this->socketStackClient.AddOnTop(theSocket);
  this->DoSetSocket(theSocket, this->sslClient);
}

void SSLdata::RemoveLastSocketServer()
{ MacroRegisterFunctionWithName("SSLdata::RemoveLastSocketServer");
  if (this->socketStackServer.size > 0)
  { int lastSocket = this->socketStackServer.PopLastObject();
    close(lastSocket);
  }
  if (this->socketStackServer.size <= 0)
    return;
  this->DoSetSocket(*this->socketStackServer.LastObject(), this->sslServeR);
}

void SSLdata::RemoveLastSocketClient()
{ MacroRegisterFunctionWithName("SSLdata::RemoveLastSocketClient");
  if (this->socketStackClient.size > 0)
  { int lastSocket = this->socketStackClient.PopLastObject();
    close(lastSocket);
  }
  if (this->socketStackClient.size <= 0)
    return;
  this->DoSetSocket(*this->socketStackClient.LastObject(), this->sslClient);
}

bool SSLdata::HandShakeIamClientNoSocketCleanup
(int inputSocketID, std::stringstream* commentsOnFailure,
 std::stringstream* commentsGeneral)
{
#ifdef MACRO_use_open_ssl
  MacroRegisterFunctionWithName("WebServer::HandShakeIamClientNoSocketCleanup");
  this->FreeClientSSL();
  this->sslClient = SSL_new(this->contextServer);
  if (this->sslClient == 0)
  { logOpenSSL << logger::red << "Failed to allocate ssl" << logger::endL;
    crash << "Failed to allocate ssl: not supposed to happen" << crash;
  }
  this->SetSocketAddToStackClient(inputSocketID);
  int maxNumHandshakeTries = 4;
  for (int i = 0; i < maxNumHandshakeTries; i ++)
  { this->errorCode = SSL_connect(this->sslClient);
    this->flagSSLHandshakeSuccessful = false;
    if (this->errorCode != 1)
    { if (this->errorCode == 0)
      { if (commentsOnFailure != 0)
          *commentsOnFailure << "Attempt " << i + 1
          << ": SSL handshake not successfull in a "
          << "controlled fashion (errorCode = 0). <br>";
      } else
      { if (commentsOnFailure != 0)
          *commentsOnFailure << "Attempt " << i + 1
          << ": SSL handshake not successfull with a fatal error with "
          << "errorCode: " << this->errorCode << ". <br>";
      }
      switch(SSL_get_error(this->sslClient, this->errorCode))
      {
      case SSL_ERROR_NONE:
        if (commentsOnFailure != 0)
          *commentsOnFailure << "No error reported, this shouldn't happen. <br>";
        maxNumHandshakeTries = 1;
        break;
      case SSL_ERROR_ZERO_RETURN:
        if (commentsOnFailure != 0)
          *commentsOnFailure << "The TLS/SSL connection has been closed (possibly cleanly).  <br>";
        maxNumHandshakeTries = 1;
        break;
      case SSL_ERROR_WANT_READ:
      case SSL_ERROR_WANT_WRITE:
        if (commentsOnFailure != 0)
          *commentsOnFailure << " During regular I/O: repeat needed (not implemented).  <br>";
        break;
      case SSL_ERROR_WANT_CONNECT:
      case SSL_ERROR_WANT_ACCEPT:
        if (commentsOnFailure != 0)
          *commentsOnFailure << " During handshake negotiations: repeat needed (not implemented). <br> ";
        break;
      case SSL_ERROR_WANT_X509_LOOKUP:
        if (commentsOnFailure != 0)
          *commentsOnFailure << " Application callback set by SSL_CTX_set_client_cert_cb(): "
          << "repeat needed (not implemented).  <br>";
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
        if (commentsOnFailure != 0)
          *commentsOnFailure << "A failure in the SSL library occurred. <br>";
        //theError = ERR_get_error(3ssl);
        //if (theError!=SSL_ERROR_WANT_READ && theError!=SSL_ERROR_WANT_WRITE)
        //  maxNumHandshakeTries =1;
        break;
      default:
        if (commentsOnFailure != 0)
          *commentsOnFailure << "Unknown error. <br>";
        maxNumHandshakeTries = 1;
        break;
      }
      if (commentsOnFailure != 0)
        *commentsOnFailure << "Retrying connection in 0.5 seconds...<br>";
      theGlobalVariables.FallAsleep(500000);
    } else
    { this->flagSSLHandshakeSuccessful = true;
      break;
    }
  }
  if (this->flagSSLHandshakeSuccessful)
  { if (commentsGeneral != 0)
      *commentsGeneral << "<span style =\"color:green\">SSL handshake successfull.</span>\n<br>\n";
  } else
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "<span style =\"color:red\">SSL handshake failed.</span>\n<br>\n";
    return false;
  }
  //CHK_SSL(err);
  /* Get the cipher - opt */
  /* Get client's certificate (note: beware of dynamic allocation) - opt */
  if (false)
    this->InspectCertificates(commentsOnFailure, commentsGeneral);

#endif // MACRO_use_open_ssl
  return true;
}

bool SSLdata::InspectCertificates
(std::stringstream *commentsOnFailure, std::stringstream *commentsGeneral)
{ MacroRegisterFunctionWithName("SSLdata::InspectCertificates");
  this->peer_certificate = SSL_get_peer_certificate(this->sslClient);
  if (this->peer_certificate != 0)
  { char* tempCharPtr = 0;
    if (commentsGeneral != 0)
      *commentsGeneral << "SSL connection using: "
      << SSL_get_cipher(this->sslClient) << ".<br>\n";
    tempCharPtr = X509_NAME_oneline(X509_get_subject_name (this->peer_certificate), 0, 0);
    if (tempCharPtr == 0)
    { if (commentsOnFailure != 0)
        *commentsOnFailure << "X509_NAME_oneline return null; this is not supposed to happen. <br>\n";
      return false;
    }
    this->otherCertificateSubjectName = tempCharPtr;
    OPENSSL_free(tempCharPtr);
    if (commentsGeneral != 0)
      *commentsGeneral << "Peer certificate: "
      << "subject: " << this->otherCertificateSubjectName << "<br>\n";

    tempCharPtr = X509_NAME_oneline(X509_get_issuer_name(this->peer_certificate), 0, 0);
    if (tempCharPtr == 0)
    { if (commentsOnFailure != 0)
        *commentsOnFailure << "X509_NAME_oneline return null; this is not supposed to happen. <br>\n";
      return false;
    }
    this->otherCertificateIssuerName = tempCharPtr;
    OPENSSL_free(tempCharPtr);
    if (commentsGeneral != 0)
      *commentsGeneral << "Issuer name: "
      << this->otherCertificateIssuerName << "<br>\n";
    X509_free(this->peer_certificate);
    this->peer_certificate = 0;
    return true;
  } else
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "SSL connection using: "
      << SSL_get_cipher(this->sslClient) << ". "
      << "No client certificate.<br>\n";
    return false;
  }
}

int SSLdata::SSLRead
(SSL* theSSL, void* buffer, int bufferSize, std::string* outputError,
 std::stringstream* commentsGeneral, bool includeNoErrorInComments)
{ ERR_clear_error();
  int result = SSL_read(theSSL, buffer, bufferSize);
  this->ClearErrorQueue
  (result, theSSL, outputError, commentsGeneral, includeNoErrorInComments);
  return result;
}

int SSLdata::SSLWrite
(SSL* theSSL, void* buffer, int bufferSize, std::string* outputError,
 std::stringstream* commentsGeneral, bool includeNoErrorInComments)
{ ERR_clear_error();
  int result = SSL_write(theSSL, buffer, bufferSize);
  this->ClearErrorQueue
  (result, theSSL, outputError, commentsGeneral, includeNoErrorInComments);
  return result;
}

bool SSLdata::SSLReadLoop
(int numTries, SSL* theSSL, std::string& output, const LargeInt& expectedLength,
 std::string* outputError, std::stringstream* commentsGeneral, bool includeNoErrorInComments)
{ MacroRegisterFunctionWithName("SSLdata::SSLreadLoop");
  this->buffer.SetSize(100000);
  output = "";
  int i = 0;
  std::string next;
  int numBytes = - 1;
  for (i = 0; i < numTries; i ++)
  { numBytes = theWebServer.theSSLdata.SSLRead
    (theSSL, this->buffer.TheObjects, this->buffer.size,
     outputError, commentsGeneral, includeNoErrorInComments);
    if (numBytes > 0)
    { next.assign(this->buffer.TheObjects, numBytes);
      output += next;
      if (expectedLength <= 0)
        break;
      else
        if (expectedLength <= output.size())
          break;
    }
    if (numBytes == 0)
      break;
  }
  if (numBytes < 0)
  { if (commentsGeneral != 0)
      *commentsGeneral << "SSL-ERROR reading from socket. ";
    return false;
  }
  return true;
}

#include "vpfHeader5Crypto.h"
bool SSLdata::SSLWriteLoop
(int numTries, SSL* theSSL, const std::string& input, std::string* outputError,
 std::stringstream* commentsGeneral, bool includeNoErrorInComments)
{ MacroRegisterFunctionWithName("SSLdata::SSLwriteLoop");
  Crypto::ConvertStringToListBytesSigned(input, this->buffer);
  int i = 0;
  int numBytes = - 1;
  for (i = 0; i < numTries; i ++)
  { numBytes = this->SSLWrite
    (theSSL, this->buffer.TheObjects, this->buffer.size,
     outputError, commentsGeneral, includeNoErrorInComments);
    if (numBytes == this->buffer.size)
      break;
  }
  if (numBytes != this->buffer.size)
  { if (commentsGeneral != 0)
      *commentsGeneral  << i << " errors writing to socket.\n NumBytes: " << numBytes << ". ";
    return false;
  }
  return true;
}

std::string SSLdata::errors::errorWantRead = "SSL_ERROR_WANT_READ";

void SSLdata::ClearErrorQueue
(int errorCode, SSL* theSSL, std::string* outputError,
 std::stringstream* commentsGeneral, bool includeNoErrorInComments)
{ MacroRegisterFunctionWithName("SSLdata::ToStringError");
#ifdef MACRO_use_open_ssl
  int numErrors = 0;
  int theCode = SSL_get_error(theSSL, errorCode);
  ERR_clear_error();
  if (theCode == SSL_ERROR_NONE)
  { if (commentsGeneral != 0 && includeNoErrorInComments)
      *commentsGeneral << "\n<br>\nNo error.\n";
    return;
  }
  numErrors ++;
  int extraErrorCode = 0;
  switch (theCode)
  {
  case SSL_ERROR_ZERO_RETURN:
    if (outputError != 0)
      *outputError = "SSL_ERROR_ZERO_RETURN";
    break;
  case SSL_ERROR_WANT_READ:
    if (outputError != 0)
      *outputError = SSLdata::errors::errorWantRead;
    break;
  case SSL_ERROR_WANT_WRITE:
    if (outputError != 0)
      *outputError = "SSL_ERROR_WANT_WRITE";
    break;
  case SSL_ERROR_WANT_CONNECT:
    if (outputError != 0)
      *outputError = "SSL_ERROR_WANT_CONNECT";
    break;
  case SSL_ERROR_WANT_ACCEPT:
    if (outputError != 0)
      *outputError = "SSL_ERROR_WANT_ACCEPT";
    break;
  case SSL_ERROR_WANT_X509_LOOKUP:
    if (outputError != 0)
      *outputError = "SSL_ERROR_WANT_X509_LOOKUP";
    break;
  case SSL_ERROR_SYSCALL:
    if (outputError != 0)
      *outputError = "SSL_ERROR_SYSCALL";
    extraErrorCode = ERR_get_error();
    if (extraErrorCode == 0)
    { if (commentsGeneral != 0)
        *commentsGeneral << "Bad eof. ";
    } else if (extraErrorCode == - 1)
    { if (commentsGeneral != 0)
        *commentsGeneral << "I/O error outside of ssl. "
        << theWebServer.ToStringLastErrorDescription();
    }
    break;
  case SSL_ERROR_SSL:
    if (outputError != 0)
      *outputError = "SSL_ERROR_SSL";
    break;
  default:
    if (outputError != 0)
      *outputError = "SSL_ERROR_unknown";
    break;
  }
#endif
}
#endif //MACRO_use_open_ssl

bool WebWorker::ReceiveAllHttpSSL()
{ MacroRegisterFunctionWithName("WebWorker::ReceiveAllHttpSSL");
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    return true;
#ifdef MACRO_use_open_ssl
  this->messageBody = "";
  this->messageHead = "";
  this->requestTypE = this->requestUnknown;
  unsigned const int bufferSize = 60000;
  char buffer[bufferSize];
  if (this->connectedSocketID == - 1)
    crash << "Attempting to receive on a socket with ID equal to - 1. " << crash;
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 5;  // 5 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  setsockopt(this->connectedSocketID, SOL_SOCKET, SO_RCVTIMEO, (void*)(&tv), sizeof(timeval));
  std::string errorString;
  int numFailedReceives = 0;
  int maxNumFailedReceives = 1;
  double numSecondsAtStart = theGlobalVariables.GetElapsedSeconds();
  int numBytesInBuffer = - 1;
  while (true)
  { numBytesInBuffer = this->parent->theSSLdata.SSLRead
    (this->parent->theSSLdata.sslServeR, &buffer, bufferSize - 1, &errorString, 0, true);
    if (numBytesInBuffer >= 0 && numBytesInBuffer <= (signed) bufferSize)
      break;
    numFailedReceives ++;
    if (numFailedReceives > maxNumFailedReceives)
    { if (errorString == SSLdata::errors::errorWantRead)
        this->error = errorString;
      else
      { std::stringstream out;
        out
        << "WebWorker::ReceiveAllHttpSSL on socket "
        << this->connectedSocketID
        << " failed (so far "
        << numFailedReceives << " fails). "
        << "Error: " << errorString << ". ";
        out << "Too many failed receives, aborting. ";
        this->error = out.str();
      }
      //logIO << out.str() << logger::endL;
      numBytesInBuffer = 0;
      return false;
    }
  }
  this->messageHead.assign(buffer, numBytesInBuffer);
  this->ParseMessageHead();
  if (this->requestTypE == WebWorker::requestTypes::requestPost)
    this->displayUserInput = "POST " + this->addressGetOrPost;
  else
    this->displayUserInput = "GET " + this->addressGetOrPost;
  if (this->ContentLength <= 0)
  { //logIO << " exiting successfully" << logger::endL;
    return true;
  }
  if (this->messageBody.size() == (unsigned) this->ContentLength)
    return true;
  this->messageBody.clear(); //<- needed else the length error check will pop.
  //logWorker << "Content-length parsed to be: " << this->ContentLength
  //<< "However the size of mainArgumentRAW is: " << this->mainArgumentRAW.size();
  if (this->ContentLength > 10000000)
  { this->CheckConsistency();
    error = "Content-length parsed to be more than 10 million bytes, aborting.";
    logWorker << this->error << logger::endL;
    this->displayUserInput = this->error;
    return false;
  }
  this->remainingBytesToSenD = (std::string) "HTTP/1.0 100 Continue\r\n\r\n";
  this->SendAllBytesNoHeaders();
  this->remainingBytesToSenD.SetSize(0);
  std::string bufferString;
  std::stringstream comments;
  while ((signed) this->messageBody.size() < this->ContentLength)
  { if (theGlobalVariables.GetElapsedSeconds() - numSecondsAtStart > 180)
    { this->error = "Receiving bytes timed out (180 seconds).";
      logIO << this->error << logger::endL;
      this->displayUserInput = this->error;
      return false;
    }
    numBytesInBuffer = this->parent->theSSLdata.SSLRead
    (this->parent->theSSLdata.sslServeR, &buffer, bufferSize - 1, &errorString, 0, true);
    if (numBytesInBuffer == 0)
    { this->error = "While trying to fetch message-body, received 0 bytes. " +
      this->parent->ToStringLastErrorDescription();
      this->displayUserInput = this->error;
      return false;
    }
    if (numBytesInBuffer < 0)
    { if (errno == EAGAIN ||
          errno == EWOULDBLOCK ||
          errno == EINTR ||
          errno == EIO ||
          errno == ENOBUFS ||
          errno == ENOMEM)
        continue;
      this->error = "Error fetching message body: " + this->parent->ToStringLastErrorDescription();
      ERR_print_errors_fp(stderr);
      logIO << this->error << logger::endL;
      this->displayUserInput = this->error;
      return false;
    }
    bufferString.assign(buffer, numBytesInBuffer);
    this->messageBody += bufferString;
  }
  if ((signed) this->messageBody.size() != this->ContentLength)
  { this->messageHead += this->messageBody;
    this->ParseMessageHead();
  }
  if ((signed) this->messageBody.size() != this->ContentLength)
  { std::stringstream out;
    out << "The message-body received by me had length " << this->messageBody.size()
    << " yet I expected a message of length " << this->ContentLength << ". More details follow. "
    << this->ToStringMessageFullUnsafe();
    logIO << out.str() << logger::endL;
  }
  return true;
  #else
  return false;
  #endif
}

void WebWorker::SendAllBytesHttpSSL()
{ MacroRegisterFunctionWithName("WebWorker::SendAllBytesHttpSSL");
#ifdef MACRO_use_open_ssl
  if (this->remainingBytesToSenD.size == 0)
    return;
  this->CheckConsistency();
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    crash << "Error: WebWorker::SendAllBytesHttpSSL called while flagUsingSSLinCurrentConnection is set to false. " << crash;
  if (this->connectedSocketID == - 1)
  { logWorker << logger::red << "Socket::SendAllBytes failed: connectedSocketID= - 1." << logger::endL;
    return;
  }
  logWorker << "SSL Worker " << this->indexInParent + 1
  << " sending " << this->remainingBytesToSenD.size << " bytes in chunks of: ";
  double startTime = theGlobalVariables.GetElapsedSeconds();
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 5;  // 5 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  int numTimesRunWithoutSending = 0;
  int timeOutInSeconds = 20;
  setsockopt(this->connectedSocketID, SOL_SOCKET, SO_SNDTIMEO, (void*)(&tv), sizeof(timeval));
  std::string errorString;
  while (this->remainingBytesToSenD.size > 0)
  { if (theGlobalVariables.GetElapsedSeconds() - startTime > timeOutInSeconds)
    { logWorker << "WebWorker::SendAllBytes failed: more than " << timeOutInSeconds << " seconds have elapsed. "
      << logger::endL;
      return;
    }
    int numBytesSent = this->parent->theSSLdata.SSLWrite
    (this->parent->theSSLdata.sslServeR, this->remainingBytesToSenD.TheObjects,
     this->remainingBytesToSenD.size, &errorString, 0, true);
    if (numBytesSent < 0)
    { logWorker << "WebWorker::SendAllBytes failed: SSL_write error. " << logger::endL;
      return;
    }
    if (numBytesSent == 0)
      numTimesRunWithoutSending ++;
    else
      numTimesRunWithoutSending = 0;
    logWorker << numBytesSent;
    this->remainingBytesToSenD.Slice(numBytesSent, this->remainingBytesToSenD.size - numBytesSent);
    if (this->remainingBytesToSenD.size > 0)
      logWorker << ", ";
    if (numTimesRunWithoutSending > 3)
    { logWorker << "WebWorker::SendAllBytes failed: send function went through 3 cycles without "
      << " sending any bytes. "
      << logger::endL;
      return;
    }
    logWorker.flush();
  }
  logWorker << "... done." << logger::endL;
#endif
}

bool ProgressReportWebServer::flagServerExists = true;
const int WebServer::maxNumPendingConnections = 1000000;

ProgressReportWebServer::ProgressReportWebServer(const std::string& inputStatus)
{ this->flagDeallocated = false;
  this->indexProgressReport = theWebServer.theProgressReports.size;
  this->SetStatus(inputStatus);
}

ProgressReportWebServer::ProgressReportWebServer()
{ this->flagDeallocated = false;
  this->indexProgressReport = theWebServer.theProgressReports.size;
}

ProgressReportWebServer::~ProgressReportWebServer()
{ if (!this->flagServerExists)
    return;
  theWebServer.theProgressReports.SetSize(this->indexProgressReport);
  this->flagDeallocated = true;
}

void ProgressReportWebServer::SetStatus(const std::string& inputStatus)
{ MacroRegisterFunctionWithName("ProgressReportWebServer::SetStatus");
  if (theGlobalVariables.flagComputationFinishedAllOutputSentClosing || !this->flagServerExists)
    return;
  if (!theGlobalVariables.flagRunningBuiltInWebServer)
    return;
  theWebServer.CheckConsistency();
  MutexRecursiveWrapper& safetyFirst = theGlobalVariables.MutexWebWorkerStaticFiasco;
  safetyFirst.LockMe();
  if (this->indexProgressReport >= theWebServer.theProgressReports.size)
    theWebServer.theProgressReports.SetSize(this->indexProgressReport + 1);
  theWebServer.theProgressReports[this->indexProgressReport] = inputStatus;
  //std::cout << "SetStatus: passed mutex section" << std::endl;
  std::stringstream toBePiped;
  for (int i = 0; i < theWebServer.theProgressReports.size; i ++)
    if (theWebServer.theProgressReports[i] != "")
      toBePiped << "<br>" << theWebServer.theProgressReports[i];
  safetyFirst.UnlockMe();
  if (!theGlobalVariables.flagRunningBuiltInWebServer)
    return;
  theWebServer.GetActiveWorker().pipeWorkerToWorkerStatus.WriteAfterEmptying(toBePiped.str(), false, false);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr* sa)
{ if (sa->sa_family == AF_INET)
    return &(((struct sockaddr_in*) sa)->sin_addr);
  return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

std::string WebWorker::ToStringMessageShortUnsafe(FormatExpressions* theFormat) const
{ //if (theGlobalVariables.flagUsingSSLinCurrentConnection)
  //  return "Message cannot be viewed when using SSL";
  std::stringstream out;
  bool useHtml = theFormat == 0 ? false: theFormat->flagUseHTML;
  std::string lineBreak = useHtml ? "<br>\n" : "\r\n";
  out << lineBreak;
  if (this->requestTypE == this->requestGet)
    out << "GET ";
  else if (this->requestTypE == this->requestPost)
    out << "POST ";
  else if (this->requestTypE == this->requestChunked)
    out << "GET or POST **chunked** " ;
  else
    out << "Request type undefined.";
  out << "<hr>Address get or post:\n" << HtmlRoutines::ConvertStringToHtmlString(this->addressGetOrPost, true);
  out << lineBreak << "\nAddress computed:\n" << HtmlRoutines::ConvertStringToHtmlString(this->addressComputed, true);
  out << lineBreak << "\nArgument computed:\n" << HtmlRoutines::ConvertStringToHtmlString(this->argumentComputed, true);
  out << lineBreak << "\nVirtual file/directory name:\n" << HtmlRoutines::ConvertStringToHtmlString(this->VirtualFileName, true);
  out << lineBreak << "\nRelative physical file/directory name:\n" << HtmlRoutines::ConvertStringToHtmlString(this->RelativePhysicalFileNamE, true);
  out << lineBreak << "\nExecutable url:\n" << theGlobalVariables.DisplayNameExecutable;
  out << lineBreak << "\nPhysical address project base:\n" << theGlobalVariables.PhysicalPathProjectBase;
  out << lineBreak << "\nPhysical address server base:\n" << theGlobalVariables.PhysicalPathServerBasE;
  out << lineBreak << "\nPhysical address output folder:\n" << theGlobalVariables.PhysicalPathHtmlFolder;
  return out.str();
}

std::string WebWorker::ToStringMessageFullUnsafe(bool useHTML) const
{ //if (theGlobalVariables.flagUsingSSLinCurrentConnection)
  //  return "Message cannot be viewed when using SSL";
  std::stringstream out;
  out << this->ToStringMessageUnsafe(useHTML);
  if (this->theMessageHeaderStrings.size > 0)
  { out << "<hr>\nStrings extracted from message: ";
    for (int i = 0; i < this->theMessageHeaderStrings.size; i ++)
      out << "<br>" << this->theMessageHeaderStrings[i];
  }
  return out.str();
}

std::string WebWorker::ToStringMessageUnsafe(bool useHTML) const
{ //if (theGlobalVariables.flagUsingSSLinCurrentConnection)
  //  return "Message cannot be viewed when using SSL";
  std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML = true;
  out << this->ToStringMessageShortUnsafe(&tempFormat);
  out << "<hr>";
  out << "Main address: " << HtmlRoutines::ConvertStringToHtmlString(this->addressGetOrPost, true) << "<br>";
  if (this->requestTypE == this->requestGet)
    out << "GET " << HtmlRoutines::ConvertStringToHtmlString(this->addressGetOrPost, true);
  if (this->requestTypE == this->requestPost)
    out << "POST " << HtmlRoutines::ConvertStringToHtmlString(this->addressGetOrPost, true);
  if (this->requestTypE == this->requestChunked)
    out << "POST or GET **chunked** " << HtmlRoutines::ConvertStringToHtmlString(this->addressGetOrPost, true);
  if (this->flagKeepAlive)
    out << "<br><b>Keeping alive.</b><br>";
  else
    out << "<br><b>NOT</b> keeping alive.<br>";
  out << "<br>Cookies (" << this->cookies.size << " total):";
  for (int i = 0; i < this->cookies.size; i ++)
    out << "<br>" << this->cookies[i];
  if (useHTML)
  { out << "\n<hr>\nHost with port:<br>\n" << theGlobalVariables.hostWithPort;
    out << "\n<hr>\nFull message head:<br>\n" << HtmlRoutines::ConvertStringToHtmlString(this->messageHead, true);
    out << "\n<hr>\nFull message body:<br>\n" << HtmlRoutines::ConvertStringToHtmlString(this->messageBody, true);
  } else
  { out << "\nFull message head (length " << this->messageHead.size()
    << "):\n" << this->messageHead;
    out << "\nFull message body (length " << this->messageBody.size()
    << "):\n" << this->messageBody;
  }
  return out.str();
}

void WebWorker::resetConnection()
{ MacroRegisterFunctionWithName("WebWorker::resetConnection");
  //This function needs a security audit.
  this->resetMessageComponentsExceptRawMessage();
  UserCalculatorData blankUser;
  theGlobalVariables.userDefault = blankUser;
  if (theGlobalVariables.userDefault.enteredActivationToken != "" ||
      theGlobalVariables.userDefault.enteredAuthenticationToken != "" ||
      theGlobalVariables.userDefault.enteredGoogleToken != "" ||
      theGlobalVariables.userDefault.enteredPassword != "" ||
      theGlobalVariables.userDefault.enteredHashedSaltedPassword != "")
    crash << "User default not reset correctly. " << crash;
  theGlobalVariables.flagLoggedIn = false;
  theGlobalVariables.flagLogInAttempted = false;
  theGlobalVariables.webArguments.Clear();
  theGlobalVariables.userCalculatorRequestType = "";
  theGlobalVariables.userInputStringIfAvailable = "";
  theGlobalVariables.userInputStringRAWIfAvailable = "";
  this->flagAllBytesSentUsingFile = false;
  this->flagEncounteredErrorWhileServingFile = false;
}

void WebWorker::resetMessageComponentsExceptRawMessage()
{ this->addressComputed = "";
  this->addressGetOrPost = "";
  this->argumentComputed = "";
  this->RelativePhysicalFileNamE = "";
  this->VirtualFileName = "";
  this->displayUserInput = "";
  this->theMessageHeaderStrings.SetSize(0);
  this->requestHeaders.Clear();
  this->requestTypE = this->requestUnknown;
  this->ContentLength = - 1;
}

static std::stringstream standardOutputStreamAfterTimeout;
void WebWorker::StandardOutputAfterTimeOut(const std::string& input)
{ standardOutputStreamAfterTimeout << input;
}

std::string WebWorker::GetNavigationPage()
{ MacroRegisterFunctionWithName("WebWorker::GetNavigationPage");
  std::stringstream out;
  out << "<html><body>";
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=accounts" << "\">Accounts</a>";
  out << "<br>";
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=logout" << "\">Log out</a>";
  out << "<br>";
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=database" << "\">Database</a>";
  out << "<br>";
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=changePasswordPage" << "\">Change password</a>";
  out << "<br>";
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=statusPublic" << "\">Server status</a>";
  out << "<br>";
  out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=calculator" << "\">Calculator</a>";
  out << "</body></html>";
  return out.str();
}

std::string WebWorker::GetDatabaseJSON()
{ MacroRegisterFunctionWithName("WebWorker::GetDatabaseJSON");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return "Only logged-in admins can access database. ";
  std::stringstream out;
  std::string operation = theGlobalVariables.GetWebInput(WebAPI::databaseParameters::operation);
  std::string labels = HtmlRoutines::ConvertURLStringToNormal
  (theGlobalVariables.GetWebInput(WebAPI::databaseParameters::labels), false);
#ifdef MACRO_use_MongoDB
  JSData result;
  if (operation == WebAPI::databaseParameters::fetch)
    result = DatabaseRoutinesGlobalFunctionsMongo::ToJSONDatabaseFetch(labels);
  else
    result["error"] = "Uknown database operation: " + operation + ". ";
  if (theGlobalVariables.UserDebugFlagOn())
  { result["databaseOperation"] = operation;
    result["databaseLabels"] = labels;
  }
  out << result.ToString(false);
#else
  out << "<b>Database not available. </b>";
#endif // MACRO_use_MongoDB
  return out.str();
}

std::string WebWorker::GetDatabaseDeleteOneItem()
{ MacroRegisterFunctionWithName("WebWorker::GetDatabaseDeleteOneItem");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return "Only logged-in admins can execute the delete command. ";
  std::stringstream commentsStream;
  std::string inputEncoded = theGlobalVariables.GetWebInput("item");
  std::string inputString = HtmlRoutines::ConvertURLStringToNormal(inputEncoded, false);
  JSData inputParsed;
  if (!inputParsed.readstring(inputString, false, &commentsStream))
  { commentsStream << "Failed to parse input string. ";
    return commentsStream.str();
  }
  commentsStream << "Parsed input string: " << inputParsed.ToString(false, false) << "\n";
  if (DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntry(inputParsed, &commentsStream))
    return "success";
#ifdef MACRO_use_MongoDB

#else
  commentsStream << "<b>Database not available. </b>";
#endif // MACRO_use_MongoDB
  return commentsStream.str();
}

bool WebWorker::ExtractArgumentsFromCookies(std::stringstream& argumentProcessingFailureComments)
{ MacroRegisterFunctionWithName("WebWorker::ExtractArgumentsFromCookies");
  MapLisT<std::string, std::string, MathRoutines::hashString> newlyFoundArgs;
  bool result = true;
  for (int i = 0; i < this->cookies.size; i ++)
    if (!HtmlRoutines::ChopCGIStringAppend(this->cookies[i], newlyFoundArgs, argumentProcessingFailureComments))
      result = false;
  for (int i = 0; i < newlyFoundArgs.size(); i ++)
  { if (theGlobalVariables.webArguments.Contains(newlyFoundArgs.theKeys[i]))
      //if (theGlobalVariables.webArguments.GetValueCreate(newlyFoundArgs.theKeys[i]) != "")
      continue; //<-if a key is already given cookie entries are ignored.
    std::string trimmed = newlyFoundArgs.theValues[i];
    if (trimmed.size() > 0)
      if (trimmed[trimmed.size() - 1] == ';')
        trimmed = trimmed.substr(0, trimmed.size() - 1);
    //<-except the last cookie, cookies have extra semicolumn at the end, trimming.
    bool isGood = true;
    if (newlyFoundArgs.theKeys[i] == "request") //<- we are careful about
    { //reading arbitrary requests from the cookie. Those may effectively deny login
      //to a person who does not know to change the request type from the web address.
      //To prevent that we refuse to read requests from cookies except for the
      //whitelist below.
      isGood = false;
      if (trimmed == "template" ||
          trimmed == "exercise" ||
          trimmed == "database" ||
          trimmed == "accounts" ||
          trimmed == "calculator" ||
          trimmed == "scoredQuiz")
        isGood = true;
    }
    if (isGood)
      theGlobalVariables.webArguments.SetKeyValue(newlyFoundArgs.theKeys[i], trimmed);
  }
  return result;
}

bool WebWorker::ExtractArgumentsFromMessage
(const std::string& input, std::stringstream& argumentProcessingFailureComments, int recursionDepth)
{ MacroRegisterFunctionWithName("WebWorker::ExtractArgumentsFromMessage");
  if (recursionDepth > 1)
  { argumentProcessingFailureComments << "Error: input string encoded too many times";
    return false;
  }
  MapLisT<std::string, std::string, MathRoutines::hashString>& theArgs =
  theGlobalVariables.webArguments;
  if (!HtmlRoutines::ChopCGIStringAppend(input, theArgs, argumentProcessingFailureComments))
    return false;
  //argumentProcessingFailureComments << "DEBUG: args from message pure: " << theArgs.ToStringHtml();
  if (theGlobalVariables.flagRunningApache)
    if (this->addressComputed == "")
      this->addressComputed = theGlobalVariables.GetWebInput("request");
  if (theArgs.Contains("doubleURLencodedInput"))
  { std::string newInput = theGlobalVariables.GetWebInput("doubleURLencodedInput");
    theArgs.Clear();
    return this->ExtractArgumentsFromMessage
    (newInput, argumentProcessingFailureComments, recursionDepth + 1);
  }
  return true;
}

//Returns false if something unexpected happens during the login procedure.
//Returning true does not necessarily mean the login information was accepted.
//Returning false guarantees the login information was not accepted.
bool WebWorker::Login(std::stringstream& argumentProcessingFailureComments)
{ MacroRegisterFunctionWithName("WebWorker::Login");
  theGlobalVariables.flagLoggedIn = false;
  MapLisT<std::string, std::string, MathRoutines::hashString>& theArgs = theGlobalVariables.webArguments;
  UserCalculatorData& theUser = theGlobalVariables.userDefault;
  theUser.username = HtmlRoutines::ConvertURLStringToNormal
  (theGlobalVariables.GetWebInput("username"), true);
  if (theUser.username.find('%') != std::string::npos)
    argumentProcessingFailureComments << "<b>Unusual behavior: % sign in username.</b>";
  theUser.enteredAuthenticationToken = HtmlRoutines::ConvertURLStringToNormal
  (theGlobalVariables.GetWebInput("authenticationToken"), false);
  theUser.enteredGoogleToken = HtmlRoutines::ConvertURLStringToNormal
  (theGlobalVariables.GetWebInput("googleToken"), false);
  theUser.enteredActivationToken = HtmlRoutines::ConvertURLStringToNormal
  (theGlobalVariables.GetWebInput("activationToken"), false);

  theUser.enteredPassword =
  HtmlRoutines::ConvertStringToURLString
  (HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("password"), true), false);
  //<-Passwords are ONE-LAYER url-encoded
  //<-INCOMING pluses in passwords MUST be decoded as spaces, this is how form.submit() works!
  //<-Incoming pluses must be re-coded as spaces (%20).

  theUser.flagEnteredPassword = (theUser.enteredPassword != "");
  if (theUser.flagEnteredPassword)
  { theUser.flagMustLogin = true;
    theUser.enteredGoogleToken = "";
    theUser.enteredAuthenticationToken = "";
    theUser.enteredActivationToken = "";
    theUser.flagEnteredAuthenticationToken = false;
    theUser.flagEnteredActivationToken = false;
    theGlobalVariables.flagLogInAttempted = true;
    argumentProcessingFailureComments << "Password was entered: all other authentication methods ignored. ";
  }
  if (theUser.enteredActivationToken != "")
  { theUser.enteredGoogleToken = "";
    theUser.enteredAuthenticationToken = "";
    theUser.flagEnteredAuthenticationToken = false;
    theUser.flagEnteredActivationToken = true;
    theGlobalVariables.flagLogInAttempted = true;
    argumentProcessingFailureComments << "Activation token entered: authentication token and google token ignored. ";
  }
  if (theUser.username != "")
    theUser.enteredGoogleToken = "";
  if (theUser.enteredAuthenticationToken != "")
  { theUser.flagEnteredAuthenticationToken = true;
    theGlobalVariables.flagLogInAttempted = true;
  }
  /////////////////////////////////////////////
  //this may need a security audit: the URLStringToNormal and GetWebInput
  //functions may leave traces of (freed) memory
  //of the old password. Could only be used
  //if the attacker has control of the server executable - which probably means we
  //already are in big trouble - so this really shouldn't be such a big deal.
  /////////////////////////////////////////////
  if (theUser.flagEnteredPassword &&
      theUser.flagEnteredAuthenticationToken &&
      theUser.enteredActivationToken == "" &&
      theUser.enteredGoogleToken == "")
    return !theUser.flagMustLogin;
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    return false;
  bool doAttemptGoogleTokenLogin = false;
  if (theUser.enteredGoogleToken != "")
  { if (theUser.enteredActivationToken == "" &&
        theUser.enteredPassword == "" &&
        theUser.enteredAuthenticationToken == "")
    { theUser.username = "";
      doAttemptGoogleTokenLogin = true;
    }
    if (theUser.username == "")
      doAttemptGoogleTokenLogin = true;
  } else if (theUser.username == "")
  { //stOutput << "DEBUG: no username specified";
    return !theUser.flagMustLogin;
  }
  bool changingPass =
  theGlobalVariables.userCalculatorRequestType == WebAPI::request::changePassword ||
  theGlobalVariables.userCalculatorRequestType == WebAPI::request::activateAccount ||
  theGlobalVariables.userCalculatorRequestType == WebAPI::request::activateAccountJSON;
  if (changingPass)
    theUser.enteredAuthenticationToken = "";
  if (doAttemptGoogleTokenLogin)
  { theGlobalVariables.flagLoggedIn =
    DatabaseRoutinesGlobalFunctions::LoginViaGoogleTokenCreateNewAccountIfNeeded
    (theUser, &argumentProcessingFailureComments, 0);
  } else if (theUser.enteredAuthenticationToken != "" || theUser.enteredPassword != "" ||
             theUser.enteredActivationToken != "")
  { theGlobalVariables.flagLoggedIn = DatabaseRoutinesGlobalFunctions::LoginViaDatabase
    (theUser, &argumentProcessingFailureComments);
  }
  theGlobalVariables.CookiesToSetUsingHeaders.SetKeyValue("username",
   HtmlRoutines::ConvertStringToURLString(theUser.username, false)
   //<-User name must be stored in URL-encoded fashion, NO PLUSES.
  );
  if (theGlobalVariables.flagLoggedIn && theUser.enteredActivationToken == "")
  { //in case the user logged in with password, we need
    //to give him/her the correct authentication token
    theGlobalVariables.CookiesToSetUsingHeaders.SetKeyValue
    (DatabaseStrings::labelAuthenticationToken,
     HtmlRoutines::ConvertStringToURLString(theUser.actualAuthenticationToken, false)
     //<-URL-encoded fashion, NO PLUSES.
    );
    theGlobalVariables.SetWebInpuT
    (DatabaseStrings::labelAuthenticationToken,
     HtmlRoutines::ConvertStringToURLString(theUser.actualAuthenticationToken, false));
  } else
    theGlobalVariables.CookiesToSetUsingHeaders.SetKeyValue("authenticationToken", "0");
  bool shouldDisplayMessage = false;
  if (!theGlobalVariables.flagLoggedIn && theUser.username != "")
  { if (theUser.flagEnteredPassword || theUser.flagEnteredActivationToken)
      shouldDisplayMessage = true;
    if (theUser.flagEnteredAuthenticationToken)
      if (theUser.enteredActivationToken != "0" && theUser.enteredActivationToken != "")
        shouldDisplayMessage = true;
  }
  theUser.clearAuthenticationTokenAndPassword();
  if (shouldDisplayMessage)
    argumentProcessingFailureComments << "Invalid user and/or authentication. ";
  theArgs.SetKeyValue("password", "********************************************");
  return true;
}

std::string WebWorker::GetHtmlHiddenInputs(bool includeUserName, bool includeAuthenticationToken)
{ MacroRegisterFunctionWithName("WebWorker::GetHtmlHiddenInputs");
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    return "";
  std::stringstream out;
  if (includeAuthenticationToken)
    out << "<input type =\"hidden\" id =\"authenticationToken\" name =\"authenticationToken\">\n";
  if (includeUserName)
    out << "<input type =\"hidden\" id =\"username\" name =\"username\">\n";
  //the values of the hidden inputs will be filled in via javascript
  if (this->flagFoundMalformedFormInput)
    out << "<b>Your input formed had malformed entries.</b>";
  out
  << "<input type =\"hidden\" id =\"studentView\" name =\"studentView\">\n"
  << "<input type =\"hidden\" id =\"studentSection\" name =\"studentSection\">\n"
  << "<input type =\"hidden\" id =\"courseHome\" name =\"courseHome\">\n"
  << "<input type =\"hidden\" id =\"fileName\" name =\"fileName\">\n";
  return out.str();
}

void WebWorker::OutputResultAfterTimeout()
{ MacroRegisterFunctionWithName("WebWorker::OutputResultAfterTimeout");
  std::stringstream out;
  //out << theParser.ToStringOutputSpecials();
  std::string theFileName = "output/" + theGlobalVariables.RelativePhysicalNameOutpuT;
  std::string theLink = "output/" + HtmlRoutines::ConvertStringToURLString
  (theGlobalVariables.RelativePhysicalNameOutpuT, false);
  out << "Output written in: <a href=\"/"
  << theLink << "\" target = \"_blank\"> " << theLink << "</a><br>";
  //out <<  "DEBUG: filename: " << theFileName << "<br>";
  if (standardOutputStreamAfterTimeout.str().size() != 0)
    out << standardOutputStreamAfterTimeout.str() << "<hr>";
  out << theParser->ToStringOutputAndSpecials();
  std::fstream outputTimeOutFile;
  FileOperations::OpenFileCreateIfNotPresentVirtual
  (outputTimeOutFile, theFileName, false, true, false);
  outputTimeOutFile << "<html><body>" << out.str() << "</body></html>";
  outputTimeOutFile.close();
  WebWorker::OutputSendAfterTimeout(out.str());
}

void WebWorker::OutputCrashAfterTimeout()
{ MacroRegisterFunctionWithName("WebWorker::OutputCrashAfterTimeout");
  logWorker << logger::red << "Crashing AFTER timeout!" << logger::endL;
  WebWorker::OutputSendAfterTimeout(standardOutputStreamAfterTimeout.str());
  theWebServer.SignalActiveWorkerDoneReleaseEverything();
}

void WebWorker::OutputSendAfterTimeout(const std::string& input)
{ MacroRegisterFunctionWithName("WebWorker::OutputSendAfterTimeout");
  theGlobalVariables.flagTimedOutComputationIsDone = true;
  logWorker << "WebWorker::StandardOutputPart2ComputationTimeout called with worker number "
  << theWebServer.GetActiveWorker().indexInParent + 1 << "." << logger::endL;
  //requesting pause which will be cleared by the receiver of pipeWorkerToWorkerIndicatorData
  theWebServer.GetActiveWorker().PauseComputationReportReceived.RequestPausePauseIfLocked(false, false);
  logWorker << logger::blue << "Sending result through indicator pipe." << logger::endL;
  theWebServer.GetActiveWorker().pipeWorkerToWorkerIndicatorData.WriteAfterEmptying(input, false, false);
  logBlock << logger::blue << "Final output written to indicator, blocking until data "
  << "is received on the other end." << logger::endL;
  if (!theWebServer.GetActiveWorker().PauseComputationReportReceived.PauseIfRequestedWithTimeOut(false, false))
  {} else
  {//requesting pause which will be cleared by the receiver of pipeWorkerToWorkerIndicatorData
    theWebServer.GetActiveWorker().PauseComputationReportReceived.RequestPausePauseIfLocked(false, false);
  }
  theWebServer.GetActiveWorker().pipeWorkerToWorkerIndicatorData.WriteAfterEmptying("finished", false, false);
  logWorker << logger::red << "\"finished\" sent through indicator pipe, waiting." << logger::endL;
  theWebServer.GetActiveWorker().PauseComputationReportReceived.PauseIfRequestedWithTimeOut(false, false);
}

void WebWorker::ParseMessageHead()
{ MacroRegisterFunctionWithName("WebWorker::ParseMessageHead");
  this->resetMessageComponentsExceptRawMessage();
  std::string buffer;
  buffer.reserve(this->messageHead.size());
  this->theMessageHeaderStrings.SetExpectedSize(50);
  this->cookies.SetExpectedSize(30);
  this->theMessageHeaderStrings.size = 0;
  this->cookies.size = 0;
  this->connectionFlags.size = 0;
  this->messageBody = "";
  this->flagKeepAlive = false;
  this->ContentLength = - 1;
  this->addressGetOrPost = "";
  this->hostWithPort = "";
  for (unsigned i = 0; i < this->messageHead.size(); i ++)
    if (this->messageHead[i] != ' ' && this->messageHead[i] != '\n' && this->messageHead[i] != '\r')
    { buffer.push_back(this->messageHead[i]);
      if (i == this->messageHead.size() - 1)
        this->theMessageHeaderStrings.AddOnTop(buffer);
    } else
    { if (buffer != "")
      { this->theMessageHeaderStrings.AddOnTop(buffer);
        buffer = "";
      }
      if (i > 0)
        if (this->messageHead[i - 1] == '\r' && this->messageHead[i] == '\n')
          this->theMessageHeaderStrings.AddOnTop("\n");
    }
  for (int i = 0; i < this->theMessageHeaderStrings.size; i ++)
    if (this->theMessageHeaderStrings[i] == "GET")
    { this->requestTypE = this->requestGet;
      i ++;
      if (i < this->theMessageHeaderStrings.size)
        this->addressGetOrPost = this->theMessageHeaderStrings[i];
    } else if (this->theMessageHeaderStrings[i] == "POST")
    { this->requestTypE = this->requestPost;
      i ++;
      //Short post messages may be attached to the message head
      //if that is the case the message does not end in \n
      if (i < this->theMessageHeaderStrings.size)
      { this->addressGetOrPost = this->theMessageHeaderStrings[i];
        if (*this->theMessageHeaderStrings.LastObject() != "\n")
          this->messageBody = *this->theMessageHeaderStrings.LastObject();
        else
          this->messageBody = "";
      }
    } else if (this->theMessageHeaderStrings[i] == "HEAD")
    { this->requestTypE = this->requestHead;
      i ++;
      if (i < this->theMessageHeaderStrings.size)
        this->addressGetOrPost = this->theMessageHeaderStrings[i];
    } else if ( this->theMessageHeaderStrings[i] == "transfer-coding:" ||
                this->theMessageHeaderStrings[i] == "Transfer-coding:" ||
                this->theMessageHeaderStrings[i] == "Transfer-Coding:" )
    { i ++;
      if (i < this->theMessageHeaderStrings.size)
        if (this->theMessageHeaderStrings[i] == "chunked" ||
           this->theMessageHeaderStrings[i] == "chunked;"||
           this->theMessageHeaderStrings[i] == "Chunked"||
           this->theMessageHeaderStrings[i] == "Chunked;"
           )
          this->requestTypE = this->requestChunked;
    } else if ((this->theMessageHeaderStrings[i] == "Content-Length:" ||
                this->theMessageHeaderStrings[i] == "Content-length:" ||
                this->theMessageHeaderStrings[i] == "content-length:")
               && i + 1 < this->theMessageHeaderStrings.size)
    { if (this->theMessageHeaderStrings[i + 1].size() < 10000)
      { LargeIntUnsigned theLI;
        if (theLI.AssignStringFailureAllowed(this->theMessageHeaderStrings[i + 1], true))
          if (!theLI.IsIntegerFittingInInt(&this->ContentLength))
            this->ContentLength = - 1;
      }
    } else if (this->theMessageHeaderStrings[i] == "Host:" ||
               this->theMessageHeaderStrings[i] == "host:")
    { i ++;
      if (i < this->theMessageHeaderStrings.size)
        this->hostWithPort = this->theMessageHeaderStrings[i];
    } else if (this->theMessageHeaderStrings[i] == "Cookie:" ||
               this->theMessageHeaderStrings[i] == "cookie:")
    { i ++;
      for (; i < this->theMessageHeaderStrings.size; i ++)
      { if (this->theMessageHeaderStrings[i] == "\n")
          break;
        this->cookies.AddOnTop(this->theMessageHeaderStrings[i]);
      }
    } else if (this->theMessageHeaderStrings[i] == "Connection:" ||
               this->theMessageHeaderStrings[i] == "connection:")
    { i ++;
      for (; i < this->theMessageHeaderStrings.size; i ++)
      { if (this->theMessageHeaderStrings[i] == "\n")
          break;
        this->connectionFlags.AddOnTop(this->theMessageHeaderStrings[i]);
        if (this->theMessageHeaderStrings[i] == "keep-alive" ||
            this->theMessageHeaderStrings[i] == "Keep-Alive" ||
            this->theMessageHeaderStrings[i] == "Keep-alive")
          this->flagKeepAlive = true;
      }
    }
  theGlobalVariables.hostWithPort = this->hostWithPort;
}

void WebWorker::AttemptUnknownRequestErrorCorrection()
{ MacroRegisterFunctionWithName("WebWorker::AttemptUnknownRequestErrorCorrection");
  if (this->requestTypE != this->requestUnknown)
    return;
  logIO << logger::red << "Unknown request." << logger::endL;
  logIO << logger::blue << "Message head length: " << this->messageHead.size() << logger::endL;
  logIO << HtmlRoutines::ConvertStringToHtmlStringRestrictSize(this->messageHead, false, 300) << logger::endL;
  logIO << logger::orange << "Message body length: " << this->messageBody.size() << logger::endL;
  logIO << HtmlRoutines::ConvertStringToHtmlStringRestrictSize(this->messageBody, false, 300) << logger::endL;
  logIO << logger::green << "Attempting to correct unknown request.\n";
  if (this->messageBody.size() == 0)
    if (*this->theMessageHeaderStrings.LastObject() != "\n")
    { logIO << logger::green
      << "Message body set to last message chunk.\n";
      this->messageBody = *this->theMessageHeaderStrings.LastObject();
    }
  if (this->messageBody.size() != 0)
  { logIO << "Request set to: POST\n";
    this->requestTypE = this->requestPost;
  } else
  { logIO << "Request set to: GET\n";
    this->requestTypE = this->requestGet;
  }
  if (this->addressGetOrPost == "")
  { logIO << "Address set to: " << theGlobalVariables.DisplayNameExecutable << "\n";
    this->addressGetOrPost = theGlobalVariables.DisplayNameExecutable;
  }
  logIO << logger::blue
  << "Unrecognized message head, length: " << this->messageHead.size() << ".\n";
  logIO << logger::red << "Message body length: " << this->messageBody.size() << ". ";
  logIO << logger::endL;
}

bool WebWorker::ReceiveAllHttp()
{ MacroRegisterFunctionWithName("WebWorker::ReceiveAllHttp");
  this->messageBody = "";
  this->messageHead = "";
  this->requestTypE = this->requestUnknown;
  unsigned const int bufferSize = 60000;
  char buffer[bufferSize];
  if (this->connectedSocketID == - 1)
    crash << "Attempting to receive on a socket with ID equal to - 1. " << crash;
  //std::cout << "Got thus far 10" << std::endl;
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 5;  // 5 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  setsockopt(this->connectedSocketID, SOL_SOCKET, SO_RCVTIMEO, (void*)(&tv), sizeof(timeval));
  double numSecondsAtStart = theGlobalVariables.GetElapsedSeconds();
  int numBytesInBuffer = recv(this->connectedSocketID, &buffer, bufferSize - 1, 0);
  int numFailedReceives = 0;
  bool result = true;
  while ((numBytesInBuffer < 0) || (numBytesInBuffer > ((signed)bufferSize)))
  { std::stringstream out;
    numFailedReceives ++;
    out
    << "WebWorker::ReceiveAllHttp on socket " << this->connectedSocketID
    << " failed (so far "
    << numFailedReceives << " fails). "
    << "Return value: " << numBytesInBuffer
    << ". Error description: "
    << this->parent->ToStringLastErrorDescription();
    if (numFailedReceives > 5)
    { out
      << ". 5+ failed receives so far, aborting. ";
      this->displayUserInput = out.str();
      this->error = out.str();
      logIO << out.str() << logger::endL;
      numBytesInBuffer = 0;
      result = false;
      break;
    }
    logIO << logger::orange << out.str() << "\n";
    //std::string bufferCopy(buffer, bufferSize);
    logIO << "Number of bytes in buffer so far: " << bufferSize;
    logIO << logger::endL;
    numBytesInBuffer = recv(this->connectedSocketID, &buffer, bufferSize - 1, 0);
  }
  this->messageHead.assign(buffer, numBytesInBuffer);
  if (numBytesInBuffer == 0)
    return true;
  this->ParseMessageHead();
  if (this->requestTypE == WebWorker::requestTypes::requestPost)
    this->displayUserInput = "POST " + this->addressGetOrPost;
  else if (this->requestTypE == WebWorker::requestTypes::requestGet)
    this->displayUserInput = "GET " + this->addressGetOrPost;
  else if (this->requestTypE == WebWorker::requestTypes::requestHead)
    this->displayUserInput = "HEAD " + this->addressGetOrPost;
  else if (this->requestTypE == WebWorker::requestTypes::requestChunked)
    this->displayUserInput = "GET or POST **chunked** " + this->addressGetOrPost;
  else
    this->displayUserInput = "UNKNOWN REQUEST " + this->addressGetOrPost;
  if (this->ContentLength <= 0)
  { if (this->requestTypE == this->requestUnknown)
      this->AttemptUnknownRequestErrorCorrection();
    return true;
  }
  if (this->messageBody.size() == (unsigned) this->ContentLength)
  { if (this->requestTypE == this->requestUnknown)
      this->AttemptUnknownRequestErrorCorrection();
    return true;
  }
  this->messageBody.clear();//<-needed else the length error check won't work out.
  if (this->ContentLength > 10000000)
  { this->CheckConsistency();
    error = "Content-length parsed to be more than 10 million bytes, aborting.";
    logWorker << this->error << logger::endL;
    this->displayUserInput = this->error;
    return false;
  }
  this->remainingBytesToSenD = (std::string) "HTTP/1.0 100 Continue\r\n\r\n";
  this->SendAllBytesNoHeaders();
  this->remainingBytesToSenD.SetSize(0);
  std::string bufferString;
  //logWorker << logger::red << "DEBUG: expecting: " << this->ContentLength << " bytes " << logger::endL;
  while ((signed) this->messageBody.size() < this->ContentLength)
  { if (theGlobalVariables.GetElapsedSeconds() - numSecondsAtStart > 180)
    { this->error = "Receiving bytes timed out (180 seconds).";
      logIO << this->error << logger::endL;
      this->displayUserInput = this->error;
      return false;
    }
    numBytesInBuffer = recv(this->connectedSocketID, &buffer, bufferSize - 1, 0);
    if (numBytesInBuffer == 0)
    { this->error = "While trying to fetch message-body, received 0 bytes. " +
      this->parent->ToStringLastErrorDescription();
      logIO << this->error << logger::endL;
      this->displayUserInput = this->error;
      return false;
    }
    if (numBytesInBuffer < 0)
    { if (errno == EAGAIN ||
          errno == EWOULDBLOCK ||
          errno == EINTR ||
          errno == EIO || // errno == ENOBUFS ||
          errno == ENOMEM)
        continue;
      this->error = "Error fetching message body: " + this->parent->ToStringLastErrorDescription();
      logIO << logger::red << this->error << logger::endL;
      this->displayUserInput = this->error;
      return false;
    }
    bufferString.assign(buffer, numBytesInBuffer);
    this->messageBody += bufferString;
  }
  if ((signed) this->messageBody.size() != this->ContentLength)
    if (this->requestTypE != this->requestChunked)
    { logIO << logger::red
      << "Message body is of length: " << this->messageBody.size()
      << " yet this->ContentLength equals: "
      << this->ContentLength
      << ". Perhaps very long headers got truncated? "
      << logger::endL;
      this->messageHead += this->messageBody;
      this->ParseMessageHead();
      this->AttemptUnknownRequestErrorCorrection();
    }
  if ((signed) this->messageBody.size() != this->ContentLength)
  { std::stringstream out;
    out << "The message-body received by me had length "
    << this->messageBody.size()
    << " yet I expected a message of length "
    << this->ContentLength << ". The message body was: "
    << this->messageBody;
    this->error = out.str();
    logIO << out.str() << logger::endL;
    this->AttemptUnknownRequestErrorCorrection();
  }
  return result;
}

void WebWorker::ReportDisplayUserInput()
{ MacroRegisterFunctionWithName("WebWorker::ReportDisplayUserInput");
  if (this->displayUserInput.size() > 3000)
    this->displayUserInput.resize(3000);
  this->pipeWorkerToWorkerUserInput.WriteAfterEmptying
  (this->displayUserInput,false, false);
  //logWorker << logger::blue << "Piping " << this->displayUserInput << " to the server. " << logger::endL;
}

void WebWorker::ExtractHostInfo()
{ MacroRegisterFunctionWithName("WebWorker::ExtractHostInfo");
  unsigned int i = 0;
  for (i = 0; i < this->hostWithPort.size(); i ++)
    if (this->hostWithPort[i] == ':')
      break;
  if (i < this->hostWithPort.size())
    this->hostNoPort = this->hostWithPort.substr(0, i);
  else
    this->hostNoPort = this->hostWithPort;
  theGlobalVariables.hostWithPort = this->hostWithPort;
  theGlobalVariables.hostNoPort = this->hostNoPort;
  if (theGlobalVariables.hostNoPort == "localhost" ||
      theGlobalVariables.hostNoPort == "127.0.0.1")
    theGlobalVariables.flagRequestComingLocally = true;
  else
    theGlobalVariables.flagRequestComingLocally = false;
}

void WebWorker::ExtractAddressParts()
{ MacroRegisterFunctionWithName("WebWorker::ExtractAdressParts");
  bool found = false;
  for (signed i = 0; i < (signed) this->addressGetOrPost.size(); i ++)
    if (this->addressGetOrPost[i] == '?')
    { this->addressComputed = this->addressGetOrPost.substr(0, i);
      this->argumentComputed = this->addressGetOrPost.substr(i + 1, std::string::npos);
      found = true;
      break;
    }
  if (!found)
    this->addressComputed = this->addressGetOrPost;
  if (this->messageBody != "")
    this->argumentComputed = this->messageBody;
  if (theGlobalVariables.flagRunningApache && this->argumentComputed == "")
  { this->argumentComputed = this->addressComputed;
    this->addressComputed = "";
  }
}

std::string WebWorker::GetHeaderConnectionClose()
{ std::stringstream out;
  out << "Connection: close";
  return out.str();
}

std::string WebWorker::GetHeaderConnectionKeepAlive()
{ std::stringstream out;
  out << "Connection: keep-alive";
  return out.str();
}

std::string WebWorker::GetHeaderSetCookie()
{ std::stringstream out;
  for (int i = 0; i < theGlobalVariables.CookiesToSetUsingHeaders.size(); i ++)
  { out << "Set-Cookie: " << theGlobalVariables.CookiesToSetUsingHeaders.theKeys[i]
    << "="
    << theGlobalVariables.CookiesToSetUsingHeaders.theValues[i]
    << "; Path=/; Expires=Sat, 01 Jan 2030 20:00:00 GMT; Secure";
    if (i != theGlobalVariables.CookiesToSetUsingHeaders.size() - 1)
      out << "\r\n";
  }
  return out.str();
}

void WebWorker::SetHeadeR(const std::string& httpResponseNoTermination, const std::string& remainingHeaderNoTermination)
{ MacroRegisterFunctionWithName("WebWorker::SetHeader");
  if (theGlobalVariables.flagRunningApache)
  { if (remainingHeaderNoTermination != "")
      stOutput << remainingHeaderNoTermination << "\r\n";
    if (theGlobalVariables.flagLoggedIn && WebWorker::GetHeaderSetCookie() != "")
      stOutput << WebWorker::GetHeaderSetCookie() << "\r\n";
    stOutput << "\r\n";
    return;
  }
  std::stringstream out;
  out << httpResponseNoTermination << "\r\n";
  if (!this->flagKeepAlive)
    out << this->GetHeaderConnectionClose() << "\r\n";
  else
    out << this->GetHeaderConnectionKeepAlive() << "\r\n";

  if (remainingHeaderNoTermination != "")
    out << remainingHeaderNoTermination << "\r\n";
  if ((theGlobalVariables.flagLoggedIn || theGlobalVariables.flagLogInAttempted) &&
      WebWorker::GetHeaderSetCookie() != "")
    out << WebWorker::GetHeaderSetCookie() << "\r\n";
  std::string finalHeader = out.str();
  this->remainingHeaderToSend.SetSize(0);
  this->remainingHeaderToSend.SetExpectedSize(finalHeader.size());
  for (unsigned i = 0; i < finalHeader.size(); i ++)
    this->remainingHeaderToSend.AddOnTop(finalHeader[i]);
}

void WebWorker::SetHeaderOKNoContentLength(const std::string& extraHeader, const std::string &contentType)
{ MacroRegisterFunctionWithName("WebWorker::SetHeaderOKNoContentLength");
  std::string header = "Content-Type: " + contentType + "; charset=utf-8\r\nAccess-Control-Allow-Origin: *";
  if (extraHeader != "")
    header += "\r\n" + extraHeader;
  this->SetHeadeR("HTTP/1.0 200 OK", header);
  this->flagDoAddContentLength = true;
}

void WebWorker::SanitizeVirtualFileName()
{ MacroRegisterFunctionWithName("WebWorker::SanitizePhysicalFileName");
  std::string resultName;
  resultName.reserve(this->VirtualFileName.size());
  bool foundslash = false;
  for (signed i = (signed) this->VirtualFileName.size() - 1; i >= 0; i --)
  { bool isOK = true;
    if (foundslash && this->VirtualFileName[i] == '.')
      if (i > 0 && this->VirtualFileName[i - 1] == '.')
      { this->flagFileNameSanitized = true;
        isOK = false;
      }
    if (isOK)
      resultName.push_back(this->VirtualFileName[i]);
    if (this->VirtualFileName[i] == '/')
      foundslash = true;
  }
  this->VirtualFileName = "";
  for (int i = resultName.size() - 1; i >= 0; i --)
    this->VirtualFileName.push_back(resultName[i]);
}

int WebWorker::ProcessCalculatorExamplesJSON()
{ MacroRegisterFunctionWithName("WebWorker::ProcessCalculatorExamplesJSON");
  this->SetHeaderOKNoContentLength("");
  stOutput << theParser->ToStringFunctionHandlersJSON();
  return 0;
}

int WebWorker::ProcessToggleMonitoring()
{ MacroRegisterFunctionWithName("WebWorker::ProcessToggleMonitoring");
  this->SetHeaderOKNoContentLength("");
  if (theGlobalVariables.UserDefaultHasAdminRights())
    this->flagToggleMonitoring = true;
  JSData result;
  HtmlInterpretation::GetJSDataUserInfo(result, "Attempt to toggle monitoring. ");
  stOutput << result.ToString(false);
  return 0;
}

int WebWorker::ProcessServerStatusJSON()
{ MacroRegisterFunctionWithName("WebWorker::ProcessServerStatusJSON");
  this->SetHeaderOKNoContentLength("");
  std::stringstream out;
  out << " <table><tr><td style =\"vertical-align:top\">"
  << this->parent->ToStringStatusAll() << "</td><td>"
  << theGlobalVariables.ToStringHTMLTopCommandLinuxSystem()
  << "</td></tr></table>";
  JSData outputJS;
  outputJS["result"] = out.str();
  stOutput << outputJS.ToString(false);
  return 0;
}

int WebWorker::ProcessPauseWorker()
{ MacroRegisterFunctionWithName("WebWorker::ProcessPauseWorker");
  this->SetHeaderOKNoContentLength("");
  std::string theMainInput = theGlobalVariables.GetWebInput("mainInput");
  std::stringstream out;
  JSData result;
  if (theMainInput == "")
  { result["error"] = "To pause a worker please 1) turn on monitoring and 2) provide the worker number in the mainInput field. ";
    stOutput << result.ToString(false);
    return 0;
  }
  int inputWebWorkerNumber = atoi(theMainInput.c_str());
  int inputWebWorkerIndex = inputWebWorkerNumber - 1;
  if (inputWebWorkerIndex < 0 || inputWebWorkerIndex >= this->parent->theWorkers.size)
  { out << "<b>User requested worker number " << inputWebWorkerNumber
    << " out of " << this->parent->theWorkers.size << " which is out of range. </b>";
    result["error"] = out.str();
    stOutput << result.ToString(false);
    return 0;
  }
  if (!this->parent->theWorkers[inputWebWorkerIndex].flagInUse)
  { out << "<b>Requested worker number " << inputWebWorkerNumber << " is not in use. Total number of workers: "
    << this->parent->theWorkers.size << ". </b>";
    result["error"] = out.str();
    stOutput << result.ToString(false);
    return 0;
  }
  if (inputWebWorkerIndex == this->indexInParent)
  { out << "Worker " << inputWebWorkerIndex
    << " attempts to pause itself, this is not allowed.";
    result["error"] = out.str();
    stOutput << result.ToString(false);
    return 0;
  }
  logWorker << "Proceeding to toggle worker pause." << logger::endL;
  WebWorker& otherWorker = this->parent->theWorkers[inputWebWorkerIndex];
  if (!otherWorker.PauseWorker.CheckPauseIsRequested(false, false, false))
  { otherWorker.PauseWorker.RequestPausePauseIfLocked(false, false);
    result["status"]= "paused";
    stOutput << result.ToString(false);
    return 0;
  }
  result["status"]= "unpaused";
  stOutput << result.ToString(false);
  otherWorker.PauseWorker.ResumePausedProcessesIfAny(false, false);
  return 0;
}

int WebWorker::ProcessMonitor()
{ MacroRegisterFunctionWithName("WebWorker::ProcessMonitor");
  this->SetHeaderOKNoContentLength("");
  logWorker << "Processing get monitor." << logger::endL;
  JSData result;
  HtmlInterpretation::GetJSDataUserInfo(result, "");
  std::string theMainInput = theGlobalVariables.GetWebInput("mainInput");
  if (theMainInput == "") {
    result["error"] = "<b>Monitor takes as argument the number of the child process that is running the computation.</b>";
    stOutput << result.ToString(false);
    return 0;
  }
  if (theGlobalVariables.flagAllowProcessMonitoring && theGlobalVariables.UserDefaultHasAdminRights())
  { result["webWorkerNumber"] = atoi(theMainInput.c_str());
    result["authenticated"] = "true";
  } else
  { result["authenticated"] = "false";
    std::stringstream commentStream;
    commentStream << "Process monitoring is allowed only for logged-in admins with process monitoring turned on. "
    << "The link to turn on process monitoring in the calculator navigation panel. ";
    result["comments"] = commentStream.str();
  }
  stOutput << result.ToString(false);
  return 0;
}

int WebWorker::ProcessComputationIndicator()
{ MacroRegisterFunctionWithName("WebWorker::ProcessComputationIndicator");
  this->SetHeaderOKNoContentLength("");
  logWorker << "Processing get request indicator." << logger::endL;
  std::string theMainInput = theGlobalVariables.GetWebInput("mainInput");
  JSData result;
  std::stringstream out, comments;
  if (!theGlobalVariables.UserDefaultHasAdminRights())
  { result["error"] = "Process monitoring only allowed to logged-in admins. ";
    stOutput << result.ToString(false);
    return 0;
  }
  if (theMainInput == "")
  { out << "To get a computation indicator you need to supply the number of "
    << "the child process in the mainInput field.";
    result["error"] = out.str();
    stOutput << result.ToString(false);
    return 0;
  }
  int inputWebWorkerNumber = atoi(theMainInput.c_str());
  int inputWebWorkerIndex = inputWebWorkerNumber - 1;
  if (inputWebWorkerIndex < 0 || inputWebWorkerIndex >= this->parent->theWorkers.size)
  { out << "Indicator error. Worker number "
    << inputWebWorkerNumber << " is out of range: there are "
    << this->parent->theWorkers.size
    << " workers. ";
    result["error"] = out.str();
    stOutput << result.ToString(false);
    return 0;
  }
  comments
  << "The maximum number of "
  << " connections/computations you can run is: "
  << theWebServer.MaxNumWorkersPerIPAdress << ". "
  << "The most recent error message reported by the "
  << "worker you want to monitor is: "
  << this->parent->theWorkers[inputWebWorkerIndex].pingMessage;
  if (!this->parent->theWorkers[inputWebWorkerIndex].flagInUse)
  { out << "Indicator error. Worker number "
    << inputWebWorkerNumber << " is not in use. "
    << " Total number of workers: "
    << this->parent->theWorkers.size << ". ";
    result["error"] = out.str();
    result["comments"] = comments.str();
    stOutput << result.ToString(false);
    return 0;
  }
  if (inputWebWorkerIndex == this->indexInParent)
  { out << "Indicator error. Worker number "
    << inputWebWorkerNumber << " requested to monitor itself. "
    << " This is not allowed. ";
    result["error"] = out.str();
    result["comments"] = comments.str();
    stOutput << result.ToString(false);
    return 0;
  }
  WebWorker& otherWorker = this->parent->theWorkers[inputWebWorkerIndex];
  if (otherWorker.userAddress.theObject != this->userAddress.theObject)
  { out << "User ip address: " << this->userAddress.theObject
    << " does not coincide with address that initiated the process. This is not allowed. ";
    result["error"] = out.str();
    stOutput << result.ToString(false);
    return 0;
  }
  if (this->flagUsingSSLInWorkerProcess != true)
  { out << "Monitoring allowed only over https. ";
    result["error"] = out.str();
    stOutput << result.ToString(false);
    return 0;
  }
  otherWorker.pipeWorkerToWorkerRequestIndicator.WriteAfterEmptying("!", false, false);
  otherWorker.pipeWorkerToWorkerIndicatorData.Read(false, false);
  if (otherWorker.pipeWorkerToWorkerIndicatorData.thePipe.lastRead.size > 0)
  { std::string outputString;
    outputString.assign(otherWorker.pipeWorkerToWorkerIndicatorData.thePipe.lastRead.TheObjects, otherWorker.pipeWorkerToWorkerIndicatorData.thePipe.lastRead.size);
    if (outputString != "finished")
    { result["data"] = outputString;
      result["status"] = "running";
    } else
      result["status"] = "finished";
    stOutput << result.ToString(false);
    otherWorker.PauseComputationReportReceived.ResumePausedProcessesIfAny(false, false);
  } //else
  //Empty response
  //will not be displayed, keeping the previous response displayed.
  //  stOutput << "<b>Not implemented: request for indicator for worker " << inputWebWorkerNumber
  //  << " out of " << this->parent->theWorkers.size << ".</b>";
  return 0;
}

void WebWorker::WriteProgressReportToFile(const std::string& input)
{ MacroRegisterFunctionWithName("WebWorker::WriteProgressReportToFile");
  logWorker << logger::green << "Progress report written to file: " << theGlobalVariables.RelativePhysicalNameProgressReport << logger::endL;
  std::fstream theFile;
  if (!FileOperations::OpenFileCreateIfNotPresentVirtual(theFile, "output/" + theGlobalVariables.RelativePhysicalNameProgressReport, false, true, false))
    FileOperations::OpenFileCreateIfNotPresentVirtual
    (theFile, "output/progressReport_failed_to_create_file.html",
     false, true, false);
  theFile << standardOutputStreamAfterTimeout.str() << "<hr>" << input;
  theFile.flush();
  theFile.close();
}

void WebWorker::PipeProgressReportToParentProcess(const std::string& input)
{ if (!this->flagProgressReportAllowed)
    return;
  MacroRegisterFunctionWithName("WebWorker::PipeProgressReportToParentProcess");
  this->PauseIndicatorPipeInUse.RequestPausePauseIfLocked(false, false);
  if (this->PauseWorker.CheckPauseIsRequested(false, false, false))
  { logBlock << ": pausing as requested ..." << logger::endL;
    this->WriteProgressReportToFile(input);
  }
  this->PauseWorker.PauseIfRequested(false, false);     //if pause was requested, here we block
  this->pipeWorkerToWorkerRequestIndicator.Read(false, false);
  if (this->pipeWorkerToWorkerRequestIndicator.thePipe.lastRead.size == 0)
  { this->PauseIndicatorPipeInUse.ResumePausedProcessesIfAny(false, false);
    return;
  }
  if (theGlobalVariables.flagTimedOutComputationIsDone)
  { this->PauseIndicatorPipeInUse.ResumePausedProcessesIfAny(false, false);
    return;
  }
  this->pipeWorkerToWorkerIndicatorData.WriteAfterEmptying(input, false, false);
  this->PauseIndicatorPipeInUse.ResumePausedProcessesIfAny(false, false);
}

int WebWorker::ProcessFolder()
{ MacroRegisterFunctionWithName("WebWorker::ProcessFolder");
  this->SetHeaderOKNoContentLength("");
  std::stringstream out;
  out << "<html><body>";
  if (this->RelativePhysicalFileNamE.size() > 0)
    if (this->RelativePhysicalFileNamE[this->RelativePhysicalFileNamE.size() - 1] != '/')
      this->RelativePhysicalFileNamE.push_back('/');
  if (this->addressGetOrPost.size() > 0)
    if (this->addressGetOrPost[this->addressGetOrPost.size() - 1] != '/')
      this->addressGetOrPost.push_back('/');
  if (this->flagFileNameSanitized)
  { out << "<hr>The virtual file name I extracted was: " << this->VirtualFileName
    << "<br>However, I do not allow folder names that contain dots. "
    << " Therefore I have sanitized the main address to: " << this->RelativePhysicalFileNamE;
  }
  List<std::string> theFileNames, theFileTypes;
  if (!FileOperations::GetFolderFileNamesUnsecure(this->RelativePhysicalFileNamE, theFileNames, &theFileTypes))
  { out << "<b>Failed to open directory with physical address "
    << this->RelativePhysicalFileNamE << " </b></body></html>";
    stOutput << out.str();
    return 0;
  }
  out << "Browsing folder: " << this->addressGetOrPost << "<br>Virtual name: "
  << this->VirtualFileName << "<hr>";
  List<std::string> theFolderNamesHtml, theFileNamesHtml;
  for (int i = 0; i < theFileNames.size; i ++)
  { std::stringstream currentStream;
    bool isDir = (theFileTypes[i] == ".d");
    //logWorker << logger::red << "Current file name: " << HtmlRoutines::ConvertStringToURLString(theFileNames[i]) << logger::endL;
    currentStream << "<a href=\"" << this->addressGetOrPost << HtmlRoutines::ConvertStringToURLString(theFileNames[i], false);
    if (isDir)
      currentStream << "/";
    currentStream << "\">" << theFileNames[i];
    if (isDir)
      currentStream << "/";
    currentStream << "</a><br>";
    if (isDir)
      theFolderNamesHtml.AddOnTop(currentStream.str());
    else
      theFileNamesHtml.AddOnTop(currentStream.str());
  }
  theFolderNamesHtml.QuickSortAscending();
  theFileNamesHtml.QuickSortAscending();
  for (int i = 0; i < theFolderNamesHtml.size; i ++)
    out << theFolderNamesHtml[i];
  for (int i = 0; i < theFileNamesHtml.size; i ++)
    out << theFileNamesHtml[i];
  out << "\n</body></html>";
  stOutput << out.str();
  return 0;
}

int WebWorker::ProcessFile()
{ MacroRegisterFunctionWithName("WebWorker::ProcessFile");
  if (!FileOperations::FileExistsUnsecure(this->RelativePhysicalFileNamE))
  { this->SetHeadeR("HTTP/1.0 404 Object not found", "Content-Type: text/html");
    stOutput << "<html>"
    << HtmlRoutines::GetCSSLinkCalculator()
    << "<body>";
    stOutput << "One page <a href = \"" << theGlobalVariables.DisplayNameExecutableApp << "\">app</a>. ";
    stOutput << " Same app without browser cache: <a href = \"" << theGlobalVariables.DisplayNameExecutableAppNoCache << "\">app no cache</a>.<hr>";
    stOutput << "<b>File does not exist.</b>";
    if (this->flagFileNameSanitized)
    { stOutput << "<hr>You requested virtual file: " << this->VirtualFileName
      << "<br>However, I do not allow addresses that contain dots (to avoid access to folders below the server). "
      << " Therefore I have sanitized the address to a relative physical address: " << this->RelativePhysicalFileNamE;
    }
    stOutput << "<br><b> Address:</b> "
    << HtmlRoutines::ConvertStringToHtmlString(this->addressGetOrPost, true) << "<br><b>Virtual file name:</b> "
    << HtmlRoutines::ConvertStringToHtmlString(this->VirtualFileName, true)
    << "<br><b>Computed relative physical file name:</b> "
    << HtmlRoutines::ConvertStringToHtmlString(this->RelativePhysicalFileNamE, true);
    stOutput << "<br><b> Request:</b> " << theGlobalVariables.userCalculatorRequestType;
    stOutput << "<hr><hr><hr>Message details:<br>"
    << this->ToStringMessageUnsafe()
    << this->ToStringMessageFullUnsafe();
    stOutput << "</body></html>";
    this->flagEncounteredErrorWhileServingFile = true;
    return 0;
  }
  std::string fileExtension = FileOperations::GetFileExtensionWithDot(this->RelativePhysicalFileNamE);
  bool isBinary = this->IsFileExtensionOfBinaryFile(fileExtension);
  std::fstream theFile;
  if (!FileOperations::OpenFileUnsecure(theFile, this->RelativePhysicalFileNamE, false, false, !isBinary))
  { this->SetHeaderOKNoContentLength("");
    stOutput << "<html><body><b>Error: file appears to exist but I could not open it.</b> ";
    if (this->flagFileNameSanitized)
    { stOutput << "<hr>You requested virtual file: " << this->VirtualFileName
      << "<br>However, I do not allow addresses that contain dots. "
      << " Therefore I have sanitized the address to a relative physical address: " << this->RelativePhysicalFileNamE;
    }
    stOutput << "<br><b> File display name: </b>"
    << this->addressGetOrPost << "<br><b>Virtual file name: </b>"
    << this->VirtualFileName << "</body></html>";
    return 0;
  }
  theFile.seekp(0, std::ifstream::end);
  unsigned int fileSize = theFile.tellp();
  if (fileSize > 100000000)
  { this->SetHeadeR("HTTP/1.0 413 Payload Too Large", "");
    stOutput << "<html><body><b>Error: user requested file: "
    << this->VirtualFileName
    << " but it is too large, namely, " << fileSize
    << " bytes.</b></body></html>";
    return 0;
  }
  std::stringstream theHeader;
  bool withCacheHeader = false;
  theHeader << "HTTP/1.0 200 OK\r\n"
  << this->GetMIMEtypeFromFileExtension(fileExtension)
  << "Access-Control-Allow-Origin: *\r\n";
  for (int i = 0; i < this->parent->addressStartsSentWithCacheMaxAge.size; i ++)
    if (MathRoutines::StringBeginsWith(this->VirtualFileName, this->parent->addressStartsSentWithCacheMaxAge[i]))
    { theHeader << WebAPI::HeaderCacheControl << "\r\n";
      withCacheHeader = true;
      break;
    }
  std::stringstream debugBytesStream;
  if (theGlobalVariables.UserDebugFlagOn() && fileExtension == ".html")
  { debugBytesStream << "<!-- DEBUG info: ";
    if (withCacheHeader)
      debugBytesStream << "Sent with Cache-Control header.\n ";
    debugBytesStream << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable()
    << "-->";
  }
  if (theGlobalVariables.UserDebugFlagOn() && fileExtension == ".appcache")
  { debugBytesStream << "#" << this->GetMIMEtypeFromFileExtension(fileExtension);
  }
  if (theGlobalVariables.flagRunningApache)
  { this->SetHeaderOKNoContentLength("");
    stOutput << "<html><body>"
    << "<a href=\"" << this->VirtualFileName << "\">" << this->VirtualFileName << "</a>"
    << "</body></html>";
    stOutput << debugBytesStream.str();
    return 0;
  }
  unsigned int totalLength = fileSize + debugBytesStream.str().size();
  theHeader << "Content-length: " << totalLength << "\r\n";
  if (!this->flagKeepAlive)
    theHeader << this->GetHeaderConnectionClose() << "\r\n";
  //std::string theCookie = this->GetHeaderSetCookie();
  //if (theCookie != "")
  //  theHeader << theCookie << "\r\n";
  theHeader << "\r\n";
  this->QueueStringForSendingNoHeadeR(theHeader.str());
  if (this->requestTypE == this->requestHead)
  { this->SendAllBytesNoHeaders();
    this->flagAllBytesSentUsingFile = true;
    this->flagEncounteredErrorWhileServingFile = false;
    return 0;
  }
  const int bufferSize = 64*1024;
  this->bufferFileIO.SetSize(bufferSize);
  theFile.seekg(0);
  theFile.read(&this->bufferFileIO[0], this->bufferFileIO.size);
  int numBytesRead = theFile.gcount();
  while (numBytesRead != 0)
  { this->bufferFileIO.SetSize(numBytesRead);
    this->QueueBytesForSendingNoHeadeR(this->bufferFileIO);
    this->bufferFileIO.SetSize(bufferSize);
    theFile.read(this->bufferFileIO.TheObjects, this->bufferFileIO.size);
    numBytesRead = theFile.gcount();
  }
  stOutput << debugBytesStream.str();
  this->SendAllBytesNoHeaders();
  this->flagAllBytesSentUsingFile = true;
  this->flagEncounteredErrorWhileServingFile = false;
  return 0;
}

void WebWorker::reset()
{ this->connectedSocketID = - 1;
  this->ProcessPID = - 1;
  this->connectedSocketIDLastValueBeforeRelease = - 1;
  this->connectionID = - 1;
  this->indexInParent = - 1;
  this->millisecondOffset = - 1;
  this->parent = 0;
  this->indentationLevelHTML = 0;
  this->displayUserInput = "";
  this->requestTypE = this->requestUnknown;
  this->flagToggleMonitoring = false;
  this->flagDoAddContentLength = false;
  this->flagFileNameSanitized = false;
  this->timeOfLastPingServerSideOnly = - 1;
  this->timeServerAtWorkerStart = - 1;
  this->flagFoundMalformedFormInput = false;
  this->flagProgressReportAllowed = false;
  this->flagKeepAlive = false;
  this->flagDidSendAll = false;
  this->flagUsingSSLInWorkerProcess = false;
  this->flagAllBytesSentUsingFile = false;
  this->flagEncounteredErrorWhileServingFile = false;
  theGlobalVariables.flagUsingSSLinCurrentConnection = false;
  theGlobalVariables.flagLoggedIn = false;
  theGlobalVariables.userDefault.reset();
  this->RelativePhysicalFileNamE = "";
  this->numberOfReceivesCurrentConnection = 0;
  this->Release();
}

WebWorker::WebWorker()
{ this->flagDeallocated = false;
  this->reset();
}

bool WebWorker::IamActive()
{ if (this->parent == 0 || this->indexInParent == - 1)
    return false;
  return this->parent->activeWorker == this->indexInParent;
}

bool WebWorker::IsFileExtensionOfBinaryFile(const std::string& fileExtension)
{ if (fileExtension == ".png")
    return true;
  if (fileExtension == ".mp4")
    return true;
  if (fileExtension == ".mpeg")
    return true;
  if (fileExtension == ".jpg")
    return true;
  return false;
}

void WebWorker::WrapUpConnection()
{ MacroRegisterFunctionWithName("WebWorker::WrapUpConnection");
  if (theGlobalVariables.flagServerDetailedLog)
    logIO << "DEBUG: wrapping up connection. " << logger::endL;
  this->resultWork["connectionsServed"] = this->numberOfReceivesCurrentConnection;
  if (this->flagToggleMonitoring)
    this->resultWork["result"] = "toggleMonitoring";
  else
    this->resultWork["result"] = "close";
  this->pipeWorkerToServerControls.WriteAfterEmptying(this->resultWork.ToString(false), false, false);
  if (theGlobalVariables.flagServerDetailedLog)
    logIO << "DEBUG: done with pipes, releasing resources. " << logger::endL;
  this->Release();
  if (theGlobalVariables.flagServerDetailedLog)
    logIO << "DEBUG: released. " << logger::endL;
  theGlobalVariables.flagComputationCompletE = true;
  theGlobalVariables.flagComputationFinishedAllOutputSentClosing = true;
}

void WebWorker::SendAllAndWrapUp()
{ MacroRegisterFunctionWithName("WebWorker::SendAllAndWrapUp");
  if (!this->IamActive())
  { logWorker << logger::red << "Signal done called on non-active worker" << logger::endL;
    return;
  }
  this->SendAllBytesWithHeaders();
  this->WrapUpConnection();
}

WebWorker::~WebWorker()
{ //Workers are not allowed to release resources in the destructor:
  //a Worker's destructor is called when expanding List<WebWorker>.
  this->flagDeallocated = true;
}

std::string WebWorker::GetMIMEtypeFromFileExtension(const std::string& fileExtension)
{ MacroRegisterFunctionWithName("WebWorker::GetMIMEtypeFromFileExtension");
  if (fileExtension == ".html")
    return "Content-Type: text/html\r\n";
  if (fileExtension == ".txt")
    return "Content-Type: text/plain\r\n";
  if (fileExtension == ".png")
    return "Content-Type: image/png\r\n";
  if (fileExtension == ".js")
    return "Content-Type: text/javascript\r\n";
  if (fileExtension == ".ico")
    return "Content-Type: image/x-icon\r\n";
  if (fileExtension == ".css")
    return "Content-Type: text/css\r\n";
  if (fileExtension == ".eot")
    return "Content-Type: application/vnd.ms-fontobject\r\n";
  if (fileExtension == ".ttf")
    return "Content-Type: application/octet-stream\r\n";
  if (fileExtension == ".svg")
    return "Content-Type: image/svg+xml\r\n";
  if (fileExtension == ".appcache")
    return "Content-Type: text/cache-manifest\r\n";
  if (fileExtension == ".woff")
    return "Content-Type: application/font-woff\r\n";
  return "Content-Type: application/octet-stream\r\n";
}

int WebWorker::ProcessUnknown()
{ MacroRegisterFunctionWithName("WebWorker::ProcessUnknown");
  this->SetHeadeR("HTTP/1.0 501 Method Not Implemented", "Content-Type: text/html");
  stOutput << "<html><head>"
  << HtmlRoutines::GetCSSLinkCalculator()
  << "</head>";
  stOutput << "<body>";
  stOutput << "<calculatorNavigation>" << theGlobalVariables.ToStringNavigation()
  << "</calculatorNavigation>\n";
  stOutput << "<b>Requested method is not implemented. </b> "
  << "<hr>The original message received from the server follows."
  << "<hr>\n" << this->ToStringMessageUnsafe();

  if (this->requestTypE == this->requestChunked)
    logHttpErrors << logger::red << "Chunked messaging not implemented. Message follows. " << logger::endL
    << this->ToStringMessageFullUnsafe() << logger::endL;
  else
    logHttpErrors << logger::red << "Method not implemented. Message follows. " << logger::endL
    << this->ToStringMessageFullUnsafe(false) << logger::endL;
  stOutput << "</body></html>";
  return 0;
}

bool WebWorker::ShouldDisplayLoginPage()
{ if (theGlobalVariables.userCalculatorRequestType == "login")
    return true;
  if (theGlobalVariables.flagUsingSSLinCurrentConnection && !theGlobalVariables.flagLoggedIn &&
      theGlobalVariables.userCalculatorRequestType != "calculator")
    return true;
  if (theGlobalVariables.UserRequestMustBePromptedToLogInIfNotLoggedIn() && !theGlobalVariables.flagLoggedIn)
    return true;
  return false;
}

std::string WebWorker::GetAuthenticationToken(const std::string& reasonForNoAuthentication)
{ MacroRegisterFunctionWithName("WebWorker::GetAuthenticationToken");
  if (theGlobalVariables.flagLoggedIn && theGlobalVariables.flagUsingSSLinCurrentConnection)
    return theGlobalVariables.userDefault.actualActivationToken;
  return reasonForNoAuthentication;
}

std::string WebWorker::GetChangePasswordPagePartOne(bool& outputDoShowPasswordChangeField)
{ MacroRegisterFunctionWithName("WebWorker::GetChangePasswordPagePartOne");
  std::stringstream out;
  std::string claimedActivationToken =
  HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("activationToken"), false);
  std::string claimedEmail =
  HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("email"), false);
  out << "<input type =\"hidden\" id =\"activationToken\" value =\""
  << claimedActivationToken << "\">";
  if (claimedActivationToken == "")
  { out << "Activation token is empty. ";
    return out.str();
  }
  if (claimedEmail == "")
  { out << "Activation email is empty. ";
    return out.str();
  }
  std::string actualEmailActivationToken, usernameAssociatedWithToken;
  if (theGlobalVariables.userDefault.email == claimedEmail)
  { out << "\n<b><span style =\"color:green\">Email " << claimedEmail << " updated. </span></b>";
    return out.str();
  }
  JSData findEmail, emailInfo, findUser, userInfo;
  findEmail[DatabaseStrings::labelEmail] = claimedEmail;
  if (!DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSON(DatabaseStrings::tableEmailInfo, findEmail, emailInfo, &out, true))
  { out << "\n<span style =\"color:red\"><b>Failed to fetch email activation token for email: "
    << claimedEmail << " </b></span>";
    return out.str();
  }
  usernameAssociatedWithToken = emailInfo[DatabaseStrings::labelUsernameAssociatedWithToken].string;
  actualEmailActivationToken = emailInfo[DatabaseStrings::labelActivationToken].string;
  if (actualEmailActivationToken != claimedActivationToken)
  { out << "\n<span style =\"color:red\"><b>Bad activation token. Could not activate your email. </b></span>";
    //out << "DEBUG: actual token: " << actualEmailActivationToken << ". Claimed token: " << claimedActivationToken;
    return out.str();
  }
  if (usernameAssociatedWithToken != theGlobalVariables.userDefault.username)
  { out << "\n<span style =\"color:red\"><b>Activation token was issued for another user. </b></span>";
    return out.str();
  }
  emailInfo[DatabaseStrings::labelActivationToken] = "";
  if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON
       (DatabaseStrings::tableEmailInfo, findEmail, emailInfo, 0, &out))
  { out << "\n<span style =\"color:red\"><b>Could not reset the activation token (database is down?). </b></span>";
    return out.str();
  }
  userInfo[DatabaseStrings::labelEmail] = claimedEmail;
  findUser[DatabaseStrings::labelUsername] = theGlobalVariables.userDefault.username;
  if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON
       (DatabaseStrings::tableUsers, findUser, userInfo, 0, &out))
  { out << "\n<span style =\"color:red\"><b>Could not store your email (database is down?). </b></span>";
    return out.str();
  }
  theGlobalVariables.userDefault.email = claimedEmail;
  out << "\n<span style =\"color:green\"><b>Email successfully updated. </b></span>";
  if (theGlobalVariables.userDefault.actualActivationToken != "" &&
      theGlobalVariables.userDefault.actualActivationToken != "activated" &&
      theGlobalVariables.userDefault.actualHashedSaltedPassword != "")
  { out << "<br>It appears your password is already set. "
    << "<br>If you'd like to change it using your old password, "
    << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=changePasswordPage\">click here</a>. ";
    outputDoShowPasswordChangeField = false;
    userInfo[DatabaseStrings::labelActivationToken] = "activated";
  } else
  { out << "<br>"
    << "<span style =\"color:orange\">To fully activate your account, please choose a password.</span></b>";
  }
  return out.str();
}

std::string WebWorker::GetChangePasswordPage()
{ MacroRegisterFunctionWithName("WebWorker::GetChangePasswordPage");
  std::stringstream out;
  out << "<html>";
  out << "<head>";
  out << HtmlRoutines::GetCSSLinkCalculator();
  out << "</head>";
  out << "<body> ";
#ifdef MACRO_use_MongoDB
  out << "<calculatorNavigation>" << HtmlInterpretation::ToStringNavigation() << "</calculatorNavigation>";
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
  { out << "Changing password requires secure connection. ";
    out << "</body></html>";
    return out.str();
  }
  theWebServer.CheckExecutableVersionAndRestartIfNeeded(true);
  //out << "<form name =\"login\" id =\"login\" action =\"calculator\" method =\"GET\" accept-charset =\"utf-8\">";
  out
  << "User: " << theGlobalVariables.userDefault.username
  << "<input type =\"hidden\" id =\"username\" placeholder =\"username\" "
  << "value =\"" << theGlobalVariables.userDefault.username << "\" "
  << "required>";
  bool doShowPasswordChangeField = true;
  out << "<table>";
  JSData findUser, updateUser;
  if (theGlobalVariables.userDefault.flagUserHasActivationToken)
    out << "<tr><td colspan =\"2\">"
    << "<b><span style =\"color:orange\">"
    << "To fully activate your account, "
    << "please choose a password.</span></b></td></tr>";
  if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::activateAccount)
    out << "<tr><td colspan =\"2\">" << WebWorker::GetChangePasswordPagePartOne(doShowPasswordChangeField) << "</td></tr>";
  else
  { if (theGlobalVariables.userDefault.actualActivationToken != "" &&
        theGlobalVariables.userDefault.actualActivationToken != "activated" &&
        theGlobalVariables.userDefault.actualHashedSaltedPassword != "" &&
        theGlobalVariables.userCalculatorRequestType == "changePasswordPage")
    { out << "<tr><td colspan =\"2\">";
      out << "You already have a password but your account was marked as not activated "
      << "- probably by an admin/instructor. ";
      findUser[DatabaseStrings::labelUsername] = theGlobalVariables.userDefault.username;
      updateUser[DatabaseStrings::labelActivationToken] = "activated";
      if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON
           (DatabaseStrings::tableUsers, findUser, updateUser, 0, &out))
        out << " <span style =\"color:red\"><b>Failed to activate your account. </b></span>";
      else
        out << " <span style =\"color:green\"><b>Your account is now marked as activated.</b></span>";
      out << "</td></tr>";
    }
    out
    << "<tr><td>"
    << "Old password:\n "
    << "</td>"
    << "<td>"
    << "<input type =\"password\" id =\"password\" placeholder =\"password\">\n"
    << "</td></tr>";
  }
  if (doShowPasswordChangeField)
  { out
    << "<tr>"
    << "<td>"
    << "New password:\n"
    << "</td>"
    << "<td>"
    << "<input type =\"password\" id =\"newPassword\" placeholder =\"new password\">\n"
    << "</td></tr>"
    << "<tr><td>"
    << "Re-enter new password: \n"
    << "</td><td>"
    << "<input type =\"password\"  id =\"reenteredPassword\" placeholder =\"re-enter\" "
    << "onkeyup =\"if (event.keyCode == 13) submitChangePassRequest('passwordChangeResult');\">\n"
    << "</td></tr>";
  }
  out
  << "</table>";
  if (doShowPasswordChangeField)
  { out << "<button  onclick=\"submitChangePassRequest('passwordChangeResult') \"> Submit</button>\n"
    << "<span id =\"passwordChangeResult\"> </span>\n";
    out << "<hr>"
    << "Email: " << theGlobalVariables.userDefault.email
    << "<br>\n"
    << "Change/set email: <input type =\"email\" id =\"emailInputID\">\n "
    << "<button onclick=\"submitChangePassRequest('emailChangeResult') \"> Submit</button>\n";
    out << "<span id =\"emailChangeResult\"></span>\n";
  }
  out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
#else
  out << "Database not available";
#endif //MACRO_use_MongoDB
  out << "</body></html>";
  return out.str();
}

bool WebWorker::DoSetEmail
(UserCalculatorData& inputOutputUser, std::stringstream* commentsOnFailure, std::stringstream* commentsGeneralNonSensitive,
 std::stringstream* commentsGeneralSensitive)
{ MacroRegisterFunctionWithName("WebWorker::DoSetEmail");
#ifdef MACRO_use_MongoDB
  EmailRoutines theEmail;
  theEmail.toEmail = inputOutputUser.email;
  if (!theEmail.IsOKEmail(theEmail.toEmail, commentsOnFailure))
    return false;
  //out << "DEBUG: got to here part 2. ";
  UserCalculator userCopy;
  userCopy.UserCalculatorData::operator=(inputOutputUser);
  userCopy.email = inputOutputUser.email;
  if (!userCopy.ComputeAndStoreActivationEmailAndTokens(commentsOnFailure, commentsGeneralNonSensitive))
    return false;
  theEmail.emailContent = userCopy.activationEmail;
  theEmail.subject = userCopy.activationEmailSubject;
  if (commentsGeneralNonSensitive != 0)
    *commentsGeneralNonSensitive << "<br><b>Sending email... </b>";
  theEmail.SendEmailWithMailGun(commentsOnFailure, commentsGeneralNonSensitive, commentsGeneralSensitive);
  if (commentsGeneralSensitive != 0)
  { if (theGlobalVariables.UserDefaultHasAdminRights())
      *commentsGeneralSensitive << "<hr>Content of sent email (admin view only):<br>"
      << HtmlRoutines::ConvertStringToHtmlString(theEmail.emailContent, true);
  } else
  { if (commentsGeneralNonSensitive != 0)
      *commentsGeneralNonSensitive << "Email content not displayed. ";
  }
  userCopy.clearAuthenticationTokenAndPassword();
  userCopy.actualActivationToken = "";
  inputOutputUser = userCopy;
  return true;
#else
  (void) inputOutputUser;
  (void) commentsOnFailure;
  (void) commentsGeneralNonSensitive;
  (void) commentsGeneralSensitive;

  return false; //MACRO_use_MongoDB
#endif
}

int WebWorker::SetEmail(const std::string& input)
{ MacroRegisterFunctionWithName("WebWorker::SetEmail");
  (void) input;
#ifdef MACRO_use_MongoDB
  std::stringstream out, debugStream;
  //double startTime = theGlobalVariables.GetElapsedSeconds();
  theGlobalVariables.userDefault.email = input;
  std::stringstream* adminOutputStream = 0;
  if (theGlobalVariables.UserDefaultHasAdminRights())
    adminOutputStream = &out;
  this->DoSetEmail(theGlobalVariables.userDefault, &out, &out, adminOutputStream);
  stOutput << out.str();
  if (theGlobalVariables.UserDefaultHasAdminRights())
    stOutput << "<hr><b>Admin view only. </b>" << debugStream.str();
  stOutput << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds() << " second(s).";
#else
  stOutput << "<b>Error: database not available.</b>";
#endif
  return 0;
}

int WebWorker::ProcessChangePassword(const std::string& reasonForNoAuthentication)
{ MacroRegisterFunctionWithName("WebWorker::ProcessChangePassword");
  (void) reasonForNoAuthentication;
  //stOutput << " ere i am";
  //if (theGlobalVariables.UserDebugFlagOn())
  //theGlobalVariables.SetWebInpuT("debugFlag", "true");
  //stOutput << "DEBUG: " << this->ToStringCalculatorArgumentsHumanReadable();
  this->SetHeaderOKNoContentLength("");
#ifdef MACRO_use_MongoDB
  UserCalculatorData& theUser = theGlobalVariables.userDefault;
  theUser.enteredAuthenticationToken = "";
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
  { stOutput << "<b style =\"color:red\">Please use secure connection.</b>";
    return 0;
  }
  if (!theGlobalVariables.flagLoggedIn)
  { stOutput << "<b style =\"color:red\">Please enter (old) password.</b> " << reasonForNoAuthentication;
    return 0;
  }
  std::string newPassword =
  HtmlRoutines::ConvertStringToURLString
  (HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("newPassword"), true), false);
  //<-Passwords are ONE-LAYER url-encoded
  //<-INCOMING pluses in passwords MUST be decoded as spaces, this is how form.submit() works!
  //<-Incoming pluses must be re-coded as spaces (%20).

  std::string reenteredPassword =
  HtmlRoutines::ConvertStringToURLString
  (HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("reenteredPassword"), true), false);
  //<-Passwords are ONE-LAYER url-encoded
  //<-INCOMING pluses in passwords MUST be decoded as spaces, this is how form.submit() works!
  //<-Incoming pluses must be re-coded as spaces (%20).

  std::string newEmail = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("email"), false);
  if (newEmail != "")
  { JSData queryEmailTaken, notUsed;
    queryEmailTaken[DatabaseStrings::labelEmail] = newEmail;
    if (DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSON(DatabaseStrings::tableUsers, queryEmailTaken, notUsed, 0, true))
    { stOutput << "<b style =\"color:red\">It appears the email is already taken. </b>";
      return 0;
    }
  }
  if (newPassword == "" && reenteredPassword == "" && newEmail != "")
    return this->SetEmail(newEmail);
  if (newPassword != reenteredPassword)
  { stOutput << "<b style =\"color:red\">Passwords don't match. </b>";
    return 0;
  }
  std::stringstream commentsOnFailure;
  std::string newAuthenticationToken;
  if (!DatabaseRoutinesGlobalFunctions::SetPassword
       (theUser.username, newPassword, newAuthenticationToken, commentsOnFailure))
  { stOutput << "<span style =\"color:red\"><b>" << commentsOnFailure.str() << "</b></span>";
    return 0;
  }
  JSData findQuery, setQuery;
  findQuery[DatabaseStrings::labelUsername] = theUser.username;
  setQuery[DatabaseStrings::labelActivationToken] = "activated";
  if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON
       (DatabaseStrings::tableUsers, findQuery, setQuery, 0, &commentsOnFailure))
    stOutput << "<span style =\"color:red\"><b>Failed to set activationToken: "
    << commentsOnFailure.str() << "</b></span>";

  stOutput << "<span style =\"color:green\"> <b>Password change successful. </b></span>";
  //stOutput << "DEBUG: computed sha1salted, entered: " << theUser.actualShaonedSaltedPassword
  //<< ", " << theUser.enteredShaonedSaltedPassword;
  if (theGlobalVariables.GetWebInput("doReload") != "false")
  { stOutput
    << "<meta http-equiv=\"refresh\" content =\"0; url ='"
    << theGlobalVariables.DisplayNameExecutable  << "?request=logout"
    << "&username="
    << HtmlRoutines::ConvertStringToURLString(theGlobalVariables.userDefault.username, false)
    << "&activationToken = &authenticationToken = &"
    << "'\" />";
  }
#else
  stOutput << "Operation not possible: project compiled without database support.";
#endif
  return 0;
}

int WebWorker::ProcessCompute()
{ MacroRegisterFunctionWithName("WebWorker::ProcessCompute");
  this->SetHeaderOKNoContentLength("");
//  theParser.initComputationStats();
  theParser->inputString = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("mainInput"), false);
  theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection = WebServer::ReturnActiveIndicatorAlthoughComputationIsNotDone;
  theGlobalVariables.initOutputReportAndCrashFileNames
  (HtmlRoutines::ConvertStringToURLString(theParser->inputString, false), theParser->inputString);

  //std::cout << "DEBUG: input string: " << theParser.inputString;
  //std::cout.flush();
  ////////////////////////////////////////////////
  //  the initialization below moved to the start of the web server!
  //  theParser.init();
  ////////////////////////////////////////////////
  this->flagProgressReportAllowed = true;
  theParser->Evaluate(theParser->inputString);
  this->flagProgressReportAllowed = false;
  if (theGlobalVariables.flagRunningBuiltInWebServer)
    if (theGlobalVariables.flagOutputTimedOut)
    { this->OutputResultAfterTimeout();
      return 0;
    }
  JSData result;
  result = theParser->ToJSONOutputAndSpecials();
  if (theGlobalVariables.UserDebugFlagOn())
    result["debug"] = this->ToStringMessageFullUnsafe();

  stOutput << result.ToString(false);
  theGlobalVariables.flagComputationCompletE = true;
  return 0;
}

int WebWorker::ProcessActivateAccount()
{ MacroRegisterFunctionWithName("WebWorker::ProcessActivateAccount");
  this->SetHeaderOKNoContentLength("");
  bool doShowDetails = true;
  stOutput << WebWorker::GetChangePasswordPagePartOne(doShowDetails);
  return 0;
}

int WebWorker::ProcessChangePasswordPage()
{ MacroRegisterFunctionWithName("WebWorker::ProcessChangePasswordPage");
  this->SetHeaderOKNoContentLength("");
  stOutput << this->GetChangePasswordPage();
  return 0;
}

int WebWorker::ProcessForgotLoginPage()
{ MacroRegisterFunctionWithName("WebWorker::ProcessForgotLoginPage");
  this->SetHeaderOKNoContentLength("");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
  { DatabaseRoutinesGlobalFunctions::LogoutViaDatabase();
    theGlobalVariables.userDefault.clearAuthenticationTokenAndPassword();
  }
  stOutput << this->GetForgotLoginPage();
  return 0;
}

int WebWorker::ProcessLogout()
{ MacroRegisterFunctionWithName("WebWorker::ProcessLogout");
  this->SetHeaderOKNoContentLength("");
  DatabaseRoutinesGlobalFunctions::LogoutViaDatabase();
  return this->ProcessLoginUserInfo("Coming from logout");
}

int WebWorker::ProcessSelectCourseJSON()
{ MacroRegisterFunctionWithName("WebWorker::ProcessSelectCourseJSON");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetSelectCourseJSON();
  return 0;
}

int WebWorker::ProcessTopicListJSON()
{ MacroRegisterFunctionWithName("WebWorker::ProcessTopicListJSON");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetTopicTableJSON();
  return 0;
}

int WebWorker::ProcessCalculatorOnePageJS(bool appendBuildHash)
{ MacroRegisterFunctionWithName("WebWorker::ProcessCalculatorOnePageJS");
  if (appendBuildHash)
    this->SetHeaderOKNoContentLength(WebAPI::HeaderCacheControl, "text/javascript");
  else
    this->SetHeaderOKNoContentLength("", "text/javascript");
  stOutput << HtmlInterpretation::GetOnePageJS(appendBuildHash);
  return 0;
}

int WebWorker::ProcessApp(bool appendBuildHash)
{ MacroRegisterFunctionWithName("WebWorker::ProcessApp");
  this->SetHeaderOKNoContentLength("");
  theWebServer.CheckExecutableVersionAndRestartIfNeeded(true);
  stOutput << HtmlInterpretation::GetApp(appendBuildHash);
  return 0;
}

int WebWorker::ProcessAbout()
{ MacroRegisterFunctionWithName("WebWorker::ProcessAbout");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetAboutPage();
  return 0;
}

int WebWorker::ProcessTemplate()
{ MacroRegisterFunctionWithName("WebWorker::ProcessTemplate");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetPageFromTemplate();
  //if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
  //  stOutput << "<!--" << this->ToStringMessageFullUnsafe() << "-->";
  return 0;
}

int WebWorker::ProcessLoginUserInfo(const std::string& comments)
{ MacroRegisterFunctionWithName("WebWorker::ProcessUserInfoJSON");
  this->SetHeaderOKNoContentLength("");
  theWebServer.CheckExecutableVersionAndRestartIfNeeded(true);
  stOutput << HtmlInterpretation::GetJSONUserInfo(comments);
  //if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
  //  stOutput << "<!--" << this->ToStringMessageFullUnsafe() << "-->";
  return 0;
}

int WebWorker::ProcessTemplateJSON()
{ MacroRegisterFunctionWithName("WebWorker::ProcessTemplateJSON");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetJSONFromTemplate();
  //if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights())
  //  stOutput << "<!--" << this->ToStringMessageFullUnsafe() << "-->";
  return 0;
}

int WebWorker::ProcessExamPageJSON()
{ MacroRegisterFunctionWithName("WebWorker::ProcessExamPageJSON");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetExamPageJSON();
  return 0;
}

int WebWorker::ProcessNavigation()
{ MacroRegisterFunctionWithName("WebWorker::ProcessNavigation");
  this->SetHeaderOKNoContentLength("");
  stOutput << this->GetNavigationPage();
  return 0;
}

int WebWorker::ProcessGetAuthenticationToken(const std::string& reasonForNoAuthentication)
{ MacroRegisterFunctionWithName("WebWorker::ProcessGetAuthenticationToken");
  this->SetHeaderOKNoContentLength("");
  stOutput << this->GetAuthenticationToken(reasonForNoAuthentication);
  return 0;
}

int WebWorker::ProcessSetProblemDatabaseInfo()
{ MacroRegisterFunctionWithName("WebWorker::ProcessSetProblemDatabaseInfo");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetSetProblemDatabaseInfoHtml();
  return 0;
}

int WebWorker::ProcessAddUserEmails()
{ MacroRegisterFunctionWithName("WebWorker::ProcessAddUserEmails");
  this->SetHeaderOKNoContentLength("");
  stOutput << this->GetAddUserEmails();
  return 0;
}

int WebWorker::ProcessModifyPage()
{ MacroRegisterFunctionWithName("WebWorker::ProcessModifyPage");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::ModifyProblemReport();
  return 0;
}

int WebWorker::ProcessAssignTeacherToSection()
{ MacroRegisterFunctionWithName("WebWorker::ProcessAssignTeacherToSection");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::AddTeachersSections();
  return 0;
}

int WebWorker::ProcessScores()
{ MacroRegisterFunctionWithName("WebWorker::ProcessScores");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetScoresPage();
  return 0;
}

int WebWorker::ProcessScoresInCoursePage()
{ MacroRegisterFunctionWithName("WebWorker::ProcessScoresInCoursePage");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetScoresInCoursePage();
  return 0;
}

int WebWorker::ProcessAccounts()
{ MacroRegisterFunctionWithName("WebWorker::ProcessAccounts");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetAccountsPage(this->hostWithPort);
  return 0;
}

int WebWorker::ProcessAccountsJSON()
{ MacroRegisterFunctionWithName("WebWorker::ProcessAccountsJSON");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetAccountsPageJSON(this->hostWithPort);
  return 0;
}

int WebWorker::ProcessDatabaseJSON()
{ MacroRegisterFunctionWithName("WebWorker::ProcessDatabaseJSON");
  this->SetHeaderOKNoContentLength("");
  stOutput << this->GetDatabaseJSON();
  return 0;
}

int WebWorker::ProcessDatabaseDeleteEntry()
{ MacroRegisterFunctionWithName("WebWorker::ProcessDatabaseDeleteEntry");
  this->SetHeaderOKNoContentLength("");
  stOutput << this->GetDatabaseDeleteOneItem();
  return 0;
}

int WebWorker::ProcessDatabaseModifyEntry()
{ MacroRegisterFunctionWithName("WebWorker::ProcessDatabaseModifyEntry");
  this->SetHeaderOKNoContentLength("");
  stOutput << "Not implemented yet";
  return 0;
}

int WebWorker::ProcessBrowseProblems()
{ MacroRegisterFunctionWithName("WebWorker::ProcessBrowseProblems");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetBrowseProblems();
  return 0;
}

int WebWorker::ProcessEditPageJSON()
{ MacroRegisterFunctionWithName("WebWorker::ProcessEditPageJSON");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetEditPageJSON();
  return 0;
}

#include "vpfHeader2Math10_LaTeXRoutines.h"
int WebWorker::ProcessSlidesOrHomeworkFromSource()
{ MacroRegisterFunctionWithName("WebWorker::ProcessSlidesOrHomeworkFromSource");
  this->SetHeaderOKNoContentLength("");
  LaTeXcrawler theCrawler;
  for (int i = 0; i < theGlobalVariables.webArguments.size(); i ++)
  { std::string theKey = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.webArguments.theKeys[i], false);
    //stOutput << "DEBUG: considering key: " << theKey
    //<< " with value: " << theGlobalVariables.webArguments.theValues[i] << "<br>";
    if (theKey != "fileName" && MathRoutines::StringBeginsWith(theKey, "file"))
    { theCrawler.slideFileNamesVirtualWithPatH.AddOnTop
      (MathRoutines::StringTrimWhiteSpace
       (HtmlRoutines::ConvertURLStringToNormal
       (theGlobalVariables.webArguments.theValues[i], false)));
    }
    if (MathRoutines::StringBeginsWith(theKey, "isSolutionFile"))
    { theCrawler.slideFilesExtraFlags.AddOnTop
      (MathRoutines::StringTrimWhiteSpace
       (HtmlRoutines::ConvertURLStringToNormal
       (theGlobalVariables.webArguments.theValues[i], false)));
    }
  }
  if (theCrawler.slideFilesExtraFlags.size > theCrawler.slideFileNamesVirtualWithPatH.size)
    theCrawler.slideFilesExtraFlags.SetSize(theCrawler.slideFileNamesVirtualWithPatH.size);
  else
    for (int i = theCrawler.slideFilesExtraFlags.size; i < theCrawler.slideFileNamesVirtualWithPatH.size; i ++)
      theCrawler.slideFilesExtraFlags.AddOnTop("");
  theCrawler.desiredPresentationTitle =
  HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("title"), false);
  std::stringstream comments;
  if (theGlobalVariables.GetWebInput("layout") == "printable")
    theCrawler.flagProjectorMode = false;
  if (theGlobalVariables.GetWebInput("answerKey") == "true")
    theCrawler.flagAnswerKey = true;
  if (theGlobalVariables.userCalculatorRequestType == "homeworkFromSource" ||
      theGlobalVariables.userCalculatorRequestType == "homeworkSource")
    theCrawler.flagHomeworkRatherThanSlides = true;
  if (!theCrawler.BuildOrFetchFromCachePDF(&comments, &comments))
  { JSData result;
    HtmlInterpretation::GetJSDataUserInfo(result, comments.str());
    this->flagDoAddContentLength = true;
    stOutput << result.ToString(false);
    return 0;
  }
  this->SetHeadeR("HTTP/1.0 200 OK", "Content-Type: application/pdf; Access-Control-Allow-Origin: *");
  this->flagDoAddContentLength = true;
  stOutput << theCrawler.targetPDFbinaryContent;
  return 0;
}

int WebWorker::ProcessSlidesSource()
{ MacroRegisterFunctionWithName("WebWorker::ProcessSlidesSource");
  this->SetHeaderOKNoContentLength("");
  LaTeXcrawler theCrawler;
  for (int i = 0; i < theGlobalVariables.webArguments.size(); i ++)
  { std::string theKey = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.webArguments.theKeys[i], false);
    //stOutput << "DEBUG: considering key: " << theKey
    //<< " with value: " << theGlobalVariables.webArguments.theValues[i] << "<br>";
    if (theKey != "fileName" && MathRoutines::StringBeginsWith(theKey, "file"))
    { theCrawler.slideFileNamesVirtualWithPatH.AddOnTop
      (MathRoutines::StringTrimWhiteSpace
       (HtmlRoutines::ConvertURLStringToNormal
       (theGlobalVariables.webArguments.theValues[i], false)));
    }
  }
  if (theGlobalVariables.userCalculatorRequestType == "homeworkSource")
    theCrawler.flagHomeworkRatherThanSlides = true;
  else
    theCrawler.flagHomeworkRatherThanSlides = false;
  theCrawler.desiredPresentationTitle =
  HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("title"), false);
  std::stringstream comments;
  if (theGlobalVariables.GetWebInput("layout") == "printable")
    theCrawler.flagProjectorMode = false;
  if (theGlobalVariables.GetWebInput("answerKey") == "false")
    theCrawler.flagAnswerKey = false;
  else
    theCrawler.flagAnswerKey = true;
  theCrawler.flagSourceOnly = true;
  theCrawler.flagCrawlTexSourcesRecursively = true;
  if (!theCrawler.BuildOrFetchFromCachePDF(&comments, &comments))
  { this->flagDoAddContentLength = true;
    stOutput << "<!DOCTYPE html>"
    << "<html>"
    << "<head>"
    << HtmlRoutines::GetCSSLinkCalculator()
    << "</head>"
    << "<body>"
    << "<calculatorNavigation>"
    << HtmlInterpretation::ToStringNavigation()
    << "</calculatorNavigation>";
    stOutput << comments.str();
    stOutput << "</body></html>";
    return 0;
  }
  this->SetHeadeR("HTTP/1.0 200 OK", "Content-Type: application/x-latex; Access-Control-Allow-Origin: *");
  this->flagDoAddContentLength = true;
  stOutput << theCrawler.targetLaTeX;
  return 0;
}

int WebWorker::ProcessClonePage(bool useJSON)
{ MacroRegisterFunctionWithName("WebWorker::ProcessClonePage");
  this->SetHeaderOKNoContentLength("");
  stOutput << this->GetClonePageResult(useJSON);
  return 0;
}

int WebWorker::ProcessProblemGiveUp()
{ MacroRegisterFunctionWithName("WebWorker::ProcessProblemGiveUp");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetAnswerOnGiveUp();
  return 0;
}

int WebWorker::ProcessProblemGiveUpJSON()
{ MacroRegisterFunctionWithName("WebWorker::ProcessProblemGiveUpJSON");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetAnswerOnGiveUp();
  return 0;
}

int WebWorker::ProcessProblemSolution()
{ MacroRegisterFunctionWithName("WebWorker::ProcessProblemSolution");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetProblemSolution();
  return 0;
}

int WebWorker::ProcessSubmitAnswersPreview()
{ MacroRegisterFunctionWithName("WebWorker::ProcessSubmitAnswersPreview");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::submitAnswersPreview();
  return 0;
}

std::string WebWorker::GetClonePageResult(bool useJSON)
{ MacroRegisterFunctionWithName("WebWorker::GetClonePageResult");
  this->SetHeaderOKNoContentLength("");
  return HtmlInterpretation::ClonePageResult(useJSON);
}

std::string WebWorker::GetAddUserEmails()
{ MacroRegisterFunctionWithName("WebWorker::GetAddUserEmails");
  return HtmlInterpretation::AddUserEmails(this->hostWithPort);
}

std::string HtmlInterpretation::ModifyProblemReport()
{ MacroRegisterFunctionWithName("WebWorker::ModifyProblemReport");
  bool shouldProceed = theGlobalVariables.flagLoggedIn && theGlobalVariables.UserDefaultHasAdminRights();
  if (shouldProceed)
    shouldProceed = theGlobalVariables.flagUsingSSLinCurrentConnection;
  if (!shouldProceed)
    return "<b>Modifying problems allowed only for logged-in admins under ssl connection. </b>";
  std::string mainInput = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("mainInput"), false);
  std::string fileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("fileName"), false);
  std::stringstream commentsOnFailure;
  bool fileExists = FileOperations::FileExistsVirtualCustomizedReadOnly(fileName, &commentsOnFailure);
  std::fstream theFile;
  if (!FileOperations::OpenFileVirtualCustomizedWriteOnly(theFile, fileName, false, true, false, &commentsOnFailure))
  { commentsOnFailure << "<b style =\"color:red\">Failed to open/create file: " << fileName << ". </b>";
    return commentsOnFailure.str();
  }
  theFile << mainInput;
  theFile.close();
  std::stringstream out;
  if (!fileExists)
    out << "File " << fileName << " didn't previously exist: just created it for you. ";
  out << "<b style =\"color:green\"> Written content to file: "
  << fileName << ". </b>";
  return out.str();
}

int WebWorker::ProcessSubmitAnswers()
{ MacroRegisterFunctionWithName("WebWorker::ProcessSubmitAnswers");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::SubmitAnswers();
  return 0;
}

std::string HtmlInterpretation::GetJavascriptCaptcha()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetCaptchaJavascript");
  std::stringstream out;
  out << "<script src ='https://www.google.com/recaptcha/api.js'></script>";
  return out.str();
}

std::string HtmlInterpretation::GetCaptchaDiv()
{ MacroRegisterFunctionWithName("HtmlInterpretation::GetCaptchaDiv");
  std::stringstream out;
  std::string recaptchaPublic;
  if (!FileOperations::LoadFileToStringVirtual("certificates/recaptcha-public.txt", recaptchaPublic, true, true, &out))
    out << "<span style =\"color:red\"><b>Couldn't find the recaptcha key in file: certificates/recaptcha-public.txt. </b></span>";
  else
    out << "<div class =\"g-recaptcha\" data-sitekey =\"" << recaptchaPublic << "\"></div>";
  return out.str();
}

std::string WebWorker::GetForgotLoginPage()
{ MacroRegisterFunctionWithName("WebWorker::GetForgotLoginPage");
  std::stringstream out;
  out << "<html>"
  << HtmlRoutines::GetCSSLinkCalculator();
  out << HtmlInterpretation::GetJavascriptCaptcha();
  out << "<body>\n";
  out << "<calculatorNavigation>"
  << theGlobalVariables.ToStringNavigation()
  << "</calculatorNavigation>\n";
  theWebServer.CheckExecutableVersionAndRestartIfNeeded(true);
  out << "<form name =\"forgotlogWorkerin\" id =\"login\">";
  out << "<table>"
  << "<tr>"
  << "<td> Email:</td>"
  << "<td> <input type =\"text\" id =\"email\" name =\"email\" placeholder =\"email\">\n</td>\n"
  << "</tr>"
  << "</table>";
  out << HtmlInterpretation::GetCaptchaDiv();
  out << "</form>";
  out << "<button onclick=\"submitForgotLogin();\">Send recovery email</button>"
  << "<span id =\"forgotLoginResultReport\"></span>"
  << "\n<br>\n"
  << "<span id =\"forgotLoginResult\"></span>";
  out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  out << "</body></html>";

  return out.str();
}

bool WebWorker::CorrectRequestsBEFORELoginReturnFalseIfModified()
{ MacroRegisterFunctionWithName("WebWorker::CorrectRequestsBEFORELoginReturnFalseIfModified");
  bool stateNotModified = true;
  if (!theGlobalVariables.flagSSLisAvailable)
    if (this->addressComputed == theGlobalVariables.DisplayNameExecutable &&
        theGlobalVariables.userCalculatorRequestType == "")
    { this->addressComputed = theGlobalVariables.DisplayNameExecutable;
      theGlobalVariables.userCalculatorRequestType = "calculator";
      stateNotModified = false;
    }
  if (this->addressComputed == "/" || this->addressComputed == "")
  { this->addressComputed = theGlobalVariables.DisplayNameExecutableApp; //was: theGlobalVariables.DisplayNameExecutable;
    theGlobalVariables.userCalculatorRequestType = WebAPI::app;
    //theGlobalVariables.userCalculatorRequestType = "selectCourse";
    stateNotModified = false;
  }
  return stateNotModified;
}

bool WebWorker::RedirectIfNeeded(std::stringstream& argumentProcessingFailureComments)
{ MacroRegisterFunctionWithName("WebWorker::RedirectIfNeeded");
  UserCalculatorData& theUser = theGlobalVariables.userDefault;
  if (!theUser.flagEnteredPassword)
    return false;
  if (theGlobalVariables.userCalculatorRequestType == "changePassword" ||
      theGlobalVariables.userCalculatorRequestType == WebAPI::request::activateAccount ||
      theGlobalVariables.userCalculatorRequestType == WebAPI::request::activateAccountJSON)
    return false;
  std::stringstream redirectedAddress;
  if (this->addressComputed == theGlobalVariables.DisplayNameExecutable)
    redirectedAddress << theGlobalVariables.DisplayNameExecutable << "?request="
    << theGlobalVariables.userCalculatorRequestType << "&";
  else
    redirectedAddress << this->addressComputed << "?";
  for (int i = 0; i < theGlobalVariables.webArguments.size(); i ++)
    if (theGlobalVariables.webArguments.theKeys[i] != "password" &&
        theGlobalVariables.webArguments.theKeys[i] != "request" &&
        theGlobalVariables.webArguments.theKeys[i] != "googleToken" &&
        theGlobalVariables.webArguments.theKeys[i] != "G_AUTHUSER_H" &&
        theGlobalVariables.webArguments.theKeys[i] != "activationToken"
        )
      redirectedAddress << theGlobalVariables.webArguments.theKeys[i] << "="
      << theGlobalVariables.webArguments.theValues[i] << "&";
  if (argumentProcessingFailureComments.str() != "")
    redirectedAddress << "error="
    << HtmlRoutines::ConvertStringToURLString(argumentProcessingFailureComments.str(), false)
    << "&";
  std::stringstream headerStream;
  headerStream << "Location: " << redirectedAddress.str();
  this->SetHeadeR("HTTP/1.0 303 See other", headerStream.str());
  stOutput << "<html><head>"
  << "<meta http-equiv=\"refresh\" content =\"0; url ='" << redirectedAddress.str()
  << "'\" />"
  << "</head>";
  stOutput << "<body>Click <a href=\"" << redirectedAddress.str() << "\">"
  << " here " << "</a> if your browser does not redirect the page automatically. ";
  //stOutput << "<hr>DEBUG: addressComputed: <br>" << HtmlRoutines::ConvertStringToHtmlString(this->addressComputed, true)
  //<< "<hr>addressToRedirectTo: <br>\n" << HtmlRoutines::ConvertStringToHtmlString(redirectedAddress.str(), true)
  //<< "<hr>argumentComputed: <br>\n" << HtmlRoutines::ConvertStringToHtmlString(this->argumentComputed, true)
  //<< "<hr>addressGetOrPost: <br>" << HtmlRoutines::ConvertStringToHtmlString(this->addressGetOrPost, true)
  //<< "<hr>MessageBody: <br>\n" << HtmlRoutines::ConvertStringToHtmlString(this->messageBody, true)
  //<< " <br>MessageHead: <br>\n" << HtmlRoutines::ConvertStringToHtmlString(this->messageHead, true)
  //<< HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable()
  //<< this->ToStringMessageFullUnsafe();
  stOutput << "</body></html>";
  return true;
}

bool WebWorker::CorrectRequestsAFTERLoginReturnFalseIfModified()
{ MacroRegisterFunctionWithName("WebWorker::CorrectRequestsAFTERLoginReturnFalseIfModified");
  bool stateNotModified = true;
  bool shouldFallBackToDefaultPage = false;
  if (this->addressComputed == "/" || this->addressComputed == "")
    shouldFallBackToDefaultPage = true;
  if (!theGlobalVariables.flagSSLisAvailable)
    if (this->addressComputed == theGlobalVariables.DisplayNameExecutable &&
        theGlobalVariables.userCalculatorRequestType == "")
    { this->addressComputed = theGlobalVariables.DisplayNameExecutable;
      theGlobalVariables.userCalculatorRequestType = WebAPI::app;
      stateNotModified = false;
    }
  if (theGlobalVariables.flagLoggedIn)
  { if (this->addressComputed == theGlobalVariables.DisplayNameExecutable &&
        theGlobalVariables.userCalculatorRequestType == "")
      shouldFallBackToDefaultPage = true;
    else if (theGlobalVariables.userCalculatorRequestType == "template" &&
             theGlobalVariables.GetWebInput("courseHome") == "")
      shouldFallBackToDefaultPage = true;
  }
  if (shouldFallBackToDefaultPage)
  { this->addressComputed = theGlobalVariables.DisplayNameExecutableApp;
    theGlobalVariables.userCalculatorRequestType = WebAPI::app;
    stateNotModified = false;
  }
  return stateNotModified;
}

bool WebWorker::ProcessRedirectAwayFromWWW()
{ std::string addressNoWWW;
  if (!MathRoutines::StringBeginsWith(theGlobalVariables.hostWithPort, "www.", &addressNoWWW))
    return false;
  std::stringstream newAddressStream, redirectHeaderStream;
  newAddressStream << "https://" << addressNoWWW;
  if (this->addressGetOrPost.size() != 0)
    if (this->addressGetOrPost[0] != '/')
      this->addressGetOrPost = '/' + this->addressGetOrPost;
  newAddressStream << this->addressGetOrPost;
  if (theGlobalVariables.flagRunningApache)
    redirectHeaderStream << "Content-Type: text/html\r\n";
  redirectHeaderStream << "Location: " << newAddressStream.str();
  //double fixme;
  this->SetHeadeR("HTTP/1.0 301 Moved Permanently", redirectHeaderStream.str());
  //this->SetHeaderOKNoContentLength();
  stOutput << "<html><body>Please remove the www. from the address, it creates issues with authentication services. "
  << "Click <a href=\"" << newAddressStream.str() << "\">here</a> if not redirected automatically. ";
  stOutput << "</body></html>";
  return true;
}

int WebWorker::ProcessLoginNeededOverUnsecureConnection()
{ MacroRegisterFunctionWithName("WebWorker::ProcessLoginNeededOverUnsecureConnection");
  std::stringstream redirectStream, newAddressStream;
  newAddressStream << "https://" << this->hostNoPort;
  if (this->hostNoPort == "")
    newAddressStream << "calculator-algebra.org";
  newAddressStream << ":" << this->parent->httpSSLPort;
  if (this->addressGetOrPost.size() != 0)
    if (this->addressGetOrPost[0] != '/')
      this->addressGetOrPost = '/' + this->addressGetOrPost;
  newAddressStream << this->addressGetOrPost;
  if (theGlobalVariables.flagRunningApache)
    redirectStream << "Content-Type: text/html\r\n";
  if (this->hostNoPort != "")
  { redirectStream << "Location: " << newAddressStream.str();
    this->SetHeadeR("HTTP/1.0 301 Moved Permanently", redirectStream.str());
    stOutput << "<html><body>Address available through secure (SSL) connection only. "
    << "Click <a href=\"" << newAddressStream.str() << "\">here</a> if not redirected automatically. ";
  } else
  { this->SetHeaderOKNoContentLength("");
    stOutput << "<html><body>Address available through secure (SSL) connection only. <br>"
    << "<b style ='color:red'>In the web address, please change http to https. </b><br>"
    << "Unfortunately, I can't redirect you automatically as your browser did not tell me "
    << "under what domain name it sees me, and the server responds to multiple domain names. ";

  }
  //stOutput << "<br>" << redirectStream.str();
  if (theGlobalVariables.flagRunningApache)
    stOutput << "To avoid seeing this message, <b style = \"color:red\">please use the secure version:</b> "
    << "<a href=\"https://" << this->hostNoPort << "\">https://" << this->hostNoPort
    << "</a>. If using bookmarks, don't forget to re-bookmark to the secure site. ";
  stOutput << "</body></html>";
  return 0;
}

bool WebWorker::RequireSSL()
{ return theGlobalVariables.flagSSLisAvailable;
}

int WebWorker::ServeClient()
{ MacroRegisterFunctionWithName("WebWorker::ServeClient");
  theGlobalVariables.timeServeClientStart = theGlobalVariables.GetElapsedMilliseconds();
  theGlobalVariables.flagComputationStarted = true;
  theGlobalVariables.userDefault.flagMustLogin = true;
  theGlobalVariables.userDefault.flagStopIfNoLogin = true;
  UserCalculatorData& theUser = theGlobalVariables.userDefault;
  theGlobalVariables.IndicatorStringOutputFunction = WebServer::PipeProgressReportToParentProcess;
  if (this->requestTypE != this->requestGet &&
      this->requestTypE != this->requestPost &&
      this->requestTypE != this->requestHead)
  { if (this->requestTypE != this->requestUnknown &&
        this->requestTypE != this->requestChunked)
      crash << "Something is wrong: request type does not have any of the expected values. " << crash;
    this->ProcessUnknown();
    return 0;
  }
  if (theGlobalVariables.theThreads.size <= 1)
    crash << "Number of threads must be at least 2 in this point of code..." << crash;
  this->ExtractHostInfo();
  this->ExtractAddressParts();
  std::stringstream argumentProcessingFailureComments;
  this->flagArgumentsAreOK = true;
  if (!this->ExtractArgumentsFromMessage(this->argumentComputed, argumentProcessingFailureComments))
    this->flagArgumentsAreOK = false;
  if (!this->ExtractArgumentsFromCookies(argumentProcessingFailureComments))
    this->flagArgumentsAreOK = false;
  theGlobalVariables.userCalculatorRequestType = "";
  if (this->addressComputed == theGlobalVariables.DisplayNameExecutable)
    theGlobalVariables.userCalculatorRequestType = theGlobalVariables.GetWebInput("request");
  std::stringstream comments;
  comments << "Address, request computed: " << this->addressComputed << ", "
  << theGlobalVariables.userCalculatorRequestType << ". ";
  bool isNotModified = this->CorrectRequestsBEFORELoginReturnFalseIfModified();
  if (!isNotModified)
    comments << this->ToStringAddressRequest() << ": modified before login. ";
  if (theWebServer.addressStartsInterpretedAsCalculatorRequest.Contains(this->addressComputed))
  { theGlobalVariables.userCalculatorRequestType = this->addressComputed;
    std::string correctedRequest;
    if (MathRoutines::StringBeginsWith(theGlobalVariables.userCalculatorRequestType, "/", &correctedRequest))
    { //std::cout << "Got to here";
      theGlobalVariables.userCalculatorRequestType = correctedRequest;
      comments << "Address was interpretted as request, so your request was set to: " << theGlobalVariables.userCalculatorRequestType << ". ";
    }
    //std::cout << "Address request set to: " << theGlobalVariables.userCalculatorRequestType << std::endl;
  }
  theUser.flagMustLogin = this->parent->RequiresLogin(theGlobalVariables.userCalculatorRequestType, this->addressComputed);
  if (!theUser.flagMustLogin)
    comments << "Login not needed. ";

  if (this->RequireSSL() && !theGlobalVariables.flagUsingSSLinCurrentConnection)
    return this->ProcessLoginNeededOverUnsecureConnection();
  if (this->ProcessRedirectAwayFromWWW())
    return 0;
  this->flagArgumentsAreOK = this->Login(argumentProcessingFailureComments);
  //stOutput << "DEBUG: got to here pt 2.";
  isNotModified = this->CorrectRequestsAFTERLoginReturnFalseIfModified();
  if (!isNotModified)
    comments << this->ToStringAddressRequest() << ": modified after login. ";
  if (this->RedirectIfNeeded(argumentProcessingFailureComments))
  { //logWorker << "DEBUG: redirecting as needed: " <<  argumentProcessingFailureComments.str() << logger::endL;
    return 0;
  }
  theUser.flagStopIfNoLogin =
  (!theGlobalVariables.flagLoggedIn && theGlobalVariables.flagUsingSSLinCurrentConnection &&
   theUser.flagMustLogin);
  if (theUser.flagStopIfNoLogin)
  { if (theGlobalVariables.userCalculatorRequestType == "changePassword")
      theUser.flagStopIfNoLogin = false;
  }
  if (theUser.flagStopIfNoLogin)
  { if (theGlobalVariables.userCalculatorRequestType != "logout" &&
        theGlobalVariables.userCalculatorRequestType != "login")
      argumentProcessingFailureComments << this->ToStringAddressRequest() << " requires login. ";
    argumentProcessingFailureComments << comments.str();
    theGlobalVariables.CookiesToSetUsingHeaders.SetKeyValue("authenticationToken", "");
    if (argumentProcessingFailureComments.str() != "")
      theGlobalVariables.SetWebInpuT("authenticationToken", "");
    return this->ProcessLoginUserInfo(argumentProcessingFailureComments.str());
  }
  //logWorker << "DEBUG: argumentProcessingFailureComments: " <<  argumentProcessingFailureComments.str() << logger::endL;
  if (argumentProcessingFailureComments.str() != "" &&
      (theUser.flagMustLogin || theGlobalVariables.userCalculatorRequestType == WebAPI::request::userInfoJSON))
  { theGlobalVariables.SetWebInpuT("error", argumentProcessingFailureComments.str());
    //logWorker << "DEBUG: set global error" << logger::endL;
  }
  //stOutput << "DEBUG: got to here pt 3.";
  if (theUser.flagMustLogin)
    this->errorLogin = argumentProcessingFailureComments.str();
  if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.flagLoggedIn)
    if (theGlobalVariables.GetWebInput("spoofHostName") != "")
    { theGlobalVariables.hostNoPort = HtmlRoutines::ConvertURLStringToNormal
      (theGlobalVariables.GetWebInput("spoofHostName"), false);
      //stOutput << "spoofhostname: " << theGlobalVariables.GetWebInput("spoofHostName");
      theGlobalVariables.CookiesToSetUsingHeaders.SetKeyValue("spoofHostName", theGlobalVariables.hostNoPort);
    }
  if (theGlobalVariables.flagLoggedIn && theGlobalVariables.UserDefaultHasAdminRights() &&
      theGlobalVariables.userCalculatorRequestType == WebAPI::databaseParameters::entryPoint)
    return this->ProcessDatabaseJSON();
  else if (theGlobalVariables.flagLoggedIn &&
           theGlobalVariables.userCalculatorRequestType == "databaseDeleteOneEntry")
    return this->ProcessDatabaseDeleteEntry();
  else if (theGlobalVariables.flagLoggedIn && theGlobalVariables.UserDefaultHasAdminRights() &&
           theGlobalVariables.userCalculatorRequestType == "databaseModifyEntry")
    return this->ProcessDatabaseModifyEntry();
  else if (theGlobalVariables.flagLoggedIn && theGlobalVariables.userCalculatorRequestType == "accounts")
    return this->ProcessAccounts();
  else if (theGlobalVariables.flagLoggedIn && theGlobalVariables.userCalculatorRequestType == "accountsJSON")
    return this->ProcessAccountsJSON();
  else if (theGlobalVariables.flagLoggedIn && theGlobalVariables.UserDefaultHasAdminRights() &&
           theGlobalVariables.userCalculatorRequestType == "navigation")
    return this->ProcessNavigation();
  else if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::examplesJSON)
    return this->ProcessCalculatorExamplesJSON();
  else if (theGlobalVariables.userCalculatorRequestType == "toggleMonitoring")
    return this->ProcessToggleMonitoring();
  else if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::serverStatusJSON)
    return this->ProcessServerStatusJSON();
  if (theGlobalVariables.userCalculatorRequestType == "monitor")
    return this->ProcessMonitor();
  else if (theGlobalVariables.userCalculatorRequestType == "pause" && theGlobalVariables.flagAllowProcessMonitoring)
    return this->ProcessPauseWorker();
  else if (theGlobalVariables.userCalculatorRequestType == "indicator" && theGlobalVariables.flagAllowProcessMonitoring)
    return this->ProcessComputationIndicator();
  //The following line is NOT ALLOWED:
  //this->parent->ReleaseNonActiveWorkers();
  //Reason: the architecture changed, now multiple requests
  //can be piped through one worker.
  else if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::setProblemData)
    return this->ProcessSetProblemDatabaseInfo();
  else if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::changePassword)
    return this->ProcessChangePassword(argumentProcessingFailureComments.str());
  else if (theGlobalVariables.userCalculatorRequestType == "changePasswordPage" ||
           theGlobalVariables.userCalculatorRequestType == WebAPI::request::activateAccount)
    return this->ProcessChangePasswordPage();
  else if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::activateAccountJSON)
    return this->ProcessActivateAccount();
  else if (theGlobalVariables.userCalculatorRequestType == "signUp")
    return this->ProcessSignUP();
  else if (theGlobalVariables.userCalculatorRequestType == "forgotLogin")
    return this->ProcessForgotLogin();
  else if (theGlobalVariables.userCalculatorRequestType == "forgotLoginPage")
    return this->ProcessForgotLoginPage();
  else if (theGlobalVariables.userCalculatorRequestType == "login")
    return this->ProcessLoginUserInfo(comments.str());
  else if (theGlobalVariables.userCalculatorRequestType == "logout")
    return this->ProcessLogout();
  else if ((theGlobalVariables.userCalculatorRequestType == "addEmails"||
            theGlobalVariables.userCalculatorRequestType == "addUsers") &&
            theGlobalVariables.flagLoggedIn)
    return this->ProcessAddUserEmails();
  else if (theGlobalVariables.userCalculatorRequestType == "setTeacher" &&
           theGlobalVariables.flagLoggedIn)
    return this->ProcessAssignTeacherToSection();
  else if ((theGlobalVariables.userCalculatorRequestType == "submitAnswers" ||
            theGlobalVariables.userCalculatorRequestType == "submitExercise") &&
           theGlobalVariables.flagLoggedIn)
    return this->ProcessSubmitAnswers();
  else if (theGlobalVariables.userCalculatorRequestType == "scores" && theGlobalVariables.flagLoggedIn)
    return this->ProcessScores();
  else if (theGlobalVariables.userCalculatorRequestType == "scoresInCoursePage" && theGlobalVariables.flagLoggedIn)
    return this->ProcessScoresInCoursePage();
  else if (theGlobalVariables.UserGuestMode() && theGlobalVariables.userCalculatorRequestType == "submitExerciseNoLogin")
    return this->ProcessSubmitAnswers();
  else if ((theGlobalVariables.userCalculatorRequestType == "problemGiveUp" &&
            theGlobalVariables.flagLoggedIn) ||
            theGlobalVariables.userCalculatorRequestType == "problemGiveUpNoLogin")
    return this->ProcessProblemGiveUp();
  else if ((theGlobalVariables.userCalculatorRequestType == "problemGiveUpJSON" &&
            theGlobalVariables.flagLoggedIn) ||
            theGlobalVariables.userCalculatorRequestType == "problemGiveUpNoLoginJSON")
    return this->ProcessProblemGiveUpJSON();
  else if ((theGlobalVariables.userCalculatorRequestType == "problemSolution" &&
            theGlobalVariables.flagLoggedIn) ||
            theGlobalVariables.userCalculatorRequestType == "problemSolutionNoLogin")
    return this->ProcessProblemSolution();
  else if ((theGlobalVariables.userCalculatorRequestType == "submitAnswersPreview" ||
            theGlobalVariables.userCalculatorRequestType == "submitExercisePreview") &&
           theGlobalVariables.flagLoggedIn)
    return this->ProcessSubmitAnswersPreview();
  else if (theGlobalVariables.userCalculatorRequestType == "submitExercisePreviewNoLogin" &&
           theGlobalVariables.UserGuestMode())
    return this->ProcessSubmitAnswersPreview();
  else if (theGlobalVariables.userCalculatorRequestType == "scoredQuiz" ||
           theGlobalVariables.userCalculatorRequestType == "exercise" ||
           theGlobalVariables.userCalculatorRequestType == "exerciseNoLogin" ||
           theGlobalVariables.userCalculatorRequestType == "exerciseJSON" ||
           theGlobalVariables.userCalculatorRequestType == "scoredQuizJSON")
    return this->ProcessExamPageJSON();
  else if (theGlobalVariables.userCalculatorRequestType == "template" ||
           theGlobalVariables.userCalculatorRequestType == "templateNoLogin")
    return this->ProcessTemplate();
  else if (theGlobalVariables.userCalculatorRequestType == "templateJSON" ||
           theGlobalVariables.userCalculatorRequestType == "templateJSONNoLogin")
    return this->ProcessTemplateJSON();
  else if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::userInfoJSON)
    return this->ProcessLoginUserInfo(comments.str());
  else if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::editPage)
    return this->ProcessEditPageJSON();
  else if (theGlobalVariables.userCalculatorRequestType == "modifyPage")
    return this->ProcessModifyPage();
  else if (theGlobalVariables.userCalculatorRequestType == "slidesFromSource" ||
           theGlobalVariables.userCalculatorRequestType == "homeworkFromSource")
    return this->ProcessSlidesOrHomeworkFromSource();
  else if (theGlobalVariables.userCalculatorRequestType == "slidesSource" ||
           theGlobalVariables.userCalculatorRequestType == "homeworkSource")
    return this->ProcessSlidesSource();
  else if (theGlobalVariables.userCalculatorRequestType == "clonePage")
    return this->ProcessClonePage(false);
  else if (theGlobalVariables.userCalculatorRequestType == "clonePageJSON")
    return this->ProcessClonePage(true);
  else if (theGlobalVariables.userCalculatorRequestType == "compute")
    return this->ProcessCompute();
  else if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::selectCourseJSON)
    return this->ProcessSelectCourseJSON();
  else if (theGlobalVariables.userCalculatorRequestType == "about")
    return this->ProcessAbout();
  else if (theGlobalVariables.userCalculatorRequestType == "topicListJSON" ||
           theGlobalVariables.userCalculatorRequestType == "topicListJSONNoLogin")
    return this->ProcessTopicListJSON();
  else if (theGlobalVariables.userCalculatorRequestType == WebAPI::app)
    return this->ProcessApp(true);
  else if (theGlobalVariables.userCalculatorRequestType == WebAPI::appNoCache)
    return this->ProcessApp(false);
  else if ("/" + theGlobalVariables.userCalculatorRequestType == WebAPI::request::onePageJS)
    return this->ProcessCalculatorOnePageJS(false);
  else if ("/" + theGlobalVariables.userCalculatorRequestType == WebAPI::request::onePageJSWithHash)
    return this->ProcessCalculatorOnePageJS(true);
  return this->ProcessFolderOrFile();
}

int WebWorker::ProcessFolderOrFile()
{ MacroRegisterFunctionWithName("WebWorker::ProcessFolderOrFile");
  this->VirtualFileName = HtmlRoutines::ConvertURLStringToNormal(this->addressComputed, true);
  this->SanitizeVirtualFileName();
  std::stringstream commentsOnFailure;
  if (!FileOperations::GetPhysicalFileNameFromVirtual
      (this->VirtualFileName, this->RelativePhysicalFileNamE, theGlobalVariables.UserDefaultHasAdminRights(), false,
       &commentsOnFailure))
  { //  std::cout << "GOT TO not found!" << std::endl;
    this->SetHeadeR("HTTP/1.0 404 Object not found", "Content-Type: text/html");
    stOutput << "<html><body><b>File name deemed unsafe. "
    << "Please note that folder names are not allowed to contain dots and file names "
    << "are not allowed to start with dots.</b> There may be additional restrictions "
    << "on file names added for security reasons.</body></html>";
    return 0;
  }
  if (FileOperations::IsFolderUnsecure(this->RelativePhysicalFileNamE))
    return this->ProcessFolder();
  return this->ProcessFile();
}

void WebWorker::ResetPipesNoAllocation()
{ MacroRegisterFunctionWithName("WebWorker::ResetPipesNoAllocation");
  this->PauseComputationReportReceived.ResetNoAllocation();
  this->PauseWorker.ResetNoAllocation();
  this->PauseIndicatorPipeInUse.ResetNoAllocation();
  this->pipeWorkerToWorkerRequestIndicator.ResetNoAllocation();
  this->pipeWorkerToWorkerIndicatorData.ResetNoAllocation();
}

void WebWorker::ReleaseKeepInUseFlag()
{ MacroRegisterFunctionWithName("WebWorker::ReleaseKeepInUseFlag");
  this->pipeWorkerToServerTimerPing.Release();
  this->pipeWorkerToServerControls.Release();
  this->pipeWorkerToWorkerRequestIndicator.Release();
  this->pipeWorkerToWorkerIndicatorData.Release();
  this->pipeWorkerToWorkerUserInput.Release();
  this->pipeWorkerToWorkerStatus.Release();
  this->PauseComputationReportReceived.Release();
  this->PauseWorker.Release();
  this->PauseIndicatorPipeInUse.Release();
  this->timeOfLastPingServerSideOnly = - 1;
  WebServer::Release(this->connectedSocketID);
}

void WebWorker::Release()
{ this->ReleaseKeepInUseFlag();
  this->flagInUse = false;
}

void WebWorker::OutputShowIndicatorOnTimeout()
{ MacroRegisterFunctionWithName("WebWorker::OutputShowIndicatorOnTimeout");
  MutexLockGuard theLock(this->PauseIndicatorPipeInUse.mutexForProcessBlocking.GetElement());
  //this->PauseIndicatorPipeInUse.RequestPausePauseIfLocked();
  theGlobalVariables.flagOutputTimedOut = true;
  theGlobalVariables.flagTimedOutComputationIsDone = false;
  //ProgressReportWebServer theReport("WebServer::OutputShowIndicatorOnTimeout");
  logWorker << logger::blue << "Computation timeout, sending progress indicator instead of output. " << logger::endL;
  if (theGlobalVariables.flagTimeOutExplanationAlreadyDisplayed)
  { stOutput << "Your computation is taking more than "
    << theGlobalVariables.takeActionAfterComputationMilliseconds
    << " ms. ";
  }
  if (!theGlobalVariables.flagAllowProcessMonitoring)
  { stOutput << "Monitoring computations is not allowed on this server.<br> "
    << "If you want to carry out a long computation you will need to install the calculator on your own machine.<br> "
    << "At the moment, the only way to do that is by setting the variable "
    << "theGlobalVariables.flagAllowProcessMonitoring to true, "
    << "achieved through manually compiling the calculator from source.<br> ";
    return;
  }
  if (this->indexInParent < 0)
    crash << "Index of worker is smaller than 0, this shouldn't happen. " << crash;
  stOutput << "<script language = \"javascript\">\n"
  << "window.calculator.processMonitoring.monitor.start("
  << this->indexInParent + 1
  << ");\n"
  << "</script>";
  this->SendAllBytesWithHeaders();
  for (int i = 0; i < this->parent->theWorkers.size; i ++)
    if (i != this->indexInParent)
      this->parent->theWorkers[i].Release();
  this->parent->Release(this->connectedSocketID);
  //set flags properly:
  //we need to rewire the standard output and the crashing mechanism:
  crash.CleanUpFunction = WebWorker::OutputCrashAfterTimeout;
  //note that standard output cannot be rewired in the beginning of the function as we use the old stOutput
  stOutput.theOutputFunction = WebWorker::StandardOutputAfterTimeOut;
  //theReport.SetStatus("WebServer::OutputShowIndicatorOnTimeout: continuing computation.");
  this->PauseIndicatorPipeInUse.mutexForProcessBlocking.GetElement().UnlockMe();
  //theReport.SetStatus("WebServer::OutputShowIndicatorOnTimeout: exiting function.");
  //this->SignalIamDoneReleaseEverything();
  //logWorker << consoleGreen("Indicator: released everything and signalled end.") << logger::endL;
}

std::string WebWorker::ToStringAddressRequest() const
{ std::stringstream out;
  out << "Address = ";
  if (this->addressComputed == "")
    out << "[empty]";
  else
    out << this->addressComputed;
  out << "; ";
  out << " request = ";
  if (theGlobalVariables.userCalculatorRequestType == "")
    out << "[empty]";
  else
    out << theGlobalVariables.userCalculatorRequestType;
  return out.str();
}

std::string WebWorker::ToStringStatus() const
{ std::stringstream out;
  //if (theWebServer.currentlyConnectedAddresses.GetCoefficientsSum() != theWebServer.
  out << "<br>Worker " << this->indexInParent + 1;
  if (this->flagInUse)
  { if (this->parent->activeWorker == this->indexInParent)
      out << ", <span style =\"color:green\"><b>current process</b></span>";
    else
      out << ", <b>in use</b>";
    out << ", <a href=\"calculator?request=monitor&mainInput=" << this->indexInParent + 1 << "\">monitor process "
    << this->indexInParent + 1 << "</a>";
  } else
    out << ", not in use";
  if (this->displayUserInput != "")
    out << ", user input: <span style =\"color:blue\">" << this->displayUserInput << "</span>";
  out << ", connection " << this->connectionID << ", process ID: ";
  if (this->ProcessPID != 0)
    out << this->ProcessPID;
  else
    out << "(not accessible)";
  out << ", socketID: ";
  if (this->connectedSocketID == - 1)
    out << "released in current process, value before release: " << this->connectedSocketIDLastValueBeforeRelease;
  else
    out << this->connectedSocketID;
  out << ". ";
  out << " Server time at last ping: " << this->timeOfLastPingServerSideOnly << " seconds. ";
  if (this->pingMessage != "")
    out << " Message at last ping: " << this->pingMessage;
  if (this->status != "")
    out << "<br><span style =\"color:red\"><b> Status: " << this->status << "</b></span><br>";
  out << "Pipes: " << this->pipeWorkerToServerControls.ToString()
  << ", " << this->pipeWorkerToServerTimerPing.ToString()
  << ", " << this->pipeWorkerToWorkerRequestIndicator.ToString()
  << ", " << this->pipeWorkerToWorkerIndicatorData.ToString()
  << ", " << this->pipeWorkerToWorkerUserInput.ToString()
  << ", " << this->pipeWorkerToWorkerStatus.ToString()
  << ", " << this->PauseWorker.ToString()
  << ", " << this->PauseComputationReportReceived.ToString()
  << ", " << this->PauseIndicatorPipeInUse.ToString();
  out << ", user address: " << this->userAddress << ".";
  return out.str();
}

bool WebServer::CheckConsistency()
{ if (this->flagDeallocated)
    crash << "Use after free of WebServer." << crash;
  return true;
}

void WebServer::ReleaseEverything()
{
#ifdef MACRO_use_open_ssl
  this->theSSLdata.FreeSSL();
#endif
  logger& currentLog = theGlobalVariables.flagIsChildProcess ? logWorker : logServer;
  ProgressReportWebServer::flagServerExists = false;
  for (int i = 0; i < this->theWorkers.size; i ++)
    this->theWorkers[i].Release();
  theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection = 0;
  theGlobalVariables.IndicatorStringOutputFunction = 0;
  theGlobalVariables.PauseUponUserRequest = 0;
  this->activeWorker = - 1;
  if (theGlobalVariables.flagServerDetailedLog)
    currentLog << logger::red << "DEBUG: "
    << " About to close socket: " << this->listeningSocketHTTP << ". " << logger::endL;
  if (this->listeningSocketHTTP != - 1)
  { close(this->listeningSocketHTTP);
    if (theGlobalVariables.flagServerDetailedLog)
      currentLog << logger::red << "DEBUG: "
      << " Just closed socket: " << this->listeningSocketHTTP << logger::endL;
    this->listeningSocketHTTP = - 1;
  }
  if (this->listeningSocketHttpSSL != - 1)
  { close(this->listeningSocketHttpSSL);
    if (theGlobalVariables.flagServerDetailedLog)
      currentLog << logger::red << "DEBUG: "
      << " Just closed socket: " << this->listeningSocketHttpSSL << logger::endL;
  }
  this->listeningSocketHttpSSL = - 1;
}

WebServer::~WebServer()
{ this->flagDeallocated = true;
}

void WebServer::ReturnActiveIndicatorAlthoughComputationIsNotDone()
{ //logWorker << logger::red << ("Got THUS far") << logger::endL;
  //logWorker << "here am i";
  MacroRegisterFunctionWithName("WebServer::ReturnActiveIndicatorAlthoughComputationIsNotDone");
  theWebServer.GetActiveWorker().OutputShowIndicatorOnTimeout();
  //stOutput << "What the hell";
  //stOutput.Flush();
}

void WebServer::FlushActiveWorker()
{ theWebServer.GetActiveWorker().SendAllBytesWithHeaders();
}

void WebServer::SendStringThroughActiveWorker(const std::string& theString)
{ theWebServer.GetActiveWorker().QueueStringForSendingWithHeadeR(theString, false);
}

void WebServer::PipeProgressReportToParentProcess(const std::string& theString)
{ if (theWebServer.activeWorker == - 1)
    return;
  theWebServer.GetActiveWorker().PipeProgressReportToParentProcess(theString);
}

WebServer::WebServer()
{ this->flagPort8155 = true;
  this->flagDeallocated = false;
  this->flagTryToKillOlderProcesses = true;
  this->activeWorker = - 1;
  this->timeLastExecutableModification = - 1;
  this->listeningSocketHTTP = - 1;
  this->listeningSocketHttpSSL = - 1;
  this->highestSocketNumber = - 1;
  this->flagReapingChildren = false;
  this->MaxNumWorkersPerIPAdress = 24;
  this->MaxTotalUsedWorkers = 40;
  this->NumFailedSelectsSoFar = 0;
  this->NumSuccessfulSelectsSoFar = 0;
  this->NumProcessesReaped = 0;
  this->NumProcessAssassinated = 0;
  this->NumConnectionsSoFar = 0;
  this->NumWorkersNormallyExited = 0;
  this->WebServerPingIntervalInSeconds = 10;
  this->NumberOfServerRequestsWithinAllConnections = 0;
}

WebWorker& WebServer::GetActiveWorker()
{ MacroRegisterFunctionWithName("WebServer::GetActiveWorker");
  void (*oldOutputFunction)(const std::string& stringToOutput) = stOutput.theOutputFunction;
  stOutput.theOutputFunction = 0; //<- We are checking if the web server is in order.
  //Before that we prevent the crashing mechanism from trying to use (the eventually corrupt) web server
  //to report the error over the Internet.
  if (this->activeWorker < 0 || this->activeWorker >= this->theWorkers.size)
    crash << "Active worker index is " << this->activeWorker << " however I have " << this->theWorkers.size
    << " workers. " << crash;
  stOutput.theOutputFunction = oldOutputFunction;//<-the web server is in order,
  //therefore we restore the ability to report crashes over the internet.
  return this->theWorkers[this->activeWorker];
}

void WebServer::SignalActiveWorkerDoneReleaseEverything()
{ MacroRegisterFunctionWithName("WebServer::SignalActiveWorkerDoneReleaseEverything");
  if (theWebServer.activeWorker == - 1)
    return;
  theWebServer.GetActiveWorker().SendAllAndWrapUp();
  theWebServer.activeWorker = - 1;
}

void WebServer::ReleaseActiveWorker()
{ MacroRegisterFunctionWithName("WebServer::ReleaseActiveWorker");
  if (this->activeWorker == - 1)
    return;
  this->GetActiveWorker().Release();
  this->activeWorker = - 1;
}

void WebServer::WorkerTimerPing(int64_t pingTime)
{ if (theWebServer.activeWorker == - 1)
  { if (!theGlobalVariables.flagComputationFinishedAllOutputSentClosing)
      crash << "WebServer::WorkerTimerPing called when the computation is not entirely complete. " << crash;
    return;
  }
  std::stringstream outTimestream;
  outTimestream << pingTime;
  theWebServer.GetActiveWorker().pipeWorkerToServerTimerPing.WriteAfterEmptying(outTimestream.str(), false, false);
}

void WebServer::ReleaseNonActiveWorkers()
{ MacroRegisterFunctionWithName("WebServer::ReleaseNonActiveWorkers");
  for (int i = 0; i < this->theWorkers.size; i ++)
    if (i != this->activeWorker)
      this->theWorkers[i].ReleaseKeepInUseFlag();
}

void WebServer::ReleaseSocketsNonActiveWorkers()
{ MacroRegisterFunctionWithName("WebServer::ReleaseSocketsNonActiveWorkers");
  for (int i = 0; i < this->theWorkers.size; i ++)
    if (i != this->activeWorker)
      this->Release(this->theWorkers[i].connectedSocketID);
}

bool WebServer::EmergencyRemoval_LastCreatedWorker()
{ this->GetActiveWorker().Release();
  this->activeWorker = - 1;
  this->theWorkers.SetSize(this->theWorkers.size - 1);
  return false;
}

bool WebServer::CreateNewActiveWorker()
{ MacroRegisterFunctionWithName("WebServer::CreateNewActiveWorker");
  if (this->activeWorker != - 1)
  { crash << "Calling CreateNewActiveWorker requires the active worker index to be - 1." << crash;
    return false;
  }
  bool found = false;
  for (int i = 0; i < this->theWorkers.size; i ++)
    if (!this->theWorkers[i].flagInUse)
    { this->activeWorker = i;
      found = true;
      break;
    }
  if (this->activeWorker == - 1)
  { this->activeWorker = this->theWorkers.size;
    this->theWorkers.SetSize(this->theWorkers.size + 1);
  }
  this->GetActiveWorker().indexInParent = this->activeWorker;
  this->GetActiveWorker().parent = this;
  this->GetActiveWorker().pingMessage = "";
  if (found)
  { this->theWorkers[this->activeWorker].flagInUse = true;
    return true;
  }
  this->GetActiveWorker().Release();
  this->theWorkers[this->activeWorker].flagInUse = false; //<-until everything is initialized, we cannot be in use.
  std::stringstream stowstream, wtosStream, wtowStream;
  stowstream << "S->W" << this->activeWorker + 1 << ": ";
  wtosStream << "W" << this->activeWorker + 1 << "->S: ";
  wtowStream << "W<->W" << this->activeWorker + 1 << ": ";
  std::string stow = stowstream.str();
  std::string wtos = wtosStream.str();
  std::string wtow = wtowStream.str();
  if (!this->GetActiveWorker().PauseComputationReportReceived.CreateMe(stow + "report received", false, true))
  { logPlumbing << "Failed to create pipe: "
    << this->GetActiveWorker().PauseComputationReportReceived.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!this->GetActiveWorker().PauseWorker.CreateMe(stow + "pause", false, true))
  { logPlumbing << "Failed to create pipe: "
    << this->GetActiveWorker().PauseWorker.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!this->GetActiveWorker().PauseIndicatorPipeInUse.CreateMe(stow + "ind. pipe busy", false, true))
  { logPlumbing << "Failed to create pipe: "
    << this->GetActiveWorker().PauseIndicatorPipeInUse.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!this->GetActiveWorker().pipeWorkerToWorkerRequestIndicator.CreateMe(wtow + "request-indicator"))
  { logPlumbing << "Failed to create pipe: "
    << this->GetActiveWorker().pipeWorkerToWorkerRequestIndicator.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!this->GetActiveWorker().pipeWorkerToServerTimerPing.CreateMe(wtos + "ping", false, false, false, true))
  { logPlumbing << "Failed to create pipe: "
    << this->GetActiveWorker().pipeWorkerToServerTimerPing.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!this->GetActiveWorker().pipeWorkerToServerControls.CreateMe(wtos + "controls", false, false, false, true))
  { logPlumbing << "Failed to create pipe: "
    << this->GetActiveWorker().pipeWorkerToServerControls.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!this->GetActiveWorker().pipeWorkerToWorkerIndicatorData.CreateMe(wtow + "indicator-data"))
  { logPlumbing << "Failed to create pipe: "
    << this->GetActiveWorker().pipeWorkerToWorkerIndicatorData.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!this->GetActiveWorker().pipeWorkerToWorkerUserInput.CreateMe(wtow + "user input", false, false, false, true))
  { logPlumbing << "Failed to create pipe: "
    << this->GetActiveWorker().pipeWorkerToWorkerUserInput.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!this->GetActiveWorker().pipeWorkerToWorkerStatus.CreateMe(wtos + "worker status", false, false, false, true))
  { logPlumbing << "Failed to create pipe: "
    << this->GetActiveWorker().pipeWorkerToWorkerStatus.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  logPlumbing << logger::green
  << "Allocated new worker & plumbing data structures. Total worker data structures: "
  << this->theWorkers.size << ". "
  << logger::endL;
  this->theWorkers[this->activeWorker].flagInUse = true;
  return true;
}

std::string WebServer::ToStringLastErrorDescription()
{ std::stringstream out;
  out << strerror(errno) << ". ";
  return out.str();
}

std::string WebServer::ToStringStatusActive()
{ MacroRegisterFunctionWithName("WebServer::ToStringStatusActive");
  if (this->activeWorker == - 1)
    return "server.";
  std::stringstream out;
  if (this->activeWorker != this->GetActiveWorker().indexInParent)
    crash << "Bad index in parent!" << crash;
  out << this->GetActiveWorker().ToStringStatus();
  return out.str();
}

std::string WebServer::ToStringConnectionSummary()
{ MacroRegisterFunctionWithName("WebServer::ToStringConnectionSummary");
  if (theGlobalVariables.flagRunningApache)
     return "Running through standard Apache web server, no connection details to display. ";
  std::stringstream out;
  TimeWrapper now;
  now.AssignLocalTime();
  out << "<b>Server status.</b> Server time: local: " << now.ToStringLocal() << ", gm: " << now.ToStringGM() << ".<br>";
  double timeRunning = - 1;
  if (this->activeWorker < 0 || this->activeWorker >= this->theWorkers.size)
    timeRunning = theGlobalVariables.GetElapsedSeconds();
  else
    timeRunning = this->GetActiveWorker().timeOfLastPingServerSideOnly;
  out
  << timeRunning
  << " seconds = "
  << TimeWrapper::ToStringSecondsToDaysHoursSecondsString
     (timeRunning, false, false)
  << " web server uptime. ";
  int approxNumPings = timeRunning / this->WebServerPingIntervalInSeconds;
  if (approxNumPings < 0)
    approxNumPings = 0;
  int numConnectionsSoFarApprox = this->NumConnectionsSoFar - approxNumPings;
  if (numConnectionsSoFarApprox < 0)
    numConnectionsSoFarApprox = 0;
  out << "~" << numConnectionsSoFarApprox << " actual connections "
  << "(with " << this->NumberOfServerRequestsWithinAllConnections << " server requests served)" << " + ~"
  << approxNumPings << " self-test-pings (" << this->NumConnectionsSoFar << " connections total)"
  << " served since last restart. ";

  out
  << " The number tends to be high as many browsers open more than one connection per page visit. <br>"
  << "<b>The following policies are quite strict and will be relaxed in the future. </b><br>"
  << this->MaxTotalUsedWorkers << " global maximum of simultaneous non-closed connections allowed. "
  << "When the limit is exceeded, all connections except a randomly chosen one will be terminated. "
  << "<br> " << this->MaxNumWorkersPerIPAdress
  << " maximum simultaneous connection per IP address. "
  << "When the limit is exceeded, all connections from that IP address are terminated. ";
  return out.str();
}

std::string WebServer::ToStringStatusForLogFile()
{ MacroRegisterFunctionWithName("WebServer::ToStringStatusForLogFile");
  if (theGlobalVariables.flagRunningApache)
     return "Running through standard Apache web server, no connection details to display. ";
  std::stringstream out;
  out << this->ToStringConnectionSummary();
  int numInUse = 0;
  for (int i = 0; i < this->theWorkers.size; i ++)
    if (this->theWorkers[i].flagInUse)
      numInUse ++;
  out << "<hr>Currently, there are " << numInUse << " worker(s) in use. The peak number of worker(s)/concurrent connections was " << this->theWorkers.size << ". ";
  out
  << "<br>kill commands: " << this->NumProcessAssassinated
  << ", processes reaped: " << this->NumProcessesReaped
  << ", normally reclaimed workers: " << this->NumWorkersNormallyExited
  << ", connections so far: " << this->NumConnectionsSoFar;
  return out.str();
}

std::string WebServer::ToStringStatusAll()
{ MacroRegisterFunctionWithName("WebServer::ToStringStatusAll");
  if (theGlobalVariables.flagRunningApache)
    return "Running through Apache. ";
  std::stringstream out;

  if (!theGlobalVariables.UserDefaultHasAdminRights())
  { out << this->ToStringConnectionSummary();
    return out.str();
  }
  out << this->ToStringStatusForLogFile();
  out << "<hr>";
  out << "<a href=\"/LogFiles/server_starts_and_unexpected_restarts.html\">" << "Log files</a><br>";
  out << "<a href=\"/LogFiles/" << GlobalVariables::GetDateForLogFiles() << "/\">" << "Current log files</a><hr>";
  if (this->activeWorker == - 1)
    out << "The process is functioning as a server.";
  else
  { out << "The process is functioning as a worker. The active worker is number " << this->activeWorker + 1 << ". ";
    out << "<br>" << this->ToStringStatusActive();
  }
  for (int i = 0; i < this->theWorkers.size; i ++)
  { WebWorker& currentWorker = this->theWorkers[i];
    if (!currentWorker.flagInUse)
      continue;
    currentWorker.pipeWorkerToWorkerStatus.ReadWithoutEmptying(false, false);
    currentWorker.status = currentWorker.pipeWorkerToWorkerStatus.GetLastRead();
  }
  out << "<hr>";
  out << "Connections: " << this->currentlyConnectedAddresses.ToString();
  for (int i = 0; i < this->theWorkers.size; i ++)
    out << "<hr>" << this->theWorkers[i].ToStringStatus();
  return out.str();
}

void WebServer::CheckExecutableVersionAndRestartIfNeeded(bool callReload)
{ struct stat theFileStat;
  if (stat(theGlobalVariables.PhysicalNameExecutableWithPath.c_str(), &theFileStat) != 0)
    return;
  if (this->timeLastExecutableModification != - 1)
    if (this->timeLastExecutableModification != theFileStat.st_ctime)
    { stOutput << "<b>The server executable was updated, but the server has not been restarted yet. "
      << "Restarting in 0.5 seconds...</b>";
      if (callReload)
        stOutput << "<script language =\"javascript\">setTimeout(resubmit, 500); "
        << " function resubmit() { location.reload(true);}</script>";
      else
        stOutput << "<script language =\"javascript\">setTimeout(resubmit, 500); "
        << " function resubmit() { document.getElementById('formCalculator').submit();}</script>";
      if (this->activeWorker != - 1)
      { this->GetActiveWorker().SendAllBytesWithHeaders();
        this->ReleaseActiveWorker();
      }
      logWorker << "Current process spawned from file with time stamp: "
      << this->timeLastExecutableModification
      << "; latest executable has different time stamp: " << theFileStat.st_ctime
      << ". " << logger::red << "RESTARTING." << logger::endL;
      this->Restart();
    }
}

void WebServer::Restart()
{ //logWorker << "Killing all copies of the calculator and restarting..." << logger::endL;
  //logWorker << "Time limit after restart: " << theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit << logger::endL;
  if (this->listeningSocketHTTP != - 1)
    this->Release(this->listeningSocketHTTP);
  if (this->listeningSocketHttpSSL != - 1)
    this->Release(this->listeningSocketHttpSSL);
  std::stringstream theCommand;
  logger* currentLog = 0;
  currentLog = &logWorker;
  if (theGlobalVariables.processType == "serverMonitor")
    currentLog = &logServerMonitor;
  if (theGlobalVariables.processType == "server")
    currentLog = &logServer;
  int timeLimitSeconds = theGlobalVariables.MaxComputationMilliseconds / 1000;

  *currentLog << logger::red << " restarting with time limit " << timeLimitSeconds << logger::endL;
  theCommand << "killall " << theGlobalVariables.PhysicalNameExecutableNoPath << " \r\n./";
  theCommand << theGlobalVariables.PhysicalNameExecutableNoPath;
  if (theWebServer.flagPort8155)
    theCommand << " server " << " nokill " << timeLimitSeconds;
  else
    theCommand << " server8080 " << " nokill " << timeLimitSeconds;
//  std::cout << "\nCalling: " << theCommand.str() << "\n";
  theGlobalVariables.CallSystemNoOutput(theCommand.str(), true); //kill any other running copies of the calculator.
}

void WebServer::initPortsITry()
{ if (this->flagPort8155)
    this->PortsITryHttp.AddOnTop("8155");
  this->PortsITryHttp.AddOnTop("8080");
  this->PortsITryHttp.AddOnTop("8081");
  this->PortsITryHttp.AddOnTop("8082");
  this->PortsITryHttp.AddOnTop("8155");
  if (!theGlobalVariables.flagSSLisAvailable)
    return;
  this->PortsITryHttpSSL.AddOnTop("8166");
  this->PortsITryHttpSSL.AddOnTop("8083");
  this->PortsITryHttpSSL.AddOnTop("8084");
  this->PortsITryHttpSSL.AddOnTop("8085");
}

void WebServer::initListeningSockets()
{ MacroRegisterFunctionWithName("WebServer::initListeningSockets");
  if (listen(this->listeningSocketHTTP, WebServer::maxNumPendingConnections) == - 1)
    crash << "Listen function failed on http port." << crash;
  if (theGlobalVariables.flagSSLisAvailable)
    if (listen(this->listeningSocketHttpSSL, WebServer::maxNumPendingConnections) == - 1)
      crash << "Listen function failed on https port." << crash;
  this->highestSocketNumber = - 1;
  if (this->listeningSocketHTTP != - 1)
  { this->theListeningSockets.AddOnTop(this->listeningSocketHTTP);
    this->highestSocketNumber = MathRoutines::Maximum(this->listeningSocketHTTP, this->highestSocketNumber);
  }
  if (this->listeningSocketHttpSSL != - 1)
  { this->theListeningSockets.AddOnTop(this->listeningSocketHttpSSL);
    this->highestSocketNumber = MathRoutines::Maximum(this->listeningSocketHttpSSL, this->highestSocketNumber);
  }
}

void WebServer::initDates()
{ this->timeLastExecutableModification = - 1;
  struct stat theFileStat;
  if (stat(theGlobalVariables.PhysicalNameExecutableWithPath.c_str(), &theFileStat) != 0)
    return;
  this->timeLastExecutableModification = theFileStat.st_ctime;
}

void WebServer::ReleaseWorkerSideResources()
{ MacroRegisterFunctionWithName("WebServer::ReleaseWorkerSideResources");
  logger& currentLog = theGlobalVariables.flagIsChildProcess ? logWorker : logProcessKills;
  if (theGlobalVariables.flagServerDetailedLog)
    currentLog << logger::red << "DEBUG: server about to RELEASE active workder. " << logger::endL;
  this->Release(this->GetActiveWorker().connectedSocketID);
  if (theGlobalVariables.flagServerDetailedLog)
    currentLog << logger::green << "DEBUG: server RELEASED active worker. " << logger::endL;
  //<-release socket- communication is handled by the worker.
  this->activeWorker = - 1; //<-The active worker is needed only in the child process.
}

bool WebServer::RequiresLogin(const std::string& inputRequest, const std::string& inputAddress)
{ MacroRegisterFunctionWithName("WebServer::RequiresLogin");
  if (inputAddress == theGlobalVariables.DisplayNameExecutable)
    if (this->requestsNotNeedingLogin.Contains(inputRequest))
      return false;
  for (int i = 0; i < this->addressStartsNotNeedingLogin.size; i ++)
    if (MathRoutines::StringBeginsWith(inputAddress, this->addressStartsNotNeedingLogin[i]))
      return false;
  return true;
}

void segfault_sigaction(int signal, siginfo_t* si, void* arg)
//<- this signal should never happen in
//<- server, so even if racy, we take the risk of a hang.
//<- racy-ness in child process does not bother us: hanged children are still fine.
{ (void) signal; //avoid unused parameter warning, portable.
  (void) arg;
  crash << "Caught segfault at address: " << si->si_addr << crash;
  exit(0);
}

void WebServer::fperror_sigaction(int signal)
{ (void) signal;
  crash << "Fatal arithmetic error. " << crash;
  exit(0);
}

void WebServer::TerminateChildSystemCall(int i)
{ if (!this->theWorkers[i].flagInUse)
    return;
  this->theWorkers[i].flagInUse = false;
  this->currentlyConnectedAddresses.SubtractMonomial(this->theWorkers[i].userAddress, 1);
  if (this->theWorkers[i].ProcessPID > 0)
  { if (theGlobalVariables.flagServerDetailedLog)
      logProcessKills << "DEBUG: " << " killing child index: " << i << "." << logger::endL;
    kill(this->theWorkers[i].ProcessPID, SIGKILL);
    this->theWorkers[i].ProcessPID = - 1;
  }
  this->theWorkers[i].ResetPipesNoAllocation();
}

void WebServer::HandleTooManyConnections(const std::string& incomingUserAddress)
{ MacroRegisterFunctionWithName("WebServer::HandleTooManyConnections");
  if (theGlobalVariables.flagIsChildProcess)
    return;
  if (theGlobalVariables.flagServerDetailedLog)
    logProcessStats << logger::red << "DEBUG: "
    << " too many connections handler start. " << logger::endL;
  MonomialWrapper<std::string, MathRoutines::hashString>
  incomingAddress(incomingUserAddress);
  bool purgeIncomingAddress =
  (this->currentlyConnectedAddresses.GetMonomialCoefficient(incomingAddress) >
   this->MaxNumWorkersPerIPAdress);
  if (!purgeIncomingAddress)
    return;
  List<double> theTimes;
  List<int> theIndices;
  for (int i = 0; i < this->theWorkers.size; i ++)
    if (this->theWorkers[i].flagInUse)
      if (this->theWorkers[i].userAddress == incomingAddress)
      { theTimes.AddOnTop(this->theWorkers[i].timeServerAtWorkerStart);
        theIndices.AddOnTop(i);
      }
  theTimes.QuickSortAscending(0, &theIndices);
  for (int j = 0; j < theTimes.size; j ++)
  { this->TerminateChildSystemCall(theIndices[j]);
    std::stringstream errorStream;
    errorStream
    << "Terminating child " << theIndices[j] + 1 << " with PID "
    << this->theWorkers[theIndices[j]].ProcessPID
    << ": address: " << incomingAddress << " opened more than "
    << this->MaxNumWorkersPerIPAdress << " simultaneous connections. ";
    this->theWorkers[theIndices[j]].pingMessage = errorStream.str();
    logProcessKills << logger::red << errorStream.str() << logger::endL;
    this->NumProcessAssassinated ++;
  }
  if (theGlobalVariables.flagServerDetailedLog)
    logProcessStats << logger::green
    << "DEBUG: connection cleanup successful. " << logger::endL;
}

void WebServer::ProcessOneChildMessage(int childIndex, int& outputNumInUse)
{
  std::string messageString = this->theWorkers[childIndex].pipeWorkerToServerControls.GetLastRead();
  this->theWorkers[childIndex].flagInUse = false;
  this->currentlyConnectedAddresses.SubtractMonomial(this->theWorkers[childIndex].userAddress, 1);
  std::stringstream commentsOnFailure;
  JSData workerMessage;
  if (!workerMessage.readstring(messageString, false, &commentsOnFailure))
    logServer << logger::red << "Worker "
    << childIndex + 1 << " sent corrupted result message: "
    << messageString << ". Marking for reuse. " << logger::endL;
  else
    logServer << logger::green << "Worker "
    << childIndex + 1 << " done with message: "
    << messageString
    << ". Marking for reuse. " << logger::endL;
  outputNumInUse --;
  this->NumWorkersNormallyExited ++;
  if (workerMessage["result"].string == "toggleMonitoring")
    this->ToggleProcessMonitoring();
  if (workerMessage["connectionsServed"].type == JSData::JSnumber)
    this->NumberOfServerRequestsWithinAllConnections += (int) workerMessage["connectionsServed"].number;
}

void WebServer::RecycleChildrenIfPossible()
{ //Listen for children who have exited properly.
  //This might need to be rewritten: I wasn't able to make this work with any
  //mechanism other than pipes.
  MacroRegisterFunctionWithName("WebServer::RecycleChildrenIfPossible");
  if (theGlobalVariables.flagIsChildProcess)
    return;
  if (theGlobalVariables.flagServerDetailedLog)
    logProcessStats << logger::red << "DEBUG: RecycleChildrenIfPossible start. " << logger::endL;
//  this->ReapChildren();
  int numInUse = 0;
  for (int i = 0; i < this->theWorkers.size; i ++)
    if (this->theWorkers[i].flagInUse)
      numInUse ++;
  for (int i = 0; i < this->theWorkers.size; i ++)
    if (this->theWorkers[i].flagInUse)
    { PipePrimitive& currentControlPipe = this->theWorkers[i].pipeWorkerToServerControls;
      if (currentControlPipe.flagReadEndBlocks)
        crash << "Pipe: " << currentControlPipe.ToString() << " has blocking read end. " << crash;
      currentControlPipe.ReadIfFailThenCrash(false, true);
      if (currentControlPipe.lastRead.size > 0)
        this->ProcessOneChildMessage(i, numInUse);
      else
        logServer << logger::orange << "Worker " << i + 1 << " not done yet. " << logger::endL;
      PipePrimitive& currentPingPipe = this->theWorkers[i].pipeWorkerToServerTimerPing;
      if (currentPingPipe.flagReadEndBlocks)
        crash << "Pipe: " << currentPingPipe.ToString() << " has blocking read end. " << crash;
      currentPingPipe.ReadIfFailThenCrash(false, true);
      if (currentPingPipe.lastRead.size > 0)
      { this->theWorkers[i].pingMessage = currentPingPipe.GetLastRead();
        this->theWorkers[i].timeOfLastPingServerSideOnly = theGlobalVariables.GetElapsedSeconds();
        if (this->theWorkers[i].pingMessage != "")
          logServer << logger::blue << "Worker " << i + 1 << " ping: " << this->theWorkers[i].pingMessage << ". " << logger::endL;
      } else if (this->theWorkers[i].PauseWorker.CheckPauseIsRequested(false, true, true))
      { this->theWorkers[i].pingMessage = "worker paused, no pings.";
        this->theWorkers[i].timeOfLastPingServerSideOnly = theGlobalVariables.GetElapsedSeconds();
      } else
      { bool presumedDead = false;
        if (this->theWorkers[i].flagInUse)
          if (theGlobalVariables.MaxTimeNoPingBeforeChildIsPresumedDead > 0)
            if (theGlobalVariables.GetElapsedSeconds() -
                this->theWorkers[i].timeOfLastPingServerSideOnly >
                theGlobalVariables.MaxTimeNoPingBeforeChildIsPresumedDead)
              presumedDead = true;
        if (presumedDead)
        { this->TerminateChildSystemCall(i);
          std::stringstream pingTimeoutStream;
          pingTimeoutStream
          << theGlobalVariables.GetElapsedSeconds() - this->theWorkers[i].timeOfLastPingServerSideOnly
          << " seconds passed since worker " << i + 1
          << " last pinged the server; killing connection "
          << this->theWorkers[i].connectionID
          << ", pid: "
          << this->theWorkers[i].ProcessPID << ". ";
          logProcessKills << logger::red << pingTimeoutStream.str() << logger::endL;
          this->theWorkers[i].pingMessage = "<b style =\"color:red\">" + pingTimeoutStream.str() + "</b>";
          numInUse --;
          this->NumProcessAssassinated ++;
        }
      }
    }
  if (numInUse <= this->MaxTotalUsedWorkers)
  { if (theGlobalVariables.flagServerDetailedLog)
      logProcessStats << logger::green
      << "DEBUG: RecycleChildrenIfPossible exit point 1. " << logger::endL;
    return;
  }
  for (int i = 0; i < this->theWorkers.size && numInUse > 1; i ++)
  { if (this->theWorkers[i].flagInUse)
    { this->TerminateChildSystemCall(i);
      std::stringstream errorStream;
      errorStream
      << "Terminating child " << i + 1 << " with PID "
      << this->theWorkers[i].ProcessPID
      << ": too many workers in use. ";
      this->theWorkers[i].pingMessage = errorStream.str();
      logProcessKills << logger::red << errorStream.str() << logger::endL;
      numInUse --;
      this->NumProcessAssassinated ++;
    }
  }
  if (theGlobalVariables.flagServerDetailedLog)
    logProcessStats << logger::green
    << "DEBUG: RecycleChildrenIfPossible exit point 2. " << logger::endL;
}

bool WebServer::initPrepareWebServerALL()
{ MacroRegisterFunctionWithName("WebServer::initPrepareWebServerALL");
  this->initPortsITry();
  if (this->flagTryToKillOlderProcesses)
    this->Restart();
  usleep(10000);
  this->initDates();
  if (!this->initBindToPorts())
    return false;
  this->initPrepareSignals();
  this->initListeningSockets();
  return true;
}

bool WebServer::initBindToPorts()
{ MacroRegisterFunctionWithName("WebServer::initBindToPorts");
  addrinfo hints;
  addrinfo *servinfo = 0;
  addrinfo *p = 0;
  int yes = 1;
  int rv;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP
  // loop through all the results and bind to the first we can
  List<std::string>* thePorts = &this->PortsITryHttp;
  int* theListeningSocket = 0;
  theListeningSocket = &this->listeningSocketHTTP;
  for (int j = 0; j < 2; j ++, thePorts = &this->PortsITryHttpSSL, theListeningSocket = &this->listeningSocketHttpSSL)
    for (int i = 0; i < (*thePorts).size; i ++)
    { rv = getaddrinfo(NULL, (*thePorts)[i].c_str(), &hints, &servinfo);
      if (rv != 0)
      { logWorker << "getaddrinfo: " << gai_strerror(rv) << logger::endL;
        return false;
      }
      for (p = servinfo; p != NULL; p = p->ai_next)
      { *theListeningSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (*theListeningSocket == - 1)
        { logWorker << "Error: socket failed.\n";
          continue;
        }
        if (setsockopt(*theListeningSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == - 1)
          crash << "Error: setsockopt failed, error: \n" << strerror(errno) << crash;
        if (bind(*theListeningSocket, p->ai_addr, p->ai_addrlen) == - 1)
        { close(*theListeningSocket);
          *theListeningSocket = - 1;
          logServer << "Error: bind failed at port: " << (*thePorts)[i] << ". Error: "
          << this->ToStringLastErrorDescription() << logger::endL;
          continue;
        }
        int setFlagCounter = 0;
        while (fcntl(*theListeningSocket, F_SETFL, O_NONBLOCK) != 0)
        { if (++ setFlagCounter > 10)
            crash << "Error: failed to set non-blocking status to listening socket. " << crash;
        }
        break;
      }
      if (p != NULL)
      { logServer << logger::yellow << "Successfully bound to port " << (*thePorts)[i] << logger::endL;
        if (j == 0)
          this->httpPort = (*thePorts)[i];
        else
          this->httpSSLPort = (*thePorts)[i];
        break;
      }
      freeaddrinfo(servinfo); // all done with this structure
    }
  if (this->listeningSocketHTTP == - 1)
    crash << "Failed to bind to any of the ports " << this->PortsITryHttp.ToStringCommaDelimited() << "\n" << crash;
  if (theGlobalVariables.flagSSLisAvailable && this->listeningSocketHttpSSL == - 1)
    crash << "Failed to bind to any of the ports " << this->PortsITryHttpSSL.ToStringCommaDelimited() << "\n" << crash;
  return true;
}

void WebServer::initPrepareSignals()
{ MacroRegisterFunctionWithName("WebServer::initPrepareSignals");
  if (sigemptyset(&SignalSEGV.sa_mask) == - 1)
    crash << "Failed to initialize SignalSEGV mask. Crashing to let you know. " << crash;
  SignalSEGV.sa_sigaction = &segfault_sigaction;
  SignalSEGV.sa_flags = SA_SIGINFO;
  if (sigaction(SIGSEGV, &SignalSEGV, NULL) == - 1)
    crash << "Failed to register SIGSEGV handler (segmentation fault (attempt to write memory without permission))."
    << " Crashing to let you know. " << crash;
  ///////////////////////
  //catch floating point exceptions
  if (sigemptyset(&SignalFPE.sa_mask) == - 1)
    crash << "Failed to initialize SignalFPE mask. Crashing to let you know. " << crash;
  SignalFPE.sa_sigaction = 0;
  SignalFPE.sa_handler = &WebServer::fperror_sigaction;
  if (sigaction(SIGFPE, &SignalFPE, NULL) == - 1)
    crash << "Failed to register SIGFPE handler (fatal arithmetic error). Crashing to let you know. " << crash;
  ////////////////////
  //ignore interrupts
  //if (sigemptyset(&SignalINT.sa_mask) == - 1)
  //  crash << "Failed to initialize SignalINT mask. Crashing to let you know. " << crash;
  //SignalINT.sa_sigaction = 0;
  //SignalINT.sa_handler = &WebServer::Signal_SIGINT_handler;
  //if (sigaction(SIGINT, &SignalINT, NULL) == - 1)
  //  crash << "Failed to register null SIGINT handler. Crashing to let you know. " << crash;
  ////////////////////
  //reap children
  //if (sigemptyset(&SignalChild.sa_mask) == - 1)
  //  crash << "Failed to initialize SignalChild mask. Crashing to let you know. " << crash;
  //if (sigaddset(&SignalChild.sa_mask, SIGINT) == - 1)
  //  crash << "Failed to initialize SignalChild mask. Crashing to let you know. " << crash;
  ////////////////////////////////
  //sigchld signal should automatically be blocked when calling the sigchld handler.
  //Nevertheless, let's explicitly add it:
  //if (sigaddset(&SignalChild.sa_mask, SIGCHLD) == - 1)
  //  crash << "Failed to initialize SignalChild mask. Crashing to let you know. " << crash;
  ////////////////////////////////
  //if (sigaddset(&SignalChild.sa_mask, SIGFPE) == - 1)
  //  crash << "Failed to initialize SignalChild mask. Crashing to let you know. " << crash;
  //if (sigaddset(&SignalChild.sa_mask, SIGSEGV) == - 1)
  //  crash << "Failed to initialize SignalChild mask. Crashing to let you know. " << crash;
  SignalChild.sa_flags = SA_NOCLDWAIT;
  SignalChild.sa_handler =  NULL;
  //&WebServer::Signal_SIGCHLD_handler; // reap all dead processes
  if (sigaction(SIGCHLD, &SignalChild, NULL) == - 1)
    crash << "Was not able to register SIGCHLD handler (reaping child processes). Crashing to let you know." << crash;
//  sigemptyset(&sa.sa_mask);
//  sa.sa_flags = SA_RESTART;
//  if (sigaction(SIGCHLD, &sa, NULL) == - 1)
//    logWorker << "sigaction returned - 1" << logger::endL;
}

extern void MonitorWebServer();

void WebServer::WriteVersionJSFile()
{ MacroRegisterFunctionWithName("WebServer::WriteVersionJSFile");
  std::stringstream out;
  out << "\"use strict\"; //This file is automatically generated, please do not modify.\n";
  out << "var serverInformation = {\n  ";
  out << "  version:\"" << theGlobalVariables.buildVersionSimple << "\",\n";
  out << "};\n";
  out << "module.exports = {\nserverInformation,\n};\n";
  std::fstream theFileStream;
  FileOperations::OpenFileCreateIfNotPresentVirtual
  (theFileStream, "/calculator-html/server_information.js", false, true, false, false);
  theFileStream << out.str();

}

int WebServer::Run()
{ MacroRegisterFunctionWithName("WebServer::Run");
  theGlobalVariables.RelativePhysicalNameCrashLog = "crash_WebServerRun.html";
#ifdef MACRO_use_open_ssl
  SSLdata::initSSLkeyFiles();
#endif
  if (!this->flagTryToKillOlderProcesses)//<-worker log resets are needed, else forked processes reset their common log.
  //<-resets of the server logs are not needed, but I put them here nonetheless.
  { logWorker         .reset();
    logServerMonitor  .reset();
    logHttpErrors     .reset();
    logBlock          .reset();
    logIO             .reset();
    logOpenSSL        .reset();
    logProcessKills   .reset();
    logSocketAccept   .reset();
    logProcessStats   .reset();
    logPlumbing       .reset();
    logEmail          .reset();
    logServer         .reset();
    logSuccessfulForks.reset();
    logSuccessfulForks.flagWriteImmediately = true;
  }
  if (true)
  { int pidMonitor = fork();
    if (pidMonitor < 0)
      crash << "Failed to create server process. " << crash;
    if (pidMonitor == 0)
    { theGlobalVariables.processType = "serverMonitor";
      theGlobalVariables.flagIsChildProcess = true;
      MonitorWebServer();//<-this attempts to connect to the server over the internet and restarts if it can't.
      return 0;
    }
  }
  if (theParser == 0)
    theParser = new Calculator;
  PointerObjectDestroyer<Calculator> calculatorDestroyer(theParser);
  theParser->init();
  theParser->ComputeAutoCompleteKeyWords();
  theParser->WriteAutoCompleteKeyWordsToFile();
  this->WriteVersionJSFile();
  //theGlobalVariables.WriteSourceCodeFilesJS();
  theGlobalVariables.initModifiableDatabaseFields();
  HtmlRoutines::LoadStrings();
  theParser->flagShowCalculatorExamples = false;
  if (!this->initPrepareWebServerALL())
    return 1;
  logServer << logger::purple << "server: waiting for connections...\r\n" << logger::endL;
  sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size = sizeof their_addr;
  char userAddressBuffer[INET6_ADDRSTRLEN];
  this->initSSL();
  fd_set FDListenSockets;
  this->NumSuccessfulSelectsSoFar = 0;
  this->NumFailedSelectsSoFar = 0;
  long long previousReportedNumberOfSelects = 0;
  int previousServerStatReport = 0;
  int previousServerStatDetailedReport = 0;
  sigset_t allSignals, oldSignals;
  sigfillset(&allSignals);
  while (true)
  { // main accept() loop
    //    logWorker << logger::red << "select returned!" << logger::endL;
    FD_ZERO(&FDListenSockets);
    for (int i = 0; i < this->theListeningSockets.size; i ++)
      FD_SET(this->theListeningSockets[i], &FDListenSockets);
    if (theGlobalVariables.flagServerDetailedLog)
      logServer << logger::red << "DEBUG: About to enter select loop. " << logger::endL;
    while (select(this->highestSocketNumber + 1, &FDListenSockets, 0, 0, 0) == - 1)
    { if (this->flagReapingChildren)
        logServer << logger::yellow << "Select interrupted while reaping children. "
        << logger::endL;
      else
        logServer << logger::red << " Select failed: possibly due to reaping children. Error message: "
        << strerror(errno) << logger::endL;
      this->NumFailedSelectsSoFar ++;
    }
    if (theGlobalVariables.flagServerDetailedLog)
      logServer << logger::green << "DEBUG: select success. " << logger::endL;
    int64_t millisecondsBeforeFork = theGlobalVariables.GetElapsedMilliseconds();
    this->NumSuccessfulSelectsSoFar ++;
    if ((this->NumSuccessfulSelectsSoFar + this->NumFailedSelectsSoFar) - previousReportedNumberOfSelects > 100)
    { logSocketAccept << logger::blue << this->NumSuccessfulSelectsSoFar << " successful and "
      << this->NumFailedSelectsSoFar << " bad ("
      << this->NumSuccessfulSelectsSoFar + this->NumFailedSelectsSoFar << " total) selects. " << logger::endL;
      previousReportedNumberOfSelects = this->NumSuccessfulSelectsSoFar + this->NumFailedSelectsSoFar;
    }
    if (this->NumProcessAssassinated + this->NumProcessesReaped + this->NumWorkersNormallyExited +
        this->NumConnectionsSoFar - previousServerStatReport > 99)
    { previousServerStatReport = this->NumProcessAssassinated + this->NumProcessesReaped +
      this->NumWorkersNormallyExited + this->NumConnectionsSoFar;
      logProcessStats << "# kill commands: " << this->NumProcessAssassinated
      << " #processes reaped: " << this->NumProcessesReaped
      << " #normally reclaimed workers: " << this->NumWorkersNormallyExited
      << " #connections so far: " << this->NumConnectionsSoFar << logger::endL;
    }
    if (this->NumProcessAssassinated + this->NumProcessesReaped + this->NumWorkersNormallyExited +
        this->NumConnectionsSoFar - previousServerStatDetailedReport > 499)
    { previousServerStatDetailedReport = this->NumProcessAssassinated + this->NumProcessesReaped +
      this->NumWorkersNormallyExited + this->NumConnectionsSoFar;
      logProcessStats << this->ToStringStatusForLogFile() << logger::endL;
    }
    int newConnectedSocket = - 1;
    theGlobalVariables.flagUsingSSLinCurrentConnection = false;
    bool found = false;
    for (int i = theListeningSockets.size - 1; i >= 0; i --)
      if (FD_ISSET(this->theListeningSockets[i], &FDListenSockets))
      { //if (this->theListeningSockets[i] == this->listeningSocketHTTP)
        newConnectedSocket = accept(this->theListeningSockets[i], (struct sockaddr *)&their_addr, &sin_size);
        if (newConnectedSocket >= 0)
        { logServer << logger::green << "Connection candidate "
          << this->NumConnectionsSoFar + 1 << ". "
          << "Connected via listening socket " << this->theListeningSockets[i]
          << " on socket: " << newConnectedSocket;
          if (this->theListeningSockets[i] == this->listeningSocketHttpSSL)
          { theGlobalVariables.flagUsingSSLinCurrentConnection = true;
            logServer << logger::purple << " (SSL encrypted). " << logger::endL;
          } else
            logServer << logger::yellow << " (non-encrypted). " << logger::endL;
          break;
        } else
        { logSocketAccept << logger::red << "This is not supposed to happen: accept failed. Error: "
          << this->ToStringLastErrorDescription() << logger::endL;
          found = true;
        }
      }
    if (newConnectedSocket < 0 && !found)
      logSocketAccept << logger::red << "This is not supposed to to happen: select succeeded "
      << "but I found no set socket. " << logger::endL;
    if (newConnectedSocket < 0)
    { if (theGlobalVariables.flagServerDetailedLog)
        logServer << "DEBUG: newConnectedSocket is negative: " << newConnectedSocket << ". Not accepting. " << logger::endL;
      continue;
    }
    inet_ntop
    (their_addr.ss_family, get_in_addr((struct sockaddr *) &their_addr), userAddressBuffer, sizeof userAddressBuffer);
    this->HandleTooManyConnections(userAddressBuffer);
    this->RecycleChildrenIfPossible();
    if (!this->CreateNewActiveWorker())
    { logPlumbing << logger::purple
      << " failed to create an active worker. System error string: "
      << strerror(errno) << "\n"
      << logger::red << "Failed to create active worker: closing connection. " << logger::endL;
      close (newConnectedSocket);
      continue;
    }
    /////////////
    this->GetActiveWorker().connectedSocketID = newConnectedSocket;
    this->GetActiveWorker().flagUsingSSLInWorkerProcess = theGlobalVariables.flagUsingSSLinCurrentConnection;
    this->GetActiveWorker().connectedSocketIDLastValueBeforeRelease = newConnectedSocket;
    this->GetActiveWorker().timeServerAtWorkerStart = theGlobalVariables.GetElapsedSeconds();
    this->GetActiveWorker().timeOfLastPingServerSideOnly = this->GetActiveWorker().timeServerAtWorkerStart;
    this->GetActiveWorker().millisecondOffset = millisecondsBeforeFork; //<- measured right after select()
    //<-cannot set earlier as the active worker may change after recycling.
    this->NumConnectionsSoFar ++;
    this->GetActiveWorker().connectionID = this->NumConnectionsSoFar;
    this->GetActiveWorker().userAddress.theObject = userAddressBuffer;
    this->currentlyConnectedAddresses.AddMonomial(this->GetActiveWorker().userAddress, 1);
//    logWorker << this->ToStringStatus();
    /////////////
    if (theGlobalVariables.flagServerDetailedLog)
      logProcessStats << "DEBUG: about to fork, sigprocmasking " << logger::endL;
    int error = sigprocmask(SIG_BLOCK, &allSignals, &oldSignals);
    if (error < 0)
    { logServer << logger::red << "DEBUG: Sigprocmask failed. The server is going to crash. " << logger::endL;
      crash << "Sigprocmas failed. This should not happen. " << crash;
    }
    if (theGlobalVariables.flagServerDetailedLog)
    { logProcessStats << "DEBUG: Sigprocmask done. Proceeding to fork. "
      << "Time elapsed: " << theGlobalVariables.GetElapsedSeconds() << " second(s). <br>"
      << logger::endL;
    }
    int incomingPID = fork(); //creates an almost identical copy of this process.
    //<- Please don't assign directly to this->GetActiveWorker().ProcessPID.
    //<- There may be a race condition around this line of code and I
    //want as little code as possible between the fork and the logFile.
    //The original process is the parent, the almost identical copy is the child.
    //logWorker << "\r\nChildPID: " << this->childPID;
    if (incomingPID == 0)
      theGlobalVariables.processType = "worker";
    if (theGlobalVariables.flagServerDetailedLog)
      if (incomingPID == 0)
      { logWorker << "DEBUG: fork() successful in worker; elapsed ms @ fork(): "
        << theGlobalVariables.GetElapsedMilliseconds() << logger::endL;
      }
    if (theGlobalVariables.flagServerDetailedLog)
      if (incomingPID > 0)
      { logSuccessfulForks << "DEBUG: fork() successful; elapsed ms @ fork(): "
        << theGlobalVariables.GetElapsedMilliseconds() << logger::endL;
      }
    if (incomingPID < 0)
      logProcessKills << logger::red << " FAILED to spawn a child process. " << logger::endL;
    this->GetActiveWorker().ProcessPID = incomingPID;
    if (this->GetActiveWorker().ProcessPID == 0)
    { // this is the child (worker) process
      theGlobalVariables.flagIsChildProcess = true;
      if (theGlobalVariables.flagServerDetailedLog)
        logWorker << logger::green << "DEBUG: "
        << " FORK successful in worker, next step. Time elapsed: " << theGlobalVariables.GetElapsedSeconds()
        << " second(s). Calling sigprocmask. " << logger::endL;
      sigprocmask(SIG_SETMASK, &oldSignals, NULL);
      if (theGlobalVariables.flagServerDetailedLog)
        logWorker << logger::green << "DEBUG: sigprocmask success, running... " << logger::endL;
      int result = this->GetActiveWorker().Run();
      if (theGlobalVariables.flagServerDetailedLog)
        logWorker << "DEBUG: run finished, releasing resources. " << logger::endL;
      this->ReleaseEverything();
      if (theGlobalVariables.flagServerDetailedLog)
        logWorker << logger::green << "DEBUG: resources released, returning. " << logger::endL;
      return result;
    }
    if (theGlobalVariables.flagServerDetailedLog)
      logProcessStats << logger::green << "DEBUG: fork successful. Time elapsed: "
      << theGlobalVariables.GetElapsedMilliseconds() << " ms. "
      << "About to unmask signals. " << logger::endL;
    error = sigprocmask(SIG_SETMASK, &oldSignals, NULL);
    if (error < 0)
    { logServer << "Sigprocmask failed on server, I shall now crash. " << logger::endL;
      crash << "Sigprocmask failed on server." << crash;
    }
    if (theGlobalVariables.flagServerDetailedLog)
      logProcessStats << logger::green << "DEBUG: unmask successful. " << logger::endL;
    this->ReleaseWorkerSideResources();
  }
  this->ReleaseEverything();
#ifdef MACRO_use_open_ssl
  this->theSSLdata.FreeEverythingShutdownSSL();
#endif //MACRO_use_open_ssl
  return 0;
}

int WebWorker::Run()
{ MacroRegisterFunctionWithName("WebWorker::Run");
  theGlobalVariables.millisecondOffset = this->millisecondOffset;
  this->CheckConsistency();
  if (this->connectedSocketID == - 1)
    crash << "Worker::Run() started on a connecting with ID equal to - 1. " << crash;
  this->ResetPipesNoAllocation();
  std::stringstream processNameStream;
  processNameStream << "W" << this->indexInParent + 1 << ": ";
  PauseProcess::currentProcessName = processNameStream.str();
  theGlobalVariables.flagServerForkedIntoWorker = true;
  crash.CleanUpFunction = WebServer::SignalActiveWorkerDoneReleaseEverything;
  CreateTimerThread();
#ifdef MACRO_use_open_ssl
  theWebServer.SSLServerSideHandShake();
  if (theGlobalVariables.flagSSLisAvailable && theGlobalVariables.flagUsingSSLinCurrentConnection &&
      !this->parent->theSSLdata.flagSSLHandshakeSuccessful)
  { theGlobalVariables.flagUsingSSLinCurrentConnection = false;
    this->parent->SignalActiveWorkerDoneReleaseEverything();
    this->parent->ReleaseEverything();
    logOpenSSL << logger::red << "ssl fail #: " << this->parent->NumConnectionsSoFar << logger::endL;
    return - 1;
  }
#endif //Macro_use_open_ssl
  if (theGlobalVariables.flagSSLisAvailable && theGlobalVariables.flagUsingSSLinCurrentConnection)
    logOpenSSL << logger::green << "ssl success #: " << this->parent->NumConnectionsSoFar << ". " << logger::endL;
  /////////////////////////////////////////////////////////////////////////
  stOutput.theOutputFunction = WebServer::SendStringThroughActiveWorker;
  int result = 0;
  this->numberOfReceivesCurrentConnection = 0;
  while (true)
  { StateMaintainerCurrentFolder preserveCurrentFolder;
    this->flagAllBytesSentUsingFile = false;
    this->flagEncounteredErrorWhileServingFile = false;
    if (!this->ReceiveAll())
    { this->WrapUpConnection();
      bool sslWasOK = true;
#ifdef MACRO_use_open_ssl
      sslWasOK = (this->error == SSLdata::errors::errorWantRead);
#endif
      if (this->numberOfReceivesCurrentConnection > 0 && sslWasOK)
      { logIO << logger::green << "Connection timed out after successfully receiving "
        << this->numberOfReceivesCurrentConnection << " times. " << logger::endL;
        return 0;
      }
      logIO << logger::red << "Failed to receive all with error: " << this->error;
      return - 1;
    }
    this->numberOfReceivesCurrentConnection ++;
    if (theParser == 0)
    { theParser = new Calculator;
      theParser->init();
      logWorker << logger::blue << "Created new calculator for connection: " << this->numberOfReceivesCurrentConnection << logger::endL;
    }
    PointerObjectDestroyer<Calculator> calculatorDestroyer(theParser);
    if (this->messageHead.size() == 0)
      break;
    result = this->ServeClient();
    if (this->connectedSocketID == - 1)
      break;
    if (!this->flagAllBytesSentUsingFile)
      this->SendAllBytesWithHeaders();
    // We break the connection if we need to turn on monitoring as that concerns other processes.
    if (!this->flagKeepAlive || this->flagEncounteredErrorWhileServingFile || this->flagToggleMonitoring)
      break;
    //The function call needs security audit.
    this->resetConnection();
    logWorker << logger::blue << "Received " << this->numberOfReceivesCurrentConnection << " times on this connection, waiting for more. "
    << logger::endL;
    theGlobalVariables.millisecondOffset += theGlobalVariables.GetElapsedMilliseconds();
    this->parent->WorkerTimerPing(theGlobalVariables.millisecondOffset);
  }
  this->WrapUpConnection();
  return result;
}

void WebServer::Release(int& theDescriptor)
{ PauseProcess::Release(theDescriptor);
}

extern int mainTest(List<std::string>& remainingArgs);

void WebServer::FigureOutOperatingSystem()
{ MacroRegisterFunctionWithName("WebServer::FigureOutOperatingSystem");
  if (theGlobalVariables.OperatingSystem != "")
    return;
  List<std::string> supportedOSes;
  supportedOSes.AddOnTop("Ubuntu");
  supportedOSes.AddOnTop("CentOS");
  std::string commandOutput = theGlobalVariables.CallSystemWithOutput("cat /etc/*-release");
  if (commandOutput.find("ubuntu") != std::string::npos || commandOutput.find("Ubuntu") != std::string::npos ||
      commandOutput.find("UBUNTU") != std::string::npos)
    theGlobalVariables.OperatingSystem = "Ubuntu";
  else if (commandOutput.find("CentOS") != std::string::npos)
    theGlobalVariables.OperatingSystem = "CentOS";
  else
    theGlobalVariables.OperatingSystem = "";
  if (theGlobalVariables.OperatingSystem != "")
    return;
  logWorker << logger::red << "Your Linux flavor is not currently supported. " << logger::endL;
  logWorker << "We support the following Linux distros: "
  << logger::blue << supportedOSes.ToStringCommaDelimited() << logger::endL;
  logWorker << "Please post a request for support of your Linux flavor on our bug tracker: " << logger::endL
  << logger::green << "https://github.com/tmilev/calculator"
  << logger::endL << "and we will add your Linux flavor to the list of supported distros. " << logger::endL;
}

void WebServer::CheckSystemInstallationOpenSSL()
{ MacroRegisterFunctionWithName("WebServer::CheckSystemInstallationOpenSSL");
  if (theGlobalVariables.flagRunningApache)
    return;
  bool doInstallOpenSSL = false;
#ifndef MACRO_use_MongoDB
  doInstallOpenSSL = true;
#endif
  if (!doInstallOpenSSL)
    return;
  if (theGlobalVariables.configuration["openSSL"].type != JSData::JSUndefined)
    return;
  theGlobalVariables.configuration["openSSL"] = "Attempted installation";
  theGlobalVariables.StoreConfiguration();
  WebServer::FigureOutOperatingSystem();
  StateMaintainerCurrentFolder preserveFolder;
  logServer << "You appear to be missing an openssl installation. Let me try to install that for you. "
  << logger::green << "Enter your the sudo password as prompted please. " << logger::endL;
  if (theGlobalVariables.OperatingSystem == "Ubuntu")
  { theGlobalVariables.CallSystemNoOutput("sudo apt-get install libssl-dev", false);
    theGlobalVariables.configuration["openSSL"] = "Attempted installation on Ubuntu";
  } else if (theGlobalVariables.OperatingSystem == "CentOS")
  { theGlobalVariables.CallSystemNoOutput("sudo yum install openssl-devel", false);
    theGlobalVariables.configuration["openSSL"] = "Attempted installation on CentOS";
  }
  theGlobalVariables.StoreConfiguration();
}

void WebServer::CheckSystemInstallationMongoDB()
{ MacroRegisterFunctionWithName("WebServer::CheckSystemInstallationMongoDB");
  if (theGlobalVariables.flagRunningApache)
    return;
  bool doInstallMongo = false;
#ifndef MACRO_use_open_ssl
  doInstallMongo = true;
#endif
  if (!doInstallMongo)
    return;
  WebServer::FigureOutOperatingSystem();
  if (theGlobalVariables.OperatingSystem == "")
    return;
  if (theGlobalVariables.configuration["mongoDB"].type != JSData::JSUndefined)
    return;
  theGlobalVariables.configuration["mongoDB"] = "Attempted installation";
  theGlobalVariables.StoreConfiguration();

  StateMaintainerCurrentFolder preserveFolder;
  logServer << "You appear to be missing an openssl installation. Let me try to install that for you. "
  << logger::green << "Enter your the sudo password as prompted please. " << logger::endL;
  if (theGlobalVariables.OperatingSystem == "Ubuntu")
  { theGlobalVariables.CallSystemNoOutput("sudo apt-get install mongodb", false);
    theGlobalVariables.configuration["mongoDB"] = "Attempted installation on Ubuntu";
  } else if (theGlobalVariables.OperatingSystem == "CentOS")
  { theGlobalVariables.CallSystemNoOutput("sudo yum install mongodb", false);
    theGlobalVariables.configuration["mongoDB"] = "Attempted installation on CentOS";
  }
  theGlobalVariables.ChDir(theGlobalVariables.PhysicalPathProjectBase);
  theGlobalVariables.CallSystemNoOutput("make clean", false);
  logServer << "Proceeding to rebuild the calculator. " << logger::red
  << "This is expected to take 10+ minutes. " << logger::endL;
  theGlobalVariables.CallSystemNoOutput("make -j8", false);
  theGlobalVariables.StoreConfiguration();
}

void WebServer::CheckMongoDBSetup()
{ MacroRegisterFunctionWithName("WebServer::CheckMongoDBSetup");
#ifdef MACRO_use_MongoDB
  logServer << logger::green << "Compiled with mongo DB support. " << logger::endL;
#else
  logServer << logger::red << "Compiled without mongo DB support. " << logger::endL;
#endif
  if (theGlobalVariables.configuration["mongoDBSetup"].type != JSData::JSUndefined)
  { return;
  }
  logServer << logger::yellow << "configuration so far: " << theGlobalVariables.configuration.ToString(false);
  theGlobalVariables.configuration["mongoDBSetup"] = "Attempting";
  theGlobalVariables.StoreConfiguration();
  WebServer::FigureOutOperatingSystem();
  logServer << logger::yellow << "Mongo setup file missing, proceeding with setup. " << logger::endL;
  logServer << logger::green << "Enter the sudo password as prompted please. " << logger::endL;
  //result << logger::yellow << "(Re)-starting mongo: " << logger::endL;
  StateMaintainerCurrentFolder maintainFolder;
  theGlobalVariables.ChDir("../external-source");

  std::stringstream commandUnzipMongoC, commandUnzipLibbson;
  commandUnzipMongoC << "tar -xvzf mongo-c-driver-1.9.3.tar.gz";
  logServer << logger::green << commandUnzipMongoC.str() << logger::endL;
  logServer << theGlobalVariables.CallSystemWithOutput(commandUnzipMongoC.str());
  commandUnzipLibbson << "tar -xvzf libbson-1.9.3.tar.gz";
  logServer << logger::green << commandUnzipLibbson.str() << logger::endL;
  logServer << theGlobalVariables.CallSystemWithOutput(commandUnzipLibbson.str());

  theGlobalVariables.ChDir("./mongo-c-driver-1.9.3");
  theGlobalVariables.CallSystemNoOutput("./configure", true);
  theGlobalVariables.CallSystemNoOutput("make -j8", true);
  logServer << "Need sudo access for command: "
  << logger::red << "sudo make install" << logger::endL;
  theGlobalVariables.CallSystemNoOutput("sudo make install", true);
  theGlobalVariables.ChDir("../libbson-1.9.3");
  theGlobalVariables.CallSystemNoOutput("./configure", true);
  theGlobalVariables.CallSystemNoOutput("make -j8", true);
  logServer  << "Need sudo access for command: "
  << logger::red << "sudo make install" << logger::endL;
  theGlobalVariables.CallSystemNoOutput("sudo make install", true);
  theGlobalVariables.ChDir("../../bin");
  logServer  << "Need sudo access for command to configure linker to use local usr/local/lib path (needed by mongo): "
  << logger::red << "sudo cp ../external-source/usr_local_lib_for_mongo.conf /etc/ld.so.conf.d/" << logger::endL;
  theGlobalVariables.CallSystemNoOutput("sudo cp ../external-source/usr_local_lib_for_mongo.conf /etc/ld.so.conf.d/", true);
  logServer  << "Need sudo access for command: " << logger::red << "sudo ldconfig" << logger::endL;
  theGlobalVariables.CallSystemNoOutput("sudo ldconfig", true);
  theGlobalVariables.configuration["mongoDBSetup"] = "Setup complete";
  theGlobalVariables.StoreConfiguration();
}

void WebServer::CheckFreecalcSetup()
{ MacroRegisterFunctionWithName("WebServer::CheckFreecalcSetup");
  if (theGlobalVariables.configuration["freecalcSetup"].type != JSData::JSUndefined)
    return;
  theGlobalVariables.configuration["freecalcSetup"] = "Setup started";
  theGlobalVariables.StoreConfiguration();
  WebServer::FigureOutOperatingSystem();
  logServer << logger::yellow << "Freelcalc setup file missing, proceeding to set it up. " << logger::endL;
  StateMaintainerCurrentFolder preserveFolder;
  std::string startingDir = FileOperations::GetCurrentFolder();
  theGlobalVariables.ChDir(theGlobalVariables.PhysicalPathProjectBase + "../");
  logServer << logger::green << "Current folder: " << FileOperations::GetCurrentFolder() << logger::endL;
  logServer << logger::green << "git clone https://github.com/tmilev/freecalc.git" << logger::endL;
  theGlobalVariables.CallSystemNoOutput("git clone https://github.com/tmilev/freecalc.git", true);
  theGlobalVariables.configuration["freecalcSetup"] = "Setup complete";
  theGlobalVariables.StoreConfiguration();
}

void WebServer::CheckSVNSetup()
{ MacroRegisterFunctionWithName("WebServer::CheckSVNSetup");
  if (theGlobalVariables.configuration["attemptedSVNSetup"].type != JSData::JSUndefined)
    return;
  theGlobalVariables.configuration["attemptedSVNSetup"] = "Attempted to install";
  theGlobalVariables.StoreConfiguration();
  WebServer::FigureOutOperatingSystem();
  logServer << logger::yellow << "SVN setup file missing, proceeding to set it up. " << logger::endL
  << logger::green << "Enter your the sudo password as prompted please. " << logger::endL;
  if (theGlobalVariables.OperatingSystem == "Ubuntu")
  { logServer << logger::yellow << "sudo apt-get install subversion" << logger::endL;
    theGlobalVariables.CallSystemNoOutput("sudo apt-get install subversion", false);
    theGlobalVariables.configuration["attemptedSVNSetup"] = "Attempted to install on Ubuntu. ";
  } else if (theGlobalVariables.OperatingSystem == "CentOS")
  { logServer << logger::yellow << "sudo yum install subversion" << logger::endL;
    theGlobalVariables.CallSystemNoOutput("sudo yum install subversion", false);
    theGlobalVariables.configuration["attemptedSVNSetup"] = "Attempted to install on CentOS. ";
  }
  theGlobalVariables.StoreConfiguration();
}

void WebServer::CheckMathJaxSetup()
{ MacroRegisterFunctionWithName("WebServer::CheckMathJaxSetup");
  if (theGlobalVariables.configuration["MathJax"].type != JSData::JSUndefined)
    return;
  theGlobalVariables.configuration["MathJax"] = "Attempting to install";
  theGlobalVariables.StoreConfiguration();
  WebServer::FigureOutOperatingSystem();
  logServer << logger::yellow << "MathJax setup file missing, proceeding to set it up. " << logger::endL
  << logger::green << "Enter your the sudo password as prompted please. " << logger::endL;
  if (theGlobalVariables.OperatingSystem == "Ubuntu")
    theGlobalVariables.CallSystemNoOutput("sudo apt-get install unzip", false);
  else if (theGlobalVariables.OperatingSystem == "CentOS")
    theGlobalVariables.CallSystemNoOutput("sudo yum install unzip", false);
  logServer << "Proceeding to unzip MathJax. ";
  //std::string currentFolder =FileOperations::GetCurrentFolder();
  //result << "Changing folder to: " << theGlobalVariables.PhysicalPathProjectBase << logger::endL;
  //theGlobalVariables.ChDir(theGlobalVariables.PhysicalPathProjectBase);
  logServer << "Current folder: " << FileOperations::GetCurrentFolder() << logger::endL;
  theGlobalVariables.CallSystemNoOutput("mkdir -p ../../public_html", false);
  theGlobalVariables.CallSystemNoOutput("unzip ./../html-common/MathJax-2.7-latest.zip -d ./../../public_html/", false);
  theGlobalVariables.CallSystemNoOutput("mv ./../../public_html/MathJax-2.7.2 ./../../public_html/MathJax-2.7-latest", false);
  theGlobalVariables.configuration["MathJax"] = "Unzipped";
  theGlobalVariables.StoreConfiguration();
}

void WebServer::AnalyzeMainArguments(int argC, char **argv)
{ MacroRegisterFunctionWithName("WebServer::AnalyzeMainArguments");
  if (argC < 0)
    argC = 0;
  theGlobalVariables.programArguments.SetSize(argC);
  //std::cout << "RAND_MAX: " << RAND_MAX;
  //std::cout << "Program arguments: \n";
  for (int i = 0; i < argC; i ++)
  { theGlobalVariables.programArguments[i] = argv[i];
    //std::cout << "Argument " << i + 1 << ": " << theGlobalVariables.programArguments[i] << "\n";
  }
  ////////////////////////////////////////////////////
  theGlobalVariables.flagRunningAce = true;
  theGlobalVariables.flagRunningCommandLine = false;
  theGlobalVariables.flagRunningConsoleTest = false;
  theGlobalVariables.flagRunningApache = false;
  #ifdef MACRO_use_open_ssl
  theGlobalVariables.flagSSLisAvailable = true;
  #endif
  theGlobalVariables.flagRunningBuiltInWebServer = false;
  ////////////////////////////////////////////////////
  if (argC == 0)
  { theGlobalVariables.flagRunningCommandLine = true;
    return;
  }
  theGlobalVariables.initDefaultFolderAndFileNames(theGlobalVariables.programArguments[0]);
  std::string envRequestMethodApache = WebServer::GetEnvironment("REQUEST_METHOD");
  if (envRequestMethodApache == "GET" || envRequestMethodApache == "POST" || envRequestMethodApache == "HEAD")
  { theGlobalVariables.flagRunningApache = true;
    return;
  }
  if (argC < 2)
  { if (!theGlobalVariables.flagRunningApache)
      theGlobalVariables.flagRunningCommandLine = true;
    return;
  }
  std::string& secondArgument = theGlobalVariables.programArguments[1];
  if (secondArgument == "server" || secondArgument == "server8080")
  { if (secondArgument == "server8080")
      theWebServer.flagPort8155 = false;
    theGlobalVariables.flagRunningBuiltInWebServer = true;
    theGlobalVariables.flagRunningAce = false;
    theWebServer.flagTryToKillOlderProcesses = true;
    if (argC == 2)
      return;
    std::string& thirdArgument = theGlobalVariables.programArguments[2];
    std::string timeLimitString = "100";
    std::string killOrder = "";
    if (thirdArgument == "nokill")
      killOrder = "nokill";
    else
      timeLimitString = thirdArgument;
    if (argC >= 4 && killOrder == "nokill")
      timeLimitString = theGlobalVariables.programArguments[3];
    theWebServer.flagTryToKillOlderProcesses = !(killOrder == "nokill");
    Rational timeLimit;
    timeLimit.AssignString(timeLimitString);
    int timeLimitInt = 0;
    if (timeLimit.IsIntegerFittingInInt(&timeLimitInt))
    { theGlobalVariables.MaxComputationMilliseconds = timeLimitInt;
      if (theGlobalVariables.MaxComputationMilliseconds <= 0)
        theGlobalVariables.MaxComputationMilliseconds = 0;
      else
        theGlobalVariables.MaxComputationMilliseconds *= 1000;
      std::cout << "Max computation time: " << theGlobalVariables.MaxComputationMilliseconds << " ms." << std::endl;
    }
    return;
  }
  ////////////////////////////////
  if (secondArgument == "test")
  { theGlobalVariables.flagRunningConsoleTest = true;
    return;
  }
  theGlobalVariables.flagRunningCommandLine = true;
}

void WebServer::InitializeGlobalVariablesHashes()
{
  StateMaintainerCurrentFolder preserveCurrentFolder;

  std::string theDir = FileOperations::GetCurrentFolder();
  theGlobalVariables.ChDir("../");
  if (theGlobalVariables.buildVersionSimple == "")
    theGlobalVariables.buildVersionSimple =
    MathRoutines::StringTrimWhiteSpace(theGlobalVariables.CallSystemWithOutput("git rev-list --count HEAD"));
  theGlobalVariables.buildVersionSimple = MathRoutines::StringTrimWhiteSpace(theGlobalVariables.buildVersionSimple);
  for (unsigned i = 0; i < theGlobalVariables.buildVersionSimple.size(); i ++)
    if (MathRoutines::isALatinLetter(theGlobalVariables.buildVersionSimple[i]))
    { theGlobalVariables.buildVersionSimple = "?";
      break;
    }
  theGlobalVariables.buildHeadHashWithServerTime = MathRoutines::StringTrimWhiteSpace
  (theGlobalVariables.CallSystemWithOutput("git rev-parse HEAD"));
  for (unsigned i = 0; i < theGlobalVariables.buildHeadHashWithServerTime.size(); i ++)
    if (!MathRoutines::IsAHexDigit(theGlobalVariables.buildHeadHashWithServerTime[i]))
    { theGlobalVariables.buildHeadHashWithServerTime = "x";
      break;
    }
  std::stringstream buildHeadHashWithTimeStream;
  buildHeadHashWithTimeStream << theGlobalVariables.buildHeadHashWithServerTime
  << theGlobalVariables.GetGlobalTimeInSeconds();
  theGlobalVariables.buildHeadHashWithServerTime = buildHeadHashWithTimeStream.str();
  theGlobalVariables.ChDir(theDir);

  FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash().AddOnTopNoRepetitionMustBeNewCrashIfNot
  (WebAPI::request::calculatorHTML);
  WebAPI::request::onePageJSWithHash = FileOperations::GetVirtualNameWithHash(WebAPI::request::onePageJS);
}

void WebServer::InitializeGlobalVariables()
{ theGlobalVariables.takeActionAfterComputationMilliseconds = 0;
  theGlobalVariables.flagReportEverything = true;
  ParallelComputing::cgiLimitRAMuseNumPointersInList = 4000000000;
  this->InitializeGlobalVariablesHashes();
  this->requestsNotNeedingLogin.AddOnTop("about");
  this->requestsNotNeedingLogin.AddOnTop("signUp");
  this->requestsNotNeedingLogin.AddOnTop("signUpPage");
  this->requestsNotNeedingLogin.AddOnTop("forgotLogin");
  this->requestsNotNeedingLogin.AddOnTop("forgotLoginPage");
  this->requestsNotNeedingLogin.AddOnTop("compute");
  this->requestsNotNeedingLogin.AddOnTop("calculator");
  this->requestsNotNeedingLogin.AddOnTop(WebAPI::request::examplesJSON);
  this->requestsNotNeedingLogin.AddOnTop("exerciseNoLogin");
  this->requestsNotNeedingLogin.AddOnTop("exerciseJSON");
  this->requestsNotNeedingLogin.AddOnTop("templateNoLogin");
  this->requestsNotNeedingLogin.AddOnTop("templateJSONNoLogin");
  this->requestsNotNeedingLogin.AddOnTop("topicListJSONNoLogin");
  this->requestsNotNeedingLogin.AddOnTop("submitExerciseNoLogin");
  this->requestsNotNeedingLogin.AddOnTop("submitExercisePreviewNoLogin");
  this->requestsNotNeedingLogin.AddOnTop("problemGiveUpNoLogin");
  this->requestsNotNeedingLogin.AddOnTop("problemSolutionNoLogin");
  this->requestsNotNeedingLogin.AddOnTop(WebAPI::request::selectCourseJSON);
  this->requestsNotNeedingLogin.AddOnTop("slidesFromSource");
  this->requestsNotNeedingLogin.AddOnTop("homeworkFromSource");
  this->requestsNotNeedingLogin.AddOnTop("slidesSource");
  this->requestsNotNeedingLogin.AddOnTop("homeworkSource");
  this->requestsNotNeedingLogin.AddOnTop(WebAPI::app);
  this->requestsNotNeedingLogin.AddOnTop(WebAPI::appNoCache);
  this->requestsNotNeedingLogin.AddOnTop(WebAPI::request::userInfoJSON);
  this->requestsNotNeedingLogin.AddOnTop(WebAPI::request::serverStatusJSON);

  this->addressStartsNotNeedingLogin.AddOnTop("favicon.ico");
  this->addressStartsNotNeedingLogin.AddOnTop("/favicon.ico");
  this->addressStartsNotNeedingLogin.AddOnTop("/html-common/");
  this->addressStartsNotNeedingLogin.AddOnTop("/calculator-html/");
  this->addressStartsNotNeedingLogin.AddOnTop("/css/");
  this->addressStartsNotNeedingLogin.AddOnTop("/javascriptlibs/");
  this->addressStartsNotNeedingLogin.AddOnTop("/MathJax-2.7-latest/");
  this->addressStartsNotNeedingLogin.AddOnTop("/login");
  this->addressStartsNotNeedingLogin.AddOnTop("/" + WebAPI::app);
  this->addressStartsNotNeedingLogin.AddOnTop("/" + WebAPI::appNoCache);
  this->addressStartsNotNeedingLogin.AddOnTop(WebAPI::request::onePageJS);


  this->addressStartsInterpretedAsCalculatorRequest.AddOnTop("/" + WebAPI::app);
  this->addressStartsInterpretedAsCalculatorRequest.AddOnTop(WebAPI::app);
  this->addressStartsInterpretedAsCalculatorRequest.AddOnTop("/" + WebAPI::appNoCache);
  this->addressStartsInterpretedAsCalculatorRequest.AddOnTop(WebAPI::appNoCache);
  this->addressStartsInterpretedAsCalculatorRequest.AddOnTop(WebAPI::request::onePageJS);
  this->addressStartsInterpretedAsCalculatorRequest.AddOnTop(WebAPI::request::onePageJSWithHash);
  //NO! Adding "logout", for example: this->addressStartsNotNeedingLogin.AddOnTop("logout");
  //is FORBIDDEN! Logging someone out definitely requires authentication (imagine someone
  //asking for logouts on your account once every second: this would be fatal as proper logout resets
  //the authentication tokens.

  MapLisT<std::string, std::string, MathRoutines::hashString>&
  folderSubstitutionsNonSensitive = FileOperations::FolderVirtualLinksNonSensitive();
  MapLisT<std::string, std::string, MathRoutines::hashString>&
  folderSubstitutionsSensitive = FileOperations::FolderVirtualLinksSensitive();
  FileOperations::FolderVirtualLinksULTRASensitive(); //<- allocates data structure
  folderSubstitutionsNonSensitive.Clear();
  this->addressStartsNotNeedingLogin.AddListOnTop
  (FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash());

  this->addressStartsSentWithCacheMaxAge.AddOnTop("/MathJax-2.7-latest/");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("/html-common/");
  for (int i = 0; i < FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash().size; i ++)
    this->addressStartsSentWithCacheMaxAge.AddOnTop
    (FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash()[i] +
     theGlobalVariables.buildHeadHashWithServerTime);
  //Warning: order of substitutions is important. Only the first rule that applies is applied, only once.
  //No further rules are applied after that.
  folderSubstitutionsNonSensitive.SetKeyValue("/output/", "output/");//<-coming from webserver
  folderSubstitutionsNonSensitive.SetKeyValue("output/", "output/");//<-internal use
  folderSubstitutionsNonSensitive.SetKeyValue("/certificates-public/", "certificates-public/");//<-coming from webserver
  folderSubstitutionsNonSensitive.SetKeyValue("certificates-public/", "certificates-public/");//<-internal use
  folderSubstitutionsNonSensitive.SetKeyValue("problems/", "../problems/");
  folderSubstitutionsNonSensitive.SetKeyValue("/problems/", "../problems/");

  folderSubstitutionsNonSensitive.SetKeyValue("/html/", "../public_html/");//<-coming from webserver

  folderSubstitutionsNonSensitive.SetKeyValue("/calculator-html/", "./calculator-html/");//<-coming from webserver
  folderSubstitutionsNonSensitive.SetKeyValue("/font/", "./html-common/fonts/");
  folderSubstitutionsNonSensitive.SetKeyValue("/fonts/", "./html-common/fonts/");
  folderSubstitutionsNonSensitive.SetKeyValue("/html-common/font/", "./html-common/fonts/");
  folderSubstitutionsNonSensitive.SetKeyValue("/html-common/", "./html-common/");//<-coming from webserver

  folderSubstitutionsNonSensitive.SetKeyValue("DefaultProblemLocation/", "../problems/");//<-internal use
  //referred to by site:
  folderSubstitutionsNonSensitive.SetKeyValue("coursetemplates/", "../coursetemplates/");
  folderSubstitutionsNonSensitive.SetKeyValue("/coursesavailable/", "../coursesavailable/"); //<-web server
  folderSubstitutionsNonSensitive.SetKeyValue("topiclists/", "../topiclists/");

  folderSubstitutionsNonSensitive.SetKeyValue
  ("/MathJax-2.7-latest/config/mathjax-calculator-setup.js", "./calculator-html/mathjax-calculator-setup.js");//<-coming from web server
  folderSubstitutionsNonSensitive.SetKeyValue("/MathJax-2.7-latest/", "../public_html/MathJax-2.7-latest/");//<-coming from webserver

  folderSubstitutionsNonSensitive.SetKeyValue("/LaTeX-materials/", "../LaTeX-materials/");
  //referred to by site:
  folderSubstitutionsNonSensitive.SetKeyValue("LaTeX-materials/", "../LaTeX-materials/");
  folderSubstitutionsNonSensitive.SetKeyValue("/freecalc/", "../freecalc/");
  //referred to by site:
  folderSubstitutionsNonSensitive.SetKeyValue("freecalc/", "../freecalc/");
  folderSubstitutionsNonSensitive.SetKeyValue("/slides-video/", "../slides-video/");
  folderSubstitutionsNonSensitive.SetKeyValue("slides-video/", "../slides-video/");

  folderSubstitutionsSensitive.Clear();
  folderSubstitutionsSensitive.SetKeyValue("LogFiles/", "LogFiles/");//<-internal use
  folderSubstitutionsSensitive.SetKeyValue("/LogFiles/", "LogFiles/");//<-coming from webserver
  folderSubstitutionsSensitive.SetKeyValue("configuration/", "configuration/");//<-internal use
  folderSubstitutionsSensitive.SetKeyValue("/configuration/", "configuration/");//<-coming from webserver

  folderSubstitutionsSensitive.SetKeyValue("/crashes/", "./LogFiles/crashes/");
  folderSubstitutionsSensitive.SetKeyValue("crashes/", "./LogFiles/crashes/");

  FileOperations::FilesStartsToWhichWeAppendHostName().AddOnTop("favicon.ico");
  FileOperations::FilesStartsToWhichWeAppendHostName().AddOnTop("/favicon.ico");
  FileOperations::FilesStartsToWhichWeAppendHostName().AddOnTop("/coursesavailable/default/default.txt");
  FileOperations::FilesStartsToWhichWeAppendHostName().AddOnTop("/calculator-html/about.html");
  FileOperations::FilesStartsToWhichWeAppendHostName().AddOnTop("/html/about.html");

  FileOperations::FolderStartsToWhichWeAppendInstructorUsernameSlash().AddOnTop("topiclists/");
  FileOperations::FolderStartsToWhichWeAppendInstructorUsernameSlash().AddOnTop("coursetemplates/");
  FileOperations::FolderStartsToWhichWeAppendInstructorUsernameSlash().AddOnTop("/coursesavailable/");
  FileOperations::FolderStartsToWhichWeAppendInstructorUsernameSlash().AddOnTop("problems/");
  FileOperations::FolderStartsToWhichWeAppendInstructorUsernameSlash().AddOnTop("DefaultProblemLocation/");
}

int main(int argc, char **argv)
{ return WebServer::main(argc, argv);
}

extern int mainTest(List<std::string>& remainingArgs);

void WebServer::ToggleProcessMonitoring()
{ MacroRegisterFunctionWithName("WebServer::ToggleProcessMonitoring");
  if (theGlobalVariables.flagAllowProcessMonitoring)
    WebServer::TurnProcessMonitoringOff();
  else
    WebServer::TurnProcessMonitoringOn();
}

void WebServer::TurnProcessMonitoringOn()
{ MacroRegisterFunctionWithName("WebServer::TurnProcessMonitoringOn");
  logServer
  << logger::purple << "************************" << logger::endL
  << logger::red << "WARNING: process monitoring IS ON. " << logger::endL
  << logger::purple << "************************" << logger::endL;
  theGlobalVariables.flagAllowProcessMonitoring = true;
  theGlobalVariables.takeActionAfterComputationMilliseconds = 5000; //5 seconds
}

void WebServer::TurnProcessMonitoringOff()
{ MacroRegisterFunctionWithName("WebServer::TurnProcessMonitoringOn");
  logServer
  << logger::green << "************************" << logger::endL
  << logger::green << "Process monitoring is now off. " << logger::endL
  << logger::green << "************************" << logger::endL;
  theGlobalVariables.flagAllowProcessMonitoring = false;
  theGlobalVariables.takeActionAfterComputationMilliseconds = 0;
}

bool GlobalVariables::LoadConfiguration()
{ MacroRegisterFunctionWithName("GlobalVariables::LoadConfiguration");
  std::string configuration;
  std::stringstream out;
  bool createConfiguration = false;
  //std::cout << "DEBUG: loading config:" << std::endl;
  if (FileOperations::LoadFileToStringVirtual("/configuration/configuration.json", configuration, true, false, &out))
  { if (!theGlobalVariables.configuration.readstring(configuration, false, &out))
    { logServer << logger::red << out.str() << logger::endL;
      createConfiguration = true;
    }
  } else
  { createConfiguration = true;
    //std::cout << "DEBUG: no congig:" << out.str() << std::endl;
    logServer << logger::yellow << out.str() << logger::endL;
  }
  if (createConfiguration)
  { logServer << logger::yellow << "Did not find configuration file." << logger::endL;
    this->StoreConfiguration();
    return false;
  }
  //logServer << logger::green << "Successfully loaded configuration: "
  //<< theGlobalVariables.configuration.ToString(false) << logger::endL;
  return true;
}

bool GlobalVariables::StoreConfiguration()
{ MacroRegisterFunctionWithName("GlobalVariables::StoreConfiguration");
  std::fstream configurationFile;
  if (!FileOperations::OpenFileCreateIfNotPresentVirtual
      (configurationFile, "configuration/configuration.json", false, true, false, true))
  { logServer << logger::red << "Could not open file: /configuration/configuration.json" << logger::endL;
    return false;
  }
  configurationFile << theGlobalVariables.configuration.ToString(false);
  return true;
}

void GlobalVariables::ComputeConfigurationFlags()
{ MacroRegisterFunctionWithName("GlobalVariables::ComputeConfigurationFlags");
  theGlobalVariables.flagServerDetailedLog = theGlobalVariables.configuration["serverDetailedLog"].isTrueRepresentationInJSON();
  if (theGlobalVariables.flagServerDetailedLog)
  { logServer
    << logger::purple << "************************" << logger::endL
    << logger::purple << "************************" << logger::endL
    << logger::purple << "************************" << logger::endL
    << logger::red << "WARNING: DETAILED server logging is on. " << logger::endL
    << "This is strictly for development purposes, please do not deploy on live systems. " << logger::endL
    << "To turn off/on server logging: set serverDetailedLog to false in the file configuration/configuration.json." << logger::endL
    << logger::purple << "************************" << logger::endL
    << logger::purple << "************************" << logger::endL
    << logger::purple << "************************" << logger::endL;
  }
  theGlobalVariables.flagCachingInternalFilesOn = !theGlobalVariables.configuration["serverRAMCachingOff"].isTrueRepresentationInJSON();
  if (!theGlobalVariables.flagCachingInternalFilesOn && theGlobalVariables.flagRunningBuiltInWebServer)
  { logServer
    << logger::purple << "************************" << logger::endL
    << logger::red << "WARNING: caching files is off. " << logger::endL
    << "This is for development purposes only, please do not deploy on live systems. " << logger::endL
    << "To turn off/on modify variable serverRAMCachingOff in configuration/configuration.json." << logger::endL
    << logger::purple << "************************" << logger::endL;
  }
  theGlobalVariables.flagRunServerOnEmptyCommandLine = theGlobalVariables.configuration["runServerOnEmptyCommandLine"].isTrueRepresentationInJSON();
  if (theGlobalVariables.flagRunningCommandLine &&
      theGlobalVariables.programArguments.size == 1 &&
      theGlobalVariables.flagRunServerOnEmptyCommandLine )
  { logServer << logger::green
    << "runServerOnEmptyCommandLine is set to true => Starting server with default configuration. "
    << logger::endL;
    theGlobalVariables.flagRunningBuiltInWebServer = true;
    theGlobalVariables.flagRunningCommandLine = false;
    theGlobalVariables.flagRunningApache = false;
    theGlobalVariables.MaxComputationMilliseconds = 30000; // 30 seconds, default
    theWebServer.flagTryToKillOlderProcesses = false;
    theWebServer.flagPort8155 = true;
  }
}

int WebServer::main(int argc, char **argv)
{ theGlobalVariables.InitThreadsExecutableStart();
  //use of loggers forbidden before calling   theWebServer.AnalyzeMainArguments(...):
  //we need to initialize first the folder locations relative to the executable.
  MacroRegisterFunctionWithName("main");
  try
  { InitializeGlobalObjects();
    theWebServer.AnalyzeMainArguments(argc, argv);
    theGlobalVariables.LoadConfiguration();
    if (theGlobalVariables.MaxComputationMilliseconds > 0)
      theGlobalVariables.MaxTimeNoPingBeforeChildIsPresumedDead =
      theGlobalVariables.MaxComputationMilliseconds + 20000; // + 20 seconds
    else
      theGlobalVariables.MaxTimeNoPingBeforeChildIsPresumedDead = - 1;
    //using loggers allowed from now on.
    theWebServer.InitializeGlobalVariables();
    theGlobalVariables.ComputeConfigurationFlags();


    theWebServer.CheckSystemInstallationOpenSSL();
    theWebServer.CheckSystemInstallationMongoDB();
    theWebServer.CheckMongoDBSetup();
    theWebServer.CheckMathJaxSetup();
    theWebServer.CheckSVNSetup();
    theWebServer.CheckFreecalcSetup();

    theGlobalVariables.flagAceIsAvailable = FileOperations::FileExistsVirtual("/MathJax-2.7-latest/", false);
    if (!theGlobalVariables.flagAceIsAvailable && theGlobalVariables.flagRunningBuiltInWebServer)
      logServer << logger::red << "MathJax not available. " << logger::endL;
  //  if (false &&
  //      theGlobalVariables.flagRunningBuiltInWebServer)
  //  { theWebServer.TurnProcessMonitoringOn();
  //  }
    if (theGlobalVariables.flagRunningBuiltInWebServer)
    { if (theGlobalVariables.MaxComputationMilliseconds <= 0)
        logServer
        << logger::purple << "************************" << logger::endL
        << logger::red << "WARNING: no computation time limit set. " << logger::endL
        << logger::purple << "************************" << logger::endL;
      if (theGlobalVariables.MaxComputationMilliseconds > 500000)
        logServer
        << logger::purple << "************************" << logger::endL
        << logger::red << "WARNING: computation time limit is high: "
        << (theGlobalVariables.MaxComputationMilliseconds / 1000)
        << " seconds. " << logger::endL
        << logger::purple << "************************" << logger::endL;
    }
    //logServer << "DEBUG: got to here pt 3" << logger::endL;
    if (theGlobalVariables.flagRunningConsoleTest)
      return mainTest(theGlobalVariables.programArguments);
    if (theGlobalVariables.flagRunningApache)
      return WebServer::mainApache();
    if (theGlobalVariables.flagRunningBuiltInWebServer)
      return theWebServer.Run();
    if (theGlobalVariables.flagRunningCommandLine)
      return WebServer::mainCommandLine();
  }
  catch (...)
  { crash << "Exception caught: something very wrong has happened. " << crash;
  }
  crash << "This point of code is not supposed to be reachable. " << crash;
  return - 1;
}

int WebServer::mainCommandLine()
{ MacroRegisterFunctionWithName("main_command_input");
  theGlobalVariables.IndicatorStringOutputFunction = HtmlRoutines::MakeStdCoutReport;
  //  stOutput << "\n\n\n" << theParser.DisplayPathServerBase << "\n\n";
  //  return 0;
//std::cout << "Running cmd line. \n";
  //logServer << "DEBUG: got to here pt 5" << logger::endL;
  PointerObjectDestroyer<Calculator> theDestroyer(theParser);
  theParser = new Calculator;
  theParser->init();
  //logServer << "DEBUG: got to here pt 5.3" << logger::endL;
  logger result("", 0, false, "server");
  //logServer << "DEBUG: got to here pt 5.5" << logger::endL;
  if (theGlobalVariables.programArguments.size > 1)
    for (int i = 1; i < theGlobalVariables.programArguments.size; i ++)
    { theParser->inputString += theGlobalVariables.programArguments[i];
      if (i != theGlobalVariables.programArguments.size - 1)
        theParser->inputString += " ";
    }
  else
  { result << "Input: " << logger::yellow;
    std::cin >> theParser->inputString;
  }
  theParser->flagUseHtml = false;
  //logServer << "DEBUG: got to here pt 6" << logger::endL;
  theParser->Evaluate(theParser->inputString);
  std::fstream outputFile;
  std::string outputFileName;
  if (!FileOperations::GetPhysicalFileNameFromVirtual
      ("output/outputFileCommandLine.html", outputFileName, false, false, 0))
  { outputFileName = "Failed to extract output file from output/outputFileCommandLine.html";
  }
  FileOperations::OpenFileCreateIfNotPresentVirtual
  (outputFile, "output/outputFileCommandLine.html", false, true, false);
  result << theParser->outputString;
  outputFile << theParser->outputString;
  result << "\nTotal running time: " << logger::blue << theGlobalVariables.GetElapsedMilliseconds() << " ms. "
  << logger::endL
  << "Output written in: " << logger::green << outputFileName << logger::endL << "\n";
  return 0;
}

std::string WebServer::GetEnvironment(const std::string& envVarName)
{ char* queryStringPtr = getenv(envVarName.c_str());
  if (queryStringPtr == 0)
    return "";
  return queryStringPtr;

}

#include <sys/resource.h> //<- for setrlimit(...) function. Restricts the time the executable can run.
int WebServer::mainApache()
{ MacroRegisterFunctionWithName("main_apache");
  rlimit theLimit;
  theLimit.rlim_cur = 30;
  theLimit.rlim_max = 60;
  setrlimit(RLIMIT_CPU, &theLimit);
  stOutput.theOutputFunction = 0;
  //stOutput << "Content-Type: text/html\r\nSet-cookie: test =1;\r\n\r\nThe output bytes start here:\n";
  theGlobalVariables.IndicatorStringOutputFunction = 0;
  theGlobalVariables.flagServerForkedIntoWorker = true;
  theGlobalVariables.flagComputationStarted = true;
//  stOutput << "<hr>First line<hr>";
  theGlobalVariables.MaxComputationMilliseconds = 30000; //<-30 second computation time restriction!
  theWebServer.initPrepareSignals();
  CreateTimerThread();
  theWebServer.CreateNewActiveWorker();
  WebWorker& theWorker = theWebServer.GetActiveWorker();
  PointerObjectDestroyer<Calculator> theDestroyer(theParser);
  theParser = new Calculator;
  theParser->init();
  std::cin >> theWorker.messageBody;
  theWebServer.httpSSLPort = "443";
  theWebServer.httpPort = "80";
  std::string theRequestMethod = WebServer::GetEnvironment("REQUEST_METHOD");
  theWorker.cookiesApache = WebServer::GetEnvironment("HTTP_COOKIE");
  std::string thePort = WebServer::GetEnvironment("SERVER_PORT");
  theGlobalVariables.IPAdressCaller = WebServer::GetEnvironment("REMOTE_ADDR");
  theWorker.hostWithPort = WebServer::GetEnvironment("SERVER_NAME") + ":" + thePort;
  theGlobalVariables.hostWithPort = theWorker.hostWithPort;
  theGlobalVariables.hostNoPort = theWorker.hostNoPort;
  std::string theURL = WebServer::GetEnvironment("REQUEST_URI");
  unsigned numBytesBeforeQuestionMark = 0;
  for (numBytesBeforeQuestionMark = 0; numBytesBeforeQuestionMark < theURL.size(); numBytesBeforeQuestionMark++)
    if (theURL[numBytesBeforeQuestionMark] == '?')
      break;
  theGlobalVariables.DisplayNameExecutable = theURL.substr(0, numBytesBeforeQuestionMark);
  theWorker.addressGetOrPost = theGlobalVariables.DisplayNameExecutable +
  "?" + WebServer::GetEnvironment("QUERY_STRING");

  if (thePort == "443")
  { theGlobalVariables.flagUsingSSLinCurrentConnection = true;
    theGlobalVariables.flagSSLisAvailable = true;
  }
  if (theRequestMethod == "GET")
    theWorker.requestTypE = theWorker.requestGet;
  if (theRequestMethod == "POST")
    theWorker.requestTypE = theWorker.requestPost;
  theGlobalVariables.flagComputationCompletE = true;
  MathRoutines::StringSplitExcludeDelimiter(theWorker.cookiesApache, ' ', theWorker.cookies);
  for (int i = 0; i < theWorker.cookies.size; i ++)
    theWorker.cookies[i] = MathRoutines::StringTrimWhiteSpace(theWorker.cookies[i]);
/*  stOutput << "<html><body>";
  stOutput << "DEBUG: your input, bounced back: "
  << "\n<hr>server port: <br>\n" << thePort
  << "\n<hr>Server base: <br>\n" << theGlobalVariables.PhysicalPathProjectBase
  << "\n<hr>Calculator display name apache: <br>\n" << theGlobalVariables.DisplayNameCalculatorApache
  << "\n<hr>Cookies: <br>\n" << theWorker.cookiesApache;
  for (int i = 0; i < theWorker.cookies.size; i ++)
    stOutput << "<br>\n" << theWorker.cookies[i] << "\n";
  stOutput << "\n<hr>query string:<br>\n" << theWorker.addressGetOrPost
  << "\n<hr>message body:<br>\n" << theWorker.messageBody
  << "\n<hr>request method:<br>\n" << theRequestMethod
  << "\n</body></html>";
  */
  theWorker.ServeClient();
//  std::string theOutputHeader(theWorker.remainingHeaderToSend.TheObjects, theWorker.remainingHeaderToSend.size);
  //std::string theOutputBody(theWorker.remainingBodyToSend.TheObjects, theWorker.remainingBodyToSend.size);
  //stOutput << theOutputBody;
  theGlobalVariables.flagComputationFinishedAllOutputSentClosing = true;
  return 0;
}

std::string HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable()
{ MacroRegisterFunctionWithName("WebWorker::ToStringCalculatorArgumentsHumanReadable");
  if (!theGlobalVariables.UserDebugFlagOn())
    return "";
  std::stringstream out;
  out << "<hr>\n";
  out << "Default user: " << HtmlRoutines::ConvertStringToHtmlString(theGlobalVariables.userDefault.username, false);
  if (theGlobalVariables.flagLoggedIn)
    out << "\n<br>\nLogged in.";
  out << "\n<br>\nAddress:\n" << HtmlRoutines::ConvertStringToHtmlString(theWebServer.GetActiveWorker().addressComputed, true);
  out << "\n<br>\nRequest:\n" << theGlobalVariables.userCalculatorRequestType;
  if (theGlobalVariables.UserDefaultHasAdminRights())
    out << "\n<br>\n<b>User has admin rights</b>";
  if (theWebServer.RequiresLogin(theGlobalVariables.userCalculatorRequestType, theWebServer.GetActiveWorker().addressComputed))
    out << "\n<br>\nAddress requires login. ";
  else
    out << "\n<br>\nAddress <b>does not</b> require any login. ";
  out << "\n<hr>\n";
  for (int i = 0; i < theGlobalVariables.webArguments.size(); i ++)
  { out << theGlobalVariables.webArguments.theKeys[i] << ": "
    << HtmlRoutines::ConvertStringToHtmlString(theGlobalVariables.webArguments[i], true);
    if (i != theGlobalVariables.webArguments.size() - 1)
      out << "\n<br>\n";
  }
  out << "\n<hr>\n";
  if (theGlobalVariables.flagRunningBuiltInWebServer)
  { out << "Sent with max cache: "
    << theWebServer.addressStartsSentWithCacheMaxAge.ToStringCommaDelimited()
    << "<hr>";
    out << theWebServer.GetActiveWorker().ToStringMessageUnsafe();
  }
  return out.str();
}

void WebWorker::PrepareFullMessageHeaderAndFooter()
{ MacroRegisterFunctionWithName("WebWorker::PrepareFullMessageHeaderAndFooter");
  this->remainingBytesToSenD.SetSize(0);
  this->remainingBytesToSenD.SetExpectedSize(this->remainingBodyToSend.size + this->remainingHeaderToSend.size + 30);
  if (this->remainingHeaderToSend.size == 0)
  { if (this->requestTypE != this->requestHead)
      this->remainingBytesToSenD = this->remainingBodyToSend;
    this->remainingBodyToSend.SetSize(0);
    return;
  }
  this->remainingBytesToSenD = this->remainingHeaderToSend;
  this->remainingHeaderToSend.SetSize(0);
  std::stringstream contentLengthStream;
//  double fixme;
//  if (false)
  if (this->flagDoAddContentLength)
    contentLengthStream << "Content-Length: " << this->remainingBodyToSend.size << "\r\n";
  contentLengthStream << "\r\n";
  std::string contentLengthString = contentLengthStream.str();
  for (unsigned i = 0; i < contentLengthString.size(); i ++)
    this->remainingBytesToSenD.AddOnTop(contentLengthString[i]);
  //std::string debugString(this->remainingBytesToSenD.TheObjects, this->remainingBytesToSenD.size);
  //std::cout << "DEBUG: headers+ body ="
  //<< this->remainingBytesToSenD.size << " + "
  //<< this->remainingBodyToSend.size << "\n" << debugString;
  if (this->requestTypE != this->requestHead)
    this->remainingBytesToSenD.AddListOnTop(this->remainingBodyToSend);
  this->remainingBodyToSend.SetSize(0);
}

void WebWorker::SendAllBytesWithHeaders()
{ MacroRegisterFunctionWithName("WebWorker::SendAllBytesWithHeaders");
//  static bool calledOnce = false;
//  if (calledOnce)
//  { crash << "WebWorker::SendAllBytesNoHeaders called more than once" << crash;
//  }
//  calledOnce = true;
  this->PrepareFullMessageHeaderAndFooter();
  //std::string tempS(this->remainingBytesToSenD.TheObjects, this->remainingBytesToSenD.size);
  //logWorker << logger::red << "DEBUG Message:\n" << logger::normalColor << tempS << logger::endL;
  this->SendAllBytesNoHeaders();
}

void WebWorker::SendAllBytesHttp()
{ MacroRegisterFunctionWithName("WebWorker::SendAllBytesHttp");
  if (this->remainingBytesToSenD.size == 0)
    return;
  this->CheckConsistency();
//  firstCallStream << "WebWorker::SendAllBytesHttp called once. Input: \n "
//  << this->ToStringMessageFullUnsafe() << "\nStack trace:\n"
//  << crash.GetStackTraceEtcErrorMessage() ;
//  if (this->flagDidSendAll)
//  { static std::stringstream firstCallStream;
//    firstCallStream
//    << "\n<br>\n"
//    << "WebWorker::SendAllBytesHttp called more than once. "
//    << "Input: \n "
//    << this->ToStringMessageFullUnsafe() << "\nStack trace:\n"
//    << crash.GetStackTraceEtcErrorMessage()
//    << "\n<hr>Sending back:<hr>\n";
//
//    std::string theBytes(this->remainingBytesToSenD.TheObjects, this->remainingBytesToSenD.size);
//    ;
//    firstCallStream << theBytes;
//    logHttpErrors << firstCallStream.str() << logger::endL;
//  }
  this->flagDidSendAll = true;
  //std::string tempS(this->remainingBytesToSenD.TheObjects, this->remainingBytesToSenD.size);
  //logWorker << logger::green << "Sending: " << tempS << logger::endL;
  if (this->connectedSocketID == - 1)
  { logWorker << logger::red << "Socket::SendAllBytes failed: connectedSocketID= - 1." << logger::endL;
    return;
  }
  logWorker << "Sending " << this->remainingBytesToSenD.size << " bytes in chunks of: ";
  logWorker.flush();
  double startTime = theGlobalVariables.GetElapsedSeconds();
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 5; // 5 Secs Timeout
  tv.tv_usec = 0; // Not init'ing this can cause strange errors
  int numTimesRunWithoutSending = 0;
  int timeOutInSeconds = 20;
  setsockopt(this->connectedSocketID, SOL_SOCKET, SO_SNDTIMEO, (void*)(&tv), sizeof(timeval));
  while (this->remainingBytesToSenD.size > 0)
  { if (theGlobalVariables.GetElapsedSeconds() - startTime > timeOutInSeconds)
    { logWorker << "WebWorker::SendAllBytes failed: more than " << timeOutInSeconds << " seconds have elapsed. "
      << logger::endL;
      return;
    }
    int numBytesSent =send
    (this->connectedSocketID, &this->remainingBytesToSenD[0], this->remainingBytesToSenD.size, 0);
    if (numBytesSent < 0)
    { if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR || errno == EIO)
      logIO << "WebWorker::SendAllBytes failed. Error: " << this->parent->ToStringLastErrorDescription() << logger::endL;
      return;
    }
    if (numBytesSent == 0)
      numTimesRunWithoutSending ++;
    else
      numTimesRunWithoutSending = 0;
    logWorker << numBytesSent;
    this->remainingBytesToSenD.Slice(numBytesSent, this->remainingBytesToSenD.size - numBytesSent);
    if (this->remainingBytesToSenD.size > 0)
      logWorker << ", ";
    if (numTimesRunWithoutSending > 3)
    { logWorker << "WebWorker::SendAllBytes failed: send function went through 3 cycles without "
      << " sending any bytes. "
      << logger::endL;
      return;
    }
    logWorker << logger::endL;
  }
  logWorker << "All bytes sent. " << logger::endL;
}

void WebWorker::QueueBytesForSendingNoHeadeR(const List<char>& bytesToSend, bool MustSendAll)
{ MacroRegisterFunctionWithName("WebWorker::QueueBytesForSending");
  (void) MustSendAll;
  this->remainingBytesToSenD.AddListOnTop(bytesToSend);
//  if (this->remainingBytesToSend.size >=1024*512 || MustSendAll)
//    this->SendAllBytes();
}

void WebWorker::QueueStringForSendingWithHeadeR(const std::string& stringToSend, bool MustSendAll)
{ MacroRegisterFunctionWithName("WebWorker::QueueStringForSendingWithHeadeR");
  (void) MustSendAll;
  int oldSize = this->remainingBodyToSend.size;
  this->remainingBodyToSend.SetSize(this->remainingBodyToSend.size + stringToSend.size());
  for (unsigned i = 0; i < stringToSend.size(); i ++)
    this->remainingBodyToSend[i + oldSize] = stringToSend[i];
}

void WebWorker::QueueStringForSendingNoHeadeR(const std::string& stringToSend, bool MustSendAll)
{ MacroRegisterFunctionWithName("WebWorker::QueueStringForSendingNoHeadeR");
  (void) MustSendAll;
  int oldSize = this->remainingBytesToSenD.size;
  this->remainingBytesToSenD.SetSize(this->remainingBytesToSenD.size + stringToSend.size());
  for (unsigned i = 0; i < stringToSend.size(); i ++)
    this->remainingBytesToSenD[i + oldSize] = stringToSend[i];
}

void WebWorker::SendAllBytesNoHeaders()
{ MacroRegisterFunctionWithName("WebWorker::SendAllBytesNoHeaders");
  if (theGlobalVariables.flagUsingSSLinCurrentConnection)
  { this->SendAllBytesHttpSSL();
    return;
  }
  this->SendAllBytesHttp();
}
