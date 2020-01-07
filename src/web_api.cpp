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

static ProjectInformationInstance projectInfoWebApiCPP(__FILE__, "Web api.");

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
    global.userCalculatorRequestType == WebAPI::request::database
  ) {
    return this->ProcessDatabaseJSON();
  } else if (
    global.flagLoggedIn &&
    global.userCalculatorRequestType == "databaseDeleteOneEntry"
  ) {
    return this->ProcessDatabaseDeleteEntry();
  } else if (
    global.flagLoggedIn && global.UserDefaultHasAdminRights() &&
    global.userCalculatorRequestType == "databaseModifyEntry"
  ) {
    return this->ProcessDatabaseModifyEntry();
  } else if (global.flagLoggedIn && global.userCalculatorRequestType == "accountsJSON") {
    return this->ProcessAccountsJSON();
  } else if (global.userCalculatorRequestType == WebAPI::request::examplesJSON) {
    return this->ProcessCalculatorExamplesJSON();
  } else if (global.userCalculatorRequestType == WebAPI::request::serverStatusJSON) {
    return this->ProcessServerStatusJSON();
  }
  if (global.userCalculatorRequestType == WebAPI::request::pause) {
    return this->ProcessPauseWorker();
  } else if (global.userCalculatorRequestType == WebAPI::request::unpause) {
    return this->ProcessUnpauseWorker();
  } else if (global.userCalculatorRequestType == WebAPI::request::indicator) {
    return this->ProcessComputationIndicator();
  } else if (global.userCalculatorRequestType == WebAPI::request::setProblemWeight) {
    return this->ProcessSetProblemWeight();
  } else if (global.userCalculatorRequestType == WebAPI::request::setProblemDeadline) {
    return this->ProcessSetProblemDeadline();
  } else if (global.userCalculatorRequestType == WebAPI::request::changePassword) {
    return this->ProcessChangePassword(argumentProcessingFailureComments.str());
  } else if (global.userCalculatorRequestType == WebAPI::request::activateAccountJSON) {
    return this->ProcessActivateAccount();
  } else if (global.userCalculatorRequestType == WebAPI::request::signUp) {
    return this->ProcessSignUP();
  } else if (global.userCalculatorRequestType == WebAPI::request::forgotLogin) {
    return this->ProcessForgotLogin();
  } else if (global.userCalculatorRequestType == WebAPI::request::login) {
    return this->ProcessLoginUserInfo(comments.str());
  } else if (global.userCalculatorRequestType == WebAPI::request::logout) {
    return this->ProcessLogout();
  } else if ((
      global.userCalculatorRequestType == WebAPI::request::addEmails ||
      global.userCalculatorRequestType == WebAPI::request::addUsers
    ) &&
    global.flagLoggedIn
  ) {
    return this->ProcessAddUserEmails();
  } else if (
    global.userCalculatorRequestType == "setTeacher" &&
    global.flagLoggedIn
  ) {
    return this->ProcessAssignTeacherToSection();
  } else if ((
      global.userCalculatorRequestType == WebAPI::request::submitAnswers ||
      global.userCalculatorRequestType == WebAPI::request::submitExercise
    ) &&
    global.flagLoggedIn
  ) {
    return this->ProcessSubmitAnswers();
  } else if (global.userCalculatorRequestType == "scores" && global.flagLoggedIn) {
    return this->ProcessScores();
  } else if (global.userCalculatorRequestType == "scoresInCoursePage" && global.flagLoggedIn) {
    return this->ProcessScoresInCoursePage();
  } else if (global.UserGuestMode() && global.userCalculatorRequestType == "submitExerciseNoLogin") {
    return this->ProcessSubmitAnswers();
  } else if ((
      global.userCalculatorRequestType == WebAPI::request::problemGiveUp &&
      global.flagLoggedIn
    ) ||
    global.userCalculatorRequestType == WebAPI::request::problemGiveUpNoLogin
  ) {
    return this->ProcessProblemGiveUp();
  } else if ((
      global.userCalculatorRequestType == "problemSolution" &&
      global.flagLoggedIn
    ) ||
    global.userCalculatorRequestType == "problemSolutionNoLogin"
  ) {
    return this->ProcessProblemSolution();
  } else if ((
      global.userCalculatorRequestType == "submitAnswersPreview" ||
      global.userCalculatorRequestType == "submitExercisePreview"
    ) &&
    global.flagLoggedIn
  ) {
    return this->ProcessSubmitAnswersPreview();
  } else if (
    global.userCalculatorRequestType == WebAPI::request::submitExercisePreviewNoLogin &&
    global.UserGuestMode()
  ) {
    return this->ProcessSubmitAnswersPreview();
  } else if (
    global.userCalculatorRequestType == "scoredQuiz" ||
    global.userCalculatorRequestType == "exercise" ||
    global.userCalculatorRequestType == "exerciseNoLogin" ||
    global.userCalculatorRequestType == "exerciseJSON" ||
    global.userCalculatorRequestType == "scoredQuizJSON"
  ) {
    return this->ProcessExamPageJSON();
  } else if (
    global.userCalculatorRequestType == "templateJSON" ||
    global.userCalculatorRequestType == WebAPI::request::templateJSONNoLogin
  ) {
    return this->ProcessTemplateJSON();
  } else if (global.userCalculatorRequestType == WebAPI::request::userInfoJSON) {
    comments << argumentProcessingFailureComments.str();
    return this->ProcessLoginUserInfo(comments.str());
  } else if (global.userCalculatorRequestType == WebAPI::request::editPage) {
    return this->ProcessEditPageJSON();
  } else if (global.userCalculatorRequestType == WebAPI::request::modifyPage) {
    return this->ProcessModifyPage();
  } else if (
    global.userCalculatorRequestType == "slidesFromSource" ||
    global.userCalculatorRequestType == "homeworkFromSource"
  ) {
    return this->ProcessSlidesOrHomeworkFromSource();
  } else if (
    global.userCalculatorRequestType == "slidesSource" ||
    global.userCalculatorRequestType == "homeworkSource"
  ) {
    return this->ProcessSlidesSource();
  } else if (global.userCalculatorRequestType == WebAPI::request::clonePage) {
    return this->ProcessClonePage();
  } else if (global.userCalculatorRequestType == WebAPI::request::compute) {
    return this->ProcessCompute();
  } else if (global.userCalculatorRequestType == WebAPI::request::selectCourseJSON) {
    return this->ProcessSelectCourseJSON();
  } else if (
    global.userCalculatorRequestType == "topicListJSON" ||
    global.userCalculatorRequestType == WebAPI::request::topicListJSONNoLogin
  ) {
    return this->ProcessTopicListJSON();
  } else if (global.userCalculatorRequestType == WebAPI::app) {
    return this->ProcessApp(true);
  } else if (global.userCalculatorRequestType == WebAPI::appNoCache) {
    return this->ProcessApp(false);
  } else if ("/" + global.userCalculatorRequestType == WebAPI::request::onePageJS) {
    return this->ProcessCalculatorOnePageJS(false);
  } else if ("/" + global.userCalculatorRequestType == WebAPI::request::onePageJSWithHash) {
    return this->ProcessCalculatorOnePageJS(true);
  }
  return false;
}

bool WebAPIResponse::ProcessCalculatorExamplesJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessCalculatorExamplesJSON");
  global.theProgress.WriteResponse(global.calculator().GetElement().ToJSONFunctionHandlers());
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
  return global.theProgress.WriteResponse(outputJS);
}

bool WebAPIResponse::ProcessUnpauseWorker() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessUnpauseWorker");
  this->owner->SetHeaderOKNoContentLength("");
  JSData progressReader, result;
  int indexWorker = this->owner->GetIndexIfRunningWorkerId(progressReader);
  if (indexWorker < 0) {
    return global.theProgress.WriteResponse(progressReader);
  }
  WebWorker& otherWorker = this->owner->parent->theWorkers[indexWorker];
  if (!otherWorker.PauseWorker.Unlock()) {
    result[WebAPI::result::error] = "Failed to unpause process";
  } else {
    result[WebAPI::result::status] = "unpaused";
  }
  return global.theProgress.WriteResponse(result);
}

bool WebAPIResponse::ProcessPauseWorker() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessPauseWorker");
  this->owner->SetHeaderOKNoContentLength("");
  JSData progressReader, result;
  int indexWorker = this->owner->GetIndexIfRunningWorkerId(progressReader);
  if (indexWorker < 0) {
    return global.theProgress.WriteResponse(progressReader);
  }
  WebWorker& otherWorker = this->owner->parent->theWorkers[indexWorker];
  if (otherWorker.PauseWorker.Lock()) {
    result[WebAPI::result::status] = "paused";
  } else {
    result[WebAPI::result::error] = "Failed to pause process. ";
  }
  return global.theProgress.WriteResponse(result);
}

bool WebAPIResponse::ProcessComputationIndicator() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessComputationIndicator");
  this->owner->SetHeaderOKNoContentLength("");
  global << "Processing get request indicator." << logger::endL;
  JSData result = this->owner->ProcessComputationIndicatorJSData();
  return global.theProgress.WriteResponse(result);
}

bool WebAPIResponse::ProcessSignUP() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessSignUP");
  this->owner->SetHeaderOKNoContentLength("");
  return this->owner->WriteToBodyJSON(this->owner->GetSignUpRequestResult());
}

bool WebAPIResponse::ProcessChangePassword(const std::string& reasonForNoAuthentication) {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessChangePassword");
  (void) reasonForNoAuthentication;
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  if (!global.flagDatabaseCompiled) {
    result[WebAPI::result::error] = "Database not compiled";
    this->owner->WriteToBodyJSON(result);
    return true;
  }
  UserCalculatorData& theUser = global.userDefault;
  theUser.enteredAuthenticationToken = "";
  if (!global.flagUsingSSLinCurrentConnection) {
    result[WebAPI::result::error] = "Please use secure connection.";
    return this->owner->WriteToBodyJSON(result);
  }
  if (!global.flagLoggedIn) {
    result[WebAPI::result::error] = "Please enter (old) password. " + reasonForNoAuthentication;
    return this->owner->WriteToBodyJSON(result);
  }
  std::string newPassword = HtmlRoutines::ConvertStringToURLString(
    HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("newPassword"), true),
    false
  );
  //<-Passwords are ONE-LAYER url-encoded
  //<-INCOMING pluses in passwords MUST be decoded as spaces, this is how form.submit() works!
  //<-Incoming pluses must be re-coded as spaces (%20).

  std::string reenteredPassword = HtmlRoutines::ConvertStringToURLString(
    HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("reenteredPassword"), true),
    false
  );
  //<-Passwords are ONE-LAYER url-encoded
  //<-INCOMING pluses in passwords MUST be decoded as spaces, this is how form.submit() works!
  //<-Incoming pluses must be re-coded as spaces (%20).

  std::string newEmail = HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("email"), false);
  if (newEmail != "") {
    JSData notUsed;
    QueryExact queryEmailTaken(DatabaseStrings::tableUsers, DatabaseStrings::labelEmail, newEmail);
    if (Database::get().FindOne(
      queryEmailTaken, notUsed, nullptr
    )) {
      result[WebAPI::result::error] = "It appears the email is already taken. ";
      return this->owner->WriteToBodyJSON(result);
    }
  }
  if (newPassword == "" && reenteredPassword == "" && newEmail != "") {
    result = this->owner->SetEmail(newEmail);
    return this->owner->WriteToBodyJSON(result);
  }
  if (newPassword != reenteredPassword) {
    result[WebAPI::result::error] = "Passwords don't match. ";
    return this->owner->WriteToBodyJSON(result);
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
    return this->owner->WriteToBodyJSON(result);
  }
  JSData setQuery;
  QueryExact findQuery(DatabaseStrings::tableUsers, DatabaseStrings::labelUsername, theUser.username);
  setQuery[DatabaseStrings::labelActivationToken] = "activated";
  if (!Database::get().UpdateOne(
    findQuery, setQuery, &commentsOnFailure
  )) {
    result[WebAPI::result::error] = "Failed to set activationToken: " +commentsOnFailure.str();
    return this->owner->WriteToBodyJSON(result);
  }
  std::stringstream out;
  out << "<b style = \"color:green\">Password change successful. </b>";
  if (global.GetWebInput("doReload") != "false") {
    out
    << "<meta http-equiv=\"refresh\" content =\"0; url ='"
    << global.DisplayNameExecutable  << "?request=logout"
    << "&username="
    << HtmlRoutines::ConvertStringToURLString(global.userDefault.username, false)
    << "&activationToken = &authenticationToken = &"
    << "'\" />";
  }
  result[WebAPI::result::comments] = out.str();
  return this->owner->WriteToBodyJSON(result);
}

bool WebAPIResponse::ProcessCompute() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessCompute");
  this->owner->SetHeaderOKNoContentLength("");
  bool monitoringDesired = (global.GetWebInput(WebAPI::request::monitoring) != "false");
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
  //  the initialization below moved to the start of the web server!
  //  theParser.init();
  ////////////////////////////////////////////////
  if (monitoringDesired) {
    global.theProgress.AllowMonitoring();
  }
  theCalculator.Evaluate(theCalculator.inputString);
  global.theProgress.BanMonitoring();
  JSData result;
  result = theCalculator.ToJSONOutputAndSpecials();
  result[WebAPI::result::commentsGlobal] = global.Comments.getCurrentReset();
  this->owner->WriteToBodyJSON(result);
  global.flagComputationCompletE = true;
  return true;
}

bool WebAPIResponse::ProcessActivateAccount() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessActivateAccount");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  bool notUsed = false;
  result[WebAPI::result::resultHtml] = this->owner->GetChangePasswordPagePartOne(notUsed);
  return this->owner->WriteToBodyJSON(result);
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
  return this->owner->WriteToBodyJSON(WebAPIResponse::GetSelectCourseJSON());
}

bool WebAPIResponse::ProcessTopicListJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessTopicListJSON");
  this->owner->SetHeaderOKNoContentLength("");
  JSData resultJSON = WebAPIResponse::GetTopicTableJSON();
  return this->owner->WriteToBodyJSONAppendComments(resultJSON);
}

bool WebAPIResponse::ProcessCalculatorOnePageJS(bool appendBuildHash) {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessCalculatorOnePageJS");
  if (appendBuildHash) {
    this->owner->SetHeaderOKNoContentLength(WebAPI::HeaderCacheControl, "text/javascript");
  } else {
    this->owner->SetHeaderOKNoContentLength("", "text/javascript");
  }
  return this->owner->WriteToBody(WebAPIResponse::GetOnePageJS(appendBuildHash));
}

bool WebAPIResponse::ProcessApp(bool appendBuildHash) {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessApp");
  this->owner->SetHeaderOKNoContentLength("", "text/html");
  if (global.server().RestartIsNeeded()) {
    return true;
  }
  return this->owner->WriteToBody(WebAPIResponse::GetApp(appendBuildHash));
}

bool WebAPIResponse::ProcessLoginUserInfo(const std::string& comments) {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessLoginUserInfo");
  this->owner->SetHeaderOKNoContentLength("");
  if (global.server().RestartIsNeeded()) {
    return true;
  }
  return this->owner->WriteToBodyJSON(WebAPIResponse::GetJSONUserInfo(comments));
}

bool WebAPIResponse::ProcessTemplateJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessTemplateJSON");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::GetJSONFromTemplate();
  return this->owner->WriteToBodyJSON(result);
}

bool WebAPIResponse::ProcessExamPageJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessExamPageJSON");
  this->owner->SetHeaderOKNoContentLength("");
  return this->owner->WriteToBodyJSON(WebAPIResponse::GetExamPageJSON());
}

bool WebAPIResponse::ProcessSetProblemWeight() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessSetProblemWeight");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::SetProblemWeight();
  return this->owner->WriteToBodyJSON(result);
}

bool WebAPIResponse::ProcessSetProblemDeadline() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessSetProblemDatabaseInfo");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::SetProblemDeadline();
  return this->owner->WriteToBodyJSON(result);
}

bool WebAPIResponse::ProcessAddUserEmails() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessAddUserEmails");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = this->owner->GetAddUserEmails();
  return this->owner->WriteToBodyJSON(result);
}

bool WebAPIResponse::ProcessModifyPage() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessModifyPage");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::ModifyProblemReport();
  return this->owner->WriteToBodyJSON(result);
}

bool WebAPIResponse::ProcessAssignTeacherToSection() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessAssignTeacherToSection");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::AddTeachersSections();
  return this->owner->WriteToBodyJSON(result);
}

bool WebAPIResponse::ProcessScores() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessScores");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::GetScoresPage();
  return this->owner->WriteToBodyJSON(result);
}

bool WebAPIResponse::ProcessScoresInCoursePage() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessScoresInCoursePage");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = WebAPIResponse::GetScoresInCoursePage();
  return this->owner->WriteToBodyJSON(result);
}

bool WebAPIResponse::ProcessAccountsJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessAccountsJSON");
  this->owner->SetHeaderOKNoContentLength("");
  return this->owner->WriteToBodyJSON(WebAPIResponse::GetAccountsPageJSON(this->owner->hostWithPort));
}

bool WebAPIResponse::ProcessDatabaseJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessDatabaseJSON");
  this->owner->SetHeaderOKNoContentLength("");
  return this->owner->WriteToBodyJSON(this->owner->GetDatabaseJSON());
}

bool WebAPIResponse::ProcessDatabaseDeleteEntry() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessDatabaseDeleteEntry");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::resultHtml] = this->owner->GetDatabaseDeleteOneItem();
  return this->owner->WriteToBodyJSON(this->owner->GetDatabaseJSON());
}

bool WebAPIResponse::ProcessDatabaseModifyEntry() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessDatabaseModifyEntry");
  this->owner->SetHeaderOKNoContentLength("");
  JSData result;
  result[WebAPI::result::error] = "WebAPIResponse::ProcessDatabaseModifyEntry not implemented yet";
  return this->owner->WriteToBodyJSON(result);
}

bool WebAPIResponse::ProcessEditPageJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessEditPageJSON");
  this->owner->SetHeaderOKNoContentLength("");
  return this->owner->WriteToBodyJSON(WebAPIResponse::GetEditPageJSON());
}

bool WebAPIResponse::ProcessSlidesOrHomeworkFromSource() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessSlidesOrHomeworkFromSource");
  this->owner->SetHeaderOKNoContentLength("");
  LaTeXCrawler theCrawler;
  for (int i = 0; i < global.webArguments.size(); i ++) {
    std::string theKey = HtmlRoutines::ConvertURLStringToNormal(global.webArguments.theKeys[i], false);
    if (theKey != WebAPI::problem::fileName && StringRoutines::StringBeginsWith(theKey, "file")) {
      theCrawler.slideFileNamesVirtualWithPatH.AddOnTop(StringRoutines::StringTrimWhiteSpace(
        HtmlRoutines::ConvertURLStringToNormal(global.webArguments.theValues[i], false)
      ));
    }
    if (StringRoutines::StringBeginsWith(theKey, "isSolutionFile")) {
      theCrawler.slideFilesExtraFlags.AddOnTop(StringRoutines::StringTrimWhiteSpace(
        HtmlRoutines::ConvertURLStringToNormal(global.webArguments.theValues[i], false)
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
  HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("title"), false);
  std::stringstream comments;
  if (global.GetWebInput("layout") == "printable") {
    theCrawler.flagProjectorMode = false;
  }
  if (global.GetWebInput("answerKey") == "true") {
    theCrawler.flagAnswerKey = true;
  }
  if (
    global.userCalculatorRequestType == "homeworkFromSource" ||
    global.userCalculatorRequestType == "homeworkSource"
  ) {
    theCrawler.flagHomeworkRatherThanSlides = true;
  }
  if (!theCrawler.BuildOrFetchFromCachePDF(&comments, &comments)) {
    JSData result;
    WebAPIResponse::GetJSDataUserInfo(result, comments.str());
    this->owner->flagDoAddContentLength = true;
    this->owner->WriteToBodyJSON(result);
    return true;
  }
  this->owner->SetHeader("HTTP/1.0 200 OK", "Content-Type: application/pdf; Access-Control-Allow-Origin: *");
  this->owner->flagDoAddContentLength = true;
  return this->owner->WriteToBody(theCrawler.targetPDFbinaryContent);
}

bool WebAPIResponse::ProcessSlidesSource() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessSlidesSource");
  this->owner->SetHeaderOKNoContentLength("");
  LaTeXCrawler theCrawler;
  for (int i = 0; i < global.webArguments.size(); i ++) {
    std::string theKey = HtmlRoutines::ConvertURLStringToNormal(global.webArguments.theKeys[i], false);
    if (
      theKey != WebAPI::problem::fileName && StringRoutines::StringBeginsWith(theKey, "file")
    ) {
      theCrawler.slideFileNamesVirtualWithPatH.AddOnTop(StringRoutines::StringTrimWhiteSpace(
        HtmlRoutines::ConvertURLStringToNormal(global.webArguments.theValues[i], false)
      ));
    }
  }
  if (global.userCalculatorRequestType == "homeworkSource") {
    theCrawler.flagHomeworkRatherThanSlides = true;
  } else {
    theCrawler.flagHomeworkRatherThanSlides = false;
  }
  theCrawler.desiredPresentationTitle =
  HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("title"), false);
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
    return this->owner->WriteToBodyJSON(WebAPIResponse::GetJSONUserInfo(comments.str()));
  }
  this->owner->SetHeader("HTTP/1.0 200 OK", "Content-Type: application/x-latex; Access-Control-Allow-Origin: *");
  this->owner->flagDoAddContentLength = true;
  return this->owner->WriteToBody(theCrawler.targetLaTeX);
}

bool WebAPIResponse::ProcessClonePage() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessClonePage");
  this->owner->SetHeaderOKNoContentLength("");
  return this->owner->WriteToBodyJSON(this->owner->GetClonePageResult());
}

bool WebAPIResponse::ProcessProblemGiveUp() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessProblemGiveUp");
  this->owner->SetHeaderOKNoContentLength("");
  return this->owner->WriteToBodyJSON(WebAPIResponse::GetAnswerOnGiveUp());
}

bool WebAPIResponse::ProcessProblemSolution() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessProblemSolution");
  this->owner->SetHeaderOKNoContentLength("");
  return this->owner->WriteToBodyJSON(WebAPIResponse::GetProblemSolutionJSON());
}

bool WebAPIResponse::ProcessSubmitAnswersPreview() {
  MacroRegisterFunctionWithName("WebAPIResponse::ProcessSubmitAnswersPreview");
  this->owner->SetHeaderOKNoContentLength("");
  return this->owner->WriteToBodyJSON(WebAPIResponse::submitAnswersPreviewJSON());
}

bool WebAPIResponse::ProcessSubmitAnswers() {
  MacroRegisterFunctionWithName("WebWorker::ProcessSubmitAnswers");
  this->owner->SetHeaderOKNoContentLength("");
  return this->owner->WriteToBodyJSON(WebAPIResponse::SubmitAnswersJSON());
}
