
#ifndef header_calculator_inner_functions_vector_partition_function_1_ALREADY_INCLUDED
#define header_calculator_inner_functions_vector_partition_function_1_ALREADY_INCLUDED

#include "calculator_interface.h"

class CalculatorFunctionsVectorPartitionFunction {
public:
  static bool vectorPartitionFunctionFormula(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool vectorPartitionFunctionFormulaElementary(
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
  // Returns the Bernoulli sum / Faulhaber formula: the sum \sum_{i=0}^n i^p.
  static bool bernoulliSum(
      Calculator &calculator, const Expression &input, Expression &output);
  static bool lattice(
      Calculator &calculator, const Expression &input, Expression &output);
  static bool subLatticeWithIntegralScalarProducts(
      Calculator &calculator, const Expression &input, Expression &output);
};

#endif // header_calculator_inner_functions_vector_partition_function_1_ALREADY_INCLUDED
