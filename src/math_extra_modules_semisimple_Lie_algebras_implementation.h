// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader_ImplementationHeaderModulesSSLielgebras
#define vpfHeader_ImplementationHeaderModulesSSLielgebras
#include "math_extra_modules_semisimple_Lie_algebras.h"
#include "math_extra_drawing_variables.h"
#include "math_extra_differential_operators.h"

template<class Coefficient>
Rational ModuleSSalgebra<Coefficient>::hwTrace(
  const Pair<MonomialTensor<int, HashFunctions::hashFunction>,
  MonomialTensor<int, HashFunctions::hashFunction> >& thePair,
  ProgressReport* theProgressReport
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<Coefficient>::hwTrace");
  int indexInCache = this->cachedPairs.getIndex(thePair);
  if (indexInCache != - 1) {
    return this->cachedTraces[indexInCache];
  }
  if (thePair.object1.generatorsIndices.size == 0) {
    return 1;
  }
  Pair<MonomialTensor<int, HashFunctions::hashFunction>,
  MonomialTensor<int, HashFunctions::hashFunction> > newPair;
  MonomialTensor<int, HashFunctions::hashFunction>& newLeft = newPair.object1;
  MonomialTensor<int, HashFunctions::hashFunction>& newRight = newPair.object2;
  const MonomialTensor<int, HashFunctions::hashFunction>& oldRight = thePair.object2;
  newLeft = thePair.object1;
  (*newLeft.powers.lastObject()) -= 1;
  int theIndex = *newLeft.generatorsIndices.lastObject();
  if (*newLeft.powers.lastObject() == 0) {
    newLeft.generatorsIndices.size --;
    newLeft.powers.size --;
  }
  int theIndexMinus = 2 * this->getOwner().getNumberOfPositiveRoots() + this->getOwner().getRank() - theIndex - 1;
  int theSimpleIndex = theIndex - this->getOwner().getNumberOfPositiveRoots() - this->getOwner().getRank();
  MonomialTensor<int, HashFunctions::hashFunction> Accum;
  Accum.powers.reserve(oldRight.powers.size);
  Accum.generatorsIndices.reserve(oldRight.generatorsIndices.size);
  Vector<Rational> RemainingWeight;
  Rational result = 0;
  Rational summand;
  WeylGroupData& theWeyl = this->getOwner().weylGroup;
  for (int i = 0; i < oldRight.generatorsIndices.size; i ++) {
    if (oldRight.generatorsIndices[i] == theIndexMinus) {
      summand = 0;
      newRight = Accum;
      newRight.multiplyByGeneratorPowerOnTheRight(oldRight.generatorsIndices[i], oldRight.powers[i] - 1);
      RemainingWeight.makeZero(theWeyl.getDimension());
      for (int j = i + 1; j < oldRight.generatorsIndices.size; j ++) {
        newRight.multiplyByGeneratorPowerOnTheRight(oldRight.generatorsIndices[j], oldRight.powers[j]);
        RemainingWeight += theWeyl.rootSystem[oldRight.generatorsIndices[j]] * oldRight.powers[j];
      }
      RemainingWeight += this->theHWFDpartSimpleCoordS;
      summand += theWeyl.getScalarProductSimpleRoot(RemainingWeight, theSimpleIndex);
      summand *= 2;
      summand /= theWeyl.cartanSymmetric.elements[theSimpleIndex][theSimpleIndex];
      summand += 1;
      summand -= oldRight.powers[i];
      if (!summand.isEqualToZero()) {
        summand *= this->hwTrace(
          newPair, theProgressReport
        );
      }
      summand *= oldRight.powers[i];
      result += summand;
    }
    Accum.generatorsIndices.addOnTop(oldRight.generatorsIndices[i]);
    Accum.powers.addOnTop(oldRight.powers[i]);
  }
  if (this->cachedPairs.size < this->MaxNumCachedPairs) {
    this->cachedPairs.addOnTop(thePair);
    this->cachedTraces.addOnTop(result);
  }
  if (theProgressReport != nullptr && this->cachedPairs.size < 500000) {
    std::stringstream tempStream;
    tempStream << "Number of cached pairs: " << this->cachedPairs.size
    << " at recursion depth " << global.customStackTrace.size;
    theProgressReport->report(tempStream.str());
  }
  return result;
}

template<class Coefficient>
void ModuleSSalgebra<Coefficient>::applyTAA(MonomialTensor<int, HashFunctions::hashFunction>& theMon) {
  for (int i = 0; i < theMon.generatorsIndices.size; i ++) {
    theMon.generatorsIndices[i] =
    this->getOwner().getNumberOfPositiveRoots() * 2 + this->getOwner().getRank() - theMon.generatorsIndices[i] - 1;
  }
  theMon.powers.reverseElements();
  theMon.generatorsIndices.reverseElements();
}

template<class Coefficient>
Rational ModuleSSalgebra<Coefficient>::hwtaabfSimpleGensOnly(
  const MonomialTensor<int, HashFunctions::hashFunction>& leftMon,
  const MonomialTensor<int, HashFunctions::hashFunction>& rightMon,
  ProgressReport* theProgressReport
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<Coefficient>::hwtaabfSimpleGensOnly");
  const MonomialTensor<int, HashFunctions::hashFunction>* left = &leftMon;
  const MonomialTensor<int, HashFunctions::hashFunction>* right = &rightMon;
  if (leftMon > rightMon) {
    MathRoutines::swap(left, right);
  }
  Pair<MonomialTensor<int, HashFunctions::hashFunction>, MonomialTensor<int, HashFunctions::hashFunction> > thePair;
  thePair.object1 = *left;
  thePair.object2 = *right;
  this->applyTAA(thePair.object1);
  Rational result = this->hwTrace(thePair, theProgressReport);
  if (theProgressReport != nullptr) {
    std::stringstream tempStream;
    tempStream << this->cachedPairs.size << " total cached pairs";
    theProgressReport->report(tempStream.str());
  }
  return result;
}

template<class Coefficient>
void ModuleSSalgebra<Coefficient>::substitution(const PolynomialSubstitution<Rational>& theSub) {
  for (int i = 0; i < this->actionsGeneratorsMaT.size; i ++) {
    this->actionsGeneratorsMaT[i].substitution(theSub);
  }
  for (int i = 0; i < this->actionsGeneratorS.size; i ++) {
    for (int j = 0; j < this->actionsGeneratorS[i].size; j ++) {
      for (int k = 0; k < this->actionsGeneratorS[i][j].size; k ++) {
        this->actionsGeneratorS[i][j][k].substitution(theSub);
      }
    }
  }
  List<MonomialUniversalEnveloping<Coefficient> > oldGeneratingWordsNonReduced;
  oldGeneratingWordsNonReduced = this->theGeneratingWordsNonReduced;
  this->theGeneratingWordsNonReduced.clear();
  for (int i = 0; i < oldGeneratingWordsNonReduced.size; i ++) {
    oldGeneratingWordsNonReduced[i].substitution(theSub);
    this->theGeneratingWordsNonReduced.addOnTop(oldGeneratingWordsNonReduced[i]);
  }
  for (int i = 0; i < this->theGeneratingWordsGrouppedByWeight.size; i ++) {
    for (int j = 0; j < this->theGeneratingWordsGrouppedByWeight[i].size; j ++) {
      this->theGeneratingWordsGrouppedByWeight[i][j].substitution(theSub);
    }
  }
  for (int i = 0; i < this->theBilinearFormsAtEachWeightLevel.size; i ++) {
    this->theBilinearFormsAtEachWeightLevel[i].substitution(theSub);
    this->theBilinearFormsInverted[i].substitution(theSub);
  }
  for (int i = 0; i < this->theHWDualCoordsBaseFielD.size; i ++) {
    this->theHWDualCoordsBaseFielD[i].substitution(theSub, Rational::one(), nullptr);
    this->theHWFundamentalCoordsBaseField[i].substitution(theSub, Rational::one(), nullptr);
    this->theHWSimpleCoordSBaseField[i].substitution(theSub, Rational::one(), nullptr);
  }
}

template <class Coefficient>
MatrixTensor<Coefficient>& ModuleSSalgebra<Coefficient>::getActionGeneratorIndex(int generatorIndex) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<Coefficient>::getActionGeneratorIndex");
  int numGenerators = this->getOwner().getNumberOfGenerators();
  if (generatorIndex < 0 || generatorIndex >= numGenerators) {
    global.fatal << "Bad generator index: " << generatorIndex << ". " << global.fatal;
  }
  if (this->ComputedGeneratorActions.selected[generatorIndex]) {
    return this->actionsGeneratorsMaT[generatorIndex];
  }
  this->ComputedGeneratorActions.addSelectionAppendNewIndex(generatorIndex);
  if (this->hasFreeAction(generatorIndex)) {
    global.fatal << "Due to a change in "
    << "implementation of the generalized Verma module class. " << global.fatal;
    this->actionsGeneratorsMaT[generatorIndex].makeZero();
    return this->actionsGeneratorsMaT[generatorIndex];
  }
  if (this->hasZeroActionFDpart(generatorIndex)) {
    this->actionsGeneratorsMaT[generatorIndex].makeZero();
    return this->actionsGeneratorsMaT[generatorIndex];
  }
  if (this->getOwner().isSimpleGenerator(generatorIndex)) {
    return this->getActionSimpleGeneratorIndex(generatorIndex);
  }
  MatrixTensor<Coefficient>& output = this->actionsGeneratorsMaT[generatorIndex];
  if (this->getOwner().isGeneratorFromCartan(generatorIndex)) {
    output.makeZero();
    MonomialMatrix theMon;
    Vector<Coefficient> weightH;
    Coefficient tempCF, hwCFshift;
    weightH.makeEi(this->getOwner().getRank(), generatorIndex - this->getOwner().getNumberOfPositiveRoots());
    hwCFshift = this->getOwner().weylGroup.rootScalarCartanRoot(weightH, this->theHWSimpleCoordSBaseField);
    hwCFshift -= this->getOwner().weylGroup.rootScalarCartanRoot(weightH, this->theHWFDpartSimpleCoordS);
    for (int i = 0; i < this->theGeneratingWordsNonReduced.size; i ++) {
      Vector<Rational>& theWeight = this->theGeneratingWordsWeightsPlusWeightFDpart[i];
      tempCF = this->getOwner().weylGroup.rootScalarCartanRoot(weightH, theWeight);
      tempCF += hwCFshift;
      theMon.isIdentity = false;
      theMon.vIndex = i;
      theMon.dualIndex = i;
      output.addMonomial(theMon, tempCF);
    }
    return output;
  }
  List<int> adActions;
  Rational theCoeff;
  this->getOwner().getChevalleyGeneratorAsLieBracketsSimpleGenerators(generatorIndex, adActions, theCoeff);
  MatrixTensor<Coefficient> tempO;
  output = this->getActionGeneratorIndex(*adActions.lastObject());
  for (int i = adActions.size - 2; i >= 0; i --) {
    tempO = this->getActionGeneratorIndex(adActions[i]);
    output.lieBracketOnTheLeft(tempO);
  }
  Coefficient tempCF;
  tempCF = theCoeff;
  output *= tempCF;
  return output;
}

template  <class Coefficient>
void ModuleSSalgebra<Coefficient>::getMatrixHomogenousElt(
  ElementUniversalEnveloping<Coefficient>& inputHomogeneous,
  List<List<ElementUniversalEnveloping<Coefficient> > >& outputSortedByArgumentWeight,
  Vector<Rational>& weightUEEltSimpleCoords,
  MatrixTensor<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra::getMatrixHomogenousElt");
  this->getAdActionHomogenousElt(
    inputHomogeneous, weightUEEltSimpleCoords, outputSortedByArgumentWeight, ringUnit, ringZero
  );
  output.makeZero();
  for (int j = 0; j < outputSortedByArgumentWeight.size; j ++) {
    for (int k = 0; k < outputSortedByArgumentWeight[j].size; k ++) {
      MonomialUniversalEnveloping<Coefficient>& currentMon = this->theGeneratingWordsGrouppedByWeight[j][k];
      ElementUniversalEnveloping<Coefficient>& imageCurrentMon = outputSortedByArgumentWeight[j][k];
      int indexColumn = this->theGeneratingWordsNonReduced.getIndex(currentMon);
      if (indexColumn == - 1) {
        global.fatal << "Column index equals -1. " << global.fatal;
      }
      for (int l = 0; l < imageCurrentMon.size; l ++) {
        int indexRow = this->theGeneratingWordsNonReduced.getIndex(imageCurrentMon[l]);
        if (indexRow == - 1) {
          global.fatal << "Negative row index not allowed. " << global.fatal;
        }
        output.addMonomial(MonomialMatrix(indexRow, indexColumn), imageCurrentMon.theCoeffs[l]);
      }
    }
  }
  if (!this->flagIsInitialized) {
    return;
  }
}

template <class Coefficient>
bool CharacterSemisimpleLieAlgebraModule<Coefficient>::splitOverLeviMonomialsEncodeHighestWeight(
  std::string* report,
  CharacterSemisimpleLieAlgebraModule& output,
  const Selection& splittingParSel,
  const Selection& ParSelFDInducingPart,
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& outputWeylSub
) {
  MacroRegisterFunctionWithName("CharacterSemisimpleLieAlgebraModule<Coefficient>::splitOverLeviMonomialsEncodeHighestWeight");
  this->checkNonZeroOwner();
  std::stringstream out;
  std::string tempS;
  if (this->getOwner()->getRank() != splittingParSel.numberOfElements) {
    global.fatal << "Parabolic selection selects out of "
    << splittingParSel.numberOfElements
    << " elements while the weyl group is of rank "
    << this->getOwner()->getRank() << ". " << global.fatal;
  }
  outputWeylSub.makeParabolicFromSelectionSimpleRoots(this->getOwner()->weylGroup, splittingParSel, 1);
  outputWeylSub.computeRootSubsystem();

  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms complementGroup, theFDWeyl;
  complementGroup.ambientWeyl = outputWeylSub.ambientWeyl;
  theFDWeyl.ambientWeyl = outputWeylSub.ambientWeyl;

  Selection invertedSel;
  invertedSel = splittingParSel;
  invertedSel.invertSelection();
  complementGroup.makeParabolicFromSelectionSimpleRoots(this->getOwner()->weylGroup, invertedSel, 1);
  complementGroup.computeRootSubsystem();
  out << outputWeylSub.toString(false);
  CharacterSemisimpleLieAlgebraModule charAmbientFDWeyl, remainingCharDominantLevi;
  theFDWeyl.makeParabolicFromSelectionSimpleRoots(this->getOwner()->weylGroup, ParSelFDInducingPart, 1);
  Weight<Coefficient> tempMon, localHighest;
  List<Coefficient> tempMults;
  HashedList<Vector<Coefficient> > tempHashedRoots;
  WeylGroupData& theWeyL = this->getOwner()->weylGroup;
  charAmbientFDWeyl.makeZero();
  Coefficient bufferCoeff, highestCoeff;
  out << "Starting character: " << this->toString();
  tempMon.owner = this->getOwner();
  for (int i = 0; i < this->size(); i ++) {
    const Weight<Coefficient>& currentMon = (*this)[i];
    if (!theFDWeyl.freudenthalFormulaIrrepIsWRTLeviPart(
      currentMon.weightFundamentalCoordinates, tempHashedRoots, tempMults, tempS, 10000
    )) {
      if (report != nullptr) {
        *report = tempS;
      }
      return false;
    }
    for (int j = 0; j < tempHashedRoots.size; j ++) {
      bufferCoeff = this->coefficients[i];
      tempMon.weightFundamentalCoordinates = theWeyL.getFundamentalCoordinatesFromSimple(tempHashedRoots[j]);
      bufferCoeff *= tempMults[j];
      charAmbientFDWeyl.addMonomial(tempMon, bufferCoeff);
    }
  }

  remainingCharDominantLevi.makeZero();
  Vectors<Coefficient> orbitDom;
  tempMon.owner = this->getOwner();
  for (int i = 0; i < charAmbientFDWeyl.size(); i ++) {
    orbitDom.setSize(0);
    if (!theFDWeyl.generateOrbitReturnFalseIfTruncated(
      theWeyL.getSimpleCoordinatesFromFundamental(charAmbientFDWeyl[i].weightFundamentalCoordinates), orbitDom, false, 10000
    )) {
      out << "failed to generate the complement-sub-Weyl-orbit of weight "
      << this->getOwner()->weylGroup.getSimpleCoordinatesFromFundamental(charAmbientFDWeyl[i].weightFundamentalCoordinates).toString();
      if (report != nullptr) {
        *report = out.str();
      }
      return false;
    }
    for (int k = 0; k < orbitDom.size; k ++) {
      if (outputWeylSub.isDominantWeight(orbitDom[k])) {
        tempMon.weightFundamentalCoordinates = theWeyL.getFundamentalCoordinatesFromSimple(orbitDom[k]);
        remainingCharDominantLevi.addMonomial(tempMon, charAmbientFDWeyl.coefficients[i]);
      }
    }
  }
  output.makeZero();
  out << "<br>Character with respect to Levi part: "
  << HtmlRoutines::getMathNoDisplay(remainingCharDominantLevi.toString());
  Vector<Coefficient> simpleGeneratorBaseField;
  while (!remainingCharDominantLevi.isEqualToZero()) {
    localHighest = *remainingCharDominantLevi.monomials.lastObject();
    for (bool found = true; found; ) {
      found = false;
      for (int i = 0; i < outputWeylSub.simpleRootsInner.size; i ++) {
        tempMon = localHighest;
        simpleGeneratorBaseField = outputWeylSub.simpleRootsInner[i]; // <- implicit type conversion here!
        tempMon.weightFundamentalCoordinates += this->getOwner()->weylGroup.getFundamentalCoordinatesFromSimple(simpleGeneratorBaseField);
        if (remainingCharDominantLevi.monomials.contains(tempMon)) {
          localHighest = tempMon;
          found = true;
        }
      }
    }
    highestCoeff = remainingCharDominantLevi.coefficients[remainingCharDominantLevi.monomials.getIndex(localHighest)];
    output.addMonomial(localHighest, highestCoeff);
    if (!outputWeylSub.freudenthalFormulaIrrepIsWRTLeviPart(
      localHighest.weightFundamentalCoordinates, tempHashedRoots, tempMults, tempS, 10000
    )) {
      if (report != nullptr) {
        *report = tempS;
      }
      return false;
    }
    for (int i = 0; i < tempHashedRoots.size; i ++) {
      tempMon.owner = this->getOwner();
      tempMon.weightFundamentalCoordinates = theWeyL.getFundamentalCoordinatesFromSimple(tempHashedRoots[i]);
      bufferCoeff = tempMults[i];
      bufferCoeff *= highestCoeff;
      remainingCharDominantLevi.subtractMonomial(tempMon, bufferCoeff);
    }
  }
  out << "<br>Character with respect to Levi part: " << HtmlRoutines::getMathNoDisplay(output.toString());
  if (report != nullptr) {
    DrawingVariables drawingVariables;
    std::string tempS;
    this->drawMeNoMultiplicities(tempS, drawingVariables, 10000);
    Vector<Rational> tempRoot;
    out << "<hr>In the following weight visualization, "
    << "a yellow line is drawn if the corresponding weights are "
    << "simple reflections of one another, "
    << "with respect to a simple root of the Levi part of the parabolic subalgebra. ";
    for (int i = 0; i < output.size(); i ++) {
      tempRoot = theWeyL.getSimpleCoordinatesFromFundamental(output[i].weightFundamentalCoordinates).getVectorRational();
      outputWeylSub.drawContour(tempRoot, drawingVariables, "#a0a000", 1000);
      std::stringstream tempStream;
      tempStream << output.coefficients[i].toString();
      drawingVariables.drawTextAtVectorBufferRational(tempRoot, tempStream.str(), "black");
    }
    out << "<hr>" << drawingVariables.getHTMLDiv(theWeyL.getDimension(), false);
    *report = out.str();
  }
  return true;
}

template<class Coefficient>
void ModuleSSalgebra<Coefficient>::splitOverLevi(
  std::string* report,
  Selection& splittingParSel,
  List<ElementUniversalEnveloping<Coefficient> >* outputEigenVectors,
  Vectors<Coefficient>* outputWeightsFundCoords,
  Vectors<Coefficient>* outputEigenSpace,
  CharacterSemisimpleLieAlgebraModule<Coefficient>* outputChar
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<Coefficient>::splitOverLevi");
  this->checkInitialization();
  if (this->theChaR.size() != 1) {
    if (report != nullptr) {
      std::stringstream out;
      out << "I have been instructed only to split modules that are irreducible over the ambient Lie algebra";
      out << " Instead I got the character " << this->theChaR.toString() << " (" << this->theChaR.size() << " monomials)";
      *report = out.str();
    }
    return;
  }
  if (this->getOwner().getRank() != splittingParSel.numberOfElements) {
    global.fatal << "Semisimple rank is "
    << this->getOwner().getRank() << " but splitting parabolic selects "
    << " out of " << splittingParSel.numberOfElements << " simple roots. " << global.fatal;
  }
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms subWeyl;
  subWeyl.ambientWeyl = &this->owner->weylGroup;
  MemorySaving<CharacterSemisimpleLieAlgebraModule<Coefficient> > buffer;
  CharacterSemisimpleLieAlgebraModule<Coefficient>& charWRTsubalgebra = (outputChar == 0) ? buffer.getElement() : *outputChar;
  this->theChaR.splitOverLeviMonomialsEncodeHighestWeight(
    report, charWRTsubalgebra, splittingParSel, this->parabolicSelectionNonSelectedAreElementsLevi, subWeyl
  );
  std::stringstream out;
  if (report != nullptr) {
    out << *report;
  }
  Selection splittingParSelectedInLevi;
  splittingParSelectedInLevi = splittingParSel;
  splittingParSelectedInLevi.invertSelection();
  if (!splittingParSelectedInLevi.isSubset(this->parabolicSelectionSelectedAreElementsLevi)) {
    out << "The parabolic subalgebra you selected is not a subalgebra of the ambient parabolic subalgebra. "
    << "The parabolic has root of Levi given by " << splittingParSel.toString()
    << " while the ambient parabolic subalgebra has root of Levi given by "
    << this->parabolicSelectionNonSelectedAreElementsLevi.toString();
    if (report != nullptr) {
      *report = out.str();
    }
    return;
  }
  out << "<br>Parabolic selection: " << splittingParSel.toString();
  List<List<Vector<Coefficient> > > eigenSpacesPerSimpleGenerator;
  //if (false)
  eigenSpacesPerSimpleGenerator.setSize(splittingParSelectedInLevi.cardinalitySelection);
  Vectors<Coefficient> tempSpace1, tempSpace2;
  MemorySaving<Vectors<Coefficient> > tempEigenVects;
  Vectors<Coefficient>& theFinalEigenSpace = (outputEigenSpace == 0) ? tempEigenVects.getElement() : *outputEigenSpace;
  //WeylGroup& theWeyL = this->theAlgebra.theWeyl;
  theFinalEigenSpace.makeEiBasis(this->getDimension());
  for (int i = 0; i < splittingParSelectedInLevi.cardinalitySelection; i ++) {
    int theGenIndex = splittingParSelectedInLevi.elements[i] +
    this->getOwner().getRank() + this->getOwner().getNumberOfPositiveRoots();
    MatrixTensor<Coefficient>& currentOp = this->getActionGeneratorIndex(theGenIndex);
    Matrix<Coefficient> currentOpMat;
    currentOp.getMatrix(currentOpMat, this->getDimension());
    currentOpMat.getZeroEigenSpaceModifyMe(eigenSpacesPerSimpleGenerator[i]);
    tempSpace1 = theFinalEigenSpace;
    tempSpace2 = eigenSpacesPerSimpleGenerator[i];
    theFinalEigenSpace.intersectTwoLinearSpaces(tempSpace1, tempSpace2, theFinalEigenSpace);
  }
  out << "<br>Eigenvectors:<table> ";
  std::stringstream readyForLatexComsumption;
  readyForLatexComsumption << "\\begin{tabular}{|lll|}\n <br>";
  readyForLatexComsumption << "\\hline\\multicolumn{3}{|c|}{Highest weight $"
  << this->theHWFundamentalCoordsBaseField.toStringLetterFormat("\\omega") << "$}\\\\\n<br>";
  readyForLatexComsumption << "weight fund. coord.& singular vector \\\\\\hline\n<br>";
  Vector<Coefficient> currentWeight;
  Vector<Coefficient> hwFundCoordsNilPart;
  hwFundCoordsNilPart = this->theHWFundamentalCoordsBaseField;
  hwFundCoordsNilPart -= this->theHWFDpartFundamentalCoordS;
  ElementUniversalEnveloping<Coefficient> currentElt, tempElt;
  for (int j = 0; j < theFinalEigenSpace.size; j ++) {
    out << "<tr><td>";
    currentElt.makeZero(this->getOwner());
    Vector<Coefficient>& currentVect = theFinalEigenSpace[j];
    int lastNonZeroIndex = - 1;
    for (int i = 0; i < currentVect.size; i ++) {
      if (!(currentVect[i].isEqualToZero())) {
        tempElt.makeZero(this->getOwner());
        tempElt.addMonomial(this->theGeneratingWordsNonReduced[i], 1);
        tempElt *= currentVect[i];
        currentElt += tempElt;
        lastNonZeroIndex = i;
      }
    }
    currentWeight = subWeyl.ambientWeyl->getFundamentalCoordinatesFromSimple(
      this->theGeneratingWordsWeightsPlusWeightFDpart[lastNonZeroIndex]
    );//<-implicitTypeConversionHere
    currentWeight += hwFundCoordsNilPart;
    readyForLatexComsumption <<  "$" << currentWeight.toStringLetterFormat("\\omega")
    << "$&$" << currentVect.toStringLetterFormat("m") << "$";
    if (currentElt.size() > 1) {
      out << "(";
    }
    if (outputEigenVectors != 0) {
      outputEigenVectors->addOnTop(currentElt);
    }
    if (outputWeightsFundCoords != 0) {
      outputWeightsFundCoords->addOnTop(currentWeight);
    }
    out << currentElt.toString(&global.theDefaultFormat.getElement());
    if (currentElt.size() > 1) {
      out << ")";
    }
    out << " v_\\lambda";
    out << "</td><td>(weight: " << currentWeight.toStringLetterFormat("\\omega") << ")</td></tr>";
    readyForLatexComsumption << "\\\\\n<br>";
  }
  out << "</table>";
  readyForLatexComsumption << "\\hline \n<br> \\end{tabular}";
  out << "<br>Your ready for LaTeX consumption text follows.<br>";
  out << readyForLatexComsumption.str();
  if (report != nullptr) {
    *report = out.str();
  }
}

template <class Coefficient>
MatrixTensor<Coefficient>& ModuleSSalgebra<Coefficient>::getActionSimpleGeneratorIndex(int generatorIndex) {
  Vector<Rational> genWeight = this->getOwner().getWeightOfGenerator(generatorIndex);
  Vector<Rational> targetWeight;
  Pair<MonomialTensor<int, HashFunctions::hashFunction>, MonomialTensor<int, HashFunctions::hashFunction> > currentPair;
  MatrixTensor<Coefficient>& outputMat = this->actionsGeneratorsMaT[generatorIndex];
  Vector<Coefficient> theScalarProds;
  outputMat.makeZero();
  for (int i = 0; i < this->theGeneratingWordsGrouppedByWeight.size; i ++) {
    List<MonomialTensor<int, HashFunctions::hashFunction> >& currentWordList =
    this->theGeneratingWordsIntGrouppedByWeight[i];
    const Vector<Rational>& currentWeight = this->theModuleWeightsSimpleCoords[i];
    targetWeight = currentWeight + genWeight;
    int weightLevelIndex = this->theModuleWeightsSimpleCoords.getIndex(targetWeight);
    if (weightLevelIndex != - 1) {
      int columnOffset = this->getOffsetFromWeightIndex(i);
      int rowOffset = this->getOffsetFromWeightIndex(weightLevelIndex);
      List<MonomialTensor<int, HashFunctions::hashFunction> >&
      otherWordList = this->theGeneratingWordsIntGrouppedByWeight[weightLevelIndex];
      for (int j = 0; j < currentWordList.size; j ++) {
        theScalarProds.setSize(otherWordList.size);
        for (int k = 0; k < otherWordList.size; k ++) {
          if (generatorIndex > this->getOwner().getNumberOfPositiveRoots()) {
            currentPair.object1 = currentWordList[j];
            this->applyTAA(currentPair.object1);
            currentPair.object2 = otherWordList[k];
            currentPair.object2.multiplyByGeneratorPowerOnTheLeft(this->getOwner().getOppositeGeneratorIndex(generatorIndex), 1);
          } else {
            currentPair.object1 = currentWordList[j];
            currentPair.object1.multiplyByGeneratorPowerOnTheLeft(generatorIndex, 1);
            this->applyTAA(currentPair.object1);
            currentPair.object2 = otherWordList[k];
          }
          ProgressReport theReport;
          theScalarProds[k] = this->hwTrace(currentPair, &theReport);
        }
        this->theBilinearFormsInverted[weightLevelIndex].actOnVectorColumn(theScalarProds);
        for (int k = 0; k < theScalarProds.size; k ++) {
          outputMat.addMonomial(MonomialMatrix(rowOffset + k, columnOffset + j), theScalarProds[k]);
        }
      }
    }
  }
  return outputMat;
}

template<class Coefficient>
bool ModuleSSalgebra<Coefficient>::makeFromHW(
  SemisimpleLieAlgebra& inputAlgebra,
  Vector<Coefficient>& HWFundCoords,
  const Selection& selNonSelectedAreElementsLevi,
  const Coefficient& ringUnit,
  const Coefficient& ringZero,
  std::string* outputReport,
  bool computeSimpleGens
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<Coefficient>::makeFromHW");
  ProgressReport theReport;
  this->owner = &inputAlgebra;
  SemisimpleLieAlgebra& theAlgebrA = inputAlgebra;

  int theRank = theAlgebrA.getRank();
  if (HWFundCoords.size != theRank || selNonSelectedAreElementsLevi.numberOfElements != theRank) {
    global.fatal << "I am asked to create a "
    << "generalized Verma module with a semisimple Lie algebra of rank "
    << theRank << " but the input highest weight, "
    << HWFundCoords.toString() << ", has " << HWFundCoords.size << " coordinates and "
    << " the parabolic section indicates rank of "
    << selNonSelectedAreElementsLevi.numberOfElements << ". " << global.fatal;
  }
  WeylGroupData& theWeyl = theAlgebrA.weylGroup;
  this->cachedPairs.clear();
  this->cachedTraces.setSize(0);

  this->parabolicSelectionNonSelectedAreElementsLevi = selNonSelectedAreElementsLevi;
  this->parabolicSelectionSelectedAreElementsLevi = this->parabolicSelectionNonSelectedAreElementsLevi;
  this->parabolicSelectionSelectedAreElementsLevi.invertSelection();

  this->theHWFundamentalCoordsBaseField = HWFundCoords;
  this->theHWDualCoordsBaseFielD.setSize(theRank);
  this->theHWFDpartFundamentalCoordS.setSize(theRank);

  for (int i = 0; i < theRank; i ++) {
    this->theHWFDpartFundamentalCoordS[i] = 0;
    if (this->parabolicSelectionSelectedAreElementsLevi.selected[i]) {
      int theCoord;
      if (!this->theHWFundamentalCoordsBaseField[i].isSmallInteger(&theCoord)) {
        if (outputReport != nullptr) {
          *outputReport = "The given module over the Levi part is not finite dimensional";
        }
        return false;
      }
      this->theHWFDpartFundamentalCoordS[i] = theCoord;
    }
    this->theHWDualCoordsBaseFielD[i] = this->theHWFundamentalCoordsBaseField[i];
    this->theHWDualCoordsBaseFielD[i] *= theWeyl.cartanSymmetric.elements[i][i] / 2;
  }

  this->theHWFDpartSimpleCoordS = theWeyl.getSimpleCoordinatesFromFundamental(this->theHWFDpartFundamentalCoordS, Rational::zero());
  this->theHWFDpartDualCoords = theWeyl.getDualCoordinatesFromFundamental(this->theHWFDpartFundamentalCoordS);
  this->theHWSimpleCoordSBaseField = theWeyl.getSimpleCoordinatesFromFundamental(this->theHWFundamentalCoordsBaseField, ringZero);
  this->theChaR.makeFromWeight(this->theHWSimpleCoordSBaseField, this->owner);

  unsigned long long startingNumRationalOperations =
  Rational::totalLargeAdditions +
  Rational::totalSmallAdditions +
  Rational::totalLargeMultiplications +
  Rational::totalSmallMultiplications;
  LittelmannPath startingPath;
  startingPath.makeFromWeightInSimpleCoords(this->theHWFDpartSimpleCoordS, theWeyl);
  List<List<int> > generatorsIndices;
  if (!startingPath.generateOrbit(
    this->thePaths, generatorsIndices, 1000, &this->parabolicSelectionNonSelectedAreElementsLevi
  )) {
    if (outputReport != nullptr) {
      *outputReport = "Error: number of Littelmann paths exceeded allowed limit of 1000.";
    }
    return false;
  }
  this->theModuleWeightsSimpleCoords.clear();
  Weight<Coefficient> tempCharMon;
  tempCharMon.owner = nullptr;
  this->theCharOverH.makeZero();
  for (int i = 0; i < this->thePaths.size; i ++) {
    this->theModuleWeightsSimpleCoords.addOnTopNoRepetition(*this->thePaths[i].waypoints.lastObject());
    tempCharMon.weightFundamentalCoordinates = theWeyl.getFundamentalCoordinatesFromSimple(*this->thePaths[i].waypoints.lastObject());
    this->theCharOverH.addMonomial(tempCharMon, 1);
  }
  this->theModuleWeightsSimpleCoords.quickSortAscending();
  std::stringstream out2;
  this->theGeneratingWordsGrouppedByWeight.setSize(this->theModuleWeightsSimpleCoords.size);
  this->theGeneratingWordsIntGrouppedByWeight.setSize(this->theModuleWeightsSimpleCoords.size);
  for (int i = 0; i < this->theGeneratingWordsGrouppedByWeight.size; i ++) {
    this->theGeneratingWordsGrouppedByWeight[i].size = 0;
    this->theGeneratingWordsIntGrouppedByWeight[i].size = 0;
  }
  MonomialUniversalEnveloping<Coefficient> currentNonReducedElement;
  MonomialTensor<int, HashFunctions::hashFunction> tempMonInt;
  for (int i = 0; i < this->thePaths.size; i ++) {
    List<int>& currentPath = generatorsIndices[i];
    currentNonReducedElement.makeOne(this->getOwner());
    tempMonInt.makeConstant();
    for (int j = currentPath.size - 1; j >= 0; j --) {
      int theIndex = currentPath[j];
      if (theIndex > 0) {
        theIndex ++;
      }
      currentNonReducedElement.multiplyByGeneratorPowerOnTheRight(
        this->getOwner().getGeneratorFromDisplayIndex(theIndex), ringUnit
      );
      tempMonInt.multiplyByGeneratorPowerOnTheRight(this->getOwner().getGeneratorFromDisplayIndex(theIndex), 1);
    }
    Vector<Rational>& hwCurrent = *this->thePaths[i].waypoints.lastObject();
    int theIndex = this->theModuleWeightsSimpleCoords.getIndex(hwCurrent);
    if (theIndex == - 1) {
      out2 << "Error: could not generate all weights in the weight support. Maybe they are too many? Allowed "
      << "# of weights is 10000";
      if (outputReport != nullptr) {
        *outputReport = out2.str();
      }
      return false;
    }
    this->theGeneratingWordsGrouppedByWeight[theIndex].addOnTop(currentNonReducedElement);
    this->theGeneratingWordsIntGrouppedByWeight[theIndex].addOnTop(tempMonInt);
  }
  this->theGeneratingWordsNonReduced.clear();
  this->theGeneratingWordsNonReduced.setExpectedSize(this->thePaths.size);
  this->theGeneratingWordsNonReducedInt.clear();
  this->theGeneratingWordsNonReducedInt.setExpectedSize(this->thePaths.size);
  this->theGeneratingWordsNonReduced.size = 0;
  this->theGeneratingWordsWeightsPlusWeightFDpart.setSize(0);
  this->theGeneratingWordsWeightsPlusWeightFDpart.reserve(this->thePaths.size);
  for (int i = 0; i < this->theGeneratingWordsGrouppedByWeight.size; i ++) {
    List<MonomialUniversalEnveloping<Coefficient> >& currentList = this->theGeneratingWordsGrouppedByWeight[i];
    List<MonomialTensor<int, HashFunctions::hashFunction> >& currentListInt = this->theGeneratingWordsIntGrouppedByWeight[i];
    currentList.quickSortDescending();
    currentListInt.quickSortDescending();
    for (int j = 0; j < currentList.size; j ++) {
      //wordCounter ++;
      this->theGeneratingWordsNonReduced.addOnTop(currentList[j]);
      this->theGeneratingWordsNonReducedInt.addOnTop(currentListInt[j]);
      this->theGeneratingWordsWeightsPlusWeightFDpart.addOnTop(this->theModuleWeightsSimpleCoords[i]);
    }
  }
  this->intermediateStepForMakeFromHW(ringUnit, ringZero);
  this->NumCachedPairsBeforeSimpleGen = this->cachedPairs.size;
  this->NumRationalMultiplicationsAndAdditionsBeforeSimpleGen =
  Rational::totalLargeAdditions + Rational::totalSmallAdditions +
  Rational::totalLargeMultiplications + Rational::totalSmallMultiplications - startingNumRationalOperations;
  bool isBad = false;
  for (int k = 0; k < this->theBilinearFormsAtEachWeightLevel.size; k ++) {
    Matrix<Coefficient>& theBF = this->theBilinearFormsAtEachWeightLevel[k];
    Matrix<Coefficient>& theBFinverted = this->theBilinearFormsInverted[k];
    if (!theBF.isNonNegativeAllEntries()) {
      this->flagConjectureBholds = false;
    }
    if (theBFinverted.numberOfRows <= 0) {
      isBad = true;
    }
  }
  if (isBad) {
    if (outputReport != nullptr) {
      out2 << "<br>Error: the Littelmann-path induced monomials do not give a monomial basis. ";
      *outputReport = out2.str();
    }
    return false;
  }
  ElementSemisimpleLieAlgebra<Rational> tempSSElt;
  if (computeSimpleGens) {
    for (int k = 0; k < 2; k ++) {
      for (int j = 0; j < this->getOwner().getRank(); j ++) {
        if (this->parabolicSelectionSelectedAreElementsLevi.selected[j]) {
          int theIndex = this->getOwner().getNumberOfPositiveRoots() - j - 1;
          if (k == 1) {
            theIndex = this->getOwner().getNumberOfPositiveRoots() + this->getOwner().getRank() + j;
          }
          tempSSElt.makeGenerator(theIndex, this->getOwner());
          if (outputReport != nullptr) {
            out2 << "<hr>Simple generator: " << tempSSElt.toString(&global.theDefaultFormat.getElement());
          }
          MatrixTensor<Coefficient>& theMatrix = this->getActionGeneratorIndex(theIndex);
          std::stringstream tempStream;
          tempStream << "computing action simple generator index " << (2 * k - 1) * (j + 1) << " ... ";
          theReport.report(tempStream.str());
          tempStream << " done!";
          theReport.report(tempStream.str());
          if (outputReport != nullptr) {
            if (this->getDimension() < 50) {
              out2 << "<br>Matrix of elemenent in the m_i basis:<br>" << HtmlRoutines::getMathNoDisplay(theMatrix.toString(), 5000);
            } else {
              out2 << "<br>Matrix of elemenent in the m_i basis:<br>" << theMatrix.toString();
            }
          }
      /*    for (int j = 0; j < this->actionsSimpleGens[i].size; j ++)
            for (int k = 0; k< this->actionsSimpleGens[i][j].size; k++) {
              out << "<br>" << theSimpleGenerator.toString(global, tempFormat) << "\\cdot "
              << this->theGeneratingWordsGrouppedByWeight[j][k].toString(false, false, tempFormat)
              << "\\cdot v=" << this->actionsSimpleGens[i][j][k].toString(global, tempFormat)
              << "\\cdot v"
              ;
            }*/
          if (k == 1) {
            this->getActionGeneratorIndex(this->getOwner().getNumberOfPositiveRoots() + j);
          }
        }
      }
    }
  }
  if (outputReport != nullptr) {
    *outputReport = out2.str();
  }
  this->flagIsInitialized = true;
  theReport.report("Done with module generation");
  return true;
}

template<class Coefficient>
void ModuleSSalgebra<Coefficient>::intermediateStepForMakeFromHW(
  const Coefficient& ringUnit, const Coefficient& ringZero
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<Coefficient>::intermediateStepForMakeFromHW");
  ProgressReport theReport;
  ProgressReport theReport2;
  Vector<Rational> targetWeight;
  this->theBilinearFormsAtEachWeightLevel.setSize(this->theGeneratingWordsGrouppedByWeight.size);
  this->theBilinearFormsInverted.setSize(this->theGeneratingWordsGrouppedByWeight.size);
  this->ComputedGeneratorActions.initialize(this->getOwner().getNumberOfGenerators());
  this->actionsGeneratorS.setSize(this->getOwner().getNumberOfGenerators());
  this->actionsGeneratorsMaT.setSize(this->getOwner().getNumberOfGenerators());
  int numScalarProducts = 0;
  this->flagConjectureBholds = true;
  this->flagConjectureCholds = true;
  for (int l = 0; l < this->theGeneratingWordsGrouppedByWeight.size; l ++) {
    Matrix<Coefficient>& currentBF = this->theBilinearFormsAtEachWeightLevel[l];
    List<MonomialUniversalEnveloping<Coefficient> >& currentWordList =
    this->theGeneratingWordsGrouppedByWeight[l];
    List<MonomialTensor<int, HashFunctions::hashFunction> >& currentWordListInt = this->theGeneratingWordsIntGrouppedByWeight[l];
    currentBF.initialize(currentWordList.size, currentWordList.size);
    for (int i = 0; i < currentWordList.size; i ++) {
      for (int j = i; j < currentWordList.size; j ++) {
        std::stringstream tempStream;
        tempStream << " Computing Shapovalov form layer " << l << " out of " << this->theGeneratingWordsGrouppedByWeight.size
        << " between indices " << i + 1 << " and " << j + 1 << " out of " << currentWordList.size;
        numScalarProducts ++;
        currentBF.elements[i][j] = this->hwtaabfSimpleGensOnly(currentWordListInt[i], currentWordListInt[j], &theReport2);
        theReport.report(tempStream.str());
        if (i != j) {
          currentBF.elements[j][i] = currentBF.elements[i][j];
        }
      }
    }
    Matrix<Coefficient> tempMat;
    tempMat = currentBF;
    Coefficient tempRat;
    tempMat.computeDeterminantOverwriteMatrix(tempRat, ringUnit, ringZero);
    if (!tempRat.isEqualToZero()) {
      this->theBilinearFormsInverted[l] = currentBF;
      this->theBilinearFormsInverted[l].invert();
      if (!currentBF.isNonNegativeAllEntries()) {
        this->flagConjectureBholds = false;
      }
    } else {
      this->theBilinearFormsInverted[l].initialize(0, 0);
    }
  }
}

template <class Coefficient>
template <class ResultType>
void ModuleSSalgebra<Coefficient>::getElementsNilradical(
  List<ElementUniversalEnveloping<ResultType> >& output,
  bool useNegativeNilradical,
  List<int>* outputListOfGenerators,
  bool useNilWeight,
  bool ascending
) {
  SemisimpleLieAlgebra& ownerSS = this->getOwner();
  ownerSS.orderNilradical(this->parabolicSelectionNonSelectedAreElementsLevi, useNilWeight, ascending);
  ElementUniversalEnveloping<ResultType> theElt;
  output.setSize(0);
  output.reserve(ownerSS.getNumberOfPositiveRoots());

  int theBeginning = useNegativeNilradical ? 0: ownerSS.getNumberOfPositiveRoots() +ownerSS.getRank();
  MemorySaving<List<int> > tempList;
  if (outputListOfGenerators == nullptr) {
    outputListOfGenerators = &tempList.getElement();
  }
  outputListOfGenerators->setSize(0);
  outputListOfGenerators->reserve(ownerSS.getNumberOfPositiveRoots());
  for (int i = theBeginning; i < theBeginning+ownerSS.getNumberOfPositiveRoots(); i ++) {
    if (this->isNotInLevi(i)) {
      outputListOfGenerators->addOnTop(i);
    }
  }
  //bubble sort:
  for (int i = 0; i < outputListOfGenerators->size; i ++) {
    for (int j = i + 1; j < outputListOfGenerators->size; j ++) {
      if (
        ownerSS.UEGeneratorOrderIncludingCartanElts[(*outputListOfGenerators)[i]] >
        ownerSS.UEGeneratorOrderIncludingCartanElts[(*outputListOfGenerators)[j]]
      ) {
        outputListOfGenerators->swapTwoIndices(i, j);
      }
    }
  }
  for (int i = 0; i < outputListOfGenerators->size; i ++) {
    theElt.makeOneGeneratorCoefficientOne(outputListOfGenerators->objects[i], *this->owner);
    output.addOnTop(theElt);
  }
}

template<class Coefficient>
void ModuleSSalgebra<Coefficient>::checkConsistency() {
  MacroRegisterFunctionWithName("ModuleSSalgebra<Coefficient>::TestConsistency");
  ProgressReport theReport;
  MatrixTensor<Coefficient> left, right, output, otherOutput, tempMat, diffMat;
  for (int i = 0; i < this->getOwner().getNumberOfGenerators(); i ++) {
    for (int j = 0; j < this->getOwner().getNumberOfGenerators(); j ++) {
      left = this->getActionGeneratorIndex(i);
      right = this->getActionGeneratorIndex(j);
      output = right;
      output.lieBracketOnTheLeft(left);
      ElementSemisimpleLieAlgebra<Rational> leftGen, rightGen, outputGen;
      leftGen.makeGenerator(i, *this->theAlgebras, this->indexAlgebra);
      rightGen.makeGenerator(j, *this->theAlgebras, this->indexAlgebra);
      this->getOwner().lieBracket(leftGen, rightGen, outputGen);
      otherOutput.makeZero();
      for (int k = 0; k < outputGen.size(); k ++) {
        tempMat = this->getActionGeneratorIndex(outputGen[k].generatorIndex);
        tempMat *= outputGen.coefficients[k];
        otherOutput += tempMat;
      }
      diffMat = otherOutput;
      diffMat -= output;
      if (!diffMat.isEqualToZero()) {
        global.fatal
        << "<br>[" << left.toString() << ", " << right.toString() << "] = "
        << output.toString() << "<br> [" << leftGen.toString()
        << ", " << rightGen.toString() << "] = " << outputGen.toString()
        << "<br>"
        << outputGen.toString() << "->" << otherOutput.toString()
        << "<br>Something is wrong with the algorithm, a check fails! "
        << global.fatal;
      } else {
        std::stringstream tempStream;
        tempStream << "tested index " << i + 1
        << " out of " << this->getOwner().getNumberOfGenerators()
        << ", " << j + 1 << " out of " << this->getOwner().getNumberOfGenerators();
        theReport.report(tempStream.str());
      }
    }
  }
  for (int i = 0; i < this->getOwner().getNumberOfPositiveRoots(); i ++) {
    left = this->getActionGeneratorIndex(i);
    right = this->getActionGeneratorIndex(this->getOwner().getNumberOfGenerators() - 1 - i);
    left.transpose();
    left -= right;
    left = this->getActionGeneratorIndex(i);
    right.lieBracketOnTheLeft(left);
  }
  global.comments << "Consistency check passed successfully!";
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::getFDchar(CharacterSemisimpleLieAlgebraModule<Coefficient>& output) {
  output.makeZero();
  if (this->theHWFundamentalCoordsBaseField.size <= 0) {
    return;
  }
  Weight<Coefficient> tempMon;
  tempMon.owner = &this->getOwner();
  WeylGroupData& theWeyl = this->getOwner().weylGroup;
  for (int i = 0; i < this->theModuleWeightsSimpleCoords.size; i ++) {
    tempMon.weightFundamentalCoordinates = theWeyl.getFundamentalCoordinatesFromSimple(this->theModuleWeightsSimpleCoords[i]);
    output.addMonomial(tempMon, this->theGeneratingWordsGrouppedByWeight[i].size);
  }
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::expressAsLinearCombinationHomogenousElement(
  ElementUniversalEnveloping<Coefficient>& inputHomogeneous,
  ElementUniversalEnveloping<Coefficient>& outputHomogeneous,
  int indexInputBasis,
  const Vector<Coefficient>& substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  Vector <Coefficient> theScalarProducts;
  List<MonomialUniversalEnveloping<Coefficient> >& inputBasis =
  this->theGeneratingWordsGrouppedByWeight[indexInputBasis];
  theScalarProducts.setSize(inputBasis.size);

  for (int i = 0; i < inputBasis.size; i ++) {
    std::stringstream tempStream;
    inputHomogeneous.highestWeightTransposeAntiAutomorphismBilinearForm(
      inputBasis[i],
      theScalarProducts[i],
      &substitutionHiGoesToIthElement,
      ringUnit,
      ringZero,
      &tempStream
    );
  }
  this->theBilinearFormsInverted[indexInputBasis].actOnVectorColumn(theScalarProducts, ringZero);
  outputHomogeneous.makeZero(*this->theAlgebras, this->indexAlgebra);
  ElementUniversalEnveloping<Coefficient> tempElt;
  for (int i = 0; i < theScalarProducts.size; i ++) {
    tempElt.makeZero(*this->theAlgebras, this->indexAlgebra);
    tempElt.addMonomial(this->theGeneratingWordsGrouppedByWeight[indexInputBasis][i], ringUnit);
    tempElt *= theScalarProducts[i];
    outputHomogeneous += tempElt;
  }
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::reset() {
  this->actionsGeneratorsMaT.setSize(0);
  this->actionsGeneratorS.setSize(0);
  this->ComputedGeneratorActions.initialize(0);
  this->owner = nullptr;
  this->theGeneratingWordsNonReduced.clear();
  // Note: for some reason, the linker fails to resolve without the explicit template
  // specialization below.
  // [Update:] made a bug report on this in the gcc bug tracker.
  // This issue has officially been recognized as a gcc bug. Hope to get a fix soon.
  this->theGeneratingWordsNonReducedInt.clear();
  this->theGeneratingWordsWeightsPlusWeightFDpart.setSize(0);
  this->thePaths.setSize(0);
  this->theGeneratingWordsGrouppedByWeight.setSize(0);
  this->theGeneratingWordsIntGrouppedByWeight.setSize(0);
  //List<ElementUniversalEnveloping<Coefficient> > theSimpleGens;
  //List<List<List<ElementUniversalEnveloping<Coefficient> > > > actionsSimpleGens;
  //List<Matrix<Coefficient> > actionsSimpleGensMatrixForM;
  this->theBilinearFormsAtEachWeightLevel.setSize(0);
  this->theBilinearFormsInverted.setSize(0);
  //Vectors<Rational> weightsSimpleGens;
  this->theHWDualCoordsBaseFielD.setSize(0);
  this->theHWSimpleCoordSBaseField.setSize(0);
  this->theHWFundamentalCoordsBaseField.setSize(0);
  this->theHWFDpartDualCoords.setSize(0);
  this->theHWFDpartSimpleCoordS.setSize(0);
  this->theHWFDpartFundamentalCoordS.setSize(0);
  //List<List<Matrix<Coefficient> > >
  this->theModuleWeightsSimpleCoords.clear();
  this->theCharOverH.makeZero();
  this->theChaR.makeZero();
  this->parabolicSelectionNonSelectedAreElementsLevi.initialize(0);
  this->parabolicSelectionSelectedAreElementsLevi.initialize(0);
  this->highestWeightVectorNotation = "";
  //Note: for some reason, the linker fails to resolve without the
  //explicit template specialization below.
  //[Update:] This is now a recognized gcc bug.
  this->cachedPairs.clear();
  this->cachedTraces.setSize(0);
  this->flagIsInitialized = false;
  this->flagConjectureBholds = true;
  this->flagConjectureCholds = true;
  this->NumCachedPairsBeforeSimpleGen = 0;
  this->NumRationalMultiplicationsAndAdditionsBeforeSimpleGen = 0;
  this->MaxNumCachedPairs = 1000000;
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::getAdActionHomogenousElt(
  ElementUniversalEnveloping<Coefficient>& inputHomogeneous,
  Vector<Rational>& weightUEEltSimpleCoords,
  List<List<ElementUniversalEnveloping<Coefficient> > >& outputSortedByArgumentWeight,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  Vector<Rational> targetWeight;
  outputSortedByArgumentWeight.setSize(this->theGeneratingWordsGrouppedByWeight.size);
  ElementUniversalEnveloping<Coefficient> theElt;
  std::string generatorString = inputHomogeneous.toString();
  ProgressReport theReport;
  for (int i = 0; i < this->theGeneratingWordsGrouppedByWeight.size; i ++) {
    List<MonomialUniversalEnveloping<Coefficient> >& currentWordList = this->theGeneratingWordsGrouppedByWeight[i];
    List<ElementUniversalEnveloping<Coefficient> >& outputCurrentList = outputSortedByArgumentWeight[i];
    outputCurrentList.setSize(currentWordList.size);
    Vector<Rational>& currentWeight = this->theModuleWeightsSimpleCoords[i];
    targetWeight = currentWeight + weightUEEltSimpleCoords;
    int theIndex = this->theModuleWeightsSimpleCoords.getIndex(targetWeight);
    for (int j = 0; j < currentWordList.size; j ++) {
      std::stringstream progressStream;
      progressStream << "Computing action of "
      << generatorString << " on weight layer " << i + 1
      << " out of " << this->theGeneratingWordsGrouppedByWeight.size
      << ", word " << j + 1 << " out of " << currentWordList.size << "...";
      theReport.report(progressStream.str());
      ElementUniversalEnveloping<Coefficient>& currentOutputWord = outputCurrentList[j];
      if (theIndex == - 1) {
        currentOutputWord.makeZero(*this->theAlgebras, this->indexAlgebra);
      } else {
        theElt = inputHomogeneous;
        theElt.multiplyBy(currentWordList[j], ringUnit);
        this->expressAsLinearCombinationHomogenousElement(
          theElt, currentOutputWord, theIndex, this->theHWDualCoordsBaseFielD, ringUnit, ringZero
        );
      }
      progressStream << " done!";
      theReport.report(progressStream.str());
    }
  }
}

template <class Coefficient>
void ElementTensorsGeneralizedVermas<Coefficient>::substitution(
  const PolynomialSubstitution<Rational>& theSub,
  ListReferences<ModuleSSalgebra<Coefficient> >& theMods
) {
  ElementTensorsGeneralizedVermas<Coefficient> output;
  MonomialTensorGeneralizedVermas<Coefficient> currentMon;
  output.makeZero();
  Coefficient tempCF;
  for (int i = 0; i < this->size(); i ++) {
    currentMon = (*this)[i];
    currentMon.substitution(theSub, theMods);
    tempCF = this->coefficients[i];
    tempCF.substitution(theSub, 1, nullptr);
    output.addMonomial(currentMon, tempCF);
  }
  *this = output;
}

template <class Coefficient>
ElementTensorsGeneralizedVermas<Coefficient>& ElementTensorsGeneralizedVermas<
  Coefficient
>::operator=(const ElementSumGeneralizedVermas<Coefficient>& other) {
  this->makeZero();
  MonomialTensorGeneralizedVermas<Coefficient> theMon;
  for (int i = 0; i < other.size(); i ++) {
    theMon = other[i];
    this->addMonomial(theMon, other.coefficients[i]);
  }
  return *this;
}

template <class Coefficient>
bool ElementTensorsGeneralizedVermas<Coefficient>::multiplyOnTheLeft(
  const ElementUniversalEnveloping<Coefficient>& theUE,
  ElementTensorsGeneralizedVermas<Coefficient>& output,
  SemisimpleLieAlgebra& ownerAlgebra,
  const Coefficient& ringUnit
) const {
  ElementTensorsGeneralizedVermas<Coefficient> buffer;
  output.makeZero();
  for (int i = 0; i < theUE.size(); i ++) {
    if (!this->multiplyOnTheLeft(theUE[i], buffer, ownerAlgebra, ringUnit)) {
      ElementSumGeneralizedVermas<Coefficient> tempOutput;
      global.comments << "<hr>emergency mode!";
      for (int j = 0; j < this->size(); j ++) {
        const MonomialTensorGeneralizedVermas<Coefficient>& currentMon = (*this)[j];
        if (currentMon.theMons.size != 1) {
          return false;
        }
        MonomialGeneralizedVerma<Coefficient>& currentSingleMon = currentMon.theMons[0];
        if (j == 0) {
          tempOutput.makeZero();
        }
        tempOutput.addMonomial(currentSingleMon, this->coefficients[j]);
      }
      tempOutput.multiplyMeByUEEltOnTheLeft(theUE);
      output = tempOutput;
      return true;
    }
    buffer *= theUE.coefficients[i];
    output += buffer;
  }
  return true;
}

template <class Coefficient>
void ElementTensorsGeneralizedVermas<Coefficient>::makeHWV(
  ModuleSSalgebra<Coefficient>& theOwner, const Coefficient& ringUnit
) {
  MonomialTensorGeneralizedVermas<Coefficient> tensorMon;
  Coefficient currentCoeff;
  currentCoeff = ringUnit;
  tensorMon.theMons.setSize(1);
  MonomialGeneralizedVerma<Coefficient>& theMon = tensorMon.theMons[0];
  theMon.indexFDVector = theOwner.theGeneratingWordsNonReduced.size - 1;
  theMon.makeConstant(theOwner);
  this->makeZero();
  this->addMonomial(tensorMon, ringUnit);
}

template <class Coefficient>
bool ElementTensorsGeneralizedVermas<Coefficient>::multiplyOnTheLeft(
  const MonomialUniversalEnveloping<Coefficient>& theUE,
  ElementTensorsGeneralizedVermas<Coefficient>& output,
  SemisimpleLieAlgebra& ownerAlgebra,
  const Coefficient& ringUnit
) const {
  MacroRegisterFunctionWithName("ElementTensorsGeneralizedVermas<Coefficient>::multiplyOnTheLeft");
  if (&output == this) {
    global.fatal << "Output equals input, this is not supposed to happen. " << global.fatal;
  }
  output = *this;
  ElementTensorsGeneralizedVermas<Coefficient> buffer;
  for (int i = theUE.powers.size - 1; i >= 0; i --) {
    int thePower;
    if (!theUE.powers[i].isSmallInteger(&thePower)) {
      return false;
    }
    int theIndex = theUE.generatorsIndices[i];
    for (int j = 0; j < thePower; j ++) {
      output.multiplyByElementLieAlg(buffer, ownerAlgebra, theIndex, ringUnit);
      output = buffer;
    }
  }
  return true;
}

template <class Coefficient>
void ElementTensorsGeneralizedVermas<Coefficient>::multiplyByElementLieAlg(
  ElementTensorsGeneralizedVermas<Coefficient>& output,
  SemisimpleLieAlgebra& ownerAlgebra,
  int indexGenerator,
  const Coefficient& ringUnit
) const {
  output.makeZero();
  MonomialTensorGeneralizedVermas<Coefficient> accumMon, monActedOn;
  ElementSumGeneralizedVermas<Coefficient> tempElt;
  ElementUniversalEnveloping<Coefficient> theGenerator;
  theGenerator.makeOneGenerator(indexGenerator, ownerAlgebra, ringUnit);
  Coefficient currentCoeff;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialTensorGeneralizedVermas<Coefficient>& currentMon = (*this)[i];
    accumMon.theMons.setSize(0);
    for (int j = 0; j < currentMon.theMons.size; j ++) {
      tempElt.makeZero();
      tempElt.addMonomial(currentMon.theMons[j], ringUnit);
      tempElt.multiplyMeByUEEltOnTheLeft(theGenerator);
      for (int k = 0; k < tempElt.size(); k ++) {
        currentCoeff = this->coefficients[i];
        currentCoeff *= tempElt.coefficients[k];
        monActedOn = accumMon;
        monActedOn *= tempElt[k];
        for (int l = j + 1; l < currentMon.theMons.size; l ++) {
          monActedOn *= currentMon.theMons[l];
        }
        output.addMonomial(monActedOn, currentCoeff);
      }
      accumMon *= currentMon.theMons[j];
    }
  }
}

template <class Coefficient>
void ElementTensorsGeneralizedVermas<Coefficient>::tensorOnTheRight(
  const ElementTensorsGeneralizedVermas<Coefficient>& right
) {
  MacroRegisterFunctionWithName("ElementTensorsGeneralizedVermas<Coefficient>::tensorOnTheRight");
  if (right.isEqualToZero()) {
    this->makeZero();
    return;
  }
  int maxNumMonsFinal = this->size() * right.size();
  ElementTensorsGeneralizedVermas<Coefficient> output;
  MonomialTensorGeneralizedVermas<Coefficient> bufferMon;
  output.makeZero();
  output.setExpectedSize(maxNumMonsFinal);
  Coefficient theCoeff;
  for (int i = 0; i < right.size(); i ++) {
    for (int j = 0; j < this->size(); j ++) {
      bufferMon = (*this)[j];
      bufferMon *= right[i];
      theCoeff = this->coefficients[j];
      theCoeff *= right.coefficients[i];
      output.addMonomial(bufferMon, theCoeff);
    }
  }
  *this = output;
}

template <class Coefficient>
std::string ModuleSSalgebra<Coefficient>::toString(FormatExpressions* format) const {
  MacroRegisterFunctionWithName("ModuleSSalgebra::toString");
  if (this->owner == nullptr) {
    return "(Error: module not initialized)";
  }
  SemisimpleLieAlgebra& theAlgebrA = *this->owner;
  WeylGroupData& theWeyl = theAlgebrA.weylGroup;
  std::stringstream out;
  out << "<br>Semisimple Lie algebra acting on generalized Verma module: "
  << theAlgebrA.toStringLieAlgebraName() << ".";
  out << "<br>Parabolic selection: "
  << this->parabolicSelectionNonSelectedAreElementsLevi.toString();
  out << "<br>Highest weight of Generalized Verma module in fundamental coordinates: "
  << this->theHWFundamentalCoordsBaseField.toString();
  out << "<br>In simple coordinates: "
  << this->theHWSimpleCoordSBaseField.toString();
  out << "<br>Finite dimensional part h. w. fundamental coordinates: "
  << this->theHWFDpartFundamentalCoordS.toString();
  out << "<br>Finite dimensinoal part h. w. simple coords: "
  << this->theHWFDpartSimpleCoordS.toString();
  out << "<br>Inducing module character (over the Cartan subalgebra): ";
  FormatExpressions latexFormat;
  latexFormat.flagUseLatex = true;
  latexFormat.flagUseHTML = false;
  latexFormat.maximumLineLength = 0;
  if (this->theCharOverH.size() < 100) {
    out << HtmlRoutines::getMathNoDisplay(this->theCharOverH.toString(&latexFormat));
  } else {
    out << this->theCharOverH.toString();
  }
  out << "<br>Dimensionn of the finite dimensional part of the module: " << this->getDimension();
  out << "<br>A module basis follows.";
  out << "<table><tr><td>Monomial label</td><td>Definition</td><td>Littelmann path string</td></tr>";
  ElementWeylGroup tempWelt;
  int wordCounter = 0;
  SimpleReflection aGen;
  for (int i = 0; i < this->theGeneratingWordsGrouppedByWeight.size; i ++) {
    List<MonomialUniversalEnveloping<Coefficient> >& currentList = this->theGeneratingWordsGrouppedByWeight[i];
    List<MonomialTensor<int, HashFunctions::hashFunction> >& currentListInt = this->theGeneratingWordsIntGrouppedByWeight[i];
    for (int j = 0; j < currentList.size; j ++) {
      wordCounter ++;
      tempWelt.generatorsLastAppliedFirst.setSize(currentListInt[j].generatorsIndices.size);
      for (int k = 0; k < currentListInt[j].generatorsIndices.size; k ++) {
        aGen.makeSimpleReflection(theWeyl.rootsOfBorel.size - 1 - currentListInt[j].generatorsIndices[k]);
        tempWelt.generatorsLastAppliedFirst[k] = aGen;
      }
      out << "<tr><td>m_{ " << wordCounter << "} </td><td>"
      << currentList[j].toString(&global.theDefaultFormat.getElement())
      << "  v_\\lambda</td><td>" << tempWelt.toString() << "</td> </tr>";
    }
  }
  out << "</table>";

  out << "<br>Character: " << this->theChaR.toString();
  out << "<br>Computed generator actions ("
  << this->ComputedGeneratorActions.cardinalitySelection << " out of "
  << this->actionsGeneratorsMaT.size << " computed actions) follow. "
  << "Note that generator actions are computed on demand, only the simple "
  << "Chevalley generators are computed by default. ";
  out << "<table><tr><td>Generator</td><td>Action</td></tr>";
  ElementSemisimpleLieAlgebra<Rational> tempSSElt;
  for (int i = 0; i < this->actionsGeneratorsMaT.size; i ++) {
    if (this->ComputedGeneratorActions.selected[i]) {
      tempSSElt.makeGenerator(i, theAlgebrA);
      out << "<tr>";
      out << "<td>"
      << HtmlRoutines::getMathNoDisplay(tempSSElt.toString(format))
      << "</td>";
      out << "<td>";
      if (this->getDimension() < 28) {
        Matrix<Coefficient> outputMat;
        this->actionsGeneratorsMaT[i].getMatrix(outputMat, this->getDimension());
        out
        << HtmlRoutines::getMathNoDisplay(outputMat.toString(&latexFormat), 5000)
        << " = ";
        out << this->actionsGeneratorsMaT[i].toString();
      } else {
        out << this->actionsGeneratorsMaT[i].toString();
      }
      out << "</td></tr>";
    }
  }
  out << "</table>";
  DrawingVariables theDV;
  this->theCharOverH.drawMeAssumeCharIsOverCartan(theWeyl, theDV);
  out << " A picture of the weight support follows. "
  << theDV.getHTMLDiv(theWeyl.getDimension(), false);

  bool isBad = false;
  for (int k = 0; k < this->theBilinearFormsAtEachWeightLevel.size; k ++) {
    Matrix<Coefficient>& theBF = this->theBilinearFormsAtEachWeightLevel[k];
    Matrix<Coefficient>& theBFinverted = this->theBilinearFormsInverted[k];
    out << "<hr>weight in simple coords: " << this->theModuleWeightsSimpleCoords[k].toString();
    List<MonomialUniversalEnveloping<Coefficient> >& currentList = this->theGeneratingWordsGrouppedByWeight[k];
    for (int i = 0; i < currentList.size; i ++) {
      MonomialUniversalEnveloping<Coefficient>& currentElt = currentList[i];
      out << "<br>monomial " << i + 1 << ": "
      << currentElt.toString(&global.theDefaultFormat.getElement());
    }
    out << "; Matrix of Shapovalov form associated to current weight level: <br> "
    << theBF.toString(&global.theDefaultFormat.getElement());
/*    if (!theBF.isPositiveDefinite()) {
      monomialDetailStream << "<b>Is not positive definite!</b>";
      this->flagConjectureCholds = false;
    }
    else
      monomialDetailStream << " (positive definite)";*/
    if (!theBF.isNonNegativeAllEntries()) {
      out << "<b>Has negative entries</b>";
    } else {
      out << " (positive entries only )";
    }
    out << " corresonding inverted matrix:<br>";
    if (theBFinverted.numberOfRows > 0) {
      out << theBFinverted.toString(&global.theDefaultFormat.getElement());
    } else {
      out << "<b>The matrix of the bilinear form is not invertible!</b>";
      isBad = true;
    }
  }
  if (isBad) {
    out << "<br>Error: the Littelmann-path induced "
    << "monomials do not give a monomial basis. ";
  }
  out << "<br>Cached Shapovalov products before generator action computation: "
  << this->cachedPairs.size << ". Dimension : "
  << this->getDimension();
  out << "<br>Cached Shapovalov products final: "
  << this->cachedPairs.size << "; dimension : " << this->getDimension();
  return out.str();
}

template <class Coefficient>
bool ModuleSSalgebra<Coefficient>::isNotInParabolic(int theGeneratorIndex) {
  Vector<Rational> theWeight = this->getOwner().getWeightOfGenerator(theGeneratorIndex);
  for (int j = 0; j < this->parabolicSelectionNonSelectedAreElementsLevi.cardinalitySelection; j ++) {
    if (!(theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]] < 0)) {
      return true;
    }
  }
  return false;
}

template <class Coefficient>
bool ModuleSSalgebra<Coefficient>::isNotInLevi(int theGeneratorIndex) {
  Vector<Rational> theWeight = this->getOwner().getWeightOfGenerator(theGeneratorIndex);
  for (int j = 0; j < this->parabolicSelectionNonSelectedAreElementsLevi.cardinalitySelection; j ++) {
    if (!theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[j]].isEqualToZero()) {
      return true;
    }
  }
  return false;
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::getGenericUnMinusElt(
  bool shiftPowersByNumVarsBaseField,
  ElementUniversalEnveloping<RationalFraction<Rational> >& output,
  bool useNilWeight,
  bool ascending
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra::getGenericUnMinusElt");
  List<ElementUniversalEnveloping<Coefficient> > eltsNilrad;
  this->getElementsNilradical(eltsNilrad, true, useNilWeight, ascending);
  RationalFraction<Rational> tempRF;
  output.makeZero(*this->theAlgebras, this->indexAlgebra);
  MonomialUniversalEnveloping<RationalFraction<Rational> > tempMon;
  tempMon.makeConstant(*this->theAlgebras, this->indexAlgebra);
  int varShift = 0;
  if (shiftPowersByNumVarsBaseField) {
    varShift = this->minimalNumberOfVariables();
  }
  for (int i = 0; i < eltsNilrad.size; i ++) {
    tempRF.makeOneLetterMonomial(i + varShift, 1);
    tempMon.multiplyByGeneratorPowerOnTheRight(eltsNilrad[i][0].generatorsIndices[0], tempRF);
  }
  tempRF.makeOne();
  output.addMonomial(tempMon, tempRF);
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::getGenericUnMinusElt(
  bool shiftPowersByNumVarsBaseField,
  ElementUniversalEnveloping<Polynomial<Rational> >& output,
  bool useNilWeight,
  bool ascending
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra::getGenericUnMinusElt");
  List<ElementUniversalEnveloping<Coefficient> > eltsNilrad;
  this->getElementsNilradical(eltsNilrad, true, nullptr, useNilWeight, ascending);
  Polynomial<Rational> tempRF;
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<Polynomial<Rational> > tempMon;
  tempMon.makeOne(*this->owner);
  int varShift = 0;
  if (shiftPowersByNumVarsBaseField) {
    varShift = this->minimalNumberOfVariables();
  }
  for (int i = 0; i < eltsNilrad.size; i ++) {
    tempRF.makeMonomial(i + varShift, 1, 1);
    tempMon.multiplyByGeneratorPowerOnTheRight(eltsNilrad[i][0].generatorsIndices[0], tempRF);
  }
  tempRF.makeOne();
  output.addMonomial(tempMon, tempRF);
}

template <class Coefficient>
bool ModuleSSalgebra<Coefficient>::getActionGeneralizedVermaModuleAsDifferentialOperator(
  ElementSemisimpleLieAlgebra<Rational>& inputElt,
  QuasiDifferentialOperator<Rational>& output,
  bool useNilWeight,
  bool ascending
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra_CoefficientType::getActionGeneralizedVermaModuleAsDifferentialOperator");
  List<ElementUniversalEnveloping<Coefficient> > eltsNilrad;
  List<int> indicesNilrad;
  this->getElementsNilradical(eltsNilrad, true, &indicesNilrad, useNilWeight, ascending);
  ElementUniversalEnveloping<Polynomial<Rational> > theGenElt, result;
  this->getGenericUnMinusElt(true, theGenElt, useNilWeight, ascending);
  result.assignElementLieAlgebra(inputElt, *this->owner, 1);
  Polynomial<Rational> onePolynomial;
  onePolynomial.makeConstant(1);
  theGenElt.simplify(onePolynomial);
  result *= theGenElt;
  result.simplify(onePolynomial);
  MatrixTensor<Polynomial<Rational> > endoPart, tempMT, idMT;
  idMT.makeIdentitySpecial();
  MatrixTensor<RationalFraction<Rational> > tempMat1;

  int varShift = this->minimalNumberOfVariables();
  ElementWeylAlgebra<Rational> weylPartSummand, exponentContribution, oneIndexContribution,
  eulerOperatorContribution;
  Polynomial<Rational> tempP1, negativeExponentDenominatorContribution, theCoeff;
  QuasiDifferentialMononomial monQDO, monQDO2;
  Rational tempRat;
  output.makeZero();
  Rational currentShift;
  for (int i = 0; i < result.size(); i ++) {
    //problemCounter ++;
    const MonomialUniversalEnveloping<Polynomial<Rational> >& currentMon = result[i];
    endoPart = idMT;
    for (int j = currentMon.powers.size - 1; j >= indicesNilrad.size; j --) {
      int thePower = 0;
      if (!currentMon.powers[j].isSmallInteger(&thePower)) {
        return false;
      }
      tempMat1 = this->getActionGeneratorIndex(currentMon.generatorsIndices[j]);
      tempMT.makeZero();
      for (int k = 0; k < tempMat1.size(); k ++) {
        if (tempMat1.coefficients[k].expressionType == RationalFraction<Rational>::typeRationalFunction) {
          return false;
        }
        tempMat1.coefficients[k].getNumerator(tempP1);
        tempMT.addMonomial(tempMat1[k], tempP1);
      }
      MathRoutines::raiseToPower(tempMT, thePower, idMT);
      endoPart *= tempMT;
    }
    exponentContribution.makeOne();
    theCoeff = result.coefficients[i];
    for (int j = 0; j < indicesNilrad.size; j ++) {
      currentMon.powers[j].getConstantTerm(currentShift);
      ElementWeylAlgebra<Rational>::getStandardOrderDifferentialOperatorCorrespondingToNRaisedTo(
        currentShift, j + varShift, oneIndexContribution, negativeExponentDenominatorContribution
      );
      exponentContribution *= oneIndexContribution;
      theCoeff.divideBy(negativeExponentDenominatorContribution, theCoeff, tempP1, &MonomialPolynomial::orderDefault());
      if (!tempP1.isEqualToZero()) {
        global.fatal << "This is a mathematical error! "
        << "Something is very wrong with embedding semisimple Lie algebras in Weyl algebras. "
        << global.fatal;
      }
    }
    for (int l = 0; l < theCoeff.size(); l ++) {
      if (!this->getActionEulerOperatorPart(theCoeff[l], eulerOperatorContribution)) {
        return false;
      }
      weylPartSummand = exponentContribution;
      weylPartSummand *= eulerOperatorContribution;
      weylPartSummand *= theCoeff.coefficients[l];
      for (int j = 0; j < weylPartSummand.size(); j ++) {
        for (int k = 0; k < endoPart.size(); k ++) {
          monQDO.theMatMon = endoPart[k];
          monQDO.theWeylMon = weylPartSummand[j];
          Polynomial<Rational>& currentEndoCoeff = endoPart.coefficients[k];
          for (int m = 0; m < currentEndoCoeff.size(); m ++) {
            monQDO2 = monQDO;
            monQDO2.theWeylMon.polynomialPart *= currentEndoCoeff[m];
            tempRat = currentEndoCoeff.coefficients[m];
            tempRat *= weylPartSummand.coefficients[j];
            output.addMonomial(monQDO2, tempRat);
          }
        }
      }
    }
  }
  return true;
}

template<class Coefficient>
void ModuleSSalgebra<Coefficient>::splitFDpartOverFKLeviRedSubalg(
  HomomorphismSemisimpleLieAlgebra& theHmm,
  Selection& LeviInSmall,
  List<ElementUniversalEnveloping<Coefficient> >* outputEigenVectors,
  Vectors<Coefficient>* outputWeightsFundCoords,
  Vectors<Coefficient>* outputEigenSpace,
  std::stringstream* comments
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<Coefficient>::splitFDpartOverFKLeviRedSubalg");
  if (this->theChaR.size() != 1) {
    if (comments != nullptr) {
      std::stringstream out;
      out << "I have been instructed only to split modules that are irreducible over the ambient Lie algebra";
      out << " Instead I got the character " << this->theChaR.toString()
      << " (" << this->theChaR.size() << " monomials)";
      *comments << out.str();
    }
    return;
  }
  std::string tempS;
  std::stringstream out;
  if (comments != nullptr) {
    out << tempS;
  }
  out << "<br>Parabolic selection: " << LeviInSmall.toString();
  std::stringstream tempStream1;
  tempStream1 << "Started splitting the f.d. part of the " << theHmm.theRange().toStringLieAlgebraName() << "-module with highest weight in fund coords "
  << this->theChaR[0].weightFundamentalCoordinates.toString();
  ProgressReport theReport;
  theReport.report(tempStream1.str());
  List<List<Vector<Coefficient> > > eigenSpacesPerSimpleGenerator;
  Selection InvertedLeviInSmall;
  InvertedLeviInSmall = LeviInSmall;
  InvertedLeviInSmall.invertSelection();
  eigenSpacesPerSimpleGenerator.setSize(InvertedLeviInSmall.cardinalitySelection);
  Vectors<Coefficient> tempSpace1, tempSpace2;
  MemorySaving<Vectors<Coefficient> > tempEigenVects;
  Vectors<Coefficient>& theFinalEigenSpace = (outputEigenSpace == nullptr) ? tempEigenVects.getElement() : *outputEigenSpace;
  theFinalEigenSpace.setSize(0);
  if (InvertedLeviInSmall.cardinalitySelection == 0) {
    theFinalEigenSpace.makeEiBasis(this->getDimension());
  }
  for (int i = 0; i < InvertedLeviInSmall.cardinalitySelection; i ++) {
    ElementSemisimpleLieAlgebra<Rational>& currentElt =
    theHmm.imagesSimpleChevalleyGenerators[InvertedLeviInSmall.elements[i]];
    MatrixTensor<Coefficient> currentOp, tempMat;
    currentOp.makeZero();
    for (int j = 0; j < currentElt.size(); j ++) {
      tempMat = this->getActionGeneratorIndex(currentElt[j].generatorIndex);
      tempMat *= currentElt.coefficients[j];
      currentOp += tempMat;
    }
    std::stringstream tempStream3;
    double timeAtStart1 = global.getElapsedSeconds();
    tempStream3 << "Computing eigenspace corresponding to " << currentElt.toString() << "...";
    theReport.report(tempStream3.str());
    Matrix<Coefficient> currentOpMat;
    currentOp.getMatrix(currentOpMat, this->getDimension());
    currentOpMat.getZeroEigenSpace(eigenSpacesPerSimpleGenerator[i]);
    tempStream3 << " done in " << global.getElapsedSeconds() - timeAtStart1 << " seconds. ";
    theReport.report(tempStream3.str());
    if (i == 0) {
      theFinalEigenSpace = eigenSpacesPerSimpleGenerator[i];
    } else {
      std::stringstream tempStream4;
      double timeAtStart2 = global.getElapsedSeconds();
      tempStream4 << "Intersecting with eigenspace corresponding to " << currentElt.toString() << "...";
      tempSpace1 = theFinalEigenSpace;
      theReport.report(tempStream4.str());
      tempSpace2 = eigenSpacesPerSimpleGenerator[i];
      theFinalEigenSpace.intersectTwoLinearSpaces(tempSpace1, tempSpace2, theFinalEigenSpace);
      tempStream4 << " done in " << global.getElapsedSeconds() - timeAtStart2 << " seconds. ";
      theReport.report(tempStream4.str());
    }
  }
  out << "<br>Eigenvectors: <table>";
  std::stringstream readyForLatexComsumption;
  readyForLatexComsumption << "\\begin{tabular}{|lll|}\n <br>";
  readyForLatexComsumption << "\\hline\\multicolumn{3}{|c|}{Highest weight $"
  << this->theHWFundamentalCoordsBaseField.toStringLetterFormat("\\omega") << "$}\\\\\n<br>";
  readyForLatexComsumption << "weight fund. coord.& singular vector \\\\\\hline\n<br>";
  Vector<Coefficient> currentWeight;
  Vector<Coefficient> hwFundCoordsNilPart;
  hwFundCoordsNilPart = this->theHWFundamentalCoordsBaseField;
  hwFundCoordsNilPart -= this->theHWFDpartFundamentalCoordS;
  ElementUniversalEnveloping<Coefficient> currentElt, tempElt;
  if (outputEigenVectors != nullptr) {
    outputEigenVectors->setSize(0);
  }
  for (int j = 0; j < theFinalEigenSpace.size; j ++) {
    out << "<tr><td>";
    currentElt.makeZero(this->getOwner());
    Vector<Coefficient>& currentVect = theFinalEigenSpace[j];
    int lastNonZeroIndex = - 1;
    for (int i = 0; i < currentVect.size; i ++) {
      if (!(currentVect[i].isEqualToZero())) {
        tempElt.makeZero(this->getOwner());
        tempElt.addMonomial(this->theGeneratingWordsNonReduced[i], 1);
        tempElt *= currentVect[i];
        currentElt += tempElt;
        lastNonZeroIndex = i;
      }
    }
    currentWeight = theHmm.theRange().weylGroup.getFundamentalCoordinatesFromSimple(
      this->theGeneratingWordsWeightsPlusWeightFDpart[lastNonZeroIndex]
    );//<-implicit type conversion here
    currentWeight += hwFundCoordsNilPart;
    readyForLatexComsumption <<  "$" << currentWeight.toStringLetterFormat("\\omega") << "$&$" << currentVect.toStringLetterFormat("m") << "$";
    if (currentElt.size() > 1) {
      out << "(";
    }
    if (outputEigenVectors != nullptr) {
      outputEigenVectors->addOnTop(currentElt);
    }
    if (outputWeightsFundCoords != nullptr) {
      outputWeightsFundCoords->addOnTop(currentWeight);
    }
    out << currentElt.toString(&global.theDefaultFormat.getElement());
    if (currentElt.size() > 1) {
      out << ")";
    }
    out << " v_\\lambda";
    out << "</td><td>(weight: " << currentWeight.toStringLetterFormat("\\omega") << ")</td></tr>";
    readyForLatexComsumption << "\\\\\n<br>";
  }
  out << "</table>";
  readyForLatexComsumption << "\\hline \n<br> \\end{tabular}";
  out << "<br>Your ready for LaTeX consumption text follows.<br>";
  out << readyForLatexComsumption.str();
  theReport.report("splitFDpartOverFKLeviRedSubalg done!");
  if (comments != nullptr) {
    *comments << out.str();
  }
}

template <class Coefficient>
bool ModuleSSalgebra<Coefficient>::getActionEulerOperatorPart(
  const MonomialPolynomial& theCoeff, ElementWeylAlgebra<Rational>& outputDO
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra::getActionEulerOperatorPart");
  int powerMonCoeff = 0;
  ElementWeylAlgebra<Rational> currentMonContribution;
  outputDO.makeOne();
  for (int i = 0; i < theCoeff.minimalNumberOfVariables(); i ++) {
    if (!theCoeff(i).isSmallInteger(&powerMonCoeff)) {
      global.fatal
      << "Getting Euler operator part of action on generalized Verma module: "
      << "I have an exponent with non-small integer entry. "
      << global.fatal;
    }
    currentMonContribution.makexidj(i, i, 0);
    currentMonContribution.raiseToPower(powerMonCoeff);
    outputDO *= currentMonContribution;
  }
  return true;
}

template <class Coefficient>
std::string MonomialTensorGeneralizedVermas<Coefficient>::toString(FormatExpressions* format, bool includeV) const {
  std::stringstream out;
  if (this->theMons.size > 1) {
    for (int i = 0; i < this->theMons.size; i ++) {
      bool ishwv = this->theMons[i].isHWV();
      if (!ishwv) {
        out << "(";
      }
      out << this->theMons[i].toString(format, includeV);
      if (!ishwv) {
        out  << ")";
      }
      if (i != this->theMons.size - 1) {
        out << "\\otimes ";
      }
    }
  } else {
    out << this->theMons[0].toString(format, includeV);
  }
  return out.str();
}

template <class Coefficient>
std::string MonomialGeneralizedVerma<Coefficient>::toString(FormatExpressions* format, bool includeV) const {
  if (this->owner == nullptr) {
    global.fatal << "Non-initialized generalized Verma monomial (owner is 0)." << global.fatal;
  }
  ModuleSSalgebra<Coefficient>& theMod = *this->owner;
  std::string tempS;
  if (tempS == "1") {
    tempS = "";
  }
  if (tempS == "- 1" || tempS == "-1") {
    tempS = "-";
  }
  tempS += this->theMonCoeffOne.toString(format);
  if (tempS == "1") {
    tempS = "";
  }
  if (tempS == "- 1" || tempS == "-1") {
    tempS = "-";
  }
  bool needsCdot = (tempS != "1" && tempS != "-" && tempS != "");
  std::stringstream out;
  out << tempS;
  tempS = theMod.theGeneratingWordsNonReduced[this->indexFDVector].toString(format);
  if (tempS != "1") {
    out << tempS;
  }
  needsCdot = needsCdot || (tempS != "1" && tempS != "-");
  if (needsCdot) {
    out << "\\cdot ";
  }
  if (includeV) {
    out << theMod.elementToStringHWV(format);
  }
  return out.str();
}

template <class Coefficient>
void MonomialGeneralizedVerma<Coefficient>::substitution(
  const PolynomialSubstitution<Rational>& theSub, ListReferences<ModuleSSalgebra<Coefficient> >& theMods
) {
  this->theMonCoeffOne.substitution(theSub);
  ModuleSSalgebra<Coefficient> newOwner;
  newOwner = *this->owner;
  newOwner.substitution(theSub);
  int newModIndex = theMods.addNoRepetitionOrReturnIndexFirst(newOwner);
  this->owner = &theMods[newModIndex];
}

template <class Coefficient>
void MonomialTensorGeneralizedVermas<Coefficient>::operator=(const MonomialGeneralizedVerma<Coefficient>& other) {
  this->theMons.setSize(1);
  this->theMons[0] = other;
}

template <class Coefficient>
void MonomialGeneralizedVerma<Coefficient>::multiplyMeByUEEltOnTheLeft(
  const ElementUniversalEnveloping<Coefficient>& theUE,
  ElementSumGeneralizedVermas<Coefficient>& output
) const {
  MacroRegisterFunctionWithName("MonomialGeneralizedVerma<Coefficient>::multiplyMeByUEEltOnTheLeft");
  MonomialGeneralizedVerma<Coefficient> currentMon;
  output.makeZero();
  ElementSumGeneralizedVermas<Coefficient> buffer;
  ProgressReport theReport;
  if (!this->getOwner().owner->flagHasNilradicalOrder) {
    global.fatal << "Calling generalized verma module simplification requires nilradical order on the generators. "
    << global.fatal;
  }
  for (int j = 0; j < theUE.size(); j ++) {
    currentMon.theMonCoeffOne = theUE[j];
    currentMon.theMonCoeffOne *= this->theMonCoeffOne;
    currentMon.owner = this->owner;
    currentMon.indexFDVector = this->indexFDVector;
    currentMon.owner = this->owner;
    std::stringstream reportStream;
    reportStream << "reducing mon: " << currentMon.toString()
    << ", index" << j + 1 << " out of " << theUE.size() << "...";
    theReport.report(reportStream.str());
    currentMon.reduceMe(buffer);
    reportStream << " done.";
    theReport.report(reportStream.str());
    buffer *= theUE.coefficients[j];
    output += buffer;
  }
}

template <class Coefficient>
void ElementSumGeneralizedVermas<Coefficient>::multiplyMeByUEEltOnTheLeft(
  const ElementUniversalEnveloping<Coefficient>& theUE
) {
  MacroRegisterFunctionWithName("ElementSumGeneralizedVermas<Coefficient>::multiplyMeByUEEltOnTheLeft");
  ElementSumGeneralizedVermas<Coefficient> buffer, Accum;
  Accum.makeZero();
  for (int i = 0; i < this->size(); i ++) {
    (*this)[i].multiplyMeByUEEltOnTheLeft(theUE, buffer);
    buffer *= this->coefficients[i];
    Accum += buffer;
  }
  *this = Accum;
}

template <class Coefficient>
void MonomialGeneralizedVerma<Coefficient>::reduceMe(
  ElementSumGeneralizedVermas<Coefficient>& output
) const {
  MacroRegisterFunctionWithName("MonomialGeneralizedVerma::reduceMe");
  ModuleSSalgebra<Coefficient>& theMod = *this->owner;
  output.makeZero();
  MonomialUniversalEnveloping<Coefficient> tempMon;
  tempMon = this->theMonCoeffOne;
  tempMon *= theMod.theGeneratingWordsNonReduced[this->indexFDVector];
  int indexCheck = theMod.theGeneratingWordsNonReduced.getIndex(tempMon);
  if (!this->owner->owner->flagHasNilradicalOrder) {
    global.fatal << "Owner needs nilradical order!!!" << global.fatal;
  }
  if (indexCheck != - 1) {
    MonomialGeneralizedVerma<Coefficient> basisMon;
    basisMon.makeConstant(*this->owner);
    basisMon.indexFDVector = indexCheck;
    output.addMonomial(basisMon, 1);
    return;
  }
  ElementUniversalEnveloping<Coefficient> theUEelt;
  theUEelt.makeZero(*this->getOwner().owner);
  theUEelt.addMonomial(this->theMonCoeffOne, 1);
  theUEelt.simplify();

  MonomialUniversalEnveloping<Coefficient> currentMon;
  MonomialGeneralizedVerma<Coefficient> newMon;
  MatrixTensor<Coefficient> tempMat1, tempMat2;
  ProgressReport theReport;
  Coefficient theCF;
  for (int l = 0; l < theUEelt.size(); l ++) {
    currentMon = theUEelt[l];
    tempMat1.makeIdentitySpecial();
    for (int k = currentMon.powers.size - 1; k >= 0; k --) {
      std::stringstream reportStream;
      reportStream << "accounting monomial " << currentMon.toString() << " of index "
      << l + 1 << " out of "
      << theUEelt.size() << " and letter index " << currentMon.powers.size - k
      << " out of " << currentMon.powers.size << "...";
      theReport.report(reportStream.str());
      int thePower = - 1;
      if (!currentMon.powers[k].isSmallInteger(&thePower)) {
        break;
      }
      int theIndex = currentMon.generatorsIndices[k];
      if (theMod.hasFreeAction(theIndex)) {
        break;
      }
      tempMat2 = tempMat1;
      tempMat1 = theMod.getActionGeneratorIndex(theIndex);
      tempMat1.raiseToPower(thePower);
      tempMat1 *= tempMat2;
      currentMon.powers.size --;
      currentMon.generatorsIndices.size --;
      reportStream << "done!";
      theReport.report(reportStream.str());
    }
    newMon.owner = this->owner;
    for (int i = 0; i < tempMat1.size(); i ++) {
      int otherIndex = - 1;
      if (tempMat1[i].dualIndex == this->indexFDVector) {
        otherIndex = tempMat1[i].vIndex;
      }
      if (tempMat1[i].isIdentity) {
        otherIndex = this->indexFDVector;
      }
      if (otherIndex != - 1) {
        newMon.theMonCoeffOne = currentMon;
        newMon.indexFDVector = otherIndex;
        theCF = theUEelt.coefficients[l];
        theCF *= tempMat1.coefficients[i];
        output.addMonomial(newMon, theCF);
      }
    }
  }
  theMod.getOwner().orderStandardAscending();
}

template <class Coefficient>
void ElementSumGeneralizedVermas<Coefficient>::makeHWV(
  ModuleSSalgebra<Coefficient>& theOwner, const Coefficient& ringUnit
) {
  this->makeZero();
  MonomialGeneralizedVerma<Coefficient> theMon;
  theMon.indexFDVector = theOwner.theGeneratingWordsNonReduced.size - 1;
  theMon.theMonCoeffOne.makeOne(theOwner.getOwner());
  theMon.owner = &theOwner;
  this->addMonomial(theMon, ringUnit);
}

template<class Coefficient>
bool ElementSumGeneralizedVermas<Coefficient>::extractElementUniversalEnveloping(
  ElementUniversalEnveloping<Coefficient>& output, SemisimpleLieAlgebra& theOwner
) {
  output.makeZero(theOwner);
  ModuleSSalgebra<Coefficient>* theModPtr = nullptr;
  MonomialUniversalEnveloping<Coefficient> tempMon;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialGeneralizedVerma<Coefficient>& currentMon = (*this)[i];
    if (i == 0) {
      theModPtr = currentMon.owner;
    }
    if (currentMon.owner != theModPtr) {
      return false;
    }
    tempMon = currentMon.theMonCoeffOne;
    tempMon *= currentMon.getOwner().theGeneratingWordsNonReduced[currentMon.indexFDVector];
    output.addMonomial(tempMon, this->coefficients[i]);
  }
  return true;
}
#endif
