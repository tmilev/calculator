#include "database.h"
#include "string_constants.h"
#include "general_logging_global_variables.h"
#include "general_strings.h"
#include "web_api.h"
#include "general_file_operations_encodings.h"

std::string Database::name = "local";

QueryExact::QueryExact() {
  this->maximumNumberOfItems = 1;
}

QueryExact::QueryExact(
  const std::string& desiredCollection,
  const std::string& label,
  const std::string& desiredValue
) {
  this->collection = desiredCollection;
  this->nestedLabels.addOnTop(label);
  this->exactValue = desiredValue;
  this->maximumNumberOfItems = 1;
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
  this->maximumNumberOfItems = 1;
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

JSData QueryExact::toJSON() const {
  JSData result;
  JSData key;
  key = this->nestedLabels;
  result[DatabaseStrings::labelKey] = key;
  result[DatabaseStrings::labelValue] = this->exactValue;
  result[DatabaseStrings::labelCollection] = this->collection;
  result[DatabaseStrings::maximumNumberOfItems] = this->maximumNumberOfItems;
  return result;
}

bool QueryExact::fromJSON(
  const JSData& source, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("QueryExact::fromJSON");
  (void) commentsOnFailure;
  this->nestedLabels.clear();
  this->exactValue = JSData();
  this->maximumNumberOfItems = 1;
  JSData findQuery = source;
  this->collection = findQuery[DatabaseStrings::labelCollection].stringValue;
  if (this->collection == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Empty collection not allowed.";
    }
    return false;
  }
  if (findQuery.hasKey(DatabaseStrings::maximumNumberOfItems)) {
    findQuery[DatabaseStrings::maximumNumberOfItems].isIntegerFittingInInt(
      &this->maximumNumberOfItems
    );
  }
  JSData nestedKeySource = findQuery[DatabaseStrings::labelKey];
  for (const JSData& key : nestedKeySource.listObjects) {
    std::string label = key.stringValue;
    if (label == "") {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Empty label not allowed.";
      }
      return false;
    }
    this->nestedLabels.addOnTop(label);
  }
  this->exactValue = findQuery[DatabaseStrings::labelValue];
  return true;
}

JSData QueryExact::toJSONCombineKeysAndValue() const {
  JSData result;
  result.elementType = JSData::Token::tokenObject;
  if (this->nestedLabels.size == 0) {
    return result;
  }
  JSData nullObject;
  JSData* current = &result;
  std::string label;
  for (int i = 0; i < this->nestedLabels.size; i ++) {
    label = this->nestedLabels[i];
    (*current)[label] = nullObject;
    if (i != this->nestedLabels.size - 1) {
      current = &(*current)[label];
    }
  }
  (*current)[label] = this->exactValue;
  return result;
}

void QueryExact::setLabelsValue(
  const List<std::string>& labels, const std::string& desiredValue
) {
  this->exactValue = desiredValue;
  this->nestedLabels = labels;
}

std::string QueryExact::concatenateNestedLabels() const {
  return StringRoutines::join(this->nestedLabels, ",");
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
  STACK_TRACE("QueryExact::fromString");
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
      << "When deleting an object, it needs at least "
      << "two labels: table name and objectid. "
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
    List<std::string>({DatabaseStrings::labelId}),
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
    << "deleteOneEntryUnsetUnsecure: "
    << "project compiled without mongoDB support. ";
  }
  return false;
#endif
}

bool Database::findExactlyOne(
  const QueryOneOfExactly& query,
  const QueryResultOptions* options,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  List<JSData> all;
  if (!this->find(query, options, all, commentsOnFailure)) {
    return false;
  }
  if (all.size != 1) {
    return false;
  }
  output = all[0];
  return true;
}

bool Database::find(
  const QueryOneOfExactly& query,
  const QueryResultOptions* options,
  List<JSData>& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Database::find");
  switch (global.databaseType) {
  case DatabaseType::internal:
    return
    this->localDatabase.client.find(
      query, options, output, commentsOnFailure
    );
    return false;
  case DatabaseType::fallback:
    return
    this->fallbackDatabase.client.find(
      query, options, output, commentsOnFailure
    );
  case DatabaseType::noDatabaseEveryoneIsAdmin:
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Error: find failed. "
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
  out << "Database name: [" << Database::name << "]";
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

bool Database::updateOne(
  const QueryExact& findQuery,
  const QuerySet& dataToMerge,
  bool createIfNotFound,
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
    this->fallbackDatabase.client.updateOne(
      findQuery, dataToMerge, createIfNotFound, commentsOnFailure
    );
  case DatabaseType::internal:
    return
    this->localDatabase.client.updateOne(
      findQuery, dataToMerge, createIfNotFound, commentsOnFailure
    );
  }
  return false;
}

bool Database::shutdown(std::stringstream* commentsOnFailure) {
  switch (global.databaseType) {
  case DatabaseType::noDatabaseEveryoneIsAdmin:
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Database::shutdown failed. "
      << DatabaseStrings::errorDatabaseDisabled;
    }
    return false;
  case DatabaseType::fallback:
    if (!this->fallbackDatabase.flagIsRunning) {
      // The database shutdown is already started.
      return true;
    }
    return this->fallbackDatabase.client.shutdown(commentsOnFailure);
  case DatabaseType::internal:
    if (!this->localDatabase.flagIsRunning) {
      // The database is already shutdown.
      // The database shutdown is already started.
      // The server is in a different process and my not have fully shutdown
      // yet.
      return true;
    }
    return this->localDatabase.client.shutdown(commentsOnFailure);
  }
  global.fatal << "This should be unreachable. " << global.fatal;
  return false;
}

bool Database::initialize(int maximumConnections) {
  STACK_TRACE("Database::initializeServer");
  this->user.owner = this;
  this->flagInitializedServer = true;
  if (global.hasDisabledDatabaseEveryoneIsAdmin()) {
    return true;
  }
  switch (global.databaseType) {
  case DatabaseType::noDatabaseEveryoneIsAdmin:
    // should be unreachable.
    return true;
  case DatabaseType::fallback:
    global
    << Logger::red
    << "Using "
    << "**SLOW** "
    << "fall-back JSON storage."
    << Logger::endL;
    this->fallbackDatabase.initializeAsFallback();
    return true;
  case DatabaseType::internal:
    this->localDatabase.initializeForkAndRun(maximumConnections);
    return false;
  }
  return false;
}

bool Database::fetchCollectionNames(
  List<std::string>& output, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Database::fetchCollectionNames");
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
    Database::get().fallbackDatabase.client.fetchCollectionNames(
      output, commentsOnFailure
    );
  case DatabaseType::internal:
    return
    Database::get().localDatabase.client.fetchCollectionNames(
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
  if (totalItems != nullptr) {
    *totalItems = - 1;
  }
  QueryOneOfExactly query;
  QueryExact oneQuery;
  List<JSData> rowsJSON;
  oneQuery.maximumNumberOfItems = 200;
  oneQuery.collection = tableName;
  Database::get().find(query, nullptr, rowsJSON, commentsOnFailure);
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
    result[DatabaseStrings::error] = commentsOnFailure.str();
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
  if (findQuery.nestedLabels.size == 0) {
    return this->toJSONDatabaseCollection(findQuery.collection);
  }
  JSData result;
  result[DatabaseStrings::labelCollection] = findQuery.collection;
  List<JSData> rowsJSON;
  long long totalItems = 0;
  std::stringstream comments;
  QueryOneOfExactly query;
  query.queries.addOnTop(findQuery);
  if (!Database::find(query, &projector, rowsJSON, &comments)) {
    result["error"] = comments.str();
    return result;
  }
  Database::correctData(rowsJSON);
  JSData rows;
  rows.elementType = JSData::Token::tokenArray;
  rows.listObjects = rowsJSON;
  bool flagDebuggingAdmin = global.userDefaultIsDebuggingAdmin();
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

JSData Database::toJSONAllCollections() {
  STACK_TRACE("Database::toJSONAllCollections");
  JSData result;
  std::stringstream out;
  result["currentTable"] = "";
  if (global.userDebugFlagOn() != 0) {
    result[WebAPI::Result::comments] =
    "Requested table empty, returning list of tables. ";
  }
  List<std::string> collectionNamesList;
  if (!Database::fetchCollectionNames(collectionNamesList, &out)) {
    result["error"] = out.str();
    return result;
  }
  JSData collectionNames;
  collectionNames.elementType = JSData::Token::tokenArray;
  collectionNames.listObjects.setSize(collectionNamesList.size);
  for (int i = 0; i < collectionNamesList.size; i ++) {
    collectionNames[i] = collectionNamesList[i];
  }
  result["collections"] = collectionNames;
  return result;
}

JSData Database::toJSONDatabaseCollection(const std::string& currentTable) {
  STACK_TRACE("Database::toJSONDatabaseCollection");
  if (currentTable == "") {
    return this->toJSONAllCollections();
  }
  JSData result;
  std::stringstream comments;
  result["currentTable"] = currentTable;
  QueryExact findQuery;
  QueryResultOptions projector;
  projector = Database::getStandardProjectors(currentTable);
  findQuery.collection = currentTable;
  findQuery.maximumNumberOfItems = 200;
  QueryOneOfExactly query(findQuery);
  List<JSData> rowsJSON;
  long long totalItems = 0;
  std::stringstream* commentsPointer = nullptr;
  bool flagDebuggingAdmin = global.userDefaultIsDebuggingAdmin();
  if (flagDebuggingAdmin) {
    commentsPointer = &comments;
  }
  if (!Database::find(query, &projector, rowsJSON, &comments)) {
    result["error"] = comments.str();
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

JSData QuerySetOnce::toJSON() const {
  JSData result;
  JSData key;
  key = this->nestedLabels;
  result[DatabaseStrings::labelKey] = key;
  result[DatabaseStrings::labelValue] = this->value;
  return result;
}

bool QuerySetOnce::fromJSON(
  const JSData& data, std::stringstream* commentsOnStream
) {
  if (!data.hasKey(DatabaseStrings::labelValue)) {
    if (commentsOnStream != nullptr) {
      *commentsOnStream << "The missing value entry is mandatory.";
    }
    return false;
  }
  this->value = data.getValue(DatabaseStrings::labelValue);
  this->nestedLabels.clear();
  JSData keys = data.getValue(DatabaseStrings::labelKey);
  for (const JSData& key : keys.listObjects) {
    this->nestedLabels.addOnTop(key.stringValue);
  }
  return true;
}

void QuerySetOnce::updateOneEntry(JSData& modified) const {
  STACK_TRACE("QuerySetOnce::updateOneEntry");
  if (this->nestedLabels.size == 0) {
    modified = this->value;
    return;
  }
  JSData* output = &modified;
  for (int i = 0; i < this->nestedLabels.size - 1; i ++) {
    output = &((*output)[this->nestedLabels[i]]);
  }
  output->setKeyValue(*this->nestedLabels.lastObject(), this->value);
}

QuerySet::QuerySet() {}

void QuerySet::fromJSONNoFail(const JSData& inputValue) {
  STACK_TRACE("QuerySet::fromJSONNoFail");
  std::stringstream comments;
  bool mustBeTrue = this->fromJSON(inputValue, &comments);
  if (!mustBeTrue) {
    global.fatal << "From json failed. " << comments.str() << global.fatal;
  }
}

void QuerySet::fromJSONStringNoFail(const std::string& inputValue) {
  std::stringstream comments;
  bool mustBeTrue = this->fromJSONString(inputValue, &comments);
  if (!mustBeTrue) {
    global.fatal
    << "Failed to create query from input: "
    << inputValue
    << ". "
    << global.fatal;
  }
}

bool QuerySet::fromJSONString(
  const std::string& inputValue, std::stringstream* commentsOnFailure
) {
  JSData input;
  if (!input.parse(inputValue, true, commentsOnFailure)) {
    return false;
  }
  return this->fromJSON(input, commentsOnFailure);
}

void QuerySet::addKeyValueStringPair(
  const std::string& key, const std::string& value
) {
  QuerySetOnce querySetOnce;
  querySetOnce.nestedLabels.addOnTop(key);
  querySetOnce.value = value;
  this->data.addOnTop(querySetOnce);
}

void QuerySet::addKeyValuePair(
  const std::string& key, const JSData& value
) {
  QuerySetOnce querySetOnce;
  querySetOnce.nestedLabels.addOnTop(key);
  querySetOnce.value = value;
  this->data.addOnTop(querySetOnce);
}

void QuerySet::addNestedKeyValuePair(
  const List<std::string>& nestedKeys, const JSData& value
) {
  QuerySetOnce querySetOnce;
  querySetOnce.nestedLabels = nestedKeys;
  querySetOnce.value = value;
  this->data.addOnTop(querySetOnce);
}

void QuerySet::addValue(const JSData& value) {
  QuerySetOnce querySetOnce;
  querySetOnce.value = value;
  this->data.addOnTop(querySetOnce);
}

std::string QuerySet::toStringDebug() const {
  std::stringstream out;
  out << this->toJSON().toString(nullptr);
  return out.str();
}

JSData QuerySet::toJSON() const {
  JSData result;
  result.makeEmptyArray();
  for (const QuerySetOnce& set : this->data) {
    result.listObjects.addOnTop(set.toJSON());
  }
  return result;
}

bool QuerySet::fromJSON(
  const JSData& inputValue, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("QuerySet::fromJSON");
  if (inputValue.elementType != JSData::Token::tokenArray) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "QuerySet must be encoded as an array. Your input: "
      << inputValue.toString();
    }
    return false;
  }
  this->data.clear();
  for (const JSData& entry : inputValue.listObjects) {
    QuerySetOnce querySetOnce;
    if (!querySetOnce.fromJSON(entry, commentsOnFailure)) {
      return false;
    }
    this->data.addOnTop(querySetOnce);
  }
  return true;
}

void QuerySet::mergeData(JSData& toMergeInto) const {
  for (const QuerySetOnce& set : this->data) {
    set.updateOneEntry(toMergeInto);
  }
}

void QueryResultOptions::makeProjection(const List<std::string>& fields) {
  this->fieldsToProjectTo = fields;
}

void QueryResultOptions::applyProjection(JSData& input, JSData& output) const {
  if (input.hasNestedKey(this->fieldsToProjectTo, &output)) {
    return;
  }
  output = JSData();
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

QueryFindAndUpdate::QueryFindAndUpdate() {
  this->createIfNotFound = false;
}

JSData QueryFindAndUpdate::toJSON() const {
  JSData result;
  result["find"] = this->find.toJSON();
  result["update"] = this->update.toJSON();
  result["createIfNotFound"] = this->createIfNotFound;
  return result;
}

bool QueryFindAndUpdate::fromJSON(
  JSData& input, std::stringstream* commentsOnFailure
) {
  if (!this->find.fromJSON(input["find"], commentsOnFailure)) {
    return false;
  }
  this->createIfNotFound = false;
  if (input["createIfNotFound"].isTrueRepresentationInJSON()) {
    this->createIfNotFound = true;
  }
  return this->update.fromJSON(input["update"], commentsOnFailure);
}

QueryOneOfExactly::QueryOneOfExactly() {}

QueryOneOfExactly::QueryOneOfExactly(const QueryExact& query) {
  this->queries.addOnTop(query);
}

JSData QueryOneOfExactly::toJSON() const {
  STACK_TRACE("QueryOneOfExactly::toJSON");
  JSData result;
  JSData queryJSON;
  queryJSON.makeEmptyArray();
  for (const QueryExact& query : this->queries) {
    queryJSON.listObjects.addOnTop(query.toJSON());
  }
  result["findMany"] = queryJSON;
  return result;
}

std::string QueryOneOfExactly::collection() const {
  std::string collection = "";
  for (const QueryExact& query : this->queries) {
    if (query.collection == "") {
      return "";
    }
    if (collection == "") {
      collection = query.collection;
    }
    if (collection != query.collection) {
      return "";
    }
  }
  return collection;
}

bool QueryOneOfExactly::fromJSON(
  const JSData& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("QueryOneOfExactly::fromJSON");
  if (!input.hasKey("findMany")) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Missing key findMany.";
    }
    return false;
  }
  const JSData& findMany = input.objects.getValueNoFail("findMany");
  for (const JSData& query : findMany.listObjects) {
    QueryExact queryExact;
    if (!queryExact.fromJSON(query, commentsOnFailure)) {
      return false;
    }
    this->queries.addOnTop(queryExact);
  }
  return true;
}
