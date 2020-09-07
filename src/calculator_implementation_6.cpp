// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "calculator_inner_functions.h"
#include "calculator_inner_typed_functions.h"
#include "calculator_html_functions.h"
#include "web_api.h"
#include "math_general_implementation.h"
#include "calculator_html_interpretation.h"
#include "math_extra_drawing_variables.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "crypto.h"
#include "abstract_syntax_notation_one_decoder.h"
#include "math_extra_elliptic_curves_implementation.h"
#include "transport_layer_security.h"
#include "string_constants.h"
#include <iomanip>

CalculatorHTML::Test::Test() {
  this->filesToInterpret = 0;
  this->randomSeed = 0;
  this->firstFileIndex = 0;
  this->flagCorrectedTotalFiles = false;
}

bool CalculatorHTML::Test::computeTotalFiles() {
  MacroRegisterFunctionWithName("CalculatorHTML::Test::computeTotalFiles");
  std::stringstream commentsOnFailure;
  if (!FileOperations::getFolderFileNamesVirtual(
    "problems/default/",
    this->fileNamesAll,
    &this->fileExtensionsAll,
    false,
    false,
    &commentsOnFailure
  )) {
    commentsOnFailure << "Failed to fetch problem files. ";
    this->errorComments += commentsOnFailure.str();
    return false;
  }
  this->fileNames.setSize(0);
  this->fileNames.setExpectedSize(this->fileNames.size);
  for (int i = 0; i < this->fileNamesAll.size; i ++) {
    if (this->fileExtensionsAll[i] == ".html") {
      this->fileNames.addOnTop(this->fileNamesAll[i]);
    }
  }
  return true;
}

std::string CalculatorHTML::toStringLinkFromProblem(
  const std::string& theFileName,
  bool practiceMode,
  int randomSeed
) {
  JSData request;
  request[WebAPI::problem::fileName] = theFileName;
  request[WebAPI::frontend::problemFileName] = theFileName;
  std::stringstream randomSeedStream;
  randomSeedStream << randomSeed;
  request[WebAPI::problem::randomSeed] = randomSeedStream.str();
  request[WebAPI::problem::fileName] = theFileName;
  request[WebAPI::frontend::currentPage] = WebAPI::frontend::problemPage;
  if (practiceMode) {
    request[WebAPI::frontend::exerciseType] = WebAPI::frontend::exercise  ;
  } else  {
    request[WebAPI::frontend::exerciseType] = WebAPI::frontend::scoredQuiz;
  }
  std::stringstream out;
  out << "<a href='" << global.displayNameExecutableAppNoCache
  << "#"
  << request.toString()
  << "'>"
  << theFileName
  << "</a>";
  return  out.str();
}

std::string CalculatorHTML::Test::OneProblemTest::toStringHTMLTableRow(int rowIndex) {
  MacroRegisterFunctionWithName("CalculatorHTML::Test::OneProblemTest::toStringHTMLTableRow");
  std::stringstream out;
  out << "<tr>";
  out << "<td style = 'min-width:25px'>" << rowIndex << ". </td>";
  out << "<td style = 'min-width:200px'>";
  out << CalculatorHTML::toStringLinkFromProblem(this->fileName)
  << "</td>";
  if (this->errorLoad != "") {
    out << "<td style = 'min-width:300px'><b>Couldn't load.</b> "
    << this->errorLoad << "</td>";
    out << "</tr>";
    return out.str();
  }
  out << "<td style = 'min-width:60px'><b style = 'color:green'>Success</b></td>";
  if (!this->flagInterpretationSuccess) {
    out << "<td style = 'min-width:400px'><b style ='color:red'>Failure.</b> "
    << "Comments: " << this->errorInterpretation;
    out << "</td></tr>";
    return out.str();
  }
  out << "<td style = 'min-width:60px'><b style = 'color:green'>Success</b></td>";
  if (this->answers.size == 0) {
    out << "<td><b style = 'color:red'>No built-in answer.</b></td>";
  } else {
    if (this->flagAllBuiltInAnswersOK) {
      out << "<td style = 'min-width:60px'><b style = 'color:green'>Success</b></td>";
    } else {
      out << "<td style = 'min-width:60px'><b style = 'color:red'>Failure.</b></td>";
      out << "<td style = 'min-width:400px'>";
      for (int i = 0; i < this->answers.size; i ++) {
        out << this->answers[i].builtInAnswer;
      }
      if (this->errorAnswers != "") {
        out << this->errorAnswers;
      }
      out << "</td>"
      << "</tr>";
      return out.str();
    }
  }
  return out.str();
}

std::string CalculatorHTML::Test::toHTMLDebug() {
  std::stringstream out;
  out << "File names all: " << this->fileNamesAll.toStringCommaDelimited() << "<br>";
  out << "Extensions all: " << this->fileExtensionsAll.toStringCommaDelimited() << "<br>";
  out << "File names extracted: " << this->fileNames.toStringCommaDelimited();
  return out.str();
}

std::string CalculatorHTML::Test::toHTMLBuiltIn() {
  std::stringstream tableBad, tableGood, out;
  std::stringstream tableHeader;
  tableHeader
  << "<table>"
  << "<tr>"
  << "<th>Index</th>"
  << "<th>Problem</th>"
  << "<th>load</th>"
  << "<th>Interpret</th>"
  << "<th>Answer OK?</th>"
  << "</tr>";
  tableBad << tableHeader.str();
  tableGood << tableHeader.str();

  int numberBad = 0;
  for (int i = 0; i < this->results.size; i ++) {
    std::string nextRow = this->results[i].toStringHTMLTableRow(
      this->firstFileIndex + i
    );
    bool isGood = true;
    if (
      !this->results[i].flagAllBuiltInAnswersOK ||
      !this->results[i].flagInterpretationSuccess
    ) {
      numberBad ++;
      isGood = false;
    }
    if (!isGood) {
      tableBad << nextRow;
    } else  {
      tableGood << nextRow;
    }
  }
  tableBad << "</table>";
  tableGood << "</table>";
  if (this->errorComments != "") {
    out << this->errorComments << "<br>";
  }
  if (numberBad > 0) {
    out << "<b style = 'color:red'>"
    << numberBad << " bad problems</b>. ";
  }
  out << "Total problems: " << this->results.size << "<br>";
  if (numberBad > 0) {
    out << tableBad.str() << "<br>";
  }
  out << tableGood.str();
  return out.str();
}

CalculatorHTML::Test::OneProblemTest::OneProblemTest() {
  this->randomSeed = 0;
  this->flagInterpretationSuccess = false;
  this->flagAllBuiltInAnswersOK = false;
  this->flagSuccess = false;
}

CalculatorHTML::Test::OneProblemTest::OneAnswer::OneAnswer() {
  this->flagBuiltInWorks = false;
  this->flagBuiltInGenerated = false;
}

bool CalculatorHTML::Test::OneProblemTest::run() {
  CalculatorHTML theProblem;
  std::stringstream commentsOnFailure;
  std::stringstream randomSeedStringStarting;
  randomSeedStringStarting << this->randomSeed;
  theProblem.fileName = this->fileName;
  this->flagSuccess = false;
  StateMaintainer<MapList<std::string, std::string, MathRoutines::hashString> >
  maintainArguments(global.webArguments);
  if (!theProblem.loadMe(false, randomSeedStringStarting.str(), &commentsOnFailure)) {
    this->errorLoad = commentsOnFailure.str();
    return this->flagSuccess;
  }
  this->flagInterpretationSuccess = theProblem.interpretHtml(&commentsOnFailure);

  if (!this->flagInterpretationSuccess) {
    this->errorInterpretation = commentsOnFailure.str();
    return this->flagSuccess;
  }
  std::stringstream randomSeedStream;
  randomSeedStream << theProblem.theProblemData.randomSeed;
  this->answers.setSize(theProblem.theProblemData.theAnswers.size());
  this->flagAllBuiltInAnswersOK = true;
  global.setWebInput(WebAPI::problem::fileName, theProblem.fileName);
  global.setWebInput(WebAPI::problem::randomSeed, randomSeedStream.str());
  this->flagSuccess = true;
  for (int j = 0; j < this->answers.size; j ++) {
    CalculatorHTML::Test::OneProblemTest::OneAnswer& current = this->answers[j];
    current.answerId = theProblem.theProblemData.theAnswers.theValues[j].answerId;
    current.answerIdWebAPI = WebAPI::problem::calculatorAnswerPrefix + current.answerId;
    global.setWebInput(current.answerIdWebAPI, "1");
    current.builtInAnswerAPICall = WebAPIResponse::getAnswerOnGiveUp(
      randomSeedStream.str(),
      &current.builtInAnswer,
      &current.flagBuiltInGenerated,
      false
    );
    global.webArguments.removeKey(current.answerIdWebAPI);
    if (!current.flagBuiltInGenerated) {
      commentsOnFailure << "Failed to generate answer: " << current.answerId << "<br>";
      commentsOnFailure << current.builtInAnswerAPICall[WebAPI::result::resultHtml].theString;
      this->flagAllBuiltInAnswersOK = false;
      this->flagSuccess = false;
      break;
    }
    current.builtInAnswerEncoded = HtmlRoutines::convertStringToURLString(
      current.builtInAnswer, false
    );
    global.setWebInput(current.answerIdWebAPI, current.builtInAnswerEncoded);
    current.builtInAnswerReply = WebAPIResponse::submitAnswersJSON(
      randomSeedStream.str(), &current.flagBuiltInWorks, false
    );
    global.webArguments.removeKey(current.answerIdWebAPI);
    if (!current.flagBuiltInWorks) {
      this->flagAllBuiltInAnswersOK = false;
      commentsOnFailure << "<br>Built-in answer of index: "
      << j << " does not work:<br>"
      << current.builtInAnswer
      << "<hr>"
      << current.builtInAnswerReply[WebAPI::result::resultHtml].theString;
      this->flagSuccess = false;
      break;
    }
  }
  this->errorAnswers = commentsOnFailure.str();
  return this->flagSuccess;
}

std::string CalculatorHTML::Test::toStringSummary() {
  std::stringstream out;
  out << "First file index: "
  << this->firstFileIndex << ", inputFilesToInterpret: "
  << this->filesToInterpret << " inputRandomSeed: "
  << this->randomSeed
  << ". ";
  int firstFail = 0;
  for (; firstFail < this->results.size; firstFail ++) {
    if (!this->results[firstFail].flagSuccess) {
      break;
    }
  }
  out << "First failed index: " << firstFail << ".\n";
  return out.str();
}

bool CalculatorHTML::Test::BuiltInMultiple(
  int inputFirstFileIndex,
  int inputFilesToInterpret,
  int inputRandomSeed,
  int numberOfRepetitions,
  std::stringstream* comments
) {
  List<int> randomSeeds;
  randomSeeds.setSize(numberOfRepetitions);
  ProgressReport theReport;
  for (int i = 0; i < numberOfRepetitions; i ++) {
    randomSeeds[i] = inputRandomSeed + i;
    std::stringstream reportStream;
    reportStream << "Testing problems, round "
    << i + 1 << " out of " << numberOfRepetitions
    << ", starting random seed: " << randomSeeds[i] << ". ";
    theReport.report(reportStream.str());
    CalculatorHTML::Test tester;
    if (!tester.builtIn(inputFirstFileIndex, inputFilesToInterpret, randomSeeds[i])) {
      if (comments != nullptr) {
        *comments << "Failed run " << i + 1 << " out of " << numberOfRepetitions << ". "
        << tester.toHTMLBuiltIn();
        *comments << "Failed run " << i + 1 << " out of " << numberOfRepetitions << ". "
        << tester.toStringSummary();
      }
      return false;
    }
    if (i == numberOfRepetitions - 1 && comments != nullptr) {
      *comments << numberOfRepetitions
      << " consecutive successfull tests of all involved problems. "
      << "The tests were carried out with starting random seeds: "
      << randomSeeds.toStringCommaDelimited()
      << "<br>"
      << tester.toHTMLBuiltIn();
    }
  }
  return true;
}

bool CalculatorHTML::Test::builtIn(
  int inputFirstFileIndex,
  int inputFilesToInterpret,
  int inputRandomSeed
) {
  MacroRegisterFunctionWithName("CalculatorHTML::Test::builtIn");
  this->firstFileIndex = inputFirstFileIndex;
  this->filesToInterpret = inputFilesToInterpret;
  this->randomSeed = inputRandomSeed;

  ProgressReport theReport;
  if (!this->computeTotalFiles()) {
    return false;
  }
  if (this->firstFileIndex < 0) {
    this->firstFileIndex = 0;
  }
  if (
    this->filesToInterpret <= 0 ||
    this->filesToInterpret > this->fileNames.size
  ) {
    this->filesToInterpret = this->fileNames.size;
  }
  int lastIndex = this->firstFileIndex + this->filesToInterpret;
  if (lastIndex > this->fileNames.size) {
    lastIndex = this->fileNames.size;
    this->filesToInterpret = lastIndex - this->firstFileIndex;
    this->flagCorrectedTotalFiles = true;
  }
  bool result = true;
  this->results.setExpectedSize(this->filesToInterpret);
  this->results.setSize(0);
  std::stringstream commentsOnFailure;
  int badSoFar = 0;
  for (int i = this->firstFileIndex; i < lastIndex; i ++) {
    this->results.setSize(this->results.size + 1);
    OneProblemTest& currentTest = *this->results.lastObject();
    currentTest.fileName = "problems/" + this->fileNames[i];
    currentTest.randomSeed = this->randomSeed;
    std::stringstream reportStream;
    if (badSoFar > 0) {
      reportStream << "<b style = 'color:red'>Found " << badSoFar << " bad files so far. </b><br>";
    }
    reportStream << "File: "
    << i
    << " out of "
    << this->fileNames.size
    << ". ";
    reportStream
    << currentTest.fileName
    << ". Random seed: "
    << this->randomSeed << ".";

    if (global.flagRunningConsoleTest) {
      global << reportStream.str() << Logger::endL;
    }
    theReport.report(reportStream.str());
    if (!currentTest.run()) {
      result = false;
    }
    if (!currentTest.flagSuccess) {
      badSoFar ++;
      if (global.flagRunningConsoleTest) {
        global << Logger::red << "Failure @ index: " << i << ". "
        << "Elapsed ms: " << global.getElapsedMilliseconds() << ". "
        << Logger::endL;        
      }
    } else {
      if (global.flagRunningConsoleTest) {
        global << Logger::green << "Success @ index: " << i << ". "
        << "Elapsed ms: " << global.getElapsedMilliseconds() << ". "
        << Logger::endL;
      }
    }
  }
  this->errorComments += commentsOnFailure.str();
  return result;
}

bool TopicElementParser::Test::all() {
  std::stringstream comments;
  TopicElementParser::Test::defaultTopicListsOKCrashOnFailure();
  TopicElementParser::Test::defaultPdfsOKCrashOnFailure();
  return true;
}

bool TopicElementParser::Test::defaultTopicListsOKCrashOnFailure() {
  TopicElementParser::Test tester;
  if (!tester.defaultTopicListsOK()) {
    global.fatal << "Topic list tests failed. " << tester.comments.str() << global.fatal;
  }
  return false;
}

bool TopicElementParser::Test::defaultPdfsOKCrashOnFailure() {
  TopicElementParser::Test tester;
  if (!tester.defaultPdfsOK()) {
    global.fatal << "Default pdfs are broken. " << tester.comments.str() << global.fatal;
  }
  return false;
}

bool TopicElementParser::Test::defaultPdfsOK() {
 MacroRegisterFunctionWithName("TopicElementParser::Test::defaultPdfsOK");
  CourseList courses;
  if (!courses.load()) {
    this->comments << "Failed to load the list of available courses. "
    << courses.errorMessage;
    return false;
  }
  for (int i = 0; i < courses.theCourses.size; i ++) {
    CalculatorHTML owner;
    owner.topicListFileName = courses.theCourses[i].courseTopicsWithFolder();
    if (!owner.loadAndParseTopicList(this->comments)) {
      this->comments << "Failed to load course "
      << owner.topicListFileName << ". ";
      return false;
    }
    if (!owner.topics.checkTopicPdfs(&this->comments)) {
      this->comments << "Topic pdf check failed. ";
      return false;
    }
  }
  return true;
}

bool TopicElementParser::Test::defaultTopicListsOK() {
  MacroRegisterFunctionWithName("TopicElementParser::Test::defaultTopicListsOK");
  CourseList courses;
  if (!courses.load()) {
    this->comments << "Failed to load the list of available courses. "
    << courses.errorMessage;
    return false;
  }
  for (int i = 0; i < courses.theCourses.size; i ++) {
    CalculatorHTML owner;
    owner.topicListFileName = courses.theCourses[i].courseTopicsWithFolder();
    if (!owner.loadAndParseTopicList(this->comments)) {
      this->comments << "Failed to load course "
      << owner.topicListFileName << ". ";
      return false;
    }
    if (!owner.topics.checkNoErrors(&this->comments)) {
      this->comments << "Errors in topic list: "
      << owner.topicListFileName << ". ";
      return false;
    }
    if (!owner.topics.checkProblemsOpen(&this->comments)) {
      this->comments << "Not all problems open correctly in topic list: "
      << owner.topicListFileName << ". ";
      return false;
    }
    this->comments << "Topic list: "
    << owner.topicListFileName << " with "
    << owner.topics.theTopics.size()
    << " topics <b style = 'color:green'>looks ok</b>.<br>";
  }
  return true;
}

bool CalculatorHTML::Test::all() {
  CalculatorHTML::Test::builtInCrashOnFailure();
  return true;
}

bool CalculatorHTML::Test::builtInCrashOnFailure() {
  std::stringstream comments;
  if (!CalculatorHTML::Test::BuiltInMultiple(0, 0, 0, 3, &comments)) {
    global.fatal << "Built-in problem tests failed. "
    << comments.str() << global.fatal;
  }
  return true;
}

bool CalculatorFunctions::innerTestTopicListProblems(
  Calculator& calculator,
  const Expression& input,
  Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestTopicListProblems");
  if (!global.userDefaultHasAdminRights()) {
    return calculator << "Topic list tests available to logged-in admins only. ";
  }
  (void) input;
  TopicElementParser::Test tester;
  tester.defaultTopicListsOK();
  return output.assignValue(tester.comments, calculator);
}

bool CalculatorFunctions::innerTestProblemInterpretation(
  Calculator& calculator,
  const Expression& input,
  Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestProblemInterpretation");
  if (!global.userDefaultHasAdminRights()) {
    return calculator << "Automated tests available to logged-in admins only. ";
  }
  if (input.size() != 4) {
    return calculator
    << "I expected two arguments: "
    << "1) index of first problem to test, where "
    << "0 = start at beginning, 1 = start at second problem, etc.; "
    << "2) number of problems to test (0 or less = test all); "
    << "3) starting random seed, set to 0 if you don't know what this is. "
    ;
  }
  if (global.theResponse.monitoringAllowed()) {
    global.theResponse.initiate("Triggered by innerTestProblemInterpretation.");
  }
  int desiredNumberOfTests = 0;
  int firstFileIndex = 0;
  int randomSeed = 0;
  input[1].isSmallInteger(&firstFileIndex);
  input[2].isSmallInteger(&desiredNumberOfTests);
  input[3].isSmallInteger(&randomSeed);
  std::stringstream comments;
  CalculatorHTML::Test::BuiltInMultiple(firstFileIndex, desiredNumberOfTests, randomSeed, 3, &comments);
  return output.assignValue(comments.str(), calculator);
}

bool CalculatorFunctions::innerIntervalClosedFromSequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntervalClosedFromSequence");
  if (input.size() != 2) {
    return false;
  }
  if (!input[1].isSequenceNElements(2)) {
    return false;
  }
  output = input[1];
  return output.setChildAtomValue(0, calculator.opIntervalClosed());
}

bool CalculatorFunctions::innerIntervalOpenFromSequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntervalOpenFromSequence");
  if (input.size() != 2) {
    return false;
  }
  if (!input[1].isSequenceNElements(2)) {
    return false;
  }
  output = input[1];
  return output.setChildAtomValue(0, calculator.opIntervalOpen());
}

bool CalculatorFunctions::innerIntervalRightClosedFromSequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntervalRightClosedFromSequence");
  if (input.size() != 2) {
    return false;
  }
  if (!input[1].isSequenceNElements()) {
    return false;
  }
  output = input[1];
  return output.setChildAtomValue(0, calculator.opIntervalRightClosed());
}

bool CalculatorFunctions::innerIntervalLeftClosedFromSequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntervalLeftClosedFromSequence");
  if (input.size() != 2) {
    return false;
  }
  if (!input[1].isSequenceNElements()) {
    return false;
  }
  output = input[1];
  return output.setChildAtomValue(0, calculator.opIntervalLeftClosed());
}

bool CalculatorFunctions::innerGetFirstSummandContaining(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGetFirstSummandContaining");
  if (!input.startsWithGivenOperation("GetFirstSummandContaining", 3)) {
    return false;
  }
  List<Expression> theSummands;
  calculator.collectOpands(input[1], calculator.opPlus(), theSummands);
  for (int i = 0; i < theSummands.size; i ++) {
    if (theSummands[i].containsAsSubExpressionNoBuiltInTypes(input[2])) {
      output = theSummands[i];
      return true;
    }
  }
  return false;
}

bool CalculatorFunctions::innerGetSummand(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGetSummand");
  if (!input.startsWithGivenOperation("GetSummand", 3)) {
    return false;
  }
  const Expression& theExpression = input[1];
  if (theExpression.startsWith(calculator.opPlus(), 3)) {
    int summandIndex = - 1;
    if (!input[2].isSmallInteger(&summandIndex)) {
      return false;
    }
    if (summandIndex < 0) {
      return false;
    }
    List<Expression> theSummands;
    List<Expression> theSums;
    calculator.collectOpands(theExpression, calculator.opPlus(), theSummands);
    for (int i = 0; i < theSummands.size; i ++) {
      if (theSummands[i].containsAsSubExpressionNoBuiltInTypes(calculator.opSum())) {
        theSums.addOnTop(theSummands[i]);
        theSummands.removeIndexShiftDown(i);
        i --;
      }
    }
    if (theSums.size > 1) {
      return false;
    }
    if (summandIndex < theSummands.size) {
      output = theSummands[summandIndex];
      return true;
    }
    if (theSums.size == 0) {
      return false;
    }
    output.reset(calculator);
    output.addChildAtomOnTop("GetSummand");
    output.addChildOnTop(theSums[0]);
    Expression shiftE;
    shiftE.assignValue(theSummands.size, calculator);
    return output.addChildOnTop(input[2] - shiftE);
  }
  List<Expression> theMultiplicands;
  theExpression.getMultiplicandsRecursive(theMultiplicands);
  Expression theSum = *theMultiplicands.lastObject();
  theMultiplicands.removeLastObject();
  Expression theCoeff;
  if (theMultiplicands.size > 0) {
    theCoeff.makeProduct(calculator, theMultiplicands);
  } else {
    theCoeff.assignValue(1, calculator);
  }
  if (!theSum.startsWith(calculator.opSum(), 3)) {
    return false;
  }
  const Expression& theLimits = theSum[1];
  if (!theLimits.startsWith(calculator.opLimitBoundary(), 3)) {
    return false;
  }
  const Expression& theBottomBoundary = theLimits[1];
  if (!theBottomBoundary.startsWith(calculator.opDefine(), 3)) {
    return false;
  }
  Expression theSub = theBottomBoundary;
  Expression //oneE,
  valueToSubWith;
  //oneE.assignValue(1, calculator);
  valueToSubWith = theBottomBoundary[2] + input[2];
  theSub.setChild(2, valueToSubWith);
  Expression theCommandSequence(calculator);
  theCommandSequence.addChildAtomOnTop(calculator.opEndStatement());
  theCommandSequence.addChildOnTop(theSub);
  theCommandSequence.addChildOnTop(theCoeff * theSum[2]);
  return output.makeXOX(calculator, calculator.opUnderscore(), theCommandSequence, calculator.expressionTwo());
}

bool CalculatorFunctions::innerPlotVectorField(Calculator& calculator, const Expression& input, Expression& output) {
  return CalculatorFunctions::innerPlotDirectionOrVectorField(calculator, input, output, false);
}

bool CalculatorFunctions::innerPlotDirectionField(Calculator& calculator, const Expression& input, Expression& output) {
  return CalculatorFunctions::innerPlotDirectionOrVectorField(calculator, input, output, true);
}

bool CalculatorFunctions::innerPlotDirectionOrVectorField(
  Calculator& calculator, const Expression& input, Expression& output, bool vectorsAreNormalized
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotDirectionOrVectorField");
  (void) vectorsAreNormalized;
  if (input.size() < 5) {
    return output.makeError(
      "Vector fields take at least four arguments: the vector field, "
      "the low left corner, the upper right, and the "
      "number of segments in each direction. ",
      calculator
    );
  }
  if (input.hasBoundVariables()) {
    return false;
  }
  Plot thePlot;
  thePlot.dimension = 2;
  PlotObject thePlotObj;
  thePlotObj.leftPtE = input[2];
  thePlotObj.rightPtE = input[3];
  if (input.size() >= 7) {
    if (!input[6].isOfType<std::string>(&thePlotObj.colorJS)) {
      thePlotObj.colorJS = input[6].toString();
    }
  } else {
    thePlotObj.colorJS = "blue";
  }
  thePlotObj.colorRGB = static_cast<int>(HtmlRoutines::redGreenBlue(0, 0, 255));
  DrawingVariables::getColorIntFromColorString(thePlotObj.colorJS, thePlotObj.colorRGB);
  thePlotObj.lineWidth = 1;
  if (input.size() >= 8) {
    input[7].evaluatesToDouble(&thePlotObj.lineWidth);
  }
  Vector<double> lowLeft, upRight;
  if (!calculator.getVectorDoubles(input[2], lowLeft, 2)) {
    return calculator << "Failed to low left corner from: " << input[2].toString();
  }
  if (!calculator.getVectorDoubles(input[3], upRight, 2)) {
    return calculator << "Failed to up right corner from: " << input[3].toString();
  }
  thePlotObj.yHigh = upRight[1];
  thePlotObj.yLow = lowLeft[1];
  thePlotObj.xHigh = upRight[0];
  thePlotObj.xLow = lowLeft[0];
  List<std::string> lowLeftStrings, upRightStrings;
  lowLeft.toListStringsBasicType(lowLeftStrings);
  upRight.toListStringsBasicType(upRightStrings);
  thePlotObj.theVarRangesJS.setSize(2);
  thePlotObj.theVarRangesJS[0].setSize(2);
  thePlotObj.theVarRangesJS[1].setSize(2);
  thePlotObj.theVarRangesJS[0][0] = lowLeftStrings[0];
  thePlotObj.theVarRangesJS[0][1] = upRightStrings[0];
  thePlotObj.theVarRangesJS[1][0] = lowLeftStrings[1];
  thePlotObj.theVarRangesJS[1][1] = upRightStrings[1];
  thePlotObj.manifoldImmersion = input[1];
  Expression jsConverterE;
  if (input.size() >= 6) {
    if (CalculatorFunctions::functionMakeJavascriptExpression(calculator, input[5], jsConverterE)) {
      thePlotObj.defaultLengthJS = jsConverterE.toString();
    } else {
      return calculator << "Failed to extract javascript from " << input[5].toString();
    }
  }
  if (CalculatorFunctions::functionMakeJavascriptExpression(calculator, thePlotObj.manifoldImmersion, jsConverterE)) {
    thePlotObj.manifoldImmersionJS = jsConverterE.toString();
    thePlotObj.manifoldImmersion.hasInputBoxVariables(&thePlot.boxesThatUpdateMe);
  } else {
    return calculator << "Failed to extract javascript from " << input[1].toString();
  }
  thePlotObj.manifoldImmersion.getFreeVariables(thePlotObj.variablesInPlay, true);
  Expression xE, yE;
  xE.makeAtom("x", calculator);
  yE.makeAtom("y", calculator);
  if (thePlotObj.variablesInPlay.size == 0) {
    thePlotObj.variablesInPlay.addOnTop(xE);
  }
  if (thePlotObj.variablesInPlay.size == 1) {
    if (thePlotObj.variablesInPlay.contains(xE)) {
      thePlotObj.variablesInPlay.addOnTop(yE);
    } else {
      thePlotObj.variablesInPlay.addOnTop(xE);
    }
  }
  thePlotObj.variablesInPlay.quickSortAscending();
  thePlotObj.variablesInPlayJS.setSize(thePlotObj.variablesInPlay.size);
  for (int i = 0; i < thePlotObj.variablesInPlay.size; i ++) {
    thePlotObj.variablesInPlayJS[i] = thePlotObj.variablesInPlay[i].toString();
  }
  thePlotObj.thePlotType = "plotDirectionField";
  if (
    !input[4].isSequenceNElements(2) &&
    !input[4].startsWith(calculator.opIntervalOpen(), 3)
  ) {
    return calculator
    << "<hr>Could not extract a list of elements for the "
    << "number of segments from: " << input[4].toString();
  }
  thePlotObj.numSegmenTsJS.setSize(2);
  for (int i = 0; i < 2; i ++) {
    if (!CalculatorFunctions::functionMakeJavascriptExpression(
      calculator, input[4][i + 1], jsConverterE
    )) {
      return calculator << "Failed to convert "
      << input[4][i + 1].toString() << " to javascript. ";
    }
    thePlotObj.numSegmenTsJS[i] = jsConverterE.toString();
  }
  thePlot.thePlots.addOnTop(thePlotObj);
  return output.assignValue(thePlot, calculator);
}

bool CalculatorFunctions::innerJWTVerifyAgainstRSA256(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerJWTverifyAgainstRSA256");
  if (input.size() != 4) {
    return calculator << "The JWT verify command expects 3 arguments: "
    << "string with the token in the usual format (\"a.b.c\"), "
    << "the modulus of the key and the exponent of the key. ";
  }
  std::string theTokenString;
  if (!input[1].isOfType(&theTokenString)) {
    return calculator << "The first argument of " << input.toString() << " is not a string. ";
  }
  std::stringstream out;
  std::string theModBase64, theExpBase64;
  if (!input[2].isOfType(&theModBase64) || !input[3].isOfType(&theExpBase64)) {
    return calculator << "Failed to convert the arguments "
    << input[2].toString()
    << " and " << input[3].toString()
    << " to base64 strings";
  }
  JSONWebToken theToken;
  LargeIntegerUnsigned theMod, theExp;
  if (!theToken.assignString(theTokenString, &out)) {
    return output.assignValue(out.str(), calculator);
  }
  out << "Sucesfully extracted JWT token. <br>"
  << theToken.toString()
  << "<br>";
  if (
    !Crypto::convertBase64ToLargeUnsigned(theModBase64, theMod, &out) ||
    !Crypto::convertBase64ToLargeUnsigned(theExpBase64, theExp, &out)
  ) {
    return output.assignValue(out.str(), calculator);
  }
  out << "<br>Successfully extracted modulus and exponent";
  theToken.verifyRSA256(theMod, theExp, &out, &out);
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerJWTVerifyAgainstKnownKeys(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerJWTverifyAgainstKnownKeys");
  if (!global.userDefaultHasAdminRights()) {
    return calculator << "This function is only available to logged-in admins. ";
  }
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  std::stringstream out;
  Crypto::verifyJWTagainstKnownKeys(inputString, &out, &out);
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerHexToString(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerHexToString");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    inputString = input[1].toString();
  }
  std::string result;
  std::stringstream commentsOnFailure;
  if (!Crypto::convertHexToString(inputString, result, &commentsOnFailure)) {
    return calculator << "Failed to interpret your input as string. " << commentsOnFailure.str();
  }
  return output.assignValue(result, calculator);
}

bool CalculatorFunctions::innerIntegerToHex(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegerToHex");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger theLI;
  if (!input[1].isInteger(&theLI)) {
    return false;
  }
  if (theLI < 0) {
    return calculator << "I only convert positive integers to hex strings. ";
  }
  std::string result;
  if (!Crypto::convertLargeUnsignedToHexSignificantDigitsFirst(theLI.value, 0, result)) {
    return calculator << "Failed to convert " << theLI
    << " to a hex string. ";
  }
  return output.assignValue(result, calculator);
}

bool CalculatorFunctions::innerHexToInteger(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerHexToInteger");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  LargeIntegerUnsigned result;
  int notUsedNumberOfLeadingZeroes = 0;
  if (!Crypto::convertHexToInteger(inputString, result, notUsedNumberOfLeadingZeroes)) {
    return calculator << "Failed to interpret " << inputString
    << " as a hex string ";
  }
  Rational resultRat = result;
  return output.assignValue(resultRat, calculator);
}

bool CalculatorFunctions::innerTestJSON(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestJSON");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    inputString = input[1].toString();
  }
  JSData theData;
  std::stringstream out;
  if (!theData.readstring(inputString, &out)) {
    return output.assignValue(out.str(), calculator);
  }
  out << "Input:<br>" << inputString << "<hr>Output: <br>"
  << theData.toString(nullptr);
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerBase64ToHex(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerBase64ToHex");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    inputString = input[1].toString();
  }
  std::string result, bitStream;
  if (!Crypto::convertBase64ToString(inputString, bitStream, &calculator.comments, &calculator.comments)) {
    return false;
  }
  Crypto::convertStringToHex(bitStream, result, 0, false);
  return output.assignValue(result, calculator);
}

bool CalculatorFunctions::innerGenerateRandomPrime(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGenerateRandomPrime");
  if (input.size() != 2) {
    return false;
  }
  int numberOfBytes = 0;
  if (!input[1].isSmallInteger(&numberOfBytes)) {
    return false;
  }
  int maxNumberOfBytes = 128;
  int minNumberOfBytes = 1;
  if (numberOfBytes > maxNumberOfBytes || numberOfBytes < minNumberOfBytes) {
    return calculator << "Max number of bytes: "
    << maxNumberOfBytes << ", min number of bytes: "
    << minNumberOfBytes << ", you requested: " << numberOfBytes << ". ";
  }
  LargeIntegerUnsigned result;
  Crypto::Random::getRandomLargePrime(result, numberOfBytes);
  return output.assignValue(result, calculator);
}

bool CalculatorFunctions::innerTestRSASign(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestRSASign");
  if (input.size() != 4) {
    return calculator << "RSA signature takes as input 3 arguments: message and two primes. ";
  }
  std::stringstream out, errorStream;
  std::string messageString;
  if (!input[1].isOfType(&messageString)) {
    return false;
  }
  PrivateKeyRSA theKey;
  if (
    !input[2].isIntegerNonNegative(&theKey.primeOne) ||
    !input[3].isIntegerNonNegative(&theKey.primeTwo)
  ) {
    return calculator << "Failed to extract positive integers from the second and third argument. ";
  }
  if (
    !theKey.computeFromTwoPrimes(theKey.primeOne, theKey.primeTwo, true, &errorStream)
  ) {
    return calculator << "Inputs do not appear to be prime. " << errorStream.str();
  }
  List<unsigned char> message, paddedMessage, signature;
  message = messageString;
  theKey.hashAndPadPKCS1(message, SignatureAlgorithmSpecification::HashAlgorithm::sha256, paddedMessage);
  out << "Temporary private key:<br>" << theKey.toString() << "<br>";
  theKey.signBytesPadPKCS1(message, SignatureAlgorithmSpecification::HashAlgorithm::sha256, signature);
  out << "Message:<br>" << Crypto::convertListUnsignedCharsToHex(message);
  out << "<br>Padded message digest:<br>" << Crypto::convertListUnsignedCharsToHex(paddedMessage);
  out << "<br>Signature:<br>" << Crypto::convertListUnsignedCharsToHex(signature);
  ElementZmodP theElement, theOne;
  theElement.modulus = theKey.thePublicKey.theModulus;
  theOne.makeOne(theElement.modulus);
  Crypto::convertListUnsignedCharsToLargeUnsignedIntegerBigEndian(signature, theElement.value);
  out << "<br>Signature integer:<br>" << theElement.value.toString();
  MathRoutines::raiseToPower(theElement, theKey.thePublicKey.theExponent, theOne);
  out << "<br>Signature power e mod n [e = "
  << theKey.thePublicKey.theExponent << ", n = "
  << theKey.thePublicKey.theModulus << "]"
  << ":<br>" << theElement.value.toString();
  std::string theHex;
  Crypto::convertLargeUnsignedToHexSignificantDigitsFirst(theElement.value, 0, theHex);
  out << "<br>Converted to hex:<br>" << theHex;
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerRSAEncrypt(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerRSAEncrypt");
  if (input.size() != 4) {
    return false;
  }
  LargeInteger theExponent, theModulus, theMessage, result;
  if (
    !input[1].isInteger(& theModulus) ||
    !input[2].isInteger(&theExponent) ||
    !input[3].isInteger(&theMessage)
  ) {
    return calculator << "Failed to extract three (large) integers from the arguments of "
    << input.toString();
  }
  if (theModulus < 0) {
    theModulus *= - 1;
  }
  if (theModulus == 0 || theExponent == 0) {
    return calculator << "The modulus and exponent must be non-zero.";
  }
  if (theModulus == 1) {
    return calculator << "Modulus 1 not allowed";
  }
  result = Crypto::rsaEncrypt(theModulus.value, theExponent, theMessage);
  return output.assignValue(Rational(result), calculator);
}

bool CalculatorFunctions::innerSendEmailWithMailGun(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSendEmailWithMailGun");
  if (!global.userDefaultHasAdminRights()) {
    return calculator << "Sending mail available to logged-in admins only. ";
  }
  std::stringstream out;
  if (global.flagDatabaseCompiled) {
    if (input.size() != 4) {
      return calculator << "Send email requires three arguments. ";
    }
    EmailRoutines theEmail;
    if (
      !input[1].isOfType(&theEmail.toEmail) ||
      !input[2].isOfType(&theEmail.emailContent) ||
      !input[3].isOfType(&theEmail.subject)
    ) {
      return calculator << "Arguments of " << input.toString()
      << "expected to be strings (enclose in \"\" please). ";
    }
    theEmail.sendEmailWithMailGun(&out, &out, &out);
  } else {
    (void) input;
    out << "Error: database not running. ";
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerIsSquare(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsSquare");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger theLI;
  if (!input[1].isInteger(&theLI)) {
    return false;
  }
  if (theLI < 0) {
    return output.assignValue(0, calculator);
  }
  if (theLI == 0) {
    return output.assignValue(1, calculator);
  }
  List<int> theMults;
  List<LargeInteger> theFactors;
  if (!theLI.value.factor(theFactors, theMults, 0, 4, nullptr)) {
    return calculator << "Failed to factor: " << theLI.toString() << " (may be too large?).";
  }
  int result = 1;
  for (int i = 0; i < theMults.size; i ++) {
    if ((theMults[i] % 2) != 0) {
      result = 0;
      break;
    }
  }
  return output.assignValue(result, calculator);
}

bool CalculatorFunctions::innerIsSquareFree(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsSquareFree");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger theInteger;
  if (!input[1].isInteger(&theInteger)) {
    return false;
  }
  List<int> theMults;
  List<LargeInteger> theFactors;
  if (!theInteger.value.factor(
    theFactors, theMults, 0, 3, &calculator.comments
  )) {
    return calculator << "Failed to factor: "
    << theInteger.toString() << " (may be too large?).";
  }
  int result = 1;
  for (int i = 0; i < theMults.size; i ++) {
    if (theMults[i] > 1) {
      result = 0;
      break;
    }
  }
  return output.assignValue(result, calculator);
}

bool CalculatorFunctions::innerIsSquareFreePolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsSquareFreePolynomial");
  if (input.size() != 2) {
    return false;
  }
  WithContext<Polynomial<Rational> > polynomial;
  if (!calculator.convert(
    input, CalculatorConversions::innerPolynomial<Rational>, polynomial
  )) {
    return false;
  }
  std::stringstream out;
  bool squareFree = polynomial.content.isSquareFree(Rational::one(), &out);
  return output.assignValue(Rational(int(squareFree)), calculator);
}

bool CalculatorFunctions::innerIsPower(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsPower");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger toBeFactored;
  if (!input[1].isInteger(&toBeFactored)) {
    return false;
  }
  if (toBeFactored.isEqualToZero()) {
    return false;
  }
  List<int> theMults;
  List<LargeInteger> theFactors;
  if (!toBeFactored.value.factor(
    theFactors, theMults, 0, 3, &calculator.comments
  )) {
    return calculator << "Failed to factor: "
    << toBeFactored.toString() << " (may be too large?).";
  }
  int result = 1;
  if (theMults.size > 0) {
    result = (theMults[0] > 1);
  }
  for (int i = 1; i < theMults.size; i ++) {
    if (theMults[i] != theMults[0]) {
      result = 0;
      break;
    }
  }
  return output.assignValue(result, calculator);
}

bool CalculatorFunctions::innerFactorIntegeR(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFactorInteger");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctions::functionFactorInteger(calculator, input[1], output);
}

bool CalculatorFunctions::functionFactorInteger(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::functionFactorInteger");
  LargeInteger integer;
  int upperBound = 30000;
  int opFactorInteger = calculator.operations.getIndexNoFail("FactorInteger");

  if (input.startsWith(opFactorInteger) && input.size() == 3) {
    if (!input[1].isInteger(&integer)) {
      return false;
    }
    if (!input[2].isIntegerFittingInInt(&upperBound)) {
      return calculator << "Failed to extract integer from " << upperBound << ".";
    }
  } else if (!input.isInteger(&integer)) {
    return false;
  }
  if (integer.isEqualToZero()) {
    return false;
  }
  List<LargeInteger> primeFactors;
  List<int> multiplicities;
  bool complete = integer.value.factor(
    primeFactors,
    multiplicities,
    upperBound,
    3,
    &calculator.comments
  );
  if (!complete && primeFactors.size <= 1) {
    return false;
  }
  List<Expression> result;
  int numberOfPrimeFactors = primeFactors.size;
  if (!complete) {
    numberOfPrimeFactors --;
  }
  for (int i = 0; i < numberOfPrimeFactors; i ++) {
    Expression currentE;
    currentE.assignValue(Rational(primeFactors[i]), calculator);
    for (int j = 0; j < multiplicities[i]; j ++) {
      result.addOnTop(currentE);
    }
  }
  if (integer < 0 && result.size > 0) {
    result[0] *= - 1;
  }
  if (complete) {
    return output.makeSequence(calculator, &result);
  } else {
    Expression factorsSoFar, factorNext, numberLast;
    factorsSoFar.makeSequence(calculator, &result);
    factorNext.reset(calculator);
    factorNext.addChildAtomOnTop(opFactorInteger);
    numberLast.assignValue(Rational(primeFactors[primeFactors.size - 1]), calculator);
    factorNext.addChildOnTop(numberLast);
    return output.makeXOX(calculator, calculator.opUnion(), factorsSoFar, factorNext);
  }
}

bool CalculatorFunctions::innerFactorOutNumberContent(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFactorOutNumberContent");
  if (input.size() != 2) {
    return calculator << "FactorOutNumberContent expects single argument. ";
  }
  LinearCombination<Expression, Rational> theV;
  if (!calculator.functionCollectSummands(calculator, input[1], theV)) {
    return calculator << "Failed to extract summands from: " << input[1].toString();
  }
  if (theV.isEqualToZero()) {
    return output.assignValue(0, calculator);
  }
  Rational theCF = theV.scaleNormalizeLeadingMonomial(nullptr);
  if (theCF == 0) {
    return false;
  }
  theCF.invert();
  if (theCF == 1 ) {
    output = input[1];
    return true;
  }
  Expression left, right;
  left.assignValue(theCF, calculator);
  right.makeSum(calculator, theV);
  output = left * right;
  return true;
}

bool CalculatorFunctions::innerSubList(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSubList");
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isSequenceNElements()) {
    return false;
  }
  HashedList<Expression> boundVars;
  if (!input[2].getBoundVariables(boundVars)) {
    return calculator << "Could not get bound variables from: " << input[2].toString();
  }
  if (input[2].isEqualToOne()) {
    output = input[1];
    return true;
  }
  if (boundVars.size == 0) {
    return output.makeSequence(calculator, nullptr);
  }
  Expression theSubbed, toBeSubbed, subbedSimplified;
  toBeSubbed.reset(calculator);
  toBeSubbed.addChildAtomOnTop(calculator.opBind());
  toBeSubbed.addChildOnTop(boundVars[0]);
  List<Expression> theList;
  for (int i = 1; i < input[1].size(); i ++) {
    theSubbed = input[2];
    theSubbed.substituteRecursively(toBeSubbed, input[1][i]);
    if (!calculator.evaluateExpression(calculator, theSubbed, subbedSimplified)) {
      return calculator << "Failed to evaluate " << theSubbed.toString();
    }
    if (subbedSimplified.isEqualToOne()) {
      theList.addOnTop(input[1][i]);
    }
  }
  return output.makeSequence(calculator, &theList);
}

bool CalculatorFunctions::innerApplyToList(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerApplyToList");
  if (input.size() != 3) {
    return false;
  }
  const Expression& theFun = input[1];
  if (!input[2].isSequenceNElements()) {
    return false;
  }
  List<Expression> result;
  result.setSize(input[2].size() - 1);
  for (int i = 1; i < input[2].size(); i ++) {
    result[i - 1].reset(calculator);
    result[i - 1].addChildOnTop(theFun);
    result[i - 1].addChildOnTop(input[2][i]);
  }
  return output.makeSequence(calculator, &result);
}

bool CalculatorFunctions::innerArccosAlgebraic(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerArccosAlgebraic");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argumentE = input[1];
  Rational theRat;
  if (argumentE.isRational(&theRat)) {
    if (theRat == 1) {
      return output.assignValue(0, calculator);
    }
    if (theRat == 0) {
      output.makeAtom(calculator.opPi(), calculator);
      output /= 2;
      return true;
    }
    if (theRat == - 1) {
      return output.makeAtom(calculator.opPi(), calculator);
    }
    if (theRat == Rational(1, 2)) {
      output.makeAtom(calculator.opPi(), calculator);
      output /= 3;
      return true;
    }
    if (theRat == Rational(- 1, 2)) {
      output.makeAtom(calculator.opPi(), calculator);
      output *= 2;
      output /= 3;
      return true;
    }
  }
  AlgebraicNumber argument, candidate;
  if (argumentE.isOfType<AlgebraicNumber>(&argument)) {
    candidate.assignRationalQuadraticRadical(
      Rational(1, 2),
      calculator.theObjectContainer.theAlgebraicClosure,
      nullptr
    );
    if (candidate == argument) {
      output.makeAtom(calculator.opPi(), calculator);
      output /= 4;
      return true;
    }
    candidate *= - 1;
    if (candidate ==argument) {
      output.makeAtom(calculator.opPi(), calculator);
      output /= 4;
      output *= 3;
      return true;
    }
    candidate.assignRationalQuadraticRadical(
      Rational(3, 4),
      calculator.theObjectContainer.theAlgebraicClosure,
      nullptr
    );
    if (candidate == argument) {
      output.makeAtom(calculator.opPi(), calculator);
      output /= 6;
      return true;
    }
    candidate *= - 1;
    if (candidate == argument) {
      output.makeAtom(calculator.opPi(), calculator);
      output /= 6;
      output *= 5;
      return true;
    }
  }
  return false;
}

bool CalculatorFunctions::innerArcsinAlgebraic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerArcsinAlgebraic");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argumentE = input[1];
  Rational theRat;
  if (argumentE.isRational(&theRat)) {
    if (theRat == 1) {
      output.makeAtom(calculator.opPi(), calculator);
      output /= 2;
      return true;
    }
    if (theRat == 0) {
      return output.assignValue(0, calculator);
    }
    if (theRat == - 1) {
      output.makeAtom(calculator.opPi(), calculator);
      output /= - 2;
      return true;
    }
    if (theRat == Rational(1, 2)) {
      output.makeAtom(calculator.opPi(), calculator);
      output /= 6;
      return true;
    }
    if (theRat == Rational(- 1, 2)) {
      output.makeAtom(calculator.opPi(), calculator);
      output /= - 6;
      return true;
    }
  }
  AlgebraicNumber argument, candidate;
  if (argumentE.isOfType<AlgebraicNumber>(&argument)) {
    candidate.assignRationalQuadraticRadical(
      Rational(1, 2),
      calculator.theObjectContainer.theAlgebraicClosure,
      nullptr
    );
    if (candidate == argument) {
      output.makeAtom(calculator.opPi(), calculator);
      output /= 4;
      return true;
    }
    candidate *= - 1;
    if (candidate == argument) {
      output.makeAtom(calculator.opPi(), calculator);
      output /= - 4;
      return true;
    }
    candidate.assignRationalQuadraticRadical(
      Rational(3, 4),
      calculator.theObjectContainer.theAlgebraicClosure,
      nullptr
    );
    if (candidate == argument) {
      output.makeAtom(calculator.opPi(), calculator);
      output /= 3;
      return true;
    }
    candidate *= - 1;
    if (candidate == argument) {
      output.makeAtom(calculator.opPi(), calculator);
      output /= - 3;
      return true;
    }
  }
  return false;
}

bool CalculatorFunctions::innerMatchesPattern(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerMatchesPattern");
  if (input.size() != 3) {
    return false;
  }
  MapList<Expression, Expression> matchedExpressions;
  if (!calculator.expressionMatchesPattern(input[2], input[1], matchedExpressions, nullptr)) {
    return output.assignValue(0, calculator);
  }
  Expression commandList;
  commandList.reset(calculator);
  commandList.addChildAtomOnTop(calculator.opEndStatement());
  for (int i = 0; i < matchedExpressions.size(); i ++) {
    Expression currentCommand;
    if (!matchedExpressions.theKeys[i].startsWith(calculator.opBind(), 2)) {
      std::stringstream errorStream;
      errorStream << "Bound variable " << matchedExpressions.theKeys[i].toString()
      << " does not start with the bind atom. ";
      return output.makeError(errorStream.str(), calculator);
    }
    currentCommand.makeXOX(
      calculator,
      calculator.opDefine(),
      matchedExpressions.theKeys[i][1],
      matchedExpressions.theValues[i]
    );
    commandList.addChildOnTop(currentCommand);
  }
  output.reset(calculator);
  output.addChildAtomOnTop(calculator.opCommandEnclosure());
  return output.addChildOnTop(commandList);
}

bool CalculatorFunctions::innerDegreesToRadians(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDegreesToRadians");
  if (!input.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  if (!input[2].isOperationGiven("\\circ")) {
    return false;
  }
  Expression piE;
  piE.makeAtom(calculator.opPi(), calculator);
  output = input[1] * piE;
  output /= 180;
  return true;
}

bool CalculatorFunctions::innerLessThanOrEqualTo(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerLessThanOrEqualTo");
  if (input.size() != 3) {
    return false;
  }
  Expression result(calculator);
  result.addChildAtomOnTop("\\geq");
  result.addChildOnTop(input[2]);
  result.addChildOnTop(input[1]);
  output = result;
  return true;
}

bool CalculatorFunctions::innerGreaterThanOrEqualTo(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGreaterThanOrEqualTo");
  if (input.size() != 3) {
    return false;
  }
  const Expression& left = input[1];
  const Expression& right = input[2];
  Rational leftRat, rightRat;
  if (left.isRational(&leftRat) && right.isRational(&rightRat)) {
    if (leftRat >= rightRat) {
      return output.assignValue(1, calculator);
    }
    return output.assignValue(0, calculator);
  }
  double leftD, rightD;
  if (left.evaluatesToDouble(&leftD) && right.evaluatesToDouble(&rightD)) {
    if (leftD >= rightD) {
      return output.assignValue(1, calculator);
    }
    return output.assignValue(0, calculator);
  }
  return false;
}

bool CalculatorFunctions::innerGreaterThan(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGreaterThan");
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression& left = input[1];
  const Expression& right = input[2];
  Rational leftRat, rightRat;
  if (left.isRational(&leftRat) && right.isRational(&rightRat)) {
    if (leftRat > rightRat) {
      return output.assignValue(1, calculator);
    }
    return output.assignValue(0, calculator);
  }
  double leftD, rightD;
  if (left.evaluatesToDouble(&leftD) && right.evaluatesToDouble(&rightD)) {
    if (leftD > rightD) {
      return output.assignValue(1, calculator);
    }
    return output.assignValue(0, calculator);
  }
  return false;
}

bool CalculatorFunctions::innerLessThan(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerLessThan");
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression swappedE(calculator);
  swappedE.addChildOnTop(input[0]);
  swappedE.addChildOnTop(input[2]);
  swappedE.addChildOnTop(input[1]);
  return CalculatorFunctions::innerGreaterThan(calculator, swappedE, output);
}

bool CalculatorFunctions::innerCollectOpands(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCollectOpands");
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    return false;
  }
  List<Expression> theList;
  calculator.appendOpandsReturnTrueIfOrderNonCanonical(input[2], theList, input[1].theData);
  return output.makeSequence(calculator, &theList);
}

bool CalculatorFunctions::innerCollectMultiplicands(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerCollectSummands");
  if (input.size() != 2) {
    return false;
  }
  List<Expression> theList;
  calculator.appendOpandsReturnTrueIfOrderNonCanonical(input[1], theList, calculator.opTimes());
  return output.makeSequence(calculator, &theList);
}

bool CalculatorFunctions::innerCollectSummands(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCollectSummands");
  if (input.size() != 2) {
    return false;
  }
  List<Expression> theList;
  calculator.appendSummandsReturnTrueIfOrderNonCanonical(input[1], theList);
  return output.makeSequence(calculator, &theList);
}

bool CalculatorFunctions::leftIntervalGreaterThanRight(const Expression& left, const Expression& right) {
  if (left.owner == nullptr || right.owner == nullptr) {
    return left > right;
  }
  if (left.size() != 3 || right.size() != 3) {
    return left > right;
  }
  if (!left.isIntervalRealLine()) {
    return left > right;
  }
  if (!right.isIntervalRealLine()) {
    return left > right;
  }
  double left1, right1, left2, right2;
  bool left1IsDouble = left[1].evaluatesToDouble(&left1);
  bool right1IsDouble = right[1].evaluatesToDouble(&right1);
  bool left2IsDouble = left[2].evaluatesToDouble(&left2);
  bool righ2IsDouble = right[2].evaluatesToDouble(&right2);
  const Expression& inftyE = right.owner->expressionInfinity();
  const Expression& mInftyE = right.owner->expressionMinusInfinity();
  if (left1IsDouble && right1IsDouble) {
    if (left1 > right1) {
      return true;
    }
    if (right1 > left1) {
      return false;
    }
    if (left2IsDouble && righ2IsDouble) {
      if (left2 > right2) {
        return true;
      }
      if (right2 > left2) {
        return false;
      }
    }
    if (left[2] == inftyE && righ2IsDouble) {
      return true;
    }
    if (left2IsDouble && right[2] == inftyE) {
      return false;
    }
  }
  if (left[1] == mInftyE && right1IsDouble) {
    return false;
  }
  if (left1IsDouble && right[1] == mInftyE) {
    return true;
  }
  if (left[1] == mInftyE || right[1] == mInftyE) {
    if (left2IsDouble && righ2IsDouble) {
      if (left2 > right2) {
        return true;
      }
      if (right2 > left2) {
        return false;
      }
    }
    if (left[2] == inftyE && righ2IsDouble) {
      return true;
    }
    if (left2IsDouble && right[2] == inftyE) {
      return false;
    }
  }
  return left > right;
}

bool CalculatorFunctions::innerIntersectIntervals(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntersectIntervals");
  if (!input.startsWith(calculator.opIntersection(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!leftE.isIntervalRealLine()) {
    return false;
  }
  if (!rightE.isIntervalRealLine()) {
    return false;
  }
  double left1 = 0, left2 = 0, right1 = 0, right2 = 0;
  if (!leftE[1].evaluatesToDouble(&left1) || !leftE[2].evaluatesToDouble(&left2)) {
    return false;
  }
  if (!rightE[1].evaluatesToDouble(&right1) || !rightE[2].evaluatesToDouble(&right2)) {
    return false;
  }
  if (left1 > left2 || right1 > right2) {
    return false;
  }
  bool leftIsClosed = true;
  bool rightIsClosed = true;
  Expression leftFinal, rightFinal;
  double leftResult = 0, rightResult = 0;
  if (left1 < right1) {
    leftFinal = rightE[1];
    leftResult = right1;
    if (
      rightE.startsWith(calculator.opSequence()) ||
      rightE.startsWith(calculator.opIntervalRightClosed())
    ) {
      leftIsClosed = false;
    }
  }
  if (left1 - right1 == 0.0) {
    leftFinal = leftE[1];
    leftResult = left1;
    if (
      leftE .startsWith(calculator.opSequence()) ||
      leftE .startsWith(calculator.opIntervalRightClosed())||
      rightE.startsWith(calculator.opSequence()) ||
      rightE.startsWith(calculator.opIntervalRightClosed())
    ) {
      leftIsClosed = false;
    }
  }
  if (left1 > right1) {
    leftFinal = leftE[1];
    leftResult = left1;
    if (
      leftE.startsWith(calculator.opSequence()) ||
      leftE.startsWith(calculator.opIntervalRightClosed())
    ) {
      leftIsClosed = false;
    }
  }
///////////////////////////
  if (left2 > right2) {
    rightFinal = rightE[2];
    rightResult = right2;
    if (
      rightE.startsWith(calculator.opSequence()) ||
      rightE.startsWith(calculator.opIntervalLeftClosed())
    ) {
      rightIsClosed = false;
    }
  }
  if (left2 - right2 == 0.0) {
    rightFinal = rightE[2];
    rightResult = right2;
    if (
      leftE .startsWith(calculator.opSequence()) ||
      leftE .startsWith(calculator.opIntervalLeftClosed())||
      rightE.startsWith(calculator.opSequence()) ||
      rightE.startsWith(calculator.opIntervalLeftClosed())
    ) {
      rightIsClosed = false;
    }
  }
  if (left2 < right2) {
    rightFinal = leftE[2];
    rightResult = left2;
    if (
      leftE.startsWith(calculator.opSequence()) ||
      leftE.startsWith(calculator.opIntervalLeftClosed())
    ) {
      rightIsClosed = false;
    }
  }
  if (leftResult > rightResult) {
    return output.makeAtom("\\emptyset", calculator);
  }
  if (leftIsClosed && rightIsClosed) {
    return output.makeXOX(calculator, calculator.opIntervalClosed(), leftFinal, rightFinal);
  }
  if (!leftIsClosed && rightIsClosed) {
    return output.makeXOX(calculator, calculator.opIntervalRightClosed(), leftFinal, rightFinal);
  }
  if (leftIsClosed && !rightIsClosed) {
    return output.makeXOX(calculator, calculator.opIntervalLeftClosed(), leftFinal, rightFinal);
  }
  if (!leftIsClosed && !rightIsClosed) {
    return output.makeXOX(calculator, calculator.opSequence(), leftFinal, rightFinal);
  }
  return false;
}

bool CalculatorFunctions::innerUnionUnionIntervals(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerUnionUnionIntervals");
  if (input.size() != 3) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightComposite = input[2];
  if (!rightComposite.startsWith(calculator.opUnion(), 3)) {
    return false;
  }
  const Expression& rightE = input[2][1];
  Expression theMiddleUnion, middleUnionReduced;
  theMiddleUnion.makeXOX(calculator, calculator.opUnion(), leftE, rightE);
  if (!CalculatorFunctions::innerUnionIntervals(calculator, theMiddleUnion, middleUnionReduced)) {
    return false;
  }
  return output.makeXOX(calculator, calculator.opUnion(), middleUnionReduced, rightComposite[2]);
}

bool CalculatorFunctions::innerUnionIntervals(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerUnionIntervals");
  if (input.size() != 3) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!leftE.isIntervalRealLine()) {
    return false;
  }
  if (!rightE.isIntervalRealLine()) {
    return false;
  }
  double left1 = 0, left2 = 0, right1 = 0, right2 = 0;
  if (!leftE[1].evaluatesToDouble(&left1) || !leftE[2].evaluatesToDouble(&left2)) {
    return false;
  }
  if (!rightE[1].evaluatesToDouble(&right1) || !rightE[2].evaluatesToDouble(&right2)) {
    return false;
  }
  if (left1 > left2 || right1 > right2) {
    return false;
  }
  bool makeUnion = false;
  if (right1 <= left2 && left2 <= right2) {
    makeUnion = true;
  }
  if (
    (right1 - left2 == 0.0) &&
    leftE.startsWith(calculator.opIntervalOpen(), 3) &&
    rightE.startsWith(calculator.opIntervalOpen(), 3)
  ) {
    makeUnion = false;
  }
  if (!makeUnion) {
    return false;
  }
  bool leftIsClosed = false;
  bool rightIsClosed = false;
  Expression leftFinal, rightFinal;
  if (left1 < right1) {
    leftFinal = leftE[1];
    if (
      leftE.startsWith(calculator.opIntervalClosed()) ||
      leftE.startsWith(calculator.opIntervalLeftClosed())
    ) {
      leftIsClosed = true;
    }
  }
  if (left1 - right1 == 0.0) {
    leftFinal = leftE[1];
    if (
      leftE .startsWith(calculator.opIntervalClosed()) ||
      leftE .startsWith(calculator.opIntervalLeftClosed())||
      rightE.startsWith(calculator.opIntervalClosed()) ||
      rightE.startsWith(calculator.opIntervalLeftClosed())
    ) {
      leftIsClosed = true;
    }
  }
  if (left1 > right1) {
    leftFinal = rightE[1];
    if (
      rightE.startsWith(calculator.opIntervalClosed()) ||
      rightE.startsWith(calculator.opIntervalLeftClosed())
    ) {
      leftIsClosed = true;
    }
  }
///////////////////////
  if (left2 > right2) {
    rightFinal = leftE[2];
    if (
      leftE.startsWith(calculator.opIntervalClosed()) ||
      leftE.startsWith(calculator.opIntervalRightClosed())
    ) {
      rightIsClosed = true;
    }
  }
  if (left2 - right2 == 0.0) {
    rightFinal = rightE[2];
    if (
      leftE .startsWith(calculator.opIntervalClosed()) ||
      leftE .startsWith(calculator.opIntervalRightClosed())||
      rightE.startsWith(calculator.opIntervalClosed()) ||
      rightE.startsWith(calculator.opIntervalRightClosed())
    ) {
      rightIsClosed = true;
    }
  }
  if (left2 < right2) {
    rightFinal = rightE[2];
    if (
      rightE.startsWith(calculator.opIntervalClosed()) ||
      rightE.startsWith(calculator.opIntervalRightClosed())
    ) {
      rightIsClosed = true;
    }
  }
  if (leftIsClosed && rightIsClosed) {
    return output.makeXOX(calculator, calculator.opIntervalClosed(), leftFinal, rightFinal);
  }
  if (!leftIsClosed && rightIsClosed) {
    return output.makeXOX(calculator, calculator.opIntervalRightClosed(), leftFinal, rightFinal);
  }
  if (leftIsClosed && !rightIsClosed) {
    return output.makeXOX(calculator, calculator.opIntervalLeftClosed(), leftFinal, rightFinal);
  }
  if (!leftIsClosed && !rightIsClosed) {
    return output.makeXOX(calculator, calculator.opIntervalOpen(), leftFinal, rightFinal);
  }
  return false;
}

bool CalculatorFunctions::innerNormalizeIntervals(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerNormalizeIntervals");
  if (input.size() != 2) {
    return false;
  }
  List<Expression> outputList;
  if (!calculator.collectOpands(input[1], calculator.opUnion(), outputList)) {
    return false;
  }
  List<Expression>::Comparator order(CalculatorFunctions::leftIntervalGreaterThanRight);
  outputList.quickSortAscending(&order);
  return output.makeXOXOdotsOX(calculator, calculator.opUnion(), outputList);
}

bool CalculatorFunctions::innerUnionEmptySet(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerUnionEmptySet");
  if (!input.startsWith(calculator.opUnion(), 3)) {
    return false;
  }
  if (input[1].isOperationGiven("\\emptyset")) {
    output = input[2];
    return true;
  }
  if (input[2].isOperationGiven("\\emptyset")) {
    output = input[1];
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerIntersectUnion(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntersectUnion");
  return CalculatorBasics::distribute(calculator, input, output, calculator.opUnion(), calculator.opIntersection(), false);
}

bool CalculatorFunctions::innerCompareIntervalsNumerically(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompareIntervalsNumerically");
  if (input.size() != 4) {
    return calculator << "Comparing intervals numerically takes 3 arguments: two unions and precision. ";
  }
  double precision = 0;
  if (!input[3].evaluatesToDouble(&precision)) {
    return calculator << "Could not extract precision from the last argument.";
  }
  List<Expression> leftList, rightList;
  if (!calculator.collectOpands(input[1], calculator.opUnion(), leftList)) {
    return false;
  }
  if (!calculator.collectOpands(input[2], calculator.opUnion(), rightList)) {
    return false;
  }
  if (leftList.size != rightList.size) {
    return output.assignValue(0, calculator);
  }
  List<Expression>::Comparator order(CalculatorFunctions::leftIntervalGreaterThanRight);
  leftList.quickSortAscending(&order);
  rightList.quickSortAscending(&order);
  for (int i = 0; i < leftList.size; i ++) {
    if (leftList[i] == rightList[i]) {
      continue;
    }
    if ((!leftList[i].isIntervalRealLine() ) || (!rightList[i].isIntervalRealLine())) {
      return output.assignValue(0, calculator);
    }
    if (leftList[i][0] != rightList[i][0]) {
      return output.assignValue(0, calculator);
    }
    double left1 = 0, left2 = 0, right1 = 0, right2 = 0;
    if (
      !leftList[i][1].evaluatesToDouble(&left1) ||
      !rightList[i][1].evaluatesToDouble(&right1)
    ) {
      if (leftList[i][1] != rightList[i][1]) {
        return output.assignValue(0, calculator);
      }
    }
    if (
      !rightList[i][2].evaluatesToDouble(&right2)||
      !leftList[i][2].evaluatesToDouble(&left2)
    ) {
      if (leftList[i][2] != rightList[i][2]) {
        return output.assignValue(0, calculator);
      }
    }
    if (
      FloatingPoint::absFloating(left1 - right1) > precision ||
      FloatingPoint::absFloating(left2 - right2) > precision
    ) {
      return output.assignValue(0, calculator);
    }
  }
  return output.assignValue(1, calculator);

}

bool CalculatorFunctions::innerIntersectEmptySet(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntersectEmptySet");
  if (!input.startsWith(calculator.opIntersection(), 3)) {
    return false;
  }
  if (input[1].isOperationGiven("\\emptyset")) {
    output = input[1];
    return true;
  }
  if (input[2].isOperationGiven("\\emptyset")) {
    output = input[2];
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerIsLinearOrConstantIn(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsLinearOrConstantIn");
  if (input.size() < 3) {
    return false;
  }
  List<List<Expression> > theSummands;
  if (!calculator.getSumProductsExpressions(input[2], theSummands)) {
    return calculator << "Failed to extract sum from "
    << input[2].toString();
  }
  for (int i = 0; i < theSummands.size; i ++) {
    bool found = false;
    for (int j = 0; j < theSummands[i].size; j ++) {
      if (theSummands[i][j] == input[1]) {
        if (found) {
          return output.assignValue(0, calculator);
        }
        found = true;
      } else if (!theSummands[i][j].evaluatesToDouble()) {
        return output.assignValue(0, calculator);
      }
    }
  }
  return output.assignValue(1, calculator);
}

bool CalculatorFunctions::innerIsProductLinearOrConstantTermsIn(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsProductLinearOrConstantTermsIn");
  if (input.size() < 3) {
    return false;
  }
  List<Expression> theMultiplicands;
  if (!calculator.collectOpands(input[2], calculator.opTimes(),theMultiplicands)) {
    return calculator << "Could not extract multiplicands from: "
    << input[2].toString();
  }
  for (int k = 0; k < theMultiplicands.size; k ++) {
    List<List<Expression> > theSummands;
    if (!calculator.getSumProductsExpressions(theMultiplicands[k], theSummands)) {
      return calculator << "Failed to extract sum from "
      << theMultiplicands[k].toString();
    }
    for (int i = 0; i < theSummands.size; i ++) {
      bool found = false;
      for (int j = 0; j < theSummands[i].size; j ++) {
        if (theSummands[i][j] == input[1]) {
          if (found) {
            return output.assignValue(0, calculator);
          }
          found = true;
        } else if (!theSummands[i][j].evaluatesToDouble()) {
          return output.assignValue(0, calculator);
        }
      }
    }
  }
  return output.assignValue(1, calculator);
}

bool CalculatorFunctions::innerOrIdentical(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerOrIdentical");
  (void) calculator;
  if (!input.startsWithGivenOperation("or", 3)) {
    return false;
  }
  if (input[1] == input[2]) {
    output = input[1];
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerSineOfAngleSumToTrig(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSineOfAngleSumToTrig");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (!argument.startsWith(calculator.opPlus(), 3)) {
    return false;
  }
  Expression sinA, sinB, cosA, cosB;
  sinA.makeOX(calculator, calculator.opSin(), argument[1]);
  sinB.makeOX(calculator, calculator.opSin(), argument[2]);
  cosA.makeOX(calculator, calculator.opCos(), argument[1]);
  cosB.makeOX(calculator, calculator.opCos(), argument[2]);
  output = sinA * cosB + cosA * sinB;
  return true;
}

bool CalculatorFunctions::innerTrigSumToTrigProduct(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTrigSumToTrigProduct");
  if (!input.startsWith(calculator.opPlus(), 3)) {
    return false;
  }
  Expression leftE = input[1], rightE = input[2];
  bool isGood = false;
  int leftSign = 1;
  int rightSign = 1;
  if (leftE.startsWith(calculator.opTimes(), 3)) {
    if (leftE[1].isEqualToMOne()) {
      leftE = leftE[2];
      leftSign = - 1;
    }
  }
  if (rightE.startsWith(calculator.opTimes(), 3)) {
    if (rightE[1].isEqualToMOne()) {
      rightE = rightE[2];
      rightSign = - 1;
    }
  }
  if (
    leftE.startsWith(calculator.opSin(), 2) &&
    rightE.startsWith(calculator.opSin(), 2)
  ) {
    isGood = true;
  } else if (
    leftE.startsWith(calculator.opCos(), 2) &&
    rightE.startsWith(calculator.opCos(), 2)
  ) {
    isGood = true;
  }
  if (!isGood) {
    return false;
  }
  Expression argSum, argDiff, leftMultiplicand, rightMultiplicand;
  if (leftE.startsWith(calculator.opSin(), 2)) {
    argSum =  (leftE[1] * leftSign + rightE[1] * rightSign) / 2;
    argDiff = (leftE[1] * leftSign - rightE[1] * rightSign) / 2;
    leftMultiplicand.makeOX(calculator, calculator.opCos(), argDiff);
    rightMultiplicand.makeOX(calculator, calculator.opSin(), argSum);
    output = leftMultiplicand * rightMultiplicand * 2;
    return true;
  } else {
    if (leftSign == rightSign) {
      argSum = (leftE[1] + rightE[1]) / 2;
      argDiff = (leftE[1] - rightE[1]) / 2;
      leftMultiplicand.makeOX(calculator, calculator.opCos(), argDiff);
      rightMultiplicand.makeOX(calculator, calculator.opCos(), argSum);
      output = leftMultiplicand * rightMultiplicand * 2;
      if (leftSign == - 1) {
        output *= - 1;
      }
      return true;
    } else {
      argSum = (leftE[1] + rightE[1]) / 2;
      argDiff = (leftE[1] * leftSign + rightE[1] * rightSign) / 2;
      leftMultiplicand.makeOX(calculator, calculator.opSin(), argDiff);
      rightMultiplicand.makeOX(calculator, calculator.opSin(), argSum);
      output = leftMultiplicand * rightMultiplicand * 2;
      return true;
    }
  }
  // return false;
}

bool CalculatorFunctions::innerCosineOfAngleSumToTrig(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCosineOfAngleSumToTrig");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (!argument.startsWith(calculator.opPlus(), 3)) {
    return false;
  }
  Expression sinA, sinB, cosA, cosB;
  sinA.makeOX(calculator, calculator.opSin(), argument[1]);
  sinB.makeOX(calculator, calculator.opSin(), argument[2]);
  cosA.makeOX(calculator, calculator.opCos(), argument[1]);
  cosB.makeOX(calculator, calculator.opCos(), argument[2]);
  output = cosA * cosB - sinA * sinB;
  return true;
}

bool CalculatorFunctions::innerDistributeSqrt(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerDistributeSqrt");
  if (!input.startsWith(calculator.opSqrt(), 3)) {
    return false;
  }
  const Expression& base = input[2];
  const Expression& oneOverExponentE = input[1];
  if (!base.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (!base[1].isConstantNumber()) {
    return false;
  }
  bool isGood = base[1].isPositiveNumber() || base[2].isPositiveNumber();
  if (!isGood) {
    if (oneOverExponentE.isInteger()) {
      isGood = true;
    } else {
      Rational exponentRat;
      if (oneOverExponentE.isRational(&exponentRat)) {
        if (!exponentRat.getDenominator().isEven()) {
          isGood = true;
        }
      }
    }
  }
  if (!isGood) {
    return false;
  }
  Expression leftE, rightE;
  leftE.makeXOX(calculator, calculator.opSqrt(), oneOverExponentE, base[1]);
  rightE.makeXOX(calculator, calculator.opSqrt(), oneOverExponentE, base[2]);
  return output.makeXOX(calculator, calculator.opTimes(), leftE, rightE);
}

bool CalculatorFunctions::innerIsAlgebraicRadical(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsAlgebraicRadical");
  if (input.size() != 2) {
    return false;
  }
  int result = static_cast<int>(input[1].isAlgebraicRadical());
  return output.assignValue(result, calculator);
}

bool CalculatorFunctions::innerIsReal(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsReal");
  if (input.size() != 2) {
    return false;
  }
  if (!input[1].evaluatesToDouble()) {
    return output.assignValue(0, calculator);
  }
  return output.assignValue(1, calculator);
}

bool CalculatorFunctions::innerExpressiontoUTF8String(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerExpressiontoUTF8String");
  if (input.size() > 2) {
    Expression argumentSequence = input;
    argumentSequence.setChildAtomValue(0, calculator.opSequence());
    return output.assignValue(argumentSequence.toUTF8String(), calculator);
  }
  return output.assignValue(input[1].toUTF8String(), calculator);
}

bool CalculatorFunctions::innerIsProductTermsUpToPower(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsProductTermsUpToPower");
  if (input.size() < 3) {
    return false;
  }
  Expression theBase;
  theBase = input[1];
  LargeInteger desiredMaxPower = 1;
  if (theBase.startsWith(calculator.opThePower(), 3)) {
    if (theBase[2].isInteger(&desiredMaxPower)) {
      if (desiredMaxPower > 0) {
        theBase = input[1][1];
      } else {
        desiredMaxPower = 1;
      }
    } else {
      desiredMaxPower = 1;
    }
  }
  List<Expression> theMultiplicands;
  if (!calculator.collectOpands(input[2], calculator.opTimes(), theMultiplicands)) {
    return calculator << "Could not extract multiplicands from: "
    << input[2].toString();
  }

  for (int k = 0; k < theMultiplicands.size; k ++) {
    List<List<Expression> > theSummands;
    if (!calculator.getSumProductsExpressions(theMultiplicands[k], theSummands)) {
      return calculator << "Failed to extract sum from "
      << theMultiplicands[k].toString();
    }
    for (int i = 0; i < theSummands.size; i ++) {
      LargeInteger foundPower = 0;
      for (int j = 0; j < theSummands[i].size; j ++) {
        if (theSummands[i][j] == theBase) {
          foundPower ++;
          continue;
        }
        if (theSummands[i][j].startsWith(calculator.opThePower(), 3)) {
          if (theSummands[i][j][1] == theBase) {
            LargeInteger localPower;
            if (theSummands[i][j][2].isInteger(&localPower)) {
              foundPower += localPower;
              continue;
            }
          }
        }
        if (!theSummands[i][j].evaluatesToDouble()) {
          return output.assignValue(0, calculator);
        }
      }
      if (foundPower > desiredMaxPower) {
        return output.assignValue(0, calculator);
      }
    }
  }
  return output.assignValue(1, calculator);
}

bool CalculatorFunctions::innerScaleToLeadingUnit(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerScaleToLeadingUnit");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  LinearCombination<Expression, Rational> theCollection;
  calculator.functionCollectSummands(calculator, argument, theCollection);
  theCollection /= theCollection.getLeadingCoefficient(nullptr);
  return output.makeSum(calculator, theCollection);
}

bool CalculatorFunctionsBinaryOps::innerPowerRationalByRationalOutputAlgebraic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerRatByRatReducePrimeFactors");
  if (!input.startsWith(calculator.opThePower(), 3)) {
    return false;
  }
  Rational exponent;
  if (!input[1].isOfType<Rational>()) {
    return false;
  }
  if (!input[2].isOfType<Rational>(&exponent)) {
    return false;
  }
  if (exponent.getDenominator() != 2) {
    return false;
  }
  Expression theRadical, reduced;
  theRadical.makeXOX(calculator, calculator.opSqrt(), calculator.expressionTwo(), input[1]);
  if (!CalculatorFunctions::innerSqrt(calculator, theRadical, reduced)) {
    return false;
  }
  if (!reduced.isOfType<AlgebraicNumber>()) {
    return false;
  }
  Expression theIntegerPower;
  theIntegerPower.assignValue(Rational(exponent.getNumerator()), calculator);
  return output.makeXOX(calculator, calculator.opThePower(),reduced, theIntegerPower);
}

bool CalculatorFunctions::innerNewtonsMethod(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerNewtonsMethod");
  if (input.size() != 4) {
    return false;
  }
  Expression theFun;
  if (!CalculatorFunctions::functionEqualityToArithmeticExpression(calculator, input[1], theFun)) {
    theFun = input[1];
  }
  HashedList<Expression> theVars;
  if (!theFun.getFreeVariables(theVars, true)) {
    return calculator << "Failed to get free variables from: " << theFun.toString();
  }
  if (theVars.size != 1) {
    return calculator << "While trying to extract a function from: " << theFun.toString()
    << ", got " << theVars.size << " variables. Newton's method requires an expression that depends "
    << "on exactly one variable. The variables I got were: "
    << theVars.toStringCommaDelimited();
  }
  int numIterations = - 1;
  if (!input[3].isSmallInteger(&numIterations)) {
    std::stringstream errorStream;
    errorStream << "While doing Newton's method, could not extract a **small** integer from the third argument "
    << input[3].toString() << " of "
    << input.toString() << ". Please enter a number as the third argument of Newton's method. ";
    return output.makeError(errorStream.str(), calculator);
  }
  if (numIterations < 1 || numIterations > 50) {
    std::stringstream errorStream;
    errorStream << "While doing Newton's method with the command: " << input.toString()
    << ", the third argument requests " << numIterations
    << " iterations. However, the number of iterations is required to be a number between 1 and 50. ";
    return output.makeError(errorStream.str(), calculator);
  }
  MapList<std::string, Expression, MathRoutines::hashString> theSub;
  theSub.setKeyValue("x", theVars[0]);
  theSub.setKeyValue("f", theFun);
  theSub.setKeyValue("a", calculator.getNewAtom());
  theSub.setKeyValue("iteratedMap", calculator.getNewAtom());
  theSub.setKeyValue("NewtonMap", calculator.getNewAtom());
  theSub.setKeyValue("y", calculator.getNewAtom());
  theSub.setKeyValue("startingPoint", input[2]);
  theSub.setKeyValue("numIterations", input[3]);
  return output.assignStringParsed(
    "(NewtonMap{}{{a}} = DoubleValue( (iteratedMap =x- f/ Differentiate{}(x, f); x ={{a}}; iteratedMap )_3); "
    "y_{0} = startingPoint;"
    "y_{{a}} = NewtonMap{}(y_{a- 1});"
    "y_{numIterations})_4",
    &theSub,
    calculator
  );
}

bool CalculatorFunctions::innerValueOfModularExpression(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerValueOfModularExpression");
  if (input.size() != 2) {
    return false;
  }
  ElementZmodP candidateZmodP;
  if (input[1].isOfType(&candidateZmodP)) {
    return output.assignValue(candidateZmodP.value, calculator);
  }
  Polynomial<ElementZmodP> candidatePolynomial;
  if (input[1].isOfType(&candidatePolynomial)) {
    Polynomial<Rational> rationalConversion;
    for (int i = 0; i < candidatePolynomial.size(); i ++) {
      rationalConversion.addMonomial(candidatePolynomial.monomials[i], candidatePolynomial.coefficients[i].value);
    }
    return output.assignValueWithContext(rationalConversion, input[1].getContext(), calculator);
  }

  PolynomialModuloPolynomial<ElementZmodP> quotientElement;
  if (input[1].isOfType(&quotientElement)) {
    return output.assignValueWithContext(quotientElement.value, input[1].getContext(), calculator);
  }
  return false;
}

bool CalculatorFunctions::innerElementEllipticCurveNormalForm(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerElementEllipticCurveNormalForm");
  if (input.size() != 4) {
    return calculator << "Elliptic curve expects 3 arguments (curve, generator letter, baseX and baseY) ";
  }
  const Expression& xDefE = input[2];
  const Expression& yDefE = input[3];
  //  HashedList<Expression> xEcandidates, yEcandidates;
  //  if (!xDefE.getFreeVariables(xEcandidates, false))
  //    return calculator << "Failed to get free variables from " << xDefE.toString();
  //  if (!yDefE.getFreeVariables(yEcandidates, false))
  //    return calculator << "Failed to get free variables from " << yDefE.toString();
  //  if (xEcandidates.size != 1 || yEcandidates.size != 1)
  //    return calculator << "Couldn't get single free variable from " << xEcandidates.toStringCommaDelimited()
  //    << " and/or " << yEcandidates.toStringCommaDelimited();
  //  if (CalculatorFunctions::innerEqualityToArithmeticExpression())
  if (!xDefE.startsWith(calculator.opDefine(), 3)) {
    return calculator << "Failed to extract variable form " << xDefE.toString();
  }
  if (!yDefE.startsWith(calculator.opDefine(), 3)) {
    return calculator << "Failed to extract variable form " << yDefE.toString();
  }
  ElementEllipticCurve<ElementZmodP> eltZmodP;
  ElementEllipticCurve<Rational> eltRational;
  eltZmodP.flagInfinity = false;
  eltRational.flagInfinity = false;
  bool isRational = true;
  bool isElementZmodP = true;
  if (!xDefE[2].isOfType(&eltRational.xCoordinate)) {
    isRational = false;
  }
  if (!yDefE[2].isOfType(&eltRational.yCoordinate)) {
    isRational = false;
  }
  if (!xDefE[2].isOfType(&eltZmodP.xCoordinate)) {
    isElementZmodP = false;
  }
  if (!yDefE[2].isOfType(&eltZmodP.yCoordinate)) {
    isElementZmodP = false;
  }
  if (!isRational && !isElementZmodP) {
    return calculator << "Could not extract rational or element of z mod p from "
    << xDefE[2].toString() << ", " << yDefE[2].toString();
  }
  if (isElementZmodP) {
    if (eltZmodP.xCoordinate.modulus != eltZmodP.yCoordinate.modulus) {
      return calculator << "The two base coordinates have different moduli. ";
    }
  }
  Expression theCurveE;
  if (!CalculatorFunctions::functionEqualityToArithmeticExpression(calculator, input[1], theCurveE)) {
    return calculator << "Could not get arithmetic expression from: " << input[1].toString()
    << ". I was expecting a cubic equality.";
  }
  Expression thePolyE;
  Polynomial<Rational> thePoly;
  if (!CalculatorConversions::functionPolynomial<Rational>(
    calculator, theCurveE, thePolyE
  )) {
    return calculator << "Could not get polynomial from " << theCurveE.toString();
  }
  if (!thePolyE.isOfType(&thePoly)) {
    return calculator << "Could not convert to polynomial: " << thePolyE.toString();
  }
  ExpressionContext curveContext = thePolyE.getContext();
  if (curveContext.numberOfVariables() != 2) {
    return calculator << "Expected 2 context variables in "
    << theCurveE.toString() << ", got context: "
    << curveContext.toString();
  }
  MonomialP leadingMonomial;
  List<MonomialP>::Comparator monomialOrder(MonomialP::greaterThan_totalDegree_rightSmallerWins);
  thePoly.getIndexLeadingMonomial(&leadingMonomial, nullptr, &monomialOrder);
  int indexX = 0;
  int indexY = 1;
  if (leadingMonomial[indexX] != 3) {
    MathRoutines::swap(indexX, indexY);
  }
  Expression xE = xDefE[1];
  Expression yE = yDefE[1];
  if (curveContext.getVariable(indexX) != xE) {
    MathRoutines::swap(xE, yE);
  }
  if (curveContext.getVariable(indexY) != yE) {
    return calculator << "Curve variable "
    << curveContext.getVariable(1).toString()
    << " not equal to " << yE.toString();
  }
  calculator << "Created elliptic curve " << thePolyE.toString()
  << " = 0. The variables are assumed to be: x = " << xE.toString() << ", y = " << yE.toString();
  if (thePoly.size() > 4) {
    return calculator << "Elliptic curve allowed to have max 4 terms, yours has: " << thePoly.size();
  }
  MonomialP xCubed, xLinear, ySquared;
  xCubed.makeEi(indexX, 3);
  xLinear.makeEi(indexX, 1);
  ySquared.makeEi(indexY, 2);
  Rational coefficientY = thePoly.getCoefficientOf(ySquared);
  Rational coefficientXcubed = - thePoly.getCoefficientOf(xCubed);
  if (coefficientY == 0) {
    return calculator << "Did not find square term in your curve.";
  }
  if (coefficientXcubed == 0) {
    return calculator << "Did not find cube term in your curve.";
  }
  Rational coefficientXlinear = - thePoly.getCoefficientOf(xLinear);
  Rational constCoefficient = - thePoly.getConstantTerm();
  EllipticCurveWeierstrassNormalForm& curveConstants = isRational ? eltRational.owner : eltZmodP.owner;
  if (
    !coefficientXlinear.isInteger(&curveConstants.linearCoefficient) ||
    !constCoefficient.isInteger(&curveConstants.constantTerm)
  ) {
    return calculator << "At the moment only integer elliptic curve coefficients are supported. Your coefficients were: "
    << coefficientXlinear << ", " << constCoefficient << ". ";
  }
  int numNonZeroGoodCoeffs = 2;
  if (coefficientXlinear != 0) {
    numNonZeroGoodCoeffs ++;
  }
  if (constCoefficient != 0) {
    numNonZeroGoodCoeffs ++;
  }
  if (numNonZeroGoodCoeffs != thePoly.size()) {
    return calculator << "It appears your curve: " << theCurveE.toString()
    << " is not of the form y^2 = x^3 + ax + b. ";
  }
  ExpressionContext theContext(calculator);
  theContext.addVariable(xE);
  theContext.addVariable(yE);
  if (isRational) {
    return output.assignValueWithContext(eltRational, theContext, calculator);
  }
  if (isElementZmodP) {
    return output.assignValueWithContext(eltZmodP, theContext, calculator);
  }
  return false;
}

bool CalculatorFunctions::innerPrecomputeSemisimpleLieAlgebraStructure(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPrecomputeSemisimpleLieAlgebraStructure");
  if (!global.theResponse.monitoringAllowed()) {
    global.theResponse.initiate("Triggered by innerPrecomputeSemisimpleLieAlgebraStructure.");
  }
  (void) input;
  List<DynkinType> theTypes;
  DynkinType::getPrecomputedDynkinTypes(theTypes);
  ProgressReport theReport;
  std::stringstream out;
  int lastIndexPlusOne = theTypes.size;
  //lastIndexPlusOne = 1;
  out << "Generated structure constants, "
  << "root subalgebras and sl(2) subalgebras for the following. ";
  for (int i = 0; i < lastIndexPlusOne; i ++) {
    std::stringstream reportStream;
    reportStream << "Computing structure of subalgebra "
    << theTypes[i].toString() << " (" << i + 1 << " out of " << theTypes.size << ").";
    theReport.report(reportStream.str());
    SemisimpleLieAlgebra theAlgebra;
    theAlgebra.theWeyl.makeFromDynkinType(theTypes[i]);
    theAlgebra.computeChevalleyConstants();
    theAlgebra.toHTMLCalculator(true, true, false);
    SltwoSubalgebras theSl2s(theAlgebra);
    theSl2s.theRootSAs.flagPrintParabolicPseudoParabolicInfo = true;
    theAlgebra.FindSl2Subalgebras(theAlgebra, theSl2s);
    theSl2s.toHTML();
    if ((false)) {
      if (theTypes[i].hasPrecomputedSubalgebras()) {
        SemisimpleSubalgebras theSubalgebras;
        MapReferences<DynkinType, SemisimpleLieAlgebra> subalgebrasContainer;
        ListReferences<SltwoSubalgebras> sl2Conainer;
        if (!theSubalgebras.computeStructureWriteFiles(
          theAlgebra,
          calculator.theObjectContainer.theAlgebraicClosure,
          subalgebrasContainer,
          sl2Conainer,
          nullptr,
          false,
          true,
          false,
          true,
          true,
          false,
          true
        )) {
          out << "Failed to compute " << theTypes[i].toString();
        }
      }
    }
    out << theTypes[i].toString();
    if (i != theTypes.size - 1) {
      out << ", ";
    }
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerShowKnownObjectIds(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerShowKnownObjectIds");
  (void) input;
  return output.assignValue(ASNObject::toStringAllRecognizedObjectIds(), calculator);
}

bool CalculatorFunctions::innerTestASN1Decode(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestASN1Decode");
  if (input.size() != 2) {
    return false;
  }
  std::string data;
  if (!input[1].isOfType(&data)) {
    return false;
  }
  AbstractSyntaxNotationOneSubsetDecoder theDecoder;
  List<unsigned char> dataList;
  dataList = data;
  std::stringstream commentsOnError;
  std::stringstream out;
  ASNElement result;
  if (!theDecoder.decode(dataList, 0, result, &commentsOnError)) {
    out << "Failed to decode.<br>" << commentsOnError.str();
  } else {
    List<unsigned char> recoded;
    result.writeBytesUpdatePromisedLength(recoded);
    std::string originalHex = Crypto::convertStringToHex(data, 0, false);
    std::string recodedHex = Crypto::convertListUnsignedCharsToHex(recoded);
    out << StringRoutines::Differ::differenceHTMLStatic(originalHex, recodedHex, "original", "recoded");
    out << theDecoder.toStringAnnotateBinary();
  }
  return output.assignValue(out.str(), calculator);
}

std::string StringRoutines::convertStringToCalculatorDisplay(
  const std::string& input
) {
  std::string converted = StringRoutines::convertStringToHexIfNonReadable(input, 100, true);
  if (converted == input) {
    return converted;
  }
  std::stringstream out;
  out << "convertHexToString{}\"" << converted << "\"";
  out << "<br>\""
  << StringRoutines::convertStringToJavascriptString(input)
  << "\"";
  return out.str();
}

std::string StringRoutines::convertByteToHex(unsigned char byte) {
  std::stringstream out;
  out << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned>(byte);
  return out.str();
}

std::string StringRoutines::convertStringToHexIfNonReadable(
  const std::string& input, int lineWidthZeroForNone, bool useHTML
) {
  bool foundBad = false;
  for (unsigned i = 0; i < input.size(); i ++) {
    // All characters smaller than \t = 9
    // and all characters larger than 127
    // are considered non-standard.
    if (static_cast<unsigned>(input[i]) < 9 || static_cast<unsigned>(input[i]) > 127) {
      foundBad = true;
      break;
    }
  }
  if (!foundBad) {
    return input;
  }
  return Crypto::convertStringToHex(input, lineWidthZeroForNone, useHTML);
}
