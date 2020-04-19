// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "webserver.h"
#include "calculator_interface.h"
#include "database.h"
#include "calculator_problem_storage.h"
#include "web_api.h"
#include "string_constants.h"
#include "crypto.h"
#include <sys/wait.h>//<-waitpid f-n here
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <arpa/inet.h> // <- inet_ntop declared here (ntop = network to presentation)
#include <unistd.h>
#include <sys/stat.h>//<-for file statistics
#include <fcntl.h>//<-setting flags of file descriptors
#include "math_extra_latex_routines.h"
#include <sys/resource.h> //<- for setrlimit(...) function. Restricts the time the executable can run.

WebServer& GlobalVariables::server() {
  static WebServer theServer;
  return theServer;
}

class SignalsInfrastructure {
public:
  struct sigaction SignalSEGV;
  struct sigaction SignalFPE;
  struct sigaction SignalChild;
  struct sigaction SignalINT;
  sigset_t allSignals;
  sigset_t oldSignals;
  bool flagSignalsAreBlocked;
  bool flagSignalsAreStored;
  bool flagInitialized;
  SignalsInfrastructure();
  void blockSignals();
  void unblockSignals();
  void initializeSignals();
  static SignalsInfrastructure& theSignals();
};

SignalsInfrastructure& SignalsInfrastructure::theSignals() {
  static SignalsInfrastructure result;
  return result;
}

SignalsInfrastructure::SignalsInfrastructure() {
  this->flagSignalsAreBlocked = false;
  this->flagSignalsAreStored = false;
  this->flagInitialized = false;
}

//This class locks/unlocks all signals within its scope
class SignalLock {
  SignalLock() {
    SignalsInfrastructure::theSignals().blockSignals();
  }
  ~SignalLock() {
    SignalsInfrastructure::theSignals().unblockSignals();
  }
};

void SignalsInfrastructure::unblockSignals() {
  int error = sigprocmask(SIG_SETMASK, &oldSignals, nullptr);
  if (error < 0) {
    global << "Sigprocmask failed on server, I shall now crash. " << logger::endL;
    global.fatal << "Sigprocmask failed on server." << global.fatal;
  }
  this->flagSignalsAreBlocked = false;
  if (global.flagServerDetailedLog) {
    global << logger::green << "Detail: unmask successful. " << logger::endL;
  }
}

void SignalsInfrastructure::blockSignals() {
  sigset_t* theSignals = nullptr;
  if (!this->flagSignalsAreStored) {
    //store signals on first run.
    theSignals = &this->oldSignals;
  }
  int error = sigprocmask(SIG_BLOCK, &this->allSignals, theSignals);
  this->flagSignalsAreStored = true;
  if (error < 0) {
    global
    << logger::red << "Fatal error: sigprocmask failed. "
    << "The server is going to crash. " << logger::endL;
    global.fatal << "Sigprocmas failed. "
    << "This should not happen. " << global.fatal;
  }
  this->flagSignalsAreBlocked = true;
}

bool WebWorker::CheckConsistency() {
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

std::string WebWorker::openIndentTag(const std::string& theTag) {
  std::stringstream out;
  for (int i = 0; i < this->indentationLevelHTML; i ++) {
    out << "  ";
  }
  out << theTag << "\n";
  this->indentationLevelHTML ++;
  return out.str();
}

std::string WebWorker::closeIndentTag(const std::string& theTag) {
  std::stringstream out;
  this->indentationLevelHTML --;
  for (int i = 0; i < this->indentationLevelHTML; i ++) {
    out << "  ";
  }
  out << theTag << "\n";
  return out.str();
}

bool WebWorker::IsAllowedAsRequestCookie(const std::string& input) {
  return
  input != "login" &&
  input != "logout" &&
  input != WebAPI::request::changePassword &&
  input != "changePasswordPage" &&
  input != WebAPI::request::activateAccountJSON;
}

void WebServer::initSSL() {
  MacroRegisterFunctionWithName("WebServer::initSSL");
  if (!global.flagSSLIsAvailable) {
    global << logger::red << "SSL is DISABLED." << logger::endL;
    return;
  }
  //////////////////////////////////////////////////////////////////////////
  this->theTLS.initializeNonThreadSafeOnFirstCall(true);
}

bool WebServer::SSLServerSideHandShake(std::stringstream* commentsOnFailure) {
  if (!global.flagSSLIsAvailable) {
    return false;
  }
  if (!global.flagUsingSSLinCurrentConnection) {
    return false;
  }
  return this->theTLS.HandShakeIamServer(this->GetActiveWorker().connectedSocketID, commentsOnFailure);
}

bool WebWorker::ReceiveAll() {
  MacroRegisterFunctionWithName("WebWorker::ReceiveAll");
  this->messageBody = "";
  this->messageHead = "";
  this->requestTypE = this->requestUnknown;
  if (this->connectedSocketID == - 1) {
    global.fatal << "Attempting to receive on a socket with ID equal to - 1. " << global.fatal;
  }
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 3;  // 3 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  setsockopt(this->connectedSocketID, SOL_SOCKET, SO_RCVTIMEO, static_cast<void*>(&tv), sizeof(timeval));
  std::string errorString;
  int numFailedReceives = 0;
  int maxNumFailedReceives = 2;
  double numSecondsAtStart = global.GetElapsedSeconds();
  int numBytesInBuffer = 0;
  List<char>& readBuffer = this->parent->theTLS.readBuffer;
  while (true) {
    numBytesInBuffer = this->parent->theTLS.readOnce(this->connectedSocketID, &errorString, nullptr, true);

    //int64_t readTime = global.GetElapsedMilliseconds() - msBeforesslread;
    if (numBytesInBuffer >= 0) {
      //if (numBytesInBuffer > 0 && numBytesInBuffer <= (signed) reader.size) {
      break;
    }
    numFailedReceives ++;
    if (numFailedReceives > maxNumFailedReceives) {
      if (errorString == TransportLayerSecurityOpenSSL::errors::errorWantRead) {
        this->error = errorString;
      } else {
        std::stringstream out;
        out
        << "WebWorker::ReceiveAll on socket "
        << this->connectedSocketID
        << " failed (so far "
        << numFailedReceives << " fails). "
        << "Error: " << errorString << ". ";
        out << "Too many failed receives, aborting. ";
        this->error = out.str();
      }
      numBytesInBuffer = 0;
      return false;
    }
  }
  this->messageHead.assign(readBuffer.theObjects, static_cast<unsigned>(numBytesInBuffer));
  this->ParseMessageHead();

  if (this->requestTypE == WebWorker::requestTypes::requestPost) {
    this->displayUserInput = "POST " + this->addressGetOrPost;
  } else {
    this->displayUserInput = "GET " + this->addressGetOrPost;
  }
  if (this->ContentLength <= 0) {
    //logIO << " exiting successfully" << logger::endL;
    return true;
  }
  if (this->messageBody.size() == static_cast<unsigned>(this->ContentLength)) {
    return true;
  }
  this->messageBody.clear(); //<- needed else the length error check will pop.
  if (this->ContentLength > 10000000) {
    this->CheckConsistency();
    error = "Content-length parsed to be more than 10 million bytes, aborting.";
    global << this->error << logger::endL;
    this->displayUserInput = this->error;
    return false;
  }
  this->remainingBytesToSenD = std::string("HTTP/1.0 100 Continue\r\n\r\n");
  this->SendAllBytesNoHeaderS();
  this->remainingBytesToSenD.setSize(0);
  std::string bufferString;
  while (static_cast<signed>(this->messageBody.size()) < this->ContentLength) {
    if (global.GetElapsedSeconds() - numSecondsAtStart > 5) {
      this->error = "Receiving bytes timed out (5 seconds).";
      global << this->error << logger::endL;
      this->displayUserInput = this->error;
      return false;
    }
    numBytesInBuffer = this->parent->theTLS.readOnce(
      this->connectedSocketID,
      &errorString,
      nullptr,
      true
    );
    if (numBytesInBuffer == 0) {
      this->error = "While trying to fetch message-body, received 0 bytes. " +
      this->parent->ToStringLastErrorDescription();
      this->displayUserInput = this->error;
      return false;
    }
    if (numBytesInBuffer < 0) {
      if (
        errno == EAGAIN  ||
        errno == EWOULDBLOCK ||
        errno == EINTR   ||
        errno == EIO ||
        errno == ENOBUFS ||
        errno == ENOMEM
      ) {
        continue;
      }
      this->error = "Error fetching message body: " + this->parent->ToStringLastErrorDescription();
      global << this->error << logger::endL;
      this->displayUserInput = this->error;
      return false;
    }
    bufferString.assign(readBuffer.theObjects, static_cast<unsigned>(numBytesInBuffer));
    this->messageBody += bufferString;
  }
  if (static_cast<signed>(this->messageBody.size()) != this->ContentLength) {
    this->messageHead += this->messageBody;
    this->ParseMessageHead();
  }
  if (static_cast<signed>(this->messageBody.size()) != this->ContentLength) {
    std::stringstream out;
    out << "The message-body received by me had length " << this->messageBody.size()
    << " yet I expected a message of length " << this->ContentLength << ". More details follow. "
    << this->ToStringMessageFull();
    global << out.str() << logger::endL;
  }
  return true;
}

void WebWorker::SendAllBytesNoHeaderS() {
  MacroRegisterFunctionWithName("WebWorker::SendAllBytesNoHeaderS");
  this->CheckConsistency();
  global << "Worker " << this->indexInParent + 1
  << " sending " << this->remainingBytesToSenD.size << " bytes in chunks of: ";
  double startTime = global.GetElapsedSeconds();
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 5;  // 5 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  int numTimesRunWithoutSending = 0;
  int timeOutInSeconds = 20;
  setsockopt(this->connectedSocketID, SOL_SOCKET, SO_SNDTIMEO, static_cast<void*>(&tv), sizeof(timeval));
  std::string errorString;
  std::stringstream commentsOnError;
  while (this->remainingBytesToSenD.size > 0) {
    if (global.GetElapsedSeconds() - startTime > timeOutInSeconds) {
      global << "WebWorker::SendAllBytes failed: more than " << timeOutInSeconds << " seconds have elapsed. "
      << logger::endL;
      return;
    }
    int numBytesSent = this->parent->theTLS.writeOnce(
      this->connectedSocketID,
      this->remainingBytesToSenD,
      &errorString,
      nullptr,
      &commentsOnError,
      true
    );
    if (numBytesSent < 0) {
      global << "WebWorker::SendAllBytes failed: SSL_write error. " << logger::endL;
      return;
    }
    if (numBytesSent == 0) {
      numTimesRunWithoutSending ++;
    } else {
      numTimesRunWithoutSending = 0;
    }
    global << numBytesSent;
    this->remainingBytesToSenD.SliceInPlace(numBytesSent, this->remainingBytesToSenD.size - numBytesSent);
    if (this->remainingBytesToSenD.size > 0) {
      global << ", ";
    }
    if (numTimesRunWithoutSending > 3) {
      global << "WebWorker::SendAllBytes failed: send function went through 3 cycles without "
      << " sending any bytes. "
      << logger::endL;
      return;
    }
  }
  global << "... done." << logger::endL;
}

const int WebServer::maxNumPendingConnections = 1000000;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr* sa) {
  if (sa->sa_family == AF_INET) {
    return &((reinterpret_cast<struct sockaddr_in*>(sa))->sin_addr);
  }
  return &((reinterpret_cast<struct sockaddr_in6*>(sa))->sin6_addr);
}

std::string WebWorker::ToStringMessageFull() const {
  //if (global.flagUsingSSLinCurrentConnection)
  //  return "Message cannot be viewed when using SSL";
  std::stringstream out;
  out << this->ToStringMessageShort();
  if (this->theMessageHeaderStrings.size > 0) {
    out << "<hr>\nStrings extracted from message: ";
    for (int i = 0; i < this->theMessageHeaderStrings.size; i ++) {
      out << "<br>" << HtmlRoutines::ConvertStringToHtmlString(this->theMessageHeaderStrings[i], false);
    }
  }
  return out.str();
}

std::string WebWorker::ToStringMessageShort() const {
  std::stringstream out;
  if (this->requestTypE == this->requestGet) {
    out << "GET ";
  } else if (this->requestTypE == this->requestPost) {
    out << "POST ";
  } else if (this->requestTypE == this->requestChunked) {
    out << "GET or POST **chunked** " ;
  } else {
    out << "Request type undefined.";
  }
  std::string lineBreak = "\n<br>";
  out << "<hr>Address get or post:\n"
  << HtmlRoutines::ConvertStringToHtmlString(this->addressGetOrPost, true);
  out << lineBreak << "\nAddress computed:\n"
  << HtmlRoutines::ConvertStringToHtmlString(this->addressComputed, true);
  out << lineBreak << "\nArgument computed:\n"
  << HtmlRoutines::ConvertStringToHtmlString(this->argumentComputed, true);
  out << lineBreak << "\nVirtual file/directory name:\n"
  << HtmlRoutines::ConvertStringToHtmlString(this->VirtualFileName, true);
  out << lineBreak << "\nRelative physical file/directory name:\n"
  << HtmlRoutines::ConvertStringToHtmlString(this->RelativePhysicalFileNamE, true);
  out << lineBreak << "\nExecutable url:\n"
  << HtmlRoutines::ConvertStringToHtmlString(global.DisplayNameExecutable, false);
  out << lineBreak << "\nPhysical address project base:\n"
  << HtmlRoutines::ConvertStringToHtmlString(global.PhysicalPathProjectBase, false);
  out << lineBreak << "\nPhysical address server base:\n"
  << HtmlRoutines::ConvertStringToHtmlString(global.PhysicalPathServerBase, false);
  out << "<hr>";
  if (this->flagKeepAlive) {
    out << "<br><b>Keeping alive.</b><br>";
  } else {
    out << "<br><b>NOT</b> keeping alive.<br>";
  }
  out << "<br>Cookies (" << this->cookies.size << " total):";
  for (int i = 0; i < this->cookies.size; i ++) {
    out << "<br>" << HtmlRoutines::ConvertStringToHtmlString(this->cookies[i], false);
  }
  out << "\n<hr>\nHost with port:<br>\n" << HtmlRoutines::ConvertStringToHtmlString(global.hostWithPort, false);
  out << "\n<hr>\nFull message head:<br>\n" << HtmlRoutines::ConvertStringToHtmlString(this->messageHead, true);
  out << "\n<hr>\nFull message body:<br>\n" << HtmlRoutines::ConvertStringToHtmlString(this->messageBody, true);
  return out.str();
}

void WebWorker::resetConnection() {
  MacroRegisterFunctionWithName("WebWorker::resetConnection");
  //This function needs a security audit.
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
  global.webArguments.Clear();
  global.requestType = "";
  global.userInputStringIfAvailable = "";
  global.userInputStringRAWIfAvailable = "";
}

void WebWorker::resetMessageComponentsExceptRawMessage() {
  this->addressComputed = "";
  this->addressGetOrPost = "";
  this->argumentComputed = "";
  this->RelativePhysicalFileNamE = "";
  this->VirtualFileName = "";
  this->displayUserInput = "";
  this->theMessageHeaderStrings.setSize(0);
  this->requestHeaders.Clear();
  this->requestTypE = this->requestUnknown;
  this->ContentLength = - 1;
}

JSData WebWorker::GetDatabaseJSON() {
  MacroRegisterFunctionWithName("WebWorker::GetDatabaseJSON");
  JSData result;
  if (!global.UserDefaultHasAdminRights()) {
    result[WebAPI::result::error] = "Only logged-in admins can access database. ";
    return result;
  }
  if (!global.flagDisableDatabaseLogEveryoneAsAdmin) {
    std::string operation = global.GetWebInput(WebAPI::databaseParameters::operation);
    std::string labels = HtmlRoutines::ConvertURLStringToNormal(
      global.GetWebInput(WebAPI::databaseParameters::labels), false
    );
    if (operation == WebAPI::databaseParameters::fetch) {
      result = Database::get().ToJSONDatabaseFetch(labels);
    } else {
      result[WebAPI::result::error] = "Uknown database operation: " + operation + ". ";
    }
    if (global.UserDebugFlagOn()) {
      result["databaseOperation"] = operation;
      result["databaseLabels"] = labels;
    }
  } else {
    result["error"] = "Database not available (cannot get database info). ";
  }
  return result;
}

std::string WebWorker::GetDatabaseDeleteOneItem() {
  MacroRegisterFunctionWithName("WebWorker::GetDatabaseDeleteOneItem");
  if (!global.UserDefaultHasAdminRights()) {
    return "Only logged-in admins can execute the delete command. ";
  }
  std::stringstream commentsStream;
  std::string inputEncoded = global.GetWebInput("item");
  std::string inputString = HtmlRoutines::ConvertURLStringToNormal(inputEncoded, false);
  JSData inputParsed;
  if (!inputParsed.readstring(inputString, &commentsStream)) {
    commentsStream << "Failed to parse input string. ";
    return commentsStream.str();
  }
  commentsStream << "Parsed input string: " << inputParsed.toString(nullptr) << "\n";
  if (Database::get().DeleteOneEntry(inputParsed, &commentsStream)) {
    return "success";
  }
  if (!global.flagDatabaseCompiled) {
    commentsStream << "Database not available (cannot delete item). ";
  }
  return commentsStream.str();
}

bool WebWorker::ExtractArgumentsFromCookies(std::stringstream& argumentProcessingFailureComments) {
  MacroRegisterFunctionWithName("WebWorker::ExtractArgumentsFromCookies");
  MapList<std::string, std::string, MathRoutines::HashString> newlyFoundArgs;
  bool result = true;
  for (int i = 0; i < this->cookies.size; i ++) {
    if (!HtmlRoutines::ChopCGIStringAppend(this->cookies[i], newlyFoundArgs, argumentProcessingFailureComments)) {
      result = false;
    }
  }
  for (int i = 0; i < newlyFoundArgs.size(); i ++) {
    if (global.webArguments.Contains(newlyFoundArgs.theKeys[i])) {
      continue; //<-if a key is already given cookie entries are ignored.
    }
    std::string trimmed = newlyFoundArgs.theValues[i];
    if (trimmed.size() > 0) {
      if (trimmed[trimmed.size() - 1] == ';') {
        trimmed = trimmed.substr(0, trimmed.size() - 1);
      }
    }
    //<-except the last cookie, cookies have extra semicolumn at the end, trimming.
    bool isGood = true;
    if (newlyFoundArgs.theKeys[i] == "request") { //<- we are careful about
      //reading arbitrary requests from the cookie. Those may effectively deny login
      //to a person who does not know to change the request type from the web address.
      //To prevent that we refuse to read requests from cookies except for the
      //whitelist below.
      isGood = false;
      if (
        trimmed == "template" ||
        trimmed == "exercise" ||
        trimmed == "database" ||
        trimmed == "accounts" ||
        trimmed == "calculator" ||
        trimmed == "scoredQuiz"
      ) {
        isGood = true;
      }
    }
    if (isGood) {
      global.webArguments.SetKeyValue(newlyFoundArgs.theKeys[i], trimmed);
    }
  }
  return result;
}

bool WebWorker::ExtractArgumentsFromMessage(
  const std::string& input, std::stringstream& argumentProcessingFailureComments, int recursionDepth
) {
  MacroRegisterFunctionWithName("WebWorker::ExtractArgumentsFromMessage");
  if (recursionDepth > 1) {
    argumentProcessingFailureComments << "Error: input string encoded too many times";
    return false;
  }
  MapList<std::string, std::string, MathRoutines::HashString>& theArgs =
  global.webArguments;
  if (!HtmlRoutines::ChopCGIStringAppend(input, theArgs, argumentProcessingFailureComments)) {
    return false;
  }
  return true;
}

// Returns false if something unexpected happens during the login procedure.
// Returning true does not necessarily mean the login information was accepted.
// Returning false guarantees the login information was not accepted.
bool WebWorker::LoginProcedure(std::stringstream& argumentProcessingFailureComments, std::stringstream* comments) {
  MacroRegisterFunctionWithName("WebWorker::LoginProcedure");
  global.flagLoggedIn = false;
  if (global.flagDisableDatabaseLogEveryoneAsAdmin) {
    global.flagLoggedIn = true;
    global.flagDatabaseCompiled = false;
    return true;
  }

  MapList<std::string, std::string, MathRoutines::HashString>& theArgs = global.webArguments;
  UserCalculatorData& theUser = global.userDefault;
  theUser.username = HtmlRoutines::ConvertURLStringToNormal(
    global.GetWebInput("username"), true
  );
  if (theUser.username.find('%') != std::string::npos) {
    argumentProcessingFailureComments << "<b>Unusual behavior: % sign in username.</b>";
  }
  theUser.enteredAuthenticationToken = HtmlRoutines::ConvertURLStringToNormal(
    global.GetWebInput("authenticationToken"), false
  );
  theUser.enteredGoogleToken = HtmlRoutines::ConvertURLStringToNormal(
    global.GetWebInput("googleToken"), false
  );
  theUser.enteredActivationToken = HtmlRoutines::ConvertURLStringToNormal(
    global.GetWebInput("activationToken"), false
  );
  theUser.enteredPassword = HtmlRoutines::ConvertStringToURLString(
    HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("password"), true), false
  );
  //<-Passwords are ONE-LAYER url-encoded
  //<-INCOMING pluses in passwords MUST be decoded as spaces, this is how form.submit() works!
  //<-Incoming pluses must be re-coded as spaces (%20).

  theUser.flagEnteredPassword = (theUser.enteredPassword != "");
  if (theUser.flagEnteredPassword) {
    theUser.flagMustLogin = true;
    theUser.enteredGoogleToken = "";
    theUser.enteredAuthenticationToken = "";
    theUser.enteredActivationToken = "";
    theUser.flagEnteredAuthenticationToken = false;
    theUser.flagEnteredActivationToken = false;
    global.flagLogInAttempted = true;
    if (global.UserDebugFlagOn() && comments != nullptr) {
      *comments << "Password was entered: all other authentication methods ignored. ";
    }
  }
  if (theUser.enteredActivationToken != "") {
    theUser.enteredGoogleToken = "";
    theUser.enteredAuthenticationToken = "";
    theUser.flagEnteredAuthenticationToken = false;
    theUser.flagEnteredActivationToken = true;
    global.flagLogInAttempted = true;
    if (global.UserDebugFlagOn() && comments != nullptr) {
      *comments << "Activation token entered: authentication token and google token ignored. ";
    }
  }
  if (theUser.username != "") {
    theUser.enteredGoogleToken = "";
  }
  if (theUser.enteredAuthenticationToken != "") {
    theUser.flagEnteredAuthenticationToken = true;
    global.flagLogInAttempted = true;
  }
  /////////////////////////////////////////////
  // this may need a security audit: the URLStringToNormal and GetWebInput
  // functions may leave traces of (freed) memory
  // of the old password. Could only be used
  // if the attacker has control of the server executable - which probably means we
  // already are in big trouble - so this really shouldn't be such a big deal.
  /////////////////////////////////////////////
  if (
    !theUser.flagEnteredPassword &&
    !theUser.flagEnteredAuthenticationToken &&
    theUser.enteredActivationToken == "" &&
    theUser.enteredGoogleToken == ""
  ) {
    return !theUser.flagMustLogin;
  }
  if (!global.flagUsingSSLinCurrentConnection) {
    return false;
  }
  bool doAttemptGoogleTokenLogin = false;
  if (theUser.enteredGoogleToken != "") {
    if (
      theUser.enteredActivationToken == "" &&
      theUser.enteredPassword == "" &&
      theUser.enteredAuthenticationToken == ""
    ) {
      theUser.username = "";
      doAttemptGoogleTokenLogin = true;
    }
    if (theUser.username == "") {
      doAttemptGoogleTokenLogin = true;
    }
  } else if (theUser.username == "") {
    return !theUser.flagMustLogin;
  }
  bool changingPass =
  global.requestType == WebAPI::request::changePassword ||
  global.requestType == WebAPI::request::activateAccountJSON;

  if (changingPass) {
    theUser.enteredAuthenticationToken = "";
  }
  if (doAttemptGoogleTokenLogin) {
    bool tokenIsGood = false;
    global.flagLoggedIn = Database().get().theUser.LoginViaGoogleTokenCreateNewAccountIfNeeded(
      theUser, &argumentProcessingFailureComments, nullptr, tokenIsGood
    );
    if (tokenIsGood && !global.flagLoggedIn && comments != nullptr) {
      *comments << "Your authentication is valid but I have problems with my database records. ";
    }
  } else if (
    theUser.enteredAuthenticationToken != "" ||
    theUser.enteredPassword != "" ||
    theUser.enteredActivationToken != ""
  ) {
    global.flagLoggedIn = Database().get().theUser.LoginViaDatabase(
      theUser, &argumentProcessingFailureComments
    );
  }
  global.CookiesToSetUsingHeaders.SetKeyValue(
    "username",
    HtmlRoutines::ConvertStringToURLString(theUser.username, false)
    //<-User name must be stored in URL-encoded fashion, NO PLUSES.
  );
  if (global.flagLoggedIn && theUser.enteredActivationToken == "") {
    //in case the user logged in with password, we need
    //to give him/her the correct authentication token
    global.CookiesToSetUsingHeaders.SetKeyValue(
      DatabaseStrings::labelAuthenticationToken,
      HtmlRoutines::ConvertStringToURLString(theUser.actualAuthenticationToken, false)
      //<-URL-encoded fashion, NO PLUSES.
    );
    global.SetWebInpuT(
      DatabaseStrings::labelAuthenticationToken,
      HtmlRoutines::ConvertStringToURLString(theUser.actualAuthenticationToken, false)
    );
  } else {
    global.CookiesToSetUsingHeaders.SetKeyValue("authenticationToken", "0");
  }
  bool shouldDisplayMessage = false;
  if (!global.flagLoggedIn && theUser.username != "") {
    if (theUser.flagEnteredPassword || theUser.flagEnteredActivationToken) {
      shouldDisplayMessage = true;
    }
    if (theUser.flagEnteredAuthenticationToken) {
      if (theUser.enteredActivationToken != "0" && theUser.enteredActivationToken != "") {
        shouldDisplayMessage = true;
      }
    }
  }
  theUser.clearAuthenticationTokenAndPassword();
  if (shouldDisplayMessage) {
    argumentProcessingFailureComments << "Invalid user and/or authentication. ";
  }
  theArgs.SetKeyValue("password", "********************************************");
  return true;
}

std::string WebWorker::GetHtmlHiddenInputs(bool includeUserName, bool includeAuthenticationToken) {
  MacroRegisterFunctionWithName("WebWorker::GetHtmlHiddenInputs");
  if (!global.flagUsingSSLinCurrentConnection) {
    return "";
  }
  std::stringstream out;
  if (includeAuthenticationToken) {
    out << "<input type =\"hidden\" id =\"authenticationToken\" name =\"authenticationToken\">\n";
  }
  if (includeUserName) {
    out << "<input type =\"hidden\" id =\"username\" name =\"username\">\n";
  }
  //the values of the hidden inputs will be filled in via javascript
  if (this->flagFoundMalformedFormInput) {
    out << "<b>Your input formed had malformed entries.</b>";
  }
  out
  << "<input type =\"hidden\" id =\"studentView\" name =\"studentView\">\n"
  << "<input type =\"hidden\" id =\"studentSection\" name =\"studentSection\">\n"
  << "<input type =\"hidden\" id =\"courseHome\" name =\"courseHome\">\n"
  << "<input type =\"hidden\" id =\"fileName\" name =\"fileName\">\n";
  return out.str();
}


void WebWorker::WriteAfterTimeoutProgress(const std::string& input, bool forceFileWrite) {
  this->PauseIfRequested();
  MacroRegisterFunctionWithName("WebWorker::WriteAfterTimeoutProgress");
  if (!this->workerToWorkerRequestIndicator.ReadOnceIfFailThenCrash(true)) {
    global << logger::red
    << "Failed to read non-blocking worker-to-worker pipe. " << logger::endL;
    return;
  }
  if (this->workerToWorkerRequestIndicator.lastRead.size == 0 && !forceFileWrite) {
    return;
  }
  this->WriteAfterTimeoutString(
    input,
    WebAPI::result::running,
    global.RelativePhysicalNameOptionalProgressReport
  );
}

void WebWorker::ParseMessageHead() {
  MacroRegisterFunctionWithName("WebWorker::ParseMessageHead");
  this->resetMessageComponentsExceptRawMessage();
  std::string buffer;
  buffer.reserve(this->messageHead.size());
  this->theMessageHeaderStrings.setExpectedSize(50);
  this->cookies.setExpectedSize(30);
  this->theMessageHeaderStrings.size = 0;
  this->cookies.size = 0;
  this->connectionFlags.size = 0;
  this->messageBody = "";
  this->flagKeepAlive = false;
  this->ContentLength = - 1;
  this->addressGetOrPost = "";
  this->hostWithPort = "";
  for (unsigned i = 0; i < this->messageHead.size(); i ++) {
    if (this->messageHead[i] != ' ' && this->messageHead[i] != '\n' && this->messageHead[i] != '\r') {
      buffer.push_back(this->messageHead[i]);
      if (i == this->messageHead.size() - 1) {
        this->theMessageHeaderStrings.addOnTop(buffer);
      }
    } else {
      if (buffer != "") {
        this->theMessageHeaderStrings.addOnTop(buffer);
        buffer = "";
      }
      if (i > 0) {
        if (this->messageHead[i - 1] == '\r' && this->messageHead[i] == '\n') {
          this->theMessageHeaderStrings.addOnTop("\n");
        }
      }
    }
  }
  for (int i = 0; i < this->theMessageHeaderStrings.size; i ++) {
    if (this->theMessageHeaderStrings[i] == "GET") {
      this->requestTypE = this->requestGet;
      i ++;
      if (i < this->theMessageHeaderStrings.size) {
        this->addressGetOrPost = this->theMessageHeaderStrings[i];
      }
    } else if (this->theMessageHeaderStrings[i] == "POST") {
      this->requestTypE = this->requestPost;
      i ++;
      //Short post messages may be attached to the message head
      //if that is the case the message does not end in \n
      if (i < this->theMessageHeaderStrings.size) {
        this->addressGetOrPost = this->theMessageHeaderStrings[i];
        if (*this->theMessageHeaderStrings.LastObject() != "\n") {
          this->messageBody = *this->theMessageHeaderStrings.LastObject();
        } else {
          this->messageBody = "";
        }
      }
    } else if (this->theMessageHeaderStrings[i] == "HEAD") {
      this->requestTypE = this->requestHead;
      i ++;
      if (i < this->theMessageHeaderStrings.size) {
        this->addressGetOrPost = this->theMessageHeaderStrings[i];
      }
    } else if (
      this->theMessageHeaderStrings[i] == "transfer-coding:" ||
      this->theMessageHeaderStrings[i] == "Transfer-coding:" ||
      this->theMessageHeaderStrings[i] == "Transfer-Coding:"
    ) {
      i ++;
      if (i < this->theMessageHeaderStrings.size) {
        if (
          this->theMessageHeaderStrings[i] == "chunked" ||
          this->theMessageHeaderStrings[i] == "chunked;" ||
          this->theMessageHeaderStrings[i] == "Chunked" ||
          this->theMessageHeaderStrings[i] == "Chunked;"
        ) {
          this->requestTypE = this->requestChunked;
        }
      }
    } else if ((
        this->theMessageHeaderStrings[i] == "Content-Length:" ||
        this->theMessageHeaderStrings[i] == "Content-length:" ||
        this->theMessageHeaderStrings[i] == "content-length:"
      ) &&
      i + 1 < this->theMessageHeaderStrings.size
    ) {
      if (this->theMessageHeaderStrings[i + 1].size() < 10000) {
        LargeIntegerUnsigned theLI;
        if (theLI.AssignStringFailureAllowed(this->theMessageHeaderStrings[i + 1], true)) {
          if (!theLI.IsIntegerFittingInInt(&this->ContentLength)) {
            this->ContentLength = - 1;
          }
        }
      }
    } else if (
      this->theMessageHeaderStrings[i] == "Host:" ||
      this->theMessageHeaderStrings[i] == "host:"
    ) {
      i ++;
      if (i < this->theMessageHeaderStrings.size) {
        this->hostWithPort = this->theMessageHeaderStrings[i];
      }
    } else if (
      this->theMessageHeaderStrings[i] == "Cookie:" ||
      this->theMessageHeaderStrings[i] == "cookie:"
    ) {
      i ++;
      for (; i < this->theMessageHeaderStrings.size; i ++) {
        if (this->theMessageHeaderStrings[i] == "\n")
          break;
        this->cookies.addOnTop(this->theMessageHeaderStrings[i]);
      }
    } else if (
      this->theMessageHeaderStrings[i] == "Connection:" ||
      this->theMessageHeaderStrings[i] == "connection:"
    ) {
      i ++;
      for (; i < this->theMessageHeaderStrings.size; i ++) {
        if (this->theMessageHeaderStrings[i] == "\n") {
          break;
        }
        this->connectionFlags.addOnTop(this->theMessageHeaderStrings[i]);
        if (
          this->theMessageHeaderStrings[i] == "keep-alive" ||
          this->theMessageHeaderStrings[i] == "Keep-Alive" ||
          this->theMessageHeaderStrings[i] == "Keep-alive"
        ) {
          this->flagKeepAlive = true;
        }
      }
    }
  }
  if (this->messageBody.size() > 0 && this->ContentLength < 0) {
    this->ContentLength = static_cast<signed>(this->messageBody.size());
  }
  global.hostWithPort = this->hostWithPort;
}

void WebWorker::AttemptUnknownRequestErrorCorrection() {
  MacroRegisterFunctionWithName("WebWorker::AttemptUnknownRequestErrorCorrection");
  if (this->requestTypE != this->requestUnknown) {
    return;
  }
  global << logger::red << "Unknown request. " << logger::endL;
  global << logger::blue << "Message head length: " << this->messageHead.size() << logger::endL;
  std::string messageHeadHexed = Crypto::ConvertStringToHex(this->messageHead, 100, false);
  global << HtmlRoutines::ConvertStringToHtmlStringRestrictSize(messageHeadHexed, false, 300) << logger::endL;
  global << logger::orange << "Message body length: " << this->messageBody.size() << ". " << logger::endL;
  global << HtmlRoutines::ConvertStringToHtmlStringRestrictSize(this->messageBody, false, 300) << logger::endL;
  global << logger::green << "Attempting to correct unknown request.\n";
  if (this->messageBody.size() == 0) {
    if (*this->theMessageHeaderStrings.LastObject() != "\n") {
      global << logger::green
      << "Message body set to last message chunk.\n";
      this->messageBody = *this->theMessageHeaderStrings.LastObject();
    }
  }
  if (this->messageBody.size() != 0) {
    global << "Request set to: POST\n";
    this->requestTypE = this->requestPost;
  } else {
    global << "Request set to: GET\n";
    this->requestTypE = this->requestGet;
  }
  if (this->addressGetOrPost == "") {
    global << "Address set to: " << global.DisplayNameExecutable << "\n";
    this->addressGetOrPost = global.DisplayNameExecutable;
  }
  global << logger::blue
  << "Unrecognized message head, length: " << this->messageHead.size() << ".\n";
  global << logger::red << "Message body length: " << this->messageBody.size() << ". ";
  global << logger::endL;
}

void WebWorker::ExtractHostInfo() {
  MacroRegisterFunctionWithName("WebWorker::ExtractHostInfo");
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
  global.hostWithPort = this->hostWithPort;
  global.hostNoPort = this->hostNoPort;
  if (
    global.hostNoPort == "localhost" ||
    global.hostNoPort == "127.0.0.1"
  ) {
    global.flagRequestComingLocally = true;
  } else {
    global.flagRequestComingLocally = false;
  }
}

void WebWorker::ExtractAddressParts() {
  MacroRegisterFunctionWithName("WebWorker::ExtractAdressParts");
  bool found = false;
  for (unsigned i = 0; i <  this->addressGetOrPost.size(); i ++) {
    if (this->addressGetOrPost[i] == '?') {
      this->addressComputed = this->addressGetOrPost.substr(0, i);
      this->argumentComputed = this->addressGetOrPost.substr(i + 1, std::string::npos);
      found = true;
      break;
    }
  }
  if (!found) {
    this->addressComputed = this->addressGetOrPost;
  }
  if (this->messageBody != "") {
    this->argumentComputed = this->messageBody;
  }
}

std::string WebWorker::GetHeaderConnectionClose() {
  std::stringstream out;
  out << "Connection: close";
  return out.str();
}

std::string WebWorker::GetHeaderConnectionKeepAlive() {
  std::stringstream out;
  out << "Connection: keep-alive";
  return out.str();
}

std::string WebWorker::GetHeaderSetCookie() {
  std::stringstream out;
  for (int i = 0; i < global.CookiesToSetUsingHeaders.size(); i ++) {
    out << "Set-Cookie: " << global.CookiesToSetUsingHeaders.theKeys[i]
    << "="
    << global.CookiesToSetUsingHeaders.theValues[i]
    << "; Path=/; Expires=Sat, 01 Jan 2030 20:00:00 GMT;Secure;SameSite=Strict;";
    if (i != global.CookiesToSetUsingHeaders.size() - 1) {
      out << "\r\n";
    }
  }
  return out.str();
}

void WebWorker::SetHeader(
  const std::string& httpResponseNoTermination,
  const std::string& remainingHeaderNoTermination
) {
  MacroRegisterFunctionWithName("WebWorker::SetHeader");
  std::stringstream out;
  out << httpResponseNoTermination << "\r\n";
  if (!this->flagKeepAlive) {
    out << this->GetHeaderConnectionClose() << "\r\n";
  } else {
    out << this->GetHeaderConnectionKeepAlive() << "\r\n";
  }
  if (remainingHeaderNoTermination != "") {
    out << remainingHeaderNoTermination << "\r\n";
  }
  if (
    (global.flagLoggedIn || global.flagLogInAttempted) &&
    WebWorker::GetHeaderSetCookie() != ""
  ) {
    out << WebWorker::GetHeaderSetCookie() << "\r\n";
  }
  std::string finalHeader = out.str();
  this->remainingHeaderToSend.setSize(0);
  this->remainingHeaderToSend.setExpectedSize(static_cast<signed>(finalHeader.size()));
  for (unsigned i = 0; i < finalHeader.size(); i ++) {
    this->remainingHeaderToSend.addOnTop(finalHeader[i]);
  }
}

void WebWorker::SetHeaderOKNoContentLength(const std::string& extraHeader, const std::string& contentType) {
  MacroRegisterFunctionWithName("WebWorker::SetHeaderOKNoContentLength");
  std::string header = "Content-Type: " + contentType + "; charset=utf-8\r\nAccess-Control-Allow-Origin: *";
  if (extraHeader != "") {
    header += "\r\n" + extraHeader;
  }
  this->SetHeader("HTTP/1.0 200 OK", header);
  this->flagDoAddContentLength = true;
}

void WebWorker::SanitizeVirtualFileName() {
  MacroRegisterFunctionWithName("WebWorker::SanitizeVirtualFileName");
  std::string resultName;
  resultName.reserve(this->VirtualFileName.size());
  bool foundslash = false;
  for (signed i = static_cast<signed>(this->VirtualFileName.size()) - 1; i >= 0; i --) {
    unsigned k = static_cast<unsigned>(i);
    bool isOK = true;
    if (foundslash && this->VirtualFileName[k] == '.') {
      if (i > 0 && this->VirtualFileName[k - 1] == '.') {
        this->flagFileNameSanitized = true;
        isOK = false;
      }
    }
    if (isOK) {
      resultName.push_back(this->VirtualFileName[k]);
    }
    if (this->VirtualFileName[k] == '/') {
      foundslash = true;
    }
  }
  this->VirtualFileName = "";
  for (signed i = static_cast<signed>(resultName.size()) - 1; i >= 0; i --) {
    this->VirtualFileName.push_back(resultName[static_cast<unsigned>(i)]);
  }
}

int WebWorker::GetIndexIfRunningWorkerId(
  JSData& outputComputationStatus
) {
  MacroRegisterFunctionWithName("WebWorker::GetJSONResultFromFile");
  std::string workerId = global.GetWebInput(WebAPI::request::workerId);
  std::stringstream commentsOnError;
  std::string computationResult;
  // 1. Warning: timing attacks on the speed of looking up file names
  // may be used to guess an old worker id.
  // No need to worry as user computations should not contain any authentication
  // or other critical information. User computations are considered
  // ultra-sensitive only because they are private, so there is no reason
  // to overdo the cryptographic protections beyond the common-sense one
  // of requesting a unique id.
  int indexOther = this->parent->workerIds.GetValue(workerId, - 1);
  if (indexOther >= 0) {
    this->parent->theWorkers[indexOther].writingReportFile.Lock();
  }
  bool success = FileOperations::LoadFileToStringVirtual_AccessUltraSensitiveFoldersIfNeeded(
    "results/" + workerId,
    computationResult,
    true,
    true,
    &commentsOnError
  );
  if (indexOther >= 0) {
    this->parent->theWorkers[indexOther].writingReportFile.Unlock();
  }
  if (!success) {
    commentsOnError << "Failed to load your output with id: " << workerId << ". ";
    outputComputationStatus[WebAPI::result::error] = commentsOnError.str();
    return - 1;
  }
  if (!outputComputationStatus.readstring(computationResult)) {
    commentsOnError << "I found your worker id: "
    << workerId << " but I could not parse its JSON status. "
    << "This is likely an internal server error. ";
    outputComputationStatus[WebAPI::result::error] = commentsOnError.str();
    return - 1;
  }
  if (!outputComputationStatus[WebAPI::result::status].isEqualTo(WebAPI::result::running)) {
    return - 1;
  }
  int indexFromFile = - 1;
  outputComputationStatus[WebAPI::result::workerIndex].isIntegerFittingInInt(&indexFromFile);
  if (indexOther < 0) {
    indexOther = indexFromFile;
  }
  if (indexOther != indexFromFile || indexOther == - 1) {
    global.fatal << "Corrupt index worker: indexOther: " << indexOther
    << "; indexFromFile: " << indexFromFile << ". " << global.fatal;
  }
  if (indexOther >= this->parent->theWorkers.size) {
    // Possible causes of this situation:
    // 1) worker id was created in a worker process that was fired up after the current
    // worker.
    // 2) server executable was restarted.
    // Once a worker id is not found, we want to close the connection.
    std::stringstream out;
    out << "WorkerId has too large of an index: " << indexOther << ". " << workerId  << " not found. ";
    outputComputationStatus[WebAPI::result::comments] = out.str();
    this->flagKeepAlive = false;
    indexOther = - 1;
  }
  return indexOther;
}

JSData WebWorker::ProcessComputationIndicatorJSData() {
  MacroRegisterFunctionWithName("WebWorker::ProcessComputationIndicatorJSData");
  // Timer thread will no longer time us out.
  JSData result;
  std::stringstream commentsOnFailure;
  bool allowed = true;
  if (
    !this->flagUsingSSLInWorkerProcess &&
    !global.flagDisableDatabaseLogEveryoneAsAdmin
  ) {
    allowed = false;
  }
  if (!allowed) {
    commentsOnFailure << "Monitoring allowed only "
    << "1) over https or "
    << "2) when flag disableDatabaseLogEveryoneAsAdmin is set to true "
    << "using file configuration/configuration.json.";
    result[WebAPI::result::error] = commentsOnFailure.str();
    return result;
  }
  int otherIndex = this->GetIndexIfRunningWorkerId(result);
  if (otherIndex < 0) {
    this->flagKeepAlive = false;
    return result;
  }
  WebWorker& otherWorker = this->parent->theWorkers[otherIndex];
  // Request a progress report from the running worker, non-blocking.
  otherWorker.workerToWorkerRequestIndicator.WriteOnceAfterEmptying("!", true);
  return result;
}

void WebWorker::WriteAfterTimeoutCarbonCopy(
  const JSData& input, const std::string& fileNameCarbonCopy
) {
  if (fileNameCarbonCopy == "") {
    return;
  }
  std::string extraFilename = "output/";
  extraFilename += FileOperations::CleanUpForFileNameUse(
    fileNameCarbonCopy
  ) + ".json";
  std::stringstream commentsOnError;
  bool success = FileOperations::WriteFileVirual(extraFilename, input.toString(nullptr), &commentsOnError);
  if (!success) {
    global << logger::red << "Error writing optional file. " << commentsOnError.str() << logger::endL;
  }
}

void WebWorker::WriteAfterTimeoutString(
  const std::string& input,
  const std::string& status,
  const std::string& fileNameCarbonCopy
) {
  MacroRegisterFunctionWithName("WebWorker::WriteAfterTimeout");
  JSData result;
  result[WebAPI::result::resultHtml] = input;
  WebWorker::WriteAfterTimeoutPartTwo(result, status, fileNameCarbonCopy);
}

void WebWorker::WriteAfterTimeoutJSON(
  const JSData& input,
  const std::string& status,
  const std::string& fileNameCarbonCopy
) {
  MacroRegisterFunctionWithName("WebWorker::WriteAfterTimeoutJSON");
  JSData result;
  result = input;
  WebWorker::WriteAfterTimeoutPartTwo(result, status, fileNameCarbonCopy);
}

void WebWorker::WriteAfterTimeoutPartTwo(
  JSData& result,
  const std::string& status,
  const std::string& fileNameCarbonCopy
) {
  MacroRegisterFunctionWithName("WebWorker::WriteAfterTimeoutPartTwo");
  std::stringstream commentsOnError;
  result[WebAPI::result::status] = status;
  WebWorker& currentWorker = global.server().GetActiveWorker();
  result[WebAPI::result::workerIndex] = currentWorker.indexInParent;
  std::string toWrite = result.toString(nullptr);
  currentWorker.writingReportFile.Lock();
  bool success = FileOperations::WriteFileVirualWithPermissions_AccessUltraSensitiveFoldersIfNeeded(
    "results/" + currentWorker.workerId,
    toWrite,
    true,
    true,
    &commentsOnError
  );
  currentWorker.WriteAfterTimeoutCarbonCopy(result, fileNameCarbonCopy);
  currentWorker.writingReportFile.Unlock();
  if (success) {
    global << logger::green << "Data written to file: "
    << currentWorker.workerId << logger::endL;
  } else {
    global << "Failed to write computation data. " << commentsOnError.str();
  }  
}

int WebWorker::ProcessFolder() {
  MacroRegisterFunctionWithName("WebWorker::ProcessFolder");
  this->SetHeaderOKNoContentLength("");
  std::stringstream outPage, outError;
  outPage << "<html><body>";
  if (this->RelativePhysicalFileNamE.size() > 0) {
    if (this->RelativePhysicalFileNamE[this->RelativePhysicalFileNamE.size() - 1] != '/') {
      this->RelativePhysicalFileNamE.push_back('/');
    }
  }
  if (this->addressGetOrPost.size() > 0) {
    if (this->addressGetOrPost[this->addressGetOrPost.size() - 1] != '/') {
      this->addressGetOrPost.push_back('/');
    }
  }
  if (this->flagFileNameSanitized) {
    std::stringstream sanitization;
    sanitization << "<hr>The virtual file name I extracted was: "
    << HtmlRoutines::ConvertStringToHtmlString(this->VirtualFileName, false)
    << "<br>However, I do not allow folder names that contain dots. "
    << "Therefore I have sanitized the main address to: "
    << HtmlRoutines::ConvertStringToHtmlString(this->RelativePhysicalFileNamE, false);
    outPage << sanitization.str();
    outError << sanitization.str();
  }
  List<std::string> theFileNames, theFileTypes;
  if (!FileOperations::GetFolderFileNamesUnsecure(this->RelativePhysicalFileNamE, theFileNames, &theFileTypes)) {
    outError << "<b>Failed to open directory with physical address "
    << HtmlRoutines::ConvertStringToHtmlString(this->RelativePhysicalFileNamE, false)
    << " </b>";
    JSData result;
    result[WebAPI::result::error] = outError.str();
    global.theResponse.WriteResponse(result);
    return 0;
  }
  outPage << "Browsing folder: "
  << HtmlRoutines::ConvertStringToHtmlString(this->addressGetOrPost, false) << "<br>Virtual name: "
  << HtmlRoutines::ConvertStringToHtmlString(this->VirtualFileName, false) << "<hr>";
  List<std::string> folderLinksSanitized, fileLinksSanitized;
  for (int i = 0; i < theFileNames.size; i ++) {
    std::stringstream currentStream;
    bool isDir = (theFileTypes[i] == ".d");
    currentStream << "<a href=\"" << HtmlRoutines::ConvertStringToURLString(theFileNames[i], false);
    if (isDir) {
      currentStream << "/";
    }
    currentStream << "\">" << HtmlRoutines::ConvertStringToHtmlString(theFileNames[i], false);
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
  folderLinksSanitized.QuickSortAscending();
  fileLinksSanitized.QuickSortAscending();
  for (int i = 0; i < folderLinksSanitized.size; i ++) {
    outPage << folderLinksSanitized[i];
  }
  for (int i = 0; i < fileLinksSanitized.size; i ++) {
    outPage << fileLinksSanitized[i];
  }
  outPage << "\n</body></html>";
  this->SetHeaderOKNoContentLength("", "text/html");
  this->WriteToBody(outPage.str());
  this->SendPending();
  return 0;
}

int WebWorker::ProcessFileDoesntExist() {
  this->SetHeader("HTTP/1.0 404 Object not found", "Content-Type: text/html");
  // WARNING: cross-site scripting danger. Pay attention when editing.
  // Use ConvertStringToHtmlString to sanitize strings.
  // Never return non-sanitized user input back to the user.
  std::stringstream out;
  out << "<html>"
  << "<body>";
  out << "One page <a href = \"" << global.DisplayNameExecutableApp << "\">app</a>. ";
  out << " Same app without browser cache: <a href = \""
  << global.DisplayNameExecutableAppNoCache << "\">app no cache</a>.<hr>";
  out << "<b>File does not exist.</b>";
  if (this->flagFileNameSanitized) {
    out
    << "<hr>You requested virtual file: "
    << HtmlRoutines::ConvertStringToHtmlString(this->VirtualFileName, false)
    << "<br>However, I do not allow addresses that contain dots "
    << "(to avoid access to folders below the server). "
    << "Therefore I have sanitized the address to a relative physical address: "
    << HtmlRoutines::ConvertStringToHtmlString(this->RelativePhysicalFileNamE, false)
    << ".";
  }
  out << "<br><b>Address:</b> "
  << HtmlRoutines::ConvertStringToHtmlString(this->addressGetOrPost, true)
  << "<br><b>Virtual file name:</b> "
  << HtmlRoutines::ConvertStringToHtmlString(this->VirtualFileName, true)
  << "<br><b>Computed relative physical file name:</b> "
  << HtmlRoutines::ConvertStringToHtmlString(this->RelativePhysicalFileNamE, true);
  out << "<br><b>Request:</b> "
  << HtmlRoutines::ConvertStringToHtmlString(global.requestType, true);
  out << "</body></html>";
  // End of WARNING: cross-site scripting danger.
  this->WriteToBody(out.str());
  this->SendPending();
  return - 1;
}

int WebWorker::ProcessFileCantOpen() {
  this->SetHeaderOKNoContentLength("");
  std::stringstream out;
  out << "Error: file appears to exist but I could not open it. ";
  global << logger::yellow << "Can't open file. "
  << logger::red << strerror(errno) << logger::endL;
  if (this->flagFileNameSanitized) {
    out << "You requested virtual file: "
    << this->VirtualFileName
    << "However, I do not allow addresses that contain dots. "
    << "Therefore I have sanitized the address to a relative physical address: "
    << this->RelativePhysicalFileNamE;
  }
  out << "File display name: "
  << this->addressGetOrPost
  << ". Virtual file name: "
  << this->VirtualFileName << ". ";
  JSData result;
  result[WebAPI::result::error] = out.str();
  return this->WriteToBodyJSOn(result);
}

int WebWorker::ProcessFileTooLarge(long fileSize) {
  this->SetHeader("HTTP/1.0 413 Payload Too Large", "");
  std::stringstream out;
  out << "User requested file: "
  << this->VirtualFileName
  << " but it is too large, namely, " << fileSize
  << " bytes.";
  JSData result;
  result[WebAPI::result::error] = out.str();
  return this->WriteToBodyJSOn(result);
}

int WebWorker::ProcessFile() {
  MacroRegisterFunctionWithName("WebWorker::ProcessFile");
  if (!FileOperations::FileExistsUnsecure(this->RelativePhysicalFileNamE)) {
    return this->ProcessFileDoesntExist();
  }
  std::string fileExtension = FileOperations::GetFileExtensionWithDot(this->RelativePhysicalFileNamE);
  bool isBinary = this->IsFileExtensionOfBinaryFile(fileExtension);
  std::fstream theFile;
  if (!FileOperations::OpenFileUnsecure(theFile, this->RelativePhysicalFileNamE, false, false, !isBinary)) {
    return this->ProcessFileCantOpen();
  }
  theFile.seekp(0, std::ifstream::end);
  long fileSize = theFile.tellp();
  if (fileSize > 100000000) {
    return this->ProcessFileTooLarge(fileSize);
  }
  std::stringstream theHeader;
  theHeader << "HTTP/1.0 200 OK\r\n"
  << this->HeaderFromFileExtension(fileExtension)
  << "Access-Control-Allow-Origin: *\r\n";
  for (int i = 0; i < this->parent->addressStartsSentWithCacheMaxAge.size; i ++) {
    if (StringRoutines::StringBeginsWith(this->VirtualFileName, this->parent->addressStartsSentWithCacheMaxAge[i])) {
      theHeader << WebAPI::HeaderCacheControl << "\r\n";
      break;
    }
  }
  theHeader << "Content-length: " << fileSize << "\r\n";
  if (!this->flagKeepAlive) {
    theHeader << this->GetHeaderConnectionClose() << "\r\n";
  }
  //std::string theCookie = this->GetHeaderSetCookie();
  //if (theCookie != "")
  //  theHeader << theCookie << "\r\n";
  theHeader << "\r\n";
  this->QueueStringForSendingNoHeadeR(theHeader.str());
  if (this->requestTypE == this->requestHead) {
    this->SendAllBytesNoHeaderS();
    return 0;
  }
  const int bufferSize = 64 * 1024;
  this->bufferFileIO.setSize(bufferSize);
  theFile.seekg(0);
  theFile.read(&this->bufferFileIO[0], this->bufferFileIO.size);
  long numBytesRead = theFile.gcount();
  while (numBytesRead != 0) {
    this->bufferFileIO.setSize(static_cast<int>(numBytesRead));
    this->QueueBytesForSendingNoHeadeR(this->bufferFileIO);
    this->bufferFileIO.setSize(bufferSize);
    theFile.read(this->bufferFileIO.theObjects, this->bufferFileIO.size);
    numBytesRead = theFile.gcount();
  }
  this->SendAllBytesNoHeaderS();
  return 0;
}

void WebWorker::reset() {
  this->connectedSocketID = - 1;
  this->ProcessPID = - 1;
  this->connectedSocketIDLastValueBeforeRelease = - 1;
  this->connectionID = - 1;
  this->indexInParent = - 1;
  this->millisecondsAfterSelect = - 1;
  this->parent = nullptr;
  this->indentationLevelHTML = 0;
  this->displayUserInput = "";
  this->requestTypE = this->requestUnknown;
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
  this->RelativePhysicalFileNamE = "";
  this->numberOfReceivesCurrentConnection = 0;
  this->numberOfConsecutiveNoReportsBeforeDisconnect = 0;
  this->Release();
}

WebWorker::WebWorker() {
  this->flagDeallocated = false;
  this->reset();
}

bool WebWorker::IamActive() {
  if (this->parent == nullptr || this->indexInParent == - 1) {
    return false;
  }
  return this->parent->activeWorker == this->indexInParent;
}

bool WebWorker::IsFileExtensionOfBinaryFile(const std::string& fileExtension) {
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

void WebWorker::WrapUpConnection() {
  MacroRegisterFunctionWithName("WebWorker::WrapUpConnection");
  if (global.flagServerDetailedLog) {
    global << "Detail: wrapping up connection. " << logger::endL;
  }
  this->resultWork["connectionsServed"] = this->numberOfReceivesCurrentConnection;
  this->resultWork["result"] = "close";
  if (global.flagRestartNeeded) {
    this->resultWork["restartNeeded"] = "true";
  }
  this->pipeWorkerToServerControls.WriteOnceAfterEmptying(
    this->resultWork.toString(nullptr), false
  );
  if (global.flagServerDetailedLog) {
    global << "Detail: done with pipes, releasing resources. " << logger::endL;
  }
  this->Release();
  if (global.flagServerDetailedLog) {
    global << "Detail: released. " << logger::endL;
  }
  global.flagComputationCompletE = true;
  global.JoinAllThreads();
}

WebWorker::~WebWorker() {
  // Workers are not allowed to release resources in the destructor:
  // a Worker's destructor is called when expanding List<WebWorker>.
  // global << " web worker destructor called. " << logger::endL;
  this->flagDeallocated = true;
}

std::string WebWorker::HeaderFromFileExtension(const std::string& fileExtension) {
  std::stringstream out;
  out << "Content-Type: " << this->MIMETypeFromFileExtension(fileExtension) << "\r\n";
  return out.str();
}

void WebServer::initializeMainMIMETypes() {
  this->MIMETypes.SetKeyValue(".html", "text/html"                    );
  this->MIMETypes.SetKeyValue(".php" , "text/html"                    );
  this->MIMETypes.SetKeyValue(".txt" , "text/plain"                   );
  this->MIMETypes.SetKeyValue(".png" , "image/png"                    );
  this->MIMETypes.SetKeyValue(".js"  , "text/javascript"              );
  this->MIMETypes.SetKeyValue(".ico" , "image/x-icon"                 );
  this->MIMETypes.SetKeyValue(".css" , "text/css"                     );
  this->MIMETypes.SetKeyValue(".eot" , "application/vnd.ms-fontobject");
  this->MIMETypes.SetKeyValue(".ttf" , "application/octet-stream"     );
  this->MIMETypes.SetKeyValue(".svg" , "image/svg+xml"                );
  this->MIMETypes.SetKeyValue(".woff", "application/font-woff"        );
}

std::string WebWorker::MIMETypeFromFileExtension(const std::string& fileExtension) {
  MacroRegisterFunctionWithName("WebWorker::GetMIMEtypeFromFileExtension");
  return this->parent->MIMETypes.GetValue(fileExtension, "application/octet-stream");
}

int WebWorker::ProcessUnknown() {
  MacroRegisterFunctionWithName("WebWorker::ProcessUnknown");
  this->SetHeader("HTTP/1.0 501 Method Not Implemented", "Content-Type: text/html");
  global.theResponse.WriteResponse(WebAPIResponse::GetJSONUserInfo("Unknown request"), false);
  return 0;
}

bool WebWorker::ShouldDisplayLoginPage() {
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
  if (global.UserRequestMustBePromptedToLogInIfNotLoggedIn() && !global.flagLoggedIn) {
    return true;
  }
  return false;
}

std::string WebWorker::GetAuthenticationToken(const std::string& reasonForNoAuthentication) {
  MacroRegisterFunctionWithName("WebWorker::GetAuthenticationToken");
  if (global.flagLoggedIn && global.flagUsingSSLinCurrentConnection) {
    return global.userDefault.actualActivationToken;
  }
  return reasonForNoAuthentication;
}

std::string WebWorker::GetChangePasswordPagePartOne(bool& outputDoShowPasswordChangeField) {
  MacroRegisterFunctionWithName("WebWorker::GetChangePasswordPagePartOne");
  std::stringstream out;
  std::string claimedActivationToken =
  HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("activationToken"), false);
  std::string claimedEmail =
  HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("email"), false);
  out << "<input type =\"hidden\" id =\"activationToken\" value =\""
  << claimedActivationToken << "\">";
  if (claimedActivationToken == "") {
    out << "Activation token is empty. ";
    return out.str();
  }
  if (claimedEmail == "") {
    out << "Activation email is empty. ";
    return out.str();
  }
  std::string actualEmailActivationToken, usernameAssociatedWithToken;
  if (global.userDefault.email == claimedEmail) {
    out << "\n<b style =\"color:green\">Email " << claimedEmail << " updated. </b>";
    return out.str();
  }
  QueryExact findEmail(DatabaseStrings::tableEmailInfo, DatabaseStrings::labelEmail, claimedEmail);
  JSData userInfo;
  QuerySet emailInfo;
  if (!Database::get().FindOne(findEmail, emailInfo.value, &out)) {
    out << "\n<b style =\"color:red\">"
    << "Failed to fetch email activation token for email: "
    << claimedEmail << " </b>";
    return out.str();
  }
  usernameAssociatedWithToken = emailInfo.value[DatabaseStrings::labelUsernameAssociatedWithToken].theString;
  actualEmailActivationToken = emailInfo.value[DatabaseStrings::labelActivationToken].theString;
  if (actualEmailActivationToken != claimedActivationToken) {
    out << "\n<b style =\"color:red\">Bad activation token. Could not activate your email. </b>";
    return out.str();
  }
  if (usernameAssociatedWithToken != global.userDefault.username) {
    out << "\n<b style =\"color:red\">Activation token was issued for another user. </b>";
    return out.str();
  }
  emailInfo.value[DatabaseStrings::labelActivationToken] = "";
  if (!Database::get().UpdateOne(findEmail, emailInfo, &out)) {
    out << "\n<b style =\"color:red\">"
    << "Could not reset the activation token (database is down?). "
    << "</b>";
    return out.str();
  }
  userInfo[DatabaseStrings::labelEmail] = claimedEmail;
  QueryExact findUser(
    DatabaseStrings::tableUsers,
    DatabaseStrings::labelUsername,
    global.userDefault.username
  );
  if (!Database::get().UpdateOne(
    findUser, userInfo, &out
  )) {
    out << "\n<b style =\"color:red\">Could not store your email (database is down?). </b>";
    return out.str();
  }
  global.userDefault.email = claimedEmail;
  out << "\n<b style =\"color:green\">Email successfully updated. </b>";
  if (
    global.userDefault.actualActivationToken != "" &&
    global.userDefault.actualActivationToken != "activated" &&
    global.userDefault.actualHashedSaltedPassword != ""
  ) {
    out << "<br>It appears your password is already set. "
    << "<br>If you'd like to change it using your old password, "
    << "<a href=\"" << global.DisplayNameExecutable
    << "?request=changePasswordPage\">click here</a>. ";
    outputDoShowPasswordChangeField = false;
    userInfo[DatabaseStrings::labelActivationToken] = "activated";
  } else {
    out << "<br>"
    << "<b style =\"color:orange\">To fully activate your account, please choose a password.</b>";
  }
  return out.str();
}

bool WebWorker::DoSetEmail(
  UserCalculatorData& inputOutputUser,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive,
  std::stringstream* commentsGeneralSensitive
) {
  MacroRegisterFunctionWithName("WebWorker::DoSetEmail");
  (void) inputOutputUser;
  (void) commentsOnFailure;
  (void) commentsGeneralNonSensitive;
  (void) commentsGeneralSensitive;
  if (!global.flagDatabaseCompiled) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "DoSetEmail: project compiled without database support. ";
    }
    return false;
  }
  EmailRoutines theEmail;
  theEmail.toEmail = inputOutputUser.email;
  if (!theEmail.IsOKEmail(theEmail.toEmail, commentsOnFailure)) {
    return false;
  }
  UserCalculator userCopy;
  userCopy.UserCalculatorData::operator=(inputOutputUser);
  userCopy.email = inputOutputUser.email;
  if (!userCopy.ComputeAndStoreActivationEmailAndTokens(commentsOnFailure, commentsGeneralNonSensitive)) {
    return false;
  }
  theEmail.emailContent = userCopy.activationEmail;
  theEmail.subject = userCopy.activationEmailSubject;
  if (commentsGeneralNonSensitive != nullptr) {
    *commentsGeneralNonSensitive << "<br><b>Sending email... </b>";
  }
  theEmail.SendEmailWithMailGun(commentsOnFailure, commentsGeneralNonSensitive, commentsGeneralSensitive);
  if (commentsGeneralSensitive != nullptr) {
    if (global.UserDefaultHasAdminRights()) {
      *commentsGeneralSensitive << "<hr>Content of sent email (administrator view only):<br>"
      << HtmlRoutines::ConvertStringToHtmlString(theEmail.emailContent, true);
    }
  } else {
    if (commentsGeneralNonSensitive != nullptr) {
      *commentsGeneralNonSensitive << "Email content not displayed. ";
    }
  }
  userCopy.clearAuthenticationTokenAndPassword();
  userCopy.actualActivationToken = "";
  inputOutputUser = userCopy;
  return true;
}

JSData WebWorker::SetEmail(const std::string& input) {
  MacroRegisterFunctionWithName("WebWorker::SetEmail");
  (void) input;
  JSData result;
  if (!global.flagDatabaseCompiled) {
    result[WebAPI::result::error] = "Database not available (cannot set email). ";
    return result;
  }
  std::stringstream out, debugStream;
  //double startTime = global.GetElapsedSeconds();
  global.userDefault.email = input;
  std::stringstream* adminOutputStream = nullptr;
  if (global.UserDefaultHasAdminRights()) {
    adminOutputStream = &out;
  }
  this->DoSetEmail(global.userDefault, &out, &out, adminOutputStream);
  if (global.UserDefaultHasAdminRights()) {
    out << "<hr><b>Administrator view only. </b>" << debugStream.str();
  }
  out << "<br>Response time: " << global.GetElapsedSeconds() << " second(s).";
  result[WebAPI::result::comments] = out.str();
  return  result;
}

int WebWorker::ProcessGetAuthenticationToken(const std::string& reasonForNoAuthentication) {
  MacroRegisterFunctionWithName("WebWorker::ProcessGetAuthenticationToken");
  this->SetHeaderOKNoContentLength("");
  return this->WriteToBody(this->GetAuthenticationToken(reasonForNoAuthentication));
}

JSData WebWorker::GetClonePageResult() {
  MacroRegisterFunctionWithName("WebWorker::GetClonePageResult");
  this->SetHeaderOKNoContentLength("");
  return WebAPIResponse::ClonePageResult();
}

std::string WebWorker::GetAddUserEmails() {
  MacroRegisterFunctionWithName("WebWorker::GetAddUserEmails");
  return WebAPIResponse::AddUserEmails(this->hostWithPort);
}

std::string WebAPIResponse::ModifyProblemReport() {
  MacroRegisterFunctionWithName("WebWorker::ModifyProblemReport");
  bool shouldProceed = global.flagLoggedIn && global.UserDefaultHasAdminRights();
  if (shouldProceed) {
    shouldProceed = global.flagUsingSSLinCurrentConnection;
  }
  if (!shouldProceed) {
    return "<b>Modifying problems allowed only for "
    "logged-in admins under ssl connection. </b>";
  }
  std::string mainInput = HtmlRoutines::ConvertURLStringToNormal(
    global.GetWebInput(WebAPI::problem::fileContent), false
  );
  std::string fileName = HtmlRoutines::ConvertURLStringToNormal(
    global.GetWebInput(WebAPI::problem::fileName), false
  );
  std::stringstream commentsOnFailure;
  bool fileExists = FileOperations::FileExistsVirtualCustomizedReadOnly(
    fileName, &commentsOnFailure
  );
  std::fstream theFile;
  if (global.flagDisableDatabaseLogEveryoneAsAdmin) {
    global.userDefault.instructorComputed = "default";
  }
  if (!FileOperations::OpenFileVirtualCustomizedWriteOnly(
    theFile, fileName, false, true, false, &commentsOnFailure
  )) {
    commentsOnFailure
    << "<b style =\"color:red\">Failed to open/create file: "
    << fileName << ". </b>";
    return commentsOnFailure.str();
  }
  theFile << mainInput;
  theFile.close();
  std::stringstream out;
  if (!fileExists) {
    out << "File " << fileName << " didn't previously exist: just created it for you. ";
  }
  out << "<b style =\"color:green\">Wrote " << mainInput.size() << " bytes to file: "
  << fileName << ". </b>";
  return out.str();
}

std::string WebAPIResponse::GetJavascriptCaptcha() {
  MacroRegisterFunctionWithName("WebAPIResponse::GetCaptchaJavascript");
  std::stringstream out;
  out << "<script src = 'https://www.google.com/recaptcha/api.js'></script>";
  return out.str();
}

std::string WebAPIResponse::GetCaptchaDiv() {
  MacroRegisterFunctionWithName("WebAPIResponse::GetCaptchaDiv");
  std::stringstream out;
  std::string recaptchaPublic;
  if (!FileOperations::LoadFileToStringVirtual_AccessUltraSensitiveFoldersIfNeeded(
    "certificates/recaptcha-public.txt", recaptchaPublic, true, true, &out
  )) {
    out << "<b style =\"color:red\">Couldn't find the recaptcha key in file: "
    << "certificates/recaptcha-public.txt</b>";
  } else {
    out << "<div class =\"g-recaptcha\" data-sitekey =\"" << recaptchaPublic << "\"></div>";
  }
  return out.str();
}

bool WebWorker::CorrectRequestsBEFORELoginReturnFalseIfModified() {
  MacroRegisterFunctionWithName("WebWorker::CorrectRequestsBEFORELoginReturnFalseIfModified");
  bool stateNotModified = true;
  if (!global.flagSSLIsAvailable) {
    if (
      this->addressComputed == global.DisplayNameExecutable &&
      global.requestType == ""
    ) {
      this->addressComputed = global.DisplayNameExecutable;
      global.requestType = "calculator";
      stateNotModified = false;
    }
  }
  if (this->addressComputed == "/" || this->addressComputed == "") {
    this->addressComputed = global.DisplayNameExecutableApp; //was: global.DisplayNameExecutable;
    global.requestType = WebAPI::app;
    //global.requestType = "selectCourse";
    stateNotModified = false;
  }
  return stateNotModified;
}

bool WebWorker::RedirectIfNeeded(std::stringstream& argumentProcessingFailureComments) {
  MacroRegisterFunctionWithName("WebWorker::RedirectIfNeeded");
  UserCalculatorData& theUser = global.userDefault;
  if (!theUser.flagEnteredPassword) {
    return false;
  }
  if (
    global.requestType == WebAPI::request::changePassword ||
    global.requestType == WebAPI::request::activateAccountJSON
  ) {
    return false;
  }
  std::stringstream redirectedAddress;
  if (this->addressComputed == global.DisplayNameExecutable) {
    redirectedAddress << global.DisplayNameExecutable << "?request="
    << global.requestType << "&";
  } else {
    redirectedAddress << this->addressComputed << "?";
  }
  for (int i = 0; i < global.webArguments.size(); i ++) {
    if (
      global.webArguments.theKeys[i] != "password" &&
      global.webArguments.theKeys[i] != "request" &&
      global.webArguments.theKeys[i] != "googleToken" &&
      global.webArguments.theKeys[i] != "G_AUTHUSER_H" &&
      global.webArguments.theKeys[i] != "activationToken"
    ) {
      redirectedAddress << global.webArguments.theKeys[i] << "="
      << global.webArguments.theValues[i] << "&";
    }
  }
  if (argumentProcessingFailureComments.str() != "") {
    redirectedAddress << "error="
    << HtmlRoutines::ConvertStringToURLString(argumentProcessingFailureComments.str(), false)
    << "&";
  }
  std::stringstream headerStream;
  headerStream << "Location: " << redirectedAddress.str();
  this->SetHeader("HTTP/1.0 303 See other", headerStream.str());
  std::stringstream out;
  out << "<html><head>"
  << "<meta http-equiv=\"refresh\" content =\"0; url ='" << redirectedAddress.str()
  << "'\" />"
  << "</head>";
  out << "<body>Click <a href=\"" << redirectedAddress.str() << "\">"
  << " here " << "</a> if your browser does not redirect the page automatically. ";
  out << "</body></html>";
  this->WriteToBody(out.str());
  return true;
}

bool WebWorker::CorrectRequestsAFTERLoginReturnFalseIfModified() {
  MacroRegisterFunctionWithName("WebWorker::CorrectRequestsAFTERLoginReturnFalseIfModified");
  bool stateNotModified = true;
  bool shouldFallBackToDefaultPage = false;
  if (this->addressComputed == "/" || this->addressComputed == "") {
    shouldFallBackToDefaultPage = true;
  }
  if (!global.flagSSLIsAvailable) {
    if (
      this->addressComputed == global.DisplayNameExecutable &&
      global.requestType == ""
    ) {
      this->addressComputed = global.DisplayNameExecutable;
      global.requestType = WebAPI::app;
      stateNotModified = false;
    }
  }
  if (global.flagLoggedIn) {
    if (
      this->addressComputed == global.DisplayNameExecutable &&
      global.requestType == ""
    ) {
      shouldFallBackToDefaultPage = true;
    } else if (
      global.requestType == "template" &&
      global.GetWebInput(WebAPI::problem::courseHome) == ""
    ) {
      shouldFallBackToDefaultPage = true;
    }
  }
  if (shouldFallBackToDefaultPage) {
    this->addressComputed = global.DisplayNameExecutableApp;
    global.requestType = WebAPI::app;
    stateNotModified = false;
  }
  return stateNotModified;
}

bool WebWorker::ProcessRedirectAwayFromWWW() {
  std::string addressNoWWW;
  if (!StringRoutines::StringBeginsWith(
    global.hostWithPort, "www.", &addressNoWWW
  )) {
    return false;
  }
  std::stringstream newAddressStream, redirectHeaderStream;
  newAddressStream << "https://" << addressNoWWW;
  if (this->addressGetOrPost.size() != 0) {
    if (this->addressGetOrPost[0] != '/') {
      this->addressGetOrPost = '/' + this->addressGetOrPost;
    }
  }
  newAddressStream << this->addressGetOrPost;
  redirectHeaderStream << "Location: " << newAddressStream.str();
  this->SetHeader("HTTP/1.0 301 Moved Permanently", redirectHeaderStream.str());
  std::stringstream out;
  out << "<html><body>Please remove the www. "
  << "from the address, it creates issues with authentication services. "
  << "Click <a href=\"" << newAddressStream.str()
  << "\">here</a> if not redirected automatically. ";
  out << "</body></html>";
  this->WriteToBody(out.str());
  return true;
}

int WebWorker::ProcessLoginNeededOverUnsecureConnection() {
  MacroRegisterFunctionWithName("WebWorker::ProcessLoginNeededOverUnsecureConnection");
  std::stringstream redirectStream, newAddressStream;
  newAddressStream << "https://" << this->hostNoPort;
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
    this->SetHeader("HTTP/1.0 301 Moved Permanently", redirectStream.str());
    outBody
    << "<html><body>Address available through secure (SSL) connection only. "
    << "Click <a href=\"" << newAddressStream.str()
    << "\">here</a> if not redirected automatically. ";
  } else {
    this->SetHeaderOKNoContentLength("");
    outBody << "<html><body>Address available through "
    << "secure (SSL) connection only. <br>"
    << "<b style ='color:red'>In the web address, please "
    << "change http to https. </b><br>"
    << "Unfortunately, I can't redirect you automatically "
    << "as your browser did not tell me "
    << "under what domain name it sees me, and the "
    << "server responds to multiple domain names. ";

  }
  outBody << "</body></html>";
  return this->WriteToBody(outBody.str());
}

bool WebWorker::RequireSSL() {
  return global.flagSSLIsAvailable;
}

int WebWorker::ServeClient() {
  MacroRegisterFunctionWithName("WebWorker::ServeClient");
  global.millisecondsComputationStart = global.GetElapsedMilliseconds();
  global.flagComputationStarted = true;
  global.flagComputationCompletE = false;
  global.userDefault.flagMustLogin = true;
  global.userDefault.flagStopIfNoLogin = true;
  UserCalculatorData& theUser = global.userDefault;
  this->response.reset(*this);
  if (
    this->requestTypE != this->requestGet &&
    this->requestTypE != this->requestPost &&
    this->requestTypE != this->requestHead
  ) {
    if (
      this->requestTypE != this->requestUnknown &&
      this->requestTypE != this->requestChunked
    ) {
      global.fatal << "Something is wrong: request type does "
      << "not have any of the expected values. " << global.fatal;
    }
    this->ProcessUnknown();
    return 0;
  }
  if (global.theThreads.size <= 1) {
    global.fatal << "Number of threads must be at least 2 in this point of code..." << global.fatal;
  }
  this->ExtractHostInfo();
  this->ExtractAddressParts();
  std::stringstream argumentProcessingFailureComments;
  this->flagArgumentsAreOK = true;
  if (!this->ExtractArgumentsFromMessage(this->argumentComputed, argumentProcessingFailureComments)) {
    this->flagArgumentsAreOK = false;
  }
  if (!this->ExtractArgumentsFromCookies(argumentProcessingFailureComments)) {
    this->flagArgumentsAreOK = false;
  }
  global.requestType = "";
  if (this->addressComputed == global.DisplayNameExecutable) {
    global.requestType = global.GetWebInput("request");
  }
  std::stringstream comments;
  comments << "Address, request computed: ";
  if (this->addressComputed != "") {
    comments << HtmlRoutines::ConvertStringToHtmlString(this->addressComputed, false);
  } else {
    comments << "[empty]";
  }
  comments << ", ";
  if (global.requestType != "") {
    comments << HtmlRoutines::ConvertStringToHtmlString(global.requestType, false);
  } else {
    comments << "[empty]";
  }
  comments << ". ";
  bool isNotModified = this->CorrectRequestsBEFORELoginReturnFalseIfModified();
  if (!isNotModified) {
    comments << this->ToStringAddressRequest() << ": modified before login. ";
  }
  if (global.server().addressStartsInterpretedAsCalculatorRequest.Contains(this->addressComputed)) {
    global.requestType = this->addressComputed;
    std::string correctedRequest;
    if (StringRoutines::StringBeginsWith(global.requestType, "/", &correctedRequest)) {
      global.requestType = correctedRequest;
      comments << "Address was interpretted as request, so your request was set to: "
      << global.requestType << ". ";
    }
  }
  theUser.flagMustLogin = this->parent->RequiresLogin(
    global.requestType, this->addressComputed
  );
  if (!theUser.flagMustLogin) {
    comments << "Login not needed. ";
  }
  if (this->RequireSSL() && !global.flagUsingSSLinCurrentConnection) {
    return this->ProcessLoginNeededOverUnsecureConnection();
  }
  if (this->ProcessRedirectAwayFromWWW()) {
    return 0;
  }
  this->flagArgumentsAreOK = this->LoginProcedure(argumentProcessingFailureComments, &comments);
  isNotModified = this->CorrectRequestsAFTERLoginReturnFalseIfModified();
  if (!isNotModified) {
    comments << this->ToStringAddressRequest() << ": modified after login. ";
  }
  if (this->RedirectIfNeeded(argumentProcessingFailureComments)) {
    return 0;
  }
  theUser.flagStopIfNoLogin = (
    !global.flagLoggedIn &&
    global.flagUsingSSLinCurrentConnection &&
    theUser.flagMustLogin
  );
  if (theUser.flagStopIfNoLogin) {
    if (global.requestType == WebAPI::request::changePassword) {
      theUser.flagStopIfNoLogin = false;
    }
  }
  if (theUser.flagStopIfNoLogin) {
    if (
      global.requestType != WebAPI::request::logout &&
      global.requestType != WebAPI::request::login
    ) {
      argumentProcessingFailureComments << this->ToStringAddressRequest()
      << " requires login. ";
    }
    argumentProcessingFailureComments << comments.str();
    global.CookiesToSetUsingHeaders.SetKeyValue("authenticationToken", "");
    if (argumentProcessingFailureComments.str() != "") {
      global.SetWebInpuT("authenticationToken", "");
    }
    return this->response.ProcessLoginUserInfo(argumentProcessingFailureComments.str());
  }
  if (
    argumentProcessingFailureComments.str() != "" && (
      theUser.flagMustLogin ||
      global.requestType == WebAPI::request::userInfoJSON
    )
  ) {
    global.SetWebInpuT("error", argumentProcessingFailureComments.str());
  }
  if (global.UserDefaultHasAdminRights() && global.flagLoggedIn) {
    if (global.GetWebInput("spoofHostName") != "") {
      global.hostNoPort = HtmlRoutines::ConvertURLStringToNormal(
        global.GetWebInput("spoofHostName"), false
      );
      global.CookiesToSetUsingHeaders.SetKeyValue("spoofHostName", global.hostNoPort);
    }
  }
  if (this->response.ServeResponseFalseIfUnrecognized(
    argumentProcessingFailureComments, comments
  )) {
    return 0;
  }
  return this->ProcessFolderOrFile();
}

int WebWorker::ProcessFolderOrFile() {
  MacroRegisterFunctionWithName("WebWorker::ProcessFolderOrFile");
  this->VirtualFileName = HtmlRoutines::ConvertURLStringToNormal(this->addressComputed, true);
  this->SanitizeVirtualFileName();
  std::stringstream commentsOnFailure;
  if (
    !FileOperations::GetPhysicalFileNameFromVirtual(
      this->VirtualFileName,
      this->RelativePhysicalFileNamE,
      global.UserDefaultHasAdminRights(),
      false,
      &commentsOnFailure
  )) {
    this->SetHeader("HTTP/1.0 404 Object not found", "Content-Type: text/html");
    std::stringstream out;
    JSData result;
    out << "File name: "
    << HtmlRoutines::ConvertStringToHtmlStringRestrictSize(this->VirtualFileName, false, 1000)
    << " <b style = 'color:red'>deemed unsafe</b>. "
    << "Please note that folder names are not allowed to contain dots and file names "
    << "are not allowed to start with dots. There may be additional restrictions "
    << "on file names added for security reasons.";
    result[WebAPI::result::error] = out.str();
    return global.theResponse.WriteResponse(result);
  }
  if (FileOperations::IsFolderUnsecure(this->RelativePhysicalFileNamE)) {
    return this->ProcessFolder();
  }
  return this->ProcessFile();
}

void WebWorker::PauseIfRequested() {
  this->PauseWorker.Lock(); // if pause was requested, here we block
  this->PauseWorker.Unlock();
}

void WebWorker::ResetMutexProcesses() {
  MacroRegisterFunctionWithName("WebWorker::ResetMutexProcesses");
  this->PauseWorker.ResetNoAllocation();
  this->writingReportFile.ResetNoAllocation();
}

void WebWorker::ReleaseKeepInUseFlag() {
  MacroRegisterFunctionWithName("WebWorker::ReleaseKeepInUseFlag");
  this->pipeWorkerToServerTimerPing.Release();
  this->pipeWorkerToServerControls.Release();
  this->workerToWorkerRequestIndicator.Release();
  this->pipeWorkerToWorkerStatus.Release();
  this->PauseWorker.Release();
  this->writingReportFile.Release();
  this->millisecondsLastPingServerSideOnly = - 1;
  WebServer::Release(this->connectedSocketID);
}

void WebWorker::Release() {
  this->ReleaseKeepInUseFlag();
  this->flagInUsE = false;
}

void WebWorker::GetIndicatorOnTimeout(
  JSData& output, const std::string& message
) {
  MacroRegisterFunctionWithName("WebWorker::GetIndicatorOnTimeout");
  global << logger::blue
  << "Computation timeout, sending progress indicator instead of output. "
  << logger::endL;
  std::stringstream timeOutComments;
  output[WebAPI::result::timeOut] = true;

  timeOutComments << message;
  if (global.theResponse.flagBanProcessMonitorinG) {
    timeOutComments
    << "Monitoring computations is not allowed on this server.<br> "
    << "Please note that monitoring computations "
    << "is the default behavior, so the "
    << "owners of the server must have explicitly banned monitoring. ";
    output[WebAPI::result::timeOutComments] = timeOutComments.str();
  } else if (!global.theResponse.flagReportDesired){
    timeOutComments
    << "Monitoring computations not desired by user. ";
    output[WebAPI::result::timeOutComments] = timeOutComments.str();
  } else {
    output[WebAPI::result::workerId] = this->workerId;
    output[WebAPI::result::timeOutComments] = timeOutComments.str();
  }
}

void WebWorker::WriteAfterTimeoutShowIndicator(const std::string& message) {
  MacroRegisterFunctionWithName("WebWorker::WriteAfterTimeoutShowIndicator");
  global << "Long computation, about to display indicator. " << logger::endL;
  JSData result;
  this->GetIndicatorOnTimeout(result, message);
  if (this->indexInParent < 0) {
    global.fatal
    << "Index of worker is smaller than 0, this shouldn't happen. "
    << global.fatal;
  }
  this->WriteToBodyJSOn(result);
  this->WriteAfterTimeoutProgress(
    global.ToStringProgressReportNoThreadData(true), true
  );
  this->SendPending();
  for (int i = 0; i < this->parent->theWorkers.size; i ++) {
    if (i != this->indexInParent) {
      this->parent->theWorkers[i].Release();
    }
  }
  this->parent->Release(this->connectedSocketID);
  // set flags properly:
  // we need to rewire the standard output and the crashing mechanism:
}

std::string WebWorker::ToStringAddressRequest() const {
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

std::string WebWorker::ToStringStatus() const {
  std::stringstream out;
  out << "<br>Worker " << this->indexInParent + 1;
  if (this->flagExited) {
    out << ", <b style = 'color:red'>exited</b>";
  }
  if (this->flagInUsE) {
    if (this->parent->activeWorker == this->indexInParent) {
      out << ", <b style ='color:green'>current process</b>";
    } else {
      out << ", <b>in use</b>";
    }
    out << ", <a href=\"calculator?request=monitor&mainInput="
    << this->indexInParent + 1 << "\">monitor process "
    << this->indexInParent + 1 << "</a>";
  } else {
    out << ", not in use";
  }
  if (this->displayUserInput != "") {
    out << ", user input: <span style = 'color:blue'>"
    << this->displayUserInput << "</span>";
  }
  out << ", connection " << this->connectionID << ", process ID: ";
  if (this->ProcessPID != 0) {
    out << this->ProcessPID;
  } else {
    out << "(not accessible)";
  }
  out << ", socketID: ";
  if (this->connectedSocketID == - 1) {
    out << "released in current process, value before release: "
    << this->connectedSocketIDLastValueBeforeRelease;
  } else {
    out << this->connectedSocketID;
  }
  out << ". ";
  out << "Server time at last ping: "
  << this->millisecondsLastPingServerSideOnly << " milliseconds. ";
  if (this->pingMessage != "") {
    out << " Message at last ping: " << this->pingMessage;
  }
  if (this->status != "") {
    out << "<br><b style =\"color:red\">Status: "
    << this->status << "</b><br>";
  }
  out << "Pipes: " << this->pipeWorkerToServerControls.toString()
  << ", " << this->pipeWorkerToServerTimerPing.toString()
  << ", " << this->workerToWorkerRequestIndicator.toString()
  << ", " << this->pipeWorkerToWorkerStatus.toString();
  out << ". MutexProcesses: " << this->PauseWorker.toString()
  << ", " << this->writingReportFile.toString();
  out << ", user address: " << this->userAddress << ".";
  return out.str();
}

bool WebServer::CheckConsistency() {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of WebServer." << global.fatal;
  }
  return true;
}

void WebServer::ReleaseEverything() {
  this->theTLS.Free();
  for (int i = 0; i < this->theWorkers.size; i ++) {
    this->theWorkers[i].Release();
  }
  this->activeWorker = - 1;
  if (global.flagServerDetailedLog) {
    global << logger::red << "Detail: "
    << "About to close socket: " << this->listeningSocketHTTP << ". " << logger::endL;
  }
  for (int i = 0; i < this->theListeningSockets.size; i ++) {
    int socket = this->theListeningSockets[i];
    close(socket);
    if (global.flagServerDetailedLog) {
      global << logger::red << "Detail: "
      << "Closed socket: " << socket << ". " << logger::endL;
    }
  }
  this->listeningSocketHTTP = - 1;
  this->listeningSocketHTTPSBuiltIn = - 1;
  this->listeningSocketHTTPSOpenSSL = - 1;
}

WebServer::~WebServer() {
  this->flagDeallocated = true;
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
  this->previousServerStatReport = 0;
  this->previousServerStatDetailedReport = 0;
  this->processIdServer = - 1;
}

void WebServer::Signal_SIGCHLD_handler(int s) {
  (void) s; //avoid unused parameter warning, portable.
  if (global.flagIsChildProcess) {
    return;
  }
  if (global.flagServerDetailedLog) {
    global << "Detail: webServer received SIGCHLD signal. " << logger::endL;
  }
  global.server().flagReapingChildren = true;
  global.server().ReapChildren();
}

void WebServer::ReapChildren() {
  // Attention: this function is executed simultaneously with the
  // main execution path. Any non-standard operations here may be racy.
  // Please avoid allocating RAM memory outside of the stack.
  // Do not use anything that is not thread-safe here.
  // In particular, do not use any loggers.
  int waitResult = 0;
  int exitFlags = WNOHANG| WEXITED;
  do {
    waitResult = waitpid(- 1, nullptr, exitFlags);
    if (waitResult > 0) {
      for (int i = 0; i < this->theWorkers.size; i++) {
        if (this->theWorkers[i].ProcessPID == waitResult) {
          this->theWorkers[i].flagExited = true;
          this->NumProcessesReaped ++;
        }
      }
    }
  } while (waitResult > 0);
}

WebWorker& WebServer::GetActiveWorker() {
  MacroRegisterFunctionWithName("WebServer::GetActiveWorker");
  if (this->activeWorker < 0 || this->activeWorker >= this->theWorkers.size) {
    global.fatal << "Active worker index is " << this->activeWorker << " however I have " << this->theWorkers.size
    << " workers. " << global.fatal;
  }
  return this->theWorkers[this->activeWorker];
}

void WebServer::WrapUp() {
  MacroRegisterFunctionWithName("WebServer::WrapUp");
  if (global.server().activeWorker == - 1) {
    return;
  }
  global.server().GetActiveWorker().WrapUpConnection();
  global.server().activeWorker = - 1;
}

void WebServer::ReleaseActiveWorker() {
  MacroRegisterFunctionWithName("WebServer::ReleaseActiveWorker");
  if (this->activeWorker == - 1) {
    return;
  }
  this->GetActiveWorker().Release();
  this->activeWorker = - 1;
}

void WebServer::WorkerTimerPing(int64_t pingTime) {
  if (global.server().activeWorker == - 1) {
    if (!global.flagComputationFinishedAllOutputSentClosing) {
      global.fatal << "No active worker yet the computation is not complete. " << global.fatal;
    }
    return;
  }
  std::stringstream outTimestream;
  outTimestream << pingTime;
  global.server().GetActiveWorker().pipeWorkerToServerTimerPing.WriteOnceAfterEmptying(outTimestream.str(), false);
}

void WebServer::ReleaseNonActiveWorkers() {
  MacroRegisterFunctionWithName("WebServer::ReleaseNonActiveWorkers");
  for (int i = 0; i < this->theWorkers.size; i ++) {
    if (i != this->activeWorker) {
      this->theWorkers[i].ReleaseKeepInUseFlag();
    }
  }
}

void WebServer::ReleaseSocketsNonActiveWorkers() {
  MacroRegisterFunctionWithName("WebServer::ReleaseSocketsNonActiveWorkers");
  for (int i = 0; i < this->theWorkers.size; i ++) {
    if (i != this->activeWorker) {
      this->Release(this->theWorkers[i].connectedSocketID);
    }
  }
}

bool WebServer::EmergencyRemoval_LastCreatedWorker() {
  this->GetActiveWorker().Release();
  this->activeWorker = - 1;
  this->theWorkers.setSize(this->theWorkers.size - 1);
  return false;
}

std::string WebServer::ToStringWorkerToWorker() {
  std::stringstream out;
  out << "W<->W" << this->activeWorker + 1 << ": ";
  return out.str();
}

bool WebServer::CreateNewActiveWorker() {
  MacroRegisterFunctionWithName("WebServer::CreateNewActiveWorker");
  if (this->activeWorker != - 1) {
    global.fatal << "Calling CreateNewActiveWorker requires the active worker index to be - 1." << global.fatal;
    return false;
  }
  bool found = false;
  for (int i = 0; i < this->theWorkers.size; i ++) {
    if (!this->theWorkers[i].flagInUsE) {
      this->activeWorker = i;
      found = true;
      break;
    }
  }
  if (this->activeWorker == - 1) {
    this->activeWorker = this->theWorkers.size;
    this->theWorkers.setSize(this->theWorkers.size + 1);
  }
  this->GetActiveWorker().indexInParent = this->activeWorker;
  this->GetActiveWorker().parent = this;
  this->GetActiveWorker().pingMessage = "";
  if (found) {
    this->theWorkers[this->activeWorker].flagInUsE = true;
    this->theWorkers[this->activeWorker].flagExited = false;
    return true;
  }
  this->GetActiveWorker().Release();
  this->theWorkers[this->activeWorker].flagInUsE = false; //<-until everything is initialized, we cannot be in use.
  std::stringstream stowstream, wtosStream;
  stowstream << "S->W" << this->activeWorker + 1 << ": ";
  wtosStream << "W" << this->activeWorker + 1 << "->S: ";
  std::string stow = stowstream.str();
  std::string wtos = wtosStream.str();
  std::string wtow = this->ToStringWorkerToWorker();
  WebWorker& worker = this->GetActiveWorker();
  if (!worker.workerToWorkerRequestIndicator.CreateMe(wtow + "request-indicator", false, false, true)) {
    global << "Failed to create pipe: "
    << worker.workerToWorkerRequestIndicator.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!worker.pipeWorkerToServerTimerPing.CreateMe(wtos + "ping", false, false, true)) {
    global << "Failed to create pipe: "
    << worker.pipeWorkerToServerTimerPing.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!worker.pipeWorkerToServerControls.CreateMe(wtos + "controls", false, false, true)) {
    global << "Failed to create pipe: "
    << worker.pipeWorkerToServerControls.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!worker.pipeWorkerToWorkerStatus.CreateMe(wtos + "worker status", false, false, true)) {
    global << "Failed to create pipe: "
    << worker.pipeWorkerToWorkerStatus.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  global << logger::green
  << "Allocated new worker & plumbing data structures. Total worker data structures: "
  << this->theWorkers.size << ". "
  << logger::endL;
  this->theWorkers[this->activeWorker].workerId = "";
  this->theWorkers[this->activeWorker].flagInUsE = true;
  this->theWorkers[this->activeWorker].flagExited = false;

  return true;
}

std::string WebServer::ToStringLastErrorDescription() {
  std::stringstream out;
  out << strerror(errno) << ". ";
  return out.str();
}

std::string WebServer::ToStringStatusActive() {
  MacroRegisterFunctionWithName("WebServer::ToStringStatusActive");
  if (this->activeWorker == - 1) {
    return "server.";
  }
  std::stringstream out;
  if (this->activeWorker != this->GetActiveWorker().indexInParent) {
    global.fatal << "Bad index in parent!" << global.fatal;
  }
  out << this->GetActiveWorker().ToStringStatus();
  return out.str();
}

std::string WebServer::ToStringConnectionSummary() {
  MacroRegisterFunctionWithName("WebServer::ToStringConnectionSummary");
  std::stringstream out;
  TimeWrapper now;
  now.AssignLocalTime();
  out << "<b>Server status.</b> Server time: local: " << now.ToStringLocal() << ", gm: " << now.ToStringGM() << ".<br>";
  int64_t timeRunninG = - 1;
  if (this->activeWorker < 0 || this->activeWorker >= this->theWorkers.size) {
    timeRunninG = global.GetElapsedMilliseconds();
  } else {
    timeRunninG = this->GetActiveWorker().millisecondsLastPingServerSideOnly;
  }
  out
  << (double(timeRunninG) / 1000)
  << " seconds = "
  << TimeWrapper::ToStringSecondsToDaysHoursSecondsString(timeRunninG / 1000, false, false)
  << " web server uptime. ";
  int64_t approxNumPings = timeRunninG / this->WebServerPingIntervalInSeconds / 1000;
  if (approxNumPings < 0) {
    approxNumPings = 0;
  }
  int64_t numConnectionsSoFarApprox = this->NumConnectionsSoFar - approxNumPings;
  if (numConnectionsSoFarApprox < 0) {
    numConnectionsSoFarApprox = 0;
  }
  out << "~" << numConnectionsSoFarApprox << " actual connections "
  << "(with " << this->NumberOfServerRequestsWithinAllConnections << " server requests served)" << " + ~"
  << approxNumPings << " self-test-pings (" << this->NumConnectionsSoFar << " connections total) "
  << "served since last restart. ";

  out
  << "The number tends to be high as many browsers open more than one connection per page visit. <br>"
  << "<b>The following policies are quite strict and will be relaxed in the future.</b><br>"
  << this->MaxTotalUsedWorkers << " global maximum of simultaneous non-closed connections allowed. "
  << "When the limit is exceeded, all connections except a randomly chosen one will be terminated. "
  << "<br> " << this->MaxNumWorkersPerIPAdress
  << " maximum simultaneous connection per IP address. "
  << "When the limit is exceeded, all connections from that IP address are terminated. ";
  return out.str();
}

std::string WebServer::ToStringStatusForLogFile() {
  MacroRegisterFunctionWithName("WebServer::ToStringStatusForLogFile");
  std::stringstream out;
  out << this->ToStringConnectionSummary();
  int numInUse = 0;
  for (int i = 0; i < this->theWorkers.size; i ++) {
    if (this->theWorkers[i].flagInUsE) {
      numInUse ++;
    }
  }
  out << "<hr>Currently, there are " << numInUse
  << " worker(s) in use. The peak number of worker(s)/concurrent connections was "
  << this->theWorkers.size << ". ";
  out
  << "<br>kill commands: " << this->NumProcessAssassinated
  << ", processes reaped: " << this->NumProcessesReaped
  << ", normally reclaimed workers: " << this->NumWorkersNormallyExited
  << ", connections so far: " << this->NumConnectionsSoFar;
  return out.str();
}

std::string WebServer::ToStringStatusAll() {
  MacroRegisterFunctionWithName("WebServer::ToStringStatusAll");
  std::stringstream out;

  if (!global.UserDefaultHasAdminRights()) {
    out << this->ToStringConnectionSummary();
    return out.str();
  }
  out << this->ToStringStatusForLogFile();
  out << "<hr>";
  out << "<a href=\"/LogFiles/server_starts_and_unexpected_restarts.html\">" << "Log files</a><br>";
  out << "<a href=\"/LogFiles/" << GlobalVariables::GetDateForLogFiles() << "/\">" << "Current log files</a><hr>";
  if (this->activeWorker == - 1) {
    out << "The is a server process.";
  } else {
    out << "The is a worker process. Active worker: " << this->activeWorker + 1 << ". ";
    out << "<br>" << this->ToStringStatusActive();
  }
  for (int i = 0; i < this->theWorkers.size; i ++) {
    WebWorker& currentWorker = this->theWorkers[i];
    if (!currentWorker.flagInUsE) {
      continue;
    }
    currentWorker.pipeWorkerToWorkerStatus.ReadOnceWithoutEmptying(false);
    currentWorker.status = currentWorker.pipeWorkerToWorkerStatus.GetLastRead();
  }
  out << "<hr>";
  out << "Connections: " << this->currentlyConnectedAddresses.toString();
  for (int i = 0; i < this->theWorkers.size; i ++) {
    out << "<hr>" << this->theWorkers[i].ToStringStatus();
  }
  return out.str();
}

bool WebServer::RestartIsNeeded() {
  MacroRegisterFunctionWithName("WebServer::RestartIsNeeded");
  struct stat theFileStat;
  if (stat(global.PhysicalNameExecutableWithPath.c_str(), &theFileStat) != 0) {
    return false;
  }
  if (this->timeLastExecutableModification == - 1) {
    return false;
  }
  if (this->timeLastExecutableModification == theFileStat.st_ctime) {
    return false;
  }
  std::stringstream out;
  out << "<html>";
  out << "<head><script language =\"javascript\">setTimeout(resubmit, 500); "
  << " function resubmit() { location.reload(true);}</script></head>";
  out << "<body>";

  out << "<b>The server executable was updated, "
  << "but the server has not been restarted yet. "
  << "Restarting in 0.5 seconds...</b>";
  out << "</body></html>";
  global << "Current process spawned from file with time stamp: "
  << this->timeLastExecutableModification
  << "; latest executable has different time stamp: " << theFileStat.st_ctime
  << ". " << logger::red << "RESTARTING." << logger::endL;
  global.flagRestartNeeded = true;
  this->GetActiveWorker().WriteToBody(out.str());
  return true;
}

void WebServer::StopKillAll() {
  if (
    global.logs.logType != GlobalVariables::LogData::type::server &&
    global.logs.logType != GlobalVariables::LogData::type::serverMonitor
  ) {
    global.fatal << "Server restart is allowed only to "
    << "the server/monitor processes. " << global.fatal;
  }
  global << logger::red << "Server restart requested. " << logger::endL;
  global << "Sending kill signal to all copies of the calculator. " << logger::endL;
  for (int i = 0; i < this->theWorkers.size; i ++) {
    this->TerminateChildSystemCall(i);
  }
  SignalsInfrastructure::theSignals().unblockSignals();
  global << "Waiting for child processes to exit. " << logger::endL;
  int workersStillInUse = 0;
  int waitAttempts = 0;
  int maximumWaitAttempts = 30;
  while (true) {
    workersStillInUse = 0;
    for (int i = 0 ; i < this->theWorkers.size; i ++) {
      if (!this->theWorkers[i].flagExited && this->theWorkers[i].flagInUsE) {
        workersStillInUse ++;
        break;
      }
    }
    if (workersStillInUse == 0) {
      break;
    }
    waitAttempts ++;
    if (waitAttempts > maximumWaitAttempts) {
      global << logger::red << "Child exit timeout: made "
      << waitAttempts << " attempts to exit. " << logger::endL;
      break;
    }
    global << logger::blue << "Still waiting on "
    << workersStillInUse << " workers to finish. " << logger::endL;
    global.FallAsleep(1000000);
  }
  this->ReleaseEverything();
  exit(0);
}

void WebServer::initPortsITry() {
  this->portHTTP = global.configuration[Configuration::portHTTP].theString;
  if (!global.flagSSLIsAvailable) {
    return;
  }
  this->portHTTPSOpenSSL = global.configuration[Configuration::portHTTPSOpenSSL].theString;
  this->portHTTPSBuiltIn = global.configuration[Configuration::portHTTPSBuiltIn].theString;
  this->portHTTPSDefault = this->portHTTPSOpenSSL;
}

void WebServer::initListeningSockets() {
  MacroRegisterFunctionWithName("WebServer::initListeningSockets");
  this->highestSocketNumber = - 1;
  if (this->listeningSocketHTTP != - 1) {
    this->theListeningSockets.addOnTop(this->listeningSocketHTTP);
  }
  if (this->listeningSocketHTTPSBuiltIn != - 1) {
    this->theListeningSockets.addOnTop(this->listeningSocketHTTPSBuiltIn);
    this->listeningSocketHTTPSDefault = this->listeningSocketHTTPSBuiltIn;
  }
  if (this->listeningSocketHTTPSOpenSSL != - 1) {
    this->theListeningSockets.addOnTop(this->listeningSocketHTTPSOpenSSL);
    this->listeningSocketHTTPSDefault = this->listeningSocketHTTPSOpenSSL;
  }
  this->highestSocketNumber = - 1;
  for (int i = 0; i < this->theListeningSockets.size; i ++) {
    this->highestSocketNumber = MathRoutines::Maximum(
      this->theListeningSockets[i], this->highestSocketNumber
    );
    if (listen(this->theListeningSockets[i], WebServer::maxNumPendingConnections) == - 1) {
      global.fatal << "Listen function failed on socket: " << this->theListeningSockets[i] << global.fatal;
    }
  }
}

void WebServer::initDates() {
  this->timeLastExecutableModification = - 1;
  struct stat theFileStat;
  if (stat(global.PhysicalNameExecutableWithPath.c_str(), &theFileStat) != 0) {
    return;
  }
  this->timeLastExecutableModification = theFileStat.st_ctime;
}

void WebServer::ReleaseWorkerSideResources() {
  MacroRegisterFunctionWithName("WebServer::ReleaseWorkerSideResources");
  if (global.flagServerDetailedLog) {
    global << logger::red << "Detail: server about to RELEASE active workder. " << logger::endL;
  }
  this->Release(this->GetActiveWorker().connectedSocketID);
  if (global.flagServerDetailedLog) {
    global << logger::green << "Detail: server RELEASED active worker. " << logger::endL;
  }
  //<-release socket- communication is handled by the worker.
  this->activeWorker = - 1; //<-The active worker is needed only in the child process.
}

bool WebServer::RequiresLogin(const std::string& inputRequest, const std::string& inputAddress) {
  MacroRegisterFunctionWithName("WebServer::RequiresLogin");
  if (inputAddress == global.DisplayNameExecutable) {
    if (this->requestsNotNeedingLogin.Contains(inputRequest)) {
      return false;
    }
  }
  for (int i = 0; i < this->addressStartsNotNeedingLogin.size; i ++) {
    if (StringRoutines::StringBeginsWith(inputAddress, this->addressStartsNotNeedingLogin[i])) {
      return false;
    }
  }
  return true;
}

void segfault_sigaction[[noreturn]](int signal, siginfo_t* si, void* arg) {
  // <- this signal should never happen in
  // <- server, so even if racy, we take the risk of a hang.
  // <- racy-ness in child process does not bother us: hanged children are still fine.
  (void) signal; //avoid unused parameter warning, portable.
  (void) arg;
  global.fatal << "Caught segfault at address: " << si->si_addr << global.fatal;
  exit(0);
}

void WebServer::fperror_sigaction[[noreturn]](int signal) {
  (void) signal;
  global.fatal << "Fatal arithmetic error. " << global.fatal;
  exit(0);
}

void WebServer::TerminateProcessId(int processId) {
  kill(processId, SIGKILL);
}

void WebServer::TerminateChildSystemCall(int i) {
  // Signal lock not needed: signals are unlocked outside of the select loop.
  if (!this->theWorkers[i].flagInUsE || this->theWorkers[i].flagExited) {
    return;
  }
  this->MarkChildNotInUse(i);
  if (this->theWorkers[i].ProcessPID > 0) {
    if (global.flagServerDetailedLog) {
      global << "Detail: " << " killing child index: " << i << "." << logger::endL;
    }
    this->TerminateProcessId(this->theWorkers[i].ProcessPID);
    this->theWorkers[i].ProcessPID = - 1;
  }
}

void WebServer::HandleTooManyConnections(const std::string& incomingUserAddress) {
  MacroRegisterFunctionWithName("WebServer::HandleTooManyConnections");
  if (global.flagIsChildProcess) {
    return;
  }
  if (global.flagServerDetailedLog) {
    global << logger::red << "Detail: "
    << "too many connections handler start. " << logger::endL;
  }
  MonomialWrapper<std::string, MathRoutines::HashString>
  incomingAddress(incomingUserAddress);
  bool purgeIncomingAddress = (
    this->currentlyConnectedAddresses.GetMonomialCoefficient(incomingAddress) >
    this->MaxNumWorkersPerIPAdress
  );
  if (!purgeIncomingAddress) {
    return;
  }
  List<double> theTimes;
  List<int> theIndices;
  for (int i = 0; i < this->theWorkers.size; i ++) {
    if (this->theWorkers[i].flagInUsE) {
      if (this->theWorkers[i].userAddress == incomingAddress) {
        theTimes.addOnTop(this->theWorkers[i].millisecondsServerAtWorkerStart);
        theIndices.addOnTop(i);
      }
    }
  }
  theTimes.QuickSortAscending(nullptr, &theIndices);
  if (!SignalsInfrastructure::theSignals().flagSignalsAreBlocked) {
    global.fatal << "Signals must be blocked at this point of code. " << global.fatal;
  }
  for (int j = 0; j < theTimes.size; j ++) {
    // Child processes exit completely when their sigchld
    // signal has been handled.
    // Signal processing is blocked outside of the select loop.
    // Therefore
    // any children that have exited properly are still not reaped
    // at this point in code.
    // In particular, it should not be possible to terminate by accident
    // a pid that is not owned by the server.
    this->TerminateChildSystemCall(theIndices[j]);
    this->NumProcessAssassinated ++;
    std::stringstream errorStream;
    errorStream
    << "Terminating child " << theIndices[j] + 1 << " with PID "
    << this->theWorkers[theIndices[j]].ProcessPID
    << ": address: " << incomingAddress << " opened more than "
    << this->MaxNumWorkersPerIPAdress << " simultaneous connections. ";
    this->theWorkers[theIndices[j]].pingMessage = errorStream.str();
    global << logger::red << errorStream.str() << logger::endL;
  }
  if (global.flagServerDetailedLog) {
    global << logger::green
    << "Detail: connection cleanup successful. " << logger::endL;
  }
}

void WebServer::MarkChildNotInUse(int childIndex) {
  WebWorker& worker = this->theWorkers[childIndex];
  worker.flagInUsE = false;
  this->currentlyConnectedAddresses.SubtractMonomial(
    worker.userAddress, 1
  );
  this->workerIds.RemoveKey(worker.workerId);
  worker.workerId = "";
}

void WebServer::ProcessOneChildMessage(int childIndex, int& outputNumInUse) {
  std::string messageString = this->theWorkers[childIndex].pipeWorkerToServerControls.GetLastRead();
  this->MarkChildNotInUse(childIndex);
  std::stringstream commentsOnFailure;
  JSData workerMessage;
  if (!workerMessage.readstring(messageString, &commentsOnFailure)) {
    global << logger::red << "Worker "
    << childIndex + 1 << " sent corrupted result message: "
    << messageString << ". Marking for reuse. " << logger::endL;
  } else {
    global << logger::green << "Worker "
    << childIndex + 1 << " done with message: "
    << messageString
    << ". Marking for reuse. " << logger::endL;
  }
  outputNumInUse --;
  this->NumWorkersNormallyExited ++;
  if (workerMessage["connectionsServed"].theType == JSData::token::tokenLargeInteger) {
    this->NumberOfServerRequestsWithinAllConnections +=
    workerMessage["connectionsServed"].theInteger.GetElement().GetIntValueTruncated()
    ;
  }
  if (
    workerMessage["stopNeeded"].isTrueRepresentationInJSON() ||
    workerMessage["restartNeeded"].isTrueRepresentationInJSON()
  ) {
    this->StopKillAll();
  }
}

void WebServer::RecycleOneChild(int childIndex, int& numberInUse) {
  if (!this->theWorkers[childIndex].flagInUsE) {
    return;
  }
  WebWorker& currentWorker = this->theWorkers[childIndex];
  PipePrimitive& currentControlPipe = currentWorker.pipeWorkerToServerControls;
  if (currentControlPipe.flagReadEndBlocks) {
    global.fatal << "Pipe: " << currentControlPipe.toString() << " has blocking read end. " << global.fatal;
  }
  currentControlPipe.ReadOnceIfFailThenCrash(true);
  if (currentControlPipe.lastRead.size > 0) {
    this->ProcessOneChildMessage(childIndex, numberInUse);
  } else {
    global << logger::orange << "Worker " << childIndex + 1 << " not done yet. " << logger::endL;
  }
  PipePrimitive& currentPingPipe = currentWorker.pipeWorkerToServerTimerPing;
  if (currentPingPipe.flagReadEndBlocks) {
    global.fatal << "Pipe: " << currentPingPipe.toString() << " has blocking read end. " << global.fatal;
  }
  currentPingPipe.ReadOnceIfFailThenCrash(true);
  if (currentPingPipe.lastRead.size > 0) {
    currentWorker.pingMessage = currentPingPipe.GetLastRead();
    currentWorker.millisecondsLastPingServerSideOnly = global.GetElapsedMilliseconds();
    if (currentWorker.pingMessage != "") {
      global << logger::blue << "Worker " << childIndex + 1 << " ping: "
      << currentWorker.pingMessage << ". " << logger::endL;
    }
    return;
  }
  if (global.millisecondsNoPingBeforeChildIsPresumedDead <= 0) {
    return;
  }
  int64_t millisecondsElapsed = global.GetElapsedMilliseconds() - currentWorker.millisecondsLastPingServerSideOnly;
  if (millisecondsElapsed <= global.millisecondsNoPingBeforeChildIsPresumedDead) {
    return;
  }
  this->TerminateChildSystemCall(childIndex);
  std::stringstream pingTimeoutStream;
  pingTimeoutStream
  << millisecondsElapsed
  << " milliseconds passed since worker " << childIndex + 1
  << " last pinged the server; "
  << "the maximum allowed is: " << global.millisecondsNoPingBeforeChildIsPresumedDead
  << ". ";
  pingTimeoutStream << "Killing connection "
  << currentWorker.connectionID
  << ", pid: "
  << currentWorker.ProcessPID << ". ";
  global << logger::red << pingTimeoutStream.str() << logger::endL;
  currentWorker.pingMessage = "<b style =\"color:red\">" + pingTimeoutStream.str() + "</b>";
  numberInUse --;
  this->NumProcessAssassinated ++;
}

void WebServer::HandleTooManyWorkers(int& numInUse) {
  if (numInUse <= this->MaxTotalUsedWorkers) {
    if (global.flagServerDetailedLog) {
      global << logger::green
      << "Detail: RecycleChildrenIfPossible exit point 1. " << logger::endL;
    }
    return;
  }
  for (int i = 0; i < this->theWorkers.size && numInUse > 1; i ++) {
    if (!this->theWorkers[i].flagInUsE) {
      continue;
    }
    this->TerminateChildSystemCall(i);
    std::stringstream errorStream;
    errorStream
    << "Terminating child " << i + 1 << " with PID "
    << this->theWorkers[i].ProcessPID
    << ": too many workers in use. ";
    this->theWorkers[i].pingMessage = errorStream.str();
    global << logger::red << errorStream.str() << logger::endL;
    numInUse --;
    this->NumProcessAssassinated ++;
  }
  if (global.flagServerDetailedLog) {
    global << logger::green
    << "Detail: RecycleChildrenIfPossible exit point 2. " << logger::endL;
  }
}

void WebServer::RecycleChildrenIfPossible() {
  // Listen for children who have exited properly.
  // This might need to be rewritten: I wasn't able to make this work with any
  // mechanism other than pipes.
  MacroRegisterFunctionWithName("WebServer::RecycleChildrenIfPossible");
  if (global.flagIsChildProcess) {
    return;
  }
  if (global.flagServerDetailedLog) {
    global << logger::red << "Detail: RecycleChildrenIfPossible start. " << logger::endL;
  }
  int numInUse = 0;
  for (int i = 0; i < this->theWorkers.size; i ++) {
    if (this->theWorkers[i].flagInUsE) {
      numInUse ++;
    }
  }
  for (int i = 0; i < this->theWorkers.size; i ++) {
    this->RecycleOneChild(i, numInUse);
  }
  this->HandleTooManyWorkers(numInUse);
}

bool WebServer::initPrepareWebServerALL() {
  MacroRegisterFunctionWithName("WebServer::initPrepareWebServerALL");
  this->initPortsITry();
  this->initDates();
  if (!this->initBindToPorts()) {
    return false;
  }
  this->initializeSignals();
  this->initListeningSockets();
  return true;
}

bool WebServer::initBindToOnePort(const std::string& thePort, int& outputListeningSocket) {
  MacroRegisterFunctionWithName("WebServer::initBindToOnePort");
  addrinfo hints;
  addrinfo *servinfo = nullptr;
  addrinfo *p = nullptr;
  int yes = 1;
  int rv;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP
  // loop through all the results and bind to the first we can
  rv = getaddrinfo(nullptr, thePort.c_str(), &hints, &servinfo);
  if (rv != 0) {
    global << "getaddrinfo: " << gai_strerror(rv) << logger::endL;
    return false;
  }
  outputListeningSocket = - 1;
  for (p = servinfo; p != nullptr; p = p->ai_next) {
    outputListeningSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (outputListeningSocket == - 1) {
      global << "Error: socket failed.\n";
      continue;
    }
    if (setsockopt(outputListeningSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == - 1) {
      global.fatal << "Error: setsockopt failed, error: \n" << strerror(errno) << global.fatal;
    }
    if (bind(outputListeningSocket, p->ai_addr, p->ai_addrlen) == - 1) {
      close(outputListeningSocket);
      outputListeningSocket = - 1;
      global << "Error: bind failed at port: " << thePort << ". Error: "
      << this->ToStringLastErrorDescription() << logger::endL;
      continue;
    }
    int setFlagCounter = 0;
    while (fcntl(outputListeningSocket, F_SETFL, O_NONBLOCK) != 0) {
      if (++ setFlagCounter > 10) {
        global.fatal << "Error: failed to set non-blocking status to listening socket. " << global.fatal;
      }
    }
    break;
  }
  freeaddrinfo(servinfo); // all done with this structure
  if (outputListeningSocket == - 1) {
    global.fatal << "Failed to bind to port: " << thePort << ". " << global.fatal;
  }
  return true;
}

bool WebServer::initBindToPorts() {
  MacroRegisterFunctionWithName("WebServer::initBindToPorts");
  if (!this->initBindToOnePort(this->portHTTP, this->listeningSocketHTTP)) {
    return false;
  }
  global << "Running at: " << logger::yellow << "http://localhost:" << this->portHTTP << logger::endL;
  bool flagBuiltInTLSAvailable = false;
  if (flagBuiltInTLSAvailable) {
    if (!this->initBindToOnePort(this->portHTTPSBuiltIn, this->listeningSocketHTTPSBuiltIn)) {
      return false;
    }
  }
  if (global.flagSSLIsAvailable) {
    if (!this->initBindToOnePort(this->portHTTPSOpenSSL, this->listeningSocketHTTPSOpenSSL)) {
      return false;
    }
    global << "Running at: " << logger::yellow << "https://localhost:" << this->portHTTPSOpenSSL << logger::endL;
  }
  return true;
}

void WebServer::initializeSignals() {
  SignalsInfrastructure::theSignals().initializeSignals();
}

void SignalsInfrastructure::initializeSignals() {
  MacroRegisterFunctionWithName("SignalsInfrastructure::initializeSignals");
  if (this->flagInitialized) {
    return;
  }
  sigfillset(&this->allSignals);
  if (sigemptyset(&this->SignalSEGV.sa_mask) == - 1) {
    global.fatal << "Failed to initialize SignalSEGV mask. Crashing to let you know. " << global.fatal;
  }
  this->SignalSEGV.sa_sigaction = &segfault_sigaction;
  this->SignalSEGV.sa_flags = SA_SIGINFO;
  if (sigaction(SIGSEGV, &SignalSEGV, nullptr) == - 1) {
    global.fatal << "Failed to register SIGSEGV handler "
    << "(segmentation fault (attempt to write memory without permission)). "
    << "Crashing to let you know. " << global.fatal;
  }
  ///////////////////////
  //catch floating point exceptions
  if (sigemptyset(&SignalFPE.sa_mask) == - 1) {
    global.fatal << "Failed to initialize SignalFPE mask. Crashing to let you know. " << global.fatal;
  }
  SignalFPE.sa_sigaction = nullptr;
  SignalFPE.sa_handler = &WebServer::fperror_sigaction;
  if (sigaction(SIGFPE, &SignalFPE, nullptr) == - 1) {
    global.fatal << "Failed to register SIGFPE handler (fatal arithmetic error). Crashing to let you know. " << global.fatal;
  }
  ////////////////////
  //ignore interruptsflagSignalsInitialized
  //if (sigemptyset(&SignalINT.sa_mask) == - 1)
  //  global.fatal << "Failed to initialize SignalINT mask. Crashing to let you know. " << global.fatal;
  //SignalINT.sa_sigaction = 0;
  //SignalINT.sa_handler = &WebServer::Signal_SIGINT_handler;
  //if (sigaction(SIGINT, &SignalINT, NULL) == - 1)
  //  global.fatal << "Failed to register null SIGINT handler. Crashing to let you know. " << global.fatal;
  ////////////////////
  //reap children
  //if (sigemptyset(&SignalChild.sa_mask) == - 1)
  //  global.fatal << "Failed to initialize SignalChild mask. Crashing to let you know. " << global.fatal;
  //if (sigaddset(&SignalChild.sa_mask, SIGINT) == - 1)
  //  global.fatal << "Failed to initialize SignalChild mask. Crashing to let you know. " << global.fatal;
  ////////////////////////////////
  //sigchld signal should automatically be blocked when calling the sigchld handler.
  //Nevertheless, let's explicitly add it:
  //if (sigaddset(&SignalChild.sa_mask, SIGCHLD) == - 1)
  //  global.fatal << "Failed to initialize SignalChild mask. Crashing to let you know. " << global.fatal;
  ////////////////////////////////
  //if (sigaddset(&SignalChild.sa_mask, SIGFPE) == - 1)
  //  global.fatal << "Failed to initialize SignalChild mask. Crashing to let you know. " << global.fatal;
  //if (sigaddset(&SignalChild.sa_mask, SIGSEGV) == - 1)
  //  global.fatal << "Failed to initialize SignalChild mask. Crashing to let you know. " << global.fatal;
  SignalChild.sa_flags = SA_NOCLDWAIT;
  SignalChild.sa_handler = &WebServer::Signal_SIGCHLD_handler; // reap all dead processes
  if (sigaction(SIGCHLD, &SignalChild, nullptr) == - 1) {
    global.fatal << "Was not able to register SIGCHLD handler (reaping child processes). Crashing to let you know." << global.fatal;
  }
//  sigemptyset(&sa.sa_mask);
//  sa.sa_flags = SA_RESTART;
//  if (sigaction(SIGCHLD, &sa, NULL) == - 1)
//    global << "sigaction returned - 1" << logger::endL;
  this->flagInitialized = true;
}

extern void MonitorWebServer(int pidServer);

void WebServer::WriteVersionJSFile() {
  MacroRegisterFunctionWithName("WebServer::WriteVersionJSFile");
  std::stringstream out;
  out << "\"use strict\"; //This file is automatically generated, please do not modify.\n";
  out << "var serverInformation = {\n  ";
  out << "  version:\"" << global.buildVersionSimple << "\",\n";
  out << "};\n";
  out << "module.exports = {\nserverInformation,\n};\n";
  std::fstream theFileStream;
  FileOperations::OpenFileCreateIfNotPresentVirtual(
    theFileStream, "/calculator-html/server_information.js", false, true, false, false
  );
  theFileStream << out.str();
}

// class not included in header for portability reasons
// - the header needs not to know of
// the fd_set and sockaddr_storage structures.
class Listener {
public:
  fd_set FDSetListenSockets;
  WebServer* owner;
  sockaddr_storage theirAddress; // connector's address information
  char userAddressBuffer[INET6_ADDRSTRLEN];
  std::string userAddress;
  void zeroSocketSet();
  Listener(WebServer* inputOwner) {
    this->owner = inputOwner;
  }
  int Accept();
  void Select();
  void ComputeUserAddress();
};

void Listener::zeroSocketSet() {
  FD_ZERO(&this->FDSetListenSockets);
  for (int i = 0; i < this->owner->theListeningSockets.size; i ++) {
    FD_SET(this->owner->theListeningSockets[i], &this->FDSetListenSockets);
  }
  if (global.flagServerDetailedLog) {
    global << logger::red << "Detail: About to enter select loop. " << logger::endL;
  }

}

void Listener::Select() {
  while (select(this->owner->highestSocketNumber + 1, &this->FDSetListenSockets, nullptr, nullptr, nullptr) == - 1) {
    if (this->owner->flagReapingChildren) {
      if (global.flagServerDetailedLog) {
        global << logger::yellow << "Interrupted select loop by child exit signal. "
        << logger::endL;
      }
      this->owner->flagReapingChildren = false;
    } else {
      global << logger::red << "Select failed: this is not expected. Error message: "
      << strerror(errno) << logger::endL;
    }
    this->owner->NumFailedSelectsSoFar ++;
  }
}

int Listener::Accept() {
  MacroRegisterFunctionWithName("Listener::Accept");
  socklen_t sin_size = sizeof (this->theirAddress);
  for (int i = this->owner->theListeningSockets.size - 1; i >= 0; i --) {
    int currentListeningSocket = this->owner->theListeningSockets[i];
    if (!FD_ISSET(currentListeningSocket, &this->FDSetListenSockets)) {
      continue;
    }
    int result = accept(currentListeningSocket, reinterpret_cast<struct sockaddr *>(&this->theirAddress), &sin_size);
    if (result >= 0) {
      this->owner->lastListeningSocket = currentListeningSocket;
      global << logger::green << "Connection candidate "
      << this->owner->NumConnectionsSoFar + 1 << ". "
      << "Connected via listening socket " << currentListeningSocket
      << " on socket: " << result << logger::endL;
      return result;
    } else {
      global << logger::red
      << "This is not supposed to happen: accept failed. Error: "
      << this->owner->ToStringLastErrorDescription() << logger::endL;
    }
  }
  return - 1;
}

void Listener::ComputeUserAddress() {
  inet_ntop(
    this->theirAddress.ss_family,
    get_in_addr(reinterpret_cast<struct sockaddr *>(&this->theirAddress)),
    this->userAddressBuffer,
    sizeof this->userAddressBuffer
  );
  this->userAddress = this->userAddressBuffer;
}

TransportLayerSecurity& TransportLayerSecurity::DefaultTLS_READ_ONLY() {
  return global.server().theTLS;
}

int WebServer::Daemon() {
  MacroRegisterFunctionWithName("WebServer::Daemon");
  global.logs.logType = GlobalVariables::LogData::type::daemon;
  std::stringstream restartCommand;
  restartCommand << global.PhysicalNameExecutableWithPath << " ";
  for (int i = 2; i < global.programArguments.size; i ++) {
    restartCommand << global.programArguments[i] << " ";
  }
  while(true) {
    int pidChild = fork();
    if (pidChild == 0) {
      // child process
      global << logger::orange << "Development daemon is starting command:" << logger::endL;
      global << logger::green << restartCommand.str() << logger::endL;
      global.externalCommandStream(restartCommand.str());
    }
    int exitStatus = - 1;
    waitpid(pidChild, &exitStatus, 0);
    if (exitStatus != 0) {
      global << logger::red;
    } else {
      global << logger::green;
    }
    global << "Server exited with status: " << exitStatus << logger::endL;
    global.FallAsleep(100000);
  }
}

int WebServer::Run() {
  MacroRegisterFunctionWithName("WebServer::Run");
  global.RelativePhysicalNameCrashReport = "crash_WebServerRun.html";
  WebServer::initializeRandomBytes();
  if (global.millisecondsMaxComputation <= 0) {
    global
    << logger::purple << "************************" << logger::endL
    << logger::red << "WARNING: no computation time limit set. " << logger::endL
    << logger::purple << "************************" << logger::endL;
  }
  if (global.millisecondsMaxComputation > 500000) {
    global
    << logger::purple << "************************" << logger::endL
    << logger::red << "WARNING: computation time limit is high: "
    << (global.millisecondsMaxComputation / 1000)
    << " seconds. " << logger::endL
    << logger::purple << "************************" << logger::endL;
  }

  // <-worker log resets are needed, else forked processes reset their common log.
  // <-resets of the server logs are not needed, but I put them here nonetheless.
  if (global.flagSSLIsAvailable) {
    // creates key files if absent. Does not call any openssl functions.
    std::stringstream commentsOnFailure;
    TransportLayerSecurity::initializeNonThreadSafePartsCommon();
    this->theTLS.initSSLKeyFiles(&commentsOnFailure);
  }
  global.logs.server.reset();
  global.logs.serverMonitor.reset();
  global.logs.worker.reset();
  this->processIdServer = getpid();
  if (global.flagLocalhostConnectionMonitor) {
    global << logger::green << "Start monitor process." << logger::endL;
    this->processIdServer = this->Fork();
    if (this->processIdServer < 0) {
      global.fatal << "Failed to create server process. " << global.fatal;
    }
    if (this->processIdServer > 0) {
      global.logs.logType = GlobalVariables::LogData::type::serverMonitor;
      global.flagIsChildProcess = true;
      MonitorWebServer(this->processIdServer); //<-this attempts to connect to the server over the internet and restarts if it can't.
      return 0;
    }
  }
  this->initializeSignals();
  global.calculator().GetElement().initialize();
  // cannot call initializeMutex here: not before we execute Fork();
  global.calculator().GetElement().ComputeAutoCompleteKeyWords();
  global.calculator().GetElement().WriteAutoCompleteKeyWordsToFile();
  this->WriteVersionJSFile();
  global.initModifiableDatabaseFields();
  HtmlRoutines::LoadStrings();
  this->theTLS.initializeNonThreadSafeOnFirstCall(true);
  global.calculator().GetElement().flagShowCalculatorExamples = false;
  if (!this->initPrepareWebServerALL()) {
    return 1;
  }
  global << logger::purple << "waiting for connections..." << logger::endL;
  this->initSSL();
  this->NumSuccessfulSelectsSoFar = 0;
  this->NumFailedSelectsSoFar = 0;
  long long previousReportedNumberOfSelects = 0;
  Listener theListener(this);
  while (true) {
    // main accept() loop
    theListener.zeroSocketSet();
    SignalsInfrastructure::theSignals().unblockSignals();
    theListener.Select();
    SignalsInfrastructure::theSignals().blockSignals();
    if (global.flagServerDetailedLog) {
      global << logger::green << "Detail: select success. " << logger::endL;
    }
    int64_t millisecondsAfterSelect = global.GetElapsedMilliseconds();
    this->NumSuccessfulSelectsSoFar ++;
    if ((this->NumSuccessfulSelectsSoFar + this->NumFailedSelectsSoFar) - previousReportedNumberOfSelects > 100) {
      global << logger::blue << this->NumSuccessfulSelectsSoFar << " successful and "
      << this->NumFailedSelectsSoFar << " bad ("
      << this->NumSuccessfulSelectsSoFar + this->NumFailedSelectsSoFar << " total) selects. " << logger::endL;
      previousReportedNumberOfSelects = this->NumSuccessfulSelectsSoFar + this->NumFailedSelectsSoFar;
    }
    int reportCount =
    this->NumProcessAssassinated + this->NumProcessesReaped +
    this->NumWorkersNormallyExited + this->NumConnectionsSoFar;
    if (reportCount - previousServerStatReport > 99) {
      this->previousServerStatReport = reportCount;
      global << "# kill commands: " << this->NumProcessAssassinated
      << " #processes reaped: " << this->NumProcessesReaped
      << " #normally reclaimed workers: " << this->NumWorkersNormallyExited
      << " #connections so far: " << this->NumConnectionsSoFar << logger::endL;
    }
    if (reportCount - previousServerStatDetailedReport > 499) {
      this->previousServerStatDetailedReport = reportCount;
      global << this->ToStringStatusForLogFile() << logger::endL;
    }
    int newConnectedSocket = theListener.Accept();
    if (newConnectedSocket < 0) {
      global << logger::red
      << "This is not supposed to to happen: select succeeded "
      << "but I found no set socket. " << logger::endL;
    }
    if (newConnectedSocket < 0) {
      if (global.flagServerDetailedLog) {
        global << "Detail: newConnectedSocket is negative: "
        << newConnectedSocket << ". Not accepting. " << logger::endL;
      }
      continue;
    }
    theListener.ComputeUserAddress();
    this->HandleTooManyConnections(theListener.userAddress);
    this->RecycleChildrenIfPossible();
    if (!this->CreateNewActiveWorker()) {
      global << logger::purple
      << " failed to create an active worker. System error string: "
      << strerror(errno) << "\n"
      << logger::red << "Failed to create active worker: closing connection. " << logger::endL;
      close (newConnectedSocket);
      continue;
    }
    /////////////
    this->GetActiveWorker().connectedSocketID = newConnectedSocket;
    this->ComputeSSLFlags();
    this->GetActiveWorker().connectedSocketIDLastValueBeforeRelease = newConnectedSocket;
    this->GetActiveWorker().millisecondsServerAtWorkerStart = global.GetElapsedMilliseconds();
    this->GetActiveWorker().millisecondsLastPingServerSideOnly = this->GetActiveWorker().millisecondsServerAtWorkerStart;
    this->GetActiveWorker().millisecondsAfterSelect = millisecondsAfterSelect; //<- measured right after select()
    //<-cannot set earlier as the active worker may change after recycling.
    this->NumConnectionsSoFar ++;
    this->GetActiveWorker().connectionID = this->NumConnectionsSoFar;
    this->GetActiveWorker().userAddress.theObject = theListener.userAddress;
    this->currentlyConnectedAddresses.AddMonomial(this->GetActiveWorker().userAddress, 1);
//    global << this->ToStringStatus();
    /////////////
    if (global.flagServerDetailedLog) {
      global << "Detail: about to fork, sigprocmasking " << logger::endL;
    }
    if (global.flagServerDetailedLog) {
      global << "Detail: Sigprocmask done. Proceeding to fork. "
      << "Time elapsed: " << global.GetElapsedSeconds() << " second(s). <br>"
      << logger::endL;
    }
    int incomingPID = this->Fork(); // creates an almost identical copy of this process.
    // <- Please don't assign directly to this->GetActiveWorker().ProcessPID.
    // <- There may be a race condition around this line of code and I
    // want as little code as possible between the fork and the logFile.
    // The original process is the parent, the almost identical copy is the child.
    // global << "\r\nChildPID: " << this->childPID;
    if (global.flagServerDetailedLog && incomingPID == 0) {
      global << "Detail: Fork() successful in worker; elapsed ms @ Fork(): "
      << global.GetElapsedMilliseconds() << logger::endL;
    }
    if (global.flagServerDetailedLog && incomingPID > 0) {
      global << "Detail: Fork() successful; elapsed ms @ Fork(): "
      << global.GetElapsedMilliseconds() << logger::endL;
    }
    if (incomingPID < 0) {
      global << logger::red << " FAILED to spawn a child process. " << logger::endL;
    }
    this->GetActiveWorker().ProcessPID = incomingPID;
    if (this->GetActiveWorker().ProcessPID == 0) {
      // this is the child (worker) process
      global.flagIsChildProcess = true;
      if (global.flagServerDetailedLog) {
        global << logger::green << "Detail: "
        << "FORK successful in worker, next step. "
        << "Time elapsed: " << global.GetElapsedSeconds()
        << " second(s). Calling sigprocmask. " << logger::endL;
      }
      SignalsInfrastructure::theSignals().unblockSignals();
      if (global.flagServerDetailedLog) {
        global << logger::green << "Detail: sigprocmask success, running... " << logger::endL;
      }
      int result = this->GetActiveWorker().Run();
      if (global.flagServerDetailedLog) {
        global << "Detail: run finished, releasing resources. " << logger::endL;
      }
      this->ReleaseEverything();
      if (global.flagServerDetailedLog) {
        global << logger::green << "Detail: resources released, returning. " << logger::endL;
      }
      return result;
    }
    if (global.flagServerDetailedLog) {
      global << logger::green << "Detail: fork successful. Time elapsed: "
      << global.GetElapsedMilliseconds() << " ms. "
      << "About to unmask signals. " << logger::endL;
    }
    this->ReleaseWorkerSideResources();
  }
  // Cleanup, if ever needed:
  // this->ReleaseEverything();
  // this->theTLS.FreeEverythingShutdown();
  // return 0;
}

void WebServer::ComputeSSLFlags() {
  global.flagUsingSSLinCurrentConnection = false;

  if (this->lastListeningSocket == this->listeningSocketHTTPSBuiltIn) {
    global.flagUsingSSLinCurrentConnection = true;
    this->theTLS.flagUseBuiltInTlS = true;
  } else if (this->lastListeningSocket == this->listeningSocketHTTPSOpenSSL){
    global.flagUsingSSLinCurrentConnection = true;
    this->theTLS.flagUseBuiltInTlS = false;
  }
  this->GetActiveWorker().flagUsingSSLInWorkerProcess = global.flagUsingSSLinCurrentConnection;
}

bool WebWorker::RunInitialize() {
  global.millisecondOffset = this->millisecondsAfterSelect;
  this->CheckConsistency();
  if (this->connectedSocketID == - 1) {
    global.fatal << "Worker::Run() started on a connecting with ID equal to - 1. " << global.fatal;
  }
  std::stringstream processNameStream;
  processNameStream << "W" << this->indexInParent + 1 << ": ";
  MutexProcess::currentProcessName = processNameStream.str();
  global.flagServerForkedIntoWorker = true;
  CreateTimerThread();
  // Check web worker indices are initialized properly:
  global.server().GetActiveWorker();
  if (global.flagUsingSSLinCurrentConnection) {
    std::stringstream commentsOnFailure;
    if (!global.server().SSLServerSideHandShake(&commentsOnFailure)) {
      global.flagUsingSSLinCurrentConnection = false;
      this->parent->WrapUp();
      this->parent->ReleaseEverything();
      global << logger::red << "Ssl fail #: " << this->parent->NumConnectionsSoFar << logger::endL;
      global << commentsOnFailure.str() << logger::endL;
      return false;
    }
  }
  if (global.flagSSLIsAvailable && global.flagUsingSSLinCurrentConnection) {
    global << logger::green << "ssl success #: " << this->parent->NumConnectionsSoFar << ". " << logger::endL;
  }
  /////////////////////////////////////////////////////////////////////////
  this->numberOfReceivesCurrentConnection = 0;
  return  true;
}

bool WebWorker::FailReceiveReturnFalse() {
  bool sslWasOK = true;
  if (global.flagSSLIsAvailable) {
    sslWasOK = (this->error == TransportLayerSecurityOpenSSL::errors::errorWantRead);
  }
  if (this->numberOfReceivesCurrentConnection > 0 && sslWasOK) {
    global << logger::green << "Connection timed out after successfully receiving "
    << this->numberOfReceivesCurrentConnection << " times. " << logger::endL;
    this->lastResult = 0;
    return false;
  }
  global << logger::red << "Failed to receive all with error: " << this->error;
  this->lastResult = - 1;
  return false;
}

bool WebWorker::RunOnce() {
  this->lastResult = 0;
  StateMaintainerCurrentFolder preserveCurrentFolder;
  if (!this->ReceiveAll()) {
    return this->FailReceiveReturnFalse();
  }
  if (this->numberOfReceivesCurrentConnection > 0) {
    global.calculator().FreeMemory();
    global.calculator().GetElement().initialize();
    global.Comments.resetComments();
    global << logger::blue << "Created new calculator for connection: "
    << this->numberOfReceivesCurrentConnection << logger::endL;
  }
  if (this->messageHead.size() == 0) {
    global << logger::red << "Empty message head. " << logger::endL;
    if (this->numberOfReceivesCurrentConnection == 0) {
      this->lastResult = - 1;
    }
    return false;
  }
  this->lastResult = this->ServeClient();
  if (this->lastResult == - 1) {
    return false;
  }
  this->EnsureAllBytesSent();
  this->numberOfReceivesCurrentConnection ++;
  if (
    (!this->flagKeepAlive) ||
    global.flagRestartNeeded ||
    global.theResponse.TimedOut()
  ) {
    return false;
  }
  // The function call needs security audit.
  this->resetConnection();
  global << logger::blue << "Received " << this->numberOfReceivesCurrentConnection
  << " times on this connection, waiting for more. "
  << logger::endL;
  global.millisecondOffset += global.GetElapsedMilliseconds();
  this->parent->WorkerTimerPing(global.millisecondOffset);
  return true;
}

int WebWorker::Run() {
  MacroRegisterFunctionWithName("WebWorker::Run");
  if (!this->RunInitialize()) {
    this->lastResult = - 1;
    return this->lastResult;
  }
  while (this->RunOnce()) {
  }
  this->parent->WrapUp();
  this->parent->ReleaseEverything();
  return this->lastResult;
}

void WebServer::Release(int& theDescriptor) {
  MutexProcess::Release(theDescriptor);
}

extern int mainTest(List<std::string>& remainingArgs);

void WebServer::FigureOutOperatingSystem() {
  MacroRegisterFunctionWithName("WebServer::FigureOutOperatingSystem");
  if (global.OperatingSystem != "") {
    return;
  }
  List<std::string> supportedOSes;
  supportedOSes.addOnTop("Ubuntu");
  supportedOSes.addOnTop("CentOS");
  std::string commandOutput = global.externalCommandReturnOutput("cat /etc/*-release");
  if (
    commandOutput.find("ubuntu") != std::string::npos ||
    commandOutput.find("Ubuntu") != std::string::npos ||
    commandOutput.find("UBUNTU") != std::string::npos
  ) {
    global.OperatingSystem = "Ubuntu";
  } else if (commandOutput.find("CentOS") != std::string::npos) {
    global.OperatingSystem = "CentOS";
  } else {
    global.OperatingSystem = "";
  }
  if (global.OperatingSystem != "") {
    return;
  }
  global << logger::red << "Your Linux flavor is not currently supported. " << logger::endL;
  global << "We support the following Linux distros: "
  << logger::blue << supportedOSes.ToStringCommaDelimited() << logger::endL;
  global << "Please post a request for support of your Linux flavor on our bug tracker: " << logger::endL
  << logger::green << HtmlRoutines::gitRepository
  << logger::endL << "and we will add your Linux flavor to the list of supported distros. " << logger::endL;
}

void WebServer::CheckSystemInstallationOpenSSL() {
  MacroRegisterFunctionWithName("WebServer::CheckSystemInstallationOpenSSL");
  if (!global.flagDatabaseCompiled) {
    return;
  }
  if (global.configuration["openSSL"].theType != JSData::token::tokenUndefined) {
    return;
  }
  global.configuration["openSSL"] = "Attempted installation";
  global.ConfigurationStore();
  WebServer::FigureOutOperatingSystem();
  StateMaintainerCurrentFolder preserveFolder;
  std::string sslInstallCommand = "CentOS";
  if (global.OperatingSystem == "Ubuntu") {
    sslInstallCommand = "sudo apt-get install libssl-dev";
  } else if (global.OperatingSystem == "CentOS") {
    sslInstallCommand = "sudo yum install openssl-devel";
  } else {
    global << "You appear to be missing an openssl installation. "
    << "To support https, please install openssl. ";
    return;
  }
  if (sslInstallCommand != "") {
    global << "You appear to be missing an openssl installation. Let me try to install that for you. "
    << logger::green << "About to request sudo password for: " << sslInstallCommand << logger::endL;
    global << logger::red << "To refuse the command type CTRL+C. " << logger::endL;
    global.externalCommandNoOutput(sslInstallCommand, true);
    global.configuration["openSSL"] = "Attempted installation on Ubuntu";
  }
  global.ConfigurationStore();
}

void WebServer::CheckSystemInstallationMongoDB() {
  MacroRegisterFunctionWithName("WebServer::CheckSystemInstallationMongoDB");
  if (global.flagDatabaseCompiled) {
    return;
  }
  WebServer::FigureOutOperatingSystem();
  if (global.OperatingSystem == "") {
    return;
  }
  if (global.configuration["mongoDB"].theType != JSData::token::tokenUndefined) {
    return;
  }
  global.configuration["mongoDB"] = "Attempted installation";
  global.ConfigurationStore();

  StateMaintainerCurrentFolder preserveFolder;
  global << "You appear to be missing an mongoDB installation. Let me try to install that for you. "
  << logger::green << "Enter your the sudo password as prompted please. " << logger::endL;
  if (global.OperatingSystem == "Ubuntu") {
    global.externalCommandNoOutput("sudo apt-get install mongodb", true);
    global.configuration["mongoDB"] = "Attempted installation on Ubuntu";
  } else if (global.OperatingSystem == "CentOS") {
    global.externalCommandNoOutput("sudo yum install mongodb", true);
    global.configuration["mongoDB"] = "Attempted installation on CentOS";
  }
  global.ChDir(global.PhysicalPathProjectBase);
  global.externalCommandNoOutput("make clean", true);
  global << "Proceeding to rebuild the calculator. " << logger::red
  << "This is expected to take 10+ minutes. " << logger::endL;
  global.externalCommandNoOutput("make -j8", true);
  global.ConfigurationStore();
}

void WebServer::CheckMongoDBSetup() {
  MacroRegisterFunctionWithName("WebServer::CheckMongoDBSetup");
  if (global.flagDatabaseCompiled) {
    global << logger::green << "Compiled with mongo DB support. " << logger::endL;
  } else {
    global << logger::red << "Compiled without mongo DB support. " << logger::endL;
  }
  if (global.configuration["mongoDBSetup"].theType != JSData::token::tokenUndefined) {
    return;
  }
  global.configuration["mongoDBSetup"] = "Attempting";
  global.ConfigurationStore();
  WebServer::FigureOutOperatingSystem();
  global << logger::yellow << "Mongo setup file missing, proceeding with setup. " << logger::endL;
  global << logger::green << "Enter the sudo password as prompted please. " << logger::endL;
  //result << logger::yellow << "(Re)-starting mongo: " << logger::endL;
  StateMaintainerCurrentFolder maintainFolder;
  global.ChDir("../external-source");

  std::stringstream commandUnzipMongoC, commandUnzipLibbson;
  commandUnzipMongoC << "tar -xvzf mongo-c-driver-1.9.3.tar.gz";
  global << logger::green << commandUnzipMongoC.str() << logger::endL;
  global << global.externalCommandReturnOutput(commandUnzipMongoC.str());
  commandUnzipLibbson << "tar -xvzf libbson-1.9.3.tar.gz";
  global << logger::green << commandUnzipLibbson.str() << logger::endL;
  global << global.externalCommandReturnOutput(commandUnzipLibbson.str());

  global.ChDir("./mongo-c-driver-1.9.3");
  global.externalCommandNoOutput("./configure", true);
  global.externalCommandNoOutput("make -j8", true);
  global << "Need sudo access for command: "
  << logger::red << "sudo make install" << logger::endL;
  global.externalCommandNoOutput("sudo make install", true);
  global.ChDir("../libbson-1.9.3");
  global.externalCommandNoOutput("./configure", true);
  global.externalCommandNoOutput("make -j8", true);
  global << "Need sudo access for command: "
  << logger::red << "sudo make install" << logger::endL;
  global.externalCommandNoOutput("sudo make install", true);
  global.ChDir("../../bin");
  global << "Need sudo access for command to configure linker to use local usr/local/lib path (needed by mongo): "
  << logger::red << "sudo cp ../external-source/usr_local_lib_for_mongo.conf /etc/ld.so.conf.d/" << logger::endL;
  global.externalCommandNoOutput("sudo cp ../external-source/usr_local_lib_for_mongo.conf /etc/ld.so.conf.d/", true);
  global << "Need sudo access for command: " << logger::red << "sudo ldconfig" << logger::endL;
  global.externalCommandNoOutput("sudo ldconfig", true);
  global.configuration["mongoDBSetup"] = "Setup complete";
  global.ConfigurationStore();
}

void WebServer::CheckFreecalcSetup() {
  MacroRegisterFunctionWithName("WebServer::CheckFreecalcSetup");
  if (global.configuration["freecalcSetup"].theType != JSData::token::tokenUndefined) {
    return;
  }
  global.configuration["freecalcSetup"] = "Setup started";
  global.ConfigurationStore();
  WebServer::FigureOutOperatingSystem();
  global << logger::yellow << "Freelcalc setup file missing, proceeding to set it up. " << logger::endL;
  StateMaintainerCurrentFolder preserveFolder;
  std::string startingDir = FileOperations::GetCurrentFolder();
  global.ChDir(global.PhysicalPathProjectBase + "../");
  global << logger::green << "Current folder: " << FileOperations::GetCurrentFolder() << logger::endL;
  global << logger::green << "git clone https://github.com/tmilev/freecalc.git" << logger::endL;
  global.externalCommandNoOutput("git clone https://github.com/tmilev/freecalc.git", true);
  global.configuration["freecalcSetup"] = "Setup complete";
  global.ConfigurationStore();
}

void WebServer::CheckUnzipInstall() {
  WebServer::FigureOutOperatingSystem();
  if (global.OperatingSystem == "Ubuntu") {
    global.externalCommandNoOutput("sudo apt-get install unzip", true);
  } else if (global.OperatingSystem == "CentOS") {
    global.externalCommandNoOutput("sudo yum install unzip", true);
  }

}

void WebServer::CheckMathJaxSetup() {
  MacroRegisterFunctionWithName("WebServer::CheckMathJaxSetup");
  std::string checkForMathJaxAndAutoInstall = "Check for mathjax on every boot.";
  if (
    global.configuration[Configuration::mathJaxSetup].theString !=
    checkForMathJaxAndAutoInstall
  ) {
    return;
  }
  global.ConfigurationStore();
  if ((false)) {
    WebServer::CheckUnzipInstall();
  }
  std::string mathjaxBase;
  std::string mathjaxZipSource;
  std::string publicHtmlFolder;
  std::stringstream commentsOnFailure;
  if (!FileOperations::GetPhysicalFileNameFromVirtual(
    Configuration::mathJaxLatest,
    mathjaxBase,
    false,
    false,
    &commentsOnFailure
  )) {
    global << logger::red << "Failed to compute mathjax folder. "
    << logger::endL << commentsOnFailure.str() << logger::endL;
    return;
  }
  std::string toDisableMathJaxChecks = "change: "
  "file: configuration/configuration.json "
  "key: " + Configuration::mathJaxSetup +
  " to any string different from its current value.";

  if (FileOperations::FileExistsUnsecure(mathjaxBase)) {
    // Mathjax folder is there.
    // We assume it is correctly installed.
    global << logger::yellow
    << "MathJax found. To disable futher checks for mathjax: "
    << toDisableMathJaxChecks << logger::endL;
    return;
  }
  global << logger::red << "MathJax not found. "
  << logger::green << "Attempting to auto-install it for you. "
  << logger::endL
  << "To disable mathjax checks/auto-installation: "
  << toDisableMathJaxChecks << logger::endL;
  if (!FileOperations::GetPhysicalFileNameFromVirtual(
    Configuration::publicHTML, publicHtmlFolder, false, false, & commentsOnFailure
  )) {
    global << logger::red << "Failed to compute public html folder. "
    << logger::endL << commentsOnFailure.str() << logger::endL;
    return;
  }
  if (!FileOperations::GetPhysicalFileNameFromVirtual(
    Configuration::HTMLCommon + "MathJax-2.7-latest.zip",
    mathjaxZipSource,
    false,
    false,
    &commentsOnFailure
  )) {
    global << logger::red << "Failed to compute mathjax zip source folder. "
    << logger::endL << commentsOnFailure.str() << logger::endL;
    return;
  }
  global << "Proceeding to unzip MathJax. ";
  global << "Current folder: " << FileOperations::GetCurrentFolder() << logger::endL;
  std::stringstream unzipCommand, moveCommand;
  unzipCommand << "unzip " << mathjaxZipSource << " -d " << publicHtmlFolder;
  moveCommand << "mv " << publicHtmlFolder << "MathJax-2.7.2 " << publicHtmlFolder << "MathJax-2.7-latest";
  global.externalCommandNoOutput(unzipCommand.str(), true);
  global.externalCommandNoOutput(moveCommand.str(), true);
}

bool WebServer::AnalyzeMainArgumentsTimeString(const std::string& timeLimitString) {
  if (timeLimitString == "") {
    return false;
  }
  Rational timeLimit;
  if (!timeLimit.AssignStringFailureAllowed(timeLimitString)) {
    return false;
  }
  int timeLimitInt = 0;
  if (timeLimit.IsIntegerFittingInInt(&timeLimitInt)) {
    global.millisecondsMaxComputation = timeLimitInt;
    if (global.millisecondsMaxComputation <= 0) {
      global.millisecondsMaxComputation = 0;
    } else {
      global.millisecondsMaxComputation *= 1000;
    }
  }
  return  true;
}

void WebServer::InitializeBuildFlags() {
  ////////////////////////////////////////////////////
  global.flagRunningConsoleRegular = false;
  global.flagRunningConsoleTest = false;
  #ifdef MACRO_use_open_ssl
  global.flagSSLIsAvailable = true;
  #endif
  #ifdef MACRO_use_MongoDB
  global.flagDatabaseCompiled = true;
  #endif
  global.flagRunningBuiltInWebServer = false;
  ////////////////////////////////////////////////////
}

std::string MainFlags::server = "server";
std::string MainFlags::daemon = "daemon";
std::string MainFlags::help = "help";
std::string MainFlags::pathExecutable = "path_executable";
std::string MainFlags::configurationFile = "configuration_file";
std::string MainFlags::test = "test";

class ArgumentAnalyzer {
public:
  int currentIndex;
  HashedList<std::string, MathRoutines::HashString> commandLineConfigurations;
  bool processOneArgument();
  bool setServer();
  bool setTest();
  bool setPathExecutable();
  bool setConfigurationFile();
  bool processCommandLineConfigurations(std::string& input);
};

bool ArgumentAnalyzer::setServer() {
  std::string timeLimitString;
  global.flagRunningBuiltInWebServer = true;
  global.flagRunningConsoleRegular = false;
  global.flagRunningConsoleTest = false;
  if (this->currentIndex + 1 < global.programArguments.size) {
    timeLimitString = global.programArguments[this->currentIndex + 1];
    if (WebServer::AnalyzeMainArgumentsTimeString(timeLimitString)) {
      this->currentIndex ++;
    }
  }
  return true;
}

bool ArgumentAnalyzer::setTest() {
  global.flagRunningConsoleTest = true;
  global.flagRunningConsoleRegular = false;
  global.flagRunningBuiltInWebServer = false;
  global.configurationFileName = "/configuration/configuration_for_testing.json";
  return true;
}

bool ArgumentAnalyzer::setPathExecutable() {
  if (this->currentIndex + 1 >= global.programArguments.size) {
    global << logger::red
    << "The executable path is missing. " << logger::endL;
    return false;
  }
  this->currentIndex ++;
  global.PathExecutableUserInputOrDeduced = global.programArguments[this->currentIndex];
  return true;
}

bool ArgumentAnalyzer::setConfigurationFile() {
  if (this->currentIndex + 1 >= global.programArguments.size) {
    global << logger::red
    << "The configuration filename is missing. " << logger::endL;
    return false;
  }
  this->currentIndex ++;
  global.configurationFileName = global.programArguments[this->currentIndex];
  return true;
}

bool ArgumentAnalyzer::processCommandLineConfigurations(std::string& input) {
  if (!this->commandLineConfigurations.Contains(input)) {
    return true;
  }
  if (this->currentIndex + 1 >= global.programArguments.size) {
    global << logger::red
    << "The configuration: " << input << " is missing. " << logger::endL;
    return false;
  }
  this->currentIndex ++;
  global.configurationCommandLine[input] = global.programArguments[this->currentIndex];
  return true;
}

bool ArgumentAnalyzer::processOneArgument() {
  if (this->currentIndex >= global.programArguments.size) {
    return false;
  }
  std::string current = StringRoutines::StringTrimWhiteSpace(global.programArguments[this->currentIndex]);
  if (current == MainFlags::server) {
    return this->setServer();
  }
  if (current == MainFlags::help && this->currentIndex == 1) {
    global.flagRunningConsoleHelp = true;
    return true;
  }
  if (current == MainFlags::daemon && this->currentIndex == 1) {
    global.flagDaemonMonitor = true;
    return true;
  }
  if (current == MainFlags::test) {
    return this->setTest();
  }
  if (current == MainFlags::pathExecutable) {
    return this->setPathExecutable();
  }
  if (current == MainFlags::configurationFile) {
    return this->setConfigurationFile();
  }
  return this->processCommandLineConfigurations(current);
}

void WebServer::AnalyzeMainArguments(int argC, char **argv) {
  MacroRegisterFunctionWithName("WebServer::AnalyzeMainArguments");
  global.configurationCommandLine.reset(JSData::token::tokenObject);
  if (argC < 0) {
    argC = 0;
  }
  global.programArguments.setSize(argC);
  for (int i = 0; i < argC; i ++) {
    global.programArguments[i] = argv[i];
    //std::cout << "Argument " << i + 1 << ": " << global.programArguments[i] << "\n";
  }
  global.flagRunningConsoleRegular = true;
  if (argC == 0) {
    return;
  }
  global.PathExecutableUserInputOrDeduced = global.programArguments[0];
  if (argC < 2) {
    return;
  }
  ArgumentAnalyzer arguments;
  arguments.currentIndex = 1;
  arguments.commandLineConfigurations.addOnTop(List<std::string> ({
    Configuration::portHTTP,
    Configuration::portHTTPSOpenSSL
  }));
  for (; arguments.processOneArgument(); arguments.currentIndex ++) {
  }
}

void WebServer::InitializeMainHashes() {
  StateMaintainerCurrentFolder preserveCurrentFolder;
  if (global.buildVersionSimple == "") {
    global.buildVersionSimple =
    StringRoutines::StringTrimWhiteSpace(global.externalCommandReturnOutput("git rev-list --count HEAD"));
  }
  global.buildVersionSimple = StringRoutines::StringTrimWhiteSpace(global.buildVersionSimple);
  for (unsigned i = 0; i < global.buildVersionSimple.size(); i ++) {
    if (MathRoutines::isALatinLetter(global.buildVersionSimple[i])) {
      global.buildVersionSimple = "?";
      break;
    }
  }
  global.buildHeadHashWithServerTime = StringRoutines::StringTrimWhiteSpace(
    global.externalCommandReturnOutput("git rev-parse HEAD")
  );
  for (unsigned i = 0; i < global.buildHeadHashWithServerTime.size(); i ++) {
    if (!MathRoutines::IsAHexDigit(global.buildHeadHashWithServerTime[i])) {
      global.buildHeadHashWithServerTime = "x";
      break;
    }
  }
  std::stringstream buildHeadHashWithTimeStream;
  buildHeadHashWithTimeStream << global.buildHeadHashWithServerTime
  << global.GetGlobalTimeInSeconds();
  global.buildHeadHashWithServerTime = buildHeadHashWithTimeStream.str();

  FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash().AddOnTopNoRepetitionMustBeNewCrashIfNot(
    WebAPI::request::calculatorHTML
  );
  WebAPI::request::onePageJSWithHash = FileOperations::GetVirtualNameWithHash(WebAPI::request::onePageJS);
}

void WebServer::InitializeMainRequests() {
  MacroRegisterFunctionWithName("WebServer::InitializeMainRequests");
  this->requestsNotNeedingLogin.addOnTop("about");
  this->requestsNotNeedingLogin.addOnTop("signUp");
  this->requestsNotNeedingLogin.addOnTop("signUpPage");
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::forgotLogin);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::compute);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::calculatorPage);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::examplesJSON);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::indicator);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::pause);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::exerciseNoLogin);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::exerciseJSON);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::templateNoLogin);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::templateJSONNoLogin);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::topicListJSONNoLogin);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::problemGiveUpNoLogin);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::problemSolutionNoLogin);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::homeworkFromSource);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::slides::source);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::slides::pdfFromSource);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::homeworkSource);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::submitExerciseNoLogin);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::submitExercisePreviewNoLogin);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::selectCourseJSON);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::app);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::appNoCache);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::userInfoJSON);
  this->requestsNotNeedingLogin.addOnTop(WebAPI::request::serverStatusJSON);
}

void WebServer::InitializeMainAddresses() {
  MacroRegisterFunctionWithName("WebServer::InitializeMainAddresses");
  this->addressStartsNotNeedingLogin.addOnTop("favicon.ico");
  this->addressStartsNotNeedingLogin.addOnTop("/favicon.ico");
  this->addressStartsNotNeedingLogin.addOnTop("/html-common/");
  this->addressStartsNotNeedingLogin.addOnTop("/calculator-html/");
  this->addressStartsNotNeedingLogin.addOnTop("/src/");
  this->addressStartsNotNeedingLogin.addOnTop("/output/");
  this->addressStartsNotNeedingLogin.addOnTop("/css/");
  this->addressStartsNotNeedingLogin.addOnTop("/javascriptlibs/");
  this->addressStartsNotNeedingLogin.addOnTop("/MathJax-2.7-latest/");
  this->addressStartsNotNeedingLogin.addOnTop("/login");
  this->addressStartsNotNeedingLogin.addOnTop("/" + WebAPI::app);
  this->addressStartsNotNeedingLogin.addOnTop("/" + WebAPI::appNoCache);
  this->addressStartsNotNeedingLogin.addOnTop(WebAPI::request::onePageJS);

  this->addressStartsInterpretedAsCalculatorRequest.addOnTop("/" + WebAPI::app);
  this->addressStartsInterpretedAsCalculatorRequest.addOnTop(WebAPI::app);
  this->addressStartsInterpretedAsCalculatorRequest.addOnTop("/" + WebAPI::appNoCache);
  this->addressStartsInterpretedAsCalculatorRequest.addOnTop(WebAPI::appNoCache);
  this->addressStartsInterpretedAsCalculatorRequest.addOnTop(WebAPI::request::onePageJS);
  this->addressStartsInterpretedAsCalculatorRequest.addOnTop(WebAPI::request::onePageJSWithHash);

  // FORBIDDEN:
  // this->addressStartsNotNeedingLogin.addOnTop("logout");
  // Logging someone out definitely
  // requires authentication imagine someone
  // asking for logouts on your account once every second:
  // this would be fatal as proper logout resets
  // the authentication tokens.
  this->addressStartsNotNeedingLogin.addListOnTop(
    FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash()
  );

  this->addressStartsSentWithCacheMaxAge.addOnTop("/MathJax-2.7-latest/");
  this->addressStartsSentWithCacheMaxAge.addOnTop("/html-common/");
  for (int i = 0; i < FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash().size; i ++) {
    this->addressStartsSentWithCacheMaxAge.addOnTop(
      FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash()[i] +
      global.buildHeadHashWithServerTime
    );
  }

}

void WebServer::InitializeMainFoldersInstructorSpecific() {
  MacroRegisterFunctionWithName("WebServer::InitializeMainFoldersInstructorSpecific");
  List<std::string> incoming = List<std::string>({
    Configuration::topicLists      ,
    Configuration::courseTemplates ,
    Configuration::coursesAvailable,
    Configuration::problemsFolder  ,
  });
  for (int i = 0; i < incoming.size; i ++) {
    FileOperations::FolderStartsToWhichWeAppendInstructorUsernameSlash().addOnTop(incoming[i]);
    FileOperations::FolderStartsToWhichWeAppendInstructorUsernameSlash().addOnTop("/" + incoming[i]);
  }
}

void WebServer::InitializeMainAll() {
  ParallelComputing::cgiLimitRAMuseNumPointersInList = 4000000000;
  this->InitializeMainHashes();
  FileOperations::InitializeFoldersULTRASensitive();
  FileOperations::InitializeFoldersSensitive();
  FileOperations::InitializeFoldersNonSensitive();
  FileOperations::CheckFolderLinks();
  this->InitializeMainFoldersInstructorSpecific();
  this->InitializeMainRequests();
  this->InitializeMainAddresses();
  this->initializeMainMIMETypes();
}

extern int mainTest(List<std::string>& remainingArgs);

void WebServer::TurnProcessMonitoringOn() {
  MacroRegisterFunctionWithName("WebServer::TurnProcessMonitoringOn");
  global.theResponse.flagBanProcessMonitorinG = false;
  global.configuration[Configuration::processMonitoringBanned] = false;
  global
  << logger::yellow << "Process monitoring IS ON, reply in: " << logger::green
  << global.millisecondsReplyAfterComputation << " ms." << logger::endL;
}

void WebServer::TurnProcessMonitoringOff() {
  MacroRegisterFunctionWithName("WebServer::TurnProcessMonitoringOff");
  global
  << logger::green << "************************" << logger::endL
  << logger::red << "Process monitoring is now off. " << logger::endL
  << logger::green << "************************" << logger::endL;
  global.theResponse.flagBanProcessMonitorinG = true;
  global.millisecondsReplyAfterComputation = 0;
  global.configuration[Configuration::processMonitoringBanned] = true;
}

bool GlobalVariables::ConfigurationLoad() {
  MacroRegisterFunctionWithName("GlobalVariables::ConfigurationLoad");
  std::stringstream out;
  if (this->configurationFileName == "") {
    this->configurationFileName = "/configuration/configuration.json";
  }
  if (!FileOperations::LoadFileToStringVirtual(
    this->configurationFileName, global.configurationFileContent, true, &out
  )) {
    global << logger::yellow << "Failed to read configuration file. " << out.str() << logger::endL;
    std::string computedPhysicalFileName;
    if (FileOperations::GetPhysicalFileNameFromVirtual(
      this->configurationFileName, computedPhysicalFileName, true, false, nullptr
    )) {
      global << logger::yellow << "Computed configuration file name: "
      << logger::blue << computedPhysicalFileName << logger::endL;
    }
    return false;
  }
  if (!global.configuration.readstring(
    this->configurationFileContent, &out
  )) {
    global << logger::red << "Failed to read configuration. " << out.str() << logger::endL;
    return false;
  }
  return true;
}

bool GlobalVariables::ConfigurationStore() {
  MacroRegisterFunctionWithName("GlobalVariables::ConfigurationStore");
  if (global.configurationCommandLine.objects.size() > 0) {
    global << logger::yellow << "Command-line overrides in the present configuration: configuration file not stored. ";
    return true;
  }
  std::string correctedConfiguration = global.configuration.toString(&JSData::PrintOptions::NewLine());
  if (correctedConfiguration == global.configurationFileContent) {
    return true;
  }
  std::fstream configurationFile;
  if (!FileOperations::OpenFileCreateIfNotPresentVirtual(
    configurationFile, this->configurationFileName, false, true, false, true
  )) {
    global << logger::red << "Could not open file: " << this->configurationFileName << logger::endL;
    std::string configFileNamePhysical;
    if (FileOperations::GetPhysicalFileNameFromVirtual(
      this->configurationFileName, configFileNamePhysical, true, false, nullptr
    )) {
      global << logger::red << "Physical file name configuration: "
      << logger::blue << configFileNamePhysical << logger::endL;
      global << logger::red << "Server base: " << logger::blue
      << global.PhysicalPathProjectBase << logger::endL;
    }
    return false;
  }
  global << logger::blue
  << "Configuration has been reformatted/recomputed, overwriting previous configuration file." << logger::endL;
  configurationFile << correctedConfiguration;
  return true;
}

void GlobalVariables::ConfigurationProcess() {
  MacroRegisterFunctionWithName("GlobalVariables::ConfigurationProcess");
  for (int i = 0; i < global.configurationCommandLine.objects.size(); i ++) {
    global.configuration.SetKeyValue(
      global.configurationCommandLine.objects.theKeys[i],
      global.configurationCommandLine.objects.theValues[i]
    );
  }
  global.flagServerDetailedLog = global.configuration[
    Configuration::serverDetailedLog
  ].isTrueRepresentationInJSON();
  global.flagDisableDatabaseLogEveryoneAsAdmin = global.configuration[
    Configuration::disableDatabaseLogEveryoneAsAdmin
  ].isTrueRepresentationInJSON();
  if (global.flagDisableDatabaseLogEveryoneAsAdmin) {
    global.flagDatabaseCompiled = false;
    global
    << logger::purple << "************************" << logger::endL
    << logger::red << "WARNING: database disabled, " << logger::green
    << "no database operations permitted." << logger::endL
    << logger::red
    << "Everyone gets logged-in as administrator. " << logger::endL
    << logger::purple << "************************" << logger::endL;
  }
  if (global.configuration[Configuration::serverAutoMonitor].isTrueRepresentationInJSON()) {
    global.flagLocalhostConnectionMonitor = true;
  } else {
    global.flagLocalhostConnectionMonitor = false;
    if (!global.flagRunningConsoleRegular && !global.flagRunningConsoleTest) {
      global
      << logger::red << "WARNING: server auto-monitoring is off. " << logger::endL;
    }
  }
  if (global.flagServerDetailedLog) {
    global
    << logger::purple << "************************" << logger::endL
    << logger::purple << "************************" << logger::endL
    << logger::purple << "************************" << logger::endL
    << logger::red << "WARNING: DETAILED server logging is on. " << logger::endL
    << "This is strictly for development purposes, "
    << "please do not deploy on live systems. " << logger::endL
    << "To turn off/on server logging: set serverDetailedLog "
    << "to false in the file configuration/configuration.json." << logger::endL
    << logger::purple << "************************" << logger::endL
    << logger::purple << "************************" << logger::endL
    << logger::purple << "************************" << logger::endL;
  }
  global.flagCachingInternalFilesOn = !global.configuration[
    "serverRAMCachingOff"
  ].isTrueRepresentationInJSON();
  if (!global.flagCachingInternalFilesOn && global.flagRunningBuiltInWebServer) {
    global
    << logger::purple << "************************" << logger::endL
    << logger::red << "WARNING: caching files is off. " << logger::endL
    << "This is for development purposes only, "
    << "please do not deploy on live systems. " << logger::endL
    << "To turn off/on modify variable serverRAMCachingOff "
    << "in configuration/configuration.json." << logger::endL
    << logger::purple << "************************" << logger::endL;
  }
  global.flagRunServerOnEmptyCommandLine = global.configuration[
    "runServerOnEmptyCommandLine"
  ].isTrueRepresentationInJSON();
  if (
    global.flagRunningConsoleRegular &&
    global.programArguments.size == 1 &&
    global.flagRunServerOnEmptyCommandLine
  ) {
    global << logger::green
    << "runServerOnEmptyCommandLine is set to true => Starting server with default configuration. "
    << logger::endL;
    global.flagRunningBuiltInWebServer = true;
    global.flagRunningConsoleRegular = false;
    global.millisecondsMaxComputation = 30000; // 30 seconds, default
  }
  int reader = 0;
  if (global.configuration[
    Configuration::millisecondsReplyAfterComputation
  ].isIntegerFittingInInt(&reader)) {
    global.millisecondsReplyAfterComputation = reader;
  } else {
    global.configuration[
      Configuration::millisecondsReplyAfterComputation
    ] = global.millisecondsReplyAfterComputation;
  }
  if (
    global.configuration[
      Configuration::processMonitoringBanned
    ].isTrueRepresentationInJSON()
  ) {
    global << logger::blue << "Process monitoring banned from configuration.json. " << logger::endL;
    WebServer::TurnProcessMonitoringOff();
  } else {
    WebServer::TurnProcessMonitoringOn();
  }
  if (global.configuration[Configuration::monitorPingTime].isIntegerFittingInInt(
    &global.server().WebServerPingIntervalInSeconds
  )) {
    if (global.server().WebServerPingIntervalInSeconds < 0) {
      global.server().WebServerPingIntervalInSeconds = 10;
    }
  } else {
    global.configuration[Configuration::monitorPingTime] = global.server().WebServerPingIntervalInSeconds;
  }
  if (global.configuration[Configuration::gitRepository].theType == JSData::token::tokenString) {
    HtmlRoutines::gitRepository = global.configuration[Configuration::gitRepository].theString;
  } else {
    global.configuration[Configuration::gitRepository] = HtmlRoutines::gitRepository;
  }
  if (global.configuration[Configuration::portHTTP].theString == "") {
    global.configuration[Configuration::portHTTP] = "8155";
  }
  if (global.configuration[Configuration::portHTTPSOpenSSL].theString == "") {
    global.configuration[Configuration::portHTTPSOpenSSL] = "8166";
  }
  if (global.configuration[Configuration::portHTTPSBuiltIn].theString == "") {
    global.configuration[Configuration::portHTTPSBuiltIn] = "8177";
  }
  List<List<std::string> > folderVirtualLinksDefault = FileOperations::InitializeFolderVirtualLinksDefaults();
  for (int i = 0; i < folderVirtualLinksDefault.size; i ++) {
    std::string key = folderVirtualLinksDefault[i][0];
    std::string value = folderVirtualLinksDefault[i][1];
    if (
      global.configuration[key].theString == ""
    ) {
      global.configuration[key] = value;
    }
  }
}

void WebServer::CheckInstallation() {
  global.server().CheckSystemInstallationOpenSSL();
  global.server().CheckSystemInstallationMongoDB();
  global.server().CheckMongoDBSetup();
  global.server().CheckMathJaxSetup();
  global.server().CheckFreecalcSetup();
}

int WebServer::main(int argc, char **argv) {
  global.init();
  global.InitThreadsExecutableStart();
  MacroRegisterFunctionWithName("main");
  try {
    // Initializations basic (timer, ...).
    InitializeGlobalObjects();
    // Initializations of build flags.
    global.server().WebServer::InitializeBuildFlags();
    // Process executable arguments.
    // May set the value of
    // global.PathProjectBaseUserInputOrDeduced
    global.server().AnalyzeMainArguments(argc, argv);
    if (global.flagRunningConsoleHelp) {
      return WebServer::mainConsoleHelp();
    }
    // Initializes server base path
    // using global.PathProjectBaseUserInputOrDeduced.
    global.initDefaultFolderAndFileNames();
    // Ensure the server path coincides with the current
    // directory:
    global.ChDir(global.PhysicalPathProjectBase);
    // Initializes folder locations needed by logging facilities.
    FileOperations::InitializeFoldersSensitive();

    global << logger::green << "Project base folder: "
    << logger::blue << global.PhysicalPathProjectBase
    << logger::endL;
    global << logger::green << "Current folder: "
    << logger::blue << FileOperations::GetCurrentFolder()
    << logger::endL;
    // Compute configuration file location.
    // Load the configuration file.
    global.ConfigurationLoad();
    // Compute various flags and settings from the desired configuration.
    // Correct bad configuration settings if any.
    global.ConfigurationProcess();
    // Store back the config file if it changed.
    global.ConfigurationStore();
    global << "Computation timeout: " << logger::red
    << global.millisecondsMaxComputation << " ms." << logger::endL;

    if (!Database::get().initializeServer()) {
      global.fatal << "Failed to initialize database. " << global.fatal;
    }
    if (global.millisecondsMaxComputation > 0) {
      global.millisecondsNoPingBeforeChildIsPresumedDead =
      global.millisecondsMaxComputation + 20000; // + 20 seconds
    } else {
      global.millisecondsNoPingBeforeChildIsPresumedDead = - 1;
    }
    // Uses the configuration file.
    // Calls again global.server().InitializeMainFoldersSensitive();
    global.server().InitializeMainAll();
    global.server().CheckMathJaxSetup();
    bool mathJaxPresent = FileOperations::FileExistsVirtual("/MathJax-2.7-latest/", false);
    if (!mathJaxPresent && global.flagRunningBuiltInWebServer) {
      global << logger::red << "MathJax not available. " << logger::endL;
    }
    if (global.flagDaemonMonitor) {
      return global.server().Daemon();
    } else if (global.flagRunningBuiltInWebServer) {
      return global.server().Run();
    } else if (global.flagRunningConsoleTest) {
      return mainTest(global.programArguments);
    } else {
      return WebServer::mainCommandLine();
    }
  } catch (...) {
    global.fatal << "Exception caught: something very wrong has happened. " << global.fatal;
  }
  global.fatal << "This point of code is not supposed to be reachable. " << global.fatal;
  return - 1;
}

int WebServer::mainConsoleHelp() {
  std::cout << "All flags are optional.\n";
  std::cout << "Run the server with:\n";
  std::cout << "calculator " << MainFlags::server << " [[max_seconds_per_computation]] " << MainFlags::configurationFile << " [[configuration_file_name]]"
  << MainFlags::pathExecutable << " [[custom_path_to_run_from]]" << "\n";
  std::cout << "Examples:\n";
  std::cout << "Run as server with defaults:\n";
  std::cout << "./calculator\n";
  std::cout << "Run as server with custom computation timeout:\n";
  std::cout << "./calculator server 200\n";
  std::cout << "Run as server with custom timeout, custom base path and custom configuration file:\n";
  std::cout << "./calculator " << MainFlags::server << " 50 " << MainFlags::configurationFile << " \"./configuration/configuration.json\" "
  << MainFlags::pathExecutable  << " \"./\"" << "\n";
  std::cout << "Run unit tests with:\n";
  std::cout << "calculator " << MainFlags::test << "\n";
  return 0;
}

int WebServer::mainCommandLine() {
  MacroRegisterFunctionWithName("WebServer::mainCommandLine");
  Calculator& theCalculator = global.calculator().GetElement();
  theCalculator.initialize();
  if (global.programArguments.size > 1) {
    for (int i = 1; i < global.programArguments.size; i ++) {
      theCalculator.inputString += global.programArguments[i];
      if (i != global.programArguments.size - 1) {
        theCalculator.inputString += " ";
      }
    }
  } else {
    global << "Input: " << logger::yellow;
    std::cin >> theCalculator.inputString;
  }
  theCalculator.flagUseHtml = false;
  theCalculator.Evaluate(theCalculator.inputString);
  std::fstream outputFile;
  std::string outputFileName;
  if (!FileOperations::GetPhysicalFileNameFromVirtual(
    "output/outputFileCommandLine.html", outputFileName, false, false, nullptr
  )) {
    outputFileName = "Failed to extract output file from output/outputFileCommandLine.html";
  }
  FileOperations::OpenFileCreateIfNotPresentVirtual(
    outputFile, "output/outputFileCommandLine.html", false, true, false
  );
  global << theCalculator.outputString;
  outputFile << theCalculator.outputString;
  global << "\nTotal running time: " << logger::blue << global.GetElapsedMilliseconds() << " ms. "
  << logger::endL
  << "Output written in: " << logger::green << outputFileName << logger::endL << "\n";
  return 0;
}

std::string WebServer::GetEnvironment(const std::string& envVarName) {
  char* queryStringPtr = getenv(envVarName.c_str());
  if (queryStringPtr == nullptr) {
    return "";
  }
  return queryStringPtr;
}

void WebWorker::PrepareFullMessageHeaderAndFooter() {
  MacroRegisterFunctionWithName("WebWorker::PrepareFullMessageHeaderAndFooter");
  this->remainingBytesToSenD.setSize(0);
  this->remainingBytesToSenD.setExpectedSize(this->remainingBodyToSend.size + this->remainingHeaderToSend.size + 30);
  if (this->remainingHeaderToSend.size == 0) {
    if (this->requestTypE != this->requestHead) {
      this->remainingBytesToSenD = this->remainingBodyToSend;
    }
    this->remainingBodyToSend.setSize(0);
    return;
  }
  this->remainingBytesToSenD = this->remainingHeaderToSend;
  this->remainingHeaderToSend.setSize(0);
  std::stringstream contentLengthStream;
  if (this->flagDoAddContentLength) {
    contentLengthStream << "Content-length: " << this->remainingBodyToSend.size << "\r\n";
  }
  contentLengthStream << "\r\n";
  std::string contentLengthString = contentLengthStream.str();
  for (unsigned i = 0; i < contentLengthString.size(); i ++) {
    this->remainingBytesToSenD.addOnTop(contentLengthString[i]);
  }
  if (this->requestTypE != this->requestHead) {
    this->remainingBytesToSenD.addListOnTop(this->remainingBodyToSend);
  }
  this->remainingBodyToSend.setSize(0);
}

bool WebWorker::EnsureAllBytesSent() {
  if (
    this->remainingBytesToSenD.size == 0 &&
    this->remainingBodyToSend.size == 0 &&
    this->remainingHeaderToSend.size == 0
  ) {
    return true;
  }
  global << logger::red << "This should not happen: not all bytes have been processed. " << logger::endL;
  global << logger::red << "Type: " << logger::blue << global.requestType
  << ", address: " << this->addressComputed << logger::endL;
  global << logger::red << "Remaining header: " << logger::red
  << this->remainingHeaderToSend.ToStringCommaDelimited() << logger::endL;
  global << logger::red << "Remaining body size: " << logger::blue
  << this->remainingBodyToSend.size << logger::endL;
  global << logger::red << "Remaining bytes: " << logger::blue
  << this->remainingBytesToSenD.size << logger::endL;
  this->SendPending();
  return false;
}

void WebWorker::SendPending() {
  MacroRegisterFunctionWithName("WebWorker::SendPending");
  this->PrepareFullMessageHeaderAndFooter();
  this->SendAllBytesNoHeaderS();
}

void WebWorker::QueueBytesForSendingNoHeadeR(const List<char>& bytesToSend, bool MustSendAll) {
  MacroRegisterFunctionWithName("WebWorker::QueueBytesForSendingNoHeadeR");
  (void) MustSendAll;
  this->remainingBytesToSenD.addListOnTop(bytesToSend);
//  if (this->remainingBytesToSend.size >= 1024 * 512 || MustSendAll)
//    this->SendAllBytes();
}

bool WebWorker::WriteToBody(const std::string& stringToSend) {
  MacroRegisterFunctionWithName("WebWorker::WriteToBody");
  int oldSize = this->remainingBodyToSend.size;
  this->remainingBodyToSend.setSize(
    this->remainingBodyToSend.size + static_cast<signed>(stringToSend.size())
  );
  for (unsigned i = 0; i < stringToSend.size(); i ++) {
    this->remainingBodyToSend[static_cast<signed>(i) + oldSize] = stringToSend[i];
  }
  return true;
}

void WebWorker::QueueStringForSendingNoHeadeR(const std::string& stringToSend, bool MustSendAll) {
  MacroRegisterFunctionWithName("WebWorker::QueueStringForSendingNoHeadeR");
  (void) MustSendAll;
  int oldSize = this->remainingBytesToSenD.size;
  this->remainingBytesToSenD.setSize(
    this->remainingBytesToSenD.size + static_cast<signed>(stringToSend.size())
  );
  for (unsigned i = 0; i < stringToSend.size(); i ++) {
    this->remainingBytesToSenD[static_cast<signed>(i) + oldSize] = stringToSend[i];
  }
}
