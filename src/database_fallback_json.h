#ifndef DATABASE_FALLBACK_HEADER_ALREADY_INCLUDED
#define DATABASE_FALLBACK_HEADER_ALREADY_INCLUDED
#include "general_database_system_independent.h"
#include "multiprocessing.h"

static ProjectInformationInstance projectInfoDatabaseFallbackJSONHeader(__FILE__, "No-database fallback using json file, header.");

class DatabaseFallback {
public:
  MutexProcess access;
  HashedList<std::string, MathRoutines::HashString> knownCollections;
  JSData reader;
  static DatabaseFallback& theDatabase();
  bool UpdateOneFromQueryString(
    const std::string& collectionName,
    const std::string& findQuery,
    const JSData& updateQuery,
    List<std::string>* fieldsToSetIfNullUseFirstFieldIfUpdateQuery,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool HasCollection(const std::string& collection, std::stringstream* commentsOnFailure);
  bool ReadDatabase(JSData& output, std::stringstream* commentsOnFailure);
  void initialize();
};

#endif // DATABASE_FALLBACK_HEADER_ALREADY_INCLUDED

