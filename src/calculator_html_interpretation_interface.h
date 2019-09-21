//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#ifndef vpfHeaderHtmlInterpretationInterfaceAlreadyDefined
#define vpfHeaderHtmlInterpretationInterfaceAlreadyDefined
#include "macros.h"
#include "general_lists.h"
#include <ctime>

class JSData;

static ProjectInformationInstance ProjectInfoHeaderHtmlInterpretationInterface(__FILE__, "Html interpretation interface.");

class HtmlInterpretation {
public:
  std::string htmlRaw;
  std::string htmlJSbuild;
  List<std::string> currentWords;
  // List<bool> charIsSplitting;
  List<std::string> jsFileNames;
  List<std::string> jsFileContents;

  void BuildHtmlJSpage(bool appendBuildHash);
  static std::string GetHtmlTagWithManifest();
  static std::string ModifyProblemReport();
  static std::string ClonePageResult();
  static std::string AddTeachersSections();
  static std::string AddUserEmails(const std::string& hostWebAddressWithPort);
  static JSData SubmitAnswersJSON(const std::string& inputRandomSeed, bool* outputIsCorrect, bool timeSafetyBrake);
  static std::string SubmitAnswersString(const std::string& inputRandomSeed, bool* outputIsCorrect, bool timeSafetyBrake);
  static std::string SubmitAnswersString();
  static std::string GetProblemSolutionString();
  static JSData GetProblemSolutionJSON();
  static std::string GetEditPageJSON();
  static std::string GetBrowseProblems();
  static std::string GetJavascriptCaptcha();
  static std::string GetCaptchaDiv();
  static std::string GetExamPageJSON();
  static std::string GetPageFromTemplate();
  static std::string GetJSONFromTemplate();
  static std::string GetJSONUserInfo(const std::string &comments);
  static void GetJSDataUserInfo(JSData& outputAppend, const std::string &comments);
  static std::string GetTopicTableJSON();
  static std::string GetAnswerOnGiveUp();
  static std::string GetAnswerOnGiveUp(
    const std::string& inputRandomSeed, std::string* outputNakedAnswer = nullptr, bool* outputDidSucceed = nullptr
  );
  static std::string GetSelectCourseJSON();
  static std::string GetScoresPage();
  static std::string GetAboutPage();
  static std::string GetApp(bool appendBuildHash);
  static std::string GetOnePageJS(bool appendBuildHash);
  std::string GetOnePageJSBrowserify();
  static std::string GetScoresInCoursePage();
  static std::string GetNavigationPanelWithGenerationTime();
  static JSData submitAnswersPreviewJSON();
  static std::string submitAnswersPreviewString();
  static std::string GetSetProblemDatabaseInfoHtml();
  static std::string GetAccountsPage(const std::string& hostWebAddressWithPort);
  static std::string GetAccountsPageJSON(const std::string& hostWebAddressWithPort);
  static std::string GetAccountsPageBody(const std::string& hostWebAddressWithPort);
  static std::string ToStringAssignSection();
  static std::string ToStringUserDetails(
    bool adminsOnly, List<JSData>& theUsers, const std::string& hostWebAddressWithPort
  );
  static std::string ToStringUserScores();
  static std::string ToStringUserDetailsTable(
    bool adminsOnly, List<JSData>& theUsers, const std::string& hostWebAddressWithPort
  );
  static std::string ToStringCalculatorArgumentsHumanReadable();
  static std::string GetSanitizedComment(
    const Expression& input, FormatExpressions& theFormat, bool& resultIsPlot
  );
  static std::string GetCommentsInterpretation(
    Calculator& theInterpreterWithAdvice, int indexShift, FormatExpressions& theFormat
  );
  static std::string ToStringNavigation();
};


#endif
