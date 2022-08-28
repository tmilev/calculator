// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "database.h"
#include "string_constants.h"
#include "crypto.h"

std::string Database::Test::adminPassword = "111";
Database::Test::Test(bool useFallbackDatabase) {
  this->maintainServerForkFlag.initialize(global.flagServerForkedIntoWorker);
  this->maintainerDatabase.initialize(global.flagDatabaseCompiled);
  if (useFallbackDatabase) {
    global.flagDatabaseCompiled = false;
  }
  global.flagServerForkedIntoWorker = true;
  DatabaseStrings::databaseName = "calculatortest";
  Database::FallBack::databaseFilename = "test/test_database.json";
  Database::get().initializeServer();
  Database::get().initializeWorker();
}

bool Database::Test::all() {
  STACK_TRACE("Database::Test::all");
  Database::Test::basics(false);
  Database::Test::basics(true);
  return true;
}

bool Database::Test::basics(bool useFallbackDatabase) {
  STACK_TRACE("Database::Test::basics");
  global
  << "Testing default database. "
  << Database::toString()
  << Logger::endL;
  Database::Test tester(useFallbackDatabase);
  tester.deleteDatabase();
  tester.createAdminAccount();
  return true;
}

bool Database::Test::deleteDatabase() {
  std::stringstream commentsOnFailure;
  if (!Database::get().deleteDatabase(&commentsOnFailure)) {
    global
    << Logger::red
    << "Failed to delete database: "
    << commentsOnFailure.str()
    << Logger::endL;
  }
  return true;
}

bool Database::Test::createAdminAccount() {
  STACK_TRACE("Database::Test::createAdminAccount");
  Database::get().initializeServer();
  Crypto::Random::initializeRandomBytesForTesting();
  UserCalculatorData userData;
  userData.username = WebAPI::userDefaultAdmin;
  userData.enteredPassword = Database::Test::adminPassword;
  std::stringstream commentsOnFailure;
  if (
    !Database::get().user.loginViaDatabase(userData, &commentsOnFailure)
  ) {
    global.fatal
    << "Failed to login as administrator on an empty database. "
    << commentsOnFailure.str()
    << global.fatal;
  }
  return true;
}
