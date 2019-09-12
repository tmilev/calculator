#ifndef VPFHEADER3CALCULATOR5database_mongo_h
#define VPFHEADER3CALCULATOR5database_mongo_h

#include "calculator_interface.h"
static ProjectInformationInstance ProjectInfoVPFHEADER3CALCULATOR5database_mongo_h(
  __FILE__, "Header, calculator<->mongoDB interface. "
);

class CalculatorDatabaseFunctions {
public:
  static bool innerExecuteMongoQuery(Calculator& theCommands, const Expression& input, Expression& output);
  static bool innerRepairDatabaseEmailRecords(Calculator& theCommands, const Expression& input, Expression& output);
};
#endif
