// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".

#include "calculator_interface.h"
#include "calculator_inner_functions.h"
#include "calculator_inner_typed_functions.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_extra_modules_semisimple_Lie_algebras_implementation.h"
#include "math_extra_semisimple_Lie_algebras_implementation.h"
#include "math_general.h"

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
    global.fatal << "The number of inequalities: " << matA.numberOfRows << " does not match the number of "
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

bool CalculatorFunctions::innerGetLinksToSimpleLieAlgerbas(Calculator& calculator, const Expression& input, Expression& output) {
  (void) input;
  std::stringstream outFromHD, outRecomputeLinks;

  outFromHD << "\n\n<p>\n\n<table>"
  << "<tr><td>Structure constants</td>"
  << "<td>Semisimple subalgebras</td> "
  << "<td>sl(2) subalgebras</td><td>root subalgebras</td> </tr>\n";
  List<DynkinType> precomputedTypes;
  DynkinType::getPrecomputedDynkinTypes(precomputedTypes);
  for (int i = 0; i < precomputedTypes.size; i ++) {
    outFromHD << calculator.toStringSemismipleLieAlgebraLinksFromHD(precomputedTypes[i]);
    if (precomputedTypes[i].hasPrecomputedSubalgebras()) {
      std::stringstream recomputeCommand;
      recomputeCommand << "PrintSemisimpleSubalgebrasRecompute{}("
      << precomputedTypes[i].toString() << ")";
      outRecomputeLinks << "<br>"
      << HtmlRoutines::getCalculatorComputationAnchor(recomputeCommand.str());
    }
  }
  outFromHD << "</table></p>";
  std::string fileName = "semisimple_lie_algebra_structure.html";
  std::stringstream out;
  out
  << calculator.writeFileToOutputFolderReturnLink(outFromHD.str(), fileName, "Links file");
  out << "<br>Recompute links.";
  out << outRecomputeLinks.str();
  out << outFromHD.str();
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerPrintSSsubalgebrasNilradicals(Calculator& calculator, const Expression& input, Expression& output) {
  return CalculatorFunctions::innerPrintSSSubalgebras(calculator, input, output, true, true, true, true, true, true);
}

bool CalculatorFunctions::innerPrintSSsubalgebrasRecompute(Calculator& calculator, const Expression& input, Expression& output) {
  return CalculatorFunctions::innerPrintSSSubalgebras(calculator, input, output, true, true, false, true, false, true);
}

bool CalculatorFunctions::innerPrintSSsubalgebrasNoSolutions(Calculator& calculator, const Expression& input, Expression& output) {
  return CalculatorFunctions::innerPrintSSSubalgebras(calculator, input, output, true, false, false, false, false, false);
}

bool CalculatorFunctions::innerPrintSSsubalgebrasNoCentralizers(Calculator& calculator, const Expression& input, Expression& output) {
  return CalculatorFunctions::innerPrintSSSubalgebras(calculator, input, output, true, true, false, true, false, false);
}

bool CalculatorFunctions::innerPrintSSsubalgebrasRegular(Calculator& calculator, const Expression& input, Expression& output) {
  return CalculatorFunctions::innerPrintSSSubalgebras(calculator, input, output, false, true, false, true, false, true);
}

bool CalculatorFunctions::innerPrintSSSubalgebras(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool doForceRecompute,
  bool doAttemptToSolveSystems,
  bool docomputePairingTable,
  bool docomputeModuleDecompositionsition,
  bool doComputeNilradicals,
  bool doAdjustCentralizers
) {
  MacroRegisterFunctionWithName("Calculator::innerPrintSSsubalgebras");
  if (doForceRecompute) {
    if (!global.userDefaultHasAdminRights()) {
      return calculator << "Only logged-in admins allowed to force-recompute semisimple subalgebras. ";
    }
  }
  if (global.theResponse.monitoringAllowed()) {
    global.theResponse.initiate("Triggered by printSemisimpleSubalgebras.");
  }
  if (input.size() != 2) {
    return calculator << "Semisimple Lie algebra expects a single argument. ";
  }
  std::stringstream out;
  WithContext<SemisimpleLieAlgebra*> ownerAlgebra;
  SemisimpleLieAlgebra* ownerSSPointer = nullptr;
  bool isAlreadySubalgebrasObject = input[1].isOfType<SemisimpleSubalgebras>();
  if (!isAlreadySubalgebrasObject) {
    if (!calculator.convert(
      input[1],
      CalculatorConversions::functionSemisimpleLieAlgebra,
      ownerAlgebra
    )) {
      return output.makeError("Error extracting Lie algebra.", calculator);
    }
    ownerSSPointer = ownerAlgebra.content;
    if (ownerSSPointer->getRank() > 8) {
      out << "<b>This code is completely experimental and has been set to run up to rank 6. "
      << "As soon as the algorithms are mature enough, higher ranks will be allowed. </b>";
      return output.assignValue(out.str(), calculator);
    } else {
      out << "<b>This code is completely experimental. "
      << "Use the following printouts on your own risk.</b><br>";
    }
  } else {
    ownerSSPointer = input[1].getValue<SemisimpleSubalgebras>().owner;
  }
  if (ownerSSPointer == nullptr) {
    global.fatal << "zero pointer to semisimple Lie algebra: this shouldn't happen. " << global.fatal;
  }
  SemisimpleLieAlgebra& ownerLieAlgebra = *ownerSSPointer;
  std::string dynkinString = ownerSSPointer->theWeyl.theDynkinType.toString();
  global.relativePhysicalNameOptionalProgressReport = "progress_subalgebras_" + dynkinString;
  global.relativePhysicalNameOptionalResult = "result_subalgebras_" + dynkinString;
  SemisimpleSubalgebras& theSubalgebras =
  calculator.theObjectContainer.getSemisimpleSubalgebrasCreateIfNotPresent(ownerLieAlgebra.theWeyl.theDynkinType);
  theSubalgebras.computeStructureWriteFiles(
    ownerLieAlgebra,
    calculator.theObjectContainer.theAlgebraicClosure,
    calculator.theObjectContainer.semisimpleLieAlgebras,
    calculator.theObjectContainer.theSltwoSAs,
    &out,
    doForceRecompute,
    !isAlreadySubalgebrasObject,
    doComputeNilradicals,
    docomputeModuleDecompositionsition,
    doAttemptToSolveSystems,
    docomputePairingTable,
    doAdjustCentralizers
  );
  return output.assignValue(out.str(), calculator);
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
    out << HtmlRoutines::getMathSpanPure("F=" + theF.toString() + ";")
    << "<br>" << HtmlRoutines::getMathSpanPure("H=" + theH.toString() + ";") << "<br>"
    << HtmlRoutines::getMathSpanPure("E=" + theE.toString() + ";")
    << "<br><br>The log stream of the computation follows. " << logStream.str();
  } else {
    out << "<br>Couldn't extend E to sl(2)-triple. The log stream follows. " << logStream.str();
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerCasimir(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerCasimir");
  if (input.size() != 2) {
    return calculator << "Casimir function expects a single input. ";
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!calculator.convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, algebra
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra& algebraReference = *algebra.content;
  ElementUniversalEnveloping<RationalFunction<Rational> > theCasimir;
  theCasimir.makeCasimir(algebraReference);
  calculator << "Context Lie algebra: " << algebraReference.theWeyl.theDynkinType.toString()
  << ". The coefficient: " << algebraReference.theWeyl.getKillingDividedByTraceRatio().toString()
  <<  ". The Casimir element of the ambient Lie algebra. ";
  ExpressionContext context(calculator);
  context.setAmbientSemisimpleLieAlgebra(algebraReference);
  return output.assignValueWithContext(theCasimir, context, calculator);
}

bool CalculatorFunctions::innerEmbedG2inB3(Calculator& calculator, const Expression& input, Expression& output) {
  if (input.size() != 2) {
    return false;
  }

  output = input[1];
  if (!output.isOfType < ElementUniversalEnveloping<RationalFunction<Rational> > >()) {
    return output.makeError("Failed to convert argument to element of the Universal enveloping algebra. ", calculator);
  }
  SemisimpleLieAlgebra& ownerSS = *output.getAmbientSemisimpleLieAlgebraNonConstUseWithCaution();
  if (!ownerSS.isOfSimpleType('G', 2)) {
    return output.makeError("Error: embedding of G_2 in B_3 takes elements of U(G_2) as arguments.", calculator);
  }
  HomomorphismSemisimpleLieAlgebra theHmm;
  calculator.makeHmmG2InB3(theHmm);

  ElementUniversalEnveloping<RationalFunction<Rational> > argument = output.getValue<ElementUniversalEnveloping<RationalFunction<Rational> > >();
  ElementUniversalEnveloping<RationalFunction<Rational> > outputUE;
  if (!theHmm.applyHomomorphism(argument, outputUE)) {
    return output.makeError("Failed to apply homomorphism for unspecified reason", calculator);
  }
  outputUE.simplify();
  ExpressionContext context(calculator);
  context.setAmbientSemisimpleLieAlgebra(theHmm.theRange());
  return output.assignValueWithContext(outputUE, context, calculator);
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


bool CalculatorFunctions::innerCharacterSSLieAlgFD(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerCharacterSSLieAlgFD");
  Vector<Rational> theHW;
  Selection parSel;
  WithContext<SemisimpleLieAlgebra*> ownerSSLiealg;
  Expression tempE, tempE2;
  if (!calculator.getTypeHighestWeightParabolic(
    calculator, input, output, theHW, parSel, ownerSSLiealg, nullptr
  )) {
    return false;
  }
  if (output.isError()) {
    return true;
  }
  if (parSel.cardinalitySelection != 0) {
    return output.makeError("I know only to compute with finite dimensional characters, for the time being. ", calculator);
  }
  CharacterSemisimpleLieAlgebraModule<Rational> theElt;
  theElt.makeFromWeight(ownerSSLiealg.content->theWeyl.getSimpleCoordinatesFromFundamental(theHW), ownerSSLiealg.content);
  return output.assignValue(theElt, calculator);
}

bool Calculator::innerConesIntersect(Calculator& calculator, const Expression& input, Expression& output) {
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

bool Calculator::innerReverseOrderRecursivelY(Calculator& calculator, const Expression& input, Expression& output) {
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

bool Calculator::innerReverseOrder(Calculator& calculator, const Expression& input, Expression& output) {
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

bool CalculatorFunctions::innerWriteGenVermaModAsDiffOperators(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool AllGenerators,
  bool useNilWeight,
  bool ascending
) {
  MacroRegisterFunctionWithName("Calculator::innerWriteGenVermaModAsDiffOperators");
  RecursionDepthCounter theRecursionIncrementer(&calculator.recursionDepth);
  Vectors<Polynomial<Rational> > theHWs;
  theHWs.setSize(1);
  Selection theParSel;
  WithContext<SemisimpleLieAlgebra*> theSSalgebra;
  Expression truncatedInput = input;
  if (truncatedInput.children.size > 4) {
    int numEltsToCut = truncatedInput.children.size - 4;
    for (int i = 0; i < numEltsToCut; i ++) {
      truncatedInput.children.removeLastObject();
    }
  }
  if (!calculator.getTypeHighestWeightParabolic<Polynomial<Rational> >(
    calculator,
    truncatedInput,
    output,
    theHWs[0],
    theParSel,
    theSSalgebra,
    CalculatorConversions::functionPolynomial<Rational>)
  ) {
    return output.makeError("Failed to extract type, highest weight, parabolic selection", calculator);
  }
  if (output.isError()) {
    return true;
  }
  std::string letterString = "x";
  std::string partialString = "\\partial";
  std::string exponentLetterString = "a";
  if (input.children.size > 4) {
    letterString = input[4].toString();
  }
  if (input.children.size > 5) {
    partialString = input[5].toString();
  }
  if (input.children.size > 6) {
    exponentLetterString = input[6].toString();
  }
  return calculator.innerWriteGenVermaModAsDiffOperatorInner(
    calculator,
    input,
    output,
    theHWs,
    theSSalgebra.context,
    theParSel,
    theSSalgebra.content,
    AllGenerators,
    &letterString,
    &partialString,
    &exponentLetterString,
    useNilWeight,
    ascending
  );
}
