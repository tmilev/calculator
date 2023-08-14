#include "database.h"
#include "general_logging_global_variables.h"
#include "general_file_operations_encodings.h"
#include "general_strings.h"
#include "string_constants.h"
#include "calculator_problem_storage.h"
#include "web_api.h"

bool DatabaseLoader::writeBackup() {
  STACK_TRACE("DatabaseLoader::writeBackup");
  std::stringstream comments;
  Database::name = "local";
  DatabaseInternalServer& database = Database::get().localDatabase.server;
  database.initializeLoadFromHardDrive();
  std::string jsonName =
  "database_backups/automated_backups/" +
  global.getDateForLogFiles() +
  "/local.json";
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
  global
  << Logger::green
  << "Backed up your current database in: "
  << Logger::purple
  << jsonName
  << Logger::endL;
  return true;
}

bool DatabaseLoader::loadDatabase(
  const std::string& databaseName, std::stringstream& comments
) {
  DatabaseLoader loader;
  return loader.doLoadDatabase(databaseName, comments);
}

bool DatabaseLoader::doLoadDatabase(
  const std::string& databaseName, std::stringstream& comments
) {
  STACK_TRACE("DatabaseLoader::loadDatabase");
  this->fileName = databaseName;
  global << "Loading database from: " << this->fileName << Logger::endL;
  if (!this->loadJSONFromHardDrive(comments)) {
    return false;
  }
  if (!this->correctDatabaseJSON(comments)) {
    return false;
  }
  return this->loadFromJSON(this->databaseJSON, comments);
}

bool DatabaseLoader::loadJSONFromHardDrive(std::stringstream& comments) {
  // TODO(tmilev): this snippet is deprecated. Delete soon.
  // if (!StringRoutines::stringEndsWith(this->fileName, ".json")) {
  // MongoDatabaseDumpToJSONConverter converter;
  // return converter.load(this->fileName, databaseJSON, comments);
  // }
  std::string jsonString;
  int64_t startingTime = global.getElapsedMilliseconds();
  if (
    !FileOperations::
    loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(
      this->fileName, jsonString, true, true, &comments
    )
  ) {
    return false;
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

bool MongoDatabaseDumpToJSONConverter::load(
  const std::string& folderNameFromUser,
  JSData& output,
  std::stringstream& commentsOnFailure
) {
  STACK_TRACE("MongoDatabaseDumpToJSONConverter::load");
  std::string folderName = folderNameFromUser;
  if (!StringRoutines::stringEndsWith(folderName, "/")) {
    folderName += "/";
  }
  FileOperations::getFolderFileNamesVirtual(
    folderName,
    this->collectionFileNames,
    nullptr,
    true,
    true,
    &commentsOnFailure
  );
  for (int i = 0; i < this->collectionFileNames.size; i ++) {
    if (
      this->collectionFileNames[i] == ".." ||
      this->collectionFileNames[i] == "."
    ) {
      this->collectionFileNames.removeIndexShiftDown(i);
      i --;
    }
  }
  global << "Found collections: " << this->collectionFileNames << Logger::endL;
  output.reset();
  for (const std::string& collectionFileName : this->collectionFileNames) {
    JSData currentCollection;
    std::string fileName = folderName + collectionFileName;
    if (
      !this->loadOneCollection(
        fileName, currentCollection, commentsOnFailure
      )
    ) {
      return false;
    }
    List<std::string> split;
    StringRoutines::splitExcludeDelimiter(collectionFileName, '.', split);
    std::string collectionName = split[0];
    output[collectionName] = currentCollection;
  }
  return true;
}

bool MongoDatabaseDumpToJSONConverter::loadOneCollection(
  const std::string& collectionFileName,
  JSData& output,
  std::stringstream& comments
) {
  STACK_TRACE("MongoDatabaseDumpToJSONConverter::loadOneCollection");
  List<std::string> split;
  StringRoutines::splitExcludeDelimiter(collectionFileName, '.', split);
  std::string collectionName = split[0];
  std::string contentString;
  if (
    !FileOperations::
    loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(
      collectionFileName, contentString, true, true, &comments
    )
  ) {
    return false;
  }
  int64_t startingTime = global.getElapsedMilliseconds();
  bool result =
  this->collectionFromFileContent(contentString, output, comments);
  int64_t elapsed = global.getElapsedMilliseconds() - startingTime;
  int64_t speed = contentString.size();
  global
  << "Parsed json of size: "
  << Logger::blue
  << contentString.size()
  << " bytes"
  << Logger::normalColor
  << " in "
  << Logger::purple
  << elapsed
  << Logger::normalColor
  << " milliseconds. "
  << Logger::endL;
  if (elapsed != 0) {
    speed /= elapsed;
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

bool MongoDatabaseDumpToJSONConverter::collectionFromFileContent(
  const std::string& input,
  JSData& output,
  std::stringstream& comments
) {
  STACK_TRACE(
    "MongoDatabaseDumpToJSONConverter::collectionJSONFromFileContent"
  );
  output.makeEmptyArray();
  List<std::string> recordStrings;
  StringRoutines::splitExcludeDelimiter(input, '\n', recordStrings);
  for (const std::string& recordString : recordStrings) {
    JSData record;
    if (!record.parse(recordString, true, &comments)) {
      comments << "Failed to parse json. ";
      return false;
    }
    output.listObjects.addOnTop(record);
  }
  return true;
}
