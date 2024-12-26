#include "crypto_calculator.h"
#include "database.h"
#include "general_file_operations_encodings.h"
#include "general_strings.h"
#include "string_constants.h"

std::string Database::Test::adminPassword = "111";

std::string Database::Test::testDatabaseName(DatabaseType databaseType) {
  switch (databaseType) {
  case DatabaseType::internal:
    return "test_local";
  case DatabaseType::fallback:
    return "test_fallback";
  case DatabaseType::noDatabaseEveryoneIsAdmin:
    return "bad_folder_name";
  }
  return "";
}

void Database::Test::startDatabase(DatabaseType databaseType) {
  global.databaseType = databaseType;
  global.flagServerForkedIntoWorker = true;
  Crypto::Random::initializeRandomBytesForTesting();
  Database::name = Database::Test::testDatabaseName(databaseType);
  Database::get().initialize(10);
}

Database::Test::Test(DatabaseType inputDatabaseType) {
  this->databaseType = inputDatabaseType;
  this->maintainServerForkFlag.initialize(global.flagServerForkedIntoWorker);
  this->maintainerDatabase.initialize(global.databaseType);
  this->maintainerDatabaseName.initialize(Database::name);
  global
  << "Unit test: starting database: "
  << Database::name
  << ". "
  << Logger::endL;
  Database::Test::startDatabase(inputDatabaseType);
}

Database::Test::~Test() {
  Database::get().shutdown(nullptr);
  this->deleteDatabase();
  global
  << "Unit test: database deleted: "
  << Database::name
  << ". "
  << Logger::endL;
}

bool Database::Test::all() {
  STACK_TRACE("Database::Test::all");
  Database::Test::basics(DatabaseType::fallback);
  Database::Test::basics(DatabaseType::internal);
  Database::Test::findWithOptions(DatabaseType::fallback);
  Database::Test::findWithOptions(DatabaseType::internal);
  Database::Test::loadFromJSON();
  return true;
}

bool Database::Test::loadFromJSON() {
  STACK_TRACE("Database::Test::loadFromJSON");
  StateMaintainer<std::string> maintainerDatabaseName(Database::name);
  StateMaintainer<DatabaseType> maintainerDatabaseType(global.databaseType);
  global.databaseType = DatabaseType::internal;
  Database::name = Database::Test::testDatabaseName(global.databaseType);
  std::stringstream comments;
  bool mustBeTrue =
  DatabaseLoader::loadDatabase(
    "test/database/test_local.json", false, comments
  );
  if (!mustBeTrue) {
    global.fatal
    << "Loading test database failed. "
    << comments.str()
    << global.fatal;
  }
  DatabaseInternalServer& server = Database::get().localDatabase.server;
  QueryFind query;
  query.nestedLabels.addOnTop(DatabaseStrings::labelId);
  // The id from file test/database/test_local.json
  query.exactValue = "d739b82f2492ed095fa15a52";
  query.collection = "users";
  List<std::string> objectIds;
  LargeInteger totalItems;
  if (
    !server.findObjectIds(query, objectIds, &totalItems, &comments)
  ) {
    global.fatal << "Failed to find object ids. " << global.fatal;
  }
  if (
    objectIds[0] != query.exactValue.stringValue || objectIds.size != 1
  ) {
    global.fatal
    << "Got: "
    << objectIds.toStringCommaDelimited()
    << "\nexpected:\n["
    << query.exactValue
    << "]"
    << global.fatal;
  }
  if (!totalItems.isEqualToOne()) {
    global.fatal
    << "Expected one item, got: "
    << totalItems.toString()
    << global.fatal;
  }
  JSData user;
  mustBeTrue =
  server.loadObject(
    query.exactValue.stringValue, query.collection, user, &comments
  );
  if (!mustBeTrue) {
    global.fatal
    << "Failed to load object: "
    << query.toString()
    << ". "
    << comments.str()
    << global.fatal;
  }
  if (user["username"].stringValue != "default") {
    global.fatal << "Failed to load the default username. " << global.fatal;
  }
  return true;
}

bool Database::Test::findWithOptions(DatabaseType databaseType) {
  STACK_TRACE("Database::Test::findWithOptions");
  global
  << "Testing default database. "
  << Database::toString()
  << Logger::endL;
  Database::Test tester(databaseType);
  tester.createAdminAccount(false);
  QueryFind queryExact;
  queryExact.exactValue = "default";
  queryExact.nestedLabels.addOnTop("username");
  queryExact.collection = "users";
  QueryFindOneOf query;
  query.queries.addOnTop(queryExact);
  QueryResultOptions options;
  options.fieldsProjectedAway.addOnTop("_id");
  List<JSData> result;
  Database::get().find(query, &options, result, nullptr, nullptr);
  JSData admin = result[0];
  if (admin["username"].stringValue != "default") {
    global.fatal << "Failed to find expected admin username. " << global.fatal;
  }
  if (admin.objects.keys.contains("_id")) {
    global.fatal
    << "Options failed to exclude the _id entry. Loaded object: "
    << admin.toString()
    << global.fatal;
  }
  return true;
}

bool Database::Test::basics(DatabaseType databaseType) {
  STACK_TRACE("Database::Test::basics");
  global
  << "Testing default database. "
  << Database::toString()
  << Logger::endL;
  Database::Test tester(databaseType);
  tester.createAdminAccount(false);
  return true;
}

bool Database::Test::noShutdownSignal() {
  global
  << "Database: starting database that will not be shutdown correctly. "
  << Logger::endL;
  Database::Test::startDatabase(DatabaseType::internal);
  Database::Test::createAdminAccount(false);
  global
  << "Database: premature exit without shutdown. "
  << "The database should still shutdown correctly."
  << Logger::endL;
  global
  << Logger::green
  << "If you get a database shutdown message, then all tests passed. "
  << Logger::endL;
  std::exit(0);
  return true;
}

bool Database::Test::deleteDatabase() {
  std::stringstream commentsOnFailure;
  if (!Database::get().shutdown(&commentsOnFailure)) {
    global.fatal
    << Logger::red
    << "Failed to shutdown database: "
    << commentsOnFailure.str()
    << global.fatal;
  }
  if (Database::name != "test_fallback" && Database::name != "test_local") {
    // In case we have a code mess up,
    // let us not accidentally delete the main database sitting on the
    // developer's machine.
    global.fatal
    << "Database deletion is allowed only for test_fallback and test_local."
    << global.fatal;
  }
  std::string folderToRemove;
  FileOperations::getPhysicalFileNameFromVirtual(
    DatabaseInternal::folder(),
    folderToRemove,
    true,
    true,
    &commentsOnFailure
  );
  global.externalCommandStream("rm -rf " + folderToRemove);
  return true;
}

bool Database::Test::createAdminAccountReturnUser(
  bool withEmail, UserCalculatorData& outputUserData
) {
  STACK_TRACE("Database::Test::createAdminAccountReturnUser");
  outputUserData.username = WebAPI::userDefaultAdmin;
  outputUserData.enteredPassword = Database::Test::adminPassword;
  if (withEmail) {
    outputUserData.email = "test.admin.user@calculator-algebra.org";
  }
  std::stringstream commentsOnFailure;
  if (
    !Database::get().user.loginViaDatabase(
      outputUserData, &commentsOnFailure
    )
  ) {
    global.fatal
    << "Failed to login as administrator on an empty database. "
    << commentsOnFailure.str()
    << global.fatal;
  }
  return true;
}

bool Database::Test::createAdminAccount(bool withEmail) {
  STACK_TRACE("Database::Test::createAdminAccount");
  UserCalculatorData userData;
  return Database::Test::createAdminAccountReturnUser(withEmail, userData);
}

bool QueryUpdate::Test::all() {
  QueryUpdate::Test::basics(DatabaseType::internal);
  QueryUpdate::Test::basics(DatabaseType::fallback);
  return true;
}

void QueryUpdate::Test::updateNoFail(QueryFind& find, QueryUpdate updater) {
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

void QueryUpdate::Test::findExactlyOneNoFail(
  QueryFind& find, JSData& result
) {
  std::stringstream comments;
  List<JSData> output;
  QueryFindOneOf wrapperQuery;
  wrapperQuery.queries.addOnTop(find);
  LargeInteger totalItems = 0;
  bool success =
  Database::get().find(
    wrapperQuery, nullptr, output, &totalItems, &comments
  );
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

void QueryUpdate::Test::matchKeyValue(
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

bool QueryUpdate::Test::basics(DatabaseType databaseType) {
  STACK_TRACE("QueryUpdate::Test::basics");
  Database::Test tester(databaseType);
  QueryFind find;
  JSData found;
  JSData expected;
  QueryUpdate updater;
  std::stringstream errorStream;
  find.collection = DatabaseStrings::tableUsers;
  find.nestedLabels.addOnTop(DatabaseStrings::labelUsername);
  find.exactValue = "ttt";
  updater.fromJSONStringNoFail(
    "[{key:[\"username\"], value:\"ttt\"}, "
    "{key:[\"a\", \"b\", \"c\"], value:\"123\"},"
    "{key:[\"instructor\"], value:\"X\"}]"
  );
  QueryUpdate::Test::updateNoFail(find, updater);
  QueryUpdate::Test::findExactlyOneNoFail(find, found);
  expected.parseNoFail(
    "{username:\"ttt\",a:{b:{c:\"123\"}},instructor:\"X\"}", true
  );
  QueryUpdate::Test::matchKeyValue(found, expected);
  updater.fromJSONStringNoFail(
    "[{key:[\"$set\", \"a.b\",\"$set.a.b\"], value:\"123\"}]"
  );
  QueryUpdate::Test::updateNoFail(find, updater);
  QueryUpdate::Test::findExactlyOneNoFail(find, found);
  expected.parseNoFail(
    "{"
    "username:\"ttt\",a:{b:{c:\"123\"}}, "
    "\"$set\": {\"a.b\":{\"$set.a.b\":\"123\"}}"
    "}",
    true
  );
  QueryUpdate::Test::matchKeyValue(found, expected);
  find.exactValue = "ttt2";
  updater.fromJSONStringNoFail(
    "[{key:[\"username\"], value:\"ttt2\"}, "
    "{key:[\"instructor\"], value:\"X\"}]"
  );
  QueryUpdate::Test::updateNoFail(find, updater);
  find.nestedLabels.clear();
  find.nestedLabels.addOnTop("instructor");
  find.exactValue = "X";
  QueryFindOneOf finder;
  finder.queries.addOnTop(find);
  List<JSData> foundAll;
  LargeInteger totalItems = 0;
  bool success =
  Database::get().find(
    finder, nullptr, foundAll, &totalItems, &errorStream
  );
  std::string foundString = found.toString();
  if (!success || foundAll.size != 2) {
    global.fatal
    << "Wrong number of updated items:\n"
    << foundString
    << global.fatal;
  }
  return true;
}
