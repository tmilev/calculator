// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_html_interpretation.h"
#include "web_api.h"
#include "database.h"
#include <iomanip>
#include "string_constants.h"

JSData WebAPIResponse::GetProblemSolutionJSON() {
  MacroRegisterFunctionWithName("WebAPIReponse::GetProblemSolution");
  if (!global.UserDefaultHasAdminRights()) {
    global.theResponse.DisallowReport();
  }
  int64_t startMilliseconds = global.GetElapsedMilliseconds();
  CalculatorHTML theProblem;
  std::stringstream out, errorStream;
  JSData result;
  theProblem.LoadCurrentProblemItem(false, global.GetWebInput("randomSeed"), &errorStream);
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
  if (global.GetWebInput("randomSeed") == "") {
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
  MapList<std::string, std::string, MathRoutines::HashString>& theArgs = global.webArguments;
  for (int i = 0; i < theArgs.size(); i ++) {
    StringRoutines::StringBeginsWith(theArgs.theKeys[i], "calculatorAnswer", &lastStudentAnswerID);
  }
  int indexLastAnswerId = theProblem.GetAnswerIndex(lastStudentAnswerID);
  if (indexLastAnswerId == - 1) {
    out << "<b>Student submitted answerID: " << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "</b>";
    if (global.UserDebugFlagOn() && global.UserDefaultHasAdminRights()) {
      out << "<hr>" << theProblem.theProblemData.ToStringAvailableAnswerIds();
    }
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = global.GetElapsedMilliseconds() - startMilliseconds;
    return result;
  }
  Answer& currentA = theProblem.theProblemData.theAnswers.theValues[indexLastAnswerId];
  Calculator theInterpreteR;
  theInterpreteR.initialize();
  theInterpreteR.flagPlotNoControls = true;
  theInterpreteR.flagWriteLatexPlots = false;
  if (!theProblem.PrepareCommands(&comments)) {
    out << "<b>Failed to prepare calculator commands.</b>"
    << "<br>Comments:<br>" << comments.str();
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
    out << "<b style = 'color:red'>Failed to compose the solution. "
    << "Likely there is a bug with the problem. </b>"
    << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.ToStringSyntacticStackHumanReadable(false, false);
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = global.GetElapsedMilliseconds() - startMilliseconds;
    return result;
  }
  if (theInterpreteR.flagAbortComputationASAP) {
    out << "<b style = 'color:red'>Failed to compose the solution. "
    << "Likely there is a bug with the problem. </b>"
    << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.outputString
    << theInterpreteR.outputCommentsString
    << "<hr>Input: <br>" << theInterpreteR.inputString;
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = global.GetElapsedMilliseconds() - startMilliseconds;
    return result;
  }
  if (!theProblem.InterpretProcessExecutedCommands(theInterpreteR, currentA.solutionElements, out)) {
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = global.GetElapsedMilliseconds() - startMilliseconds;
    return result;
  }
  for (int i = 0; i < currentA.solutionElements.size; i ++) {
    if (!currentA.solutionElements[i].IsHidden()) {
      out << currentA.solutionElements[i].ToStringInterpretedBody();
    }
  }
  if (global.UserDebugFlagOn() && global.UserDefaultHasAdminRights()) {
    out << "<hr>"
    << "<a href=\"" << global.DisplayNameExecutable
    << "?request=calculator&mainInput="
    << HtmlRoutines::ConvertStringToURLString(answerCommandsNoEnclosures.str(), false)
    << "\">Input link</a>"
    <<  "<br>" << theInterpreteR.outputString << "<hr>" << theInterpreteR.outputCommentsString;
  }
  result[WebAPI::result::resultHtml] = out.str();
  result[WebAPI::result::millisecondsComputation] = global.GetElapsedMilliseconds() - startMilliseconds;
  return result;
}

std::string WebAPIResponse::SetProblemWeight() {
  MacroRegisterFunctionWithName("WebAPIReponse::SetProblemWeight");
  if (!global.flagDatabaseCompiled) {
    return "Cannot modify problem weights (no database available)";
  }
  if (!global.UserDefaultHasAdminRights()) {
    return "<b>Only admins may set problem weights.</b>";
  }
  CalculatorHTML theProblem;
  std::string inputProblemInfo = HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("mainInput"), false);
  std::stringstream commentsOnFailure, out;
  if (theProblem.MergeProblemWeightAndStore(inputProblemInfo, &commentsOnFailure)) {
    out << "<b style = 'color:green'>Modified.</b>";
  } else {
    out << "<b style = 'color:red'>" << commentsOnFailure.str() << "</b>";
  }
  return out.str();
}

std::string WebAPIResponse::SetProblemDeadline() {
  MacroRegisterFunctionWithName("WebAPIReponse::SetProblemWeight");
  if (!global.flagDatabaseCompiled) {
    return "Cannot modify problem weights (no database available)";
  }
  if (!global.UserDefaultHasAdminRights()) {
    return "<b>Only admins may set problem weights.</b>";
  }
  CalculatorHTML theProblem;
  std::string inputProblemInfo = HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("mainInput"), false);
  std::stringstream commentsOnFailure, out;
  if (theProblem.MergeProblemDeadlineAndStore(inputProblemInfo, &commentsOnFailure)) {
    out << "<b style =\"color:green\">Modified. </b>";
  } else {
    out << "<b style =\"color:red\">" << commentsOnFailure.str() << "</b>";
  }
  return out.str();
}

std::string WebAPIResponse::GetSanitizedComment(
  const Expression& input, FormatExpressions& theFormat, bool& resultIsPlot
) {
  MacroRegisterFunctionWithName("WebAPIReponse::GetSanitizedComment");
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
  if (
    input.StartsWith(input.owner->opRulesOff()) ||
    input.StartsWith(input.owner->opRulesOn())
  ) {
    return "";
  }
  return input.ToString(&theFormat);
}

std::string WebAPIResponse::GetCommentsInterpretation(
  Calculator& theInterpreterWithAdvice, int indexShift, FormatExpressions& theFormat
) {
  MacroRegisterFunctionWithName("WebAPIReponse::GetCommentsInterpretation");
  std::stringstream out;
  theFormat.flagExpressionIsFinal = true;
  theFormat.flagIncludeExtraHtmlDescriptionsInPlots = false;
  theInterpreterWithAdvice.theObjectContainer.resetPlots();
  if (indexShift >= theInterpreterWithAdvice.theProgramExpression.size()) {
    return "";
  }
  const Expression& currentE = theInterpreterWithAdvice.theProgramExpression[indexShift][1];
  bool resultIsPlot = false;
  if (!currentE.StartsWith(theInterpreterWithAdvice.opEndStatement())) {
    out << WebAPIResponse::GetSanitizedComment(currentE, theFormat, resultIsPlot);
    return out.str();
  }
  std::string currentS;
  for (int i = 1; i < currentE.size(); i ++) {
    currentS = WebAPIResponse::GetSanitizedComment(currentE[i], theFormat,resultIsPlot);
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

JSData WebAPIResponse::SubmitAnswersPreviewJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::submitAnswersPreviewJSON");
  if (!global.UserDefaultHasAdminRights()) {
    global.theResponse.DisallowReport();
  }
  double startTime = global.GetElapsedSeconds();
  std::string lastStudentAnswerID;
  std::string lastAnswer;
  std::stringstream out, studentAnswerSream;
  MapList<std::string, std::string, MathRoutines::HashString>& theArgs =
  global.webArguments;
  JSData result;
  for (int i = 0; i < theArgs.size(); i ++) {
    if (StringRoutines::StringBeginsWith(
      theArgs.theKeys[i], "calculatorAnswer", &lastStudentAnswerID)
    ) {
      lastAnswer = "(" + HtmlRoutines::ConvertURLStringToNormal(theArgs.theValues[i], false) + "); ";
    }
  }
  studentAnswerSream << lastAnswer;
  out << "Your answer(s): \\(\\displaystyle " << lastAnswer << "\\)" << "\n<br>\n";
  CalculatorHTML theProblem;
  std::stringstream errorStream, comments;
  theProblem.LoadCurrentProblemItem(
    global.UserRequestRequiresLoadingRealExamData(),
    global.GetWebInput("randomSeed"),
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
    << "<br>Response time: " << global.GetElapsedSeconds() - startTime
    << " second(s). ";
    result[WebAPI::result::error] = errorStream.str();
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  }
  Answer& currentA = theProblem.theProblemData.theAnswers.theValues[indexLastAnswerId];
  if (!theProblem.PrepareCommands(&comments)) {
    errorStream << "Something went wrong while interpreting the problem file. ";
    if (global.UserDebugFlagOn() && global.UserDefaultHasAdminRights()) {
      errorStream << comments.str();
    }
    result[WebAPI::result::millisecondsComputation] = global.GetElapsedSeconds() - startTime;
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
    result[WebAPI::result::millisecondsComputation] = global.GetElapsedSeconds() - startTime;
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  } else if (theInterpreteR.flagAbortComputationASAP) {
    out << "<b style = 'color:red'>Failed to evaluate your answer, got:</b><br>"
    << theInterpreteR.outputString;
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = global.GetElapsedSeconds() - startTime;
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
  << "<a href=\"" << global.DisplayNameExecutable
  << "?request=calculator&mainInput="
  << HtmlRoutines::ConvertStringToURLString(calculatorInputStreamNoEnclosures.str(), false)
  << "\">Input link</a>";
  theInterpreterWithAdvice.Evaluate(calculatorInputStream.str());
  if (theInterpreterWithAdvice.syntaxErrors != "") {
    out << "<br><span style =\"color:red\"><b>"
    << "Something went wrong when parsing your answer "
    << "in the context of the current problem. "
    << "</b></span>";
    if (global.UserDefaultHasAdminRights()) {
      out
      << problemLinkStream.str()
      << theInterpreterWithAdvice.outputString << "<br>"
      << theInterpreterWithAdvice.outputCommentsString;
    }
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = global.GetElapsedSeconds() - startTime;
    return result;
  }
  if (theInterpreterWithAdvice.flagAbortComputationASAP) {
    out << "<br><b style = 'color:red'>"
    << "Something went wrong when interpreting your answer "
    << "in the context of the current problem. "
    << "</b>";
    if (global.UserDefaultHasAdminRights() && global.UserDebugFlagOn()) {
      out << "<br>Logged-in as administator with debug flag on => printing error details. "
      << theInterpreterWithAdvice.outputString << "<br>"
      << theInterpreterWithAdvice.outputCommentsString;
      out << "<hr><b>Calculator input:</b> "
      << problemLinkStream.str() << "<br>"
      << calculatorInputStream.str();
    }
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = global.GetElapsedSeconds() - startTime;
    return result;
  }
  if (hasCommentsBeforeSubmission){
    out << WebAPIResponse::GetCommentsInterpretation(theInterpreterWithAdvice, 3, theFormat);
  }
  result[WebAPI::result::millisecondsComputation] = global.GetElapsedSeconds() - startTime;
  if (global.UserDefaultHasAdminRights() && global.UserDebugFlagOn()) {
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

JSData WebAPIResponse::ClonePageResult() {
  MacroRegisterFunctionWithName("WebAPIReponse::ClonePageResult");
  JSData result;
  if (
    ! global.flagLoggedIn ||
    ! global.UserDefaultHasAdminRights() ||
    ! global.flagUsingSSLinCurrentConnection
  ) {
    result[WebAPI::result::error] = "Cloning problems allowed only for logged-in admins under ssl connection.";
    return result;
  }
  std::string fileNameTarget = HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput(WebAPI::problem::fileNameTarget), false);
  std::string fileNameToBeCloned = HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput(WebAPI::problem::fileName), false);
  std::stringstream out;
  std::string startingFileString;
  if (!FileOperations::LoadFileToStringVirtualCustomizedReadOnly(fileNameToBeCloned, startingFileString, &out)) {
    out << "Could not find input file: " << fileNameToBeCloned;
    result[WebAPI::result::error] = out.str();
    return result;
  }
  std::fstream theFile;
  if (FileOperations::FileExistsVirtualCustomizedReadOnly(fileNameTarget, nullptr)) {
    out << "Output file: " << fileNameTarget << " already exists. ";
    result[WebAPI::result::error] = out.str();
    return result;
  }
  if (!FileOperations::OpenFileVirtualCustomizedWriteOnlyCreateIfNeeded(
    theFile, fileNameTarget, false, false, false, &out
  )) {
    out << "Failed to open output file: " << fileNameTarget << ".";
    result[WebAPI::result::error] = out.str();
    return result;
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
  return result;
}

void BuilderApplication::BuildHtmlJSpage(bool appendBuildHash) {
  MacroRegisterFunctionWithName("WebAPIReponse::BuildHtmlJSpage");
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

bool BuilderApplication::FileNameAllowedToBeMissing(const std::string& input) {
  if (input == "/calculator-html/external/build/output-min.js") {
    return true;
  }
  return  false;
}

std::string WebAPIResponse::GetOnePageJS(bool appendBuildHash) {
  MacroRegisterFunctionWithName("WebAPIReponse::GetOnePageJS");
  BuilderApplication theInterpretation;
  std::stringstream out;
  std::stringstream errorStream;
  if (!FileOperations::LoadFileToStringVirtual(
    "/calculator-html/index.html", theInterpretation.htmlRaw, false, &errorStream
  )) {
    out << "<html><body><b>Failed to load the application file. </b>"
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
      if (!theInterpretation.FileNameAllowedToBeMissing(theInterpretation.jsFileNames[i])) {
        errorStream << "Failed to load javascript file: " << theInterpretation.jsFileNames[i];
        return errorStream.str();
      }
      std::stringstream moduleNotFound;
      moduleNotFound << "console.log(\"File '"
      << theInterpretation.jsFileNames[i] << "' not found. "
      << "Failure to find that file has been "
      << "specifically white-listed as ok. \");";
      theInterpretation.jsFileContents[i] = moduleNotFound.str();
    }
  }
  return theInterpretation.GetOnePageJSBrowserify();
}

std::string BuilderApplication::GetOnePageJSBrowserify() {
  MacroRegisterFunctionWithName("WebAPIReponse::GetOnePageJSBrowserify");
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

std::string WebAPIResponse::GetApp(bool appendBuildHash) {
  MacroRegisterFunctionWithName("WebAPIReponse::GetApp");
  BuilderApplication theInterpretation;
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

JSData Course::ToJSON() const {
  JSData result;
  result["title"] = this->title;
  result["courseHome"] = Configuration::courseTemplates + this->courseTemplate;
  result["topicList"] = Configuration::topicLists + this->courseTopicsNoFolder;
  if (this->flagRoughDraft != "") {
    result["roughDraft"] = this->flagRoughDraft;
  }
  return result;
}

std::string Course::courseTopicsWithFolder() {
  return Configuration::topicLists + this->courseTopicsNoFolder;
}

std::string Course::ToString() const {
  std::stringstream out;
  out << "Html: " << this->courseTemplate
  << "\n" << "Topics: " << this->courseTopicsNoFolder;
  return out.str();
}

bool Course::IsEmpty() {
  return this->courseTemplate == "" && this->courseTopicsNoFolder == "" && this->title == "";
}

void Course::reset() {
  this->courseTemplate = "";
  this->courseTopicsNoFolder = "";
  this->title = "";
  this->flagRoughDraft = "";
}

std::string CourseList::ToHtml() {
  return this->theCourses.ToString();
}

bool CourseList::LoadFromString(const std::string& input) {
  MacroRegisterFunctionWithName("CourseList::LoadFromString");
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
      if (current.courseTopicsNoFolder != "") {
        this->theCourses.AddOnTop(current);
        current.reset();
      }
      current.courseTopicsNoFolder = StringRoutines::StringTrimWhiteSpace(currentArgument);
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
  return true;
}

bool CourseList::Load() {
  std::string theTopicFile;
  std::stringstream commentsOnFailure;
  if (!FileOperations::LoadFileToStringVirtualCustomizedReadOnly(
    "/coursesavailable/default.txt", theTopicFile, &commentsOnFailure
  )) {
    commentsOnFailure << "Failed to fetch available courses from /coursesavailable/default.txt. ";
    this->errorMessage = commentsOnFailure.str();
    return false;
  }
  return this->LoadFromString(theTopicFile);
}

JSData CourseList::ToJSON() {
  JSData output;
  if (this->errorMessage != "") {
    output[WebAPI::result::error] = this->errorMessage;
  }
  output["courses"].theType = JSData::token::tokenArray;
  for (int i = 0; i < this->theCourses.size; i ++) {
    Course& currentCourse = this->theCourses[i];
    output["courses"].theList.AddOnTop(currentCourse.ToJSON());
  }
  return output;
}

JSData WebAPIResponse::GetSelectCourseJSON() {
  MacroRegisterFunctionWithName("WebAPIReponse::GetSelectCourseJSON");
  CourseList theCourses;
  theCourses.Load();
  return theCourses.ToJSON();
}

std::string WebAPIResponse::GetHtmlTagWithManifest() {
  MacroRegisterFunctionWithName("WebAPIReponse::GetHtmlTagWithManifest");
  std::stringstream out;
  out << "<!DOCTYPE HTML>\n";
  out << "<html>\n<!-- tag added automatically; user-specified html tag ignored-->\n";
  return out.str();
}

JSData WebAPIResponse::GetTopicTableJSON() {
  MacroRegisterFunctionWithName("WebAPIReponse::GetTopicTableJSON");
  std::stringstream out;
  CalculatorHTML thePage;
  std::stringstream comments;
  thePage.fileName = HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("courseHome"), false);
  thePage.topicListFileName = HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("topicList"), false);
  JSData result;
  if (!thePage.LoadAndParseTopicList(out)) {
    out << "Failed to load and parse topic list.";
    result[WebAPI::result::error] = out.str();
    return result;
  }
  if (!thePage.LoadMe(true, global.GetWebInput("randomSeed"), &comments)) {
    comments << "\"Failed to load file: "
    << global.GetWebInput("courseHome") << ""
    << "<br>Comments:<br> " << comments.str();
    comments << "\"";
    result[WebAPI::result::comments] = comments.str();
    return result;
  }
  thePage.ComputeTopicListAndPointsEarned(comments);
  return thePage.ToStringTopicListJSON();
}

void WebAPIResponse::GetJSDataUserInfo(JSData& outputAppend, const std::string& comments) {
  MacroRegisterFunctionWithName("WebAPIReponse::GetJSDataUserInfo");
  outputAppend["linkApp"] = WebAPIResponse::youHaveReachedTheBackend;
  outputAppend[WebAPI::result::loginDisabledEveryoneIsAdmin] = global.flagDisableDatabaseLogEveryoneAsAdmin;
  outputAppend[WebAPI::result::useFallbackDatabase] = !global.flagDatabaseCompiled;
  if (comments != "") {
    outputAppend[WebAPI::result::comments] = HtmlRoutines::ConvertStringToHtmlString(comments, false);
  }
  if (global.theResponse.MonitoringAllowed()) {
    outputAppend[WebAPI::UserInfo::processMonitoring] = "true";
    outputAppend[Configuration::millisecondsReplyAfterComputation] = static_cast<double>(global.millisecondsReplyAfterComputation);
  } else {
    outputAppend[WebAPI::UserInfo::processMonitoring] = "false";
  }
  if (global.GetWebInput(WebAPI::result::error) != "") {
    outputAppend[WebAPI::result::error] = HtmlRoutines::ConvertStringToHtmlString(global.GetWebInput(WebAPI::result::error), false);
  }
  if (!global.flagLoggedIn) {
    outputAppend[WebAPI::result::status] = "not logged in";
    return;
  }
  outputAppend[WebAPI::result::status] = "logged in";
  outputAppend[DatabaseStrings::labelUsername]            = HtmlRoutines::ConvertStringToHtmlString(global.userDefault.username                 , false);
  outputAppend[DatabaseStrings::labelAuthenticationToken] = HtmlRoutines::ConvertStringToHtmlString(global.userDefault.actualAuthenticationToken, false);
  outputAppend[DatabaseStrings::labelUserRole]            = HtmlRoutines::ConvertStringToHtmlString(global.userDefault.userRole                 , false);
  outputAppend[DatabaseStrings::labelInstructor]          = HtmlRoutines::ConvertStringToHtmlString(global.userDefault.instructorInDB           , false);
  outputAppend[DatabaseStrings::labelSection]             = HtmlRoutines::ConvertStringToHtmlString(global.userDefault.sectionInDB              , false);
  outputAppend[DatabaseStrings::labelCurrentCourses]      = HtmlRoutines::ConvertStringToHtmlString(global.userDefault.courseInDB               , false);
  outputAppend[DatabaseStrings::labelDeadlinesSchema]     = HtmlRoutines::ConvertStringToHtmlString(global.userDefault.deadlineSchema           , false);
  JSData sectionsTaught;
  sectionsTaught.theType = JSData::token::tokenArray;
  for (int i = 0; i < global.userDefault.sectionsTaught.size; i ++) {
    JSData nextSection;
    nextSection = HtmlRoutines::ConvertStringToHtmlString(global.userDefault.sectionsTaught[i], false);
    sectionsTaught.theList.AddOnTop(nextSection);
  }
  outputAppend[DatabaseStrings::labelSectionsTaught] = sectionsTaught;
}

JSData WebAPIResponse::GetJSONUserInfo(const std::string& comments) {
  MacroRegisterFunctionWithName("WebAPIReponse::GetJSONUserInfo");
  JSData output;
  WebAPIResponse::GetJSDataUserInfo(output, comments);
  return output;
}

std::string WebAPIResponse::GetJSONFromTemplate() {
  MacroRegisterFunctionWithName("WebAPIReponse::GetJSONFromTemplate");
  std::stringstream out;
  CalculatorHTML thePage;
  thePage.flagUseJSON = true;
  std::stringstream comments;
  global.Comments << "DEBUG: and course home is: " << global.GetWebInput("courseHome");
  thePage.fileName = HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("courseHome"), false);
  if (!thePage.LoadMe(true, global.GetWebInput("randomSeed"), &comments)) {
    out << "<b>Failed to load file: "
    << global.GetWebInput("courseHome") << ". </b>"
    << "<br>Comments:<br> " << comments.str();
    return out.str();
  }
  if (!thePage.InterpretHtml(&comments)) {
    out << "<b>Failed to interpret as template the following file: "
    << global.GetWebInput("courseHome") << ". </b>"
    << "<br>Comments:<br> " << comments.str();
    return out.str();
  }
  out << "<!-- File automatically generated from template: "
  << global.GetWebInput(WebAPI::problem::fileName)
  << ".-->\n";
  out << thePage.outputHtmlBodyNoTag;
  out << "<small>Generated in " << global.GetElapsedMilliseconds() << " ms. </small>";
  return out.str();
}

JSData WebAPIResponse::GetExamPageJSON() {
  MacroRegisterFunctionWithName("WebAPIReponse::GetExamPageJSON");
  std::stringstream out;
  JSData output;
  if (!global.flagLoggedIn && global.requestType == "scoredQuizJSON") {
    output[WebAPI::result::error] = "Scored quiz requires login";
    return output;
  }
  CalculatorHTML theFile;
  theFile.flagDoPrependProblemNavigationBar = false;
  theFile.flagUseJSON = true;
  std::stringstream errorAndDebugStream;
  std::string problemBody = theFile.LoadAndInterpretCurrentProblemItemJSON(
    global.UserRequestRequiresLoadingRealExamData(),
    global.GetWebInput("randomSeed"),
    &errorAndDebugStream
  );
  //<-must come after theFile.outputHtmlHeadNoTag
  errorAndDebugStream << theFile.outputDebugInformationBody;
  out << problemBody;
  std::string commentsProblem = errorAndDebugStream.str();
  output[WebAPI::problem::content] = HtmlRoutines::ConvertStringToURLString(out.str(), false);
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
  return output;
}

JSData WebAPIResponse::GetEditPageJSON() {
  MacroRegisterFunctionWithName("WebAPIReponse::GetEditPageJSON");
  JSData output;
  if (
    !global.flagLoggedIn ||
    !global.UserDefaultHasAdminRights()
  ) {
    output[WebAPI::result::error] = "Only logged-in admins are allowed to edit pages.";
    return output;
  }
  CalculatorHTML theFile;
  theFile.LoadFileNames();
  std::stringstream failureStream;
  if (!theFile.LoadMe(false, global.GetWebInput("randomSeed"), &failureStream)) {
    std::stringstream errorStream;
    errorStream << " <b>Failed to load file: "
    << theFile.fileName << ", perhaps the file does not exist. </b>"
    << failureStream.str();
    output[WebAPI::result::error] = errorStream.str();
    return output;
  }
  if (!theFile.ParseHTML(&failureStream)) {
    std::stringstream errorStream;
    errorStream << "<b>Failed to parse file: " << theFile.fileName
    << ".</b> Details:<br>" << failureStream.str();
    output[WebAPI::result::error] = errorStream.str();
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
    theAutocompleteKeyWords.AddOnTopNoRepetition(theFile.topics.knownTopicBundles.theKeys);
  }
  JSData theAutoCompleteWordsJS;
  theAutoCompleteWordsJS.theType = JSData::token::tokenArray;
  for (int i = 0; i < theAutocompleteKeyWords.size; i ++) {
    theAutoCompleteWordsJS[i] = theAutocompleteKeyWords[i];
  }
  output["autoComplete"] = theAutoCompleteWordsJS;
  output["content"] = HtmlRoutines::ConvertStringToURLString(theFile.inputHtml, false);
  return output;
}

JSData WebAPIResponse::SubmitAnswersJSON() {
  return WebAPIResponse::SubmitAnswersJSON(global.GetWebInput("randomSeed"), nullptr, true);
}

JSData WebAPIResponse::SubmitAnswersJSON(
  const std::string& inputRandomSeed, bool* outputIsCorrect, bool timeSafetyBrake
) {
  MacroRegisterFunctionWithName("WebAPIReponse::submitAnswers");
  if (!global.UserDefaultHasAdminRights()) {
    global.theResponse.DisallowReport();
  }

  std::stringstream output, errorStream, comments;
  JSData result;
  double startTime = global.GetElapsedSeconds();
  CalculatorHTML theProblem;
  theProblem.LoadCurrentProblemItem(
    global.UserRequestRequiresLoadingRealExamData(), inputRandomSeed, &errorStream
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
    global.fatal << "This shouldn't happen: empty file name: theProblem.fileName." << global.fatal;
  }
  std::string studentAnswerNameReader;
  theProblem.studentTagsAnswered.init(theProblem.theProblemData.theAnswers.size());
  MapList<std::string, std::string, MathRoutines::HashString>& theArgs = global.webArguments;
  int answerIdIndex = - 1;
  for (int i = 0; i < theArgs.size(); i ++) {
    if (StringRoutines::StringBeginsWith(
      theArgs.theKeys[i], "calculatorAnswer", &studentAnswerNameReader
    )) {
      int newAnswerIndex = theProblem.GetAnswerIndex(studentAnswerNameReader);
      if (answerIdIndex == - 1) {
        answerIdIndex = newAnswerIndex;
      } else if (
        answerIdIndex != newAnswerIndex &&
        answerIdIndex != - 1 &&
        newAnswerIndex != - 1
      ) {
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

  currentA.currentAnswerClean = HtmlRoutines::ConvertURLStringToNormal(
    currentA.currentAnswerURLed, false
  );
  currentA.currentAnswerURLed = HtmlRoutines::ConvertStringToURLString(
    currentA.currentAnswerClean, false
  );//<-encoding back to overwrite malformed input
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
  if (global.UserDebugFlagOn() && global.UserDefaultHasAdminRights()) {
    debugInputStream
    << global.ToStringCalculatorComputation(
      completedProblemStreamNoEnclosures.str(), "Input, no enclosures. "
    );
  }
  if (timeSafetyBrake) {
    global.millisecondsMaxComputation = global.GetElapsedMilliseconds() + 20000; // + 20 sec
  }
  Calculator theInterpreter;
  theInterpreter.initialize();
  theInterpreter.flagWriteLatexPlots = false;
  theInterpreter.flagPlotNoControls = true;

  theInterpreter.Evaluate(completedProblemStream.str());
  if (theInterpreter.flagAbortComputationASAP || theInterpreter.syntaxErrors != "") {
    if (theInterpreter.errorsPublic.str() != "") {
      output << "While checking your answer, got the error: "
      << "<br><b style =\"color:red\">"
      << theInterpreter.errorsPublic.str()
      << "</b> "
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
      global.millisecondsMaxComputation = global.GetElapsedMilliseconds() + 20000; //+20 sec
    }
    isolatedInterpreter.Evaluate("(" + currentA.currentAnswerClean + ")");
    if (isolatedInterpreter.syntaxErrors != "") {
      output << isolatedInterpreter.ToStringSyntacticStackHumanReadable(false, true);
    } else {
      output << isolatedInterpreter.outputString;
    }
    if (global.UserDebugFlagOn() && global.UserDefaultHasAdminRights()) {
      output << "<hr><b>Administartor view internals.</b><hr>"
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
    output << "<tr><td>";
    output << "<b style = 'color:red'>Your answer appears to be incorrect. </b>";
    output << "</td></tr>";
    if (global.UserDefaultHasAdminRights() && global.UserDebugFlagOn()) {
      output << "<tr><td>Administrator view internals. "
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
    output << "<tr><td>"
    << WebAPIResponse::GetCommentsInterpretation(theInterpreter, 3, theFormat)
    << "</td></tr>\n";
  }
  if (global.flagDatabaseCompiled) {
    UserCalculator& theUser = theProblem.currentUseR;
    theUser.::UserCalculatorData::operator=(global.userDefault);
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
        std::string theDeadlineString = theProblem.GetDeadline(
          theProblem.fileName,
          HtmlRoutines::ConvertStringToURLString(theSQLstring, false),
          unused
        );

        if (theDeadlineString == "" || theDeadlineString == " ") {
          hasDeadline = false;
        } else {
          TimeWrapper now, deadline; //<-needs a fix for different time formats.
          // <-For the time being, we hard-code it
          // to month/day/year format (no time to program it better).
          std::stringstream badDateStream;
          if (!deadline.AssignMonthDayYear(theDeadlineString, badDateStream)) {
            output << "<tr><td><b>Problem reading deadline. </b> The deadline string was: "
            << theDeadlineString << ". Comments: "
            << "<span style =\"color:red\">" << badDateStream.str() << "</span>"
            << "</td></tr><tr><td> This should not happen. "
            << CalculatorHTML::BugsGenericMessage << "</td></tr>";
            output << "</table>";
            result[WebAPI::result::resultHtml] = output.str();
            return result;
          }
          now.AssignLocalTime();
          // out << "Now: " << asctime (&now.theTime) << " mktime: " << mktime(&now.theTime)
          // << " deadline: " << asctime(&deadline.theTime) << " mktime: " << mktime(&deadline.theTime);
          secondsTillDeadline = deadline.SubtractAnotherTimeFromMeInSeconds(now) + 7 * 3600;
          deadLinePassed = (secondsTillDeadline < - 18000);
        }
      }
      if (deadLinePassed) {
        output << "<tr><td><b style =\"color:red\">"
        << "Deadline passed, attempt not recorded."
        << "</b></td></tr>";
      } else {
        currentA.numSubmissions ++;
        if ((*outputIsCorrect)) {
          currentA.numCorrectSubmissions ++;
          if (currentA.firstCorrectAnswerClean == "") {
            currentA.firstCorrectAnswerClean = currentA.currentAnswerClean;
          } else {
            output << "<tr><td>[first correct answer: "
            << currentA.firstCorrectAnswerClean << "]</td></tr>";
          }
        }
      }
    }
    if (theProblem.flagIsForReal) {
      std::stringstream comments;
      theUser.SetProblemData(theProblem.fileName, currentProblemData);
      if (!theUser.StoreProblemData(theProblem.fileName, &comments)) {
        output << "<tr><td><b>This shouldn't happen and may be a bug: "
        << "failed to store your answer in the database. "
        << CalculatorHTML::BugsGenericMessage
        << "</b><br>Comments: "
        << comments.str() << "</td></tr>";
      } else {
        output << "<tr><td>So far " << currentA.numCorrectSubmissions
        << " correct and "
        << currentA.numSubmissions - currentA.numCorrectSubmissions
        << " incorrect submissions.</td></tr>";
      }
      if (hasDeadline) {
        if (secondsTillDeadline < 0) {
          secondsTillDeadline *= - 1;
        }
        if (deadLinePassed) {
          output << "<tr><td><span style =\"color:red\"><b>Submission "
          << TimeWrapper::ToStringSecondsToDaysHoursSecondsString(
            secondsTillDeadline, false, false
          ) << " after deadline. </b></span></td></tr>";
        } else {
          output << "<tr><td><span style =\"color:green\"><b>Submission "
          << TimeWrapper::ToStringSecondsToDaysHoursSecondsString(
            secondsTillDeadline, false, false
          ) << " before deadline. </b></span></td></tr>";
        }
      } else {
        output << "<tr><td><b style =\"color:green\">"
        << "No deadline yet."
        << "</b></td></tr>";
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
    << "<hr><b>If you entered this expression through the "
    << "keyboard (without copying + pasting) this is a bug: "
    << "please report it to the web site administrator. "
    << "Don't forget to mention your keyboard/character setup. "
    << "Are you using the standard English keyboard? "
    << "Cyrillic, Chinese, etc. characters are not accepted. </b> "
    << "<hr><b style ='color:red'>Copying and pasting an answer "
    << "not computed by yourself is considered cheating "
    << "(example: answer from an online program for doing homework).</b>";
  }
  output << "</td></tr>";
  output << "</table>";
  result[WebAPI::result::resultHtml] = output.str();
  result[WebAPI::result::millisecondsComputation] = global.GetElapsedSeconds() - startTime;
  return result;
}

std::string WebAPIResponse::AddTeachersSections() {
  MacroRegisterFunctionWithName("WebAPIReponse::AddTeachersSections");
  std::stringstream out;
  if (!global.UserDefaultHasAdminRights() || !global.flagUsingSSLinCurrentConnection) {
    out << "<b>Only admins may assign sections to teachers.</b>";
    return out.str();
  }
  std::string input = HtmlRoutines::ConvertURLStringToNormal(
    global.GetWebInput("teachersAndSections"), false
  );
  JSData inputParsed;
  if (!inputParsed.readstring(input, &out)) {
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
  if (!global.flagDatabaseCompiled) {
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
    QueryExact findQuery(
      DatabaseStrings::tableUsers,
      DatabaseStrings::labelUsername,
      currentTeacher.username
    );
    QuerySet setQuery;
    setQuery.value[DatabaseStrings::labelSectionsTaught] = desiredSectionsList;
    if (!Database::get().UpdateOne(findQuery, setQuery, &out)) {
      out << "<span style =\"color:red\">Failed to store course info of instructor: " << theTeachers[i] << ". </span><br>";
    } else {
      out << "<span style =\"color:green\">Assigned " << theTeachers[i] << " to section(s): "
      << desiredSectionsList << "</span><br>";
    }
  }
  return out.str();
}

std::string WebAPIResponse::AddUserEmails(const std::string& hostWebAddressWithPort) {
  MacroRegisterFunctionWithName("WebAPIReponse::AddUserEmails");
  (void) hostWebAddressWithPort;
  std::stringstream out;
  if (
    !global.UserDefaultHasAdminRights() ||
    !global.flagUsingSSLinCurrentConnection
  ) {
    out << "<b>Only admins may add users, under ssl connection. </b>";
    return out.str();
  }
  std::string inputEmails = HtmlRoutines::ConvertURLStringToNormal(
    global.GetWebInput(WebAPI::request::userList), false
  );
  std::string userPasswords = HtmlRoutines::ConvertURLStringToNormal(
    global.GetWebInput(WebAPI::request::passwordList), false
  );
  std::string userGroup =
  StringRoutines::StringTrimWhiteSpace(HtmlRoutines::ConvertURLStringToNormal(
    global.GetWebInput(DatabaseStrings::labelSection), false
  ));
  std::string userRole = HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("userRole"), false);

  if (inputEmails == "") {
    out << "AddUserEmails failed: <b>No emails to add</b>";
    return out.str();
  }
  std::stringstream comments;
  bool sentEmails = true;
  bool doSendEmails = global.requestType == "sendEmails" ?  true : false;
  int numNewUsers = 0;
  int numUpdatedUsers = 0;
  bool createdUsers = Database::get().theUser.AddUsersFromEmails(
    inputEmails, userPasswords, userRole, userGroup, comments, numNewUsers, numUpdatedUsers
  );
  if (createdUsers) {
    out << "<span style =\"color:green\">Success: "
    << numNewUsers << " new users and " << numUpdatedUsers
    << " user updates. </span> User roles: " << userRole;
  } else
    out << "<b style = 'color:red'>Failed to add all users.</b>"
    << " Errors follow. <hr>"
    << comments.str() << "<hr>";
  if (doSendEmails) {
    if (sentEmails) {
      out << "<b style = 'color:green'>"
      << "Activation emails successfully sent. "
      << "</b>";
    } else {
      out << "<b style ='color:red'>"
      << "Failed to send all activation emails. "
      << "</b>";
    }
  }
  return out.str();
}

const std::string CalculatorHTML::BugsGenericMessage =
"Please take a screenshot, copy the link address and send those along "
"with a short explanation to the administrator of the web site. ";

JSData WebAPIResponse::GetAnswerOnGiveUp() {
  return WebAPIResponse::GetAnswerOnGiveUp(global.GetWebInput("randomSeed"));
}

JSData WebAPIResponse::GetAnswerOnGiveUp(
  const std::string& inputRandomSeed,
  std::string* outputNakedAnswer,
  bool* answerGenerationSuccess,
  bool doIncludeTimeStats
) {
  MacroRegisterFunctionWithName("CalculatorHTML::GetAnswerOnGiveUp");
  GlobalVariables::Response::StateMaintainer maintain(global.theResponse);
  if (!global.UserDefaultHasAdminRights()) {
    global.theResponse.DisallowReport();
  }
  if (outputNakedAnswer != nullptr) {
    *outputNakedAnswer = "";
  }
  JSData result;
  result[WebAPI::problem::answerGenerationSuccess] = "false";
  if (answerGenerationSuccess != nullptr) {
    *answerGenerationSuccess = false;
  }
  int64_t startTimeInMilliseconds = global.GetElapsedMilliseconds();
  CalculatorHTML theProblem;
  std::stringstream errorStream;
  theProblem.LoadCurrentProblemItem(false, inputRandomSeed, &errorStream);
  if (!theProblem.flagLoadedSuccessfully) {
    errorStream << "Problem name is: " << theProblem.fileName
    << " <b>Could not load problem, this may be a bug. "
    << CalculatorHTML::BugsGenericMessage << "</b>";
    result[WebAPI::result::error] = errorStream.str();
    return result;
  }
  if (theProblem.flagIsForReal) {
    errorStream << " <b>Not allowed to show answer "
    << "of a problem being tested for real. </b>";
    result[WebAPI::result::error] = errorStream.str();
    return result;
  }
  if (inputRandomSeed == "") {
    result[WebAPI::result::error] = "<b>I could not figure out "
    "the exercise problem (missing random seed). </b>";
    return result;
  }
  if (!theProblem.ParseHTMLPrepareCommands(&errorStream)) {
    errorStream << "<br><b>Problem preparation failed.</b>";
    result[WebAPI::result::error] = errorStream.str();
    return result;
  }
  std::string lastStudentAnswerID;
  MapList<std::string, std::string, MathRoutines::HashString>& theArgs = global.webArguments;
  for (int i = 0; i < theArgs.size(); i ++) {
    StringRoutines::StringBeginsWith(
      theArgs.theKeys[i],
      WebAPI::problem::calculatorAnswerPrefix,
      &lastStudentAnswerID
    );
  }
  int indexLastAnswerId = theProblem.GetAnswerIndex(lastStudentAnswerID);
  std::stringstream out;
  if (indexLastAnswerId == - 1) {
    errorStream << "File: "
    << theProblem.fileName
    << "<br><b>Student submitted answerID: " << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "</b>";
    if (global.UserDebugFlagOn() && global.UserDefaultHasAdminRights()) {
      errorStream << "<hr>" << theProblem.theProblemData.ToStringAvailableAnswerIds();
    }
    result[WebAPI::result::millisecondsComputation] = global.GetElapsedMilliseconds() - startTimeInMilliseconds;
    result[WebAPI::result::error] = errorStream.str();
    return result;
  }
  Answer& currentA = theProblem.theProblemData.theAnswers.theValues[indexLastAnswerId];
  if (currentA.commandsNoEnclosureAnswerOnGiveUpOnly == "") {
    out << "<b> Unfortunately there is no answer given for this "
    << "question (answerID: " << lastStudentAnswerID << ").</b>";
    if (global.UserDebugFlagOn() && global.UserDefaultHasProblemComposingRights()) {
      out << "<br>Answer status: " << currentA.ToString();
    }
    result[WebAPI::result::error] = out.str();
    return result;
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
    out << "<b style ='color:red'>Failed to evaluate the default answer. "
    << "Likely there is a bug with the problem. </b>";
    if (global.UserDefaultHasProblemComposingRights()) {
      out << global.ToStringCalculatorComputation(
        answerCommandsNoEnclosure.str(), "Calculator input no enclosures"
      );
    }
    out << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.ToStringSyntacticStackHumanReadable(false, false);
    result[WebAPI::result::resultHtml] = out.str();
    int64_t ellapsedTime = global.GetElapsedMilliseconds() - startTimeInMilliseconds;
    result[WebAPI::result::millisecondsComputation] = ellapsedTime;
    return result;
  }
  if (theInterpreteR.flagAbortComputationASAP) {
    out << "<b style = 'color:red'>Failed to evaluate the default answer. "
    << "Likely there is a bug with the problem. </b>";
    if (global.UserDefaultHasProblemComposingRights()) {
      out << global.ToStringCalculatorComputation(
        answerCommandsNoEnclosure.str(), "Calculator input no enclosures"
      );
    }
    out << "<br>" << CalculatorHTML::BugsGenericMessage << "<br>Details: <br>"
    << theInterpreteR.outputString
    << theInterpreteR.outputCommentsString
    << "<hr>Input: <br>" << theInterpreteR.inputString;
    int64_t ellapsedTime = global.GetElapsedMilliseconds() - startTimeInMilliseconds;
    result[WebAPI::result::millisecondsComputation] = ellapsedTime;
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  }
  FormatExpressions theFormat;
  theFormat.flagExpressionIsFinal = true;
  theFormat.flagIncludeExtraHtmlDescriptionsInPlots = false;
  theFormat.flagUseQuotes = false;
  theFormat.flagUseLatex = true;
  theFormat.flagUsePmatrix = true;
  bool isFirst = true;
  const Expression& currentE = theInterpreteR.theProgramExpression[
    theInterpreteR.theProgramExpression.size() - 1
  ][1];
  if (!currentE.StartsWith(theInterpreteR.opEndStatement())) {
    out << "\\(\\displaystyle " << currentE.ToString(&theFormat) << "\\)";
    if (outputNakedAnswer != nullptr) {
      *outputNakedAnswer = currentE.ToString(&theFormat);
    }
    result[WebAPI::problem::answerGenerationSuccess] = "true";
    if (answerGenerationSuccess != nullptr) {
      *answerGenerationSuccess = true;
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
        result[WebAPI::problem::answerGenerationSuccess] = "true";
        if (answerGenerationSuccess != nullptr) {
          *answerGenerationSuccess = true;
        }
      }
      isFirst = false;
    }
  }
  if (doIncludeTimeStats) {
    int64_t ellapsedTime = global.GetElapsedMilliseconds() - startTimeInMilliseconds;
    result[WebAPI::result::millisecondsComputation] = ellapsedTime;
  }
  if (global.UserDebugFlagOn() && global.UserDefaultHasAdminRights()) {
    out
    << "<hr>"
    << global.ToStringCalculatorComputation(
      answerCommandsNoEnclosure.str(),
      "Calculator input no enclosures"
    );
    out << theInterpreteR.outputString << "<hr>"
    << theInterpreteR.outputCommentsString
    << "<hr>" << global.ToStringCalculatorComputation(
      theInterpreteR.inputString, "Raw calculator input"
    );
  }
  result[WebAPI::result::resultHtml] = out.str();
  return result;
}

JSData WebAPIResponse::GetAccountsPageJSON(const std::string& hostWebAddressWithPort) {
  MacroRegisterFunctionWithName("WebAPIReponse::GetAccountsPageJSON");
  (void) hostWebAddressWithPort;
  JSData output;
  if (global.flagDisableDatabaseLogEveryoneAsAdmin) {
    output[WebAPI::result::error] = "Database disabled (cannot get accounts). ";
    return output;
  }
  if (
    !global.UserDefaultHasAdminRights() ||
    !global.flagLoggedIn ||
    !global.flagUsingSSLinCurrentConnection
  ) {
    output[WebAPI::result::error] = "Must be logged-in administrator over ssl.";
    return output;
  }
  std::stringstream commentsOnFailure;
  JSData findStudents;
  JSData findAdmins;
  List<JSData> students;
  List<JSData> admins;
  long long totalStudents;
  findStudents[DatabaseStrings::labelInstructor] = global.userDefault.username;
  findAdmins[DatabaseStrings::labelUserRole] = UserCalculator::Roles::administator;
  List<std::string> columnsToRetain;
  columnsToRetain.AddOnTop(DatabaseStrings::labelUsername);
  columnsToRetain.AddOnTop(DatabaseStrings::labelEmail);
  columnsToRetain.AddOnTop(DatabaseStrings::labelActivationToken);
  columnsToRetain.AddOnTop(DatabaseStrings::labelCurrentCourses);
  columnsToRetain.AddOnTop(DatabaseStrings::labelInstructor);
  columnsToRetain.AddOnTop(DatabaseStrings::labelSection);
  columnsToRetain.AddOnTop(DatabaseStrings::labelSemester);
  if (!Database::FindFromJSONWithProjection(
    DatabaseStrings::tableUsers,
    findStudents,
    students,
    columnsToRetain,
    - 1,
    &totalStudents,
    &commentsOnFailure
  )) {
    output["error"] = "Failed to load user info. Comments: " + commentsOnFailure.str();
    return output;
  }
  if (!Database::FindFromJSONWithProjection(
    DatabaseStrings::tableUsers,
    findAdmins,
    admins,
    columnsToRetain,
    - 1,
    nullptr,
    &commentsOnFailure
  )) {
    output["error"] = "Failed to load user info. Comments: " + commentsOnFailure.str();
    return output;
  }
  output["admins"] = admins;
  output["students"] = students;
  return output;
}

std::string WebAPIResponse::GetAccountsPageBody(const std::string& hostWebAddressWithPort) {
  MacroRegisterFunctionWithName("WebAPIReponse::GetAccountsPageBody");
  (void) hostWebAddressWithPort;
  if (!global.flagDatabaseCompiled) {
    return "Database not available. ";
  }
  std::stringstream out;
  if (
    !global.UserDefaultHasAdminRights() ||
    !global.flagLoggedIn ||
    !global.flagUsingSSLinCurrentConnection
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
  findStudents[DatabaseStrings::labelInstructor] = global.userDefault.username;
  findAdmins[DatabaseStrings::labelUserRole] = UserCalculator::Roles::administator;
  if (!Database::get().FindFromJSON(
    DatabaseStrings::tableUsers,
    findStudents,
    students,
    - 1,
    &totalStudents,
    &commentsOnFailure
  )) {
    out << "<b>Failed to load user info.</b> Comments: " << commentsOnFailure.str();
    return out.str();
  }
  if (!Database::get().FindFromJSON(
    DatabaseStrings::tableUsers,
    findAdmins,
    admins,
    - 1,
    nullptr,
    &commentsOnFailure
  )) {
    out << "<b>Failed to load user info.</b> Comments: "
    << commentsOnFailure.str();
    return out.str();
  }
  out << "<hr>";
  out << WebAPIResponse::ToStringAssignSection();
  out << "<hr>";
  out
  << WebAPIResponse::ToStringUserDetails(true, admins, hostWebAddressWithPort);
  out << "<hr>";
  out << WebAPIResponse::ToStringUserDetails(
    false, students, hostWebAddressWithPort
  );
  return out.str();
}

std::string WebAPIResponse::GetScoresPage() {
  MacroRegisterFunctionWithName("WebWorker::GetScoresPage");
  std::stringstream out;
  CalculatorHTML thePage;
  thePage.LoadDatabaseInfo(out);
  std::string theScoresHtml = WebAPIResponse::ToStringUserScores();
  out << theScoresHtml;
  return out.str();
}

std::string WebAPIResponse::ToStringUserDetailsTable(
  bool adminsOnly, List<JSData>& theUsers, const std::string& hostWebAddressWithPort
) {
  MacroRegisterFunctionWithName("WebAPIReponse::ToStringUserDetailsTable");
  if (!global.flagDatabaseCompiled) {
    return "Compiled without database support. ";
  }
  std::stringstream out;
  bool flagFilterCourse = (!adminsOnly) && (global.GetWebInput("filterAccounts") == "true");
  std::string currentCourse = HtmlRoutines::ConvertURLStringToNormal(
    global.GetWebInput("courseHome"), false
  );
  if (flagFilterCourse) {
    out << "<br>Displaying only students in course: <b style =\"color:blue\">"
    << currentCourse << "</b>. "
    << "<a href=\"" << global.DisplayNameExecutable
    << "?request=accounts&"
    << global.ToStringCalcArgsNoNavigation(nullptr)
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
    if (flagFilterCourse && (
      currentUser.courseInDB != currentCourse ||
      currentUser.instructorInDB != global.userDefault.username
    )) {
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
      << HtmlRoutines::ConvertStringToURLString(
        UserCalculator::GetActivationAddressFromActivationToken(
          currentUser.actualActivationToken,
          hostWebAddressWithPort,
          currentUser.username,
          currentUser.email
        ),
        false
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
    oneLink << "<a href=\"" << global.DisplayNameExecutable << "?request=login&username="
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
  << "<th>Pre-filled login link</th><th>Course info</th></tr>";
  for (int i = 0; i < nonActivatedAccountBucketsBySection.size; i ++) {
    if (!adminsOnly) {
      if (nonActivatedAccountBucketsBySection[i].size > 0) {
        tableStream << "<tr><td colspan =\"6\" style =\"text-align:center\">"
        << theSections[i] << "</td></tr>";
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
        preFilledLoginLinks << preFilledLinkBucketsBySection[i][j]
        << "<br>";
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

std::string WebAPIResponse::ToStringAssignSection() {
  MacroRegisterFunctionWithName("WebAPIReponse::ToStringAssignSection");
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
  out << "id =\"" << idExtraTextarea << "\" ";
  out << "placeholder =\"list of sections\">";
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

int ProblemData::getExpectedNumberOfAnswers(
  const std::string& problemName, std::stringstream& commentsOnFailure
) {
  MacroRegisterFunctionWithName("ProblemData::getExpectedNumberOfAnswers");
  if (this->knownNumberOfAnswersFromHD != - 1) {
    return this->knownNumberOfAnswersFromHD;
  }
  if (global.problemExpectedNumberOfAnswers.size() == 0) {
    JSData findProblemInfo;
    findProblemInfo.theType = JSData::token::tokenArray;
    List<JSData> result;
    List<std::string> fields;
    fields.AddOnTop(DatabaseStrings::labelProblemFileName);
    fields.AddOnTop(DatabaseStrings::labelProblemTotalQuestions);

    if (Database::FindFromJSONWithProjection(
      DatabaseStrings::tableProblemInformation,
      findProblemInfo,
      result,
      fields,
      - 1,
      nullptr,
      &commentsOnFailure
    )) {
      for (int i = 0; i < result.size; i ++) {
        const std::string& currentProblemName = result[i][
          DatabaseStrings::labelProblemFileName
        ].theString;
        if (currentProblemName == "") {
          continue;
        }
        const std::string& expectedNumberOfAnswersString = result[i][
          DatabaseStrings::labelProblemTotalQuestions
        ].theString;
        if (expectedNumberOfAnswersString == "") {
          continue;
        }
        std::stringstream theStream(expectedNumberOfAnswersString);
        int numAnswers = - 1;
        theStream >> numAnswers;
        if (numAnswers == - 1) {
          continue;
        }
        global.problemExpectedNumberOfAnswers.SetKeyValue(
          currentProblemName, numAnswers
        );
      }
    }
  }
  if (global.problemExpectedNumberOfAnswers.Contains(problemName)) {
    return global.problemExpectedNumberOfAnswers.GetValueCreate(problemName);
  }
  global << logger::yellow << "Couldn't find problem info in DB for: "
  << problemName << ", trying to read problem from hd. " << logger::endL;
  CalculatorHTML problemParser;
  problemParser.fileName = problemName;
  if (!problemParser.LoadMe(false, "", &commentsOnFailure)) {
    global << logger::yellow << WebAPI::problem::failedToLoadProblem
    << commentsOnFailure.str() << logger::endL;
    return 0;
  }
  if (!problemParser.ParseHTML(&commentsOnFailure)) {
    global << logger::red << "<b>Failed to parse file: "
    << problemParser.fileName
    << ".</b> Details:<br>" << commentsOnFailure.str();
    return 0;
  }
  this->knownNumberOfAnswersFromHD = problemParser.theProblemData.theAnswers.size();
  global << logger::yellow << "Loaded problem: " << problemName
  << "; number of answers: " << this->knownNumberOfAnswersFromHD << logger::endL;
  this->expectedNumberOfAnswersFromDB = this->knownNumberOfAnswersFromHD;
  JSData newDBentry;
  QueryExact findEntry(
    DatabaseStrings::tableProblemInformation,
    DatabaseStrings::labelProblemFileName,
    problemName
  );
  newDBentry[DatabaseStrings::labelProblemFileName] = problemName;
  std::stringstream stringConverter;
  stringConverter << this->knownNumberOfAnswersFromHD;
  newDBentry[DatabaseStrings::labelProblemTotalQuestions] = stringConverter.str();
  Database::get().UpdateOne(
    findEntry, newDBentry, &commentsOnFailure
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
  if (!global.flagDatabaseCompiled) {
    return false;
  }
  theProblem.currentUseR.::UserCalculatorData::operator=(global.userDefault);
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
  theProblem.currentUseR.ComputePointsEarned(
    theProblem.currentUseR.theProblemData.theKeys,
    &theProblem.topics.theTopics,
    comments
  );
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
  this->numStudentsSolvedEntireTopic.initializeFillInObject(this->theProblem.topics.theTopics.size(), 0);
  this->numStudentsSolvedPartOfTopic.initializeFillInObject(this->theProblem.topics.theTopics.size(), 0);
  this->numStudentsSolvedNothingInTopic.initializeFillInObject(this->theProblem.topics.theTopics.size(), 0);
  bool ignoreSectionsIdontTeach = true;
  this->currentSection = global.userDefault.sectionComputed;
  this->currentCourse = global.GetWebInput("courseHome");
  if (global.GetWebInput("request") == "scoresInCoursePage") {
    this->currentSection = StringRoutines::StringTrimWhiteSpace(
      HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("mainInput"), false)
    );
  }
  for (int i = 0; i < this->userProblemData.size; i ++) {
    //currentUserRecord.currentUseR.courseInfo.rawStringStoredInDB = this->userTablE[i][courseInfoIndex];
    //currentUserRecord.currentUseR.AssignCourseInfoString(&comments);
    if (ignoreSectionsIdontTeach) {
      if (currentUserRecord.currentUseR.courseComputed != this->currentCourse) {
        continue;
      }
      if (global.UserStudentVieWOn()) {
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
    currentUserRecord.currentUseR.username = this->userProblemData[i][
      DatabaseStrings::labelUsername
    ].theString;
    if (!currentUserRecord.currentUseR.InterpretDatabaseProblemDataJSON(
      this->userProblemData[i][DatabaseStrings::labelProblemDataJSON],
      comments
    )) {
      continue;
    }
    currentUserRecord.MergeProblemWeight(
      theProblem.currentUseR.problemWeights,
      currentUserRecord.currentUseR.theProblemData,
      false,
      &comments
    );
    currentUserRecord.currentUseR.ComputePointsEarned(
      theProblem.problemNamesNoTopics,
      &theProblem.topics.theTopics,
      comments
    );
    this->scoresBreakdown.LastObject()->Clear();
    for (int j = 0; j < theProblem.topics.theTopics.size(); j ++) {
      TopicElement& currentTopic = theProblem.topics.theTopics.theValues[j];
      Rational currentPts = currentTopic.totalPointsEarned;
      Rational maxPts = currentTopic.maxPointsInAllChildren;
      this->scoresBreakdown.LastObject()->SetKeyValue(
        theProblem.topics.theTopics.theKeys[j], currentPts
      );
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

std::string WebAPIResponse::GetScoresInCoursePage() {
  MacroRegisterFunctionWithName("WebWorker::GetScoresInCoursePage");
  if (!global.UserDefaultHasAdminRights()) {
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
  << theScores.theProblem.topics.theTopics.size() << ");\n";
  for (int i = 0; i < theScores.theProblem.topics.theTopics.size(); i ++) {
    TopicElement& currentElt = theScores.theProblem.topics.theTopics.theValues[i];
    out << "studentScoresInHomePage[" << i << "] = new Object;\n";
    if (currentElt.flagSubproblemHasNoWeight) {
      out << "studentScoresInHomePage[" << i << "].weightsOK = false;\n";
    } else {
      out << "studentScoresInHomePage[" << i << "].weightsOK = true;\n";
    }
    out << "studentScoresInHomePage[" << i << "].theId ="
    << "'"
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

std::string WebAPIResponse::ToStringUserScores() {
  MacroRegisterFunctionWithName("WebAPIReponse::ToStringUserScores");
  if (!global.UserDefaultHasAdminRights()) {
    return "only admins are allowed to view scores";
  }
  if (!global.flagDatabaseCompiled) {
    return "Error: database not running. ";
  }

  std::stringstream out;
  out.precision(4);
  UserScores theScores;
  if (!theScores.ComputeScoresAndStats(out)) {
    return out.str();
  }
  out << "<b>Section: </b>" << theScores.currentSection
  << "<br><b>Course: </b>"
  << theScores.currentCourse << "\n<br>\n";
  out << "<table class =\"scoreTable\"><tr><th rowspan =\"3\">User</th>"
  << "<th rowspan =\"3\">Section</th><th rowspan =\"3\"> Total score</th>";
  for (int i = 0; i < theScores.theProblem.topics.theTopics.size(); i ++) {
    TopicElement& currentElt = theScores.theProblem.topics.theTopics.theValues[i];
    if (
      currentElt.problemFileName != "" ||
      currentElt.type != TopicElement::types::chapter
    ) {
      continue;
    }
    int numCols = currentElt.totalSubSectionsUnderMeIncludingEmptySubsections;
    out << "<td colspan =\"" << numCols << "\"";
    if (
      currentElt.totalSubSectionsUnderME == 0 &&
      currentElt.flagContainsProblemsNotInSubsection
    ) {
      out << " rowspan =\"3\"";
    }
    out << ">" << currentElt.title << "</td>";
  }
  out << "</tr>\n";
  out << "<tr>";
  for (int i = 0; i < theScores.theProblem.topics.theTopics.size(); i ++) {
    TopicElement& currentElt = theScores.theProblem.topics.theTopics.theValues[i];
    if (currentElt.problemFileName != "" || currentElt.type != TopicElement::types::section) {
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
  for (int i = 0; i < theScores.theProblem.topics.theTopics.size(); i ++) {
    TopicElement& currentElt = theScores.theProblem.topics.theTopics.theValues[i];
    if (
      currentElt.problemFileName == "" &&
      currentElt.type != TopicElement::types::problem &&
      currentElt.type != TopicElement::types::topic &&
      currentElt.type != TopicElement::types::texHeader
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
    if (
      currentElt.problemFileName != "" ||
      currentElt.type != TopicElement::types::topic
    ) {
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
  for (int j = 0; j < theScores.theProblem.topics.theTopics.size(); j ++) {
    TopicElement& currentElt = theScores.theProblem.topics.theTopics.theValues[j];
    if (currentElt.problemFileName != "") {
      continue;
    }
    if (
      currentElt.type != TopicElement::types::topic &&
      !currentElt.flagContainsProblemsNotInSubsection
    ) {
      continue;
    }
    out << "<td>" << currentElt.maxPointsInAllChildren << "</td>";
  }
  out << "</tr>";
  for (int i = 0; i < theScores.userInfos.size; i ++) {
    out << "<tr><td>" << theScores.userNames[i] << "</td>"
    << "<td>" << theScores.userInfos[i] << "</td>"
    << "<td>" << theScores.userScores[i].GetDoubleValue() << "</td>";
    for (int j = 0; j < theScores.theProblem.topics.theTopics.size(); j ++) {
      TopicElement& currentElt = theScores.theProblem.topics.theTopics.theValues[j];
      if (currentElt.problemFileName != "") {
        continue;
      }
      if (
        currentElt.type != TopicElement::types::topic &&
        !currentElt.flagContainsProblemsNotInSubsection
      ) {
        continue;
      }
      if (theScores.scoresBreakdown[i].Contains(
        theScores.theProblem.topics.theTopics.theKeys[j]
      )) {
        out << "<td>"
        << theScores.scoresBreakdown[i].theValues[j].GetDoubleValue()
        << "</td>";
      } else {
        out << "<td></td>";
      }
    }
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

std::string WebAPIResponse::ToStringUserDetails(
  bool adminsOnly, List<JSData>& theUsers, const std::string& hostWebAddressWithPort
) {
  MacroRegisterFunctionWithName("WebAPIReponse::ToStringUserDetails");
  std::stringstream out;
  if (!global.flagDatabaseCompiled) {
    out << "<b>Adding emails not available (database not present).</b> ";
    return out.str();
  }
  std::string userRole = adminsOnly ? UserCalculator::Roles::administator : "student";
  std::string idAddressTextarea = "inputAddUsers" + userRole;
  std::string idExtraTextarea = "inputAddExtraInfo" + userRole;
  std::string idOutput = "idOutput" + userRole;
  std::string idPasswordTextarea = "inputAddDefaultPasswords" + userRole;
  out << "<ul><li>Add <b>" << userRole << "(s)</b> here.</li> ";
  out << "<li>Added/updated users will have their current course set to: <br>"
  << "<span class =\"currentCourseIndicator\">"
  << HtmlRoutines::ConvertURLStringToNormal(global.GetWebInput("courseHome"), false)
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
  out << WebAPIResponse::ToStringUserDetailsTable(adminsOnly, theUsers, hostWebAddressWithPort);
  out << "</span>";
  return out.str();
}
