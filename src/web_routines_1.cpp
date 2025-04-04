#include "calculator_inner_functions.h"
#include "crypto_calculator.h"
#include "database.h"
#include "general_file_operations_encodings.h"
#include "general_time_date.h"
#include "network_calculator.h"
#include "string_constants.h"
#include "user.h"
#include "web_client.h"
#include "webserver.h"
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class WebServerMonitor {
public:
  void backupDatabaseIfNeeded();
  double timeAtLastBackup;
  int pidServer;
  void monitor(int pidServer, const std::string& pingAuthentication);
  void stop();
  WebServerMonitor();
};

// A function to monitor the web server and restart if goes non-responsive.
// This function is extern'ed, so not wrapping up in a class.
void monitorWebServer(int pidServer, const std::string& pingAuthentication) {
  WebServerMonitor monitor;
  monitor.monitor(pidServer, pingAuthentication);
}

WebServerMonitor::WebServerMonitor() {
  this->timeAtLastBackup = - 1;
  this->pidServer = - 1;
}

void WebServerMonitor::backupDatabaseIfNeeded() {
  STACK_TRACE("WebServer::backupDatabaseIfNeeded");
  if (
    this->timeAtLastBackup > 0 &&
    global.getElapsedSeconds() - this->timeAtLastBackup < (24* 3600)
  ) {
    return;
  }
  std::stringstream commandStream;
  commandStream
  << "mongodump --db calculator --archive ="
  << global.physicalPathProjectBase
  << "database_backups/mongo"
  << global.getDateForLogFiles()
  << ".mongo";
  global
  << Logger::orange
  << "Backing up database with command: "
  << Logger::endL;
  global << commandStream.str() << Logger::endL;
  global.externalCommandReturnOutput(commandStream.str());
  global << Logger::green << "Backing up completed. " << Logger::endL;
  this->timeAtLastBackup = global.getElapsedSeconds();
}

void WebServerMonitor::monitor(
  int pidServer, const std::string& pingAuthentication
) {
  if (!global.flagLocalhostConnectionMonitor) {
    return;
  }
  this->pidServer = pidServer;
  STACK_TRACE("Monitor");
  int maximumPingFailures = 3;
  // warning: setting global.server().WebServerPingIntervalInSeconds to more
  // than 1000
  // may overflow the variable int microsecondsToSleep.
  int microsecondsToSleep =
  1000000 * global.server().webServerPingIntervalInSeconds;
  //
  if (global.server().webServerPingIntervalInSeconds > 30) {
    global
    << Logger::red
    << "**********WARNING**************"
    << Logger::endL
    << Logger::red
    << " The ping interval: "
    << global.server().webServerPingIntervalInSeconds
    << " is set to a large value. "
    << "Set the ping interval to less than 30 seconds to remove this message. "
    << Logger::endL;
  }
  std::fstream fileStream;
  FileOperations::openFileCreateIfNotPresentVirtual(
    fileStream,
    "/LogFiles/server_starts_and_unexpected_restarts.html",
    true,
    false,
    false,
    true
  );
  fileStream
  << "<a href=\"/LogFiles/logs/"
  << GlobalVariables::getDateForLogFiles()
  << "/\">"
  << GlobalVariables::getDateForLogFiles()
  << "</a>"
  << "<br>\n";
  fileStream.close();
  WebClient webCrawler;
  webCrawler.initialize();
  webCrawler.connectorContainer.getElement().name = "web_crawler_connector";
  global
  << Logger::blue
  << "Pinging "
  << webCrawler.connectorContainer.getElement().addressToConnectTo
  << " at port/service "
  << webCrawler.connectorContainer.getElement().portOrService
  << " every "
  << (microsecondsToSleep / 1000000)
  << " second(s). "
  << Logger::endL;
  global
  << Logger::red
  << "Please beware that the server will "
  << "restart and you will lose all computations "
  << "if "
  << maximumPingFailures
  << " consecutive pings fail. "
  << Logger::endL;
  int totalConsecutiveFailedPings = 0;
  int totalPings = 0;
  TimeWrapper now;
  for (;;) {
    global.fallAsleep(microsecondsToSleep);
    this->backupDatabaseIfNeeded();
    webCrawler.pingCalculatorStatus(pingAuthentication);
    totalPings ++;
    if (webCrawler.lastTransactionErrors != "") {
      now.assignLocalTime();
      totalConsecutiveFailedPings ++;
      global
      << Logger::red
      << "Connection monitor: ping of ["
      << webCrawler.connectorContainer.getElement().addressToConnectTo
      << "] at port/service ["
      << webCrawler.connectorContainer.getElement().portOrService
      << "] failed. GM time: "
      << now.toStringGM()
      << ", local time: "
      << now.toStringLocal()
      << ". "
      << "Errors: "
      << webCrawler.lastTransactionErrors
      << webCrawler.lastTransaction
      << totalConsecutiveFailedPings
      << " consecutive fails so far, restarting on "
      << maximumPingFailures
      << ". "
      << Logger::endL;
    } else {
      std::cout
      << "Connection monitor: ping #"
      << totalPings
      << ": received "
      << webCrawler.lastNumberOfBytesRead
      << " bytes. "
      << std::endl;
      totalConsecutiveFailedPings = 0;
    }
    if (totalConsecutiveFailedPings >= maximumPingFailures) {
      this->stop();
    }
  }
}

void WebServerMonitor::stop() {
  TimeWrapper now;
  now.assignLocalTime();
  global
  << Logger::red
  << "Server stopped responding. "
  << "Logging this event and stopping the monitor."
  << Logger::endL;
  std::fstream fileStream;
  FileOperations::openFileCreateIfNotPresentVirtual(
    fileStream,
    "LogFiles/server_starts_and_unexpected_restarts.html",
    true,
    false,
    false,
    true
  );
  fileStream
  << "<b style ='color:red'>"
  << "Unexpected server restart: server "
  << "stopped responding (locked pipe?). Time: local: "
  << now.toStringLocal()
  << ", GM: "
  << now.toStringGM()
  << "</b><br>\n";
  fileStream.flush();
  std::stringstream killServerChildrenCommand;
  killServerChildrenCommand << "pkill -9 -P " << this->pidServer;
  global
  << "Terminating server children with command: "
  << killServerChildrenCommand.str()
  << Logger::endL;
  global.externalCommandNoOutput(killServerChildrenCommand.str(), true);
  global
  << Logger::red
  << "Terminating server with pid: "
  << this->pidServer
  << Logger::endL;
  WebServer::terminateProcessId(this->pidServer);
  global
  << Logger::red
  << "Restarting monitor. "
  << this->pidServer
  << Logger::endL;
  global.server().stopKillAll();
}

WebClient::WebClient() {
  this->flagInitialized = false;
  this->lastNumberOfBytesRead = 0;
}

void WebClient::freeAddressInfo() {
  this->connectorContainer.getElement().freeAddressInformation();
}

void WebClient::initialize() {
  if (this->flagInitialized) {
    return;
  }
  this->flagContinueWasNeeded = false;
  STACK_TRACE("WebCrawler::initialize");
  this->flagDoUseGET = true;
  this->buffer.initializeFillInObject(50000, 0);
  this->peerAddress = "127.0.0.1";
  this->desiredPortOrService =
  global.configuration[Configuration::portHTTP].stringValue;
}

WebClient::~WebClient() {
  this->closeEverything();
}

void WebClient::closeEverything() {
  if (!this->flagInitialized) {
    return;
  }
  this->flagInitialized = false;
  this->connectorContainer.getElement().closeSocket();
  this->connectorContainer.getElement().freeAddressInformation();
}

void WebClient::pingCalculatorStatus(const std::string& pingAuthentication) {
  STACK_TRACE("WebClient::pingCalculatorStatus");
  std::stringstream reportStream;
  this->lastTransaction = "";
  this->lastTransactionErrors = "";
  Connector& connector = this->connectorContainer.getElement();
  connector.initialize(this->peerAddress, this->desiredPortOrService);
  std::stringstream comments;
  if (!connector.connectWrapper(&comments)) {
    this->lastTransactionErrors = comments.str();
    return;
  }
  std::string getMessage =
  "GET /cgi-bin/calculator?request=" + pingAuthentication;
  std::stringstream errorStream1;
  int numberOfBytesWritten =
  Pipe::writeWithTimeoutViaSelect(
    connector.socketInteger, getMessage, 1, 10, &errorStream1
  );
  if (static_cast<unsigned>(numberOfBytesWritten) != getMessage.size()) {
    this->lastTransactionErrors +=
    "\nERROR writing to socket. " + errorStream1.str();
    connector.closeSocket();
  }
  std::stringstream errorStream2;
  this->lastNumberOfBytesRead =
  Pipe::readWithTimeOutViaSelectOneFileDescriptor(
    connector.socketInteger, this->buffer, 10000, 1, 10, &errorStream2
  );
  if (this->lastNumberOfBytesRead < 0) {
    this->lastTransactionErrors +=
    "ERROR reading from socket. " + errorStream2.str();
    connector.closeSocket();
  }
  std::string readString;
  readString.assign(
    buffer.objects, static_cast<unsigned>(this->lastNumberOfBytesRead)
  );
  reportStream
  << "Wrote "
  << numberOfBytesWritten
  << ", read "
  << this->lastNumberOfBytesRead
  << " bytes: "
  << readString
  << ". ";
  this->lastTransaction = reportStream.str();
  connector.closeEverything();
}

void WebClient::fetchWebPage(
  std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
) {
  STACK_TRACE("WebCrawler::fetchWebPage");
  (void) commentsOnFailure;
  (void) commentsGeneral;
  this->transportLayerSecurity.getOpenSSLData().checkCanInitializeToClient();
  this->transportLayerSecurity.initializeNonThreadSafeOnFirstCall(false);
#ifdef MACRO_use_open_ssl
  this->lastTransaction = "";
  this->lastTransactionErrors = "";
  Connector& connector = this->connectorContainer.getElement();
  connector.initialize(this->peerAddress, this->desiredPortOrService);
  if (!connector.connectWrapper(commentsOnFailure)) {
    this->lastTransactionErrors = commentsOnFailure->str();
    return;
  }
  this->fetchWebPagePart2(commentsOnFailure, commentsGeneral);
  this->transportLayerSecurity.removeLastSocket();
  connector.closeEverything();
#endif //Macro_use_openssl
}

void WebClient::fetchWebPagePart2(
  std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
) {
  STACK_TRACE("WebCrawler::fetchWebPagePart2");
  (void) commentsOnFailure;
  (void) commentsGeneral;
#ifdef MACRO_use_open_ssl
  std::stringstream messageHeader;
  std::stringstream continueHeader;
  Connector& connector = this->connectorContainer.getElement();
  if (this->flagDoUseGET) {
    messageHeader
    << "GET "
    << this->url
    << " HTTP/1.0"
    << "\r\n"
    << "Host: "
    << connector.addressToConnectTo
    << "\r\n\r\n";
  } else {
    messageHeader
    << "POST "
    << this->url
    << " HTTP/1.0"
    << "\r\n"
    << "Host: "
    << connector.addressToConnectTo;
    messageHeader << "\r\nContent-length: " << this->postMessageToSend.size();
    messageHeader << "\r\n\r\n";
    messageHeader << this->postMessageToSend;
  }
  this->transportLayerSecurity.getOpenSSLData().checkCanInitializeToClient();
  if (
    !this->transportLayerSecurity.handShakeIAmClientNoSocketCleanup(
      connector.socketInteger, commentsOnFailure, commentsGeneral
    )
  ) {
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
  if (
    !this->transportLayerSecurity.sslWriteLoop(
      10, messageHeader.str(), &errorSSL, commentsGeneral, true
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "SSL critical error: " << errorSSL;
    }
    return;
  }
  if (
    !this->transportLayerSecurity.sslReadLoop(
      10, this->headerReceived, 0, &errorSSL, commentsGeneral, true
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "SSL critical error: " << errorSSL;
    }
    return;
  }
  unsigned bodyStart = 0;
  int totalCrLfs = 0;
  for (; bodyStart < this->headerReceived.size(); bodyStart ++) {
    if (totalCrLfs >= 4) {
      break;
    }
    if (
      this->headerReceived[bodyStart] == '\n' ||
      this->headerReceived[bodyStart] == '\r'
    ) {
      totalCrLfs ++;
    } else {
      totalCrLfs = 0;
    }
  }
  if (bodyStart < this->headerReceived.size()) {
    this->bodyReceivedWithHeader = this->headerReceived.substr(bodyStart);
    this->headerReceived = this->headerReceived.substr(0, bodyStart);
  }
  List<std::string> headerPieces;
  StringRoutines::stringSplitDefaultDelimiters(
    this->headerReceived, headerPieces
  );
  std::string expectedLengthString;
  for (int i = 0; i < headerPieces.size; i ++) {
    if (
      headerPieces[i] == "Content-length:" ||
      headerPieces[i] == "Content-Length:" ||
      headerPieces[i] == "content-length:"
    ) {
      if (i + 1 < headerPieces.size) {
        expectedLengthString = headerPieces[i + 1];
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
      *commentsOnFailure
      << "No 200 ok message found. Headers received: "
      << this->headerReceived;
    }
    return;
  }
  if (
    expectedLength == static_cast<int64_t>(
      this->bodyReceivedWithHeader.size()
    )
  ) {
    this->bodyReceived = this->bodyReceivedWithHeader;
    return;
  }
  this->flagContinueWasNeeded = true;
  continueHeader << "HTTP/1.0 100 Continue\r\n\r\n";
  if (
    !this->transportLayerSecurity.sslWriteLoop(
      10, continueHeader.str(), &errorSSL, commentsGeneral, true
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "SSL critical error: " << errorSSL;
    }
    return;
  }
  std::string secondPart;
  if (
    !this->transportLayerSecurity.sslReadLoop(
      10, secondPart, expectedLength, &errorSSL, commentsGeneral, true
    )
  ) {
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
    this->bodyReceivedOutsideOfExpectedLength =
    this->bodyReceived.substr(static_cast<unsigned>(length));
    this->bodyReceived =
    this->bodyReceived.substr(0, static_cast<unsigned>(length));
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral
    << "<br>Body (length: "
    << this->bodyReceived.size()
    << ")<br>"
    << this->bodyReceived;
    if (this->bodyReceivedOutsideOfExpectedLength.size() == 0) {
      *commentsGeneral
      << "<br><b style='color:green'>No extraneous data received</b>";
    } else {
      *commentsGeneral
      << "<br><b style='color:red'>Received more data than expected "
      << "(perhaps due to a protocol error?).</b>"
      << "<br>"
      << this->bodyReceivedOutsideOfExpectedLength;
    }
  }
  this->transportLayerSecurity.free();
#endif
}

bool CalculatorFunctions::fetchWebPageGET(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::fetchWebPageGET");
  if (!global.userDefaultHasAdminRights()) {
    return
    output.assignValue(
      calculator,
      std::string("Fetching web pages available only for logged-in admins. ")
    );
  }
  WebClient webClient;
  if (input.size() != 4) {
    return
    calculator
    << "Fetching web page expects 3 arguments: "
    << "server, service/port, and webpage. ";
  }
  if (!input[1].isOfType(&webClient.peerAddress)) {
    webClient.peerAddress = input[1].toString();
  }
  if (!input[2].isOfType(&webClient.desiredPortOrService)) {
    webClient.desiredPortOrService = input[2].toString();
  }
  if (!input[3].isOfType(&webClient.url)) {
    webClient.peerAddress = input[3].toString();
  }
  std::stringstream out;
  webClient.flagDoUseGET = true;
  out << webClient.toString();
  webClient.fetchWebPage(&out, &out);
  out
  << "<br>"
  << webClient.lastTransactionErrors
  << "<hr>"
  << webClient.lastTransaction;
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctions::fetchWebPagePOST(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::fetchWebPagePOST");
  if (!global.userDefaultHasAdminRights()) {
    return
    output.assignValue(
      calculator,
      std::string("Fetching web pages available only for logged-in admins. ")
    );
  }
  WebClient crawler;
  if (input.size() != 5) {
    return
    calculator
    << "Fetching web page expects 4 arguments: "
    << "server, service/port, webpage and message to post. ";
  }
  if (!input[1].isOfType(&crawler.peerAddress)) {
    crawler.peerAddress = input[1].toString();
  }
  if (!input[2].isOfType(&crawler.desiredPortOrService)) {
    crawler.desiredPortOrService = input[2].toString();
  }
  if (!input[3].isOfType(&crawler.url)) {
    crawler.url = input[3].toString();
  }
  if (!input[4].isOfType(&crawler.postMessageToSend)) {
    crawler.postMessageToSend = input[4].toString();
  }
  std::stringstream out;
  out << crawler.toString() << "<br>";
  crawler.flagDoUseGET = false;
  crawler.fetchWebPage(&out, &out);
  out
  << "<br>"
  << crawler.lastTransactionErrors
  << "<hr>"
  << crawler.lastTransaction;
  return output.assignValue(calculator, out.str());
}

std::string WebClient::toString() const {
  std::stringstream out;
  out
  << "Peer:  "
  << this->peerAddress
  << " port: "
  << this->desiredPortOrService
  << " url: "
  << this->url;
  return out.str();
}

bool CalculatorFunctions::fetchKnownPublicKeys(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::fetchKnownPublicKeys");
  (void) input;
  std::stringstream out;
  if (!global.userDefaultHasAdminRights()) {
    out << "You need to be a logged-in administrator to call this function. ";
    return output.assignValue(calculator, out.str());
  }
  WebClient crawler;
  crawler.transportLayerSecurity.getOpenSSLData().name = "crawler";
  crawler.updatePublicKeys(&out, &out);
  crawler.closeEverything();
  global << "Closed web connection." << Logger::endL;
  return output.assignValue(calculator, out.str());
}

void WebClient::updatePublicKeys(
  std::stringstream* commentsOnFailure, std::stringstream* commentsGeneral
) {
  STACK_TRACE("WebCrawler::updatePublicKeys");
  this->peerAddress = "www.googleapis.com";
  this->desiredPortOrService = "https";
  this->url = "https://www.googleapis.com/oauth2/v3/certs";
  this->flagDoUseGET = true;
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "<hr>" << "Updating public keys <hr>";
  }
  this->fetchWebPage(commentsOnFailure, commentsGeneral);
  if (this->bodyReceived == "") {
    global
    << Logger::red
    << "Could not fetch the google public keys ..."
    << Logger::endL;
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Could not fetch google certificate list. ";
    }
    return;
  }
  std::string googleKeysFileName = "certificates-public/google.txt";
  std::string googleKeysDebugFileName = "certificates-public/debug-google.txt";
  if (
    !FileOperations::writeFileVirualWithPermissions(
      googleKeysFileName, this->bodyReceived, true, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<br>Failed to open: " << googleKeysFileName;
    }
    global
    << Logger::red
    << "Failed to create google keys file name. "
    << Logger::endL;
    return;
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral << "<br>Updated file: " << googleKeysFileName;
  }
  global
  << Logger::green
  << "Updated public key file: "
  << googleKeysFileName
  << Logger::endL;
  std::stringstream debugData;
  debugData
  << "Expected body length: "
  << this->expectedLength.toString()
  << "\n";
  if (this->flagContinueWasNeeded) {
    debugData << "Did send a continue message.\n";
  } else {
    debugData << "Did NOT send a continue message.\n";
  }
  debugData
  << "\nBody received with header, "
  << "length: "
  << this->bodyReceivedWithHeader.size()
  << ":\n"
  << this->bodyReceivedWithHeader
  << "\nBody received, after header "
  << "length: "
  << this->bodyReceived.size()
  << ":\n"
  << this->bodyReceived
  << "\nBody received, beyond expected length: "
  << this->bodyReceivedOutsideOfExpectedLength
  << "\n"
  << "\nHeader:\n "
  << this->headerReceived;
  FileOperations::writeFileVirualWithPermissions(
    googleKeysDebugFileName, debugData.str(), true, commentsOnFailure
  );
}

bool Crypto::verifyJWTagainstKnownKeys(
  const std::string& inputToken,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneral
) {
  STACK_TRACE("Crypto::verifyJWTagainstKnownKeys");
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
  JSONWebToken webToken;
  if (!webToken.assignString(inputToken, commentsOnFailure)) {
    return false;
  }
  std::string keyIDstring = "";
  JSData header;
  if (!header.parse(webToken.headerJSON)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Couldn't load JSON from the user token.";
    }
    return false;
  }
  if (header.elementType == JSData::Type::tokenObject) {
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
    *commentsGeneral
    << "Seeking key: <b style =\"color:brown\">"
    << keyIDstring
    << "</b>. ";
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
      *commentsGeneral
      << "<br><b style =\"color:red\">Couldn't find key ID: "
      << keyIDstring
      << " from cached certificate.</b>";
    }
    if (commentsGeneral != nullptr) {
      *commentsGeneral << "<br>Reloading google public keys. ";
    }
    WebClient webClient;
    webClient.transportLayerSecurity.getOpenSSLData().name =
    "public key fetcher";
    webClient.updatePublicKeys(commentsOnFailure, commentsGeneral);
  }
  if (index == - 1) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "<b style =\"color:red\">Could not find key id: "
      << keyIDstring
      << "</b>. ";
    }
    return false;
  }
  if (commentsGeneral != nullptr) {
    *commentsGeneral
    << "<b style =\"color:green\">Found key id: "
    << keyIDstring
    << ".</b>";
  }
  PublicKeyRSA& currentCertificate = Crypto::knownCertificates[index];
  return
  webToken.verifyRSA256(
    currentCertificate.modulus,
    currentCertificate.exponent,
    commentsOnFailure,
    commentsGeneral
  );
}

bool WebAPIResponse::verifyRecaptcha(
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNONsensitive,
  std::stringstream* commentsGeneralSensitive
) {
  STACK_TRACE("WebAPIResponse::verifyRecaptcha");
  if (global.flagDebugLogin) {
    if (commentsGeneralNONsensitive != nullptr) {
      *commentsGeneralNONsensitive
      << "Debugging your login: "
      << "not verifying your recaptcha. ";
    }
    return true;
  }
  std::stringstream messageToSendStream;
  std::string secret;
  std::stringstream notUsed;
  if (commentsOnFailure == nullptr) {
    commentsOnFailure = &notUsed;
  }
  if (
    !FileOperations::
    loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(
      "certificates/recaptcha-secret.txt",
      secret,
      true,
      true,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load recaptcha secret.";
    }
    return false;
  }
  std::string recaptchaURLencoded = global.getWebInput("recaptchaToken");
  if (commentsGeneralSensitive != nullptr) {
    *commentsGeneralSensitive << "Recaptcha: " << recaptchaURLencoded;
  }
  if (recaptchaURLencoded == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "<b style ='color:red'>Recaptcha appears to be missing. </b>";
    }
    return false;
  }
  messageToSendStream
  << "response="
  << recaptchaURLencoded
  << "&"
  << "secret="
  << secret;
  WebClient webClient;
  webClient.flagDoUseGET = true;
  webClient.url = "https://www.google.com/recaptcha/api/siteverify";
  webClient.url += "?" + messageToSendStream.str();
  webClient.peerAddress = "www.google.com";
  webClient.desiredPortOrService = "https";
  webClient.postMessageToSend = messageToSendStream.str();
  webClient.fetchWebPage(commentsOnFailure, commentsGeneralSensitive);
  std::string response = webClient.bodyReceived;
  JSData jsonParser;
  if (!jsonParser.parse(response, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "<b style ='color:red'>"
      << "Failed to extract response token from captcha verification. "
      << "</b>"
      << "<br>The response string was: "
      << response;
    }
    return 0;
  }
  if (!jsonParser.hasKey("success")) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "<b style ='color:red'>"
      << "Captcha failure: could not find key 'success'."
      << "</b>";
    }
    return false;
  }
  JSData success;
  success = jsonParser.getValue("success");
  if (
    success.elementType != JSData::Type::tokenBool ||
    success.booleanValue != true
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Could not verify your captcha solution. "
      << "The response from google was: "
      << response;
    }
    return false;
  } else {
    if (commentsGeneralNONsensitive != nullptr) {
      *commentsGeneralNONsensitive
      << "Your recaptcha answer appears to be valid. ";
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
  STACK_TRACE("WebAPIResponse::processForgotLogin");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  this->forgotLogin(result);
  return global.response.writeResponse(result, false);
}

void WebAPIResponse::forgotLogin(JSData& result) {
  STACK_TRACE("WebAPIResponse::forgotLogin");
  std::stringstream out;
  if (!global.userDefaultHasAdminRights()) {
    Database::get().user.logoutViaDatabase();
  }
  UserCalculator user;
  user.email =
  HtmlRoutines::convertURLStringToNormal(global.getWebInput("email"), false);
  if (!WebAPIResponse::verifyRecaptcha(&out, &out, nullptr)) {
    result[WebAPI::Result::comments] = out.str();
    return;
  }
  bool userExists = false;
  bool databaseIsOK = false;
  user.exists(userExists, databaseIsOK, &out);
  if (!databaseIsOK) {
    out << "Database is down. ";
    result[WebAPI::Result::error] = out.str();
    return;
  }
  if (!userExists) {
    out
    << "We failed to find your email: ["
    << user.email
    << "] in our records. ";
    result[WebAPI::Result::comments] = out.str();
    return;
  }
  if (!user.loadFromDatabase(&out, &out)) {
    out << "Failed to fetch user info for email: " << user.email << ". ";
    result[WebAPI::Result::comments] = out.str();
    return;
  }
  out << "Your email is on record. ";
  std::stringstream commentsOnError;
  std::stringstream* commentsOnErrorPointer = nullptr;
  bool success = false;
  if (global.flagDebugLogin) {
    out << "Setting email with debug information. ";
    commentsOnErrorPointer = &commentsOnError;
  }
  success =
  this->doSetEmail(
    false, user, &commentsOnError, &out, commentsOnErrorPointer
  );
  if (!success) {
    result[WebAPI::Result::error] = commentsOnError.str();
  }
  out << "Response time: " << global.getElapsedSeconds() << " second(s). ";
  result[WebAPI::Result::comments] = out.str();
}

bool WebWorker::writeToBodyJSON(const JSData& result) {
  std::string toWrite =
  HtmlRoutines::convertStringToHtmlString(result.toString(nullptr), false);
  if (toWrite.size() < 2000) {
    if (
      toWrite.find(WebAPIResponse::youHaveReachedTheBackend) !=
      std::string::npos
    ) {
      std::string sanitizedCalculatorApp =
      HtmlRoutines::convertStringToHtmlString(
        global.displayApplication, false
      );
      std::stringstream outLinkApp;
      outLinkApp
      << "You've reached the calculator's backend. "
      << "The app can be accessed here: <a href = '"
      << sanitizedCalculatorApp
      << "'>app</a>";
      toWrite =
      StringRoutines::replaceAll(
        toWrite, WebAPIResponse::youHaveReachedTheBackend, outLinkApp.str()
      );
    }
  }
  return this->writeToBody(toWrite);
}

bool GlobalVariables::Response::writeResponse(
  const JSData& incoming, bool isCrash
) {
  MutexlockGuard guard(global.mutexReturnBytes);
  STACK_TRACE("GlobalVariables::Response::writeResponse");
  if (global.runMode != GlobalVariables::RunMode::builtInWebServer) {
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
    output[WebAPI::Result::commentsGlobal] = comments;
  }
  if (global.userDebugFlagOn()) {
    output[WebAPI::Result::commentsDebug] = worker.toStringMessageShort();
  }
  if (this->flagTimedOut) {
    worker.writeAfterTimeoutJSON(
      output, status, global.relativePhysicalNameOptionalResult
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

void GlobalVariables::Response::report(const JSData& input) {
  MutexlockGuard guard(global.mutexReturnBytes);
  STACK_TRACE("GlobalVariables::Progress::report");
  return
  global.server().getActiveWorker().writeAfterTimeoutProgress(input, false);
}

void GlobalVariables::Response::initiate(const std::string& message) {
  // TODO(tmilev): investigate the performance of this snippet
  if (global.response.flagTimedOut) {
    return;
  }
  MutexlockGuard guard(global.mutexReturnBytes);
  STACK_TRACE("GlobalVariables::Progress::initiate");
  if (!global.response.monitoringAllowed()) {
    return;
  }
  if (!global.response.flagReportDesired) {
    return;
  }
  global.response.flagTimedOut = true;
  global.server().getActiveWorker().writeAfterTimeoutShowIndicator(message);
}
