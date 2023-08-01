#include "web_api.h"
#include "json.h"
#include "general_logging_global_variables.h"
#include "string_constants.h"
#include "database.h"
#include "calculator_html_interpretation.h"

bool WebAPIResponse::Test::all() {
  StateMaintainer<std::string> databaseName(Database::name);
  WebAPIResponse::Test::scoredQuiz(DatabaseType::fallback);
  WebAPIResponse::Test::scoredQuiz(DatabaseType::internal);
  WebAPIResponse::Test::solveJSON();
  WebAPIResponse::Test::compareExpressions();
  return true;
}

bool WebAPIResponse::Test::solveJSON() {
  WebAPIResponse response;
  global.calculator().freeMemory();
  global.calculator().getElement().initialize(
    Calculator::Mode::educational
  );
  global.comments.resetComments();
  global.setWebInput(WebAPI::Request::calculatorInput, "1+1");
  JSData result = response.solveJSON();
  std::string expected =
  "{\"solution\":{\"solution\":{\"input\":\"1+1\",\"solutionSteps\":"
  "[{\"transformation\":\"2\",\"stepType\":\"=\",\"annotations\":"
  "[\"AddRationals\"]}],\"finalExpression\":\"2\",\"comments\":"
  "\"\",\"error\":\"\"}},\"commentsGlobal\":\"\"}";
  std::string got = result.toString();
  if (got != expected) {
    global.fatal
    << "Got solution:\n"
    << got
    << "\nExpected solution:\n"
    << expected
    << "\nWhat I got, escaped:\n"
    << HtmlRoutines::convertStringEscapeNewLinesQuotesBackslashes(got)
    << global.fatal;
  }
  return true;
}

class OneComparisonSet {
public:
  bool compareAll(JSData& input);
  bool doCompareAll();
  std::string desiredAnswer;
  List<std::string> correct;
  List<std::string> incomplete;
  List<std::string> incorrect;
};

class OneComparison {
public:
  std::string given;
  std::string desired;
  bool expectedWrong;
  bool expectedIncomplete;
  OneComparison() {
    this->expectedWrong = false;
    this->expectedIncomplete = false;
  }
  bool compare(bool hideDesiredAnswer);
  std::string toString() const;
};

bool OneComparisonSet::compareAll(JSData& input) {
  this->desiredAnswer = input["desiredAnswer"].stringValue;
  JSData correctJSON = input["correct"];
  for (int i = 0; i < correctJSON.listObjects.size; i ++) {
    this->correct.addOnTop(correctJSON.listObjects[i].stringValue);
  }
  JSData incompleteJSON = input["incomplete"];
  for (int i = 0; i < incompleteJSON.listObjects.size; i ++) {
    this->incomplete.addOnTop(incompleteJSON.listObjects[i].stringValue);
  }
  JSData wrongJSON = input["incorrect"];
  for (int i = 0; i < wrongJSON.listObjects.size; i ++) {
    this->incorrect.addOnTop(wrongJSON.listObjects[i].stringValue);
  }
  return this->doCompareAll();
}

bool OneComparisonSet::doCompareAll() {
  for (int i = 0; i < this->incorrect.size; i ++) {
    OneComparison comparison;
    comparison.desired = this->desiredAnswer;
    comparison.given = this->incorrect[i];
    comparison.expectedWrong = true;
    comparison.expectedIncomplete = false;
    comparison.compare(true);
    comparison.compare(false);
  }
  for (int i = 0; i < this->incomplete.size; i ++) {
    OneComparison comparison;
    comparison.desired = this->desiredAnswer;
    comparison.given = this->incomplete[i];
    comparison.expectedWrong = false;
    comparison.expectedIncomplete = true;
    comparison.compare(true);
    comparison.compare(false);
  }
  for (int i = 0; i < this->correct.size; i ++) {
    OneComparison comparison;
    comparison.desired = this->desiredAnswer;
    comparison.given = this->correct[i];
    comparison.expectedWrong = false;
    comparison.expectedIncomplete = false;
    comparison.compare(true);
    comparison.compare(false);
  }
  return true;
}

std::string OneComparison::toString() const {
  std::stringstream out;
  out
  << "{ given: "
  << this->given
  << ", desired: "
  << this->desired
  << ", expectedWrong: "
  << this->expectedWrong
  << ", expectedIncomplete: "
  << this->expectedIncomplete
  << "}";
  return out.str();
}

bool OneComparison::compare(bool hideDesiredAnswer) {
  STACK_TRACE("OneComparison::compare");
  WebAPIResponse response;
  global.calculator().freeMemory();
  global.calculator().getElement().initialize(
    Calculator::Mode::educational
  );
  global.setWebInput(
    WebAPI::Request::compareExpressionsGiven,
    HtmlRoutines::convertStringToURLString(this->given, false)
  );
  global.setWebInput(
    WebAPI::Request::compareExpressionsDesired,
    HtmlRoutines::convertStringToURLString(this->desired, false)
  );
  JSData result = response.compareExpressions(false);
  std::string areEqual =
  result[WebAPI::Result::ComparisonData::areEqual].toString();
  std::string areEqualAsAnswers =
  result[WebAPI::Result::ComparisonData::areEqualAsAnswers].toString();
  if (this->expectedWrong) {
    if (areEqual != "false" || areEqualAsAnswers != "false") {
      global.fatal
      << "Expected wrong answer for: "
      << this->toString()
      << ", got: "
      << result.toString()
      << global.fatal;
    }
    return true;
  }
  if (this->expectedIncomplete) {
    if (areEqual != "true" || areEqualAsAnswers != "false") {
      global.fatal
      << "Expected incomplete answer for: "
      << this->toString()
      << ", got: "
      << result.toString()
      << global.fatal;
    }
    return true;
  }
  if (areEqual != "true" && areEqualAsAnswers != "true") {
    global.fatal
    << "Expected correct answer for: "
    << this->toString()
    << ", got: "
    << result.toString()
    << global.fatal;
  }
  if (hideDesiredAnswer) {
    if (
      result[WebAPI::Result::ComparisonData::desired].isString(nullptr)
    ) {
      global.fatal
      << "Desired answer must be absent but was given. "
      << global.fatal;
    }
  }
  return true;
}

bool WebAPIResponse::Test::compareExpressions() {
  STACK_TRACE("WebAPIResponse::Test::compareExpressions");
  std::string comparisonJSON;
  std::stringstream commentsOnFailure;
  if (
    !FileOperations::loadFileToStringVirtual(
      "test/compare_expressions.json",
      comparisonJSON,
      false,
      &commentsOnFailure
    )
  ) {
    global.fatal
    << "File compare_expressions.json is missing. "
    << commentsOnFailure.str()
    << global.fatal;
  }
  JSData comparison;
  if (!comparison.parse(comparisonJSON, false, &commentsOnFailure)) {
    global.fatal
    << "Failed to parse compare_expressions.json. "
    << commentsOnFailure.str()
    << global.fatal;
  }
  for (int i = 0; i < comparison.listObjects.size; i ++) {
    OneComparisonSet example;
    example.compareAll(comparison.listObjects[i]);
  }
  return true;
}

bool WebAPIResponse::Test::scoredQuiz(DatabaseType databaseType) {
  STACK_TRACE("WebAPIResponse::Test::scoredQuiz");
  global
  << "Test scored quiz. "
  << Logger::blue
  << Database::toString()
  << Logger::endL;
  Database::Test::deleteDatabase();
  Course::Test::Setup setup(databaseType);
  setup.setupAll();
  std::string sample = "test/problems/interval_notation_1.html";
  global.webArguments[WebAPI::Problem::fileName] = sample;
  global.requestType = WebAPI::Frontend::scoredQuiz;
  std::string databaseBeforeFirstRun =
  Database::get().fallbackDatabase.server.toJSONStoreEntireDatabase().toString(
  );
  JSData resultFirst = WebAPIResponse::getExamPageJSON();
  std::string databaseAfterFirstRun =
  Database::get().fallbackDatabase.server.toJSONStoreEntireDatabase().toString(
  );
  JSData resultSecond = WebAPIResponse::getExamPageJSON();
  std::string afterSecondRun =
  Database::get().fallbackDatabase.server.toJSONStoreEntireDatabase().toString(
  );
  if (resultFirst != resultSecond) {
    global.fatal
    << "Two consecutive scored quiz requests "
    << "resulted in two different problems. First:\n"
    << resultFirst
    << "\nSecond:\n"
    << resultSecond
    << "\nDatabase before start:\n"
    << databaseBeforeFirstRun
    << "\nDatabase before second run:\n"
    << databaseAfterFirstRun
    << "\nDatabase after second run:\n"
    << afterSecondRun
    << global.fatal;
  }
  if (
    !StringRoutines::stringContains(
      resultFirst.toString(), "mathematical"
    )
  ) {
    global.fatal
    << "Test problem expected to contain the string "
    << "'mathematical'. Instead it was:\n"
    << resultFirst.toString()
    << global.fatal;
  }
  JSData problemRecord;
  QueryExact userQuery;
  userQuery.collection = DatabaseStrings::tableUsers;
  userQuery.nestedLabels.addOnTop(DatabaseStrings::labelUsername);
  userQuery.exactValue = WebAPI::userDefaultAdmin;
  List<JSData> recordedProblems;
  std::stringstream comments;
  bool success =
  Database::get().find(userQuery, nullptr, recordedProblems, &comments);
  if (!success) {
    global.fatal << "Failed to find admin user. " << global.fatal;
  }
  std::string randomSeed =
  recordedProblems[0][DatabaseStrings::labelProblemDataJSON][sample][
    "randomSeed"
  ].stringValue;
  if (randomSeed == "") {
    global.fatal
    << "Expected non-empty random seed in problem record:\n"
    << recordedProblems.toString()
    << global.fatal;
  }
  return true;
}
