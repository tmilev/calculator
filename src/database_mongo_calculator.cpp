#include <mongoc.h>
#include <bcon.h>
#include "vpfHeader3Calculator5_Database_Mongo.h"
#include "vpfheader7databaseinterface_mongodb.h"
ProjectInformationInstance ProjectInfoVpfMongoCalculatorInterface(__FILE__, "Database mongo<->calculator interface.");

extern logger logWorker;

bool CalculatorDatabaseFunctions::innerExecuteMongoQuery
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorDatabaseFunctions::innerExecuteMongoQuery");
  if (!theGlobalVariables.UserDefaultHasAdminRights())
    return output.AssignValue((std::string) "Admin rights needed for mongoDB queries. ", theCommands);
  if (input.size() < 3)
    return theCommands << "Expected at least 2 arguments: collection name and query";
  std::string inputCollection, inputQuery;
  if (!input[2].IsOfType(&inputQuery))
    return theCommands << "Expected string as second argument, got: " << input[2].ToString() << " instead. ";
  if (!input[1].IsOfType(&inputCollection))
    return theCommands << "Expected string as first argument, got: " << input[1].ToString() << " instead. ";
  List<std::string> outputStrings;
  std::stringstream commentsOnFailure;
  if (!DatabaseRoutinesGlobalFunctionsMongo::FindQuery(inputCollection, inputQuery, outputStrings, -1, 0, &commentsOnFailure))
    return output.AssignValue(commentsOnFailure.str(), theCommands);
  return output.AssignValue(outputStrings.ToStringCommaDelimited(), theCommands);
/*

  std::stringstream out;
  mongoc_client_t* client;
  mongoc_collection_t* collection;
  mongoc_cursor_t* cursor;
  const bson_t* doc;
  bson_t* query;
  char* str;
  mongoc_init();
  client = mongoc_client_new ("mongodb://localhost:27017");
  collection = mongoc_client_get_collection (client, "calculator", inputCollection.c_str());
  bson_error_t theError;
  query = bson_new_from_json((const uint8_t*) inputQuery.c_str(), inputQuery.size(), &theError);
  if (query == NULL)
  { out << "Failed to convert " << inputQuery << " to bson. Error: " << theError.message;
    return output.AssignValue(out.str(), theCommands);
  }
  cursor = mongoc_collection_find_with_opts (collection, query, NULL, NULL);
  while (mongoc_cursor_next (cursor, &doc))
  { str = bson_as_canonical_extended_json (doc, NULL);
    out << str;
    bson_free (str);
  }
  bson_destroy (query);
  mongoc_cursor_destroy (cursor);
  mongoc_collection_destroy (collection);
  mongoc_client_destroy (client);
  mongoc_cleanup ();
  return output.AssignValue(out.str(), theCommands);*/
}

