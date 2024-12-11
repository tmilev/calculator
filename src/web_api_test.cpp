#include "calculator_html_interpretation.h"
#include "database.h"
#include "general_file_operations_encodings.h"
#include "general_logging_global_variables.h"
#include "json.h"
#include "string_constants.h"
#include "web_api.h"

bool WebAPIResponse::Test::all() {
  StateMaintainer<std::string> databaseName(Database::name);
  WebAPIResponse::Test::addUsersFromData();
  WebAPIResponse::Test::scoredQuiz(DatabaseType::fallback);
  WebAPIResponse::Test::scoredQuiz(DatabaseType::internal);
  WebAPIResponse::Test::solveJSON();
  WebAPIResponse::Test::compareExpressions();
  WebAPIResponse::Test::forgotLogin();
  WebAPIResponse::Test::signUp();
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

bool WebAPIResponse::Test::forgotLogin() {
  STACK_TRACE("WebAPIResponse::Test::forgotLogin");
  Database::Test tester(DatabaseType::internal);
  Database::Test::createAdminAccount(true);
  DatabaseUserRoutines blankUser;
  global.userDefault.reset();
  StateMaintainer<bool> maintainDebugLogin(global.flagDebugLogin);
  global.flagDebugLogin = true;
  std::stringstream unused;
  StateMaintainer<
    MapList<
      std::string,
      std::string,
      HashFunctions::hashFunction<std::string>
    >
  > maintainWebArgument(global.webArguments);
  global.webArguments[DatabaseStrings::labelUsername] =
  WebAPI::userDefaultAdmin;
  global.webArguments[DatabaseStrings::labelEmail] =
  "test.admin.user@calculator-algebra.org";
  global.comments.resetComments();
  WebAPIResponse api;
  JSData forgotLoginJson;
  api.forgotLogin(forgotLoginJson);
  if (forgotLoginJson.hasKey(WebAPI::Result::error)) {
    global.fatal
    << "Failed to change password: "
    << forgotLoginJson
    << global.fatal;
  }
  std::string globalComments = global.comments.getCurrentReset();
  List<std::string> expectedStrings = List<std::string>({
      std::string("noreply@mail2.noreply.calculator.algebra.org"),
      std::string("dummy_mailgun_key")
    }
  );
  for (const std::string& expected : expectedStrings) {
    if (!StringRoutines::stringContains(globalComments, expected)) {global.fatal
    << "Change password: expected comment: ["
    << expected
    << "] not found in: "
    << globalComments
    << ". "
          << global.fatal;}
  }
  UserCalculator userLoader;
  userLoader.email = "test.admin.user@calculator-algebra.org";
  std::stringstream comments;
  if (!userLoader.loadFromDatabase(&comments, &comments)) {
    global.fatal << "Failed to load user from database!" << global.fatal;
  }
  unsigned expectedSize = 24;
  if (userLoader.actualActivationToken.size() != expectedSize) {
    global.fatal
    << "Activation token: "
    << userLoader.actualActivationToken
    << " has "
    << userLoader.actualActivationToken.size()
    << " bytes instead of the expected "
    << expectedSize
    << ". "
    << global.fatal;
  }
  return true;
}

bool WebAPIResponse::Test::signUp() {
  STACK_TRACE("WebAPIResponse::Test::signUp");
  Database::Test tester(DatabaseType::internal);
  Database::Test::createAdminAccount(true);
  DatabaseUserRoutines blankUser;
  global.userDefault.reset();
  StateMaintainer<bool> maintainDebugLogin(global.flagDebugLogin);
  global.flagDebugLogin = true;
  std::stringstream unused;
  StateMaintainer<
    MapList<
      std::string,
      std::string,
      HashFunctions::hashFunction<std::string>
    >
  > maintainWebArgument(global.webArguments);
  global.webArguments[WebAPI::Request::desiredUsername] = "someNewUser";
  global.webArguments[DatabaseStrings::labelEmail] =
  "some.email@calculator-algebra.org";
  global.webArguments[WebAPI::Request::doSendActivationEmail] = "false";
  global.comments.resetComments();
  WebAPIResponse api;
  JSData signUpResult = api.getSignUpRequestResult();
  if (signUpResult.hasKey(WebAPI::Result::error)) {
    global.fatal
    << "Failed to sign up: "
    << signUpResult.toStringPretty()
    << global.fatal;
  }
  std::string globalComments = global.comments.getCurrentReset();
  List<std::string> expectedStrings = List<std::string>({
      std::string("noreply@mail2.noreply.calculator.algebra.org"),
      std::string("dummy_mailgun_key")
    }
  );
  for (const std::string& expected : expectedStrings) {
    if (!StringRoutines::stringContains(globalComments, expected)) {
      global.fatal
      << "Change password: expected comment: ["
      << expected
      << "] not found in: "
      << globalComments
      << ". "
      << global.fatal;
    }
  }
  UserCalculator userLoader;
  userLoader.email = "some.email@calculator-algebra.org";
  std::stringstream comments;
  if (!userLoader.loadFromDatabase(&comments, &comments)) {
    global.fatal << "Failed to load user from database!" << global.fatal;
  }
  unsigned expectedSize = 24;
  if (userLoader.actualActivationToken.size() != expectedSize) {
    global.fatal
    << "Activation token: "
    << userLoader.actualActivationToken
    << " has "
    << userLoader.actualActivationToken.size()
    << " bytes instead of the expected "
    << expectedSize
    << ". "
    << global.fatal;
  }
  // TODO(tmilev): test that the signUp activation link works!
  return true;
}

bool WebAPIResponse::Test::addUsersFromData() {
  STACK_TRACE("WebAPIResponse::Test::addUsersFromData");
  Database::Test tester(DatabaseType::internal);
  Database::Test::createAdminAccount(false);
  StateMaintainer<int64_t> maintainMillisecondsMaxComputation(
    global.millisecondsMaxComputation
  );
  StateMaintainer<int64_t> millisecondsReplyAfterComputation(
    global.millisecondsReplyAfterComputation
  );
  int numberOfNewUsers = 0;
  int numberOfUpdatedUsers = 0;
  std::stringstream comments;
  bool mustBeTrue =
  WebAPIResponse::addUsersFromData(
    "student1,student2@example.com",
    "111,222",
    "student",
    "1",
    numberOfNewUsers,
    numberOfUpdatedUsers,
    &comments
  );
  if (!mustBeTrue) {
    global.fatal
    << "Failed to add users from data: "
    << comments.str()
    << global.fatal;
  }
  UserCalculator user;
  user.username = "student1";
  mustBeTrue = user.loadFromDatabase(nullptr, nullptr);
  if (!mustBeTrue) {
    global.fatal << "Failed to load student1 from database" << global.fatal;
  }
  std::string expectedPassword =
  "DBCziaEqLNTPyS0MdT61CMdxg6ciXUFlrCwNUnx2UWQ=";
  if (user.actualHashedSaltedPassword != expectedPassword) {
    global.fatal
    << "User student1 password: got: ["
    << user.actualHashedSaltedPassword
    << "], expected: ["
    << expectedPassword
    << "]"
    << global.fatal;
  }
  if (user.email != "") {
    global.fatal << "User student1 has wrong email." << global.fatal;
  }
  user.username = "student2@example.com";
  mustBeTrue = user.loadFromDatabase(nullptr, nullptr);
  if (!mustBeTrue) {
    global.fatal << "Failed to load student1 from database" << global.fatal;
  }
  expectedPassword = "uqS7AvF_cZgwXVC6T_o2elF9vWmlrv8j84kRUU-ojoA=";
  if (user.actualHashedSaltedPassword != expectedPassword) {
    global.fatal
    << "User student2@example.com password: got: ["
    << user.actualHashedSaltedPassword
    << "], expected: ["
    << expectedPassword
    << "]"
    << global.fatal;
  }
  if (user.email != "student2@example.com") {
    global.fatal
    << "User student2@example.com has wrong email."
    << global.fatal;
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
  Course::Test::Setup setup(databaseType);
  setup.setupAll();
  std::string sample = "test/problems/interval_notation_1.html";
  global.webArguments[WebAPI::Problem::fileName] = sample;
  global.requestType = WebAPI::Frontend::scoredQuiz;
  std::string databaseBeforeFirstRun =
  Database::get().fallbackDatabase.server.toJSONDatabase().toString();
  JSData resultFirst = WebAPIResponse::getExamPageJSON();
  std::string databaseAfterFirstRun =
  Database::get().fallbackDatabase.server.toJSONDatabase().toString();
  JSData resultSecond = WebAPIResponse::getExamPageJSON();
  std::string afterSecondRun =
  Database::get().fallbackDatabase.server.toJSONDatabase().toString();
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
  QueryFind userQuery;
  userQuery.collection = DatabaseStrings::tableUsers;
  userQuery.nestedLabels.addOnTop(DatabaseStrings::labelUsername);
  userQuery.exactValue = WebAPI::userDefaultAdmin;
  List<JSData> recordedProblems;
  std::stringstream comments;
  LargeInteger totalItems = 0;
  bool success =
  Database::get().find(
    userQuery, nullptr, recordedProblems, &totalItems, &comments
  );
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
