// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef CALCULATOR_DATABASE_MONGO_HEADER_ALREADY_INCLUDED
#define CALCULATOR_DATABASE_MONGO_HEADER_ALREADY_INCLUDED

#include "calculator_interface.h"

class CalculatorDatabaseFunctions {
public:
  static bool innerExecuteMongoQuery(Calculator& theCommands, const Expression& input, Expression& output);
};
#endif
