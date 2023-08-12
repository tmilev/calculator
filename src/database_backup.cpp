#include "database.h"
#include "string_constants.h"
#include "general_logging_global_variables.h"
#include "general_strings.h"
#include "web_api.h"
#include "general_file_operations_encodings.h"

bool DatabaseLoader::writeBackup(){
  STACK_TRACE("DatabaseLoader::writeBackup");
  std::stringstream comments;
  Database::name = "local";
  DatabaseInternalServer &database = Database::get().localDatabase.server;
  List<std::string> collectionNames;
  database.initializeLoadFromHardDrive();
  std::string folderName = "database_backups/backup/" + global.getDateForLogFiles() + "/";
  for (DatabaseCollection& collection : database.collections.values){
    JSData result;

    collection.toJSON(result);
    std::string collectionName = folderName+collection.name;
    if (!   FileOperations::writeFileVirualWithPermissions_accessUltraSensitiveFoldersIfNeeded(collectionName, result.toString(), true, true, &comments)){
      global << "Failed to write collection: " << collectionName << Logger::endL;
      return false;
    }

  }

  return true;
}


bool DatabaseLoader::loadDatabase(const std::string& databaseName){
  return false;
}
