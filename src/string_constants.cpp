#include "string_constants.h"

std::string WebAPI::appNoCache = "appNoCache";
std::string WebAPI::app = "app";
std::string WebAPI::compareExpressionsPage = "compareExpressionsPage";
std::string WebAPI::compareExpressionsPageNoCache =
"compareExpressionsPageNoCache";
std::string WebAPI::headerCacheControl =
"Cache-Control: max-age=129600000, public";
std::string WebAPI::commentsServer = "commentsServer";
std::string WebAPI::userDefaultAdmin = "default";
std::string WebAPI::DatabaseParameters::labels = "databaseLabels";
std::string WebAPI::DatabaseParameters::operation = "databaseOperation";
std::string WebAPI::DatabaseParameters::fetch = "databaseFetch";
std::string WebAPI::Result::error = "error";
std::string WebAPI::Result::status = "status";
std::string WebAPI::Result::running = "running";
std::string WebAPI::Result::resultHtml = "resultHtml";
std::string WebAPI::Result::scripts = "scripts";
std::string WebAPI::Result::outputString = "outputString";
std::string WebAPI::Result::syntaxErrors = "syntaxErrors";
std::string WebAPI::Result::syntaxErrorsExtra = "syntaxErrorsExtra";
std::string WebAPI::Result::resultLabel = "result";
std::string WebAPI::Result::badInput = "badInput";
std::string WebAPI::Result::comments = "comments";
std::string WebAPI::Result::performance = "performance";
std::string WebAPI::Result::performancePerHandler = "performancePerHandler";
std::string WebAPI::Result::performanceTurnOffSlowRules =
"performanceTurnOffSlowRules";
std::string WebAPI::Result::performanceTurnOffVerySlowRules =
"performanceTurnOffVerySlowRules";
std::string WebAPI::Result::computationTime = "computationTime";
std::string WebAPI::Result::parsingLog = "parsingLog";
std::string WebAPI::Result::commentsGlobal = "commentsGlobal";
std::string WebAPI::Result::commentsDebug = "commentsDebug";
std::string WebAPI::Result::timeOut = "timeOut";
std::string WebAPI::Result::timeOutComments = "timeOutComments";
std::string WebAPI::Result::workerId = "workerId";
std::string WebAPI::Result::workerIndex = "workerIndex";
std::string WebAPI::Result::crashReport = "crashReport";
std::string WebAPI::Result::millisecondsComputation = "timeComputation";
std::string WebAPI::Result::loginDisabledEveryoneIsAdmin =
"loginDisabledEveryoneIsAdmin";
std::string WebAPI::Result::debugLogin = "debugLogin";
std::string WebAPI::Result::database = "database";
std::string WebAPI::Result::httpsSupport = "httpsSupport";
std::string WebAPI::Result::solution = "solution";
std::string WebAPI::Result::SolutionData::steps = "solutionSteps";
std::string WebAPI::Result::SolutionData::input = "input";
std::string WebAPI::Result::SolutionData::finalExpression = "finalExpression";
std::string WebAPI::Result::SolutionData::comments = "comments";
std::string WebAPI::Result::SolutionData::transformation = "transformation";
std::string WebAPI::Result::SolutionData::annotations = "annotations";
std::string WebAPI::Result::SolutionData::stepType = "stepType";
std::string WebAPI::Result::scriptType = "scriptType";
std::string WebAPI::Result::ComparisonData::areEqual = "areEqual";
std::string WebAPI::Result::ComparisonData::areEqualAsAnswers =
"areEqualAsAnswers";
std::string WebAPI::Result::ComparisonData::given = "given";
std::string WebAPI::Result::ComparisonData::desired = "desired";
std::string WebAPI::Result::ComparisonData::givenRaw = "givenRaw";
std::string WebAPI::Result::ComparisonData::desiredRaw = "desiredRaw";
std::string WebAPI::Result::ComparisonData::errorEvaluation =
"errorEvaluation";
std::string WebAPI::Result::ComparisonData::errorInAnswer = "errorInAnswer";
std::string WebAPI::Problem::fileContent = "fileContent";
std::string WebAPI::Problem::title = "title";
std::string WebAPI::Problem::fileName = "fileName";
std::string WebAPI::Problem::fileNameTarget = "fileNameTarget";
std::string WebAPI::Problem::courseHome = "courseHome";
std::string WebAPI::Problem::courseTitle = "title";
std::string WebAPI::Problem::topicList = "topicList";
std::string WebAPI::Problem::idProblem = "id";
std::string WebAPI::Problem::deadlineSingle = "deadline";
std::string WebAPI::Problem::failedToLoadProblem = "Failed to load problem. ";
std::string WebAPI::Problem::problemLabel = "problemLabel";
std::string WebAPI::Problem::content = "content";
std::string WebAPI::Problem::commentsProblem = "commentsProblem";
std::string WebAPI::Problem::calculatorAnswerPrefix = "calculatorAnswer";
std::string WebAPI::Problem::answerGenerationSuccess = "generationSuccess";
std::string WebAPI::Problem::randomSeed = "randomSeed";
std::string WebAPI::Problem::forReal = "forReal";
std::string WebAPI::Problem::answerWasHardCoded = "answerWasHardCoded";
std::string WebAPI::Problem::answerId = "answerId";
std::string WebAPI::Problem::answerValue = "answerValue";
std::string WebAPI::Problem::answerCheck = "answerCheck";
std::string WebAPI::Request::workerId = WebAPI::Result::workerId;
std::string WebAPI::Request::database = "database";
std::string WebAPI::Request::userInfoJSON = "userInfoJSON";
std::string WebAPI::Request::examplesJSON = "calculatorExamplesJSON";
std::string WebAPI::Request::editPage = "editPageJSON";
std::string WebAPI::Request::calculatorHTML = "/calculator_html";
std::string WebAPI::Request::onePageJS =
"/calculator_html/javascript_all_in_one.js";
std::string WebAPI::Request::onePageJSWithHash =
"/calculator_html/javascript_all_in_one.js";
std::string WebAPI::Request::calculatorWorkerJS =
"/calculator_html/web_assembly/calculator_web_assembly_all_in_one.js";
std::string WebAPI::Request::calculatorWorkerJSWithHash =
"/calculator_html/web_assembly/calculator_web_assembly_all_in_one.js";
std::string WebAPI::Request::calculatorCSS = "/calculator_html/calculator.css";
std::string WebAPI::Request::lieAlgebrasCSS =
"/calculator_html/style_lie_algebras.css";
std::string WebAPI::Request::requestAPI = "request";
std::string WebAPI::Request::setProblemWeight = "setProblemWeight";
std::string WebAPI::Request::setProblemDeadline = "setProblemDeadline";
std::string WebAPI::Request::changePassword = "changePassword";
std::string WebAPI::Request::changePasswordPage = "changePasswordPage";
std::string WebAPI::Request::activateAccountJSON = "activateAccountJSON";
std::string WebAPI::Request::debugFlag = "debugFlag";
std::string WebAPI::Request::serverStatusJSON = "serverStatusJSON";
std::string WebAPI::Request::selectCourseJSON = "selectCourseJSON";
std::string WebAPI::Request::compute = "compute";
std::string WebAPI::Request::monitoring = "monitoring";
std::string WebAPI::Request::calculatorInput = "calculatorInput";
std::string WebAPI::Request::forgotLogin = "forgotLogin";
std::string WebAPI::Request::login = "login";
std::string WebAPI::Request::logout = "logout";
std::string WebAPI::Request::signUp = "signUp";
std::string WebAPI::Request::submitExercise = "submitExercise";
std::string WebAPI::Request::submitExerciseNoLogin = "submitExerciseNoLogin";
std::string WebAPI::Request::submitAnswers = "submitAnswers";
std::string WebAPI::Request::submitExercisePreviewNoLogin =
"submitExercisePreviewNoLogin";
std::string WebAPI::Request::indicator = "indicator";
std::string WebAPI::Request::pause = "pause";
std::string WebAPI::Request::unpause = "unpause";
std::string WebAPI::Request::modifyPage = "modifyPage";
std::string WebAPI::Request::clonePage = "clonePageJSON";
std::string WebAPI::Request::problemGiveUp = "problemGiveUp";
std::string WebAPI::Request::exerciseNoLogin = "exerciseNoLogin";
std::string WebAPI::Request::exerciseJSON = "exerciseJSON";
std::string WebAPI::Request::problemGiveUpNoLogin = "problemGiveUpNoLogin";
std::string WebAPI::Request::templateNoLogin = "templateNoLogin";
std::string WebAPI::Request::templateJSONNoLogin = "templateJSONNoLogin";
std::string WebAPI::Request::topicListJSONNoLogin = "topicListJSONNoLogin";
std::string WebAPI::Request::problemSolutionNoLogin = "problemSolutionNoLogin";
std::string WebAPI::Request::homeworkFromSource = "homeworkFromSource";
std::string WebAPI::Request::homeworkSource = "homeworkSource";
std::string WebAPI::Request::addUsers = "addUsers";
std::string WebAPI::Request::setTeacher = "setTeacher";
std::string WebAPI::Request::teachersAndSections = "teachersAndSections";
std::string WebAPI::Request::teachers = "teachers";
std::string WebAPI::Request::sections = "sections";
std::string WebAPI::Request::addEmails = "addEmails";
std::string WebAPI::Request::userList = "userList";
std::string WebAPI::Request::passwordList = "passwordList";
std::string WebAPI::Request::solveRequest = "solveJSON";
std::string WebAPI::Request::compareExpressions = "compareExpressions";
std::string WebAPI::Request::compareExpressionsInternal =
"compareExpressionsInternal";
std::string WebAPI::Request::compareExpressionsGiven =
"compareExpressionsGiven";
std::string WebAPI::Request::compareExpressionsDesired =
"compareExpressionsDesired";
std::string WebAPI::Request::submitAnswerHardcoded = "submitAnswerHardcoded";
std::string WebAPI::Request::Slides::source = "slidesSource";
std::string WebAPI::Request::Slides::pdfFromSource = "slidesFromSource";
std::string WebAPI::Request::Slides::title = "slideTitle";
std::string WebAPI::Request::Slides::files = "files";
std::string WebAPI::Request::Slides::content = "content";
std::string WebAPI::Request::Slides::queryHomework = "queryHomework";
std::string WebAPI::Request::Slides::isSolution = "isSolution";
std::string WebAPI::Request::Slides::slideFilename = "slideFilename";
std::string WebAPI::Frontend::currentPage = "currentPage";
std::string WebAPI::Frontend::calculatorPage = "calculator";
std::string WebAPI::Frontend::problemPage = "problemPage";
std::string WebAPI::Frontend::problemFileName = "problemFileName";
std::string WebAPI::Frontend::exercise = "exerciseJSON";
std::string WebAPI::Frontend::exerciseType = "exerciseType";
std::string WebAPI::Frontend::scoredQuiz = "scoredQuizJSON";
std::string WebAPI::Frontend::calculatorExamples = "calculatorExamples";
std::string WebAPI::Frontend::calculatorHandlerDocumentation =
"calculatorHandlerDocumentation";
std::string WebAPI::UserInfo::processMonitoring = "processMonitoring";
std::string WebAPI::Calculator::testFileNameVirtual =
"test/automated_test_known_inputs_outputs.json";
std::string DatabaseStrings::labelUserId = "id";
std::string DatabaseStrings::labelUsername = "username";
std::string DatabaseStrings::labelPassword = "password";
std::string DatabaseStrings::labelUserRole = "userRole";
std::string DatabaseStrings::labelProblemDataJSON = "problemDataJSON";
std::string DatabaseStrings::labelProblemFileName = "problemFileName";
std::string DatabaseStrings::labelAuthenticationToken = "authenticationToken";
std::string DatabaseStrings::labelActivationToken = "activationToken";
std::string DatabaseStrings::labelTimeOfActivationTokenCreation =
"activationTokenCreationTime";
std::string DatabaseStrings::labelTimeOfAuthenticationTokenCreation =
"authenticationCreationTime";
std::string DatabaseStrings::labelEmail = "email";
std::string DatabaseStrings::tableUsers = "users";
std::string DatabaseStrings::databaseName = "calculator";
std::string DatabaseStrings::tableDeadlines = "deadlines";
std::string DatabaseStrings::labelDeadlines = "deadlines";
std::string DatabaseStrings::labelSection = "studentSection";
std::string DatabaseStrings::labelCurrentCourses = "currentCourses";
std::string DatabaseStrings::labelDeadlinesSchema = "deadlineSchema";
std::string DatabaseStrings::labelInstructor = "instructor";
std::string DatabaseStrings::labelSemester = "semester";
std::string DatabaseStrings::tableProblemWeights = "problemWeights";
std::string DatabaseStrings::labelProblemWeightsSchema =
"problemWeightsSchema";
std::string DatabaseStrings::labelProblemWeight = "weight";
std::string DatabaseStrings::tableProblemInformation = "problemInformation";
std::string DatabaseStrings::labelProblemTotalQuestions =
"problemsTotalQuestions";
std::string DatabaseStrings::labelSectionsTaught = "sectionsTaught";
std::string DatabaseStrings::tableEmailInfo = "emailInfo";
std::string DatabaseStrings::labelLastActivationEmailTime =
"lastActivationEmailTime";
std::string DatabaseStrings::labelNumActivationEmails = "numActivationEmails";
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
std::string DatabaseStrings::errorDatabaseDisabled =
"Database disabled explicitly by server configuration. ";
std::string Configuration::processMonitoringBanned = "processMonitoringBanned";
std::string Configuration::monitorPingTime = "monitorPingTime";
std::string Configuration::millisecondsReplyAfterComputation =
"millisecondsReplyAfterComputation";
std::string Configuration::builtInTLSAvailable = "builtInTLSAvailable";
std::string Configuration::serverAutoMonitor = "serverAutoMonitor";
std::string Configuration::disableDatabaseLogEveryoneAsAdmin =
"disableDatabaseLogEveryoneAsAdmin";
std::string Configuration::debugLogin = "debugLogin";
std::string Configuration::database = "database";
std::string Configuration::serverDetailedLog = "serverDetailedLog";
std::string Configuration::gitRepository = "gitRepository";
std::string Configuration::dontUseOpenSSL = "dontUseOpenSSL";
std::string Configuration::portHTTP = "portHTTP";
std::string Configuration::portHTTPSOpenSSL = "portHTTPSOpenSSL";
std::string Configuration::portHTTPSBuiltIn = "portHTTPSBuiltIn";
std::string Configuration::calculatorWasm = "calculator.wasm";
std::string Configuration::HTMLCommon = "html-common/";
std::string Configuration::HTMLCommonFonts = "html-common/fonts";
std::string Configuration::HTMLCommonFont = "html-common/font";
std::string Configuration::HTMLAceSrcMin = "html-common/ace/src-min/";
std::string Configuration::examples = "examples/";
std::string Configuration::calculatorHTML = "calculator_html/";
std::string Configuration::publicHTML = "public_html/";
std::string Configuration::certificatesPublic = "certificates-public/";
std::string Configuration::sourceCode = "src/";
std::string Configuration::testFolder = "test/";
std::string Configuration::outputFolder = "output/";
std::string Configuration::HTMLGeneral = "html/";
std::string Configuration::problemsFolder = "problems/";
std::string Configuration::courseTemplates = "coursetemplates/";
std::string Configuration::coursesAvailable = "coursesavailable/";
std::string Configuration::topicLists = "topiclists/";
std::string Configuration::laTeXMaterials = "LaTeX-materials/";
std::string Configuration::slidesVideo = "slides-video/";
std::string Configuration::freecalc = "freecalc/";
std::string Configuration::useMathTags = "useMathTags";
std::string Configuration::webAddress = "webAddress";
std::string Configuration::sendEmailFrom = "sendEmailFrom";
std::string Configuration::actAsWebServerForTheseHosts =
"actAsWebServerForTheseHosts";
std::string Configuration::ActAsWebServer::baseFolder = "baseFolder";
std::string Configuration::ActAsWebServer::portHTTP = "portHTTP";
std::string Configuration::ActAsWebServer::portHTTPS = "portHTTPS";
std::string Configuration::ActAsWebServer::landingPage = "landingPage";
std::string Configuration::ActAsWebServer::privateKeyFile = "privateKeyFile";
std::string Configuration::ActAsWebServer::certificateFile = "certificateFile";
