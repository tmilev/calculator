
#ifndef header_calculator_database_mongo_ALREADY_INCLUDED
#define header_calculator_database_mongo_ALREADY_INCLUDED

#include "calculator_interface.h"

class CalculatorDatabaseFunctions {
public:
  static bool executeMongoQuery(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

#endif
