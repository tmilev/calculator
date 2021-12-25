// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "calculator_interface.h"
#include "calculator_inner_functions.h"
#include "calculator_inner_typed_functions.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_extra_modules_semisimple_lie_algebras_implementation.h"
#include "math_extra_semisimple_lie_algebras_implementation.h"
#include "math_general.h"
#include "math_rational_function_implementation.h"
#include "math_extra_weyl_algebras_implementation.h"

template<class Element>
bool Matrix<Element>::systemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(
  Matrix<Element>& matA, Matrix<Element>& matb, Vector<Element>* outputSolution
) {
  // this function return true if Ax = b >= 0 has a solution with x >= 0 and records a solution x at outputPoint
  // else returns false, where b is a given nonnegative column vector, A is an n by m matrix
  // and x is a column vector with m entries
  Matrix<Rational> workingMatrix;
  Vector<Rational> matX;
  Selection BaseVariables;
  Rational GlobalGoal;
  GlobalGoal.makeZero();
  if (matA.numberOfRows != matb.numberOfRows) {
    global.fatal << "The number of inequalities: "
    << matA.numberOfRows << " does not match the number of "
    << "constaints: " << matb.numberOfRows << ". " << global.fatal;
  }
  for (int j = 0; j < matb.numberOfRows; j ++) {
    GlobalGoal += matb.elements[j][0];
    if (matb.elements[j][0].isNegative()) {
      global.fatal << "Constraint index " << j << " is negative: "
      << matb.elements[j][0] << " which is not allowed. " << global.fatal;
    }
  }
  if (GlobalGoal.isEqualToZero()) {
    return false;
  }
  int numberOfTrueVariables = matA.numberOfColumns;
  workingMatrix.initialize(matA.numberOfRows, numberOfTrueVariables + matA.numberOfRows);
  HashedList<Selection> VisitedVertices;
  VisitedVertices.clear();
  BaseVariables.initialize(workingMatrix.numberOfColumns);
  workingMatrix.makeZero();
  matX.makeZero(workingMatrix.numberOfColumns);
  for (int j = 0; j < matA.numberOfColumns; j ++) {
    for (int i = 0; i < matA.numberOfRows; i ++) {
      workingMatrix.elements[i][j].assign(matA.elements[i][j]);
    }
  }
  for (int j = 0; j < matA.numberOfRows; j ++) {
    workingMatrix.elements[j][j + numberOfTrueVariables].makeOne();
    matX[j + numberOfTrueVariables] = (matb.elements[j][0]);
    BaseVariables.addSelectionAppendNewIndex(j + numberOfTrueVariables);
  }
  Rational ChangeGradient; //Change, PotentialChange;
  int EnteringVariable = 0;
  bool WeHaveNotEnteredACycle = true;
  while (EnteringVariable != - 1 && WeHaveNotEnteredACycle && GlobalGoal.isPositive()) {
    EnteringVariable = - 1; ChangeGradient.makeZero();
    for (int i = 0; i < workingMatrix.numberOfColumns; i ++) {
      if (!BaseVariables.selected[i]) {
        Rational PotentialChangeGradient; bool hasAPotentialLeavingVariable;
        Matrix<Rational>::computePotentialChangeGradient(
          workingMatrix, BaseVariables, numberOfTrueVariables, i, PotentialChangeGradient, hasAPotentialLeavingVariable
        );
        if (PotentialChangeGradient.isGreaterThanOrEqualTo(ChangeGradient) && hasAPotentialLeavingVariable) {
          EnteringVariable = i;
          ChangeGradient.assign(PotentialChangeGradient);
        }
      }
    }
    if (EnteringVariable != - 1) {
      int LeavingVariableRow;
      Rational MaxMovement;
      Matrix<Rational>::getMaxMovementAndLeavingVariableRow(
        MaxMovement, LeavingVariableRow, EnteringVariable, workingMatrix, matX, BaseVariables
      );
      Rational tempRat, tempTotalChange;
      if (workingMatrix.elements[LeavingVariableRow][EnteringVariable].isEqualToZero()) {
        global.fatal << "The leaving-entering coefficient is not allowed to be zero. " << global.fatal;
      }
      tempRat.assign(workingMatrix.elements[LeavingVariableRow][EnteringVariable]);
      tempRat.invert();
      for (int i = 0; i < workingMatrix.numberOfRows; i ++) {
        if (!workingMatrix.elements[i][BaseVariables.elements[i]].isEqualTo(1)) {
          global.fatal << "The base variable coefficient is required to be 1 at this point of code. "
          << global.fatal;
        }
      }
      workingMatrix.rowTimesScalar(LeavingVariableRow, tempRat);
      tempTotalChange.assign(MaxMovement);
      tempTotalChange.multiplyBy(ChangeGradient);
      matX[EnteringVariable] += MaxMovement;
      if (!tempTotalChange.isEqualToZero()) {
        VisitedVertices.clear();
        GlobalGoal.subtract(tempTotalChange);
      } else {
        int tempI = VisitedVertices.getIndex(BaseVariables);
        if (tempI == - 1) {
          VisitedVertices.addOnTop(BaseVariables);
        } else {
          WeHaveNotEnteredACycle = false;
        }
      }
      for (int i = 0; i < workingMatrix.numberOfRows; i ++) {
        if (!workingMatrix.elements[i][EnteringVariable].isEqualToZero()&& i != LeavingVariableRow) {
          tempRat.assign(workingMatrix.elements[i][EnteringVariable]);
          tempRat.multiplyBy(MaxMovement);
          matX[BaseVariables.elements[i]] -= tempRat;
          tempRat.assign(workingMatrix.elements[i][EnteringVariable]);
          tempRat.negate();
          workingMatrix.addTwoRows(LeavingVariableRow, i, 0, tempRat);
        }
        if (i == LeavingVariableRow) {
          matX[BaseVariables.elements[i]] = 0;
        }
      }
      if (!matX[BaseVariables.elements[LeavingVariableRow]].isEqualToZero()) {
        global.fatal << "Leaving variable coefficient not allowed to be zero. " << global.fatal;
      }
      BaseVariables.selected[BaseVariables.elements[LeavingVariableRow]] = false;
      BaseVariables.elements[LeavingVariableRow] = EnteringVariable;
      BaseVariables.selected[EnteringVariable] = true;
      //BaseVariables.ComputeDebugString();
      for (int i = 0; i < workingMatrix.numberOfRows; i ++) {
        if (!workingMatrix.elements[i][BaseVariables.elements[i]].isEqualTo(1)) {
          global.fatal << "New base variable expected to be equal to 1. " << global.fatal;
        }
      }
    }
  }
  for (int i = numberOfTrueVariables; i < matX.size; i ++) {
    if (matX[i].isPositive()) {
      return false;
    }
  }
  if (outputSolution != nullptr) {
    outputSolution->setSize(numberOfTrueVariables);
    for (int i = 0; i < numberOfTrueVariables; i ++) {
      (*outputSolution)[i] = matX[i];
    }
  }
  return true;
}

bool CalculatorFunctions::attemptExtendingEtoHEFwithHinCartan(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerAttemptExtendingEtoHEFwithHinCartan");
  if (input.size() != 3) {
    return output.assignError(calculator, "Function takes 2 arguments - type and an element of the Lie algebra.");
  }
  WithContext<SemisimpleLieAlgebra*> ownerAlgebra;
  if (!CalculatorConversions::convert(
    calculator,
    input[1],
    ownerAlgebra
  )) {
    return output.assignError(calculator, "Error extracting Lie algebra.");
  }
  SemisimpleLieAlgebra* ownerSemisimple = ownerAlgebra.content;
  ElementSemisimpleLieAlgebra<Rational> rationalExpression;
  if (!CalculatorConversions::loadElementSemisimpleLieAlgebraRationalCoefficients(
    calculator, input[2], rationalExpression, *ownerSemisimple
  )) {
    return output.assignError(calculator, "Failed to extract element of semisimple Lie algebra. ");
  }
  ElementSemisimpleLieAlgebra<AlgebraicNumber> fElement;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> hElement;
  ElementSemisimpleLieAlgebra<AlgebraicNumber> eElement;
  eElement = rationalExpression;
  std::stringstream out, logStream;
  bool success = ownerSemisimple->attemptExtendingEtoHEFwithHinCartan(eElement, hElement, fElement, &logStream);
  if (success) {
    out << HtmlRoutines::getMathNoDisplay("F=" + fElement.toString() + ";")
    << "<br>" << HtmlRoutines::getMathNoDisplay("H=" + hElement.toString() + ";") << "<br>"
    << HtmlRoutines::getMathNoDisplay("E=" + eElement.toString() + ";")
    << "<br><br>The log stream of the computation follows. " << logStream.str();
  } else {
    out << "<br>Couldn't extend E to sl(2)-triple. The log stream follows. " << logStream.str();
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctions::zModP(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerZmodP");
  if (!input.isListNElements(3)) {
    return false;
  }
  Rational left, right;
  if (!input[1].isRational(&left) || !input[2].isRational(&right)) {
    return false;
  }
  LargeInteger base;
  if (!right.isInteger(&base)) {
    return false;
  }
  if (base.isEqualToZero()) {
    return false;
  }
  LargeIntegerUnsigned greatestCommonDivisor;
  LargeIntegerUnsigned::greatestCommonDivisor(left.getDenominator(), base.value, greatestCommonDivisor);
  if (greatestCommonDivisor > 1) {
    return false;
  }
  ElementZmodP outputElement;
  outputElement.modulus = base.value;
  outputElement = left.getNumerator();
  ExpressionContext context;
  context.initialize(calculator);
  context.setDefaultModulus(outputElement.modulus);
  return output.assignValueWithContext(calculator, outputElement, context);
}

bool CalculatorFunctions::conesIntersect(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerConesIntersect");
  if (!input.isListNElements(3)) {
    return calculator << "Function ConesIntersection expects 2 arguments, got "
    << input.size() - 1 << " instead. ";
  }
  Matrix<Rational> coneNonStrictMatForm;
  Matrix<Rational> coneStrictMatForm;
  Vectors<Rational> coneNonStrictGens;
  Vectors<Rational> coneStrictGens;
  if (!calculator.functionGetMatrixNoComputation(input[1], coneStrictMatForm)) {
    return calculator << "Failed to extract matrix from the first argument, " << input[1].toString();
  }
  if (!calculator.functionGetMatrixNoComputation(input[2], coneNonStrictMatForm)) {
    return calculator << "Failed to extract matrix from the second argument, " << input[2].toString();
  }
  std::stringstream out;
  if (coneNonStrictMatForm.numberOfColumns != coneStrictMatForm.numberOfColumns) {
    out << "I got as input vectors of different dimensions, "
    << "first groups had vectors of dimension "
    << coneNonStrictMatForm.numberOfColumns
    << " and second of dimension "
    << coneStrictMatForm.numberOfColumns << " which is not allowed. ";
    return output.assignError(calculator, out.str());
  }
  coneNonStrictMatForm.getVectorsFromRows(coneNonStrictGens);
  coneStrictMatForm.getVectorsFromRows(coneStrictGens);
  out << "<br>Input non-strict (i.e., over Z_{&gt;= 0}) cone: ";
  for (int i = 0; i < coneStrictGens.size; i ++) {
    out << "<br>v_{" << i + 1 << "}=" << coneStrictGens[i].toString() << ";";
  }
  out << "<br>Input strict (i.e., over Z_{&gt;0}) cone: ";
  for (int i = 0; i < coneNonStrictGens.size; i ++) {
    out << "<br>v_{" << coneStrictGens.size + i + 1 << "}=" << coneNonStrictGens[i].toString() << ";";
  }
  Vector<Rational> outputIntersection, outputSeparatingNormal;
  bool conesDoIntersect = coneNonStrictGens.conesIntersect(
    coneStrictGens, coneNonStrictGens, &outputIntersection, &outputSeparatingNormal
  );
  if (conesDoIntersect) {
    Vector<Rational> checkVector;
    checkVector.makeZero(coneStrictMatForm.numberOfColumns);
    for (int i = 0; i < coneStrictGens.size; i ++) {
      checkVector += coneStrictGens[i] * outputIntersection[i];
    }
    for (int i = 0; i < coneNonStrictGens.size; i ++) {
      checkVector += coneNonStrictGens[i] * outputIntersection[coneStrictGens.size + i];
    }
    if (!checkVector.isEqualToZero()) {
      global.fatal << "<br>The output linear combination " << outputIntersection.toString()
      << " corresponds to the cone intersection " << checkVector.toString()
      << " and is not equal to zero! Here is the cone output so far: "
      << out.str() << global.fatal;
    }
    out << "<br>Cones intersect, here is one intersection: 0= " << outputIntersection.toStringLetterFormat("v");
  } else {
    out << "<br>Cones have empty intersection.";
    out << "<br>A normal separating the cones is: n =" << outputSeparatingNormal.toString() << ". Indeed, ";
    for (int i = 0; i < coneStrictGens.size; i ++) {
      out << "<br>\\langle v_{" << i + 1 << "}, n\\rangle = "
      << outputSeparatingNormal.scalarEuclidean(coneStrictGens[i]).toString();
    }
    for (int i = 0; i < coneNonStrictGens.size; i ++) {
      out << "<br>\\langle v_{" << i + 1 + coneStrictGens.size << "}, n\\rangle = "
      << outputSeparatingNormal.scalarEuclidean(coneNonStrictGens[i]).toString();
    }
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctions::reverseOrderRecursively(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerReverseOrderRecursively");
  if (input.size() < 2) {
    return false;
  }
  Expression toReverse;
  if (input.size() == 2) {
    toReverse = input[1];
  } else {
    toReverse = input;
    toReverse.setChildAtomValue(0, calculator.opSequence());
  }
  return calculator.functionReverseOrderRecursively(calculator, toReverse, output);
}

bool Calculator::functionReverseOrderRecursively(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (!input.isSuitableForRecursion()) {
    output = input;
    return true;
  }
  output.reset(calculator, input.size());
  output.addChildOnTop(input[0]);
  for (int i = input.size() - 1; i >= 1; i --) {
    Expression currentE = input[i];
    Expression reversedCurrentE;
    if (!calculator.functionReverseOrderRecursively(calculator,  currentE, reversedCurrentE)) {
      return false;
    }
    output.addChildOnTop(reversedCurrentE);
  }
  return true;
}

bool CalculatorFunctions::reverseOrder(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerReverseOrder");
  if (!input.isSuitableForRecursion()) {
    output = input;
    return true;
  }
  if (input.size() < 2) {
    return false;
  }
  Expression toReverse;
  if (input.size() == 2) {
    toReverse = input[1];
  } else {
    toReverse = input;
    toReverse.setChildAtomValue(0, calculator.opSequence());
  }
  output.reset(calculator, toReverse.size());
  output.addChildOnTop(toReverse[0]);
  for (int i = toReverse.size() - 1; i >= 1; i --) {
    output.addChildOnTop(toReverse[i]);
  }
  return true;
}

bool CalculatorFunctions::notFunction(Calculator& calculator, const Expression& input, Expression& output) {
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  int theInt;
  if (!argument.isSmallInteger(&theInt)) {
    return false;
  }
  if (theInt == 0) {
    return output.assignValue(calculator, 1);
  }
  return output.assignValue(calculator, 0);
}

bool CalculatorFunctions::printZnEnumeration(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::printZnEnumeration");
  if (!input.isListNElements(3)) {
    return false;
  }
  int grade, dimension;
  if (!input[2].isSmallInteger(&grade) || !input[1].isSmallInteger(&dimension)) {
    return false;
  }
  if (grade > 10 || dimension > 5 || grade < 0 || dimension < 0) {
    return false;
  }
  SelectionPositiveIntegers selection;
  selection.initialize(dimension);
  std::stringstream out2, out;
  LargeIntegerUnsigned gradeLarge = static_cast<unsigned>(grade);
  int counter = 0;
  for (
    selection.setFirstInGradeLevel(gradeLarge);
    selection.getGrading() == gradeLarge;
    selection.incrementReturnFalseIfPastLast()
  ) {
    out2 << selection.toString() << "<br>";
    counter ++;
  }
  out << "Total " << counter << " vectors:<br>" << out2.str();
  return output.assignValue(calculator, out.str());
}

template <>
void Vector<Rational>::perturbNormalRelativeToVectorsInGeneralPosition(
  const Vectors<Rational>& nonStrictConeNonPositiveScalar,
  const List<Vector<Rational> >& vectorsToBeInGeneralPosition
);

bool CalculatorFunctions::perturbSplittingNormal(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::perturbSplittingNormal");
  std::stringstream out;
  if (input.size() != 4) {
    out << "Perturbing splitting normal takes 3 arguments: normal, "
    << "positive vectors, and vectors relative to which to perturb. "
    << "Instead I got " << input.size() - 1 << ". ";
    return output.assignError(calculator, out.str());
  }
  Vector<Rational> splittingNormal;
  if (!calculator.getVector(input[1], splittingNormal, nullptr)) {
    return output.assignError(calculator, "Failed to extract normal from first argument. ");
  }
  Matrix<Rational> matrix;
  Vectors<Rational> nonStrictCone, vectorsToPerturbRelativeTo;
  if (!calculator.functionGetMatrix(
    input[2], matrix, false, nullptr, splittingNormal.size, nullptr
  )) {
    return output.assignError(calculator, "Failed to extract matrix from second argument. ");
  }
  nonStrictCone.assignMatrixRows(matrix);
  if (!calculator.functionGetMatrix(
    input[3], matrix, false, nullptr, splittingNormal.size, nullptr
  )) {
    return output.assignError(calculator, "Failed to extract matrix from third argument. ");
  }
  vectorsToPerturbRelativeTo.assignMatrixRows(matrix);
  for (int i = 0; i < nonStrictCone.size; i ++) {
    if (splittingNormal.scalarEuclidean(nonStrictCone[i]) < 0) {
      std::stringstream out;
      out << "The normal vector " << splittingNormal.toString()
      << " is has negative scalar product with " << nonStrictCone[i].toString();
      return output.assignError(calculator, out.str());
    }
  }
  out << "Perturbing " << splittingNormal.toString() << " relative to cone "
  << nonStrictCone.toString() << " and vectors " << vectorsToPerturbRelativeTo.toString();
  splittingNormal.perturbNormalRelativeToVectorsInGeneralPosition(nonStrictCone, vectorsToPerturbRelativeTo);
  out << "<br>End result: " << splittingNormal.toString();
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctions::printAllVectorPartitions(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::printAllVectorPartitions");
  RecursionDepthCounter theRecursion(&calculator.recursionDepth);
  if (input.size() != 3) {
    return output.assignError(calculator, "Function printAllPartitions expects 2 arguments.");
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!CalculatorConversions::convert(
    calculator,
    input[1],
    algebra
  )) {
    return output.assignError(calculator, "Error extracting Lie algebra.");
  }
  SemisimpleLieAlgebra* semisimpleLieAlgebraPointer = algebra.content;

  SemisimpleLieAlgebra& semisimpleLieAlgebra = *semisimpleLieAlgebraPointer;
  ExpressionContext context(calculator);
  Vector<Rational> theHW;
  if (!calculator.getVector<Rational>(input[2], theHW, &context, semisimpleLieAlgebra.getRank())) {
    return output.assignError(calculator, "Failed to extract weight you want partitioned from " + input[2].toString());
  }
  Vector<int> theHWint;
  theHWint.setSize(theHW.size);
  for (int i = 0; i < theHW.size; i ++) {
    if (!theHW[i].isSmallInteger(&theHWint[i]) || theHW[i] < 0) {
      return output.assignError(
        calculator,
        "The input weight you gave is bad: "
        "it must consist of non-negative small integers"
      );
    }
  }
  std::stringstream out;
  out << "<br>the weight you want partitioned: " << theHWint;
  Vector<int> thePartition;
  thePartition.setSize(semisimpleLieAlgebra.getNumberOfPositiveRoots());
  for (int i = 0; i < thePartition.size; i ++) {
    thePartition[i] = 0;
  }
  Vector<Rational> weight, tmpWt;
  Vectors<Rational>& rootsBorel = semisimpleLieAlgebra.weylGroup.rootsOfBorel;
  int counter = 0;
  int totalCycles = 0;
  weight.makeZero(semisimpleLieAlgebra.getRank());
  int i = rootsBorel.size;
  while (i > 0 && counter < 10000) {
    totalCycles ++;
    if (weight == theHW) {
      tmpWt = thePartition;
      out << "<br>" << tmpWt.toStringLetterFormat("\\alpha");
      counter ++;
    }
    if (!(theHW - weight).isPositive() || i > rootsBorel.size) {
      if (i <= rootsBorel.size) {
        weight -= rootsBorel[i - 1] * thePartition[i - 1];
        thePartition[i - 1] = 0;
      }
      i --;
      if (i > 0) {
        weight += rootsBorel[i - 1];
        thePartition[i - 1] ++;
      }
    } else {
      i ++;
    }
  }
  out << "<br>Done in " << totalCycles << " cycles.";
  out << "<br>" << counter << " total partitions ";
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctions::interpolatePolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::interpolatePolynomial");
  if (input.size() < 2) {
    return false;
  }
  Expression convertedE;
  if (!CalculatorConversions::makeMatrix(calculator, input, convertedE)) {
    return false;
  }
  Matrix<Rational> pointsOfInterpoly;
  if (!calculator.functionGetMatrix(
    convertedE, pointsOfInterpoly, false, nullptr, 2
  )) {
    return calculator
    << "<hr>Failed to extract points of interpolation from "
    << convertedE.toString();
  }
  Polynomial<Rational> interPoly;
  Vector<Rational> arguments, values;
  pointsOfInterpoly.getVectorFromColumn(0, arguments);
  pointsOfInterpoly.getVectorFromColumn(1, values);
  interPoly.interpolate(arguments, values);
  ExpressionContext context(calculator);
  context.makeOneVariableFromString("x");
  return output.assignValueWithContext(calculator, interPoly, context);
}

bool CalculatorFunctions::operationBinary(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  int operation
) {
  MemorySaving<Calculator::OperationHandlers>& theOperation = calculator.operations.values[operation];
  if (theOperation.isZeroPointer()) {
    return false;
  }
  List<Function>& handlers = theOperation.getElement().handlers;
  for (int i = 0; i < handlers.size; i ++) {
    if (handlers[i].inputFitsMyInnerType(input)) {
      if (handlers[i].functionAddress(calculator, input, output)) {
        return true;
      }
    }
  }
  return false;
}

bool CalculatorFunctions::elementWeylAlgebraOrPolynomial(
  Calculator& calculator, const Expression& input, Expression& output, bool assignPoly
) {
  MacroRegisterFunctionWithName("Calculator::elementWeylAlgebraOrPolynomial");
  if (!input.isListNElements(3)) {
    return false;
  }
  Vector<Polynomial<Rational> > inputPolynomialForm;
  ExpressionContext startContext(calculator);
  if (!calculator.getVectorFromFunctionArguments(
    input,
    inputPolynomialForm,
    &startContext,
    2
  )) {
    return calculator
    << "<hr>Failed to extract polynomials from arguments of "
    << input.toString();
  }
  int letterDifferential = 0;
  int letterPolynomial = 0;
  if (
    !inputPolynomialForm[0].isOneLetterFirstDegree(&letterDifferential) ||
    !inputPolynomialForm[1].isOneLetterFirstDegree(&letterPolynomial) ||
    letterDifferential == letterPolynomial
  ) {
    return calculator
    << "<hr>Failed to get different one-variable polynomials from input. "
    << input.toString();
  }
  ExpressionContext endContext(calculator);
  endContext.makeOneVariableOneDifferentialOperator(
    startContext.getVariable(letterPolynomial),
    startContext.getVariable(letterDifferential)
  );
  ElementWeylAlgebra<Rational> outputElementWeylAlgebra;
  if (assignPoly) {
    outputElementWeylAlgebra.makexi(0);
  } else {
    outputElementWeylAlgebra.makedi(0);
  }
  return output.assignValueWithContext(calculator, outputElementWeylAlgebra, endContext);
}

bool CalculatorBasics::extractBaseMultiplication(
  Calculator& calculator, const Expression& input, Expression& output
) {
  RecursionDepthCounter recursionIncrementer(&calculator.recursionDepth);
  MacroRegisterFunctionWithName("Calculator::extractBaseMultiplication");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  bool result = false;
  // handle Anything * Rational = Rational * Anything
  output = input;
  if (output[2].isOfType<Rational>()) {
    output.swapChildren(1, 2);
    result = true;
  }
  if (output[2].isOfType<double>() && !output[1].isOfType<Rational>()) {
    output.swapChildren(1, 2);
    result = true;
  }
  if (output[2].isListStartingWithAtom(calculator.opTimes())) {
    if (output[2].size() != 3) {
      return result;
    }
    // handle Anything1 * (Rational * Anything2) = Rational * (Anything1 * Anything2)
    if (output[2][1].isOfType<Rational>()) {
      Expression tempRight;
      tempRight.makeXOX(calculator, calculator.opTimes(), output[1], output[2][2]);
      output.makeXOX(calculator, calculator.opTimes(), output[2][1], tempRight);
      result = true;
    }
    // <- handle a * (b * anything)
    // on condition that a*b has an inner handler
    Expression tempExp, newExpr;
    tempExp.makeXOX(calculator, calculator.opTimes(), output[1], output[2][1]);
    if (CalculatorFunctions::times(calculator, tempExp, newExpr)) {
      output.makeProduct(calculator, newExpr, output[2][2]);
      result = true;
    }
  }
  // handle 0 * anything = 0
  if (output[1].isEqualToZero()) {
    return output.assignValue(calculator, 0);
  }
  return result;
}
