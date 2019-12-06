// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "general_logging_global_variables.h"
#include "database_fallback_json.h"
// Avoid previous extern warning:
extern ProjectInformationInstance projectInfoDatabaseFallbackJSON;
ProjectInformationInstance projectInfoDatabaseFallbackJSON(__FILE__, "No-database fallback using json file.");

MutexProcess DatabaseFallback::DatabaseAccess;

bool DatabaseFallback::UpdateOneFromQueryString(
  const std::string& collectionName,
  const std::string& findQuery,
  const JSData& updateQuery,
  List<std::string>* fieldsToSetIfNullUseFirstFieldIfUpdateQuery,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseFallback::UpdateOneFromQueryString");
  if (!theGlobalVariables.flagDatabaseUseFallback) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "DatabaseFallback::UpdateOneFromQueryString: fallback db disabled and project compiled without mongoDB support. ";
    }
    return false;
  }
  DatabaseFallback::DatabaseAccess.Lock();
  JSData database;
  if (!DatabaseFallback::ReadDatabase(commentsOnFailure, database)) {
    return false;
  }

  int pleaseContinueHere;










  DatabaseFallback::DatabaseAccess.Unlock();
  return false;
}

bool DatabaseFallback::ReadDatabase(std::stringstream* commentsOnFailure, JSData& output) {
  std::string theDatabase;
  if (!FileOperations::LoadFileToStringVirtual(
    "database-fallback/database.json", theDatabase, true, commentsOnFailure
  )) {
    return false;
  }
  return output.readstring(theDatabase, false, commentsOnFailure);
}
