 // The current file is licensed under the license terms found in the main header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
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
