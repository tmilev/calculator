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
  for (int i = 0; i < findOrQueries.size; i ++) {
    global << "DEBUG: Finding query: " << findOrQueries[i].ToJSON().ToString(false) << logger::endL;
    if (this->FindOne(findOrQueries[i], output, commentsOnFailure)) {
      return true;
    }
  }
  return false;
}

bool Database::FallBack::UpdateOne(
  const QueryExact& findQuery,
  const JSData& dataToMerge,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseFallback::UpdateOneFromQueryString");
  global << "DEBUG: Inside update one." << logger::endL;
  if (!global.flagDatabaseUseFallback) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "DatabaseFallback::UpdateOneFromQueryString: "
      << "fallback db disabled and project compiled without mongoDB support. ";
    }
    return false;
  }
  MutexProcessLockGuard guardDB(this->access);
  global << "DEBUG: About to read database." << logger::endL;
  if (!this->ReadAndIndexDatabase(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read and index database. ";
    }
    return false;
  }
  if (!this->UpdateOneNoLocks(findQuery, dataToMerge, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to update one. ";
    }
    return false;
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "DEBUG: Got to store db. about to store: " << this->reader.ToString(false);
  }
  return this->StoreDatabase(commentsOnFailure);
}

bool Database::FallBack::UpdateOneNoLocks(
  const QueryExact& findQuery,
  const JSData& dataToMerge,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::FallBack::UpdateOneNoLocks");
  if (!this->HasCollection(findQuery.collection, commentsOnFailure)) {
    return false;
  }
  int index = - 1;
  if (!this->FindIndexOneNoLocksMinusOneOnNotFound(findQuery, index, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to find one entry. ";
    }
    return false;
  }
  if (index == - 1) {
    index = this->reader[findQuery.collection].theList.size;
    JSData incoming;
    incoming.theType = JSData::token::tokenObject;
    this->reader[findQuery.collection].theList.AddOnTop(incoming);
  }
  JSData& modified = this->reader[findQuery.collection][index];
  if (dataToMerge.theType != JSData::token::tokenObject) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "I only know how to merge objects, you gave me: "
      << dataToMerge.ToString(false);
    }
    return false;
  }
  bool result = modified.MergeInMe(dataToMerge, commentsOnFailure);
  if (!result && commentsOnFailure != nullptr) {
    *commentsOnFailure << "Merge failed. ";
  }
  return result;
}

bool Database::FallBack::FindOne(
  const QueryExact& query,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::FallBack::FindOne");
  MutexProcessLockGuard guardDB(this->access);
  if (!this->ReadAndIndexDatabase(commentsOnFailure)) {
    global << "DEBUG:: failed to index DB!!!!!!!!!!!!!!!!!" << logger::endL;
    return false;
  }
  int index = - 1;
  if (!this->FindIndexOneNoLocksMinusOneOnNotFound(query, index, commentsOnFailure)) {
    global << "DEBUG:: find failed!!!!!!!!!!!!!!!!!" << logger::endL;
    return false;
  }
  global << "DEBUG:: GOT TO HERE!!!!!!!!!!!!!!!!!" << logger::endL;
  if (index < 0) {
    return false;
  }
  output = this->reader[query.collection][index];
  return true;
}

std::string Database::FallBack::ToStringIndices() const {
  std::stringstream out;
  out << this->indices.size() << " indices total.\n";
  int maxIndexedToDisplay = 3;
  for (int i = 0; i < this->indices.size(); i ++) {
    const MapList<std::string, List<int32_t>, MathRoutines::HashString>& currentLocation =
    this->indices.theValues[i].locations;
    out << this->indices.theKeys[i] << ": " << currentLocation.size() << " indexed: ";
    int numberToDisplay = MathRoutines::Minimum(currentLocation.size(), maxIndexedToDisplay);
    out << "[";
    for (int j = 0; j < numberToDisplay; j ++) {
      out << currentLocation.theKeys[j];
      if (j != numberToDisplay - 1) {
        out << ", ";
      }
    }
    if (numberToDisplay < currentLocation.size()) {
      out << ", ...";
    }
    out << "]\n";
  }
  out << "Known indices: " << this->knownIndices.ToStringCommaDelimited();
  return out.str();
}

bool Database::FallBack::FindIndexOneNoLocksMinusOneOnNotFound(
  const QueryExact& query,
  int& output,
  std::stringstream* commentsOnNotFound
) {
  output = - 1;
  if (!this->HasCollection(query.collection, commentsOnNotFound)) {
    if (commentsOnNotFound != nullptr) {
      *commentsOnNotFound << "Collection " << query.collection << " not found. ";
    }
    return false;
  }
  std::string key = query.getCollectionAndLabel();
  if (!this->indices.Contains(key)) {
    if (commentsOnNotFound != nullptr) {
      *commentsOnNotFound << "Finding by non-indexed key: "
      << key << " not allowed. Indices allowed: "
      << this->ToStringIndices() << ". ";
    }
    return false;
  }
  Database::FallBack::Index& currentIndex = indices.GetValueCreate(key);
  if (query.value.theType != JSData::token::tokenString) {
    if (commentsOnNotFound != nullptr) {
      *commentsOnNotFound << "At the moment, only string value queries are supported.";
    }
    return false;
  }
  int currentLocationIndex = currentIndex.locations.GetIndex(query.value.theString);
  if (currentLocationIndex == - 1) {
    if (commentsOnNotFound != nullptr ) {
      *commentsOnNotFound << "Entry " << query.value.ToString(false) << " not found. " ;
      *commentsOnNotFound << "DEBUG: keys present: " << currentIndex.locations.theKeys.ToStringCommaDelimited() << ". " ;
    }
    return true;
  }
  if (currentIndex.locations.theValues[currentLocationIndex].size == 0) {
    if (commentsOnNotFound != nullptr ) {
      *commentsOnNotFound << "Element not found. ";
    }
    return true;
  }
  output = currentIndex.locations.theValues[currentLocationIndex][0];
  return true;
}

bool Database::FallBack::FetchCollectionNames(
  List<std::string>& output, std::stringstream* commentsOnFailure
) {
  MutexProcessLockGuard guardDB(this->access);
  if (this->ReadDatabase(commentsOnFailure)) {
    return false;
  }
  output.SetSize(0);
  output.AddListOnTop(this->reader.objects.theKeys);
  return true;
}

bool Database::FallBack::HasCollection(
  const std::string& collection, std::stringstream* commentsOnFailure
) {
  if (Database::FallBack::knownCollectionS.Contains(collection)) {
    this->reader[collection].theType = JSData::token::tokenArray;
    return true;
  }
  if (this->reader.HasKey(collection)) {
    return true;
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Database collection "
    << collection << " not found. "
    << "Known collections: "
    << this->knownCollectionS.ToStringCommaDelimited()
    << ". ";
  }
  return false;
}

Database::FallBack::FallBack() {
  this->owner = nullptr;
  this->flagDatabaseRead = false;
}

void Database::FallBack::initialize() {
  this->access.CreateMe("databaseFallback", true, false);
  this->knownIndices.AddOnTop({
    DatabaseStrings::tableUsers + "." + DatabaseStrings::labelUsername
  });
  this->knownCollectionS.AddOnTop({
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
  MacroRegisterFunctionWithName("Database::FallBack::ReadAndIndexDatabase");
  if (!this->ReadDatabase(commentsOnFailure)) {
    return false;
  }
  for (int i = 0; i < this->knownIndices.size; i ++) {
    this->indices.GetValueCreate(this->knownIndices[i]);
  }
  for (int i = 0; i < this->reader.objects.size(); i ++) {
    std::string collection = this->reader.objects.theKeys[i];
    JSData& currentCollection = this->reader.objects.theValues[i];
    for (int i = 0; i < currentCollection.theList.size; i ++) {
      this->IndexOneRecord(currentCollection.theList[i], i, collection);
    }
  }
  global << "Database indexed. " << this->ToStringIndices() << logger::endL;
  return true;
}

void Database::FallBack::IndexOneRecord(
  const JSData& entry, int32_t row, const std::string& collection
) {
  global << "DEBUG: about to index record: " << entry.ToString(false) << logger::endL;
  if (entry.theType != JSData::token::tokenObject) {
    global << "DEBUG: not of type object! " << logger::endL;
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

bool Database::FallBack::StoreDatabase(std::stringstream* commentsOnFailure) {
  return FileOperations::WriteFileVirualWithPermissions("database_fallback/database.json", this->reader.ToString(false), true, commentsOnFailure);
}

bool Database::FallBack::ReadDatabase(std::stringstream* commentsOnFailure) {
  std::string theDatabase;
  if (!FileOperations::LoadFileToStringVirtual(
    "database_fallback/database.json", theDatabase, true, commentsOnFailure
  )) {
    if (!FileOperations::FileExistsVirtual(
      "database_fallback/database.json",
      true,
      false,
      commentsOnFailure
    )) {
      return false;
    }
  }
  return this->reader.readstring(theDatabase, commentsOnFailure);
}
