#include "calculator_database_mongo.h"
#include "database.h"

bool CalculatorDatabaseFunctions::innerExecuteMongoQuery(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorDatabaseFunctions::innerExecuteMongoQuery");
  if (!global.UserDefaultHasAdminRights()) {
    return output.assignValue(std::string("Administrator rights needed for mongoDB queries. "), theCommands);
  }
  if (input.size() < 3) {
    return theCommands << "Expected at least 2 arguments: collection name and query. ";
  }
  std::string inputCollection, inputQuery;
  if (!input[2].isOfType(&inputQuery)) {
    theCommands << "Expected string as second argument. Using the string: "
    << input[2].toString() << " as second name. ";
    inputQuery = input[2].toString();
  }
  if (!input[1].isOfType(&inputCollection)) {
    theCommands << "Expected string as first argument. Using the string: "
    << input[1].toString() << " as collection name. ";
    inputCollection = input[1].toString();
  }
  List<JSData> outputList;
  std::stringstream commentsOnFailure;
  if (!Database::get().FindFromString(
    inputCollection, inputQuery, outputList, - 1, nullptr, &commentsOnFailure
  )) {
    return output.assignValue(commentsOnFailure.str(), theCommands);
  }
  JSData finalOutput;
  finalOutput.theType = JSData::token::tokenArray;
  finalOutput.theList = outputList;
  return output.assignValue(HtmlRoutines::toHtmlTableRowsFromJSON(finalOutput), theCommands);
}
