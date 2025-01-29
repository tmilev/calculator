#include "calculator_educational_functions_1.h"
#include "calculator_html_interpretation.h"
#include "database.h"
#include "general_file_operations_encodings.h"
#include "string_constants.h"
#include "web_api.h"
#include <iomanip>

const std::string CalculatorHTML::bugsGenericMessage =
"Please take a screenshot, copy the link address and send those along "
"with a short explanation to the administrator of the web site. ";

JSData WebAPIResponse::getProblemSolutionJSON() {
  STACK_TRACE("WebAPIReponse::getProblemSolutionJSON");
  if (!global.userDefaultHasAdminRights()) {
    global.response.disallowReport();
  }
  int64_t startMilliseconds = global.getElapsedMilliseconds();
  CalculatorHTML problem;
  std::stringstream out;
  std::stringstream errorStream;
  JSData result;
  problem.loadCurrentProblemItem(
    false, global.getWebInput(WebAPI::Problem::randomSeed), &errorStream
  );
  if (!problem.flagLoadedSuccessfully) {
    out
    << "Problem name is: "
    << problem.fileName
    << " <b>Could not load problem, this may be a bug. "
    << CalculatorHTML::bugsGenericMessage
    << "</b>";
    if (errorStream.str() != "") {
      out << " Comments: " << errorStream.str();
    }
    result[WebAPI::Result::resultHtml] = out.str();
    return result;
  }
  if (problem.flagIsForReal) {
    out
    << " <b>Not allowed to show answer of a "
    << "problem being tested for real. </b>";
    result[WebAPI::Result::resultHtml] = out.str();
    return result;
  }
  if (global.getWebInput(WebAPI::Problem::randomSeed) == "") {
    out
    << "<b>I could not figure out the "
    << "exercise problem (missing random seed). </b>";
    result[WebAPI::Result::resultHtml] = out.str();
    return result;
  }
  std::stringstream comments;
  if (!problem.parseHTMLPrepareCommands(&comments)) {
    out << "<br><b>Failed to parse problem.</b> Comments: " << comments.str();
    result[WebAPI::Result::resultHtml] = out.str();
    return result;
  }
  std::string lastStudentAnswerID;
  MapList<std::string, std::string, HashFunctions::hashFunction<std::string> >&
  arguments =
  global.webArguments;
  for (int i = 0; i < arguments.size(); i ++) {
    StringRoutines::stringBeginsWith(
      arguments.keys[i],
      WebAPI::Problem::calculatorAnswerPrefix,
      &lastStudentAnswerID
    );
  }
  int indexLastAnswerId = problem.getAnswerIndex(lastStudentAnswerID);
  if (indexLastAnswerId == - 1) {
    out
    << "<b>Student submitted answerID: "
    << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "</b>";
    if (global.userDebugFlagOn() && global.userDefaultHasAdminRights()) {
      out << "<hr>" << problem.problemData.toStringAvailableAnswerIds();
    }
    result[WebAPI::Result::resultHtml] = out.str();
    result[WebAPI::Result::millisecondsComputation] =
    global.getElapsedMilliseconds() - startMilliseconds;
    return result;
  }
  Answer& answer = problem.problemData.answers.values[indexLastAnswerId];
  Calculator interpreter;
  interpreter.initialize(Calculator::Mode::educational);
  interpreter.flagPlotNoControls = true;
  interpreter.flagWriteLatexPlots = false;
  if (!problem.prepareCommands(&comments)) {
    out
    << "<b>Failed to prepare calculator commands.</b>"
    << "<br>Comments:<br>"
    << comments.str();
    result[WebAPI::Result::resultHtml] = out.str();
    return result;
  }
  if (answer.solutionElements.size == 0) {
    out
    << "<b>There is no solution given for this question (answerID: "
    << lastStudentAnswerID
    << ").</b>";
    result[WebAPI::Result::resultHtml] = out.str();
    return result;
  }
  std::stringstream answerCommands;
  std::stringstream answerCommandsNoEnclosures;
  answerCommands
  << Calculator::Functions::Names::commandEnclosure
  << "{}("
  << answer.commandsBeforeAnswer
  << "); "
  << answer.commandsSolutionOnly;
  answerCommandsNoEnclosures
  << answer.commandsBeforeAnswerNoEnclosuresInternal
  << answer.commandsSolutionOnly;
  interpreter.evaluate(answerCommands.str());
  if (interpreter.parser.syntaxErrors != "") {
    out
    << "<b style='color:red'>Failed to compose the solution. "
    << "Likely there is a bug with the problem. </b>"
    << "<br>"
    << CalculatorHTML::bugsGenericMessage
    << "<br>Details: <br>"
    << interpreter.parser.toStringSyntacticStackHTMLSimple();
    result[WebAPI::Result::resultHtml] = out.str();
    result[WebAPI::Result::millisecondsComputation] =
    global.getElapsedMilliseconds() - startMilliseconds;
    return result;
  }
  if (interpreter.flagAbortComputationASAP) {
    out
    << "<b style='color:red'>Failed to compose the solution. "
    << "Likely there is a bug with the problem. </b>"
    << "<br>"
    << CalculatorHTML::bugsGenericMessage
    << "<br>Details: <br>"
    << interpreter.outputHTMLString
    << interpreter.outputCommentsString
    << "<hr>Input: <br>"
    << interpreter.inputString;
    result[WebAPI::Result::resultHtml] = out.str();
    result[WebAPI::Result::millisecondsComputation] =
    global.getElapsedMilliseconds() - startMilliseconds;
    return result;
  }
  if (
    !problem.processExecutedCommands(interpreter, answer.solutionElements, out)
  ) {
    result[WebAPI::Result::resultHtml] = out.str();
    result[WebAPI::Result::millisecondsComputation] =
    global.getElapsedMilliseconds() - startMilliseconds;
    return result;
  }
  for (int i = 0; i < answer.solutionElements.size; i ++) {
    if (answer.solutionElements[i].shouldShow()) {
      out << answer.solutionElements[i].toStringInterpretedBody();
    }
  }
  if (global.userDebugFlagOn() && global.userDefaultHasAdminRights()) {
    out
    << "<hr>"
    << HtmlRoutines::getCalculatorComputationAnchorSameURL(
      answerCommandsNoEnclosures.str(), "Input link"
    )
    << "<br>"
    << interpreter.outputHTMLString
    << "<hr>"
    << interpreter.outputCommentsString
    << "<hr><b>Raw command:</b> "
    << answer.commandsSolutionOnly;
  }
  result[WebAPI::Result::resultHtml] = out.str();
  result[WebAPI::Result::millisecondsComputation] =
  global.getElapsedMilliseconds() - startMilliseconds;
  return result;
}

std::string WebAPIResponse::setProblemWeight() {
  STACK_TRACE("WebAPIReponse::setProblemWeight");
  if (!global.userDefaultHasAdminRights()) {
    return "<b>Only admins may set problem weights.</b>";
  }
  CalculatorHTML problem;
  std::string inputProblemInfo =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput("mainInput"), false
  );
  std::stringstream commentsOnFailure;
  std::stringstream out;
  if (problem.mergeProblemWeightAndStore(inputProblemInfo, &commentsOnFailure))
  {
    out << "<b style='color:green'>Modified.</b>";
  } else {
    out << "<b style='color:red'>" << commentsOnFailure.str() << "</b>";
  }
  return out.str();
}

std::string WebAPIResponse::setProblemDeadline() {
  STACK_TRACE("WebAPIReponse::setProblemDeadline");
  if (!global.userDefaultHasAdminRights()) {
    return "<b>Only admins may set problem weights.</b>";
  }
  if (global.userDefault.deadlineSchema == "") {
    return "Empty deadline shema not allowed.";
  }
  CalculatorHTML problem;
  std::string inputProblemInfo =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput("mainInput"), false
  );
  std::stringstream commentsOnFailure;
  std::stringstream out;
  if (
    problem.mergeProblemDeadlineAndStore(inputProblemInfo, &commentsOnFailure)
  ) {
    out
    << "<b style='color:green'>Modified. </b>"
    << global.comments.getCurrentReset();
  } else {
    out << "<b style='color:red'>" << commentsOnFailure.str() << "</b>";
  }
  return out.str();
}

std::string WebAPIResponse::getSanitizedComment(
  const Expression& input, FormatExpressions& format, bool& resultIsPlot
) {
  STACK_TRACE("WebAPIReponse::getSanitizedComment");
  format.flagUseQuotes = false;
  resultIsPlot = false;
  std::string currentString;
  if (input.isOfType<std::string>(&currentString)) {
    if (
      StringRoutines::stringBeginsWith(
        currentString, "Approximations have been"
      )
    ) {
      return "";
    }
    return input.toString(&format);
  }
  if (input.isOfType<Plot>()) {
    resultIsPlot = true;
    return input.toString(&format);
  }
  if (input.hasType<Plot>()) {
    return "";
  }
  // <- expression has a partially drawn plot-> not displaying.
  if (input.owner == nullptr) {
    return "";
  }
  if (
    input.startsWith(input.owner->opRulesOff()) ||
    input.startsWith(input.owner->opRulesOn())
  ) {
    return "";
  }
  return input.toString(&format);
}

std::string WebAPIResponse::getCommentsInterpretation(
  Calculator& interpreterWithAdvice, int indexShift, FormatExpressions& format
) {
  STACK_TRACE("WebAPIReponse::getCommentsInterpretation");
  std::stringstream out;
  format.flagExpressionIsFinal = true;
  format.flagIncludeExtraHtmlDescriptionsInPlots = false;
  interpreterWithAdvice.objectContainer.resetPlots();
  if (indexShift >= interpreterWithAdvice.programExpression.size()) {
    return "";
  }
  const Expression& currentExpression =
  interpreterWithAdvice.programExpression[indexShift][1];
  bool resultIsPlot = false;
  if (
    !currentExpression.startsWith(interpreterWithAdvice.opCommandSequence())
  ) {
    out
    << WebAPIResponse::getSanitizedComment(
      currentExpression, format, resultIsPlot
    );
    return out.str();
  }
  std::string currentString;
  for (int i = 1; i < currentExpression.size(); i ++) {
    currentString =
    WebAPIResponse::getSanitizedComment(
      currentExpression[i], format, resultIsPlot
    );
    if (StringRoutines::stringTrimWhiteSpace(currentString) == "") {
      continue;
    }
    out << currentString;
    if (i != currentExpression.size() - 1 && !resultIsPlot) {
      out << "<br>";
    }
  }
  return out.str();
}

JSData WebAPIResponse::submitAnswersPreviewJSON() {
  STACK_TRACE("WebAPIResponse::submitAnswersPreviewJSON");
  if (!global.userDefaultHasAdminRights()) {
    global.response.disallowReport();
  }
  double startTime = global.getElapsedSeconds();
  std::string lastStudentAnswerID;
  std::string lastAnswer;
  std::stringstream out;
  std::stringstream studentAnswerSream;
  MapList<std::string, std::string, HashFunctions::hashFunction<std::string> >&
  arguments =
  global.webArguments;
  JSData result;
  for (int i = 0; i < arguments.size(); i ++) {
    if (
      StringRoutines::stringBeginsWith(
        arguments.keys[i],
        WebAPI::Problem::calculatorAnswerPrefix,
        &lastStudentAnswerID
      )
    ) {
      lastAnswer =
      "(" +
      HtmlRoutines::convertURLStringToNormal(arguments.values[i], false) +
      "); ";
    }
  }
  studentAnswerSream << lastAnswer;
  out
  << "Your answer(s): \\(\\displaystyle "
  << lastAnswer
  << "\\)"
  << "\n<br>\n";
  CalculatorHTML problem;
  std::stringstream errorStream;
  std::stringstream comments;
  problem.loadCurrentProblemItem(
    global.userRequestRequiresLoadingRealExamData(),
    global.getWebInput(WebAPI::Problem::randomSeed),
    &errorStream
  );
  if (!problem.flagLoadedSuccessfully) {
    out << "<br><b>" << WebAPI::Problem::failedToLoadProblem << "</b> ";
  }
  if (!problem.parseHTMLPrepareCommands(&comments)) {
    out << "<br><b>Failed to parse problem.</b> Comments: " << comments.str();
  }
  int indexLastAnswerId = problem.getAnswerIndex(lastStudentAnswerID);
  if (indexLastAnswerId == - 1) {
    errorStream
    << "<br>Student submitted answerID: "
    << lastStudentAnswerID
    << " but that is not an ID of an answer tag. "
    << "<br>Response time: "
    << global.getElapsedSeconds() - startTime
    << " second(s). ";
    result[WebAPI::Result::error] = errorStream.str();
    result[WebAPI::Result::resultHtml] = out.str();
    return result;
  }
  Answer& currentAnswer =
  problem.problemData.answers.values[indexLastAnswerId];
  if (!problem.prepareCommands(&comments)) {
    errorStream
    << "Something went wrong while interpreting the problem file. ";
    if (global.userDebugFlagOn() && global.userDefaultHasAdminRights()) {
      errorStream << comments.str();
    }
    result[WebAPI::Result::millisecondsComputation] =
    global.getElapsedSeconds() - startTime;
    result[WebAPI::Result::error] = errorStream.str();
    result[WebAPI::Result::resultHtml] = out.str();
    return result;
  }
  Calculator interpreter;
  interpreter.flagUseLnInsteadOfLog = true;
  interpreter.initialize(Calculator::Mode::educational);
  interpreter.flagWriteLatexPlots = false;
  interpreter.flagPlotNoControls = true;
  std::stringstream studentAnswerWithComments;
  studentAnswerWithComments
  << Calculator::Functions::Names::commandEnclosure
  << "{}("
  << currentAnswer.commandsCommentsBeforeInterpretation
  << ");"
  << studentAnswerSream.str();
  interpreter.evaluate(studentAnswerWithComments.str());
  if (interpreter.parser.syntaxErrors != "") {
    errorStream
    << "<b style ='color:red'>Parsing failure:</b><br>"
    << interpreter.parser.toStringSyntacticStackHTMLSimple();
    result[WebAPI::Result::error] = errorStream.str();
    result[WebAPI::Result::millisecondsComputation] =
    global.getElapsedSeconds() - startTime;
    result[WebAPI::Result::resultHtml] = out.str();
    return result;
  } else if (interpreter.flagAbortComputationASAP) {
    out
    << "<b style='color:red'>Failed to evaluate your answer, got:</b><br>"
    << interpreter.outputHTMLString;
    result[WebAPI::Result::resultHtml] = out.str();
    result[WebAPI::Result::millisecondsComputation] =
    global.getElapsedSeconds() - startTime;
    return result;
  }
  FormatExpressions format;
  format.flagUseLatex = true;
  format.flagUsePmatrix = true;
  const Expression& studentAnswerNoContextExpression =
  interpreter.programExpression[interpreter.programExpression.size() - 1];
  out << "<b style='color:magenta'>Interpreting as:</b><br>";
  out
  << "\\(\\displaystyle "
  << studentAnswerNoContextExpression.toString(&format)
  << "\\)";
  Calculator interpreterWithAdvice;
  interpreterWithAdvice.flagUseLnInsteadOfLog = true;
  interpreterWithAdvice.initialize(Calculator::Mode::educational);
  interpreterWithAdvice.flagWriteLatexPlots = false;
  interpreterWithAdvice.flagPlotNoControls = true;
  std::stringstream calculatorInputStream;
  std::stringstream calculatorInputStreamNoEnclosures;
  calculatorInputStream
  << Calculator::Functions::Names::commandEnclosure
  << "{}("
  << currentAnswer.commandsBeforeAnswer
  << ");";
  calculatorInputStreamNoEnclosures
  << currentAnswer.commandsBeforeAnswerNoEnclosuresInternal;
  calculatorInputStream
  << Calculator::Functions::Names::commandEnclosure
  << "{}("
  << currentAnswer.answerId
  << " = "
  << lastAnswer
  << ");";
  calculatorInputStreamNoEnclosures
  << currentAnswer.answerId
  << " = "
  << lastAnswer
  << "";
  bool hasCommentsBeforeSubmission = (
    StringRoutines::stringTrimWhiteSpace(
      currentAnswer.commandsCommentsBeforeSubmission
    ) !=
    ""
  );
  if (hasCommentsBeforeSubmission) {
    calculatorInputStream
    << Calculator::Functions::Names::commandEnclosure
    << "{}("
    << currentAnswer.commandsCommentsBeforeSubmission
    << ");";
    calculatorInputStreamNoEnclosures
    << currentAnswer.commandsCommentsBeforeSubmission;
  }
  std::stringstream problemLinkStream;
  problemLinkStream
  << HtmlRoutines::getCalculatorComputationAnchorSameURL(
    calculatorInputStreamNoEnclosures.str(), "Input link"
  );
  interpreterWithAdvice.evaluate(calculatorInputStream.str());
  if (interpreterWithAdvice.parser.syntaxErrors != "") {
    out
    << "<br><b style='color:red'>"
    << "Something went wrong when parsing your answer "
    << "in the context of the current problem. "
    << "</b>";
    if (global.userDefaultHasAdminRights()) {
      out
      << problemLinkStream.str()
      << interpreterWithAdvice.outputHTMLString
      << "<br>"
      << interpreterWithAdvice.outputCommentsString;
    }
    result[WebAPI::Result::resultHtml] = out.str();
    result[WebAPI::Result::millisecondsComputation] =
    global.getElapsedSeconds() - startTime;
    return result;
  }
  if (interpreterWithAdvice.flagAbortComputationASAP) {
    out
    << "<br><b style='color:red'>"
    << "Something went wrong when interpreting your answer "
    << "in the context of the current problem. "
    << "</b>";
    if (global.userDefaultHasAdminRights() && global.userDebugFlagOn()) {
      out
      << "<br>Logged-in as administrator with debug flag on "
      << "=> printing error details. "
      << interpreterWithAdvice.outputHTMLString
      << "<br>"
      << interpreterWithAdvice.outputCommentsString;
      out
      << "<hr><b>Calculator input:</b> "
      << problemLinkStream.str()
      << "<br>"
      << calculatorInputStream.str();
    }
    result[WebAPI::Result::resultHtml] = out.str();
    result[WebAPI::Result::millisecondsComputation] =
    global.getElapsedSeconds() - startTime;
    return result;
  }
  if (hasCommentsBeforeSubmission) {
    out
    << WebAPIResponse::getCommentsInterpretation(
      interpreterWithAdvice, 3, format
    );
  }
  result[WebAPI::Result::millisecondsComputation] =
  global.getElapsedSeconds() - startTime;
  if (global.userDefaultHasAdminRights() && global.userDebugFlagOn()) {
    out
    << "<hr> "
    << problemLinkStream.str()
    << "<br>"
    << calculatorInputStreamNoEnclosures.str()
    << "<br>"
    << "Result:<br>"
    << interpreterWithAdvice.outputHTMLString
    << "<br>"
    << interpreterWithAdvice.outputCommentsString
    << "<br>Parsed elements: "
    << problem.parser.toStringParsingStack(problem.content);
  }
  result[WebAPI::Result::resultHtml] = out.str();
  return result;
}

JSData WebAPIResponse::clonePageResult() {
  STACK_TRACE("WebAPIReponse::clonePageResult");
  JSData result;
  if (
    !global.flagLoggedIn ||
    !global.userDefaultHasAdminRights() ||
    !global.flagUsingSSLinCurrentConnection
  ) {
    result[WebAPI::Result::error] =
    "Cloning problems allowed only for logged-in admins under ssl connection.";
    return result;
  }
  std::string fileNameTarget =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Problem::fileNameTarget), false
  );
  std::string fileNameToBeCloned =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Problem::fileName), false
  );
  std::stringstream out;
  std::string startingFileString;
  if (
    !FileOperations::loadFiletoStringVirtualCustomizedReadOnly(
      fileNameToBeCloned, startingFileString, &out
    )
  ) {
    out << "Could not find input file: " << fileNameToBeCloned;
    result[WebAPI::Result::error] = out.str();
    return result;
  }
  std::fstream fileStream;
  if (
    FileOperations::fileExistsVirtualCustomizedReadOnly(
      fileNameTarget, nullptr
    )
  ) {
    out << "Output file: " << fileNameTarget << " already exists. ";
    result[WebAPI::Result::error] = out.str();
    return result;
  }
  if (
    !FileOperations::openFileVirtualCustomizedWriteOnlyCreateIfNeeded(
      fileStream, fileNameTarget, false, false, false, &out
    )
  ) {
    out << "Failed to open output file: " << fileNameTarget << ".";
    result[WebAPI::Result::error] = out.str();
    return result;
  }
  fileStream << startingFileString;
  fileStream.close();
  std::string fileNameNonVirtual;
  std::stringstream comments;
  if (
    !FileOperations::getPhysicalFileNameFromVirtualCustomizedReadOnly(
      fileNameTarget, fileNameNonVirtual, &comments
    )
  ) {
    out << "Could not get physical file name from virtual. " << comments.str();
  } else {
    out
    << "Wrote "
    << startingFileString.size()
    << " bytes to file: "
    << fileNameTarget;
  }
  result[WebAPI::Result::resultHtml] = out.str();
  return result;
}

void BuilderApplication::initializeTags(bool appendBuildHash) {
  this->allInOneJavascriptTagOriginal =
  "<script type='text/javascript' src='" +
  WebAPI::Request::onePageJS +
  "'></script>";
  std::string virtualJavascriptFileName = WebAPI::Request::onePageJS;
  if (appendBuildHash) {
    virtualJavascriptFileName =
    FileOperations::getVirtualNameWithHash(WebAPI::Request::onePageJS);
  }
  this->allInOneJavascriptTagDesired =
  "<script src='" +
  virtualJavascriptFileName +
  "' onerror = 'errorLoadingScript(this);'></script>\n";
  this->calculatorCSSTagOriginal =
  "<link type='text/css' rel='stylesheet' href='" +
  WebAPI::Request::calculatorCSS +
  "'>";
  std::string virtualCSSFileName = WebAPI::Request::calculatorCSS;
  if (appendBuildHash) {
    virtualCSSFileName =
    FileOperations::getVirtualNameWithHash(WebAPI::Request::calculatorCSS);
  }
  this->calculatorCSSTagDesired =
  "<link type='text/css' rel='stylesheet' href='" +
  virtualCSSFileName +
  "' onerror = 'errorLoadingScript(this);'>";
}

void BuilderApplication::buildHtmlJavascriptPage(bool appendBuildHash) {
  STACK_TRACE("BuilderApplication::buildHtmlJavascriptPage");
  this->initializeTags(appendBuildHash);
  this->htmlJSbuild = this->htmlRaw;
  if (
    !StringRoutines::replaceOnce(
      this->htmlJSbuild,
      this->allInOneJavascriptTagOriginal,
      this->allInOneJavascriptTagDesired
    )
  ) {
    global
    << Logger::red
    << "Failed to find javascript_all_in_one.js tag in the html data."
    << Logger::endL;
  }
  if (
    !StringRoutines::replaceOnce(
      this->htmlJSbuild,
      this->calculatorCSSTagOriginal,
      this->calculatorCSSTagDesired
    )
  ) {
    global
    << Logger::red
    << "Failed to find javascript_all_in_one.js tag in the html data."
    << Logger::endL;
  }
}

bool BuilderApplication::fileNameAllowedToBeMissing(const std::string& input) {
  // External dependency.
  // The calculator must be capable of running without it.
  if (input == "/calculator_html/external/build/output-min.js") {
    return true;
  }
  return false;
}

std::string WebAPIResponse::getOnePageJS() {
  STACK_TRACE("WebAPIResponse::getOnePageJS");
  return
  WebAPIResponse::getBrowserification("/calculator_html/BUILD.json", "./app");
}

std::string WebAPIResponse::getCalculatorWorkerJS() {
  STACK_TRACE("WebAPIResponse::getCalculatorWorkerJS");
  return
  WebAPIResponse::getBrowserification(
    "/calculator_html/web_assembly/BUILD.json",
    "./web_assembly/calculator_web_assembly_runner"
  );
}

std::string WebAPIResponse::getBrowserification(
  const std::string& buildJSONVirtualFileName,
  const std::string& scriptEntryPoint
) {
  STACK_TRACE("WebAPIReponse::getBrowserification");
  BuilderApplication builder;
  std::stringstream errorStream;
  if (!builder.loadJavascriptFileNames(buildJSONVirtualFileName, &errorStream))
  {
    errorStream
    << "<b>Failed to load the javascript build json file: "
    << buildJSONVirtualFileName
    << "</b>";
    return errorStream.str();
  }
  builder.jsFileContents.setSize(builder.jsFileNames.size);
  for (int i = 0; i < builder.jsFileNames.size; i ++) {
    if (
      !FileOperations::loadFileToStringVirtual(
        builder.jsFileNames[i], builder.jsFileContents[i], false, &errorStream
      )
    ) {
      if (!builder.fileNameAllowedToBeMissing(builder.jsFileNames[i])) {
        errorStream
        << "Failed to load javascript file: "
        << builder.jsFileNames[i];
        return errorStream.str();
      }
      std::stringstream moduleNotFound;
      moduleNotFound
      << "console.log(\"File '"
      << builder.jsFileNames[i]
      << "' not found. "
      << "Failure to find that file has been "
      << "specifically white-listed as ok. \");";
      builder.jsFileContents[i] = moduleNotFound.str();
    }
    if (builder.jsFileNames[i] == "/calculator_html/web_assembly/calculator.js")
    {
      // Special exception for web assembly.
      // Save ourselves lots of headache and
      // configuration by manually inserting the snippets we
      // care about into the auto-generated calculator.js file.
      builder.jsFileContents[i] +=
      "module.exports={\n"
      "TTY,\n"
      "Module,\n"
      "intArrayFromString,\n"
      "addOnInit,\n"
      "_free,\n"
      "};";
    }
  }
  return builder.getBrowserificationAssembled(scriptEntryPoint);
}

bool BuilderApplication::loadJavascriptFileNames(
  const std::string& buildFileNameVirtual, std::stringstream* commentsOnFailure
) {
  if (
    !FileOperations::loadFileToStringVirtual(
      buildFileNameVirtual, this->buildFile, false, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load build file. ";
    }
    return false;
  }
  JSData reader;
  if (!reader.parse(this->buildFile, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to parse build json file. ";
    }
    return false;
  }
  if (reader.elementType != JSData::Type::tokenArray) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Build json not of type list. ";
    }
    return false;
  }
  for (int i = 0; i < reader.listObjects.size; i ++) {
    const JSData& current = reader.listObjects[i];
    if (current.elementType != JSData::Type::tokenString) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Found non-string source javascript file. ";
      }
      return false;
    }
    this->jsFileNames.addOnTop(current.stringValue);
  }
  return true;
}

std::string BuilderApplication::getBrowserificationAssembled(
  const std::string& entryPoint
) {
  STACK_TRACE("BuilderApplication::getBrowserificationAssembled");
  std::stringstream out;
  out << "(()=>{\n" << "let jsContent = {\n";
  for (int i = 0; i < this->jsFileContents.size; i ++) {
    std::string fileNameNoJS;
    if (
      !StringRoutines::stringEndsWith(
        this->jsFileNames[i], ".js", &fileNameNoJS
      )
    ) {
      fileNameNoJS = this->jsFileNames[i];
    }
    out
    << "\""
    << fileNameNoJS
    << "\" : function(require, module, exports){\n";
    out << this->jsFileContents[i];
    out << "\n},\n";
  }
  out
  << "};\n"
  << "jsContent['/calculator_html/browserifier'](jsContent, '"
  << entryPoint
  << "', null);\n"
  << "\n})();";
  return out.str();
}

std::string WebAPIResponse::getHTMLAllInOneJavascriptCSS(
  const std::string& virtualHTMLFileName, bool appendBuildHash
) {
  STACK_TRACE("WebAPIReponse::getHTMLAllInOneJavascriptCSS");
  BuilderApplication builder;
  std::stringstream out;
  std::stringstream errorStream;
  if (
    !FileOperations::loadFileToStringVirtual(
      virtualHTMLFileName, builder.htmlRaw, false, &errorStream
    )
  ) {
    out
    << "<html><body><b>Failed to load the application file. </b>"
    << "Further comments follow. "
    << errorStream.str()
    << "</body></html>";
    return out.str();
  }
  builder.buildHtmlJavascriptPage(appendBuildHash);
  return builder.htmlJSbuild;
}

std::string WebAPIResponse::getApp(bool appendBuildHash) {
  return
  WebAPIResponse::getHTMLAllInOneJavascriptCSS(
    "/calculator_html/index.html", appendBuildHash
  );
}

std::string WebAPIResponse::getCompareExpressionsPage(bool appendBuildHash) {
  return
  WebAPIResponse::getHTMLAllInOneJavascriptCSS(
    "/calculator_html/compare_expressions_calculator.html", appendBuildHash
  );
}

JSData Course::toJSON() const {
  JSData result;
  result[WebAPI::Problem::courseTitle] = this->title;
  result[WebAPI::Problem::courseHome] =
  Configuration::courseTemplates + this->courseTemplate;
  result[WebAPI::Problem::topicList] =
  Configuration::topicLists + this->courseTopicsNoFolder;
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
  out
  << "Html: "
  << this->courseTemplate
  << "\n"
  << "Topics: "
  << this->courseTopicsNoFolder;
  return out.str();
}

bool Course::isEmpty() {
  return
  this->courseTemplate == "" &&
  this->courseTopicsNoFolder == "" &&
  this->title == "";
}

void Course::reset() {
  this->courseTemplate = "";
  this->courseTopicsNoFolder = "";
  this->title = "";
  this->flagRoughDraft = "";
}

std::string CourseList::toHtml() {
  return this->allCourses.toString();
}

bool CourseList::loadFromString(const std::string& input) {
  STACK_TRACE("CourseList::loadFromString");
  std::stringstream tableReader(input);
  std::string currentLine;
  std::string currentArgument;
  Course current;
  while (std::getline(tableReader, currentLine, '\n')) {
    if (
      StringRoutines::stringBeginsWith(currentLine, "Html:", &currentArgument)
    ) {
      if (current.courseTemplate != "") {
        this->allCourses.addOnTop(current);
        current.reset();
      }
      current.courseTemplate =
      StringRoutines::stringTrimWhiteSpace(currentArgument);
    }
    if (
      StringRoutines::stringBeginsWith(
        currentLine, "Topics:", &currentArgument
      )
    ) {
      if (current.courseTopicsNoFolder != "") {
        this->allCourses.addOnTop(current);
        current.reset();
      }
      current.courseTopicsNoFolder =
      StringRoutines::stringTrimWhiteSpace(currentArgument);
    }
    if (
      StringRoutines::stringBeginsWith(currentLine, "Title:", &currentArgument)
    ) {
      if (current.title != "") {
        this->allCourses.addOnTop(current);
        current.reset();
      }
      current.title = StringRoutines::stringTrimWhiteSpace(currentArgument);
    }
    if (
      StringRoutines::stringBeginsWith(
        currentLine, "RoughDraft:", &currentArgument
      )
    ) {
      if (current.flagRoughDraft != "") {
        this->allCourses.addOnTop(current);
        current.reset();
      }
      current.flagRoughDraft =
      StringRoutines::stringTrimWhiteSpace(currentArgument);
    }
  }
  if (!current.isEmpty()) {
    this->allCourses.addOnTop(current);
  }
  return true;
}

bool CourseList::load() {
  std::string topicFile;
  std::stringstream commentsOnFailure;
  if (
    !FileOperations::loadFiletoStringVirtualCustomizedReadOnly(
      "/coursesavailable/default.txt", topicFile, &commentsOnFailure
    )
  ) {
    commentsOnFailure
    << "Failed to fetch available courses "
    << "from /coursesavailable/default.txt. ";
    this->errorMessage = commentsOnFailure.str();
    return false;
  }
  return this->loadFromString(topicFile);
}

JSData CourseList::toJSON() {
  JSData output;
  if (this->errorMessage != "") {
    output[WebAPI::Result::error] = this->errorMessage;
  }
  output["courses"].elementType = JSData::Type::tokenArray;
  for (int i = 0; i < this->allCourses.size; i ++) {
    Course& currentCourse = this->allCourses[i];
    output["courses"].listObjects.addOnTop(currentCourse.toJSON());
  }
  return output;
}

JSData WebAPIResponse::getSelectCourseJSON() {
  STACK_TRACE("WebAPIReponse::getSelectCourseJSON");
  CourseList courses;
  courses.load();
  return courses.toJSON();
}

std::string WebAPIResponse::getHtmlTagWithManifest() {
  STACK_TRACE("WebAPIReponse::getHtmlTagWithManifest");
  std::stringstream out;
  out << "<!DOCTYPE HTML>\n";
  out
  << "<html>\n<!-- tag added automatically; "
  << "user-specified html tag ignored-->\n";
  return out.str();
}

JSData WebAPIResponse::getTopicTableJSON() {
  STACK_TRACE("WebAPIReponse::getTopicTableJSON");
  std::stringstream out;
  CalculatorHTML page;
  std::stringstream comments;
  page.fileName =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Problem::courseHome), false
  );
  page.topicListFileName =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Problem::topicList), false
  );
  JSData result;
  if (!page.loadAndParseTopicList(out)) {
    out << "Failed to load and parse topic list.";
    result[WebAPI::Result::error] = out.str();
    return result;
  }
  if (
    !page.loadMe(
      true, global.getWebInput(WebAPI::Problem::randomSeed), &comments
    )
  ) {
    comments
    << "While loading topic table, failed to load file: ["
    << global.getWebInput(WebAPI::Problem::courseHome)
    << "].";
    result[WebAPI::Result::comments] = comments.str();
    return result;
  }
  page.computeTopicListAndPointsEarned(comments);
  return page.toStringTopicListJSON(&comments);
}

void WebAPIResponse::getJSDataUserInfo(
  JSData& outputAppend, const std::string& comments
) {
  STACK_TRACE("WebAPIReponse::getJSDataUserInfo");
  outputAppend["linkApp"] = WebAPIResponse::youHaveReachedTheBackend;
  outputAppend[WebAPI::Result::loginDisabledEveryoneIsAdmin] =
  global.hasDisabledDatabaseEveryoneIsAdmin();
  outputAppend[WebAPI::Result::debugLogin] = global.flagDebugLogin;
  outputAppend[WebAPI::Result::database] = Database::name;
  outputAppend[WebAPI::Result::httpsSupport] = global.flagSSLAvailable;
  if (comments != "") {
    outputAppend[WebAPI::Result::comments] =
    HtmlRoutines::convertStringToHtmlString(comments, false);
  }
  if (global.response.monitoringAllowed()) {
    outputAppend[WebAPI::UserInfo::processMonitoring] = "true";
    outputAppend[Configuration::millisecondsReplyAfterComputation] =
    static_cast<double>(global.millisecondsReplyAfterComputation);
  } else {
    outputAppend[WebAPI::UserInfo::processMonitoring] = "false";
  }
  if (global.getWebInput(WebAPI::Result::error) != "") {
    outputAppend[WebAPI::Result::error] =
    HtmlRoutines::convertStringToHtmlString(
      global.getWebInput(WebAPI::Result::error), false
    );
  }
  if (!global.flagLoggedIn) {
    outputAppend[WebAPI::Result::status] = "not logged in";
    return;
  }
  outputAppend[WebAPI::Result::status] = "logged in";
  outputAppend[DatabaseStrings::labelUsername] =
  HtmlRoutines::convertStringToHtmlString(global.userDefault.username, false);
  outputAppend[DatabaseStrings::labelEmail] = global.userDefault.email;
  outputAppend[DatabaseStrings::labelAuthenticationToken] =
  HtmlRoutines::convertStringToHtmlString(
    global.userDefault.actualAuthenticationToken, false
  );
  outputAppend[DatabaseStrings::labelUserRole] =
  HtmlRoutines::convertStringToHtmlString(global.userDefault.userRole, false);
  outputAppend[DatabaseStrings::labelInstructor] =
  HtmlRoutines::convertStringToHtmlString(
    global.userDefault.instructorInDB, false
  );
  outputAppend[DatabaseStrings::labelSection] =
  HtmlRoutines::convertStringToHtmlString(
    global.userDefault.sectionInDB, false
  );
  outputAppend[DatabaseStrings::labelCurrentCourses] =
  HtmlRoutines::convertStringToHtmlString(
    global.userDefault.courseInDB, false
  );
  outputAppend[DatabaseStrings::labelDeadlinesSchema] =
  HtmlRoutines::convertStringToHtmlString(
    global.userDefault.deadlineSchema, false
  );
  JSData sectionsTaught;
  sectionsTaught.elementType = JSData::Type::tokenArray;
  for (int i = 0; i < global.userDefault.sectionsTaught.size; i ++) {
    JSData nextSection;
    nextSection =
    HtmlRoutines::convertStringToHtmlString(
      global.userDefault.sectionsTaught[i], false
    );
    sectionsTaught.listObjects.addOnTop(nextSection);
  }
  outputAppend[DatabaseStrings::labelSectionsTaught] = sectionsTaught;
}

JSData WebAPIResponse::getJSONUserInfo(const std::string& comments) {
  STACK_TRACE("WebAPIReponse::getJSONUserInfo");
  JSData output;
  WebAPIResponse::getJSDataUserInfo(output, comments);
  return output;
}

std::string WebAPIResponse::getJSONFromTemplate() {
  STACK_TRACE("WebAPIReponse::getJSONFromTemplate");
  std::stringstream out;
  CalculatorHTML page;
  std::stringstream comments;
  page.fileName =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Problem::courseHome), false
  );
  if (
    !page.loadMe(
      true, global.getWebInput(WebAPI::Problem::randomSeed), &comments
    )
  ) {
    out
    << "<b>Failed to load file: "
    << global.getWebInput(WebAPI::Problem::courseHome)
    << ". </b>"
    << "<br>Comments:<br> "
    << comments.str();
    return out.str();
  }
  if (!page.interpretHtml(&comments)) {
    out
    << "<b>Failed to interpret as template the following file: "
    << global.getWebInput(WebAPI::Problem::courseHome)
    << ". </b>"
    << "<br>Comments:<br> "
    << comments.str();
    return out.str();
  }
  out
  << "<!-- File automatically generated from template: "
  << global.getWebInput(WebAPI::Problem::fileName)
  << ".-->\n";
  out << page.outputHtmlBodyNoTag;
  out
  << "<small>Generated in "
  << global.getElapsedMilliseconds()
  << " ms. </small>";
  return out.str();
}

JSData WebAPIResponse::getExamPageJSON() {
  STACK_TRACE("WebAPIReponse::getExamPageJSON");
  std::stringstream out;
  JSData output;
  if (
    !global.flagLoggedIn && global.requestType == WebAPI::Frontend::scoredQuiz
  ) {
    output[WebAPI::Result::error] = "Scored quiz requires login";
    return output;
  }
  CalculatorHTML problem;
  std::stringstream errorAndDebugStream;
  std::string problemBody =
  problem.loadAndInterpretCurrentProblemItemJSON(
    global.userRequestRequiresLoadingRealExamData(),
    global.getWebInput(WebAPI::Problem::randomSeed),
    &errorAndDebugStream
  );
  // <-must come after file.outputHtmlHeadNoTag
  errorAndDebugStream << problem.outputDebugInformationBody;
  out << problemBody;
  std::string commentsProblem = errorAndDebugStream.str();
  output[WebAPI::Problem::content] =
  HtmlRoutines::convertStringToURLString(out.str(), false);
  if (commentsProblem != "") {
    output[WebAPI::Problem::commentsProblem] = commentsProblem;
  }
  output[WebAPI::Problem::deadlineSingle] = problem.outputDeadlineString;
  output[WebAPI::Problem::problemLabel] = problem.outputProblemLabel;
  output[WebAPI::Problem::title] = problem.outputProblemTitle;
  output[WebAPI::Problem::fileName] = problem.fileName;
  output[WebAPI::Problem::idProblem] = problem.fileName;
  if (problem.flagLoadedSuccessfully) {
    output["answers"] = problem.getEditorBoxesHTML();
    JSData scripts;
    scripts = JSData::Type::tokenArray;
    scripts.listObjects.setSize(problem.scripts.size());
    for (int i = 0; i < problem.scripts.size(); i ++) {
      scripts[problem.scripts.keys[i]] =
      HtmlRoutines::convertStringToURLString(problem.scripts.values[i], false);
    }
    output["scripts"] = scripts;
    output[WebAPI::Problem::forReal] = problem.flagIsForReal;
    if (!problem.flagIsForReal) {
      std::stringstream randomSeedStream;
      randomSeedStream << problem.problemData.randomSeed;
      output[WebAPI::Problem::randomSeed] = randomSeedStream.str();
    }
  }
  return output;
}

JSData WebAPIResponse::getEditPageJSON(bool showSourceRelaxed) {
  STACK_TRACE("WebAPIReponse::getEditPageJSON");
  JSData output;
  if (!global.flagLoggedIn || !global.userDefaultHasAdminRights()) {
    if (!showSourceRelaxed) {
      output[WebAPI::Result::error] = "Only logged-in admins can edit pages.";
      return output;
    } else {
      output[WebAPI::Result::comments] =
      "Only logged-in admins can edit pages.";
    }
  }
  CalculatorHTML editedFile;
  editedFile.loadFileNames();
  std::stringstream failureStream;
  if (
    !editedFile.loadMe(
      false, global.getWebInput(WebAPI::Problem::randomSeed), &failureStream
    )
  ) {
    std::stringstream errorStream;
    errorStream
    << " <b>Failed to load file: ["
    << editedFile.fileName
    << "], perhaps the file does not exist. </b>"
    << failureStream.str();
    output[WebAPI::Result::error] = errorStream.str();
    return output;
  }
  if (!editedFile.parser.parseHTML(&failureStream)) {
    std::stringstream errorStream;
    errorStream
    << "<b>Failed to parse file: "
    << editedFile.fileName
    << ".</b> Details:<br>"
    << failureStream.str();
    output[WebAPI::Result::error] = errorStream.str();
    // return output.toString(false);
  }
  HashedList<std::string> autocompleteKeyWords;
  editedFile.initBuiltInSpanClasses();
  std::stringstream comments;
  if (
    editedFile.courseHome == editedFile.fileName ||
    editedFile.topicListFileName == editedFile.fileName
  ) {
    editedFile.loadAndParseTopicList(comments);
    autocompleteKeyWords.addOnTopNoRepetition(
      editedFile.parser.calculatorClasses
    );
    autocompleteKeyWords.addOnTopNoRepetition(
      editedFile.parser.calculatorClassesAnswerFields
    );
    autocompleteKeyWords.addOnTopNoRepetition(
      editedFile.calculatorTopicElementNames
    );
    autocompleteKeyWords.addOnTopNoRepetition(
      editedFile.topics.knownTopicBundles.keys
    );
  } else {
    Calculator currentCalculator;
    currentCalculator.initialize(Calculator::Mode::educational);
    currentCalculator.computeAutoCompleteKeyWords();
    autocompleteKeyWords.addOnTopNoRepetition(
      editedFile.parser.calculatorClasses
    );
    autocompleteKeyWords.addOnTopNoRepetition(
      currentCalculator.autoCompleteKeyWords
    );
    editedFile.initAutocompleteExtras();
    autocompleteKeyWords.addOnTopNoRepetition(editedFile.autoCompleteExtras);
  }
  JSData autoCompleteWordsJS;
  autoCompleteWordsJS.elementType = JSData::Type::tokenArray;
  for (int i = 0; i < autocompleteKeyWords.size; i ++) {
    autoCompleteWordsJS[i] = autocompleteKeyWords[i];
  }
  output["autoComplete"] = autoCompleteWordsJS;
  output["content"] =
  HtmlRoutines::convertStringToURLString(editedFile.parser.inputHtml, false);
  return output;
}

JSData WebAPIResponse::submitAnswersJSON() {
  return
  WebAPIResponse::submitAnswersJSON(
    global.getWebInput(WebAPI::Problem::randomSeed), nullptr, true
  );
}

class AnswerCheckerNoProblem {
public:
  std::string answerGiven;
  std::string answerId;
  std::string answerCheck;
  Calculator interpreter;
  // Hidden commands providing context for the problem.
  // Include computations related to setting up a problem or
  // computing its answer.
  std::string commandsBeforeAnswer;
  // Visible commands providing context for the problem.
  // Include comments on the student answer such as
  // simple error checks.
  std::string commandsCommentsBeforeSubmission;
  // Intended for problem debugging.
  std::string commandsBeforeAnswerNoEnclosuresInternal;
  std::string errorSyntax;
  std::string verification;
  std::string completedProblem;
  std::string completedProblemNoEnclosures;
  std::string administratorViewInternals;
  bool hasCommentsBeforeSubmission;
  bool answerIsCorrect;
  bool hideGivenAnswer;
  AnswerCheckerNoProblem();
  void prepareForEvaluation();
  bool checkAnswer(bool* outputAnswerIsCorrect);
  JSData toJSON(bool hideGivenAnswer);
  void computeVerificationString();
};

class AnswerChecker {
public:
  CalculatorHTML problem;
  AnswerCheckerNoProblem checker;
  JSData result;
  int64_t startTime;
  int answerIndex;
  std::string storageReport;
  JSData submitAnswersJSON(
    const std::string& inputRandomSeed,
    bool* outputIsCorrect,
    bool timeSafetyBrake
  );
  bool prepareProblem(const std::string& inputRandomSeed);
  bool extractStudentAnswerPartOne();
  bool extractStudentAnswerPartTwo();
  bool storeInDatabase(bool answerIsCorrect);
  bool checkAnswerStandard(bool* outputIsCorrect);
  bool checkAnswerHardcoded(bool* outputIsCorrect);
  void computeResultHTML();
  AnswerChecker();
};

AnswerChecker::AnswerChecker() {
  this->startTime = 0;
  this->answerIndex = - 1;
}

bool AnswerChecker::prepareProblem(const std::string& inputRandomSeed) {
  STACK_TRACE("AnswerChecker::prepareProblem");
  std::stringstream errorStream;
  std::stringstream comments;
  this->startTime = global.getElapsedMilliseconds();
  this->problem.loadCurrentProblemItem(
    global.userRequestRequiresLoadingRealExamData(),
    inputRandomSeed,
    &errorStream
  );
  if (!this->problem.flagLoadedSuccessfully) {
    errorStream << "Failed to load current problem. ";
    this->result[WebAPI::Result::error] = errorStream.str();
    return false;
  }
  if (!this->problem.parseHTMLPrepareCommands(&comments)) {
    errorStream << "<b>Failed to parse problem. </b>";
    this->result[WebAPI::Result::error] = errorStream.str();
    this->result[WebAPI::Result::comments] = comments.str();
    return false;
  }
  if (
    !this->problem.problemData.flagRandomSeedGiven &&
    !this->problem.flagIsForReal
  ) {
    errorStream << "<b>Random seed not given.</b>";
    this->result[WebAPI::Result::error] = errorStream.str();
    return false;
  }
  if (this->problem.fileName == "") {
    global.fatal << "This shouldn't happen: empty file name." << global.fatal;
  }
  return true;
}

bool AnswerChecker::extractStudentAnswerPartOne() {
  STACK_TRACE("AnswerChecker::extractStudentAnswerPartOne");
  std::string studentAnswerNameReader;
  this->problem.studentTagsAnswered.initialize(
    this->problem.problemData.answers.size()
  );
  MapList<std::string, std::string, HashFunctions::hashFunction<std::string> >&
  webArguments =
  global.webArguments;
  this->answerIndex = - 1;
  for (int i = 0; i < webArguments.size(); i ++) {
    if (
      !StringRoutines::stringBeginsWith(
        webArguments.keys[i],
        WebAPI::Problem::calculatorAnswerPrefix,
        &studentAnswerNameReader
      )
    ) {
      continue;
    }
    int newAnswerIndex =
    this->problem.getAnswerIndex(studentAnswerNameReader);
    if (this->answerIndex == - 1) {
      this->answerIndex = newAnswerIndex;
    } else if (
      this->answerIndex != newAnswerIndex &&
      this->answerIndex != - 1 &&
      newAnswerIndex != - 1
    ) {
      std::stringstream out;
      out
      << "<b>You submitted two or more answers [answer tags: "
      << this->problem.problemData.answers.values[this->answerIndex].answerId
      << " and "
      << this->problem.problemData.answers.values[newAnswerIndex].answerId
      << "].</b> At present, multiple answer submission is not supported. ";
      this->result[WebAPI::Result::resultHtml] = out.str();
      return false;
    }
    if (this->answerIndex == - 1) {
      std::stringstream out;
      out
      << "<b> You submitted an answer to tag with id "
      << studentAnswerNameReader
      << " which is not on my list of answerable tags. </b>";
      this->result[WebAPI::Result::resultHtml] = out.str();
      return false;
    }
    Answer& currentProblemData =
    this->problem.problemData.answers.values[this->answerIndex];
    currentProblemData.currentAnswerURLed = webArguments.values[i];
    if (currentProblemData.currentAnswerURLed == "") {
      std::stringstream out;
      out
      << "<b> Your answer to tag with id "
      << studentAnswerNameReader
      << " appears to be empty, please resubmit. </b>";
      this->result[WebAPI::Result::resultHtml] = out.str();
      return false;
    }
  }
  if (this->answerIndex == - 1) {
    std::stringstream out;
    out << "<b>Something is wrong: I found no submitted answers.</b>";
    this->result[WebAPI::Result::resultHtml] = out.str();
    return false;
  }
  return true;
}

bool AnswerChecker::extractStudentAnswerPartTwo() {
  STACK_TRACE("AnswerChecker::extractStudentAnswerPartTwo");
  ProblemData& currentProblemData = this->problem.problemData;
  Answer& currentAnswer =
  currentProblemData.answers.values[this->answerIndex];
  currentAnswer.currentAnswerClean =
  HtmlRoutines::convertURLStringToNormal(
    currentAnswer.currentAnswerURLed, false
  );
  currentAnswer.currentAnswerURLed =
  HtmlRoutines::convertStringToURLString(
    currentAnswer.currentAnswerClean, false
  );
  // <-encoding back to overwrite malformed input
  this->problem.studentTagsAnswered.addSelectionAppendNewIndex(
    this->answerIndex
  );
  return true;
}

bool AnswerChecker::storeInDatabase(bool answerIsCorrect) {
  STACK_TRACE("AnswerChecker::storeInDatabase");
  if (!this->problem.flagIsForReal) {
    return true;
  }
  ProblemData& currentProblemData = this->problem.problemData;
  Answer& currentAnswer =
  currentProblemData.answers.values[this->answerIndex];
  UserCalculator& user = this->problem.currentUser;
  user.::UserCalculatorData::operator=(global.userDefault);
  bool deadLinePassed = false;
  bool hasDeadline = true;
  double secondsTillDeadline = - 1;
  std::stringstream out;
  if (!this->problem.loadAndParseTopicList(out)) {
    hasDeadline = false;
  }
  std::string sqlString;
  sqlString = user.sectionComputed;
  if (hasDeadline) {
    bool unused = false;
    std::string deadlineString =
    this->problem.getDeadline(
      this->problem.fileName,
      HtmlRoutines::convertStringToURLString(sqlString, false),
      unused
    );
    if (deadlineString == "" || deadlineString == " ") {
      hasDeadline = false;
    } else {
      TimeWrapper now;
      TimeWrapper deadline;
      // <-needs a fix for different time formats.
      // <-For the time being, we hard-code it
      // to month/day/year format (no time to program it better).
      std::stringstream badDateStream;
      if (!deadline.assignMonthDayYear(deadlineString, badDateStream)) {
        out
        << "<b>Problem reading deadline. </b> The deadline string was: "
        << deadlineString
        << ". Comments: "
        << "<span style='color:red'>"
        << badDateStream.str()
        << "</span>. "
        << "This should not happen. "
        << CalculatorHTML::bugsGenericMessage
        << "";
        this->result[WebAPI::Result::resultHtml] = out.str();
        return false;
      }
      now.assignLocalTime();
      secondsTillDeadline =
      deadline.subtractAnotherTimeFromMeInSeconds(now) + 7 * 3600;
      deadLinePassed = (secondsTillDeadline < - 18000);
    }
  }
  if (deadLinePassed) {
    out
    << "<b style='color:red'>"
    << "Deadline passed, attempt not recorded."
    << "</b>";
  } else {
    currentAnswer.numberOfSubmissions ++;
    if (answerIsCorrect) {
      currentAnswer.numberOfCorrectSubmissions ++;
      if (currentAnswer.firstCorrectAnswerClean == "") {
        currentAnswer.firstCorrectAnswerClean =
        currentAnswer.currentAnswerClean;
      } else {
        out
        << "[first correct answer: "
        << currentAnswer.firstCorrectAnswerClean
        << "]";
      }
    }
  }
  std::stringstream comments;
  user.setProblemData(this->problem.fileName, currentProblemData);
  if (!user.storeProblemData(this->problem.fileName, &comments)) {
    out
    << "<b>This shouldn't happen and may be a bug: "
    << "failed to store your answer in the database. "
    << CalculatorHTML::bugsGenericMessage
    << "</b><br>Comments: "
    << comments.str();
  } else {
    out
    << "So far "
    << currentAnswer.numberOfCorrectSubmissions
    << " correct and "
    << currentAnswer.numberOfSubmissions -
    currentAnswer.numberOfCorrectSubmissions
    << " incorrect submissions. ";
  }
  if (hasDeadline) {
    if (secondsTillDeadline < 0) {
      secondsTillDeadline *= - 1;
    }
    if (deadLinePassed) {
      out
      << "<b style='color:red'>Submission "
      << TimeWrapper::toStringSecondsToDaysHoursSecondsString(
        secondsTillDeadline, false, false
      )
      << " after deadline. </b>";
    } else {
      out
      << "<b style='color:green'>Submission "
      << TimeWrapper::toStringSecondsToDaysHoursSecondsString(
        secondsTillDeadline, false, false
      )
      << " before deadline. </b>";
    }
  } else {
    out << "<b style='color:green'>" << "No deadline yet." << "</b>";
  }
  this->storageReport = out.str();
  return true;
}

bool AnswerChecker::checkAnswerHardcoded(bool* outputIsCorrect) {
  STACK_TRACE("AnswerChecker::checkAnswerHardcoded");
  ProblemData& currentProblemData = this->problem.problemData;
  Answer& answer = currentProblemData.answers.values[this->answerIndex];
  this->checker.interpreter.initialize(Calculator::Mode::educational);
  this->checker.interpreter.flagWriteLatexPlots = false;
  this->checker.interpreter.flagPlotNoControls = true;
  CompareExpressions comparison(true);
  comparison.compare(
    answer.currentAnswerClean,
    answer.commandAnswerOnGiveUp,
    this->checker.interpreter
  );
  this->checker.answerIsCorrect = comparison.flagAreEqual;
  if (outputIsCorrect != nullptr) {
    *outputIsCorrect = this->checker.answerIsCorrect;
  }
  this->result = comparison.toJSON();
  if (this->result.hasKey(WebAPI::Result::resultHtml)) {
    this->checker.verification +=
    this->result.getValue(WebAPI::Result::resultHtml).stringValue;
  }
  return
  comparison.syntaxErrorsLeftRaw == "" &&
  comparison.syntaxErrorsRightRaw == "" &&
  comparison.errorEvaluation == "";
}

AnswerCheckerNoProblem::AnswerCheckerNoProblem() {
  this->answerIsCorrect = false;
  this->hideGivenAnswer = true;
  this->hasCommentsBeforeSubmission = false;
}

bool AnswerChecker::checkAnswerStandard(bool* outputIsCorrect) {
  STACK_TRACE("AnswerChecker::checkAnswerStandard");
  ProblemData& currentProblemData = this->problem.problemData;
  Answer& answer = currentProblemData.answers.values[this->answerIndex];
  this->checker.commandsBeforeAnswer = answer.commandsBeforeAnswer;
  this->checker.commandsBeforeAnswerNoEnclosuresInternal =
  answer.commandsBeforeAnswerNoEnclosuresInternal;
  this->checker.answerId = answer.answerId;
  this->checker.answerGiven = answer.currentAnswerClean;
  this->checker.commandsCommentsBeforeSubmission =
  answer.commandsCommentsBeforeSubmission;
  this->checker.answerCheck = answer.commandVerificationOnly;
  bool result = this->checker.checkAnswer(outputIsCorrect);
  return result;
}

JSData AnswerCheckerNoProblem::toJSON(bool hideGivenAnswer) {
  (void) hideGivenAnswer;
  JSData result;
  if (this->errorSyntax != "") {
    result[WebAPI::Result::resultHtml] = this->errorSyntax;
  }
  return result;
}

void AnswerCheckerNoProblem::prepareForEvaluation() {
  std::stringstream completedProblemStreamNoEnclosures;
  std::stringstream completedProblemStream;
  completedProblemStream
  << Calculator::Functions::Names::commandEnclosure
  << "{}("
  << this->commandsBeforeAnswer
  << ");";
  completedProblemStreamNoEnclosures
  << this->commandsBeforeAnswerNoEnclosuresInternal;
  completedProblemStream
  << Calculator::Functions::Names::commandEnclosure
  << "{}("
  << this->answerId
  << "= ("
  << this->answerGiven
  << ");"
  << ");";
  completedProblemStreamNoEnclosures
  << this->answerId
  << "= ("
  << this->answerGiven
  << ");";
  this->hasCommentsBeforeSubmission = (
    StringRoutines::stringTrimWhiteSpace(
      this->commandsCommentsBeforeSubmission
    ) !=
    ""
  );
  if (this->hasCommentsBeforeSubmission) {
    completedProblemStream
    << Calculator::Functions::Names::commandEnclosure
    << "{}("
    << this->commandsCommentsBeforeSubmission
    << ");";
    completedProblemStreamNoEnclosures
    << this->commandsCommentsBeforeSubmission;
  }
  completedProblemStream
  << SyntacticElementHTML::cleanUpCommandString(this->answerCheck);
  completedProblemStreamNoEnclosures
  << SyntacticElementHTML::cleanUpCommandString(this->answerCheck);
  if (global.userDebugFlagOn() && global.userDefaultHasAdminRights()) {
    this->administratorViewInternals +=
    HtmlRoutines::getCalculatorComputationAnchorSameURL(
      completedProblemStreamNoEnclosures.str(), "Input, no enclosures. "
    );
  }
  this->completedProblem = completedProblemStream.str();
  this->completedProblemNoEnclosures =
  completedProblemStreamNoEnclosures.str();
}

bool AnswerCheckerNoProblem::checkAnswer(bool* outputIsCorrect) {
  STACK_TRACE("AnswerCheckerNoProblem::checkAnswer");
  this->prepareForEvaluation();
  this->interpreter.initialize(Calculator::Mode::educational);
  this->interpreter.flagWriteLatexPlots = false;
  this->interpreter.flagPlotNoControls = true;
  this->interpreter.evaluate(this->completedProblem);
  this->answerIsCorrect = false;
  if (
    this->interpreter.flagAbortComputationASAP ||
    this->interpreter.parser.syntaxErrors != ""
  ) {
    std::stringstream out;
    if (this->interpreter.errorsPublic.str() != "") {
      out
      << "While checking your answer, got the error: "
      << "<br><b style='color:red'>"
      << this->interpreter.errorsPublic.str()
      << "</b> "
      << "<br><b>Most likely your answer is wrong. </b>";
    } else {
      out
      << "<b>Error while processing your answer(s).</b> "
      << "<br>Perhaps your answer was wrong and "
      << "generated division by zero during checking. ";
    }
    out << "<br>Here's what I understood. ";
    Calculator isolatedInterpreter;
    isolatedInterpreter.initialize(Calculator::Mode::educational);
    isolatedInterpreter.flagWriteLatexPlots = false;
    isolatedInterpreter.flagPlotNoControls = true;
    isolatedInterpreter.evaluate("(" + this->answerGiven + ")");
    if (isolatedInterpreter.parser.syntaxErrors != "") {
      out << isolatedInterpreter.parser.toStringSyntacticStackHTMLSimple();
    } else {
      out << isolatedInterpreter.outputHTMLString;
    }
    if (global.userDebugFlagOn() && global.userDefaultHasAdminRights()) {
      out
      << "<hr><b>Administartor view internals.</b><hr>"
      << this->administratorViewInternals
      << "<hr>"
      << interpreter.outputHTMLString
      << "<br>"
      << interpreter.outputCommentsString
      << "<hr>Input, no enclosures: <hr>"
      << this->completedProblemNoEnclosures
      << "<br>";
    }
    this->errorSyntax = out.str();
    return false;
  }
  this->answerIsCorrect = false;
  *outputIsCorrect = false;
  int mustBeOne = - 1;
  Expression last =
  this->interpreter.programExpression[
    this->interpreter.programExpression.size() - 1
  ];
  if (!last.isSmallInteger(&mustBeOne)) {
    this->answerIsCorrect = false;
  } else {
    this->answerIsCorrect = (mustBeOne == 1);
  }
  if (outputIsCorrect != nullptr) {
    *outputIsCorrect = this->answerIsCorrect;
  }
  this->computeVerificationString();
  return true;
}

void AnswerCheckerNoProblem::computeVerificationString() {
  std::stringstream out;
  out << "<table style='width:300px'>";
  if (!this->answerIsCorrect) {
    out << "<tr><td>";
    out << "<b style='color:red'>Your answer appears to be incorrect. </b>";
    out << "</td></tr>";
    if (global.userDefaultHasAdminRights() && global.userDebugFlagOn()) {
      out
      << "<tr><td>Administrator view internals. "
      << "<hr>"
      << this->administratorViewInternals
      << "<br>The calculator output is: "
      << this->interpreter.outputHTMLString
      << "Comments: "
      << this->interpreter.comments.str()
      << "<hr>Input, no enclosures: <hr>"
      << this->completedProblemNoEnclosures;
      out
      << "<hr>Input, full:<hr>"
      << this->interpreter.inputString
      << "<hr></td></tr>";
    }
  } else {
    out << "<tr><td><b style='color:green'>Correct!</b>" << "</td></tr>";
  }
  if (this->hasCommentsBeforeSubmission) {
    FormatExpressions format;
    out
    << "<tr><td>"
    << WebAPIResponse::getCommentsInterpretation(this->interpreter, 3, format)
    << "</td></tr>\n";
  }
  out << "<tr><td>Your answer was: ";
  out << "\\(";
  out << this->answerGiven;
  out << "\\)";
  std::string errorMessage;
  errorMessage =
  this->interpreter.parser.toStringIsCorrectAsciiCalculatorString(
    this->answerGiven
  );
  if (errorMessage != "") {
    out
    << "<br>"
    << errorMessage
    << "<hr><b>If you entered this expression through the "
    << "keyboard (without copying + pasting) this is a bug: "
    << "please report it to the web site administrator. "
    << "Don't forget to mention your keyboard/character setup. "
    << "Are you using the standard English keyboard? "
    << "Cyrillic, Chinese, etc. characters are not accepted. </b> "
    << "<hr><b style='color:red'>Copying and pasting an answer "
    << "not computed by yourself is considered cheating "
    << "(example: answer from an online program for doing homework).</b>";
  }
  out << "</td></tr>";
  out << "</table>";
  this->verification += out.str();
}

JSData WebAPIResponse::checkAnswer(bool hideDesiredAnswer) {
  AnswerCheckerNoProblem checker;
  checker.answerId =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Problem::answerId), false
  );
  checker.answerGiven =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Problem::answerValue), false
  );
  checker.answerCheck =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Problem::answerCheck), false
  );
  checker.checkAnswer(&checker.answerIsCorrect);
  return checker.toJSON(hideDesiredAnswer);
}

JSData AnswerChecker::submitAnswersJSON(
  const std::string& inputRandomSeed,
  bool* outputIsCorrect,
  bool timeSafetyBrake
) {
  STACK_TRACE("AnswerChecker::submitAnswersJSON");
  if (!global.userDefaultHasAdminRights()) {
    global.response.disallowReport();
  }
  if (!this->prepareProblem(inputRandomSeed)) {
    return this->result;
  }
  if (!this->extractStudentAnswerPartOne()) {
    return this->result;
  }
  if (!this->extractStudentAnswerPartTwo()) {
    return this->result;
  }
  if (!this->problem.flagIsForReal) {
    this->result[WebAPI::Problem::randomSeed] = inputRandomSeed;
  }
  ProblemData& currentProblemData = this->problem.problemData;
  Answer& currentAnswer =
  currentProblemData.answers.values[this->answerIndex];
  bool errorsCheckingAnswer = false;
  if (timeSafetyBrake) {
    global.millisecondsMaxComputation =
    global.getElapsedMilliseconds() + 20000;
    // + 20 sec
  }
  bool temporary = false;
  if (outputIsCorrect == nullptr) {
    outputIsCorrect = &temporary;
  }
  if (currentAnswer.flagAnswerHardcoded) {
    errorsCheckingAnswer = this->checkAnswerHardcoded(outputIsCorrect);
  } else {
    errorsCheckingAnswer = this->checkAnswerStandard(outputIsCorrect);
  }
  if (!errorsCheckingAnswer) {
    this->computeResultHTML();
    return this->result;
  }
  if (!this->storeInDatabase(*outputIsCorrect)) {
    return this->result;
  }
  this->computeResultHTML();
  return this->result;
}

void AnswerChecker::computeResultHTML() {
  this->result[WebAPI::Result::resultHtml] =
  this->checker.verification + this->checker.errorSyntax + this->storageReport;
  this->result[WebAPI::Result::millisecondsComputation] =
  global.getElapsedMilliseconds() - this->startTime;
  this->result[WebAPI::Problem::forReal] = this->problem.flagIsForReal;
}

JSData WebAPIResponse::submitAnswersHardcoded(bool hideDesiredAnswer) {
  double startTime = global.getElapsedSeconds();
  (void) hideDesiredAnswer;
  AnswerChecker checker;
  checker.checker.answerId =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Problem::answerId), false
  );
  checker.checker.answerGiven =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Problem::answerValue), false
  );
  checker.checker.answerCheck =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Problem::answerCheck), false
  );
  checker.checker.commandsBeforeAnswer = "1";
  bool errorsCheckingAnswer =
  checker.checker.checkAnswer(&checker.checker.answerIsCorrect);
  if (!errorsCheckingAnswer) {
    return checker.checker.toJSON(hideDesiredAnswer);
  }
  checker.result[WebAPI::Result::resultHtml] =
  checker.checker.verification + checker.storageReport;
  checker.result[WebAPI::Result::millisecondsComputation] =
  global.getElapsedSeconds() - startTime;
  return checker.result;
}

JSData WebAPIResponse::submitAnswersJSON(
  const std::string& inputRandomSeed,
  bool* outputIsCorrect,
  bool timeSafetyBrake
) {
  AnswerChecker checker;
  return
  checker.submitAnswersJSON(inputRandomSeed, outputIsCorrect, timeSafetyBrake);
}

std::string WebAPIResponse::addTeachersSections() {
  STACK_TRACE("WebAPIReponse::addTeachersSections");
  std::stringstream out;
  if (!global.flagLoggedIn || global.userDefault.username == "") {
    out
    << "To set teachers you must first log in. Username: ["
    << global.userDefault.username
    << "].";
    return out.str();
  }
  if (!global.flagUsingSSLinCurrentConnection) {
    out << "Setting teachers only allowed over a secure connection.";
    return out.str();
  }
  if (!global.userDefaultHasAdminRights()) {
    out << "Only admins may assign sections to teachers.";
    return out.str();
  }
  std::string input =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Request::teachersAndSections), false
  );
  JSData inputParsed;
  if (!inputParsed.parse(input, false, &out)) {
    out << "<b style='color:red'>Failed to interpret your input. </b>";
    return out.str();
  }
  if (
    inputParsed[WebAPI::Request::teachers].elementType !=
    JSData::Type::tokenString
  ) {
    out
    << "<b style='color:red'>Failed to extract key "
    << WebAPI::Request::teachers
    << " from your input. </b>";
    return out.str();
  }
  if (
    inputParsed[WebAPI::Request::sections].elementType !=
    JSData::Type::tokenString
  ) {
    out
    << "<b style='color:red'>Failed to find key "
    << WebAPI::Request::sections
    << " in your input. </b>";
    return out.str();
  }
  std::string desiredUsers =
  HtmlRoutines::convertURLStringToNormal(
    inputParsed[WebAPI::Request::teachers].stringValue, false
  );
  std::string desiredSectionsOneString =
  HtmlRoutines::convertURLStringToNormal(
    inputParsed[WebAPI::Request::sections].stringValue, false
  );
  if (desiredUsers == "") {
    return "Empty user list not allowed.";
  }
  List<std::string> desiredSectionsList;
  List<std::string> teachers;
  List<char> delimiters;
  delimiters.addOnTop(' ');
  delimiters.addOnTop('\r');
  delimiters.addOnTop('\n');
  delimiters.addOnTop('\t');
  delimiters.addOnTop(',');
  delimiters.addOnTop(';');
  delimiters.addOnTop(static_cast<char>(160));
  // <-&nbsp
  StringRoutines::splitExcludeDelimiters(
    desiredSectionsOneString, delimiters, desiredSectionsList
  );
  StringRoutines::splitExcludeDelimiters(desiredUsers, delimiters, teachers);
  if (teachers.size == 0) {
    out << "<b>Could not extract teachers from " << desiredUsers << ".</b>";
    return out.str();
  }
  UserCalculator currentTeacher;
  for (int i = 0; i < teachers.size; i ++) {
    currentTeacher.reset();
    currentTeacher.username = teachers[i];
    if (!currentTeacher.loadFromDatabase(&out, &out)) {
      out
      << "<span style='color:red'>Failed to fetch teacher: "
      << teachers[i]
      << "</span><br>";
      continue;
    }
    currentTeacher.sectionsTaught = desiredSectionsList;
    QueryFind findQuery(
      DatabaseStrings::tableUsers,
      DatabaseStrings::labelUsername,
      currentTeacher.username
    );
    QueryUpdate setQuery;
    JSData desiredSectionListJSON;
    desiredSectionListJSON = desiredSectionsList;
    setQuery.addKeyValuePair(
      DatabaseStrings::labelSectionsTaught, desiredSectionListJSON
    );
    if (!Database::get().updateOne(findQuery, setQuery, true, &out)) {
      out
      << "<span style='color:red'>Failed to store course info of instructor: "
      << teachers[i]
      << ". </span><br>";
    } else {
      out
      << "<span style='color:green'>Assigned "
      << teachers[i]
      << " to section(s): "
      << desiredSectionsList
      << "</span><br>";
    }
  }
  return out.str();
}

std::string WebAPIResponse::addUsersOrEmails(
  const std::string& hostWebAddressWithPort
) {
  STACK_TRACE("WebAPIReponse::addUsersOrEmails");
  (void) hostWebAddressWithPort;
  std::stringstream out;
  if (
    !global.userDefaultHasAdminRights() ||
    !global.flagUsingSSLinCurrentConnection
  ) {
    out << "<b>Only admins may add users, under ssl connection. </b>";
    return out.str();
  }
  std::string inputEmails =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Request::userList), false
  );
  std::string userPasswords =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Request::passwordList), false
  );
  std::string userGroup =
  StringRoutines::stringTrimWhiteSpace(
    HtmlRoutines::convertURLStringToNormal(
      global.getWebInput(DatabaseStrings::labelSection), false
    )
  );
  std::string userRole =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(DatabaseStrings::labelUserRole), false
  );
  if (inputEmails == "") {
    out << "addUserEmails failed: <b>No emails to add</b>";
    return out.str();
  }
  std::stringstream comments;
  bool sentEmails = true;
  bool doSendEmails = global.requestType == "sendEmails" ? true : false;
  int numberOfNewUsers = 0;
  int numberOfUpdatedUsers = 0;
  bool createdUsers =
  WebAPIResponse::addUsersFromData(
    inputEmails,
    userPasswords,
    userRole,
    userGroup,
    numberOfNewUsers,
    numberOfUpdatedUsers,
    &comments
  );
  if (createdUsers) {
    out
    << "<span style='color:green'>Success: "
    << numberOfNewUsers
    << " new users and "
    << numberOfUpdatedUsers
    << " user updates.</span> User roles: "
    << userRole;
  } else {
    out
    << "<b style='color:red'>Failed to add all users.</b> "
    << "Errors follow.<hr>"
    << comments.str()
    << "<hr>";
  }
  if (doSendEmails) {
    if (sentEmails) {
      out
      << "<b style='color:green'>"
      << "Activation emails successfully sent. "
      << "</b>";
    } else {
      out
      << "<b style='color:red'>"
      << "Failed to send all activation emails. "
      << "</b>";
    }
  }
  return out.str();
}

JSData WebAPIResponse::getAnswerOnGiveUp() {
  return
  WebAPIResponse::getAnswerOnGiveUp(
    global.getWebInput(WebAPI::Problem::randomSeed)
  );
}

JSData WebAPIResponse::getAnswerOnGiveUp(
  const std::string& inputRandomSeed,
  std::string* outputNakedAnswer,
  bool* answerGenerationSuccess,
  bool doIncludeTimeStats
) {
  STACK_TRACE("WebAPIResponse::getAnswerOnGiveUp");
  GlobalVariables::Response::StateMaintainer maintain(global.response);
  if (!global.userDefaultHasAdminRights()) {
    global.response.disallowReport();
  }
  if (outputNakedAnswer != nullptr) {
    *outputNakedAnswer = "";
  }
  JSData result;
  result[WebAPI::Problem::answerGenerationSuccess] = "false";
  if (answerGenerationSuccess != nullptr) {
    *answerGenerationSuccess = false;
  }
  int64_t startTimeInMilliseconds = global.getElapsedMilliseconds();
  CalculatorHTML problem;
  std::stringstream errorStream;
  problem.loadCurrentProblemItem(false, inputRandomSeed, &errorStream);
  if (!problem.flagLoadedSuccessfully) {
    errorStream
    << "Problem name is: "
    << problem.fileName
    << " <b>Could not load problem, this may be a bug. "
    << CalculatorHTML::bugsGenericMessage
    << "</b>";
    result[WebAPI::Result::error] = errorStream.str();
    return result;
  }
  if (problem.flagIsForReal) {
    errorStream
    << " <b>Not allowed to show answer "
    << "of a problem being tested for real. </b>";
    result[WebAPI::Result::error] = errorStream.str();
    return result;
  }
  if (inputRandomSeed == "") {
    result[WebAPI::Result::error] =
    "<b>I could not figure out "
    "the exercise problem (missing random seed). </b>";
    return result;
  }
  result[WebAPI::Problem::randomSeed] = inputRandomSeed;
  if (!problem.parseHTMLPrepareCommands(&errorStream)) {
    errorStream << "<br><b>Problem preparation failed.</b>";
    result[WebAPI::Result::error] = errorStream.str();
    return result;
  }
  std::string lastStudentAnswerId;
  MapList<std::string, std::string, HashFunctions::hashFunction<std::string> >&
  arguments =
  global.webArguments;
  for (int i = 0; i < arguments.size(); i ++) {
    StringRoutines::stringBeginsWith(
      arguments.keys[i],
      WebAPI::Problem::calculatorAnswerPrefix,
      &lastStudentAnswerId
    );
  }
  int indexLastAnswerId = problem.getAnswerIndex(lastStudentAnswerId);
  std::stringstream out;
  if (indexLastAnswerId == - 1) {
    errorStream
    << "File: "
    << problem.fileName
    << "<br><b>Student submitted answerID: "
    << lastStudentAnswerId
    << " but that is not an ID of an answer tag. "
    << "</b>";
    if (global.userDebugFlagOn() && global.userDefaultHasAdminRights()) {
      errorStream
      << "<hr>"
      << problem.problemData.toStringAvailableAnswerIds();
    }
    result[WebAPI::Result::millisecondsComputation] =
    global.getElapsedMilliseconds() - startTimeInMilliseconds;
    result[WebAPI::Result::error] = errorStream.str();
    return result;
  }
  Answer& currentAnswer =
  problem.problemData.answers.values[indexLastAnswerId];
  if (currentAnswer.commandAnswerOnGiveUp == "") {
    out
    << "<b>No answer given for "
    << "question (answerID: "
    << lastStudentAnswerId
    << ").</b>";
    if (
      global.userDebugFlagOn() && global.userDefaultHasProblemComposingRights()
    ) {
      out << "<br>Answer status: " << currentAnswer.toString();
    }
    result[WebAPI::Result::error] = out.str();
    return result;
  }
  Calculator interpreter;
  interpreter.initialize(Calculator::Mode::educational);
  interpreter.flagPlotNoControls = true;
  interpreter.flagWriteLatexPlots = false;
  std::stringstream answerCommands;
  std::stringstream answerCommandsNoEnclosure;
  answerCommands << currentAnswer.commandsBeforeAnswer;
  answerCommandsNoEnclosure
  << currentAnswer.commandsBeforeAnswerNoEnclosuresInternal;
  answerCommands
  << Calculator::Functions::Names::commandEnclosure
  << "{}("
  << currentAnswer.commandAnswerOnGiveUp
  << ");";
  answerCommandsNoEnclosure << currentAnswer.commandAnswerOnGiveUp;
  interpreter.evaluate(answerCommands.str());
  if (interpreter.parser.syntaxErrors != "") {
    out
    << "<b style='color:red'>Failed to evaluate the default answer. "
    << "Likely there is a bug with the problem. </b>";
    if (global.userDefaultHasProblemComposingRights()) {
      out
      << HtmlRoutines::getCalculatorComputationAnchorSameURL(
        answerCommandsNoEnclosure.str(), "Calculator input no enclosures"
      );
    }
    out
    << "<br>"
    << CalculatorHTML::bugsGenericMessage
    << "<br>Details: <br>"
    << interpreter.parser.toStringSyntacticStackHTMLSimple();
    result[WebAPI::Result::resultHtml] = out.str();
    int64_t elapsedTime =
    global.getElapsedMilliseconds() - startTimeInMilliseconds;
    result[WebAPI::Result::millisecondsComputation] = elapsedTime;
    return result;
  }
  if (interpreter.flagAbortComputationASAP) {
    out
    << "<b style='color:red'>Failed to evaluate the default answer. "
    << "Likely there is a bug with the problem. </b>";
    if (global.userDefaultHasProblemComposingRights()) {
      out
      << HtmlRoutines::getCalculatorComputationAnchorSameURL(
        answerCommandsNoEnclosure.str(), "Calculator input no enclosures"
      );
    }
    out
    << "<br>"
    << CalculatorHTML::bugsGenericMessage
    << "<br>Details: <br>"
    << interpreter.outputHTMLString
    << interpreter.outputCommentsString
    << "<hr>Input: <br>"
    << interpreter.inputString;
    int64_t elapsedTime =
    global.getElapsedMilliseconds() - startTimeInMilliseconds;
    result[WebAPI::Result::millisecondsComputation] = elapsedTime;
    result[WebAPI::Result::resultHtml] = out.str();
    return result;
  }
  FormatExpressions format;
  format.flagExpressionIsFinal = true;
  format.flagIncludeExtraHtmlDescriptionsInPlots = false;
  format.flagUseQuotes = false;
  format.flagUseLatex = true;
  format.flagUsePmatrix = true;
  bool isFirst = true;
  const Expression& currentExpression =
  interpreter.programExpression[interpreter.programExpression.size() - 1][1];
  if (!currentExpression.startsWith(interpreter.opCommandSequence())) {
    out
    << "\\(\\displaystyle "
    << currentExpression.toString(&format)
    << "\\)";
    if (outputNakedAnswer != nullptr) {
      *outputNakedAnswer = currentExpression.toString(&format);
    }
    result[WebAPI::Problem::answerGenerationSuccess] = "true";
    if (answerGenerationSuccess != nullptr) {
      *answerGenerationSuccess = true;
    }
  } else {
    for (int j = 1; j < currentExpression.size(); j ++) {
      if (
        currentExpression[j].startsWith(interpreter.opRulesOff()) ||
        currentExpression[j].startsWith(interpreter.opRulesOn())
      ) {
        continue;
      }
      std::string stringAnswer;
      if (currentExpression[j].isOfType<std::string>(&stringAnswer)) {
        if (
          StringRoutines::stringBeginsWith(
            stringAnswer, "Approximations have been"
          )
        ) {
          continue;
        }
      }
      if (!isFirst) {
        out << "<br>";
      }
      format.flagExpressionIsFinal = true;
      format.flagIncludeExtraHtmlDescriptionsInPlots = false;
      format.flagUseQuotes = false;
      format.flagUseLatex = true;
      if (currentExpression[j].isOfType<std::string>()) {
        out << currentExpression[j].getValue<std::string>();
      } else {
        out
        << "\\(\\displaystyle "
        << currentExpression[j].toString(&format)
        << "\\)";
      }
      if (isFirst) {
        if (outputNakedAnswer != nullptr) {
          *outputNakedAnswer = currentExpression[j].toString(&format);
        }
        result[WebAPI::Problem::answerGenerationSuccess] = "true";
        if (answerGenerationSuccess != nullptr) {
          *answerGenerationSuccess = true;
        }
      }
      isFirst = false;
    }
  }
  if (doIncludeTimeStats) {
    int64_t elapsedTime =
    global.getElapsedMilliseconds() - startTimeInMilliseconds;
    result[WebAPI::Result::millisecondsComputation] = elapsedTime;
  }
  if (global.userDebugFlagOn() && global.userDefaultHasAdminRights()) {
    out
    << "<hr>"
    << HtmlRoutines::getCalculatorComputationAnchorSameURL(
      answerCommandsNoEnclosure.str(), "Calculator input no enclosures"
    );
    out
    << interpreter.outputHTMLString
    << "<hr>"
    << interpreter.outputCommentsString
    << "<hr>"
    << HtmlRoutines::getCalculatorComputationAnchorSameURL(
      interpreter.inputString, "Raw calculator input"
    );
  }
  result[WebAPI::Result::resultHtml] = out.str();
  return result;
}

JSData WebAPIResponse::getAccountsPageJSON(
  const std::string& hostWebAddressWithPort
) {
  STACK_TRACE("WebAPIReponse::getAccountsPageJSON");
  (void) hostWebAddressWithPort;
  JSData output;
  if (global.hasDisabledDatabaseEveryoneIsAdmin()) {
    output[WebAPI::Result::error] =
    "Database disabled (cannot get accounts). ";
    return output;
  }
  if (
    !global.userDefaultHasAdminRights() ||
    !global.flagLoggedIn ||
    !global.flagUsingSSLinCurrentConnection
  ) {
    output[WebAPI::Result::error] =
    "Must be logged-in administrator over ssl.";
    return output;
  }
  std::stringstream commentsOnFailure;
  QueryFind findStudents;
  List<JSData> students;
  List<JSData> admins;
  findStudents.collection = DatabaseStrings::tableUsers;
  findStudents.nestedLabels.addOnTop(DatabaseStrings::labelInstructor);
  findStudents.exactValue = global.userDefault.username;
  findStudents.maximumNumberOfItems = 1000;
  QueryResultOptions columnsToRetain;
  columnsToRetain.fieldsToProjectTo.addOnTop(DatabaseStrings::labelUsername);
  columnsToRetain.fieldsToProjectTo.addOnTop(DatabaseStrings::labelEmail);
  columnsToRetain.fieldsToProjectTo.addOnTop(
    DatabaseStrings::labelInstructor
  );
  columnsToRetain.fieldsToProjectTo.addOnTop(
    DatabaseStrings::labelActivationToken
  );
  columnsToRetain.fieldsToProjectTo.addOnTop(
    DatabaseStrings::labelCurrentCourses
  );
  columnsToRetain.fieldsToProjectTo.addOnTop(
    DatabaseStrings::labelInstructor
  );
  columnsToRetain.fieldsToProjectTo.addOnTop(DatabaseStrings::labelSection);
  columnsToRetain.fieldsToProjectTo.addOnTop(DatabaseStrings::labelSemester);
  if (
    !Database::get().find(
      findStudents, &columnsToRetain, students, nullptr, &commentsOnFailure
    )
  ) {
    output["error"] =
    "Failed to load user info. Comments: " + commentsOnFailure.str();
    return output;
  }
  QueryFind findAdmins;
  findAdmins.collection = DatabaseStrings::tableUsers;
  findAdmins.nestedLabels.addOnTop(DatabaseStrings::labelUserRole);
  findAdmins.exactValue = UserCalculator::Roles::administrator;
  if (
    !Database::get().find(
      findAdmins, &columnsToRetain, admins, nullptr, &commentsOnFailure
    )
  ) {
    output["error"] =
    "Failed to load user info. Comments: " + commentsOnFailure.str();
    return output;
  }
  output["admins"] = admins;
  output["students"] = students;
  return output;
}

std::string WebAPIResponse::getScoresPage() {
  STACK_TRACE("WebAPIResponse::getScoresPage");
  std::stringstream out;
  CalculatorHTML page;
  page.loadDatabaseInfo(out);
  std::string scoresHtml = WebAPIResponse::toStringUserScores();
  out << scoresHtml;
  return out.str();
}

std::string WebAPIResponse::toStringUserDetailsTable(
  bool adminsOnly,
  List<JSData>& users,
  const std::string& hostWebAddressWithPort
) {
  STACK_TRACE("WebAPIReponse::toStringUserDetailsTable");
  std::stringstream out;
  bool flagFilterCourse = (!adminsOnly) && (
    global.getWebInput("filterAccounts") == "true"
  );
  std::string currentCourse =
  HtmlRoutines::convertURLStringToNormal(
    global.getWebInput(WebAPI::Problem::courseHome), false
  );
  if (flagFilterCourse) {
    out
    << "<br>Displaying only students in course: <b style='color:blue'>"
    << currentCourse
    << "</b>. "
    << "<a href=\""
    << global.displayNameExecutable
    << "?request=accounts&"
    << global.toStringCalculatorArgumentsNoNavigation(nullptr)
    << "filterAccounts=false&"
    << "\">Show all. </a>"
    << "<br>";
  }
  UserCalculator currentUser;
  HashedList<std::string> sections;
  List<std::string> sectionDescriptions;
  List<List<std::string> > activatedAccountBucketsBySection;
  List<List<std::string> > preFilledLinkBucketsBySection;
  List<List<std::string> > nonActivatedAccountBucketsBySection;
  for (int i = 0; i < users.size; i ++) {
    currentUser.loadFromJSON(users[i]);
    if (
      flagFilterCourse && (
        currentUser.courseInDB != currentCourse ||
        currentUser.instructorInDB != global.userDefault.username
      )
    ) {
      continue;
    }
    if (!sections.contains(currentUser.sectionInDB)) {
      std::stringstream currentSectionInfo;
      currentSectionInfo
      << "<b>Section: </b>"
      << currentUser.sectionInDB
      << ", <b>Course: </b>"
      << currentUser.courseInDB
      << ", <b>Instructor: </b>"
      << currentUser.instructorInDB;
      sections.addOnTop(currentUser.sectionInDB);
      sectionDescriptions.addOnTop(currentSectionInfo.str());
    }
  }
  sections.quickSortAscending(nullptr, &sectionDescriptions);
  activatedAccountBucketsBySection.setSize(sections.size);
  nonActivatedAccountBucketsBySection.setSize(sections.size);
  preFilledLinkBucketsBySection.setSize(sections.size);
  int totalActivatedUsers = 0;
  for (int i = 0; i < users.size; i ++) {
    currentUser.loadFromJSON(users[i]);
    if (currentUser.courseInDB.find('%') != std::string::npos) {
      out
      << "<b style='color:red'>Non-expected behavior: user: "
      << currentUser.username
      << "current course: "
      << currentUser.courseInDB
      << " contains the % symbol. </b><br>";
    }
    std::stringstream oneTableLineStream;
    oneTableLineStream
    << "<tr>"
    << "<td>"
    << currentUser.username
    << "</td>"
    << "<td>"
    << currentUser.email
    << "</td>";
    bool isActivated = true;
    std::string webAddress =
    "https://" + hostWebAddressWithPort + "/cgi-bin/calculator";
    if (
      currentUser.actualActivationToken != "activated" &&
      currentUser.actualActivationToken != "error"
    ) {
      isActivated = false;
      totalActivatedUsers ++;
      oneTableLineStream
      << "<td><span style='color:red'>not activated</span></td>";
      std::string activationLink;
      if (currentUser.actualActivationToken != "") {
        oneTableLineStream << "<td>";
        if (
          UserCalculator::getActivationAddressFromActivationToken(
            currentUser.actualActivationToken,
            currentUser.username,
            currentUser.email,
            false,
            activationLink,
            nullptr
          )
        ) {
          oneTableLineStream
          << "<a href=\""
          << activationLink
          << "\"> (Re)activate account and change password</a>";
        } else {
          oneTableLineStream
          << "The server does not know its web address, "
          << "please set it through file configuration.json";
        }
        oneTableLineStream << "</td>";
      }
      oneTableLineStream << "<td>";
      if (
        !UserCalculator::getActivationAddressFromActivationToken(
          currentUser.actualActivationToken,
          currentUser.username,
          currentUser.email,
          false,
          activationLink,
          nullptr
        )
      ) {
        oneTableLineStream
        << "The server does not know its web address, "
        << "please set it through file configuraiton.json";
      } else {
        oneTableLineStream
        << "<a href='mailto:"
        << currentUser.email
        << "?subject=Math homework account activation&";
        oneTableLineStream << "body=";
        std::stringstream emailBody;
        emailBody
        << "Dear user,\n you have not activated "
        << "your homework server account yet. \n"
        << "To activate your account and set "
        << "your password please use the link: "
        << HtmlRoutines::convertStringToURLString("\n\n", false)
        << HtmlRoutines::convertStringToURLString(activationLink, false)
        << HtmlRoutines::convertStringToURLString("\n\n", false)
        << " Once you activate your account, you can log in safely here: \n"
        << HtmlRoutines::convertStringToURLString("\n\n", false)
        << webAddress
        << HtmlRoutines::convertStringToURLString("\n\n", false)
        << "Best regards, \ncalculator-algebra.org.";
        oneTableLineStream << emailBody.str() << "'>Send email manually.</a> ";
      }
      oneTableLineStream << "</td>";
    } else if (currentUser.actualActivationToken == "error") {
      oneTableLineStream << "<td>error</td><td></td>";
    } else {
      oneTableLineStream
      << "<td><span style='color:green'>activated</span></td>"
      << "<td></td><td></td>";
    }
    std::stringstream oneLink;
    oneLink
    << "<a href=\""
    << global.displayNameExecutable
    << "?request=login&username="
    << currentUser.username
    << "\">"
    << currentUser.username
    << "</a>";
    oneTableLineStream << "<td>" << oneLink.str() << "</td>";
    oneTableLineStream << "</tr>";
    int indexCurrentBucket = sections.getIndex(currentUser.sectionInDB);
    if (indexCurrentBucket != - 1) {
      if (isActivated) {
        activatedAccountBucketsBySection[indexCurrentBucket].addOnTop(
          oneTableLineStream.str()
        );
      } else {
        nonActivatedAccountBucketsBySection[indexCurrentBucket].addOnTop(
          oneTableLineStream.str()
        );
      }
      preFilledLinkBucketsBySection[indexCurrentBucket].addOnTop(
        oneLink.str()
      );
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
  tableStream
  << "<table><tr><th>User</th><th>Email</th>"
  << "<th>Activated?</th><th>Activation link</th>"
  << "<th>Activation manual email</th>"
  << "<th>Pre-filled login link</th><th>Course info</th></tr>";
  for (int i = 0; i < nonActivatedAccountBucketsBySection.size; i ++) {
    if (!adminsOnly) {
      if (nonActivatedAccountBucketsBySection[i].size > 0) {
        tableStream
        << "<tr><td colspan =\"6\" style =\"text-align:center\">"
        << sections[i]
        << "</td></tr>";
      }
    }
    for (int j = 0; j < nonActivatedAccountBucketsBySection[i].size; j ++) {
      tableStream << nonActivatedAccountBucketsBySection[i][j];
    }
  }
  for (int i = 0; i < activatedAccountBucketsBySection.size; i ++) {
    if (!adminsOnly) {
      if (activatedAccountBucketsBySection[i].size > 0) {
        tableStream
        << "<tr><td colspan =\"7\" style =\"text-align:center\">"
        << sections[i]
        << "</td></tr>";
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
        preFilledLoginLinks << sections[i] << "<br>";
      }
      for (int j = 0; j < preFilledLinkBucketsBySection[i].size; j ++) {
        preFilledLoginLinks << preFilledLinkBucketsBySection[i][j] << "<br>";
      }
    }
  }
  out << "\n" << users.size << " user(s)";
  if (totalActivatedUsers > 0) {
    out
    << ", <span style='color:red'>"
    << totalActivatedUsers
    << " have not activated their accounts. </span>";
  }
  out << tableStream.str() << preFilledLoginLinks.str();
  return out.str();
}

void ProblemData::readExpectedNumberOfAnswersFromDatabase(
  std::stringstream& commentsOnFailure
) {
  STACK_TRACE("ProblemData::readExpectedNumberOfAnswersFromDatabase");
  if (global.problemExpectedNumberOfAnswers.size() != 0) {
    // Expected number of answers is already computed.
    return;
  }
  List<JSData> result;
  QueryResultOptions options;
  options.fieldsToProjectTo.addOnTop(DatabaseStrings::labelProblemFileName);
  options.fieldsToProjectTo.addOnTop(
    DatabaseStrings::labelProblemTotalQuestions
  );
  QueryFind findProblemInfo;
  findProblemInfo.collection = DatabaseStrings::tableProblemInformation;
  if (
    !Database::get().find(
      findProblemInfo, &options, result, nullptr, &commentsOnFailure
    )
  ) {
    return;
  }
  for (int i = 0; i < result.size; i ++) {
    const std::string& currentProblemName =
    result[i][DatabaseStrings::labelProblemFileName].stringValue;
    if (currentProblemName == "") {
      continue;
    }
    const std::string& expectedNumberOfAnswersString =
    result[i][DatabaseStrings::labelProblemTotalQuestions].stringValue;
    if (expectedNumberOfAnswersString == "") {
      continue;
    }
    std::stringstream stringStream(expectedNumberOfAnswersString);
    int numberOfAnswers = - 1;
    stringStream >> numberOfAnswers;
    if (numberOfAnswers == - 1) {
      continue;
    }
    global.problemExpectedNumberOfAnswers.setKeyValue(
      currentProblemName, numberOfAnswers
    );
  }
}

int ProblemData::getExpectedNumberOfAnswers(
  const std::string& problemName, std::stringstream& commentsOnFailure
) {
  STACK_TRACE("ProblemData::getExpectedNumberOfAnswers");
  if (this->knownNumberOfAnswersFromHD != - 1) {
    return this->knownNumberOfAnswersFromHD;
  }
  this->readExpectedNumberOfAnswersFromDatabase(commentsOnFailure);
  if (global.problemExpectedNumberOfAnswers.contains(problemName)) {
    return
    global.problemExpectedNumberOfAnswers.getValueCreate(problemName, 0);
  }
  global
  << Logger::yellow
  << "Couldn't find problem info in DB for: "
  << problemName
  << ", trying to read problem from hd. "
  << Logger::endL;
  CalculatorHTML problemParser;
  problemParser.fileName = problemName;
  if (!problemParser.loadMe(false, "", &commentsOnFailure)) {
    global
    << Logger::yellow
    << WebAPI::Problem::failedToLoadProblem
    << commentsOnFailure.str()
    << Logger::endL;
    return 0;
  }
  if (!problemParser.parseHTML(&commentsOnFailure)) {
    global
    << Logger::red
    << "<b>Failed to parse file: "
    << problemParser.fileName
    << ".</b> Details:<br>"
    << commentsOnFailure.str();
    return 0;
  }
  this->knownNumberOfAnswersFromHD = problemParser.problemData.answers.size();
  global
  << Logger::yellow
  << "Loaded problem: "
  << problemName
  << "; number of answers: "
  << this->knownNumberOfAnswersFromHD
  << Logger::endL;
  this->expectedNumberOfAnswersFromDB = this->knownNumberOfAnswersFromHD;
  QueryFind findEntry(
    DatabaseStrings::tableProblemInformation,
    DatabaseStrings::labelProblemFileName,
    problemName
  );
  JSData newDatabaseEntry;
  newDatabaseEntry[DatabaseStrings::labelProblemFileName] = problemName;
  std::stringstream stringConverter;
  stringConverter << this->knownNumberOfAnswersFromHD;
  newDatabaseEntry[DatabaseStrings::labelProblemTotalQuestions] =
  stringConverter.str();
  QueryUpdate updateQuery;
  updateQuery.addKeyValueStringPair(
    DatabaseStrings::labelProblemTotalQuestions, stringConverter.str()
  );
  updateQuery.addKeyValueStringPair(
    DatabaseStrings::labelProblemFileName, problemName
  );
  Database::get().updateOne(findEntry, updateQuery, true, &commentsOnFailure);
  return this->knownNumberOfAnswersFromHD;
}

void UserCalculator::computePointsEarned(
  const HashedList<std::string>& gradableProblems,
  MapList<std::string, TopicElement, HashFunctions::hashFunction<std::string> >
  * topics,
  std::stringstream& commentsOnFailure
) {
  STACK_TRACE("UserCalculator::computePointsEarned");
  this->pointsEarned = 0;
  this->pointsMaximum = 0;
  if (topics != nullptr) {
    for (int i = 0; i < topics->size(); i ++) {
      (*topics).values[i].totalPointsEarned = 0;
      (*topics).values[i].pointsEarnedInProblemsThatAreImmediateChildren = 0;
      (*topics).values[i].maxPointsInAllChildren = 0;
      (*topics).values[i].flagSubproblemHasNoWeight = false;
    }
  }
  for (int i = 0; i < this->problemData.size(); i ++) {
    const std::string problemName = this->problemData.keys[i];
    if (!gradableProblems.contains(problemName)) {
      continue;
    }
    ProblemData& currentProblem = this->problemData.values[i];
    currentProblem.points = 0;
    currentProblem.totalSubmissions = 0;
    currentProblem.totalCorrectlyAnswered = 0;
    Rational currentWeight;
    currentProblem.flagProblemWeightIsOK =
    currentProblem.adminData.getWeightFromCourse(
      this->courseComputed, currentWeight
    );
    if (!currentProblem.flagProblemWeightIsOK) {
      currentWeight = 0;
    }
    for (int j = 0; j < currentProblem.answers.size(); j ++) {
      if (currentProblem.answers.values[j].numberOfCorrectSubmissions > 0) {
        currentProblem.totalCorrectlyAnswered ++;
      }
      currentProblem.totalSubmissions +=
      currentProblem.answers.values[j].numberOfSubmissions;
    }
    if (currentProblem.flagProblemWeightIsOK) {
      int expectedNumberOfAnswers =
      currentProblem.getExpectedNumberOfAnswers(
        problemName, commentsOnFailure
      );
      if (expectedNumberOfAnswers > 0) {
        currentProblem.points = (
          currentWeight * currentProblem.totalCorrectlyAnswered
        ) /
        expectedNumberOfAnswers;
        this->pointsEarned += currentProblem.points;
      }
    }
    if (topics != nullptr) {
      if (topics->contains(problemName)) {
        TopicElement& currentElement =
        topics->getValueCreateEmpty(problemName);
        this->pointsMaximum += currentWeight;
        for (int j = 0; j < currentElement.parentTopics.size; j ++) {
          (*topics).values[currentElement.parentTopics[j]].totalPointsEarned +=
          currentProblem.points;
          (*topics).values[currentElement.parentTopics[j]].
          maxPointsInAllChildren +=
          currentWeight;
          if (currentWeight == 0) {
            (*topics).values[currentElement.parentTopics[j]].
            flagSubproblemHasNoWeight =
            true;
          }
        }
        if (currentElement.parentTopics.size > 1) {
          (*topics).values[
            currentElement.parentTopics[currentElement.parentTopics.size - 2]
          ].pointsEarnedInProblemsThatAreImmediateChildren +=
          currentProblem.points;
        }
      }
    }
  }
}

class UserScores {
public:
  CalculatorHTML problem;
  std::string currentSection;
  std::string currentCourse;
  List<
    MapList<std::string, Rational, HashFunctions::hashFunction<std::string> >
  > scoresBreakdown;
  List<JSData> userProblemData;
  List<Rational> userScores;
  List<std::string> userInfos;
  List<std::string> userNames;
  List<LargeInteger> numberOfStudentsSolvedEntireTopic;
  List<LargeInteger> numberOfStudentsSolvedPartOfTopic;
  List<LargeInteger> numberOfStudentsSolvedNothingInTopic;
  bool computeScoresAndStats(std::stringstream& comments);
};

bool UserScores::computeScoresAndStats(std::stringstream& comments) {
  STACK_TRACE("UserScores::computeScoresAndStats");
  problem.currentUser.::UserCalculatorData::operator=(global.userDefault);
  this->problem.loadFileNames();
  if (!this->problem.loadAndParseTopicList(comments)) {
    return false;
  }
  if (!this->problem.prepareSectionList(comments)) {
    return false;
  }
  if (!this->problem.loadDatabaseInfo(comments)) {
    comments
    << "<span style ='color:red'>"
    << "Could not load your problem history.</span> <br>";
  }
  problem.currentUser.computePointsEarned(
    problem.currentUser.problemData.keys, &problem.topics.topics, comments
  );
  List<std::string> userLabels;
  int usernameIndex = userLabels.getIndex(DatabaseStrings::labelUsername);
  if (usernameIndex == - 1) {
    comments << "Could not find username column. ";
    return false;
  }
  int problemDataIndex =
  userLabels.getIndex(DatabaseStrings::labelProblemDataJSON);
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
  this->numberOfStudentsSolvedEntireTopic.initializeFillInObject(
    this->problem.topics.topics.size(), 0
  );
  this->numberOfStudentsSolvedPartOfTopic.initializeFillInObject(
    this->problem.topics.topics.size(), 0
  );
  this->numberOfStudentsSolvedNothingInTopic.initializeFillInObject(
    this->problem.topics.topics.size(), 0
  );
  bool ignoreSectionsIdontTeach = true;
  this->currentSection = global.userDefault.sectionComputed;
  this->currentCourse = global.getWebInput(WebAPI::Problem::courseHome);
  if (global.getWebInput("request") == "scoresInCoursePage") {
    this->currentSection =
    StringRoutines::stringTrimWhiteSpace(
      HtmlRoutines::convertURLStringToNormal(
        global.getWebInput("mainInput"), false
      )
    );
  }
  for (int i = 0; i < this->userProblemData.size; i ++) {
    // currentUserRecord.currentUser.courseInfo.rawStringStoredInDB =
    // this->userTablE[i][courseInfoIndex];
    // currentUserRecord.currentUser.AssignCourseInfoString(&comments);
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
    this->userNames.addOnTop(
      this->userProblemData[i][DatabaseStrings::labelUsername].stringValue
    );
    this->userInfos.addOnTop(currentUserRecord.currentUser.sectionInDB);
    this->scoresBreakdown.setSize(this->scoresBreakdown.size + 1);
    currentUserRecord.currentUser.username =
    this->userProblemData[i][DatabaseStrings::labelUsername].stringValue;
    if (
      !currentUserRecord.currentUser.interpretDatabaseProblemDataJSON(
        this->userProblemData[i][DatabaseStrings::labelProblemDataJSON],
        comments
      )
    ) {
      continue;
    }
    currentUserRecord.mergeProblemWeight(
      problem.currentUser.problemWeights,
      currentUserRecord.currentUser.problemData,
      false,
      &comments
    );
    currentUserRecord.currentUser.computePointsEarned(
      problem.problemNamesNoTopics, &problem.topics.topics, comments
    );
    this->scoresBreakdown.lastObject()->clear();
    for (int j = 0; j < problem.topics.topics.size(); j ++) {
      TopicElement& currentTopic = problem.topics.topics.values[j];
      Rational currentPoints = currentTopic.totalPointsEarned;
      Rational maximumPoints = currentTopic.maxPointsInAllChildren;
      this->scoresBreakdown.lastObject()->setKeyValue(
        problem.topics.topics.keys[j], currentPoints
      );
      if (maximumPoints == currentPoints) {
        this->numberOfStudentsSolvedEntireTopic[j] ++;
      } else if (currentPoints > 0) {
        this->numberOfStudentsSolvedPartOfTopic[j] ++;
      } else {
        this->numberOfStudentsSolvedNothingInTopic[j] ++;
      }
    }
    *this->userScores.lastObject() =
    currentUserRecord.currentUser.pointsEarned;
  }
  return true;
}

std::string WebAPIResponse::getScoresInCoursePage() {
  STACK_TRACE("WebAPIResponse::getScoresInCoursePage");
  if (!global.userDefaultHasAdminRights()) {
    return "Only admins are allowed to view student scores.";
  }
  std::stringstream out;
  out.precision(4);
  UserScores scores;
  if (!scores.computeScoresAndStats(out)) {
    return out.str();
  }
  out << "Section: " << scores.currentSection << ". ";
  out << "<script type =\"text/javascript\">\n";
  out
  << "studentScoresInHomePage = new Array("
  << scores.problem.topics.topics.size()
  << ");\n";
  for (int i = 0; i < scores.problem.topics.topics.size(); i ++) {
    TopicElement& currentElement = scores.problem.topics.topics.values[i];
    out << "studentScoresInHomePage[" << i << "] = new Object;\n";
    if (currentElement.flagSubproblemHasNoWeight) {
      out << "studentScoresInHomePage[" << i << "].weightsOK = false;\n";
    } else {
      out << "studentScoresInHomePage[" << i << "].weightsOK = true;\n";
    }
    out << "studentScoresInHomePage[" << i << "].theId =" << "'" << "';\n";
    out
    << "studentScoresInHomePage["
    << i
    << "].numSolvedAll ="
    << scores.numberOfStudentsSolvedEntireTopic[i]
    << ";\n";
    out
    << "studentScoresInHomePage["
    << i
    << "].numSolvedPart ="
    << scores.numberOfStudentsSolvedPartOfTopic[i]
    << ";\n";
    out
    << "studentScoresInHomePage["
    << i
    << "].numSolvedNone ="
    << scores.numberOfStudentsSolvedNothingInTopic[i]
    << ";\n";
  }
  out << "</script>";
  return out.str();
}

std::string WebAPIResponse::toStringUserScores() {
  STACK_TRACE("WebAPIReponse::toStringUserScores");
  if (!global.userDefaultHasAdminRights()) {
    return "only admins are allowed to view scores";
  }
  std::stringstream out;
  out.precision(4);
  UserScores scores;
  if (!scores.computeScoresAndStats(out)) {
    return out.str();
  }
  out
  << "<b>Section: </b>"
  << scores.currentSection
  << "<br><b>Course: </b>"
  << scores.currentCourse
  << "\n<br>\n";
  out
  << "<table class =\"scoreTable\"><tr><th rowspan =\"3\">User</th>"
  << "<th rowspan =\"3\">Section</th><th rowspan =\"3\"> Total score</th>";
  for (int i = 0; i < scores.problem.topics.topics.size(); i ++) {
    TopicElement& currentElement = scores.problem.topics.topics.values[i];
    if (
      currentElement.problemFileName != "" ||
      currentElement.type != TopicElement::types::chapter
    ) {
      continue;
    }
    int numberOfColumns =
    currentElement.totalSubSectionsUnderMeIncludingEmptySubsections;
    out << "<td colspan =\"" << numberOfColumns << "\"";
    if (
      currentElement.totalSubSectionsUnderMe == 0 &&
      currentElement.flagContainsProblemsNotInSubsection
    ) {
      out << " rowspan =\"3\"";
    }
    out << ">" << currentElement.title << "</td>";
  }
  out << "</tr>\n";
  out << "<tr>";
  for (int i = 0; i < scores.problem.topics.topics.size(); i ++) {
    TopicElement& currentElement = scores.problem.topics.topics.values[i];
    if (
      currentElement.problemFileName != "" ||
      currentElement.type != TopicElement::types::section
    ) {
      continue;
    }
    int numberOfColumns =
    currentElement.totalSubSectionsUnderMeIncludingEmptySubsections;
    out << "<td colspan =\"" << numberOfColumns << "\"";
    if (
      currentElement.totalSubSectionsUnderMe == 0 &&
      currentElement.flagContainsProblemsNotInSubsection
    ) {
      out << " rowspan =\"2\"";
    }
    out << ">" << currentElement.title << "</td>";
  }
  out << "</tr>\n";
  out << "<tr>";
  for (int i = 0; i < scores.problem.topics.topics.size(); i ++) {
    TopicElement& currentElement = scores.problem.topics.topics.values[i];
    if (
      currentElement.problemFileName == "" &&
      currentElement.type != TopicElement::types::problem &&
      currentElement.type != TopicElement::types::topic &&
      currentElement.type != TopicElement::types::texHeader
    ) {
      if ((
          currentElement.flagContainsProblemsNotInSubsection &&
          currentElement.totalSubSectionsUnderMeIncludingEmptySubsections > 1
        ) ||
        currentElement.immediateChildren.size == 0
      ) {
        out << "<td></td>";
      }
      continue;
    }
    if (
      currentElement.problemFileName != "" ||
      currentElement.type != TopicElement::types::topic
    ) {
      continue;
    }
    out << "<td>" << currentElement.title << "</td>";
  }
  out << "</tr>\n";
  out << "<tr><td><b>maximum score</b></td>" << "<td>-</td>";
  out
  << "<td>"
  << scores.problem.currentUser.pointsMaximum.getDoubleValue()
  << "</td>";
  for (int j = 0; j < scores.problem.topics.topics.size(); j ++) {
    TopicElement& currentElement = scores.problem.topics.topics.values[j];
    if (currentElement.problemFileName != "") {
      continue;
    }
    if (
      currentElement.type != TopicElement::types::topic &&
      !currentElement.flagContainsProblemsNotInSubsection
    ) {
      continue;
    }
    out << "<td>" << currentElement.maxPointsInAllChildren << "</td>";
  }
  out << "</tr>";
  for (int i = 0; i < scores.userInfos.size; i ++) {
    out
    << "<tr><td>"
    << scores.userNames[i]
    << "</td>"
    << "<td>"
    << scores.userInfos[i]
    << "</td>"
    << "<td>"
    << scores.userScores[i].getDoubleValue()
    << "</td>";
    for (int j = 0; j < scores.problem.topics.topics.size(); j ++) {
      TopicElement& currentElement = scores.problem.topics.topics.values[j];
      if (currentElement.problemFileName != "") {
        continue;
      }
      if (
        currentElement.type != TopicElement::types::topic &&
        !currentElement.flagContainsProblemsNotInSubsection
      ) {
        continue;
      }
      if (
        scores.scoresBreakdown[i].contains(
          scores.problem.topics.topics.keys[j]
        )
      ) {
        out
        << "<td>"
        << scores.scoresBreakdown[i].values[j].getDoubleValue()
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
