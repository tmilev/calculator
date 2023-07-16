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
  STACK_TRACE("DatabaseInternalClient::findOneWithOptions");
  QueryOneOfExactly queryOrForm;
  queryOrForm.queries.addOnTop(query);
  JSData nonProjectedOutput;
  (void) commentsGeneralNonSensitive;
  if (
    !this->findOneFromSome(
      queryOrForm, nonProjectedOutput, commentsOnFailure
    )
  ) {
    return false;
  }
  options.applyProjection(nonProjectedOutput, output);
  return true;
}

bool DatabaseInternalClient::updateOne(
  const QueryExact& findQuery,
  const QuerySet& updateQuery,
  bool createIfNotFound,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalClient::updateOne");
  QueryFindAndUpdate queryFindAndUpdate;
  queryFindAndUpdate.find = findQuery;
  queryFindAndUpdate.update = updateQuery;
  queryFindAndUpdate.createIfNotFound = createIfNotFound;
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

std::string DatabaseInternal::toStringInitializationErrors() const {
  std::stringstream out;
  out << "Database not initialized correctly.<br>";
  for (int i = 0; i < this->initializationErrors.size; i ++) {
    out << this->initializationErrors[i] << "\n";
  }
  return out.str();
}

std::string DatabaseInternal::toPayLoad(const std::string& input) {
  return input;
}

bool DatabaseInternalClient::findOneFromSome(
  const QueryOneOfExactly& findOrQueries,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalClient::findOneFromSome");
  DatabaseInternalResult result;
  DatabaseInternalRequest request;
  request.requestType = DatabaseInternalRequest::Type::findOneFromSome;
  request.queryOneOfExactly = findOrQueries;
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
  if (result.success) {
    result.writeContent(output);
  }
  return result.success;
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
  const QueryOneOfExactly& findOrQueries,
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

void DatabaseInternal::accountInitializationError(const std::string& error) {
  if (this->initializationErrors.size > 20) {
    return;
  }
  this->initializationErrors.addOnTop(error);
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
  bool result = this->receiveInClientFromServer(output, commentsOnFailure);
  return result;
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
  for (int i = 0; i < this->connections.size; i ++) {
    int readEnd = this->connections[i].clientToServer.pipeEnds[0];
    this->readEnds.addOnTop(readEnd);
    this->mapFromReadEndsToWorkerIds.setKeyValue(readEnd, i);
  }
  this->failedToInitialize = !this->server.initializeLoadFromHardDrive();
  while (true) {
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
    result.comments = this->toStringInitializationErrors();
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
  switch (request.requestType) {
  case DatabaseInternalRequest::Type::findAndUpdate:
    result.success =
    this->server.findAndUpdate(
      request.queryFindAndUpdate, &commentsOnFailure
    );
    break;
  case DatabaseInternalRequest::Type::findOneFromSome:
    result.success =
    this->server.findOneFromSome(
      request.queryOneOfExactly, result.content, &commentsOnFailure
    );
    break;
  default:
    break;
  }
  if (!result.success) {
    result.comments = commentsOnFailure.str();
  }
  result.success =
  this->currentServerToClient().writeOnceNoFailure(
    result.toJSON().toString(), 0, true
  );
  return result.success;
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
    !this->reader.objects.contains(DatabaseStrings::resultSuccess)
  ) {
    if (commentsOnFalse != nullptr) {
      *commentsOnFalse
      << "Missing result success entry: "
      << DatabaseStrings::resultSuccess
      << ". Database result: "
      << this->reader.toString();
    }
    return false;
  }
  JSData successValue =
  this->reader.objects.getValueNoFail(DatabaseStrings::resultSuccess);
  if (successValue.isTrueRepresentationInJSON()) {
    return true;
  }
  if (
    commentsOnFalse != nullptr &&
    this->reader.objects.contains(DatabaseStrings::resultComments)
  ) {
    *commentsOnFalse
    << this->reader.objects.getValueNoFail(DatabaseStrings::resultComments).
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
  if (!this->reader.parse(input, true, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to parse database result. ";
    }
    return false;
  }
  this->content = JSData();
  if (this->reader.hasKey(DatabaseStrings::resultContent)) {
    this->content = this->reader[DatabaseStrings::resultContent];
  }
  this->success =
  this->reader[DatabaseStrings::resultSuccess].isTrueRepresentationInJSON();
  return true;
}

JSData DatabaseInternalResult::toJSON() {
  STACK_TRACE("DatabaseInternalResult::toJSON");
  this->reader = JSData();
  this->reader[DatabaseStrings::resultSuccess] = this->success;
  this->reader[DatabaseStrings::resultComments] = this->comments;
  return this->reader;
}

void DatabaseInternalResult::writeContent(JSData& output) {
  output = this->content;
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
  if (this->requestType == DatabaseInternalRequest::Type::findOneFromSome) {
    output[DatabaseStrings::requestContent] = this->queryOneOfExactly.toJSON();
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
  out
  << "request: "
  << this->typeToString(this->requestType)
  << " findAndUpdate: "
  << this->queryFindAndUpdate.toJSON().toString()
  << " queryOneOfExactly: "
  << this->queryOneOfExactly.toJSON().toString();
  return out.str();
}

std::string DatabaseInternalRequest::typeToString(
  DatabaseInternalRequest::Type input
) {
  switch (input) {
  case DatabaseInternalRequest::Type::findAndUpdate:
    return "findAndUpdate";
  case DatabaseInternalRequest::Type::findOneFromSome:
    return "fineOneFromSome";
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
  if (
    this->contentReader[DatabaseStrings::requestType].stringValue ==
    "fineOneFromSome"
  ) {
    this->requestType = DatabaseInternalRequest::Type::findOneFromSome;
    return
    this->queryOneOfExactly.fromJSON(
      this->contentReader[DatabaseStrings::requestContent],
      commentsOnFailure
    );
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "DatabaseInternalRequest::fromJSData: unknown request type. ";
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
) {

  return false;
}

bool DatabaseInternalServer::findOneFromSome(
  const QueryOneOfExactly& query,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalServer::findOneFromSome");
  std::string objectId;
  for (const QueryExact& queryExact : query.queries) {
    if (
      !DatabaseInternalServer::findObjectId(
        queryExact, objectId, commentsOnFailure
      )
    ) {
      continue;
    }
    if (objectId == "") {
      continue;
    }
    return
    DatabaseInternalServer::loadObject(
      objectId, queryExact.collection, output, commentsOnFailure
    );
  }
  return false;
}

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
      if (query.exactValue.elementType == JSData::Token::tokenString) {
        *commentsOnFailure << "Searching by empty string is not allowed. ";
      } else {
        *commentsOnFailure
        << "For now, only searching by exact string value is allowed. ";
      }
      *commentsOnFailure
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
  bool result =
  this->storeObject(objectId, collectionName, object, commentsOnFailure);
  return result;
}

bool DatabaseInternalServer::storeObject(
  const std::string& objectId,
  const std::string& collectionName,
  const JSData& data,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalServer::storeObject");
  global
  << "Writing object id: "
  << objectId
  << ", "
  << collectionName
  << Logger::endL;
  if (!this->collections.contains(collectionName)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "DatabaseInternalServer::storeObject: collection not found.";
    }
    return false;
  }
  DatabaseCollection& collection =
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
        << "Failed to update the database index. "
        << "However, your object was stored. ";
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
) {
  std::string filename = this->objectFilename(objectId, collectionName);
  std::string content;
  if (
    !FileOperations::
    loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(
      filename, content, true, true, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Object not present: " << objectId;
    }
    return false;
  }
  return output.parse(content, true, commentsOnFailure);
}

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
    global
    << Logger::yellow
    << "DatabaseInternalServer::createObject: "
    << "input record: is missing a primay id. Indices:"
    << collection.toJSONIndices()
    << Logger::endL;
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "DatabaseInternalServer::createObject: "
      << "input record: is missing a primay id. ";
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

std::string DatabaseInternalServer::toStringIndices() const {
  std::stringstream out;
  for (int i = 0; i < this->collections.size(); i ++) {
    out
    << "collection: ["
    << this->collections.keys[i]
    << "]:"
    << this->collections.values[i].toStringIndices();
    if (i != this->collections.size() - 1) {
      out << ", ";
    }
  }
  return out.str();
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

bool DatabaseInternalServer::initializeLoadFromHardDrive() {
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
    if (!collection.loadIndicesFromHardDrive(false)) {
      global
      << Logger::red
      << "Failed to load indices from the hard drive. "
      << Logger::green
      << "This is OK if running the calculator for the first time. "
      << Logger::endL;
      continue;
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
    index.collectionName = collectionName;
    index.nestedKeys.addOnTop(key);
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

std::string DatabaseCollection::fileNameIndex() const {
  return this->owner->folder() + this->name + ".json";
}

std::string DatabaseCollection::toStringIndices() const {
  std::stringstream out;
  out << "collection name: [" << this->name << "]: {";
  for (int i = 0; i < this->indices.size(); i ++) {
    out
    << "["
    << this->indices.keys[i]
    << "]: ["
    << this->indices.values[i].toString()
    << "]";
  }
  out << "}";
  return out.str();
}

bool DatabaseCollection::loadIndicesFromHardDrive(bool clearPreExistingIndices)
{
  STACK_TRACE("DatabaseCollection::loadIndicesFromHardDrive");
  if (this->indicesLoaded) {
    return true;
  }
  std::string filename = this->fileNameIndex();
  std::string contentString;
  std::stringstream commentsOnFailure;
  if (
    !FileOperations::fileExistsVirtual(filename, true, true, nullptr)
  ) {
    global
    << Logger::red
    << "Missing index file: "
    << filename
    << Logger::endL
    << "I assume this is the first run and "
    << "will try to generate the index for you. "
    << Logger::endL;
    if (!this->storeIndicesToHardDrive(&commentsOnFailure)) {
      global
      << Logger::red
      << "Failed to store indices to the hard drive. "
      << Logger::endL;
      this->owner->accountInitializationError(
        commentsOnFailure.str() + "Failed to store indices to the hard drive. "
      );
      return false;
    }
  }
  if (
    !FileOperations::
    loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(
      filename, contentString, true, true, &commentsOnFailure
    )
  ) {
    this->owner->accountInitializationError(commentsOnFailure.str());
    global
    << Logger::red
    << "Error loading file: "
    << filename
    << Logger::endL;
    return false;
  }
  if (clearPreExistingIndices) {
    this->indices.clear();
  }
  JSData content;
  if (!content.parse(contentString, true, &commentsOnFailure)) {
    this->owner->accountInitializationError(commentsOnFailure.str());
    return false;
  }
  for (int i = 0; i < content.objects.size(); i ++) {
    std::string indexCombinedKey = content.objects.keys[i];
    DatabaseInternalIndex& index =
    this->indices.getValueCreateEmpty(indexCombinedKey);
    if (
      !index.fromJSON(
        content.objects.values[i], this->name, &commentsOnFailure
      )
    ) {
      this->owner->accountInitializationError(commentsOnFailure.str());
      return false;
    }
  }
  return true;
}

bool DatabaseCollection::storeIndicesToHardDrive(
  std::stringstream* commentsOnFailure
) {
  std::string filename = this->fileNameIndex();
  JSData content;
  this->toJSONIndices(content);
  if (
    !FileOperations::
    writeFileVirualWithPermissions_accessUltraSensitiveFoldersIfNeeded(
      filename, content.toString(), true, true, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to store the index of "
      << this->name
      << " to filename: "
      << filename
      << ". ";
    }
    return false;
  }
  return true;
}

DatabaseInternalIndex::DatabaseInternalIndex() {}

std::string DatabaseInternalIndex::toString() const {
  std::stringstream out;
  out
  << "index: {"
  << this->collectionName
  << ": "
  << this->nestedKeys.toStringCommaDelimited()
  << "}";
  out << ", keyValueToObjectId: ";
  for (int i = 0; i < this->keyValueToObjectId.size(); i ++) {
    out
    << "{"
    << this->keyValueToObjectId.keys[i]
    << ": "
    << this->keyValueToObjectId.values[i]
    << "}";
  }
  out << ", objectIdToKeyValue: ";
  for (int i = 0; i < this->objectIdToKeyValue.size(); i ++) {
    out
    << "{"
    << this->objectIdToKeyValue.keys[i]
    << ": "
    << this->objectIdToKeyValue.values[i]
    << "}";
  }
  return out.str();
}

bool DatabaseInternalIndex::fromJSON(
  const JSData& input,
  const std::string& collectionName,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalIndex::fromJSON");
  this->collectionName = collectionName;
  for (int i = 0; i < input.objects.size(); i ++) {
    std::string key = input.objects.keys[i];
    JSData& value = input.objects.values[i];
    if (value.elementType != JSData::Token::tokenString) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Key is not string. ";
      }
      return false;
    }
    std::string valueString = value.stringValue;
    this->objectIdToKeyValue.setKeyValue(key, valueString);
    if (valueString != "") {
      this->keyValueToObjectId.setKeyValue(valueString, key);
    }
  }
  return true;
}

JSData DatabaseInternalIndex::toJSON() const {
  JSData result;
  result.elementType = JSData::Token::tokenObject;
  for (int i = 0; i < this->objectIdToKeyValue.size(); i ++) {
    result[this->objectIdToKeyValue.keys[i]] =
    this->objectIdToKeyValue.values[i];
  }
  return result;
}

JSData DatabaseCollection::toJSONIndices() const {
  JSData result;
  this->toJSONIndices(result);
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
