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
  static bool FindFromJSONWithProjection
  (const std::string& collectionName, const JSData& findQuery,
   List<JSData>& output, List<std::string>& fieldsToProjectTo, int maxOutputItems = - 1,
   long long* totalItems = 0, std::stringstream* commentsOnFailure = 0);
  static bool FindFromJSONWithOptions
  (const std::string& collectionName, const JSData& findQuery,
   List<JSData>& output, const JSData& options, int maxOutputItems = - 1,
   long long* totalItems = 0, std::stringstream* commentsOnFailure = 0, std::stringstream* commentsGeneralNonSensitive = 0);
  static bool FindOneFromQueryString
  (const std::string& collectionName, const std::string& findQuery,
   JSData& output, std::stringstream* commentsOnFailure = 0);
  static bool FindOneFromQueryStringWithProjection
  (const std::string& collectionName, const std::string& findQuery, const List<std::string>& fieldsToProjectTo,
   JSData& output, std::stringstream* commentsOnFailure = 0);
  static JSData GetProjectionFromFieldNames(const List<std::string>& fieldsToProjectTo, int offset);
  static bool FindOneFromQueryStringWithOptions
  (const std::string& collectionName, const std::string& findQuery, const JSData& options,
   JSData& output, std::stringstream* commentsOnFailure = 0, std::stringstream* commentsGeneralNonSensitive = 0);
  static bool FindOneFromJSONWithProjection
  (const std::string& collectionName, const JSData& findQuery, const List<std::string>& fieldsToProjectTo,
   JSData& output, std::stringstream* commentsOnFailure, bool doEncodeFindFields);

  static bool GetOrFindQuery(const List<JSData>& input, std::string& output, std::stringstream* commentsOnFailure = 0);
  static bool FindOneFromJSON
  (const std::string& collectionName, const JSData& findQuery,
   JSData& output, std::stringstream* commentsOnFailure, bool doEncodeFindFields);
  static bool FindOneFromSome
  (const std::string& collectionName, const List<JSData>& findOrQueries,
   JSData& output, std::stringstream* commentsOnFailure = 0);
  static bool IsValidJSONMongoUpdateQuery
  (const JSData& updateQuery, std::stringstream* commentsOnFailure = 0);
  static bool IsValidJSONMongoFindQuery
  (const JSData& findQuery, std::stringstream* commentsOnFailure = 0, bool mustBeObject = true);
  static bool UpdateOneFromJSON
  (const std::string& collectionName, const JSData& findQuery, const JSData& updateQuery,
   List<std::string>* fieldsToSetIfNullUseFirstFieldIfUpdateQuery,
   std::stringstream* commentsOnFailure = 0);
  static bool UpdateOneFromJSONSpecifyField
  (const std::string& collectionName, const JSData& findQuery, const JSData& updateQuery,
   std::string fieldToSet, std::stringstream* commentsOnFailure = 0);
  static bool UpdateOneFromSomeJSON
  (const std::string& collectionName, const List<JSData>& findOrQueries, const JSData& updateQuery,
   std::stringstream* commentsOnFailure = 0);
  static bool UpdateOneFromQueryString
  (const std::string& collectionName, const std::string& findQuery, const JSData& updateQuery,
   List<std::string>* fieldsToSetIfNullUseFirstFieldIfUpdateQuery,
   std::stringstream* commentsOnFailure = 0);
  static bool FetchCollectionNames(List<std::string>& output, std::stringstream* commentsOnFailure);
  static bool FetchTable
  (const std::string& tableName, List<std::string>& outputLabels, List<List<std::string> >& outputRows,
   long long* totalItems = 0, std::stringstream* commentsOnFailure = 0);
  static bool DeleteOneEntry(const JSData& theEntry, std::stringstream* commentsOnFailure);
  static bool DeleteOneEntryById
  (const std::string& tableName, const JSData& findQuery, std::stringstream* commentsOnFailure);
  static bool DeleteOneEntryUnsetUnsecure
  (const std::string& tableName, const JSData& findQuery, List<std::string>& selector,
   std::stringstream* commentsOnFailure);
  static std::string ToHtmlDatabaseCollection(const std::string& currentTable);
  static JSData ToJSONFetchItem(const List<std::string>& labelStrings);
  static JSData ToJSONDatabaseCollection(const std::string& currentTable);
  static JSData ToJSONDatabaseFetch(const std::string& incomingLabels);
  void CreateHashIndex(const std::string& collectionName, const std::string& theKey);
  static bool getLabels(const JSData& fieldEntries, List<std::string>& theLabels, std::stringstream* commentsOnFailure);
  static bool isDeleteable(const List<std::string>& theLabels, List<std::string>** outputPattern, std::stringstream* commentsOnFailure);
  static bool isDeleteable(const JSData& theEntry, List<std::string>** outputPattern, std::stringstream* commentsOnFailure);
  static bool matchesPattern
  (const List<std::string>& fieldLabel, const List<std::string>& pattern);
  static JSData GetStandardProjectors();
  DatabaseRoutinesGlobalFunctionsMongo();
  ~DatabaseRoutinesGlobalFunctionsMongo();
};

#endif // VPFHEADER7DATABASEINTERFACE_MONGODB_H

