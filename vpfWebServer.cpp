//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServer.h"
#include "vpfHeader3Calculator0_Interface.h"
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
const std::string fileCertificate= "../cert.pem";
const std::string fileKey= "../key.pem";

void WebServer::initSSL()
{ MacroRegisterFunctionWithName("WebServer::initSSL");
  if (!theGlobalVariables.flagSSLisAvailable)
  { theLog << logger::red << "SSL is NOT available." << logger::endL;
    return;
  }
#ifdef MACRO_use_open_ssl
  if (!FileOperations::FileExistsUnsecure(fileCertificate) || !FileOperations::FileExistsUnsecure(fileKey))
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
  if (!SSL_CTX_set_cipher_list(theSSLdata.ctx,
  "ALL:!ECDHE-ECDSA-AES256-SHA:!ECDHE-ECDSA-AES128-SHA:!ECDHE-ECDSA-RC4-SHA:!ECDHE-ECDSA-DES-CBC3-SHA"
  ))
  { ERR_print_errors_fp(stderr);
    crash << "openssl error: failed to set cipher list." << crash;
  } else
    theLog << logger::orange << "Restricted ciphers to workaround Safari's bugs. " << logger::endL;
  if (SSL_CTX_use_certificate_file(theSSLdata.ctx, fileCertificate.c_str(), SSL_FILETYPE_PEM) <= 0)
  { ERR_print_errors_fp(stderr);
    exit(3);
  }
  if (SSL_CTX_use_PrivateKey_file(theSSLdata.ctx, fileKey.c_str(), SSL_FILETYPE_PEM) <= 0)
  { ERR_print_errors_fp(stderr);
    exit(4);
  }
  if (!SSL_CTX_check_private_key(theSSLdata.ctx))
  { fprintf(stderr,"Private key does not match the certificate public key\n");
    exit(5);
  }
////////Safari web browser: no further user of foul language necessary.
  SSL_CTX_set_options(theSSLdata.ctx, SSL_OP_SAFARI_ECDHE_ECDSA_BUG);
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
{
  if (!theGlobalVariables.flagSSLisAvailable)
    return;
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection)
    return;
  logIO << "Initiating SSL handshake. " << logger::endL;
#ifdef MACRO_use_open_ssl
  MacroRegisterFunctionWithName("WebServer::SSLServerSideHandShake");
//  theLog << "Got to here 1" << logger::endL;
  theSSLdata.ssl = SSL_new(theSSLdata.ctx);
//  theLog << "Got to here 1.05" << logger::endL;
  if(theSSLdata.ssl==0)
  { theLog << "Failed to allocate ssl" << logger::endL;
    crash << "Failed to allocate ssl: not supposed to happen" << crash;
  }
//  theLog << "Got to here 1.1" << logger::endL;
  SSL_set_fd (theSSLdata.ssl, this->GetActiveWorker().connectedSocketID);
//  theLog << "Got to here 1.2" << logger::endL;
  theSSLdata.errorCode = SSL_accept (theSSLdata.ssl);
//  theLog << "Got to here 1.3" << logger::endL;
//    CHK_SSL(err);
//  theLog << "Got to here 2" << logger::endL;
  /* Get the cipher - opt */
  /* Get client's certificate (note: beware of dynamic allocation) - opt */
  theSSLdata.client_cert = SSL_get_peer_certificate (theSSLdata.ssl);
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

  theLog << "Content length computed to be: " << this->ContentLength << logger::endL;
  if (this->requestType==WebWorker::requestTypes::requestPostCalculator)
    this->displayUserInput="POST " + this->mainArgumentRAW;
  else
    this->displayUserInput="GET " + this->mainAddresSRAW;
  if (this->ContentLength<=0)
  { logIO << " exiting successfully" << logger::endL;
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
  this->remainingBytesToSend=(std::string) "HTTP/1.1 100 Continue\r\n";
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
      theLog << this->error << logger::endL;
      this->displayUserInput=this->error;
      return false;
    }
    logIO << logger::blue << "about to read ..." << logger::endL;
    numBytesInBuffer= SSL_read(theSSLdata.ssl, &buffer, bufferSize-1);
    if (numBytesInBuffer==0)
    { this->error= "While trying to fetch message-body, received 0 bytes. " +
      this->parent->ToStringLastErrorDescription();
      this->displayUserInput=this->error;
      return false;
    }
    if (numBytesInBuffer<0)
    { this->error= "Error fetching message body: " + this->parent->ToStringLastErrorDescription();
      ERR_print_errors_fp(stderr);
      theLog << this->error << logger::endL;
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
const int WebServer::maxNumPendingConnections=10;

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
{ theLog << "Signal interrupt handler called with input: " << s;
//  << ". Waiting for children to exit... " << logger::endL;
  theWebServer.ReleaseActiveWorker();
  theWebServer.ReleaseNonActiveWorkers();
  while(waitpid(-1, NULL, WNOHANG | WEXITED) > 0)
  { }
  theLog << "All children have exited. " << logger::endL;
  exit(0);
}

void WebServer::Signal_SIGCHLD_handler(int s)
{ theLog << "Received SIGCHLD signal." << logger::endL;
//  theWebServer.ReapChildren();
}

void WebServer::ReapChildren()
{ MacroRegisterFunctionWithName("WebServer::ReapChildren");
  int waitResult=0;
  do
  { waitResult= waitpid(-1, NULL, WNOHANG| WEXITED);
//    theLog << "waitresult is: " << waitResult << logger::endL;
    if (waitResult>0)
      for (int i=0; i<this->theWorkers.size; i++)
        if (this->theWorkers[i].ProcessPID==waitResult)
        { this->theWorkers[i].pipeWorkerToServerControls.WriteAfterEmptying("close");
          theLog << logger::green << "Child with pid " << waitResult << " successfully reaped. " << logger::endL;
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
{ if (theGlobalVariables.flagUsingSSLinCurrentConnection)
    return "Message cannot be viewed when using SSL";
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
  out << lineBreak << "Calculator address: " << theGlobalVariables.DisplayNameCalculatorWithPath;
  out << lineBreak << "Physical address project base: " << theGlobalVariables.PhysicalPathProjectBase;
  out << lineBreak << "Physical address server base: " << theGlobalVariables.PhysicalPathServerBasE;
  out << lineBreak << "Physical address output folder: " << theGlobalVariables.PhysicalPathOutputFolder;
  return out.str();
}

std::string WebWorker::ToStringMessageFullUnsafe()const
{ if (theGlobalVariables.flagUsingSSLinCurrentConnection)
    return "Message cannot be viewed when using SSL";
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
{ if (theGlobalVariables.flagUsingSSLinCurrentConnection)
    return "Message cannot be viewed when using SSL";
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
  if (theGlobalVariables.GetWebInput("debugFlag")!="true")
    return "";
  std::stringstream out;
  out << "<hr>";
  out << "Default user: " << theGlobalVariables.userDefault;
  if (theGlobalVariables.flagLoggedIn)
    out << "<br>Logged in. ";
  if (theGlobalVariables.UserDefaultHasAdminRights())
    out << "<br><b>User has admin rights</b>";
  out << "<hr>";
  for (int i=0; i<theGlobalVariables.webFormArguments.size; i++)
  { out << theGlobalVariables.webFormArgumentNames[i] << ": " << theGlobalVariables.webFormArguments[i];
    if (i!=theGlobalVariables.webFormArgumentNames.size-1)
      out << "<br>";
  }
  out << "<hr>";
  return out.str();
}

bool WebWorker::ProcessRawArguments
(const std::string& urlEncodedInputString, std::stringstream& argumentProcessingFailureComments, int recursionDepth)
{ MacroRegisterFunctionWithName("WebServer::ProcessRawArguments");
  if (recursionDepth>1)
  { argumentProcessingFailureComments << "Error: input string encoded too many times";
    return false;
  }
  HashedList<std::string, MathRoutines::hashString>& inputStringNames=theGlobalVariables.webFormArgumentNames;
  List<std::string>& inputStrings=theGlobalVariables.webFormArguments;
  if (!CGI::ChopCGIInputStringToMultipleStrings
      (urlEncodedInputString, inputStrings, inputStringNames, argumentProcessingFailureComments))
    return false;
  if (inputStringNames.Contains("doubleURLencodedInput"))
  { std::string newInput=theGlobalVariables.GetWebInput("doubleURLencodedInput");
    inputStringNames.Clear();
    inputStrings.SetSize(0);
    return this->ProcessRawArguments(newInput, argumentProcessingFailureComments, recursionDepth+1);
  }
  theGlobalVariables.userCalculatorRequestType=theGlobalVariables.GetWebInput("request");
//  stOutput << "userCalculatorRequest type is: " << theGlobalVariables.userCalculatorRequestType;
  std::string password;
  std::string desiredUser=theGlobalVariables.GetWebInput("username");
  if (desiredUser=="")
    desiredUser=theGlobalVariables.GetWebInput("usernameHidden");
  if (desiredUser!="")
    CGI::URLStringToNormal(desiredUser, desiredUser);
  this->authenticationToken=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("authenticationToken"));
  if (this->authenticationToken!="")
    this->flagAuthenticationTokenWasSubmitted=true;
  this->flagPasswordWasSubmitted=(theGlobalVariables.GetWebInput("password")!="");
  if (this->flagPasswordWasSubmitted)
  { password=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("password"));
    inputStrings[inputStringNames.GetIndex("password")]="********************************************";
  }
  if (inputStringNames.Contains("textInput") &&! inputStringNames.Contains("mainInput"))
  { argumentProcessingFailureComments << "Received calculator link in an old format, interpreting 'textInput' as 'mainInput'";
    inputStringNames.SetObjectAtIndex(inputStringNames.GetIndex("textInput"), "mainInput");
  }
  if (desiredUser!="" && theGlobalVariables.flagUsingSSLinCurrentConnection)
  { bool changingPass=theGlobalVariables.userCalculatorRequestType=="changePassword" ||
    theGlobalVariables.userCalculatorRequestType=="activateAccount";
    if (changingPass)
      this->authenticationToken="";
//    argumentProcessingFailureComments << "Logging in as: " << desiredUser << "<br>";
    theGlobalVariables.flagLoggedIn=DatabaseRoutinesGlobalFunctions::LoginViaDatabase
    (desiredUser, password, this->authenticationToken, theGlobalVariables.userRole, &argumentProcessingFailureComments);
    if (theGlobalVariables.flagLoggedIn)
    { theGlobalVariables.userDefault=desiredUser;
      theGlobalVariables.SetWebInput("authenticationToken", CGI::StringToURLString( this->authenticationToken));
    } else if (changingPass)
      theGlobalVariables.userDefault=desiredUser;
    else
    { theGlobalVariables.userDefault="";
      theGlobalVariables.SetWebInput
      ("error", CGI::StringToURLString("<b>Invalid user or password.</b> Comments follow. "+argumentProcessingFailureComments.str()));
    }
  }
  password="********************************************";
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
  << "<input type=\"hidden\" id=\"authenticationToken\" name=\"authenticationToken\">\n"
  << "<input type=\"hidden\" id=\"usernameHidden\" name=\"usernameHidden\">\n"
  << "<input type=\"hidden\" id=\"currentExamHome\" name=\"currentExamHome\">\n"
  << "<input type=\"hidden\" id=\"currentExamIntermediate\" name=\"currentExamIntermediate\">\n"
  << "<input type=\"hidden\" id=\"currentExamFile\" name=\"currentExamFile\">\n"
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
  << theGlobalVariables.DisplayNameCalculatorWithPath << "\">\n";
  std::string civilizedInputSafish;
  if (CGI::StringToHtmlString(theParser.inputString, civilizedInputSafish))
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
    stOutput << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?" << theParser.inputStringRawestOfTheRaw << "\">Link to your input.</a>";
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
  stOutput << "<script type=\"text/javascript\" src=\"/autocomplete.js\"></script>\n";
  stOutput << this->closeIndentTag("</tr>");
  stOutput << this->closeIndentTag("</table>");
  stOutput << this->closeIndentTag("</td>");
  stOutput << this->closeIndentTag("</tr>");
  if (startingIndent!=this->indentationLevelHTML)
    crash << "Non-balanced html tags." << crash;
}

void WebWorker::OutputBeforeComputation()
{ MacroRegisterFunctionWithName("WebServer::OutputBeforeComputation");
  stOutput << "<html><meta name=\"keywords\" content= \"Root system, Root system Lie algebra, "
  << "Vector partition function calculator, vector partition functions, Semisimple Lie algebras, "
  << "Root subalgebras, sl(2)-triples\"> <head> <title>calculator version  " << __DATE__ << ", " << __TIME__ << "</title>";
//  if (theGlobalVariables.flagUsingBuiltInWebServer)
  stOutput << CGI::GetLaTeXProcessingJavascript();
//  else
//    stOutput << "<script src=\"" << theGlobalVariables.DisplayPathServerBase << "/jsmath/easy/load.js\">";
  stOutput << "<link rel=\"stylesheet\" type=\"text/css\" href=\"/styleCalculator.css\">";
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
  stOutput << this->openIndentTag("<table><!-- Output table, 2 cells (1 column 2 rows) -->");
  this->OutputBeforeComputationUserInputAndAutoComplete();
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
  FileOperations::OpenFileCreateIfNotPresentOnTopOfOutputFolder
  (outputTimeOutFile, theGlobalVariables.RelativePhysicalNameOutpuT, false, true, false);
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
  stOutput << "\n\n<script language=\"javascript\">\n" << "  var theAutocompleteDictionary = new Array;\n  ";
  for (int i=0; i<theParser.theAtoms.size; i++)
    if (theParser.theAtoms[i].size()>2)
      stOutput << "  theAutocompleteDictionary.push(\"" << theParser.theAtoms[i] << "\");\n";
  stOutput << "</script>\n";
  stOutput << "</body></html>";
  stOutput << "<!--";
  ProgressReport theReport(&theGlobalVariables);
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
//  << theGlobalVariables.DisplayNameCalculatorWithPath << "\nmainaddress.size: "
//  << this->mainAddress.size() << "\nonePredefinedCopyOfGlobalVariables.DisplayNameCalculatorWithPath.size(): "
//  << theGlobalVariables.DisplayNameCalculatorWithPath.size();
  this->mainArgumentRAW="";
  std::string calculatorArgumentRawWithQuestionMark, tempS;
  if (!MathRoutines::StringBeginsWith
      (this->mainAddresSRAW, theGlobalVariables.DisplayNameCalculatorWithPath, &calculatorArgumentRawWithQuestionMark))
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
  theLog << logger::yellow << "this->mainArgumentRAW=" << this->mainArgumentRAW << logger::endL;
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
    { theLog << "WebWorker::SendAllBytes failed. Error: "
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
    theLog << out.str() << logger::endL;
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
  this->remainingBytesToSend=(std::string) "HTTP/1.1 100 Continue\r\n";
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
      theLog << this->error << logger::endL;
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
    { this->error= "Error fetching message body: " + this->parent->ToStringLastErrorDescription();
      theLog << this->error << logger::endL;
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
  { if (foundslash && this->mainAddress[i]=='.')
      this->flagMainAddressSanitized=true;
    else
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
//  this->RelativePhysicalFileName= this->mainAddress.substr(numBytesToChop, std::string::npos);
}

int WebWorker::ProcessServerStatus()
{ MacroRegisterFunctionWithName("WebWorker::ProcessGetRequestServerStatus");
  stOutput << "<html>"
  << "<header>"
  << "<link rel=\"stylesheet\" type=\"text/css\" href=\"/styleCalculator.css\">"
  << "</header>"
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
    << inputWebWorkerNumber << "is out of range: there are " << this->parent->theWorkers.size
    << "workers. </b>";
    return 0;
  }
  if (!this->parent->theWorkers[inputWebWorkerIndex].flagInUse)
  { stOutput << "<b>Indicator error. Worker number " << inputWebWorkerNumber << " is not in use. "
    << "Total number of workers: " << this->parent->theWorkers.size << ". </b>";
    return 0;
  }
  if (inputWebWorkerIndex==this->indexInParent)
  { stOutput << "<b>Indicator error. Worker number " << inputWebWorkerNumber << " requested to monitor itself. "
    << " This is not allowed.</b>";
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
  if (!FileOperations::OpenFileCreateIfNotPresentOnTopOfOutputFolder(theFile, theGlobalVariables.RelativePhysicalNameProgressReport, false, true, false))
    FileOperations::OpenFileCreateIfNotPresentOnTopOfOutputFolder
    (theFile, "progressReport_failed_to_create_file.html",
     false, true, false);
  theFile << standardOutputStreamAfterTimeout.str() << "<hr>" << input;
  theFile.flush();
  theFile.close();
}

void WebWorker::PipeProgressReportToParentProcess(const std::string& input)
{ MacroRegisterFunctionWithName("WebWorker::PipeProgressReportToParentProcess");
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
  out << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" << "<html><body>";
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
  if (!CGI::GetPhysicalFileNameFromRelativeInput(this->RelativePhysicalFileName, this->PhysicalFileName_COMPUTED_DO_NOT_CHANGE))
  { stOutput << "HTTP/1.1 404 Object not found\r\nContent-Type: text/html\r\n\r\n";
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
  { stOutput << "HTTP/1.1 404 Object not found\r\nContent-Type: text/html\r\n\r\n";
    stOutput << "<html><body><b>File does not exist.</b>";
    if (this->flagMainAddressSanitized)
    { stOutput << "<hr>The original main address I extracted was: " << this->mainAddressNonSanitized
      << "<br>However, I do not allow addresses that contain dots (to avoid access to folders below the server). "
      << " Therefore I have sanitized the main address to: " << this->mainAddress;
    }
    stOutput << "<br><b> File display name:</b> "
    << this->mainAddress << "<br><b>Relative physical name of file:</b> " << this->RelativePhysicalFileName;
    stOutput << "<hr><hr><hr>Message details:<br>" << this->ToStringMessageUnsafe();
    stOutput << "</body></html>";
    return 0;
  }
  std::string fileExtension=FileOperations::GetFileExtensionWithDot(this->PhysicalFileName_COMPUTED_DO_NOT_CHANGE);
  bool isBinary=this->IsFileExtensionOfBinaryFile(fileExtension);
  std::fstream theFile;
  if (!FileOperations::OpenFileUnsecure(theFile, this->PhysicalFileName_COMPUTED_DO_NOT_CHANGE, false, false, !isBinary))
  { stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
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
  { theHeader << "HTTP/1.1 413 Payload Too Large\r\n"
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

  theHeader << "HTTP/1.1 200 OK\r\n" << this->GetMIMEtypeFromFileExtension(fileExtension)
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
  stOutput << "HTTP/1.1 501 Method Not Implemented\r\nContent-Type: text/html\r\n\r\n";
  stOutput << "<b>Requested method is not implemented. </b> <hr>The original message received from the server follows."
  << "<hr>\n" << this->ToStringMessageUnsafe();
  return 0;
}

int WebWorker::ProcessCalculator()
{ MacroRegisterFunctionWithName("WebServer::ProcessCalculator");
  ProgressReportWebServer theReport;
  std::stringstream argumentProcessingFailureComments;
  if (!this->ProcessRawArguments(theParser.inputStringRawestOfTheRaw, argumentProcessingFailureComments))
  { stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
    << "<html><body>" << "Failed to process the calculator arguments. <b>"
    << argumentProcessingFailureComments.str() << "</b></body></html>";
    stOutput.Flush();
    return 0;
  }
  if (this->flagPasswordWasSubmitted && theGlobalVariables.userCalculatorRequestType!="changePassword" &&
      theGlobalVariables.userCalculatorRequestType!="activateAccount"
      )
  { std::stringstream redirectedAddress;
    redirectedAddress << theGlobalVariables.DisplayNameCalculatorWithPath << "?";
    for (int i=0; i<theGlobalVariables.webFormArgumentNames.size; i++)
      if (theGlobalVariables.webFormArgumentNames[i]!="password")
        redirectedAddress << theGlobalVariables.webFormArgumentNames[i] << "=" << theGlobalVariables.webFormArguments[i] << "&";
    stOutput << "HTTP/1.1 303 See other\r\nLocation: ";
    stOutput << redirectedAddress.str();
    stOutput << "\r\n\r\n";
//    stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
//    stOutput << redirectedAddress.str();
//    stOutput << "\r\n";
    stOutput.Flush();
    return 0;
  }
  stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  stOutput << argumentProcessingFailureComments.str();
//  stOutput << "<br>got to this point, requesttype: " << theGlobalVariables.userCalculatorRequestType;
  if (theGlobalVariables.GetWebInput("error")!="")
    stOutput << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("error"));
  if (theGlobalVariables.userCalculatorRequestType=="pause")
    return this->ProcessPauseWorker();
  if (theGlobalVariables.userCalculatorRequestType=="status")
    return this->ProcessServerStatus();
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
  if ((theGlobalVariables.flagUsingSSLinCurrentConnection && !theGlobalVariables.flagLoggedIn)||
      theGlobalVariables.userCalculatorRequestType=="login")
    return this->ProcessLoginPage();
  if (theGlobalVariables.flagUsingSSLinCurrentConnection && theGlobalVariables.flagLoggedIn &&
      theGlobalVariables.userCalculatorRequestType=="logout")
    return this->ProcessLogout();
  if (( theGlobalVariables.userCalculatorRequestType=="addEmails"||
        theGlobalVariables.userCalculatorRequestType=="addUsers"   )
      &&
      theGlobalVariables.flagLoggedIn)
    return this->ProcessAddUserEmails();
  if ((theGlobalVariables.userCalculatorRequestType=="submitProblem" ||
       theGlobalVariables.userCalculatorRequestType=="submitExercise")&&
      theGlobalVariables.flagLoggedIn)
    return this->ProcessSubmitProblem();
  if (theGlobalVariables.userCalculatorRequestType=="submitProblemPreview" &&
      theGlobalVariables.flagLoggedIn)
    return this->ProcessSubmitProblemPreview();
  if (( theGlobalVariables.userCalculatorRequestType=="examForReal" ||
        theGlobalVariables.userCalculatorRequestType=="exercises") &&
      theGlobalVariables.flagLoggedIn &&
      theGlobalVariables.flagUsingSSLinCurrentConnection)
    return this->ProcessExamPage();
//  stOutput << "main request is: " << theGlobalVariables.userCalculatorRequestType
//  << "<br>web keys: " << theGlobalVariables.webFormArgumentNames.ToStringCommaDelimited()
//  << "<br>web entries: " << theGlobalVariables.webFormArguments.ToStringCommaDelimited();
  if (theGlobalVariables.userCalculatorRequestType=="browseDatabase" &&
      theGlobalVariables.flagLoggedIn &&
      theGlobalVariables.flagUsingSSLinCurrentConnection)
    return this->ProcessDatabase();
  theParser.inputString=CGI::URLStringToNormal(theGlobalVariables.GetWebInput("mainInput"));
  this->OutputBeforeComputation();
  theWebServer.CheckExecutableVersionAndRestartIfNeeded(false);
  theParser.init();
  if (theGlobalVariables.flagUsingBuiltInWebServer)
    theReport.SetStatus("OutputWeb: Computing...");
  if (theParser.inputString!="")
    theParser.Evaluate(theParser.inputString);
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
//  out << "  el.contentWindow.location.reload();";
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

std::string WebWorker::GetLoginHTMLinternal()
{ MacroRegisterFunctionWithName("WebWorker::GetLoginHTMLinternal");
  std::stringstream out;
//  out << "<hr>message main: "
//  << this->theMessage
//  << "<hr> main argument: "
//  << this->mainArgumentRAW;
  out << "<form name=\"login\" id=\"login\" action=\"calculator\" method=\"POST\" accept-charset=\"utf-8\">"
  <<  "User name: "
  << "<input type=\"text\" name=\"username\" placeholder=\"username\" required>"
  << "<br>Password: "
  << "<input type=\"password\" name=\"password\" placeholder=\"password\" autocomplete=\"on\">"
  << this->GetHtmlHiddenInputExercise()
  << this->GetHtmlHiddenInputs()
  << "<input type=\"submit\" value=\"Login\">"
  << "</form>";
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
  out << "<header>";
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
  << "  https.open(\"POST\", \"" << theGlobalVariables.DisplayNameCalculatorWithPath << "\", true);\n"
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
  out << "</header>";
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
  << "<input type=\"password\"  id=\"reenteredPassword\" placeholder=\"password\">\n"
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
  this->authenticationToken="";
  if (!theGlobalVariables.flagLoggedIn)
  { stOutput << "<span style=\"color:red\"><b> Password change available only to logged in users.</b></span>";
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
  << theGlobalVariables.DisplayNameCalculatorWithPath  << "?request=login"
  << "&username="
  << CGI::StringToURLString(theGlobalVariables.userDefault)
  << "&authenticationToken=" << CGI::StringToURLString(newAuthenticationToken)
  << "&currentExamIntermediate="
  << theGlobalVariables.GetWebInput("currentExamIntermediate")
  << "&currentExamFile="
  << theGlobalVariables.GetWebInput("currentExamFile")
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

std::string WebWorker::GetLoginPage()
{ MacroRegisterFunctionWithName("WebWorker::GetLoginPage");
  std::stringstream out;
  out << "<html>"
  << WebWorker::GetJavascriptStandardCookies()
  << "<body ";
  out << "onload=\"loadSettings();  ";
  if (!this->flagAuthenticationTokenWasSubmitted)
    out
    << "if (document.getElementById('authenticationToken') !=null)"
    << "  if (document.getElementById('authenticationToken').value!='')"
    << "    document.getElementById('login').submit();";
//    << "  window.location='calculator?username='+GlobalUser+'&authenticationToken='+GlobalAuthenticationToken;";
  out << "\">\n";
  theWebServer.CheckExecutableVersionAndRestartIfNeeded(true);
  out << WebWorker::GetLoginHTMLinternal() << "</body></html>";
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
  << "function addCookie(theName, theValue, exdays)\n"
  << "{ exdate= new Date();\n"
  << "  exdate.setDate(exdate.getDate() + exdays);\n"
  << "  c_value=escape(theValue) + ((exdays==null) ? \"\" : \"; expires=\"+exdate.toUTCString());\n"
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
    out << "  addCookie(\"debugFlag\", \"" << theGlobalVariables.GetWebInput("debugFlag") << "\", 100);  \n";
  if (theGlobalVariables.flagLoggedIn && theGlobalVariables.userCalculatorRequestType!="" &&
      theGlobalVariables.userCalculatorRequestType!="compute")
  { if (this->IsAllowedAsRequestCookie(theGlobalVariables.userCalculatorRequestType))
      out << "  addCookie(\"request\", \"" << theGlobalVariables.userCalculatorRequestType << "\", 100);\n";
    out << "  addCookie(\"currentExamFile\", \""
    << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamFile")) << "\", 100);\n";
    out << "  addCookie(\"currentExamHome\", \""
    << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamHome")) << "\", 100);\n";
    out << "  addCookie(\"currentExamIntermediate\", \""
    << CGI::URLStringToNormal(theGlobalVariables.GetWebInput("currentExamIntermediate")) << "\", 100);\n";
  }
  out
  << "}\n";
  out
  << "function storeSettingsSecurity(){\n";
  if (theGlobalVariables.flagUsingSSLinCurrentConnection&& theGlobalVariables.flagLoggedIn)
    out << "   addCookie(\"authenticationToken\", \""
    << //CGI::StringToURLString
    this->authenticationToken << "\", 150);"
    << "//150 days is a little longer than a semester\n"
    << "  addCookie(\"username\", \"" << theGlobalVariables.userDefault << "\", 150);\n";
  out
  << "}\n";
  out
//  << "function getCalculatorCGIsettings(){\n"
//  << "  result =\"examStatus=\"+getCookie(\"examStatus\");\n"
//  << "  result+=\"&username=\"+getCookie(\"username\");\n"
//  << "  result+=\"&authenticationToken=\"+getCookie(\"authenticationToken\");\n"
//  << "  result+=\"&currentExamFile=\"+getCookie(\"currentExamFile\");\n"
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
  << "  if (document.getElementById(\"request\")!=null)\n "
  << "    if(getCookie(\"request\")!='')\n"
  << "      document.getElementById(\"request\").value=getCookie(\"request\");\n"
  << "  if (document.getElementById(\"currentExamFile\")!=null)\n "
  << "    if(getCookie(\"currentExamFile\")!='')\n"
  << "      document.getElementById(\"currentExamFile\").value=getCookie(\"currentExamFile\");\n"
  << "  if (document.getElementById(\"currentExamHome\")!=null)\n "
  << "    if(getCookie(\"currentExamHome\")!='')\n"
  << "      document.getElementById(\"currentExamHome\").value=getCookie(\"currentExamHome\");\n"
  << "  if (document.getElementById(\"currentExamIntermediate\")!=null)\n "
  << "    if(getCookie(\"currentExamIntermediate\")!='')\n"
  << "      document.getElementById(\"currentExamIntermediate\").value=getCookie(\"currentExamIntermediate\");\n"
  << "  if (document.getElementById(\"authenticationToken\")!=null)\n"
  << "    if(getCookie(\"authenticationToken\")!='')\n"
  << "      document.getElementById(\"authenticationToken\").value=getCookie(\"authenticationToken\");\n "
  << "  if (document.getElementById(\"usernameHidden\")!=null)\n"
  << "    if(getCookie(\"username\")!='')\n"
  << "      document.getElementById(\"usernameHidden\").value=getCookie(\"username\");\n "
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
//  out << "  el.contentWindow.location.reload();";
  out << "  timeOutCounter+=timeIncrementInTenthsOfSecond;\n";
  out << "  var oRequest = new XMLHttpRequest();\n";
  if (inputIndex==-1)
    theLog << logger::red << "Worker index in parent is -1!!!" << logger::endL;
  else
    theLog << "Worker index: " << inputIndex << logger::endL;
  out << "  var sURL  = \"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?request=indicator&mainInput="
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
  out << "  pauseURL  = \"" << theGlobalVariables.DisplayNameCalculatorWithPath
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

int WebWorker::ServeClient()
{ MacroRegisterFunctionWithName("WebServer::ServeClient");
  ProgressReportWebServer theReport;
  theReport.SetStatus("All bytes from client received, processing. Worker process in use ...");
  theGlobalVariables.flagComputationStarted=true;
  theGlobalVariables.IndicatorStringOutputFunction=WebServer::PipeProgressReportToParentProcess;
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
{ MacroRegisterFunctionWithName("WebServer::ReleaseMyPipe");
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
{ MacroRegisterFunctionWithName("WebServer::OutputShowIndicatorOnTimeout");
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
  out << "Worker " << this->indexInParent+1;
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
  theGlobalVariables.WebServerTimerPing=0;
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
  for (int i=0; i<this->theWorkers.size; i++)
    if (!this->theWorkers[i].flagInUse)
    { this->activeWorker=i;
      this->theWorkers[i].pingMessage="";
      break;
    }
  if (this->activeWorker==-1)
  { this->activeWorker=this->theWorkers.size;
    this->theWorkers.SetSize(this->theWorkers.size+1);
  }
  this->GetActiveWorker().indexInParent=this->activeWorker;
  this->GetActiveWorker().Release();
  this->GetActiveWorker().parent=this;
  this->GetActiveWorker().timeOfLastPingServerSideOnly=theGlobalVariables.GetElapsedSeconds();
  this->GetActiveWorker().pingMessage="";
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

std::string WebServer::ToStringStatusAll()
{ MacroRegisterFunctionWithName("WebServer::ToStringStatusFull");
  std::stringstream out;
  if (this->activeWorker==-1)
    out << "The process is functioning as a server.";
  else
  { out << "The process is functioning as a worker. The active worker is number " << this->activeWorker+1 << ".";
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
  out << "<hr><hr>" << numInUse << " workers in use out of total " << this->theWorkers.size << " workers.";
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
{ crash << "Caught segfault at address: " << si->si_addr << crash;
  exit(0);
}

void fperror_sigaction(int signal)
{ crash << "Fatal arithmetic error. " << crash;
  exit(0);
}

void WebServer::RecycleChildrenIfPossible()
{ //Listen for children who have exited properly.
  //This might need to be rewritten: I wasn't able to make this work with any
  //mechanism other than pipes.
  MacroRegisterFunctionWithName("WebServer::RecycleChildrenIfPossible");
//  this->ReapChildren();
  for (int i=0; i<this->theWorkers.size; i++)
    if (this->theWorkers[i].flagInUse)
    { this->theWorkers[i].pipeWorkerToServerControls.Read();
      if (this->theWorkers[i].pipeWorkerToServerControls.lastRead.size>0)
      { this->theWorkers[i].flagInUse=false;
        theLog << logger::green << "Worker " << i+1 << " done, marking for reuse." << logger::endL;
//        waitpid(this->theWorkers[i].ProcessPID, 0, )
      } else
        theLog << logger::yellow << "Worker " << i+1 << " not done yet." << logger::endL;
      this->theWorkers[i].pipeWorkerToServerUserInput.Read();
      if (this->theWorkers[i].pipeWorkerToServerUserInput.lastRead.size>0)
        this->theWorkers[i].displayUserInput.assign
        (this->theWorkers[i].pipeWorkerToServerUserInput.lastRead.TheObjects,
          this->theWorkers[i].pipeWorkerToServerUserInput.lastRead.size);
      this->theWorkers[i].pipeWorkerToServerTimerPing.Read();
      if (this->theWorkers[i].pipeWorkerToServerTimerPing.lastRead.size>0)
      { this->theWorkers[i].pingMessage.assign
        (this->theWorkers[i].pipeWorkerToServerTimerPing.lastRead.TheObjects,
         this->theWorkers[i].pipeWorkerToServerTimerPing.lastRead.size);
        this->theWorkers[i].timeOfLastPingServerSideOnly=theGlobalVariables.GetElapsedSeconds();
      } else if (this->theWorkers[i].PauseWorker.CheckPauseIsRequested())
      { this->theWorkers[i].pingMessage="worker paused, no pings.";
        this->theWorkers[i].timeOfLastPingServerSideOnly=theGlobalVariables.GetElapsedSeconds();
      } else if ( theGlobalVariables.MaxTimeNoPingBeforeChildIsPresumedDead>0 &&
                  theGlobalVariables.GetElapsedSeconds()-this->theWorkers[i].timeOfLastPingServerSideOnly>
                  theGlobalVariables.MaxTimeNoPingBeforeChildIsPresumedDead)
      { this->theWorkers[i].flagInUse=false;
        kill(this->theWorkers[i].ProcessPID, SIGKILL);
        std::stringstream pingTimeoutStream;
        pingTimeoutStream << theGlobalVariables.GetElapsedSeconds()-this->theWorkers[i].timeOfLastPingServerSideOnly
        << " seconds passed since worker " << i+1
        << " last pinged the server; killing pid: "
        << this->theWorkers[i].ProcessPID << ". ";
        logIO << logger::red << pingTimeoutStream.str() << logger::endL;
        this->theWorkers[i].pingMessage="<span style=\"color:red\"><b>" + pingTimeoutStream.str()+"</b></span>";
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
          theLog << "Error: bind failed. " << this->ToStringLastErrorDescription() << logger::endL;
          continue;
        }
        fcntl(*theListeningSocket, F_SETFL, O_NONBLOCK);
        break;
      }
      if (p!=NULL)
      { theLog << logger::yellow << "Successfully bound to port " << (*thePorts)[i] << logger::endL;
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
  sa.sa_handler = 0;// &WebServer::Signal_SIGCHLD_handler; // reap all dead processes
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

int WebServer::Run()
{ MacroRegisterFunctionWithName("WebServer::Run");
  theGlobalVariables.RelativePhysicalNameCrashLog="crash_WebServerRun.html";
  if (!this->initPrepareWebServerALL())
    return 1;
  theLog << logger::purple <<  "server: waiting for connections...\r\n" << logger::endL;

  unsigned int connectionsSoFar=0;
  sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  char userAddressBuffer[INET6_ADDRSTRLEN];
  this->initSSL();
  fd_set FDListenSockets;
  while(true)
  { // main accept() loop
    sin_size = sizeof their_addr;
//    theLog << logger::red << "select returned!" << logger::endL;
    FD_ZERO(&FDListenSockets);
    for (int i=0; i<this->theListeningSockets.size; i++)
      FD_SET(this->theListeningSockets[i], &FDListenSockets);
    while (select(this->highestSocketNumber+1, & FDListenSockets, 0, 0, 0)==-1)
    { logIO << logger::red << "Select failed: this is not supposed to happen. The select error message was: "
      << strerror(errno) << logger::endL;
    }
    int newConnectedSocket =-1;
    theGlobalVariables.flagUsingSSLinCurrentConnection=false;
    for (int i=theListeningSockets.size-1; i>=0; i--)
      if (FD_ISSET(this->theListeningSockets[i], &FDListenSockets))
      { newConnectedSocket=accept(this->theListeningSockets[i], (struct sockaddr *)&their_addr, &sin_size);
        if (newConnectedSocket>=0)
        { logIO << logger::green << "Connected via listening socket " << this->theListeningSockets[i]
          << " on socket: " << newConnectedSocket;
          if (this->theListeningSockets[i]==this->listeningSocketHttpSSL)
          { theGlobalVariables.flagUsingSSLinCurrentConnection=true;
            logIO << logger::purple << " (SSL encrypted)." << logger::endL;
          } else
            logIO << logger::yellow << " (non-encrypted)." << logger::endL;
          break;
        }
      }
    if (newConnectedSocket <0)
    { theLog << logger::red << "This is not supposed to happen: accept failed. Error: "
      << this->ToStringLastErrorDescription() << logger::endL;
      continue;
    }
//    theLog << logger::purple << "NewconnectedSocket: " << newConnectedSocket << ", listeningSocket: "
//    << theListeningSocket << logger::endL;
    this->RecycleChildrenIfPossible();
    if (!this->CreateNewActiveWorker())
    { logBlock << logger::purple << "Failed to create an active worker. System error string: "
      << strerror(errno) << logger::endL;
      logIO << logger::red << "Failed to create active worker: closing connection. " << logger::endL;
      close (newConnectedSocket);
      continue;
    }
    this->GetActiveWorker().connectedSocketID=newConnectedSocket;
    this->GetActiveWorker().connectedSocketIDLastValueBeforeRelease=newConnectedSocket;
    connectionsSoFar++;
    this->GetActiveWorker().connectionID=connectionsSoFar;
    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), userAddressBuffer, sizeof userAddressBuffer);
    this->GetActiveWorker().userAddress=userAddressBuffer;
//    theLog << this->ToStringStatus();
    this->GetActiveWorker().ProcessPID=fork(); //creates an almost identical copy of this process.
    //The original process is the parent, the almost identical copy is the child.
    //theLog << "\r\nChildPID: " << this->childPID;
    if (this->GetActiveWorker().ProcessPID==-1)
      logIO << logger::red << "FAILED to spawn a child process. " << logger::endL;
    if (this->GetActiveWorker().ProcessPID==0)
    { // this is the child (worker) process
      this->SSLServerSideHandShake();
//      this->Release(theListeningSocket);//worker has no access to socket listener
      theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection=
      this->ReturnActiveIndicatorAlthoughComputationIsNotDone;
      theGlobalVariables.WebServerTimerPing=this->WorkerTimerPing;
      theGlobalVariables.flagAllowUseOfThreadsAndMutexes=true;
      crash.CleanUpFunction=WebServer::SignalActiveWorkerDoneReleaseEverything;
      InitializeTimer();
      CreateTimerThread();
      /////////////////////////////////////////////////////////////////////////
//      stOutput.theOutputFunction=WebServer::SendStringThroughActiveWorker;
//      stOutput.flushOutputFunction=this->FlushActiveWorker;
//      theLog << this->ToStringStatusActive() << logger::endL;
      this->GetActiveWorker().CheckConsistency();
//      std::cout << "Got thus far 7" << std::endl;

      //if (!this->flagUseSSL)
      //{
      if (!this->GetActiveWorker().ReceiveAll())
      { stOutput << "HTTP/1.1 400 Bad request\r\nContent-type: text/html\r\n\r\n"
        << "<html><body><b>HTTP error 400 (bad request). </b> There was an error with the request. "
        << "One possibility is that the input was too large. "
        << "<br>The error message returned was:<br>"
        << this->GetActiveWorker().error
        << " <hr><hr>The message (part) that was received is: "
        << this->GetActiveWorker().ToStringMessageFullUnsafe()
        << "</body></html>";
        stOutput.Flush();
        this->GetActiveWorker().SendAllBytes();
//        this->ReleaseEverything();
//        this->SSLfreeEverythingShutdownSSL();
        return -1;
      }
      this->GetActiveWorker().SendDisplayUserInputToServer();
     // std::cout << "Got thus far 9" << std::endl;
      int result= this->GetActiveWorker().ServeClient();
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

int WebServer::main_command_input()
{ MacroRegisterFunctionWithName("main_command_input");
  theGlobalVariables.IndicatorStringOutputFunction=CGI::MakeStdCoutReport;
  //  stOutput << "\n\n\n" << theParser.DisplayPathServerBase << "\n\n";
  //  return 0;
  theParser.init();
  theParser.inputStringRawestOfTheRaw =theGlobalVariables.programArguments[0];
  theParser.flagUseHtml=false;
  theParser.Evaluate(theParser.inputStringRawestOfTheRaw);
  std::fstream outputFile;
  FileOperations::OpenFileCreateIfNotPresentOnTopOfOutputFolder
  (outputFile, "outputFileCommandLine.html", false, true, false);
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
	WebWorker theWorker;
  return theWorker.ProcessCalculator();
}

extern int mainTest(List<std::string>& remainingArgs);

void WebServer::AnalyzeMainArguments(int argC, char **argv)
{ MacroRegisterFunctionWithName("WebServer::AnalyzeMainArguments");
  if (argC<0)
    argC=0;
  theGlobalVariables.programArguments.SetSize(argC);
  for (int i=0; i<argC; i++)
    theGlobalVariables.programArguments[i]=argv[i];
  std::cout << "\nProgram arguments: " << theGlobalVariables.programArguments.ToStringCommaDelimited() << "\n";

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
  theGlobalVariables.flagUsingBuiltInWebServer=
  (secondArgument=="server" || secondArgument=="server8155" || secondArgument=="serverSSL");
  theGlobalVariables.flagRunningCommandLine=!theGlobalVariables.flagUsingBuiltInWebServer;
  if (theGlobalVariables.flagRunningCommandLine)
  { theGlobalVariables.programArguments.PopIndexShiftDown(0);
    for (int i=1; i<theGlobalVariables.programArguments.size; i++)
      theGlobalVariables.programArguments[0]+="\n"+ theGlobalVariables.programArguments[i];
    theGlobalVariables.programArguments.SetSize(1);
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
  theWebServer.AnalyzeMainArguments(argc, argv);
  //theLog << logger::red << " At ze moment theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit= "
  //<< theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit << logger::endL;

  if (theGlobalVariables.flagRunningConsoleTest)
    return mainTest(theGlobalVariables.programArguments);
  if (theGlobalVariables.flagUsingApacheWebServer)
    return WebServer::main_apache_client();
  if (theGlobalVariables.flagUsingBuiltInWebServer)
    return theWebServer.Run();
  if (theGlobalVariables.flagRunningCommandLine)
    return WebServer::main_command_input();
  }
  catch (...)
  { crash << "Exception caught: something very wrong has happened. " << crash;
  }
  crash << "This point of code is not supposed to be reachable. " << crash;
  return -1;
}
