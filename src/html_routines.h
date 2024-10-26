#ifndef header_html_routines_ALREADY_INCLUDED
#define header_html_routines_ALREADY_INCLUDED

#include "general_maps.h"

class JSData;

class HtmlRoutines {
private:
  static std::string getCalculatorComputationAnchor(
    const std::string& urlApplication,
    const std::string& inputNoEncoding,
    const std::string& desiredAnchorTextEmptyForDefault
  );
  static std::string getMathSpan(
    const std::string& input, bool useDisplayStyle = false
  );
public:
  static MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  >& preLoadedFiles();
  static std::stringstream outputStream;
  static int globalGeneralPurposeID;
  static void loadStrings();
  static std::string gitRepository;
  static std::string latexWithCopyButton;
  static std::string URLKeyValuePairsToNormalRecursiveHtml(
    const std::string& input, int recursionDepth = 0
  );
  // Returns a no-display span with name="script" with the given content.
  static std::string jsonContainer(
    const std::string& scriptType, const JSData& scriptContent
  );
  static void convertURLStringToNormal(
    const std::string& input,
    std::string& output,
    bool replacePlusBySpace
  );
  static std::string convertURLStringToNormal(
    const std::string& input, bool replacePlusBySpace
  );
  static bool convertStringToHtmlStringReturnTrueIfModified(
    const std::string& input,
    std::string& output,
    bool doReplaceNewLineByBr
  );
  static std::string convertStringToURLStringExceptDashesAndSlashes(
    const std::string& input
  );
  static std::string convertStringToURLString(
    const std::string& input, bool usePlusesForSpacebars
  );
  static std::string convertStringToHtmlString(
    const std::string& inputString, bool doReplaceNewLineByBr
  );
  static std::string convertStringToHtmlStringRestrictSize(
    const std::string& inputString,
    bool doReplaceNewLineByBr,
    int maxStringSize
  );
  static std::string convertStringEscapeQuotesAndBackslashes(
    const std::string& input
  );
  static std::string convertStringEscapeNewLinesQuotesBackslashes(
    const std::string& input
  );
  static bool URLStringToNormalOneStep(
    std::string& readAhead,
    std::stringstream& out,
    bool replacePlusBySpace
  );
  static void replaceEqualitiesAndAmpersandsBySpaces(
    std::string& inputOutput
  );
  // Returns an url with a calculator computation
  static std::string getCalculatorComputationURL(
    const std::string& inputNoEncoding
  );
  static std::string getProblemURLRelative(const std::string& problemName);
  static std::string getCalculatorComputationAnchorSamePage(
    const std::string& inputNoEncoding
  );
  static std::string getCalculatorComputationAnchorSameURL(
    const std::string& inputNoEncoding,
    const std::string& desiredAnchorTextEmptyForDefault
  );
  static std::string getCalculatorComputationAnchorThisServer(
    const std::string& inputNoEncoding,
    const std::string& desiredAnchorTextEmptyForDefault,
    bool useAppWithoutCache = false
  );
  static std::string getCalculatorComputationAnchorRelativeAppAddress(
    const std::string& inputNoEncoding,
    const std::string& desiredAnchorTextEmptyForDefault
  );
  static std::string getHtmlLinkToGithubRepository(
    const std::string& displayString
  );
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
  static std::string getMathNoDisplay(const std::string& input);
  static std::string getStyleButtonLikeHtml();
  static std::string clearNewLines(const std::string& inputString);
  static std::string backslashQuotes(const std::string& inputString);
  static std::string clearSlashes(const std::string& inputString);
  static std::string cleanUpForLaTeXLabelUse(const std::string& inputString);
  static bool isRepresentedByItselfInURLs(char input);
  static void clearDollarSigns(std::string& input, std::string& output);
  static void subEqualitiesWithSimeq(
    std::string& input, std::string& output
  );
  static bool accountOneInputPercentEncodedString(
    const std::string& fieldName,
    const std::string& fieldValue,
    MapList<
      std::string,
      std::string,
      HashFunctions::hashFunction<std::string>
    >& outputMap,
    std::stringstream& commentsOnFailure
  );
  static bool chopPercentEncodedString(
    const std::string& input,
    MapList<
      std::string,
      std::string,
      HashFunctions::hashFunction<std::string>
    >& outputMap,
    std::stringstream& commentsOnFailure
  );
  static bool chopPercentEncodedStringAppend(
    const std::string& input,
    MapList<
      std::string,
      std::string,
      HashFunctions::hashFunction<std::string>
    >& outputMap,
    std::stringstream& commentsOnFailure
  );
  static void elementToStringTooltip(
    const std::string& input,
    const std::string& inputTooltip,
    std::string& output,
    bool useHtml
  );
  static std::string elementToStringTooltip(
    const std::string& input,
    const std::string& inputTooltip,
    bool useHtml
  ) {
    std::string result;
    HtmlRoutines::elementToStringTooltip(
      input, inputTooltip, result, useHtml
    );
    return result;
  }
  static std::string elementToStringTooltip(
    const std::string& input, const std::string& inputTooltip
  ) {
    return HtmlRoutines::elementToStringTooltip(input, inputTooltip, true);
  }
  static uint32_t redGreenBlue(
    unsigned int r, unsigned int g, unsigned int b
  );
  static void makeStdCoutReport(const std::string& input);
  static void makeReportIndicatorFile(const std::string& input);
  static std::string toHtmlTable(
    List<std::string>& labels,
    List<List<std::string> >& content,
    bool nestTables
  );
  static std::string toHtmlTableRowsFromStringContainingJSON(
    const std::string& inputJSON
  );
  static std::string toHtmlTableRowsFromJSON(const JSData& input);
  static std::string toHtmlLatexLiteralWithCopy(
    const std::string& inputLatex
  );
};

#endif
