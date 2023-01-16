
#ifndef header_calculator_inner_functions_vector_partition_function_1_ALREADY_INCLUDED
#define header_calculator_inner_functions_vector_partition_function_1_ALREADY_INCLUDED

#include "calculator_interface.h"

class CalculatorFunctionsVectorPartitionFunction {
public:
  static bool vectorPartitionFunctionFormula(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool vectorPartitionFunctionFormulaWithDetails(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool vectorPartitionFunctionFormulaInternal(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool flagShowDetails
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
    bool flagIncludeHistory,
    bool flagAmalgamateChambers
  );
  static bool getVectorsForConeDecomposition(
    Calculator& calculator,
    const Expression& input,
    Vectors<Rational>& output,
    std::string& outputLabel
  );
  static bool getVectorsForConeDecompositionFromDynkinType(
    Calculator& calculator,
    const Expression& input,
    Vectors<Rational>& output,
    std::string& outputLabel
  );
  static bool coneDecompositionSpannedSlicesNoHistory(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsVectorPartitionFunction::coneDecomposition(
      calculator, input, output, true, false, true
    );
  }
  static bool coneDecompositionSpannedSlicesNoHistoryNoAmalgamation(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsVectorPartitionFunction::coneDecomposition(
      calculator, input, output, true, false, false
    );
  }
  static bool coneDecompositionSpannedSlicesWithHistory(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsVectorPartitionFunction::coneDecomposition(
      calculator, input, output, true, true, true
    );
  }
  static bool coneDecompositionArbitrarySlicesWithHistory(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsVectorPartitionFunction::coneDecomposition(
      calculator, input, output, true, true, false
    );
  }
  static bool coneDecompositionArbitrarySlices(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsVectorPartitionFunction::coneDecomposition(
      calculator, input, output, false, false, false
    );
  }
  // Returns the Bernoulli sum / Faulhaber formula: the sum \sum_{i=0}^n i^p.
  static bool bernoulliSum(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool lattice(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool quotientLatticeRepresentatives(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool reduceModuloLattice(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool dualLattice(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool subLatticeWithIntegralScalarProducts(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool subLatticeScalarProductTimesDirectionInLattice(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool kostantPartitionFunctionLatex(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotLatticeRestricted(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

#endif // header_calculator_inner_functions_vector_partition_function_1_ALREADY_INCLUDED
