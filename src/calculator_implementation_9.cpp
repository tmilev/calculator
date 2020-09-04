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
      << HtmlRoutines::getCalculatorComputationAnchorNewPage(recomputeCommand.str(), "");
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
    out << HtmlRoutines::getMathNoDisplay("F=" + theF.toString() + ";")
    << "<br>" << HtmlRoutines::getMathNoDisplay("H=" + theH.toString() + ";") << "<br>"
    << HtmlRoutines::getMathNoDisplay("E=" + theE.toString() + ";")
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

bool CalculatorFunctions::innerGetChevGen(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerGetChevGen");
  if (input.size() != 3) {
    return false;
  }
  WithContext<SemisimpleLieAlgebra*> theSSalg;
  if (!calculator.convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, theSSalg
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  int theIndex;
  if (!input[2].isSmallInteger(&theIndex)) {
    return false;
  }
  if (theIndex > theSSalg.content->getNumberOfPositiveRoots() || theIndex == 0 || theIndex < - theSSalg.content->getNumberOfPositiveRoots()) {
    return output.makeError("Bad Chevalley-Weyl generator index.", calculator);
  }
  ElementSemisimpleLieAlgebra<Rational> theElt;
  if (theIndex > 0) {
    theIndex += theSSalg.content->getRank() - 1;
  }
  theIndex += theSSalg.content->getNumberOfPositiveRoots();
  theElt.makeGenerator(theIndex, *theSSalg.content);
  ElementUniversalEnveloping<RationalFunction<Rational> > theUE;
  theUE.assignElementLieAlgebra(theElt, *theSSalg.content);
  ExpressionContext context(calculator);
  int indexInOwner = calculator.theObjectContainer.semisimpleLieAlgebras.getIndex(
    theSSalg.content->theWeyl.theDynkinType
  );
  context.setIndexAmbientSemisimpleLieAlgebra(indexInOwner);
  return output.assignValueWithContext(theUE, context, calculator);
}

bool CalculatorFunctions::innerGetCartanGen(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerGetCartanGen");
  if (input.size() != 3) {
    return false;
  }
  WithContext<SemisimpleLieAlgebra*> theSSalg;
  if (!calculator.convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, theSSalg
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  if (theSSalg.content == nullptr) {
    global.fatal << "This is a programming error: called conversion function successfully, "
    << "but the output is a zero pointer to a semisimple Lie algebra. "
    << global.fatal;
  }
  int theIndex;
  if (!input[2].isSmallInteger(&theIndex)) {
    return false;
  }
  if (
    theIndex == 0 ||
    theIndex > theSSalg.content->getNumberOfPositiveRoots() ||
    theIndex < - theSSalg.content->getNumberOfPositiveRoots()
  ) {
    return output.makeError("Bad Cartan subalgebra generator index.", calculator);
  }
  ElementSemisimpleLieAlgebra<Rational> theElt;
  Vector<Rational> theH = theSSalg.content->theWeyl.rootSystem[theSSalg.content->getRootIndexFromDisplayIndex(theIndex)];
  theElt.makeCartanGenerator(theH, *theSSalg.content);
  ElementUniversalEnveloping<RationalFunction<Rational> > theUE;
  theUE.assignElementLieAlgebra(theElt, *theSSalg.content);
  ExpressionContext theContext(calculator);
  int theAlgIndex = calculator.theObjectContainer.semisimpleLieAlgebras.getIndex(theSSalg.content->theWeyl.theDynkinType);
  theContext.setIndexAmbientSemisimpleLieAlgebra(theAlgIndex);
  return output.assignValueWithContext(theUE, theContext, calculator);
}

bool CalculatorFunctions::innerRootSubsystem(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerRootSubsystem");
  if (input.size() < 3) {
    return false;
  }
  WithContext<SemisimpleLieAlgebra*> algebra;
  if (!calculator.convert(
    input[1], CalculatorConversions::functionSemisimpleLieAlgebra, algebra
  )) {
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  SemisimpleLieAlgebra* theSSlieAlg = algebra.content;
  int theRank = theSSlieAlg->getRank();
  Vector<Rational> currentRoot;
  Vectors<Rational> outputRoots;
  WeylGroupData& theWeyl = theSSlieAlg->theWeyl;
  if (!theWeyl.theDynkinType.isSimple()) {
    return calculator << "<hr>Function root subsystem works for simple ambient types only. ";
  }
  for (int i = 2; i < input.size(); i ++) {
    if (!calculator.getVector(input[i], currentRoot, nullptr, theRank, nullptr)) {
      return false;
    }
    if (!theWeyl.rootSystem.contains(currentRoot)) {
      return output.makeError("Input vector " + currentRoot.toString() + " is not a root. ", calculator);
    }
    outputRoots.addOnTop(currentRoot);
  }
  std::stringstream out;
  DynkinDiagramRootSubalgebra theDiagram;
  theWeyl.transformToSimpleBasisGenerators(outputRoots, theWeyl.rootSystem);
  theDiagram.ambientBilinearForm = theWeyl.cartanSymmetric;
  theDiagram.ambientRootSystem = theWeyl.rootSystem;
  theDiagram.computeDiagramInputIsSimple(outputRoots);
  out << "Diagram final: " << theDiagram.toString()
  << ". Simple basis: " << theDiagram.simpleBasesConnectedComponents.toString();
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

bool CalculatorFunctions::innerPrintSSLieAlgebra(
  Calculator& calculator, const Expression& input, Expression& output, bool Verbose
) {
  return CalculatorFunctions::innerWriteToHDOrPrintSSLieAlgebra(calculator, input, output, Verbose, false);
}

bool CalculatorFunctions::innerWriteSSLieAlgebraToHD(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerWriteSSLieAlgebraToHD");
  calculator.checkInputNotSameAsOutput(input, output);
  if (!global.userDefaultHasAdminRights() && !global.flagRunningConsoleTest) {
    return output.makeError(
      "Caching structure constants to HD available to admins only. ",
      calculator);
  }
  return CalculatorFunctions::innerWriteToHDOrPrintSSLieAlgebra(calculator, input, output, true, true);
}

bool CalculatorFunctions::innerWriteToHDOrPrintSSLieAlgebra(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool Verbose,
  bool writeToHD
) {
  MacroRegisterFunctionWithName("Calculator::innerWriteToHDOrPrintSSLieAlgebra");
  if (input.size() != 2) {
    return calculator << "Print semisimple Lie algebras expects 1 argument. ";
  }
  return CalculatorFunctions::functionWriteToHDOrPrintSSLieAlgebra(
    calculator, input[1], output, Verbose, writeToHD
  );
}

bool CalculatorFunctions::functionWriteToHDOrPrintSSLieAlgebra(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool Verbose,
  bool writeToHD
) {
  MacroRegisterFunctionWithName("Calculator::functionWriteToHDOrPrintSSLieAlgebra");
  WithContext<SemisimpleLieAlgebra*> tempSSpointer;
  input.checkInitialization();
  if (!calculator.convert(
    input,
    CalculatorConversions::functionSemisimpleLieAlgebra,
    tempSSpointer
  )) {

    calculator << "Failed to extract Lie algebra from: " << input.toString() << "<br>";
    return output.makeError("Error extracting Lie algebra.", calculator);
  }
  tempSSpointer.content->checkConsistency();
  tempSSpointer.context.checkInitialization();
  SemisimpleLieAlgebra& theSSalgebra = *tempSSpointer.content;
  std::string result = theSSalgebra.toHTMLCalculator(Verbose, writeToHD, calculator.flagWriteLatexPlots);
  return output.assignValue(result, calculator);
}

bool CalculatorFunctions::innerPrintB3G2branchingIntermediate(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  Vectors<RationalFunction<Rational> >& theHWs,
  BranchingData& theG2B3Data,
  ExpressionContext& theContext
) {
  MacroRegisterFunctionWithName("Calculator::innerPrintB3G2branchingIntermediate");
  (void) input;
  std::stringstream out, timeReport;
  std::stringstream latexTable, latexTable2;
  bool isFD = (theG2B3Data.selInducing.cardinalitySelection == 0);
  if (isFD) {
    out << "<table border =\"1\"><tr><td>$so(7)$-highest weight</td><td>Decomposition over $G_2$</td>"
    <<  "<td>$G_2\\cap b$-eigenvectors </td></tr>";
    latexTable << "\\begin{longtable}{|ccccl|} \\caption{\\label{tableB3fdsOverG2charsAndHWV} "
    << " Decompositions of finite dimensional $so(7)$-modules over $G_2$}\\\\"
    << "\\hline so(7)& dim. &$G_2$&dim.& $\\mathfrak b \\cap G_2$-singular vectors\\\\ \\hline"
    << "\\endhead \n<br>";
  } else {
    out << "Let " << HtmlRoutines::getMathNoDisplay("p\\subset so(7)") << " be the "
    << theG2B3Data.selInducing.toString() << "-parabolic subalgebra"
    << " and let " << HtmlRoutines::getMathNoDisplay("{p}'= p\\cap G_2")
    << ". Then  " << HtmlRoutines::getMathNoDisplay("{p}'") << " is the "
    << theG2B3Data.selSmallParSel.toString() << "- parabolic subalgebra of G_2"
    << "<br> <table border =\"1\"><tr><td>$so(7)$-highest weight</td>"
    << "<td>character difference from top</td>"
    << "<td>Decomposition of inducing module over "
    << HtmlRoutines::getMathNoDisplay("p'")
    << "</td><td>" << HtmlRoutines::getMathNoDisplay("p'\\cap b")
    << "-eigenvectors</td><td>Casimir projector</td><td>Extra multiplier</td><td>corresponding "
    << HtmlRoutines::getMathNoDisplay("G_2\\cap b")
    << "-eigenvectors</td><td>Shapovalov square</td></tr>";
    latexTable << "\\begin{longtable}{|cccclll|} \\caption{\\label{tableB3fdsOverG2charsAndHWV"
    << theG2B3Data.selInducing.toString() << "} "
    << "Decomposition of inducing $" << theG2B3Data.selInducing.toString() << "$-$\\mathfrak p$-module over $"
    << theG2B3Data.selSmallParSel.toString() << "$-$\\mathfrak p'=\\mathfrak p\\cap G_2$}\\\\"
    << "\\hline $\\mathfrak p-inducing$& dim. &$\\mathfrak p' decomp. $&dim.&"
    << " $\\mathfrak b \\cap \\mathfrak p'$-singular vectors & Casimir projector "
    << "& Corresp. $\\mathfrak b \\cap G_2$-singular vectors  \\\\ \\hline"
    << "\\endhead \n<br>";
  }
  theContext.getFormat(theG2B3Data.theFormat);
  theG2B3Data.theFormat.flagUseLatex = true;
  theG2B3Data.theFormat.numberOfAmpersandsPerNewLineForLaTeX = 0;
  Expression tempExpression;
  RationalFunction<Rational> rfZero, rfOne;
  rfZero.makeZero();
  rfOne.makeOne();
  latexTable2 << "\\begin{longtable}{|rll|}\\caption"
  << "{Values of $x_1$ for each $v_{\\lambda,i}$}\\label{tableCriticalValuesvlambda}"
  << "\\endhead";
  for (int i = 0; i < theHWs.size; i ++) {
    theG2B3Data.theWeightFundCoords = theHWs[i];
    calculator.innerSplitFDpartB3overG2inner(calculator, theG2B3Data, tempExpression);
    timeReport << tempExpression.getValue<std::string>();
    RationalFunction<Rational> numEigenVectors;
    numEigenVectors = rfZero;
    for (int j = 0; j < theG2B3Data.theSmallCharFDpart.size(); j ++) {
      numEigenVectors += theG2B3Data.theSmallCharFDpart.coefficients[j];
    }
    theG2B3Data.theFormat.customPlusSign = "";
    int eigenIndexcounter = 0;
    if (i != 0) {
      latexTable2 << "\\hline\\multicolumn{3}{|c|}{$\\lambda ="
      << theG2B3Data.theWeightFundCoords.toStringLetterFormat("\\omega", &theG2B3Data.theFormat)
      << "$}\\\\vector& coefficient of $v_\\lambda$ in $Sh_{\\lambda,i}$ &$x_1\\notin$ \\\\\\hline";
    }
    for (int k = 0; k < theG2B3Data.theSmallCharFDpart.size(); k ++) {
      CharacterSemisimpleLieAlgebraModule<RationalFunction<Rational> > tempChar;
      tempChar.addMonomial(theG2B3Data.theSmallCharFDpart[k], theG2B3Data.theSmallCharFDpart.coefficients[k]);
      int multiplicity = 0;
      theG2B3Data.theSmallCharFDpart.coefficients[k].isSmallInteger(&multiplicity);
      for (int counter = 0; counter < multiplicity; counter ++, eigenIndexcounter ++) {
        out << "<tr>";
        if (k == 0 && counter == 0) {
          theG2B3Data.theFormat.customPlusSign = "\\oplus ";
          theG2B3Data.theFormat.fundamentalWeightLetter = "\\omega";
          out << "<td rowspan =\"" << numEigenVectors.toString() << "\">" << theG2B3Data.theAmbientChar.toString(&theG2B3Data.theFormat) << "</td> ";
          if (!isFD) {
            out << "<td rowspan =\"" << numEigenVectors.toString() << "\">";
            for (int charcounter1 = 0; charcounter1 < theG2B3Data.theCharacterDifferences.size; charcounter1 ++) {
              std::string tempS = theG2B3Data.theFormat.customPlusSign;
              theG2B3Data.theFormat.customPlusSign = "";
              out << "A_{" << charcounter1 << "}=" << theG2B3Data.theCharacterDifferences[charcounter1].toString(&theG2B3Data.theFormat) << ";";
              if (charcounter1 != theG2B3Data.theCharacterDifferences.size - 1) {
                out << "<br>";
              }
              theG2B3Data.theFormat.customPlusSign = tempS;
            }
            out << "</td> ";
          }
          latexTable << "\\multirow{" << theG2B3Data.theEigenVectorS.size  << "}{*}{$"
          << theG2B3Data.theAmbientChar.toString(&theG2B3Data.theFormat) << "$}";
          Vector<RationalFunction<Rational> > theSimpleCoordinates;
          theSimpleCoordinates = theG2B3Data.WeylFD.ambientWeyl->getSimpleCoordinatesFromFundamental(
            theG2B3Data.theAmbientChar[0].weightFundamentalCoordS
          );
          RationalFunction<Rational> theWeylSize;
          theWeylSize = theG2B3Data.WeylFD.weylDimensionFormulaInnerSimpleCoords(theSimpleCoordinates);
          latexTable << "& \\multirow{" << theG2B3Data.theEigenVectorS.size  << "}{*}{$"
          << theWeylSize.toString(&theG2B3Data.theFormat)
          << "$}";
        } else {
          latexTable << "&";
        }
        latexTable << " & ";
        if (counter == 0) {
          theG2B3Data.theFormat.fundamentalWeightLetter = "\\psi";
          theG2B3Data.theFormat.customPlusSign = "\\oplus ";
          out << "<td rowspan =\"" << multiplicity << " \">" << tempChar.toString(&theG2B3Data.theFormat) << "</td>";
          latexTable << "\\multirow{" << multiplicity  << "}{*}{$";
          latexTable << tempChar.toString(&theG2B3Data.theFormat) << "$}";
          latexTable << "&\\multirow{" << multiplicity  << "}{*}{$";
          if (multiplicity > 1) {
            latexTable << multiplicity << "\\times";
          }
          Vector<RationalFunction<Rational> > theSimpleCoordinates;
          theSimpleCoordinates = theG2B3Data.WeylFDSmall.ambientWeyl->getSimpleCoordinatesFromFundamental(
            tempChar[0].weightFundamentalCoordS
          );
          RationalFunction<Rational> dimension;
          dimension = theG2B3Data.WeylFDSmall.weylDimensionFormulaInnerSimpleCoords(theSimpleCoordinates, rfOne);
          latexTable << dimension.toString(&theG2B3Data.theFormat) << "$}";
        } else {
          latexTable << "&";
        }
        latexTable << "&";
        theG2B3Data.theFormat.customPlusSign = "";
        out << "<td>" << HtmlRoutines::getMathNoDisplay(theG2B3Data.theEigenVectorsLevi[eigenIndexcounter].toString(&theG2B3Data.theFormat)) << "</td>";
        theG2B3Data.theFormat.maximumLineLength = 20;
        latexTable << "$\\begin{array}{l}" << theG2B3Data.theEigenVectorsLevi[eigenIndexcounter].toString(&theG2B3Data.theFormat) << "\\end{array}$ \n";
        if (!isFD) {
          std::string tempS1 = theG2B3Data.getStringCasimirProjector(eigenIndexcounter, 12);
          std::string tempS2 = "("+ theG2B3Data.theUEelts[eigenIndexcounter].toString(&theG2B3Data.theFormat) + ")\\cdot v_\\lambda";
          out << "<td>" << HtmlRoutines::getMathNoDisplay(tempS1) << "</td>";
          out << "<td>" << theG2B3Data.additionalMultipliers[eigenIndexcounter].toString() << "</td>";
          out << "<td>" << HtmlRoutines::getMathNoDisplay(tempS2) << "</td>";
          out << "<td>" << theG2B3Data.theShapovalovProducts[eigenIndexcounter].toString(&theG2B3Data.theFormat);
          out << "</td>";
          int theIndex = - 1;
          numEigenVectors.isSmallInteger(&theIndex);
          if (theIndex - eigenIndexcounter - 1 > 0) {
            List<Rational> tempList, tempList2;
            latexTable2 << " $v_{\\lambda," <<  theIndex- eigenIndexcounter - 1 << "} $&";
            Polynomial<Rational> tempP;
            theG2B3Data.theShapovalovProducts[eigenIndexcounter].getNumerator(tempP);
            tempP.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
            latexTable2 << "$\\begin{array}{l}" << tempP.toString(&theG2B3Data.theFormat) << "\\end{array}$ & ";
            if (tempP.findOneVariableRationalRoots(tempList)) {
              tempList2.addOnTopNoRepetition(tempList);
              out << "<td>Rational roots: " << tempList.toString() << "</td>";
              latexTable2 << tempList2.toString();
            }
            latexTable2 << "\\\\";
          }
          latexTable << "& $\\begin{array}{l}" << tempS1 << "\\end{array}$"
          << "&" << "$\\begin{array}{l}" << tempS2 << "\\end{array}$";
        }
        latexTable << "\\\\ \n <br>\n";
        if (counter != multiplicity - 1) {
          if (isFD) {
            latexTable << "\\cline{5-5}";
          } else {
            latexTable << "\\cline{5-7}";
          }
        }
        out << "</tr>";
      }
      if (k != theG2B3Data.theSmallCharFDpart.size() - 1) {
        if (isFD) {
          latexTable << "\\cline{3-5}";
        } else {
          latexTable << "\\cline{3-7}";
        }
      }
    }
    latexTable << "\\hline";
  }
  latexTable << "\\hline";
  out << "</table>";
  latexTable << "\\end{longtable}";
  latexTable2 << "\\end{longtable}";
  out << "<br>" << timeReport.str() << "<br><br><br><b>Ready for LaTeX consumption:</b><br  >";
  out << latexTable2.str();
  out << "<br><br><br>";
  out << latexTable.str();
  out << "<br>";
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerPrintB3G2branchingTable(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerPrintB3G2branchingTable");
  Vectors<RationalFunction<Rational> > theHWs;
  BranchingData theG2B3Data;
  ExpressionContext context(calculator);
  if (!CalculatorFunctions::innerPrintB3G2branchingTableCommon(
    calculator, input, output, theHWs, theG2B3Data, context
  )) {
    return false;
  }
  if (output.isError()) {
    return true;
  }
  return CalculatorFunctions::innerPrintB3G2branchingIntermediate(
    calculator, input, output, theHWs, theG2B3Data, context
  );
}

bool CalculatorFunctions::innerPrintB3G2branchingTableCharsOnly(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("Calculator::innerPrintB3G2branchingTableCharsOnly");
  BranchingData theg2b3data;
  ExpressionContext theContext(calculator);
  Vectors<RationalFunction<Rational> > theHWs;
  CalculatorFunctions::innerPrintB3G2branchingTableCommon(
    calculator, input, output, theHWs, theg2b3data, theContext
  );
  if (output.isError()) {
    return true;
  }
  CharacterSemisimpleLieAlgebraModule<RationalFunction<Rational> > theCharacter, outputChar;
  std::stringstream out;
  std::stringstream latexTable;
  bool isFD = (theg2b3data.selInducing.cardinalitySelection == 0);
  if (isFD) {
    out << "<table>"
    << "<tr><td>$so(7)$-highest weight</td>"
    << "<td>Dimension</td><td>Decomposition over $G_2$</td><td>Dimensions</td>" << "</tr>";
    latexTable << "\\begin{longtable}{|rl|} \\caption{\\label{tableB3fdsOverG2charsonly} "
    << "Decompositions of finite dimensional $so(7)$-modules over $G_2$}\\\\"
    << "\\hline$so(7)$-module & ~~~~~~ decomposition over $G_2$\\endhead \\hline\n<br>";
  } else {
    out << "Let " << HtmlRoutines::getMathNoDisplay("p\\subset so(7)") << " be the "
    << theg2b3data.selInducing.toString() << "-parabolic subalgebra "
    << "and let " << HtmlRoutines::getMathNoDisplay("{p}'= p\\cap G_2")
    << ". Then  " << HtmlRoutines::getMathNoDisplay("{p}'") << " is the "
    << theg2b3data.selSmallParSel.toString() << "- parabolic subalgebra of G_2"
    << "<br> <table><tr><td>$so(7)$-highest weight</td>"
    << "<td>Dimension of inducing fin. dim. "
    << HtmlRoutines::getMathNoDisplay(" p")
    << "-module</td><td>Decomposition of inducing module over "
    << HtmlRoutines::getMathNoDisplay("p'")
    << "</td><td>Dimensions</td>"
    << " <td>Highest weight <br> is sufficiently generic <br> if none of <br>the following vanish</td>"
    << "</tr>";
    latexTable << "\\begin{longtable}{|p{2cm}l|} \\caption{\\label{tableB3fdsOverG2charsonly"
    << theg2b3data.selInducing.toString() << "} "
    << "Decompositions of inducing $\\mathfrak{p}" << "_{"
    << theg2b3data.selInducing.toString()
    << "}" << "$-modules over $\\bar{ \\mathfrak {l}}$"
    << ", where $\\mathfrak{l}$ is the reductive Levi part of "
    << "$\\mathfrak{p}" << "_{" << theg2b3data.selInducing.toString()
    <<  "}$ and $\\bar{\\mathfrak {l}}$ is the reductive Levi part of $\\bar{\\mathfrak {p}} _{"
    << theg2b3data.selSmallParSel.toString()  << "}$" << " } \\\\"
    << "\\hline $V_\\lambda(\\mathfrak l)$ " << "& Decomposition over $\\bar { \\mathfrak l}$ "
    << "\\endhead \\hline\n<br>";
  }
  theg2b3data.theFormat.flagUseLatex = true;
  ElementUniversalEnveloping<RationalFunction<Rational> > theCasimir, theCentralCharacter, resultChar;
  HashedList<ElementUniversalEnveloping<RationalFunction<Rational> > > theCentralChars;
  theCasimir.makeCasimir(theg2b3data.theHmm.theDomain());
  WeylGroupData& smallWeyl = theg2b3data.theHmm.theDomain().theWeyl;
  for (int k = 0; k < theHWs.size; k ++) {
    theCharacter.makeFromWeight(
      theg2b3data.theHmm.theRange().theWeyl.getSimpleCoordinatesFromFundamental(theHWs[k]),
      &theg2b3data.theHmm.theRange()
    );
    theCharacter.splitCharacterOverReductiveSubalgebra(nullptr, outputChar, theg2b3data);
    theg2b3data.theFormat.fundamentalWeightLetter = "\\omega";
    out << "<tr><td> " << theCharacter.toString(&theg2b3data.theFormat) << "</td> ";
    Vector<RationalFunction<Rational> > simpleCoordinates;
    simpleCoordinates = theg2b3data.WeylFD.ambientWeyl->getSimpleCoordinatesFromFundamental(
      theCharacter[0].weightFundamentalCoordS
    );
    RationalFunction<Rational> dimension;
    dimension = theg2b3data.WeylFD.weylDimensionFormulaInnerSimpleCoords(simpleCoordinates);
    out << "<td>" << dimension.toString() << "</td>";
    latexTable << " $ " << theCharacter.toString(&theg2b3data.theFormat) << " $ ";
    theg2b3data.theFormat.fundamentalWeightLetter = "\\psi";
    out << "<td>" << outputChar.toString(&theg2b3data.theFormat) << "</td>";
    out << "<td>";
    theg2b3data.theFormat.customPlusSign = "\\oplus ";
    Vector<RationalFunction<Rational> > leftWeightSimple, leftWeightDual, rightWeightSimple, rightWeightDual;
    theCentralChars.clear();
    for (int i = 0; i < outputChar.size(); i ++) {
      if (!outputChar.coefficients[i].isEqualToOne()) {
        out << outputChar.coefficients[i].toString() << " x ";
      }
      simpleCoordinates = theg2b3data.WeylFDSmall.ambientWeyl->getSimpleCoordinatesFromFundamental(
        outputChar[i].weightFundamentalCoordS
      );
      dimension = theg2b3data.WeylFDSmall.weylDimensionFormulaInnerSimpleCoords(simpleCoordinates);
      out << dimension;
      if (i != outputChar.size() - 1) {
        out << "+";
      }
      leftWeightSimple = smallWeyl.getSimpleCoordinatesFromFundamental(outputChar[i].weightFundamentalCoordS);
      leftWeightDual = smallWeyl.getDualCoordinatesFromFundamental(outputChar[i].weightFundamentalCoordS);
      for (int j = 0; j < outputChar.size(); j ++) {
        rightWeightSimple = smallWeyl.getSimpleCoordinatesFromFundamental(outputChar[j].weightFundamentalCoordS);
        rightWeightDual = smallWeyl.getDualCoordinatesFromFundamental(outputChar[j].weightFundamentalCoordS);
        if ((rightWeightSimple - leftWeightSimple).isPositive()) {
          resultChar = theCasimir;
          theCentralCharacter = theCasimir;
          resultChar.modOutVermaRelations(&rightWeightDual);
          theCentralCharacter.modOutVermaRelations(&leftWeightDual);
          resultChar -= theCentralCharacter;
          resultChar.scaleNormalizeLeadingMonomial(nullptr);
          resultChar *= - 1;
          theCentralChars.addOnTopNoRepetition(resultChar);
        }
      }
    }
    out << "</td>";
    theg2b3data.theFormat.maximumLineLength = 80;
    latexTable << "& $\\begin{array}{l} " << outputChar.toString(&theg2b3data.theFormat) << "\\end{array} $ ";
    theg2b3data.theFormat.customPlusSign = "";
    if (!isFD && theCentralChars.size > 0) {
      out << "<td>";
      latexTable << " \\\\\n<br> ";
      latexTable << "\\multicolumn{2}{|p{11.6cm}|}{ \\tiny Strong Condition B: ";
      for (int l = 0; l < theCentralChars.size; l ++) {
        out << theCentralChars[l].toString(&theg2b3data.theFormat) << "<br> ";
        latexTable << "$" << theCentralChars[l].toString(&theg2b3data.theFormat) << "\\neq 0$";
        if (l != theCentralChars.size - 1) {
          latexTable << ", ";
        }
      }
      latexTable << "\\normalsize}";
      out << " </td>";
    }
    out << "</tr>";
    latexTable << "\\\\\n <br>\n";
    latexTable << "\\hline";
  }
  out << "</table>";
  latexTable << "\\end{longtable}";
  out << "<br><b>Ready for LaTeX consumption:</b><br>%preamble: "
  << "<br>\\documentclass{article}<br>\\usepackage{longtable, amssymb}"
  << "<br>\\begin{document}<br>%text body<br>" << latexTable.str()
  << "<br>%end of text body <br>\\end{document}";
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

bool Calculator::outerExtractBaseMultiplication(
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
