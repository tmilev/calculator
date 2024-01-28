#include "crypto_calculator.h"
#include "database.h"
#include "general_file_operations_encodings.h"
#include "signals_infrastructure.h"
#include "string_constants.h"

std::string DatabaseInternal::folder() {
  return "database/" + Database::name + "/";
}

DatabaseInternal::DatabaseInternal() {
  this->processId = - 1;
  this->maximumMessageSize = 65536;
  this->flagFailedToInitialize = true;
  this->flagIsFallback = false;
  this->flagIsRunning = false;
}

bool DatabaseInternalClient::fetchCollectionNames(
  List<std::string>& output, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalClient::fetchCollectionNames");
  DatabaseInternalResult result;
  DatabaseInternalRequest request;
  request.requestType = DatabaseInternalRequest::Type::fetchCollectionNames;
  if (
    !this->owner->sendAndReceiveFromClientFull(
      request, result, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to send payload to database. ";
    }
    return false;
  }
  if (result.success) {
    output.clear();
    for (const JSData& element : result.content) {
      output.addOnTop(element.stringValue);
    }
  }
  return result.success;
}

DatabaseInternalClient::DatabaseInternalClient() {
  this->owner = nullptr;
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
    !this->owner->sendAndReceiveFromClientFull(
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

bool DatabaseInternalClient::checkInitialization() {
  if (this->owner == nullptr) {
    global.fatal
    << "Database internal client has non-initialized owner. "
    << global.fatal;
  }
  return true;
}

bool DatabaseInternalClient::find(
  const QueryOneOfExactly& findOrQueries,
  const QueryResultOptions* options,
  List<JSData>& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalClient::find");
  this->checkInitialization();
  DatabaseInternalResult result;
  DatabaseInternalRequest request;
  request.requestType = DatabaseInternalRequest::Type::find;
  request.queryOneOfExactly = findOrQueries;
  if (options != nullptr) {
    request.options = *options;
  }
  if (
    !this->owner->sendAndReceiveFromClientFull(
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
  } else if (commentsOnFailure != nullptr) {
    *commentsOnFailure << result.comments;
  }
  return result.success;
}

bool DatabaseInternalClient::shutdown(std::stringstream* commentsOnFailure) {
  STACK_TRACE("DatabaseInternalClient::shutdown");
  this->checkInitialization();
  DatabaseInternalResult result;
  DatabaseInternalRequest request;
  request.requestType = DatabaseInternalRequest::Type::shutdown;
  if (
    !this->owner->sendAndReceiveFromClientFull(
      request, result, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to send payload to database. ";
    }
    return false;
  }
  if (result.success) {
    this->owner->flagIsRunning = false;
  } else {
    *commentsOnFailure << result.comments;
  }
  return result.success;
}

int DatabaseInternal::forkOutDatabase() {
  STACK_TRACE("DatabaseInternal::forkOutDatabase");
  this->processId = ForkCreator::forkProcessAndAcquireRandomness(true);
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

void DatabaseInternal::initializeCommon() {
  this->client.owner = this;
  this->server.owner = this;
  this->flagIsRunning = true;
  for (int i = 0; i < 50; i ++) {
    this->largeMessages.getValueCreateEmpty(i);
  }
}

void DatabaseInternal::initializeAsFallback() {
  STACK_TRACE("DatabaseInternal::initializeAsFallback");
  this->initializeCommon();
  this->flagIsFallback = true;
  if (!this->mutexFallbackDatabase.createMe("DatabaseMutex", false)) {
    global.fatal
    << "Failed to create fallback database process mutex. "
    << global.fatal;
  }
}

void DatabaseInternal::initializeForkAndRun(int maximumConnections) {
  STACK_TRACE("DatabaseInternal::initializeForkAndRun");
  this->initializeCommon();
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

void DatabaseInternal::accountInitializationError(const std::string& error) {
  if (this->initializationErrors.size > 20) {
    return;
  }
  this->initializationErrors.addOnTop(error);
}

bool DatabaseInternal::sendAndReceiveFromClientFull(
  const DatabaseInternalRequest& input,
  DatabaseInternalResult& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternal::sendAndReceiveFromClientFull");
  std::string inputMessage;
  std::string outputMessage;
  inputMessage = input.toJSON().toString();
  if (
    !this->sendAndReceiveFromClientInternal(
      inputMessage, outputMessage, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      global
      << "Failed to send and receive from client.\n"
      << commentsOnFailure->str()
      << Logger::endL;
    }
    return false;
  }
  if (!output.fromJSON(outputMessage, commentsOnFailure)) {
    return false;
  }
  if (output.messageId <= 0 || output.messageSize <= 0) {
    // Not a large message.
    return true;
  }
  // We have a large message that does not fit in a single chunk.
  DatabaseInternalRequest chunkRequest;
  chunkRequest.messageHandle = output.messageHandle;
  chunkRequest.messageId = output.messageId;
  chunkRequest.requestType = DatabaseInternalRequest::Type::getLargeMessage;
  // The chunk size should be larger than 40000, the
  // maximumRuns computed below is smaller than the actual expected maximum
  // runs if everything runs fine.
  unsigned long expectedSize = static_cast<unsigned long>(output.messageSize);
  // Warning: without the extra + constant this may get rounded down wrongly.
  // +1 should be enough, but +10 should be good enough in case something
  // unexpected happens with the pipe read/write.
  int maximumRuns = expectedSize / 40000 + 10;
  std::string fullMessage;
  for (int i = 0; i < maximumRuns; i ++) {
    outputMessage = "";
    inputMessage = chunkRequest.toJSON().toString();
    bool chunkOK =
    this->sendAndReceiveFromClientInternal(
      inputMessage, outputMessage, commentsOnFailure
    );
    if (!chunkOK) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Internal failure while receiving "
        << "chunked message of expected size: "
        << expectedSize
        << ". ";
        global
        << "Failed to send and receive from client.\n"
        << commentsOnFailure->str()
        << Logger::endL;
      }
      return false;
    }
    fullMessage += outputMessage;
    if (fullMessage.size() >= expectedSize) {
      // Full message received.
      break;
    }
  }
  // Failure should not happen here, but is possible due to:
  // 1. Code errors:
  // - this has happened during development;
  // - has             not happened in our official server with our knowledge;
  // - hopefully has never happened in our official server.
  // 2. If the message handle expired, which should be possible
  // in theory if the database is flooded with requests.
  // The data may contain sensitive information such as password hashes.
  // Do not display parsing error comments: please pass nullptr
  // for the error comments.
  bool result = output.fromJSON(fullMessage, nullptr);
  if (result != true && commentsOnFailure != nullptr) {
    *commentsOnFailure
    << "Internal database error while moving large message of size: "
    << fullMessage.size()
    << ", with expected size: "
    << expectedSize
    << ". ";
  }
  return result;
}

bool DatabaseInternal::sendAndReceiveFromClientInternal(
  const std::string& input,
  std::string& output,
  std::stringstream* commentsOnFailure
) {
  if (!this->flagIsRunning) {
    // Do not send messages to a non-running database.
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Database not running. ";
    }
    return false;
  }
  if (this->flagIsFallback) {
    return
    this->sendAndReceiveFromClientToServerFallbackWithProcessMutex(
      input, output
    );
  }
  return
  this->sendAndReceiveFromClientToServerThroughPipe(
    input, output, commentsOnFailure
  );
}

bool DatabaseInternal::sendAndReceiveFromClientToServerFallbackWithProcessMutex
(const std::string& input, std::string& output) {
  STACK_TRACE(
    "DatabaseInternal::sendAndReceiveFromClientToServerFallbackWithProcessMutex"
  );
  MutexProcesslockGuard guard(this->mutexFallbackDatabase);
  // Read the entire DB from file:
  // the fallback database is slow.
  this->flagFailedToInitialize = !this->server.initializeLoadFromHardDrive();
  (void) guard;
  Crypto::convertStringToListBytesSigned(input, this->buffer);
  this->executeGetString(output);
  return true;
}

bool DatabaseInternal::sendAndReceiveFromClientToServerThroughPipe(
  const std::string& input,
  std::string& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternal::sendAndReceiveFromClientToServerThroughPipe");
  if (!this->sendFromClientToServer(input, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      global
      << "Failed to send from DB client to server.\n"
      << commentsOnFailure->str()
      << Logger::endL;
    }
    return false;
  }
  return this->receiveInClientFromServer(output, commentsOnFailure);
}

bool DatabaseInternal::sendFromClientToServer(
  const std::string& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternal::sendFromClientToServer");
  DatabaseInternalConnection& connection =
  this->connections[this->currentWorkerId];
  bool result = connection.clientToServer.writeOnceNoFailure(input, 0, true);
  if (!result && commentsOnFailure != nullptr) {
    *commentsOnFailure << "Failed to send bytes to database. ";
  }
  return result;
}

bool DatabaseInternal::receiveInClientFromServer(
  std::string& output, std::stringstream* commentsOnFailure
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
  Crypto::convertListCharsToString(
    connection.serverToClient.buffer, output
  );
  return true;
}

void DatabaseInternal::shutdownUnexpectedlyStatic(int unused) {
  (void) unused;
  if (global.databaseType != DatabaseType::internal) {
    return;
  }
  Database::get().localDatabase.shutdownUnexpectedly();
}

void DatabaseInternal::shutdownUnexpectedly() {
  global << Logger::red << "Received parent death signal. " << Logger::endL;
  this->flagIsRunning = false;
  for (DatabaseInternalConnection& connection : this->connections) {
    connection.clientToServer.release();
    connection.serverToClient.release();
  }
}

void DatabaseInternal::run() {
  STACK_TRACE("DatabaseInternal::run");
  for (int i = 0; i < this->connections.size; i ++) {
    int readEnd = this->connections[i].clientToServer.pipeEnds[0];
    this->readEnds.addOnTop(readEnd);
    this->mapFromReadEndsToWorkerIds.setKeyValue(readEnd, i);
  }
  this->flagFailedToInitialize = !this->server.initializeLoadFromHardDrive();
  this->flagIsRunning = true;
  static struct sigaction hangUpSignal;
  static struct sigaction interruptSignal;
  SignalsInfrastructure::signals().initializeOneSignal(
    SIGHUP, hangUpSignal, DatabaseInternal::shutdownUnexpectedlyStatic
  );
  SignalsInfrastructure::signals().initializeOneSignal(
    SIGINT, interruptSignal, DatabaseInternal::shutdownUnexpectedlyStatic
  );
  global << Logger::purple << "Database main loop start." << Logger::endL;
  while (this->flagIsRunning) {
    this->runOneConnection();
  }
  global << Logger::purple << "Exit database main loop. " << Logger::endL;
  std::exit(0);
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

void DatabaseInternal::executeGetString(std::string& output) {
  DatabaseInternalResult result;
  this->executeGetResult(result);
  if (result.nextChunk.size() > 0) {
    // The message is a chunk of a large message.
    // We send it raw.
    output = result.nextChunk;
    return;
  }
  output = result.toJSON().toString();
  if (output.size() <= 65000) {
    return;
  }
  // The message is too large.
  this->largeMessageCount ++;
  DatabaseInternalResult handle;
  handle.success = true;
  handle.messageId = this->largeMessageCount;
  handle.messageSize = output.size();
  handle.messageHandle = this->largeMessageCount % this->largeMessages.size();
  DatabaseLargeMessage& largeMessage =
  this->largeMessages.getValueNoFailNonConst(handle.messageHandle);
  largeMessage.content = output;
  largeMessage.messageId = handle.messageId;
  largeMessage.bytesSent = 0;
  output = handle.toJSON().toString();
}

void DatabaseInternal::executeGetResult(DatabaseInternalResult& output) {
  STACK_TRACE("DatabaseInternal::executeGetResult");
  if (this->flagFailedToInitialize) {
    output.success = false;
    output.comments = this->toStringInitializationErrors();
    return;
  }
  DatabaseInternalRequest request;
  std::string requestString;
  Crypto::convertListCharsToString(this->buffer, requestString);
  std::stringstream commentsOnFailure;
  if (!request.fromJSON(requestString, &commentsOnFailure)) {
    output.comments = commentsOnFailure.str();
    output.success = false;
    return;
  }
  output.success = false;
  switch (request.requestType) {
  case DatabaseInternalRequest::Type::findAndUpdate:
    output.success =
    this->server.findAndUpdate(
      request.queryFindAndUpdate, &commentsOnFailure
    );
    break;
  case DatabaseInternalRequest::Type::find:
    output.success =
    this->server.find(
      request.queryOneOfExactly,
      &request.options,
      output.content,
      &commentsOnFailure
    );
    break;
  case DatabaseInternalRequest::Type::fetchCollectionNames:
    output.success =
    this->server.fetchCollectionNames(output.content, &commentsOnFailure);
    break;
  case DatabaseInternalRequest::Type::shutdown:
    output.success = this->server.shutdown();
    break;
  case DatabaseInternalRequest::Type::getLargeMessage:
    this->fetchLargeMessage(request, output);
    break;
  case DatabaseInternalRequest::Type::unknown:
    break;
  }
  if (!output.success) {
    output.comments = commentsOnFailure.str();
  }
  output.comments += global.comments.getCurrentReset();
}

void DatabaseInternal::fetchLargeMessage(
  DatabaseInternalRequest& input, DatabaseInternalResult& output
) {
  if (input.messageId <= 0) {
    output.success = false;
    output.comments = "Message id must be positive";
    return;
  }
  int messageHandle = input.messageHandle % this->largeMessages.size();
  if (messageHandle < 0) {
    messageHandle += this->largeMessages.size();
  }
  DatabaseLargeMessage& message =
  this->largeMessages.getValueNoFailNonConst(messageHandle);
  if (message.messageId != input.messageId) {
    output.success = false;
    output.comments = "Message handle is stale. ";
    return;
  }
  // Sending in chunks of 65000.
  if (
    static_cast<unsigned long>(message.bytesSent) >= message.content.size()
  ) {
    output.success = false;
    output.comments = "Message already sent fully.";
    return;
  }
  output.nextChunk = message.content.substr(message.bytesSent, 65000);
  message.bytesSent += output.nextChunk.size();
}

bool DatabaseInternal::sendFromServerToClient(const std::string& message) {
  STACK_TRACE("DatabaseInternal::sendFromServerToClient");
  return this->currentServerToClient().writeOnceNoFailure(message, 0, true);
}

bool DatabaseInternal::executeAndSend() {
  STACK_TRACE("DatabaseInternal::executeAndSend");
  std::string result;
  this->executeGetString(result);
  return this->sendFromServerToClient(result);
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
  this->messageId = - 1;
  this->messageHandle = - 1;
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
  this->content.clear();
  if (this->reader.hasKey(DatabaseStrings::resultContent)) {
    this->content = this->reader[DatabaseStrings::resultContent].listObjects;
  }
  this->success =
  this->reader[DatabaseStrings::resultSuccess].isTrueRepresentationInJSON();
  this->reader[DatabaseStrings::messageHandle].integerValue.getElement().
  isIntegerFittingInInt(&this->messageHandle);
  this->messageId =
  this->reader[DatabaseStrings::messageId].integerValue.getElement();
  this->reader[DatabaseStrings::messageSize].integerValue.getElement().
  isIntegerFittingInInt(&this->messageSize);
  this->comments = this->reader[DatabaseStrings::resultComments].stringValue;
  return true;
}

JSData DatabaseInternalResult::toJSON() {
  STACK_TRACE("DatabaseInternalResult::toJSON");
  this->reader = JSData();
  this->reader[DatabaseStrings::resultSuccess] = this->success;
  this->reader[DatabaseStrings::resultComments] = this->comments;
  if (this->messageId > 0) {
    this->reader[DatabaseStrings::messageId] = this->messageId;
    this->reader[DatabaseStrings::messageHandle] = this->messageHandle;
    this->reader[DatabaseStrings::messageSize] = this->messageSize;
  } else {
    this->reader[DatabaseStrings::resultContent] = this->content;
  }
  return this->reader;
}

void DatabaseInternalResult::writeContent(List<JSData>& output) {
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
  if (this->requestType == DatabaseInternalRequest::Type::find) {
    output[DatabaseStrings::requestContent] = this->queryOneOfExactly.toJSON();
    if (this->options.isNonTrivial()) {
      output[DatabaseStrings::requestOptions] = this->options.toJSON();
    }
    return;
  }
  if (this->requestType == DatabaseInternalRequest::Type::getLargeMessage) {
    output[DatabaseStrings::messageHandle] = this->messageHandle;
    output[DatabaseStrings::messageId] = this->messageId;
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

DatabaseInternalRequest::Type DatabaseInternalRequest::stringToType(
  const std::string& input
) {
  if (
    input ==
    DatabaseInternalRequest::typeToString(
      DatabaseInternalRequest::Type::find
    )
  ) {
    return DatabaseInternalRequest::Type::find;
  }
  if (
    input ==
    DatabaseInternalRequest::typeToString(
      DatabaseInternalRequest::Type::findAndUpdate
    )
  ) {
    return DatabaseInternalRequest::Type::findAndUpdate;
  }
  if (
    input ==
    DatabaseInternalRequest::typeToString(
      DatabaseInternalRequest::Type::fetchCollectionNames
    )
  ) {
    return DatabaseInternalRequest::Type::fetchCollectionNames;
  }
  if (
    input ==
    DatabaseInternalRequest::typeToString(
      DatabaseInternalRequest::Type::shutdown
    )
  ) {
    return DatabaseInternalRequest::Type::shutdown;
  }
  if (
    input ==
    DatabaseInternalRequest::typeToString(
      DatabaseInternalRequest::Type::getLargeMessage
    )
  ) {
    return DatabaseInternalRequest::Type::getLargeMessage;
  }
  return DatabaseInternalRequest::Type::unknown;
}

std::string DatabaseInternalRequest::typeToString(
  DatabaseInternalRequest::Type input
) {
  switch (input) {
  case DatabaseInternalRequest::Type::findAndUpdate:
    return "findAndUpdate";
  case DatabaseInternalRequest::Type::find:
    return "find";
  case DatabaseInternalRequest::Type::unknown:
    return "unknown";
  case DatabaseInternalRequest::Type::fetchCollectionNames:
    return "fetchCollectionNames";
  case DatabaseInternalRequest::Type::shutdown:
    return "shutdown";
  case DatabaseInternalRequest::Type::getLargeMessage:
    return "getLargeMessage";
  }
  global.fatal
  << "Unhandled case in DatabaseInternalRequest::typeToString. "
  << global.fatal;
  return "";
}

bool DatabaseInternalRequest::fromJSData(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalRequest::fromJSData");
  this->requestType =
  DatabaseInternalRequest::stringToType(
    this->contentReader[DatabaseStrings::requestType].stringValue
  );
  switch (this->requestType) {
  case DatabaseInternalRequest::Type::findAndUpdate:
    return
    this->queryFindAndUpdate.fromJSON(
      this->contentReader[DatabaseStrings::requestContent],
      commentsOnFailure
    );
  case DatabaseInternalRequest::Type::find:
    if (this->contentReader.hasKey(DatabaseStrings::requestOptions)) {
      if (
        !this->options.fromJSON(
          this->contentReader[DatabaseStrings::requestOptions],
          commentsOnFailure
        )
      ) {
        return false;
      }
    }
    return
    this->queryOneOfExactly.fromJSON(
      this->contentReader[DatabaseStrings::requestContent],
      commentsOnFailure
    );
  case DatabaseInternalRequest::Type::getLargeMessage:
    this->messageHandle =
    this->contentReader[DatabaseStrings::messageHandle].integerValue.getElement
    ().getIntValueTruncated();
    this->messageId =
    this->contentReader[DatabaseStrings::messageId].integerValue.getElement();
    return true;
  case DatabaseInternalRequest::Type::fetchCollectionNames:
    return true;
  case DatabaseInternalRequest::Type::shutdown:
    return true;
  case DatabaseInternalRequest::Type::unknown:
    break;
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

bool DatabaseInternalServer::fetchCollectionNames(
  List<JSData>& output, std::stringstream* commentsOnFailure
) {
  (void) commentsOnFailure;
  output = this->collections.keys;
  return true;
}

bool DatabaseInternalServer::shutdown() {
  STACK_TRACE("DatabaseInternalServer::shutdown");
  global << Logger::red << "Database shutdown started." << Logger::endL;
  this->collections.clear();
  this->owner->flagIsRunning = false;
  return true;
}

bool DatabaseInternalServer::find(
  const QueryOneOfExactly& query,
  const QueryResultOptions* options,
  List<JSData>& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalServer::find");
  (void) options;
  List<std::string> currentObjectIds;
  HashedList<std::string> allObjectIds;
  JSData loader;
  std::string collection;
  for (int i = 0; i < query.queries.size; i ++) {
    QueryExact& queryExact = query.queries[i];
    if (i == 0) {
      collection = queryExact.collection;
    } else if (queryExact.collection != collection) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Combined query must refer to a single collection, "
        << "you used 2: "
        << queryExact.collection
        << ", collection.";
      }
      return false;
    }
    if (
      !DatabaseInternalServer::findObjectIds(
        queryExact, currentObjectIds, commentsOnFailure
      )
    ) {
      return false;
    }
    allObjectIds.addOnTopNoRepetition(currentObjectIds);
  }
  for (const std::string& objectId : allObjectIds) {
    if (
      !DatabaseInternalServer::loadObject(
        objectId, collection, loader, commentsOnFailure
      )
    ) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Failed to load object id: " << objectId << ". ";
      }
      return false;
    }
    loader[DatabaseStrings::labelId] = objectId;
    if (options != nullptr) {
      if (options->isNonTrivial()) {
        options->applyProjection(loader, loader);
      }
    }
    output.addOnTop(loader);
  }
  return true;
}

bool DatabaseInternalServer::findObjectIds(
  const QueryExact& query,
  List<std::string>& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalServer::findObjectIds");
  if (!this->collections.contains(query.collection)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Unknown collection: " << query.collection << ". ";
    }
    return false;
  }
  DatabaseCollection& collection =
  this->collections.getValueNoFailNonConst(query.collection);
  if (query.nestedLabels.size == 0) {
    DatabaseInternalIndex& index = collection.indexOfObjectIds();
    int numberOfItems =
    MathRoutines::minimum(
      query.maximumNumberOfItems, index.objectIdToKeyValue.size()
    );
    for (int i = 0; i < numberOfItems; i ++) {
      output.addOnTop(index.objectIdToKeyValue.keys[i]);
    }
    return true;
  }
  std::string concatenatedLabels = query.concatenateNestedLabels();
  if (!collection.indices.contains(concatenatedLabels)) {
    global
    << Logger::red
    << "Attempt to search using a non-indexed key: "
    << concatenatedLabels
    << " in collection: "
    << collection.name
    << ". ";
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Attempt to search using a non-indexed key: "
      << concatenatedLabels
      << ". ";
    }
    return false;
  }
  DatabaseInternalIndex& index =
  collection.indices.getValueNoFailNonConst(concatenatedLabels);
  std::string desiredValue = query.exactValue.stringValue;
  if (desiredValue == "") {
    if (commentsOnFailure != nullptr) {
      if (query.exactValue.elementType == JSData::Type::tokenString) {
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
  if (index.keyValueToObjectIds.contains(desiredValue)) {
    output.addListOnTop(
      index.keyValueToObjectIds.getValueNoFail(desiredValue)
    );
  }
  return true;
}

bool DatabaseInternalServer::findAndUpdate(
  QueryFindAndUpdate& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalServer::findAndUpdate");
  List<std::string> objectIds;
  if (!this->findObjectIds(input.find, objectIds, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "DatabaseInternalServer::findAndUpdate: failed to find object id. ";
    }
    return false;
  }
  if (objectIds.size > 1) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "DatabaseInternalServer::findAndUpdate: found "
      << objectIds.size
      << " objects with the given query.";
    }
    return false;
  }
  std::string objectId = "";
  if (objectIds.size > 0) {
    objectId = objectIds[0];
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
  this->storeObject(
    objectId, collectionName, object, true, commentsOnFailure
  );
  return result;
}

bool DatabaseInternalServer::storeObject(
  const std::string& objectId,
  const std::string& collectionName,
  const JSData& data,
  bool storeUpdatedIndexToHardDrive,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("DatabaseInternalServer::storeObject");
  global
  << "Writing object id: "
  << objectId
  << " in collection: "
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
  bool shouldStore =
  collection.updateObjectInIndexReturnTrueIfChanged(objectId, data);
  // We store the entire index to the hard drive.
  // This has run time of O(number of records), and should run in low
  // millisecond range for under 10k records.
  // This can be optimized to have approximately constant run time.
  // Please do so if there is a solid incentive.
  if (!shouldStore || !storeUpdatedIndexToHardDrive) {
    return true;
  }
  if (!collection.storeIndicesToHardDrive(commentsOnFailure)) {
    global
    << Logger::red
    << "Unexpected failure to store the indices of collection "
    << collectionName
    << "."
    << Logger::endL;
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to update the database index. "
      << "However, your object was stored. ";
    }
    return false;
  }
  return true;
}

bool DatabaseCollection::updateObjectInIndexReturnTrueIfChanged(
  const std::string& objectId, const JSData& data
) {
  // Update the indices with the new found object
  bool found = false;
  for (DatabaseInternalIndex& index : this->indices.values) {
    JSData primaryKeyValueJSON;
    if (!data.hasNestedKey(index.nestedKeys, &primaryKeyValueJSON)) {
      continue;
    }
    std::string incomingValue = primaryKeyValueJSON.stringValue;
    bool change = false;
    index.setObjectIdValue(objectId, incomingValue, change);
    found = found || change;
  }
  return found;
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
  // Objects may contain password hashes.
  // When parsed correctly, these fields are sanitized away:
  // they trimmed down so that only the first few and
  // last few characters are displayed.
  // However, if for some reason we have issues parsing the json
  // - for example, bug in our json parsing library -
  // we may accidentally leak the password hash in the
  // error message.
  // To be on the safe side, let us suppress the error message.
  // So, please pass nullptr for the "comments" error message argument.
  if (!output.parse(content, true, nullptr)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to parse object id: "
      << objectId
      << ". Not displaying the exact parsing issue for sercurity reasons. ";
    }
    global
    << Logger::red
    << "Failed to parse objectId: "
    << objectId
    << " in collection: "
    << collectionName
    << Logger::endL;
    return false;
  }
  return true;
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
    << "input record: "
    << initialValue.toString()
    << " is missing a primary id. Indices:"
    << collection.toJSONIndices()
    << Logger::endL;
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "DatabaseInternalServer::createObject: "
      << "input record: is missing a primary id. ";
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
    outputObjectId, collectionName, objectCopy, true, commentsOnFailure
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

void DatabaseInternalServer::ensureStandardCollectionIndices() {
  STACK_TRACE("DatabaseInternalServer::ensureStandardCollectionIndices");
  this->ensureCollection(
    DatabaseStrings::tableDeadlines,
    List<std::string>({
        DatabaseStrings::labelId,
        DatabaseStrings::labelDeadlinesSchema
      }
    )
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
    List<std::string>({
        DatabaseStrings::labelId,
        DatabaseStrings::labelProblemWeightsSchema
      }
    )
  );
}

bool DatabaseInternalServer::initializeLoadFromHardDrive() {
  STACK_TRACE("DatabaseInternalServer::initializeLoadFromHardDrive");
  this->ensureStandardCollectionIndices();
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

std::string DatabaseInternalServer::collectionsSchemaFileName() const {
  return this->owner->folder() + "collections.json";
}

JSData DatabaseInternalServer::toJSONDatabase() {
  JSData result;
  for (DatabaseCollection& collection : this->collections.values) {
    JSData collectionJSON;
    collection.toJSON(collectionJSON);
    result[collection.name] = collectionJSON;
  }
  return result;
}

DatabaseInternalIndex& DatabaseCollection::indexOfObjectIds() {
  return this->indices.getValueNoFailNonConst(DatabaseStrings::labelId);
}

void DatabaseCollection::initialize(
  const std::string& inputName, DatabaseInternal* inputOwner
) {
  this->name = inputName;
  this->owner = inputOwner;
  this->indices.clear();
  this->loadedRecords.clear();
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
    if (!index.fromJSON(content.objects.values[i], this->name)) {
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
  std::string contentString = content.toString();
  if (
    !FileOperations::
    writeFileVirualWithPermissions_accessUltraSensitiveFoldersIfNeeded(
      filename, contentString, true, true, commentsOnFailure
    )
  ) {
    global
    << Logger::red
    << "Failed to write the index file for collection: "
    << this->name
    << ". "
    << Logger::endL;
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
  for (int i = 0; i < this->keyValueToObjectIds.size(); i ++) {
    out
    << "{"
    << this->keyValueToObjectIds.keys[i]
    << ": "
    << this->keyValueToObjectIds.values[i]
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
  const JSData& input, const std::string& collectionName
) {
  STACK_TRACE("DatabaseInternalIndex::fromJSON");
  this->collectionName = collectionName;
  this->keyValueToObjectIds.clear();
  this->objectIdToKeyValue.clear();
  for (int i = 0; i < input.objects.size(); i ++) {
    std::string key = input.objects.keys[i];
    std::string value;
    if (!input.objects.values[i].isString(&value)) {
      global
      << Logger::red
      << "Non-string key values not allowed, skipping. "
      << "Index may need to be rebuilt. "
      << Logger::endL;
    }
    this->objectIdToKeyValue.setKeyValue(key, value);
  }
  this->computeKeyValueToObjectIds();
  return true;
}

void DatabaseInternalIndex::removeObjectId(
  const std::string& objectId, const std::string& previousValue
) {
  if (!this->keyValueToObjectIds.contains(previousValue)) {
    return;
  }
  HashedList<std::string>& objectIdContainer =
  this->keyValueToObjectIds.getValueNoFailNonConst(previousValue);
  objectIdContainer.removeFirstOccurenceSwapWithLast(objectId);
  if (objectIdContainer.size == 0) {
    this->keyValueToObjectIds.removeKey(previousValue);
  }
}

void DatabaseInternalIndex::setObjectIdValue(
  const std::string& objectId,
  const std::string& newValue,
  bool& outputValueChanged
) {
  outputValueChanged = false;
  if (this->objectIdToKeyValue.contains(objectId)) {
    std::string previousValue =
    this->objectIdToKeyValue.getValueNoFail(objectId);
    if (previousValue == newValue) {
      return;
    }
    this->removeObjectId(objectId, previousValue);
  }
  outputValueChanged = true;
  this->objectIdToKeyValue.setKeyValue(objectId, newValue);
  HashedList<std::string>& objectIdContainer =
  this->keyValueToObjectIds.getValueCreateEmpty(newValue);
  objectIdContainer.addOnTopNoRepetition(objectId);
}

void DatabaseInternalIndex::computeKeyValueToObjectIds() {
  STACK_TRACE("DatabaseInternalIndex::computeKeyValueToObjectIds");
  this->keyValueToObjectIds.clear();
  for (int i = 0; i < this->objectIdToKeyValue.size(); i ++) {
    const std::string& objectId = this->objectIdToKeyValue.keys[i];
    const std::string& valueForObject = this->objectIdToKeyValue.values[i];
    HashedList<std::string>& objectIds =
    this->keyValueToObjectIds.getValueCreateEmpty(valueForObject);
    objectIds.clear();
    objectIds.addOnTopNoRepetition(objectId);
  }
}

JSData DatabaseInternalIndex::toJSON() const {
  JSData result;
  result.elementType = JSData::Type::tokenObject;
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

void DatabaseCollection::toJSON(JSData& result) const {
  result.makeEmptyArray();
  if (!this->indices.contains(DatabaseStrings::labelId)) {
    return;
  }
  std::stringstream commentsOnFailure;
  const DatabaseInternalIndex& index =
  this->indices.getValueNoFail(DatabaseStrings::labelId);
  for (const std::string& key : index.objectIdToKeyValue.keys) {
    JSData object;
    if (
      !this->owner->server.loadObject(
        key, this->name, object, &commentsOnFailure
      )
    ) {
      global
      << Logger::red
      << "Failed to load object: "
      << key
      << ". "
      << commentsOnFailure.str()
      << Logger::endL;
    }
    object[DatabaseStrings::labelId] = key;
    result.listObjects.addOnTop(object);
  }
}

void DatabaseCollection::toJSONIndices(JSData& output) const {
  output.reset(JSData::Type::tokenObject);
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
