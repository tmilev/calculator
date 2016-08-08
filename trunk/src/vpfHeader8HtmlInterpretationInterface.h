//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderHtmlInterpretationInterfaceAlreadyDefined
#define vpfHeaderHtmlInterpretationInterfaceAlreadyDefined
#include "vpfMacros.h"
#include <ctime>


static ProjectInformationInstance ProjectInfoHeaderHtmlInterpretationInterface(__FILE__, "Html interpretation interface.");

class HtmlInterpretation{
public:

  static std::string ModifyProblemReport();
  static std::string ClonePageResult();
  static std::string AddUserEmails(const std::string& hostWebAddressWithPort);
  static std::string SubmitProblem();
  static std::string GetProblemSolution();
  static std::string GetEditPageHTML();
  static std::string GetExamPage();
  static std::string GetExamPageInterpreter();
  static std::string GetAnswerOnGiveUp();
  static std::string SubmitProblemPreview();
  static std::string GetAccountsPage(const std::string& hostWebAddressWithPort);
  static std::string ToStringUserDetails
  (bool adminsOnly, List<List<std::string> > userTable, List<std::string> columnLabels,
   const std::string& hostWebAddressWithPort);
  static std::string ToStringUserDetailsTable
  (bool adminsOnly, List<List<std::string> > userTable, List<std::string> columnLabels,
   const std::string& hostWebAddressWithPort);
  static std::string ToStringCalculatorArgumentsHumanReadable();
};


#endif
