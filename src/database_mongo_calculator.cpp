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
  { theCommands << "Expected string as second argument. Using the string: "
    << input[2].ToString() << " as second name. ";
    inputQuery = input[2].ToString();
  }
  if (!input[1].IsOfType(&inputCollection))
  { theCommands << "Expected string as first argument. Using the string: "
    << input[1].ToString() << " as collection name. ";
    inputCollection = input[1].ToString();
  }
  List<std::string> outputStrings;
  std::stringstream commentsOnFailure;
  if (!DatabaseRoutinesGlobalFunctionsMongo::FindFromString(inputCollection, inputQuery, outputStrings, - 1, 0, &commentsOnFailure))
    return output.AssignValue(commentsOnFailure.str(), theCommands);
  return output.AssignValue(outputStrings.ToStringCommaDelimited(), theCommands);
}
