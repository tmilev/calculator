#include "calculator_6_database_mongo.h"
#include "vpfHeader7DatabaseInterface_Mongodb.h"
ProjectInformationInstance ProjectInfoVpfMongoCalculatorInterface(__FILE__, "Database mongo<->calculator interface.");

extern logger logWorker;

bool CalculatorDatabaseFunctions::innerExecuteMongoQuery(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorDatabaseFunctions::innerExecuteMongoQuery");
  if (!theGlobalVariables.UserDefaultHasAdminRights()) {
    return output.AssignValue((std::string) "Admin rights needed for mongoDB queries. ", theCommands);
  }
  if (input.size() < 3) {
    return theCommands << "Expected at least 2 arguments: collection name and query. ";
  }
  std::string inputCollection, inputQuery;
  if (!input[2].IsOfType(&inputQuery)) {
    theCommands << "Expected string as second argument. Using the string: "
    << input[2].ToString() << " as second name. ";
    inputQuery = input[2].ToString();
  }
  if (!input[1].IsOfType(&inputCollection)) {
    theCommands << "Expected string as first argument. Using the string: "
    << input[1].ToString() << " as collection name. ";
    inputCollection = input[1].ToString();
  }
  List<JSData> outputList;
  std::stringstream commentsOnFailure;
  if (!DatabaseRoutinesGlobalFunctionsMongo::FindFromString(inputCollection, inputQuery, outputList, - 1, 0, &commentsOnFailure)) {
    return output.AssignValue(commentsOnFailure.str(), theCommands);
  }
  JSData finalOutput;
  finalOutput.theType = JSData::token::tokenArray;
  finalOutput.theList = outputList;
  return output.AssignValue(HtmlRoutines::ToHtmlTableRowsFromJSON(finalOutput), theCommands);
}
