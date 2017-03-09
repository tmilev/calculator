//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderHtmlInterpretationInterfaceAlreadyDefined
#define vpfHeaderHtmlInterpretationInterfaceAlreadyDefined
#include "vpfMacros.h"
#include <ctime>


static ProjectInformationInstance ProjectInfoHeaderHtmlInterpretationInterface(__FILE__, "Html interpretation interface.");

class HtmlInterpretation{
public:
  static std::string GetHtmlTagWithManifest();
  static std::string ModifyProblemReport();
  static std::string ClonePageResult();
  static std::string AddTeachersSections();
  static std::string AddUserEmails(const std::string& hostWebAddressWithPort);
  static std::string SubmitProblem
  (const std::string& inputRandomSeed, bool* outputIsCorrect, bool timeSafetyBrake);
  static std::string SubmitProblem();
  static std::string GetProblemSolution();
  static std::string GetEditPageHTML();
  static std::string GetExamPage();
  static std::string GetPageFromTemplate();
  static std::string GetTopicTable();
  static std::string GetExamPageInterpreter();
  static std::string GetAnswerOnGiveUp();
  static std::string GetAnswerOnGiveUp
  (const std::string& inputRandomSeed, std::string* outputNakedAnswer=0, bool* outputDidSucceed=0);
  static std::string GetSelectCourse();
  static std::string GetScoresPage();
  static std::string GetScoresInCoursePage();
  static std::string GetNavigationPanelWithGenerationTime();
  static std::string SubmitProblemPreview();
  static std::string GetSetProblemDatabaseInfoHtml();
  static std::string GetAccountsPage(const std::string& hostWebAddressWithPort);
  static std::string GetAccountsPageBody(const std::string& hostWebAddressWithPort);
  static std::string ToStringAssignSection();
  static std::string ToStringUserDetails
  (bool adminsOnly, List<List<std::string> >& userTable, List<std::string>& columnLabels,
   const std::string& hostWebAddressWithPort);
  static std::string ToStringUserScores();
  static std::string ToStringUserDetailsTable
  (bool adminsOnly, List<List<std::string> >& userTable, List<std::string>& columnLabels,
   const std::string& hostWebAddressWithPort);
  static std::string ToStringCalculatorArgumentsHumanReadable();
  static std::string GetSanitizedComment
  (const Expression& input,
  FormatExpressions& theFormat);
  static std::string GetCommentsInterpretation
  (Calculator& theInterpreterWithAdvice,
   int indexShift, FormatExpressions& theFormat)

  ;
  static std::string ToStringNavigation();
};


#endif
