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
  (void) findQuery;
  (void) updateQuery;
  (void) commentsOnFailure;
  QueryFindAndUpdate queryFindAndUpdate;
  queryFindAndUpdate.find = findQuery;
  queryFindAndUpdate.update = updateQuery;
  std::string payload =
  this->toPayLoad(queryFindAndUpdate.toJSON().toString());
  std::string result;
  if (!this->sendAndReceiveFromClientToServer(payload, result)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to send payload to database. ";
    }
    return false;
  }
  global << "DEBUG: got to here!!!! Result: " << result << Logger::endL;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "DEBUG: result so far: " << result;
  }
  return false;
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
  const std::string& input, std::string& output
) {
  if (!this->sendFromClientToServer(input)) {
    return false;
  }
  return this->receiveInClientFromServer(output);
}

bool DatabaseInternal::sendFromClientToServer(const std::string& input) {
  global
  << "DEBUG: current worker id: "
  << this->currentWorkerId
  << Logger::endL;
  DatabaseInternalConnection& connection =
  this->connections[this->currentWorkerId];
  global
  << "DEBUG: about to send: "
  << input.size()
  << " bytes. "
  << Logger::endL;
  bool result = connection.clientToServer.writeOnceNoFailure(input, 0, true);
  global << "DEBUG: sent OK: " << input.size() << " bytes. " << Logger::endL;
  return result;
}

bool DatabaseInternal::receiveInClientFromServer(std::string& output) {
  DatabaseInternalConnection& connection =
  this->connections[this->currentWorkerId];
  global
  << "DEBUG: About to read server to client on read end: "
  << connection.serverToClient.pipeEnds[0]
  << Logger::endL;
  bool result = connection.serverToClient.readOnceNoFailure(true);
  global
  << "DEBUG:  read: done: "
  << connection.serverToClient.buffer.size
  << Logger::endL;
  Crypto::convertListCharsToString(connection.serverToClient.buffer, output);
  return result;
}

void DatabaseInternal::run() {
  STACK_TRACE("DatabaseInternal::run");
  int totalRuns = 0;
  for (int i = 0; i < this->connections.size; i ++) {
      int readEnd =       this->connections[i].clientToServer.pipeEnds[0];

    this->readEnds.addOnTop(readEnd
    );
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
    global << "DEBUG: failed to read!!!" << Logger::endL;
    return false;
  }
  this->currentWorkerId = this->mapFromReadEndsToWorkerIds.getValueNoFail(fileDescriptor);
  global
  << "DEBUG: read OK!!! Read: "
  << this->buffer.size
  << " bytes. "
  << Logger::endL;
  return this->executeAndSend();
}

bool DatabaseInternal::executeAndSend() {
  STACK_TRACE("DatabaseInternal::executeAndSend");
  global << "DEBUG: current worker id: " << this->currentWorkerId;
  return
  this->connections[this->currentWorkerId].serverToClient.writeOnceNoFailure(
    "Not implemented yet", 0, true
  );
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
