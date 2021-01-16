#ifndef vpfWEBAPI_HEADER_H
#define vpfWEBAPI_HEADER_H
#include "macros.h"

class MainFlags {
public:
  static std::string server;
  static std::string pathExecutable;
  static std::string configurationFile;
  static std::string test;
  static std::string help;
  static std::string daemon;
};

class WebAPI {
public:
  static std::string appNoCache;
  static std::string app;
  static std::string compareExpressionsPage;
  static std::string compareExpressionsPageNoCache;
  struct request {
    static std::string requestAPI;
    static std::string calculatorPage;
    static std::string database;
    static std::string setProblemWeight;
    static std::string setProblemDeadline;
    static std::string changePassword;
    static std::string examplesJSON;
    static std::string activateAccountJSON;
    static std::string calculatorHTML;
    static std::string onePageJS;
    static std::string onePageJSWithHash;
    static std::string calculatorCSS;
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
    static std::string compareExpressionsGiven;
    static std::string compareExpressionsDesired;
    struct slides {
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
    static std::string currentPage;
    static std::string addUsers;
    static std::string addEmails;
    static std::string userList;
    static std::string passwordList;
  };
  struct result {
    static std::string resultHtml;
    static std::string syntaxErrors;
    static std::string syntaxErrorsExtra;
    static std::string resultLabel;
    static std::string badInput;
    static std::string comments;
    static std::string performance;
    static std::string computationTime;
    static std::string parsingLog;
    static std::string commentsGlobal;
    static std::string timeOut;
    static std::string timeOutComments;
    static std::string workerId;
    static std::string workerIndex;
    static std::string error;
    static std::string status;
    static std::string running;
    static std::string crashReport;
    static std::string millisecondsComputation;
    static std::string loginDisabledEveryoneIsAdmin;
    static std::string useFallbackDatabase;
    static std::string httpsSupport;
    static std::string comparison;
    struct ComparisonData {
      static std::string given;
      static std::string desired;
      static std::string givenRaw;
      static std::string desiredRaw;
      static std::string areEqual;
      static std::string areEqualAsAnswers;
    };

    static std::string solution;
    struct SolutionData {
      static std::string input;
      static std::string finalExpression;
      static std::string steps;
    };
  };
  struct frontend {
    static std::string problemFileName;
    static std::string problemPage;
    static std::string currentPage;
    static std::string exerciseType;
    static std::string scoredQuiz;
    static std::string exercise;
  };
  struct problem {
    static std::string title;
    static std::string fileName;
    static std::string courseHome;
    static std::string courseTitle;
    static std::string topicList;
    static std::string fileNameTarget;
    static std::string fileContent;
    static std::string idProblem; //<- a little more specific than just id, easier to string-search
    static std::string deadlineSingle;
    static std::string failedToLoadProblem;
    static std::string problemLabel;
    static std::string content;
    static std::string randomSeed;
    static std::string answerGenerationSuccess;
    static std::string commentsProblem;
    static std::string calculatorAnswerPrefix;
  };
  struct databaseParameters {
    static std::string labels;
    static std::string operation;
    static std::string fetch;
  };
  struct calculator {
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
  static std::string processMonitoringBanned                 ;
  static std::string processMonitoringTriggerTime            ;
  static std::string millisecondsReplyAfterComputation       ;
  static std::string builtInTLSAvailable                     ;
  static std::string dontUseOpenSSL                          ;
  static std::string serverAutoMonitor                       ;
  static std::string monitorPingTime                         ;
  static std::string disableDatabaseLogEveryoneAsAdmin       ;
  static std::string serverDetailedLog                       ;
  static std::string gitRepository                           ;
  static std::string portHTTP                                ;
  static std::string portHTTPSOpenSSL                        ;
  static std::string portHTTPSBuiltIn                        ;
  // Folder links follow.
  static std::string calculatorHTML            ;
  static std::string HTMLCommon                ;
  static std::string HTMLCommonFonts           ;
  static std::string HTMLJQueryMinified        ;
  static std::string HTMLAceSrcMin             ;
  static std::string HTMLJQueryUI              ;
  static std::string HTMLMathQuill             ;
  static std::string HTMLMathQuillFonts        ;

  static std::string HTMLCommonFont            ;
  static std::string publicHTML                ;
  static std::string certificatesPublic        ;
  static std::string sourceCode                ;
  static std::string testFolder                ;
  static std::string outputFolder              ;
  static std::string HTMLGeneral               ;
  static std::string problemsFolder            ;
  static std::string courseTemplates           ;
  static std::string coursesAvailable          ;
  static std::string topicLists                ;
  static std::string laTeXMaterials            ;
  static std::string slidesVideo               ;
  static std::string freecalc                  ;
  static std::string useMathTags               ;
};

class DatabaseStrings {
public:
  // calculator credentials for database use
  static std::string theDatabaseName;
  // user column names
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
  // email info
  static std::string tableEmailInfo;
  static std::string labelLastActivationEmailTime;
  static std::string labelNumActivationEmails;
  static std::string labelUsernameAssociatedWithToken;

  // course info column names
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
  // problem weights info
  static std::string tableProblemWeights;
  static std::string labelProblemWeight;
  // problem information
  static std::string tableProblemInformation;
  static std::string labelProblemTotalQuestions;
  // deleted fields
  static std::string anyFielD;
  static std::string labelFields;
  static std::string labelTable;
  static std::string objectSelectoR;
  static std::string objectSelectorMongo;
  static std::string labelIdMongo;
  // deleted info
  static std::string tableDeleted;
  // app info
  static std::string labelCurrentPage;
  static std::string labelPageSignUp;
  static std::string labelPageAccount;
  static std::string labelPageActivateAccount;
  static std::string labelCalculatorRequest;
  static std::string labelCalculatorInput;
  // errors
  static std::string errorDatabaseDisableD;
};

#endif
