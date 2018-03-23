#include <mongoc.h>
#include "vpfHeader3Calculator5_Database_Mongo.h"

bool CalculatorDatabaseFunctions::innerExecuteMongoQuery
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorDatabaseFunctions::innerExecuteMongoQuery");
  return output.AssignValue((std::string) "Not implemented yet.", theCommands);
}
