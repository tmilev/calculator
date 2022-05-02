// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_inner_functions_vector_partition_function_1.h"

bool CalculatorFunctionsVectorPartitionFunction::vectorPartitionFunctionFormula
(Calculator& calculator, const Expression& input, Expression& output) {
  STACK_TRACE(
    "CalculatorFunctionsVectorPartitionFunction::"
    "vectorPartitionFunctionFormula"
  );
  Vectors<Rational> vectors;
  Matrix<Rational> matrix;
  if (
    !CalculatorConversions::functionGetMatrix(
      calculator, input, matrix, false
    )
  ) {
    return calculator << "Failed to extract matrix of rationals. ";
  }
  if (
    calculator.objectContainer.vectorPartitionFunctions.contains(vectors)
  ) {
    return
    output.assignValue(
      calculator,
      calculator.objectContainer.vectorPartitionFunctions.getValueNoFail(
        vectors
      )
    );
  }
  matrix.getVectorsFromRows(vectors);
  PartialFractions& result =
  calculator.objectContainer.vectorPartitionFunctions.getValueCreateEmpty(
    vectors
  );
  result.initializeAndSplit(vectors, &calculator.comments);
  result.computeAllVectorPartitionFunctions();
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsVectorPartitionFunction::coneDecomposition(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsVectorPartitionFunction::coneDecomposition");
  Vectors<Rational> vectors;
  Matrix<Rational> matrix;
  if (
    !CalculatorConversions::functionGetMatrix(
      calculator, input, matrix, false
    )
  ) {
    return calculator << "Failed to extract matrix of rationals. ";
  }
  matrix.getVectorsFromRows(vectors);
  ConeCollection chambers;
  chambers.initializeFromDirectionsAndRefine(vectors);
  std::stringstream out;
  out << chambers.toHTML();
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsVectorPartitionFunction::
applyVectorPartitionFunctionFormula(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsVectorPartitionFunction::"
    "applyVectorPartitionFunctionFormula"
  );
  Vector<Rational> vector;
  if (
    !calculator.getVectorFromFunctionArguments(input, vector, nullptr, - 1)
  ) {
    return false;
  }
  WithContext<PartialFractions> element;
  if (!input[0].isOfTypeWithContext(&element)) {
    return false;
  }
  PartialFractions partialFractions = element.content;
  Rational result;
  partialFractions.evaluateVectorPartitionFunction(vector, result, &calculator.comments);
  return output.assignValue(calculator, result);
}
