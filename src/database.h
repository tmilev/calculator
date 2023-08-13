
#ifndef header_database_ALREADY_INCLUDED
#define header_database_ALREADY_INCLUDED

#include "general_database_system_independent.h"
#include "multiprocessing.h"
#include "general_logging_global_variables.h"
#include "math_large_integers.h"

class UserCalculator;
class Database;

// Forward declare a class that can listen to sockets.
class Listener;
class QueryExact;

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
  bool fromJSON(const JSData& input, std::stringstream* commentsOnFailure);
  bool isNonTrivial() const;
  void makeProjection(const List<std::string>& fields);
  void applyProjection(const JSData& input, JSData& output) const;
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
  bool extractNestedKeysFromJSON(
    const JSData& input, std::stringstream* commentsOnFailure
  );
public:
  std::string collection;
  // Labels.
  List<std::string> nestedLabels;
  // Exact value we are looking for.
  JSData exactValue;
  // Maximum number of elements to select.
  int maximumNumberOfItems;
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

class QueryOneOfExactly {
public:
  List<QueryExact> queries;
  bool fromJSON(const JSData& input, std::stringstream* commentsOnFailure);
  JSData toJSON() const;
  std::string collection() const;
  QueryOneOfExactly();
  QueryOneOfExactly(const QueryExact& query);
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
  JSData reader;
public:
  List<JSData> content;
  bool success;
  std::string comments;
  // Used when an output message is too large.
  // When that happens, the message bytes will contain a json in which
  // success will be set to true,
  // and the messageId will be set to the present value.
  // The message id does not expire for the duration of the server process.
  LargeInteger messageId;
  // Location of the message in the message buffer.
  // May be expired if multiple messageIds are stored.
  int messageHandle;
  // Total message size.
  int messageSize;
  // A chunk of a raw large message, to be sent next.
  std::string nextChunk;
  int bytesSentSoFar;
  bool fromJSON(
    const std::string& input, std::stringstream* commentsOnFailure
  );
  JSData toJSON();
  void writeContent(List<JSData>& output);
  bool isSuccess(std::stringstream* commentsOnFalse) const;
  DatabaseInternalResult();
};

class DatabaseInternalRequest {
  JSData contentReader;
  bool fromJSData(std::stringstream* commentsOnFailure);
public:
  enum Type {
    unknown,
    findAndUpdate,
    find,
    fetchCollectionNames,
    shutdown,
    getLargeMessage,
  };
  Type requestType;
  QueryFindAndUpdate queryFindAndUpdate;
  QueryOneOfExactly queryOneOfExactly;
  QueryResultOptions options;
  // Used to fetch the result of a previously executed query
  // that was too large to receive in one go. Holds a process-unique id of the
  // message.
  LargeInteger messageId;
  // Holds teh location of the stored message. May be expired.
  int messageHandle;
  bool fromJSON(
    const std::string& input, std::stringstream* commentsOnFailure
  );
  void toJSON(JSData& output) const;
  JSData toJSON() const;
  std::string toStringForDebugging() const;
  static std::string typeToString(DatabaseInternalRequest::Type input);
  static Type stringToType(const std::string& input);
  DatabaseInternalRequest();
};

class DatabaseInternal;

class DatabaseInternalIndex {
public:
  std::string collectionName;
  List<std::string> nestedKeys;
  MapList<std::string, HashedList<std::string> > keyValueToObjectIds;
  MapList<std::string, std::string> objectIdToKeyValue;
  JSData toJSON() const;
  bool fromJSON(const JSData& input, const std::string& collectionName);
  std::string toString() const;
  // Sets an object id to a given value.
  void setObjectIdValue(
    const std::string& objectId,
    const std::string& newValue,
    bool& outputValueChanged
  );
  void removeObjectId(
    const std::string& objectId, const std::string& previousValue
  );
  DatabaseInternalIndex();
  void computeKeyValueToObjectIds();
};

// A table/collection in the database.
class DatabaseCollection {
public:
  // User-defined entries by which objects should be indexed.
  MapReferences<std::string, DatabaseInternalIndex> indices;
  bool indicesLoaded;
  std::string name;
  // A map from the object ids to the records that are loaded in RAM.
  // This is empty at the start and is loaded on demand.
  MapList<std::string, JSData> loadedRecords;
  DatabaseInternal* owner;
  JSData toJSONSchema() const;
  bool loadIndicesFromHardDrive(bool clearPreExistingIndices);
  bool storeIndicesToHardDrive(std::stringstream* commentsOnFailure);
  void toJSONIndices(JSData& output) const;
  JSData toJSONIndices() const;
  void toJSON(JSData& result) const;
  std::string toStringIndices() const;
  std::string fileNameIndex() const;
  DatabaseInternalIndex& indexOfObjectIds();
  DatabaseCollection();
  void initialize(
    const std::string& inputName, DatabaseInternal* inputOwner
  );
  bool updateObjectInIndexReturnTrueIfChanged(
    const std::string& objectId, const JSData& data
  );
};

class DatabaseInternalServer {
public:
  DatabaseInternal* owner;
  MapReferences<std::string, DatabaseCollection> collections;
  DatabaseInternalServer();
  bool initializeLoadFromHardDrive();
  void ensureStandardCollectionIndices();
  bool ensureCollection(
    const std::string& collectionName,
    const List<std::string>& indexableKeys
  );
  bool findAndUpdate(
    QueryFindAndUpdate& input, std::stringstream* commentsOnFailure
  );
  std::string collectionsSchemaFileName() const;
  bool shutdown();
  bool find(
    const QueryOneOfExactly& query,
    const QueryResultOptions* options,
    List<JSData>& output,
    std::stringstream* commentsOnFailure
  );
  bool findObjectIds(
    const QueryExact& query,
    List<std::string>& output,
    std::stringstream* commentsOnFailure
  );
  bool fetchCollectionNames(
    List<JSData>& output, std::stringstream* commentsOnFailure
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
    bool storeUpdatedIndexToHardDrive,
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
  std::string toStringIndices() const;
  // Converts the entire database to JSON.
  JSData toJSONDatabase();
  bool writeDatabase();
};

class DatabaseInternalClient {
public:
  DatabaseInternal* owner;
  DatabaseInternalClient();
  bool find(
    const QueryOneOfExactly& query,
    const QueryResultOptions* options,
    List<JSData>& output,
    std::stringstream* commentsOnFailure
  );
  bool updateOne(
    const QueryExact& findQuery,
    const QuerySet& updateQuery,
    bool createIfNotFound,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool fetchCollectionNames(
    List<std::string>& output, std::stringstream* commentsOnFailure
  );
  bool checkInitialization();
  // Shuts the database down gracefully.
  // Do not use in SIGHUP and other abnormal shutdowns: this function
  // must not run on the server process.
  bool shutdown(std::stringstream* commentsOnFailure);
};

class DatabaseLargeMessage {
public:
  LargeInteger messageId;
  std::string content;
  int bytesSent;
};

// A self contained simple database.
// Forks out into multiple processes - future client(s) and server.
// Clients and server communicate through pre-allocated file
// descriptors (currently, linux pipes).
class DatabaseInternal {
  // Map from listening sockets to ports.
  ListReferences<DatabaseInternalConnection> connections;
  MapList<int, int> mapFromReadEndsToWorkerIds;
  List<int> readEnds;
  List<char> buffer;
  MapReferences<int, DatabaseLargeMessage> largeMessages;
  LargeInteger largeMessageCount;
  bool flagFailedToInitialize;
  // An interprocess mutex to be used in fallback mode, so that
  // different clients can read/write from the database
  // with data races. The mutex will only be initialized in fallback mode.
  MutexProcess mutexFallbackDatabase;
  List<std::string> initializationErrors;
  bool sendFromClientToServer(
    const std::string& input, std::stringstream* commentsOnFailure
  );
  bool receiveInClientFromServer(
    std::string& output, std::stringstream* commentsOnFailure
  );
  PipePrimitive& currentServerToClient();
  PipePrimitive& currentClientToServer();
public:
  int processId;
  int currentWorkerId;
  int maximumMessageSize;
  // 1. When this is false, the client and server are
  // in different processes and communicate through a pipe
  // which syncronizes concurrent database calls.
  // 2. When this is true, the client and the server are
  // in the same process. The database reads and writes
  // use an inter-process mutex to lock the database
  // for exclusive use to prevent data races.
  bool flagIsFallback;
  bool flagIsRunning;
  DatabaseInternalClient client;
  DatabaseInternalServer server;
  void accountInitializationError(const std::string& error);
  bool sendAndReceiveFromClientFull(
    const DatabaseInternalRequest& input,
    DatabaseInternalResult& output,
    std::stringstream* commentsOnFailure
  );
  bool sendAndReceiveFromClientInternal(
    const std::string& input,
    std::string& output,
    std::stringstream* commentsOnFailure
  );
  // Sends data from the client process to
  // the database server process.
  // The data sent here will be received by
  // DatabaseInternal::runOneConnection.
  bool sendAndReceiveFromClientToServerThroughPipe(
    const std::string& input,
    std::string& output,
    std::stringstream* commentsOnFailure
  );
  // Sends data from the server process to the client process.
  bool sendFromServerToClient(const std::string& message);
  // A fallback method where all server
  // computation is carried in
  // the current client process.
  // All other client processes are locked away
  // with a process mutex.
  bool sendAndReceiveFromClientToServerFallbackWithProcessMutex(
    const std::string& input, std::string& output
  );
  static std::string folder();
  // Forks out a child process to be the database server.
  // Returns the process id of the database server to the parent
  // and 0 to the child server database process.
  int forkOutDatabase();
  void initializeCommon();
  void initializeAsFallback();
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
  void executeGetResult(DatabaseInternalResult& output);
  void fetchLargeMessage(
    DatabaseInternalRequest& input, DatabaseInternalResult& output
  );
  void executeGetString(std::string& output);
  bool executeAndSend();
  // Used to shut down the database process when the parent
  // process dies or the process receives a sigint.
  void shutdownUnexpectedly();
  static void shutdownUnexpectedlyStatic(int unused);
  DatabaseInternal();
  std::string toStringInitializationErrors() const;
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
    int& outputNumberOfNewUsers,
    int& outputNumberOfUpdatedUsers,
    std::stringstream* comments
  );
  bool loadUserInformation(
    UserCalculatorData& output, std::stringstream* commentsOnFailure
  );
  UserOfDatabase();
  bool addOneUser(
    const std::string& userNameOrEmail,
    const std::string& password,
    std::string& userRole,
    std::string& userGroup,
    int& outputNumberOfNewUsers,
    int& outputNumberOfUpdatedUsers,
    std::stringstream* commentsOnFailure
  );
};

class Database {
public:
  bool flagInitializedServer;
  bool flagInitializedWorker;
  static std::string name;
  List<std::string> modifyableColumns;
  int databaseInstanceCountMustBeOneOrZero;
  // Get global database instance.
  // Implemented as function rather than static member to
  // avoid the static initalization order fiasco.
  bool initialize(int maximumConnections);
  bool checkInitialization();
  static Database& get();
  static std::string toString();
  UserOfDatabase user;
  // TODO(tmilev): Rename this to fallbackDatabase.
  DatabaseInternal fallbackDatabase;
  DatabaseInternal localDatabase;
  // Finds the first object(s) up to a limit
  // that satisfy the query.
  // Returns false if there's a database error.
  bool find(
    const QueryOneOfExactly& query,
    const QueryResultOptions* options,
    List<JSData>& output,
    std::stringstream* commentsOnFailure
  );
  // Finds exactly one item that matches the query.
  // Will return false both on a database error and when
  // the number of items found is not exactly 1.
  bool findExactlyOne(
    const QueryOneOfExactly& query,
    const QueryResultOptions* options,
    JSData& output,
    std::stringstream* commentsOnFailure
  );
  bool updateOne(
    const QueryExact& findQuery,
    const QuerySet& dataToMerge,
    bool createIfNotFound,
    std::stringstream* commentsOnFailure
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
  JSData toJSONAllCollections();
  JSData toJSONDatabaseFetch(const std::string& findQuery);
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
  bool shutdown(std::stringstream* commentsOnFailure);
  class Test {
  public:
    StateMaintainer<bool> maintainServerForkFlag;
    StateMaintainer<DatabaseType> maintainerDatabase;
    StateMaintainer<std::string> maintainerDatabaseName;
    DatabaseType databaseType;
    static std::string adminPassword;
    // A special test that does not shutdown the database correctly.
    // This test is only allowed to run once per test executable run.
    // It test what happens when the parent process exits early,
    // without shutting the database down.
    static bool noShutdownSignal();
    static bool all();
    static bool basics(DatabaseType databaseType);
    static bool findWithOptions(DatabaseType databaseType);
    static bool loadFromJSON();
    bool deleteDatabase();
    static bool createAdminAccount();
    Test(DatabaseType inputDatabaseType);
    ~Test();
    static void startDatabase(DatabaseType databaseType);
    static std::string testDatabaseName(DatabaseType databaseType);
  };

  Database();
  ~Database();
};

class DatabaseLoader {
public:
  std::string fileName;
  static bool writeBackup();
  static bool loadDatabase(
    const std::string& databaseName, std::stringstream& comments
  );
  bool doLoadDatabase(
    const std::string& databaseName, std::stringstream& comments
  );
  bool loadFromJSON(JSData& input, std::stringstream& comments);
  bool loadOneCollectionFromJSON(
    const std::string& collectionName,
    JSData& input,
    std::stringstream& comments
  );
  bool loadOneObject(
    DatabaseCollection& collection,
    JSData& input,
    std::stringstream& comments
  );
};

// A temporary class to convert a mongo database dump to a json.
// May be deleted in the future once we're done with mongoDB.
class MongoDatabaseDumpToJSONConverter {
public:
  List<std::string> collectionFileNames;
  bool load(
    const std::string& folderNameFromUser,
    JSData& output,
    std::stringstream& commentsOnFailure
  );
  bool loadOneCollection(
    const std::string& collectionFileName,
    JSData& output,
    std::stringstream& comments
  );
  // Loads a collection from file content.
  // Expects newline-separated json records.
  bool collectionFromFileContent(
    const std::string& input,
    JSData& output,
    std::stringstream& comments
  );
};

#endif // header_database_ALREADY_INCLUDED
