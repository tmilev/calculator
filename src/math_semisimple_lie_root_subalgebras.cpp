// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_implementation.h"
#include "math_extra_semisimple_lie_algebras_root_subalgebras.h"
#include "math_extra_semisimple_lie_algebras_implementation.h"
#include "math_extra_finite_groups_implementation.h"

void RootSubalgebra::getCoxeterElement(Matrix<Rational>& output) {
  int dimension = this->getAmbientWeyl().getDimension();
  output.makeIdentityMatrix(dimension);
  Matrix<Rational> matrix;
  for (int i = 0; i < this->simpleRootsReductiveSubalgebra.size; i ++) {
    this->getAmbientWeyl().getMatrixReflection(this->simpleRootsReductiveSubalgebra[i], matrix);
    output.multiplyOnTheLeft(matrix);
  }
}

void RootSubalgebra::getCoxeterPlane(Vector<double>& outputBasis1, Vector<double>& outputBasis2) {
  //this->computeRho(true);
  int dimension = this->getAmbientWeyl().getDimension();
  if (dimension < 2) {
    return;
  }
  if (this->simpleRootsReductiveSubalgebra.size < 2) {
    if (this->simpleRootsReductiveSubalgebra.size == 1) {
      outputBasis1 = this->simpleRootsReductiveSubalgebra[0].getVectorDouble();
    } else {
      outputBasis1.makeEi(dimension, 0);
    }
    if (outputBasis1[0] == 0.0) {
      outputBasis2.makeEi(dimension, 0);
    } else {
      outputBasis2.makeEi(dimension, 1);
    }
    return;
  }
  Matrix<Rational> matCoxeterElt;
  this->getCoxeterElement(matCoxeterElt);
  this->computeDynkinDiagramKAndCentralizer();
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms tempGroup;
  int coxeterNumber = 1;
  for (int i = 0; i < this->dynkinDiagram.simpleBasesConnectedComponents.size; i ++) {
    tempGroup.ambientWeyl = &this->getAmbientWeyl();
    tempGroup.simpleRootsInner = this->dynkinDiagram.simpleBasesConnectedComponents[i];
    tempGroup.computeRootSubsystem();
    Vector<Rational>& lastRoot = *tempGroup.RootSubsystem.lastObject();
    Vector<Rational> lastRootInSimpleCoords;
    lastRoot.getCoordinatesInBasis(tempGroup.simpleRootsInner, lastRootInSimpleCoords);
    coxeterNumber = MathRoutines::maximum(lastRootInSimpleCoords.sumCoordinates().numeratorShort, coxeterNumber);
  }
  Complex<double> theEigenValue;
  theEigenValue.realPart = FloatingPoint::cosFloating(2 * MathRoutines::pi() / coxeterNumber);
  theEigenValue.imaginaryPart = FloatingPoint::sinFloating(2 * MathRoutines::pi() / coxeterNumber);
  Matrix<Complex<double> > eigenMat;
  eigenMat.initialize(matCoxeterElt.numberOfRows, matCoxeterElt.numberOfColumns);
  for (int i = 0; i < eigenMat.numberOfRows; i ++) {
    for (int j = 0; j < eigenMat.numberOfColumns; j ++) {
      eigenMat.elements[i][j] = matCoxeterElt.elements[i][j].getDoubleValue();
      if (i == j) {
        eigenMat.elements[i][i] -= theEigenValue;
      }
    }
  }
  List<Vector<Complex<double> > > theEigenSpaceList;
  eigenMat.getZeroEigenSpace(theEigenSpaceList);
  Vectors<Complex<double> > theEigenSpace;
  theEigenSpace.operator=(theEigenSpaceList);
  DrawOperations tempDO;
  tempDO.initDimensions(dimension);
  for (int i = 0; i < dimension; i ++) {
    for (int j = 0; j < dimension; j ++) {
      tempDO.bilinearForm.elements[i][j] =
      this->getAmbientWeyl().cartanSymmetric.elements[i][j].getDoubleValue();
    }
  }
  outputBasis1.setSize(dimension);
  outputBasis2.setSize(dimension);
  if (theEigenSpace.size > 0) {
    if (coxeterNumber > 2) {
      for (int j = 0; j < dimension; j ++) {
        outputBasis1[j] = theEigenSpace[0][j].realPart;
        outputBasis2[j] = theEigenSpace[0][j].imaginaryPart;
      }
      tempDO.modifyToOrthonormalNoShiftSecond(outputBasis2, outputBasis1);
    } else if (coxeterNumber <= 2 && theEigenSpace.size > 1) {
      for (int j = 0; j < dimension; j ++) {
        outputBasis1[j] = theEigenSpace[0][j].realPart;
        outputBasis2[j] = theEigenSpace[1][j].realPart;
      }
      tempDO.modifyToOrthonormalNoShiftSecond(outputBasis2, outputBasis1);
    }
  }
}

void RootSubalgebra::computeDynkinDiagramKAndCentralizer() {
  this->simpleRootsReductiveSubalgebra = this->genK;
  this->dynkinDiagram.computeDiagramTypeModifyInput(this->simpleRootsReductiveSubalgebra, this->getAmbientWeyl());
  this->SimpleBasisCentralizerRoots.size = 0;
  for (int i = 0; i < this->getAmbientWeyl().rootsOfBorel.size; i ++) {
    if (this->rootIsInCentralizer(this->getAmbientWeyl().rootsOfBorel[i])) {
      this->SimpleBasisCentralizerRoots.addOnTop(this->getAmbientWeyl().rootsOfBorel[i]);
    }
  }
  this->theCentralizerDiagram.computeDiagramTypeModifyInput(this->SimpleBasisCentralizerRoots, this->getAmbientWeyl());
}

void RootSubalgebra::computeModuleDecompositionAmbientAlgebraDimensionsOnly() {
  MacroRegisterFunctionWithName("RootSubalgebra::computeModuleDecompositionAmbientAlgebraDimensionsOnly");
  this->moduleDecompoAmbientAlgebraDimensionsOnly.makeZero();
  for (int i = 0; i < this->getNumberOfModules(); i ++) {
    this->moduleDecompoAmbientAlgebraDimensionsOnly.addMonomial(MonomialVector(this->modules[i].size - 1), 1);
  }
}

void RootSubalgebra::computeCentralizerFromKModulesAndSortKModules() {
  MacroRegisterFunctionWithName("RootSubalgebra::computeCentralizerFromKModulesAndSortKModules");
  this->centralizerKmodules.initialize(this->modules.size);
  this->CentralizerRoots.size = 0;
  this->CentralizerRoots.reserve(this->modules.size);
  this->SimpleBasisCentralizerRoots.size = 0;
  this->SimpleBasisCentralizerRoots.reserve(this->modules.size);
  if (this->simpleRootsReductiveSubalgebra.size == 0) {
    if (this->modules.size != this->getOwnerLieAlgebra().getNumberOfGenerators()) {
      global.fatal << " bad number of modules!" << global.fatal;
    }
  } else {
    if (this->theDynkinType.isEqualToZero()) {
      global.fatal << "Simple basis is " << this->simpleRootsReductiveSubalgebra.toString() << " but Dynkin type is: "
      << this->theDynkinType.toString() << global.fatal;
    }
  }
  for (int i = 0; i < this->modules.size; i ++) {
    if (this->modules[i].size == 1) {
      this->centralizerKmodules.addSelectionAppendNewIndex(i);
      if (!this->WeightsModulesPrimalSimple[i][0].isEqualToZero()) {
        this->CentralizerRoots.addOnTop(this->WeightsModulesPrimalSimple[i][0]);
        this->SimpleBasisCentralizerRoots.addOnTop(this->WeightsModulesPrimalSimple[i][0]);
      }
    }
  }
  this->theCentralizerDiagram.computeDiagramTypeModifyInput(this->SimpleBasisCentralizerRoots, this->getAmbientWeyl());
  this->theCentralizerDiagram.getDynkinType(this->theCentralizerDynkinType);
  if (this->theDynkinType.isEqualToZero()) {
    if (this->theCentralizerDynkinType.getRank() + this->theDynkinType.getRank() != this->owner->owner->getRank()) {
      global.fatal << "Centralizer of " << this->theDynkinType.toString() << " computed to be "
      << this->theCentralizerDynkinType.toString()
      << " which is impossible. " << global.fatal;
    }
  }
}

void RootSubalgebra::computeExtremeWeightInTheSameKModule(
  const Vector<Rational>& input, Vector<Rational>& outputW, bool lookingForHighest
) {
  this->getAmbientWeyl().computeExtremeRootInTheSameKMod(this->simpleRootsReductiveSubalgebra, input, outputW, lookingForHighest);
}

void RootSubalgebra::computeHighestWeightInTheSameKModule(const Vector<Rational>& input, Vector<Rational>& outputHW) {
  this->computeExtremeWeightInTheSameKModule(input, outputHW, true);
}

void RootSubalgebra::computeLowestWeightInTheSameKModule(const Vector<Rational>& input, Vector<Rational>& outputLW) {
  this->computeExtremeWeightInTheSameKModule(input, outputLW, false);
}

bool RootSubalgebra::rootsDefineSubalgebra(Vectors<Rational>& theRoots) {
  Vector<Rational> tempRoot;
  for (int i = 0; i < theRoots.size; i ++) {
    if (!this->isARoot(theRoots[i])) {
      return false;
    }
    for (int j = i + 1; j < theRoots.size; j ++) {
      tempRoot = theRoots[i];
      tempRoot += theRoots[j];
      if (this->isARoot(tempRoot)) {
        if (!theRoots.contains(tempRoot)) {
          return false;
        }
      }
    }
  }
  return true;
}

bool RootSubalgebra::isSubalgebraBorelHighest(const Vector<Rational>& input) {
  for (int i = 0; i < this->simpleRootsReductiveSubalgebra.size; i ++) {
    if (this->isARootOrZero(input + this->simpleRootsReductiveSubalgebra[i])) {
      return false;
    }
  }
  return true;
}

bool RootSubalgebra::isSubalgebraBorelLowest(const Vector<Rational>& input) {
  for (int i = 0; i < this->simpleRootsReductiveSubalgebra.size; i ++) {
    if (this->isARootOrZero(input - this->simpleRootsReductiveSubalgebra[i])) {
      return false;
    }
  }
  return true;
}

bool RootSubalgebra::rootIsInCentralizer(const Vector<Rational>& input) {
  Vector<Rational> tempRoot;
  for (int i = 0; i < this->simpleRootsReductiveSubalgebra.size; i ++) {
    tempRoot = input + this->simpleRootsReductiveSubalgebra[i];
    if (this->isARoot(tempRoot) || tempRoot.isEqualToZero()) {
      return false;
    }
    tempRoot = input - this->simpleRootsReductiveSubalgebra[i];
    if (this->isARoot(tempRoot) || tempRoot.isEqualToZero()) {
      return false;
    }
  }
  return true;
}

void RootSubalgebra::writeLieBracketTableAndOppositeKModulesToFile(
  std::fstream &output, List<List<List<int> > >& inMultTable, List<int>& inOpposites
) {
  output << "pairing_table_size: " << inMultTable.size << "\n";
  for (int i = 0; i < inMultTable.size; i ++) {
    for (int j = 0; j < inMultTable.size; j ++) {
      output << inMultTable[i][j].size << " ";
      for (int k = 0; k < inMultTable[i][j].size; k ++) {
        output << inMultTable[i][j][k] << " ";
      }
    }
  }
  output << "\nopposites: ";
  for (int i = 0; i < inMultTable.size; i ++) {
    output << inOpposites[i] << " ";
  }
}

void RootSubalgebra::readLieBracketTableAndOppositeKModulesFromFile(
  std::fstream& input, List<List<List<int> > >& outMultTable, List<int>& outOpposites
) {
  std::string tempS;
  int tempI, theSize;
  input >> tempS >> theSize;
  outMultTable.setSize(theSize);
  outOpposites.setSize(theSize);
  for (int i = 0; i < theSize; i ++) {
    outMultTable[i].setSize(theSize);
    for (int j = 0; j < theSize; j ++) {
      input >> tempI;
      outMultTable[i][j].setSize(tempI);
      for (int k = 0; k < outMultTable[i][j].size; k ++) {
        input >> outMultTable[i][j][k];
      }
    }
  }
  input >> tempS;
  for (int i = 0; i < outMultTable.size; i ++) {
    input >> outOpposites[i];
  }
  if (tempS != "opposites:") {
    global.fatal << "Error reading from file. " << global.fatal;
  }
}

bool RootSubalgebra::rootIsInNilradicalParabolicCentralizer(Selection& positiveSimpleRootsSel, Vector<Rational>& input) {
  Vector<Rational> tempRoot;
  bool found = true;
  Vector<Rational> currentRoot = input;
  bool foundPositive = false;
  while (found) {
    found = false;
    for (int k = 0; k < this->SimpleBasisCentralizerRoots.size; k ++) {
      tempRoot = currentRoot - this->SimpleBasisCentralizerRoots[k];
      if (this->isARoot(tempRoot) || tempRoot.isEqualToZero()) {
        currentRoot = tempRoot;
        found = true;
        if (positiveSimpleRootsSel.selected[k]) {
          foundPositive = true;
        }
        if (currentRoot.isEqualToZero()) {
          return foundPositive;
        }
      }
    }
  }
  return false;
}

void RootSubalgebra::generatePossibleNilradicalsRecursive(
  PauseThread& PauseMutex,
  List<List<List<int> > >& multTable,
  List<Selection>& impliedSelections,
  List<int>& oppositeKmods,
  RootSubalgebras& owner,
  int indexInOwner
) {
  int& RecursionDepth = owner.RecursionDepthNilradicalsGeneration;
  List<int>& counters = owner.CountersNilradicalsGeneration;
  while (RecursionDepth > - 1) {
    while (counters[RecursionDepth] < this->modules.size) {
      if (!impliedSelections[RecursionDepth].selected[counters[RecursionDepth]]) {
        if (this->indexIsCompatibleWithPrevious(
          counters[RecursionDepth], RecursionDepth, multTable, impliedSelections, oppositeKmods, owner
        )) {
          RecursionDepth ++;
          counters[RecursionDepth] = counters[RecursionDepth - 1];
        }
      }
      counters[RecursionDepth] ++;
    }
    this->possibleNilradicalComputation(impliedSelections[RecursionDepth], owner, indexInOwner);
    RecursionDepth --;
    if (RecursionDepth > - 1) {
      counters[RecursionDepth] ++;
    }
    PauseMutex.safePointDontCallMeFromDestructors();
  }
}

bool RootSubalgebra::listHasNonSelectedIndexLowerThanGiven(int index, List<int>& tempList, Selection& tempSel) {
  for (int j = 0; j < tempList.size; j ++) {
    if (tempList[j] < index) {
      if (!tempSel.selected[tempList[j]]) {
        return false;
      }
    }
  }
  return true;
}

bool RootSubalgebra::indexIsCompatibleWithPrevious(
  int startIndex,
  int RecursionDepth,
  List<List<List<int> > >& multTable,
  List<Selection>& impliedSelections,
  List<int>& oppositeKmods,
  RootSubalgebras& owner
) {
  Selection& targetSel = impliedSelections[RecursionDepth + 1];
  Selection& originalSel = impliedSelections[RecursionDepth];
  targetSel = originalSel;
  targetSel.addSelectionAppendNewIndex(startIndex);
  for (int k = targetSel.cardinalitySelection - 1; k < targetSel.cardinalitySelection; k ++) {
    int tempI = targetSel.elements[k];
    for (int i = 0; i < targetSel.cardinalitySelection; i ++) {
      if (targetSel.selected[oppositeKmods[targetSel.elements[i]]]) {
        return false;
      }
      List<int>& tempList = multTable[tempI][targetSel.elements[i]];
      for (int j = 0; j < tempList.size; j ++) {
        if (tempList[j] < startIndex && !originalSel.selected[tempList[j]]) {
          return false;
        } else {
          targetSel.addSelectionAppendNewIndex(tempList[j]);
        }
      }
    }
  }
  if (!owner.approveKModuleSelectionWRTActionsNormalizerCentralizerNilradical(targetSel)) {
    return false;
  }
  return true;
}

void RootSubalgebra::possibleNilradicalComputation(Selection& selKmods, RootSubalgebras& owner, int indexInOwner) {
  this->numberOfNilradicalsAllowed ++;
  if (owner.flagCountingNilradicalsOnlyNoComputation) {
    owner.numNilradicalsBySA[indexInOwner] ++;
    this->makeProgressReportpossibleNilradicalComputation(owner);
    return;
  }
  //this->ComputeDebugString();
  if (this->flagFirstRoundCounting) {
    this->NumTotalSubalgebras = this->numberOfNilradicalsAllowed;
  }
  if (!this->flagFirstRoundCounting) {
    this->nilradicalKmodules = selKmods;
    if (!this->coneConditionHolds(owner, indexInOwner, owner.flagComputingLprohibitingWeights)) {
      this->numberOfConeConditionFailures ++;
      owner.numberOfConeConditionFailures ++;
      if (owner.flagStoringNilradicals) {
        List<List<int> >& currentSAList = owner.storedNilradicals[indexInOwner];
        List<int> newNilradical;
        newNilradical.setSize(selKmods.cardinalitySelection);
        for (int i = 0; i < selKmods.cardinalitySelection; i ++) {
          newNilradical[i] = selKmods.elements[i];
        }
        currentSAList.addOnTop(newNilradical);
      }
    } else {

      //the below commented out code should be incapsulated. It computes whether a given nilradical is a nilradical of a parabolic subalgebra.
      //this task is pushed on the end of the to-do list.
      /* owner.NumconeConditionHoldsBySSpart.objects[indexInOwner] ++;
      if (owner.ReportStringNonNilradicalParabolic == "") {
        this->computeRootsOfK();
        Vectors<Rational> tempNilradical; Vectors<Rational> tempOthers; Vectors<Rational> tempK;
        for (int i = 0; i < this->kModules.size; i ++)
          if (this->NilradicalKmods.selected[i])
            tempNilradical.addListOnTop(this->kModules.objects[i]);
          else
            tempOthers.addListOnTop(this->kModules.objects[i]);
        for (int i = 0; i < this->positiveRootsReductiveSubalgebra.size; i ++) {
          tempOthers.addOnTop(this->positiveRootsReductiveSubalgebra.objects[i]);
          tempOthers.addOnTop(-this->positiveRootsReductiveSubalgebra.objects[i]);
          tempK.addOnTop(this->positiveRootsReductiveSubalgebra.objects[i]);
        }
        if (Vectors<Rational>::conesIntersect(empNilradical, tempOthers, owner.ambientWeyl.cartanSymmetric.numberOfRows)) {
          Vectors<Rational> tempRoots; std::stringstream out; std::string tempS;
          this->ambientWeyl.getEpsilonCoordinates(tempNilradical, tempRoots);
          tempRoots.toStringEpsilonForm(tempS, true, false, false);
          out << tempS;
          this->ambientWeyl.getEpsilonCoordinates(tempK, tempRoots);
          tempRoots.toStringEpsilonForm(tempS, true, false, false);
          out << "\n\n" << tempS;
          owner.ReportStringNonNilradicalParabolic = out.str();
        }
      }*/
    }
  }
  this->makeProgressReportpossibleNilradicalComputation(owner);
}

void RootSubalgebra::makeProgressReportGeneratorAutomorphisms(int progress, int outOf, int found) {
  ProgressReport theReport;
  std::stringstream out4, out5;
  out5 << progress + 1 << " out of " << outOf << " checked; ";
  out5 << found << " found pos. generators";
  theReport.report(out5.str());
}

void RootSubalgebra::makeProgressReportpossibleNilradicalComputation(RootSubalgebras& owner) {
  MacroRegisterFunctionWithName("RootSubalgebra::makeProgressReportpossibleNilradicalComputation");
  if (!global.response.monitoringAllowed()) {
    return;
  }
  ProgressReport report1, report2, report3, report4, report5;
  if (this->flagMakingProgressReport) {
    std::stringstream out1, out2, out3, out4, out5;
    if (this->flagFirstRoundCounting) {
      out1 << "Counting ss part " << this->dynkinDiagram.toString();
      out2 << "# nilradicals for fixed ss part: " << this->NumTotalSubalgebras;
      owner.NumSubalgebrasCounted ++;
      out3 << owner.NumSubalgebrasCounted << " total subalgebras counted";
    } else {
      out1 << "Computing ss part " << this->dynkinDiagram.toString();
      out2 << this->numberOfNilradicalsAllowed << " Nilradicals processed out of " << this->NumTotalSubalgebras;
      owner.NumSubalgebrasProcessed ++;
      out3 << "Total # subalgebras processed: " << owner.NumSubalgebrasProcessed;
      out4 << "Num cone condition failures: " << owner.numberOfConeConditionFailures;
      out5 << "Num failures to find l-prohibiting relations: " << owner.badRelations.size;
      report4.report(out4.str());
      report5.report(out5.str());
    }
    report1.report(out1.str());
    report2.report(out2.str());
    report3.report(out3.str());
  }
}

void RootSubalgebra::generateKModuleLieBracketTable(List<List<List<int> > >& output, List<int>& oppositeKmods) {
  output.setSize(this->modules.size);
  oppositeKmods.setSize(this->modules.size);
  int numTotal = this->modules.size * this->modules.size;
  std::stringstream out;
  out << "Computing pairing table for the module decomposition of the root subalgebra of type "
  << this->dynkinDiagram.toString()
  << "\n<br>\nwith centralizer " << this->theCentralizerDiagram.toString();
  ProgressReport theReport;
  theReport.report(out.str());
  ProgressReport theReport2(10, GlobalVariables::Response::ReportType::general);
  for (int i = 0; i < this->modules.size; i ++) {
    output[i].setSize(this->modules.size);
    for (int j = 0; j < this->modules.size; j ++) {
      this->KModuleLieBracketKmodule(i, j, oppositeKmods, output[i][j]);
      if (theReport2.tickAndWantReport()) {
        std::stringstream out5;
        out5 << "Computing pairing table: " << i * this->modules.size + j + 1 << " out of " << numTotal;
        theReport2.report(out5.str());
      }
    }
  }
}

bool RootSubalgebra::isARoot(const Vector<Rational>& input) {
  if (input.size != this->getOwnerLieAlgebra().getRank()) {
    return false;
  }
  return this->getAmbientWeyl().rootSystem.contains(input);
}

bool RootSubalgebra::isARootOrZero(const Vector<Rational>& input) {
  return input.isEqualToZero() || this->isARoot(input);
}

void RootSubalgebra::KModuleLieBracketKmodule(int index1, int index2, List<int>& oppositeKmods, List<int>& output) {
  MacroRegisterFunctionWithName("RootSubalgebra::KModuleLieBracketKmodule");
  ElementSemisimpleLieAlgebra<Rational> theLieBracket;
  output.size = 0;
  for (int i = 0; i < this->modules[index1].size; i ++) {
    for (int j = 0; j < this->modules[index2].size; j ++) {
      Vector<Rational>& leftWeight = this->WeightsModulesPrimalSimple[index1][i];
      Vector<Rational>& rightWeight = this->WeightsModulesPrimalSimple[index2][j];
      this->getOwnerLieAlgebra().lieBracket(this->modules[index1][i],this->modules[index2][j], theLieBracket);
      if (theLieBracket.isEqualToZero()) {
        continue;
      }
      if (theLieBracket.isElementCartan()) {
        oppositeKmods[index1] = index2;
        oppositeKmods[index2] = index1;
        continue;
      }
      Vector<Rational> weightSum= leftWeight +rightWeight;
      for (int k = 0; k< this->WeightsModulesPrimalSimple.size; k++) {
        if (this->WeightsModulesPrimalSimple[k].getIndex(weightSum) != - 1) {
          output.addOnTopNoRepetition(k);
          break;
        }
      }
    }
  }
}

Vector<Rational> RootSubalgebra::getFundamentalCoordinatessOverSubalgebraSemisimplePart(const Vector<Rational>& inputGweightSimpleCoords) const {
  MacroRegisterFunctionWithName("RootSubalgebra::GetFundamentalCoordsOverK");
  Vector<Rational> output;
  output.makeZero(this->simpleRootsReductiveSubalgebra.size);
  for (int i = 0; i < this->simpleRootsReductiveSubalgebra.size; i ++) {
    output[i] = this->getAmbientWeyl().rootScalarCartanRoot(inputGweightSimpleCoords, this->simpleRootsReductiveSubalgebra[i]) * 2 /
    this->getAmbientWeyl().rootScalarCartanRoot(this->simpleRootsReductiveSubalgebra[i], this->simpleRootsReductiveSubalgebra[i]);
  }
  return output;
}

Vector<Rational> RootSubalgebra::getSimpleCoordinatesOverSubalgebraSemisimplePart(const Vector<Rational>& inputGweightSimpleCoords) const {
  MacroRegisterFunctionWithName("RootSubalgebra::getSimpleCoordinatesOverSubalgebraSemisimplePart");
  Vector<Rational> result;
  result.makeZero(this->simpleRootsReductiveSubalgebra.size);
  if (this->simpleRootsReductiveSubalgebra.size == 0) {
    return result;
  }
  for (int i = 0; i < this->simpleRootsReductiveSubalgebra.size; i ++) {
    result[i] = this->getAmbientWeyl().rootScalarCartanRoot(inputGweightSimpleCoords, this->simpleRootsReductiveSubalgebra[i]);
  }
  this->scalarProdInvertedMatrixOrdered.actOnVectorColumn(result);
  return result;
}

void RootSubalgebra::computeHighestVectorsHighestWeights() {
  MacroRegisterFunctionWithName("RootSubalgebra::computeHighestVectorsHighestWeights");
  this->highestVectors.setExpectedSize(this->getOwnerLieAlgebra().getNumberOfGenerators());
  this->highestWeightsPrimalSimple.setExpectedSize(this->getOwnerLieAlgebra().getNumberOfGenerators());
  this->highestWeightsNonPrimalFundamental.setExpectedSize(this->getOwnerLieAlgebra().getNumberOfGenerators());
  this->highestWeightsPrimalSimple.setSize(0);
  this->highestVectors.setSize(0);
  this->highestWeightsNonPrimalFundamental.setSize(0);
  ElementSemisimpleLieAlgebra<Rational> currentElt;
  List<Vector<Rational> >& ambientRootSystem= this->getAmbientWeyl().rootSystem;
  for (int i = 0; i <ambientRootSystem.size; i ++) {
    if (this->isSubalgebraBorelHighest(ambientRootSystem[i])) {
      currentElt.makeGGenerator(ambientRootSystem[i], this->getOwnerLieAlgebra());
      this->highestVectors.addOnTop(currentElt);
      this->highestWeightsPrimalSimple.addOnTop(ambientRootSystem[i]);
      this->highestWeightsNonPrimalFundamental.addOnTop(this->getFundamentalCoordinatessOverSubalgebraSemisimplePart(ambientRootSystem[i]));
    }
  }
  Vectors<Rational> cartanCentralizer;
  this->simpleRootsReductiveSubalgebra.getOrthogonalComplement(cartanCentralizer, &this->getAmbientWeyl().cartanSymmetric);
  Vector<Rational> zeroRoot;
  zeroRoot.makeZero(this->simpleRootsReductiveSubalgebra.size);
  for (int i = 0; i <cartanCentralizer.size; i ++) {
    currentElt.makeCartanGenerator(cartanCentralizer[i], this->getOwnerLieAlgebra());
    this->highestVectors.addOnTop(currentElt);
    this->highestWeightsPrimalSimple.addOnTop(currentElt.getRootIMustBeWeight());
    this->highestWeightsNonPrimalFundamental.addOnTop(zeroRoot);
  }
}

bool RootSubalgebra::compareLeftGreaterThanRight(const Vector<Rational>& weightLeft, const Vector<Rational>& weightRight) {
  Vector<Rational> KssPartLeft = this->getSimpleCoordinatesOverSubalgebraSemisimplePart(weightLeft);
  Vector<Rational> KssPartRight = this->getSimpleCoordinatesOverSubalgebraSemisimplePart(weightRight);
  if (KssPartLeft>KssPartRight) {
    return true;
  }
  if (KssPartLeft<KssPartRight) {
    return false;
  }
  return weightLeft>weightRight;
}

void RootSubalgebra::computeModuleFromHighestVector(int moduleIndex) {
  MacroRegisterFunctionWithName("RootSubalgebra::computeModuleFromHighestVector");
  HashedList<Vector<Rational> > currentWeights;
  Vectors<Rational> zeroSpace;
  Vector<Rational> currentWeight;
  currentWeights.setExpectedSize(this->getAmbientWeyl().rootSystem.size);
  currentWeights.addOnTop(this->highestWeightsPrimalSimple[moduleIndex]);
  if (this->highestWeightsPrimalSimple[moduleIndex].isEqualToZero()) {
    zeroSpace.addOnTop(this->highestVectors[moduleIndex].getCartanPart());
  } else {
    for (int j = 0; j<currentWeights.size; j ++) {
      for (int k = 0; k< this->simpleRootsReductiveSubalgebra.size; k++) {
        currentWeight = currentWeights[j]-this->simpleRootsReductiveSubalgebra[k];
        if (this->isARoot(currentWeight)) {
          currentWeights.addOnTopNoRepetition(currentWeight);
        }
        if (currentWeight.isEqualToZero()) {
          if (!zeroSpace.linearSpanContainsVector(currentWeights[j])) {
            zeroSpace.addOnTop(currentWeights[j]);
            currentWeights.addOnTop(- this->simpleRootsReductiveSubalgebra[k]);
          }
        }
      }
    }
    currentWeight.makeZero(this->getOwnerLieAlgebra().getRank());
    for (int i = 0; i <zeroSpace.size; i ++) {
      currentWeights.addOnTop(currentWeight);
    }
  }
  Vectors<Rational>& wPrimalSimple = this->WeightsModulesPrimalSimple[moduleIndex];
  Vectors<Rational>& wNONprimalFundamental = this->WeightsModulesNONPrimalFundamental[moduleIndex];
  Vectors<Rational>& wNONprimalSimple = this->WeightsModulesNONPrimalSimple[moduleIndex];
  wPrimalSimple = currentWeights;
  wPrimalSimple.quickSortDescendingCustom(*this);
  wNONprimalFundamental.setSize(wPrimalSimple.size);
  wNONprimalSimple.setSize(wPrimalSimple.size);
  for (int j = 0; j < wPrimalSimple.size; j ++) {
    wNONprimalFundamental[j] = this->getFundamentalCoordinatessOverSubalgebraSemisimplePart(wPrimalSimple[j]);
    wNONprimalSimple[j] = this->getSimpleCoordinatesOverSubalgebraSemisimplePart(wPrimalSimple[j]);
    if (this->isSubalgebraBorelLowest(wPrimalSimple[j]) || wPrimalSimple.size == 1) {
      this->LowestWeightsPrimalSimple[moduleIndex] = wPrimalSimple[j];
      if (j != wPrimalSimple.size - 1) {
        global.fatal << "Last module weight is not lowest. The simple basis is: "
        << this->simpleRootsReductiveSubalgebra.toString() << ". The lowest weight is "
        << this->LowestWeightsPrimalSimple[moduleIndex].toString() << " and the weights of the module are: "
        << wPrimalSimple.toString() << ". I think this shouldn't happen, should it?" << global.fatal;
      }
    }
  }
  List<ElementSemisimpleLieAlgebra<Rational> >& theMod = this->modules[moduleIndex];
  theMod.setSize(wPrimalSimple.size);
  int indexInZeroSpace = 0;
  for (int i = 0; i < wPrimalSimple.size; i ++) {
    ElementSemisimpleLieAlgebra<Rational>& currentElt = theMod[i];
    if (!wPrimalSimple[i].isEqualToZero()) {
      currentElt.makeGGenerator(wPrimalSimple[i], this->getOwnerLieAlgebra());
      continue;
    }
    currentElt.makeCartanGenerator(zeroSpace[indexInZeroSpace], this->getOwnerLieAlgebra());
    indexInZeroSpace ++;
  }
}

void RootSubalgebra::computeModulesFromHighestVectors() {
  MacroRegisterFunctionWithName("RootSubalgebra::computeModulesFromHighestVectors");
  this->checkScalarProductMatrixOrdered();
  this->WeightsModulesPrimalSimple.setSize(this->getNumberOfModules());
  this->WeightsModulesNONPrimalSimple.setSize(this->getNumberOfModules());
  this->WeightsModulesNONPrimalFundamental.setSize(this->getNumberOfModules());
  this->LowestWeightsPrimalSimple.setSize(this->getNumberOfModules());
  this->modules.setSize(this->getNumberOfModules());
  for (int i = 0; i < this->getNumberOfModules(); i ++) {
    this->computeModuleFromHighestVector(i);
  }
}

void RootSubalgebra::computeModuleDecomposition() {
  MacroRegisterFunctionWithName("RootSubalgebra::computeModuleDecomposition");
  this->ModuleDecompoHighestWeights.makeZero();
  Weight<Rational> theM;
  for (int i = 0; i < this->modules.size; i ++) {
    theM.weightFundamentalCoordinates = this->highestWeightsNonPrimalFundamental[i];
    this->ModuleDecompoHighestWeights.addMonomial(theM, 1);
  }
}

void RootSubalgebra::computeKModules() {
  MacroRegisterFunctionWithName("RootSubalgebra::computeKModules");
  this->computeRootsOfK();
  this->scalarProdInvertedMatrixOrdered = this->scalarProdMatrixOrdered;
  if (this->scalarProdInvertedMatrixOrdered.numberOfRows > 0) {
    this->scalarProdInvertedMatrixOrdered.invert();
  }
  this->computeHighestVectorsHighestWeights();
  this->computeModulesFromHighestVectors();
  this->computeModuleDecomposition();
  this->computeModuleDecompositionAmbientAlgebraDimensionsOnly();
  //Handle the elements of the Cartan.
  int dimFinal = 0;
  for (int i = 0; i < this->modules.size; i ++) {
    dimFinal += this->modules[i].size;
  }
  if (dimFinal != this->getOwnerLieAlgebra().getNumberOfGenerators()) {
    global.fatal << "Sum of k-module dimensions does not equal the dimension of the ambient Lie algebra. " << global.fatal;
  }
}

int RootSubalgebra::numberOfRootsInNilradical() {
  int result = 0;
  for (int i = 0; i < this->nilradicalKmodules.cardinalitySelection; i ++) {
    result += this->modules[this->nilradicalKmodules.elements[i]].size;
  }
  return result;
}

int RootSubalgebra::getIndexKModuleContainingRoot(const Vector<Rational>& input) {
  for (int i = 0; i < this->WeightsModulesPrimalSimple.size; i ++) {
    if (this->WeightsModulesPrimalSimple[i].contains(input)) {
      return i;
    }
  }
  return - 1;
}

bool RootSubalgebra::coneConditionHolds(
  RootSubalgebras& owner,
  int indexInOwner,
  Vectors<Rational>& NilradicalRoots,
  Vectors<Rational>& Ksingular,
  bool doextractRelations
) {
  Matrix<Rational> tempA, tempB;
  if (Vectors<Rational>::conesIntersect(NilradicalRoots, Ksingular, nullptr, nullptr)) {
    if (doextractRelations) {
      this->extractRelations(tempA, tempB, NilradicalRoots, owner, indexInOwner, Ksingular);
    }
    return false;
  }
  return true;
}

bool RootSubalgebra::coneConditionHolds(RootSubalgebras& owner, int indexInOwner, bool doextractRelations) {
  Vectors<Rational> NilradicalRoots;
  Vectors<Rational> Ksingular;
  if (this->modules.size == 0) {
    return true;
  }
  NilradicalRoots.size = 0;
  int counter = 0;
  for (int i = 0; i < this->nilradicalKmodules.cardinalitySelection; i ++) {
    Vectors<Rational>& tempKmod = this->WeightsModulesPrimalSimple[this->nilradicalKmodules.elements[i]];
    for (int j = 0; j < tempKmod.size; j ++) {
      NilradicalRoots.addOnTop(tempKmod[j]);
      counter ++;
    }
  }
  Ksingular.size = 0;
  for (int i = 0; i < this->modules.size; i ++) {
    if (!this->nilradicalKmodules.selected[i]) {
      Ksingular.addOnTop(this->highestWeightsPrimalSimple[i]);
    }
  }
  if (!this->coneConditionHolds(owner, indexInOwner, NilradicalRoots, Ksingular, doextractRelations)) {
    return false;
  } else {
    return true;
  }
}

bool RootSubalgebra::checkRankInequality() const {
  if ((this->theDynkinType.getRank() + this->theCentralizerDynkinType.getRank()) * 2 < this->owner->owner->getRank()) {
    global.fatal << "2*(Centralizer rank + rank) < ambient rank, "
    << "which is mathematically impossible. There was a programming error. "
    << global.fatal;
  }
  return true;
}

bool RootSubalgebra::checkForSmallRelations(ConeRelation& theRel, Vectors<Rational>& nilradicalRoots) {
  MacroRegisterFunctionWithName("RootSubalgebra::checkForSmallRelations");
  Vector<Rational> weightSum;
  bool tempBool;
  int tempI;
  for (int i = 0; i < this->modules.size; i ++) {
    if (!this->nilradicalKmodules.selected[i]) {
      for (int j = i + 1; j < this->modules.size; j ++) {
        if (!this->nilradicalKmodules.selected[j]) {
          weightSum = this->highestWeightsPrimalSimple[i];
          weightSum += this->highestWeightsPrimalSimple[j];
          if (!weightSum.isEqualToZero()) {
            theRel.BetaCoeffs.setSize(0);
            theRel.Betas.setSize(0);
            tempI = nilradicalRoots.getIndex(weightSum);
            if (tempI != - 1) {
              tempBool = true;
              theRel.BetaCoeffs.setSize(1);
              theRel.Betas.setSize(1);
              theRel.BetaCoeffs[0].makeOne();
              theRel.Betas[0] =(nilradicalRoots[tempI]);
            } else {
              tempBool =
              this->getAmbientWeyl().hasStronglyPerpendicularDecompositionWRT(
                weightSum, - 1, nilradicalRoots, theRel.Betas, theRel.BetaCoeffs, true
              );
            }
            if (tempBool) {
              theRel.Alphas.size = 0;
              theRel.AlphaCoeffs.size = 0;
              theRel.Alphas.addOnTop(this->highestWeightsPrimalSimple[i]);
              theRel.Alphas.addOnTop(this->highestWeightsPrimalSimple[j]);
              theRel.AlphaCoeffs.addOnTop(1);
              theRel.AlphaCoeffs.addOnTop(1);
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}

void RootSubalgebra::matrixToRelation(
  ConeRelation& output,
  Matrix<Rational>& matA,
  Matrix<Rational>& matX,
  int dimension,
  Vectors<Rational>& nilradicalRoots
) {
  output.AlphaCoeffs.size = 0;
  output.Alphas.size = 0;
  output.BetaCoeffs.size = 0;
  output.Betas.size = 0;
  Vector<Rational> tempRoot;
  tempRoot.setSize(dimension);
  matX.scaleToIntegralForMinimalRationalHeightNoSignChange();
  if (matA.numberOfColumns != matX.numberOfRows) {
    global.fatal << "Right matrix has different number of columns from "
    << "the number of rows of the left one. " << global.fatal;
  }
  for (int i = 0; i < matA.numberOfColumns; i ++) {
    if (!matX.elements[i][0].isEqualToZero()) {
      for (int j = 0; j < dimension; j ++) {
        tempRoot.objects[j].assign(matA.elements[j][i]);
      }
      if (!(matX.elements[i][0].denominatorShort == 1)) {
        global.fatal << "Matrix element not integer. " << global.fatal;
      }
      if (i < nilradicalRoots.size) {
        output.Betas.addOnTop(tempRoot);
        output.BetaCoeffs.addOnTop(matX.elements[i][0]);
      } else {
        tempRoot.negate();
        output.Alphas.addOnTop(tempRoot);
        output.AlphaCoeffs.addOnTop(matX.elements[i][0]);
      }
    }
  }
}

void RootSubalgebra::extractRelations(
  Matrix<Rational>& matA,
  Matrix<Rational>& matX,
  Vectors<Rational>& NilradicalRoots,
  RootSubalgebras& owner,
  int indexInOwner,
  Vectors<Rational>& Ksingular
) {
  int dimension = this->getOwnerLieAlgebra().getRank();
  Vector<Rational> tempRoot;
  tempRoot.makeZero(dimension);
  ConeRelation theRel; theRel.IndexOwnerRootSubalgebra = indexInOwner;
  if (owner.flagLookingForMinimalRels) {
    theRel.FixRightHandSide(*this, NilradicalRoots);
    theRel.makeLookCivilized(*this);
    owner.theMinRels.addRelationNoRepetition(theRel, owner);
  }
  bool tempBool = this->attemptTheTripleTrick(theRel, NilradicalRoots);
  if (tempBool) {
    this->NumRelationsWithStronglyPerpendicularDecomposition ++;
    theRel.makeLookCivilized(*this);
    owner.goodRelations.addRelationNoRepetition(theRel, owner);
  } else {
    //if (!this->checkForSmallRelations(theRel, NilradicalRoots))
    this->matrixToRelation(theRel, matA, matX, dimension, NilradicalRoots);
    this->makeGeneratingSingularVectors(theRel, NilradicalRoots);
    theRel.FixRightHandSide(*this, NilradicalRoots);
    theRel.makeLookCivilized(*this);
    theRel.computeRelationString(owner, true, true);
    if ((false)) {
      if (theRel.diagram.toString() == "C^{2}_3") {
        Selection tempSel;
        tempSel.initialize(Ksingular.size);
        LargeInteger tempNum = MathRoutines::nChooseK(Ksingular.size, 2);
        for (int i = 0; i < tempNum; i ++) {
          tempSel.incrementSelectionFixedCardinality(2);
          theRel.Alphas.setSize(2);
          theRel.AlphaCoeffs.setSize(2);
          theRel.Betas.size = 0;
          theRel.BetaCoeffs.size = 0;
          for (int j = 0; j < tempSel.cardinalitySelection; j ++) {
            theRel.Alphas[j] = Ksingular[tempSel.elements[j]];
          }
          if (theRel.IsStrictlyWeaklyProhibiting(*this, NilradicalRoots, owner, indexInOwner)) {
            break;
          }
        }
        if (!theRel.checkForBugs(*this, NilradicalRoots)) {
          global.fatal << "check for bugs failed. " << global.fatal;
        }
      }
      owner.badRelations.addOnTop(theRel);
    }
  }
}

bool RootSubalgebra::attemptTheTripleTrick(ConeRelation& theRel, Vectors<Rational>& NilradicalRoots) {
  Vectors<Rational> tempRoots;
  tempRoots.size = 0;
  for (int i = 0; i < this->modules.size; i ++) {
    if (!this->nilradicalKmodules.selected[i]) {
      if (this->isGeneratingSingularVectors(i, NilradicalRoots)) {
        tempRoots.addOnTop(this->highestWeightsPrimalSimple[i]);
      }
    }
  }
  return this->attemptTheTripleTrickWRTSubalgebra(theRel, tempRoots, NilradicalRoots);
}

bool RootSubalgebra::attemptTheTripleTrickWRTSubalgebra(
  ConeRelation& theRel, Vectors<Rational>& highestWeightsAllowed, Vectors<Rational>& NilradicalRoots
) {
  Vector<Rational> tempRoot, Accum;
  SelectionWithMaxMultiplicity tempSel;
  Vectors<Rational> chosenAlphas;
  int theRank = this->getOwnerLieAlgebra().getRank();
  DynkinDiagramRootSubalgebra theDiagram;
  for (int i = 2; i <= MathRoutines::maximum(highestWeightsAllowed.size, theRank); i ++) {
    tempSel.initMaxMultiplicity(highestWeightsAllowed.size, i);
    LargeInteger numberOfElements = tempSel.numberOfCombinationsOfCardinality(i);
    for (int j = 0; j < numberOfElements; j ++) {
      tempSel.incrementSubsetFixedCardinality(i);
      Accum.makeZero(this->getAmbientWeyl().cartanSymmetric.numberOfRows);
      chosenAlphas.size = 0;
      for (int k = 0; k < tempSel.elements.size; k ++) {
        tempRoot = highestWeightsAllowed[tempSel.elements[k]];
        chosenAlphas.addOnTop(tempRoot);
        tempRoot *= tempSel.multiplicities[tempSel.elements[k]];
        Accum += tempRoot;
      }
      theRel.Betas.size = 0; theRel.BetaCoeffs.size = 0;
      if (!Accum.isEqualToZero()) {
        if (this->getAmbientWeyl().hasStronglyPerpendicularDecompositionWRT(
          Accum, - 1, NilradicalRoots, theRel.Betas, theRel.BetaCoeffs, true
        )) {
          int startNumBetas = theRel.Betas.size;
          chosenAlphas.addListOnTop(theRel.Betas);
          theDiagram.computeDiagramTypeModifyInput(chosenAlphas, this->getAmbientWeyl());
          int theRank = theDiagram.rankTotal();
          if (
            theRank > 4 || theDiagram.toString() == "B^{2}_4" ||
            theDiagram.toString() == "C^{2}_4"
          ) {
            int goalNumBetas = 2;
            theRel.Betas.size = 0; theRel.BetaCoeffs.size = 0;
            for (int l = goalNumBetas - 1; l < startNumBetas; l ++) {
              if (this->getAmbientWeyl().hasStronglyPerpendicularDecompositionWRT(
                Accum, l + 1, NilradicalRoots, theRel.Betas, theRel.BetaCoeffs, true
              )) {
                break;
              }
            }
          }
          theRel.Alphas.setSize(tempSel.cardinalitySelectionWithoutMultiplicities());
          theRel.AlphaCoeffs.setSize(tempSel.elements.size);
          for (int k = 0; k < tempSel.elements.size; k ++) {
            theRel.Alphas[k] = highestWeightsAllowed[tempSel.elements[k]];
            theRel.AlphaCoeffs[k] = tempSel.multiplicities[tempSel.elements[k]];
          }
          return true;
        }
      }
    }
  }
  return false;
}

void RootSubalgebra::ensureAlphasDontSumToRoot(ConeRelation& theRel, Vectors<Rational>& NilradicalRoots) {
  Vector<Rational> alpha1, alpha2, beta1, tempRoot;
  bool madeChange = true;
  while (madeChange) {
    madeChange = false;
    for (int i = 0; i < theRel.Alphas.size; i ++) {
      for (int j = i + 1; j < theRel.Alphas.size; j ++) {
        beta1 = theRel.Alphas[i];
        beta1 += theRel.Alphas[j];
        if (this->isARootOrZero(beta1)) {
          this->computeHighestWeightInTheSameKModule(beta1, tempRoot);
          if (NilradicalRoots.contains(beta1)) {
            alpha1 = theRel.Alphas[i];
            alpha2 = theRel.Alphas[j];
            theRel.Alphas.setSize(2);
            theRel.AlphaCoeffs.setSize(2);
            theRel.Alphas[0] = alpha1;
            theRel.Alphas[1] = alpha2;
            theRel.AlphaCoeffs[0] = 1;
            theRel.AlphaCoeffs[1] = 1;
            theRel.Betas.setSize(1);
            theRel.BetaCoeffs.setSize(1);
            theRel.BetaCoeffs[0] = 1;
            theRel.Betas[0] = beta1;
            madeChange = false;
            break;
          } else {
            int changedIndex = i, otherIndex = j;
            Rational alpha1coeff, alpha2coeff;
            if (alpha1coeff.isGreaterThanOrEqualTo(alpha2coeff)) {
              changedIndex = j;
              otherIndex = i;
            }
            alpha1coeff = theRel.AlphaCoeffs[changedIndex];
            alpha2coeff = theRel.AlphaCoeffs[otherIndex];
            alpha2 = theRel.Alphas[otherIndex];
            alpha2coeff.subtract(alpha1coeff);
            madeChange = true;
            theRel.Alphas[changedIndex] = beta1;
            theRel.AlphaCoeffs[changedIndex] = alpha1coeff;
            if (alpha2coeff.isEqualToZero()) {
              theRel.Alphas.removeIndexSwapWithLast(otherIndex);
              theRel.AlphaCoeffs.removeIndexSwapWithLast(otherIndex);
            } else {
              theRel.Alphas[otherIndex] = alpha2;
              theRel.AlphaCoeffs[otherIndex] = alpha2coeff;
            }
          }
        }
      }
    }
  }
}

void RootSubalgebra::computeEpsilonCoordinatesWithRespectToSubalgebra() {
  this->kModulesKepsCoords.setSize(this->modules.size);
  this->kModulesgEpsCoords.setSize(this->modules.size);
  Vectors<Rational> EpsCoordsWRTk;
  Vectors<Rational> simpleBasisG;
  int dimension = this->getAmbientWeyl().cartanSymmetric.numberOfRows;
  simpleBasisG.setSize(dimension);
  for (int i = 0; i < dimension; i ++) {
    simpleBasisG[i].makeZero(dimension);
    simpleBasisG[i][i] = 1;
  }
  Vector<Rational> tempRoot, tempRoot2, tempRoot3;
  for (int i = 0; i < this->modules.size; i ++) {
    if (this->simpleRootsReductiveSubalgebra.size > 0) {
      EpsCoordsWRTk.size = 0;
      for (int j = 0; j < this->modules[i].size; j ++) {
        tempRoot.setSize(this->simpleRootsReductiveSubalgebra.size);
        for (int k = 0; k< this->simpleRootsReductiveSubalgebra.size; k ++) {
          this->getAmbientWeyl().rootScalarCartanRoot(
            this->WeightsModulesPrimalSimple[i][j], this->simpleRootsReductiveSubalgebra[k], tempRoot[k]
          );
        }
        this->scalarProdInvertedMatrixOrdered.actOnVectorColumn(tempRoot, tempRoot3);
        tempRoot2.makeZero(this->getAmbientWeyl().cartanSymmetric.numberOfRows);
        for (int j = 0; j < this->simpleRootsReductiveSubalgebra.size; j ++) {
          tempRoot2 += this->simpleRootsReductiveSubalgebra[j] * tempRoot3[j];
        }
        EpsCoordsWRTk.addOnTop(tempRoot2);
      }
      this->getAmbientWeyl().getEpsilonCoordinatesWRTsubalgebra(
        this->simpleRootsReductiveSubalgebra, EpsCoordsWRTk, this->kModulesKepsCoords[i]
      );
      this->getAmbientWeyl().getEpsilonCoordinatesWRTsubalgebra(
        simpleBasisG, this->WeightsModulesPrimalSimple[i], this->kModulesgEpsCoords[i]
      );
    } else {
      Vector<Rational> emptyV;
      this->kModulesgEpsCoords[i].initializeFillInObject(this->modules[i].size, emptyV);
    }
    Vector<Rational> tempRoot;
    if (this->kModulesKepsCoords[i].size > 0) {
      this->kModulesKepsCoords[i].average(tempRoot, this->kModulesKepsCoords[i][0].size);
      if (!tempRoot.isEqualToZero()) {
        global.fatal << "Root expected to be zero at this point. " << global.fatal;
      }
    }
  }
  this->getAmbientWeyl().getEpsilonCoordinatesWRTsubalgebra(this->simpleRootsReductiveSubalgebra, this->simpleRootsReductiveSubalgebra, this->SimpleBasisKEpsCoords);
  this->getAmbientWeyl().getEpsilonCoordinatesWRTsubalgebra(simpleBasisG, this->simpleRootsReductiveSubalgebra, this->SimpleBasisgEpsCoords);
}

bool RootSubalgebra::attemptExtensionToIsomorphismNoCentralizer(
  Vectors<Rational>& Domain,
  Vectors<Rational>& Range,
  int RecursionDepth,
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms* outputAutomorphisms,
  bool GenerateAllpossibleExtensions,
  bool* abortKmodule,
  Vectors<Rational>* additionalDomain,
  Vectors<Rational>* additionalRange
) {
  int CurrentRank = Domain.getRankElementSpan();
  if (CurrentRank != Range.getRankElementSpan()) {
    global.fatal << "Ranks do not coincide. " << global.fatal;
  }
  if (abortKmodule != nullptr) {
    *abortKmodule = false;
  }
  if (CurrentRank == this->getAmbientWeyl().cartanSymmetric.numberOfRows) {
    return this->isAnIsomorphism(Domain, Range, outputAutomorphisms, additionalDomain, additionalRange);
  }
  Vectors<Rational> domainRec = Domain;
  Vectors<Rational> rangeRec = Range;
  RootSubalgebra leftSA;
  RootSubalgebra rightSA;
  Rational tempRatD, tempRatR;
  Vector<Rational>& LastRootD = *domainRec.lastObject();
  Vector<Rational>& LastRootR = *rangeRec.lastObject();
  if (RecursionDepth != 0) {
    for (int i = 0; i <domainRec.size; i ++) {
      this->getAmbientWeyl().rootScalarCartanRoot(domainRec[i], LastRootD, tempRatD);
      this->getAmbientWeyl().rootScalarCartanRoot(rangeRec[i], LastRootR, tempRatR);
      if (!tempRatR.isEqualTo(tempRatD)) {
        return false;
      }
    }
  }
  leftSA.owner = this->owner;
  rightSA.owner = this->owner;
  leftSA.genK = domainRec;
  rightSA.genK=rangeRec;
  leftSA.computeEssentials();
  rightSA.computeEssentials();
  if (RecursionDepth!= 0) {
    if (
      leftSA.dynkinDiagram.toString() != rightSA.dynkinDiagram.toString() ||
      leftSA.theCentralizerDiagram.toString() != rightSA.theCentralizerDiagram.toString() ||
      rightSA.modules.size != leftSA.modules.size
    ) {
      if (abortKmodule != nullptr) {
        *abortKmodule = true;
      }
      return false;
    }
  }
  int counter = 0;
  domainRec.addOnTop(leftSA.highestWeightsPrimalSimple[counter]);
  while (domainRec.getRankElementSpan() == CurrentRank) {
    counter ++;
    if (leftSA.modules.size <= counter) {
      global.fatal << "Left subalgebra modules not allowed to be empty. " << global.fatal;
    }
    domainRec.removeIndexSwapWithLast(domainRec.size - 1);
    domainRec.addOnTop(leftSA.highestWeightsPrimalSimple[counter]);
  }
  //find a minimal possible new kmodule to throw in
  for (int i = 0; i < leftSA.modules.size; i ++) {
    if (leftSA.modules[i].size > leftSA.modules[counter].size) {
      domainRec.lastObject()->operator=(leftSA.highestWeightsPrimalSimple[i]);
      if (domainRec.getRankElementSpan() == CurrentRank) {
        domainRec.lastObject()->operator=(leftSA.highestWeightsPrimalSimple[counter]);
      } else {
        counter = i;
      }
    }
  }
  if (!(domainRec.getRankElementSpan() == CurrentRank + 1)) {
    global.fatal << "Ranks do not match. " << global.fatal;
  }
  Vectors<Rational>& firstKmodLeft = leftSA.WeightsModulesPrimalSimple[counter];
  bool result = false;
  bool tempBool;
  for (int i = 0; i < rightSA.modules.size; i ++) {
    if (firstKmodLeft.size == rightSA.modules[i].size) {
      for (int j = 0; j < firstKmodLeft.size; j ++) {
        rangeRec.addOnTop(rightSA.WeightsModulesPrimalSimple[i][j]);
        if (rangeRec.getRankElementSpan() != (CurrentRank + 1)) {
          continue;
        }
        if (this->attemptExtensionToIsomorphismNoCentralizer(
          domainRec,
          rangeRec,
          RecursionDepth + 1,
          outputAutomorphisms,
          GenerateAllpossibleExtensions,
          &tempBool,
          additionalDomain,
          additionalRange
        )) {
          if (!GenerateAllpossibleExtensions) {
            return true;
          } else {
            result = true;
          }
        } else {
          if (tempBool) {
            break;
          }
        }
        rangeRec.removeLastObject();
      }
    }
  }
  return result;
}

bool RootSubalgebra::isAnIsomorphism(
  Vectors<Rational>& domain,
  Vectors<Rational>& range,
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms* outputAutomorphisms,
  Vectors<Rational>* additionalDomain,
  Vectors<Rational>* additionalRange
) {
  Matrix<Rational> matB;
  Vectors<Rational> tempRoots;
  int dimension = this->getAmbientWeyl().cartanSymmetric.numberOfRows;
  tempRoots.setSize(dimension);
  matB.initialize(dimension, dimension);
  for (int i = 0; i < dimension; i ++) {
    for (int j = 0; j < dimension; j ++) {
      matB.elements[i][j] = domain[i][j];
    }
    tempRoots[i].makeZero(dimension);
  }
  matB.invert();
  Rational tempRat2;
  for (int k = 0; k < dimension; k ++) {
    for (int i = 0; i < dimension; i ++) {
      for (int j = 0; j < dimension; j ++) {
        tempRat2 = range[j][k];
        tempRat2.multiplyBy(matB.elements[i][j]);
        tempRoots[i][k] += tempRat2;
      }
    }
  }
  Vector<Rational> tempRoot;
  if (additionalDomain != nullptr) {
    for (int i = 0; i < additionalDomain->size; i ++) {
      additionalDomain->objects[i].getCoordinatesInBasis(tempRoots, tempRoot);
      if (!(tempRoot == additionalRange->objects[i])) {
        return false;
      }
    }
  }
  for (int i = 0; i < this->getAmbientWeyl().rootsOfBorel.size; i ++) {
    this->getAmbientWeyl().rootsOfBorel[i].getCoordinatesInBasis(tempRoots, tempRoot);
    if (!this->isARoot(tempRoot)) {
      return false;
    }
  }
  if (outputAutomorphisms != nullptr) {
    outputAutomorphisms->ExternalAutomorphisms.addOnTop(tempRoots);
  }
  return true;
}

void RootSubalgebra::toHTML(int index, FormatExpressions* format) {
  MacroRegisterFunctionWithName("RootSubalgebra::toHTML");
  this->checkInitialization();
  std::fstream output;
  std::stringstream myPath;
  myPath << this->owner->owner->fileNames.toStringVirtualFolderName();
  myPath << "rootSubalgebra_" << index + 1 << ".html";
  FileOperations::openFileCreateIfNotPresentVirtual(output, myPath.str(), false, true, false);
  output << "<html><title>" << this->getAmbientWeyl().dynkinType.toString() << " root subalgebra of type "
  << this->dynkinDiagram.toString() << "</title>";
  output << "<meta name = \"keywords\" content = \"" << this->getAmbientWeyl().dynkinType.toString()
  << " root subsystems, root subsystems, root systems";
  if (this->getAmbientWeyl().dynkinType.hasExceptionalComponent()) {
    output << ", exceptional Lie algebra";
  }
  output << " \">";
  output << "<body>" << this->toString(format) << "</body></html>";
  output.close();
}

std::string RootSubalgebra::toString(FormatExpressions* format) {
  MacroRegisterFunctionWithName("RootSubalgebra::toString");
  (void) format;//taking care of unused parameter warning in a portable way
  std::stringstream out;
  bool useLatex = false;
  bool useHtml = true;
  bool includeKEpsCoords = false;
  if (
    this->SimpleBasisgEpsCoords.size != this->simpleRootsReductiveSubalgebra.size ||
    this->SimpleBasisKEpsCoords.size != this->simpleRootsReductiveSubalgebra.size ||
    this->kModulesgEpsCoords.size != this->modules.size ||
    this->kModulesKepsCoords.size != this->modules.size
  ) {
    includeKEpsCoords = false;
  }
  out << "Type: " << HtmlRoutines::getMathNoDisplay(this->dynkinDiagram.toString());
  out << " (Dynkin type computed to be: " << HtmlRoutines::getMathNoDisplay(this->theDynkinType.toString()) << ")";
  out << "\n<br>\nSimple basis: " << this->simpleRootsReductiveSubalgebra.toString();
  out << "\n<br>\nSimple basis epsilon form: "
  << this->SimpleBasisgEpsCoords.toStringEpsilonForm(useLatex, useHtml, false);
  out << "\n<br>\nSimple basis epsilon form with respect to k: "
  << this->SimpleBasisKEpsCoords.toStringEpsilonForm(useLatex, useHtml, false);
  out << "<br>Number of outer autos with trivial action on orthogonal complement and extending to autos of ambient algebra: "
  << this->outerSAautosExtendingToAmbientAutosGenerators.elements.size;
  out << "<br>Number of outer autos with trivial action on orthogonal complement: "
  << this->outerSAautos.elements.size << ". ";
  out << "<br>\nC(k_{ss})_{ss}: " << this->theCentralizerDiagram.toString();
  out << "<br>\n simple basis centralizer: " << this->SimpleBasisCentralizerRoots.toString();
  out << "<hr>\n Number of k-submodules of g: " << this->modules.size;
  out << "<br>Module decomposition, fundamental coords over k: ";
  out << HtmlRoutines::getMathNoDisplay(this->ModuleDecompoHighestWeights.toString());
  out << "<br>\n";
  out << "\ng/k k-submodules<table border =\"1\">\n<tr><th>id</th><th>size</th>"
  << "<th>b\\cap k-lowest weight</th><th>b\\cap k-highest weight</th><th>Module basis</th><th>Weights epsilon coords</th>";
  if (includeKEpsCoords) {
    out << "<th>epsilon coords wrt k</th>";
  }
  out << "</tr>";
  this->kModulesgEpsCoords.setSize(this->modules.size);
  for (int i = 0; i < this->modules.size; i ++) {
    out << "\n<tr><td>Module " << i + 1 << "</td><td>" << this->modules[i].size << "</td>";
    out << "<td>" << this->LowestWeightsPrimalSimple[i].toString() << "</td>";
    out << "<td>" << this->highestWeightsPrimalSimple[i].toString() << "</td>";
    out << "<td>";
    for (int j = 0; j < this->modules[i].size; j ++) {
      out << this->modules[i][j].toString();
      if (j != this->modules[i].size - 1) {
        out << "<br>";
      }
    }
    out << "</td><td>";
    this->getAmbientWeyl().getEpsilonCoordinates(this->WeightsModulesPrimalSimple[i], this->kModulesgEpsCoords[i]);
    for (int j = 0; j < this->modules[i].size; j ++) {
      out << this->kModulesgEpsCoords[i][j].toStringEpsilonFormat();
      if (j != this->modules[i].size - 1) {
        out << "<br>";
      }
    }
    out << "</td>";
    if (includeKEpsCoords) {
      out << "<td>";
      out << "N/A";
      out << "</td>";
    }
    out << "</tr>";
  }
  out << "</table>";
  if (this->theMultTable.size != 0) {
    if (useHtml) {
      out << "\n\n Pairing table:\n\n";
    }
    if (useLatex) {
      out << "\n\n\\noindent Pairing table:\n\n\\noindent";
    }
    out << this->toStringLieBracketTable(useLatex, useHtml, *this) << "\n";
  }
  out << "<hr>Information about the subalgebra generation algorithm.";
  out << "<br>Heirs rejected due to having symmetric Cartan type outside "
  << "of list dictated by parabolic heirs: " << this->numHeirsRejectedBadAngleS
  << "<br>Heirs rejected due to not being maximally dominant: " << this->numHeirsRejectedNotMaximallyDominant
  << "<br>Heirs rejected due to not being maximal with respect to small Dynkin diagram automorphism that extends to "
  << "ambient automorphism: " << this->numHeirsRejectedNotMaximallyDominant
  << "<br>Heirs rejected due to having ambient Lie algebra "
  << "decomposition iso to an already found subalgebra: " << this->numHeirsRejectedSameModuleDecompo;
  if (this->indexInducingSubalgebra == - 1) {
    out << "<br>This subalgebra is not parabolically induced by anyone";
  } else {
    out << "<br>Parabolically induced by " << this->owner->toStringAlgebraLink(this->indexInducingSubalgebra);
  }
  out << "<br>Potential Dynkin type extensions: ";
  for (int i = 0; i < this->potentialExtensionDynkinTypes.size; i ++) {
    out << this->potentialExtensionDynkinTypes[i].toString() << ", ";
  }
  return out.str();
}

bool RootSubalgebra::isGeneratingSingularVectors(int indexKmod, Vectors<Rational>& NilradicalRoots) {
  Vector<Rational>& currentRoot = this->highestWeightsPrimalSimple[indexKmod];
  if (currentRoot.isEqualToZero()) {
    return false;
  }
  Vector<Rational> theSum;
  for (int i = 0; i < NilradicalRoots.size; i ++) {
    theSum = currentRoot + NilradicalRoots[i];
    if (this->isARootOrZero(theSum)) {
      if (!NilradicalRoots.contains(theSum)) {
        return false;
      }
    }
  }
  return true;
}

void RootSubalgebra::makeGeneratingSingularVectors(ConeRelation& theRelation, Vectors<Rational>& nilradicalRoots) {
  bool isMaximal = false;
  Vector<Rational> tempRoot;
  while (!isMaximal) {
    isMaximal = true;
    for (int i = 0; i < theRelation.AlphaCoeffs.size; i ++) {
      for (int j = 0; j < nilradicalRoots.size; j ++) {
        tempRoot = theRelation.Alphas[i];
        tempRoot += nilradicalRoots[j];
        if ((this->isARoot(tempRoot) || tempRoot.isEqualToZero()) &&(!nilradicalRoots.contains(tempRoot))) {
          this->computeHighestWeightInTheSameKModule(tempRoot, tempRoot);
          tempRoot -= theRelation.Alphas[i];
          theRelation.Alphas[i] += tempRoot;
          int tempI = theRelation.Betas.getIndex(tempRoot);
          if (tempI == - 1) {
            theRelation.Betas.addOnTop(tempRoot);
            theRelation.BetaCoeffs.addOnTop(theRelation.AlphaCoeffs[i]);
          } else {
            theRelation.BetaCoeffs[tempI] += theRelation.AlphaCoeffs[i];
          }
          isMaximal = false;
          break;
        }
        if (theRelation.Alphas[i].isEqualToZero()) {
          theRelation.Alphas.removeIndexSwapWithLast(i);
          theRelation.AlphaCoeffs.removeIndexSwapWithLast(i);
          i --;
          isMaximal = false;
          break;
        }
      }
    }
  }
}

void RootSubalgebra::getLinearCombinationFromMaxRankRootsAndExtraRoot(bool DoEnumeration) {
  int dimension = this->getAmbientWeyl().cartanSymmetric.numberOfRows;
  std::stringstream out2;
  std::stringstream out;
  out2 << this->toString() << "\n";
  Matrix<Rational> tempMat;
  this->simpleRootsReductiveSubalgebra.getMatrixRootsToRows(tempMat);
  tempMat.invert();
  int counter = 0;
  HashedList<Vector<Rational> >& AllRoots = this->getAmbientWeyl().rootSystem;
  for (int i = 0; i <AllRoots.size; i ++) {
    Vector<Rational> linComb;
    if (this->allRootsSubalgebra.getIndex(AllRoots[i]) == - 1) {
      for (int j = 0; j < dimension; j ++) {
        linComb[j].makeZero();
        for (int k = 0; k < dimension; k++) {
          Rational tempRat;
          tempRat.assign(tempMat.elements[k][j]);
          tempRat.multiplyBy(AllRoots[i][k]);
          linComb[j] += tempRat;
        }
      }
      int x = linComb.findLeastCommonMultipleDenominatorsTruncateToInt();
      linComb *= - x;
      std::string tempS;
      if (this->linearCombinationToString(AllRoots[i], x, linComb, tempS)) {
        out << tempS << "\n";
        counter ++;
        if (this->LowestWeightsPrimalSimple.getIndex(AllRoots[i]) != - 1) {
          out2 << tempS << "\n";
        }
      }
    }
  }
  out << "\\multicolumn{2}{|c|}{Number of relations: " << counter << " }\\\\\\hline";
  std::string tempS = out.str();
  out2 << "\n\n" << tempS;
  if (DoEnumeration) {
    this->testedRootsAlpha = this->LowestWeightsPrimalSimple;
    this->doKRootsEnumeration();
  }
//  this->getLinearCombinationFromMaxRankRootsAndExtraRootMethod2();
}

void RootSubalgebra::getLinearCombinationFromMaxRankRootsAndExtraRootMethod2() {
  int dimension = this->getAmbientWeyl().cartanSymmetric.numberOfRows;
  std::stringstream out;
  out << this->toString() << "\n\n";
  Vector<Rational> tempRoot;
  tempRoot = this->simpleRootsReductiveSubalgebra[0];
  this->computeHighestWeightInTheSameKModule(tempRoot, tempRoot);
  HashedList<Vector<Rational> >& AllRoots = this->getAmbientWeyl().rootSystem;
  for (int l = 0; l < this->simpleRootsReductiveSubalgebra.size; l ++) {
    Rational tempRat;
    this->getAmbientWeyl().rootScalarCartanRoot(tempRoot, this->simpleRootsReductiveSubalgebra[l], tempRat);
    if (!tempRat.isEqualToZero()) {
      int counter = 0;
      Vectors<Rational> tempRoots;
      tempRoots =(this->simpleRootsReductiveSubalgebra);
      tempRoots[l] =(tempRoot);
      Matrix<Rational> tempMat;
      tempRoots.getMatrixRootsToRows(tempMat);
      tempMat.invert();
      for (int i = 0; i <AllRoots.size; i ++) {
        Vector<Rational> linComb;
        if (this->allRootsSubalgebra.getIndex(AllRoots.objects[i]) == - 1) {
          for (int j = 0; j < dimension; j ++) {
            linComb[j].makeZero();
            for (int k = 0; k < dimension; k++) {
              Rational tempRat;
              tempRat.assign(tempMat.elements[k][j]);
              tempRat.multiplyBy(AllRoots[i][k]);
              linComb[j] += tempRat;
            }
          }
          int x = linComb.findLeastCommonMultipleDenominatorsTruncateToInt();
          linComb *= - x;
          std::string tempS;
          if (this->linearCombinationToStringDistinguishedIndex(l, AllRoots.objects[i], x, linComb, tempS)) {
            out << tempS << "\n";
            counter ++;
          }
        }
      }
      out << "\\multicolumn{2}{|c|}{Number of relations: " << counter << " }\\\\\\hline";
    }
  }
}

bool RootSubalgebra::linearCombinationToString(
  const Vector<Rational>& alphaRoot, int coeff, Vector<Rational>& linComb, std::string& output
) {
  int dimension = this->getAmbientWeyl().cartanSymmetric.numberOfRows;
  if (coeff == 1) {
    return false;
  }
  std::stringstream out;
  std::string tempS = alphaRoot.toString();
  out << "(" << tempS << ")&$";
  out << coeff << "\\alpha_" << dimension + 1;
  for (int i = 0; i < dimension; i ++) {
     //if (linComb.coordinates[i].isEqualToZero())
    //  return false;
    tempS = linComb[i].toString();
    if (tempS != "0") {
      if (tempS == "- 1" || tempS == "-1") {
        tempS = "-";
      }
      if (tempS == "1") {
        tempS = "+";
      }
      if (!(tempS[0] == '+')&& !(tempS[0] == '-')) {
        tempS.insert(0, "+");
      }
      out << tempS << "\\beta_" << i + 1;
    }
  }
  out << "= 0$\\\\";
  output = out.str();
  return true;
}

RootSubalgebra::RootSubalgebra() {
  this->flagDeallocated = false;
  this->owner = nullptr;
  this->NumGmodKtableRowsAllowedLatex = 35;
  this->flagMakingProgressReport = true;
  this->flagComputeConeCondition = true;
  this->initForNilradicalGeneration();
  this->initNoOwnerReset();
}

void RootSubalgebra::initNoOwnerReset() {
  this->indexInducingSubalgebra = - 1;
  this->numHeirsRejectedNotMaximallyDominant = 0;
  this->numHeirsRejectedSameModuleDecompo = 0;
  this->numHeirsRejectedBadAngleS = 0;
  this->numHeirsRejectedNotMaxWRTouterAuto = 0;
}

void RootSubalgebra::initForNilradicalGeneration() {
  this->numberOfNilradicalsAllowed = 0;
  this->numberOfConeConditionFailures = 0;
  this->NumRelationsWithStronglyPerpendicularDecomposition = 0;
  this->flagFirstRoundCounting = true;
}

std::string RootSubalgebra::toStringLieBracketTable(bool useLaTeX, bool useHtml, RootSubalgebra& owner) {
  MacroRegisterFunctionWithName("RootSubalgebra::toStringLieBracketTable");
  std::stringstream out;
  out << "\t";
  if (!(useLaTeX||useHtml)) {
    for (int i = 0; i < this->theMultTable.size; i ++) {
      out << i << "\t";
    }
  } else {
    if (useHtml) {
      out << "<table><tr><th></th>";
    }
    if (useLaTeX) {
      out << "\\begin{tabular}{c|";
      for (int i = 0; i < this->theMultTable.size; i ++) {
        out << "c";
      }
      out << "|} & ";
    }
    for (int i = 0; i < this->theMultTable.size; i ++) {
      if (useHtml) {
        out << "<th>";
      }
      out << i;
      if (useHtml) {
        out << "</th>";
      } else {
        if (i != this->theMultTable.size - 1) {
          out << "&";
        }
      }
    }
    if (useHtml) {
      out << "</tr>";
    }
    if (useLaTeX) {
      out << "\\\\\\hline";
    }
  }
  for (int i = 0; i < this->theMultTable.size; i ++) {
    if (useHtml) {
      out << "\n<tr><td>";
    }
    out << "\n" << i;
    if (useLaTeX) {
      out << " & ";
    }
    if (useHtml) {
      out << "</td>";
    }
    for (int j = 0; j < this->theMultTable[i].size; j ++) {
      if ((j == owner.CentralizerRoots.size - 1) && (i <= owner.CentralizerRoots.size - 1)) {
        if (useLaTeX) {
          out << "\\multicolumn{1}{c|}{";
        }
        if (useHtml) {
          out << "<td rhs =\"1\">";
        }
      } else {
        if (useHtml) {
          out << "<td>";
        }
      }
      for (int k = 0; k < this->theMultTable[i][j].size; k ++) {
        out << this->theMultTable[i][j][k] << ", ";
      }
      if (useLaTeX) {
        if ((j == owner.CentralizerRoots.size - 1) && (i <= owner.CentralizerRoots.size - 1)) {
          out << "}";
        }
        if (j != this->theMultTable[i].size - 1) {
          out << " & ";
        }
      }
      if (useHtml) {
        out << "</td>";
      }
    }
    if (useHtml) {
      out << "</tr>";
    }
    if (useLaTeX) {
      out << "\\\\";
      if (i == owner.CentralizerRoots.size - 1) {
        out << "\\cline{2-" << owner.CentralizerRoots.size + 1 << "}";
      }
    }
  }
  if (useLaTeX) {
    out << "\n\\hline opposite & ";
  }
  if (useHtml) {
    out << "<tr><td>opposite</td>";
  }
  for (int i = 0; i <owner.theOppositeKmods.size; i ++) {
    if (useHtml) {
      out << "<td>";
    }
    out << i << "/" << owner.theOppositeKmods.objects[i];
    if (useHtml) {
      out << "</td>";
    }
    if (useLaTeX) {
      if (i != owner.theOppositeKmods.size - 1) {
        if (useLaTeX) {
          out << " & ";
        } else {
          out << "\t";
        }
      }
    }
  }
  if (useLaTeX) {
    out << "\n\\end{tabular}";
  }
  return out.str();
}

WeylGroupData& RootSubalgebra::getAmbientWeyl() const {
  return this->getOwnerLieAlgebra().weylGroup;
}

WeylGroupAutomorphisms& RootSubalgebra::getAmbientWeylAutomorphisms() const {
  this->checkInitialization();
  return this->owner->theWeylGroupAutomorphisms;
}

bool RootSubalgebra::linearCombinationToStringDistinguishedIndex(
  int distinguished, Vector<Rational>& alphaRoot, int coeff, Vector<Rational>& linComb, std::string& output
) {
  int dimension = this->getAmbientWeyl().cartanSymmetric.numberOfRows;
  if (coeff == 1) {
    return false;
  }
  std::stringstream out;
  std::string tempS = alphaRoot.toString();
  out << "(" << tempS << ")&$";
  out << coeff << "\\alpha_" << dimension + 1;
  for (int i = 0; i < dimension; i ++) {
    tempS = linComb.objects[i].toString();
    if (tempS != "0") {
      if (tempS == "- 1" || tempS == "-1") {
        tempS = "-";
      }
      if (tempS == "1") {
        tempS = "+";
      }
      if (!(tempS[0] == '+') && !(tempS[0] == '-')) {
        tempS.insert(0, "+");
      }
      out << tempS;
      if (i != distinguished) {
        out << "\\beta_" << i + 1;
      } else {
        out << "\\gamma";
      }
    }
  }
  out << "= 0$\\\\";
  output = out.str();
  return true;
}

SemisimpleLieAlgebra& RootSubalgebra::getOwnerLieAlgebra() const {
  if (this->owner == nullptr) {
    global.fatal << "Attempt to "
    << "access ambient Lie algebra of non-initialized root subalgebras. " << global.fatal;
  }
  return *this->owner->owner;
}

bool RootSubalgebra::operator>(const RootSubalgebra& other) const {
  //current implementation does not work as expected in types E_7 and for large D_n's
  if (this->theDynkinType > other.theDynkinType) {
    return true;
  }
  if (other.theDynkinType > this->theDynkinType) {
    return false;
  }
  if (this->theCentralizerDynkinType > other.theCentralizerDynkinType) {
    return true;
  }
  if (other.theCentralizerDynkinType > this->theCentralizerDynkinType) {
    return false;
  }
  return false;//<-this might need to be fixed.
}

void RootSubalgebra::generatePossibleNilradicalsInit(List<Selection>& impliedSelections, int& parabolicsCounter) {
  impliedSelections.setSize(this->modules.size + 1);
  parabolicsCounter = 0;
}

void RootSubalgebra::generatePossibleNilradicals(
  PauseThread& PauseMutex,
  List<Selection>& impliedSelections,
  int& parabolicsCounter,
  bool useParabolicsInNilradical,
  RootSubalgebras& owner,
  int indexInOwner
) {
  this->generateKModuleLieBracketTable(this->theMultTable, this->theOppositeKmods);
  this->NumTotalSubalgebras = 0;
  Selection emptySel;
  emptySel.initialize(this->SimpleBasisCentralizerRoots.size);
  owner.computeActionNormalizerOfCentralizerIntersectNilradical(emptySel, *this);
  int numCycles = MathRoutines::twoToTheNth(this->SimpleBasisCentralizerRoots.size);
  List<Selection> StartingNilradicalsNoRepetition;
  StartingNilradicalsNoRepetition.reserve(numCycles);
  Selection tempSel, ParabolicsGenerator;
  if (!owner.flagNilradicalComputationInitialized) {
    owner.CountersNilradicalsGeneration.setSize(this->modules.size + 1);
  }
  if (owner.flagStoringNilradicals) {
    owner.storedNilradicals[indexInOwner].size = 0;
  }
  if (useParabolicsInNilradical) {
    this->flagFirstRoundCounting = false;
    ParabolicsGenerator.initialize(this->SimpleBasisCentralizerRoots.size);
    for (int i = 0; i < numCycles; i ++, ParabolicsGenerator.incrementSelection()) {
      tempSel.initialize(this->modules.size);
      for (int j = 0; j < this->CentralizerRoots.size; j ++) {
        if (this->rootIsInNilradicalParabolicCentralizer(ParabolicsGenerator, this->CentralizerRoots[j])) {
          tempSel.addSelectionAppendNewIndex(j);
        }
      }
      if (owner.flagUsingActionsNormalizerCentralizerNilradical) {
        owner.raiseSelectionUntilApproval(tempSel);
      }
      StartingNilradicalsNoRepetition.addOnTopNoRepetition(tempSel);
    }
    for (;
      parabolicsCounter<StartingNilradicalsNoRepetition.size;
      parabolicsCounter ++, owner.flagNilradicalComputationInitialized = false
    ) {
      if (!owner.flagNilradicalComputationInitialized) {
        impliedSelections[0] =(StartingNilradicalsNoRepetition[parabolicsCounter]);
        owner.RecursionDepthNilradicalsGeneration = 0;
        owner.CountersNilradicalsGeneration[0] = this->CentralizerRoots.size;
      }
      this->generatePossibleNilradicalsRecursive(
        PauseMutex, this->theMultTable, impliedSelections, this->theOppositeKmods, owner, indexInOwner
      );
    }
  } else {
    this->flagFirstRoundCounting = false;
    impliedSelections[0].initialize(this->modules.size);
    owner.RecursionDepthNilradicalsGeneration = 0;
    owner.CountersNilradicalsGeneration[0] = 0;
    this->generatePossibleNilradicalsRecursive(
      PauseMutex, this->theMultTable, impliedSelections, this->theOppositeKmods, owner, indexInOwner
    );
  }
}

bool RootSubalgebra::attemptExtensionToIsomorphism(
  Vectors<Rational>& domain,
  Vectors<Rational>& range,
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms *outputAutomorphisms,
  RootSubalgebras& inputOwner,
  bool* domainAndRangeGenerateNonIsoSAs
) {
  if (outputAutomorphisms != nullptr) {
    outputAutomorphisms->ExternalAutomorphisms.size = 0;
  }
  if (domainAndRangeGenerateNonIsoSAs != nullptr) {
    *domainAndRangeGenerateNonIsoSAs = false;
  }
  RootSubalgebra theDomainRootSA;
  RootSubalgebra theRangeRootSA;
  theDomainRootSA.owner = &inputOwner;
  theRangeRootSA.owner = &inputOwner;
  theDomainRootSA.genK = domain;
  theRangeRootSA.genK = range;
  theDomainRootSA.computeEssentials();
  theRangeRootSA.computeEssentials();
  if (
    theDomainRootSA.dynkinDiagram.toString() != theRangeRootSA.dynkinDiagram.toString() ||
    theDomainRootSA.theCentralizerDiagram.toString() != theRangeRootSA.theCentralizerDiagram.toString()
  ) {
    if (domainAndRangeGenerateNonIsoSAs != nullptr) {
      *domainAndRangeGenerateNonIsoSAs = true;
    }
    return false;
  }
  Vectors<Rational> isoDomain, isoRange;
  Permutation permComponentsCentralizer;
  List<int> tempList, tempPermutation2;
  SelectionWithDifferentMaxMultiplicities tempAutosCentralizer;
  List<List<List<int> > > CentralizerDiagramAutomorphisms;
  theDomainRootSA.theCentralizerDiagram.getAutomorphisms(CentralizerDiagramAutomorphisms);
  theDomainRootSA.theCentralizerDiagram.toString();
  tempAutosCentralizer.initPart1(CentralizerDiagramAutomorphisms.size);
  for (int i = 0; i <CentralizerDiagramAutomorphisms.size; i ++) {
    tempAutosCentralizer.capacities[i] = CentralizerDiagramAutomorphisms[i].size - 1;
  }
  tempList.setSize(theDomainRootSA.theCentralizerDiagram.sameTypeComponents.size);
  int tempSize = 0;
  for (int i = 0; i < theDomainRootSA.theCentralizerDiagram.sameTypeComponents.size; i ++) {
    tempList[i] = theDomainRootSA.theCentralizerDiagram.sameTypeComponents[i].size;
    tempSize += tempList[i];
  }
  permComponentsCentralizer.initPermutation(tempList, tempSize);
  int tempI2= permComponentsCentralizer.totalNumberSubsetsSmallInt();
  int NumAutosCentralizer = tempAutosCentralizer.totalNumberSubsetsSmallInt();
  permComponentsCentralizer.getPermutationLthElementIsTheImageofLthIndex(tempPermutation2);
  Matrix<Rational> tempMat;
  Selection tempSel;
  for (int i = 0; i <domain.size; i ++) {
    isoDomain.addOnTop(domain[i]);
    if (isoDomain.getRankElementSpan(&tempMat, &tempSel) < isoDomain.size) {
      isoDomain.removeLastObject();
    } else {
      isoRange.addOnTop(range[i]);
    }
  }
  if (isoRange.getRankElementSpan(&tempMat, &tempSel) < isoRange.size) {
    return false;
  }
  int givenSize = isoDomain.size;
  for (int j = 0; j < tempI2; j ++) {
    for (int l = 0; l < NumAutosCentralizer; l ++) {
      isoDomain.size = givenSize;
      isoRange.size = givenSize;
      theDomainRootSA.theCentralizerDiagram.getMapFromPermutation(
        isoDomain,
        isoRange,
        tempPermutation2,
        CentralizerDiagramAutomorphisms,
        tempAutosCentralizer,
        theRangeRootSA.theCentralizerDiagram
      );
      if (theDomainRootSA.attemptExtensionToIsomorphismNoCentralizer(
        isoDomain, isoRange, 0, outputAutomorphisms, false, nullptr, &domain, &range
      )) {
        if (outputAutomorphisms == nullptr) {
          return true;
        }
      }
      if (outputAutomorphisms != nullptr) {
        theDomainRootSA.makeProgressReportGeneratorAutomorphisms(
          l + NumAutosCentralizer * j, tempI2 * NumAutosCentralizer, outputAutomorphisms->ExternalAutomorphisms.size
        );
      }
      tempAutosCentralizer.incrementReturnFalseIfPastLast();
    }
    permComponentsCentralizer.incrementAndGetPermutation(tempPermutation2);
  }
  return false;
}

bool RootSubalgebra::generateIsomorphismsPreservingBorel(
  RootSubalgebra& right, SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms *outputAutomorphisms
) {
  if (this->dynkinDiagram.toString() != right.dynkinDiagram.toString()) {
    return false;
  }
  if (this->theCentralizerDiagram.toString() != right.theCentralizerDiagram.toString()) {
    return false;
  }
  if (outputAutomorphisms != nullptr) {
    outputAutomorphisms->ExternalAutomorphisms.size = 0;
    outputAutomorphisms->simpleRootsInner.size = 0;
    outputAutomorphisms->simpleRootsInner = this->SimpleBasisCentralizerRoots;
  }
  Vectors<Rational> isoDomain, isoRange;
  Permutation permComponents, permComponentsCentralizer;
  List<int> tempList, tempPermutation1, tempPermutation2;
  SelectionWithDifferentMaxMultiplicities tempAutos, tempAutosCentralizer;
  List<List<List<int> > > DiagramAutomorphisms, CentralizerDiagramAutomorphisms;
  this->dynkinDiagram.getAutomorphisms(DiagramAutomorphisms);
  this->theCentralizerDiagram.getAutomorphisms(CentralizerDiagramAutomorphisms);
  tempAutos.initPart1(DiagramAutomorphisms.size);
  tempAutosCentralizer.initPart1(CentralizerDiagramAutomorphisms.size);
  for (int i = 0; i < DiagramAutomorphisms.size; i ++) {
    tempAutos.capacities[i] = DiagramAutomorphisms[i].size - 1;
  }
  for (int i = 0; i < CentralizerDiagramAutomorphisms.size; i ++) {
    tempAutosCentralizer.capacities[i] = CentralizerDiagramAutomorphisms[i].size - 1;
  }
  tempList.setSize(this->dynkinDiagram.sameTypeComponents.size);
  int tempSize = 0;
  for (int i = 0; i < this->dynkinDiagram.sameTypeComponents.size; i ++) {
    tempList[i] = this->dynkinDiagram.sameTypeComponents[i].size;
    tempSize += tempList[i];
  }
  permComponents.initPermutation(tempList, tempSize);
  tempList.setSize(this->theCentralizerDiagram.sameTypeComponents.size);
  tempSize = 0;
  for (int i = 0; i < this->theCentralizerDiagram.sameTypeComponents.size; i ++) {
    tempList[i] = this->theCentralizerDiagram.sameTypeComponents[i].size;
    tempSize += tempList[i];
  }
  permComponentsCentralizer.initPermutation(tempList, tempSize);
  int tempI1;
  int NumAutos;
  tempI1 = permComponents.totalNumberSubsetsSmallInt();
  NumAutos = tempAutos.totalNumberSubsetsSmallInt();
  int tempI2 = permComponentsCentralizer.totalNumberSubsetsSmallInt();
  int NumAutosCentralizer = tempAutosCentralizer.totalNumberSubsetsSmallInt();
  permComponents.getPermutationLthElementIsTheImageofLthIndex(tempPermutation1);
  permComponentsCentralizer.getPermutationLthElementIsTheImageofLthIndex(tempPermutation2);
  for (int i = 0; i < tempI1; i ++) {
    for (int j = 0; j < tempI2; j ++) {
      for (int k = 0; k < NumAutos; k++) {
        for (int l = 0; l < NumAutosCentralizer; l ++) {
          isoDomain.size = 0; isoRange.size = 0;
          this->dynkinDiagram.getMapFromPermutation(
            isoDomain, isoRange, tempPermutation1, DiagramAutomorphisms, tempAutos, right.dynkinDiagram
          );
          this->theCentralizerDiagram.getMapFromPermutation(
            isoDomain,
            isoRange,
            tempPermutation2,
            CentralizerDiagramAutomorphisms,
            tempAutosCentralizer,
            right.theCentralizerDiagram
          );
          if (this->attemptExtensionToIsomorphismNoCentralizer(
            isoDomain, isoRange, 0, outputAutomorphisms, false, nullptr, nullptr, nullptr
          )) {
            if (outputAutomorphisms == nullptr) {
              return true;
            }
          }
          if (outputAutomorphisms != nullptr) {
            this->makeProgressReportGeneratorAutomorphisms(
              l + NumAutosCentralizer * (k + NumAutos * (j + i * tempI2)),
              tempI1 * tempI2 * NumAutos * NumAutosCentralizer,
              outputAutomorphisms->ExternalAutomorphisms.size
            );
          }
          tempAutosCentralizer.incrementReturnFalseIfPastLast();
        }
        tempAutos.incrementReturnFalseIfPastLast();
      }
      permComponentsCentralizer.incrementAndGetPermutation(tempPermutation2);
    }
    permComponents.incrementAndGetPermutation(tempPermutation1);
  }
  return false;
}

void RootSubalgebra::doKRootsEnumeration() {
  this->theKEnumerations.setSize(this->positiveRootsKConnectedComponents.size);
  this->theKComponentRanks.setSize(this->positiveRootsKConnectedComponents.size);
  Matrix<Rational> tempMat;
  Selection tempSel;
  for (int i = 0; i < this->positiveRootsKConnectedComponents.size; i ++) {
    this->theKEnumerations[i].initialize(this->positiveRootsKConnectedComponents[i].size);
    this->theKComponentRanks[i] =
    this->positiveRootsKConnectedComponents[i].getRankElementSpan(&tempMat, &tempSel);
  }
  this->doKRootsEnumerationRecursively(0);
}

void RootSubalgebra::doKRootsEnumerationRecursively(int indexEnumeration) {
  int theRank = this->theKComponentRanks[indexEnumeration];
  LargeInteger numberOfIterations = MathRoutines::nChooseK(this->positiveRootsKConnectedComponents[indexEnumeration].size, theRank);
  Selection& selection = this->theKEnumerations[indexEnumeration];
  selection.initialize(selection.numberOfElements);
  for (int i = 0; i < numberOfIterations; i ++) {
    this->theKEnumerations[indexEnumeration].incrementSelectionFixedCardinality(theRank);
    if (indexEnumeration < this->positiveRootsKConnectedComponents.size - 1) {
      this->doKRootsEnumerationRecursively(indexEnumeration + 1);
    } else {
      this->subalgebraEnumerationsToLinearCombinations();
    }
  }
}

void RootSubalgebra::subalgebraEnumerationsToLinearCombinations() {
  int dimension = this->getAmbientWeyl().cartanSymmetric.numberOfRows;
  Matrix<Rational> tempMat;
  Selection tempSelection;
  tempMat.initialize(dimension, dimension);
  int counter = 0;
  for (int i = 0; i < this->positiveRootsKConnectedComponents.size; i ++) {
    this->positiveRootsKConnectedComponents[i].selectionToMatrixAppend(
      this->theKEnumerations[i], dimension, tempMat, counter
    );
    counter += this->theKComponentRanks[i];
  }
  //tempMat.ComputeDebugString();
  if (tempMat.invert()) {
    //tempMat.ComputeDebugString();
    for (int l = 0; l < this->testedRootsAlpha.size; l ++) {
      Vector<Rational> linComb;
      Vector<Rational>& TestedRootAlpha = this->testedRootsAlpha[l];
      for (int j = 0; j < dimension; j ++) {
        linComb[j].makeZero();
        for (int k = 0; k < dimension; k ++) {
          Rational tempRat;
          tempRat.assign(tempMat.elements[k][j]);
          tempRat.multiplyBy(TestedRootAlpha[k]);
          linComb[j] += tempRat;
        }
      }
      int x = linComb.findLeastCommonMultipleDenominatorsTruncateToInt();
      linComb *= - x;
      bool foundBadCombination = true;
      for (int i = 0; i < dimension; i ++) {
        if (linComb[i].numeratorShort == - 1 || linComb[i].numeratorShort == 1) {
          foundBadCombination = false;
          break;
        }
      }
      if (foundBadCombination) {
        std::string tempS;
        this->linearCombinationToString(TestedRootAlpha, x, linComb, tempS);
      }
    }
  }
}

bool RootSubalgebra::checkConsistency() const {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of root subalgebra. " << global.fatal;
    return false;
  }
  return true;
}

bool RootSubalgebra::checkInitialization() const {
  this->checkConsistency();
  if (this->owner == nullptr) {
    global.fatal << "Root subalgebra is not initialized properly. " << global.fatal;
    return false;
  }
  return true;
}

bool RootSubalgebra::checkScalarProductMatrixOrdered() const {
  Matrix<Rational> theMat;
  this->simpleRootsReductiveSubalgebra.getGramMatrix(theMat, &this->getAmbientWeyl().cartanSymmetric);
  if (theMat != this->scalarProdMatrixOrdered) {
    global.fatal << "Bilinear product matrix does not match the stored value. " << global.fatal;
  }
  return true;
}

void RootSubalgebra::computePotentialExtensions() {
  MacroRegisterFunctionWithName("RootSubalgebra::computePotentialExtensions");
  this->owner->growDynkinType(
    this->theDynkinType, this->potentialExtensionDynkinTypes, &this->potentialExtensionRootPermutations
  );
  this->potentialExtensionCartanSymmetrics.setSize(this->potentialExtensionDynkinTypes.size);
  for (int i = 0; i < this->potentialExtensionDynkinTypes.size; i ++) {
    this->potentialExtensionDynkinTypes[i].getCartanSymmetric(this->potentialExtensionCartanSymmetrics[i]);
  }
}

bool RootSubalgebras::growDynkinType(
  const DynkinType& input, List<DynkinType>& output, List<List<int> >* outputPermutationSimpleRoots
) const {
  MacroRegisterFunctionWithName("RootSubalgebras::growDynkinType");
  input.grow(this->validScales, this->getOwnerWeyl().getDimension(), output, outputPermutationSimpleRoots);
  char theLetter;
  if (!this->owner->weylGroup.dynkinType.isSimple(&theLetter)) {
    return true;
  }
  for (int i = output.size - 1; i >= 0; i --) {
    bool isGood = true;
    if (theLetter == 'A') {
      if (
        output[i].containsType('B') || output[i].containsType('C') || output[i].containsType('D') ||
        output[i].containsType('E') || output[i].containsType('F') || output[i].containsType('G')
      ) {
        isGood = false;
      }
    }
    if (theLetter == 'B') {
      if (
        output[i].containsType('C') || output[i].containsType('E') ||
        output[i].containsType('F') || output[i].containsType('G')
      ) {
        isGood = false;
      }
    }
    if (theLetter == 'C') {
      if (
        output[i].containsType('D') || output[i].containsType('E') ||
        output[i].containsType('F') || output[i].containsType('G')
      ) {
        isGood = false;
      }
    }
    if (theLetter == 'D') {
      if (
        output[i].containsType('B') || output[i].containsType('C') ||
        output[i].containsType('E') || output[i].containsType('F') || output[i].containsType('G')
      ) {
        isGood = false;
      }
    }
    if (theLetter == 'E') {
      if (
        output[i].containsType('B') || output[i].containsType('C') ||
        output[i].containsType('F') || output[i].containsType('G')
      ) {
        isGood = false;
      }
    }
    if (theLetter == 'F') {
      if (output[i].containsType('G')) {
        isGood = false;
      }
    }
    if (theLetter == 'G') {
      if (output[i].containsType('B')) {
        isGood = false;
      }
    }
    if (!isGood) {
      output.removeIndexSwapWithLast(i);
      if (outputPermutationSimpleRoots != nullptr) {
        outputPermutationSimpleRoots->removeIndexSwapWithLast(i);
      }
    }
  }
  return true;
}

void RootSubalgebra::computeOuterSubalgebraAutomorphismsExtendingToAmbientAutomorphismsGenerators() {
  MacroRegisterFunctionWithName("RootSubalgebra::computeOuterSubalgebraAutomorphismsExtendingToAmbientAutomorphismsGenerators");
  if (this->simpleRootsReductiveSubalgebra.size == 0) {
    return;
  }
  List<MatrixTensor<Rational> > outerAutos;
  this->theDynkinType.getOuterAutosGeneratorsActOnVectorColumn(outerAutos);
  Matrix<Rational> simpleBasisMatrixTimesCartanSymm;
  simpleBasisMatrixTimesCartanSymm.assignVectorsToRows(this->simpleRootsReductiveSubalgebra);
  simpleBasisMatrixTimesCartanSymm *= this->getAmbientWeyl().cartanSymmetric;
  Vectors<Rational> basisOrthogonalRoots;
  simpleBasisMatrixTimesCartanSymm.getZeroEigenSpaceModifyMe(basisOrthogonalRoots);
  Vectors<Rational> imagesWeightBasis, weightBasis = this->simpleRootsReductiveSubalgebra;
  weightBasis.addListOnTop(basisOrthogonalRoots);
  Matrix<Rational> basisMatrixInverted, resultingOperator;
  basisMatrixInverted.assignVectorsToColumns(weightBasis);
  basisMatrixInverted.invert();
  this->outerSAautos.generators.setSize(outerAutos.size);
  for (int i = 0; i < outerAutos.size; i ++) {
    outerAutos[i].actOnVectorROWSOnTheLeft(this->simpleRootsReductiveSubalgebra, imagesWeightBasis);
    imagesWeightBasis.addListOnTop(basisOrthogonalRoots);
    resultingOperator.assignVectorsToColumns(imagesWeightBasis);
    resultingOperator *= basisMatrixInverted;
    this->outerSAautos.generators[i] = resultingOperator;
  }
  this->outerSAautos.generateElements(0);
  this->outerSAautosExtendingToAmbientAutosGenerators.elements.clear();
  for (int i = 0; i < this->outerSAautos.elements.size; i ++) {
    if (this->getAmbientWeylAutomorphisms().isElementWeylGroupOrOuterAutomorphisms(this->outerSAautos.elements[i])) {
      this->outerSAautosExtendingToAmbientAutosGenerators.elements.addOnTop(this->outerSAautos.elements[i]);
    }
  }
}

bool RootSubalgebra::checkForMaximalDominanceCartanSubalgebra() {
  MacroRegisterFunctionWithName("RootSubalgebra::checkForMaximalDominanceCartanSubalgebra");
  Vectors<Rational> simpleBasisOriginalOrderCopy;
  for (int i = 0; i < this->outerSAautos.elements.size; i ++) {
    if (!this->outerSAautos.elements[i].isIdentity()) {
      simpleBasisOriginalOrderCopy = this->simpleBasisKinOrderOfGeneration;
      this->outerSAautos.elements[i].actOnVectorsColumn(simpleBasisOriginalOrderCopy);
      this->getAmbientWeylAutomorphisms().raiseToMaximallyDominant(simpleBasisOriginalOrderCopy);
      for (int j = 0; j < simpleBasisOriginalOrderCopy.size; j ++) {
        if (simpleBasisOriginalOrderCopy[j] != this->simpleBasisKinOrderOfGeneration[j]) {
          if (simpleBasisOriginalOrderCopy[j].IsGreaterThanLexicographic(this->simpleBasisKinOrderOfGeneration[j])) {
            if (this->indexInducingSubalgebra != - 1) {
              this->owner->subalgebras[this->indexInducingSubalgebra].numHeirsRejectedNotMaxWRTouterAuto ++;
            }
            return false;
          } else {
            break;
          }
        }
      }
    }
  }
  return true;
}

void RootSubalgebra::computeEssentials() {
  MacroRegisterFunctionWithName("RootSubalgebra::computeEssentials");
  this->simpleRootsReductiveSubalgebra = this->genK;
  this->simpleRootsReductiveSubalgebra.getGramMatrix(this->scalarProdMatrixOrdered, &this->getAmbientWeyl().cartanSymmetric);
  this->dynkinDiagram.ambientRootSystem= this->getAmbientWeyl().rootSystem;
  this->dynkinDiagram.ambientBilinearForm= this->getAmbientWeyl().cartanSymmetric;
  this->dynkinDiagram.computeDiagramInputIsSimple(this->simpleRootsReductiveSubalgebra);
  this->dynkinDiagram.getDynkinType(this->theDynkinType);
  if (this->simpleRootsReductiveSubalgebra.size != 0) {
    if (this->theDynkinType.toString() == "0") {
      global.fatal << "Subalgebra dynkin type computed to be zero while the simple basis is: "
      << this->simpleRootsReductiveSubalgebra.toString() << ". " << global.fatal;
    }
  }
  this->computeKModules();
  this->computeCentralizerFromKModulesAndSortKModules();
  this->computeModuleDecompositionAmbientAlgebraDimensionsOnly();
  this->checkRankInequality();
  this->nilradicalKmodules.initialize(this->modules.size);
}

bool RootSubalgebra::computeEssentialsIfNew() {
  MacroRegisterFunctionWithName("RootSubalgebra::computeEssentialsIfNew");
  this->genK = this->simpleRootsReductiveSubalgebra;
  this->checkInitialization();
  ProgressReport report;
  std::stringstream reportStream;
  this->simpleBasisKScaledToActByTwo = this->simpleRootsReductiveSubalgebra;
  for (int i = 0; i < this->simpleRootsReductiveSubalgebra.size; i ++) {
    this->simpleBasisKScaledToActByTwo[i] *= 2 / this->getAmbientWeyl().rootScalarCartanRoot(
      this->simpleRootsReductiveSubalgebra[i], this->simpleRootsReductiveSubalgebra[i]
    );
  }
  if (report.tickAndWantReport()) {
    reportStream << "Computing root subalgebra... ";
    report.report(reportStream.str());
  }
  if (this->indexInducingSubalgebra != - 1) {
    this->simpleRootsReductiveSubalgebra.getGramMatrix(this->scalarProdMatrixPermuted, &this->getAmbientWeyl().cartanSymmetric);
    int goodPermutation = - 1;
    List<List<int> >& extensionRootPermutations =
    this->owner->subalgebras[this->indexInducingSubalgebra].potentialExtensionRootPermutations;
    List<Matrix<Rational> >& extensionCartanSymmetrics =
    this->owner->subalgebras[this->indexInducingSubalgebra].potentialExtensionCartanSymmetrics;
    for (int i = 0; i <extensionRootPermutations.size && goodPermutation == - 1; i ++) {
      this->scalarProdMatrixOrdered.makeZeroMatrix(this->simpleRootsReductiveSubalgebra.size);
      for (int j = 0; j < this->simpleRootsReductiveSubalgebra.size; j ++) {
        for (int k = 0; k < this->simpleRootsReductiveSubalgebra.size; k ++) {
          this->scalarProdMatrixOrdered(
            extensionRootPermutations[i][j], extensionRootPermutations[i][k]
          ) = this->scalarProdMatrixPermuted(j, k);
        }
      }
      if (this->scalarProdMatrixOrdered == extensionCartanSymmetrics[i]) {
        goodPermutation = i;
        break;
      }
    }
    if (goodPermutation == - 1) {
      this->owner->subalgebras[this->indexInducingSubalgebra].numHeirsRejectedBadAngleS ++;
      return false;
    }
    Vectors<Rational> copySimpleBasisK = this->simpleRootsReductiveSubalgebra;
    for (int i = 0; i < this->simpleRootsReductiveSubalgebra.size; i ++) {
      this->simpleRootsReductiveSubalgebra[extensionRootPermutations[goodPermutation][i]] = copySimpleBasisK[i];
    }
  } else {
    this->simpleRootsReductiveSubalgebra.getGramMatrix(this->scalarProdMatrixOrdered, &this->getAmbientWeyl().cartanSymmetric);
  }
  if (report.tickAndWantReport()) {
    reportStream << "...found a candidate type... ";
    report.report(reportStream.str());
  }
  if (this->simpleRootsReductiveSubalgebra.getRankElementSpan() != this->simpleRootsReductiveSubalgebra.size) {
    global.fatal << "<br>simple basis vectors not linearly independent! " << global.fatal;
  }
  if (!this->getAmbientWeylAutomorphisms().areMaximallyDominantGroupOuter(this->simpleBasisKinOrderOfGeneration)) {
    Vectors<Rational> tempVs = this->simpleBasisKinOrderOfGeneration;
    tempVs.removeLastObject();
    if (!this->getAmbientWeylAutomorphisms().areMaximallyDominantGroupOuter(tempVs)) {
      global.fatal << "<br>First vectors "
      << tempVs.toString() << " are not maximally dominant. " << global.fatal;
    }
    if (this->indexInducingSubalgebra != - 1) {
      this->owner->subalgebras[this->indexInducingSubalgebra].numHeirsRejectedNotMaximallyDominant ++;
    }
    return false;
  }
  if (report.tickAndWantReport()) {
    reportStream << "...the candidate's roots are maximally dominant... ";
    report.report(reportStream.str());
  }
  this->dynkinDiagram.ambientBilinearForm = this->getAmbientWeyl().cartanSymmetric;
  this->dynkinDiagram.ambientRootSystem = this->getAmbientWeyl().rootSystem;
  this->dynkinDiagram.computeDiagramInputIsSimple(this->simpleRootsReductiveSubalgebra);
  this->dynkinDiagram.getDynkinType(this->theDynkinType);
  this->computeKModules();
  this->computeCentralizerFromKModulesAndSortKModules();
  this->computeModuleDecompositionAmbientAlgebraDimensionsOnly();
  this->checkRankInequality();
  for (int i = 0; i < this->owner->subalgebras.size; i ++) {
    if (
      this->owner->subalgebras[i].dynkinDiagram == this->dynkinDiagram &&
      this->owner->subalgebras[i].theCentralizerDynkinType == this->theCentralizerDynkinType &&
      this->moduleDecompoAmbientAlgebraDimensionsOnly == this->owner->subalgebras[i].moduleDecompoAmbientAlgebraDimensionsOnly
    ) {
      if (this->indexInducingSubalgebra != - 1) {
        this->owner->subalgebras[this->indexInducingSubalgebra].numHeirsRejectedSameModuleDecompo ++;
      }
      return false;
    }
  }
  if (report.tickAndWantReport()) {
    reportStream << "...module decomposition computed, subalgebra type: "
    << this->theDynkinType.toString()
    << ", centralizer type: " << this->theCentralizerDynkinType.toString()
    << ". Computing outer automorphisms that "
    << "have zero action on centralizer and extend to ambient automorphisms... ";
    report.report(reportStream.str());
  }
  return true;
}

bool RootSubalgebra::isEquivalentToByDiagramsAndDimensions(const RootSubalgebra& other) const {
  return
  this->moduleDecompoAmbientAlgebraDimensionsOnly ==
  other.moduleDecompoAmbientAlgebraDimensionsOnly &&
  this->theDynkinType == other.theDynkinType &&
  this->theCentralizerDynkinType == other.theCentralizerDynkinType;
}

void RootSubalgebra::generateAutomorphismsPreservingBorel(
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms &outputAutomorphisms
) {
  this->computeEssentials();
  this->generateIsomorphismsPreservingBorel(*this, &outputAutomorphisms);
}

void RootSubalgebras::computeAllReductiveRootSubalgebrasUpToIsomorphismOLD(
  bool sort, bool computeEpsilonCoordinates
) {
  MacroRegisterFunctionWithName("RootSubalgebras::computeAllReductiveRootSubalgebrasUpToIsomorphismOLD");
  this->subalgebras.size = 0;
  this->getOwnerWeyl().computeRho(true);
  //this->initDynkinDiagramsNonDecided(this->ambientWeyl, WeylLetter, WeylRank);
  RootSubalgebras rootSAsGenerateAll;
  rootSAsGenerateAll.subalgebras.setSize(this->GetOwnerSSalgebra().getRank()*2+ 1);
  rootSAsGenerateAll.subalgebras[0].genK.size = 0;
  rootSAsGenerateAll.subalgebras[0].owner = this;
  rootSAsGenerateAll.subalgebras[0].computeEssentials();
  this->computeAllReductiveRootSubalgebrasContainingInputUpToIsomorphismOLD(rootSAsGenerateAll.subalgebras, 1);
  if (sort) {
    this->sortDescendingOrderBySSRank();
  }
  if (computeEpsilonCoordinates) {
    for (int i = 0; i < this->subalgebras.size; i ++) {
      this->subalgebras[i].computeEpsilonCoordinatesWithRespectToSubalgebra();
    }
  }
}


bool SlTwoSubalgebra::operator>(const SlTwoSubalgebra& right) const {
  MacroRegisterFunctionWithName("SlTwoSubalgebra::operatorGreaterThan");
  if (this->owner != right.owner) {
    global.fatal << "Error: comparing sl(2) subalgebras with different owners." << global.fatal;
  }
  if ( this->lengthHSquared > right.lengthHSquared) {
    return true;
  }
  if (right.lengthHSquared > this->lengthHSquared) {
    return false;
  }
  return this->hCharacteristic > right.hCharacteristic;
}

bool SlTwoSubalgebra::operator==(const SlTwoSubalgebra& right) const {
 // See Dynkin, Semisimple Lie subalgebras of semisimple Lie algebras, chapter 7-10
  if (this->owner != right.owner) {
    global.fatal << "Comparing sl(2) "
    << "subalgebras that have different ambient Lie algebras. " << global.fatal;
  }
  return this->hCharacteristic == right.hCharacteristic;
}

std::string SlTwoSubalgebra::toStringTripleVerification(FormatExpressions* format) const {
  std::stringstream out;
  bool useHtml = format == nullptr ? true : format->flagUseHTML;
  out << "<br>Lie brackets of the above elements.<br>";
  if (useHtml) {
    out << "\n<br>\n";
  }
  out << "\\(\\begin{array}{rcl}"
  << "[e, f]&=&" << this->eBracketF.toString(format) << "\\\\\n"
  << "[h, e]&=&" << this->hBracketE.toString(format) << "\\\\\n"
  << "[h, f]&=&" << this->hBracketF.toString(format)
  << "\\end{array}\\)";

  return out.str();
}

std::string SlTwoSubalgebra::toStringTripleArbitrary(FormatExpressions* format) const {
  std::stringstream out;
  out << "Starting h, e, f triple. H is computed according to Dynkin, "
  << "and the coefficients of f are arbitrarily chosen. "
  << "<br>More precisely, the chevalley generators participating in f are ordered in the order "
  << "in which their roots appear, and the coefficients are chosen to be the increasing odd "
  << "numbers 1, 3, 5, ....<br>"
  << "This arbitrary (but well-defined) choice of "
  << "f guarantees that the computation is linear and fast. <br>\n";
  out << "\\(\\begin{array}{rcl}"
  << "h&=&" << this->hPolynomialRational.toString(format) << "\\\\"
  << "e&=&" << this->eArbitraryUnknown.toString(format) << "\\\\"
  << "f&=&" << this->fArbitrary.toString(format)
  << "\\end{array}\\)";
  return out.str();
}

std::string SlTwoSubalgebra::toStringTripleArbitraryMatrix() const {
  std::stringstream out;
  out << "<br>Matrix form of the system we are trying to solve:\n";
  FormatExpressions formatLatex;
  formatLatex.flagUseLatex = true;
  formatLatex.flagUseHTML = false;
  out << "\\("
  << this->systemArbitraryMatrix.toString(&formatLatex)
  << "[col. vect.]"
  << "="
  << this->systemArbitraryColumnVector.toString(&formatLatex)
  << "\\)"
  ;
  return out.str();
}

std::string SlTwoSubalgebra::toStringTripleUnknowns(FormatExpressions* format) const {
  std::stringstream out;
  out << "<hr>Unknown elements.<br>";
  out << "\\(\\begin{array}{rcl}"
  << "h&=&" << this->hPolynomialRational.toString(format) << "\\\\\n"
  << "e&=&" << this->eUnknown.toString(format) << "\\\\\n"
  << "f&=&" << this->fUnknown.toString(format)
  << "\\end{array}\\)";
  out << "<br>Lie brackets of the unknowns.<br>";
  out << "\\([e,f] - h = "
  << this->eBracketFMinusHUnknown.toString(format)
  << "\\)";
  return out.str();
}

std::string SlTwoSubalgebra::toStringKostantSekiguchiTriple(
  FormatExpressions* format
) const {
  if (this->eKostantSekiguchi.isEqualToZero()) {
    return "";
  }
  std::stringstream out;
  out << "<br>Kostant-Sekiguchi elements.<br>";
  out << "\\(";
  out << "\\begin{array}{rcl}";
  out << "h&=&" << this->hPolynomialAlgebraic.toString(format) << "\\\\\n";
  out << "e&=&" << this->eKostantSekiguchi.toString(format) << "\\\\\n";
  out << "f&=&" << this->fKostantSekiguchi.toString(format);
  out << "\\end{array}\\)";
  return out.str();
}

std::string SlTwoSubalgebra::toStringKostantSekiguchiTripleInternals(
  FormatExpressions* format
) const {
  std::stringstream out;
  out << "<br>The unknown Kostant-Sekiguchi elements.<br>";
  std::stringstream tripleSystem;
  tripleSystem << "\\begin{array}{rcl}";
  tripleSystem << "h&=&" << this->hPolynomialAlgebraic.toString(format) << "\\\\\n";
  tripleSystem << "e&=&" << this->eKostantSekiguchiUnknown.toString(format) << "\\\\\n";
  tripleSystem << "f&=&" << this->fKostantSekiguchiUnknown.toString(format);
  tripleSystem << "\\end{array}";
  out << HtmlRoutines::getMathNoDisplay(tripleSystem.str());
  out << "<br>\\(e-f=" << this->eMinusFUnknown.toString(format) << "\\)";
  out << "<br>\\(\\theta(e-f)=" << this->involutionAppliedToEMinusF.toString(format) << "\\)";
  out << "<br>The polynomial system we need to solve.<br>";
  out << this->toStringPolynomialSystem(this->systemToSolveKostantSekiguchi, format);
  return out.str();
}

template <typename Coefficient>
std::string SlTwoSubalgebra::toStringPolynomialSystem(
 const PolynomialSubstitution<Coefficient>& system,
  FormatExpressions* format
) const {
  std::stringstream latexStreamActual;
  latexStreamActual << "\\begin{array}{rcl}";
  for (int i = 0; i < system.size; i ++) {
    latexStreamActual << system[i].toString(format) << "&=&0\\\\";
  }
  latexStreamActual << "\\end{array}";
  return HtmlRoutines::getMathNoDisplay(latexStreamActual.str()) ;
}

std::string SlTwoSubalgebra::toStringTripleUnknownsPolynomialSystem(FormatExpressions* format) const {
  std::stringstream out;
  out << "<br>The polynomial system that corresponds to finding the h, e, f triple:<br>\n";
  out << this->toStringPolynomialSystem(this->systemToSolve, format) << "\n<br>\n";
  return out.str();
}

std::string SlTwoSubalgebra::toStringTriple(FormatExpressions* format) const {
  std::stringstream out;
  out << "\\begin{array}{rcl}";
  out << "h&=&" << this->hElement.toString(format) << "\\\\\n";
  out << "e&=&" << this->eElement.toString(format) << "\\\\\n";
  out << "f&=&" << this->fElement.toString(format);
  out << "\\end{array}";
  return HtmlRoutines::getMathNoDisplay(out.str());
}

bool SlTwoSubalgebra::hasImplementedStandardCartanInvolution(
  LinearMapSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >* whichInvolution
) {
  char dynkinType = 0;
  if (!this->owner->weylGroup.dynkinType.isSimple(&dynkinType)) {
    return false;
  }
  if (dynkinType != 'A') {
    return false;
  }
  if (whichInvolution == nullptr) {
    return true;
  }
  whichInvolution->imagesChevalleyGenerators.setSize(
    this->owner->getNumberOfGenerators()
  );
  int numberOfPositiveRoots = this->owner->getNumberOfPositiveRoots();
  int rank = this->owner->getRank();
  for (int i = 0; i < numberOfPositiveRoots; i ++) {
    const Vector<Rational>& root = this->owner->weylGroup.rootsOfBorel[i];
    ChevalleyGenerator positive;
    positive.makeGeneratorRootSpace(*this->owner, root);
    ChevalleyGenerator negative;
    negative.makeGeneratorRootSpace(*this->owner, - root);
    whichInvolution->imagesChevalleyGenerators[positive.generatorIndex].makeGGenerator(- root, *this->owner);
    whichInvolution->imagesChevalleyGenerators[positive.generatorIndex] *= - 1;
    whichInvolution->imagesChevalleyGenerators[negative.generatorIndex].makeGGenerator(root, *this->owner);
    whichInvolution->imagesChevalleyGenerators[negative.generatorIndex] *= - 1;
  }
  for (int i = 0; i < rank; i ++) {
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& current =
    whichInvolution->imagesChevalleyGenerators[numberOfPositiveRoots + i];
    Vector<Rational> root;
    root.makeEi(rank, i);
    current.makeCartanGenerator(root, *this->owner);
  }
  return true;
}

bool SlTwoSubalgebra::attemptExtendingHFtoHEFWithRespectToSubalgebra(
  Vectors<Rational>& rootsWithCharacteristic2,
  Selection& zeroCharacteristics,
  Vectors<Rational>& simpleBasisSubalgebras,
  Vector<Rational>& h,
  AlgebraicClosureRationals* inputAlgebraicClosure
) {
  MacroRegisterFunctionWithName("SlTwoSubalgebra::attemptExtendingHFtoHEFWithRespectToSubalgebra");
  if (zeroCharacteristics.cardinalitySelection == zeroCharacteristics.numberOfElements) {
    return false;
  }
  this->algebraicClosure = inputAlgebraicClosure;
  this->hAlgebraic.makeCartanGenerator(h, this->getOwnerSemisimpleAlgebra());
  this->participatingPositiveRoots.size = 0;
  int relativeDimension = simpleBasisSubalgebras.size;
  // int dimension = this->theWeyl.cartanSymmetric.numberOfRows;
  if (relativeDimension != zeroCharacteristics.numberOfElements) {
    global.fatal << "Relative dimension is incorrect. " << global.fatal;
  }
  // Format. We are looking for an sl(2) for which e = a_0 g^\alpha_0+... a_kg^\alpha_k, and
  // f=b_0 g^{-\alpha_0}+... +b_kg^{-\alpha_k}
  // where the first \alpha's are ordered as in rootsInPlay.
  // Those are ordered as follows. First come the simple roots of characteristic 2,
  // and the last \alpha's are the members of SelectedExtraPositiveRoots
  // (i.e. root equal to the sum of one simple root
  // of characteristic 2 with a simple roots of characteristic 0).
  // Then the first k variables of the polynomials below will be a_0, ..., a_k., and
  // the last k variables will be the b_i's
  // the l^th polynomial will correspond to the coefficient of g^\alpha_{l/2}, where
  // l/2 is the index of the root
  // of SelectedExtraPositiveRoots, if l is even, and to the
  // coefficient of  g^{-\alpha_{(l+ 1)/2}} otherwise
  for (int i = 0; i < relativeDimension; i ++) {
    if (!zeroCharacteristics.selected[i]) {
      this->participatingPositiveRoots.addOnTop(simpleBasisSubalgebras[i]);
    }
  }
  Vectors<Rational> selectedExtraPositiveRoots;
  for (int i = 0; i < rootsWithCharacteristic2.size; i ++) {
    if (!simpleBasisSubalgebras.contains(rootsWithCharacteristic2[i])) {
      selectedExtraPositiveRoots.addOnTop(rootsWithCharacteristic2[i]);
    }
  }
  this->participatingPositiveRoots.addListOnTop(selectedExtraPositiveRoots);
  LinearMapSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > cartanInvolutionStandard;
  LinearMapSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >* cartanInvolutionToRespect = nullptr;
  if (this->hasImplementedStandardCartanInvolution(&cartanInvolutionStandard)) {
    cartanInvolutionToRespect = &cartanInvolutionStandard;
  }
  this->initializeHEFSystemFromFCoefficients(
    h,
    cartanInvolutionToRespect
  );
  Matrix<Rational> matrix, adjoinedColumn, result;
  matrix = this->systemArbitraryMatrix;
  adjoinedColumn = this->systemArbitraryColumnVector;
  this->fElement.makeZero();
  this->eElement.makeZero();
  ChevalleyGenerator generator;
  if (!Matrix<Rational>::solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(
    matrix, adjoinedColumn, result
  )) {
    return false;
  }
  this->fElement = this->fArbitrary;
  for (int i = 0; i < this->participatingPositiveRoots.size; i ++) {
    generator.makeGenerator(
      this->getOwnerSemisimpleAlgebra(),
      this->getOwnerSemisimpleAlgebra().getGeneratorIndexFromRoot(this->participatingPositiveRoots[i])
    );
    this->eElement.addMonomial(generator, result.elements[i][0]);
  }
  this->attemptRealizingKostantSekiguchi();
  return true;
}

bool SlTwoSubalgebra::attemptRealizingKostantSekiguchi() {
  MacroRegisterFunctionWithName("SlTwoSubalgebra::attemptRealizingKostantSekiguchi");
  PolynomialSystem<AlgebraicNumber> computation;
  computation.algebraicClosure = this->algebraicClosure;
  computation.initializeForSystemSolution();
  computation.groebner.polynomialOrder.monomialOrder = MonomialPolynomial::orderDefault();
  computation.solveSerreLikeSystem(this->systemToSolveKostantSekiguchi);
  if (!computation.flagSystemSolvedOverBaseField) {
    return false;
  }
  List<Polynomial<AlgebraicNumber> > solution;
  solution.setSize(computation.systemSolution.size);
  for (int i = 0; i < solution.size; i ++) {
    solution[i].makeConstant(computation.systemSolution[i]);
  }
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > eSolved, fSolved;
  eSolved = this->eKostantSekiguchiUnknown;
  fSolved = this->fKostantSekiguchiUnknown;
  eSolved.substitutionCoefficients(solution);
  fSolved.substitutionCoefficients(solution);
  this->eKostantSekiguchi = eSolved;
  this->fKostantSekiguchi = fSolved;
  return true;
}

bool SlTwoSubalgebra::checkConsistencyParticipatingRoots(
  const Vector<Rational>& targetH
) {
  MacroRegisterFunctionWithName("SlTwoSubalgebra::checkConsistencyParticipatingRoots");
  for (int i = 0; i < this->participatingPositiveRoots.size; i ++) {
    if (this->getOwnerWeyl().rootScalarCartanRoot(targetH, this->participatingPositiveRoots[i]) == 2) {
      continue;
    }
    global.fatal << "The scalar product of the h element: "
    << targetH.toString()
    << " and the root in play "
    << this->participatingPositiveRoots[i].toString()
    << " must be 2, but equals instead "
    << this->getOwnerWeyl().rootScalarCartanRoot(
      targetH, this->participatingPositiveRoots[i]
    ).toString()
    << global.fatal;
  }
  return true;
}

void SlTwoSubalgebra::initializeUnknownTriples(const Vector<Rational>& targetH) {
  if (this->algebraicClosure == nullptr) {
    global.fatal << "The algebraic closure is required." << global.fatal;
  }
  // Set the h element.
  this->hPolynomialRational.makeZero();
  this->hPolynomialRational.makeCartanGenerator(targetH, this->getOwnerSemisimpleAlgebra());
  // Set the h element but over the algebraic numbers.
  this->hPolynomialAlgebraic.makeZero();
  this->hPolynomialAlgebraic.makeCartanGenerator(targetH, this->getOwnerSemisimpleAlgebra());
  ///
  // Zero e and f for the arbitrary f choice optimization.
  this->eArbitraryUnknown.makeZero();
  // In the code below, f will be chosen arbitrarily.
  this->fArbitrary.makeZero();
  ///
  // Zero e and f without arbitrary optimizations.
  this->eUnknown.makeZero();
  this->fUnknown.makeZero();
  ///
  // Zero e  and f for the Kostant-Sekiguchi computation.
  this->eKostantSekiguchiUnknown.makeZero();
  this->fKostantSekiguchiUnknown.makeZero();

  for (int i = 0; i < this->participatingPositiveRoots.size; i ++) {
    // Initialize arbitrary triple
    ChevalleyGenerator negative;
    negative.makeGeneratorRootSpace(
      this->getOwnerSemisimpleAlgebra(), - this->participatingPositiveRoots[i]
    );
    this->fArbitrary.addMonomial(negative, i * i + 1); //(i % 2== 0)? 1: 2;
    ChevalleyGenerator positive;
    positive.makeGeneratorRootSpace( this->getOwnerSemisimpleAlgebra(), this->participatingPositiveRoots[i]);
    Polynomial<Rational> unknownCoefficient;
    unknownCoefficient.makeMonomial(i, 1, 1);
    this->eArbitraryUnknown.addMonomial(positive, unknownCoefficient);
    // Initialize true unknown triple
    unknownCoefficient.makeMonomial(i, 1, 1);
    this->eUnknown.addMonomial(positive, unknownCoefficient);
    unknownCoefficient.makeMonomial(i + this->participatingPositiveRoots.size, 1, 1);
    this->fUnknown.addMonomial(negative, unknownCoefficient);

    Polynomial<AlgebraicNumber> unknownCoefficientAlgebraic;
    unknownCoefficientAlgebraic.makeMonomial(i, 1, this->algebraicClosure->one());
    this->eKostantSekiguchiUnknown.addMonomial(positive, unknownCoefficientAlgebraic);
    unknownCoefficientAlgebraic.makeMonomial(i + this->participatingPositiveRoots.size, 1, this->algebraicClosure->one());
    this->fKostantSekiguchiUnknown.addMonomial(negative, unknownCoefficientAlgebraic);
  }
}

void SlTwoSubalgebra::computeLieBracketsUnknowns() {
  this->getOwnerSemisimpleAlgebra().lieBracket(
    this->eArbitraryUnknown, this->fArbitrary, this->eBracketFMinusHArbitraryUnknown
  );
  this->eBracketFMinusHArbitraryUnknown -= this->hPolynomialRational;
  this->getOwnerSemisimpleAlgebra().lieBracket(
    this->eUnknown, this->fUnknown, this->eBracketFMinusHUnknown
  );
  this->eBracketFMinusHUnknown -= this->hPolynomialRational;
  this->getOwnerSemisimpleAlgebra().lieBracket(
    this->eKostantSekiguchiUnknown, this->fKostantSekiguchiUnknown, this->eBracketFMinusHUnknownKostantSekiguchi
  );
  this->eBracketFMinusHUnknownKostantSekiguchi -= this->hPolynomialAlgebraic;
}

void SlTwoSubalgebra::computePolynomialSystems() {
  this->systemToSolveArbitrary.clear();
  for (int i = 0; i < this->eBracketFMinusHArbitraryUnknown.size(); i++) {
    this->systemToSolveArbitrary.addOnTop(this->eBracketFMinusHArbitraryUnknown.coefficients[i]);
  }
  this->systemToSolve.clear();
  for (int i = 0; i < this->eBracketFMinusHUnknown.size(); i ++) {
    this->systemToSolve.addOnTop(this->eBracketFMinusHUnknown.coefficients[i]);
  }
  this->systemToSolveKostantSekiguchi.clear();
  for (int i = 0; i < this->eBracketFMinusHUnknownKostantSekiguchi.size(); i ++) {
    this->systemToSolveKostantSekiguchi.addOnTop(
      this->eBracketFMinusHUnknownKostantSekiguchi.coefficients[i]
    );
  }
}

void SlTwoSubalgebra::adjoinKostantSekiguchiRelationsToPolynomialSystem(
  LinearMapSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >* cartanInvolutionPreservedByEMinusF
) {
  MacroRegisterFunctionWithName("SlTwoSubalgebra::adjoinKostantSekiguchiRelationsToPolynomialSystem");
  if (cartanInvolutionPreservedByEMinusF == nullptr) {
    return;
  }
  this->eMinusFUnknown = this->eKostantSekiguchiUnknown - this->fKostantSekiguchiUnknown;
  cartanInvolutionPreservedByEMinusF->applyTo(
    this->eMinusFUnknown, this->involutionAppliedToEMinusF
  );
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > mustBeZero;
  mustBeZero = this->eMinusFUnknown - this->involutionAppliedToEMinusF;
  for (int i = 0; i < mustBeZero.size(); i ++) {
    this->systemToSolveKostantSekiguchi.addOnTop(
      mustBeZero.coefficients[i]
    );
  }
}

void SlTwoSubalgebra::initializeHEFSystemFromFCoefficients(
  const Vector<Rational>& targetH,
  LinearMapSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >* cartanInvolutionPreservedByEMinusF
) {
  MacroRegisterFunctionWithName("SlTwoSubalgebra::initializeHEFSystemFromFCoefficients");
  this->checkConsistencyParticipatingRoots(targetH);
  this->initializeUnknownTriples(targetH);
  this->computeLieBracketsUnknowns();
  this->computePolynomialSystems();
  this->adjoinKostantSekiguchiRelationsToPolynomialSystem(
    cartanInvolutionPreservedByEMinusF
  );
  this->initializeHEFSystemFromFCoefficientsPartTwo();
}

void SlTwoSubalgebra::initializeHEFSystemFromFCoefficientsPartTwo() {
  MacroRegisterFunctionWithName("SlTwoSubalgebra::initializeHEFSystemFromECoefficientsPartTwo");
  this->systemArbitraryMatrix.initialize(this->systemToSolve.size, this->participatingPositiveRoots.size);
  this->systemArbitraryColumnVector.initialize(this->systemToSolve.size, 1);
  this->systemArbitraryMatrix.makeZero();
  this->systemArbitraryColumnVector.makeZero();
  this->systemToSolveArbitrary.getLinearSystemFromLinearPolynomials(this->systemArbitraryMatrix, this->systemArbitraryColumnVector);
}

void RootSubalgebra::getSsl2SubalgebrasAppendListNoRepetition(
  SlTwoSubalgebras& output,
  int indexRootSAinContainer,
  AlgebraicClosureRationals* algebraicClosure
) {
  MacroRegisterFunctionWithName("RootSubalgebra::getSsl2SubalgebrasAppendListNoRepetition");
  // reference: Dynkin, semisimple Lie algebras of simple lie algebras, theorems 10.1 - 10.4
  int relativeDimension = this->simpleRootsReductiveSubalgebra.size;
  if (relativeDimension == 0) {
    return;
  }
  Selection selectionRootsWithZeroCharacteristic;
  Selection simpleRootsChar2;
  Vectors<Rational> rootsScalarProduct2HnonRaised;
  Vectors<Rational> reflectedSimpleBasisK;
  rootsScalarProduct2HnonRaised.reserve(this->positiveRootsReductiveSubalgebra.size);
  ElementWeylGroup raisingElt;
  selectionRootsWithZeroCharacteristic.initialize(relativeDimension);
  Matrix<Rational> invertedRelativeKillingForm;
  invertedRelativeKillingForm.initialize(relativeDimension, relativeDimension);
  for (int k = 0; k < relativeDimension; k ++) {
    for (int j = 0; j < relativeDimension; j ++) {
      invertedRelativeKillingForm(k, j) = this->getAmbientWeyl().rootScalarCartanRoot(
        this->simpleRootsReductiveSubalgebra[k], this->simpleRootsReductiveSubalgebra[j]
      );
    }
  }
  invertedRelativeKillingForm.invert();
  int numberOfCycles = MathRoutines::twoToTheNth(selectionRootsWithZeroCharacteristic.numberOfElements);
  ProgressReport report;
  Vectors<Rational> rootsZeroChar;
  rootsZeroChar.reserve(selectionRootsWithZeroCharacteristic.numberOfElements);
  Vectors<Rational> relativeRootSystem;
  this->positiveRootsReductiveSubalgebra.getCoordinatesInBasis(this->simpleRootsReductiveSubalgebra, relativeRootSystem);
  SlTwoSubalgebra sl2;
  sl2.container = &output;
  sl2.owner = &this->getOwnerLieAlgebra();
  SemisimpleLieAlgebra& lieAlgebra = this->getOwnerLieAlgebra();
  DynkinDiagramRootSubalgebra diagramZeroCharRoots;
  for (
    int cycleCounter = 0;
    cycleCounter < numberOfCycles;
    cycleCounter ++,
    selectionRootsWithZeroCharacteristic.incrementSelection()
  ) {
    this->simpleRootsReductiveSubalgebra.subSelection(selectionRootsWithZeroCharacteristic, rootsZeroChar);
    diagramZeroCharRoots.computeDiagramTypeModifyInput(rootsZeroChar, this->getAmbientWeyl());
    int slack = 0;
    rootsScalarProduct2HnonRaised.size = 0;
    simpleRootsChar2 = selectionRootsWithZeroCharacteristic;
    simpleRootsChar2.invertSelection();
    Vector<Rational> simpleRootsChar2Vector;
    simpleRootsChar2Vector = simpleRootsChar2;
    for (int j = 0; j < relativeRootSystem.size; j ++) {
      if (simpleRootsChar2Vector.scalarEuclidean(relativeRootSystem[j]) == 1) {
        slack ++;
        rootsScalarProduct2HnonRaised.addOnTop(this->positiveRootsReductiveSubalgebra[j]);
      }
    }
    int dynkinEpsilon = diagramZeroCharRoots.numberRootsGeneratedByDiagram() + relativeDimension - slack;
    // If Dynkin's epsilon is not zero the subalgebra cannot be an S sl(2) subalgebra.
    // otherwise, as far as I understand, it always is //
    // except for G_2 (go figure!).
    // (but selectionRootsWithZeroCharacteristic still have to be found)
    // this is done in the below code.
    if (dynkinEpsilon != 0) {
      continue;
    }
    Vector<Rational> relativeCharacteristic, relativeSimpleCoords;
    relativeCharacteristic.makeZero(relativeDimension);
    for (int k = 0; k < relativeDimension; k ++) {
      if (!selectionRootsWithZeroCharacteristic.selected[k]) {
        relativeCharacteristic[k] = 2;
      }
    }
    invertedRelativeKillingForm.actOnVectorColumn(relativeCharacteristic, relativeSimpleCoords);
    Vector<Rational> characteristicH;
    characteristicH.makeZero(lieAlgebra.getRank());
    for (int j = 0; j < relativeDimension; j ++) {
      characteristicH += this->simpleRootsReductiveSubalgebra[j] * relativeSimpleCoords[j];
    }
    for (int k = 0; k < rootsScalarProduct2HnonRaised.size; k ++) {
      if (this->getAmbientWeyl().rootScalarCartanRoot(characteristicH, rootsScalarProduct2HnonRaised[k]) != 2) {
        global.fatal << "CharacteristicH is: " << characteristicH.toString()
        << "; rootsWithScalarProduct2NonRaised: "
        << rootsScalarProduct2HnonRaised.toString()
        << "; the scalar product with vector "
        << rootsScalarProduct2HnonRaised[k].toString() << " is:  "
        << this->getAmbientWeyl().rootScalarCartanRoot(characteristicH, rootsScalarProduct2HnonRaised[k]).toString()
        << " which is supposed to equal 2. " << global.fatal;
      }
    }
    this->getAmbientWeyl().raiseToDominantWeight(characteristicH, nullptr, nullptr, &raisingElt);
    reflectedSimpleBasisK = this->simpleRootsReductiveSubalgebra;
    for (int k = 0; k < reflectedSimpleBasisK.size; k ++) {
      this->getAmbientWeyl().actOn(raisingElt, reflectedSimpleBasisK[k]);
    }
    sl2.rootsWithScalar2WithH = rootsScalarProduct2HnonRaised;
    for (int k = 0; k < sl2.rootsWithScalar2WithH.size; k ++) {
      this->getAmbientWeyl().actOn(raisingElt, sl2.rootsWithScalar2WithH[k]);
    }
    for (int i = 0; i < sl2.rootsWithScalar2WithH.size; i ++) {
      if (this->getAmbientWeyl().rootScalarCartanRoot(characteristicH, sl2.rootsWithScalar2WithH[i]) != 2) {
        global.fatal << "Bad scalar product after raising: raised characteristic: "
        << characteristicH.toString()
        << " simplebasisK: " << this->simpleRootsReductiveSubalgebra.toString()
        << "raised by: " << raisingElt.toString()
        << " to get: " << reflectedSimpleBasisK.toString()
        << " theSl2.RootsWithScalar2WithH: "
        << sl2.rootsWithScalar2WithH.toString()
        << ", theSl2.RootsWithScalar2WithH[i]: "
        << sl2.rootsWithScalar2WithH[i].toString()
        << " scalar product: "
        << this->getAmbientWeyl().rootScalarCartanRoot(characteristicH, sl2.rootsWithScalar2WithH[i]).toString()
        << ". The inverted relative cartan: " << invertedRelativeKillingForm.toString()
        << ". The cartan: " << this->getAmbientWeyl().cartanSymmetric.toString() << ". "
        << global.fatal;
      }
    }
    sl2.hElement.makeCartanGenerator(characteristicH, lieAlgebra);
    sl2.lengthHSquared = sl2.getOwnerSemisimpleAlgebra().weylGroup.rootScalarCartanRoot(characteristicH, characteristicH);
    sl2.eElement.makeZero();
    sl2.fElement.makeZero();
    if (sl2.attemptExtendingHFtoHEFWithRespectToSubalgebra(
      sl2.rootsWithScalar2WithH,
      selectionRootsWithZeroCharacteristic,
      reflectedSimpleBasisK,
      characteristicH,
      algebraicClosure
    )) {
      int indexIsoSl2 = - 1;
      sl2.makeReportPrecomputations(indexRootSAinContainer, *this);
      if (output.containsSl2WithGivenHCharacteristic(sl2.hCharacteristic, &indexIsoSl2)) {
        output.getElement(indexIsoSl2).indicesContainingRootSubalgebras.addOnTop(indexRootSAinContainer);
        output.indicesSl2sContainedInRootSubalgebras[indexRootSAinContainer].addOnTop(indexIsoSl2);
      } else {
        output.indicesSl2sContainedInRootSubalgebras[indexRootSAinContainer].addOnTop(output.size);
        sl2.indexInContainer = output.size;
        output.addOnTop(sl2);
      }
    } else {
      output.badHCharacteristics.addOnTop(characteristicH);
      DynkinType tempType;
      diagramZeroCharRoots.getDynkinType(tempType);
      global.comments
      << "<br>obtained bad characteristic " << characteristicH.toString()
      << ". The zero char root diagram is "
      << tempType.toString() << "; the Dynkin epsilon is "
      << dynkinEpsilon << "= the num roots generated by diagram "
      << diagramZeroCharRoots.numberRootsGeneratedByDiagram()
      << " + the relative dimension " << relativeDimension
      << " - the slack " << slack
      << "<br>The relative root system is: " << relativeRootSystem.toString();
    }
    std::stringstream out;
    out << "Exploring Dynkin characteristics case "
    << cycleCounter + 1 << " out of " << numberOfCycles;
    report.report(out.str());
  }
}

void RootSubalgebras::computeAllReductiveRootSAsInit() {
  this->getOwnerWeyl().computeRho(true);
  this->validScales.clear();
  this->validScales.setExpectedSize(this->owner->getRank() * 2);
  for (int i = 0; i < this->owner->getRank(); i ++) {
    this->validScales.addOnTopNoRepetition(2 / this->owner->weylGroup.cartanSymmetric(i, i));
  }
  this->theWeylGroupAutomorphisms.theWeyl = &this->getOwnerWeyl();
}

void RootSubalgebras::computeParabolicPseudoParabolicNeitherOrder() {
  MacroRegisterFunctionWithName("RootSubalgebras::computeParabolicPseudoParabolicNeitherOrder");
  Selection parSel;
  parSel.initialize(this->owner->getRank());
  Vectors<Rational> basis, currentBasis;
  List<bool> Explored;
  Explored.initializeFillInObject(this->subalgebras.size, false);
  this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither.setSize(0);
  this->NumNonPseudoParabolic = 0;
  this->NumParabolic = 0;
  this->NumPseudoParabolicNonParabolic = 0;
  RootSubalgebra currentSA;
  currentSA.owner = this;
  basis.makeEiBasis(this->owner->getRank());
  List<RootSubalgebra> currentList;
  ProgressReport theReport;
  for (int i = 0; i < 2; i ++) {
    currentList.setSize(0);
    int counter = 0;
    do {
      if (theReport.tickAndWantReport()) {
        std::stringstream reportStream;
        if (i == 0) {
          reportStream << "Exploring which of the subalgebras are parabolic. ";
        } else {
          reportStream << "Exploring which of the subalgebras are pseudo-parabolic. ";
        }
        reportStream << "Current " << (i == 0 ? "pseudo-parabolic " : "parabolic ") << "selection: "
        << parSel.toString() << ", total  " << counter << " selections explored. ";
        counter ++;
        theReport.report(reportStream.str());
      }
      basis.subSelection(parSel, currentBasis);
      if (currentBasis.getRankElementSpan() != currentBasis.size) {
        continue;
      }
      currentSA.genK = currentBasis;
      currentSA.computeEssentials();
      if (currentBasis.size != 0) {
        if (currentSA.theDynkinType.toString() == "0") {
          global.fatal << "Subalgebra dynkin type computed to be zero while currentBasis is " << currentBasis.toString()
          << " and simple basis k is: " << currentSA.simpleRootsReductiveSubalgebra.toString() << global.fatal;
        }
      }
      int theIndex = this->getIndexUpToEquivalenceByDiagramsAndDimensions(currentSA);
      if (theIndex == - 1) {
        global.fatal << "Experimental code has failed an internal check on currentSA: " << currentSA.toString() << global.fatal;
      }
      if (!Explored[theIndex]) {
        currentList.addOnTop(this->subalgebras[theIndex]);
        Explored[theIndex] = true;
        if (i == 0) {
          this->NumParabolic ++;
        } else {
          this->NumPseudoParabolicNonParabolic ++;
        }
      }
    } while (parSel.incrementReturnFalseIfPastLast());
    currentList.quickSortAscending();
    this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither.addListOnTop(currentList);
    basis.addOnTop(this->owner->weylGroup.rootSystem[0]);
    parSel.initialize(this->owner->getRank() + 1);
  }
  this->NumNonPseudoParabolic = this->subalgebras.size - this->NumParabolic - this->NumPseudoParabolicNonParabolic;
  currentList.setSize(0);
  for (int i = 0; i < this->subalgebras.size; i ++) {
    if (!Explored[i]) {
      currentList.addOnTop(this->subalgebras[i]);
    }
  }
  currentList.quickSortAscending();
  this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither.addListOnTop(currentList);
}

void RootSubalgebras::computeAllReductiveRootSubalgebrasUpToIsomorphism() {
  MacroRegisterFunctionWithName("RootSubalgebras::computeAllReductiveRootSubalgebrasUpToIsomorphism");
  this->initOwnerMustBeNonZero();
  this->computeAllReductiveRootSAsInit();
  HashedList<Vector<Rational> > tempVs;
  this->flagPrintGAPinput = this->owner->weylGroup.loadGAPRootSystem(tempVs);
  ProgressReport theReport2;
  RootSubalgebra currentSA;
  currentSA.owner = this;
  currentSA.computeEssentialsIfNew();
  currentSA.computePotentialExtensions();
  this->subalgebras.reserve(this->getOwnerWeyl().rootsOfBorel.size);
  this->subalgebras.addOnTop(currentSA);
  std::string reportString;
  for (int i = 0; i < this->subalgebras.size; i ++) {
    if (theReport2.tickAndWantReport()) {
      std::stringstream reportStream;
      for (int j = 0; j < this->subalgebras[i].potentialExtensionDynkinTypes.size; j ++) {
        reportStream << this->subalgebras[i].potentialExtensionDynkinTypes[j].toString();
        if (j != this->subalgebras[i].potentialExtensionDynkinTypes.size - 1) {
          reportStream << ", ";
        }
      }
      reportString = reportStream.str();
    }
    for (int j = 0; j < this->subalgebras[i].modules.size; j ++) {
      if (this->subalgebras[i].highestWeightsPrimalSimple[j].isEqualToZero()) {
        continue;
      }
      if (theReport2.tickAndWantReport()) {
        std::stringstream out;
        out << "Exploring extensions of subalgebra " << i + 1
        << " out of " << this->subalgebras.size << ". Type current SA: "
        << this->subalgebras[i].theDynkinType.toString() << ". Possible standard parabolic extensions: "
        << reportString << ". Exploring extension by lowest weight vector of module "
        << j + 1 << " out of " << this->subalgebras[i].modules.size;
        theReport2.report(out.str());
      }
      currentSA.initNoOwnerReset();
      currentSA.simpleRootsReductiveSubalgebra = this->subalgebras[i].simpleRootsReductiveSubalgebra;
      currentSA.simpleRootsReductiveSubalgebra.addOnTop(this->subalgebras[i].LowestWeightsPrimalSimple[j]);
      currentSA.simpleBasisKinOrderOfGeneration = this->subalgebras[i].simpleBasisKinOrderOfGeneration;
      currentSA.simpleBasisKinOrderOfGeneration.addOnTop(this->subalgebras[i].LowestWeightsPrimalSimple[j]);
      currentSA.indexInducingSubalgebra = i;
      if (!currentSA.computeEssentialsIfNew()) {
        continue;
      }
      if (currentSA.simpleRootsReductiveSubalgebra.getRankElementSpan() != currentSA.simpleRootsReductiveSubalgebra.size) {
        global.fatal << "<br>simple basis vectors not linearly independent! " << global.fatal;
      }
      this->subalgebras.addOnTop(currentSA);
      this->subalgebras.lastObject()->computePotentialExtensions();
    }
  }
  std::stringstream reportStream;
  if (theReport2.tickAndWantReport()) {
    reportStream << "Reductive root subalgebra computation done: total " << this->subalgebras.size
    << " subalgebras. Proceeding to sort the subalgebras...";
    theReport2.report(reportStream.str());
  }
  this->sortDescendingOrderBySSRank();
  if (theReport2.tickAndWantReport()) {
    reportStream << "done. ";
    theReport2.report(reportStream.str());
  }
  if (this->flagComputeConeCondition) {
    if (theReport2.tickAndWantReport()) {
      reportStream << "Proceeding to compute the module pairing tables ... ";
      theReport2.report(reportStream.str());
    }
    this->computeKmodMultTables();
    if (theReport2.tickAndWantReport()) {
      reportStream << " done. ";
      theReport2.report(reportStream.str());
    }
  }
  if (this->flagPrintParabolicPseudoParabolicInfo) {
    if (theReport2.tickAndWantReport()) {
      reportStream << "Computing which subalgebras are pseudo parabolic/parabolic/neither... ";
      theReport2.report(reportStream.str());
    }
    this->computeParabolicPseudoParabolicNeitherOrder();
    if (theReport2.tickAndWantReport()) {
      reportStream << " done. ";
      theReport2.report(reportStream.str());
    }
  }
}

void RootSubalgebras::computeAllRootSubalgebrasUpToIsomorphism(int StartingIndex, int NumToBeProcessed) {
  static PauseThread localController;
  this->NumSubalgebrasProcessed = 0;
  this->numberOfConeConditionFailures = 0;
  this->NumSubalgebrasCounted = 0;
  for (int i = StartingIndex; i < NumToBeProcessed + StartingIndex; i ++) {
    this->subalgebras[i].flagComputeConeCondition = this->flagComputeConeCondition;
    this->subalgebras[i].generatePossibleNilradicals(
      localController,
      this->ImpiedSelectionsNilradical,
      this->parabolicsCounterNilradicalGeneration,
      false,
      *this,
      i
    );
    if (i != NumToBeProcessed+StartingIndex - 1) {
      this->subalgebras[i + 1].generatePossibleNilradicalsInit(
        this->ImpiedSelectionsNilradical, this->parabolicsCounterNilradicalGeneration
      );
    }
  }
}

WeylGroupData& RootSubalgebras::getOwnerWeyl() const {
  return this->GetOwnerSSalgebra().weylGroup;
}

SemisimpleLieAlgebra& RootSubalgebras::GetOwnerSSalgebra() const {
  this->checkInitialization();
  return *this->owner;
}

void RootSubalgebras::computeLProhibitingRelations() {
  static PauseThread localController;
  if (this->flagStoringNilradicals) {
    this->storedNilradicals.setSize(this->subalgebras.size);
  }
  for (;
    this->IndexCurrentSANilradicalsGeneration < this->NumReductiveRootSAsToBeProcessedNilradicalsGeneration;
    this->IndexCurrentSANilradicalsGeneration ++
  ) {
    this->subalgebras[this->IndexCurrentSANilradicalsGeneration].flagComputeConeCondition = this->flagComputeConeCondition;
    this->subalgebras[this->IndexCurrentSANilradicalsGeneration].generatePossibleNilradicals(
      localController,
      this->ImpiedSelectionsNilradical,
      this->parabolicsCounterNilradicalGeneration,
      this->flagUsingParabolicsInCentralizers,
      *this,
      this->IndexCurrentSANilradicalsGeneration
    );
    if (this->IndexCurrentSANilradicalsGeneration != this->NumReductiveRootSAsToBeProcessedNilradicalsGeneration - 1) {
      this->subalgebras[this->IndexCurrentSANilradicalsGeneration + 1].generatePossibleNilradicalsInit(
        this->ImpiedSelectionsNilradical, this->parabolicsCounterNilradicalGeneration
      );
    }
  }
}

void RootSubalgebras::sortDescendingOrderBySSRank() {
  MacroRegisterFunctionWithName("RootSubalgebras::sortDescendingOrderBySSRank");
  //Bubble sort
  RootSubalgebras output;
  List<int> SortingArray;
  List<int> inverseOfSortingArray;
  SortingArray.setSize(this->subalgebras.size);
  for (int i = 0; i < this->subalgebras.size; i ++) {
    SortingArray[i] = i;
  }
  for (int i = 0; i < this->subalgebras.size; i ++) {
    for (int j = i + 1; j < this->subalgebras.size; j ++) {
      if (this->subalgebras[SortingArray[j]].dynkinDiagram.isGreaterThan(
        this->subalgebras[SortingArray[i]].dynkinDiagram
      )) {
        SortingArray.swapTwoIndices(i, j);
      }
    }
  }
  inverseOfSortingArray.setSize(SortingArray.size);
  for (int i = 0; i <SortingArray.size; i ++) {
    inverseOfSortingArray[SortingArray[i]] = i;
  }
  output.subalgebras.reserve(this->subalgebras.size);
  for (int i = 0; i < this->subalgebras.size; i ++) {
    output.subalgebras.addOnTop(this->subalgebras[SortingArray[i]]);
    RootSubalgebra& currentSA = *output.subalgebras.lastObject();
    List<int>& otherArray = this->subalgebras[SortingArray[i]].indicesSubalgebrasContainingK;
    currentSA.indicesSubalgebrasContainingK.reserve(otherArray.size);
    currentSA.indicesSubalgebrasContainingK.setSize(0);
    for (int j = 0; j < otherArray.size; j ++) {
      currentSA.indicesSubalgebrasContainingK.addOnTop(inverseOfSortingArray[otherArray[j]]);
    }
    if (currentSA.indexInducingSubalgebra != - 1) {
      currentSA.indexInducingSubalgebra = inverseOfSortingArray[currentSA.indexInducingSubalgebra];
    }
  }
  for (int i = 0; i < this->subalgebras.size; i ++) {
    this->subalgebras[i] = output.subalgebras[i];
  }
}

void RootSubalgebras::toHTML(FormatExpressions* format) {
  MacroRegisterFunctionWithName("RootSubalgebras::toHTML");
  this->checkInitialization();
  std::string myPathVirtual = this->owner->fileNames.toStringVirtualFolderName() + this->owner->fileNames.toStringFileNameNoPathRootSubalgebras();
  std::fstream output;
  FileOperations::openFileCreateIfNotPresentVirtualCreateFoldersIfNeeded(output, myPathVirtual, false, true, false);
  if (!FileOperations::fileExistsVirtual(myPathVirtual)) {
    global.fatal << "This may or may not be a programming error. Failed to create virtual file " << myPathVirtual
    << ". Possible explanations. 1. File permissions - can I write in that folder? "
    << "2. Programming error (less likely). "
    << global.fatal;
  }
  output << "<html><title> Root subsystems of "
  << this->subalgebras[0].dynkinDiagram.toString()
  << "</title>";
  output << "<meta name = \"keywords\" content = \""
  << this->subalgebras[0].dynkinDiagram.toString()
  << " root subsystems, root subsystems, root systems";
  if (this->getOwnerWeyl().dynkinType.hasExceptionalComponent()) {
    output << ", exceptional Lie algebra";
  }
  output << " \">";
  output << SemisimpleLieAlgebra::toHTMLCalculatorHeadElements();
  output << SemisimpleLieAlgebra::toHTMLCalculatorBodyOnload()
  << this->owner->toStringHTMLMenuStructureSummary("", true, false, true, true)
  << this->owner->toHTMLCalculatorMainDiv()
  << this->toString(format)
  << "<hr>LaTeX table with root subalgebra details.<br>"
  << this->toStringDynkinTableFormatToLaTeX(format)
  << "</div>"
  << "</body></html>";
  output.close();
  for (int i = 0; i < this->subalgebras.size; i ++) {
    this->subalgebras[i].toHTML(i, format);
  }
}

std::string RootSubalgebras::toString(FormatExpressions* format) {
  return this->toStringDynkinTableHTML(format);
}

void RootSubalgebras::toStringCentralizerIsomorphisms(
  std::string& output, bool useLatex, bool useHtml, int fromIndex, int NumToProcess
) {
  std::stringstream out;
  std::string tempS;
  //W'' stands for the graph isomorphisms of C(k_ss) extending to Vector<Rational>
  //system isomorphisms of the entire algebra.
  for (int i = fromIndex; i < NumToProcess; i ++) {
    this->generateKintersectBOuterIsos(this->subalgebras[i]);
  }
  if (useLatex) {
    out << "\\begin{tabular}{ccccc}"
    << "$\\mathfrak{k}_{ss}$& $C(k_{ss})_{ss}$ & $\\#W''$ &$\\#W'''$&$\\#(W'''\\rtimes W'')$\\\\\\hline";
  }
  if (useHtml) {
    out << "<br><table><tr><td>k_{ss}</td><td></td>"
    << "<td>Weyl group of C(k_{ss})_{ss}</td><td>Outer automorphisms of C(k_{ss})_{ss}<td></tr>";
  }
  Vectors<Rational> emptyRoots;
  emptyRoots.size = 0;
  for (int i = fromIndex; i < NumToProcess; i ++) {
    RootSubalgebra& current = this->subalgebras[i];
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& theOuterIsos = this->centralizerOuterIsomorphisms[i];
    theOuterIsos.computeSubGroupFromGeneratingReflections(&emptyRoots, &theOuterIsos.ExternalAutomorphisms, 0, true);
    Rational numInnerIsos = current.theCentralizerDiagram.getSizeCorrespondingWeylGroupByFormula();
    if (useHtml) {
      out << "<td>";
    }
    tempS = current.dynkinDiagram.toString();
    out << tempS;
    if (useHtml) {
      out << "</td><td>";
    }
    if (useLatex) {
      out << " & ";
    }
    tempS = current.theCentralizerDiagram.toString();
    out << tempS;
    if (useHtml) {
      out << "</td><td>";
    }
    if (useLatex) {
      out << " & ";
    }
    out << theOuterIsos.allElements.size;
    if (useHtml) {
      out << "</td><td>";
    }
    if (useLatex) {
      out << " & ";
    }
    out << numInnerIsos.toString();
    if (useHtml) {
      out << "</td><td>";
    }
    if (useLatex) {
      out << " & ";
    }
    out << (numInnerIsos * theOuterIsos.allElements.size).toString();
    if (useHtml) {
      out << "</td></tr>";
    }
    if (useLatex) {
      out << " \\\\\n";
    }
  }
  if (useLatex) {
    out << "\\end{tabular}";
  }
  if (useHtml) {
    out << "</table><br>";
  }
  output = out.str();
}

std::string RootSubalgebras::toStringAlgebraLink(int index) {
  std::stringstream out;
  out << "<a href = \"rootSubalgebra_" << index + 1 << ".html\">"
  << this->subalgebras[index].dynkinDiagram.toString() << "</a>";
  return out.str();
}

template<class Coefficient>
Vector<Rational> ElementSemisimpleLieAlgebra<Coefficient>::getRootIMustBeWeight() const {
  if (this->isEqualToZero()) {
    global.fatal << "Calling ElementSemisimpleLieAlgebra::getRootIMustBeWeight on a zero element is forbidden." << global.fatal;
  }
  if (this->size() > 1) {
    if (!this->isElementCartan()) {
      global.fatal << "Calling ElementSemisimpleLieAlgebra::getRootIMustBeWeight "
      << "on a non-weight element is forbidden. The element is: "
      << this->toString() << global.fatal;
    }
    Vector<Rational> result;
    result.makeZero(this->getOwner()->getRank());
    return result;
  }
  return this->getOwner()->getWeightOfGenerator((*this)[0].generatorIndex);
}

std::string RootSubalgebras::toStringDynkinTableHTML(FormatExpressions* format) {
  MacroRegisterFunctionWithName("RootSubalgebras::ToStringDynkinTable");
  (void) format; //taking care of unused parameter in a portable way.
  std::stringstream out;
  std::string tooltipSAs =
  "h - fixed Cartan subalgebra. k - subalgebra containing h. "
  "k_{ss}=[k, k] - regular semisimple subalgebra in the sense of "
  "Dynkin, Semisimple Lie subalgebras of semisimple Lie algebras. "
  "k_{ss} is parametrized by a root subsytem of \\Delta(g). "
  "C(k_{ss}) consists of root spaces with roots strongly orthogonal to \\Delta(k) and a part of the Cartan h";
  int col = 0;
  int row = 0;
  out << "g: " << this->subalgebras[0].dynkinDiagram.toString()
  << ". There are " << this->subalgebras.size << " root subalgebras entries (= " << this->subalgebras.size - 2
  << " larger than the Cartan subalgebra + the Cartan subalgebra + the full subalgebra).\n\n";
  out << "<table border =\"1\">\n <colgroup>";
  for (int i = 0; i < this->NumColsPerTableLatex; i ++) {
    out << "<col width = \"180\">";
  }
  out << "</colgroup>";
  for (int i = 0; i < this->subalgebras.size; i ++) {
    row = i / this->NumColsPerTableLatex;
    col = i % this->NumColsPerTableLatex;
    if (col == 0) {
      out << "<tr>";
    }
    out << "<td style = \"vertical-align: text-top;\" title =\"" << tooltipSAs << "\">";
    out << "\n\nType k_{ss}: " << this->toStringAlgebraLink(i);
    out << "<br>";
    if (i == 0) {
      out << "<b>(Full subalgebra)</b>";
    }
    if (i == this->subalgebras.size - 1) {
      out << "<b>(Cartan subalgebra)</b>";
    }
    out << "\n<br>\nType C(k_{ss})_{ss}: " << this->subalgebras[i].theCentralizerDiagram.toString();
    if (row == this->NumLinesPerTableLatex) {
      row = 0;
    }
    out << "</td>";
    if (col == this->NumColsPerTableLatex - 1 || i == this->subalgebras.size - 1) {
      out << "</tr>";
    }
  }
  out << "</table>\n\n";
  if (this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither.size > 0) {
    out << "<hr>There are " << this->NumParabolic << " parabolic, "
    << this->NumPseudoParabolicNonParabolic << " pseudo-parabolic but not parabolic and "
    << this->NumNonPseudoParabolic << " non pseudo-parabolic root subsystems.";
    HashedList<Vector<Rational> > GAPPosRootSystem;
    if (this->flagPrintGAPinput && this->owner->weylGroup.loadGAPRootSystem(GAPPosRootSystem)) {
      out << " The roots needed to generate the root subsystems are listed below using the root indices in GAP. ";
      for (int i = 0; i < this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither.size; i ++) {
        RootSubalgebra& currentSA = this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither[i];
        out << "<br>";
        out << "[";
        if (i < this->NumParabolic) {
          out << "\"parabolic\",";
        } else if (i < this->NumParabolic + this->NumPseudoParabolicNonParabolic) {
          out << "\"pseudoParabolicNonParabolic\",";
        } else {
          out << "\"nonPseudoParabolic\",";
        }
        out << "\"" << currentSA.theDynkinType.toString() << "\", ";
        out << "[";
        for (int j = 0; j < currentSA.simpleRootsReductiveSubalgebra.size; j ++) {
          int theIndex = GAPPosRootSystem.getIndex(currentSA.simpleRootsReductiveSubalgebra[j]);
          if (theIndex == - 1) {
            theIndex = GAPPosRootSystem.getIndex(- currentSA.simpleRootsReductiveSubalgebra[j]);
          }
          out << theIndex + 1;
          if (j != currentSA.simpleRootsReductiveSubalgebra.size - 1) {
            out << ", ";
          }
        }
        out << "]]";
        if (i != this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither.size - 1) {
          out << ",";
        }
      }
    }
    out << "<hr>The roots needed to generate the root subsystems are listed below. ";
    for (int i = 0; i < this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither.size; i ++) {
      RootSubalgebra& currentSA = this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither[i];
      out << "<br>";
      out << "[";
      if (i < this->NumParabolic) {
        out << "\"parabolic\",";
      } else if (i < this->NumParabolic+this->NumPseudoParabolicNonParabolic) {
        out << "\"pseudoParabolicNonParabolic\",";
      } else {
        out << "\"nonPseudoParabolic\",";
      }
      out << "\"" << currentSA.theDynkinType.toString() << "\", ";
      out << "[";
      for (int j = 0; j < currentSA.simpleRootsReductiveSubalgebra.size; j ++) {
        out << "[";
        for (int k = 0; k < currentSA.simpleRootsReductiveSubalgebra[j].size; k ++) {
          out << currentSA.simpleRootsReductiveSubalgebra[j][k].toString();
          if (k != currentSA.simpleRootsReductiveSubalgebra[j].size - 1) {
            out << ", ";
          }
        }
        out << "]";
        if (j != currentSA.simpleRootsReductiveSubalgebra.size - 1) {
          out << ", ";
        }
      }
      out << "]]";
      if (i != this->theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither.size - 1) {
        out << ",";
      }
    }
  }
  return out.str();
}

std::string RootSubalgebras::toStringDynkinTableFormatToLaTeX(FormatExpressions* format) {
  MacroRegisterFunctionWithName("RootSubalgebras::toStringDynkinTableFormatToLaTeX");
  std::stringstream out;
  std::string endline = "\n<br>";
  if (format != nullptr) {
    format->flagSupressDynkinIndexOne = true;
  }
  out << "\\documentclass{article}" << endline
  << "\\usepackage{longtable, amssymb, lscape}" << endline
  << "\\begin{document}" << endline;
  out << "Lie algebra type: $" << this->subalgebras[0].theDynkinType.toString(format)
  << "$. There are " << this->subalgebras.size << " table entries (= " << this->subalgebras.size - 2
  << " larger than the Cartan subalgebra + the Cartan subalgebra + the full subalgebra)." << endline
  << "Let $\\mathfrak g$ stand for the type of the regular subalgebra and $C(\\mathfrak g)$ for the type of the centralizer. "
  << "Let $r$ stand for the rank of $\\mathfrak g$, let $r_c$ stand for the rank of the semisimple part of the centralizer, "
  << "let $p$ stand for the number of positive roots of $\\mathfrak g$, "
  << "let $q$ stand for the number of positive roots of the centralizer, and let $m$ stand for the number of "
  << "$A_1$ components (of all root lengths) of $\\mathfrak g$. ";
  out << "\\begin{longtable}{cccccccc}" << endline;
  out << "$\\mathfrak g$ & $C(\\mathfrak g)$& $p$ & $q$&  $m$& $r$ & $c_r$ \\\\\\endhead" << endline;
  for (int i = 0; i < this->subalgebras.size; i ++) {
    RootSubalgebra& currentSA = this->subalgebras[i];
    out << "$" << currentSA.theDynkinType.toString(format) << "$&" ;
    out << "$" << currentSA.theCentralizerDynkinType.toString(format) << "$&" ;
    out << "$" << (currentSA.theDynkinType.getRootSystemSize() / 2) << "$&" ;
    out << "$" << (currentSA.theCentralizerDynkinType.getRootSystemSize() / 2)<< "$&" ;
    out << "$" << currentSA.theDynkinType.getNumberOfSimpleComponentsOfGivenRank(1) << "$&" ;
    out << "$" << currentSA.theDynkinType.getRank() << "$&" ;
    out << "$" << currentSA.theCentralizerDynkinType.getRank() << "$&" ;
    out << "\\\\" << endline;
  }
  out << "\\end{longtable}" << endline;
  out << "\\end{document}" << endline;
  return out.str();
}

void RootSubalgebras::computeKmodMultTables() {
  for (int i = 0; i < this->subalgebras.size; i ++) {
    this->subalgebras[i].generateKModuleLieBracketTable(
      this->subalgebras[i].theMultTable, this->subalgebras[i].theOppositeKmods
    );
  }
}

bool RootSubalgebras::checkInitialization() const {
  if (this->owner == nullptr) {
    global.fatal << "Root subalgebras with non-initialized owner. " << global.fatal;
  }
  return true;
}

void RootSubalgebras::initOwnerMustBeNonZero() {
  MacroRegisterFunctionWithName("RootSubalgebras::initOwnerMustBeNonZero");
  this->checkInitialization();
  this->subalgebras.setSize(0);
  this->owner->weylGroup.computeRho(false);
}

int RootSubalgebras::getIndexUpToEquivalenceByDiagramsAndDimensions(const RootSubalgebra& theSA) {
  MacroRegisterFunctionWithName("RootSubalgebras::getIndexUpToEquivalenceByDiagramsAndDimensions");
  int result = - 1;
  for (int i = 0; i < this->subalgebras.size; i ++) {
    if (this->subalgebras[i].isEquivalentToByDiagramsAndDimensions(theSA)) {
      if (result != - 1) {
        global.fatal << "Experimental code internal check failed. " << global.fatal;
      }
      result = i;
    }
  }
  return result;
}

int RootSubalgebras::getindexSubalgebraIsomorphicTo(RootSubalgebra& input) {
  MacroRegisterFunctionWithName("RootSubalgebras::getindexSubalgebraIsomorphicTo");
  for (int i = 0; i < this->subalgebras.size; i ++) {
    if (input.simpleRootsReductiveSubalgebra == this->subalgebras[i].simpleRootsReductiveSubalgebra) {
      return i;
    }
  }
  return - 1;
}

void RootSubalgebras::computeAllReductiveRootSubalgebrasContainingInputUpToIsomorphismOLD(
  List<RootSubalgebra>& bufferSAs, int RecursionDepth
) {
  this->subalgebras.addOnTop(bufferSAs[RecursionDepth - 1]);
  int currentAlgebraIndex = this->subalgebras.size - 1;
  if (RecursionDepth >= bufferSAs.size) {
    bufferSAs.setSize(bufferSAs.size + this->getOwnerWeyl().cartanSymmetric.numberOfRows);
  }
  bufferSAs[RecursionDepth].genK = bufferSAs[RecursionDepth - 1].genK;
  bufferSAs[RecursionDepth].owner = this;
  ProgressReport theReport;
  for (int k = 0; k < bufferSAs[RecursionDepth - 1].modules.size; k ++) {
    if (bufferSAs[RecursionDepth - 1].highestWeightsPrimalSimple[k].isPositive()) {
      bufferSAs[RecursionDepth].genK.addOnTop(bufferSAs[RecursionDepth - 1].highestWeightsPrimalSimple[k]);
      bufferSAs[RecursionDepth].computeDynkinDiagramKAndCentralizer();
      std::stringstream out;
      out << "Included root " << k + 1 << " out of " << bufferSAs[RecursionDepth - 1].modules.size
      << " Total found SAs: " << this->subalgebras.size;
      theReport.report(out.str());
      int indexSA = this->indexSubalgebra(bufferSAs[RecursionDepth]);
      if (indexSA == - 1) {
        bufferSAs[RecursionDepth].computeEssentials();
        this->subalgebras[currentAlgebraIndex].indicesSubalgebrasContainingK.addOnTopNoRepetition(
          this->subalgebras.size
        );
        this->computeAllReductiveRootSubalgebrasContainingInputUpToIsomorphismOLD(bufferSAs, RecursionDepth + 1);
      } else {
        this->subalgebras[currentAlgebraIndex].indicesSubalgebrasContainingK.addOnTopNoRepetition(indexSA);
      }
      bufferSAs[RecursionDepth].genK.removeIndexSwapWithLast(bufferSAs[RecursionDepth].genK.size - 1);
    }
  }
}

void RootSubalgebras::makeProgressReportAutomorphisms(
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms &theSubgroup, RootSubalgebra& theRootSA
) {
  std::stringstream out4, out1;
  out1 << "k_ss: " << theRootSA.dynkinDiagram.toString() << " C(k_ss): "
  << theRootSA.theCentralizerDiagram.toString();
  out4 << "Num elements ";
  if (theSubgroup.truncated) {
    out4 << "truncated ";
  }
  out4 << "group preserving k: " << theSubgroup.allElements.size;
  ProgressReport theReport;
  theReport.report(out1.str() + out4.str());
}

void RootSubalgebras::generateActionKintersectBIsos(RootSubalgebra& theRootSA) {
  Selection emptySel;
  emptySel.initialize(theRootSA.SimpleBasisCentralizerRoots.size);
  this->computeNormalizerOfCentralizerIntersectNilradical(emptySel, theRootSA);
}

void RootSubalgebras::generateKintersectBOuterIsos(RootSubalgebra& theRootSA) {
  Selection fullSel;
  fullSel.initialize(theRootSA.SimpleBasisCentralizerRoots.size);
  fullSel.incrementSelectionFixedCardinality(theRootSA.SimpleBasisCentralizerRoots.size);
  this->computeNormalizerOfCentralizerIntersectNilradical(fullSel, theRootSA);
}

void RootSubalgebras::computeActionNormalizerOfCentralizerIntersectNilradical(
  Selection& SelectedBasisRoots, RootSubalgebra& theRootSA
) {
  this->computeNormalizerOfCentralizerIntersectNilradical(SelectedBasisRoots, theRootSA);
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& theSubgroup = this->CentralizerIsomorphisms.lastObject();
  this->ActionsNormalizerCentralizerNilradical.setSize(theSubgroup.allElements.size - 1);
  Vector<Rational> tempRoot;
  ProgressReport theReport;
  for (int i = 0; i < theSubgroup.allElements.size - 1; i ++) {
    this->ActionsNormalizerCentralizerNilradical[i].setSize(theRootSA.modules.size);
    for (int j = 0; j < theRootSA.modules.size; j ++) {
      tempRoot = theRootSA.highestWeightsPrimalSimple[j];
      theSubgroup.actByNonSimpleElement(i + 1, tempRoot);
      int tempI = theRootSA.getIndexKModuleContainingRoot(tempRoot);
      this->ActionsNormalizerCentralizerNilradical[i][j] = tempI;
    }
    if (global.response.monitoringAllowed()) {
      std::stringstream out;
      out << "Computing action of element " << i + 1 << " out of " << theSubgroup.allElements.size;
      theReport.report(out.str());
    }
  }
}

void RootSubalgebras::computeNormalizerOfCentralizerIntersectNilradical(
  Selection& SelectedBasisRoots, RootSubalgebra& theRootSA
) {
  Vectors<Rational> selectedRootsBasisCentralizer;
  selectedRootsBasisCentralizer.size = 0;
  for (int i = 0; i <SelectedBasisRoots.numberOfElements; i ++) {
    if (!SelectedBasisRoots.selected[i]) {
      selectedRootsBasisCentralizer.addOnTop(theRootSA.SimpleBasisCentralizerRoots[i]);
    }
  }
  this->CentralizerIsomorphisms.reserve(this->subalgebras.size);
  this->centralizerOuterIsomorphisms.reserve(this->subalgebras.size);

  this->CentralizerIsomorphisms.setSize(this->CentralizerIsomorphisms.size + 1);
  this->centralizerOuterIsomorphisms.setSize(this->CentralizerIsomorphisms.size);
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& outputSubgroup = this->CentralizerIsomorphisms.lastObject();
  outputSubgroup.ambientWeyl = &theRootSA.getAmbientWeyl();
  this->makeProgressReportAutomorphisms(outputSubgroup, theRootSA);
  theRootSA.generateIsomorphismsPreservingBorel(theRootSA, &outputSubgroup);
  outputSubgroup.computeSubGroupFromGeneratingReflections(
    &selectedRootsBasisCentralizer, &outputSubgroup.ExternalAutomorphisms, this->UpperLimitNumElementsWeyl, false
  );
  outputSubgroup.simpleRootsInner = selectedRootsBasisCentralizer;
  this->centralizerOuterIsomorphisms.lastObject().ExternalAutomorphisms = outputSubgroup.ExternalAutomorphisms;
  this->centralizerOuterIsomorphisms.lastObject().ambientWeyl = &this->getOwnerWeyl();
  this->makeProgressReportAutomorphisms(outputSubgroup, theRootSA);
}

RootSubalgebras::RootSubalgebras() {
  this->flagNilradicalComputationInitialized = false;
  this->flagStoringNilradicals = false;
  this->flagUsingParabolicsInCentralizers = true;
  this->flagCountingNilradicalsOnlyNoComputation = false;
  this->flagComputingLprohibitingWeights = false;
  this->flagComputeConeCondition = false;
  this->flagUsingActionsNormalizerCentralizerNilradical = true;
  this->flagLookingForMinimalRels = false;
  this->NumLinesPerTableLatex = 20;
  this->NumColsPerTableLatex = 4;
  this->UpperLimitNumElementsWeyl = 0;
  this->owner = nullptr;
  this->flagPrintGAPinput = false;
  this->flagPrintParabolicPseudoParabolicInfo = false;
  this->initForNilradicalGeneration();
  this->NumNonPseudoParabolic = 0;
  this->NumParabolic = 0;
  this->NumPseudoParabolicNonParabolic = 0;
}

void RootSubalgebras::initForNilradicalGeneration() {
  this->NumSubalgebrasProcessed = 0;
  this->numberOfConeConditionFailures = 0;
  this->NumSubalgebrasCounted = 0;
  this->IndexCurrentSANilradicalsGeneration = 0;
  this->ReportStringNonNilradicalParabolic = "";
  this->NumReductiveRootSAsToBeProcessedNilradicalsGeneration = this->subalgebras.size - 1;
  if (this->subalgebras.size > 0) {
    this->subalgebras[0].generatePossibleNilradicalsInit(
      this->ImpiedSelectionsNilradical, this->parabolicsCounterNilradicalGeneration
    );
    this->NumconeConditionHoldsBySSpart.initializeFillInObject(this->subalgebras.size, 0);
  }
}

bool RootSubalgebras::approveKModuleSelectionWRTActionsNormalizerCentralizerNilradical(Selection& targetSel) {
  if (!this->flagUsingActionsNormalizerCentralizerNilradical) {
    return true;
  }
  for (int i = 0; i < this->ActionsNormalizerCentralizerNilradical.size; i ++) {
    if (!this->approveSelAgainstOneGenerator(this->ActionsNormalizerCentralizerNilradical[i], targetSel)) {
      return false;
    }
  }
  return true;
}

void RootSubalgebras::raiseSelectionUntilApproval(Selection& targetSel) {
  bool raised = true;
  while (raised) {
    raised = false;
    for (int i = 0; i < this->ActionsNormalizerCentralizerNilradical.size; i ++) {
      if (!this->approveSelAgainstOneGenerator(this->ActionsNormalizerCentralizerNilradical[i], targetSel)) {
        this->applyOneGenerator(this->ActionsNormalizerCentralizerNilradical[i], targetSel);
        raised = true;
      }
    }
  }
}

void RootSubalgebras::toStringConeConditionNotSatisfying(std::string& output, bool includeMatrixForm) {
  Vectors<Rational> tempRoots, tempRoots2;
  std::stringstream out;
  std::stringstream out2;
  std::string tempS;
  int numNonSolvableNonReductive = 0;
  char simpleType;
  int theRank;
  if (!this->getOwnerWeyl().dynkinType.isSimple(&simpleType, &theRank)) {
    global.fatal << "toStringConeConditionNotSatisfying "
    << "called on a non-simple Lie algebra. " << global.fatal;
  }
  if (simpleType == 'B') {
    out << "$\\mathrm{so}(2n + 1)$ is realized as a matrix Lie algebra as "
    << "$\\left\\{\\left(\\begin{array}{c|c|c}A&\\begin{array}{c}v_1\\\\ \\vdots \\\\ "
    << "v_n\\end{array} &C= -C^T \\\\\\hline \\begin{array}{ccc}w_1 &\\dots&  w_n\\end{array} &0& "
    << "\\begin{array}{ccc}-v_n &\\dots&  -v_1\\end{array} "
    << "\\\\\\hline D= -D^T&\\begin{array}{c}-w_n\\\\ \\vdots \\\\ -w_1\\end{array} "
    << "& -A^T\\end{array}\\right)\\right\\}$.\n\n";
  }
  if (simpleType == 'C') {
    out << "$\\mathrm{sp}(2n)$ is realized as a matrix Lie algebra "
    << "as $\\left\\{\\left(\\begin{array}{c|c}A& C \\\\\\hline D& -A^T\\end{array}"
    << "\\right)| C=C^T, D= D^T\\right\\}.$";
  }
  out << " In this realization, the Cartan subalgebra $\\mathfrak{h}$ can "
  << "be chosen to consist of the diagonal matrices of the above form.\n\n";
  if (!includeMatrixForm) {
    out << "\n\\begin{longtable}{r|l}\n\\multicolumn{2}{c}{";
    if (simpleType == 'B') {
      out << " $ \\mathfrak{g}\\simeq \\mathrm{so(" << this->getOwnerWeyl().cartanSymmetric.numberOfRows * 2 + 1 << ")}$";
    }
    if (simpleType == 'C') {
      out << " $\\mathfrak{g}\\simeq \\mathrm{sp(" << this->getOwnerWeyl().cartanSymmetric.numberOfRows * 2 << ")}$";
    }
    out << "} \\\\\\hline";
  }
  for (int i = 0; i < this->subalgebras.size - 1; i ++) {
    if (this->storedNilradicals[i].size > 0) {
      RootSubalgebra& currentRootSA = this->subalgebras[i];
      tempRoots.size = 0;
      for (int j = 0; j < currentRootSA.positiveRootsReductiveSubalgebra.size; j ++) {
        tempRoots.addOnTop(currentRootSA.positiveRootsReductiveSubalgebra[j]);
        tempRoots.addOnTop(- currentRootSA.positiveRootsReductiveSubalgebra[j]);
      }
      if (includeMatrixForm) {
        out << "\n\n\\noindent\\rule{\\textwidth}{1.5pt}\n\n";
      } else {
        out << "\\hline\\begin{tabular}{r}";
      }
      out << "$\\Delta(\\mathfrak{k})$ is of type "
      << currentRootSA.dynkinDiagram.toString() << "; ";
      if (!includeMatrixForm) {
        out << "\\\\";
      }
      currentRootSA.getAmbientWeyl().getEpsilonCoordinates(currentRootSA.positiveRootsReductiveSubalgebra, tempRoots2);
      tempS = tempRoots2.toStringEpsilonForm(true, false, false);
      out << " $\\Delta^+(\\mathfrak{k})=$ " << tempS;
      if (includeMatrixForm) {
        out << "\n\n\\noindent\\rule{\\textwidth}{0.3pt}\n\n";
      } else {
        out << "\\end{tabular} &\n\\begin{tabular}{l}";
      }
      int numNonReductiveCurrent = 0;
      for (int j = 0; j < this->storedNilradicals[i].size; j ++) {
        List<int>& currentNilrad = this->storedNilradicals[i][j];
        if (currentNilrad.size > 0) {
          numNonSolvableNonReductive ++;
          numNonReductiveCurrent ++;
          tempRoots.size = currentRootSA.positiveRootsReductiveSubalgebra.size * 2;
          for (int k = 0; k < currentNilrad.size; k ++) {
            tempRoots.addListOnTop(currentRootSA.WeightsModulesPrimalSimple[currentNilrad[k]]);
          }
          this->toStringRootSpaces(tempS, includeMatrixForm, tempRoots);
          out << tempS << "\n";
          if (numNonReductiveCurrent % 2 == 0) {
            out << "\n\n";
            if (!includeMatrixForm) {
              out << "\\\\";
            }
          }
        }
      }
      if (!includeMatrixForm) {
        out << "\\end{tabular}\n\n\\\\";
      }
    }
  }
  if (!includeMatrixForm) {
    out << "\n\\end{longtable}";
  }
  out2 << "\n\nThe number of non-conjugate non-solvable non-reductive "
  << "root subalgebras not satisfying the cone condition is: "
  << numNonSolvableNonReductive << "\n\n";
  tempS = out.str();
  out2 << tempS;
  output = out2.str();
}

void RootSubalgebras::toStringRootSpaces(std::string& output, bool includeMatrixForm, Vectors<Rational>& input) {
  std::string tempS; std::stringstream out;
  Vectors<Rational> epsCoords;
  Matrix<int> tempMat;
  char simpleType;
  int dimension = - 1;
  if (!this->getOwnerWeyl().dynkinType.isSimple(&simpleType, &dimension)) {
    global.fatal << "toStringConeConditionNotSatisfying "
    << "called on a non-simple Lie algebra. " << global.fatal;
  }
  if (simpleType == 'B') {
    this->getOwnerWeyl().getEpsilonCoordinates(input, epsCoords);
    tempMat.makeIdentityMatrix(dimension * 2 + 1, 1, 0);
    tempMat.elements[dimension][dimension] = 0;
    for (int i = 0; i < epsCoords.size; i ++) {
      bool isShort = false;
      int firstIndex = - 1;
      int secondIndex = - 1;
      bool firstSignIsPositive = true;
      bool secondSignIsPositive = true;
      Vector<Rational>& currentRoot = epsCoords[i];
      for (int j = 0; j < dimension; j ++) {
        if (currentRoot[j] != 0) {
          isShort = !isShort;
          if (isShort) {
            if (currentRoot[j].isPositive()) {
              firstSignIsPositive = true;
            } else {
              firstSignIsPositive = false;
            }
            firstIndex = j;
          } else {
            if (currentRoot[j].isPositive()) {
              secondSignIsPositive = true;
            } else {
              secondSignIsPositive = false;
            }
            secondIndex = j;
          }
        }
      }
      if (!isShort) {
        bool signsAreDifferent =(firstSignIsPositive !=secondSignIsPositive);
        if (signsAreDifferent) {
          int positiveIndex, negativeIndex;
          if (firstSignIsPositive) {
            positiveIndex = firstIndex;
            negativeIndex = secondIndex;
          } else {
            positiveIndex = secondIndex;
            negativeIndex = firstIndex;
          }
          tempMat.elements[positiveIndex][negativeIndex] = 1;
          tempMat.elements[dimension + 1 + negativeIndex][dimension + 1 + positiveIndex] = - 1;
        } else {
          if (firstSignIsPositive) {
            tempMat.elements[firstIndex][secondIndex + dimension + 1] = 1;
            tempMat.elements[secondIndex][firstIndex + dimension + 1] = - 1;
          } else {
            tempMat.elements[dimension + 1 + firstIndex][secondIndex] = 1;
            tempMat.elements[dimension + 1 + secondIndex][firstIndex] = - 1;
          }
        }
      } else {
        if (firstSignIsPositive) {
          tempMat.elements[firstIndex][dimension] = 1;
          tempMat.elements[dimension][dimension + 1 + firstIndex] = - 1;
        } else {
          tempMat.elements[dimension][firstIndex] = 1;
          tempMat.elements[firstIndex + 1 + dimension][dimension] = - 1;
        }
      }
    }
  }
  if (simpleType == 'C') {
    this->getOwnerWeyl().getEpsilonCoordinates(input, epsCoords);
    tempMat.makeIdentityMatrix(dimension * 2, 1, 0);
    for (int i = 0; i < epsCoords.size; i ++) {
      bool isLong= false;
      int firstIndex = - 1;
      int secondIndex = - 1;
      bool firstSignIsPositive = true;
      bool secondSignIsPositive = true;
      Vector<Rational>& currentRoot = epsCoords[i];
      for (int j = 0; j < dimension; j ++) {
        if (currentRoot[j] != 0) {
          isLong = !isLong;
          if (isLong) {
            if (currentRoot[j].isPositive()) {
              firstSignIsPositive = true;
            } else {
              firstSignIsPositive = false;
            }
            firstIndex = j;
          } else {
            if (currentRoot[j].isPositive()) {
              secondSignIsPositive = true;
            } else {
              secondSignIsPositive = false;
            }
            secondIndex = j;
          }
        }
      }
      if (!isLong) {
        bool signsAreDifferent = (firstSignIsPositive !=secondSignIsPositive);
        if (signsAreDifferent) {
          int positiveIndex = - 1, negativeIndex = - 1;
          if (firstSignIsPositive) {
            positiveIndex = firstIndex;
            negativeIndex = secondIndex;
          } else {
            positiveIndex = secondIndex;
            negativeIndex = firstIndex;
          }
          tempMat.elements[positiveIndex][negativeIndex] = 1;
          tempMat.elements[dimension + negativeIndex][dimension + positiveIndex] = - 1;
        } else {
          if (firstSignIsPositive) {
            tempMat.elements[firstIndex][secondIndex + dimension] = 1;
            tempMat.elements[secondIndex][firstIndex + dimension] = 1;
          } else {
            tempMat.elements[dimension + firstIndex][secondIndex] = 1;
            tempMat.elements[dimension + secondIndex][firstIndex] = 1;
          }
        }
      } else {
        if (firstSignIsPositive) {
          tempMat.elements[firstIndex][dimension + firstIndex] = 1;
        } else {
          tempMat.elements[dimension + firstIndex][firstIndex] = 1;
        }
      }
    }
  }
  if (includeMatrixForm) {
    out << "\\begin{tabular}{cc} \\begin{tabular}{l}";
  }
  out << "$\\Delta(\\mathfrak{n})=$";
  if (includeMatrixForm) {
    out << "\\\\";
  }
  int numNilradicalRootSpaces = 0;
  for (int i = 0; i < epsCoords.size; i ++) {
    Vector<Rational>& currentRoot = epsCoords[i];
    tempS = currentRoot.toStringEpsilonFormat();
    if (!epsCoords.contains(- currentRoot)) {
      out << tempS << ", ";
      numNilradicalRootSpaces++;
    }
    if (includeMatrixForm) {
      if (numNilradicalRootSpaces % 2 == 0 && numNilradicalRootSpaces != 0) {
        out << "\\\\";
      }
    }
  }
  if (includeMatrixForm) {
    out << "\\end{tabular} & $\\mathfrak{l}=\\left(\\begin{array}{";
    for (int i = 0; i < tempMat.numberOfColumns; i ++) {
      out << "c";
      if (simpleType == 'B' && (i == dimension - 1 || i == dimension)) {
        out << "|";
      }
    }
    out << "}";
    for (int i = 0; i < tempMat.numberOfRows; i ++) {
      if (simpleType == 'B' && (i == dimension || i == dimension + 1)) {
        out << "\\hline";
      }
      for (int j = 0; j < tempMat.numberOfColumns; j ++) {
        if (tempMat.elements[i][j] != 0 && tempMat.elements[j][i] == 0) {
          out << "\\blacktriangle";
        }
        if (tempMat.elements[i][j] != 0 && tempMat.elements[j][i] != 0) {
          out << "\\bullet";
        }
        if (j != tempMat.numberOfColumns - 1) {
          out << "&";
        } else {
          out << "\\\\";
        }
      }
    }
    out << "\\end{array}\\right)$ \\end{tabular}  ";
  }
  output = out.str();
}

void RootSubalgebras::applyOneGenerator(List<int>& generator, Selection& targetSel) {
  Selection tempSel;
  tempSel.initialize(targetSel.numberOfElements);
  for (int i = 0; i < targetSel.cardinalitySelection; i ++) {
    tempSel.addSelectionAppendNewIndex(generator[targetSel.elements[i]]);
  }
  targetSel = tempSel;
}

bool RootSubalgebras::approveSelAgainstOneGenerator(List<int>& generator, Selection& targetSel) {
  Selection tempSel;
  tempSel.initialize(targetSel.numberOfElements);
  for (int i = 0; i < targetSel.cardinalitySelection; i ++) {
    tempSel.addSelectionAppendNewIndex(generator[targetSel.elements[i]]);
  }
  for (int i = 0; i < tempSel.numberOfElements; i ++) {
    if (targetSel.selected[i] && !tempSel.selected[i]) {
      return true;
    }
    if (!targetSel.selected[i] && tempSel.selected[i]) {
      return false;
    }
  }
  return true;
}

int RootSubalgebras::indexSubalgebra(RootSubalgebra& input) {
  for (int j = 0; j < this->subalgebras.size; j ++) {
    RootSubalgebra& right = this->subalgebras[j];
    if (
      input.dynkinDiagram.toString() == right.dynkinDiagram.toString() &&
      input.theCentralizerDiagram.toString() == right.theCentralizerDiagram.toString()
    ) {
      if (!this->getOwnerWeyl().isOfSimpleType('E', 7)) {
        return j;
      } else {
        input.computeEssentials();
        if (input.generateIsomorphismsPreservingBorel(right, nullptr)) {
          return j;
        }
      }
    }
  }
  return - 1;
}

bool RootSubalgebras::isNewSubalgebra(RootSubalgebra& input) {
  return this->indexSubalgebra(input) == - 1;
}

void RootSubalgebra::computeRootsOfK() {
  MacroRegisterFunctionWithName("RootSubalgebra::computeRootsOfK");
  this->allRootsSubalgebra.clear();
  HashedList<Vector<Rational> >& ambientRootSystem= this->getAmbientWeyl().rootSystem;
  this->allRootsSubalgebra.setExpectedSize(ambientRootSystem.size);
  Vector<Rational> currentRoot;
  this->allRootsSubalgebra.addListOnTop(this->simpleRootsReductiveSubalgebra);
  for (int i = 0; i < this->allRootsSubalgebra.size; i ++) {
    for (int j = 0; j < this->simpleRootsReductiveSubalgebra.size; j ++) {
      currentRoot = this->allRootsSubalgebra[i] + this->simpleRootsReductiveSubalgebra[j];
      if (ambientRootSystem.contains(currentRoot)) {
        this->allRootsSubalgebra.addOnTopNoRepetition(currentRoot);
      }
    }
  }
  this->positiveRootsReductiveSubalgebra = this->allRootsSubalgebra;
  for (int i = 0; i < this->positiveRootsReductiveSubalgebra.size; i ++) {
    this->allRootsSubalgebra.addOnTop(- this->positiveRootsReductiveSubalgebra[i]);
  }
  if (this->simpleRootsReductiveSubalgebra.size == 0) {
    if (this->allRootsSubalgebra.size != 0) {
      global.fatal << "Internal check went bad. " << global.fatal;
    }
  }
}

void ConeRelation::RelationOneSideToStringCoordForm(
  std::string& output, List<Rational>& coeffs, Vectors<Rational>& theRoots, bool EpsilonForm
) {
  std::stringstream out;
  std::string tempS;
  for (int i = 0; i < theRoots.size; i ++) {
    tempS = coeffs[i].toString();
    if (tempS == "1") {
      tempS = "";
    }
    if (tempS == "- 1" || tempS == "-1") {
      tempS = "-";
    }
    if ((tempS == "0")) {
      global.fatal << "zero not allowed at this point of code. " << global.fatal;
    }
    out << tempS;
    if (!EpsilonForm) {
      tempS = theRoots[i].toString();
    } else {
      tempS = theRoots[i].toStringEpsilonFormat();
    }
    out << "(" << tempS << ")";
    if (i != theRoots.size - 1) {
      out << " + ";
    }
  }
  output = out.str();
}

void ConeRelation::relationOneSideToString(
  std::string& output,
  const std::string& letterType,
  List<Rational>& coeffs,
  List<List<int> >& kComponents,
  Vectors<Rational>& theRoots,
  bool useLatex,
  RootSubalgebra& owner
) {
  if (theRoots.size != coeffs.size) {
    global.fatal << "The number of coefficients must equal the number of roots. " << global.fatal;
  }
  std::stringstream out;
  std::string tempS;
  if (useLatex) {
    out << "\\begin{tabular}{";
    for (int i = 0; i < theRoots.size; i ++) {
      out << "c";
    }
    out << "}";
  }
  for (int i = 0; i < theRoots.size; i ++) {
    tempS = coeffs[i].toString();
    if (tempS == "1") {
      tempS = "";
    }
    if (tempS == "- 1" || tempS == "-1") {
      tempS = "-";
    }
    if ((tempS == "0")) {
      global.fatal << "zero not allowed here. " << global.fatal;
    }
    out << tempS;
    if (!useLatex) {
      tempS = theRoots[i].toString();
      out << "(" << tempS << ")";
      if (i != theRoots.size - 1) {
        out << " + ";
      }
    } else {
      out << "$" << letterType << "_" << i + 1 << "$";
      if (i != theRoots.size - 1) {
        out << "+ & ";
      }
    }
  }
  if (useLatex) {
    out << "\\\\";
  }
  List<int> TakenIndices;
  List<int> NumPrimesUniTypicComponent;
  TakenIndices.initializeFillInObject(owner.dynkinDiagram.simpleBasesConnectedComponents.size, - 1);
  NumPrimesUniTypicComponent.initializeFillInObject(owner.dynkinDiagram.sameTypeComponents.size, - 1);
  for (int i = 0; i < kComponents.size; i ++) {
    if (useLatex) {
      out << "\\tiny{ ";
    }
    for (int j = 0; j < kComponents[i].size; j ++) {
      int index = kComponents[i][j];
      int indexUniComponent = owner.dynkinDiagram.indexUniComponent[index];
      out << owner.dynkinDiagram.simpleComponentTypes[index].toString();
      if (TakenIndices[index] == - 1) {
        NumPrimesUniTypicComponent[indexUniComponent] ++;
        TakenIndices[index] = NumPrimesUniTypicComponent[indexUniComponent];
      }
      for (int k = 0; k < TakenIndices[index]; k ++) {
        out << "'";
      }
      if (j != kComponents[i].size - 1) {
        out << "+";
      }
    }
    if (useLatex) {
      out << " }";
      if (i != kComponents.size - 1) {
        out << " & ";
      }
    }
  }
  if (useLatex) {
    out << "\\end{tabular}";
  }
  output = out.str();
}

int ConeRelation::toString(
  std::string& output, RootSubalgebras& owners, bool useLatex, bool includeScalarsProductsEachSide, bool includeMixedScalarProducts
) {
  std::string tempS;
  std::stringstream out;
  if (this->AlphaCoeffs.size != this->Alphas.size || this->BetaCoeffs.size != this->Betas.size) {
    global.fatal << "Number of coefficients is wrong. " << global.fatal;
  }
  int LatexLineCounter = 0;
  this->computeConnectedComponents(
    this->Alphas, owners.subalgebras[this->IndexOwnerRootSubalgebra],  this->AlphaKComponents
  );
  this->computeConnectedComponents(
    this->Betas, owners.subalgebras[this->IndexOwnerRootSubalgebra], this->BetaKComponents
  );
  this->relationOneSideToString(
    tempS,
    "\\alpha",
    this->AlphaCoeffs,
    this->AlphaKComponents,
    this->Alphas,
    useLatex,
    owners.subalgebras[this->IndexOwnerRootSubalgebra]
  );
  out << tempS;
  if (useLatex) {
    out << " &\\begin{tabular}{c} ";
  }
  out << "=";
  if (useLatex) {
    out << " \\\\~ \\end{tabular} & ";
  }
  this->relationOneSideToString(
    tempS,
    "\\beta",
    this->BetaCoeffs,
    this->BetaKComponents,
    this->Betas,
    useLatex,
    owners.subalgebras[this->IndexOwnerRootSubalgebra]
  );
  out << tempS;
  if (useLatex) {
    out << " & ";
  }
  this->diagram.toString();
  out << tempS;
  this->theDiagramRelAndK.toString();
  if (useLatex) {
    out << " & ";
  }
  out << tempS;
  if (includeScalarsProductsEachSide) {
    out << " & ";
    LatexLineCounter += this->rootsToScalarProductString(
      this->Alphas, this->Alphas, "\\alpha", "\\alpha", tempS, owners.subalgebras[this->IndexOwnerRootSubalgebra]
    );
    out << tempS;
    LatexLineCounter += this->rootsToScalarProductString(
      this->Betas, this->Betas, "\\beta", "\\beta", tempS, owners.subalgebras[this->IndexOwnerRootSubalgebra]
    );
    out << " " << tempS;
  }
  if (includeMixedScalarProducts) {
    LatexLineCounter += this->rootsToScalarProductString(
      this->Alphas, this->Betas, "\\alpha", "\\beta", tempS, owners.subalgebras[this->IndexOwnerRootSubalgebra]
    );
    out << tempS;
  }
  out << "\n";
  output = out.str();
  return MathRoutines::maximum(2, LatexLineCounter);
}

int ConeRelation::rootsToScalarProductString(
  Vectors<Rational>& inputLeft,
  Vectors<Rational>& inputRight,
  const std::string& letterTypeLeft,
  const std::string& letterTypeRight,
  std::string& output,
  RootSubalgebra& owner
) {
  std::string tempS; std::stringstream out;
  int numLinesLatex = 0;
  Rational tempRat;
  for (int i = 0; i < inputLeft.size; i ++) {
    for (int j = 0; j < inputRight.size; j ++) {
      if (i < j || letterTypeLeft != letterTypeRight) {
        owner.getAmbientWeyl().rootScalarCartanRoot(inputLeft[i], inputRight[j], tempRat);
        if (!tempRat.isEqualToZero()) {
          tempS = tempRat.toString();
          out << "$\\langle" << letterTypeLeft << "_" << i + 1
          << ", " << letterTypeRight << "_" << j + 1 << "\\rangle =" << tempS << "$, ";
          numLinesLatex ++;
        }
      }
    }
  }
  output = out.str();
  return numLinesLatex;
}

void ConeRelation::computeConnectedComponents(Vectors<Rational>& input, RootSubalgebra& owner, List<List<int> >& output) {
  output.setSize(input.size);
  for (int i = 0; i < input.size; i ++) {
    output[i].size = 0;
    for (int j = 0; j < owner.dynkinDiagram.simpleBasesConnectedComponents.size; j ++) {
      if (owner.dynkinDiagram.simpleBasesConnectedComponents[j].containsVectorNonPerpendicularTo(
          input[i], owner.getAmbientWeyl().cartanSymmetric
      )) {
        output[i].addOnTop(j);
      }
    }
  }
}

bool ConeRelation::IsStrictlyWeaklyProhibiting(
  RootSubalgebra& owner, Vectors<Rational>& NilradicalRoots, RootSubalgebras& owners, int indexInOwner
) {
  Vectors<Rational> tempRoots;
  tempRoots = this->Alphas;
  tempRoots.addListOnTop(this->Betas);
  tempRoots.addListOnTop(owner.genK);
  //owner.ambientWeyl.transformToSimpleBasisGenerators(tempRoots);
  this->diagram.computeDiagramTypeModifyInput(tempRoots, owner.getAmbientWeyl());
  if (this->diagram.toString() == "F^{1}_4") {
    return false;
  }
  if (this->diagram.simpleComponentTypes[0].letter == 'A' && this->diagram.simpleComponentTypes[0].rank == 1) {
   //  global.fatal << global.fatal;
  }
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms tempSubgroup;
  tempSubgroup.ambientWeyl = &(owner.getAmbientWeyl());
  tempSubgroup.computeSubGroupFromGeneratingReflections(&tempRoots, &tempSubgroup.ExternalAutomorphisms, 0, true);
  Vectors<Rational> NilradicalIntersection, genSingHW;
  tempRoots = tempSubgroup.RootSubsystem;
  NilradicalRoots.intersectWith(tempRoots, NilradicalIntersection);
  for (int i = 0; i < owner.highestWeightsPrimalSimple.size; i ++) {
    if (
      !owner.nilradicalKmodules.selected[i] &&
      tempRoots.contains(owner.highestWeightsPrimalSimple[i]) &&
      owner.isGeneratingSingularVectors(i, NilradicalIntersection)
    ) {
      genSingHW.addOnTop(owner.highestWeightsPrimalSimple[i]);
    }
  }
  if (owner.coneConditionHolds(owners, indexInOwner, NilradicalIntersection, genSingHW, false)) {
    return false;
  }
  if (!owner.attemptTheTripleTrickWRTSubalgebra(*this, genSingHW, NilradicalIntersection)) {
    return false;
  }
  this->sortRelation(owner);
  this->fixRepeatingRoots(this->Alphas, this->AlphaCoeffs);
  this->computeDiagramRelationsAndK(owner);
  return true;
}

void ConeRelation::computeDiagramAndDiagramRelationsAndK(RootSubalgebra& owner) {
  Vectors<Rational> tempRoots;
  tempRoots = this->Alphas;
  tempRoots.addListOnTop(this->Betas);
  this->diagram.computeDiagramTypeModifyInput(tempRoots, owner.getAmbientWeyl());
  this->computeDiagramRelationsAndK(owner);
}

void ConeRelation::makeLookCivilized(RootSubalgebra& owner) {
  if (this->Alphas.size == 0 || this->Betas.size == 0) {
    return;
  }
  Vectors<Rational> tempRoots;
  tempRoots = this->Alphas;
  tempRoots.addListOnTop(this->Betas);
  this->diagram.computeDiagramTypeModifyInput(tempRoots, owner.getAmbientWeyl());
  if (
    this->diagram.simpleComponentTypes[0].letter == 'A' &&
    this->diagram.simpleComponentTypes[0].rank == 1
  ) {
    this->computeDiagramRelationsAndK(owner);
    global.fatal << "Failed to compute diagram relation and k. " << global.fatal;
  }
  this->sortRelation(owner);
  this->fixRepeatingRoots(this->Alphas, this->AlphaCoeffs);
  this->computeDiagramRelationsAndK(owner);
}

void ConeRelation::FixRightHandSide(RootSubalgebra& owner, Vectors<Rational>& NilradicalRoots) {
  bool hasChanged = true;
  Vector<Rational> tempRoot;
  while (hasChanged) {
    hasChanged = false;
    for (int i = 0; i < this->Betas.size; i ++) {
      for (int j = i + 1; j < this->Betas.size; j ++) {
        tempRoot = this->Betas[i];
        tempRoot += this->Betas[j];
        if (owner.getAmbientWeyl().rootSystem.contains(tempRoot)) {
          int leavingIndex = j; int remainingIndex = i;
          if (this->BetaCoeffs[j].isGreaterThan(this->BetaCoeffs[i])) {
            leavingIndex = i;
            remainingIndex = j;
          }
          this->Betas[leavingIndex] = tempRoot;
          this->BetaCoeffs[remainingIndex].subtract(this->BetaCoeffs[leavingIndex]);
          if (this->BetaCoeffs[remainingIndex].isEqualToZero()) {
            this->BetaCoeffs.removeIndexSwapWithLast(remainingIndex);
            this->Betas.removeIndexSwapWithLast(remainingIndex);
          }
          if (!NilradicalRoots.contains(tempRoot)) {
            global.fatal << "Nilradical doesn't have required root. " << global.fatal;
          }
          hasChanged = true;
        }
      }
    }
  }
}

bool ConeRelation::checkForBugs(RootSubalgebra& owner, Vectors<Rational>& NilradicalRoots) {
  for (int i = 0; i < this->Alphas.size; i ++) {
    int tempI = owner.highestWeightsPrimalSimple.getIndex(this->Alphas[i]);
    if (tempI == - 1) {
      return false;
    }
    if (NilradicalRoots.contains(this->Alphas[i])) {
      return false;
    }
  }
  for (int i = 0; i < this->Betas.size; i ++) {
    if (!NilradicalRoots.contains(this->Betas[i])) {
      return false;
    }
  }
  return true;
}

void ConeRelation::getSumAlphas(Vector<Rational>& output, int dimension) {
  if (this->AlphaCoeffs.size != this->Alphas.size) {
    global.fatal << "Wrong number of alpha coefficients" << global.fatal;
  }
  output.makeZero(dimension);
  Vector<Rational> tempRoot;
  for (int i = 0; i < this->Alphas.size; i ++) {
    tempRoot = this->Alphas[i];
    tempRoot *= this->AlphaCoeffs[i];
    output += tempRoot;
  }
}

void ConeRelation::sortRelation(RootSubalgebra& owner) {
  this->ComputeKComponents(this->Alphas, this->AlphaKComponents, owner);
  this->ComputeKComponents(this->Betas, this->BetaKComponents, owner);
  //bubble sort
  for (int i = 0; i < this->Alphas.size; i ++) {
    for (int j = i + 1; j < this->Alphas.size; j ++) {
      if (this->leftSortedBiggerThanOrEqualToRight(
        this->AlphaKComponents[j], this->AlphaKComponents[i]
      )) {
        this->AlphaCoeffs.swapTwoIndices(i, j);
        this->Alphas.swapTwoIndices(i, j);
        this->AlphaKComponents.swapTwoIndices(i, j);
      }
    }
  }
  for (int i = 0; i < this->Betas.size; i ++) {
    for (int j = i + 1; j < this->Betas.size; j ++) {
      if (this->leftSortedBiggerThanOrEqualToRight(this->BetaKComponents[j], this->BetaKComponents[i])) {
        this->BetaCoeffs.swapTwoIndices(i, j);
        this->Betas.swapTwoIndices(i, j);
        this->BetaKComponents.swapTwoIndices(i, j);
      }
    }
  }
}

void ConeRelation::ComputeKComponents(Vectors<Rational>& input, List<List<int> >& output, RootSubalgebra& owner) {
  output.setSize(input.size);
  for (int i = 0; i < input.size; i ++) {
    output[i].size = 0;
    for (int j = 0; j < owner.dynkinDiagram.simpleBasesConnectedComponents.size; j ++) {
      if (owner.dynkinDiagram.simpleBasesConnectedComponents[j].containsVectorNonPerpendicularTo(
        input[i], owner.getAmbientWeyl().cartanSymmetric
      )) {
        output[i].addOnTop(j);
      }
    }
  }
}

void ConeRelation::computeDiagramRelationsAndK(RootSubalgebra& owner) {
  Vectors<Rational> tempRoots;
  tempRoots.size = 0;
  tempRoots.reserve(owner.getAmbientWeyl().cartanSymmetric.numberOfRows * 2);
  tempRoots.addListOnTop(owner.simpleRootsReductiveSubalgebra);
  for (int i = 0; i < this->diagram.simpleBasesConnectedComponents.size; i ++) {
    tempRoots.addListOnTop(this->diagram.simpleBasesConnectedComponents[i]);
  }
  this->theDiagramRelAndK.computeDiagramTypeModifyInput(tempRoots, owner.getAmbientWeyl());
}

void ConeRelation::fixRepeatingRoots(Vectors<Rational>& theRoots, List<Rational>& coeffs) {
  for (int i = 0; i < theRoots.size; i ++) {
    if (coeffs[i].isEqualToZero()) {
      coeffs.removeIndexSwapWithLast(i);
      theRoots.removeIndexSwapWithLast(i);
      i --;
    }
    for (int j = i + 1; j < theRoots.size; j ++) {
      if (theRoots[i] == theRoots[j]) {
        coeffs[i] += coeffs[j];
        theRoots.removeIndexSwapWithLast(j);
        coeffs.removeIndexSwapWithLast(j);
        j --;
      }
    }
  }
}

bool ConeRelation::leftSortedBiggerThanOrEqualToRight(List<int>& left, List<int>& right) {
  if (left.size > right.size) {
    return true;
  }
  if (right.size > left.size) {
    return false;
  }
  for (int i = 0; i < right.size; i ++) {
    if (right[i] > left[i]) {
      return false;
    }
    if (left[i] > right[i]) {
      return true;
    }
  }
  return true;
}

bool ConeRelation::generateAutomorphisms(ConeRelation& right) {
  return this->relationString == right.relationString;
}

void ConeRelations::addRelationNoRepetition(ConeRelation& input, RootSubalgebras& owners) {
  input.computeRelationString(owners, true, true);
  int i = static_cast<signed>(this->getHash(input));
  List<int>& theIndices = this->hashBuckets[i];
  for (int j = 0; j < theIndices.size; j ++) {
    if (this->objects[theIndices[j]].generateAutomorphisms(input)) {
      return;
    }
  }
  if (!this->flagIncludeSmallerRelations) {
    if (input.theDiagramRelAndK.toString() != owners.subalgebras[0].dynkinDiagram.toString()) {
      return;
    }
  }
  this->addOnTop(input);
}

void ConeRelations::getLatexHeaderAndFooter(std::string& outputHeader, std::string& outputFooter) {
  outputHeader.clear();
  outputFooter.clear();
  outputHeader.append("\\begin{tabular}{rcl p{1cm}p{1cm}p{3cm} } \\multicolumn{3}{c}");
  outputHeader.append("{ Relation / linked $\\mathfrak{k}$-components}");
  outputHeader.append(" &$\\alpha_i$'s, $\\beta_i$'s generate & adding $\\mathfrak{k}$ generates&");
  outputHeader.append("Non-zero scalar products\\\\");
  outputFooter.append("\\end{tabular}");
}

void ConeRelations::toString(std::string& output, RootSubalgebras& owners, bool useLatex) {
  std::stringstream out;
  std::string tempS, header, footer;
  Vectors<Rational> tempAlphas, tempBetas;
  this->getLatexHeaderAndFooter(header, footer);
  if (useLatex) {
    out << header;
  }
  int oldIndex = - 1;
  int lineCounter = 0;
  for (int i = 0; i < this->size; i ++) {
    if (oldIndex != this->objects[i].IndexOwnerRootSubalgebra) {
      oldIndex = this->objects[i].IndexOwnerRootSubalgebra;
      if (useLatex) {
        out << "\\hline\\multicolumn{5}{c}{$\\mathfrak{k}$-semisimple type: "
        << owners.subalgebras[oldIndex].dynkinDiagram.toString()
        << "}\\\\\n\\hline\\hline";
      }
    }
    lineCounter += this->objects[i].toString(tempS, owners, useLatex, true, true);
    out << tempS;
    if (useLatex) {
      out << "\\\\";
    }
    out << "\n";
    if (this->flagIncludeCoordinateRepresentation) {
      lineCounter += 2;
      out << "\\multicolumn{5}{c}{$\\varepsilon$-form~relative~to~"
      << "the~subalgebra~generated~by~$\\mathfrak{k}$~and~the~relation}\\\\\n";
      this->objects[i].RelationOneSideToStringCoordForm(tempS, this->objects[i].AlphaCoeffs, tempAlphas, true);
      out << "\\multicolumn{5}{c}{" << tempS;
      this->objects[i].RelationOneSideToStringCoordForm(tempS, this->objects[i].BetaCoeffs, tempBetas, true);
      out << "=" << tempS; //<<"~~~~";
      out << "}\\\\\\hline\n";
    }
    if (lineCounter>this->NumAllowedLatexLines) {
      out << footer << "\n\n\n" << header;
      lineCounter = 0;
    }
  }
  if (useLatex) {
    out << footer;
  }
  if (this->flagIncludeSubalgebraDataInDebugString) {
    FormatExpressions tempFormat;
    out << "\n\n\\newpage" << owners.toString(&tempFormat);
  }
  output = out.str();
}
