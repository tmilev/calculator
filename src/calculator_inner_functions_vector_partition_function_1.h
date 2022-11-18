
#ifndef header_calculator_inner_functions_vector_partition_function_1_ALREADY_INCLUDED
#define header_calculator_inner_functions_vector_partition_function_1_ALREADY_INCLUDED

#include "calculator_interface.h"

class CalculatorFunctionsVectorPartitionFunction {
public:
  static bool vectorPartitionFunctionFormula(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool applyVectorPartitionFunctionFormula(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool coneDecomposition(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool flagUseSpannedSlices,
  bool flagIncludeHistory
  );
  static bool coneDecompositionSpannedSlicesNoHistory(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsVectorPartitionFunction::coneDecomposition(
      calculator, input, output, true, false
    );
  }
  static bool coneDecompositionSpannedSlicesWithHistory(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsVectorPartitionFunction::coneDecomposition(
      calculator, input, output, true, true
    );
  }
  static bool coneDecompositionArbitrarySlices(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsVectorPartitionFunction::coneDecomposition(
      calculator, input, output, false, false
    );
  }
};

#endif // header_calculator_inner_functions_vector_partition_function_1_ALREADY_INCLUDED
