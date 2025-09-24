#ifndef header_web_api_ALREADY_INCLUDED
#define header_web_api_ALREADY_INCLUDED

#include "general_logging_global_variables.h"

class JSData;

class BuilderApplication {
public:
  std::string htmlRaw;
  std::string htmlJSbuild;
  List<std::string> currentWords;
  std::string buildFile;
  List<std::string> jsFileNames;
  List<std::string> jsFileContents;
  std::string allInOneJavascriptTagOriginal;
  std::string allInOneJavascriptTagDesired;
  std::string calculatorCSSTagOriginal;
  std::string calculatorCSSTagDesired;
  void initializeTags(bool appendBuildHash);
  bool fileNameAllowedToBeMissing(const std::string& input);
  void buildHtmlJavascriptPage(bool appendBuildHash);
  std::string getBrowserificationAssembled(const std::string& entryPoint);
  bool loadJavascriptFileNames(
    const std::string& buildFileNameVirtual,
    std::stringstream* commentsOnFailure
  );
};

class WebWorker;

class WebAPIResponse {
public:
  WebWorker* owner;
  static std::string youHaveReachedTheBackend;
  WebAPIResponse();
  bool processDatabaseJSON();
  bool processDatabaseDeleteEntry();
  bool processDatabaseModifyEntry();
  bool processProblemGiveUp();
  bool processPing();
  bool processProblemSolution();
  bool processPauseWorker();
  bool processUnpauseWorker();
  bool processAccountsJSON();
  void initializeCalculatorComputation();
  bool processCompute();
  bool processCalculatorExamplesJSON();
  bool processSubmitAnswers();
  bool processSubmitAnswerHardcoded();
  bool processSubmitAnswersPreview();
  bool processServerStatusJSON();
  bool processSetProblemWeight();
  bool processSetProblemDeadline();
  bool processSlidesOrHomeworkFromSource();
  bool processSlidesSource();
  bool processClonePage();
  bool processModifyPage();
  bool processAddUsersOrEmails();
  bool processComputationIndicator();
  bool processChangePassword(const std::string& reasonForNoAuthentication);
  bool processActivateAccount();
  void activateAccount(JSData& output);
  bool processScores();
  bool processApp(bool appendBuildHash);
  bool processCompareExpressionsPage(bool appendBuildHash);
  bool processCompareExpressions(bool hideDesiredAnswer);
  bool processCheckAnswer(bool hideDesiredAnswer);
  JSData checkAnswer(bool hideDesiredAnswer);
  bool processCalculatorOnePageJS(bool appendBuildHash);
  bool processCalculatorWebWorkerJS(bool appendBuildHash);
  bool processTopicListJSON();
  bool processSolveJSON();
  bool processScoresInCoursePage();
  bool processAssignTeacherToSection();
  bool processExamPageJSON();
  bool processTemplateJSON();
  bool processDeleteAccount();
  bool processLoginUserInfo(const std::string& comments);
  bool processSelectCourseJSON();
  bool processLogout();
  bool processSignUp();
  bool processForgotLogin();
  void forgotLogin(JSData& result);
  bool processEditPageJSON(bool showSourceRelaxed);
  void reset(WebWorker& inputOwner);
  bool serveResponseFalseIfUnrecognized(
    std::stringstream& argumentProcessingFailureComments,
    std::stringstream& comments
  );
  static std::string modifyProblemReport();
  static JSData clonePageResult();
  static void changePassword(
    JSData& result, const std::string& reasonForNoAuthentication
  );
  static JSData setEmail(
    bool confirmEmailOnlyNoPasswordSet, const std::string& input
  );
  static bool doSetEmail(
    bool confirmEmailOnlyNoPasswordSet,
    UserCalculatorData& inputOutputUser,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNonSensitive,
    std::stringstream* commentsGeneralSensitive
  );
  static JSData getSignUpRequestResult();
  static bool verifyRecaptcha(
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNONsensitive,
    std::stringstream* commentsGeneralSensitive
  );
  static std::string addTeachersSections();
  static std::string addUsersOrEmails(
    const std::string& hostWebAddressWithPort
  );
  static bool addUsersFromData(
    const std::string& emailList,
    const std::string& userPasswords,
    const std::string& userRole,
    const std::string& userGroup,
    int& outputNumberOfNewUsers,
    int& outputNumberOfUpdatedUsers,
    std::stringstream* comments
  );
  static bool addOneUser(
    const std::string& userNameOrEmail,
    const std::string& password,
    const std::string& desiredUserRole,
    const std::string& userGroup,
    int& outputNumberOfNewUsers,
    int& outputNumberOfUpdatedUsers,
    std::stringstream* commentsOnFailure
  );
  static JSData submitAnswersJSON(
    const std::string& inputRandomSeed,
    bool* outputIsCorrect,
    bool timeSafetyBrake
  );
  static JSData submitAnswersPreviewJSON();
  static JSData submitAnswersJSON();
  JSData submitAnswersHardcoded(bool hideDesiredAnswer);
  static JSData getProblemSolutionJSON();
  static JSData getAnswerOnGiveUp();
  static JSData getEditPageJSON(
    // When this is set, will returns the source of
    // the problem
    // even though the user has no admin rights.
    // Intended for non-teaching uses when anonymous
    // users of the system are allowed to peek inside the source.
    bool showSourceRelaxed
  );
  static std::string getJavascriptCaptcha();
  static std::string getCaptchaDiv();
  static JSData getExamPageJSON();
  static std::string getJSONFromTemplate();
  // Deletes an account in two phases.
  // The first phase generates a confirmation token.
  // The second phase actually deletes the account if
  // the confirumation token is included.
  // The confirmation token is a guard
  // against an attacker who generates a link to
  // the deletion API and get a victim to accidentally click
  // and erase their account without wanting to do so.
  static JSData deleteAccount();
  // Generates a confirmation token for the account deletion.
  static JSData deleteAccountGenerateToken();
  // Deletes the account if the given confirmation token
  // matches the record.
  static JSData deleteAccountFinal(const std::string& token);
  static JSData getJSONUserInfo(const std::string& comments);
  static void getJSDataUserInfo(
    JSData& outputAppend, const std::string& comments
  );
  static JSData getTopicTableJSON();
  JSData solveJSON();
  JSData compareExpressions(bool hideDesiredAnswer);
  static JSData getAnswerOnGiveUp(
    const std::string& inputRandomSeed,
    std::string* outputNakedAnswer = nullptr,
    bool* answerGenerationSuccess = nullptr,
    bool doIncludeTimeStats = true
  );
  static std::string getCommentsInterpretation(
    Calculator& interpreterWithAdvice,
    int indexShift,
    FormatExpressions& format
  );
  static JSData getSelectCourseJSON();
  static std::string getScoresPage();
  static std::string getApp(bool appendBuildHash);
  static std::string getHTMLAllInOneJavascriptCSS(
    const std::string& virtualHTMLFileName, bool appendBuildHash
  );
  static std::string getCompareExpressionsPage(bool appendBuildHash);
  static std::string getOnePageJS();
  static std::string getCalculatorWorkerJS();
  static std::string getBrowserification(
    const std::string& buildJSONVirtualFileName,
    const std::string& scriptEntryPoint
  );
  static std::string getScoresInCoursePage();
  static std::string setProblemDeadline();
  static std::string setProblemWeight();
  static JSData getAccountsPageJSON(const std::string& hostWebAddressWithPort);
  static std::string toStringUserScores();
  static std::string toStringUserDetailsTable(
    bool adminsOnly,
    List<JSData>& users,
    const std::string& hostWebAddressWithPort
  );
  static std::string getSanitizedComment(
    const Expression& input, FormatExpressions& format, bool& resultIsPlot
  );
  class Test {
  public:
    static bool scoredQuiz(DatabaseType databaseType);
    static bool all();
    static bool solveJSON();
    static bool compareExpressions();
    static bool addUsersFromData();
    static bool forgotLogin();
    static bool changePasswordEmailOnly();
    static bool changePassword();
    static bool signUp();
    static bool deleteAccount();
    static void extactActivationTokenFromEmail(
      const std::string& email, std::string& outputToken
    );
  };
};

#endif // header_web_api_ALREADY_INCLUDED
