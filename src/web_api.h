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
  bool processLoginUserInfo(const std::string& comments);
  bool processSelectCourseJSON();
  bool processLogout();
  bool processSignUp();
  bool processForgotLogin();
  bool processEditPageJSON(bool showSourceRelaxed);
  void reset(WebWorker& inputOwner);
  bool serveResponseFalseIfUnrecognized(
    std::stringstream& argumentProcessingFailureComments,
    std::stringstream& comments
  );
  static std::string getHtmlTagWithManifest();
  static std::string modifyProblemReport();
  static JSData clonePageResult();
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
    // When this is set, will returns the source of the problem
    // even though the user has no admin rights.
    // Intended for non-teaching uses when anonymous
    // users of the system are allowed to peek inside the source.
    bool showSourceRelaxed
  );
  static std::string getJavascriptCaptcha();
  static std::string getCaptchaDiv();
  static JSData getExamPageJSON();
  static std::string getJSONFromTemplate();
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
  static JSData getAccountsPageJSON(
    const std::string& hostWebAddressWithPort
  );
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
  };
};

#endif // header_web_api_ALREADY_INCLUDED
