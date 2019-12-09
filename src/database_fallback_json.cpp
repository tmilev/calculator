// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "general_logging_global_variables.h"
#include "database.h"
#include "string_constants.h"
// Avoid previous extern warning:
static ProjectInformationInstance projectInfoDatabaseFallbackJSON(__FILE__, "No-database fallback using json file.");

bool Database::FallBack::UpdateOneFromQueryString(
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
  MutexProcessLockGuard guardDB(this->access);
  JSData database;
  if (this->ReadDatabase(database, commentsOnFailure)) {
    return false;
  }
  if (!this->HasCollection(collectionName, commentsOnFailure)) {
    return false;
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Not implemented yet: findQuery: " << findQuery << " updadeQuery: " << updateQuery.ToString(false);
  }
  return false;
}

bool Database::FallBack::HasCollection(const std::string& collection, std::stringstream* commentsOnFailure) {
  if (!this->reader.HasKey(collection)) {
    return true;
  }
  if (Database::FallBack::knownCollections.Contains(collection)) {
    this->reader[collection].theType = JSData::token::tokenArray;
    return true;
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Database collection not found. ";
  }
  return false;
}

Database::FallBack::FallBack() {
  this->owner = nullptr;
}

void Database::FallBack::initialize() {
  this->access.CreateMe("databaseFallback", true, false);
  this->knownCollections.AddOnTop({
    DatabaseStrings::tableDeadlines,
    DatabaseStrings::tableDeleted,
    DatabaseStrings::tableEmailInfo,
    DatabaseStrings::tableProblemInformation,
    DatabaseStrings::tableProblemWeights,
    DatabaseStrings::tableUsers
  });
}

bool Database::FallBack::ReadDatabase(JSData& output, std::stringstream* commentsOnFailure) {
  std::string theDatabase;
  if (!FileOperations::LoadFileToStringVirtual(
    "database-fallback/database.json", theDatabase, true, commentsOnFailure
  )) {
    if (!FileOperations::FileExistsVirtual(
      "database-fallback/database.json",
      true,
      false,
      commentsOnFailure
    )) {
      return false;
    }
  }
  return output.readstring(theDatabase, false, commentsOnFailure);
}
