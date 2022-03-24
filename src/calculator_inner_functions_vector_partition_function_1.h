// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_calculator_inner_functions_vector_partition_function_1_ALREADY_INCLUDED
#define header_calculator_inner_functions_vector_partition_function_1_ALREADY_INCLUDED

#include "calculator_interface.h"

class CalculatorFunctionsVectorPartitionFunction {
public:
  static bool vectorPartitionFunctionFormula(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool coneDecomposition(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

#endif // header_calculator_inner_functions_vector_partition_function_1_ALREADY_INCLUDED
