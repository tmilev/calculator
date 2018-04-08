#ifndef VPFHEADER7DATABASEINTERFACE_MONGODB_H
#define VPFHEADER7DATABASEINTERFACE_MONGODB_H
#include "vpfHeader1General8DatabaseSystemIndependent.h"

static ProjectInformationInstance ProjectInfoVpfHEADER7DATABASEINTERFACE_MONGODB_H(__FILE__, "MongoDB interface header.");

class DatabaseRoutinesGlobalFunctionsMongo
{
public:
  bool flagInitialized;
  bool initialize(std::stringstream* commentsOnFailure);
  static int numDatabaseInstancesMustBeOneOrZero;
  static bool LoadUserInfo(UserCalculatorData& output);
  static bool FindFromString
  (const std::string& collectionName, const std::string& findQuery,
   List<JSData>& output, int maxOutputItems = - 1,
   long long* totalItems = 0, std::stringstream* commentsOnFailure = 0);
  static bool FindFromJSON
  (const std::string& collectionName, const JSData& findQuery,
   List<JSData>& output, int maxOutputItems = - 1,
   long long* totalItems = 0, std::stringstream* commentsOnFailure = 0);
  static bool FindOneFromQueryString
  (const std::string& collectionName, const std::string& findQuery,
   JSData& output, std::stringstream* commentsOnFailure = 0);

  static bool GetOrFindQuery(const List<JSData>& input, std::string& output, std::stringstream* commentsOnFailure = 0);
  static bool FindOneFromJSON
  (const std::string& collectionName, const JSData& findQuery,
   JSData& output, std::stringstream* commentsOnFailure = 0);
  static bool FindOneFromSome
  (const std::string& collectionName, const List<JSData>& findOrQueries,
   JSData& output, std::stringstream* commentsOnFailure = 0);
  static bool IsValidJSONMongoQuery
  (const JSData& findQuery, std::stringstream* commentsOnFailure = 0, bool mustBeObject = true);
  static bool UpdateOneFromJSON
  (const std::string& collectionName, const JSData& findQuery, const JSData& updateQuery,
   std::stringstream* commentsOnFailure = 0);
  static bool UpdateOneFromSomeJSON
  (const std::string& collectionName, const List<JSData>& findOrQueries, const JSData& updateQuery,
   std::stringstream* commentsOnFailure = 0);
  static bool UpdateOneFromQueryString
  (const std::string& collectionName, const std::string& findQuery, const JSData& updateQuery,
   std::stringstream* commentsOnFailure = 0);
  static bool FetchCollectionNames(List<std::string>& output, std::stringstream* commentsOnFailure);
  static bool FetchTable
  (const std::string& tableName, List<std::string>& outputLabels, List<List<std::string> >& outputRows,
   long long* totalItems = 0, std::stringstream* commentsOnFailure = 0);
  static std::string ToHtmlDatabaseCollection(const std::string& currentTable);
  void CreateHashIndex(const std::string& collectionName, const std::string& theKey);
  DatabaseRoutinesGlobalFunctionsMongo();
  ~DatabaseRoutinesGlobalFunctionsMongo();
};

#endif // VPFHEADER7DATABASEINTERFACE_MONGODB_H
