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

void Database::Test::initializeForDatabaseOperations() {
  STACK_TRACE("Database::Test::initializeForDatabaseOperations");
  Database::get().initializeServer();
  Crypto::Random::initializeRandomBytesForTesting();
}

bool Database::Test::createAdminAccount() {
  STACK_TRACE("Database::Test::createAdminAccount");
  this->initializeForDatabaseOperations();
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

bool QuerySet::Test::all() {
  QuerySet::Test::basics(false);
  QuerySet::Test::basics(true);
  return true;
}

void QuerySet::Test::updateNoFail(QueryExact& find, QuerySet updater) {
  std::stringstream comments;
  if (!Database::get().updateOne(find, updater, &comments)) {
    global.fatal
    << "Failed to update using finder:\n"
    << find.toString()
    << "\nupdater:\n"
    << updater.toStringDebug()
    << "\ncomments:\n"
    << comments.str()
    << global.fatal;
  }
}

void QuerySet::Test::findNoFail(QueryExact& find, JSData& result) {
  std::stringstream comments;
  if (!Database::get().findOne(find, result, &comments)) {
    global.fatal
    << "Failed to find updated:\n"
    << find.toString()
    << "\ncomments:\n"
    << comments.str()
    << global.fatal;
  }
}

void QuerySet::Test::matchKeyValue(
  const JSData& mustContain, const JSData& mustBeContained
) {
  JSData empty;
  for (int i = 0; i < mustBeContained.objects.size(); i ++) {
    std::string key = mustBeContained.objects.keys[i];
    JSData contained = mustContain.objects.getValue(key, empty);
    JSData expected = mustBeContained.objects.getValue(key, empty);
    if (contained != expected) {
      global.fatal
      << "Found key: "
      << key
      << "\nwith value:\n"
      << contained
      << "\nExpected:\n"
      << expected
      << "\nFull JSON actually found:\n"
      << mustContain
      << "\nFull JSON expected:\n"
      << mustBeContained
      << global.fatal;
    }
  }
}

bool QuerySet::Test::basics(bool useFallbackDatabase) {
  Database::Test tester(useFallbackDatabase);
  tester.deleteDatabase();
  tester.initializeForDatabaseOperations();
  QueryExact find;
  find.collection = DatabaseStrings::tableUsers;
  find.nestedLabels.addOnTop(DatabaseStrings::labelUsername);
  find.value = "ttt";
  QuerySet updater;
  JSData found;
  updater.value.parseNoFail(
    "{" +
    DatabaseStrings::labelUsername +
    ":\"ttt\", " +
    QueryExact::getLabelFromNestedLabels(
      List<std::string>({"a", "b", "c"})
    ) +
    ": \"123\"}",
    true
  );
  QuerySet::Test::updateNoFail(find, updater);
  QuerySet::Test::findNoFail(find, found);
  JSData expected;
  expected.parseNoFail("{username:\"ttt\",a:{b:{c:\"123\"}}}", true);
  QuerySet::Test::matchKeyValue(found, expected);
  updater.value.parseNoFail(
    "{" +
    DatabaseStrings::labelUsername +
    ":\"ttt\", " +
    QueryExact::getLabelFromNestedLabels(
      List<std::string>({"$set", "a.b", "$set.a.b"})
    ) +
    ": \"123\"}",
    true
  );
  QuerySet::Test::updateNoFail(find, updater);
  QuerySet::Test::findNoFail(find, found);
  expected.parseNoFail(
    "{"
    "username:\"ttt\",a:{b:{c:\"123\"}}, "
    "\"$set\": {\"a.b\":{\"$set.a.b\":\"123\"}}"
    "}",
    true
  );
  QuerySet::Test::matchKeyValue(found, expected);
  return true;
}
