#include "database.h"
#include "string_constants.h"
#include "crypto_calculator.h"

std::string DatabaseInternal::folder() {
  return "database_internal/" + DatabaseStrings::databaseName + "/";
}

void DatabaseInternal::createHashIndex(
  const std::string& collectionName, const std::string& key
) {
  (void) collectionName;
  (void) key;
  global.fatal
  << "DatabaseInternal::createHashIndex: not implemented yet. "
  << global.fatal;
}

DatabaseInternal::DatabaseInternal() {
  this->processId = - 1;
  this->maximumMessageSize = 65536;
  this->failedToInitialize = true;
}

bool DatabaseInternalClient::fetchCollectionNames(
  List<std::string>& output, std::stringstream* commentsOnFailure
) {
  (void) output;
  (void) commentsOnFailure;
  global.fatal
  << "DatabaseInternal::fetchCollectionNames: not implemented yet. "
  << global.fatal;
  return false;
}

DatabaseInternalClient::DatabaseInternalClient() {
  this->owner = nullptr;
}

bool DatabaseInternalClient::findOneWithOptions(
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
  global.fatal
  << "DatabaseInternal::findOneWithOptions: not implemented yet. "
  << global.fatal;
  return false;
}

bool DatabaseInternalClient::updateOne(
  const QueryExact& findQuery,
  const QuerySet& updateQuery,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternal::updateOne");
  QueryFindAndUpdate queryFindAndUpdate;
  queryFindAndUpdate.find = findQuery;
  queryFindAndUpdate.update = updateQuery;
  DatabaseInternalResult result;
  DatabaseInternalRequest request;
  request.queryFindAndUpdate = queryFindAndUpdate;
  request.requestType = DatabaseInternalRequest::Type::findAndUpdate;
  if (
    !this->owner->sendAndReceiveFromClientToServer(
      request, result, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to send payload to database. ";
    }
    return false;
  }
  return result.isSuccess(commentsOnFailure);
}

std::string DatabaseInternal::toPayLoad(const std::string& input) {
  return input;
}

bool DatabaseInternalClient::findOneFromSome(
  const List<QueryExact>& findOrQueries,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  return false;
}

bool DatabaseInternal::deleteDatabase(std::stringstream* commentsOnFailure) {
  (void) commentsOnFailure;
  global.fatal
  << "DatabaseInternal::deleteDatabase: not implemented yet."
  << global.fatal;
  return false;
}

bool DatabaseInternalClient::findFromJSONWithOptions(
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
  global.fatal
  << "DatabaseInternal::findFromJSONWithOptions: not implemented yet. "
  << global.fatal;
  return false;
}

bool DatabaseInternalClient::updateOneFromSome(
  const List<QueryExact>& findOrQueries,
  const QuerySet& updateQuery,
  std::stringstream* commentsOnFailure
) {
  (void) findOrQueries;
  (void) updateQuery;
  (void) commentsOnFailure;
  global.fatal
  << "DatabaseInternal::updateOneFromSome: not implemented yet. "
  << global.fatal;
  return false;
}

int DatabaseInternal::forkOutDatabase() {
  STACK_TRACE("DatabaseInternal::forkOutDatabase");
  this->processId = ForkCreator::forkProcessAndAcquireRandomness();
  if (this->processId == - 1) {
    global.fatal << "Failed to start database. " << global.fatal;
  }
  if (this->processId != 0) {
    // This is the parent process.
    return this->processId;
  }
  global.logs.logType = GlobalVariables::LogData::type::database;
  return 0;
}

void DatabaseInternal::initializeForkAndRun(int maximumConnections) {
  STACK_TRACE("DatabaseInternal::initializeForkAndRun");
  this->connections.setSize(maximumConnections);
  for (int i = 0; i < maximumConnections; i ++) {
    this->connections[i].create(i);
  }
  this->client.owner = this;
  this->server.owner = this;
  if (this->forkOutDatabase() > 0) {
    // This is the parent process that will ultimately become the client.
    return;
  }
  this->run();
}

bool DatabaseInternal::sendAndReceiveFromClientToServer(
  const DatabaseInternalRequest& input,
  DatabaseInternalResult& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternal::sendAndReceiveFromClientToServer");
  if (
    !this->sendFromClientToServer(
      input.toJSON().toString(), commentsOnFailure
    )
  ) {
    return false;
  }
  return this->receiveInClientFromServer(output, commentsOnFailure);
}

bool DatabaseInternal::sendFromClientToServer(
  const std::string& input, std::stringstream* commentsOnFailure
) {
  DatabaseInternalConnection& connection =
  this->connections[this->currentWorkerId];
  bool result = connection.clientToServer.writeOnceNoFailure(input, 0, true);
  if (!result && commentsOnFailure != nullptr) {
    *commentsOnFailure << "Failed to send bytes to database. ";
  }
  return result;
}

bool DatabaseInternal::receiveInClientFromServer(
  DatabaseInternalResult& output, std::stringstream* commentsOnFailure
) {
  DatabaseInternalConnection& connection =
  this->connections[this->currentWorkerId];
  bool result = connection.serverToClient.readOnceNoFailure(true);
  if (!result) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to receive database result. ";
    }
    return false;
  }
  std::string stringBuffer;
  Crypto::convertListCharsToString(
    connection.serverToClient.buffer, stringBuffer
  );
  return output.fromJSON(stringBuffer, commentsOnFailure);
}

void DatabaseInternal::run() {
  STACK_TRACE("DatabaseInternal::run");
  int totalRuns = 0;
  for (int i = 0; i < this->connections.size; i ++) {
    int readEnd = this->connections[i].clientToServer.pipeEnds[0];
    this->readEnds.addOnTop(readEnd);
    this->mapFromReadEndsToWorkerIds.setKeyValue(readEnd, i);
  }
  std::stringstream commentsOnFailure;
  this->failedToInitialize =
  this->server.initializeLoadFromHardDrive(&commentsOnFailure);
  this->initializationError = commentsOnFailure.str();
  while (true) {
    totalRuns ++;
    global << "DEBUG: total database runs:  " << totalRuns << Logger::endL;
    this->runOneConnection();
  }
  global << "Exit database main loop. " << Logger::endL;
}

bool DatabaseInternal::runOneConnection() {
  STACK_TRACE("DatabaseInternal::runOneConnection");
  int fileDescriptor = 0;
  if (
    !Pipe::readWithTimeOutViaSelect(
      this->readEnds,
      this->buffer,
      fileDescriptor,
      this->maximumMessageSize,
      0
    )
  ) {
    return false;
  }
  this->currentWorkerId =
  this->mapFromReadEndsToWorkerIds.getValueNoFail(fileDescriptor);
  return this->executeAndSend();
}

bool DatabaseInternal::executeAndSend() {
  STACK_TRACE("DatabaseInternal::executeAndSend");
  DatabaseInternalResult result;
  if (this->failedToInitialize) {
    result.success = false;
    result.comments = this->failedToInitialize;
    return
    this->currentServerToClient().writeOnceNoFailure(
      result.toJSON().toString(), 0, true
    );
  }
  DatabaseInternalRequest request;
  std::string requestString;
  Crypto::convertListCharsToString(this->buffer, requestString);
  std::stringstream commentsOnFailure;
  if (!request.fromJSON(requestString, &commentsOnFailure)) {
    result.comments = commentsOnFailure.str();
    result.success = false;
    return
    this->currentServerToClient().writeOnceNoFailure(
      result.toJSON().toString(), 0, true
    );
  }
  result.success = false;
  global
  << "DEBUG: parsed: "
  << request.toJSON().toString()
  << " from "
  << requestString
  << Logger::endL;
  switch (request.requestType) {
  case DatabaseInternalRequest::Type::findAndUpdate:
    result.success =
    this->server.findAndUpdate(
      request.queryFindAndUpdate, &commentsOnFailure
    );
    break;
  default:
    break;
  }
  if (!result.success) {
    result.comments = commentsOnFailure.str();
  }
  return
  this->currentServerToClient().writeOnceNoFailure(
    result.toJSON().toString(), 0, true
  );
}

PipePrimitive& DatabaseInternal::currentServerToClient() {
  return this->connections[this->currentWorkerId].serverToClient;
}

void DatabaseInternalConnection::create(int inputIndexInOwner) {
  this->indexInOwner = inputIndexInOwner;
  std::stringstream clientToServerName;
  std::stringstream serverToClientName;
  clientToServerName << "database_client_to_server_" << indexInOwner;
  serverToClientName << "database_server_to_client_" << indexInOwner;
  if (
    !this->clientToServer.createMe(
      clientToServerName.str(), true, false, false
    ) ||
    !this->serverToClient.createMe(
      clientToServerName.str(), true, false, false
    )
  ) {
    global.fatal
    << "Failed to create database internal connection. "
    << global.fatal;
  }
}

DatabaseInternalConnection::DatabaseInternalConnection() {
  this->indexInOwner = - 1;
}

bool DatabaseInternalResult::isSuccess(std::stringstream* commentsOnFalse)
const {
  if (
    !this->content.objects.contains(DatabaseStrings::resultSuccess)
  ) {
    if (commentsOnFalse != nullptr) {
      *commentsOnFalse
      << "Missing result success entry: "
      << DatabaseStrings::resultSuccess
      << ". Database result: "
      << this->content.toString();
    }
    return false;
  }
  JSData successValue =
  this->content.objects.getValueNoFail(DatabaseStrings::resultSuccess);
  if (successValue.isTrueRepresentationInJSON()) {
    return true;
  }
  if (
    commentsOnFalse != nullptr &&
    this->content.objects.contains(DatabaseStrings::resultComments)
  ) {
    *commentsOnFalse
    << this->content.objects.getValueNoFail(DatabaseStrings::resultComments).
    toString();
  }
  return false;
}

DatabaseInternalResult::DatabaseInternalResult() {
  this->success = false;
}

bool DatabaseInternalResult::fromJSON(
  const std::string& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalResult::fromJSON");
  if (this->content.parse(input, true, commentsOnFailure)) {
    return true;
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Failed to parse database result. ";
  }
  return false;
}

JSData DatabaseInternalResult::toJSON() {
  STACK_TRACE("DatabaseInternalResult::toJSON");
  this->content = JSData();
  this->content[DatabaseStrings::resultSuccess] = this->success;
  this->content[DatabaseStrings::resultComments] = this->comments;
  return this->content;
}

DatabaseInternalRequest::DatabaseInternalRequest() {
  this->requestType = DatabaseInternalRequest::Type::unknown;
}

bool DatabaseInternalRequest::fromJSON(
  const std::string& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalRequest::fromJSON");
  if (!this->contentReader.parse(input, true, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "DatabaseInternalRequest::fromJSON: failed to parse input. ";
    }
    return true;
  }
  return this->fromJSData(commentsOnFailure);
}

void DatabaseInternalRequest::toJSON(JSData& output) const {
  output = JSData();
  output[DatabaseStrings::requestType] =
  this->typeToString(this->requestType);
  if (this->requestType == DatabaseInternalRequest::Type::findAndUpdate) {
    output[DatabaseStrings::requestContent] =
    this->queryFindAndUpdate.toJSON();
    return;
  }
}

JSData DatabaseInternalRequest::toJSON() const {
  JSData result;
  this->toJSON(result);
  return result;
}

std::string DatabaseInternalRequest::toStringForDebugging() const {
  std::stringstream out;
  out << "request: " << this->typeToString(this->requestType);
  return out.str();
}

std::string DatabaseInternalRequest::typeToString(
  DatabaseInternalRequest::Type input
) {
  switch (input) {
  case DatabaseInternalRequest::Type::findAndUpdate:
    return "findAndUpdate";
  case DatabaseInternalRequest::Type::unknown:
    return "unknown";
  }
  global.fatal << "This should be unreachable. " << global.fatal;
  return "";
}

bool DatabaseInternalRequest::fromJSData(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalRequest::fromJSData");
  this->requestType = DatabaseInternalRequest::Type::unknown;
  if (
    this->contentReader[DatabaseStrings::requestType].stringValue ==
    "findAndUpdate"
  ) {
    this->requestType = DatabaseInternalRequest::Type::findAndUpdate;
    return
    this->queryFindAndUpdate.fromJSON(
      this->contentReader[DatabaseStrings::requestContent],
      commentsOnFailure
    );
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "DatabaseInternalRequest::fromJSData: unknown request type. ";
    *commentsOnFailure
    << "DEBUG: failed to get from "
    << this->contentReader.toString();
  }
  return false;
}

DatabaseInternalServer::DatabaseInternalServer() {
  this->owner = nullptr;
}

bool DatabaseInternalServer::findOneWithOptions(
  const QueryExact& query,
  const QueryResultOptions& options,
  JSData& output,
  std::stringstream* commentsOnFailure
) {}

bool DatabaseInternalServer::findObjectId(
  const QueryExact& query,
  std::string& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalServer::findObjectId");
  if (!this->collections.contains(query.collection)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Unknown collection: " << query.collection << ". ";
    }
    return false;
  }
  DatabaseCollection& collection =
  this->collections.getValueNoFailNonConst(query.collection);
  std::string concatenatedLabels = query.concatenateNestedLabels();
  if (!collection.indices.contains(concatenatedLabels)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Attempt to search using a non-indexed key: "
      << concatenatedLabels
      << ". ";
    }
    return false;
  }
  std::string desiredValue = query.exactValue.stringValue;
  if (desiredValue == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "For now, only searching by exact string value is allowed. "
      << "You wanted to search by: "
      << query.toString()
      << ". ";
    }
    return false;
  }
  DatabaseInternalIndex& index =
  collection.indices.getValueNoFailNonConst(concatenatedLabels);
  output = index.keyValueToObjectId.getValue(desiredValue, "");
  return true;
}

bool DatabaseInternalServer::findAndUpdate(
  QueryFindAndUpdate& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalServer::findAndUpdate");
  std::string objectId;
  if (!this->findObjectId(input.find, objectId, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "DatabaseInternalServer::findAndUpdate: failed to find object id. ";
    }
    return false;
  }
  bool createIfNotFound = input.createIfNotFound;
  if (objectId == "" && !createIfNotFound) {
    // Empty object id. We are not allowed to create a new object.
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "DatabaseInternalServer::findAndUpdate: object not found. ";
    }
    return false;
  }
  std::string collectionName = input.find.collection;
  if (objectId == "" && createIfNotFound) {
    // Empty object id. Create the new object.
    JSData initialValue = input.find.toJSONCombineKeysAndValue();
    if (
      !this->createObject(
        initialValue, collectionName, objectId, commentsOnFailure
      )
    ) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "DatabaseInternalServer::findAndUpdate: failed to create object. ";
      }
      return false;
    }
  }
  JSData object;
  if (
    !this->loadObject(objectId, collectionName, object, commentsOnFailure)
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "DatabaseInternalServer::findAndUpdate: failed to load object. ";
    }
    return false;
  }
  input.update.mergeData(object);
  return
  this->storeObject(objectId, collectionName, object, commentsOnFailure);
}

bool DatabaseInternalServer::storeObject(
  const std::string& objectId,
  const std::string& collectionName,
  const JSData& data,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalServer::storeObject");
  if (!this->collections.contains(collectionName)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "DatabaseInternalServer::storeObject: collection not found.";
    }
    return false;
  }
  DatabaseCollection collection =
  this->collections.getValueNoFailNonConst(collectionName);
  std::string fileName = this->objectFilename(objectId, collectionName);
  std::string content = data.toString();
  // As of writing, we have no means of handling the case when a
  // corrupt version of the file is written. This may happen if
  // the database process is killed during a large write operation.
  // Should that happen, either
  // 1) the database will have to be repaired manually or
  // 2) we will need to implement code that autorepairs the damage.
  // We do not expect that to happen in practice, ever.
  // Until (if ever) there is a solid incentive
  // to make object writes failure very
  // resilient, let us leave this as is.
  if (
    !FileOperations::
    writeFileVirualWithPermissions_accessUltraSensitiveFoldersIfNeeded(
      fileName, content, true, true, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to write object to the hard drive.";
    }
    return false;
  }
  // Update the indices with the new found object
  bool found = false;
  for (DatabaseInternalIndex& index : collection.indices.values) {
    JSData primaryKeyValueJSON;
    if (!data.hasNestedKey(index.nestedKeys, &primaryKeyValueJSON)) {
      continue;
    }
    std::string primaryKeyValue = primaryKeyValueJSON.stringValue;
    if (primaryKeyValue == "") {
      continue;
    }
    std::string previousValue =
    index.objectIdToKeyValue.getValue(objectId, "");
    if (previousValue == primaryKeyValue) {
      continue;
    }
    found = true;
    std::string previousObjectId =
    index.keyValueToObjectId.getValue(previousValue, "");
    index.keyValueToObjectId.removeKey(previousObjectId);
    index.objectIdToKeyValue.setKeyValue(objectId, primaryKeyValue);
    index.keyValueToObjectId.setKeyValue(primaryKeyValue, objectId);
  }
  // We store the entire index to the hard drive.
  // This has run time of O(number of records), and should run in low
  // millisecond
  // range for under 100k records.
  // This can be optimized to have approximately constant run time.
  // Please do so if there is a solid incentive.
  if (found) {
    if (!collection.storeIndicesToHardDrive(commentsOnFailure)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Failed to update the databse index. "
        << "However, your object was stored.";
      }
      return false;
    }
  }
  return true;
}

bool DatabaseInternalServer::loadObject(
  const std::string& objectId,
  const std::string& collectionName,
  JSData& output,
  std::stringstream* commentsOnFailure
) {}

bool DatabaseInternalServer::createObject(
  const JSData& initialValue,
  const std::string& collectionName,
  std::string& outputObjectId,
  std::stringstream* commentsOnFailure
) {
  if (!this->collections.contains(collectionName)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Missing collection name: "
      << collectionName
      << ". ";
    }
    return false;
  }
  DatabaseCollection & collection =
  this->collections.getValueNoFailNonConst(collectionName);
  bool foundPrimaryKey = false;
  for (DatabaseInternalIndex& index : collection.indices.values) {
    JSData primaryKeyValueJSON;
    if (
      !initialValue.hasNestedKey(index.nestedKeys, &primaryKeyValueJSON)
    ) {
      continue;
    }
    std::string primaryKeyValue = primaryKeyValueJSON.stringValue;
    if (primaryKeyValue != "") {
      foundPrimaryKey = true;
      break;
    }
  }
  if (!foundPrimaryKey) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "DatabaseInternalServer::createObject: failed to find primary key. ";
    }
    return false;
  }
  bool continueRunning = true;
  bool checkFailed = true;
  int emergencyBreak = 0;
  do {
    emergencyBreak ++;
    if (emergencyBreak > 1000) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Too many fails to create object id. ";
      }
      return false;
    }
    outputObjectId = Crypto::Random::getRandomHexStringLeaveMemoryTrace(12);
    this->objectExists(
      outputObjectId,
      collectionName,
      continueRunning,
      checkFailed,
      commentsOnFailure
    );
  } while (continueRunning && !checkFailed);
  JSData objectCopy = initialValue;
  // This will override any user provided id.
  // Users are not allowed to set this.
  objectCopy[DatabaseStrings::labelId] = outputObjectId;
  return
  this->storeObject(
    outputObjectId, collectionName, objectCopy, commentsOnFailure
  );
}

std::string DatabaseInternalServer::objectFilename(
  const std::string& objectId, const std::string& collectionName
) {
  return this->owner->folder() + collectionName + "/" + objectId + ".json";
}

void DatabaseInternalServer::objectExists(
  const std::string& objectId,
  const std::string& collectionName,
  bool& output,
  bool& outputFailed,
  std::stringstream* commentsOnFailure
) {
  std::string fileName = this->objectFilename(objectId, collectionName);
  output =
  FileOperations::fileExistsVirtual(fileName, true, true, commentsOnFailure);
  outputFailed = false;
}

bool DatabaseInternalServer::initializeLoadFromHardDrive(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalServer::initializeLoadFromHardDrive");
  this->ensureCollection(
    DatabaseStrings::tableDeadlines,
    List<std::string>({DatabaseStrings::labelId})
  );
  this->ensureCollection(
    DatabaseStrings::tableDeleted,
    List<std::string>({DatabaseStrings::labelId})
  );
  this->ensureCollection(
    DatabaseStrings::tableEmailInfo,
    List<std::string>({DatabaseStrings::labelId})
  );
  this->ensureCollection(
    DatabaseStrings::tableProblemInformation,
    List<std::string>({DatabaseStrings::labelId})
  );
  this->ensureCollection(
    DatabaseStrings::tableUsers,
    List<std::string>({
        DatabaseStrings::labelId,
        DatabaseStrings::labelUsername,
        DatabaseStrings::labelEmail
      }
    )
  );
  this->ensureCollection(
    DatabaseStrings::tableProblemWeights,
    List<std::string>({DatabaseStrings::labelId})
  );
  for (DatabaseCollection& collection : this->collections.values) {
    if (!collection.loadIndicesFromHardDrive(commentsOnFailure)) {
      return false;
    }
  }
  return true;
}

bool DatabaseInternalServer::ensureCollection(
  const std::string& collectionName,
  const List<std::string>& indexableKeys
) {
  DatabaseCollection& collection =
  this->collections.getValueCreateEmpty(collectionName);
  collection.initialize(collectionName, this->owner);
  for (const std::string& key : indexableKeys) {
    DatabaseInternalIndex& index = collection.indices.getValueCreateEmpty(key);
    index.nestedKeys = indexableKeys;
  }
  return true;
}

void DatabaseInternalServer::storeEverything() {
  STACK_TRACE("DatabaseInternalServer::storeEverything");
}

void DatabaseInternalServer::storeCollectionList() {
  JSData collectionsData;
  for (const std::string& collectionName : this->collections.keys) {
    collectionsData[collectionName] =
    this->collections.getValueNoFail(collectionName).toJSONSchema();
  }
  std::string filename = this->collectionsSchemaFileName();
  std::stringstream commentsOnFailure;
  if (
    !FileOperations::
    writeFileVirualWithPermissions_accessUltraSensitiveFoldersIfNeeded(
      filename,
      collectionsData.toString(),
      true,
      true,
      &commentsOnFailure
    )
  ) {
    global
    << Logger::red
    << "Failed to store collections list! Data may be lost. "
    << Logger::endL;
  }
}

std::string DatabaseInternalServer::collectionsSchemaFileName() const {
  return this->owner->folder() + "collections.json";
}

void DatabaseCollection::initialize(
  const std::string& inputName, DatabaseInternal* inputOwner
) {
  this->name = inputName;
  this->owner = inputOwner;
}

DatabaseCollection::DatabaseCollection() {
  this->indicesLoaded = false;
  this->owner = nullptr;
}

bool DatabaseInternalIndex::fromJSON(
  const JSData& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalIndex::fromJSON");
  for (int i = 0; i < input.objects.size(); i ++) {
    std::string key = input.objects.keys[i];
    JSData& value = input.objects.values[i];
    if (value.elementType != JSData::Token::tokenString) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Key is not string. ";
      }
      return false;
    }
    this->objectIdToKeyValue.setKeyValue(key, value.stringValue);
    if (value.stringValue != "") {
      this->keyValueToObjectId.setKeyValue(value.stringValue, key);
    }
  }
  return true;
}

bool DatabaseCollection::loadIndicesFromHardDrive(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseCollection::loadIndicesFromHardDrive");
  if (this->indicesLoaded) {
    return true;
  }
  std::string filename = this->owner->folder() + this->name;
  std::string contentString;
  if (
    !FileOperations::
    loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(
      filename, contentString, true, true, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to load indices from hard drive.";
    }
    return false;
  }
  this->indices.clear();
  JSData content;
  if (!content.parse(contentString, true, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to parse indices from hard drive.";
    }
    return false;
  }
  for (int i = 0; i < content.objects.size(); i ++) {
    DatabaseInternalIndex& index =
    this->indices.getValueCreateEmpty(content.objects.keys[i]);
    if (
      !index.fromJSON(content.objects.values[i], commentsOnFailure)
    ) {
      return false;
    }
  }
  return true;
}

bool DatabaseCollection::storeIndicesToHardDrive(
  std::stringstream* commentsOnFailure
) {
  std::string filename = this->owner->folder() + this->name;
  JSData content;
  this->toJSONIndices(content);
  return
  FileOperations::
  writeFileVirualWithPermissions_accessUltraSensitiveFoldersIfNeeded(
    filename, content.toString(), true, true, commentsOnFailure
  );
}

JSData DatabaseInternalIndex::toJSON() const {
  JSData result;
  for (int i = 0; i < this->objectIdToKeyValue.size(); i ++) {
    result[this->objectIdToKeyValue.keys[i]] =
    this->objectIdToKeyValue.values[i];
  }
  return result;
}

void DatabaseCollection::toJSONIndices(JSData& output) const {
  output.reset(JSData::Token::tokenObject);
  for (int i = 0; i < this->indices.size(); i ++) {
    output[this->indices.keys[i]] = this->indices.values[i].toJSON();
  }
}

JSData DatabaseCollection::toJSONSchema() const {
  JSData result;
  JSData index;
  index.makeEmptyArray();
  JSData labelId;
  labelId = DatabaseStrings::labelId;
  index.listObjects.addOnTop(labelId);
  for (const std::string& indexEntry : this->indices.keys) {
    JSData entry;
    entry = indexEntry;
    index.listObjects.addOnTop(entry);
  }
  result["index"] = index;
  return result;
}
