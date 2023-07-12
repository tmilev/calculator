
#ifndef header_database_ALREADY_INCLUDED
#define header_database_ALREADY_INCLUDED

#include "general_database_system_independent.h"
#include "multiprocessing.h"

class UserCalculator;
class Database;

// Forward declare a class that can listen to sockets.
class Listener;

class QuerySetOnce {
public:
  List<std::string> nestedLabels;
  JSData value;
  JSData toJSON() const;
  bool fromJSON(const JSData& data, std::stringstream* commentsOnStream);
  void updateOneEntry(JSData& toBeModified) const;
};

// Stores a recipe for updating an item.
// A typical item would be stored allong the lines of:
// {
//   "collection1": [
//     {
//       key1: "id1",
//       key2: {
//         subkey1: "XX",
//         subkey2: "YY"
//       }
//     },
//     {
//       key1: "id2",
//       key2: {
//         subkey1: "XX",
//         subkey2: "ZZ"
//       }
//     },
//   ]
// }
class QuerySet {
private:
public:
  List<QuerySetOnce> data;
  // Adds a single string key with a single string value.
  void addKeyValueStringPair(
    const std::string& key, const std::string& value
  );
  void addKeyValuePair(const std::string& key, const JSData& value);
  void addNestedKeyValuePair(
    const List<std::string>& nestedKeys, const JSData& value
  );
  void addValue(const JSData& value);
  QuerySet();
  bool fromJSON(
    const JSData& inputValue, std::stringstream* commentsOnFailure
  );
  bool fromJSONString(
    const std::string& inputValue, std::stringstream* commentsOnFailure
  );
  void fromJSONStringNoFail(const std::string& inputValue);
  void fromJSONNoFail(const JSData& inputValue);
  JSData toJSON() const;
  std::string toStringDebug() const;
  void mergeData(JSData& toMergeInto) const;
  class Test {
  public:
    static bool all();
    static bool basics(DatabaseType databaseType);
    static void updateNoFail(QueryExact& find, QuerySet updater);
    static void findNoFail(QueryExact& find, JSData& result);
    static void matchKeyValue(
      const JSData& mustContain, const JSData& mustBeContained
    );
  };
};

class QueryResultOptions {
public:
  List<std::string> fieldsToProjectTo;
  List<std::string> fieldsProjectedAway;
  JSData toJSON() const;
  void makeProjection(const List<std::string>& fields);
};

class DatabaseFallback {
private:
  bool initialized;
  bool updateOneNolocks(
    const QueryExact& findQuery,
    const QuerySet& updateQuery,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool updateOneEntry(
    JSData& modified,
    const List<std::string>& labels,
    const JSData& value,
    std::stringstream* commentsOnFailure = nullptr
  );
public:
  Database* owner;
  MutexProcess access;
  HashedList<std::string> knownCollections;
  HashedList<std::string> knownIndices;
  JSData databaseContent;
  class Index {
  public:
    // Collection A, label B is denoted as A.B.
    // Dots in the labels and collections are forbidden.
    std::string collection;
    std::string label;
    std::string collectionAndLabelCache;
    MapList<
      std::string,
      List<int32_t>,
      HashFunctions::hashFunction<std::string>
    > locations;
    static std::string collectionAndLabelStatic(
      const std::string& inputCollection, const std::string& inputLabel
    );
    std::string collectionAndLabel();
  };

  MapReferences<
    std::string,
    DatabaseFallback::Index,
    HashFunctions::hashFunction<std::string>
  > indices;
  static std::string jsonLocation();
  bool deleteDatabase(std::stringstream* commentsOnFailure);
  bool updateOne(
    const QueryExact& findQuery,
    const QuerySet& updateQuery,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool updateOneFromSome(
    const List<QueryExact>& findOrQueries,
    const QuerySet& updateQuery,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool findOne(
    const QueryExact& query,
    JSData& output,
    std::stringstream* commentsOnFailure
  );
  bool findOneWithOptions(
    const QueryExact& query,
    const QueryResultOptions& options,
    JSData& output,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNonSensitive = nullptr
  );
  // Return indicates query success / failure.
  // When the element isn't found but otherwise there were
  // no problems with the query, true will be returned with
  // output set to [].
  bool findIndexOneNolocksMinusOneNotFound(
    const QueryExact& query,
    int& output,
    std::stringstream* commentsOnNotFound
  );
  bool fetchCollectionNames(
    List<std::string>& output, std::stringstream* commentsOnFailure
  );
  void createHashIndex(
    const std::string& collectionName, const std::string& key
  );
  bool hasCollection(
    const std::string& collection, std::stringstream* commentsOnFailure
  );
  bool storeDatabase(std::stringstream* commentsOnFailure);
  bool readDatabase(std::stringstream* commentsOnFailure);
  bool readAndIndexDatabase(std::stringstream* commentsOnFailure);
  bool readAndIndexDatabaseWithLockGuard(
    std::stringstream* commentsOnFailure
  );
  bool indexDatabase(std::stringstream* commentsOnFailure);
  const JSData& getFullCollection(const std::string& collection);
  void indexOneRecord(
    const JSData& entry, int32_t row, const std::string& collection
  );
  void initialize();
  bool findOneFromSome(
    const List<QueryExact>& findOrQueries,
    JSData& output,
    std::stringstream* commentsOnFailure
  );
  bool findFromJSONWithOptions(
    const QueryExact& findQuery,
    List<JSData>& output,
    const QueryResultOptions& options,
    int maximumOutputItems,
    long long* totalItems,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNonSensitive
  );
  std::string toStringIndices() const;
  DatabaseFallback();
};

// Stores a query for an item.
// A typical item would be stored allong the lines of:
// {
//   "collection1": [
//     {
//       key1: "id1",
//       key2: {
//         subkey1: "XX",
//         subkey2: "YY"
//       }
//     },
//     {
//       key1: "id2",
//       key2: {
//         subkey1: "XX",
//         subkey2: "ZZ"
//       }
//     },
//   ]
// }
// Example 1.
// Item {key1:"id1", key2: {subkey1: "XX", subkey2: "YY"}} is selected with:
// collection = "collection1"
// nestedLabels = ["key1"]
// value = "id1"
//
// Example 2.
// Item {key1:"id1", key2: {subkey1: "XX", subkey2: "YY"}} can also be selected
// with:
// collection = "collection1"
// nestedLabels = ["key2", "subkey2"]
// value = "YY"
class QueryExact {
public:
  std::string collection;
  // Labels.
  List<std::string> nestedLabels;
  // Exact value we are looking for.
  JSData exactValue;
  bool selectAny;
  QueryExact();
  QueryExact(
    const std::string& desiredCollection,
    const std::string& label,
    const std::string& desiredValue
  );
  QueryExact(
    const std::string& desiredCollection,
    const List<std::string>& desiredLabels,
    const std::string& desiredValue
  );
  // Concatenates the nested labels with a comma separator.
  std::string concatenateNestedLabels() const;
  void setLabelValue(
    const std::string& label, const std::string& desiredValue
  );
  void setLabelsValue(
    const List<std::string>& labels, const std::string& desiredValue
  );
  JSData toJSON() const;
  // Returns a combination of the labels and value into JSON format,
  // ignoring the collection name.
  // In the example of
  // nestedLabels = ["key1", "subkey2"]
  // value = {subkey3: "XX"},
  // this will return {"key1": {subkey2: {subkey3: "XX"}}}
  JSData toJSONCombineKeysAndValue() const;
  bool fromString(
    const std::string& input, std::stringstream* commentsOnFailure
  );
  bool fromJSON(
    const JSData& source, std::stringstream* commentsOnFailure
  );
  std::string toString() const;
  bool isEmpty() const;
};

class QueryFindAndUpdate {
public:
  QuerySet update;
  QueryExact find;
  bool createIfNotFound;
  JSData toJSON() const;
  bool fromJSON(JSData& input, std::stringstream* commentsOnFailure);
  QueryFindAndUpdate();
};

class DatabaseInternalConnection {
public:
  int indexInOwner;
  PipePrimitive clientToServer;
  PipePrimitive serverToClient;
  DatabaseInternalConnection();
  void create(int inputIndexInOwner);
};

class DatabaseInternalResult {
  JSData content;
public:
  bool success;
  std::string comments;
  bool fromJSON(
    const std::string& input, std::stringstream* commentsOnFailure
  );
  JSData toJSON();
  bool isSuccess(std::stringstream* commentsOnFalse) const;
  DatabaseInternalResult();
};

class DatabaseInternalRequest {
  JSData contentReader;
  bool fromJSData(std::stringstream* commentsOnFailure);
public:
  enum Type {
    unknown, findAndUpdate,
  };
  Type requestType;
  QueryFindAndUpdate queryFindAndUpdate;
  bool fromJSON(
    const std::string& input, std::stringstream* commentsOnFailure
  );
  void toJSON(JSData& output) const;
  JSData toJSON() const;
  std::string toStringForDebugging() const;
  static std::string typeToString(DatabaseInternalRequest::Type input);
  DatabaseInternalRequest();
};

class DatabaseInternal;

class DatabaseInternalIndex {
public:
  std::string collectionName;
  List<std::string> nestedKeys;
  MapList<std::string, std::string> keyValueToObjectId;
  MapList<std::string, std::string> objectIdToKeyValue;
  JSData toJSON() const;
  bool fromJSON(const JSData& input, std::stringstream* commentsOnFailure);
  std::string toString() const;
  DatabaseInternalIndex();
};

// A table/collection in the database.
class DatabaseCollection {
public:
  // User-defined entries by which objects should be indexed.
  MapReferences<std::string, DatabaseInternalIndex> indices;
  bool indicesLoaded;
  std::string name;
  DatabaseInternal* owner;
  JSData toJSONSchema() const;
  bool loadIndicesFromHardDrive(std::stringstream* commentsOnFailure);
  bool storeIndicesToHardDrive(std::stringstream* commentsOnFailure);
  void toJSONIndices(JSData& output) const;
  std::string fileNameIndex() const;
  DatabaseCollection();
  void initialize(
    const std::string& inputName, DatabaseInternal* inputOwner
  );
};

class DatabaseInternalServer {
public:
  DatabaseInternal* owner;
  MapReferences<std::string, DatabaseCollection> collections;
  DatabaseInternalServer();
  bool initializeLoadFromHardDrive(std::stringstream* commentsOnFailure);
  void storeEverything();
  bool ensureCollection(
    const std::string& collectionName,
    const List<std::string>& indexableKeys
  );
  bool findAndUpdate(
    QueryFindAndUpdate& input, std::stringstream* commentsOnFailure
  );
  void storeCollectionList();
  std::string collectionsSchemaFileName() const;
  bool findOneWithOptions(
    const QueryExact& query,
    const QueryResultOptions& options,
    JSData& output,
    std::stringstream* commentsOnFailure
  );
  bool findObjectId(
    const QueryExact& query,
    std::string& output,
    std::stringstream* commentsOnFailure
  );
  bool createObject(
    const JSData& initialValue,
    const std::string& collectionName,
    std::string& outputObjectId,
    std::stringstream* commentsOnFailure
  );
  bool loadObject(
    const std::string& objectId,
    const std::string& collectionName,
    JSData& output,
    std::stringstream* commentsOnFailure
  );
  bool storeObject(
    const std::string& objectId,
    const std::string& collectionName,
    const JSData& data,
    std::stringstream* commentsOnFailure
  );
  void objectExists(
    const std::string& objectId,
    const std::string& collectionName,
    bool& output,
    bool& outputFailed,
    std::stringstream* commentsOnFailure
  );
  std::string objectFilename(
    const std::string& objectId, const std::string& collectionName
  );
};

class DatabaseInternalClient {
public:
  DatabaseInternal* owner;
  DatabaseInternalClient();
  bool findOneWithOptions(
    const QueryExact& query,
    const QueryResultOptions& options,
    JSData& output,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNonSensitive = nullptr
  );
  bool findOneFromSome(
    const List<QueryExact>& findOrQueries,
    JSData& output,
    std::stringstream* commentsOnFailure
  );
  bool findFromJSONWithOptions(
    const QueryExact& findQuery,
    List<JSData>& output,
    const QueryResultOptions& options,
    int maximumOutputItems,
    long long* totalItems,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNonSensitive
  );
  bool updateOne(
    const QueryExact& findQuery,
    const QuerySet& updateQuery,
    bool createIfNotFound,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool updateOneFromSome(
    const List<QueryExact>& findOrQueries,
    const QuerySet& updateQuery,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool fetchCollectionNames(
    List<std::string>& output, std::stringstream* commentsOnFailure
  );
};

// A self contained
// simple database implementation.
// Forks out into multiple: processes - future client(s) and server.
// Clients and server read/write to/from number of pre-allocated file
// descriptors (currently, linux pipes).
class DatabaseInternal {
  // Map from listening sockets to ports.
  ListReferences<DatabaseInternalConnection> connections;
  MapList<int, int> mapFromReadEndsToWorkerIds;
  List<int> readEnds;
  List<char> buffer;
  bool failedToInitialize;
  std::string initializationError;
  bool sendFromClientToServer(
    const std::string& input, std::stringstream* commentsOnFailure
  );
  bool receiveInClientFromServer(
    DatabaseInternalResult& output, std::stringstream* commentsOnFailure
  );
  PipePrimitive& currentServerToClient();
  PipePrimitive& currentClientToServer();
public:
  int processId;
  int currentWorkerId;
  int maximumMessageSize;
  DatabaseInternalClient client;
  DatabaseInternalServer server;
  bool sendAndReceiveFromClientToServer(
    const DatabaseInternalRequest& input,
    DatabaseInternalResult& output,
    std::stringstream* commentsOnFailure
  );
  static std::string folder();
  bool deleteDatabase(std::stringstream* commentsOnFailure);
  void createHashIndex(
    const std::string& collectionName, const std::string& key
  );
  // Forks out a child process to be the database server.
  // Returns the process id of the database server to the parent
  // and 0 to the child server database process.
  int forkOutDatabase();
  void initializeForkAndRun(int maximumConnections);
  // Runs the main server loop which listens to a number of
  // file descriptors from all database clients.
  void run();
  // Processes a request from a single client.
  // As of writing, this will execute the entire operation, locking out the DB
  // for all other processes. Since we expect the db read/write operations to
  // be extremely quick, we do not expect a performance penalty for the blocked
  // waiting clients.
  // Should that change, please move the handling of client requests to
  // separate thread(s).
  bool runOneConnection();
  // Executes one single operation and sends the result back.
  bool executeAndSend();
  DatabaseInternal();
  std::string toPayLoad(const std::string& input);
};

class UserOfDatabase {
private:
  bool firstLoginOfAdmin(
    UserCalculatorData& incoming,
    UserCalculator& userInDatabase,
    std::stringstream* commentsOnFailure
  );
public:
  Database* owner;
  bool logoutViaDatabase();
  bool loginViaDatabase(
    UserCalculatorData& user, std::stringstream* commentsOnFailure
  );
  bool loginNoDatabaseSupport(
    UserCalculatorData& user, std::stringstream* commentsGeneral
  );
  bool loginViaGoogleTokenCreateNewAccountIfNeeded(
    UserCalculatorData& user,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral,
    bool& tokenIsGood
  );
  bool setPassword(
    const std::string& inputUsername,
    const std::string& inputNewPassword,
    std::string& outputAuthenticationToken,
    std::stringstream& comments
  );
  bool userExists(
    const std::string& inputUsername, std::stringstream& comments
  );
  bool userDefaultHasInstructorRights();
  // TODO(tmilev): refactor down to database-only operations.
  static bool sendActivationEmail(
    const std::string& emailList,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral,
    std::stringstream* commentsGeneralSensitive
  );
  // TODO(tmilev): refactor down to database-only operations.
  static bool sendActivationEmail(
    const List<std::string>& emails,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneral,
    std::stringstream* commentsGeneralSensitive
  );
  // TODO(tmilev): refactor down to database-only operations.
  bool addUsersFromEmails(
    const std::string& emailList,
    const std::string& userPasswords,
    std::string& userRole,
    std::string& userGroup,
    std::stringstream& comments,
    int& outputNumberOfNewUsers,
    int& outputNumberOfUpdatedUsers
  );
  bool loadUserInformation(
    UserCalculatorData& output, std::stringstream* commentsOnFailure
  );
  UserOfDatabase();
};

class Database {
public:
  bool flagInitializedServer;
  bool flagInitializedWorker;
  List<std::string> modifyableColumns;
  int databaseInstanceCountMustBeOneOrZero;
  // Get global database instance.
  // Implemented as function rather than static member to
  // avoid the static initalization order fiasco.
  static Database& get();
  bool initializeServer(int maximumConnections);
  bool initializeWorker();
  bool checkInitialization();
  void createHashIndex(
    const std::string& collectionName, const std::string& key
  );
  static std::string toString();
  UserOfDatabase user;
  // TODO(tmilev): Rename this to fallbackDatabase.
  DatabaseFallback fallbackDatabase;
  DatabaseInternal localDatabase;
  bool findFromJSON(
    const QueryExact& findQuery,
    List<JSData>& output,
    int maxOutputItems = - 1,
    long long* totalItems = nullptr,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool findFromJSONWithProjection(
    const QueryExact& findQuery,
    List<JSData>& output,
    List<std::string>& fieldsToProjectTo,
    int maxOutputItems = - 1,
    long long* totalItems = nullptr,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool findFromJSONWithOptions(
    const QueryExact& findQuery,
    List<JSData>& output,
    const QueryResultOptions& options,
    int maximumOutputItems = - 1,
    long long* totalItems = nullptr,
    std::stringstream* commentsOnFailure = nullptr,
    std::stringstream* commentsGeneralNonSensitive = nullptr
  );
  bool findOneWithOptions(
    const QueryExact& query,
    const QueryResultOptions& options,
    JSData& output,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNonSensitive = nullptr
  );
  bool findOne(
    const QueryExact& query,
    JSData& output,
    std::stringstream* commentsOnFailure
  );
  bool findOneFromSome(
    const List<QueryExact>& alternatives,
    JSData& output,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool updateOne(
    const QueryExact& findQuery,
    const QuerySet& dataToMerge,
    bool createIfNotFound,
    std::stringstream* commentsOnFailure
  );
  bool updateOneFromSome(
    const List<QueryExact>& findOrQueries,
    const QuerySet& updateQuery,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool fetchCollectionNames(
    List<std::string>& output, std::stringstream* commentsOnFailure
  );
  static bool fetchTable(
    const std::string& tableName,
    List<std::string>& outputLabels,
    List<List<std::string> >& outputRows,
    long long* totalItems = nullptr,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool deleteOneEntry(
    const JSData& entry, std::stringstream* commentsOnFailure
  );
  bool deleteOneEntryById(
    const QueryExact& findQuery, std::stringstream* commentsOnFailure
  );
  static bool deleteOneEntryUnsetUnsecure(
    const QueryExact& findQuery,
    List<std::string>& selector,
    std::stringstream* commentsOnFailure
  );
  std::string toHtmlDatabaseCollection(const std::string& currentTable);
  JSData toJSONFetchItem(
    QueryExact& findQuery, QueryResultOptions& projector
  );
  // Redacts object ids and hashes.
  static void correctData(JSData& row);
  static void correctData(List<JSData>& toBeCorrected);
  static void correctDataFromLabels(
    JSData& row, const List<std::string>& labels
  );
  static void correctDataFromLabels(
    JSData& row, const std::string& oneLabel
  );
  JSData toJSONDatabaseCollection(const std::string& currentTable);
  JSData toJSONDatabaseFetch(const std::string& findQueryAndProjector);
  static bool getLabels(
    const JSData& fieldEntries,
    List<std::string>& labels,
    std::stringstream* commentsOnFailure
  );
  static bool isDeleteable(
    const List<std::string>& labels,
    List<std::string>** outputPattern,
    std::stringstream* commentsOnFailure
  );
  static bool isDeleteable(
    const JSData& entry,
    List<std::string>** outputPattern,
    std::stringstream* commentsOnFailure
  );
  static bool matchesPattern(
    const List<std::string>& fieldLabel,
    const List<std::string>& pattern
  );
  static QueryResultOptions getStandardProjectors(
    const std::string& collectionName
  );
  bool deleteDatabase(std::stringstream* commentsOnFailure);
  class Test {
  public:
    StateMaintainer<bool> maintainServerForkFlag;
    StateMaintainer<DatabaseType> maintainerDatabase;
    StateMaintainer<std::string> maintainerDatabaseName;
    static std::string adminPassword;
    static bool all();
    static bool basics(DatabaseType databaseType);
    bool deleteDatabase();
    void initializeForDatabaseOperations();
    bool createAdminAccount();
    Test(DatabaseType databaseType);
  };

  Database();
  ~Database();
};

#endif // header_database_ALREADY_INCLUDED
