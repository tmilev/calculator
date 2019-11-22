#include "string_constants.h"

static ProjectInformationInstance ProjectInfoVpfStringConstants(__FILE__, "String constants");

std::string WebAPI::appNoCache = "appNoCache";
std::string WebAPI::app = "app";
std::string WebAPI::HeaderCacheControl = "Cache-Control: max-age=129600000, public";

std::string WebAPI::commentsServer = "commentsServer";

std::string WebAPI::request::userInfoJSON = "userInfoJSON";
std::string WebAPI::request::examplesJSON = "calculatorExamplesJSON";
std::string WebAPI::request::editPage = "editPageJSON";
std::string WebAPI::databaseParameters::entryPoint = "database";
std::string WebAPI::databaseParameters::labels = "databaseLabels";
std::string WebAPI::databaseParameters::operation = "databaseOperation";
std::string WebAPI::databaseParameters::fetch = "databaseFetch";

std::string WebAPI::result::error = "error";
std::string WebAPI::result::status = "status";
std::string WebAPI::result::running = "running";
std::string WebAPI::result::resultStringified = "resultStringified";
std::string WebAPI::result::resultHtml = "resultHtml";
std::string WebAPI::result::syntaxErrors = "syntaxErrors";
std::string WebAPI::result::resultLabel = "result";
std::string WebAPI::result::badInput = "badInput";
std::string WebAPI::result::comments = "comments";
std::string WebAPI::result::timeOut = "timeOut";
std::string WebAPI::result::timeOutComments = "timeOutComments";
std::string WebAPI::result::workerId = "workerId";
std::string WebAPI::result::workerIndex = "workerIndex";
std::string WebAPI::result::crashReport = "crashReport";
std::string WebAPI::result::millisecondsComputation = "timeComputation";
std::string WebAPI::result::loginDisabledEveryoneIsAdmin = "loginDisabledEveryoneIsAdmin";

std::string WebAPI::problem::fileContent = "fileContent";
std::string WebAPI::problem::title = "title";
std::string WebAPI::problem::fileName = "fileName";
std::string WebAPI::problem::fileNameTarget = "fileNameTarget";
std::string WebAPI::problem::idProblem = "id";
std::string WebAPI::problem::deadlineSingle = "deadline";
std::string WebAPI::problem::failedToLoadProblem = "Failed to load problem. ";
std::string WebAPI::problem::problemLabel = "problemLabel";
std::string WebAPI::problem::content = "content";
std::string WebAPI::problem::commentsProblem = "commentsProblem";


std::string WebAPI::request::calculatorHTML = "/calculator-html";
std::string WebAPI::request::onePageJS = "/calculator-html/javascript_all_in_one.js";
std::string WebAPI::request::onePageJSWithHash = "/calculator-html/javascript_all_in_one.js";
std::string WebAPI::request::calculatorPage = "calculator";
std::string WebAPI::request::setProblemData = "setProblemData";
std::string WebAPI::request::changePassword = "changePassword";
std::string WebAPI::request::activateAccountJSON = "activateAccountJSON";
std::string WebAPI::request::serverStatusJSON = "serverStatusJSON";
std::string WebAPI::request::selectCourseJSON = "selectCourseJSON";
std::string WebAPI::request::compute = "compute";
std::string WebAPI::request::monitoring = "monitoring";
std::string WebAPI::request::calculatorInput = "calculatorInput";
std::string WebAPI::request::forgotLogin = "forgotLogin";
std::string WebAPI::request::login = "login";
std::string WebAPI::request::logout = "logout";
std::string WebAPI::request::signUp = "signUp";

std::string WebAPI::request::submitExercise = "submitExercise";
std::string WebAPI::request::submitExerciseNoLogin = "submitExerciseNoLogin";
std::string WebAPI::request::submitAnswers = "submitAnswers";
std::string WebAPI::request::submitExercisePreviewNoLogin = "submitExercisePreviewNoLogin";
std::string WebAPI::request::indicator = "indicator";
std::string WebAPI::request::pause = "pause";
std::string WebAPI::request::unpause = "unpause";
std::string WebAPI::request::modifyPage = "modifyPage";
std::string WebAPI::request::clonePage = "clonePageJSON";
std::string WebAPI::request::problemGiveUp = "problemGiveUp";
std::string WebAPI::request::problemGiveUpNoLogin = "problemGiveUpNoLogin";
std::string WebAPI::request::workerId = WebAPI::result::workerId;
std::string WebAPI::request::workerIndex = WebAPI::result::workerIndex;

std::string WebAPI::UserInfo::processMonitoring = "processMonitoring";

std::string DatabaseStrings::labelUserId = "id";
std::string DatabaseStrings::labelUsername = "username";
std::string DatabaseStrings::labelPassword = "password";
std::string DatabaseStrings::labelUserRole = "userRole";
std::string DatabaseStrings::labelProblemDatA = "problemData";
std::string DatabaseStrings::labelProblemDataJSON = "problemDataJSON";
std::string DatabaseStrings::labelAuthenticationToken = "authenticationToken";
std::string DatabaseStrings::labelActivationToken = "activationToken";
std::string DatabaseStrings::labelTimeOfActivationTokenCreation = "activationTokenCreationTime";
std::string DatabaseStrings::labelTimeOfAuthenticationTokenCreation = "authenticationCreationTime";
std::string DatabaseStrings::labelEmail = "email";

std::string DatabaseStrings::tableUsers = "users";
std::string DatabaseStrings::theDatabaseUser = "ace";
std::string DatabaseStrings::theDatabaseName = "aceDB";
std::string DatabaseStrings::theDatabaseNameMongo = "calculator";
std::string DatabaseStrings::tableDeadlines = "deadlines";
std::string DatabaseStrings::labelDeadlines = "deadlines";

std::string DatabaseStrings::labelSection = "studentSection";
std::string DatabaseStrings::labelCurrentCourses = "currentCourses";
std::string DatabaseStrings::labelDeadlinesSchema = "deadlineSchema";
std::string DatabaseStrings::labelInstructor = "instructor";
std::string DatabaseStrings::labelSemester = "semester";

std::string DatabaseStrings::tableProblemWeights = "problemWeights";
std::string DatabaseStrings::labelProblemWeightsSchema = "problemWeightsSchema";
std::string DatabaseStrings::labelProblemWeights = "weight";

std::string DatabaseStrings::tableProblemInformation = "problemInformation";
std::string DatabaseStrings::labelProblemTotalQuestions = "problemsTotalQuestions";
std::string DatabaseStrings::labelProblemName = "problemName";

std::string DatabaseStrings::labelSectionsTaught = "sectionsTaught";

std::string DatabaseStrings::tableEmailInfo = "emailInfo";
std::string DatabaseStrings::labelLastActivationEmailTime = "lastActivationEmailTime";
std::string DatabaseStrings::labelNumActivationEmails = "numActivationEmails";
std::string DatabaseStrings::labelUsernameAssociatedWithToken = "usernameAssociatedWithToken";

std::string DatabaseStrings::tableDeleted = "deleted";

std::string DatabaseStrings::anyFielD = "${any}";
std::string DatabaseStrings::objectSelectoR = "${number}";
std::string DatabaseStrings::objectSelectorMongo = "$oid";
std::string DatabaseStrings::labelFields = "fields";
std::string DatabaseStrings::labelTable = "table";
std::string DatabaseStrings::labelIdMongo = "_id";
std::string DatabaseStrings::labelCurrentPage = "currentPage";
std::string DatabaseStrings::labelPageSignUp = "signUp";
std::string DatabaseStrings::labelCalculatorRequest = "calculatorRequest";
std::string DatabaseStrings::labelCalculatorInput = "calculatorInput";
std::string DatabaseStrings::labelPageAccount = "account";
std::string DatabaseStrings::labelPageActivateAccount = "activateAccount";

std::string Configuration::processMonitoringBanned = "processMonitoringBanned";
std::string Configuration::millisecondsReplyAfterComputationDefault = "millisecondsReplyAfterComputationDefault";
std::string Configuration::monitorPingTime = "monitorPingTime";
std::string Configuration::millisecondsReplyAfterComputation = "millisecondsReplyAfterComputation";
std::string Configuration::builtInTLSAvailable = "builtInTLSAvailable";
std::string Configuration::serverAutoMonitor = "serverAutoMonitor";
std::string Configuration::disableDatabaseLogEveryoneAsAdmin = "disableDatabaseLogEveryoneAsAdmin";
std::string Configuration::serverDetailedLog = "serverDetailedLog";
std::string Configuration::gitRepository = "gitRepository";
std::string Configuration::autoUnitTest = "autoUnitTest";
std::string Configuration::portHTTP = "portHTTP";
std::string Configuration::portHTTPSOpenSSL = "portHTTPSOpenSSL";
std::string Configuration::portHTTPSBuiltIn = "portHTTPSBuiltIn";
