// The current file is licensed under the license terms found in the main header file "calculator.h".
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

std::string WebAPIResponse::youHaveReachedTheBackend = "You've reached the calculator's backend. ";

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
  MacroRegisterFunctionWithName("WebAPIResponse::serveResponseFalseIfUnrecognized");
  if (
    global.flagLoggedIn && global.userDefaultHasAdminRights() &&
    global.requestType == WebAPI::request::database
  ) {
    return this->processDatabaseJSON();
  } else if (
    global.flagLoggedIn &&
    global.requestType == "databasedeleteOneEntry"
  ) {
    return this->processDatabaseDeleteEntry();
  } else if (
    global.flagLoggedIn && global.userDefaultHasAdminRights() &&
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
    return this->processChangePassword(argumentProcessingFailureComments.str());
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
    global.requestType == "setTeacher" &&
    global.flagLoggedIn
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
  } else if (global.requestType == "scoresInCoursePage" && global.flagLoggedIn) {
    return this->processScoresInCoursePage();
  } else if (global.userGuestMode() && global.requestType == "submitExerciseNoLogin") {
    return this->processSubmitAnswers();
  } else if ((
      global.requestType == WebAPI::request::problemGiveUp &&
      global.flagLoggedIn
    ) ||
    global.requestType == WebAPI::request::problemGiveUpNoLogin
  ) {
    return this->processProblemGiveUp();
  } else if ((
      global.requestType == "problemSolution" &&
      global.flagLoggedIn
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
    return this->processEditPageJSON();
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
  } else if (global.requestType == WebAPI::request::solveRequest){
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
  } else if (global.requestType == WebAPI::request::compareExpressionsInternal) {
    return this->processCompareExpressions(false);
  } else if ("/" + global.requestType == WebAPI::request::onePageJS) {
    return this->processCalculatorOnePageJS(false);
  } else if ("/" + global.requestType == WebAPI::request::onePageJSWithHash) {
    return this->processCalculatorOnePageJS(true);
  }
  return false;
}

bool WebAPIResponse::processCalculatorExamplesJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::processCalculatorExamplesJSON");
  global.response.writeResponse(
    global.calculator().getElement().examples.toJSONFunctionHandlers(), false
  );
  return true;
}

bool WebAPIResponse::processServerStatusJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::processServerStatusJSON");
  this->owner->setHeaderOKNoContentLength("");
  std::stringstream out;
  out << " <table><tr><td style =\"vertical-align:top\">"
  << this->owner->parent->toStringStatusAll() << "</td><td>"
  << global.toHTMLTopCommandLinuxSystem()
  << "</td></tr></table>";
  JSData outputJS;
  outputJS[WebAPI::result::resultHtml] = out.str();
  return global.response.writeResponse(outputJS, false);
}

bool WebAPIResponse::processUnpauseWorker() {
  MacroRegisterFunctionWithName("WebAPIResponse::processUnpauseWorker");
  this->owner->setHeaderOKNoContentLength("");
  JSData progressReader, result;
  int indexWorker = this->owner->getIndexIfRunningWorkerId(progressReader);
  if (indexWorker < 0) {
    this->owner->flagKeepAlive = false;
    return global.response.writeResponse(progressReader, false);
  }
  WebWorker& otherWorker = this->owner->parent->theWorkers[indexWorker];
  if (!otherWorker.PauseWorker.unlock()) {
    result[WebAPI::result::error] = "Failed to unpause process";
  } else {
    result[WebAPI::result::status] = "unpaused";
  }
  return global.response.writeResponse(result, false);
}

bool WebAPIResponse::processPauseWorker() {
  MacroRegisterFunctionWithName("WebAPIResponse::processPauseWorker");
  this->owner->setHeaderOKNoContentLength("");
  JSData progressReader, result;
  int indexWorker = this->owner->getIndexIfRunningWorkerId(progressReader);
  if (indexWorker < 0) {
    this->owner->flagKeepAlive = false;
    return global.response.writeResponse(progressReader, false);
  }
  WebWorker& otherWorker = this->owner->parent->theWorkers[indexWorker];
  if (otherWorker.PauseWorker.lock()) {
    result[WebAPI::result::status] = "paused";
  } else {
    result[WebAPI::result::error] = "Failed to pause process. ";
  }
  return global.response.writeResponse(result, false);
}

bool WebAPIResponse::processComputationIndicator() {
  MacroRegisterFunctionWithName("WebAPIResponse::processComputationIndicator");
  this->owner->setHeaderOKNoContentLength("");
  global << "Processing get request indicator." << Logger::endL;
  JSData result = this->owner->processComputationIndicatorJSData();
  return global.response.writeResponse(result, false);
}

bool WebAPIResponse::processSignUp() {
  MacroRegisterFunctionWithName("WebAPIResponse::processSignUp");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(this->owner->getSignUpRequestResult(), false);
}

bool WebAPIResponse::processChangePassword(const std::string& reasonForNoAuthentication) {
  MacroRegisterFunctionWithName("WebAPIResponse::processChangePassword");
  (void) reasonForNoAuthentication;
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  if (!global.flagDatabaseCompiled) {
    result[WebAPI::result::error] = "Database not compiled";
    return global.response.writeResponse(result, false);
  }
  UserCalculatorData& theUser = global.userDefault;
  theUser.enteredAuthenticationToken = "";
  if (!global.flagUsingSSLinCurrentConnection) {
    result[WebAPI::result::error] = "Please use secure connection.";
    return global.response.writeResponse(result);
  }
  if (!global.flagLoggedIn) {
    result[WebAPI::result::error] = "Please enter (old) password. " + reasonForNoAuthentication;
    return global.response.writeResponse(result);
  }
  std::string newPassword = HtmlRoutines::convertStringToURLString(
    HtmlRoutines::convertURLStringToNormal(global.getWebInput("newPassword"), true),
    false
  );
  // <-Passwords are ONE-LAYER url-encoded
  // <-INCOMING pluses in passwords MUST be decoded as spaces, this is how form.submit() works!
  // <-Incoming pluses must be re-coded as spaces (%20).

  std::string reenteredPassword = HtmlRoutines::convertStringToURLString(
    HtmlRoutines::convertURLStringToNormal(global.getWebInput("reenteredPassword"), true),
    false
  );
  // <-Passwords are ONE-LAYER url-encoded
  // <-INCOMING pluses in passwords MUST be decoded as spaces, this is how form.submit() works!
  // <-Incoming pluses must be re-coded as spaces (%20).

  std::string newEmail = HtmlRoutines::convertURLStringToNormal(global.getWebInput("email"), false);
  if (newEmail != "") {
    JSData notUsed;
    QueryExact queryEmailTaken(DatabaseStrings::tableUsers, DatabaseStrings::labelEmail, newEmail);
    if (Database::get().findOne(
      queryEmailTaken, notUsed, nullptr
    )) {
      result[WebAPI::result::error] = "It appears the email is already taken. ";
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
  if (!Database::get().theUser.setPassword(
    theUser.username,
    newPassword,
    newAuthenticationToken,
    commentsOnFailure
  )) {
    result[WebAPI::result::error] = commentsOnFailure.str();
    return global.response.writeResponse(result);
  }
  JSData setQuery;
  QueryExact findQuery(DatabaseStrings::tableUsers, DatabaseStrings::labelUsername, theUser.username);
  setQuery[DatabaseStrings::labelActivationToken] = "activated";
  if (!Database::get().updateOne(
    findQuery, setQuery, &commentsOnFailure
  )) {
    result[WebAPI::result::error] = "Failed to set activationToken: " +commentsOnFailure.str();
    return global.response.writeResponse(result);
  }
  std::stringstream out;
  out << "<b style = 'color:green'>Password change successful. </b>";
  if (global.getWebInput("doReload") != "false") {
    out
    << "<meta http-equiv=\"refresh\" content =\"0; url ='"
    << global.displayNameExecutable  << "?request=logout"
    << "&username="
    << HtmlRoutines::convertStringToURLString(global.userDefault.username, false)
    << "&activationToken = &authenticationToken = &"
    << "'\" />";
  }
  result[WebAPI::result::resultHtml] = out.str();
  return global.response.writeResponse(result);
}

void WebAPIResponse::initializeCalculatorComputation() {
  MacroRegisterFunctionWithName("WebAPIResponse::initializeCalculatorComputation");
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
  MacroRegisterFunctionWithName("WebAPIResponse::processCompute");
  this->initializeCalculatorComputation();
  Calculator& theCalculator = global.calculator().getElement();
  theCalculator.inputString = HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::request::calculatorInput),
    false
  );
  global.initOutputReportAndCrashFileNames(
    HtmlRoutines::convertStringToURLString(theCalculator.inputString, false),
    theCalculator.inputString
  );
  ////////////////////////////////////////////////
  // the initialization below moved to the start of the web server!
  // theParser.initialize();
  ////////////////////////////////////////////////
  global.response.allowReport();
  theCalculator.evaluate(theCalculator.inputString);
  global.response.disallowReport();
  JSData result;
  result = theCalculator.toJSONOutputAndSpecials();
  result[WebAPI::result::commentsGlobal] = global.comments.getCurrentReset();
  global.response.writeResponse(result, false);
  global.flagComputationComplete = true;
  return true;
}

JSData WebAPIResponse::solveJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::solveJSON");
  this->initializeCalculatorComputation();
  Calculator& calculator = global.calculator().getElement();
  calculator.inputString = HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::request::calculatorInput),
    false
  );
  global.initOutputReportAndCrashFileNames(
    HtmlRoutines::convertStringToURLString("SolveJSON{}(" + calculator.inputString + ")", false),
    calculator.inputString
  );
  global.response.disallowReport();
  JSData solution = calculator.solve(calculator.inputString);
  JSData result;
  result[WebAPI::result::solution] = solution;
  result[WebAPI::result::commentsGlobal] = global.comments.getCurrentReset();
  global.flagComputationComplete = true;
  result[WebAPI::result::error] = "not implemented yet.";
  return result;
}

JSData WebAPIResponse::compareExpressions(bool hideDesiredAnswer) {
  MacroRegisterFunctionWithName("WebAPIResponse::compareExpressions");
  std::string given = HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::request::compareExpressionsGiven),
    false
  );
  std::string desired = HtmlRoutines::convertURLStringToNormal(
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
  MacroRegisterFunctionWithName("WebAPIResponse::processActivateAccount");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  bool notUsed = false;
  result[WebAPI::result::resultHtml] = this->owner->getChangePasswordPagePartOne(notUsed);
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processLogout() {
  MacroRegisterFunctionWithName("WebAPIResponse::processLogout");
  this->owner->setHeaderOKNoContentLength("");
  Database::get().theUser.logoutViaDatabase();
  return this->processLoginUserInfo("Coming from logout");
}

bool WebAPIResponse::processSelectCourseJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::processSelectCourseJSON");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(WebAPIResponse::getSelectCourseJSON());
}

bool WebAPIResponse::processTopicListJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::processTopicListJSON");
  this->owner->setHeaderOKNoContentLength("");
  JSData resultJSON = WebAPIResponse::getTopicTableJSON();
  return global.response.writeResponse(resultJSON);
}

bool WebAPIResponse::processSolveJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::processSolveJSON");
  this->owner->setHeaderOKNoContentLength("");
  JSData resultJSON = this->solveJSON();
  return global.response.writeResponse(resultJSON);
}

bool WebAPIResponse::processCompareExpressions(bool hideDesiredAnswer) {
  MacroRegisterFunctionWithName("WebAPIResponse::processCompareExpressions");
  this->owner->setHeaderOKNoContentLength("", "text/html");
  JSData resultJSON = this->compareExpressions(hideDesiredAnswer);
  return global.response.writeResponse(resultJSON);
  return true;
}

bool WebAPIResponse::processCheckAnswer(bool hideDesiredAnswer) {
  MacroRegisterFunctionWithName("WebAPIResponse::processCheckAnswer");
  this->owner->setHeaderOKNoContentLength("", "text/html");
  JSData resultJSON = WebAPIResponse::checkAnswer(hideDesiredAnswer);
  return global.response.writeResponse(resultJSON);
  return true;
}

bool WebAPIResponse::processCalculatorOnePageJS(bool appendBuildHash) {
  MacroRegisterFunctionWithName("WebAPIResponse::processCalculatorOnePageJS");
  if (appendBuildHash) {
    this->owner->setHeaderOKNoContentLength(WebAPI::headerCacheControl, "text/javascript");
  } else {
    this->owner->setHeaderOKNoContentLength("", "text/javascript");
  }
  this->owner->writeToBody(WebAPIResponse::getOnePageJS());
  this->owner->sendPending();
  return true;
}

bool WebAPIResponse::processApp(bool appendBuildHash) {
  MacroRegisterFunctionWithName("WebAPIResponse::processApp");
  this->owner->setHeaderOKNoContentLength("", "text/html");
  this->owner->writeToBody(WebAPIResponse::getApp(appendBuildHash));
  this->owner->sendPending();
  return true;
}

bool WebAPIResponse::processCompareExpressionsPage(bool appendBuildHash) {
  MacroRegisterFunctionWithName("WebAPIResponse::processCompareExpressionsPage");
  this->owner->setHeaderOKNoContentLength("", "text/html");
  this->owner->writeToBody(WebAPIResponse::getCompareExpressionsPage(appendBuildHash));
  this->owner->sendPending();
  return true;
}

bool WebAPIResponse::processLoginUserInfo(const std::string& comments) {
  MacroRegisterFunctionWithName("WebAPIResponse::processLoginUserInfo");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(WebAPIResponse::getJSONUserInfo(comments), false);
}

bool WebAPIResponse::processTemplateJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::processTemplateJSON");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::getJSONFromTemplate();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processExamPageJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::processExamPageJSON");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(WebAPIResponse::getExamPageJSON());
}

bool WebAPIResponse::processSetProblemWeight() {
  MacroRegisterFunctionWithName("WebAPIResponse::processSetProblemWeight");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::setProblemWeight();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processSetProblemDeadline() {
  MacroRegisterFunctionWithName("WebAPIResponse::processSetProblemDatabaseInfo");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::setProblemDeadline();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processAddUserEmails() {
  MacroRegisterFunctionWithName("WebAPIResponse::processAddUserEmails");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = this->owner->getAddUserEmails();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processModifyPage() {
  MacroRegisterFunctionWithName("WebAPIResponse::processModifyPage");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::modifyProblemReport();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processAssignTeacherToSection() {
  MacroRegisterFunctionWithName("WebAPIResponse::processAssignTeacherToSection");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::addTeachersSections();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processScores() {
  MacroRegisterFunctionWithName("WebAPIResponse::processScores");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::getScoresPage();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processScoresInCoursePage() {
  MacroRegisterFunctionWithName("WebAPIResponse::processScoresInCoursePage");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::getScoresInCoursePage();
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processAccountsJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::processAccountsJSON");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(WebAPIResponse::getAccountsPageJSON(this->owner->hostWithPort));
}

bool WebAPIResponse::processDatabaseJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::processDatabaseJSON");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(this->owner->getDatabaseJSON());
}

bool WebAPIResponse::processDatabaseDeleteEntry() {
  MacroRegisterFunctionWithName("WebAPIResponse::processDatabaseDeleteEntry");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = this->owner->getDatabaseDeleteOneItem();
  return global.response.writeResponse(this->owner->getDatabaseJSON());
}

bool WebAPIResponse::processDatabaseModifyEntry() {
  MacroRegisterFunctionWithName("WebAPIResponse::processDatabaseModifyEntry");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::error] = "WebAPIResponse::processDatabaseModifyEntry not implemented yet";
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processEditPageJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::processEditPageJSON");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(WebAPIResponse::getEditPageJSON());
}

bool WebAPIResponse::processSlidesOrHomeworkFromSource() {
  MacroRegisterFunctionWithName("WebAPIResponse::processSlidesOrHomeworkFromSource");
  this->owner->setHeaderOKNoContentLength("");

  LaTeXCrawler theCrawler;
  JSData resultOnError;
  std::stringstream commentsOnFailure;
  if (!theCrawler.initializeFromGlobalVariables(&commentsOnFailure)) {
    commentsOnFailure << "Failed to process slides or homework from source. ";
    resultOnError[WebAPI::result::error] = commentsOnFailure.str();
    return global.response.writeResponse(resultOnError);
  }
  std::stringstream comments;
  if (global.getWebInput("layout") == "printable") {
    theCrawler.flagProjectorMode = false;
  }
  if (global.getWebInput("answerKey") == "true") {
    theCrawler.flagAnswerKey = true;
  }
  if (
    global.requestType == "homeworkFromSource" ||
    global.requestType == "homeworkSource"
  ) {
    theCrawler.flagHomeworkRatherThanSlides = true;
  }
  if (!theCrawler.buildOrFetchFromCachePDF(&comments, &comments)) {
    resultOnError["targetPdfFileName"] = theCrawler.targetPDFFileNameWithPathVirtual;
    resultOnError[WebAPI::result::error] = comments.str();
    this->owner->flagDoAddContentLength = true;
    global.response.writeResponse(resultOnError);
    return true;
  }
  this->owner->setHeader("HTTP/1.0 200 OK", "Content-Type: application/pdf; Access-Control-Allow-Origin: *");
  this->owner->flagDoAddContentLength = true;
  this->owner->writeToBody(theCrawler.targetPDFbinaryContent);
  this->owner->sendPending();
  return true;
}

bool LaTeXCrawler::initializeFromGlobalVariables(std::stringstream* commentsOnFailure) {
  LaTeXCrawler::Slides theSlides;
  std::string slideSpecification = global.webArguments.getValue(WebAPI::request::slides::content, "");
  if (!theSlides.fromString(slideSpecification, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to initialize slides from global inputs: key: " << WebAPI::request::slides::content << " is missing. ";
    }
    return false;
  }
  this->slideFileNamesVirtualWithPatH.addListOnTop(theSlides.filesToCrawl);
  this->desiredPresentationTitle = theSlides.title;
  return true;
}

bool WebAPIResponse::processSlidesSource() {
  MacroRegisterFunctionWithName("WebAPIResponse::processSlidesSource");
  this->owner->setHeaderOKNoContentLength("");
  LaTeXCrawler theCrawler;
  JSData result;
  std::stringstream commentsOnFailure;
  if (!theCrawler.initializeFromGlobalVariables(&commentsOnFailure)) {
    result[WebAPI::result::error] = commentsOnFailure.str();
    return global.response.writeResponse(result);
  }
  if (global.requestType == "homeworkSource") {
    theCrawler.flagHomeworkRatherThanSlides = true;
  } else {
    theCrawler.flagHomeworkRatherThanSlides = false;
  }
  std::stringstream comments;
  if (global.getWebInput("layout") == "printable") {
    theCrawler.flagProjectorMode = false;
  }
  if (global.getWebInput("answerKey") == "false") {
    theCrawler.flagAnswerKey = false;
  } else {
    theCrawler.flagAnswerKey = true;
  }
  theCrawler.flagSourceOnly = true;
  theCrawler.flagCrawlTexSourcesRecursively = true;
  if (!theCrawler.buildOrFetchFromCachePDF(&comments, &comments)) {
    this->owner->flagDoAddContentLength = true;
    comments << "Failed to build your slides. ";
    result[WebAPI::result::error] = comments.str();
    return global.response.writeResponse(result);
  }
  this->owner->setHeader("HTTP/1.0 200 OK", "Content-Type: application/x-latex; Access-Control-Allow-Origin: *");
  this->owner->flagDoAddContentLength = true;
  return this->owner->writeToBody(theCrawler.targetLaTeX);
}

bool WebAPIResponse::processClonePage() {
  MacroRegisterFunctionWithName("WebAPIResponse::processClonePage");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(this->owner->getClonePageResult());
}

bool WebAPIResponse::processProblemGiveUp() {
  MacroRegisterFunctionWithName("WebAPIResponse::processProblemGiveUp");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(WebAPIResponse::getAnswerOnGiveUp());
}

bool WebAPIResponse::processPing() {
  MacroRegisterFunctionWithName("WebAPIResponse::processPing");
  this->owner->setHeaderOKNoContentLength("");
  JSData result;
  result["ping"] = "ok";
  this->owner->flagKeepAlive = false;
  this->owner->statistics.pingReceives ++;
  return global.response.writeResponse(result);
}

bool WebAPIResponse::processProblemSolution() {
  MacroRegisterFunctionWithName("WebAPIResponse::processProblemSolution");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(WebAPIResponse::getProblemSolutionJSON());
}

bool WebAPIResponse::processSubmitAnswersPreview() {
  MacroRegisterFunctionWithName("WebAPIResponse::processSubmitAnswersPreview");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(WebAPIResponse::submitAnswersPreviewJSON());
}

bool WebAPIResponse::processSubmitAnswers() {
  MacroRegisterFunctionWithName("WebWorker::processSubmitAnswers");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(WebAPIResponse::submitAnswersJSON());
}

bool WebAPIResponse::processSubmitAnswerHardcoded() {
  MacroRegisterFunctionWithName("WebWorker::processSubmitAnswerHardcoded");
  this->owner->setHeaderOKNoContentLength("");
  return global.response.writeResponse(WebAPIResponse::submitAnswersHardcoded(true));
}
