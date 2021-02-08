// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderHtmlSnippetsAlreadyDefined
#define vpfHeaderHtmlSnippetsAlreadyDefined
#include "macros.h"
#include "general_maps.h"
#include "general_list_references.h"

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
  std::string getOnePageJavascriptBrowserify();
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
  bool processAddUserEmails();
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
  bool processTopicListJSON();
  bool processSolveJSON();
  bool processScoresInCoursePage();
  bool processAssignTeacherToSection();
  bool processExamPageJSON();
  bool processTemplateJSON();
  bool processLoginUserInfo(const std::string &comments);
  bool processSelectCourseJSON();
  bool processExamPageInterpreter();
  bool processLogout();
  bool processSignUp();
  bool processForgotLogin();
  bool processEditPageJSON();

  void reset(WebWorker& inputOwner);
  bool serveResponseFalseIfUnrecognized(
    std::stringstream& argumentProcessingFailureComments,
    std::stringstream& comments
  );
  static std::string getHtmlTagWithManifest();
  static std::string modifyProblemReport();
  static JSData clonePageResult();
  static std::string addTeachersSections();
  static std::string addUserEmails(const std::string& hostWebAddressWithPort);
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

  static JSData getEditPageJSON();
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
    FormatExpressions& theFormat
  );

  static JSData getSelectCourseJSON();
  static std::string getScoresPage();
  static std::string getApp(bool appendBuildHash);
  static std::string getHTMLAllInOneJavascriptCSS(
    const std::string& virtualHTMLFileName, bool appendBuildHash
  );
  static std::string getCompareExpressionsPage(bool appendBuildHash);
  static std::string getOnePageJS();
  static std::string getScoresInCoursePage();
  static std::string setProblemDeadline();
  static std::string setProblemWeight();
  static JSData getAccountsPageJSON(const std::string& hostWebAddressWithPort);
  static std::string getAccountsPageBody(
    const std::string& hostWebAddressWithPort
  );
  static std::string toStringAssignSection();
  static std::string toStringUserDetails(
    bool adminsOnly,
    List<JSData>& theUsers,
    const std::string& hostWebAddressWithPort
  );
  static std::string toStringUserScores();
  static std::string toStringUserDetailsTable(
    bool adminsOnly,
    List<JSData>& theUsers,
    const std::string& hostWebAddressWithPort
  );
  static std::string getSanitizedComment(
    const Expression& input, FormatExpressions& theFormat, bool& resultIsPlot
  );
  class Test {
  public:
    static bool all();
    static bool solveJSON();
    static bool compareExpressions();
  };
};

class HtmlRoutines {
private:
  static std::string getCalculatorComputationAnchor(
    const std::string& urlApplication,
    const std::string& inputNoEncoding,
    const std::string& desiredAnchorTextEmptyForDefault
  );
  static std::string getMathSpan(
    const std::string& input,
    int upperNumChars = 10000,
    bool useDisplayStyle = false
  );

public:
  static MapList<std::string, std::string, MathRoutines::hashString>& preLoadedFiles();
  static std::stringstream outputStream;
  static int globalMathSpanID;
  static int GlobalGeneralPurposeID;
  static int numLinesAll;
  static int numRegularLines;
  static int numDashedLines;
  static int numDottedLines;
  static int shiftX;
  static int shiftY;
  static int scale;
  static void loadStrings();
  static std::string gitRepository;

  static std::string URLKeyValuePairsToNormalRecursiveHtml(
    const std::string& input, int recursionDepth = 0
  );

  // A script tag a that contains a json data structure.
  static std::string scriptFromJSON(const std::string& scriptType, const JSData& scriptContent);
  // Same as scriptFromJSON but uses a no-display span with name="script".
  static std::string jsonContainer(
    const std::string& scriptType, const JSData& scriptContent
  );
  static void convertURLStringToNormal(
    const std::string& input, std::string& output, bool replacePlusBySpace
  );
  static std::string convertURLStringToNormal(
    const std::string& input, bool replacePlusBySpace
  );
  static bool convertStringToHtmlStringReturnTrueIfModified(
    const std::string& input, std::string& output, bool doReplaceNewLineByBr
  );
  static std::string convertStringToURLStringExceptDashesAndSlashes(
    const std::string& input
  );
  static std::string convertStringToURLString(
    const std::string& input, bool usePlusesForSpacebars
  );
  static std::string convertStringToHtmlString(
    const std::string& theString, bool doReplaceNewLineByBr
  );
  static std::string convertStringToHtmlStringRestrictSize(
    const std::string& theString, bool doReplaceNewLineByBr, int maxStringSize
  );
  static std::string convertStringEscapeQuotesAndBackslashes(
    const std::string& input
  );
  static std::string convertStringEscapeNewLinesQuotesBackslashes(
    const std::string& input
  );

  static bool URLStringToNormalOneStep(
    std::string& readAhead, std::stringstream& out, bool replacePlusBySpace
  );
  static void replaceEqualitiesAndAmpersandsBySpaces(std::string& inputOutput);
  static std::string getCalculatorComputationURL(
    const std::string& inputNoEncoding
  );
  static std::string getProblemURLRelative(const std::string& problemName);
  static std::string getCalculatorComputationAnchorSamePage(
    const std::string& inputNoEncoding
  );
  static std::string getCalculatorComputationAnchorNewPage(
    const std::string& inputNoEncoding,
    const std::string& desiredAnchorTextEmptyForDefault
  );

  static std::string getCalculatorComputationNewPage(
    const std::string& inputNoEncoding,
    const std::string& desiredAnchorTextEmptyForDefault
  );
  static std::string getSliderSpanStartsHidden(
    const std::string& content,
    const std::string& label = "Expand/collapse",
    const std::string& desiredID = ""
  );
  static std::string getHtmlLinkToGithubRepository(const std::string& displayString);
  static std::string getHtmlLinkFromProjectFileName(
    const std::string& fileName,
    const std::string& fileDesc = "",
    int line = - 1
  );
  static std::string doubleBackslashes(const std::string& input);

  static std::string getCSSLink(const std::string& fileNameVirtual);
  static std::string getJavascriptLink(const std::string& fileNameVirtual);

  static const std::string& getCSSAddStyleTags(
    const std::string& fileNameVirtual
  );
  static const std::string& getJavascriptAddScriptTags(
    const std::string& fileNameVirtual
  );
  static const std::string& getFile(
    const std::string& fileNameVirtual,
    const std::string& additionalBeginTag = "",
    const std::string& additionalEndTag = ""
  );

  static const std::string& getJavascriptAceEditorScriptWithTags();

  static std::string getJavascriptVariable(const std::string& variableName);

  static std::string getMathDisplayStyle(
    const std::string& input, int upperNumChars = 10000
  );
  static std::string getMathNoDisplay(
    const std::string& input, int upperNumChars = 10000
  );
  static std::string getStyleButtonLikeHtml();
  static std::string clearNewLines(const std::string& theString);
  static std::string backslashQuotes(const std::string& theString);
  static std::string clearSlashes(const std::string& theString);
  static std::string cleanUpForLaTeXLabelUse(const std::string& inputString);
  static bool isRepresentedByItselfInURLs(char input);
  static void clearDollarSigns(std::string& theString, std::string& output);
  static void subEqualitiesWithSimeq(
    std::string& theString, std::string& output
  );
  static bool accountOneInputPercentEncodedString(
    const std::string& fieldName,
    const std::string& fieldValue,
    MapList<std::string, std::string, MathRoutines::hashString>& outputMap,
    std::stringstream& commentsOnFailure
  );
  static bool chopPercentEncodedString(
    const std::string& input,
    MapList<std::string, std::string, MathRoutines::hashString>& outputMap,
    std::stringstream& commentsOnFailure
  );
  static bool chopPercentEncodedStringAppend(
    const std::string& input,
    MapList<std::string, std::string, MathRoutines::hashString>& outputMap,
    std::stringstream& commentsOnFailure
  );
  static void elementToStringTooltip(
    const std::string& input,
    const std::string& inputTooltip,
    std::string& output,
    bool useHtml
  );
  static std::string elementToStringTooltip(
    const std::string& input, const std::string& inputTooltip, bool useHtml
  ) {
    std::string result;
    HtmlRoutines::elementToStringTooltip(input, inputTooltip, result, useHtml);
    return result;
  }
  static std::string elementToStringTooltip(
    const std::string& input, const std::string& inputTooltip
  ) {
    return HtmlRoutines::elementToStringTooltip(input, inputTooltip, true);
  }
  static uint32_t redGreenBlue(unsigned int r, unsigned int g, unsigned int b);
  static void makeStdCoutReport(const std::string& input);
  static void makeReportIndicatorFile(const std::string& input);
  static std::string toHtmlTable(
    List<std::string>& labels,
    List<List<std::string> >& content,
    bool nestTables
  );
  static std::string toHtmlTableRowsFromStringContainingJSON(
    const std::string& theJSON
  );
  static std::string toHtmlTableRowsFromJSON(const JSData& input);
};
#endif
