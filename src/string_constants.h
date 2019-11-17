#ifndef vpfWEBAPI_HEADER_H
#define vpfWEBAPI_HEADER_H
#include "macros.h"

static ProjectInformationInstance vpfWEBAPI_HEADER_Hinstance(__FILE__, "Header, web api strings. ");

class WebAPI {
public:
  static std::string appNoCache;
  static std::string app;
  struct request {
    static std::string calculatorPage;
    static std::string setProblemData;
    static std::string changePassword;
    static std::string examplesJSON;
    static std::string activateAccountJSON;
    static std::string calculatorHTML;
    static std::string onePageJS;
    static std::string onePageJSWithHash;
    static std::string userInfoJSON;
    static std::string editPage;
    static std::string selectCourseJSON;
    static std::string serverStatusJSON;
    static std::string clonePage;
    static std::string indicator;
    static std::string pause;
    static std::string unpause;
    static std::string submitExerciseNoLogin;
    static std::string submitExercisePreviewNoLogin;
    static std::string submitExercise;
    static std::string submitAnswers;
    static std::string problemGiveUp;
    static std::string problemGiveUpNoLogin;
    static std::string workerId;
    static std::string workerIndex;
    static std::string forgotLogin;
    static std::string compute;
    static std::string monitoring;
    static std::string calculatorInput;
    static std::string login;
    static std::string logout;
    static std::string signUp;
  };
  struct result {
    static std::string resultHtml;
    static std::string resultStringified;
    static std::string syntaxErrors;
    static std::string resultLabel;
    static std::string badInput;
    static std::string comments;
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
  };
  struct problem {
    static std::string title;
    static std::string fileName;
    static std::string fileNameTarget;
    static std::string fileContent;
    static std::string idProblem; //<- a little more specific than just id, easier to string-search
    static std::string deadlineSingle;
    static std::string failedToLoadProblem;
    static std::string problemLabel;
    static std::string content;
    static std::string commentsProblem;
  };
  struct databaseParameters {
    static std::string entryPoint;
    static std::string labels;
    static std::string operation;
    static std::string fetch;
  };
  static std::string HeaderCacheControl;
  static std::string commentsServer;
  struct UserInfo {
    static std::string processMonitoring;
  };
};

class Configuration {
public:
  static std::string processMonitoringBanned;
  static std::string processMonitoringTriggerTime;
  static std::string millisecondsReplyAfterComputationDefault;
  static std::string millisecondsReplyAfterComputation;
  static std::string builtInTLSAvailable;
  static std::string serverAutoMonitor;
  static std::string monitorPingTime;
  static std::string disableDatabaseLogEveryoneAsAdmin;
  static std::string serverDetailedLog;
  static std::string gitRepository;
  static std::string autoUnitTest;
};

class DatabaseStrings {
public:
  // calculator credentials for database use
  static std::string theDatabaseUser;
  static std::string theDatabaseName;
  static std::string theDatabaseNameMongo;
  // user column names
  static std::string labelUserId;
  static std::string labelUsername;
  static std::string labelEmail;
  static std::string labelPassword;
  static std::string labelAuthenticationToken;

  static std::string labelActivationToken;
  static std::string labelUserRole;
  static std::string labelProblemDatA;
  static std::string labelProblemDataJSON;
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
  static std::string labelProblemWeights;
  // problem information
  static std::string tableProblemInformation;
  static std::string labelProblemTotalQuestions;
  static std::string labelProblemName;
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
};

#endif
