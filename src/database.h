// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef DATABASE_HEADER_ALREADY_INCLUDED
#define DATABASE_HEADER_ALREADY_INCLUDED
#include "calculator_interface.h"
#include "general_time_date.h"
#include "general_database_system_independent.h"
#include "multiprocessing.h"

static ProjectInformationInstance projectInfoDatabaseH(__FILE__, "Database interface header. ");

class Database {
public:
  bool flagInitialized;
  List<std::string> modifyableColumns;
  int numDatabaseInstancesMustBeOneOrZero;

  // Get global database instance.
  // Implemented as function rather than static member to
  // avoid the static initalization order fiasco.
  static Database& get();

  bool initialize();
  bool CheckInitialization();
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
    bool SetPassword(
      const std::string& inputUsername,
      const std::string& inputNewPassword,
      std::string& outputAuthenticationToken,
      std::stringstream& comments
    );
    bool UserExists(const std::string& inputUsername, std::stringstream& comments);
    bool UserDefaultHasInstructorRights();

    bool StoreProblemInfoToDatabase(
      const UserCalculatorData& theUser, bool overwrite, std::stringstream& commentsOnFailure
    );

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
    static bool AddUsersFromEmails(
      const std::string& emailList,
      const std::string& userPasswords,
      std::string& userRole,
      std::string& userGroup,
      std::stringstream& comments,
      int& outputNumNewUsers,
      int& outputNumUpdatedUsers
    );
    static bool LoadUserInfo(UserCalculatorData& output, std::stringstream* commentsOnFailure);
    User();
  };
  User theUser;

  class FallBack{
  public:
    Database* owner;
    MutexProcess access;
    HashedList<std::string, MathRoutines::HashString> knownCollections;
    JSData reader;
    bool UpdateOneFromQueryString(
      const std::string& collectionName,
      const std::string& findQuery,
      const JSData& updateQuery,
      List<std::string>* fieldsToSetIfNullUseFirstFieldIfUpdateQuery,
      std::stringstream* commentsOnFailure = nullptr
    );
    bool HasCollection(const std::string& collection, std::stringstream* commentsOnFailure);
    bool ReadDatabase(JSData& output, std::stringstream* commentsOnFailure);
    void initialize();
    FallBack();
  };
  FallBack theFallBack;

  class Mongo{

  };
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
    const JSData& options,
    int maxOutputItems = - 1,
    long long* totalItems = nullptr,
    std::stringstream* commentsOnFailure = nullptr,
    std::stringstream* commentsGeneralNonSensitive = nullptr
  );
  static bool FindOneFromQueryString(
    const std::string& collectionName,
    const std::string& findQuery,
    JSData& output,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool FindOneFromQueryStringWithProjection(
    const std::string& collectionName,
    const std::string& findQuery,
    const List<std::string>& fieldsToProjectTo,
    JSData& output,
    std::stringstream* commentsOnFailure = nullptr
  );
  static JSData GetProjectionFromFieldNames(
    const List<std::string>& fieldsToProjectTo,
    int offset
  );
  static bool FindOneFromQueryStringWithOptions(
    const std::string& collectionName,
    const std::string& findQuery,
    const JSData& options,
    JSData& output,
    std::stringstream* commentsOnFailure = nullptr,
    std::stringstream* commentsGeneralNonSensitive = nullptr
  );
  static bool FindOneFromJSONWithProjection(
    const std::string& collectionName,
    const JSData& findQuery,
    const List<std::string>& fieldsToProjectTo,
    JSData& output,
    std::stringstream* commentsOnFailure,
    bool doEncodeFindFields
  );
  static bool GetOrFindQuery(
    const List<JSData>& input,
    std::string& output,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool FindOneFromJSON(
    const std::string& collectionName,
    const JSData& findQuery,
    JSData& output,
    std::stringstream* commentsOnFailure,
    bool doEncodeFindFields
  );
  static bool FindOneFromSome(
    const std::string& collectionName,
    const List<JSData>& findOrQueries,
    JSData& output,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool IsValidJSONMongoUpdateQuery(
    const JSData& updateQuery,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool IsValidJSONMongoFindQuery(
    const JSData& findQuery,
    std::stringstream* commentsOnFailure = nullptr,
    bool mustBeObject = true
  );
  static bool UpdateOneFromJSON(
    const std::string& collectionName,
    const JSData& findQuery,
    const JSData& updateQuery,
    List<std::string>* fieldsToSetIfNullUseFirstFieldIfUpdateQuery,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool UpdateOneFromJSONSpecifyField(
    const std::string& collectionName,
    const JSData& findQuery,
    const JSData& updateQuery,
    std::string fieldToSet,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool UpdateOneFromSomeJSON(
    const std::string& collectionName,
    const List<JSData>& findOrQueries,
    const JSData& updateQuery,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool UpdateOneFromQueryString(
    const std::string& collectionName,
    const std::string& findQuery,
    const JSData& updateQuery,
    List<std::string>* fieldsToSetIfNullUseFirstFieldIfUpdateQuery,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool FetchCollectionNames(List<std::string>& output, std::stringstream* commentsOnFailure);
  static bool FetchTable(
    const std::string& tableName,
    List<std::string>& outputLabels,
    List<List<std::string> >& outputRows,
    long long* totalItems = nullptr,
    std::stringstream* commentsOnFailure = nullptr
  );
  static bool DeleteOneEntry(const JSData& theEntry, std::stringstream* commentsOnFailure);
  static bool DeleteOneEntryById(
    const std::string& tableName,
    const JSData& findQuery,
    std::stringstream* commentsOnFailure
  );
  static bool DeleteOneEntryUnsetUnsecure(
    const std::string& tableName,
    const JSData& findQuery,
    List<std::string>& selector,
    std::stringstream* commentsOnFailure
  );
  static std::string ToHtmlDatabaseCollection(const std::string& currentTable);
  static JSData ToJSONFetchItem(const List<std::string>& labelStrings);
  static JSData ToJSONDatabaseCollection(const std::string& currentTable);
  static JSData ToJSONDatabaseFetch(const std::string& incomingLabels);
  void CreateHashIndex(const std::string& collectionName, const std::string& theKey);
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
  static JSData GetStandardProjectors();
  Database();
  ~Database();
};

#endif // DATABASE_HEADER_ALREADY_INCLUDED
