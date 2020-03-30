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

QueryResultOptions Database::GetStandardProjectors(const std::string& collectionName) {
  QueryResultOptions result;
  if (collectionName == DatabaseStrings::tableUsers) {
    result.fieldsProjectedAway.AddOnTop(DatabaseStrings::labelProblemDataJSON);
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

bool Database::CheckInitialization() {
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
  global << logger::blue << "Initializing mongoDB. " << logger::endL;
  if (!global.flagServerForkedIntoWorker) {
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
  bool flagFindOneOnly;
  MongoQuery();
  ~MongoQuery();
  bool FindMultiple(
    List<JSData>& output,
    const JSData& inputOptions,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNonSensitive
  );
  bool FindOne(
    JSData& output,
    const JSData& inputOptions,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNonSensitive
  );
  bool UpdateOneWithOptions(std::stringstream* commentsOnFailure);
  bool RemoveOne(std::stringstream* commentsOnFailure);
  bool UpdateOne(std::stringstream* commentsOnFailure, bool doUpsert);
  bool InsertOne(const JSData& incoming, std::stringstream* commentsOnFailure);
  bool UpdateOneNoOptions(std::stringstream* commentsOnFailure);
  std::string ToStringDebug();
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
    global << logger::red << "While removing: "
    << this->findQuery << " inside: "
    << this->collectionName << " got mongo error: "
    << this->theError.message << logger::endL;
    return false;
  }
  //char* bufferOutpurStringFormat = 0;
  //bufferOutpurStringFormat = bson_as_canonical_extended_json(this->updateResult, NULL);
  //std::string updateResultString(bufferOutpurStringFormat);
  //bson_free(bufferOutpurStringFormat);
  //global << logger::red << "Update result: " << updateResultString << logger::endL;
  return true;
}

std::string MongoQuery::ToStringDebug() {
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
    global << logger::red << "While updating: "
    << this->findQuery << " to: " << this->updateQuery << " inside: "
    << this->collectionName << " got mongo error: "
    << this->theError.message << logger::endL;
    return false;
  }
  // char* bufferOutpurStringFormat = 0;
  // bufferOutpurStringFormat = bson_as_canonical_extended_json(this->updateResult, NULL);
  // std::string updateResultString(bufferOutpurStringFormat);
  // bson_free(bufferOutpurStringFormat);
  // global << logger::red << "Update result: " << updateResultString << logger::endL;
  return true;
}

bool MongoQuery::UpdateOne(std::stringstream* commentsOnFailure, bool doUpsert) {
  MacroRegisterFunctionWithName("MongoQuery::UpdateOne");
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
    global << logger::red << "While updating: "
    << this->findQuery << " to: " << this->updateQuery << " inside: "
    << this->collectionName << " got mongo error: "
    << this->theError.message << logger::endL;
    return false;
  }
  // char* bufferOutpurStringFormat = 0;
  // bufferOutpurStringFormat = bson_as_canonical_extended_json(this->updateResult, NULL);
  // std::string updateResultString(bufferOutpurStringFormat);
  // bson_free(bufferOutpurStringFormat);
  // global << logger::red << "Update result: " << updateResultString << logger::endL;
  return true;
}

bool MongoQuery::UpdateOneNoOptions(std::stringstream* commentsOnFailure) {
  return this->UpdateOne(commentsOnFailure, false);
}

bool MongoQuery::UpdateOneWithOptions(std::stringstream* commentsOnFailure) {
  return this->UpdateOne(commentsOnFailure, true);
}

bool MongoQuery::FindOne(
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
  if (commentsGeneralNonSensitive != nullptr && global.UserDefaultHasAdminRights()) {
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
    global << logger::red << "While finding: "
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
      global << logger::red << "Mongo error: " << this->theError.message << logger::endL;
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
    global << logger::red << "Mongo error: bad mongoDB cursor. " << logger::endL;
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
      outputString.AddOnTop(current);
    }
    this->totalItems ++;
    if (this->flagFindOneOnly) {
      break;
    }
  }
  output.SetSize(outputString.size);
  for (int i = 0; i < outputString.size; i ++) {
    JSData encoded;
    if (!encoded.readstring(outputString[i], commentsOnFailure)) {
      global << logger::red << "Mongo/JSData error: failed to parse JSON. " << logger::endL;
      return false;
    }
    if (!Database::ConvertJSONMongoToJSON(encoded, output[i], commentsOnFailure)) {
      global << logger::red << "Mongo/JSData error: failed to convert mongo JSON to JSON. " << logger::endL;
      return false;
    }
  }
  return true;
}
#endif

std::string Database::Mongo::ConvertErrorToString(void* bson_error_t_pointer) {
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

bool Database::Mongo::DeleteDatabase(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("Database::Mongo::DeleteDatabase");
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
      *commentsOnFailure << "Failed to drop database. " << this->ConvertErrorToString(&errorResult);
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

void Database::Mongo::CreateHashIndex(
  const std::string& collectionName, const std::string& theKey
) {
  (void) collectionName;
  (void) theKey;
  #ifdef MACRO_use_MongoDB
  MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::CreateHashIndex");
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

bool Database::FindFromString(
  const std::string& collectionName,
  const std::string& findQuery,
  List<JSData>& output,
  int maxOutputItems,
  long long* totalItems,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::FindFromString");
#ifdef MACRO_use_MongoDB
  JSData theData;
  //global << logger::blue << "Query input: " << findQuery << logger::endL;
  if (!theData.readstring(findQuery, commentsOnFailure)) {
    return false;
  }
  return Database::get().FindFromJSON(
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

void QueryResultOptions::MakeProjection(const List<std::string>& fields) {
  this->fieldsToProjectTo = fields;
}

JSData QueryResultOptions::ToJSON() const {
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

bool Database::FindFromJSONWithProjection(
  const std::string& collectionName,
  const JSData& findQuery,
  List<JSData>& output,
  List<std::string>& fieldsToProjectTo,
  int maxOutputItems,
  long long* totalItems,
  std::stringstream* commentsOnFailure
) {
  QueryResultOptions options;
  options.MakeProjection(fieldsToProjectTo);
  return Database::FindFromJSONWithOptions(
    collectionName,
    findQuery,
    output,
    options,
    maxOutputItems,
    totalItems,
    commentsOnFailure
  );
}

bool Database::FindFromJSON(
  const std::string& collectionName,
  const JSData& findQuery,
  List<JSData>& output,
  int maxOutputItems,
  long long* totalItems,
  std::stringstream* commentsOnFailure
) {
  QueryResultOptions options;
  return Database::FindFromJSONWithOptions(
    collectionName,
    findQuery,
    output,
    options,
    maxOutputItems,
    totalItems,
    commentsOnFailure
  );
}

bool Database::FindFromJSONWithOptions(
  const std::string& collectionName,
  const JSData& findQuery,
  List<JSData>& output,
  const QueryResultOptions& options,
  int maxOutputItems,
  long long* totalItems,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive
) {
  MacroRegisterFunctionWithName("Database::FindFromJSONWithOptions");
  if (global.flagDisableDatabaseLogEveryoneAsAdmin) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Database::FindFromJSONWithOptions fail. " << DatabaseStrings::errorDatabaseDisableD;
    }
    return false;
  }
  (void) commentsGeneralNonSensitive;
#ifdef MACRO_use_MongoDB
  MongoQuery query;
  query.collectionName = collectionName;
  query.findQuery = findQuery.toString(nullptr);
  query.maxOutputItems = maxOutputItems;
  global << logger::blue << "Query input JSON: " << query.ToStringDebug()
  << ", options: " << options.ToJSON().toString()
  << logger::endL;
  bool result = query.FindMultiple(
    output, options.ToJSON(), commentsOnFailure, commentsGeneralNonSensitive
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
    *commentsOnFailure << "FindFromJSONWithOptions: project compiled without mongoDB support. ";
  }
  return false;
#endif
}

bool Database::Mongo::FindOneFromSome(
  const List<QueryExact>& findOrQueries,
  JSData& output,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::FindOneFromSome");
  #ifdef MACRO_use_MongoDB
  MongoQuery query;
  if (!Database::Mongo::GetOrFindQuery(findOrQueries, query.findQuery, commentsOnFailure)) {
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
  return query.FindOne(output,emptyOptions, commentsOnFailure, nullptr);
  #else
  (void) output;
  (void) findOrQueries;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "MongoDB not compiled in where it should be. ";
  }
  return false;
  #endif
}

bool Database::Mongo::GetOrFindQuery(
  const List<QueryExact>& input,
  std::string& output,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::GetOrFindQuery");
  (void) commentsOnFailure;
  std::stringstream queryStream;
  queryStream << "{\"$or\": [";
  for (int i = 0; i < input.size; i ++) {
    queryStream << input[i].ToJSON().toString(nullptr);
    if (i < input.size - 1) {
      queryStream << ", ";
    }
  }
  queryStream << "]}";
  output = queryStream.str();
  return true;
}

bool Database::Mongo::FindOneWithOptions(
  const QueryExact& query,
  const QueryResultOptions& options,
  JSData& output,
  std::stringstream* commentsOnFailure,
  std::stringstream* commentsGeneralNonSensitive
) {
#ifdef MACRO_use_MongoDB
  MongoQuery mongoQuery;
  mongoQuery.collectionName = query.collection;
  mongoQuery.findQuery = query.ToJSON().toString();
  mongoQuery.maxOutputItems = 1;
  List<JSData> outputList;
  mongoQuery.FindMultiple(
    outputList, options.ToJSON(), commentsOnFailure, commentsGeneralNonSensitive
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
    *commentsOnFailure << "Database::Mongo::FindOneWithOptions failed. No mongoDB support. ";
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
    *commentsOnFailure << "Labels: " << theLabels.ToStringCommaDelimited()
    << " do not match any modifiable field pattern. ";
  }
  return false;
}

bool Database::getLabels(
  const JSData& fieldEntries, List<std::string>& output, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::getLabels");
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

bool Database::isDeleteable(
  const JSData& theEntry, List<std::string>** outputPattern, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::isDeleteable");
  if (theEntry.theType != JSData::token::tokenObject || !theEntry.HasKey(DatabaseStrings::labelFields)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "The labels json is required to be an object of the form {fields: [tableName, objectId,...]}. ";
    }
    return false;
  }
  List<std::string> theLabels;
  if (!Database::getLabels(
    theEntry.objects.GetValueConstCrashIfNotPresent("fields"), theLabels, commentsOnFailure
  )) {
    return false;
  }
  return Database::isDeleteable(theLabels, outputPattern, commentsOnFailure);
}

bool Database::DeleteOneEntry(const JSData& theEntry, std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("Database::DeleteOneEntry");
  if (!global.UserDefaultHasAdminRights()) {
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
    return this->DeleteOneEntryById(findQuery, commentsOnFailure);
  }
  return Database::DeleteOneEntryUnsetUnsecure(
    findQuery, theLabels, commentsOnFailure
  );
}

bool Database::DeleteOneEntryById(
  const QueryExact& findQuery, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::DeleteOneEntryById");
#ifdef MACRO_use_MongoDB
  JSData foundItem;
  if (!this->FindOne(findQuery, foundItem, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Query: "
      << findQuery.ToJSON().toString(nullptr) << " returned no hits in table: "
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
  query.findQuery = findQuery.ToJSON().toString(nullptr);
  query.collectionName = findQuery.collection;
  return query.RemoveOne(commentsOnFailure);
#else
  (void) findQuery;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "DeleteOneEntryById: project compiled without mongoDB support. ";
  }
  return false;
#endif

}

bool Database::DeleteOneEntryUnsetUnsecure(
  const QueryExact& findQuery,
  List<std::string>& selector,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::DeleteOneEntryUnsetUnsecure");
#ifdef MACRO_use_MongoDB

  std::string selectorString = QueryExact::getLabelFromNestedLabels(selector);
  JSData foundItem;
  QueryResultOptions options;
  options.fieldsToProjectTo.AddListOnTop(selector);
  options.fieldsToProjectTo.AddOnTop(selectorString);
  bool didFindItem = Database::get().FindOneWithOptions(
    findQuery,
    options,
    foundItem,
    commentsOnFailure,
    nullptr
  );

  if (!didFindItem) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Query: " << findQuery.ToJSON().toString(nullptr)
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
  query.findQuery = findQuery.ToJSON().toString(nullptr);
  query.collectionName = findQuery.collection;
  std::stringstream updateQueryStream;
  updateQueryStream << "{\"$unset\": {\"" << selectorString << "\":\"\"}}";
  query.updateQuery = updateQueryStream.str();
  return query.UpdateOneNoOptions(commentsOnFailure);
#else
  (void) findQuery;
  (void) selector;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "DeleteOneEntryUnsetUnsecure: project compiled without mongoDB support. ";
  }
  return false;
#endif
}

bool Database::Mongo::UpdateOneFromQueryString(
  const std::string& collectionName,
  const std::string& findQuery,
  const std::string& updateQuery,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::UpdateOneFromQueryString");
#ifdef MACRO_use_MongoDB
  MongoQuery query;
  query.findQuery = findQuery;
  query.collectionName = collectionName;
  query.updateQuery = updateQuery;
  return query.UpdateOneWithOptions(commentsOnFailure);
#else
  (void) collectionName;
  (void) findQuery;
  (void) updateQuery;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "UpdateOneFromQueryString failed: DB not compiled.";
  }
  return false;
#endif
}

bool Database::UpdateOneFromSome(
  const List<QueryExact>& findOrQueries,
  const QuerySet& updateQuery,
  std::stringstream* commentsOnFailure
) {
  if (global.flagDatabaseCompiled) {
    return this->mongoDB.UpdateOneFromSome(findOrQueries, updateQuery, commentsOnFailure);
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "Database::UpdateOneFromSome not implemented yet. ";
  }
  return false;
}

bool Database::Mongo::UpdateOneFromSome(
  const List<QueryExact>& findOrQueries,
  const QuerySet& updateQuery,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::UpdateOneFromSomeJSON");
  std::string queryString;
  if (!this->GetOrFindQuery(findOrQueries, queryString, commentsOnFailure)) {
    return false;
  }
  if (findOrQueries.size == 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Empty or-queries not allowed. ";
    }
    return false;
  }
  JSData setJSON;
  if (!updateQuery.ToJSONSetMongo(setJSON, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to extract set mongo json. ";
    }
    return false;
  }
  return this->UpdateOneFromQueryString(
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

std::string QuerySet::ToStringDebug() const {
  std::stringstream out;
  JSData jsonSetMongo;
  if (!this->ToJSONSetMongo(jsonSetMongo, &out)) {
    return out.str();
  }
  out << jsonSetMongo.toString(nullptr);
  return out.str();
}

bool QuerySet::ToJSONSetMongo(JSData& output, std::stringstream* commentsOnFailure) const {
  JSData data;
  if (!this->ToJSONMongo(data, commentsOnFailure)) {
    return false;
  }
  output.reset(JSData::token::token::tokenObject);
  output["$set"] = data;
  return true;
}

bool QuerySet::ToJSONMongo(
  JSData& output, std::stringstream* commentsOnFailure
) const {
  JSData converted;
  if (!Database::ConvertJSONToJSONMongo(this->value, converted, commentsOnFailure)) {
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
    output[keyPrefix + converted.objects.theKeys[i]] = converted.objects.theValues[i];
  }
  return true;
}

bool Database::Mongo::UpdateOne(
  const QueryExact& findQuery,
  const QuerySet& updateQuery,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::UpdateOneFromJSON");
  JSData updateQueryJSON;
  if (!updateQuery.ToJSONSetMongo(updateQueryJSON, commentsOnFailure)) {
    return false;
  }
  if (!Database::Mongo::UpdateOneFromQueryString(
    findQuery.collection,
    findQuery.ToJSON().toString(nullptr),
    updateQueryJSON.toString(nullptr),
    commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to update one element. ";
    }
    global << "Failed to update element found by: "
    << findQuery.ToJSON().toString(nullptr)
    << " with: " << updateQuery.ToStringDebug();
  }
  return true;
}

bool Database::User::LoadUserInfo(
  UserCalculatorData& output, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::LoadUserInfo");
  if (output.username == "" && output.email == "") {
    return false;
  }
  JSData userEntry;
  if (!this->owner->FindOneFromSome(
    output.GetFindMeFromUserNameQuery(),
    userEntry,
    commentsOnFailure
  )) {
    return false;
  }
  return output.LoadFromJSON(userEntry);
}

bool Database::FetchCollectionNames(
  List<std::string>& output, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Database::FetchCollectionNames");
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
    return Database::get().mongoDB.FetchCollectionNames(output, commentsOnFailure);
  }
  return this->theFallBack.FetchCollectionNames(output, commentsOnFailure);
}

bool Database::Mongo::FetchCollectionNames(
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
  output.SetSize(0);
  if (theCollectionChars != nullptr) {
    for (int i = 0; theCollectionChars[i]; i ++) {
      std::string currentCollection(theCollectionChars[i]);
      output.AddOnTop(currentCollection);
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
  Database::get().FindFromJSON(
    tableName, findQuery, rowsJSON, 200, totalItems, commentsOnFailure
  );
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
        outputRows[i][j] = rowsJSON[i].GetValue(theLabels[j]).toString(nullptr);
      } else {
        outputRows[i][j] = "";
      }
    }
  }
  return true;
}

JSData Database::ToJSONDatabaseFetch(const std::string& incomingLabels) {
  MacroRegisterFunctionWithName("Database::ToJSONDatabaseFetch");
  JSData result;
  JSData labels;
  std::stringstream commentsOnFailure;
  if (!labels.readstring(incomingLabels, &commentsOnFailure)) {
    commentsOnFailure << "Failed to parse labels from: "
    << StringRoutines::StringTrimToLengthForDisplay(incomingLabels, 100);
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
    return this->ToJSONDatabaseCollection("");
  }
  if (labelStrings.size == 1) {
    return this->ToJSONDatabaseCollection(labelStrings[0]);
  }
  return Database::ToJSONFetchItem(labelStrings);
}

JSData Database::ToJSONFetchItem(const List<std::string>& labelStrings) {
  MacroRegisterFunctionWithName("Database::ToJSONFetchItem");
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
  labelStrings.Slice(2, labelStrings.size - 2,  projector.fieldsToProjectTo);
  List<JSData> rowsJSON;
  long long totalItems = 0;
  std::stringstream comments;
  std::stringstream* commentsPointer = nullptr;
  bool flagDebuggingAdmin = global.UserDefaultIsDebuggingAdmin();
  if (flagDebuggingAdmin) {
    commentsPointer = &comments;
  }
  if (!Database::FindFromJSONWithOptions(
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

JSData Database::ToJSONDatabaseCollection(const std::string& currentTable) {
  MacroRegisterFunctionWithName("Database::ToJSONDatabaseCollection");
  JSData result;
  std::stringstream out;
  result["currentTable"] = currentTable;
  if (currentTable == "") {
    if (global.UserDebugFlagOn() != 0) {
      result[WebAPI::result::comments] = "Requested table empty, returning list of tables. ";
    }
    List<std::string> theCollectionNames;
    if (Database::FetchCollectionNames(theCollectionNames, &out)) {
      JSData collectionNames;
      collectionNames.theType = JSData::token::tokenArray;
      collectionNames.theList.SetSize(theCollectionNames.size);
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
  projector = Database::GetStandardProjectors(currentTable);
  findQuery.theType = JSData::token::tokenObject;
  List<JSData> rowsJSON;
  long long totalItems = 0;
  std::stringstream comments;
  std::stringstream* commentsPointer = nullptr;
  bool flagDebuggingAdmin = global.UserDefaultIsDebuggingAdmin();
  if (flagDebuggingAdmin) {
    commentsPointer = &comments;
  }
  if (!Database::FindFromJSONWithOptions(
    currentTable, findQuery, rowsJSON, projector, 200, &totalItems, &out, commentsPointer
  )) {
    result["error"] = out.str();
    return result;
  }
  if (rowsJSON.size == 0) {
    result = Database::ToJSONDatabaseCollection("");
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

std::string Database::ToHtmlDatabaseCollection(const std::string& currentTable) {
  MacroRegisterFunctionWithName("Database::ToStringDatabaseCollection");
  std::stringstream out;
  if (currentTable == "") {
    List<std::string> theCollectionNames;
    if (this->FetchCollectionNames(theCollectionNames, &out)) {
      out << "There are " << theCollectionNames.size << " collections. ";
      for (int i = 0; i < theCollectionNames.size; i ++) {
        out << "<br>";
        out << "<a href=\"" << global.DisplayNameExecutable
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
  out << "<br>" << HtmlRoutines::ToHtmlTable(theLabels, theRows, true);
  return out.str();
}

