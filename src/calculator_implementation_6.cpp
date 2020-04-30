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

bool CalculatorHTML::Test::ComputeTotalFiles() {
  MacroRegisterFunctionWithName("CalculatorHTML::Test::ComputeTotalFiles");
  std::stringstream commentsOnFailure;
  if (!FileOperations::GetFolderFileNamesVirtual(
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
  out << "<a href='" << global.DisplayNameExecutableAppNoCache
  << "#"
  << request.toString()
  << "'>"
  << theFileName
  << "</a>";
  return  out.str();
}

std::string CalculatorHTML::Test::OneProblemTest::ToStringHTMLTableRow(int rowIndex) {
  MacroRegisterFunctionWithName("CalculatorHTML::Test::OneProblemTest::ToStringHTMLTableRow");
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

std::string CalculatorHTML::Test::ToHTMLDebug() {
  std::stringstream out;
  out << "File names all: " << this->fileNamesAll.toStringCommaDelimited() << "<br>";
  out << "Extensions all: " << this->fileExtensionsAll.toStringCommaDelimited() << "<br>";
  out << "File names extracted: " << this->fileNames.toStringCommaDelimited();
  return out.str();
}

std::string CalculatorHTML::Test::ToHTMLBuiltIn() {
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
    std::string nextRow = this->results[i].ToStringHTMLTableRow(
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

bool CalculatorHTML::Test::OneProblemTest::Run() {
  CalculatorHTML theProblem;
  std::stringstream commentsOnFailure;
  std::stringstream randomSeedStringStarting;
  randomSeedStringStarting << this->randomSeed;
  theProblem.fileName = this->fileName;
  this->flagSuccess = false;
  StateMaintainer<MapList<std::string, std::string, MathRoutines::HashString> >
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
  global.SetWebInpuT(WebAPI::problem::fileName, theProblem.fileName);
  global.SetWebInpuT(WebAPI::problem::randomSeed, randomSeedStream.str());
  this->flagSuccess = true;
  for (int j = 0; j < this->answers.size; j ++) {
    CalculatorHTML::Test::OneProblemTest::OneAnswer& current = this->answers[j];
    current.answerId = theProblem.theProblemData.theAnswers.theValues[j].answerId;
    current.answerIdWebAPI = WebAPI::problem::calculatorAnswerPrefix + current.answerId;
    global.SetWebInpuT(current.answerIdWebAPI, "1");
    current.builtInAnswerAPICall = WebAPIResponse::GetAnswerOnGiveUp(
      randomSeedStream.str(),
      &current.builtInAnswer,
      &current.flagBuiltInGenerated,
      false
    );
    global.webArguments.RemoveKey(current.answerIdWebAPI);
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
    global.SetWebInpuT(current.answerIdWebAPI, current.builtInAnswerEncoded);
    current.builtInAnswerReply = WebAPIResponse::SubmitAnswersJSON(
      randomSeedStream.str(), &current.flagBuiltInWorks, false
    );
    global.webArguments.RemoveKey(current.answerIdWebAPI);
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

std::string CalculatorHTML::Test::ToStringSummary() {
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
    if (!tester.BuiltIn(inputFirstFileIndex, inputFilesToInterpret, randomSeeds[i])) {
      if (comments != nullptr) {
        *comments << "Failed run " << i + 1 << " out of " << numberOfRepetitions << ". "
        << tester.ToHTMLBuiltIn();
        *comments << "Failed run " << i + 1 << " out of " << numberOfRepetitions << ". "
        << tester.ToStringSummary();
      }
      return false;
    }
    if (i == numberOfRepetitions - 1 && comments != nullptr) {
      *comments << numberOfRepetitions
      << " consecutive successfull tests of all involved problems. "
      << "The tests were carried out with starting random seeds: "
      << randomSeeds.toStringCommaDelimited()
      << "<br>"
      << tester.ToHTMLBuiltIn();
    }
  }
  return true;
}

bool CalculatorHTML::Test::BuiltIn(
  int inputFirstFileIndex,
  int inputFilesToInterpret,
  int inputRandomSeed
) {
  MacroRegisterFunctionWithName("CalculatorHTML::Test::BuiltIn");
  this->firstFileIndex = inputFirstFileIndex;
  this->filesToInterpret = inputFilesToInterpret;
  this->randomSeed = inputRandomSeed;

  ProgressReport theReport;
  if (!this->ComputeTotalFiles()) {
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
    if (!currentTest.Run()) {
      result = false;
    }
    if (!currentTest.flagSuccess) {
      badSoFar ++;
      if (global.flagRunningConsoleTest) {
        global << Logger::red << "Failure @ index: " << i << ". "
        << "Elapsed ms: " << global.GetElapsedMilliseconds() << ". "
        << Logger::endL;        
      }
    } else {
      if (global.flagRunningConsoleTest) {
        global << Logger::green << "Success @ index: " << i << ". "
        << "Elapsed ms: " << global.GetElapsedMilliseconds() << ". "
        << Logger::endL;
      }
    }
  }
  this->errorComments += commentsOnFailure.str();
  return result;
}

bool TopicElementParser::Test::all() {
  std::stringstream comments;
  TopicElementParser::Test::DefaultTopicListsOKCrashOnFailure();
  TopicElementParser::Test::DefaultPdfsOKCrashOnFailure();
  return true;
}

bool TopicElementParser::Test::DefaultTopicListsOKCrashOnFailure() {
  TopicElementParser::Test tester;
  if (!tester.DefaultTopicListsOK()) {
    global.fatal << "Topic list tests failed. " << tester.comments.str() << global.fatal;
  }
  return false;
}

bool TopicElementParser::Test::DefaultPdfsOKCrashOnFailure() {
  TopicElementParser::Test tester;
  if (!tester.DefaultPdfsOK()) {
    global.fatal << "Default pdfs are broken. " << tester.comments.str() << global.fatal;
  }
  return false;
}

bool TopicElementParser::Test::DefaultPdfsOK() {
 MacroRegisterFunctionWithName("TopicElementParser::Test::DefaultPdfsOK");
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
    if (!owner.topics.CheckTopicPdfs(&this->comments)) {
      this->comments << "Topic pdf check failed. ";
      return false;
    }
  }
  return true;
}

bool TopicElementParser::Test::DefaultTopicListsOK() {
  MacroRegisterFunctionWithName("TopicElementParser::Test::DefaultTopicListsOK");
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
    if (!owner.topics.CheckNoErrors(&this->comments)) {
      this->comments << "Errors in topic list: "
      << owner.topicListFileName << ". ";
      return false;
    }
    if (!owner.topics.CheckProblemsOpen(&this->comments)) {
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
  CalculatorHTML::Test::BuiltInCrashOnFailure();
  return true;
}

bool CalculatorHTML::Test::BuiltInCrashOnFailure() {
  std::stringstream comments;
  if (!CalculatorHTML::Test::BuiltInMultiple(0, 0, 0, 3, &comments)) {
    global.fatal << "Built-in problem tests failed. "
    << comments.str() << global.fatal;
  }
  return true;
}

bool CalculatorFunctions::innerTestTopicListProblems(
  Calculator& theCommands,
  const Expression& input,
  Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestTopicListProblems");
  if (!global.UserDefaultHasAdminRights()) {
    return theCommands << "Topic list tests available to logged-in admins only. ";
  }
  (void) input;
  TopicElementParser::Test tester;
  tester.DefaultTopicListsOK();
  return output.assignValue(tester.comments, theCommands);
}

bool CalculatorFunctions::innerTestProblemInterpretation(
  Calculator& theCommands,
  const Expression& input,
  Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestProblemInterpretation");
  if (!global.UserDefaultHasAdminRights()) {
    return theCommands << "Automated tests available to logged-in admins only. ";
  }
  if (input.size() != 4) {
    return theCommands
    << "I expected two arguments: "
    << "1) index of first problem to test, where "
    << "0 = start at beginning, 1 = start at second problem, etc.; "
    << "2) number of problems to test (0 or less = test all); "
    << "3) starting random seed, set to 0 if you don't know what this is. "
    ;
  }
  if (global.theResponse.monitoringAllowed()) {
    global.theResponse.Initiate("Triggered by innerTestProblemInterpretation.");
  }
  int desiredNumberOfTests = 0;
  int firstFileIndex = 0;
  int randomSeed = 0;
  input[1].isSmallInteger(&firstFileIndex);
  input[2].isSmallInteger(&desiredNumberOfTests);
  input[3].isSmallInteger(&randomSeed);
  std::stringstream comments;
  CalculatorHTML::Test::BuiltInMultiple(firstFileIndex, desiredNumberOfTests, randomSeed, 3, &comments);
  return output.assignValue(comments.str(), theCommands);
}

bool CalculatorFunctions::innerIntervalClosedFromSequence(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntervalClosedFromSequence");
  if (input.size() != 2) {
    return false;
  }
  if (!input[1].isSequenceNElements(2)) {
    return false;
  }
  output = input[1];
  return output.setChildAtomValue(0, theCommands.opIntervalClosed());
}

bool CalculatorFunctions::innerIntervalOpenFromSequence(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntervalOpenFromSequence");
  if (input.size() != 2) {
    return false;
  }
  if (!input[1].isSequenceNElements(2)) {
    return false;
  }
  output = input[1];
  return output.setChildAtomValue(0, theCommands.opIntervalOpen());
}

bool CalculatorFunctions::innerIntervalRightClosedFromSequence(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntervalRightClosedFromSequence");
  if (input.size() != 2) {
    return false;
  }
  if (!input[1].isSequenceNElements()) {
    return false;
  }
  output = input[1];
  return output.setChildAtomValue(0, theCommands.opIntervalRightClosed());
}

bool CalculatorFunctions::innerIntervalLeftClosedFromSequence(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntervalLeftClosedFromSequence");
  if (input.size() != 2) {
    return false;
  }
  if (!input[1].isSequenceNElements()) {
    return false;
  }
  output = input[1];
  return output.setChildAtomValue(0, theCommands.opIntervalLeftClosed());
}

bool CalculatorFunctions::innerGetFirstSummandContaining(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGetFirstSummandContaining");
  if (!input.startsWithGivenOperation("GetFirstSummandContaining", 3)) {
    return false;
  }
  List<Expression> theSummands;
  theCommands.CollectOpands(input[1], theCommands.opPlus(), theSummands);
  for (int i = 0; i < theSummands.size; i ++) {
    if (theSummands[i].ContainsAsSubExpressionNoBuiltInTypes(input[2])) {
      output = theSummands[i];
      return true;
    }
  }
  return false;
}

bool CalculatorFunctions::innerGetSummand(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGetSummand");
  if (!input.startsWithGivenOperation("GetSummand", 3)) {
    return false;
  }
  const Expression& theExpression = input[1];
  if (theExpression.startsWith(theCommands.opPlus(), 3)) {
    int summandIndex = - 1;
    if (!input[2].isSmallInteger(&summandIndex)) {
      return false;
    }
    if (summandIndex < 0) {
      return false;
    }
    List<Expression> theSummands;
    List<Expression> theSums;
    theCommands.CollectOpands(theExpression, theCommands.opPlus(), theSummands);
    for (int i = 0; i < theSummands.size; i ++) {
      if (theSummands[i].ContainsAsSubExpressionNoBuiltInTypes(theCommands.opSum())) {
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
    output.reset(theCommands);
    output.addChildAtomOnTop("GetSummand");
    output.addChildOnTop(theSums[0]);
    Expression shiftE;
    shiftE.assignValue(theSummands.size, theCommands);
    return output.addChildOnTop(input[2] - shiftE);
  }
  List<Expression> theMultiplicands;
  theExpression.getMultiplicandsRecursive(theMultiplicands);
  Expression theSum = *theMultiplicands.lastObject();
  theMultiplicands.removeLastObject();
  Expression theCoeff;
  if (theMultiplicands.size > 0) {
    theCoeff.makeProduct(theCommands, theMultiplicands);
  } else {
    theCoeff.assignValue(1, theCommands);
  }
  if (!theSum.startsWith(theCommands.opSum(), 3)) {
    return false;
  }
  const Expression& theLimits = theSum[1];
  if (!theLimits.startsWith(theCommands.opLimitBoundary(), 3)) {
    return false;
  }
  const Expression& theBottomBoundary = theLimits[1];
  if (!theBottomBoundary.startsWith(theCommands.opDefine(), 3)) {
    return false;
  }
  Expression theSub = theBottomBoundary;
  Expression //oneE,
  valueToSubWith;
  //oneE.assignValue(1, theCommands);
  valueToSubWith = theBottomBoundary[2] + input[2];
  theSub.setChild(2, valueToSubWith);
  Expression theCommandSequence(theCommands);
  theCommandSequence.addChildAtomOnTop(theCommands.opEndStatement());
  theCommandSequence.addChildOnTop(theSub);
  theCommandSequence.addChildOnTop(theCoeff * theSum[2]);
  return output.MakeXOX(theCommands, theCommands.opUnderscore(), theCommandSequence, theCommands.ETwo());
}

bool CalculatorFunctions::innerPlotVectorField(Calculator& theCommands, const Expression& input, Expression& output) {
  return CalculatorFunctions::innerPlotDirectionOrVectorField(theCommands, input, output, false);
}

bool CalculatorFunctions::innerPlotDirectionField(Calculator& theCommands, const Expression& input, Expression& output) {
  return CalculatorFunctions::innerPlotDirectionOrVectorField(theCommands, input, output, true);
}

bool CalculatorFunctions::innerPlotDirectionOrVectorField(
  Calculator& theCommands, const Expression& input, Expression& output, bool vectorsAreNormalized
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotDirectionOrVectorField");
  (void) vectorsAreNormalized;
  if (input.size() < 5) {
    return output.makeError(
      "Vector fields take at least four arguments: the vector field, "
      "the low left corner, the upper right, and the "
      "number of segments in each direction. ",
      theCommands
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
  thePlotObj.colorRGB = static_cast<int>(HtmlRoutines::RedGreenBlue(0, 0, 255));
  DrawingVariables::GetColorIntFromColorString(thePlotObj.colorJS, thePlotObj.colorRGB);
  thePlotObj.lineWidth = 1;
  if (input.size() >= 8) {
    input[7].evaluatesToDouble(&thePlotObj.lineWidth);
  }
  Vector<double> lowLeft, upRight;
  if (!theCommands.GetVectorDoubles(input[2], lowLeft, 2)) {
    return theCommands << "Failed to low left corner from: " << input[2].toString();
  }
  if (!theCommands.GetVectorDoubles(input[3], upRight, 2)) {
    return theCommands << "Failed to up right corner from: " << input[3].toString();
  }
  thePlotObj.yHigh = upRight[1];
  thePlotObj.yLow = lowLeft[1];
  thePlotObj.xHigh = upRight[0];
  thePlotObj.xLow = lowLeft[0];
  List<std::string> lowLeftStrings, upRightStrings;
  lowLeft.ToListStringsBasicType(lowLeftStrings);
  upRight.ToListStringsBasicType(upRightStrings);
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
    if (CalculatorFunctions::functionMakeJavascriptExpression(theCommands, input[5], jsConverterE)) {
      thePlotObj.defaultLengthJS = jsConverterE.toString();
    } else {
      return theCommands << "Failed to extract javascript from " << input[5].toString();
    }
  }
  if (CalculatorFunctions::functionMakeJavascriptExpression(theCommands, thePlotObj.manifoldImmersion, jsConverterE)) {
    thePlotObj.manifoldImmersionJS = jsConverterE.toString();
    thePlotObj.manifoldImmersion.HasInputBoxVariables(&thePlot.boxesThatUpdateMe);
  } else {
    return theCommands << "Failed to extract javascript from " << input[1].toString();
  }
  thePlotObj.manifoldImmersion.GetFreeVariables(thePlotObj.variablesInPlay, true);
  Expression xE, yE;
  xE.makeAtom("x", theCommands);
  yE.makeAtom("y", theCommands);
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
    !input[4].startsWith(theCommands.opIntervalOpen(), 3)
  ) {
    return theCommands
    << "<hr>Could not extract a list of elements for the "
    << "number of segments from: " << input[4].toString();
  }
  thePlotObj.numSegmenTsJS.setSize(2);
  for (int i = 0; i < 2; i ++) {
    if (!CalculatorFunctions::functionMakeJavascriptExpression(
      theCommands, input[4][i + 1], jsConverterE
    )) {
      return theCommands << "Failed to convert "
      << input[4][i + 1].toString() << " to javascript. ";
    }
    thePlotObj.numSegmenTsJS[i] = jsConverterE.toString();
  }
  thePlot.thePlots.addOnTop(thePlotObj);
  return output.assignValue(thePlot, theCommands);
}

bool CalculatorFunctions::innerJWTVerifyAgainstRSA256(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerJWTverifyAgainstRSA256");
  if (input.size() != 4) {
    return theCommands << "The JWT verify command expects 3 arguments: "
    << "string with the token in the usual format (\"a.b.c\"), "
    << "the modulus of the key and the exponent of the key. ";
  }
  std::string theTokenString;
  if (!input[1].isOfType(&theTokenString)) {
    return theCommands << "The first argument of " << input.toString() << " is not a string. ";
  }
  std::stringstream out;
  std::string theModBase64, theExpBase64;
  if (!input[2].isOfType(&theModBase64) || !input[3].isOfType(&theExpBase64)) {
    return theCommands << "Failed to convert the arguments "
    << input[2].toString()
    << " and " << input[3].toString()
    << " to base64 strings";
  }
  JSONWebToken theToken;
  LargeIntegerUnsigned theMod, theExp;
  if (!theToken.assignString(theTokenString, &out)) {
    return output.assignValue(out.str(), theCommands);
  }
  out << "Sucesfully extracted JWT token. <br>"
  << theToken.toString()
  << "<br>";
  if (
    !Crypto::convertBase64ToLargeUnsigned(theModBase64, theMod, &out) ||
    !Crypto::convertBase64ToLargeUnsigned(theExpBase64, theExp, &out)
  ) {
    return output.assignValue(out.str(), theCommands);
  }
  out << "<br>Successfully extracted modulus and exponent";
  theToken.verifyRSA256(theMod, theExp, &out, &out);
  return output.assignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerJWTVerifyAgainstKnownKeys(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerJWTverifyAgainstKnownKeys");
  if (!global.UserDefaultHasAdminRights()) {
    return theCommands << "This function is only available to logged-in admins. ";
  }
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  std::stringstream out;
  Crypto::VerifyJWTagainstKnownKeys(inputString, &out, &out);
  return output.assignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerHexToString(Calculator& theCommands, const Expression& input, Expression& output) {
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
  if (!Crypto::ConvertHexToString(inputString, result, &commentsOnFailure)) {
    return theCommands << "Failed to interpret your input as string. " << commentsOnFailure.str();
  }
  return output.assignValue(result, theCommands);
}

bool CalculatorFunctions::innerIntegerToHex(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegerToHex");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger theLI;
  if (!input[1].isInteger(&theLI)) {
    return false;
  }
  if (theLI < 0) {
    return theCommands << "I only convert positive integers to hex strings. ";
  }
  std::string result;
  if (!Crypto::convertLargeUnsignedToHexSignificantDigitsFirst(theLI.value, 0, result)) {
    return theCommands << "Failed to convert " << theLI
    << " to a hex string. ";
  }
  return output.assignValue(result, theCommands);
}

bool CalculatorFunctions::innerHexToInteger(Calculator& theCommands, const Expression& input, Expression& output) {
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
    return theCommands << "Failed to interpret " << inputString
    << " as a hex string ";
  }
  Rational resultRat = result;
  return output.assignValue(resultRat, theCommands);
}

bool CalculatorFunctions::innerTestJSON(
  Calculator& theCommands, const Expression& input, Expression& output
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
    return output.assignValue(out.str(), theCommands);
  }
  out << "Input:<br>" << inputString << "<hr>Output: <br>"
  << theData.toString(nullptr);
  return output.assignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerBase64ToHex(
  Calculator& theCommands, const Expression& input, Expression& output
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
  if (!Crypto::ConvertBase64ToString(inputString, bitStream, &theCommands.comments, &theCommands.comments)) {
    return false;
  }
  Crypto::convertStringToHex(bitStream, result, 0, false);
  return output.assignValue(result, theCommands);
}

bool CalculatorFunctions::innerGenerateRandomPrime(
  Calculator& theCommands, const Expression& input, Expression& output
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
    return theCommands << "Max number of bytes: "
    << maxNumberOfBytes << ", min number of bytes: "
    << minNumberOfBytes << ", you requested: " << numberOfBytes << ". ";
  }
  LargeIntegerUnsigned result;
  Crypto::Random::getRandomLargePrime(result, numberOfBytes);
  return output.assignValue(result, theCommands);
}

bool CalculatorFunctions::innerTestRSASign(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTestRSASign");
  if (input.size() != 4) {
    return theCommands << "RSA signature takes as input 3 arguments: message and two primes. ";
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
    return theCommands << "Failed to extract positive integers from the second and third argument. ";
  }
  if (
    !theKey.ComputeFromTwoPrimes(theKey.primeOne, theKey.primeTwo, true, &errorStream)
  ) {
    return theCommands << "Inputs do not appear to be prime. " << errorStream.str();
  }
  List<unsigned char> message, paddedMessage, signature;
  message = messageString;
  theKey.HashAndPadPKCS1(message, SignatureAlgorithmSpecification::HashAlgorithm::sha256, paddedMessage);
  out << "Temporary private key:<br>" << theKey.toString() << "<br>";
  theKey.SignBytesPadPKCS1(message, SignatureAlgorithmSpecification::HashAlgorithm::sha256, signature);
  out << "Message:<br>" << Crypto::convertListUnsignedCharsToHex(message);
  out << "<br>Padded message digest:<br>" << Crypto::convertListUnsignedCharsToHex(paddedMessage);
  out << "<br>Signature:<br>" << Crypto::convertListUnsignedCharsToHex(signature);
  ElementZmodP theElement, theOne;
  theElement.theModulus = theKey.thePublicKey.theModulus;
  theOne.makeOne(theElement.theModulus);
  Crypto::convertListUnsignedCharsToLargeUnsignedIntegerBigEndian(signature, theElement.theValue);
  out << "<br>Signature integer:<br>" << theElement.theValue.toString();
  MathRoutines::raiseToPower(theElement, theKey.thePublicKey.theExponent, theOne);
  out << "<br>Signature power e mod n [e = "
  << theKey.thePublicKey.theExponent << ", n = "
  << theKey.thePublicKey.theModulus << "]"
  << ":<br>" << theElement.theValue.toString();
  std::string theHex;
  Crypto::convertLargeUnsignedToHexSignificantDigitsFirst(theElement.theValue, 0, theHex);
  out << "<br>Converted to hex:<br>" << theHex;
  return output.assignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerRSAEncrypt(
  Calculator& theCommands, const Expression& input, Expression& output
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
    return theCommands << "Failed to extract three (large) integers from the arguments of "
    << input.toString();
  }
  if (theModulus < 0) {
    theModulus *= - 1;
  }
  if (theModulus == 0 || theExponent == 0) {
    return theCommands << "The modulus and exponent must be non-zero.";
  }
  if (theModulus == 1) {
    return theCommands << "Modulus 1 not allowed";
  }
  result = Crypto::RSAencrypt(theModulus.value, theExponent, theMessage);
  return output.assignValue(Rational(result), theCommands);
}

bool CalculatorFunctions::innerSendEmailWithMailGun(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSendEmailWithMailGun");
  if (!global.UserDefaultHasAdminRights()) {
    return theCommands << "Sending mail available to logged-in admins only. ";
  }
  std::stringstream out;
  if (global.flagDatabaseCompiled) {
    if (input.size() != 4) {
      return theCommands << "Send email requires three arguments. ";
    }
    EmailRoutines theEmail;
    if (
      !input[1].isOfType(&theEmail.toEmail) ||
      !input[2].isOfType(&theEmail.emailContent) ||
      !input[3].isOfType(&theEmail.subject)
    ) {
      return theCommands << "Arguments of " << input.toString()
      << "expected to be strings (enclose in \"\" please). ";
    }
    theEmail.sendEmailWithMailGun(&out, &out, &out);
  } else {
    (void) input;
    out << "Error: database not running. ";
  }
  return output.assignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerIsSquare(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsSquare");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger theLI;
  if (!input[1].isInteger(&theLI)) {
    return false;
  }
  if (theLI < 0) {
    return output.assignValue(0, theCommands);
  }
  if (theLI == 0) {
    return output.assignValue(1, theCommands);
  }
  List<int> theMults;
  List<LargeInteger> theFactors;
  if (!theLI.value.factor(theFactors, theMults, 0, 4, nullptr)) {
    return theCommands << "Failed to factor: " << theLI.toString() << " (may be too large?).";
  }
  int result = 1;
  for (int i = 0; i < theMults.size; i ++) {
    if ((theMults[i] % 2) != 0) {
      result = 0;
      break;
    }
  }
  return output.assignValue(result, theCommands);
}

bool CalculatorFunctions::innerIsSquareFree(
  Calculator& theCommands, const Expression& input, Expression& output
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
    theFactors, theMults, 0, 3, &theCommands.comments
  )) {
    return theCommands << "Failed to factor: "
    << theInteger.toString() << " (may be too large?).";
  }
  int result = 1;
  for (int i = 0; i < theMults.size; i ++) {
    if (theMults[i] > 1) {
      result = 0;
      break;
    }
  }
  return output.assignValue(result, theCommands);
}

bool CalculatorFunctions::innerIsSquareFreePolynomial(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsSquareFreePolynomial");
  if (input.size() != 2) {
    return false;
  }
  WithContext<Polynomial<Rational> > polynomial;
  if (!theCommands.Convert(
    input, CalculatorConversions::innerPolynomial<Rational>, polynomial
  )) {
    return false;
  }
  std::stringstream out;
  bool squareFree = polynomial.content.isSquareFree(Rational::one(), &out);
  return output.assignValue(Rational(int(squareFree)), theCommands);
}

bool CalculatorFunctions::innerIsPower(
  Calculator& theCommands, const Expression& input, Expression& output
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
    theFactors, theMults, 0, 3, &theCommands.comments
  )) {
    return theCommands << "Failed to factor: "
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
  return output.assignValue(result, theCommands);
}

bool CalculatorFunctions::innerFactorIntegeR(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFactorInteger");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctions::functionFactorInteger(theCommands, input[1], output);
}

bool CalculatorFunctions::functionFactorInteger(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::functionFactorInteger");
  LargeInteger integer;
  int upperBound = 30000;
  int opFactorInteger = theCommands.operations.getIndexNoFail("FactorInteger");

  if (input.startsWith(opFactorInteger) && input.size() == 3) {
    if (!input[1].isInteger(&integer)) {
      return false;
    }
    if (!input[2].IsIntegerFittingInInt(&upperBound)) {
      return theCommands << "Failed to extract integer from " << upperBound << ".";
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
    &theCommands.comments
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
    currentE.assignValue(Rational(primeFactors[i]), theCommands);
    for (int j = 0; j < multiplicities[i]; j ++) {
      result.addOnTop(currentE);
    }
  }
  if (integer < 0 && result.size > 0) {
    result[0] *= - 1;
  }
  if (complete) {
    return output.makeSequence(theCommands, &result);
  } else {
    Expression factorsSoFar, factorNext, numberLast;
    factorsSoFar.makeSequence(theCommands, &result);
    factorNext.reset(theCommands);
    factorNext.addChildAtomOnTop(opFactorInteger);
    numberLast.assignValue(Rational(primeFactors[primeFactors.size - 1]), theCommands);
    factorNext.addChildOnTop(numberLast);
    return output.MakeXOX(theCommands, theCommands.opUnion(), factorsSoFar, factorNext);
  }
}

bool CalculatorFunctions::innerFactorOutNumberContent(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFactorOutNumberContent");
  if (input.size() != 2) {
    return theCommands << "FactorOutNumberContent expects single argument. ";
  }
  LinearCombination<Expression, Rational> theV;
  if (!theCommands.functionCollectSummands(theCommands, input[1], theV)) {
    return theCommands << "Failed to extract summands from: " << input[1].toString();
  }
  if (theV.isEqualToZero()) {
    return output.assignValue(0, theCommands);
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
  left.assignValue(theCF, theCommands);
  right.makeSum(theCommands, theV);
  output = left * right;
  return true;
}

bool CalculatorFunctions::innerSubList(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSubList");
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isSequenceNElements()) {
    return false;
  }
  HashedList<Expression> boundVars;
  if (!input[2].getBoundVariables(boundVars)) {
    return theCommands << "Could not get bound variables from: " << input[2].toString();
  }
  if (input[2].isEqualToOne()) {
    output = input[1];
    return true;
  }
  if (boundVars.size == 0) {
    return output.makeSequence(theCommands, nullptr);
  }
  Expression theSubbed, toBeSubbed, subbedSimplified;
  toBeSubbed.reset(theCommands);
  toBeSubbed.addChildAtomOnTop(theCommands.opBind());
  toBeSubbed.addChildOnTop(boundVars[0]);
  List<Expression> theList;
  for (int i = 1; i < input[1].size(); i ++) {
    theSubbed = input[2];
    theSubbed.substituteRecursively(toBeSubbed, input[1][i]);
    if (!theCommands.EvaluateExpression(theCommands, theSubbed, subbedSimplified)) {
      return theCommands << "Failed to evaluate " << theSubbed.toString();
    }
    if (subbedSimplified.isEqualToOne()) {
      theList.addOnTop(input[1][i]);
    }
  }
  return output.makeSequence(theCommands, &theList);
}

bool CalculatorFunctions::innerApplyToList(Calculator& theCommands, const Expression& input, Expression& output) {
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
    result[i - 1].reset(theCommands);
    result[i - 1].addChildOnTop(theFun);
    result[i - 1].addChildOnTop(input[2][i]);
  }
  return output.makeSequence(theCommands, &result);
}

bool CalculatorFunctions::innerPolynomialDivisionQuotient(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerPolynomialDivisionQuotient");
  ExpressionContext theContext(theCommands);
  Vector<Polynomial<AlgebraicNumber> > polynomialsRational;
  if (!theCommands.getListPolynomialVariableLabelsLexicographic(input, polynomialsRational, theContext)) {
    return output.makeError("Failed to extract list of polynomials. ", theCommands);
  }
  GroebnerBasisComputation<AlgebraicNumber> computation;
  computation.thePolynomialOrder.monomialOrder = MonomialP::orderDefault();
  computation.flagStoreQuotients = true;
  for (int i = 1; i < polynomialsRational.size; i ++) {
    if (polynomialsRational[i].isEqualToZero()) {
      return output.makeError("Division by zero.", theCommands);
    }
    computation.addBasisElementNoReduction(polynomialsRational[i]);
  }
  Polynomial<AlgebraicNumber> outputRemainder;
  computation.remainderDivisionByBasis(polynomialsRational[0], outputRemainder, - 1);
  Expression currentE, thePolyE;
  List<Expression> theList;
  for (int i = 0; i < computation.theQuotients.size; i ++) {
    currentE.reset(theCommands);
    currentE.addChildAtomOnTop("MakeExpression");
    thePolyE.assignValueWithContext(computation.theQuotients[i], theContext, theCommands);
    currentE.addChildOnTop(thePolyE);
    theList.addOnTop(currentE);
  }
  if (theList.size == 1) {
    output = theList[0];
    return true;
  }
  return output.makeSequence(theCommands, &theList);
}

bool CalculatorFunctions::innerArccosAlgebraic(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerArccosAlgebraic");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argumentE = input[1];
  Rational theRat;
  if (argumentE.isRational(&theRat)) {
    if (theRat == 1) {
      return output.assignValue(0, theCommands);
    }
    if (theRat == 0) {
      output.makeAtom(theCommands.opPi(), theCommands);
      output /= 2;
      return true;
    }
    if (theRat == - 1) {
      return output.makeAtom(theCommands.opPi(), theCommands);
    }
    if (theRat == Rational(1, 2)) {
      output.makeAtom(theCommands.opPi(), theCommands);
      output /= 3;
      return true;
    }
    if (theRat == Rational(- 1, 2)) {
      output.makeAtom(theCommands.opPi(), theCommands);
      output *= 2;
      output /= 3;
      return true;
    }
  }
  AlgebraicNumber argument, candidate;
  if (argumentE.isOfType<AlgebraicNumber>(&argument)) {
    candidate.assignRationalQuadraticRadical(
      Rational(1, 2),
      theCommands.theObjectContainer.theAlgebraicClosure,
      nullptr
    );
    if (candidate == argument) {
      output.makeAtom(theCommands.opPi(), theCommands);
      output /= 4;
      return true;
    }
    candidate *= - 1;
    if (candidate ==argument) {
      output.makeAtom(theCommands.opPi(), theCommands);
      output /= 4;
      output *= 3;
      return true;
    }
    candidate.assignRationalQuadraticRadical(
      Rational(3, 4),
      theCommands.theObjectContainer.theAlgebraicClosure,
      nullptr
    );
    if (candidate == argument) {
      output.makeAtom(theCommands.opPi(), theCommands);
      output /= 6;
      return true;
    }
    candidate *= - 1;
    if (candidate == argument) {
      output.makeAtom(theCommands.opPi(), theCommands);
      output /= 6;
      output *= 5;
      return true;
    }
  }
  return false;
}

bool CalculatorFunctions::innerArcsinAlgebraic(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerArcsinAlgebraic");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argumentE = input[1];
  Rational theRat;
  if (argumentE.isRational(&theRat)) {
    if (theRat == 1) {
      output.makeAtom(theCommands.opPi(), theCommands);
      output /= 2;
      return true;
    }
    if (theRat == 0) {
      return output.assignValue(0, theCommands);
    }
    if (theRat == - 1) {
      output.makeAtom(theCommands.opPi(), theCommands);
      output /= - 2;
      return true;
    }
    if (theRat == Rational(1, 2)) {
      output.makeAtom(theCommands.opPi(), theCommands);
      output /= 6;
      return true;
    }
    if (theRat == Rational(- 1, 2)) {
      output.makeAtom(theCommands.opPi(), theCommands);
      output /= - 6;
      return true;
    }
  }
  AlgebraicNumber argument, candidate;
  if (argumentE.isOfType<AlgebraicNumber>(&argument)) {
    candidate.assignRationalQuadraticRadical(
      Rational(1, 2),
      theCommands.theObjectContainer.theAlgebraicClosure,
      nullptr
    );
    if (candidate == argument) {
      output.makeAtom(theCommands.opPi(), theCommands);
      output /= 4;
      return true;
    }
    candidate *= - 1;
    if (candidate == argument) {
      output.makeAtom(theCommands.opPi(), theCommands);
      output /= - 4;
      return true;
    }
    candidate.assignRationalQuadraticRadical(
      Rational(3, 4),
      theCommands.theObjectContainer.theAlgebraicClosure,
      nullptr
    );
    if (candidate == argument) {
      output.makeAtom(theCommands.opPi(), theCommands);
      output /= 3;
      return true;
    }
    candidate *= - 1;
    if (candidate == argument) {
      output.makeAtom(theCommands.opPi(), theCommands);
      output /= - 3;
      return true;
    }
  }
  return false;
}

bool CalculatorFunctions::innerMatchesPattern(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerMatchesPattern");
  if (input.size() != 3) {
    return false;
  }
  MapList<Expression, Expression> matchedExpressions;
  if (!theCommands.ExpressionMatchesPattern(input[2], input[1], matchedExpressions, nullptr)) {
    return output.assignValue(0, theCommands);
  }
  Expression commandList;
  commandList.reset(theCommands);
  commandList.addChildAtomOnTop(theCommands.opEndStatement());
  for (int i = 0; i < matchedExpressions.size(); i ++) {
    Expression currentCommand;
    if (!matchedExpressions.theKeys[i].startsWith(theCommands.opBind(), 2)) {
      std::stringstream errorStream;
      errorStream << "Bound variable " << matchedExpressions.theKeys[i].toString()
      << " does not start with the bind atom. ";
      return output.makeError(errorStream.str(), theCommands);
    }
    currentCommand.MakeXOX(
      theCommands,
      theCommands.opDefine(),
      matchedExpressions.theKeys[i][1],
      matchedExpressions.theValues[i]
    );
    commandList.addChildOnTop(currentCommand);
  }
  output.reset(theCommands);
  output.addChildAtomOnTop(theCommands.opCommandEnclosure());
  return output.addChildOnTop(commandList);
}

bool CalculatorFunctions::innerDegreesToRadians(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDegreesToRadians");
  if (!input.startsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  if (!input[2].isOperationGiven("\\circ")) {
    return false;
  }
  Expression piE;
  piE.makeAtom(theCommands.opPi(), theCommands);
  output = input[1] * piE;
  output /= 180;
  return true;
}

bool CalculatorFunctions::innerLessThanOrEqualTo(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerLessThanOrEqualTo");
  if (input.size() != 3) {
    return false;
  }
  Expression result(theCommands);
  result.addChildAtomOnTop("\\geq");
  result.addChildOnTop(input[2]);
  result.addChildOnTop(input[1]);
  output = result;
  return true;
}

bool CalculatorFunctions::innerGreaterThanOrEqualTo(
  Calculator& theCommands, const Expression& input, Expression& output
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
      return output.assignValue(1, theCommands);
    }
    return output.assignValue(0, theCommands);
  }
  double leftD, rightD;
  if (left.evaluatesToDouble(&leftD) && right.evaluatesToDouble(&rightD)) {
    if (leftD >= rightD) {
      return output.assignValue(1, theCommands);
    }
    return output.assignValue(0, theCommands);
  }
  return false;
}

bool CalculatorFunctions::innerGreaterThan(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGreaterThan");
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression& left = input[1];
  const Expression& right = input[2];
  Rational leftRat, rightRat;
  if (left.isRational(&leftRat) && right.isRational(&rightRat)) {
    if (leftRat > rightRat) {
      return output.assignValue(1, theCommands);
    }
    return output.assignValue(0, theCommands);
  }
  double leftD, rightD;
  if (left.evaluatesToDouble(&leftD) && right.evaluatesToDouble(&rightD)) {
    if (leftD > rightD) {
      return output.assignValue(1, theCommands);
    }
    return output.assignValue(0, theCommands);
  }
  return false;
}

bool CalculatorFunctions::innerLessThan(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerLessThan");
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression swappedE(theCommands);
  swappedE.addChildOnTop(input[0]);
  swappedE.addChildOnTop(input[2]);
  swappedE.addChildOnTop(input[1]);
  return CalculatorFunctions::innerGreaterThan(theCommands, swappedE, output);
}

bool CalculatorFunctions::innerCollectOpands(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCollectOpands");
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    return false;
  }
  List<Expression> theList;
  theCommands.AppendOpandsReturnTrueIfOrderNonCanonical(input[2], theList, input[1].theData);
  return output.makeSequence(theCommands, &theList);
}

bool CalculatorFunctions::innerCollectMultiplicands(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerCollectSummands");
  if (input.size() != 2) {
    return false;
  }
  List<Expression> theList;
  theCommands.AppendOpandsReturnTrueIfOrderNonCanonical(input[1], theList, theCommands.opTimes());
  return output.makeSequence(theCommands, &theList);
}

bool CalculatorFunctions::innerCollectSummands(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCollectSummands");
  if (input.size() != 2) {
    return false;
  }
  List<Expression> theList;
  theCommands.AppendSummandsReturnTrueIfOrderNonCanonical(input[1], theList);
  return output.makeSequence(theCommands, &theList);
}

bool CalculatorFunctions::LeftIntervalGreaterThanRight(const Expression& left, const Expression& right) {
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
  const Expression& inftyE = right.owner->EInfinity();
  const Expression& mInftyE = right.owner->EMInfinity();
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
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntersectIntervals");
  if (!input.startsWith(theCommands.opIntersection(), 3)) {
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
      rightE.startsWith(theCommands.opSequence()) ||
      rightE.startsWith(theCommands.opIntervalRightClosed())
    ) {
      leftIsClosed = false;
    }
  }
  if (left1 - right1 == 0.0) {
    leftFinal = leftE[1];
    leftResult = left1;
    if (
      leftE .startsWith(theCommands.opSequence()) ||
      leftE .startsWith(theCommands.opIntervalRightClosed())||
      rightE.startsWith(theCommands.opSequence()) ||
      rightE.startsWith(theCommands.opIntervalRightClosed())
    ) {
      leftIsClosed = false;
    }
  }
  if (left1 > right1) {
    leftFinal = leftE[1];
    leftResult = left1;
    if (
      leftE.startsWith(theCommands.opSequence()) ||
      leftE.startsWith(theCommands.opIntervalRightClosed())
    ) {
      leftIsClosed = false;
    }
  }
///////////////////////////
  if (left2 > right2) {
    rightFinal = rightE[2];
    rightResult = right2;
    if (
      rightE.startsWith(theCommands.opSequence()) ||
      rightE.startsWith(theCommands.opIntervalLeftClosed())
    ) {
      rightIsClosed = false;
    }
  }
  if (left2 - right2 == 0.0) {
    rightFinal = rightE[2];
    rightResult = right2;
    if (
      leftE .startsWith(theCommands.opSequence()) ||
      leftE .startsWith(theCommands.opIntervalLeftClosed())||
      rightE.startsWith(theCommands.opSequence()) ||
      rightE.startsWith(theCommands.opIntervalLeftClosed())
    ) {
      rightIsClosed = false;
    }
  }
  if (left2 < right2) {
    rightFinal = leftE[2];
    rightResult = left2;
    if (
      leftE.startsWith(theCommands.opSequence()) ||
      leftE.startsWith(theCommands.opIntervalLeftClosed())
    ) {
      rightIsClosed = false;
    }
  }
  if (leftResult > rightResult) {
    return output.makeAtom("\\emptyset", theCommands);
  }
  if (leftIsClosed && rightIsClosed) {
    return output.MakeXOX(theCommands, theCommands.opIntervalClosed(), leftFinal, rightFinal);
  }
  if (!leftIsClosed && rightIsClosed) {
    return output.MakeXOX(theCommands, theCommands.opIntervalRightClosed(), leftFinal, rightFinal);
  }
  if (leftIsClosed && !rightIsClosed) {
    return output.MakeXOX(theCommands, theCommands.opIntervalLeftClosed(), leftFinal, rightFinal);
  }
  if (!leftIsClosed && !rightIsClosed) {
    return output.MakeXOX(theCommands, theCommands.opSequence(), leftFinal, rightFinal);
  }
  return false;
}

bool CalculatorFunctions::innerUnionUnionIntervals(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerUnionUnionIntervals");
  if (input.size() != 3) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightComposite = input[2];
  if (!rightComposite.startsWith(theCommands.opUnion(), 3)) {
    return false;
  }
  const Expression& rightE = input[2][1];
  Expression theMiddleUnion, middleUnionReduced;
  theMiddleUnion.MakeXOX(theCommands, theCommands.opUnion(), leftE, rightE);
  if (!CalculatorFunctions::innerUnionIntervals(theCommands, theMiddleUnion, middleUnionReduced)) {
    return false;
  }
  return output.MakeXOX(theCommands, theCommands.opUnion(), middleUnionReduced, rightComposite[2]);
}

bool CalculatorFunctions::innerUnionIntervals(
  Calculator& theCommands, const Expression& input, Expression& output
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
    leftE.startsWith(theCommands.opIntervalOpen(), 3) &&
    rightE.startsWith(theCommands.opIntervalOpen(), 3)
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
      leftE.startsWith(theCommands.opIntervalClosed()) ||
      leftE.startsWith(theCommands.opIntervalLeftClosed())
    ) {
      leftIsClosed = true;
    }
  }
  if (left1 - right1 == 0.0) {
    leftFinal = leftE[1];
    if (
      leftE .startsWith(theCommands.opIntervalClosed()) ||
      leftE .startsWith(theCommands.opIntervalLeftClosed())||
      rightE.startsWith(theCommands.opIntervalClosed()) ||
      rightE.startsWith(theCommands.opIntervalLeftClosed())
    ) {
      leftIsClosed = true;
    }
  }
  if (left1 > right1) {
    leftFinal = rightE[1];
    if (
      rightE.startsWith(theCommands.opIntervalClosed()) ||
      rightE.startsWith(theCommands.opIntervalLeftClosed())
    ) {
      leftIsClosed = true;
    }
  }
///////////////////////
  if (left2 > right2) {
    rightFinal = leftE[2];
    if (
      leftE.startsWith(theCommands.opIntervalClosed()) ||
      leftE.startsWith(theCommands.opIntervalRightClosed())
    ) {
      rightIsClosed = true;
    }
  }
  if (left2 - right2 == 0.0) {
    rightFinal = rightE[2];
    if (
      leftE .startsWith(theCommands.opIntervalClosed()) ||
      leftE .startsWith(theCommands.opIntervalRightClosed())||
      rightE.startsWith(theCommands.opIntervalClosed()) ||
      rightE.startsWith(theCommands.opIntervalRightClosed())
    ) {
      rightIsClosed = true;
    }
  }
  if (left2 < right2) {
    rightFinal = rightE[2];
    if (
      rightE.startsWith(theCommands.opIntervalClosed()) ||
      rightE.startsWith(theCommands.opIntervalRightClosed())
    ) {
      rightIsClosed = true;
    }
  }
  if (leftIsClosed && rightIsClosed) {
    return output.MakeXOX(theCommands, theCommands.opIntervalClosed(), leftFinal, rightFinal);
  }
  if (!leftIsClosed && rightIsClosed) {
    return output.MakeXOX(theCommands, theCommands.opIntervalRightClosed(), leftFinal, rightFinal);
  }
  if (leftIsClosed && !rightIsClosed) {
    return output.MakeXOX(theCommands, theCommands.opIntervalLeftClosed(), leftFinal, rightFinal);
  }
  if (!leftIsClosed && !rightIsClosed) {
    return output.MakeXOX(theCommands, theCommands.opIntervalOpen(), leftFinal, rightFinal);
  }
  return false;
}

bool CalculatorFunctions::innerNormalizeIntervals(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::outerNormalizeIntervals");
  if (input.size() != 2) {
    return false;
  }
  List<Expression> outputList;
  if (!theCommands.CollectOpands(input[1], theCommands.opUnion(), outputList)) {
    return false;
  }
  List<Expression>::Comparator order(CalculatorFunctions::LeftIntervalGreaterThanRight);
  outputList.quickSortAscending(&order);
  return output.MakeXOXOdotsOX(theCommands, theCommands.opUnion(), outputList);
}

bool CalculatorFunctions::innerUnionEmptySet(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerUnionEmptySet");
  if (!input.startsWith(theCommands.opUnion(), 3)) {
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

bool CalculatorFunctions::innerIntersectUnion(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntersectUnion");
  return theCommands.outerDistribute(theCommands, input, output, theCommands.opUnion(), theCommands.opIntersection(), false);
}

bool CalculatorFunctions::innerCompareIntervalsNumerically(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCompareIntervalsNumerically");
  if (input.size() != 4) {
    return theCommands << "Comparing intervals numerically takes 3 arguments: two unions and precision. ";
  }
  double precision = 0;
  if (!input[3].evaluatesToDouble(&precision)) {
    return theCommands << "Could not extract precision from the last argument.";
  }
  List<Expression> leftList, rightList;
  if (!theCommands.CollectOpands(input[1], theCommands.opUnion(), leftList)) {
    return false;
  }
  if (!theCommands.CollectOpands(input[2], theCommands.opUnion(), rightList)) {
    return false;
  }
  if (leftList.size != rightList.size) {
    return output.assignValue(0, theCommands);
  }
  List<Expression>::Comparator order(CalculatorFunctions::LeftIntervalGreaterThanRight);
  leftList.quickSortAscending(&order);
  rightList.quickSortAscending(&order);
  for (int i = 0; i < leftList.size; i ++) {
    if (leftList[i] == rightList[i]) {
      continue;
    }
    if ((!leftList[i].isIntervalRealLine() ) || (!rightList[i].isIntervalRealLine())) {
      return output.assignValue(0, theCommands);
    }
    if (leftList[i][0] != rightList[i][0]) {
      return output.assignValue(0, theCommands);
    }
    double left1 = 0, left2 = 0, right1 = 0, right2 = 0;
    if (
      !leftList[i][1].evaluatesToDouble(&left1) ||
      !rightList[i][1].evaluatesToDouble(&right1)
    ) {
      if (leftList[i][1] != rightList[i][1]) {
        return output.assignValue(0, theCommands);
      }
    }
    if (
      !rightList[i][2].evaluatesToDouble(&right2)||
      !leftList[i][2].evaluatesToDouble(&left2)
    ) {
      if (leftList[i][2] != rightList[i][2]) {
        return output.assignValue(0, theCommands);
      }
    }
    if (
      FloatingPoint::Abs(left1 - right1) > precision ||
      FloatingPoint::Abs(left2 - right2) > precision
    ) {
      return output.assignValue(0, theCommands);
    }
  }
  return output.assignValue(1, theCommands);

}

bool CalculatorFunctions::innerIntersectEmptySet(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntersectEmptySet");
  if (!input.startsWith(theCommands.opIntersection(), 3)) {
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
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsLinearOrConstantIn");
  if (input.size() < 3) {
    return false;
  }
  List<List<Expression> > theSummands;
  if (!theCommands.GetSumProductsExpressions(input[2], theSummands)) {
    return theCommands << "Failed to extract sum from "
    << input[2].toString();
  }
  for (int i = 0; i < theSummands.size; i ++) {
    bool found = false;
    for (int j = 0; j < theSummands[i].size; j ++) {
      if (theSummands[i][j] == input[1]) {
        if (found) {
          return output.assignValue(0, theCommands);
        }
        found = true;
      } else if (!theSummands[i][j].evaluatesToDouble()) {
        return output.assignValue(0, theCommands);
      }
    }
  }
  return output.assignValue(1, theCommands);
}

bool CalculatorFunctions::innerIsProductLinearOrConstTermsIn(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsProductLinearOrConstTermsIn");
  if (input.size() < 3) {
    return false;
  }
  List<Expression> theMultiplicands;
  if (!theCommands.CollectOpands(input[2], theCommands.opTimes(),theMultiplicands)) {
    return theCommands << "Could not extract multiplicands from: "
    << input[2].toString();
  }
  for (int k = 0; k < theMultiplicands.size; k ++) {
    List<List<Expression> > theSummands;
    if (!theCommands.GetSumProductsExpressions(theMultiplicands[k], theSummands)) {
      return theCommands << "Failed to extract sum from "
      << theMultiplicands[k].toString();
    }
    for (int i = 0; i < theSummands.size; i ++) {
      bool found = false;
      for (int j = 0; j < theSummands[i].size; j ++) {
        if (theSummands[i][j] == input[1]) {
          if (found) {
            return output.assignValue(0, theCommands);
          }
          found = true;
        } else if (!theSummands[i][j].evaluatesToDouble()) {
          return output.assignValue(0, theCommands);
        }
      }
    }
  }
  return output.assignValue(1, theCommands);
}

bool CalculatorFunctions::innerOrIdentical(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerOrIdentical");
  (void) theCommands;
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
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSineOfAngleSumToTrig");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (!argument.startsWith(theCommands.opPlus(), 3)) {
    return false;
  }
  Expression sinA, sinB, cosA, cosB;
  sinA.MakeOX(theCommands, theCommands.opSin(), argument[1]);
  sinB.MakeOX(theCommands, theCommands.opSin(), argument[2]);
  cosA.MakeOX(theCommands, theCommands.opCos(), argument[1]);
  cosB.MakeOX(theCommands, theCommands.opCos(), argument[2]);
  output = sinA * cosB + cosA * sinB;
  return true;
}

bool CalculatorFunctions::innerTrigSumToTrigProduct(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerTrigSumToTrigProduct");
  if (!input.startsWith(theCommands.opPlus(), 3)) {
    return false;
  }
  Expression leftE = input[1], rightE = input[2];
  bool isGood = false;
  int leftSign = 1;
  int rightSign = 1;
  if (leftE.startsWith(theCommands.opTimes(), 3)) {
    if (leftE[1].IsEqualToMOne()) {
      leftE = leftE[2];
      leftSign = - 1;
    }
  }
  if (rightE.startsWith(theCommands.opTimes(), 3)) {
    if (rightE[1].IsEqualToMOne()) {
      rightE = rightE[2];
      rightSign = - 1;
    }
  }
  if (
    leftE.startsWith(theCommands.opSin(), 2) &&
    rightE.startsWith(theCommands.opSin(), 2)
  ) {
    isGood = true;
  } else if (
    leftE.startsWith(theCommands.opCos(), 2) &&
    rightE.startsWith(theCommands.opCos(), 2)
  ) {
    isGood = true;
  }
  if (!isGood) {
    return false;
  }
  Expression argSum, argDiff, leftMultiplicand, rightMultiplicand;
  if (leftE.startsWith(theCommands.opSin(), 2)) {
    argSum =  (leftE[1] * leftSign + rightE[1] * rightSign) / 2;
    argDiff = (leftE[1] * leftSign - rightE[1] * rightSign) / 2;
    leftMultiplicand.MakeOX(theCommands, theCommands.opCos(), argDiff);
    rightMultiplicand.MakeOX(theCommands, theCommands.opSin(), argSum);
    output = leftMultiplicand * rightMultiplicand * 2;
    return true;
  } else {
    if (leftSign == rightSign) {
      argSum = (leftE[1] + rightE[1]) / 2;
      argDiff = (leftE[1] - rightE[1]) / 2;
      leftMultiplicand.MakeOX(theCommands, theCommands.opCos(), argDiff);
      rightMultiplicand.MakeOX(theCommands, theCommands.opCos(), argSum);
      output = leftMultiplicand * rightMultiplicand * 2;
      if (leftSign == - 1) {
        output *= - 1;
      }
      return true;
    } else {
      argSum = (leftE[1] + rightE[1]) / 2;
      argDiff = (leftE[1] * leftSign + rightE[1] * rightSign) / 2;
      leftMultiplicand.MakeOX(theCommands, theCommands.opSin(), argDiff);
      rightMultiplicand.MakeOX(theCommands, theCommands.opSin(), argSum);
      output = leftMultiplicand * rightMultiplicand * 2;
      return true;
    }
  }
  // return false;
}

bool CalculatorFunctions::innerCosineOfAngleSumToTrig(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerCosineOfAngleSumToTrig");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (!argument.startsWith(theCommands.opPlus(), 3)) {
    return false;
  }
  Expression sinA, sinB, cosA, cosB;
  sinA.MakeOX(theCommands, theCommands.opSin(), argument[1]);
  sinB.MakeOX(theCommands, theCommands.opSin(), argument[2]);
  cosA.MakeOX(theCommands, theCommands.opCos(), argument[1]);
  cosB.MakeOX(theCommands, theCommands.opCos(), argument[2]);
  output = cosA * cosB - sinA * sinB;
  return true;
}

bool CalculatorFunctions::innerDistributeSqrt(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerDistributeSqrt");
  if (!input.startsWith(theCommands.opSqrt(), 3)) {
    return false;
  }
  const Expression& base = input[2];
  const Expression& oneOverExponentE = input[1];
  if (!base.startsWith(theCommands.opTimes(), 3)) {
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
        if (!exponentRat.getDenominator().IsEven()) {
          isGood = true;
        }
      }
    }
  }
  if (!isGood) {
    return false;
  }
  Expression leftE, rightE;
  leftE.MakeXOX(theCommands, theCommands.opSqrt(), oneOverExponentE, base[1]);
  rightE.MakeXOX(theCommands, theCommands.opSqrt(), oneOverExponentE, base[2]);
  return output.MakeXOX(theCommands, theCommands.opTimes(), leftE, rightE);
}

bool CalculatorFunctions::innerIsAlgebraicRadical(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsAlgebraicRadical");
  if (input.size() != 2) {
    return false;
  }
  int result = static_cast<int>(input[1].IsAlgebraicRadical());
  return output.assignValue(result, theCommands);
}

bool CalculatorFunctions::innerIsReal(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsReal");
  if (input.size() != 2) {
    return false;
  }
  if (!input[1].evaluatesToDouble()) {
    return output.assignValue(0, theCommands);
  }
  return output.assignValue(1, theCommands);
}

bool CalculatorFunctions::innerExpressiontoUTF8String(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerExpressiontoUTF8String");
  if (input.size() > 2) {
    Expression argumentSequence = input;
    argumentSequence.setChildAtomValue(0, theCommands.opSequence());
    return output.assignValue(argumentSequence.toUTF8String(), theCommands);
  }
  return output.assignValue(input[1].toUTF8String(), theCommands);
}

bool CalculatorFunctions::innerIsProductTermsUpToPower(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIsProductTermsUpToPower");
  if (input.size() < 3) {
    return false;
  }
  Expression theBase;
  theBase = input[1];
  LargeInteger desiredMaxPower = 1;
  if (theBase.startsWith(theCommands.opThePower(), 3)) {
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
  if (!theCommands.CollectOpands(input[2], theCommands.opTimes(), theMultiplicands)) {
    return theCommands << "Could not extract multiplicands from: "
    << input[2].toString();
  }

  for (int k = 0; k < theMultiplicands.size; k ++) {
    List<List<Expression> > theSummands;
    if (!theCommands.GetSumProductsExpressions(theMultiplicands[k], theSummands)) {
      return theCommands << "Failed to extract sum from "
      << theMultiplicands[k].toString();
    }
    for (int i = 0; i < theSummands.size; i ++) {
      LargeInteger foundPower = 0;
      for (int j = 0; j < theSummands[i].size; j ++) {
        if (theSummands[i][j] == theBase) {
          foundPower ++;
          continue;
        }
        if (theSummands[i][j].startsWith(theCommands.opThePower(), 3)) {
          if (theSummands[i][j][1] == theBase) {
            LargeInteger localPower;
            if (theSummands[i][j][2].isInteger(&localPower)) {
              foundPower += localPower;
              continue;
            }
          }
        }
        if (!theSummands[i][j].evaluatesToDouble()) {
          return output.assignValue(0, theCommands);
        }
      }
      if (foundPower > desiredMaxPower) {
        return output.assignValue(0, theCommands);
      }
    }
  }
  return output.assignValue(1, theCommands);
}

bool CalculatorFunctions::innerScaleToLeadingUnit(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerScaleToLeadingUnit");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  LinearCombination<Expression, Rational> theCollection;
  theCommands.functionCollectSummands(theCommands, argument, theCollection);
  theCollection /= theCollection.GetLeadingCoefficient(nullptr);
  return output.makeSum(theCommands, theCollection);
}

bool CalculatorFunctionsBinaryOps::innerPowerRationalByRationalOutputAlgebraic(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBinaryOps::innerPowerRatByRatReducePrimeFactors");
  if (!input.startsWith(theCommands.opThePower(), 3)) {
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
  theRadical.MakeXOX(theCommands, theCommands.opSqrt(), theCommands.ETwo(), input[1]);
  if (!CalculatorFunctions::innerSqrt(theCommands, theRadical, reduced)) {
    return false;
  }
  if (!reduced.isOfType<AlgebraicNumber>()) {
    return false;
  }
  Expression theIntegerPower;
  theIntegerPower.assignValue(Rational(exponent.getNumerator()), theCommands);
  return output.MakeXOX(theCommands, theCommands.opThePower(),reduced, theIntegerPower);
}

bool CalculatorFunctions::innerNewtonsMethod(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerNewtonsMethod");
  if (input.size() != 4) {
    return false;
  }
  Expression theFun;
  if (!CalculatorFunctions::functionEqualityToArithmeticExpression(theCommands, input[1], theFun)) {
    theFun = input[1];
  }
  HashedList<Expression> theVars;
  if (!theFun.GetFreeVariables(theVars, true)) {
    return theCommands << "Failed to get free variables from: " << theFun.toString();
  }
  if (theVars.size != 1) {
    return theCommands << "While trying to extract a function from: " << theFun.toString()
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
    return output.makeError(errorStream.str(), theCommands);
  }
  if (numIterations < 1 || numIterations > 50) {
    std::stringstream errorStream;
    errorStream << "While doing Newton's method with the command: " << input.toString()
    << ", the third argument requests " << numIterations
    << " iterations. However, the number of iterations is required to be a number between 1 and 50. ";
    return output.makeError(errorStream.str(), theCommands);
  }
  MapList<std::string, Expression, MathRoutines::HashString> theSub;
  theSub.setKeyValue("x", theVars[0]);
  theSub.setKeyValue("f", theFun);
  theSub.setKeyValue("a", theCommands.getNewAtom());
  theSub.setKeyValue("iteratedMap", theCommands.getNewAtom());
  theSub.setKeyValue("NewtonMap", theCommands.getNewAtom());
  theSub.setKeyValue("y", theCommands.getNewAtom());
  theSub.setKeyValue("startingPoint", input[2]);
  theSub.setKeyValue("numIterations", input[3]);
  return output.AssignStringParsed(
    "(NewtonMap{}{{a}} = DoubleValue( (iteratedMap =x- f/ Differentiate{}(x, f); x ={{a}}; iteratedMap )_3); "
    "y_{0} = startingPoint;"
    "y_{{a}} = NewtonMap{}(y_{a- 1});"
    "y_{numIterations})_4",
    &theSub,
    theCommands
  );
}

bool CalculatorFunctions::innerValueOfModularExpression(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerValueOfModularExpression");
  if (input.size() != 2) {
    return false;
  }
  ElementZmodP candidateZmodP;
  if (input[1].isOfType(&candidateZmodP)) {
    return output.assignValue(candidateZmodP.theValue, theCommands);
  }
  Polynomial<ElementZmodP> candidatePolynomial;
  if (input[1].isOfType(&candidatePolynomial)) {
    Polynomial<Rational> rationalConversion;
    for (int i = 0; i < candidatePolynomial.size(); i ++) {
      rationalConversion.addMonomial(candidatePolynomial.theMonomials[i], candidatePolynomial.coefficients[i].theValue);
    }
    return output.assignValueWithContext(rationalConversion, input[1].getContext(), theCommands);
  }

  PolynomialModuloPolynomial<ElementZmodP> quotientElement;
  if (input[1].isOfType(&quotientElement)) {
    return output.assignValueWithContext(quotientElement.value, input[1].getContext(), theCommands);
  }
  return false;
}

bool CalculatorFunctions::innerElementEllipticCurveNormalForm(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerElementEllipticCurveNormalForm");
  if (input.size() != 4) {
    return theCommands << "Elliptic curve expects 3 arguments (curve, generator letter, baseX and baseY) ";
  }
  const Expression& xDefE = input[2];
  const Expression& yDefE = input[3];
  //  HashedList<Expression> xEcandidates, yEcandidates;
  //  if (!xDefE.GetFreeVariables(xEcandidates, false))
  //    return theCommands << "Failed to get free variables from " << xDefE.toString();
  //  if (!yDefE.GetFreeVariables(yEcandidates, false))
  //    return theCommands << "Failed to get free variables from " << yDefE.toString();
  //  if (xEcandidates.size != 1 || yEcandidates.size != 1)
  //    return theCommands << "Couldn't get single free variable from " << xEcandidates.toStringCommaDelimited()
  //    << " and/or " << yEcandidates.toStringCommaDelimited();
  //  if (CalculatorFunctions::innerEqualityToArithmeticExpression())
  if (!xDefE.startsWith(theCommands.opDefine(), 3)) {
    return theCommands << "Failed to extract variable form " << xDefE.toString();
  }
  if (!yDefE.startsWith(theCommands.opDefine(), 3)) {
    return theCommands << "Failed to extract variable form " << yDefE.toString();
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
    return theCommands << "Could not extract rational or element of z mod p from "
    << xDefE[2].toString() << ", " << yDefE[2].toString();
  }
  if (isElementZmodP) {
    if (eltZmodP.xCoordinate.theModulus != eltZmodP.yCoordinate.theModulus) {
      return theCommands << "The two base coordinates have different moduli. ";
    }
  }
  Expression theCurveE;
  if (!CalculatorFunctions::functionEqualityToArithmeticExpression(theCommands, input[1], theCurveE)) {
    return theCommands << "Could not get arithmetic expression from: " << input[1].toString()
    << ". I was expecting a cubic equality.";
  }
  Expression thePolyE;
  Polynomial<Rational> thePoly;
  if (!CalculatorConversions::functionPolynomial<Rational>(
    theCommands, theCurveE, thePolyE
  )) {
    return theCommands << "Could not get polynomial from " << theCurveE.toString();
  }
  if (!thePolyE.isOfType(&thePoly)) {
    return theCommands << "Could not convert to polynomial: " << thePolyE.toString();
  }
  ExpressionContext curveContext = thePolyE.getContext();
  if (curveContext.numberOfVariables() != 2) {
    return theCommands << "Expected 2 context variables in "
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
    return theCommands << "Curve variable "
    << curveContext.getVariable(1).toString()
    << " not equal to " << yE.toString();
  }
  theCommands << "Created elliptic curve " << thePolyE.toString()
  << " = 0. The variables are assumed to be: x = " << xE.toString() << ", y = " << yE.toString();
  if (thePoly.size() > 4) {
    return theCommands << "Elliptic curve allowed to have max 4 terms, yours has: " << thePoly.size();
  }
  MonomialP xCubed, xLinear, ySquared;
  xCubed.makeEi(indexX, 3);
  xLinear.makeEi(indexX, 1);
  ySquared.makeEi(indexY, 2);
  Rational coefficientY = thePoly.GetMonomialCoefficient(ySquared);
  Rational coefficientXcubed = - thePoly.GetMonomialCoefficient(xCubed);
  if (coefficientY == 0) {
    return theCommands << "Did not find square term in your curve.";
  }
  if (coefficientXcubed == 0) {
    return theCommands << "Did not find cube term in your curve.";
  }
  Rational coefficientXlinear = - thePoly.GetMonomialCoefficient(xLinear);
  Rational constCoefficient = - thePoly.GetConstantTerm();
  EllipticCurveWeierstrassNormalForm& curveConstants = isRational ? eltRational.owner : eltZmodP.owner;
  if (
    !coefficientXlinear.isInteger(&curveConstants.linearCoefficient) ||
    !constCoefficient.isInteger(&curveConstants.constantTerm)
  ) {
    return theCommands << "At the moment only integer elliptic curve coefficients are supported. Your coefficients were: "
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
    return theCommands << "It appears your curve: " << theCurveE.toString()
    << " is not of the form y^2 = x^3 + ax + b. ";
  }
  ExpressionContext theContext(theCommands);
  theContext.addVariable(xE);
  theContext.addVariable(yE);
  if (isRational) {
    return output.assignValueWithContext(eltRational, theContext, theCommands);
  }
  if (isElementZmodP) {
    return output.assignValueWithContext(eltZmodP, theContext, theCommands);
  }
  return false;
}

bool CalculatorFunctions::innerPrecomputeSemisimpleLieAlgebraStructure(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPrecomputeSemisimpleLieAlgebraStructure");
  if (!global.theResponse.monitoringAllowed()) {
    global.theResponse.Initiate("Triggered by innerPrecomputeSemisimpleLieAlgebraStructure.");
  }
  (void) input;
  List<DynkinType> theTypes;
  DynkinType::GetPrecomputedDynkinTypes(theTypes);
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
    theAlgebra.theWeyl.MakeFromDynkinType(theTypes[i]);
    theAlgebra.ComputeChevalleyConstants();
    theAlgebra.ToHTMLCalculator(true, true, false);
    SltwoSubalgebras theSl2s(theAlgebra);
    theSl2s.theRootSAs.flagPrintParabolicPseudoParabolicInfo = true;
    theAlgebra.FindSl2Subalgebras(theAlgebra, theSl2s);
    theSl2s.toHTML();
    if ((false)) {
      if (theTypes[i].HasPrecomputedSubalgebras()) {
        SemisimpleSubalgebras theSubalgebras;
        MapReferences<DynkinType, SemisimpleLieAlgebra> subalgebrasContainer;
        ListReferences<SltwoSubalgebras> sl2Conainer;
        if (!theSubalgebras.ComputeStructureWriteFiles(
          theAlgebra,
          theCommands.theObjectContainer.theAlgebraicClosure,
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
  return output.assignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerShowKnownObjectIds(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerShowKnownObjectIds");
  (void) input;
  return output.assignValue(ASNObject::ToStringAllRecognizedObjectIds(), theCommands);
}

bool CalculatorFunctions::innerTestASN1Decode(
  Calculator& theCommands, const Expression& input, Expression& output
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
  if (!theDecoder.Decode(dataList, 0, result, &commentsOnError)) {
    out << "Failed to decode.<br>" << commentsOnError.str();
  } else {
    List<unsigned char> recoded;
    result.WriteBytesUpdatePromisedLength(recoded);
    std::string originalHex = Crypto::convertStringToHex(data, 0, false);
    std::string recodedHex = Crypto::convertListUnsignedCharsToHex(recoded);
    out << StringRoutines::Differ::DifferenceHTMLStatic(originalHex, recodedHex, "original", "recoded");
    out << theDecoder.ToStringAnnotateBinary();
  }
  return output.assignValue(out.str(), theCommands);
}

std::string StringRoutines::ConvertStringToCalculatorDisplay(
  const std::string& input
) {
  std::string converted = StringRoutines::ConvertStringToHexIfNonReadable(input, 100, true);
  if (converted == input) {
    return converted;
  }
  std::stringstream out;
  out << "ConvertHexToString{}\"" << converted << "\"";
  out << "<br>\""
  << StringRoutines::ConvertStringToJavascriptString(input)
  << "\"";
  return out.str();
}

std::string StringRoutines::ConvertByteToHex(unsigned char byte) {
  std::stringstream out;
  out << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned>(byte);
  return out.str();
}

std::string StringRoutines::ConvertStringToHexIfNonReadable(
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
