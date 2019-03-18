//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader_ImplementationHeaderModulesSSLielgebras
#define vpfHeader_ImplementationHeaderModulesSSLielgebras
#include "vpfHeader2Math6_ModulesSSLieAlgebras.h"
#include "vpfHeader2Math9DrawingVariables.h"

static ProjectInformationInstance ProjectInfoVpfImplementationHeaderModulesSSLielgebras(__FILE__, "Implementation header, generalized Verma modules. ");

template<class coefficient>
Rational ModuleSSalgebra<coefficient>::hwTrace(
  const Pair<MonomialTensor<int, MathRoutines::IntUnsignIdentity>,
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> > &thePair, ProgressReport* theProgressReport
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<coefficient>::hwTrace");
  int indexInCache = this->cachedPairs.GetIndex(thePair);
  if (indexInCache != - 1)
    return this->cachedTraces[indexInCache];
  if (thePair.Object1.generatorsIndices.size == 0)
    return 1;
  Pair<MonomialTensor<int, MathRoutines::IntUnsignIdentity>,
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> > newPair;
  MonomialTensor<int, MathRoutines::IntUnsignIdentity>& newLeft = newPair.Object1;
  MonomialTensor<int, MathRoutines::IntUnsignIdentity>& newRight = newPair.Object2;
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& oldRight = thePair.Object2;
  newLeft = thePair.Object1;
  (*newLeft.Powers.LastObject()) -= 1;
  int theIndex = *newLeft.generatorsIndices.LastObject();
  if (*newLeft.Powers.LastObject() == 0) {
    newLeft.generatorsIndices.size --;
    newLeft.Powers.size --;
  }
  int theIndexMinus = 2 * this->GetOwner().GetNumPosRoots() + this->GetOwner().GetRank() - theIndex - 1;
  int theSimpleIndex = theIndex - this->GetOwner().GetNumPosRoots() - this->GetOwner().GetRank();
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> Accum;
  Accum.Powers.Reserve(oldRight.Powers.size);
  Accum.generatorsIndices.Reserve(oldRight.generatorsIndices.size);
  Vector<Rational> RemainingWeight;
  Rational result = 0;
  Rational summand;
  WeylGroupData& theWeyl = this->GetOwner().theWeyl;
  //std::stringstream tempStr;
  //tempStr << thePair.Object2;
  //if (tempStr.str() == "g_{1}^{1}g_{2}^{1}")
  //  stOutput << "<hr><hr>";
  //stOutput << "<br>Computing (" << thePair.Object1 << ", " << thePair.Object2 << ")";
  //if (this->cachedPairs.size< this->MaxNumCachedPairs)
  //{ indexInCache = this->cachedPairs.size;
  //  this->cachedPairs.AddOnTop(thePair);
  //  this->cachedTraces.AddOnTop(0);
  //}
  for (int i = 0; i < oldRight.generatorsIndices.size; i ++) {
    if (oldRight.generatorsIndices[i] == theIndexMinus) {
      summand = 0;
      newRight = Accum;
      newRight.MultiplyByGeneratorPowerOnTheRight(oldRight.generatorsIndices[i], oldRight.Powers[i] - 1);
      RemainingWeight.MakeZero(theWeyl.GetDim());
      for (int j = i + 1; j < oldRight.generatorsIndices.size; j ++) {
        newRight.MultiplyByGeneratorPowerOnTheRight(oldRight.generatorsIndices[j], oldRight.Powers[j]);
        RemainingWeight += theWeyl.RootSystem[oldRight.generatorsIndices[j]] * oldRight.Powers[j];
      }
      RemainingWeight += this->theHWFDpartSimpleCoordS;
      //stOutput << "<br>Remaining weight: " << RemainingWeight.ToString();
      summand += theWeyl.GetScalarProdSimpleRoot(RemainingWeight, theSimpleIndex);
      summand *= 2;
      summand /= theWeyl.CartanSymmetric.elements[theSimpleIndex][theSimpleIndex];
      //stOutput << "<br>The scalar product: " << summand.ToString();
      summand += 1;
      summand -= oldRight.Powers[i];
      //stOutput << "<br>summand: " << summand.ToString();
      if (!summand.IsEqualToZero())
        summand *= this->hwTrace(newPair, theProgressReport);
      //stOutput << "<br>summand after recursion: " << summand.ToString();
      summand *= oldRight.Powers[i];
      result += summand;
    }
    Accum.generatorsIndices.AddOnTop(oldRight.generatorsIndices[i]);
    Accum.Powers.AddOnTop(oldRight.Powers[i]);
  }
  //if (indexInCache != - 1)
  //  this->cachedTraces[indexInCache] =result;
  //if (ProjectInformation::GetMainProjectInfo().CustomStackTrace.size<35)
  if (this->cachedPairs.size< this->MaxNumCachedPairs) {
    this->cachedPairs.AddOnTop(thePair);
    this->cachedTraces.AddOnTop(result);
  }
  if (theProgressReport != 0 && this->cachedPairs.size < 500000) {
    std::stringstream tempStream;
    tempStream << "Number of cached pairs: " << this->cachedPairs.size << " at recursion depth " << theGlobalVariables.CustomStackTrace.size;
    theProgressReport->Report(tempStream.str());
  }

  //stOutput << "<br>Computed (" << thePair.Object1 << ", "
  //<< thePair.Object2 << ")=" << result.ToString();
  //if (tempStr.str() == "g_{1}^{1}g_{2}^{1}")
  //  stOutput << "<hr><hr>";

  return result;
}

template<class coefficient>
void ModuleSSalgebra<coefficient>::ApplyTAA(MonomialTensor<int, MathRoutines::IntUnsignIdentity>& theMon) {
  for (int i = 0; i < theMon.generatorsIndices.size; i ++)
    theMon.generatorsIndices[i] =
    this->GetOwner().GetNumPosRoots() * 2 + this->GetOwner().GetRank() - theMon.generatorsIndices[i] - 1;
  theMon.Powers.ReverseOrderElements();
  theMon.generatorsIndices.ReverseOrderElements();
}

template<class coefficient>
Rational ModuleSSalgebra<coefficient>::hwtaabfSimpleGensOnly(
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& leftMon,
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>& rightMon,
  ProgressReport* theProgressReport
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<coefficient>::hwtaabfSimpleGensOnly");
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>* left = &leftMon;
  const MonomialTensor<int, MathRoutines::IntUnsignIdentity>* right = &rightMon;
  if (leftMon > rightMon)
    MathRoutines::swap(left, right);
  Pair<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, MonomialTensor<int, MathRoutines::IntUnsignIdentity> > thePair;
  thePair.Object1 = *left;
  thePair.Object2 = *right;
  //stOutput << "<br>Computing " << thePair.Object1 << " times " << thePair.Object2 << "<br>";
  this->ApplyTAA(thePair.Object1);
  Rational result = this->hwTrace(thePair, theProgressReport);
  if (theProgressReport != 0) {
    std::stringstream tempStream;
    tempStream << this->cachedPairs.size << " total cached pairs";
    theProgressReport->Report(tempStream.str());
  }
  return result;
}

template<class coefficient>
void ModuleSSalgebra<coefficient>::Substitution(const PolynomialSubstitution<Rational>& theSub) {
  for (int i = 0; i < this->actionsGeneratorsMaT.size; i ++) {
    this->actionsGeneratorsMaT[i].Substitution(theSub);
  }
  for (int i = 0; i < this->actionsGeneratorS.size; i ++) {
    for (int j = 0; j < this->actionsGeneratorS[i].size; j ++) {
      for (int k = 0; k < this->actionsGeneratorS[i][j].size; k ++) {
        this->actionsGeneratorS[i][j][k].Substitution(theSub);
      }
    }
  }
  List<MonomialUniversalEnveloping<coefficient> > oldGeneratingWordsNonReduced;
  oldGeneratingWordsNonReduced = this->theGeneratingWordsNonReduced;
  this->theGeneratingWordsNonReduced.Clear();
  for (int i = 0; i < oldGeneratingWordsNonReduced.size; i ++) {
    oldGeneratingWordsNonReduced[i].Substitution(theSub);
    this->theGeneratingWordsNonReduced.AddOnTop(oldGeneratingWordsNonReduced[i]);
  }
  for (int i = 0; i < this->theGeneratingWordsGrouppedByWeight.size; i ++) {
    for (int j = 0; j < this->theGeneratingWordsGrouppedByWeight[i].size; j ++) {
      this->theGeneratingWordsGrouppedByWeight[i][j].Substitution(theSub);
    }
  }
  for (int i = 0; i < this->theBilinearFormsAtEachWeightLevel.size; i ++) {
    this->theBilinearFormsAtEachWeightLevel[i].Substitution(theSub);
    this->theBilinearFormsInverted[i].Substitution(theSub);
  }
  for (int i = 0; i < this->theHWDualCoordsBaseFielD.size; i ++) {
    this->theHWDualCoordsBaseFielD[i].Substitution(theSub);
    this->theHWFundamentalCoordsBaseField[i].Substitution(theSub);
    this->theHWSimpleCoordSBaseField[i].Substitution(theSub);
  }
}

template <class coefficient>
MatrixTensor<coefficient>& ModuleSSalgebra<coefficient>::GetActionGeneratorIndeX(int generatorIndex) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<coefficient>::GetActionGeneratorIndeX");
  int numGenerators = this->GetOwner().GetNumGenerators();
  if (generatorIndex < 0 || generatorIndex >= numGenerators)
    crash << crash;
  if (this->ComputedGeneratorActions.selected[generatorIndex]) {
    return this->actionsGeneratorsMaT[generatorIndex];
  }
  this->ComputedGeneratorActions.AddSelectionAppendNewIndex(generatorIndex);
  if (this->HasFreeAction(generatorIndex)) {
    crash << "This is a programming error, due to a change in "
    << "implementation of the generalized Verma module class. " << crash;
    this->actionsGeneratorsMaT[generatorIndex].MakeZero();
    return this->actionsGeneratorsMaT[generatorIndex];
  }
  if (this->HasZeroActionFDpart(generatorIndex)) {
    this->actionsGeneratorsMaT[generatorIndex].MakeZero();
    return this->actionsGeneratorsMaT[generatorIndex];
  }
  if (this->GetOwner().IsASimpleGenerator(generatorIndex))
    return this->GetActionSimpleGeneratorIndex(generatorIndex);
  MatrixTensor<coefficient>& output = this->actionsGeneratorsMaT[generatorIndex];
  if (this->GetOwner().IsGeneratorFromCartan(generatorIndex)) {
    output.MakeZero();
    MonomialMatrix theMon;
    Vector<coefficient> weightH;
    coefficient tempCF, hwCFshift;
    weightH.MakeEi(this->GetOwner().GetRank(), generatorIndex - this->GetOwner().GetNumPosRoots());
    hwCFshift = this->GetOwner().theWeyl.RootScalarCartanRoot(weightH, this->theHWSimpleCoordSBaseField);
    hwCFshift -= this->GetOwner().theWeyl.RootScalarCartanRoot(weightH, this->theHWFDpartSimpleCoordS);
    for (int i = 0; i < this->theGeneratingWordsNonReduced.size; i ++) {
      Vector<Rational>& theWeight = this->theGeneratingWordsWeightsPlusWeightFDpart[i];
      tempCF = this->GetOwner().theWeyl.RootScalarCartanRoot(weightH, theWeight);
      tempCF += hwCFshift;
      theMon.IsId = false;
      theMon.vIndex = i;
      theMon.dualIndex = i;
      output.AddMonomial(theMon, tempCF);
    }
    return output;
  }
  List<int> adActions;
  Rational theCoeff;
  this->GetOwner().GetChevalleyGeneratorAsLieBracketsSimpleGens(generatorIndex, adActions, theCoeff);
  MatrixTensor<coefficient> tempO;
  output = this->GetActionGeneratorIndeX(*adActions.LastObject());
  for (int i = adActions.size - 2; i >= 0; i --) {
    tempO = this->GetActionGeneratorIndeX(adActions[i]);
    output.LieBracketOnTheLeft(tempO);
  }
  coefficient tempCF;
  tempCF = theCoeff;
  output *= tempCF;
  return output;
}

template  <class coefficient>
void ModuleSSalgebra<coefficient>::GetMatrixHomogenousElt(
  ElementUniversalEnveloping<coefficient>& inputHomogeneous,
  List<List<ElementUniversalEnveloping<coefficient> > >& outputSortedByArgumentWeight,
  Vector<Rational>& weightUEEltSimpleCoords,
  MatrixTensor<coefficient>& output,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra::GetMatrixHomogenousElt");
  // stOutput << "<hr>status of the module @ start GetMatrixHomogenousElt" << this->ToString();
  this->GetAdActionHomogenousElT(
    inputHomogeneous, weightUEEltSimpleCoords, outputSortedByArgumentWeight, theRingUnit, theRingZero
  );
  output.MakeZero();
  for (int j = 0; j < outputSortedByArgumentWeight.size; j ++) {
    for (int k = 0; k < outputSortedByArgumentWeight[j].size; k ++) {
      MonomialUniversalEnveloping<coefficient>& currentMon = this->theGeneratingWordsGrouppedByWeight[j][k];
      ElementUniversalEnveloping<coefficient>& imageCurrentMon = outputSortedByArgumentWeight[j][k];
      int indexColumn = this->theGeneratingWordsNonReduced.GetIndex(currentMon);
      if (indexColumn == - 1)
        crash << crash;
      for (int l = 0; l < imageCurrentMon.size; l ++) {
        int indexRow = this->theGeneratingWordsNonReduced.GetIndex(imageCurrentMon[l]);
        if (indexRow == - 1)
          crash << crash;
        output.AddMonomial(MonomialMatrix(indexRow, indexColumn), imageCurrentMon.theCoeffs[l]);
      }
    }
  }
  if (!this->flagIsInitialized) {
    return;
  }
}

template <class coefficient>
bool charSSAlgMod<coefficient>::SplitOverLeviMonsEncodeHIGHESTWeight(
  std::string* Report,
  charSSAlgMod& output,
  const Selection& splittingParSel,
  const Selection& ParSelFDInducingPart,
  SubgroupWeylGroupOLD& outputWeylSub
) {
  MacroRegisterFunctionWithName("charSSAlgMod<coefficient>::SplitOverLeviMonsEncodeHIGHESTWeight");
  this->CheckNonZeroOwner();
  std::stringstream out;
  std::string tempS;
  //stOutput << "Splitting parabolic selection: " << splittingParSel.ToString();
  if (this->GetOwner()->GetRank() != splittingParSel.MaxSize)
    crash << "This is a programming error: parabolic selection selects out of " << splittingParSel.MaxSize
    << " elements while the weyl group is of rank " << this->GetOwner()->GetRank() << ". " << crash;
  outputWeylSub.MakeParabolicFromSelectionSimpleRoots(this->GetOwner()->theWeyl, splittingParSel, 1);
  outputWeylSub.ComputeRootSubsystem();

  SubgroupWeylGroupOLD complementGroup, theFDWeyl;
  complementGroup.AmbientWeyl = outputWeylSub.AmbientWeyl;
  theFDWeyl.AmbientWeyl = outputWeylSub.AmbientWeyl;

  Selection invertedSel;
  invertedSel = splittingParSel;
  invertedSel.InvertSelection();
  complementGroup.MakeParabolicFromSelectionSimpleRoots(this->GetOwner()->theWeyl, invertedSel, 1);
  complementGroup.ComputeRootSubsystem();
  out << outputWeylSub.ToString(false);
  charSSAlgMod charAmbientFDWeyl, remainingCharDominantLevi;
  theFDWeyl.MakeParabolicFromSelectionSimpleRoots(this->GetOwner()->theWeyl, ParSelFDInducingPart, 1);
  Weight<coefficient> tempMon, localHighest;
  List<coefficient> tempMults;
  HashedList<Vector<coefficient> > tempHashedRoots;
  WeylGroupData& theWeyL = this->GetOwner()->theWeyl;
  charAmbientFDWeyl.MakeZero();
  coefficient bufferCoeff, highestCoeff;
  out << "Starting character: " << this->ToString();
  tempMon.owner = this->GetOwner();
  for (int i = 0; i < this->size(); i ++) {
    const Weight<coefficient>& currentMon = (*this)[i];
    if (!theFDWeyl.FreudenthalEvalIrrepIsWRTLeviPart(currentMon.weightFundamentalCoordS, tempHashedRoots, tempMults, tempS, 10000)) {
      if (Report != 0) {
        *Report = tempS;
      }
      return false;
    }
    for (int j = 0; j < tempHashedRoots.size; j ++) {
      bufferCoeff = this->theCoeffs[i];
      tempMon.weightFundamentalCoordS = theWeyL.GetFundamentalCoordinatesFromSimple(tempHashedRoots[j]);
      bufferCoeff *= tempMults[j];
      charAmbientFDWeyl.AddMonomial(tempMon, bufferCoeff);
    }
  }

  remainingCharDominantLevi.MakeZero();
  Vectors<coefficient> orbitDom;
  tempMon.owner = this->GetOwner();
  for (int i = 0; i < charAmbientFDWeyl.size(); i ++) {
    orbitDom.SetSize(0);
    if (!theFDWeyl.GenerateOrbitReturnFalseIfTruncated(
      theWeyL.GetSimpleCoordinatesFromFundamental(charAmbientFDWeyl[i].weightFundamentalCoordS), orbitDom, 10000
    )) {
      out << "failed to generate the complement-sub-Weyl-orbit of weight "
      << this->GetOwner()->theWeyl.GetSimpleCoordinatesFromFundamental(charAmbientFDWeyl[i].weightFundamentalCoordS).ToString();
      if (Report != 0) {
        *Report = out.str();
      }
      return false;
    }
    for (int k = 0; k < orbitDom.size; k ++) {
      if (outputWeylSub.IsDominantWeight(orbitDom[k])) {
        tempMon.weightFundamentalCoordS = theWeyL.GetFundamentalCoordinatesFromSimple(orbitDom[k]);
        remainingCharDominantLevi.AddMonomial(tempMon, charAmbientFDWeyl.theCoeffs[i]);
      }
    }
  }
  output.MakeZero();
  out << "<br>Character w.r.t Levi part: " << HtmlRoutines::GetMathMouseHover(remainingCharDominantLevi.ToString());
  Vector<coefficient> simpleGeneratorBaseField;
  while (!remainingCharDominantLevi.IsEqualToZero()) {
    localHighest = *remainingCharDominantLevi.theMonomials.LastObject();
    for (bool Found = true; Found; ) {
      Found = false;
      for (int i = 0; i < outputWeylSub.simpleGenerators.size; i ++) {
        tempMon = localHighest;
        simpleGeneratorBaseField = outputWeylSub.simpleGenerators[i]; // <- implicit type conversion here!
        tempMon.weightFundamentalCoordS += this->GetOwner()->theWeyl.GetFundamentalCoordinatesFromSimple(simpleGeneratorBaseField);
        if (remainingCharDominantLevi.theMonomials.Contains(tempMon)) {
          localHighest = tempMon;
          Found = true;
        }
      }
    }
    highestCoeff = remainingCharDominantLevi.theCoeffs[remainingCharDominantLevi.theMonomials.GetIndex(localHighest)];
    output.AddMonomial(localHighest, highestCoeff);
    if (!outputWeylSub.FreudenthalEvalIrrepIsWRTLeviPart(
      localHighest.weightFundamentalCoordS, tempHashedRoots, tempMults, tempS, 10000
    )) {
      if (Report != 0) {
        *Report = tempS;
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
  out << "<br>Character w.r.t Levi part: " << HtmlRoutines::GetMathMouseHover(output.ToString());
  if (Report != 0) {
    DrawingVariables theDV;
    std::string tempS;
    this->DrawMeNoMults(tempS, theDV, 10000);
    Vector<Rational> tempRoot;
    out << "<hr>In the following weight visualization, a yellow line is drawn if the corresponding weights are "
    << " simple reflections of one another, with respect to a simple root of the Levi part of the parabolic subalgebra. ";
    for (int i = 0; i < output.size(); i ++) {
      tempRoot = theWeyL.GetSimpleCoordinatesFromFundamental(output[i].weightFundamentalCoordS).GetVectorRational();
      outputWeylSub.DrawContour(tempRoot, theDV, "#a0a000", 1000);
      std::stringstream tempStream;
      tempStream << output.theCoeffs[i].ToString();
      theDV.drawTextAtVectorBufferRational(tempRoot, tempStream.str(), "black");
    }
    out << "<hr>" << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyL.GetDim());
    *Report = out.str();
  }
  return true;
}

template<class coefficient>
void ModuleSSalgebra<coefficient>::SplitOverLevi(
  std::string* Report,
  Selection& splittingParSel,
  List<ElementUniversalEnveloping<coefficient> >* outputEigenVectors,
  Vectors<coefficient>* outputWeightsFundCoords,
  Vectors<coefficient>* outputEigenSpace,
  charSSAlgMod<coefficient>* outputChar
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<coefficient>::SplitOverLevi");
  this->CheckInitialization();
  if (this->theChaR.size() != 1) {
    if (Report != 0) {
      std::stringstream out;
      out << "I have been instructed only to split modules that are irreducible over the ambient Lie algebra";
      out << " Instead I got the character " << this->theChaR.ToString() << " (" << this->theChaR.size() << " monomials)";
      *Report = out.str();
    }
    return;
  }
  if (this->GetOwner().GetRank() != splittingParSel.MaxSize) {
    crash << "This is a programming error: semisimple rank is "
    << this->GetOwner().GetRank() << " but splitting parabolic selects "
    << " out of " << splittingParSel.MaxSize << " simple roots. " << crash;
  }
  SubgroupWeylGroupOLD subWeyl;
  subWeyl.AmbientWeyl = &this->owner->theWeyl;
  MemorySaving<charSSAlgMod<coefficient> > buffer;
  charSSAlgMod<coefficient>& charWRTsubalgebra = (outputChar == 0) ? buffer.GetElement() : *outputChar;
  this->theChaR.SplitOverLeviMonsEncodeHIGHESTWeight(
    Report, charWRTsubalgebra, splittingParSel, this->parabolicSelectionNonSelectedAreElementsLevi, subWeyl
  );
  std::stringstream out;
  if (Report != 0) {
    out << *Report;
  }
  Selection splittingParSelectedInLevi;
  splittingParSelectedInLevi = splittingParSel;
  splittingParSelectedInLevi.InvertSelection();
  if (!splittingParSelectedInLevi.IsSubset(this->parabolicSelectionSelectedAreElementsLevi)) {
    out << "The parabolic subalgebra you selected is not a subalgebra of the ambient parabolic subalgebra."
    << " The parabolic has root of Levi given by " << splittingParSel.ToString()
    << " while the ambient parabolic subalgebra has root of Levi given by "
    << this->parabolicSelectionNonSelectedAreElementsLevi.ToString();
    if (Report != 0) {
      *Report = out.str();
    }
    return;
  }
  out << "<br>Parabolic selection: " << splittingParSel.ToString();
  List<List<Vector<coefficient> > > eigenSpacesPerSimpleGenerator;
  //if (false)
  eigenSpacesPerSimpleGenerator.SetSize(splittingParSelectedInLevi.CardinalitySelection);
  Vectors<coefficient> tempSpace1, tempSpace2;
  MemorySaving<Vectors<coefficient> > tempEigenVects;
  Vectors<coefficient>& theFinalEigenSpace = (outputEigenSpace == 0) ? tempEigenVects.GetElement() : *outputEigenSpace;
  //WeylGroup& theWeyL = this->theAlgebra.theWeyl;
  theFinalEigenSpace.MakeEiBasis(this->GetDim());
  for (int i = 0; i < splittingParSelectedInLevi.CardinalitySelection; i ++) {
    int theGenIndex = splittingParSelectedInLevi.elements[i] +
    this->GetOwner().GetRank() + this->GetOwner().GetNumPosRoots();
    MatrixTensor<coefficient>& currentOp = this->GetActionGeneratorIndeX(theGenIndex);
    Matrix<coefficient> currentOpMat;
    currentOp.GetMatrix(currentOpMat, this->GetDim());
    currentOpMat.GetZeroEigenSpaceModifyMe(eigenSpacesPerSimpleGenerator[i]);
    tempSpace1 = theFinalEigenSpace;
    tempSpace2 = eigenSpacesPerSimpleGenerator[i];
    theFinalEigenSpace.IntersectTwoLinSpaces(tempSpace1, tempSpace2, theFinalEigenSpace);
  }
  out << "<br>Eigenvectors:<table> ";
  //Vector<Rational> zeroRoot;
  //zeroRoot.MakeZero(this->theAlgebra->GetRank());
  std::stringstream readyForLatexComsumption;
  readyForLatexComsumption << "\\begin{tabular}{|lll|}\n <br>";
  readyForLatexComsumption << "\\hline\\multicolumn{3}{|c|}{Highest weight $"
  << this->theHWFundamentalCoordsBaseField.ToStringLetterFormat("\\omega") << "$}\\\\\n<br>";
  readyForLatexComsumption << "weight fund. coord.& singular vector \\\\\\hline\n<br>";
  Vector<coefficient> currentWeight;
  Vector<coefficient> hwFundCoordsNilPart;
  hwFundCoordsNilPart = this->theHWFundamentalCoordsBaseField;
  hwFundCoordsNilPart -= this->theHWFDpartFundamentalCoordS;
  ElementUniversalEnveloping<coefficient> currentElt, tempElt;
  for (int j = 0; j < theFinalEigenSpace.size; j ++) {
    out << "<tr><td>";
    currentElt.MakeZero(this->GetOwner());
    Vector<coefficient>& currentVect = theFinalEigenSpace[j];
    int lastNonZeroIndex = - 1;
    for (int i = 0; i < currentVect.size; i ++) {
      if (!(currentVect[i].IsEqualToZero())) {
        tempElt.MakeZero(this->GetOwner());
        tempElt.AddMonomial(this->theGeneratingWordsNonReduced[i], 1);
        tempElt *= currentVect[i];
        currentElt += tempElt;
        lastNonZeroIndex = i;
      }
    }
    currentWeight = subWeyl.AmbientWeyl->GetFundamentalCoordinatesFromSimple(this->theGeneratingWordsWeightsPlusWeightFDpart[lastNonZeroIndex]);//<-implicitTypeConversionHere
    currentWeight += hwFundCoordsNilPart;
    readyForLatexComsumption <<  "$" << currentWeight.ToStringLetterFormat("\\omega") << "$&$" << currentVect.ToStringLetterFormat("m") << "$";
    if (currentElt.size() > 1)
      out << "(";
    if (outputEigenVectors != 0)
      outputEigenVectors->AddOnTop(currentElt);
    if (outputWeightsFundCoords != 0)
      outputWeightsFundCoords->AddOnTop(currentWeight);
    out << currentElt.ToString(&theGlobalVariables.theDefaultFormat.GetElement());
    if (currentElt.size() > 1)
      out << ")";
    out << " v_\\lambda";
    out << "</td><td>(weight: " << currentWeight.ToStringLetterFormat("\\omega") << ")</td></tr>";
    readyForLatexComsumption << "\\\\\n<br>";
  }
  out << "</table>";
  readyForLatexComsumption << "\\hline \n<br> \\end{tabular}";
  out << "<br>Your ready for LaTeX consumption text follows.<br>";
  out << readyForLatexComsumption.str();
  if (Report != 0) {
    *Report = out.str();
  }
}

template <class coefficient>
MatrixTensor<coefficient>& ModuleSSalgebra<coefficient>::GetActionSimpleGeneratorIndex(int generatorIndex) {
  Vector<Rational> genWeight = this->GetOwner().GetWeightOfGenerator(generatorIndex);
  Vector<Rational> targetWeight;
  Pair<MonomialTensor<int, MathRoutines::IntUnsignIdentity>, MonomialTensor<int, MathRoutines::IntUnsignIdentity> > currentPair;
  MatrixTensor<coefficient>& outputMat = this->actionsGeneratorsMaT[generatorIndex];
  Vector<coefficient> theScalarProds;
  outputMat.MakeZero();
  for (int i = 0; i < this->theGeneratingWordsGrouppedByWeight.size; i ++) {
    List<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >& currentWordList =
    this->theGeneratingWordsIntGrouppedByWeight[i];
    const Vector<Rational>& currentWeight = this->theModuleWeightsSimpleCoords[i];
    targetWeight = currentWeight + genWeight;
    //stOutput << "<br>target weight: " << targetWeight.ToString() << "="
    //<< currentWeight.ToString() << "+" << inputWeight.ToString();
    int weightLevelIndex = this->theModuleWeightsSimpleCoords.GetIndex(targetWeight);
    if (weightLevelIndex != - 1) {
      int columnOffset = this->GetOffsetFromWeightIndex(i);
      int rowOffset = this->GetOffsetFromWeightIndex(weightLevelIndex);
      List<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >&
      otherWordList = this->theGeneratingWordsIntGrouppedByWeight[weightLevelIndex];
      for (int j = 0; j < currentWordList.size; j ++) {
        theScalarProds.SetSize(otherWordList.size);
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
        this->theBilinearFormsInverted[weightLevelIndex].ActOnVectorColumn(theScalarProds);
        for (int k = 0; k < theScalarProds.size; k ++)
          outputMat.AddMonomial(MonomialMatrix(rowOffset + k, columnOffset + j), theScalarProds[k]);
      }
    }
  }
  return outputMat;
}

template<class coefficient>
bool ModuleSSalgebra<coefficient>::MakeFromHW(
  SemisimpleLieAlgebra& inputAlgebra,
  Vector<coefficient>& HWFundCoords,
  const Selection& selNonSelectedAreElementsLevi,
  const coefficient& theRingUnit,
  const coefficient& theRingZero,
  std::string* outputReport,
  bool computeSimpleGens
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<coefficient>::MakeFromHW");
  ProgressReport theReport;
  this->owner = &inputAlgebra;
  SemisimpleLieAlgebra& theAlgebrA = inputAlgebra;

  int theRank = theAlgebrA.GetRank();
  if (HWFundCoords.size != theRank || selNonSelectedAreElementsLevi.MaxSize != theRank) {
    crash << "This is a programming error. I am asked to create a "
    << "generalized Verma module with a semisimple Lie algebra of rank "
    << theRank << " but the input highest weight, "
    << HWFundCoords.ToString() << ", has " << HWFundCoords.size << " coordinates and "
    << " the parabolic section indicates rank of "
    << selNonSelectedAreElementsLevi.MaxSize << ". " << crash;
  }
  WeylGroupData& theWeyl = theAlgebrA.theWeyl;
  this->cachedPairs.Clear();
  this->cachedTraces.SetSize(0);

  this->parabolicSelectionNonSelectedAreElementsLevi = selNonSelectedAreElementsLevi;
  this->parabolicSelectionSelectedAreElementsLevi = this->parabolicSelectionNonSelectedAreElementsLevi;
  this->parabolicSelectionSelectedAreElementsLevi.InvertSelection();

  this->theHWFundamentalCoordsBaseField = HWFundCoords;
  this->theHWDualCoordsBaseFielD.SetSize(theRank);
  this->theHWFDpartFundamentalCoordS.SetSize(theRank);

  for (int i = 0; i < theRank; i ++) {
    this->theHWFDpartFundamentalCoordS[i] = 0;
    if (this->parabolicSelectionSelectedAreElementsLevi.selected[i]) {
      int theCoord;
      if (!this->theHWFundamentalCoordsBaseField[i].IsSmallInteger(&theCoord)) {
        if (outputReport != 0) {
          *outputReport = "The given module over the Levi part is not finite dimensional";
        }
        return false;
      }
      this->theHWFDpartFundamentalCoordS[i] = theCoord;
    }
    this->theHWDualCoordsBaseFielD[i] = this->theHWFundamentalCoordsBaseField[i];
    this->theHWDualCoordsBaseFielD[i] *= theWeyl.CartanSymmetric.elements[i][i] / 2;
  }

  this->theHWFDpartSimpleCoordS = theWeyl.GetSimpleCoordinatesFromFundamental(this->theHWFDpartFundamentalCoordS);
  this->theHWFDpartDualCoords = theWeyl.GetDualCoordinatesFromFundamental(this->theHWFDpartFundamentalCoordS);
  this->theHWSimpleCoordSBaseField = theWeyl.GetSimpleCoordinatesFromFundamental(this->theHWFundamentalCoordsBaseField);
  this->theChaR.MakeFromWeight(this->theHWSimpleCoordSBaseField, this->owner);

  int startingNumRationalOperations = Rational::TotalLargeAdditions + Rational::TotalSmallAdditions + Rational::TotalLargeMultiplications + Rational::TotalSmallMultiplications;
  LittelmannPath startingPath;
  startingPath.MakeFromWeightInSimpleCoords(this->theHWFDpartSimpleCoordS, theWeyl);
  //stOutput << "<br>starting littelmann path: " << startingPath.ToString() << this->parabolicSelectionNonSelectedAreElementsLevi.ToString();
  List<List<int> > generatorsIndices;
  if (!startingPath.GenerateOrbit(
    this->thePaths, generatorsIndices, 1000, &this->parabolicSelectionNonSelectedAreElementsLevi
  )) {
    if (outputReport != 0) {
      *outputReport = "Error: number of Littelmann paths exceeded allowed limit of 1000.";
    }
    return false;
  }
  this->theModuleWeightsSimpleCoords.Clear();
  Weight<coefficient> tempCharMon;
  tempCharMon.owner = 0;
  this->theCharOverH.MakeZero();
  for (int i = 0; i < this->thePaths.size; i ++) {
    this->theModuleWeightsSimpleCoords.AddOnTopNoRepetition(*this->thePaths[i].Waypoints.LastObject());
    tempCharMon.weightFundamentalCoordS = theWeyl.GetFundamentalCoordinatesFromSimple(*this->thePaths[i].Waypoints.LastObject());
    this->theCharOverH.AddMonomial(tempCharMon, 1);
  }
  this->theModuleWeightsSimpleCoords.QuickSortAscending();
  std::stringstream out2;
  this->theGeneratingWordsGrouppedByWeight.SetSize(this->theModuleWeightsSimpleCoords.size);
  this->theGeneratingWordsIntGrouppedByWeight.SetSize(this->theModuleWeightsSimpleCoords.size);
  for (int i = 0; i < this->theGeneratingWordsGrouppedByWeight.size; i ++) {
    this->theGeneratingWordsGrouppedByWeight[i].size = 0;
    this->theGeneratingWordsIntGrouppedByWeight[i].size = 0;
  }
  MonomialUniversalEnveloping<coefficient> currentNonReducedElement;
  MonomialTensor<int, MathRoutines::IntUnsignIdentity> tempMonInt;
  for (int i = 0; i < this->thePaths.size; i ++) {
    List<int>& currentPath = generatorsIndices[i];
    currentNonReducedElement.MakeOne(this->GetOwner());
    tempMonInt.MakeConst();
    for (int j = currentPath.size - 1; j >= 0; j --) {
      int theIndex = currentPath[j];
      if (theIndex > 0) {
        theIndex ++;
      }
      currentNonReducedElement.MultiplyByGeneratorPowerOnTheRight(this->GetOwner().GetGeneratorFromDisplayIndex(theIndex), theRingUnit);
      tempMonInt.MultiplyByGeneratorPowerOnTheRight(this->GetOwner().GetGeneratorFromDisplayIndex(theIndex), 1);
    }
    Vector<Rational>& hwCurrent = *this->thePaths[i].Waypoints.LastObject();
    int theIndex = this->theModuleWeightsSimpleCoords.GetIndex(hwCurrent);
    if (theIndex == - 1) {
      out2 << "Error: could not generate all weights in the weight support. Maybe they are too many? Allowed "
      << " # of weights is 10000";
      if (outputReport != 0) {
        *outputReport = out2.str();
      }
      return false;
    }
    this->theGeneratingWordsGrouppedByWeight[theIndex].AddOnTop(currentNonReducedElement);
    this->theGeneratingWordsIntGrouppedByWeight[theIndex].AddOnTop(tempMonInt);
  }
  this->theGeneratingWordsNonReduced.Clear();
  this->theGeneratingWordsNonReduced.SetExpectedSize(this->thePaths.size);
  this->theGeneratingWordsNonReducedInt.Clear();
  this->theGeneratingWordsNonReducedInt.SetExpectedSize(this->thePaths.size);
  this->theGeneratingWordsNonReduced.size = 0;
  this->theGeneratingWordsWeightsPlusWeightFDpart.SetSize(0);
  this->theGeneratingWordsWeightsPlusWeightFDpart.Reserve(this->thePaths.size);
  for (int i = 0; i < this->theGeneratingWordsGrouppedByWeight.size; i ++) {
    List<MonomialUniversalEnveloping<coefficient> >& currentList = this->theGeneratingWordsGrouppedByWeight[i];
    List<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >& currentListInt = this->theGeneratingWordsIntGrouppedByWeight[i];
    currentList.QuickSortDescending();
    currentListInt.QuickSortDescending();
    for (int j = 0; j < currentList.size; j ++) {
      //wordCounter ++;
      this->theGeneratingWordsNonReduced.AddOnTop(currentList[j]);
      this->theGeneratingWordsNonReducedInt.AddOnTop(currentListInt[j]);
      this->theGeneratingWordsWeightsPlusWeightFDpart.AddOnTop(this->theModuleWeightsSimpleCoords[i]);
    }
  }
  this->IntermediateStepForMakeFromHW(theRingUnit, theRingZero);
  this->NumCachedPairsBeforeSimpleGen = this->cachedPairs.size;
  this->NumRationalMultiplicationsAndAdditionsBeforeSimpleGen =
  Rational::TotalLargeAdditions + Rational::TotalSmallAdditions + Rational::TotalLargeMultiplications + Rational::TotalSmallMultiplications - startingNumRationalOperations;
  bool isBad = false;
  for (int k = 0; k < this->theBilinearFormsAtEachWeightLevel.size; k ++) {
    Matrix<coefficient>& theBF = this->theBilinearFormsAtEachWeightLevel[k];
    Matrix<coefficient>& theBFinverted = this->theBilinearFormsInverted[k];
    if (!theBF.IsNonNegativeAllEntries()) {
      this->flagConjectureBholds = false;
    }
    if (theBFinverted.NumRows <= 0) {
      isBad = true;
    }
  }
  if (isBad) {
    if (outputReport != 0) {
      out2 << "<br>Error: the Littelmann-path induced monomials do not give a monomial basis. ";
      *outputReport = out2.str();
    }
    return false;
  }
  ElementSemisimpleLieAlgebra<Rational> tempSSElt;
  if (computeSimpleGens) {
    for (int k = 0; k < 2; k ++) {
      for (int j = 0; j < this->GetOwner().GetRank(); j ++) {
        if (this->parabolicSelectionSelectedAreElementsLevi.selected[j]) {
          int theIndex = this->GetOwner().GetNumPosRoots() - j - 1;
          if (k == 1) {
            theIndex = this->GetOwner().GetNumPosRoots() + this->GetOwner().GetRank() + j;
          }
          tempSSElt.MakeGenerator(theIndex, this->GetOwner());
          if (outputReport != 0) {
            out2 << "<hr>Simple generator: " << tempSSElt.ToString(&theGlobalVariables.theDefaultFormat.GetElement());
          }
          MatrixTensor<coefficient>& theMatrix = this->GetActionGeneratorIndeX(theIndex);
          std::stringstream tempStream;
          tempStream << "computing action simple generator index " << (2 * k - 1) * (j + 1) << " ... ";
          theReport.Report(tempStream.str());
          tempStream << " done!";
          theReport.Report(tempStream.str());
          if (outputReport != 0) {
            if (this->GetDim() < 50) {
              out2 << "<br>Matrix of elemenent in the m_i basis:<br>" << HtmlRoutines::GetMathMouseHover(theMatrix.ToString(), 5000);
            } else {
              out2 << "<br>Matrix of elemenent in the m_i basis:<br>" << theMatrix.ToString();
            }
          }
      /*    for (int j = 0; j < this->actionsSimpleGens[i].size; j ++)
            for (int k = 0; k< this->actionsSimpleGens[i][j].size; k++) {
              out << "<br>" << theSimpleGenerator.ToString(theGlobalVariables, tempFormat) << "\\cdot "
              << this->theGeneratingWordsGrouppedByWeight[j][k].ToString(false, false, tempFormat)
              << "\\cdot v=" << this->actionsSimpleGens[i][j][k].ToString(theGlobalVariables, tempFormat)
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
  if (outputReport != 0)
    *outputReport = out2.str();
  this->flagIsInitialized = true;
  theReport.Report("Done with module generation");
  return true;
}

template<class coefficient>
void ModuleSSalgebra<coefficient>::IntermediateStepForMakeFromHW(
  const coefficient& theRingUnit, const coefficient& theRingZero
) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<coefficient>::IntermediateStepForMakeFromHW");
  ProgressReport theReport;
  ProgressReport theReport2;
  Vector<Rational> targetWeight;
  this->theBilinearFormsAtEachWeightLevel.SetSize(this->theGeneratingWordsGrouppedByWeight.size);
  this->theBilinearFormsInverted.SetSize(this->theGeneratingWordsGrouppedByWeight.size);
  this->ComputedGeneratorActions.init(this->GetOwner().GetNumGenerators());
  this->actionsGeneratorS.SetSize(this->GetOwner().GetNumGenerators());
  this->actionsGeneratorsMaT.SetSize(this->GetOwner().GetNumGenerators());
  int numScalarProducts = 0;
  this->flagConjectureBholds = true;
  this->flagConjectureCholds = true;
  for (int l = 0; l < this->theGeneratingWordsGrouppedByWeight.size; l ++) {
    Matrix<coefficient>& currentBF = this->theBilinearFormsAtEachWeightLevel[l];
    List<MonomialUniversalEnveloping<coefficient> >& currentWordList =
    this->theGeneratingWordsGrouppedByWeight[l];
    List<MonomialTensor<int, MathRoutines::IntUnsignIdentity> >& currentWordListInt = this->theGeneratingWordsIntGrouppedByWeight[l];
    currentBF.init(currentWordList.size, currentWordList.size);
    for (int i = 0; i < currentWordList.size; i ++) {
      for (int j = i; j < currentWordList.size; j ++) {
        std::stringstream tempStream;
        tempStream << " Computing Shapovalov form layer " << l << " out of " << this->theGeneratingWordsGrouppedByWeight.size
        << " between indices " << i + 1 << " and " << j + 1 << " out of " << currentWordList.size;
        numScalarProducts ++;
        currentBF.elements[i][j] = this->hwtaabfSimpleGensOnly(currentWordListInt[i], currentWordListInt[j], &theReport2);
        theReport.Report(tempStream.str());
        if (i != j) {
          currentBF.elements[j][i] = currentBF.elements[i][j];
        }
      }
    }
    Matrix<coefficient> tempMat;
    tempMat = currentBF;
    coefficient tempRat;
    tempMat.ComputeDeterminantOverwriteMatrix(tempRat, theRingUnit, theRingZero);
    if (!tempRat.IsEqualToZero()) {
      this->theBilinearFormsInverted[l] = currentBF;
      this->theBilinearFormsInverted[l].Invert();
      if (!currentBF.IsNonNegativeAllEntries()) {
        this->flagConjectureBholds = false;
      }
    } else {
      this->theBilinearFormsInverted[l].init(0, 0);
    }
  }
}

template <class coefficient>
template <class ResultType>
void ModuleSSalgebra<coefficient>::GetElementsNilradical(
  List<ElementUniversalEnveloping<ResultType> >& output,
  bool useNegativeNilradical,
  List<int>* outputListOfGenerators,
  bool useNilWeight,
  bool ascending
) {
  SemisimpleLieAlgebra& ownerSS = this->GetOwner();
  ownerSS.OrderNilradical(this->parabolicSelectionNonSelectedAreElementsLevi, useNilWeight, ascending);
  ElementUniversalEnveloping<ResultType> theElt;
  output.SetSize(0);
  output.Reserve(ownerSS.GetNumPosRoots());

  int theBeginning = useNegativeNilradical ? 0: ownerSS.GetNumPosRoots() +ownerSS.GetRank();
  MemorySaving<List<int> > tempList;
  if (outputListOfGenerators == 0)
    outputListOfGenerators = &tempList.GetElement();
  outputListOfGenerators->SetSize(0);
  outputListOfGenerators->Reserve(ownerSS.GetNumPosRoots());
  for (int i = theBeginning; i < theBeginning+ownerSS.GetNumPosRoots(); i ++)
    if (this->IsNotInLevi(i))
      outputListOfGenerators->AddOnTop(i);
  //bubble sort:
  for (int i = 0; i < outputListOfGenerators->size; i ++) {
    for (int j = i + 1; j < outputListOfGenerators->size; j ++) {
      if (
        ownerSS.UEGeneratorOrderIncludingCartanElts[(*outputListOfGenerators)[i]] >
        ownerSS.UEGeneratorOrderIncludingCartanElts[(*outputListOfGenerators)[j]]
      ) {
        outputListOfGenerators->SwapTwoIndices(i, j);
      }
    }
  }
  for (int i = 0; i < outputListOfGenerators->size; i ++) {
    theElt.MakeOneGeneratorCoeffOne(outputListOfGenerators->TheObjects[i], *this->owner);
    output.AddOnTop(theElt);
  }
}

template<class coefficient>
void ModuleSSalgebra<coefficient>::CheckConsistency(GlobalVariables& theGlobalVariables) {
  MacroRegisterFunctionWithName("ModuleSSalgebra<coefficient>::TestConsistency");
  ProgressReport theReport;
  MatrixTensor<coefficient> left, right, output, otherOutput, tempMat, diffMat;
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
      otherOutput.MakeZero();
      for (int k = 0; k < outputGen.size(); k ++) {
        tempMat = this->GetActionGeneratorIndeX(outputGen[k].theGeneratorIndex);
        tempMat *= outputGen.theCoeffs[k];
        otherOutput += tempMat;
      }
      diffMat = otherOutput;
      diffMat -= output;
      if (!diffMat.IsEqualToZero()) {
        crash << "<br>[" << left.ToString() << ", " << right.ToString() << "] = " << output.ToString() << "<br> [" << leftGen.ToString()
        << ", " << rightGen.ToString() << "] = " << outputGen.ToString() << "<br> " << outputGen.ToString() << "->" << otherOutput.ToString()
        << "<br>This is a programming error: something is wrong with the algorithm, a check fails! " << crash;
      } else {
        std::stringstream tempStream;
        tempStream << "tested index" << i + 1 << " out of " << this->GetOwner().GetNumGenerators() << ", " << j + 1 << " out of " << this->GetOwner().GetNumGenerators();
        theReport.Report(tempStream.str());
      }
    }
  }
  for (int i = 0; i < this->GetOwner().GetNumPosRoots(); i ++) {
    left = this->GetActionGeneratorIndeX(i);
    right = this->GetActionGeneratorIndeX(this->GetOwner().GetNumGenerators() - 1 - i);
    left.Transpose();
    left -= right;
    left = this->GetActionGeneratorIndeX(i);
    right.LieBracketOnTheLeft(left);
  }
  stOutput << "Consistency check passed successfully!";
}

template <class coefficient>
void ModuleSSalgebra<coefficient>::GetFDchar(charSSAlgMod<coefficient>& output) {
  output.MakeZero();
  if (this->theHWFundamentalCoordsBaseField.size <= 0)
    return;
  Weight<coefficient> tempMon;
  tempMon.owner = &this->GetOwner();
  WeylGroupData& theWeyl = this->GetOwner().theWeyl;
  for (int i = 0; i < this->theModuleWeightsSimpleCoords.size; i ++) {
    tempMon.weightFundamentalCoordS = theWeyl.GetFundamentalCoordinatesFromSimple(this->theModuleWeightsSimpleCoords[i]);
    output.AddMonomial(tempMon, this->theGeneratingWordsGrouppedByWeight[i].size);
  }
}

template <class coefficient>
void ModuleSSalgebra<coefficient>::ExpressAsLinearCombinationHomogenousElement(
  ElementUniversalEnveloping<coefficient>& inputHomogeneous,
  ElementUniversalEnveloping<coefficient>& outputHomogeneous,
  int indexInputBasis,
  const Vector<coefficient>& subHiGoesToIthElement,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  Vector <coefficient> theScalarProducts;
  List<MonomialUniversalEnveloping<coefficient> >& inputBasis = this->theGeneratingWordsGrouppedByWeight[indexInputBasis];
  theScalarProducts.SetSize(inputBasis.size);

  for (int i = 0; i < inputBasis.size; i ++) {
    std::stringstream tempStream;
    inputHomogeneous.HWTAAbilinearForm(inputBasis[i], theScalarProducts[i], &subHiGoesToIthElement, theRingUnit, theRingZero, &tempStream);
  }
  this->theBilinearFormsInverted[indexInputBasis].ActOnVectorColumn(theScalarProducts, theRingZero);
  outputHomogeneous.MakeZero(*this->theAlgebras, this->indexAlgebra);
  ElementUniversalEnveloping<coefficient> tempElt;
  for (int i = 0; i < theScalarProducts.size; i ++) {
    tempElt.MakeZero(*this->theAlgebras, this->indexAlgebra);
    tempElt.AddMonomial(this->theGeneratingWordsGrouppedByWeight[indexInputBasis][i], theRingUnit);
    tempElt *= theScalarProducts[i];
    outputHomogeneous += tempElt;
  }
}

template <class coefficient>
void ModuleSSalgebra<coefficient>::reset() {
  this->actionsGeneratorsMaT.SetSize(0);
  this->actionsGeneratorS.SetSize(0);
  this->ComputedGeneratorActions.init(0);
  this->owner = 0;
  this->theGeneratingWordsNonReduced.Clear();
  //Note: for some reason, the linker fails to resolve without the explicit template
  //specialization below.
  //[Update:] made a bug report on this in the gcc bug tracker.
  //This issue has officially been recognized as a gcc bug. Hope to get a fix soon.
  this->theGeneratingWordsNonReducedInt.Clear();
  this->theGeneratingWordsWeightsPlusWeightFDpart.SetSize(0);
  this->thePaths.SetSize(0);
  this->theGeneratingWordsGrouppedByWeight.SetSize(0);
  this->theGeneratingWordsIntGrouppedByWeight.SetSize(0);
  //List<ElementUniversalEnveloping<coefficient> > theSimpleGens;
  //List<List<List<ElementUniversalEnveloping<coefficient> > > > actionsSimpleGens;
  //List<Matrix<coefficient> > actionsSimpleGensMatrixForM;
  this->theBilinearFormsAtEachWeightLevel.SetSize(0);
  this->theBilinearFormsInverted.SetSize(0);
  //Vectors<Rational> weightsSimpleGens;
  this->theHWDualCoordsBaseFielD.SetSize(0);
  this->theHWSimpleCoordSBaseField.SetSize(0);
  this->theHWFundamentalCoordsBaseField.SetSize(0);
  this->theHWFDpartDualCoords.SetSize(0);
  this->theHWFDpartSimpleCoordS.SetSize(0);
  this->theHWFDpartFundamentalCoordS.SetSize(0);
  //List<List<Matrix<coefficient> > >
  this->theModuleWeightsSimpleCoords.Clear();
  this->theCharOverH.MakeZero();
  this->theChaR.MakeZero();
  this->parabolicSelectionNonSelectedAreElementsLevi.init(0);
  this->parabolicSelectionSelectedAreElementsLevi.init(0);
  this->highestWeightVectorNotation = "";
  //Note: for some reason, the linker fails to resolve without the
  //explicit template specialization below.
  //[Update:] This is now a recognized gcc bug.
  this->cachedPairs.Clear();
  this->cachedTraces.SetSize(0);
  this->flagIsInitialized = false;
  this->flagConjectureBholds = true;
  this->flagConjectureCholds = true;
  this->NumCachedPairsBeforeSimpleGen = 0;
  this->NumRationalMultiplicationsAndAdditionsBeforeSimpleGen = 0;
  this->MaxNumCachedPairs = 1000000;
}

template <class coefficient>
void ModuleSSalgebra<coefficient>::GetAdActionHomogenousElT(
  ElementUniversalEnveloping<coefficient>& inputHomogeneous,
  Vector<Rational>& weightUEEltSimpleCoords,
  List<List<ElementUniversalEnveloping<coefficient> > >& outputSortedByArgumentWeight,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  Vector<Rational> targetWeight;
  outputSortedByArgumentWeight.SetSize(this->theGeneratingWordsGrouppedByWeight.size);
  ElementUniversalEnveloping<coefficient> theElt;
  std::string generatorString = inputHomogeneous.ToString();
  ProgressReport theReport;
  for (int i = 0; i < this->theGeneratingWordsGrouppedByWeight.size; i ++) {
    List<MonomialUniversalEnveloping<coefficient> >& currentWordList = this->theGeneratingWordsGrouppedByWeight[i];
    List<ElementUniversalEnveloping<coefficient> >& outputCurrentList = outputSortedByArgumentWeight[i];
    outputCurrentList.SetSize(currentWordList.size);
    Vector<Rational>& currentWeight = this->theModuleWeightsSimpleCoords[i];
    targetWeight = currentWeight + weightUEEltSimpleCoords;
    //stOutput << "<br>target weight: " << targetWeight.ToString() << "="
    //<< currentWeight.ToString() << "+" << inputWeight.ToString();
    int theIndex = this->theModuleWeightsSimpleCoords.GetIndex(targetWeight);
    for (int j = 0; j < currentWordList.size; j ++) {
      std::stringstream progressStream;
      progressStream << "Computing action of " << generatorString << " on weight layer " << i + 1
      << " out of " << this->theGeneratingWordsGrouppedByWeight.size
      << ", word " << j + 1 << " out of " << currentWordList.size << "...";
      theReport.Report(progressStream.str());
      ElementUniversalEnveloping<coefficient>& currentOutputWord = outputCurrentList[j];
      if (theIndex == - 1) {
        currentOutputWord.MakeZero(*this->theAlgebras, this->indexAlgebra);
      } else {
        theElt = inputHomogeneous;
        theElt.MultiplyBy(currentWordList[j], theRingUnit);
        this->ExpressAsLinearCombinationHomogenousElement(theElt, currentOutputWord, theIndex, this->theHWDualCoordsBaseFielD, theRingUnit, theRingZero);
      }
      progressStream << " done!";
      theReport.Report(progressStream.str());
    }
  }
}

template <class coefficient>
void ElementTensorsGeneralizedVermas<coefficient>::Substitution(
  const PolynomialSubstitution<Rational>& theSub, ListReferences<ModuleSSalgebra<coefficient> >& theMods
) {
  ElementTensorsGeneralizedVermas<coefficient> output;
  MonomialTensorGeneralizedVermas<coefficient> currentMon;
  output.MakeZero();
  coefficient tempCF;
  for (int i = 0; i < this->size(); i ++) {
    currentMon = (*this)[i];
    //stOutput << "<br>currentMon before sub: " << currentMon.ToString();
    currentMon.Substitution(theSub, theMods);
    //stOutput << "<br>currentMon after sub: " << currentMon.ToString();
    tempCF = this->theCoeffs[i];
    //stOutput << "<br>cf before sub: " << tempCF.ToString();
    tempCF.Substitution(theSub);
    //stOutput << "<br>cf after sub: " << tempCF.ToString();
    output.AddMonomial(currentMon, tempCF);
  }
  *this = output;
}

template <class coefficient>
void ElementTensorsGeneralizedVermas<coefficient>::operator=(const ElementSumGeneralizedVermas<coefficient>& other) {
  this->MakeZero();
  MonomialTensorGeneralizedVermas<coefficient> theMon;
  for (int i = 0; i < other.size(); i ++) {
    theMon = other[i];
    this->AddMonomial(theMon, other.theCoeffs[i]);
  }
}

template <class coefficient>
bool ElementTensorsGeneralizedVermas<coefficient>::MultiplyOnTheLeft(
  const ElementUniversalEnveloping<coefficient>& theUE,
  ElementTensorsGeneralizedVermas<coefficient>& output,
  SemisimpleLieAlgebra& ownerAlgebra,
  const coefficient& theRingUnit
) const {
  ElementTensorsGeneralizedVermas<coefficient> buffer;
  output.MakeZero();
  for (int i = 0; i < theUE.size(); i ++) {
    if (!this->MultiplyOnTheLeft(theUE[i], buffer, ownerAlgebra, theRingUnit)) {
      ElementSumGeneralizedVermas<coefficient> tempOutput;
      stOutput << "<hr>emergency mode!";
      for (int j = 0; j < this->size(); j ++) {
        const MonomialTensorGeneralizedVermas<coefficient>& currentMon = (*this)[j];
        if (currentMon.theMons.size != 1) {
          return false;
        }
        MonomialGeneralizedVerma<coefficient>& currentSingleMon = currentMon.theMons[0];
        if (j == 0)
          tempOutput.MakeZero();
        tempOutput.AddMonomial(currentSingleMon, this->theCoeffs[j]);
      }
      tempOutput.MultiplyMeByUEEltOnTheLeft(theUE);
      output = tempOutput;
      return true;
    }
    //int commentmewhendone;
    //theUE.theCoeffs[i].checkConsistency();
    //buffer.GrandMasterConsistencyCheck();
    //buffer.checkConsistency();
    for (int k = 0; k < buffer.theCoeffs.size; k ++) {
      std::string debugString = buffer.theCoeffs[k].ToString();
      //buffer.theCoeffs[k].checkConsistency();
    }
    buffer *= theUE.theCoeffs[i];
    //stOutput << "<br>and your beloved buffer, after being multiplied by " << theUE.theCoeffs[i].ToString() << " equals " << buffer.ToString();
    output += buffer;
  }
  return true;
  //for (int i = 0; i < theOwner.size; i ++)
  //{ //stOutput << "<hr><hr>Module" << i + 1 << "<br>" << theOwner[i].ToString();
  //}
}

template <class coefficient>
void ElementTensorsGeneralizedVermas<coefficient>::MakeHWV(
  ModuleSSalgebra<coefficient>& theOwner, const coefficient& theRingUnit
) {
  MonomialTensorGeneralizedVermas<coefficient> tensorMon;
  coefficient currentCoeff;
  currentCoeff = theRingUnit;
  tensorMon.theMons.SetSize(1);
  MonomialGeneralizedVerma<coefficient>& theMon = tensorMon.theMons[0];
  theMon.indexFDVector = theOwner.theGeneratingWordsNonReduced.size - 1;
  theMon.MakeConst(theOwner);
  this->MakeZero();
  this->AddMonomial(tensorMon, theRingUnit);
}

template <class coefficient>
bool ElementTensorsGeneralizedVermas<coefficient>::MultiplyOnTheLeft(
  const MonomialUniversalEnveloping<coefficient>& theUE,
  ElementTensorsGeneralizedVermas<coefficient>& output,
  SemisimpleLieAlgebra& ownerAlgebra,
  const coefficient& theRingUnit
) const {
  MacroRegisterFunctionWithName("ElementTensorsGeneralizedVermas<coefficient>::MultiplyOnTheLeft");
  if (&output == this)
    crash << "Output equals input, this is not supposed to happen. " << crash;
  //int commentmewhendone;
  //static int problemCounter = 0;
  //problemCounter ++;
  //output.checkConsistency();
  //std::string debugString;
  //if (problemCounter ==44)
  //{ debugString= this->ToString();
  //}
  //this->checkConsistency();
  ////
  output = *this;
  ////
  //output.checkConsistency();
  ElementTensorsGeneralizedVermas<coefficient> buffer;
  for (int i = theUE.Powers.size - 1; i >= 0; i --) {
    int thePower;
    if (!theUE.Powers[i].IsSmallInteger(&thePower))
      return false;
    int theIndex = theUE.generatorsIndices[i];
    for (int j = 0; j < thePower; j ++) {
      //stOutput << "<hr>Acting by generator index " << theIndex << " on " << output.ToString();
      output.MultiplyByElementLieAlg(buffer, ownerAlgebra, theIndex, theRingUnit);
      //buffer.checkConsistency();
      output = buffer;
      //int commentmewhendone4;
      //output.checkConsistency();
      //stOutput << "<br>to get: " << output.ToString();
    }
  }
  //int commentmewhendone2;
  //output.checkConsistency();
  return true;
}

template <class coefficient>
void ElementTensorsGeneralizedVermas<coefficient>::MultiplyByElementLieAlg(
  ElementTensorsGeneralizedVermas<coefficient>& output,
  SemisimpleLieAlgebra& ownerAlgebra,
  int indexGenerator,
  const coefficient& theRingUnit
) const {
  output.MakeZero();
  MonomialTensorGeneralizedVermas<coefficient> accumMon, monActedOn;
  ElementSumGeneralizedVermas<coefficient> tempElt;
  ElementUniversalEnveloping<coefficient> theGenerator;
  theGenerator.MakeOneGenerator(indexGenerator, ownerAlgebra, theRingUnit);
  //FormatExpressions tempFormat;
  //tempFormat.MakeAlphabetArbitraryWithIndex("g", "h");
  coefficient currentCoeff;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialTensorGeneralizedVermas<coefficient>& currentMon = (*this)[i];
    accumMon.theMons.SetSize(0);
    for (int j = 0; j < currentMon.theMons.size; j ++) {
      tempElt.MakeZero();
      tempElt.AddMonomial(currentMon.theMons[j], theRingUnit);
      //stOutput << "<hr> Acting by " << theGenerator.ToString() << " on " << tempElt.ToString() << "<br>";
      tempElt.MultiplyMeByUEEltOnTheLeft(theGenerator);
      for (int k = 0; k < tempElt.size(); k ++) {
        currentCoeff = this->theCoeffs[i];
        //stOutput << "<br>coeff: " << currentCoeff.ToString() << " times " << tempElt.theCoeffs[k].ToString() << " equals ";
        currentCoeff *= tempElt.theCoeffs[k];
        //stOutput << currentCoeff.ToString();
        monActedOn = accumMon;
        monActedOn *= tempElt[k];
        for (int l = j + 1; l < currentMon.theMons.size; l ++)
          monActedOn *= currentMon.theMons[l];
        //stOutput << "<br>Adding: " << currentCoeff.ToString() << " times " << monActedOn.ToString() << " to " << output.ToString();
        output.AddMonomial(monActedOn, currentCoeff);
        //stOutput << "<br>to get " << output.ToString();
      }
      accumMon*= currentMon.theMons[j];
    }
  }
}

template <class coefficient>
void ElementTensorsGeneralizedVermas<coefficient>::TensorOnTheRight(
  const ElementTensorsGeneralizedVermas<coefficient>& right
) {
  MacroRegisterFunctionWithName("ElementTensorsGeneralizedVermas<coefficient>::TensorOnTheRight");
  if (right.IsEqualToZero()) {
    this->MakeZero();
    return;
  }
  int maxNumMonsFinal = this->size() * right.size();
  ElementTensorsGeneralizedVermas<coefficient> output;
  MonomialTensorGeneralizedVermas<coefficient> bufferMon;
  output.MakeZero();
  output.SetExpectedSize(maxNumMonsFinal);
  coefficient theCoeff;
  for (int i = 0; i < right.size(); i ++) {
    for (int j = 0; j < this->size(); j ++) {
      bufferMon = (*this)[j];
      bufferMon *= right[i];
      theCoeff = this->theCoeffs[j];
      theCoeff *= right.theCoeffs[i];
      output.AddMonomial(bufferMon, theCoeff);
      ParallelComputing::SafePointDontCallMeFromDestructors();
    }
  }
  *this = output;
}

#endif
