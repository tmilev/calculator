#include "calculator_database_mongo.h"
#include "database.h"

bool CalculatorDatabaseFunctions::innerExecuteMongoQuery(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorDatabaseFunctions::innerExecuteMongoQuery");
  if (!global.userDefaultHasAdminRights()) {
    return output.assignValue(std::string("Administrator rights needed for mongoDB queries. "), calculator);
  }
  if (input.size() < 3) {
    return calculator << "Expected at least 2 arguments: collection name and query. ";
  }
  std::string inputCollection, inputQuery;
  if (!input[2].isOfType(&inputQuery)) {
    calculator << "Expected string as second argument. Using the string: "
    << input[2].toString() << " as second name. ";
    inputQuery = input[2].toString();
  }
  if (!input[1].isOfType(&inputCollection)) {
    calculator << "Expected string as first argument. Using the string: "
    << input[1].toString() << " as collection name. ";
    inputCollection = input[1].toString();
  }
  List<JSData> outputList;
  std::stringstream commentsOnFailure;
  if (!Database::get().findFromString(
    inputCollection, inputQuery, outputList, - 1, nullptr, &commentsOnFailure
  )) {
    return output.assignValue(commentsOnFailure.str(), calculator);
  }
  JSData finalOutput;
  finalOutput.theType = JSData::token::tokenArray;
  finalOutput.listObjects = outputList;
  return output.assignValue(HtmlRoutines::toHtmlTableRowsFromJSON(finalOutput), calculator);
}
