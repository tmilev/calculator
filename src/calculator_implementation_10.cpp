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
  result.elementaryMethod.comments.flagGenerateComments = true;
  result.elementaryMethod.compute();
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsVectorPartitionFunction::
vectorPartitionFunctionFormulaWithDetails(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return
  CalculatorFunctionsVectorPartitionFunction::
  vectorPartitionFunctionFormulaInternal(calculator, input, output, true);
}

bool CalculatorFunctionsVectorPartitionFunction::vectorPartitionFunctionFormula
(Calculator& calculator, const Expression& input, Expression& output) {
  return
  CalculatorFunctionsVectorPartitionFunction::
  vectorPartitionFunctionFormulaInternal(calculator, input, output, false);
}

bool CalculatorFunctionsVectorPartitionFunction::
vectorPartitionFunctionFormulaInternal(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool flagShowDetails
) {
  STACK_TRACE(
    "CalculatorFunctionsVectorPartitionFunction::"
    "vectorPartitionFunctionFormula"
  );
  Vectors<Rational> vectors;
  std::string label;
  if (
    !CalculatorFunctionsVectorPartitionFunction::getVectorsForConeDecomposition
    (calculator, input, vectors, label)
  ) {
    return false;
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
  VectorPartitionFunction& result =
  calculator.objectContainer.vectorPartitionFunctions.getValueCreateEmpty(
    vectors
  );
  result.fractions.flagShowDetails = flagShowDetails;
  result.fractions.label = label;
  result.fractions.chambers.flagUseSpannedSlices = true;
  result.fractions.chambers.flagAmalgamateChambers = true;
  result.fractions.initializeAndSplit(vectors, &calculator.comments);
  result.fractions.computeAllVectorPartitionFunctions();
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsVectorPartitionFunction::getVectorsForConeDecomposition
(
  Calculator& calculator,
  const Expression& input,
  Vectors<Rational>& output,
  std::string& outputLabel
) {
  Matrix<Rational> matrix;
  outputLabel = "";
  if (
    CalculatorConversions::functionGetMatrix(
      calculator, input, matrix, false
    )
  ) {
    matrix.getVectorsFromRows(output);
    return true;
  }
  if (input.size() == 2) {
    if (
      CalculatorFunctionsVectorPartitionFunction::
      getVectorsForConeDecompositionFromDynkinType(
        calculator, input[1], output, outputLabel
      )
    ) {
      return true;
    }
  }
  return calculator << "Failed to extract matrix of rationals. ";
}

bool CalculatorFunctionsVectorPartitionFunction::
getVectorsForConeDecompositionFromDynkinType(
  Calculator& calculator,
  const Expression& input,
  Vectors<Rational>& output,
  std::string& outputLabel
) {
  STACK_TRACE(
    "CalculatorFunctionsVectorPartitionFunction::"
    "getVectorsForConeDecompositionFromDynkinType"
  );
  DynkinSimpleType type;
  if (
    !CalculatorConversions::functionDynkinSimpleType(calculator, input, type)
  ) {
    return false;
  }
  WeylGroupData weylGroup;
  weylGroup.makeArbitrarySimple(type.letter, type.rank);
  weylGroup.computeRootsOfBorel(output);
  FormatExpressions format;
  format.flagSupressDynkinIndexOne = true;
  outputLabel = "\\(" + weylGroup.dynkinType.toString(&format) + "\\)";
  return true;
}

bool CalculatorFunctionsVectorPartitionFunction::coneDecomposition(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool flagUseSpannedSlices,
  bool flagIncludeHistory,
  bool flagAmalgamateChambers
) {
  STACK_TRACE("CalculatorFunctionsVectorPartitionFunction::coneDecomposition");
  Vectors<Rational> vectors;
  std::string label;
  if (
    !CalculatorFunctionsVectorPartitionFunction::getVectorsForConeDecomposition
    (calculator, input, vectors, label)
  ) {
    return false;
  }
  ConeCollection chambers;
  chambers.flagUseSpannedSlices = flagUseSpannedSlices;
  chambers.flagAmalgamateChambers = flagAmalgamateChambers;
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
  partialFractions.evaluateVectorPartitionFunctionChecked(
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

bool CalculatorFunctionsVectorPartitionFunction::
subLatticeScalarProductTimesDirectionInLattice(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsVectorPartitionFunction::"
    "subLatticeWithIntegralScalarProducts"
  );
  if (input.size() != 5) {
    return
    calculator
    << "subLatticeWithIntegralScalarProducts expects 4 inputs: "
    << "lattice, lattice, vector and vector.";
  }
  Vector<Rational> scalarProductWith;
  Vector<Rational> direction;
  Lattice lattice;
  Lattice target;
  if (!input[1].isOfType(&lattice)) {
    return calculator << "Couldn't get lattice from: " << input[1] << ".";
  }
  if (!input[2].isOfType(&target)) {
    return calculator << "Couldn't get lattice from: " << input[2] << ".";
  }
  if (!calculator.getVector(input[3], scalarProductWith)) {
    return calculator << "Couldn't get vector from: " << input[3] << ".";
  }
  if (!calculator.getVector(input[4], direction)) {
    return calculator << "Couldn't get vector from: " << input[4] << ".";
  }
  int dimension = scalarProductWith.size;
  if (
    !lattice.basisRationalForm.isSquare() ||
    !target.basisRationalForm.isSquare()
  ) {
    return calculator << "Need full rank lattices. ";
  }
  if (
    direction.size != dimension ||
    lattice.getDimension() != dimension ||
    target.getDimension() != dimension
  ) {
    return calculator << "Dimensions don't match.";
  }
  Lattice result;
  lattice.subLatticeScalarProductTimesDirectionInLattice(
    scalarProductWith, direction, target, result
  );
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

bool CalculatorFunctionsVectorPartitionFunction::reduceModuloLattice(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsVectorPartitionFunction::reduceModuloLattice"
  );
  if (input.size() != 3) {
    return false;
  }
  Vector<Rational> vector;
  if (!calculator.getVector(input[1], vector)) {
    return false;
  }
  Lattice lattice;
  if (!input[2].isOfType(&lattice)) {
    return false;
  }
  if (lattice.getDimension() != vector.size || vector.size < 1) {
    return calculator << "Dimensions don't match. ";
  }
  if (!lattice.reduceVector(vector)) {
    return
    calculator
    << "Failed to reduce vector: "
    << vector
    << " mod lattice: "
    << lattice.toString();
  }
  return
  CalculatorConversions::expressionFromVectorRational(
    calculator, vector, output
  );
}

bool CalculatorFunctionsVectorPartitionFunction::quotientLatticeRepresentatives
(Calculator& calculator, const Expression& input, Expression& output) {
  STACK_TRACE(
    "CalculatorFunctionsVectorPartitionFunction::"
    "quotientLatticeRepresentatives"
  );
  if (input.size() != 3) {
    return false;
  }
  Lattice dividend;
  Lattice divisor;
  if (
    !input[1].isOfType(&dividend) || !input[2].isOfType(&divisor)
  ) {
    return
    calculator
    << "Could not extract lattice arguments from "
    << input.toString();
  }
  if (
    dividend.getDimension() != divisor.getDimension() ||
    dividend.getRank() != dividend.getDimension()
  ) {
    return
    calculator
    << "Both lattices must be of full rank and equal dimension";
  }
  if (!dividend.containsLattice(divisor)) {
    return
    calculator
    << "The divisor "
    << divisor.toString()
    << " is not a sub-lattice of the dividend "
    << dividend.toString()
    << ".";
  }
  Vectors<Rational> representatives;
  if (!dividend.getAllRepresentatives(divisor, representatives)) {
    return
    calculator
    << "Failed to get all representatives. "
    << "Perhaps they are too many?";
  }
  Matrix<Rational> result;
  result.assignVectorsToRows(representatives);
  return output.makeMatrix(calculator, result);
}

bool CalculatorFunctionsVectorPartitionFunction::kostantPartitionFunctionLatex(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctionsVectorPartitionFunction::"
    "kostantPartitionFunctionLatex"
  );
  List<std::string> labels;
  List<Vectors<Rational> > vectors;
  labels.setSize(input.size() - 1);
  vectors.setSize(input.size() - 1);
  for (int i = 1; i < input.size(); i ++) {
    if (
      !CalculatorFunctionsVectorPartitionFunction::
      getVectorsForConeDecompositionFromDynkinType(
        calculator, input[i], vectors[i - 1], labels[i - 1]
      )
    ) {
      return calculator << "Failed to extract dynkin type from " << input[i];
    }
  }
  std::stringstream latex;
  FormatExpressions format;
  format.flagUseFrac = true;
  format.flagSuppressOneIn1overXtimesY = true;
  for (int i = 0; i < vectors.size; i ++) {
    PartialFractions computation;
    computation.label = labels[i];
    computation.initializeAndSplit(vectors[i], nullptr);
    computation.chambers.flagAmalgamateChambers = true;
    computation.chambers.flagUseSpannedSlices = true;
    computation.computeAllVectorPartitionFunctions();
    latex << computation.toLatexSelfContainedDocumentBody();
  }
  return
  output.assignValue(
    calculator, HtmlRoutines::toHtmlLatexLiteralWithCopy(latex.str())
  );
}
