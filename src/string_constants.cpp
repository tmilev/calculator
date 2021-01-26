#include "string_constants.h"

std::string WebAPI::appNoCache = "appNoCache";
std::string WebAPI::app = "app";
std::string WebAPI::compareExpressionsPage = "compareExpressionsPage";
std::string WebAPI::compareExpressionsPageNoCache = "compareExpressionsPageNoCache";

std::string WebAPI::headerCacheControl = "Cache-Control: max-age=129600000, public";

std::string WebAPI::commentsServer = "commentsServer";
std::string WebAPI::userDefaultAdmin = "default";

std::string WebAPI::databaseParameters::labels     = "databaseLabels"   ;
std::string WebAPI::databaseParameters::operation  = "databaseOperation";
std::string WebAPI::databaseParameters::fetch      = "databaseFetch"    ;

std::string WebAPI::result::error                                   = "error"                       ;
std::string WebAPI::result::status                                  = "status"                      ;
std::string WebAPI::result::running                                 = "running"                     ;
std::string WebAPI::result::resultHtml                              = "resultHtml"                  ;
std::string WebAPI::result::outputString                            = "outputString"                ;
std::string WebAPI::result::syntaxErrors                            = "syntaxErrors"                ;
std::string WebAPI::result::syntaxErrorsExtra                       = "syntaxErrorsExtra"           ;
std::string WebAPI::result::resultLabel                             = "result"                      ;
std::string WebAPI::result::badInput                                = "badInput"                    ;
std::string WebAPI::result::comments                                = "comments"                    ;
std::string WebAPI::result::performance                             = "performance"                 ;
std::string WebAPI::result::computationTime                         = "computationTime"             ;
std::string WebAPI::result::parsingLog                              = "parsingLog"                  ;
std::string WebAPI::result::commentsGlobal                          = "commentsGlobal"              ;
std::string WebAPI::result::timeOut                                 = "timeOut"                     ;
std::string WebAPI::result::timeOutComments                         = "timeOutComments"             ;
std::string WebAPI::result::workerId                                = "workerId"                    ;
std::string WebAPI::result::workerIndex                             = "workerIndex"                 ;
std::string WebAPI::result::crashReport                             = "crashReport"                 ;
std::string WebAPI::result::millisecondsComputation                 = "timeComputation"             ;
std::string WebAPI::result::loginDisabledEveryoneIsAdmin            = "loginDisabledEveryoneIsAdmin";
std::string WebAPI::result::useFallbackDatabase                     = "useFallbackDatabase"         ;
std::string WebAPI::result::httpsSupport                            = "httpsSupport"                ;
std::string WebAPI::result::solution                                = "solution"                    ;
std::string WebAPI::result::SolutionData::steps                     = "solutionSteps"               ;
std::string WebAPI::result::SolutionData::input                     = "input"                       ;
std::string WebAPI::result::SolutionData::finalExpression           = "finalExpression"             ;
std::string WebAPI::result::comparison                              = "comparison"                  ;
std::string WebAPI::result::scriptType                              = "scriptType"                  ;
std::string WebAPI::result::ComparisonData::areEqual                = "areEqual"                    ;
std::string WebAPI::result::ComparisonData::areEqualAsAnswers       = "areEqualAsAnswers"           ;
std::string WebAPI::result::ComparisonData::given                   = "given"                       ;
std::string WebAPI::result::ComparisonData::desired                 = "desired"                     ;
std::string WebAPI::result::ComparisonData::givenRaw                = "givenRaw"                    ;
std::string WebAPI::result::ComparisonData::desiredRaw              = "desiredRaw"                  ;
std::string WebAPI::result::ComparisonData::errorEvaluation         = "errorEvaluation"             ;
std::string WebAPI::result::ComparisonData::errorInAnswer           = "errorInAnswer"               ;


std::string WebAPI::problem::fileContent             = "fileContent"             ;
std::string WebAPI::problem::title                   = "title"                   ;
std::string WebAPI::problem::fileName                = "fileName"                ;
std::string WebAPI::problem::fileNameTarget          = "fileNameTarget"          ;
std::string WebAPI::problem::courseHome              = "courseHome"              ;
std::string WebAPI::problem::courseTitle             = "title"                   ;
std::string WebAPI::problem::topicList               = "topicList"               ;
std::string WebAPI::problem::idProblem               = "id"                      ;
std::string WebAPI::problem::deadlineSingle          = "deadline"                ;
std::string WebAPI::problem::failedToLoadProblem     = "Failed to load problem. ";
std::string WebAPI::problem::problemLabel            = "problemLabel"            ;
std::string WebAPI::problem::content                 = "content"                 ;
std::string WebAPI::problem::commentsProblem         = "commentsProblem"         ;
std::string WebAPI::problem::calculatorAnswerPrefix  = "calculatorAnswer"        ;
std::string WebAPI::problem::answerGenerationSuccess = "generationSuccess"       ;
std::string WebAPI::problem::randomSeed              = "randomSeed"              ;
std::string WebAPI::problem::answerWasHardCoded      = "answerWasHardCoded"      ;

std::string WebAPI::request::workerId                     = WebAPI::result::workerId                   ;
std::string WebAPI::request::database                     = "database"                                 ;
std::string WebAPI::request::userInfoJSON                 = "userInfoJSON"                             ;
std::string WebAPI::request::examplesJSON                 = "calculatorExamplesJSON"                   ;
std::string WebAPI::request::editPage                     = "editPageJSON"                             ;
std::string WebAPI::request::calculatorHTML               = "/calculator-html"                         ;
std::string WebAPI::request::onePageJS                    = "/calculator-html/javascript_all_in_one.js";
std::string WebAPI::request::onePageJSWithHash            = "/calculator-html/javascript_all_in_one.js";
std::string WebAPI::request::calculatorCSS                = "/calculator-html/calculator.css";
std::string WebAPI::request::requestAPI                   = "request"                     ;
std::string WebAPI::request::setProblemWeight             = "setProblemWeight"            ;
std::string WebAPI::request::setProblemDeadline           = "setProblemDeadline"          ;
std::string WebAPI::request::changePassword               = "changePassword"              ;
std::string WebAPI::request::activateAccountJSON          = "activateAccountJSON"         ;
std::string WebAPI::request::debugFlag                    = "debugFlag"                   ;
std::string WebAPI::request::serverStatusJSON             = "serverStatusJSON"            ;
std::string WebAPI::request::selectCourseJSON             = "selectCourseJSON"            ;
std::string WebAPI::request::compute                      = "compute"                     ;
std::string WebAPI::request::monitoring                   = "monitoring"                  ;
std::string WebAPI::request::calculatorInput              = "calculatorInput"             ;
std::string WebAPI::request::forgotLogin                  = "forgotLogin"                 ;
std::string WebAPI::request::login                        = "login"                       ;
std::string WebAPI::request::logout                       = "logout"                      ;
std::string WebAPI::request::signUp                       = "signUp"                      ;
std::string WebAPI::request::submitExercise               = "submitExercise"              ;
std::string WebAPI::request::submitExerciseNoLogin        = "submitExerciseNoLogin"       ;
std::string WebAPI::request::submitAnswers                = "submitAnswers"               ;
std::string WebAPI::request::submitExercisePreviewNoLogin = "submitExercisePreviewNoLogin";
std::string WebAPI::request::indicator                    = "indicator"                   ;
std::string WebAPI::request::pause                        = "pause"                       ;
std::string WebAPI::request::unpause                      = "unpause"                     ;
std::string WebAPI::request::modifyPage                   = "modifyPage"                  ;
std::string WebAPI::request::clonePage                    = "clonePageJSON"               ;
std::string WebAPI::request::problemGiveUp                = "problemGiveUp"               ;
std::string WebAPI::request::exerciseNoLogin              = "exerciseNoLogin"             ;
std::string WebAPI::request::exerciseJSON                 = "exerciseJSON"                ;
std::string WebAPI::request::problemGiveUpNoLogin         = "problemGiveUpNoLogin"        ;
std::string WebAPI::request::templateNoLogin              = "templateNoLogin"             ;
std::string WebAPI::request::templateJSONNoLogin          = "templateJSONNoLogin"         ;
std::string WebAPI::request::topicListJSONNoLogin         = "topicListJSONNoLogin"        ;
std::string WebAPI::request::problemSolutionNoLogin       = "problemSolutionNoLogin"      ;
std::string WebAPI::request::homeworkFromSource           = "homeworkFromSource"          ;
std::string WebAPI::request::homeworkSource               = "homeworkSource"              ;
std::string WebAPI::request::addUsers                     = "addUsers"                    ;
std::string WebAPI::request::addEmails                    = "addEmails"                   ;
std::string WebAPI::request::userList                     = "userList"                    ;
std::string WebAPI::request::passwordList                 = "passwordList"                ;
std::string WebAPI::request::solveRequest                 = "solveJSON"                   ;
std::string WebAPI::request::compareExpressions           = "compareExpressions"          ;
std::string WebAPI::request::compareExpressionsInternal   = "compareExpressionsInternal"  ;
std::string WebAPI::request::compareExpressionsGiven      = "compareExpressionsGiven"     ;
std::string WebAPI::request::compareExpressionsDesired    = "compareExpressionsDesired"   ;

std::string WebAPI::request::slides::source               = "slidesSource"                ;
std::string WebAPI::request::slides::pdfFromSource        = "slidesFromSource"            ;
std::string WebAPI::request::slides::title                = "slideTitle"                  ;
std::string WebAPI::request::slides::files                = "files"                       ;
std::string WebAPI::request::slides::content              = "content"                     ;
std::string WebAPI::request::slides::queryHomework        = "queryHomework"               ;
std::string WebAPI::request::slides::isSolution           = "isSolution"                  ;
std::string WebAPI::request::slides::slideFilename        = "slideFilename"               ;

std::string WebAPI::frontend::currentPage     = "currentPage"    ;
std::string WebAPI::frontend::calculatorPage  = "calculator"     ;
std::string WebAPI::frontend::problemPage     = "problemPage"    ;
std::string WebAPI::frontend::problemFileName = "problemFileName";
std::string WebAPI::frontend::exercise        = "exerciseJSON"   ;
std::string WebAPI::frontend::exerciseType    = "exerciseType"   ;
std::string WebAPI::frontend::scoredQuiz      = "scoredQuizJSON" ;

std::string WebAPI::UserInfo::processMonitoring = "processMonitoring";

std::string WebAPI::calculator::testFileNameVirtual = "test/automated_test_known_inputs_outputs.json";

std::string DatabaseStrings::labelUserId                            = "id"                         ;
std::string DatabaseStrings::labelUsername                          = "username"                   ;
std::string DatabaseStrings::labelPassword                          = "password"                   ;
std::string DatabaseStrings::labelUserRole                          = "userRole"                   ;
std::string DatabaseStrings::labelProblemDataJSON                   = "problemDataJSON"            ;
std::string DatabaseStrings::labelProblemFileName                   = "problemFileName"            ;
std::string DatabaseStrings::labelAuthenticationToken               = "authenticationToken"        ;
std::string DatabaseStrings::labelActivationToken                   = "activationToken"            ;
std::string DatabaseStrings::labelTimeOfActivationTokenCreation     = "activationTokenCreationTime";
std::string DatabaseStrings::labelTimeOfAuthenticationTokenCreation = "authenticationCreationTime" ;
std::string DatabaseStrings::labelEmail                             = "email"                      ;

std::string DatabaseStrings::tableUsers           = "users"     ;
std::string DatabaseStrings::theDatabaseName      = "calculator";
std::string DatabaseStrings::tableDeadlines       = "deadlines" ;
std::string DatabaseStrings::labelDeadlines       = "deadlines" ;

std::string DatabaseStrings::labelSection          = "studentSection";
std::string DatabaseStrings::labelCurrentCourses   = "currentCourses";
std::string DatabaseStrings::labelDeadlinesSchema  = "deadlineSchema";
std::string DatabaseStrings::labelInstructor       = "instructor"    ;
std::string DatabaseStrings::labelSemester         = "semester"      ;

std::string DatabaseStrings::tableProblemWeights       = "problemWeights"      ;
std::string DatabaseStrings::labelProblemWeightsSchema = "problemWeightsSchema";
std::string DatabaseStrings::labelProblemWeight        = "weight"              ;

std::string DatabaseStrings::tableProblemInformation    = "problemInformation"    ;
std::string DatabaseStrings::labelProblemTotalQuestions = "problemsTotalQuestions";

std::string DatabaseStrings::labelSectionsTaught = "sectionsTaught";

std::string DatabaseStrings::tableEmailInfo                   = "emailInfo"                  ;
std::string DatabaseStrings::labelLastActivationEmailTime     = "lastActivationEmailTime"    ;
std::string DatabaseStrings::labelNumActivationEmails         = "numActivationEmails"        ;
std::string DatabaseStrings::labelUsernameAssociatedWithToken = "usernameAssociatedWithToken";

std::string DatabaseStrings::tableDeleted             = "deleted"          ;
std::string DatabaseStrings::anyFielD                 = "${any}"           ;
std::string DatabaseStrings::objectSelectoR           = "${number}"        ;
std::string DatabaseStrings::objectSelectorMongo      = "$oid"             ;
std::string DatabaseStrings::labelFields              = "fields"           ;
std::string DatabaseStrings::labelTable               = "table"            ;
std::string DatabaseStrings::labelIdMongo             = "_id"              ;
std::string DatabaseStrings::labelCurrentPage         = "currentPage"      ;
std::string DatabaseStrings::labelPageSignUp          = "signUp"           ;
std::string DatabaseStrings::labelCalculatorRequest   = "calculatorRequest";
std::string DatabaseStrings::labelCalculatorInput     = "calculatorInput"  ;
std::string DatabaseStrings::labelPageAccount         = "account"          ;
std::string DatabaseStrings::labelPageActivateAccount = "activateAccount"  ;

std::string DatabaseStrings::errorDatabaseDisableD    = "Database disabled explicitly by server configuration. ";

std::string Configuration::processMonitoringBanned                  = "processMonitoringBanned"                 ;
std::string Configuration::monitorPingTime                          = "monitorPingTime"                         ;
std::string Configuration::millisecondsReplyAfterComputation        = "millisecondsReplyAfterComputation"       ;
std::string Configuration::builtInTLSAvailable                      = "builtInTLSAvailable"                     ;
std::string Configuration::serverAutoMonitor                        = "serverAutoMonitor"                       ;
std::string Configuration::disableDatabaseLogEveryoneAsAdmin        = "disableDatabaseLogEveryoneAsAdmin"       ;
std::string Configuration::serverDetailedLog                        = "serverDetailedLog"                       ;
std::string Configuration::gitRepository                            = "gitRepository"                           ;
std::string Configuration::dontUseOpenSSL                           = "dontUseOpenSSL"                          ;
std::string Configuration::portHTTP                                 = "portHTTP"                                ;
std::string Configuration::portHTTPSOpenSSL                         = "portHTTPSOpenSSL"                        ;
std::string Configuration::portHTTPSBuiltIn                         = "portHTTPSBuiltIn"                        ;
std::string Configuration::HTMLCommon                 = "html-common/"                                         ;
std::string Configuration::HTMLCommonFonts            = "html-common/fonts"                                    ;
std::string Configuration::HTMLCommonFont             = "html-common/font"                                     ;
std::string Configuration::HTMLAceSrcMin              = "html-common/ace/src-min/"                             ;
std::string Configuration::examples                   = "examples/"                                            ;

std::string Configuration::calculatorHTML             = "calculator-html/"                                     ;
std::string Configuration::publicHTML                 = "public_html/"                                         ;
std::string Configuration::certificatesPublic         = "certificates-public/"                                 ;
std::string Configuration::sourceCode                 = "src/"                                                 ;
std::string Configuration::testFolder                 = "test/"                                                ;
std::string Configuration::outputFolder               = "output/"                                              ;
std::string Configuration::HTMLGeneral                = "html/"                                                ;
std::string Configuration::problemsFolder             = "problems/"                                            ;
std::string Configuration::courseTemplates            = "coursetemplates/"                                     ;
std::string Configuration::coursesAvailable           = "coursesavailable/"                                    ;
std::string Configuration::topicLists                 = "topiclists/"                                          ;
std::string Configuration::laTeXMaterials             = "LaTeX-materials/"                                     ;
std::string Configuration::slidesVideo                = "slides-video/"                                        ;
std::string Configuration::freecalc                   = "freecalc/"                                            ;
std::string Configuration::useMathTags                = "useMathTags"                                          ;

