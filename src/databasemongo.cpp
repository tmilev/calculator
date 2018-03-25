#include <mongoc.h>
#include <bcon.h>
#include "vpfHeader3Calculator5_Database_Mongo.h"

bool CalculatorDatabaseFunctions::innerExecuteMongoQuery
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorDatabaseFunctions::innerExecuteMongoQuery");
  std::stringstream out;
  mongoc_client_t *client;
  mongoc_collection_t *collection;
  mongoc_cursor_t *cursor;
  const bson_t *doc;
  bson_t *query;
  char *str;
  mongoc_init ();
  client = mongoc_client_new ("mongodb://localhost:27017/?appname=find-example");
  collection = mongoc_client_get_collection (client, "calculator", "users");
  query = bson_new ();
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
  return output.AssignValue(out.str(), theCommands);
}
