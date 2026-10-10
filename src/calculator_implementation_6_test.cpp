#include "calculator_html_interpretation.h"
#include "calculator_inner_functions.h"
#include "general_file_operations_encodings.h"
#include "html_routines.h"
#include "string_constants.h"
#include "test.h"
#include "web_api.h"

namespace Testing {

bool TopicElementParserTest::hasEducationalMaterials() {
  std::string unused;
  std::stringstream comments;
  return
  FileOperations::loadFiletoStringVirtualCustomizedReadOnly(
    "/coursesavailable/default.txt", unused, &comments
  );
}

void TopicElementParserTest::logMessageNoEducationalMaterials() {
  global << Logger::red << "***************" << Logger::endL;
  global << Logger::red << "WARNING: topic files are absent." << Logger::endL;
  global
  << Logger::red
  << "EDUCATIONAL MATERIALS TEST SKIPPED."
  << Logger::endL;
  global << Logger::red << "***************" << Logger::endL;
}

bool TopicElementParserTest::all() {
  std::stringstream comments;
  if (!TopicElementParserTest::hasEducationalMaterials()) {
    TopicElementParserTest::logMessageNoEducationalMaterials();
    return true;
  }
  TopicElementParserTest::defaultTopicListsOKCrashOnFailure();
  TopicElementParserTest::defaultPdfsOKCrashOnFailure();
  return true;
}

bool TopicElementParserTest::defaultTopicListsOKCrashOnFailure() {
  TopicElementParser::Test tester;
  if (!tester.defaultTopicListsOK()) {
    global.fatal
    << "Topic list tests failed. "
    << tester.comments.str()
    << global.fatal;
  }
  return false;
}

bool TopicElementParserTest::defaultPdfsOKCrashOnFailure() {
  TopicElementParser::Test tester;
  int whichTopic = 0;
  if (!tester.defaultPdfsOK(whichTopic)) {
    std::stringstream topicBuildCommand;
    topicBuildCommand << "BuildSlidesInTopicList(" << whichTopic + 1 << ")";
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

bool CalculatorHTMLTest::all() {
  CalculatorHTMLTest::parsingTest();
  CalculatorHTMLTest::builtInCrashOnFailure();
  return true;
}

bool CalculatorHTMLTest::parsingTest() {
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

bool CalculatorHTMLTest::builtInCrashOnFailure() {
  if (!TopicElementParserTest::hasEducationalMaterials()) {
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
    FileOperations::writeFileVirtual(
      CalculatorHTML::Test::filenameFailedInterpretation,
      comments.str(),
      nullptr
    );
    global
    << "Error report dumped in:\n"
    << "https://localhost:8166/" +
    CalculatorHTML::Test::filenameFailedInterpretation
    << Logger::endL;
    global.fatal << "Built-in problem tests failed. " << global.fatal;
  }
  std::string actualOutputString = actualOutput.toStringPretty();
  if (actualOutputString != desiredOutputString) {
    if (desiredResultKnown) {
      std::string newFileName =
      CalculatorHTML::Test::filenameFullOutput + ".new";
      FileOperations::writeFileVirtual(
        newFileName, actualOutputString, nullptr
      );
      global
      << Logger::red
      << "Problem interpretation changed. "
      << Logger::endL;
      global.fatal
      << "Detected change in the expected problem "
      << "content and the actual one. Wrote the new output as "
      << newFileName
      << ", please use a diff tool to see the difference. "
      << "When you have inspected the difference and the new behavior "
      << "looks OK, simply delete file "
      << CalculatorHTML::Test::filenameFullOutput
      << " and rerun the unit tests, along the lines of "
      << "`./calculator test problems` "
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

bool CourseTest::all() {
  STACK_TRACE("CourseTest::all");
  global << "Testing course setup. " << Logger::endL;
  CourseTest::setDeadlines(DatabaseType::fallback);
  CourseTest::setDeadlines(DatabaseType::internal);
  return true;
}

bool CourseTest::setDeadlines(DatabaseType databaseType) {
  STACK_TRACE("CourseTest::setDeadlines");
  CourseTest::Setup setup(databaseType);
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
  QueryUpdateTest::findExactlyOneNoFail(finder, resultData);
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

void CalculatorFunctionsFreecalcTest::all() {
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
  CalculatorFunctionsFreecalcTest::crawl();
}

void CalculatorFunctionsFreecalcTest::crawl() {
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

CourseTest::Setup::Setup(DatabaseType databaseType):
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

bool CourseTest::Setup::setupAll() {
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
} // namespace: Testing.
