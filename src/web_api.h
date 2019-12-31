// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderHtmlSnippetsAlreadyDefined
#define vpfHeaderHtmlSnippetsAlreadyDefined
#include "macros.h"
#include "general_maps.h"
#include "general_list_references.h"

static ProjectInformationInstance projectInfoHtmlSnippetsHeader(__FILE__, "Html snippets header.");

class JSData;

class BuilderApplication {
public:
  std::string htmlRaw;
  std::string htmlJSbuild;
  List<std::string> currentWords;
  // List<bool> charIsSplitting;
  List<std::string> jsFileNames;
  List<std::string> jsFileContents;

  void BuildHtmlJSpage(bool appendBuildHash);
  std::string GetOnePageJSBrowserify();
};

class WebWorker;

class WebAPIResponse {
  public:
  WebWorker* owner;
  static std::string youHaveReachedTheBackend;
  WebAPIResponse();

  bool ProcessDatabaseJSON();
  bool ProcessDatabaseDeleteEntry();
  bool ProcessDatabaseModifyEntry();
  bool ProcessProblemGiveUp();
  bool ProcessProblemSolution();
  bool ProcessPauseWorker();
  bool ProcessUnpauseWorker();
  bool ProcessAccountsJSON();
  bool ProcessCompute();
  bool ProcessCalculatorExamplesJSON();
  bool ProcessSubmitAnswers();
  bool ProcessSubmitAnswersPreview();
  bool ProcessServerStatusJSON();
  bool ProcessSetProblemWeight();
  bool ProcessSetProblemDeadline();
  bool ProcessSlidesOrHomeworkFromSource();
  bool ProcessSlidesSource();
  bool ProcessClonePage();
  bool ProcessModifyPage();
  bool ProcessAddUserEmails();
  bool ProcessComputationIndicator();

  bool ProcessChangePassword(const std::string& reasonForNoAuthentication);
  bool ProcessActivateAccount();
  bool ProcessScores();
  bool ProcessApp(bool appendBuildHash);
  bool ProcessCalculatorOnePageJS(bool appendBuildHash);
  bool ProcessTopicListJSON();
  bool ProcessScoresInCoursePage();
  bool ProcessAssignTeacherToSection();
  bool ProcessExamPageJSON();
  bool ProcessTemplateJSON();
  bool ProcessLoginUserInfo(const std::string &comments);
  bool ProcessSelectCourseJSON();
  bool ProcessExamPageInterpreter();
  bool ProcessLogout();
  bool ProcessSignUP();
  bool ProcessForgotLogin();
  bool ProcessEditPageJSON();

  void reset(WebWorker& inputOwner);
  bool ServeResponseFalseIfUnrecognized(
    std::stringstream& argumentProcessingFailureComments,
    std::stringstream& comments
  );
  static std::string GetHtmlTagWithManifest();
  static std::string ModifyProblemReport();
  static JSData ClonePageResult();
  static std::string AddTeachersSections();
  static std::string AddUserEmails(const std::string& hostWebAddressWithPort);
  static JSData SubmitAnswersJSON(
    const std::string& inputRandomSeed,
    bool* outputIsCorrect,
    bool timeSafetyBrake
  );
  static JSData SubmitAnswersJSON();
  static JSData GetProblemSolutionJSON();
  static JSData GetEditPageJSON();
  static std::string GetJavascriptCaptcha();
  static std::string GetCaptchaDiv();
  static JSData GetExamPageJSON();
  static std::string GetPageFromTemplate();
  static std::string GetJSONFromTemplate();
  static JSData GetJSONUserInfo(const std::string& comments);
  static void GetJSDataUserInfo(JSData& outputAppend, const std::string& comments);
  static JSData GetTopicTableJSON();
  static JSData GetAnswerOnGiveUp();
  static JSData GetAnswerOnGiveUp(
    const std::string& inputRandomSeed,
    std::string* outputNakedAnswer = nullptr,
    bool* answerGenerationSuccess = nullptr,
    bool doIncludeTimeStats = true
  );
  static JSData GetSelectCourseJSON();
  static std::string GetScoresPage();
  static std::string GetApp(bool appendBuildHash);
  static std::string GetOnePageJS(bool appendBuildHash);
  static std::string GetScoresInCoursePage();
  static JSData submitAnswersPreviewJSON();
  static std::string SetProblemDeadline();
  static std::string SetProblemWeight();
  static std::string GetAccountsPage(const std::string& hostWebAddressWithPort);
  static JSData GetAccountsPageJSON(const std::string& hostWebAddressWithPort);
  static std::string GetAccountsPageBody(const std::string& hostWebAddressWithPort);
  static std::string ToStringAssignSection();
  static std::string ToStringUserDetails(
    bool adminsOnly,
    List<JSData>& theUsers,
    const std::string& hostWebAddressWithPort
  );
  static std::string ToStringUserScores();
  static std::string ToStringUserDetailsTable(
    bool adminsOnly,
    List<JSData>& theUsers,
    const std::string& hostWebAddressWithPort
  );
  static std::string GetSanitizedComment(
    const Expression& input, FormatExpressions& theFormat, bool& resultIsPlot
  );
  static std::string GetCommentsInterpretation(
    Calculator& theInterpreterWithAdvice,
    int indexShift,
    FormatExpressions& theFormat
  );
};

class HtmlRoutines {
public:
public:
  static MapList<std::string, std::string, MathRoutines::HashString> preLoadedFiles;
  static std::stringstream outputStream;
  static int GlobalMathSpanID;
  static int GlobalCanvasID;
  static int GlobalGeneralPurposeID;
  static int numLinesAll;
  static int numRegularLines;
  static int numDashedLines;
  static int numDottedLines;
  static int shiftX;
  static int shiftY;
  static int scale;
  static void LoadStrings();
  static std::string gitRepository;

  static std::string URLKeyValuePairsToNormalRecursiveHtml(
    const std::string& input, int recursionDepth = 0
  );

  static void ConvertURLStringToNormal(const std::string& input, std::string& output, bool replacePlusBySpace);
  static std::string ConvertURLStringToNormal(const std::string& input, bool replacePlusBySpace);
  static bool ConvertStringToHtmlStringReturnTrueIfModified(
    const std::string& input, std::string& output, bool doReplaceNewLineByBr
  );
  static std::string ConvertStringToURLStringExceptDashesAndSlashes(const std::string& input);
  static std::string ConvertStringToURLString(const std::string& input, bool usePlusesForSpacebars);
  static std::string ConvertStringToHtmlString(const std::string& theString, bool doReplaceNewLineByBr);
  static std::string ConvertStringToHtmlStringRestrictSize(
    const std::string& theString, bool doReplaceNewLineByBr, int maxStringSize
  );
  static std::string ConvertStringEscapeQuotesAndBackslashes(const std::string& input);
  static std::string ConvertStringEscapeNewLinesQuotesBackslashes(const std::string& input);

  static bool URLStringToNormalOneStep(std::string& readAhead, std::stringstream& out, bool replacePlusBySpace);
  static void ReplaceEqualitiesAndAmpersandsBySpaces(std::string& inputOutput);
  static void MakeSureWeylGroupIsSane(char& theWeylLetter, int& theRank);
  static std::string GetCalculatorComputationURL(const std::string& inputNoEncoding);
  static std::string GetCalculatorComputationAnchor(const std::string& inputNoEncoding);
  static std::string GetSliderSpanStartsHidden(
    const std::string& content,
    const std::string& label = "Expand/collapse",
    const std::string& desiredID = ""
  );
  static std::string GetHtmlLinkToGithubRepo(const std::string& displayString);
  static std::string GetHtmlLinkFromProjectFileName(
    const std::string& fileName, const std::string& fileDesc = "", int line = - 1
  );
  static std::string DoubleBackslashes(const std::string& input);

  static std::string GetCSSLink(const std::string& fileNameVirtual);
  static std::string GetJavascriptLink(const std::string& fileNameVirtual);

  static const std::string& GetCSSAddStyleTags(const std::string& fileNameVirtual);
  static const std::string& GetJavascriptAddScriptTags(const std::string& fileNameVirtual);
  static const std::string& GetFile(
    const std::string& fileNameVirtual,
    const std::string& additionalBeginTag = "",
    const std::string& additionalEndTag = ""
  );

  static const std::string& GetMathQuillStyleSheeTWithTags();
  static const std::string GetMathQuillStyleSheetLink();

  static const std::string& GetJavascriptAceEditorScriptWithTags();

  static const std::string GetCSSLinkCalculator(const std::string& relativeTo);
  static const std::string GetCSSLinkLieAlgebras(const std::string& relativeTo);
  static const std::string GetCSSLinkLieAlgebrasAndCalculator(const std::string& relativeTo);

  static const std::string GetJavascriptLinkGraphicsNDimensionsWithPanels(const std::string& relativeTo);
  static const std::string GetJavascriptLinkGraphicsNDimensions(const std::string& relativeTo);
  static const std::string GetJavascriptLinkPanels(const std::string& relativeTo);

  static std::string GetJavascriptMathjax(const std::string& baseFolder);
  static const std::string& GetJavascriptBrowserifier();

  static const std::string& GetJavascriptMathQuillDefaulTWithTags();
  static const std::string GetJavascriptMathQuillDefaultLink();
  static const std::string& GetJavascriptMathQuillMatrixSupporTWithTags();
  static const std::string GetJavascriptMathQuillMatrixSupportLink();

  static std::string GetJavascriptMathQuillDefaultFull();
  static std::string GetJavascriptMathQuillMatrixSupportFull();
  static std::string GetJavascriptVariable(const std::string& theVar);

  static std::string GetMathSpanPure(const std::string& input, int upperNumChars = 10000);
  static std::string GetMathSpanBeginArrayL(const std::string& input, int upperNumChars = 10000);
  static std::string GetMathMouseHover(const std::string& input, int upperNumChars = 10000);
  static std::string GetMathMouseHoverBeginArrayL(const std::string& input, int upperNumChars = 10000);
  static std::string GetStyleButtonLikeHtml();
  static std::string GetHtmlButton(
    const std::string& buttonID,
    const std::string& theScript,
    const std::string& buttonText
  );
  static std::string GetHtmlSpanHidableStartsHiddeN(
    const std::string& input,
    const std::string& labelExpandButton = "info expand/collapse",
    const std::string& desiredId = ""
  );
  static std::string clearNewLines(const std::string& theString);
  static std::string backslashQuotes(const std::string& theString);
  static std::string clearSlashes(const std::string& theString);
  static std::string CleanUpForLaTeXLabelUse(const std::string& inputString);
  static bool IsRepresentedByItselfInURLs(char input);
  static void clearDollarSigns(std::string& theString, std::string& output);
  static void subEqualitiesWithSimeq(std::string& theString, std::string& output);
  static bool AccountOneInputCGIString(
    const std::string& fieldName,
    const std::string& fieldValue,
    MapList<std::string, std::string, MathRoutines::HashString>& outputMap,
    std::stringstream& commentsOnFailure
  );
  static bool ChopCGIString(
    const std::string& input,
    MapList<std::string, std::string, MathRoutines::HashString>& outputMap,
    std::stringstream& commentsOnFailure
  );
  static bool ChopCGIStringAppend(
    const std::string& input,
    MapList<std::string, std::string, MathRoutines::HashString>& outputMap,
    std::stringstream& commentsOnFailure
  );
  static void ElementToStringTooltip(
    const std::string& input, const std::string& inputTooltip, std::string& output, bool useHtml
  );
  static std::string ElementToStringTooltip(
    const std::string& input, const std::string& inputTooltip, bool useHtml
  ) {
    std::string result;
    HtmlRoutines::ElementToStringTooltip(input, inputTooltip, result, useHtml);
    return result;
  }
  static std::string ElementToStringTooltip(const std::string& input, const std::string& inputTooltip) {
    return HtmlRoutines::ElementToStringTooltip(input, inputTooltip, true);
  }
  static uint32_t RedGreenBlue(unsigned int r, unsigned int g, unsigned int b);
  static void MakeStdCoutReport(const std::string& input);
  static void MakeReportIndicatorFile(const std::string& input);
  static std::string ToHtmlTable(List<std::string>& labels, List<List<std::string> >& content, bool nestTables);
  static std::string ToHtmlTableRowsFromStringContainingJSON(const std::string& theJSON);
  static std::string ToHtmlTableRowsFromJSON(const JSData& input);
};
#endif
