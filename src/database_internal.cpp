#include "database.h"
#include "string_constants.h"

std::string LocalDatabase::folder() {
  return "database/" + DatabaseStrings::databaseName + "/";
}

void LocalDatabase::createHashIndex(
  const std::string& collectionName, const std::string& key
) {
  (void) collectionName;
  (void) key;
  global.fatal << "Not implemented yet." << global.fatal;
}

void LocalDatabase::initialize() {}

bool LocalDatabase::fetchCollectionNames(
  List<std::string>& output, std::stringstream* commentsOnFailure
) {
  (void) output;
  (void) commentsOnFailure;
  global.fatal << "Not implemented yet." << global.fatal;
  return false;
}

bool LocalDatabase::findOneWithOptions(
  const QueryExact& query,
  const QueryResultOptions& options,
  JSData& output,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive
) {
  (void) query;
  (void) options;
  (void) output;
  (void) commentsOnFailure;
  (void) commentsGeneralNonSensitive;
  global.fatal << "Not implemented yet" << global.fatal;
  return false;
}

bool LocalDatabase::updateOne(
  const QueryExact& findQuery,
  const QuerySet& updateQuery,
  std::stringstream* commentsOnFailure
) {
  (void) findQuery;
  (void) updateQuery;
  (void) commentsOnFailure;
  global.fatal << "Not implemented yet" << global.fatal;
  return false;
}

bool LocalDatabase::findOneFromSome(
  const List<QueryExact>& findOrQueries,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  (void) findOrQueries;
  (void) output;
  (void) commentsOnFailure;
  global.fatal << "Not implemented yet" << global.fatal;
  return false;
}

bool LocalDatabase::deleteDatabase(std::stringstream* commentsOnFailure) {
  (void) commentsOnFailure;
  global.fatal << "Not implemented yet" << global.fatal;
  return false;
}

bool LocalDatabase::findFromJSONWithOptions(
  const QueryExact& findQuery,
  List<JSData>& output,
  const QueryResultOptions& options,
  int maximumOutputItems,
  long long* totalItems,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive
) {
  (void) findQuery;
  (void) output;
  (void) options;
  (void) maximumOutputItems;
  (void) totalItems;
  (void) commentsOnFailure;
  (void) commentsGeneralNonSensitive;
  global.fatal << "Not implemented yet" << global.fatal;
  return false;
}

bool LocalDatabase::updateOneFromSome(
  const List<QueryExact>& findOrQueries,
  const QuerySet& updateQuery,
  std::stringstream* commentsOnFailure
) {
  (void) findOrQueries;
  (void) updateQuery;
  (void) commentsOnFailure;
  global.fatal << "Not implemented yet" << global.fatal;
  return false;
}
