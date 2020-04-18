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

std::string WebAPIResponse::youHaveReachedTheBackend = "You've reached the calculator's backend. ";

WebAPIResponse::WebAPIResponse() {
  this->owner = nullptr;
}

void WebAPIResponse::reset(WebWorker& inputOwner) {
  this->owner = &inputOwner;
}

bool WebAPIResponse::ServeResponseFalseIfUnrecognized(
  std::stringstream& argumentProcessingFailureComments,
  std::stringstream& comments
) {
  MacroRegisterFunctionWithName("WebAPIResponse::ServeResponseFalseIfUnrecognized");
  if (
    global.flagLoggedIn && global.UserDefaultHasAdminRights() &&
    global.requestType == WebAPI::request::database
  ) {
    return this->ProcessDatabaseJSON();
  } else if (
    global.flagLoggedIn &&
    global.requestType == "databaseDeleteOneEntry"
  ) {
    return this->ProcessDatabaseDeleteEntry();
  } else if (
    global.flagLoggedIn && global.UserDefaultHasAdminRights() &&
    global.requestType == "databaseModifyEntry"
  ) {
    return this->ProcessDatabaseModifyEntry();
  } else if (global.flagLoggedIn && global.requestType == "accountsJSON") {
    return this->ProcessAccountsJSON();
  } else if (global.requestType == WebAPI::request::examplesJSON) {
    return this->ProcessCalculatorExamplesJSON();
  } else if (global.requestType == WebAPI::request::serverStatusJSON) {
    return this->ProcessServerStatusJSON();
  }
  if (global.requestType == WebAPI::request::pause) {
    return this->ProcessPauseWorker();
  } else if (global.requestType == WebAPI::request::unpause) {
    return this->ProcessUnpauseWorker();
  } else if (global.requestType == WebAPI::request::indicator) {
    return this->ProcessComputationIndicator();
  } else if (global.requestType == WebAPI::request::setProblemWeight) {
    return this->ProcessSetProblemWeight();
  } else if (global.requestType == WebAPI::request::setProblemDeadline) {
    return this->ProcessSetProblemDeadline();
  } else if (global.requestType == WebAPI::request::changePassword) {
    return this->ProcessChangePassword(argumentProcessingFailureComments.str());
  } else if (global.requestType == WebAPI::request::activateAccountJSON) {
    return this->ProcessActivateAccount();
  } else if (global.requestType == WebAPI::request::signUp) {
    return this->ProcessSignUP();
  } else if (global.requestType == WebAPI::request::forgotLogin) {
    return this->ProcessForgotLogin();
  } else if (global.requestType == WebAPI::request::login) {
    return this->ProcessLoginUserInfo(comments.str());
  } else if (global.requestType == WebAPI::request::logout) {
    return this->ProcessLogout();
  } else if ((
      global.requestType == WebAPI::request::addEmails ||
      global.requestType == WebAPI::request::addUsers
    ) &&
    global.flagLoggedIn
  ) {
    return this->ProcessAddUserEmails();
  } else if (
    global.requestType == "setTeacher" &&
    global.flagLoggedIn
  ) {
    return this->ProcessAssignTeacherToSection();
  } else if ((
      global.requestType == WebAPI::request::submitAnswers ||
      global.requestType == WebAPI::request::submitExercise
    ) &&
    global.flagLoggedIn
  ) {
    return this->ProcessSubmitAnswers();
  } else if (global.requestType == "scores" && global.flagLoggedIn) {
    return this->ProcessScores();
  } else if (global.requestType == "scoresInCoursePage" && global.flagLoggedIn) {
    return this->ProcessScoresInCoursePage();
  } else if (global.UserGuestMode() && global.requestType == "submitExerciseNoLogin") {
    return this->ProcessSubmitAnswers();
  } else if ((
      global.requestType == WebAPI::request::problemGiveUp &&
      global.flagLoggedIn
    ) ||
    global.requestType == WebAPI::request::problemGiveUpNoLogin
  ) {
    return this->ProcessProblemGiveUp();
  } else if ((
      global.requestType == "problemSolution" &&
      global.flagLoggedIn
    ) ||
    global.requestType == "problemSolutionNoLogin"
  ) {
    return this->ProcessProblemSolution();
  } else if ((
      global.requestType == "submitAnswersPreview" ||
      global.requestType == "submitExercisePreview"
    ) &&
    global.flagLoggedIn
  ) {
    return this->ProcessSubmitAnswersPreview();
  } else if (
    global.requestType == WebAPI::request::submitExercisePreviewNoLogin &&
    global.UserGuestMode()
  ) {
    return this->ProcessSubmitAnswersPreview();
  } else if (
    global.requestType == "scoredQuiz" ||
    global.requestType == "exercise" ||
    global.requestType == "exerciseNoLogin" ||
    global.requestType == "exerciseJSON" ||
    global.requestType == "scoredQuizJSON"
  ) {
    return this->ProcessExamPageJSON();
  } else if (
    global.requestType == "templateJSON" ||
    global.requestType == WebAPI::request::templateJSONNoLogin
  ) {
    return this->ProcessTemplateJSON();
  } else if (global.requestType == WebAPI::request::userInfoJSON) {
    comments << argumentProcessingFailureComments.str();
    return this->ProcessLoginUserInfo(comments.str());
  } else if (global.requestType == WebAPI::request::editPage) {
    return this->ProcessEditPageJSON();
  } else if (global.requestType == WebAPI::request::modifyPage) {
    return this->ProcessModifyPage();
  } else if (
    global.requestType == WebAPI::request::slides::pdfFromSource ||
    global.requestType == "homeworkFromSource"
  ) {
    return this->ProcessSlidesOrHomeworkFromSource();
  } else if (
    global.requestType == WebAPI::request::slides::source ||
    global.requestType == "homeworkSource"
  ) {
    return this->ProcessSlidesSource();
  } else if (global.requestType == WebAPI::request::clonePage) {
    return this->ProcessClonePage();
  } else if (global.requestType == WebAPI::request::compute) {
    return this->ProcessCompute();
  } else if (global.requestType == WebAPI::request::selectCourseJSON) {
    return this->ProcessSelectCourseJSON();
  } else if (
    global.requestType == "topicListJSON" ||
    global.requestType == WebAPI::request::topicListJSONNoLogin
  ) {
    return this->ProcessTopicListJSON();
  } else if (global.requestType == WebAPI::app) {
    return this->ProcessApp(true);
  } else if (global.requestType == WebAPI::appNoCache) {
    return this->ProcessApp(false);
  } else if ("/" + global.requestType == WebAPI::request::onePageJS) {
    return this->ProcessCalculatorOnePageJS(false);
  } else if ("/" + global.requestType == WebAPI::request::onePageJSWithHash) {
    return this->ProcessCalculatorOnePageJS(true);
  }
  return false;
}

bool WebAPIResponse::ProcessCalculatorExamplesJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessCalculatorExamplesJSON");
  global.theResponse.WriteResponse(
    global.calculator().GetElement().ToJSONFunctionHandlers(), false
  );
  return true;
}

bool WebAPIResponse::ProcessServerStatusJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessServerStatusJSON");
  this->owner->SetHeaderOKNoContentLength("");
  std::stringstream out;
  out << " <table><tr><td style =\"vertical-align:top\">"
  << this->owner->parent->ToStringStatusAll() << "</td><td>"
  << global.ToStringHTMLTopCommandLinuxSystem()
  << "</td></tr></table>";
  JSData outputJS;
  outputJS[WebAPI::result::resultHtml] = out.str();
  return global.theResponse.WriteResponse(outputJS, false);
}

bool WebAPIResponse::ProcessUnpauseWorker() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessUnpauseWorker");
  this->owner->SetHeaderOKNoContentLength("");
  JSData progressReader, result;
  int indexWorker = this->owner->GetIndexIfRunningWorkerId(progressReader);
  if (indexWorker < 0) {
    this->owner->flagKeepAlive = false;
    return global.theResponse.WriteResponse(progressReader, false);
  }
  WebWorker& otherWorker = this->owner->parent->theWorkers[indexWorker];
  if (!otherWorker.PauseWorker.Unlock()) {
    result[WebAPI::result::error] = "Failed to unpause process";
  } else {
    result[WebAPI::result::status] = "unpaused";
  }
  return global.theResponse.WriteResponse(result, false);
}

bool WebAPIResponse::ProcessPauseWorker() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessPauseWorker");
  this->owner->SetHeaderOKNoContentLength("");
  JSData progressReader, result;
  int indexWorker = this->owner->GetIndexIfRunningWorkerId(progressReader);
  if (indexWorker < 0) {
    this->owner->flagKeepAlive = false;
    return global.theResponse.WriteResponse(progressReader, false);
  }
  WebWorker& otherWorker = this->owner->parent->theWorkers[indexWorker];
  if (otherWorker.PauseWorker.Lock()) {
    result[WebAPI::result::status] = "paused";
  } else {
    result[WebAPI::result::error] = "Failed to pause process. ";
  }
  return global.theResponse.WriteResponse(result, false);
}

bool WebAPIResponse::ProcessComputationIndicator() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessComputationIndicator");
  this->owner->SetHeaderOKNoContentLength("");
  global << "Processing get request indicator." << logger::endL;
  JSData result = this->owner->ProcessComputationIndicatorJSData();
  return global.theResponse.WriteResponse(result, false);
}

bool WebAPIResponse::ProcessSignUP() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessSignUP");
  this->owner->SetHeaderOKNoContentLength("");
  return global.theResponse.WriteResponse(this->owner->GetSignUpRequestResult(), false);
}

bool WebAPIResponse::ProcessChangePassword(const std::string& reasonForNoAuthentication) {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessChangePassword");
  (void) reasonForNoAuthentication;
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  if (!global.flagDatabaseCompiled) {
    result[WebAPI::result::error] = "Database not compiled";
    return global.theResponse.WriteResponse(result, false);
  }
  UserCalculatorData& theUser = global.userDefault;
  theUser.enteredAuthenticationToken = "";
  if (!global.flagUsingSSLinCurrentConnection) {
    result[WebAPI::result::error] = "Please use secure connection.";
    return global.theResponse.WriteResponse(result);
  }
  if (!global.flagLoggedIn) {
    result[WebAPI::result::error] = "Please enter (old) password. " + reasonForNoAuthentication;
    return global.theResponse.WriteResponse(result);
  }
  std::string newPassword = HtmlRoutines::ConvertStringToURLString(
    HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("newPassword"), true),
    false
  );
  // <-Passwords are ONE-LAYER url-encoded
  // <-INCOMING pluses in passwords MUST be decoded as spaces, this is how form.submit() works!
  // <-Incoming pluses must be re-coded as spaces (%20).

  std::string reenteredPassword = HtmlRoutines::ConvertStringToURLString(
    HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("reenteredPassword"), true),
    false
  );
  // <-Passwords are ONE-LAYER url-encoded
  // <-INCOMING pluses in passwords MUST be decoded as spaces, this is how form.submit() works!
  // <-Incoming pluses must be re-coded as spaces (%20).

  std::string newEmail = HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("email"), false);
  if (newEmail != "") {
    JSData notUsed;
    QueryExact queryEmailTaken(DatabaseStrings::tableUsers, DatabaseStrings::labelEmail, newEmail);
    if (Database::get().FindOne(
      queryEmailTaken, notUsed, nullptr
    )) {
      result[WebAPI::result::error] = "It appears the email is already taken. ";
      return global.theResponse.WriteResponse(result);
    }
  }
  if (newPassword == "" && reenteredPassword == "" && newEmail != "") {
    result = this->owner->SetEmail(newEmail);
    return global.theResponse.WriteResponse(result);
  }
  if (newPassword != reenteredPassword) {
    result[WebAPI::result::error] = "Passwords don't match. ";
    return global.theResponse.WriteResponse(result);
  }
  std::stringstream commentsOnFailure;
  std::string newAuthenticationToken;
  if (!Database::get().theUser.SetPassword(
    theUser.username,
    newPassword,
    newAuthenticationToken,
    commentsOnFailure
  )) {
    result[WebAPI::result::error] = commentsOnFailure.str();
    return global.theResponse.WriteResponse(result);
  }
  JSData setQuery;
  QueryExact findQuery(DatabaseStrings::tableUsers, DatabaseStrings::labelUsername, theUser.username);
  setQuery[DatabaseStrings::labelActivationToken] = "activated";
  if (!Database::get().UpdateOne(
    findQuery, setQuery, &commentsOnFailure
  )) {
    result[WebAPI::result::error] = "Failed to set activationToken: " +commentsOnFailure.str();
    return global.theResponse.WriteResponse(result);
  }
  std::stringstream out;
  out << "<b style = 'color:green'>Password change successful. </b>";
  if (global.GetWebInput("doReload") != "false") {
    out
    << "<meta http-equiv=\"refresh\" content =\"0; url ='"
    << global.DisplayNameExecutable  << "?request=logout"
    << "&username="
    << HtmlRoutines::ConvertStringToURLString(global.userDefault.username, false)
    << "&activationToken = &authenticationToken = &"
    << "'\" />";
  }
  result[WebAPI::result::resultHtml] = out.str();
  return global.theResponse.WriteResponse(result);
}

bool WebAPIResponse::ProcessCompute() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessCompute");
  this->owner->SetHeaderOKNoContentLength("");
  global.theResponse.flagReportDesired = true;
  if (global.GetWebInput(WebAPI::request::monitoring) == "false") {
    global.theResponse.flagReportDesired = false;
  } else {
    global.theResponse.flagReportDesired = true;
  }
  Calculator& theCalculator = global.calculator().GetElement();

  theCalculator.inputString = HtmlRoutines::ConvertURLStringToNormal(
    global.GetWebInput(WebAPI::request::calculatorInput),
    false
  );
  global.initOutputReportAndCrashFileNames(
    HtmlRoutines::ConvertStringToURLString(theCalculator.inputString, false),
    theCalculator.inputString
  );
  ////////////////////////////////////////////////
  // the initialization below moved to the start of the web server!
  // theParser.init();
  ////////////////////////////////////////////////
  global.theResponse.AllowReport();
  theCalculator.Evaluate(theCalculator.inputString);
  global.theResponse.DisallowReport();
  JSData result;
  result = theCalculator.ToJSONOutputAndSpecials();
  result[WebAPI::result::commentsGlobal] = global.Comments.getCurrentReset();
  global.theResponse.WriteResponse(result, false);
  global.flagComputationCompletE = true;
  return true;
}

bool WebAPIResponse::ProcessActivateAccount() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessActivateAccount");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  bool notUsed = false;
  result[WebAPI::result::resultHtml] = this->owner->GetChangePasswordPagePartOne(notUsed);
  return global.theResponse.WriteResponse(result);
}

bool WebAPIResponse::ProcessLogout() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessLogout");
  this->owner->SetHeaderOKNoContentLength("");
  Database::get().theUser.LogoutViaDatabase();
  return this->ProcessLoginUserInfo("Coming from logout");
}

bool WebAPIResponse::ProcessSelectCourseJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessSelectCourseJSON");
  this->owner->SetHeaderOKNoContentLength("");
  return global.theResponse.WriteResponse(WebAPIResponse::GetSelectCourseJSON());
}

bool WebAPIResponse::ProcessTopicListJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessTopicListJSON");
  this->owner->SetHeaderOKNoContentLength("");
  JSData resultJSON = WebAPIResponse::GetTopicTableJSON();
  return global.theResponse.WriteResponse(resultJSON);
}

bool WebAPIResponse::ProcessCalculatorOnePageJS(bool appendBuildHash) {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessCalculatorOnePageJS");
  if (appendBuildHash) {
    this->owner->SetHeaderOKNoContentLength(WebAPI::HeaderCacheControl, "text/javascript");
  } else {
    this->owner->SetHeaderOKNoContentLength("", "text/javascript");
  }
  this->owner->WriteToBody(WebAPIResponse::GetOnePageJS(appendBuildHash));
  this->owner->SendPending();
  return true;
}

bool WebAPIResponse::ProcessApp(bool appendBuildHash) {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessApp");
  this->owner->SetHeaderOKNoContentLength("", "text/html");
  if (global.server().RestartIsNeeded()) {
    return true;
  }
  this->owner->WriteToBody(WebAPIResponse::GetApp(appendBuildHash));
  this->owner->SendPending();
  return true;
}

bool WebAPIResponse::ProcessLoginUserInfo(const std::string& comments) {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessLoginUserInfo");
  this->owner->SetHeaderOKNoContentLength("");
  if (global.server().RestartIsNeeded()) {
    return true;
  }
  return global.theResponse.WriteResponse(WebAPIResponse::GetJSONUserInfo(comments), false);
}

bool WebAPIResponse::ProcessTemplateJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessTemplateJSON");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::GetJSONFromTemplate();
  return global.theResponse.WriteResponse(result);
}

bool WebAPIResponse::ProcessExamPageJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessExamPageJSON");
  this->owner->SetHeaderOKNoContentLength("");
  return global.theResponse.WriteResponse(WebAPIResponse::GetExamPageJSON());
}

bool WebAPIResponse::ProcessSetProblemWeight() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessSetProblemWeight");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::SetProblemWeight();
  return global.theResponse.WriteResponse(result);
}

bool WebAPIResponse::ProcessSetProblemDeadline() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessSetProblemDatabaseInfo");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::SetProblemDeadline();
  return global.theResponse.WriteResponse(result);
}

bool WebAPIResponse::ProcessAddUserEmails() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessAddUserEmails");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = this->owner->GetAddUserEmails();
  return global.theResponse.WriteResponse(result);
}

bool WebAPIResponse::ProcessModifyPage() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessModifyPage");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::ModifyProblemReport();
  return global.theResponse.WriteResponse(result);
}

bool WebAPIResponse::ProcessAssignTeacherToSection() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessAssignTeacherToSection");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::AddTeachersSections();
  return global.theResponse.WriteResponse(result);
}

bool WebAPIResponse::ProcessScores() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessScores");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::GetScoresPage();
  return global.theResponse.WriteResponse(result);
}

bool WebAPIResponse::ProcessScoresInCoursePage() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessScoresInCoursePage");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::GetScoresInCoursePage();
  return global.theResponse.WriteResponse(result);
}

bool WebAPIResponse::ProcessAccountsJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessAccountsJSON");
  this->owner->SetHeaderOKNoContentLength("");
  return global.theResponse.WriteResponse(WebAPIResponse::GetAccountsPageJSON(this->owner->hostWithPort));
}

bool WebAPIResponse::ProcessDatabaseJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessDatabaseJSON");
  this->owner->SetHeaderOKNoContentLength("");
  return global.theResponse.WriteResponse(this->owner->GetDatabaseJSON());
}

bool WebAPIResponse::ProcessDatabaseDeleteEntry() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessDatabaseDeleteEntry");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = this->owner->GetDatabaseDeleteOneItem();
  return global.theResponse.WriteResponse(this->owner->GetDatabaseJSON());
}

bool WebAPIResponse::ProcessDatabaseModifyEntry() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessDatabaseModifyEntry");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::error] = "WebAPIResponse::ProcessDatabaseModifyEntry not implemented yet";
  return global.theResponse.WriteResponse(result);
}

bool WebAPIResponse::ProcessEditPageJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessEditPageJSON");
  this->owner->SetHeaderOKNoContentLength("");
  return global.theResponse.WriteResponse(WebAPIResponse::GetEditPageJSON());
}

bool WebAPIResponse::ProcessSlidesOrHomeworkFromSource() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessSlidesOrHomeworkFromSource");
  this->owner->SetHeaderOKNoContentLength("");

  LaTeXCrawler theCrawler;
  JSData resultOnError;
  std::stringstream commentsOnFailure;
  if (!theCrawler.initializeFromGlobalVariables(&commentsOnFailure)) {
    commentsOnFailure << "Failed to process slides or homework from source. ";
    resultOnError[WebAPI::result::error] = commentsOnFailure.str();
    return global.theResponse.WriteResponse(resultOnError);
  }
  std::stringstream comments;
  if (global.GetWebInput("layout") == "printable") {
    theCrawler.flagProjectorMode = false;
  }
  if (global.GetWebInput("answerKey") == "true") {
    theCrawler.flagAnswerKey = true;
  }
  if (
    global.requestType == "homeworkFromSource" ||
    global.requestType == "homeworkSource"
  ) {
    theCrawler.flagHomeworkRatherThanSlides = true;
  }
  if (!theCrawler.BuildOrFetchFromCachePDF(&comments, &comments)) {
    resultOnError["targetPdfFileName"] = theCrawler.targetPDFFileNameWithPathVirtual;
    resultOnError[WebAPI::result::error] = comments.str();
    this->owner->flagDoAddContentLength = true;
    global.theResponse.WriteResponse(resultOnError);
    return true;
  }
  this->owner->SetHeader("HTTP/1.0 200 OK", "Content-Type: application/pdf; Access-Control-Allow-Origin: *");
  this->owner->flagDoAddContentLength = true;
  this->owner->WriteToBody(theCrawler.targetPDFbinaryContent);
  this->owner->SendPending();
  return true;
}

bool LaTeXCrawler::initializeFromGlobalVariables(std::stringstream* commentsOnFailure) {
  LaTeXCrawler::Slides theSlides;
  std::string slideSpecification = global.webArguments.GetValue(WebAPI::request::slides::content, "");
  if (!theSlides.FromString(slideSpecification, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to initialize slides from global inputs: key: " << WebAPI::request::slides::content << " is missing. ";
    }
    return false;
  }
  this->slideFileNamesVirtualWithPatH.addListOnTop(theSlides.filesToCrawl);
  this->desiredPresentationTitle = theSlides.title;
  return true;
}

bool WebAPIResponse::ProcessSlidesSource() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessSlidesSource");
  this->owner->SetHeaderOKNoContentLength("");
  LaTeXCrawler theCrawler;
  JSData result;
  std::stringstream commentsOnFailure;
  if (!theCrawler.initializeFromGlobalVariables(&commentsOnFailure)) {
    result[WebAPI::result::error] = commentsOnFailure.str();
    return global.theResponse.WriteResponse(result);
  }
  if (global.requestType == "homeworkSource") {
    theCrawler.flagHomeworkRatherThanSlides = true;
  } else {
    theCrawler.flagHomeworkRatherThanSlides = false;
  }
  std::stringstream comments;
  if (global.GetWebInput("layout") == "printable") {
    theCrawler.flagProjectorMode = false;
  }
  if (global.GetWebInput("answerKey") == "false") {
    theCrawler.flagAnswerKey = false;
  } else {
    theCrawler.flagAnswerKey = true;
  }
  theCrawler.flagSourceOnly = true;
  theCrawler.flagCrawlTexSourcesRecursively = true;
  if (!theCrawler.BuildOrFetchFromCachePDF(&comments, &comments)) {
    this->owner->flagDoAddContentLength = true;
    comments << "Failed to build your slides. ";
    result[WebAPI::result::error] = comments.str();
    return global.theResponse.WriteResponse(result);
  }
  this->owner->SetHeader("HTTP/1.0 200 OK", "Content-Type: application/x-latex; Access-Control-Allow-Origin: *");
  this->owner->flagDoAddContentLength = true;
  return this->owner->WriteToBody(theCrawler.targetLaTeX);
}

bool WebAPIResponse::ProcessClonePage() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessClonePage");
  this->owner->SetHeaderOKNoContentLength("");
  return global.theResponse.WriteResponse(this->owner->GetClonePageResult());
}

bool WebAPIResponse::ProcessProblemGiveUp() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessProblemGiveUp");
  this->owner->SetHeaderOKNoContentLength("");
  return global.theResponse.WriteResponse(WebAPIResponse::GetAnswerOnGiveUp());
}

bool WebAPIResponse::ProcessProblemSolution() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessProblemSolution");
  this->owner->SetHeaderOKNoContentLength("");
  return global.theResponse.WriteResponse(WebAPIResponse::GetProblemSolutionJSON());
}

bool WebAPIResponse::ProcessSubmitAnswersPreview() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessSubmitAnswersPreview");
  this->owner->SetHeaderOKNoContentLength("");
  return global.theResponse.WriteResponse(WebAPIResponse::SubmitAnswersPreviewJSON());
}

bool WebAPIResponse::ProcessSubmitAnswers() {
  MacroRegisterFunctionWithName("WebWorker::ProcessSubmitAnswers");
  this->owner->SetHeaderOKNoContentLength("");
  return global.theResponse.WriteResponse(WebAPIResponse::SubmitAnswersJSON());
}
