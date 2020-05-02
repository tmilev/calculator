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
  void SetLabelValue(const std::string& label, const std::string& desiredValue);
  void SetLabelsValue(const List<std::string>& labels, const std::string& desiredValue);
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
  bool ToJSONMongo(JSData& output, std::stringstream* commentsOnFailure) const;
  bool ToJSONSetMongo(JSData& output, std::stringstream* commentsOnFailure) const;
  std::string toStringDebug() const;
};

class QueryResultOptions {
public:
  List<std::string> fieldsToProjectTo;
  List<std::string> fieldsProjectedAway;
  JSData toJSON() const;
  void MakeProjection(const List<std::string>& fields);

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
  void CreateHashIndex(const std::string& collectionName, const std::string& theKey);

  class User {
  public:
    Database *owner;
    bool LogoutViaDatabase();
    bool LoginViaDatabase(
      UserCalculatorData& theUseR,
      std::stringstream* commentsOnFailure
    );
    bool LoginNoDatabaseSupport(UserCalculatorData& theUser, std::stringstream* commentsGeneral);
    bool LoginViaGoogleTokenCreateNewAccountIfNeeded(
      UserCalculatorData& theUseR,
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
    bool UserExists(const std::string& inputUsername, std::stringstream& comments);
    bool UserDefaultHasInstructorRights();

    //TODO(tmilev): refactor down to database-only operations.
    static bool SendActivationEmail(
      const std::string& emailList,
      std::stringstream* commentsOnFailure,
      std::stringstream* commentsGeneral,
      std::stringstream* commentsGeneralSensitive
    );
    //TODO(tmilev): refactor down to database-only operations.
    static bool SendActivationEmail(
      const List<std::string>& theEmails,
      std::stringstream* commentsOnFailure,
      std::stringstream* commentsGeneral,
      std::stringstream* commentsGeneralSensitive
    );
    //TODO(tmilev): refactor down to database-only operations.
    bool AddUsersFromEmails(
      const std::string& emailList,
      const std::string& userPasswords,
      std::string& userRole,
      std::string& userGroup,
      std::stringstream& comments,
      int& outputNumNewUsers,
      int& outputNumUpdatedUsers
    );
    bool LoadUserInfo(UserCalculatorData& output, std::stringstream* commentsOnFailure);
    User();
  };
  User theUser;

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
    bool DeleteDatabase(std::stringstream* commentsOnFailure);
    bool UpdateOne(
      const QueryExact& findQuery,
      const QuerySet &updateQuery,
      std::stringstream* commentsOnFailure = nullptr
    );
    bool FindOne(
      const QueryExact& query,
      JSData& output,
      std::stringstream* commentsOnFailure
    );
    // Return indicates query success / failure.
    // When the element isn't found but otherwise there were
    // no problems with the query, true will be returned with
    // output set to [].
    bool FindIndexOneNoLocksMinusOneNotFound(
      const QueryExact& query,
      int& output,
      std::stringstream* commentsOnNotFound
    );
    bool UpdateOneNoLocks(
      const QueryExact& findQuery,
      const QuerySet& updateQuery,
      std::stringstream* commentsOnFailure = nullptr
    );
    bool FetchCollectionNames(
      List<std::string>& output, std::stringstream* commentsOnFailure
    );
    bool CheckDatabaseRead();
    void CreateHashIndex(const std::string& collectionName, const std::string& theKey);
    bool HasCollection(const std::string& collection, std::stringstream* commentsOnFailure);
    bool StoreDatabase(std::stringstream* commentsOnFailure);
    bool ReadDatabase(std::stringstream* commentsOnFailure);
    bool ReadAndIndexDatabase(std::stringstream* commentsOnFailure);
    void IndexOneRecord(const JSData& entry, int32_t row, const std::string& collection);
    void initialize();
    bool FindOneFromSome(
      const List<QueryExact>& findOrQueries,
      JSData& output,
      std::stringstream* commentsOnFailure
    );
    std::string ToStringIndices() const;
    FallBack();
  };
  FallBack theFallBack;

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
    std::string ConvertErrorToString(void* bson_error_t_pointer);
    bool initialize();
    void shutdown();
    bool DeleteDatabase(std::stringstream *commentsOnFailure);
    void CreateHashIndex(const std::string& collectionName, const std::string& theKey);
    bool FetchCollectionNames(
      List<std::string>& output, std::stringstream* commentsOnFailure
    );
    static bool FindOneWithOptions(
      const QueryExact& query,
      const QueryResultOptions& options,
      JSData& output,
      std::stringstream* commentsOnFailure,
      std::stringstream* commentsGeneralNonSensitive = nullptr
    );
    static bool FindOneFromSome(
      const List<QueryExact>& findOrQueries,
      JSData& output,
      std::stringstream* commentsOnFailure
    );
    static bool GetOrFindQuery(
      const List<QueryExact>& input,
      std::string& output,
      std::stringstream* commentsOnFailure = nullptr
    );
    bool UpdateOneFromSome(
      const List<QueryExact>& findOrQueries,
      const QuerySet& updateQuery,
      std::stringstream* commentsOnFailure = nullptr
    );
    bool UpdateOne(
      const QueryExact& findQuery,
      const QuerySet& updateQuery,
      std::stringstream* commentsOnFailure = nullptr
    );
    bool UpdateOneFromQueryString(
      const std::string& collectionName,
      const std::string& findQuery,
      const std::string& updateQuery,
      std::stringstream* commentsOnFailure = nullptr
    );
    Mongo();
    ~Mongo();
  };
  Mongo mongoDB;
  static bool FindFromString(
    const std::string& collectionName,
    const std::string& findQuery,
    List<JSData>& output,
    int maxOutputItems = - 1,
    long long* totalItems = nullptr,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool FindFromJSON(
    const std::string& collectionName,
    const JSData& findQuery,
    List<JSData>& output,
    int maxOutputItems = - 1,
    long long* totalItems = nullptr,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool FindFromJSONWithProjection(
    const std::string& collectionName,
    const JSData& findQuery,
    List<JSData>& output,
    List<std::string>& fieldsToProjectTo,
    int maxOutputItems = - 1,
    long long* totalItems = nullptr,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool FindFromJSONWithOptions(
    const std::string& collectionName,
    const JSData& findQuery,
    List<JSData>& output,
    const QueryResultOptions &options,
    int maxOutputItems = - 1,
    long long* totalItems = nullptr,
    std::stringstream* commentsOnFailure = nullptr,
    std::stringstream* commentsGeneralNonSensitive = nullptr
  );
  bool FindOneWithOptions(
    const QueryExact& query,
    const QueryResultOptions& options,
    JSData& output,
    std::stringstream* commentsOnFailure,
    std::stringstream* commentsGeneralNonSensitive = nullptr
  );
  bool FindOne(
    const QueryExact& query,
    JSData& output,
    std::stringstream* commentsOnFailure
  );
  bool FindOneFromSome(
    const List<QueryExact>& alternatives,
    JSData& output,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool UpdateOne(
    const QueryExact& findQuery,
    const QuerySet &dataToMerge,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool UpdateOneFromSome(
    const List<QueryExact>& findOrQueries,
    const QuerySet& updateQuery,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool FetchCollectionNames(List<std::string>& output, std::stringstream* commentsOnFailure);
  static bool FetchTable(
    const std::string& tableName,
    List<std::string>& outputLabels,
    List<List<std::string> >& outputRows,
    long long* totalItems = nullptr,
    std::stringstream* commentsOnFailure = nullptr
  );
  bool DeleteOneEntry(const JSData& theEntry, std::stringstream* commentsOnFailure);
  bool DeleteOneEntryById(
    const QueryExact& findQuery,
    std::stringstream* commentsOnFailure
  );
  static bool DeleteOneEntryUnsetUnsecure(
    const QueryExact& findQuery,
    List<std::string>& selector,
    std::stringstream* commentsOnFailure
  );
  std::string ToHtmlDatabaseCollection(const std::string& currentTable);
  static JSData ToJSONFetchItem(const List<std::string>& labelStrings);
  JSData ToJSONDatabaseCollection(const std::string& currentTable);
  JSData ToJSONDatabaseFetch(const std::string& incomingLabels);
  static bool getLabels(
    const JSData& fieldEntries,
    List<std::string>& theLabels,
    std::stringstream* commentsOnFailure
  );
  static bool isDeleteable(
    const List<std::string>& theLabels,
    List<std::string>** outputPattern,
    std::stringstream* commentsOnFailure
  );
  static bool isDeleteable(
    const JSData& theEntry,
    List<std::string>** outputPattern,
    std::stringstream* commentsOnFailure
  );
  static bool matchesPattern(const List<std::string>& fieldLabel, const List<std::string>& pattern);
  static QueryResultOptions GetStandardProjectors(const std::string& collectionName);
  static std::string ConvertStringToMongoKeyString(const std::string& input);
  static bool ConvertJSONMongoToJSON(
    const JSData& input,
    JSData& output,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool ConvertJSONToJSONMongo(
    const JSData& input,
    JSData& output,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool ConvertJSONToJSONEncodeKeys(
    const JSData& input,
    JSData& output,
    int recursionDepth,
    bool encodeOverDecode,
    std::stringstream* commentsOnFailure
  );
  bool DeleteDatabase(std::stringstream* commentsOnFailure);
  class Test {
  public:
    void SetUp();
    static bool all();
    bool DeleteDatabase();
    bool AdminAccountCreation();
    void TearDown();
    Test();
    ~Test();
  };
  Database();
  ~Database();
};

#endif // DATABASE_HEADER_ALREADY_INCLUDED
