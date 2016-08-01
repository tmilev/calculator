//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderHtmlSnippetsAlreadyDefined
#define vpfHeaderHtmlSnippetsAlreadyDefined
#include "vpfMacros.h"
#include <ctime>


static ProjectInformationInstance ProjectInfoHeaderHtmlSnippets(__FILE__, "Html snippets header.");

class HtmlSnippets{
public:
static std::string GetJavascriptStandardCookies();
static std::string GetJavascriptHideHtml();
static std::string GetJavascriptSubmitEmailS(const std::string& homeFile);
static std::string GetJavascriptSubmitEmailsAce(const std::string& homeFile);
static std::string GetJavascriptCookieFunctions();

};


#endif
