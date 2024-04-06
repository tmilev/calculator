#ifndef header_string_constants_ALREADY_INCLUDED
#define header_string_constants_ALREADY_INCLUDED

#include <sstream>

class MainFlags {
public:
  static std::string server;
  static std::string format;
  static std::string basePath;
  static std::string configurationFile;
  static std::string test;
  static std::string help;
  static std::string daemon;
  static std::string loadDatabase;
  static std::string deploy;
};

class WebAPI {
public:
  static std::string appNoCache;
  static std::string app;
  static std::string compareExpressionsPage;
  static std::string compareExpressionsPageNoCache;
  struct Request {
    static std::string requestAPI;
    static std::string database;
    static std::string setProblemWeight;
    static std::string setProblemDeadline;
    static std::string changePassword;
    static std::string changePasswordPage;
    static std::string examplesJSON;
    static std::string activateAccountJSON;
    static std::string calculatorHTML;
    static std::string onePageJS;
    static std::string onePageJSWithHash;
    static std::string calculatorWorkerJS;
    static std::string calculatorWorkerJSWithHash;
    static std::string calculatorCSS;
    static std::string lieAlgebrasCSS;
    static std::string userInfoJSON;
    static std::string editPage;
    static std::string debugFlag;
    static std::string selectCourseJSON;
    static std::string serverStatusJSON;
    static std::string clonePage;
    static std::string indicator;
    static std::string pause;
    static std::string unpause;
    static std::string exerciseNoLogin;
    static std::string exerciseJSON;
    static std::string submitExerciseNoLogin;
    static std::string submitExercisePreviewNoLogin;
    static std::string submitExercise;
    static std::string templateNoLogin;
    static std::string templateJSONNoLogin;
    static std::string topicListJSONNoLogin;
    static std::string problemGiveUpNoLogin;
    static std::string problemSolutionNoLogin;
    static std::string homeworkFromSource;
    static std::string solveRequest;
    static std::string compareExpressions;
    static std::string compareExpressionsInternal;
    static std::string compareExpressionsGiven;
    static std::string compareExpressionsDesired;
    static std::string submitAnswerHardcoded;
    static std::string teachersAndSections;
    static std::string teachers;
    static std::string sections;
    struct Slides {
    public:
      static std::string source;
      static std::string pdfFromSource;
      static std::string title;
      static std::string files;
      static std::string isSolution;
      static std::string content;
      static std::string queryHomework;
      static std::string slideFilename;
    };

    static std::string homeworkSource;
    static std::string submitAnswers;
    static std::string modifyPage;
    static std::string problemGiveUp;
    static std::string workerId;
    static std::string forgotLogin;
    static std::string compute;
    static std::string monitoring;
    static std::string calculatorInput;
    static std::string login;
    static std::string logout;
    static std::string signUp;
    static std::string addUsers;
    static std::string addEmails;
    static std::string setTeacher;
    static std::string userList;
    static std::string passwordList;
  };

  struct Result {
    static std::string resultHtml;
    static std::string syntaxErrors;
    static std::string syntaxErrorsExtra;
    static std::string resultLabel;
    static std::string badInput;
    static std::string comments;
    static std::string performance;
    static std::string performancePerHandler;
    static std::string performanceTurnOffSlowRules;
    static std::string performanceTurnOffVerySlowRules;
    static std::string computationTime;
    static std::string parsingLog;
    static std::string commentsGlobal;
    static std::string commentsDebug;
    static std::string timeOut;
    static std::string timeOutComments;
    static std::string workerId;
    static std::string functions;
    static std::string workerIndex;
    static std::string outputString;
    static std::string error;
    static std::string status;
    static std::string running;
    static std::string crashReport;
    static std::string millisecondsComputation;
    static std::string loginDisabledEveryoneIsAdmin;
    static std::string debugLogin;
    static std::string database;
    static std::string httpsSupport;
    static std::string scriptType;
    static std::string scripts;
    struct ComparisonData {
      // The given (student answer) expression string.
      static std::string givenRaw;
      // The desired (teacher-prodived answer) expression string.
      static std::string desiredRaw;
      // The given answer as parsed / understood by the calculator.
      static std::string given;
      // The desired answer as parsed / understood by the calculator.
      static std::string desired;
      // If non-empty, signifies the calculator could not
      // evaluate one of the answers, for reasons such as division by zero.
      static std::string errorEvaluation;
      // If non-empty, signifies the calculator found some
      // obvious error in the answer such as wrong variable name.
      static std::string errorInAnswer;
      // Whether the desired and given answer simplify to
      // equal mathematical expressions.
      static std::string areEqual;
      // Whether the desired and given answer
      // are equal as aswers, i.e., simplify to equal mathematical expressions
      // under a reduced set of simplification rules.
      static std::string areEqualAsAnswers;
    };

    static std::string solution;
    struct SolutionData {
      static std::string input;
      static std::string finalExpression;
      static std::string comments;
      static std::string steps;
      static std::string transformation;
      static std::string annotations;
      static std::string stepType;
    };
  };

  struct Frontend {
    static std::string problemFileName;
    static std::string problemPage;
    static std::string currentPage;
    static std::string calculatorPage;
    static std::string exerciseType;
    static std::string scoredQuiz;
    static std::string exercise;
    static std::string calculatorHandlerDocumentation;
    static std::string calculatorExamples;
  };

  struct Problem {
    static std::string title;
    static std::string fileName;
    static std::string courseHome;
    static std::string courseTitle;
    static std::string topicList;
    static std::string fileNameTarget;
    static std::string fileContent;
    static std::string idProblem;
    // <- a little more specific than just id, easier to string-search
    static std::string answerId;
    // <- a little more specific than just id, easier to string-search
    static std::string answerValue;
    static std::string answerCheck;
    static std::string deadlineSingle;
    static std::string failedToLoadProblem;
    static std::string problemLabel;
    static std::string content;
    static std::string randomSeed;
    static std::string answerGenerationSuccess;
    static std::string commentsProblem;
    static std::string calculatorAnswerPrefix;
    static std::string answerWasHardCoded;
    static std::string forReal;
  };

  struct DatabaseParameters {
    static std::string findQuery;
    static std::string projector;
    static std::string operation;
    static std::string fetch;
    static std::string allTables;
  };

  struct Calculator {
  public:
    static std::string testFileNameVirtual;
  };

  static std::string headerCacheControl;
  static std::string commentsServer;
  struct UserInfo {
    static std::string processMonitoring;
  };

  static std::string userDefaultAdmin;
};

class Configuration {
public:
  static std::string processMonitoringBanned;
  static std::string processMonitoringTriggerTime;
  static std::string millisecondsReplyAfterComputation;
  static std::string builtInTLSAvailable;
  static std::string dontUseOpenSSL;
  static std::string serverAutoMonitor;
  static std::string monitorPingTime;
  static std::string disableDatabaseLogEveryoneAsAdmin;
  static std::string database;
  static std::string debugLogin;
  static std::string serverDetailedLog;
  static std::string gitRepository;
  static std::string portHTTP;
  static std::string portHTTPSOpenSSL;
  static std::string portHTTPSBuiltIn;
  // Folder links follow.
  static std::string calculatorWasm;
  static std::string calculatorHTML;
  static std::string HTMLCommon;
  static std::string HTMLCommonFonts;
  static std::string HTMLAceSrcMin;
  static std::string examples;
  static std::string HTMLCommonFont;
  static std::string publicHTML;
  static std::string certificatesPublic;
  static std::string sourceCode;
  static std::string testFolder;
  static std::string outputFolder;
  static std::string HTMLGeneral;
  static std::string problemsFolder;
  static std::string courseTemplates;
  static std::string coursesAvailable;
  static std::string topicLists;
  static std::string laTeXMaterials;
  static std::string slidesVideo;
  static std::string freecalc;
  static std::string useMathTags;
  static std::string webAddress;
  static std::string sendEmailFrom;
  static std::string actAsWebServerForTheseHosts;
  static std::string deploy;
  // Information needed to deploy the current master branch through github to a given URL.
  // The deployment code is in MainFunctions::deploy.
  struct Deploy {

    static std::string username;
    static std::string url;
    static std::string baseFolder;
  };
  struct ActAsWebServer {
    static std::string portHTTP;
    static std::string portHTTPS;
    static std::string baseFolder;
    static std::string landingPage;
    static std::string privateKeyFile;
    static std::string certificateFile;
  };
};

class DatabaseStrings {
public:
  // User column names.
  static std::string labelUserId;
  static std::string labelUsername;
  static std::string labelEmail;
  static std::string labelPassword;
  static std::string labelAuthenticationToken;
  static std::string labelActivationToken;
  static std::string labelUserRole;
  static std::string labelProblemDataJSON;
  static std::string labelProblemFileName;
  static std::string labelTimeOfActivationTokenCreation;
  static std::string labelTimeOfAuthenticationTokenCreation;
  static std::string tableUsers;
  // Email information.
  static std::string tableEmailInfo;
  static std::string labelLastActivationEmailTime;
  static std::string labelTotalActivationEmails;
  // Course information column names.
  static std::string labelCurrentCourses;
  static std::string labelSection;
  static std::string labelDeadlinesSchema;
  static std::string labelProblemWeightsSchema;
  static std::string labelSectionsTaught;
  static std::string labelInstructor;
  static std::string labelSemester;
  // deadlines info
  static std::string labelDeadlines;
  static std::string tableDeadlines;
  // Problem weights information
  static std::string tableProblemWeights;
  static std::string labelProblemWeight;
  // Problem information.
  static std::string tableProblemInformation;
  static std::string labelProblemTotalQuestions;
  // Deleted fields.
  static std::string anyField;
  static std::string labelFields;
  static std::string labelCollection;
  static std::string labelKey;
  static std::string labelValue;
  static std::string objectSelector;
  static std::string labelId;
  // Deleted information
  static std::string tableDeleted;
  // App information.
  static std::string labelCurrentPage;
  static std::string labelPageSignUp;
  static std::string labelPageAccount;
  static std::string labelPageActivateAccount;
  static std::string labelCalculatorRequest;
  static std::string labelCalculatorInput;
  // Errors.
  static std::string errorDatabaseDisabled;
  // Database result fields.
  static std::string resultSuccess;
  static std::string resultComments;
  // Database request fields.
  static std::string requestType;
  static std::string requestContent;
  static std::string requestOptions;
  static std::string requestProjectAwayFrom;
  static std::string requestProjectInto;
  static std::string resultContent;
  static std::string messageId;
  static std::string messageHandle;
  static std::string messageSize;
  static std::string error;
  static std::string maximumNumberOfItems;
};

#endif // header_string_constants_ALREADY_INCLUDED
