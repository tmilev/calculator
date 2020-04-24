// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "database.h"
#include "string_constants.h"

Database::Test::Test() {
  this->SetUp();
}

Database::Test::~Test() {
  this->TearDown();
}

void Database::Test::SetUp() {
  global.flagServerForkedIntoWorker = true;
  DatabaseStrings::theDatabaseName = "calculatortest";
  Database::FallBack::databaseFilename = "test/test_database.json";
}

void Database::Test::TearDown() {
  global.flagServerForkedIntoWorker = false;
  DatabaseStrings::theDatabaseName = "calculator";
}

bool Database::Test::all() {
  MacroRegisterFunctionWithName("Database::Test::all");
  Database::Test tester;
  tester.DeleteDatabase();
  tester.AdminAccountCreation();
  return true;
}

bool Database::Test::DeleteDatabase() {
  std::stringstream commentsOnFailure;
  if (!Database::get().DeleteDatabase(&commentsOnFailure)) {
    global << Logger::red << "Failed to delete database: " << commentsOnFailure.str() << Logger::endL;
  }
  return true;
}

bool Database::Test::AdminAccountCreation() {
  MacroRegisterFunctionWithName("Database::Test::AdminAccountCreation");
  UserCalculatorData userData;
  userData.username = WebAPI::userDefaultAdmin;
  userData.enteredPassword = "111";
  std::stringstream commentsOnFailure;

  if (!Database::get().theUser.LoginViaDatabase(userData, &commentsOnFailure)) {
    global.fatal << "Failed to login as administrator on an empty database. " << global.fatal;
  }
  return true;
}
