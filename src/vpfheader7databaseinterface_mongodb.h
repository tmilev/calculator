#ifndef VPFHEADER7DATABASEINTERFACE_MONGODB_H
#define VPFHEADER7DATABASEINTERFACE_MONGODB_H
#include "vpfHeader1General8DatabaseSystemIndependent.h"

static ProjectInformationInstance ProjectInfoVpfHEADER7DATABASEINTERFACE_MONGODB_H(__FILE__, "MongoDB interface header.");

class DatabaseRoutinesGlobalFunctionsMongo
{
public:
  static int numDatabaseInstancesMustBeOneOrZero;
  static void LoadUserInfo(UserCalculatorData& output);
  static bool FindFromString
  (const std::string& collectionName, const std::string& findQuery,
   List<std::string>& output, int maxOutputItems = -1,
   long long* totalItems = 0, std::stringstream* commentsOnFailure = 0);
  static bool FindFromJSON
  (const std::string& collectionName, const JSData& findQuery,
   List<std::string>& output, int maxOutputItems = -1,
   long long* totalItems = 0, std::stringstream* commentsOnFailure = 0);
  static bool FindOneFromJSON
  (const std::string& collectionName, const JSData& findQuery,
   std::string& output, std::stringstream* commentsOnFailure = 0);
  static bool UpdateOneFromJSON
  (const std::string& collectionName, const JSData& findQuery, const JSData& updateQuery,
   std::stringstream* commentsOnFailure = 0);
  DatabaseRoutinesGlobalFunctionsMongo();
  ~DatabaseRoutinesGlobalFunctionsMongo();
};

#endif // VPFHEADER7DATABASEINTERFACE_MONGODB_H

