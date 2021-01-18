// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_html_interpretation.h"
#include "web_api.h"
#include "database.h"
#include <iomanip>
#include "string_constants.h"

JSData WebAPIResponse::getProblemSolutionJSON() {
  MacroRegisterFunctionWithName("WebAPIReponse::getProblemSolutionJSON");
  if (!global.userDefaultHasAdminRights()) {
    global.theResponse.disallowReport();
  }
  int64_t startMilliseconds = global.getElapsedMilliseconds();
  CalculatorHTML problem;
  std::stringstream out, errorStream;
  JSData result;
  problem.loadCurrentProblemItem(false, global.getWebInput(WebAPI::problem::randomSeed), &errorStream);
  if (!problem.flagLoadedSuccessfully) {
    out << "Problem name is: " << problem.fileName
    << " <b>Could not load problem, this may be a bug. "
    << CalculatorHTML::bugsGenericMessage << "</b>";
    if (errorStream.str() != "") {
      out << " Comments: " << errorStream.str();
    }
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  }
  if (problem.flagIsForReal) {
    out << " <b>Not allowed to show answer of a problem being tested for real. </b>";
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  }
  if (global.getWebInput(WebAPI::problem::randomSeed) == "") {
    out << "<b>I could not figure out the exercise problem (missing random seed). </b>";
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  }
  std::stringstream comments;
  if (!problem.parseHTMLPrepareCommands(&comments)) {
    out << "<br><b>Failed to parse problem.</b> Comments: " << comments.str();
    result[WebAPI::result::resultHtml] = out.str();
    return result;

  }
  std::string lastStudentAnswerID;
  MapList<std::string, std::string, MathRoutines::hashString>& theArgs = global.webArguments;
  for (int i = 0; i < theArgs.size(); i ++) {
    StringRoutines::stringBeginsWith(theArgs.keys[i], WebAPI::problem::calculatorAnswerPrefix, &lastStudentAnswerID);
  }
  int indexLastAnswerId = problem.getAnswerIndex(lastStudentAnswerID);
  if (indexLastAnswerId == - 1) {
    out << "<b>Student submitted answerID: " << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "</b>";
    if (global.userDebugFlagOn() && global.userDefaultHasAdminRights()) {
      out << "<hr>" << problem.problemData.toStringAvailableAnswerIds();
    }
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = global.getElapsedMilliseconds() - startMilliseconds;
    return result;
  }
  Answer& answer = problem.problemData.answers.values[indexLastAnswerId];
  Calculator interpreter;
  interpreter.initialize();
  interpreter.flagPlotNoControls = true;
  interpreter.flagWriteLatexPlots = false;
  if (!problem.prepareCommands(&comments)) {
    out << "<b>Failed to prepare calculator commands.</b>"
    << "<br>Comments:<br>" << comments.str();
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  }
  if (answer.solutionElements.size == 0) {
    out << "<b>There is no solution given for this question (answerID: " << lastStudentAnswerID << ").</b>";
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  }
  std::stringstream answerCommands, answerCommandsNoEnclosures;
  answerCommands << Calculator::Atoms::commandEnclosure
  << "{}(" << answer.commandsBeforeAnswer << "); "
  << answer.commandsSolutionOnly;
  answerCommandsNoEnclosures
  << answer.commandsBeforeAnswerNoEnclosuresForDEBUGGING
  << answer.commandsSolutionOnly;
  interpreter.evaluate(answerCommands.str());
  if (interpreter.syntaxErrors != "") {
    out << "<b style = 'color:red'>Failed to compose the solution. "
    << "Likely there is a bug with the problem. </b>"
    << "<br>" << CalculatorHTML::bugsGenericMessage << "<br>Details: <br>"
    << interpreter.toStringSyntacticStackHTMLSimple();
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = global.getElapsedMilliseconds() - startMilliseconds;
    return result;
  }
  if (interpreter.flagAbortComputationASAP) {
    out << "<b style = 'color:red'>Failed to compose the solution. "
    << "Likely there is a bug with the problem. </b>"
    << "<br>" << CalculatorHTML::bugsGenericMessage << "<br>Details: <br>"
    << interpreter.outputString
    << interpreter.outputCommentsString
    << "<hr>Input: <br>" << interpreter.inputString;
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = global.getElapsedMilliseconds() - startMilliseconds;
    return result;
  }
  if (!problem.interpretProcessExecutedCommands(interpreter, answer.solutionElements, out)) {
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = global.getElapsedMilliseconds() - startMilliseconds;
    return result;
  }
  for (int i = 0; i < answer.solutionElements.size; i ++) {
    if (!answer.solutionElements[i].isHidden()) {
      out << answer.solutionElements[i].toStringInterpretedBody();
    }
  }
  if (global.userDebugFlagOn() && global.userDefaultHasAdminRights()) {
    out << "<hr>"
    << HtmlRoutines::getCalculatorComputationAnchorNewPage(
      answerCommandsNoEnclosures.str(), "Input link"
    ) << "<br>" << interpreter.outputString
    << "<hr>" << interpreter.outputCommentsString
    << "<hr><b>Raw command:</b> " << answer.commandsSolutionOnly;
  }
  result[WebAPI::result::resultHtml] = out.str();
  result[WebAPI::result::millisecondsComputation] = global.getElapsedMilliseconds() - startMilliseconds;
  return result;
}

std::string WebAPIResponse::setProblemWeight() {
  MacroRegisterFunctionWithName("WebAPIReponse::setProblemWeight");
  if (!global.flagDatabaseCompiled) {
    return "Cannot modify problem weights (no database available)";
  }
  if (!global.userDefaultHasAdminRights()) {
    return "<b>Only admins may set problem weights.</b>";
  }
  CalculatorHTML theProblem;
  std::string inputProblemInfo = HtmlRoutines::convertURLStringToNormal(global.getWebInput("mainInput"), false);
  std::stringstream commentsOnFailure, out;
  if (theProblem.mergeProblemWeightAndStore(inputProblemInfo, &commentsOnFailure)) {
    out << "<b style = 'color:green'>Modified.</b>";
  } else {
    out << "<b style = 'color:red'>" << commentsOnFailure.str() << "</b>";
  }
  return out.str();
}

std::string WebAPIResponse::setProblemDeadline() {
  MacroRegisterFunctionWithName("WebAPIReponse::setProblemDeadline");
  if (!global.flagDatabaseCompiled) {
    return "Cannot modify problem weights (no database available)";
  }
  if (!global.userDefaultHasAdminRights()) {
    return "<b>Only admins may set problem weights.</b>";
  }
  CalculatorHTML theProblem;
  std::string inputProblemInfo = HtmlRoutines::convertURLStringToNormal(global.getWebInput("mainInput"), false);
  std::stringstream commentsOnFailure, out;
  if (theProblem.mergeProblemDeadlineAndStore(inputProblemInfo, &commentsOnFailure)) {
    out << "<b style ='color:green'>Modified. </b>" << global.comments.getCurrentReset();
  } else {
    out << "<b style ='color:red'>" << commentsOnFailure.str() << "</b>";
  }
  return out.str();
}

std::string WebAPIResponse::getSanitizedComment(
  const Expression& input, FormatExpressions& theFormat, bool& resultIsPlot
) {
  MacroRegisterFunctionWithName("WebAPIReponse::getSanitizedComment");
  theFormat.flagUseQuotes = false;
  resultIsPlot = false;
  std::string theString;
  if (input.isOfType<std::string>(&theString)) {
    if (StringRoutines::stringBeginsWith(theString, "Approximations have been")) {
      return "";
    }
    return input.toString(&theFormat);
  }
  if (input.isOfType<Plot>()) {
    resultIsPlot = true;
    return input.toString(&theFormat);
  }
  if (input.hasType<Plot>()) {
    return "";
  }
  //<- expression has a partially drawn plot-> not displaying.
  if (input.owner == nullptr) {
    return "";
  }
  if (
    input.startsWith(input.owner->opRulesOff()) ||
    input.startsWith(input.owner->opRulesOn())
  ) {
    return "";
  }
  return input.toString(&theFormat);
}

std::string WebAPIResponse::getCommentsInterpretation(
  Calculator& interpreterWithAdvice, int indexShift, FormatExpressions& theFormat
) {
  MacroRegisterFunctionWithName("WebAPIReponse::getCommentsInterpretation");
  std::stringstream out;
  theFormat.flagExpressionIsFinal = true;
  theFormat.flagIncludeExtraHtmlDescriptionsInPlots = false;
  interpreterWithAdvice.theObjectContainer.resetPlots();
  if (indexShift >= interpreterWithAdvice.programExpression.size()) {
    return "";
  }
  const Expression& currentE = interpreterWithAdvice.programExpression[indexShift][1];
  bool resultIsPlot = false;
  if (!currentE.startsWith(interpreterWithAdvice.opCommandSequence())) {
    out << WebAPIResponse::getSanitizedComment(currentE, theFormat, resultIsPlot);
    return out.str();
  }
  std::string currentS;
  for (int i = 1; i < currentE.size(); i ++) {
    currentS = WebAPIResponse::getSanitizedComment(currentE[i], theFormat, resultIsPlot);
    if (StringRoutines::stringTrimWhiteSpace(currentS) == "") {
      continue;
    }
    out << currentS;
    if (i != currentE.size() - 1 && !resultIsPlot) {
      out << "<br>";
    }
  }
  return out.str();
}

JSData WebAPIResponse::submitAnswersPreviewJSON() {
  MacroRegisterFunctionWithName("WebAPIResponse::submitAnswersPreviewJSON");
  if (!global.userDefaultHasAdminRights()) {
    global.theResponse.disallowReport();
  }
  double startTime = global.getElapsedSeconds();
  std::string lastStudentAnswerID;
  std::string lastAnswer;
  std::stringstream out, studentAnswerSream;
  MapList<std::string, std::string, MathRoutines::hashString>& theArgs =
  global.webArguments;
  JSData result;
  for (int i = 0; i < theArgs.size(); i ++) {
    if (StringRoutines::stringBeginsWith(
      theArgs.keys[i], WebAPI::problem::calculatorAnswerPrefix, &lastStudentAnswerID)
    ) {
      lastAnswer = "(" + HtmlRoutines::convertURLStringToNormal(theArgs.values[i], false) + "); ";
    }
  }
  studentAnswerSream << lastAnswer;
  out << "Your answer(s): \\(\\displaystyle " << lastAnswer << "\\)" << "\n<br>\n";
  CalculatorHTML problem;
  std::stringstream errorStream, comments;
  problem.loadCurrentProblemItem(
    global.userRequestRequiresLoadingRealExamData(),
    global.getWebInput(WebAPI::problem::randomSeed),
    &errorStream
  );
  if (!problem.flagLoadedSuccessfully) {
    out << "<br><b>" << WebAPI::problem::failedToLoadProblem << "</b> ";
  }
  if (!problem.parseHTMLPrepareCommands(&comments)) {
    out << "<br><b>Failed to parse problem.</b> Comments: " << comments.str();
  }
  int indexLastAnswerId = problem.getAnswerIndex(lastStudentAnswerID);
  if (indexLastAnswerId == - 1) {
    errorStream << "<br>Student submitted answerID: " << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "<br>Response time: " << global.getElapsedSeconds() - startTime
    << " second(s). ";
    result[WebAPI::result::error] = errorStream.str();
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  }
  Answer& currentA = problem.problemData.answers.values[indexLastAnswerId];
  if (!problem.prepareCommands(&comments)) {
    errorStream << "Something went wrong while interpreting the problem file. ";
    if (global.userDebugFlagOn() && global.userDefaultHasAdminRights()) {
      errorStream << comments.str();
    }
    result[WebAPI::result::millisecondsComputation] = global.getElapsedSeconds() - startTime;
    result[WebAPI::result::error] = errorStream.str();
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  }
  Calculator interpreter;
  interpreter.flagUseLnInsteadOfLog = true;
  interpreter.initialize();
  interpreter.flagWriteLatexPlots = false;
  interpreter.flagPlotNoControls = true;
  std::stringstream studentAnswerWithComments;
  studentAnswerWithComments
  << Calculator::Atoms::commandEnclosure << "{}("
  << currentA.commandsCommentsBeforeInterpretatioN
  << ");"
  << studentAnswerSream.str();

  interpreter.evaluate(studentAnswerWithComments.str());
  if (interpreter.syntaxErrors != "") {
    errorStream << "<b style ='color:red'>Parsing failure:</b><br>"
    << interpreter.toStringSyntacticStackHTMLSimple();
    result[WebAPI::result::error] = errorStream.str();
    result[WebAPI::result::millisecondsComputation] = global.getElapsedSeconds() - startTime;
    result[WebAPI::result::resultHtml] = out.str();
    return result;
  } else if (interpreter.flagAbortComputationASAP) {
    out << "<b style = 'color:red'>Failed to evaluate your answer, got:</b><br>"
    << interpreter.outputString;
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = global.getElapsedSeconds() - startTime;
    return result;
  }
  FormatExpressions theFormat;
  theFormat.flagUseLatex = true;
  theFormat.flagUsePmatrix = true;
  const Expression& studentAnswerNoContextE =
  interpreter.programExpression[interpreter.programExpression.size() - 1];
  out << "<span style =\"color:magenta\"><b>Interpreting as:</b></span><br>";
  out << "\\(\\displaystyle "
  << studentAnswerNoContextE.toString(&theFormat) << "\\)";
  Calculator interpreterWithAdvice;
  interpreterWithAdvice.flagUseLnInsteadOfLog = true;
  interpreterWithAdvice.initialize();
  interpreterWithAdvice.flagWriteLatexPlots = false;
  interpreterWithAdvice.flagPlotNoControls = true;
  std::stringstream calculatorInputStream,
  calculatorInputStreamNoEnclosures;

  calculatorInputStream << Calculator::Atoms::commandEnclosure << "{}("
  << currentA.commandsBeforeAnswer << ");";
  calculatorInputStreamNoEnclosures
  << currentA.commandsBeforeAnswerNoEnclosuresForDEBUGGING;

  calculatorInputStream << Calculator::Atoms::commandEnclosure << "{}("
  << currentA.answerId << " = " << lastAnswer
  << ");";
  calculatorInputStreamNoEnclosures
  << currentA.answerId << " = " << lastAnswer << "";
  bool hasCommentsBeforeSubmission = (StringRoutines::stringTrimWhiteSpace(currentA.commandsCommentsBeforeSubmission) != "");
  if (hasCommentsBeforeSubmission) {
    calculatorInputStream << Calculator::Atoms::commandEnclosure << "{}("
    <<  currentA.commandsCommentsBeforeSubmission
    << ");";
    calculatorInputStreamNoEnclosures
    << currentA.commandsCommentsBeforeSubmission;
  }
  std::stringstream problemLinkStream;
  problemLinkStream
  << HtmlRoutines::getCalculatorComputationAnchorNewPage(
    calculatorInputStreamNoEnclosures.str(), "Input link"
  );
  interpreterWithAdvice.evaluate(calculatorInputStream.str());
  if (interpreterWithAdvice.syntaxErrors != "") {
    out << "<br><b style='color:red'>"
    << "Something went wrong when parsing your answer "
    << "in the context of the current problem. "
    << "</b>";
    if (global.userDefaultHasAdminRights()) {
      out
      << problemLinkStream.str()
      << interpreterWithAdvice.outputString << "<br>"
      << interpreterWithAdvice.outputCommentsString;
    }
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = global.getElapsedSeconds() - startTime;
    return result;
  }
  if (interpreterWithAdvice.flagAbortComputationASAP) {
    out << "<br><b style = 'color:red'>"
    << "Something went wrong when interpreting your answer "
    << "in the context of the current problem. "
    << "</b>";
    if (global.userDefaultHasAdminRights() && global.userDebugFlagOn()) {
      out << "<br>Logged-in as administator with debug flag on => printing error details. "
      << interpreterWithAdvice.outputString << "<br>"
      << interpreterWithAdvice.outputCommentsString;
      out << "<hr><b>Calculator input:</b> "
      << problemLinkStream.str() << "<br>"
      << calculatorInputStream.str();
    }
    result[WebAPI::result::resultHtml] = out.str();
    result[WebAPI::result::millisecondsComputation] = global.getElapsedSeconds() - startTime;
    return result;
  }
  if (hasCommentsBeforeSubmission){
    out << WebAPIResponse::getCommentsInterpretation(interpreterWithAdvice, 3, theFormat);
  }
  result[WebAPI::result::millisecondsComputation] = global.getElapsedSeconds() - startTime;
  if (global.userDefaultHasAdminRights() && global.userDebugFlagOn()) {
    out << "<hr> " << problemLinkStream.str()
    << "<br>"
    << calculatorInputStreamNoEnclosures.str()
    << "<br>"
    << "Result:<br>"
    << interpreterWithAdvice.outputString
    << "<br>" << interpreterWithAdvice.outputCommentsString
    << "<br>Parsed elements: " << problem.parser.toStringParsingStack(problem.content)
    ;
  }
  result[WebAPI::result::resultHtml] = out.str();
  return result;
}

JSData WebAPIResponse::clonePageResult() {
  MacroRegisterFunctionWithName("WebAPIReponse::clonePageResult");
  JSData result;
  if (
    ! global.flagLoggedIn ||
    ! global.userDefaultHasAdminRights() ||
    ! global.flagUsingSSLinCurrentConnection
  ) {
    result[WebAPI::result::error] = "Cloning problems allowed only for logged-in admins under ssl connection.";
    return result;
  }
  std::string fileNameTarget = HtmlRoutines::convertURLStringToNormal(global.getWebInput(WebAPI::problem::fileNameTarget), false);
  std::string fileNameToBeCloned = HtmlRoutines::convertURLStringToNormal(global.getWebInput(WebAPI::problem::fileName), false);
  std::stringstream out;
  std::string startingFileString;
  if (!FileOperations::loadFiletoStringVirtualCustomizedReadOnly(fileNameToBeCloned, startingFileString, &out)) {
    out << "Could not find input file: " << fileNameToBeCloned;
    result[WebAPI::result::error] = out.str();
    return result;
  }
  std::fstream theFile;
  if (FileOperations::fileExistsVirtualCustomizedReadOnly(fileNameTarget, nullptr)) {
    out << "Output file: " << fileNameTarget << " already exists. ";
    result[WebAPI::result::error] = out.str();
    return result;
  }
  if (!FileOperations::openFileVirtualCustomizedWriteOnlyCreateIfNeeded(
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
  if (!FileOperations::getPhysicalFileNameFromVirtualCustomizedReadOnly(
    fileNameTarget, fileNameNonVirtual, &comments
  )) {
    out << "Could not get physical file name from virtual. " << comments.str();
  } else {
    out << "Wrote " << startingFileString.size() << " bytes to file: " << fileNameTarget;
  }
  result[WebAPI::result::resultHtml] = out.str();
  return result;
}

void BuilderApplication::initializeTags(bool appendBuildHash) {
  this->allInOneJavascriptTagOriginal =
  "<script type=\"text/javascript\" src=\"" + WebAPI::request::onePageJS + "\"></script>";
  std::string virtualJavascriptFileName = WebAPI::request::onePageJS;
  if (appendBuildHash) {
    virtualJavascriptFileName = FileOperations::GetVirtualNameWithHash(WebAPI::request::onePageJS);
  }
  this->allInOneJavascriptTagDesired =
  "<script src=\"" +
  virtualJavascriptFileName +
  "\" onerror = 'errorLoadingScript(this);'></script>\n";
  this->calculatorCSSTagOriginal =
  "<link type=\"text/css\" rel=\"stylesheet\" href=\"" + WebAPI::request::calculatorCSS + "\">";
  std::string virtualCSSFileName = WebAPI::request::calculatorCSS;
  if (appendBuildHash) {
    virtualCSSFileName = FileOperations::GetVirtualNameWithHash(WebAPI::request::calculatorCSS);
  }
  this->calculatorCSSTagDesired =
  "<link type=\"text/css\" rel=\"stylesheet\" href=\"" +
  virtualCSSFileName +
  "\" onerror = 'errorLoadingScript(this);'>";
}

void BuilderApplication::buildHtmlJavascriptPage(bool appendBuildHash) {
  MacroRegisterFunctionWithName("WebAPIReponse::buildHtmlJavascriptPage");
  this->initializeTags(appendBuildHash);
  this->htmlJSbuild = this->htmlRaw;
  if (!StringRoutines::replaceOnce(
    this->htmlJSbuild, this->allInOneJavascriptTagOriginal, this->allInOneJavascriptTagDesired
  )) {
    global << Logger::red
    << "Failed to find javascript_all_in_one.js tag in the html data."
    << Logger::endL;
  }
  if (!StringRoutines::replaceOnce(
    this->htmlJSbuild, this->calculatorCSSTagOriginal, this->calculatorCSSTagDesired
  )) {
    global << Logger::red
    << "Failed to find javascript_all_in_one.js tag in the html data."
    << Logger::endL;
  }
}

bool BuilderApplication::fileNameAllowedToBeMissing(const std::string& input) {
  if (input == "/calculator-html/external/build/output-min.js") {
    return true;
  }
  return  false;
}

std::string WebAPIResponse::getOnePageJS() {
  MacroRegisterFunctionWithName("WebAPIReponse::getOnePageJS");
  BuilderApplication builder;
  std::stringstream errorStream;
  if (!builder.loadJavascriptFileNames("/calculator-html/BUILD.json", &errorStream)) {
    errorStream << "<b>Failed to load the BUILD.json list of javascript sources.</b>";
    return errorStream.str();
  }
  builder.jsFileContents.setSize(builder.jsFileNames.size);
  for (int i = 0; i < builder.jsFileNames.size; i ++) {
    if (!FileOperations::loadFileToStringVirtual(
      builder.jsFileNames[i],
      builder.jsFileContents[i],
      false,
      &errorStream
    )) {
      if (!builder.fileNameAllowedToBeMissing(builder.jsFileNames[i])) {
        errorStream << "Failed to load javascript file: " << builder.jsFileNames[i];
        return errorStream.str();
      }
      std::stringstream moduleNotFound;
      moduleNotFound << "console.log(\"File '"
      << builder.jsFileNames[i] << "' not found. "
      << "Failure to find that file has been "
      << "specifically white-listed as ok. \");";
      builder.jsFileContents[i] = moduleNotFound.str();
    }
  }
  return builder.getOnePageJavascriptBrowserify();
}

bool BuilderApplication::loadJavascriptFileNames(
  const std::string& buildFileNameVirtual,
  std::stringstream* commentsOnFailure
) {
  if (!FileOperations::loadFileToStringVirtual(
    buildFileNameVirtual, this->buildFile, false, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load build file. ";
    }
    return false;
  }
  JSData reader;
  if (!reader.readstring(this->buildFile, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to parse build json file. ";
    }
    return false;
  }
  if (reader.theType != JSData::token::tokenArray) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Build json not of type list. ";
    }
    return false;
  }
  for (int i = 0; i < reader.listObjects.size; i ++) {
    const JSData& current = reader.listObjects[i];
    if (current.theType != JSData::token::tokenString) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Found non-string source javascript file. ";
      }
      return false;
    }
    this->jsFileNames.addOnTop(current.stringValue);
  }
  return true;
}

std::string BuilderApplication::getOnePageJavascriptBrowserify() {
  MacroRegisterFunctionWithName("BuilderApplication::getOnePageJavascriptBrowserify");
  std::stringstream out;
  out << "(()=>{\n"
  << "let theJSContent = {\n";
  for (int i = 0; i < this->jsFileContents.size; i ++) {
    std::string fileNameNoJS;
    if (!StringRoutines::stringEndsWith(this->jsFileNames[i], ".js", &fileNameNoJS)) {
      fileNameNoJS = this->jsFileNames[i];
    }
    out << "\"" << fileNameNoJS << "\" : function(require, module, exports){\n";
    out << this->jsFileContents[i];
    out << "\n},\n";
  }
  out << "};\n"
  << "theJSContent['/calculator-html/browserifier'](null, null, null);\n"
  << "\n}) ();";
  return out.str();
}

std::string WebAPIResponse::getHTMLAllInOneJavascriptCSS(
  const std::string& virtualHTMLFileName, bool appendBuildHash
) {
  MacroRegisterFunctionWithName("WebAPIReponse::getHTMLAllInOneJavascriptCSS");
  BuilderApplication builder;
  std::stringstream out;
  std::stringstream errorStream;
  if (!FileOperations::loadFileToStringVirtual(
    virtualHTMLFileName,
    builder.htmlRaw,
    false,
    &errorStream
  )) {
    out << "<html><body><b>Failed to load the application file. "
    << "Further comments follow. " << errorStream.str() << "</body></html>";
    return out.str();
  }
  builder.buildHtmlJavascriptPage(appendBuildHash);
  return builder.htmlJSbuild;
}

std::string WebAPIResponse::getApp(bool appendBuildHash) {
  return WebAPIResponse::getHTMLAllInOneJavascriptCSS("/calculator-html/index.html", appendBuildHash);
}

std::string WebAPIResponse::getCompareExpressionsPage(bool appendBuildHash) {
  return WebAPIResponse::getHTMLAllInOneJavascriptCSS("/calculator-html/compare_expressions.html", appendBuildHash);
}

JSData Course::toJSON() const {
  JSData result;
  result[WebAPI::problem::courseTitle] = this->title;
  result[WebAPI::problem::courseHome] = Configuration::courseTemplates + this->courseTemplate;
  result[WebAPI::problem::topicList] = Configuration::topicLists + this->courseTopicsNoFolder;
  if (this->flagRoughDraft != "") {
    result["roughDraft"] = this->flagRoughDraft;
  }
  return result;
}

std::string Course::courseTopicsWithFolder() {
  return Configuration::topicLists + this->courseTopicsNoFolder;
}

std::string Course::toString() const {
  std::stringstream out;
  out << "Html: " << this->courseTemplate
  << "\n" << "Topics: " << this->courseTopicsNoFolder;
  return out.str();
}

bool Course::isEmpty() {
  return this->courseTemplate == "" && this->courseTopicsNoFolder == "" && this->title == "";
}

void Course::reset() {
  this->courseTemplate = "";
  this->courseTopicsNoFolder = "";
  this->title = "";
  this->flagRoughDraft = "";
}

std::string CourseList::toHtml() {
  return this->theCourses.toString();
}

bool CourseList::loadFromString(const std::string& input) {
  MacroRegisterFunctionWithName("CourseList::loadFromString");
  std::stringstream tableReader(input);
  std::string currentLine, currentArgument;
  Course current;
  while (std::getline(tableReader, currentLine, '\n')) {
    if (StringRoutines::stringBeginsWith(currentLine, "Html:", &currentArgument)) {
      if (current.courseTemplate != "") {
        this->theCourses.addOnTop(current);
        current.reset();
      }
      current.courseTemplate = StringRoutines::stringTrimWhiteSpace(currentArgument);
    }
    if (StringRoutines::stringBeginsWith(currentLine, "Topics:", &currentArgument)) {
      if (current.courseTopicsNoFolder != "") {
        this->theCourses.addOnTop(current);
        current.reset();
      }
      current.courseTopicsNoFolder = StringRoutines::stringTrimWhiteSpace(currentArgument);
    }
    if (StringRoutines::stringBeginsWith(currentLine, "Title:", &currentArgument)) {
      if (current.title != "") {
        this->theCourses.addOnTop(current);
        current.reset();
      }
      current.title = StringRoutines::stringTrimWhiteSpace(currentArgument);
    }
    if (StringRoutines::stringBeginsWith(currentLine, "RoughDraft:", &currentArgument)) {
      if (current.flagRoughDraft != "") {
        this->theCourses.addOnTop(current);
        current.reset();
      }
      current.flagRoughDraft = StringRoutines::stringTrimWhiteSpace(currentArgument);
    }
  }
  if (!current.isEmpty()) {
    this->theCourses.addOnTop(current);
  }
  return true;
}

bool CourseList::load() {
  std::string theTopicFile;
  std::stringstream commentsOnFailure;
  if (!FileOperations::loadFiletoStringVirtualCustomizedReadOnly(
    "/coursesavailable/default.txt", theTopicFile, &commentsOnFailure
  )) {
    commentsOnFailure << "Failed to fetch available courses from /coursesavailable/default.txt. ";
    this->errorMessage = commentsOnFailure.str();
    return false;
  }
  return this->loadFromString(theTopicFile);
}

JSData CourseList::toJSON() {
  JSData output;
  if (this->errorMessage != "") {
    output[WebAPI::result::error] = this->errorMessage;
  }
  output["courses"].theType = JSData::token::tokenArray;
  for (int i = 0; i < this->theCourses.size; i ++) {
    Course& currentCourse = this->theCourses[i];
    output["courses"].listObjects.addOnTop(currentCourse.toJSON());
  }
  return output;
}

JSData WebAPIResponse::getSelectCourseJSON() {
  MacroRegisterFunctionWithName("WebAPIReponse::getSelectCourseJSON");
  CourseList theCourses;
  theCourses.load();
  return theCourses.toJSON();
}

std::string WebAPIResponse::getHtmlTagWithManifest() {
  MacroRegisterFunctionWithName("WebAPIReponse::getHtmlTagWithManifest");
  std::stringstream out;
  out << "<!DOCTYPE HTML>\n";
  out << "<html>\n<!-- tag added automatically; user-specified html tag ignored-->\n";
  return out.str();
}

JSData WebAPIResponse::getTopicTableJSON() {
  MacroRegisterFunctionWithName("WebAPIReponse::getTopicTableJSON");
  std::stringstream out;
  CalculatorHTML thePage;
  std::stringstream comments;
  thePage.fileName = HtmlRoutines::convertURLStringToNormal(global.getWebInput(WebAPI::problem::courseHome), false);
  thePage.topicListFileName = HtmlRoutines::convertURLStringToNormal(global.getWebInput(WebAPI::problem::topicList), false);
  JSData result;
  if (!thePage.loadAndParseTopicList(out)) {
    out << "Failed to load and parse topic list.";
    result[WebAPI::result::error] = out.str();
    return result;
  }
  if (!thePage.loadMe(true, global.getWebInput(WebAPI::problem::randomSeed), &comments)) {
    comments << "While loading topic table, failed to load file: ["
    << global.getWebInput(WebAPI::problem::courseHome) << "].";
    result[WebAPI::result::comments] = comments.str();
    return result;
  }
  thePage.computeTopicListAndPointsEarned(comments);
  return thePage.toStringTopicListJSON(&comments);
}

void WebAPIResponse::getJSDataUserInfo(JSData& outputAppend, const std::string& comments) {
  MacroRegisterFunctionWithName("WebAPIReponse::getJSDataUserInfo");
  outputAppend["linkApp"] = WebAPIResponse::youHaveReachedTheBackend;
  outputAppend[WebAPI::result::loginDisabledEveryoneIsAdmin] = global.flagDisableDatabaseLogEveryoneAsAdmin;
  outputAppend[WebAPI::result::useFallbackDatabase] = !global.flagDatabaseCompiled;
  outputAppend[WebAPI::result::httpsSupport] = global.flagSSLIsAvailable;
  if (comments != "") {
    outputAppend[WebAPI::result::comments] = HtmlRoutines::convertStringToHtmlString(comments, false);
  }
  if (global.theResponse.monitoringAllowed()) {
    outputAppend[WebAPI::UserInfo::processMonitoring] = "true";
    outputAppend[Configuration::millisecondsReplyAfterComputation] = static_cast<double>(
      global.millisecondsReplyAfterComputation
    );
  } else {
    outputAppend[WebAPI::UserInfo::processMonitoring] = "false";
  }
  if (global.getWebInput(WebAPI::result::error) != "") {
    outputAppend[WebAPI::result::error] = HtmlRoutines::convertStringToHtmlString(
      global.getWebInput(WebAPI::result::error), false
    );
  }
  if (!global.flagLoggedIn) {
    outputAppend[WebAPI::result::status] = "not logged in";
    return;
  }
  outputAppend[WebAPI::result::status] = "logged in";
  outputAppend[DatabaseStrings::labelUsername]            = HtmlRoutines::convertStringToHtmlString(global.userDefault.username                 , false);
  outputAppend[DatabaseStrings::labelAuthenticationToken] = HtmlRoutines::convertStringToHtmlString(global.userDefault.actualAuthenticationToken, false);
  outputAppend[DatabaseStrings::labelUserRole]            = HtmlRoutines::convertStringToHtmlString(global.userDefault.userRole                 , false);
  outputAppend[DatabaseStrings::labelInstructor]          = HtmlRoutines::convertStringToHtmlString(global.userDefault.instructorInDB           , false);
  outputAppend[DatabaseStrings::labelSection]             = HtmlRoutines::convertStringToHtmlString(global.userDefault.sectionInDB              , false);
  outputAppend[DatabaseStrings::labelCurrentCourses]      = HtmlRoutines::convertStringToHtmlString(global.userDefault.courseInDB               , false);
  outputAppend[DatabaseStrings::labelDeadlinesSchema]     = HtmlRoutines::convertStringToHtmlString(global.userDefault.deadlineSchema           , false);
  JSData sectionsTaught;
  sectionsTaught.theType = JSData::token::tokenArray;
  for (int i = 0; i < global.userDefault.sectionsTaught.size; i ++) {
    JSData nextSection;
    nextSection = HtmlRoutines::convertStringToHtmlString(global.userDefault.sectionsTaught[i], false);
    sectionsTaught.listObjects.addOnTop(nextSection);
  }
  outputAppend[DatabaseStrings::labelSectionsTaught] = sectionsTaught;
}

JSData WebAPIResponse::getJSONUserInfo(const std::string& comments) {
  MacroRegisterFunctionWithName("WebAPIReponse::getJSONUserInfo");
  JSData output;
  WebAPIResponse::getJSDataUserInfo(output, comments);
  return output;
}

std::string WebAPIResponse::getJSONFromTemplate() {
  MacroRegisterFunctionWithName("WebAPIReponse::getJSONFromTemplate");
  std::stringstream out;
  CalculatorHTML thePage;
  std::stringstream comments;
  thePage.fileName = HtmlRoutines::convertURLStringToNormal(global.getWebInput(WebAPI::problem::courseHome), false);
  if (!thePage.loadMe(true, global.getWebInput(WebAPI::problem::randomSeed), &comments)) {
    out << "<b>Failed to load file: "
    << global.getWebInput(WebAPI::problem::courseHome) << ". </b>"
    << "<br>Comments:<br> " << comments.str();
    return out.str();
  }
  if (!thePage.interpretHtml(&comments)) {
    out << "<b>Failed to interpret as template the following file: "
    << global.getWebInput(WebAPI::problem::courseHome) << ". </b>"
    << "<br>Comments:<br> " << comments.str();
    return out.str();
  }
  out << "<!-- File automatically generated from template: "
  << global.getWebInput(WebAPI::problem::fileName)
  << ".-->\n";
  out << thePage.outputHtmlBodyNoTag;
  out << "<small>Generated in " << global.getElapsedMilliseconds() << " ms. </small>";
  return out.str();
}

JSData WebAPIResponse::getExamPageJSON() {
  MacroRegisterFunctionWithName("WebAPIReponse::getExamPageJSON");
  std::stringstream out;
  JSData output;
  if (!global.flagLoggedIn && global.requestType == "scoredQuizJSON") {
    output[WebAPI::result::error] = "Scored quiz requires login";
    return output;
  }
  CalculatorHTML theFile;
  std::stringstream errorAndDebugStream;
  std::string problemBody = theFile.LoadAndInterpretCurrentProblemItemJSON(
    global.userRequestRequiresLoadingRealExamData(),
    global.getWebInput(WebAPI::problem::randomSeed),
    &errorAndDebugStream
  );
  //<-must come after theFile.outputHtmlHeadNoTag
  errorAndDebugStream << theFile.outputDebugInformationBody;
  out << problemBody;
  std::string commentsProblem = errorAndDebugStream.str();
  output[WebAPI::problem::content] = HtmlRoutines::convertStringToURLString(out.str(), false);
  if (commentsProblem != "") {
    output[WebAPI::problem::commentsProblem] = commentsProblem;
  }
  output[WebAPI::problem::deadlineSingle] = theFile.outputDeadlineString;
  output[WebAPI::problem::problemLabel] = theFile.outputProblemLabel;
  output[WebAPI::problem::title] = theFile.outputProblemTitle;
  output[WebAPI::problem::fileName] = theFile.fileName;
  output[WebAPI::problem::idProblem] = theFile.fileName;
  if (theFile.flagLoadedSuccessfully) {
    output["answers"] = theFile.getJavascriptMathQuillBoxesForJSON();
    JSData theScripts;
    theScripts = JSData::token::tokenArray;
    theScripts.listObjects.setSize(theFile.theScripts.size());
    for (int i = 0; i < theFile.theScripts.size(); i ++) {
      theScripts[theFile.theScripts.keys[i]] =
      HtmlRoutines::convertStringToURLString(theFile.theScripts.values[i], false);
    }
    output["scripts"] = theScripts;
    output["forReal"] = theFile.flagIsForReal;
    if (!theFile.flagIsForReal) {
      std::stringstream randomSeedStream;
      randomSeedStream << theFile.problemData.randomSeed;
      output[WebAPI::problem::randomSeed] = randomSeedStream.str();
    }
  }
  return output;
}

JSData WebAPIResponse::getEditPageJSON() {
  MacroRegisterFunctionWithName("WebAPIReponse::getEditPageJSON");
  JSData output;
  if (
    !global.flagLoggedIn ||
    !global.userDefaultHasAdminRights()
  ) {
    output[WebAPI::result::error] = "Only logged-in admins are allowed to edit pages.";
    return output;
  }
  CalculatorHTML theFile;
  theFile.loadFileNames();
  std::stringstream failureStream;
  if (!theFile.loadMe(false, global.getWebInput(WebAPI::problem::randomSeed), &failureStream)) {
    std::stringstream errorStream;
    errorStream << " <b>Failed to load file: ["
    << theFile.fileName << "], perhaps the file does not exist. </b>"
    << failureStream.str();
    output[WebAPI::result::error] = errorStream.str();
    return output;
  }
  if (!theFile.parser.parseHTML(&failureStream)) {
    std::stringstream errorStream;
    errorStream << "<b>Failed to parse file: " << theFile.fileName
    << ".</b> Details:<br>" << failureStream.str();
    output[WebAPI::result::error] = errorStream.str();
    //return output.toString(false);
  }
  HashedList<std::string, MathRoutines::hashString> theAutocompleteKeyWords;
  theFile.initBuiltInSpanClasses();
  std::stringstream comments;
  if (theFile.courseHome == theFile.fileName || theFile.topicListFileName == theFile.fileName) {
    theFile.loadAndParseTopicList(comments);
    theAutocompleteKeyWords.addOnTopNoRepetition(theFile.parser.calculatorClasses);
    theAutocompleteKeyWords.addOnTopNoRepetition(theFile.parser.calculatorClassesAnswerFields);
    theAutocompleteKeyWords.addOnTopNoRepetition(theFile.calculatorTopicElementNames);
    theAutocompleteKeyWords.addOnTopNoRepetition(theFile.topics.knownTopicBundles.keys);
  } else {
    Calculator tempCalculator;
    tempCalculator.initialize();
    tempCalculator.computeAutoCompleteKeyWords();
    theAutocompleteKeyWords.addOnTopNoRepetition(theFile.parser.calculatorClasses);
    theAutocompleteKeyWords.addOnTopNoRepetition(tempCalculator.autoCompleteKeyWords);
    theFile.initAutocompleteExtras();
    theAutocompleteKeyWords.addOnTopNoRepetition(theFile.autoCompleteExtras);
  }
  JSData theAutoCompleteWordsJS;
  theAutoCompleteWordsJS.theType = JSData::token::tokenArray;
  for (int i = 0; i < theAutocompleteKeyWords.size; i ++) {
    theAutoCompleteWordsJS[i] = theAutocompleteKeyWords[i];
  }
  output["autoComplete"] = theAutoCompleteWordsJS;
  output["content"] = HtmlRoutines::convertStringToURLString(theFile.parser.inputHtml, false);
  return output;
}

JSData WebAPIResponse::submitAnswersJSON() {
  return WebAPIResponse::submitAnswersJSON(global.getWebInput(WebAPI::problem::randomSeed), nullptr, true);
}

JSData WebAPIResponse::submitAnswersJSON(
  const std::string& inputRandomSeed, bool* outputIsCorrect, bool timeSafetyBrake
) {
  MacroRegisterFunctionWithName("WebAPIReponse::submitAnswers");
  if (!global.userDefaultHasAdminRights()) {
    global.theResponse.disallowReport();
  }

  std::stringstream output, errorStream, comments;
  JSData result;
  double startTime = global.getElapsedSeconds();
  CalculatorHTML theProblem;
  theProblem.loadCurrentProblemItem(
    global.userRequestRequiresLoadingRealExamData(), inputRandomSeed, &errorStream
  );
  if (!theProblem.flagLoadedSuccessfully) {
    errorStream << "Failed to load current problem. ";
    result[WebAPI::result::error] = errorStream.str();
    return result;
  }
  if (!theProblem.parseHTMLPrepareCommands(&comments)) {
    errorStream << "<b>Failed to parse problem. </b>";
    result[WebAPI::result::error] = errorStream.str();
    result[WebAPI::result::comments] = comments.str();
    return result;
  }
  if (!theProblem.problemData.flagRandomSeedGiven && !theProblem.flagIsForReal) {
    output << "<b>Random seed not given.</b>";
  }
  if (theProblem.fileName == "") {
    global.fatal << "This shouldn't happen: empty file name: theProblem.fileName." << global.fatal;
  }
  std::string studentAnswerNameReader;
  theProblem.studentTagsAnswered.initialize(theProblem.problemData.answers.size());
  MapList<std::string, std::string, MathRoutines::hashString>& theArgs = global.webArguments;
  int answerIdIndex = - 1;
  for (int i = 0; i < theArgs.size(); i ++) {
    if (StringRoutines::stringBeginsWith(
      theArgs.keys[i], WebAPI::problem::calculatorAnswerPrefix, &studentAnswerNameReader
    )) {
      int newAnswerIndex = theProblem.getAnswerIndex(studentAnswerNameReader);
      if (answerIdIndex == - 1) {
        answerIdIndex = newAnswerIndex;
      } else if (
        answerIdIndex != newAnswerIndex &&
        answerIdIndex != - 1 &&
        newAnswerIndex != - 1
      ) {
        output << "<b>You submitted two or more answers [answer tags: "
        << theProblem.problemData.answers.values[answerIdIndex].answerId
        << " and " << theProblem.problemData.answers.values[newAnswerIndex].answerId
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
      Answer& currentProblemData = theProblem.problemData.answers.values[answerIdIndex];
      currentProblemData.currentAnswerURLed = theArgs.values[i];
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
  ProblemData& currentProblemData = theProblem.problemData;
  Answer& currentA = currentProblemData.answers.values[answerIdIndex];

  currentA.currentAnswerClean = HtmlRoutines::convertURLStringToNormal(
    currentA.currentAnswerURLed, false
  );
  currentA.currentAnswerURLed = HtmlRoutines::convertStringToURLString(
    currentA.currentAnswerClean, false
  );//<-encoding back to overwrite malformed input
  theProblem.studentTagsAnswered.addSelectionAppendNewIndex(answerIdIndex);
  std::stringstream completedProblemStreamNoEnclosures;

  std::stringstream completedProblemStream;
  completedProblemStream << Calculator::Atoms::commandEnclosure << "{}("
  << currentA.commandsBeforeAnswer
  << ");";
  completedProblemStreamNoEnclosures << currentA.commandsBeforeAnswerNoEnclosuresForDEBUGGING;

  completedProblemStream << Calculator::Atoms::commandEnclosure << "{}("
  << currentA.answerId << "= (" << currentA.currentAnswerClean << ");"
  << ");";
  completedProblemStreamNoEnclosures << currentA.answerId << "= (" << currentA.currentAnswerClean << ");";

  bool hasCommentsBeforeSubmission = (
    StringRoutines::stringTrimWhiteSpace(currentA.commandsCommentsBeforeSubmission) != ""
  );
  if (hasCommentsBeforeSubmission) {
    completedProblemStream
    << Calculator::Atoms::commandEnclosure << "{}("
    << currentA.commandsCommentsBeforeSubmission
    << ");";
    completedProblemStreamNoEnclosures << currentA.commandsCommentsBeforeSubmission;
  }
  completedProblemStream << theProblem.cleanUpCommandString(currentA.commandVerificationOnly);
  completedProblemStreamNoEnclosures << theProblem.cleanUpCommandString(currentA.commandVerificationOnly);

  std::stringstream debugInputStream;
  if (global.userDebugFlagOn() && global.userDefaultHasAdminRights()) {
    debugInputStream
    << HtmlRoutines::getCalculatorComputationAnchorNewPage(
      completedProblemStreamNoEnclosures.str(), "Input, no enclosures. "
    );
  }
  if (timeSafetyBrake) {
    global.millisecondsMaxComputation = global.getElapsedMilliseconds() + 20000; // + 20 sec
  }
  Calculator interpreter;
  interpreter.initialize();
  interpreter.flagWriteLatexPlots = false;
  interpreter.flagPlotNoControls = true;

  interpreter.evaluate(completedProblemStream.str());
  if (interpreter.flagAbortComputationASAP || interpreter.syntaxErrors != "") {
    if (interpreter.errorsPublic.str() != "") {
      output << "While checking your answer, got the error: "
      << "<br><b style =\"color:red\">"
      << interpreter.errorsPublic.str()
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
      global.millisecondsMaxComputation = global.getElapsedMilliseconds() + 20000; //+20 sec
    }
    isolatedInterpreter.evaluate("(" + currentA.currentAnswerClean + ")");
    if (isolatedInterpreter.syntaxErrors != "") {
      output << isolatedInterpreter.toStringSyntacticStackHTMLSimple();
    } else {
      output << isolatedInterpreter.outputString;
    }
    if (global.userDebugFlagOn() && global.userDefaultHasAdminRights()) {
      output << "<hr><b>Administartor view internals.</b><hr>"
      << debugInputStream.str() << "<hr>"
      << interpreter.outputString
      << "<br>" << interpreter.outputCommentsString
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
  if (!interpreter.programExpression[interpreter.programExpression.size() - 1].isSmallInteger(&mustBeOne)) {
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
    if (global.userDefaultHasAdminRights() && global.userDebugFlagOn()) {
      output << "<tr><td>Administrator view internals. "
      << "<hr>" << debugInputStream.str()
      << "<br>The calculator output is: " << interpreter.outputString
      << "Comments: " << interpreter.comments.str()
      << "<hr>Input, no enclosures: <hr>"
      << completedProblemStreamNoEnclosures.str();
      output << "<hr>Input, full:<hr>"
      << interpreter.inputString << "<hr></td></tr>";
    }
  } else {
    output << "<tr><td><span style =\"color:green\"><b>Correct! </b></span>" << "</td></tr>";
  }
  if (hasCommentsBeforeSubmission) {
    output << "<tr><td>"
    << WebAPIResponse::getCommentsInterpretation(interpreter, 3, theFormat)
    << "</td></tr>\n";
  }
  if (global.flagDatabaseCompiled) {
    UserCalculator& theUser = theProblem.currentUser;
    theUser.::UserCalculatorData::operator=(global.userDefault);
    bool deadLinePassed = false;
    bool hasDeadline = true;
    double secondsTillDeadline = - 1;
    if (theProblem.flagIsForReal) {
      if (!theProblem.loadAndParseTopicList(output)) {
        hasDeadline = false;
      }
      std::string theSQLstring;
      theSQLstring = theUser.sectionComputed;
      if (hasDeadline) {
        bool unused = false;
        std::string theDeadlineString = theProblem.getDeadline(
          theProblem.fileName,
          HtmlRoutines::convertStringToURLString(theSQLstring, false),
          unused
        );

        if (theDeadlineString == "" || theDeadlineString == " ") {
          hasDeadline = false;
        } else {
          TimeWrapper now, deadline; //<-needs a fix for different time formats.
          // <-For the time being, we hard-code it
          // to month/day/year format (no time to program it better).
          std::stringstream badDateStream;
          if (!deadline.assignMonthDayYear(theDeadlineString, badDateStream)) {
            output << "<tr><td><b>Problem reading deadline. </b> The deadline string was: "
            << theDeadlineString << ". Comments: "
            << "<span style =\"color:red\">" << badDateStream.str() << "</span>"
            << "</td></tr><tr><td> This should not happen. "
            << CalculatorHTML::bugsGenericMessage << "</td></tr>";
            output << "</table>";
            result[WebAPI::result::resultHtml] = output.str();
            return result;
          }
          now.assignLocalTime();
          // out << "Now: " << asctime (&now.theTime) << " mktime: " << mktime(&now.theTime)
          // << " deadline: " << asctime(&deadline.theTime) << " mktime: " << mktime(&deadline.theTime);
          secondsTillDeadline = deadline.subtractAnotherTimeFromMeInSeconds(now) + 7 * 3600;
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
      theUser.setProblemData(theProblem.fileName, currentProblemData);
      if (!theUser.storeProblemData(theProblem.fileName, &comments)) {
        output << "<tr><td><b>This shouldn't happen and may be a bug: "
        << "failed to store your answer in the database. "
        << CalculatorHTML::bugsGenericMessage
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
          << TimeWrapper::toStringSecondsToDaysHoursSecondsString(
            secondsTillDeadline, false, false
          ) << " after deadline. </b></span></td></tr>";
        } else {
          output << "<tr><td><span style =\"color:green\"><b>Submission "
          << TimeWrapper::toStringSecondsToDaysHoursSecondsString(
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
  errorMessage = interpreter.toStringIsCorrectAsciiCalculatorString(currentA.currentAnswerClean);
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
  result[WebAPI::result::millisecondsComputation] = global.getElapsedSeconds() - startTime;
  return result;
}

std::string WebAPIResponse::addTeachersSections() {
  MacroRegisterFunctionWithName("WebAPIReponse::addTeachersSections");
  std::stringstream out;
  if (!global.userDefaultHasAdminRights() || !global.flagUsingSSLinCurrentConnection) {
    out << "<b>Only admins may assign sections to teachers.</b>";
    return out.str();
  }
  std::string input = HtmlRoutines::convertURLStringToNormal(
    global.getWebInput("teachersAndSections"), false
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
  HtmlRoutines::convertURLStringToNormal(inputParsed["teachers"].stringValue, false);
  std::string desiredSectionsOneString =
  HtmlRoutines::convertURLStringToNormal(inputParsed["students"].stringValue, false);
  List<std::string> desiredSectionsList;

  List<std::string> theTeachers;
  List<char> delimiters;
  delimiters.addOnTop(' ');
  delimiters.addOnTop('\r');
  delimiters.addOnTop('\n');
  delimiters.addOnTop('\t');
  delimiters.addOnTop(',');
  delimiters.addOnTop(';');
  delimiters.addOnTop(static_cast<char>(160)); //<-&nbsp
  StringRoutines::stringSplitExcludeDelimiters(desiredSectionsOneString, delimiters, desiredSectionsList);

  StringRoutines::stringSplitExcludeDelimiters(desiredUsers, delimiters, theTeachers);
  if (theTeachers.size == 0) {
    out << "<b>Could not extract teachers from " << desiredUsers << ".</b>";
    return out.str();
  }
  UserCalculator currentTeacher;
  for (int i = 0; i < theTeachers.size; i ++) {
    currentTeacher.reset();
    currentTeacher.username = theTeachers[i];
    if (!currentTeacher.loadFromDatabase(&out, &out)) {
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
    if (!Database::get().updateOne(findQuery, setQuery, &out)) {
      out << "<span style =\"color:red\">Failed to store course info of instructor: " << theTeachers[i] << ". </span><br>";
    } else {
      out << "<span style =\"color:green\">Assigned " << theTeachers[i] << " to section(s): "
      << desiredSectionsList << "</span><br>";
    }
  }
  return out.str();
}

std::string WebAPIResponse::addUserEmails(const std::string& hostWebAddressWithPort) {
  MacroRegisterFunctionWithName("WebAPIReponse::addUserEmails");
  (void) hostWebAddressWithPort;
  std::stringstream out;
  if (
    !global.userDefaultHasAdminRights() ||
    !global.flagUsingSSLinCurrentConnection
  ) {
    out << "<b>Only admins may add users, under ssl connection. </b>";
    return out.str();
  }
  std::string inputEmails = HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::request::userList), false
  );
  std::string userPasswords = HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::request::passwordList), false
  );
  std::string userGroup =
  StringRoutines::stringTrimWhiteSpace(HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(DatabaseStrings::labelSection), false
  ));
  std::string userRole = HtmlRoutines::convertURLStringToNormal(global.getWebInput("userRole"), false);

  if (inputEmails == "") {
    out << "addUserEmails failed: <b>No emails to add</b>";
    return out.str();
  }
  std::stringstream comments;
  bool sentEmails = true;
  bool doSendEmails = global.requestType == "sendEmails" ?  true : false;
  int numNewUsers = 0;
  int numUpdatedUsers = 0;
  bool createdUsers = Database::get().theUser.addUsersFromEmails(
    inputEmails, userPasswords, userRole, userGroup, comments, numNewUsers, numUpdatedUsers
  );
  if (createdUsers) {
    out << "<span style =\"color:green\">Success: "
    << numNewUsers << " new users and " << numUpdatedUsers
    << " user updates.</span> User roles: " << userRole;
  } else
    out << "<b style = 'color:red'>Failed to add all users.</b> "
    << "Errors follow.<hr>"
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

const std::string CalculatorHTML::bugsGenericMessage =
"Please take a screenshot, copy the link address and send those along "
"with a short explanation to the administrator of the web site. ";

JSData WebAPIResponse::getAnswerOnGiveUp() {
  return WebAPIResponse::getAnswerOnGiveUp(global.getWebInput(WebAPI::problem::randomSeed));
}

JSData WebAPIResponse::getAnswerOnGiveUp(
  const std::string& inputRandomSeed,
  std::string* outputNakedAnswer,
  bool* answerGenerationSuccess,
  bool doIncludeTimeStats
) {
  MacroRegisterFunctionWithName("CalculatorHTML::getAnswerOnGiveUp");
  GlobalVariables::Response::StateMaintainer maintain(global.theResponse);
  if (!global.userDefaultHasAdminRights()) {
    global.theResponse.disallowReport();
  }
  if (outputNakedAnswer != nullptr) {
    *outputNakedAnswer = "";
  }
  JSData result;
  result[WebAPI::problem::answerGenerationSuccess] = "false";
  if (answerGenerationSuccess != nullptr) {
    *answerGenerationSuccess = false;
  }
  int64_t startTimeInMilliseconds = global.getElapsedMilliseconds();
  CalculatorHTML theProblem;
  std::stringstream errorStream;
  theProblem.loadCurrentProblemItem(false, inputRandomSeed, &errorStream);
  if (!theProblem.flagLoadedSuccessfully) {
    errorStream << "Problem name is: " << theProblem.fileName
    << " <b>Could not load problem, this may be a bug. "
    << CalculatorHTML::bugsGenericMessage << "</b>";
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
  if (!theProblem.parseHTMLPrepareCommands(&errorStream)) {
    errorStream << "<br><b>Problem preparation failed.</b>";
    result[WebAPI::result::error] = errorStream.str();
    return result;
  }
  std::string lastStudentAnswerID;
  MapList<std::string, std::string, MathRoutines::hashString>& theArgs = global.webArguments;
  for (int i = 0; i < theArgs.size(); i ++) {
    StringRoutines::stringBeginsWith(
      theArgs.keys[i],
      WebAPI::problem::calculatorAnswerPrefix,
      &lastStudentAnswerID
    );
  }
  int indexLastAnswerId = theProblem.getAnswerIndex(lastStudentAnswerID);
  std::stringstream out;
  if (indexLastAnswerId == - 1) {
    errorStream << "File: "
    << theProblem.fileName
    << "<br><b>Student submitted answerID: " << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "</b>";
    if (global.userDebugFlagOn() && global.userDefaultHasAdminRights()) {
      errorStream << "<hr>" << theProblem.problemData.toStringAvailableAnswerIds();
    }
    result[WebAPI::result::millisecondsComputation] = global.getElapsedMilliseconds() - startTimeInMilliseconds;
    result[WebAPI::result::error] = errorStream.str();
    return result;
  }
  Answer& currentA = theProblem.problemData.answers.values[indexLastAnswerId];
  if (currentA.commandsNoEnclosureAnswerOnGiveUpOnly == "") {
    out << "<b> Unfortunately there is no answer given for this "
    << "question (answerID: " << lastStudentAnswerID << ").</b>";
    if (global.userDebugFlagOn() && global.userDefaultHasProblemComposingRights()) {
      out << "<br>Answer status: " << currentA.toString();
    }
    result[WebAPI::result::error] = out.str();
    return result;
  }
  Calculator interpreter;
  interpreter.initialize();
  interpreter.flagPlotNoControls = true;
  interpreter.flagWriteLatexPlots = false;
  std::stringstream answerCommands, answerCommandsNoEnclosure;
  answerCommands << currentA.commandsBeforeAnswer;
  answerCommandsNoEnclosure << currentA.commandsBeforeAnswerNoEnclosuresForDEBUGGING;
  answerCommands << Calculator::Atoms::commandEnclosure
  << "{}(" << currentA.commandsNoEnclosureAnswerOnGiveUpOnly << ");";
  answerCommandsNoEnclosure << currentA.commandsNoEnclosureAnswerOnGiveUpOnly;
  interpreter.evaluate(answerCommands.str());
  if (interpreter.syntaxErrors != "") {
    out << "<b style ='color:red'>Failed to evaluate the default answer. "
    << "Likely there is a bug with the problem. </b>";
    if (global.userDefaultHasProblemComposingRights()) {
      out << HtmlRoutines::getCalculatorComputationAnchorNewPage(
        answerCommandsNoEnclosure.str(), "Calculator input no enclosures"
      );
    }
    out << "<br>" << CalculatorHTML::bugsGenericMessage << "<br>Details: <br>"
    << interpreter.toStringSyntacticStackHTMLSimple();
    result[WebAPI::result::resultHtml] = out.str();
    int64_t ellapsedTime = global.getElapsedMilliseconds() - startTimeInMilliseconds;
    result[WebAPI::result::millisecondsComputation] = ellapsedTime;
    return result;
  }
  if (interpreter.flagAbortComputationASAP) {
    out << "<b style = 'color:red'>Failed to evaluate the default answer. "
    << "Likely there is a bug with the problem. </b>";
    if (global.userDefaultHasProblemComposingRights()) {
      out << HtmlRoutines::getCalculatorComputationAnchorNewPage(
        answerCommandsNoEnclosure.str(), "Calculator input no enclosures"
      );
    }
    out << "<br>" << CalculatorHTML::bugsGenericMessage << "<br>Details: <br>"
    << interpreter.outputString
    << interpreter.outputCommentsString
    << "<hr>Input: <br>" << interpreter.inputString;
    int64_t ellapsedTime = global.getElapsedMilliseconds() - startTimeInMilliseconds;
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
  const Expression& currentE = interpreter.programExpression[
    interpreter.programExpression.size() - 1
  ][1];
  if (!currentE.startsWith(interpreter.opCommandSequence())) {
    out << "\\(\\displaystyle " << currentE.toString(&theFormat) << "\\)";
    if (outputNakedAnswer != nullptr) {
      *outputNakedAnswer = currentE.toString(&theFormat);
    }
    result[WebAPI::problem::answerGenerationSuccess] = "true";
    if (answerGenerationSuccess != nullptr) {
      *answerGenerationSuccess = true;
    }
  } else {
    for (int j = 1; j < currentE.size(); j ++) {
      if (
        currentE[j].startsWith(interpreter.opRulesOff()) ||
        currentE[j].startsWith(interpreter.opRulesOn())
      ) {
        continue;
      }
      std::string stringAnswer;
      if (currentE[j].isOfType<std::string>(&stringAnswer)) {
        if (StringRoutines::stringBeginsWith(stringAnswer, "Approximations have been")) {
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
      if (currentE[j].isOfType<std::string>()) {
        out << currentE[j].getValue<std::string>();
      } else {
        out << "\\(\\displaystyle " << currentE[j].toString(&theFormat) << "\\)";
      }
      if (isFirst) {
        if (outputNakedAnswer != nullptr) {
          *outputNakedAnswer = currentE[j].toString(&theFormat);
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
    int64_t ellapsedTime = global.getElapsedMilliseconds() - startTimeInMilliseconds;
    result[WebAPI::result::millisecondsComputation] = ellapsedTime;
  }
  if (global.userDebugFlagOn() && global.userDefaultHasAdminRights()) {
    out
    << "<hr>"
    << HtmlRoutines::getCalculatorComputationAnchorNewPage(
      answerCommandsNoEnclosure.str(),
      "Calculator input no enclosures"
    );
    out << interpreter.outputString << "<hr>"
    << interpreter.outputCommentsString
    << "<hr>" << HtmlRoutines::getCalculatorComputationAnchorNewPage(
      interpreter.inputString, "Raw calculator input"
    );
  }
  result[WebAPI::result::resultHtml] = out.str();
  return result;
}

JSData WebAPIResponse::getAccountsPageJSON(const std::string& hostWebAddressWithPort) {
  MacroRegisterFunctionWithName("WebAPIReponse::getAccountsPageJSON");
  (void) hostWebAddressWithPort;
  JSData output;
  if (global.flagDisableDatabaseLogEveryoneAsAdmin) {
    output[WebAPI::result::error] = "Database disabled (cannot get accounts). ";
    return output;
  }
  if (
    !global.userDefaultHasAdminRights() ||
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
  columnsToRetain.addOnTop(DatabaseStrings::labelUsername);
  columnsToRetain.addOnTop(DatabaseStrings::labelEmail);
  columnsToRetain.addOnTop(DatabaseStrings::labelActivationToken);
  columnsToRetain.addOnTop(DatabaseStrings::labelCurrentCourses);
  columnsToRetain.addOnTop(DatabaseStrings::labelInstructor);
  columnsToRetain.addOnTop(DatabaseStrings::labelSection);
  columnsToRetain.addOnTop(DatabaseStrings::labelSemester);
  if (!Database::findFromJSONWithProjection(
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
  if (!Database::findFromJSONWithProjection(
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

std::string WebAPIResponse::getAccountsPageBody(const std::string& hostWebAddressWithPort) {
  MacroRegisterFunctionWithName("WebAPIReponse::getAccountsPageBody");
  (void) hostWebAddressWithPort;
  if (!global.flagDatabaseCompiled) {
    return "Database not available. ";
  }
  std::stringstream out;
  if (
    !global.userDefaultHasAdminRights() ||
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
  if (!Database::get().findFromJSON(
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
  if (!Database::get().findFromJSON(
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
  out << WebAPIResponse::toStringAssignSection();
  out << "<hr>";
  out
  << WebAPIResponse::toStringUserDetails(true, admins, hostWebAddressWithPort);
  out << "<hr>";
  out << WebAPIResponse::toStringUserDetails(
    false, students, hostWebAddressWithPort
  );
  return out.str();
}

std::string WebAPIResponse::getScoresPage() {
  MacroRegisterFunctionWithName("WebWorker::getScoresPage");
  std::stringstream out;
  CalculatorHTML thePage;
  thePage.loadDatabaseInfo(out);
  std::string theScoresHtml = WebAPIResponse::toStringUserScores();
  out << theScoresHtml;
  return out.str();
}

std::string WebAPIResponse::toStringUserDetailsTable(
  bool adminsOnly, List<JSData>& theUsers, const std::string& hostWebAddressWithPort
) {
  MacroRegisterFunctionWithName("WebAPIReponse::toStringUserDetailsTable");
  if (!global.flagDatabaseCompiled) {
    return "Compiled without database support. ";
  }
  std::stringstream out;
  bool flagFilterCourse = (!adminsOnly) && (global.getWebInput("filterAccounts") == "true");
  std::string currentCourse = HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::problem::courseHome), false
  );
  if (flagFilterCourse) {
    out << "<br>Displaying only students in course: <b style =\"color:blue\">"
    << currentCourse << "</b>. "
    << "<a href=\"" << global.displayNameExecutable
    << "?request=accounts&"
    << global.toStringCalculatorArgumentsNoNavigation(nullptr)
    << "filterAccounts=false&"
    << "\">Show all. </a>"
    << "<br>";
  }
  UserCalculator currentUser;
  HashedList<std::string, MathRoutines::hashString> theSections;
  List<std::string> sectionDescriptions;
  List<List<std::string> > activatedAccountBucketsBySection;
  List<List<std::string> > preFilledLinkBucketsBySection;
  List<List<std::string> > nonActivatedAccountBucketsBySection;
  for (int i = 0; i < theUsers.size; i ++) {
    currentUser.loadFromJSON(theUsers[i]);
    if (flagFilterCourse && (
      currentUser.courseInDB != currentCourse ||
      currentUser.instructorInDB != global.userDefault.username
    )) {
      continue;
    }
    if (!theSections.contains(currentUser.sectionInDB)) {
      std::stringstream currentSectionInfo;
      currentSectionInfo << "<b>Section: </b>" << currentUser.sectionInDB
      << ", <b>Course: </b>" << currentUser.courseInDB
      << ", <b>Instructor: </b>" << currentUser.instructorInDB;
      theSections.addOnTop(currentUser.sectionInDB);
      sectionDescriptions.addOnTop(currentSectionInfo.str());
    }
  }
  theSections.quickSortAscending(nullptr, &sectionDescriptions);
  activatedAccountBucketsBySection.setSize(theSections.size);
  nonActivatedAccountBucketsBySection.setSize(theSections.size);
  preFilledLinkBucketsBySection.setSize(theSections.size);
  int numActivatedUsers = 0;
  for (int i = 0; i < theUsers.size; i ++) {
    currentUser.loadFromJSON(theUsers[i]);
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
        << UserCalculator::getActivationAddressFromActivationToken
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
      << HtmlRoutines::convertStringToURLString("\n\n", false)
      << HtmlRoutines::convertStringToURLString(
        UserCalculator::getActivationAddressFromActivationToken(
          currentUser.actualActivationToken,
          hostWebAddressWithPort,
          currentUser.username,
          currentUser.email
        ),
        false
      )
      << HtmlRoutines::convertStringToURLString("\n\n", false)
      << " Once you activate your account, you can log in safely here: \n"
      << HtmlRoutines::convertStringToURLString("\n\n", false)
      << webAddress
      << HtmlRoutines::convertStringToURLString("\n\n", false)
      << "Best regards, \ncalculator-algebra.org.";
      oneTableLineStream << emailBody.str() << "\">Send email manually.</a> ";
      oneTableLineStream << "</td>";
    } else if (currentUser.actualActivationToken == "error") {
      oneTableLineStream << "<td>error</td><td></td>";
    } else {
      oneTableLineStream << "<td><span style =\"color:green\">activated</span></td><td></td><td></td>";
    }
    std::stringstream oneLink;
    oneLink << "<a href=\"" << global.displayNameExecutable << "?request=login&username="
    << currentUser.username << "\">" << currentUser.username << "</a>";
    oneTableLineStream << "<td>" << oneLink.str() << "</td>";
    oneTableLineStream << "</tr>";
    int indexCurrentBucket = theSections.getIndex(currentUser.sectionInDB);
    if (indexCurrentBucket != - 1) {
      if (isActivated) {
        activatedAccountBucketsBySection[indexCurrentBucket].addOnTop(oneTableLineStream.str());
      } else {
        nonActivatedAccountBucketsBySection[indexCurrentBucket].addOnTop(oneTableLineStream.str());
      }
      preFilledLinkBucketsBySection[indexCurrentBucket].addOnTop(oneLink.str());
    }
  }
  for (int i = 0; i < nonActivatedAccountBucketsBySection.size; i ++) {
    nonActivatedAccountBucketsBySection[i].quickSortAscending();
  }
  for (int i = 0; i < activatedAccountBucketsBySection.size; i ++) {
    activatedAccountBucketsBySection[i].quickSortAscending();
  }
  for (int i = 0; i < preFilledLinkBucketsBySection.size; i ++) {
    preFilledLinkBucketsBySection[i].quickSortAscending();
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

std::string WebAPIResponse::toStringAssignSection() {
  MacroRegisterFunctionWithName("WebAPIReponse::toStringAssignSection");
  std::stringstream out;
  std::string idAddressTextarea = "inputSetTeacher";
  std::string idExtraTextarea = "inputSections";
  std::string idOutput = "idOutputSections";
  out << "assign section(s) to teacher(s)<br> ";
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
    fields.addOnTop(DatabaseStrings::labelProblemFileName);
    fields.addOnTop(DatabaseStrings::labelProblemTotalQuestions);

    if (Database::findFromJSONWithProjection(
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
        ].stringValue;
        if (currentProblemName == "") {
          continue;
        }
        const std::string& expectedNumberOfAnswersString = result[i][
          DatabaseStrings::labelProblemTotalQuestions
        ].stringValue;
        if (expectedNumberOfAnswersString == "") {
          continue;
        }
        std::stringstream theStream(expectedNumberOfAnswersString);
        int numAnswers = - 1;
        theStream >> numAnswers;
        if (numAnswers == - 1) {
          continue;
        }
        global.problemExpectedNumberOfAnswers.setKeyValue(
          currentProblemName, numAnswers
        );
      }
    }
  }
  if (global.problemExpectedNumberOfAnswers.contains(problemName)) {
    return global.problemExpectedNumberOfAnswers.getValueCreate(problemName);
  }
  global << Logger::yellow << "Couldn't find problem info in DB for: "
  << problemName << ", trying to read problem from hd. " << Logger::endL;
  CalculatorHTML problemParser;
  problemParser.fileName = problemName;
  if (!problemParser.loadMe(false, "", &commentsOnFailure)) {
    global << Logger::yellow << WebAPI::problem::failedToLoadProblem
    << commentsOnFailure.str() << Logger::endL;
    return 0;
  }
  if (!problemParser.parseHTML(&commentsOnFailure)) {
    global << Logger::red << "<b>Failed to parse file: "
    << problemParser.fileName
    << ".</b> Details:<br>" << commentsOnFailure.str();
    return 0;
  }
  this->knownNumberOfAnswersFromHD = problemParser.problemData.answers.size();
  global << Logger::yellow << "Loaded problem: " << problemName
  << "; number of answers: " << this->knownNumberOfAnswersFromHD << Logger::endL;
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
  Database::get().updateOne(
    findEntry, newDBentry, &commentsOnFailure
  );
  return this->knownNumberOfAnswersFromHD;
}

void UserCalculator::computePointsEarned(
  const HashedList<std::string, MathRoutines::hashString>& gradableProblems,
  MapList<std::string, TopicElement, MathRoutines::hashString>* theTopics,
  std::stringstream& commentsOnFailure
) {
  MacroRegisterFunctionWithName("UserCalculator::computePointsEarned");
  this->pointsEarned = 0;
  this->pointsMax = 0;
  if (theTopics != nullptr) {
    for (int i = 0; i < theTopics->size(); i ++) {
      (*theTopics).values[i].totalPointsEarned = 0;
      (*theTopics).values[i].pointsEarnedInProblemsThatAreImmediateChildren = 0;
      (*theTopics).values[i].maxPointsInAllChildren = 0;
      (*theTopics).values[i].flagSubproblemHasNoWeight = false;
      //(*theTopics).theValues[i].maxCorrectAnswersInAllChildren = 0;
      //(*theTopics).theValues[i].numAnsweredInAllChildren = 0;
    }
  }
  for (int i = 0; i < this->problemData.size(); i ++) {
    const std::string problemName = this->problemData.keys[i];
    if (!gradableProblems.contains(problemName)) {
      continue;
    }
    ProblemData& currentP = this->problemData.values[i];
    currentP.points = 0;
    currentP.totalNumSubmissions = 0;
    currentP.numCorrectlyAnswered = 0;
    Rational currentWeight;
    currentP.flagProblemWeightIsOK =
    currentP.adminData.getWeightFromCourse(this->courseComputed, currentWeight);
    if (!currentP.flagProblemWeightIsOK) {
      currentWeight = 0;
    }
    for (int j = 0; j < currentP.answers.size(); j ++) {
      if (currentP.answers.values[j].numCorrectSubmissions > 0) {
        currentP.numCorrectlyAnswered ++;
      }
      currentP.totalNumSubmissions += currentP.answers.values[j].numSubmissions;
    }
    if (currentP.flagProblemWeightIsOK) {
      int expectedNumberOfAnswers = currentP.getExpectedNumberOfAnswers(problemName, commentsOnFailure);
      if (expectedNumberOfAnswers > 0) {
        currentP.points = (currentWeight * currentP.numCorrectlyAnswered) / expectedNumberOfAnswers;
        this->pointsEarned += currentP.points;
      }
    }
    if (theTopics != nullptr) {
      if (theTopics->contains(problemName)) {
        TopicElement& currentElt = theTopics->getValueCreate(problemName);
        this->pointsMax += currentWeight;
        for (int j = 0; j < currentElt.parentTopics.size; j ++) {
          (*theTopics).values[currentElt.parentTopics[j]].totalPointsEarned += currentP.points;
          (*theTopics).values[currentElt.parentTopics[j]].maxPointsInAllChildren += currentWeight;
          if (currentWeight == 0) {
            (*theTopics).values[currentElt.parentTopics[j]].flagSubproblemHasNoWeight = true;
          }
        }
        if (currentElt.parentTopics.size > 1) {
          (*theTopics).values[currentElt.parentTopics[currentElt.parentTopics.size - 2]].
            pointsEarnedInProblemsThatAreImmediateChildren += currentP.points;
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
  List<MapList<std::string, Rational, MathRoutines::hashString> > scoresBreakdown;
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
  theProblem.currentUser.::UserCalculatorData::operator=(global.userDefault);
  this->theProblem.loadFileNames();
  if (!this->theProblem.loadAndParseTopicList(comments)) {
    return false;
  }
  if (!this->theProblem.prepareSectionList(comments)) {
    return false;
  }
  if (!this->theProblem.loadDatabaseInfo(comments)) {
    comments << "<span style ='color:red'>Could not load your problem history.</span> <br>";
  }
  theProblem.currentUser.computePointsEarned(
    theProblem.currentUser.problemData.keys,
    &theProblem.topics.theTopics,
    comments
  );
  List<std::string> userLabels;
  int usernameIndex = userLabels.getIndex(DatabaseStrings::labelUsername);
  if (usernameIndex == - 1) {
    comments << "Could not find username column. ";
    return false;
  }
  int problemDataIndex = userLabels.getIndex(DatabaseStrings::labelProblemDataJSON);
  if (problemDataIndex == - 1) {
    comments << "Could not find problem data column. ";
    return false;
  }
  CalculatorHTML currentUserRecord;
  this->userScores.reserve(this->userProblemData.size);
  this->userNames.reserve(this->userProblemData.size);
  this->userInfos.reserve(this->userProblemData.size);
  this->scoresBreakdown.reserve(this->userProblemData.size);
  this->userScores.setSize(0);
  this->userNames.setSize(0);
  this->userInfos.setSize(0);
  this->scoresBreakdown.setSize(0);
  this->numStudentsSolvedEntireTopic.initializeFillInObject(this->theProblem.topics.theTopics.size(), 0);
  this->numStudentsSolvedPartOfTopic.initializeFillInObject(this->theProblem.topics.theTopics.size(), 0);
  this->numStudentsSolvedNothingInTopic.initializeFillInObject(this->theProblem.topics.theTopics.size(), 0);
  bool ignoreSectionsIdontTeach = true;
  this->currentSection = global.userDefault.sectionComputed;
  this->currentCourse = global.getWebInput(WebAPI::problem::courseHome);
  if (global.getWebInput("request") == "scoresInCoursePage") {
    this->currentSection = StringRoutines::stringTrimWhiteSpace(
      HtmlRoutines::convertURLStringToNormal(global.getWebInput("mainInput"), false)
    );
  }
  for (int i = 0; i < this->userProblemData.size; i ++) {
    //currentUserRecord.currentUser.courseInfo.rawStringStoredInDB = this->userTablE[i][courseInfoIndex];
    //currentUserRecord.currentUser.AssignCourseInfoString(&comments);
    if (ignoreSectionsIdontTeach) {
      if (currentUserRecord.currentUser.courseComputed != this->currentCourse) {
        continue;
      }
      if (global.userStudentVieWOn()) {
        if (currentUserRecord.currentUser.sectionInDB != this->currentSection) {
          continue;
        }
      } else {
        if (currentUserRecord.currentUser.sectionInDB != this->currentSection) {
          continue;
        }
      }
    }
    this->userScores.addOnTop(- 1);
    this->userNames.addOnTop(this->userProblemData[i][DatabaseStrings::labelUsername].stringValue);
    this->userInfos.addOnTop(currentUserRecord.currentUser.sectionInDB);
    this->scoresBreakdown.setSize(this->scoresBreakdown.size + 1);
    currentUserRecord.currentUser.username = this->userProblemData[i][
      DatabaseStrings::labelUsername
    ].stringValue;
    if (!currentUserRecord.currentUser.interpretDatabaseProblemDataJSON(
      this->userProblemData[i][DatabaseStrings::labelProblemDataJSON],
      comments
    )) {
      continue;
    }
    currentUserRecord.mergeProblemWeight(
      theProblem.currentUser.problemWeights,
      currentUserRecord.currentUser.problemData,
      false,
      &comments
    );
    currentUserRecord.currentUser.computePointsEarned(
      theProblem.problemNamesNoTopics,
      &theProblem.topics.theTopics,
      comments
    );
    this->scoresBreakdown.lastObject()->clear();
    for (int j = 0; j < theProblem.topics.theTopics.size(); j ++) {
      TopicElement& currentTopic = theProblem.topics.theTopics.values[j];
      Rational currentPts = currentTopic.totalPointsEarned;
      Rational maxPts = currentTopic.maxPointsInAllChildren;
      this->scoresBreakdown.lastObject()->setKeyValue(
        theProblem.topics.theTopics.keys[j], currentPts
      );
      if (maxPts == currentPts) {
        this->numStudentsSolvedEntireTopic[j] ++;
      } else if (currentPts > 0) {
        this->numStudentsSolvedPartOfTopic[j] ++;
      } else {
        this->numStudentsSolvedNothingInTopic[j] ++;
      }
    }
    *this->userScores.lastObject() = currentUserRecord.currentUser.pointsEarned;
  }
  return true;
}

std::string WebAPIResponse::getScoresInCoursePage() {
  MacroRegisterFunctionWithName("WebWorker::getScoresInCoursePage");
  if (!global.userDefaultHasAdminRights()) {
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
    TopicElement& currentElt = theScores.theProblem.topics.theTopics.values[i];
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

std::string WebAPIResponse::toStringUserScores() {
  MacroRegisterFunctionWithName("WebAPIReponse::toStringUserScores");
  if (!global.userDefaultHasAdminRights()) {
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
    TopicElement& currentElt = theScores.theProblem.topics.theTopics.values[i];
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
    TopicElement& currentElt = theScores.theProblem.topics.theTopics.values[i];
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
    TopicElement& currentElt = theScores.theProblem.topics.theTopics.values[i];
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

  out << "<tr><td><b>maximum score</b></td>"
  << "<td>-</td>";
  out
  << "<td>" << theScores.theProblem.currentUser.pointsMax.getDoubleValue()
  << "</td>";
  for (int j = 0; j < theScores.theProblem.topics.theTopics.size(); j ++) {
    TopicElement& currentElt = theScores.theProblem.topics.theTopics.values[j];
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
    << "<td>" << theScores.userScores[i].getDoubleValue() << "</td>";
    for (int j = 0; j < theScores.theProblem.topics.theTopics.size(); j ++) {
      TopicElement& currentElt = theScores.theProblem.topics.theTopics.values[j];
      if (currentElt.problemFileName != "") {
        continue;
      }
      if (
        currentElt.type != TopicElement::types::topic &&
        !currentElt.flagContainsProblemsNotInSubsection
      ) {
        continue;
      }
      if (theScores.scoresBreakdown[i].contains(
        theScores.theProblem.topics.theTopics.keys[j]
      )) {
        out << "<td>"
        << theScores.scoresBreakdown[i].values[j].getDoubleValue()
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

std::string WebAPIResponse::toStringUserDetails(
  bool adminsOnly, List<JSData>& theUsers, const std::string& hostWebAddressWithPort
) {
  MacroRegisterFunctionWithName("WebAPIReponse::toStringUserDetails");
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
  << HtmlRoutines::convertURLStringToNormal(global.getWebInput(WebAPI::problem::courseHome), false)
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
  out << WebAPIResponse::toStringUserDetailsTable(adminsOnly, theUsers, hostWebAddressWithPort);
  out << "</span>";
  return out.str();
}
