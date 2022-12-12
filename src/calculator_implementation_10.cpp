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
  result.elementaryMethod.compute();
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

bool CalculatorFunctionsVectorPartitionFunction::bernoulliSum(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsVectorPartitionFunction::bernouliSum");
  if (input.size() != 3) {
    return false;
  }
  int power = 0;
  if (!input[1].isSmallInteger(&power)) {
    return
    calculator
    << "First argument of "
    << input
    << " is not a small integer.";
  }
  if (power < 0) {
    return calculator << "First argument of " << input << " is negative. ";
  }
  if (power > 300) {
    return
    calculator
    << "The maximum power allowed is 300, yours is: "
    << power
    << ". ";
  }
  BernoulliSumComputer computer;
  WithContext<Polynomial<Rational> > result;
  result.context.addVariable(input[2]);
  computer.getBernoulliSumStartingAtOne(power, result.content);
  return result.toExpression(calculator, output);
}

bool CalculatorFunctionsVectorPartitionFunction::lattice(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsVectorPartitionFunction::lattice");
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
  Lattice lattice;
  lattice.makeFromMatrix(matrix);
  return output.assignValue(calculator, lattice);
}

bool CalculatorFunctionsVectorPartitionFunction::
subLatticeWithIntegralScalarProducts(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsVectorPartitionFunction::"
    "subLatticeWithIntegralScalarProducts"
  );
  if (input.size() != 3) {
    return
    calculator
    << "subLatticeWithIntegralScalarProducts expects 2 inputs, "
    << "lattice and vector.";
  }
  Vector<Rational> shift;
  Lattice lattice;
  if (!input[2].isOfType(&lattice)) {
    return
    calculator
    << "Second argument of subLatticeWithIntegralScalarProducts "
    << "expected to be a lattice.";
  }
  if (!calculator.getVector(input[1], shift)) {
    return
    calculator
    << "First argument of subLatticeWithIntegralScalarProducts "
    << "expected to be a vector.";
  }
  if (shift.size != lattice.getDimension()) {
    return
    calculator
    << "Lattice is in dimensino "
    << lattice.getDimension()
    << " but the shift has "
    << shift.size
    << " coordinates. ";
  }
  Lattice result;
  lattice.subLatticeWithIntegralScalarProducts(shift, result);
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsVectorPartitionFunction::dualLattice(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsVectorPartitionFunction::dualLattice");
  if (input.size() != 2) {
    return false;
  }
  Lattice lattice;
  if (!input[1].isOfType(&lattice)) {
    return false;
  }
  Lattice dual;
  if (!lattice.getDualLattice(dual)) {
    return calculator << "Couldn't compute the dual lattice. ";
  }
  return output.assignValue(calculator, dual);
}
