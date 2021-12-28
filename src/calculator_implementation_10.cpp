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
  if (CalculatorConversions::functionGetMatrix(calculator, input, matrix, false)) {

  }
  global.comments << "DEBUG: got matrix so far: " << matrix.toString() << ". ";
  global.fatal << "Not implemented yet. " << global.fatal;
  return false;
}
