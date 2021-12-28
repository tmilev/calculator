// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef DATABASE_HEADER_ALREADY_INCLUDED
#define DATABASE_HEADER_ALREADY_INCLUDED
#include "calculator_interface.h"
#include "general_time_date.h"
#include "general_database_system_independent.h"
#include "multiprocessing.h"

class QueryExact {
  public:
  std::string collection;
  List<std::string> nestedLabels;
  JSData value;
  QueryExact();
  QueryExact(const std::string& desiredCollection, const std::string& label, const std::string& desiredValue);
  QueryExact(const std::string& desiredCollection, const List<std::string>& desiredLabels, const std::string& desiredValue);
  void setLabelValue(const std::string& label, const std::string& desiredValue);
  void setLabelsValue(const List<std::string>& labels, const std::string& desiredValue);
  std::string getCollectionAndLabel() const;
  std::string getLabel() const;
  static std::string getLabelFromNestedLabels(const List<std::string>& nestedLabels);
  JSData toJSON() const;
  bool isEmpty() const;
};

class QuerySet {
public:
  List<std::string> nestedLabels;
  JSData value;
  QuerySet();
  QuerySet(const JSData& inputValue);
  bool toJSONMongo(JSData& output, std::stringstream* commentsOnFailure) const;
  bool toJSONSetMongo(JSData& output, std::stringstream* commentsOnFailure) const;
  std::string toStringDebug() const;
};

class QueryResultOptions {
public:
  List<std::string> fieldsToProjectTo;
  List<std::string> fieldsProjectedAway;
  JSData toJSON() const;
  void makeProjection(const List<std::string>& fields);

};

class Database {
public:
  bool flagInitializedServer;
  bool flagInitializedWorker;
  List<std::string> modifyableColumns;
  int numDatabaseInstancesMustBeOneOrZero;

  // Get global database instance.
  // Implemented as function rather than static member to
  // avoid the static initalization order fiasco.
  static Database& get();

  bool initializeServer();
  bool initializeWorker();
  bool checkInitialization();
  void createHashIndex(const std::string& collectionName, const std::string& key);

  class User {
  public:
    Database *owner;
    bool logoutViaDatabase();
    bool loginViaDatabase(
      UserCalculatorData& user,
      std::stringstream* commentsOnFailure
    );
    bool loginNoDatabaseSupport(UserCalculatorData& user, std::stringstream* commentsGeneral);
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
    bool userExists(const std::string& inputUsername, std::stringstream& comments);
    bool userDefaultHasInstructorRights();

    //TODO(tmilev): refactor down to database-only operations.
    static bool sendActivationEmail(
      const std::string& emailList,
      std::stringstream* commentsOnFailure,
      std::stringstream* commentsGeneral,
      std::stringstream* commentsGeneralSensitive
    );
    //TODO(tmilev): refactor down to database-only operations.
    static bool sendActivationEmail(
      const List<std::string>& emails,
      std::stringstream* commentsOnFailure,
      std::stringstream* commentsGeneral,
      std::stringstream* commentsGeneralSensitive
    );
    //TODO(tmilev): refactor down to database-only operations.
    bool addUsersFromEmails(
      const std::string& emailList,
      const std::string& userPasswords,
      std::string& userRole,
      std::string& userGroup,
      std::stringstream& comments,
      int& outputNumNewUsers,
      int& outputNumUpdatedUsers
    );
    bool loadUserInformation(UserCalculatorData& output, std::stringstream* commentsOnFailure);
    User();
  };
  User user;

  class FallBack {
  public:
    Database* owner;
    MutexProcess access;
    HashedList<std::string, MathRoutines::hashString> knownCollectionS;
    HashedList<std::string, MathRoutines::hashString> knownIndices;
    JSData reader;
    bool flagDatabaseRead;
    static std::string databaseFilename;
    class Index {
    public:
      // Collection A, label B is denoted as A.B.
      // Dots in the labels and collections are forbidden.
      std::string collection;
      std::string label;
      std::string collectionAndLabelCache;
      MapList<std::string, List<int32_t>, MathRoutines::hashString> locations;
      static std::string collectionAndLabelStatic(const std::string& inputCollection, const std::string& inputLabel);
      std::string collectionAndLabel();
    };
    MapReferences<std::string, Database::FallBack::Index, MathRoutines::hashString> indices;
    bool deleteDatabase(std::stringstream* commentsOnFailure);
    bool updateOne(
      const QueryExact& findQuery,
      const QuerySet &updateQuery,
      std::stringstream* commentsOnFailure = nullptr
    );
    bool findOne(
      const QueryExact& query,
      JSData& output,
      std::stringstream* commentsOnFailure
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
    bool updateOneNolocks(
      const QueryExact& findQuery,
      const QuerySet& updateQuery,
      std::stringstream* commentsOnFailure = nullptr
    );
    bool fetchCollectionNames(
      List<std::string>& output, std::stringstream* commentsOnFailure
    );
    void createHashIndex(const std::string& collectionName, const std::string& key);
    bool hasCollection(const std::string& collection, std::stringstream* commentsOnFailure);
    bool storeDatabase(std::stringstream* commentsOnFailure);
    bool readDatabase(std::stringstream* commentsOnFailure);
    bool readAndIndexDatabase(std::stringstream* commentsOnFailure);
    void indexOneRecord(const JSData& entry, int32_t row, const std::string& collection);
    void initialize();
    bool findOneFromSome(
      const List<QueryExact>& findOrQueries,
      JSData& output,
      std::stringstream* commentsOnFailure
    );
    std::string toStringIndices() const;
    FallBack();
  };
  FallBack fallBack;

  class Mongo {
  public:
    // The following variable has type mongoc_client_t.
    // This type is not forward declared due to compiler errors such as:
    // error: using typedef-name ‘mongoc_client_t’ after ‘struct’.
    void* client;
    // The following variable has type mongoc_client_t.
    // Not declared for reasons similar to the above.
    void* database;
    Database* owner;
    bool flagInitialized;
    std::string convertErrorToString(void* bson_error_t_pointer);
    bool initialize();
    void shutdown();
    bool deleteDatabase(std::stringstream *commentsOnFailure);
    void createHashIndex(const std::string& collectionName, const std::string& key);
    bool fetchCollectionNames(
      List<std::string>& output, std::stringstream* commentsOnFailure
    );
    static bool findOneWithOptions(
      const QueryExact& query,
      const QueryResultOptions& options,
      JSData& output,
      std::stringstream* commentsOnFailure,
      std::stringstream* commentsGeneralNonSensitive = nullptr
    );
    static bool findOneFromSome(
      const List<QueryExact>& findOrQueries,
      JSData& output,
      std::stringstream* commentsOnFailure
    );
    static bool getOrFindQuery(
      const List<QueryExact>& input,
      std::string& output,
      std::stringstream* commentsOnFailure = nullptr
    );
    bool updateOneFromSome(
      const List<QueryExact>& findOrQueries,
      const QuerySet& updateQuery,
      std::stringstream* commentsOnFailure = nullptr
    );
    bool updateOne(
      const QueryExact& findQuery,
      const QuerySet& updateQuery,
      std::stringstream* commentsOnFailure = nullptr
    );
    bool updateOneFromQueryString(
      const std::string& collectionName,
      const std::string& findQuery,
      const std::string& updateQuery,
      std::stringstream* commentsOnFailure = nullptr
    );
    Mongo();
    ~Mongo();
  };
  Mongo mongoDB;
  static bool findFromString(
    const std::string& collectionName,
    const std::string& findQuery,
    List<JSData>& output,
    int maxOutputItems = - 1,
    long long* totalItems = nullptr,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool findFromJSON(
    const std::string& collectionName,
    const JSData& findQuery,
    List<JSData>& output,
    int maxOutputItems = - 1,
    long long* totalItems = nullptr,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool findFromJSONWithProjection(
    const std::string& collectionName,
    const JSData& findQuery,
    List<JSData>& output,
    List<std::string>& fieldsToProjectTo,
    int maxOutputItems = - 1,
    long long* totalItems = nullptr,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool findFromJSONWithOptions(
    const std::string& collectionName,
    const JSData& findQuery,
    List<JSData>& output,
    const QueryResultOptions &options,
    int maxOutputItems = - 1,
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
    const QuerySet &dataToMerge,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool updateOneFromSome(
    const List<QueryExact>& findOrQueries,
    const QuerySet& updateQuery,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool fetchCollectionNames(List<std::string>& output, std::stringstream* commentsOnFailure);
  static bool FetchTable(
    const std::string& tableName,
    List<std::string>& outputLabels,
    List<List<std::string> >& outputRows,
    long long* totalItems = nullptr,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool deleteOneEntry(const JSData& entry, std::stringstream* commentsOnFailure);
  bool deleteOneEntryById(
    const QueryExact& findQuery,
    std::stringstream* commentsOnFailure
  );
  static bool deleteOneEntryUnsetUnsecure(
    const QueryExact& findQuery,
    List<std::string>& selector,
    std::stringstream* commentsOnFailure
  );
  std::string toHtmlDatabaseCollection(const std::string& currentTable);
  static JSData toJSONFetchItem(const List<std::string>& labelStrings);
  JSData toJSONDatabaseCollection(const std::string& currentTable);
  JSData toJSONDatabaseFetch(const std::string& incomingLabels);
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
  static bool matchesPattern(const List<std::string>& fieldLabel, const List<std::string>& pattern);
  static QueryResultOptions getStandardProjectors(const std::string& collectionName);
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
    void setUp();
    static bool all();
    bool deleteDatabase();
    bool adminAccountCreation();
    void tearDown();
    Test();
    ~Test();
  };
  Database();
  ~Database();
};

#endif // DATABASE_HEADER_ALREADY_INCLUDED
