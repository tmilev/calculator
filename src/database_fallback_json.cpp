// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "general_logging_global_variables.h"
#include "database.h"
#include "string_constants.h"
// Avoid previous extern warning:
static ProjectInformationInstance projectInfoDatabaseFallbackJSON(__FILE__, "No-database fallback using json file.");

bool Database::FallBack::FindOneFromSome(
  const List<QueryExact>& findOrQueries,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Database::FallBack::FindOneFromSome not implemented yet.";
  }
  return false;
}

bool Database::FallBack::UpdateOne(
  const QueryExact& findQuery,
  const JSData& updateQuery,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseFallback::UpdateOneFromQueryString");
  if (!theGlobalVariables.flagDatabaseUseFallback) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "DatabaseFallback::UpdateOneFromQueryString: "
      << "fallback db disabled and project compiled without mongoDB support. ";
    }
    return false;
  }
  MutexProcessLockGuard guardDB(this->access);
  JSData database;
  if (this->ReadAndIndexDatabase(commentsOnFailure)) {
    return false;
  }
  if (!this->HasCollection(findQuery.collection, commentsOnFailure)) {
    return false;
  }

  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Not implemented yet: findQuery: "
    << findQuery.ToJSON().ToString(false)
    << " updadeQuery: " << updateQuery.ToString(false);
  }
  return false;
}

bool Database::FallBack::FetchCollectionNames(List<std::string>& output, std::stringstream* commentsOnFailure) {
  MutexProcessLockGuard guardDB(this->access);
  if (this->ReadDatabase(commentsOnFailure)) {
    return false;
  }
  output.SetSize(0);
  output.AddListOnTop(this->reader.objects.theKeys);
  return true;
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

void Database::FallBack::CreateHashIndex(const std::string& collectionName, const std::string& theKey) {
  Database::FallBack::Index newIndex;
  newIndex.collection = collectionName;
  newIndex.label = theKey;
  newIndex.collectionAndLabelCache = newIndex.collectionAndLabel();
  this->indices.SetKeyValue(newIndex.collectionAndLabelCache, newIndex);
}

std::string Database::FallBack::Index::collectionAndLabelStatic(
  const std::string& inputCollection, const std::string& inputLabel
) {
  return inputCollection + "." + inputLabel;
}

std::string Database::FallBack::Index::collectionAndLabel() {
  return this->collectionAndLabelStatic(this->collection, this->label);
}

bool Database::FallBack::ReadAndIndexDatabase(std::stringstream* commentsOnFailure) {
  if (!this->ReadDatabase(commentsOnFailure)) {
    return false;
  }
  for (int i = 0; i < this->reader.objects.size(); i ++) {
    std::string collection = this->reader.objects.theKeys[i];
    JSData& currentCollection = this->reader.objects.theValues[i];
    for (int i = 0; i < currentCollection.theList.size; i ++) {

    }
  }
  return true;
}

void Database::FallBack::IndexOneRecord(
  const JSData& entry, int64_t row, const std::string& collection
) {
  if (entry.theType != JSData::token::tokenObject) {
    return;
  }
  for (int i = 0; i < entry.objects.size(); i ++) {
    std::string indexLabel = Database::FallBack::Index::collectionAndLabelStatic(collection, entry.objects.theKeys[i]);
    if (!this->indices.Contains(indexLabel)) {
      continue;
    }
    const JSData& keyToIndexBy = entry.objects.theValues[i];
    if (keyToIndexBy.theType != JSData::token::tokenString) {
      continue;
    }
    Database::FallBack::Index& currentIndex = this->indices.GetValueCreate(indexLabel);
    currentIndex.locations.GetValueCreate(keyToIndexBy.theString).AddOnTop(row);
  }
}

bool Database::FallBack::ReadDatabase(std::stringstream* commentsOnFailure) {
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
  return this->reader.readstring(theDatabase, false, commentsOnFailure);
}
