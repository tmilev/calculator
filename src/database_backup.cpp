#include "database.h"
#include "general_logging_global_variables.h"
#include "general_file_operations_encodings.h"
#include "general_strings.h"

bool DatabaseLoader::writeBackup(){
  STACK_TRACE("DatabaseLoader::writeBackup");
  std::stringstream comments;
  Database::name = "local";
  DatabaseInternalServer &database = Database::get().localDatabase.server;
  List<std::string> collectionNames;
  database.initializeLoadFromHardDrive();
  std::string folderName = "database_backups/automated_backups/" + global.getDateForLogFiles() + "/";
  for (DatabaseCollection& collection : database.collections.values){
    JSData result;

    collection.toJSON(result);
    std::string collectionName = folderName+collection.name;
    if (!   FileOperations::writeFileVirualWithPermissions_accessUltraSensitiveFoldersIfNeeded(collectionName, result.toString(), true, true, &comments)){
      global << "Failed to write collection: " << collectionName << Logger::endL;
      return false;
    }

  }
  global << Logger::green  << "Backed up your current database in: "  << Logger::purple << folderName << Logger::endL;

  return true;
}


bool DatabaseLoader::loadDatabase(const std::string& databaseName,   std::stringstream &comments
){
  DatabaseLoader loader;
  return loader.doLoadDatabase(databaseName, comments);
}

  bool DatabaseLoader::doLoadDatabase(const std::string& databaseName, std::stringstream& comments){
  STACK_TRACE("DatabaseLoader::loadDatabase");
  this->folderName = "database_backups/"+databaseName + "/";
  global << "Folder to load from: " << this->folderName << Logger::endL;
  FileOperations::getFolderFileNamesVirtual(this->folderName, this->collectionFileNames, nullptr, true, true,&comments );
  for (int i = 0; i < this->collectionFileNames.size; i ++){
    if (this->collectionFileNames[i] == ".." ||this-> collectionFileNames[i] == ".") {
      this->collectionFileNames.removeIndexShiftDown(i);
      i--;
    }
  }
  global << "Found collections: " << this->collectionFileNames << Logger::endL;
  bool result = true;
  DatabaseInternalServer& database = Database::get().localDatabase.server;
  database.ensureStandardCollectionIndices();

  for (const std::string& collectionFileName: this->collectionFileNames){
result = result ||    this->loadOneCollection(collectionFileName, comments);
  }


  return true;
}

  bool DatabaseLoader::loadOneCollection(const std::string& collectionFileName, std::stringstream& comments){
  STACK_TRACE("DatabaseLoader::loadOneCollection");
  List<std::string> split;
  StringRoutines::splitExcludeDelimiter(collectionFileName, '.', split);
  std::string collectionName = split[0];
  std::string contentString;
  if (!FileOperations::loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(this->folderName+ collectionFileName, contentString, true, true, &comments)){
return false;
  }
  JSData content;
  if (!this->collectionJSONFromFileContent(contentString, content, comments)){
return false;
  }
  return this->loadOneCollectionFromJSON(collectionFileName, content, comments);

  }


  bool DatabaseLoader::collectionJSONFromFileContent(const std::string& input, JSData& output, std::stringstream& comments){
  STACK_TRACE("DatabaseLoader::collectionJSONFromFileContent");
  if (output.parse(input,true, nullptr)){
  return true;
  }
  output.makeEmptyArray();
  global << "Failed to parse collection as regular json. "
         << "Attempting to parse as a newline split list of jsons." <<Logger::endL;
  List<std::string> recordStrings;

  StringRoutines::splitExcludeDelimiter(input, '\n', recordStrings);
  for (const std::string& recordString: recordStrings){
  JSData record;
  if (!  record.parse(recordString,true, &comments)){
      comments << "Failed to parse json. ";
      return false;
  }
  output.listObjects.addOnTop(record);
  }
  return true;
  }

  bool DatabaseLoader::loadOneCollectionFromJSON(const std::string& input, JSData& output, std::stringstream& comments){
  DatabaseInternalServer& database = Database::get().localDatabase.server;

  }
