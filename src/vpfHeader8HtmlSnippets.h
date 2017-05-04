//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderHtmlSnippetsAlreadyDefined
#define vpfHeaderHtmlSnippetsAlreadyDefined
#include "vpfMacros.h"
#include "vpfHeader1General6Maps.h"
#include "vpfHeader1General1_ListReferences.h"

static ProjectInformationInstance ProjectInfoHeaderHtmlSnippets(__FILE__, "Html snippets header.");

class HtmlRoutines
{
public:
public:
  static std::string StyleSheetMathQuillWithTags;
  static std::string StyleSheetCalculatorWithTags;
  static std::string JavascriptAutoCompleteWithTags;
  static std::string JavascriptMathjax;
  static std::string JavascriptMathQuillDefault;
  static std::string JavascriptMathQuillMatrixSupport;
  static std::string JavascriptSha1;
  static std::string JavascriptInitializeButtons;
  static std::string JavascriptAceEditorScript;
  static std::string JavascriptCalculatorPage;
  static std::string JavascriptAccountManagement;
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
  static void outputLineJavaScriptSpecific(const std::string& lineTypeName, int theDimension, std::string& stringColor, int& lineCounter);
  static void PrepareOutputLineJavaScriptSpecific(const std::string& lineTypeName, int numberLines);
  static std::string URLKeyValuePairsToNormalRecursiveHtml(const std::string& input, int recursionDepth=0);

  static void ConvertURLStringToNormal(const std::string& input, std::string& output, bool replacePlusBySpace);
  static std::string ConvertURLStringToNormal(const std::string& input, bool replacePlusBySpace);
  static bool ConvertStringToHtmlStringReturnTrueIfModified(const std::string& input, std::string& output, bool doReplaceNewLineByBr);
  static std::string ConvertStringToURLString(const std::string& input, bool usePlusesForSpacebars);
  static std::string ConvertStringToHtmlString(const std::string& theString, bool doReplaceNewLineByBr);
  static std::string ConvertStringToBackslashEscapedString(const std::string& input);

  static bool URLStringToNormalOneStep(std::string& readAhead, std::stringstream& out, bool replacePlusBySpace);
  static void ReplaceEqualitiesAndAmpersandsBySpaces(std::string& inputOutput);
  static void MakeSureWeylGroupIsSane(char& theWeylLetter, int& theRank);
  static std::string GetCalculatorLink(const std::string& DisplayNameCalculator, const std::string& input);
  static std::string GetSliderSpanStartsHidden(const std::string& content, const std::string& label="Expand/collapse", const std::string& desiredID="");
  static std::string GetHtmlLinkFromProjectFileName
  (const std::string& fileName, const std::string& fileDesc="", int line=-1);
  static std::string GetLatexEmbeddableLinkFromCalculatorInput(const std::string& address, const std::string& display);
  static std::string DoubleBackslashes(const std::string& input);

  static std::string& GetMathQuillStyleSheetWithTags();
  static std::string& GetJavascriptInitializeButtons();
  static std::string& GetJavascriptAceEditorScript();
  static std::string& GetJavascriptCalculatorPage();
  static std::string& GetJavascriptAccountManagementWithTags();
  static std::string& GetCalculatorStyleSheetWithTags();
  static std::string& GetJavascriptAutocompleteWithTags();
  static std::string& GetJavascriptSha1();
  static std::string& GetJavascriptMathjax();
  static std::string& GetJavascriptMathQuillDefault();
  static std::string& GetJavascriptMathQuillMatrixSupport();

  static std::string GetJavascriptStandardCookies();
  static std::string GetJavascriptHideHtml();
  static std::string GetJavascriptSubmitEmails();
  static std::string GetJavascriptCookieFunctions();
  static std::string GetJavascriptSubmitMainInputIncludeCurrentFile();
  static std::string GetJavascriptSubmitURLString();
  static std::string GetJavascriptCanvasGraphicsWithTags();
  static std::string GetJavascriptTopicListWithTags();
  static std::string GetDatePickerJavascriptInit();
  static std::string GetJavascriptVariable(const std::string& theVar);


  static std::string GetMathSpanPure(const std::string& input, int upperNumChars=10000);
  static std::string GetMathSpanBeginArrayL(const std::string& input, int upperNumChars=10000);
  static std::string GetMathMouseHover(const std::string& input, int upperNumChars=10000);
  static std::string GetMathMouseHoverBeginArrayL(const std::string& input, int upperNumChars=10000);
  static std::string GetStyleButtonLikeHtml();
  static std::string GetHtmlButton(const std::string& buttonID, const std::string& theScript, const std::string& buttonText);
  static std::string GetHtmlSpanHidableStartsHiddeN
  (const std::string& input, const std::string& labelExpandButton= "info expand/collapse", const std::string& desiredId="");
  static std::string clearNewLines(const std::string& theString);
  static std::string backslashQuotes(const std::string& theString);
  static std::string clearSlashes(const std::string& theString);
  static std::string CleanUpForFileNameUse(const std::string& inputString);
  static std::string CleanUpForLaTeXLabelUse(const std::string& inputString);
  static std::string GetCalculatorLinkUnclosedPostArguments(const std::string& inputArguments);
  static std::string GetCalculatorLinkUnclosedPostArguments
  (const std::string& DisplayNameCalculator, const std::string& inputArguments);
  static bool IsRepresentedByItselfInURLs(char input);
  static void clearDollarSigns(std::string& theString, std::string& output);
  static void subEqualitiesWithSimeq(std::string& theString, std::string& output);
  static bool AccountOneInputCGIString
(const std::string& fieldName, const std::string& fieldValue, MapLisT<std::string, std::string, MathRoutines::hashString>& outputMap,
 std::stringstream& commentsOnFailure)
  ;
  static bool ChopCGIString
(const std::string& input, MapLisT<std::string, std::string, MathRoutines::hashString>& outputMap,
 std::stringstream& commentsOnFailure)
  ;
  static bool ChopCGIStringAppend
(const std::string& input, MapLisT<std::string, std::string, MathRoutines::hashString>& outputMap,
 std::stringstream& commentsOnFailure)
  ;
  static void ElementToStringTooltip(const std::string& input, const std::string& inputTooltip, std::string& output, bool useHtml);
  static std::string ElementToStringTooltip(const std::string& input, const std::string& inputTooltip, bool useHtml){ std::string result; HtmlRoutines::ElementToStringTooltip(input, inputTooltip, result, useHtml); return result; }
  static std::string ElementToStringTooltip(const std::string& input, const std::string& inputTooltip){ return HtmlRoutines::ElementToStringTooltip(input, inputTooltip, true); }
  static uint32_t RedGreenBlue(unsigned int r, unsigned int g, unsigned int b);
  static void MakeStdCoutReport(const std::string& input);
  static void MakeReportIndicatorFile(const std::string& input);
  static void FormatCPPSourceCode(const std::string& FileName);
};
#endif
