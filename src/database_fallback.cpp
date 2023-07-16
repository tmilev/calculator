#include "general_logging_global_variables.h"
#include "database.h"
#include "string_constants.h"
#include "crypto_calculator.h"

std::string DatabaseFallback::jsonLocation() {
  return "database/" + DatabaseStrings::databaseName + "/database.json";
}

bool DatabaseFallback::deleteDatabase(std::stringstream* commentsOnFailure) {
  this->databaseContent.reset(JSData::Token::tokenObject);
  if (DatabaseStrings::databaseName != "test") {
    global.fatal
    << "Attempt to delete non-test database: "
    << "["
    << DatabaseStrings::databaseName
    << "]. "
    << "Is this what you actually want to do? "
    << "Assuming this is an accident, "
    << "if not, simply remove the present check. "
    << global.fatal;
  }
  this->indexDatabase(commentsOnFailure);
  if (
    !FileOperations::
    writeFileVirualWithPermissions_accessUltraSensitiveFoldersIfNeeded(
      DatabaseFallback::jsonLocation(),
      "{}",
      true,
      true,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to delete database. ";
    }
    return false;
  }
  return true;
}

bool DatabaseFallback::findOneFromSome(
  const QueryOneOfExactly& findOrQueries,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("FallbackDatabase::findOneFromSome");
  for (const QueryExact& query : findOrQueries.queries) {
    if (this->findOne(query, output, commentsOnFailure)) {
      return true;
    }
  }
  return false;
}

bool DatabaseFallback::updateOneFromSome(
  const QueryOneOfExactly& findOrQueries,
  const QuerySet& updateQuery,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("FallbackDatabase::updateOneFromSome");
  JSData unusedOutput;
  for (const QueryExact& query : findOrQueries.queries) {
    if (!this->findOne(query, unusedOutput, commentsOnFailure)) {
      continue;
    }
    return this->updateOne(query, updateQuery, commentsOnFailure);
  }
  return false;
}

bool DatabaseFallback::updateOne(
  const QueryExact& findQuery,
  const QuerySet& dataToMerge,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("FallbackDatabase::updateOne");
  if (global.hasDisabledDatabaseEveryoneIsAdmin()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "FallbackDatabase::updateOne failed. "
      << DatabaseStrings::errorDatabaseDisabled;
    }
    return false;
  }
  MutexProcesslockGuard guardDatabase(this->access);
  if (!this->readAndIndexDatabase(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read and index database. ";
    }
    global << "Failed to read and index database. " << Logger::endL;
    return false;
  }
  if (
    !this->updateOneNolocks(findQuery, dataToMerge, commentsOnFailure)
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to update one. ";
    }
    return false;
  }
  return this->storeDatabase(commentsOnFailure);
}

bool DatabaseFallback::updateOneNolocks(
  const QueryExact& findQuery,
  const QuerySet& dataToMerge,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseFallback::updateOneNolocks");
  if (!this->hasCollection(findQuery.collection, commentsOnFailure)) {
    return false;
  }
  int index = - 1;
  if (
    !this->findIndexOneNolocksMinusOneNotFound(
      findQuery, index, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to find one entry. ";
    }
    return false;
  }
  if (index == - 1) {
    index = this->databaseContent[findQuery.collection].listObjects.size;
    JSData incoming = findQuery.toJSONCombineKeysAndValue();
    incoming[DatabaseStrings::labelId] =
    Crypto::Random::getRandomHexStringLeaveMemoryTrace(12);
    this->databaseContent[findQuery.collection].listObjects.addOnTop(incoming);
  }
  dataToMerge.mergeData(
    this->databaseContent[findQuery.collection][index]
  );
  return true;
}

bool DatabaseFallback::findOneWithOptions(
  const QueryExact& query,
  const QueryResultOptions& options,
  JSData& output,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive
) {
  (void) commentsGeneralNonSensitive;
  if (options.toJSON().objects.size() > 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Fallback database does not support non-empty query options. "
      << options.toJSON();
    }
    return false;
  }
  return findOne(query, output, commentsOnFailure);
}

bool DatabaseFallback::findOne(
  const QueryExact& query,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("FallbackDatabase::findOne");
  MutexProcesslockGuard guardDB(this->access);
  if (!this->readAndIndexDatabase(commentsOnFailure)) {
    return false;
  }
  int index = - 1;
  if (
    !this->findIndexOneNolocksMinusOneNotFound(
      query, index, commentsOnFailure
    )
  ) {
    return false;
  }
  if (index < 0) {
    return false;
  }
  output = this->databaseContent[query.collection][index];
  return true;
}

bool DatabaseFallback::findFromJSONWithOptions(
  const QueryExact& findQuery,
  List<JSData>& output,
  const QueryResultOptions& options,
  int maximumOutputItems,
  long long* totalItems,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive
) {
  STACK_TRACE("FallbackDatabase::findFromJSONWithOptions");
  if (!this->readAndIndexDatabaseWithLockGuard(commentsOnFailure)) {
    return false;
  }
  if (!this->databaseContent.hasKey(findQuery.collection)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Collection not found: " << findQuery.collection;
    }
    return true;
  }
  output.clear();
  const ListReferences<JSData>& records =
  this->databaseContent[findQuery.collection].listObjects;
  if (totalItems != nullptr) {
    *totalItems = records.size;
  }
  int i = 0;
  for (const JSData& record : records) {
    i ++;
    if (i > maximumOutputItems && maximumOutputItems >= 0) {
      return true;
    }
    output.addOnTop(record);
  }
  (void) options;
  (void) commentsGeneralNonSensitive;
  return true;
}

std::string DatabaseFallback::toStringIndices() const {
  std::stringstream out;
  out << this->indices.size() << " indices total. ";
  int maxIndexedToDisplay = 3;
  for (int i = 0; i < this->indices.size(); i ++) {
    const MapList<
      std::string,
      List<int32_t>,
      HashFunctions::hashFunction<std::string>
    >& currentLocation =
    this->indices.values[i].locations;
    out
    << this->indices.keys[i]
    << ": "
    << currentLocation.size()
    << " indexed: ";
    int numberToDisplay =
    MathRoutines::minimum(currentLocation.size(), maxIndexedToDisplay);
    out << "[";
    for (int j = 0; j < numberToDisplay; j ++) {
      out << currentLocation.keys[j];
      if (j != numberToDisplay - 1) {
        out << ", ";
      }
    }
    if (numberToDisplay < currentLocation.size()) {
      out << ", ...";
    }
    out << "] ";
  }
  out << "Known indices: " << this->knownIndices.toStringCommaDelimited();
  return out.str();
}

bool DatabaseFallback::findIndexOneNolocksMinusOneNotFound(
  const QueryExact& query,
  int& output,
  std::stringstream* commentsOnNotFound
) {
  STACK_TRACE("FallbackDatabase::findIndexOneNolocksMinusOneNotFound");
  output = - 1;
  if (!this->hasCollection(query.collection, commentsOnNotFound)) {
    if (commentsOnNotFound != nullptr) {
      *commentsOnNotFound
      << "Collection "
      << query.collection
      << " not found. ";
    }
    return false;
  }
  std::string key;
  key = StringRoutines::join(query.nestedLabels, ",");
  if (!this->indices.contains(key)) {
    if (commentsOnNotFound != nullptr) {
      *commentsOnNotFound
      << "Finding by non-indexed key: "
      << key
      << " not allowed. Indices allowed: "
      << this->toStringIndices()
      << ". ";
    }
    return false;
  }
  DatabaseFallback::Index& currentIndex =
  this->indices.getValueCreateEmpty(key);
  std::string value;
  int currentLocationIndex = currentIndex.locations.getIndex(value);
  if (currentLocationIndex == - 1) {
    if (commentsOnNotFound != nullptr) {
      *commentsOnNotFound
      << "Entry "
      << query.exactValue.toString(nullptr)
      << " not found. ";
    }
    return true;
  }
  if (currentIndex.locations.values[currentLocationIndex].size == 0) {
    if (commentsOnNotFound != nullptr) {
      *commentsOnNotFound << "Element not found. ";
    }
    return true;
  }
  output = currentIndex.locations.values[currentLocationIndex][0];
  return true;
}

bool DatabaseFallback::fetchCollectionNames(
  List<std::string>& output, std::stringstream* commentsOnFailure
) {
  MutexProcesslockGuard guardDB(this->access);
  if (!this->readDatabase(commentsOnFailure)) {
    return false;
  }
  output.setSize(0);
  output.addListOnTop(this->databaseContent.objects.keys);
  return true;
}

bool DatabaseFallback::hasCollection(
  const std::string& collection, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("FallbackDatabase::hasCollection");
  if (DatabaseFallback::knownCollections.contains(collection)) {
    this->databaseContent[collection].elementType = JSData::Token::tokenArray;
    return true;
  }
  if (this->databaseContent.hasKey(collection)) {
    return true;
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "Database collection "
    << collection
    << " not found. "
    << "Known collections: "
    << this->knownCollections.toStringCommaDelimited()
    << ". ";
  }
  return false;
}

DatabaseFallback::DatabaseFallback() {
  this->owner = nullptr;
  this->initialized = false;
}

void DatabaseFallback::initialize() {
  if (this->initialized) {
    return;
  }
  this->initialized = true;
  this->access.createMe("databaseFallback", false);
  this->knownIndices.addListOnTop({
      DatabaseStrings::tableUsers + "." + DatabaseStrings::labelUsername,
      DatabaseStrings::tableUsers + "." + DatabaseStrings::labelEmail,
      DatabaseStrings::tableEmailInfo + "." + DatabaseStrings::labelEmail,
      DatabaseStrings::tableDeadlines +
      "." +
      DatabaseStrings::labelDeadlinesSchema,

    }
  );
  this->knownCollections.addListOnTop({
      DatabaseStrings::tableUsers,
      DatabaseStrings::tableEmailInfo,
      DatabaseStrings::tableDeadlines,
      DatabaseStrings::tableProblemWeights,
      DatabaseStrings::tableProblemInformation
    }
  );
}

void DatabaseFallback::createHashIndex(
  const std::string& collectionName, const std::string& key
) {
  DatabaseFallback::Index newIndex;
  newIndex.collection = collectionName;
  newIndex.label = key;
  newIndex.collectionAndLabelCache = newIndex.collectionAndLabel();
  this->indices.setKeyValue(newIndex.collectionAndLabelCache, newIndex);
}

std::string DatabaseFallback::Index::collectionAndLabelStatic(
  const std::string& inputCollection, const std::string& inputLabel
) {
  return inputCollection + "." + inputLabel;
}

std::string DatabaseFallback::Index::collectionAndLabel() {
  return this->collectionAndLabelStatic(this->collection, this->label);
}

bool DatabaseFallback::readAndIndexDatabaseWithLockGuard(
  std::stringstream* commentsOnFailure
) {
  MutexProcesslockGuard guardDB(this->access);
  return this->readAndIndexDatabase(commentsOnFailure);
}

bool DatabaseFallback::readAndIndexDatabase(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("FallbackDatabase::readAndIndexDatabase");
  if (!this->readDatabase(commentsOnFailure)) {
    return false;
  }
  return this->indexDatabase(commentsOnFailure);
}

bool DatabaseFallback::indexDatabase(std::stringstream* commentsOnFailure) {
  STACK_TRACE("FallbackDatabase::indexDatabase");
  (void) commentsOnFailure;
  this->indices.clear();
  for (int i = 0; i < this->knownIndices.size; i ++) {
    this->indices.getValueCreateEmpty(this->knownIndices[i]);
  }
  for (int i = 0; i < this->databaseContent.objects.size(); i ++) {
    std::string collection = this->databaseContent.objects.keys[i];
    JSData& currentCollection = this->databaseContent.objects.values[i];
    for (int i = 0; i < currentCollection.listObjects.size; i ++) {
      this->indexOneRecord(
        currentCollection.listObjects[i], i, collection
      );
    }
  }
  // global << "Database indexed. " << this->toStringIndices() << Logger::endL;
  return true;
}

void DatabaseFallback::indexOneRecord(
  const JSData& entry, int32_t row, const std::string& collection
) {
  if (entry.elementType != JSData::Token::tokenObject) {
    return;
  }
  for (int i = 0; i < entry.objects.size(); i ++) {
    std::string indexLabel =
    DatabaseFallback::Index::collectionAndLabelStatic(
      collection, entry.objects.keys[i]
    );
    if (!this->indices.contains(indexLabel)) {
      continue;
    }
    const JSData& keyToIndexBy = entry.objects.values[i];
    if (keyToIndexBy.elementType != JSData::Token::tokenString) {
      continue;
    }
    DatabaseFallback::Index& currentIndex =
    this->indices.getValueCreateEmpty(indexLabel);
    currentIndex.locations.getValueCreateEmpty(keyToIndexBy.stringValue).
    addOnTop(row);
  }
}

bool DatabaseFallback::storeDatabase(std::stringstream* commentsOnFailure) {
  if (!this->indexDatabase(commentsOnFailure)) {
    return false;
  }
  return
  FileOperations::
  writeFileVirualWithPermissions_accessUltraSensitiveFoldersIfNeeded(
    DatabaseFallback::jsonLocation(),
    this->databaseContent.toString(nullptr),
    true,
    true,
    commentsOnFailure
  );
}

bool DatabaseFallback::readDatabase(std::stringstream* commentsOnFailure) {
  std::string database;
  if (
    !FileOperations::
    loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(
      DatabaseFallback::jsonLocation(),
      database,
      true,
      true,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Database load failed. ";
    }
    if (
      !FileOperations::fileExistsVirtual(
        DatabaseFallback::jsonLocation(), true, true, commentsOnFailure
      )
    ) {
      global
      << Logger::green
      << "Fallback database file does not exist. Creating ..."
      << Logger::endL;
      this->databaseContent.elementType = JSData::Token::tokenObject;
      return true;
    }
    return false;
  }
  global
  << "Database size: "
  << Logger::blue
  << database.size()
  << Logger::endL;
  return this->databaseContent.parse(database, commentsOnFailure);
}
