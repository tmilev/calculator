#include "database.h"
#include "string_constants.h"
#include <sys/wait.h>//<-waitpid f-n here
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <arpa/inet.h> // <- inet_ntop declared here (ntop = network to presentation)
#include <unistd.h>
#include <sys/stat.h>//<-for file statistics
#include <fcntl.h>//<-setting flags of file descriptors
#include <sys/resource.h> //<- for setrlimit(...) function. Restricts the time the executable can run.

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
  this->socket = - 1;
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
  global.fatal
  << "LocalDatabase::updateOne: not implemented yet. "
  << global.fatal;
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
  status =
  connect(this->socket, serverInfo->ai_addr, serverInfo->ai_addrlen);
  if (status != 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to connect to database. ";
    }
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
