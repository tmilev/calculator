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
  global.comments << "DEBUG: init and split: " << result.originalVectors.toString()
  << ", hull: " << result.chambers.convexHull.toString() << "<hr>";
  result.computeAllVectorPartitionFunctions();
  global.comments << "DEBUG: compute all: " << result.originalVectors.toString()
  << ", hull: " << result.chambers.convexHull.toString() << "<hr>";
   output.assignValue(calculator, result);
   global.comments << "DEBUG: after assign: " << output.getValue<PartialFractions>().chambers.convexHull.toString() ;
   global.comments << "DEBUG: after assign: " << output.getValue<PartialFractions>().originalVectors.toString() << "<hr>";

   return true;
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
  partialFractions.evaluateVectorPartitionFunction(vector, result);
  return output.assignValue(calculator, result);
}
