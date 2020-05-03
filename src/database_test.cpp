// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "database.h"
#include "string_constants.h"

Database::Test::Test() {
  this->setUp();
}

Database::Test::~Test() {
  this->tearDown();
}

void Database::Test::setUp() {
  global.flagServerForkedIntoWorker = true;
  DatabaseStrings::theDatabaseName = "calculatortest";
  Database::FallBack::databaseFilename = "test/test_database.json";
}

void Database::Test::tearDown() {
  global.flagServerForkedIntoWorker = false;
  DatabaseStrings::theDatabaseName = "calculator";
}

bool Database::Test::all() {
  MacroRegisterFunctionWithName("Database::Test::all");
  Database::Test tester;
  tester.deleteDatabase();
  tester.adminAccountCreation();
  return true;
}

bool Database::Test::deleteDatabase() {
  std::stringstream commentsOnFailure;
  if (!Database::get().deleteDatabase(&commentsOnFailure)) {
    global << Logger::red << "Failed to delete database: " << commentsOnFailure.str() << Logger::endL;
  }
  return true;
}

bool Database::Test::adminAccountCreation() {
  MacroRegisterFunctionWithName("Database::Test::adminAccountCreation");
  UserCalculatorData userData;
  userData.username = WebAPI::userDefaultAdmin;
  userData.enteredPassword = "111";
  std::stringstream commentsOnFailure;

  if (!Database::get().theUser.loginViaDatabase(userData, &commentsOnFailure)) {
    global.fatal << "Failed to login as administrator on an empty database. " << global.fatal;
  }
  return true;
}
