#include "database.h"
#include "string_constants.h"
#include "network_calculator.h"
#include "crypto_calculator.h"

std::string DatabaseLocal::folder() {
  return "database/" + DatabaseStrings::databaseName + "/";
}

void DatabaseLocal::createHashIndex(
  const std::string& collectionName, const std::string& key
) {
  (void) collectionName;
  (void) key;
  global.fatal
  << "LocalDatabase::createHashIndex: not implemented yet. "
  << global.fatal;
}

DatabaseLocal::DatabaseLocal() {
  this->processId = - 1;
  this->socketDatabaseServer = - 1;
  this->socketDatabaseClient = - 1;
  this->port = "8177";
}

bool DatabaseLocal::fetchCollectionNames(
  List<std::string>& output, std::stringstream* commentsOnFailure
) {
  (void) output;
  (void) commentsOnFailure;
  global.fatal
  << "LocalDatabase::fetchCollectionNames: not implemented yet. "
  << global.fatal;
  return false;
}

bool DatabaseLocal::findOneWithOptions(
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
  << "LocalDatabase::findOneWithOptions: not implemented yet. "
  << global.fatal;
  return false;
}

bool DatabaseLocal::updateOne(
  const QueryExact& findQuery,
  const QuerySet& updateQuery,
  std::stringstream* commentsOnFailure
) {
  (void) findQuery;
  (void) updateQuery;
  (void) commentsOnFailure;
  Connector connector("database_client_connector");
global << "DEBUG: connector for port: " << this->port << Logger::endL;
  connector.initialize("127.0.0.1", this->port);
  if (!connector.connectWrapper(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to connect to database. ";
    }
    return false;
  }
  QueryFindAndUpdate queryFindAndUpdate;
  queryFindAndUpdate.find = findQuery;
  queryFindAndUpdate.update = updateQuery;
  std::string payload =
  this->toPayLoad(queryFindAndUpdate.toJSON().toString());
  std::string result;
  global << "DEBUG: before send and receive!!!" << Logger::endL;
  if (!connector.sendAndReceive(payload, result)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to send payload to database. ";
    }
    return false;
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "DEBUG: result so far: " << result;
  }
  return false;
}

std::string DatabaseLocal::toPayLoad(const std::string& input) {
  return input;
}

bool DatabaseLocal::findOneFromSome(
  const List<QueryExact>& findOrQueries,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
    return false;
  (void) findOrQueries;
  (void) output;
  (void) commentsOnFailure;
  struct addrinfo hints;
  if (commentsOnFailure != nullptr) {
    global << "DEBUG: not implemented yet. " << Logger::endL;
  }
  memset(&hints, 0, sizeof hints);
  // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;
  // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;
  // TCP stream sockets
  hints.ai_flags = AI_PASSIVE;
  // fill in my IP for me
  struct addrinfo* serverInfo = nullptr;
  int status =
  getaddrinfo(
    "localhost", this->port.c_str(), &hints, &serverInfo
  );
  if (status == - 1) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to get addrinfo. ";
    }
    return false;
  }
  //  this->socketDatabaseClient = socket()
  status =
  connect(
    this->socketDatabaseClient,
    serverInfo->ai_addr,
    serverInfo->ai_addrlen
  );
  if (status != 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "LocalDatabase::findOneFromSome: failed to connect to database. ";
    }
    global
    << "DEBUG: LocalDatabase::findOneFromSome: "
    << "failed to connect to database. port: "
    << this->port
    << "socket: "
    << this->socketDatabaseServer
    << Logger::endL;
    return false;
  }
  global.fatal
  << "LocalDatabase::findOneFromSome: not implemented yet. "
  << global.fatal;
  return false;
}

bool DatabaseLocal::deleteDatabase(std::stringstream* commentsOnFailure) {
  (void) commentsOnFailure;
  global.fatal
  << "LocalDatabase::deleteDatabase: not implemented yet."
  << global.fatal;
  return false;
}

bool DatabaseLocal::findFromJSONWithOptions(
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
  << "LocalDatabase::findFromJSONWithOptions: not implemented yet. "
  << global.fatal;
  return false;
}

bool DatabaseLocal::updateOneFromSome(
  const List<QueryExact>& findOrQueries,
  const QuerySet& updateQuery,
  std::stringstream* commentsOnFailure
) {
  (void) findOrQueries;
  (void) updateQuery;
  (void) commentsOnFailure;
  global.fatal
  << "LocalDatabase::updateOneFromSome: not implemented yet. "
  << global.fatal;
  return false;
}

void DatabaseLocal::listenToSocket() {
  STACK_TRACE("LocalDatabase::listenToSocket");
  this->port = "8177";
  int actualPort = 0;
  if (
    !Listener::initializeBindToOnePort(
      this->port, this->socketDatabaseServer, actualPort, true
    )
  ) {
    global.fatal << "Failed to bind to port 0. " << global.fatal;
  }
  global
  << "Database initialized, listening on port: "
  << this->port
  << " mapped to port: "
  << actualPort
  << ", socket: "
  << this->socketDatabaseServer
  << "."
  << Logger::endL;
  if (
    listen(
      this->socketDatabaseServer, Listener::maximumPendingConnections
    ) ==
    - 1
  ) {
    global.fatal
    << "Failed to listen on database socket: "
    << this->socketDatabaseServer
    << global.fatal;
  }
}

int DatabaseLocal::forkOutDatabase() {
  STACK_TRACE("LocalDatabase::forkOutDatabase");
  Crypto::Random::getRandomBytesSecureInternalMayLeaveTracesInMemory(
    this->randomId, 24
  );
  this->idLoggable =
  Crypto::convertListUnsignedCharsToHexFormat(this->randomId, 0, false);
  this->idLoggable =
  this->idLoggable.substr(0, 4) +
  "_" +
  this->idLoggable.substr(this->idLoggable.size() - 4, 4);
  global << "Database session id: " << this->idLoggable << Logger::endL;
  this->processId = ForkCreator::forkProcessAndAcquireRandomness();
  if (this->processId == - 1) {
    global.fatal << "Failed to start database. " << global.fatal;
  }
  if (this->processId != 0) {
    // This is the parent process.
    return this->processId;
  }
  global.logs.logType = GlobalVariables::LogData::type::database;
  Crypto::computeSha3_256(this->randomId, this->randomIdHash);
  this->randomId.clear();
  return 0;
}

void DatabaseLocal::initializeForkAndRun() {
  STACK_TRACE("LocalDatabase::initializeForkAndRun");
  if (! this->clientToServer.createMe("database_client_to_server") || !
  this->serverToClient.createMe("database_server_to_client")){
      global.fatal << "Failed to create database send+receive pipes." << global.fatal;
  }
  if (this->forkOutDatabase() > 0) {
      // This is the parent process that will ultimately become the client.
    return;
  }
  this->run();
}




void DatabaseLocal::run() {
  STACK_TRACE("LocalDatabase::run");
  std::stringstream portStream;
  portStream << this->port;
  this->allListeningSockets.setKeyValue(
    this->socketDatabaseServer, portStream.str()
  );
  this->listenToSocket();
  Listener listener(
    this->socketDatabaseServer, &this->allListeningSockets
  );
  global << "DEBUG: about to start database main loop" << Logger::endL;
  while (true) {
    this->runOneConnection(listener);
  }
  global << "Exit database main loop" << Logger::endL;
}

bool DatabaseLocal::runOneConnection(Listener& listener) {
  STACK_TRACE("LocalDatabase::runOneConnection");
  listener.zeroSocketSet();
  listener.selectWrapper();
  int newConnectedSocket = listener.acceptWrapper();
  if (newConnectedSocket == - 1) {
    global
    << Logger::red
    << "Database failed to accept connection"
    << Logger::endL;
    global << Logger::yellow << strerror(errno) << Logger::endL;
    return true;
  }
  ReceiverInternal receiver(newConnectedSocket, "database_receiver");
  if (!receiver.recieveWithLengthHeader()) {
    return false;
  }
  Crypto::convertBytesToString(receiver.received, this->lastRequestBytes);
  return this->executeAndSend(newConnectedSocket);
}

bool DatabaseLocal::executeAndSend(int connectedSocket) {
  STACK_TRACE("LocalDatabase::executeAndSend");
  SenderInternal sender(connectedSocket, "database_sender");
  return
  sender.sendWithLengthHeader(
    "execute and send not implemented yet. Last request received: " +
    this->lastRequestBytes +
    "\n"
  );
}
