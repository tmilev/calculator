// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "calculator_inner_functions_vector_partition_function_1.h"

bool CalculatorFunctionsVectorPartitionFunction::vectorPartitionFunctionFormula(
  Calculator& calculator,
  const Expression& input,
  Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsVectorPartitionFunction::vectorPartitionFunctionFormula");
  Vectors<Rational> vectors;
  Matrix<Rational> matrix;
  if (!CalculatorConversions::functionGetMatrix(
    calculator, input, matrix, false
  )) {
    return calculator << "Failed to extract matrix of rationals. ";
  }
  matrix.getVectorsFromRows(vectors);
  PartialFractions partialFractions;
  partialFractions.initializeAndSplit(vectors, &calculator.comments);
  partialFractions.computeAllVectorPartitionFunctions();
  std::stringstream out;
  out << partialFractions.toHTML();
  out << "<br>Chambers:<br>" << partialFractions.chambers.toHTML();
  output.assignValue(calculator, out.str());

  return true;
}

bool CalculatorFunctionsVectorPartitionFunction::coneDecomposition(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsVectorPartitionFunction::coneDecomposition");
  Vectors<Rational> vectors;
  Matrix<Rational> matrix;
  if (!CalculatorConversions::functionGetMatrix(
    calculator, input, matrix, false
  )) {
    return calculator << "Failed to extract matrix of rationals. ";
  }
  matrix.getVectorsFromRows(vectors);
  ConeCollection chambers;
  chambers.initializeFromDirectionsAndRefine(vectors);
  std::stringstream out;
  out << chambers.toHTML();
  return output.assignValue(calculator, out.str());
}
