#ifdef MACRO_use_MongoDB
#include <mongoc.h>
#include <bcon.h>
#endif //MACRO_use_MongoDB
#include "database.h"
#include "json.h"
#include "general_logging_global_variables.h"
#include "web_api.h"
#include "string_constants.h"
#include "general_strings.h"

QueryResultOptions Database::getStandardProjectors(const std::string& collectionName) {
  QueryResultOptions result;
  if (collectionName == DatabaseStrings::tableUsers) {
    result.fieldsProjectedAway.addOnTop(DatabaseStrings::labelProblemDataJSON);
  }
  return result;
}

Database& Database::get() {
  static Database result;
  return result;
}

Database::~Database() {
}

Database::Database() {
  this->flagInitializedServer = false;
  this->numDatabaseInstancesMustBeOneOrZero = 0;
}

Database::User::User() {
  this->owner = nullptr;
}

bool Database::checkInitialization() {
  if (!this->flagInitializedServer) {
    global.fatal << "Database not initialized at a place it should be. " << global.fatal;
  }
  return true;
}

bool Database::Mongo::initialize() {
#ifdef MACRO_use_MongoDB
  if (this->flagInitialized) {
    return true;
  }
  global << Logger::blue << "Initializing mongoDB. " << Logger::endL;
  if (!global.flagServerforkedIntoWorker) {
    global.fatal << "MongoDB not allowed to run before server fork. " << global.fatal;
  }
  this->flagInitialized = true;
  mongoc_init();
  this->client = mongoc_client_new("mongodb://localhost:27017");
  this->database = mongoc_client_get_database(
    static_cast<mongoc_client_t*>(this->client),
    DatabaseStrings::theDatabaseName.c_str()
  );
#endif
  return true;
}

Database::Mongo::Mongo() {
  this->flagInitialized = false;
  this->client = nullptr;
  this->database = nullptr;
}

void Database::Mongo::shutdown() {
  if (this->database == nullptr && this->client == nullptr) {
    return;
  }
  this->flagInitialized = false;
  #ifdef MACRO_use_MongoDB
  mongoc_database_destroy(static_cast<mongoc_database_t*>(this->database));
  this->database = nullptr;
  if (this->client != nullptr) {
    mongoc_client_destroy(static_cast<mongoc_client_t*>(this->client));
    this->client = nullptr;
  }
  mongoc_cleanup();
  #endif //MACRO_use_MongoDB
}

Database::Mongo::~Mongo() {
  this->shutdown();
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
  if (!Database::get().mongoDB.initialize()) {
    global.fatal << "Mongo DB not initialized when it should be" << global.fatal;
    return;
  }
  this->name = collectionName;
  this->collection = mongoc_client_get_collection(
    static_cast<mongoc_client_t*>(Database::get().mongoDB.client),
    DatabaseStrings::theDatabaseName.c_str(),
    this->name.c_str()
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
  bool flagfindOneOnly;
  MongoQuery();
  ~MongoQuery();
  bool FindMultiple(
    List<JSData>& output,
    const JSData& inputOptions,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNonSensitive
  );
  bool findOne(
    JSData& output,
    const JSData& inputOptions,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNonSensitive
  );
  bool updateOneWithOptions(std::stringstream* commentsOnFailure);
  bool RemoveOne(std::stringstream* commentsOnFailure);
  bool updateOne(std::stringstream* commentsOnFailure, bool doUpsert);
  bool InsertOne(const JSData& incoming, std::stringstream* commentsOnFailure);
  bool updateOneNoOptions(std::stringstream* commentsOnFailure);
  std::string toStringDebug();
  // Abbreviation to get the default database.
  Database::Mongo& getDB() {
    return Database::get().mongoDB;
  }
};

MongoQuery::MongoQuery() {
  if (!Database::get().mongoDB.initialize()) {
    global.fatal << "Mongo DB did not start correctly. " << global.fatal;
  }
  this->query = nullptr;
  this->command = nullptr;
  this->update = nullptr;
  this->options = nullptr;
  this->cursor = nullptr;
  this->updateResult = nullptr;
  this->maxOutputItems = - 1;
  this->totalItems = - 1;
  this->flagfindOneOnly = false;
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
  if (!Database::get().mongoDB.initialize()) {
    return false;
  }
  MongoCollection theCollection(this->collectionName);
  if (this->query != nullptr) {
    global.fatal << "At this point of code, query is supposed to be 0. " << global.fatal;
  }
  this->query = bson_new_from_json(
    reinterpret_cast<const uint8_t*>(this->findQuery.c_str()),
    static_cast<unsigned>(this->findQuery.size()),
    &this->theError
  );
  bool result = mongoc_collection_remove(
    theCollection.collection,
    MONGOC_REMOVE_SINGLE_REMOVE,
    this->query,
    nullptr,
    &this->theError
  );
  if (!result) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << this->theError.message;
    }
    global << Logger::red << "While removing: "
    << this->findQuery << " inside: "
    << this->collectionName << " got mongo error: "
    << this->theError.message << Logger::endL;
    return false;
  }
  //char* bufferOutpurStringFormat = 0;
  //bufferOutpurStringFormat = bson_as_canonical_extended_json(this->updateResult, NULL);
  //std::string updateResultString(bufferOutpurStringFormat);
  //bson_free(bufferOutpurStringFormat);
  //global << Logger::red << "Update result: " << updateResultString << Logger::endL;
  return true;
}

std::string MongoQuery::toStringDebug() {
  std::stringstream out;
  out << this->collectionName << ", " << this->findQuery << ", "
  << this->updateQuery << ", " << this->optionsQuery;
  return out.str();
}

bool MongoQuery::InsertOne(const JSData& incoming, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("MongoQuery::InsertOne");
  if (!Database::get().mongoDB.initialize()) {
    return false;
  }
  MongoCollection theCollection(this->collectionName);
  if (this->query != nullptr) {
    global.fatal << "At this point of code, query is supposed to be 0. " << global.fatal;
  }
  std::string incomingJSONString = incoming.toString(nullptr);
  this->update = bson_new_from_json(
    reinterpret_cast<const uint8_t*>(incomingJSONString.c_str()),
    static_cast<signed>(incomingJSONString.size()),
    &this->theError
  );
  if (this->update == nullptr) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to create insertion bson.";
    }
    return false;
  }
  this->updateResult = bson_new();
  bool result = mongoc_collection_insert_one(
    theCollection.collection, this->update, nullptr, this->updateResult, &this->theError
  );
  if (!result) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << this->theError.message;
    }
    global << Logger::red << "While updating: "
    << this->findQuery << " to: " << this->updateQuery << " inside: "
    << this->collectionName << " got mongo error: "
    << this->theError.message << Logger::endL;
    return false;
  }
  // char* bufferOutpurStringFormat = 0;
  // bufferOutpurStringFormat = bson_as_canonical_extended_json(this->updateResult, NULL);
  // std::string updateResultString(bufferOutpurStringFormat);
  // bson_free(bufferOutpurStringFormat);
  // global << Logger::red << "Update result: " << updateResultString << Logger::endL;
  return true;
}

bool MongoQuery::updateOne(std::stringstream* commentsOnFailure, bool doUpsert) {
  MacroRegisterFunctionWithName("MongoQuery::updateOne");
  if (!Database::get().mongoDB.initialize()) {
    return false;
  }
  MongoCollection theCollection(this->collectionName);
  if (this->query != nullptr) {
    global.fatal << "At this point of code, query is supposed to be 0. " << global.fatal;
  }
  this->query = bson_new_from_json(
    reinterpret_cast<const uint8_t*>(this->findQuery.c_str()),
    static_cast<signed>(this->findQuery.size()),
    &this->theError
  );
  this->update = bson_new_from_json(
    reinterpret_cast<const uint8_t*>(this->updateQuery.c_str()),
    static_cast<signed>(this->updateQuery.size()),
    &this->theError
  );
  if (doUpsert) {
    this->optionsQuery = "{\"upsert\": true}";
    this->options = bson_new_from_json(
      reinterpret_cast<const uint8_t*>(this->optionsQuery.c_str()),
      static_cast<signed>(this->optionsQuery.size()),
      &this->theError
    );
  }
  this->updateResult = bson_new();
  bool result = false;
  if (doUpsert) {
    result = mongoc_collection_update_one(
      theCollection.collection,
      this->query,
      this->update,
      this->options,
      this->updateResult,
      &this->theError
    );
  } else {
    result = mongoc_collection_update_one(
      theCollection.collection, this->query, this->update, nullptr, this->updateResult, &this->theError
    );
  }
  if (!result) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << this->theError.message;
    }
    global << Logger::red << "While updating: "
    << this->findQuery << " to: " << this->updateQuery << " inside: "
    << this->collectionName << " got mongo error: "
    << this->theError.message << Logger::endL;
    return false;
  }
  // char* bufferOutpurStringFormat = 0;
  // bufferOutpurStringFormat = bson_as_canonical_extended_json(this->updateResult, NULL);
  // std::string updateResultString(bufferOutpurStringFormat);
  // bson_free(bufferOutpurStringFormat);
  // global << Logger::red << "Update result: " << updateResultString << Logger::endL;
  return true;
}

bool MongoQuery::updateOneNoOptions(std::stringstream* commentsOnFailure) {
  return this->updateOne(commentsOnFailure, false);
}

bool MongoQuery::updateOneWithOptions(std::stringstream* commentsOnFailure) {
  return this->updateOne(commentsOnFailure, true);
}

bool MongoQuery::findOne(
  JSData& output,
  const JSData& inputOptions,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive
) {
  List<JSData> outputsMultiple;
  if (!this->FindMultiple(
    outputsMultiple,
    inputOptions,
    commentsOnFailure,
    commentsGeneralNonSensitive
  )) {
    return false;
  }
  if (outputsMultiple.size < 1) {
    return false;
  }
  output = outputsMultiple[0];
  return true;
}

bool MongoQuery::FindMultiple(
  List<JSData>& output,
  const JSData& inputOptions,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive
) {
  MacroRegisterFunctionWithName("MongoQuery::FindMultiple");
  if (!Database::get().mongoDB.initialize()) {
    return false;
  }
  MongoCollection theCollection(this->collectionName);
  if (commentsGeneralNonSensitive != nullptr && global.userDefaultHasAdminRights()) {
    *commentsGeneralNonSensitive
    << "Query: " << this->findQuery << ". Options: " << inputOptions.toString(nullptr);
  }
  if (this->query != nullptr) {
    global.fatal << "At this point of code, query is supposed to be 0. " << global.fatal;
  }
  this->query = bson_new_from_json(
    reinterpret_cast<const uint8_t*>(this->findQuery.c_str()),
    static_cast<signed>(this->findQuery.size()),
    &this->theError
  );
  if (this->query == nullptr) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << this->theError.message;
    }
    global << Logger::red << "While finding: "
    << this->findQuery << " inside: "
    << this->collectionName << " got mongo error: "
    << this->theError.message << Logger::endL;
    return false;
  }
  if (this->options != nullptr) {
    bson_destroy(this->options);
    this->options = nullptr;
  }
  if (inputOptions.theType != JSData::token::tokenUndefined) {
    std::string optionsString = inputOptions.toString(nullptr);
    this->options = bson_new_from_json(
      reinterpret_cast<const uint8_t*>(optionsString.c_str()),
      static_cast<signed>(optionsString.size()),
      &this->theError
    );
    if (this->options == nullptr) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << this->theError.message;
      }
      global << Logger::red << "Mongo error: " << this->theError.message << Logger::endL;
      return false;
    }
  }
  this->cursor = mongoc_collection_find_with_opts(
    theCollection.collection, this->query, this->options, nullptr
  );
  if (this->cursor == nullptr) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Bad mongoDB cursor. ";
    }
    global << Logger::red << "Mongo error: bad mongoDB cursor. " << Logger::endL;
    return false;
  }
  this->totalItems = 0;
  const bson_t* bufferOutput;
  List<std::string> outputString;
  while (mongoc_cursor_next(this->cursor, &bufferOutput)) {
    char* bufferOutpurStringFormat = nullptr;
    bufferOutpurStringFormat = bson_as_canonical_extended_json(bufferOutput, nullptr);
    std::string current(bufferOutpurStringFormat);
    bson_free(bufferOutpurStringFormat);
    if (this->maxOutputItems <= 0 || this->totalItems < this->maxOutputItems) {
      outputString.addOnTop(current);
    }
    this->totalItems ++;
    if (this->flagfindOneOnly) {
      break;
    }
  }
  output.setSize(outputString.size);
  for (int i = 0; i < outputString.size; i ++) {
    JSData encoded;
    if (!encoded.readstring(outputString[i], commentsOnFailure)) {
      global << Logger::red << "Mongo/JSData error: failed to parse JSON. " << Logger::endL;
      return false;
    }
    if (!Database::convertJSONMongoToJSON(encoded, output[i], commentsOnFailure)) {
      global << Logger::red << "Mongo/JSData error: failed to convert mongo JSON to JSON. " << Logger::endL;
      return false;
    }
  }
  return true;
}
#endif

std::string Database::Mongo::convertErrorToString(void* bson_error_t_pointer) {
  std::stringstream out;
#ifdef MACRO_use_MongoDB
  bson_error_t* error = static_cast<bson_error_t*>(bson_error_t_pointer);
  out << "Mongo message: " << error->message << ", code: " << error->code <<  ", domain: " << error->domain;
#else
  (void) bson_error_t_pointer;
  out << "Mongo DB not compiled. ";
#endif
  return out.str();
}

bool Database::Mongo::deleteDatabase(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("Database::Mongo::deleteDatabase");
  if (DatabaseStrings::theDatabaseName != "calculatortest") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "The only database allowed to be deleted is calculatortest. ";
    }
    return false;
  }
#ifdef MACRO_use_MongoDB
  if (!this->initialize()) {
    return false;
  }
  bson_error_t errorResult;
  if (!mongoc_database_drop(static_cast<mongoc_database_t*>(this->database), &errorResult)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to drop database. " << this->convertErrorToString(&errorResult);
    }
    return false;
  }
  return true;
#else
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Cannot delete database: not compiled with mongoDB support.";
  }
  return false;
#endif
}

void Database::Mongo::createHashIndex(
  const std::string& collectionName, const std::string& theKey
) {
  (void) collectionName;
  (void) theKey;
  #ifdef MACRO_use_MongoDB
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::createHashIndex");
  MongoQuery query;
  std::stringstream theCommand;
  theCommand << "{\"createIndexes\":\"" << collectionName << "\", \"indexes\": [{\"key\":{\""
  << theKey << "\": \"hashed\"}, \"name\": \"" << collectionName  << "_" << theKey << "_hash\"" << "}]} ";
  query.command = bson_new_from_json(
    reinterpret_cast<const uint8_t*>(theCommand.str().c_str()),
    static_cast<signed>(theCommand.str().size()),
    &query.theError
  );
  mongoc_database_write_command_with_opts(
    static_cast<mongoc_database_t*>(this->database),
    query.command,
    nullptr,
    query.updateResult,
    &query.theError
  );
  #endif
}

bool Database::findFromString(
  const std::string& collectionName,
  const std::string& findQuery,
  List<JSData>& output,
  int maxOutputItems,
  long long* totalItems,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::findFromString");
#ifdef MACRO_use_MongoDB
  JSData theData;
  //global << Logger::blue << "Query input: " << findQuery << Logger::endL;
  if (!theData.readstring(findQuery, commentsOnFailure)) {
    return false;
  }
  return Database::get().findFromJSON(
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

void QueryResultOptions::makeProjection(const List<std::string>& fields) {
  this->fieldsToProjectTo = fields;
}

JSData QueryResultOptions::toJSON() const {
  JSData result, fields;
  result.reset(JSData::token::tokenObject);
  bool found = false;
  for (int i = 0; i < this->fieldsToProjectTo.size; i ++) {
    fields[this->fieldsToProjectTo[i]] = 1;
    found = true;
  }
  for (int i = 0; i < this->fieldsProjectedAway.size; i ++) {
    fields[this->fieldsProjectedAway[i]] = 0;
    found = true;
  }
  if (found) {
    result["projection"] = fields;
  }
  return result;
}

bool Database::findFromJSONWithProjection(
  const std::string& collectionName,
  const JSData& findQuery,
  List<JSData>& output,
  List<std::string>& fieldsToProjectTo,
  int maxOutputItems,
  long long* totalItems,
  std::stringstream* commentsOnFailure
) {
  QueryResultOptions options;
  options.makeProjection(fieldsToProjectTo);
  return Database::findFromJSONWithOptions(
    collectionName,
    findQuery,
    output,
    options,
    maxOutputItems,
    totalItems,
    commentsOnFailure
  );
}

bool Database::findFromJSON(
  const std::string& collectionName,
  const JSData& findQuery,
  List<JSData>& output,
  int maxOutputItems,
  long long* totalItems,
  std::stringstream* commentsOnFailure
) {
  QueryResultOptions options;
  return Database::findFromJSONWithOptions(
    collectionName,
    findQuery,
    output,
    options,
    maxOutputItems,
    totalItems,
    commentsOnFailure
  );
}

bool Database::findFromJSONWithOptions(
  const std::string& collectionName,
  const JSData& findQuery,
  List<JSData>& output,
  const QueryResultOptions& options,
  int maxOutputItems,
  long long* totalItems,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive
) {
  MacroRegisterFunctionWithName("Database::findFromJSONWithOptions");
  if (global.flagDisableDatabaseLogEveryoneAsAdmin) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Database::findFromJSONWithOptions fail. " << DatabaseStrings::errorDatabaseDisableD;
    }
    return false;
  }
  (void) commentsGeneralNonSensitive;
#ifdef MACRO_use_MongoDB
  MongoQuery query;
  query.collectionName = collectionName;
  query.findQuery = findQuery.toString(nullptr);
  query.maxOutputItems = maxOutputItems;
  global << Logger::blue << "Query input JSON: " << query.toStringDebug()
  << ", options: " << options.toJSON().toString()
  << Logger::endL;
  bool result = query.FindMultiple(
    output, options.toJSON(), commentsOnFailure, commentsGeneralNonSensitive
  );
  if (totalItems != nullptr) {
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
    *commentsOnFailure << "findFromJSONWithOptions: project compiled without mongoDB support. ";
  }
  return false;
#endif
}

bool Database::Mongo::findOneFromSome(
  const List<QueryExact>& findOrQueries,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::findOneFromSome");
  #ifdef MACRO_use_MongoDB
  MongoQuery query;
  if (!Database::Mongo::getOrFindQuery(findOrQueries, query.findQuery, commentsOnFailure)) {
    return false;
  }
  for (int i = 0; i < findOrQueries.size; i ++) {
    if (i == 0) {
      query.collectionName = findOrQueries[i].collection;
      continue;
    }
    if (query.collectionName != findOrQueries[i].collection) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Not allowed: or-queries involve different collections: "
        << query.collectionName << " and " << findOrQueries[i].collection << ". ";
      }
      return false;
    }
  }
  JSData emptyOptions;
  return query.findOne(output,emptyOptions, commentsOnFailure, nullptr);
  #else
  (void) output;
  (void) findOrQueries;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "MongoDB not compiled in where it should be. ";
  }
  return false;
  #endif
}

bool Database::Mongo::getOrFindQuery(
  const List<QueryExact>& input,
  std::string& output,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::getOrFindQuery");
  (void) commentsOnFailure;
  std::stringstream queryStream;
  queryStream << "{\"$or\": [";
  for (int i = 0; i < input.size; i ++) {
    queryStream << input[i].toJSON().toString(nullptr);
    if (i < input.size - 1) {
      queryStream << ", ";
    }
  }
  queryStream << "]}";
  output = queryStream.str();
  return true;
}

bool Database::Mongo::findOneWithOptions(
  const QueryExact& query,
  const QueryResultOptions& options,
  JSData& output,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive
) {
#ifdef MACRO_use_MongoDB
  MongoQuery mongoQuery;
  mongoQuery.collectionName = query.collection;
  mongoQuery.findQuery = query.toJSON().toString();
  mongoQuery.maxOutputItems = 1;
  List<JSData> outputList;
  mongoQuery.FindMultiple(
    outputList, options.toJSON(), commentsOnFailure, commentsGeneralNonSensitive
  );
  if (outputList.size == 0) {
    return false;
  }

  output  = outputList[0];

  return true;
#else
  (void) query;
  (void) options;
  (void) output;
  (void) commentsGeneralNonSensitive;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Database::Mongo::findOneWithOptions failed. No mongoDB support. ";
  }
  return false;
#endif
}

bool Database::matchesPattern(
  const List<std::string>& fieldLabel, const List<std::string>& pattern
) {
  MacroRegisterFunctionWithName("Database::matchesPattern");
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

bool Database::isDeleteable(
  const List<std::string>& theLabels, List<std::string>** outputPattern, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::isDeleteable");
  for (int i = 0; i < global.databaseModifiableFields.size; i ++) {
    if (Database::matchesPattern(
      theLabels, global.databaseModifiableFields[i]
    )) {
      if (outputPattern != nullptr) {
        *outputPattern = &global.databaseModifiableFields[i];
      }
      return true;
    }
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Labels: " << theLabels.toStringCommaDelimited()
    << " do not match any modifiable field pattern. ";
  }
  return false;
}

bool Database::getLabels(
  const JSData& fieldEntries, List<std::string>& output, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::getLabels");
  output.setSize(0);
  for (int i = 0; i < fieldEntries.theList.size; i ++) {
    if (fieldEntries.theList[i].theType != JSData::token::tokenString) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Label index " << i << " is not of type string as required. ";
      }
      return false;
    }
    output.addOnTop(fieldEntries.theList[i].theString);
  }
  return true;
}

bool Database::isDeleteable(
  const JSData& theEntry, List<std::string>** outputPattern, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::isDeleteable");
  if (theEntry.theType != JSData::token::tokenObject || !theEntry.hasKey(DatabaseStrings::labelFields)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "The labels json is required to be an object of the form {fields: [tableName, objectId,...]}. ";
    }
    return false;
  }
  List<std::string> theLabels;
  if (!Database::getLabels(
    theEntry.objects.getValueNoFail("fields"), theLabels, commentsOnFailure
  )) {
    return false;
  }
  return Database::isDeleteable(theLabels, outputPattern, commentsOnFailure);
}

bool Database::deleteOneEntry(const JSData& theEntry, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("Database::deleteOneEntry");
  if (!global.userDefaultHasAdminRights()) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Only logged-in admins can delete DB entries.";
    }
    return false;
  }
  List<std::string>* labelTypes = nullptr;
  if (!isDeleteable(theEntry, &labelTypes, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Entry: " << theEntry.toString(nullptr) << " not deleteable.";
    }
    return false;
  }
  List<std::string> theLabels;
  if (!Database::getLabels(
    theEntry.objects.getValueNoFail(DatabaseStrings::labelFields),
    theLabels,
    commentsOnFailure
  )) {
    return false;
  }
  if (theLabels.size < 2) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "When deleting an object, it needs at least two labels: table name and objectid. "
      << "Your input appears to have only " << theLabels.size << " entries: " << theLabels.toStringCommaDelimited()
      << ". ";
    }
    return false;
  }
  QueryExact findQuery(
    theLabels[0],
    List<std::string>({
      DatabaseStrings::labelIdMongo,
      DatabaseStrings::objectSelectorMongo
    }),
    theLabels[1]
  );
  std::string tableName = theLabels[0];
  if (theLabels.size == 0) {
    return this->deleteOneEntryById(findQuery, commentsOnFailure);
  }
  return Database::deleteOneEntryUnsetUnsecure(
    findQuery, theLabels, commentsOnFailure
  );
}

bool Database::deleteOneEntryById(
  const QueryExact& findQuery, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::deleteOneEntryById");
#ifdef MACRO_use_MongoDB
  JSData foundItem;
  if (!this->findOne(findQuery, foundItem, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Query: "
      << findQuery.toJSON().toString(nullptr) << " returned no hits in table: "
      << findQuery.collection;
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
  query.findQuery = findQuery.toJSON().toString(nullptr);
  query.collectionName = findQuery.collection;
  return query.RemoveOne(commentsOnFailure);
#else
  (void) findQuery;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "deleteOneEntryById: project compiled without mongoDB support. ";
  }
  return false;
#endif

}

bool Database::deleteOneEntryUnsetUnsecure(
  const QueryExact& findQuery,
  List<std::string>& selector,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::deleteOneEntryUnsetUnsecure");
#ifdef MACRO_use_MongoDB

  std::string selectorString = QueryExact::getLabelFromNestedLabels(selector);
  JSData foundItem;
  QueryResultOptions options;
  options.fieldsToProjectTo.addListOnTop(selector);
  options.fieldsToProjectTo.addOnTop(selectorString);
  bool didFindItem = Database::get().findOneWithOptions(
    findQuery,
    options,
    foundItem,
    commentsOnFailure,
    nullptr
  );

  if (!didFindItem) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Query: " << findQuery.toJSON().toString(nullptr)
      << " returned no hits in table: "
      << findQuery.collection;
    }
    return false;
  }
  MongoQuery queryBackUp;
  queryBackUp.collectionName = DatabaseStrings::tableDeleted;
  JSData backUp;
  backUp["deleted"] = foundItem;
  if (!queryBackUp.InsertOne(backUp, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to insert backup: " << backUp.toString(nullptr);
    }
    return false;
  }
  MongoQuery query;
  query.findQuery = findQuery.toJSON().toString(nullptr);
  query.collectionName = findQuery.collection;
  std::stringstream updateQueryStream;
  updateQueryStream << "{\"$unset\": {\"" << selectorString << "\":\"\"}}";
  query.updateQuery = updateQueryStream.str();
  return query.updateOneNoOptions(commentsOnFailure);
#else
  (void) findQuery;
  (void) selector;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "deleteOneEntryUnsetUnsecure: project compiled without mongoDB support. ";
  }
  return false;
#endif
}

bool Database::Mongo::updateOneFromQueryString(
  const std::string& collectionName,
  const std::string& findQuery,
  const std::string& updateQuery,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::updateOneFromQueryString");
#ifdef MACRO_use_MongoDB
  MongoQuery query;
  query.findQuery = findQuery;
  query.collectionName = collectionName;
  query.updateQuery = updateQuery;
  return query.updateOneWithOptions(commentsOnFailure);
#else
  (void) collectionName;
  (void) findQuery;
  (void) updateQuery;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "updateOneFromQueryString failed: DB not compiled.";
  }
  return false;
#endif
}

bool Database::updateOneFromSome(
  const List<QueryExact>& findOrQueries,
  const QuerySet& updateQuery,
  std::stringstream* commentsOnFailure
) {
  if (global.flagDatabaseCompiled) {
    return this->mongoDB.updateOneFromSome(findOrQueries, updateQuery, commentsOnFailure);
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Database::updateOneFromSome not implemented yet. ";
  }
  return false;
}

bool Database::Mongo::updateOneFromSome(
  const List<QueryExact>& findOrQueries,
  const QuerySet& updateQuery,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::updateOneFromSomeJSON");
  std::string queryString;
  if (!this->getOrFindQuery(findOrQueries, queryString, commentsOnFailure)) {
    return false;
  }
  if (findOrQueries.size == 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Empty or-queries not allowed. ";
    }
    return false;
  }
  JSData setJSON;
  if (!updateQuery.toJSONSetMongo(setJSON, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to extract set mongo json. ";
    }
    return false;
  }
  return this->updateOneFromQueryString(
    findOrQueries[0].collection,
    queryString,
    setJSON.toString(nullptr),
    commentsOnFailure
  );
}

QuerySet::QuerySet() {
  this->value.theType = JSData::token::tokenObject;
}

QuerySet::QuerySet(const JSData& inputValue) {
  this->value = inputValue;
}

std::string QuerySet::toStringDebug() const {
  std::stringstream out;
  JSData jsonSetMongo;
  if (!this->toJSONSetMongo(jsonSetMongo, &out)) {
    return out.str();
  }
  out << jsonSetMongo.toString(nullptr);
  return out.str();
}

bool QuerySet::toJSONSetMongo(JSData& output, std::stringstream* commentsOnFailure) const {
  JSData data;
  if (!this->toJSONMongo(data, commentsOnFailure)) {
    return false;
  }
  output.reset(JSData::token::token::tokenObject);
  output["$set"] = data;
  return true;
}

bool QuerySet::toJSONMongo(
  JSData& output, std::stringstream* commentsOnFailure
) const {
  JSData converted;
  if (!Database::convertJSONToJSONMongo(this->value, converted, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to convert: "
      << this->value.toString(nullptr) << " to JSON. ";
    }
    return false;
  }
  if (converted.theType != JSData::token::tokenObject) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Query is not converted to an object. ";
    }
    return false;
  }
  output.reset(JSData::token::tokenObject);
  std::string keyPrefix = QueryExact::getLabelFromNestedLabels(
    this->nestedLabels
  );
  if (keyPrefix != "") {
    keyPrefix += ".";
  }
  for (int i = 0; i < converted.objects.size(); i ++) {
    output[keyPrefix + converted.objects.keys[i]] = converted.objects.values[i];
  }
  return true;
}

bool Database::Mongo::updateOne(
  const QueryExact& findQuery,
  const QuerySet& updateQuery,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::updateOneFromJSON");
  JSData updateQueryJSON;
  if (!updateQuery.toJSONSetMongo(updateQueryJSON, commentsOnFailure)) {
    return false;
  }
  if (!Database::Mongo::updateOneFromQueryString(
    findQuery.collection,
    findQuery.toJSON().toString(nullptr),
    updateQueryJSON.toString(nullptr),
    commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to update one element. ";
    }
    global << "Failed to update element found by: "
    << findQuery.toJSON().toString(nullptr)
    << " with: " << updateQuery.toStringDebug();
  }
  return true;
}

bool Database::User::loadUserInformation(
  UserCalculatorData& output, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::loadUserInformation");
  if (output.username == "" && output.email == "") {
    return false;
  }
  JSData userEntry;
  if (!this->owner->findOneFromSome(
    output.getFindMeFromUserNameQuery(),
    userEntry,
    commentsOnFailure
  )) {
    return false;
  }
  return output.loadFromJSON(userEntry);
}

bool Database::fetchCollectionNames(
  List<std::string>& output, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::fetchCollectionNames");
  if (!Database::get().initializeWorker()) {
    return false;
  }
  if (global.flagDisableDatabaseLogEveryoneAsAdmin) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Database disabled. ";
    }
    return false;
  }
  if (global.flagDatabaseCompiled) {
    return Database::get().mongoDB.fetchCollectionNames(output, commentsOnFailure);
  }
  return this->theFallBack.fetchCollectionNames(output, commentsOnFailure);
}

bool Database::Mongo::fetchCollectionNames(
  List<std::string>& output, std::stringstream* commentsOnFailure
) {
#ifdef MACRO_use_MongoDB
  bson_t opts = BSON_INITIALIZER;
  bson_error_t error;
  mongoc_database_get_collection_names_with_opts(
    static_cast<mongoc_database_t*>(this->database), &opts, &error
  );
  char **theCollectionChars = mongoc_database_get_collection_names_with_opts(
    static_cast<mongoc_database_t*>(this->database), &opts, &error
  );
  bool result = true;
  output.setSize(0);
  if (theCollectionChars != nullptr) {
    for (int i = 0; theCollectionChars[i]; i ++) {
      std::string currentCollection(theCollectionChars[i]);
      output.addOnTop(currentCollection);
    }
    bson_strfreev(theCollectionChars);
    theCollectionChars = nullptr;
  } else {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to fetch all collections. ";
    }
    result = false;
  }
  bson_destroy(&opts);
  return result;
#else
  (void) output;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Database not compiled. ";
  }
  return false;
#endif
}

bool Database::FetchTable(
  const std::string& tableName,
  List<std::string>& outputLabels,
  List<List<std::string> >& outputRows,
  long long* totalItems,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::FetchTable");
  JSData findQuery;
  findQuery.theType = JSData::token::tokenObject;
  List<JSData> rowsJSON;
  Database::get().findFromJSON(
    tableName, findQuery, rowsJSON, 200, totalItems, commentsOnFailure
  );
  HashedList<std::string, MathRoutines::hashString> theLabels;
  for (int i = 0; i < rowsJSON.size; i ++) {
    for (int j = 0; j < rowsJSON[i].objects.size(); j ++) {
      theLabels.addOnTopNoRepetition(rowsJSON[i].objects.keys[j]);
    }
  }
  outputLabels = theLabels;
  outputRows.setSize(rowsJSON.size);
  for (int i = 0; i < rowsJSON.size; i ++) {
    outputRows[i].setSize(theLabels.size);
    for (int j = 0; j < theLabels.size; j ++) {
      if (rowsJSON[i].objects.contains(theLabels[j])) {
        outputRows[i][j] = rowsJSON[i].getValue(theLabels[j]).toString(nullptr);
      } else {
        outputRows[i][j] = "";
      }
    }
  }
  return true;
}

JSData Database::toJSONDatabaseFetch(const std::string& incomingLabels) {
  MacroRegisterFunctionWithName("Database::toJSONDatabaseFetch");
  JSData result;
  JSData labels;
  std::stringstream commentsOnFailure;
  if (!labels.readstring(incomingLabels, &commentsOnFailure)) {
    commentsOnFailure << "Failed to parse labels from: "
    << StringRoutines::stringTrimToLengthForDisplay(incomingLabels, 100);
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
    return this->toJSONDatabaseCollection("");
  }
  if (labelStrings.size == 1) {
    return this->toJSONDatabaseCollection(labelStrings[0]);
  }
  return Database::toJSONFetchItem(labelStrings);
}

JSData Database::toJSONFetchItem(const List<std::string>& labelStrings) {
  MacroRegisterFunctionWithName("Database::toJSONFetchItem");
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
  JSData findQuery;
  findQuery.theType = JSData::token::tokenObject;
  findQuery[DatabaseStrings::labelIdMongo][DatabaseStrings::objectSelectorMongo] = labelStrings[1];
  QueryResultOptions projector;
  labelStrings.slice(2, labelStrings.size - 2,  projector.fieldsToProjectTo);
  List<JSData> rowsJSON;
  long long totalItems = 0;
  std::stringstream comments;
  std::stringstream* commentsPointer = nullptr;
  bool flagDebuggingAdmin = global.userDefaultIsDebuggingAdmin();
  if (flagDebuggingAdmin) {
    commentsPointer = &comments;
  }
  if (!Database::findFromJSONWithOptions(
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
  result["totalRows"] = static_cast<int>(totalItems);
  return result;
}

JSData Database::toJSONDatabaseCollection(const std::string& currentTable) {
  MacroRegisterFunctionWithName("Database::toJSONDatabaseCollection");
  JSData result;
  std::stringstream out;
  result["currentTable"] = currentTable;
  if (currentTable == "") {
    if (global.userDebugFlagOn() != 0) {
      result[WebAPI::result::comments] = "Requested table empty, returning list of tables. ";
    }
    List<std::string> theCollectionNames;
    if (Database::fetchCollectionNames(theCollectionNames, &out)) {
      JSData collectionNames;
      collectionNames.theType = JSData::token::tokenArray;
      collectionNames.theList.setSize(theCollectionNames.size);
      for (int i = 0; i < theCollectionNames.size; i ++) {
        collectionNames[i] = theCollectionNames[i];
      }
      result["collections"] = collectionNames;
    } else {
      result["error"] = out.str();
    }
    return result;
  }
  JSData findQuery;
  QueryResultOptions projector;
  projector = Database::getStandardProjectors(currentTable);
  findQuery.theType = JSData::token::tokenObject;
  List<JSData> rowsJSON;
  long long totalItems = 0;
  std::stringstream comments;
  std::stringstream* commentsPointer = nullptr;
  bool flagDebuggingAdmin = global.userDefaultIsDebuggingAdmin();
  if (flagDebuggingAdmin) {
    commentsPointer = &comments;
  }
  if (!Database::findFromJSONWithOptions(
    currentTable, findQuery, rowsJSON, projector, 200, &totalItems, &out, commentsPointer
  )) {
    result["error"] = out.str();
    return result;
  }
  if (rowsJSON.size == 0) {
    result = Database::toJSONDatabaseCollection("");
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
  result["totalRows"] = static_cast<int>(totalItems);
  return result;
}

std::string Database::toHtmlDatabaseCollection(const std::string& currentTable) {
  MacroRegisterFunctionWithName("Database::ToStringDatabaseCollection");
  std::stringstream out;
  if (currentTable == "") {
    List<std::string> theCollectionNames;
    if (this->fetchCollectionNames(theCollectionNames, &out)) {
      out << "There are " << theCollectionNames.size << " collections. ";
      for (int i = 0; i < theCollectionNames.size; i ++) {
        out << "<br>";
        out << "<a href=\"" << global.displayNameExecutable
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
  if (!Database::FetchTable(currentTable, theLabels, theRows, &totalItems, &out)) {
    return out.str();
  }
  out << "Total: " << totalItems << ". ";
  if (totalItems > theRows.size) {
    out << "Only the first " << theRows.size << " are displayed. ";
  }
  out << "<br>" << HtmlRoutines::toHtmlTable(theLabels, theRows, true);
  return out.str();
}

