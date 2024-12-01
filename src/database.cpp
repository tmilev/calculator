#include "database.h"
#include "general_file_operations_encodings.h"
#include "general_logging_global_variables.h"
#include "general_strings.h"
#include "html_routines.h"
#include "string_constants.h"

std::string Database::name = "local";

QueryFind::QueryFind() {
  this->maximumNumberOfItems = 1;
}

QueryFind::QueryFind(
  const std::string& desiredCollection,
  const std::string& label,
  const std::string& desiredValue
) {
  this->collection = desiredCollection;
  this->nestedLabels.addOnTop(label);
  this->exactValue = desiredValue;
  this->maximumNumberOfItems = 1;
}

QueryFind::QueryFind(
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

bool QueryFind::isEmpty() const {
  if (this->collection == "") {
    return true;
  }
  if (this->nestedLabels.size == 0) {
    return true;
  }
  return false;
}

std::string QueryFind::toString() const {
  std::stringstream out;
  out
  << "collection: "
  << this->collection
  << ", query: "
  << this->toJSON().toString();
  return out.str();
}

JSData QueryFind::toJSON() const {
  JSData result;
  JSData key;
  key = this->nestedLabels;
  result[DatabaseStrings::labelKey] = key;
  result[DatabaseStrings::labelValue] = this->exactValue;
  result[DatabaseStrings::labelCollection] = this->collection;
  result[DatabaseStrings::maximumNumberOfItems] = this->maximumNumberOfItems;
  return result;
}

bool QueryFind::fromJSON(
  const JSData& source, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("QueryFind::fromJSON");
  (void) commentsOnFailure;
  this->exactValue = JSData();
  this->maximumNumberOfItems = 1;
  JSData findQuery = source;
  this->collection = findQuery[DatabaseStrings::labelCollection].stringValue;
  if (this->collection == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Empty collection not allowed. ";
    }
    return false;
  }
  if (findQuery.hasKey(DatabaseStrings::maximumNumberOfItems)) {
    findQuery[DatabaseStrings::maximumNumberOfItems].isIntegerFittingInInt(
      &this->maximumNumberOfItems
    );
  }
  JSData nestedKeySource = findQuery[DatabaseStrings::labelKey];
  if (
    !this->extractNestedKeysFromJSON(nestedKeySource, commentsOnFailure)
  ) {
    return false;
  }
  this->exactValue = findQuery[DatabaseStrings::labelValue];
  return true;
}

bool QueryFind::extractNestedKeysFromJSON(
  const JSData& input, std::stringstream* commentsOnFailure
) {
  this->nestedLabels.clear();
  std::string singleKey;
  if (input.isString(&singleKey)) {
    this->nestedLabels.addOnTop(singleKey);
    return true;
  }
  for (const JSData& key : input.listObjects) {
    std::string label = key.stringValue;
    if (label == "") {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Empty label not allowed.";
      }
      return false;
    }
    this->nestedLabels.addOnTop(label);
  }
  return true;
}

JSData QueryFind::toJSONCombineKeysAndValue() const {
  JSData result;
  result.elementType = JSData::Type::tokenObject;
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

void QueryFind::setLabelsValue(
  const List<std::string>& labels, const std::string& desiredValue
) {
  this->exactValue = desiredValue;
  this->nestedLabels = labels;
}

std::string QueryFind::concatenateNestedLabels() const {
  return StringRoutines::join(this->nestedLabels, ",");
}

void QueryFind::setLabelValue(
  const std::string& label, const std::string& desiredValue
) {
  this->exactValue = desiredValue;
  this->nestedLabels.setSize(1);
  this->nestedLabels[0] = label;
}

bool QueryFind::fromString(
  const std::string& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("QueryFind::fromString");
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
      fieldEntries.listObjects[i].elementType != JSData::Type::tokenString
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
    entry.elementType != JSData::Type::tokenObject ||
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
  QueryFind findQuery(
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
  const QueryFind& findQuery, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Database::deleteOneEntryById");
  (void) findQuery;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "deleteOneEntryById: project compiled without mongoDB support. ";
  }
  return false;
}

bool Database::deleteOneEntryUnsetUnsecure(
  const QueryFind& findQuery,
  List<std::string>& selector,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Database::deleteOneEntryUnsetUnsecure");
  (void) findQuery;
  (void) selector;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "deleteOneEntryUnsetUnsecure: "
    << "project compiled without mongoDB support. ";
  }
  return false;
}

bool Database::findExactlyOne(
  const QueryFindOneOf& query,
  const QueryResultOptions* options,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  List<JSData> all;
  if (!this->find(query, options, all, nullptr, commentsOnFailure)) {
    return false;
  }
  if (all.size != 1) {
    return false;
  }
  output = all[0];
  return true;
}

bool Database::find(
  const QueryFindOneOf& query,
  const QueryResultOptions* options,
  List<JSData>& output,
  LargeInteger* outputTotalItems,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Database::find");
  switch (global.databaseType) {
  case DatabaseType::internal:
    return
    this->localDatabase.client.find(
      query, options, output, outputTotalItems, commentsOnFailure
    );
  case DatabaseType::fallback:
    return
    this->fallbackDatabase.client.find(
      query, options, output, outputTotalItems, commentsOnFailure
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
    // Hard-coded exceptions. The following fields tend to be large.
    result.fieldsProjectedAway.addOnTop(
      DatabaseStrings::labelProblemDataJSON
    );
    result.fieldsProjectedAway.addOnTop("problemData");
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
  const QueryFind& findQuery,
  const QueryUpdate& dataToMerge,
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
  STACK_TRACE("Database::initialize");
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
  LargeInteger* totalItems,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Database::fetchTable");
  if (totalItems != nullptr) {
    *totalItems = - 1;
  }
  QueryFindOneOf query;
  QueryFind oneQuery;
  List<JSData> rowsJSON;
  oneQuery.maximumNumberOfItems = 200;
  oneQuery.collection = tableName;
  Database::get().find(
    query, nullptr, rowsJSON, totalItems, commentsOnFailure
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

JSData Database::toJSONDatabaseFetch(const std::string& findQuery) {
  STACK_TRACE("Database::toJSONDatabaseFetch");
  JSData result;
  QueryFind queryExact;
  std::stringstream commentsOnFailure;
  if (!queryExact.fromString(findQuery, &commentsOnFailure)) {
    result[DatabaseStrings::error] = commentsOnFailure.str();
    return result;
  }
  QueryResultOptions projector;
  return Database::get().toJSONFetchItem(queryExact, projector);
}

JSData Database::toJSONFetchItem(
  QueryFind& findQuery, QueryResultOptions& projector
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
  LargeInteger totalItems = 0;
  std::stringstream comments;
  QueryFindOneOf query;
  query.queries.addOnTop(findQuery);
  if (
    !Database::find(query, &projector, rowsJSON, &totalItems, &comments)
  ) {
    result[DatabaseStrings::error] =
    "Failure in toJSONFetchItem. " + comments.str();
    return result;
  }
  Database::correctData(rowsJSON);
  JSData rows;
  rows.elementType = JSData::Type::tokenArray;
  rows.listObjects = rowsJSON;
  bool flagDebuggingAdmin = global.userDefaultIsDebuggingAdmin();
  if (flagDebuggingAdmin) {
    result["findQuery"] = findQuery.toJSON();
  }
  result["rows"] = rows;
  result[DatabaseStrings::labelTotalRows] = totalItems;
  return result;
}

void Database::correctData(List<JSData>& toBeCorrected) {
  for (int i = 0; i < toBeCorrected.size; i ++) {
    Database::correctData(toBeCorrected[i]);
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
    result[DatabaseStrings::error] = out.str();
    return result;
  }
  JSData collectionNames;
  collectionNames.elementType = JSData::Type::tokenArray;
  collectionNames.listObjects.setSize(collectionNamesList.size);
  for (int i = 0; i < collectionNamesList.size; i ++) {
    collectionNames[i] = collectionNamesList[i];
  }
  result["collections"] = collectionNames;
  result[DatabaseStrings::labelTotalRows] = collectionNamesList.size;
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
  QueryFind findQuery;
  QueryResultOptions projector;
  projector = Database::getStandardProjectors(currentTable);
  findQuery.collection = currentTable;
  findQuery.maximumNumberOfItems = 200;
  QueryFindOneOf query(findQuery);
  List<JSData> rowsJSON;
  LargeInteger totalItems = 0;
  if (
    !Database::find(query, &projector, rowsJSON, &totalItems, &comments)
  ) {
    result[DatabaseStrings::error] = comments.str();
    return result;
  }
  Database::correctData(rowsJSON);
  JSData rows;
  rows.elementType = JSData::Type::tokenArray;
  rows.listObjects = rowsJSON;
  result["rows"] = rows;
  result[DatabaseStrings::labelTotalRows] = totalItems;
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
  LargeInteger totalItems = - 1;
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
  "/LogFiles/logs/" + GlobalVariables::getDateForLogFiles() + "/workers.html";
  this->logs.server.fileName =
  "/LogFiles/logs/" + GlobalVariables::getDateForLogFiles() + "/server.html";
  this->logs.serverMonitor.fileName =
  "/LogFiles/logs/" +
  GlobalVariables::getDateForLogFiles() +
  "/servermonitor.html";
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

JSData QueryUpdateOnce::toJSON() const {
  JSData result;
  JSData key;
  key = this->nestedLabels;
  result[DatabaseStrings::labelKey] = key;
  result[DatabaseStrings::labelValue] = this->value;
  return result;
}

bool QueryUpdateOnce::fromJSON(
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

void QueryUpdateOnce::updateOneEntry(JSData& modified) const {
  STACK_TRACE("QueryUpdateOnce::updateOneEntry");
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

QueryUpdate::QueryUpdate() {}

void QueryUpdate::fromJSONNoFail(const JSData& inputValue) {
  STACK_TRACE("QueryUpdate::fromJSONNoFail");
  std::stringstream comments;
  bool mustBeTrue = this->fromJSON(inputValue, &comments);
  if (!mustBeTrue) {
    global.fatal << "From json failed. " << comments.str() << global.fatal;
  }
}

void QueryUpdate::fromJSONStringNoFail(const std::string& inputValue) {
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

bool QueryUpdate::fromJSONString(
  const std::string& inputValue, std::stringstream* commentsOnFailure
) {
  JSData input;
  if (!input.parse(inputValue, true, commentsOnFailure)) {
    return false;
  }
  return this->fromJSON(input, commentsOnFailure);
}

void QueryUpdate::addKeyValueStringPair(
  const std::string& key, const std::string& value
) {
  QueryUpdateOnce querySetOnce;
  querySetOnce.nestedLabels.addOnTop(key);
  querySetOnce.value = value;
  this->data.addOnTop(querySetOnce);
}

void QueryUpdate::addKeyValuePair(
  const std::string& key, const JSData& value
) {
  QueryUpdateOnce querySetOnce;
  querySetOnce.nestedLabels.addOnTop(key);
  querySetOnce.value = value;
  this->data.addOnTop(querySetOnce);
}

void QueryUpdate::addNestedKeyValuePair(
  const List<std::string>& nestedKeys, const JSData& value
) {
  QueryUpdateOnce querySetOnce;
  querySetOnce.nestedLabels = nestedKeys;
  querySetOnce.value = value;
  this->data.addOnTop(querySetOnce);
}

void QueryUpdate::addValue(const JSData& value) {
  QueryUpdateOnce querySetOnce;
  querySetOnce.value = value;
  this->data.addOnTop(querySetOnce);
}

std::string QueryUpdate::toStringDebug() const {
  std::stringstream out;
  out << this->toJSON().toString(nullptr);
  return out.str();
}

JSData QueryUpdate::toJSON() const {
  JSData result;
  result.makeEmptyArray();
  for (const QueryUpdateOnce& set : this->data) {
    result.listObjects.addOnTop(set.toJSON());
  }
  return result;
}

bool QueryUpdate::fromJSON(
  const JSData& inputValue, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("QueryUpdate::fromJSON");
  if (inputValue.elementType != JSData::Type::tokenArray) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "QuerySet must be encoded as an array. Your input: "
      << inputValue.toString();
    }
    return false;
  }
  this->data.clear();
  for (const JSData& entry : inputValue.listObjects) {
    QueryUpdateOnce querySetOnce;
    if (!querySetOnce.fromJSON(entry, commentsOnFailure)) {
      return false;
    }
    this->data.addOnTop(querySetOnce);
  }
  return true;
}

void QueryUpdate::mergeData(JSData& toMergeInto) const {
  for (const QueryUpdateOnce& set : this->data) {
    set.updateOneEntry(toMergeInto);
  }
}

void QueryResultOptions::makeProjection(const List<std::string>& fields) {
  this->fieldsToProjectTo = fields;
}

void QueryResultOptions::applyProjection(
  const JSData& input, JSData& output
) const {
  if (&input == &output) {
    JSData inputCopy;
    inputCopy = input;
    this->applyProjection(inputCopy, output);
    return;
  }
  for (const std::string& field : this->fieldsToProjectTo) {
    if (!input.hasKey(field)) {
      continue;
    }
    output[field] = input.objects.getValueNoFail(field);
  }
  if (this->fieldsToProjectTo.size > 0) {
    return;
  }
  output = input;
  for (const std::string& field : this->fieldsProjectedAway) {
    output.objects.removeKey(field);
  }
}

bool QueryResultOptions::isNonTrivial() const {
  return
  this->fieldsProjectedAway.size > 0 ||
  this->fieldsToProjectTo.size > 0;
}

JSData QueryResultOptions::toJSON() const {
  JSData result;
  result[DatabaseStrings::requestProjectAwayFrom] = this->fieldsProjectedAway;
  result[DatabaseStrings::requestProjectInto] = this->fieldsToProjectTo;
  return result;
}

bool QueryResultOptions::fromJSON(
  const JSData& input, std::stringstream* commentsOnFailure
) {
  JSData inputCopy = input;
  this->fieldsProjectedAway.clear();
  this->fieldsToProjectTo.clear();
  for (
    JSData& field :
    inputCopy[DatabaseStrings::requestProjectAwayFrom].listObjects
  ) {
    if (field.stringValue == "") {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Empty projection field not allowed. ";
      }
      return false;
    }
    this->fieldsProjectedAway.addOnTop(field.stringValue);
  }
  for (
    JSData& field : inputCopy[DatabaseStrings::requestProjectInto].listObjects
  ) {
    if (field.stringValue == "") {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Empty projection field not allowed. ";
      }
      return false;
    }
    this->fieldsToProjectTo.addOnTop(field.stringValue);
  }
  return true;
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

QueryFindOneOf::QueryFindOneOf() {}

QueryFindOneOf::QueryFindOneOf(const QueryFind& query) {
  this->queries.addOnTop(query);
}

JSData QueryFindOneOf::toJSON() const {
  STACK_TRACE("QueryOneOfExactly::toJSON");
  JSData result;
  JSData queryJSON;
  queryJSON.makeEmptyArray();
  for (const QueryFind& query : this->queries) {
    queryJSON.listObjects.addOnTop(query.toJSON());
  }
  result["findMany"] = queryJSON;
  return result;
}

std::string QueryFindOneOf::collection() const {
  std::string collection = "";
  for (const QueryFind& query : this->queries) {
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

bool QueryFindOneOf::fromJSON(
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
    QueryFind queryExact;
    if (!queryExact.fromJSON(query, commentsOnFailure)) {
      return false;
    }
    this->queries.addOnTop(queryExact);
  }
  return true;
}
