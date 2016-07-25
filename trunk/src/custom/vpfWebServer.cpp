//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServer.h"
WebServer theWebServer;

#include "../vpfHeader3Calculator0_Interface.h"
#include "vpfHeader7DatabaseInterface_MySQL.h"

ProjectInformationInstance projectInfoInstanceWebServer(__FILE__, "Web server implementation.");

#include <sys/wait.h>//<-waitpid f-n here
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <arpa/inet.h> // <- inet_ntop declared here (ntop= network to presentation)
#include <unistd.h>
#include <sys/stat.h>//<-for file statistics
#include <fcntl.h>//<-setting flags of file descriptors

#ifdef MACRO_use_open_ssl
//installation of these headers in ubuntu:
//sudo apt-get install libssl-dev
//on opensuse:
//sudo yast -i libopenssl-devel
//Instructions: look at the examples folder in the openssl.
//openssl tutorial (couldn't make it work myself):
//http://www.ibm.com/developerworks/library/l-openssl/
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

struct SSLdata{
public:
  int errorCode;
  SSL* ssl;
  X509* client_cert;
  SSL_CTX* ctx;
  std::string otherCertificateIssuerName, otherCertificateSubjectName;
  SSLdata()
  { this->errorCode=-1;
    this->ssl=0;
    this->client_cert=0;
    this->ctx=0;
  }
};

SSLdata theSSLdata;

void SSL_write_Wrapper(SSL* inputSSL, const std::string& theString)
{ MacroRegisterFunctionWithName("SSL_write_Wrapper");
  int err =  SSL_write (inputSSL, theString.c_str(), theString.size());
  if ((err)==-1)
  { ERR_print_errors_fp(stderr);
    crash << "Errors while calling SSL_write. " << crash;
  }
}

#endif // MACRO_use_open_ssl
//http://stackoverflow.com/questions/10175812/how-to-create-a-self-signed-certificate-with-openssl
//openssl req -x509 -newkey rsa:2048 -nodes -keyout key.pem -out cert.pem -days 1001
//Alternatively:
//certificate with certificate signing request:
//openssl req -out CSR.csr -new -newkey rsa:2048 -nodes -keyout privateKey.key
//then get the CSR.csr file to a signing authority,
//from where you get the signedFileCertificate1 andsignedFileCertificate3
const std::string fileCertificate= "certificates/cert.pem";
const std::string fileKey= "certificates/key.pem";
const std::string signedFileCertificate1= "certificates/2_calculator-algebra.org.crt";
//const std::string signedFileCertificate2= "certificates/1_Intermediate.crt";
const std::string signedFileCertificate3= "certificates/1_root_bundle.crt";
const std::string signedFileKey= "certificates/privateKey.key";

void WebServer::initSSL()
{ MacroRegisterFunctionWithName("WebServer::initSSL");
  if (!theGlobalVariables.flagSSLisAvailable)
  { theLog << logger::red << "SSL is DISABLED." << logger::endL;
    return;
  }
#ifdef MACRO_use_open_ssl
  //////////////////////////////////////////////////////////////////////////
  std::string fileCertificatePhysical, fileKeyPhysical,
  singedFileCertificate1Physical, signedFileCertificate3Physical,
  signedFileKeyPhysical;

  FileOperations::GetPhysicalFileNameFromVirtual
  (signedFileCertificate1, singedFileCertificate1Physical);
  FileOperations::GetPhysicalFileNameFromVirtual
  (signedFileCertificate3, signedFileCertificate3Physical);
  FileOperations::GetPhysicalFileNameFromVirtual
  (fileCertificate, fileCertificatePhysical);
  FileOperations::GetPhysicalFileNameFromVirtual
  (fileKey, fileKeyPhysical);
  FileOperations::GetPhysicalFileNameFromVirtual
  (signedFileKey, signedFileKeyPhysical);
//  std::cout << "\n\nproject base: " << theGlobalVariables.PhysicalPathProjectBase;
//  std::cout << "\n\nfileKey physical: " << fileKeyPhysical;
  //////////////////////////////////////////////////////////
  if (!FileOperations::FileExistsVirtual(fileCertificate) ||
      !FileOperations::FileExistsVirtual(fileKey))
  { theLog << logger::red << "SSL is available but CERTIFICATE files are missing." << logger::endL;
    theGlobalVariables.flagSSLisAvailable=false;
    return;
  }
  theLog << logger::green << "SSL is available." << logger::endL;
  const SSL_METHOD *meth;
  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();
  meth = TLSv1_2_server_method();
  theSSLdata.ctx = SSL_CTX_new (meth);
  if (!theSSLdata.ctx)
  { ERR_print_errors_fp(stderr);
    crash << "openssl error: failed to create CTX object." << crash;
  }
  //////////Safari web browser: no further use of foul language necessary
/*  if (!SSL_CTX_set_cipher_list(theSSLdata.ctx,
  "ALL:!ECDHE-ECDSA-AES256-SHA:!ECDHE-ECDSA-AES128-SHA:!ECDHE-ECDSA-RC4-SHA:!ECDHE-ECDSA-DES-CBC3-SHA"
  ))
  { ERR_print_errors_fp(stderr);
    crash << "openssl error: failed to set cipher list." << crash;
  } else
    theLog << logger::orange << "Restricted ciphers to workaround Safari's bugs. " << logger::endL;
*/


  if (SSL_CTX_use_certificate_chain_file(theSSLdata.ctx, signedFileCertificate3Physical.c_str()) <=0)
  { theLog << logger::purple << "Found no officially signed certificate, trying self-signed certificate. "
    << logger::endL;
    if (SSL_CTX_use_certificate_file(theSSLdata.ctx, fileCertificatePhysical.c_str(), SSL_FILETYPE_PEM) <= 0)
    { ERR_print_errors_fp(stderr);
      exit(3);
    }
    if (SSL_CTX_use_PrivateKey_file(theSSLdata.ctx, fileKeyPhysical.c_str(), SSL_FILETYPE_PEM) <= 0)
    { ERR_print_errors_fp(stderr);
      exit(4);
    }
  } else
  { theLog << logger::green << "Found officially signed certificate... " << logger::endL;
//    if (SSL_CTX_use_certificate_chain_file(theSSLdata.ctx, signedFileCertificate2.c_str()) <=0)
//    { ERR_print_errors_fp(stderr);
//      exit(3);
//    }
    if (SSL_CTX_use_certificate_chain_file(theSSLdata.ctx, singedFileCertificate1Physical.c_str()) <=0)
    { ERR_print_errors_fp(stderr);
      exit(3);
    }
    if (SSL_CTX_use_PrivateKey_file(theSSLdata.ctx, signedFileKeyPhysical.c_str(), SSL_FILETYPE_PEM) <= 0)
    { ERR_print_errors_fp(stderr);
      exit(4);
    }
  }
  if (!SSL_CTX_check_private_key(theSSLdata.ctx))
  { fprintf(stderr,"Private key does not match the certificate public key\n");
    exit(5);
  }
////////Safari web browser: no further use of foul language necessary.
//  SSL_CTX_set_options(theSSLdata.ctx, SSL_OP_SAFARI_ECDHE_ECDSA_BUG);
////////
#endif // MACRO_use_open_ssl
}

void WebServer::SSLfreeResources()
{ if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    return;
#ifdef MACRO_use_open_ssl
  theGlobalVariables.flagUsingSSLinCurrentConnection=false;
  SSL_free (theSSLdata.ssl);
  theSSLdata.ssl=0;
#endif // MACRO_use_open_ssl
}

void WebServer::SSLfreeEverythingShutdownSSL()
{ if (!theGlobalVariables.flagSSLisAvailable)
    return;
#ifdef MACRO_use_open_ssl
  theGlobalVariables.flagSSLisAvailable=false;
  SSL_CTX_free (theSSLdata.ctx);
  theSSLdata.ctx=0;
#endif // MACRO_use_open_ssl
}

void WebServer::SSLServerSideHandShake()
{ if (!theGlobalVariables.flagSSLisAvailable)
    return;
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    return;
//  logOpenSSL << "Initiating SSL handshake. " << logger::endL;
#ifdef MACRO_use_open_ssl
  MacroRegisterFunctionWithName("WebServer::SSLServerSideHandShake");
//  theLog << "Got to here 1" << logger::endL;
  theSSLdata.ssl = SSL_new(theSSLdata.ctx);
//  theLog << "Got to here 1.05" << logger::endL;
  if(theSSLdata.ssl==0)
  { logOpenSSL << logger::red << "Failed to allocate ssl" << logger::endL;
    crash << "Failed to allocate ssl: not supposed to happen" << crash;
  }
//  theLog << "Got to here 1.1" << logger::endL;
  SSL_set_fd (theSSLdata.ssl, this->GetActiveWorker().connectedSocketID);
//  theLog << "Got to here 1.2" << logger::endL;
  int maxNumHandshakeTries=1;
//  int theError=-1;
  for (int i=0; i<maxNumHandshakeTries; i++)
  { theSSLdata.errorCode = SSL_accept (theSSLdata.ssl);
    this->flagSSLHandshakeSuccessful=false;
    if (theSSLdata.errorCode!=1)
    { if (theSSLdata.errorCode==0)
        logOpenSSL << "OpenSSL handshake not successful in a controlled manner. ";
      else
        logOpenSSL << "OpenSSL handshake not successful with a fatal error. ";
      logOpenSSL << "Attempt " << i+1 << " out of " << maxNumHandshakeTries << " failed. ";
      switch(SSL_get_error(theSSLdata.ssl, theSSLdata.errorCode))
      {
      case SSL_ERROR_NONE:
        logOpenSSL << logger::red << "No error reported, this shouldn't happen. " << logger::endL;
        maxNumHandshakeTries=1;
        break;
      case SSL_ERROR_ZERO_RETURN:
        logOpenSSL << logger::red << "The TLS/SSL connection has been closed (possibly cleanly). " << logger::endL;
        maxNumHandshakeTries=1;
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
        maxNumHandshakeTries=1;
        break;
  //    case SSL_ERROR_WANT_ASYNC:
  //      logOpenSSL << logger::red << "Asynchronous engine is still processing data. "
  //      << logger::endL;
  //      break;
      case SSL_ERROR_SYSCALL:
        logOpenSSL << logger::red << "Error: some I/O error occurred. "
        << logger::endL;
        maxNumHandshakeTries=1;
        break;
      case SSL_ERROR_SSL:
        logOpenSSL << logger::red << "A failure in the SSL library occurred. "
        << logger::endL;
//        theError=ERR_get_error(3ssl);
//        if (theError!=SSL_ERROR_WANT_READ && theError!=SSL_ERROR_WANT_WRITE)
//          maxNumHandshakeTries=1;
        break;
      default:
        logOpenSSL << logger::red << "Unknown error. " << logger::endL;
        maxNumHandshakeTries=1;
        break;
      }
      logOpenSSL << "Retrying connection in 0.5 seconds...";
      theGlobalVariables.FallAsleep(500000);
    } else
    { this->flagSSLHandshakeSuccessful=true;
      break;
    }
  }
//  theLog << "Got to here 1.3" << logger::endL;
//    CHK_SSL(err);
//  theLog << "Got to here 2" << logger::endL;
  /* Get the cipher - opt */
  /* Get client's certificate (note: beware of dynamic allocation) - opt */
/*  theSSLdata.client_cert = SSL_get_peer_certificate (theSSLdata.ssl);
  if (theSSLdata.client_cert != NULL)
  { char* tempCharPtr=0;
    logIO << logger::purple << "SSL connection using: " << SSL_get_cipher (theSSLdata.ssl) << logger::endL;
    tempCharPtr = X509_NAME_oneline (X509_get_subject_name (theSSLdata.client_cert), 0, 0);
    if (tempCharPtr==0)
    { logIO << "X509_NAME_oneline return null; this is not supposed to happen. " << logger::endL;
      crash << "X509_NAME_oneline return null; this is not supposed to happen. " << crash;
    }
    theSSLdata.otherCertificateSubjectName=tempCharPtr;
    OPENSSL_free(tempCharPtr);
    logIO << "Client certificate:" << logger::endL
    << "Subject: " << theSSLdata.otherCertificateSubjectName << logger::endL;
    tempCharPtr = X509_NAME_oneline (X509_get_issuer_name  (theSSLdata.client_cert), 0, 0);
    if (tempCharPtr==0)
    { logIO << "X509_NAME_oneline return null; this is not supposed to happen. " << logger::endL;
      crash << "X509_NAME_oneline return null; this is not supposed to happen. " << crash;
    }
    theSSLdata.otherCertificateIssuerName=tempCharPtr;
    OPENSSL_free(tempCharPtr);
    logIO << "Issuer name: " << theSSLdata.otherCertificateIssuerName << logger::endL;
    X509_free (theSSLdata.client_cert);
  } else
    logIO << logger::purple << "SSL connection using: " << SSL_get_cipher (theSSLdata.ssl) << ". "
    << logger::normalColor << "No client certificate." << logger::endL;
    */
#endif // MACRO_use_open_ssl
}

bool WebWorker::ReceiveAllHttpSSL()
{ MacroRegisterFunctionWithName("WebWorker::ReceiveAllHttpSSL");
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    return true;
#ifdef MACRO_use_open_ssl
  unsigned const int bufferSize=60000;
  char buffer[bufferSize];
//  std::cout << "Got thus far 9" << std::endl;
//  std::cout.flush();
  if (this->connectedSocketID==-1)
    crash << "Attempting to receive on a socket with ID equal to -1. " << crash;
//  std::cout << "Got thus far 10" << std::endl;
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 30;  // 30 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
//  logIO << "BEfore setsockopt: " << logger::endL;
  setsockopt(this->connectedSocketID, SOL_SOCKET, SO_RCVTIMEO,(void*)(&tv), sizeof(timeval));
  ProgressReportWebServer theReport;
  theReport.SetStatus("WebWorker: receiving bytes...");
//  logIO << "BEfore ssl_read: " << logger::endL;
  int numBytesInBuffer= SSL_read(theSSLdata.ssl, &buffer, bufferSize-1);
//  logIO << "passed ssl_read: " << logger::endL;
//  std::cout << "Got thus far 11" << std::endl;
  theReport.SetStatus("WebWorker: first bytes received...");
  double numSecondsAtStart=theGlobalVariables.GetElapsedSeconds();
  if (numBytesInBuffer<0 || numBytesInBuffer>(signed)bufferSize)
  { std::stringstream out;
    out << "Socket::ReceiveAllHttpSSL on socket " << this->connectedSocketID << " failed. ";
    ERR_print_errors_fp(stderr);
    this->displayUserInput=out.str();
    theLog << out.str() << logger::endL;
    return false;
  }
  this->theMessage.assign(buffer, numBytesInBuffer);
//  theLog << this->parent->ToStringStatusActive() << ": received " << numBytesInBuffer << " bytes. " << logger::endL;
  this->ParseMessage();
//    std::cout << "Got thus far 12" << std::endl;
//  theLog << "Content length computed to be: " << this->ContentLength << logger::endL;
  if (this->requestType==WebWorker::requestTypes::requestPostCalculator)
    this->displayUserInput="POST " + this->mainArgumentRAW;
  else
    this->displayUserInput="GET " + this->mainAddresSRAW;
  if (this->ContentLength<=0)
  { //logIO << " exiting successfully" << logger::endL;
    return true;
  }
  if (this->mainArgumentRAW.size()==(unsigned) this->ContentLength)
    return true;
//  theLog << "Content-length parsed to be: " << this->ContentLength
//  << "However the size of mainArgumentRAW is: " << this->mainArgumentRAW.size();
  std::stringstream reportStream;
  reportStream << "WebWorker: received first message of "
  << this->ContentLength << " bytes. ";
  theReport.SetStatus(reportStream.str());
  if (this->ContentLength>10000000)
  { this->CheckConsistency();
    error="Content-length parsed to be more than 10 million bytes, aborting.";
    theLog << this->error << logger::endL;
    this->displayUserInput=this->error;
    return false;
  }
//  std::cout << "Debug code here!!!";
/*  if (this->mainArgumentRAW!="")
  { std::stringstream errorstream;
    errorstream << "Content-length equals: " << this->ContentLength
    << " and does not coincide with the size of the message-body (total " << this->mainArgumentRAW.size()
    << "bytes), yet the message-body is non-empty. ";
    this->error=errorstream.str();
    theLog << this->error << logger::endL;
    theLog << logger::red << "The messaged received was: " << logger::endL;
    theLog << this->mainArgumentRAW;
    this->displayUserInput=this->error;
    return false;
  }*/
  reportStream << "Sending continue message ...";
  theReport.SetStatus(reportStream.str());
  this->remainingBytesToSend=(std::string) "HTTP/1.0 100 Continue\r\n";
  this->SendAllBytes();
  this->remainingBytesToSend.SetSize(0);
  reportStream << " done. ";
  theReport.SetStatus(reportStream.str());
  if ((signed) this->mainArgumentRAW.size()<this->ContentLength)
  { reportStream << " Only " << this->mainArgumentRAW.size() << " out of "
    << this->ContentLength << " bytes received, proceeding to receive the rest. ";
    theReport.SetStatus(reportStream.str());
  }
  std::string bufferString;
  while ((signed) this->mainArgumentRAW.size()<this->ContentLength)
  { if (theGlobalVariables.GetElapsedSeconds()-numSecondsAtStart>180)
    { this->error= "Receiving bytes timed out (180 seconds).";
      logIO << this->error << logger::endL;
      this->displayUserInput=this->error;
      return false;
    }
//    logIO << logger::blue << "about to read ..." << logger::endL;
    numBytesInBuffer= SSL_read(theSSLdata.ssl, &buffer, bufferSize-1);
    if (numBytesInBuffer==0)
    { this->error= "While trying to fetch message-body, received 0 bytes. " +
      this->parent->ToStringLastErrorDescription();
      this->displayUserInput=this->error;
      return false;
    }
    if (numBytesInBuffer<0)
    { if (errno==EAGAIN || errno == EWOULDBLOCK || errno == EINTR || errno==EIO || errno==ENOBUFS
          || errno==ENOMEM)
        continue;
      this->error= "Error fetching message body: " + this->parent->ToStringLastErrorDescription();
      ERR_print_errors_fp(stderr);
      logIO << this->error << logger::endL;
      this->displayUserInput=this->error;
      return false;
    }
    bufferString.assign(buffer, numBytesInBuffer);
    this->mainArgumentRAW+=bufferString;
  }
  if ((signed) this->mainArgumentRAW.size()!=this->ContentLength)
  { std::stringstream out;
    out << "The message-body received by me had length " << this->mainArgumentRAW.size()
    << " yet I expected a message of length " << this->ContentLength << ".";
    this->error=out.str();
    this->displayUserInput=this->error;
    theLog << this->error << logger::endL;
    theReport.SetStatus(out.str());
    return false;
  }
  theReport.SetStatus("Webworker: received everything, processing. ");
  return true;
#else
  crash << "This piece of code should never be reached. " << crash;
  return true;
#endif // MACRO_use_open_ssl
}

void WebWorker::SendAllBytesHttpSSL()
{ if (this->remainingBytesToSend.size==0)
    return;
  MacroRegisterFunctionWithName("WebWorker::SendAllBytesHttpSSL");
  this->CheckConsistency();
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    crash << "Error: WebWorker::SendAllBytesHttpSSL called while flagUsingSSLinCurrentConnection is set to false. " << crash;
#ifdef MACRO_use_open_ssl
  ProgressReportWebServer theReport;
  theReport.SetStatus("WebWorker::SendAllBytesHttps: starting ...");
  if (this->connectedSocketID==-1)
  { theLog << logger::red << "Socket::SendAllBytes failed: connectedSocketID=-1." << logger::endL;
    theReport.SetStatus("Socket::SendAllBytes failed: connectedSocketID=-1. WebWorker::SendAllBytes - finished.");
    return;
  }
  theLog << "SSL Worker " << this->indexInParent+1 << " sending " << this->remainingBytesToSend.size << " bytes in chunks of: ";
  std::stringstream reportStream;
  ProgressReportWebServer theReport2;
  reportStream << "Sending " << this->remainingBytesToSend.size << " bytes...";
  theReport.SetStatus(reportStream.str());
  //  theLog << "\r\nIn response to: " << this->theMessage;
  double startTime=theGlobalVariables.GetElapsedSeconds();
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 5;  // 5 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  int numTimesRunWithoutSending=0;
  int timeOutInSeconds =20;

//    theLog << "before setsockopt: " << logger::endL;
  setsockopt(this->connectedSocketID, SOL_SOCKET, SO_SNDTIMEO,(void*)(&tv), sizeof(timeval));
//    theLog << "fter setsockopt: " << logger::endL;
  while (this->remainingBytesToSend.size>0)
  { std::stringstream reportStream2;
    reportStream2 << this->remainingBytesToSend.size << " bytes remaining to send. ";
    if (numTimesRunWithoutSending>0)
      reportStream2 << "Previous attempt to send bytes resulted in 0 bytes sent; this is attempt number "
      << numTimesRunWithoutSending+1 << ". ";
    theReport2.SetStatus(reportStream2.str());
    if (theGlobalVariables.GetElapsedSeconds()-startTime>timeOutInSeconds)
    { theLog << "WebWorker::SendAllBytes failed: more than " << timeOutInSeconds << " seconds have elapsed. "
      << logger::endL;
      std::stringstream reportStream3;
      reportStream3 << "WebWorker::SendAllBytes failed: more than "
      << timeOutInSeconds << " seconds have elapsed. Continuing ...";
      theReport.SetStatus(reportStream3.str());
      return;
    }
//    theLog << "before ssl write: " << logger::endL;
    int numBytesSent =  SSL_write (theSSLdata.ssl, this->remainingBytesToSend.TheObjects, this->remainingBytesToSend.size);
//    theLog << "after ssl write: " << logger::endL;
    if (numBytesSent<0)
    { ERR_print_errors_fp(stderr);
      theLog << "WebWorker::SendAllBytes failed: SSL_write error. " << logger::endL;
      theReport.SetStatus
      ("WebWorker::SendAllBytes failed. Error: " + this->parent->ToStringLastErrorDescription()
       + "WebWorker::SendAllBytes - continue ...");
      return;
    }
    if (numBytesSent==0)
      numTimesRunWithoutSending++;
    else
      numTimesRunWithoutSending=0;
    theLog << numBytesSent;
    this->remainingBytesToSend.Slice(numBytesSent, this->remainingBytesToSend.size-numBytesSent);
    if (this->remainingBytesToSend.size>0)
      theLog << ", ";
    if (numTimesRunWithoutSending>3)
    { theLog << "WebWorker::SendAllBytes failed: send function went through 3 cycles without "
      << " sending any bytes. "
      << logger::endL;
      theReport.SetStatus
      ("WebWorker::SendAllBytes failed: send function went through 3 cycles without sending any bytes. ");
      return;
    }
    theLog.flush();
  }
  theLog << "... done." << logger::endL;
  reportStream << " done. ";
  theReport2.SetStatus(reportStream.str());
  theReport.SetStatus("WebWorker::SendAllBytes - finished.");
#endif // MACRO_use_open_ssl
}

bool ProgressReportWebServer::flagServerExists=true;
const int WebServer::maxNumPendingConnections=200;

ProgressReportWebServer::ProgressReportWebServer(const std::string& inputStatus)
{ this->flagDeallocated=false;
  this->indexProgressReport=theWebServer.theProgressReports.size;
  this->SetStatus(inputStatus);
}

ProgressReportWebServer::ProgressReportWebServer()
{ this->flagDeallocated=false;
  this->indexProgressReport=theWebServer.theProgressReports.size;
}

ProgressReportWebServer::~ProgressReportWebServer()
{ if (!this->flagServerExists)
    return;
  theWebServer.theProgressReports.SetSize(this->indexProgressReport);
  this->flagDeallocated=true;
}

void ProgressReportWebServer::SetStatus(const std::string& inputStatus)
{ MacroRegisterFunctionWithName("ProgressReportWebServer::SetStatus");
  if (theGlobalVariables.flagComputationFinishedAllOutputSentClosing || !this->flagServerExists)
    return;
  if (!theGlobalVariables.flagUsingBuiltInWebServer)
    return;
  theWebServer.CheckConsistency();
//  theLog << logger::endL << logger::red << "SetStatus: outputFunction: "
//  << (int) stOutput.theOutputFunction << logger::endL;
  MutexRecursiveWrapper& safetyFirst=theGlobalVariables.MutexWebWorkerStaticFiasco;
//    std::cout << "Got thus far ProgressReportWebServer::SetStatus 2" << std::endl;
  safetyFirst.LockMe();
//    std::cout << "Got thus far ProgressReportWebServer::SetStatus 3" << std::endl;
  if (this->indexProgressReport>=theWebServer.theProgressReports.size)
    theWebServer.theProgressReports.SetSize(this->indexProgressReport+1);
  theWebServer.theProgressReports[this->indexProgressReport]=inputStatus;
//    std::cout << "SetStatus: passed mutex section" << std::endl;
  std::stringstream toBePiped;
  for (int i=0; i<theWebServer.theProgressReports.size; i++)
    if (theWebServer.theProgressReports[i]!="")
      toBePiped << "<br>" << theWebServer.theProgressReports[i];
  safetyFirst.UnlockMe();
  // theLog << logger::endL << logger::red << "SetStatus before the issue: outputFunction: "
  // << (int) stOutput.theOutputFunction << logger::endL;
  if (!theGlobalVariables.flagUsingBuiltInWebServer)
    return;
  theWebServer.GetActiveWorker().pipeWorkerToServerWorkerStatus.WriteAfterEmptying(toBePiped.str());
}

void WebServer::Signal_SIGINT_handler(int s)
{ MacroRegisterFunctionWithName("WebServer::Signal_SIGINT_handler");
  theLog << "Signal interrupt handler called with input: " << s;
//  << ". Waiting for children to exit... " << logger::endL;
  theWebServer.ReleaseActiveWorker();
  theWebServer.ReleaseNonActiveWorkers();
  while(waitpid(-1, NULL, WNOHANG | WEXITED) > 0)
  { }
  theLog << "All children have exited. " << logger::endL;
  exit(0);
}

void WebServer::Signal_SIGCHLD_handler(int s)
{ (void) s; //avoid unused parameter warning, portable.
  if (theWebServer.flagThisIsWorkerProcess)
    return;
  theLog << "Received SIGCHLD signal." << logger::endL;
  theWebServer.flagReapingChildren=true;
  theWebServer.ReapChildren();
  theWebServer.flagReapingChildren=false;
}

void WebServer::ReapChildren()
{ MacroRegisterFunctionWithName("WebServer::ReapChildren");
  int waitResult=0;
  int exitFlags= WNOHANG| WEXITED;
  do
  { waitResult= waitpid(-1, NULL, exitFlags);
//    theLog << "waitresult is: " << waitResult << logger::endL;
    if (waitResult>0)
      for (int i=0; i<this->theWorkers.size; i++)
        if (this->theWorkers[i].ProcessPID==waitResult)
        { this->theWorkers[i].pipeWorkerToServerControls.WriteNoLocksUNSAFE_FOR_USE_BY_WEBSERVER_ONLY("close");
          this->theWorkers[i].flagInUse=false;
          this->currentlyConnectedAddresses.SubtractMonomial(this->theWorkers[i].userAddress, 1);
          theLog << logger::green << "Child with pid " << waitResult << " successfully reaped. " << logger::endL;
          this->NumProcessesReaped++;
        }
  }while (waitResult>0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{ if (sa->sa_family == AF_INET)
    return &(((struct sockaddr_in*)sa)->sin_addr);
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

std::string WebWorker::ToStringMessageShortUnsafe(FormatExpressions* theFormat)const
{ //if (theGlobalVariables.flagUsingSSLinCurrentConnection)
  //  return "Message cannot be viewed when using SSL";
  std::stringstream out;
  bool useHtml=theFormat==0 ? false: theFormat->flagUseHTML;
  std::string lineBreak= useHtml ? "<br>\n" : "\r\n";
  out << lineBreak;
  if (this->requestType==this->requestGetCalculator)
    out << "GET " << "(from calculator)";
  else if (this->requestType==this->requestPostCalculator)
    out << "POST " << "(from calculator)";
  else if (this->requestType==this->requestGetNotCalculator)
    out << "GET " << "(NOT from calculator)";
  else
    out << "Request type undefined.";
  out << "<hr>Main address raw: " << this->mainAddresSRAW;
  out << lineBreak << "Main address: " << this->mainAddress;
  out << lineBreak << "Main argument: " << this->mainArgumentRAW;
  out << lineBreak << "Relative physical file address referred to by main address: " << this->RelativePhysicalFileName;
  out << lineBreak << "Calculator address: " << theGlobalVariables.DisplayNameExecutableWithPath;
  out << lineBreak << "Physical address project base: " << theGlobalVariables.PhysicalPathProjectBase;
  out << lineBreak << "Physical address server base: " << theGlobalVariables.PhysicalPathServerBasE;
  out << lineBreak << "Physical address output folder: " << theGlobalVariables.PhysicalPathHtmlFolder;
  return out.str();
}

std::string WebWorker::ToStringMessageFullUnsafe()const
{ //if (theGlobalVariables.flagUsingSSLinCurrentConnection)
  //  return "Message cannot be viewed when using SSL";
  std::stringstream out;
  out << this->ToStringMessageUnsafe();
  if (this->theStrings.size>0)
  { out << "<hr>\nStrings extracted from message: ";
    for (int i =0; i<this->theStrings.size; i++)
      out << "<br>" << this->theStrings[i];
  }
  return out.str();
}

std::string WebWorker::ToStringMessageUnsafe()const
{ //if (theGlobalVariables.flagUsingSSLinCurrentConnection)
  //  return "Message cannot be viewed when using SSL";
  std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML=true;
  out << this->ToStringMessageShortUnsafe(&tempFormat);
  out << "<hr>";
  out << "Main address RAW: " << this->mainAddresSRAW << "<br>";
  if (this->requestType==this->requestGetCalculator || this->requestType==this->requestGetNotCalculator)
    out << "GET " << this->mainAddresSRAW;
  if (requestType==this->requestPostCalculator)
    out << "POST " << this->mainAddresSRAW;
  out << "\n<br>\nFull client message:\n" << this->theMessage;
  return out.str();
}

void WebWorker::resetMessageComponentsExceptRawMessage()
{ this->mainArgumentRAW="";
  this->mainAddress="";
  this->mainAddresSRAW="";
  this->RelativePhysicalFileName="";
  this->displayUserInput="";
  this->theStrings.SetSize(0);
  this->requestType=this->requestUnknown;
  this->ContentLength=-1;
}

static std::stringstream standardOutputStreamAfterTimeout;
void WebWorker::StandardOutputAfterTimeOut(const std::string& input)
{ standardOutputStreamAfterTimeout << input;
//  theLog << logger::endL << logger::green << " Standard output: " << standardOutputStreamAfterTimeout.str()
//  << logger::endL;
}

std::string WebWorker::ToStringCalculatorArgumentsHumanReadable()
{ MacroRegisterFunctionWithName("WebWorker::ToStringCalculatorArgumentsHumanReadable");
  if (!theGlobalVariables.UserDebugFlagOn())
    return "";
  std::stringstream out;
  out << "<hr>";
  out << "Default user: " << theGlobalVariables.userDefault;
  if (theGlobalVariables.flagLoggedIn)
    out << "<br>Logged in. ";
  if (theGlobalVariables.UserDefaultHasAdminRights())
    out << "<br><b>User has admin rights</b>";
  out << "<hr>";
  for (int i=0; i<theGlobalVariables.webArguments.size(); i++)
  { out << theGlobalVariables.webArguments.theKeys[i] << ": " << theGlobalVariables.webArguments[i];
    if (i!=theGlobalVariables.webArguments.size()-1)
      out << "<br>";
  }
  out << "<hr>";
  if (theGlobalVariables.flagUsingBuiltInWebServer)
    out << theWebServer.GetActiveWorker().ToStringMessageUnsafe();
  return out.str();
}

bool WebWorker::ProcessRawArguments
(const std::string& urlEncodedInputString, std::stringstream& argumentProcessingFailureComments, int recursionDepth)
{ MacroRegisterFunctionWithName("WebWorker::ProcessRawArguments");
//  stOutput << "DEBUG: here I am.";
  MapList<std::string, std::string, MathRoutines::hashString>& theArgs=theGlobalVariables.webArguments;
  if (recursionDepth>1)
  { argumentProcessingFailureComments << "Error: input string encoded too many times";
    return false;
  }
  if (!CGI::ChopCGIString
      (urlEncodedInputString, theArgs, argumentProcessingFailureComments))
    return false;
  if (theArgs.Contains("doubleURLencodedInput"))
  { std::string newInput=theGlobalVariables.GetWebInput("doubleURLencodedInput");
    theArgs.Clear();
    return this->ProcessRawArguments(newInput, argumentProcessingFailureComments, recursionDepth+1);
  }
  theGlobalVariables.userCalculatorRequestType=theGlobalVariables.GetWebInput("request");
  if (theGlobalVariables.flagUsingSSLinCurrentConnection &&
      theGlobalVariables.userCalculatorRequestType=="exercisesNoLogin")
  { theGlobalVariables.userCalculatorRequestType="exercises";
    theGlobalVariables.SetWebInpuT("request", "exercises");
  }
//  argumentProcessingFailureComments << "DEBUG: Comma delimited inputstringnames: "
//  << inputStrings.ToStringCommaDelimited() << " values: " <<  inputStringNames.ToStringCommaDelimited();
//  stOutput << "userCalculatorRequest type is: " << theGlobalVariables.userCalculatorRequestType;
  std::string password;
  std::string desiredUser;
  if (!theGlobalVariables.UserGuestMode())
  { //argumentProcessingFailureComments << "DEBUG: not guest mode.";
    desiredUser=theGlobalVariables.GetWebInput("username");
    //if (!theGlobalVariables.webFormArgumentNames.Contains("username"))
    //  argumentProcessingFailureComments << "<hr>no username" << "<hr>";
    //argumentProcessingFailureComments << "DEBUG:  desired user1: " << desiredUser
    // << this->ToStringCalculatorArgumentsHumanReadable() << " URL encoded input: "
    // << urlEncodedInputString;
    if (desiredUser=="")
      desiredUser=theGlobalVariables.GetWebInput("usernameHidden");
    if (desiredUser!="")
      CGI::URLStringToNormal(desiredUser, desiredUser);
    //argumentProcessingFailureComments << "DEBUG:  desired user: " << desiredUser
    // << this->ToStringCalculatorArgumentsHumanReadable() << " URL encoded input: "
    // << urlEncodedInputString;
    this->authenticationToken=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("authenticationToken"));
    if (this->authenticationToken!="")
      this->flagAuthenticationTokenWasSubmitted=true;
    this->flagPasswordWasSubmitted=(theGlobalVariables.GetWebInput("password")!="");
    if (this->flagPasswordWasSubmitted)
    { password=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("password"));
      //this may need a security overview: the SetValue function may leave traces in memory
      //of the old password. Not a big deal for the time being.
      theArgs.SetKeyValue
      ("password", "********************************************");
    }
  }
  if (theArgs.Contains("textInput") && !theArgs.Contains("mainInput"))
  { argumentProcessingFailureComments
    << "Received calculator link in an old format, interpreting 'textInput' as 'mainInput'";
    theArgs.theKeys.SetObjectAtIndex(theArgs.theKeys.GetIndex("textInput"), "mainInput");
  }
  if (desiredUser!="" && theGlobalVariables.flagUsingSSLinCurrentConnection)
  { bool changingPass=theGlobalVariables.userCalculatorRequestType=="changePassword" ||
    theGlobalVariables.userCalculatorRequestType=="activateAccount";
    if (changingPass)
      this->authenticationToken="";
    //argumentProcessingFailureComments << "DEBUG: Logging in as: " << desiredUser << " pass: " << password << "<br>";
    theGlobalVariables.flagLoggedIn=false;
    if (this->authenticationToken!="" || password!="")
      theGlobalVariables.flagLoggedIn= DatabaseRoutinesGlobalFunctions::LoginViaDatabase
      (desiredUser, password, this->authenticationToken, theGlobalVariables.userRole, &argumentProcessingFailureComments);
    if (!theGlobalVariables.flagLoggedIn)
    { this->authenticationToken="";
      //argumentProcessingFailureComments << "<b>DEBUG: Auth token set to empty</b>";
    }
    if (theGlobalVariables.flagLoggedIn)
    { theGlobalVariables.userDefault=desiredUser;
      theGlobalVariables.SetWebInpuT("authenticationToken", CGI::StringToURLString(this->authenticationToken));
    } else if (changingPass)
      theGlobalVariables.userDefault=desiredUser;
    else if (this->authenticationToken!="" || password!="")
    { theGlobalVariables.userDefault="";
      theGlobalVariables.SetWebInpuT
      ("error", CGI::StringToURLString("<b>Invalid user or password.</b> Comments follow. "+argumentProcessingFailureComments.str()));
    }
  }
  password="********************************************";
  return true;
}

bool WebWorker::ProcessRawArgumentsNoLoginInterpreterMode
(const std::string& urlEncodedInputString, std::stringstream& argumentProcessingFailureComments, int recursionDepth)
{ MacroRegisterFunctionWithName("WebWorker::ProcessRawArgumentsNoLoginInterpreterMode");
  //stOutput << "DEBUG: here I am.";
  MapList<std::string, std::string, MathRoutines::hashString>& theArgs=theGlobalVariables.webArguments;
  if (recursionDepth>1)
  { argumentProcessingFailureComments << "Error: input string encoded too many times";
    return false;
  }
  if (!CGI::ChopCGIString
      (urlEncodedInputString, theArgs, argumentProcessingFailureComments))
    return false;
  if (theArgs.Contains("doubleURLencodedInput"))
  { std::string newInput=theGlobalVariables.GetWebInput("doubleURLencodedInput");
    theArgs.Clear();
    return this->ProcessRawArguments(newInput, argumentProcessingFailureComments, recursionDepth+1);
  }
  theGlobalVariables.userCalculatorRequestType=theGlobalVariables.GetWebInput("request");
  if (theGlobalVariables.flagUsingSSLinCurrentConnection &&
      theGlobalVariables.userCalculatorRequestType=="exercisesNoLogin")
  { theGlobalVariables.userCalculatorRequestType="exercises";
    theGlobalVariables.SetWebInpuT("request", "exercises");
  }
  std::string desiredUser;
  if (!theGlobalVariables.UserGuestMode())
  { desiredUser=theGlobalVariables.GetWebInput("username");
    if (desiredUser=="")
      desiredUser=theGlobalVariables.GetWebInput("usernameHidden");
    if (desiredUser!="")
      CGI::URLStringToNormal(desiredUser, desiredUser);
  }
  if (theArgs.Contains("textInput") && !theArgs.Contains("mainInput"))
  { argumentProcessingFailureComments
    << "Received calculator link in an old format, interpreting 'textInput' as 'mainInput'";
    theArgs.theKeys.SetObjectAtIndex(theArgs.theKeys.GetIndex("textInput"), "mainInput");
  }
  theGlobalVariables.userDefault=desiredUser;
  theGlobalVariables.userRole=theGlobalVariables.GetWebInput("userRole");
  return true;
}

std::string WebWorker::GetHtmlHiddenInputs()
{ MacroRegisterFunctionWithName("WebWorker::GetHtmlHiddenInputs");
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    return "";
  std::stringstream out;
  //the values of the hidden inputs will be filled in via javascript
  if (this->flagFoundMalformedFormInput)
    out << "<b>Your input formed had malformed entries.</b>";
  out
  << "<input type=\"hidden\" id=\"debugFlag\" name=\"debugFlag\">\n"
  << "<input type=\"hidden\" id=\"studentView\" name=\"studentView\">\n"
  << "<input type=\"hidden\" id=\"studentSection\" name=\"studentSection\">\n"
  << "<input type=\"hidden\" id=\"authenticationToken\" name=\"authenticationToken\">\n"
  << "<input type=\"hidden\" id=\"usernameHidden\" name=\"usernameHidden\">\n"
  << "<input type=\"hidden\" id=\"currentExamHome\" name=\"currentExamHome\">\n"
  << "<input type=\"hidden\" id=\"fileName\" name=\"fileName\">\n"
  << "<input type=\"hidden\" id=\"ignoreSecurity\" name=\"ignoreSecurity\">\n"
  ;
  return out.str();
}

void WebWorker::OutputBeforeComputationUserInputAndAutoComplete()
{ MacroRegisterFunctionWithName("WebWorker::OutputBeforeComputationUserInputAndAutoComplete");
  int startingIndent=this->indentationLevelHTML;
  stOutput << this->openIndentTag("<tr style=\"vertical-align:top\">");
  stOutput << this->openIndentTag("<td><!-- cell with input, autocomplete space and output-->");
  stOutput << this->openIndentTag("<table><!--table with input, autocomplete space and output-->");
  stOutput << this->openIndentTag("<tr>");
  stOutput << this->openIndentTag("<td style=\"vertical-align:top\"><!-- input form here -->");
  //stOutput << this->ToStringCalculatorArgumentsHumanReadable();
  stOutput << "\n<FORM method=\"POST\" id=\"formCalculator\" name=\"formCalculator\" action=\""
  << theGlobalVariables.DisplayNameExecutableWithPath << "\">\n";
  std::string civilizedInputSafish;
  if (CGI::StringToHtmlStringReturnTrueIfModified(theParser.inputString, civilizedInputSafish))
    stOutput << "Your input has been treated normally, however the return string of your input has been modified. More precisely, &lt; and &gt;  are "
    << " modified due to a javascript hijack issue. <br>";
  stOutput << this->GetHtmlHiddenInputComputation();
  stOutput << "<textarea rows=\"3\" cols=\"30\" name=\"mainInput\" id=\"mainInputID\", style=\"white-space:normal\" "
  << "onkeypress=\"if (event.keyCode == 13 && event.shiftKey) {storeSettings(); "
  << " this.form.submit(); return false;}\" "
  << "onkeyup=\"suggestWord();\", onkeydown=\"suggestWord(); arrowAction(event);\", onmouseup=\"suggestWord();\", oninput=\"suggestWord();\""
  << ">";
  stOutput << civilizedInputSafish;
  stOutput << "</textarea>\n<br>\n";
  stOutput << this->GetHtmlHiddenInputs();
  stOutput << "<input type=\"submit\" title=\"Shift+Enter=shortcut from input text box. \" "
  << "name=\"buttonGo\" value=\"Go\" onmousedown=\"storeSettings();\" ";
  stOutput << "> ";
  if (theParser.inputString!="")
    stOutput << "<a href=\"" << theGlobalVariables.DisplayNameExecutableWithPath << "?" << theParser.inputStringRawestOfTheRaw << "\">Link to your input.</a>";
  stOutput << "\n</FORM>\n";
  stOutput << this->closeIndentTag("</td>");
  stOutput << this->openIndentTag("<td style=\"vertical-align:top\"><!--Autocomplete space here -->");
  stOutput << this->openIndentTag("<table><!-- Autocomplete table 2 cells (2 rows 1 column)-->");
  stOutput << this->openIndentTag("<tr>");
  stOutput << this->openIndentTag("<td nowrap>");
  stOutput << "<span \"style:nowrap\" id=\"idAutocompleteHints\">Ctrl+space autocompletes, ctrl+arrow selects word</span>";
  stOutput << this->closeIndentTag("</td>");
  stOutput << this->closeIndentTag("</tr>");
  stOutput << this->openIndentTag("<tr>");
  stOutput << this->openIndentTag("<td id=\"idAutocompleteSpan\">");
  stOutput << this->closeIndentTag("</td>");
  stOutput << this->closeIndentTag("</tr>");
  stOutput << this->openIndentTag("<tr>");
  stOutput << this->openIndentTag("<td id=\"idAutocompleteDebug\">");
  stOutput << this->closeIndentTag("</td>");
  stOutput << this->closeIndentTag("</tr>");
  stOutput << this->closeIndentTag("</table><!--Autocomplete table end-->");
  stOutput << this->closeIndentTag("</td>");
  stOutput << CGI::GetJavascriptAutocompleteWithTags();
  stOutput << this->closeIndentTag("</tr>");
  stOutput << this->closeIndentTag("</table>");
  stOutput << this->closeIndentTag("</td>");
  stOutput << this->closeIndentTag("</tr>");
  if (startingIndent!=this->indentationLevelHTML)
    crash << "Non-balanced html tags. " << crash;
}

void WebWorker::OutputBeforeComputation()
{ MacroRegisterFunctionWithName("WebServer::OutputBeforeComputation");
  stOutput << "<html><meta name=\"keywords\" content= \"Root system, Root system Lie algebra, "
  << "Vector partition function calculator, vector partition functions, Semisimple Lie algebras, "
  << "Root subalgebras, sl(2)-triples\"> <head> <title>calculator version  " << __DATE__ << ", " << __TIME__ << "</title>";
//  if (theGlobalVariables.flagUsingBuiltInWebServer)
  stOutput << CGI::GetJavascriptMathjax();
  stOutput << CGI::GetJavascriptInjectCalculatorResponseInNode();
//  else
//    stOutput << "<script src=\"" << theGlobalVariables.DisplayPathServerBase << "/jsmath/easy/load.js\">";

  stOutput << CGI::GetCalculatorStyleSheetWithTags();
  stOutput << "\n</head>\n<body onload=\"loadSettings();\">\n";
  if (theGlobalVariables.flagLoggedIn)
    stOutput << "<nav>" << theGlobalVariables.ToStringNavigation() << "</nav>" << "<section>";

//  civilizedInput="\\int( 1/x dx)";
//  civilizedInput="\\int (1/(x(1+x^2)^2))dx";
//  civilizedInput="%LogEvaluation \\int (1/(5+2x+7x^2)^2)dx";
  theGlobalVariables.initOutputReportAndCrashFileNames
  (theParser.inputStringRawestOfTheRaw, theParser.inputString);

  stOutput << WebWorker::GetJavascriptHideHtml();
  stOutput << WebWorker::GetJavascriptStandardCookies();
  stOutput << this->openIndentTag("<table><!-- Outermost table, 3 cells (3 columns 1 row)-->");
  stOutput << this->openIndentTag("<tr style=\"vertical-align:top\">");
  stOutput << this->openIndentTag("<td><!-- Cell containing the output table-->");
  stOutput << this->openIndentTag("<table><!-- Output table, 2 cells (1 column 2-3 rows) -->");
  this->OutputBeforeComputationUserInputAndAutoComplete();
  if (theGlobalVariables.UserDebugFlagOn())
    stOutput << "<tr><td>"
    << this->ToStringCalculatorArgumentsHumanReadable()
    << "</td></tr>";
  //  stOutput << "<br>Number of lists created before evaluation: " << NumListsCreated;
}

void WebWorker::OutputResultAfterTimeout()
{ MacroRegisterFunctionWithName("WebWorker::OutputResultAfterTimeout");
  std::stringstream out;
//  out << theParser.ToStringOutputSpecials();
  out << this->openIndentTag("<tr>");
  out << this->openIndentTag("<td>");
  if (standardOutputStreamAfterTimeout.str().size()!=0)
    out << standardOutputStreamAfterTimeout.str() << "<hr>";
  out << theParser.ToStringOutputAndSpecials();
  out << this->closeIndentTag("</td>");
  out << this->closeIndentTag("</tr>");
  out << this->closeIndentTag("</table>");
  out << this->closeIndentTag("</td>");

  out << this->openIndentTag("<td>");
  out << theParser.outputCommentsString;
  out << this->closeIndentTag("</td>");
  out << this->closeIndentTag("</tr>");
  out << this->closeIndentTag("</table>");
  std::fstream outputTimeOutFile;
  FileOperations::OpenFileCreateIfNotPresentVirtual
  (outputTimeOutFile, "output/" + theGlobalVariables.RelativePhysicalNameOutpuT, false, true, false);
  outputTimeOutFile << "<html><body>" << out.str() << "</body></html>";
  outputTimeOutFile.close();
  WebWorker::OutputSendAfterTimeout(out.str());
}

void WebWorker::OutputCrashAfterTimeout()
{ MacroRegisterFunctionWithName("WebWorker::OutputCrashAfterTimeout");
  theLog << logger::red << theWebServer.ToStringActiveWorker() << ": crashing AFTER timeout!" << logger::endL;
  WebWorker::OutputSendAfterTimeout(standardOutputStreamAfterTimeout.str());
  theWebServer.SignalActiveWorkerDoneReleaseEverything();
}

void WebWorker::OutputSendAfterTimeout(const std::string& input)
{ MacroRegisterFunctionWithName("WebWorker::OutputSendAfterTimeout");
  theGlobalVariables.flagTimedOutComputationIsDone=true;
  theLog << "WebWorker::StandardOutputPart2ComputationTimeout called with worker number "
  << theWebServer.GetActiveWorker().indexInParent+1 << "." << logger::endL;
  //requesting pause which will be cleared by the receiver of pipeWorkerToServerIndicatorData
  theWebServer.GetActiveWorker().PauseComputationReportReceived.RequestPausePauseIfLocked();
  theLog << logger::blue << "Sending result through indicator pipe." << logger::endL;
  ProgressReportWebServer theReport("Sending result through indicator pipe.");

  theWebServer.GetActiveWorker().pipeWorkerToServerIndicatorData.WriteAfterEmptying(input);
  logBlock << logger::blue << "Final output written to indicator, blocking until data "
  << "is received on the other end." << logger::endL;
  theReport.SetStatus("Blocking until result data is received.");
  if(!theWebServer.GetActiveWorker().PauseComputationReportReceived.PauseIfRequestedWithTimeOut())
    theReport.SetStatus("Computation report NOT RECEIVED (timed out).");
  else
  { theReport.SetStatus("Computation report received.");
  //requesting pause which will be cleared by the receiver of pipeWorkerToServerIndicatorData
    theWebServer.GetActiveWorker().PauseComputationReportReceived.RequestPausePauseIfLocked();
  }
//  theLog << logger::red << "Result data is received, sending \"finished\"." << logger::endL;
  theWebServer.GetActiveWorker().pipeWorkerToServerIndicatorData.WriteAfterEmptying("finished");
  theReport.SetStatus(" \"finished\" sent through indicator pipe, waiting.");
  theLog << logger::red << "\"finished\" sent through indicator pipe, waiting." << logger::endL;
  theWebServer.GetActiveWorker().PauseComputationReportReceived.PauseIfRequestedWithTimeOut();
}

void WebWorker::OutputStandardResult()
{ MacroRegisterFunctionWithName("WebServer::OutputStandardResult");
  stOutput << this->openIndentTag("<tr>");
  stOutput << this->openIndentTag("<td>");
  stOutput << theParser.ToStringOutputAndSpecials();
  stOutput << this->closeIndentTag("</td>");
  stOutput << this->closeIndentTag("</tr>");
  stOutput << this->closeIndentTag("</table><!--table with input, autocomplete space and output-->");
  stOutput << this->closeIndentTag("</td>");

//  bool displayClientMessage=theWebServer.flagUsingBuiltInServer && theWebServer.activeWorker!=-1;
  //displayClientMessage=false;
  if (theParser.outputCommentsString!="")
  { stOutput << "<td valign=\"top\">";
//    if (displayClientMessage)
//      stOutput << "<b>Message from client: </b>" << theWebServer.GetActiveWorker().ToStringMessageFull() << "<hr>";
    //if (theParser.outputCommentsString.size()<10000)
    stOutput << theParser.outputCommentsString;
    //else
    //stOutput << "The comments generated by your computation are a bit too long. If you want to see them click the show/hide button below.<br>"
    //<< CGI::GetHtmlSpanHidableStartsHiddeN(theParser.outputCommentsString);
    stOutput << "</td>";
  }
  stOutput << "<td valign=\"top\">";
  theGlobalVariables.theSourceCodeFiles().QuickSortAscending();
  stOutput << theGlobalVariables.ToStringSourceCodeInfo();
  stOutput << "<hr><b>Calculator status. </b><br>";
  stOutput << theParser.ToString();

  stOutput << "</td></tr></table>";
  if (theGlobalVariables.flagLoggedIn)
    stOutput << "</section>";
  std::stringstream tempStream3;
  HashedList<std::string, MathRoutines::hashString> autocompleteKeyWords;
  autocompleteKeyWords.SetExpectedSize(theParser.theAtoms.size*2);
  for (int i=0; i<theParser.theAtoms.size; i++)
    autocompleteKeyWords.AddOnTopNoRepetition(theParser.theAtoms[i]);
  for (int i=0; i<theParser.namedRules.size; i++)
    autocompleteKeyWords.AddOnTopNoRepetition(theParser.namedRules[i]);
  autocompleteKeyWords.AddOnTopNoRepetition("NoFrac");
  autocompleteKeyWords.AddOnTopNoRepetition("NoApproximations");
  autocompleteKeyWords.AddOnTopNoRepetition("ShowContext");
  autocompleteKeyWords.AddOnTopNoRepetition("LogParsing");
  autocompleteKeyWords.AddOnTopNoRepetition("LogEvaluation");
  autocompleteKeyWords.AddOnTopNoRepetition("NumberColors");
  autocompleteKeyWords.AddOnTopNoRepetition("LogRules");
  autocompleteKeyWords.AddOnTopNoRepetition("LogCache");
  autocompleteKeyWords.AddOnTopNoRepetition("LogFull");
  autocompleteKeyWords.AddOnTopNoRepetition("LatexLink");
  autocompleteKeyWords.AddOnTopNoRepetition("UseLnInsteadOfLog");
  autocompleteKeyWords.AddOnTopNoRepetition("UseLnAbsInsteadOfLog");
  autocompleteKeyWords.AddOnTopNoRepetition("CalculatorStatus");
  autocompleteKeyWords.AddOnTopNoRepetition("FullTree");
  autocompleteKeyWords.AddOnTopNoRepetition("HideLHS");
  autocompleteKeyWords.AddOnTopNoRepetition("DontUsePredefinedWordSplits");
  autocompleteKeyWords.AddOnTopNoRepetition("PlotShowJavascriptOnly");
  autocompleteKeyWords.AddOnTopNoRepetition("PlotNoCoordinateDetails");
  stOutput << "\n\n<script language=\"javascript\">\n" << "  var theAutocompleteDictionary = [\n  ";
  for (int i=0; i<autocompleteKeyWords.size; i++)
    if (autocompleteKeyWords[i].size()>2)
    { stOutput << "\"" << autocompleteKeyWords[i] << "\"";
      if (i!=autocompleteKeyWords.size-1)
        stOutput << ", ";
    }
  stOutput << "];\n";
  stOutput << "</script>\n";
  stOutput << "</body></html>";
  stOutput << "<!--";
  ProgressReport theReport;
  for(int i=0; i<theParser.SystemCommands.size; i++)
  { std::stringstream out;
    out << "\n\ncommand: " << theParser.SystemCommands[i] << "\n" ;
    theReport.Report(out.str());
    theGlobalVariables.CallSystemNoOutput(theParser.SystemCommands[i]);
  }
  stOutput << "-->";

  //close()
}

void WebWorker::ExtractArgumentFromAddress()
{ MacroRegisterFunctionWithName("WebWorker::ExtractArgumentFromAddress");
//  theLog << "\nmain address:" << this->mainAddress << "=?="
//  << theGlobalVariables.DisplayNameExecutableWithPath << "\nmainaddress.size: "
//  << this->mainAddress.size() << "\nonePredefinedCopyOfGlobalVariables.DisplayNameExecutableWithPath.size(): "
//  << theGlobalVariables.DisplayNameExecutableWithPath.size();
  this->mainArgumentRAW="";
  std::string calculatorArgumentRawWithQuestionMark, tempS;
  if (!MathRoutines::StringBeginsWith
      (this->mainAddresSRAW, theGlobalVariables.DisplayNameExecutableWithPath, &calculatorArgumentRawWithQuestionMark))
  { bool isDeprecatedAddress=//used to capture old links to the calculator
    MathRoutines::StringBeginsWith
    (this->mainAddresSRAW, "/vectorpartition/cgi-bin/calculator", &calculatorArgumentRawWithQuestionMark) ||
    MathRoutines::StringBeginsWith
    (this->mainAddresSRAW, "/vpf/cgi-bin/calculator", &calculatorArgumentRawWithQuestionMark) ||
    MathRoutines::StringBeginsWith
    (this->mainAddresSRAW, "/cgi-bin/calculator", &calculatorArgumentRawWithQuestionMark);
    if (!isDeprecatedAddress)
    { CGI::URLStringToNormal(this->mainAddresSRAW, this->mainAddress);
      return;
    }
  }
  CGI::URLStringToNormal(this->mainAddresSRAW, this->mainAddress);
  this->requestType=this->requestGetCalculator;
  MathRoutines::SplitStringInTwo(calculatorArgumentRawWithQuestionMark, 1, tempS, this->mainArgumentRAW);
//  theLog << logger::yellow << "this->mainArgumentRAW=" << this->mainArgumentRAW << logger::endL;
}

void WebWorker::ParseMessage()
{ MacroRegisterFunctionWithName("WebWorker::ParseMessage");
  //std::cout << "Got thus far 13" << std::endl;
  this->resetMessageComponentsExceptRawMessage();
  std::string buffer;
  buffer.reserve(this->theMessage.size());
  this->theStrings.SetExpectedSize(20);
  for (unsigned i =0; i<this->theMessage.size(); i++)
    if (theMessage[i]!=' ' && theMessage[i]!='\n' && theMessage[i]!='\r')
    { buffer.push_back(this->theMessage[i]);
      if (i==this->theMessage.size()-1)
        this->theStrings.AddOnTop(buffer);
    } else
    { if (buffer!="")
      { this->theStrings.AddOnTop(buffer);
        buffer="";
      }
      if (i>0)
        if (theMessage[i]=='\n' && theMessage[i-1]=='\r')
          this->theStrings.AddOnTop("!CRLF!");
    }
  for (int i=0; i<this->theStrings.size; i++)
    if (this->theStrings[i]=="GET")
    { this->requestType=this->requestGetNotCalculator;
      i++;
      if (i<this->theStrings.size)
      { this->mainAddresSRAW=this->theStrings[i];
        this->ExtractArgumentFromAddress();
      }
    } else if (this->theStrings[i]=="POST")
    { this->requestType=this->requestPostCalculator;
      i++;
      if (i<this->theStrings.size)
      { this->mainAddresSRAW=this->theStrings[i];
        if (*this->theStrings.LastObject()!="!CRLF!")
          this->mainArgumentRAW=*this->theStrings.LastObject();
        else
          this->mainArgumentRAW="";
      }
    } else if ((this->theStrings[i]=="Content-Length:" || this->theStrings[i]=="Content-length:" ||
                this->theStrings[i]=="content-length:")
               && i+1<this->theStrings.size)
    { if (this->theStrings[i+1].size()<10000)
      { LargeIntUnsigned theLI;
        if (theLI.AssignStringFailureAllowed(this->theStrings[i+1], true))
          if (!theLI.IsIntegerFittingInInt(&this->ContentLength))
            this->ContentLength=-1;
      }
    }
//  std::cout << "Got thus far 14" << std::endl;

}

void WebWorker::QueueBytesForSending(const List<char>& bytesToSend, bool MustSendAll)
{ MacroRegisterFunctionWithName("WebWorker::QueueBytesForSending");
  this->remainingBytesToSend.AddListOnTop(bytesToSend);
  if (this->remainingBytesToSend.size>=1024*512 || MustSendAll)
    this->SendAllBytes();
}

void WebWorker::QueueStringForSending(const std::string& stringToSend, bool MustSendAll)
{ MacroRegisterFunctionWithName("WebWorker::SendStringToSocket");
  int oldSize=this->remainingBytesToSend.size;
  this->remainingBytesToSend.SetSize(this->remainingBytesToSend.size+stringToSend.size());
  for (unsigned i=0; i<stringToSend.size(); i++)
    this->remainingBytesToSend[i+oldSize]=stringToSend[i];
//  if (stringToSend.size()>0)
//    if (stringToSend[stringToSend.size()-1]=='\0')
//      theLog << "WARNING: string is null terminated!\r\n";
//    theLog << "Last character string: " << *this->remainingBytesToSend.LastObject() << "\r\n";
  if (this->remainingBytesToSend.size>=1024*512 || MustSendAll)
    this->SendAllBytes();
}

void WebWorker::SendAllBytes()
{ MacroRegisterFunctionWithName("WebWorker::SendAllBytes");
  if (theGlobalVariables.flagUsingSSLinCurrentConnection)
  { this->SendAllBytesHttpSSL();
    return;
  }
  this->SendAllBytesHttp();
}

void WebWorker::SendAllBytesHttp()
{ if (this->remainingBytesToSend.size==0)
    return;
  MacroRegisterFunctionWithName("WebWorker::SendAllBytesHttp");
  this->CheckConsistency();
  ProgressReportWebServer theReport;
  theReport.SetStatus("WebWorker::SendAllBytes: starting ...");
  if (this->connectedSocketID==-1)
  { theLog << logger::red << "Socket::SendAllBytes failed: connectedSocketID=-1." << logger::endL;
    theReport.SetStatus("Socket::SendAllBytes failed: connectedSocketID=-1. WebWorker::SendAllBytes - finished.");
    return;
  }
  theLog << "Sending " << this->remainingBytesToSend.size << " bytes in chunks of: ";
  theLog.flush();
  std::stringstream reportStream;
  ProgressReportWebServer theReport2;
  reportStream << "Sending " << this->remainingBytesToSend.size << " bytes...";
  theReport.SetStatus(reportStream.str());
  //  theLog << "\r\nIn response to: " << this->theMessage;
  double startTime=theGlobalVariables.GetElapsedSeconds();
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 5;  // 5 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  int numTimesRunWithoutSending=0;
  int timeOutInSeconds =20;
  setsockopt(this->connectedSocketID, SOL_SOCKET, SO_SNDTIMEO,(void*)(&tv), sizeof(timeval));
  while (this->remainingBytesToSend.size>0)
  { std::stringstream reportStream2;
    reportStream2 << this->remainingBytesToSend.size << " bytes remaining to send. ";
    if (numTimesRunWithoutSending>0)
      reportStream2 << "Previous attempt to send bytes resulted in 0 bytes sent; this is attempt number "
      << numTimesRunWithoutSending+1 << ". ";
    theReport2.SetStatus(reportStream2.str());
    if (theGlobalVariables.GetElapsedSeconds()-startTime>timeOutInSeconds)
    { theLog << "WebWorker::SendAllBytes failed: more than " << timeOutInSeconds << " seconds have elapsed. "
      << logger::endL;
      std::stringstream reportStream3;
      reportStream3 << "WebWorker::SendAllBytes failed: more than "
      << timeOutInSeconds << " seconds have elapsed. Continuing ...";
      theReport.SetStatus(reportStream3.str());
      return;
    }
    int numBytesSent=send
    (this->connectedSocketID, &this->remainingBytesToSend[0], this->remainingBytesToSend.size,0);
    if (numBytesSent<0)
    { if (errno==EAGAIN || errno ==EWOULDBLOCK || errno== EINTR || errno==EIO )
      logIO << "WebWorker::SendAllBytes failed. Error: "
      << this->parent->ToStringLastErrorDescription() << logger::endL;
      theReport.SetStatus
      ("WebWorker::SendAllBytes failed. Error: " + this->parent->ToStringLastErrorDescription()
       + "WebWorker::SendAllBytes - continue ...");
      return;
    }
    if (numBytesSent==0)
      numTimesRunWithoutSending++;
    else
      numTimesRunWithoutSending=0;
    theLog << numBytesSent;
    this->remainingBytesToSend.Slice(numBytesSent, this->remainingBytesToSend.size-numBytesSent);
    if (this->remainingBytesToSend.size>0)
      theLog << ", ";
    if (numTimesRunWithoutSending>3)
    { theLog << "WebWorker::SendAllBytes failed: send function went through 3 cycles without "
      << " sending any bytes. "
      << logger::endL;
      theReport.SetStatus
      ("WebWorker::SendAllBytes failed: send function went through 3 cycles without sending any bytes. ");
      return;
    }
    theLog << logger::endL;
  }
  reportStream << " done. ";
  theReport2.SetStatus(reportStream.str());
  theReport.SetStatus("WebWorker::SendAllBytes - finished.");
}

bool WebWorker::CheckConsistency()
{ stOutput.theOutputFunction=0;
  if (this->flagDeallocated)
    crash << "Use after free of webworker." << crash;
  if (this->parent==0)
    crash << "Parent of web worker is not initialized." << crash;
  if (this->indexInParent==-1)
    crash << "Index in parent is bad. " << crash;
  if (this->connectionID==-1)
    crash << "Connection id is bad. " << crash;
  stOutput.theOutputFunction=WebServer::SendStringThroughActiveWorker;
  return true;
}

bool WebWorker::ReceiveAll()
{ MacroRegisterFunctionWithName("WebWorker::ReceiveAll");
  if (theGlobalVariables.flagUsingSSLinCurrentConnection)
    return this->ReceiveAllHttpSSL();
  return this->ReceiveAllHttp();
}

bool WebWorker::ReceiveAllHttp()
{ MacroRegisterFunctionWithName("WebWorker::ReceiveAllHttp");
  unsigned const int bufferSize=60000;
  char buffer[bufferSize];
//  std::cout << "Got thus far 9" << std::endl;

  if (this->connectedSocketID==-1)
    crash << "Attempting to receive on a socket with ID equal to -1. " << crash;
//  std::cout << "Got thus far 10" << std::endl;
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 30;  // 30 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  setsockopt(this->connectedSocketID, SOL_SOCKET, SO_RCVTIMEO,(void*)(&tv), sizeof(timeval));
  ProgressReportWebServer theReport;
  theReport.SetStatus("WebWorker: receiving bytes...");
  int numBytesInBuffer= recv(this->connectedSocketID, &buffer, bufferSize-1, 0);
//  std::cout << "Got thus far 11" << std::endl;
  theReport.SetStatus("WebWorker: first bytes received...");
  double numSecondsAtStart=theGlobalVariables.GetElapsedSeconds();
  if (numBytesInBuffer<0 || numBytesInBuffer>(signed)bufferSize)
  { std::stringstream out;
    out << "Socket::ReceiveAll on socket " << this->connectedSocketID << " failed. Error: "
    << this->parent->ToStringLastErrorDescription();
    this->displayUserInput=out.str();
    logIO << out.str() << logger::endL;
    return false;
  }
  this->theMessage.assign(buffer, numBytesInBuffer);
//  theLog << this->parent->ToStringStatusActive() << ": received " << numBytesInBuffer << " bytes. " << logger::endL;
  this->ParseMessage();
//    std::cout << "Got thus far 12" << std::endl;

//  theLog << "Content length computed to be: " << this->ContentLength;
  if (this->requestType==WebWorker::requestTypes::requestPostCalculator)
    this->displayUserInput="POST " + this->mainArgumentRAW;
  else
    this->displayUserInput="GET " + this->mainAddresSRAW;
  if (this->ContentLength<=0)
    return true;
  if (this->mainArgumentRAW.size()==(unsigned) this->ContentLength)
    return true;
//  theLog << "Content-length parsed to be: " << this->ContentLength
//  << "However the size of mainArgumentRAW is: " << this->mainArgumentRAW.size();
  std::stringstream reportStream;
  reportStream << "WebWorker: received first message of "
  << this->ContentLength << " bytes. ";
  theReport.SetStatus(reportStream.str());
  if (this->ContentLength>10000000)
  { this->CheckConsistency();
    error="Content-length parsed to be more than 10 million bytes, aborting.";
    theLog << this->error << logger::endL;
    this->displayUserInput=this->error;
    return false;
  }
//  std::cout << "Debug code here!!!";
/*  if (this->mainArgumentRAW!="")
  { std::stringstream errorstream;
    errorstream << "Content-length equals: " << this->ContentLength
    << " and does not coincide with the size of the message-body (total " << this->mainArgumentRAW.size()
    << "bytes), yet the message-body is non-empty. ";
    this->error=errorstream.str();
    theLog << this->error << logger::endL;
    theLog << logger::red << "The messaged received was: " << logger::endL;
    theLog << this->mainArgumentRAW;
    this->displayUserInput=this->error;
    return false;
  }*/
  reportStream << "Sending continue message ...";
  theReport.SetStatus(reportStream.str());
  this->remainingBytesToSend=(std::string) "HTTP/1.0 100 Continue\r\n";
  this->SendAllBytes();
  this->remainingBytesToSend.SetSize(0);
  reportStream << " done. ";
  theReport.SetStatus(reportStream.str());
  if ((signed) this->mainArgumentRAW.size()<this->ContentLength)
  { reportStream << " Only " << this->mainArgumentRAW.size() << " out of "
    << this->ContentLength << " bytes received, proceeding to receive the rest. ";
    theReport.SetStatus(reportStream.str());
  }
  std::string bufferString;
  while ((signed) this->mainArgumentRAW.size()<this->ContentLength)
  { if (theGlobalVariables.GetElapsedSeconds()-numSecondsAtStart>180)
    { this->error= "Receiving bytes timed out (180 seconds).";
      logIO << this->error << logger::endL;
      this->displayUserInput=this->error;
      return false;
    }
    numBytesInBuffer= recv(this->connectedSocketID, &buffer, bufferSize-1, 0);
    if (numBytesInBuffer==0)
    { this->error= "While trying to fetch message-body, received 0 bytes. " +
      this->parent->ToStringLastErrorDescription();
      this->displayUserInput=this->error;
      return false;
    }
    if (numBytesInBuffer<0)
    { if (errno==EAGAIN || errno == EWOULDBLOCK || errno == EINTR || errno==EIO //|| errno==ENOBUFS
          || errno==ENOMEM)
        continue;
      this->error= "Error fetching message body: " + this->parent->ToStringLastErrorDescription();
      logIO << logger::red << this->error << logger::endL;
      this->displayUserInput=this->error;
      return false;
    }
    bufferString.assign(buffer, numBytesInBuffer);
    this->mainArgumentRAW+=bufferString;
  }
  if ((signed) this->mainArgumentRAW.size()!=this->ContentLength)
  { std::stringstream out;
    out << "The message-body received by me had length " << this->mainArgumentRAW.size()
    << " yet I expected a message of length " << this->ContentLength << ".";
    this->error=out.str();
    this->displayUserInput=this->error;
    theLog << this->error << logger::endL;
    theReport.SetStatus(out.str());
    return false;
  }
  theReport.SetStatus("Webworker: received everything, processing. ");
  return true;
}

void WebWorker::SendDisplayUserInputToServer()
{ MacroRegisterFunctionWithName("WebWorker::SendDisplayUserInputToServer");
  if (this->displayUserInput.size()>3000)
    this->displayUserInput.resize(3000);
  this->pipeWorkerToServerUserInput.WriteAfterEmptying(this->displayUserInput);
//  theLog << logger::blue << "Piping " << this->displayUserInput << " to the server. " << logger::endL;
}

void WebWorker::SanitizeMainAddress()
{ MacroRegisterFunctionWithName("WebWorker::SanitizeMainAddress");
  std::string resultAddress;
  this->mainAddressNonSanitized=this->mainAddress;
  resultAddress.reserve(this->mainAddress.size());
  bool foundslash=false;
  for (signed i=(signed) this->mainAddress.size()-1; i>=0; i--)
    if (this->mainAddress[i]=='?')
    { this->mainAddress=this->mainAddress.substr(0, i);
      this->flagMainAddressSanitized=true;
      break;
    }
  for (signed i=(signed) this->mainAddress.size()-1; i>=0; i--)
  { bool isOK=true;
    if (foundslash && this->mainAddress[i]=='.')
      if (i>0 && this->mainAddress[i-1]=='.')
      { this->flagMainAddressSanitized=true;
        isOK=false;
      }
    if (isOK)
      resultAddress.push_back(this->mainAddress[i]);
    if (this->mainAddress[i]=='/')
      foundslash=true;
  }
  this->mainAddress="";
  for (int i=resultAddress.size()-1; i>=0;i--)
    this->mainAddress.push_back(resultAddress[i]);
}

std::string WebWorker::openIndentTag(const std::string& theTag)
{ std::stringstream out;
  for (int i=0; i<this->indentationLevelHTML; i++)
    out << "  ";
  out << theTag << "\n";
  this->indentationLevelHTML++;
  return out.str();
}

std::string WebWorker::closeIndentTag(const std::string& theTag)
{ std::stringstream out;
  this->indentationLevelHTML--;
  for (int i=0; i<this->indentationLevelHTML; i++)
    out << "  ";
  out << theTag << "\n";
  return out.str();
}

void WebWorker::ExtractPhysicalAddressFromMainAddress()
{ MacroRegisterFunctionWithName("WebWorker::ExtractPhysicalAddressFromMainAddress");
//  int numBytesToChop=0;//theGlobalVariables.DisplayPathServerBase.size();
//  std::string displayAddressStart= this->mainAddress.substr(0, numBytesToChop);
//  if (displayAddressStart!=theGlobalVariables.DisplayPathServerBase)
//    numBytesToChop=0;
  this->SanitizeMainAddress();
  this->RelativePhysicalFileName= this->mainAddress;//.substr(numBytesToChop, std::string::npos);
  if (this->RelativePhysicalFileName.size()>0)
    if (this->RelativePhysicalFileName[0]=='/')
      this->RelativePhysicalFileName=this->RelativePhysicalFileName.substr(1,std::string::npos);
//  this->RelativePhysicalFileName= this->mainAddress.substr(numBytesToChop, std::string::npos);
}

int WebWorker::ProcessCalculatorExamples()
{ MacroRegisterFunctionWithName("WebWorker::ProcessCalculatorExamples");
  stOutput << theParser.ToStringFunctionHandlers();
  return 0;
}

int WebWorker::ProcessServerStatusPublic()
{ MacroRegisterFunctionWithName("WebWorker::ProcessServerStatusPublic");
  stOutput << theWebServer.ToStringStatusPublic();
  stOutput.Flush();
  return 0;
}

int WebWorker::ProcessServerStatus()
{ MacroRegisterFunctionWithName("WebWorker::ProcessGetRequestServerStatus");
  stOutput << "<html>"
  << "<head>"
  << CGI::GetCalculatorStyleSheetWithTags()
  << "</head>"
  << "<body>";
  if (theGlobalVariables.flagLoggedIn)
    stOutput << "<nav>" << theGlobalVariables.ToStringNavigation() << "</nav>";
  stOutput << "<section>";
  if (theGlobalVariables.UserDefaultHasAdminRights())
    stOutput << " <table><tr><td style=\"vertical-align:top\">"
    << this->parent->ToStringStatusAll() << "</td><td>"
    << theGlobalVariables.ToStringHTMLTopCommandLinuxSystem()
    << "</td></tr></table>";
  else
    stOutput << "<b>Viewing server status available only to logged-in admins.</b>";
  stOutput << "</section>";
  stOutput << "</body></html>";
  return 0;
}

int WebWorker::ProcessPauseWorker()
{ MacroRegisterFunctionWithName("WebWorker::ProcessPauseWorker");
  theLog << "Proceeding to toggle worker pause." << logger::endL;
  std::string theMainInput=theGlobalVariables.GetWebInput("mainInput");
  if (theMainInput=="")
  { stOutput << "<b>To pause a worker please provide the worker number in the mainInput field.</b>";
    return 0;
  }
  int inputWebWorkerNumber= atoi(theMainInput.c_str());
  int inputWebWorkerIndex= inputWebWorkerNumber-1;
  if (inputWebWorkerIndex<0 || inputWebWorkerIndex>=this->parent->theWorkers.size)
  { stOutput << "<b>User requested worker number " << inputWebWorkerNumber
    << " out of " << this->parent->theWorkers.size << " which is out of range. </b>";
    return 0;
  }
  if (!this->parent->theWorkers[inputWebWorkerIndex].flagInUse)
  { stOutput << "<b>Requested worker number " << inputWebWorkerNumber << " is not in use. Total number of workers: "
    << this->parent->theWorkers.size << ". </b>";
    return 0;
  }
  WebWorker& otherWorker=this->parent->theWorkers[inputWebWorkerIndex];
//  if (theGlobalVariables.flagLogInterProcessCommunication)
//  theLog << "About to read pipeServerToWorker..." << logger::endL;
  if (!otherWorker.PauseWorker.CheckPauseIsRequested())
  { otherWorker.PauseWorker.RequestPausePauseIfLocked();
    stOutput << "paused";
    return 0;
  }
  stOutput << "unpaused";
  otherWorker.PauseWorker.ResumePausedProcessesIfAny();
  return 0;
}

int WebWorker::ProcessMonitor()
{ MacroRegisterFunctionWithName("WebWorker::ProcessMonitor");
  theLog << "Processing get monitor." << logger::endL;
  std::string theMainInput=theGlobalVariables.GetWebInput("mainInput");
  if (theMainInput=="")
  { stOutput << "<b>Monitor takes as argument the number of the child process that is running the computation.</b>";
    return 0;
  }
  int inputWebWorkerNumber= atoi(theMainInput.c_str());
  stOutput << "<html><body>" << this->GetJavaScriptIndicatorBuiltInServer(inputWebWorkerNumber-1, true)
  << "</body></html>";
  return 0;
}

int WebWorker::ProcessComputationIndicator()
{ MacroRegisterFunctionWithName("WebWorker::ProcessComputationIndicator");
  theLog << "Processing get request indicator." << logger::endL;
  ProgressReportWebServer theReport("Preparing indicator report");
  std::string theMainInput=theGlobalVariables.GetWebInput("mainInput");
  if (theMainInput=="")
  { stOutput << "<b>To get a computation indicator you need to supply the number "
    << "of the child process in the mainInput field.</b>";
    return 0;
  }
  int inputWebWorkerNumber= atoi(theMainInput.c_str());
  int inputWebWorkerIndex= inputWebWorkerNumber-1;
  if (inputWebWorkerIndex<0 || inputWebWorkerIndex>=this->parent->theWorkers.size)
  { stOutput << "<b>Indicator error. Worker number "
    << inputWebWorkerNumber << " is out of range: there are " << this->parent->theWorkers.size
    << " workers. </b>";
    return 0;
  }
  std::stringstream theErrorMessageStream;
  theErrorMessageStream
  << "<span style=\"color:red\">Your computation may have terminated unexpectedly. The maximum number of "
  << " connections/computations you can run is: " << theWebServer.MaxNumWorkersPerIPAdress << ". </span>"
  << "At the time of writing, you use up 1 connection per computation, "
  << "1 - 4 connections for each web browser request (depending on the browser) "
  << "and 1 per computation window request. The "
  << "most recent error message reported by the worker you want to monitor is: "
  << this->parent->theWorkers[inputWebWorkerIndex].pingMessage;
  if (!this->parent->theWorkers[inputWebWorkerIndex].flagInUse)
  { stOutput << "<b>Indicator error. Worker number " << inputWebWorkerNumber << " is not in use. "
    << theErrorMessageStream.str()
    << " Total number of workers: " << this->parent->theWorkers.size << ". </b>";
    return 0;
  }
  if (inputWebWorkerIndex==this->indexInParent)
  { stOutput << "<b>Indicator error. Worker number " << inputWebWorkerNumber << " requested to monitor itself. "
    << " This is not allowed. " << theErrorMessageStream.str() << "</b>";
    return 0;
  }
  WebWorker& otherWorker=this->parent->theWorkers[inputWebWorkerIndex];
//  theLog << "Worker " << this->parent->activeWorker
//  << consoleYellow(" piping 'indicator'" ) << logger::endL;
  otherWorker.pipeServerToWorkerRequestIndicator.WriteAfterEmptying("!");
//  theLog << "'indicator' piped, waiting for return." << logger::endL;
  otherWorker.pipeWorkerToServerIndicatorData.Read();
//  theLog << "indicator returned." << logger::endL;
  if (otherWorker.pipeWorkerToServerIndicatorData.lastRead.size>0)
  { std::string outputString;
    outputString.assign(otherWorker.pipeWorkerToServerIndicatorData.lastRead.TheObjects, otherWorker.pipeWorkerToServerIndicatorData.lastRead.size);
    //theLog << logger::yellow << "Indicator string read: " << logger::blue << outputString << logger::endL;
    stOutput << outputString;
    otherWorker.PauseComputationReportReceived.ResumePausedProcessesIfAny();
  }
//  stOutput << "<b>Not implemented: request for indicator for worker " << inputWebWorkerNumber
//  << " out of " << this->parent->theWorkers.size << ".</b>";
  return 0;
}

void WebWorker::WriteProgressReportToFile(const std::string& input)
{ MacroRegisterFunctionWithName("WebWorker::WriteProgressReportToFile");
  theLog << logger::green << "Progress report written to file: " << theGlobalVariables.RelativePhysicalNameProgressReport << logger::endL;
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
  static int counter=0;
  counter++;
  std::stringstream debugStream1, debugStream2;
  debugStream1 << "PipeProgressReportToParentProcess called " << counter << " times. Calling pause...";
  ProgressReportWebServer theReport(debugStream1.str());
  this->PauseIndicatorPipeInUse.RequestPausePauseIfLocked();
//    theLog << "about to potentially block " << logger::endL;
  debugStream2 << "PipeProgressReportToParentProcess called " << counter << " times. Pause passed...";
  theReport.SetStatus(debugStream2.str());
  if (this->PauseWorker.CheckPauseIsRequested())
  { theReport.SetStatus("PipeProgressReportToParentProcess: pausing as requested...");
    this->WriteProgressReportToFile(input);
  }
  this->PauseWorker.PauseIfRequested();     //if pause was requested, here we block
//    theLog << "(possible) block passed" << logger::endL;
  theReport.SetStatus("PipeProgressReportToParentProcess: computing...");
  this->pipeServerToWorkerRequestIndicator.Read();
  if (this->pipeServerToWorkerRequestIndicator.lastRead.size==0)
  { this->PauseIndicatorPipeInUse.ResumePausedProcessesIfAny();
    return;
  }
  if (theGlobalVariables.flagTimedOutComputationIsDone)
  { this->PauseIndicatorPipeInUse.ResumePausedProcessesIfAny();
    return;
  }
//  if (theGlobalVariables.flagLogInterProcessCommunication)
//  theLog << " data written!";
  theReport.SetStatus("PipeProgressReportToParentProcess: piping computation process...");
  this->pipeWorkerToServerIndicatorData.WriteAfterEmptying(input);
  theReport.SetStatus("PipeProgressReportToParentProcess: exiting 1...");
  this->PauseIndicatorPipeInUse.ResumePausedProcessesIfAny();
  theReport.SetStatus("PipeProgressReportToParentProcess: exiting 2...");
}

int WebWorker::ProcessFolder()
{ MacroRegisterFunctionWithName("WebWorker::ProcessFolder");
  std::stringstream out;
  out << "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n" << "<html><body>";
//  out << this->ToString();
  if (this->PhysicalFileName_COMPUTED_DO_NOT_CHANGE.size()>0)
    if (this->PhysicalFileName_COMPUTED_DO_NOT_CHANGE[this->PhysicalFileName_COMPUTED_DO_NOT_CHANGE.size()-1]!='/')
      this->PhysicalFileName_COMPUTED_DO_NOT_CHANGE.push_back('/');
  if (this->mainAddress.size()>0)
    if (this->mainAddress[this->mainAddress.size()-1]!='/')
      this->mainAddress.push_back('/');
  if (this->flagMainAddressSanitized)
  { out << "<hr>The original main address I extracted was: " << this->mainAddressNonSanitized
    << "<br>However, I do not allow addresses that contain dots (to avoid access to folders below the server). "
    << " Therefore I have sanitized the main address to: " << this->mainAddress;
  }
  List<std::string> theFileNames, theFileTypes;
  if (!FileOperations::GetFolderFileNamesUnsecure(this->PhysicalFileName_COMPUTED_DO_NOT_CHANGE, theFileNames, &theFileTypes))
  { out << "<b>Failed to open directory with physical address "
    << this->PhysicalFileName_COMPUTED_DO_NOT_CHANGE << " </b></body></html>";
    stOutput << out.str();
    return 0;
  }
  out << "Browsing folder: " << this->mainAddress << "<br>Relative physical address: "
  << this->RelativePhysicalFileName << "<hr>";
  List<std::string> theFolderNamesHtml, theFileNamesHtml;
  for (int i=0; i<theFileNames.size; i++)
  { std::stringstream currentStream;
    bool isDir= (theFileTypes[i]==".d");
//    theLog << logger::red << "Current file name: " << CGI::StringToURLString(theFileNames[i]) << logger::endL;
    currentStream << "<a href=\"" << this->mainAddress << CGI::StringToURLString(theFileNames[i]);
    if (isDir)
      currentStream << "/";
//    else
//      theLog <<
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
  for (int i =0; i< theFolderNamesHtml.size; i++)
    out << theFolderNamesHtml[i];
  for (int i =0; i< theFileNamesHtml.size; i++)
    out << theFileNamesHtml[i];
  out << "</body></html>";
  stOutput << out.str();
  return 0;
}

void WebWorker::reset()
{ this->connectedSocketID=-1;
  this->ProcessPID=-1;
  this->connectedSocketIDLastValueBeforeRelease=-1;
  this->connectionID=-1;
  this->indexInParent=-1;
  this->parent=0;
  this->indentationLevelHTML=0;
  this->displayUserInput="";
  this->requestType=this->requestUnknown;
  this->flagMainAddressSanitized=false;
  this->timeOfLastPingServerSideOnly=-1;
  this->flagAuthenticationTokenWasSubmitted=false;
  this->flagFoundMalformedFormInput=false;
  this->flagPasswordWasSubmitted=false;
  this->flagProgressReportAllowed=false;
  theGlobalVariables.flagUsingSSLinCurrentConnection=false;
  theGlobalVariables.flagLoggedIn=false;
  for (unsigned i=0; i<theGlobalVariables.userDefault.size(); i++)
    theGlobalVariables.userDefault[i]=' ';
  theGlobalVariables.userDefault="";
  for (unsigned i=0; i<this->authenticationToken.size(); i++)
    this->authenticationToken[i]=' ';
  this->authenticationToken="";
  this->PhysicalFileName_COMPUTED_DO_NOT_CHANGE="";
  this->Release();
}

WebWorker::WebWorker()
{ this->flagDeallocated=false;
  this->reset();
}

bool WebWorker::IamActive()
{ if (this->parent==0 || this->indexInParent==-1)
    return false;
  return this->parent->activeWorker==this->indexInParent;
}

bool WebWorker::IsFileExtensionOfBinaryFile(const std::string& fileExtension)
{ if (fileExtension==".png")
    return true;

  return false;
}

void WebWorker::SignalIamDoneReleaseEverything()
{ MacroRegisterFunctionWithName("WebWorker::SignalIamDoneReleaseEverything");
  if (!this->IamActive())
  { theLog << logger::red << "Signal done called on non-active worker" << logger::endL;
    return;
  }
//  std::cout << "got thus far xxxxxxx" << std:SignalIamDoneReleaseEverything:endl;
  this->pipeWorkerToServerControls.WriteAfterEmptying("close");
  theLog << logger::blue << "Worker " << this->indexInParent+1 << " finished, sending result. " << logger::endL;
  this->SendAllBytes();
  this->Release();
  theGlobalVariables.flagComputationCompletE=true;
  theGlobalVariables.flagComputationFinishedAllOutputSentClosing=true;
}

WebWorker::~WebWorker()
{ //Workers are not allowed to release resources in the destructor:
  //a Worker's destructor is called when expanding List<WebWorker>.
  this->flagDeallocated=true;
}

std::string WebWorker::GetMIMEtypeFromFileExtension(const std::string& fileExtension)
{ MacroRegisterFunctionWithName("WebWorker::GetMIMEtypeFromFileExtension");
  if (fileExtension==".html")
    return "Content-Type: text/html\r\n";
  if (fileExtension==".txt")
    return "Content-Type: text/plain\r\n";
  if (fileExtension==".png")
    return "Content-Type: image/png\r\n";
  if (fileExtension==".js")
    return "Content-Type: text/javascript\r\n";
  if (fileExtension==".ico")
    return "Content-Type: image/x-icon\r\n";
  if (fileExtension==".css")
    return "Content-Type: text/css\r\n";
  return "Content-Type: application/octet-stream\r\n";
}

int WebWorker::ProcessNonCalculator()
{ MacroRegisterFunctionWithName("WebWorker::ProcessNonCalculator");
  this->ExtractPhysicalAddressFromMainAddress();
  ProgressReportWebServer theProgressReport;
  theProgressReport.SetStatus("<br>Processing non-computational web-server request.");
  //theLog << this->ToStringShort() << "\r\n";
//  std::cout << " ere be i at this moment10\n";
  if (!FileOperations::GetPhysicalFileNameFromVirtual(this->RelativePhysicalFileName, this->PhysicalFileName_COMPUTED_DO_NOT_CHANGE))
  { stOutput << "HTTP/1.0 404 Object not found\r\nContent-Type: text/html\r\n\r\n";
    stOutput << "<html><body><b>File name deemed unsafe. "
    << "Please note that folder names are not allowed to contain dots and file names "
    << "are not allowed to start with dots.</b> There may be additional restrictions "
    << "on file names added for security reasons.</body></html>";
    return 0;
  }
  if (FileOperations::IsFolderUnsecure(this->PhysicalFileName_COMPUTED_DO_NOT_CHANGE))
    return this->ProcessFolder();
//  std::cout << " ere be i at this moment\n";
  if (!FileOperations::FileExistsUnsecure(this->PhysicalFileName_COMPUTED_DO_NOT_CHANGE))
  { stOutput << "HTTP/1.0 404 Object not found\r\nContent-Type: text/html\r\n\r\n";
    stOutput << "<html><body><b>File does not exist.</b>";
    if (this->flagMainAddressSanitized)
    { stOutput << "<hr>The original main address I extracted was: " << this->mainAddressNonSanitized
      << "<br>However, I do not allow addresses that contain dots (to avoid access to folders below the server). "
      << " Therefore I have sanitized the main address to: " << this->mainAddress;
    }
    stOutput << "<br><b> File display name:</b> "
    << this->mainAddress << "<br><b>Relative physical name of file:</b> " << this->RelativePhysicalFileName
    << this->mainAddress << "<br><b>Physical name of file relative to running environment:</b> " << this->PhysicalFileName_COMPUTED_DO_NOT_CHANGE
    ;
    stOutput << "<hr><hr><hr>Message details:<br>" << this->ToStringMessageUnsafe();
    stOutput << "</body></html>";
    return 0;
  }
  std::string fileExtension=FileOperations::GetFileExtensionWithDot(this->PhysicalFileName_COMPUTED_DO_NOT_CHANGE);
  bool isBinary=this->IsFileExtensionOfBinaryFile(fileExtension);
  std::fstream theFile;
  if (!FileOperations::OpenFileUnsecure(theFile, this->PhysicalFileName_COMPUTED_DO_NOT_CHANGE, false, false, !isBinary))
  { stOutput << "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";
    stOutput << "<html><body><b>Error: file appears to exist but I could not open it.</b> ";
    if (this->flagMainAddressSanitized)
    { stOutput << "<hr>The original main address I extracted was: " << this->mainAddressNonSanitized
      << "<br>However, I do not allow addresses that contain dots (to avoid access to folders below the server). "
      << " Therefore I have sanitized the main address to: " << this->mainAddress;
    }
    stOutput << "<br><b> File display name: </b>"
    << this->mainAddress << "<br><b>Relative physical file name: </b>"
    << this->RelativePhysicalFileName << "</body></html>";
    return 0;
  }
  theFile.seekp(0, std::ifstream::end);
  unsigned int fileSize=theFile.tellp();
  std::stringstream theHeader;
  if (fileSize>50000000)
  { theHeader << "HTTP/1.0 413 Payload Too Large\r\n"
    << "<html><body><b>Error: user requested file: " << this->RelativePhysicalFileName
    << " but it is too large, namely, " << fileSize << " bytes.</b></body></html>";
    this->QueueBytesForSending(theHeader.str());
    this->SendAllBytes();
    return 0;
  }
  std::stringstream reportStream;
  reportStream << "<br>Serving file " << this->RelativePhysicalFileName << " ...";
  ProgressReportWebServer theProgressReport2;
  theProgressReport2.SetStatus(reportStream.str());

  theHeader << "HTTP/1.0 200 OK\r\n" << this->GetMIMEtypeFromFileExtension(fileExtension)
  << "Content-length: " << fileSize << "\r\n\r\n";
  this->QueueStringForSending(theHeader.str());
  const int bufferSize=64*1024;
  this->bufferFileIO.SetSize(bufferSize);
  theFile.seekg(0);
  theFile.read(&this->bufferFileIO[0], this->bufferFileIO.size);
  int numBytesRead=theFile.gcount();
  ///////////////////
//  theLog << "*****Message summary begin\r\n" << theHeader.str();
//  theLog << "Sending file  " << this->RelativePhysicalFileName; << " with file extension " << fileExtension
//  << ", file size: " << fileSize;
//  theLog << "\r\n*****Message summary end\r\n";
  ///////////////////
  while (numBytesRead!=0)
  { this->bufferFileIO.SetSize(numBytesRead);
    this->QueueBytesForSending(this->bufferFileIO);
    this->bufferFileIO.SetSize(bufferSize);
    theFile.read(this->bufferFileIO.TheObjects, this->bufferFileIO.size);
    numBytesRead=theFile.gcount();
  }
  this->SendAllBytes();
  reportStream << " done!";
  theProgressReport2.SetStatus(reportStream.str());
  return 0;
}

int WebWorker::ProcessUnknown()
{ MacroRegisterFunctionWithName("WebWorker::ProcessUnknown");
  stOutput << "HTTP/1.0 501 Method Not Implemented\r\nContent-Type: text/html\r\n\r\n";
  stOutput << "<b>Requested method is not implemented. </b> <hr>The original message received from the server follows."
  << "<hr>\n" << this->ToStringMessageUnsafe();
  return 0;
}

bool WebWorker::ShouldDisplayLoginPage()
{ if (theGlobalVariables.userCalculatorRequestType=="login")
    return true;
  if (theGlobalVariables.flagUsingSSLinCurrentConnection && !theGlobalVariables.flagLoggedIn &&
      theGlobalVariables.userCalculatorRequestType!="compute")
    return true;
  if (theGlobalVariables.UserRequestMustBePromptedToLogInIfNotLoggedIn() && !theGlobalVariables.flagLoggedIn)
    return true;
  return false;
}

int WebWorker::ProcessCalculatorNoLoginInterpreterMode()
{ MacroRegisterFunctionWithName("WebWorker::ProcessCalculatorNoLoginInterpreterMode");
//  this->CheckConsistency();

  std::stringstream argumentProcessingFailureComments;
  if (!this->ProcessRawArgumentsNoLoginInterpreterMode
      (theParser.inputStringRawestOfTheRaw, argumentProcessingFailureComments))
  { stOutput << "Failed to process the calculator arguments. <b>"
    << argumentProcessingFailureComments.str() << "</b>";
    stOutput.Flush();
    return 0;
  }
//  stOutput << "DEBUG: got here!";
//  stOutput.Flush();
  //The user is assumed logged in.
  theGlobalVariables.flagLoggedIn=true;
  if (theGlobalVariables.userCalculatorRequestType=="calculatorExamples")
    return this->ProcessCalculatorExamples();
  if (theGlobalVariables.GetWebInput("error")!="")
    stOutput << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("error"));
  else if (argumentProcessingFailureComments.str()!="")
    stOutput << argumentProcessingFailureComments.str();
  if ((theGlobalVariables.userCalculatorRequestType=="submitProblem" ||
       theGlobalVariables.userCalculatorRequestType=="submitExercise")
      && theGlobalVariables.flagLoggedIn)
    return this->ProcessSubmitProblem();
  if (theGlobalVariables.UserGuestMode() && theGlobalVariables.userCalculatorRequestType=="submitExerciseNoLogin")
    return this->ProcessSubmitProblem();
  if ((theGlobalVariables.userCalculatorRequestType=="problemGiveUp" &&
       theGlobalVariables.flagLoggedIn) ||
      theGlobalVariables.userCalculatorRequestType== "problemGiveUpNoLogin")
    return this->ProcessProblemGiveUp();
  if ((theGlobalVariables.userCalculatorRequestType=="problemSolution" &&
       theGlobalVariables.flagLoggedIn) ||
      theGlobalVariables.userCalculatorRequestType== "problemSolutionNoLogin")
    return this->ProcessProblemSolution();
  if ((theGlobalVariables.userCalculatorRequestType=="submitProblemPreview" ||
       theGlobalVariables.userCalculatorRequestType=="submitExercisePreview" ) &&
      theGlobalVariables.flagLoggedIn)
    return this->ProcessSubmitProblemPreview();
  if (theGlobalVariables.userCalculatorRequestType=="submitExercisePreviewNoLogin" &&
      theGlobalVariables.UserGuestMode())
    return this->ProcessSubmitProblemPreview();
  if (theGlobalVariables.userCalculatorRequestType=="exercisesNoLogin")
    return this->ProcessExamPage();
  if (theGlobalVariables.userCalculatorRequestType=="editPage")
    return this->ProcessEditPage();
  if (theGlobalVariables.userCalculatorRequestType=="modifyPage")
    return this->ProcessModifyPage();
  if (theGlobalVariables.userCalculatorRequestType=="clonePage")
    return this->ProcessClonePage();
  return this->ProcessExamPageInterpreter();
}

int WebWorker::ProcessCalculator()
{ MacroRegisterFunctionWithName("WebWorker::ProcessCalculator");
//  this->CheckConsistency();
  ProgressReportWebServer theReport;
  std::stringstream argumentProcessingFailureComments;
//  stOutput <<"DEBUG: here i am";
  if (!this->ProcessRawArguments(theParser.inputStringRawestOfTheRaw, argumentProcessingFailureComments))
  { stOutput << "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"
    << "<html><body>" << "Failed to process the calculator arguments. <b>"
    << argumentProcessingFailureComments.str() << "</b></body></html>";
    stOutput.Flush();
    return 0;
  }
//  stOutput <<"DEBIG: here i am2";
//  std::cout << "DEBUG: processing connection " << theWebServer.NumConnectionsSoFar << std::endl;
  if (this->flagPasswordWasSubmitted && theGlobalVariables.userCalculatorRequestType!="changePassword" &&
      theGlobalVariables.userCalculatorRequestType!="activateAccount" //&&
//      !theGlobalVariables.flagIgnoreSecurityToWorkaroundSafarisBugs
      )
  { std::stringstream redirectedAddress;
    redirectedAddress << theGlobalVariables.DisplayNameExecutableWithPath << "?";
    for (int i=0; i<theGlobalVariables.webArguments.size(); i++)
      if (theGlobalVariables.webArguments.theKeys[i]!="password" &&
          theGlobalVariables.webArguments.theKeys[i]!="authenticationInsecure")
        redirectedAddress << theGlobalVariables.webArguments.theKeys[i] << "="
        << theGlobalVariables.webArguments.theValues[i] << "&";
//    std::cout << "DEBUG: redirect, connection: " << theWebServer.NumConnectionsSoFar << ", redirecting to: "
//    << redirectedAddress.str() << "\n";
    stOutput << "HTTP/1.1 303 See other\r\nLocation: ";
    stOutput << redirectedAddress.str();
    stOutput << "\r\n\r\n<html><head>"
    << "<meta http-equiv=\"refresh\" content=\"0; url='" << redirectedAddress.str()
    << "'\" /></head><body>Click <a href=\"" << redirectedAddress.str() << "\">"
    << " here "<< "</a> if your browser does not redirect the page automatically. </body></html>";
//    stOutput << "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";
//    stOutput << "DEBUG " << "\r\n" << redirectedAddress.str();
//    stOutput << "\r\n";
    stOutput.Flush();
    return 0;
  }
  stOutput << "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";
//  stOutput.Flush();
//  stOutput << "<html>DEBUG 123:" << argumentProcessingFailureComments.str();
//  if (theGlobalVariables.flagLoggedIn)
//    stOutput << "LOGGED in canyabelieveit?";
//  stOutput << "<br>got to this point, requesttype: " << theGlobalVariables.userCalculatorRequestType;
  if (theGlobalVariables.userCalculatorRequestType=="calculatorExamples")
    return this->ProcessCalculatorExamples();
//  stOutput << "<html><body>Gt to here!";
//  if (theGlobalVariables.GetWebInput("error")!="")
//    stOutput << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("error"));
//  else if (argumentProcessingFailureComments.str()!="")
//    stOutput << argumentProcessingFailureComments.str();
  if (theGlobalVariables.userCalculatorRequestType=="pause")
    return this->ProcessPauseWorker();
  if (theGlobalVariables.userCalculatorRequestType=="status")
    return this->ProcessServerStatus();
  if (theGlobalVariables.userCalculatorRequestType=="statusPublic")
    return this->ProcessServerStatusPublic();
  if (theGlobalVariables.userCalculatorRequestType=="indicator")
    return this->ProcessComputationIndicator();
  if (theGlobalVariables.userCalculatorRequestType=="monitor")
    return this->ProcessMonitor();
  //unless the worker is an indicator, it has no access to communication channels of the other workers
  this->parent->ReleaseNonActiveWorkers();
  if (theGlobalVariables.userCalculatorRequestType=="setProblemData")
    return this->ProcessSetProblemDatabaseInfo();
  if (theGlobalVariables.userCalculatorRequestType=="changePassword")
    return this->ProcessChangePassword();
  if (theGlobalVariables.userCalculatorRequestType=="changePasswordPage" ||
      theGlobalVariables.userCalculatorRequestType=="activateAccount")
    return this->ProcessChangePasswordPage();
  if (this->ShouldDisplayLoginPage())
    return this->ProcessLoginPage();
  if (theGlobalVariables.UserSecureNonAdminOperationsAllowed() &&
      theGlobalVariables.userCalculatorRequestType=="logout")
    return this->ProcessLogout();
  if (( theGlobalVariables.userCalculatorRequestType=="addEmails"||
        theGlobalVariables.userCalculatorRequestType=="addUsers"   )
      &&
      theGlobalVariables.flagLoggedIn)
    return this->ProcessAddUserEmails();
  if ((theGlobalVariables.userCalculatorRequestType=="submitProblem" ||
       theGlobalVariables.userCalculatorRequestType=="submitExercise")
      && theGlobalVariables.flagLoggedIn)
    return this->ProcessSubmitProblem();
  if (theGlobalVariables.UserGuestMode() && theGlobalVariables.userCalculatorRequestType=="submitExerciseNoLogin")
    return this->ProcessSubmitProblem();
  if ((theGlobalVariables.userCalculatorRequestType=="problemGiveUp" &&
       theGlobalVariables.flagLoggedIn) ||
      theGlobalVariables.userCalculatorRequestType== "problemGiveUpNoLogin")
    return this->ProcessProblemGiveUp();
  if ((theGlobalVariables.userCalculatorRequestType=="problemSolution" &&
       theGlobalVariables.flagLoggedIn) ||
      theGlobalVariables.userCalculatorRequestType== "problemSolution")
    return this->ProcessProblemSolution();
  if ((theGlobalVariables.userCalculatorRequestType=="submitProblemPreview" ||
       theGlobalVariables.userCalculatorRequestType=="submitExercisePreview" ) &&
      theGlobalVariables.flagLoggedIn)
    return this->ProcessSubmitProblemPreview();
  if (theGlobalVariables.userCalculatorRequestType=="submitExercisePreviewNoLogin" &&
      theGlobalVariables.UserGuestMode())
    return this->ProcessSubmitProblemPreview();
  if (( theGlobalVariables.userCalculatorRequestType=="examForReal" ||
        theGlobalVariables.userCalculatorRequestType=="exercises"))
  { if (theGlobalVariables.UserSecureNonAdminOperationsAllowed())
      return this->ProcessExamPage();
    else
      return this->ProcessLoginPage();
  }
  if (theGlobalVariables.userCalculatorRequestType=="exercisesNoLogin")
    return this->ProcessExamPage();
  if (theGlobalVariables.userCalculatorRequestType=="editPage")
    return this->ProcessEditPage();
  if (theGlobalVariables.userCalculatorRequestType=="modifyPage")
    return this->ProcessModifyPage();
  if (theGlobalVariables.userCalculatorRequestType=="clonePage")
    return this->ProcessClonePage();
    //  stOutput << "main request is: " << theGlobalVariables.userCalculatorRequestType
//  << "<br>web keys: " << theGlobalVariables.webFormArgumentNames.ToStringCommaDelimited()
//  << "<br>web entries: " << theGlobalVariables.webFormArguments.ToStringCommaDelimited();
  if (theGlobalVariables.userCalculatorRequestType=="browseDatabase" &&
      theGlobalVariables.flagLoggedIn &&
      theGlobalVariables.flagUsingSSLinCurrentConnection)
    return this->ProcessDatabase();
  theParser.inputString=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("mainInput"));
  theParser.flagShowCalculatorExamples=(theGlobalVariables.GetWebInput("showExamples")=="true");

  theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection=
  theWebServer.ReturnActiveIndicatorAlthoughComputationIsNotDone;
  this->OutputBeforeComputation();
  theWebServer.CheckExecutableVersionAndRestartIfNeeded(false);
////////////////////////////////////////////////
//  the initialization below moved to the start of the web server!
//  theParser.init();
////////////////////////////////////////////////
  if (theGlobalVariables.flagUsingBuiltInWebServer)
    theReport.SetStatus("OutputWeb: Computing...");
  this->flagProgressReportAllowed=true;
  if (theParser.inputString!="")
    theParser.Evaluate(theParser.inputString);
  this->flagProgressReportAllowed=false;
  if (theGlobalVariables.flagUsingBuiltInWebServer)
    theReport.SetStatus("OutputWeb: Computation complete, preparing output");
  theGlobalVariables.flagComputationCompletE=true;
  if (theGlobalVariables.flagUsingBuiltInWebServer)
    if (theGlobalVariables.flagOutputTimedOut)
    { theReport.SetStatus("OutputWeb: calling OutputResultAfterTimeout.");
      this->OutputResultAfterTimeout();
      theReport.SetStatus("OutputWeb: called OutputResultAfterTimeout, flushing.");
      stOutput.Flush();
      theReport.SetStatus("OutputWeb: called OutputResultAfterTimeout, exiting.");
      return 0;
    }
  theReport.SetStatus("OutputWeb:  calling OutputStandardResult");
  WebWorker::OutputStandardResult();
  theReport.SetStatus("OutputWeb: called OutputStandardResult, flushing.");
  stOutput.Flush();
  theReport.SetStatus("OutputWeb: called OutputStandardResult, exiting...");
  return 0;
}

std::string WebWorker::GetJavaScriptIndicatorFromHD()
{ std::stringstream out;
  out << " <!>\n";
  out << " <script type=\"text/javascript\"> \n";
  out << " var timeOutCounter=0;\n";
//  out << " var newReportString=\"\";\n";
  out << " var showProgress=false;";
  out << " function progressReport()\n";
  out << "{ var el = document.getElementById(\"idProgressReport\");	\n";
  out << "  if (!showProgress) \n";
  out << "  { el.style.display = 'none';\n";
  out << "    return;";
  out << "  }\n";
  out << "  el.style.display = '';\n";
  out << "  timeOutCounter++;\n";
  out << "  var oRequest = new XMLHttpRequest();\n";
  out << "  var sURL  = \"" << theGlobalVariables.DisplayNameProgressReport << "\";\n";
  out << "  oRequest.open(\"GET\",sURL,false);\n";
//  out << "  oRequest.setRequestHeader(\"Indicator\",navigator.userAgent);\n";
  out << "  oRequest.send(null)\n";
  out << "  if (oRequest.status==200)\n";
  out << "  { newReportString= oRequest.responseText;\n";
  out << "    el.innerHTML= \"<hr>Refreshing each second. Client time: ~\"+ timeOutCounter+\" second(s)<br>\" +newReportString+\"<hr>\";\n";
  out << "  }\n";
  out << "   window.setTimeout(\"progressReport()\",1000);\n";
  out << " }\n";
  out << " </script>\n";
  out << CGI::GetHtmlButton
  ("progressReportButton", "showProgress=!showProgress; progressReport()", "expand/collapse progress report");
  out << "<br><div "
  //<< "src=\"" << this->indicatorFileNameDisplaY << "\" "
  << "id=\"idProgressReport\" style=\"display:none\">\n";
  out << " </div>\n";
  out << " \n";
  out << " \n";
  return out.str();
}

std::string WebWorker::GetInsecureConnectionAngryMessage()
{ MacroRegisterFunctionWithName("WebWorker::GetInsecureConnectionAngryMessage");
  std::stringstream out;
  out
  << "<b>This is a reduced security connection.</b><br>"
  << "For secure connection: close safari & open in another browser: "
  << "<a href=\"https://calculator-algebra.org/calculator\">https://calculator-algebra.org/calculator</a>."
;
  return out.str();
}

std::string WebWorker::GetLoginHTMLinternal()
{ MacroRegisterFunctionWithName("WebWorker::GetLoginHTMLinternal");
  std::stringstream out;
//  out << "<hr>message main: "
//  << this->theMessage
//  << "<hr> main argument: "
//  << this->mainArgumentRAW;
  //if (theGlobalVariables.flagIgnoreSecurityToWorkaroundSafarisBugs)
//  { out
//    << WebWorker::GetInsecureConnectionAngryMessage()
//    << "<br>Please do not use a password you use in other sites here.</b>";
//    out << CGI::GetJavascriptSha1();
//    out << "<script type=\"text/javascript\">\n";
//    out
//    << "function submitAuthentication(){\n"
//    << " shaObj = new jsSHA('SHA-1','TEXT');\n"
//    << " alert(document.getElementById('password').value);\n"
//    << " shaObj.update(document.getElementById('username').value+document.getElementById('authenticationInsecure').value);\n"
//    << " var hash = shaObj.getHash('B64');\n"
//    << " document.getElementById('authenticationInsecure').value=hash;\n"
//    << " document.getElementById('authenticationToken').value= encodeURIComponent(hash);\n"
    //<< " alert(hash);\n"
//    << " document.getElementById('login').submit();"
//    << "}\n"
//    ;
//    out << "</script>\n";
//  }
  out << "<form name=\"login\" id=\"login\" action=\"calculator\" method=\"POST\" accept-charset=\"utf-8\">\n"
  <<  "User name:\n"
  << "<input type=\"text\" id=\"username\" name=\"username\" placeholder=\"username\" ";
  if (!this->flagAuthenticationTokenWasSubmitted
      &&
      theGlobalVariables.GetWebInput("username")!="")
    out << "value=\"" << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("username")) << "\"";
  out << "required>";
  out << "</input>\n";
  out << "<br>Password: ";
  //if (!theGlobalVariables.flagIgnoreSecurityToWorkaroundSafarisBugs)
  out << "<input type=\"password\" id=\"password\" name=\"password\" placeholder=\"password\" autocomplete=\"on\">";
  //else
    //out << "<input type=\"password\" id=\"authenticationInsecure\" "
    //<< "name=\"authenticationInsecure\" placeholder=\"password\" autocomplete=\"off\">";

  out << this->GetHtmlHiddenInputExercise() << this->GetHtmlHiddenInputs();
//  if (!theGlobalVariables.flagIgnoreSecurityToWorkaroundSafarisBugs)
  out << "<input type=\"submit\" value=\"Login\"></form>";
  //else
  //out << "</form><button onclick=\"submitAuthentication();\">Login</button>";
//  if (theGlobalVariables.flagIgnoreSecurityToWorkaroundSafarisBugs)
//  { out << "<br>Ignoring ssl encryption as a workaround. "
//    << " Only a thin javascript layer of in-browser encryption is protecting your password. "
//    << " Javascript encryption is courtesy of Brian Turek "
//    << "(<a href=\"http://caligatio.github.com/jsSHA/\">http://caligatio.github.com/jsSHA/</a>). "
//    ;
//  }
  out << this->ToStringCalculatorArgumentsHumanReadable();
  return out.str();
}

std::string WebWorker::GetHtmlHiddenInputComputation()
{ return "<input type=\"hidden\" name=\"request\" id=\"request\" value=\"compute\">\n";
}

std::string WebWorker::GetHtmlHiddenInputExercise()
{ return "<input type=\"hidden\" name=\"request\" id=\"request\" value=\"exercises\">";
}

std::string WebWorker::GetChangePasswordPage()
{ MacroRegisterFunctionWithName("WebWorker::GetChangePasswordPage");
  std::stringstream out;
  out << "<html>";
  out << "<head>";
  out
  << "<script type=\"text/javascript\"> \n"
  << "function submitChangePassRequest(){\n"
  << "  spanVerification = document.getElementById(\"passwordChangeResult\");\n"
  << "  inputUser= document.getElementById(\"username\");\n"
  << "  inputPassword= document.getElementById(\"password\");\n"
  << "  inputNewPassword= document.getElementById(\"newPassword\");\n"
  << "  inputReenteredPassword= document.getElementById(\"reenteredPassword\");\n"
  << "  params=\"request=changePassword\"\n"
  << "          + \"&username=\" + encodeURIComponent(inputUser.value) \n"
  << "          + \"&password=\"+encodeURIComponent(inputPassword.value)\n"
  << "          + \"&newPassword=\"+encodeURIComponent(inputNewPassword.value)\n"
  << "          + \"&reenteredPassword=\"+encodeURIComponent(inputReenteredPassword.value)\n"
  << "  ;\n"
  << "  var https = new XMLHttpRequest();\n"
  << "  https.open(\"POST\", \"" << theGlobalVariables.DisplayNameExecutableWithPath << "\", true);\n"
  << "  https.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\");\n"
  ;
  //Old code, submits all answers. May need to be used as an alternative
  //submission option.
  //  for (int i=0; i<this->theContent.size; i++)
  //    if (this->IsStudentAnswer(this->theContent[i]))
  //      out << "  inputParams+=\"&calculatorAnswer" << this->theContent[i].GetKeyValue("id") << "=\"+encodeURIComponent("
  //      << "document.getElementById('" << this->theContent[i].GetKeyValue("id") << "').value);\n";
  out
  << "  https.onload = function() {\n"
  << "    spanVerification.innerHTML=https.responseText;\n"
  << "  }\n"
  << "  https.send(params);\n"
  << "}\n"
  << "</script>";
  out << "</head>";
  out << "<body> ";
//    << "  window.location='calculator?username='+GlobalUser+'&authenticationToken='+GlobalAuthenticationToken;";
  theWebServer.CheckExecutableVersionAndRestartIfNeeded(true);
//  out << "<form name=\"login\" id=\"login\" action=\"calculator\" method=\"GET\" accept-charset=\"utf-8\">";
  if (!theGlobalVariables.flagLoggedIn &&
      theGlobalVariables.userCalculatorRequestType!="activateAccount")
    out
    <<  "User name or email: "
    << "<input type=\"text\" id=\"username\" placeholder=\"username\" "
    << "value=\"" << theGlobalVariables.userDefault << "\" "
    << "required>";
  else
    out
    <<  "User: " << theGlobalVariables.userDefault
    << "<input type=\"hidden\" id=\"username\" placeholder=\"username\" "
    << "value=\"" << theGlobalVariables.userDefault << "\" "
    << "required>";

  if (theGlobalVariables.userCalculatorRequestType=="activateAccount")
    out << "<input type=\"hidden\" id=\"password\" value=\""
    << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("activationToken")) << "\">";
  else
    out << "<br>Password: "
    << "<input type=\"password\" id=\"password\" placeholder=\"password\">\n";
  out << "<br>New password: \n"
  << "<input type=\"password\" id=\"newPassword\" placeholder=\"password\">\n"
  << "<br>Re-enter new password: \n"
  << "<input type=\"password\"  id=\"reenteredPassword\" placeholder=\"password\" "
  << "onkeyup=\"if (event.keyCode == 13) submitChangePassRequest();\">\n"
  << "<button  onclick=\"submitChangePassRequest() \"> Submit</button>\n"
  << "<span id=\"passwordChangeResult\"> </span>\n"
//  << "</form>"
  ;
  out << this->ToStringCalculatorArgumentsHumanReadable();
  out << "</body></html>";
  return out.str();
}

int WebWorker::ProcessChangePassword()
{ MacroRegisterFunctionWithName("WebWorker::ProcessChangePassword");
  //stOutput << " ere i am";
//  if (theGlobalVariables.UserDebugFlagOn())
  theGlobalVariables.SetWebInpuT("debugFlag", "true");
//  stOutput << "DEBUG: " << this->ToStringCalculatorArgumentsHumanReadable();
  this->authenticationToken="";
  if (!theGlobalVariables.flagLoggedIn || !theGlobalVariables.flagUsingSSLinCurrentConnection)
  { stOutput << "<span style=\"color:red\"><b> Password change available only to logged in users via SSL.</b></span>";
    stOutput.Flush();
    return 0;
  }
  std::string newPassword=theGlobalVariables.GetWebInput("newPassword");
  std::string reenteredPassword=theGlobalVariables.GetWebInput("reenteredPassword");
  if (newPassword!=reenteredPassword)
  { stOutput << "<span style=\"color:red\"><b> Passwords don't match.</b></span>";
    return 0;
  }
  std::stringstream commentsOnFailure;
  std::string newAuthenticationToken;
  if (!DatabaseRoutinesGlobalFunctions::SetPassword(theGlobalVariables.userDefault, newPassword, newAuthenticationToken, commentsOnFailure))
  { stOutput << "<span style=\"color:red\"><b>" << commentsOnFailure.str() << "</b></span>";
    return 0;
  }
  if (!DatabaseRoutinesGlobalFunctions::SetEntry
      (theGlobalVariables.userDefault, "users", "activationToken", "activated", commentsOnFailure))
    stOutput << "<span style=\"color:red\"><b>Failed to set activationToken: "
    << commentsOnFailure.str() << "</b></span>";

  stOutput << "<span style=\"color:green\"> <b>Password change successful. </b></span>";
  stOutput
  << "<meta http-equiv=\"refresh\" content=\"0; url="
  << theGlobalVariables.DisplayNameExecutableWithPath  << "?request=login"
  << "&username="
  << CGI::StringToURLString(theGlobalVariables.userDefault)
  << "&authenticationToken=" << CGI::StringToURLString(newAuthenticationToken)
  << "&fileName="
  << theGlobalVariables.GetWebInput("fileName")
  << "\" />"
  ;
  stOutput.Flush();
  return 0;
}

int WebWorker::ProcessChangePasswordPage()
{ MacroRegisterFunctionWithName("WebWorker::ProcessChangePasswordPage");
  stOutput << this->GetChangePasswordPage();
  return 0;
}

int WebWorker::ProcessLogout()
{ MacroRegisterFunctionWithName("WebWorker::ProcessLogout");
  DatabaseRoutinesGlobalFunctions::LogoutViaDatabase();
  this->authenticationToken="";
  stOutput << this->GetLoginPage();
  return 0;
}

int WebWorker::ProcessExamPage()
{ MacroRegisterFunctionWithName("WebWorker::ProcessExamPage");
  stOutput << this->GetExamPage();
  stOutput.Flush();
  return 0;
}

int WebWorker::ProcessExamPageInterpreter()
{ stOutput << this->GetExamPageInterpreter();
  stOutput.Flush();
  return 0;
}

int WebWorker::ProcessLoginPage()
{ MacroRegisterFunctionWithName("WebWorker::ProcessLoginPage");
  stOutput << this->GetLoginPage();
  stOutput.Flush();
  return 0;
}

int WebWorker::ProcessDatabase()
{ MacroRegisterFunctionWithName("WebWorker::ProcessDatabase");
  stOutput << this->GetDatabasePage();
  stOutput.Flush();
  return 0;
}

int WebWorker::ProcessEditPage()
{ MacroRegisterFunctionWithName("WebWorker::ProcessEditPage");
  stOutput << this->GetEditPageHTML();
  stOutput.Flush();
  return 0;
}

int WebWorker::ProcessClonePage()
{ MacroRegisterFunctionWithName("WebWorker::ProcessClonePage");
  stOutput << this->GetClonePageResult();
  stOutput.Flush();
  return 0;
}

std::string WebWorker::GetLoginPage()
{ MacroRegisterFunctionWithName("WebWorker::GetLoginPage");
  std::stringstream out;
  out << "<html>"
  << WebWorker::GetJavascriptStandardCookies()
  << "<body ";
  out << "onload=\"loadSettings();  ";
  if (!this->flagAuthenticationTokenWasSubmitted &&
      theGlobalVariables.flagUsingSSLinCurrentConnection)
    if (theGlobalVariables.GetWebInput("username")=="")
      out
      << "if (document.getElementById('authenticationToken') !=null)"
      << "  if (document.getElementById('authenticationToken').value!='')"
      << "    document.getElementById('login').submit();"
//    << "alert('was about to submit');"
    ;
//    << "  window.location='calculator?username='+GlobalUser+'&authenticationToken='+GlobalAuthenticationToken;";
  out << "\">\n";
  theWebServer.CheckExecutableVersionAndRestartIfNeeded(true);
  out << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("error"));
  out << WebWorker::GetLoginHTMLinternal() << "</body></html>";
  return out.str();
}

std::string WebWorker::GetJavascriptSubmitEmails(const std::string& homeFile)
{ std::stringstream out;
  out
  << "<script type=\"text/javascript\" id=\"scriptSubmitEmails\"> \n"
  << "function addEmailsOrUsers(idEmailList, problemCollectionName, idOutput, userRole, idExtraInfo, requestType){\n"
  << "  spanOutput = document.getElementById(idOutput);\n"
  << "  if (spanOutput==null){\n"
  << "    spanOutput = document.createElement('span');\n"
  << "    document.body.appendChild(spanOutput);\n"
  << "    spanOutput.innerHTML= \"<span style='color:red'> ERROR: span with id \" + idEmailList + \"MISSING! </span>\";\n"
  << "  }\n"
  << "  spanEmailList = document.getElementById(idEmailList);\n"
  << "  spanExtraInfo = document.getElementById(idExtraInfo);\n"
  << "  inputParams='request='+requestType;\n"
  << "  inputParams+='&userRole='+userRole;\n"
  << "  inputParams+='&" << theGlobalVariables.ToStringCalcArgsNoNavigation() << "';\n"
  << "  inputParams+='&mainInput=' + encodeURIComponent(spanEmailList.value);\n"
  << "  inputParams+='&extraInfo=' + encodeURIComponent(spanExtraInfo.value);\n"
//  << "  inputParams+='&currentExamHome=' + problemCollectionName;\n"
  << "  inputParams+='&currentExamHome=" << CGI::StringToURLString(homeFile) << "';\n"
  << "  var https = new XMLHttpRequest();\n"
  << "  https.open(\"POST\", \"" << theGlobalVariables.DisplayNameExecutableWithPath << "\", true);\n"
  << "  https.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\");\n"
  << "  https.onload = function() {\n"
  << "    spanOutput.innerHTML=https.responseText;\n"
//  << "    code=document.getElementById('progressReportJavascript').innerHTML;"
//  << "    eval.call(code);\n"
  //<< "    p();\n"
//  << "    eval(spanOutput.innerHTML);\n"
//  << "    if (typeof progressReport == 'function')\n"
//  << "      progressReport();\n"
  << "  }\n"
  << "  https.send(inputParams);\n"
  << "}\n"
  << "</script>";
  return out.str();
}

std::string WebWorker::GetJavascriptHideHtml()
{ std::stringstream output;
  output << " <!>\n";
  output << " <script type=\"text/javascript\"> \n";
  output << " function switchMenu(obj)\n";
  output << " { var el = document.getElementById(obj);	\n";
  output << "   if ( el.style.display != \"none\" ) \n";
  output << "     el.style.display = 'none';\n";
  output << "   else \n";
  output << "     el.style.display = '';\n";
  output << " }\n";
  output << " function hideItem(obj)\n";
  output << " { document.getElementById(obj).style.display=\"none\";\n";
  output << " }\n";
  output << " function showItem(obj)\n";
  output << " { document.getElementById(obj).style.display=\"\";\n";
  output << " }\n";
  output << " </script>\n";
  return output.str();
}

bool WebWorker::IsAllowedAsRequestCookie(const std::string& input)
{ return input!="login" && input!="logout"
  && input!="changePassword"
  && input!="changePasswordPage";
}

std::string WebWorker::GetJavascriptStandardCookies()
{ std::stringstream out;
  out
  << "<script type=\"text/javascript\"> \n"
  << "function getCookie(c_name)\n"
  << "{ VPFcookie=document.cookie.split(\";\");\n"
  << "  for (i=0;i<VPFcookie.length;i++)\n"
  << "  { x=VPFcookie[i].substr(0,VPFcookie[i].indexOf(\"=\"));\n"
  << "  	y=VPFcookie[i].substr(VPFcookie[i].indexOf(\"=\")+1);\n"
  << "    x=x.replace(/^\\s+|\\s+$/g,\"\");\n"
  << "    if (x==c_name)\n"
  << "      return unescape(y);\n"
  << "  }\n"
  << "  return \"\";\n"
  << "}\n"
  << "\n"
  << "function addCookie(theName, theValue, exdays, secure)\n"
  << "{ exdate= new Date();\n"
  << "  exdate.setDate(exdate.getDate() + exdays);\n"
  << "  c_value=escape(theValue) + ((exdays==null) ? \"\" : \"; expires=\"+exdate.toUTCString());\n"
  << "  if(secure)\n"
  << "    c_value+=\"; secure;\"; \n"
  << "  document.cookie=theName + \"=\" + c_value;\n"
  << "}\n"
  << "function storeSettings()\n"
  << "{ theCalculatorForm=document.getElementById(\"mainInputID\");  \n"
  << "  //alert(theCalculatorForm.style.width);\n"
  << "  if (theCalculatorForm!=null){\n"
  << "    addCookie(\"widthCalculatorText\", theCalculatorForm.style.width, 100);  \n"
  << "    addCookie(\"heightCalculatorText\", theCalculatorForm.style.height, 100);\n"
  << "  }\n"
//  << " storeSettingsProgress();\n"
  << "}\n";
  out
  << "function storeSettingsProgress(){\n";
  if (theGlobalVariables.GetWebInput("debugFlag")!="")
    out << "  addCookie(\"debugFlag\", \"" << theGlobalVariables.GetWebInput("debugFlag") << "\", 100, false);  \n";
  if (theGlobalVariables.GetWebInput("studentSection")!="")
    out << "  addCookie(\"studentSection\", \"" << theGlobalVariables.GetWebInput("studentSection") << "\", 100, true);  \n";
  if (theGlobalVariables.GetWebInput("studentView")!="")
    out << "  addCookie(\"studentView\", \"" << theGlobalVariables.GetWebInput("studentView") << "\", 100, true);  \n";
  if (this->IsAllowedAsRequestCookie(theGlobalVariables.userCalculatorRequestType))
    out << "  addCookie(\"request\", \"" << theGlobalVariables.userCalculatorRequestType << "\", 100, false);\n";
  if (
      //(theGlobalVariables.flagLoggedIn || theGlobalVariables.UserGuestMode()) &&
       theGlobalVariables.userCalculatorRequestType!="" &&
       theGlobalVariables.userCalculatorRequestType!="compute")
  { if (theGlobalVariables.GetWebInput("fileName")!="")
      out << "  addCookie(\"fileName\", \""
      << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("fileName"))
      << "\", 100, false);\n";
    if (theGlobalVariables.GetWebInput("currentExamHome")!="")
      out << "  addCookie(\"currentExamHome\", \""
      << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome"))
      << "\", 100, false);\n";
  }
  out
  << "}\n";
  out
  << "function storeSettingsSecurity(){\n";
  if (theGlobalVariables.flagLoggedIn)
  { out << "   addCookie(\"authenticationToken\", \""
    << this->authenticationToken << "\", 150, true);"
    << "//150 days is a little longer than a semester\n"
    << "  addCookie(\"username\", \"" << theGlobalVariables.userDefault << "\", 150, true);\n";
  }
  out << "}\n";
  out
//  << "function getCalculatorCGIsettings(){\n"
//  << "  result =\"examStatus=\"+getCookie(\"examStatus\");\n"
//  << "  result+=\"&username=\"+getCookie(\"username\");\n"
//  << "  result+=\"&authenticationToken=\"+getCookie(\"authenticationToken\");\n"
//  << "  result+=\"&fileName=\"+getCookie(\"fileName\");\n"
//  << "  return result;\n"
//  << "}\n"
  << "function loadSettings(){\n"
  << "  storeSettingsSecurity();\n"
  << "  storeSettingsProgress();\n"
  << "  theCalculatorForm=document.getElementById(\"mainInputID\");  \n"
  << "  if (theCalculatorForm!=null){\n"
  << "    theOldWidth=getCookie(\"widthCalculatorText\");\n"
  << "    theOldHeight=getCookie(\"heightCalculatorText\");\n"
  << "    theCalculatorForm.style.width  = theOldWidth;\n"
  << "    theCalculatorForm.style.height = theOldHeight;\n"
  << "  }\n"
  << "  if (document.getElementById(\"debugFlag\")!=null)\n "
  << "    if(getCookie(\"debugFlag\")!='')\n"
  << "      document.getElementById(\"debugFlag\").value=getCookie(\"debugFlag\");\n"
  << "  if (document.getElementById(\"studentView\")!=null)\n "
  << "    if(getCookie(\"studentView\")!='')\n"
  << "      document.getElementById(\"studentView\").value=getCookie(\"studentView\");\n"
  << "  if (document.getElementById(\"studentSection\")!=null)\n "
  << "    if(getCookie(\"studentSection\")!='')\n"
  << "      document.getElementById(\"studentSection\").value=getCookie(\"studentSection\");\n"
  << "  if (document.getElementById(\"request\")!=null)\n "
  << "    if(getCookie(\"request\")!='')\n"
  << "      if(getCookie(\"request\")!='logout' && getCookie(\"request\")!='login' )\n"
  << "        document.getElementById(\"request\").value=getCookie(\"request\");\n"
  << "  if (document.getElementById(\"fileName\")!=null)\n "
  << "    if(getCookie(\"fileName\")!='')\n"
  << "      document.getElementById(\"fileName\").value=getCookie(\"fileName\");\n"
  << "  if (document.getElementById(\"currentExamHome\")!=null)\n "
  << "    if(getCookie(\"currentExamHome\")!='')\n"
  << "      document.getElementById(\"currentExamHome\").value=getCookie(\"currentExamHome\");\n"
  << "  if (document.getElementById(\"authenticationToken\")!=null)\n"
  << "    if(getCookie(\"authenticationToken\")!='')\n"
  << "      document.getElementById(\"authenticationToken\").value=getCookie(\"authenticationToken\");\n "
  << "  if (document.getElementById(\"usernameHidden\")!=null)\n"
  << "    if(getCookie(\"username\")!='')\n"
  << "      document.getElementById(\"usernameHidden\").value=getCookie(\"username\");\n "
  << "  if (document.getElementById(\"ignoreSecurity\")!=null)\n"
  << "    if(getCookie(\"ignoreSecurity\")!='')\n"
  << "      document.getElementById(\"ignoreSecurity\").value=getCookie(\"ignoreSecurity\");\n "
  << "}\n";
  out << " </script>\n";
  return out.str();
}

std::string WebWorker::GetJavaScriptIndicatorBuiltInServer(int inputIndex, bool callProgressReport)
{ MacroRegisterFunctionWithName("WebWorker::GetJavaScriptIndicatorBuiltInServer");
  std::stringstream out;
  out << " <!>\n";
//  out << "\n<br>\n<button onclick=\"progressReport()\">Manual report</button>";
  out << "\n<br>\n<button id=\"idButtonSendTogglePauseRequest\" onclick=\"SendTogglePauseRequest()\">Pause</button>";
  out << "<span id=\"idPauseToggleServerResponse\"></span>\n";
  out << "<span id=\"idProgressReportTimer\"></span>\n";
  out << "\n<br>\n<div id=\"idProgressReport\"></div>\n";
  out << "\n<script type=\"text/javascript\" id=\"progressReportJavascript\"> \n";
  out << "var isPaused=false;\n";
  out << "var isFinished=false;\n";
  out << "var timeIncrementInTenthsOfSecond=4;//measured in tenths of a second\n";
  out << "var timeOutCounter=0;//measured in tenths of a second\n";
  out << "\nfunction progressReport()\n";
  out << "{ if (isFinished)\n";
  out << "    return;\n";
  out << "  var progReport = document.getElementById(\"idProgressReport\");	\n";
  out << "  var progReportTimer = document.getElementById(\"idProgressReportTimer\");	\n";
  out << "  if(isPaused)\n";
  out << "    return;\n";
  out << "  progReportTimer.innerHTML =\"<hr>Refreshing every \"+timeIncrementInTenthsOfSecond/10+\" second(s). Client time: ~\"+ Math.floor(timeOutCounter/10)+\" second(s)<br>\";\n";
  out << "  progReportTimer.style.display = '';\n";
  out << "  progReport.style.display = '';\n";
  out << "  timeOutCounter+=timeIncrementInTenthsOfSecond;\n";
  out << "  var oRequest = new XMLHttpRequest();\n";
  if (inputIndex==-1)
    theLog << logger::red << "Worker index in parent is -1!!!" << logger::endL;
  else
    theLog << "Worker index: " << inputIndex << logger::endL;
  out << "  var sURL  = \"" << theGlobalVariables.DisplayNameExecutableWithPath << "?request=indicator&mainInput="
  << inputIndex+1 << "\";\n";
  out << "  oRequest.open(\"GET\",sURL,false);\n";
//  out << "  oRequest.setRequestHeader(\"Indicator\",navigator.userAgent);\n";
  out << "  oRequest.send(null)\n";
  out << "  if (oRequest.status==200)\n";
  out << "  { newReportString= oRequest.responseText;\n";
  out << "    if (oRequest.responseText==\"finished\")\n";
  out << "    { isFinished=true;\n";
  out << "      document.getElementById(\"idPauseToggleServerResponse\").innerHTML=\"Computation finished.\";\n";
  out << "      return;\n";
  out << "    }\n";
  out << "    if (oRequest.responseText!=\"\")\n";
  out << "      progReport.innerHTML=newReportString+\"<hr>\";\n";
  out << "  }\n";
  out << "   window.setTimeout(\"progressReport()\",timeIncrementInTenthsOfSecond*100);\n";
  out << " }\n";
  out << "function SendTogglePauseRequest()\n";
  out << "{ if (isFinished)\n";
  out << "    return;\n";
  out << "  var pauseRequest = new XMLHttpRequest();\n";
  theLog << "Generating indicator address for worker number " << inputIndex+1 << "." << logger::endL;
  out << "  pauseURL  = \"" << theGlobalVariables.DisplayNameExecutableWithPath
  << "?request=pause&mainInput=" << inputIndex+1 << "\";\n";
  out << "  pauseRequest.open(\"GET\",pauseURL,false);\n";
//  out << "  oRequest.setRequestHeader(\"Indicator\",navigator.userAgent);\n";
  out << "  pauseRequest.send(null)\n";
  out << "  if (pauseRequest.status!=200)\n";
  out << "    return;\n";
  out << "  if(pauseRequest.responseText==\"paused\")\n";
  out << "    isPaused=true;\n";
  out << "  if(pauseRequest.responseText==\"unpaused\")\n";
  out << "    isPaused=false;\n";
  out << "  if (isPaused)\n";
  out << "    document.getElementById(\"idButtonSendTogglePauseRequest\").innerHTML=\"Continue\";\n";
  out << "  else\n";
  out << "    document.getElementById(\"idButtonSendTogglePauseRequest\").innerHTML=\"Pause\";\n";
  out << "  document.getElementById(\"idPauseToggleServerResponse\").innerHTML=pauseRequest.responseText;\n";
  out << "  if (!isPaused)\n";
  out << "    progressReport();\n";
  out << "}\n";
//  out << " progressReport();";
//  out << " var newReportString=\"\";\n";
  if (callProgressReport)
    out << "progressReport();\n";
  out << " </script>\n";
  out << " \n";
  out << " \n";
  return out.str();
}

std::string WebWorker::GetDatabasePage()
{ MacroRegisterFunctionWithName("WebWorker::GetDatabasePage");
  std::stringstream out;
  out << "<html>"
  << "<head>"
  << CGI::GetCalculatorStyleSheetWithTags()
  << WebWorker::GetJavascriptStandardCookies()
  << "</head>"
  << "<body onload=\"loadSettings();\">\n";
  out << "<nav>" << theGlobalVariables.ToStringNavigation() << "</nav>";
#ifdef MACRO_use_MySQL
  DatabaseRoutines theRoutines;
  if (!theGlobalVariables.UserDefaultHasAdminRights() || !theGlobalVariables.flagLoggedIn)
    out << "Browsing database allowed only for logged-in admins.";
  else
    out << "<section>" << theRoutines.ToStringCurrentTableHTML() << "</section>";
#else
out << "<b>Database not available. </b>";
#endif // MACRO_use_MySQL
  out << this->ToStringCalculatorArgumentsHumanReadable();
  out << "</body></html>";
  return out.str();
}

int WebWorker::ServeClient()
{ MacroRegisterFunctionWithName("WebWorker::ServeClient");
  ProgressReportWebServer theReport;
  theReport.SetStatus("All bytes from client received, processing. Worker process in use ...");
  theGlobalVariables.flagComputationStarted=true;
  theGlobalVariables.IndicatorStringOutputFunction=WebServer::PipeProgressReportToParentProcess;
  //std::cout << "DEBUG: serving client on connection: " << theWebServer.NumConnectionsSoFar << " address raw: "
  //<< this->mainAddresSRAW << " message raw: " << this->mainArgumentRAW;

  if (this->requestType==this->requestGetCalculator || this->requestType==this->requestPostCalculator)
  { theParser.inputStringRawestOfTheRaw=this->mainArgumentRAW;
    this->mainArgumentRAW="";
    //theParser.javaScriptDisplayingIndicator=this->GetJavaScriptIndicatorBuiltInServer(true);
    theParser.javaScriptDisplayingIndicator="";
    this->ProcessCalculator();
  } else if (this->requestType==this->requestGetNotCalculator)
    this->ProcessNonCalculator();
  else if (this->requestType==this->requestUnknown)
    this->ProcessUnknown();
  else
    crash << "Request type not parsed properly. " << crash;
  if (theGlobalVariables.theThreads.size==1)
    crash << "Number of threads must be at least 2 in this point of code..." << crash;
  this->parent->ReleaseNonActiveWorkers();
  theReport.SetStatus("Reply to client prepared, proceeding to send and release all resources...");
  this->SignalIamDoneReleaseEverything();
  return 0;
}

void WebWorker::ReleaseKeepInUseFlag()
{ MacroRegisterFunctionWithName("WebWorker::ReleaseMyPipe");
  this->pipeWorkerToServerTimerPing.Release();
  this->pipeWorkerToServerControls.Release();
  this->pipeServerToWorkerRequestIndicator.Release();
  this->pipeWorkerToServerIndicatorData.Release();
  this->pipeWorkerToServerUserInput.Release();
  this->pipeWorkerToServerWorkerStatus.Release();
  this->PauseComputationReportReceived.Release();
  this->PauseWorker.Release();
  this->PauseIndicatorPipeInUse.Release();
  this->timeOfLastPingServerSideOnly=-1;
  WebServer::Release(this->connectedSocketID);
}

void WebWorker::Release()
{ this->ReleaseKeepInUseFlag();
  this->flagInUse=false;
}

void WebWorker::OutputShowIndicatorOnTimeout()
{ MacroRegisterFunctionWithName("WebWorker::OutputShowIndicatorOnTimeout");
  this->PauseIndicatorPipeInUse.RequestPausePauseIfLocked();
  theGlobalVariables.flagOutputTimedOut=true;
  theGlobalVariables.flagTimedOutComputationIsDone=false;
  ProgressReportWebServer theReport("WebServer::OutputShowIndicatorOnTimeout");
  theLog << logger::blue << "Computation timeout, sending progress indicator instead of output. " << logger::endL;
  bool useTableTags=(theGlobalVariables.userCalculatorRequestType!="addEmails");
  if (useTableTags)
    stOutput << "</td></tr>";
  else
    stOutput << "Not using table tags, calc request type: " << theGlobalVariables.userCalculatorRequestType;
  if (theGlobalVariables.flagTimeOutExplanationAlreadyDisplayed)
  { if (useTableTags)
      stOutput << "<tr><td>";
    stOutput << "Your computation is taking more than " << theGlobalVariables.MaxComputationTimeBeforeWeTakeAction
    << " seconds. ";
    if (useTableTags)
      stOutput << "</td></tr>";
  }
  if (useTableTags)
    stOutput << "<tr><td>";
  stOutput << "A progress indicator, as reported by your current computation, is displayed below. "
  << "When done, your computation result will be displayed below. ";
  if (useTableTags)
    stOutput << "</td></tr>";
  if (useTableTags)
    stOutput << "<tr><td>";
  stOutput << this->GetJavaScriptIndicatorBuiltInServer(this->indexInParent, useTableTags);
  if (useTableTags)
    stOutput << "</td></tr>" << "</table></body></html>";

//  theLog << logger::red << "Indicator: sending all bytes" << logger::endL;
  theReport.SetStatus("WebServer::OutputShowIndicatorOnTimeout: sending all bytes.");
  this->SendAllBytes();
  theReport.SetStatus("WebServer::OutputShowIndicatorOnTimeout: all bytes sent.");
//  theLog << logger::blue << "Indicator: sending all bytes DONE" << logger::endL;
  for (int i=0; i<this->parent->theWorkers.size; i++)
    if (i!=this->indexInParent)
      this->parent->theWorkers[i].Release();
  //this->parent->Release(this->pipeServerToWorkerControls[0]);
  //this->parent->Release(this->pipeServerToWorkerControls[1]);
  //this->parent->Release(this->pipeWorkerToServerControls[0]);
  //this->parent->Release(this->pipeWorkerToServerControls[1]);
//  this->parent->Release(this->pipeServerToWorkerIndicator[0]);
//  this->parent->Release(this->pipeServerToWorkerIndicator[1]);
//  this->parent->Release(this->pipeWorkerToServerIndicator[0]);
//  this->parent->Release(this->pipeWorkerToServerIndicator[1]);
  this->parent->Release(this->connectedSocketID);
  //set flags properly:
  //we need to rewire the standard output and the crashing mechanism:
  crash.CleanUpFunction=WebWorker::OutputCrashAfterTimeout;
  //note that standard output cannot be rewired in the beginning of the function as we use the old stOutput
  stOutput.theOutputFunction=WebWorker::StandardOutputAfterTimeOut;
  theReport.SetStatus("WebServer::OutputShowIndicatorOnTimeout: continuing computation.");
  this->PauseIndicatorPipeInUse.ResumePausedProcessesIfAny();
  theReport.SetStatus("WebServer::OutputShowIndicatorOnTimeout: exiting function.");
//  this->SignalIamDoneReleaseEverything();
//  theLog << consoleGreen("Indicator: released everything and signalled end.") << logger::endL;
}

std::string WebWorker::ToStringStatus()const
{ std::stringstream out;
//  if (theWebServer.currentlyConnectedAddresses.GetCoefficientsSum()!=theWebServer.
  out << "<br>Worker " << this->indexInParent+1;
  if (this->flagInUse)
  { if (this->parent->activeWorker==this->indexInParent)
      out << ", <span style=\"color:green\"><b>current process</b></span>";
    else
      out << ", <b>in use</b>";
    out << ", <a href=\"calculator?request=monitor&mainInput=" << this->indexInParent +1 << "\">monitor process "
    << this->indexInParent +1 << "</a>";
  } else
    out << ", not in use";
  if (this->displayUserInput!="")
    out << ", user input: <span style=\"color:blue\">" << this->displayUserInput << "</span>";
  out << ", connection " << this->connectionID << ", process ID: ";
  if (this->ProcessPID!=0)
    out << this->ProcessPID;
  else
    out << "(not accessible)";
  out << ", socketID: ";
  if (this->connectedSocketID==-1)
    out << "released in current process, value before release: " << this->connectedSocketIDLastValueBeforeRelease;
  else
    out << this->connectedSocketID;
  out << ". ";
  out << " Server time at last ping: " << this->timeOfLastPingServerSideOnly << " seconds. ";
  if (this->pingMessage!="")
    out << " Message at last ping: " << this->pingMessage;
  if (this->status!="")
    out << "<br><span style=\"color:red\"><b> Status: " << this->status << "</b></span><br>";
  out << "Pipe indices: " << this->pipeWorkerToServerControls.ToString()
  << ", " << this->pipeWorkerToServerTimerPing.ToString()
  << ", " << this->pipeServerToWorkerRequestIndicator.ToString()
  << ", " << this->pipeWorkerToServerIndicatorData.ToString()
  << ", " << this->pipeWorkerToServerUserInput.ToString()
  << ", " << this->pipeWorkerToServerWorkerStatus.ToString()
  << ", " << this->PauseWorker.ToString()
  << ", " << this->PauseComputationReportReceived.ToString()
  << ", " << this->PauseIndicatorPipeInUse.ToString()
  ;
  out << ", user address: " << this->userAddress << ".";
  return out.str();
}

bool WebServer::CheckConsistency()
{ if (this->flagDeallocated)
    crash << "Use after free of WebServer." << crash;
  return true;
}

void WebServer::ReleaseEverything()
{ this->SSLfreeResources();
  ProgressReportWebServer::flagServerExists=false;
  for (int i=0; i<this->theWorkers.size; i++)
    this->theWorkers[i].Release();
  theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection=0;
  //theGlobalVariables.WebServerTimerPing=0;
  theGlobalVariables.IndicatorStringOutputFunction=0;
  theGlobalVariables.PauseUponUserRequest=0;
  this->activeWorker=-1;
  if (this->listeningSocketHTTP!=-1)
    close(this->listeningSocketHTTP);
  this->listeningSocketHTTP=-1;
  if (this->listeningSocketHttpSSL!=-1)
    close(this->listeningSocketHttpSSL);
  this->listeningSocketHttpSSL=-1;
}

WebServer::~WebServer()
{ this->flagDeallocated=true;
}

void WebServer::ReturnActiveIndicatorAlthoughComputationIsNotDone()
{ //theLog << logger::red << ("Got THUS far") << logger::endL;
//  theLog << "here am i";
  theWebServer.GetActiveWorker().OutputShowIndicatorOnTimeout();
//  stOutput << "What the hell";
//  stOutput.Flush();
}

void WebServer::FlushActiveWorker()
{ theWebServer.GetActiveWorker().SendAllBytes();
}

void WebServer::SendStringThroughActiveWorker(const std::string& theString)
{ theWebServer.GetActiveWorker().QueueStringForSending(theString, false);
}

void WebServer::PipeProgressReportToParentProcess(const std::string& theString)
{ theWebServer.GetActiveWorker().PipeProgressReportToParentProcess(theString);
}

WebServer::WebServer()
{ this->flagDeallocated=false;
  this->flagTryToKillOlderProcesses=true;
  this->flagPort8155=false;
  this->activeWorker=-1;
  this->timeLastExecutableModification=-1;
  this->listeningSocketHTTP=-1;
  this->listeningSocketHttpSSL=-1;
  this->highestSocketNumber=-1;
  this->flagSSLHandshakeSuccessful=false;
  this->flagReapingChildren=false;
  this->MaxNumWorkersPerIPAdress=8;
  this->MaxTotalUsedWorkers=20;
  this->NumFailedSelectsSoFar=0;
  this->NumSuccessfulSelectsSoFar=0;
  this->NumProcessesReaped=0;
  this->NumProcessAssassinated=0;
  this->NumConnectionsSoFar=0;
  this->NumWorkersNormallyExited=0;
  this->WebServerPingIntervalInSeconds=20;
  this->flagThisIsWorkerProcess=false;
}

WebWorker& WebServer::GetActiveWorker()
{ MacroRegisterFunctionWithName("WebServer::GetActiveWorker");
  void (*oldOutputFunction)(const std::string& stringToOutput) =stOutput.theOutputFunction;
  stOutput.theOutputFunction=0; //<- We are checking if the web server is in order.
  //Before that we prevent the crashing mechanism from trying to use (the eventually corrupt) web server
  //to report the error over the Internet.
  if (this->activeWorker<0 || this->activeWorker>=this->theWorkers.size)
    crash << "Active worker index is " << this->activeWorker << " however I have " << this->theWorkers.size
    << " workers. " << crash;
  stOutput.theOutputFunction=oldOutputFunction;//<-the web server is in order,
  //therefore we restore the ability to report crashes over the internet.
  return this->theWorkers[this->activeWorker];
}

void WebServer::SignalActiveWorkerDoneReleaseEverything()
{ MacroRegisterFunctionWithName("WebServer::SignalActiveWorkerDoneReleaseEverything");
  if (theWebServer.activeWorker==-1)
    return;
  theWebServer.GetActiveWorker().SignalIamDoneReleaseEverything();
  theWebServer.activeWorker=-1;
}

void WebServer::ReleaseActiveWorker()
{ MacroRegisterFunctionWithName("WebServer::ReleaseActiveWorker");
  if (this->activeWorker==-1)
    return;
  this->GetActiveWorker().Release();
  this->activeWorker=-1;
}

void WebServer::WorkerTimerPing(double pingTime)
{
  if (theWebServer.activeWorker==-1)
  { if (!theGlobalVariables.flagComputationFinishedAllOutputSentClosing)
      crash << "WebServer::WorkerTimerPing called when the computation is not entirely complete. " << crash;
    return;
  }
  std::stringstream outTimestream;
  outTimestream << "Worker: " << pingTime << " seconds passed. ";
  theWebServer.GetActiveWorker().pipeWorkerToServerTimerPing.WriteAfterEmptying(outTimestream.str());
}

void WebServer::ReleaseNonActiveWorkers()
{ MacroRegisterFunctionWithName("WebServer::ReleaseNonActiveWorkers");
  for (int i=0; i<this->theWorkers.size; i++)
    if (i!=this->activeWorker)
      this->theWorkers[i].ReleaseKeepInUseFlag();
}

void WebServer::ReleaseSocketsNonActiveWorkers()
{ MacroRegisterFunctionWithName("WebServer::ReleaseSocketsNonActiveWorkers");
  for (int i=0; i<this->theWorkers.size; i++)
    if (i!=this->activeWorker)
      this->Release(this->theWorkers[i].connectedSocketID);
}

bool WebServer::EmergencyRemoval_LastCreatedWorker()
{ this->GetActiveWorker().Release();
  this->activeWorker=-1;
  this->theWorkers.SetSize(this->theWorkers.size-1);
  return false;
}

bool WebServer::CreateNewActiveWorker()
{ MacroRegisterFunctionWithName("WebServer::CreateNewActiveWorker");
  if (this->activeWorker!=-1)
  { crash << "Calling CreateNewActiveWorker requires the active worker index to be -1." << crash;
    return false;
  }
  bool found=false;
  for (int i=0; i<this->theWorkers.size; i++)
    if (!this->theWorkers[i].flagInUse)
    { this->activeWorker=i;
      found=true;
      break;
    }
  if (this->activeWorker==-1)
  { this->activeWorker=this->theWorkers.size;
    this->theWorkers.SetSize(this->theWorkers.size+1);
  }
  this->GetActiveWorker().indexInParent=this->activeWorker;
  this->GetActiveWorker().parent=this;
  this->GetActiveWorker().timeOfLastPingServerSideOnly=theGlobalVariables.GetElapsedSeconds();
  this->GetActiveWorker().pingMessage="";
  if (found)
  { this->theWorkers[this->activeWorker].flagInUse=true;
    return true;
  }
  this->GetActiveWorker().Release();
  this->theWorkers[this->activeWorker].flagInUse=false; //<-until everything is initialized, we cannot be in use.
  if (!this->GetActiveWorker().PauseComputationReportReceived.CreateMe("server to worker computation report received"))
    return this->EmergencyRemoval_LastCreatedWorker();
  if (!this->GetActiveWorker().PauseWorker.CreateMe("server to worker pause"))
    return this->EmergencyRemoval_LastCreatedWorker();
  if (!this->GetActiveWorker().PauseIndicatorPipeInUse.CreateMe("server to worker indicator pipe in use"))
    return this->EmergencyRemoval_LastCreatedWorker();
  if (!this->GetActiveWorker().pipeServerToWorkerRequestIndicator.CreateMe("server to worker request indicator"))
    return this->EmergencyRemoval_LastCreatedWorker();
  if (!this->GetActiveWorker().pipeWorkerToServerTimerPing.CreateMe("worker to server timer ping"))
    return this->EmergencyRemoval_LastCreatedWorker();
  if (!this->GetActiveWorker().pipeWorkerToServerControls.CreateMe("worker to server controls"))
    return this->EmergencyRemoval_LastCreatedWorker();
  if (!this->GetActiveWorker().pipeWorkerToServerIndicatorData.CreateMe("worker to server indicator data"))
    return this->EmergencyRemoval_LastCreatedWorker();
  if (!this->GetActiveWorker().pipeWorkerToServerUserInput.CreateMe("worker to server user input"))
    return this->EmergencyRemoval_LastCreatedWorker();
  if (!this->GetActiveWorker().pipeWorkerToServerWorkerStatus.CreateMe("worker to server worker status"))
    return this->EmergencyRemoval_LastCreatedWorker();
  logPlumbing << logger::green
  << "Allocated new worker & plumbing data structures. Total worker data structures: " << this->theWorkers.size << ". "
  << logger::endL;
  this->theWorkers[this->activeWorker].flagInUse=true;
  return true;
}

std::string WebServer::ToStringLastErrorDescription()
{ std::stringstream out;
  out << logger::red << "Process " << this->ToStringActiveWorker() << ": " << strerror(errno) << ". ";
  return out.str();
}

std::string WebServer::ToStringActiveWorker()
{ if (theWebServer.activeWorker==-1)
    return "server";
  std::stringstream out;
  out << "worker " << theWebServer.activeWorker;
  return out.str();
}

std::string WebServer::ToStringStatusActive()
{ MacroRegisterFunctionWithName("WebServer::ToStringStatusActive");
  if (this->activeWorker==-1)
    return "server.";
  std::stringstream out;
  if (this->activeWorker!=this->GetActiveWorker().indexInParent)
    crash << "Bad index in parent!" << crash;
  out << this->GetActiveWorker().ToStringStatus();
  return out.str();
}

std::string WebServer::ToStringStatusPublicNoTop()
{ MacroRegisterFunctionWithName("WebServer::ToStringStatusPublicNoTop");
  std::stringstream out;
  out << "<b>The calculator web server server status.</b><hr>"
  << "<br>" << this->GetActiveWorker().timeOfLastPingServerSideOnly
  << " seconds = "
  << TimeWrapper::ToStringSecondsToDaysHoursSecondsString
  (this->GetActiveWorker().timeOfLastPingServerSideOnly, false)
  << " web server uptime. ";
  int approxNumPings=
  this->GetActiveWorker().timeOfLastPingServerSideOnly/this->WebServerPingIntervalInSeconds;
  if (approxNumPings<0)
    approxNumPings=0;
  int numConnectionsSoFarApprox=this->NumConnectionsSoFar-approxNumPings;
  if (numConnectionsSoFarApprox<0)
    numConnectionsSoFarApprox=0;
  out << "~" << numConnectionsSoFarApprox << " actual connections + ~"
  << approxNumPings << " self-test-pings (" << this->NumConnectionsSoFar << " connections total)"
  << " served since last restart. "
  << "This counts one connection per problem answer preview, page visit, progress report ping, etc. "
  ;

  out << "<br>" << this->theWorkers.size  << " = peak number of concurrent connections. "
  << " The number tends to be high as many browsers open more than one connection per page visit. <br>"
  << this->MaxTotalUsedWorkers << " global maximum of simultaneous non-closed connections allowed. "
  << "When the limit is exceeded, a random connection will be terminated. "
  << "<br>  " << this->MaxNumWorkersPerIPAdress
  << " maximum simultaneous connection per IP address. "
  << "When the limit is exceeded, a random connection from that IP address will be terminated. "
  ;
  return out.str();
}
std::string WebServer::ToStringStatusPublic()
{ MacroRegisterFunctionWithName("WebServer::ToStringStatusPublic");
  std::stringstream out;
  out << "<html><body>";
  out << this->ToStringStatusPublicNoTop();
  std::string topString= theGlobalVariables.CallSystemWithOutput("top -b -n 1 -s");
  //out << "<br>" << topString;
  std::string lineString, wordString;
  std::stringstream topStream(topString);
  out << "<hr><b>Server machine details.</b><br>";
  for (int i=0; i<5; i++)
  { std::getline(topStream, lineString);
    out << lineString << "<br>\n ";
  }
  out << "The source code of the web server and the entire calculator can be found here: "
  << "<a href=\"https://sourceforge.net/p/vectorpartition/code/HEAD/tree/\">calculator source code</a>. ";
  out << "</body></html>";
  return out.str();
}

std::string WebServer::ToStringStatusAll()
{ MacroRegisterFunctionWithName("WebServer::ToStringStatusFull");
  std::stringstream out;
  out << this->ToStringStatusPublicNoTop() << "<hr>";
  if (this->activeWorker==-1)
    out << "The process is functioning as a server.";
  else
  { out << "The process is functioning as a worker. The active worker is number " << this->activeWorker+1 << ". ";
    out << "<br>" << this->ToStringStatusActive();
  }
  int numInUse=0;
  for (int i=0; i<this->theWorkers.size; i++)
    if (this->theWorkers[i].flagInUse)
      numInUse++;
  for (int i=0; i<this->theWorkers.size; i++)
  { WebWorker& currentWorker=this->theWorkers[i];
    if (!currentWorker.flagInUse)
      continue;
    currentWorker.pipeWorkerToServerWorkerStatus.ReadWithoutEmptying();
    currentWorker.status.assign
    (currentWorker.pipeWorkerToServerWorkerStatus.lastRead.TheObjects,
     currentWorker.pipeWorkerToServerWorkerStatus.lastRead.size);
  }
  out << "<hr><hr>" << numInUse << " workers in use out of total " << this->theWorkers.size << " workers. ";
  out << "Connections: " << this->currentlyConnectedAddresses.ToString();
  for (int i=0; i<this->theWorkers.size; i++)
    out << "<hr>" << this->theWorkers[i].ToStringStatus();
  return out.str();
}

void WebServer::CheckExecutableVersionAndRestartIfNeeded(bool callReload)
{ struct stat theFileStat;
  if (stat(theGlobalVariables.PhysicalNameExecutableWithPath.c_str(), &theFileStat)!=0)
    return;
  if (this->timeLastExecutableModification!=-1)
    if (this->timeLastExecutableModification!=theFileStat.st_ctime)
    { stOutput << "<b>The server executable was updated, but the server has not been restarted yet. "
      << "Restarting in 0.5 seconds...</b>";
      if (callReload)
        stOutput << "<script language=\"javascript\">setTimeout(resubmit, 500); "
        << " function resubmit() { location.reload(true);}</script>";
      else
        stOutput << "<script language=\"javascript\">setTimeout(resubmit, 500); "
        << " function resubmit() { document.getElementById('formCalculator').submit();}</script>";
      if (this->activeWorker!=-1)
      { this->GetActiveWorker().SendAllBytes();
        this->ReleaseActiveWorker();
      }
      theLog << "Current process spawned from file with time stamp: "
      << this->timeLastExecutableModification
      << "; latest executable has different time stamp: " << theFileStat.st_ctime
      << ". " << logger::red << "RESTARTING." << logger::endL;
      this->Restart();
    }
}

void WebServer::Restart()
{ //theLog << "Killing all copies of the calculator and restarting..." << logger::endL;
  //theLog << "Time limit after restart: " << theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit << logger::endL;
  if (this->listeningSocketHTTP!=-1)
    this->Release(this->listeningSocketHTTP);
  if (this->listeningSocketHttpSSL!=-1)
    this->Release(this->listeningSocketHttpSSL);
//  char arg1[7]="server";
//  char arg2[7]="nokill";
//  char* args[2];
//  args[1]=arg1;
//  args[2]=arg2;
//  char** args=0 ;
//  execv("./calculator server nokill", args);
//char *exec_argv[] = { "./calculator", "server", "nokill"};
//sleep(1);
//execv("/proc/self/exe", exec_argv);
  std::stringstream theCommand;
  theLog << logger::red << " restarting with time limit "
  << theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit << logger::endL;
  int timeInteger=(int) theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit;
  theCommand << "killall " << theGlobalVariables.PhysicalNameExecutableNoPath + " \r\n./";
  theCommand << theGlobalVariables.PhysicalNameExecutableNoPath;
  if (this->flagPort8155)
    theCommand << " server8155 nokill " << timeInteger;
  else if (theGlobalVariables.flagSSLisAvailable)
    theCommand << " serverSSL nokill " << timeInteger;
  else
    theCommand << " server nokill " << timeInteger;
  theGlobalVariables.CallSystemNoOutput(theCommand.str()); //kill any other running copies of the calculator.
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
  if (listen(this->listeningSocketHTTP, WebServer::maxNumPendingConnections) == -1)
    crash << "Listen function failed on http port." << crash;
  if (theGlobalVariables.flagSSLisAvailable)
    if (listen(this->listeningSocketHttpSSL, WebServer::maxNumPendingConnections) == -1)
      crash << "Listen function failed on https port." << crash;
  this->highestSocketNumber=-1;
  if (this->listeningSocketHTTP!=-1)
  { this->theListeningSockets.AddOnTop(this->listeningSocketHTTP);
    this->highestSocketNumber=MathRoutines::Maximum(this->listeningSocketHTTP, this->highestSocketNumber);
  }
  if (this->listeningSocketHttpSSL!=-1)
  { this->theListeningSockets.AddOnTop(this->listeningSocketHttpSSL);
    this->highestSocketNumber=MathRoutines::Maximum(this->listeningSocketHttpSSL, this->highestSocketNumber);
  }
}

void WebServer::initDates()
{ this->timeLastExecutableModification=-1;
  struct stat theFileStat;
  if (stat(theGlobalVariables.PhysicalNameExecutableWithPath.c_str(), &theFileStat)!=0)
    return;
  this->timeLastExecutableModification=theFileStat.st_ctime;
}

void WebServer::ReleaseWorkerSideResources()
{ MacroRegisterFunctionWithName("WebServer::ReleaseWorkerSideResources");
  this->Release(this->GetActiveWorker().connectedSocketID);
  //<-release socket- communication is handled by the worker.
  this->activeWorker=-1; //<-The active worker is needed only in the child process.
}

void segfault_sigaction(int signal, siginfo_t *si, void *arg)
{ (void) signal; //avoid unused parameter warning, portable.
  (void) arg;
  crash << "Caught segfault at address: " << si->si_addr << crash;
  exit(0);
}

void fperror_sigaction(int signal)
{ (void) signal;
  crash << "Fatal arithmetic error. " << crash;
  exit(0);
}

void WebServer::TerminateChildSystemCall(int i)
{ this->theWorkers[i].flagInUse=false;
  this->currentlyConnectedAddresses.SubtractMonomial(this->theWorkers[i].userAddress, 1);
  kill(this->theWorkers[i].ProcessPID, SIGKILL);
}

void WebServer::RecycleChildrenIfPossible(const std::string& incomingUserAddress)
{ //Listen for children who have exited properly.
  //This might need to be rewritten: I wasn't able to make this work with any
  //mechanism other than pipes.
  MacroRegisterFunctionWithName("WebServer::RecycleChildrenIfPossible");
//  this->ReapChildren();
  MonomialWrapper<std::string, MathRoutines::hashString> incomingAddress(incomingUserAddress);
  int numInUse=0;
  for (int i=0; i<this->theWorkers.size; i++)
    if (this->theWorkers[i].flagInUse)
      numInUse++;
  bool purgeIncomingAddress=
  (this->currentlyConnectedAddresses.GetMonomialCoefficient(incomingAddress)>this->MaxNumWorkersPerIPAdress);

  for (int i=0; i<this->theWorkers.size; i++)
    if (this->theWorkers[i].flagInUse)
    { this->theWorkers[i].pipeWorkerToServerControls.ReadNoLocksUNSAFE_FOR_USE_BY_WEBSERVER_ONLY();
      if (this->theWorkers[i].pipeWorkerToServerControls.lastRead.size>0)
      { this->theWorkers[i].flagInUse=false;
        this->currentlyConnectedAddresses.SubtractMonomial(this->theWorkers[i].userAddress, 1);
        theLog << logger::green << "Worker " << i+1 << " done, marking for reuse. " << logger::endL;
        numInUse--;
        this->NumWorkersNormallyExited++;
//        waitpid(this->theWorkers[i].ProcessPID, 0, )
      } else
        theLog << logger::yellow << "Worker " << i+1 << " not done yet. " << logger::endL;
      this->theWorkers[i].pipeWorkerToServerUserInput.ReadNoLocksUNSAFE_FOR_USE_BY_WEBSERVER_ONLY();
      if (this->theWorkers[i].pipeWorkerToServerUserInput.lastRead.size>0)
        this->theWorkers[i].displayUserInput.assign
        (this->theWorkers[i].pipeWorkerToServerUserInput.lastRead.TheObjects,
          this->theWorkers[i].pipeWorkerToServerUserInput.lastRead.size);
      this->theWorkers[i].pipeWorkerToServerTimerPing.ReadNoLocksUNSAFE_FOR_USE_BY_WEBSERVER_ONLY();
      if (purgeIncomingAddress)
        if (this->theWorkers[i].userAddress==incomingAddress)
        { this->TerminateChildSystemCall(i);
          std::stringstream errorStream;
          errorStream
          << "Terminating child " << i+1 << " with PID "
          << this->theWorkers[i].ProcessPID
          << ": purging all connections from " << incomingUserAddress
          << ": address opened more than " << this->MaxNumWorkersPerIPAdress << " simultaneous connections. ";
          this->theWorkers[i].pingMessage=errorStream.str();
          logProcessKills << logger::red  << errorStream.str() << logger::endL;
          numInUse--;
          this->NumProcessAssassinated++;
          continue;
        }
      if (this->theWorkers[i].pipeWorkerToServerTimerPing.lastRead.size>0)
      { this->theWorkers[i].pingMessage.assign
        (this->theWorkers[i].pipeWorkerToServerTimerPing.lastRead.TheObjects,
         this->theWorkers[i].pipeWorkerToServerTimerPing.lastRead.size);
        this->theWorkers[i].timeOfLastPingServerSideOnly=theGlobalVariables.GetElapsedSeconds();
      } else if (this->theWorkers[i].PauseWorker.CheckPauseIsRequested_UNSAFE_SERVER_USE_ONLY())
      { this->theWorkers[i].pingMessage="worker paused, no pings.";
        this->theWorkers[i].timeOfLastPingServerSideOnly=theGlobalVariables.GetElapsedSeconds();
      } else if (theGlobalVariables.MaxTimeNoPingBeforeChildIsPresumedDead>0 &&
                 theGlobalVariables.GetElapsedSeconds()-this->theWorkers[i].timeOfLastPingServerSideOnly>
                 theGlobalVariables.MaxTimeNoPingBeforeChildIsPresumedDead &&
                 this->theWorkers[i].flagInUse)
      { this->TerminateChildSystemCall(i);
        std::stringstream pingTimeoutStream;
        pingTimeoutStream << theGlobalVariables.GetElapsedSeconds()-this->theWorkers[i].timeOfLastPingServerSideOnly
        << " seconds passed since worker " << i+1
        << " last pinged the server; killing pid: "
        << this->theWorkers[i].ProcessPID << ". ";
        logProcessKills << logger::red << pingTimeoutStream.str() << logger::endL;
        this->theWorkers[i].pingMessage="<span style=\"color:red\"><b>" + pingTimeoutStream.str()+"</b></span>";
        numInUse--;
        this->NumProcessAssassinated++;
      }
    }
  if (numInUse<=this->MaxTotalUsedWorkers)
    return;
  for (int i=0; i<this->theWorkers.size && numInUse>1; i++)
  { if (this->theWorkers[i].flagInUse)
    { this->TerminateChildSystemCall(i);
      std::stringstream errorStream;
      errorStream
      << "Terminating child " << i+1 << " with PID "
      << this->theWorkers[i].ProcessPID
      << ": too many workers in use. ";
      this->theWorkers[i].pingMessage=errorStream.str();
      logProcessKills << logger::red  << errorStream.str() << logger::endL;
      numInUse--;
      this->NumProcessAssassinated++;
    }
  }
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
  addrinfo *servinfo=0;
  addrinfo *p=0;
  int yes=1;
  int rv;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP
  // loop through all the results and bind to the first we can
  List<std::string>* thePorts=&this->PortsITryHttp;
  int* theListeningSocket=0;
  theListeningSocket=& this->listeningSocketHTTP;
  for (int j=0; j<2; j++, thePorts=& this->PortsITryHttpSSL, theListeningSocket=& this->listeningSocketHttpSSL)
    for (int i=0; i<(*thePorts).size; i++)
    { if ((rv = getaddrinfo(NULL, (*thePorts)[i].c_str(), &hints, &servinfo)) != 0)
      { theLog << "getaddrinfo: " << gai_strerror(rv) << logger::endL;
        return false;
      }
      for(p = servinfo; p != NULL; p = p->ai_next)
      { *theListeningSocket= socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (*theListeningSocket == -1)
        { theLog << "Error: socket failed.\n";
          continue;
        }
        if (setsockopt(*theListeningSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
          crash << "Error: setsockopt failed, error: \n" << strerror(errno) << crash;
        if (bind(*theListeningSocket, p->ai_addr, p->ai_addrlen) == -1)
        { close(*theListeningSocket);
          *theListeningSocket=-1;
          theLog << "Error: bind failed at port: " << (*thePorts)[i] << ". Error: "
          << this->ToStringLastErrorDescription() << logger::endL;
          continue;
        }
        fcntl(*theListeningSocket, F_SETFL, O_NONBLOCK);
        break;
      }
      if (p!=NULL)
      { theLog << logger::yellow << "Successfully bound to port " << (*thePorts)[i] << logger::endL;
        if (j==0)
          this->httpPort=(*thePorts)[i];
        else
          this->httpSSLPort=(*thePorts)[i];
        break;
      }
      freeaddrinfo(servinfo); // all done with this structure
    }
  if (this->listeningSocketHTTP == -1)
    crash << "Failed to bind to any of the ports " << this->PortsITryHttp.ToStringCommaDelimited() << "\n" << crash;
  if (theGlobalVariables.flagSSLisAvailable && this->listeningSocketHttpSSL==-1)
    crash << "Failed to bind to any of the ports " << this->PortsITryHttpSSL.ToStringCommaDelimited() << "\n" << crash;
  return true;
}

void WebServer::initPrepareSignals()
{ MacroRegisterFunctionWithName("WebServer::initPrepareSignals");
  struct sigaction sa;
  sa.sa_sigaction = &segfault_sigaction;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_SIGINFO;
  if (sigaction(SIGSEGV, &sa, NULL) == -1)
  { theLog << "sigaction returned -1" << logger::endL;
    crash << "Was not able to register SIGSEGV handler. Crashing to let you know. " << crash;
  }
  //catch floating point exceptions
  sa.sa_sigaction=0;
  sa.sa_handler= &fperror_sigaction;
  sigemptyset(&sa.sa_mask);
//  sa.sa_flags = SA_SIGINFO;
  if (sigaction(SIGFPE, &sa, NULL) == -1)
  { theLog << "sigaction returned -1" << logger::endL;
    crash << "Was not able to register SIGFPE handler. Crashing to let you know. " << crash;
  }
  sa.sa_handler=&WebServer::Signal_SIGINT_handler;
  if (sigaction(SIGINT, &sa, NULL) == -1)
    theLog << "sigaction returned -1" << logger::endL;
  sa.sa_handler =  &WebServer::Signal_SIGCHLD_handler; // reap all dead processes
//  sa.sa_handler =  0; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_NOCLDWAIT;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
  { theLog << "sigaction returned -1" << logger::endL;
    crash << "sigaction returned -1" << crash;
  }
//  sigemptyset(&sa.sa_mask);
//  sa.sa_flags = SA_RESTART;
//  if (sigaction(SIGCHLD, &sa, NULL) == -1)
//    theLog << "sigaction returned -1" << logger::endL;
}

extern void MonitorWebServer();

int WebServer::Run()
{ MacroRegisterFunctionWithName("WebServer::Run");
  theGlobalVariables.RelativePhysicalNameCrashLog="crash_WebServerRun.html";
  theParser.init();
  if (true)
  { int pidServer=fork();
    if (pidServer<0)
      crash << "Failed to create server process. " << crash;
    if (pidServer!=0)
    { MonitorWebServer();//<-this attempts to connect to the server over the internet and restarts if it can't.
      return 0;
    }
  }
  CGI::LoadStrings();
  theParser.flagShowCalculatorExamples=false;
  if (!this->initPrepareWebServerALL())
    return 1;
  theLog << logger::purple <<  "server: waiting for connections...\r\n" << logger::endL;

  sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size = sizeof their_addr;
  char userAddressBuffer[INET6_ADDRSTRLEN];
  this->initSSL();
  fd_set FDListenSockets;
  this->NumSuccessfulSelectsSoFar=0;
  this->NumFailedSelectsSoFar=0;
  long long previousReportedNumberOfSelects=0;
  int previousServerStatReport=0;
  while(true)
  { // main accept() loop
//    theLog << logger::red << "select returned!" << logger::endL;
    FD_ZERO(&FDListenSockets);
    for (int i=0; i<this->theListeningSockets.size; i++)
      FD_SET(this->theListeningSockets[i], &FDListenSockets);

    while (select(this->highestSocketNumber+1, & FDListenSockets, 0, 0, 0)==-1)
    { if (this->flagReapingChildren)
        logIO << logger::yellow << "Select interrupted while reaping children. "
        << logger::endL;
      else
        logIO << logger::red << "Select failed: possibly due to reaping children. Error message: "
        << strerror(errno) << logger::endL;
      this->NumFailedSelectsSoFar++;
    }
    this->NumSuccessfulSelectsSoFar++;
    if ((this->NumSuccessfulSelectsSoFar+this->NumFailedSelectsSoFar)-previousReportedNumberOfSelects>100)
    { logSocketAccept << logger::blue << this->NumSuccessfulSelectsSoFar << " successful and " << this->NumFailedSelectsSoFar << "="
      << this->NumSuccessfulSelectsSoFar + this->NumFailedSelectsSoFar << " total selects. " << logger::endL;
      previousReportedNumberOfSelects=this->NumSuccessfulSelectsSoFar+this->NumFailedSelectsSoFar;
    }
    if (this->NumProcessAssassinated+this->NumProcessesReaped+this->NumWorkersNormallyExited+this->NumConnectionsSoFar-previousServerStatReport>99)
    { previousServerStatReport=this->NumProcessAssassinated+this->NumProcessesReaped+this->NumWorkersNormallyExited+this->NumConnectionsSoFar;
      logProcessStats << "# kill commands: " << this->NumProcessAssassinated
      << " #processes reaped: " << this->NumProcessesReaped
      << " #normally reclaimed workers: " << this->NumWorkersNormallyExited
      << " #connections so far: " << this->NumConnectionsSoFar << logger::endL;
    }
    int newConnectedSocket =-1;
    theGlobalVariables.flagUsingSSLinCurrentConnection=false;
    bool found=false;
    for (int i=theListeningSockets.size-1; i>=0; i--)
      if (FD_ISSET(this->theListeningSockets[i], &FDListenSockets))
      { //if (this->theListeningSockets[i]==this->listeningSocketHTTP)
        newConnectedSocket=accept(this->theListeningSockets[i], (struct sockaddr *)&their_addr, &sin_size);
        if (newConnectedSocket>=0)
        { logIO << logger::green << "Connected via listening socket " << this->theListeningSockets[i]
          << " on socket: " << newConnectedSocket;
          if (this->theListeningSockets[i]==this->listeningSocketHttpSSL)
          { theGlobalVariables.flagUsingSSLinCurrentConnection=true;
            logIO << logger::purple << " (SSL encrypted)." << logger::endL;
          } else
            logIO << logger::yellow << " (non-encrypted)." << logger::endL;
          break;
        } else
        { logSocketAccept << logger::red << "This is not supposed to happen: accept failed. Error: "
          << this->ToStringLastErrorDescription() << logger::endL;
          found=true;
        }
      }
    if (newConnectedSocket<0 && !found)
      logSocketAccept << logger::red << "This is not supposed to to happen: select succeeded "
      << "but I found no set socket. "
      << logger::endL;
    if (newConnectedSocket <0)
      continue;
//    theLog << logger::purple << "NewconnectedSocket: " << newConnectedSocket << ", listeningSocket: "
//    << theListeningSocket << logger::endL;
    inet_ntop
    (their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),
     userAddressBuffer, sizeof userAddressBuffer);
    this->RecycleChildrenIfPossible(userAddressBuffer);
    if (!this->CreateNewActiveWorker())
    { logPlumbing << logger::purple << "Failed to create an active worker. System error string: "
      << strerror(errno) << logger::endL;
      logIO << logger::red << "Failed to create active worker: closing connection. " << logger::endL;
      close (newConnectedSocket);
      continue;
    }
    this->GetActiveWorker().connectedSocketID=newConnectedSocket;
    this->GetActiveWorker().connectedSocketIDLastValueBeforeRelease=newConnectedSocket;
    this->NumConnectionsSoFar++;
    this->GetActiveWorker().connectionID=this->NumConnectionsSoFar;
    this->GetActiveWorker().userAddress.theObject=userAddressBuffer;
    this->currentlyConnectedAddresses.AddMonomial(this->GetActiveWorker().userAddress, 1 );
//    theLog << this->ToStringStatus();
    this->GetActiveWorker().ProcessPID=fork(); //creates an almost identical copy of this process.
    //The original process is the parent, the almost identical copy is the child.
    //theLog << "\r\nChildPID: " << this->childPID;
    if (this->GetActiveWorker().ProcessPID==-1)
      logProcessKills << logger::red << "FAILED to spawn a child process. " << logger::endL;
    if (this->GetActiveWorker().ProcessPID==0)
    { // this is the child (worker) process
      this->flagThisIsWorkerProcess=true;
      //theGlobalVariables.WebServerTimerPing=this->WorkerTimerPing;
      theGlobalVariables.flagAllowUseOfThreadsAndMutexes=true;
      crash.CleanUpFunction=WebServer::SignalActiveWorkerDoneReleaseEverything;
      InitializeTimer();
      CreateTimerThread();
      this->SSLServerSideHandShake();
      if (theGlobalVariables.flagSSLisAvailable &&
          theGlobalVariables.flagUsingSSLinCurrentConnection &&
          !this->flagSSLHandshakeSuccessful)
      { theGlobalVariables.flagUsingSSLinCurrentConnection=false;
        this->SignalActiveWorkerDoneReleaseEverything();
        this->ReleaseEverything();
        logOpenSSL << logger::red << "ssl fail #: " << this->NumConnectionsSoFar << logger::endL;
        return -1;
      }
      if (theGlobalVariables.flagSSLisAvailable && theGlobalVariables.flagUsingSSLinCurrentConnection)
        logOpenSSL << logger::green << "ssl success #: " << this->NumConnectionsSoFar << ". " << logger::endL;
      //  this->Release(theListeningSocket);//worker has no access to socket listener
      /////////////////////////////////////////////////////////////////////////
      // stOutput.theOutputFunction=WebServer::SendStringThroughActiveWorker;
      // stOutput.flushOutputFunction=this->FlushActiveWorker;
      // theLog << this->ToStringStatusActive() << logger::endL;
      this->GetActiveWorker().CheckConsistency();
      // std::cout << "Got thus far 7" << std::endl;

      //if (!this->flagUseSSL)
      //{
      if (!this->GetActiveWorker().ReceiveAll())
      { stOutput << "HTTP/1.0 400 Bad request\r\nContent-type: text/html\r\n\r\n"
        << "<html><body><b>HTTP error 400 (bad request). </b> There was an error with the request. "
        << "One possibility is that the input was too large. "
        << "<br>The error message returned was:<br>"
        << this->GetActiveWorker().error
        << " <hr><hr>The message (part) that was received is: "
        << this->GetActiveWorker().ToStringMessageFullUnsafe()
        << "</body></html>";
        stOutput.Flush();
        this->SignalActiveWorkerDoneReleaseEverything();
        this->ReleaseEverything();
//        this->SSLfreeEverythingShutdownSSL();
        return -1;
      }
      this->GetActiveWorker().SendDisplayUserInputToServer();
//      std::cout << "Serving step connection: " << this->NumConnectionsSoFar << std::endl;
      int result= this->GetActiveWorker().ServeClient();
//      std::cout << "Serving SECOND step connection: " << this->NumConnectionsSoFar << std::endl;
      this->ReleaseEverything();
//      this->SSLfreeEverythingShutdownSSL();
      return result;
    }
    this->ReleaseWorkerSideResources();
  }
  this->ReleaseEverything();
  this->SSLfreeEverythingShutdownSSL();
  return 0;
}

void WebServer::Release(int& theDescriptor)
{ PauseController::Release(theDescriptor);
}

int WebServer::main_problem_interpreter()
{ MacroRegisterFunctionWithName("main_problem_interpreter");
  theGlobalVariables.IndicatorStringOutputFunction=0;
  theGlobalVariables.flagAllowUseOfThreadsAndMutexes=true;
  theGlobalVariables.flagComputationStarted=true;
//  stOutput << "<hr>First line<hr>";
  theGlobalVariables.DisplayNameExecutableWithPath="/cgi-bin/interpret.py";
//  stOutput << "<hr>DEBUG: physical path project base: " << theGlobalVariables.PhysicalPathProjectBase << "<br>";
  InitializeTimer();
//  CreateTimerThread();

  theWebServer.initPrepareSignals();
//  std::cout << "DEBUG: Running offline interpreter. \n";
  theParser.init();
  if (theGlobalVariables.programArguments.size<3)
  { stOutput << "This is either programming error or very unexpected user behavior: "
    << "calculator called with input: " << theGlobalVariables.programArguments;
    return 0;
  }
  theParser.inputStringRawestOfTheRaw=theGlobalVariables.programArguments[2];
  WebWorker theWorker;
  theWorker.ProcessCalculatorNoLoginInterpreterMode();
  theGlobalVariables.flagComputationCompletE=true;
  return 0;
}

int WebServer::main_command_input()
{ MacroRegisterFunctionWithName("main_command_input");
  theGlobalVariables.IndicatorStringOutputFunction=CGI::MakeStdCoutReport;
  //  stOutput << "\n\n\n" << theParser.DisplayPathServerBase << "\n\n";
  //  return 0;
//std::cout << "Running cmd line. \n";
  theParser.init();
  theParser.inputStringRawestOfTheRaw =theGlobalVariables.programArguments[0];
  theParser.flagUseHtml=false;
  theParser.Evaluate(theParser.inputStringRawestOfTheRaw);
  std::fstream outputFile;
  FileOperations::OpenFileCreateIfNotPresentVirtual
  (outputFile, "output/outputFileCommandLine.html", false, true, false);
  stOutput << theParser.outputString;
  outputFile << theParser.outputString;
  stOutput << "\nTotal running time: " << GetElapsedTimeInSeconds() << " seconds. \nOutput written in file ./outputFileCommandLine.html\n";
  return 0;
}

int WebServer::main_apache_client()
{ MacroRegisterFunctionWithName("main_apache_client");
  stOutput << "Content-Type: text/html\n\n";
  theGlobalVariables.IndicatorStringOutputFunction=CGI::MakeReportIndicatorFile;
  std::cin >> theParser.inputStringRawestOfTheRaw;

	if (theParser.inputStringRawestOfTheRaw=="")
	{ theParser.inputStringRawestOfTheRaw=getenv("QUERY_STRING");
    std::string& IPAdressCaller=theGlobalVariables.IPAdressCaller;
    IPAdressCaller=getenv("REMOTE_ADDR");
    for (int i=0; i<MathRoutines::Minimum((int)IPAdressCaller.size(), SomeRandomPrimesSize); i++)
      IPAdressCaller[i]='A'+(IPAdressCaller[i]*SomeRandomPrimes[i])%26;
	}
	theParser.javaScriptDisplayingIndicator=WebWorker::GetJavaScriptIndicatorFromHD();
	theWebServer.CreateNewActiveWorker();
  return theWebServer.GetActiveWorker().ProcessCalculator();
}

extern int mainTest(List<std::string>& remainingArgs);

void WebServer::AnalyzeMainArguments(int argC, char **argv)
{ MacroRegisterFunctionWithName("WebServer::AnalyzeMainArguments");
  //std::cout << "DEBUG: Here I am. ";
  if (argC<0)
    argC=0;
  theGlobalVariables.programArguments.SetSize(argC);
//  std::cout << "Program arguments: \n";
  for (int i=0; i<argC; i++)
    theGlobalVariables.programArguments[i]=argv[i];
  //std::cout << "\nDEBUG: Program arguments: <br>"
  //<< theGlobalVariables.programArguments.ToStringCommaDelimited() << "\n";

  theGlobalVariables.flagUsingBuiltInWebServer=false;
  theGlobalVariables.flagRunningCommandLine=false;
  theGlobalVariables.flagRunningConsoleTest=false;
  theGlobalVariables.flagUsingApacheWebServer=false;
  if (argC>=1)
  { if (theGlobalVariables.programArguments[0]!="test")
      theGlobalVariables.initDefaultFolderAndFileNames(theGlobalVariables.programArguments[0]);
    else
    { theGlobalVariables.flagRunningConsoleTest=true;
      theGlobalVariables.programArguments.PopIndexShiftDown(0);
      return;
    }
  }
  if (argC<2)
  { theGlobalVariables.flagUsingApacheWebServer=true;
    return;
  }
  std::string& secondArgument=theGlobalVariables.programArguments[1];
  if (secondArgument=="test")
  { theGlobalVariables.programArguments.PopIndexShiftDown(0);
    theGlobalVariables.programArguments.PopIndexShiftDown(0);
    theGlobalVariables.flagRunningConsoleTest=true;
    return;
  }
  if (secondArgument=="interpretProblem")
  { theGlobalVariables.flagRunningAsProblemInterpreter=true;
    theGlobalVariables.flagUsingBuiltInWebServer=false;
    theGlobalVariables.flagRunningCommandLine=false;
    return;
  }
  theGlobalVariables.flagUsingBuiltInWebServer=
  (secondArgument=="server" || secondArgument=="server8155" || secondArgument=="serverSSL");
  theGlobalVariables.flagRunningCommandLine=
  !theGlobalVariables.flagUsingBuiltInWebServer &&
  !theGlobalVariables.flagRunningAsProblemInterpreter;
  //std::cout << "\n<br>DEBUG: got to here pt 1. <br>";
  if (theGlobalVariables.flagRunningCommandLine)
  { theGlobalVariables.programArguments.RemoveIndexShiftDown(0);
    return;
  }
  if (secondArgument=="server8155")
    theWebServer.flagPort8155=true;
  if (secondArgument=="serverSSL" || secondArgument=="server8155")
  {
#ifdef MACRO_use_open_ssl
    theGlobalVariables.flagSSLisAvailable=true;
#endif // MACRO_use_open_ssl
  }
  if (argC<3)
    return;
  std::string& thirdArgument=theGlobalVariables.programArguments[2];
  theWebServer.flagTryToKillOlderProcesses=!(thirdArgument=="nokill");
  std::string timeLimitString="100";
  if (thirdArgument!="nokill")
    timeLimitString=thirdArgument;
  else if (argC>3)
    timeLimitString=theGlobalVariables.programArguments[3];
//  std::cout << "<br>timelimitstring: " << timeLimitString;
  //std::cout << "\nDEBUG: got to here pt 2. ";
  Rational timeLimit;
  timeLimit.AssignString(timeLimitString);
  int timeLimitInt=0;
  if (timeLimit.IsIntegerFittingInInt(&timeLimitInt))
    theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=timeLimitInt;
}

void WebServer::InitializeGlobalVariables()
{ theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=5000000;
  theGlobalVariables.MaxComputationTimeBeforeWeTakeAction=5;
  theGlobalVariables.flagReportEverything=true;
  ParallelComputing::cgiLimitRAMuseNumPointersInList=4000000000;
}

int WebServer::main(int argc, char **argv)
{ theGlobalVariables.InitThreadsExecutableStart();
  //for (int i=0; i<argc; i++)
  //  std::cout << "argument " << i << ": " << argv[i] << "\n";
  MacroRegisterFunctionWithName("main");
  try {
  InitializeGlobalObjects();
  theWebServer.InitializeGlobalVariables();
//  std::cout << "display name calculator: "
//  << theGlobalVariables.DisplayNameExecutableWithPath << std::endl;
  theWebServer.AnalyzeMainArguments(argc, argv);
  //theLog << logger::red << " At ze moment theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit= "
  //<< theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit << logger::endL;

  if (theGlobalVariables.flagRunningConsoleTest)
    return mainTest(theGlobalVariables.programArguments);
  if (theGlobalVariables.flagUsingApacheWebServer)
    return WebServer::main_apache_client();
  if (theGlobalVariables.flagUsingBuiltInWebServer)
    return theWebServer.Run();
  if (theGlobalVariables.flagRunningAsProblemInterpreter)
    return theWebServer.main_problem_interpreter();
  if (theGlobalVariables.flagRunningCommandLine)
    return WebServer::main_command_input();
  }
  catch (...)
  { crash << "Exception caught: something very wrong has happened. " << crash;
  }
  crash << "This point of code is not supposed to be reachable. " << crash;
  return -1;
}
