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
  std::string bodyReceived;
  std::string bodyReceivedWithHeader;
  std::string bodyReceivedOutsideOfExpectedLength;

  bool flagInitialized;
  bool flagContinueWasNeeded;
  bool flagDoUseGET;
  LargeInteger expectedLength;
  List<char> buffer;
  int lastNumBytesRead;
  struct sockaddr_in serverAddress;
  struct hostent* serverOtherSide;
  struct addrinfo hints;
  struct addrinfo* serverInfo;
  WebCrawler();
  ~WebCrawler();
  void closeEverything();
  void updatePublicKeys(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  void fetchWebPagePart2(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  void initialize();
  // Ping authentication used for pings only, over localhost connections only.
  void pingCalculatorStatus(const std::string& pingAuthentication);
  void freeAddressInfo();
  void fetchWebPage(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral);
  bool verifyRecaptcha(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNONsensitive,
    std::stringstream* commentsGeneralSensitive
  );
};

class WebServerMonitor {
public:
  void backupDatabaseIfNeeded();
  double timeAtLastBackup;
  int pidServer;
  void monitor(int pidServer, const std::string& pingAuthentication);
  void stop();
  WebServerMonitor();
};

void monitorWebServer(int pidServer, const std::string& pingAuthentication) {
  WebServerMonitor monitor;
  monitor.monitor(pidServer, pingAuthentication);
}

WebServerMonitor::WebServerMonitor() {
  this->timeAtLastBackup = - 1;
  this->pidServer = - 1;
}

void WebServerMonitor::backupDatabaseIfNeeded() {
  MacroRegisterFunctionWithName("WebServer::BackupDatabaseIfNeeded");
  if (
    this->timeAtLastBackup > 0 &&
    global.getElapsedSeconds() - this->timeAtLastBackup < (24 * 3600)
  ) {
    return;
  }
  std::stringstream commandStream;
  commandStream << "mongodump --db calculator --archive ="
  << global.physicalPathProjectBase
  << "database-backups/dbBackup"
  << global.getDateForLogFiles() << ".mongo";
  global << Logger::orange << "Backing up database with command: " << Logger::endL;
  global << commandStream.str() << Logger::endL;
  global.externalCommandReturnOutput(commandStream.str());
  global << Logger::green << "Backing up completed. " << Logger::endL;
  this->timeAtLastBackup = global.getElapsedSeconds();
}

void WebServerMonitor::monitor(int pidServer, const std::string& pingAuthentication) {
  if (!global.flagLocalhostConnectionMonitor) {
    return;
  }
  this->pidServer = pidServer;
  MacroRegisterFunctionWithName("Monitor");
  int maxNumPingFailures = 3;
  // warning: setting global.server().WebServerPingIntervalInSeconds to more than 1000
  // may overflow the variable int microsecondsToSleep.
  int microsecondsToSleep = 1000000 * global.server().webServerPingIntervalInSeconds;//
  if (global.server().webServerPingIntervalInSeconds > 30) {
    global << Logger::red << "**********WARNING**************"
    << Logger::endL
    << Logger::red << " The ping interval: "
    << global.server().webServerPingIntervalInSeconds
    << " is set to a large value. "
    << "Set the ping interval to less than 30 seconds to remove this message. " << Logger::endL;
  }
  std::fstream theFile;
  FileOperations::openFileCreateIfNotPresentVirtual(
    theFile, "/LogFiles/server_starts_and_unexpected_restarts.html", true, false, false, true
  );
  theFile << "<a href=\"/LogFiles/" << GlobalVariables::getDateForLogFiles() << "/\">"
  << GlobalVariables::getDateForLogFiles() << "</a>" << "<br>\n";
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
    global.fallAsleep(microsecondsToSleep);
    this->backupDatabaseIfNeeded();
    theCrawler.pingCalculatorStatus(pingAuthentication);
    numPings ++;
    if (theCrawler.lastTransactionErrors != "") {
      now.assignLocalTime();
      numConsecutiveFailedPings ++;
      global << Logger::red << "Connection monitor: ping of " << theCrawler.addressToConnectTo
      << " at port/service " << theCrawler.portOrService
      << " failed. GM time: " << now.toStringGM() << ", local time: " << now.toStringLocal()
      << ". " << "Errors: "
      << theCrawler.lastTransactionErrors << theCrawler.lastTransaction
      << numConsecutiveFailedPings << " consecutive fails so far, restarting on " << maxNumPingFailures
      << ". " << Logger::endL;
    } else {
      std::cout << "Connection monitor: ping #" << numPings
      << ": received " << theCrawler.lastNumBytesRead << " bytes. " << std::endl;
      numConsecutiveFailedPings = 0;
    }
    if (numConsecutiveFailedPings >= maxNumPingFailures) {
      this->stop();
    }
  }
}

void WebServerMonitor::stop() {
  TimeWrapper now;
  now.assignLocalTime();
  global << Logger::red << "Server stopped responding. "
  << "Logging this event and stopping the monitor." << Logger::endL;
  std::fstream theFile;
  FileOperations::openFileCreateIfNotPresentVirtual(
    theFile, "LogFiles/server_starts_and_unexpected_restarts.html", true, false, false, true
  );
  theFile << "<b style ='color:red'>Unexpected server restart: server stopped responding (locked pipe?). Time: local: "
  << now.toStringLocal() << ", GM: " << now.toStringGM() << "</b><br>\n";
  theFile.flush();
  std::stringstream killServerChildrenCommand;
  killServerChildrenCommand << "pkill -9 -P " << this->pidServer;
  global << "Terminating server children with command: " << killServerChildrenCommand.str() << Logger::endL;
  global.externalCommandNoOutput(killServerChildrenCommand.str(), true);
  global << Logger::red << "Terminating server with pid: " << this->pidServer << Logger::endL;
  WebServer::terminateProcessId(this->pidServer);
  global << Logger::red << "Restarting monitor. " << this->pidServer << Logger::endL;
  global.server().stopKillAll();
}

WebCrawler::WebCrawler() {
  this->flagInitialized = false;
  this->theSocket = - 1;
  this->serverOtherSide = nullptr;
  this->serverInfo = nullptr;
  this->lastNumBytesRead = 0;
}

void WebCrawler::freeAddressInfo() {
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
  this->closeEverything();
}

void WebCrawler::closeEverything() {
  if (!this->flagInitialized) {
    return;
  }
  this->flagInitialized = false;
  close(this->theSocket);
  this->theSocket = - 1;
  this->freeAddressInfo();
}

void WebCrawler::pingCalculatorStatus(const std::string& pingAuthentication) {
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
    this->freeAddressInfo();
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
        reportStream << failStream.str() << "Could not connect through port. select returned: " << numSelected;
        connectionResult = - 1;
      } else {
        connectionResult = connect(this->theSocket, this->serverInfo->ai_addr, this->serverInfo->ai_addrlen);
      }
    }
    if (connectionResult == - 1) {
      reportStream << "<br>Failed to connect: address: " << this->addressToConnectTo << " port: "
      << this->portOrService << ". ";
      this->lastTransactionErrors = reportStream.str();
      close(this->theSocket);
      continue;
    } else {
      reportStream << "<br>connected: " << this->addressToConnectTo << " port: " << this->portOrService << ". ";
    }
    std::string getMessage = "GET /cgi-bin/calculator?request=" + pingAuthentication;
    std::stringstream errorStream1;
    numBytesWritten = Pipe::writeWithTimeoutViaSelect(this->theSocket, getMessage, 1, 10, &errorStream1);
    if (static_cast<unsigned>(numBytesWritten) != getMessage.size()) {
      this->lastTransactionErrors += "\nERROR writing to socket. " + errorStream1.str();
      close(this->theSocket);
      continue;
    }
    std::stringstream errorStream2;
    this->lastNumBytesRead = Pipe::readWithTimeOutViaSelect(this->theSocket, this->buffer, 1, 10, &errorStream2);
    if (this->lastNumBytesRead < 0) {
      this->lastTransactionErrors += "ERROR reading from socket. " + errorStream2.str();
      close(this->theSocket);
      continue;
    }
    std::string readString;
    readString.assign(buffer.objects, static_cast<unsigned>(this->lastNumBytesRead));
    reportStream << "Wrote " << numBytesWritten  << ", read "
    << this->lastNumBytesRead << " bytes: " << readString << ". ";
    this->lastTransaction = reportStream.str();
    close(this->theSocket);
  }
  this->freeAddressInfo();
}

void WebCrawler::fetchWebPage(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral) {
  MacroRegisterFunctionWithName("WebCrawler::fetchWebPage");
  (void) commentsOnFailure;
  (void) commentsGeneral;
  this->theTSL.openSSLData.checkCanInitializeToClient();
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
    this->freeAddressInfo();
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
          << "Could not connect through port. select returned: " << numSelected;
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
    this->fetchWebPagePart2(commentsOnFailure, commentsGeneral);
    this->theTSL.removeLastSocket();
    close(this->theSocket);
    break;
  }
  this->freeAddressInfo();
#endif //Macro_use_openssl
}

void WebCrawler::fetchWebPagePart2(
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
  this->theTSL.openSSLData.checkCanInitializeToClient();
  if (!this->theTSL.handShakeIAmClientNoSocketCleanup(
    this->theSocket, commentsOnFailure, commentsGeneral
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Could not shake hands. ";
    }
    return;
  }
  this->headerReceived = "";
  this->bodyReceived = "";
  this->flagContinueWasNeeded = false;
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "<hr>";
  }
  std::string errorSSL;
  if (!this->theTSL.sslWriteLoop(
    10, theMessageHeader.str(), &errorSSL, commentsGeneral, true
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "SSL critical error: " << errorSSL;
    }
    return;
  }
  if (!this->theTSL.sslReadLoop(
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
  StringRoutines::stringSplitDefaultDelimiters(this->headerReceived, theHeaderPieces);
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
    this->bodyReceived = this->bodyReceivedWithHeader;
    return;
  }
  this->flagContinueWasNeeded = true;
  theContinueHeader << "HTTP/1.0 100 Continue\r\n\r\n";
  if (!this->theTSL.sslWriteLoop(
    10, theContinueHeader.str(), &errorSSL, commentsGeneral, true
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "SSL critical error: " << errorSSL;
    }
    return;
  }
  std::string secondPart;
  if (!this->theTSL.sslReadLoop(
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
  this->bodyReceived = this->bodyReceivedWithHeader + secondPart;
  int length = 0;
  this->expectedLength.isIntegerFittingInInt(&length);
  if (static_cast<unsigned>(length) < this->bodyReceived.size()) {
    this->bodyReceivedOutsideOfExpectedLength = this->bodyReceived.substr(static_cast<unsigned>(length));
    this->bodyReceived = this->bodyReceived.substr(0, static_cast<unsigned>(length));
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "<br>Body (length: "
    << this->bodyReceived.size()
    << ")<br>" << this->bodyReceived;
    if (this->bodyReceivedOutsideOfExpectedLength.size() == 0) {
      *commentsGeneral << "<br><b style='color:green'>No extraneous data received</b>";
    } else {
      *commentsGeneral<< "<br><b style='color:red'>Received more data than expected "
      << "(perhaps due to a protocol error?).</b>"
      << "<br>" << this->bodyReceivedOutsideOfExpectedLength;
    }
  }
  this->theTSL.free();
#endif
}

bool CalculatorFunctions::innerFetchWebPageGET(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFetchWebPageGET");
  if (!global.userDefaultHasAdminRights()) {
    return output.assignValue(std::string("Fetching web pages available only for logged-in admins. "), calculator);
  }
  WebCrawler theCrawler;
  if (input.size() != 4) {
    return calculator << "Fetching web page expects 3 arguments: server, service/port, and webpage. ";
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
  theCrawler.fetchWebPage(&out, &out);
  out << "<br>" << theCrawler.lastTransactionErrors << "<hr>" << theCrawler.lastTransaction;
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::fetchWebPagePOST(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFetchWebPagePOST");
  if (!global.userDefaultHasAdminRights()) {
    return output.assignValue(std::string("Fetching web pages available only for logged-in admins. "), calculator);
  }
  WebCrawler crawler;
  if (input.size() != 5) {
    return calculator << "Fetching web page expects 4 arguments: server, service/port, webpage and message to post. ";
  }
  if (!input[1].isOfType(&crawler.serverToConnectTo)) {
    crawler.serverToConnectTo = input[1].toString();
  }
  if (!input[2].isOfType(&crawler.portOrService)) {
    crawler.portOrService = input[2].toString();
  }
  if (!input[3].isOfType(&crawler.addressToConnectTo)) {
    crawler.addressToConnectTo = input[3].toString();
  }
  if (!input[4].isOfType(&crawler.postMessageToSend)) {
    crawler.postMessageToSend = input[4].toString();
  }
  std::stringstream out;
  out
  << "Server:  " << crawler.serverToConnectTo
  << " port: " << crawler.portOrService
  << " resource: " << crawler.addressToConnectTo
  << "<br>";
  crawler.flagDoUseGET = false;
  crawler.fetchWebPage(&out, &out);
  out << "<br>" << crawler.lastTransactionErrors << "<hr>" << crawler.lastTransaction;
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerFetchKnownPublicKeys(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFetchKnownPublicKeys");
  (void) input;
  std::stringstream out;
  if (!global.userDefaultHasAdminRights()) {
    out << "You need to be a logged-in administrator to call this function. ";
    return output.assignValue(out.str(), calculator);
  }
  WebCrawler crawler;
  crawler.theTSL.openSSLData.name = "crawler";
  crawler.updatePublicKeys(&out, &out);
  crawler.closeEverything();
  global << "Closed web connection." << Logger::endL;
  return output.assignValue(out.str(), calculator);
}

void WebCrawler::updatePublicKeys(std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral) {
  MacroRegisterFunctionWithName("WebCrawler::updatePublicKeys");
  this->serverToConnectTo  = "www.googleapis.com";
  this->portOrService      = "https";
  this->addressToConnectTo = "https://www.googleapis.com/oauth2/v3/certs";
  this->flagDoUseGET = true;
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "<hr>" << "Updating public keys <hr>";
  }
  this->fetchWebPage(commentsOnFailure, commentsGeneral);
  if (this->bodyReceived == "") {
    global << Logger::red << "Could not fetch the google public keys ..." << Logger::endL;
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Could not fetch google certificate list. ";
    }
    return;
  }
  std::string googleKeysFileName = "certificates-public/google.txt";
  std::string googleKeysDebugFileName = "certificates-public/debug-google.txt";
  if (!FileOperations::writeFileVirualWithPermissions(
    googleKeysFileName, this->bodyReceived, true, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<br>Failed to open: " << googleKeysFileName;
    }
    global << Logger::red << "Failed to create google keys file name. " << Logger::endL;
    return;
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "<br>Updated file: " << googleKeysFileName;
  }
  global << Logger::green << "Updated public key file: " << googleKeysFileName << Logger::endL;
  std::stringstream debugData;
  debugData
  << "Expected body length: " << this->expectedLength.toString() << "\n";
  if (this->flagContinueWasNeeded) {
    debugData << "Did send a continue message.\n";
  } else {
    debugData << "Did NOT send a continue message.\n";
  }
  debugData << "\nBody received with header, "
  << "length: " << this->bodyReceivedWithHeader.size()
  << ":\n"
  << this->bodyReceivedWithHeader
  << "\nBody received, after header "
  << "length: " << this->bodyReceived.size()
  << ":\n"
  << this->bodyReceived
  << "\nBody received, beyond expected length: "
  << this->bodyReceivedOutsideOfExpectedLength
  << "\n"
  << "\nHeader:\n "
  << this->headerReceived;
  FileOperations::writeFileVirualWithPermissions(googleKeysDebugFileName, debugData.str(), true, commentsOnFailure);
}

bool Crypto::verifyJWTagainstKnownKeys(
  const std::string& inputToken,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  MacroRegisterFunctionWithName("Crypto::verifyJWTagainstKnownKeys");
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
  if (!header.parse(theToken.headerJSON)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Couldn't load JSON from the user token.";
    }
    return false;
  }
  if (header.elementType == JSData::token::tokenObject) {
    if (header.hasKey("kid")) {
      keyIDstring = header.getValue("kid").stringValue;
    }
  }
  if (keyIDstring == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Couldn't find key id in the token.";
    }
    return false;
  }
  int index = - 1;
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "Seeking key: <b style =\"color:brown\">" << keyIDstring << "</b>. ";
  }
  for (int i = 0; i < 2; i ++) {
    Crypto::loadKnownCertificates(commentsOnFailure, commentsGeneral);
    for (int j = 0; j < Crypto::knownCertificates.size; j ++) {
      if (keyIDstring == Crypto::knownCertificates[j].keyid) {
        index = j;
        break;
      }
    }
    if (index != - 1 || i == 1) {
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
    theCrawler.updatePublicKeys(commentsOnFailure, commentsGeneral);
  }
  if (index == - 1) {
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
  PublicKeyRSA& currentCert = Crypto::knownCertificates[index];
  return theToken.verifyRSA256(
    currentCert.theModulus, currentCert.theExponent, commentsOnFailure, commentsGeneral
  );
}

bool WebCrawler::verifyRecaptcha(
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
  if (!FileOperations::loadFiletoStringVirtual_AccessUltraSensitiveFoldersIfNeeded(
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
  this->fetchWebPage(commentsOnFailure, commentsGeneralSensitive);
  std::string response = this->bodyReceived;
  JSData theJSparser;
  if (!theJSparser.parse(response, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "<b style ='color:red'>" << "Failed to extract response token from captcha verification. "
      << "</b>"
      << "<br>The response string was: "
      << response;
    }
    return 0;
  }
  if (!theJSparser.hasKey("success")) {
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
  if (theSuccess.elementType != JSData::token::tokenBool || theSuccess.booleanValue != true) {
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
    return global.response.writeResponse(result, false);
  }
  std::stringstream out;
  if (!global.userDefaultHasAdminRights()) {
    Database::get().theUser.logoutViaDatabase();
  }
  UserCalculator theUser;
  theUser.email = HtmlRoutines::convertURLStringToNormal(global.getWebInput("email"), false);
  WebCrawler theCrawler;
  out << "<br><b> "
  << "Please excuse our verbose technical messages.</b>"
  << "<br><b>We are still testing our system; "
  << "we will remove the technical garbage as soon as we are done. "
  << "</b><br>\n";
  if (!theCrawler.verifyRecaptcha(&out, &out, nullptr)) {
    result[WebAPI::result::comments] = out.str();
    return global.response.writeResponse(result, false);
  }
  if (!theUser.exists(&out)) {
    out << "<br><b style ='color:red'>"
    << "We failed to find your email: " << theUser.email << " in our records. "
    << "</b>";
    result[WebAPI::result::comments] = out.str();
    return global.response.writeResponse(result, false);
  }
  if (!theUser.loadFromDatabase(&out, &out)) {
    out << "<br><b style='color:red'>"
    << "Failed to fetch user info for email: " << theUser.email
    << "</b>";
    result[WebAPI::result::comments] = out.str();
    return global.response.writeResponse(result, false);
  }
  out << "<b style ='color:green'>"
  << "Your email is on record. "
  << "</b>";
  if (!global.userDefaultHasAdminRights()) {
    this->owner->doSetEmail(theUser, &out, &out, nullptr);
  } else {
    this->owner->doSetEmail(theUser, &out, &out, &out);
  }
  out << "<br>Response time: " << global.getElapsedSeconds() << " second(s); "
  << global.getElapsedSeconds() << " second(s) spent creating account. ";
  result[WebAPI::result::comments] = out.str();
  return global.response.writeResponse(result, false);
}

JSData WebWorker::getSignUpRequestResult() {
  MacroRegisterFunctionWithName("WebWorker::getSignUpRequestResult");
  JSData result;
  std::stringstream errorStream;
  if (!global.flagDatabaseCompiled) {
    result["error"] = "Database not available (cannot sign up). ";
    return result;
  }
  Database::get().theUser.logoutViaDatabase();
  UserCalculator theUser;
  theUser.username = HtmlRoutines::convertURLStringToNormal(global.getWebInput("desiredUsername"), false);
  theUser.email = HtmlRoutines::convertURLStringToNormal(global.getWebInput("email"), false);
  std::stringstream generalCommentsStream;
  std::stringstream outputStream;
  generalCommentsStream
  << "<b>Please excuse our technical messages, they will be removed soon.</b>";
  WebCrawler theCrawler;
  if (!theCrawler.verifyRecaptcha(&errorStream, &generalCommentsStream, nullptr)) {
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
  if (global.userDefaultHasAdminRights()) {
    adminOutputStream = &generalCommentsStream;
  }
  //int fixThis;
  //adminOutputStream = &generalCommentsStream;
  this->doSetEmail(theUser, &errorStream, &generalCommentsStream, adminOutputStream);
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
      toWrite = StringRoutines::replaceAll(toWrite, WebAPIResponse::youHaveReachedTheBackend, outLinkApp.str());
    }
  }  
  return this->writeToBody(toWrite);
}

bool GlobalVariables::Response::writeResponse(const JSData& incoming, bool isCrash) {
  MutexlockGuard guard(global.mutexReturnBytes);
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
  MutexlockGuard guard(global.mutexReturnBytes);
  MacroRegisterFunctionWithName("GlobalVariables::Progress::report");
  return global.server().getActiveWorker().writeAfterTimeoutProgress(input, false);
}

void GlobalVariables::Response::initiate(const std::string& message) {
  // TODO(tmilev): investigate the performance of this snippet
  if (global.response.flagTimedOut) {
    return;
  }
  MutexlockGuard guard(global.mutexReturnBytes);
  MacroRegisterFunctionWithName("GlobalVariables::Progress::initiate");
  if (!global.response.monitoringAllowed()) {
    return;
  }
  if (!global.response.flagReportDesired) {
    return;
  }
  global.response.flagTimedOut = true;
  global.server().getActiveWorker().writeAfterTimeoutShowIndicator(message);
}
