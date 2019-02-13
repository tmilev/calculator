//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfHeader1General5TimeDate.h"
#include "webserver.h"
#include "vpfHeader5Crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
//#include <libexplain/connect.h>

#include "vpfHeader7DatabaseInterface.h"

extern WebServer theWebServer;
ProjectInformationInstance ProjectInfoVpf6_5calculatorWebRoutines(__FILE__, "Calculator web routines. ");

class WebCrawler
{
public:
//  SSLdata theSSlData;
  int theSocket;
  std::string portOrService;
  std::string addressToConnectTo;
  std::string serverToConnectTo;
  std::string postMessageToSend;
  std::string lastTransactionErrors;
  std::string lastTransaction;
  std::string headerReceived;
  std::string bodyReceiveD;
  std::string bodyReceivedWithHeader;
  std::string bodyReceivedOutsideOfExpectedLength;

  bool flagInitialized;
  bool flagContinueWasNeeded;
  bool flagDoUseGET;
  LargeInt expectedLength;
  List<char> buffer;
  struct sockaddr_in serverAddress;
  struct hostent *serverOtherSide;
  struct addrinfo hints;
  struct addrinfo *serverInfo;
  WebCrawler();
  ~WebCrawler();
  void UpdatePublicKeys(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  void FetchWebPagePart2(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  void init();
  void PingCalculatorStatus();
  void FreeAddressInfo();
  void FetchWebPage(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  bool VerifyRecaptcha
  (std::stringstream* commentsOnFailure,
   std::stringstream* commentsGeneralNONsensitive,
   std::stringstream* commentsGeneralSensitive);
};

class WebServerMonitor
{
public:
  void BackupDatabaseIfNeeded();
  double timeAtLastBackup;
  void Monitor();
  WebServerMonitor();
};

void MonitorWebServer()
{ WebServerMonitor theMonitor;
  theMonitor.Monitor();
}

WebServerMonitor::WebServerMonitor()
{ this->timeAtLastBackup = - 1;

}

void WebServerMonitor::BackupDatabaseIfNeeded()
{ MacroRegisterFunctionWithName("WebServer::BackupDatabaseIfNeeded");
  if (this->timeAtLastBackup > 0 &&
      theGlobalVariables.GetElapsedSeconds() - this->timeAtLastBackup < (24 * 3600))
    return;
  std::stringstream commandStream;
  commandStream << "mongodump --db calculator --archive ="
  << theGlobalVariables.PhysicalPathProjectBase
  << "database-backups/dbBackup"
  << theGlobalVariables.GetDateForLogFiles() << ".mongo";
  logServerMonitor << logger::orange << "Backing up database with command: " << logger::endL;
  logServerMonitor << commandStream.str() << logger::endL;
  theGlobalVariables.CallSystemWithOutput(commandStream.str());
  logServerMonitor << logger::green << "Backing up completed. " << logger::endL;
  this->timeAtLastBackup = theGlobalVariables.GetElapsedSeconds();
}

void WebServerMonitor::Monitor()
{ MacroRegisterFunctionWithName("Monitor");
  int maxNumPingFailures = 3;
  //warning: setting theWebServer.WebServerPingIntervalInSeconds to more than 1000
  //may overflow the variable int microsecondsToSleep.
  //theWebServer.WebServerPingIntervalInSeconds=1000;
  int microsecondsToSleep = 1000000 * theWebServer.WebServerPingIntervalInSeconds;//
  if (theWebServer.flagTryToKillOlderProcesses)
    for (;;)
      theGlobalVariables.FallAsleep(microsecondsToSleep);
  if (theWebServer.WebServerPingIntervalInSeconds > 30)
    logServerMonitor << logger::red << "**********WARNING**************"
    << logger::endL
    << logger::red << " The ping interval: "
    << theWebServer.WebServerPingIntervalInSeconds
    << " is set to a large value. "
    << "Set the ping interval to less than 30 seconds to remove this message. " << logger::endL;
  std::fstream theFile;
  FileOperations::OpenFileCreateIfNotPresentVirtual
  (theFile,"/LogFiles/server_starts_and_unexpected_restarts.html", true, false, false, true);
  theFile << "<a href=\"/LogFiles/" << GlobalVariables::GetDateForLogFiles() << "/\">"
  << GlobalVariables::GetDateForLogFiles() << "</a>" << "<br>\n";
  theFile.close();
  WebCrawler theCrawler;
  theCrawler.init();
  logServerMonitor << logger::blue << "Pinging " << theCrawler.addressToConnectTo << " at port/service "
  << theCrawler.portOrService << " every " << (microsecondsToSleep / 1000000) << " second(s). "
  << logger::endL;
  logServerMonitor << logger::red << "Please beware that the server will restart and you will lose all computations "
  << "if " << maxNumPingFailures << " consecutive pings fail. " << logger::endL;
  int numConsecutiveFailedPings = 0;
  int numPings = 0;
  TimeWrapper now;
  for (;;)
  { theGlobalVariables.FallAsleep(microsecondsToSleep);
    this->BackupDatabaseIfNeeded();
    //std::cout << "Pinging " << theCrawler.addressToConnectTo
    //<< " at port/service " << theCrawler.portOrService << ".\n";
    theCrawler.PingCalculatorStatus();
    numPings ++;
    if (theCrawler.lastTransactionErrors != "")
    { now.AssignLocalTime();
      numConsecutiveFailedPings ++;
      logServerMonitor << logger::red << "Connection monitor: Ping of " << theCrawler.addressToConnectTo
      << " at port/service " << theCrawler.portOrService
      << " failed. GM time: " << now.ToStringGM() << ", local time: " << now.ToStringLocal()
      << ". " << "Got the following errors/messages: "
      << theCrawler.lastTransactionErrors << theCrawler.lastTransaction << ". "
      << numConsecutiveFailedPings << " consecutive fails so far, restarting on " << maxNumPingFailures
      << ". " << logger::endL;
    } else
    { std::cout << "Connection monitor: Ping success #" << numPings << std::endl;
      numConsecutiveFailedPings = 0;
    }
    if (numConsecutiveFailedPings >= maxNumPingFailures)
    { now.AssignLocalTime();
      logServerMonitor << logger::red << "Server stopped responding (probably locked pipe?)"
      << ", restarting. " << logger::endL;
      FileOperations::OpenFileCreateIfNotPresentVirtual
      (theFile, "LogFiles/server_starts_and_unexpected_restarts.html", true, false, false, true);
      theFile << "<b style ='color:red'>Unexpected server restart: server stopped responding (locked pipe?). Time: local: "
      << now.ToStringLocal() << ", GM: " << now.ToStringGM() << "</b><br>\n";
      theFile.flush();
      theWebServer.Restart();
    }
  }
}

WebCrawler::WebCrawler()
{ this->flagInitialized = false;
  this->theSocket = - 1;
  this->serverOtherSide = 0;
  this->serverInfo = 0;
}

void WebCrawler::FreeAddressInfo()
{ if (this->serverInfo != 0)
    freeaddrinfo(this->serverInfo);
  this->serverInfo = 0;
}

void WebCrawler::init()
{ if (this->flagInitialized)
    return;
  this->flagContinueWasNeeded = false;
  MacroRegisterFunctionWithName("WebCrawler::init");
  this->flagDoUseGET = true;
  buffer.initFillInObject(50000, 0);
  if (!theWebServer.flagPort8155)
  { this->portOrService = "8080";
//    this->addressToConnectTo ="localhost";
    this->addressToConnectTo = "127.0.0.1";
  } else
  { this->portOrService = "8155";
    this->addressToConnectTo = "127.0.0.1";
  }
}

WebCrawler::~WebCrawler()
{ if (!this->flagInitialized)
    return;
  this->flagInitialized = false;
  close(this->theSocket);
  this->theSocket = - 1;
  this->FreeAddressInfo();
}

void WebCrawler::PingCalculatorStatus()
{ MacroRegisterFunctionWithName("WebCrawler::PingCalculatorStatus");
  std::stringstream reportStream;
  this->lastTransaction = "";
  this->lastTransactionErrors = "";
  memset(&this->hints, 0, sizeof this->hints); // make sure the struct is empty
  this->hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
  this->hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
  this->hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
  this->serverInfo = 0;
  int status = getaddrinfo(this->addressToConnectTo.c_str(), this->portOrService.c_str(), &hints, &this->serverInfo);
  if (status != 0)
  { this->lastTransactionErrors =  "Could not find address: getaddrinfo error: ";
    this->lastTransactionErrors += gai_strerror(status);
    this->FreeAddressInfo();
    return;
  }
  if (this->serverInfo == 0)
  { this->lastTransactionErrors = "Server info is zero";
    return;
  }
  struct addrinfo *p = 0;  // will point to the results
  this->theSocket = - 1;
  char ipString[INET6_ADDRSTRLEN];
  for (p = this->serverInfo; p != 0; p = p->ai_next, close(this->theSocket))
  { void *theAddress = 0;
    this->theSocket = - 1;
    // get the pointer to the address itself,
    // different fields in IPv4 and IPv6:
    if (p->ai_family == AF_INET)
    { // IPv4
      struct sockaddr_in *ipv4 = (struct sockaddr_in *) p->ai_addr;
      theAddress = &(ipv4->sin_addr);
      reportStream << "IPv4: ";
    } else
    { // IPv6
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) p->ai_addr;
      theAddress = &(ipv6->sin6_addr);
      reportStream << "IPv6: ";
    }
    // convert the IP to a string and print it:
    inet_ntop(p->ai_family, theAddress, ipString, sizeof ipString);
//    std::string ipString()
    reportStream << this->addressToConnectTo << ": " << ipString << "<br>";
    this->theSocket = socket(this->serverInfo->ai_family, this->serverInfo->ai_socktype, this->serverInfo->ai_protocol);
    int connectionResult = - 1;
    if (this->theSocket < 0)
    { this->lastTransactionErrors = "failed to create socket ";
      continue;
    } else
    { fd_set fdConnectSockets;
      FD_ZERO(&fdConnectSockets);
      FD_SET(this->theSocket, &fdConnectSockets);
      timeval timeOut;
      timeOut.tv_sec = 1;
      timeOut.tv_usec = 0;
      int numPingFails = 0;
      int numSelected = 0;
      std::stringstream failStream;
      do
      { if (numPingFails > 10)
          break;
        numSelected = select(this->theSocket + 1, &fdConnectSockets, 0, 0, &timeOut);
        failStream << "While pinging, select failed. Error message: "
        << strerror(errno) << ". \n";
        numPingFails ++;
      } while (numSelected < 0);
      if (numSelected <= 0)
      { logIO << logger::red << failStream.str() << logger::endL;
        reportStream << failStream.str() << "Could not connect through port. Select returned: " << numSelected;
        connectionResult = - 1;
      } else
        connectionResult = connect(this->theSocket, this->serverInfo->ai_addr, this->serverInfo->ai_addrlen);
    }
    if (connectionResult == - 1)
    { reportStream << "<br>Failed to connect: address: " << this->addressToConnectTo << " port: "
      << this->portOrService << ". ";
//      << explain_errno_connect(this->theSocket, this->serverInfo->ai_addr, this->serverInfo->ai_addrlen);
      this->lastTransactionErrors = reportStream.str();
      close(this->theSocket);
      continue;
    } else
      reportStream << "<br>connected: " << this->addressToConnectTo << " port: " << this->portOrService << ". ";
    std::string getMessage = "GET /cgi-bin/calculator?request=statusPublic";
    std::stringstream errorStream1;
    int numBytes = Pipe::WriteWithTimeoutViaSelect(this->theSocket,getMessage, 1, 10, &errorStream1);
    if ((unsigned) numBytes != getMessage.size())
    { this->lastTransactionErrors += "\nERROR writing to socket. "+ errorStream1.str();
      close(this->theSocket);
      continue;
    }
    std::stringstream errorStream2;
    numBytes = Pipe::ReadWithTimeOutViaSelect(this->theSocket, this->buffer, 1, 10, &errorStream2);
    if (numBytes < 0)
    { this->lastTransactionErrors += "\nERROR reading from socket. " + errorStream2.str();
      close(this->theSocket);
      continue;
    }
    std::string readString;
    readString.assign(buffer.TheObjects, numBytes);
    reportStream << "<br>Read: " << readString;
    this->lastTransaction = reportStream.str();
    close(this->theSocket);
  }
  this->FreeAddressInfo();
}

void WebCrawler::FetchWebPage
(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral)
{ MacroRegisterFunctionWithName("WebCrawler::FetchWebPage");
  (void) commentsOnFailure; (void) commentsGeneral;
#ifdef MACRO_use_open_ssl
  //logOpenSSL << logger::green  << "DEBUG: got to FetchWebPage start. " << logger::endL;
  this->lastTransaction = "";
  this->lastTransactionErrors = "";
  memset(&this->hints, 0, sizeof this->hints); // make sure the struct is empty
  this->hints.ai_family   = AF_UNSPEC  ;       // don't care IPv4 or IPv6
  this->hints.ai_socktype = SOCK_STREAM;       // TCP stream sockets
  this->hints.ai_flags    = AI_PASSIVE ;       // fill in my IP for me
  this->serverInfo = 0;
  int status = getaddrinfo(this->serverToConnectTo.c_str(), this->portOrService.c_str(), &hints, &this->serverInfo);
  if (status != 0)
  { this->lastTransactionErrors =  "Error calling getaddrinfo: ";
    this->lastTransactionErrors += gai_strerror(status);
    if (commentsOnFailure != 0)
      *commentsOnFailure << this->lastTransactionErrors << ". "
      << "Server: " << this->serverToConnectTo << ", port or service: " << this->portOrService << ". ";
    this->FreeAddressInfo();
    return;
  }
  if (this->serverInfo == 0)
  { this->lastTransactionErrors = "Server info is zero";
    return;
  }
  struct addrinfo *p = 0;// will point to the results
  this->theSocket = - 1;
  char ipString[INET6_ADDRSTRLEN];
  timeval timeOut;
  for (p = this->serverInfo; p != 0; p = p->ai_next, close(this->theSocket))
  { void *theAddress = 0;
    this->theSocket = - 1;
    // get the pointer to the address itself,
    // different fields in IPv4 and IPv6:
    if (p->ai_family == AF_INET)
    { // IPv4
      struct sockaddr_in *ipv4 = (struct sockaddr_in *) p->ai_addr;
      theAddress = &(ipv4->sin_addr);
      if (commentsGeneral != 0)
        *commentsGeneral << "IPv4: ";
    } else
    { // IPv6
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) p->ai_addr;
      theAddress = &(ipv6->sin6_addr);
      if (commentsGeneral!= 0)
        *commentsGeneral << "IPv6: ";
    }
    // convert the IP to a string and print it:
    inet_ntop(p->ai_family, theAddress, ipString, sizeof ipString);
//    std::string ipString()
    if (commentsGeneral != 0)
      *commentsGeneral << this->addressToConnectTo << ": " << ipString << "<br>";
    this->theSocket = socket(this->serverInfo->ai_family, this->serverInfo->ai_socktype, this->serverInfo->ai_protocol);
    int connectionResult = - 1;
    if (this->theSocket < 0)
    { this->lastTransactionErrors = "failed to create socket ";
      continue;
    } else
    { fd_set fdConnectSockets;
      FD_ZERO(&fdConnectSockets);
      FD_SET(this->theSocket, &fdConnectSockets);
      timeOut.tv_sec = 1;
      timeOut.tv_usec = 0;
      int numPingFails = 0;
      int numSelected = 0;
      std::stringstream failStream;
      do
      { if (numPingFails > 10)
          break;
        numSelected = select(this->theSocket + 1, &fdConnectSockets, 0, 0, &timeOut);
        failStream << "While pinging, select failed. Error message: "
        << strerror(errno) << ". \n";
        numPingFails ++;
      } while (numSelected < 0);
      if (numSelected <= 0)
      { logIO << logger::red << failStream.str() << logger::endL;
        if (commentsOnFailure != 0)
          *commentsOnFailure << failStream.str()
          << "Could not connect through port. Select returned: " << numSelected;
        connectionResult = - 1;
      } else
        connectionResult = connect(this->theSocket, this->serverInfo->ai_addr, this->serverInfo->ai_addrlen);
      timeOut.tv_sec = 3;  // 5 Secs Timeout
      timeOut.tv_usec = 0;  // Not init'ing this can cause strange errors
      setsockopt(connectionResult, SOL_SOCKET, SO_RCVTIMEO, (void*)(&timeOut), sizeof(timeval));
    }
    if (connectionResult == - 1)
    { std::stringstream errorStream;
      errorStream
      << "Failed to connect: address: " << this->addressToConnectTo << " port: "
      << this->portOrService << ".<br>";
      if (commentsOnFailure != 0)
        *commentsOnFailure << errorStream.str();
      this->lastTransactionErrors = errorStream.str();
      close(this->theSocket);
      continue;
    } else
    { if (commentsGeneral != 0)
        *commentsGeneral << "connected: "
        << this->addressToConnectTo << " port: " << this->portOrService << ". <hr>";
    }
    this->FetchWebPagePart2(commentsOnFailure, commentsGeneral);
    theWebServer.theSSLdata.RemoveLastSocketClient();
    close(this->theSocket);
    break;
  }
  this->FreeAddressInfo();
#endif //Macro_use_openssl
}

void WebCrawler::FetchWebPagePart2
(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral)
{ MacroRegisterFunctionWithName("WebCrawler::FetchWebPagePart2");
  (void) commentsOnFailure;
  (void) commentsGeneral;
#ifdef MACRO_use_open_ssl
  std::stringstream theMessageHeader, theContinueHeader;
  if (this->flagDoUseGET)
  { theMessageHeader << "GET " << this->addressToConnectTo << " HTTP/1.0"
    << "\r\n" << "Host: " << this->serverToConnectTo << "\r\n\r\n";
  } else
  { theMessageHeader << "POST " << this->addressToConnectTo << " HTTP/1.0"
    << "\r\n" << "Host: " << this->serverToConnectTo;
    theMessageHeader << "\r\nContent-length: " << this->postMessageToSend.size();
    theMessageHeader << "\r\n\r\n";
    theMessageHeader << this->postMessageToSend;
  }

  if (!theWebServer.theSSLdata.HandShakeIamClientNoSocketCleanup
       (this->theSocket, commentsOnFailure, commentsGeneral))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Could not shake hands. ";
    return;
  }
  this->headerReceived = "";
  this->bodyReceiveD = "";
  this->flagContinueWasNeeded = false;
  if (commentsGeneral != 0)
    *commentsGeneral << "<hr>";
  std::string errorSSL;
  if (!theWebServer.theSSLdata.SSLWriteLoop
      (10, theWebServer.theSSLdata.sslClient, theMessageHeader.str(),
       &errorSSL, commentsGeneral, true))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "SSL critical error: " << errorSSL;
    return;
  }
  if (!theWebServer.theSSLdata.SSLReadLoop
      (10, theWebServer.theSSLdata.sslClient, this->headerReceived, 0,
       &errorSSL, commentsGeneral, true))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "SSL critical error: " << errorSSL;
    return;
  }
  unsigned bodyStart = 0;
  int numcrlfs = 0;
  //std::stringstream tempStream;
  for (; bodyStart < this->headerReceived.size(); bodyStart ++)
  { if (numcrlfs >= 4)
      break;
    if (this->headerReceived[bodyStart] == '\n' ||
        this->headerReceived[bodyStart] == '\r')
    { numcrlfs ++;
      //if (fullMessage[bodyStart] == '\n')
      //  tempStream << "\\N";
      //else
      //  tempStream << "\\R";
    } else
    { numcrlfs = 0;
      //tempStream << fullMessage[bodyStart];
    }
  }
  if (bodyStart < this->headerReceived.size())
  { this->bodyReceivedWithHeader = this->headerReceived.substr(bodyStart);
    this->headerReceived = this->headerReceived.substr(0, bodyStart);
  }
  List<std::string> theHeaderPieces;
  MathRoutines::StringSplitDefaultDelimiters(this->headerReceived, theHeaderPieces);
  std::string expectedLengthString;
  for (int i = 0; i < theHeaderPieces.size; i ++)
    if (theHeaderPieces[i] == "Content-length:" ||
        theHeaderPieces[i] == "Content-Length:" ||
        theHeaderPieces[i] == "content-length:")
      if (i + 1 < theHeaderPieces.size)
      { expectedLengthString=theHeaderPieces[i + 1];
        break;
      }
  this->expectedLength = - 1;
  if (expectedLengthString != "")
    expectedLength.AssignString(expectedLengthString);
  if (commentsGeneral != 0)
    *commentsGeneral << "<br>Expected length: " << this->expectedLength;
  if (commentsGeneral != 0)
    *commentsGeneral << "<br>Header:<br>" << this->headerReceived;
  if (this->headerReceived.find("200 OK") == std::string::npos)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "No 200 ok message found. Headers received: " << this->headerReceived;
    return;
  }
  if (expectedLength == this->bodyReceivedWithHeader.size())
  { this->bodyReceiveD = this->bodyReceivedWithHeader;
    return;
  }
  this->flagContinueWasNeeded = true;
  //if (this->bodyReceivedWithHeader == "")
  //{
  theContinueHeader << "HTTP/1.0 100 Continue\r\n\r\n";
  //theContinueHeader << "\r\n\r\n";
  if (!theWebServer.theSSLdata.SSLWriteLoop
      (10, theWebServer.theSSLdata.sslClient, theContinueHeader.str(),
       &errorSSL, commentsGeneral, true))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "SSL critical error: " << errorSSL;
    return;
  }
  std::string secondPart;
  if (!theWebServer.theSSLdata.SSLReadLoop
       (10, theWebServer.theSSLdata.sslClient, secondPart, expectedLength,
        &errorSSL, commentsGeneral, true))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "SSL critical error: " << errorSSL;
    return;
  }
  if (commentsGeneral != 0)
    *commentsGeneral << "<br>Second part length: "
    << secondPart.size();
  this->bodyReceiveD = this->bodyReceivedWithHeader + secondPart;
  int theSize = 0;
  this->expectedLength.IsIntegerFittingInInt(&theSize);
  if ((unsigned) theSize < this->bodyReceiveD.size())
  { this->bodyReceivedOutsideOfExpectedLength = this->bodyReceiveD.substr(theSize);
    this->bodyReceiveD = this->bodyReceiveD.substr(0, theSize);
  }
  //if (!theWebServer.theSSLdata.SSLreadLoop
  //    (10,theWebServer.theSSLdata.sslClient, secondPart, comments, comments, true))
  //  return;
  //this->bodyReceived+=secondPart;
  //stOutput << tempStream.str();
  if (commentsGeneral != 0)
  { *commentsGeneral << "<br>Body (length: "
    << this->bodyReceiveD.size()
    << ")<br>" << this->bodyReceiveD;
    if (this->bodyReceivedOutsideOfExpectedLength.size() == 0)
      *commentsGeneral << "<br><span style =\"color:green\"><b>No extraneous data received</b></span>";
    else
      *commentsGeneral<< "<br><span style =\"color:red\"><b>Received more data than expected "
      << "(perhaps due to a protocol error?).</b></span>"
      << "<br>" << this->bodyReceivedOutsideOfExpectedLength;
  }
#endif
}

bool CalculatorFunctionsGeneral::innerFetchWebPageGET(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerFetchWebPageGET");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return output.AssignValue((std::string) "Fetching web pages available only for logged-in admins. ", theCommands);
  WebCrawler theCrawler;
  if (input.size() != 4)
    return theCommands << "Fetching web page expects 3 arguments: server, service/port, and webpage. ";
  if (!input[1].IsOfType(&theCrawler.serverToConnectTo))
    theCrawler.serverToConnectTo = input[1].ToString();
  if (!input[2].IsOfType(&theCrawler.portOrService))
    theCrawler.portOrService = input[2].ToString();
  if (!input[3].IsOfType(&theCrawler.addressToConnectTo))
    theCrawler.addressToConnectTo = input[3].ToString();
  std::stringstream out;
  theCrawler.flagDoUseGET = true;
  out
  << "Server:  " << theCrawler.serverToConnectTo
  << " port: " << theCrawler.portOrService
  << " resource: " << theCrawler.addressToConnectTo
  << "<br>";
  theCrawler.FetchWebPage(&out, &out);
  out << "<br>" << theCrawler.lastTransactionErrors << "<hr>" << theCrawler.lastTransaction;
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerFetchWebPagePOST(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerFetchWebPagePOST");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return output.AssignValue((std::string) "Fetching web pages available only for logged-in admins. ", theCommands);
  WebCrawler theCrawler;
  if (input.size() != 5)
    return theCommands << "Fetching web page expects 4 arguments: server, service/port, webpage and message to post. ";
  if (!input[1].IsOfType(&theCrawler.serverToConnectTo))
    theCrawler.serverToConnectTo = input[1].ToString();
  if (!input[2].IsOfType(&theCrawler.portOrService))
    theCrawler.portOrService = input[2].ToString();
  if (!input[3].IsOfType(&theCrawler.addressToConnectTo))
    theCrawler.addressToConnectTo = input[3].ToString();
  if (!input[4].IsOfType(&theCrawler.postMessageToSend))
    theCrawler.postMessageToSend = input[4].ToString();
  std::stringstream out;
  out
  << "Server:  " << theCrawler.serverToConnectTo
  << " port: " << theCrawler.portOrService
  << " resource: " << theCrawler.addressToConnectTo
  << "<br>";
  theCrawler.flagDoUseGET = false;
  theCrawler.FetchWebPage(&out, &out);
  out << "<br>" << theCrawler.lastTransactionErrors << "<hr>" << theCrawler.lastTransaction;
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerFetchKnownPublicKeys(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerFetchKnownPublicKeys");
  (void) input;
  std::stringstream out;
  if (!theGlobalVariables.UserDefaultHasAdminRights())
  { out << "You need to be a logged-in admin to call this function. ";
    return output.AssignValue(out.str(), theCommands);
  }
  WebCrawler theCrawler;
  theCrawler.UpdatePublicKeys(&out, &out);
  return output.AssignValue(out.str(), theCommands);
}

void WebCrawler::UpdatePublicKeys(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral)
{ MacroRegisterFunctionWithName("WebCrawler::UpdatePublicKeys");
  this->serverToConnectTo  = "www.googleapis.com";
  this->portOrService      = "https";
  this->addressToConnectTo = "https://www.googleapis.com/oauth2/v3/certs";
  this->flagDoUseGET = true;
  if (commentsGeneral != 0)
    *commentsGeneral << "<hr>"
    << "Updating public keys <hr>";
  this->FetchWebPage(commentsOnFailure, commentsGeneral);
  if (this->bodyReceiveD == "")
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Could not fetch google certificate list. ";
    return;
  }
  std::string googleKeysFileName = "certificates-public/google.txt";
  std::string googleKeysDebugFileName = "certificates-public/debug-google.txt";
  std::fstream googleKeysFile, googleKeysDebugFile;
  if (!FileOperations::OpenFileCreateIfNotPresentVirtual
       (googleKeysFile, googleKeysFileName, false, true, false))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "<br>Failed to open: " << googleKeysFileName;
    return;
  }
  FileOperations::OpenFileCreateIfNotPresentVirtual
  (googleKeysDebugFile, googleKeysDebugFileName, false, true, false);
  if (commentsGeneral != 0)
    *commentsGeneral << "<br>Updated file: " << googleKeysFileName;
  googleKeysFile << this->bodyReceiveD;
  googleKeysDebugFile
  << "Expected body length: " << this->expectedLength.ToString() << "\n";
  if (this->flagContinueWasNeeded)
    googleKeysDebugFile << "Did send a continue message.\n";
  else
    googleKeysDebugFile << "Did NOT send a continue message.\n";
  googleKeysDebugFile << "\nBody received with header, "
  << "length: " << this->bodyReceivedWithHeader.size()
  << ":\n"
  << this->bodyReceivedWithHeader
  << "\nBody received, after header "
  << "length: " << this->bodyReceiveD.size()
  << ":\n"
  << this->bodyReceiveD
  << "\nBody received, beyond expected length: "
  << this->bodyReceivedOutsideOfExpectedLength
  << "\n"
  << "\nHeader:\n "
  << this->headerReceived;
  googleKeysFile.flush();
}

bool Crypto::VerifyJWTagainstKnownKeys
(const std::string& inputToken, std::stringstream* commentsOnFailure,
 std::stringstream* commentsGeneral)
{ MacroRegisterFunctionWithName("Crypto::VerifyJWTagainstKnownKeys");
  //This function is slightly insecure.
  //If an attacker hijacks a machine connecting the server to the outside
  //and impersonates google
  //the attacker would be able to impersonate another user.
  //Since we are reloading the google keys for
  //every JWT key we fail to find in our cache,
  //this could happen more easily
  //than if we were storing the google keys until their expiry date.
  //To solve this problem, we need to
  //ensure that the google public keys are authentic.
  //For that we need to leverage the built-in web of trust of
  //the host system (pgp keys, etc.).
  //This is system dependent and a lot of work-> not doing now, but will do
  //in the future as the need arises.
  JSONWebToken theToken;
  //if (commentsGeneral!= 0)
  //  *commentsGeneral << "DEBUG: inputToken: " << inputToken;
  //stOutput << "DEBUG:Got to here, part - 1";
  if (!theToken.AssignString(inputToken, commentsOnFailure))
    return false;
  //stOutput << "DEBUG:Got to here, part -0.5";
  std::string keyIDstring = "";
  JSData header;
  if (!header.readstring(theToken.headerJSON, false))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Couldn't load JSON from the user token.";
    return false;
  }
  if (header.type == header.JSObject)
    if (header.HasKey("kid"))
      keyIDstring = header.GetValue("kid").string;
  if (keyIDstring == "")
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Couldn't find key id in the token.";
    return false;
  }
  //stOutput << "DEBUG:Got to here";
  int theIndex = - 1;
  if (commentsGeneral != 0)
    *commentsGeneral << "Seeking key: <b style =\"color:brown\">"
    << keyIDstring << "</b>. ";
  for (int i = 0; i < 2; i ++)
  { Crypto::LoadKnownCertificates(commentsOnFailure, commentsGeneral);
    //if (commentsOnFailure != 0)
    //  *commentsOnFailure << "DEBUG: got to after loading certificates.";
    for (int j = 0; j < Crypto::knownCertificates.size; j ++)
      if (keyIDstring == Crypto::knownCertificates[j].keyid)
      { theIndex = j;
        //if (commentsGeneral!= 0)
        //  *commentsGeneral << "<br>DEBUG: current keyid: "
        //  << Crypto::knownCertificates[j].keyid;
        break;
      }
    if (theIndex != - 1 || i == 1)
      break;
    if (commentsGeneral != 0 && i == 0)
      *commentsGeneral << "<br><span style =\"color:red\"><b>Couldn't find key ID: "
      << keyIDstring << " from cached certificate.</b></span>";
    if (commentsGeneral != 0 )
      *commentsGeneral << "<br>Reloading google public keys. ";
    WebCrawler theCrawler;
    theCrawler.UpdatePublicKeys(commentsOnFailure, commentsGeneral);
  }
  //stOutput << "DEBUG:Got to here, part 2";
  //if (commentsOnFailure != 0)
  //  *commentsOnFailure << "<hr>DEBUG: got to here, PART 2.";
  if (theIndex == - 1)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "<span style =\"color:red\"><b>Could not find key id: "
      << keyIDstring << "</b></span>. ";
    return false;
  }
  if (commentsGeneral != 0)
    *commentsGeneral << "<span style =\"color:green\"><b>Found key id: "
    << keyIDstring << ".</b></span>";
  Certificate& currentCert = Crypto::knownCertificates[theIndex];
  return theToken.VerifyRSA256
  (currentCert.theModuluS, currentCert.theExponenT, commentsOnFailure, commentsGeneral);
}

bool WebCrawler::VerifyRecaptcha
(std::stringstream* commentsOnFailure,
 std::stringstream* commentsGeneralNONsensitive,
 std::stringstream* commentsGeneralSensitive)
{ MacroRegisterFunctionWithName("WebCrawler::VerifyRecaptcha");
  std::stringstream messageToSendStream;
  std::string secret;
  std::stringstream notUsed;
  if (commentsOnFailure == 0)
    commentsOnFailure = &notUsed;
  if (!FileOperations::LoadFileToStringVirtual
       ("certificates/recaptcha-secret.txt", secret, true, true, commentsOnFailure))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "<span style =\"color:red\"><b>"
      << "Failed to load recaptcha secret."
      << " </b></span>";
    return false;
  }
  std::string recaptchaURLencoded = theGlobalVariables.GetWebInput("recaptchaToken");
  if (commentsGeneralSensitive != 0)
    *commentsGeneralSensitive << "Recaptcha: " << recaptchaURLencoded;
  if (recaptchaURLencoded == "")
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "<span style =\"color:red\"><b>Recaptcha appears to be missing. </b></span>";
    return false;
  }
  messageToSendStream << "response="
  << recaptchaURLencoded
  << "&"
  << "secret="
  << secret;
  this->flagDoUseGET = true;
  this->addressToConnectTo = "https://www.google.com/recaptcha/api/siteverify";
  this->addressToConnectTo += "?" + messageToSendStream.str();
  this->serverToConnectTo = "www.google.com";
  this->portOrService = "https";
  this->postMessageToSend = messageToSendStream.str();
  this->FetchWebPage(commentsOnFailure, commentsGeneralSensitive);
  std::string response = this->bodyReceiveD;
  JSData theJSparser;
  if (!theJSparser.readstring(response, false, commentsOnFailure))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "<span style =\"color:red\">"
      << "<b>" << "Failed to extract response token from captcha verification. "
      << "</b>"
      << " <br>The response string was: "
      << response
      << "</span>";
    return 0;
  }
  if (!theJSparser.HasKey("success"))
  { if (commentsOnFailure != 0)
      *commentsOnFailure
      << "<span style =\"color:red\">"
      << "<b>" << "Captcha failure: could not find key 'success'."
      << "</b>"
      << "</span>";
    return false;
  }
  JSData theSuccess;
  theSuccess = theJSparser.GetValue("success");
  if (theSuccess.type != theJSparser.JSbool || theSuccess.boolean != true)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "<br><span style =\"color:red\">"
      << "<b>" << "Could not verify your captcha solution. "
      << "</b>"
      << "The response from google was: "
      << response
      << "</span>";
    return false;
  } else
  { if (commentsGeneralNONsensitive != 0)
      *commentsGeneralNONsensitive
      << "<br><span style =\"color:green\">"
      << "<b>" << "Your recaptcha answer appears to be valid. "
      << "</b></span>\n<br>\n"
//      << "DEBUG: " << response
      ;
    if (commentsGeneralSensitive != 0)
      *commentsGeneralSensitive
      << "The response from google was: "
      << response;
  }
  return true;
}

std::string WebWorker::GetSignUpRequestResult()
{ MacroRegisterFunctionWithName("WebWorker::GetSignUpRequestResult");
  //double startTime =theGlobalVariables.GetElapsedSeconds();
  JSData result;
  std::stringstream errorStream;
#ifdef MACRO_use_MongoDB
  DatabaseRoutinesGlobalFunctions::LogoutViaDatabase();
  UserCalculator theUser;
  theUser.username = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("desiredUsername"), false);
  theUser.email = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("email"), false);
  std::stringstream generalCommentsStream;
  std::stringstream outputStream;
  generalCommentsStream
  << "<b>Please excuse our technical messages, they will be removed soon.</b>";
  WebCrawler theCrawler;
  if (!theCrawler.VerifyRecaptcha(&errorStream, &generalCommentsStream, 0))
  { result["error"] = errorStream.str();
    result["comments"] = generalCommentsStream.str();
    return result.ToString(false);
  }
  if (theUser.username == "")
  { errorStream << "Empty username not allowed. ";
    result["error"] = errorStream.str();
    result["comments"] = generalCommentsStream.str();
    return result.ToString(false);
  }
  if (!EmailRoutines::IsOKEmail(theUser.email, &generalCommentsStream))
  { errorStream << "Your email address does not appear to be valid. ";
    result["error"] = errorStream.str();
    result["comments"] = generalCommentsStream.str();
    return result.ToString(false);
  }
  if (theUser.Iexist(&generalCommentsStream))
  { errorStream
    << "Either the username ("
    << theUser.username
    << ") or the email ("
    << theUser.email
    << ") you requested is already taken.";
    result["error"] = errorStream.str();
    result["comments"] = generalCommentsStream.str();
    return result.ToString(false);
  } else
  { outputStream << "<span style =\"color:green\"><b>"
    << "Username ("
    << theUser.username
    << ") with email ("
    << theUser.email
    << ") is available. </b></span>";
  }
  if (!theUser.StoreToDB(false, &errorStream))
  { errorStream << "Failed to store error stream. ";
    result["error"] = errorStream.str();
    result["comments"] = generalCommentsStream.str();
    result["result"] = outputStream.str();
    return result.ToString(false);
  }
  std::stringstream* adminOutputStream = 0;
  if (theGlobalVariables.UserDefaultHasAdminRights())
    adminOutputStream = &generalCommentsStream;
  //int fixThis;
  //adminOutputStream = &generalCommentsStream;
  this->DoSetEmail(theUser, &errorStream, &generalCommentsStream, adminOutputStream);
  result["error"] = errorStream.str();
  result["comments"] = generalCommentsStream.str();
  result["result"] = outputStream.str();
#else
  result["error"] = "Error: database not available. ";
#endif
  return result.ToString(false);
}

int WebWorker::ProcessSignUP()
{ MacroRegisterFunctionWithName("WebWorker::ProcessSignUP");
  this->SetHeaderOKNoContentLength("");
  stOutput << this->GetSignUpRequestResult();
  return 0;
}

int WebWorker::ProcessForgotLogin()
{ MacroRegisterFunctionWithName("WebWorker::ProcessForgotLogin");
  //double startTime =theGlobalVariables.GetElapsedSeconds();
  this->SetHeaderOKNoContentLength("");
#ifdef MACRO_use_MongoDB
  std::stringstream out;
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    DatabaseRoutinesGlobalFunctions::LogoutViaDatabase();
  UserCalculator theUser;
  theUser.email = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("email"), false);
  WebCrawler theCrawler;
  out << "<br><b> "
  << "Please excuse our verbose technical messages. </b>"
  << "<br><b>We are still testing our system; "
  << "we will remove the technical garbage as soon as we are done. "
  << "</b><br>\n";
  if (!theCrawler.VerifyRecaptcha(&out, &out, 0))
  { stOutput << out.str();
    return 0;
  }

  if (!theUser.Iexist(&out))
  { out << "<br><b style =\"color:red\">"
    << "We failed to find your email: " << theUser.email << " in our records. "
    << "</b>";
    stOutput << out.str();
    return 0;
  }
  if (!theUser.LoadFromDB(&out, &out))
  { out << "<br><b style='color:red'>"
    << "Failed to fetch user info for email: " << theUser.email
    << "</b>";
    stOutput << out.str();
    return 0;
  }
  stOutput << "<b style =\"color:green\">"
  << "Your email is on record. "
  << "</b>";
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    this->DoSetEmail(theUser, &out, &out, 0);
  else
    this->DoSetEmail(theUser, &out, &out, &out);
  stOutput << out.str();
  stOutput << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds() << " second(s); "
  << theGlobalVariables.GetElapsedSeconds() << " second(s) spent creating account. ";
#else
  stOutput << "Error: database not running. ";
#endif
  return 0;
}

