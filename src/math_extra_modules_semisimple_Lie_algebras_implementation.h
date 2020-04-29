// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader_ImplementationHeaderModulesSSLielgebras
#define vpfHeader_ImplementationHeaderModulesSSLielgebras
#include "math_extra_modules_semisimple_Lie_algebras.h"
#include "math_extra_drawing_variables.h"

template<class Coefficient>
Rational ModuleSSalgebra<Coefficient>::hwTrace(
  const Pair<MonomialTensor<int, MathRoutines::IntUnsignIdentity>,
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> >& thePair,
  ProgressReport* theProgressReport
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<Coefficient>::hwTrace");
  int indexInCache = this->cachedPairs.getIndex(thePair);
  if (indexInCache != - 1) {
    return this->cachedTraces[indexInCache];
  }
  if (thePair.Object1.generatorsIndices.size == 0) {
    return 1;
  }
  Pair<MonomialTensor<int, MathRoutines::IntUnsignIdentity>,
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> > newPair;
  MonomialTensor<int, MathRoutines::IntUnsignIdentity>& newLeft = newPair.Object1;
  MonomialTensor<int, MathRoutines::IntUnsignIdentity>& newRight = newPair.Object2;
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& oldRight = thePair.Object2;
  newLeft = thePair.Object1;
  (*newLeft.Powers.lastObject()) -= 1;
  int theIndex = *newLeft.generatorsIndices.lastObject();
  if (*newLeft.Powers.lastObject() == 0) {
    newLeft.generatorsIndices.size --;
    newLeft.Powers.size --;
  }
  int theIndexMinus = 2 * this->GetOwner().GetNumPosRoots() + this->GetOwner().getRank() - theIndex - 1;
  int theSimpleIndex = theIndex - this->GetOwner().GetNumPosRoots() - this->GetOwner().getRank();
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> Accum;
  Accum.Powers.reserve(oldRight.Powers.size);
  Accum.generatorsIndices.reserve(oldRight.generatorsIndices.size);
  Vector<Rational> RemainingWeight;
  Rational result = 0;
  Rational summand;
  WeylGroupData& theWeyl = this->GetOwner().theWeyl;
  for (int i = 0; i < oldRight.generatorsIndices.size; i ++) {
    if (oldRight.generatorsIndices[i] == theIndexMinus) {
      summand = 0;
      newRight = Accum;
      newRight.MultiplyByGeneratorPowerOnTheRight(oldRight.generatorsIndices[i], oldRight.Powers[i] - 1);
      RemainingWeight.makeZero(theWeyl.getDimension());
      for (int j = i + 1; j < oldRight.generatorsIndices.size; j ++) {
        newRight.MultiplyByGeneratorPowerOnTheRight(oldRight.generatorsIndices[j], oldRight.Powers[j]);
        RemainingWeight += theWeyl.RootSystem[oldRight.generatorsIndices[j]] * oldRight.Powers[j];
      }
      RemainingWeight += this->theHWFDpartSimpleCoordS;
      summand += theWeyl.GetScalarProdSimpleRoot(RemainingWeight, theSimpleIndex);
      summand *= 2;
      summand /= theWeyl.cartanSymmetric.elements[theSimpleIndex][theSimpleIndex];
      summand += 1;
      summand -= oldRight.Powers[i];
      if (!summand.isEqualToZero()) {
        summand *= this->hwTrace(
          newPair, theProgressReport
        );
      }
      summand *= oldRight.Powers[i];
      result += summand;
    }
    Accum.generatorsIndices.addOnTop(oldRight.generatorsIndices[i]);
    Accum.Powers.addOnTop(oldRight.Powers[i]);
  }
  if (this->cachedPairs.size < this->MaxNumCachedPairs) {
    this->cachedPairs.addOnTop(thePair);
    this->cachedTraces.addOnTop(result);
  }
  if (theProgressReport != nullptr && this->cachedPairs.size < 500000) {
    std::stringstream tempStream;
    tempStream << "Number of cached pairs: " << this->cachedPairs.size
    << " at recursion depth " << global.CustomStackTrace.size;
    theProgressReport->report(tempStream.str());
  }
  return result;
}

template<class Coefficient>
void ModuleSSalgebra<Coefficient>::ApplyTAA(MonomialTensor<int, MathRoutines::IntUnsignIdentity>& theMon) {
  for (int i = 0; i < theMon.generatorsIndices.size; i ++) {
    theMon.generatorsIndices[i] =
    this->GetOwner().GetNumPosRoots() * 2 + this->GetOwner().getRank() - theMon.generatorsIndices[i] - 1;
  }
  theMon.Powers.reverseElements();
  theMon.generatorsIndices.reverseElements();
}

template<class Coefficient>
Rational ModuleSSalgebra<Coefficient>::hwtaabfSimpleGensOnly(
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& leftMon,
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& rightMon,
  ProgressReport* theProgressReport
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<Coefficient>::hwtaabfSimpleGensOnly");
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>* left = &leftMon;
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>* right = &rightMon;
  if (leftMon > rightMon) {
    MathRoutines::swap(left, right);
  }
  Pair<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, MonomialTensor<int, MathRoutines::IntUnsignIdentity> > thePair;
  thePair.Object1 = *left;
  thePair.Object2 = *right;
  this->ApplyTAA(thePair.Object1);
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
    this->theHWDualCoordsBaseFielD[i].substitution(theSub);
    this->theHWFundamentalCoordsBaseField[i].substitution(theSub);
    this->theHWSimpleCoordSBaseField[i].substitution(theSub);
  }
}

template <class Coefficient>
MatrixTensor<Coefficient>& ModuleSSalgebra<Coefficient>::GetActionGeneratorIndeX(int generatorIndex) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<Coefficient>::GetActionGeneratorIndeX");
  int numGenerators = this->GetOwner().GetNumGenerators();
  if (generatorIndex < 0 || generatorIndex >= numGenerators) {
    global.fatal << "Bad generator index: " << generatorIndex << ". " << global.fatal;
  }
  if (this->ComputedGeneratorActions.selected[generatorIndex]) {
    return this->actionsGeneratorsMaT[generatorIndex];
  }
  this->ComputedGeneratorActions.AddSelectionAppendNewIndex(generatorIndex);
  if (this->HasFreeAction(generatorIndex)) {
    global.fatal << "This is a programming error, due to a change in "
    << "implementation of the generalized Verma module class. " << global.fatal;
    this->actionsGeneratorsMaT[generatorIndex].makeZero();
    return this->actionsGeneratorsMaT[generatorIndex];
  }
  if (this->HasZeroActionFDpart(generatorIndex)) {
    this->actionsGeneratorsMaT[generatorIndex].makeZero();
    return this->actionsGeneratorsMaT[generatorIndex];
  }
  if (this->GetOwner().IsASimpleGenerator(generatorIndex)) {
    return this->GetActionSimpleGeneratorIndex(generatorIndex);
  }
  MatrixTensor<Coefficient>& output = this->actionsGeneratorsMaT[generatorIndex];
  if (this->GetOwner().IsGeneratorFromCartan(generatorIndex)) {
    output.makeZero();
    MonomialMatrix theMon;
    Vector<Coefficient> weightH;
    Coefficient tempCF, hwCFshift;
    weightH.makeEi(this->GetOwner().getRank(), generatorIndex - this->GetOwner().GetNumPosRoots());
    hwCFshift = this->GetOwner().theWeyl.RootScalarCartanRoot(weightH, this->theHWSimpleCoordSBaseField);
    hwCFshift -= this->GetOwner().theWeyl.RootScalarCartanRoot(weightH, this->theHWFDpartSimpleCoordS);
    for (int i = 0; i < this->theGeneratingWordsNonReduced.size; i ++) {
      Vector<Rational>& theWeight = this->theGeneratingWordsWeightsPlusWeightFDpart[i];
      tempCF = this->GetOwner().theWeyl.RootScalarCartanRoot(weightH, theWeight);
      tempCF += hwCFshift;
      theMon.IsId = false;
      theMon.vIndex = i;
      theMon.dualIndex = i;
      output.addMonomial(theMon, tempCF);
    }
    return output;
  }
  List<int> adActions;
  Rational theCoeff;
  this->GetOwner().GetChevalleyGeneratorAsLieBracketsSimpleGens(generatorIndex, adActions, theCoeff);
  MatrixTensor<Coefficient> tempO;
  output = this->GetActionGeneratorIndeX(*adActions.lastObject());
  for (int i = adActions.size - 2; i >= 0; i --) {
    tempO = this->GetActionGeneratorIndeX(adActions[i]);
    output.LieBracketOnTheLeft(tempO);
  }
  Coefficient tempCF;
  tempCF = theCoeff;
  output *= tempCF;
  return output;
}

template  <class Coefficient>
void ModuleSSalgebra<Coefficient>::GetMatrixHomogenousElt(
  ElementUniversalEnveloping<Coefficient>& inputHomogeneous,
  List<List<ElementUniversalEnveloping<Coefficient> > >& outputSortedByArgumentWeight,
  Vector<Rational>& weightUEEltSimpleCoords,
  MatrixTensor<Coefficient>& output,
  const Coefficient& theRingUnit,
  const Coefficient& theRingZero
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra::GetMatrixHomogenousElt");
  this->GetAdActionHomogenousElT(
    inputHomogeneous, weightUEEltSimpleCoords, outputSortedByArgumentWeight, theRingUnit, theRingZero
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
bool CharacterSemisimpleLieAlgebraModule<Coefficient>::SplitOverLeviMonsEncodeHIGHESTWeight(
  std::string* report,
  CharacterSemisimpleLieAlgebraModule& output,
  const Selection& splittingParSel,
  const Selection& ParSelFDInducingPart,
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& outputWeylSub
) {
  MacroRegisterFunctionWithName("CharacterSemisimpleLieAlgebraModule<Coefficient>::SplitOverLeviMonsEncodeHIGHESTWeight");
  this->CheckNonZeroOwner();
  std::stringstream out;
  std::string tempS;
  if (this->GetOwner()->getRank() != splittingParSel.MaxSize) {
    global.fatal << "This is a programming error: parabolic selection selects out of " << splittingParSel.MaxSize
    << " elements while the weyl group is of rank " << this->GetOwner()->getRank() << ". " << global.fatal;
  }
  outputWeylSub.MakeParabolicFromSelectionSimpleRoots(this->GetOwner()->theWeyl, splittingParSel, 1);
  outputWeylSub.ComputeRootSubsystem();

  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms complementGroup, theFDWeyl;
  complementGroup.AmbientWeyl = outputWeylSub.AmbientWeyl;
  theFDWeyl.AmbientWeyl = outputWeylSub.AmbientWeyl;

  Selection invertedSel;
  invertedSel = splittingParSel;
  invertedSel.InvertSelection();
  complementGroup.MakeParabolicFromSelectionSimpleRoots(this->GetOwner()->theWeyl, invertedSel, 1);
  complementGroup.ComputeRootSubsystem();
  out << outputWeylSub.toString(false);
  CharacterSemisimpleLieAlgebraModule charAmbientFDWeyl, remainingCharDominantLevi;
  theFDWeyl.MakeParabolicFromSelectionSimpleRoots(this->GetOwner()->theWeyl, ParSelFDInducingPart, 1);
  Weight<Coefficient> tempMon, localHighest;
  List<Coefficient> tempMults;
  HashedList<Vector<Coefficient> > tempHashedRoots;
  WeylGroupData& theWeyL = this->GetOwner()->theWeyl;
  charAmbientFDWeyl.makeZero();
  Coefficient bufferCoeff, highestCoeff;
  out << "Starting character: " << this->toString();
  tempMon.owner = this->GetOwner();
  for (int i = 0; i < this->size(); i ++) {
    const Weight<Coefficient>& currentMon = (*this)[i];
    if (!theFDWeyl.FreudenthalEvalIrrepIsWRTLeviPart(
      currentMon.weightFundamentalCoordS, tempHashedRoots, tempMults, tempS, 10000
    )) {
      if (report != nullptr) {
        *report = tempS;
      }
      return false;
    }
    for (int j = 0; j < tempHashedRoots.size; j ++) {
      bufferCoeff = this->coefficients[i];
      tempMon.weightFundamentalCoordS = theWeyL.GetFundamentalCoordinatesFromSimple(tempHashedRoots[j]);
      bufferCoeff *= tempMults[j];
      charAmbientFDWeyl.addMonomial(tempMon, bufferCoeff);
    }
  }

  remainingCharDominantLevi.makeZero();
  Vectors<Coefficient> orbitDom;
  tempMon.owner = this->GetOwner();
  for (int i = 0; i < charAmbientFDWeyl.size(); i ++) {
    orbitDom.setSize(0);
    if (!theFDWeyl.GenerateOrbitReturnFalseIfTruncated(
      theWeyL.GetSimpleCoordinatesFromFundamental(charAmbientFDWeyl[i].weightFundamentalCoordS), orbitDom, false, 10000
    )) {
      out << "failed to generate the complement-sub-Weyl-orbit of weight "
      << this->GetOwner()->theWeyl.GetSimpleCoordinatesFromFundamental(charAmbientFDWeyl[i].weightFundamentalCoordS).toString();
      if (report != nullptr) {
        *report = out.str();
      }
      return false;
    }
    for (int k = 0; k < orbitDom.size; k ++) {
      if (outputWeylSub.IsDominantWeight(orbitDom[k])) {
        tempMon.weightFundamentalCoordS = theWeyL.GetFundamentalCoordinatesFromSimple(orbitDom[k]);
        remainingCharDominantLevi.addMonomial(tempMon, charAmbientFDWeyl.coefficients[i]);
      }
    }
  }
  output.makeZero();
  out << "<br>Character w.r.t Levi part: " << HtmlRoutines::GetMathMouseHover(remainingCharDominantLevi.toString());
  Vector<Coefficient> simpleGeneratorBaseField;
  while (!remainingCharDominantLevi.isEqualToZero()) {
    localHighest = *remainingCharDominantLevi.theMonomials.lastObject();
    for (bool Found = true; Found; ) {
      Found = false;
      for (int i = 0; i < outputWeylSub.simpleRootsInner.size; i ++) {
        tempMon = localHighest;
        simpleGeneratorBaseField = outputWeylSub.simpleRootsInner[i]; // <- implicit type conversion here!
        tempMon.weightFundamentalCoordS += this->GetOwner()->theWeyl.GetFundamentalCoordinatesFromSimple(simpleGeneratorBaseField);
        if (remainingCharDominantLevi.theMonomials.contains(tempMon)) {
          localHighest = tempMon;
          Found = true;
        }
      }
    }
    highestCoeff = remainingCharDominantLevi.coefficients[remainingCharDominantLevi.theMonomials.getIndex(localHighest)];
    output.addMonomial(localHighest, highestCoeff);
    if (!outputWeylSub.FreudenthalEvalIrrepIsWRTLeviPart(
      localHighest.weightFundamentalCoordS, tempHashedRoots, tempMults, tempS, 10000
    )) {
      if (report != nullptr) {
        *report = tempS;
      }
      return false;
    }
    for (int i = 0; i < tempHashedRoots.size; i ++) {
      tempMon.owner = this->GetOwner();
      tempMon.weightFundamentalCoordS = theWeyL.GetFundamentalCoordinatesFromSimple(tempHashedRoots[i]);
      bufferCoeff = tempMults[i];
      bufferCoeff *= highestCoeff;
      remainingCharDominantLevi.SubtractMonomial(tempMon, bufferCoeff);
    }
  }
  out << "<br>Character w.r.t Levi part: " << HtmlRoutines::GetMathMouseHover(output.toString());
  if (report != nullptr) {
    DrawingVariables theDV;
    std::string tempS;
    this->DrawMeNoMults(tempS, theDV, 10000);
    Vector<Rational> tempRoot;
    out << "<hr>In the following weight visualization, "
    << "a yellow line is drawn if the corresponding weights are "
    << "simple reflections of one another, "
    << "with respect to a simple root of the Levi part of the parabolic subalgebra. ";
    for (int i = 0; i < output.size(); i ++) {
      tempRoot = theWeyL.GetSimpleCoordinatesFromFundamental(output[i].weightFundamentalCoordS).GetVectorRational();
      outputWeylSub.DrawContour(tempRoot, theDV, "#a0a000", 1000);
      std::stringstream tempStream;
      tempStream << output.coefficients[i].toString();
      theDV.drawTextAtVectorBufferRational(tempRoot, tempStream.str(), "black");
    }
    out << "<hr>" << theDV.GetHtmlDiv(theWeyL.getDimension());
    *report = out.str();
  }
  return true;
}

template<class Coefficient>
void ModuleSSalgebra<Coefficient>::SplitOverLevi(
  std::string* report,
  Selection& splittingParSel,
  List<ElementUniversalEnveloping<Coefficient> >* outputEigenVectors,
  Vectors<Coefficient>* outputWeightsFundCoords,
  Vectors<Coefficient>* outputEigenSpace,
  CharacterSemisimpleLieAlgebraModule<Coefficient>* outputChar
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<Coefficient>::SplitOverLevi");
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
  if (this->GetOwner().getRank() != splittingParSel.MaxSize) {
    global.fatal << "This is a programming error: semisimple rank is "
    << this->GetOwner().getRank() << " but splitting parabolic selects "
    << " out of " << splittingParSel.MaxSize << " simple roots. " << global.fatal;
  }
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms subWeyl;
  subWeyl.AmbientWeyl = &this->owner->theWeyl;
  MemorySaving<CharacterSemisimpleLieAlgebraModule<Coefficient> > buffer;
  CharacterSemisimpleLieAlgebraModule<Coefficient>& charWRTsubalgebra = (outputChar == 0) ? buffer.getElement() : *outputChar;
  this->theChaR.SplitOverLeviMonsEncodeHIGHESTWeight(
    report, charWRTsubalgebra, splittingParSel, this->parabolicSelectionNonSelectedAreElementsLevi, subWeyl
  );
  std::stringstream out;
  if (report != nullptr) {
    out << *report;
  }
  Selection splittingParSelectedInLevi;
  splittingParSelectedInLevi = splittingParSel;
  splittingParSelectedInLevi.InvertSelection();
  if (!splittingParSelectedInLevi.IsSubset(this->parabolicSelectionSelectedAreElementsLevi)) {
    out << "The parabolic subalgebra you selected is not a subalgebra of the ambient parabolic subalgebra."
    << " The parabolic has root of Levi given by " << splittingParSel.toString()
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
  eigenSpacesPerSimpleGenerator.setSize(splittingParSelectedInLevi.CardinalitySelection);
  Vectors<Coefficient> tempSpace1, tempSpace2;
  MemorySaving<Vectors<Coefficient> > tempEigenVects;
  Vectors<Coefficient>& theFinalEigenSpace = (outputEigenSpace == 0) ? tempEigenVects.getElement() : *outputEigenSpace;
  //WeylGroup& theWeyL = this->theAlgebra.theWeyl;
  theFinalEigenSpace.MakeEiBasis(this->getDimension());
  for (int i = 0; i < splittingParSelectedInLevi.CardinalitySelection; i ++) {
    int theGenIndex = splittingParSelectedInLevi.elements[i] +
    this->GetOwner().getRank() + this->GetOwner().GetNumPosRoots();
    MatrixTensor<Coefficient>& currentOp = this->GetActionGeneratorIndeX(theGenIndex);
    Matrix<Coefficient> currentOpMat;
    currentOp.GetMatrix(currentOpMat, this->getDimension());
    currentOpMat.GetZeroEigenSpaceModifyMe(eigenSpacesPerSimpleGenerator[i]);
    tempSpace1 = theFinalEigenSpace;
    tempSpace2 = eigenSpacesPerSimpleGenerator[i];
    theFinalEigenSpace.IntersectTwoLinSpaces(tempSpace1, tempSpace2, theFinalEigenSpace);
  }
  out << "<br>Eigenvectors:<table> ";
  std::stringstream readyForLatexComsumption;
  readyForLatexComsumption << "\\begin{tabular}{|lll|}\n <br>";
  readyForLatexComsumption << "\\hline\\multicolumn{3}{|c|}{Highest weight $"
  << this->theHWFundamentalCoordsBaseField.ToStringLetterFormat("\\omega") << "$}\\\\\n<br>";
  readyForLatexComsumption << "weight fund. coord.& singular vector \\\\\\hline\n<br>";
  Vector<Coefficient> currentWeight;
  Vector<Coefficient> hwFundCoordsNilPart;
  hwFundCoordsNilPart = this->theHWFundamentalCoordsBaseField;
  hwFundCoordsNilPart -= this->theHWFDpartFundamentalCoordS;
  ElementUniversalEnveloping<Coefficient> currentElt, tempElt;
  for (int j = 0; j < theFinalEigenSpace.size; j ++) {
    out << "<tr><td>";
    currentElt.makeZero(this->GetOwner());
    Vector<Coefficient>& currentVect = theFinalEigenSpace[j];
    int lastNonZeroIndex = - 1;
    for (int i = 0; i < currentVect.size; i ++) {
      if (!(currentVect[i].isEqualToZero())) {
        tempElt.makeZero(this->GetOwner());
        tempElt.addMonomial(this->theGeneratingWordsNonReduced[i], 1);
        tempElt *= currentVect[i];
        currentElt += tempElt;
        lastNonZeroIndex = i;
      }
    }
    currentWeight = subWeyl.AmbientWeyl->GetFundamentalCoordinatesFromSimple(
      this->theGeneratingWordsWeightsPlusWeightFDpart[lastNonZeroIndex]
    );//<-implicitTypeConversionHere
    currentWeight += hwFundCoordsNilPart;
    readyForLatexComsumption <<  "$" << currentWeight.ToStringLetterFormat("\\omega")
    << "$&$" << currentVect.ToStringLetterFormat("m") << "$";
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
    out << "</td><td>(weight: " << currentWeight.ToStringLetterFormat("\\omega") << ")</td></tr>";
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
MatrixTensor<Coefficient>& ModuleSSalgebra<Coefficient>::GetActionSimpleGeneratorIndex(int generatorIndex) {
  Vector<Rational> genWeight = this->GetOwner().GetWeightOfGenerator(generatorIndex);
  Vector<Rational> targetWeight;
  Pair<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, MonomialTensor<int, MathRoutines::IntUnsignIdentity> > currentPair;
  MatrixTensor<Coefficient>& outputMat = this->actionsGeneratorsMaT[generatorIndex];
  Vector<Coefficient> theScalarProds;
  outputMat.makeZero();
  for (int i = 0; i < this->theGeneratingWordsGrouppedByWeight.size; i ++) {
    List<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >& currentWordList =
    this->theGeneratingWordsIntGrouppedByWeight[i];
    const Vector<Rational>& currentWeight = this->theModuleWeightsSimpleCoords[i];
    targetWeight = currentWeight + genWeight;
    int weightLevelIndex = this->theModuleWeightsSimpleCoords.getIndex(targetWeight);
    if (weightLevelIndex != - 1) {
      int columnOffset = this->GetOffsetFromWeightIndex(i);
      int rowOffset = this->GetOffsetFromWeightIndex(weightLevelIndex);
      List<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >&
      otherWordList = this->theGeneratingWordsIntGrouppedByWeight[weightLevelIndex];
      for (int j = 0; j < currentWordList.size; j ++) {
        theScalarProds.setSize(otherWordList.size);
        for (int k = 0; k < otherWordList.size; k ++) {
          if (generatorIndex > this->GetOwner().GetNumPosRoots()) {
            currentPair.Object1 = currentWordList[j];
            this->ApplyTAA(currentPair.Object1);
            currentPair.Object2 = otherWordList[k];
            currentPair.Object2.MultiplyByGeneratorPowerOnTheLeft(this->GetOwner().GetOppositeGeneratorIndex(generatorIndex), 1);
          } else {
            currentPair.Object1 = currentWordList[j];
            currentPair.Object1.MultiplyByGeneratorPowerOnTheLeft(generatorIndex, 1);
            this->ApplyTAA(currentPair.Object1);
            currentPair.Object2 = otherWordList[k];
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
bool ModuleSSalgebra<Coefficient>::MakeFromHW(
  SemisimpleLieAlgebra& inputAlgebra,
  Vector<Coefficient>& HWFundCoords,
  const Selection& selNonSelectedAreElementsLevi,
  const Coefficient& theRingUnit,
  const Coefficient& theRingZero,
  std::string* outputReport,
  bool computeSimpleGens
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<Coefficient>::MakeFromHW");
  ProgressReport theReport;
  this->owner = &inputAlgebra;
  SemisimpleLieAlgebra& theAlgebrA = inputAlgebra;

  int theRank = theAlgebrA.getRank();
  if (HWFundCoords.size != theRank || selNonSelectedAreElementsLevi.MaxSize != theRank) {
    global.fatal << "This is a programming error. I am asked to create a "
    << "generalized Verma module with a semisimple Lie algebra of rank "
    << theRank << " but the input highest weight, "
    << HWFundCoords.toString() << ", has " << HWFundCoords.size << " coordinates and "
    << " the parabolic section indicates rank of "
    << selNonSelectedAreElementsLevi.MaxSize << ". " << global.fatal;
  }
  WeylGroupData& theWeyl = theAlgebrA.theWeyl;
  this->cachedPairs.clear();
  this->cachedTraces.setSize(0);

  this->parabolicSelectionNonSelectedAreElementsLevi = selNonSelectedAreElementsLevi;
  this->parabolicSelectionSelectedAreElementsLevi = this->parabolicSelectionNonSelectedAreElementsLevi;
  this->parabolicSelectionSelectedAreElementsLevi.InvertSelection();

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

  this->theHWFDpartSimpleCoordS = theWeyl.GetSimpleCoordinatesFromFundamental(this->theHWFDpartFundamentalCoordS);
  this->theHWFDpartDualCoords = theWeyl.GetDualCoordinatesFromFundamental(this->theHWFDpartFundamentalCoordS);
  this->theHWSimpleCoordSBaseField = theWeyl.GetSimpleCoordinatesFromFundamental(this->theHWFundamentalCoordsBaseField);
  this->theChaR.MakeFromWeight(this->theHWSimpleCoordSBaseField, this->owner);

  unsigned long long startingNumRationalOperations =
  Rational::TotalLargeAdditions +
  Rational::TotalSmallAdditions +
  Rational::TotalLargeMultiplications +
  Rational::TotalSmallMultiplications;
  LittelmannPath startingPath;
  startingPath.MakeFromWeightInSimpleCoords(this->theHWFDpartSimpleCoordS, theWeyl);
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
    this->theModuleWeightsSimpleCoords.addOnTopNoRepetition(*this->thePaths[i].Waypoints.lastObject());
    tempCharMon.weightFundamentalCoordS = theWeyl.GetFundamentalCoordinatesFromSimple(*this->thePaths[i].Waypoints.lastObject());
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
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> tempMonInt;
  for (int i = 0; i < this->thePaths.size; i ++) {
    List<int>& currentPath = generatorsIndices[i];
    currentNonReducedElement.makeOne(this->GetOwner());
    tempMonInt.makeConstant();
    for (int j = currentPath.size - 1; j >= 0; j --) {
      int theIndex = currentPath[j];
      if (theIndex > 0) {
        theIndex ++;
      }
      currentNonReducedElement.MultiplyByGeneratorPowerOnTheRight(
        this->GetOwner().GetGeneratorFromDisplayIndex(theIndex), theRingUnit
      );
      tempMonInt.MultiplyByGeneratorPowerOnTheRight(this->GetOwner().GetGeneratorFromDisplayIndex(theIndex), 1);
    }
    Vector<Rational>& hwCurrent = *this->thePaths[i].Waypoints.lastObject();
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
    List<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >& currentListInt = this->theGeneratingWordsIntGrouppedByWeight[i];
    currentList.quickSortDescending();
    currentListInt.quickSortDescending();
    for (int j = 0; j < currentList.size; j ++) {
      //wordCounter ++;
      this->theGeneratingWordsNonReduced.addOnTop(currentList[j]);
      this->theGeneratingWordsNonReducedInt.addOnTop(currentListInt[j]);
      this->theGeneratingWordsWeightsPlusWeightFDpart.addOnTop(this->theModuleWeightsSimpleCoords[i]);
    }
  }
  this->IntermediateStepForMakeFromHW(theRingUnit, theRingZero);
  this->NumCachedPairsBeforeSimpleGen = this->cachedPairs.size;
  this->NumRationalMultiplicationsAndAdditionsBeforeSimpleGen =
  Rational::TotalLargeAdditions + Rational::TotalSmallAdditions +
  Rational::TotalLargeMultiplications + Rational::TotalSmallMultiplications - startingNumRationalOperations;
  bool isBad = false;
  for (int k = 0; k < this->theBilinearFormsAtEachWeightLevel.size; k ++) {
    Matrix<Coefficient>& theBF = this->theBilinearFormsAtEachWeightLevel[k];
    Matrix<Coefficient>& theBFinverted = this->theBilinearFormsInverted[k];
    if (!theBF.IsNonNegativeAllEntries()) {
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
      for (int j = 0; j < this->GetOwner().getRank(); j ++) {
        if (this->parabolicSelectionSelectedAreElementsLevi.selected[j]) {
          int theIndex = this->GetOwner().GetNumPosRoots() - j - 1;
          if (k == 1) {
            theIndex = this->GetOwner().GetNumPosRoots() + this->GetOwner().getRank() + j;
          }
          tempSSElt.MakeGenerator(theIndex, this->GetOwner());
          if (outputReport != nullptr) {
            out2 << "<hr>Simple generator: " << tempSSElt.toString(&global.theDefaultFormat.getElement());
          }
          MatrixTensor<Coefficient>& theMatrix = this->GetActionGeneratorIndeX(theIndex);
          std::stringstream tempStream;
          tempStream << "computing action simple generator index " << (2 * k - 1) * (j + 1) << " ... ";
          theReport.report(tempStream.str());
          tempStream << " done!";
          theReport.report(tempStream.str());
          if (outputReport != nullptr) {
            if (this->getDimension() < 50) {
              out2 << "<br>Matrix of elemenent in the m_i basis:<br>" << HtmlRoutines::GetMathMouseHover(theMatrix.toString(), 5000);
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
            this->GetActionGeneratorIndeX(this->GetOwner().GetNumPosRoots() + j);
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
void ModuleSSalgebra<Coefficient>::IntermediateStepForMakeFromHW(
  const Coefficient& theRingUnit, const Coefficient& theRingZero
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<Coefficient>::IntermediateStepForMakeFromHW");
  ProgressReport theReport;
  ProgressReport theReport2;
  Vector<Rational> targetWeight;
  this->theBilinearFormsAtEachWeightLevel.setSize(this->theGeneratingWordsGrouppedByWeight.size);
  this->theBilinearFormsInverted.setSize(this->theGeneratingWordsGrouppedByWeight.size);
  this->ComputedGeneratorActions.initialize(this->GetOwner().GetNumGenerators());
  this->actionsGeneratorS.setSize(this->GetOwner().GetNumGenerators());
  this->actionsGeneratorsMaT.setSize(this->GetOwner().GetNumGenerators());
  int numScalarProducts = 0;
  this->flagConjectureBholds = true;
  this->flagConjectureCholds = true;
  for (int l = 0; l < this->theGeneratingWordsGrouppedByWeight.size; l ++) {
    Matrix<Coefficient>& currentBF = this->theBilinearFormsAtEachWeightLevel[l];
    List<MonomialUniversalEnveloping<Coefficient> >& currentWordList =
    this->theGeneratingWordsGrouppedByWeight[l];
    List<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >& currentWordListInt = this->theGeneratingWordsIntGrouppedByWeight[l];
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
    tempMat.ComputeDeterminantOverwriteMatrix(tempRat, theRingUnit, theRingZero);
    if (!tempRat.isEqualToZero()) {
      this->theBilinearFormsInverted[l] = currentBF;
      this->theBilinearFormsInverted[l].invert();
      if (!currentBF.IsNonNegativeAllEntries()) {
        this->flagConjectureBholds = false;
      }
    } else {
      this->theBilinearFormsInverted[l].initialize(0, 0);
    }
  }
}

template <class Coefficient>
template <class ResultType>
void ModuleSSalgebra<Coefficient>::GetElementsNilradical(
  List<ElementUniversalEnveloping<ResultType> >& output,
  bool useNegativeNilradical,
  List<int>* outputListOfGenerators,
  bool useNilWeight,
  bool ascending
) {
  SemisimpleLieAlgebra& ownerSS = this->GetOwner();
  ownerSS.OrderNilradical(this->parabolicSelectionNonSelectedAreElementsLevi, useNilWeight, ascending);
  ElementUniversalEnveloping<ResultType> theElt;
  output.setSize(0);
  output.reserve(ownerSS.GetNumPosRoots());

  int theBeginning = useNegativeNilradical ? 0: ownerSS.GetNumPosRoots() +ownerSS.getRank();
  MemorySaving<List<int> > tempList;
  if (outputListOfGenerators == nullptr) {
    outputListOfGenerators = &tempList.getElement();
  }
  outputListOfGenerators->setSize(0);
  outputListOfGenerators->reserve(ownerSS.GetNumPosRoots());
  for (int i = theBeginning; i < theBeginning+ownerSS.GetNumPosRoots(); i ++) {
    if (this->IsNotInLevi(i)) {
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
    theElt.MakeOneGeneratorCoeffOne(outputListOfGenerators->theObjects[i], *this->owner);
    output.addOnTop(theElt);
  }
}

template<class Coefficient>
void ModuleSSalgebra<Coefficient>::checkConsistency() {
  MacroRegisterFunctionWithName("ModuleSSalgebra<Coefficient>::TestConsistency");
  ProgressReport theReport;
  MatrixTensor<Coefficient> left, right, output, otherOutput, tempMat, diffMat;
  for (int i = 0; i < this->GetOwner().GetNumGenerators(); i ++) {
    for (int j = 0; j < this->GetOwner().GetNumGenerators(); j ++) {
      left = this->GetActionGeneratorIndeX(i);
      right = this->GetActionGeneratorIndeX(j);
      output = right;
      output.LieBracketOnTheLeft(left);
      ElementSemisimpleLieAlgebra<Rational> leftGen, rightGen, outputGen;
      leftGen.MakeGenerator(i, *this->theAlgebras, this->indexAlgebra);
      rightGen.MakeGenerator(j, *this->theAlgebras, this->indexAlgebra);
      this->GetOwner().LieBracket(leftGen, rightGen, outputGen);
      otherOutput.makeZero();
      for (int k = 0; k < outputGen.size(); k ++) {
        tempMat = this->GetActionGeneratorIndeX(outputGen[k].theGeneratorIndex);
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
        << "<br>This is a programming error: "
        << "something is wrong with the algorithm, a check fails! "
        << global.fatal;
      } else {
        std::stringstream tempStream;
        tempStream << "tested index " << i + 1
        << " out of " << this->GetOwner().GetNumGenerators()
        << ", " << j + 1 << " out of " << this->GetOwner().GetNumGenerators();
        theReport.report(tempStream.str());
      }
    }
  }
  for (int i = 0; i < this->GetOwner().GetNumPosRoots(); i ++) {
    left = this->GetActionGeneratorIndeX(i);
    right = this->GetActionGeneratorIndeX(this->GetOwner().GetNumGenerators() - 1 - i);
    left.transpose();
    left -= right;
    left = this->GetActionGeneratorIndeX(i);
    right.LieBracketOnTheLeft(left);
  }
  global.comments << "Consistency check passed successfully!";
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::GetFDchar(CharacterSemisimpleLieAlgebraModule<Coefficient>& output) {
  output.makeZero();
  if (this->theHWFundamentalCoordsBaseField.size <= 0) {
    return;
  }
  Weight<Coefficient> tempMon;
  tempMon.owner = &this->GetOwner();
  WeylGroupData& theWeyl = this->GetOwner().theWeyl;
  for (int i = 0; i < this->theModuleWeightsSimpleCoords.size; i ++) {
    tempMon.weightFundamentalCoordS = theWeyl.GetFundamentalCoordinatesFromSimple(this->theModuleWeightsSimpleCoords[i]);
    output.addMonomial(tempMon, this->theGeneratingWordsGrouppedByWeight[i].size);
  }
}

template <class Coefficient>
void ModuleSSalgebra<Coefficient>::ExpressAsLinearCombinationHomogenousElement(
  ElementUniversalEnveloping<Coefficient>& inputHomogeneous,
  ElementUniversalEnveloping<Coefficient>& outputHomogeneous,
  int indexInputBasis,
  const Vector<Coefficient>& subHiGoesToIthElement,
  const Coefficient& theRingUnit,
  const Coefficient& theRingZero
) {
  Vector <Coefficient> theScalarProducts;
  List<MonomialUniversalEnveloping<Coefficient> >& inputBasis =
  this->theGeneratingWordsGrouppedByWeight[indexInputBasis];
  theScalarProducts.setSize(inputBasis.size);

  for (int i = 0; i < inputBasis.size; i ++) {
    std::stringstream tempStream;
    inputHomogeneous.HWTAAbilinearForm(
      inputBasis[i],
      theScalarProducts[i],
      &subHiGoesToIthElement,
      theRingUnit,
      theRingZero,
      &tempStream
    );
  }
  this->theBilinearFormsInverted[indexInputBasis].actOnVectorColumn(theScalarProducts, theRingZero);
  outputHomogeneous.makeZero(*this->theAlgebras, this->indexAlgebra);
  ElementUniversalEnveloping<Coefficient> tempElt;
  for (int i = 0; i < theScalarProducts.size; i ++) {
    tempElt.makeZero(*this->theAlgebras, this->indexAlgebra);
    tempElt.addMonomial(this->theGeneratingWordsGrouppedByWeight[indexInputBasis][i], theRingUnit);
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
void ModuleSSalgebra<Coefficient>::GetAdActionHomogenousElT(
  ElementUniversalEnveloping<Coefficient>& inputHomogeneous,
  Vector<Rational>& weightUEEltSimpleCoords,
  List<List<ElementUniversalEnveloping<Coefficient> > >& outputSortedByArgumentWeight,
  const Coefficient& theRingUnit,
  const Coefficient& theRingZero
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
        theElt.multiplyBy(currentWordList[j], theRingUnit);
        this->ExpressAsLinearCombinationHomogenousElement(
          theElt, currentOutputWord, theIndex, this->theHWDualCoordsBaseFielD, theRingUnit, theRingZero
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
    tempCF.substitution(theSub);
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
  const Coefficient& theRingUnit
) const {
  ElementTensorsGeneralizedVermas<Coefficient> buffer;
  output.makeZero();
  for (int i = 0; i < theUE.size(); i ++) {
    if (!this->multiplyOnTheLeft(theUE[i], buffer, ownerAlgebra, theRingUnit)) {
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
      tempOutput.MultiplyMeByUEEltOnTheLeft(theUE);
      output = tempOutput;
      return true;
    }
    buffer *= theUE.coefficients[i];
    output += buffer;
  }
  return true;
}

template <class Coefficient>
void ElementTensorsGeneralizedVermas<Coefficient>::MakeHWV(
  ModuleSSalgebra<Coefficient>& theOwner, const Coefficient& theRingUnit
) {
  MonomialTensorGeneralizedVermas<Coefficient> tensorMon;
  Coefficient currentCoeff;
  currentCoeff = theRingUnit;
  tensorMon.theMons.setSize(1);
  MonomialGeneralizedVerma<Coefficient>& theMon = tensorMon.theMons[0];
  theMon.indexFDVector = theOwner.theGeneratingWordsNonReduced.size - 1;
  theMon.makeConstant(theOwner);
  this->makeZero();
  this->addMonomial(tensorMon, theRingUnit);
}

template <class Coefficient>
bool ElementTensorsGeneralizedVermas<Coefficient>::multiplyOnTheLeft(
  const MonomialUniversalEnveloping<Coefficient>& theUE,
  ElementTensorsGeneralizedVermas<Coefficient>& output,
  SemisimpleLieAlgebra& ownerAlgebra,
  const Coefficient& theRingUnit
) const {
  MacroRegisterFunctionWithName("ElementTensorsGeneralizedVermas<Coefficient>::multiplyOnTheLeft");
  if (&output == this) {
    global.fatal << "Output equals input, this is not supposed to happen. " << global.fatal;
  }
  output = *this;
  ElementTensorsGeneralizedVermas<Coefficient> buffer;
  for (int i = theUE.Powers.size - 1; i >= 0; i --) {
    int thePower;
    if (!theUE.Powers[i].isSmallInteger(&thePower)) {
      return false;
    }
    int theIndex = theUE.generatorsIndices[i];
    for (int j = 0; j < thePower; j ++) {
      output.MultiplyByElementLieAlg(buffer, ownerAlgebra, theIndex, theRingUnit);
      output = buffer;
    }
  }
  return true;
}

template <class Coefficient>
void ElementTensorsGeneralizedVermas<Coefficient>::MultiplyByElementLieAlg(
  ElementTensorsGeneralizedVermas<Coefficient>& output,
  SemisimpleLieAlgebra& ownerAlgebra,
  int indexGenerator,
  const Coefficient& theRingUnit
) const {
  output.makeZero();
  MonomialTensorGeneralizedVermas<Coefficient> accumMon, monActedOn;
  ElementSumGeneralizedVermas<Coefficient> tempElt;
  ElementUniversalEnveloping<Coefficient> theGenerator;
  theGenerator.MakeOneGenerator(indexGenerator, ownerAlgebra, theRingUnit);
  Coefficient currentCoeff;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialTensorGeneralizedVermas<Coefficient>& currentMon = (*this)[i];
    accumMon.theMons.setSize(0);
    for (int j = 0; j < currentMon.theMons.size; j ++) {
      tempElt.makeZero();
      tempElt.addMonomial(currentMon.theMons[j], theRingUnit);
      tempElt.MultiplyMeByUEEltOnTheLeft(theGenerator);
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
void ElementTensorsGeneralizedVermas<Coefficient>::TensorOnTheRight(
  const ElementTensorsGeneralizedVermas<Coefficient>& right
) {
  MacroRegisterFunctionWithName("ElementTensorsGeneralizedVermas<Coefficient>::TensorOnTheRight");
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
std::string ModuleSSalgebra<Coefficient>::toString(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("ModuleSSalgebra::toString");
  if (this->owner == nullptr) {
    return "(Error: module not initialized)";
  }
  SemisimpleLieAlgebra& theAlgebrA = *this->owner;
  WeylGroupData& theWeyl = theAlgebrA.theWeyl;
  std::stringstream out;
  out << "<br>Semisimple Lie algebra acting on generalized Verma module: "
  << theAlgebrA.ToStringLieAlgebraName() << ".";
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
  if (this->theCharOverH.size() < 100) {
    out << HtmlRoutines::GetMathMouseHover(this->theCharOverH.toString(&latexFormat));
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
    List<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >& currentListInt = this->theGeneratingWordsIntGrouppedByWeight[i];
    for (int j = 0; j < currentList.size; j ++) {
      wordCounter ++;
      tempWelt.generatorsLastAppliedFirst.setSize(currentListInt[j].generatorsIndices.size);
      for (int k = 0; k < currentListInt[j].generatorsIndices.size; k ++) {
        aGen.MakeSimpleReflection(theWeyl.RootsOfBorel.size - 1 - currentListInt[j].generatorsIndices[k]);
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
  << this->ComputedGeneratorActions.CardinalitySelection << " out of "
  << this->actionsGeneratorsMaT.size << " computed actions) follow. "
  << "Note that generator actions are computed on demand, only the simple "
  << "Chevalley generators are computed by default. ";
  out << "<table><tr><td>Generator</td><td>Action</td></tr>";
  ElementSemisimpleLieAlgebra<Rational> tempSSElt;
  for (int i = 0; i < this->actionsGeneratorsMaT.size; i ++) {
    if (this->ComputedGeneratorActions.selected[i]) {
      tempSSElt.MakeGenerator(i, theAlgebrA);
      out << "<tr>";
      out << "<td>"
      << HtmlRoutines::GetMathMouseHover(tempSSElt.toString(theFormat))
      << "</td>";
      out << "<td>";
      if (this->getDimension() < 28) {
        Matrix<Coefficient> outputMat;
        this->actionsGeneratorsMaT[i].GetMatrix(outputMat, this->getDimension());
        out
        << HtmlRoutines::GetMathMouseHover(outputMat.toString(&latexFormat), 5000)
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
  this->theCharOverH.DrawMeAssumeCharIsOverCartan(theWeyl, theDV);
  out << " A picture of the weight support follows. "
  << theDV.GetHtmlDiv(theWeyl.getDimension());

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
    if (!theBF.IsNonNegativeAllEntries()) {
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
#endif
