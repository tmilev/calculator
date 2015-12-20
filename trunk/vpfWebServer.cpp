//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServer.h"
#include "vpfHeader3Calculator0_Interface.h"

ProjectInformationInstance projectInfoInstanceWebServer(__FILE__, "Web server implementation.");

#include <sys/wait.h>//<-waitpid f-n here
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <arpa/inet.h> // <- inet_ntop declared here (ntop= network to presentation)
#include <unistd.h>
#include <sys/stat.h>//<-for file statistics



#ifdef MACRO_use_open_ssl
//installation of these headers in ubuntu:
//sudo apt-get install libssl-dev
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
{ if (!this->flagUsESSL)
    return;
  MacroRegisterFunctionWithName("WebServer::initSSL");
#ifdef MACRO_use_open_ssl

  const SSL_METHOD *meth;
  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();
  meth = TLSv1_2_server_method();
  theSSLdata.ctx = SSL_CTX_new (meth);
  if (!theSSLdata.ctx)
  { ERR_print_errors_fp(stderr);
    crash << "openssl error: failed to create CTX object." << crash;
  }
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
#endif // MACRO_use_open_ssl
}

void WebServer::SSLfreeResources()
{ if (!this->GetActiveWorker().flagUsingSSLinCurrentConnection)
    return;
#ifdef MACRO_use_open_ssl
  SSL_free (theSSLdata.ssl);
#endif // MACRO_use_open_ssl
}

void WebServer::SSLfreeEverythingShutdownSSL()
{ if (!this->flagUsESSL)
    return;
#ifdef MACRO_use_open_ssl
  SSL_CTX_free (theSSLdata.ctx);
#endif // MACRO_use_open_ssl
}

void WebServer::SSLServerSideHandShake()
{ if (!this->flagUsESSL)
    return;
  if (!this->GetActiveWorker().flagUsingSSLinCurrentConnection)
    return;
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
  if (!this->flagUsingSSLinCurrentConnection)
    return true;
#ifdef MACRO_use_open_ssl
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
  int numBytesInBuffer= SSL_read(theSSLdata.ssl, &buffer, bufferSize-1);
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
  return true;
#endif // MACRO_use_open_ssl
}

void WebWorker::SendAllBytesHttpSSL()
{ if (this->remainingBytesToSend.size==0)
    return;
  MacroRegisterFunctionWithName("WebWorker::SendAllBytesHttpSSL");
  this->CheckConsistency();
  if (!this->flagUsingSSLinCurrentConnection)
    crash << "Error: WebWorker::SendAllBytesHttpSSL called while flagUsingSSLinCurrentConnection is set to false. " << crash;
#ifdef MACRO_use_open_ssl
  ProgressReportWebServer theReport;
  theReport.SetStatus("WebWorker::SendAllBytesHttps: starting ...");
  if (this->connectedSocketID==-1)
  { theLog << logger::red << "Socket::SendAllBytes failed: connectedSocketID=-1." << logger::endL;
    theReport.SetStatus("Socket::SendAllBytes failed: connectedSocketID=-1. WebWorker::SendAllBytes - finished.");
    return;
  }
  theLog << "Sending " << this->remainingBytesToSend.size << " bytes in chunks of: " << logger::endL;
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
    int numBytesSent =  SSL_write (theSSLdata.ssl, this->remainingBytesToSend.TheObjects, this->remainingBytesToSend.size);
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
    theLog << logger::endL;
  }
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
{ theLog << "Signal child handler called with input: " << s << "." << logger::endL;
  int waitResult=0;
  do
  { waitResult= waitpid(-1, NULL, WNOHANG| WEXITED);
    if (waitResult>0)
      for (int i=0; i<theWebServer.theWorkers.size; i++)
        if (theWebServer.theWorkers[i].ProcessPID==waitResult)
          theWebServer.theWorkers[i].pipeWorkerToServerControls.WriteAfterEmptying("close");
  }while (waitResult>0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{ if (sa->sa_family == AF_INET)
    return &(((struct sockaddr_in*)sa)->sin_addr);
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

std::string WebWorker::ToStringMessageShort(FormatExpressions* theFormat)const
{ std::stringstream out;
  bool useHtml=theFormat==0 ? false: theFormat->flagUseHTML;
  std::string lineBreak= useHtml ? "<br>\n" : "\r\n";
  out << lineBreak;
  if (this->requestType==this->requestGetCalculator)
    out << "GET " << "(from calculator)";
  else if (this->requestType==this->requestPostCalculator)
    out << "POST " << "(from calculator)";
  else if (this->requestType==this->requestGetNotCalculator)
    out << "GET " << "(NOT from calculator)";
  else if (this->requestType==this->requestTogglePauseCalculator)
    out << "GET " << "(pause computation)";
  else if (this->requestType==this->requestGetServerStatus)
    out << "GET (server status)";
  else if (this->requestType==this->requestGetComputationIndicator)
    out << "GET (computation indicator)";
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

std::string WebWorker::ToStringMessageFull()const
{ std::stringstream out;
  out << this->ToStringMessage();
  if (this->theStrings.size>0)
  { out << "<hr>\nStrings extracted from message: ";
    for (int i =0; i<this->theStrings.size; i++)
      out << "<br>" << this->theStrings[i];
  }
  return out.str();
}

std::string WebWorker::ToStringMessage()const
{ std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML=true;
  out << this->ToStringMessageShort(&tempFormat);

  out << "<hr>";
  out << "Main address RAW: " << this->mainAddresSRAW << "<br>";
  if (this->requestType==this->requestGetCalculator || this->requestType==this->requestGetNotCalculator ||
      this->requestType==this->requestGetServerStatus || this->requestType==this->requestGetComputationIndicator)
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

extern bool LoginViaDatabase
(const std::string& inputUsername, const std::string& inputPassword,
 std::string& inputOutputAuthenticationToken);

void WebWorker::ProcessRawArguments()
{ MacroRegisterFunctionWithName("WebServer::ProcessRawArguments");
  List<std::string> inputStrings, inputStringNames;
  CGI::ChopCGIInputStringToMultipleStrings(theParser.inputStringRawestOfTheRaw, inputStrings, inputStringNames);
  std::string password;
  this->user="";
  if (inputStringNames.Contains("user"))
  { this->user=inputStrings[inputStringNames.GetIndex("user")];
    CGI::URLStringToNormal(this->user, this->user);
    stOutput << "message: " << theParser.inputStringRawestOfTheRaw;
    stOutput << "<hr><b>user: </b>" << user << "<br>";
  }
  if (inputStringNames.Contains("authenticationToken"))
  { this->authenticationToken=inputStrings[inputStringNames.GetIndex("authenticationToken")];
    if (this->authenticationToken.size()>0)
      if (this->authenticationToken[this->authenticationToken.size()-1]!='=')
        this->authenticationToken.push_back('=');
    CGI::URLStringToNormal(this->authenticationToken, this->authenticationToken);
    stOutput << "<b>authenticationToken: </b><br>" << this->authenticationToken;
    this->flagAuthenticationTokenWasSubmitted=true;
  }
  if (inputStringNames.Contains("password"))
  { password=inputStrings[inputStringNames.GetIndex("password")];
    CGI::URLStringToNormal(password, password);
  }
  this->flagLoggedIn=false;
  if (this->user!="" && theGlobalVariables.flagUsingHttpSSL)
  { stOutput << "<br>LoginViaDatabase called: "
    << "user: " << this->user;
    this->flagLoggedIn=LoginViaDatabase(this->user, password, this->authenticationToken);
  }
  for (unsigned i=0; i<password.size(); i++)
    password[i]=' ';
  if (inputStringNames.Contains("textInput"))
    theParser.inputString= inputStrings[inputStringNames.GetIndex("textInput")];
}

void WebWorker::OutputBeforeComputation()
{ MacroRegisterFunctionWithName("WebServer::OutputBeforeComputation");
  theGlobalVariables.flagComputationCompletE=false;
  stOutput << "<html><meta name=\"keywords\" content= \"Root system, Root system Lie algebra, "
  << "Vector partition function calculator, vector partition functions, Semisimple Lie algebras, "
  << "Root subalgebras, sl(2)-triples\"> <head> <title>calculator version  " << __DATE__ << ", " << __TIME__ << "</title>";
//  if (theGlobalVariables.flagUsingBuiltInWebServer)
  stOutput << CGI::GetLaTeXProcessingJavascript();
//  else
//    stOutput << "<script src=\"" << theGlobalVariables.DisplayPathServerBase << "/jsmath/easy/load.js\">";
  stOutput << "\n</head>\n<body onload=\"checkCookie();\">\n";

//  civilizedInput="\\int( 1/x dx)";
//  civilizedInput="\\int (1/(x(1+x^2)^2))dx";
//  civilizedInput="%LogEvaluation \\int (1/(5+2x+7x^2)^2)dx";
  CGI::URLStringToNormal(theParser.inputString, theParser.inputString);
  theGlobalVariables.initOutputReportAndCrashFileNames
  (theParser.inputStringRawestOfTheRaw, theParser.inputString);

  stOutput << WebWorker::GetJavascriptHideHtml();
  stOutput << WebWorker::GetJavascriptCookieBasics();
  stOutput << this->GetJavascriptCookieForTheCalculator();
  stOutput << "<table>\n <tr valign=\"top\">\n ";
  stOutput << "<td>"; //<td> tag will be closed later in WebWorker::OutputStandardResult
  stOutput << "\n<FORM method=\"POST\" id=\"formCalculator\" name=\"formCalculator\" action=\""
  << theGlobalVariables.DisplayNameCalculatorWithPath << "\">\n" ;
  std::string civilizedInputSafish;
  if (CGI::GetHtmlStringSafeishReturnFalseIfIdentical(theParser.inputString, civilizedInputSafish))
    stOutput << "Your input has been treated normally, however the return string of your input has been modified. More precisely, &lt; and &gt;  are "
    << " modified due to a javascript hijack issue. <br>";
  stOutput << "<textarea rows=\"3\" cols=\"30\" name=\"textInput\" id=\"textInputID\", style=\"white-space:normal\" "
  << "onkeypress=\"if (event.keyCode == 13 && event.shiftKey) {storeSettings(); "
  << " this.form.submit(); return false;}\" "
  << "onkeyup=\"suggestWord();\", onkeydown=\"suggestWord(); arrowAction(event);\", onmouseup=\"suggestWord();\", oninput=\"suggestWord();\""
  << ">";
  stOutput << civilizedInputSafish;
  stOutput << "</textarea>\n<br>\n";
  if (this->flagLoggedIn && this->flagUsingSSLinCurrentConnection)
    stOutput << "<input type=\"hidden\" name=authenticationToken value=\"" << this->authenticationToken << "\">"
    << "<input type=\"hidden\" name=\"user\" value=\""
    << this->user << "\">";
  stOutput << "<input type=\"submit\" title=\"Shift+Enter=shortcut from input text box. \" "
  << "name=\"buttonGo\" value=\"Go\" onmousedown=\"storeSettings();\" ";
  stOutput << "action=\"calculator\"> ";
  if (theParser.inputString!="")
    stOutput << "<a href=\"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?" << theParser.inputStringRawestOfTheRaw << "\">Link to your input.</a>";
  stOutput << "\n</FORM>";
  //  stOutput << "<br>Number of lists created before evaluation: " << NumListsCreated;
}

void WebWorker::OutputResultAfterTimeout()
{ MacroRegisterFunctionWithName("WebWorker::OutputResultAfterTimeout");
  std::stringstream out;
//  out << theParser.ToStringOutputSpecials();
  if (standardOutputStreamAfterTimeout.str().size()!=0)
    out << standardOutputStreamAfterTimeout.str() << "<hr>";
  out << "<table><tr><td>" << theParser.ToStringOutputAndSpecials() << "</td><td>"
  << theParser.outputCommentsString << "</td></tr></table>";
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
  stOutput << theParser.ToStringOutputAndSpecials();
  stOutput << "\n\n</td>\n\n";
  stOutput << "<td>";
  stOutput << "<table>"
  << "<tr><td nowrap>"
  << "<span \"style:nowrap\" id=\"idAutocompleteHints\">Ctrl+space autocompletes, ctrl+arrow selects word</span>"
  << "</td></tr>"
  << "<tr>"
  << "<td id=\"idAutocompleteSpan\"></td>"
  << "</tr>"
  << "<tr>"
  << "<td id=\"idAutocompleteDebug\"></td>"
  << "</tr>"
  << "</table>";
  stOutput << "<script type=\"text/javascript\" src=\"/autocomplete.js\"></script>\n";
  stOutput << "</td>";
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
    system(theParser.SystemCommands[i].c_str());
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
  CGI::URLStringToNormal(this->mainAddresSRAW, this->mainAddress);
  this->mainArgumentRAW="";
  std::string calculatorArgumentRawWithQuestionMark, tempS;
  if (!MathRoutines::StringBeginsWith
      (this->mainAddresSRAW, theGlobalVariables.DisplayNameCalculatorWithPath, &calculatorArgumentRawWithQuestionMark))
    if (!MathRoutines::StringBeginsWith
        (this->mainAddresSRAW, "/vectorpartition/cgi-bin/calculator", &calculatorArgumentRawWithQuestionMark))
    { CGI::URLFileNameToFileName(this->mainAddresSRAW, this->mainAddress);
      return;
    }
  this->requestType=this->requestGetCalculator;
  MathRoutines::SplitStringInTwo(calculatorArgumentRawWithQuestionMark, 1, tempS, this->mainArgumentRAW);
  theLog << logger::yellow << "this->mainArgumentRAW=" << this->mainArgumentRAW << logger::endL;
  if (MathRoutines::StringBeginsWith(this->mainArgumentRAW, "monitor"))
    this->requestType=this->requestGetMonitor;
  if (MathRoutines::StringBeginsWith(this->mainArgumentRAW, "indicator"))
    this->requestType=this->requestGetComputationIndicator;
  if (MathRoutines::StringBeginsWith(this->mainArgumentRAW, "status"))
    this->requestType=this->requestGetServerStatus;
  if (MathRoutines::StringBeginsWith(this->mainArgumentRAW, "pauseIndicator"))
    this->requestType=this->requestTogglePauseCalculator;
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
  if (this->flagUsingSSLinCurrentConnection)
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
  theLog << "Sending " << this->remainingBytesToSend.size << " bytes in chunks of: " << logger::endL;
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
  if (this->flagUsingSSLinCurrentConnection)
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
  stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  stOutput << "<html><body> " << this->parent->ToStringStatusAll() << "</body></html>";
  return 0;
}

int WebWorker::ProcessPauseWorker()
{ MacroRegisterFunctionWithName("WebWorker::ProcessPauseWorker");
  theLog << "Proceeding to toggle worker pause." << logger::endL;
  stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  if (this->mainArgumentRAW.size()<=14)
  { stOutput << "<b>Indicator takes as argument the id of the child process that is running the computation.</b>";
    return 0;
  }
  int inputWebWorkerNumber= atoi(this->mainArgumentRAW.substr(14, std::string::npos).c_str());
  int inputWebWorkerIndex= inputWebWorkerNumber-1;
  if (inputWebWorkerIndex<0 || inputWebWorkerIndex>=this->parent->theWorkers.size)
  { stOutput << "<b>User requested " << this->mainArgumentRAW << " (worker number " << inputWebWorkerNumber
    << " out of " << this->parent->theWorkers.size << ") but the worker number is out of range. </b>";
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
  stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  if (this->mainArgumentRAW.size()<=7)
  { stOutput << "<b>Monitor takes as argument the id of the child process that is running the computation.</b>";
    return 0;
  }
  int inputWebWorkerNumber= atoi(this->mainArgumentRAW.substr(7, std::string::npos).c_str());
  stOutput << "<html><body>" << this->GetJavaScriptIndicatorBuiltInServer(inputWebWorkerNumber-1)
  << "</body></html>";
  return 0;
}

int WebWorker::ProcessComputationIndicator()
{ MacroRegisterFunctionWithName("WebWorker::ProcessComputationIndicator");
  theLog << "Processing get request indicator." << logger::endL;
  stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  ProgressReportWebServer theReport("Preparing indicator report");
  if (this->mainArgumentRAW.size()<=9)
  { stOutput << "<b>Indicator takes as argument the id of the child process that is running the computation.</b>";
    return 0;
  }
  int inputWebWorkerNumber= atoi(this->mainArgumentRAW.substr(9, std::string::npos).c_str());
  int inputWebWorkerIndex= inputWebWorkerNumber-1;
  if (inputWebWorkerIndex<0 || inputWebWorkerIndex>=this->parent->theWorkers.size)
  { stOutput << "<b>Indicator requested " << this->mainArgumentRAW << " (extracted worker number "
    << inputWebWorkerNumber << " out of " << this->parent->theWorkers.size << ") but the id is out of range. </b>";
    return 0;
  }
  if (!this->parent->theWorkers[inputWebWorkerIndex].flagInUse)
  { stOutput << "<b>Requested worker number " << inputWebWorkerNumber << " is not in use. "
    << "Total number of workers: " << this->parent->theWorkers.size << ". </b>";
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
  if (this->flagMainAddressSanitized)
  { out << "<hr>The original main address I extracted was: " << this->mainAddressNonSanitized
    << "<br>However, I do not allow addresses that contain dots (to avoid access to folders below the server). "
    << " Therefore I have sanitized the main address to: " << this->mainAddress;
  }
  List<std::string> theFileNames, theFileTypes;
  if (!FileOperations::GetFolderFileNamesOnTopOfOutputFolder(this->RelativePhysicalFileName, theFileNames, &theFileTypes))
  { out << "<b>Failed to open directory with physical address " << this->RelativePhysicalFileName << " </b></body></html>";
    stOutput << out.str();
    return 0;
  }
  out << "Browsing folder: " << this->mainAddress << "<br>Relative physical address: " << this->RelativePhysicalFileName << "<hr>";
  List<std::string> theFolderNamesHtml, theFileNamesHtml;
  for (int i=0; i<theFileNames.size; i++)
  { std::stringstream currentStream;
    bool isDir= theFileTypes[i]==".d";
    currentStream << "<a href=\"" << this->mainAddress << theFileNames[i];
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
  this->connectedSocketIDLastValueBeforeRelease=-1;
  this->connectionID=-1;
  this->indexInParent=-1;
  this->parent=0;
  this->displayUserInput="";
  this->requestType=this->requestUnknown;
  this->flagMainAddressSanitized=false;
  this->timeOfLastPingServerSideOnly=-1;
  this->flagUsingSSLinCurrentConnection=false;
  this->flagLoggedIn=false;
  this->flagAuthenticationTokenWasSubmitted=false;
  for (unsigned i=0; i<this->user.size(); i++)
    this->user[i]=' ';
  for (unsigned i=0; i<this->authenticationToken.size(); i++)
    this->authenticationToken[i]=' ';
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
  theLog << logger::blue << "worker " << this->indexInParent+1 << " is done with the work. " << logger::endL;
//  std::cout << "got thus far xxxxxxx" << std:SignalIamDoneReleaseEverything:endl;
  this->pipeWorkerToServerControls.WriteAfterEmptying("close");
  theLog << logger::blue << "Notification dispatched." << logger::endL;
  this->SendAllBytes();
  this->Release();
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
  return "Content-Type: application/octet-stream\r\n";
}

int WebWorker::ProcessNonCalculator()
{ MacroRegisterFunctionWithName("WebWorker::ProcessNonCalculator");
  this->ExtractPhysicalAddressFromMainAddress();
  ProgressReportWebServer theProgressReport;
  theProgressReport.SetStatus("<br>Processing non-computational web-server request.");
  //theLog << this->ToStringShort() << "\r\n";
//  std::cout << " ere be i at this moment10\n";
  if (FileOperations::IsFolderOnTopOfOutputFolder(this->RelativePhysicalFileName))
    return this->ProcessFolder();
//  std::cout << " ere be i at this moment\n";
  if (!FileOperations::FileExistsOnTopOfOutputFolder(this->RelativePhysicalFileName))
  { stOutput << "HTTP/1.1 404 Object not found\r\nContent-Type: text/html\r\n\r\n";
    stOutput << "<html><body><b>File does not exist.</b>";
    if (this->flagMainAddressSanitized)
    { stOutput << "<hr>The original main address I extracted was: " << this->mainAddressNonSanitized
      << "<br>However, I do not allow addresses that contain dots (to avoid access to folders below the server). "
      << " Therefore I have sanitized the main address to: " << this->mainAddress;
    }
    stOutput << "<br><b> File display name:</b> "
    << this->mainAddress << "<br><b>Relative physical name of file:</b> " << this->RelativePhysicalFileName;
    stOutput << "<hr><hr><hr>Message details:<br>" <<  this->ToStringMessage();
    stOutput << "</body></html>";
    return 0;
  }
  std::string fileExtension=FileOperations::GetFileExtensionWithDot(this->RelativePhysicalFileName);
  bool isBinary=this->IsFileExtensionOfBinaryFile(fileExtension);
  std::fstream theFile;
  if (!FileOperations::OpenFileOnTopOfOutputFolder(theFile, this->RelativePhysicalFileName, false, false, !isBinary))
  { stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    stOutput << "<html><body><b>Error: file appears to exist but I could not open it.</b> ";
    if (this->flagMainAddressSanitized)
    { stOutput << "<hr>The original main address I extracted was: " << this->mainAddressNonSanitized
      << "<br>However, I do not allow addresses that contain dots (to avoid access to folders below the server). "
      << " Therefore I have sanitized the main address to: " << this->mainAddress;
    }
    stOutput << "<br><b> File display name: </b>"
    << this->mainAddress << "<br><b>File physical name: </b>"
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
    theFile.read(&this->bufferFileIO[0], this->bufferFileIO.size);
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
  << "<hr>\n" << this->ToStringMessage();
  return 0;
}

int WebWorker::OutputWeb()
{ MacroRegisterFunctionWithName("WebServer::OutputWeb");
  ProgressReportWebServer theReport;
  theWebServer.GetActiveWorker().ProcessRawArguments();
  if (theGlobalVariables.flagUsingHttpSSL)
    if (!theWebServer.GetActiveWorker().flagLoggedIn)
    { stOutput << theWebServer.GetActiveWorker().GetLoginScreen();
      theReport.SetStatus("Serving login screen. ");
      theReport.SetStatus("Login screen served, exiting. ");
      theGlobalVariables.flagComputationCompletE=true;
      stOutput.Flush();
      return 0;
    }
  theWebServer.GetActiveWorker().OutputBeforeComputation();
  theWebServer.CheckExecutableVersionAndRestartIfNeeded();

  //stOutput << theParser.javaScriptDisplayingIndicator;
  //theParser.inputString="TestCalculatorIndicator 0";
  //theParser.inputString="printSemisimpleSubalgebrasRecompute(B_3)";
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
      WebWorker::OutputResultAfterTimeout();
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

std::string WebWorker::GetLoginHTMLelement()
{ MacroRegisterFunctionWithName("WebWorker::GetLoginHTMLelement");
  std::stringstream out;
  out << "<script type=\"text/javascript\">\n"
  << "function checkCookie(){\n"
  << "  authenticationToken=getCookie(\"authenticationToken\");\n"
  << "  user=getCookie(\"user\");\n"
  << "  if (authenticationToken!=\"\")\n"
  << "    window.location=\"/calculator?user=\"+user+\"&authenticationToken=\"+authenticationToken;\n "
  << "}\n"
  << "</script>\n"
  ;
  out << "<form name=\"login\" action=\"calculator\" method=\"get\" accept-charset=\"utf-8\">"
  <<  "User name or email: "
  << "<input type=\"text\" name=\"user\" placeholder=\"user\" required>"
  << "<br>Password: "
  << "<input type=\"password\" name=\"password\" placeholder=\"password\">"
  << "<br>Authentication token: <input type=\"password\" name=\"authenticationToken\" placeholder=\"authenticationToken\">"
  << "<br><input type=\"submit\" value=\"Login\">"
  << "</form>";
  out << "<b>Login screen not implemented yet.</b>";
  return out.str();
}

std::string WebWorker::GetLoginScreen()
{ MacroRegisterFunctionWithName("WebWorker::GetLoginScreen");
  std::stringstream out;
  out << "<html>"
  << WebWorker::GetJavascriptCookieBasics()
  << "<body ";
  if (!this->flagAuthenticationTokenWasSubmitted)
    out << "onload=\"checkCookie();\"";
  out << ">\n"
  << WebWorker::GetLoginHTMLelement()
  << "</body></html>";
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

std::string WebWorker::GetJavascriptCookieBasics()
{ std::stringstream output;
  output << " <script type=\"text/javascript\"> \n";
  output << " function getCookie(c_name)\n";
  output << " { VPFcookie=document.cookie.split(\";\");\n";
  output << "   for (i=0;i<VPFcookie.length;i++)\n";
  output << "   { x=VPFcookie[i].substr(0,VPFcookie[i].indexOf(\"=\"));\n";
  output << "   	y=VPFcookie[i].substr(VPFcookie[i].indexOf(\"=\")+1);\n";
  output << "     x=x.replace(/^\\s+|\\s+$/g,\"\");\n";
  output << "     if (x==c_name)\n";
  output << "       return unescape(y);\n";
  output << "   }\n";
  output << "   return \"\";\n";
  output << " }\n";
  output << " \n";
  output << " function addCookie(theName, theValue, exdays)\n";
  output << " { exdate= new Date();\n";
  output << "   exdate.setDate(exdate.getDate() + exdays);\n";
  output << "   c_value=escape(theValue) + ((exdays==null) ? \"\" : \"; expires=\"+exdate.toUTCString());\n";
  output << "   document.cookie=theName + \"=\" + c_value;\n";
  output << " }\n";
  output << "</script>\n";
  return output.str();

}

std::string WebWorker::GetJavascriptCookieForTheCalculator()
{ std::stringstream output;
  output << WebWorker::GetJavascriptCookieBasics();
  output << " <script type=\"text/javascript\"> \n";
  output << " function storeSettings()\n";
  output << " { theCalculatorForm=document.getElementById(\"textInputID\");  \n";
  output << "   //alert(theCalculatorForm.style.width);\n";
  output << "   addCookie(\"widthCalculatorText\", theCalculatorForm.style.width, 100);  \n";
  output << "   addCookie(\"heightCalculatorText\", theCalculatorForm.style.height, 100);\n";
  output << "   //alert(document.cookie);\n";
  output << " }\n";
  output << " \n";
  output << " function checkCookie()\n";
  output << " { theCalculatorForm=document.getElementById(\"textInputID\");  \n";
  output << "   theOldWidth=getCookie(\"widthCalculatorText\");\n";
  output << "   theOldHeight=getCookie(\"heightCalculatorText\");\n";
  output << "   //alert(\"height: \" + theOldHeight +\" width: \" + theOldWidth);\n";
  output << "   //theCalculatorForm.setStyle(\"width:\"+ theOldWidth);\n";
  output << " //  theCalculatorForm.style.height=theOldHeight;\n";
  output << "   theCalculatorForm.style.width  = theOldWidth;\n";
  output << "   theCalculatorForm.style.height = theOldHeight;\n";
  if (this->flagUsingSSLinCurrentConnection && this->flagLoggedIn)
    output << "   addCookie(\"authenticationToken\", \"" << this->authenticationToken << "\", 150);"
    << "//150 days is a little longer than a semester\n"
    << "   addCookie(\"user\", \"" << this->user << "\", 150);\n"
    ;
  output << " }\n";
  output << " </script>\n";
  return output.str();
}

std::string WebWorker::GetJavaScriptIndicatorBuiltInServer(int inputIndex)
{ MacroRegisterFunctionWithName("WebWorker::GetJavaScriptIndicatorBuiltInServer");
  std::stringstream out;
  out << " <!>\n";
//  out << "\n<br>\n<button onclick=\"progressReport()\">Manual report</button>";
  out << "\n<br>\n<button id=\"idButtonSendTogglePauseRequest\" onclick=\"SendTogglePauseRequest()\">Pause</button>";
  out << "<span id=\"idPauseToggleServerResponse\"></span>\n";
  out << "<span id=\"idProgressReportTimer\"></span>\n";
  out << "\n<br>\n<div id=\"idProgressReport\"></div>\n";
  out << "\n<script type=\"text/javascript\"> \n";
  out << "var isPaused=false;\n";
  out << "var isFinished=false;\n";
  out << "var timeIncrementInTenthsOfSecond=4;//measured in tenths of a second\n";
  out << "var timeOutCounter=0;//measured in tenths of a second\n";
  out << "progressReport();\n";
  out << "function SendTogglePauseRequest()\n";
  out << "{ if (isFinished)\n";
  out << "    return;\n";
  out << "  var pauseRequest = new XMLHttpRequest();\n";
  theLog << "Generating indicator address for worker number " << inputIndex+1 << "." << logger::endL;
  out << "  pauseURL  = \"" << theGlobalVariables.DisplayNameCalculatorWithPath
  << "?pauseIndicator" << inputIndex+1 << "\";\n";
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
  out << "  var sURL  = \"" << theGlobalVariables.DisplayNameCalculatorWithPath << "?indicator"
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
  if (this->requestType!=this->requestGetComputationIndicator &&
      this->requestType!=this->requestGetServerStatus &&
      this->requestType!=this->requestTogglePauseCalculator &&
      this->requestType!=this->requestGetMonitor)
    this->parent->ReleaseNonActiveWorkers();
    //unless the worker is an indicator, it has no access to communication channels of the other workers
  if (this->requestType==this->requestGetCalculator || this->requestType==this->requestPostCalculator)
  { stOutput << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    theParser.inputStringRawestOfTheRaw=this->mainArgumentRAW;
    //theParser.javaScriptDisplayingIndicator=this->GetJavaScriptIndicatorBuiltInServer(true);
    theParser.javaScriptDisplayingIndicator="";
    this->OutputWeb();
  } else if (this->requestType==this->requestGetNotCalculator)
    this->ProcessNonCalculator();
  else if (this->requestType==this->requestTogglePauseCalculator)
    this->ProcessPauseWorker();
  else if (this->requestType==this->requestGetServerStatus)
  { this->ProcessServerStatus();
    this->parent->ReleaseNonActiveWorkers();
  } else if (this->requestType==this->requestGetComputationIndicator)
  { this->ProcessComputationIndicator();
    this->parent->ReleaseNonActiveWorkers();
  } else if (this->requestType==this->requestGetMonitor)
  { this->ProcessMonitor();
    this->parent->ReleaseNonActiveWorkers();
  } else if (this->requestType==this->requestUnknown)
    this->ProcessUnknown();
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
  stOutput << "</td></tr>";
  if (theGlobalVariables.flagTimeOutExplanationAlreadyDisplayed)
    stOutput << "<tr><td>Your computation is taking more than " << theGlobalVariables.MaxComputationTimeBeforeWeTakeAction
    << " seconds.</td></tr>";
  stOutput << "<tr><td>A progress indicator, as reported by your current computation, is displayed below. "
  << "When done, your computation result will be displayed below. </td></tr>";
  stOutput << "<tr><td>" << this->GetJavaScriptIndicatorBuiltInServer(this->indexInParent) << "</td></tr>"
  << "</table></body></html>";

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
    out << ", <a href=\"calculator?monitor" << this->indexInParent +1 << "\">monitor process "
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
  this->flagUsESSL=false;
  this->activeWorker=-1;
  this->timeLastExecutableModification=-1;
  this->listeningSocketHTTP=-1;
  this->listeningSocketHttpSSL=-1;
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

void WebServer::CreateNewActiveWorker()
{ MacroRegisterFunctionWithName("WebServer::CreateNewActiveWorker");
  if (this->activeWorker!=-1)
  { crash << "Calling CreateNewActiveWorker requres the active worker index to be -1." << crash;
    return;
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
  this->GetActiveWorker().Release();
  this->theWorkers[this->activeWorker].flagInUse=true;
  this->GetActiveWorker().PauseComputationReportReceived.CreateMe("server to worker computation report received");
  this->GetActiveWorker().PauseWorker.CreateMe("server to worker pause");
  this->GetActiveWorker().PauseIndicatorPipeInUse.CreateMe("server to worker indicator pipe in use");
  this->GetActiveWorker().pipeServerToWorkerRequestIndicator.CreateMe("server to worker request indicator");
  this->GetActiveWorker().pipeWorkerToServerTimerPing.CreateMe("worker to server timer ping");
  this->GetActiveWorker().pipeWorkerToServerControls.CreateMe("worker to server controls");
  this->GetActiveWorker().pipeWorkerToServerIndicatorData.CreateMe("worker to server indicator data");
  this->GetActiveWorker().pipeWorkerToServerUserInput.CreateMe("worker to server user input");
  this->GetActiveWorker().pipeWorkerToServerWorkerStatus.CreateMe("worker to server worker status");
  this->GetActiveWorker().indexInParent=this->activeWorker;
  this->GetActiveWorker().parent=this;
  this->GetActiveWorker().timeOfLastPingServerSideOnly=theGlobalVariables.GetElapsedSeconds();
  this->GetActiveWorker().pingMessage="";
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

void WebServer::CheckExecutableVersionAndRestartIfNeeded()
{ struct stat theFileStat;
  if (stat(theGlobalVariables.PhysicalNameExecutableWithPath.c_str(), &theFileStat)!=0)
    return;
  theLog << "current process spawned from file with time stamp: " << this->timeLastExecutableModification
  << "; latest executable time stamp: " << theFileStat.st_ctime << logger::endL;
  if (this->timeLastExecutableModification!=-1)
    if (this->timeLastExecutableModification!=theFileStat.st_ctime)
    { stOutput << "<b>The server executable was updated, but the server has not been restarted yet. "
      << "Restarting in 0.5 seconds...</b>";
      stOutput << "<script language=\"javascript\">setTimeout(resubmit, 500); "
      << " function resubmit() { document.getElementById('formCalculator').submit();}</script>";
      if (this->activeWorker!=-1)
      { this->GetActiveWorker().SendAllBytes();
        this->ReleaseActiveWorker();
      }
      theLog << logger::red << "Time stamps are different, RESTARTING." << logger::endL;
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
  int timeInteger=(int) theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit;
  theCommand << "killall " <<  theGlobalVariables.PhysicalNameExecutableNoPath + " \r\n./";
  theCommand << theGlobalVariables.PhysicalNameExecutableNoPath;
  if (this->flagUsESSL)
    theCommand << " serverSSL nokill " << timeInteger;
  else if (this->flagPort8155)
    theCommand << " server8155 nokill " << timeInteger;
  else
    theCommand << " server nokill " << timeInteger;
  system(theCommand.str().c_str()); //kill any other running copies of the calculator.
}

void WebServer::initPortsITry()
{ if (this->flagPort8155)
    this->PortsITryHttp.AddOnTop("8155");
  this->PortsITryHttp.AddOnTop("8080");
  this->PortsITryHttp.AddOnTop("8081");
  this->PortsITryHttp.AddOnTop("8082");
  this->PortsITryHttp.AddOnTop("8155");
  if (!this->flagUsESSL)
    return;
  this->PortsITryHttpSSL.AddOnTop("8083");
  this->PortsITryHttpSSL.AddOnTop("8084");
  this->PortsITryHttpSSL.AddOnTop("8085");
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
{
  crash << "Caught segfault at address: " << si->si_addr << crash;
  exit(0);
}

void fperror_sigaction(int signal)
{
  crash << "Fatal arithmetic error. " << crash;
  exit(0);
}

void WebServer::RecycleChildrenIfPossible()
{ //Listen for children who have exited properly.
  //This might need to be rewritten: I wasn't able to make this work with any
  //mechanism other than pipes.
  for (int i=0; i<this->theWorkers.size; i++)
    if (this->theWorkers[i].flagInUse)
    { this->theWorkers[i].pipeWorkerToServerControls.Read();
      if (this->theWorkers[i].pipeWorkerToServerControls.lastRead.size>0)
      { this->theWorkers[i].flagInUse=false;
        theLog << logger::green << "worker " << i+1 << " done, marking for reuse." << logger::endL;
      } else
        theLog << logger::red << "worker " << i+1 << " not done yet." << logger::endL;
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
      } if (this->theWorkers[i].PauseWorker.CheckPauseIsRequested())
      { this->theWorkers[i].pingMessage="worker paused, no pings.";
        this->theWorkers[i].timeOfLastPingServerSideOnly=theGlobalVariables.GetElapsedSeconds();
      } else
        if (theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit>0 &&
            theGlobalVariables.GetElapsedSeconds()-this->theWorkers[i].timeOfLastPingServerSideOnly>
            theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit
            )
        { this->theWorkers[i].flagInUse=false;
          std::stringstream pingTimeoutStream;
          pingTimeoutStream << theGlobalVariables.GetElapsedSeconds()-this->theWorkers[i].timeOfLastPingServerSideOnly
          << " seconds have passed since worker " << i+1
          << " pinged the server. I am assuming the worker no longer functions, and am marking it as free for reuse. "
          << "In addition, I am killing the worker's last executable pid: "
          << this->theWorkers[i].ProcessPID << ". ";
          kill(this->theWorkers[i].ProcessPID, SIGKILL);
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
  int* theListeningSocket=& this->listeningSocketHTTP;
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
          crash << "Error: setsockopt failed.\n" << crash;
        if (bind(*theListeningSocket, p->ai_addr, p->ai_addrlen) == -1)
        { close(*theListeningSocket);
          *theListeningSocket=-1;
          theLog << "Error: bind failed. " << this->ToStringLastErrorDescription() << logger::endL;
          continue;
        }
        break;
      }
      if (p!=NULL)
      { theLog << logger::yellow << "Successfully bounded to port " << (*thePorts)[i] << logger::endL;
        break;
      }
      freeaddrinfo(servinfo); // all done with this structure
    }
  if (this->listeningSocketHTTP == -1)
    crash << "Failed to bind to any of the ports " << this->PortsITryHttp.ToStringCommaDelimited() << "\n" << crash;
  if (this->flagUsESSL)
    if (this->listeningSocketHttpSSL==-1)
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
  sa.sa_handler = &WebServer::Signal_SIGCHLD_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
    theLog << "sigaction returned -1" << logger::endL;
}

int WebServer::Run()
{ MacroRegisterFunctionWithName("WebServer::Run");
  if (!this->initPrepareWebServerALL())
    return 1;
  List<int> theSockets;
  int highestSockNum=-1;
  if (listen(this->listeningSocketHTTP, WebServer::maxNumPendingConnections) == -1)
    crash << "Listen function failed on http port." << crash;
  if (this->flagUsESSL)
    if (listen(this->listeningSocketHttpSSL, WebServer::maxNumPendingConnections) == -1)
      crash << "Listen function failed on https port." << crash;

  theLog << logger::purple <<  "server: waiting for connections...\r\n" << logger::endL;
  unsigned int connectionsSoFar=0;
  sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  char userAddressBuffer[INET6_ADDRSTRLEN];
  this->initSSL();
  while(true)
  { // main accept() loop
    sin_size = sizeof their_addr;
//    theLog << logger::red << "select returned!" << logger::endL;
    int theListeningSocket=-1;
    fd_set FDListenSockets;
    FD_ZERO(&FDListenSockets);
    if (this->listeningSocketHTTP!=-1)
    { theSockets.AddOnTop(this->listeningSocketHTTP);
      highestSockNum=MathRoutines::Maximum(this->listeningSocketHTTP, highestSockNum);
      FD_SET(this->listeningSocketHTTP, &FDListenSockets);
    }
  //  this->listeningSocketHttpSSL=-1;
    if (this->listeningSocketHttpSSL!=-1)
    { theSockets.AddOnTop(this->listeningSocketHttpSSL);
      highestSockNum=MathRoutines::Maximum(this->listeningSocketHttpSSL, highestSockNum);
      FD_SET(this->listeningSocketHttpSSL, &FDListenSockets);
    }
    select(highestSockNum+1, & FDListenSockets, 0, 0, 0);
    for (int i=0; i< theSockets.size; i++)
      if (FD_ISSET(theSockets[i], &FDListenSockets))
      { theListeningSocket=theSockets[i];
        break;
      }
    if (theListeningSocket==-1)
      crash << "Error with listening socket: not supposed to happen... " << crash;
    int newConnectedSocket = accept(theListeningSocket, (struct sockaddr *)&their_addr, &sin_size);
    if (newConnectedSocket <0)
    { logIO << logger::red << "Accept failed. Error: " << this->ToStringLastErrorDescription() << logger::endL;
      continue;
    }
//    theLog << logger::purple << "NewconnectedSocket: " << newConnectedSocket << ", listeningSocket: "
//    << theListeningSocket << logger::endL;
    this->RecycleChildrenIfPossible();
    this->CreateNewActiveWorker();
    this->GetActiveWorker().flagUsingSSLinCurrentConnection=false;
    if (theListeningSocket==this->listeningSocketHttpSSL)
    { this->GetActiveWorker().flagUsingSSLinCurrentConnection=true;
      //std::cout << "\noutput ssl:\n";
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
    if (this->GetActiveWorker().ProcessPID!=0)
    { // this is the child (worker) process
      theGlobalVariables.flagUsingHttpSSL=this->GetActiveWorker().flagUsingSSLinCurrentConnection;
      this->SSLServerSideHandShake();
      this->Release(theListeningSocket);//worker has no access to socket listener
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
        << this->GetActiveWorker().ToStringMessageFull()
        << "</body></html>";
        stOutput.Flush();
        this->GetActiveWorker().SendAllBytes();
        return -1;
      }
      this->GetActiveWorker().SendDisplayUserInputToServer();
     // std::cout << "Got thus far 9" << std::endl;
      int result= this->GetActiveWorker().ServeClient();
      this->ReleaseEverything();
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
  return WebWorker::OutputWeb();
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
  theGlobalVariables.flagUsingBuiltInWebServer= (secondArgument=="server" || secondArgument=="server8155" || secondArgument=="serverSSL");
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
  if (secondArgument=="serverSSL")
    theWebServer.flagUsESSL=true;
  if (argC<3)
    return;
  std::string& thirdArgument=theGlobalVariables.programArguments[2];
  theWebServer.flagTryToKillOlderProcesses=!(thirdArgument=="nokill");
  std::string timeLimitString="100";
  if (thirdArgument=="nokill")
    if (argC>3)
      timeLimitString=theGlobalVariables.programArguments[3];
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
