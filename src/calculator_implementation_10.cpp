#include "calculator_inner_functions_vector_partition_function_1.h"

bool CalculatorFunctionsVectorPartitionFunction::
vectorPartitionFunctionFormulaElementary(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsVectorPartitionFunction::"
    "vectorPartitionFunctionFormulaElementary"
  );
  Vectors<Rational> vectors;
  Matrix<Rational> matrix;
  if (
    !CalculatorConversions::functionGetMatrix(
      calculator, input, matrix, false
    )
  ) {
    return
    calculator
    << "Failed to extract matrix of rationals from: "
    << input.toString();
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
  VectorPartitionFunction& result =
  calculator.objectContainer.vectorPartitionFunctions.getValueCreateEmpty(
    vectors
  );
  result.initializeVectors(vectors);
  result.elementaryMethod.flagInitialized = true;
  return output.assignValue(calculator, result);
}

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
    return
    calculator
    << "Failed to extract matrix of rationals from: "
    << input.toString();
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
  VectorPartitionFunction& result =
  calculator.objectContainer.vectorPartitionFunctions.getValueCreateEmpty(
    vectors
  );
  result.fractions.initializeAndSplit(vectors, &calculator.comments);
  result.fractions.computeAllVectorPartitionFunctions();
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsVectorPartitionFunction::coneDecomposition(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool flagUseSpannedSlices,
  bool flagIncludeHistory
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
  chambers.flagUseSpannedSlices = flagUseSpannedSlices;
  chambers.initializeFromDirectionsAndRefine(vectors);
  std::stringstream out;
  if (flagIncludeHistory) {
    out << chambers.toHTMLHistory();
  }
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
    return calculator << "Failed to extract vector from: " << input.toString();
  }
  WithContext<VectorPartitionFunction> element;
  if (!input[0].isOfTypeWithContext(&element)) {
    return false;
  }
  PartialFractions partialFractions = element.content.fractions;
  if (partialFractions.ambientDimension != vector.size) {
    return
    calculator
    << "Vector dimension: "
    << vector.size
    << " not equal to the partial fraction dimension: "
    << partialFractions.ambientDimension
    << ".";
  }
  Rational result;
  partialFractions.evaluateVectorPartitionFunction(
    vector, result, &calculator.comments
  );
  return output.assignValue(calculator, result);
}
