#include "calculator_educational_functions_1.h"
#include "database.h"
#include "general_logging_global_variables.h"
#include "math_extra_latex_routines.h"
#include "string_constants.h"
#include "user.h"
#include "web_api.h"
#include "web_client.h"
#include "webserver.h"

std::string WebAPIResponse::youHaveReachedTheBackend =
"You've reached the calculator's backend. ";

WebAPIResponse::WebAPIResponse() {
  this->owner = nullptr;
}

void WebAPIResponse::reset(WebWorker& inputOwner) {
  this->owner = &inputOwner;
}

bool WebAPIResponse::serveResponseFalseIfUnrecognized(
  std::stringstream& argumentProcessingFailureComments,
  std::stringstream& comments
) {
  STACK_TRACE("WebAPIResponse::serveResponseFalseIfUnrecognized");
  if (
    global.flagLoggedIn &&
    global.userDefaultHasAdminRights() &&
    global.requestType == WebAPI::Request::database
  ) {
    return this->processDatabaseJSON();
  } else if (
    global.flagLoggedIn && global.requestType == "databasedeleteOneEntry"
  ) {
    return this->processDatabaseDeleteEntry();
  } else if (
    global.flagLoggedIn &&
    global.userDefaultHasAdminRights() &&
    global.requestType == "databaseModifyEntry"
  ) {
    return this->processDatabaseModifyEntry();
  } else if (global.flagLoggedIn && global.requestType == "accountsJSON") {
    return this->processAccountsJSON();
  } else if (global.requestType == WebAPI::Request::examplesJSON) {
    return this->processCalculatorExamplesJSON();
  } else if (global.requestType == WebAPI::Request::serverStatusJSON) {
    return this->processServerStatusJSON();
  }
  if (global.requestType == WebAPI::Request::pause) {
    return this->processPauseWorker();
  } else if (global.requestType == WebAPI::Request::unpause) {
    return this->processUnpauseWorker();
  } else if (global.requestType == WebAPI::Request::indicator) {
    return this->processComputationIndicator();
  } else if (global.requestType == WebAPI::Request::setProblemWeight) {
    return this->processSetProblemWeight();
  } else if (global.requestType == WebAPI::Request::setProblemDeadline) {
    return this->processSetProblemDeadline();
  } else if (global.requestType == WebAPI::Request::changePassword) {
    return
    this->processChangePassword(argumentProcessingFailureComments.str());
  } else if (global.requestType == WebAPI::Request::activateAccountJSON) {
    return this->processActivateAccount();
  } else if (global.requestType == WebAPI::Request::signUp) {
    return this->processSignUp();
  } else if (global.requestType == WebAPI::Request::forgotLogin) {
    return this->processForgotLogin();
  } else if (global.requestType == WebAPI::Request::login) {
    return this->processLoginUserInfo(comments.str());
  } else if (global.requestType == WebAPI::Request::logout) {
    return this->processLogout();
  } else if ((
      global.requestType == WebAPI::Request::addEmails ||
      global.requestType == WebAPI::Request::addUsers
    ) &&
    global.flagLoggedIn
  ) {
    return this->processAddUsersOrEmails();
  } else if (
    global.requestType == WebAPI::Request::setTeacher && global.flagLoggedIn
  ) {
    return this->processAssignTeacherToSection();
  } else if ((
      global.requestType == WebAPI::Request::submitAnswers ||
      global.requestType == WebAPI::Request::submitExercise
    ) &&
    global.flagLoggedIn
  ) {
    return this->processSubmitAnswers();
  } else if (global.requestType == "scores" && global.flagLoggedIn) {
    return this->processScores();
  } else if (
    global.requestType == "scoresInCoursePage" && global.flagLoggedIn
  ) {
    return this->processScoresInCoursePage();
  } else if (
    global.userGuestMode() && global.requestType == "submitExerciseNoLogin"
  ) {
    return this->processSubmitAnswers();
  } else if ((
      global.requestType == WebAPI::Request::problemGiveUp &&
      global.flagLoggedIn
    ) ||
    global.requestType == WebAPI::Request::problemGiveUpNoLogin
  ) {
    return this->processProblemGiveUp();
  } else if ((
      global.requestType == "problemSolution" && global.flagLoggedIn
    ) ||
    global.requestType == "problemSolutionNoLogin"
  ) {
    return this->processProblemSolution();
  } else if ((
      global.requestType == "submitAnswersPreview" ||
      global.requestType == "submitExercisePreview"
    ) &&
    global.flagLoggedIn
  ) {
    return this->processSubmitAnswersPreview();
  } else if (
    global.requestType == WebAPI::Request::submitExercisePreviewNoLogin &&
    global.userGuestMode()
  ) {
    return this->processSubmitAnswersPreview();
  } else if (
    global.requestType == "scoredQuiz" ||
    global.requestType == "exercise" ||
    global.requestType == "exerciseNoLogin" ||
    global.requestType == "exerciseJSON" ||
    global.requestType == WebAPI::Frontend::scoredQuiz
  ) {
    return this->processExamPageJSON();
  } else if (
    global.requestType == "templateJSON" ||
    global.requestType == WebAPI::Request::templateJSONNoLogin
  ) {
    return this->processTemplateJSON();
  } else if (global.requestType == WebAPI::Request::userInfoJSON) {
    comments << argumentProcessingFailureComments.str();
    return this->processLoginUserInfo(comments.str());
  } else if (global.requestType == WebAPI::Request::editPage) {
    return this->processEditPageJSON(true);
  } else if (global.requestType == WebAPI::Request::modifyPage) {
    return this->processModifyPage();
  } else if (
    global.requestType == WebAPI::Request::Slides::pdfFromSource ||
    global.requestType == WebAPI::Request::homeworkFromSource
  ) {
    return this->processSlidesOrHomeworkFromSource();
  } else if (
    global.requestType == WebAPI::Request::Slides::source ||
    global.requestType == WebAPI::Request::homeworkSource
  ) {
    return this->processSlidesSource();
  } else if (global.requestType == WebAPI::Request::clonePage) {
    return this->processClonePage();
  } else if (global.requestType == WebAPI::Request::compute) {
    return this->processCompute();
  } else if (global.requestType == WebAPI::Request::solveRequest) {
    return this->processSolveJSON();
  } else if (global.requestType == WebAPI::Request::selectCourseJSON) {
    return this->processSelectCourseJSON();
  } else if (
    global.requestType == "topicListJSON" ||
    global.requestType == WebAPI::Request::topicListJSONNoLogin
  ) {
    return this->processTopicListJSON();
  } else if (global.requestType == WebAPI::app) {
    return this->processApp(true);
  } else if (global.requestType == WebAPI::appNoCache) {
    return this->processApp(false);
  } else if (global.requestType == WebAPI::compareExpressionsPage) {
    return this->processCompareExpressionsPage(true);
  } else if (global.requestType == WebAPI::compareExpressionsPageNoCache) {
    return this->processCompareExpressionsPage(false);
  } else if (global.requestType == WebAPI::Request::compareExpressions) {
    return this->processCompareExpressions(true);
  } else if (global.requestType == WebAPI::Request::submitAnswerHardcoded) {
    return this->processSubmitAnswerHardcoded();
  } else if (
    global.requestType == WebAPI::Request::compareExpressionsInternal
  ) {
    return this->processCompareExpressions(false);
  } else if ("/" + global.requestType == WebAPI::Request::onePageJS) {
    return this->processCalculatorOnePageJS(false);
  } else if (
    "/" + global.requestType == WebAPI::Request::onePageJSWithHash
  ) {
    return this->processCalculatorOnePageJS(true);
  } else if (
    "/" + global.requestType == WebAPI::Request::calculatorWorkerJS
  ) {
    return this->processCalculatorWebWorkerJS(false);
  } else if (
    "/" + global.requestType == WebAPI::Request::calculatorWorkerJSWithHash
  ) {
    return this->processCalculatorWebWorkerJS(true);
  }
  return false;
}

bool WebAPIResponse::processCalculatorExamplesJSON() {
  STACK_TRACE("WebAPIResponse::processCalculatorExamplesJSON");
  JSData response =
  global.calculator().getElement().examples.toJSONFunctionHandlersAndExamples(
  );
  global.response.writeResponse(response, false);
  return true;
}

bool WebAPIResponse::processServerStatusJSON() {
  STACK_TRACE("WebAPIResponse::processServerStatusJSON");
  this->owner->setHeaderOKNoContentLength("");
  std::stringstream out;
  out
  << " <table><tr><td style =\"vertical-align:top\">"
  << this->owner->parent->toStringStatusAll()
  << "</td><td>"
  << global.toHTMLTopCommandLinuxSystem()
  << "</td></tr></table>";
  JSData outputJS;
  outputJS[WebAPI::Result::resultHtml] = out.str();
  return global.response.writeResponse(outputJS, false);
}

bool WebAPIResponse::processUnpauseWorker() {
  STACK_TRACE("WebAPIResponse::processUnpauseWorker");
  this->owner->setHeaderOKNoContentLength("");
  JSData progressReader, result;
  int indexWorker = this->owner->getIndexIfRunningWorkerId(progressReader);
  if (indexWorker < 0) {
    this->owner->flagKeepAlive = false;
    return global.response.writeResponse(progressReader, false);
  }
  WebWorker& otherWorker = this->owner->parent->allWorkers[indexWorker];
  if (!otherWorker.pauseWorker.unlock()) {
    result[WebAPI::Result::error] = "Failed to unpause process";
  } else {
    result[WebAPI::Result::status] = "unpaused";
  }
  return global.response.writeResponse(result, false);
}

bool WebAPIResponse::processPauseWorker() {
  STACK_TRACE("WebAPIResponse::processPauseWorker");
  this->owner->setHeaderOKNoContentLength("");
  JSData progressReader, result;
  int indexWorker = this->owner->getIndexIfRunningWorkerId(progressReader);
  if (indexWorker < 0) {
    this->owner->flagKeepAlive = false;
    return global.response.writeResponse(progressReader, false);
  }
  WebWorker& otherWorker = this->owner->parent->allWorkers[indexWorker];
  if (otherWorker.pauseWorker.lock()) {
    result[WebAPI::Result::status] = "paused";
  } else {
    result[WebAPI::Result::error] = "Failed to pause process. ";
  }
  return global.response.writeResponse(result, false);
}

bool WebAPIResponse::processComputationIndicator() {
  STACK_TRACE("WebAPIResponse::processComputationIndicator");
  this->owner->setHeaderOKNoContentLength("");
  global << "Processing get request indicator." << Logger::endL;
  JSData result = this->owner->processComputationIndicatorJSData();
  return global.response.writeResponse(result, false);
}

bool WebAPIResponse::processSignUp() {
  STACK_TRACE("WebAPIResponse::processSignUp");
  this->owner->setHeaderOKNoContentLength("");
  return
  global.response.writeResponse(
    WebAPIResponse::getSignUpRequestResult(), false
  );
}

bool WebAPIResponse::processChangePassword(
  const std::string& reasonForNoAuthentication
) {
  STACK_TRACE("WebAPIResponse::processChangePassword");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  WebAPIResponse::changePassword(result, reasonForNoAuthentication);
  return global.response.writeResponse(result);
}

void WebAPIResponse::changePassword(
  JSData& result, const std::string& reasonForNoAuthentication
) {
  STACK_TRACE("WebAPIResponse::changePassword");
  (void) reasonForNoAuthentication;
  UserCalculatorData& user = global.userDefault;
  user.enteredAuthenticationToken = "";
  if (!global.flagUsingSSLinCurrentConnection) {
    result[WebAPI::Result::error] = "Please use secure connection.";
    return;
  }
  if (!global.flagLoggedIn) {
    result[WebAPI::Result::error] =
    "Please enter (old) password [correctly]. " + reasonForNoAuthentication;
    return;
  }
  std::string newPassword =
  HtmlRoutines::convertStringToURLString(
    HtmlRoutines::convertURLStringToNormal(
      global.getWebInput("newPassword"), true
    ),
    false
  );
  // <-Passwords are ONE-LAYER url-encoded
  // <-INCOMING pluses in passwords MUST be decoded as spaces, this is how
  // form.submit() works!
  // <-Incoming pluses must be re-coded as spaces (%20).
  std::string reenteredPassword =
  HtmlRoutines::convertStringToURLString(
    HtmlRoutines::convertURLStringToNormal(
      global.getWebInput("reenteredPassword"), true
    ),
    false
  );
  // <-Passwords are ONE-LAYER url-encoded
  // <-INCOMING pluses in passwords MUST be decoded as spaces, this is how
  // form.submit() works!
  // <-Incoming pluses must be re-coded as spaces (%20).
  std::string newEmail =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput("email"), false
  );
  List<JSData> list;
  if (newEmail != "") {
    JSData notUsed;
    QueryFind queryEmailTaken(
      DatabaseStrings::tableUsers, DatabaseStrings::labelEmail, newEmail
    );
    bool success =
    Database::get().find(queryEmailTaken, nullptr, list, nullptr);
    if (!success) {
      result[WebAPI::Result::error] = "Database operation failed. ";
      return;
    }
    if (list.size > 0) {
      result[WebAPI::Result::error] =
      "It appears the email is already taken. ";
      return;
    }
  }
  if (newPassword == "" && reenteredPassword == "" && newEmail != "") {
    result = WebAPIResponse::setEmail(newEmail);
    return;
  }
  if (newPassword != reenteredPassword) {
    result[WebAPI::Result::error] = "Passwords don't match. ";
    return;
  }
  std::stringstream commentsOnFailure;
  std::string newAuthenticationToken;
  if (
    !Database::get().user.setPassword(
      user.username,
      newPassword,
      newAuthenticationToken,
      commentsOnFailure
    )
  ) {
    result[WebAPI::Result::error] = commentsOnFailure.str();
    return;
  }
  QueryFind findQuery(
    DatabaseStrings::tableUsers,
    DatabaseStrings::labelUsername,
    user.username
  );
  JSData setQuery;
  QueryUpdate doSetQuery;
  doSetQuery.addKeyValueStringPair(
    DatabaseStrings::labelActivationToken, "activated"
  );
  if (
    !Database::get().updateOne(
      findQuery, doSetQuery, false, &commentsOnFailure
    )
  ) {
    result[WebAPI::Result::error] =
    "Failed to set activationToken: " + commentsOnFailure.str();
    return;
  }
  std::stringstream out;
  out << "Password change successful.";
  if (global.getWebInput("doReload") != "false") {
    out
    << "<meta http-equiv=\"refresh\" content =\"0; url ='"
    << global.displayNameExecutable
    << "?request=logout"
    << "&username="
    << HtmlRoutines::convertStringToURLString(
      global.userDefault.username, false
    )
    << "&activationToken = &authenticationToken = &"
    << "'\" />";
  }
  result[WebAPI::Result::resultHtml] = out.str();
}

JSData WebAPIResponse::setEmail(const std::string& input) {
  STACK_TRACE("WebAPIResponse::setEmail");
  (void) input;
  JSData result;
  if (global.hasDisabledDatabaseEveryoneIsAdmin()) {
    result[WebAPI::Result::error] =
    "Set email failed. " + DatabaseStrings::errorDatabaseDisabled;
    return result;
  }
  std::stringstream out;
  std::stringstream debugStream;
  global.userDefault.email = input;
  std::stringstream* adminOutputStream = nullptr;
  std::stringstream errorStream;
  if (global.flagDebugLogin) {
    adminOutputStream = &errorStream;
  }
  if (
    !WebAPIResponse::doSetEmail(
      global.userDefault, &errorStream, &out, adminOutputStream
    )
  ) {
    result[WebAPI::Result::error] = errorStream.str();
  }
  if (global.flagDebugLogin) {
    out << "Debug login information. " << debugStream.str();
  }
  out << "Response time: " << global.getElapsedSeconds() << " second(s).";
  result[WebAPI::Result::comments] = out.str();
  return result;
}

bool WebAPIResponse::doSetEmail(
  UserCalculatorData& inputOutputUser,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive,
  std::stringstream* commentsGeneralSensitive
) {
  STACK_TRACE("WebWorker::doSetEmail");
  (void) inputOutputUser;
  (void) commentsOnFailure;
  (void) commentsGeneralNonSensitive;
  (void) commentsGeneralSensitive;
  if (commentsGeneralNonSensitive != nullptr) {
    *commentsGeneralNonSensitive
    << "doSetEmail: project compiled without database support. ";
  }
  EmailRoutines email;
  email.toEmail = inputOutputUser.email;
  if (!email.isOKEmail(email.toEmail, commentsOnFailure)) {
    return false;
  }
  UserCalculator userCopy;
  userCopy.UserCalculatorData::operator=(inputOutputUser);
  userCopy.email = inputOutputUser.email;
  if (
    !userCopy.computeAndStoreActivationEmailAndTokens(
      commentsOnFailure, commentsGeneralNonSensitive
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to compute and store your activation email. ";
    }
    return false;
  }
  email.emailContent = userCopy.activationEmail;
  email.subject = userCopy.activationEmailSubject;
  if (commentsGeneralNonSensitive != nullptr) {
    *commentsGeneralNonSensitive << "Sending email... ";
  }
  bool success =
  email.sendEmailWithMailGun(
    commentsOnFailure, commentsGeneralNonSensitive, commentsGeneralSensitive
  );
  if (commentsGeneralSensitive != nullptr) {
    if (global.flagDebugLogin) {
      *commentsGeneralSensitive
      << "Content of sent email (debug login): "
      << HtmlRoutines::convertStringToHtmlString(email.emailContent, true);
    }
  } else if (commentsGeneralNonSensitive != nullptr) {
    *commentsGeneralNonSensitive << "Email content not displayed. ";
  }
  userCopy.clearAuthenticationTokenAndPassword();
  userCopy.actualActivationToken = "";
  inputOutputUser = userCopy;
  return success;
}

void WebAPIResponse::initializeCalculatorComputation() {
  STACK_TRACE("WebAPIResponse::initializeCalculatorComputation");
  if (this->owner != nullptr) {
    this->owner->setHeaderOKNoContentLength("");
  }
  global.response.flagReportDesired = true;
  if (global.getWebInput(WebAPI::Request::monitoring) == "false") {
    global.response.flagReportDesired = false;
  } else {
    global.response.flagReportDesired = true;
  }
}

bool WebAPIResponse::processCompute() {
  STACK_TRACE("WebAPIResponse::processCompute");
  this->initializeCalculatorComputation();
  Calculator& calculator = global.calculator().getElement();
  calculator.inputString =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Request::calculatorInput), false
  );
  global.initOutputReportAndCrashFileNames(
    HtmlRoutines::convertStringToURLString(calculator.inputString, false),
    calculator.inputString
  );
  // The parser used to be initialized here.
  // It has since been moved to the start of the web server.
  global.response.allowReport();
  calculator.evaluate(calculator.inputString);
  global.response.disallowReport();
  JSData result;
  result = calculator.toJSONOutputAndSpecials();
  global.response.writeResponse(result, false);
  global.flagComputationComplete = true;
  return true;
}

JSData WebAPIResponse::solveJSON() {
  STACK_TRACE("WebAPIResponse::solveJSON");
  this->initializeCalculatorComputation();
  Calculator& calculator = global.calculator().getElement();
  calculator.inputString =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Request::calculatorInput), false
  );
  global.initOutputReportAndCrashFileNames(
    HtmlRoutines::convertStringToURLString(
      "SolveJSON{}(" + calculator.inputString + ")", false
    ),
    calculator.inputString
  );
  global.response.disallowReport();
  JSData solution = calculator.solve(calculator.inputString);
  JSData result;
  result[WebAPI::Result::solution] = solution;
  result[WebAPI::Result::commentsGlobal] = global.comments.getCurrentReset();
  global.flagComputationComplete = true;
  return result;
}

JSData WebAPIResponse::compareExpressions(bool hideDesiredAnswer) {
  STACK_TRACE("WebAPIResponse::compareExpressions");
  std::string given =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Request::compareExpressionsGiven),
    false
  );
  std::string desired =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Request::compareExpressionsDesired),
    false
  );
  this->initializeCalculatorComputation();
  Calculator& calculator = global.calculator().getElement();
  CompareExpressions comparison(hideDesiredAnswer);
  comparison.compare(given, desired, calculator);
  return comparison.toJSON();
}

bool WebAPIResponse::processActivateAccount() {
  STACK_TRACE("WebAPIResponse::processActivateAccount");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  bool notUsed = false;
  result[WebAPI::Result::resultHtml] =
  this->owner->getChangePasswordPagePartOne(notUsed);
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processLogout() {
  STACK_TRACE("WebAPIResponse::processLogout");
  this->owner->setHeaderOKNoContentLength("");
  Database::get().user.logoutViaDatabase();
  return this->processLoginUserInfo("Coming from logout");
}

bool WebAPIResponse::processSelectCourseJSON() {
  STACK_TRACE("WebAPIResponse::processSelectCourseJSON");
  this->owner->setHeaderOKNoContentLength("");
  return
  global.response.writeResponse(WebAPIResponse::getSelectCourseJSON());
}

bool WebAPIResponse::processTopicListJSON() {
  STACK_TRACE("WebAPIResponse::processTopicListJSON");
  this->owner->setHeaderOKNoContentLength("");
  JSData resultJSON = WebAPIResponse::getTopicTableJSON();
  return global.response.writeResponse(resultJSON);
}

bool WebAPIResponse::processSolveJSON() {
  STACK_TRACE("WebAPIResponse::processSolveJSON");
  this->owner->setHeaderOKNoContentLength("");
  JSData resultJSON = this->solveJSON();
  return global.response.writeResponse(resultJSON);
}

bool WebAPIResponse::processCompareExpressions(bool hideDesiredAnswer) {
  STACK_TRACE("WebAPIResponse::processCompareExpressions");
  this->owner->setHeaderOKNoContentLength("", "text/html");
  JSData resultJSON = this->compareExpressions(hideDesiredAnswer);
  return global.response.writeResponse(resultJSON);
  return true;
}

bool WebAPIResponse::processCheckAnswer(bool hideDesiredAnswer) {
  STACK_TRACE("WebAPIResponse::processCheckAnswer");
  this->owner->setHeaderOKNoContentLength("", "text/html");
  JSData resultJSON = WebAPIResponse::checkAnswer(hideDesiredAnswer);
  return global.response.writeResponse(resultJSON);
  return true;
}

bool WebAPIResponse::processCalculatorWebWorkerJS(bool appendBuildHash) {
  STACK_TRACE("WebAPIResponse::processCalculatorWebWorkerJS");
  if (appendBuildHash) {
    this->owner->setHeaderOKNoContentLength(
      WebAPI::headerCacheControl, "text/javascript"
    );
  } else {
    this->owner->setHeaderOKNoContentLength("", "text/javascript");
  }
  this->owner->writeToBody(WebAPIResponse::getCalculatorWorkerJS());
  this->owner->sendPending();
  return true;
}

bool WebAPIResponse::processCalculatorOnePageJS(bool appendBuildHash) {
  STACK_TRACE("WebAPIResponse::processCalculatorOnePageJS");
  if (appendBuildHash) {
    this->owner->setHeaderOKNoContentLength(
      WebAPI::headerCacheControl, "text/javascript"
    );
  } else {
    this->owner->setHeaderOKNoContentLength("", "text/javascript");
  }
  this->owner->writeToBody(WebAPIResponse::getOnePageJS());
  this->owner->sendPending();
  return true;
}

bool WebAPIResponse::processApp(bool appendBuildHash) {
  STACK_TRACE("WebAPIResponse::processApp");
  this->owner->setHeaderOKNoContentLength("", "text/html");
  this->owner->writeToBody(WebAPIResponse::getApp(appendBuildHash));
  this->owner->sendPending();
  return true;
}

bool WebAPIResponse::processCompareExpressionsPage(bool appendBuildHash) {
  STACK_TRACE("WebAPIResponse::processCompareExpressionsPage");
  this->owner->setHeaderOKNoContentLength("", "text/html");
  this->owner->writeToBody(
    WebAPIResponse::getCompareExpressionsPage(appendBuildHash)
  );
  this->owner->sendPending();
  return true;
}

bool WebAPIResponse::processLoginUserInfo(const std::string& comments) {
  STACK_TRACE("WebAPIResponse::processLoginUserInfo");
  this->owner->setHeaderOKNoContentLength("");
  return
  global.response.writeResponse(
    WebAPIResponse::getJSONUserInfo(comments), false
  );
}

bool WebAPIResponse::processTemplateJSON() {
  STACK_TRACE("WebAPIResponse::processTemplateJSON");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::Result::resultHtml] = WebAPIResponse::getJSONFromTemplate();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processExamPageJSON() {
  STACK_TRACE("WebAPIResponse::processExamPageJSON");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(WebAPIResponse::getExamPageJSON());
}

bool WebAPIResponse::processSetProblemWeight() {
  STACK_TRACE("WebAPIResponse::processSetProblemWeight");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::Result::resultHtml] = WebAPIResponse::setProblemWeight();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processSetProblemDeadline() {
  STACK_TRACE("WebAPIResponse::processSetProblemDatabaseInfo");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::Result::resultHtml] = WebAPIResponse::setProblemDeadline();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processAddUsersOrEmails() {
  STACK_TRACE("WebAPIResponse::processAddUsersOrEmails");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::Result::resultHtml] =
  WebAPIResponse::addUsersOrEmails(this->owner->hostWithPort);
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processModifyPage() {
  STACK_TRACE("WebAPIResponse::processModifyPage");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::Result::resultHtml] = WebAPIResponse::modifyProblemReport();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processAssignTeacherToSection() {
  STACK_TRACE("WebAPIResponse::processAssignTeacherToSection");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::Result::resultHtml] = WebAPIResponse::addTeachersSections();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processScores() {
  STACK_TRACE("WebAPIResponse::processScores");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::Result::resultHtml] = WebAPIResponse::getScoresPage();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processScoresInCoursePage() {
  STACK_TRACE("WebAPIResponse::processScoresInCoursePage");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::Result::resultHtml] = WebAPIResponse::getScoresInCoursePage();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processAccountsJSON() {
  STACK_TRACE("WebAPIResponse::processAccountsJSON");
  this->owner->setHeaderOKNoContentLength("");
  return
  global.response.writeResponse(
    WebAPIResponse::getAccountsPageJSON(this->owner->hostWithPort)
  );
}

bool WebAPIResponse::processDatabaseJSON() {
  STACK_TRACE("WebAPIResponse::processDatabaseJSON");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(this->owner->getDatabaseJSON());
}

bool WebAPIResponse::processDatabaseDeleteEntry() {
  STACK_TRACE("WebAPIResponse::processDatabaseDeleteEntry");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::Result::resultHtml] = this->owner->getDatabaseDeleteOneItem();
  return global.response.writeResponse(this->owner->getDatabaseJSON());
}

bool WebAPIResponse::processDatabaseModifyEntry() {
  STACK_TRACE("WebAPIResponse::processDatabaseModifyEntry");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::Result::error] =
  "WebAPIResponse::processDatabaseModifyEntry not implemented yet";
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processEditPageJSON(bool showSourceRelaxed) {
  STACK_TRACE("WebAPIResponse::processEditPageJSON");
  this->owner->setHeaderOKNoContentLength("");
  return
  global.response.writeResponse(
    WebAPIResponse::getEditPageJSON(showSourceRelaxed)
  );
}

bool WebAPIResponse::processSlidesOrHomeworkFromSource() {
  STACK_TRACE("WebAPIResponse::processSlidesOrHomeworkFromSource");
  this->owner->setHeaderOKNoContentLength("");
  LaTeXCrawler latexCrawler;
  JSData resultOnError;
  std::stringstream commentsOnFailure;
  if (!latexCrawler.initializeFromGlobalVariables(&commentsOnFailure)) {
    commentsOnFailure << "Failed to process slides or homework from source. ";
    resultOnError[WebAPI::Result::error] = commentsOnFailure.str();
    return global.response.writeResponse(resultOnError);
  }
  std::stringstream comments;
  if (global.getWebInput("layout") == "printable") {
    latexCrawler.flagProjectorMode = false;
  }
  if (global.getWebInput("answerKey") == "true") {
    latexCrawler.flagAnswerKey = true;
  }
  if (
    global.requestType == "homeworkFromSource" ||
    global.requestType == "homeworkSource"
  ) {
    latexCrawler.flagHomeworkRatherThanSlides = true;
  }
  if (!latexCrawler.buildOrFetchFromCachePDF(&comments, &comments)) {
    resultOnError["targetPdfFileName"] =
    latexCrawler.targetPDFFileNameWithPathVirtual;
    resultOnError[WebAPI::Result::error] = comments.str();
    this->owner->flagDoAddContentLength = true;
    global.response.writeResponse(resultOnError);
    return true;
  }
  this->owner->setHeader(
    "HTTP/1.0 200 OK",
    "Content-Type: application/pdf; Access-Control-Allow-Origin: *"
  );
  this->owner->flagDoAddContentLength = true;
  this->owner->writeToBody(latexCrawler.targetPDFbinaryContent);
  this->owner->sendPending();
  return true;
}

bool LaTeXCrawler::initializeFromGlobalVariables(
  std::stringstream* commentsOnFailure
) {
  LaTeXCrawler::Slides slides;
  std::string slideSpecification =
  global.webArguments.getValue(WebAPI::Request::Slides::content, "");
  if (!slides.fromString(slideSpecification, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to initialize slides from global inputs: key: "
      << WebAPI::Request::Slides::content
      << " is missing. ";
    }
    return false;
  }
  this->slideFileNamesVirtualWithPath.addListOnTop(slides.filesToCrawl);
  this->desiredPresentationTitle = slides.title;
  return true;
}

bool WebAPIResponse::processSlidesSource() {
  STACK_TRACE("WebAPIResponse::processSlidesSource");
  this->owner->setHeaderOKNoContentLength("");
  LaTeXCrawler latexCrawler;
  JSData result;
  std::stringstream commentsOnFailure;
  if (!latexCrawler.initializeFromGlobalVariables(&commentsOnFailure)) {
    result[WebAPI::Result::error] = commentsOnFailure.str();
    return global.response.writeResponse(result);
  }
  if (global.requestType == "homeworkSource") {
    latexCrawler.flagHomeworkRatherThanSlides = true;
  } else {
    latexCrawler.flagHomeworkRatherThanSlides = false;
  }
  std::stringstream comments;
  if (global.getWebInput("layout") == "printable") {
    latexCrawler.flagProjectorMode = false;
  }
  if (global.getWebInput("answerKey") == "false") {
    latexCrawler.flagAnswerKey = false;
  } else {
    latexCrawler.flagAnswerKey = true;
  }
  latexCrawler.flagSourceOnly = true;
  latexCrawler.flagCrawlTexSourcesRecursively = true;
  if (!latexCrawler.buildOrFetchFromCachePDF(&comments, &comments)) {
    this->owner->flagDoAddContentLength = true;
    comments << "Failed to build your slides. ";
    result[WebAPI::Result::error] = comments.str();
    return global.response.writeResponse(result);
  }
  this->owner->setHeader(
    "HTTP/1.0 200 OK",
    "Content-Type: application/x-latex; Access-Control-Allow-Origin: *"
  );
  this->owner->flagDoAddContentLength = true;
  return this->owner->writeToBody(latexCrawler.targetLaTeX);
}

bool WebAPIResponse::processClonePage() {
  STACK_TRACE("WebAPIResponse::processClonePage");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(this->owner->getClonePageResult());
}

bool WebAPIResponse::processProblemGiveUp() {
  STACK_TRACE("WebAPIResponse::processProblemGiveUp");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(WebAPIResponse::getAnswerOnGiveUp());
}

bool WebAPIResponse::processPing() {
  STACK_TRACE("WebAPIResponse::processPing");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result["ping"] = "ok";
  this->owner->flagKeepAlive = false;
  this->owner->statistics.pingReceives ++;
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processProblemSolution() {
  STACK_TRACE("WebAPIResponse::processProblemSolution");
  this->owner->setHeaderOKNoContentLength("");
  return
  global.response.writeResponse(WebAPIResponse::getProblemSolutionJSON());
}

bool WebAPIResponse::processSubmitAnswersPreview() {
  STACK_TRACE("WebAPIResponse::processSubmitAnswersPreview");
  this->owner->setHeaderOKNoContentLength("");
  return
  global.response.writeResponse(WebAPIResponse::submitAnswersPreviewJSON());
}

bool WebAPIResponse::processSubmitAnswers() {
  STACK_TRACE("WebWorker::processSubmitAnswers");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(WebAPIResponse::submitAnswersJSON());
}

bool WebAPIResponse::processSubmitAnswerHardcoded() {
  STACK_TRACE("WebWorker::processSubmitAnswerHardcoded");
  this->owner->setHeaderOKNoContentLength("");
  return
  global.response.writeResponse(
    WebAPIResponse::submitAnswersHardcoded(true)
  );
}

bool WebAPIResponse::addUsersFromData(
  const std::string& emailList,
  const std::string& userPasswords,
  const std::string& userRole,
  const std::string& userGroup,
  int& outputNumberOfNewUsers,
  int& outputNumberOfUpdatedUsers,
  std::stringstream* comments
) {
  STACK_TRACE("DatabaseUserRoutines::addUsersFromEmails");
  global.millisecondsMaxComputation = 100000;
  // 100 seconds
  global.millisecondsReplyAfterComputation = 200000;
  // 200 seconds
  List<std::string> emails;
  List<std::string> allPasswords;
  StringRoutines::stringSplitDefaultDelimiters(emailList, emails);
  StringRoutines::stringSplitDefaultDelimiters(userPasswords, allPasswords);
  if (allPasswords.size > 0 && allPasswords.size != emails.size) {
    if (comments != nullptr) {
      *comments
      << "Different number of usernames/emails and passwords: "
      << emails.size
      << " emails and "
      << allPasswords.size
      << " passwords. "
      << "If you want to enter usernames without password, "
      << "you need to leave the password input box empty. ";
      return false;
    }
  }
  outputNumberOfNewUsers = 0;
  outputNumberOfUpdatedUsers = 0;
  for (int i = 0; i < emails.size; i ++) {
    std::string currentPassword = "";
    if (allPasswords.size > 0) {
      currentPassword = allPasswords[i];
    }
    if (
      !WebAPIResponse::addOneUser(
        emails[i],
        currentPassword,
        userRole,
        userGroup,
        outputNumberOfNewUsers,
        outputNumberOfUpdatedUsers,
        comments
      )
    ) {
      return false;
    }
  }
  bool doSendEmails = false;
  bool result = true;
  if (doSendEmails) {
    std::stringstream* commentsGeneralSensitive = nullptr;
    if (global.userDefaultHasAdminRights()) {
      commentsGeneralSensitive = comments;
    }
    if (
      !DatabaseUserRoutines::sendActivationEmail(
        emails, comments, comments, commentsGeneralSensitive
      )
    ) {
      result = false;
    }
  }
  return result;
}

bool WebAPIResponse::addOneUser(
  const std::string& userNameOrEmail,
  const std::string& password,
  const std::string& desiredUserRole,
  const std::string& userGroup,
  int& outputNumberOfNewUsers,
  int& outputNumberOfUpdatedUsers,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("WebAPIResponse::addOneUser");
  std::string email = userNameOrEmail;
  // If it looks like email, assume it's email.
  // The user addition comes from admin user,
  // so let's trust the input within reason.
  if (!EmailRoutines::isOKEmail(email, commentsOnFailure)) {
    // Not an email.
    email = "";
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "I am assuming you are adding a username "
      << "that is not an email and proceeding bravely. ";
    }
  }
  QueryFindOneOf findUserByUsernameOrEmail;
  QueryFind findUserByUsername =
  QueryFind(
    DatabaseStrings::tableUsers,
    DatabaseStrings::labelUsername,
    userNameOrEmail
  );
  findUserByUsernameOrEmail.queries.addOnTop(findUserByUsername);
  if (email != "") {
    findUserByUsernameOrEmail.queries.addOnTop(
      QueryFind(
        DatabaseStrings::tableUsers, DatabaseStrings::labelEmail, email
      )
    );
  }
  List<JSData> allUsers;
  if (
    !Database::get().find(
      findUserByUsernameOrEmail, nullptr, allUsers, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Database error! ";
    }
    return false;
  }
  if (allUsers.size > 1) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Found more than one match for: ["
      << userNameOrEmail
      << "]. "
      << "Something is wrong with the database entries, please "
      << "contact an administrator of the site or file a bug report. "
      << "For best results, take a screenshot of how you got this message.";
    }
    return false;
  }
  std::string userRole = desiredUserRole;
  if (allUsers.size == 1) {
    UserCalculator user;
    if (!user.loadFromJSON(allUsers[0])) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "I found an existing user: ["
        << userNameOrEmail
        << "] but failed to load the user's data from the database. "
        << "Something is wrong with the database entries, please "
        << "contact an administrator of the site or file a bug report.";
      }
      return false;
    }
    if (
      user.userRole == UserCalculatorData::Roles::administrator &&
      userRole != user.userRole
    ) {
      userRole = user.userRole;
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "User "
        << userNameOrEmail
        << " is an administrator, so I am not downgrading the user's role to "
        << desiredUserRole;
      }
    }
    outputNumberOfUpdatedUsers ++;
  } else {
    outputNumberOfNewUsers ++;
  }
  UserCalculator currentUser;
  currentUser.username = userNameOrEmail;
  currentUser.email = email;
  currentUser.courseInDB = global.getWebInput(WebAPI::Problem::courseHome);
  currentUser.sectionInDB = userGroup;
  currentUser.instructorInDB = global.userDefault.username;
  currentUser.userRole = userRole;
  QueryUpdate updateUser;
  updateUser.addValue(currentUser.toJSON());
  if (
    !Database::get().updateOne(
      findUserByUsername, updateUser, true, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to update user: " << currentUser.username;
    }
    return false;
  }
  if (password == "") {
    if (
      currentUser.actualHashedSaltedPassword == "" &&
      currentUser.actualAuthenticationToken == ""
    ) {
      if (!currentUser.computeAndStoreActivationToken(commentsOnFailure)) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "Failed to store activation token: "
          << currentUser.username;
        }
        return false;
      }
    }
    return true;
  }
  currentUser.enteredPassword =
  HtmlRoutines::convertStringToURLString(password, false);
  // <-Passwords are ONE-LAYER url-encoded
  // <-INCOMING pluses in passwords MUST be decoded as spaces, this is how
  // form.submit() works!
  // <-Incoming pluses must be re-coded as spaces (%20).
  if (!currentUser.setPassword(commentsOnFailure)) {
    return false;
  }
  QueryUpdate updateActivation;
  updateActivation.addKeyValueStringPair(
    DatabaseStrings::labelActivationToken, "activated"
  );
  if (
    !Database::get().updateOne(
      findUserByUsername, updateActivation, false, commentsOnFailure
    )
  ) {
    return false;
  }
  if (currentUser.email == "") {
    return true;
  }
  return
  currentUser.computeAndStoreActivationStats(
    commentsOnFailure, commentsOnFailure
  );
}

JSData WebAPIResponse::getSignUpRequestResult() {
  STACK_TRACE("WebWorker::getSignUpRequestResult");
  JSData result;
  std::stringstream errorStream;
  std::stringstream generalCommentsStream;
  Database::get().user.logoutViaDatabase();
  UserCalculator user;
  user.username =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput("desiredUsername"), false
  );
  user.email =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput("email"), false
  );
  std::stringstream outputStream;
  if (
    !WebAPIResponse::verifyRecaptcha(
      &errorStream, &generalCommentsStream, nullptr
    )
  ) {
    result[WebAPI::Result::error] = errorStream.str();
    result[WebAPI::Result::comments] = generalCommentsStream.str();
    return result;
  }
  if (user.username == "") {
    errorStream << "Empty username not allowed. ";
    result[WebAPI::Result::error] = errorStream.str();
    result[WebAPI::Result::comments] = generalCommentsStream.str();
    return result;
  }
  if (!EmailRoutines::isOKEmail(user.email, &generalCommentsStream)) {
    errorStream << "Your email address does not appear to be valid. ";
    result[WebAPI::Result::error] = errorStream.str();
    result[WebAPI::Result::comments] = generalCommentsStream.str();
    return result;
  }
  bool userExists = false;
  bool databaseIsOK = false;
  user.exists(userExists, databaseIsOK, nullptr);
  if (!databaseIsOK) {
    errorStream << "Database is down. ";
    result[WebAPI::Result::error] = errorStream.str();
    result[WebAPI::Result::comments] = generalCommentsStream.str();
    return result;
  } else if (!userExists) {
    errorStream
    << "Either the username ("
    << user.username
    << ") or the email ("
    << user.email
    << ") you requested is already taken.";
    result[WebAPI::Result::error] = errorStream.str();
    result[WebAPI::Result::comments] = generalCommentsStream.str();
    return result;
  } else {
    outputStream
    << "Username ("
    << user.username
    << ") with email ("
    << user.email
    << ") is available.";
  }
  if (!user.storeToDatabase(false, &errorStream)) {
    errorStream << "Failed to store error stream. ";
    result[WebAPI::Result::error] = errorStream.str();
    result[WebAPI::Result::comments] = generalCommentsStream.str();
    result[WebAPI::Result::resultLabel] = outputStream.str();
    return result;
  }
  std::stringstream* adminOutputStream = nullptr;
  if (global.flagDebugLogin) {
    adminOutputStream = &generalCommentsStream;
  }
  WebAPIResponse::doSetEmail(
    user, &errorStream, &generalCommentsStream, adminOutputStream
  );
  result[WebAPI::Result::error] = errorStream.str();
  result[WebAPI::Result::comments] = generalCommentsStream.str();
  result[WebAPI::Result::resultHtml] = outputStream.str();
  return result;
}
