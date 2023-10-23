#include "database.h"
#include "general_logging_global_variables.h"
#include "general_file_operations_encodings.h"
#include "general_strings.h"
#include "string_constants.h"
#include "calculator_problem_storage.h"
#include "web_api.h"

bool DatabaseLoader::writeBackup(std::string& outputLoadDatabaseCommand) {
  STACK_TRACE("DatabaseLoader::writeBackup");
  std::stringstream comments;
  Database::name = "local";
  DatabaseInternalServer& database = Database::get().localDatabase.server;
  database.initializeLoadFromHardDrive();
  std::string jsonName =
  "database_backups/automated_backups/" +
  global.getDateForLogFiles() +
  "/local.json";
  std::string jsonPhysicalFilename;
  FileOperations::getPhysicalFileNameFromVirtual(
    jsonName, jsonPhysicalFilename, true, true, nullptr
  );
  std::string content = database.toJSONDatabase().toString();
  if (
    !FileOperations::
    writeFileVirualWithPermissions_accessUltraSensitiveFoldersIfNeeded(
      jsonName, content, true, true, &comments
    )
  ) {
    global << "Failed to write database: " << jsonName << Logger::endL;
    return false;
  }
  outputLoadDatabaseCommand =
  "./calculator load_database " + jsonPhysicalFilename;
  global << "Backed up your current database in: " << jsonName << Logger::endL;
  return true;
}

bool DatabaseLoader::loadDatabase(
  const std::string& databaseName,
  bool usePhysicalFilename,
  std::stringstream& comments
) {
  DatabaseLoader loader;
  return loader.doLoadDatabase(databaseName, usePhysicalFilename, comments);
}

bool DatabaseLoader::doLoadDatabase(
  const std::string& databaseName,
  bool usePhysicalFilename,
  std::stringstream& comments
) {
  STACK_TRACE("DatabaseLoader::loadDatabase");
  this->fileName = databaseName;
  global << "Loading database from: " << this->fileName << Logger::endL;
  if (!this->loadJSONFromHardDrive(comments, usePhysicalFilename)) {
    return false;
  }
  if (!this->correctDatabaseJSON(comments)) {
    return false;
  }
  return this->loadFromJSON(this->databaseJSON, comments);
}

bool DatabaseLoader::loadJSONFromHardDrive(
  std::stringstream& comments, bool usePhysicalFileName
) {
  std::string jsonString;
  int64_t startingTime = global.getElapsedMilliseconds();
  if (usePhysicalFileName) {
    if (
      !FileOperations::loadFileToStringUnsecure(
        this->fileName, jsonString, &comments
      )
    ) {
      return false;
    }
  } else {
    if (
      !FileOperations::
      loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(
        this->fileName, jsonString, true, true, &comments
      )
    ) {
      return false;
    }
  }
  bool result = databaseJSON.parse(jsonString, true, &comments);
  int64_t elapsed = global.getElapsedMilliseconds() - startingTime;
  int64_t size = jsonString.size();
  global
  << "Parsed json of size: "
  << Logger::blue
  << jsonString.size()
  << " bytes"
  << Logger::normalColor
  << " in "
  << Logger::purple
  << elapsed
  << Logger::normalColor
  << " milliseconds. "
  << Logger::endL;
  if (elapsed != 0) {
    int64_t speed = size / elapsed;
    global
    << "Speed: "
    << Logger::red
    << "~"
    << speed
    << " bytes/ms. "
    << Logger::endL;
  }
  return result;
}

bool DatabaseLoader::loadFromJSON(
  JSData& input, std::stringstream& comments
) {
  DatabaseInternalServer& database = Database::get().localDatabase.server;
  database.ensureStandardCollectionIndices();
  for (const std::string& collectionName : input.objects.keys) {
    JSData& content = input[collectionName];
    if (
      !this->loadOneCollectionFromJSON(collectionName, content, comments)
    ) {
      return false;
    }
  }
  return true;
}

bool DatabaseLoader::loadOneCollectionFromJSON(
  const std::string& collectionName,
  JSData& input,
  std::stringstream& comments
) {
  STACK_TRACE("DatabaseLoader::loadOneCollectionFromJSON");
  DatabaseInternalServer& database = Database::get().localDatabase.server;
  if (input.elementType != JSData::Type::tokenArray) {
    return false;
  }
  DatabaseCollection& collection =
  database.collections.getValueCreateEmpty(collectionName);
  int counter = 0;
  for (JSData& object : input.listObjects) {
    if (!this->loadOneObject(collection, object, comments)) {
      return false;
    }
    counter ++;
    global
    << "Loaded object "
    << counter
    << " out of "
    << input.listObjects.size
    << Logger::endL;
  }
  return collection.storeIndicesToHardDrive(&comments);
}

bool DatabaseLoader::loadOneObject(
  DatabaseCollection& collection,
  JSData& input,
  std::stringstream& comments
) {
  STACK_TRACE("DatabaseLoader::loadOneObject");
  std::string objectId;
  objectId = input[DatabaseStrings::labelId].stringValue;
  if (objectId == "") {
    // If the object comes from a deprecated use of mongoDB, it's id may be
    // here:
    objectId = input["_id"]["$oid"].stringValue;
  }
  if (objectId == "") {
    comments
    << "In loadOneObject: failed to load object:\n"
    << input.toString();
    return false;
  }
  input[DatabaseStrings::labelId] = objectId;
  // bool unused=false;
  // collection.indexOfObjectIds().setObjectIdValue(objectId, objectId,
  // unused);
  collection.updateObjectInIndexReturnTrueIfChanged(objectId, input);
  DatabaseInternalServer& database = Database::get().localDatabase.server;
  if (
    !database.storeObject(
      objectId, collection.name, input, false, &comments
    )
  ) {
    return false;
  }
  return true;
}

bool DatabaseLoader::correctDatabaseJSON(
  std::stringstream& commentsOnFailure
) {
  JSData& users = this->databaseJSON["users"];
  for (JSData& user : users.listObjects) {
    if (!this->correctUser(user, commentsOnFailure)) {
      return false;
    }
  }
  return true;
}

bool DatabaseLoader::correctUser(
  JSData& inputOutput, std::stringstream& commentsOnFailure
) {
  if (!inputOutput.objects.contains("problemData")) {
    return true;
  }
  global
  << "Correct user: "
  << Logger::green
  << inputOutput["username"]
  << Logger::endL;
  std::string data = inputOutput["problemData"].stringValue;
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > mapStrings;
  if (
    !HtmlRoutines::chopPercentEncodedString(
      data, mapStrings, commentsOnFailure
    )
  ) {
    return false;
  }
  ProblemData reader;
  std::string probNameNoWhiteSpace;
  for (int i = 0; i < mapStrings.size(); i ++) {
    if (
      !reader.loadFromOldFormatDeprecated(
        HtmlRoutines::convertURLStringToNormal(
          mapStrings.values[i], false
        ),
        commentsOnFailure
      )
    ) {
      return false;
    }
    probNameNoWhiteSpace =
    StringRoutines::stringTrimWhiteSpace(
      HtmlRoutines::convertURLStringToNormal(mapStrings.keys[i], false)
    );
    if (probNameNoWhiteSpace == "") {
      continue;
    }
    inputOutput["problemDataJSON"][probNameNoWhiteSpace] = reader.storeJSON();
  }
  inputOutput.objects.removeKey("problemData");
  return true;
}
