#include "database.h"
#include "string_constants.h"

QueryExact::QueryExact() {
  this->selectAny = false;
}

QueryExact::QueryExact(
  const std::string& desiredCollection,
  const std::string& label,
  const std::string& desiredValue
) {
  this->collection = desiredCollection;
  this->nestedLabels.addOnTop(label);
  this->exactValue = desiredValue;
  this->selectAny = false;
}

QueryExact::QueryExact(
  const std::string& desiredCollection,
  const List<std::string>& desiredLabels,
  const std::string& desiredValue
) {
  this->collection = desiredCollection;
  this->exactValue = desiredValue;
  this->nestedLabels.setSize(0);
  this->nestedLabels.addListOnTop(desiredLabels);
  this->selectAny = false;
}

bool QueryExact::isEmpty() const {
  if (this->collection == "") {
    return true;
  }
  if (this->nestedLabels.size == 0) {
    return true;
  }
  return false;
}

std::string QueryExact::toString() const {
  std::stringstream out;
  out
  << "collection: "
  << this->collection
  << ", query: "
  << this->toJSON().toString();
  return out.str();
}

JSData QueryExact::toJSONCombineLabelsAndValue() const {
  JSData result;
  JSData* current = &result;
  JSData unknown;
  std::string label;
  for (int i = 0; i < this->nestedLabels.size - 1; i ++) {
    label = Database::convertStringToMongoKeyString(this->nestedLabels[i]);
    (*current)[label] = unknown;
    current = &((*current)[label]);
  }
  label =
  Database::convertStringToMongoKeyString(
    *this->nestedLabels.lastObject()
  );
  (*current)[label] = this->exactValue;
  return result;
}

JSData QueryExact::toJSON() const {
  JSData result;
  if (this->nestedLabels.size == 0 && this->selectAny) {
    result.elementType = JSData::Token::tokenObject;
    return result;
  }
  JSData encodedKeys;
  if (!Database::convertJSONToJSONMongo(this->exactValue, encodedKeys)) {
    global
    << Logger::red
    << "Failed to convert find query to mongoDB encoding. "
    << Logger::endL;
    result.elementType = JSData::Token::tokenNull;
    return result;
  }
  result.elementType = JSData::Token::tokenObject;
  result[this->getLabel()] = encodedKeys;
  return result;
}

std::string QueryExact::getLabelFromNestedLabels(
  const std::string& first, const std::string& second
) {
  return
  QueryExact::getLabelFromNestedLabels(
    List<std::string>({first, second})
  );
}

std::string QueryExact::getLabelFromNestedLabels(
  const List<std::string>& nestedLabels
) {
  std::stringstream out;
  for (int i = 0; i < nestedLabels.size; i ++) {
    std::string currentKey =
    Database::convertStringToMongoKeyString(nestedLabels[i]);
    out << currentKey;
    if (i != nestedLabels.size - 1) {
      out << ".";
    }
  }
  return out.str();
}

std::string QueryExact::getLabel() const {
  return this->getLabelFromNestedLabels(this->nestedLabels);
}

bool QueryExact::getMongoKeyValue(
  std::string& outputKey, std::string& outputValue
) const {
  std::stringstream out;
  if (this->nestedLabels.size == 0) {
    return false;
  }
  out << this->collection << "." << this->getLabel();
  outputKey = out.str();
  return this->exactValue.isString(&outputValue);
}

void QueryExact::setLabelsValue(
  const List<std::string>& labels, const std::string& desiredValue
) {
  this->exactValue = desiredValue;
  this->nestedLabels = labels;
}

void QueryExact::setLabelValue(
  const std::string& label, const std::string& desiredValue
) {
  this->exactValue = desiredValue;
  this->nestedLabels.setSize(1);
  this->nestedLabels[0] = label;
}

bool QueryExact::fromString(
  const std::string& input, std::stringstream* commentsOnFailure
) {
  JSData querySource;
  if (!querySource.parse(input, false, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to parse query from: "
      << StringRoutines::stringTrimToLengthForDisplay(input, 200);
    }
    return false;
  }
  return this->fromJSON(querySource, commentsOnFailure);
}

bool QueryExact::fromJSON(
  const JSData& source, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("QueryExact::fromJSON");
  (void) commentsOnFailure;
  this->nestedLabels.clear();
  this->exactValue = JSData();
  this->selectAny = false;
  JSData findQuery = source;
  this->collection = findQuery[DatabaseStrings::labelTable].stringValue;
  if (this->collection == "") {
    return true;
  }
  std::string nestedKeySource = findQuery["key"].stringValue;
  std::string desiredValue = findQuery["value"].stringValue;
  StringRoutines::splitExcludeDelimiter(
    nestedKeySource, '.', this->nestedLabels
  );
  return true;
}

bool Database::matchesPattern(
  const List<std::string>& fieldLabel,
  const List<std::string>& pattern
) {
  STACK_TRACE("Database::matchesPattern");
  if (fieldLabel.size != pattern.size) {
    return false;
  }
  for (int i = 0; i < pattern.size; i ++) {
    if (pattern[i] == DatabaseStrings::anyField) {
      continue;
    }
    if (pattern[i] == DatabaseStrings::objectSelector) {
      continue;
    }
    if (fieldLabel[i] != pattern[i]) {
      return false;
    }
  }
  return true;
}

bool Database::isDeleteable(
  const List<std::string>& labels,
  List<std::string>** outputPattern,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Database::isDeleteable");
  for (int i = 0; i < global.databaseModifiableFields.size; i ++) {
    if (
      Database::matchesPattern(
        labels, global.databaseModifiableFields[i]
      )
    ) {
      if (outputPattern != nullptr) {
        *outputPattern = &global.databaseModifiableFields[i];
      }
      return true;
    }
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "Labels: "
    << labels.toStringCommaDelimited()
    << " do not match any modifiable field pattern. ";
  }
  return false;
}

bool Database::getLabels(
  const JSData& fieldEntries,
  List<std::string>& labels,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Database::getLabels");
  labels.setSize(0);
  for (int i = 0; i < fieldEntries.listObjects.size; i ++) {
    if (
      fieldEntries.listObjects[i].elementType != JSData::Token::tokenString
    ) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Label index "
        << i
        << " is not of type string as required. ";
      }
      return false;
    }
    labels.addOnTop(fieldEntries.listObjects[i].stringValue);
  }
  return true;
}

bool Database::isDeleteable(
  const JSData& entry,
  List<std::string>** outputPattern,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Database::isDeleteable");
  if (
    entry.elementType != JSData::Token::tokenObject ||
    !entry.hasKey(DatabaseStrings::labelFields)
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      <<
      "The labels json is required to be an object of the form {fields: [tableName, objectId,...]}. "
      ;
    }
    return false;
  }
  List<std::string> labels;
  if (
    !Database::getLabels(
      entry.objects.getValueNoFail("fields"),
      labels,
      commentsOnFailure
    )
  ) {
    return false;
  }
  return Database::isDeleteable(labels, outputPattern, commentsOnFailure);
}

bool Database::deleteOneEntry(
  const JSData& entry, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Database::deleteOneEntry");
  if (!global.userDefaultHasAdminRights()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Only logged-in admins can delete DB entries.";
    }
    return false;
  }
  List<std::string>* labelTypes = nullptr;
  if (!isDeleteable(entry, &labelTypes, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Entry: "
      << entry.toString(nullptr)
      << " not deleteable.";
    }
    return false;
  }
  List<std::string> labels;
  if (
    !Database::getLabels(
      entry.objects.getValueNoFail(DatabaseStrings::labelFields),
      labels,
      commentsOnFailure
    )
  ) {
    return false;
  }
  if (labels.size < 2) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      <<
      "When deleting an object, it needs at least two labels: table name and objectid. "
      << "Your input appears to have only "
      << labels.size
      << " entries: "
      << labels.toStringCommaDelimited()
      << ". ";
    }
    return false;
  }
  QueryExact findQuery(
    labels[0],
    List<std::string>({
        DatabaseStrings::labelId, DatabaseStrings::objectSelectorMongo
      }
    ),
    labels[1]
  );
  std::string tableName = labels[0];
  if (labels.size == 0) {
    return this->deleteOneEntryById(findQuery, commentsOnFailure);
  }
  return
  Database::deleteOneEntryUnsetUnsecure(findQuery, labels, commentsOnFailure);
}

bool Database::deleteOneEntryById(
  const QueryExact& findQuery, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Database::deleteOneEntryById");
#ifdef MACRO_use_MongoDB
  JSData foundItem;
  if (!this->findOne(findQuery, foundItem, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Query: "
      << findQuery.toJSON().toString(nullptr)
      << " returned no hits in table: "
      << findQuery.collection;
    }
    return false;
  }
  MongoQuery queryInsertIntoDeletedTable;
  JSData deletedEntry;
  deletedEntry["deleted"] = foundItem;
  queryInsertIntoDeletedTable.collectionName = DatabaseStrings::tableDeleted;
  if (
    !queryInsertIntoDeletedTable.insertOne(deletedEntry, commentsOnFailure)
  ) {
    return false;
  }
  MongoQuery query;
  query.findQuery = findQuery.toJSON().toString(nullptr);
  query.collectionName = findQuery.collection;
  return query.removeOne(commentsOnFailure);
#else
  (void) findQuery;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "deleteOneEntryById: project compiled without mongoDB support. ";
  }
  return false;
#endif
}

bool Database::deleteOneEntryUnsetUnsecure(
  const QueryExact& findQuery,
  List<std::string>& selector,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Database::deleteOneEntryUnsetUnsecure");
#ifdef MACRO_use_MongoDB
  std::string selectorString = QueryExact::getLabelFromNestedLabels(selector);
  JSData foundItem;
  QueryResultOptions options;
  options.fieldsToProjectTo.addListOnTop(selector);
  options.fieldsToProjectTo.addOnTop(selectorString);
  bool didFindItem =
  Database::get().findOneWithOptions(
    findQuery, options, foundItem, commentsOnFailure, nullptr
  );
  if (!didFindItem) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Query: "
      << findQuery.toJSON().toString(nullptr)
      << " returned no hits in table: "
      << findQuery.collection;
    }
    return false;
  }
  MongoQuery queryBackUp;
  queryBackUp.collectionName = DatabaseStrings::tableDeleted;
  JSData backUp;
  backUp["deleted"] = foundItem;
  if (!queryBackUp.insertOne(backUp, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to insert backup: "
      << backUp.toString(nullptr);
    }
    return false;
  }
  MongoQuery query;
  query.findQuery = findQuery.toJSON().toString(nullptr);
  query.collectionName = findQuery.collection;
  std::stringstream updateQueryStream;
  updateQueryStream << "{\"$unset\": {\"" << selectorString << "\":\"\"}}";
  query.updateQuery = updateQueryStream.str();
  return query.updateOneNoOptions(commentsOnFailure);
#else
  (void) findQuery;
  (void) selector;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    <<
    "deleteOneEntryUnsetUnsecure: project compiled without mongoDB support. ";
  }
  return false;
#endif
}

bool Database::convertJSONToJSONMongo(
  const JSData& input,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  if (&input == &output) {
    JSData inputCopy = input;
    return
    Database::convertJSONToJSONMongo(inputCopy, output, commentsOnFailure);
  }
  return
  Database::convertJSONToJSONEncodeKeys(
    input, output, 0, true, commentsOnFailure
  );
}

bool Database::convertJSONMongoToJSON(
  const JSData& input,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  if (&input == &output) {
    JSData inputCopy = input;
    return
    Database::convertJSONMongoToJSON(inputCopy, output, commentsOnFailure);
  }
  return
  Database::convertJSONToJSONEncodeKeys(
    input, output, 0, false, commentsOnFailure
  );
}

bool Database::convertJSONToJSONEncodeKeys(
  const JSData& input,
  JSData& output,
  int recursionDepth,
  bool encodeOverDecode,
  std::stringstream* commentsOnFailure
) {
  static const int maximumRecursionDepth = 100;
  if (recursionDepth > maximumRecursionDepth) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Input json is too deeply nested, maximum depth: "
      << maximumRecursionDepth
      << ". ";
    }
    return false;
  }
  output.reset(input.elementType);
  if (input.elementType == JSData::Token::tokenArray) {
    for (int i = 0; i < input.listObjects.size; i ++) {
      JSData nextItem;
      if (
        !Database::convertJSONToJSONEncodeKeys(
          input.listObjects[i],
          nextItem,
          recursionDepth + 1,
          encodeOverDecode,
          commentsOnFailure
        )
      ) {
        return false;
      }
      output.listObjects.addOnTop(nextItem);
    }
    return true;
  }
  if (input.elementType != JSData::Token::tokenObject) {
    output = input;
    return true;
  }
  for (int i = 0; i < input.objects.size(); i ++) {
    JSData nextItem;
    if (
      !Database::convertJSONToJSONEncodeKeys(
        input.objects.values[i],
        nextItem,
        recursionDepth + 1,
        encodeOverDecode,
        commentsOnFailure
      )
    ) {
      return false;
    }
    std::string key;
    if (encodeOverDecode) {
      key =
      Database::convertStringToMongoKeyString(input.objects.keys[i]);
    } else {
      key =
      HtmlRoutines::convertURLStringToNormal(
        input.objects.keys[i], false
      );
    }
    output[key] = nextItem;
  }
  return true;
}

std::string Database::convertStringToMongoKeyString(
  const std::string& input
) {
  std::stringstream out;
  for (unsigned int i = 0; i < input.size(); i ++) {
    if (
      HtmlRoutines::isRepresentedByItselfInURLs(input[i]) && input[i] != '.'
    ) {
      out << input[i];
    } else {
      out << "%";
      int x = input[i];
      out << std::hex << ((x / 16) % 16) << (x % 16) << std::dec;
    }
  }
  return out.str();
}

bool Database::findOneWithOptions(
  const QueryExact& query,
  const QueryResultOptions& options,
  JSData& output,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive
) {
  STACK_TRACE("Database::findOneWithOptions");
  (void) commentsGeneralNonSensitive;
  switch (global.databaseType) {
  case DatabaseType::internal:
    return
    Database::get().localDatabase.findOneWithOptions(
      query,
      options,
      output,
      commentsOnFailure,
      commentsGeneralNonSensitive
    );
    return false;
  case DatabaseType::fallback:
    return
    Database::fallbackDatabase.findOneWithOptions(
      query,
      options,
      output,
      commentsOnFailure,
      commentsGeneralNonSensitive
    );
  case DatabaseType::noDatabaseEveryoneIsAdmin:
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Error: findOneWithOptions failed. "
      << DatabaseStrings::errorDatabaseDisabled;
    }
    return false;
  }
  global.fatal << "This should be unreachable. " << global.fatal;
  return false;
}

QueryResultOptions Database::getStandardProjectors(
  const std::string& collectionName
) {
  QueryResultOptions result;
  if (collectionName == DatabaseStrings::tableUsers) {
    result.fieldsProjectedAway.addOnTop(
      DatabaseStrings::labelProblemDataJSON
    );
  }
  return result;
}

Database& Database::get() {
  static Database result;
  return result;
}

std::string Database::toString() {
  std::stringstream out;
  switch (global.databaseType) {
  case DatabaseType::noDatabaseEveryoneIsAdmin:
    out
    << "No database: everyone is admin. "
    << DatabaseStrings::errorDatabaseDisabled;
    break;
  case DatabaseType::fallback:
    out << "Current database: Fallback. ";
    break;
  case DatabaseType::internal:
    out << "Currrent database: internal. ";
  }
  out << "Database name: [" << DatabaseStrings::databaseName << "]";
  return out.str();
}

Database::~Database() {}

Database::Database() {
  this->flagInitializedServer = false;
  this->databaseInstanceCountMustBeOneOrZero = 0;
}

bool Database::checkInitialization() {
  if (!this->flagInitializedServer) {
    global.fatal
    << "Database not initialized at a place it should be. "
    << global.fatal;
  }
  return true;
}

bool Database::findOne(
  const QueryExact& query,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  QueryResultOptions emptyOptions;
  return
  this->findOneWithOptions(
    query, emptyOptions, output, commentsOnFailure, nullptr
  );
}

bool Database::updateOne(
  const QueryExact& findQuery,
  const QuerySet& dataToMerge,
  std::stringstream* commentsOnFailure
) {
  if (findQuery.isEmpty()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Empty query not allowed";
    }
    return false;
  }
  switch (global.databaseType) {
  case DatabaseType::noDatabaseEveryoneIsAdmin:
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Update one failed. "
      << DatabaseStrings::errorDatabaseDisabled;
    }
    return false;
  case DatabaseType::fallback:
    return
    this->fallbackDatabase.updateOne(
      findQuery, dataToMerge, commentsOnFailure
    );
  case DatabaseType::internal:
    return
    this->localDatabase.updateOne(findQuery, dataToMerge, commentsOnFailure);
  }
  return false;
}

bool Database::findOneFromSome(
  const List<QueryExact>& findOrQueries,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  switch (global.databaseType) {
  case DatabaseType::noDatabaseEveryoneIsAdmin:
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Database::findOneFromSome failed. "
      << DatabaseStrings::errorDatabaseDisabled;
    }
    return false;
  case DatabaseType::fallback:
    return
    this->fallbackDatabase.findOneFromSome(
      findOrQueries, output, commentsOnFailure
    );
  case DatabaseType::internal:
    return
    this->localDatabase.findOneFromSome(
      findOrQueries, output, commentsOnFailure
    );
  }
  global.fatal << "This should be unreachable. " << global.fatal;
  return false;
}

bool Database::deleteDatabase(std::stringstream* commentsOnFailure) {
  switch (global.databaseType) {
  case DatabaseType::noDatabaseEveryoneIsAdmin:
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Database::deleteDatabase failed. "
      << DatabaseStrings::errorDatabaseDisabled;
    }
    return false;
  case DatabaseType::fallback:
    return this->fallbackDatabase.deleteDatabase(commentsOnFailure);
  case DatabaseType::internal:
    return this->localDatabase.deleteDatabase(commentsOnFailure);
  }
  global.fatal << "This should be unreachable. " << global.fatal;
  return false;
}

void Database::createHashIndex(
  const std::string& collectionName, const std::string& key
) {
  STACK_TRACE("Database::createHashIndex");
  switch (global.databaseType) {
  case DatabaseType::noDatabaseEveryoneIsAdmin:
    return;
  case DatabaseType::fallback:
    this->fallbackDatabase.createHashIndex(collectionName, key);
    return;
  case DatabaseType::internal:
    this->localDatabase.createHashIndex(collectionName, key);
    return;
  }
}

bool Database::initializeWorker() {
  STACK_TRACE("Database::initializeWorker");
  if (this->flagInitializedWorker) {
    return true;
  }
  switch (global.databaseType) {
  default:
    break;
  }
  this->createHashIndex(
    DatabaseStrings::tableUsers, DatabaseStrings::labelUsername
  );
  this->createHashIndex(
    DatabaseStrings::tableUsers, DatabaseStrings::labelEmail
  );
  this->createHashIndex(
    DatabaseStrings::tableUsers, DatabaseStrings::labelInstructor
  );
  this->createHashIndex(
    DatabaseStrings::tableUsers, DatabaseStrings::labelUserRole
  );
  this->createHashIndex(
    DatabaseStrings::tableUsers,
    DatabaseStrings::labelProblemDataJSON +
    "." +
    DatabaseStrings::labelProblemFileName
  );
  this->createHashIndex(
    DatabaseStrings::tableProblemInformation,
    DatabaseStrings::labelProblemFileName
  );
  this->createHashIndex(
    DatabaseStrings::tableDeleted, DatabaseStrings::labelUsername
  );
  this->flagInitializedWorker = true;
  return true;
}

bool Database::findFromJSONWithProjection(
  const QueryExact& findQuery,
  List<JSData>& output,
  List<std::string>& fieldsToProjectTo,
  int maxOutputItems,
  long long* totalItems,
  std::stringstream* commentsOnFailure
) {
  QueryResultOptions options;
  options.makeProjection(fieldsToProjectTo);
  return
  Database::findFromJSONWithOptions(
    findQuery,
    output,
    options,
    maxOutputItems,
    totalItems,
    commentsOnFailure
  );
}

bool Database::findFromJSON(
  const QueryExact& findQuery,
  List<JSData>& output,
  int maxOutputItems,
  long long* totalItems,
  std::stringstream* commentsOnFailure
) {
  QueryResultOptions options;
  return
  Database::findFromJSONWithOptions(
    findQuery,
    output,
    options,
    maxOutputItems,
    totalItems,
    commentsOnFailure
  );
}

bool Database::findFromJSONWithOptions(
  const QueryExact& findQuery,
  List<JSData>& output,
  const QueryResultOptions& options,
  int maximumOutputItems,
  long long* totalItems,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive
) {
  STACK_TRACE("Database::findFromJSONWithOptions");
  switch (global.databaseType) {
  case DatabaseType::noDatabaseEveryoneIsAdmin:
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Database::findFromJSONWithOptions fail. "
      << DatabaseStrings::errorDatabaseDisabled;
    }
    return false;
  case DatabaseType::fallback:
    return
    Database::get().fallbackDatabase.findFromJSONWithOptions(
      findQuery,
      output,
      options,
      maximumOutputItems,
      totalItems,
      commentsOnFailure,
      commentsGeneralNonSensitive
    );
  case DatabaseType::internal:
    return
    Database::get().localDatabase.findFromJSONWithOptions(
      findQuery,
      output,
      options,
      maximumOutputItems,
      totalItems,
      commentsOnFailure,
      commentsGeneralNonSensitive
    );
  }
  global.fatal << "This should be unreachable. " << global.fatal;
  return false;
}

bool Database::initializeServer(int maximumConnections) {
  STACK_TRACE("Database::initializeServer");
  this->user.owner = this;
  this->fallbackDatabase.owner = this;
  this->flagInitializedServer = true;
  if (global.hasDisabledDatabaseEveryoneIsAdmin()) {
    return true;
  }
  switch (global.databaseType) {
  case DatabaseType::noDatabaseEveryoneIsAdmin:
    // should be unreachable.
    return true;
  case DatabaseType::fallback:
    DatabaseStrings::databaseName = "local";
    global
    << Logger::red
    << "Using "
    << "**SLOW** "
    << "fall-back JSON storage."
    << Logger::endL;
    this->fallbackDatabase.initialize();
    return true;
  case DatabaseType::internal:
    DatabaseStrings::databaseName = "local";
    this->localDatabase.initializeForkAndRun(maximumConnections);
    return false;
  }
  return false;
}

bool Database::fetchCollectionNames(
  List<std::string>& output, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Database::fetchCollectionNames");
  if (!Database::get().initializeWorker()) {
    return false;
  }
  switch (global.databaseType) {
  case DatabaseType::noDatabaseEveryoneIsAdmin:
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Database::fetchCollectionNames failed. "
      << DatabaseStrings::errorDatabaseDisabled;
    }
    return false;
  case DatabaseType::fallback:
    return
    Database::get().fallbackDatabase.fetchCollectionNames(
      output, commentsOnFailure
    );
  case DatabaseType::internal:
    return
    Database::get().localDatabase.fetchCollectionNames(
      output, commentsOnFailure
    );
  }
  global.fatal << "This code should be unreachable. " << global.fatal;
  return false;
}

bool Database::fetchTable(
  const std::string& tableName,
  List<std::string>& outputLabels,
  List<List<std::string> >& outputRows,
  long long* totalItems,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Database::fetchTable");
  QueryExact findQuery;
  List<JSData> rowsJSON;
  findQuery.selectAny = true;
  findQuery.collection = tableName;
  Database::get().findFromJSON(
    findQuery, rowsJSON, 200, totalItems, commentsOnFailure
  );
  HashedList<std::string> labels;
  for (int i = 0; i < rowsJSON.size; i ++) {
    for (int j = 0; j < rowsJSON[i].objects.size(); j ++) {
      labels.addOnTopNoRepetition(rowsJSON[i].objects.keys[j]);
    }
  }
  outputLabels = labels;
  outputRows.setSize(rowsJSON.size);
  for (int i = 0; i < rowsJSON.size; i ++) {
    outputRows[i].setSize(labels.size);
    for (int j = 0; j < labels.size; j ++) {
      if (rowsJSON[i].objects.contains(labels[j])) {
        outputRows[i][j] = rowsJSON[i].getValue(labels[j]).toString(nullptr);
      } else {
        outputRows[i][j] = "";
      }
    }
  }
  return true;
}

JSData Database::toJSONDatabaseFetch(
  const std::string& findQueryAndProjector
) {
  STACK_TRACE("Database::toJSONDatabaseFetch");
  JSData result;
  QueryExact queryExact;
  QueryResultOptions projector;
  std::stringstream commentsOnFailure;
  if (!queryExact.fromString(findQueryAndProjector, &commentsOnFailure)) {
    result["error"] = commentsOnFailure.str();
    return result;
  }
  return Database::get().toJSONFetchItem(queryExact, projector);
}

JSData Database::toJSONFetchItem(
  QueryExact& findQuery, QueryResultOptions& projector
) {
  STACK_TRACE("Database::toJSONFetchItem");
  if (findQuery.collection == "") {
    return this->toJSONDatabaseCollection("");
  }
  if (findQuery.collection != "" && findQuery.nestedLabels.size == 0) {
    return this->toJSONDatabaseCollection(findQuery.collection);
  }
  JSData result;
  result[DatabaseStrings::labelTable] = findQuery.collection;
  List<JSData> rowsJSON;
  long long totalItems = 0;
  std::stringstream comments;
  std::stringstream* commentsPointer = nullptr;
  bool flagDebuggingAdmin = global.userDefaultIsDebuggingAdmin();
  if (flagDebuggingAdmin) {
    commentsPointer = &comments;
  }
  if (
    !Database::findFromJSONWithOptions(
      findQuery,
      rowsJSON,
      projector,
      200,
      &totalItems,
      &comments,
      commentsPointer
    )
  ) {
    result["error"] = comments.str();
    return result;
  }
  Database::correctData(rowsJSON);
  JSData rows;
  rows.elementType = JSData::Token::tokenArray;
  rows.listObjects = rowsJSON;
  if (flagDebuggingAdmin) {
    result["findQuery"] = findQuery.toJSON();
  }
  result["rows"] = rows;
  result["totalRows"] = static_cast<int>(totalItems);
  return result;
}

void Database::correctData(List<JSData>& toBeCorrected) {
  for (int i = 0; i < toBeCorrected.size; i ++) {
    correctData(toBeCorrected[i]);
  }
}

void Database::correctData(JSData& row) {
  Database::correctDataFromLabels(row, DatabaseStrings::labelEmail);
  Database::correctDataFromLabels(
    row, DatabaseStrings::labelAuthenticationToken
  );
  Database::correctDataFromLabels(row, DatabaseStrings::labelPassword);
}

void Database::correctDataFromLabels(
  JSData& row, const std::string& oneLabel
) {
  Database::correctDataFromLabels(
    row, List<std::string>({oneLabel})
  );
}

void Database::correctDataFromLabels(
  JSData& row, const List<std::string>& labels
) {
  JSData* toBeModified = &row;
  for (int i = 0; i < labels.size; i ++) {
    if (!toBeModified->hasKey(labels[i])) {
      return;
    }
    toBeModified = &((*toBeModified)[labels[i]]);
  }
  std::string value;
  if (!toBeModified->isString(&value)) {
    return;
  }
  (*toBeModified) = StringRoutines::shortenInsertDots(value, 12);
}

JSData Database::toJSONDatabaseCollection(const std::string& currentTable) {
  STACK_TRACE("Database::toJSONDatabaseCollection");
  JSData result;
  std::stringstream out;
  result["currentTable"] = currentTable;
  if (currentTable == "") {
    if (global.userDebugFlagOn() != 0) {
      result[WebAPI::Result::comments] =
      "Requested table empty, returning list of tables. ";
    }
    List<std::string> collectionNamesList;
    if (Database::fetchCollectionNames(collectionNamesList, &out)) {
      JSData collectionNames;
      collectionNames.elementType = JSData::Token::tokenArray;
      collectionNames.listObjects.setSize(collectionNamesList.size);
      for (int i = 0; i < collectionNamesList.size; i ++) {
        collectionNames[i] = collectionNamesList[i];
      }
      result["collections"] = collectionNames;
    } else {
      result["error"] = out.str();
    }
    return result;
  }
  QueryExact findQuery;
  QueryResultOptions projector;
  projector = Database::getStandardProjectors(currentTable);
  findQuery.collection = currentTable;
  findQuery.selectAny = true;
  List<JSData> rowsJSON;
  long long totalItems = 0;
  std::stringstream comments;
  std::stringstream* commentsPointer = nullptr;
  bool flagDebuggingAdmin = global.userDefaultIsDebuggingAdmin();
  if (flagDebuggingAdmin) {
    commentsPointer = &comments;
  }
  if (
    !Database::findFromJSONWithOptions(
      findQuery,
      rowsJSON,
      projector,
      200,
      &totalItems,
      &out,
      commentsPointer
    )
  ) {
    result["error"] = out.str();
    return result;
  }
  Database::correctData(rowsJSON);
  if (rowsJSON.size == 0) {
    result = Database::toJSONDatabaseCollection("");
    if (flagDebuggingAdmin) {
      std::stringstream moreComments;
      moreComments
      << "First query returned 0 rows, fetching all tables instead. ";
      result["moreComments"] = moreComments.str();
      result["commentsOnFirstQuery"] = commentsPointer->str();
      result["currentTableRawOriginal"] = currentTable;
    }
    return result;
  }
  JSData rows;
  rows.elementType = JSData::Token::tokenArray;
  rows.listObjects = rowsJSON;
  result["rows"] = rows;
  result["totalRows"] = static_cast<int>(totalItems);
  return result;
}

std::string Database::toHtmlDatabaseCollection(
  const std::string& currentTable
) {
  STACK_TRACE("Database::toHtmlDatabaseCollection");
  std::stringstream out;
  if (currentTable == "") {
    List<std::string> collectionNames;
    if (this->fetchCollectionNames(collectionNames, &out)) {
      out << "There are " << collectionNames.size << " collections. ";
      for (int i = 0; i < collectionNames.size; i ++) {
        out << "<br>";
        out
        << "<a href=\""
        << global.displayNameExecutable
        << "?request=database&currentDatabaseTable="
        << collectionNames[i]
        << "\">"
        << collectionNames[i]
        << "</a>";
      }
    }
    return out.str();
  }
  out << "Current table: " << currentTable << "<br>";
  List<std::string> labels;
  List<List<std::string> > rows;
  long long totalItems = - 1;
  if (
    !Database::fetchTable(currentTable, labels, rows, &totalItems, &out)
  ) {
    return out.str();
  }
  out << "Total: " << totalItems << ". ";
  if (totalItems > rows.size) {
    out << "Only the first " << rows.size << " are displayed. ";
  }
  out << "<br>" << HtmlRoutines::toHtmlTable(labels, rows, true);
  return out.str();
}

void GlobalVariables::initialize() {
  this->logs.worker.fileName =
  "/LogFiles/logs/" +
  GlobalVariables::getDateForLogFiles() +
  "/logCommon.html";
  this->logs.server.fileName =
  "/LogFiles/logs/" + GlobalVariables::getDateForLogFiles() + "/global.html";
  this->logs.serverMonitor.fileName =
  "/LogFiles/logs/" + GlobalVariables::getDateForLogFiles() + "/global.html";
}

void GlobalVariables::initModifiableDatabaseFields() {
  STACK_TRACE("GlobalVariables::initModifiableDatabaseFields");
  List<List<std::string> >& modifiableData = global.databaseModifiableFields;
  List<std::string> currentEntry;
  modifiableData.reserve(10);
  currentEntry.addOnTop(DatabaseStrings::tableUsers);
  currentEntry.addOnTop(DatabaseStrings::objectSelector);
  currentEntry.addOnTop(DatabaseStrings::labelProblemDataJSON);
  currentEntry.addOnTop(DatabaseStrings::anyField);
  modifiableData.addOnTop(currentEntry);
  currentEntry.setSize(0);
  currentEntry.addOnTop(DatabaseStrings::tableUsers);
  currentEntry.addOnTop(DatabaseStrings::objectSelector);
  modifiableData.addOnTop(currentEntry);
  currentEntry.setSize(0);
  currentEntry.addOnTop(DatabaseStrings::tableEmailInfo);
  currentEntry.addOnTop(DatabaseStrings::objectSelector);
  modifiableData.addOnTop(currentEntry);
  std::fstream outputFile;
  FileOperations::openFileCreateIfNotPresentVirtual(
    outputFile,
    "/calculator_html/modifiable_database_fields.js",
    false,
    true,
    false
  );
  JSData modifiableDataJSON;
  modifiableDataJSON = modifiableData;
  JSData toWrite;
  toWrite["modifiableFields"] = modifiableDataJSON;
  outputFile << "//File automatically generated. Please do not modify.\n";
  outputFile << "\"use strict\";\n";
  outputFile
  << "var modifiableDatabaseData = "
  << toWrite.toString(&JSData::PrintOptions::newLine())
  << ";\n";
  outputFile << "module.exports = {modifiableDatabaseData};";
}

bool Database::updateOneFromSome(
  const List<QueryExact>& findOrQueries,
  const QuerySet& updateQuery,
  std::stringstream* commentsOnFailure
) {
  switch (global.databaseType) {
  case DatabaseType::noDatabaseEveryoneIsAdmin:
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Database::updateOneFromSome failed. "
      << DatabaseStrings::errorDatabaseDisabled;
    }
    return false;
  case DatabaseType::fallback:
    return
    this->fallbackDatabase.updateOneFromSome(
      findOrQueries, updateQuery, commentsOnFailure
    );
  case DatabaseType::internal:
    return
    this->localDatabase.updateOneFromSome(
      findOrQueries, updateQuery, commentsOnFailure
    );
  }
  global.fatal << "This piece of code should be unreachable. " << global.fatal;
  return false;
}

QuerySet::QuerySet() {
  this->value.elementType = JSData::Token::tokenObject;
}

void QuerySet::makeFromRecursive(
  const JSData& input,
  List<std::string>& nestedLabels,
  QuerySet& output
) {
  if (input.objects.size() == 0 && input.listObjects.size == 0) {
    std::string key = QueryExact::getLabelFromNestedLabels(nestedLabels);
    output.value.setKeyValue(key, input);
    return;
  }
  for (const std::string& key : input.objects.keys) {
    nestedLabels.addOnTop(key);
    QuerySet::makeFromRecursive(
      input.objects.getValueNoFail(key), nestedLabels, output
    );
    nestedLabels.removeLastObject();
  }
  for (int i = 0; i < input.listObjects.size; i ++) {
    std::stringstream out;
    out << i;
    nestedLabels.addOnTop(out.str());
    QuerySet::makeFromRecursive(
      input.listObjects[i], nestedLabels, output
    );
    nestedLabels.removeLastObject();
  }
}

void QuerySet::fromJSONNoFail(const JSData& inputValue) {
  std::stringstream comments;
  bool mustBeTrue = this->fromJSON(inputValue, &comments);
  if (!mustBeTrue) {
    global.fatal << "From json failed. " << comments.str() << global.fatal;
  }
}

bool QuerySet::fromJSON(
  const JSData& inputValue, std::stringstream* commentsOnFailure
) {
  List<std::string> nestedLabels;
  QuerySet::makeFromRecursive(inputValue, nestedLabels, *this);
  (void) commentsOnFailure;
  return true;
}

std::string QuerySet::toStringDebug() const {
  std::stringstream out;
  JSData jsonSetMongo;
  if (!this->toJSON(jsonSetMongo)) {
    return out.str();
  }
  out << jsonSetMongo.toString(nullptr);
  return out.str();
}

bool QuerySet::toJSON(JSData& output) const {
  output.reset(JSData::Token::Token::tokenObject);
  output["$set"] = this->value;
  return true;
}

void QueryResultOptions::makeProjection(const List<std::string>& fields) {
  this->fieldsToProjectTo = fields;
}

JSData QueryResultOptions::toJSON() const {
  JSData result;
  JSData fields;
  result.reset(JSData::Token::tokenObject);
  bool found = false;
  for (int i = 0; i < this->fieldsToProjectTo.size; i ++) {
    fields[this->fieldsToProjectTo[i]] = 1;
    found = true;
  }
  for (int i = 0; i < this->fieldsProjectedAway.size; i ++) {
    fields[this->fieldsProjectedAway[i]] = 0;
    found = true;
  }
  if (found) {
    result["projection"] = fields;
  }
  return result;
}

JSData QueryFindAndUpdate::toJSON() const {
  JSData result;
  result["find"] = this->find.toJSON();
  JSData update;
  result["update"] = this->update.toJSON(update);
  return result;
}

bool QueryFindAndUpdate::fromJSON(
  JSData& input, std::stringstream* commentsOnFailure
) {
  if (!this->find.fromJSON(input["find"], commentsOnFailure)) {
    return false;
  }
  return this->update.fromJSON(input["update"], commentsOnFailure);
}
