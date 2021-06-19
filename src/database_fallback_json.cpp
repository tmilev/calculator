// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "general_logging_global_variables.h"
#include "database.h"
#include "string_constants.h"

std::string Database::FallBack::databaseFilename = "database_fallback/database.json";

bool Database::FallBack::deleteDatabase(std::stringstream* commentsOnFailure) {
  this->reader.reset(JSData::token::tokenObject);
  if (!FileOperations::writeFileVirual(Database::FallBack::databaseFilename, "{}", commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to delete database. ";
    }
    return false;
  }
  return true;
}

bool Database::FallBack::findOneFromSome(
  const List<QueryExact>& findOrQueries,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  for (int i = 0; i < findOrQueries.size; i ++) {
    if (this->findOne(findOrQueries[i], output, commentsOnFailure)) {
      return true;
    }
  }
  return false;
}

bool Database::FallBack::updateOne(
  const QueryExact& findQuery,
  const QuerySet& dataToMerge,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseFallback::updateOneFromQueryString");
  if (global.flagDisableDatabaseLogEveryoneAsAdmin) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Database::FallBack::updateOne failed. "
      << DatabaseStrings::errorDatabaseDisableD;
    }
    return false;
  }
  MutexProcesslockGuard guardDB(this->access);
  if (!this->readAndIndexDatabase(commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to read and index database. ";
    }
    global << "Failed to read and index database. " << Logger::endL;
    return false;
  }
  if (!this->updateOneNolocks(findQuery, dataToMerge, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to update one. ";
    }
    return false;
  }
  return this->storeDatabase(commentsOnFailure);
}

bool Database::FallBack::updateOneNolocks(
  const QueryExact& findQuery,
  const QuerySet& dataToMerge,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::FallBack::updateOneNolocks");
  if (!this->hasCollection(findQuery.collection, commentsOnFailure)) {
    return false;
  }
  int index = - 1;
  if (!this->findIndexOneNolocksMinusOneNotFound(findQuery, index, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to find one entry. ";
    }
    return false;
  }
  if (index == - 1) {
    index = this->reader[findQuery.collection].listObjects.size;
    JSData incoming;
    incoming.elementType = JSData::token::tokenObject;
    this->reader[findQuery.collection].listObjects.addOnTop(incoming);
  }
  JSData* modified = &(this->reader[findQuery.collection][index]);
  for (int i = 0; i < dataToMerge.nestedLabels.size; i ++) {
    modified = &((*modified)[dataToMerge.nestedLabels[i]]);
  }
  for (int i = 0; i < dataToMerge.value.objects.size(); i ++) {
    (*modified)[dataToMerge.value.objects.keys[i]] = dataToMerge.value.objects.values[i];
  }
  return true;
}

bool Database::FallBack::findOne(
  const QueryExact& query,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::FallBack::findOne");
  MutexProcesslockGuard guardDB(this->access);
  if (!this->readAndIndexDatabase(commentsOnFailure)) {
    return false;
  }
  int index = - 1;
  if (!this->findIndexOneNolocksMinusOneNotFound(
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

std::string Database::FallBack::toStringIndices() const {
  std::stringstream out;
  out << this->indices.size() << " indices total.\n";
  int maxIndexedToDisplay = 3;
  for (int i = 0; i < this->indices.size(); i ++) {
    const MapList<std::string, List<int32_t>, MathRoutines::hashString>& currentLocation =
    this->indices.values[i].locations;
    out << this->indices.keys[i] << ": " << currentLocation.size() << " indexed: ";
    int numberToDisplay = MathRoutines::minimum(currentLocation.size(), maxIndexedToDisplay);
    out << "[";
    for (int j = 0; j < numberToDisplay; j ++) {
      out << currentLocation.keys[j];
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

bool Database::FallBack::findIndexOneNolocksMinusOneNotFound(
  const QueryExact& query,
  int& output,
  std::stringstream* commentsOnNotFound
) {
  MacroRegisterFunctionWithName("Database::FallBack::findIndexOneNolocksMinusOneNotFound");
  output = - 1;
  if (!this->hasCollection(query.collection, commentsOnNotFound)) {
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
      << this->toStringIndices() << ". ";
    }
    return false;
  }
  Database::FallBack::Index& currentIndex = indices.getValueCreateEmpty(key);
  if (query.value.elementType != JSData::token::tokenString) {
    if (commentsOnNotFound != nullptr) {
      *commentsOnNotFound << "At the moment, only string value queries are supported.";
    }
    return false;
  }
  int currentLocationIndex = currentIndex.locations.getIndex(query.value.stringValue);
  if (currentLocationIndex == - 1) {
    if (commentsOnNotFound != nullptr ) {
      *commentsOnNotFound << "Entry "
      << query.value.toString(nullptr) << " not found. " ;
    }
    return true;
  }
  if (currentIndex.locations.values[currentLocationIndex].size == 0) {
    if (commentsOnNotFound != nullptr ) {
      *commentsOnNotFound << "Element not found. ";
    }
    return true;
  }
  output = currentIndex.locations.values[currentLocationIndex][0];
  return true;
}

bool Database::FallBack::fetchCollectionNames(
  List<std::string>& output, std::stringstream* commentsOnFailure
) {
  MutexProcesslockGuard guardDB(this->access);
  if (!this->readDatabase(commentsOnFailure)) {
    return false;
  }
  output.setSize(0);
  output.addListOnTop(this->reader.objects.keys);
  return true;
}

bool Database::FallBack::hasCollection(
  const std::string& collection, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::FallBack::hasCollection");
  if (Database::FallBack::knownCollectionS.contains(collection)) {
    this->reader[collection].elementType = JSData::token::tokenArray;
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
  this->access.createMe("databaseFallback", false);
  this->knownIndices.addOnTop({
    DatabaseStrings::tableUsers + "." + DatabaseStrings::labelUsername
  });
  this->knownCollectionS.addOnTop({
    DatabaseStrings::tableUsers
  });
}

void Database::FallBack::createHashIndex(const std::string& collectionName, const std::string& theKey) {
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

bool Database::FallBack::readAndIndexDatabase(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("Database::FallBack::readAndIndexDatabase");
  if (!this->readDatabase(commentsOnFailure)) {
    return false;
  }
  for (int i = 0; i < this->knownIndices.size; i ++) {
    this->indices.getValueCreateEmpty(this->knownIndices[i]);
  }
  for (int i = 0; i < this->reader.objects.size(); i ++) {
    std::string collection = this->reader.objects.keys[i];
    JSData& currentCollection = this->reader.objects.values[i];
    for (int i = 0; i < currentCollection.listObjects.size; i ++) {
      this->indexOneRecord(currentCollection.listObjects[i], i, collection);
    }
  }
  // global << "Database indexed. " << this->toStringIndices() << Logger::endL;
  return true;
}

void Database::FallBack::indexOneRecord(
  const JSData& entry, int32_t row, const std::string& collection
) {
  if (entry.elementType != JSData::token::tokenObject) {
    return;
  }
  for (int i = 0; i < entry.objects.size(); i ++) {
    std::string indexLabel = Database::FallBack::Index::collectionAndLabelStatic(collection, entry.objects.keys[i]);
    if (!this->indices.contains(indexLabel)) {
      continue;
    }
    const JSData& keyToIndexBy = entry.objects.values[i];
    if (keyToIndexBy.elementType != JSData::token::tokenString) {
      continue;
    }
    Database::FallBack::Index& currentIndex = this->indices.getValueCreateEmpty(indexLabel);
    currentIndex.locations.getValueCreateEmpty(keyToIndexBy.stringValue).addOnTop(row);
  }
}

bool Database::FallBack::storeDatabase(std::stringstream* commentsOnFailure) {
  return FileOperations::writeFileVirualWithPermissions(
    Database::FallBack::databaseFilename,
    this->reader.toString(nullptr),
    true,
    commentsOnFailure
  );
}

bool Database::FallBack::readDatabase(std::stringstream* commentsOnFailure) {
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
      this->reader.elementType = JSData::token::tokenObject;
      return true;
    }
    return false;
  }
  global << "Database size: " << Logger::blue << theDatabase.size() << Logger::endL;
  return this->reader.parse(theDatabase, commentsOnFailure);
}
