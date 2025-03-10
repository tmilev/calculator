#include "calculator_html_interpretation.h"
#include "calculator_inner_functions.h"
#include "general_file_operations_encodings.h"
#include "html_routines.h"
#include "string_constants.h"
#include "web_api.h"

std::string CalculatorHTML::Test::filenameFullOutput =
"test/alltestedproblems.txt";

CalculatorHTML::Test::Test() {
  this->filesToInterpret = 0;
  this->randomSeed = 0;
  this->firstFileIndex = 0;
  this->flagCorrectedTotalFiles = false;
}

bool CalculatorHTML::Test::computeTotalFiles() {
  STACK_TRACE("CalculatorHTML::Test::computeTotalFiles");
  std::stringstream commentsOnFailure;
  if (
    !FileOperations::getFolderFileNamesVirtual(
      "problems/default/",
      this->fileNamesAll,
      &this->fileExtensionsAll,
      false,
      false,
      &commentsOnFailure
    )
  ) {
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

std::string CalculatorHTML::Test::OneProblemTest::toStringHTMLTableRow(
  int rowIndex
) {
  STACK_TRACE("CalculatorHTML::Test::OneProblemTest::toStringHTMLTableRow");
  std::stringstream out;
  out << "<tr>";
  out << "<td style='min-width:25px'>" << rowIndex << ". </td>";
  out << "<td style='min-width:200px'>";
  out << CalculatorHTML::toStringLinkFromProblem(this->fileName) << "</td>";
  if (this->errorLoad != "") {
    out
    << "<td style = 'min-width:300px'><b>Couldn't load.</b> "
    << this->errorLoad
    << "</td>";
    out << "</tr>";
    return out.str();
  }
  out
  << "<td style = 'min-width:60px'><b style = 'color:green'>Success</b></td>";
  if (!this->flagInterpretationSuccess) {
    out
    << "<td style = 'min-width:400px'><b style='color:red'>Failure.</b> "
    << "Comments: "
    << this->errorInterpretation;
    out << "</td></tr>";
    return out.str();
  }
  out << "<td style='min-width:60px'><b style='color:green'>Success</b></td>";
  if (this->answers.size == 0) {
    out << "<td><b style = 'color:red'>No built-in answer.</b></td>";
  } else {
    if (this->flagAllBuiltInAnswersOK) {
      out
      << "<td style = 'min-width:60px'><b style = 'color:green'>"
      << "Success</b></td>";
    } else {
      out
      << "<td style = 'min-width:60px'><b style = 'color:red'>"
      << "Failure.</b></td>";
      out << "<td style = 'min-width:400px'>";
      for (int i = 0; i < this->answers.size; i ++) {
        out << this->answers[i].builtInAnswer;
      }
      if (this->errorAnswers != "") {
        out << this->errorAnswers;
      }
      out << "</td>" << "</tr>";
      return out.str();
    }
  }
  return out.str();
}

std::string CalculatorHTML::Test::toHTMLDebug() {
  std::stringstream out;
  out
  << "File names all: "
  << this->fileNamesAll.toStringCommaDelimited()
  << "<br>";
  out
  << "Extensions all: "
  << this->fileExtensionsAll.toStringCommaDelimited()
  << "<br>";
  out << "File names extracted: " << this->fileNames.toStringCommaDelimited();
  return out.str();
}

std::string CalculatorHTML::Test::toHTMLBuiltIn() {
  std::stringstream tableBad;
  std::stringstream tableGood;
  std::stringstream out;
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
    std::string nextRow =
    this->results[i].toStringHTMLTableRow(this->firstFileIndex + i);
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
    } else {
      tableGood << nextRow;
    }
  }
  tableBad << "</table>";
  tableGood << "</table>";
  if (this->errorComments != "") {
    out << this->errorComments << "<br>";
  }
  if (numberBad > 0) {
    out << "<b style = 'color:red'>" << numberBad << " bad problems</b>. ";
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

JSData CalculatorHTML::Test::OneProblemTest::toJSON(CalculatorHTML& problem) {
  JSData result;
  result["randomSeed"] = this->randomSeed;
  result["body"] = problem.outputHtmlBodyNoTag;
  result["head"] = problem.outputHtmlHeadNoTag;
  return result;
}

bool CalculatorHTML::Test::OneProblemTest::run(JSData* outputComplete) {
  CalculatorHTML problem;
  std::stringstream commentsOnFailure;
  std::stringstream randomSeedStringStarting;
  randomSeedStringStarting << this->randomSeed;
  problem.fileName = this->fileName;
  this->flagSuccess = false;
  StateMaintainer<
    MapList<
      std::string, std::string, HashFunctions::hashFunction<std::string>
    >
  > maintainArguments(global.webArguments);
  if (
    !problem.loadMe(false, randomSeedStringStarting.str(), &commentsOnFailure)
  ) {
    this->errorLoad = commentsOnFailure.str();
    return this->flagSuccess;
  }
  this->flagInterpretationSuccess = problem.interpretHtml(&commentsOnFailure);
  if (!this->flagInterpretationSuccess) {
    this->errorInterpretation = commentsOnFailure.str();
    return this->flagSuccess;
  }
  std::stringstream randomSeedStream;
  randomSeedStream << problem.problemData.randomSeed;
  this->answers.setSize(problem.problemData.answers.size());
  this->flagAllBuiltInAnswersOK = true;
  global.setWebInput(WebAPI::Problem::fileName, problem.fileName);
  global.setWebInput(WebAPI::Problem::randomSeed, randomSeedStream.str());
  this->flagSuccess = true;
  for (int j = 0; j < this->answers.size; j ++) {
    CalculatorHTML::Test::OneProblemTest::OneAnswer& current =
    this->answers[j];
    current.answerId = problem.problemData.answers.values[j].answerId;
    current.answerIdWebAPI =
    WebAPI::Problem::calculatorAnswerPrefix + current.answerId;
    global.setWebInput(current.answerIdWebAPI, "1");
    current.builtInAnswerAPICall =
    WebAPIResponse::getAnswerOnGiveUp(
      randomSeedStream.str(),
      &current.builtInAnswer,
      &current.flagBuiltInGenerated,
      false
    );
    global.webArguments.removeKey(current.answerIdWebAPI);
    if (!current.flagBuiltInGenerated) {
      commentsOnFailure
      << "Failed to generate answer: "
      << current.answerId
      << "<br>";
      commentsOnFailure
      << current.builtInAnswerAPICall[WebAPI::Result::resultHtml].stringValue;
      this->flagAllBuiltInAnswersOK = false;
      this->flagSuccess = false;
      break;
    }
    current.builtInAnswerEncoded =
    HtmlRoutines::convertStringToURLString(current.builtInAnswer, false);
    global.setWebInput(current.answerIdWebAPI, current.builtInAnswerEncoded);
    current.builtInAnswerReply =
    WebAPIResponse::submitAnswersJSON(
      randomSeedStream.str(), &current.flagBuiltInWorks, false
    );
    global.webArguments.removeKey(current.answerIdWebAPI);
    if (!current.flagBuiltInWorks) {
      this->flagAllBuiltInAnswersOK = false;
      commentsOnFailure
      << "<br>Built-in answer of index: "
      << j
      << " does not work:<br>"
      << current.builtInAnswer
      << "<hr>"
      << current.builtInAnswerReply[WebAPI::Result::resultHtml].stringValue;
      this->flagSuccess = false;
      break;
    }
  }
  this->errorAnswers = commentsOnFailure.str();
  if (outputComplete != nullptr) {
    *outputComplete = this->toJSON(problem);
  }
  return this->flagSuccess;
}

std::string CalculatorHTML::Test::toStringSummary() {
  std::stringstream out;
  out
  << "First file index: "
  << this->firstFileIndex
  << ", inputFilesToInterpret: "
  << this->filesToInterpret
  << " inputRandomSeed: "
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

bool CalculatorHTML::Test::builtInMultiple(
  int inputFirstFileIndex,
  int inputFilesToInterpret,
  int inputRandomSeed,
  int numberOfRepetitions,
  JSData* outputFullResult,
  std::stringstream* comments
) {
  List<int> randomSeeds;
  randomSeeds.setSize(numberOfRepetitions);
  ProgressReport report;
  for (int i = 0; i < numberOfRepetitions; i ++) {
    randomSeeds[i] = inputRandomSeed + i;
    std::stringstream reportStream;
    reportStream
    << "Testing problems, round "
    << i + 1
    << " out of "
    << numberOfRepetitions
    << ", starting random seed: "
    << randomSeeds[i]
    << ". ";
    report.report(reportStream.str());
    CalculatorHTML::Test tester;
    JSData resultForRandomSeed;
    JSData* outputResultForRandomSeed = (outputFullResult != nullptr) ?
    &resultForRandomSeed :
    nullptr;
    if (
      !tester.builtIn(
        inputFirstFileIndex,
        inputFilesToInterpret,
        randomSeeds[i],
        outputResultForRandomSeed
      )
    ) {
      if (comments != nullptr) {
        *comments
        << "Failed run "
        << i + 1
        << " out of "
        << numberOfRepetitions
        << ". "
        << tester.toHTMLBuiltIn();
        *comments
        << "Failed run "
        << i + 1
        << " out of "
        << numberOfRepetitions
        << ". "
        << tester.toStringSummary();
      }
      return false;
    }
    if (outputFullResult != nullptr) {
      std::stringstream intToStringConverter;
      intToStringConverter << randomSeeds[i];
      (*outputFullResult)[intToStringConverter.str()] = resultForRandomSeed;
    }
    if (i == numberOfRepetitions - 1 && comments != nullptr) {
      *comments
      << numberOfRepetitions
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
  int inputRandomSeed,
  JSData* output
) {
  STACK_TRACE("CalculatorHTML::Test::builtIn");
  this->firstFileIndex = inputFirstFileIndex;
  this->filesToInterpret = inputFilesToInterpret;
  this->randomSeed = inputRandomSeed;
  ProgressReport report;
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
      reportStream
      << "<b style = 'color:red'>Found "
      << badSoFar
      << " bad files so far. </b><br>";
    }
    reportStream
    << "File: "
    << i
    << " out of "
    << this->fileNames.size
    << ". ";
    reportStream
    << currentTest.fileName
    << ". Random seed: "
    << this->randomSeed
    << ".";
    if (global.runMode == GlobalVariables::RunMode::consoleTest) {
      global << reportStream.str() << Logger::endL;
    }
    report.report(reportStream.str());
    JSData oneProblemHolder;
    JSData* oneProblem = output == nullptr ? nullptr : &oneProblemHolder;
    if (!currentTest.run(oneProblem)) {
      result = false;
    }
    if (output != nullptr) {
      (*output)[currentTest.fileName] = oneProblemHolder;
    }
    if (!currentTest.flagSuccess) {
      badSoFar ++;
      if (global.runMode == GlobalVariables::RunMode::consoleTest) {
        global
        << Logger::red
        << "Failure @ index: "
        << i
        << ". "
        << "Elapsed ms: "
        << global.getElapsedMilliseconds()
        << ". "
        << Logger::endL;
        global
        << Logger::yellow
        << "https://localhost:8166"
        << HtmlRoutines::getProblemURLRelative(currentTest.fileName)
        << Logger::endL;
      }
    } else if (global.runMode == GlobalVariables::RunMode::consoleTest) {
      global
      << Logger::green
      << "Success @ index: "
      << i
      << ". "
      << "Elapsed ms: "
      << global.getElapsedMilliseconds()
      << ". "
      << Logger::endL;
    }
  }
  this->errorComments += commentsOnFailure.str();
  return result;
}

bool TopicElementParser::Test::hasEducationalMaterials() {
  std::string unused;
  std::stringstream comments;
  return
  FileOperations::loadFiletoStringVirtualCustomizedReadOnly(
    "/coursesavailable/default.txt", unused, &comments
  );
}

void TopicElementParser::Test::logMessageNoEducationalMaterials() {
  global << Logger::red << "***************" << Logger::endL;
  global << Logger::red << "WARNING: topic files are absent." << Logger::endL;
  global
  << Logger::red
  << "EDUCATIONAL MATERIALS TEST SKIPPED."
  << Logger::endL;
  global << Logger::red << "***************" << Logger::endL;
}

bool TopicElementParser::Test::all() {
  std::stringstream comments;
  if (!TopicElementParser::Test::hasEducationalMaterials()) {
    TopicElementParser::Test::logMessageNoEducationalMaterials();
    return true;
  }
  TopicElementParser::Test::defaultTopicListsOKCrashOnFailure();
  TopicElementParser::Test::defaultPdfsOKCrashOnFailure();
  return true;
}

bool TopicElementParser::Test::defaultTopicListsOKCrashOnFailure() {
  TopicElementParser::Test tester;
  if (!tester.defaultTopicListsOK()) {
    global.fatal
    << "Topic list tests failed. "
    << tester.comments.str()
    << global.fatal;
  }
  return false;
}

bool TopicElementParser::Test::defaultPdfsOKCrashOnFailure() {
  TopicElementParser::Test tester;
  int whichTopic = 0;
  if (!tester.defaultPdfsOK(whichTopic)) {
    std::stringstream topicBuildCommand;
    topicBuildCommand << "BuildSlidesInTopicList(" << whichTopic << ")";
    global.fatal
    << "Default pdfs are broken. "
    << tester.comments.str()
    << "\nYou may want to regenerate them using command: "
    << "https://localhost:8166/"
    << HtmlRoutines::getCalculatorComputationURL(topicBuildCommand.str())
    << global.fatal;
  }
  return false;
}

bool TopicElementParser::Test::defaultPdfsOK(int& whichTopic) {
  STACK_TRACE("TopicElementParser::Test::defaultPdfsOK");
  CourseList courses;
  if (!courses.load()) {
    this->comments
    << "Failed to load the list of available courses. "
    << courses.errorMessage;
    return false;
  }
  for (whichTopic = 0; whichTopic < courses.allCourses.size; whichTopic ++) {
    CalculatorHTML owner;
    owner.topicListFileName =
    courses.allCourses[whichTopic].courseTopicsWithFolder();
    if (!owner.loadAndParseTopicList(this->comments)) {
      this->comments
      << "Failed to load course "
      << owner.topicListFileName
      << ". ";
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
  STACK_TRACE("TopicElementParser::Test::defaultTopicListsOK");
  CourseList courses;
  if (!courses.load()) {
    this->comments
    << "Failed to load the list of available courses. "
    << courses.errorMessage;
    return false;
  }
  for (int i = 0; i < courses.allCourses.size; i ++) {
    CalculatorHTML owner;
    owner.topicListFileName = courses.allCourses[i].courseTopicsWithFolder();
    if (!owner.loadAndParseTopicList(this->comments)) {
      this->comments
      << "Failed to load course "
      << owner.topicListFileName
      << ". ";
      return false;
    }
    if (!owner.topics.checkNoErrors(&this->comments)) {
      this->comments
      << "Errors in topic list: "
      << owner.topicListFileName
      << ". ";
      return false;
    }
    if (!owner.topics.checkProblemsOpen(&this->comments)) {
      this->comments
      << "Not all problems open correctly in topic list: "
      << owner.topicListFileName
      << ". ";
      return false;
    }
    this->comments
    << "Topic list: "
    << owner.topicListFileName
    << " with "
    << owner.topics.topics.size()
    << " topics <b style = 'color:green'>looks ok</b>.<br>";
  }
  return true;
}

bool CalculatorHTML::Test::all() {
  CalculatorHTML::Test::parsingTest();
  CalculatorHTML::Test::builtInCrashOnFailure();
  return true;
}

bool CalculatorHTML::Test::parsingTest() {
  List<std::string> fileNames;
  FileOperations::getFolderFileNamesVirtual("test/html_parser/", fileNames);
  for (int i = 0; i < fileNames.size; i ++) {
    if (fileNames[i] == "." || fileNames[i] == "..") {
      continue;
    }
    std::string currentFileName = "test/html_parser/" + fileNames[i];
    CalculatorHTML parser;
    if (
      !FileOperations::loadFileToStringVirtual(
        currentFileName, parser.parser.inputHtml, false, nullptr
      )
    ) {
      global.fatal
      << "Failed to load filename: "
      << currentFileName
      << "."
      << global.fatal;
    }
    if (!parser.parser.parseHTML(nullptr)) {
      global.fatal
      << "Failed to parse: "
      << currentFileName
      << ". Calculator link:\n"
      << "https://localhost:8166"
      << HtmlRoutines::getProblemURLRelative(currentFileName)
      << "\n"
      << global.fatal;
    }
  }
  return true;
}

bool CalculatorHTML::Test::builtInCrashOnFailure() {
  if (!TopicElementParser::Test::hasEducationalMaterials()) {
    global
    << Logger::red
    << "Educational materials NOT FOUND. Problem tests skipped. "
    << Logger::endL;
    return true;
  }
  std::stringstream comments;
  JSData actualOutput;
  JSData desiredOutput;
  std::string desiredOutputString;
  bool desiredResultKnown =
  FileOperations::loadFileToStringVirtual(
    CalculatorHTML::Test::filenameFullOutput,
    desiredOutputString,
    false,
    nullptr
  );
  if (
    !CalculatorHTML::Test::builtInMultiple(
      0, 0, 0, 3, &actualOutput, &comments
    )
  ) {
    global.fatal
    << "Built-in problem tests failed. "
    << comments.str()
    << global.fatal;
  }
  std::string actualOutputString = actualOutput.toStringPretty();
  if (actualOutputString != desiredOutputString) {
    if (desiredResultKnown) {
      std::string newFileName =
      CalculatorHTML::Test::filenameFullOutput + ".new";
      FileOperations::writeFileVirtual(
        newFileName, actualOutputString, nullptr
      );
      global.fatal
      << "Detected change in the expected problem "
      << "content and the actual one. Wrote the new output as "
      << newFileName
      << ", please use a diff tool to see the difference. "
      << global.fatal;
    }
    global
    << Logger::purple
    << "No built-in problems on record. "
    << "Writing the actual output to file: "
    << CalculatorHTML::Test::filenameFullOutput
    << Logger::endL;
    FileOperations::writeFileVirtual(
      CalculatorHTML::Test::filenameFullOutput, actualOutputString, nullptr
    );
  } else {
    global
    << Logger::green
    << "No changes detected in problems, as desired! Compared "
    << Logger::purple
    << actualOutputString.size()
    << Logger::green
    << " bytes. "
    << Logger::endL;
  }
  return true;
}

bool Course::Test::all() {
  STACK_TRACE("Course::Test::all");
  global << "Testing course setup. " << Logger::endL;
  Course::Test::setDeadlines(DatabaseType::fallback);
  Course::Test::setDeadlines(DatabaseType::internal);
  return true;
}

Course::Test::Setup::Setup(DatabaseType databaseType):
databaseTester(databaseType) {
  this->maintainLogin.initialize(global.flagLoggedIn);
  this->maintainSSLFlag.initialize(global.flagUsingSSLinCurrentConnection);
  this->maintainUserRole.initialize(global.userDefault);
  this->maintainWebArguments.initialize(global.webArguments);
  this->maintainRequestType.initialize(global.requestType);
  this->maintainTimePointer.initialize(global.timePointer);
  global.timePointer = global.timeMockForTests;
  global.flagLoggedIn = true;
  global.userDefault.userRole = UserCalculatorData::Roles::administrator;
  global.userDefault.username = WebAPI::userDefaultAdmin;
  global.flagUsingSSLinCurrentConnection = true;
}

bool Course::Test::Setup::setupAll() {
  this->databaseTester.createAdminAccount(false);
  global.userDefault.computeCourseInformation();
  global.webArguments.setKeyValue(
    WebAPI::Request::teachersAndSections,
    "{\"teachers\":\"default\",\"sections\":\"1,2\"}"
  );
  std::string result = WebAPIResponse::addTeachersSections();
  std::string wanted = "Assigned";
  if (!StringRoutines::stringContains(result, wanted)) {
    global.fatal
    << "Adding teachers resulted in: "
    << result
    << ", expected to contain: "
    << wanted
    << global.fatal;
  }
  return true;
}

bool Course::Test::setDeadlines(DatabaseType databaseType) {
  STACK_TRACE("Course::Test::setDeadlines");
  Course::Test::Setup setup(databaseType);
  global.setWebInput(
    WebAPI::Frontend::problemFileName, "test/problems/interval_notation_1.html"
  );
  global.setWebInput(WebAPI::Problem::courseHome, "test/test.html");
  global.setWebInput(WebAPI::Problem::topicList, "test/topiclists/test.txt");
  global.setWebInput(WebAPI::Problem::courseHome, "COURSE");
  global.setWebInput(
    "mainInput",
    HtmlRoutines::convertStringToURLString(
      "{\"test/problems/interval_notation_1.html\":"
      "{\"deadlines\":{\"1\":\"2022-08-16\"}}}",
      false
    )
  );
  global.setWebInput(DatabaseStrings::labelUsername, WebAPI::userDefaultAdmin);
  setup.setupAll();
  std::string deadlineResult = WebAPIResponse::setProblemDeadline();
  if (!StringRoutines::stringContains(deadlineResult, "Modified")) {
    global.fatal
    << "Expected: string that contains 'Modified'. Got: "
    << deadlineResult
    << global.fatal;
  }
  global.setWebInput(
    "mainInput",
    HtmlRoutines::convertStringToURLString(
      "{\"test/problems/interval_notation_1.html\":"
      "{\"deadlines\":{\"1\":\"2000-00-00\",\"2\":\"2023-01-01\"}}}",
      false
    )
  );
  WebAPIResponse::setProblemDeadline();
  std::string wanted = "2000-00-00";
  QueryFind finder;
  finder.collection = DatabaseStrings::tableDeadlines;
  finder.nestedLabels.addOnTop(DatabaseStrings::labelDeadlinesSchema);
  finder.exactValue = "deadlinesdefaultCOURSE";
  JSData resultData;
  QueryUpdate::Test::findExactlyOneNoFail(finder, resultData);
  std::string result =
  resultData[DatabaseStrings::labelDeadlines][
    "test/problems/interval_notation_1.html"
  ][DatabaseStrings::labelDeadlines]["1"].stringValue;
  if (result != wanted) {
    global.fatal
    << "Unexpected deadline: got: "
    << result
    << ", wanted: "
    << wanted
    << ". Query result: "
    << resultData
    << global.fatal;
  }
  global.setWebInput(
    "mainInput",
    HtmlRoutines::convertStringToURLString(
      "{\"test/problems/sample2.html\":"
      "{\"deadlines\":{\"2\":\"2023-01-01\"}}}",
      false
    )
  );
  WebAPIResponse::setProblemDeadline();
  if (!StringRoutines::stringContains(deadlineResult, "Modified")) {
    global.fatal
    << "Expected: string that contains 'Modified'. Got: "
    << deadlineResult
    << global.fatal;
  }
  return true;
}

void CalculatorFunctionsFreecalc::Test::all() {
  std::string unused;
  if (
    !FileOperations::loadFileToStringVirtual(
      "freecalc/homework/referenceallproblemsbycourse"
      "/calculusimasterproblemsheet.tex",
      unused,
      true,
      nullptr
    )
  ) {
    global << Logger::red << "*****************" << Logger::endL;
    global
    << "Freecalc tests skipped: "
    << Logger::red
    << "free calc not found."
    << Logger::endL;
    global << Logger::red << "*****************" << Logger::endL;
    return;
  }
  CalculatorFunctionsFreecalc::Test::crawl();
}

void CalculatorFunctionsFreecalc::Test::crawl() {
  StateMaintainer<DatabaseType> maintainer;
  maintainer.initialize(global.databaseType);
  global.databaseType = DatabaseType::noDatabaseEveryoneIsAdmin;
  Calculator calculator;
  calculator.initialize(Calculator::Mode::full);
  calculator.evaluate(
    "Crawl(\"freecalc/homework/referenceallproblemsbycourse"
    "/calculusimasterproblemsheet.tex\")"
  );
  std::string result = calculator.programExpression.toString();
  std::string expected =
  "Output file: "
  "<a href='/output/latexOutput.tex'>latexOutput.tex</a>";
  if (result != expected) {
    global.fatal
    << "While crawling freecalc, got:\n"
    << result
    << "\nexpected:\n"
    << expected
    << global.fatal;
  }
}
