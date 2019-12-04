// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_html_interpretation.h"
#include "html_snippets.h"
#include "calculator_html_interpretation_interface.h"
#include "database_mongo.h"
#include <iomanip>
#include "string_constants.h"

extern ProjectInformationInstance projectInfoInstanceHtmlInterpretationInterfaceImplementation;
ProjectInformationInstance projectInfoInstanceHtmlInterpretationInterfaceImplementation(
  __FILE__, "Routines for calculus teaching: calculator exam mode."
);

std::string HtmlInterpretation::GetProblemSolutionString() {
  JSData result;
  result = HtmlInterpretation::GetProblemSolutionJSON();
  return result.ToString(false);
}

JSData HtmlInterpretation::GetProblemSolutionJSON() {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetProblemSolution");
  int64_t startMilliseconds = theGlobalVariables.GetElapsedMilliseconds();
  CalculatorHTML theProblem;
  std::stringstream out, errorStream;
  JSData result;
  theProblem.LoadCurrentProblemItem(false, theGlobalVariables.GetWebInput("randomSeed"), &errorStream);
  if (!theProblem.flagLoadedSuccessfully) {
    out << "Problem name is: " << theProblem.fileName
    << " <b>Could not load problem, this may be a bug. "
    << CalculatorHTML::BugsGenericMessage << "</b>";
    if (errorStream.str() != "") {
      out << " Comments: " << errorStream.str();
    }
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  }
  if (theProblem.flagIsForReal) {
    out << " <b>Not allowed to show answer of a problem being tested for real. </b>";
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  }
  if (theGlobalVariables.GetWebInput("randomSeed") == "") {
    out << "<b>I could not figure out the exercise problem (missing random seed). </b>";
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  }
  std::stringstream comments;
  if (!theProblem.ParseHTMLPrepareCommands(&comments)) {
    out << "<br><b>Failed to parse problem.</b> Comments: " << comments.str();
    result[WebAPI::result::resultHtml] = out.str();
    return result;

  }
  std::string lastStudentAnswerID;
  MapList<std::string, std::string, MathRoutines::HashString>& theArgs = theGlobalVariables.webArguments;
  for (int i = 0; i < theArgs.size(); i ++) {
    StringRoutines::StringBeginsWith(theArgs.theKeys[i], "calculatorAnswer", &lastStudentAnswerID);
  }
  int indexLastAnswerId = theProblem.GetAnswerIndex(lastStudentAnswerID);
  if (indexLastAnswerId == - 1) {
    out << "<b>Student submitted answerID: " << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "</b>";
    if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights()) {
      out << "<hr>" << theProblem.theProblemData.ToStringAvailableAnswerIds();
      //out << "<hr>Client input: " << this->mainArgumentRAW << "<hr>";
      out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
    }
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = theGlobalVariables.GetElapsedMilliseconds() - startMilliseconds;
    return result;
  }
  Answer& currentA = theProblem.theProblemData.theAnswers.theValues[indexLastAnswerId];
  Calculator theInterpreteR;
  theInterpreteR.initialize();
  theInterpreteR.flagPlotNoControls = true;
  theInterpreteR.flagWriteLatexPlots = false;
  if (!theProblem.PrepareCommands(&comments)) {
    out << "<b>Failed to prepare calculator commands. </b> <br>Comments:<br>" << comments.str();
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  }
  if (currentA.solutionElements.size == 0) {
    out << "<b> Unfortunately there is no solution given for this question (answerID: " << lastStudentAnswerID << ").";
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  }
  std::stringstream answerCommands, answerCommandsNoEnclosures;
  answerCommands << "CommandEnclosure{}(" << currentA.commandsBeforeAnswer << "); "
  << currentA.commandsSolutionOnly;
  answerCommandsNoEnclosures
  << currentA.commandsBeforeAnswerNoEnclosuresForDEBUGGING
  << currentA.commandsSolutionOnly;
  theInterpreteR.Evaluate(answerCommands.str());
  if (theInterpreteR.syntaxErrors != "") {
    out << "<span style =\"color:red\"><b>Failed to compose the solution. "
    << "Likely there is a bug with the problem. </b></span>"
    << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.ToStringSyntacticStackHumanReadable(false, false);
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = theGlobalVariables.GetElapsedMilliseconds() - startMilliseconds;
    return result;
  }
  if (theInterpreteR.flagAbortComputationASAP) {
    out << "<span style =\"color:red\"><b>Failed to compose the solution. "
    << "Likely there is a bug with the problem. </b></span>"
    << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.outputString
    << theInterpreteR.outputCommentsString
    << "<hr>Input: <br>" << theInterpreteR.inputString;
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = theGlobalVariables.GetElapsedMilliseconds() - startMilliseconds;
    return result;
  }
  if (!theProblem.InterpretProcessExecutedCommands(theInterpreteR, currentA.solutionElements, out)) {
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = theGlobalVariables.GetElapsedMilliseconds() - startMilliseconds;
    return result;
  }
  for (int i = 0; i < currentA.solutionElements.size; i ++) {
    if (!currentA.solutionElements[i].IsHidden()) {
      out << currentA.solutionElements[i].ToStringInterpretedBody();
    }
  }
  if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights()) {
    out << "<hr>"
    << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=calculator&mainInput="
    << HtmlRoutines::ConvertStringToURLString(answerCommandsNoEnclosures.str(), false)
    << "\">Input link</a>"
    <<  "<br>" << theInterpreteR.outputString << "<hr>" << theInterpreteR.outputCommentsString
    << "<hr>Raw input: " << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  }
  result[WebAPI::result::resultHtml] = out.str();
  result[WebAPI::result::millisecondsComputation] = theGlobalVariables.GetElapsedMilliseconds() - startMilliseconds;
  return result;
}

std::string HtmlInterpretation::GetSetProblemDatabaseInfoHtml() {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetSetProblemDatabaseInfoHtml");
  if (!theGlobalVariables.flagDatabaseCompiled) {
    return "Cannot modify problem weights (no database available)";
  }
  if (!theGlobalVariables.UserDefaultHasAdminRights()) {
    return "<b>Only admins may set problem weights.</b>";
  }
  CalculatorHTML theProblem;
  std::string inputProblemInfo = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("mainInput"), false);
  theProblem.topicListFileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("topicList"), false);
  std::stringstream commentsOnFailure;
  if (!theProblem.LoadAndParseTopicList(commentsOnFailure)) {
    return "Failed to load topic list from file name: " + theProblem.topicListFileName + ". " + commentsOnFailure.str();
  }
  theProblem.currentUseR.UserCalculatorData::operator=(theGlobalVariables.userDefault);
  std::stringstream out;
  if (!theProblem.PrepareSectionList(commentsOnFailure)) {
    out << "<span style =\"color:red\"><b>Failed to prepare section list. </b></span>" << commentsOnFailure.str();
    return out.str();
  }
  if (!theProblem.LoadProblemInfoFromJSONAppend(
    theProblem.currentUseR.problemWeights, theProblem.currentUseR.theProblemData, out
  )) {
    out << "Failed to interpret the problem weight string. ";
    return out.str();
  }
  if (!theProblem.LoadProblemInfoFromJSONAppend(
    theProblem.currentUseR.deadlines, theProblem.currentUseR.theProblemData, out
  )) {
    out << "Failed to interpret the deadline string. ";
    return out.str();
  }
  if (theProblem.MergeProblemInfoInDatabaseJSON(inputProblemInfo, commentsOnFailure)) {
    out << "<span style =\"color:green\"><b>Modified. </b></span>";
  } else {
    out << "<span style =\"color:red\"><b>" << commentsOnFailure.str() << "</b></span>";
  }
  return out.str();
}

std::string HtmlInterpretation::GetSanitizedComment(
  const Expression& input, FormatExpressions& theFormat, bool& resultIsPlot
) {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetSanitizedComment");
  theFormat.flagUseQuotes = false;
  resultIsPlot = false;
  std::string theString;
  if (input.IsOfType<std::string>(&theString)) {
    if (StringRoutines::StringBeginsWith(theString, "Approximations have been")) {
      return "";
    }
    return input.ToString(&theFormat);
  }
  if (input.IsOfType<Plot>()) {
    resultIsPlot = true;
    return input.ToString(&theFormat);
  }
  if (input.HasType<Plot>()) {
    return "";
  }
  //<- expression has a partially drawn plot-> not displaying.
  if (input.owner == nullptr) {
    return "";
  }
  if (input.StartsWith(input.owner->opRulesOff()) || input.StartsWith(input.owner->opRulesOn())) {
    return "";
  }
  return input.ToString(&theFormat);
}

std::string HtmlInterpretation::GetCommentsInterpretation(
  Calculator& theInterpreterWithAdvice, int indexShift, FormatExpressions& theFormat
) {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetCommentsInterpretation");
  std::stringstream out;
  theFormat.flagExpressionIsFinal = true;
  theFormat.flagIncludeExtraHtmlDescriptionsInPlots = false;
  theInterpreterWithAdvice.theObjectContainer.resetPlots();
  if (indexShift >= theInterpreterWithAdvice.theProgramExpression.size()) {
    stOutput << "<br>DEBUG: something is very wrong with indexshift in comments!";
    return "";
  }
  const Expression& currentE = theInterpreterWithAdvice.theProgramExpression[indexShift][1];
  bool resultIsPlot = false;
  if (!currentE.StartsWith(theInterpreterWithAdvice.opEndStatement())) {
    out << HtmlInterpretation::GetSanitizedComment(currentE, theFormat, resultIsPlot);
    return out.str();
  }
  std::string currentS;
  for (int i = 1; i < currentE.size(); i ++) {
    currentS = HtmlInterpretation::GetSanitizedComment(currentE[i], theFormat,resultIsPlot);
    if (StringRoutines::StringTrimWhiteSpace(currentS) == "") {
      continue;
    }
    out << currentS;
    if (i != currentE.size() - 1 && !resultIsPlot) {
      out << "<br>";
    }
  }
  return out.str();
}

std::string HtmlInterpretation::submitAnswersPreviewString() {
  JSData result = HtmlInterpretation::submitAnswersPreviewJSON();
  return result.ToString(false);
}

JSData HtmlInterpretation::submitAnswersPreviewJSON() {
  MacroRegisterFunctionWithName("HtmlInterpretation::submitAnswersPreviewJSON");
  double startTime = theGlobalVariables.GetElapsedSeconds();
  std::string lastStudentAnswerID;
  std::string lastAnswer;
  std::stringstream out, studentAnswerSream;
  MapList<std::string, std::string, MathRoutines::HashString>& theArgs =
  theGlobalVariables.webArguments;
  JSData result;
  for (int i = 0; i < theArgs.size(); i ++) {
    if (StringRoutines::StringBeginsWith(theArgs.theKeys[i], "calculatorAnswer", &lastStudentAnswerID)) {
      lastAnswer = "(" + HtmlRoutines::ConvertURLStringToNormal(theArgs.theValues[i], false) + "); ";
    }
  }
  studentAnswerSream << lastAnswer;
  out << "Your answer(s): \\(\\displaystyle " << lastAnswer << "\\)" << "\n<br>\n";
  CalculatorHTML theProblem;
  std::stringstream errorStream, comments;
  theProblem.LoadCurrentProblemItem(
    theGlobalVariables.UserRequestRequiresLoadingRealExamData(),
    theGlobalVariables.GetWebInput("randomSeed"),
    &errorStream
  );
  if (!theProblem.flagLoadedSuccessfully) {
    out << "<br><b>" << WebAPI::problem::failedToLoadProblem << "</b> ";
  }
  if (!theProblem.ParseHTMLPrepareCommands(&comments)) {
    out << "<br><b>Failed to parse problem.</b> Comments: " << comments.str();
  }
  int indexLastAnswerId = theProblem.GetAnswerIndex(lastStudentAnswerID);
  if (indexLastAnswerId == - 1) {
    errorStream << "<br>Student submitted answerID: " << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "<br>Response time: " << theGlobalVariables.GetElapsedSeconds() - startTime
    << " second(s). ";
    result[WebAPI::result::error] = errorStream.str();
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  }
  Answer& currentA = theProblem.theProblemData.theAnswers.theValues[indexLastAnswerId];
  if (!theProblem.PrepareCommands(&comments)) {
    errorStream << "Something went wrong while interpreting the problem file. ";
    if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights()) {
      errorStream << comments.str();
    }
    result[WebAPI::result::millisecondsComputation] = theGlobalVariables.GetElapsedSeconds() - startTime;
    result[WebAPI::result::error] = errorStream.str();
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  }
  Calculator theInterpreteR;
  theInterpreteR.flagUseLnInsteadOfLog = true;
  theInterpreteR.initialize();
  theInterpreteR.flagWriteLatexPlots = false;
  theInterpreteR.flagPlotNoControls = true;
  std::stringstream studentAnswerWithComments;
  studentAnswerWithComments
  << "CommandEnclosure{}("
  << currentA.commandsCommentsBeforeInterpretatioN
  << ");"
  << studentAnswerSream.str();

  theInterpreteR.Evaluate(studentAnswerWithComments.str());
  if (theInterpreteR.syntaxErrors != "") {
    errorStream << "<b style ='color:red'>Failed to parse your answer, got:</b><br>"
    << theInterpreteR.ToStringSyntacticStackHumanReadable(false, true);
    result[WebAPI::result::error] = errorStream.str();
    result[WebAPI::result::millisecondsComputation] = theGlobalVariables.GetElapsedSeconds() - startTime;
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  } else if (theInterpreteR.flagAbortComputationASAP) {
    out << "<b style = 'color:red'>Failed to evaluate your answer, got:</b><br>"
    << theInterpreteR.outputString;
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = theGlobalVariables.GetElapsedSeconds() - startTime;
    return result;
  }
  FormatExpressions theFormat;
  theFormat.flagUseLatex = true;
  theFormat.flagUsePmatrix = true;
  const Expression& studentAnswerNoContextE =
  theInterpreteR.theProgramExpression[theInterpreteR.theProgramExpression.size() - 1];
  out << "<span style =\"color:magenta\"><b>Interpreting as:</b></span><br>";
  out << "\\(\\displaystyle "
  << studentAnswerNoContextE.ToString(&theFormat) << "\\)";
  Calculator theInterpreterWithAdvice;
  theInterpreterWithAdvice.flagUseLnInsteadOfLog = true;
  theInterpreterWithAdvice.initialize();
  theInterpreterWithAdvice.flagWriteLatexPlots = false;
  theInterpreterWithAdvice.flagPlotNoControls = true;
  std::stringstream calculatorInputStream,
  calculatorInputStreamNoEnclosures;

  calculatorInputStream << "CommandEnclosure{}("
  << currentA.commandsBeforeAnswer << ");";
  calculatorInputStreamNoEnclosures
  << currentA.commandsBeforeAnswerNoEnclosuresForDEBUGGING;

  calculatorInputStream << "CommandEnclosure{}("
  << currentA.answerId << " = " << lastAnswer
  << ");";
  calculatorInputStreamNoEnclosures
  << currentA.answerId << " = " << lastAnswer << "";
  bool hasCommentsBeforeSubmission = (StringRoutines::StringTrimWhiteSpace(currentA.commandsCommentsBeforeSubmission) != "");
  if (hasCommentsBeforeSubmission) {
    calculatorInputStream << "CommandEnclosure{}("
    <<  currentA.commandsCommentsBeforeSubmission
    << ");";
    calculatorInputStreamNoEnclosures
    << currentA.commandsCommentsBeforeSubmission;
  }
  std::stringstream problemLinkStream;
  problemLinkStream
  << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
  << "?request=calculator&mainInput="
  << HtmlRoutines::ConvertStringToURLString(calculatorInputStreamNoEnclosures.str(), false)
  << "\">Input link</a>";
  theInterpreterWithAdvice.Evaluate(calculatorInputStream.str());
  if (theInterpreterWithAdvice.syntaxErrors != "") {
    out << "<br><span style =\"color:red\"><b>"
    << "Something went wrong when parsing your answer "
    << "in the context of the current problem. "
    << "</b></span>";
    if (theGlobalVariables.UserDefaultHasAdminRights()) {
      out
      << problemLinkStream.str()
      << theInterpreterWithAdvice.outputString << "<br>"
      << theInterpreterWithAdvice.outputCommentsString;
    }
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = theGlobalVariables.GetElapsedSeconds() - startTime;
    return result;
  }
  if (theInterpreterWithAdvice.flagAbortComputationASAP) {
    out << "<br><span style =\"color:red\"><b>"
    << "Something went wrong when interpreting your answer "
    << "in the context of the current problem. "
    << "</b></span>";
    if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserDebugFlagOn()) {
      out << "<br>Logged-in as admin with debug flag on => printing error details. "
      << theInterpreterWithAdvice.outputString << "<br>"
      << theInterpreterWithAdvice.outputCommentsString;
      out << "<hr><b>Calculator input:</b> "
      << problemLinkStream.str() << "<br>"
      << calculatorInputStream.str();
    }
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = theGlobalVariables.GetElapsedSeconds() - startTime;
    return result;
  }
  if (hasCommentsBeforeSubmission){
    out << HtmlInterpretation::GetCommentsInterpretation(theInterpreterWithAdvice, 3, theFormat);
  }
  result[WebAPI::result::millisecondsComputation] = theGlobalVariables.GetElapsedSeconds() - startTime;
  if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserDebugFlagOn()) {
    out << "<hr> " << problemLinkStream.str()
    << "<br>"
    << calculatorInputStreamNoEnclosures.str()
    << "<br>"
    << "Result:<br>"
    << theInterpreterWithAdvice.outputString
    << "<br>" << theInterpreterWithAdvice.outputCommentsString;
  }
  result[WebAPI::result::resultHtml] = out.str();
  return result;
}

std::string HtmlInterpretation::ClonePageResult() {
  MacroRegisterFunctionWithName("HtmlInterpretation::ClonePageResult");
  if (
    !theGlobalVariables.flagLoggedIn ||
    !theGlobalVariables.UserDefaultHasAdminRights() ||
    !theGlobalVariables.flagUsingSSLinCurrentConnection
  ) {
    return "<b>Cloning problems allowed only for logged-in admins under ssl connection. </b>";
  }
  std::string fileNameTarget = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput(WebAPI::problem::fileNameTarget), false);
  std::string fileNameToBeCloned = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput(WebAPI::problem::fileName), false);
  std::stringstream out;
  std::string startingFileString;
  JSData result;
  if (!FileOperations::LoadFileToStringVirtualCustomizedReadOnly(fileNameToBeCloned, startingFileString, &out)) {
    out << "Could not find input file: " << fileNameToBeCloned;
    result[WebAPI::result::error] = out.str();
    return result.ToString(false);
  }
  std::fstream theFile;
  if (FileOperations::FileExistsVirtualCustomizedReadOnly(fileNameTarget, nullptr)) {
    out << "Output file: " << fileNameTarget << " already exists. ";
    result[WebAPI::result::error] = out.str();
    return out.str();
  }
  if (!FileOperations::OpenFileVirtualCustomizedWriteOnlyCreateIfNeeded(
    theFile, fileNameTarget, false, false, false, &out
  )) {
    out << "Failed to open output file: " << fileNameTarget << ".";
    result[WebAPI::result::error] = out.str();
    return out.str();
  }
  theFile << startingFileString;
  theFile.close();
  std::string fileNameNonVirtual;
  std::stringstream comments;
  if (!FileOperations::GetPhysicalFileNameFromVirtualCustomizedReadOnly(
    fileNameTarget, fileNameNonVirtual, &comments
  )) {
    out << "Could not get physical file name from virtual. " << comments.str();
  } else {
    out << "Wrote " << startingFileString.size() << " bytes to file: " << fileNameTarget;
  }
  result[WebAPI::result::resultHtml] = out.str();
  return result.ToString(false);
}

void HtmlInterpretation::BuildHtmlJSpage(bool appendBuildHash) {
  MacroRegisterFunctionWithName("HtmlInterpretation::BuildHtmlJSpage");
  std::stringstream outFirstPart, outSecondPart;
  std::stringstream theReader(this->htmlRaw);
  theReader.seekg(0);
  List<std::string> splitterStrings;
  std::string virtualFolder = appendBuildHash ?
  FileOperations::GetVirtualNameWithHash(WebAPI::request::calculatorHTML) :
  WebAPI::request::calculatorHTML;

  const std::string& browserifierScript = HtmlRoutines::GetJavascriptBrowserifier();
  const std::string& browserifierTag =
  "<script note = \"This tag has special treatment, do not change please.\" "
  "type = \"text/javascript\" src = \"/calculator-html/browserifier.js\"></script>";

  for (std::string currentLine; std::getline(theReader, currentLine, '\n');) {
    if (currentLine == browserifierTag) {
      outSecondPart << browserifierScript << "\n";
      continue;
    }
    int startChar = static_cast<int>(currentLine.find(WebAPI::request::calculatorHTML));
    bool shouldShortCut = false;
    if (startChar == - 1) {
      shouldShortCut = true;
    }
    if (shouldShortCut) {
      if (this->jsFileNames.size > 0) {
        outSecondPart << currentLine << "\n";
      } else {
        outFirstPart << currentLine << "\n";
      }
      continue;
    }
    std::string firstPart, secondAndThirdPart, thirdPart, notUsed;
    StringRoutines::SplitStringInTwo(currentLine, startChar, firstPart, secondAndThirdPart);
    StringRoutines::SplitStringInTwo(
      secondAndThirdPart,
      static_cast<int>(WebAPI::request::calculatorHTML.size()),
      notUsed,
      thirdPart
    );
    if (
      currentLine.find("<script") == std::string::npos ||
      currentLine.find("/MathJax.js?") != std::string::npos
    ) {
      std::stringstream lineStream;
      lineStream << firstPart << virtualFolder << thirdPart << "\n";
      if (this->jsFileNames.size > 0) {
        outSecondPart << lineStream.str();
      } else {
        outFirstPart << lineStream.str();
      }
    } else {
      StringRoutines::StringSplitExcludeDelimiter(secondAndThirdPart, '"', splitterStrings);
      this->jsFileNames.AddOnTop(splitterStrings[0]);
    }
  }
  std::string virtualOnePageJS = appendBuildHash ?
  FileOperations::GetVirtualNameWithHash(WebAPI::request::onePageJS) :
  WebAPI::request::onePageJS;
  outFirstPart << "<script src=\"" << virtualOnePageJS << "\" onerror = 'errorLoadingScript(this);'></script>\n"
  << outSecondPart.str();
  this->htmlJSbuild = outFirstPart.str();
}

std::string HtmlInterpretation::GetOnePageJS(bool appendBuildHash) {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetOnePageJS");
  HtmlInterpretation theInterpretation;
  std::stringstream out;
  std::stringstream errorStream;
  if (!FileOperations::LoadFileToStringVirtual(
    "/calculator-html/index.html", theInterpretation.htmlRaw, false, &errorStream
  )) {
    out << "<html><body><b>Failed to load the application file. "
    << "Further comments follow. " << errorStream.str() << "</body></html>";
    return out.str();
  }
  theInterpretation.BuildHtmlJSpage(appendBuildHash);
  theInterpretation.jsFileContents.SetSize(theInterpretation.jsFileNames.size);
  for (int i = 0; i < theInterpretation.jsFileNames.size; i ++) {
    if (!FileOperations::LoadFileToStringVirtual(
      theInterpretation.jsFileNames[i],
      theInterpretation.jsFileContents[i],
      false,
      &errorStream
    )) {
      return errorStream.str();
    }
  }
  return theInterpretation.GetOnePageJSBrowserify();
}

std::string HtmlInterpretation::GetOnePageJSBrowserify() {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetOnePageJSBrowserify");
  std::stringstream out;
  out << "var theJSContent = {\n";
  for (int i = 0; i < this->jsFileContents.size; i ++) {
    std::string fileNameNoJS;
    if (!StringRoutines::StringEndsWith(this->jsFileNames[i], ".js", &fileNameNoJS)) {
      fileNameNoJS = this->jsFileNames[i];
    }
    out << "\"" << fileNameNoJS << "\" : function(require, module, exports){\n";
    out << this->jsFileContents[i];
    out << "\n},\n";
  }
  out << "};\n";
  return out.str();
}

std::string HtmlInterpretation::GetApp(bool appendBuildHash) {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetApp");
  HtmlInterpretation theInterpretation;
  std::stringstream out;
  std::stringstream errorStream;
  if (!FileOperations::LoadFileToStringVirtual(
    "/calculator-html/index.html",
    theInterpretation.htmlRaw,
    false,
    &errorStream
  )) {
    out << "<html><body><b>Failed to load the application file. "
    << "Further comments follow. " << errorStream.str() << "</body></html>";
    return out.str();
  }
  theInterpretation.BuildHtmlJSpage(appendBuildHash);
  return theInterpretation.htmlJSbuild;
}

class Course {
public:
  std::string courseTemplate;
  std::string courseTopics;
  std::string title;
  std::string flagRoughDraft;
  bool IsEmpty();
  void reset();
  std::string ToString() const;
  JSData ToJSON() const;
};

JSData Course::ToJSON() const {
  JSData result;
  result["title"] = this->title;
  result["courseHome"] = "coursetemplates/" + this->courseTemplate;
  result["topicList"] = "topiclists/" + this->courseTopics;
  if (this->flagRoughDraft != "") {
    result["roughDraft"] = this->flagRoughDraft;
  }
  return result;
}

std::string Course::ToString() const {
  std::stringstream out;
  out << "Html: " << this->courseTemplate
  << "\n" << "Topics: " << this->courseTopics;
  return out.str();
}

bool Course::IsEmpty() {
  return this->courseTemplate == "" && this->courseTopics == "" && this->title == "";
}

void Course::reset() {
  this->courseTemplate = "";
  this->courseTopics = "";
  this->title = "";
  this->flagRoughDraft = "";
}

class CourseList {
public:
  List<Course> theCourses;
  void LoadFromString(const std::string& input, std::stringstream* commentsOnFailure);
  std::string ToHtml();

};

std::string CourseList::ToHtml() {
  return this->theCourses.ToString();
}

void CourseList::LoadFromString(const std::string& input, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("CourseList::LoadFromString");
  (void) commentsOnFailure;
  std::stringstream tableReader(input);
  std::string currentLine, currentArgument;
  Course current;
  while (std::getline(tableReader, currentLine, '\n')) {
    if (StringRoutines::StringBeginsWith(currentLine, "Html:", &currentArgument)) {
      if (current.courseTemplate != "") {
        this->theCourses.AddOnTop(current);
        current.reset();
      }
      current.courseTemplate = StringRoutines::StringTrimWhiteSpace(currentArgument);
    }
    if (StringRoutines::StringBeginsWith(currentLine, "Topics:", &currentArgument)) {
      if (current.courseTopics != "") {
        this->theCourses.AddOnTop(current);
        current.reset();
      }
      current.courseTopics = StringRoutines::StringTrimWhiteSpace(currentArgument);
    }
    if (StringRoutines::StringBeginsWith(currentLine, "Title:", &currentArgument)) {
      if (current.title != "") {
        this->theCourses.AddOnTop(current);
        current.reset();
      }
      current.title = StringRoutines::StringTrimWhiteSpace(currentArgument);
    }
    if (StringRoutines::StringBeginsWith(currentLine, "RoughDraft:", &currentArgument)) {
      if (current.flagRoughDraft != "") {
        this->theCourses.AddOnTop(current);
        current.reset();
      }
      current.flagRoughDraft = StringRoutines::StringTrimWhiteSpace(currentArgument);
    }
  }
  if (!current.IsEmpty()) {
    this->theCourses.AddOnTop(current);
  }
}

std::string HtmlInterpretation::GetSelectCourseJSON() {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetSelectCourseJSON");
  JSData output;
  std::stringstream comments;
  std::string coursesAvailableList = "/coursesavailable/default.txt";
  std::string theTopicFile;
  std::stringstream commentsOnFailure;
  std::string temp;
  FileOperations::GetPhysicalFileNameFromVirtualCustomizedReadOnly(coursesAvailableList, temp, &commentsOnFailure);
  if (!FileOperations::LoadFileToStringVirtualCustomizedReadOnly(
    "/coursesavailable/default.txt", theTopicFile, &commentsOnFailure
  )) {
    comments << "Failed to fetch available courses from /coursesavailable/default.txt. "
    << commentsOnFailure.str();
    output["error"] = comments.str();
    return output.ToString(false);
  }
  CourseList theCourses;
  theCourses.LoadFromString(theTopicFile, &comments);
  output["courses"].theType = JSData::token::tokenArray;
  for (int i = 0; i < theCourses.theCourses.size; i ++) {
    Course& currentCourse = theCourses.theCourses[i];
    output["courses"].theList.AddOnTop(currentCourse.ToJSON());
  }
  return output.ToString(false);
}

std::string HtmlInterpretation::GetHtmlTagWithManifest() {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetHtmlTagWithManifest");
  std::stringstream out;
  out << "<!DOCTYPE HTML>\n";
  out << "<html>\n<!-- tag added automatically; user-specified html tag ignored-->\n";
  return out.str();
}

std::string HtmlInterpretation::GetPageFromTemplate() {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetPageFromTemplate");
  std::stringstream out;
  CalculatorHTML thePage;
  std::stringstream comments;
  thePage.fileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("courseHome"), false);
  if (!thePage.LoadMe(true, theGlobalVariables.GetWebInput("randomSeed"), &comments)) {
    out << "<html>"
    << "<head>" << HtmlRoutines::GetCSSLinkCalculator("/") << "</head>"
    << "<body>";
    out
    << "<b>Failed to load file: "
    << theGlobalVariables.GetWebInput("courseHome") << ". </b>"
    << "<br>Comments:<br> " << comments.str();
    out << "</body></html>";
    return out.str();
  }
  if (!thePage.InterpretHtml(&comments)) {
    out << "<html>"
    << "<head>"
    << HtmlRoutines::GetCSSLinkCalculator("/")
    << "</head>"
    << "<body>";
    out
    << "<b>Failed to interpret as template the following file: "
    << theGlobalVariables.GetWebInput("courseHome") << ". </b>"
    << "<br>Comments:<br> " << comments.str();
    out << "</body></html>";
    return out.str();
  }
  out << HtmlInterpretation::GetHtmlTagWithManifest();
  out << "<!-- File automatically generated from template: "
  << theGlobalVariables.GetWebInput(WebAPI::problem::fileName)
  << ".-->\n";
  out << "<head><!-- tag added automatically; user-specified head tag ignored-->\n";
  out << thePage.outputHtmlHeadNoTag;
  out << "</head><!-- tag added automatically; user-specified head tag ignored-->\n";
  out << "<body" //<< ">"
  << " onload =\"loadSettings();";
  out << " initializeButtonsCommon(); ";
  out << "\"><!-- tag added automatically; user-specified body tag ignored-->\n";
  if (thePage.flagDoPrependProblemNavigationBar) {
    out << thePage.outputProblemNavigatioN;
  }
  out << thePage.outputHtmlBodyNoTag;
  out << "</body><!-- tag added automatically; user-specified body tag ignored-->\n";
  out << "</html><!-- tag added automatically; user-specified html tag ignored-->\n";
  return out.str();
}

std::string HtmlInterpretation::GetTopicTableJSON() {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetTopicTableJSON");
  std::stringstream out;
  CalculatorHTML thePage;
  std::stringstream comments;
  thePage.fileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("courseHome"), false);
  thePage.topicListFileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("topicList"), false);
  if (!thePage.LoadAndParseTopicList(out)) {
    out << "Failed to load and parse topic list.";
    return out.str();
  }
  if (thePage.LoadMe(true, theGlobalVariables.GetWebInput("randomSeed"), &comments)) {
    thePage.ComputeTopicListAndPointsEarned(comments);
  } else {
    comments << "\"Failed to load file: "
    << theGlobalVariables.GetWebInput("courseHome") << ""
    << "<br>Comments:<br> " << comments.str();
    comments << "\"";
  }
  out << thePage.ToStringTopicListJSON();
  return out.str();
}

void HtmlInterpretation::GetJSDataUserInfo(JSData& outputAppend, const std::string& comments) {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetJSDataUserInfo");
  std::stringstream outLinkApp, outLinkAppNoCache;
  // The calculator name should not contain dangerous symbols,
  // but let's sanitize in case of a programmer error.
  std::string sanitizedCalculatorApp = HtmlRoutines::ConvertStringToHtmlString(theGlobalVariables.DisplayNameExecutableApp, false);
  std::string sanitizedCalculatorAppNoCache = HtmlRoutines::ConvertStringToHtmlString(theGlobalVariables.DisplayNameExecutableAppNoCache, false);

  outLinkApp << "You've reached the calculator's backend. The app can be accessed here: <a href = '"
  << sanitizedCalculatorApp << "'>app</a>";
  outputAppend["linkApp"] = outLinkApp.str();
  outLinkAppNoCache << "<a href = '" << sanitizedCalculatorAppNoCache << "'>app no browser cache</a>";
  outputAppend["linkAppNoCache"] = outLinkAppNoCache.str();
  outputAppend[WebAPI::result::loginDisabledEveryoneIsAdmin] = theGlobalVariables.flagDisableDatabaseLogEveryoneAsAdmin;
  outputAppend[WebAPI::result::useFallbackDatabase] = theGlobalVariables.flagDatabaseUseFallback;
  if (comments != "") {
    outputAppend["comments"] = HtmlRoutines::ConvertStringToHtmlString(comments, false);
  }
  if (!theGlobalVariables.theProgress.flagBanProcessMonitoring) {
    outputAppend[WebAPI::UserInfo::processMonitoring] = "true";
    outputAppend[Configuration::millisecondsReplyAfterComputation] = static_cast<double>(theGlobalVariables.millisecondsReplyAfterComputation);
  } else {
    outputAppend[WebAPI::UserInfo::processMonitoring] = "false";
  }
  if (theGlobalVariables.GetWebInput("error") != "") {
    outputAppend["error"] = HtmlRoutines::ConvertStringToHtmlString(theGlobalVariables.GetWebInput("error"), false);
  }
  if (!theGlobalVariables.flagLoggedIn) {
    outputAppend["status"] = "not logged in";
    return;
  }
  outputAppend["status"] = "logged in";
  outputAppend[DatabaseStrings::labelUsername]            = HtmlRoutines::ConvertStringToHtmlString(theGlobalVariables.userDefault.username                 , false);
  outputAppend[DatabaseStrings::labelAuthenticationToken] = HtmlRoutines::ConvertStringToHtmlString(theGlobalVariables.userDefault.actualAuthenticationToken, false);
  outputAppend[DatabaseStrings::labelUserRole]            = HtmlRoutines::ConvertStringToHtmlString(theGlobalVariables.userDefault.userRole                 , false);
  outputAppend[DatabaseStrings::labelInstructor]          = HtmlRoutines::ConvertStringToHtmlString(theGlobalVariables.userDefault.instructorInDB           , false);
  outputAppend[DatabaseStrings::labelSection]             = HtmlRoutines::ConvertStringToHtmlString(theGlobalVariables.userDefault.sectionInDB              , false);
  outputAppend[DatabaseStrings::labelCurrentCourses]      = HtmlRoutines::ConvertStringToHtmlString(theGlobalVariables.userDefault.courseInDB               , false);
  outputAppend[DatabaseStrings::labelDeadlinesSchema]     = HtmlRoutines::ConvertStringToHtmlString(theGlobalVariables.userDefault.deadlineSchema           , false);
  JSData sectionsTaught;
  sectionsTaught.theType = JSData::token::tokenArray;
  for (int i = 0; i < theGlobalVariables.userDefault.sectionsTaught.size; i ++) {
    JSData nextSection;
    nextSection = HtmlRoutines::ConvertStringToHtmlString(theGlobalVariables.userDefault.sectionsTaught[i], false);
    sectionsTaught.theList.AddOnTop(nextSection);
  }
  outputAppend[DatabaseStrings::labelSectionsTaught] = sectionsTaught;
}

std::string HtmlInterpretation::GetJSONUserInfo(const std::string& comments) {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetJSONUserInfo");
  JSData output;
  HtmlInterpretation::GetJSDataUserInfo(output, comments);
  return output.ToString(false);
}

std::string HtmlInterpretation::GetJSONFromTemplate() {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetJSONFromTemplate");
  std::stringstream out;
  CalculatorHTML thePage;
  thePage.flagUseJSON = true;
  std::stringstream comments;
  thePage.fileName = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("courseHome"), false);
  if (!thePage.LoadMe(true, theGlobalVariables.GetWebInput("randomSeed"), &comments)) {
    out << "<b>Failed to load file: "
    << theGlobalVariables.GetWebInput("courseHome") << ". </b>"
    << "<br>Comments:<br> " << comments.str();
    return out.str();
  }
  if (!thePage.InterpretHtml(&comments)) {
    out << "<b>Failed to interpret as template the following file: "
    << theGlobalVariables.GetWebInput("courseHome") << ". </b>"
    << "<br>Comments:<br> " << comments.str();
    return out.str();
  }
  out << "<!-- File automatically generated from template: "
  << theGlobalVariables.GetWebInput(WebAPI::problem::fileName)
  << ".-->\n";
  out << thePage.outputHtmlBodyNoTag;
  out << "<small>Generated in " << theGlobalVariables.GetElapsedMilliseconds() << " ms. </small>";
  return out.str();
}

extern logger logWorker;

std::string HtmlInterpretation::GetExamPageJSON() {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetExamPageJSON");
  std::stringstream out;
  if (!theGlobalVariables.flagLoggedIn && theGlobalVariables.userCalculatorRequestType == "scoredQuizJSON") {
    out << "<b style ='color:red'>Scored quiz requires login</b>";
    return out.str();
  }
  CalculatorHTML theFile;
  theFile.flagDoPrependProblemNavigationBar = false;
  theFile.flagUseJSON = true;
  std::stringstream errorAndDebugStream;
  std::string problemBody = theFile.LoadAndInterpretCurrentProblemItemJSON(
    theGlobalVariables.UserRequestRequiresLoadingRealExamData(),
    theGlobalVariables.GetWebInput("randomSeed"),
    &errorAndDebugStream
  );
  //<-must come after theFile.outputHtmlHeadNoTag
  errorAndDebugStream << theFile.outputDebugInformationBody;
  out << problemBody;
  std::string commentsWebserver = HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  std::string commentsProblem = errorAndDebugStream.str();
  JSData output;
  output[WebAPI::problem::content] = HtmlRoutines::ConvertStringToURLString(out.str(), false);
  if (commentsWebserver != "") {
    output[WebAPI::commentsServer] = commentsWebserver;
  }
  if (commentsProblem != "") {
    output[WebAPI::problem::commentsProblem] = commentsProblem;
  }
  output[WebAPI::problem::deadlineSingle] = theFile.outputDeadlineString;
  output[WebAPI::problem::problemLabel] = theFile.outputProblemLabel;
  output[WebAPI::problem::title] = theFile.outputProblemTitle;
  output[WebAPI::problem::fileName] = theFile.fileName;
  output[WebAPI::problem::idProblem] = theFile.fileName;
  if (theFile.flagLoadedSuccessfully) {
    output["answers"] = theFile.GetJavascriptMathQuillBoxesForJSON();
    JSData theScripts;
    theScripts = JSData::token::tokenArray;
    theScripts.theList.SetSize(theFile.theScripts.size());
    for (int i = 0; i < theFile.theScripts.size(); i ++) {
      theScripts[theFile.theScripts.theKeys[i]] =
      HtmlRoutines::ConvertStringToURLString(theFile.theScripts.theValues[i], false);
    }
    output["scripts"] = theScripts;
    output["forReal"] = theFile.flagIsForReal;
    if (!theFile.flagIsForReal) {
      std::stringstream randomSeedStream;
      randomSeedStream << theFile.theProblemData.randomSeed;
      output["randomSeed"] = randomSeedStream.str();
    }
  }
  return output.ToString(false, false);
}

std::string HtmlInterpretation::GetEditPageJSON() {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetEditPageJSON");
  JSData output;
  if (
    (!theGlobalVariables.flagLoggedIn || !theGlobalVariables.UserDefaultHasAdminRights()) &&
    !theGlobalVariables.flagRunningApache
  ) {
    output["error"] = "Only logged-in admins are allowed to edit pages.";
    return output.ToString(false);
  }
  CalculatorHTML theFile;
  theFile.LoadFileNames();
  std::stringstream failureStream;
  if (!theFile.LoadMe(false, theGlobalVariables.GetWebInput("randomSeed"), &failureStream)) {
    std::stringstream errorStream;
    errorStream << " <b>Failed to load file: " << theFile.fileName << ", perhaps the file does not exist. </b>"
    << failureStream.str();
    output["error"] = errorStream.str();
    return output.ToString(false);
  }
  if (!theFile.ParseHTML(&failureStream)) {
    std::stringstream errorStream;
    errorStream << "<b>Failed to parse file: " << theFile.fileName
    << ".</b> Details:<br>" << failureStream.str();
    output["error"] = errorStream.str();
    //return output.ToString(false);
  }
  HashedList<std::string, MathRoutines::HashString> theAutocompleteKeyWords;
  theFile.initBuiltInSpanClasses();
  std::stringstream comments;
  if (theFile.flagIsExamProblem) {
    Calculator tempCalculator;
    tempCalculator.initialize();
    tempCalculator.ComputeAutoCompleteKeyWords();
    theAutocompleteKeyWords.AddOnTopNoRepetition(theFile.calculatorClasses);
    theAutocompleteKeyWords.AddOnTopNoRepetition(tempCalculator.autoCompleteKeyWords);
    theFile.initAutocompleteExtras();
    theAutocompleteKeyWords.AddOnTopNoRepetition(theFile.autoCompleteExtras);
  } else {
    theFile.LoadAndParseTopicList(comments);
    theAutocompleteKeyWords.AddOnTopNoRepetition(theFile.calculatorClasses);
    theAutocompleteKeyWords.AddOnTopNoRepetition(theFile.calculatorClassesAnswerFields);
    theAutocompleteKeyWords.AddOnTopNoRepetition(theFile.calculatorTopicElementNames);
    theAutocompleteKeyWords.AddOnTopNoRepetition(theFile.calculatorTopicBundles);
  }
  JSData theAutoCompleteWordsJS;
  theAutoCompleteWordsJS.theType = JSData::token::tokenArray;
  for (int i = 0; i < theAutocompleteKeyWords.size; i ++) {
    theAutoCompleteWordsJS[i] = theAutocompleteKeyWords[i];
  }
  output["autoComplete"] = theAutoCompleteWordsJS;
  output["content"] = HtmlRoutines::ConvertStringToURLString(theFile.inputHtml, false);
  return output.ToString(false);
}

std::string HtmlInterpretation::SubmitAnswersString() {
  JSData result;
  result = HtmlInterpretation::SubmitAnswersJSON(theGlobalVariables.GetWebInput("randomSeed"), nullptr, true);
  return result.ToString(false);
}

std::string HtmlInterpretation::SubmitAnswersString(
  const std::string& inputRandomSeed, bool* outputIsCorrect, bool timeSafetyBrake
) {
  JSData result;
  result = HtmlInterpretation::SubmitAnswersJSON(inputRandomSeed, outputIsCorrect, timeSafetyBrake);
  return result.ToString(false);
}

JSData HtmlInterpretation::SubmitAnswersJSON(
  const std::string& inputRandomSeed, bool* outputIsCorrect, bool timeSafetyBrake
) {
  MacroRegisterFunctionWithName("HtmlInterpretation::submitAnswers");
  StateMaintainer<bool> maintain(theGlobalVariables.theProgress.flagBanProcessMonitoring);
  theGlobalVariables.theProgress.flagBanProcessMonitoring = true;
  std::stringstream output, errorStream, comments;
  JSData result;
  double startTime = theGlobalVariables.GetElapsedSeconds();
  CalculatorHTML theProblem;
  theProblem.LoadCurrentProblemItem(
    theGlobalVariables.UserRequestRequiresLoadingRealExamData(), inputRandomSeed, &errorStream
  );
  if (!theProblem.flagLoadedSuccessfully) {
    errorStream << "Failed to load current problem. ";
    result[WebAPI::result::error] = errorStream.str();
    return result;
  }
  if (!theProblem.ParseHTMLPrepareCommands(&comments)) {
    errorStream << "<b>Failed to parse problem. </b>";
    result[WebAPI::result::error] = errorStream.str();
    result[WebAPI::result::comments] = comments.str();
    return result;
  }
  if (!theProblem.theProblemData.flagRandomSeedGiven && !theProblem.flagIsForReal) {
    output << "<b>Random seed not given.</b>";
  }
  if (theProblem.fileName == "") {
    crash << "This shouldn't happen: empty file name: theProblem.fileName." << crash;
  }
  std::string studentAnswerNameReader;
  theProblem.studentTagsAnswered.init(theProblem.theProblemData.theAnswers.size());
  MapList<std::string, std::string, MathRoutines::HashString>& theArgs = theGlobalVariables.webArguments;
  int answerIdIndex = - 1;
  for (int i = 0; i < theArgs.size(); i ++) {
    if (StringRoutines::StringBeginsWith(theArgs.theKeys[i], "calculatorAnswer", &studentAnswerNameReader)) {
      int newAnswerIndex = theProblem.GetAnswerIndex(studentAnswerNameReader);
      if (answerIdIndex == - 1) {
        answerIdIndex = newAnswerIndex;
      } else if (answerIdIndex != newAnswerIndex && answerIdIndex != - 1 && newAnswerIndex != - 1) {
        output << "<b>You submitted two or more answers [answer tags: "
        << theProblem.theProblemData.theAnswers.theValues[answerIdIndex].answerId
        << " and " << theProblem.theProblemData.theAnswers.theValues[newAnswerIndex].answerId
        << "].</b> At present, multiple answer submission is not supported. ";
        result[WebAPI::result::resultHtml] = output.str();
        return result;
      }
      if (answerIdIndex == - 1) {
        output << "<b> You submitted an answer to tag with id "
        << studentAnswerNameReader
        << " which is not on my list of answerable tags. </b>";
        result[WebAPI::result::resultHtml] = output.str();
        return result;
      }
      Answer& currentProblemData = theProblem.theProblemData.theAnswers.theValues[answerIdIndex];
      currentProblemData.currentAnswerURLed = theArgs.theValues[i];
      if (currentProblemData.currentAnswerURLed == "") {
        output << "<b> Your answer to tag with id " << studentAnswerNameReader
        << " appears to be empty, please resubmit. </b>";
        result[WebAPI::result::resultHtml] = output.str();
        return result;
      }
    }
  }
  if (answerIdIndex == - 1) {
    output << "<b>Something is wrong: I found no submitted answers.</b>";
    result[WebAPI::result::resultHtml] = output.str();
    return result;
  }
  ProblemData& currentProblemData = theProblem.theProblemData;
  Answer& currentA = currentProblemData.theAnswers.theValues[answerIdIndex];

  currentA.currentAnswerClean = HtmlRoutines::ConvertURLStringToNormal(currentA.currentAnswerURLed, false);
  currentA.currentAnswerURLed = HtmlRoutines::ConvertStringToURLString(currentA.currentAnswerClean, false);//<-encoding back to overwrite malformed input
  theProblem.studentTagsAnswered.AddSelectionAppendNewIndex(answerIdIndex);
  std::stringstream completedProblemStreamNoEnclosures;

  std::stringstream completedProblemStream;
  completedProblemStream << "CommandEnclosure{}("
  << currentA.commandsBeforeAnswer
  << ");";
  completedProblemStreamNoEnclosures << currentA.commandsBeforeAnswerNoEnclosuresForDEBUGGING;

  completedProblemStream << "CommandEnclosure{}("
  << currentA.answerId << "= (" << currentA.currentAnswerClean << ");"
  << ");";
  completedProblemStreamNoEnclosures << currentA.answerId << "= (" << currentA.currentAnswerClean << ");";

  bool hasCommentsBeforeSubmission = (
    StringRoutines::StringTrimWhiteSpace(currentA.commandsCommentsBeforeSubmission) != ""
  );
  if (hasCommentsBeforeSubmission) {
    completedProblemStream
    << "CommandEnclosure{}("
    << currentA.commandsCommentsBeforeSubmission
    << ");";
    completedProblemStreamNoEnclosures << currentA.commandsCommentsBeforeSubmission;
  }
  completedProblemStream << theProblem.CleanUpCommandString(currentA.commandVerificationOnly);
  completedProblemStreamNoEnclosures << theProblem.CleanUpCommandString(currentA.commandVerificationOnly);

  std::stringstream debugInputStream;
  if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights()) {
    debugInputStream
    << "Input, no enclosures, direct link: "
    << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=calculator&mainInput="
    << HtmlRoutines::ConvertStringToURLString(completedProblemStreamNoEnclosures.str(), false)
    << "\">Input link</a>";
  }
  if (timeSafetyBrake) {
    theGlobalVariables.millisecondsMaxComputation = theGlobalVariables.GetElapsedMilliseconds() + 20000; // + 20 sec
  }
  Calculator theInterpreter;
  theInterpreter.initialize();
  theInterpreter.flagWriteLatexPlots = false;
  theInterpreter.flagPlotNoControls = true;

  theInterpreter.Evaluate(completedProblemStream.str());
  if (theInterpreter.flagAbortComputationASAP || theInterpreter.syntaxErrors != "") {
    if (theInterpreter.errorsPublic.str() != "") {
      output << "While checking your answer, got the error: "
      << "<br><b><span style =\"color:red\">"
      << theInterpreter.errorsPublic.str()
      << "</span></b> "
      << "<br><b>Most likely your answer is wrong. </b>";
    } else {
      output << "<b>Error while processing your answer(s).</b> "
      << "<br>Perhaps your answer was wrong and "
      << "generated division by zero during checking. ";
    }
    output << "<br>Here's what I understood. ";
    Calculator isolatedInterpreter;
    isolatedInterpreter.initialize();
    isolatedInterpreter.flagWriteLatexPlots = false;
    isolatedInterpreter.flagPlotNoControls = true;
    if (timeSafetyBrake) {
      theGlobalVariables.millisecondsMaxComputation = theGlobalVariables.GetElapsedMilliseconds() + 20000; //+20 sec
    }
    isolatedInterpreter.Evaluate("(" + currentA.currentAnswerClean + ")");
    if (isolatedInterpreter.syntaxErrors != "") {
      output << isolatedInterpreter.ToStringSyntacticStackHumanReadable(false, true);
    } else {
      output << isolatedInterpreter.outputString;
    }
    if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights()) {
      output << "<hr><b>Admin view internals.</b><hr>"
      << debugInputStream.str() << "<hr>"
      << theInterpreter.outputString
      << "<br>" << theInterpreter.outputCommentsString
      << "<hr>Input, no enclosures: <hr>"
      << completedProblemStreamNoEnclosures.str()
      << "<br>";
    }
    result[WebAPI::result::resultHtml] = output.str();
    return result;
  }
  bool tempIsCorrect = false;
  if (outputIsCorrect == nullptr) {
    outputIsCorrect = &tempIsCorrect;
  }
  *outputIsCorrect = false;
  int mustBeOne = - 1;
  if (!theInterpreter.theProgramExpression[theInterpreter.theProgramExpression.size() - 1].IsSmallInteger(&mustBeOne)) {
    *outputIsCorrect = false;
  } else {
    *outputIsCorrect = (mustBeOne == 1);
  }
  FormatExpressions theFormat;
  output << "<table style = 'width:300px'>";
  if (!(*outputIsCorrect)) {
    output << "<tr><td><b style = 'color:red'><b>Your answer appears to be incorrect. </b></td></tr>";
    if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserDebugFlagOn()) {
      output << "<tr><td>Admin view internals. "
      << "<hr>" << debugInputStream.str()
      << "<br>The calculator output is: " << theInterpreter.outputString
      << "Comments: " << theInterpreter.Comments.str()
      << "<hr>Input, no enclosures: <hr>"
      << completedProblemStreamNoEnclosures.str();
      output << "<hr>Input, full:<hr>"
      << theInterpreter.inputString << "<hr></td></tr>";
    }
  } else {
    output << "<tr><td><span style =\"color:green\"><b>Correct! </b></span>" << "</td></tr>";
  }
  if (hasCommentsBeforeSubmission) {
    output << "<tr><td>" << HtmlInterpretation::GetCommentsInterpretation(theInterpreter, 3, theFormat) << "</td></tr>\n";
  }
  if (theGlobalVariables.flagDatabaseCompiled) {
    UserCalculator& theUser = theProblem.currentUseR;
    theUser.::UserCalculatorData::operator=(theGlobalVariables.userDefault);
    bool deadLinePassed = false;
    bool hasDeadline = true;
    double secondsTillDeadline = - 1;
    if (theProblem.flagIsForReal) {
      if (!theProblem.LoadAndParseTopicList(output)) {
        hasDeadline = false;
      }
      std::string theSQLstring;
      theSQLstring = theUser.sectionComputed;
      if (hasDeadline) {
        bool unused = false;
        std::string theDeadlineString =
        theProblem.GetDeadline(theProblem.fileName, HtmlRoutines::ConvertStringToURLString(theSQLstring, false), unused);

        if (theDeadlineString == "" || theDeadlineString == " ") {
          hasDeadline = false;
        } else {
          TimeWrapper now, deadline; //<-needs a fix for different time formats.
          //<-For the time being, we hard-code it to month/day/year format (no time to program it better).
          std::stringstream badDateStream;
          if (!deadline.AssignMonthDayYear(theDeadlineString, badDateStream)) {
            output << "<tr><td><b>Problem reading deadline. </b> The deadline string was: "
            << theDeadlineString << ". Comments: "
            << "<span style =\"color:red\">" << badDateStream.str() << "</span>"
            << "</td></tr><tr><td> This should not happen. " << CalculatorHTML::BugsGenericMessage << "</td></tr>";
            output << "</table>";
            result[WebAPI::result::resultHtml] = output.str();
            return result;
          }
          now.AssignLocalTime();
          //  out << "Now: " << asctime (&now.theTime) << " mktime: " << mktime(&now.theTime)
          //  << " deadline: " << asctime(&deadline.theTime) << " mktime: " << mktime(&deadline.theTime);
          secondsTillDeadline = deadline.SubtractAnotherTimeFromMeInSeconds(now) + 7 * 3600;
          deadLinePassed = (secondsTillDeadline < - 18000);
        }
      }
      if (deadLinePassed) {
        output << "<tr><td><span style =\"color:red\"><b>Deadline passed, attempt not recorded.</b></span></td></tr>";
      } else {
        currentA.numSubmissions ++;
        if ((*outputIsCorrect)) {
          currentA.numCorrectSubmissions ++;
          if (currentA.firstCorrectAnswerClean == "") {
            currentA.firstCorrectAnswerClean = currentA.currentAnswerClean;
          } else {
            output << "<tr><td>[first correct answer: " << currentA.firstCorrectAnswerClean << "]</td></tr>";
          }
        }
      }
    }
    if (theProblem.flagIsForReal) {
      std::stringstream comments;
      //if (theGlobalVariables.UserDefaultHasAdminRights() && theGlobalVariables.UserDebugFlagOn())
      //  stOutput << "<hr>DEBUG: adding prob data for file name: " << theProblem.fileName
      //  << "<br>"
      //  << currentProblemData.ToString() << "<br> into:<br> "
      //  << theUser.theProblemData.GetValueCreateIfNotPresent(theProblem.fileName).ToString()
      //  << "<hr>";
      theUser.SetProblemData(theProblem.fileName, currentProblemData);
      if (!theUser.StoreProblemDataToDatabaseJSON(&comments)) {
        output << "<tr><td><b>This shouldn't happen and may be a bug: failed to store your answer in the database. "
        << CalculatorHTML::BugsGenericMessage << "</b><br>Comments: "
        << comments.str() << "</td></tr>";
      } else {
        output << "<tr><td>So far " << currentA.numCorrectSubmissions << " correct and "
        << currentA.numSubmissions - currentA.numCorrectSubmissions
        << " incorrect submissions.</td></tr>";
      }
      if (hasDeadline) {
        if (secondsTillDeadline < 0) {
          secondsTillDeadline *= - 1;
        }
        if (deadLinePassed) {
          output << "<tr><td><span style =\"color:red\"><b>Submission "
          << TimeWrapper::ToStringSecondsToDaysHoursSecondsString(secondsTillDeadline, false, false)
          << " after deadline. </b></span></td></tr>";
        } else {
          output << "<tr><td><span style =\"color:green\"><b>Submission "
          << TimeWrapper::ToStringSecondsToDaysHoursSecondsString(secondsTillDeadline, false, false)
          << " before deadline. </b></span></td></tr>";
        }
      } else {
        output << "<tr><td><span style =\"color:green\"><b>No deadline yet.</b></span></td></tr>";
      }
    }
  }
  output << "<tr><td>Your answer was: ";
  output << "\\(\\displaystyle ";
  output << currentA.currentAnswerClean;
  output << "\\)";
  std::string errorMessage;
  errorMessage = theInterpreter.ToStringIsCorrectAsciiCalculatorString(currentA.currentAnswerClean);
  if (errorMessage != "") {
    output << "<br>" << errorMessage
    << "<hr><b>If you entered this expression through the keyboard (without copying + pasting) this is a bug: "
    << "please report it to the web site administrator. Don't forget to mention your keyboard/character setup. "
    << "Are you using the standard English keyboard? Cyrillic, Chinese, etc. characters are not accepted. </b> "
    << "<hr><span style =\"color:red\"><b>Copying and pasting an answer not computed by yourself "
    << " is considered cheating (example: answer from an online program for doing homework).</b> </span>";
  }
  output << "</td></tr>";
  output << "</table>";
  result[WebAPI::result::resultHtml] = output.str();
  result[WebAPI::result::millisecondsComputation] = theGlobalVariables.GetElapsedSeconds() - startTime;
  return result;
}

std::string HtmlInterpretation::AddTeachersSections() {
  MacroRegisterFunctionWithName("HtmlInterpretation::AddTeachersSections");
  std::stringstream out;
  if (!theGlobalVariables.UserDefaultHasAdminRights() || !theGlobalVariables.flagUsingSSLinCurrentConnection) {
    out << "<b>Only admins may assign sections to teachers.</b>";
    return out.str();
  }
  std::string input = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("teachersAndSections"), false);
  JSData inputParsed;
  if (!inputParsed.readstring(input, false, &out)) {
    out << "<b style='color:red'>Failed to interpret your input. </b>";
    return out.str();
  }
  if (inputParsed["teachers"].theType != JSData::token::tokenString) {
    out << "<b style='color:red'>Failed to extract key 'teachers' from your input. </b>";
    return out.str();
  }
  if (inputParsed["students"].theType != JSData::token::tokenString) {
    out << "<b style='color:red'>Failed to find key 'students' in your input. </b>";
    return out.str();
  }
  if (!theGlobalVariables.flagDatabaseCompiled) {
    return "<b>no database present.</b>";
  }
  std::string desiredUsers =
  HtmlRoutines::ConvertURLStringToNormal(inputParsed["teachers"].theString, false);
  std::string desiredSectionsOneString =
  HtmlRoutines::ConvertURLStringToNormal(inputParsed["students"].theString, false);
  List<std::string> desiredSectionsList;

  List<std::string> theTeachers;
  List<char> delimiters;
  delimiters.AddOnTop(' ');
  delimiters.AddOnTop('\r');
  delimiters.AddOnTop('\n');
  delimiters.AddOnTop('\t');
  delimiters.AddOnTop(',');
  delimiters.AddOnTop(';');
  delimiters.AddOnTop(static_cast<char>(160));//<-&nbsp
  StringRoutines::StringSplitExcludeDelimiters(desiredSectionsOneString, delimiters, desiredSectionsList);

  StringRoutines::StringSplitExcludeDelimiters(desiredUsers, delimiters, theTeachers);
  if (theTeachers.size == 0) {
    out << "<b>Could not extract teachers from " << desiredUsers << ".</b>";
    return out.str();
  }
  UserCalculator currentTeacher;
  for (int i = 0; i < theTeachers.size; i ++) {
    currentTeacher.reset();
    currentTeacher.username = theTeachers[i];
    if (!currentTeacher.LoadFromDB(&out, &out)) {
      out << "<span style =\"color:red\">Failed to fetch teacher: " << theTeachers[i] << "</span><br>";
      continue;
    }
    currentTeacher.sectionsTaught = desiredSectionsList;
    JSData findQuery, setQuery;
    findQuery[DatabaseStrings::labelUsername] = currentTeacher.username;
    setQuery[DatabaseStrings::labelSectionsTaught] = desiredSectionsList;
    if (!DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON(
      DatabaseStrings::tableUsers, findQuery, setQuery, nullptr, &out
    )) {
      out << "<span style =\"color:red\">Failed to store course info of instructor: " << theTeachers[i] << ". </span><br>";
    } else {
      out << "<span style =\"color:green\">Assigned " << theTeachers[i] << " to section(s): "
      << desiredSectionsList << "</span><br>";
    }
  }
  return out.str();
}

std::string HtmlInterpretation::AddUserEmails(const std::string& hostWebAddressWithPort) {
  MacroRegisterFunctionWithName("HtmlInterpretation::AddUserEmails");
  (void) hostWebAddressWithPort;
  std::stringstream out;
  if (!theGlobalVariables.UserDefaultHasAdminRights() || !theGlobalVariables.flagUsingSSLinCurrentConnection) {
    out << "<b>Only admins may add users, under ssl connection. </b>";
    return out.str();
  }
  //stOutput << "<br>DEBUG: userlist: " << theGlobalVariables.GetWebInput("userList") << "<br>";
  //stOutput << "DEBUG: userlist: " << theGlobalVariables.GetWebInput("passwordList") << "<br>";
  std::string inputEmails = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("userList"), false);
  std::string userPasswords = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("passwordList"), false);
  std::string userGroup =
  StringRoutines::StringTrimWhiteSpace(HtmlRoutines::ConvertURLStringToNormal(
    theGlobalVariables.GetWebInput(DatabaseStrings::labelSection), false
  ));
  std::string userRole = HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("userRole"), false);

  if (inputEmails == "") {
    out << "<b>No emails to add</b>";
    return out.str();
  }
  if (!theGlobalVariables.flagDatabaseCompiled) {
    return "<b>no database present.</b>";
  }
  std::stringstream comments;
  bool sentEmails = true;
  bool doSendEmails = theGlobalVariables.userCalculatorRequestType == "sendEmails" ?  true : false;
  int numNewUsers = 0;
  int numUpdatedUsers = 0;
  bool createdUsers = DatabaseRoutineS::AddUsersFromEmails(
    inputEmails, userPasswords, userRole, userGroup, comments, numNewUsers, numUpdatedUsers
  );
  if (createdUsers) {
    out << "<span style =\"color:green\">Success: "
    << numNewUsers << " new users and " << numUpdatedUsers
    << " user updates. </span> User roles: " << userRole;
  } else
    out << "<span style =\"color:red\">Failed to add all users. </span> Errors follow. <hr>"
    << comments.str() << "<hr>";
  if (doSendEmails) {
    if (sentEmails) {
      out << "<span style =\"color:green\">Activation emails successfully sent. </span>";
    } else {
      out << "<span style =\"color:red\">Failed to send all activation emails. </span>";
    }
  }
  return out.str();
}

const std::string CalculatorHTML::BugsGenericMessage =
"Please take a screenshot, copy the link address and send those along "
"with a short explanation to the administrator of the web site. ";

std::string HtmlInterpretation::GetAnswerOnGiveUp() {
  return HtmlInterpretation::GetAnswerOnGiveUp(theGlobalVariables.GetWebInput("randomSeed"));
}

std::string HtmlInterpretation::GetAnswerOnGiveUp(
  const std::string& inputRandomSeed, std::string* outputNakedAnswer, bool* outputDidSucceed
) {
  MacroRegisterFunctionWithName("CalculatorHTML::GetAnswerOnGiveUp");
  StateMaintainer<bool> maintain(theGlobalVariables.theProgress.flagBanProcessMonitoring);
  theGlobalVariables.theProgress.flagBanProcessMonitoring = true;
  if (outputNakedAnswer != nullptr) {
    *outputNakedAnswer = "";
  }
  if (outputDidSucceed != nullptr) {
    *outputDidSucceed = false;
  }
  int64_t startTimeInMilliseconds = theGlobalVariables.GetElapsedMilliseconds();
  CalculatorHTML theProblem;
  std::stringstream errorStream;
  theProblem.LoadCurrentProblemItem(false, inputRandomSeed, &errorStream);
  JSData result;
  if (!theProblem.flagLoadedSuccessfully) {
    errorStream << "Problem name is: " << theProblem.fileName
    << " <b>Could not load problem, this may be a bug. "
    << CalculatorHTML::BugsGenericMessage << "</b>";
    result[WebAPI::result::error] = errorStream.str();
    return result.ToString(false);
  }
  if (theProblem.flagIsForReal) {
    errorStream << " <b>Not allowed to show answer of a problem being tested for real. </b>";
    result[WebAPI::result::error] = errorStream.str();
    return result.ToString(false);
  }
  if (inputRandomSeed == "") {
    result[WebAPI::result::error] = "<b>I could not figure out the exercise problem (missing random seed). </b>";
    return result.ToString(false);
  }
  if (!theProblem.ParseHTMLPrepareCommands(&errorStream)) {
    errorStream << "<br><b>Problem preparation failed.</b>";
    result[WebAPI::result::error] = errorStream.str();
    return result.ToString(false);
  }
  std::string lastStudentAnswerID;
  MapList<std::string, std::string, MathRoutines::HashString>& theArgs = theGlobalVariables.webArguments;
  for (int i = 0; i < theArgs.size(); i ++) {
    StringRoutines::StringBeginsWith(theArgs.theKeys[i], "calculatorAnswer", &lastStudentAnswerID);
  }
  int indexLastAnswerId = theProblem.GetAnswerIndex(lastStudentAnswerID);
  std::stringstream out;
  if (indexLastAnswerId == - 1) {
    errorStream << "File: "
    << theProblem.fileName
    << "<br><b>Student submitted answerID: " << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "</b>";
    if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights()) {
      errorStream << "<hr>" << theProblem.theProblemData.ToStringAvailableAnswerIds();
      errorStream << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
    }
    result[WebAPI::result::millisecondsComputation] = theGlobalVariables.GetElapsedMilliseconds() - startTimeInMilliseconds;
    result[WebAPI::result::error] = errorStream.str();
    return result.ToString(false);
  }
  Answer& currentA = theProblem.theProblemData.theAnswers.theValues[indexLastAnswerId];
  if (currentA.commandsNoEnclosureAnswerOnGiveUpOnly == "") {
    out << "<b> Unfortunately there is no answer given for this "
    << "question (answerID: " << lastStudentAnswerID << ").</b>";
    if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasProblemComposingRights()) {
      out << "<br>Answer status: " << currentA.ToString();
    }
    return out.str();
  }
  Calculator theInterpreteR;
  theInterpreteR.initialize();
  theInterpreteR.flagPlotNoControls = true;
  theInterpreteR.flagWriteLatexPlots = false;
  std::stringstream answerCommands, answerCommandsNoEnclosure;
  answerCommands << currentA.commandsBeforeAnswer;
  answerCommandsNoEnclosure << currentA.commandsBeforeAnswerNoEnclosuresForDEBUGGING;
  answerCommands << "CommandEnclosure{}(" << currentA.commandsNoEnclosureAnswerOnGiveUpOnly << ");";
  answerCommandsNoEnclosure << currentA.commandsNoEnclosureAnswerOnGiveUpOnly;
  theInterpreteR.Evaluate(answerCommands.str());
  if (theInterpreteR.syntaxErrors != "") {
    out << "<span style =\"color:red\"><b>Failed to evaluate the default answer. "
    << "Likely there is a bug with the problem. </b></span>";
    if (theGlobalVariables.UserDefaultHasProblemComposingRights()) {
      out << "<br>\n<a href=\"" << theGlobalVariables.DisplayNameExecutable
      << "?request=calculator&"
      << "mainInput="
      << HtmlRoutines::ConvertStringToURLString(answerCommandsNoEnclosure.str(), false)
      << "\">Calculator input no enclosures</a>";
    }
    out << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.ToStringSyntacticStackHumanReadable(false, false);
    result[WebAPI::result::resultHtml] = out.str();
    int64_t ellapsedTime = theGlobalVariables.GetElapsedMilliseconds() - startTimeInMilliseconds;
    result[WebAPI::result::millisecondsComputation] = ellapsedTime;
    return result.ToString(false);
  }
  if (theInterpreteR.flagAbortComputationASAP) {
    out << "<span style =\"color:red\"><b>Failed to evaluate the default answer. "
    << "Likely there is a bug with the problem. </b></span>";
    if (theGlobalVariables.UserDefaultHasProblemComposingRights()) {
      out << "<br>\n<a href=\"" << theGlobalVariables.DisplayNameExecutable
      << "?request=calculator&"
      << "mainInput="
      << HtmlRoutines::ConvertStringToURLString(answerCommandsNoEnclosure.str(), false)
      << "\">Calculator input no enclosures</a>";
    }
    out << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.outputString
    << theInterpreteR.outputCommentsString
    << "<hr>Input: <br>" << theInterpreteR.inputString;
    int64_t ellapsedTime = theGlobalVariables.GetElapsedMilliseconds() - startTimeInMilliseconds;
    result[WebAPI::result::millisecondsComputation] = ellapsedTime;
    result[WebAPI::result::resultHtml] = out.str();
    return result.ToString(false);
  }
  FormatExpressions theFormat;
  theFormat.flagExpressionIsFinal = true;
  theFormat.flagIncludeExtraHtmlDescriptionsInPlots = false;
  theFormat.flagUseQuotes = false;
  theFormat.flagUseLatex = true;
  theFormat.flagUsePmatrix = true;
  bool isFirst = true;
  const Expression& currentE = theInterpreteR.theProgramExpression[theInterpreteR.theProgramExpression.size() - 1][1];
  if (!currentE.StartsWith(theInterpreteR.opEndStatement())) {
    out << "\\(\\displaystyle " << currentE.ToString(&theFormat) << "\\)";
    if (outputNakedAnswer != nullptr) {
      *outputNakedAnswer = currentE.ToString(&theFormat);
    }
    if (outputDidSucceed != nullptr) {
      *outputDidSucceed = true;
    }
  } else {
    for (int j = 1; j < currentE.size(); j ++) {
      if (
        currentE[j].StartsWith(theInterpreteR.opRulesOff()) ||
        currentE[j].StartsWith(theInterpreteR.opRulesOn())
      ) {
        continue;
      }
      std::string stringAnswer;
      if (currentE[j].IsOfType<std::string>(&stringAnswer)) {
        if (StringRoutines::StringBeginsWith(stringAnswer, "Approximations have been")) {
          continue;
        }
      }
      if (!isFirst) {
        out << "<br>";
      }
      theFormat.flagExpressionIsFinal = true;
      theFormat.flagIncludeExtraHtmlDescriptionsInPlots = false;
      theFormat.flagUseQuotes = false;
      theFormat.flagUseLatex = true;
      if (currentE[j].IsOfType<std::string>()) {
        out << currentE[j].GetValue<std::string>();
      } else {
        out << "\\(\\displaystyle " << currentE[j].ToString(&theFormat) << "\\)";
      }
      if (isFirst) {
        if (outputNakedAnswer != nullptr) {
          *outputNakedAnswer = currentE[j].ToString(&theFormat);
        }
        if (outputDidSucceed != nullptr) {
          *outputDidSucceed = true;
        }
      }
      isFirst = false;
    }
  }
  int64_t ellapsedTime = theGlobalVariables.GetElapsedMilliseconds() - startTimeInMilliseconds;
  result[WebAPI::result::millisecondsComputation] = ellapsedTime;
  if (theGlobalVariables.UserDebugFlagOn() && theGlobalVariables.UserDefaultHasAdminRights()) {
    out
    << "<hr><a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=calculator&"
    << "mainInput="
    << HtmlRoutines::ConvertStringToURLString(answerCommandsNoEnclosure.str() ,false)
    << "\">Calculator input no enclosures</a>"
    << theInterpreteR.outputString << "<hr>"
    << theInterpreteR.outputCommentsString
    << "<hr>Raw input: <br>" << theInterpreteR.inputString;
  }
  result[WebAPI::result::resultHtml] = out.str();
  return result.ToString(false);
}

std::string HtmlInterpretation::GetAccountsPageJSON(const std::string& hostWebAddressWithPort) {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetAccountsPageJSON");
  (void) hostWebAddressWithPort;
  JSData output;
  if (!theGlobalVariables.flagDatabaseCompiled) {
    output[WebAPI::result::error] = "Database not available (cannot get accounts). ";
    return output.ToString(false);
  }
  if (
    !theGlobalVariables.UserDefaultHasAdminRights() ||
    !theGlobalVariables.flagLoggedIn ||
    !theGlobalVariables.flagUsingSSLinCurrentConnection
  ) {
    output[WebAPI::result::error] = "Must be logged-in admin over ssl.";
    return output.ToString(false);
  }
  std::stringstream commentsOnFailure;
  JSData findStudents;
  JSData findAdmins;
  List<JSData> students;
  List<JSData> admins;
  long long totalStudents;
  findStudents[DatabaseStrings::labelInstructor] = theGlobalVariables.userDefault.username;
  findAdmins[DatabaseStrings::labelUserRole] = "admin";
  List<std::string> columnsToRetain;
  columnsToRetain.AddOnTop(DatabaseStrings::labelUsername);
  columnsToRetain.AddOnTop(DatabaseStrings::labelEmail);
  columnsToRetain.AddOnTop(DatabaseStrings::labelActivationToken);
  columnsToRetain.AddOnTop(DatabaseStrings::labelCurrentCourses);
  columnsToRetain.AddOnTop(DatabaseStrings::labelInstructor);
  columnsToRetain.AddOnTop(DatabaseStrings::labelSection);
  columnsToRetain.AddOnTop(DatabaseStrings::labelSemester);
  if (!DatabaseRoutinesGlobalFunctionsMongo::FindFromJSONWithProjection(
    DatabaseStrings::tableUsers, findStudents, students, columnsToRetain, - 1, &totalStudents, &commentsOnFailure
  )) {
    output["error"] = "Failed to load user info. Comments: " + commentsOnFailure.str();
    return output.ToString(false);
  }
  if (!DatabaseRoutinesGlobalFunctionsMongo::FindFromJSONWithProjection(
    DatabaseStrings::tableUsers, findAdmins, admins, columnsToRetain, - 1, nullptr, &commentsOnFailure
  )) {
    output["error"] = "Failed to load user info. Comments: " + commentsOnFailure.str();
    return output.ToString(false);
  }
  output["admins"] = admins;
  output["students"] = students;
  return output.ToString(false);
}

std::string HtmlInterpretation::GetAccountsPageBody(const std::string& hostWebAddressWithPort) {
  MacroRegisterFunctionWithName("HtmlInterpretation::GetAccountsPageBody");
  (void) hostWebAddressWithPort;
  if (!theGlobalVariables.flagDatabaseCompiled) {
    return "Database not available. ";
  }
  std::stringstream out;
  if (
    !theGlobalVariables.UserDefaultHasAdminRights() ||
    !theGlobalVariables.flagLoggedIn ||
    !theGlobalVariables.flagUsingSSLinCurrentConnection
  ) {
    out << "Browsing accounts allowed only for logged-in admins over ssl connection.";
    return out.str();
  }
  std::stringstream commentsOnFailure;
  JSData findStudents;
  JSData findAdmins;
  List<JSData> students;
  List<JSData> admins;
  long long totalStudents;
  findStudents[DatabaseStrings::labelInstructor] = theGlobalVariables.userDefault.username;
  findAdmins[DatabaseStrings::labelUserRole] = "admin";
  if (!DatabaseRoutinesGlobalFunctionsMongo::FindFromJSON(
    DatabaseStrings::tableUsers, findStudents, students, - 1, &totalStudents, &commentsOnFailure
  )) {
    out << "<b>Failed to load user info.</b> Comments: " << commentsOnFailure.str();
    return out.str();
  }
  if (!DatabaseRoutinesGlobalFunctionsMongo::FindFromJSON(
    DatabaseStrings::tableUsers, findAdmins, admins, - 1, nullptr, &commentsOnFailure
  )) {
    out << "<b>Failed to load user info.</b> Comments: " << commentsOnFailure.str();
    return out.str();
  }
  out << "<hr>";
  out << HtmlInterpretation::ToStringAssignSection();
  out << "<hr>";
  out << HtmlInterpretation::ToStringUserDetails(true, admins, hostWebAddressWithPort);
  out << "<hr>";
  out << HtmlInterpretation::ToStringUserDetails(false, students, hostWebAddressWithPort);
  return out.str();
}

std::string HtmlInterpretation::GetScoresPage() {
  MacroRegisterFunctionWithName("WebWorker::GetScoresPage");
  std::stringstream out;
  out << "<html>"
  << "<head>"
  << HtmlRoutines::GetCSSLinkCalculator("/")
  << "<link rel =\"stylesheet\" href=\"/calculator-html/styleScorePage.css\">"
  << "</head>"
  << "<body onload =\"loadSettings();\">\n";
  CalculatorHTML thePage;
  thePage.LoadDatabaseInfo(out);
  std::string theScoresHtml = HtmlInterpretation::ToStringUserScores();
  std::string theDebugHtml = HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  out << "<problemNavigation>" << thePage.ToStringProblemNavigation() << "</problemNavigation>";
  out << theScoresHtml;
  out << theDebugHtml;
  out << "</body></html>";
  return out.str();
}

std::string HtmlInterpretation::GetAccountsPage(const std::string& hostWebAddressWithPort) {
  MacroRegisterFunctionWithName("WebWorker::GetAccountsPage");
  std::stringstream out;
  out << "<html>"
  << "<head>"
  << HtmlRoutines::GetCSSLinkCalculator("/")
  //<< HtmlRoutines::GetJavascriptSubmitMainInputIncludeCurrentFile()
  << "</head>"
  << "<body onload =\"loadSettings();\">\n";
  bool isOK = theGlobalVariables.flagLoggedIn && theGlobalVariables.UserDefaultHasAdminRights();
  std::string accountsPageBody;
  if (isOK) {
    accountsPageBody = HtmlInterpretation::GetAccountsPageBody(hostWebAddressWithPort);
  }
  CalculatorHTML thePage;
  thePage.LoadDatabaseInfo(out);
  out << "<problemNavigation>" << thePage.ToStringProblemNavigation() << "</problemNavigation>";
  if (!isOK) {
    out << "<b>Viewing accounts is allowed for logged-in admins only</b>"
    << "</body></html>";
    return out.str();
  }
  out << accountsPageBody;
//  out << "<calculatorNavigation>" << theGlobalVariables.ToStringNavigation() << "</calculatorNavigation>\n";
  out << HtmlInterpretation::ToStringCalculatorArgumentsHumanReadable();
  out << "</body></html>";
  return out.str();
}

std::string HtmlInterpretation::ToStringUserDetailsTable(
  bool adminsOnly, List<JSData>& theUsers, const std::string& hostWebAddressWithPort
) {
  MacroRegisterFunctionWithName("HtmlInterpretation::ToStringUserDetailsTable");
  if (!theGlobalVariables.flagDatabaseCompiled) {
    return "Compiled without database support. ";
  }
  std::stringstream out;
  //std::string userRole = adminsOnly ? "admin" : "student";
  bool flagFilterCourse = (!adminsOnly) && (theGlobalVariables.GetWebInput("filterAccounts") == "true");
  std::string currentCourse = HtmlRoutines::ConvertURLStringToNormal(
    theGlobalVariables.GetWebInput("courseHome"), false
  );
  if (flagFilterCourse) {
    out << "<br>Displaying only students in course: <b style =\"color:blue\">"
    << currentCourse << "</b>. "
    << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
    << "?request=accounts&"
    << theGlobalVariables.ToStringCalcArgsNoNavigation(nullptr)
    << "filterAccounts=false&"
    << "\">Show all. </a>"
    << "<br>";
  }
  UserCalculator currentUser;
  HashedList<std::string, MathRoutines::HashString> theSections;
  List<std::string> sectionDescriptions;
  List<List<std::string> > activatedAccountBucketsBySection;
  List<List<std::string> > preFilledLinkBucketsBySection;
  List<List<std::string> > nonActivatedAccountBucketsBySection;
  for (int i = 0; i < theUsers.size; i ++) {
    currentUser.LoadFromJSON(theUsers[i]);
    if (flagFilterCourse &&
      (currentUser.courseInDB != currentCourse || currentUser.instructorInDB != theGlobalVariables.userDefault.username)
    ) {
      continue;
    }
    if (!theSections.Contains(currentUser.sectionInDB)) {
      std::stringstream currentSectionInfo;
      currentSectionInfo << "<b>Section: </b>" << currentUser.sectionInDB
      << ", <b>Course: </b>" << currentUser.courseInDB
      << ", <b>Instructor: </b>" << currentUser.instructorInDB;
      theSections.AddOnTop(currentUser.sectionInDB);
      sectionDescriptions.AddOnTop(currentSectionInfo.str());
    }
  }
  theSections.QuickSortAscending(nullptr, &sectionDescriptions);
  activatedAccountBucketsBySection.SetSize(theSections.size);
  nonActivatedAccountBucketsBySection.SetSize(theSections.size);
  preFilledLinkBucketsBySection.SetSize(theSections.size);
  int numActivatedUsers = 0;
  for (int i = 0; i < theUsers.size; i ++) {
    currentUser.LoadFromJSON(theUsers[i]);
    if (currentUser.courseInDB.find('%') != std::string::npos) {
      out << "<b style = \"color:red\">Non-expected behavior: user: "
      << currentUser.username
      << "current course: "
      << currentUser.courseInDB
      << " contains the % symbol. </b><br>";
    }
    std::stringstream oneTableLineStream;
    oneTableLineStream << "<tr>"
    << "<td>" << currentUser.username << "</td>"
    << "<td>" << currentUser.email << "</td>";
    bool isActivated = true;
    std::string webAddress = "https://" + hostWebAddressWithPort + "/cgi-bin/calculator";
    if (currentUser.actualActivationToken != "activated" && currentUser.actualActivationToken != "error") {
      isActivated = false;
      numActivatedUsers ++;
      oneTableLineStream << "<td><span style =\"color:red\">not activated</span></td>";
      if (currentUser.actualActivationToken != "") {
        oneTableLineStream << "<td>"
        << "<a href=\""
        << UserCalculator::GetActivationAddressFromActivationToken
        (currentUser.actualActivationToken, hostWebAddressWithPort,
         currentUser.username, currentUser.email)
        << "\"> (Re)activate account and change password</a>"
        << "</td>";
      }
      oneTableLineStream << "<td>";
      oneTableLineStream
      << "<a href=\"mailto:" << currentUser.email
      << "?subject =Math 140 Homework account activation&";

      oneTableLineStream << "body =";
      std::stringstream emailBody;
      emailBody << "Dear user,\n you have not activated your homework server account yet. \n"
      << "To activate your account and set your password please use the link: "
      << HtmlRoutines::ConvertStringToURLString("\n\n", false)
      << HtmlRoutines::ConvertStringToURLString(UserCalculator::GetActivationAddressFromActivationToken(
          currentUser.actualActivationToken, hostWebAddressWithPort, currentUser.username, currentUser.email
        ), false
      )
      << HtmlRoutines::ConvertStringToURLString("\n\n", false)
      << " Once you activate your account, you can log in safely here: \n"
      << HtmlRoutines::ConvertStringToURLString("\n\n", false)
      << webAddress
      << HtmlRoutines::ConvertStringToURLString("\n\n", false)
      << "Best regards, \ncalculator-algebra.org.";
      oneTableLineStream << emailBody.str() << "\">Send email manually.</a> ";
      oneTableLineStream << "</td>";
    } else if (currentUser.actualActivationToken == "error") {
      oneTableLineStream << "<td>error</td><td></td>";
    } else {
      oneTableLineStream << "<td><span style =\"color:green\">activated</span></td><td></td><td></td>";
    }
    std::stringstream oneLink;
    oneLink << "<a href=\"" << theGlobalVariables.DisplayNameExecutable << "?request=login&username="
    << currentUser.username << "\">" << currentUser.username << "</a>";
    oneTableLineStream << "<td>" << oneLink.str() << "</td>";
    oneTableLineStream << "</tr>";
    int indexCurrentBucket = theSections.GetIndex(currentUser.sectionInDB);
    if (indexCurrentBucket != - 1) {
      if (isActivated) {
        activatedAccountBucketsBySection[indexCurrentBucket].AddOnTop(oneTableLineStream.str());
      } else {
        nonActivatedAccountBucketsBySection[indexCurrentBucket].AddOnTop(oneTableLineStream.str());
      }
      preFilledLinkBucketsBySection[indexCurrentBucket].AddOnTop(oneLink.str());
    }
  }
  for (int i = 0; i < nonActivatedAccountBucketsBySection.size; i ++) {
    nonActivatedAccountBucketsBySection[i].QuickSortAscending();
  }
  for (int i = 0; i < activatedAccountBucketsBySection.size; i ++) {
    activatedAccountBucketsBySection[i].QuickSortAscending();
  }
  for (int i = 0; i < preFilledLinkBucketsBySection.size; i ++) {
    preFilledLinkBucketsBySection[i].QuickSortAscending();
  }
  std::stringstream tableStream;
  tableStream << "<table><tr><th>User</th><th>Email</th><th>Activated?</th><th>Activation link</th>"
  << "<th>Activation manual email</th>"
  << " <th>Pre-filled login link</th><th>Course info</th></tr>";
  for (int i = 0; i < nonActivatedAccountBucketsBySection.size; i ++) {
    if (!adminsOnly) {
      if (nonActivatedAccountBucketsBySection[i].size > 0) {
        tableStream << "<tr><td colspan =\"6\" style =\"text-align:center\">" << theSections[i] << "</td></tr>";
      }
    }
    for (int j = 0; j < nonActivatedAccountBucketsBySection[i].size; j ++) {
      tableStream << nonActivatedAccountBucketsBySection[i][j];
    }
  }
  for (int i = 0; i < activatedAccountBucketsBySection.size; i ++) {
    if (!adminsOnly) {
      if (activatedAccountBucketsBySection[i].size > 0) {
        tableStream << "<tr><td colspan =\"7\" style =\"text-align:center\">"
        << theSections[i] << "</td></tr>";
      }
    }
    for (int j = 0; j < activatedAccountBucketsBySection[i].size; j ++) {
      tableStream << activatedAccountBucketsBySection[i][j];
    }
  }
  tableStream << "</table>";
  std::stringstream preFilledLoginLinks;
  if (!adminsOnly) {
    for (int i = 0; i < preFilledLinkBucketsBySection.size; i ++) {
      if (preFilledLinkBucketsBySection[i].size > 0) {
        preFilledLoginLinks << theSections[i] << "<br>";
      }
      for (int j = 0; j < preFilledLinkBucketsBySection[i].size; j ++) {
        preFilledLoginLinks << preFilledLinkBucketsBySection[i][j] << "<br>";
      }
    }
  }
  out << "\n" << theUsers.size << " user(s)";
  if (numActivatedUsers > 0) {
    out << ", <span style =\"color:red\">" << numActivatedUsers
    << " have not activated their accounts. </span>";
  }
  out << tableStream.str() << preFilledLoginLinks.str();
  return out.str();
}

std::string HtmlInterpretation::ToStringAssignSection() {
  MacroRegisterFunctionWithName("HtmlInterpretation::ToStringAssignSection");
  std::stringstream out;
  std::string idAddressTextarea = "inputSetTeacher";
  std::string idExtraTextarea = "inputSections";
  std::string idOutput = "idOutputSections";
  out << "Assign section(s) to teacher(s)<br> ";
  out << "<textarea width =\"500px\" ";
  out << "id =\"" << idAddressTextarea << "\"";
  out << "placeholder =\"email or user list, comma, space or ; separated\">";
  out << "</textarea>";
  out << "<textarea width =\"500px\" ";
  out << "id =\"" << idExtraTextarea << "\"";
  out << " placeholder =\"list of sections\">";
  out << "</textarea>";
  out << "<br>";
  out
  << "<button class =\"normalButton\" onclick=\"submitStringAsMainInput("
  << "'teachers ='+ "
  << "encodeURIComponent(document.getElementById('" << idAddressTextarea << "').value)"
  << " + '&sections =' + "
  << "encodeURIComponent(document.getElementById('" << idExtraTextarea << "').value),"
  << "'" << idOutput << "',"
  << " 'setTeacher', null, '" << idOutput << "'"
  << " )\"> Set teacher</button> ";
  out << "<br><span id =\"" << idOutput << "\">\n";
  out << "</span>";
  return out.str();
}

extern logger logWorker;
int ProblemData::getExpectedNumberOfAnswers(const std::string& problemName, std::stringstream& commentsOnFailure) {
  MacroRegisterFunctionWithName("ProblemData::getExpectedNumberOfAnswers");
  if (this->knownNumberOfAnswersFromHD != - 1) {
    return this->knownNumberOfAnswersFromHD;
  }
  if (theGlobalVariables.problemExpectedNumberOfAnswers.size() == 0) {
    JSData findProblemInfo;
    findProblemInfo.theType = JSData::token::tokenArray;
    List<JSData> result;
    List<std::string> fields;
    fields.AddOnTop(DatabaseStrings::labelProblemName);
    fields.AddOnTop(DatabaseStrings::labelProblemTotalQuestions);
    //logWorker << logger::yellow << "DEBUG: About to query db to find problem info." << logger::endL;
    if (DatabaseRoutinesGlobalFunctionsMongo::FindFromJSONWithProjection(
      DatabaseStrings::tableProblemInformation, findProblemInfo, result, fields, - 1, nullptr, &commentsOnFailure
    )) {
      for (int i = 0; i < result.size; i ++) {
        const std::string& currentProblemName = result[i][DatabaseStrings::labelProblemName].theString;
        if (currentProblemName == "") {
          continue;
        }
        const std::string& expectedNumberOfAnswersString = result[i][DatabaseStrings::labelProblemTotalQuestions].theString;
        if (expectedNumberOfAnswersString == "") {
          continue;
        }
        std::stringstream theStream(expectedNumberOfAnswersString);
        int numAnswers = - 1;
        theStream >> numAnswers;
        if (numAnswers == - 1) {
          continue;
        }
        theGlobalVariables.problemExpectedNumberOfAnswers.SetKeyValue(currentProblemName, numAnswers);
        //logWorker << logger::green << "DEBUG: problem: " << currentProblemName
        //<< " got number of answers from DB: " << numAnswers;
      }
    }
  }
  if (theGlobalVariables.problemExpectedNumberOfAnswers.Contains(problemName)) {
    return theGlobalVariables.problemExpectedNumberOfAnswers.GetValueCreate(problemName);
  }
  logWorker << logger::yellow << "Couldn't find problem info in DB for: "
  << problemName << ", trying to read problem from hd. " << logger::endL;
  CalculatorHTML problemParser;
  problemParser.fileName = problemName;
  if (!problemParser.LoadMe(false, "", &commentsOnFailure)) {
    logWorker << logger::yellow << WebAPI::problem::failedToLoadProblem
    << commentsOnFailure.str() << logger::endL;
    return 0;
  }
  if (!problemParser.ParseHTML(&commentsOnFailure)) {
    logWorker << logger::red << "<b>Failed to parse file: " << problemParser.fileName
    << ".</b> Details:<br>" << commentsOnFailure.str();
    return 0;
  }
  this->knownNumberOfAnswersFromHD = problemParser.theProblemData.theAnswers.size();
  logWorker << logger::yellow << "Loaded problem: " << problemName
  << "; number of answers: " << this->knownNumberOfAnswersFromHD << logger::endL;
  this->expectedNumberOfAnswersFromDB = this->knownNumberOfAnswersFromHD;
  JSData newDBentry, findDBentry;
  findDBentry[DatabaseStrings::labelProblemName] = problemName;
  newDBentry[DatabaseStrings::labelProblemName] = problemName;
  std::stringstream stringConverter;
  stringConverter << this->knownNumberOfAnswersFromHD;
  newDBentry[DatabaseStrings::labelProblemTotalQuestions] = stringConverter.str();
  DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON(
    DatabaseStrings::tableProblemInformation, findDBentry, newDBentry, nullptr, &commentsOnFailure
  );
  return this->knownNumberOfAnswersFromHD;
}

void UserCalculator::ComputePointsEarned(
  const HashedList<std::string, MathRoutines::HashString>& gradableProblems,
  MapList<std::string, TopicElement, MathRoutines::HashString>* theTopics,
  std::stringstream& commentsOnFailure
) {
  MacroRegisterFunctionWithName("UserCalculator::ComputePointsEarned");
  this->pointsEarned = 0;
  this->pointsMax = 0;
  if (theTopics != nullptr) {
    for (int i = 0; i < theTopics->size(); i ++) {
      (*theTopics).theValues[i].totalPointsEarned = 0;
      (*theTopics).theValues[i].pointsEarnedInProblemsThatAreImmediateChildren = 0;
      (*theTopics).theValues[i].maxPointsInAllChildren = 0;
      (*theTopics).theValues[i].flagSubproblemHasNoWeight = false;
      //(*theTopics).theValues[i].maxCorrectAnswersInAllChildren = 0;
      //(*theTopics).theValues[i].numAnsweredInAllChildren = 0;
    }
  }
  for (int i = 0; i < this->theProblemData.size(); i ++) {
    const std::string problemName = this->theProblemData.theKeys[i];
    if (!gradableProblems.Contains(problemName)) {
      continue;
    }
    ProblemData& currentP = this->theProblemData.theValues[i];
    currentP.Points = 0;
    currentP.totalNumSubmissions = 0;
    currentP.numCorrectlyAnswered = 0;
    Rational currentWeight;
    currentP.flagProblemWeightIsOK =
    currentP.adminData.GetWeightFromCoursE(this->courseComputed, currentWeight);
    if (!currentP.flagProblemWeightIsOK) {
      currentWeight = 0;
      //stOutput << "Debug: weight not ok: " << problemName << "<br>";
    }
    for (int j = 0; j < currentP.theAnswers.size(); j ++) {
      if (currentP.theAnswers.theValues[j].numCorrectSubmissions > 0) {
        currentP.numCorrectlyAnswered ++;
      }
      currentP.totalNumSubmissions += currentP.theAnswers.theValues[j].numSubmissions;
    }
    if (currentP.flagProblemWeightIsOK) {
      int expectedNumberOfAnswers = currentP.getExpectedNumberOfAnswers(problemName, commentsOnFailure);
      if (expectedNumberOfAnswers > 0) {
        currentP.Points = (currentWeight * currentP.numCorrectlyAnswered) / expectedNumberOfAnswers;
        this->pointsEarned += currentP.Points;
      }
    }
    if (theTopics != nullptr) {
      if (theTopics->Contains(problemName)) {
        TopicElement& currentElt = theTopics->GetValueCreate(problemName);
        this->pointsMax += currentWeight;
        for (int j = 0; j < currentElt.parentTopics.size; j ++) {
          (*theTopics).theValues[currentElt.parentTopics[j]].totalPointsEarned += currentP.Points;
          (*theTopics).theValues[currentElt.parentTopics[j]].maxPointsInAllChildren += currentWeight;
          if (currentWeight == 0) {
            (*theTopics).theValues[currentElt.parentTopics[j]].flagSubproblemHasNoWeight = true;
          }
        }
        if (currentElt.parentTopics.size > 1) {
          (*theTopics).theValues[currentElt.parentTopics[currentElt.parentTopics.size - 2]].
            pointsEarnedInProblemsThatAreImmediateChildren += currentP.Points;
        }
      }
    }
  }
}

class UserScores {
public:
  CalculatorHTML theProblem;
  std::string currentSection;
  std::string currentCourse;
  List<MapList<std::string, Rational, MathRoutines::HashString> > scoresBreakdown;
  List<JSData> userProblemData;
  List<Rational> userScores;
  List<std::string> userInfos;
  List<std::string> userNames;
  List<LargeInteger> numStudentsSolvedEntireTopic;
  List<LargeInteger> numStudentsSolvedPartOfTopic;
  List<LargeInteger> numStudentsSolvedNothingInTopic;
  bool ComputeScoresAndStats(std::stringstream& comments);
};

bool UserScores::ComputeScoresAndStats(std::stringstream& comments) {
  MacroRegisterFunctionWithName("UserScores::ComputeScoresAndStats");
  if (!theGlobalVariables.flagDatabaseCompiled) {
    return false;
  }
  theProblem.currentUseR.::UserCalculatorData::operator=(theGlobalVariables.userDefault);
  this->theProblem.LoadFileNames();
  if (!this->theProblem.LoadAndParseTopicList(comments)) {
    return false;
  }
  if (!this->theProblem.PrepareSectionList(comments)) {
    return false;
  }
  if (!this->theProblem.LoadDatabaseInfo(comments)) {
    comments << "<span style =\"color:red\">Could not load your problem history.</span> <br>";
  }
  theProblem.currentUseR.ComputePointsEarned(theProblem.currentUseR.theProblemData.theKeys, &theProblem.theTopicS, comments);
  List<std::string> userLabels;
  int usernameIndex = userLabels.GetIndex(DatabaseStrings::labelUsername);
  if (usernameIndex == - 1) {
    comments << "Could not find username column. ";
    return false;
  }
  int problemDataIndex = userLabels.GetIndex(DatabaseStrings::labelProblemDataJSON);
  if (problemDataIndex == - 1) {
    comments << "Could not find problem data column. ";
    return false;
  }
  CalculatorHTML currentUserRecord;
  this->userScores.Reserve(this->userProblemData.size);
  this->userNames.Reserve(this->userProblemData.size);
  this->userInfos.Reserve(this->userProblemData.size);
  this->scoresBreakdown.Reserve(this->userProblemData.size);
  this->userScores.SetSize(0);
  this->userNames.SetSize(0);
  this->userInfos.SetSize(0);
  this->scoresBreakdown.SetSize(0);
  this->numStudentsSolvedEntireTopic.initializeFillInObject(this->theProblem.theTopicS.size(), 0);
  this->numStudentsSolvedPartOfTopic.initializeFillInObject(this->theProblem.theTopicS.size(), 0);
  this->numStudentsSolvedNothingInTopic.initializeFillInObject(this->theProblem.theTopicS.size(), 0);
  bool ignoreSectionsIdontTeach = true;
  this->currentSection = theGlobalVariables.userDefault.sectionComputed;
  this->currentCourse = theGlobalVariables.GetWebInput("courseHome");
  if (theGlobalVariables.GetWebInput("request") == "scoresInCoursePage") {
    this->currentSection = StringRoutines::StringTrimWhiteSpace(
      HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("mainInput"), false)
    );
  }
  for (int i = 0; i < this->userProblemData.size; i ++) {
    //currentUserRecord.currentUseR.courseInfo.rawStringStoredInDB = this->userTablE[i][courseInfoIndex];
    //currentUserRecord.currentUseR.AssignCourseInfoString(&comments);
    if (ignoreSectionsIdontTeach) {
      if (currentUserRecord.currentUseR.courseComputed != this->currentCourse) {
        continue;
      }
      if (theGlobalVariables.UserStudentVieWOn()) {
        if (currentUserRecord.currentUseR.sectionInDB != this->currentSection) {
          continue;
        }
      } else {
        if (currentUserRecord.currentUseR.sectionInDB != this->currentSection) {
          continue;
        }
      }
    }
    this->userScores.AddOnTop(- 1);
    this->userNames.AddOnTop(this->userProblemData[i][DatabaseStrings::labelUsername].theString);
    this->userInfos.AddOnTop(currentUserRecord.currentUseR.sectionInDB);
    this->scoresBreakdown.SetSize(this->scoresBreakdown.size + 1);
    currentUserRecord.currentUseR.username = this->userProblemData[i][DatabaseStrings::labelUsername].theString;
    if (!currentUserRecord.currentUseR.InterpretDatabaseProblemDataJSON(
      this->userProblemData[i][DatabaseStrings::labelProblemDataJSON],
      comments
    )) {
      continue;
    }
    currentUserRecord.LoadProblemInfoFromJSONAppend(
      theProblem.currentUseR.problemWeights, currentUserRecord.currentUseR.theProblemData, comments
    );
    currentUserRecord.currentUseR.ComputePointsEarned(theProblem.problemNamesNoTopics, &theProblem.theTopicS, comments);
    this->scoresBreakdown.LastObject()->Clear();
    for (int j = 0; j < theProblem.theTopicS.size(); j ++) {
      TopicElement& currentTopic = theProblem.theTopicS.theValues[j];
      Rational currentPts = currentTopic.totalPointsEarned;
      Rational maxPts = currentTopic.maxPointsInAllChildren;
      this->scoresBreakdown.LastObject()->SetKeyValue(theProblem.theTopicS.theKeys[j],currentPts);
      if (maxPts == currentPts) {
        this->numStudentsSolvedEntireTopic[j] ++;
      } else if (currentPts > 0) {
        this->numStudentsSolvedPartOfTopic[j] ++;
      } else {
        this->numStudentsSolvedNothingInTopic[j] ++;
      }
    }
    *this->userScores.LastObject() = currentUserRecord.currentUseR.pointsEarned;
  }
  return true;
}

std::string HtmlInterpretation::GetScoresInCoursePage() {
  MacroRegisterFunctionWithName("WebWorker::GetScoresInCoursePage");
  if (!theGlobalVariables.UserDefaultHasAdminRights()) {
    return "Only admins are allowed to view student scores.";
  }
  std::stringstream out;
  out.precision(4);
  UserScores theScores;
  if (!theScores.ComputeScoresAndStats(out)) {
    return out.str();
  }
  out << "Section: " << theScores.currentSection << ". ";
  out << "<script type =\"text/javascript\">\n";
  out << "studentScoresInHomePage = new Array("
  << theScores.theProblem.theTopicS.size() << ");\n";
  for (int i = 0; i < theScores.theProblem.theTopicS.size(); i ++) {
    TopicElement& currentElt = theScores.theProblem.theTopicS.theValues[i];
    out << "studentScoresInHomePage[" << i << "] = new Object;\n";
    if (currentElt.flagSubproblemHasNoWeight) {
      out << "studentScoresInHomePage[" << i << "].weightsOK = false;\n";
    } else {
      out << "studentScoresInHomePage[" << i << "].weightsOK = true;\n";
    }
    out << "studentScoresInHomePage[" << i << "].theId ="
    << "'"
    << currentElt.studentScoresSpanId
    << "';\n";
    out << "studentScoresInHomePage[" << i << "].numSolvedAll ="
    << theScores.numStudentsSolvedEntireTopic[i]
    << ";\n";
    out << "studentScoresInHomePage[" << i << "].numSolvedPart ="
    << theScores.numStudentsSolvedPartOfTopic[i]
    << ";\n";
    out << "studentScoresInHomePage[" << i << "].numSolvedNone ="
    << theScores.numStudentsSolvedNothingInTopic[i]
    << ";\n";
  }
  out << "</script>";
  return out.str();
}

std::string HtmlInterpretation::ToStringUserScores() {
  MacroRegisterFunctionWithName("HtmlInterpretation::ToStringUserScores");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return "only admins are allowed to view scores";
  if (!theGlobalVariables.flagDatabaseCompiled) {
    return "Error: database not running. ";
  }

  std::stringstream out;
  out.precision(4);
  UserScores theScores;
  if (!theScores.ComputeScoresAndStats(out)) {
    return out.str();
  }
//  out << "DBUG: prob names: " << theProblem.problemNamesNoTopics.ToStringCommaDelimited();
  out << "<b>Section: </b>" << theScores.currentSection
  << "<br><b>Course: </b>"
  << theScores.currentCourse << "\n<br>\n";
  out << "<table class =\"scoreTable\"><tr><th rowspan =\"3\">User</th>"
  << "<th rowspan =\"3\">Section</th><th rowspan =\"3\"> Total score</th>";
  for (int i = 0; i < theScores.theProblem.theTopicS.size(); i ++) {
    TopicElement& currentElt = theScores.theProblem.theTopicS.theValues[i];
    if (currentElt.problemFileName != "" || currentElt.type != currentElt.tChapter) {
      continue;
    }
    int numCols = currentElt.totalSubSectionsUnderMeIncludingEmptySubsections;
    out << "<td colspan =\"" << numCols << "\"";
    if (currentElt.totalSubSectionsUnderME == 0 && currentElt.flagContainsProblemsNotInSubsection) {
      out << " rowspan =\"3\"";
    }
    out << ">" << currentElt.title << "</td>";
  }
  out << "</tr>\n";
  out << "<tr>";
  for (int i = 0; i < theScores.theProblem.theTopicS.size(); i ++) {
    TopicElement& currentElt = theScores.theProblem.theTopicS.theValues[i];
    if (currentElt.problemFileName != "" || currentElt.type != currentElt.tSection) {
      continue;
    }
    int numCols = currentElt.totalSubSectionsUnderMeIncludingEmptySubsections;
    out << "<td colspan =\"" << numCols << "\"";
    if (currentElt.totalSubSectionsUnderME == 0 && currentElt.flagContainsProblemsNotInSubsection) {
      out << " rowspan =\"2\"";
    }
    out << ">" << currentElt.title << "</td>";
  }
  out << "</tr>\n";
  out << "<tr>";
  for (int i = 0; i < theScores.theProblem.theTopicS.size(); i ++) {
    TopicElement& currentElt = theScores.theProblem.theTopicS.theValues[i];
    if (
      currentElt.problemFileName == "" &&
      currentElt.type != currentElt.tProblem &&
      currentElt.type != currentElt.tSubSection &&
      currentElt.type != currentElt.tTexHeader
    ) {
      if ((
          currentElt.flagContainsProblemsNotInSubsection &&
          currentElt.totalSubSectionsUnderMeIncludingEmptySubsections > 1
        ) || currentElt.immediateChildren.size == 0
      ) {
        out << "<td></td>";
      }
      continue;
    }
    if (currentElt.problemFileName != "" || currentElt.type != currentElt.tSubSection) {
      continue;
    }
    out << "<td>" << currentElt.title << "</td>";
  }
  out << "</tr>\n";

  out << "<tr><td><b>Maximum score</b></td>"
  << "<td>-</td>";
  out
  << "<td>" << theScores.theProblem.currentUseR.pointsMax.GetDoubleValue()
  << "</td>";
  for (int j = 0; j < theScores.theProblem.theTopicS.size(); j ++) {
    TopicElement& currentElt = theScores.theProblem.theTopicS.theValues[j];
    if (currentElt.problemFileName != "") {
      continue;
    }
    if (currentElt.type != currentElt.tSubSection && !currentElt.flagContainsProblemsNotInSubsection) {
      continue;
    }
    out << "<td>" << currentElt.maxPointsInAllChildren << "</td>";
  }
  out << "</tr>";
  for (int i = 0; i < theScores.userInfos.size; i ++) {
    out << "<tr><td>" << theScores.userNames[i] << "</td>"
    << "<td>" << theScores.userInfos[i] << "</td>"
    << "<td>" << theScores.userScores[i].GetDoubleValue() << "</td>";
    for (int j = 0; j < theScores.theProblem.theTopicS.size(); j ++) {
      TopicElement& currentElt = theScores.theProblem.theTopicS.theValues[j];
      if (currentElt.problemFileName != "") {
        continue;
      }
      if (currentElt.type != currentElt.tSubSection && !currentElt.flagContainsProblemsNotInSubsection) {
        continue;
      }
      if (theScores.scoresBreakdown[i].Contains(theScores.theProblem.theTopicS.theKeys[j])) {
        out << "<td>" << theScores.scoresBreakdown[i].theValues[j].GetDoubleValue() << "</td>";
      } else {
        out << "<td></td>";
      }
    }
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

std::string HtmlInterpretation::ToStringUserDetails(
  bool adminsOnly, List<JSData>& theUsers, const std::string& hostWebAddressWithPort
) {
  MacroRegisterFunctionWithName("HtmlInterpretation::ToStringUserDetails");
  std::stringstream out;
  if (!theGlobalVariables.flagDatabaseCompiled) {
    out << "<b>Adding emails not available (database not present).</b> ";
    return out.str();
  }
  std::string userRole = adminsOnly ? "admin" : "student";
  std::string idAddressTextarea = "inputAddUsers" + userRole;
  std::string idExtraTextarea = "inputAddExtraInfo" + userRole;
  std::string idOutput = "idOutput" + userRole;
  std::string idPasswordTextarea = "inputAddDefaultPasswords" + userRole;
  out << "<ul><li>Add <b>" << userRole << "(s)</b> here.</li> ";
  out << "<li>Added/updated users will have their current course set to: <br>"
  << "<span class =\"currentCourseIndicator\">"
  << HtmlRoutines::ConvertURLStringToNormal(theGlobalVariables.GetWebInput("courseHome"), false)
  << "</span></li>"
  << "<li>To change course use the select course link in the top panel.</li>"
  << "<li>List users with a comma/space bar/new line/tab/semicolumn separated list. </li>"
  << "<li>List default passwords with a similarly separated list.</li> "
  << "<li>If left blank, password(s) are not (re)set.</li> "
  << "<li>If the password entries are not left blank and the number of passwords "
  << "does not match the number of added users, the operation will fail (no users will be added).</li> "
  << "</ul>\n";
  out << "<textarea width =\"500px\" ";
  out << "id =\"" << idAddressTextarea << "\"";
  out << "placeholder =\"user list, comma, space or ; separated\">";
  out << "</textarea>";
  out << "<textarea width =\"500px\" ";
  out << "id =\"" << idPasswordTextarea << "\"";
  out << " placeholder =\"default passwords\">";
  out << "</textarea>";
  out << "<textarea width =\"500px\" ";
  out << "id =\"" << idExtraTextarea << "\"";
  out << " placeholder =\"section/class #\">";
  out << "</textarea>";
  out << "<br>";
  out
  << "<button class =\"normalButton\" onclick=\"addEmailsOrUsers("
  << "'"    << idAddressTextarea
  << "', '"
  << "', '" << idOutput
  << "', '" << userRole
  << "', '" << idExtraTextarea
  << "', '" << idPasswordTextarea
  << "', 'addUsers'"
  << " )\"> Add users</button> ";
  out << "<br><span id =\"" << idOutput << "\">\n";
  out << HtmlInterpretation::ToStringUserDetailsTable(adminsOnly, theUsers, hostWebAddressWithPort);
  out << "</span>";
  return out.str();
}
