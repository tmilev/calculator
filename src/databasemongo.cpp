#include <mongoc.h>
#include <bcon.h>
#include "vpfheader7databaseinterface_mongodb.h"
#include "vpfJson.h"
ProjectInformationInstance ProjectInfoVpfDatabaseMongo(__FILE__, "Database mongoDB.");

int DatabaseRoutinesGlobalFunctionsMongo::numDatabaseInstancesMustBeOneOrZero = 0;
DatabaseRoutinesGlobalFunctionsMongo databaseMongo;
mongoc_client_t* databaseClient = 0;

extern logger logWorker;

DatabaseRoutinesGlobalFunctionsMongo::DatabaseRoutinesGlobalFunctionsMongo()
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::DatabaseRoutinesGlobalFunctionsMongo");
  mongoc_init();
  databaseClient = mongoc_client_new ("mongodb://localhost:27017");
}

DatabaseRoutinesGlobalFunctionsMongo::~DatabaseRoutinesGlobalFunctionsMongo()
{ mongoc_client_destroy(databaseClient);
  mongoc_cleanup();
}

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
  bool FindMultiple(List<std::string>& output, std::stringstream* commentsOnFailure);
  bool UpdateOne(std::stringstream* commentsOnFailure);
};

MongoQuery::MongoQuery()
{ this->query = 0;
  this->update = 0;
  this->options = 0;
  this->cursor = 0;
  this->updateResult = 0;
  this->maxOutputItems = - 1;
  this->totalItems = - 1;
  this->flagFindOneOnly = false;
}

MongoQuery::~MongoQuery()
{ if (this->cursor != 0)
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

bool MongoQuery::FindMultiple(List<std::string>& output, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("MongoQuery::FindMultiple");
  MongoCollection theCollection(this->collectionName);
  logWorker << "Query: " << this->findQuery << " inside: " << this->collectionName << logger::endL;
  if (this->query != 0)
    crash << "At this point of code, query is supposed to be 0. " << crash;
  this->query = bson_new_from_json((const uint8_t*) this->findQuery.c_str(), this->findQuery.size(), &this->theError);
  if (this->query == NULL)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << this->theError.message;
    return false;
  }
  this->cursor = mongoc_collection_find_with_opts(theCollection.collection, this->query, NULL, NULL);
  if (this->cursor == NULL)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Bad mongoDB cursor. ";
    return false;
  }
  this->totalItems = 0;
  const bson_t* bufferOutput;
  while (mongoc_cursor_next(this->cursor, &bufferOutput))
  { char* bufferOutpurStringFormat = 0;
    bufferOutpurStringFormat = bson_as_canonical_extended_json(bufferOutput, NULL);
    std::string current(bufferOutpurStringFormat);
    bson_free(bufferOutpurStringFormat);
    if (this->maxOutputItems <= 0 || this->totalItems < this->maxOutputItems)
      output.AddOnTop(current);
    this->totalItems ++;
    if (this->flagFindOneOnly)
      break;
  }
  return true;
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindFromString
(const std::string& collectionName, const std::string& findQuery,
 List<std::string>& output, int maxOutputItems,
 long long* totalItems, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindFromString");
  JSData theData;
  logWorker << logger::blue << "Query input: " << findQuery << logger::endL;
  if (!theData.readstring(findQuery, commentsOnFailure))
    return false;
  return DatabaseRoutinesGlobalFunctionsMongo::FindFromJSON
  (collectionName, theData, output, maxOutputItems, totalItems, commentsOnFailure);
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindFromJSON
(const std::string& collectionName, const JSData& findQuery,
 List<std::string>& output, int maxOutputItems,
 long long* totalItems, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindFromJSON");
  logWorker << logger::blue << "Query input JSON: " << findQuery.ToString() << logger::endL;
  //stOutput << "Find query: " << theData.ToString();
  MongoQuery query;
  query.collectionName = collectionName;
  query.findQuery = findQuery.ToString();
  query.maxOutputItems = maxOutputItems;
  bool result = query.FindMultiple(output, commentsOnFailure);
  if (totalItems != 0)
    *totalItems = query.totalItems;
  return result;
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSON
(const std::string& collectionName, const JSData& findQuery,
 std::string& output, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSON");
  logWorker << logger::blue << "Query input: " << findQuery.ToString() << logger::endL;
  MongoQuery query;

  query.collectionName = collectionName;
  query.findQuery = findQuery.ToString();
  query.maxOutputItems = 1;
  List<std::string> outputList;
  query.FindMultiple(outputList, commentsOnFailure);
  if (outputList.size == 0)
    return false;
  output = outputList[0];
  return true;
}

bool DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON
(const std::string& collectionName, const JSData& findQuery, const JSData& updateQuery,
 std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON");
  MongoQuery query;
  query.collectionName = collectionName;
  query.findQuery = findQuery.ToString();
  std::stringstream updateQueryStream;
  updateQueryStream << "{\"$set\": " << updateQuery.ToString() << "}";
  query.updateQuery = updateQueryStream.str();
//  logWorker << logger::blue << "DEBUG: the update query stream: " << query.updateQuery << logger::endL;
  return query.UpdateOne(commentsOnFailure);
}

void DatabaseRoutinesGlobalFunctionsMongo::LoadUserInfo(UserCalculatorData& output)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::LoadUserInfo");
  JSData theQuery;
  theQuery[DatabaseStrings::labelUsername] = output.username.value;
  if (DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSON
      (DatabaseStrings::tableUsers, theQuery, output.actualAuthenticationToken.value))
    return;
  logWorker << logger::green << "About to update one. " << logger::endL;
  DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON
  (DatabaseStrings::tableUsers, theQuery, output.ToJSON(), 0);
}
