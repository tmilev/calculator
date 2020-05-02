// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "general_logging_global_variables.h"
#include "database.h"
#include "string_constants.h"

std::string Database::FallBack::databaseFilename = "database_fallback/database.json";

bool Database::FallBack::DeleteDatabase(std::stringstream* commentsOnFailure) {
  this->reader.reset(JSData::token::tokenObject);
  if (!FileOperations::writeFileVirual(Database::FallBack::databaseFilename, "{}", commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to delete database. ";
    }
    return false;
  }
  return true;
}

bool Database::FallBack::FindOneFromSome(
  const List<QueryExact>& findOrQueries,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  for (int i = 0; i < findOrQueries.size; i ++) {
    if (this->FindOne(findOrQueries[i], output, commentsOnFailure)) {
      return true;
    }
  }
  return false;
}

bool Database::FallBack::UpdateOne(
  const QueryExact& findQuery,
  const QuerySet& dataToMerge,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseFallback::UpdateOneFromQueryString");
  if (global.flagDisableDatabaseLogEveryoneAsAdmin) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Database::FallBack::UpdateOne failed. "
      << DatabaseStrings::errorDatabaseDisableD;
    }
    return false;
  }
  MutexProcessLockGuard guardDB(this->access);
  if (!this->ReadAndIndexDatabase(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read and index database. ";
    }
    global << "Failed to read and index database. " << Logger::endL;
    return false;
  }
  if (!this->UpdateOneNoLocks(findQuery, dataToMerge, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to update one. ";
    }
    return false;
  }
  return this->StoreDatabase(commentsOnFailure);
}

bool Database::FallBack::UpdateOneNoLocks(
  const QueryExact& findQuery,
  const QuerySet& dataToMerge,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::FallBack::UpdateOneNoLocks");
  if (!this->HasCollection(findQuery.collection, commentsOnFailure)) {
    return false;
  }
  int index = - 1;
  if (!this->FindIndexOneNoLocksMinusOneNotFound(findQuery, index, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to find one entry. ";
    }
    return false;
  }
  if (index == - 1) {
    index = this->reader[findQuery.collection].theList.size;
    JSData incoming;
    incoming.theType = JSData::token::tokenObject;
    this->reader[findQuery.collection].theList.addOnTop(incoming);
  }
  JSData* modified = &(this->reader[findQuery.collection][index]);
  for (int i = 0; i < dataToMerge.nestedLabels.size; i ++) {
    modified = &((*modified)[dataToMerge.nestedLabels[i]]);
  }
  for (int i = 0; i < dataToMerge.value.objects.size(); i ++) {
    (*modified)[dataToMerge.value.objects.theKeys[i]] = dataToMerge.value.objects.theValues[i];
  }
  return true;
}

bool Database::FallBack::FindOne(
  const QueryExact& query,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::FallBack::FindOne");
  MutexProcessLockGuard guardDB(this->access);
  if (!this->ReadAndIndexDatabase(commentsOnFailure)) {
    return false;
  }
  int index = - 1;
  if (!this->FindIndexOneNoLocksMinusOneNotFound(
    query, index, commentsOnFailure
  )) {
    return false;
  }
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
    const MapList<std::string, List<int32_t>, MathRoutines::hashString>& currentLocation =
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
  out << "Known indices: " << this->knownIndices.toStringCommaDelimited();
  return out.str();
}

bool Database::FallBack::FindIndexOneNoLocksMinusOneNotFound(
  const QueryExact& query,
  int& output,
  std::stringstream* commentsOnNotFound
) {
  MacroRegisterFunctionWithName("Database::FallBack::FindIndexOneNoLocksMinusOneNotFound");
  output = - 1;
  if (!this->HasCollection(query.collection, commentsOnNotFound)) {
    if (commentsOnNotFound != nullptr) {
      *commentsOnNotFound << "Collection " << query.collection << " not found. ";
    }
    return false;
  }
  std::string key = query.getCollectionAndLabel();
  if (!this->indices.contains(key)) {
    if (commentsOnNotFound != nullptr) {
      *commentsOnNotFound << "Finding by non-indexed key: "
      << key << " not allowed. Indices allowed: "
      << this->ToStringIndices() << ". ";
    }
    return false;
  }
  Database::FallBack::Index& currentIndex = indices.getValueCreate(key);
  if (query.value.theType != JSData::token::tokenString) {
    if (commentsOnNotFound != nullptr) {
      *commentsOnNotFound << "At the moment, only string value queries are supported.";
    }
    return false;
  }
  int currentLocationIndex = currentIndex.locations.getIndex(query.value.theString);
  if (currentLocationIndex == - 1) {
    if (commentsOnNotFound != nullptr ) {
      *commentsOnNotFound << "Entry "
      << query.value.toString(nullptr) << " not found. " ;
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
  if (!this->ReadDatabase(commentsOnFailure)) {
    return false;
  }
  output.setSize(0);
  output.addListOnTop(this->reader.objects.theKeys);
  return true;
}

bool Database::FallBack::HasCollection(
  const std::string& collection, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::FallBack::HasCollection");
  if (Database::FallBack::knownCollectionS.contains(collection)) {
    this->reader[collection].theType = JSData::token::tokenArray;
    return true;
  }
  if (this->reader.hasKey(collection)) {
    return true;
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Database collection "
    << collection << " not found. "
    << "Known collections: "
    << this->knownCollectionS.toStringCommaDelimited()
    << ". ";
  }
  return false;
}

Database::FallBack::FallBack() {
  this->owner = nullptr;
  this->flagDatabaseRead = false;
}

void Database::FallBack::initialize() {
  this->access.CreateMe("databaseFallback", false);
  this->knownIndices.addOnTop({
    DatabaseStrings::tableUsers + "." + DatabaseStrings::labelUsername
  });
  this->knownCollectionS.addOnTop({
    DatabaseStrings::tableUsers
  });
}

void Database::FallBack::CreateHashIndex(const std::string& collectionName, const std::string& theKey) {
  Database::FallBack::Index newIndex;
  newIndex.collection = collectionName;
  newIndex.label = theKey;
  newIndex.collectionAndLabelCache = newIndex.collectionAndLabel();
  this->indices.setKeyValue(newIndex.collectionAndLabelCache, newIndex);
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
    this->indices.getValueCreate(this->knownIndices[i]);
  }
  for (int i = 0; i < this->reader.objects.size(); i ++) {
    std::string collection = this->reader.objects.theKeys[i];
    JSData& currentCollection = this->reader.objects.theValues[i];
    for (int i = 0; i < currentCollection.theList.size; i ++) {
      this->IndexOneRecord(currentCollection.theList[i], i, collection);
    }
  }
  // global << "Database indexed. " << this->ToStringIndices() << Logger::endL;
  return true;
}

void Database::FallBack::IndexOneRecord(
  const JSData& entry, int32_t row, const std::string& collection
) {
  if (entry.theType != JSData::token::tokenObject) {
    return;
  }
  for (int i = 0; i < entry.objects.size(); i ++) {
    std::string indexLabel = Database::FallBack::Index::collectionAndLabelStatic(collection, entry.objects.theKeys[i]);
    if (!this->indices.contains(indexLabel)) {
      continue;
    }
    const JSData& keyToIndexBy = entry.objects.theValues[i];
    if (keyToIndexBy.theType != JSData::token::tokenString) {
      continue;
    }
    Database::FallBack::Index& currentIndex = this->indices.getValueCreate(indexLabel);
    currentIndex.locations.getValueCreate(keyToIndexBy.theString).addOnTop(row);
  }
}

bool Database::FallBack::StoreDatabase(std::stringstream* commentsOnFailure) {
  return FileOperations::writeFileVirualWithPermissions(
    Database::FallBack::databaseFilename,
    this->reader.toString(nullptr),
    true,
    commentsOnFailure
  );
}

bool Database::FallBack::ReadDatabase(std::stringstream* commentsOnFailure) {
  std::string theDatabase;
  if (!FileOperations::loadFileToStringVirtual(
    Database::FallBack::databaseFilename, theDatabase, true, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Database load failed. ";
    }
    if (!FileOperations::fileExistsVirtual(
      Database::FallBack::databaseFilename,
      true,
      false,
      commentsOnFailure
    )) {
      global << Logger::green << "Fallback database file does not exist. Creating ..." << Logger::endL;
      this->reader.theType = JSData::token::tokenObject;
      return true;
    }
    return false;
  }
  global << "Database size: " << Logger::blue << theDatabase.size() << Logger::endL;
  return this->reader.readstring(theDatabase, commentsOnFailure);
}
