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
  static MapLisT<std::string, std::string, MathRoutines::hashString> preLoadedFiles;
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
  static std::string ConvertStringToURLStringExceptDashesAndSlashes(const std::string& input);
  static std::string ConvertStringToURLString(const std::string& input, bool usePlusesForSpacebars);
  static std::string ConvertStringToHtmlString(const std::string& theString, bool doReplaceNewLineByBr);
  static std::string ConvertStringToHtmlStringRestrictSize
  (const std::string& theString, bool doReplaceNewLineByBr, int maxStringSize);
  static std::string ConvertStringEscapeQuotesAndBackslashes(const std::string& input);
  static std::string ConvertStringEscapeNewLinesQuotesBackslashes(const std::string& input);

  static bool URLStringToNormalOneStep(std::string& readAhead, std::stringstream& out, bool replacePlusBySpace);
  static void ReplaceEqualitiesAndAmpersandsBySpaces(std::string& inputOutput);
  static void MakeSureWeylGroupIsSane(char& theWeylLetter, int& theRank);
  static std::string GetCalculatorLink(const std::string& DisplayNameCalculator, const std::string& input);
  static std::string GetSliderSpanStartsHidden(const std::string& content, const std::string& label="Expand/collapse", const std::string& desiredID="");
  static std::string GetHtmlLinkFromProjectFileName
  (const std::string& fileName, const std::string& fileDesc="", int line=-1);
  static std::string GetLatexEmbeddableLinkFromCalculatorInput(const std::string& address, const std::string& display);
  static std::string DoubleBackslashes(const std::string& input);

  static std::string GetCSSLink(const std::string& fileNameVirtual);
  static std::string GetJavascriptLink(const std::string& fileNameVirtual);

  static const std::string& GetCSSAddStyleTags(const std::string& fileNameVirtual);
  static const std::string& GetJavascriptAddScriptTags(const std::string& fileNameVirtual);
  static const std::string& GetFile
  (const std::string& fileNameVirtual, const std::string& additionalBeginTag="",
   const std::string& additionalEndTag="");

  static const std::string& GetJavascriptCookieFunctionSNoTags();
  static const std::string GetJavascriptCookieFunctionsLink();
  static const std::string& GetMathQuillStyleSheeTWithTags();
  static const std::string GetMathQuillStyleSheetLink();

  static const std::string& GetJavascriptAceEditorScriptWithTags();

  static const std::string& GetJavascriptCalculatorPageWithTags();
  static const std::string GetJavascriptCalculatorPageLink();

  static const std::string& GetJavascriptAccountManagemenTWithTags();
  static const std::string GetJavascriptAccountManagementLink();

  static const std::string& GetStyleSheetCalculatorWithTags();
  static const std::string GetCSSLinkCalculator();

  static const std::string& GetJavascriptInitializeButtonSWithTags();
  static const std::string GetJavascriptInitializeButtonsLink();
  static const std::string& GetJavascriptAutocompletEWithTags();
  static const std::string GetJavascriptAutocompleteLink();
  static const std::string& GetJavascriptSha1();
  static const std::string& GetJavascriptMathjax();
  static const std::string& GetJavascriptMathQuillDefaulTWithTags();
  static const std::string GetJavascriptMathQuillDefaultLink();
  static const std::string& GetJavascriptMathQuillMatrixSupporTWithTags();
  static const std::string GetJavascriptMathQuillMatrixSupportLink();
  static const std::string& GetJavascriptProblemLinkSWithTags();

  static const std::string GetJavascriptForgotLogin();
  static const std::string GetJavascriptProblemLinksLink();
  static const std::string& GetJavascriptDatabaseRoutineSWithTags();
  static const std::string GetJavascriptDatabaseRoutinesLink();
  static const std::string& GetJavascriptCanvasGraphicSWithTags();
  static const std::string GetJavascriptCanvasGraphicsLink();
  static const std::string& GetJavascriptTopicLisTWithTags();
  static const std::string GetJavascriptTopicListLink();

  static std::string GetJavascriptMathQuillDefaultFull();
  static std::string GetJavascriptMathQuillMatrixSupportFull();
  static std::string GetJavascriptStandardCookiesWithTags();
  static std::string GetJavascriptHideHtmlWithTags();
  static std::string GetJavascriptSubmitEmails();
  static std::string GetJavascriptSubmitMainInputIncludeCurrentFile();
  static std::string GetJavascriptSubmitURLString();
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
