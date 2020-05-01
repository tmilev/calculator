// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_inner_functions.h"
#include "general_time_date.h"
#include "webserver.h"
#include "crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "database.h"
#include "calculator_problem_storage.h"
#include "string_constants.h"

class WebCrawler {
public:
  TransportLayerSecurity theTSL;
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
  LargeInteger expectedLength;
  List<char> buffer;
  int lastNumBytesRead;
  struct sockaddr_in serverAddress;
  struct hostent *serverOtherSide;
  struct addrinfo hints;
  struct addrinfo *serverInfo;
  WebCrawler();
  ~WebCrawler();
  void UpdatePublicKeys(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  void FetchWebPagePart2(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  void initialize();
  void PingCalculatorStatus();
  void FreeAddressInfo();
  void FetchWebPage(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  bool VerifyRecaptcha(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNONsensitive,
    std::stringstream* commentsGeneralSensitive
  );
};

class WebServerMonitor {
public:
  void BackupDatabaseIfNeeded();
  double timeAtLastBackup;
  int pidServer;
  void Monitor(int pidServer);
  void Restart();
  WebServerMonitor();
};

void MonitorWebServer(int pidServer) {
  WebServerMonitor theMonitor;
  theMonitor.Monitor(pidServer);
}

WebServerMonitor::WebServerMonitor() {
  this->timeAtLastBackup = - 1;
  this->pidServer = - 1;
}

void WebServerMonitor::BackupDatabaseIfNeeded() {
  MacroRegisterFunctionWithName("WebServer::BackupDatabaseIfNeeded");
  if (
    this->timeAtLastBackup > 0 &&
    global.getElapsedSeconds() - this->timeAtLastBackup < (24 * 3600)
  ) {
    return;
  }
  std::stringstream commandStream;
  commandStream << "mongodump --db calculator --archive ="
  << global.PhysicalPathProjectBase
  << "database-backups/dbBackup"
  << global.GetDateForLogFiles() << ".mongo";
  global << Logger::orange << "Backing up database with command: " << Logger::endL;
  global << commandStream.str() << Logger::endL;
  global.externalCommandReturnOutput(commandStream.str());
  global << Logger::green << "Backing up completed. " << Logger::endL;
  this->timeAtLastBackup = global.getElapsedSeconds();
}

void WebServerMonitor::Monitor(int pidServer) {
  if (!global.flagLocalhostConnectionMonitor) {
    return;
  }
  this->pidServer = pidServer;
  MacroRegisterFunctionWithName("Monitor");
  int maxNumPingFailures = 3;
  // warning: setting global.server().WebServerPingIntervalInSeconds to more than 1000
  // may overflow the variable int microsecondsToSleep.
  // global.server().WebServerPingIntervalInSeconds=1000;
  int microsecondsToSleep = 1000000 * global.server().WebServerPingIntervalInSeconds;//
  if (global.server().WebServerPingIntervalInSeconds > 30) {
    global << Logger::red << "**********WARNING**************"
    << Logger::endL
    << Logger::red << " The ping interval: "
    << global.server().WebServerPingIntervalInSeconds
    << " is set to a large value. "
    << "Set the ping interval to less than 30 seconds to remove this message. " << Logger::endL;
  }
  std::fstream theFile;
  FileOperations::OpenFileCreateIfNotPresentVirtual(
    theFile, "/LogFiles/server_starts_and_unexpected_restarts.html", true, false, false, true
  );
  theFile << "<a href=\"/LogFiles/" << GlobalVariables::GetDateForLogFiles() << "/\">"
  << GlobalVariables::GetDateForLogFiles() << "</a>" << "<br>\n";
  theFile.close();
  WebCrawler theCrawler;
  theCrawler.initialize();
  global << Logger::blue << "Pinging " << theCrawler.addressToConnectTo << " at port/service "
  << theCrawler.portOrService << " every " << (microsecondsToSleep / 1000000) << " second(s). "
  << Logger::endL;
  global << Logger::red << "Please beware that the server will restart and you will lose all computations "
  << "if " << maxNumPingFailures << " consecutive pings fail. " << Logger::endL;
  int numConsecutiveFailedPings = 0;
  int numPings = 0;
  TimeWrapper now;
  for (;;) {
    global.FallAsleep(microsecondsToSleep);
    this->BackupDatabaseIfNeeded();
    theCrawler.PingCalculatorStatus();
    numPings ++;
    if (theCrawler.lastTransactionErrors != "") {
      now.AssignLocalTime();
      numConsecutiveFailedPings ++;
      global << Logger::red << "Connection monitor: ping of " << theCrawler.addressToConnectTo
      << " at port/service " << theCrawler.portOrService
      << " failed. GM time: " << now.ToStringGM() << ", local time: " << now.ToStringLocal()
      << ". " << "Errors: "
      << theCrawler.lastTransactionErrors << theCrawler.lastTransaction
      << numConsecutiveFailedPings << " consecutive fails so far, restarting on " << maxNumPingFailures
      << ". " << Logger::endL;
    } else {
      std::cout << "Connection monitor: ping #" << numPings << ": received " << theCrawler.lastNumBytesRead << " bytes. " << std::endl;
      numConsecutiveFailedPings = 0;
    }
    if (numConsecutiveFailedPings >= maxNumPingFailures) {
      this->Restart();
    }
  }
}

void WebServerMonitor::Restart() {
  TimeWrapper now;
  now.AssignLocalTime();
  global << Logger::red << "Server stopped responding (probably locked pipe?)"
  << ", restarting. " << Logger::endL;
  std::fstream theFile;
  FileOperations::OpenFileCreateIfNotPresentVirtual(
    theFile, "LogFiles/server_starts_and_unexpected_restarts.html", true, false, false, true
  );
  theFile << "<b style ='color:red'>Unexpected server restart: server stopped responding (locked pipe?). Time: local: "
  << now.ToStringLocal() << ", GM: " << now.ToStringGM() << "</b><br>\n";
  theFile.flush();
  std::stringstream killServerChildrenCommand;
  killServerChildrenCommand << "pkill -9 -P " << this->pidServer;
  global << "Terminating server children with command: " << killServerChildrenCommand.str() << Logger::endL;
  global.externalCommandNoOutput(killServerChildrenCommand.str(), true);
  global << Logger::red << "Terminating server with pid: " << this->pidServer << Logger::endL;
  WebServer::TerminateProcessId(this->pidServer);
  global << Logger::red << "Restarting monitor. " << this->pidServer << Logger::endL;
  global.server().StopKillAll();
}

WebCrawler::WebCrawler() {
  this->flagInitialized = false;
  this->theSocket = - 1;
  this->serverOtherSide = nullptr;
  this->serverInfo = nullptr;
  this->lastNumBytesRead = 0;
}

void WebCrawler::FreeAddressInfo() {
  if (this->serverInfo != nullptr) {
    freeaddrinfo(this->serverInfo);
  }
  this->serverInfo = nullptr;
}

void WebCrawler::initialize() {
  if (this->flagInitialized) {
    return;
  }
  this->flagContinueWasNeeded = false;
  MacroRegisterFunctionWithName("WebCrawler::initialize");
  this->flagDoUseGET = true;
  buffer.initializeFillInObject(50000, 0);
  this->portOrService = "8155";
  this->addressToConnectTo = "127.0.0.1";
}

WebCrawler::~WebCrawler() {
  if (!this->flagInitialized) {
    return;
  }
  this->flagInitialized = false;
  close(this->theSocket);
  this->theSocket = - 1;
  this->FreeAddressInfo();
}

void WebCrawler::PingCalculatorStatus() {
  MacroRegisterFunctionWithName("WebCrawler::PingCalculatorStatus");
  std::stringstream reportStream;
  this->lastTransaction = "";
  this->lastTransactionErrors = "";
  memset(&this->hints, 0, sizeof this->hints); // make sure the struct is empty
  this->hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
  this->hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
  this->hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
  this->serverInfo = nullptr;
  int status = getaddrinfo(this->addressToConnectTo.c_str(), this->portOrService.c_str(), &hints, &this->serverInfo);
  if (status != 0) {
    this->lastTransactionErrors = "Could not find address: getaddrinfo error: ";
    this->lastTransactionErrors += gai_strerror(status);
    this->FreeAddressInfo();
    return;
  }
  if (this->serverInfo == nullptr) {
    this->lastTransactionErrors = "Server info is zero";
    return;
  }
  struct addrinfo *p = nullptr;  // will point to the results
  this->theSocket = - 1;
  char ipString[INET6_ADDRSTRLEN];
  int numBytesWritten = - 1;
  for (p = this->serverInfo; p != nullptr; p = p->ai_next, close(this->theSocket)) {
    void* theAddress = nullptr;
    this->theSocket = - 1;
    // get the pointer to the address itself,
    // different fields in IPv4 and IPv6:
    if (p->ai_family == AF_INET) {
      // IPv4
      struct sockaddr_in *ipv4 = reinterpret_cast<struct sockaddr_in *>(p->ai_addr);
      theAddress = &(ipv4->sin_addr);
      reportStream << "IPv4: ";
    } else {
      // IPv6
      struct sockaddr_in6 *ipv6 = reinterpret_cast<struct sockaddr_in6 *>(p->ai_addr);
      theAddress = &(ipv6->sin6_addr);
      reportStream << "IPv6: ";
    }
    // convert the IP to a string and print it:
    inet_ntop(p->ai_family, theAddress, ipString, sizeof ipString);
    reportStream << this->addressToConnectTo << ": " << ipString << "<br>";
    this->theSocket = socket(this->serverInfo->ai_family, this->serverInfo->ai_socktype, this->serverInfo->ai_protocol);
    int connectionResult = - 1;
    if (this->theSocket < 0) {
      this->lastTransactionErrors = "Failed to create socket ";
      continue;
    } else {
      fd_set fdConnectSockets;
      FD_ZERO(&fdConnectSockets);
      FD_SET(this->theSocket, &fdConnectSockets);
      timeval timeOut;
      timeOut.tv_sec = 1;
      timeOut.tv_usec = 0;
      int numPingFails = 0;
      int numSelected = 0;
      std::stringstream failStream;
      do {
        if (numPingFails > 10) {
          break;
        }
        numSelected = select(this->theSocket + 1, &fdConnectSockets, nullptr, nullptr, &timeOut);
        failStream << "While pinging, select failed. Error message: "
        << strerror(errno) << ". \n";
        numPingFails ++;
      } while (numSelected < 0);
      if (numSelected <= 0) {
        global << Logger::red << failStream.str() << Logger::endL;
        reportStream << failStream.str() << "Could not connect through port. Select returned: " << numSelected;
        connectionResult = - 1;
      } else {
        connectionResult = connect(this->theSocket, this->serverInfo->ai_addr, this->serverInfo->ai_addrlen);
      }
    }
    if (connectionResult == - 1) {
      reportStream << "<br>Failed to connect: address: " << this->addressToConnectTo << " port: "
      << this->portOrService << ". ";
//      << explain_errno_connect(this->theSocket, this->serverInfo->ai_addr, this->serverInfo->ai_addrlen);
      this->lastTransactionErrors = reportStream.str();
      close(this->theSocket);
      continue;
    } else {
      reportStream << "<br>connected: " << this->addressToConnectTo << " port: " << this->portOrService << ". ";
    }
    std::string getMessage = "GET /cgi-bin/calculator?request=statusPublic";
    std::stringstream errorStream1;
    numBytesWritten = Pipe::WriteWithTimeoutViaSelect(this->theSocket, getMessage, 1, 10, &errorStream1);
    if (static_cast<unsigned>(numBytesWritten) != getMessage.size()) {
      this->lastTransactionErrors += "\nERROR writing to socket. " + errorStream1.str();
      close(this->theSocket);
      continue;
    }
    std::stringstream errorStream2;
    this->lastNumBytesRead = Pipe::ReadWithTimeOutViaSelect(this->theSocket, this->buffer, 1, 10, &errorStream2);
    if (this->lastNumBytesRead < 0) {
      this->lastTransactionErrors += "ERROR reading from socket. " + errorStream2.str();
      close(this->theSocket);
      continue;
    }
    std::string readString;
    readString.assign(buffer.theObjects, static_cast<unsigned>(this->lastNumBytesRead));
    reportStream << "Wrote " << numBytesWritten  << ", read "
    << this->lastNumBytesRead << " bytes: " << readString << ". ";
    this->lastTransaction = reportStream.str();
    close(this->theSocket);
  }
  this->FreeAddressInfo();
}

void WebCrawler::FetchWebPage(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral) {
  MacroRegisterFunctionWithName("WebCrawler::FetchWebPage");
  (void) commentsOnFailure;
  (void) commentsGeneral;
  this->theTSL.openSSLData.CheckCanInitializeToClient();
  this->theTSL.initializeNonThreadSafeOnFirstCall(false);
#ifdef MACRO_use_open_ssl
  this->lastTransaction = "";
  this->lastTransactionErrors = "";
  memset(&this->hints, 0, sizeof this->hints); // make sure the struct is empty
  this->hints.ai_family   = AF_UNSPEC  ;       // don't care IPv4 or IPv6
  this->hints.ai_socktype = SOCK_STREAM;       // TCP stream sockets
  this->hints.ai_flags    = AI_PASSIVE ;       // fill in my IP for me
  this->serverInfo = nullptr;
  int status = getaddrinfo(this->serverToConnectTo.c_str(), this->portOrService.c_str(), &hints, &this->serverInfo);
  if (status != 0) {
    this->lastTransactionErrors =  "Error calling getaddrinfo: ";
    this->lastTransactionErrors += gai_strerror(status);
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << this->lastTransactionErrors << ". "
      << "Server: " << this->serverToConnectTo << ", port or service: " << this->portOrService << ". ";
    }
    this->FreeAddressInfo();
    return;
  }
  if (this->serverInfo == nullptr) {
    this->lastTransactionErrors = "Server info is zero";
    return;
  }
  struct addrinfo *p = nullptr;// will point to the results
  this->theSocket = - 1;
  char ipString[INET6_ADDRSTRLEN];
  timeval timeOut;
  for (p = this->serverInfo; p != nullptr; p = p->ai_next, close(this->theSocket)) {
    void *theAddress = nullptr;
    this->theSocket = - 1;
    // get the pointer to the address itself,
    // different fields in IPv4 and IPv6:
    if (p->ai_family == AF_INET) {
      // IPv4
      struct sockaddr_in *ipv4 = reinterpret_cast<struct sockaddr_in *>(p->ai_addr);
      theAddress = &(ipv4->sin_addr);
      if (commentsGeneral != nullptr) {
        *commentsGeneral << "IPv4: ";
      }
    } else {
      // IPv6
      struct sockaddr_in6 *ipv6 = reinterpret_cast<struct sockaddr_in6 *> (p->ai_addr);
      theAddress = &(ipv6->sin6_addr);
      if (commentsGeneral != nullptr) {
        *commentsGeneral << "IPv6: ";
      }
    }
    // convert the IP to a string and print it:
    inet_ntop(p->ai_family, theAddress, ipString, sizeof ipString);
//    std::string ipString()
    if (commentsGeneral != nullptr) {
      *commentsGeneral << this->addressToConnectTo << ": " << ipString << "<br>";
    }
    this->theSocket = socket(this->serverInfo->ai_family, this->serverInfo->ai_socktype, this->serverInfo->ai_protocol);
    int connectionResult = - 1;
    if (this->theSocket < 0) {
      this->lastTransactionErrors = "failed to create socket ";
      continue;
    } else {
      fd_set fdConnectSockets;
      FD_ZERO(&fdConnectSockets);
      FD_SET(this->theSocket, &fdConnectSockets);
      timeOut.tv_sec = 1;
      timeOut.tv_usec = 0;
      int numPingFails = 0;
      int numSelected = 0;
      std::stringstream failStream;
      do {
        if (numPingFails > 10) {
          break;
        }
        numSelected = select(this->theSocket + 1, &fdConnectSockets, nullptr, nullptr, &timeOut);
        failStream << "While pinging, select failed. Error message: "
        << strerror(errno) << ". \n";
        numPingFails ++;
      } while (numSelected < 0);
      if (numSelected <= 0) {
        global << Logger::red << failStream.str() << Logger::endL;
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure << failStream.str()
          << "Could not connect through port. Select returned: " << numSelected;
        }
        connectionResult = - 1;
      } else {
        connectionResult = connect(this->theSocket, this->serverInfo->ai_addr, this->serverInfo->ai_addrlen);
      }
      timeOut.tv_sec = 3;  // 5 Secs Timeout
      timeOut.tv_usec = 0;  // Not initialize'ing this can cause strange errors
      setsockopt(connectionResult, SOL_SOCKET, SO_RCVTIMEO, static_cast<void*>(&timeOut), sizeof(timeval));
    }
    if (connectionResult == - 1) {
      std::stringstream errorStream;
      errorStream
      << "Failed to connect: address: " << this->addressToConnectTo << " port: "
      << this->portOrService << ".<br>";
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << errorStream.str();
      }
      this->lastTransactionErrors = errorStream.str();
      close(this->theSocket);
      continue;
    } else {
      if (commentsGeneral != nullptr) {
        *commentsGeneral << "connected: "
        << this->addressToConnectTo << " port: " << this->portOrService << ". <hr>";
      }
    }
    this->FetchWebPagePart2(commentsOnFailure, commentsGeneral);
    global.server().theTLS.RemoveLastSocket();
    close(this->theSocket);
    break;
  }
  this->FreeAddressInfo();
#endif //Macro_use_openssl
}

void WebCrawler::FetchWebPagePart2(
  std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
) {
  MacroRegisterFunctionWithName("WebCrawler::FetchWebPagePart2");
  (void) commentsOnFailure;
  (void) commentsGeneral;
#ifdef MACRO_use_open_ssl
  std::stringstream theMessageHeader, theContinueHeader;
  if (this->flagDoUseGET) {
    theMessageHeader << "GET " << this->addressToConnectTo << " HTTP/1.0"
    << "\r\n" << "Host: " << this->serverToConnectTo << "\r\n\r\n";
  } else {
    theMessageHeader << "POST " << this->addressToConnectTo << " HTTP/1.0"
    << "\r\n" << "Host: " << this->serverToConnectTo;
    theMessageHeader << "\r\nContent-length: " << this->postMessageToSend.size();
    theMessageHeader << "\r\n\r\n";
    theMessageHeader << this->postMessageToSend;
  }
  this->theTSL.openSSLData.CheckCanInitializeToClient();
  if (!this->theTSL.HandShakeIamClientNoSocketCleanup(
    this->theSocket, commentsOnFailure, commentsGeneral
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Could not shake hands. ";
    }
    return;
  }
  this->headerReceived = "";
  this->bodyReceiveD = "";
  this->flagContinueWasNeeded = false;
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "<hr>";
  }
  std::string errorSSL;
  if (!this->theTSL.SSLWriteLoop(
    10, theMessageHeader.str(), &errorSSL, commentsGeneral, true
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "SSL critical error: " << errorSSL;
    }
    return;
  }
  if (!this->theTSL.SSLReadLoop(
    10, this->headerReceived, 0, &errorSSL, commentsGeneral, true
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "SSL critical error: " << errorSSL;
    }
    return;
  }
  unsigned bodyStart = 0;
  int numcrlfs = 0;
  //std::stringstream tempStream;
  for (; bodyStart < this->headerReceived.size(); bodyStart ++) {
    if (numcrlfs >= 4) {
      break;
    }
    if (this->headerReceived[bodyStart] == '\n' || this->headerReceived[bodyStart] == '\r') {
      numcrlfs ++;
    } else {
      numcrlfs = 0;
    }
  }
  if (bodyStart < this->headerReceived.size()) {
    this->bodyReceivedWithHeader = this->headerReceived.substr(bodyStart);
    this->headerReceived = this->headerReceived.substr(0, bodyStart);
  }
  List<std::string> theHeaderPieces;
  StringRoutines::StringSplitDefaultDelimiters(this->headerReceived, theHeaderPieces);
  std::string expectedLengthString;
  for (int i = 0; i < theHeaderPieces.size; i ++) {
    if (
      theHeaderPieces[i] == "Content-length:" ||
      theHeaderPieces[i] == "Content-Length:" ||
      theHeaderPieces[i] == "content-length:"
    ) {
      if (i + 1 < theHeaderPieces.size) {
        expectedLengthString=theHeaderPieces[i + 1];
        break;
      }
    }
  }
  this->expectedLength = - 1;
  if (expectedLengthString != "") {
    expectedLength.assignString(expectedLengthString);
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "<br>Expected length: " << this->expectedLength;
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "<br>Header:<br>" << this->headerReceived;
  }
  if (this->headerReceived.find("200 OK") == std::string::npos) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "No 200 ok message found. Headers received: " << this->headerReceived;
    }
    return;
  }
  if (expectedLength == this->bodyReceivedWithHeader.size()) {
    this->bodyReceiveD = this->bodyReceivedWithHeader;
    return;
  }
  this->flagContinueWasNeeded = true;
  theContinueHeader << "HTTP/1.0 100 Continue\r\n\r\n";
  if (!this->theTSL.SSLWriteLoop(
    10, theContinueHeader.str(), &errorSSL, commentsGeneral, true
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "SSL critical error: " << errorSSL;
    }
    return;
  }
  std::string secondPart;
  if (!this->theTSL.SSLReadLoop(
    10, secondPart, expectedLength, &errorSSL, commentsGeneral, true
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "SSL critical error: " << errorSSL;
    }
    return;
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "<br>Second part length: " << secondPart.size();
  }
  this->bodyReceiveD = this->bodyReceivedWithHeader + secondPart;
  int theSize = 0;
  this->expectedLength.isIntegerFittingInInt(&theSize);
  if (static_cast<unsigned>(theSize) < this->bodyReceiveD.size()) {
    this->bodyReceivedOutsideOfExpectedLength = this->bodyReceiveD.substr(static_cast<unsigned>(theSize));
    this->bodyReceiveD = this->bodyReceiveD.substr(0, static_cast<unsigned>(theSize));
  }
  //if (!global.server().theSSLdata.SSLreadLoop
  //    (10,global.server().theSSLdata.sslClient, secondPart, comments, comments, true))
  //  return;
  //this->bodyReceived+=secondPart;
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "<br>Body (length: "
    << this->bodyReceiveD.size()
    << ")<br>" << this->bodyReceiveD;
    if (this->bodyReceivedOutsideOfExpectedLength.size() == 0) {
      *commentsGeneral << "<br><span style =\"color:green\"><b>No extraneous data received</b></span>";
    } else {
      *commentsGeneral<< "<br><span style =\"color:red\"><b>Received more data than expected "
      << "(perhaps due to a protocol error?).</b></span>"
      << "<br>" << this->bodyReceivedOutsideOfExpectedLength;
    }
  }
  this->theTSL.Free();
#endif
}

bool CalculatorFunctions::innerFetchWebPageGET(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFetchWebPageGET");
  if (!global.UserDefaultHasAdminRights()) {
    return output.assignValue(std::string("Fetching web pages available only for logged-in admins. "), theCommands);
  }
  WebCrawler theCrawler;
  if (input.size() != 4) {
    return theCommands << "Fetching web page expects 3 arguments: server, service/port, and webpage. ";
  }
  if (!input[1].isOfType(&theCrawler.serverToConnectTo)) {
    theCrawler.serverToConnectTo = input[1].toString();
  }
  if (!input[2].isOfType(&theCrawler.portOrService)) {
    theCrawler.portOrService = input[2].toString();
  }
  if (!input[3].isOfType(&theCrawler.addressToConnectTo)) {
    theCrawler.addressToConnectTo = input[3].toString();
  }
  std::stringstream out;
  theCrawler.flagDoUseGET = true;
  out
  << "Server:  " << theCrawler.serverToConnectTo
  << " port: " << theCrawler.portOrService
  << " resource: " << theCrawler.addressToConnectTo
  << "<br>";
  theCrawler.FetchWebPage(&out, &out);
  out << "<br>" << theCrawler.lastTransactionErrors << "<hr>" << theCrawler.lastTransaction;
  return output.assignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerFetchWebPagePOST(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFetchWebPagePOST");
  if (!global.UserDefaultHasAdminRights()) {
    return output.assignValue(std::string("Fetching web pages available only for logged-in admins. "), theCommands);
  }
  WebCrawler theCrawler;
  if (input.size() != 5) {
    return theCommands << "Fetching web page expects 4 arguments: server, service/port, webpage and message to post. ";
  }
  if (!input[1].isOfType(&theCrawler.serverToConnectTo)) {
    theCrawler.serverToConnectTo = input[1].toString();
  }
  if (!input[2].isOfType(&theCrawler.portOrService)) {
    theCrawler.portOrService = input[2].toString();
  }
  if (!input[3].isOfType(&theCrawler.addressToConnectTo)) {
    theCrawler.addressToConnectTo = input[3].toString();
  }
  if (!input[4].isOfType(&theCrawler.postMessageToSend)) {
    theCrawler.postMessageToSend = input[4].toString();
  }
  std::stringstream out;
  out
  << "Server:  " << theCrawler.serverToConnectTo
  << " port: " << theCrawler.portOrService
  << " resource: " << theCrawler.addressToConnectTo
  << "<br>";
  theCrawler.flagDoUseGET = false;
  theCrawler.FetchWebPage(&out, &out);
  out << "<br>" << theCrawler.lastTransactionErrors << "<hr>" << theCrawler.lastTransaction;
  return output.assignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerFetchKnownPublicKeys(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFetchKnownPublicKeys");
  (void) input;
  std::stringstream out;
  if (!global.UserDefaultHasAdminRights()) {
    out << "You need to be a logged-in administrator to call this function. ";
    return output.assignValue(out.str(), theCommands);
  }
  WebCrawler theCrawler;
  theCrawler.theTSL.openSSLData.name = "crawler";
  theCrawler.UpdatePublicKeys(&out, &out);
  return output.assignValue(out.str(), theCommands);
}

void WebCrawler::UpdatePublicKeys(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral) {
  MacroRegisterFunctionWithName("WebCrawler::UpdatePublicKeys");
  this->serverToConnectTo  = "www.googleapis.com";
  this->portOrService      = "https";
  this->addressToConnectTo = "https://www.googleapis.com/oauth2/v3/certs";
  this->flagDoUseGET = true;
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "<hr>" << "Updating public keys <hr>";
  }
  this->FetchWebPage(commentsOnFailure, commentsGeneral);
  if (this->bodyReceiveD == "") {
    global << Logger::red << "Could not fetch the google public keys ..." << Logger::endL;
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Could not fetch google certificate list. ";
    }
    return;
  }
  std::string googleKeysFileName = "certificates-public/google.txt";
  std::string googleKeysDebugFileName = "certificates-public/debug-google.txt";
  std::fstream googleKeysFile, googleKeysDebugFile;
  if (!FileOperations::OpenFileCreateIfNotPresentVirtual(
    googleKeysFile, googleKeysFileName, false, true, false
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<br>Failed to open: " << googleKeysFileName;
    }
    global << Logger::red << "Failed to create google keys file name. " << Logger::endL;
    return;
  }
  FileOperations::OpenFileCreateIfNotPresentVirtual(googleKeysDebugFile, googleKeysDebugFileName, false, true, false);
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "<br>Updated file: " << googleKeysFileName;
  }
  global << Logger::green << "Updated public key file: " << googleKeysFileName << Logger::endL;
  googleKeysFile << this->bodyReceiveD;
  googleKeysDebugFile
  << "Expected body length: " << this->expectedLength.toString() << "\n";
  if (this->flagContinueWasNeeded) {
    googleKeysDebugFile << "Did send a continue message.\n";
  } else {
    googleKeysDebugFile << "Did NOT send a continue message.\n";
  }
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

bool Crypto::VerifyJWTagainstKnownKeys(
  const std::string& inputToken,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  MacroRegisterFunctionWithName("Crypto::VerifyJWTagainstKnownKeys");
  // This function is slightly insecure.
  // If an attacker hijacks a machine connecting the server to the outside
  // and impersonates google
  // the attacker would be able to impersonate another user.
  // Since we are reloading the google keys for
  // every JWT key we fail to find in our cache,
  // this could happen more easily
  // than if we were storing the google keys until their expiry date.
  // To solve this problem, we need to
  // ensure that the google public keys are authentic.
  // For that we need to leverage the built-in web of trust of
  // the host system (pgp keys, etc.).
  // This is system dependent and a lot of work-> not doing now, but will do
  // in the future as the need arises.
  JSONWebToken theToken;
  if (!theToken.assignString(inputToken, commentsOnFailure)) {
    return false;
  }
  std::string keyIDstring = "";
  JSData header;
  if (!header.readstring(theToken.headerJSON)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Couldn't load JSON from the user token.";
    }
    return false;
  }
  if (header.theType == JSData::token::tokenObject) {
    if (header.HasKey("kid")) {
      keyIDstring = header.getValue("kid").theString;
    }
  }
  if (keyIDstring == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Couldn't find key id in the token.";
    }
    return false;
  }
  int theIndex = - 1;
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "Seeking key: <b style =\"color:brown\">" << keyIDstring << "</b>. ";
  }
  for (int i = 0; i < 2; i ++) {
    Crypto::LoadKnownCertificates(commentsOnFailure, commentsGeneral);
    for (int j = 0; j < Crypto::knownCertificates.size; j ++) {
      if (keyIDstring == Crypto::knownCertificates[j].keyid) {
        theIndex = j;
        break;
      }
    }
    if (theIndex != - 1 || i == 1) {
      break;
    }
    if (commentsGeneral != nullptr && i == 0) {
      *commentsGeneral << "<br><b style =\"color:red\">Couldn't find key ID: "
      << keyIDstring << " from cached certificate.</b>";
    }
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "<br>Reloading google public keys. ";
    }
    WebCrawler theCrawler;
    theCrawler.theTSL.openSSLData.name = "public key fetcher";
    theCrawler.UpdatePublicKeys(commentsOnFailure, commentsGeneral);
  }
  if (theIndex == - 1) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<b style =\"color:red\">Could not find key id: "
      << keyIDstring << "</b>. ";
    }
    return false;
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "<b style =\"color:green\">Found key id: "
    << keyIDstring << ".</b>";
  }
  PublicKeyRSA& currentCert = Crypto::knownCertificates[theIndex];
  return theToken.verifyRSA256(
    currentCert.theModulus, currentCert.theExponent, commentsOnFailure, commentsGeneral
  );
}

bool WebCrawler::VerifyRecaptcha(
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNONsensitive,
  std::stringstream* commentsGeneralSensitive
) {
  MacroRegisterFunctionWithName("WebCrawler::VerifyRecaptcha");
  std::stringstream messageToSendStream;
  std::string secret;
  std::stringstream notUsed;
  if (commentsOnFailure == nullptr) {
    commentsOnFailure = &notUsed;
  }
  if (!FileOperations::LoadFileToStringVirtual_AccessUltraSensitiveFoldersIfNeeded(
    "certificates/recaptcha-secret.txt", secret, true, true, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<b style =\"color:red\">"
      << "Failed to load recaptcha secret."
      << "</b>";
    }
    return false;
  }
  std::string recaptchaURLencoded = global.getWebInput("recaptchaToken");
  if (commentsGeneralSensitive != nullptr) {
    *commentsGeneralSensitive << "Recaptcha: " << recaptchaURLencoded;
  }
  if (recaptchaURLencoded == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<b style =\"color:red\">Recaptcha appears to be missing. </b>";
    }
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
  if (!theJSparser.readstring(response, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "<b style ='color:red'>" << "Failed to extract response token from captcha verification. "
      << "</b>"
      << "<br>The response string was: "
      << response;
    }
    return 0;
  }
  if (!theJSparser.HasKey("success")) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "<b style ='color:red'>" << "Captcha failure: could not find key 'success'."
      << "</b>"
      ;
    }
    return false;
  }
  JSData theSuccess;
  theSuccess = theJSparser.getValue("success");
  if (theSuccess.theType != JSData::token::tokenBool || theSuccess.theBoolean != true) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<br><b style =\"color:red\">"
      << "Could not verify your captcha solution. "
      << "</b>"
      << "The response from google was: "
      << response
      ;
    }
    return false;
  } else {
    if (commentsGeneralNONsensitive != nullptr) {
      *commentsGeneralNONsensitive
      << "<br><b style =\"color:green\">"
      << "Your recaptcha answer appears to be valid. "
      << "</b>\n<br>\n";
    }
    if (commentsGeneralSensitive != nullptr) {
      *commentsGeneralSensitive
      << "The response from google was: "
      << response;
    }
  }
  return true;
}

bool WebAPIResponse::processForgotLogin() {
  MacroRegisterFunctionWithName("WebAPIResponse::processForgotLogin");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  if (!global.flagDatabaseCompiled) {
    result[WebAPI::result::error] = "Error: database not running. ";
    return global.theResponse.writeResponse(result, false);
  }
  std::stringstream out;
  if (!global.UserDefaultHasAdminRights()) {
    Database::get().theUser.LogoutViaDatabase();
  }
  UserCalculator theUser;
  theUser.email = HtmlRoutines::convertURLStringToNormal(global.getWebInput("email"), false);
  WebCrawler theCrawler;
  out << "<br><b> "
  << "Please excuse our verbose technical messages. </b>"
  << "<br><b>We are still testing our system; "
  << "we will remove the technical garbage as soon as we are done. "
  << "</b><br>\n";
  if (!theCrawler.VerifyRecaptcha(&out, &out, nullptr)) {
    result[WebAPI::result::comments] = out.str();
    return global.theResponse.writeResponse(result, false);
  }
  if (!theUser.exists(&out)) {
    out << "<br><b style =\"color:red\">"
    << "We failed to find your email: " << theUser.email << " in our records. "
    << "</b>";
    result[WebAPI::result::comments] = out.str();
    return global.theResponse.writeResponse(result, false);
  }
  if (!theUser.loadFromDatabase(&out, &out)) {
    out << "<br><b style='color:red'>"
    << "Failed to fetch user info for email: " << theUser.email
    << "</b>";
    result[WebAPI::result::comments] = out.str();
    return global.theResponse.writeResponse(result, false);
  }
  out << "<b style ='color:green'>"
  << "Your email is on record. "
  << "</b>";
  if (!global.UserDefaultHasAdminRights()) {
    this->owner->DoSetEmail(theUser, &out, &out, nullptr);
  } else {
    this->owner->DoSetEmail(theUser, &out, &out, &out);
  }
  out << "<br>Response time: " << global.getElapsedSeconds() << " second(s); "
  << global.getElapsedSeconds() << " second(s) spent creating account. ";
  result[WebAPI::result::comments] = out.str();
  return global.theResponse.writeResponse(result, false);
}

JSData WebWorker::GetSignUpRequestResult() {
  MacroRegisterFunctionWithName("WebWorker::GetSignUpRequestResult");
  JSData result;
  std::stringstream errorStream;
  if (!global.flagDatabaseCompiled) {
    result["error"] = "Database not available (cannot sign up). ";
    return result;
  }
  Database::get().theUser.LogoutViaDatabase();
  UserCalculator theUser;
  theUser.username = HtmlRoutines::convertURLStringToNormal(global.getWebInput("desiredUsername"), false);
  theUser.email = HtmlRoutines::convertURLStringToNormal(global.getWebInput("email"), false);
  std::stringstream generalCommentsStream;
  std::stringstream outputStream;
  generalCommentsStream
  << "<b>Please excuse our technical messages, they will be removed soon.</b>";
  WebCrawler theCrawler;
  if (!theCrawler.VerifyRecaptcha(&errorStream, &generalCommentsStream, nullptr)) {
    result["error"] = errorStream.str();
    result[WebAPI::result::comments] = generalCommentsStream.str();
    return result;
  }
  if (theUser.username == "") {
    errorStream << "Empty username not allowed. ";
    result["error"] = errorStream.str();
    result[WebAPI::result::comments] = generalCommentsStream.str();
    return result;
  }
  if (!EmailRoutines::isOKEmail(theUser.email, &generalCommentsStream)) {
    errorStream << "Your email address does not appear to be valid. ";
    result["error"] = errorStream.str();
    result[WebAPI::result::comments] = generalCommentsStream.str();
    return result;
  }
  if (theUser.exists(&generalCommentsStream)) {
    errorStream
    << "Either the username ("
    << theUser.username
    << ") or the email ("
    << theUser.email
    << ") you requested is already taken.";
    result["error"] = errorStream.str();
    result[WebAPI::result::comments] = generalCommentsStream.str();
    return result;
  } else {
    outputStream << "<b style ='color:green'>"
    << "Username ("
    << theUser.username
    << ") with email ("
    << theUser.email
    << ") is available. </b>";
  }
  if (!theUser.storeToDatabase(false, &errorStream)) {
    errorStream << "Failed to store error stream. ";
    result["error"] = errorStream.str();
    result[WebAPI::result::comments] = generalCommentsStream.str();
    result[WebAPI::result::resultLabel] = outputStream.str();
    return result;
  }
  std::stringstream* adminOutputStream = nullptr;
  if (global.UserDefaultHasAdminRights()) {
    adminOutputStream = &generalCommentsStream;
  }
  //int fixThis;
  //adminOutputStream = &generalCommentsStream;
  this->DoSetEmail(theUser, &errorStream, &generalCommentsStream, adminOutputStream);
  result["error"] = errorStream.str();
  result[WebAPI::result::comments] = generalCommentsStream.str();
  result[WebAPI::result::resultHtml] = outputStream.str();
  return result;
}

bool WebWorker::writeToBodyJSON(const JSData& result) {
  std::string toWrite = HtmlRoutines::convertStringToHtmlString(
    result.toString(nullptr), false
  );
  if (toWrite.size() < 2000) {
    if (toWrite.find(WebAPIResponse::youHaveReachedTheBackend) != std::string::npos) {
      std::string sanitizedCalculatorApp = HtmlRoutines::convertStringToHtmlString(global.displayApplication, false);
      std::stringstream outLinkApp;
      outLinkApp << "You've reached the calculator's backend. The app can be accessed here: <a href = '"
      << sanitizedCalculatorApp << "'>app</a>";
      toWrite = StringRoutines::ReplaceAll(toWrite, WebAPIResponse::youHaveReachedTheBackend, outLinkApp.str());
    }
  }  
  return this->WriteToBody(toWrite);
}

bool GlobalVariables::Response::writeResponse(const JSData& incoming, bool isCrash) {
  MutexLockGuard guard(global.mutexReturnBytes);
  MacroRegisterFunctionWithName("WebWorker::writeResponse");
  if (!global.flagRunningBuiltInWebServer) {
    if (!isCrash) {
      global << incoming.toString();
    }
    return true;
  }
  WebWorker& worker = global.server().getActiveWorker();
  std::string status = "finished";
  if (isCrash) {
    status = "crash";
  }
  JSData output = incoming;
  std::string comments = global.comments.getCurrentReset();
  if (comments != "") {
    output[WebAPI::result::commentsGlobal] = comments;
  }
  if (this->flagTimedOut) {
    worker.writeAfterTimeoutJSON(
      output,
      status,
      global.relativePhysicalNameOptionalResult
    );
  } else {
    worker.setHeaderOKNoContentLength("");
    worker.writeToBodyJSON(output);
    worker.sendPending();
  }
  if (isCrash) {
    global.server().wrapUp();
  }
  return true;
}

void GlobalVariables::Response::report(const std::string &input) {
  MutexLockGuard guard(global.mutexReturnBytes);
  MacroRegisterFunctionWithName("GlobalVariables::Progress::report");
  return global.server().getActiveWorker().writeAfterTimeoutProgress(input, false);
}

void GlobalVariables::Response::Initiate(const std::string& message) {
  // TODO(tmilev): investigate the performance of this snippet
  if (global.theResponse.flagTimedOut) {
    return;
  }
  MutexLockGuard guard(global.mutexReturnBytes);
  MacroRegisterFunctionWithName("GlobalVariables::Progress::Initiate");
  if (!global.theResponse.monitoringAllowed()) {
    return;
  }
  if (!global.theResponse.flagReportDesired) {
    return;
  }
  global.theResponse.flagTimedOut = true;
  global.server().getActiveWorker().writeAfterTimeoutShowIndicator(message);
}
