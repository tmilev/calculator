#ifndef CALCULATOR_DATABASE_MONGO_HEADER_ALREADY_INCLUDED
#define CALCULATOR_DATABASE_MONGO_HEADER_ALREADY_INCLUDED

#include "calculator_interface.h"
static ProjectInformationInstance projectInfoCalculatorDatabaseMongoHeader(
  __FILE__, "Header, calculator<->mongoDB interface. "
);

class CalculatorDatabaseFunctions {
public:
  static bool innerExecuteMongoQuery(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerRepairDatabaseEmailRecords(Calculator& theCommands, const Expression& input, Expression& output);
};
#endif
