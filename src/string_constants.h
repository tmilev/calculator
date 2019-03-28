#ifndef vpfWEBAPI_HEADER_H
#define vpfWEBAPI_HEADER_H
#include "vpfMacros.h"

static ProjectInformationInstance vpfWEBAPI_HEADER_Hinstance(__FILE__, "Header, web api strings. ");

struct WebAPI {
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
    static std::string indicator;
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
    static std::string error;
    static std::string status;
    static std::string crashReport;
  };
  struct problem {
    static std::string title;
    static std::string fileName;
    static std::string idProblem; //<- a little more specific than just id, easier to string-search
    static std::string deadlineSingle;
    static std::string failedToLoadProblem;
    static std::string problemLabel;
  };
  struct databaseParameters {
    static std::string entryPoint;
    static std::string labels;
    static std::string operation;
    static std::string fetch;
  };
  static std::string HeaderCacheControl;
  static std::string problemContent;
  static std::string commentsProblem;
  static std::string commentsServer;
  struct UserInfo {
    static std::string processMonitoring;
  };
};

struct Configuration {
  static std::string processMonitoringAllowedByDefault;
  static std::string processMonitoringTriggerTime;
  static std::string replyAfterComputationMillisecondsDefault;
  static std::string replyAfterComputationMilliseconds;
};

struct DatabaseStrings
{
public:
///credentials of calculator to use database
  static std::string theDatabaseUser;
  static std::string theDatabaseName;
  static std::string theDatabaseNameMongo;
///user column names
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
///email info
  static std::string tableEmailInfo;
  static std::string labelLastActivationEmailTime;
  static std::string labelNumActivationEmails;
  static std::string labelUsernameAssociatedWithToken;

///course info column names
  static std::string labelCurrentCourses;
  static std::string labelSection;
  static std::string labelDeadlinesSchema;
  static std::string labelProblemWeightsSchema;
  static std::string labelSectionsTaught;
  static std::string labelInstructor;
  static std::string labelSemester;
///deadlines info
  static std::string labelDeadlines;
  static std::string tableDeadlines;
///problem weights info
  static std::string tableProblemWeights;
  static std::string labelProblemWeights;
///problem information
  static std::string tableProblemInformation;
  static std::string labelProblemTotalQuestions;
  static std::string labelProblemName;
///deleted fields
  static std::string anyFielD;
  static std::string labelFields;
  static std::string labelTable;
  static std::string objectSelectoR;
  static std::string objectSelectorMongo;
  static std::string labelIdMongo;
///deleted info
  static std::string tableDeleted;
///app info
  static std::string labelCurrentPage;
  static std::string labelPageSignUp;
  static std::string labelPageAccount;
  static std::string labelPageActivateAccount;
  static std::string labelCalculatorRequest;
  static std::string labelCalculatorInput;
};

#endif
