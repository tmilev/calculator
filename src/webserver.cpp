//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "webserver.h"
#include "vpfHeader3Calculator0_Interface.h"
#include "vpfHeader7DatabaseInterface.h"
#include "vpfHeader8HtmlInterpretationInterface.h"
#include "vpfHeader8HtmlSnippets.h"
#include "string_constants.h"
#include "vpfHeader5Crypto.h"

ProjectInformationInstance projectInfoInstanceWebServer(__FILE__, "Web server implementation.");
WebServer theWebServer;

#include <sys/wait.h>//<-waitpid f-n here
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <arpa/inet.h> // <- inet_ntop declared here (ntop = network to presentation)
#include <unistd.h>
#include <sys/stat.h>//<-for file statistics
#include <fcntl.h>//<-setting flags of file descriptors
#include "vpfHeader7DatabaseInterface_Mongodb.h"

struct SignalsInfrastructure {
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
};

SignalsInfrastructure::SignalsInfrastructure() {
  this->flagSignalsAreBlocked = false;
  this->flagSignalsAreStored = false;
  this->flagInitialized = false;
}

SignalsInfrastructure theSignals;
//sigset_t SignalSetToBlockWhileHandlingSIGCHLD;

//This class locks/unlocks all signals within its scope
struct SignalLock {
  SignalLock() {
    theSignals.blockSignals();
  }
  ~SignalLock() {
    theSignals.unblockSignals();
  }
};

void SignalsInfrastructure::unblockSignals() {
  int error = sigprocmask(SIG_SETMASK, &oldSignals, NULL);
  if (error < 0) {
    logServer << "Sigprocmask failed on server, I shall now crash. " << logger::endL;
    crash << "Sigprocmask failed on server." << crash;
  }
  this->flagSignalsAreBlocked = false;
  if (theGlobalVariables.flagServerDetailedLog) {
    logProcessStats << logger::green << "Detail: unmask successful. " << logger::endL;
  }
}

void SignalsInfrastructure::blockSignals() {
  sigset_t* theSignals = 0;
  if (!this->flagSignalsAreStored) {
    //store signals on first run.
    theSignals = &this->oldSignals;
  }
  int error = sigprocmask(SIG_BLOCK, &this->allSignals, theSignals);
  this->flagSignalsAreStored = true;
  if (error < 0) {
    logServer << logger::red << "Fatal error: sigprocmask failed. The server is going to crash. " << logger::endL;
    crash << "Sigprocmas failed. This should not happen. " << crash;
  }
  this->flagSignalsAreBlocked = true;
}

bool WebWorker::CheckConsistency() {
  auto oldOutputFn = stOutput.theOutputFunction;
  stOutput.theOutputFunction = 0;
  if (this->flagDeallocated) {
    crash << "Use after free of webworker." << crash;
  }
  if (this->parent == 0) {
    crash << "Parent of web worker is not initialized." << crash;
  }
  if (this->indexInParent == - 1) {
    crash << "Index in parent is bad. " << crash;
  }
  if (this->connectionID == - 1) {
    crash << "Connection id is bad. " << crash;
  }
  stOutput.theOutputFunction = oldOutputFn;
  return true;
}

bool WebWorker::ReceiveAll() {
  MacroRegisterFunctionWithName("WebWorker::ReceiveAll");
  if (this->connectedSocketID == - 1) {
    crash << "Attempting to receive on a socket with ID equal to - 1. " << crash;
  }
  if (theGlobalVariables.flagUsingSSLinCurrentConnection) {
    return this->ReceiveAllHttpSSL();
  }
  return this->ReceiveAllHttp();
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
  if (!theGlobalVariables.flagSSLIsAvailable) {
    logServer << logger::red << "SSL is DISABLED." << logger::endL;
    return;
  }
  //////////////////////////////////////////////////////////////////////////
  this->theTLS.initialize(true);
}

bool WebServer::SSLServerSideHandShake() {
  if (!theGlobalVariables.flagSSLIsAvailable) {
    return false;
  }
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection) {
    return false;
  }
  return this->theTLS.HandShakeIamServer(this->GetActiveWorker().connectedSocketID);
}

bool WebWorker::ReceiveAllHttpSSL() {
  MacroRegisterFunctionWithName("WebWorker::ReceiveAllHttpSSL");
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection) {
    return true;
  }
  if (!theGlobalVariables.flagSSLIsAvailable) {
    return false;
  }
  this->messageBody = "";
  this->messageHead = "";
  this->requestTypE = this->requestUnknown;
  if (this->connectedSocketID == - 1) {
    crash << "Attempting to receive on a socket with ID equal to - 1. " << crash;
  }
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 5;  // 5 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  setsockopt(this->connectedSocketID, SOL_SOCKET, SO_RCVTIMEO, (void*)(&tv), sizeof(timeval));
  std::string errorString;
  int numFailedReceives = 0;
  int maxNumFailedReceives = 3;
  double numSecondsAtStart = theGlobalVariables.GetElapsedSeconds();
  int numBytesInBuffer = - 1;
  List<char>& readBuffer = this->parent->theTLS.readBuffer;
  while (true) {
    numBytesInBuffer = this->parent->theTLS.SSLRead(
      &errorString, 0, true
    );
    //int64_t readTime = theGlobalVariables.GetElapsedMilliseconds() - msBeforesslread;
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
        << "WebWorker::ReceiveAllHttpSSL on socket "
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
  this->messageHead.assign(readBuffer.TheObjects, numBytesInBuffer);
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
  if (this->messageBody.size() == (unsigned) this->ContentLength) {
    return true;
  }
  this->messageBody.clear(); //<- needed else the length error check will pop.
  if (this->ContentLength > 10000000) {
    this->CheckConsistency();
    error = "Content-length parsed to be more than 10 million bytes, aborting.";
    logWorker << this->error << logger::endL;
    this->displayUserInput = this->error;
    return false;
  }
  this->remainingBytesToSenD = (std::string) "HTTP/1.0 100 Continue\r\n\r\n";
  this->SendAllBytesNoHeaders();
  this->remainingBytesToSenD.SetSize(0);
  std::string bufferString;
  while ((signed) this->messageBody.size() < this->ContentLength) {
    if (theGlobalVariables.GetElapsedSeconds() - numSecondsAtStart > 180) {
      this->error = "Receiving bytes timed out (180 seconds).";
      logIO << this->error << logger::endL;
      this->displayUserInput = this->error;
      return false;
    }
    numBytesInBuffer = this->parent->theTLS.SSLRead(
      &errorString, 0, true
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
      ERR_print_errors_fp(stderr);
      logIO << this->error << logger::endL;
      this->displayUserInput = this->error;
      return false;
    }
    bufferString.assign(readBuffer.TheObjects, numBytesInBuffer);
    this->messageBody += bufferString;
  }
  if ((signed) this->messageBody.size() != this->ContentLength) {
    this->messageHead += this->messageBody;
    this->ParseMessageHead();
  }
  if ((signed) this->messageBody.size() != this->ContentLength) {
    std::stringstream out;
    out << "The message-body received by me had length " << this->messageBody.size()
    << " yet I expected a message of length " << this->ContentLength << ". More details follow. "
    << this->ToStringMessageFullUnsafe();
    logIO << out.str() << logger::endL;
  }
  return true;
}

void WebWorker::SendAllBytesHttpSSL() {
  MacroRegisterFunctionWithName("WebWorker::SendAllBytesHttpSSL");
  if (this->remainingBytesToSenD.size == 0) {
    return;
  }
  this->CheckConsistency();
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection) {
    crash
    << "Error: WebWorker::SendAllBytesHttpSSL called "
    << "while flagUsingSSLinCurrentConnection is set to false. "
    << crash;
  }
  if (this->connectedSocketID == - 1) {
    logWorker << logger::red << "Socket::SendAllBytes failed: connectedSocketID = - 1." << logger::endL;
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
  std::stringstream commentsOnError;
  while (this->remainingBytesToSenD.size > 0) {
    if (theGlobalVariables.GetElapsedSeconds() - startTime > timeOutInSeconds) {
      logWorker << "WebWorker::SendAllBytes failed: more than " << timeOutInSeconds << " seconds have elapsed. "
      << logger::endL;
      return;
    }
    int numBytesSent = this->parent->theTLS.SSLWrite(
      this->remainingBytesToSenD,
      &errorString,
      0,
      &commentsOnError,
      true
    );
    if (numBytesSent < 0) {
      logWorker << "WebWorker::SendAllBytes failed: SSL_write error. " << logger::endL;
      return;
    }
    if (numBytesSent == 0) {
      numTimesRunWithoutSending ++;
    } else {
      numTimesRunWithoutSending = 0;
    }
    logWorker << numBytesSent;
    this->remainingBytesToSenD.Slice(numBytesSent, this->remainingBytesToSenD.size - numBytesSent);
    if (this->remainingBytesToSenD.size > 0) {
      logWorker << ", ";
    }
    if (numTimesRunWithoutSending > 3) {
      logWorker << "WebWorker::SendAllBytes failed: send function went through 3 cycles without "
      << " sending any bytes. "
      << logger::endL;
      return;
    }
    logWorker.flush();
  }
  logWorker << "... done." << logger::endL;
}

bool ProgressReportWebServer::flagServerExists = true;
const int WebServer::maxNumPendingConnections = 1000000;

ProgressReportWebServer::ProgressReportWebServer(const std::string& inputStatus) {
  this->flagDeallocated = false;
  this->indexProgressReport = theWebServer.theProgressReports.size;
  this->SetStatus(inputStatus);
}

ProgressReportWebServer::ProgressReportWebServer() {
  this->flagDeallocated = false;
  this->indexProgressReport = theWebServer.theProgressReports.size;
}

ProgressReportWebServer::~ProgressReportWebServer() {
  if (!this->flagServerExists) {
    return;
  }
  theWebServer.theProgressReports.SetSize(this->indexProgressReport);
  this->flagDeallocated = true;
}

void ProgressReportWebServer::SetStatus(const std::string& inputStatus) {
  MacroRegisterFunctionWithName("ProgressReportWebServer::SetStatus");
  if (theGlobalVariables.flagComputationFinishedAllOutputSentClosing || !this->flagServerExists) {
    return;
  }
  if (!theGlobalVariables.flagRunningBuiltInWebServer) {
    return;
  }
  theWebServer.CheckConsistency();
  MutexRecursiveWrapper& safetyFirst = theGlobalVariables.MutexWebWorkerStaticFiasco;
  safetyFirst.LockMe();
  if (this->indexProgressReport >= theWebServer.theProgressReports.size) {
    theWebServer.theProgressReports.SetSize(this->indexProgressReport + 1);
  }
  theWebServer.theProgressReports[this->indexProgressReport] = inputStatus;
  std::stringstream toBePiped;
  for (int i = 0; i < theWebServer.theProgressReports.size; i ++) {
    if (theWebServer.theProgressReports[i] != "") {
      toBePiped << "<br>" << theWebServer.theProgressReports[i];
    }
  }
  safetyFirst.UnlockMe();
  if (!theGlobalVariables.flagRunningBuiltInWebServer) {
    return;
  }
  theWebServer.GetActiveWorker().pipeWorkerToWorkerStatus.WriteOnceAfterEmptying(toBePiped.str(), false, false);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr* sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*) sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

std::string WebWorker::ToStringMessageShortUnsafe(FormatExpressions* theFormat) const {
  //if (theGlobalVariables.flagUsingSSLinCurrentConnection)
  //  return "Message cannot be viewed when using SSL";
  std::stringstream out;
  bool useHtml = theFormat == 0 ? false: theFormat->flagUseHTML;
  std::string lineBreak = useHtml ? "<br>\n" : "\r\n";
  out << lineBreak;
  if (this->requestTypE == this->requestGet) {
    out << "GET ";
  }
  else if (this->requestTypE == this->requestPost) {
    out << "POST ";
  } else if (this->requestTypE == this->requestChunked) {
    out << "GET or POST **chunked** " ;
  } else {
    out << "Request type undefined.";
  }
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

std::string WebWorker::ToStringMessageFullUnsafe(bool useHTML) const {
  //if (theGlobalVariables.flagUsingSSLinCurrentConnection)
  //  return "Message cannot be viewed when using SSL";
  std::stringstream out;
  out << this->ToStringMessageUnsafe(useHTML);
  if (this->theMessageHeaderStrings.size > 0) {
    out << "<hr>\nStrings extracted from message: ";
    for (int i = 0; i < this->theMessageHeaderStrings.size; i ++) {
      out << "<br>" << this->theMessageHeaderStrings[i];
    }
  }
  return out.str();
}

std::string WebWorker::ToStringMessageUnsafe(bool useHTML) const {
  //if (theGlobalVariables.flagUsingSSLinCurrentConnection)
  //  return "Message cannot be viewed when using SSL";
  std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML = true;
  out << this->ToStringMessageShortUnsafe(&tempFormat);
  out << "<hr>";
  out << "Main address: " << HtmlRoutines::ConvertStringToHtmlString(this->addressGetOrPost, true) << "<br>";
  if (this->requestTypE == this->requestGet) {
    out << "GET " << HtmlRoutines::ConvertStringToHtmlString(this->addressGetOrPost, true);
  }
  if (this->requestTypE == this->requestPost) {
    out << "POST " << HtmlRoutines::ConvertStringToHtmlString(this->addressGetOrPost, true);
  }
  if (this->requestTypE == this->requestChunked) {
    out << "POST or GET **chunked** " << HtmlRoutines::ConvertStringToHtmlString(this->addressGetOrPost, true);
  }
  if (this->flagKeepAlive) {
    out << "<br><b>Keeping alive.</b><br>";
  } else {
    out << "<br><b>NOT</b> keeping alive.<br>";
  }
  out << "<br>Cookies (" << this->cookies.size << " total):";
  for (int i = 0; i < this->cookies.size; i ++) {
    out << "<br>" << this->cookies[i];
  }
  if (useHTML) {
    out << "\n<hr>\nHost with port:<br>\n" << HtmlRoutines::ConvertStringToHtmlString(theGlobalVariables.hostWithPort, false);
    out << "\n<hr>\nFull message head:<br>\n" << HtmlRoutines::ConvertStringToHtmlString(this->messageHead, true);
    out << "\n<hr>\nFull message body:<br>\n" << HtmlRoutines::ConvertStringToHtmlString(this->messageBody, true);
  } else {
    out << "\nFull message head (length " << this->messageHead.size()
    << "):\n" << this->messageHead;
    out << "\nFull message body (length " << this->messageBody.size()
    << "):\n" << this->messageBody;
  }
  return out.str();
}

void WebWorker::resetConnection() {
  MacroRegisterFunctionWithName("WebWorker::resetConnection");
  //This function needs a security audit.
  this->resetMessageComponentsExceptRawMessage();
  UserCalculatorData blankUser;
  theGlobalVariables.userDefault = blankUser;
  if (
    theGlobalVariables.userDefault.enteredActivationToken != "" ||
    theGlobalVariables.userDefault.enteredAuthenticationToken != "" ||
    theGlobalVariables.userDefault.enteredGoogleToken != "" ||
    theGlobalVariables.userDefault.enteredPassword != "" ||
    theGlobalVariables.userDefault.enteredHashedSaltedPassword != ""
  ) {
    crash << "User default not reset correctly. " << crash;
  }
  theGlobalVariables.flagLoggedIn = false;
  theGlobalVariables.flagLogInAttempted = false;
  theGlobalVariables.webArguments.Clear();
  theGlobalVariables.userCalculatorRequestType = "";
  theGlobalVariables.userInputStringIfAvailable = "";
  theGlobalVariables.userInputStringRAWIfAvailable = "";
  this->flagAllBytesSentUsingFile = false;
  this->flagEncounteredErrorWhileServingFile = false;
}

void WebWorker::resetMessageComponentsExceptRawMessage() {
  this->addressComputed = "";
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
void WebWorker::StandardOutputAfterTimeOut(const std::string& input) {
  standardOutputStreamAfterTimeout << input;
}

std::string WebWorker::GetNavigationPage() {
  MacroRegisterFunctionWithName("WebWorker::GetNavigationPage");
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

std::string WebWorker::GetDatabaseJSON() {
  MacroRegisterFunctionWithName("WebWorker::GetDatabaseJSON");
  if (!theGlobalVariables.UserDefaultHasAdminRights()) {
    return "Only logged-in admins can access database. ";
  }
  JSData result;
  if (theGlobalVariables.flagDatabaseCompiled) {
    std::string operation = theGlobalVariables.GetWebInput(WebAPI::databaseParameters::operation);
    std::string labels = HtmlRoutines::ConvertURLStringToNormal(
      theGlobalVariables.GetWebInput(WebAPI::databaseParameters::labels), false
    );
    if (operation == WebAPI::databaseParameters::fetch) {
      result = DatabaseRoutinesGlobalFunctionsMongo::ToJSONDatabaseFetch(labels);
    } else {
      result["error"] = "Uknown database operation: " + operation + ". ";
    }
    if (theGlobalVariables.UserDebugFlagOn()) {
      result["databaseOperation"] = operation;
      result["databaseLabels"] = labels;
    }
  } else {
    result["error"] = "Database not available (cannot get database info). ";
  }
  return result.ToString(false);
}

std::string WebWorker::GetDatabaseDeleteOneItem() {
  MacroRegisterFunctionWithName("WebWorker::GetDatabaseDeleteOneItem");
  if (!theGlobalVariables.UserDefaultHasAdminRights()) {
    return "Only logged-in admins can execute the delete command. ";
  }
  std::stringstream commentsStream;
  std::string inputEncoded = theGlobalVariables.GetWebInput("item");
  std::string inputString = HtmlRoutines::ConvertURLStringToNormal(inputEncoded, false);
  JSData inputParsed;
  if (!inputParsed.readstring(inputString, false, &commentsStream)) {
    commentsStream << "Failed to parse input string. ";
    return commentsStream.str();
  }
  commentsStream << "Parsed input string: " << inputParsed.ToString(false, false) << "\n";
  if (DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntry(inputParsed, &commentsStream)) {
    return "success";
  }
  if (!theGlobalVariables.flagDatabaseCompiled) {
    commentsStream << "Database not available (cannot delete item). ";
  }
  return commentsStream.str();
}

bool WebWorker::ExtractArgumentsFromCookies(std::stringstream& argumentProcessingFailureComments) {
  MacroRegisterFunctionWithName("WebWorker::ExtractArgumentsFromCookies");
  MapLisT<std::string, std::string, MathRoutines::HashString> newlyFoundArgs;
  bool result = true;
  for (int i = 0; i < this->cookies.size; i ++) {
    if (!HtmlRoutines::ChopCGIStringAppend(this->cookies[i], newlyFoundArgs, argumentProcessingFailureComments)) {
      result = false;
    }
  }
  for (int i = 0; i < newlyFoundArgs.size(); i ++) {
    if (theGlobalVariables.webArguments.Contains(newlyFoundArgs.theKeys[i])) {
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
      theGlobalVariables.webArguments.SetKeyValue(newlyFoundArgs.theKeys[i], trimmed);
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
  MapLisT<std::string, std::string, MathRoutines::HashString>& theArgs =
  theGlobalVariables.webArguments;
  if (!HtmlRoutines::ChopCGIStringAppend(input, theArgs, argumentProcessingFailureComments))
    return false;
  if (theGlobalVariables.flagRunningApache) {
    if (this->addressComputed == "") {
      this->addressComputed = theGlobalVariables.GetWebInput("request");
    }
  }
  if (theArgs.Contains("doubleURLencodedInput")) {
    std::string newInput = theGlobalVariables.GetWebInput("doubleURLencodedInput");
    theArgs.Clear();
    return this->ExtractArgumentsFromMessage(
      newInput, argumentProcessingFailureComments, recursionDepth + 1
    );
  }
  return true;
}

//Returns false if something unexpected happens during the login procedure.
//Returning true does not necessarily mean the login information was accepted.
//Returning false guarantees the login information was not accepted.
bool WebWorker::LoginProcedure(std::stringstream& argumentProcessingFailureComments, std::stringstream* comments) {
  MacroRegisterFunctionWithName("WebWorker::Login");
  theGlobalVariables.flagLoggedIn = false;
  MapLisT<std::string, std::string, MathRoutines::HashString>& theArgs = theGlobalVariables.webArguments;
  UserCalculatorData& theUser = theGlobalVariables.userDefault;
  theUser.username = HtmlRoutines::ConvertURLStringToNormal(
    theGlobalVariables.GetWebInput("username"), true
  );
  if (theUser.username.find('%') != std::string::npos) {
    argumentProcessingFailureComments << "<b>Unusual behavior: % sign in username.</b>";
  }
  theUser.enteredAuthenticationToken = HtmlRoutines::ConvertURLStringToNormal(
    theGlobalVariables.GetWebInput("authenticationToken"), false
  );
  theUser.enteredGoogleToken = HtmlRoutines::ConvertURLStringToNormal(
    theGlobalVariables.GetWebInput("googleToken"), false
  );
  theUser.enteredActivationToken = HtmlRoutines::ConvertURLStringToNormal(
    theGlobalVariables.GetWebInput("activationToken"), false
  );
  theUser.enteredPassword = HtmlRoutines::ConvertStringToURLString(
    HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("password"), true), false
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
    theGlobalVariables.flagLogInAttempted = true;
    if (theGlobalVariables.UserDebugFlagOn() && comments != 0) {
      *comments << "Password was entered: all other authentication methods ignored. ";
    }
  }
  if (theUser.enteredActivationToken != "") {
    theUser.enteredGoogleToken = "";
    theUser.enteredAuthenticationToken = "";
    theUser.flagEnteredAuthenticationToken = false;
    theUser.flagEnteredActivationToken = true;
    theGlobalVariables.flagLogInAttempted = true;
    if (theGlobalVariables.UserDebugFlagOn() && comments != 0) {
      *comments << "Activation token entered: authentication token and google token ignored. ";
    }
  }
  if (theUser.username != "") {
    theUser.enteredGoogleToken = "";
  }
  if (theUser.enteredAuthenticationToken != "") {
    theUser.flagEnteredAuthenticationToken = true;
    theGlobalVariables.flagLogInAttempted = true;
  }
  /////////////////////////////////////////////
  //this may need a security audit: the URLStringToNormal and GetWebInput
  //functions may leave traces of (freed) memory
  //of the old password. Could only be used
  //if the attacker has control of the server executable - which probably means we
  //already are in big trouble - so this really shouldn't be such a big deal.
  /////////////////////////////////////////////
  if (
    !theUser.flagEnteredPassword &&
    !theUser.flagEnteredAuthenticationToken &&
    theUser.enteredActivationToken == "" &&
    theUser.enteredGoogleToken == ""
  ) {
    return !theUser.flagMustLogin;
  }
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection) {
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
  theGlobalVariables.userCalculatorRequestType == WebAPI::request::changePassword ||
  theGlobalVariables.userCalculatorRequestType == WebAPI::request::activateAccountJSON;

  if (changingPass) {
    theUser.enteredAuthenticationToken = "";
  }
  if (doAttemptGoogleTokenLogin) {
    bool tokenIsGood = false;
    theGlobalVariables.flagLoggedIn = DatabaseRoutinesGlobalFunctions::LoginViaGoogleTokenCreateNewAccountIfNeeded(
      theUser, &argumentProcessingFailureComments, 0, tokenIsGood
    );
    if (tokenIsGood && !theGlobalVariables.flagLoggedIn && comments != 0) {
      *comments << "Your authentication is valid but I have problems with my database records. ";
    }
  } else if (
    theUser.enteredAuthenticationToken != "" || theUser.enteredPassword != "" ||
    theUser.enteredActivationToken != ""
  ) {
    theGlobalVariables.flagLoggedIn = DatabaseRoutinesGlobalFunctions::LoginViaDatabase(
      theUser, &argumentProcessingFailureComments, comments
    );
  }
  theGlobalVariables.CookiesToSetUsingHeaders.SetKeyValue(
    "username",
    HtmlRoutines::ConvertStringToURLString(theUser.username, false)
    //<-User name must be stored in URL-encoded fashion, NO PLUSES.
  );
  if (theGlobalVariables.flagLoggedIn && theUser.enteredActivationToken == "") {
    //in case the user logged in with password, we need
    //to give him/her the correct authentication token
    theGlobalVariables.CookiesToSetUsingHeaders.SetKeyValue(
      DatabaseStrings::labelAuthenticationToken,
      HtmlRoutines::ConvertStringToURLString(theUser.actualAuthenticationToken, false)
      //<-URL-encoded fashion, NO PLUSES.
    );
    theGlobalVariables.SetWebInpuT(
      DatabaseStrings::labelAuthenticationToken,
      HtmlRoutines::ConvertStringToURLString(theUser.actualAuthenticationToken, false)
    );
  } else {
    theGlobalVariables.CookiesToSetUsingHeaders.SetKeyValue("authenticationToken", "0");
  }
  bool shouldDisplayMessage = false;
  if (!theGlobalVariables.flagLoggedIn && theUser.username != "") {
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
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection) {
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

void WebWorker::OutputResultAfterTimeout() {
  MacroRegisterFunctionWithName("WebWorker::OutputResultAfterTimeout");
  //out << theParser.ToStringOutputSpecials();
  std::string theFileName = "output/" + theGlobalVariables.RelativePhysicalNameOutpuT;
  std::string theLink = "output/" + HtmlRoutines::ConvertStringToURLString(
    theGlobalVariables.RelativePhysicalNameOutpuT, false
  );
  JSData output = theParser->ToJSONOutputAndSpecials();
  output["outputFile"] = theLink;
  std::fstream outputTimeOutFile;
  FileOperations::OpenFileCreateIfNotPresentVirtual(
    outputTimeOutFile, theFileName, false, true, false
  );
  outputTimeOutFile << "<html><body>" << theParser->ToStringOutputAndSpecials() << "</body></html>";
  outputTimeOutFile.close();
  WebWorker::OutputSendAfterTimeout(output.ToString(false));
}

void WebWorker::OutputCrashAfterTimeout() {
  MacroRegisterFunctionWithName("WebWorker::OutputCrashAfterTimeout");
  logWorker << logger::red << "Crashing AFTER timeout!" << logger::endL;
  WebWorker::OutputSendAfterTimeout(standardOutputStreamAfterTimeout.str());
  theWebServer.SignalActiveWorkerDoneReleaseEverything();
}

void WebWorker::OutputSendAfterTimeout(const std::string& input) {
  MacroRegisterFunctionWithName("WebWorker::OutputSendAfterTimeout");
  theGlobalVariables.flagTimedOutComputationIsDone = true;
  logWorker << "WebWorker::StandardOutputPart2ComputationTimeout called with worker number "
  << theWebServer.GetActiveWorker().indexInParent + 1 << "." << logger::endL;
  //requesting pause which will be cleared by the receiver of pipeWorkerToWorkerIndicatorData
  theWebServer.GetActiveWorker().PauseComputationReportReceived.RequestPausePauseIfLocked(false, false);
  logWorker << logger::blue << "Sending result through indicator pipe." << logger::endL;
  theWebServer.GetActiveWorker().pipeWorkerToWorkerIndicatorData.WriteOnceAfterEmptying(input, false, false);
  logBlock << logger::blue << "Final output written to indicator, blocking until data "
  << "is received on the other end." << logger::endL;
  if (!theWebServer.GetActiveWorker().PauseComputationReportReceived.PauseIfRequestedWithTimeOut(false, false)) {
  } else {
    //requesting pause which will be cleared by the receiver of pipeWorkerToWorkerIndicatorData
    theWebServer.GetActiveWorker().PauseComputationReportReceived.RequestPausePauseIfLocked(false, false);
  }
  theWebServer.GetActiveWorker().pipeWorkerToWorkerIndicatorData.WriteOnceAfterEmptying("finished", false, false);
  logWorker << logger::red << "\"finished\" sent through indicator pipe, waiting." << logger::endL;
  theWebServer.GetActiveWorker().PauseComputationReportReceived.PauseIfRequestedWithTimeOut(false, false);
}

void WebWorker::ParseMessageHead() {
  MacroRegisterFunctionWithName("WebWorker::ParseMessageHead");
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
  for (unsigned i = 0; i < this->messageHead.size(); i ++) {
    if (this->messageHead[i] != ' ' && this->messageHead[i] != '\n' && this->messageHead[i] != '\r') {
      buffer.push_back(this->messageHead[i]);
      if (i == this->messageHead.size() - 1) {
        this->theMessageHeaderStrings.AddOnTop(buffer);
      }
    } else {
      if (buffer != "") {
        this->theMessageHeaderStrings.AddOnTop(buffer);
        buffer = "";
      }
      if (i > 0) {
        if (this->messageHead[i - 1] == '\r' && this->messageHead[i] == '\n') {
          this->theMessageHeaderStrings.AddOnTop("\n");
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
          this->theMessageHeaderStrings[i] == "chunked;"||
          this->theMessageHeaderStrings[i] == "Chunked"||
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
        LargeIntUnsigned theLI;
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
        this->cookies.AddOnTop(this->theMessageHeaderStrings[i]);
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
        this->connectionFlags.AddOnTop(this->theMessageHeaderStrings[i]);
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
    this->ContentLength = this->messageBody.size();
  }
  theGlobalVariables.hostWithPort = this->hostWithPort;
}

void WebWorker::AttemptUnknownRequestErrorCorrection() {
  MacroRegisterFunctionWithName("WebWorker::AttemptUnknownRequestErrorCorrection");
  if (this->requestTypE != this->requestUnknown) {
    return;
  }
  logIO << logger::red << "Unknown request. " << logger::endL;
  logIO << logger::blue << "Message head length: " << this->messageHead.size() << logger::endL;
  std::string messageHeadHexed = Crypto::ConvertStringToHex(this->messageHead, 100, false);
  logIO << HtmlRoutines::ConvertStringToHtmlStringRestrictSize(messageHeadHexed, false, 300) << logger::endL;
  logIO << logger::orange << "Message body length: " << this->messageBody.size() << ". " << logger::endL;
  logIO << HtmlRoutines::ConvertStringToHtmlStringRestrictSize(this->messageBody, false, 300) << logger::endL;
  logIO << logger::green << "Attempting to correct unknown request.\n";
  if (this->messageBody.size() == 0) {
    if (*this->theMessageHeaderStrings.LastObject() != "\n") {
      logIO << logger::green
      << "Message body set to last message chunk.\n";
      this->messageBody = *this->theMessageHeaderStrings.LastObject();
    }
  }
  if (this->messageBody.size() != 0) {
    logIO << "Request set to: POST\n";
    this->requestTypE = this->requestPost;
  } else {
    logIO << "Request set to: GET\n";
    this->requestTypE = this->requestGet;
  }
  if (this->addressGetOrPost == "") {
    logIO << "Address set to: " << theGlobalVariables.DisplayNameExecutable << "\n";
    this->addressGetOrPost = theGlobalVariables.DisplayNameExecutable;
  }
  logIO << logger::blue
  << "Unrecognized message head, length: " << this->messageHead.size() << ".\n";
  logIO << logger::red << "Message body length: " << this->messageBody.size() << ". ";
  logIO << logger::endL;
}

bool WebWorker::ReceiveAllHttp() {
  MacroRegisterFunctionWithName("WebWorker::ReceiveAllHttp");
  this->messageBody = "";
  this->messageHead = "";
  this->requestTypE = this->requestUnknown;
  unsigned const int bufferSize = 60000;
  char buffer[bufferSize];
  if (this->connectedSocketID == - 1) {
    crash << "Attempting to receive on a socket with ID equal to - 1. " << crash;
  }
  struct timeval tv; //<- code involving tv taken from stackexchange
  tv.tv_sec = 5;  // 5 Secs Timeout
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  setsockopt(this->connectedSocketID, SOL_SOCKET, SO_RCVTIMEO, (void*)(&tv), sizeof(timeval));
  double numSecondsAtStart = theGlobalVariables.GetElapsedSeconds();
  int numBytesInBuffer = recv(this->connectedSocketID, &buffer, bufferSize - 1, 0);
  int numFailedReceives = 0;
  bool result = true;
  while ((numBytesInBuffer < 0) || (numBytesInBuffer > ((signed)bufferSize))) {
    std::stringstream out;
    numFailedReceives ++;
    out
    << "WebWorker::ReceiveAllHttp on socket " << this->connectedSocketID
    << " failed (so far "
    << numFailedReceives << " fails). "
    << "Return value: " << numBytesInBuffer
    << ". Error description: "
    << this->parent->ToStringLastErrorDescription();
    if (numFailedReceives > 5) {
      out
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
  if (numBytesInBuffer == 0) {
    return true;
  }
  this->ParseMessageHead();
  if (this->requestTypE == WebWorker::requestTypes::requestPost) {
    this->displayUserInput = "POST " + this->addressGetOrPost;
  } else if (this->requestTypE == WebWorker::requestTypes::requestGet) {
    this->displayUserInput = "GET " + this->addressGetOrPost;
  } else if (this->requestTypE == WebWorker::requestTypes::requestHead) {
    this->displayUserInput = "HEAD " + this->addressGetOrPost;
  } else if (this->requestTypE == WebWorker::requestTypes::requestChunked) {
    this->displayUserInput = "GET or POST **chunked** " + this->addressGetOrPost;
  } else {
    this->displayUserInput = "UNKNOWN REQUEST " + this->addressGetOrPost;
  }
  if (this->ContentLength <= 0) {
    if (this->requestTypE == this->requestUnknown) {
      this->AttemptUnknownRequestErrorCorrection();
    }
    return true;
  }
  if (this->messageBody.size() == (unsigned) this->ContentLength) {
    if (this->requestTypE == this->requestUnknown) {
      this->AttemptUnknownRequestErrorCorrection();
    }
    return true;
  }
  this->messageBody.clear();//<-needed else the length error check won't work out.
  if (this->ContentLength > 10000000) {
    this->CheckConsistency();
    error = "Content-length parsed to be more than 10 million bytes, aborting.";
    logWorker << this->error << logger::endL;
    this->displayUserInput = this->error;
    return false;
  }
  this->remainingBytesToSenD = (std::string) "HTTP/1.0 100 Continue\r\n\r\n";
  this->SendAllBytesNoHeaders();
  this->remainingBytesToSenD.SetSize(0);
  std::string bufferString;
  while ((signed) this->messageBody.size() < this->ContentLength) {
    if (theGlobalVariables.GetElapsedSeconds() - numSecondsAtStart > 180) {
      this->error = "Receiving bytes timed out (180 seconds).";
      logIO << this->error << logger::endL;
      this->displayUserInput = this->error;
      return false;
    }
    numBytesInBuffer = recv(this->connectedSocketID, &buffer, bufferSize - 1, 0);
    if (numBytesInBuffer == 0) {
      this->error = "While trying to fetch message-body, received 0 bytes. " +
      this->parent->ToStringLastErrorDescription();
      logIO << this->error << logger::endL;
      this->displayUserInput = this->error;
      return false;
    }
    if (numBytesInBuffer < 0) {
      if (
        errno == EAGAIN ||
        errno == EWOULDBLOCK ||
        errno == EINTR ||
        errno == EIO || // errno == ENOBUFS ||
        errno == ENOMEM
      ) {
        continue;
      }
      this->error = "Error fetching message body: " + this->parent->ToStringLastErrorDescription();
      logIO << logger::red << this->error << logger::endL;
      this->displayUserInput = this->error;
      return false;
    }
    bufferString.assign(buffer, numBytesInBuffer);
    this->messageBody += bufferString;
  }
  if ((signed) this->messageBody.size() != this->ContentLength) {
    if (this->requestTypE != this->requestChunked) {
      logIO << logger::red
      << "Message body is of length: " << this->messageBody.size()
      << " yet this->ContentLength equals: "
      << this->ContentLength
      << ". Perhaps very long headers got truncated? "
      << logger::endL;
      this->messageHead += this->messageBody;
      this->ParseMessageHead();
      this->AttemptUnknownRequestErrorCorrection();
    }
  }
  if ((signed) this->messageBody.size() != this->ContentLength) {
    std::stringstream out;
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
  theGlobalVariables.hostWithPort = this->hostWithPort;
  theGlobalVariables.hostNoPort = this->hostNoPort;
  if (
    theGlobalVariables.hostNoPort == "localhost" ||
    theGlobalVariables.hostNoPort == "127.0.0.1"
  ) {
    theGlobalVariables.flagRequestComingLocally = true;
  } else {
    theGlobalVariables.flagRequestComingLocally = false;
  }
}

void WebWorker::ExtractAddressParts() {
  MacroRegisterFunctionWithName("WebWorker::ExtractAdressParts");
  bool found = false;
  for (signed i = 0; i < (signed) this->addressGetOrPost.size(); i ++) {
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
  if (theGlobalVariables.flagRunningApache && this->argumentComputed == "") {
    this->argumentComputed = this->addressComputed;
    this->addressComputed = "";
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
  for (int i = 0; i < theGlobalVariables.CookiesToSetUsingHeaders.size(); i ++) {
    out << "Set-Cookie: " << theGlobalVariables.CookiesToSetUsingHeaders.theKeys[i]
    << "="
    << theGlobalVariables.CookiesToSetUsingHeaders.theValues[i]
    << "; Path=/; Expires=Sat, 01 Jan 2030 20:00:00 GMT; Secure";
    if (i != theGlobalVariables.CookiesToSetUsingHeaders.size() - 1) {
      out << "\r\n";
    }
  }
  return out.str();
}

void WebWorker::SetHeadeR(const std::string& httpResponseNoTermination, const std::string& remainingHeaderNoTermination) {
  MacroRegisterFunctionWithName("WebWorker::SetHeader");
  if (theGlobalVariables.flagRunningApache) {
    if (remainingHeaderNoTermination != "") {
      stOutput << remainingHeaderNoTermination << "\r\n";
    }
    if (theGlobalVariables.flagLoggedIn && WebWorker::GetHeaderSetCookie() != "") {
      stOutput << WebWorker::GetHeaderSetCookie() << "\r\n";
    }
    stOutput << "\r\n";
    return;
  }
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
    (theGlobalVariables.flagLoggedIn || theGlobalVariables.flagLogInAttempted) &&
    WebWorker::GetHeaderSetCookie() != ""
  ) {
    out << WebWorker::GetHeaderSetCookie() << "\r\n";
  }
  std::string finalHeader = out.str();
  this->remainingHeaderToSend.SetSize(0);
  this->remainingHeaderToSend.SetExpectedSize(finalHeader.size());
  for (unsigned i = 0; i < finalHeader.size(); i ++) {
    this->remainingHeaderToSend.AddOnTop(finalHeader[i]);
  }
}

void WebWorker::SetHeaderOKNoContentLength(const std::string& extraHeader, const std::string &contentType) {
  MacroRegisterFunctionWithName("WebWorker::SetHeaderOKNoContentLength");
  std::string header = "Content-Type: " + contentType + "; charset=utf-8\r\nAccess-Control-Allow-Origin: *";
  if (extraHeader != "") {
    header += "\r\n" + extraHeader;
  }
  this->SetHeadeR("HTTP/1.0 200 OK", header);
  this->flagDoAddContentLength = true;
}

void WebWorker::SanitizeVirtualFileName() {
  MacroRegisterFunctionWithName("WebWorker::SanitizeVirtualFileName");
  std::string resultName;
  resultName.reserve(this->VirtualFileName.size());
  bool foundslash = false;
  for (signed i = (signed) this->VirtualFileName.size() - 1; i >= 0; i --) {
    bool isOK = true;
    if (foundslash && this->VirtualFileName[i] == '.') {
      if (i > 0 && this->VirtualFileName[i - 1] == '.') {
        this->flagFileNameSanitized = true;
        isOK = false;
      }
    }
    if (isOK) {
      resultName.push_back(this->VirtualFileName[i]);
    }
    if (this->VirtualFileName[i] == '/') {
      foundslash = true;
    }
  }
  this->VirtualFileName = "";
  for (int i = resultName.size() - 1; i >= 0; i --) {
    this->VirtualFileName.push_back(resultName[i]);
  }
}

int WebWorker::ProcessCalculatorExamplesJSON() {
  MacroRegisterFunctionWithName("WebWorker::ProcessCalculatorExamplesJSON");
  this->SetHeaderOKNoContentLength("");
  stOutput << theParser->ToStringFunctionHandlersJSON();
  return 0;
}

int WebWorker::ProcessToggleMonitoring() {
  MacroRegisterFunctionWithName("WebWorker::ProcessToggleMonitoring");
  this->SetHeaderOKNoContentLength("");
  if (theGlobalVariables.UserDefaultHasAdminRights()) {
    this->flagToggleMonitoring = true;
  }
  JSData result;
  HtmlInterpretation::GetJSDataUserInfo(result, "Attempt to toggle monitoring. ");
  stOutput << result.ToString(false);
  return 0;
}

int WebWorker::ProcessServerStatusJSON() {
  MacroRegisterFunctionWithName("WebWorker::ProcessServerStatusJSON");
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

int WebWorker::ProcessPauseWorker() {
  MacroRegisterFunctionWithName("WebWorker::ProcessPauseWorker");
  this->SetHeaderOKNoContentLength("");
  std::string theMainInput = theGlobalVariables.GetWebInput("mainInput");
  std::stringstream out;
  JSData result;
  if (theMainInput == "") {
    result[WebAPI::result::error] = "To pause a worker please 1) turn on monitoring and 2) provide the worker number in the mainInput field. ";
    stOutput << result.ToString(false);
    return 0;
  }
  int inputWebWorkerNumber = atoi(theMainInput.c_str());
  int inputWebWorkerIndex = inputWebWorkerNumber - 1;
  if (inputWebWorkerIndex < 0 || inputWebWorkerIndex >= this->parent->theWorkers.size) {
    out << "<b>User requested worker number " << inputWebWorkerNumber
    << " out of " << this->parent->theWorkers.size << " which is out of range. </b>";
    result[WebAPI::result::error] = out.str();
    stOutput << result.ToString(false);
    return 0;
  }
  if (!this->parent->theWorkers[inputWebWorkerIndex].flagInUsE) {
    out << "<b>Requested worker number " << inputWebWorkerNumber << " is not in use. Total number of workers: "
    << this->parent->theWorkers.size << ". </b>";
    result[WebAPI::result::error] = out.str();
    stOutput << result.ToString(false);
    return 0;
  }
  if (inputWebWorkerIndex == this->indexInParent) {
    out << "Worker " << inputWebWorkerIndex
    << " attempts to pause itself, this is not allowed.";
    result[WebAPI::result::error] = out.str();
    stOutput << result.ToString(false);
    return 0;
  }
  logWorker << "Proceeding to toggle worker pause." << logger::endL;
  WebWorker& otherWorker = this->parent->theWorkers[inputWebWorkerIndex];
  if (!otherWorker.PauseWorker.CheckPauseIsRequested(false, false, false)) {
    otherWorker.PauseWorker.RequestPausePauseIfLocked(false, false);
    result[WebAPI::result::status] = "paused";
    stOutput << result.ToString(false);
    return 0;
  }
  result[WebAPI::result::status] = "unpaused";
  stOutput << result.ToString(false);
  otherWorker.PauseWorker.ResumePausedProcessesIfAny(false, false);
  return 0;
}

int WebWorker::ProcessMonitor() {
  MacroRegisterFunctionWithName("WebWorker::ProcessMonitor");
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
  if (theGlobalVariables.flagAllowProcessMonitoring && theGlobalVariables.UserDefaultHasAdminRights()) {
    result["webWorkerNumber"] = atoi(theMainInput.c_str());
    result["authenticated"] = "true";
  } else {
    result["authenticated"] = "false";
    std::stringstream commentStream;
    commentStream << "Process monitoring is allowed only for logged-in admins with process monitoring turned on. "
    << "There must be a link/button to turn on process monitoring in the calculator app. ";
    result[WebAPI::result::comments] = commentStream.str();
  }
  stOutput << result.ToString(false);
  return 0;
}

int WebWorker::ProcessComputationIndicator() {
  MacroRegisterFunctionWithName("WebWorker::ProcessComputationIndicator");
  this->SetHeaderOKNoContentLength("");
  logWorker << "Processing get request indicator." << logger::endL;
  std::string theMainInput = theGlobalVariables.GetWebInput("mainInput");
  JSData result;
  std::stringstream out, comments;
  if (!theGlobalVariables.UserDefaultHasAdminRights()) {
    result[WebAPI::result::error] = "Process monitoring only allowed to logged-in admins. ";
    stOutput << result.ToString(false);
    return 0;
  }
  if (theMainInput == "") {
    out << "To get a computation indicator you need to supply the number of "
    << "the child process in the mainInput field.";
    result[WebAPI::result::error] = out.str();
    stOutput << result.ToString(false);
    return 0;
  }
  int inputWebWorkerNumber = atoi(theMainInput.c_str());
  int inputWebWorkerIndex = inputWebWorkerNumber - 1;
  if (inputWebWorkerIndex < 0 || inputWebWorkerIndex >= this->parent->theWorkers.size) {
    out << "Indicator error. Worker number "
    << inputWebWorkerNumber << " is out of range: there are "
    << this->parent->theWorkers.size
    << " workers. ";
    result[WebAPI::result::error] = out.str();
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
  if (!this->parent->theWorkers[inputWebWorkerIndex].flagInUsE) {
    out << "Indicator error. Worker number "
    << inputWebWorkerNumber << " is not in use. "
    << " Total number of workers: "
    << this->parent->theWorkers.size << ". ";
    result[WebAPI::result::error] = out.str();
    result[WebAPI::result::comments] = comments.str();
    stOutput << result.ToString(false);
    return 0;
  }
  if (inputWebWorkerIndex == this->indexInParent) {
    out << "Indicator error. Worker number "
    << inputWebWorkerNumber << " requested to monitor itself. "
    << " This is not allowed. ";
    result[WebAPI::result::error] = out.str();
    result[WebAPI::result::comments] = comments.str();
    stOutput << result.ToString(false);
    return 0;
  }
  WebWorker& otherWorker = this->parent->theWorkers[inputWebWorkerIndex];
  if (otherWorker.userAddress.theObject != this->userAddress.theObject) {
    out << "User ip address: " << this->userAddress.theObject
    << " does not coincide with address that "
    << "initiated the process. This is not allowed. ";
    result[WebAPI::result::error] = out.str();
    stOutput << result.ToString(false);
    return 0;
  }
  if (this->flagUsingSSLInWorkerProcess != true) {
    out << "Monitoring allowed only over https. ";
    result[WebAPI::result::error] = out.str();
    stOutput << result.ToString(false);
    return 0;
  }
  otherWorker.pipeWorkerToWorkerRequestIndicator.WriteOnceAfterEmptying("!", false, false);
  otherWorker.pipeWorkerToWorkerIndicatorData.ReadOnce(false, false);
  if (otherWorker.pipeWorkerToWorkerIndicatorData.thePipe.lastRead.size > 0) {
    std::string outputString;
    outputString.assign(
      otherWorker.pipeWorkerToWorkerIndicatorData.thePipe.lastRead.TheObjects,
      otherWorker.pipeWorkerToWorkerIndicatorData.thePipe.lastRead.size
    );
    if (outputString != "finished") {
      result[WebAPI::result::resultStringified] = outputString;
      result[WebAPI::result::status] = "running";
    } else {
      result[WebAPI::result::status] = "finished";
    }
    stOutput << result.ToString(false);
    otherWorker.PauseComputationReportReceived.ResumePausedProcessesIfAny(false, false);
  } else {
    result[WebAPI::result::status] = "noReport";
    stOutput << result.ToString(false);
  }
  return 0;
}

void WebWorker::WriteProgressReportToFile(const std::string& input) {
  MacroRegisterFunctionWithName("WebWorker::WriteProgressReportToFile");
  logWorker << logger::green << "Progress report written to file: "
  << theGlobalVariables.RelativePhysicalNameProgressReport << logger::endL;
  std::fstream theFile;
  if (!FileOperations::OpenFileCreateIfNotPresentVirtual(
    theFile, "output/" + theGlobalVariables.RelativePhysicalNameProgressReport, false, true, false
  )) {
    FileOperations::OpenFileCreateIfNotPresentVirtual(
      theFile,
      "output/progressReport_failed_to_create_file.html",
      false,
      true,
      false
    );
  }
  theFile << standardOutputStreamAfterTimeout.str() << "<hr>" << input;
  theFile.flush();
  theFile.close();
}

void WebWorker::PipeProgressReport(const std::string& input) {
  if (!this->flagProgressReportAllowed) {
    return;
  }
  MacroRegisterFunctionWithName("WebWorker::PipeProgressReport");
  this->PauseIndicatorPipeInUse.RequestPausePauseIfLocked(false, false);
  if (this->PauseWorker.CheckPauseIsRequested(false, false, false)) {
    logBlock << ": pausing as requested ..." << logger::endL;
    this->WriteProgressReportToFile(input);
  }
  this->PauseWorker.PauseIfRequested(false, false);     //if pause was requested, here we block
  this->pipeWorkerToWorkerRequestIndicator.ReadOnce(false, false);
  if (this->pipeWorkerToWorkerRequestIndicator.thePipe.lastRead.size == 0) {
    this->PauseIndicatorPipeInUse.ResumePausedProcessesIfAny(false, false);
    return;
  }
  this->pipeWorkerToWorkerIndicatorData.WriteOnceAfterEmptying(input, false, true);
  if (theGlobalVariables.flagTimedOutComputationIsDone) {
    this->PauseIndicatorPipeInUse.ResumePausedProcessesIfAny(false, false);
    return;
  }
  this->PauseIndicatorPipeInUse.ResumePausedProcessesIfAny(false, false);
}

int WebWorker::ProcessFolder() {
  MacroRegisterFunctionWithName("WebWorker::ProcessFolder");
  this->SetHeaderOKNoContentLength("");
  std::stringstream out;
  out << "<html><body>";
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
    out << "<hr>The virtual file name I extracted was: " << this->VirtualFileName
    << "<br>However, I do not allow folder names that contain dots. "
    << " Therefore I have sanitized the main address to: " << this->RelativePhysicalFileNamE;
  }
  List<std::string> theFileNames, theFileTypes;
  if (!FileOperations::GetFolderFileNamesUnsecure(this->RelativePhysicalFileNamE, theFileNames, &theFileTypes)) {
    out << "<b>Failed to open directory with physical address "
    << this->RelativePhysicalFileNamE << " </b></body></html>";
    stOutput << out.str();
    return 0;
  }
  out << "Browsing folder: " << this->addressGetOrPost << "<br>Virtual name: "
  << this->VirtualFileName << "<hr>";
  List<std::string> theFolderNamesHtml, theFileNamesHtml;
  for (int i = 0; i < theFileNames.size; i ++) {
    std::stringstream currentStream;
    bool isDir = (theFileTypes[i] == ".d");
    currentStream << "<a href=\"" << this->addressGetOrPost << HtmlRoutines::ConvertStringToURLString(theFileNames[i], false);
    if (isDir) {
      currentStream << "/";
    }
    currentStream << "\">" << theFileNames[i];
    if (isDir) {
      currentStream << "/";
    }
    currentStream << "</a><br>";
    if (isDir) {
      theFolderNamesHtml.AddOnTop(currentStream.str());
    } else {
      theFileNamesHtml.AddOnTop(currentStream.str());
    }
  }
  theFolderNamesHtml.QuickSortAscending();
  theFileNamesHtml.QuickSortAscending();
  for (int i = 0; i < theFolderNamesHtml.size; i ++) {
    out << theFolderNamesHtml[i];
  }
  for (int i = 0; i < theFileNamesHtml.size; i ++) {
    out << theFileNamesHtml[i];
  }
  out << "\n</body></html>";
  stOutput << out.str();
  return 0;
}

int WebWorker::ProcessFile() {
  MacroRegisterFunctionWithName("WebWorker::ProcessFile");
  if (!FileOperations::FileExistsUnsecure(this->RelativePhysicalFileNamE)) {
    this->SetHeadeR("HTTP/1.0 404 Object not found", "Content-Type: text/html");
    stOutput << "<html>"
    << HtmlRoutines::GetCSSLinkCalculator("/")
    << "<body>";
    stOutput << "One page <a href = \"" << theGlobalVariables.DisplayNameExecutableApp << "\">app</a>. ";
    stOutput << " Same app without browser cache: <a href = \""
    << theGlobalVariables.DisplayNameExecutableAppNoCache << "\">app no cache</a>.<hr>";
    stOutput << "<b>File does not exist.</b>";
    if (this->flagFileNameSanitized) {
      stOutput
      << "<hr>You requested virtual file: "
      << HtmlRoutines::ConvertStringToHtmlString(this->VirtualFileName, false)
      << "<br>However, I do not allow addresses that contain dots "
      << "(to avoid access to folders below the server). "
      << "Therefore I have sanitized the address to a relative physical address: "
      << HtmlRoutines::ConvertStringToHtmlString(this->RelativePhysicalFileNamE, false)
      << ".";
    }
    stOutput << "<br><b>Address:</b> "
    << HtmlRoutines::ConvertStringToHtmlString(this->addressGetOrPost, true)
    << "<br><b>Virtual file name:</b> "
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
  if (!FileOperations::OpenFileUnsecure(theFile, this->RelativePhysicalFileNamE, false, false, !isBinary)) {
    this->SetHeaderOKNoContentLength("");
    stOutput << "<html><body><b>Error: file appears to exist but I could not open it.</b> ";
    if (this->flagFileNameSanitized) {
      stOutput << "<hr>You requested virtual file: " << this->VirtualFileName
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
  if (fileSize > 100000000) {
    this->SetHeadeR("HTTP/1.0 413 Payload Too Large", "");
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
  for (int i = 0; i < this->parent->addressStartsSentWithCacheMaxAge.size; i ++) {
    if (MathRoutines::StringBeginsWith(this->VirtualFileName, this->parent->addressStartsSentWithCacheMaxAge[i])) {
      theHeader << WebAPI::HeaderCacheControl << "\r\n";
      withCacheHeader = true;
      break;
    }
  }
  std::stringstream debugBytesStream;
  if (theGlobalVariables.UserDebugFlagOn() && fileExtension == ".html") {
    debugBytesStream << "<!-- DEBUG info: ";
    if (withCacheHeader) {
      debugBytesStream << "Sent with Cache-Control header.\n ";
    }
    debugBytesStream << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable()
    << "-->";
  }
  if (theGlobalVariables.UserDebugFlagOn() && fileExtension == ".appcache") {
    debugBytesStream << "#" << this->GetMIMEtypeFromFileExtension(fileExtension);
  }
  if (theGlobalVariables.flagRunningApache) {
    this->SetHeaderOKNoContentLength("");
    stOutput << "<html><body>"
    << "<a href=\"" << this->VirtualFileName << "\">" << this->VirtualFileName << "</a>"
    << "</body></html>";
    stOutput << debugBytesStream.str();
    return 0;
  }
  unsigned int totalLength = fileSize + debugBytesStream.str().size();
  theHeader << "Content-length: " << totalLength << "\r\n";
  if (!this->flagKeepAlive) {
    theHeader << this->GetHeaderConnectionClose() << "\r\n";
  }
  //std::string theCookie = this->GetHeaderSetCookie();
  //if (theCookie != "")
  //  theHeader << theCookie << "\r\n";
  theHeader << "\r\n";
  this->QueueStringForSendingNoHeadeR(theHeader.str());
  if (this->requestTypE == this->requestHead) {
    this->SendAllBytesNoHeaders();
    this->flagAllBytesSentUsingFile = true;
    this->flagEncounteredErrorWhileServingFile = false;
    return 0;
  }
  const int bufferSize = 64 * 1024;
  this->bufferFileIO.SetSize(bufferSize);
  theFile.seekg(0);
  theFile.read(&this->bufferFileIO[0], this->bufferFileIO.size);
  int numBytesRead = theFile.gcount();
  while (numBytesRead != 0) {
    this->bufferFileIO.SetSize(numBytesRead);
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

void WebWorker::reset() {
  this->connectedSocketID = - 1;
  this->ProcessPID = - 1;
  this->connectedSocketIDLastValueBeforeRelease = - 1;
  this->connectionID = - 1;
  this->indexInParent = - 1;
  this->millisecondsAfterSelect = - 1;
  this->parent = 0;
  this->indentationLevelHTML = 0;
  this->displayUserInput = "";
  this->requestTypE = this->requestUnknown;
  this->flagToggleMonitoring = false;
  this->flagDoAddContentLength = false;
  this->flagFileNameSanitized = false;
  this->millisecondsLastPingServerSideOnly = - 1;
  this->millisecondsServerAtWorkerStart = - 1;
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

WebWorker::WebWorker() {
  this->flagDeallocated = false;
  this->reset();
}

bool WebWorker::IamActive() {
  if (this->parent == 0 || this->indexInParent == - 1) {
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
  if (theGlobalVariables.flagServerDetailedLog) {
    logIO << "Detail: wrapping up connection. " << logger::endL;
  }
  this->resultWork["connectionsServed"] = this->numberOfReceivesCurrentConnection;
  if (this->flagToggleMonitoring) {
    this->resultWork["result"] = "toggleMonitoring";
  } else {
    this->resultWork["result"] = "close";
  }
  if (theGlobalVariables.flagRestartNeeded) {
    this->resultWork["restartNeeded"] = "true";
  }
  if (theGlobalVariables.flagStopNeeded) {
    this->resultWork["stopNeeded"] = "true";
    logWorker << "DEBUG: stop needed is true";
  }
  this->pipeWorkerToServerControls.WriteOnceAfterEmptying(this->resultWork.ToString(false, false), false, false);
  if (theGlobalVariables.flagServerDetailedLog) {
    logIO << "Detail: done with pipes, releasing resources. " << logger::endL;
  }
  this->Release();
  if (theGlobalVariables.flagServerDetailedLog) {
    logIO << "Detail: released. " << logger::endL;
  }
  theGlobalVariables.flagComputationCompletE = true;
  theGlobalVariables.flagComputationFinishedAllOutputSentClosing = true;
}

void WebWorker::SendAllAndWrapUp() {
  MacroRegisterFunctionWithName("WebWorker::SendAllAndWrapUp");
  if (!this->IamActive()) {
    logWorker << logger::red << "Signal done called on non-active worker" << logger::endL;
    return;
  }
  this->SendAllBytesWithHeaders();
  this->WrapUpConnection();
}

WebWorker::~WebWorker() {
  //Workers are not allowed to release resources in the destructor:
  //a Worker's destructor is called when expanding List<WebWorker>.
  this->flagDeallocated = true;
}

std::string WebWorker::GetMIMEtypeFromFileExtension(const std::string& fileExtension) {
  MacroRegisterFunctionWithName("WebWorker::GetMIMEtypeFromFileExtension");
  if (fileExtension == ".html") {
    return "Content-Type: text/html\r\n";
  }
  if (fileExtension == ".php") {
    return "Content-Type: text/html\r\n";
  }
  if (fileExtension == ".txt") {
    return "Content-Type: text/plain\r\n";
  }
  if (fileExtension == ".png") {
    return "Content-Type: image/png\r\n";
  }
  if (fileExtension == ".js") {
    return "Content-Type: text/javascript\r\n";
  }
  if (fileExtension == ".ico") {
    return "Content-Type: image/x-icon\r\n";
  }
  if (fileExtension == ".css") {
    return "Content-Type: text/css\r\n";
  }
  if (fileExtension == ".eot") {
    return "Content-Type: application/vnd.ms-fontobject\r\n";
  }
  if (fileExtension == ".ttf") {
    return "Content-Type: application/octet-stream\r\n";
  }
  if (fileExtension == ".svg") {
    return "Content-Type: image/svg+xml\r\n";
  }
  if (fileExtension == ".appcache") {
    return "Content-Type: text/cache-manifest\r\n";
  }
  if (fileExtension == ".woff") {
    return "Content-Type: application/font-woff\r\n";
  }
  return "Content-Type: application/octet-stream\r\n";
}

int WebWorker::ProcessUnknown() {
  MacroRegisterFunctionWithName("WebWorker::ProcessUnknown");
  this->SetHeadeR("HTTP/1.0 501 Method Not Implemented", "Content-Type: text/html");
  stOutput << "<html><head>"
  << HtmlRoutines::GetCSSLinkCalculator("/")
  << "</head>";
  stOutput << "<body>";
  stOutput << "<calculatorNavigation>" << theGlobalVariables.ToStringNavigation()
  << "</calculatorNavigation>\n";
  stOutput << "<b>Requested method is not implemented. </b> "
  << "<hr>The original message received from the server follows."
  << "<hr>\n" << this->ToStringMessageUnsafe();

  if (this->requestTypE == this->requestChunked) {
    logHttpErrors << logger::red << "Chunked messaging not implemented. Message follows. " << logger::endL
    << this->ToStringMessageFullUnsafe() << logger::endL;
  } else {
    logHttpErrors << logger::red << "Method not implemented. Message follows. " << logger::endL
    << this->ToStringMessageFullUnsafe(false) << logger::endL;
  }
  stOutput << "</body></html>";
  return 0;
}

bool WebWorker::ShouldDisplayLoginPage() {
  if (theGlobalVariables.userCalculatorRequestType == "login") {
    return true;
  }
  if (
    theGlobalVariables.flagUsingSSLinCurrentConnection && !theGlobalVariables.flagLoggedIn &&
    theGlobalVariables.userCalculatorRequestType != "calculator"
  ) {
    return true;
  }
  if (theGlobalVariables.UserRequestMustBePromptedToLogInIfNotLoggedIn() && !theGlobalVariables.flagLoggedIn) {
    return true;
  }
  return false;
}

std::string WebWorker::GetAuthenticationToken(const std::string& reasonForNoAuthentication) {
  MacroRegisterFunctionWithName("WebWorker::GetAuthenticationToken");
  if (theGlobalVariables.flagLoggedIn && theGlobalVariables.flagUsingSSLinCurrentConnection) {
    return theGlobalVariables.userDefault.actualActivationToken;
  }
  return reasonForNoAuthentication;
}

std::string WebWorker::GetChangePasswordPagePartOne(bool& outputDoShowPasswordChangeField) {
  MacroRegisterFunctionWithName("WebWorker::GetChangePasswordPagePartOne");
  std::stringstream out;
  std::string claimedActivationToken =
  HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("activationToken"), false);
  std::string claimedEmail =
  HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("email"), false);
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
  if (theGlobalVariables.userDefault.email == claimedEmail) {
    out << "\n<b><span style =\"color:green\">Email " << claimedEmail << " updated. </span></b>";
    return out.str();
  }
  JSData findEmail, emailInfo, findUser, userInfo;
  findEmail[DatabaseStrings::labelEmail] = claimedEmail;
  if (!DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSON(
    DatabaseStrings::tableEmailInfo, findEmail, emailInfo, &out, true
  )) {
    out << "\n<span style =\"color:red\"><b>Failed to fetch email activation token for email: "
    << claimedEmail << " </b></span>";
    return out.str();
  }
  usernameAssociatedWithToken = emailInfo[DatabaseStrings::labelUsernameAssociatedWithToken].theString;
  actualEmailActivationToken = emailInfo[DatabaseStrings::labelActivationToken].theString;
  if (actualEmailActivationToken != claimedActivationToken) {
    out << "\n<span style =\"color:red\"><b>Bad activation token. Could not activate your email. </b></span>";
    return out.str();
  }
  if (usernameAssociatedWithToken != theGlobalVariables.userDefault.username) {
    out << "\n<span style =\"color:red\"><b>Activation token was issued for another user. </b></span>";
    return out.str();
  }
  emailInfo[DatabaseStrings::labelActivationToken] = "";
  if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON(
    DatabaseStrings::tableEmailInfo, findEmail, emailInfo, 0, &out
  )) {
    out << "\n<span style =\"color:red\"><b>Could not reset the activation token (database is down?). </b></span>";
    return out.str();
  }
  userInfo[DatabaseStrings::labelEmail] = claimedEmail;
  findUser[DatabaseStrings::labelUsername] = theGlobalVariables.userDefault.username;
  if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON(
    DatabaseStrings::tableUsers, findUser, userInfo, 0, &out
  )) {
    out << "\n<span style =\"color:red\"><b>Could not store your email (database is down?). </b></span>";
    return out.str();
  }
  theGlobalVariables.userDefault.email = claimedEmail;
  out << "\n<span style =\"color:green\"><b>Email successfully updated. </b></span>";
  if (
    theGlobalVariables.userDefault.actualActivationToken != "" &&
    theGlobalVariables.userDefault.actualActivationToken != "activated" &&
    theGlobalVariables.userDefault.actualHashedSaltedPassword != ""
  ) {
    out << "<br>It appears your password is already set. "
    << "<br>If you'd like to change it using your old password, "
    << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=changePasswordPage\">click here</a>. ";
    outputDoShowPasswordChangeField = false;
    userInfo[DatabaseStrings::labelActivationToken] = "activated";
  } else {
    out << "<br>"
    << "<span style =\"color:orange\">To fully activate your account, please choose a password.</span></b>";
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
  if (!theGlobalVariables.flagDatabaseCompiled) {
    if (commentsOnFailure != 0) {
      *commentsOnFailure << "Project compiled without database support. ";
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
  if (commentsGeneralNonSensitive != 0) {
    *commentsGeneralNonSensitive << "<br><b>Sending email... </b>";
  }
  theEmail.SendEmailWithMailGun(commentsOnFailure, commentsGeneralNonSensitive, commentsGeneralSensitive);
  if (commentsGeneralSensitive != 0) {
    if (theGlobalVariables.UserDefaultHasAdminRights()) {
      *commentsGeneralSensitive << "<hr>Content of sent email (admin view only):<br>"
      << HtmlRoutines::ConvertStringToHtmlString(theEmail.emailContent, true);
    }
  } else {
    if (commentsGeneralNonSensitive != 0) {
      *commentsGeneralNonSensitive << "Email content not displayed. ";
    }
  }
  userCopy.clearAuthenticationTokenAndPassword();
  userCopy.actualActivationToken = "";
  inputOutputUser = userCopy;
  return true;
}

int WebWorker::SetEmail(const std::string& input) {
  MacroRegisterFunctionWithName("WebWorker::SetEmail");
  (void) input;
  if (!theGlobalVariables.flagDatabaseCompiled) {
    stOutput << "Database not available (cannot set email). ";
    return 0;
  }
  std::stringstream out, debugStream;
  //double startTime = theGlobalVariables.GetElapsedSeconds();
  theGlobalVariables.userDefault.email = input;
  std::stringstream* adminOutputStream = 0;
  if (theGlobalVariables.UserDefaultHasAdminRights()) {
    adminOutputStream = &out;
  }
  this->DoSetEmail(theGlobalVariables.userDefault, &out, &out, adminOutputStream);
  stOutput << out.str();
  if (theGlobalVariables.UserDefaultHasAdminRights()) {
    stOutput << "<hr><b>Admin view only. </b>" << debugStream.str();
  }
  stOutput << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds() << " second(s).";
  return 0;
}

int WebWorker::ProcessChangePassword(const std::string& reasonForNoAuthentication) {
  MacroRegisterFunctionWithName("WebWorker::ProcessChangePassword");
  (void) reasonForNoAuthentication;
  this->SetHeaderOKNoContentLength("");
  if (! theGlobalVariables.flagDatabaseCompiled) {
    stOutput << "Operation not possible: project compiled without database support.";
    return 0;
  }
  UserCalculatorData& theUser = theGlobalVariables.userDefault;
  theUser.enteredAuthenticationToken = "";
  if (!theGlobalVariables.flagUsingSSLinCurrentConnection) {
    stOutput << "<b style =\"color:red\">Please use secure connection.</b>";
    return 0;
  }
  if (!theGlobalVariables.flagLoggedIn) {
    stOutput << "<b style =\"color:red\">Please enter (old) password.</b> " << reasonForNoAuthentication;
    return 0;
  }
  std::string newPassword = HtmlRoutines::ConvertStringToURLString(
    HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("newPassword"), true),
    false
  );
  //<-Passwords are ONE-LAYER url-encoded
  //<-INCOMING pluses in passwords MUST be decoded as spaces, this is how form.submit() works!
  //<-Incoming pluses must be re-coded as spaces (%20).

  std::string reenteredPassword = HtmlRoutines::ConvertStringToURLString(
    HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("reenteredPassword"), true),
    false
  );
  //<-Passwords are ONE-LAYER url-encoded
  //<-INCOMING pluses in passwords MUST be decoded as spaces, this is how form.submit() works!
  //<-Incoming pluses must be re-coded as spaces (%20).

  std::string newEmail = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("email"), false);
  if (newEmail != "") {
    JSData queryEmailTaken, notUsed;
    queryEmailTaken[DatabaseStrings::labelEmail] = newEmail;
    if (DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSON(
      DatabaseStrings::tableUsers,
      queryEmailTaken, notUsed, 0, true
    )) {
      stOutput << "<b style =\"color:red\">It appears the email is already taken. </b>";
      return 0;
    }
  }
  if (newPassword == "" && reenteredPassword == "" && newEmail != "") {
    return this->SetEmail(newEmail);
  }
  if (newPassword != reenteredPassword) {
    stOutput << "<b style =\"color:red\">Passwords don't match. </b>";
    return 0;
  }
  std::stringstream commentsOnFailure;
  std::string newAuthenticationToken;
  if (!DatabaseRoutinesGlobalFunctions::SetPassword(
    theUser.username,
    newPassword,
    newAuthenticationToken,
    commentsOnFailure
  )) {
    stOutput << "<b style =\"color:red\">" << commentsOnFailure.str() << "</b>";
    return 0;
  }
  JSData findQuery, setQuery;
  findQuery[DatabaseStrings::labelUsername] = theUser.username;
  setQuery[DatabaseStrings::labelActivationToken] = "activated";
  if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON(
    DatabaseStrings::tableUsers, findQuery, setQuery, 0, &commentsOnFailure
  )) {
    stOutput << "<b style = \"color:red\">Failed to set activationToken: "
    << commentsOnFailure.str() << "</b>";
  }
  stOutput << "<b style = \"color:green\">Password change successful. </b>";
  if (theGlobalVariables.GetWebInput("doReload") != "false") {
    stOutput
    << "<meta http-equiv=\"refresh\" content =\"0; url ='"
    << theGlobalVariables.DisplayNameExecutable  << "?request=logout"
    << "&username="
    << HtmlRoutines::ConvertStringToURLString(theGlobalVariables.userDefault.username, false)
    << "&activationToken = &authenticationToken = &"
    << "'\" />";
  }
  return 0;
}

int WebWorker::ProcessCompute() {
  MacroRegisterFunctionWithName("WebWorker::ProcessCompute");
  this->SetHeaderOKNoContentLength("");
  //theParser.initComputationStats();
  theParser->inputString = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("mainInput"), false);
  theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection = WebServer::ReturnActiveIndicatorAlthoughComputationIsNotDone;
  theGlobalVariables.initOutputReportAndCrashFileNames(
    HtmlRoutines::ConvertStringToURLString(theParser->inputString, false), theParser->inputString
  );
  ////////////////////////////////////////////////
  //  the initialization below moved to the start of the web server!
  //  theParser.init();
  ////////////////////////////////////////////////
  this->flagProgressReportAllowed = true;
  theParser->Evaluate(theParser->inputString);
  this->flagProgressReportAllowed = false;
  if (theGlobalVariables.flagRunningBuiltInWebServer) {
    if (theGlobalVariables.flagOutputTimedOut) {
      this->OutputResultAfterTimeout();
      return 0;
    }
  }
  JSData result;
  result = theParser->ToJSONOutputAndSpecials();
  if (theGlobalVariables.UserDebugFlagOn()) {
    result["debug"] = this->ToStringMessageFullUnsafe();
  }
  stOutput << result.ToString(false);
  theGlobalVariables.flagComputationCompletE = true;
  return 0;
}

int WebWorker::ProcessActivateAccount() {
  MacroRegisterFunctionWithName("WebWorker::ProcessActivateAccount");
  this->SetHeaderOKNoContentLength("");
  bool doShowDetails = true;
  stOutput << WebWorker::GetChangePasswordPagePartOne(doShowDetails);
  return 0;
}

int WebWorker::ProcessLogout() {
  MacroRegisterFunctionWithName("WebWorker::ProcessLogout");
  this->SetHeaderOKNoContentLength("");
  DatabaseRoutinesGlobalFunctions::LogoutViaDatabase();
  return this->ProcessLoginUserInfo("Coming from logout");
}

int WebWorker::ProcessSelectCourseJSON() {
  MacroRegisterFunctionWithName("WebWorker::ProcessSelectCourseJSON");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetSelectCourseJSON();
  return 0;
}

int WebWorker::ProcessTopicListJSON() {
  MacroRegisterFunctionWithName("WebWorker::ProcessTopicListJSON");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetTopicTableJSON();
  return 0;
}

int WebWorker::ProcessCalculatorOnePageJS(bool appendBuildHash) {
  MacroRegisterFunctionWithName("WebWorker::ProcessCalculatorOnePageJS");
  if (appendBuildHash) {
    this->SetHeaderOKNoContentLength(WebAPI::HeaderCacheControl, "text/javascript");
  } else {
    this->SetHeaderOKNoContentLength("", "text/javascript");
  }
  stOutput << HtmlInterpretation::GetOnePageJS(appendBuildHash);
  return 0;
}

int WebWorker::ProcessApp(bool appendBuildHash) {
  MacroRegisterFunctionWithName("WebWorker::ProcessApp");
  this->SetHeaderOKNoContentLength("");
  if (theWebServer.RestartIsNeeded()) {
    return 0;
  }
  stOutput << HtmlInterpretation::GetApp(appendBuildHash);
  return 0;
}

int WebWorker::ProcessAbout() {
  MacroRegisterFunctionWithName("WebWorker::ProcessAbout");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetAboutPage();
  return 0;
}

int WebWorker::ProcessTemplate() {
  MacroRegisterFunctionWithName("WebWorker::ProcessTemplate");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetPageFromTemplate();
  return 0;
}

int WebWorker::ProcessLoginUserInfo(const std::string& comments) {
  MacroRegisterFunctionWithName("WebWorker::ProcessUserInfoJSON");
  this->SetHeaderOKNoContentLength("");
  if (theWebServer.RestartIsNeeded()) {
    return 0;
  }
  stOutput << HtmlInterpretation::GetJSONUserInfo(comments);
  return 0;
}

int WebWorker::ProcessTemplateJSON() {
  MacroRegisterFunctionWithName("WebWorker::ProcessTemplateJSON");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetJSONFromTemplate();
  return 0;
}

int WebWorker::ProcessExamPageJSON() {
  MacroRegisterFunctionWithName("WebWorker::ProcessExamPageJSON");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetExamPageJSON();
  return 0;
}

int WebWorker::ProcessNavigation() {
  MacroRegisterFunctionWithName("WebWorker::ProcessNavigation");
  this->SetHeaderOKNoContentLength("");
  stOutput << this->GetNavigationPage();
  return 0;
}

int WebWorker::ProcessGetAuthenticationToken(const std::string& reasonForNoAuthentication) {
  MacroRegisterFunctionWithName("WebWorker::ProcessGetAuthenticationToken");
  this->SetHeaderOKNoContentLength("");
  stOutput << this->GetAuthenticationToken(reasonForNoAuthentication);
  return 0;
}

int WebWorker::ProcessSetProblemDatabaseInfo() {
  MacroRegisterFunctionWithName("WebWorker::ProcessSetProblemDatabaseInfo");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetSetProblemDatabaseInfoHtml();
  return 0;
}

int WebWorker::ProcessAddUserEmails() {
  MacroRegisterFunctionWithName("WebWorker::ProcessAddUserEmails");
  this->SetHeaderOKNoContentLength("");
  stOutput << this->GetAddUserEmails();
  return 0;
}

int WebWorker::ProcessModifyPage() {
  MacroRegisterFunctionWithName("WebWorker::ProcessModifyPage");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::ModifyProblemReport();
  return 0;
}

int WebWorker::ProcessAssignTeacherToSection() {
  MacroRegisterFunctionWithName("WebWorker::ProcessAssignTeacherToSection");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::AddTeachersSections();
  return 0;
}

int WebWorker::ProcessScores() {
  MacroRegisterFunctionWithName("WebWorker::ProcessScores");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetScoresPage();
  return 0;
}

int WebWorker::ProcessScoresInCoursePage() {
  MacroRegisterFunctionWithName("WebWorker::ProcessScoresInCoursePage");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetScoresInCoursePage();
  return 0;
}

int WebWorker::ProcessAccounts() {
  MacroRegisterFunctionWithName("WebWorker::ProcessAccounts");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetAccountsPage(this->hostWithPort);
  return 0;
}

int WebWorker::ProcessAccountsJSON() {
  MacroRegisterFunctionWithName("WebWorker::ProcessAccountsJSON");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetAccountsPageJSON(this->hostWithPort);
  return 0;
}

int WebWorker::ProcessDatabaseJSON() {
  MacroRegisterFunctionWithName("WebWorker::ProcessDatabaseJSON");
  this->SetHeaderOKNoContentLength("");
  stOutput << this->GetDatabaseJSON();
  return 0;
}

int WebWorker::ProcessDatabaseDeleteEntry() {
  MacroRegisterFunctionWithName("WebWorker::ProcessDatabaseDeleteEntry");
  this->SetHeaderOKNoContentLength("");
  stOutput << this->GetDatabaseDeleteOneItem();
  return 0;
}

int WebWorker::ProcessDatabaseModifyEntry() {
  MacroRegisterFunctionWithName("WebWorker::ProcessDatabaseModifyEntry");
  this->SetHeaderOKNoContentLength("");
  stOutput << "Not implemented yet";
  return 0;
}

int WebWorker::ProcessBrowseProblems() {
  MacroRegisterFunctionWithName("WebWorker::ProcessBrowseProblems");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetBrowseProblems();
  return 0;
}

int WebWorker::ProcessEditPageJSON() {
  MacroRegisterFunctionWithName("WebWorker::ProcessEditPageJSON");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetEditPageJSON();
  return 0;
}

#include "vpfHeader2Math10_LaTeXRoutines.h"
int WebWorker::ProcessSlidesOrHomeworkFromSource() {
  MacroRegisterFunctionWithName("WebWorker::ProcessSlidesOrHomeworkFromSource");
  this->SetHeaderOKNoContentLength("");
  LaTeXcrawler theCrawler;
  for (int i = 0; i < theGlobalVariables.webArguments.size(); i ++) {
    std::string theKey = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.webArguments.theKeys[i], false);
    if (theKey != "fileName" && MathRoutines::StringBeginsWith(theKey, "file")) {
      theCrawler.slideFileNamesVirtualWithPatH.AddOnTop(MathRoutines::StringTrimWhiteSpace(
        HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.webArguments.theValues[i], false)
      ));
    }
    if (MathRoutines::StringBeginsWith(theKey, "isSolutionFile")) {
      theCrawler.slideFilesExtraFlags.AddOnTop(MathRoutines::StringTrimWhiteSpace(
        HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.webArguments.theValues[i], false)
      ));
    }
  }
  if (theCrawler.slideFilesExtraFlags.size > theCrawler.slideFileNamesVirtualWithPatH.size) {
    theCrawler.slideFilesExtraFlags.SetSize(theCrawler.slideFileNamesVirtualWithPatH.size);
  } else {
    for (int i = theCrawler.slideFilesExtraFlags.size; i < theCrawler.slideFileNamesVirtualWithPatH.size; i ++) {
      theCrawler.slideFilesExtraFlags.AddOnTop("");
    }
  }
  theCrawler.desiredPresentationTitle =
  HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("title"), false);
  std::stringstream comments;
  if (theGlobalVariables.GetWebInput("layout") == "printable") {
    theCrawler.flagProjectorMode = false;
  }
  if (theGlobalVariables.GetWebInput("answerKey") == "true") {
    theCrawler.flagAnswerKey = true;
  }
  if (
    theGlobalVariables.userCalculatorRequestType == "homeworkFromSource" ||
    theGlobalVariables.userCalculatorRequestType == "homeworkSource"
  ) {
    theCrawler.flagHomeworkRatherThanSlides = true;
  }
  if (!theCrawler.BuildOrFetchFromCachePDF(&comments, &comments)) {
    JSData result;
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

int WebWorker::ProcessSlidesSource() {
  MacroRegisterFunctionWithName("WebWorker::ProcessSlidesSource");
  this->SetHeaderOKNoContentLength("");
  LaTeXcrawler theCrawler;
  for (int i = 0; i < theGlobalVariables.webArguments.size(); i ++) {
    std::string theKey = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.webArguments.theKeys[i], false);
    if (
      theKey != "fileName" && MathRoutines::StringBeginsWith(theKey, "file")
    ) {
      theCrawler.slideFileNamesVirtualWithPatH.AddOnTop(MathRoutines::StringTrimWhiteSpace(
        HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.webArguments.theValues[i], false)
      ));
    }
  }
  if (theGlobalVariables.userCalculatorRequestType == "homeworkSource") {
    theCrawler.flagHomeworkRatherThanSlides = true;
  } else {
    theCrawler.flagHomeworkRatherThanSlides = false;
  }
  theCrawler.desiredPresentationTitle =
  HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("title"), false);
  std::stringstream comments;
  if (theGlobalVariables.GetWebInput("layout") == "printable") {
    theCrawler.flagProjectorMode = false;
  }
  if (theGlobalVariables.GetWebInput("answerKey") == "false") {
    theCrawler.flagAnswerKey = false;
  } else {
    theCrawler.flagAnswerKey = true;
  }
  theCrawler.flagSourceOnly = true;
  theCrawler.flagCrawlTexSourcesRecursively = true;
  if (!theCrawler.BuildOrFetchFromCachePDF(&comments, &comments)) {
    this->flagDoAddContentLength = true;
    stOutput << "<!DOCTYPE html>"
    << "<html>"
    << "<head>"
    << HtmlRoutines::GetCSSLinkCalculator("/")
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

int WebWorker::ProcessClonePage() {
  MacroRegisterFunctionWithName("WebWorker::ProcessClonePage");
  this->SetHeaderOKNoContentLength("");
  stOutput << this->GetClonePageResult();
  return 0;
}

int WebWorker::ProcessProblemGiveUp() {
  MacroRegisterFunctionWithName("WebWorker::ProcessProblemGiveUp");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetAnswerOnGiveUp();
  return 0;
}

int WebWorker::ProcessProblemSolution() {
  MacroRegisterFunctionWithName("WebWorker::ProcessProblemSolution");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::GetProblemSolutionString();
  return 0;
}

int WebWorker::ProcessSubmitAnswersPreview() {
  MacroRegisterFunctionWithName("WebWorker::ProcessSubmitAnswersPreview");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::submitAnswersPreviewString();
  return 0;
}

std::string WebWorker::GetClonePageResult() {
  MacroRegisterFunctionWithName("WebWorker::GetClonePageResult");
  this->SetHeaderOKNoContentLength("");
  return HtmlInterpretation::ClonePageResult();
}

std::string WebWorker::GetAddUserEmails() {
  MacroRegisterFunctionWithName("WebWorker::GetAddUserEmails");
  return HtmlInterpretation::AddUserEmails(this->hostWithPort);
}

std::string HtmlInterpretation::ModifyProblemReport() {
  MacroRegisterFunctionWithName("WebWorker::ModifyProblemReport");
  bool shouldProceed = theGlobalVariables.flagLoggedIn && theGlobalVariables.UserDefaultHasAdminRights();
  if (shouldProceed) {
    shouldProceed = theGlobalVariables.flagUsingSSLinCurrentConnection;
  }
  if (!shouldProceed) {
    return "<b>Modifying problems allowed only for logged-in admins under ssl connection. </b>";
  }
  std::string mainInput = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("mainInput"), false);
  std::string fileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("fileName"), false);
  std::stringstream commentsOnFailure;
  bool fileExists = FileOperations::FileExistsVirtualCustomizedReadOnly(fileName, &commentsOnFailure);
  std::fstream theFile;
  if (!FileOperations::OpenFileVirtualCustomizedWriteOnly(theFile, fileName, false, true, false, &commentsOnFailure)) {
    commentsOnFailure << "<b style =\"color:red\">Failed to open/create file: " << fileName << ". </b>";
    return commentsOnFailure.str();
  }
  theFile << mainInput;
  theFile.close();
  std::stringstream out;
  if (!fileExists) {
    out << "File " << fileName << " didn't previously exist: just created it for you. ";
  }
  out << "<b style =\"color:green\"> Written content to file: "
  << fileName << ". </b>";
  return out.str();
}

int WebWorker::ProcessSubmitAnswers() {
  MacroRegisterFunctionWithName("WebWorker::ProcessSubmitAnswers");
  this->SetHeaderOKNoContentLength("");
  stOutput << HtmlInterpretation::SubmitAnswersString();
  return 0;
}

std::string HtmlInterpretation::GetJavascriptCaptcha() {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetCaptchaJavascript");
  std::stringstream out;
  out << "<script src = 'https://www.google.com/recaptcha/api.js'></script>";
  return out.str();
}

std::string HtmlInterpretation::GetCaptchaDiv() {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetCaptchaDiv");
  std::stringstream out;
  std::string recaptchaPublic;
  if (!FileOperations::LoadFileToStringVirtual("certificates/recaptcha-public.txt", recaptchaPublic, true, true, &out)) {
    out << "<span style =\"color:red\"><b>Couldn't find the recaptcha key in file: "
    << "certificates/recaptcha-public.txt</b></span>";
  } else {
    out << "<div class =\"g-recaptcha\" data-sitekey =\"" << recaptchaPublic << "\"></div>";
  }
  return out.str();
}

bool WebWorker::CorrectRequestsBEFORELoginReturnFalseIfModified() {
  MacroRegisterFunctionWithName("WebWorker::CorrectRequestsBEFORELoginReturnFalseIfModified");
  bool stateNotModified = true;
  if (!theGlobalVariables.flagSSLIsAvailable) {
    if (
      this->addressComputed == theGlobalVariables.DisplayNameExecutable &&
      theGlobalVariables.userCalculatorRequestType == ""
    ) {
      this->addressComputed = theGlobalVariables.DisplayNameExecutable;
      theGlobalVariables.userCalculatorRequestType = "calculator";
      stateNotModified = false;
    }
  }
  if (this->addressComputed == "/" || this->addressComputed == "") {
    this->addressComputed = theGlobalVariables.DisplayNameExecutableApp; //was: theGlobalVariables.DisplayNameExecutable;
    theGlobalVariables.userCalculatorRequestType = WebAPI::app;
    //theGlobalVariables.userCalculatorRequestType = "selectCourse";
    stateNotModified = false;
  }
  return stateNotModified;
}

bool WebWorker::RedirectIfNeeded(std::stringstream& argumentProcessingFailureComments) {
  MacroRegisterFunctionWithName("WebWorker::RedirectIfNeeded");
  UserCalculatorData& theUser = theGlobalVariables.userDefault;
  if (!theUser.flagEnteredPassword) {
    return false;
  }
  if (
    theGlobalVariables.userCalculatorRequestType == "changePassword" ||
    theGlobalVariables.userCalculatorRequestType == WebAPI::request::activateAccountJSON
  ) {
    return false;
  }
  std::stringstream redirectedAddress;
  if (this->addressComputed == theGlobalVariables.DisplayNameExecutable) {
    redirectedAddress << theGlobalVariables.DisplayNameExecutable << "?request="
    << theGlobalVariables.userCalculatorRequestType << "&";
  } else {
    redirectedAddress << this->addressComputed << "?";
  }
  for (int i = 0; i < theGlobalVariables.webArguments.size(); i ++) {
    if (
      theGlobalVariables.webArguments.theKeys[i] != "password" &&
      theGlobalVariables.webArguments.theKeys[i] != "request" &&
      theGlobalVariables.webArguments.theKeys[i] != "googleToken" &&
      theGlobalVariables.webArguments.theKeys[i] != "G_AUTHUSER_H" &&
      theGlobalVariables.webArguments.theKeys[i] != "activationToken"
    ) {
      redirectedAddress << theGlobalVariables.webArguments.theKeys[i] << "="
      << theGlobalVariables.webArguments.theValues[i] << "&";
    }
  }
  if (argumentProcessingFailureComments.str() != "") {
    redirectedAddress << "error="
    << HtmlRoutines::ConvertStringToURLString(argumentProcessingFailureComments.str(), false)
    << "&";
  }
  std::stringstream headerStream;
  headerStream << "Location: " << redirectedAddress.str();
  this->SetHeadeR("HTTP/1.0 303 See other", headerStream.str());
  stOutput << "<html><head>"
  << "<meta http-equiv=\"refresh\" content =\"0; url ='" << redirectedAddress.str()
  << "'\" />"
  << "</head>";
  stOutput << "<body>Click <a href=\"" << redirectedAddress.str() << "\">"
  << " here " << "</a> if your browser does not redirect the page automatically. ";
  stOutput << "</body></html>";
  return true;
}

bool WebWorker::CorrectRequestsAFTERLoginReturnFalseIfModified() {
  MacroRegisterFunctionWithName("WebWorker::CorrectRequestsAFTERLoginReturnFalseIfModified");
  bool stateNotModified = true;
  bool shouldFallBackToDefaultPage = false;
  if (this->addressComputed == "/" || this->addressComputed == "") {
    shouldFallBackToDefaultPage = true;
  }
  if (!theGlobalVariables.flagSSLIsAvailable) {
    if (
      this->addressComputed == theGlobalVariables.DisplayNameExecutable &&
      theGlobalVariables.userCalculatorRequestType == ""
    ) {
      this->addressComputed = theGlobalVariables.DisplayNameExecutable;
      theGlobalVariables.userCalculatorRequestType = WebAPI::app;
      stateNotModified = false;
    }
  }
  if (theGlobalVariables.flagLoggedIn) {
    if (
      this->addressComputed == theGlobalVariables.DisplayNameExecutable &&
      theGlobalVariables.userCalculatorRequestType == ""
    ) {
      shouldFallBackToDefaultPage = true;
    } else if (
      theGlobalVariables.userCalculatorRequestType == "template" &&
      theGlobalVariables.GetWebInput("courseHome") == ""
    ) {
      shouldFallBackToDefaultPage = true;
    }
  }
  if (shouldFallBackToDefaultPage) {
    this->addressComputed = theGlobalVariables.DisplayNameExecutableApp;
    theGlobalVariables.userCalculatorRequestType = WebAPI::app;
    stateNotModified = false;
  }
  return stateNotModified;
}

bool WebWorker::ProcessRedirectAwayFromWWW() {
  std::string addressNoWWW;
  if (!MathRoutines::StringBeginsWith(theGlobalVariables.hostWithPort, "www.", &addressNoWWW)) {
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
  if (theGlobalVariables.flagRunningApache) {
    redirectHeaderStream << "Content-Type: text/html\r\n";
  }
  redirectHeaderStream << "Location: " << newAddressStream.str();
  this->SetHeadeR("HTTP/1.0 301 Moved Permanently", redirectHeaderStream.str());
  //this->SetHeaderOKNoContentLength();
  stOutput << "<html><body>Please remove the www. from the address, it creates issues with authentication services. "
  << "Click <a href=\"" << newAddressStream.str() << "\">here</a> if not redirected automatically. ";
  stOutput << "</body></html>";
  return true;
}

int WebWorker::ProcessLoginNeededOverUnsecureConnection() {
  MacroRegisterFunctionWithName("WebWorker::ProcessLoginNeededOverUnsecureConnection");
  std::stringstream redirectStream, newAddressStream;
  newAddressStream << "https://" << this->hostNoPort;
  if (this->hostNoPort == "") {
    newAddressStream << "calculator-algebra.org";
  }
  newAddressStream << ":" << this->parent->httpSSLPort;
  if (this->addressGetOrPost.size() != 0) {
    if (this->addressGetOrPost[0] != '/') {
      this->addressGetOrPost = '/' + this->addressGetOrPost;
    }
  }
  newAddressStream << this->addressGetOrPost;
  if (theGlobalVariables.flagRunningApache) {
    redirectStream << "Content-Type: text/html\r\n";
  }
  if (this->hostNoPort != "") {
    redirectStream << "Location: " << newAddressStream.str();
    this->SetHeadeR("HTTP/1.0 301 Moved Permanently", redirectStream.str());
    stOutput << "<html><body>Address available through secure (SSL) connection only. "
    << "Click <a href=\"" << newAddressStream.str() << "\">here</a> if not redirected automatically. ";
  } else {
    this->SetHeaderOKNoContentLength("");
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

bool WebWorker::RequireSSL() {
  return theGlobalVariables.flagSSLIsAvailable;
}

int WebWorker::ServeClient() {
  MacroRegisterFunctionWithName("WebWorker::ServeClient");
  theGlobalVariables.millisecondsComputationStart = theGlobalVariables.GetElapsedMilliseconds();
  theGlobalVariables.flagComputationStarted = true;
  theGlobalVariables.flagComputationCompletE = false;
  theGlobalVariables.userDefault.flagMustLogin = true;
  theGlobalVariables.userDefault.flagStopIfNoLogin = true;
  UserCalculatorData& theUser = theGlobalVariables.userDefault;
  theGlobalVariables.IndicatorStringOutputFunction = WebServer::PipeProgressReport;
  if (
    this->requestTypE != this->requestGet &&
    this->requestTypE != this->requestPost &&
    this->requestTypE != this->requestHead
  ) {
    if (
      this->requestTypE != this->requestUnknown &&
      this->requestTypE != this->requestChunked
    ) {
      crash << "Something is wrong: request type does not have any of the expected values. " << crash;
    }
    this->ProcessUnknown();
    return 0;
  }
  if (theGlobalVariables.theThreads.size <= 1) {
    crash << "Number of threads must be at least 2 in this point of code..." << crash;
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
  theGlobalVariables.userCalculatorRequestType = "";
  if (this->addressComputed == theGlobalVariables.DisplayNameExecutable) {
    theGlobalVariables.userCalculatorRequestType = theGlobalVariables.GetWebInput("request");
  }
  std::stringstream comments;
  comments << "Address, request computed: ";
  if (this->addressComputed != "") {
    comments << this->addressComputed;
  } else {
    comments << "[empty]";
  }
  comments << ", ";
  if (theGlobalVariables.userCalculatorRequestType != "") {
    comments << theGlobalVariables.userCalculatorRequestType;
  } else {
    comments << "[empty]";
  }
  comments << ". ";
  bool isNotModified = this->CorrectRequestsBEFORELoginReturnFalseIfModified();
  if (!isNotModified) {
    comments << this->ToStringAddressRequest() << ": modified before login. ";
  }
  if (theWebServer.addressStartsInterpretedAsCalculatorRequest.Contains(this->addressComputed)) {
    theGlobalVariables.userCalculatorRequestType = this->addressComputed;
    std::string correctedRequest;
    if (MathRoutines::StringBeginsWith(theGlobalVariables.userCalculatorRequestType, "/", &correctedRequest)) {
      theGlobalVariables.userCalculatorRequestType = correctedRequest;
      comments << "Address was interpretted as request, so your request was set to: "
      << theGlobalVariables.userCalculatorRequestType << ". ";
    }
  }
  theUser.flagMustLogin = this->parent->RequiresLogin(theGlobalVariables.userCalculatorRequestType, this->addressComputed);
  if (!theUser.flagMustLogin) {
    comments << "Login not needed. ";
  }
  if (this->RequireSSL() && !theGlobalVariables.flagUsingSSLinCurrentConnection) {
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
    !theGlobalVariables.flagLoggedIn &&
    theGlobalVariables.flagUsingSSLinCurrentConnection &&
    theUser.flagMustLogin
  );
  if (theUser.flagStopIfNoLogin) {
    if (theGlobalVariables.userCalculatorRequestType == "changePassword") {
      theUser.flagStopIfNoLogin = false;
    }
  }
  if (theUser.flagStopIfNoLogin) {
    if (
      theGlobalVariables.userCalculatorRequestType != "logout" &&
      theGlobalVariables.userCalculatorRequestType != "login"
    ) {
      argumentProcessingFailureComments << this->ToStringAddressRequest() << " requires login. ";
    }
    argumentProcessingFailureComments << comments.str();
    theGlobalVariables.CookiesToSetUsingHeaders.SetKeyValue("authenticationToken", "");
    if (argumentProcessingFailureComments.str() != "") {
      theGlobalVariables.SetWebInpuT("authenticationToken", "");
    }
    return this->ProcessLoginUserInfo(argumentProcessingFailureComments.str());
  }
  if (
    argumentProcessingFailureComments.str() != "" && (
      theUser.flagMustLogin ||
      theGlobalVariables.userCalculatorRequestType == WebAPI::request::userInfoJSON
    )
  ) {
    theGlobalVariables.SetWebInpuT("error", argumentProcessingFailureComments.str());
  }
  if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.flagLoggedIn) {
    if (theGlobalVariables.GetWebInput("spoofHostName") != "") {
      theGlobalVariables.hostNoPort = HtmlRoutines::ConvertURLStringToNormal(
        theGlobalVariables.GetWebInput("spoofHostName"), false
      );
      theGlobalVariables.CookiesToSetUsingHeaders.SetKeyValue("spoofHostName", theGlobalVariables.hostNoPort);
    }
  }
  if (
    theGlobalVariables.flagLoggedIn && theGlobalVariables.UserDefaultHasAdminRights() &&
    theGlobalVariables.userCalculatorRequestType == WebAPI::databaseParameters::entryPoint
  ) {
    return this->ProcessDatabaseJSON();
  } else if (
    theGlobalVariables.flagLoggedIn &&
    theGlobalVariables.userCalculatorRequestType == "databaseDeleteOneEntry"
  ) {
    return this->ProcessDatabaseDeleteEntry();
  } else if (
    theGlobalVariables.flagLoggedIn && theGlobalVariables.UserDefaultHasAdminRights() &&
    theGlobalVariables.userCalculatorRequestType == "databaseModifyEntry"
  ) {
    return this->ProcessDatabaseModifyEntry();
  } else if (theGlobalVariables.flagLoggedIn && theGlobalVariables.userCalculatorRequestType == "accounts") {
    return this->ProcessAccounts();
  } else if (theGlobalVariables.flagLoggedIn && theGlobalVariables.userCalculatorRequestType == "accountsJSON") {
    return this->ProcessAccountsJSON();
  } else if (
    theGlobalVariables.flagLoggedIn && theGlobalVariables.UserDefaultHasAdminRights() &&
    theGlobalVariables.userCalculatorRequestType == "navigation"
  ) {
    return this->ProcessNavigation();
  } else if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::examplesJSON) {
    return this->ProcessCalculatorExamplesJSON();
  } else if (theGlobalVariables.userCalculatorRequestType == "toggleMonitoring") {
    return this->ProcessToggleMonitoring();
  } else if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::serverStatusJSON) {
    return this->ProcessServerStatusJSON();
  }
  if (theGlobalVariables.userCalculatorRequestType == "monitor") {
    return this->ProcessMonitor();
  } else if (theGlobalVariables.userCalculatorRequestType == "pause" && theGlobalVariables.flagAllowProcessMonitoring) {
    return this->ProcessPauseWorker();
  } else if (
    theGlobalVariables.userCalculatorRequestType == WebAPI::request::indicator &&
    theGlobalVariables.flagAllowProcessMonitoring
  ) {
    return this->ProcessComputationIndicator();
  } else if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::setProblemData) {
    return this->ProcessSetProblemDatabaseInfo();
  } else if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::changePassword) {
    return this->ProcessChangePassword(argumentProcessingFailureComments.str());
  } else if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::activateAccountJSON) {
    return this->ProcessActivateAccount();
  } else if (theGlobalVariables.userCalculatorRequestType == "signUp") {
    return this->ProcessSignUP();
  } else if (theGlobalVariables.userCalculatorRequestType == "forgotLogin") {
    return this->ProcessForgotLogin();
  } else if (theGlobalVariables.userCalculatorRequestType == "login") {
    return this->ProcessLoginUserInfo(comments.str());
  } else if (theGlobalVariables.userCalculatorRequestType == "logout") {
    return this->ProcessLogout();
  } else if ((
      theGlobalVariables.userCalculatorRequestType == "addEmails"||
      theGlobalVariables.userCalculatorRequestType == "addUsers"
    ) &&
    theGlobalVariables.flagLoggedIn
  ) {
    return this->ProcessAddUserEmails();
  } else if (
    theGlobalVariables.userCalculatorRequestType == "setTeacher" &&
    theGlobalVariables.flagLoggedIn
  ) {
    return this->ProcessAssignTeacherToSection();
  } else if ((
      theGlobalVariables.userCalculatorRequestType == WebAPI::request::submitAnswers ||
      theGlobalVariables.userCalculatorRequestType == WebAPI::request::submitExercise
    ) &&
    theGlobalVariables.flagLoggedIn
  ) {
    return this->ProcessSubmitAnswers();
  } else if (theGlobalVariables.userCalculatorRequestType == "scores" && theGlobalVariables.flagLoggedIn) {
    return this->ProcessScores();
  } else if (theGlobalVariables.userCalculatorRequestType == "scoresInCoursePage" && theGlobalVariables.flagLoggedIn) {
    return this->ProcessScoresInCoursePage();
  } else if (theGlobalVariables.UserGuestMode() && theGlobalVariables.userCalculatorRequestType == "submitExerciseNoLogin") {
    return this->ProcessSubmitAnswers();
  } else if ((
      theGlobalVariables.userCalculatorRequestType == WebAPI::request::problemGiveUp &&
      theGlobalVariables.flagLoggedIn
    ) ||
    theGlobalVariables.userCalculatorRequestType == WebAPI::request::problemGiveUpNoLogin
  ) {
    return this->ProcessProblemGiveUp();
  } else if ((
      theGlobalVariables.userCalculatorRequestType == "problemSolution" &&
      theGlobalVariables.flagLoggedIn
    ) ||
    theGlobalVariables.userCalculatorRequestType == "problemSolutionNoLogin"
  ) {
    return this->ProcessProblemSolution();
  } else if ((
      theGlobalVariables.userCalculatorRequestType == "submitAnswersPreview" ||
      theGlobalVariables.userCalculatorRequestType == "submitExercisePreview"
    ) &&
    theGlobalVariables.flagLoggedIn
  ) {
    return this->ProcessSubmitAnswersPreview();
  } else if (
    theGlobalVariables.userCalculatorRequestType == WebAPI::request::submitExercisePreviewNoLogin &&
    theGlobalVariables.UserGuestMode()
  ) {
    return this->ProcessSubmitAnswersPreview();
  } else if (
    theGlobalVariables.userCalculatorRequestType == "scoredQuiz" ||
    theGlobalVariables.userCalculatorRequestType == "exercise" ||
    theGlobalVariables.userCalculatorRequestType == "exerciseNoLogin" ||
    theGlobalVariables.userCalculatorRequestType == "exerciseJSON" ||
    theGlobalVariables.userCalculatorRequestType == "scoredQuizJSON"
  ) {
    return this->ProcessExamPageJSON();
  } else if (
    theGlobalVariables.userCalculatorRequestType == "template" ||
    theGlobalVariables.userCalculatorRequestType == "templateNoLogin"
  ) {
    return this->ProcessTemplate();
  } else if (
    theGlobalVariables.userCalculatorRequestType == "templateJSON" ||
    theGlobalVariables.userCalculatorRequestType == "templateJSONNoLogin"
  ) {
    return this->ProcessTemplateJSON();
  } else if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::userInfoJSON) {
    return this->ProcessLoginUserInfo(comments.str());
  } else if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::editPage) {
    return this->ProcessEditPageJSON();
  } else if (theGlobalVariables.userCalculatorRequestType == "modifyPage") {
    return this->ProcessModifyPage();
  } else if (
    theGlobalVariables.userCalculatorRequestType == "slidesFromSource" ||
    theGlobalVariables.userCalculatorRequestType == "homeworkFromSource"
  ) {
    return this->ProcessSlidesOrHomeworkFromSource();
  } else if (
    theGlobalVariables.userCalculatorRequestType == "slidesSource" ||
    theGlobalVariables.userCalculatorRequestType == "homeworkSource"
  ) {
    return this->ProcessSlidesSource();
  } else if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::clonePage) {
    return this->ProcessClonePage();
  } else if (theGlobalVariables.userCalculatorRequestType == "compute") {
    return this->ProcessCompute();
  } else if (theGlobalVariables.userCalculatorRequestType == WebAPI::request::selectCourseJSON) {
    return this->ProcessSelectCourseJSON();
  } else if (theGlobalVariables.userCalculatorRequestType == "about") {
    return this->ProcessAbout();
  } else if (
    theGlobalVariables.userCalculatorRequestType == "topicListJSON" ||
    theGlobalVariables.userCalculatorRequestType == "topicListJSONNoLogin"
  ) {
    return this->ProcessTopicListJSON();
  } else if (theGlobalVariables.userCalculatorRequestType == WebAPI::app) {
    return this->ProcessApp(true);
  } else if (theGlobalVariables.userCalculatorRequestType == WebAPI::appNoCache) {
    return this->ProcessApp(false);
  } else if ("/" + theGlobalVariables.userCalculatorRequestType == WebAPI::request::onePageJS) {
    return this->ProcessCalculatorOnePageJS(false);
  } else if ("/" + theGlobalVariables.userCalculatorRequestType == WebAPI::request::onePageJSWithHash) {
    return this->ProcessCalculatorOnePageJS(true);
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
      theGlobalVariables.UserDefaultHasAdminRights(),
      false,
      &commentsOnFailure
  )) {
    this->SetHeadeR("HTTP/1.0 404 Object not found", "Content-Type: text/html");
    stOutput << "<html><body>File name: "
    << HtmlRoutines::ConvertStringToHtmlStringRestrictSize(this->VirtualFileName, false, 1000)
    << " <b style = 'color:red'>deemed unsafe</b>. "
    << "Please note that folder names are not allowed to contain dots and file names "
    << "are not allowed to start with dots. There may be additional restrictions "
    << "on file names added for security reasons.</body></html>";
    return 0;
  }
  if (FileOperations::IsFolderUnsecure(this->RelativePhysicalFileNamE)) {
    return this->ProcessFolder();
  }
  return this->ProcessFile();
}

void WebWorker::ResetPipesNoAllocation() {
  MacroRegisterFunctionWithName("WebWorker::ResetPipesNoAllocation");
  this->PauseComputationReportReceived.ResetNoAllocation();
  this->PauseWorker.ResetNoAllocation();
  this->PauseIndicatorPipeInUse.ResetNoAllocation();
  this->pipeWorkerToWorkerRequestIndicator.ResetNoAllocation();
  this->pipeWorkerToWorkerIndicatorData.ResetNoAllocation();
}

void WebWorker::ReleaseKeepInUseFlag() {
  MacroRegisterFunctionWithName("WebWorker::ReleaseKeepInUseFlag");
  this->pipeWorkerToServerTimerPing.Release();
  this->pipeWorkerToServerControls.Release();
  this->pipeWorkerToWorkerRequestIndicator.Release();
  this->pipeWorkerToWorkerIndicatorData.Release();
  this->pipeWorkerToWorkerStatus.Release();
  this->PauseComputationReportReceived.Release();
  this->PauseWorker.Release();
  this->PauseIndicatorPipeInUse.Release();
  this->millisecondsLastPingServerSideOnly = - 1;
  WebServer::Release(this->connectedSocketID);
}

void WebWorker::Release() {
  this->ReleaseKeepInUseFlag();
  this->flagInUsE = false;
}

void WebWorker::OutputShowIndicatorOnTimeout() {
  MacroRegisterFunctionWithName("WebWorker::OutputShowIndicatorOnTimeout");
  MutexLockGuard theLock(this->PauseIndicatorPipeInUse.mutexForProcessBlocking.GetElement());
  //this->PauseIndicatorPipeInUse.RequestPausePauseIfLocked();
  theGlobalVariables.flagOutputTimedOut = true;
  theGlobalVariables.flagTimedOutComputationIsDone = false;
  //ProgressReportWebServer theReport("WebServer::OutputShowIndicatorOnTimeout");
  logWorker << logger::blue << "Computation timeout, sending progress indicator instead of output. " << logger::endL;
  std::stringstream timeOutComments;
  JSData result;
  result[WebAPI::result::timeOut] = true;
  if (!theGlobalVariables.flagTimeOutExplanationAlreadyDisplayed) {
    timeOutComments << "Your computation is taking more than "
    << theGlobalVariables.millisecondsReplyAfterComputation
    << " ms. ";
  }
  if (!theGlobalVariables.flagAllowProcessMonitoring) {
    timeOutComments << "Monitoring computations is not allowed on this server.<br> "
    << "If you want to carry out a long computation you will need to install the calculator on your own machine.<br> "
    << "At the moment, the only way to do that is by setting the variable "
    << "theGlobalVariables.flagAllowProcessMonitoring to true, "
    << "achieved through manually compiling the calculator from source.<br> ";
    result[WebAPI::result::timeOutComments] = timeOutComments.str();
    return;
  }
  result[WebAPI::result::workerId] = this->indexInParent + 1;
  result[WebAPI::result::timeOutComments] = timeOutComments.str();
  if (this->indexInParent < 0) {
    crash << "Index of worker is smaller than 0, this shouldn't happen. " << crash;
  }
  stOutput << result.ToString(false);
  this->SendAllBytesWithHeaders();
  for (int i = 0; i < this->parent->theWorkers.size; i ++) {
    if (i != this->indexInParent) {
      this->parent->theWorkers[i].Release();
    }
  }
  this->parent->Release(this->connectedSocketID);
  //set flags properly:
  //we need to rewire the standard output and the crashing mechanism:
  crash.CleanUpFunction = WebWorker::OutputCrashAfterTimeout;
  //note that standard output cannot be rewired in the beginning of the function as we use the old stOutput
  stOutput.theOutputFunction = WebWorker::StandardOutputAfterTimeOut;
  this->PauseIndicatorPipeInUse.mutexForProcessBlocking.GetElement().UnlockMe();
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
  if (theGlobalVariables.userCalculatorRequestType == "") {
    out << "[empty]";
  } else {
    out << theGlobalVariables.userCalculatorRequestType;
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
      out << ", <b style =\"color:green\">current process</b>";
    } else {
      out << ", <b>in use</b>";
    }
    out << ", <a href=\"calculator?request=monitor&mainInput=" << this->indexInParent + 1 << "\">monitor process "
    << this->indexInParent + 1 << "</a>";
  } else {
    out << ", not in use";
  }
  if (this->displayUserInput != "") {
    out << ", user input: <span style =\"color:blue\">" << this->displayUserInput << "</span>";
  }
  out << ", connection " << this->connectionID << ", process ID: ";
  if (this->ProcessPID != 0) {
    out << this->ProcessPID;
  } else {
    out << "(not accessible)";
  }
  out << ", socketID: ";
  if (this->connectedSocketID == - 1) {
    out << "released in current process, value before release: " << this->connectedSocketIDLastValueBeforeRelease;
  } else {
    out << this->connectedSocketID;
  }
  out << ". ";
  out << " Server time at last ping: " << this->millisecondsLastPingServerSideOnly << " milliseconds. ";
  if (this->pingMessage != "") {
    out << " Message at last ping: " << this->pingMessage;
  }
  if (this->status != "") {
    out << "<br><b style =\"color:red\">Status: " << this->status << "</b><br>";
  }
  out << "Pipes: " << this->pipeWorkerToServerControls.ToString()
  << ", " << this->pipeWorkerToServerTimerPing.ToString()
  << ", " << this->pipeWorkerToWorkerRequestIndicator.ToString()
  << ", " << this->pipeWorkerToWorkerIndicatorData.ToString()
  << ", " << this->pipeWorkerToWorkerStatus.ToString()
  << ", " << this->PauseWorker.ToString()
  << ", " << this->PauseComputationReportReceived.ToString()
  << ", " << this->PauseIndicatorPipeInUse.ToString();
  out << ", user address: " << this->userAddress << ".";
  return out.str();
}

bool WebServer::CheckConsistency() {
  if (this->flagDeallocated) {
    crash << "Use after free of WebServer." << crash;
  }
  return true;
}

void WebServer::ReleaseEverything() {
  this->theTLS.Free();
  logger& currentLog = theGlobalVariables.flagIsChildProcess ? logWorker : logServer;
  ProgressReportWebServer::flagServerExists = false;
  for (int i = 0; i < this->theWorkers.size; i ++) {
    this->theWorkers[i].Release();
  }
  theGlobalVariables.WebServerReturnDisplayIndicatorCloseConnection = 0;
  theGlobalVariables.IndicatorStringOutputFunction = 0;
  theGlobalVariables.PauseUponUserRequest = 0;
  this->activeWorker = - 1;
  if (theGlobalVariables.flagServerDetailedLog) {
    currentLog << logger::red << "Detail: "
    << " About to close socket: " << this->listeningSocketHTTP << ". " << logger::endL;
  }
  if (this->listeningSocketHTTP != - 1) {
    close(this->listeningSocketHTTP);
    if (theGlobalVariables.flagServerDetailedLog) {
      currentLog << logger::red << "Detail: "
      << " Just closed socket: " << this->listeningSocketHTTP << logger::endL;
    }
    this->listeningSocketHTTP = - 1;
  }
  if (this->listeningSocketHttpSSL != - 1) {
    close(this->listeningSocketHttpSSL);
    if (theGlobalVariables.flagServerDetailedLog) {
      currentLog << logger::red << "Detail: "
      << " Just closed socket: " << this->listeningSocketHttpSSL << logger::endL;
    }
  }
  this->listeningSocketHttpSSL = - 1;
}

WebServer::~WebServer() {
  this->flagDeallocated = true;
}

void WebServer::ReturnActiveIndicatorAlthoughComputationIsNotDone() {
  MacroRegisterFunctionWithName("WebServer::ReturnActiveIndicatorAlthoughComputationIsNotDone");
  theWebServer.GetActiveWorker().OutputShowIndicatorOnTimeout();
}

void WebServer::FlushActiveWorker() {
  theWebServer.GetActiveWorker().SendAllBytesWithHeaders();
}

void WebServer::SendStringThroughActiveWorker(const std::string& theString) {
  theWebServer.GetActiveWorker().QueueStringForSendingWithHeadeR(theString, false);
}

void WebServer::PipeProgressReport(const std::string& theString) {
  if (theWebServer.activeWorker == - 1) {
    return;
  }
  theWebServer.GetActiveWorker().PipeProgressReport(theString);
}

WebServer::WebServer() {
  this->flagDeallocated = false;
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
  this->previousServerStatReport = 0;
  this->previousServerStatDetailedReport = 0;
}

void WebServer::Signal_SIGCHLD_handler(int s) {
  (void) s; //avoid unused parameter warning, portable.
  if (theGlobalVariables.flagIsChildProcess) {
    return;
  }
  if (theGlobalVariables.flagServerDetailedLog) {
    logProcessStats << "Detail: webServer received SIGCHLD signal. " << logger::endL;
  }
  theWebServer.flagReapingChildren = true;
  theWebServer.ReapChildren();
}

void WebServer::ReapChildren() {
  //Attention: this function is executed simultaneously with the
  //main execution path. Any non-standard operations here may be racy.
  //Please avoid allocating RAM memory outside of the stack.
  //Do not use anything that is not thread-safe here.
  //In particular, do not use any loggers.
  int waitResult = 0;
  int exitFlags = WNOHANG| WEXITED;
  do {
    waitResult = waitpid(- 1, NULL, exitFlags);
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
  void (*oldOutputFunction)(const std::string& stringToOutput) = stOutput.theOutputFunction;
  stOutput.theOutputFunction = 0; //<- We are checking if the web server is in order.
  //Before that we prevent the crashing mechanism from trying to use (the eventually corrupt) web server
  //to report the error over the Internet.
  if (this->activeWorker < 0 || this->activeWorker >= this->theWorkers.size) {
    crash << "Active worker index is " << this->activeWorker << " however I have " << this->theWorkers.size
    << " workers. " << crash;
  }
  stOutput.theOutputFunction = oldOutputFunction;//<-the web server is in order,
  //therefore we restore the ability to report crashes over the internet.
  return this->theWorkers[this->activeWorker];
}

void WebServer::SignalActiveWorkerDoneReleaseEverything() {
  MacroRegisterFunctionWithName("WebServer::SignalActiveWorkerDoneReleaseEverything");
  if (theWebServer.activeWorker == - 1) {
    return;
  }
  theWebServer.GetActiveWorker().SendAllAndWrapUp();
  theWebServer.activeWorker = - 1;
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
  if (theWebServer.activeWorker == - 1) {
    if (!theGlobalVariables.flagComputationFinishedAllOutputSentClosing) {
      crash << "WebServer::WorkerTimerPing called when the computation is not entirely complete. " << crash;
    }
    return;
  }
  std::stringstream outTimestream;
  outTimestream << pingTime;
  theWebServer.GetActiveWorker().pipeWorkerToServerTimerPing.WriteOnceAfterEmptying(outTimestream.str(), false, false);
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
  this->theWorkers.SetSize(this->theWorkers.size - 1);
  return false;
}

bool WebServer::CreateNewActiveWorker() {
  MacroRegisterFunctionWithName("WebServer::CreateNewActiveWorker");
  if (this->activeWorker != - 1) {
    crash << "Calling CreateNewActiveWorker requires the active worker index to be - 1." << crash;
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
    this->theWorkers.SetSize(this->theWorkers.size + 1);
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
  std::stringstream stowstream, wtosStream, wtowStream;
  stowstream << "S->W" << this->activeWorker + 1 << ": ";
  wtosStream << "W" << this->activeWorker + 1 << "->S: ";
  wtowStream << "W<->W" << this->activeWorker + 1 << ": ";
  std::string stow = stowstream.str();
  std::string wtos = wtosStream.str();
  std::string wtow = wtowStream.str();
  if (!this->GetActiveWorker().PauseComputationReportReceived.CreateMe(stow + "report received", false, true)) {
    logPlumbing << "Failed to create pipe: "
    << this->GetActiveWorker().PauseComputationReportReceived.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!this->GetActiveWorker().PauseWorker.CreateMe(stow + "pause", false, true)) {
    logPlumbing << "Failed to create pipe: "
    << this->GetActiveWorker().PauseWorker.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!this->GetActiveWorker().PauseIndicatorPipeInUse.CreateMe(stow + "ind. pipe busy", false, true)) {
    logPlumbing << "Failed to create pipe: "
    << this->GetActiveWorker().PauseIndicatorPipeInUse.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!this->GetActiveWorker().pipeWorkerToWorkerRequestIndicator.CreateMe(wtow + "request-indicator")) {
    logPlumbing << "Failed to create pipe: "
    << this->GetActiveWorker().pipeWorkerToWorkerRequestIndicator.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!this->GetActiveWorker().pipeWorkerToServerTimerPing.CreateMe(wtos + "ping", false, false, false, true)) {
    logPlumbing << "Failed to create pipe: "
    << this->GetActiveWorker().pipeWorkerToServerTimerPing.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!this->GetActiveWorker().pipeWorkerToServerControls.CreateMe(wtos + "controls", false, false, false, true)) {
    logPlumbing << "Failed to create pipe: "
    << this->GetActiveWorker().pipeWorkerToServerControls.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!this->GetActiveWorker().pipeWorkerToWorkerIndicatorData.CreateMe(wtow + "indicator-data")) {
    logPlumbing << "Failed to create pipe: "
    << this->GetActiveWorker().pipeWorkerToWorkerIndicatorData.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  if (!this->GetActiveWorker().pipeWorkerToWorkerStatus.CreateMe(wtos + "worker status", false, false, false, true)) {
    logPlumbing << "Failed to create pipe: "
    << this->GetActiveWorker().pipeWorkerToWorkerStatus.name << "\n";
    return this->EmergencyRemoval_LastCreatedWorker();
  }
  logPlumbing << logger::green
  << "Allocated new worker & plumbing data structures. Total worker data structures: "
  << this->theWorkers.size << ". "
  << logger::endL;
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
    crash << "Bad index in parent!" << crash;
  }
  out << this->GetActiveWorker().ToStringStatus();
  return out.str();
}

std::string WebServer::ToStringConnectionSummary() {
  MacroRegisterFunctionWithName("WebServer::ToStringConnectionSummary");
  if (theGlobalVariables.flagRunningApache) {
    return "Running through standard Apache web server, no connection details to display. ";
  }
  std::stringstream out;
  TimeWrapper now;
  now.AssignLocalTime();
  out << "<b>Server status.</b> Server time: local: " << now.ToStringLocal() << ", gm: " << now.ToStringGM() << ".<br>";
  int64_t timeRunninG = - 1;
  if (this->activeWorker < 0 || this->activeWorker >= this->theWorkers.size) {
    timeRunninG = theGlobalVariables.GetElapsedMilliseconds();
  } else {
    timeRunninG = this->GetActiveWorker().millisecondsLastPingServerSideOnly;
  }
  out
  << (((double) timeRunninG) / 1000)
  << " seconds = "
  << TimeWrapper::ToStringSecondsToDaysHoursSecondsString(timeRunninG / 1000, false, false)
  << " web server uptime. ";
  int approxNumPings = timeRunninG / this->WebServerPingIntervalInSeconds / 1000;
  if (approxNumPings < 0) {
    approxNumPings = 0;
  }
  int numConnectionsSoFarApprox = this->NumConnectionsSoFar - approxNumPings;
  if (numConnectionsSoFarApprox < 0) {
    numConnectionsSoFarApprox = 0;
  }
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

std::string WebServer::ToStringStatusForLogFile() {
  MacroRegisterFunctionWithName("WebServer::ToStringStatusForLogFile");
  if (theGlobalVariables.flagRunningApache) {
    return "Running through standard Apache web server, no connection details to display. ";
  }
  std::stringstream out;
  out << this->ToStringConnectionSummary();
  int numInUse = 0;
  for (int i = 0; i < this->theWorkers.size; i ++) {
    if (this->theWorkers[i].flagInUsE) {
      numInUse ++;
    }
  }
  out << "<hr>Currently, there are " << numInUse
  << " worker(s) in use. The peak number of worker(s)/concurrent connections was " << this->theWorkers.size << ". ";
  out
  << "<br>kill commands: " << this->NumProcessAssassinated
  << ", processes reaped: " << this->NumProcessesReaped
  << ", normally reclaimed workers: " << this->NumWorkersNormallyExited
  << ", connections so far: " << this->NumConnectionsSoFar;
  return out.str();
}

std::string WebServer::ToStringStatusAll() {
  MacroRegisterFunctionWithName("WebServer::ToStringStatusAll");
  if (theGlobalVariables.flagRunningApache) {
    return "Running through Apache. ";
  }
  std::stringstream out;

  if (!theGlobalVariables.UserDefaultHasAdminRights()) {
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
    currentWorker.pipeWorkerToWorkerStatus.ReadOnceWithoutEmptying(false, false);
    currentWorker.status = currentWorker.pipeWorkerToWorkerStatus.GetLastRead();
  }
  out << "<hr>";
  out << "Connections: " << this->currentlyConnectedAddresses.ToString();
  for (int i = 0; i < this->theWorkers.size; i ++) {
    out << "<hr>" << this->theWorkers[i].ToStringStatus();
  }
  return out.str();
}

bool WebServer::RestartIsNeeded() {
  MacroRegisterFunctionWithName("WebServer::RestartIsNeeded");
  struct stat theFileStat;
  if (stat(theGlobalVariables.PhysicalNameExecutableWithPath.c_str(), &theFileStat) != 0) {
    return false;
  }
  if (this->timeLastExecutableModification == - 1) {
    return false;
  }
  if (this->timeLastExecutableModification == theFileStat.st_ctime) {
    return false;
  }
  stOutput << "<html>";
  stOutput << "<head><script language =\"javascript\">setTimeout(resubmit, 500); "
  << " function resubmit() { location.reload(true);}</script></head>";
  stOutput << "<body>";

  stOutput << "<b>The server executable was updated, but the server has not been restarted yet. "
  << "Restarting in 0.5 seconds...</b>";
  stOutput << "</body></html>";
  logWorker << "Current process spawned from file with time stamp: "
  << this->timeLastExecutableModification
  << "; latest executable has different time stamp: " << theFileStat.st_ctime
  << ". " << logger::red << "RESTARTING." << logger::endL;
  theGlobalVariables.flagRestartNeeded = true;

  return true;
}

void WebServer::StopKillAll(bool attemptToRestart) {
  if (
    theGlobalVariables.processType != ProcessTypes::server &&
    theGlobalVariables.processType != ProcessTypes::serverMonitor
  ) {
    crash << "Server restart is allowed only to the server/monitor processes. " << crash;
  }
  logger* currentLog = 0;
  if (theGlobalVariables.processType == "serverMonitor") {
    currentLog = &logServerMonitor;
  }
  if (theGlobalVariables.processType == "server") {
    currentLog = &logServer;
  }

  *currentLog << logger::red << "Server restart requested. " << logger::endL;
  *currentLog << "Sending kill signal to all copies of the calculator. " << logger::endL;
  for (int i = 0; i < this->theWorkers.size; i ++) {
    this->TerminateChildSystemCall(i);
  }
  theSignals.unblockSignals();
  *currentLog << "Waiting for child processes to exit. " << logger::endL;
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
      *currentLog << logger::red << "Child exit timeout: made " << waitAttempts << " attempts to exit. " << logger::endL;
      break;
    }
    *currentLog << logger::blue << "Still waiting on " << workersStillInUse << " workers to finish. " << logger::endL;
    theGlobalVariables.FallAsleep(1000000);
  }
  this->ReleaseEverything();
  std::stringstream theCommand;
  theCommand << "killall " << theGlobalVariables.PhysicalNameExecutableNoPath;
  if (attemptToRestart) {
    *currentLog << logger::yellow << "Proceeding to restart server. " << logger::endL;
    int timeLimitSeconds = theGlobalVariables.millisecondsMaxComputation / 1000;
    *currentLog << logger::red << "Restart with time limit " << timeLimitSeconds << logger::endL;
    theCommand << " && ";
    theCommand << theGlobalVariables.PhysicalNameExecutableWithPath;
    theCommand << " server " << timeLimitSeconds;
    *currentLog << logger::red << "Restart command: " << logger::blue << theCommand.str() << logger::endL;
  } else {
    *currentLog << logger::red << "Proceeding to stop server. " << logger::endL;
  }
  theGlobalVariables.CallSystemNoOutput(theCommand.str(), currentLog); //kill any other running copies of the calculator.
  while (true) {
    theGlobalVariables.FallAsleep(1000000);
    logServer << logger::red << "Waiting for killall command ... " << logger::endL;
  }
}

void WebServer::initPortsITry() {
  this->PortsITryHttp.AddOnTop("8155");
  this->PortsITryHttp.AddOnTop("8156");
  this->PortsITryHttp.AddOnTop("8157");
  this->PortsITryHttp.AddOnTop("8158");
  this->PortsITryHttp.AddOnTop("8159");
  if (!theGlobalVariables.flagSSLIsAvailable) {
    return;
  }
  this->PortsITryHttpSSL.AddOnTop("8166");
  this->PortsITryHttpSSL.AddOnTop("8167");
  this->PortsITryHttpSSL.AddOnTop("8168");
  this->PortsITryHttpSSL.AddOnTop("8169");
}

void WebServer::initListeningSockets() {
  MacroRegisterFunctionWithName("WebServer::initListeningSockets");
  if (listen(this->listeningSocketHTTP, WebServer::maxNumPendingConnections) == - 1) {
    crash << "Listen function failed on http port." << crash;
  }
  if (theGlobalVariables.flagSSLIsAvailable) {
    if (listen(this->listeningSocketHttpSSL, WebServer::maxNumPendingConnections) == - 1) {
      crash << "Listen function failed on https port." << crash;
    }
  }
  this->highestSocketNumber = - 1;
  if (this->listeningSocketHTTP != - 1) {
    this->theListeningSockets.AddOnTop(this->listeningSocketHTTP);
    this->highestSocketNumber = MathRoutines::Maximum(this->listeningSocketHTTP, this->highestSocketNumber);
  }
  if (this->listeningSocketHttpSSL != - 1) {
    this->theListeningSockets.AddOnTop(this->listeningSocketHttpSSL);
    this->highestSocketNumber = MathRoutines::Maximum(this->listeningSocketHttpSSL, this->highestSocketNumber);
  }
}

void WebServer::initDates() {
  this->timeLastExecutableModification = - 1;
  struct stat theFileStat;
  if (stat(theGlobalVariables.PhysicalNameExecutableWithPath.c_str(), &theFileStat) != 0) {
    return;
  }
  this->timeLastExecutableModification = theFileStat.st_ctime;
}

void WebServer::ReleaseWorkerSideResources() {
  MacroRegisterFunctionWithName("WebServer::ReleaseWorkerSideResources");
  logger& currentLog = theGlobalVariables.flagIsChildProcess ? logWorker : logProcessKills;
  if (theGlobalVariables.flagServerDetailedLog) {
    currentLog << logger::red << "Detail: server about to RELEASE active workder. " << logger::endL;
  }
  this->Release(this->GetActiveWorker().connectedSocketID);
  if (theGlobalVariables.flagServerDetailedLog) {
    currentLog << logger::green << "Detail: server RELEASED active worker. " << logger::endL;
  }
  //<-release socket- communication is handled by the worker.
  this->activeWorker = - 1; //<-The active worker is needed only in the child process.
}

bool WebServer::RequiresLogin(const std::string& inputRequest, const std::string& inputAddress) {
  MacroRegisterFunctionWithName("WebServer::RequiresLogin");
  if (inputAddress == theGlobalVariables.DisplayNameExecutable) {
    if (this->requestsNotNeedingLogin.Contains(inputRequest)) {
      return false;
    }
  }
  for (int i = 0; i < this->addressStartsNotNeedingLogin.size; i ++) {
    if (MathRoutines::StringBeginsWith(inputAddress, this->addressStartsNotNeedingLogin[i])) {
      return false;
    }
  }
  return true;
}

void segfault_sigaction(int signal, siginfo_t* si, void* arg) {
//<- this signal should never happen in
//<- server, so even if racy, we take the risk of a hang.
//<- racy-ness in child process does not bother us: hanged children are still fine.
  (void) signal; //avoid unused parameter warning, portable.
  (void) arg;
  crash << "Caught segfault at address: " << si->si_addr << crash;
  exit(0);
}

void WebServer::fperror_sigaction(int signal) {
  (void) signal;
  crash << "Fatal arithmetic error. " << crash;
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
  this->theWorkers[i].flagInUsE = false;
  this->currentlyConnectedAddresses.SubtractMonomial(this->theWorkers[i].userAddress, 1);
  if (this->theWorkers[i].ProcessPID > 0) {
    if (theGlobalVariables.flagServerDetailedLog) {
      logProcessKills << "Detail: " << " killing child index: " << i << "." << logger::endL;
    }
    this->TerminateProcessId(this->theWorkers[i].ProcessPID);
    this->theWorkers[i].ProcessPID = - 1;
  }
  this->theWorkers[i].ResetPipesNoAllocation();
}

void WebServer::HandleTooManyConnections(const std::string& incomingUserAddress) {
  MacroRegisterFunctionWithName("WebServer::HandleTooManyConnections");
  if (theGlobalVariables.flagIsChildProcess) {
    return;
  }
  if (theGlobalVariables.flagServerDetailedLog) {
    logProcessStats << logger::red << "Detail: "
    << " too many connections handler start. " << logger::endL;
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
        theTimes.AddOnTop(this->theWorkers[i].millisecondsServerAtWorkerStart);
        theIndices.AddOnTop(i);
      }
    }
  }
  theTimes.QuickSortAscending(0, &theIndices);
  if (!theSignals.flagSignalsAreBlocked) {
    crash << "Signals must be blocked at this point of code. " << crash;
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
    logProcessKills << logger::red << errorStream.str() << logger::endL;
  }
  if (theGlobalVariables.flagServerDetailedLog) {
    logProcessStats << logger::green
    << "Detail: connection cleanup successful. " << logger::endL;
  }
}

void WebServer::ProcessOneChildMessage(int childIndex, int& outputNumInUse) {
  std::string messageString = this->theWorkers[childIndex].pipeWorkerToServerControls.GetLastRead();
  this->theWorkers[childIndex].flagInUsE = false;
  this->currentlyConnectedAddresses.SubtractMonomial(this->theWorkers[childIndex].userAddress, 1);
  std::stringstream commentsOnFailure;
  JSData workerMessage;
  if (!workerMessage.readstring(messageString, false, &commentsOnFailure)) {
    logServer << logger::red << "Worker "
    << childIndex + 1 << " sent corrupted result message: "
    << messageString << ". Marking for reuse. " << logger::endL;
  } else {
    logServer << logger::green << "Worker "
    << childIndex + 1 << " done with message: "
    << messageString
    << ". Marking for reuse. " << logger::endL;
  }
  outputNumInUse --;
  this->NumWorkersNormallyExited ++;
  if (workerMessage["result"].theString == "toggleMonitoring") {
    this->ToggleProcessMonitoring();
  }
  if (workerMessage["connectionsServed"].theType == JSData::token::tokenLargeInteger) {
    this->NumberOfServerRequestsWithinAllConnections +=
    (int) workerMessage["connectionsServed"].theInteger.GetElement().GetIntValueTruncated()
    ;
  }
  if (workerMessage["stopNeeded"].isTrueRepresentationInJSON()) {
    this->StopKillAll(false);
  }
  if (workerMessage["restartNeeded"].isTrueRepresentationInJSON()) {
    //A worker has returned request to restart.
    //This is likely a user-triggered event; restart is desired.
    this->StopKillAll(true);
  }
}

void WebServer::RecycleOneChild(int childIndex, int& numberInUse) {
  if (!this->theWorkers[childIndex].flagInUsE) {
    return;
  }
  WebWorker& currentWorker = this->theWorkers[childIndex];
  PipePrimitive& currentControlPipe = currentWorker.pipeWorkerToServerControls;
  if (currentControlPipe.flagReadEndBlocks) {
    crash << "Pipe: " << currentControlPipe.ToString() << " has blocking read end. " << crash;
  }
  currentControlPipe.ReadOnceIfFailThenCrash(false, true);
  if (currentControlPipe.lastRead.size > 0) {
    this->ProcessOneChildMessage(childIndex, numberInUse);
  } else {
    logServer << logger::orange << "Worker " << childIndex + 1 << " not done yet. " << logger::endL;
  }
  PipePrimitive& currentPingPipe = currentWorker.pipeWorkerToServerTimerPing;
  if (currentPingPipe.flagReadEndBlocks) {
    crash << "Pipe: " << currentPingPipe.ToString() << " has blocking read end. " << crash;
  }
  currentPingPipe.ReadOnceIfFailThenCrash(false, true);
  if (currentPingPipe.lastRead.size > 0) {
    currentWorker.pingMessage = currentPingPipe.GetLastRead();
    currentWorker.millisecondsLastPingServerSideOnly = theGlobalVariables.GetElapsedMilliseconds();
    if (currentWorker.pingMessage != "") {
      logServer << logger::blue << "Worker " << childIndex + 1 << " ping: "
      << currentWorker.pingMessage << ". " << logger::endL;
    }
    return;
  }
  if (currentWorker.PauseWorker.CheckPauseIsRequested(false, true, true)) {
    currentWorker.pingMessage = "worker paused, no pings.";
    currentWorker.millisecondsLastPingServerSideOnly = theGlobalVariables.GetElapsedMilliseconds();
    return;
  }
  if (theGlobalVariables.millisecondsNoPingBeforeChildIsPresumedDead <= 0) {
    return;
  }
  int64_t millisecondsElapsed = theGlobalVariables.GetElapsedMilliseconds() - currentWorker.millisecondsLastPingServerSideOnly;
  if (millisecondsElapsed <= theGlobalVariables.millisecondsNoPingBeforeChildIsPresumedDead) {
    return;
  }
  this->TerminateChildSystemCall(childIndex);
  std::stringstream pingTimeoutStream;
  pingTimeoutStream
  << millisecondsElapsed
  << " milliseconds passed since worker " << childIndex + 1
  << " last pinged the server; "
  << "the maximum allowed is: " << theGlobalVariables.millisecondsNoPingBeforeChildIsPresumedDead
  << ". ";
  pingTimeoutStream << "Killing connection "
  << currentWorker.connectionID
  << ", pid: "
  << currentWorker.ProcessPID << ". ";
  logProcessKills << logger::red << pingTimeoutStream.str() << logger::endL;
  currentWorker.pingMessage = "<b style =\"color:red\">" + pingTimeoutStream.str() + "</b>";
  numberInUse --;
  this->NumProcessAssassinated ++;
}

void WebServer::HandleTooManyWorkers(int& numInUse) {
  if (numInUse <= this->MaxTotalUsedWorkers) {
    if (theGlobalVariables.flagServerDetailedLog) {
      logProcessStats << logger::green
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
    logProcessKills << logger::red << errorStream.str() << logger::endL;
    numInUse --;
    this->NumProcessAssassinated ++;
  }
  if (theGlobalVariables.flagServerDetailedLog) {
    logProcessStats << logger::green
    << "Detail: RecycleChildrenIfPossible exit point 2. " << logger::endL;
  }
}

void WebServer::RecycleChildrenIfPossible() {
  //Listen for children who have exited properly.
  //This might need to be rewritten: I wasn't able to make this work with any
  //mechanism other than pipes.
  MacroRegisterFunctionWithName("WebServer::RecycleChildrenIfPossible");
  if (theGlobalVariables.flagIsChildProcess) {
    return;
  }
  if (theGlobalVariables.flagServerDetailedLog) {
    logProcessStats << logger::red << "Detail: RecycleChildrenIfPossible start. " << logger::endL;
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

bool WebServer::initBindToPorts() {
  MacroRegisterFunctionWithName("WebServer::initBindToPorts");
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
  for (int j = 0; j < 2; j ++, thePorts = &this->PortsITryHttpSSL, theListeningSocket = &this->listeningSocketHttpSSL) {
    for (int i = 0; i < (*thePorts).size; i ++) {
      rv = getaddrinfo(NULL, (*thePorts)[i].c_str(), &hints, &servinfo);
      if (rv != 0) {
        logWorker << "getaddrinfo: " << gai_strerror(rv) << logger::endL;
        return false;
      }
      for (p = servinfo; p != NULL; p = p->ai_next) {
        *theListeningSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (*theListeningSocket == - 1) {
          logWorker << "Error: socket failed.\n";
          continue;
        }
        if (setsockopt(*theListeningSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == - 1) {
          crash << "Error: setsockopt failed, error: \n" << strerror(errno) << crash;
        }
        if (bind(*theListeningSocket, p->ai_addr, p->ai_addrlen) == - 1) {
          close(*theListeningSocket);
          *theListeningSocket = - 1;
          logServer << "Error: bind failed at port: " << (*thePorts)[i] << ". Error: "
          << this->ToStringLastErrorDescription() << logger::endL;
          continue;
        }
        int setFlagCounter = 0;
        while (fcntl(*theListeningSocket, F_SETFL, O_NONBLOCK) != 0) {
          if (++ setFlagCounter > 10) {
            crash << "Error: failed to set non-blocking status to listening socket. " << crash;
          }
        }
        break;
      }
      if (p != NULL) {
        logServer << logger::yellow << "Successfully bound to port " << (*thePorts)[i] << logger::endL;
        if (j == 0) {
          this->httpPort = (*thePorts)[i];
        } else {
          this->httpSSLPort = (*thePorts)[i];
        }
        break;
      }
      freeaddrinfo(servinfo); // all done with this structure
    }
  }
  if (this->listeningSocketHTTP == - 1) {
    crash << "Failed to bind to any of the ports " << this->PortsITryHttp.ToStringCommaDelimited() << "\n" << crash;
  }
  if (theGlobalVariables.flagSSLIsAvailable && this->listeningSocketHttpSSL == - 1) {
    crash << "Failed to bind to any of the ports " << this->PortsITryHttpSSL.ToStringCommaDelimited() << "\n" << crash;
  }
  return true;
}

void WebServer::initializeSignals() {
  theSignals.initializeSignals();
}

void SignalsInfrastructure::initializeSignals() {
  MacroRegisterFunctionWithName("SignalsInfrastructure::initializeSignals");
  if (this->flagInitialized) {
    return;
  }
  sigfillset(&this->allSignals);
  if (sigemptyset(&this->SignalSEGV.sa_mask) == - 1) {
    crash << "Failed to initialize SignalSEGV mask. Crashing to let you know. " << crash;
  }
  this->SignalSEGV.sa_sigaction = &segfault_sigaction;
  this->SignalSEGV.sa_flags = SA_SIGINFO;
  if (sigaction(SIGSEGV, &SignalSEGV, NULL) == - 1) {
    crash << "Failed to register SIGSEGV handler (segmentation fault (attempt to write memory without permission))."
    << " Crashing to let you know. " << crash;
  }
  ///////////////////////
  //catch floating point exceptions
  if (sigemptyset(&SignalFPE.sa_mask) == - 1) {
    crash << "Failed to initialize SignalFPE mask. Crashing to let you know. " << crash;
  }
  SignalFPE.sa_sigaction = 0;
  SignalFPE.sa_handler = &WebServer::fperror_sigaction;
  if (sigaction(SIGFPE, &SignalFPE, NULL) == - 1) {
    crash << "Failed to register SIGFPE handler (fatal arithmetic error). Crashing to let you know. " << crash;
  }
  ////////////////////
  //ignore interruptsflagSignalsInitialized
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
  bool handleChildExit = true;
  if (handleChildExit) {
    logServer << logger::red << "Selected an unstable feature: signal child exit handler. "
    << "Please use with caution. " << logger::endL;
    SignalChild.sa_handler = &WebServer::Signal_SIGCHLD_handler; // reap all dead processes
  } else {
    SignalChild.sa_handler = NULL;
  }
  if (sigaction(SIGCHLD, &SignalChild, NULL) == - 1) {
    crash << "Was not able to register SIGCHLD handler (reaping child processes). Crashing to let you know." << crash;
  }
//  sigemptyset(&sa.sa_mask);
//  sa.sa_flags = SA_RESTART;
//  if (sigaction(SIGCHLD, &sa, NULL) == - 1)
//    logWorker << "sigaction returned - 1" << logger::endL;
  this->flagInitialized = true;
}

extern void MonitorWebServer(int pidServer);

void WebServer::WriteVersionJSFile() {
  MacroRegisterFunctionWithName("WebServer::WriteVersionJSFile");
  std::stringstream out;
  out << "\"use strict\"; //This file is automatically generated, please do not modify.\n";
  out << "var serverInformation = {\n  ";
  out << "  version:\"" << theGlobalVariables.buildVersionSimple << "\",\n";
  out << "};\n";
  out << "module.exports = {\nserverInformation,\n};\n";
  std::fstream theFileStream;
  FileOperations::OpenFileCreateIfNotPresentVirtual(
    theFileStream, "/calculator-html/server_information.js", false, true, false, false
  );
  theFileStream << out.str();
}

int WebServer::Run() {
  MacroRegisterFunctionWithName("WebServer::Run");
  theGlobalVariables.RelativePhysicalNameCrashLog = "crash_WebServerRun.html";
  //<-worker log resets are needed, else forked processes reset their common log.
  //<-resets of the server logs are not needed, but I put them here nonetheless.
  if (theGlobalVariables.flagSSLIsAvailable) {
    // creates key files if absent. Does not call any openssl functions.
    this->theTLS.initSSLKeyFiles();
  }
  logWorker         .reset();
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
  if (theGlobalVariables.flagServerAutoMonitor) {
    int pidServer = fork();
    if (pidServer < 0) {
      crash << "Failed to create server process. " << crash;
    }
    if (pidServer > 0) {
      theGlobalVariables.processType = "serverMonitor";
      theGlobalVariables.flagIsChildProcess = true;
      MonitorWebServer(pidServer);//<-this attempts to connect to the server over the internet and restarts if it can't.
      return 0;
    }
  }
  this->initializeSignals();
  // This object will delete theParser when out of scope:
  this->theCalculator.theObjectPointer = &theParser;
  this->theCalculator.RenewObject();
  theParser->initialize();
  //cannot call initializeMutex here: not before we execute fork();
  theParser->ComputeAutoCompleteKeyWords();
  theParser->WriteAutoCompleteKeyWordsToFile();
  this->WriteVersionJSFile();
  //theGlobalVariables.WriteSourceCodeFilesJS();
  theGlobalVariables.initModifiableDatabaseFields();
  HtmlRoutines::LoadStrings();
  theParser->flagShowCalculatorExamples = false;
  if (!this->initPrepareWebServerALL()) {
    return 1;
  }
  logServer << logger::purple << "waiting for connections..." << logger::endL;
  sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size = sizeof their_addr;
  char userAddressBuffer[INET6_ADDRSTRLEN];
  this->initSSL();
  fd_set FDListenSockets;
  this->NumSuccessfulSelectsSoFar = 0;
  this->NumFailedSelectsSoFar = 0;
  long long previousReportedNumberOfSelects = 0;
  while (true) {
    // main accept() loop
    FD_ZERO(&FDListenSockets);
    for (int i = 0; i < this->theListeningSockets.size; i ++) {
      FD_SET(this->theListeningSockets[i], &FDListenSockets);
    }
    if (theGlobalVariables.flagServerDetailedLog) {
      logServer << logger::red << "Detail: About to enter select loop. " << logger::endL;
    }
    theSignals.unblockSignals();
    while (select(this->highestSocketNumber + 1, &FDListenSockets, 0, 0, 0) == - 1) {
      if (this->flagReapingChildren) {
        if (theGlobalVariables.flagServerDetailedLog) {
          logServer << logger::yellow << "Interrupted while by child exit signal. "
          << logger::endL;
        }
        this->flagReapingChildren = false;
      } else {
        logServer << logger::red << "Select failed: this is not expected. Error message: "
        << strerror(errno) << logger::endL;
      }
      this->NumFailedSelectsSoFar ++;
    }
    theSignals.blockSignals();
    if (theGlobalVariables.flagServerDetailedLog) {
      logServer << logger::green << "Detail: select success. " << logger::endL;
    }
    int64_t millisecondsAfterSelect = theGlobalVariables.GetElapsedMilliseconds();
    this->NumSuccessfulSelectsSoFar ++;
    if ((this->NumSuccessfulSelectsSoFar + this->NumFailedSelectsSoFar) - previousReportedNumberOfSelects > 100) {
      logSocketAccept << logger::blue << this->NumSuccessfulSelectsSoFar << " successful and "
      << this->NumFailedSelectsSoFar << " bad ("
      << this->NumSuccessfulSelectsSoFar + this->NumFailedSelectsSoFar << " total) selects. " << logger::endL;
      previousReportedNumberOfSelects = this->NumSuccessfulSelectsSoFar + this->NumFailedSelectsSoFar;
    }
    int reportCount =
    this->NumProcessAssassinated + this->NumProcessesReaped +
    this->NumWorkersNormallyExited + this->NumConnectionsSoFar;
    if (reportCount - previousServerStatReport > 99) {
      this->previousServerStatReport = reportCount;
      logProcessStats << "# kill commands: " << this->NumProcessAssassinated
      << " #processes reaped: " << this->NumProcessesReaped
      << " #normally reclaimed workers: " << this->NumWorkersNormallyExited
      << " #connections so far: " << this->NumConnectionsSoFar << logger::endL;
    }
    if (reportCount - previousServerStatDetailedReport > 499) {
      this->previousServerStatDetailedReport = reportCount;
      logProcessStats << this->ToStringStatusForLogFile() << logger::endL;
    }
    int newConnectedSocket = - 1;
    theGlobalVariables.flagUsingSSLinCurrentConnection = false;
    bool found = false;
    for (int i = theListeningSockets.size - 1; i >= 0; i --) {
      if (FD_ISSET(this->theListeningSockets[i], &FDListenSockets)) {
        newConnectedSocket = accept(this->theListeningSockets[i], (struct sockaddr *)& their_addr, &sin_size);
        if (newConnectedSocket >= 0) {
          logServer << logger::green << "Connection candidate "
          << this->NumConnectionsSoFar + 1 << ". "
          << "Connected via listening socket " << this->theListeningSockets[i]
          << " on socket: " << newConnectedSocket;
          if (this->theListeningSockets[i] == this->listeningSocketHttpSSL) {
            theGlobalVariables.flagUsingSSLinCurrentConnection = true;
            logServer << logger::purple << " (SSL encrypted). " << logger::endL;
          } else {
            logServer << logger::yellow << " (non-encrypted). " << logger::endL;
          }
          break;
        } else {
          logSocketAccept << logger::red
          << "This is not supposed to happen: accept failed. Error: "
          << this->ToStringLastErrorDescription() << logger::endL;
          found = true;
        }
      }
    }
    if (newConnectedSocket < 0 && !found) {
      logSocketAccept << logger::red << "This is not supposed to to happen: select succeeded "
      << "but I found no set socket. " << logger::endL;
    }
    if (newConnectedSocket < 0) {
      if (theGlobalVariables.flagServerDetailedLog) {
        logServer << "Detail: newConnectedSocket is negative: "
        << newConnectedSocket << ". Not accepting. " << logger::endL;
      }
      continue;
    }
    inet_ntop(
      their_addr.ss_family,
      get_in_addr((struct sockaddr *) &their_addr),
      userAddressBuffer,
      sizeof userAddressBuffer
    );
    this->HandleTooManyConnections(userAddressBuffer);
    this->RecycleChildrenIfPossible();
    if (!this->CreateNewActiveWorker()) {
      logPlumbing << logger::purple
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
    this->GetActiveWorker().millisecondsServerAtWorkerStart = theGlobalVariables.GetElapsedMilliseconds();
    this->GetActiveWorker().millisecondsLastPingServerSideOnly = this->GetActiveWorker().millisecondsServerAtWorkerStart;
    this->GetActiveWorker().millisecondsAfterSelect = millisecondsAfterSelect; //<- measured right after select()
    //<-cannot set earlier as the active worker may change after recycling.
    this->NumConnectionsSoFar ++;
    this->GetActiveWorker().connectionID = this->NumConnectionsSoFar;
    this->GetActiveWorker().userAddress.theObject = userAddressBuffer;
    this->currentlyConnectedAddresses.AddMonomial(this->GetActiveWorker().userAddress, 1);
//    logWorker << this->ToStringStatus();
    /////////////
    if (theGlobalVariables.flagServerDetailedLog) {
      logProcessStats << "Detail: about to fork, sigprocmasking " << logger::endL;
    }
    if (theGlobalVariables.flagServerDetailedLog) {
      logProcessStats << "Detail: Sigprocmask done. Proceeding to fork. "
      << "Time elapsed: " << theGlobalVariables.GetElapsedSeconds() << " second(s). <br>"
      << logger::endL;
    }
    this->theTLS.AddMoreEntropyFromTimer();

    int incomingPID = fork(); //creates an almost identical copy of this process.
    //<- Please don't assign directly to this->GetActiveWorker().ProcessPID.
    //<- There may be a race condition around this line of code and I
    //want as little code as possible between the fork and the logFile.
    //The original process is the parent, the almost identical copy is the child.
    //logWorker << "\r\nChildPID: " << this->childPID;
    if (incomingPID == 0) {
      theGlobalVariables.processType = "worker";
    }
    if (theGlobalVariables.flagServerDetailedLog && incomingPID == 0) {
      logWorker << "Detail: fork() successful in worker; elapsed ms @ fork(): "
      << theGlobalVariables.GetElapsedMilliseconds() << logger::endL;
    }
    if (theGlobalVariables.flagServerDetailedLog && incomingPID > 0) {
      logSuccessfulForks << "Detail: fork() successful; elapsed ms @ fork(): "
      << theGlobalVariables.GetElapsedMilliseconds() << logger::endL;
    }
    if (incomingPID < 0) {
      logProcessKills << logger::red << " FAILED to spawn a child process. " << logger::endL;
    }
    this->GetActiveWorker().ProcessPID = incomingPID;
    if (this->GetActiveWorker().ProcessPID == 0) {
      // this is the child (worker) process
      theGlobalVariables.flagIsChildProcess = true;
      if (theGlobalVariables.flagServerDetailedLog) {
        logWorker << logger::green << "Detail: "
        << " FORK successful in worker, next step. Time elapsed: " << theGlobalVariables.GetElapsedSeconds()
        << " second(s). Calling sigprocmask. " << logger::endL;
      }
      theSignals.unblockSignals();
      if (theGlobalVariables.flagServerDetailedLog) {
        logWorker << logger::green << "Detail: sigprocmask success, running... " << logger::endL;
      }
      int result = this->GetActiveWorker().Run();
      if (theGlobalVariables.flagServerDetailedLog) {
        logWorker << "Detail: run finished, releasing resources. " << logger::endL;
      }
      this->ReleaseEverything();
      if (theGlobalVariables.flagServerDetailedLog) {
        logWorker << logger::green << "Detail: resources released, returning. " << logger::endL;
      }
      return result;
    }
    if (theGlobalVariables.flagServerDetailedLog) {
      logProcessStats << logger::green << "Detail: fork successful. Time elapsed: "
      << theGlobalVariables.GetElapsedMilliseconds() << " ms. "
      << "About to unmask signals. " << logger::endL;
    }
    this->ReleaseWorkerSideResources();
  }
  this->ReleaseEverything();
  this->theTLS.FreeEverythingShutdown();
  return 0;
}

int WebWorker::Run() {
  MacroRegisterFunctionWithName("WebWorker::Run");
  theGlobalVariables.millisecondOffset = this->millisecondsAfterSelect;
  this->CheckConsistency();
  if (this->connectedSocketID == - 1) {
    crash << "Worker::Run() started on a connecting with ID equal to - 1. " << crash;
  }
  this->ResetPipesNoAllocation();
  std::stringstream processNameStream;
  processNameStream << "W" << this->indexInParent + 1 << ": ";
  PauseProcess::currentProcessName = processNameStream.str();
  theGlobalVariables.flagServerForkedIntoWorker = true;
  crash.CleanUpFunction = WebServer::SignalActiveWorkerDoneReleaseEverything;
  CreateTimerThread();
  if (theGlobalVariables.flagUsingSSLinCurrentConnection) {
    if (!theWebServer.SSLServerSideHandShake()) {
      theGlobalVariables.flagUsingSSLinCurrentConnection = false;
      this->parent->SignalActiveWorkerDoneReleaseEverything();
      this->parent->ReleaseEverything();
      logOpenSSL << logger::red << "Ssl fail #: " << this->parent->NumConnectionsSoFar << logger::endL;
      return - 1;
    }
  }
  if (theGlobalVariables.flagSSLIsAvailable && theGlobalVariables.flagUsingSSLinCurrentConnection) {
    logOpenSSL << logger::green << "ssl success #: " << this->parent->NumConnectionsSoFar << ". " << logger::endL;
  }
  /////////////////////////////////////////////////////////////////////////
  stOutput.theOutputFunction = WebServer::SendStringThroughActiveWorker;
  int result = 0;
  this->numberOfReceivesCurrentConnection = 0;
  while (true) {
    StateMaintainerCurrentFolder preserveCurrentFolder;
    this->flagAllBytesSentUsingFile = false;
    this->flagEncounteredErrorWhileServingFile = false;
    if (!this->ReceiveAll()) {
      bool sslWasOK = true;
      if (theGlobalVariables.flagSSLIsAvailable) {
        sslWasOK = (this->error == TransportLayerSecurityOpenSSL::errors::errorWantRead);
      }
      if (this->numberOfReceivesCurrentConnection > 0 && sslWasOK) {
        logIO << logger::green << "Connection timed out after successfully receiving "
        << this->numberOfReceivesCurrentConnection << " times. " << logger::endL;
        result = 0;
        break;
      }
      logIO << logger::red << "Failed to receive all with error: " << this->error;
      result = - 1;
      break;
    }
    if (this->numberOfReceivesCurrentConnection > 0) {
      this->parent->theCalculator.RenewObject();
      theParser->initialize();
      logWorker << logger::blue << "Created new calculator for connection: "
      << this->numberOfReceivesCurrentConnection << logger::endL;
    }
    if (this->messageHead.size() == 0) {
      break;
    }
    result = this->ServeClient();
    if (this->connectedSocketID == - 1) {
      break;
    }
    if (!this->flagAllBytesSentUsingFile) {
      this->SendAllBytesWithHeaders();
    }
    this->numberOfReceivesCurrentConnection ++;
    // We break the connection if we need to turn on monitoring as that concerns other processes.
    if (
      !this->flagKeepAlive ||
      this->flagEncounteredErrorWhileServingFile ||
      this->flagToggleMonitoring ||
      theGlobalVariables.flagRestartNeeded ||
      theGlobalVariables.flagStopNeeded
    ) {
      break;
    }
    //The function call needs security audit.
    this->resetConnection();
    logWorker << logger::blue << "Received " << this->numberOfReceivesCurrentConnection
    << " times on this connection, waiting for more. "
    << logger::endL;
    theGlobalVariables.millisecondOffset += theGlobalVariables.GetElapsedMilliseconds();
    this->parent->WorkerTimerPing(theGlobalVariables.millisecondOffset);
  }
  this->WrapUpConnection();
  return result;
}

void WebServer::Release(int& theDescriptor) {
  PauseProcess::Release(theDescriptor);
}

extern int mainTest(List<std::string>& remainingArgs);

void WebServer::FigureOutOperatingSystem() {
  MacroRegisterFunctionWithName("WebServer::FigureOutOperatingSystem");
  if (theGlobalVariables.OperatingSystem != "") {
    return;
  }
  List<std::string> supportedOSes;
  supportedOSes.AddOnTop("Ubuntu");
  supportedOSes.AddOnTop("CentOS");
  std::string commandOutput = theGlobalVariables.CallSystemWithOutput("cat /etc/*-release");
  if (
    commandOutput.find("ubuntu") != std::string::npos ||
    commandOutput.find("Ubuntu") != std::string::npos ||
    commandOutput.find("UBUNTU") != std::string::npos
  ) {
    theGlobalVariables.OperatingSystem = "Ubuntu";
  } else if (commandOutput.find("CentOS") != std::string::npos) {
    theGlobalVariables.OperatingSystem = "CentOS";
  } else {
    theGlobalVariables.OperatingSystem = "";
  }
  if (theGlobalVariables.OperatingSystem != "") {
    return;
  }
  logServer << logger::red << "Your Linux flavor is not currently supported. " << logger::endL;
  logServer << "We support the following Linux distros: "
  << logger::blue << supportedOSes.ToStringCommaDelimited() << logger::endL;
  logServer << "Please post a request for support of your Linux flavor on our bug tracker: " << logger::endL
  << logger::green << HtmlRoutines::gitRepository
  << logger::endL << "and we will add your Linux flavor to the list of supported distros. " << logger::endL;
}

void WebServer::CheckSystemInstallationOpenSSL() {
  MacroRegisterFunctionWithName("WebServer::CheckSystemInstallationOpenSSL");
  if (theGlobalVariables.flagRunningApache) {
    return;
  }
  if (!theGlobalVariables.flagDatabaseCompiled) {
    return;
  }
  if (theGlobalVariables.configuration["openSSL"].theType != JSData::token::tokenUndefined) {
    return;
  }
  theGlobalVariables.configuration["openSSL"] = "Attempted installation";
  theGlobalVariables.ConfigurationStore();
  WebServer::FigureOutOperatingSystem();
  StateMaintainerCurrentFolder preserveFolder;
  std::string sslInstallCommand = "CentOS";
  if (theGlobalVariables.OperatingSystem == "Ubuntu") {
    sslInstallCommand = "sudo apt-get install libssl-dev";
  } else if (theGlobalVariables.OperatingSystem == "CentOS") {
    sslInstallCommand = "sudo yum install openssl-devel";
  } else {
    logServer << "You appear to be missing an openssl installation. "
    << "To support https, please install openssl. ";
    return;
  }
  if (sslInstallCommand != "") {
    logServer << "You appear to be missing an openssl installation. Let me try to install that for you. "
    << logger::green << "About to request sudo password for: " << sslInstallCommand << logger::endL;
    logServer << logger::red << "To refuse the command type CTRL+C. " << logger::endL;
    theGlobalVariables.CallSystemNoOutput(sslInstallCommand, &logServer);
    theGlobalVariables.configuration["openSSL"] = "Attempted installation on Ubuntu";
  }
  theGlobalVariables.ConfigurationStore();
}

void WebServer::CheckSystemInstallationMongoDB() {
  MacroRegisterFunctionWithName("WebServer::CheckSystemInstallationMongoDB");
  if (theGlobalVariables.flagRunningApache) {
    return;
  }
  if (theGlobalVariables.flagDatabaseCompiled) {
    return;
  }
  WebServer::FigureOutOperatingSystem();
  if (theGlobalVariables.OperatingSystem == "") {
    return;
  }
  if (theGlobalVariables.configuration["mongoDB"].theType != JSData::token::tokenUndefined) {
    return;
  }
  theGlobalVariables.configuration["mongoDB"] = "Attempted installation";
  theGlobalVariables.ConfigurationStore();

  StateMaintainerCurrentFolder preserveFolder;
  logServer << "You appear to be missing an mongoDB installation. Let me try to install that for you. "
  << logger::green << "Enter your the sudo password as prompted please. " << logger::endL;
  if (theGlobalVariables.OperatingSystem == "Ubuntu") {
    theGlobalVariables.CallSystemNoOutput("sudo apt-get install mongodb", &logServer);
    theGlobalVariables.configuration["mongoDB"] = "Attempted installation on Ubuntu";
  } else if (theGlobalVariables.OperatingSystem == "CentOS") {
    theGlobalVariables.CallSystemNoOutput("sudo yum install mongodb", &logServer);
    theGlobalVariables.configuration["mongoDB"] = "Attempted installation on CentOS";
  }
  theGlobalVariables.ChDir(theGlobalVariables.PhysicalPathProjectBase);
  theGlobalVariables.CallSystemNoOutput("make clean", &logServer);
  logServer << "Proceeding to rebuild the calculator. " << logger::red
  << "This is expected to take 10+ minutes. " << logger::endL;
  theGlobalVariables.CallSystemNoOutput("make -j8", &logServer);
  theGlobalVariables.ConfigurationStore();
}

void WebServer::CheckMongoDBSetup() {
  MacroRegisterFunctionWithName("WebServer::CheckMongoDBSetup");
  if (theGlobalVariables.flagDatabaseCompiled) {
    logServer << logger::green << "Compiled with mongo DB support. " << logger::endL;
  } else {
    logServer << logger::red << "Compiled without mongo DB support. " << logger::endL;
  }
  if (theGlobalVariables.configuration["mongoDBSetup"].theType != JSData::token::tokenUndefined) {
    return;
  }
  logServer << "DEBUG: mongoDB token: " << (int) theGlobalVariables.configuration["mongoDBSetup"].theType << logger::endL;
  logServer << logger::yellow << "configuration so far: " << theGlobalVariables.configuration.ToString(false) << logger::endL;
  theGlobalVariables.configuration["mongoDBSetup"] = "Attempting";
  theGlobalVariables.ConfigurationStore();
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
  theGlobalVariables.CallSystemNoOutput("./configure", &logServer);
  theGlobalVariables.CallSystemNoOutput("make -j8", &logServer);
  logServer << "Need sudo access for command: "
  << logger::red << "sudo make install" << logger::endL;
  theGlobalVariables.CallSystemNoOutput("sudo make install", &logServer);
  theGlobalVariables.ChDir("../libbson-1.9.3");
  theGlobalVariables.CallSystemNoOutput("./configure", &logServer);
  theGlobalVariables.CallSystemNoOutput("make -j8", &logServer);
  logServer << "Need sudo access for command: "
  << logger::red << "sudo make install" << logger::endL;
  theGlobalVariables.CallSystemNoOutput("sudo make install", &logServer);
  theGlobalVariables.ChDir("../../bin");
  logServer  << "Need sudo access for command to configure linker to use local usr/local/lib path (needed by mongo): "
  << logger::red << "sudo cp ../external-source/usr_local_lib_for_mongo.conf /etc/ld.so.conf.d/" << logger::endL;
  theGlobalVariables.CallSystemNoOutput("sudo cp ../external-source/usr_local_lib_for_mongo.conf /etc/ld.so.conf.d/", &logServer);
  logServer << "Need sudo access for command: " << logger::red << "sudo ldconfig" << logger::endL;
  theGlobalVariables.CallSystemNoOutput("sudo ldconfig", &logServer);
  theGlobalVariables.configuration["mongoDBSetup"] = "Setup complete";
  theGlobalVariables.ConfigurationStore();
}

void WebServer::CheckFreecalcSetup() {
  MacroRegisterFunctionWithName("WebServer::CheckFreecalcSetup");
  if (theGlobalVariables.configuration["freecalcSetup"].theType != JSData::token::tokenUndefined) {
    return;
  }
  theGlobalVariables.configuration["freecalcSetup"] = "Setup started";
  theGlobalVariables.ConfigurationStore();
  WebServer::FigureOutOperatingSystem();
  logServer << logger::yellow << "Freelcalc setup file missing, proceeding to set it up. " << logger::endL;
  StateMaintainerCurrentFolder preserveFolder;
  std::string startingDir = FileOperations::GetCurrentFolder();
  theGlobalVariables.ChDir(theGlobalVariables.PhysicalPathProjectBase + "../");
  logServer << logger::green << "Current folder: " << FileOperations::GetCurrentFolder() << logger::endL;
  logServer << logger::green << "git clone https://github.com/tmilev/freecalc.git" << logger::endL;
  theGlobalVariables.CallSystemNoOutput("git clone https://github.com/tmilev/freecalc.git", &logServer);
  theGlobalVariables.configuration["freecalcSetup"] = "Setup complete";
  theGlobalVariables.ConfigurationStore();
}

void WebServer::CheckMathJaxSetup() {
  MacroRegisterFunctionWithName("WebServer::CheckMathJaxSetup");
  if (theGlobalVariables.configuration["MathJax"].theType != JSData::token::tokenUndefined) {
    return;
  }
  theGlobalVariables.configuration["MathJax"] = "Attempting to install";
  theGlobalVariables.ConfigurationStore();
  WebServer::FigureOutOperatingSystem();
  logServer << logger::yellow << "MathJax setup file missing, proceeding to set it up. " << logger::endL
  << logger::green << "Enter your the sudo password as prompted please. " << logger::endL;
  if (theGlobalVariables.OperatingSystem == "Ubuntu") {
    theGlobalVariables.CallSystemNoOutput("sudo apt-get install unzip", &logServer);
  } else if (theGlobalVariables.OperatingSystem == "CentOS") {
    theGlobalVariables.CallSystemNoOutput("sudo yum install unzip", &logServer);
  }
  logServer << "Proceeding to unzip MathJax. ";
  //std::string currentFolder =FileOperations::GetCurrentFolder();
  //result << "Changing folder to: " << theGlobalVariables.PhysicalPathProjectBase << logger::endL;
  //theGlobalVariables.ChDir(theGlobalVariables.PhysicalPathProjectBase);
  logServer << "Current folder: " << FileOperations::GetCurrentFolder() << logger::endL;
  theGlobalVariables.CallSystemNoOutput("mkdir -p ../../public_html", &logServer);
  theGlobalVariables.CallSystemNoOutput("unzip ./../html-common/MathJax-2.7-latest.zip -d ./../../public_html/", &logServer);
  theGlobalVariables.CallSystemNoOutput("mv ./../../public_html/MathJax-2.7.2 ./../../public_html/MathJax-2.7-latest", &logServer);
  theGlobalVariables.configuration["MathJax"] = "Unzipped";
  theGlobalVariables.ConfigurationStore();
}

void WebServer::AnalyzeMainArgumentsTimeString(const std::string& timeLimitString) {
  if (timeLimitString == "") {
    return;
  }
  Rational timeLimit;
  timeLimit.AssignString(timeLimitString);
  int timeLimitInt = 0;
  if (timeLimit.IsIntegerFittingInInt(&timeLimitInt)) {
    theGlobalVariables.millisecondsMaxComputation = timeLimitInt;
    if (theGlobalVariables.millisecondsMaxComputation <= 0) {
      theGlobalVariables.millisecondsMaxComputation = 0;
    } else {
      theGlobalVariables.millisecondsMaxComputation *= 1000;
    }
    std::cout << "Max computation time: " << theGlobalVariables.millisecondsMaxComputation << " ms." << std::endl;
  }

}

void WebServer::AnalyzeMainArguments(int argC, char **argv) {
  MacroRegisterFunctionWithName("WebServer::AnalyzeMainArguments");
  if (argC < 0) {
    argC = 0;
  }
  theGlobalVariables.programArguments.SetSize(argC);
  for (int i = 0; i < argC; i ++) {
    theGlobalVariables.programArguments[i] = argv[i];
    //std::cout << "Argument " << i + 1 << ": " << theGlobalVariables.programArguments[i] << "\n";
  }
  ////////////////////////////////////////////////////
  theGlobalVariables.flagRunningAce = true;
  theGlobalVariables.flagRunningCommandLine = false;
  theGlobalVariables.flagRunningConsoleTest = false;
  theGlobalVariables.flagRunningApache = false;
  #ifdef MACRO_use_open_ssl
  theGlobalVariables.flagSSLIsAvailable = true;
  #endif
  #ifdef MACRO_use_MongoDB
  theGlobalVariables.flagDatabaseCompiled = true;
  #endif
  theGlobalVariables.flagRunningBuiltInWebServer = false;
  ////////////////////////////////////////////////////
  if (argC == 0) {
    theGlobalVariables.flagRunningCommandLine = true;
    return;
  }
  theGlobalVariables.initDefaultFolderAndFileNames(theGlobalVariables.programArguments[0]);
  std::string envRequestMethodApache = WebServer::GetEnvironment("REQUEST_METHOD");
  if (envRequestMethodApache == "GET" || envRequestMethodApache == "POST" || envRequestMethodApache == "HEAD") {
    theGlobalVariables.flagRunningApache = true;
    return;
  }
  if (argC < 2) {
    if (!theGlobalVariables.flagRunningApache) {
      theGlobalVariables.flagRunningCommandLine = true;
    }
    return;
  }
  std::string& secondArgument = theGlobalVariables.programArguments[1];
  if (secondArgument == "server") {
    theGlobalVariables.flagRunningBuiltInWebServer = true;
    theGlobalVariables.flagRunningAce = false;
    if (argC == 2) {
      return;
    }
    bool doRestart = false;
    std::string timeLimitString = "";
    std::string& thirdArgument = theGlobalVariables.programArguments[2];
    if (thirdArgument == "restart") {
      doRestart = true;
      if (argC > 3) {
        timeLimitString = theGlobalVariables.problemExpectedNumberOfAnswers[3];
      }
    } else {
      timeLimitString = thirdArgument;
    }
    WebServer::AnalyzeMainArgumentsTimeString(timeLimitString);
    if (doRestart) {
      theWebServer.StopKillAll(true);
    }
    return;
  }
  ////////////////////////////////
  if (secondArgument == "test") {
    theGlobalVariables.flagRunningConsoleTest = true;
    return;
  }
  theGlobalVariables.flagRunningCommandLine = true;
}

void WebServer::InitializeGlobalVariablesHashes() {
  StateMaintainerCurrentFolder preserveCurrentFolder;
  std::string theDir = FileOperations::GetCurrentFolder();
  theGlobalVariables.ChDir("../");
  if (theGlobalVariables.buildVersionSimple == "") {
    theGlobalVariables.buildVersionSimple =
    MathRoutines::StringTrimWhiteSpace(theGlobalVariables.CallSystemWithOutput("git rev-list --count HEAD"));
  }
  theGlobalVariables.buildVersionSimple = MathRoutines::StringTrimWhiteSpace(theGlobalVariables.buildVersionSimple);
  for (unsigned i = 0; i < theGlobalVariables.buildVersionSimple.size(); i ++) {
    if (MathRoutines::isALatinLetter(theGlobalVariables.buildVersionSimple[i])) {
      theGlobalVariables.buildVersionSimple = "?";
      break;
    }
  }
  theGlobalVariables.buildHeadHashWithServerTime = MathRoutines::StringTrimWhiteSpace(
    theGlobalVariables.CallSystemWithOutput("git rev-parse HEAD")
  );
  for (unsigned i = 0; i < theGlobalVariables.buildHeadHashWithServerTime.size(); i ++) {
    if (!MathRoutines::IsAHexDigit(theGlobalVariables.buildHeadHashWithServerTime[i])) {
      theGlobalVariables.buildHeadHashWithServerTime = "x";
      break;
    }
  }
  std::stringstream buildHeadHashWithTimeStream;
  buildHeadHashWithTimeStream << theGlobalVariables.buildHeadHashWithServerTime
  << theGlobalVariables.GetGlobalTimeInSeconds();
  theGlobalVariables.buildHeadHashWithServerTime = buildHeadHashWithTimeStream.str();
  theGlobalVariables.ChDir(theDir);

  FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash().AddOnTopNoRepetitionMustBeNewCrashIfNot(
    WebAPI::request::calculatorHTML
  );
  WebAPI::request::onePageJSWithHash = FileOperations::GetVirtualNameWithHash(WebAPI::request::onePageJS);
}

void WebServer::InitializeGlobalVariables() {
  theGlobalVariables.millisecondsReplyAfterComputation = 0;
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
  this->requestsNotNeedingLogin.AddOnTop(WebAPI::request::submitExerciseNoLogin);
  this->requestsNotNeedingLogin.AddOnTop(WebAPI::request::submitExercisePreviewNoLogin);
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
  this->addressStartsNotNeedingLogin.AddOnTop("/src/");
  this->addressStartsNotNeedingLogin.AddOnTop("/output/");
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

  MapLisT<std::string, std::string, MathRoutines::HashString>&
  folderSubstitutionsNonSensitive = FileOperations::FolderVirtualLinksNonSensitive();
  MapLisT<std::string, std::string, MathRoutines::HashString>&
  folderSubstitutionsSensitive = FileOperations::FolderVirtualLinksSensitive();
  FileOperations::FolderVirtualLinksULTRASensitive(); //<- allocates data structure
  folderSubstitutionsNonSensitive.Clear();
  this->addressStartsNotNeedingLogin.AddListOnTop(
    FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash()
  );

  this->addressStartsSentWithCacheMaxAge.AddOnTop("/MathJax-2.7-latest/");
  this->addressStartsSentWithCacheMaxAge.AddOnTop("/html-common/");
  for (int i = 0; i < FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash().size; i ++) {
    this->addressStartsSentWithCacheMaxAge.AddOnTop(
      FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash()[i] +
      theGlobalVariables.buildHeadHashWithServerTime
    );
  }
  //Warning: order of substitutions is important.
  //Only the first rule that applies is applied, once.
  //No further rules are applied after that.
  folderSubstitutionsNonSensitive.SetKeyValue("/output/", "output/");//<-coming from webserver
  folderSubstitutionsNonSensitive.SetKeyValue("output/", "output/");//<-internal use
  folderSubstitutionsNonSensitive.SetKeyValue("/src/", "src/");//<-coming from webserver
  folderSubstitutionsNonSensitive.SetKeyValue("src/", "src/");//<-internal use
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

  folderSubstitutionsNonSensitive.SetKeyValue(
    "/MathJax-2.7-latest/config/mathjax-calculator-setup.js",
    "./calculator-html/mathjax-calculator-setup.js"
  );//<-coming from web server
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

int main(int argc, char **argv) {
  return WebServer::main(argc, argv);
}

extern int mainTest(List<std::string>& remainingArgs);

void WebServer::ToggleProcessMonitoring() {
  MacroRegisterFunctionWithName("WebServer::ToggleProcessMonitoring");
  if (theGlobalVariables.flagAllowProcessMonitoring) {
    WebServer::TurnProcessMonitoringOff();
  } else {
    WebServer::TurnProcessMonitoringOn();
  }
}

void WebServer::TurnProcessMonitoringOn() {
  MacroRegisterFunctionWithName("WebServer::TurnProcessMonitoringOn");
  logServer
  << logger::purple << "************************" << logger::endL
  << logger::red << "WARNING: process monitoring IS ON. " << logger::endL
  << logger::purple << "************************" << logger::endL;
  theGlobalVariables.flagAllowProcessMonitoring = true;
  theGlobalVariables.millisecondsReplyAfterComputation = theGlobalVariables.millisecondsReplyAfterComputationDefault;
  theGlobalVariables.configuration[Configuration::processMonitoringAllowedByDefault] = true;

  theGlobalVariables.ConfigurationStore();
}

void WebServer::TurnProcessMonitoringOff() {
  MacroRegisterFunctionWithName("WebServer::TurnProcessMonitoringOn");
  logServer
  << logger::green << "************************" << logger::endL
  << logger::green << "Process monitoring is now off. " << logger::endL
  << logger::green << "************************" << logger::endL;
  theGlobalVariables.flagAllowProcessMonitoring = false;
  theGlobalVariables.millisecondsReplyAfterComputation = 0;
  theGlobalVariables.configuration[Configuration::processMonitoringAllowedByDefault] = false;
  theGlobalVariables.ConfigurationStore();
}

bool GlobalVariables::ConfigurationLoad() {
  MacroRegisterFunctionWithName("GlobalVariables::ConfigurationLoad");
  std::stringstream out;
  std::string configurationFileName = "/configuration/configuration.json";
  if (!FileOperations::LoadFileToStringVirtual(
    configurationFileName, theGlobalVariables.configurationFileContent, true, false, &out
  )) {
    logServer << logger::yellow << "Failed to read configuration file. " << out.str() << logger::endL;
    std::string computedPhysicalFileName;
    if (FileOperations::GetPhysicalFileNameFromVirtual(configurationFileName, computedPhysicalFileName, true, false, 0)) {
      logServer << logger::yellow << "Computed configuration file name: " << logger::blue << computedPhysicalFileName << logger::endL;
    }
    return false;
  }
  if (!theGlobalVariables.configuration.readstring(theGlobalVariables.configurationFileContent, false, &out)) {
    logServer << logger::red << "Failed to read configuration. " << out.str() << logger::endL;
    return false;
  }
  return true;
}

bool GlobalVariables::ConfigurationStore() {
  MacroRegisterFunctionWithName("GlobalVariables::ConfigurationStore");
  std::string correctedConfiguration = theGlobalVariables.configuration.ToString(false, true);
  if (correctedConfiguration == theGlobalVariables.configurationFileContent) {
    return true;
  }
  std::fstream configurationFile;
  std::string configFileNameVirtual = "configuration/configuration.json";
  if (!FileOperations::OpenFileCreateIfNotPresentVirtual(
    configurationFile, configFileNameVirtual, false, true, false, true
  )) {
    logServer << logger::red << "Could not open file: " << configFileNameVirtual << logger::endL;
    std::string configFileNamePhysical;
    if (FileOperations::GetPhysicalFileNameFromVirtual(configFileNameVirtual, configFileNamePhysical, true, false, 0)) {
      logServer << logger::red << "Physical file name configuration: " << logger::blue << configFileNamePhysical << logger::endL;
      logServer << logger::red << "Server base: " << logger::blue << theGlobalVariables.PhysicalPathProjectBase << logger::endL;
    }
    return false;
  }
  logServer << logger::blue
  << "Configuration has been reformatted/recomputed, overwriting previous configuration file." << logger::endL;
  configurationFile << correctedConfiguration;
  return true;
}

void GlobalVariables::ConfigurationProcess() {
  MacroRegisterFunctionWithName("GlobalVariables::ConfigurationProcess");
  theGlobalVariables.flagServerDetailedLog = theGlobalVariables.configuration[
    Configuration::serverDetailedLog
  ].isTrueRepresentationInJSON();
  theGlobalVariables.flagDisableDatabaseLogEveryoneAsAdmin = theGlobalVariables.configuration[
    Configuration::disableDatabaseLogEveryoneAsAdmin
  ].isTrueRepresentationInJSON();
  if (theGlobalVariables.flagDisableDatabaseLogEveryoneAsAdmin) {
    theGlobalVariables.flagDatabaseCompiled = false;
    logServer
    << logger::purple << "************************" << logger::endL
    << logger::red << "WARNING: database disabled, " << logger::green
    << "no database operations permitted." << logger::endL
    << logger::red
    << "Everyone gets logged-in as admin. " << logger::endL
    << logger::purple << "************************" << logger::endL;
  }
  if (theGlobalVariables.configuration[Configuration::serverAutoMonitor].isTrueRepresentationInJSON()) {
    theGlobalVariables.flagServerAutoMonitor = true;
  } else {
    theGlobalVariables.flagServerAutoMonitor = false;
    if (!theGlobalVariables.flagRunningCommandLine) {
      logServer
      << logger::purple << "************************" << logger::endL
      << logger::red << "WARNING: server auto-monitoring is off. " << logger::endL
      << logger::purple << "************************" << logger::endL;
    }
  }
  if (theGlobalVariables.flagServerDetailedLog) {
    logServer
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
  theGlobalVariables.flagCachingInternalFilesOn = !theGlobalVariables.configuration[
    "serverRAMCachingOff"
  ].isTrueRepresentationInJSON();
  if (!theGlobalVariables.flagCachingInternalFilesOn && theGlobalVariables.flagRunningBuiltInWebServer) {
    logServer
    << logger::purple << "************************" << logger::endL
    << logger::red << "WARNING: caching files is off. " << logger::endL
    << "This is for development purposes only, "
    << "please do not deploy on live systems. " << logger::endL
    << "To turn off/on modify variable serverRAMCachingOff "
    << "in configuration/configuration.json." << logger::endL
    << logger::purple << "************************" << logger::endL;
  }
  theGlobalVariables.flagRunServerOnEmptyCommandLine = theGlobalVariables.configuration[
    "runServerOnEmptyCommandLine"
  ].isTrueRepresentationInJSON();
  if (
    theGlobalVariables.flagRunningCommandLine &&
    theGlobalVariables.programArguments.size == 1 &&
    theGlobalVariables.flagRunServerOnEmptyCommandLine
  ) {
    logServer << logger::green
    << "runServerOnEmptyCommandLine is set to true => Starting server with default configuration. "
    << logger::endL;
    theGlobalVariables.flagRunningBuiltInWebServer = true;
    theGlobalVariables.flagRunningCommandLine = false;
    theGlobalVariables.flagRunningApache = false;
    theGlobalVariables.millisecondsMaxComputation = 30000; // 30 seconds, default
  }
  int reader = 0;
  if (theGlobalVariables.configuration[
    Configuration::millisecondsReplyAfterComputationDefault
  ].isIntegerFittingInInt(&reader)) {
    theGlobalVariables.millisecondsReplyAfterComputationDefault = reader;
  }
  if (
    theGlobalVariables.configuration[
      Configuration::processMonitoringAllowedByDefault
    ].isTrueRepresentationInJSON()
  ) {
    logServer << logger::blue << "Process monitoring turned on from configuration.json." << logger::endL;
    WebServer::TurnProcessMonitoringOn();
  }
  if (theGlobalVariables.configuration[Configuration::useBuiltInTLS].isTrueRepresentationInJSON()) {
    logServer << logger::red << "Experimental: " << logger::blue << "using built-in TLS library." << logger::endL;
    TransportLayerSecurity::flagDontUseOpenSSL = true;
  }
  if (theGlobalVariables.configuration[Configuration::monitorPingTime].isIntegerFittingInInt(
    &theWebServer.WebServerPingIntervalInSeconds
  )) {
    if (theWebServer.WebServerPingIntervalInSeconds < 0) {
      theWebServer.WebServerPingIntervalInSeconds = 10;
    }
  } else {
    theGlobalVariables.configuration[Configuration::monitorPingTime] = theWebServer.WebServerPingIntervalInSeconds;
  }
  if (theGlobalVariables.configuration[Configuration::gitRepository].theType == JSData::token::tokenString) {
    HtmlRoutines::gitRepository = theGlobalVariables.configuration[Configuration::gitRepository].theString;
  } else {
    theGlobalVariables.configuration[Configuration::gitRepository] = HtmlRoutines::gitRepository;
  }
}

int WebServer::main(int argc, char **argv) {
  theGlobalVariables.InitThreadsExecutableStart();
  //use of loggers forbidden before calling   theWebServer.AnalyzeMainArguments(...):
  //we need to initialize first the folder locations relative to the executable.
  MacroRegisterFunctionWithName("main");
  try {
    InitializeGlobalObjects();
    theWebServer.AnalyzeMainArguments(argc, argv);
    // load and parse configuration json
    logServer << logger::green << "Project base folder: " << logger::blue << theGlobalVariables.PhysicalPathProjectBase << logger::endL;
    theGlobalVariables.ConfigurationLoad();
    if (theGlobalVariables.millisecondsMaxComputation > 0) {
      theGlobalVariables.millisecondsNoPingBeforeChildIsPresumedDead =
      theGlobalVariables.millisecondsMaxComputation + 20000; // + 20 seconds
    } else {
      theGlobalVariables.millisecondsNoPingBeforeChildIsPresumedDead = - 1;
    }
    theWebServer.InitializeGlobalVariables();
    // Compute various flags and settings from the desired configuration.
    // Correct bad configuration settings if any.
    theGlobalVariables.ConfigurationProcess();
    // Store back the config file if it changed.
    theGlobalVariables.ConfigurationStore();

    theWebServer.CheckSystemInstallationOpenSSL();
    theWebServer.CheckSystemInstallationMongoDB();
    theWebServer.CheckMongoDBSetup();
    theWebServer.CheckMathJaxSetup();
    theWebServer.CheckFreecalcSetup();

    theGlobalVariables.flagAceIsAvailable = FileOperations::FileExistsVirtual("/MathJax-2.7-latest/", false);
    if (!theGlobalVariables.flagAceIsAvailable && theGlobalVariables.flagRunningBuiltInWebServer) {
      logServer << logger::red << "MathJax not available. " << logger::endL;
    }
    if (theGlobalVariables.flagRunningBuiltInWebServer) {
      if (theGlobalVariables.millisecondsMaxComputation <= 0) {
        logServer
        << logger::purple << "************************" << logger::endL
        << logger::red << "WARNING: no computation time limit set. " << logger::endL
        << logger::purple << "************************" << logger::endL;
      }
      if (theGlobalVariables.millisecondsMaxComputation > 500000) {
        logServer
        << logger::purple << "************************" << logger::endL
        << logger::red << "WARNING: computation time limit is high: "
        << (theGlobalVariables.millisecondsMaxComputation / 1000)
        << " seconds. " << logger::endL
        << logger::purple << "************************" << logger::endL;
      }
    }
    if (theGlobalVariables.flagRunningConsoleTest) {
      return mainTest(theGlobalVariables.programArguments);
    }
    if (theGlobalVariables.flagRunningApache) {
      return WebServer::mainApache();
    }
    if (theGlobalVariables.flagRunningBuiltInWebServer) {
      return theWebServer.Run();
    }
    if (theGlobalVariables.flagRunningCommandLine) {
      return WebServer::mainCommandLine();
    }
  }
  catch (...) {
    crash << "Exception caught: something very wrong has happened. " << crash;
  }
  crash << "This point of code is not supposed to be reachable. " << crash;
  return - 1;
}

int WebServer::mainCommandLine() {
  MacroRegisterFunctionWithName("main_command_input");
  theGlobalVariables.IndicatorStringOutputFunction = HtmlRoutines::MakeStdCoutReport;
  PointerObjectDestroyer<Calculator> theDestroyer(theParser);
  theDestroyer.RenewObject();
  theParser->initialize();
  logger result("", 0, false, "server");
  if (theGlobalVariables.programArguments.size > 1) {
    for (int i = 1; i < theGlobalVariables.programArguments.size; i ++) {
      theParser->inputString += theGlobalVariables.programArguments[i];
      if (i != theGlobalVariables.programArguments.size - 1) {
        theParser->inputString += " ";
      }
    }
  } else {
    result << "Input: " << logger::yellow;
    std::cin >> theParser->inputString;
  }
  theParser->flagUseHtml = false;
  theParser->Evaluate(theParser->inputString);
  std::fstream outputFile;
  std::string outputFileName;
  if (!FileOperations::GetPhysicalFileNameFromVirtual(
    "output/outputFileCommandLine.html", outputFileName, false, false, 0
  )) {
    outputFileName = "Failed to extract output file from output/outputFileCommandLine.html";
  }
  FileOperations::OpenFileCreateIfNotPresentVirtual(
    outputFile, "output/outputFileCommandLine.html", false, true, false
  );
  result << theParser->outputString;
  outputFile << theParser->outputString;
  result << "\nTotal running time: " << logger::blue << theGlobalVariables.GetElapsedMilliseconds() << " ms. "
  << logger::endL
  << "Output written in: " << logger::green << outputFileName << logger::endL << "\n";
  return 0;
}

std::string WebServer::GetEnvironment(const std::string& envVarName) {
  char* queryStringPtr = getenv(envVarName.c_str());
  if (queryStringPtr == 0) {
    return "";
  }
  return queryStringPtr;
}

#include <sys/resource.h> //<- for setrlimit(...) function. Restricts the time the executable can run.
int WebServer::mainApache() {
  MacroRegisterFunctionWithName("main_apache");
  rlimit theLimit;
  theLimit.rlim_cur = 30;
  theLimit.rlim_max = 60;
  setrlimit(RLIMIT_CPU, &theLimit);
  stOutput.theOutputFunction = 0;
  //stOutput << "Content-Type: text/html\r\nSet-cookie: test =1;\r\n\r\nThe output bytes start here:\n";
  theGlobalVariables.IndicatorStringOutputFunction = 0;
  theGlobalVariables.flagServerForkedIntoWorker = true;
  theGlobalVariables.flagComputationStarted = true;
  theGlobalVariables.millisecondsMaxComputation = 30000; //<-30 second computation time restriction!
  theWebServer.initializeSignals();
  CreateTimerThread();
  theWebServer.CreateNewActiveWorker();
  WebWorker& theWorker = theWebServer.GetActiveWorker();
  PointerObjectDestroyer<Calculator> theDestroyer(theParser);
  theDestroyer.RenewObject();
  theParser->initialize();
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
  for (numBytesBeforeQuestionMark = 0; numBytesBeforeQuestionMark < theURL.size(); numBytesBeforeQuestionMark ++) {
    if (theURL[numBytesBeforeQuestionMark] == '?') {
      break;
    }
  }
  theGlobalVariables.DisplayNameExecutable = theURL.substr(0, numBytesBeforeQuestionMark);
  theWorker.addressGetOrPost = theGlobalVariables.DisplayNameExecutable +
  "?" + WebServer::GetEnvironment("QUERY_STRING");
  if (thePort == "443") {
    theGlobalVariables.flagUsingSSLinCurrentConnection = true;
    theGlobalVariables.flagSSLIsAvailable = true;
  }
  if (theRequestMethod == "GET") {
    theWorker.requestTypE = theWorker.requestGet;
  }
  if (theRequestMethod == "POST") {
    theWorker.requestTypE = theWorker.requestPost;
  }
  theGlobalVariables.flagComputationCompletE = true;
  MathRoutines::StringSplitExcludeDelimiter(theWorker.cookiesApache, ' ', theWorker.cookies);
  for (int i = 0; i < theWorker.cookies.size; i ++) {
    theWorker.cookies[i] = MathRoutines::StringTrimWhiteSpace(theWorker.cookies[i]);
  }
  theWorker.ServeClient();
  theGlobalVariables.flagComputationFinishedAllOutputSentClosing = true;
  return 0;
}

std::string HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable() {
  MacroRegisterFunctionWithName("WebWorker::ToStringCalculatorArgumentsHumanReadable");
  if (!theGlobalVariables.UserDebugFlagOn()) {
    return "";
  }
  std::stringstream out;
  out << "<hr>\n";
  out << "Default user: " << HtmlRoutines::ConvertStringToHtmlString(theGlobalVariables.userDefault.username, false);
  if (theGlobalVariables.flagLoggedIn) {
    out << "\n<br>\nLogged in.";
  }
  out << "\n<br>\nAddress:\n" << HtmlRoutines::ConvertStringToHtmlString(theWebServer.GetActiveWorker().addressComputed, true);
  out << "\n<br>\nRequest:\n" << theGlobalVariables.userCalculatorRequestType;
  if (theGlobalVariables.UserDefaultHasAdminRights()) {
    out << "\n<br>\n<b>User has admin rights</b>";
  }
  if (theWebServer.RequiresLogin(
    theGlobalVariables.userCalculatorRequestType, theWebServer.GetActiveWorker().addressComputed
  )) {
    out << "\n<br>\nAddress requires login. ";
  } else {
    out << "\n<br>\nAddress <b>does not</b> require any login. ";
  }
  out << "\n<hr>\n";
  for (int i = 0; i < theGlobalVariables.webArguments.size(); i ++) {
    out << theGlobalVariables.webArguments.theKeys[i] << ": "
    << HtmlRoutines::ConvertStringToHtmlString(theGlobalVariables.webArguments[i], true);
    if (i != theGlobalVariables.webArguments.size() - 1) {
      out << "\n<br>\n";
    }
  }
  out << "\n<hr>\n";
  if (theGlobalVariables.flagRunningBuiltInWebServer) {
    out << "Sent with max cache: "
    << theWebServer.addressStartsSentWithCacheMaxAge.ToStringCommaDelimited()
    << "<hr>";
    out << theWebServer.GetActiveWorker().ToStringMessageUnsafe();
  }
  return out.str();
}

void WebWorker::PrepareFullMessageHeaderAndFooter() {
  MacroRegisterFunctionWithName("WebWorker::PrepareFullMessageHeaderAndFooter");
  this->remainingBytesToSenD.SetSize(0);
  this->remainingBytesToSenD.SetExpectedSize(this->remainingBodyToSend.size + this->remainingHeaderToSend.size + 30);
  if (this->remainingHeaderToSend.size == 0) {
    if (this->requestTypE != this->requestHead) {
      this->remainingBytesToSenD = this->remainingBodyToSend;
    }
    this->remainingBodyToSend.SetSize(0);
    return;
  }
  this->remainingBytesToSenD = this->remainingHeaderToSend;
  this->remainingHeaderToSend.SetSize(0);
  std::stringstream contentLengthStream;
  if (this->flagDoAddContentLength) {
    contentLengthStream << "Content-length: " << this->remainingBodyToSend.size << "\r\n";
  }
  contentLengthStream << "\r\n";
  std::string contentLengthString = contentLengthStream.str();
  for (unsigned i = 0; i < contentLengthString.size(); i ++) {
    this->remainingBytesToSenD.AddOnTop(contentLengthString[i]);
  }
  if (this->requestTypE != this->requestHead) {
    this->remainingBytesToSenD.AddListOnTop(this->remainingBodyToSend);
  }
  this->remainingBodyToSend.SetSize(0);
}

void WebWorker::SendAllBytesWithHeaders() {
  MacroRegisterFunctionWithName("WebWorker::SendAllBytesWithHeaders");
  this->PrepareFullMessageHeaderAndFooter();
  this->SendAllBytesNoHeaders();
}

void WebWorker::SendAllBytesHttp() {
  MacroRegisterFunctionWithName("WebWorker::SendAllBytesHttp");
  if (this->remainingBytesToSenD.size == 0) {
    return;
  }
  this->CheckConsistency();
  this->flagDidSendAll = true;
  if (this->connectedSocketID == - 1) {
    logWorker << logger::red << "Socket::SendAllBytes failed: connectedSocketID = - 1." << logger::endL;
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
  while (this->remainingBytesToSenD.size > 0) {
    if (theGlobalVariables.GetElapsedSeconds() - startTime > timeOutInSeconds) {
      logWorker << "WebWorker::SendAllBytes failed: more than " << timeOutInSeconds << " seconds have elapsed. "
      << logger::endL;
      return;
    }
    int numBytesSent = send(
      this->connectedSocketID,
      &this->remainingBytesToSenD[0],
      this->remainingBytesToSenD.size,
      0
    );
    if (numBytesSent < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR || errno == EIO) {
        logIO << "WebWorker::SendAllBytes failed. Error: "
        << this->parent->ToStringLastErrorDescription() << logger::endL;
      }
      return;
    }
    if (numBytesSent == 0) {
      numTimesRunWithoutSending ++;
    } else {
      numTimesRunWithoutSending = 0;
    }
    logWorker << numBytesSent;
    this->remainingBytesToSenD.Slice(numBytesSent, this->remainingBytesToSenD.size - numBytesSent);
    if (this->remainingBytesToSenD.size > 0) {
      logWorker << ", ";
    }
    if (numTimesRunWithoutSending > 3) {
      logWorker << "WebWorker::SendAllBytes failed: send function went through 3 cycles without "
      << " sending any bytes. "
      << logger::endL;
      return;
    }
    logWorker << logger::endL;
  }
  logWorker << "All bytes sent. " << logger::endL;
}

void WebWorker::QueueBytesForSendingNoHeadeR(const List<char>& bytesToSend, bool MustSendAll) {
  MacroRegisterFunctionWithName("WebWorker::QueueBytesForSendingNoHeadeR");
  (void) MustSendAll;
  this->remainingBytesToSenD.AddListOnTop(bytesToSend);
//  if (this->remainingBytesToSend.size >=1024*512 || MustSendAll)
//    this->SendAllBytes();
}

void WebWorker::QueueStringForSendingWithHeadeR(const std::string& stringToSend, bool MustSendAll) {
  MacroRegisterFunctionWithName("WebWorker::QueueStringForSendingWithHeadeR");
  (void) MustSendAll;
  int oldSize = this->remainingBodyToSend.size;
  this->remainingBodyToSend.SetSize(this->remainingBodyToSend.size + stringToSend.size());
  for (unsigned i = 0; i < stringToSend.size(); i ++) {
    this->remainingBodyToSend[i + oldSize] = stringToSend[i];
  }
}

void WebWorker::QueueStringForSendingNoHeadeR(const std::string& stringToSend, bool MustSendAll) {
  MacroRegisterFunctionWithName("WebWorker::QueueStringForSendingNoHeadeR");
  (void) MustSendAll;
  int oldSize = this->remainingBytesToSenD.size;
  this->remainingBytesToSenD.SetSize(this->remainingBytesToSenD.size + stringToSend.size());
  for (unsigned i = 0; i < stringToSend.size(); i ++) {
    this->remainingBytesToSenD[i + oldSize] = stringToSend[i];
  }
}

void WebWorker::SendAllBytesNoHeaders() {
  MacroRegisterFunctionWithName("WebWorker::SendAllBytesNoHeaders");
  if (theGlobalVariables.flagUsingSSLinCurrentConnection) {
    this->SendAllBytesHttpSSL();
    return;
  }
  this->SendAllBytesHttp();
}
