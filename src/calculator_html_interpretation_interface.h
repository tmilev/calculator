// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
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
  static JSData ClonePageResult();
  static std::string AddTeachersSections();
  static std::string AddUserEmails(const std::string& hostWebAddressWithPort);
  static JSData SubmitAnswersJSON(const std::string& inputRandomSeed, bool* outputIsCorrect, bool timeSafetyBrake);
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
    const std::string& inputRandomSeed, std::string* outputNakedAnswer = nullptr, bool* outputDidSucceed = nullptr
  );
  static JSData GetSelectCourseJSON();
  static std::string GetScoresPage();
  static std::string GetApp(bool appendBuildHash);
  static std::string GetOnePageJS(bool appendBuildHash);
  std::string GetOnePageJSBrowserify();
  static std::string GetScoresInCoursePage();
  static JSData submitAnswersPreviewJSON();
  static std::string GetSetProblemDatabaseInfoHtml();
  static std::string GetAccountsPage(const std::string& hostWebAddressWithPort);
  static JSData GetAccountsPageJSON(const std::string& hostWebAddressWithPort);
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
};


#endif
