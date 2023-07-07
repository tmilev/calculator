
#ifndef header_database_ALREADY_INCLUDED
#define header_database_ALREADY_INCLUDED

#include "general_database_system_independent.h"
#include "multiprocessing.h"

class UserCalculator;
class Database;

// Forward declare a class that can listen to sockets.
class Listener;

// Stores a recipe for updating an item.
// A typical item would be stored allong the lines of:
// {
//   "collection1": [
//     {
//       keyId:"id1", value1: {
//         key1: "XX", key2: "YY", "nasty%2fencoded%2fkey": "AA"
//       }
//     },
//     {keyId:"id2", value1: {key1: "XX", key2: "ZZ"}},
//   ]
// }
class QuerySet {
private:
  static void makeFromRecursive(
    const JSData& input,
    List<std::string>& nestedLabels,
    QuerySet& output
  );
public:
  // Key-value pairs to be set.
  // 1. All keys that contain dots are regarded as sub-field selectors.
  // The query:
  // nestedLabels=[]
  // value = {"key1.value1": "XXY"}
  // will set the value1.key1 sub-key of an item.
  // 2. All keys of this json that do not contain
  // dots are expected to be %-encoded.
  JSData value;
  QuerySet();
  bool fromJSON(
    const JSData& inputValue, std::stringstream* commentsOnFailure
  );
  void fromJSONNoFail(const JSData& inputValue);
  bool toJSON(JSData& output) const;
  std::string toStringDebug() const;
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
//       keyId:"id1", value1: {
//         key1: "XX", key2: "YY", "nasty%2fencoded%2fkey": "AA"
//       }
//     },
//     {keyId:"id2", value1: {key1: "XX", key2: "ZZ"}},
//   ]
// }
// Example 1.
// Item {keyId:"id1", value1: {key1: "XX", key2: "YY"}} is selected with:
// collection = "collection1"
// nestedLabels = ["keyId"]
// value = "id1"
//
// Example 2.
// Item {keyId:"id1", value1: {key1: "XX", key2: "YY"}} can also be selected
// with:
// collection = "collection1"
// nestedLabels = ["value1", "key2"]
// value = "YY"
//
// Important. All keys in our database are expected to not contain a dot,
// as that conflicts with MongoDB's syntax for selection of sub-JSON.
// To ensure that, all keys in our database are assumed to be %-encoded.
// Final values [leaf nodes when representing our JSON as a tree]
// in our database are allowed to contain dots and need not be %-encoded.
class QueryExact {
public:
  std::string collection;
  // Labels, stored in non-encoded form.
  List<std::string> nestedLabels;
  // Exact values we are looking for. All keys must be %-encoded.
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
  void setLabelValue(
    const std::string& label, const std::string& desiredValue
  );
  void setLabelsValue(
    const List<std::string>& labels, const std::string& desiredValue
  );
  bool getMongoKeyValue(
    std::string& outputKey, std::string& outputValue
  ) const;
  std::string getLabel() const;
  static std::string getLabelFromNestedLabels(
    const List<std::string>& nestedLabels
  );
  static std::string getLabelFromNestedLabels(
    const std::string& first, const std::string& second
  );
  // Returns a combination of the labels and value into JSON format,
  // ignoring the collection name.
  // In the example of
  // nestedLabels = ["keyId"]
  // value = "id1",
  // this will return {"keyId": "id1"}
  JSData toJSONCombineLabelsAndValue() const;
  JSData toJSON() const;
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
  JSData toJSON() const;
  bool fromJSON(JSData& input, std::stringstream* commentsOnFailure);
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

// Work in progress. When done, this will be a self contained
// simple database implementation that supports concurrent reads,
// writes and locks.
class DatabaseInternal {
  // Map from listening sockets to ports.
  ListReferences<DatabaseInternalConnection> connections;
  MapList<int, int> mapFromReadEndsToWorkerIds;
  List<int> readEnds;
  List<char> buffer;
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
  bool sendAndReceiveFromClientToServer(
    const DatabaseInternalRequest& input,
    DatabaseInternalResult& output,
    std::stringstream* commentsOnFailure
  );
  static std::string folder();
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
    std::stringstream* commentsOnFailure = nullptr
  );
  bool updateOneFromSome(
    const List<QueryExact>& findOrQueries,
    const QuerySet& updateQuery,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool deleteDatabase(std::stringstream* commentsOnFailure);
  void createHashIndex(
    const std::string& collectionName, const std::string& key
  );
  // Forks out a child process to run the database.
  // Returns the process id of the child database to the parent
  // and 0 to the child database process.
  int forkOutDatabase();
  void initializeForkAndRun(int maximumConnections);
  // Not fully implemented yet, do not use.
  // When implemented, would listen on socket and read write.
  // Reads/writes using basic pipes.
  void run();
  bool runOneConnection();
  bool executeAndSend();
  bool fetchCollectionNames(
    List<std::string>& output, std::stringstream* commentsOnFailure
  );
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
  static std::string convertStringToMongoKeyString(const std::string& input);
  static bool convertJSONMongoToJSON(
    const JSData& input,
    JSData& output,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool convertJSONToJSONMongo(
    const JSData& input,
    JSData& output,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool convertJSONToJSONEncodeKeys(
    const JSData& input,
    JSData& output,
    int recursionDepth,
    bool encodeOverDecode,
    std::stringstream* commentsOnFailure
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
