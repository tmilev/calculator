#include "database.h"
#include "string_constants.h"
#include "network_calculator.h"
#include "crypto_calculator.h"

std::string LocalDatabase::folder() {
  return "database/" + DatabaseStrings::databaseName + "/";
}

void LocalDatabase::createHashIndex(
  const std::string& collectionName, const std::string& key
) {
  (void) collectionName;
  (void) key;
  global.fatal
  << "LocalDatabase::createHashIndex: not implemented yet. "
  << global.fatal;
}

LocalDatabase::LocalDatabase() {
  this->processId = - 1;
  this->socketDatabaseServer = - 1;
  this->socketDatabaseClient = - 1;
  this->port = - 1;
}

bool LocalDatabase::fetchCollectionNames(
  List<std::string>& output, std::stringstream* commentsOnFailure
) {
  (void) output;
  (void) commentsOnFailure;
  global.fatal
  << "LocalDatabase::fetchCollectionNames: not implemented yet. "
  << global.fatal;
  return false;
}

bool LocalDatabase::findOneWithOptions(
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

bool LocalDatabase::updateOne(
  const QueryExact& findQuery,
  const QuerySet& updateQuery,
  std::stringstream* commentsOnFailure
) {
  (void) findQuery;
  (void) updateQuery;
  (void) commentsOnFailure;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "LocalDatabase::updateOne not implemented yet. ";
  }
  return false;
}

bool LocalDatabase::findOneFromSome(
  const List<QueryExact>& findOrQueries,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  (void) findOrQueries;
  (void) output;
  (void) commentsOnFailure;
  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
  // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;
  // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;
  // TCP stream sockets
  hints.ai_flags = AI_PASSIVE;
  // fill in my IP for me
  struct addrinfo* serverInfo = nullptr;
  std::stringstream portStream;
  portStream << this->port;
  int status =
  getaddrinfo(
    "localhost", portStream.str().c_str(), &hints, &serverInfo
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

bool LocalDatabase::deleteDatabase(std::stringstream* commentsOnFailure) {
  (void) commentsOnFailure;
  global.fatal
  << "LocalDatabase::deleteDatabase: not implemented yet."
  << global.fatal;
  return false;
}

bool LocalDatabase::findFromJSONWithOptions(
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

bool LocalDatabase::updateOneFromSome(
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

void LocalDatabase::listenToPort() {
  STACK_TRACE("LocalDatabase::listenToPort");
  if (
    !Listener::initializeBindToOnePort(
      "0", this->socketDatabaseServer, this->port, true
    )
  ) {
    global.fatal << "Failed to bind to port 0. " << global.fatal;
  }
  global
  << "Database initialized, listening on port: "
  << this->port
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

int LocalDatabase::forkOutDatabase() {
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

void LocalDatabase::initializeForkAndRun() {
  STACK_TRACE("LocalDatabase::initializeForkAndRun");
  this->listenToPort();
  if (this->forkOutDatabase() == 0) {
    this->initializeClient();
    return;
  }
  this->run();
}

void LocalDatabase::initializeClient() {
  close(this->socketDatabaseServer);
  this->socketDatabaseServer = - 1;
}

void LocalDatabase::run() {
  STACK_TRACE("LocalDatabase::run");
  std::stringstream portStream;
  portStream << this->port;
  this->allListeningSockets.setKeyValue(
    this->socketDatabaseServer, portStream.str()
  );
  Listener listener(
    this->socketDatabaseServer, &this->allListeningSockets
  );
  while (this->runOneConnection(listener)) {}
}

bool LocalDatabase::runOneConnection(Listener& listener) {
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
  return true;
}
