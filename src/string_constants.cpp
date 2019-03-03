#include "string_constants.h"

static ProjectInformationInstance ProjectInfoVpfJSON(__FILE__, "String constants");

std::string WebAPI::appNoCache = "appNoCache";
std::string WebAPI::app = "app";
std::string WebAPI::request::calculatorHTML = "/calculator-html";
std::string WebAPI::request::onePageJS = "/calculator-html/javascript_all_in_one.js";
std::string WebAPI::request::onePageJSWithHash = "/calculator-html/javascript_all_in_one.js";
std::string WebAPI::request::calculatorPage = "calculator";
std::string WebAPI::request::setProblemData = "setProblemData";
std::string WebAPI::request::changePassword = "changePassword";
std::string WebAPI::request::activateAccount = "activateAccount";
std::string WebAPI::request::activateAccountJSON = "activateAccountJSON";
std::string WebAPI::request::serverStatusJSON = "serverStatusJSON";
std::string WebAPI::request::selectCourseJSON = "selectCourseJSON";
std::string WebAPI::HeaderCacheControl = "Cache-Control: max-age=129600000, public";

std::string WebAPI::problemContent = "problemContent";
std::string WebAPI::problemFileName = "fileName";
std::string WebAPI::problemId = "id";
std::string WebAPI::commentsProblem = "commentsProblem";
std::string WebAPI::commentsServer = "commentsServer";

std::string WebAPI::request::userInfoJSON = "userInfoJSON";
std::string WebAPI::request::examplesJSON = "calculatorExamplesJSON";
std::string WebAPI::request::editPage = "editPageJSON";
std::string WebAPI::databaseParameters::entryPoint = "database";
std::string WebAPI::databaseParameters::labels = "databaseLabels";
std::string WebAPI::databaseParameters::operation = "databaseOperation";
std::string WebAPI::databaseParameters::fetch = "databaseFetch";

std::string WebAPI::problemSingleDeadline = "deadline";
std::string WebAPI::result::resultHtml = "resultHtml";
std::string WebAPI::result::syntaxErrors = "syntaxErrors";
std::string WebAPI::result::resultLabel = "result";
std::string WebAPI::result::badInput = "badInput";
std::string WebAPI::result::comments = "comments";
std::string WebAPI::result::timeOut = "timeOut";
std::string WebAPI::result::timeOutComments = "timeOutComments";
std::string WebAPI::result::workerId = "workerId";
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

std::string Configuration::processMonitoringAllowedByDefault = "processMonitoringAllowedByDefault";
std::string Configuration::replyAfterComputationMillisecondsDefault = "replyAfterComputationMillisecondsDefault";
std::string Configuration::replyAfterComputationMilliseconds = "replyAfterComputationMilliseconds";
