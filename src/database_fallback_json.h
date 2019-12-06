#ifndef DATABASE_FALLBACK_HEADER_ALREADY_INCLUDED
#define DATABASE_FALLBACK_HEADER_ALREADY_INCLUDED
#include "general_database_system_independent.h"
#include "multiprocessing.h"

static ProjectInformationInstance projectInfoDatabaseFallbackJSONHeader(__FILE__, "No-database fallback using json file, header.");

class DatabaseFallback {
public:
  static MutexProcess DatabaseAccess;
  static bool UpdateOneFromQueryString(
    const std::string& collectionName,
    const std::string& findQuery,
    const JSData& updateQuery,
    List<std::string>* fieldsToSetIfNullUseFirstFieldIfUpdateQuery,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool ReadDatabase(std::stringstream* commentsOnFailure, JSData& output);
};

#endif // DATABASE_FALLBACK_HEADER_ALREADY_INCLUDED

