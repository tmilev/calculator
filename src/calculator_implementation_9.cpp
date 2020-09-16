// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "calculator_interface.h"
#include "calculator_inner_functions.h"
#include "calculator_inner_typed_functions.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_extra_modules_semisimple_Lie_algebras_implementation.h"
#include "math_extra_semisimple_Lie_algebras_implementation.h"
#include "math_general.h"
#include "math_rational_function_implementation.h"
#include "math_extra_Weyl_algebras_implementation.h"

template<class Element>
bool Matrix<Element>::systemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(
  Matrix<Element>& matA, Matrix<Element>& matb, Vector<Element>* outputSolution
) {
  // this function return true if Ax = b >= 0 has a solution with x >= 0 and records a solution x at outputPoint
  // else returns false, where b is a given nonnegative column vector, A is an n by m matrix
  // and x is a column vector with m entries
  Matrix<Rational> tempMatA;
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
  //tempMatb.assign(matb);
  tempMatA.initialize(matA.numberOfRows, numberOfTrueVariables + matA.numberOfRows);
  HashedList<Selection> VisitedVertices;
  VisitedVertices.clear();
  BaseVariables.initialize(tempMatA.numberOfColumns);
  tempMatA.makeZero();
  matX.makeZero(tempMatA.numberOfColumns);
  for (int j = 0; j < matA.numberOfColumns; j ++) {
    for (int i = 0; i < matA.numberOfRows; i ++) {
      tempMatA.elements[i][j].assign(matA.elements[i][j]);
    }
  }
  for (int j = 0; j < matA.numberOfRows; j ++) {
    tempMatA.elements[j][j + numberOfTrueVariables].makeOne();
    matX[j + numberOfTrueVariables] = (matb.elements[j][0]);
    BaseVariables.addSelectionAppendNewIndex(j + numberOfTrueVariables);
  }
  Rational ChangeGradient; //Change, PotentialChange;
  int EnteringVariable = 0;
  bool WeHaveNotEnteredACycle = true;
  while (EnteringVariable != - 1 && WeHaveNotEnteredACycle && GlobalGoal.isPositive()) {
    EnteringVariable = - 1; ChangeGradient.makeZero();
    for (int i = 0; i < tempMatA.numberOfColumns; i ++) {
      if (!BaseVariables.selected[i]) {
        Rational PotentialChangeGradient; bool hasAPotentialLeavingVariable;
        Matrix<Rational>::computePotentialChangeGradient(
          tempMatA, BaseVariables, numberOfTrueVariables, i, PotentialChangeGradient, hasAPotentialLeavingVariable
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
        MaxMovement, LeavingVariableRow, EnteringVariable, tempMatA, matX, BaseVariables
      );
      Rational tempRat, tempTotalChange;
      if (tempMatA.elements[LeavingVariableRow][EnteringVariable].isEqualToZero()) {
        global.fatal << "The leaving-entering coefficient is not allowed to be zero. " << global.fatal;
      }
      tempRat.assign(tempMatA.elements[LeavingVariableRow][EnteringVariable]);
      tempRat.invert();
      for (int i = 0; i < tempMatA.numberOfRows; i ++) {
        if (!tempMatA.elements[i][BaseVariables.elements[i]].isEqualTo(1)) {
          global.fatal << "The base variable coefficient is required to be 1 at this point of code. "
          << global.fatal;
        }
      }
      tempMatA.rowTimesScalar(LeavingVariableRow, tempRat);
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
      for (int i = 0; i < tempMatA.numberOfRows; i ++) {
        if (!tempMatA.elements[i][EnteringVariable].isEqualToZero()&& i != LeavingVariableRow) {
          tempRat.assign(tempMatA.elements[i][EnteringVariable]);
          tempRat.multiplyBy(MaxMovement);
          matX[BaseVariables.elements[i]] -= tempRat;
          tempRat.assign(tempMatA.elements[i][EnteringVariable]);
          tempRat.negate();
          tempMatA.addTwoRows(LeavingVariableRow, i, 0, tempRat);
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
      for (int i = 0; i < tempMatA.numberOfRows; i ++) {
        if (!tempMatA.elements[i][BaseVariables.elements[i]].isEqualTo(1)) {
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

bool CalculatorFunctions::innerAttemptExtendingEtoHEFwithHinCartan(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerAttemptExtendingEtoHEFwithHinCartan");
  if (input.size() != 3) {
    return output.makeError("Function takes 2 arguments - type and an element of the Lie algebra.", calculator);
  }
  WithContext<SemisimpleLieAlgebra*> ownerAlgebra;
  if (!calculator.convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, ownerAlgebra
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra* ownerSemisimple = ownerAlgebra.content;
  ElementSemisimpleLieAlgebra<Rational> theErational;
  if (!CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(
    calculator, input[2], theErational, *ownerSemisimple
  )) {
    return output.makeError("Failed to extract element of semisimple Lie algebra. ", calculator);
  }
  ElementSemisimpleLieAlgebra<AlgebraicNumber> theF, theH, theE;
  theE = theErational;
  std::stringstream out, logStream;
  bool success = ownerSemisimple->attemptExtendingEtoHEFwithHinCartan(theE, theH, theF, &logStream);
  if (success) {
    out << HtmlRoutines::getMathNoDisplay("F=" + theF.toString() + ";")
    << "<br>" << HtmlRoutines::getMathNoDisplay("H=" + theH.toString() + ";") << "<br>"
    << HtmlRoutines::getMathNoDisplay("E=" + theE.toString() + ";")
    << "<br><br>The log stream of the computation follows. " << logStream.str();
  } else {
    out << "<br>Couldn't extend E to sl(2)-triple. The log stream follows. " << logStream.str();
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerAdCommonEigenSpaces(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerAdCommonEigenSpaces");
  if (input.size() < 3) {
    return output.makeError(
      "Function ad common eigenspaces needs at least 2 arguments - type and at least one element of the algebra.",
      calculator
    );
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!calculator.convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, algebra
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra* ownerSemisimple = algebra.content;
  List<ElementSemisimpleLieAlgebra<Rational> > theOperators, outputElts;
  theOperators.reserve(input.size() - 2);
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  for (int i = 2; i < input.size(); i ++) {
    if (!CalculatorConversions::innerElementSemisimpleLieAlgebraRationalCoeffs(
      calculator, input[i], tempElt, *ownerSemisimple
    )) {
      return output.makeError("Failed to extract element of semisimple Lie algebra. ", calculator);
    }
    theOperators.addOnTop(tempElt);
  }
  ownerSemisimple->getCommonCentralizer(theOperators, outputElts);
  std::stringstream out;
  out << "<br>EigenSpace basis (" << outputElts.size << " elements total):<br> (";
  for (int i = 0; i < outputElts.size; i ++) {
    ElementSemisimpleLieAlgebra<Rational>& currentElt = outputElts[i];
    out << currentElt.toString();
    if (i != outputElts.size - 1) {
      out << ", ";
    }
  }
  out << ")";
  output.assignValue(out.str(), calculator);
  return true;
}

bool CalculatorFunctions::innerZmodP(Calculator& calculator, const Expression& input, Expression& output) {
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
  LargeIntegerUnsigned theGCD;
  LargeIntegerUnsigned::greatestCommonDivisor(left.getDenominator(), base.value, theGCD);
  if (theGCD > 1) {
    return false;
  }
  ElementZmodP outputElement;
  outputElement.modulus = base.value;
  outputElement = left.getNumerator();
  ExpressionContext context;
  context.initialize(calculator);
  context.setDefaultModulus(outputElement.modulus);
  return output.assignValueWithContext(outputElement, context, calculator);
}

bool CalculatorFunctions::innerConesIntersect(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerConesIntersect");
  if (!input.isListNElements(3)) {
    return calculator << "Function ConesIntersection expects 2 arguments, got " << input.children.size - 1 << " instead. ";
  }
  Matrix<Rational> coneNonStrictMatForm;
  Matrix<Rational> coneStrictMatForm;
  Vectors<Rational> coneNonStrictGens;
  Vectors<Rational> coneStrictGens;
  if (!calculator.functionGetMatrix(input[1], coneStrictMatForm)) {
    return calculator << "Failed to extract matrix from the first argument, " << input[1].toString();
  }
  if (!calculator.functionGetMatrix(input[2], coneNonStrictMatForm)) {
    return calculator << "Failed to extract matrix from the second argument, " << input[2].toString();
  }
  std::stringstream out;
  if (coneNonStrictMatForm.numberOfColumns != coneStrictMatForm.numberOfColumns) {
    out << "I got as input vectors of different dimensions, "
    << "first groups had vectors of dimension "
    << coneNonStrictMatForm.numberOfColumns
    << " and second of dimension "
    << coneStrictMatForm.numberOfColumns << " which is not allowed. ";
    return output.makeError(out.str(), calculator);
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
      global.fatal << "<br>This is a programming error: the output linear combination " << outputIntersection.toString()
      << " corresponds to the cone intersection " << checkVector.toString()
      << " and is not equal to zero! Here is the cone output so far: "
      << out.str() << global.fatal;
    }
    out << "<br>Cones intersect, here is one intersection: 0= " << outputIntersection.toStringLetterFormat("v");
  } else {
    out << "<br>Cones have empty intersection.";
    out << "<br> A normal separating the cones is: n =" << outputSeparatingNormal.toString() << ". Indeed, ";
    for (int i = 0; i < coneStrictGens.size; i ++) {
      out << "<br>\\langle v_{" << i + 1 << "}, n\\rangle = "
      << outputSeparatingNormal.scalarEuclidean(coneStrictGens[i]).toString();
    }
    for (int i = 0; i < coneNonStrictGens.size; i ++) {
      out << "<br>\\langle v_{" << i + 1 + coneStrictGens.size << "}, n\\rangle = "
      << outputSeparatingNormal.scalarEuclidean(coneNonStrictGens[i]).toString();
    }
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerReverseOrderRecursively(Calculator& calculator, const Expression& input, Expression& output) {
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

bool CalculatorFunctions::innerReverseOrder(Calculator& calculator, const Expression& input, Expression& output) {
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

bool CalculatorFunctions::innerNot(Calculator& calculator, const Expression& input, Expression& output) {
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  int theInt;
  if (!argument.isSmallInteger(&theInt)) {
    return false;
  }
  if (theInt == 0) {
    return output.assignValue(1, calculator);
  }
  return output.assignValue(0, calculator);
}

bool CalculatorFunctions::innerPrintZnEnumeration(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerPrintZnEnumeration");
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
  SelectionPositiveIntegers theSel;
  theSel.initialize(dimension);
  std::stringstream out2, out;
  LargeIntegerUnsigned gradeLarge = static_cast<unsigned>(grade);
  int counter = 0;
  for (
    theSel.setFirstInGradeLevel(gradeLarge);
    theSel.getGrading() == gradeLarge;
    theSel.incrementReturnFalseIfPastLast()
  ) {
    out2 << theSel.toString() << "<br>";
    counter ++;
  }
  out << "Total " << counter << " vectors:<br>" << out2.str();
  return output.assignValue(out.str(), calculator);
}

template <>
void Vector<Rational>::perturbNormalRelativeToVectorsInGeneralPosition(
  const Vectors<Rational>& nonStrictConeNonPositiveScalar,
  const List<Vector<Rational> >& vectorsToBeInGeneralPosition
);

bool CalculatorFunctions::innerPerturbSplittingNormal(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerPerturbSplittingNormal");
  std::stringstream out;
  if (input.size() != 4) {
    out << "Perturbing splitting normal takes 3 arguments: normal, "
    << "positive vectors, and vectors relative to which to perturb. "
    << "Instead I got " << input.size() - 1 << ". ";
    return output.makeError(out.str(), calculator);
  }
  Vector<Rational> splittingNormal;
  if (!calculator.getVector(input[1], splittingNormal, nullptr)) {
    return output.makeError("Failed to extract normal from first argument. ", calculator);
  }
  Matrix<Rational> theMat;
  Vectors<Rational> nonStrictCone, vectorsToPerturbRelativeTo;
  if (!calculator.functionGetMatrix(
    input[2], theMat, nullptr, splittingNormal.size, nullptr
  )) {
    return output.makeError("Failed to extract matrix from second argument. ", calculator);
  }
  nonStrictCone.assignMatrixRows(theMat);
  if (!calculator.functionGetMatrix(
    input[3], theMat, nullptr, splittingNormal.size, nullptr
  )) {
    return output.makeError("Failed to extract matrix from third argument. ", calculator);
  }
  vectorsToPerturbRelativeTo.assignMatrixRows(theMat);
  for (int i = 0; i < nonStrictCone.size; i ++) {
    if (splittingNormal.scalarEuclidean(nonStrictCone[i]) < 0) {
      std::stringstream out;
      out << "The normal vector " << splittingNormal.toString()
      << " is has negative scalar product with " << nonStrictCone[i].toString();
      return output.makeError(out.str(), calculator);
    }
  }
  out << "Perturbing " << splittingNormal.toString() << " relative to cone "
  << nonStrictCone.toString() << " and vectors " << vectorsToPerturbRelativeTo.toString();
  splittingNormal.perturbNormalRelativeToVectorsInGeneralPosition(nonStrictCone, vectorsToPerturbRelativeTo);
  out << "<br>End result: " << splittingNormal.toString();
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerPrintAllVectorPartitions(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerPrintAllVectorPartitions");
  RecursionDepthCounter theRecursion(&calculator.recursionDepth);
  if (input.size() != 3) {
    return output.makeError("Function innerPrintAllPartitions expects 2 arguments.", calculator);
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!calculator.convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, algebra
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra* theSSowner = algebra.content;

  SemisimpleLieAlgebra& theSSalgebra = *theSSowner;
  ExpressionContext theContext(calculator);
  Vector<Rational> theHW;
  if (!calculator.getVector<Rational>(input[2], theHW, &theContext, theSSalgebra.getRank())) {
    return output.makeError("Failed to extract weight you want partitioned from " + input[2].toString(), calculator);
  }
  Vector<int> theHWint;
  theHWint.setSize(theHW.size);
  for (int i = 0; i < theHW.size; i ++) {
    if (!theHW[i].isSmallInteger(&theHWint[i]) || theHW[i] < 0) {
      return output.makeError(
        "The input weight you gave is bad: "
        "it must consist of non-negative small integers",
        calculator
      );
    }
  }
  std::stringstream out;
  out << "<br>the weight you want partitioned: " << theHWint;
  Vector<int> thePartition;
  thePartition.setSize(theSSalgebra.getNumberOfPositiveRoots());
  for (int i = 0; i < thePartition.size; i ++) {
    thePartition[i] = 0;
  }
  Vector<Rational> theWeight, tmpWt;
  Vectors<Rational>& rootsBorel = theSSalgebra.theWeyl.rootsOfBorel;
  int counter = 0;
  int totalCycles = 0;
  theWeight.makeZero(theSSalgebra.getRank());
  int i = rootsBorel.size;
  while (i > 0 && counter < 10000) {
    totalCycles ++;
    if (theWeight == theHW) {
      tmpWt = thePartition;
      out << "<br>" << tmpWt.toStringLetterFormat("\\alpha");
      counter ++;
    }
    if (!(theHW - theWeight).isPositive() || i > rootsBorel.size) {
      if (i <= rootsBorel.size) {
        theWeight -= rootsBorel[i - 1] * thePartition[i - 1];
        thePartition[i - 1] = 0;
      }
      i --;
      if (i > 0) {
        theWeight += rootsBorel[i - 1];
        thePartition[i - 1] ++;
      }
    } else {
      i ++;
    }
  }
  out << "<br>Done in " << totalCycles << " cycles.";
  out << "<br>" << counter << " total partitions ";
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerInterpolatePoly(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerInterpolatePoly");
  if (input.size() < 2) {
    return false;
  }
  Expression convertedE;
  if (!CalculatorConversions::innerMakeMatrix(calculator, input, convertedE)) {
    return false;
  }
  Matrix<Rational> pointsOfInterpoly;
  if (!calculator.functionGetMatrix(
    convertedE, pointsOfInterpoly, nullptr, 2
  )) {
    return calculator
    << "<hr>Failed to extract points of interpolation from "
    << convertedE.toString();
  }
  Polynomial<Rational> interPoly;
  Vector<Rational> theArgs, theValues;
  pointsOfInterpoly.getVectorFromColumn(0, theArgs);
  pointsOfInterpoly.getVectorFromColumn(1, theValues);
  interPoly.interpolate(theArgs, theValues);
  ExpressionContext theContext(calculator);
  theContext.makeOneVariableFromString("x");
  return output.assignValueWithContext(interPoly, theContext, calculator);
}

bool CalculatorFunctions::innerOperationBinary(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  int theOp
) {
  MemorySaving<Calculator::OperationHandlers>& theOperation = calculator.operations.theValues[theOp];
  if (theOperation.isZeroPointer()) {
    return false;
  }
  List<Function>& handlers = theOperation.getElement().handlers;
  for (int i = 0; i < handlers.size; i ++) {
    if (handlers[i].inputFitsMyInnerType(input)) {
      if (handlers[i].theFunction(calculator, input, output)) {
        return true;
      }
    }
  }
  return false;
}

bool CalculatorFunctions::innerEWAorPoly(Calculator& calculator, const Expression& input, Expression& output, bool assignPoly) {
  MacroRegisterFunctionWithName("Calculator::innerEWAorPoly");
  if (!input.isListNElements(3)) {
    return false;
  }
  Vector<Polynomial<Rational> > inputPolForm;
  ExpressionContext startContext(calculator);
  if (!calculator.getVectorFromFunctionArguments(
    input,
    inputPolForm,
    &startContext,
    2,
    CalculatorConversions::functionPolynomial<Rational>
  )) {
    return calculator
    << "<hr>Failed to extract polynomials from arguments of "
    << input.toString();
  }
  int letterDiff = 0, letterPol = 0;
  if (
    !inputPolForm[0].isOneLetterFirstDegree(&letterDiff) ||
    !inputPolForm[1].isOneLetterFirstDegree(&letterPol) ||
    letterDiff == letterPol
  ) {
    return calculator
    << "<hr>Failed to get different one-variable polynomials from input. "
    << input.toString();
  }
  ExpressionContext endContext(calculator);
  endContext.makeOneVariableOneDifferentialOperator(
    startContext.getVariable(letterPol),
    startContext.getVariable(letterDiff)
  );
  ElementWeylAlgebra<Rational> outputEWA;
  if (assignPoly) {
    outputEWA.makexi(0, 1);
  } else {
    outputEWA.makedi(0, 1);
  }
  return output.assignValueWithContext(outputEWA, endContext, calculator);
}

bool CalculatorBasics::extractBaseMultiplication(
  Calculator& calculator, const Expression& input, Expression& output
) {
  RecursionDepthCounter theRecursionIncrementer(&calculator.recursionDepth);
  MacroRegisterFunctionWithName("Calculator::outerExtractBaseMultiplication");
  if (!input.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  bool result = false;
  // handle Anything * Rational = Rational * Anything
  output = input;
  if (output[2].isOfType<Rational>()) {
    output.children.swapTwoIndices(1, 2);
    result = true;
  }
  if (output[2].isOfType<double>() && !output[1].isOfType<Rational>()) {
    output.children.swapTwoIndices(1, 2);
    result = true;
  }
  if (output[2].isListStartingWithAtom(calculator.opTimes())) {
    if (output[2].children.size != 3) {
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
    if (CalculatorFunctions::innerTimes(calculator, tempExp, newExpr)) {
      output.makeProduct(calculator, newExpr, output[2][2]);
      result = true;
    }
  }
  // handle 0 * anything = 0
  if (output[1].isEqualToZero()) {
    return output.assignValue(0, calculator);
  }
  return result;
}
