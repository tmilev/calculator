// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "web_api.h"
#include "general_logging_global_variables.h"
#include "string_constants.h"
#include "webserver.h"
#include "calculator.h"
#include "database.h"
#include "calculator_problem_storage.h"
#include "math_extra_latex_routines.h"
#include "calculator_educational_functions_1.h"

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
    global.requestType == WebAPI::request::database
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
  } else if (global.requestType == WebAPI::request::examplesJSON) {
    return this->processCalculatorExamplesJSON();
  } else if (global.requestType == WebAPI::request::serverStatusJSON) {
    return this->processServerStatusJSON();
  }
  if (global.requestType == WebAPI::request::pause) {
    return this->processPauseWorker();
  } else if (global.requestType == WebAPI::request::unpause) {
    return this->processUnpauseWorker();
  } else if (global.requestType == WebAPI::request::indicator) {
    return this->processComputationIndicator();
  } else if (global.requestType == WebAPI::request::setProblemWeight) {
    return this->processSetProblemWeight();
  } else if (global.requestType == WebAPI::request::setProblemDeadline) {
    return this->processSetProblemDeadline();
  } else if (global.requestType == WebAPI::request::changePassword) {
    return
    this->processChangePassword(argumentProcessingFailureComments.str());
  } else if (global.requestType == WebAPI::request::activateAccountJSON) {
    return this->processActivateAccount();
  } else if (global.requestType == WebAPI::request::signUp) {
    return this->processSignUp();
  } else if (global.requestType == WebAPI::request::forgotLogin) {
    return this->processForgotLogin();
  } else if (global.requestType == WebAPI::request::login) {
    return this->processLoginUserInfo(comments.str());
  } else if (global.requestType == WebAPI::request::logout) {
    return this->processLogout();
  } else if ((
      global.requestType == WebAPI::request::addEmails ||
      global.requestType == WebAPI::request::addUsers
    ) &&
    global.flagLoggedIn
  ) {
    return this->processAddUserEmails();
  } else if (
    global.requestType == WebAPI::request::setTeacher && global.flagLoggedIn
  ) {
    return this->processAssignTeacherToSection();
  } else if ((
      global.requestType == WebAPI::request::submitAnswers ||
      global.requestType == WebAPI::request::submitExercise
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
      global.requestType == WebAPI::request::problemGiveUp &&
      global.flagLoggedIn
    ) ||
    global.requestType == WebAPI::request::problemGiveUpNoLogin
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
    global.requestType == WebAPI::request::submitExercisePreviewNoLogin &&
    global.userGuestMode()
  ) {
    return this->processSubmitAnswersPreview();
  } else if (
    global.requestType == "scoredQuiz" ||
    global.requestType == "exercise" ||
    global.requestType == "exerciseNoLogin" ||
    global.requestType == "exerciseJSON" ||
    global.requestType == "scoredQuizJSON"
  ) {
    return this->processExamPageJSON();
  } else if (
    global.requestType == "templateJSON" ||
    global.requestType == WebAPI::request::templateJSONNoLogin
  ) {
    return this->processTemplateJSON();
  } else if (global.requestType == WebAPI::request::userInfoJSON) {
    comments << argumentProcessingFailureComments.str();
    return this->processLoginUserInfo(comments.str());
  } else if (global.requestType == WebAPI::request::editPage) {
    return this->processEditPageJSON(true);
  } else if (global.requestType == WebAPI::request::modifyPage) {
    return this->processModifyPage();
  } else if (
    global.requestType == WebAPI::request::slides::pdfFromSource ||
    global.requestType == WebAPI::request::homeworkFromSource
  ) {
    return this->processSlidesOrHomeworkFromSource();
  } else if (
    global.requestType == WebAPI::request::slides::source ||
    global.requestType == WebAPI::request::homeworkSource
  ) {
    return this->processSlidesSource();
  } else if (global.requestType == WebAPI::request::clonePage) {
    return this->processClonePage();
  } else if (global.requestType == WebAPI::request::compute) {
    return this->processCompute();
  } else if (global.requestType == WebAPI::request::solveRequest) {
    return this->processSolveJSON();
  } else if (global.requestType == WebAPI::request::selectCourseJSON) {
    return this->processSelectCourseJSON();
  } else if (
    global.requestType == "topicListJSON" ||
    global.requestType == WebAPI::request::topicListJSONNoLogin
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
  } else if (global.requestType == WebAPI::request::compareExpressions) {
    return this->processCompareExpressions(true);
  } else if (global.requestType == WebAPI::request::submitAnswerHardcoded) {
    return this->processSubmitAnswerHardcoded();
  } else if (
    global.requestType == WebAPI::request::compareExpressionsInternal
  ) {
    return this->processCompareExpressions(false);
  } else if ("/" + global.requestType == WebAPI::request::onePageJS) {
    return this->processCalculatorOnePageJS(false);
  } else if (
    "/" + global.requestType == WebAPI::request::onePageJSWithHash
  ) {
    return this->processCalculatorOnePageJS(true);
  } else if (
    "/" + global.requestType == WebAPI::request::calculatorWorkerJS
  ) {
    return this->processCalculatorWebWorkerJS(false);
  } else if (
    "/" + global.requestType == WebAPI::request::calculatorWorkerJSWithHash
  ) {
    return this->processCalculatorWebWorkerJS(true);
  }
  return false;
}

bool WebAPIResponse::processCalculatorExamplesJSON() {
  STACK_TRACE("WebAPIResponse::processCalculatorExamplesJSON");
  global.response.writeResponse(
    global.calculator().getElement().examples.toJSONFunctionHandlers(),
    false
  );
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
  outputJS[WebAPI::result::resultHtml] = out.str();
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
    result[WebAPI::result::error] = "Failed to unpause process";
  } else {
    result[WebAPI::result::status] = "unpaused";
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
    result[WebAPI::result::status] = "paused";
  } else {
    result[WebAPI::result::error] = "Failed to pause process. ";
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
    this->owner->getSignUpRequestResult(), false
  );
}

bool WebAPIResponse::processChangePassword(
  const std::string& reasonForNoAuthentication
) {
  STACK_TRACE("WebAPIResponse::processChangePassword");
  (void) reasonForNoAuthentication;
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  UserCalculatorData& user = global.userDefault;
  user.enteredAuthenticationToken = "";
  if (!global.flagUsingSSLinCurrentConnection) {
    result[WebAPI::result::error] = "Please use secure connection.";
    return global.response.writeResponse(result);
  }
  if (!global.flagLoggedIn) {
    result[WebAPI::result::error] =
    "Please enter (old) password. " + reasonForNoAuthentication;
    return global.response.writeResponse(result);
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
  if (newEmail != "") {
    JSData notUsed;
    QueryExact queryEmailTaken(
      DatabaseStrings::tableUsers, DatabaseStrings::labelEmail, newEmail
    );
    if (Database::get().findOne(queryEmailTaken, notUsed, nullptr)) {
      result[WebAPI::result::error] =
      "It appears the email is already taken. ";
      return global.response.writeResponse(result);
    }
  }
  if (newPassword == "" && reenteredPassword == "" && newEmail != "") {
    result = this->owner->setEmail(newEmail);
    return global.response.writeResponse(result);
  }
  if (newPassword != reenteredPassword) {
    result[WebAPI::result::error] = "Passwords don't match. ";
    return global.response.writeResponse(result);
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
    result[WebAPI::result::error] = commentsOnFailure.str();
    return global.response.writeResponse(result);
  }
  QueryExact findQuery(
    DatabaseStrings::tableUsers,
    DatabaseStrings::labelUsername,
    user.username
  );
  JSData setQuery;
  setQuery[DatabaseStrings::labelActivationToken] = "activated";
  if (
    !Database::get().updateOne(
      findQuery, QuerySet::makeFrom(setQuery), &commentsOnFailure
    )
  ) {
    result[WebAPI::result::error] =
    "Failed to set activationToken: " + commentsOnFailure.str();
    return global.response.writeResponse(result);
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
  result[WebAPI::result::resultHtml] = out.str();
  return global.response.writeResponse(result);
}

void WebAPIResponse::initializeCalculatorComputation() {
  STACK_TRACE("WebAPIResponse::initializeCalculatorComputation");
  if (this->owner != nullptr) {
    this->owner->setHeaderOKNoContentLength("");
  }
  global.response.flagReportDesired = true;
  if (global.getWebInput(WebAPI::request::monitoring) == "false") {
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
    global.getWebInput(WebAPI::request::calculatorInput), false
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
    global.getWebInput(WebAPI::request::calculatorInput), false
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
  result[WebAPI::result::solution] = solution;
  result[WebAPI::result::commentsGlobal] = global.comments.getCurrentReset();
  global.flagComputationComplete = true;
  return result;
}

JSData WebAPIResponse::compareExpressions(bool hideDesiredAnswer) {
  STACK_TRACE("WebAPIResponse::compareExpressions");
  std::string given =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::request::compareExpressionsGiven),
    false
  );
  std::string desired =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::request::compareExpressionsDesired),
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
  result[WebAPI::result::resultHtml] =
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
  result[WebAPI::result::resultHtml] = WebAPIResponse::getJSONFromTemplate();
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
  result[WebAPI::result::resultHtml] = WebAPIResponse::setProblemWeight();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processSetProblemDeadline() {
  STACK_TRACE("WebAPIResponse::processSetProblemDatabaseInfo");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::setProblemDeadline();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processAddUserEmails() {
  STACK_TRACE("WebAPIResponse::processAddUserEmails");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = this->owner->getAddUserEmails();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processModifyPage() {
  STACK_TRACE("WebAPIResponse::processModifyPage");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::modifyProblemReport();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processAssignTeacherToSection() {
  STACK_TRACE("WebAPIResponse::processAssignTeacherToSection");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::addTeachersSections();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processScores() {
  STACK_TRACE("WebAPIResponse::processScores");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::getScoresPage();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processScoresInCoursePage() {
  STACK_TRACE("WebAPIResponse::processScoresInCoursePage");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::getScoresInCoursePage();
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
  result[WebAPI::result::resultHtml] = this->owner->getDatabaseDeleteOneItem();
  return global.response.writeResponse(this->owner->getDatabaseJSON());
}

bool WebAPIResponse::processDatabaseModifyEntry() {
  STACK_TRACE("WebAPIResponse::processDatabaseModifyEntry");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::error] =
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
    resultOnError[WebAPI::result::error] = commentsOnFailure.str();
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
    resultOnError[WebAPI::result::error] = comments.str();
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
  global.webArguments.getValue(WebAPI::request::slides::content, "");
  if (!slides.fromString(slideSpecification, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to initialize slides from global inputs: key: "
      << WebAPI::request::slides::content
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
    result[WebAPI::result::error] = commentsOnFailure.str();
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
    result[WebAPI::result::error] = comments.str();
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
