#ifdef MACRO_use_MongoDB
#include <mongoc.h>
#include <bcon.h>
mongoc_client_t* databaseClient = nullptr;
mongoc_database_t *database = nullptr;
#endif //MACRO_use_MongoDB
#include "vpfHeader7DatabaseInterface_Mongodb.h"
#include "vpfJson.h"
#include "vpfHeader1General4General_Logging_GlobalVariables.h"
#include "vpfHeader8HtmlSnippets.h"
#include "string_constants.h"
ProjectInformationInstance ProjectInfoVpfDatabaseMongo(__FILE__, "Database mongoDB.");

int DatabaseRoutinesGlobalFunctionsMongo::numDatabaseInstancesMustBeOneOrZero = 0;
DatabaseRoutinesGlobalFunctionsMongo databaseMongo;

extern logger logWorker;

JSData DatabaseRoutinesGlobalFunctionsMongo::GetStandardProjectors() {
  JSData result;
  JSData userProjector;
  userProjector[DatabaseStrings::labelProblemDataJSON] = 0;
  userProjector[DatabaseStrings::labelProblemDatA] = 0;
  result[DatabaseStrings::tableUsers]["projection"] = userProjector;
  return result;
}

DatabaseRoutinesGlobalFunctionsMongo::DatabaseRoutinesGlobalFunctionsMongo() {
  this->flagInitialized = false;
}

bool DatabaseRoutinesGlobalFunctionsMongo::initialize(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::initialize");
#ifdef MACRO_use_MongoDB
  if (this->flagInitialized) {
    return true;
  }
  logWorker << logger::blue << "Initializing database. " << logger::endL;
  if (!theGlobalVariables.flagServerForkedIntoWorker) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "MongoDB not allowed to run before server fork. ";
    }
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
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Calculator compiled without mongoDB support. ";
  }
  return false;
#endif

}

DatabaseRoutinesGlobalFunctionsMongo::~DatabaseRoutinesGlobalFunctionsMongo() {
 
#ifdef MACRO_use_MongoDB
  if (database != nullptr) {
    mongoc_database_destroy(database);
    database = nullptr;
  }
  if (databaseClient != nullptr) {
    mongoc_client_destroy(databaseClient);
    databaseClient = nullptr;
  }
  mongoc_cleanup();
#endif
}

#ifdef MACRO_use_MongoDB
class MongoCollection {
public:
  mongoc_collection_t* collection;
  std::string name;
  MongoCollection(const std::string& collectionName);
  ~MongoCollection();
};

MongoCollection::MongoCollection(const std::string& collectionName) {
  if (!databaseMongo.initialize(0)) {
    crash << "Mongo DB not initialized when it should be" << crash;
    return;
  }
  this->name = collectionName;
  this->collection = mongoc_client_get_collection(
    databaseClient, DatabaseStrings::theDatabaseNameMongo.c_str(), this->name.c_str()
  );
}

MongoCollection::~MongoCollection() {
  mongoc_collection_destroy(this->collection);
  this->collection = nullptr;
}

class MongoQuery {
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
  bool FindMultiple(
    List<JSData>& output,
    const JSData& inputOptions,
    std::stringstream* commentsOnFailure,
    std::stringstream *commentsGeneralNonSensitive
  );
  bool UpdateOneWithOptions(std::stringstream* commentsOnFailure);
  bool RemoveOne(std::stringstream* commentsOnFailure);
  bool UpdateOne(std::stringstream* commentsOnFailure, bool doUpsert);
  bool InsertOne(const JSData& incoming, std::stringstream* commentsOnFailure);
  bool UpdateOneNoOptions(std::stringstream* commentsOnFailure);
};

MongoQuery::MongoQuery() {
  if (!databaseMongo.initialize(0)) {
    crash << "Mongo DB did not start correctly. " << crash;
  }
  this->query = nullptr;
  this->command = nullptr;
  this->update = nullptr;
  this->options = nullptr;
  this->cursor = nullptr;
  this->updateResult = nullptr;
  this->maxOutputItems = - 1;
  this->totalItems = - 1;
  this->flagFindOneOnly = false;
}

MongoQuery::~MongoQuery() {
  if (this->command != nullptr) {
    bson_destroy (this->command);
    this->command = nullptr;
  }
  if (this->cursor != nullptr) {
    mongoc_cursor_destroy(this->cursor);
    this->cursor = nullptr;
  }
  if (this->updateResult != nullptr) {
    bson_destroy(this->updateResult);
    this->updateResult = nullptr;
  }
  if (this->options != nullptr) {
    bson_destroy(this->options);
    this->options = nullptr;
  }
  if (this->update != nullptr) {
    bson_destroy(this->update);
    this->update = nullptr;
  }
  if (this->query != nullptr) {
    bson_destroy(this->query);
    this->query = nullptr;
  }
}

bool MongoQuery::RemoveOne(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("MongoQuery::RemoveOne");
  if (!databaseMongo.initialize(commentsOnFailure)) {
    return false;
  }
  MongoCollection theCollection(this->collectionName);
  if (this->query != nullptr) {
    crash << "At this point of code, query is supposed to be 0. " << crash;
  }
  this->query = bson_new_from_json
  ((const uint8_t*) this->findQuery.c_str(), this->findQuery.size(), &this->theError);
  bool result = mongoc_collection_remove(theCollection.collection, MONGOC_REMOVE_SINGLE_REMOVE, this->query, NULL, &this->theError);
  if (!result) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << this->theError.message;
    }
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

bool MongoQuery::InsertOne(const JSData& incoming, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("MongoQuery::InsertOne");
  if (!databaseMongo.initialize(commentsOnFailure)) {
    return false;
  }
  MongoCollection theCollection(this->collectionName);
  if (this->query != nullptr) {
    crash << "At this point of code, query is supposed to be 0. " << crash;
  }
  std::string incomingJSONString = incoming.ToString(true, false);
  this->update = bson_new_from_json(
    (const uint8_t*) incomingJSONString.c_str(), incomingJSONString.size(), &this->theError
  );
  if (this->update == NULL) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to create insertion bson.";
    }
    return false;
  }
  this->updateResult = bson_new();
  bool result = mongoc_collection_insert_one(theCollection.collection, this->update, NULL, this->updateResult, &this->theError);
  if (!result) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << this->theError.message;
    }
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

bool MongoQuery::UpdateOne(std::stringstream* commentsOnFailure, bool doUpsert) {
  MacroRegisterFunctionWithName("MongoQuery::UpdateOne");
  if (!databaseMongo.initialize(commentsOnFailure)) {
    return false;
  }
  MongoCollection theCollection(this->collectionName);
//  logWorker << "DEBUG: Update: " << this->findQuery << " to: "
//  << this->updateQuery << " inside: " << this->collectionName << logger::endL;
  if (this->query != nullptr) {
    crash << "At this point of code, query is supposed to be 0. " << crash;
  }
  this->query = bson_new_from_json(
    (const uint8_t*) this->findQuery.c_str(), this->findQuery.size(), &this->theError
  );
  this->update = bson_new_from_json(
    (const uint8_t*) this->updateQuery.c_str(), this->updateQuery.size(), &this->theError
  );
  if (doUpsert) {
    this->optionsQuery = "{\"upsert\": true}";
    this->options = bson_new_from_json(
      (const uint8_t*) this->optionsQuery.c_str(), this->optionsQuery.size(), &this->theError
    );
  }
  this->updateResult = bson_new();
  bool result = false;
  if (doUpsert) {
    result = mongoc_collection_update_one(
      theCollection.collection, this->query, this->update, this->options, this->updateResult, &this->theError
    );
  } else {
    result = mongoc_collection_update_one(
      theCollection.collection, this->query, this->update, 0, this->updateResult, &this->theError
    );
  }
  if (!result) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << this->theError.message;
    }
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

bool MongoQuery::UpdateOneNoOptions(std::stringstream* commentsOnFailure) {
  return this->UpdateOne(commentsOnFailure, false);
}

bool MongoQuery::UpdateOneWithOptions(std::stringstream* commentsOnFailure) {
  return this->UpdateOne(commentsOnFailure, true);
}

bool MongoQuery::FindMultiple(
  List<JSData>& output,
  const JSData& inputOptions,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive
) {
  MacroRegisterFunctionWithName("MongoQuery::FindMultiple");
  if (!databaseMongo.initialize(commentsOnFailure)) {
    return false;
  }
  MongoCollection theCollection(this->collectionName);
  if (commentsGeneralNonSensitive != nullptr && theGlobalVariables.UserDefaultHasAdminRights()) {
    *commentsGeneralNonSensitive
    << "Query: " << this->findQuery << ". Options: " << inputOptions.ToString(false, false);
  }
  if (this->query != nullptr) {
    crash << "At this point of code, query is supposed to be 0. " << crash;
  }
  this->query = bson_new_from_json((const uint8_t*) this->findQuery.c_str(), this->findQuery.size(), &this->theError);
  if (this->query == NULL) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << this->theError.message;
    }
    logWorker << logger::red << "While finding: "
    << this->findQuery << " inside: "
    << this->collectionName << " got mongo error: "
    << this->theError.message << logger::endL;
    return false;
  }
  if (this->options != nullptr) {
    bson_destroy(this->options);
    this->options = nullptr;
  }
  if (inputOptions.theType != JSData::token::tokenUndefined) {
    std::string optionsString = inputOptions.ToString(false, false);
    this->options = bson_new_from_json((const uint8_t*) optionsString.c_str(), optionsString.size(), &this->theError);
    if (this->options == NULL) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << this->theError.message;
      }
      logWorker << logger::red << "Mongo error: " << this->theError.message << logger::endL;
      return false;
    }
  }
  this->cursor = mongoc_collection_find_with_opts(theCollection.collection, this->query, this->options, NULL);
  if (this->cursor == NULL) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad mongoDB cursor. ";
    }
    logWorker << logger::red << "Mongo error: bad mongoDB cursor. " << logger::endL;
    return false;
  }
  this->totalItems = 0;
  const bson_t* bufferOutput;
  List<std::string> outputString;
  while (mongoc_cursor_next(this->cursor, &bufferOutput)) {
    char* bufferOutpurStringFormat = nullptr;
    bufferOutpurStringFormat = bson_as_canonical_extended_json(bufferOutput, NULL);
    std::string current(bufferOutpurStringFormat);
    bson_free(bufferOutpurStringFormat);
    if (this->maxOutputItems <= 0 || this->totalItems < this->maxOutputItems) {
      outputString.AddOnTop(current);
    }
    this->totalItems ++;
    if (this->flagFindOneOnly) {
      break;
    }
  }

  output.SetSize(outputString.size);
  for (int i = 0; i < outputString.size; i ++) {
    if (!output[i].readstring(outputString[i], true, commentsOnFailure)) {
      logWorker << logger::red << "Mongo/JSData error: failed to read string" << logger::endL;
      return false;
    }
  }
  //double timeAfterQuery = theGlobalVariables.GetElapsedSeconds();
  //double timeInMsDouble = (timeAfterQuery - timeBeforeQuery ) * 1000;
  //double timeWithDBStart = (timeAfterQuery - timeBeforeDatabase) *1000 ;
  //logWorker << logger::green << "Time in ms: " << timeInMsDouble << logger::endL;
  //logWorker << logger::blue << "Time in ms INDCLUDING first connection: " << timeWithDBStart << logger::endL;
  //logWorker << logger::green << "DEBUG: Query successful. Output size: " << output.size << ". "
  //<< output.ToStringCommaDelimited() << logger::endL;
  return true;
}

void DatabaseRoutinesGlobalFunctionsMongo::CreateHashIndex(
  const std::string& collectionName, const std::string& theKey
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::CreateHashIndex");
  MongoQuery query;
  std::stringstream theCommand;
  theCommand << "{\"createIndexes\":\"" << collectionName << "\", \"indexes\": [{\"key\":{\""
  << theKey << "\": \"hashed\"}, \"name\": \"" << collectionName  << "_" << theKey << "_hash\"" << "}]} ";
  query.command = bson_new_from_json(
    (const uint8_t*) theCommand.str().c_str(), theCommand.str().size(), &query.theError
  );
  mongoc_database_write_command_with_opts(database, query.command, NULL, query.updateResult, &query.theError);
}

#endif
bool DatabaseRoutinesGlobalFunctionsMongo::FindFromString(
  const std::string& collectionName,
  const std::string& findQuery,
  List<JSData>& output,
  int maxOutputItems,
  long long* totalItems,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindFromString");
#ifdef MACRO_use_MongoDB
  JSData theData;
  //logWorker << logger::blue << "Query input: " << findQuery << logger::endL;
  if (!theData.readstring(findQuery, true, commentsOnFailure)) {
    return false;
  }
  return DatabaseRoutinesGlobalFunctionsMongo::FindFromJSON(
    collectionName, theData, output, maxOutputItems, totalItems, commentsOnFailure
  );
#else
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Error: code compiled without DB support";
  }
  (void) collectionName;
  (void) findQuery;
  (void) output;
  (void) maxOutputItems;
  (void) totalItems;
  return false;
#endif

}

JSData DatabaseRoutinesGlobalFunctionsMongo::GetProjectionFromFieldNames(
  const List<std::string>& fieldsToProjectTo, int offset
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::GetProjectionFromFieldNames");
  JSData result;
  JSData fields;
  for (int i = offset; i < fieldsToProjectTo.size; i ++) {
    fields[fieldsToProjectTo[i]] = 1;
  }
  result["projection"] = fields;
  return result;
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindFromJSONWithProjection(
  const std::string& collectionName,
  const JSData& findQuery,
  List<JSData>& output,
  List<std::string>& fieldsToProjectTo,
  int maxOutputItems,
  long long* totalItems,
  std::stringstream* commentsOnFailure
) {
  return DatabaseRoutinesGlobalFunctionsMongo::FindFromJSONWithOptions(
    collectionName,
    findQuery,
    output,
    DatabaseRoutinesGlobalFunctionsMongo::GetProjectionFromFieldNames(fieldsToProjectTo, 0),
    maxOutputItems,
    totalItems,
    commentsOnFailure
  );
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindFromJSON(
  const std::string& collectionName,
  const JSData& findQuery,
  List<JSData>& output,
  int maxOutputItems,
  long long* totalItems,
  std::stringstream* commentsOnFailure
) {
  JSData options;
  options.theType = JSData::token::tokenUndefined;
  return DatabaseRoutinesGlobalFunctionsMongo::FindFromJSONWithOptions(
    collectionName, findQuery, output, options, maxOutputItems, totalItems, commentsOnFailure
  );
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindFromJSONWithOptions(
  const std::string& collectionName,
  const JSData& findQuery,
  List<JSData>& output,
  const JSData& options,
  int maxOutputItems,
  long long* totalItems,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindFromJSONWithOptions");
  (void) commentsGeneralNonSensitive;
#ifdef MACRO_use_MongoDB
  logWorker << logger::blue << "Query input JSON: " << findQuery.ToString(true) << logger::endL;
  MongoQuery query;
  query.collectionName = collectionName;
  query.findQuery = findQuery.ToString(true);
  query.maxOutputItems = maxOutputItems;
  bool result = query.FindMultiple(output, options, commentsOnFailure, commentsGeneralNonSensitive);
  if (totalItems != 0) {
    *totalItems = query.totalItems;
  }
  return result;
#else
  (void) options;
  (void) collectionName;
  (void) findQuery;
  (void) output;
  (void) maxOutputItems;
  (void) totalItems;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Project compiled without mongoDB support. ";
  }
  return false;
#endif
}

bool DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoUpdateQuery(
  const JSData& updateQuery, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoUpdateQuery");
  if (
    updateQuery.theType != JSData::token::tokenString &&
    updateQuery.theType != JSData::token::tokenObject &&
    updateQuery.theType != JSData::token::tokenArray
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "JSData: "
      << HtmlRoutines::ConvertStringToHtmlString(updateQuery.ToString(false), false)
      << " expected to be a string, array or an object. ";
    }
    return false;
  }
  if (updateQuery.theType == JSData::token::tokenObject) {
    for (int i = 0; i < updateQuery.objects.size(); i ++) {
      if (!DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoUpdateQuery(
        updateQuery.objects.theValues[i], commentsOnFailure
      )) {
        return false;
      }
    }
  }
  if (updateQuery.theType == JSData::token::tokenArray) {
    for (int i = 0; i < updateQuery.theList.size; i ++) {
      if (!DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoUpdateQuery(
        updateQuery.theList[i], commentsOnFailure
      )) {
        return false;
      }
    }
  }
  return true;
}

bool DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoFindQuery(
  const JSData& findQuery, std::stringstream* commentsOnFailure, bool mustBeObject
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoFindQuery");
  if (mustBeObject) {
    if (findQuery.theType != JSData::token::tokenObject) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "JSData: "
        << HtmlRoutines::ConvertStringToHtmlString(findQuery.ToString(false), false)
        << " expected to be an object. ";
      }
      return false;
    }
  } else {
    if (findQuery.theType == JSData::token::tokenArray) {
      if (findQuery.theList.size == 0) {
        return true;
      }
    }
    if (findQuery.theType != JSData::token::tokenString && findQuery.theType != JSData::token::tokenObject) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "JSData: "
        << HtmlRoutines::ConvertStringToHtmlString(findQuery.ToString(false), false)
        << " expected to be a string or an object. ";
      }
      return false;
    }
  }
  for (int i = 0; i < findQuery.objects.size(); i ++) {
    if (!DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoFindQuery(
      findQuery.objects.theValues[i], commentsOnFailure, false
    )) {
      return false;
    }
  }
  return true;
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindOneFromSome(
  const std::string& collectionName,
  const List<JSData>& findOrQueries,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindOneFromSome");
  std::string theQuery;
  if (!DatabaseRoutinesGlobalFunctionsMongo::GetOrFindQuery(findOrQueries, theQuery, commentsOnFailure)) {
    return false;
  }
  return DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryString(
    collectionName, theQuery, output, commentsOnFailure
  );
}

bool DatabaseRoutinesGlobalFunctionsMongo::GetOrFindQuery(
  const List<JSData>& input, std::string& output, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::GetOrFindQuery");
  for (int i = 0; i < input.size; i ++) {
    if (!DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoFindQuery(input[i], commentsOnFailure, true)) {
      return false;
    }
  }
  std::stringstream queryStream;
  queryStream << "{\"$or\": [";
  for (int i = 0; i < input.size; i ++) {
    queryStream << input[i].ToString(true);
    if (i < input.size - 1) {
      queryStream << ", ";
    }
  }
  queryStream << "]}";
  output = queryStream.str();
  return true;
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryString(
  const std::string& collectionName, const std::string& findQuery, JSData& output, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryString");
  JSData options;
  options.theType = JSData::token::tokenUndefined;
  return DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryStringWithOptions(
    collectionName, findQuery, options, output, commentsOnFailure
  );
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryStringWithProjection(
  const std::string& collectionName,
  const std::string& findQuery,
  const List<std::string>& fieldsToProjectTo,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  return DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryStringWithOptions(
    collectionName,
    findQuery,
    DatabaseRoutinesGlobalFunctionsMongo::GetProjectionFromFieldNames(fieldsToProjectTo, 0),
    output,
    commentsOnFailure
  );
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSONWithProjection(
  const std::string& collectionName,
  const JSData& findQuery,
  const List<std::string>& fieldsToProjectTo,
  JSData& output,
  std::stringstream* commentsOnFailure,
  bool doEncodeFindFields
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSONWithOptions");
  JSData theProjection = DatabaseRoutinesGlobalFunctionsMongo::GetProjectionFromFieldNames(fieldsToProjectTo, 0);
  std::string findQueryString = findQuery.ToString(doEncodeFindFields);
  return DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryStringWithOptions(
    collectionName, findQueryString, theProjection, output, commentsOnFailure
  );
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryStringWithOptions(
  const std::string& collectionName,
  const std::string& findQuery,
  const JSData& options,
  JSData& output,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryStringWithOptions");
  (void) commentsGeneralNonSensitive;
#ifdef MACRO_use_MongoDB
  MongoQuery query;
  query.collectionName = collectionName;
  query.findQuery = findQuery;
  query.maxOutputItems = 1;
  List<JSData> outputList;
  query.FindMultiple(outputList, options, commentsOnFailure, commentsGeneralNonSensitive);
  if (outputList.size == 0) {
    return false;
  }
  output = outputList[0];
  return true;
#else
  (void) collectionName;
  (void) findQuery;
  (void) output;
  (void) options;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Project compiled without mongoDB support. ";
  }
  return false;
#endif
}

bool DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSON(
  const std::string& collectionName,
  const JSData& findQuery,
  JSData& output,
  std::stringstream* commentsOnFailure,
  bool doEncodeFindFields
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindOneFromJSON");
  if (!DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoFindQuery(findQuery, commentsOnFailure, true)) {
    return false;
  }
  return DatabaseRoutinesGlobalFunctionsMongo::FindOneFromQueryString(
    collectionName, findQuery.ToString(doEncodeFindFields), output, commentsOnFailure
  );
}

bool DatabaseRoutinesGlobalFunctionsMongo::matchesPattern(
  const List<std::string>& fieldLabel, const List<std::string>& pattern
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::matchesPattern");
  if (fieldLabel.size != pattern.size) {
    return false;
  }
  for (int i = 0; i < pattern.size; i ++) {
    if (pattern[i] == DatabaseStrings::anyFielD) {
      continue;
    }
    if (pattern[i] == DatabaseStrings::objectSelectoR) {
      continue;
    }
    if (fieldLabel[i] != pattern[i]) {
      return false;
    }
  }
  return true;
}

bool DatabaseRoutinesGlobalFunctionsMongo::isDeleteable(
  const List<std::string>& theLabels, List<std::string>** outputPattern, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::isDeleteable");
  for (int i = 0; i < theGlobalVariables.databaseModifiableFields.size; i ++) {
    if (DatabaseRoutinesGlobalFunctionsMongo::matchesPattern(
      theLabels, theGlobalVariables.databaseModifiableFields[i]
    )) {
      if (outputPattern != nullptr) {
        *outputPattern = &theGlobalVariables.databaseModifiableFields[i];
      }
      return true;
    }
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Labels: " << theLabels.ToStringCommaDelimited()
    << " do not match any modifiable field pattern. ";
  }
  return false;
}

bool DatabaseRoutinesGlobalFunctionsMongo::getLabels(
  const JSData& fieldEntries, List<std::string>& output, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::getLabels");
  output.SetSize(0);
  for (int i = 0; i < fieldEntries.theList.size; i ++) {
    if (fieldEntries.theList[i].theType != JSData::token::tokenString) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Label index " << i << " is not of type string as required. ";
      }
      return false;
    }
    output.AddOnTop(fieldEntries.theList[i].theString);
  }
  return true;
}

bool DatabaseRoutinesGlobalFunctionsMongo::isDeleteable(
  const JSData& theEntry, List<std::string>** outputPattern, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::isDeleteable");
  if (theEntry.theType != JSData::token::tokenObject || !theEntry.HasKey(DatabaseStrings::labelFields)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "The labels json is required to be an object of the form {fields: [tableName, objectId,...]}. ";
    }
    return false;
  }
  List<std::string> theLabels;
  if (!DatabaseRoutinesGlobalFunctionsMongo::getLabels(
    theEntry.objects.GetValueConstCrashIfNotPresent("fields"), theLabels, commentsOnFailure
  )) {
    return false;
  }
  return DatabaseRoutinesGlobalFunctionsMongo::isDeleteable(theLabels, outputPattern, commentsOnFailure);
}

bool DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntry(const JSData& theEntry, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntry");
  if (!theGlobalVariables.UserDefaultHasAdminRights()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Only logged-in admins can delete DB entries.";
    }
    return false;
  }
  List<std::string>* labelTypes = nullptr;
  if (!isDeleteable(theEntry, &labelTypes, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Entry: " << theEntry.ToString(false, false) << " not deleteable.";
    }
    return false;
  }
  List<std::string> theLabels;
  if (!DatabaseRoutinesGlobalFunctionsMongo::getLabels(
    theEntry.objects.GetValueConstCrashIfNotPresent(DatabaseStrings::labelFields),
    theLabels,
    commentsOnFailure
  )) {
    return false;
  }
  if (theLabels.size < 2) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "When deleting an object, it needs at least two labels: table name and objectid. "
      << "Your input appears to have only " << theLabels.size << " entries: " << theLabels.ToStringCommaDelimited()
      << ". ";
    }
    return false;
  }
  JSData findQuery;
  std::string tableName = theLabels[0];
  findQuery[DatabaseStrings::labelIdMongo][DatabaseStrings::objectSelectorMongo] = theLabels[1];
  if (theLabels.size == 0) {
    return DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntryById(tableName, findQuery, commentsOnFailure);
  }
  return DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntryUnsetUnsecure(
    tableName, findQuery, theLabels, commentsOnFailure
  );
}

bool DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntryById(
  const std::string& tableName, const JSData& findQuery, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntryById");
#ifdef MACRO_use_MongoDB
  JSData foundItem;
  if (!FindOneFromJSON(tableName, findQuery, foundItem, commentsOnFailure, false)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Query: " << findQuery.ToString(false, false) << " returned no hits in table: "
      << tableName;
    }
    return false;
  }
  MongoQuery queryInsertIntoDeletedTable;
  JSData deletedEntry;
  deletedEntry["deleted"] = foundItem;
  queryInsertIntoDeletedTable.collectionName = DatabaseStrings::tableDeleted;
  if (!queryInsertIntoDeletedTable.InsertOne(deletedEntry, commentsOnFailure)) {
    return false;
  }
  MongoQuery query;
  query.findQuery = findQuery.ToString(false);
  query.collectionName = tableName;
  logWorker << logger::red << "DEBUG: remove query: " << logger::blue << query.findQuery << logger::endL;
  return query.RemoveOne(commentsOnFailure);
#else
  (void) tableName;
  (void) findQuery;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Project compiled without mongoDB support. ";
  }
  return false;
#endif

}

bool DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntryUnsetUnsecure(
  const std::string& tableName,
  const JSData& findQuery,
  List<std::string>& selector,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::DeleteOneEntryUnsecure");
#ifdef MACRO_use_MongoDB

  std::stringstream selectorStream;
  for (int i = 0; i < selector.size; i ++) {
    selectorStream << JSData::EncodeKeyForMongo(selector[i]);
    if (i != selector.size - 1) {
      selectorStream << ".";
    }
  }

  JSData foundItem;
  List<std::string> selectorsCombined;
  selectorsCombined.AddOnTop(selectorStream.str());
  bool didFindItem = FindOneFromJSONWithProjection(
    tableName, findQuery, selectorsCombined, foundItem, commentsOnFailure, false
  );

  if (!didFindItem) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Query: " << findQuery.ToString(false, false) << " returned no hits in table: "
      << tableName;
    }
    return false;
  }
  MongoQuery queryBackUp;
  queryBackUp.collectionName = DatabaseStrings::tableDeleted;
  JSData backUp;
  backUp["deleted"] = foundItem;
  if (!queryBackUp.InsertOne(backUp, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to insert backup: " << backUp.ToString(false);
    }
    return false;
  }
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
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Project compiled without mongoDB support. ";
  }
  return false;
#endif
}

bool DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromQueryString(
  const std::string& collectionName,
  const std::string& findQuery,
  const JSData& updateQuery,
  List<std::string>* fieldsToSetIfNullUseFirstFieldIfUpdateQuery,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromQueryString");
  (void) fieldsToSetIfNullUseFirstFieldIfUpdateQuery;
#ifdef MACRO_use_MongoDB
  if (!DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoUpdateQuery(updateQuery, commentsOnFailure)) {
    return false;
  }
  MongoQuery query;
  query.findQuery = findQuery;
  query.collectionName = collectionName;
  std::stringstream updateQueryStream;
  if (fieldsToSetIfNullUseFirstFieldIfUpdateQuery == nullptr) {
    updateQueryStream << "{\"$set\": " << updateQuery.ToString(true) << "}";
  } else {
    updateQueryStream << "{\"$set\":{";
    updateQueryStream << "\"";
    for (int i = 0; i < fieldsToSetIfNullUseFirstFieldIfUpdateQuery->size; i ++) {
      updateQueryStream << JSData::EncodeKeyForMongo((*fieldsToSetIfNullUseFirstFieldIfUpdateQuery)[i]);
      if (i != fieldsToSetIfNullUseFirstFieldIfUpdateQuery->size - 1) {
        updateQueryStream << ".";
      }
    }
    updateQueryStream << "\":";
    updateQueryStream << updateQuery.ToString(true);
    updateQueryStream << "}}";
  }
  query.updateQuery = updateQueryStream.str();
  //logWorker << logger::blue << "DEBUG: the find query: " << query.findQuery << logger::endL;
  //logWorker << logger::blue << "DEBUG: the update query: " << query.updateQuery << logger::endL;
  return query.UpdateOneWithOptions(commentsOnFailure);
#else
  (void) collectionName;
  (void) findQuery;
  (void) updateQuery;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Project compiled without mongoDB support. ";
  }
  return false;
#endif
}

bool DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromSomeJSON(
  const std::string& collectionName,
  const List<JSData>& findOrQueries,
  const JSData& updateQuery,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromSomeJSON");
  std::string queryString;
  if (!DatabaseRoutinesGlobalFunctionsMongo::GetOrFindQuery(findOrQueries, queryString, commentsOnFailure)) {
    return false;
  }
  if (!DatabaseRoutinesGlobalFunctionsMongo::IsValidJSONMongoUpdateQuery(updateQuery, commentsOnFailure)) {
    logWorker << logger::green << "Not valid json mongo query, comments: " << commentsOnFailure->str() << logger::endL;
    return false;
  }
  return DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromQueryString(
    collectionName, queryString, updateQuery, 0, commentsOnFailure
  );
}

bool DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSONSpecifyField(
  const std::string& collectionName,
  const JSData& findQuery,
  const JSData& updateQuery,
  std::string fieldToSet,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSONSpecifyField");
  List<std::string> fields;
  fields.AddOnTop(fieldToSet);
  return DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON(
    collectionName, findQuery, updateQuery, &fields, commentsOnFailure
  );
}

bool DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON(
  const std::string& collectionName,
  const JSData& findQuery,
  const JSData& updateQuery,
  List<std::string>* fieldsToSetIfNullUseFirstFieldIfUpdateQuery,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromJSON");
  return DatabaseRoutinesGlobalFunctionsMongo::UpdateOneFromQueryString(
    collectionName,
    findQuery.ToString(true),
    updateQuery,
    fieldsToSetIfNullUseFirstFieldIfUpdateQuery,
    commentsOnFailure
  );
}

bool DatabaseRoutinesGlobalFunctionsMongo::LoadUserInfo(UserCalculatorData& output) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::LoadUserInfo");
  if (output.username == "" && output.email == "") {
    return false;
  }
  JSData userEntry;
  if (!DatabaseRoutinesGlobalFunctionsMongo::FindOneFromSome(
    DatabaseStrings::tableUsers, output.GetFindMeFromUserNameQuery(), userEntry
  )) {
    return false;
  }
  return output.LoadFromJSON(userEntry);
}

bool DatabaseRoutinesGlobalFunctionsMongo::FetchCollectionNames(
  List<std::string>& output, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FetchCollectionNames");
  if (!databaseMongo.initialize(commentsOnFailure)) {
    return false;
  }
#ifdef MACRO_use_MongoDB
  bson_t opts = BSON_INITIALIZER;
  bson_error_t error;
  mongoc_database_get_collection_names_with_opts(database, &opts, &error);
  char **theCollectionChars = mongoc_database_get_collection_names_with_opts(database, &opts, &error);
  bool result = true;
  output.SetSize(0);
  if (theCollectionChars != NULL) {
    for (int i = 0; theCollectionChars[i]; i ++) {
      std::string currentCollection(theCollectionChars[i]);
      output.AddOnTop(currentCollection);
    }
    bson_strfreev(theCollectionChars);
    theCollectionChars = 0;
  } else {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to fetch all collections";
    }
    result = false;
  }
  bson_destroy(&opts);
  return result;
#else
  (void) output;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "MongoDB not installed";
  }
  return false;
#endif
}

bool DatabaseRoutinesGlobalFunctionsMongo::FetchTable(
  const std::string& tableName,
  List<std::string>& outputLabels,
  List<List<std::string> >& outputRows,
  long long* totalItems,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FetchTable");
  JSData findQuery;
  findQuery.theType = JSData::token::tokenObject;
  List<JSData> rowsJSON;
  DatabaseRoutinesGlobalFunctionsMongo::FindFromJSON(tableName, findQuery, rowsJSON, 200, totalItems, commentsOnFailure);
  HashedList<std::string, MathRoutines::HashString> theLabels;
  for (int i = 0; i < rowsJSON.size; i ++) {
    for (int j = 0; j < rowsJSON[i].objects.size(); j ++) {
      theLabels.AddOnTopNoRepetition(rowsJSON[i].objects.theKeys[j]);
    }
  }
  outputLabels = theLabels;
  outputRows.SetSize(rowsJSON.size);
  for (int i = 0; i < rowsJSON.size; i ++) {
    outputRows[i].SetSize(theLabels.size);
    for (int j = 0; j < theLabels.size; j ++) {
      if (rowsJSON[i].objects.Contains(theLabels[j])) {
        outputRows[i][j] = rowsJSON[i].GetValue(theLabels[j]).ToString(false, false);
      } else {
        outputRows[i][j] = "";
      }
    }
  }
  return true;
}

JSData DatabaseRoutinesGlobalFunctionsMongo::ToJSONDatabaseFetch(const std::string& incomingLabels) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::ToJSONDatabaseFetch");
  JSData result;
  JSData labels;
  std::stringstream commentsOnFailure;
  if (!labels.readstring(incomingLabels, false, &commentsOnFailure)) {
    commentsOnFailure << "Failed to parse labels from: " << MathRoutines::StringTrimToLengthForDisplay(incomingLabels, 100);
    result["error"] = commentsOnFailure.str();
    return result;
  }
  List<std::string> labelStrings;
  if (!labels.isListOfStrings(&labelStrings)) {
    commentsOnFailure << "Labels required to be an array of strings. ";
    result["error"] = commentsOnFailure.str();
    return result;
  }
  if (labelStrings.size == 0) {
    return DatabaseRoutinesGlobalFunctionsMongo::ToJSONDatabaseCollection("");
  }
  if (labelStrings.size == 1) {
    return DatabaseRoutinesGlobalFunctionsMongo::ToJSONDatabaseCollection(labelStrings[0]);
  }
  return DatabaseRoutinesGlobalFunctionsMongo::ToJSONFetchItem(labelStrings);
}

JSData DatabaseRoutinesGlobalFunctionsMongo::ToJSONFetchItem(const List<std::string>& labelStrings) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::ToJSONFetchItem");
  JSData result;
  if (labelStrings.size < 1) {
    result["error"] = "No collection specified. ";
    return result;
  }
  if (labelStrings.size < 2) {
    result["error"] = "No id specified. ";
    return result;
  }
  std::stringstream out;
  std::string currentTable = labelStrings[0];
  result["currentTable"] = currentTable;
  JSData projector, findQuery;
  findQuery.theType = JSData::token::tokenObject;
  projector.theType = JSData::token::tokenObject;
  findQuery[DatabaseStrings::labelIdMongo][DatabaseStrings::objectSelectorMongo] = labelStrings[1];
  if (labelStrings.size > 2) {
    projector = DatabaseRoutinesGlobalFunctionsMongo::GetProjectionFromFieldNames(labelStrings, 2);
  }
  List<JSData> rowsJSON;
  long long totalItems = 0;
  std::stringstream comments;
  std::stringstream* commentsPointer = nullptr;
  bool flagDebuggingAdmin = theGlobalVariables.UserDefaultIsDebuggingAdmin();
  if (flagDebuggingAdmin) {
    commentsPointer = &comments;
  }
  if (!DatabaseRoutinesGlobalFunctionsMongo::FindFromJSONWithOptions(
    currentTable, findQuery, rowsJSON, projector, 200, &totalItems, &out, commentsPointer
  )) {
    result["error"] = out.str();
    return result;
  }
  JSData theRows;
  theRows.theType = JSData::token::tokenArray;
  theRows.theList = rowsJSON;
  if (flagDebuggingAdmin) {
    result["findQuery"] = findQuery;
  }
  result["rows"] = theRows;
  result["totalRows"] = (int) totalItems;
  return result;
}

JSData DatabaseRoutinesGlobalFunctionsMongo::ToJSONDatabaseCollection(const std::string& currentTable) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::ToJSONDatabaseCollection");
  JSData result;
  std::stringstream out;
  result["currentTable"] = currentTable;
  if (currentTable == "") {
    if (theGlobalVariables.UserDebugFlagOn() != 0) {
      result["comments"] = "Requested table empty, returning list of tables. ";
    }
    List<std::string> theCollectionNames;
    if (DatabaseRoutinesGlobalFunctionsMongo::FetchCollectionNames(theCollectionNames, &out)) {
      JSData collectionNames;
      collectionNames.theType = JSData::token::tokenArray;
      collectionNames.theList = theCollectionNames;
      result["collections"] = collectionNames;
    } else {
      result["error"] = out.str();
    }
    return result;
  }
  JSData allProjectors, projector, findQuery;
  allProjectors = DatabaseRoutinesGlobalFunctionsMongo::GetStandardProjectors();
  if (allProjectors.HasKey(currentTable)) {
    projector = allProjectors[currentTable];
  }
  findQuery.theType = JSData::token::tokenObject;
  List<JSData> rowsJSON;
  long long totalItems = 0;
  std::stringstream comments;
  std::stringstream* commentsPointer = nullptr;
  bool flagDebuggingAdmin = theGlobalVariables.UserDefaultIsDebuggingAdmin();
  if (flagDebuggingAdmin) {
    commentsPointer = &comments;
  }
  if (!DatabaseRoutinesGlobalFunctionsMongo::FindFromJSONWithOptions(
    currentTable, findQuery, rowsJSON, projector, 200, &totalItems, &out, commentsPointer
  )) {
    result["error"] = out.str();
    return result;
  }
  if (rowsJSON.size == 0) {
    result = DatabaseRoutinesGlobalFunctionsMongo::ToJSONDatabaseCollection("");
    if (flagDebuggingAdmin) {
      std::stringstream moreComments;
      moreComments << "First query returned 0 rows, fetching all tables instead. ";
      result["moreComments"] = moreComments.str();
      result["commentsOnFirstQuery"] = commentsPointer->str();
      result["currentTableRawOriginal"] = currentTable;
    }
    return result;
  }
  JSData theRows;
  theRows.theType = JSData::token::tokenArray;
  theRows.theList = rowsJSON;
  result["rows"] = theRows;
  result["totalRows"] = (int) totalItems;
  return result;
}

std::string DatabaseRoutinesGlobalFunctionsMongo::ToHtmlDatabaseCollection(const std::string& currentTable) {
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::ToStringDatabaseCollection");
  std::stringstream out;
  if (currentTable == "") {
    List<std::string> theCollectionNames;
    if (DatabaseRoutinesGlobalFunctionsMongo::FetchCollectionNames(theCollectionNames, &out)) {
      out << "There are " << theCollectionNames.size << " collections. ";
      for (int i = 0; i < theCollectionNames.size; i ++) {
        out << "<br>";
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
  if (!DatabaseRoutinesGlobalFunctionsMongo::FetchTable(currentTable, theLabels, theRows, &totalItems, &out)) {
    return out.str();
  }
  out << "Total: " << totalItems << ". ";
  if (totalItems > theRows.size) {
    out << "Only the first " << theRows.size << " are displayed. ";
  }
  out << "<br>" << HtmlRoutines::ToHtmlTable(theLabels, theRows, true);
  return out.str();
}

