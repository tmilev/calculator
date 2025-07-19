#include "calculator_interface.h"
#include "crypto_calculator.h"
#include "database.h"
#include "general_file_operations_encodings.h"
#include "general_time_date.h"
#include "network_calculator.h"
#include "signals_infrastructure.h"
#include "string_constants.h"
#include "system_functions_global_objects.h"
#include "web_api.h"
#include "web_client.h"
#include "webserver.h"
#include <arpa/inet.h> // <- inet_ntop (ntop = network to presentation)
#include <fcntl.h>//<-setting flags of file descriptors
#include <iostream>
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <sys/resource.h> //<- setrlimit(...): restrict executable runtime
#include <sys/stat.h>//<-for file statistics
#include <sys/wait.h>//<-waitpid f-n here
#include <unistd.h>

const std::string WebServer::Statististics::pingRequestsString =
"pingRequests";
const std::string WebServer::Statististics::allRequestsString = "allRequests";
const std::string WebServer::Statististics::standaloneServerRequestsString =
"standaloneServerRequests";

WebServer& GlobalVariables::server() {
  static WebServer result;
  return result;
}

SignalsInfrastructure& SignalsInfrastructure::signals() {
  static SignalsInfrastructure result;
  return result;
}

SignalsInfrastructure::SignalsInfrastructure() {
  this->flagSignalsAreBlocked = false;
  this->flagSignalsAreStored = false;
  this->flagInitialized = false;
}

// This class locks/unlocks all signals within its scope
class SignalLock {
  SignalLock() {
    SignalsInfrastructure::signals().blockSignals();
  }
  ~SignalLock() {
    SignalsInfrastructure::signals().unblockSignals();
  }
};

void SignalsInfrastructure::unblockSignals() {
  int error = sigprocmask(SIG_SETMASK, &oldSignals, nullptr);
  if (error < 0) {
    global
    << "Sigprocmask failed on server, I shall now crash. "
    << Logger::endL;
    global.fatal << "Sigprocmask failed on server." << global.fatal;
  }
  this->flagSignalsAreBlocked = false;
  if (global.flagServerDetailedLog) {
    global << Logger::green << "Detail: unmask successful. " << Logger::endL;
  }
}

void SignalsInfrastructure::blockSignals() {
  sigset_t* signals = nullptr;
  if (!this->flagSignalsAreStored) {
    // Store signals on first run.
    signals = &this->oldSignals;
  }
  int error = sigprocmask(SIG_BLOCK, &this->allSignals, signals);
  this->flagSignalsAreStored = true;
  if (error < 0) {
    global
    << Logger::red
    << "Fatal error: sigprocmask failed. "
    << "The server is going to crash. "
    << Logger::endL;
    global.fatal
    << "Sigprocmas failed. "
    << "This should not happen. "
    << global.fatal;
  }
  this->flagSignalsAreBlocked = true;
}

bool WebWorker::checkConsistency() {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of webworker." << global.fatal;
  }
  if (this->parent == nullptr) {
    global.fatal << "Parent of web worker is not initialized." << global.fatal;
  }
  if (this->indexInParent == - 1) {
    global.fatal << "Index in parent is bad. " << global.fatal;
  }
  if (this->connectionID == - 1) {
    global.fatal << "Connection id is bad. " << global.fatal;
  }
  return true;
}

std::string WebWorker::openIndentTag(const std::string& tag) {
  std::stringstream out;
  for (int i = 0; i < this->indentationLevelHTML; i ++) {
    out << "  ";
  }
  out << tag << "\n";
  this->indentationLevelHTML ++;
  return out.str();
}

std::string WebWorker::closeIndentTag(const std::string& tag) {
  std::stringstream out;
  this->indentationLevelHTML --;
  for (int i = 0; i < this->indentationLevelHTML; i ++) {
    out << "  ";
  }
  out << tag << "\n";
  return out.str();
}

void WebServer::initializeSSL() {
  STACK_TRACE("WebServer::initializeSSL");
  if (!global.flagSSLAvailable) {
    global << Logger::red << "SSL is DISABLED." << Logger::endL;
    return;
  }
  this->transportLayerSecurity.initializeNonThreadSafeOnFirstCall(true);
}

bool WebServer::sslServerSideHandShake(std::stringstream* commentsOnFailure) {
  if (!global.flagSSLAvailable) {
    return false;
  }
  if (!global.flagUsingSSLinCurrentConnection) {
    return false;
  }
  return
  this->transportLayerSecurity.handShakeIamServer(
    this->getActiveWorker().connectedSocketID, commentsOnFailure
  );
}

bool WebWorker::receiveAll() {
  STACK_TRACE("WebWorker::receiveAll");
  this->messageBody = "";
  this->messageHead = "";
  this->requestType = this->requestUnknown;
  if (this->connectedSocketID == - 1) {
    global.fatal
    << "Attempting to receive on a socket with ID equal to - 1. "
    << global.fatal;
  }
  struct timeval tv;
  int failedReceives = 0;
  int maximumFailedReceives = 1;
  int secondsReplyAfter = static_cast<int>(
    global.millisecondsReplyAfterComputation
  ) /
  1000;
  // The following formula ensures that all receive timeouts take 1 second less
  // than millisecondsReplyAfterComputation. This gives ample time (1 second)
  // for the calculator to take reset the computation time counter. This
  // prevents
  // the reply-after-computation
  // mechanism from racing with browser clients that keep connections open
  // without closing.
  tv.tv_sec = (secondsReplyAfter - 1) / (maximumFailedReceives + 1);
  if (tv.tv_sec < 1) {
    tv.tv_sec = 1;
  }
  tv.tv_usec = 0;
  setsockopt(
    this->connectedSocketID,
    SOL_SOCKET,
    SO_RCVTIMEO,
    static_cast<void*>(&tv),
    sizeof(timeval)
  );
  std::string errorString;
  double numberOfSecondsAtStart = global.getElapsedSeconds();
  int numberOfBytesInBuffer = 0;
  List<char>& readBuffer = this->parent->transportLayerSecurity.readBuffer;
  while (true) {
    numberOfBytesInBuffer =
    this->parent->transportLayerSecurity.readOnce(
      this->connectedSocketID, &errorString, nullptr, true
    );
    if (numberOfBytesInBuffer >= 0) {
      break;
    }
    failedReceives ++;
    if (failedReceives > maximumFailedReceives) {
      if (errorString == TransportLayerSecurityOpenSSL::errors::errorWantRead) {
        this->error = errorString;
      } else {
        std::stringstream out;
        out
        << "WebWorker::receiveAll on socket "
        << this->connectedSocketID
        << " failed (so far "
        << failedReceives
        << " fails). "
        << "Error: "
        << errorString
        << ". ";
        out << "Too many failed receives, aborting. ";
        this->error = out.str();
      }
      return false;
    }
  }
  this->messageHead.assign(
    readBuffer.objects, static_cast<unsigned>(numberOfBytesInBuffer)
  );
  this->parseMessageHead();
  if (this->requestType == WebWorker::requestTypes::requestPost) {
    this->displayUserInput = "POST " + this->addressGetOrPost;
  } else {
    this->displayUserInput = "GET " + this->addressGetOrPost;
  }
  if (this->contentLength <= 0) {
    return true;
  }
  if (this->messageBody.size() == static_cast<unsigned>(this->contentLength)) {
    return true;
  }
  this->messageBody.clear();
  // <- needed else the length error check will pop.
  if (this->contentLength > 10000000) {
    this->checkConsistency();
    this->error =
    "Content-length parsed to be "
    "more than 10 million bytes, aborting.";
    global << this->error << Logger::endL;
    this->displayUserInput = this->error;
    return false;
  }
  this->remainingBytesToSend = std::string("HTTP/1.0 100 Continue\r\n\r\n");
  this->sendAllBytesNoHeaders();
  this->remainingBytesToSend.setSize(0);
  std::string bufferString;
  while (static_cast<signed>(this->messageBody.size()) < this->contentLength) {
    if (global.getElapsedSeconds() - numberOfSecondsAtStart > 5) {
      this->error = "Receiving bytes timed out (5 seconds).";
      global << this->error << Logger::endL;
      this->displayUserInput = this->error;
      return false;
    }
    numberOfBytesInBuffer =
    this->parent->transportLayerSecurity.readOnce(
      this->connectedSocketID, &errorString, nullptr, true
    );
    if (numberOfBytesInBuffer == 0) {
      this->error =
      "While trying to fetch message-body, received 0 bytes. " +
      this->parent->toStringLastErrorDescription();
      this->displayUserInput = this->error;
      return false;
    }
    if (numberOfBytesInBuffer < 0) {
      if (
        errno == EAGAIN ||
        errno == EWOULDBLOCK ||
        errno == EINTR ||
        errno == EIO ||
        errno == ENOBUFS ||
        errno == ENOMEM
      ) {
        continue;
      }
      this->error =
      "Error fetching message body: " +
      this->parent->toStringLastErrorDescription();
      global << this->error << Logger::endL;
      this->displayUserInput = this->error;
      return false;
    }
    bufferString.assign(
      readBuffer.objects, static_cast<unsigned>(numberOfBytesInBuffer)
    );
    this->messageBody += bufferString;
  }
  if (static_cast<signed>(this->messageBody.size()) != this->contentLength) {
    this->messageHead += this->messageBody;
    this->parseMessageHead();
  }
  if (static_cast<signed>(this->messageBody.size()) != this->contentLength) {
    std::stringstream out;
    out
    << "The message-body received by me had length "
    << this->messageBody.size()
    << " yet I expected a message of length "
    << this->contentLength
    << ". More details follow. "
    << this->toStringMessageFull();
    global << out.str() << Logger::endL;
  }
  return true;
}

void WebWorker::sendAllBytesNoHeaders() {
  STACK_TRACE("WebWorker::sendAllBytesNoHeaders");
  this->checkConsistency();
  global
  << "Worker "
  << this->indexInParent + 1
  << " sending "
  << this->remainingBytesToSend.size
  << " bytes in chunks of: ";
  double startTime = global.getElapsedSeconds();
  struct timeval tv;
  tv.tv_sec = 5;
  // 5 Secs Timeout
  tv.tv_usec = 0;
  int numberOfTimesRunWithoutSending = 0;
  int timeOutInSeconds = 20;
  setsockopt(
    this->connectedSocketID,
    SOL_SOCKET,
    SO_SNDTIMEO,
    static_cast<void*>(&tv),
    sizeof(timeval)
  );
  std::string errorString;
  std::stringstream commentsOnError;
  while (this->remainingBytesToSend.size > 0) {
    if (global.getElapsedSeconds() - startTime > timeOutInSeconds) {
      global
      << "WebWorker::sendAllBytesNoHeaders failed: more than "
      << timeOutInSeconds
      << " seconds have elapsed. "
      << Logger::endL;
      return;
    }
    int bytesSent =
    this->parent->transportLayerSecurity.writeOnce(
      this->connectedSocketID,
      this->remainingBytesToSend,
      &errorString,
      nullptr,
      &commentsOnError,
      true
    );
    if (bytesSent < 0) {
      global
      << Logger::red
      << "WebWorker::sendAllBytesNoHeaders: writeOnce failed. "
      << Logger::blue
      << "Socket: "
      << this->connectedSocketID
      << ". "
      << Logger::endL;
      this->parent->transportLayerSecurity.getOpenSSLData().clearErrorQueue(
        bytesSent
      );
      return;
    }
    if (bytesSent == 0) {
      numberOfTimesRunWithoutSending ++;
    } else {
      numberOfTimesRunWithoutSending = 0;
    }
    global << bytesSent;
    this->remainingBytesToSend.sliceInPlace(
      bytesSent, this->remainingBytesToSend.size - bytesSent
    );
    if (this->remainingBytesToSend.size > 0) {
      global << ", ";
    }
    if (numberOfTimesRunWithoutSending > 3) {
      global
      << "WebWorker::sendAllBytesNoHeaders failed: "
      << "send function went through 3 cycles without "
      << "sending any bytes. "
      << Logger::endL;
      return;
    }
  }
  global << "... done." << Logger::endL;
}

void SystemFunctions::segfaultSigaction[[noreturn]](
  int signal, siginfo_t* si, void* arg
) {
  // <- This signal should never happen in
  // <- server, so even if racy, we take the risk of a hang.
  // <- Racy-ness in child process does not bother us: hanged children are
  // still
  // fine.
  (void) signal;
  (void) arg;
  global.fatal
  << "Caught segfault at address: "
  << si->si_addr
  << global.fatal;
  exit(0);
}

// IPv4 or IPv6:
void* SystemFunctions::getIncomingAddress(struct sockaddr* sa) {
  if (sa->sa_family == AF_INET) {
    return &((reinterpret_cast<struct sockaddr_in*>(sa))->sin_addr);
  }
  return &((reinterpret_cast<struct sockaddr_in6*>(sa))->sin6_addr);
}

std::string WebWorker::toStringMessageFull() const {
  std::stringstream out;
  out << this->toStringMessageShort();
  if (this->messageHeaderStrings.size > 0) {
    out << "<hr>\nStrings extracted from message: ";
    for (int i = 0; i < this->messageHeaderStrings.size; i ++) {
      out
      << "<br>"
      << HtmlRoutines::convertStringToHtmlString(
        this->messageHeaderStrings[i], false
      );
    }
  }
  return out.str();
}

std::string WebWorker::toStringMessageShort() const {
  std::stringstream out;
  if (this->requestType == this->requestGet) {
    out << "GET ";
  } else if (this->requestType == this->requestPost) {
    out << "POST ";
  } else if (this->requestType == this->requestChunked) {
    out << "GET or POST **chunked** ";
  } else {
    out << "Request type undefined.";
  }
  std::string lineBreak = "\n<br>";
  out
  << "<hr>Address get or post:\n"
  << HtmlRoutines::convertStringToHtmlString(this->addressGetOrPost, true);
  out
  << lineBreak
  << "\nAddress computed:\n"
  << HtmlRoutines::convertStringToHtmlString(this->addressComputed, true);
  out
  << lineBreak
  << "\nArgument from URL:\n"
  << HtmlRoutines::convertStringToHtmlString(this->argumentFomURL, true);
  out
  << lineBreak
  << "\nVirtual file/directory name:\n"
  << HtmlRoutines::convertStringToHtmlString(this->virtualFileName, true);
  out
  << lineBreak
  << "\nRelative physical file/directory name:\n"
  << HtmlRoutines::convertStringToHtmlString(
    this->relativePhysicalFileName, true
  );
  out
  << lineBreak
  << "\nExecutable url:\n"
  << HtmlRoutines::convertStringToHtmlString(
    global.displayNameExecutable, false
  );
  out
  << lineBreak
  << "\nPhysical address project base:\n"
  << HtmlRoutines::convertStringToHtmlString(
    global.physicalPathProjectBase, false
  );
  out
  << lineBreak
  << "\nPhysical address server base:\n"
  << HtmlRoutines::convertStringToHtmlString(
    global.physicalPathServerBase, false
  );
  out << "<hr>";
  if (this->flagKeepAlive) {
    out << "<br><b>Keeping alive.</b><br>";
  } else {
    out << "<br><b>NOT</b> keeping alive.<br>";
  }
  out << "<br>Cookies (" << this->cookies.size << " total):";
  for (int i = 0; i < this->cookies.size; i ++) {
    out
    << "<br>"
    << HtmlRoutines::convertStringToHtmlString(this->cookies[i], false);
  }
  out
  << "\n<hr>\nHost with port:<br>\n"
  << HtmlRoutines::convertStringToHtmlString(global.web.hostWithPort, false)
  << "<br>Host without port:<br>\n"
  << HtmlRoutines::convertStringToHtmlString(global.web.hostNoPort, false);
  out
  << "\n<hr>\nFull message head:<br>\n"
  << HtmlRoutines::convertStringToHtmlString(this->messageHead, true);
  out
  << "\n<hr>\nFull message body:<br>\n"
  << HtmlRoutines::convertStringToHtmlString(this->messageBody, true);
  return out.str();
}

void WebWorker::resetConnection() {
  STACK_TRACE("WebWorker::resetConnection");
  // This function needs a security audit.
  this->resetMessageComponentsExceptRawMessage();
  UserCalculatorData blankUser;
  global.userDefault = blankUser;
  if (
    global.userDefault.enteredActivationToken != "" ||
    global.userDefault.enteredAuthenticationToken != "" ||
    global.userDefault.enteredGoogleToken != "" ||
    global.userDefault.enteredPassword != "" ||
    global.userDefault.enteredHashedSaltedPassword != ""
  ) {
    global.fatal << "User default not reset correctly. " << global.fatal;
  }
  global.flagLoggedIn = false;
  global.flagLogInAttempted = false;
  global.webArguments.clear();
  global.requestType = "";
  global.userInputStringIfAvailable = "";
}

void WebWorker::resetMessageComponentsExceptRawMessage() {
  this->addressComputed = "";
  this->addressGetOrPost = "";
  this->argumentFomURL = "";
  this->argumentFomMessageBody = "";
  this->relativePhysicalFileName = "";
  this->virtualFileName = "";
  this->displayUserInput = "";
  this->messageHeaderStrings.setSize(0);
  this->requestHeaders.clear();
  this->requestType = this->requestUnknown;
  this->contentLength = - 1;
}

JSData WebWorker::getDatabaseJSON() {
  STACK_TRACE("WebWorker::getDatabaseJSON");
  JSData result;
  if (!global.userDefaultHasAdminRights()) {
    result[WebAPI::Result::error] =
    "Only logged-in admins can access database. ";
    return result;
  }
  if (global.hasDisabledDatabaseEveryoneIsAdmin()) {
    result[WebAPI::Result::error] =
    "Database not available (cannot get database info). ";
    return result;
  }
  std::string operation =
  global.getWebInput(WebAPI::DatabaseParameters::operation);
  std::string findQuery =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::DatabaseParameters::findQuery), false
  );
  std::string projector =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::DatabaseParameters::projector), false
  );
  if (operation == WebAPI::DatabaseParameters::fetch) {
    result = Database::get().toJSONDatabaseFetch(findQuery);
  } else if (operation == WebAPI::DatabaseParameters::allTables) {
    result = Database::get().toJSONDatabaseCollection("");
  } else {
    result[WebAPI::Result::error] =
    "Unknown database operation: " + operation + ". ";
  }
  if (global.userDebugFlagOn()) {
    result["databaseOperation"] = operation;
    result["findQuery"] = findQuery;
  }
  return result;
}

std::string WebWorker::getDatabaseDeleteOneItem() {
  STACK_TRACE("WebWorker::getDatabaseDeleteOneItem");
  if (!global.userDefaultHasAdminRights()) {
    return "Only logged-in admins can execute the delete command. ";
  }
  std::stringstream commentsStream;
  std::string inputEncoded = global.getWebInput("item");
  std::string inputString =
  HtmlRoutines::convertURLStringToNormal(inputEncoded, false);
  JSData inputParsed;
  if (!inputParsed.parse(inputString, false, &commentsStream)) {
    commentsStream << "Failed to parse input string. ";
    return commentsStream.str();
  }
  commentsStream
  << "Parsed input string: "
  << inputParsed.toString(nullptr)
  << "\n";
  if (Database::get().deleteOneEntry(inputParsed, &commentsStream)) {
    return "success";
  }
  return commentsStream.str();
}

bool WebWorker::isValidCookieWebArgument(
  const std::string& argumentName, const std::string& argumentValue
) {
  if (argumentName != "request") {
    return true;
  }
  // <- we are careful about
  // reading arbitrary requests from the cookie. Those may effectively deny
  // login to a person who does not know to change the request type from the
  // web address.
  // To prevent that we refuse to read requests from cookies except for the
  // whitelist below.
  return
  argumentValue == "template" ||
  argumentValue == "exercise" ||
  argumentValue == "database" ||
  argumentValue == "accounts" ||
  argumentValue == "calculator" ||
  argumentValue == "scoredQuiz";
}

bool WebWorker::extractArgumentsFromCookies(
  std::stringstream& argumentProcessingFailureComments
) {
  STACK_TRACE("WebWorker::extractArgumentsFromCookies");
  MapList<std::string, std::string, HashFunctions::hashFunction<std::string> >
  newlyFoundArguments;
  bool result = true;
  for (int i = 0; i < this->cookies.size; i ++) {
    if (
      !HtmlRoutines::chopPercentEncodedStringAppend(
        this->cookies[i],
        newlyFoundArguments,
        argumentProcessingFailureComments
      )
    ) {
      result = false;
    }
  }
  for (int i = 0; i < newlyFoundArguments.size(); i ++) {
    if (global.webArguments.contains(newlyFoundArguments.keys[i])) {
      continue;
      // <-if a key is already given cookie entries are ignored.
    }
    std::string trimmed = newlyFoundArguments.values[i];
    if (trimmed.size() > 0) {
      if (trimmed[trimmed.size() - 1] == ';') {
        trimmed = trimmed.substr(0, trimmed.size() - 1);
      }
    }
    // <-except the last cookie, cookies have extra semicolumn at the end,
    // trimming.
    const std::string key = newlyFoundArguments.keys[i];
    if (this->isValidCookieWebArgument(key, trimmed)) {
      global.webArguments.setKeyValue(key, trimmed);
    }
  }
  return result;
}

bool WebWorker::extractArgumentsFromMessage(
  const std::string& input,
  std::stringstream& argumentProcessingFailureComments,
  int recursionDepth
) {
  STACK_TRACE("WebWorker::extractArgumentsFromMessage");
  if (recursionDepth > 1) {
    argumentProcessingFailureComments
    << "Error: input string encoded too many times";
    return false;
  }
  MapList<std::string, std::string, HashFunctions::hashFunction<std::string> >&
  arguments =
  global.webArguments;
  if (
    !HtmlRoutines::chopPercentEncodedStringAppend(
      input, arguments, argumentProcessingFailureComments
    )
  ) {
    return false;
  }
  return true;
}

// Returns false if something unexpected happens during the login procedure.
// Returning true does not necessarily mean the login information was accepted.
// Returning false guarantees the login information was not accepted.
bool WebWorker::loginProcedure(
  std::stringstream& argumentProcessingFailureComments,
  std::stringstream* comments
) {
  STACK_TRACE("WebWorker::loginProcedure");
  global.flagLoggedIn = false;
  if (global.hasDisabledDatabaseEveryoneIsAdmin()) {
    global.flagLoggedIn = true;
    return true;
  }
  MapList<std::string, std::string, HashFunctions::hashFunction<std::string> >&
  arguments =
  global.webArguments;
  UserCalculatorData& user = global.userDefault;
  user.username =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(DatabaseStrings::labelUsername), true
  );
  user.email =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(DatabaseStrings::labelEmail), true
  );
  if (user.username.find('%') != std::string::npos) {
    argumentProcessingFailureComments
    << "<b>Unusual behavior: % sign in username.</b>";
  }
  user.enteredAuthenticationToken =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput("authenticationToken"), false
  );
  user.enteredGoogleToken =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput("googleToken"), false
  );
  user.enteredActivationToken =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput("activationToken"), false
  );
  user.enteredPassword =
  HtmlRoutines::convertStringToURLString(
    HtmlRoutines::convertURLStringToNormal(
      global.getWebInput(DatabaseStrings::labelPassword), true
    ),
    false
  );
  // <-Passwords are ONE-LAYER url-encoded
  // <-INCOMING pluses in passwords MUST be decoded as spaces, this is how
  // form.submit() works!
  // <-Incoming pluses must be re-coded as spaces (%20).
  user.flagEnteredPassword = (user.enteredPassword != "");
  if (user.flagEnteredPassword) {
    user.flagMustLogin = true;
    user.enteredGoogleToken = "";
    user.enteredAuthenticationToken = "";
    user.enteredActivationToken = "";
    user.flagEnteredAuthenticationToken = false;
    user.flagEnteredActivationToken = false;
    global.flagLogInAttempted = true;
    if (global.userDebugFlagOn() && comments != nullptr) {
      *comments
      << "Password was entered: all other authentication methods ignored. ";
    }
  }
  if (user.enteredActivationToken != "") {
    user.enteredGoogleToken = "";
    user.enteredAuthenticationToken = "";
    user.flagEnteredAuthenticationToken = false;
    user.flagEnteredActivationToken = true;
    global.flagLogInAttempted = true;
    if (global.userDebugFlagOn() && comments != nullptr) {
      *comments
      << "Activation token entered: "
      << "authentication token and google token ignored. ";
    }
  }
  if (user.username != "") {
    user.enteredGoogleToken = "";
  }
  if (user.enteredAuthenticationToken != "") {
    user.flagEnteredAuthenticationToken = true;
    global.flagLogInAttempted = true;
  }
  // The URLStringToNormal and getWebInput
  // functions may leave traces of (freed) memory
  // of the old password. Could only be used
  // if the attacker has control of the server executable - which probably
  // means we already are in big trouble - so this really shouldn't be
  // such a big deal.
  if (
    !user.flagEnteredPassword &&
    !user.flagEnteredAuthenticationToken &&
    user.enteredActivationToken == "" &&
    user.enteredGoogleToken == ""
  ) {
    return !user.flagMustLogin;
  }
  if (!global.flagUsingSSLinCurrentConnection) {
    return false;
  }
  bool doAttemptGoogleTokenLogin = false;
  if (user.enteredGoogleToken != "") {
    if (
      user.enteredActivationToken == "" &&
      user.enteredPassword == "" &&
      user.enteredAuthenticationToken == ""
    ) {
      user.username = "";
      doAttemptGoogleTokenLogin = true;
    }
    if (user.username == "") {
      doAttemptGoogleTokenLogin = true;
    }
  } else if (user.username == "") {
    return !user.flagMustLogin;
  }
  bool changingPassword = global.requestType == WebAPI::Request::changePassword
  ||
  global.requestType == WebAPI::Request::activateAccountJSON;
  if (changingPassword) {
    user.enteredAuthenticationToken = "";
  }
  if (doAttemptGoogleTokenLogin) {
    bool tokenIsGood = false;
    global.flagLoggedIn =
    Database().get().user.loginViaGoogleTokenCreateNewAccountIfNeeded(
      user, &argumentProcessingFailureComments, nullptr, tokenIsGood
    );
    if (tokenIsGood && !global.flagLoggedIn && comments != nullptr) {
      *comments
      << "Your authentication is valid but "
      << "I have problems with my database records. ";
    }
  } else if (
    user.enteredAuthenticationToken != "" ||
    user.enteredPassword != "" ||
    user.enteredActivationToken != ""
  ) {
    global.flagLoggedIn =
    Database().get().user.loginViaDatabase(
      user, &argumentProcessingFailureComments
    );
  }
  global.cookiesToBeSent.setKeyValue(
    DatabaseStrings::labelUsername,
    HtmlRoutines::convertStringToURLString(user.username, false) // <-User name
    // must be stored in URL-encoded fashion, NO PLUSES.
  );
  if (global.flagLoggedIn && user.enteredActivationToken == "") {
    // In case the user logged in with password, we need
    // to give the user the correct authentication token.
    global.cookiesToBeSent.setKeyValue(
      DatabaseStrings::labelAuthenticationToken,
      HtmlRoutines::convertStringToURLString(
        user.actualAuthenticationToken, false
      ) // <-URL-encoded fashion, NO PLUSES.
    );
    global.setWebInput(
      DatabaseStrings::labelAuthenticationToken,
      HtmlRoutines::convertStringToURLString(
        user.actualAuthenticationToken, false
      )
    );
  } else {
    global.cookiesToBeSent.setKeyValue("authenticationToken", "0");
  }
  bool shouldDisplayMessage = false;
  if (!global.flagLoggedIn && user.username != "") {
    if (user.flagEnteredPassword || user.flagEnteredActivationToken) {
      shouldDisplayMessage = true;
    }
    if (user.flagEnteredAuthenticationToken) {
      if (
        user.enteredActivationToken != "0" && user.enteredActivationToken != ""
      ) {
        shouldDisplayMessage = true;
      }
    }
  }
  user.clearAuthenticationTokenAndPassword();
  if (shouldDisplayMessage) {
    argumentProcessingFailureComments
    << "\nInvalid user and/or authentication. ";
  }
  arguments.setKeyValue(
    DatabaseStrings::labelPassword,
    "********************************************"
  );
  return true;
}

void WebWorker::writeAfterTimeoutProgress(
  const JSData& input, bool forceFileWrite
) {
  this->pauseIfRequested();
  STACK_TRACE("WebWorker::writeAfterTimeoutProgress");
  if (!this->workerToWorkerRequestIndicator.readOnceNoFailure(true)) {
    global
    << Logger::red
    << "Failed to read non-blocking worker-to-worker pipe. "
    << Logger::endL;
    return;
  }
  if (
    this->workerToWorkerRequestIndicator.lastRead.size == 0 && !forceFileWrite
  ) {
    return;
  }
  this->writeAfterTimeoutJSON(
    input,
    WebAPI::Result::running,
    global.relativePhysicalNameOptionalProgressReport
  );
}

void WebWorker::parseMessageHead() {
  STACK_TRACE("WebWorker::parseMessageHead");
  this->resetMessageComponentsExceptRawMessage();
  std::string buffer;
  buffer.reserve(this->messageHead.size());
  this->messageHeaderStrings.setExpectedSize(50);
  this->cookies.setExpectedSize(30);
  this->messageHeaderStrings.size = 0;
  this->cookies.size = 0;
  this->connectionFlags.size = 0;
  this->messageBody = "";
  this->flagKeepAlive = false;
  this->contentLength = - 1;
  this->addressGetOrPost = "";
  this->hostWithPort = "";
  for (unsigned i = 0; i < this->messageHead.size(); i ++) {
    if (
      this->messageHead[i] != ' ' &&
      this->messageHead[i] != '\n' &&
      this->messageHead[i] != '\r'
    ) {
      buffer.push_back(this->messageHead[i]);
      if (i == this->messageHead.size() - 1) {
        this->messageHeaderStrings.addOnTop(buffer);
      }
    } else {
      if (buffer != "") {
        this->messageHeaderStrings.addOnTop(buffer);
        buffer = "";
      }
      if (i > 0) {
        if (this->messageHead[i - 1] == '\r' && this->messageHead[i] == '\n') {
          this->messageHeaderStrings.addOnTop("\n");
        }
      }
    }
  }
  for (int i = 0; i < this->messageHeaderStrings.size; i ++) {
    if (this->messageHeaderStrings[i] == "GET") {
      this->requestType = this->requestGet;
      i ++;
      if (i < this->messageHeaderStrings.size) {
        this->addressGetOrPost = this->messageHeaderStrings[i];
      }
    } else if (this->messageHeaderStrings[i] == "POST") {
      this->requestType = this->requestPost;
      i ++;
      // Short post messages may be attached to the message head
      // if that is the case the message does not end in \n.
      if (i < this->messageHeaderStrings.size) {
        this->addressGetOrPost = this->messageHeaderStrings[i];
        if (*this->messageHeaderStrings.lastObject() != "\n") {
          this->messageBody = *this->messageHeaderStrings.lastObject();
        } else {
          this->messageBody = "";
        }
      }
    } else if (this->messageHeaderStrings[i] == "HEAD") {
      this->requestType = this->requestHead;
      i ++;
      if (i < this->messageHeaderStrings.size) {
        this->addressGetOrPost = this->messageHeaderStrings[i];
      }
    } else if (
      this->messageHeaderStrings[i] == "transfer-coding:" ||
      this->messageHeaderStrings[i] == "Transfer-coding:" ||
      this->messageHeaderStrings[i] == "Transfer-Coding:"
    ) {
      i ++;
      if (i < this->messageHeaderStrings.size) {
        if (
          this->messageHeaderStrings[i] == "chunked" ||
          this->messageHeaderStrings[i] == "chunked;" ||
          this->messageHeaderStrings[i] == "Chunked" ||
          this->messageHeaderStrings[i] == "Chunked;"
        ) {
          this->requestType = this->requestChunked;
        }
      }
    } else if ((
        this->messageHeaderStrings[i] == "Content-Length:" ||
        this->messageHeaderStrings[i] == "Content-length:" ||
        this->messageHeaderStrings[i] == "content-length:"
      ) &&
      i + 1 < this->messageHeaderStrings.size
    ) {
      if (this->messageHeaderStrings[i + 1].size() < 10000) {
        LargeIntegerUnsigned largeInteger;
        if (
          largeInteger.assignStringFailureAllowed(
            this->messageHeaderStrings[i + 1], true
          )
        ) {
          if (!largeInteger.isIntegerFittingInInt(&this->contentLength)) {
            this->contentLength = - 1;
          }
        }
      }
    } else if (
      this->messageHeaderStrings[i] == "Host:" ||
      this->messageHeaderStrings[i] == "host:"
    ) {
      i ++;
      if (i < this->messageHeaderStrings.size) {
        this->hostWithPort = this->messageHeaderStrings[i];
      }
    } else if (
      this->messageHeaderStrings[i] == "Cookie:" ||
      this->messageHeaderStrings[i] == "cookie:"
    ) {
      i ++;
      for (; i < this->messageHeaderStrings.size; i ++) {
        if (this->messageHeaderStrings[i] == "\n") break;
        this->cookies.addOnTop(this->messageHeaderStrings[i]);
      }
    } else if (
      this->messageHeaderStrings[i] == "Connection:" ||
      this->messageHeaderStrings[i] == "connection:"
    ) {
      i ++;
      for (; i < this->messageHeaderStrings.size; i ++) {
        if (this->messageHeaderStrings[i] == "\n") {
          break;
        }
        this->connectionFlags.addOnTop(this->messageHeaderStrings[i]);
        if (
          this->messageHeaderStrings[i] == "keep-alive" ||
          this->messageHeaderStrings[i] == "Keep-Alive" ||
          this->messageHeaderStrings[i] == "Keep-alive"
        ) {
          this->flagKeepAlive = true;
        }
      }
    }
  }
  if (this->messageBody.size() > 0 && this->contentLength < 0) {
    this->contentLength = static_cast<signed>(this->messageBody.size());
  }
  global.web.hostWithPort = this->hostWithPort;
}

void WebWorker::attemptUnknownRequestErrorCorrection() {
  STACK_TRACE("WebWorker::attemptUnknownRequestErrorCorrection");
  if (this->requestType != this->requestUnknown) {
    return;
  }
  global << Logger::red << "Unknown request. " << Logger::endL;
  global
  << Logger::blue
  << "Message head length: "
  << this->messageHead.size()
  << Logger::endL;
  std::string messageHeadHexed =
  Crypto::convertStringToHex(this->messageHead, 100, false);
  global
  << HtmlRoutines::convertStringToHtmlStringRestrictSize(
    messageHeadHexed, false, 300
  )
  << Logger::endL;
  global
  << Logger::orange
  << "Message body length: "
  << this->messageBody.size()
  << ". "
  << Logger::endL;
  global
  << HtmlRoutines::convertStringToHtmlStringRestrictSize(
    this->messageBody, false, 300
  )
  << Logger::endL;
  global << Logger::green << "Attempting to correct unknown request.\n";
  if (this->messageBody.size() == 0) {
    if (*this->messageHeaderStrings.lastObject() != "\n") {
      global << Logger::green << "Message body set to last message chunk.\n";
      this->messageBody = *this->messageHeaderStrings.lastObject();
    }
  }
  if (this->messageBody.size() != 0) {
    global << "Request set to: POST\n";
    this->requestType = this->requestPost;
  } else {
    global << "Request set to: GET\n";
    this->requestType = this->requestGet;
  }
  if (this->addressGetOrPost == "") {
    global << "Address set to: " << global.displayNameExecutable << "\n";
    this->addressGetOrPost = global.displayNameExecutable;
  }
  global
  << Logger::blue
  << "Unrecognized message head, length: "
  << this->messageHead.size()
  << ".\n";
  global
  << Logger::red
  << "Message body length: "
  << this->messageBody.size()
  << ". ";
  global << Logger::endL;
}

void WebWorker::extractHostInfo() {
  STACK_TRACE("WebWorker::extractHostInfo");
  unsigned int i = 0;
  for (i = 0; i < this->hostWithPort.size(); i ++) {
    if (this->hostWithPort[i] == ':') {
      break;
    }
  }
  if (i < this->hostWithPort.size()) {
    this->hostNoPort = this->hostWithPort.substr(0, i);
  } else {
    this->hostNoPort = this->hostWithPort;
  }
  global.web.hostWithPort = this->hostWithPort;
  global.web.hostNoPort = this->hostNoPort;
  global.web.port =
  this->parent->allListeningSockets.getValue(
    this->parent->lastListeningSocket, ""
  );
  if (
    global.web.hostNoPort == "localhost" ||
    global.web.hostNoPort == "127.0.0.1"
  ) {
    global.flagRequestComingLocally = true;
  } else {
    global.flagRequestComingLocally = false;
  }
}

void WebWorker::extractAddressParts() {
  STACK_TRACE("WebWorker::extractAddressParts");
  bool found = false;
  for (unsigned i = 0; i < this->addressGetOrPost.size(); i ++) {
    if (this->addressGetOrPost[i] == '?') {
      this->addressComputed = this->addressGetOrPost.substr(0, i);
      this->argumentFomURL =
      this->addressGetOrPost.substr(i + 1, std::string::npos);
      found = true;
      break;
    }
  }
  if (!found) {
    this->addressComputed = this->addressGetOrPost;
  }
  if (this->messageBody != "") {
    this->argumentFomMessageBody = this->messageBody;
  }
}

std::string WebWorker::getHeaderConnectionClose() {
  std::stringstream out;
  out << "Connection: close";
  return out.str();
}

std::string WebWorker::getHeaderConnectionKeepAlive() {
  std::stringstream out;
  out << "Connection: keep-alive";
  return out.str();
}

std::string WebWorker::getHeaderSetCookie() {
  std::stringstream out;
  for (int i = 0; i < global.cookiesToBeSent.size(); i ++) {
    out
    << "Set-Cookie: "
    << global.cookiesToBeSent.keys[i]
    << "="
    << global.cookiesToBeSent.values[i]
    << "; Path=/; Expires=Sat, 01 Jan 2030 20:00:00 GMT;"
    << "Secure;SameSite=Strict;";
    if (i != global.cookiesToBeSent.size() - 1) {
      out << "\r\n";
    }
  }
  return out.str();
}

void WebWorker::setHeader(
  const std::string& httpResponseNoTermination,
  const std::string& remainingHeaderNoTermination
) {
  STACK_TRACE("WebWorker::setHeader");
  std::stringstream out;
  out << httpResponseNoTermination << "\r\n";
  if (!this->flagKeepAlive) {
    out << this->getHeaderConnectionClose() << "\r\n";
  } else {
    out << this->getHeaderConnectionKeepAlive() << "\r\n";
  }
  if (remainingHeaderNoTermination != "") {
    out << remainingHeaderNoTermination << "\r\n";
  }
  if ((global.flagLoggedIn || global.flagLogInAttempted) &&
    WebWorker::getHeaderSetCookie() != ""
  ) {
    out << WebWorker::getHeaderSetCookie() << "\r\n";
  }
  std::string finalHeader = out.str();
  this->remainingHeaderToSend.setSize(0);
  this->remainingHeaderToSend.setExpectedSize(
    static_cast<signed>(finalHeader.size())
  );
  for (unsigned i = 0; i < finalHeader.size(); i ++) {
    this->remainingHeaderToSend.addOnTop(finalHeader[i]);
  }
}

void WebWorker::setHeaderOKNoContentLength(
  const std::string& extraHeader, const std::string& contentType
) {
  STACK_TRACE("WebWorker::setHeaderOKNoContentLength");
  std::string header =
  "Content-Type: " +
  contentType +
  "; charset=utf-8\r\nAccess-Control-Allow-Origin: *";
  if (extraHeader != "") {
    header += "\r\n" + extraHeader;
  }
  this->setHeader("HTTP/1.0 200 OK", header);
  this->flagDoAddContentLength = true;
}

void WebWorker::sanitizeVirtualFileName() {
  STACK_TRACE("WebWorker::sanitizeVirtualFileName");
  std::string resultName;
  resultName.reserve(this->virtualFileName.size());
  bool foundslash = false;
  for (
    signed i = static_cast<signed>(this->virtualFileName.size()) - 1; i >= 0;
    i --
  ) {
    unsigned k = static_cast<unsigned>(i);
    bool isOK = true;
    if (foundslash && this->virtualFileName[k] == '.') {
      if (i > 0 && this->virtualFileName[k - 1] == '.') {
        this->flagFileNameSanitized = true;
        isOK = false;
      }
    }
    if (isOK) {
      resultName.push_back(this->virtualFileName[k]);
    }
    if (this->virtualFileName[k] == '/') {
      foundslash = true;
    }
  }
  this->virtualFileName = "";
  for (signed i = static_cast<signed>(resultName.size()) - 1; i >= 0; i --) {
    this->virtualFileName.push_back(resultName[static_cast<unsigned>(i)]);
  }
}

bool WebWorker::isValidWorkerId(const std::string& workerId) {
  return StringRoutines::isLatinLetterOrDigitSequence(workerId);
}

int WebWorker::getIndexOfWorkerId(
  JSData& outputComputationStatus, std::string& outputWorkerId
) {
  STACK_TRACE("WebWorker::getIndexOfWorkerId");
  outputWorkerId = global.getWebInput(WebAPI::Request::workerId);
  std::stringstream commentsOnError;
  std::string computationResult;
  if (!this->isValidWorkerId(outputWorkerId)) {
    outputComputationStatus[WebAPI::Result::error] =
    "Your workerId seems to be invalid.";
    return - 1;
  }
  // 1. Warning: timing attacks on the speed of looking up file names
  // may be used to guess an old worker id.
  // No need to worry as user computations should not contain any
  // authentication
  // or other critical information. User computations are considered
  // ultra-sensitive only because they are private, so there is no reason
  // to overdo the cryptographic protections beyond the common-sense one
  // of requesting a unique id.
  int indexOther = this->parent->workerIds.getValue(outputWorkerId, - 1);
  if (indexOther >= 0) {
    this->parent->allWorkers[indexOther].writingReportFile.lock();
  }
  bool success =
  FileOperations::loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(
    "results/" + outputWorkerId,
    computationResult,
    true,
    true,
    &commentsOnError
  );
  if (indexOther >= 0) {
    this->parent->allWorkers[indexOther].writingReportFile.unlock();
  }
  if (!success) {
    commentsOnError
    << "Failed to load your output with id: "
    << outputWorkerId
    << ". ";
    outputComputationStatus[WebAPI::Result::error] = commentsOnError.str();
    return - 1;
  }
  if (!outputComputationStatus.parse(computationResult)) {
    commentsOnError
    << "I found your worker id: "
    << outputWorkerId
    << " but I could not parse its JSON status. "
    << "This is likely an internal server error. ";
    outputComputationStatus[WebAPI::Result::error] = commentsOnError.str();
    return - 1;
  }
  int indexFromFile = - 1;
  outputComputationStatus[WebAPI::Result::workerIndex].isIntegerFittingInInt(
    &indexFromFile
  );
  if (indexOther < 0) {
    indexOther = indexFromFile;
  }
  if (indexOther != indexFromFile || indexOther == - 1) {
    global.fatal
    << "Corrupt index worker: indexOther: "
    << indexOther
    << "; indexFromFile: "
    << indexFromFile
    << ". "
    << global.fatal;
  }
  if (indexOther >= this->parent->allWorkers.size) {
    // Possible causes of this situation:
    // 1) worker id was created in a worker process that was fired up after the
    // current
    // worker.
    // 2) server executable was restarted.
    // Once a worker id is not found, we want to close the connection.
    std::stringstream out;
    out
    << "WorkerId has too large of an index: "
    << indexOther
    << ". "
    << outputWorkerId
    << " not found. ";
    outputComputationStatus[WebAPI::Result::comments] = out.str();
    this->flagKeepAlive = false;
    indexOther = - 1;
  }
  return indexOther;
}

JSData WebWorker::processComputationIndicatorJSData() {
  STACK_TRACE("WebWorker::processComputationIndicatorJSData");
  // Timer thread will no longer time us out.
  JSData result;
  std::stringstream commentsOnFailure;
  bool allowed = true;
  if (
    !this->flagUsingSSLInWorkerProcess &&
    !global.hasDisabledDatabaseEveryoneIsAdmin()
  ) {
    allowed = false;
  }
  if (!allowed) {
    commentsOnFailure
    << "Monitoring allowed only "
    << "1) over https or "
    << "2) when flag disableDatabaseLogEveryoneAsAdmin is set to true "
    << "using file configuration/configuration.json.";
    result[WebAPI::Result::error] = commentsOnFailure.str();
    return result;
  }
  std::string unusedWorkerId;
  int otherIndex = this->getIndexOfWorkerId(result, unusedWorkerId);
  if (otherIndex < 0) {
    this->flagKeepAlive = false;
    return result;
  }
  WebWorker& otherWorker = this->parent->allWorkers[otherIndex];
  // Request a progress report from the running worker, non-blocking.
  otherWorker.workerToWorkerRequestIndicator.writeOnceAfterEmptying("!", true);
  return result;
}

void WebWorker::writeAfterTimeoutCarbonCopy(
  const JSData& input, const std::string& fileNameCarbonCopy
) {
  if (fileNameCarbonCopy == "") {
    return;
  }
  std::string extraFilename = "output/";
  extraFilename +=
  FileOperations::cleanUpForFileNameUse(fileNameCarbonCopy) + ".json";
  std::stringstream commentsOnError;
  bool success =
  FileOperations::writeFileVirtual(
    extraFilename, input.toString(nullptr), &commentsOnError
  );
  if (!success) {
    global
    << Logger::red
    << "Error writing optional file. "
    << commentsOnError.str()
    << Logger::endL;
  }
}

void WebWorker::writeAfterTimeoutJSON(
  const JSData& input,
  const std::string& status,
  const std::string& fileNameCarbonCopy
) {
  STACK_TRACE("WebWorker::writeAfterTimeoutJSON");
  JSData result;
  result = input;
  WebWorker::writeAfterTimeoutPartTwo(result, status, fileNameCarbonCopy);
}

void WebWorker::writeAfterTimeoutPartTwo(
  JSData& result,
  const std::string& status,
  const std::string& fileNameCarbonCopy
) {
  STACK_TRACE("WebWorker::writeAfterTimeoutPartTwo");
  std::stringstream commentsOnError;
  result[WebAPI::Result::status] = status;
  WebWorker& currentWorker = global.server().getActiveWorker();
  result[WebAPI::Result::workerIndex] = currentWorker.indexInParent;
  std::string toWrite = result.toString(nullptr);
  currentWorker.writingReportFile.lock();
  bool success =
  FileOperations::
  writeFileVirualWithPermissions_accessUltraSensitiveFoldersIfNeeded(
    "results/" + currentWorker.workerId, toWrite, true, true, &commentsOnError
  );
  currentWorker.writeAfterTimeoutCarbonCopy(result, fileNameCarbonCopy);
  currentWorker.writingReportFile.unlock();
  if (success) {
    global
    << Logger::green
    << "Data written to file: "
    << currentWorker.workerId
    << Logger::endL;
  } else {
    global << "Failed to write computation data. " << commentsOnError.str();
  }
}

int WebWorker::processFolder() {
  STACK_TRACE("WebWorker::processFolder");
  this->setHeaderOKNoContentLength("");
  std::stringstream outPage;
  std::stringstream outError;
  outPage << "<html><body>";
  if (this->relativePhysicalFileName.size() > 0) {
    if (
      this->relativePhysicalFileName[this->relativePhysicalFileName.size() - 1]
      !=
      '/'
    ) {
      this->relativePhysicalFileName.push_back('/');
    }
  }
  if (this->addressGetOrPost.size() > 0) {
    if (this->addressGetOrPost[this->addressGetOrPost.size() - 1] != '/') {
      this->addressGetOrPost.push_back('/');
    }
  }
  if (this->flagFileNameSanitized) {
    std::stringstream sanitization;
    sanitization
    << "<hr>The virtual file name I extracted was: "
    << HtmlRoutines::convertStringToHtmlString(this->virtualFileName, false)
    << "<br>However, I do not allow folder names that contain dots. "
    << "Therefore I have sanitized the main address to: "
    << HtmlRoutines::convertStringToHtmlString(
      this->relativePhysicalFileName, false
    );
    outPage << sanitization.str();
    outError << sanitization.str();
  }
  List<std::string> fileNames;
  List<std::string> fileTypes;
  if (
    !FileOperations::getFolderFileNamesUnsecure(
      this->relativePhysicalFileName, fileNames, &fileTypes
    )
  ) {
    outError
    << "<b>Failed to open directory with physical address "
    << HtmlRoutines::convertStringToHtmlString(
      this->relativePhysicalFileName, false
    )
    << " </b>";
    JSData result;
    result[WebAPI::Result::error] = outError.str();
    global.response.writeResponse(result);
    return 0;
  }
  outPage
  << "Browsing folder: "
  << HtmlRoutines::convertStringToHtmlString(this->addressGetOrPost, false)
  << "<br>Virtual name: "
  << HtmlRoutines::convertStringToHtmlString(this->virtualFileName, false)
  << "<hr>";
  List<std::string> folderLinksSanitized;
  List<std::string> fileLinksSanitized;
  for (int i = 0; i < fileNames.size; i ++) {
    std::stringstream currentStream;
    bool isDir = (fileTypes[i] == ".d");
    currentStream
    << "<a href=\""
    << HtmlRoutines::convertStringToURLString(fileNames[i], false);
    if (isDir) {
      currentStream << "/";
    }
    currentStream
    << "\">"
    << HtmlRoutines::convertStringToHtmlString(fileNames[i], false);
    if (isDir) {
      currentStream << "/";
    }
    currentStream << "</a><br>";
    if (isDir) {
      folderLinksSanitized.addOnTop(currentStream.str());
    } else {
      fileLinksSanitized.addOnTop(currentStream.str());
    }
  }
  folderLinksSanitized.quickSortAscending();
  fileLinksSanitized.quickSortAscending();
  for (int i = 0; i < folderLinksSanitized.size; i ++) {
    outPage << folderLinksSanitized[i];
  }
  for (int i = 0; i < fileLinksSanitized.size; i ++) {
    outPage << fileLinksSanitized[i];
  }
  outPage << "\n</body></html>";
  this->setHeaderOKNoContentLength("", "text/html");
  this->writeToBody(outPage.str());
  this->sendPending();
  return 0;
}

int WebWorker::processFileDoesntExist(
  bool generateLinkToCalculatorOnMissingFile
) {
  this->setHeader("HTTP/1.0 404 Object not found", "Content-Type: text/html");
  // WARNING: cross-site scripting danger. Pay attention when editing.
  // Use convertStringToHtmlString to sanitize strings.
  // Never return non-sanitized user input back to the user.
  std::stringstream out;
  out << "<html>" << "<body>";
  if (generateLinkToCalculatorOnMissingFile) {
    out
    << "one page <a href = \""
    << global.displayApplication
    << "\">app</a>. ";
    out
    << " Same app without browser cache: <a href = \""
    << global.displayApplicationNoCache
    << "\">app no cache</a>.<hr>";
  }
  out << "<b>File does not exist.</b>";
  if (this->flagFileNameSanitized) {
    out
    << "<hr>You requested virtual file: "
    << HtmlRoutines::convertStringToHtmlString(this->virtualFileName, false)
    << "<br>However, I do not allow addresses that contain dots "
    << "(to avoid access to folders below the server). "
    << "Therefore I have sanitized "
    << "the address to a relative physical address: "
    << HtmlRoutines::convertStringToHtmlString(
      this->relativePhysicalFileName, false
    )
    << ".";
  }
  out
  << "<br><b>Address:</b> "
  << HtmlRoutines::convertStringToHtmlString(this->addressGetOrPost, true)
  << "<br><b>Virtual file name:</b> "
  << HtmlRoutines::convertStringToHtmlString(this->virtualFileName, true)
  << "<br><b>Computed relative physical file name:</b> "
  << HtmlRoutines::convertStringToHtmlString(
    this->relativePhysicalFileName, true
  );
  out
  << "<br><b>Request:</b> "
  << HtmlRoutines::convertStringToHtmlString(global.requestType, true);
  out << "</body></html>";
  // End of WARNING: cross-site scripting danger.
  this->writeToBody(out.str());
  this->sendPending();
  return - 1;
}

int WebWorker::processFileCantOpen() {
  this->setHeaderOKNoContentLength("");
  std::stringstream out;
  out << "Error: file appears to exist but I could not open it. ";
  global
  << Logger::yellow
  << "Can't open file. "
  << Logger::red
  << strerror(errno)
  << Logger::endL;
  if (this->flagFileNameSanitized) {
    out
    << "You requested virtual file: "
    << this->virtualFileName
    << "However, I do not allow addresses that contain dots. "
    << "Therefore I have sanitized "
    << "the address to a relative physical address: "
    << this->relativePhysicalFileName;
  }
  out
  << "File display name: "
  << this->addressGetOrPost
  << ". Virtual file name: "
  << this->virtualFileName
  << ". ";
  JSData result;
  result[WebAPI::Result::error] = out.str();
  return this->writeToBodyJSON(result);
}

int WebWorker::processFileTooLarge(long fileSize) {
  this->setHeader("HTTP/1.0 413 Payload Too Large", "");
  std::stringstream out;
  out
  << "User requested file: "
  << this->virtualFileName
  << " but it is too large, namely, "
  << fileSize
  << " bytes.";
  JSData result;
  result[WebAPI::Result::error] = out.str();
  return this->writeToBodyJSON(result);
}

int WebWorker::processFile(bool generateLinkToCalculatorOnMissingFile) {
  STACK_TRACE("WebWorker::processFile");
  if (!FileOperations::fileExistsUnsecure(this->relativePhysicalFileName)) {
    return
    this->processFileDoesntExist(generateLinkToCalculatorOnMissingFile);
  }
  std::string fileExtension =
  FileOperations::getFileExtensionWithDot(this->relativePhysicalFileName);
  bool isBinary = this->isFileExtensionOfBinaryFile(fileExtension);
  std::fstream fileStream;
  if (
    !FileOperations::openFileUnsecure(
      fileStream, this->relativePhysicalFileName, false, false, !isBinary
    )
  ) {
    return this->processFileCantOpen();
  }
  fileStream.seekp(0, std::ifstream::end);
  long fileSize = fileStream.tellp();
  if (fileSize > 100000000) {
    return this->processFileTooLarge(fileSize);
  }
  std::stringstream header;
  header
  << "HTTP/1.0 200 OK\r\n"
  << this->headerFromFileExtension(fileExtension)
  << "Access-Control-Allow-Origin: *\r\n";
  for (
    int i = 0; i < this->parent->addressStartsSentWithCacheMaxAge.size; i ++
  ) {
    if (
      StringRoutines::stringBeginsWith(
        this->virtualFileName,
        this->parent->addressStartsSentWithCacheMaxAge[i]
      )
    ) {
      header << WebAPI::headerCacheControl << "\r\n";
      break;
    }
  }
  header << "Content-length: " << fileSize << "\r\n";
  if (!this->flagKeepAlive) {
    header << this->getHeaderConnectionClose() << "\r\n";
  }
  header << "\r\n";
  this->queueStringForSendingNoHeader(header.str());
  if (this->requestType == this->requestHead) {
    this->sendAllBytesNoHeaders();
    return 0;
  }
  const int bufferSize = 64 * 1024;
  this->bufferFileIO.setSize(bufferSize);
  fileStream.seekg(0);
  fileStream.read(&this->bufferFileIO[0], this->bufferFileIO.size);
  long numberOfBytesRead = fileStream.gcount();
  while (numberOfBytesRead != 0) {
    this->bufferFileIO.setSize(static_cast<int>(numberOfBytesRead));
    this->queueBytesForSendingNoHeader(this->bufferFileIO);
    this->bufferFileIO.setSize(bufferSize);
    fileStream.read(this->bufferFileIO.objects, this->bufferFileIO.size);
    numberOfBytesRead = fileStream.gcount();
  }
  this->sendAllBytesNoHeaders();
  return 0;
}

void WebWorker::reset() {
  this->connectedSocketID = - 1;
  this->processPID = - 1;
  this->connectedSocketIDLastValueBeforeRelease = - 1;
  this->connectionID = - 1;
  this->indexInParent = - 1;
  this->millisecondsAfterSelect = - 1;
  this->parent = nullptr;
  this->indentationLevelHTML = 0;
  this->displayUserInput = "";
  this->requestType = this->requestUnknown;
  this->flagDoAddContentLength = false;
  this->flagFileNameSanitized = false;
  this->millisecondsLastPingServerSideOnly = - 1;
  this->millisecondsServerAtWorkerStart = - 1;
  this->flagFoundMalformedFormInput = false;
  this->flagKeepAlive = false;
  this->flagDidSendAll = false;
  this->flagUsingSSLInWorkerProcess = false;
  global.flagUsingSSLinCurrentConnection = false;
  global.flagLoggedIn = false;
  global.userDefault.reset();
  this->relativePhysicalFileName = "";
  this->statistics.allReceives = 0;
  this->statistics.pingReceives = 0;
  this->statistics.standaloneServerReceives = 0;
  this->numberOfConsecutiveNoReportsBeforeDisconnect = 0;
  this->release();
}

WebWorker::WebWorker() {
  this->flagDeallocated = false;
  this->reset();
}

bool WebWorker::isActive() {
  if (this->parent == nullptr || this->indexInParent == - 1) {
    return false;
  }
  return this->parent->activeWorker == this->indexInParent;
}

bool WebWorker::isFileExtensionOfBinaryFile(const std::string& fileExtension) {
  if (fileExtension == ".png") {
    return true;
  }
  if (fileExtension == ".mp4") {
    return true;
  }
  if (fileExtension == ".mpeg") {
    return true;
  }
  if (fileExtension == ".jpg") {
    return true;
  }
  return false;
}

void WebWorker::wrapUpConnection() {
  STACK_TRACE("WebWorker::wrapUpConnection");
  if (global.flagServerDetailedLog) {
    global << "Detail: wrapping up connection. " << Logger::endL;
  }
  this->resultWork[WebServer::Statististics::allRequestsString] =
  this->statistics.allReceives;
  this->resultWork[WebServer::Statististics::pingRequestsString] =
  this->statistics.pingReceives;
  this->resultWork[WebServer::Statististics::standaloneServerRequestsString] =
  this->statistics.standaloneServerReceives;
  this->resultWork["result"] = "close";
  this->pipeWorkerToServerControls.writeOnceAfterEmptying(
    this->resultWork.toString(nullptr), false
  );
  if (global.flagServerDetailedLog) {
    global << "Detail: done with pipes, releasing resources. " << Logger::endL;
  }
  this->release();
  if (global.flagServerDetailedLog) {
    global << "Detail: released. " << Logger::endL;
  }
  global.flagComputationComplete = true;
  global.joinAllThreads();
}

WebWorker::~WebWorker() {
  // Workers are not allowed to release resources in the destructor:
  // a Worker's destructor is called when expanding List<WebWorker>.
  // global << " web worker destructor called. " << Logger::endL;
  this->flagDeallocated = true;
}

std::string WebWorker::headerFromFileExtension(
  const std::string& fileExtension
) {
  std::stringstream out;
  out
  << "Content-Type: "
  << this->mimeTypeFromFileExtension(fileExtension)
  << "\r\n";
  return out.str();
}

void WebServer::initializeMainMIMETypes() {
  this->MIMETypes.setKeyValue(".html", "text/html");
  this->MIMETypes.setKeyValue(".php", "text/html");
  this->MIMETypes.setKeyValue(".txt", "text/plain");
  this->MIMETypes.setKeyValue(".cpp", "text/plain");
  this->MIMETypes.setKeyValue(".pro", "text/plain");
  this->MIMETypes.setKeyValue(".h", "text/plain");
  this->MIMETypes.setKeyValue(".png", "image/png");
  this->MIMETypes.setKeyValue(".js", "text/javascript");
  this->MIMETypes.setKeyValue(".ico", "image/x-icon");
  this->MIMETypes.setKeyValue(".css", "text/css");
  this->MIMETypes.setKeyValue(".eot", "application/vnd.ms-fontobject");
  this->MIMETypes.setKeyValue(".ttf", "application/octet-stream");
  this->MIMETypes.setKeyValue(".svg", "image/svg+xml");
  this->MIMETypes.setKeyValue(".woff", "application/font-woff");
  this->MIMETypes.setKeyValue(".wasm", "application/wasm");
}

std::string WebWorker::mimeTypeFromFileExtension(
  const std::string& fileExtension
) {
  STACK_TRACE("WebWorker::mimeTypeFromFileExtension");
  return
  this->parent->MIMETypes.getValue(fileExtension, "application/octet-stream");
}

int WebWorker::processUnknown() {
  STACK_TRACE("WebWorker::processUnknown");
  this->setHeader(
    "HTTP/1.0 501 Method Not Implemented", "Content-Type: text/html"
  );
  global.response.writeResponse(
    WebAPIResponse::getJSONUserInfo("Unknown request"), false
  );
  return 0;
}

bool WebWorker::shouldDisplayLoginPage() {
  if (global.requestType == "login") {
    return true;
  }
  if (
    global.flagUsingSSLinCurrentConnection &&
    !global.flagLoggedIn &&
    global.requestType != "calculator"
  ) {
    return true;
  }
  if (
    global.userRequestMustBePromptedToLogInIfNotLoggedIn() &&
    !global.flagLoggedIn
  ) {
    return true;
  }
  return false;
}

std::string WebWorker::getAuthenticationToken(
  const std::string& reasonForNoAuthentication
) {
  STACK_TRACE("WebWorker::getAuthenticationToken");
  if (global.flagLoggedIn && global.flagUsingSSLinCurrentConnection) {
    return global.userDefault.actualActivationToken;
  }
  return reasonForNoAuthentication;
}

std::string WebWorker::changePasswordPagePartOne(
  bool& outputDoShowPasswordChangeField
) {
  STACK_TRACE("WebWorker::changePasswordPagePartOne");
  std::stringstream out;
  std::string claimedActivationToken =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput("activationToken"), false
  );
  std::string claimedEmail =
  HtmlRoutines::convertURLStringToNormal(global.getWebInput("email"), false);
  if (claimedActivationToken == "") {
    out << "Activation token is empty. ";
    return out.str();
  }
  if (claimedEmail == "") {
    out << "Activation email is empty. ";
    return out.str();
  }
  std::string actualEmailActivationToken;
  std::string usernameAssociatedWithToken;
  if (global.userDefault.email == claimedEmail) {
    out
    << "\n<b style='color:green'>Email "
    << claimedEmail
    << " updated. </b>";
    return out.str();
  }
  QueryFind findEmail(
    DatabaseStrings::tableEmailInfo, DatabaseStrings::labelEmail, claimedEmail
  );
  List<JSData> allEmails;
  bool success =
  Database::get().find(findEmail, nullptr, allEmails, nullptr, &out);
  if (!success) {
    out
    << "\n<b style='color:red'>"
    << "Failed to fetch email activation token for email: "
    << claimedEmail
    << " </b>";
    return out.str();
  }
  if (allEmails.size != 1) {
    out
    << "\n<b style ='color:red'>"
    << "No activation token for email: "
    << claimedEmail
    << "on record </b>";
    return out.str();
  }
  JSData& emailValue = allEmails[0];
  usernameAssociatedWithToken =
  emailValue[DatabaseStrings::labelUsername].stringValue;
  actualEmailActivationToken =
  emailValue[DatabaseStrings::labelActivationToken].stringValue;
  if (actualEmailActivationToken != claimedActivationToken) {
    out
    << "\n<b style ='color:red'>Bad activation token. "
    << "Could not activate your email. </b>";
    return out.str();
  }
  if (usernameAssociatedWithToken != global.userDefault.username) {
    out
    << "\n<b style ='color:red'>"
    << "Activation token was issued for another user. </b>";
    return out.str();
  }
  QueryUpdate emailSet;
  emailSet.addKeyValueStringPair(
    DatabaseStrings::labelUsername, usernameAssociatedWithToken
  );
  emailSet.addKeyValueStringPair(
    DatabaseStrings::labelActivationToken, actualEmailActivationToken
  );
  if (!Database::get().updateOne(findEmail, emailSet, true, &out)) {
    out
    << "\n<b style ='color:red'>"
    << "Could not reset the activation token (database is down?). "
    << "</b>";
    return out.str();
  }
  QueryFind findUser(
    DatabaseStrings::tableUsers,
    DatabaseStrings::labelUsername,
    global.userDefault.username
  );
  QueryUpdate querySet;
  querySet.addKeyValueStringPair(DatabaseStrings::labelEmail, claimedEmail);
  if (!Database::get().updateOne(findUser, querySet, true, &out)) {
    out
    << "\n<b style='color:red'>"
    << "Could not store your email (database is down?). </b>";
    return out.str();
  }
  global.userDefault.email = claimedEmail;
  out << "\n<b style='color:green'>Email successfully updated. </b>";
  JSData userInfo;
  userInfo[DatabaseStrings::labelEmail] = claimedEmail;
  if (
    global.userDefault.actualActivationToken != "" &&
    global.userDefault.actualActivationToken != "activated" &&
    global.userDefault.actualHashedSaltedPassword != ""
  ) {
    out
    << "<br>It appears your password is already set. "
    << "<br>If you'd like to change it using your old password, "
    << "<a href=\""
    << global.displayNameExecutable
    << "?request=changePasswordPage\">click here</a>. ";
    outputDoShowPasswordChangeField = false;
    userInfo[DatabaseStrings::labelActivationToken] = "activated";
  } else {
    out
    << "<br>"
    << "<b style ='color:orange'>"
    << "To fully activate your account, please choose a password.</b>";
  }
  return out.str();
}

int WebWorker::processgetAuthenticationToken(
  const std::string& reasonForNoAuthentication
) {
  STACK_TRACE("WebWorker::processgetAuthenticationToken");
  this->setHeaderOKNoContentLength("");
  return
  this->writeToBody(this->getAuthenticationToken(reasonForNoAuthentication));
}

JSData WebWorker::getClonePageResult() {
  STACK_TRACE("WebWorker::getClonePageResult");
  this->setHeaderOKNoContentLength("");
  return WebAPIResponse::clonePageResult();
}

std::string WebAPIResponse::modifyProblemReport() {
  STACK_TRACE("WebWorker::modifyProblemReport");
  bool shouldProceed = global.flagLoggedIn &&
  global.userDefaultHasAdminRights();
  if (shouldProceed) {
    shouldProceed = global.flagUsingSSLinCurrentConnection;
  }
  if (!shouldProceed) {
    return
    "<b>Modifying problems allowed only for "
    "logged-in admins under ssl connection. </b>";
  }
  std::string mainInput =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Problem::fileContent), false
  );
  mainInput = StringRoutines::replaceAll(mainInput, "\r", "");
  mainInput = StringRoutines::replaceAll(mainInput, "\t", " ");
  std::string fileName =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Problem::fileName), false
  );
  std::stringstream commentsOnFailure;
  bool fileExists =
  FileOperations::fileExistsVirtualCustomizedReadOnly(
    fileName, &commentsOnFailure
  );
  std::fstream file;
  if (global.hasDisabledDatabaseEveryoneIsAdmin()) {
    global.userDefault.instructorComputed = "default";
  }
  if (
    !FileOperations::openFileVirtualCustomizedWriteOnly(
      file, fileName, false, true, false, &commentsOnFailure
    )
  ) {
    commentsOnFailure
    << "<b style ='color:red'>Failed to open/create file: "
    << fileName
    << ". </b>";
    return commentsOnFailure.str();
  }
  file << mainInput;
  file.close();
  std::stringstream out;
  if (!fileExists) {
    out
    << "File "
    << fileName
    << " didn't previously exist: just created it for you. ";
  }
  out
  << "<b style='color:green'>Wrote "
  << mainInput.size()
  << " bytes to file: "
  << fileName
  << ". </b>";
  return out.str();
}

std::string WebAPIResponse::getJavascriptCaptcha() {
  STACK_TRACE("WebAPIResponse::getJavascriptCaptcha");
  std::stringstream out;
  out << "<script src = 'https://www.google.com/recaptcha/api.js'></script>";
  return out.str();
}

std::string WebAPIResponse::getCaptchaDiv() {
  STACK_TRACE("WebAPIResponse::getCaptchaDiv");
  std::stringstream out;
  std::string recaptchaPublic;
  if (
    !FileOperations::
    loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(
      "certificates/recaptcha-public.txt", recaptchaPublic, true, true, &out
    )
  ) {
    out
    << "<b style ='color:red'>Couldn't find the recaptcha key in file: "
    << "certificates/recaptcha-public.txt</b>";
  } else {
    out
    << "<div class =\"g-recaptcha\" data-sitekey =\""
    << recaptchaPublic
    << "\"></div>";
  }
  return out.str();
}

bool WebWorker::correctRequestsBEFORELoginReturnFalseIfModified() {
  STACK_TRACE("WebWorker::correctRequestsBEFORELoginReturnFalseIfModified");
  bool modified = false;
  if (!global.flagSSLAvailable) {
    if (
      this->addressComputed == global.displayNameExecutable &&
      global.requestType == ""
    ) {
      this->addressComputed = global.displayNameExecutable;
      global.requestType = "calculator";
      modified = true;
    }
  }
  if (this->addressComputed == "/" || this->addressComputed == "") {
    this->addressComputed = global.displayApplication;
    global.requestType = WebAPI::app;
    modified = true;
  }
  return !modified;
}

void WebWorker::redirect(const std::string& address) {
  std::stringstream headerStream;
  headerStream << "Location: " << address;
  this->setHeader("HTTP/1.0 303 See other", headerStream.str());
  std::stringstream out;
  out
  << "<html><head>"
  << "<meta http-equiv=\"refresh\" content =\"0; url ='"
  << address
  << "'\" />"
  << "</head>";
  out
  << "<body>Click <a href=\""
  << address
  << "\">"
  << " here "
  << "</a> if your browser does not redirect the page automatically. ";
  out << "</body></html>";
  this->writeToBody(out.str());
}

bool WebWorker::correctRequestsAFTERLoginReturnFalseIfModified() {
  STACK_TRACE("WebWorker::correctRequestsAFTERLoginReturnFalseIfModified");
  bool stateNotModified = true;
  bool shouldFallBackToDefaultPage = false;
  if (this->addressComputed == "/" || this->addressComputed == "") {
    shouldFallBackToDefaultPage = true;
  }
  if (!global.flagSSLAvailable) {
    if (
      this->addressComputed == global.displayNameExecutable &&
      global.requestType == ""
    ) {
      this->addressComputed = global.displayNameExecutable;
      global.requestType = WebAPI::app;
      stateNotModified = false;
    }
  }
  if (global.flagLoggedIn) {
    if (
      this->addressComputed == global.displayNameExecutable &&
      global.requestType == ""
    ) {
      shouldFallBackToDefaultPage = true;
    } else if (
      global.requestType == "template" &&
      global.getWebInput(WebAPI::Problem::courseHome) == ""
    ) {
      shouldFallBackToDefaultPage = true;
    }
  }
  if (shouldFallBackToDefaultPage) {
    this->addressComputed = global.displayApplication;
    global.requestType = WebAPI::app;
    stateNotModified = false;
  }
  return stateNotModified;
}

bool WebWorker::processRedirectAwayFromWWW() {
  std::string addressNoWWW;
  if (
    !StringRoutines::stringBeginsWith(
      global.web.hostWithPort, "www.", &addressNoWWW
    )
  ) {
    return false;
  }
  std::stringstream newAddressStream;
  std::stringstream redirectHeaderStream;
  // The address will be quoted in a link below.
  // Escape quotes, newlines, etc. to prevent injection via malformed url.
  newAddressStream
  << "https://"
  << HtmlRoutines::convertStringEscapeNewLinesQuotesBackslashes(addressNoWWW);
  if (this->addressGetOrPost.size() != 0) {
    if (this->addressGetOrPost[0] != '/') {
      this->addressGetOrPost = '/' + this->addressGetOrPost;
    }
  }
  newAddressStream << this->addressGetOrPost;
  redirectHeaderStream << "Location: " << newAddressStream.str();
  this->setHeader(
    "HTTP/1.0 301 Moved Permanently", redirectHeaderStream.str()
  );
  std::stringstream out;
  out
  << "<html><body>Please remove the www. "
  << "from the address, it creates issues with "
  << "authentication services. " // Address
  // is quote-escaped to prevent content injection.
  << "Click <a href=\""
  << newAddressStream.str()
  << "\">here</a> if not redirected automatically. ";
  out << "</body></html>";
  this->writeToBody(out.str());
  return true;
}

int WebWorker::processLoginNeededOverUnsecureConnection() {
  STACK_TRACE("WebWorker::processLoginNeededOverUnsecureConnection");
  std::stringstream redirectStream;
  std::stringstream newAddressStream;
  // The address will be included in an href entry enclosed by quotes;
  // escape quotes to prevent content injection using addresses that contain
  // quotes.
  newAddressStream
  << "https://"
  << HtmlRoutines::convertStringEscapeNewLinesQuotesBackslashes(
    this->hostNoPort
  );
  if (this->hostNoPort == "") {
    newAddressStream << "calculator-algebra.org";
  }
  newAddressStream << ":" << this->parent->portHTTPSDefault;
  if (this->addressGetOrPost.size() != 0) {
    if (this->addressGetOrPost[0] != '/') {
      this->addressGetOrPost = '/' + this->addressGetOrPost;
    }
  }
  newAddressStream << this->addressGetOrPost;
  std::stringstream outBody;
  if (this->hostNoPort != "") {
    redirectStream << "Location: " << newAddressStream.str();
    this->setHeader("HTTP/1.0 301 Moved Permanently", redirectStream.str());
    outBody
    << "<html><body>Address available through "
    << "secure (SSL) connection only. " // The
    // newAddressStream is quote-escaped;
    // this should prevent content injection in the snippet below.
    << "Click <a href=\""
    << newAddressStream.str()
    << "\">here</a> if not redirected automatically. ";
  } else {
    this->setHeaderOKNoContentLength("");
    outBody
    << "<html><body>Address available through "
    << "secure (SSL) connection only. <br>"
    << "<b style ='color:red'>In the web address, please "
    << "change http to https. </b><br>"
    << "Unfortunately, I can't redirect you automatically "
    << "as your browser did not tell me "
    << "under what domain name it sees me, and the "
    << "server responds to multiple domain names. ";
  }
  outBody << "</body></html>";
  this->writeToBody(outBody.str());
  this->sendPending();
  return 0;
}

bool WebWorker::requireSSL() {
  return global.flagSSLAvailable;
}

int WebWorker::serveWebServerTraffic() {
  global.web.flagIsStandaloneWebserver = true;
  const ActAsWebServerOnly& configuration =
  global.web.actAsWebServerOnlyForTheseHosts.getValueNoFail(this->hostNoPort);
  if (
    global.web.port != configuration.portHTTP &&
    global.web.port != configuration.portHTTPS
  ) {
    std::stringstream redirectAddressStart;
    redirectAddressStart
    << "https://"
    << this->hostNoPort
    << ":"
    << configuration.portHTTPS;
    std::string redirectAddress =
    FileOperations::addPaths(
      redirectAddressStart.str(), this->addressComputed
    );
    this->redirect(redirectAddress);
    return 0;
  }
  this->addressComputed = configuration.adjustURL(this->addressComputed);
  return this->processFolderOrFile(false);
}

int WebWorker::serveClient() {
  STACK_TRACE("WebWorker::serveClient");
  global.millisecondsComputationStart = global.getElapsedMilliseconds();
  global.flagComputationStarted = true;
  global.flagComputationComplete = false;
  global.userDefault.flagMustLogin = true;
  global.userDefault.flagStopIfNoLogin = true;
  UserCalculatorData& user = global.userDefault;
  this->response.reset(*this);
  if (
    this->requestType != this->requestGet &&
    this->requestType != this->requestPost &&
    this->requestType != this->requestHead
  ) {
    if (
      this->requestType != this->requestUnknown &&
      this->requestType != this->requestChunked
    ) {
      global.fatal
      << "Something is wrong: request type does "
      << "not have any of the expected values. "
      << global.fatal;
    }
    this->processUnknown();
    return 0;
  }
  if (global.allThreads.size <= 1) {
    global.fatal
    << "Number of threads must be at least 2 in this point of code..."
    << global.fatal;
  }
  this->extractHostInfo();
  this->extractAddressParts();
  global.web.flagIsStandaloneWebserver = false;
  if (global.web.actAsWebServerOnlyForTheseHosts.contains(this->hostNoPort)) {
    return this->serveWebServerTraffic();
  }
  std::stringstream argumentProcessingFailureComments;
  this->flagArgumentsAreOK = true;
  if (
    !this->extractArgumentsFromMessage(
      this->argumentFomURL, argumentProcessingFailureComments
    )
  ) {
    this->flagArgumentsAreOK = false;
  }
  if (
    !this->extractArgumentsFromMessage(
      this->argumentFomMessageBody, argumentProcessingFailureComments
    )
  ) {
    this->flagArgumentsAreOK = false;
  }
  if (!this->extractArgumentsFromCookies(argumentProcessingFailureComments)) {
    this->flagArgumentsAreOK = false;
  }
  global.requestType = "";
  if (this->addressComputed == global.displayNameExecutable) {
    global.requestType = global.getWebInput("request");
  }
  std::stringstream comments;
  bool isNotModified = this->correctRequestsBEFORELoginReturnFalseIfModified();
  if (!isNotModified) {
    comments << this->toStringAddressRequest() << ": modified before login. ";
  }
  if (
    global.server().addressStartsInterpretedAsCalculatorRequest.contains(
      this->addressComputed
    )
  ) {
    global.requestType = this->addressComputed;
    std::string correctedRequest;
    if (
      StringRoutines::stringBeginsWith(
        global.requestType, "/", &correctedRequest
      )
    ) {
      global.requestType = correctedRequest;
      comments
      << "Address was interpretted as request, "
      << "so your request was set to: "
      << global.requestType
      << ". ";
    }
  }
  if (global.requestType == global.server().pingAuthentication) {
    this->response.processPing();
    return 0;
  }
  user.flagMustLogin =
  this->parent->requiresLogin(global.requestType, this->addressComputed);
  if (this->requireSSL() && !global.flagUsingSSLinCurrentConnection) {
    return this->processLoginNeededOverUnsecureConnection();
  }
  if (this->processRedirectAwayFromWWW()) {
    this->sendPending();
    return 0;
  }
  this->flagArgumentsAreOK =
  this->loginProcedure(argumentProcessingFailureComments, &comments);
  isNotModified = this->correctRequestsAFTERLoginReturnFalseIfModified();
  if (!isNotModified) {
    comments << this->toStringAddressRequest() << ": modified after login. ";
  }
  user.flagStopIfNoLogin = (
    !global.flagLoggedIn &&
    global.flagUsingSSLinCurrentConnection &&
    user.flagMustLogin
  );
  if (user.flagStopIfNoLogin) {
    if (global.requestType == WebAPI::Request::changePassword) {
      user.flagStopIfNoLogin = false;
    }
  }
  if (user.flagStopIfNoLogin) {
    if (
      global.requestType != WebAPI::Request::logout &&
      global.requestType != WebAPI::Request::login
    ) {
      argumentProcessingFailureComments
      << this->toStringAddressRequest()
      << " requires login. ";
    }
    argumentProcessingFailureComments << comments.str();
    global.cookiesToBeSent.setKeyValue("authenticationToken", "");
    if (argumentProcessingFailureComments.str() != "") {
      global.setWebInput(DatabaseStrings::labelActivationToken, "");
    }
    return
    this->response.processLoginUserInfo(
      argumentProcessingFailureComments.str()
    );
  }
  if (
    argumentProcessingFailureComments.str() != "" && (
      user.flagMustLogin || global.requestType == WebAPI::Request::userInfoJSON
    )
  ) {
    global.setWebInput("error", argumentProcessingFailureComments.str());
  }
  if (
    this->response.serveResponseFalseIfUnrecognized(
      argumentProcessingFailureComments, comments
    )
  ) {
    return 0;
  }
  return this->processFolderOrFile(true);
}

int WebWorker::processFolderOrFile(
  bool generateLinkToCalculatorOnMissingFile
) {
  STACK_TRACE("WebWorker::processFolderOrFile");
  this->virtualFileName =
  HtmlRoutines::convertURLStringToNormal(this->addressComputed, true);
  this->sanitizeVirtualFileName();
  std::stringstream commentsOnFailure;
  if (
    !FileOperations::getPhysicalFileNameFromVirtual(
      this->virtualFileName,
      this->relativePhysicalFileName,
      global.userDefaultHasAdminRights(),
      false,
      &commentsOnFailure
    )
  ) {
    this->setHeader(
      "HTTP/1.0 404 Object not found", "Content-Type: text/html"
    );
    std::stringstream out;
    JSData result;
    out
    << "File name: "
    << HtmlRoutines::convertStringToHtmlStringRestrictSize(
      this->virtualFileName, false, 1000
    )
    << " <b style='color:red'>deemed unsafe</b>. "
    << "Please note that folder names "
    << "are not allowed to contain dots and file names "
    << "are not allowed to start with dots. "
    << "There may be additional restrictions "
    << "on file names added for security reasons.";
    result[WebAPI::Result::error] = out.str();
    return global.response.writeResponse(result);
  }
  if (FileOperations::isFolderUnsecure(this->relativePhysicalFileName)) {
    return this->processFolder();
  }
  return this->processFile(generateLinkToCalculatorOnMissingFile);
}

void WebWorker::pauseIfRequested() {
  this->pauseWorker.lock();
  // If pause was requested, here we block.
  this->pauseWorker.unlock();
}

void WebWorker::resetMutexProcesses() {
  STACK_TRACE("WebWorker::resetMutexProcesses");
  this->pauseWorker.resetNoAllocation();
  this->writingReportFile.resetNoAllocation();
}

void WebWorker::releaseKeepInUseFlag() {
  STACK_TRACE("WebWorker::releaseKeepInUseFlag");
  this->pipeWorkerToServerTimerPing.release();
  this->pipeWorkerToServerControls.release();
  this->workerToWorkerRequestIndicator.release();
  this->pipeWorkerToWorkerStatus.release();
  this->pauseWorker.release();
  this->writingReportFile.release();
  this->millisecondsLastPingServerSideOnly = - 1;
  WebServer::release(this->connectedSocketID);
}

void WebWorker::release() {
  this->releaseKeepInUseFlag();
  this->flagInUse = false;
}

void WebWorker::getIndicatorOnTimeout(
  JSData& output, const std::string& message
) {
  STACK_TRACE("WebWorker::getIndicatorOnTimeout");
  global
  << Logger::blue
  << "Computation timeout, sending progress indicator instead of output. "
  << Logger::endL;
  std::stringstream timeOutComments;
  output[WebAPI::Result::timeOut] = true;
  timeOutComments << message;
  if (global.response.flagBanProcessMonitoring) {
    timeOutComments
    << "Monitoring computations is not allowed on this server.<br> "
    << "Please note that monitoring computations "
    << "is the default behavior, so the "
    << "owners of the server must have explicitly banned monitoring. ";
    output[WebAPI::Result::timeOutComments] = timeOutComments.str();
  } else if (!global.response.flagReportDesired) {
    timeOutComments << "Monitoring computations not desired by user. ";
    output[WebAPI::Result::timeOutComments] = timeOutComments.str();
  } else {
    output[WebAPI::Result::workerId] = this->workerId;
    output[WebAPI::Result::timeOutComments] = timeOutComments.str();
  }
}

void WebWorker::writeAfterTimeoutShowIndicator(const std::string& message) {
  STACK_TRACE("WebWorker::writeAfterTimeoutShowIndicator");
  global << "Long computation, about to display indicator. " << Logger::endL;
  JSData result;
  this->getIndicatorOnTimeout(result, message);
  if (this->indexInParent < 0) {
    global.fatal
    << "Index of worker is smaller than 0, this shouldn't happen. "
    << global.fatal;
  }
  this->writeToBodyJSON(result);
  JSData progressReport;
  global.toJSONProgressReport(progressReport);
  this->writeAfterTimeoutProgress(progressReport, true);
  this->sendPending();
  for (int i = 0; i < this->parent->allWorkers.size; i ++) {
    if (i != this->indexInParent) {
      this->parent->allWorkers[i].release();
    }
  }
  this->parent->release(this->connectedSocketID);
}

std::string WebWorker::toStringAddressRequest() const {
  std::stringstream out;
  out << "Address = ";
  if (this->addressComputed == "") {
    out << "[empty]";
  } else {
    out << this->addressComputed;
  }
  out << "; ";
  out << " request = ";
  if (global.requestType == "") {
    out << "[empty]";
  } else {
    out << global.requestType;
  }
  return out.str();
}

std::string WebWorker::toStringStatus() const {
  std::stringstream out;
  out << "<br>Worker " << this->indexInParent + 1;
  if (this->flagExited) {
    out << ", <b style = 'color:red'>exited</b>";
  }
  if (this->flagInUse) {
    if (this->parent->activeWorker == this->indexInParent) {
      out << ", <b style='color:green'>current process</b>";
    } else {
      out << ", <b>in use</b>";
    }
    out
    << ", <a href='calculator?request=monitor&mainInput="
    << this->indexInParent + 1
    << "'>monitor process "
    << this->indexInParent + 1
    << "</a>";
  } else {
    out << ", not in use";
  }
  if (this->displayUserInput != "") {
    out
    << ", user input: <span style = 'color:blue'>"
    << this->displayUserInput
    << "</span>";
  }
  out << ", connection " << this->connectionID << ", process ID: ";
  if (this->processPID != 0) {
    out << this->processPID;
  } else {
    out << "(not accessible)";
  }
  out << ", socketID: ";
  if (this->connectedSocketID == - 1) {
    out
    << "released in current process, value before release: "
    << this->connectedSocketIDLastValueBeforeRelease;
  } else {
    out << this->connectedSocketID;
  }
  out << ". ";
  out
  << "Server time at last ping: "
  << this->millisecondsLastPingServerSideOnly
  << " milliseconds. ";
  if (this->pingMessage != "") {
    out << " Message at last ping: " << this->pingMessage;
  }
  if (this->status != "") {
    out << "<br><b style ='color:red'>Status: " << this->status << "</b><br>";
  }
  out
  << "Pipes: "
  << this->pipeWorkerToServerControls.toString()
  << ", "
  << this->pipeWorkerToServerTimerPing.toString()
  << ", "
  << this->workerToWorkerRequestIndicator.toString()
  << ", "
  << this->pipeWorkerToWorkerStatus.toString();
  out
  << ". MutexProcesses: "
  << this->pauseWorker.toString()
  << ", "
  << this->writingReportFile.toString();
  out << ", user address: " << this->userAddress << ".";
  return out.str();
}

bool WebServer::checkConsistency() {
  if (this->flagDeallocated) {
    // The web server is global, if deallocated this is likely
    // a crash outside of main(). We therefore abstain from using logging.
    std::cout << "Use after free of WebServer." << std::endl;
    std::exit(2);
  }
  return true;
}

void WebServer::releaseEverything() {
  this->checkConsistency();
  this->transportLayerSecurity.free();
  for (int i = 0; i < this->allWorkers.size; i ++) {
    this->allWorkers[i].release();
  }
  this->activeWorker = - 1;
  if (global.flagServerDetailedLog) {
    global
    << Logger::red
    << "Detail: "
    << "About to close socket: "
    << this->listeningSocketHTTP
    << ". "
    << Logger::endL;
  }
  for (int i = 0; i < this->allListeningSockets.size(); i ++) {
    int socket = this->allListeningSockets.keys[i];
    close(socket);
    if (global.flagServerDetailedLog) {
      global
      << Logger::red
      << "Detail: "
      << "Closed socket: "
      << socket
      << ". "
      << Logger::endL;
    }
  }
  this->listeningSocketHTTP = - 1;
  this->listeningSocketHTTPSBuiltIn = - 1;
  this->listeningSocketHTTPSOpenSSL = - 1;
}

WebServer::~WebServer() {
  this->flagDeallocated = true;
  TransportLayerSecurityOpenSSL::freeContextGlobal();
}

WebServer::WebServer() {
  this->flagDeallocated = false;
  this->activeWorker = - 1;
  this->timeLastExecutableModification = - 1;
  this->listeningSocketHTTP = - 1;
  this->listeningSocketHTTPSBuiltIn = - 1;
  this->listeningSocketHTTPSDefault = - 1;
  this->listeningSocketHTTPSOpenSSL = - 1;
  this->highestSocketNumber = - 1;
  this->maximumWorkersPerIPAdress = 24;
  // We need to open several pipes for each worker.
  // There's an upper limit for the number of pipes/files we can have open
  // increasing this to 84 and more is known to crash on ubuntu with default
  // user limits.
  this->maximumTotalUsedWorkers = 40;
  this->webServerPingIntervalInSeconds = 10;
  this->previousServerStatReport = 0;
  this->previousServerStatDetailedReport = 0;
  this->processIdServer = - 1;
  this->statistics.pingConnections = 0;
  this->statistics.allConnections = 0;
  this->statistics.standaloneServerRequests = 0;
  this->statistics.pingRequests = 0;
  this->statistics.allRequests = 0;
  this->statistics.processesReaped = 0;
  this->statistics.processKilled = 0;
  this->statistics.workersNormallyExited = 0;
  this->statistics.failedSelectsSoFar = 0;
  this->statistics.successfulSelectsSoFar = 0;
}

void WebServer::signal_SIGCHLD_handler(int s) {
  (void) s;
  if (global.flagIsChildProcess) {
    return;
  }
  if (global.flagServerDetailedLog) {
    global << "Detail: webServer received SIGCHLD signal. " << Logger::endL;
  }
  global.server().reapChildren();
}

void WebServer::reapChildren() {
  // Attention: this function is executed simultaneously with the
  // main execution path. Any non-standard operations here may be racy.
  // Please avoid allocating RAM memory outside of the stack.
  // Do not use anything that is not thread-safe here.
  // In particular, do not use any loggers.
  int waitResult = 0;
  int exitFlags = WNOHANG | WEXITED;
  this->checkConsistency();
  do {
    waitResult = waitpid(- 1, nullptr, exitFlags);
    if (waitResult > 0) {
      for (int i = 0; i < this->allWorkers.size; i ++) {
        if (this->allWorkers[i].processPID == waitResult) {
          this->allWorkers[i].flagExited = true;
          this->statistics.processesReaped ++;
        }
      }
    }
  } while (waitResult > 0);
}

WebWorker& WebServer::getActiveWorker() {
  STACK_TRACE("WebServer::getActiveWorker");
  if (this->activeWorker < 0 || this->activeWorker >= this->allWorkers.size) {
    global.fatal
    << "Active worker index is "
    << this->activeWorker
    << " however I have "
    << this->allWorkers.size
    << " workers. "
    << global.fatal;
  }
  return this->allWorkers[this->activeWorker];
}

void WebServer::wrapUp() {
  STACK_TRACE("WebServer::wrapUp");
  if (global.server().activeWorker == - 1) {
    return;
  }
  global.server().getActiveWorker().wrapUpConnection();
  global.server().activeWorker = - 1;
}

void WebServer::releaseActiveWorker() {
  STACK_TRACE("WebServer::releaseActiveWorker");
  if (this->activeWorker == - 1) {
    return;
  }
  this->getActiveWorker().release();
  this->activeWorker = - 1;
}

void WebServer::workerTimerPing(int64_t pingTime) {
  if (global.server().activeWorker == - 1) {
    if (!global.flagComputationFinishedAllOutputSentClosing) {
      global.fatal
      << "No active worker yet the computation is not complete. "
      << global.fatal;
    }
    return;
  }
  std::stringstream outTimestream;
  outTimestream << pingTime;
  global.server().getActiveWorker().pipeWorkerToServerTimerPing.
  writeOnceAfterEmptying(outTimestream.str(), false);
}

void WebServer::releaseNonActiveWorkers() {
  STACK_TRACE("WebServer::releaseNonActiveWorkers");
  for (int i = 0; i < this->allWorkers.size; i ++) {
    if (i != this->activeWorker) {
      this->allWorkers[i].releaseKeepInUseFlag();
    }
  }
}

void WebServer::releaseSocketsNonActiveWorkers() {
  STACK_TRACE("WebServer::releaseSocketsNonActiveWorkers");
  for (int i = 0; i < this->allWorkers.size; i ++) {
    if (i != this->activeWorker) {
      this->release(this->allWorkers[i].connectedSocketID);
    }
  }
}

bool WebServer::emergencyRemoval_LastCreatedWorker() {
  this->getActiveWorker().release();
  this->activeWorker = - 1;
  this->allWorkers.setSize(this->allWorkers.size - 1);
  return false;
}

std::string WebServer::toStringWorkerToWorker() {
  std::stringstream out;
  out << "W<->W" << this->activeWorker + 1 << ": ";
  return out.str();
}

bool WebServer::createNewActiveWorker() {
  STACK_TRACE("WebServer::createNewActiveWorker");
  if (this->activeWorker != - 1) {
    global.fatal
    << "Calling createNewActiveWorker "
    << "requires the active worker index to be - 1."
    << global.fatal;
    return false;
  }
  bool found = false;
  for (int i = 0; i < this->allWorkers.size; i ++) {
    if (!this->allWorkers[i].flagInUse) {
      this->activeWorker = i;
      found = true;
      break;
    }
  }
  if (this->activeWorker == - 1) {
    this->activeWorker = this->allWorkers.size;
    this->allWorkers.setSize(this->allWorkers.size + 1);
  }
  this->getActiveWorker().indexInParent = this->activeWorker;
  this->getActiveWorker().parent = this;
  this->getActiveWorker().pingMessage = "";
  if (found) {
    this->allWorkers[this->activeWorker].flagInUse = true;
    this->allWorkers[this->activeWorker].flagExited = false;
    return true;
  }
  this->getActiveWorker().release();
  this->allWorkers[this->activeWorker].flagInUse = false;
  // <-until everything is initialized, we cannot be in use.
  std::stringstream stowstream;
  std::stringstream wtosStream;
  stowstream << "S->W" << this->activeWorker + 1 << ": ";
  wtosStream << "W" << this->activeWorker + 1 << "->S: ";
  std::string stow = stowstream.str();
  std::string wtos = wtosStream.str();
  std::string wtow = this->toStringWorkerToWorker();
  WebWorker& worker = this->getActiveWorker();
  if (
    !worker.workerToWorkerRequestIndicator.createMe(
      wtow + "request-indicator", false, false, true
    )
  ) {
    global
    << "Failed to create pipe: "
    << worker.workerToWorkerRequestIndicator.name
    << "\n";
    return this->emergencyRemoval_LastCreatedWorker();
  }
  if (
    !worker.pipeWorkerToServerTimerPing.createMe(
      wtos + "ping", false, false, true
    )
  ) {
    global
    << "Failed to create pipe: "
    << worker.pipeWorkerToServerTimerPing.name
    << "\n";
    return this->emergencyRemoval_LastCreatedWorker();
  }
  if (
    !worker.pipeWorkerToServerControls.createMe(
      wtos + "controls", false, false, true
    )
  ) {
    global
    << "Failed to create pipe: "
    << worker.pipeWorkerToServerControls.name
    << "\n";
    return this->emergencyRemoval_LastCreatedWorker();
  }
  if (
    !worker.pipeWorkerToWorkerStatus.createMe(
      wtos + "worker status", false, false, true
    )
  ) {
    global
    << "Failed to create pipe: "
    << worker.pipeWorkerToWorkerStatus.name
    << "\n";
    return this->emergencyRemoval_LastCreatedWorker();
  }
  this->allWorkers[this->activeWorker].workerId = "";
  this->allWorkers[this->activeWorker].flagInUse = true;
  this->allWorkers[this->activeWorker].flagExited = false;
  return true;
}

std::string WebServer::toStringLastErrorDescription() {
  std::stringstream out;
  out << strerror(errno) << ". ";
  return out.str();
}

std::string WebServer::toStringStatusActive() {
  STACK_TRACE("WebServer::toStringStatusActive");
  if (this->activeWorker == - 1) {
    return "server.";
  }
  std::stringstream out;
  if (this->activeWorker != this->getActiveWorker().indexInParent) {
    global.fatal << "Bad index in parent!" << global.fatal;
  }
  out << this->getActiveWorker().toStringStatus();
  return out.str();
}

std::string WebServer::toStringConnectionSummary() {
  STACK_TRACE("WebServer::toStringConnectionSummary");
  std::stringstream out;
  TimeWrapper now;
  now.assignLocalTime();
  out
  << "<b>Server status.</b> Server time: local: "
  << now.toStringLocal()
  << ", gm: "
  << now.toStringGM()
  << ".<br>";
  int64_t timeRunning = - 1;
  if (this->activeWorker < 0 || this->activeWorker >= this->allWorkers.size) {
    timeRunning = global.getElapsedMilliseconds();
  } else {
    timeRunning = this->getActiveWorker().millisecondsLastPingServerSideOnly;
  }
  out
  << (double(timeRunning) / 1000)
  << " seconds = "
  << TimeWrapper::toStringSecondsToDaysHoursSecondsString(
    timeRunning / 1000, false, false
  )
  << " web server uptime. ";
  double connectionsPerDay = static_cast<double>(
    this->statistics.allConnections - this->statistics.pingConnections
  ) / (double(timeRunning) / 1000) *
  60 *
  60 *
  24;
  double requestsPerDayCalculator = static_cast<double>(
    this->statistics.allRequests -
    this->statistics.pingRequests -
    this->statistics.standaloneServerRequests
  ) / (double(timeRunning) / 1000) *
  60 *
  60 *
  24;
  double requestsPerDayTotal = static_cast<double>(
    this->statistics.allRequests
  ) / (double(timeRunning) / 1000) *
  60 *
  60 *
  24;
  out
  << "<hr>"
  << (
    this->statistics.allRequests -
    this->statistics.pingRequests -
    this->statistics.standaloneServerRequests
  )
  << " calculator http requests. "
  << this->statistics.standaloneServerRequests
  << " non-calculator web server requests, "
  << this->statistics.pingRequests
  << " ping requests "
  << "["
  << this->statistics.allRequests
  << " total]. "
  << "~"
  << FloatingPoint::doubleToString(requestsPerDayCalculator, 0)
  << " calculator requests per day. "
  << "~"
  << FloatingPoint::doubleToString(requestsPerDayTotal, 0)
  << " all requests per day (pings and non-calculator actions included). "
  << "<hr>"
  << "~"
  << FloatingPoint::doubleToString(connectionsPerDay, 0)
  << " calculator connections per day. "
  << "~"
  << (this->statistics.allConnections - this->statistics.pingConnections)
  << " http connections, "
  << this->statistics.pingConnections
  << " ping connections. "
  << "Timed-out connections are excluded. ";
  out
  << "<hr>"
  << this->maximumTotalUsedWorkers
  << " global maximum of simultaneous non-closed connections allowed. "
  << "When the limit is exceeded, "
  << "all connections except a randomly "
  << "chosen one will be terminated. "
  << "<br>"
  << this->maximumWorkersPerIPAdress
  << " maximum simultaneous connection per IP address. "
  << "When the limit is exceeded, "
  << "all connections from that IP address are terminated. ";
  return out.str();
}

std::string WebServer::toStringStatusForLogFile() {
  STACK_TRACE("WebServer::toStringStatusForLogFile");
  std::stringstream out;
  out << this->toStringConnectionSummary();
  int totalInUse = 0;
  for (int i = 0; i < this->allWorkers.size; i ++) {
    if (this->allWorkers[i].flagInUse) {
      totalInUse ++;
    }
  }
  out
  << "<hr>Currently, there are "
  << totalInUse
  << " worker(s) in use. The peak number of "
  << "worker(s)/concurrent connections was "
  << this->allWorkers.size
  << ". ";
  out
  << "<br>kill commands: "
  << this->statistics.processKilled
  << ", processes reaped: "
  << this->statistics.processesReaped
  << ", normally reclaimed workers: "
  << this->statistics.workersNormallyExited
  << ", all connections so far: "
  << this->statistics.allConnections
  << ", pings so far: "
  << this->statistics.pingConnections;
  return out.str();
}

std::string WebServer::toStringStatusAll() {
  STACK_TRACE("WebServer::toStringStatusAll");
  std::stringstream out;
  if (!global.userDefaultHasAdminRights()) {
    out << this->toStringConnectionSummary();
    return out.str();
  }
  out << this->toStringStatusForLogFile();
  out << "<hr>";
  out
  << "<a href='/LogFiles/server_starts_and_unexpected_restarts.html'>"
  << "Log files</a><br>";
  out
  << "<a href='/LogFiles/logs/"
  << GlobalVariables::getDateForLogFiles()
  << "/'>"
  << "Current log files</a><hr>";
  if (this->activeWorker == - 1) {
    out << "This is a server process.";
  } else {
    out
    << "This is a worker process. Active worker: "
    << this->activeWorker + 1
    << ". ";
    out << "<br>" << this->toStringStatusActive();
  }
  for (int i = 0; i < this->allWorkers.size; i ++) {
    WebWorker& currentWorker = this->allWorkers[i];
    if (!currentWorker.flagInUse) {
      continue;
    }
    currentWorker.pipeWorkerToWorkerStatus.readOnceWithoutEmptying(false);
    currentWorker.status =
    currentWorker.pipeWorkerToWorkerStatus.getLastRead();
  }
  out << "<hr>";
  out << "Connections: " << this->currentlyConnectedAddresses.toString();
  for (int i = 0; i < this->allWorkers.size; i ++) {
    out << "<hr>" << this->allWorkers[i].toStringStatus();
  }
  return out.str();
}

bool WebServer::restartNeeded() {
  STACK_TRACE("WebServer::restartNeeded");
  struct stat fileStatistic;
  if (stat(global.physicalNameExecutableWithPath.c_str(), &fileStatistic) != 0)
  {
    global
    << Logger::red
    << "Failed to extract executable statistics from: "
    << global.physicalNameExecutableWithPath
    << "."
    << Logger::endL;
    return false;
  }
  bool result = false;
  if (
    this->timeLastExecutableModification != - 1 &&
    this->timeLastExecutableModification != fileStatistic.st_ctime
  ) {
    result = true;
  }
  this->timeLastExecutableModification = fileStatistic.st_ctime;
  return result;
}

void WebServer::stopKillAll() {
  if (
    global.logs.logType != GlobalVariables::LogData::type::server &&
    global.logs.logType != GlobalVariables::LogData::type::serverMonitor
  ) {
    global.fatal
    << "Server restart is allowed only to "
    << "the server/monitor processes. "
    << global.fatal;
  }
  global << Logger::red << "Server restart requested. " << Logger::endL;
  global
  << "Sending kill signal to all copies of the calculator. "
  << Logger::endL;
  for (int i = 0; i < this->allWorkers.size; i ++) {
    this->terminateChildSystemCall(i);
  }
  SignalsInfrastructure::signals().unblockSignals();
  global << "Waiting for child processes to exit. " << Logger::endL;
  int workersStillInUse = 0;
  int waitAttempts = 0;
  int maximumWaitAttempts = 30;
  while (true) {
    workersStillInUse = 0;
    for (int i = 0; i < this->allWorkers.size; i ++) {
      if (!this->allWorkers[i].flagExited && this->allWorkers[i].flagInUse) {
        workersStillInUse ++;
        break;
      }
    }
    if (workersStillInUse == 0) {
      break;
    }
    waitAttempts ++;
    if (waitAttempts > maximumWaitAttempts) {
      global
      << Logger::red
      << "Child exit timeout: made "
      << waitAttempts
      << " attempts to exit. "
      << Logger::endL;
      break;
    }
    global
    << Logger::blue
    << "Still waiting on "
    << workersStillInUse
    << " workers to finish. "
    << Logger::endL;
    global.fallAsleep(1000000);
  }
  this->releaseEverything();
  this->releaseEverything();
  exit(0);
}

void WebServer::stop() {
  SignalsInfrastructure::signals().unblockSignals();
  this->releaseEverything();
  this->releaseEverything();
  exit(0);
}

void WebServer::initializePortsITry() {
  STACK_TRACE("WebServer::initializePortsITry");
  this->portHTTP = global.configuration[Configuration::portHTTP].stringValue;
  if (!global.flagSSLAvailable) {
    return;
  }
  this->portHTTPSOpenSSL =
  global.configuration[Configuration::portHTTPSOpenSSL].stringValue;
  this->portHTTPSBuiltIn =
  global.configuration[Configuration::portHTTPSBuiltIn].stringValue;
  this->portHTTPSDefault = this->portHTTPSOpenSSL;
  if (this->portHTTPSBuiltIn != "") {
    this->transportLayerSecurity.flagBuiltInTLSAvailable = true;
  }
  for (
    const ActAsWebServerOnly& configuration :
    global.web.actAsWebServerOnlyForTheseHosts.values
  ) {
    if (configuration.portHTTP != "") {
      this->additionalPorts.setKeyValue(configuration.portHTTP, - 1);
    }
    if (configuration.portHTTPS != "") {
      this->additionalPorts.setKeyValue(configuration.portHTTPS, - 1);
    }
  }
}

void WebServer::initializeListeningSockets() {
  STACK_TRACE("WebServer::initializeListeningSockets");
  this->highestSocketNumber = - 1;
  if (this->listeningSocketHTTP != - 1) {
    this->allListeningSockets.setKeyValue(
      this->listeningSocketHTTP, this->portHTTP
    );
  }
  if (this->listeningSocketHTTPSBuiltIn != - 1) {
    this->allListeningSockets.setKeyValue(
      this->listeningSocketHTTPSBuiltIn, this->portHTTPSBuiltIn
    );
    this->listeningSocketHTTPSDefault = this->listeningSocketHTTPSBuiltIn;
  }
  if (this->listeningSocketHTTPSOpenSSL != - 1) {
    this->allListeningSockets.setKeyValue(
      this->listeningSocketHTTPSOpenSSL, this->portHTTPSOpenSSL
    );
    this->listeningSocketHTTPSDefault = this->listeningSocketHTTPSOpenSSL;
  }
  for (int i = 0; i < this->additionalPorts.size(); i ++) {
    this->allListeningSockets.setKeyValue(
      this->additionalPorts.values[i], this->additionalPorts.keys[i]
    );
  }
  this->highestSocketNumber = - 1;
  for (int i = 0; i < this->allListeningSockets.size(); i ++) {
    this->highestSocketNumber =
    MathRoutines::maximum(
      this->allListeningSockets.keys[i], this->highestSocketNumber
    );
    if (
      listen(
        this->allListeningSockets.keys[i], Listener::maximumPendingConnections
      ) ==
      - 1
    ) {
      global.fatal
      << "Listen function failed on socket: "
      << this->allListeningSockets[i]
      << global.fatal;
    }
  }
}

void WebServer::releaseWorkerSideResources() {
  STACK_TRACE("WebServer::releaseWorkerSideResources");
  if (global.flagServerDetailedLog) {
    global
    << Logger::red
    << "Detail: server about to RELEASE active workder. "
    << Logger::endL;
  }
  this->release(this->getActiveWorker().connectedSocketID);
  if (global.flagServerDetailedLog) {
    global
    << Logger::green
    << "Detail: server RELEASED active worker. "
    << Logger::endL;
  }
  // <-release socket communication is handled by the worker.
  this->activeWorker = - 1;
  // <-The active worker is needed only in the child process.
}

bool WebServer::requiresLogin(
  const std::string& inputRequest, const std::string& inputAddress
) {
  STACK_TRACE("WebServer::requiresLogin");
  if (inputAddress == global.displayNameExecutable) {
    if (this->requestsNotNeedingLogin.contains(inputRequest)) {
      return false;
    }
  }
  for (int i = 0; i < this->addressStartsNotNeedingLogin.size; i ++) {
    if (
      StringRoutines::stringBeginsWith(
        inputAddress, this->addressStartsNotNeedingLogin[i]
      )
    ) {
      return false;
    }
  }
  return true;
}

void WebServer::fperror_sigaction[[noreturn]](int signal) {
  (void) signal;
  global.fatal << "Fatal arithmetic error. " << global.fatal;
  exit(0);
}

void WebServer::terminateProcessId(int processId) {
  kill(processId, SIGKILL);
}

void WebServer::terminateChildSystemCall(int i) {
  // Signal lock not needed: signals are unlocked outside of the select loop.
  if (!this->allWorkers[i].flagInUse || this->allWorkers[i].flagExited) {
    return;
  }
  this->markChildNotInUse(i);
  if (this->allWorkers[i].processPID > 0) {
    if (global.flagServerDetailedLog) {
      global
      << "Detail: "
      << "killing child index: "
      << i
      << "."
      << Logger::endL;
    }
    this->terminateProcessId(this->allWorkers[i].processPID);
    this->allWorkers[i].processPID = - 1;
  }
}

void WebServer::handleTooManyConnections(
  const std::string& incomingUserAddress
) {
  STACK_TRACE("WebServer::handleTooManyConnections");
  if (global.flagIsChildProcess) {
    return;
  }
  if (global.flagServerDetailedLog) {
    global
    << Logger::red
    << "Detail: "
    << "too many connections handler start. "
    << Logger::endL;
  }
  MonomialWrapper<std::string, HashFunctions::hashFunction<std::string> >
  incomingAddress(incomingUserAddress);
  bool purgeIncomingAddress = (
    this->currentlyConnectedAddresses.getCoefficientOf(incomingAddress) >
    this->maximumWorkersPerIPAdress
  );
  if (!purgeIncomingAddress) {
    return;
  }
  List<double> times;
  List<int> indices;
  for (int i = 0; i < this->allWorkers.size; i ++) {
    if (this->allWorkers[i].flagInUse) {
      if (this->allWorkers[i].userAddress == incomingAddress) {
        times.addOnTop(this->allWorkers[i].millisecondsServerAtWorkerStart);
        indices.addOnTop(i);
      }
    }
  }
  times.quickSortAscending(nullptr, &indices);
  if (!SignalsInfrastructure::signals().flagSignalsAreBlocked) {
    global.fatal
    << "Signals must be blocked at this point of code. "
    << global.fatal;
  }
  for (int j = 0; j < times.size; j ++) {
    // Child processes exit completely when their sigchld
    // signal has been handled.
    // Signal processing is blocked outside of the select loop.
    // Therefore
    // any children that have exited properly are still not reaped
    // at this point in code.
    // In particular, it should not be possible to terminate by accident
    // a pid that is not owned by the server.
    this->terminateChildSystemCall(indices[j]);
    this->statistics.processKilled ++;
    std::stringstream errorStream;
    errorStream
    << "Terminating child "
    << indices[j] + 1
    << " with PID "
    << this->allWorkers[indices[j]].processPID
    << ": address: "
    << incomingAddress
    << " opened more than "
    << this->maximumWorkersPerIPAdress
    << " simultaneous connections. ";
    this->allWorkers[indices[j]].pingMessage = errorStream.str();
    global << Logger::red << errorStream.str() << Logger::endL;
  }
  if (global.flagServerDetailedLog) {
    global
    << Logger::green
    << "Detail: connection cleanup successful. "
    << Logger::endL;
  }
}

void WebServer::markChildNotInUse(int childIndex) {
  WebWorker& worker = this->allWorkers[childIndex];
  worker.flagInUse = false;
  this->currentlyConnectedAddresses.subtractMonomial(worker.userAddress, 1);
  this->workerIds.removeKey(worker.workerId);
  worker.workerId = "";
}

void WebServer::processOneChildMessage(int childIndex, int& outputTotalInUse) {
  std::string messageString =
  this->allWorkers[childIndex].pipeWorkerToServerControls.getLastRead();
  this->markChildNotInUse(childIndex);
  std::stringstream commentsOnFailure;
  JSData workerMessage;
  if (!workerMessage.parse(messageString, false, &commentsOnFailure)) {
    global
    << Logger::red
    << "Worker "
    << childIndex + 1
    << " sent corrupted result message: "
    << messageString
    << ". Marking for reuse. "
    << Logger::endL;
  } else {
    global
    << Logger::green
    << "Worker "
    << childIndex + 1
    << " done with message: "
    << messageString
    << ". Marking for reuse. "
    << Logger::endL;
  }
  outputTotalInUse --;
  this->statistics.workersNormallyExited ++;
  if (
    workerMessage[WebServer::Statististics::allRequestsString].elementType ==
    JSData::Type::tokenLargeInteger &&
    workerMessage[WebServer::Statististics::pingRequestsString].elementType ==
    JSData::Type::tokenLargeInteger
  ) {
    int incomingAllRequests =
    workerMessage[WebServer::Statististics::allRequestsString].integerValue.
    getElement().getIntValueTruncated();
    int incomingPingRequests =
    workerMessage[WebServer::Statististics::pingRequestsString].integerValue.
    getElement().getIntValueTruncated();
    this->statistics.allRequests += incomingAllRequests;
    this->statistics.pingRequests += incomingPingRequests;
    if (incomingPingRequests > 0) {
      this->statistics.pingConnections ++;
    }
  }
  if (
    workerMessage[WebServer::Statististics::standaloneServerRequestsString].
    elementType ==
    JSData::Type::tokenLargeInteger
  ) {
    int incomingStandaloneRequests =
    workerMessage[WebServer::Statististics::standaloneServerRequestsString].
    integerValue.getElement().getIntValueTruncated();
    this->statistics.standaloneServerRequests += incomingStandaloneRequests;
  }
  if (
    workerMessage["stopNeeded"].isTrueRepresentationInJSON() ||
    workerMessage["restartNeeded"].isTrueRepresentationInJSON()
  ) {
    this->stopKillAll();
  }
}

void WebServer::recycleOneChild(int childIndex, int& numberInUse) {
  if (!this->allWorkers[childIndex].flagInUse) {
    return;
  }
  WebWorker & currentWorker = this->allWorkers[childIndex];
  PipePrimitive& currentControlPipe = currentWorker.pipeWorkerToServerControls;
  if (currentControlPipe.flagReadEndBlocks) {
    global.fatal
    << "Pipe: "
    << currentControlPipe.toString()
    << " has blocking read end. "
    << global.fatal;
  }
  currentControlPipe.readOnceNoFailure(true);
  if (currentControlPipe.lastRead.size > 0) {
    this->processOneChildMessage(childIndex, numberInUse);
  } else {
    global
    << Logger::orange
    << "Worker "
    << childIndex + 1
    << " not done yet. "
    << Logger::endL;
  }
  PipePrimitive& currentPingPipe = currentWorker.pipeWorkerToServerTimerPing;
  if (currentPingPipe.flagReadEndBlocks) {
    global.fatal
    << "Pipe: "
    << currentPingPipe.toString()
    << " has blocking read end. "
    << global.fatal;
  }
  currentPingPipe.readOnceNoFailure(true);
  if (currentPingPipe.lastRead.size > 0) {
    currentWorker.pingMessage = currentPingPipe.getLastRead();
    currentWorker.millisecondsLastPingServerSideOnly =
    global.getElapsedMilliseconds();
    if (currentWorker.pingMessage != "") {
      global
      << Logger::blue
      << "Worker "
      << childIndex + 1
      << " ping: "
      << currentWorker.pingMessage
      << ". "
      << Logger::endL;
    }
    return;
  }
  if (global.millisecondsNoPingBeforeChildIsPresumedDead <= 0) {
    return;
  }
  int64_t millisecondsElapsed =
  global.getElapsedMilliseconds() -
  currentWorker.millisecondsLastPingServerSideOnly;
  if (millisecondsElapsed <= global.millisecondsNoPingBeforeChildIsPresumedDead)
  {
    return;
  }
  this->terminateChildSystemCall(childIndex);
  std::stringstream pingTimeoutStream;
  pingTimeoutStream
  << millisecondsElapsed
  << " milliseconds passed since worker "
  << childIndex + 1
  << " last pinged the server; "
  << "the maximum allowed is: "
  << global.millisecondsNoPingBeforeChildIsPresumedDead
  << ". ";
  pingTimeoutStream
  << "Killing connection "
  << currentWorker.connectionID
  << ", pid: "
  << currentWorker.processPID
  << ". ";
  global << Logger::red << pingTimeoutStream.str() << Logger::endL;
  currentWorker.pingMessage =
  "<b style =\"color:red\">" + pingTimeoutStream.str() + "</b>";
  numberInUse --;
  this->statistics.processKilled ++;
}

void WebServer::handleTooManyWorkers(int& workerCount) {
  if (workerCount <= this->maximumTotalUsedWorkers) {
    if (global.flagServerDetailedLog) {
      global
      << Logger::green
      << "Detail: recycleChildrenIfPossible exit point 1. "
      << Logger::endL;
    }
    return;
  }
  for (int i = 0; i < this->allWorkers.size && workerCount > 1; i ++) {
    if (!this->allWorkers[i].flagInUse) {
      continue;
    }
    this->terminateChildSystemCall(i);
    std::stringstream errorStream;
    errorStream
    << "Terminating child "
    << i + 1
    << " with PID "
    << this->allWorkers[i].processPID
    << ": too many workers in use. ";
    this->allWorkers[i].pingMessage = errorStream.str();
    global << Logger::red << errorStream.str() << Logger::endL;
    workerCount --;
    this->statistics.processKilled ++;
  }
  if (global.flagServerDetailedLog) {
    global
    << Logger::green
    << "Detail: recycleChildrenIfPossible exit point 2. "
    << Logger::endL;
  }
}

void WebServer::recycleChildrenIfPossible() {
  // Listen for children who have exited properly.
  // This might need to be rewritten: I wasn't able to make this work with any
  // mechanism other than pipes.
  STACK_TRACE("WebServer::recycleChildrenIfPossible");
  if (global.flagIsChildProcess) {
    return;
  }
  if (global.flagServerDetailedLog) {
    global
    << Logger::red
    << "Detail: recycleChildrenIfPossible start. "
    << Logger::endL;
  }
  int inUseTotal = 0;
  for (int i = 0; i < this->allWorkers.size; i ++) {
    if (this->allWorkers[i].flagInUse) {
      inUseTotal ++;
    }
  }
  for (int i = 0; i < this->allWorkers.size; i ++) {
    this->recycleOneChild(i, inUseTotal);
  }
  this->handleTooManyWorkers(inUseTotal);
}

bool WebServer::initializePrepareWebServerAll() {
  STACK_TRACE("WebServer::initializePrepareWebServerAll");
  this->initializePortsITry();
  if (!this->initializeBindToPorts()) {
    return false;
  }
  this->initializeSignals();
  this->initializeListeningSockets();
  return true;
}

bool WebServer::initializeBindToPorts() {
  STACK_TRACE("WebServer::initializeBindToPorts");
  int unusedPort = 0;
  if (
    !Listener::initializeBindToOnePort(
      this->portHTTP, this->listeningSocketHTTP, unusedPort, false
    )
  ) {
    return false;
  }
  global
  << "Running at: "
  << Logger::yellow
  << "http://localhost:"
  << this->portHTTP
  << Logger::endL;
  if (this->portHTTPSBuiltIn != "") {
    if (
      !Listener::initializeBindToOnePort(
        this->portHTTPSBuiltIn,
        this->listeningSocketHTTPSBuiltIn,
        unusedPort,
        false
      )
    ) {
      return false;
    }
  }
  if (global.flagSSLAvailable) {
    if (
      !Listener::initializeBindToOnePort(
        this->portHTTPSOpenSSL,
        this->listeningSocketHTTPSOpenSSL,
        unusedPort,
        false
      )
    ) {
      return false;
    }
    global
    << "Running at: "
    << Logger::yellow
    << "https://localhost:"
    << this->portHTTPSOpenSSL
    << Logger::normalColor
    << " on listening socket: "
    << this->listeningSocketHTTPSOpenSSL
    << ". "
    << Logger::endL;
  }
  for (int i = 0; i < this->additionalPorts.size(); i ++) {
    const std::string& port = this->additionalPorts.keys[i];
    int& socket = this->additionalPorts.values[i];
    if (!Listener::initializeBindToOnePort(port, socket, unusedPort, false)) {
      return false;
    }
    int indexInTransportArray =
    this->transportLayerSecurity.additionalConfigurations.getIndex(port);
    if (indexInTransportArray < 0) {
      // Not an https port.
      global << "Listening to additional HTTP port: "
      << Logger::blue
      << port
      << Logger::normalColor
      << " on listening socket: "
      << socket
      << "."
      << Logger::endL;
      continue;
    }
    this->transportLayerSecurity.socketsToAdditionalConfigurations.setKeyValue(
      socket, indexInTransportArray
    );
    global
    << "Listening to additional HTTPS port: "
    << Logger::yellow
    << port
    << Logger::normalColor
    << " on listening socket: "
    << socket
    << ". "
    << Logger::endL;
  }
  return true;
}

void WebServer::initializeSignals() {
  SignalsInfrastructure::signals().initializeSignals();
}

void SignalsInfrastructure::initializeOneSignal(
  int signal, struct sigaction& output, void(*handler)(int)
) {
  if (sigemptyset(&output.sa_mask) == - 1) {
    global.fatal
    << "Failed to initialize signal mask. Crashing to let you know. "
    << global.fatal;
  }
  output.sa_sigaction = nullptr;
  output.sa_handler = handler;
  if (sigaction(signal, &output, nullptr) == - 1) {
    global.fatal
    << "Failed to register handler. "
    << "Crashing to let you know. "
    << global.fatal;
  }
}

void SignalsInfrastructure::initializeSignals() {
  STACK_TRACE("SignalsInfrastructure::initializeSignals");
  if (this->flagInitialized) {
    return;
  }
  sigfillset(&this->allSignals);
  if (sigemptyset(&this->SignalSEGV.sa_mask) == - 1) {
    global.fatal
    << "Failed to initialize SignalSEGV mask. Crashing to let you know. "
    << global.fatal;
  }
  this->SignalSEGV.sa_sigaction = &SystemFunctions::segfaultSigaction;
  this->SignalSEGV.sa_flags = SA_SIGINFO;
  if (sigaction(SIGSEGV, &SignalSEGV, nullptr) == - 1) {
    global.fatal
    << "Failed to register SIGSEGV handler "
    << "(segmentation fault (attempt to write memory without permission)). "
    << "Crashing to let you know. "
    << global.fatal;
  }
  // /////////////////////
  // catch floating point exceptions
  if (sigemptyset(&SignalFPE.sa_mask) == - 1) {
    global.fatal
    << "Failed to initialize SignalFPE mask. Crashing to let you know. "
    << global.fatal;
  }
  SignalFPE.sa_sigaction = nullptr;
  SignalFPE.sa_handler = &WebServer::fperror_sigaction;
  if (sigaction(SIGFPE, &SignalFPE, nullptr) == - 1) {
    global.fatal
    << "Failed to register SIGFPE handler "
    << "(fatal arithmetic error). "
    << "Crashing to let you know. "
    << global.fatal;
  }
  SignalChild.sa_flags = SA_NOCLDWAIT;
  SignalChild.sa_handler = &WebServer::signal_SIGCHLD_handler;
  // Reap all dead processes.
  if (sigaction(SIGCHLD, &SignalChild, nullptr) == - 1) {
    global.fatal
    << "Was not able to register SIGCHLD handler "
    << "(reaping child processes). "
    << "Crashing to let you know."
    << global.fatal;
  }
  this->flagInitialized = true;
}

extern void monitorWebServer(
  int pidServer, const std::string& pingAuthentication
);

void WebServer::writeVersionJSFile() {
  STACK_TRACE("WebServer::writeVersionJSFile");
  std::stringstream out;
  out
  << "\"use strict\"; "
  << "//This file is automatically generated, please do not modify.\n";
  out << "var serverInformation = {\n  ";
  out << "  version:\"" << global.buildVersionSimple << "\",\n";
  out << "};\n";
  out << "module.exports = {\nserverInformation,\n};\n";
  std::fstream fileStream;
  FileOperations::openFileCreateIfNotPresentVirtual(
    fileStream,
    "/calculator_html/server_information.js",
    false,
    true,
    false,
    false
  );
  fileStream << out.str();
}

TransportLayerSecurity& TransportLayerSecurity::defaultTLS_READ_ONLY() {
  return global.server().transportLayerSecurity;
}

int WebServer::daemon() {
  STACK_TRACE("WebServer::daemon");
  global.logs.logType = GlobalVariables::LogData::type::daemon;
  std::stringstream restartCommand;
  restartCommand << global.physicalNameExecutableWithPath << " ";
  bool prependServer = false;
  if (global.programArguments.size < 3) {
    prependServer = true;
  } else {
    prependServer = (global.programArguments[2] != MainFlags::server);
  }
  if (prependServer) {
    restartCommand << MainFlags::server << " ";
  }
  for (int i = 2; i < global.programArguments.size; i ++) {
    restartCommand << global.programArguments[i] << " ";
  }
  restartCommand << Configuration::serverAutoMonitor << " false ";
  int pidChild = - 1;
  while (true) {
    if (pidChild < 0) {
      pidChild = ForkCreator::forkRaw();
      if (pidChild == 0) {
        // Child process.
        global << Logger::orange
        << "Development daemon is starting command:"
        << Logger::endL;
        global << Logger::green << restartCommand.str() << Logger::endL;
        global.externalCommandStream(restartCommand.str());
        return 0;
      }
    }
    int exitStatus = - 1;
    int waitResult = waitpid(pidChild, &exitStatus, WNOHANG);
    if (waitResult > 0) {
      // Child exited.
      pidChild = - 1;
      if (exitStatus != 0) {
        global << Logger::red;
      } else {
        global << Logger::green;
      }
      global << "Server exited with status: " << exitStatus << Logger::endL;
      continue;
    } else if (waitResult < 0) {
      global
      << Logger::red
      << "Waitpid failed: this shouldn't happen. "
      << Logger::endL;
      pidChild = - 1;
    }
    if (pidChild > 0 && global.server().restartNeeded()) {
      global << Logger::red << "Restart is needed. " << Logger::endL;
      // Kill the process tree of the child process.
      int killResult = kill(pidChild, SIGTERM);
      global
      << Logger::red
      << "Kill signal sent to child: "
      << pidChild
      << ". "
      << Logger::endL;
      if (killResult != 0) {
        global
        << "Kill command failed: "
        << Logger::red
        << this->toStringLastErrorDescription()
        << Logger::endL;
      }
      global << Logger::blue << "Sleeping for 1 seconds." << Logger::endL;
      global.fallAsleep(1000000);
      global << "Pinging web server to reset sockets." << Logger::endL;
      WebClient webCrawler;
      webCrawler.initialize();
      webCrawler.pingCalculatorStatus("not authenticated");
      global << Logger::green << "Ping done." << Logger::endL;
    }
    global.fallAsleep(1000000);
  }
}

int WebServer::run() {
  STACK_TRACE("WebServer::run");
  global.relativePhysicalNameCrashReport = "crash_WebServerRun.html";
  WebServer::initializeRandomBytes();
  if (global.millisecondsMaxComputation <= 0) {
    global
    << Logger::purple
    << "************************"
    << Logger::endL
    << Logger::red
    << "WARNING: no computation time limit set. "
    << Logger::endL
    << Logger::purple
    << "************************"
    << Logger::endL;
  }
  if (global.millisecondsMaxComputation > 500000) {
    global
    << Logger::purple
    << "************************"
    << Logger::endL
    << Logger::red
    << "WARNING: computation time limit is high: "
    << (global.millisecondsMaxComputation / 1000)
    << " seconds. "
    << Logger::endL
    << Logger::purple
    << "************************"
    << Logger::endL;
  }
  global.logs.server.reset();
  global.logs.serverMonitor.reset();
  global.logs.worker.reset();
  Database::get().initialize(this->maximumTotalUsedWorkers);
  if (
    global.databaseType == DatabaseType::internal &&
    Database::get().localDatabase.processId == 0
  ) {
    // This is the database process.
    return 0;
  }
  this->processIdServer = getpid();
  List<unsigned char> pingBytes;
  this->pingAuthentication =
  "ping" + Crypto::Random::getRandomHexStringLeaveMemoryTrace(20);
  if (global.flagLocalhostConnectionMonitor) {
    global << Logger::green << "Start monitor process." << Logger::endL;
    this->processIdServer = ForkCreator::forkRaw();
    if (this->processIdServer < 0) {
      global.fatal << "Failed to create server process. " << global.fatal;
    }
    if (this->processIdServer > 0) {
      // This is the parent process.
      // The parent process monitors the child process.
      // The child process is the server that does all the useful work.
      // The parent process only does monitoring.
      global.logs.logType = GlobalVariables::LogData::type::serverMonitor;
      global.flagIsChildProcess = true;
      monitorWebServer(this->processIdServer, this->pingAuthentication);
      // <-this attempts to connect to the server over the internet and
      // restarts
      // if it can't.
      return 0;
    }
    // This is the child process.
  }
  global.logs.logType = GlobalVariables::LogData::type::server;
  this->initializeSignals();
  global.initializeMathematics();
  global.calculator().getElement().initializeLogDuration(
    Calculator::Mode::full
  );
  // Cannot call initializeMutex here: not before we execute fork();
  global.calculator().getElement().computeAutoCompleteKeyWords();
  global.calculator().getElement().writeAutoCompleteKeyWordsToFile();
  this->writeVersionJSFile();
  global.initModifiableDatabaseFields();
  HtmlRoutines::loadStrings();
  // Must run before port binding: create openSSL configurations.
  this->transportLayerSecurity.initializeAdditionalCertificatesWithErrorChecks(
    false
  );
  // Bind to all ports and create a map from ports to different openSSL
  // configurations.
  if (!this->initializePrepareWebServerAll()) {
    return 1;
  }
  // Must initialize the openSSL configurations
  // after ports were computed: we use the presence of the
  // https port to determine whether built in ssl should try to run.
  this->initializeSSL();
  global << Logger::purple << "waiting for connections..." << Logger::endL;
  this->statistics.successfulSelectsSoFar = 0;
  this->statistics.failedSelectsSoFar = 0;
  long long previousReportedNumberOfSelects = 0;
  Listener listener(this->highestSocketNumber, &this->allListeningSockets);
  int returnCode = 0;
  // Allocate all workers to make sure all pipes are allocated.
  // The number of open pipes can be limited, so it's best to allocate those
  // up front.
  for (int i = 0; i < this->maximumTotalUsedWorkers; i ++) {
    this->createNewActiveWorker();
    this->activeWorker = - 1;
  }
  for (int i = 0; i < this->maximumTotalUsedWorkers; i ++) {
    this->allWorkers[i].flagInUse = false;
  }
  while (
    this->runOnce(listener, previousReportedNumberOfSelects, returnCode)
  ) {}
  return 0;
}

bool WebServer::runOnce(
  Listener& listener,
  long long& previousReportedNumberOfSelects,
  int& returnCode
) {
  // Main accept() loop.
  listener.zeroSocketSet();
  SignalsInfrastructure::signals().unblockSignals();
  listener.selectWrapper();
  SignalsInfrastructure::signals().blockSignals();
  if (global.flagServerDetailedLog) {
    global << Logger::green << "Detail: select success. " << Logger::endL;
  }
  int64_t millisecondsAfterSelect = global.getElapsedMilliseconds();
  this->statistics.successfulSelectsSoFar ++;
  if ((
      this->statistics.successfulSelectsSoFar +
      this->statistics.failedSelectsSoFar
    ) -
    previousReportedNumberOfSelects >
    100
  ) {
    global
    << Logger::blue
    << this->statistics.successfulSelectsSoFar
    << " successful and "
    << this->statistics.failedSelectsSoFar
    << " bad ("
    << this->statistics.successfulSelectsSoFar +
    this->statistics.failedSelectsSoFar
    << " total) selects. "
    << Logger::endL;
    previousReportedNumberOfSelects =
    this->statistics.successfulSelectsSoFar +
    this->statistics.failedSelectsSoFar;
  }
  int reportCount =
  this->statistics.processKilled +
  this->statistics.processesReaped +
  this->statistics.workersNormallyExited +
  this->statistics.allConnections;
  if (reportCount - previousServerStatReport > 99) {
    this->previousServerStatReport = reportCount;
    global
    << "# kill commands: "
    << this->statistics.processKilled
    << " #processes reaped: "
    << this->statistics.processesReaped
    << " #normally reclaimed workers: "
    << this->statistics.workersNormallyExited
    << " #connections so far: "
    << this->statistics.allConnections
    << Logger::endL;
  }
  if (reportCount - this->previousServerStatDetailedReport > 499) {
    this->previousServerStatDetailedReport = reportCount;
    global << this->toStringStatusForLogFile() << Logger::endL;
  }
  int newConnectedSocket = listener.acceptWrapper();
  if (newConnectedSocket < 0) {
    global
    << Logger::red
    << "This is not supposed to to happen: select succeeded "
    << "but I found no set socket. "
    << Logger::endL;
    if (global.flagServerDetailedLog) {
      global
      << "Detail: newConnectedSocket is negative: "
      << newConnectedSocket
      << ". Not accepting. "
      << Logger::endL;
    }
    return true;
  }
  listener.computeUserAddress();
  this->handleTooManyConnections(listener.userAddress);
  this->recycleChildrenIfPossible();
  if (!this->createNewActiveWorker()) {
    global
    << Logger::purple
    << " failed to create an active worker. "
    << "System error string: "
    << strerror(errno)
    << "\n"
    << Logger::red
    << "Failed to create active worker: closing connection. "
    << Logger::endL;
    close(newConnectedSocket);
    return true;
  }
  // ///////////
  this->getActiveWorker().connectedSocketID = newConnectedSocket;
  this->getActiveWorker().connectedSocketIDLastValueBeforeRelease =
  newConnectedSocket;
  this->getActiveWorker().millisecondsServerAtWorkerStart =
  global.getElapsedMilliseconds();
  this->getActiveWorker().millisecondsLastPingServerSideOnly =
  this->getActiveWorker().millisecondsServerAtWorkerStart;
  this->getActiveWorker().millisecondsAfterSelect = millisecondsAfterSelect;
  // <- measured right after select()
  // <- cannot set earlier as the active worker may change after recycling.
  this->statistics.allConnections ++;
  this->getActiveWorker().connectionID = this->statistics.allConnections;
  this->getActiveWorker().userAddress.content = listener.userAddress;
  this->currentlyConnectedAddresses.addMonomial(
    this->getActiveWorker().userAddress, 1
  );
  // ///////////
  if (global.flagServerDetailedLog) {
    global << "Detail: about to fork, sigprocmasking " << Logger::endL;
  }
  if (global.flagServerDetailedLog) {
    global
    << "Detail: Sigprocmask done. Proceeding to fork. "
    << "Time elapsed: "
    << global.getElapsedSeconds()
    << " second(s). <br>"
    << Logger::endL;
  }
  Database::get().localDatabase.currentWorkerId = this->activeWorker;
  int incomingPID = this->forkWorkerProcess();
  // creates an almost identical copy of this process.
  // <- Please don't assign directly to this->getActiveWorker().ProcessPID.
  // <- There may be a race condition around this line of code and I
  // want as little code as possible between the fork and the logFile.
  // The original process is the parent, the almost identical copy is the
  // child.
  // global << "\r\nChildPID: " << this->childPID;
  if (global.flagServerDetailedLog && incomingPID == 0) {
    global
    << "Detail: fork() successful in worker; elapsed ms @ fork(): "
    << global.getElapsedMilliseconds()
    << Logger::endL;
  }
  if (global.flagServerDetailedLog && incomingPID > 0) {
    global
    << "Detail: fork() successful; elapsed ms @ fork(): "
    << global.getElapsedMilliseconds()
    << Logger::endL;
  }
  if (incomingPID < 0) {
    global
    << Logger::red
    << " FAILED to spawn a child process. "
    << Logger::endL;
  }
  this->getActiveWorker().processPID = incomingPID;
  if (this->getActiveWorker().processPID == 0) {
    // This is the child (worker) process.
    global.flagIsChildProcess = true;
    if (global.flagServerDetailedLog) {
      global
      << Logger::green
      << "Detail: "
      << "FORK successful in worker, next step. "
      << "Time elapsed: "
      << global.getElapsedSeconds()
      << " second(s). Calling sigprocmask. "
      << Logger::endL;
    }
    SignalsInfrastructure::signals().unblockSignals();
    if (global.flagServerDetailedLog) {
      global
      << Logger::green
      << "Detail: sigprocmask success, running... "
      << Logger::endL;
    }
    returnCode = this->getActiveWorker().run();
    if (global.flagServerDetailedLog) {
      global << "Detail: run finished, releasing resources. " << Logger::endL;
    }
    this->releaseEverything();
    if (global.flagServerDetailedLog) {
      global
      << Logger::green
      << "Detail: resources released, returning. "
      << Logger::endL;
    }
    return false;
  }
  if (global.flagServerDetailedLog) {
    global
    << Logger::green
    << "Detail: fork successful. Time elapsed: "
    << global.getElapsedMilliseconds()
    << " ms. "
    << "About to unmask signals. "
    << Logger::endL;
  }
  this->releaseWorkerSideResources();
  return true;
}

void WebServer::computeSSLFlags() {
  this->transportLayerSecurity.currentListeningSocket =
  this->lastListeningSocket;
  global.flagUsingSSLinCurrentConnection = false;
  if (this->lastListeningSocket == this->listeningSocketHTTPSBuiltIn) {
    global.flagUsingSSLinCurrentConnection = true;
    this->transportLayerSecurity.flagUseBuiltInTLS = true;
  } else if (this->lastListeningSocket == this->listeningSocketHTTPSOpenSSL) {
    global.flagUsingSSLinCurrentConnection = true;
    this->transportLayerSecurity.flagUseBuiltInTLS = false;
  } else if (
    this->transportLayerSecurity.socketsToAdditionalConfigurations.contains(
      this->lastListeningSocket
    )
  ) {
    global.flagUsingSSLinCurrentConnection = true;
    this->transportLayerSecurity.flagUseBuiltInTLS = false;
  }
  this->getActiveWorker().flagUsingSSLInWorkerProcess =
  global.flagUsingSSLinCurrentConnection;
}

bool WebWorker::runInitialize() {
  global.millisecondOffset = this->millisecondsAfterSelect;
  this->checkConsistency();
  if (this->connectedSocketID == - 1) {
    global.fatal
    << "Worker::run() started on a connecting with ID equal to - 1. "
    << global.fatal;
  }
  this->parent->computeSSLFlags();
  std::stringstream processNameStream;
  processNameStream << "W" << this->indexInParent + 1 << ": ";
  MutexProcess::currentProcessName = processNameStream.str();
  global.flagServerForkedIntoWorker = true;
  createTimerThread();
  // check web worker indices are initialized properly:
  global.server().getActiveWorker();
  if (global.flagUsingSSLinCurrentConnection) {
    std::stringstream commentsOnFailure;
    if (!global.server().sslServerSideHandShake(&commentsOnFailure)) {
      global.flagUsingSSLinCurrentConnection = false;
      this->parent->wrapUp();
      this->parent->releaseEverything();
      global
      << Logger::red
      << "Ssl fail #: "
      << this->parent->statistics.allConnections
      << Logger::endL;
      std::string comments = commentsOnFailure.str();
      if (comments != "") {
        global << comments << Logger::endL;
      }
      return false;
    }
  }
  if (global.flagSSLAvailable && global.flagUsingSSLinCurrentConnection) {
    global
    << Logger::green
    << "ssl success #: "
    << this->parent->statistics.allConnections
    << ". "
    << Logger::endL;
  }
  this->statistics.allReceives = 0;
  this->statistics.pingReceives = 0;
  this->statistics.standaloneServerReceives = 0;
  return true;
}

bool WebWorker::failReceiveReturnFalse() {
  bool sslWasOK = true;
  if (global.flagSSLAvailable) {
    sslWasOK = (
      this->error == TransportLayerSecurityOpenSSL::errors::errorWantRead
    );
  }
  if (this->statistics.allReceives > 0 && sslWasOK) {
    global
    << Logger::green
    << "Connection timed out after successfully receiving "
    << this->statistics.allReceives
    << " times. "
    << Logger::endL;
    this->lastResult = 0;
    return false;
  }
  global << Logger::red << "Failed to receive all with error: " << this->error;
  this->lastResult = - 1;
  return false;
}

bool WebWorker::runOnce() {
  this->lastResult = 0;
  StateMaintainerCurrentFolder preserveCurrentFolder;
  if (!this->receiveAll()) {
    return this->failReceiveReturnFalse();
  }
  if (this->statistics.allReceives > 0) {
    global.calculator().freeMemory();
    global.calculator().getElement().initialize(Calculator::Mode::full);
    global.comments.resetComments();
    global
    << Logger::blue
    << "Created new calculator for connection: "
    << this->statistics.allReceives
    << Logger::endL;
  }
  if (this->messageHead.size() == 0) {
    global << Logger::red << "Empty message head. " << Logger::endL;
    if (this->statistics.allReceives == 0) {
      this->lastResult = - 1;
    }
    return false;
  }
  this->lastResult = this->serveClient();
  if (this->lastResult == - 1) {
    return false;
  }
  this->ensureAllBytesSent();
  this->statistics.allReceives ++;
  if (global.web.flagIsStandaloneWebserver) {
    this->statistics.standaloneServerReceives ++;
  }
  if ((!this->flagKeepAlive) || global.response.isTimedOut()) {
    return false;
  }
  this->resetConnection();
  global
  << Logger::blue
  << "Received "
  << this->statistics.allReceives
  << " times on this connection, waiting for more. "
  << Logger::endL;
  global.millisecondOffset += global.getElapsedMilliseconds();
  this->parent->workerTimerPing(global.millisecondOffset);
  return true;
}

int WebWorker::run() {
  STACK_TRACE("WebWorker::run");
  if (!this->runInitialize()) {
    this->lastResult = - 1;
    return this->lastResult;
  }
  while (this->runOnce()) {}
  this->parent->wrapUp();
  this->parent->releaseEverything();
  return this->lastResult;
}

void WebServer::release(int& descriptor) {
  MutexProcess::release(descriptor);
}

extern int mainFormat();
extern int mainTest(List<std::string>& remainingArgs);

void WebServer::figureOutOperatingSystem() {
  STACK_TRACE("WebServer::figureOutOperatingSystem");
  if (global.operatingSystem != "") {
    return;
  }
  List<std::string> supportedOSes;
  supportedOSes.addOnTop("Ubuntu");
  supportedOSes.addOnTop("CentOS");
  std::string commandOutput =
  global.externalCommandReturnOutput("cat /etc/*-release");
  if (
    commandOutput.find("ubuntu") != std::string::npos ||
    commandOutput.find("Ubuntu") != std::string::npos ||
    commandOutput.find("UBUNTU") != std::string::npos
  ) {
    global.operatingSystem = "Ubuntu";
  } else if (commandOutput.find("CentOS") != std::string::npos) {
    global.operatingSystem = "CentOS";
  } else {
    global.operatingSystem = "";
  }
  if (global.operatingSystem != "") {
    return;
  }
  global
  << Logger::red
  << "Your Linux flavor is not currently supported. "
  << Logger::endL;
  global
  << "We support the following Linux distros: "
  << Logger::blue
  << supportedOSes.toStringCommaDelimited()
  << Logger::endL;
  global
  << "Please post a request for support of your "
  << "Linux flavor on our bug tracker: "
  << Logger::endL
  << Logger::green
  << HtmlRoutines::gitRepository
  << Logger::endL
  << "and we will add your Linux flavor to the list of supported distros. "
  << Logger::endL;
}

void WebServer::checkSystemInstallationOpenSSL() {
  STACK_TRACE("WebServer::checkSystemInstallationOpenSSL");
  if (
    global.configuration["openSSL"].elementType != JSData::Type::tokenUndefined
  ) {
    return;
  }
  global.configuration["openSSL"] = "Attempted installation";
  global.configurationStore();
  WebServer::figureOutOperatingSystem();
  StateMaintainerCurrentFolder preserveFolder;
  std::string sslInstallCommand = "CentOS";
  if (global.operatingSystem == "Ubuntu") {
    sslInstallCommand = "sudo apt-get install libssl-dev";
  } else if (global.operatingSystem == "CentOS") {
    sslInstallCommand = "sudo yum install openssl-devel";
  } else {
    global
    << "You appear to be missing an openssl installation. "
    << "To support https, please install openssl. ";
    return;
  }
  if (sslInstallCommand != "") {
    global
    << "You appear to be missing an openssl installation. "
    << "Let me try to install that for you. "
    << Logger::green
    << "About to request sudo password for: "
    << sslInstallCommand
    << Logger::endL;
    global
    << Logger::red
    << "To refuse the command type CTRL+C. "
    << Logger::endL;
    global.externalCommandNoOutput(sslInstallCommand, true);
    global.configuration["openSSL"] = "Attempted installation on Ubuntu";
  }
  global.configurationStore();
}

void WebServer::checkDatabaseSetup() {
  STACK_TRACE("WebServer::checkDatabaseSetup");
  switch (global.databaseType) {
  case DatabaseType::internal:
    global
    << Logger::blue
    << "Compiled with internal DB support. "
    << Logger::endL;
    break;
  case DatabaseType::fallback:
    global
    << Logger::yellow
    << "Compiled with fallback DB support. "
    << Logger::endL;
    break;
  case DatabaseType::noDatabaseEveryoneIsAdmin:
    global
    << Logger::red
    << "No database, everyone is admin. "
    << Logger::endL;
  }
}

void WebServer::checkFreecalcSetup() {
  STACK_TRACE("WebServer::checkFreecalcSetup");
  if (
    global.configuration["freecalcSetup"].elementType !=
    JSData::Type::tokenUndefined
  ) {
    return;
  }
  global.configuration["freecalcSetup"] = "Setup started";
  global.configurationStore();
  WebServer::figureOutOperatingSystem();
  global
  << Logger::yellow
  << "Freelcalc setup file missing, proceeding to set it up. "
  << Logger::endL;
  StateMaintainerCurrentFolder preserveFolder;
  std::string startingDir = FileOperations::getCurrentFolder();
  global.changeDirectory(global.physicalPathProjectBase + "../");
  global
  << Logger::green
  << "Current folder: "
  << FileOperations::getCurrentFolder()
  << Logger::endL;
  global
  << Logger::green
  << "git clone https://github.com/tmilev/freecalc.git"
  << Logger::endL;
  global.externalCommandNoOutput(
    "git clone https://github.com/tmilev/freecalc.git", true
  );
  global.configuration["freecalcSetup"] = "Setup complete";
  global.configurationStore();
}

void WebServer::initializeMainHashes() {
  StateMaintainerCurrentFolder preserveCurrentFolder;
  if (global.buildVersionSimple == "") {
    global.buildVersionSimple =
    StringRoutines::stringTrimWhiteSpace(
      global.externalCommandReturnOutput("git rev-list --count HEAD")
    );
  }
  global.buildVersionSimple =
  StringRoutines::stringTrimWhiteSpace(global.buildVersionSimple);
  for (unsigned i = 0; i < global.buildVersionSimple.size(); i ++) {
    if (MathRoutines::isLatinLetter(global.buildVersionSimple[i])) {
      global.buildVersionSimple = "?";
      break;
    }
  }
  global.buildHeadHashWithServerTime =
  StringRoutines::stringTrimWhiteSpace(
    global.externalCommandReturnOutput("git rev-parse HEAD")
  );
  for (unsigned i = 0; i < global.buildHeadHashWithServerTime.size(); i ++) {
    if (!MathRoutines::isHexDigit(global.buildHeadHashWithServerTime[i])) {
      global.buildHeadHashWithServerTime = "x";
      break;
    }
  }
  std::stringstream buildHeadHashWithTimeStream;
  buildHeadHashWithTimeStream
  << global.buildHeadHashWithServerTime
  << "_"
  << global.timePointer();
  global.buildHeadHashWithServerTime = buildHeadHashWithTimeStream.str();
  FileOperations::folderVirtualLinksToWhichWeAppendTimeAndBuildHash().
  addOnTopNoRepetitionMustBeNew(WebAPI::Request::calculatorHTML);
  WebAPI::Request::onePageJSWithHash =
  FileOperations::getVirtualNameWithHash(WebAPI::Request::onePageJS);
  WebAPI::Request::calculatorWorkerJSWithHash =
  FileOperations::getVirtualNameWithHash(WebAPI::Request::calculatorWorkerJS);
}

void WebServer::initializeMainRequests() {
  STACK_TRACE("WebServer::initializeMainRequests");
  this->requestsNotNeedingLogin.addOnTop("about");
  this->requestsNotNeedingLogin.addOnTop("signUp");
  this->requestsNotNeedingLogin.addOnTop("signUpPage");
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::solveRequest);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::forgotLogin);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::compute);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::compareExpressions);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::examplesJSON);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::indicator);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::pause);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::exerciseNoLogin);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::exerciseJSON);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::templateNoLogin);
  this->requestsNotNeedingLogin.addOnTop(
    WebAPI::Request::templateJSONNoLogin
  );
  this->requestsNotNeedingLogin.addOnTop(
    WebAPI::Request::topicListJSONNoLogin
  );
  this->requestsNotNeedingLogin.addOnTop(
    WebAPI::Request::problemGiveUpNoLogin
  );
  this->requestsNotNeedingLogin.addOnTop(
    WebAPI::Request::problemSolutionNoLogin
  );
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::homeworkFromSource);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::Slides::source);
  this->requestsNotNeedingLogin.addOnTop(
    WebAPI::Request::Slides::pdfFromSource
  );
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::homeworkSource);
  this->requestsNotNeedingLogin.addOnTop(
    WebAPI::Request::submitExerciseNoLogin
  );
  this->requestsNotNeedingLogin.addOnTop(
    WebAPI::Request::submitExercisePreviewNoLogin
  );
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::selectCourseJSON);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::app);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::appNoCache);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::compareExpressionsPage);
  this->requestsNotNeedingLogin.addOnTop(
    WebAPI::compareExpressionsPageNoCache
  );
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::compareExpressions);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::userInfoJSON);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::serverStatusJSON);
  this->requestsNotNeedingLogin.addOnTop(
    WebAPI::Request::submitAnswerHardcoded
  );
  this->requestsNotNeedingLogin.addOnTop(WebAPI::Request::editPage);
}

void WebServer::initializeMainAddresses() {
  STACK_TRACE("WebServer::initializeMainAddresses");
  this->addressStartsNotNeedingLogin.addOnTop("favicon.ico");
  this->addressStartsNotNeedingLogin.addOnTop("/favicon.ico");
  this->addressStartsNotNeedingLogin.addOnTop("/calculator_html/");
  this->addressStartsNotNeedingLogin.addOnTop("calculator.wasm");
  this->addressStartsNotNeedingLogin.addOnTop("/calculator.wasm");
  this->addressStartsNotNeedingLogin.addOnTop("/src/");
  this->addressStartsNotNeedingLogin.addOnTop("/output/");
  this->addressStartsNotNeedingLogin.addOnTop("/css/");
  this->addressStartsNotNeedingLogin.addOnTop("/javascriptlibs/");
  this->addressStartsNotNeedingLogin.addOnTop("/login");
  this->addressStartsNotNeedingLogin.addOnTop("/examples/");
  this->addressStartsNotNeedingLogin.addOnTop("/" + WebAPI::app);
  this->addressStartsNotNeedingLogin.addOnTop("/" + WebAPI::appNoCache);
  this->addressStartsNotNeedingLogin.addOnTop(WebAPI::Request::onePageJS);
  this->addressStartsNotNeedingLogin.addOnTop(Configuration::HTMLCommon);
  this->addressStartsNotNeedingLogin.addOnTop("/" + Configuration::HTMLCommon);
  this->addressStartsNotNeedingLogin.addOnTop(Configuration::publicHTML);
  this->addressStartsNotNeedingLogin.addOnTop("/" + Configuration::publicHTML);
  this->addressStartsNotNeedingLogin.addOnTop(
    "/" + WebAPI::compareExpressionsPage
  );
  this->addressStartsNotNeedingLogin.addOnTop(WebAPI::compareExpressionsPage);
  this->addressStartsNotNeedingLogin.addOnTop(
    "/" + WebAPI::compareExpressionsPageNoCache
  );
  this->addressStartsNotNeedingLogin.addOnTop(
    WebAPI::compareExpressionsPageNoCache
  );
  this->addressStartsInterpretedAsCalculatorRequest.addOnTop(
    "/" + WebAPI::app
  );
  this->addressStartsInterpretedAsCalculatorRequest.addOnTop(WebAPI::app);
  this->addressStartsInterpretedAsCalculatorRequest.addOnTop(
    "/" + WebAPI::appNoCache
  );
  this->addressStartsInterpretedAsCalculatorRequest.addOnTop(
    WebAPI::appNoCache
  );
  this->addressStartsInterpretedAsCalculatorRequest.addOnTop(
    "/" + WebAPI::compareExpressionsPage
  );
  this->addressStartsInterpretedAsCalculatorRequest.addOnTop(
    WebAPI::compareExpressionsPage
  );
  this->addressStartsInterpretedAsCalculatorRequest.addOnTop(
    "/" + WebAPI::compareExpressionsPageNoCache
  );
  this->addressStartsInterpretedAsCalculatorRequest.addOnTop(
    WebAPI::compareExpressionsPageNoCache
  );
  this->addressStartsInterpretedAsCalculatorRequest.addOnTop(
    WebAPI::Request::onePageJS
  );
  this->addressStartsInterpretedAsCalculatorRequest.addOnTop(
    WebAPI::Request::onePageJSWithHash
  );
  this->addressStartsInterpretedAsCalculatorRequest.addOnTop(
    WebAPI::Request::calculatorWorkerJS
  );
  this->addressStartsInterpretedAsCalculatorRequest.addOnTop(
    WebAPI::Request::calculatorWorkerJSWithHash
  );
  // FORBIDDEN:
  // this->addressStartsNotNeedingLogin.addOnTop("logout");
  // Logging someone out definitely
  // requires authentication: imagine someone
  // asking for logouts on your account once every second:
  // this would be fatal as proper logout resets
  // the authentication tokens.
  this->addressStartsNotNeedingLogin.addListOnTop(
    FileOperations::folderVirtualLinksToWhichWeAppendTimeAndBuildHash()
  );
  this->addressStartsSentWithCacheMaxAge.addOnTop("/html-common/");
  for (
    int i = 0; i <
    FileOperations::folderVirtualLinksToWhichWeAppendTimeAndBuildHash().size;
    i ++
  ) {
    this->addressStartsSentWithCacheMaxAge.addOnTop(
      FileOperations::folderVirtualLinksToWhichWeAppendTimeAndBuildHash()[i] +
      global.buildHeadHashWithServerTime
    );
  }
}

void WebServer::initializeMainFoldersInstructorSpecific() {
  STACK_TRACE("WebServer::initializeMainFoldersInstructorSpecific");
  List<std::string> incoming = List<std::string>({
      Configuration::topicLists,
      Configuration::courseTemplates,
      Configuration::coursesAvailable,
      Configuration::problemsFolder,
    });
  for (int i = 0; i < incoming.size; i ++) {
    FileOperations::folderStartsToWhichWeAppendInstructorUsernameSlash().
    addOnTop(incoming[i]);
    FileOperations::folderStartsToWhichWeAppendInstructorUsernameSlash().
    addOnTop("/" + incoming[i]);
  }
}

void WebServer::initializeMainAll() {
  GlobalStatistics::limitTotalPointersInList = 4000000000;
  this->initializeMainHashes();
  FileOperations::initializeFoldersULTRASensitive();
  FileOperations::initializeFoldersSensitive();
  FileOperations::initializeFoldersNonSensitive();
  FileOperations::checkFolderLinks();
  this->initializeMainFoldersInstructorSpecific();
  this->initializeMainRequests();
  this->initializeMainAddresses();
  this->initializeMainMIMETypes();
}

extern int mainTest(List<std::string>& remainingArgs);

void WebServer::turnProcessMonitoringOn() {
  STACK_TRACE("WebServer::turnProcessMonitoringOn");
  global.response.flagBanProcessMonitoring = false;
  global.configuration[Configuration::processMonitoringBanned] = false;
  global
  << Logger::yellow
  << "Process monitoring IS ON, reply in: "
  << Logger::green
  << global.millisecondsReplyAfterComputation
  << " ms."
  << Logger::endL;
}

void WebServer::turnProcessMonitoringOff() {
  STACK_TRACE("WebServer::turnProcessMonitoringOff");
  global
  << Logger::green
  << "************************"
  << Logger::endL
  << Logger::red
  << "Process monitoring is now off. "
  << Logger::endL
  << Logger::green
  << "************************"
  << Logger::endL;
  global.response.flagBanProcessMonitoring = true;
  global.millisecondsReplyAfterComputation = 0;
  global.configuration[Configuration::processMonitoringBanned] = true;
}

void WebServer::checkInstallation() {
  global.server().checkSystemInstallationOpenSSL();
  global.server().checkDatabaseSetup();
  global.server().checkFreecalcSetup();
}

void WebWorker::prepareFullMessageHeaderAndFooter() {
  STACK_TRACE("WebWorker::prepareFullMessageHeaderAndFooter");
  this->remainingBytesToSend.setSize(0);
  this->remainingBytesToSend.setExpectedSize(
    this->remainingBodyToSend.size + this->remainingHeaderToSend.size + 30
  );
  if (this->remainingHeaderToSend.size == 0) {
    if (this->requestType != this->requestHead) {
      this->remainingBytesToSend = this->remainingBodyToSend;
    }
    this->remainingBodyToSend.setSize(0);
    return;
  }
  this->remainingBytesToSend = this->remainingHeaderToSend;
  this->remainingHeaderToSend.setSize(0);
  std::stringstream contentLengthStream;
  if (this->flagDoAddContentLength) {
    contentLengthStream
    << "Content-length: "
    << this->remainingBodyToSend.size
    << "\r\n";
  }
  contentLengthStream << "\r\n";
  std::string contentLengthString = contentLengthStream.str();
  for (unsigned i = 0; i < contentLengthString.size(); i ++) {
    this->remainingBytesToSend.addOnTop(contentLengthString[i]);
  }
  if (this->requestType != this->requestHead) {
    this->remainingBytesToSend.addListOnTop(this->remainingBodyToSend);
  }
  this->remainingBodyToSend.setSize(0);
}

bool WebWorker::ensureAllBytesSent() {
  if (
    this->remainingBytesToSend.size == 0 &&
    this->remainingBodyToSend.size == 0 &&
    this->remainingHeaderToSend.size == 0
  ) {
    return true;
  }
  global
  << Logger::red
  << "This should not happen: not all bytes have been processed. "
  << Logger::endL;
  global
  << Logger::red
  << "Type: "
  << Logger::blue
  << global.requestType
  << ", address: "
  << this->addressComputed
  << Logger::endL;
  global
  << Logger::red
  << "Remaining header: "
  << Logger::red
  << this->remainingHeaderToSend.toStringCommaDelimited()
  << Logger::endL;
  global
  << Logger::red
  << "Remaining body size: "
  << Logger::blue
  << this->remainingBodyToSend.size
  << Logger::endL;
  global
  << Logger::red
  << "Remaining bytes: "
  << Logger::blue
  << this->remainingBytesToSend.size
  << Logger::endL;
  this->sendPending();
  return false;
}

void WebWorker::sendPending() {
  STACK_TRACE("WebWorker::sendPending");
  this->prepareFullMessageHeaderAndFooter();
  this->sendAllBytesNoHeaders();
}

void WebWorker::queueBytesForSendingNoHeader(
  const List<char>& bytesToSend, bool mustSendAll
) {
  STACK_TRACE("WebWorker::queueBytesForSendingNoHeader");
  (void) mustSendAll;
  this->remainingBytesToSend.addListOnTop(bytesToSend);
}

bool WebWorker::writeToBody(const std::string& stringToSend) {
  STACK_TRACE("WebWorker::writeToBody");
  int oldSize = this->remainingBodyToSend.size;
  this->remainingBodyToSend.setSize(
    this->remainingBodyToSend.size + static_cast<signed>(stringToSend.size())
  );
  for (unsigned i = 0; i < stringToSend.size(); i ++) {
    this->remainingBodyToSend[static_cast<signed>(i) + oldSize] =
    stringToSend[i];
  }
  return true;
}

void WebWorker::queueStringForSendingNoHeader(
  const std::string& stringToSend, bool mustSendAll
) {
  STACK_TRACE("WebWorker::queueStringForSendingNoHeader");
  (void) mustSendAll;
  int oldSize = this->remainingBytesToSend.size;
  this->remainingBytesToSend.setSize(
    this->remainingBytesToSend.size + static_cast<signed>(
      stringToSend.size()
    )
  );
  for (unsigned i = 0; i < stringToSend.size(); i ++) {
    this->remainingBytesToSend[static_cast<signed>(i) + oldSize] =
    stringToSend[i];
  }
}

void WebServer::initializeRandomBytes() {
  Crypto::Random::initializeRandomBytes();
}

bool WebServer::createProcessMutex() {
  WebWorker& worker = this->getActiveWorker();
  if (
    !worker.pauseWorker.createMe(
      this->toStringWorkerToWorker() + "pause mutex", true
    )
  ) {
    global
    << "Failed to create process mutex: "
    << worker.pauseWorker.name
    << "\n";
    return false;
  }
  if (
    !worker.writingReportFile.createMe(
      this->toStringWorkerToWorker() + "output file mutex", true
    )
  ) {
    global
    << "Failed to create process mutex: "
    << worker.writingReportFile.name
    << "\n";
    return false;
  }
  return true;
}

void WebServer::computeActiveWorkerId() {
  List<unsigned char> incomingId;
  Crypto::Random::getRandomBytesSecureInternalMayLeaveTracesInMemory(
    incomingId, 32
  );
  WebWorker& worker = this->getActiveWorker();
  if (worker.workerId != "") {
    this->workerIds.removeKey(worker.workerId);
  }
  worker.workerId = Crypto::convertListUnsignedCharsToHex(incomingId);
  this->workerIds.setKeyValue(worker.workerId, this->activeWorker);
  if (this->workerIds.size() > 2 * this->allWorkers.size) {
    global
    << Logger::red
    << "Warning: worker ids exceeds twice the number of workers. "
    << "This may be a memory leak. "
    << Logger::endL;
  }
}

std::string ActAsWebServerOnly::adjustURL(const std::string& url) const {
  std::string corrected = url;
  if (corrected == "/" || corrected == "") {
    corrected = this->landingPage;
  }
  return FileOperations::addPaths(this->baseFolder, corrected);
}

bool ActAsWebServerOnly::fromJSON(JSData& input) {
  this->portHTTP = input[Configuration::ActAsWebServer::portHTTP].stringValue;
  this->portHTTPS =
  input[Configuration::ActAsWebServer::portHTTPS].stringValue;
  this->baseFolder =
  input[Configuration::ActAsWebServer::baseFolder].stringValue;
  this->landingPage =
  input[Configuration::ActAsWebServer::landingPage].stringValue;
  this->certificateFile =
  input[Configuration::ActAsWebServer::certificateFile].stringValue;
  this->privateKeyFile =
  input[Configuration::ActAsWebServer::privateKeyFile].stringValue;
  return true;
}

std::string ActAsWebServerOnly::toString() const {
  return this->toJSON().toString();
}

JSData ActAsWebServerOnly::toJSON() const {
  JSData result;
  result[Configuration::ActAsWebServer::portHTTP] = this->portHTTP;
  result[Configuration::ActAsWebServer::portHTTPS] = this->portHTTPS;
  result[Configuration::ActAsWebServer::baseFolder] = this->baseFolder;
  result[Configuration::ActAsWebServer::landingPage] = this->landingPage;
  result[Configuration::ActAsWebServer::privateKeyFile] = this->privateKeyFile;
  result[Configuration::ActAsWebServer::certificateFile] =
  this->certificateFile;
  return result;
}
