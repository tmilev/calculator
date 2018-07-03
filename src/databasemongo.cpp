#ifdef MACRO_use_MongoDB
#include <mongoc.h>
#include <bcon.h>
mongoc_client_t* databaseClient = 0;
mongoc_database_t *database = 0;
#endif //MACRO_use_MongoDB
#include "vpfHeader7DatabaseInterface_Mongodb.h"
#include "vpfJson.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"
#include "vpfHeader8HtmlSnippets.h"
ProjectInformationInstance ProjectInfoVpfDatabaseMongo(__FILE__, "Database mongoDB.");

int DatabaseRoutinesGlobalFunctionsMongo::numDatabaseInstancesMustBeOneOrZero = 0;
DatabaseRoutinesGlobalFunctionsMongo databaseMongo;

extern logger logWorker;

DatabaseRoutinesGlobalFunctionsMongo::DatabaseRoutinesGlobalFunctionsMongo()
{ this->flagInitialized = false;

}

bool DatabaseRoutinesGlobalFunctionsMongo::initialize(std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::DatabaseRoutinesGlobalFunctionsMongo");
#ifdef MACRO_use_MongoDB
  if (this->flagInitialized)
    return true;
  logWorker << logger::blue << "Initializing database. " << logger::endL;
  if (!theGlobalVariables.flagServerForkedIntoWorker)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "MongoDB not allowed to run before server fork. ";
    return false;
  }
  this->flagInitialized = true;
  mongoc_init();
  databaseClient = mongoc_client_new("mongodb://localhost:27017");
  database = mongoc_client_get_database(databaseClient, DatabaseStrings::theDatabaseNameMongo.c_str());
  DatabaseRoutinesGlobalFunctionsMongo::CreateHashIndex(DatabaseStrings::tableUsers, DatabaseStrings::labelUsername);
  DatabaseRoutinesGlobalFunctionsMongo::CreateHashIndex(DatabaseStrings::tableUsers, DatabaseStrings::labelEmail);
  DatabaseRoutinesGlobalFunctionsMongo::CreateHashIndex(DatabaseStrings::tableUsers, DatabaseStrings::labelInstructor);
  DatabaseRoutinesGlobalFunctionsMongo::CreateHashIndex(DatabaseStrings::tableUsers, DatabaseStrings::labelUserRole);
  DatabaseRoutinesGlobalFunctionsMongo::CreateHashIndex(DatabaseStrings::tableProblemInformation, DatabaseStrings::labelProblemName);
  DatabaseRoutinesGlobalFunctionsMongo::CreateHashIndex(DatabaseStrings::tableDeleted, DatabaseStrings::labelUsername);
  return true;
#else
  if (commentsOnFailure != 0)
    *commentsOnFailure << "Calculator compiled without mongoDB support. ";
  return false;
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
{ if (!databaseMongo.initialize(0))
  { crash << "Mongo DB not initialized when it should be" << crash;
    return;
  }
  this->name = collectionName;
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
  bool FindMultiple(List<JSData>& output, const JSData& inputOptions, std::stringstream* commentsOnFailure);
  bool UpdateOneWithOptions(std::stringstream* commentsOnFailure);
  bool RemoveOne(std::stringstream* commentsOnFailure);
  bool UpdateOne(std::stringstream* commentsOnFailure, bool doUpsert);
  bool InsertOne(const JSData& incoming, std::stringstream* commentsOnFailure);
  bool UpdateOneNoOptions(std::stringstream* commentsOnFailure);
};

MongoQuery::MongoQuery()
{ if (!databaseMongo.initialize(0))
    crash << "Mongo DB did not start correctly. " << crash;
  this->query = 0;
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

bool MongoQuery::RemoveOne(std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("MongoQuery::UpdateOne");
  if (!databaseMongo.initialize(commentsOnFailure))
    return false;
  MongoCollection theCollection(this->collectionName);
  //logWorker << "DEBUG: Update: " << this->findQuery << " to: "
  //<< this->updateQuery << " inside: " << this->collectionName << logger::endL;
  if (this->query != 0)
    crash << "At this point of code, query is supposed to be 0. " << crash;
  this->query = bson_new_from_json
  ((const uint8_t*) this->findQuery.c_str(), this->findQuery.size(), &this->theError);
  bool result = mongoc_collection_remove(theCollection.collection, MONGOC_REMOVE_SINGLE_REMOVE, this->query, NULL, &this->theError);
  if (!result)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << this->theError.message;
    logWorker << logger::red << "While removing: "
    << this->findQuery << " inside: "
    << this->collectionName << " got mongo error: "
    << this->theError.message << logger::endL;
    return false;
  }
  //char* bufferOutpurStringFormat = 0;
  //bufferOutpurStringFormat = bson_as_canonical_extended_json(this->updateResult, NULL);
  //std::string updateResultString(bufferOutpurStringFormat);
  //bson_free(bufferOutpurStringFormat);
  //logWorker << logger::red << "Update result: " << updateResultString << logger::endL;
  return true;
}

bool MongoQuery::InsertOne(const JSData& incoming, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("MongoQuery::InsertOne");
  if (!databaseMongo.initialize(commentsOnFailure))
    return false;
  MongoCollection theCollection(this->collectionName);
  //logWorker << "DEBUG: Update: " << this->findQuery << " to: "
  //<< this->updateQuery << " inside: " << this->collectionName << logger::endL;
  if (this->query != 0)
    crash << "At this point of code, query is supposed to be 0. " << crash;
  std::string incomingJSONString = incoming.ToString(true, false);
  this->update = bson_new_from_json
  ((const uint8_t*) incomingJSONString.c_str(), incomingJSONString.size(), &this->theError);
  if (this->update == NULL)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Failed to create insertion bson.";
    return false;
  }
  this->updateResult = bson_new();
  bool result = mongoc_collection_insert_one(theCollection.collection, this->update, NULL, this->updateResult, &this->theError);
  if (!result)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << this->theError.message;
    logWorker << logger::red << "While updating: "
    << this->findQuery << " to: " << this->updateQuery << " inside: "
    << this->collectionName << " got mongo error: "
    << this->theError.message << logger::endL;
    return false;
  }
  //char* bufferOutpurStringFormat = 0;
  //bufferOutpurStringFormat = bson_as_canonical_extended_json(this->updateResult, NULL);
  //std::string updateResultString(bufferOutpurStringFormat);
  //bson_free(bufferOutpurStringFormat);
  //logWorker << logger::red << "Update result: " << updateResultString << logger::endL;
  return true;

}

bool MongoQuery::UpdateOne(std::stringstream* commentsOnFailure, bool doUpsert)
{ MacroRegisterFunctionWithName("MongoQuery::UpdateOne");
  if (!databaseMongo.initialize(commentsOnFailure))
    return false;
  MongoCollection theCollection(this->collectionName);
  //logWorker << "DEBUG: Update: " << this->findQuery << " to: "
  //<< this->updateQuery << " inside: " << this->collectionName << logger::endL;
  if (this->query != 0)
    crash << "At this point of code, query is supposed to be 0. " << crash;
  this->query = bson_new_from_json
  ((const uint8_t*) this->findQuery.c_str(), this->findQuery.size(), &this->theError);
  this->update = bson_new_from_json
  ((const uint8_t*) this->updateQuery.c_str(), this->updateQuery.size(), &this->theError);
  if (doUpsert)
  { this->optionsQuery = "{\"upsert\": true}";
    this->options = bson_new_from_json
    ((const uint8_t*) this->optionsQuery.c_str(), this->optionsQuery.size(), &this->theError);
  }
  this->updateResult = bson_new();
  bool result = false;
  if (doUpsert)
    result = mongoc_collection_update_one
    (theCollection.collection, this->query, this->update, this->options, this->updateResult, &this->theError);
  else
    result = mongoc_collection_update_one
    (theCollection.collection, this->query, this->update, 0, this->updateResult, &this->theError);
  if (!result)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << this->theError.message;
    logWorker << logger::red << "While updating: "
    << this->findQuery << " to: " << this->updateQuery << " inside: "
    << this->collectionName << " got mongo error: "
    << this->theError.message << logger::endL;
    return false;
  }
  //char* bufferOutpurStringFormat = 0;
  //bufferOutpurStringFormat = bson_as_canonical_extended_json(this->updateResult, NULL);
  //std::string updateResultString(bufferOutpurStringFormat);
  //bson_free(bufferOutpurStringFormat);
  //logWorker << logger::red << "Update result: " << updateResultString << logger::endL;
  return true;
}

bool MongoQuery::UpdateOneNoOptions(std::stringstream* commentsOnFailure)
{ return this->UpdateOne(commentsOnFailure, false);
}

bool MongoQuery::UpdateOneWithOptions(std::stringstream* commentsOnFailure)
{ return this->UpdateOne(commentsOnFailure, true);
}

bool MongoQuery::FindMultiple(List<JSData>& output, const JSData& inputOptions, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("MongoQuery::FindMultiple");
  if (!databaseMongo.initialize(commentsOnFailure))
    return false;
  MongoCollection theCollection(this->collectionName);
  //logWorker << "Query: " << this->findQuery << " inside: " << this->collectionName << logger::endL;
  if (this->query != 0)
    crash << "At this point of code, query is supposed to be 0. " << crash;
  this->query = bson_new_from_json((const uint8_t*) this->findQuery.c_str(), this->findQuery.size(), &this->theError);
  if (this->query == NULL)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << this->theError.message;
    logWorker << logger::red << "While finding: "
    << this->findQuery << " inside: "
    << this->collectionName << " got mongo error: "
    << this->theError.message << logger::endL;
    return false;
  }
  if (this->options != 0)
  { bson_destroy(this->options);
    this->options = 0;
  }
  if (inputOptions.type != JSData::JSUndefined)
  { std::string optionsString = inputOptions.ToString(false);
    this->options = bson_new_from_json((const uint8_t*) optionsString.c_str(), optionsString.size(), &this->theError);
    if (this->options == NULL)
    { if (commentsOnFailure != 0)
        *commentsOnFailure << this->theError.message;
      logWorker << logger::red << "Mongo error: " << this->theError.message << logger::endL;
      return false;
    }
  }
  this->cursor = mongoc_collection_find_with_opts(theCollection.collection, this->query, this->options, NULL);
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
  //logWorker << logger::green << "DEBUG: Query successful. Output size: " << output.size << ". "
  //<< output.ToStringCommaDelimited() << logger::endL;
  return true;
}

void DatabaseRoutinesGlobalFunctionsMongo::CreateHashIndex
(const std::string& collectionName, const std::string& theKey)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::CreateHashIndex");
  MongoQuery query;
  std::stringstream theCommand;
  theCommand << "{\"createIndexes\":\"" << collectionName << "\", \"indexes\": [{\"key\":{\""
  << theKey << "\": \"hashed\"}, \"name\": \"" << collectionName  << "_" << theKey << "_hash\"" << "}]} ";
  query.command = bson_new_from_json((const uint8_t*) theCommand.str().c_str(), theCommand.str().size(), &query.theError);
  mongoc_database_write_command_with_opts(database, query.command, NULL, query.updateResult, &query.theError);
}

#endif
bool DatabaseRoutinesGlobalFunctionsMongo::FindFromString
(const std::string& collectionName, const std::string& findQuery,
 List<JSData>& output, int maxOutputItems,
 long long* totalItems, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindFromString");
#ifdef MACRO_use_MongoDB
  JSData theData;
  //logWorker << logger::blue << "Query input: " << findQuery << logger::endL;
  if (!theData.readstring(findQuery, true, commentsOnFailure))
    return false;

  return DatabaseRoutinesGlobalFunctionsMongo::FindFromJSON
  (collectionName, theData, output, maxOutputItems, totalItems, commentsOnFailure);
#else
  if (commentsOnFailure != 0)
    *commentsOnFailure << "Error: code compiled without DB support";
  (void) collectionName;
  (void) findQuery;
  (void) output;
  (void) maxOutputItems;
  (void) totalItems;
  return false;
#endif

}

JSData DatabaseRoutinesGlobalFunctionsMongo::GetProjectionFromFieldNames(const List<std::string>& fieldsToProjectTo)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::GetProjectionFromFieldNames");
  JSData result;
  JSData fields;
  for (int i = 0; i < fieldsToProjectTo.size; i ++)
    fields[fieldsToProjectTo[i]] = 1;
  result["projection"] = fields;
  return result;
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindFromJSONWithProjection
(const std::string& collectionName, const JSData& findQuery,
 List<JSData>& output, List<std::string>& fieldsToProjectTo, int maxOutputItems,
 long long* totalItems, std::stringstream* commentsOnFailure)
{ return DatabaseRoutinesGlobalFunctionsMongo::FindFromJSONWithOptions
  (collectionName, findQuery, output,
   DatabaseRoutinesGlobalFunctionsMongo::GetProjectionFromFieldNames(fieldsToProjectTo),
   maxOutputItems, totalItems, commentsOnFailure);
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindFromJSON
(const std::string& collectionName, const JSData& findQuery,
 List<JSData>& output, int maxOutputItems,
 long long* totalItems, std::stringstream* commentsOnFailure)
{ const JSData options(JSData::JSUndefined);
  return DatabaseRoutinesGlobalFunctionsMongo::FindFromJSONWithOptions
  (collectionName, findQuery, output, options, maxOutputItems, totalItems, commentsOnFailure);
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindFromJSONWithOptions
(const std::string& collectionName, const JSData& findQuery,
 List<JSData>& output, const JSData& options, int maxOutputItems,
 long long* totalItems, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindFromJSON");
#ifdef MACRO_use_MongoDB
  //logWorker << logger::blue << "Query input JSON: " << findQuery.ToString(true) << logger::endL;
  //stOutput << "Find query: " << theData.ToString();
  MongoQuery query;
  query.collectionName = collectionName;
  query.findQuery = findQuery.ToString(true);
  query.maxOutputItems = maxOutputItems;
  bool result = query.FindMultiple(output, options, commentsOnFailure);
  if (totalItems != 0)
    *totalItems = query.totalItems;
  return result;
#else
  (void) options;
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
  //logWorker << logger::red << "DEBUG: findQuery in isvalid json: " << findQuery.ToString(false) << logger::endL;
  if (mustBeObject)
  { if (findQuery.type != findQuery.JSObject)
    { if (commentsOnFailure != 0)
        *commentsOnFailure << "JSData: "
        << HtmlRoutines::ConvertStringToHtmlString(findQuery.ToString(false), false)
        << " expected to be an object. ";
      return false;
    }
  } else
  { if (findQuery.type == findQuery.JSarray)
      if (findQuery.list.size == 0)
        return true;
    if (findQuery.type != findQuery.JSstring && findQuery.type != findQuery.JSObject)
    { if (commentsOnFailure != 0)
        *commentsOnFailure << "JSData: "
        << HtmlRoutines::ConvertStringToHtmlString(findQuery.ToString(false), false)
        << " expected to be a string or an object. ";
      return false;
    }
  }
  for (int i = 0; i < findQuery.objects.size(); i ++)
    if (!DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoQuery(findQuery.objects.theValues[i], commentsOnFailure, false))
      return false;
  return true;
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindOneFromSome
(const std::string& collectionName, const List<JSData>& findOrQueries,
 JSData& output, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindOneFromSome");
  std::string theQuery;
  if (!DatabaseRoutinesGlobalFunctionsMongo::GetOrFindQuery(findOrQueries, theQuery, commentsOnFailure))
    return false;
  return DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryString(collectionName, theQuery, output, commentsOnFailure);
}

bool DatabaseRoutinesGlobalFunctionsMongo::GetOrFindQuery
(const List<JSData>& input, std::string& output, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::GetOrFindQuery");
  for (int i = 0; i < input.size; i ++)
    if (!DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoQuery(input[i], commentsOnFailure, true))
      return false;
  std::stringstream queryStream;
  queryStream << "{\"$or\": [";
  for (int i = 0; i < input.size; i ++)
  { queryStream << input[i].ToString(true);
    if (i < input.size - 1)
      queryStream << ", ";
  }
  queryStream << "]}";
  output = queryStream.str();
  return true;
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryString
(const std::string& collectionName, const std::string& findQuery, JSData& output, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryString");
  JSData options(JSData::JSUndefined);
  return DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryStringWithOptions
  (collectionName, findQuery, options, output, commentsOnFailure);
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryStringWithProjection
(const std::string& collectionName, const std::string& findQuery, const List<std::string>& fieldsToProjectTo,
 JSData& output, std::stringstream* commentsOnFailure)
{ return DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryStringWithOptions
  (collectionName, findQuery, DatabaseRoutinesGlobalFunctionsMongo::GetProjectionFromFieldNames(fieldsToProjectTo),
   output, commentsOnFailure);
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSONWithProjection
(const std::string& collectionName, const JSData& findQuery, const List<std::string>& fieldsToProjectTo,
 JSData& output, std::stringstream* commentsOnFailure, bool doEncodeFindFields)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSONWithOptions");
  JSData theProjection = DatabaseRoutinesGlobalFunctionsMongo::GetProjectionFromFieldNames(fieldsToProjectTo);
  std::string findQueryString = findQuery.ToString(doEncodeFindFields);

 return DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryStringWithOptions
 (collectionName, findQueryString, theProjection, output, commentsOnFailure);
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryStringWithOptions
(const std::string& collectionName, const std::string& findQuery, const JSData& options,
 JSData& output, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryStringWithOptions");
#ifdef MACRO_use_MongoDB
  MongoQuery query;
  query.collectionName = collectionName;
  query.findQuery = findQuery;
  //logWorker << logger::blue << "DEBUG: Query input: " << query.findQuery << logger::endL;
  query.maxOutputItems = 1;
  List<JSData> outputList;
  query.FindMultiple(outputList, options, commentsOnFailure);
  if (outputList.size == 0)
    return false;
  output = outputList[0];
  return true;
#else
  (void) collectionName;
  (void) findQuery;
  (void) output;
  (void) options;
  if (commentsOnFailure != 0)
    *commentsOnFailure << "Project compiled without mongoDB support. ";
  return false;
#endif
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSON
(const std::string& collectionName, const JSData& findQuery,
 JSData& output, std::stringstream* commentsOnFailure, bool doEncodeFindFields)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSON");
  if (!DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoQuery(findQuery, commentsOnFailure, true))
    return false;
  //logWorker << logger::red << "DEbug: find one from json with: " << findQuery.ToString(doEncodeFindFields) << logger::endL;
  return DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryString(collectionName, findQuery.ToString(doEncodeFindFields), output, commentsOnFailure);
}

bool DatabaseRoutinesGlobalFunctionsMongo::matchesPattern
(const std::string& tableName, const List<std::string>& fieldLabel, const List<std::string>& pattern)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::matchesPattern");
  if (fieldLabel.size != pattern.size - 1)
    return false;
  if (tableName != pattern[0])
    return false;
  for (int i = 1; i < pattern.size; i ++)
  { if (pattern[i] == DatabaseStrings::anyField)
      continue;
    if (pattern[i] == DatabaseStrings::objectSelector)
      continue;
    if (fieldLabel[i - 1] != pattern[i])
      return false;
  }
  return true;
}

bool DatabaseRoutinesGlobalFunctionsMongo::isDeleteable(const std::string& tableName, const List<std::string>& theLabels, List<std::string>** outputPattern, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::isDeleteable");
  for (int i = 0; i < theGlobalVariables.databaseModifiableFields.size; i ++)
    if (DatabaseRoutinesGlobalFunctionsMongo::matchesPattern(tableName, theLabels, theGlobalVariables.databaseModifiableFields[i]))
    { if (outputPattern != 0)
        *outputPattern = &theGlobalVariables.databaseModifiableFields[i];
      return true;
    }
  if (commentsOnFailure != 0)
    *commentsOnFailure << "Labels: " << theLabels.ToStringCommaDelimited() << " do not match any modifiable field pattern. ";
  return false;
}

bool DatabaseRoutinesGlobalFunctionsMongo::getLabels(const JSData& fieldEntries, List<std::string>& output, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::getLabels");
  output.SetSize(0);
  for (int i = 0; i < fieldEntries.list.size; i ++)
  { if (fieldEntries.list[i].type != JSData::JSstring)
    { if (commentsOnFailure != 0)
        *commentsOnFailure << "Label index " << i << " is not of type string as required. ";
      return false;
    }
    output.AddOnTop(fieldEntries.list[i].string);
  }
  return true;
}

bool DatabaseRoutinesGlobalFunctionsMongo::isDeleteable(const JSData& theEntry, List<std::string>** outputPattern, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::isDeleteable");
  if (theEntry.type != JSData::JSObject                 ||
      !theEntry.HasKey(DatabaseStrings::labelTable)     ||
      !theEntry.HasKey(DatabaseStrings::objectSelector) ||
      !theEntry.HasKey(DatabaseStrings::labelFields))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "The labels json is required to be an object of the form {table: ..., object: ..., fields: [...]}. ";
    return false;
  }
  List<std::string> theLabels;
  if (!DatabaseRoutinesGlobalFunctionsMongo::getLabels
       (theEntry.objects.GetValueConstCrashIfNotPresent("fields"), theLabels, commentsOnFailure))
    return false;
  return DatabaseRoutinesGlobalFunctionsMongo::isDeleteable
  (theEntry.objects.GetValueConstCrashIfNotPresent("table").string, theLabels, outputPattern, commentsOnFailure);
}

bool DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntry(const JSData& theEntry, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntry");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Only logged-in admins can delete DB entries.";
    return false;
  }
  List<std::string>* labelTypes = 0;
  if (!isDeleteable(theEntry, &labelTypes, commentsOnFailure))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Entry: " << theEntry.ToString(false, false) << " not deleteable.";
    return false;
  }
  List<std::string> theLabels;
  if (!DatabaseRoutinesGlobalFunctionsMongo::getLabels
       (theEntry.objects.GetValueConstCrashIfNotPresent(DatabaseStrings::labelFields),
        theLabels, commentsOnFailure))
    return false;
  JSData findQuery;
  findQuery[DatabaseStrings::labelIdMongo][DatabaseStrings::objectSelectorMongo] =
  theEntry.objects.GetValueConstCrashIfNotPresent(DatabaseStrings::objectSelector);
  std::string tableName = theEntry.objects.GetValueConstCrashIfNotPresent(DatabaseStrings::labelTable).string;
  if (theLabels.size == 0)
    return DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntryById
    (tableName, findQuery, commentsOnFailure);
  return  DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntryUnsetUnsecure(tableName, findQuery, theLabels, commentsOnFailure);
}

bool DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntryById
(const std::string& tableName, const JSData& findQuery, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntryById");
#ifdef MACRO_use_MongoDB
  JSData foundItem;
  if (!FindOneFromJSON(tableName, findQuery, foundItem, commentsOnFailure, false))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Query: " << findQuery.ToString(false, false) << " returned no hits in table: "
      << tableName;
    return false;
  }
  if (commentsOnFailure != 0)
    *commentsOnFailure << "DEBUG: found item: " << foundItem.ToString(false);
  MongoQuery queryInsertIntoDeletedTable;
  JSData deletedEntry;
  deletedEntry["deleted"] = foundItem;
  queryInsertIntoDeletedTable.collectionName = DatabaseStrings::tableDeleted;
  if (!queryInsertIntoDeletedTable.InsertOne(deletedEntry, commentsOnFailure))
    return false;
  MongoQuery query;
  query.findQuery = findQuery.ToString(false);
  query.collectionName = tableName;
  logWorker << logger::red << "DEBUG: remove query: " << logger::blue << query.findQuery << logger::endL;
  //logWorker << logger::blue << "DEBUG: the find query: " << query.findQuery << logger::endL;
  //logWorker << logger::blue << "DEBUG: the update query: " << query.updateQuery << logger::endL;
  return query.RemoveOne(commentsOnFailure);
#else
  (void) tableName;
  (void) findQuery;
  if (commentsOnFailure != 0)
    *commentsOnFailure << "Project compiled without mongoDB support. ";
  return false;
#endif

}

bool DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntryUnsetUnsecure
(const std::string& tableName, const JSData& findQuery, List<std::string>& selector,
 std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntryUnsecure");
#ifdef MACRO_use_MongoDB

  std::stringstream selectorStream;
  for (int i = 0; i < selector.size; i ++)
  { selectorStream << HtmlRoutines::ConvertStringToURLStringIncludingDots(selector[i], false);
    if (i != selector.size - 1)
      selectorStream << ".";
  }

  JSData foundItem;
  List<std::string> selectorsCombined;
  selectorsCombined.AddOnTop(selectorStream.str());
  bool didFindItem = FindOneFromJSONWithProjection
  (tableName, findQuery, selectorsCombined, foundItem, commentsOnFailure, false);

  if (!didFindItem)
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Query: " << findQuery.ToString(false, false) << " returned no hits in table: "
      << tableName;
    return false;
  }
  MongoQuery queryBackUp;
  queryBackUp.collectionName = DatabaseStrings::tableDeleted;
  JSData backUp;
  backUp["deleted"] = foundItem;
  if (!queryBackUp.InsertOne(backUp, commentsOnFailure))
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Failed to insert backup: " << backUp.ToString(false);
    return false;
  }
  //stOutput << "DEBUG: found item projected: " << foundItem.ToString(false)
  //<< "backup query: " << backUp.ToString(true);
  MongoQuery query;
  query.findQuery = findQuery.ToString(false);
  query.collectionName = tableName;
  std::stringstream updateQueryStream;
  updateQueryStream << "{\"$unset\": {\"" << selectorStream.str() << "\":\"\"}}";
  query.updateQuery = updateQueryStream.str();
  logWorker << logger::red << "DEBUG: update query: " << logger::blue << query.updateQuery << logger::endL;
  //logWorker << logger::blue << "DEBUG: the find query: " << query.findQuery << logger::endL;
  //logWorker << logger::blue << "DEBUG: the update query: " << query.updateQuery << logger::endL;
  return query.UpdateOneNoOptions(commentsOnFailure);
#else
  (void) tableName;
  (void) findQuery;
  (void) selector;
  if (commentsOnFailure != 0)
    *commentsOnFailure << "Project compiled without mongoDB support. ";
  return false;
#endif
}

bool DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromQueryString
(const std::string& collectionName, const std::string& findQuery, const JSData& updateQuery,
 std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromQueryString");
#ifdef MACRO_use_MongoDB
  if (!DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoQuery(updateQuery, commentsOnFailure, true))
  { return false;
  }
  MongoQuery query;
  query.findQuery = findQuery;
  query.collectionName = collectionName;
  std::stringstream updateQueryStream;
  //logWorker << logger::blue << "DEBUG: GOT to here: " << logger::endL;
  updateQueryStream << "{\"$set\": " << updateQuery.ToString(true) << "}";
  query.updateQuery = updateQueryStream.str();
  //logWorker << logger::blue << "DEBUG: the find query: " << query.findQuery << logger::endL;
  //logWorker << logger::blue << "DEBUG: the update query: " << query.updateQuery << logger::endL;
  return query.UpdateOneWithOptions(commentsOnFailure);
#else
  (void) collectionName;
  (void) findQuery;
  (void) updateQuery;
  if (commentsOnFailure != 0)
    *commentsOnFailure << "Project compiled without mongoDB support. ";
  return false;
#endif
}

bool DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromSomeJSON
(const std::string& collectionName, const List<JSData>& findOrQueries, const JSData& updateQuery,
 std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromSomeJSON");
  logWorker << "DEBUG: HEre I am!" << logger::endL;
  std::string queryString;
  if (!DatabaseRoutinesGlobalFunctionsMongo::GetOrFindQuery(findOrQueries, queryString, commentsOnFailure))
  { logWorker << logger::blue << "DEBUG: GetOrFindQuery, comments: " << commentsOnFailure->str() << logger::endL;
    return false;
  }
  if (!DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoQuery(updateQuery, commentsOnFailure, false))
  { logWorker << logger::green << "Not valid json mongo query, comments: " << commentsOnFailure->str() << logger::endL;
    return false;
  }
  return DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromQueryString(collectionName, queryString, updateQuery, commentsOnFailure);
}

bool DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON
(const std::string& collectionName, const JSData& findQuery, const JSData& updateQuery,
 std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON");
  return DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromQueryString(collectionName, findQuery.ToString(true), updateQuery, commentsOnFailure);
}

bool DatabaseRoutinesGlobalFunctionsMongo::LoadUserInfo(UserCalculatorData& output)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::LoadUserInfo");
  if (output.username == "" && output.email == "")
    return false;
  JSData userEntry;
  if (!DatabaseRoutinesGlobalFunctionsMongo::FindOneFromSome(DatabaseStrings::tableUsers, output.GetFindMeFromUserNameQuery(), userEntry))
    return false;
  return output.LoadFromJSON(userEntry);
}

bool DatabaseRoutinesGlobalFunctionsMongo::FetchCollectionNames
(List<std::string>& output, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FetchCollectionNames");
  if (!databaseMongo.initialize(commentsOnFailure))
    return false;
#ifdef MACRO_use_MongoDB
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
    bson_strfreev(theCollectionChars);
    theCollectionChars = 0;
  } else
  { if (commentsOnFailure != 0)
      *commentsOnFailure << "Failed to fetch all collections";
    result = false;
  }
  bson_destroy(&opts);
  return result;
#else
  (void) output;
  if (commentsOnFailure != 0)
    *commentsOnFailure << "MongoDB not installed";
  return false;
#endif
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
    for (int j = 0; j < rowsJSON[i].objects.size(); j ++)
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

std::string DatabaseRoutinesGlobalFunctionsMongo::ToJSONDatabaseCollection(const std::string& currentTable)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::ToJSONDatabaseCollection");
  JSData result;
  std::stringstream out;
  if (currentTable == "")
  { List<std::string> theCollectionNames;
    if (DatabaseRoutinesGlobalFunctionsMongo::FetchCollectionNames(theCollectionNames, &out))
    { JSData collectionNames;
      collectionNames.type = collectionNames.JSarray;
      collectionNames.list = theCollectionNames;
      result["collections"] = collectionNames;
    } else
      result.string = out.str();
    return result.ToString(false);
  }
  JSData findQuery;
  findQuery.type = findQuery.JSObject;
  List<JSData> rowsJSON;
  long long totalItems = 0;
  if (!DatabaseRoutinesGlobalFunctionsMongo::FindFromJSON(currentTable, findQuery, rowsJSON, 200, &totalItems, &out))
  { result.string = out.str();
    return result.ToString(false);
  }
  if (rowsJSON.size == 0)
    return DatabaseRoutinesGlobalFunctionsMongo::ToJSONDatabaseCollection("");
  JSData theRows;
  theRows.type = theRows.JSarray;
  theRows.list = rowsJSON;
  result["rows"] = theRows;
  result["totalRows"] = (int) totalItems;
  result["currentTable"] = currentTable;
  return result.ToString(false);
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
        << "?request=database&currentDatabaseTable ="
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

