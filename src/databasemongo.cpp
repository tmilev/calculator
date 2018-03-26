#include <mongoc.h>
#include <bcon.h>
#include "vpfheader7databaseinterface_mongodb.h"
#include "vpfJson.h"
ProjectInformationInstance ProjectInfoVpfDatabaseMongo(__FILE__, "Database mongoDB.");

int DatabaseRoutinesGlobalFunctionsMongo::numDatabaseInstancesMustBeOneOrZero = 0;
DatabaseRoutinesGlobalFunctionsMongo databaseMongo;
mongoc_client_t* databaseClient = 0;

extern logger logWorker;

DatabaseRoutinesGlobalFunctionsMongo::DatabaseRoutinesGlobalFunctionsMongo()
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::DatabaseRoutinesGlobalFunctionsMongo");
  mongoc_init();
  databaseClient = mongoc_client_new ("mongodb://localhost:27017");
}

DatabaseRoutinesGlobalFunctionsMongo::~DatabaseRoutinesGlobalFunctionsMongo()
{ mongoc_client_destroy(databaseClient);
  mongoc_cleanup();
}

class MongoCollection
{
public:
  mongoc_collection_t* collection;
  std::string name;
  MongoCollection(const std::string& collectionName)
  { this->name = collectionName;
    this->collection = mongoc_client_get_collection(databaseClient, "calculator", this->name.c_str());
  }
  ~MongoCollection()
  { mongoc_collection_destroy(this->collection);
    this->collection = 0;
  }
};

class MongoQuery
{
public:
  mongoc_cursor_t* cursor;
  bson_t* query;
  bson_error_t theError;
  int maxOutputItems;
  long long totalItems;
  std::string findQuery;
  std::string collectionName;
  MongoQuery()
  { this->query = 0;
    this->cursor = 0;
    this->maxOutputItems = - 1;
    this->totalItems = - 1;
  }
  ~MongoQuery()
  { mongoc_cursor_destroy(this->cursor);
    this->cursor = 0;
    bson_destroy(this->query);
    this->query = 0;
  }
  bool Find(List<std::string>& output, std::stringstream* commentsOnFailure)
  { MongoCollection theCollection(this->collectionName);
    logWorker << "About to fire up find query with: " << this->findQuery.c_str();
    this->query = bson_new_from_json((const uint8_t*) this->findQuery.c_str(), this->findQuery.size(), &this->theError);
    if (this->query == NULL)
    { if (commentsOnFailure != 0)
        *commentsOnFailure << this->theError.message;
      return false;
    }
    this->cursor = mongoc_collection_find_with_opts(theCollection.collection, this->query, NULL, NULL);
    if (this->cursor == NULL)
    { if (commentsOnFailure != 0)
        *commentsOnFailure << "Bad mongoDB cursor. ";
      return false;
    }
    this->totalItems = 0;
    const bson_t* bufferOutput;
    while (mongoc_cursor_next(this->cursor, &bufferOutput))
    { char* bufferOutpurStringFormat = 0;
      bufferOutpurStringFormat = bson_as_canonical_extended_json(bufferOutput, NULL);
      std::string current(bufferOutpurStringFormat);
      bson_free(bufferOutpurStringFormat);
      if (this->maxOutputItems <= 0 || this->totalItems < this->maxOutputItems)
        output.AddOnTop(current);
      this->totalItems ++;
    }
    return true;
  }
};

bool DatabaseRoutinesGlobalFunctionsMongo::FindQuery
(const std::string& collectionName, const std::string& findQuery,
 List<std::string>& output, int maxOutputItems,
 long long* totalItems, std::stringstream* commentsOnFailure)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::FindQuery");
  JSData theData;
  if (!theData.readstring(findQuery, commentsOnFailure))
    return false;
  logWorker << logger::blue << "query input: " << findQuery << logger::endL
  << logger::green << "jsdata: " << theData.ToString() << logger::endL;
  //stOutput << "Find query: " << theData.ToString();
  MongoQuery query;
  query.collectionName = collectionName;
  query.findQuery = theData.ToString();
  query.maxOutputItems = maxOutputItems;
  query.Find(output, commentsOnFailure);
  if (totalItems != 0)
    *totalItems = query.totalItems;
  return true;
}

void DatabaseRoutinesGlobalFunctionsMongo::LoadUserInfo(UserCalculatorData& output)
{ MacroRegisterFunctionWithName("DatabaseRoutinesGlobalFunctionsMongo::LoadUserInfo");
}
