#ifdef MACRO_use_MongoDB
#include <mongoc.h>
#include <bcon.h>
mongoc_client_t* databaseClient = 0;
mongoc_database_t *database = 0;
#endif //MACRO_use_MongoDB
#include "vpfheader7databaseinterface_mongodb.h"
#include "vpfJson.h"
ProjectInformationInstance ProjectInfoVpfDatabaseMongo(__FILE__, "Database mongoDB.");

int DatabaseRoutinesGlobalFunctionsMongo::numDatabaseInstancesMustBeOneOrZero = 0;
DatabaseRoutinesGlobalFunctionsMongo databaseMongo;

extern logger logWorker;

DatabaseRoutinesGlobalFunctionsMongo::DatabaseRoutinesGlobalFunctionsMongo()
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::DatabaseRoutinesGlobalFunctionsMongo");
#ifdef MACRO_use_MongoDB
  mongoc_init();
  databaseClient = mongoc_client_new("mongodb://localhost:27017");
  database = mongoc_client_get_database(databaseClient, DatabaseStrings::theDatabaseNameMongo.c_str());
  DatabaseRoutinesGlobalFunctionsMongo::CreateHashIndex(DatabaseStrings::tableUsers, DatabaseStrings::labelUsername);
  DatabaseRoutinesGlobalFunctionsMongo::CreateHashIndex(DatabaseStrings::tableUsers, DatabaseStrings::labelEmail);
  DatabaseRoutinesGlobalFunctionsMongo::CreateHashIndex(DatabaseStrings::tableUsers, DatabaseStrings::labelInstructor);
  DatabaseRoutinesGlobalFunctionsMongo::CreateHashIndex(DatabaseStrings::tableUsers, DatabaseStrings::labelUserRole);
#endif
}

DatabaseRoutinesGlobalFunctionsMongo::~DatabaseRoutinesGlobalFunctionsMongo()
{
#ifdef MACRO_use_MongoDB
  if (database != 0)
  { mongoc_database_destroy(database);
    database = 0;
  }
  if (databaseClient != 0)
  { mongoc_client_destroy(databaseClient);
    databaseClient = 0;
  }
  mongoc_cleanup();
#endif
}

#ifdef MACRO_use_MongoDB
class MongoCollection
{
public:
  mongoc_collection_t* collection;
  std::string name;
  MongoCollection(const std::string& collectionName);
  ~MongoCollection();
};

MongoCollection::MongoCollection(const std::string& collectionName)
{ this->name = collectionName;
  this->collection = mongoc_client_get_collection
  (databaseClient, DatabaseStrings::theDatabaseNameMongo.c_str(), this->name.c_str());
}

MongoCollection::~MongoCollection()
{ mongoc_collection_destroy(this->collection);
  this->collection = 0;
}

class MongoQuery
{
public:
  mongoc_cursor_t* cursor;
  bson_t* query;
  bson_t* update;
  bson_t* options;
  bson_t* updateResult;
  bson_t* command;
  bson_error_t theError;
  int maxOutputItems;
  long long totalItems;
  std::string findQuery;
  std::string updateQuery;
  std::string optionsQuery;
  std::string collectionName;
  bool flagFindOneOnly;
  MongoQuery();
  ~MongoQuery();
  bool FindMultiple(List<JSData>& output, std::stringstream* commentsOnFailure);
  bool UpdateOne(std::stringstream* commentsOnFailure);
};

MongoQuery::MongoQuery()
{ this->query = 0;
  this->command = 0;
  this->update = 0;
  this->options = 0;
  this->cursor = 0;
  this->updateResult = 0;
  this->maxOutputItems = - 1;
  this->totalItems = - 1;
  this->flagFindOneOnly = false;
}

MongoQuery::~MongoQuery()
{ if (this->command != 0)
  { bson_destroy (this->command);
    this->command = 0;
  }
  if (this->cursor != 0)
  { mongoc_cursor_destroy(this->cursor);
    this->cursor = 0;
  }
  if (this->updateResult != 0)
  { bson_destroy(this->updateResult);
    this->updateResult = 0;
  }
  if (this->options != 0)
  { bson_destroy(this->options);
    this->options = 0;
  }
  if (this->update != 0)
  { bson_destroy(this->update);
    this->update = 0;
  }
  if (this->query != 0)
  { bson_destroy(this->query);
    this->query = 0;
  }
}

bool MongoQuery::UpdateOne(std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("MongoQuery::UpdateOne");
  MongoCollection theCollection(this->collectionName);
  logWorker << "Update: " << this->findQuery << " to: "
  << this->updateQuery << " inside: " << this->collectionName << logger::endL;
  if (this->query != 0)
    crash << "At this point of code, query is supposed to be 0. " << crash;
  this->query = bson_new_from_json
  ((const uint8_t*) this->findQuery.c_str(), this->findQuery.size(), &this->theError);
  this->update = bson_new_from_json
  ((const uint8_t*) this->updateQuery.c_str(), this->updateQuery.size(), &this->theError);
  this->optionsQuery = "{\"upsert\": true}";
  this->options = bson_new_from_json
  ((const uint8_t*) this->optionsQuery.c_str(), this->optionsQuery.size(), &this->theError);
  this->updateResult = bson_new();
  if (!mongoc_collection_update_one
      (theCollection.collection, this->query, this->update,
       this->options,
       this->updateResult,
       &this->theError))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << this->theError.message;
    logWorker << logger::red << "Mongo error: " << this->theError.message << logger::endL;
    return false;
  }
  char* bufferOutpurStringFormat = 0;
  bufferOutpurStringFormat = bson_as_canonical_extended_json(this->updateResult, NULL);
  std::string updateResultString(bufferOutpurStringFormat);
  bson_free(bufferOutpurStringFormat);
  logWorker << logger::red << "Update result: " << updateResultString << logger::endL;
  return true;
}

bool MongoQuery::FindMultiple(List<JSData>& output, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("MongoQuery::FindMultiple");
  MongoCollection theCollection(this->collectionName);
  //logWorker << "Query: " << this->findQuery << " inside: " << this->collectionName << logger::endL;
  if (this->query != 0)
    crash << "At this point of code, query is supposed to be 0. " << crash;
  this->query = bson_new_from_json((const uint8_t*) this->findQuery.c_str(), this->findQuery.size(), &this->theError);
  if (this->query == NULL)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << this->theError.message;
    logWorker << logger::red << "Mongo error: " << this->theError.message << logger::endL;
    return false;
  }
  this->cursor = mongoc_collection_find_with_opts(theCollection.collection, this->query, NULL, NULL);
  if (this->cursor == NULL)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Bad mongoDB cursor. ";
    logWorker << logger::red << "Mongo error: bad mongoDB cursor. " << logger::endL;
    return false;
  }
  this->totalItems = 0;
  const bson_t* bufferOutput;
  List<std::string> outputString;
  while (mongoc_cursor_next(this->cursor, &bufferOutput))
  { char* bufferOutpurStringFormat = 0;
    bufferOutpurStringFormat = bson_as_canonical_extended_json(bufferOutput, NULL);
    std::string current(bufferOutpurStringFormat);
    bson_free(bufferOutpurStringFormat);
    if (this->maxOutputItems <= 0 || this->totalItems < this->maxOutputItems)
      outputString.AddOnTop(current);
    this->totalItems ++;
    if (this->flagFindOneOnly)
      break;
  }

  output.SetSize(outputString.size);
  for (int i = 0; i < outputString.size; i ++)
    if (!output[i].readstring(outputString[i], true, commentsOnFailure))
    { logWorker << logger::red << "Mongo/JSData error: failed to read string" << logger::endL;
      return false;
    }
  logWorker << logger::green << "DEBUG: Query successful. Output size: " << output.size << ". "
  << output.ToStringCommaDelimited() << logger::endL;
  return true;
}
#endif

void DatabaseRoutinesGlobalFunctionsMongo::CreateHashIndex
(const std::string& collectionName, const std::string& theKey)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::CreateHashIndex");
  std::cout << "DEBUG Got to here" << std::endl;
  MongoQuery query;
  std::stringstream theCommand;
  theCommand << "{\"createIndexes\":\"" << collectionName << "\", \"indexes\": [{\"key\":{\""
  << theKey << "\": \"hashed\"}, \"name\": \"" << collectionName  << "_" << theKey << "_hash\"" << "}]} ";
  std::cout << "pt 2 Got to here" << std::endl;
  query.command = bson_new_from_json((const uint8_t*) theCommand.str().c_str(), theCommand.str().size(), &query.theError);
  std::cout << "pt 3Got to here. Error: " << query.theError.message << std::endl;
  mongoc_database_write_command_with_opts(database, query.command, NULL, query.updateResult, &query.theError);
  std::cout << "and the error: " << query.theError.message << std::endl;
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindFromString
(const std::string& collectionName, const std::string& findQuery,
 List<JSData>& output, int maxOutputItems,
 long long* totalItems, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindFromString");
  JSData theData;
  //logWorker << logger::blue << "Query input: " << findQuery << logger::endL;
  if (!theData.readstring(findQuery, true, commentsOnFailure))
    return false;
  return DatabaseRoutinesGlobalFunctionsMongo::FindFromJSON
  (collectionName, theData, output, maxOutputItems, totalItems, commentsOnFailure);
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindFromJSON
(const std::string& collectionName, const JSData& findQuery,
 List<JSData>& output, int maxOutputItems,
 long long* totalItems, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindFromJSON");
#ifdef MACRO_use_MongoDB
  //logWorker << logger::blue << "Query input JSON: " << findQuery.ToString(true) << logger::endL;
  //stOutput << "Find query: " << theData.ToString();
  MongoQuery query;
  query.collectionName = collectionName;
  query.findQuery = findQuery.ToString(true);
  query.maxOutputItems = maxOutputItems;
  bool result = query.FindMultiple(output, commentsOnFailure);
  if (totalItems != 0)
    *totalItems = query.totalItems;
  return result;
#else
  (void) collectionName;
  (void) findQuery;
  (void) output;
  (void) maxOutputItems;
  (void) totalItems;
  if (commentsOnFailure != 0)
    *commentsOnFailure << "Project compiled without mongoDB support. ";
  return false;
#endif
}

bool DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoQuery
(const JSData& findQuery, std::stringstream* commentsOnFailure, bool mustBeObject)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoQuery");
  if (mustBeObject)
  { if (findQuery.type != findQuery.JSObject)
    { if (commentsOnFailure != 0)
        *commentsOnFailure << "JSData: "
        << HtmlRoutines::ConvertStringToHtmlString(findQuery.ToString(false), false)
        << " expected to be an object. ";
      return false;
    }
  } else
  { if (findQuery.type != findQuery.JSstring)
    { if (commentsOnFailure != 0)
        *commentsOnFailure << "JSData: "
        << HtmlRoutines::ConvertStringToHtmlString(findQuery.ToString(false), false)
        << " expected to be a string. ";
      return false;
    }
  }
  for (int i = 0; i < findQuery.objects.size(); i++)
  { if (! DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoQuery(findQuery.objects.theValues[i], commentsOnFailure, false))
      return false;
  }
  return true;
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSON
(const std::string& collectionName, const JSData& findQuery,
 JSData& output, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSON");
#ifdef MACRO_use_MongoDB
  if (!DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoQuery(findQuery, commentsOnFailure, true))
    return false;
  MongoQuery query;

  query.collectionName = collectionName;
  query.findQuery = findQuery.ToString(true);
  logWorker << logger::blue << "DEBUG: Query input: " << query.findQuery << logger::endL;
  query.maxOutputItems = 1;
  List<JSData> outputList;
  query.FindMultiple(outputList, commentsOnFailure);
  if (outputList.size == 0)
    return false;
  output = outputList[0];
  return true;
#else
  (void) collectionName;
  (void) findQuery;
  (void) output;
  if (commentsOnFailure != 0)
    *commentsOnFailure << "Project compiled without mongoDB support. ";
  return false;
#endif
}

bool DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON
(const std::string& collectionName, const JSData& findQuery, const JSData& updateQuery,
 std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON");
#ifdef MACRO_use_MongoDB
  MongoQuery query;
  query.collectionName = collectionName;
  query.findQuery = findQuery.ToString(true);
  std::stringstream updateQueryStream;
  updateQueryStream << "{\"$set\": " << updateQuery.ToString(true) << "}";
  query.updateQuery = updateQueryStream.str();
  logWorker << logger::blue << "DEBUG: the find query: " << query.findQuery << logger::endL;
  logWorker << logger::blue << "DEBUG: the update query: " << query.updateQuery << logger::endL;
  return query.UpdateOne(commentsOnFailure);
#else
  (void) collectionName;
  (void) findQuery;
  (void) updateQuery;
  if (commentsOnFailure != 0)
    *commentsOnFailure << "Project compiled without mongoDB support. ";
  return false;
#endif
}

bool DatabaseRoutinesGlobalFunctionsMongo::LoadUserInfo(UserCalculatorData& output)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::LoadUserInfo");
  JSData theQuery, userEntry;
  theQuery[DatabaseStrings::labelUsername] = output.username;
  if (!DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSON
      (DatabaseStrings::tableUsers, theQuery, userEntry))
    return false;
  return output.LoadFromJSON(userEntry);
}

bool DatabaseRoutinesGlobalFunctionsMongo::FetchCollectionNames
(List<std::string>& output, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FetchCollectionNames");
  bson_t opts = BSON_INITIALIZER;
  bson_error_t error;
  mongoc_database_get_collection_names_with_opts(database, &opts, &error);
  char **theCollectionChars = mongoc_database_get_collection_names_with_opts(database, &opts, &error);
  bool result = true;
  output.SetSize(0);
  if (theCollectionChars != NULL)
  { for (int i = 0; theCollectionChars[i]; i ++)
    { std::string currentCollection(theCollectionChars[i]);
      output.AddOnTop(currentCollection);
    }
    bson_strfreev (theCollectionChars);
    theCollectionChars = 0;
  } else
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Failed to fetch all collections";
    result = false;
  }
  bson_destroy(&opts);
  return result;
}

bool DatabaseRoutinesGlobalFunctionsMongo::FetchTable
(const std::string& tableName, List<std::string>& outputLabels, List<List<std::string> >& outputRows, long long* totalItems,
 std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FetchTable");
  JSData findQuery;
  findQuery.type = findQuery.JSObject;
  List<JSData> rowsJSON;
  DatabaseRoutinesGlobalFunctionsMongo::FindFromJSON(tableName, findQuery, rowsJSON, 200, totalItems, commentsOnFailure);
  HashedList<std::string, MathRoutines::hashString> theLabels;
  for (int i = 0; i < rowsJSON.size; i ++)
    for (int j = 0; j < rowsJSON[i].objects.size(); j++)
      theLabels.AddOnTopNoRepetition(rowsJSON[i].objects.theKeys[j]);
  outputLabels = theLabels;
  outputRows.SetSize(rowsJSON.size);
  for (int i = 0; i < rowsJSON.size; i ++)
  { outputRows[i].SetSize(theLabels.size);
    for (int j = 0; j < theLabels.size; j ++)
      if (rowsJSON[i].objects.Contains(theLabels[j]))
        outputRows[i][j] = rowsJSON[i].GetValue(theLabels[j]).ToString(false, false);
      else
        outputRows[i][j] = "";
  }
  return true;
}

std::string DatabaseRoutinesGlobalFunctionsMongo::ToHtmlDatabaseCollection(const std::string& currentTable)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::ToStringDatabaseCollection");
  std::stringstream out;
  if (currentTable == "")
  { List<std::string> theCollectionNames;
    if (DatabaseRoutinesGlobalFunctionsMongo::FetchCollectionNames(theCollectionNames, &out))
    { out << "There are " << theCollectionNames.size << " collections. ";
      for (int i = 0; i < theCollectionNames.size; i ++)
      { out << "<br>";
        out << "<a href=\"" << theGlobalVariables.DisplayNameExecutable
        << "?request=database&currentDatabaseTable="
        << theCollectionNames[i] << "\">" << theCollectionNames[i] << "</a>";
      }
    }
    return out.str();
  }
  out << "Current table: " << currentTable << "<br>";
  List<std::string> theLabels;
  List<List<std::string> > theRows;
  long long totalItems = - 1;
  if (!DatabaseRoutinesGlobalFunctionsMongo::FetchTable(currentTable, theLabels, theRows, &totalItems, &out))
    return out.str();
  out << "Total: " << totalItems << ". ";
  if (totalItems > theRows.size)
    out << "Only the first " << theRows.size << " are displayed. ";
  out << "<br>" << HtmlRoutines::ToHtmlTable(theLabels, theRows, true);
  return out.str();

}

