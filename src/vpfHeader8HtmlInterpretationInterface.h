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
static std::string AddUserEmails();
static std::string ProcessSubmitProblem();
static std::string GetProblemSolution();
static std::string GetEditPageHTML();
static std::string GetExamPage();
static std::string GetExamPageInterpreter();
static std::string GetAnswerOnGiveUp();
static std::string SubmitProblemPreview();
};


#endif
