#include "database.h"
#include "string_constants.h"
#include "crypto_calculator.h"

std::string Database::Test::adminPassword = "111";

Database::Test::Test(DatabaseType databaseType) {
  this->maintainServerForkFlag.initialize(global.flagServerForkedIntoWorker);
  this->maintainerDatabase.initialize(global.databaseType);
  this->maintainerDatabaseName.initialize(Database::name);
  global.databaseType = databaseType;
  global.flagServerForkedIntoWorker = true;
  switch (databaseType) {
  case DatabaseType::internal:
    Database::name = "test_internal";
    break;
  case DatabaseType::fallback:
    Database::name = "test_fallback";
    break;
  case DatabaseType::noDatabaseEveryoneIsAdmin:
    Database::name = "bad_folder_name";
    break;
  }
  Database::get().initialize(10);
}

bool Database::Test::all() {
  STACK_TRACE("Database::Test::all");
  Database::Test::basics(DatabaseType::fallback);
  global << "DEBUG: got to here" << Logger::endL;
  Database::Test::basics(DatabaseType::internal);
  return true;
}

bool Database::Test::basics(DatabaseType databaseType) {
  STACK_TRACE("Database::Test::basics");
  global
  << "Testing default database. "
  << Database::toString()
  << Logger::endL;
  Database::Test tester(databaseType);
  tester.deleteDatabase();
  tester.createAdminAccount();
  return true;
}

bool Database::Test::deleteDatabase() {
  std::stringstream commentsOnFailure;
  global << "DEBug: here" << Logger::endL;
  if (!Database::get().shutdown(&commentsOnFailure)) {
    global.fatal
    << Logger::red
    << "Failed to delete database: "
    << commentsOnFailure.str()
    << global.fatal;
  }
  global << "DEBug: shutdown OK" << Logger::endL;
  std::string folderToRemove;
  FileOperations::getPhysicalFileNameFromVirtual(
    DatabaseInternal::folder(),
    folderToRemove,
    true,
    true,
    &commentsOnFailure
  );
  global << "DEBug: about to external command " << Logger::endL;
  global.externalCommandStream("rm -rf " + folderToRemove);
  return true;
}

void Database::Test::initializeForDatabaseOperations() {
  STACK_TRACE("Database::Test::initializeForDatabaseOperations");
  Database::get().initialize(10);
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
  global << "DEBUG: got to here" << Logger::endL;
  QuerySet::Test::basics(DatabaseType::fallback);
  global << "DEBUG: got to here2" << Logger::endL;
  QuerySet::Test::basics(DatabaseType::internal);
  global << "DEBUG: got to here3" << Logger::endL;
  return true;
}

void QuerySet::Test::updateNoFail(QueryExact& find, QuerySet updater) {
  std::stringstream comments;
  if (!Database::get().updateOne(find, updater, true, &comments)) {
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
  List<JSData> output;
  QueryOneOfExactly wrapperQuery;
  wrapperQuery.queries.addOnTop(find);
  bool success =
  Database::get().find(wrapperQuery, nullptr, output, &comments);
  if (!success || output.size != 1) {
    global.fatal
    << "Failed to find updated:\n"
    << find.toString()
    << "\ncomments:\n"
    << comments.str()
    << global.fatal;
  }
  result = output[0];
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

bool QuerySet::Test::basics(DatabaseType databaseType) {
  STACK_TRACE("QuerySet::Test::basics");
  Database::Test tester(databaseType);
  global << "DEBUG: got to here3" << Logger::endL;
  tester.deleteDatabase();
  global << "DEBUG: got to here next" << Logger::endL;
  tester.initializeForDatabaseOperations();
  QueryExact find;
  find.collection = DatabaseStrings::tableUsers;
  find.nestedLabels.addOnTop(DatabaseStrings::labelUsername);
  find.exactValue = "ttt";
  global << "DEBUG: got to here123" << Logger::endL;
  QuerySet updater;
  updater.fromJSONStringNoFail(
    "[{key:[\"username\"], value:\"ttt\"}, "
    "{key:[\"a\", \"b\", \"c\"], value:\"123\"}]"
  );
  QuerySet::Test::updateNoFail(find, updater);
  JSData found;
  QuerySet::Test::findNoFail(find, found);
  JSData expected;
  expected.parseNoFail("{username:\"ttt\",a:{b:{c:\"123\"}}}", true);
  QuerySet::Test::matchKeyValue(found, expected);
  updater.fromJSONStringNoFail(
    "[{key:[\"$set\", \"a.b\",\"$set.a.b\"], value:\"123\"}]"
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
