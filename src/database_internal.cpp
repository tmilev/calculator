#include "database.h"
#include "string_constants.h"
#include "network_calculator.h"
#include "crypto_calculator.h"

std::string DatabaseInternal::folder() {
  return "database/" + DatabaseStrings::databaseName + "/";
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
}

bool DatabaseInternal::fetchCollectionNames(
  List<std::string>& output, std::stringstream* commentsOnFailure
) {
  (void) output;
  (void) commentsOnFailure;
  global.fatal
  << "DatabaseInternal::fetchCollectionNames: not implemented yet. "
  << global.fatal;
  return false;
}

bool DatabaseInternal::findOneWithOptions(
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

bool DatabaseInternal::updateOne(
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
    !this->sendAndReceiveFromClientToServer(
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

bool DatabaseInternal::findOneFromSome(
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

bool DatabaseInternal::findFromJSONWithOptions(
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

bool DatabaseInternal::updateOneFromSome(
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
  while (true) {
    totalRuns ++;
    global << "DEBUG: total database runs:  " << totalRuns << Logger::endL;
    this->runOneConnection();
  }
  global << "Exit database main loop" << Logger::endL;
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
  DatabaseInternalRequest request;
  DatabaseInternalResult result;
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
  std::string temp =
  "{\"success\":false,\"comments\":\"Not implemented yet. " +
  request.toStringForDebugging() +
  "\"}";
  return this->currentServerToClient().writeOnceNoFailure(temp, 0, true);
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
  if (this->requestType == DatabaseInternalRequest::Type::findAndUpdate) {
    output[DatabaseStrings::requestType] = "findAndUpdate";
    output[DatabaseStrings::requestContent] =
    this->queryFindAndUpdate.toJSON();
    return;
  }
  output[DatabaseStrings::requestType] = "unknown";
}

JSData DatabaseInternalRequest::toJSON() const {
  JSData result;
  this->toJSON(result);
  return result;
}

std::string DatabaseInternalRequest::toStringForDebugging() const {
  std::stringstream out;
  out << "request: " << this->requestType;
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
