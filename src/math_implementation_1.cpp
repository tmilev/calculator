// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_extra_finite_groups_implementation.h"
#include "general_lists.h"
#include "math_general.h"
#include "math_extra_universal_enveloping.h"

static ProjectInformationInstance projectInfoMathImplementation1CPP(__FILE__, "Math routines implementation. ");

void SemisimpleLieAlgebra::GetChevalleyGeneratorAsLieBracketsSimpleGens(
  int generatorIndex,
  List<int>& outputIndicesFormatAd0Ad1Ad2etc,
  Rational& outputMultiplyLieBracketsToGetGenerator
) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::GetChevalleyGeneratorAsLieBracketsSimpleGens");
  outputIndicesFormatAd0Ad1Ad2etc.size = 0;
  if (this->IsGeneratorFromCartan(generatorIndex)) {
    int simpleIndex = generatorIndex - this->GetNumPosRoots();
    outputIndicesFormatAd0Ad1Ad2etc.AddOnTop(generatorIndex + this->GetRank());
    outputIndicesFormatAd0Ad1Ad2etc.AddOnTop(2 * this->GetNumPosRoots() - 1 - generatorIndex);
    outputMultiplyLieBracketsToGetGenerator = this->theWeyl.CartanSymmetric.elements[simpleIndex][simpleIndex] / 2;
    return;
  }
  Vector<Rational> theWeight = this->GetWeightOfGenerator(generatorIndex);
  outputMultiplyLieBracketsToGetGenerator = 1;
  Vector<Rational> genWeight, newWeight;
  while (!theWeight.IsEqualToZero()) {
    for (int i = 0; i < this->GetRank(); i ++) {
      genWeight.MakeEi(this->GetRank(), i);
      if (theWeight.IsPositive()) {
        genWeight.Minus();
      }
      newWeight = theWeight + genWeight;
      if (newWeight.IsEqualToZero() || this->theWeyl.IsARoot(newWeight)) {
        theWeight = newWeight;
        int theIndex = this->GetGeneratorFromRoot(- genWeight);
        outputIndicesFormatAd0Ad1Ad2etc.AddOnTop(theIndex);
        if (!theWeight.IsEqualToZero()) {
          int currentIndex = this->theWeyl.RootSystem.GetIndex(theWeight);
          theIndex = this->GetRootIndexFromGenerator(theIndex);
          if (!this->Computed.elements[theIndex][currentIndex]) {
            global.fatal << "This is a programming error. "
            << "For some reason I am not computed. Here is me: " << this->ToString() << global.fatal;
          }
          outputMultiplyLieBracketsToGetGenerator /= this->ChevalleyConstants.elements[theIndex][currentIndex];
        }
        break;
      }
    }
  }
}

bool PartFractions::ArgumentsAllowed(Vectors<Rational>& theArguments, std::string& outputWhatWentWrong) {
  if (theArguments.size < 1) {
    return false;
  }
  Cone tempCone;
  bool result = tempCone.CreateFromVertices(theArguments);
  if (tempCone.IsTheEntireSpace()) {
    outputWhatWentWrong = "Error: the vectors you gave as input span the entire space.";
    return false;
  }
  for (int i = 0; i < tempCone.Vertices.size; i ++) {
    if (tempCone.IsInCone(tempCone.Vertices[i]) && tempCone.IsInCone(- tempCone.Vertices[i])) {
      std::stringstream out;
      out << "Error: the Q_{>0} span of vectors you gave as input contains zero (as it contains the vector "
      << tempCone.Vertices[i].ToString() << " as well as its opposite vector "
      << (- tempCone.Vertices[i]).ToString()
      << "), hence the vector partition function is " << "can only take values infinity or zero. ";
      outputWhatWentWrong = out.str();
      return false;
    }
  }
  return result;
}

void Lattice::IntersectWithLineGivenBy(Vector<Rational>& inputLine, Vector<Rational>& outputGenerator) {
  Vectors<Rational> tempRoots;
  tempRoots.AddOnTop(inputLine);
  this->IntersectWithLinearSubspaceSpannedBy(tempRoots);
  if (this->basisRationalForm.NumRows > 1) {
    global.fatal << "This should not be possible. " << global.fatal;
  }
  if (this->basisRationalForm.NumRows == 0) {
    outputGenerator.MakeZero(inputLine.size);
  } else {
    this->basisRationalForm.GetVectorFromRow(0, outputGenerator);
  }
}

void LittelmannPath::ActByEFDisplayIndex(int displayIndex) {
  if (this->owner == nullptr) {
    global.fatal << " This is a programming error: LS path without initialized owner is begin acted upon. " << global.fatal;
  }
  if (displayIndex > 0) {
    this->ActByEalpha(displayIndex - 1);
  } else {
    this->ActByFalpha(- displayIndex - 1);
  }
}

void LittelmannPath::ActByEalpha(int indexAlpha) {
  if (this->owner == nullptr) {
    global.fatal << " This is a programming error: LS path without initialized owner is begin acted upon. " << global.fatal;
  }
  if (indexAlpha < 0 || indexAlpha >= this->owner->GetDim()) {
    global.fatal << " This is a programming error: index of Littelmann root operator out of range. " << global.fatal;
  }
  if (this->Waypoints.size == 0) {
    return;
  }
  Rational theMin = 0;
  int minIndex = - 1;
  if (this->owner == nullptr) {
    global.fatal << "Zero owner not allowed here. " << global.fatal;
  }
  WeylGroupData& theWeyl = *this->owner;
  theWeyl.ComputeRho(true);
  Vector<Rational>& alpha = theWeyl.RootsOfBorel[indexAlpha];
  Rational LengthAlpha = theWeyl.RootScalarCartanRoot(alpha, alpha);
  Vector<Rational> alphaScaled = alpha * 2 / LengthAlpha;
  for (int i = 0; i < this->Waypoints.size; i ++) {
    Rational tempRat = this->owner->RootScalarCartanRoot(this->Waypoints[i], alphaScaled);
    if (tempRat <= theMin) {
      theMin = tempRat;
      minIndex = i;
    }
  }
  if (minIndex <= 0 || theMin > - 1) {
    this->Waypoints.size = 0;
    return;
  }
  int precedingIndex = 0;
  for (int i = 0; i <= minIndex; i ++) {
    Rational tempScalar = theWeyl.RootScalarCartanRoot(this->Waypoints[i], alphaScaled);
    if (tempScalar >= theMin + 1) {
      precedingIndex = i;
    }
    if (tempScalar < theMin + 1) {
      break;
    }
  }
  Rational s2 = this->owner->RootScalarCartanRoot(this->Waypoints[precedingIndex], alphaScaled);
  if (!this->MinimaAreIntegral()) {
    global.Comments << "<br>Something is wrong: starting path is BAD!";
  }
  if (s2 > theMin + 1) {
    this->Waypoints.SetSize(this->Waypoints.size + 1);
    for (int i = this->Waypoints.size - 1; i >= precedingIndex + 2; i --) {
      this->Waypoints[i] = this->Waypoints[i - 1];
    }
    precedingIndex ++;
    minIndex ++;
    Vector<Rational>& r1 = this->Waypoints[precedingIndex];
    Vector<Rational>& r2 = this->Waypoints[precedingIndex - 1];
    Rational s1 = theWeyl.RootScalarCartanRoot(r1, alphaScaled);
    Rational x = (theMin + 1 - s2) / (s1 - s2);
    this->Waypoints[precedingIndex] = (r1 - r2) * x + r2;
  }
  Vectors<Rational> differences;
  differences.SetSize(minIndex-precedingIndex);
  Rational currentDist = 0;
  Rational minDist = 0;
  for (int i = 0; i < differences.size; i ++) {
    differences[i] = this->Waypoints[i + precedingIndex + 1] - this->Waypoints[i + precedingIndex];
    currentDist += theWeyl.RootScalarCartanRoot(differences[i], alphaScaled);
    if (currentDist < minDist) {
      theWeyl.SimpleReflection(indexAlpha, differences[i]);
      minDist = currentDist;
    }
  }
  for (int i = 0; i < differences.size; i ++) {
    this->Waypoints[i + precedingIndex + 1] = this->Waypoints[i + precedingIndex] + differences[i];
  }
  for (int i = minIndex + 1; i < this->Waypoints.size; i ++) {
    this->Waypoints[i] += alpha;
  }
  this->Simplify();
}

void LittelmannPath::ActByFalpha(int indexAlpha) {
  if (this->Waypoints.size == 0) {
    return;
  }
  if (this->owner == nullptr) {
    global.fatal << " This is a programming error: LS path without initialized owner is begin acted upon. " << global.fatal;
  }
  if (indexAlpha < 0 || indexAlpha >= this->owner->GetDim()) {
    global.fatal << " This is a programming error: index of Littelmann root operator out of range. " << global.fatal;
  }
  Rational theMin = 0;
  int minIndex = - 1;
  WeylGroupData& theWeyl = *this->owner;
  Vector<Rational>& alpha = theWeyl.RootsOfBorel[indexAlpha];
  Rational LengthAlpha = this->owner->RootScalarCartanRoot(alpha, alpha);
  Vector<Rational> alphaScaled = alpha * 2 / LengthAlpha;
  for (int i = 0; i < this->Waypoints.size; i ++) {
    Rational tempRat = this->owner->RootScalarCartanRoot(this->Waypoints[i], alphaScaled);
    if (tempRat <= theMin) {
      theMin = tempRat;
      minIndex = i;
    }
  }
  Rational lastScalar = this->owner->RootScalarCartanRoot(*this->Waypoints.LastObject(), alphaScaled);
  if (minIndex < 0 || lastScalar - theMin < 1) {
    this->Waypoints.size = 0;
    return;
  }
  int succeedingIndex = 0;
  for (int i = this->Waypoints.size - 1; i >= minIndex; i --) {
    Rational tempScalar = theWeyl.RootScalarCartanRoot(alphaScaled, this->Waypoints[i]);
    if (tempScalar >= theMin + 1) {
      succeedingIndex = i;
    }
    if (tempScalar < theMin + 1) {
      break;
    }
  }
  Rational s1 = this->owner->RootScalarCartanRoot(this->Waypoints[succeedingIndex], alphaScaled);
  if (s1 > theMin + 1) {
    this->Waypoints.SetSize(this->Waypoints.size + 1);
    for (int i = this->Waypoints.size - 1; i >= succeedingIndex + 1; i --) {
      this->Waypoints[i] = this->Waypoints[i - 1];
    }
    //Rational scalarNext = theWeyl.RootScalarCartanRoot(this->Waypoints[succeedingIndex], alphaScaled);
    Vector<Rational>& r1 = this->Waypoints[succeedingIndex];
    Vector<Rational>& r2 = this->Waypoints[succeedingIndex - 1];
    Rational s2 = theWeyl.RootScalarCartanRoot(r2, alphaScaled);
    Rational x = (theMin + 1 - s2) / (s1 - s2);
    this->Waypoints[succeedingIndex] = (r1 - r2) * x + r2;
  }
  Vector<Rational> diff, oldWayPoint;
  oldWayPoint = this->Waypoints[minIndex];
  Rational currentDist = 0;
  for (int i = 0; i < succeedingIndex - minIndex; i ++) {
    diff = this->Waypoints[i + minIndex + 1] - oldWayPoint;
    currentDist += theWeyl.RootScalarCartanRoot(diff, alphaScaled);
    if (currentDist > 0) {
      theWeyl.SimpleReflection(indexAlpha, diff);
      currentDist = 0;
    }
    oldWayPoint = this->Waypoints[i + minIndex + 1];
    this->Waypoints[i + minIndex + 1] = this->Waypoints[i + minIndex] + diff;
  }
  for (int i = succeedingIndex + 1; i < this->Waypoints.size; i ++) {
    this->Waypoints[i] -= alpha;
  }
  this->Simplify();
}

void LittelmannPath::Simplify() {
  if (this->Waypoints.size == 0) {
    return;
  }
  Vector<Rational> d1, d2;
  Rational d11, d12, d22;
  int leftIndex = 0;
  int rightIndex = 2;
  while (rightIndex < this->Waypoints.size) {
    Vector<Rational>& left = this->Waypoints[leftIndex];
    Vector<Rational>& middle = this->Waypoints[rightIndex - 1];
    Vector<Rational>& right = this->Waypoints[rightIndex];
    d1 = left - middle;
    d2 = right - middle;
    d11 = d1.ScalarEuclidean(d1);
    d12 = d1.ScalarEuclidean(d2);
    d22 = d2.ScalarEuclidean(d2);
    bool isBad = ((d11 * d22 - d12 * d12).IsEqualToZero() && (d12 <= 0));
    if (!isBad) {
      leftIndex ++;
      this->Waypoints[leftIndex] = middle;
    }
    rightIndex ++;
  }
  leftIndex ++;
  this->Waypoints[leftIndex] = *this->Waypoints.LastObject();
/*  if (leftIndex + 1< this->Waypoints.size) {
    this->Waypoints.SetSize(leftIndex + 1);
    tempStream << " reduced to " << this->ToString();
    global.Comments << tempStream.str();
  }*/
  this->Waypoints.SetSize(leftIndex + 1);
}

bool LittelmannPath::MinimaAreIntegral() {
  if (this->Waypoints.size == 0) {
    return true;
  }
  List<Rational> theMinima;
  WeylGroupData& theWeyl = *this->owner;
  int theDim = theWeyl.GetDim();
  theMinima.SetSize(theDim);
  for (int i = 0; i < theDim; i ++) {
    theMinima[i] = theWeyl.GetScalarProdSimpleRoot(this->Waypoints[0], i) * 2 / theWeyl.CartanSymmetric.elements[i][i];
  }
  for (int i = 1; i < this->Waypoints.size; i ++) {
    for (int j = 0; j < theDim; j ++) {
      theMinima[j] = MathRoutines::Minimum(theWeyl.GetScalarProdSimpleRoot(
        this->Waypoints[i], j) * 2 / theWeyl.CartanSymmetric.elements[j][j], theMinima[j]
      );
    }
  }
  for (int i = 0; i < theDim; i ++) {
    if (!theMinima[i].IsSmallInteger()) {
      return false;
    }
  }
  return true;
}

void LittelmannPath::MakeFromWeightInSimpleCoords(const Vector<Rational>& weightInSimpleCoords, WeylGroupData& theOwner) {
  this->owner = &theOwner;
  this->Waypoints.SetSize(2);
  this->Waypoints[0].MakeZero(theOwner.GetDim());
  this->Waypoints[1] = weightInSimpleCoords;
  this->Simplify();
}

std::string LittelmannPath::ElementToStringIndicesToCalculatorOutput(LittelmannPath& inputStartingPath, List<int>& input) {
  std::stringstream out;
  for (int i = input.size - 1; i >= 0; i --) {
    int displayIndex = input[i];
    if (displayIndex >= 0) {
      displayIndex ++;
    }
    out << "eAlpha(" << displayIndex << ", ";
  }
  out << "littelmann"
  << inputStartingPath.owner->GetFundamentalCoordinatesFromSimple(*inputStartingPath.Waypoints.LastObject()).ToString();
  for (int i = 0; i < input.size; i ++) {
    out << " ) ";
  }
  return out.str();
}

bool LittelmannPath::GenerateOrbit(
  List<LittelmannPath>& output,
  List<List<int> >& outputOperators,
  int UpperBoundNumElts,
  Selection* parabolicNonSelectedAreInLeviPart
) {
  HashedList<LittelmannPath> hashedOutput;
  hashedOutput.AddOnTop(*this);
  int theDim = this->owner->GetDim();
  outputOperators.SetSize(1);
  outputOperators[0].SetSize(0);
  List<int> currentSequence;
  if (UpperBoundNumElts > 0) {
    currentSequence.Reserve(UpperBoundNumElts);
  }
  LittelmannPath currentPath;
  bool result = true;
  Selection parabolicSelectionSelectedAreInLeviPart;
  parabolicSelectionSelectedAreInLeviPart.init(theDim);
  if (parabolicNonSelectedAreInLeviPart != nullptr) {
    parabolicSelectionSelectedAreInLeviPart = *parabolicNonSelectedAreInLeviPart;
    parabolicSelectionSelectedAreInLeviPart.InvertSelection();
  } else {
    parabolicSelectionSelectedAreInLeviPart.MakeFullSelection();
  }
  for (int lowestNonExplored = 0; lowestNonExplored < hashedOutput.size; lowestNonExplored ++) {
    if (UpperBoundNumElts > 0 && UpperBoundNumElts < hashedOutput.size) {
      result = false;
      break;
    } else {
      for (int j = 0; j < parabolicSelectionSelectedAreInLeviPart.CardinalitySelection; j ++) {
        bool found = true;
        currentPath = hashedOutput[lowestNonExplored];
        currentSequence = outputOperators[lowestNonExplored];
        int theIndex = parabolicSelectionSelectedAreInLeviPart.elements[j];
        while (found) {
          found = false;
          currentPath.ActByEalpha(theIndex);
          if (!currentPath.IsEqualToZero()) {
            if (hashedOutput.AddOnTopNoRepetition(currentPath)) {
              found = true;
              currentSequence.AddOnTop(theIndex);
              outputOperators.AddOnTop(currentSequence);
              if (!currentPath.MinimaAreIntegral()) {
                global.Comments << "<hr>Found a bad path:<br> ";
                global.Comments << " = " << currentPath.ToString();
              }
            }
          }
        }
        found = true;
        currentPath = hashedOutput[lowestNonExplored];
        currentSequence = outputOperators[lowestNonExplored];
        while (found) {
          found = false;
          currentPath.ActByFalpha(theIndex);
          if (!currentPath.IsEqualToZero()) {
            if (hashedOutput.AddOnTopNoRepetition(currentPath)) {
              found = true;
              currentSequence.AddOnTop(- theIndex - 1);
              outputOperators.AddOnTop(currentSequence);
              if (!currentPath.MinimaAreIntegral()) {
                global.Comments << "<hr>Found a bad path:<br> ";
                global.Comments << " = " << currentPath.ToString();
              }
            }
          }
        }
      }
    }
  }
  output = hashedOutput;
  return result;
}

std::string LittelmannPath:: ElementToStringOperatorSequenceStartingOnMe(List<int>& input) {
  MonomialTensor<Rational> tempMon;
  tempMon = input;
  tempMon.generatorsIndices.ReverseOrderElements();
  tempMon.Powers.ReverseOrderElements();
  return tempMon.ToString();
}

template <class coefficient>
bool MonomialUniversalEnvelopingOrdered<coefficient>::ModOutFDRelationsExperimental(
  const Vector<Rational>& theHWsimpleCoords,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  WeylGroupData& theWeyl = this->owner->theOwner->theWeyl;
  Vector<Rational> theHWsimpleCoordsTrue = theHWsimpleCoords;
  theWeyl.RaiseToDominantWeight(theHWsimpleCoordsTrue);
  Vector<Rational> theHWdualCoords = theWeyl.GetDualCoordinatesFromFundamental(
    theWeyl.GetFundamentalCoordinatesFromSimple(theHWsimpleCoordsTrue)
  );
  List<coefficient> theSub;
  theSub.SetSize(theHWdualCoords.size);
  for (int i = 0; i < theHWdualCoords.size; i ++) {
    theSub[i] = theHWdualCoords[i];
  }
  this->ModOutVermaRelations(&theSub, theRingUnit, theRingZero);
  int numPosRoots = this->owner->theOwner->GetNumPosRoots();
  Vector<Rational> currentWeight = theHWsimpleCoordsTrue;
  Vector<Rational> testWeight;
  for (int k = this->generatorsIndices.size - 1; k >= 0; k --) {
    int IndexCurrentGenerator = this->generatorsIndices[k];
    if (IndexCurrentGenerator >= numPosRoots) {
      return false;
    }
    ElementSemisimpleLieAlgebra<Rational>& currentElt = this->owner->theOrder[IndexCurrentGenerator];
    if (!currentElt.GetCartanPart().IsEqualToZero() || currentElt.size() > 1) {
      return false;
    }
    int thePower = 0;
    if (!this->Powers[k].IsSmallInteger(thePower)) {
      return false;
    }
    int rootIndex = this->owner->theOwner->GetRootIndexFromGenerator(currentElt[0].theGeneratorIndex);
    const Vector<Rational>& currentRoot = theWeyl.RootSystem[rootIndex];
    for (int j = 0; j < thePower; j ++) {
      currentWeight += currentRoot;
      testWeight = currentWeight;
      theWeyl.RaiseToDominantWeight(testWeight);
      if (!(theHWsimpleCoordsTrue - testWeight).IsPositiveOrZero()) {
        this->MakeZero(theRingZero, *this->owner);
        return true;
      }
    }
  }
  return true;
}

template <class coefficient>
bool ElementUniversalEnvelopingOrdered<coefficient>::ModOutFDRelationsExperimental(
  const Vector<Rational>& theHWsimpleCoords,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
  ElementUniversalEnvelopingOrdered<coefficient> output;
  output.MakeZero(*this->owner);
  bool result = true;
  for (int i = 0; i < this->size; i ++) {
    tempMon = this->TheObjects[i];
    if (!tempMon.ModOutFDRelationsExperimental(theHWsimpleCoords, theRingUnit, theRingZero)) {
      result = false;
    }
    output.AddMonomial(tempMon);
  }
  this->operator=(output);
  return result;
}

template <class coefficient>
bool ElementUniversalEnveloping<coefficient>::GetCoordsInBasis(
  List<ElementUniversalEnveloping<coefficient> >& theBasis,
  Vector<coefficient>& output,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) const {
  List<ElementUniversalEnveloping<coefficient> > tempBasis, tempElts;
  tempBasis = theBasis;
  tempBasis.AddOnTop(*this);
  Vectors<coefficient> tempCoords;
  if (!this->GetBasisFromSpanOfElements(tempBasis, tempCoords, tempElts, theRingUnit, theRingZero)) {
    return false;
  }
  Vector<coefficient> tempRoot;
  tempRoot = *tempCoords.LastObject();
  tempCoords.SetSize(theBasis.size);
  return tempRoot.GetCoordsInBasiS(tempCoords, output);
}

template<class coefficient>
template<class CoefficientTypeQuotientField>
bool ElementUniversalEnveloping<coefficient>::GetBasisFromSpanOfElements(
  List<ElementUniversalEnveloping<coefficient> >& theElements,
  Vectors<CoefficientTypeQuotientField>& outputCoords,
  List<ElementUniversalEnveloping<coefficient> >& outputTheBasis,
  const CoefficientTypeQuotientField& theFieldUnit,
  const CoefficientTypeQuotientField& theFieldZero
) {
  if (theElements.size == 0) {
    return false;
  }
  ElementUniversalEnveloping<coefficient> outputCorrespondingMonomials;
  outputCorrespondingMonomials.MakeZero(*theElements[0].owner);
  Vectors<CoefficientTypeQuotientField> outputCoordsBeforeReduction;
  for (int i = 0; i < theElements.size; i ++) {
    for (int j = 0; j < theElements[i].size; j ++) {
      outputCorrespondingMonomials.AddOnTopNoRepetition(theElements[i][j]);
    }
  }
  outputCoordsBeforeReduction.SetSize(theElements.size);
  for (int i = 0; i < theElements.size; i ++) {
    Vector<CoefficientTypeQuotientField>& currentList = outputCoordsBeforeReduction[i];
    currentList.MakeZero(outputCorrespondingMonomials.size);
    ElementUniversalEnveloping<coefficient>& currentElt = theElements[i];
    for (int j = 0; j < currentElt.size; j ++) {
      MonomialUniversalEnveloping<coefficient>& currentMon = currentElt[j];
      currentList[outputCorrespondingMonomials.GetIndex(currentMon)] = currentMon.Coefficient;
    }
  }
  outputTheBasis.size = 0;
  outputTheBasis.Reserve(theElements.size);
  Vectors<CoefficientTypeQuotientField> basisCoordForm;
  basisCoordForm.Reserve(theElements.size);
  Selection selectedBasis;
  outputCoordsBeforeReduction.SelectABasis(basisCoordForm, theFieldZero, selectedBasis);
  for (int i = 0; i < selectedBasis.CardinalitySelection; i ++) {
    outputTheBasis.AddOnTop(theElements.TheObjects[selectedBasis.elements[i]]);
  }
  Matrix<coefficient> bufferMat;
  Vectors<coefficient> bufferVectors;
  outputCoordsBeforeReduction.GetCoordsInBasis(
    basisCoordForm, outputCoords, bufferVectors, bufferMat, theFieldUnit, theFieldZero
  );
  return true;
}

template<class coefficient>
void ElementUniversalEnveloping<coefficient>::ModToMinDegreeFormFDRels(
  const Vector<Rational>& theHWinSimpleCoords,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  ElementUniversalEnveloping<coefficient> result;
  result.MakeZero(*this->owner);
  bool Found = true;
  int numPosRoots = this->owner->GetNumPosRoots();
  while (Found) {
    Found = false;
    for (int j = numPosRoots - 1; j >= 0; j --) {
      this->owner->UEGeneratorOrderIncludingCartanElts.SwapTwoIndices(j, numPosRoots - 1);
      this->Simplify(theRingUnit);
      this->owner->UEGeneratorOrderIncludingCartanElts.SwapTwoIndices(j, numPosRoots - 1);
      if (this->ModOutFDRelationsExperimental(theHWinSimpleCoords, theRingUnit, theRingZero)) {
        Found = true;
      }
    }
  }
  this->Simplify(theRingUnit);
}

template<class coefficient>
bool ElementUniversalEnveloping<coefficient>::ApplyMinusTransposeAutoOnMe() {
  MonomialUniversalEnveloping<coefficient> tempMon;
  ElementUniversalEnveloping<coefficient> result;
  result.MakeZero(*this->owner);
  int numPosRoots = this->GetOwner().GetNumPosRoots();
  int theRank = this->GetOwner().GetRank();
  coefficient theCoeff;
  for (int i = 0; i < this->size; i ++) {
    MonomialUniversalEnveloping<coefficient>& currentMon = this->TheObjects[i];
    theCoeff = this->theCoeffs[i];
    tempMon.owner = currentMon.owner;
    tempMon.Powers.size = 0;
    tempMon.generatorsIndices.size = 0;
    for (int j = 0; j < currentMon.Powers.size; j ++) {
      int thePower;
      if (!currentMon.Powers[j].IsSmallInteger(&thePower)) {
        return false;
      }
      int theGenerator = currentMon.generatorsIndices[j];
      if (theGenerator < numPosRoots) {
        theGenerator = 2 * numPosRoots + theRank - 1 - theGenerator;
      } else if (theGenerator >= numPosRoots + theRank) {
        theGenerator = - theGenerator + 2 * numPosRoots + theRank - 1;
      }
      tempMon.MultiplyByGeneratorPowerOnTheRight(theGenerator, currentMon.Powers[j]);
      if (thePower % 2 == 1) {
        theCoeff *= - 1;
      }
    }
    result.AddMonomial(tempMon, theCoeff);
  }
  *this = result;
  return true;
}

template <class coefficient>
bool ElementUniversalEnveloping<coefficient>::HWMTAbilinearForm(
  const ElementUniversalEnveloping<coefficient>& right,
  coefficient& output,
  const Vector<coefficient>* subHiGoesToIthElement,
  const coefficient& theRingUnit,
  const coefficient& theRingZero,
  std::stringstream* logStream
) {
  output = theRingZero;
  ElementUniversalEnveloping<coefficient> MTright;
  MTright = right;
  if (!MTright.ApplyMinusTransposeAutoOnMe()) {
    return false;
  }
  ElementUniversalEnveloping<coefficient> Accum, intermediateAccum, tempElt;
  Accum.MakeZero(*this->owners, this->indexInOwners);
  MonomialUniversalEnveloping<coefficient> constMon;
  constMon.MakeConst();
  if (logStream != nullptr) {
    *logStream << "backtraced elt: " << MTright.ToString(&global.theDefaultFormat.GetElement()) << "<br>";
    *logStream << "this element: " << this->ToString(&global.theDefaultFormat.GetElement()) << "<br>";
  }
  for (int j = 0; j < right.size; j ++) {
    intermediateAccum = *this;
    intermediateAccum.Simplify(global, theRingUnit, theRingZero);
    if (logStream != nullptr) {
      *logStream << "intermediate after simplification: "
      << intermediateAccum.ToString(&global.theDefaultFormat.GetElement()) << "<br>";
    }
    intermediateAccum.ModOutVermaRelations(&global, subHiGoesToIthElement, theRingUnit, theRingZero);
    MonomialUniversalEnveloping<coefficient>& rightMon = MTright[j];
    coefficient& rightMonCoeff = MTright.theCoeffs[j];
    int thePower;
    for (int i = rightMon.Powers.size - 1; i >= 0; i --) {
      if (rightMon.Powers[i].IsSmallInteger(&thePower)) {
        for (int k = 0; k < thePower; k ++) {
          tempElt.MakeOneGenerator(rightMon.generatorsIndices[i], *this->owners, this->indexInOwners, theRingUnit);
          MathRoutines::swap(tempElt, intermediateAccum);
          if (logStream != nullptr) {
            *logStream << "tempElt before mult: " << tempElt.ToString(&global.theDefaultFormat) << "<br>";
            *logStream << "intermediate before mult: "
            << intermediateAccum.ToString(&global.theDefaultFormat.GetElement()) << "<br>";
          }
          intermediateAccum *= (tempElt);
          if (logStream != nullptr) {
            *logStream << "intermediate before simplification: "
            << intermediateAccum.ToString(&global.theDefaultFormat.GetElement()) << "<br>";
          }
          intermediateAccum.Simplify(theRingUnit);
          if (logStream != nullptr) {
            *logStream << "intermediate after simplification: "
            << intermediateAccum.ToString(&global.theDefaultFormat.GetElement()) << "<br>";
          }
          intermediateAccum.ModOutVermaRelations(subHiGoesToIthElement, theRingUnit, theRingZero);
          if (logStream != nullptr) {
            *logStream << "intermediate after Verma rels: "
            << intermediateAccum.ToString(&global.theDefaultFormat.GetElement()) << "<br>";
          }
        }
      } else {
        return false;
      }
    }
    intermediateAccum *= rightMonCoeff;
    Accum += intermediateAccum;
    int theIndex = intermediateAccum.GetIndex(constMon);
    if (theIndex != - 1) {
      output += intermediateAccum.theCoeffs[theIndex];
    }
  }
  if (logStream != nullptr) {
    *logStream << "final UE element: " << Accum.ToString(&global.theDefaultFormat.GetElement());
  }
  return true;
}

template <class coefficient>
std::string ElementUniversalEnveloping<coefficient>::IsInProperSubmodule(
  const Vector<coefficient>* subHiGoesToIthElement, const coefficient& theRingUnit, const coefficient& theRingZero
) {
  std::stringstream out;
  List<ElementUniversalEnveloping<coefficient> > theOrbit;
  theOrbit.Reserve(1000);
  ElementUniversalEnveloping<coefficient> theElt;
  int theDim = this->GetOwner().GetRank();
  int numPosRoots = this->GetOwner().GetNumPosRoots();
  theOrbit.AddOnTop(*this);
  for (int i = 0; i < theOrbit.size; i ++) {
    for (int j = 0; j < theDim; j ++) {
      theElt.MakeOneGenerator(j + numPosRoots + theDim, *this->owner, theRingUnit);
      theElt *= theOrbit[i];
      theElt.Simplify(theRingUnit);
      theElt.ModOutVermaRelations(subHiGoesToIthElement, theRingUnit, theRingZero);
      if (!theElt.IsEqualToZero()) {
        theOrbit.AddOnTop(theElt);
      }
    }
  }
  for (int i = 0; i < theOrbit.size; i ++) {
    ElementUniversalEnveloping<coefficient>& current = theOrbit[i];
    out << "<br>" << current.ToString(&global.theDefaultFormat.GetElement());
  }
  return out.str();
}

template <class coefficient>
bool ElementUniversalEnveloping<coefficient>::ConvertToRationalCoeff(ElementUniversalEnveloping<Rational>& output) {
  output.MakeZero(*this->owner);
  MonomialUniversalEnveloping<Rational> tempMon;
  Rational theCoeff;
  for (int i = 0; i < this->size; i ++) {
    MonomialUniversalEnveloping<coefficient>& currentMon = this->TheObjects[i];
    tempMon.MakeOne(*this->owner);
    if (!this->theCoeffs[i].IsConstant(theCoeff)) {
      return false;
    }
    for (int j = 0; j < currentMon.Powers.size; j ++) {
      Rational tempRat;
      if (!currentMon.Powers[j].IsConstant(tempRat)) {
        return false;
      }
      tempMon.MultiplyByGeneratorPowerOnTheRight(currentMon.generatorsIndices[j], tempRat);
    }
    output += tempMon;
  }
  return true;
}

void branchingData::initAssumingParSelAndHmmInittedPart1NoSubgroups() {
  MacroRegisterFunctionWithName("branchingData::initAssumingParSelAndHmmInittedPart1NoSubgroups");
  this->WeylFDSmallAsSubInLarge.AmbientWeyl = &this->theHmm.theRange().theWeyl;
  this->WeylFDSmall.AmbientWeyl = &this->theHmm.theDomain().theWeyl;
  this->WeylFD.AmbientWeyl = &this->theHmm.theRange().theWeyl;
  this->selSmallParSel.init(WeylFDSmall.AmbientWeyl->GetDim());
  for (int i = 0; i < this->theHmm.ImagesCartanDomain.size; i ++) {
    Vector<Rational>& currentV = this->theHmm.ImagesCartanDomain[i];
    this->generatorsSmallSub.AddOnTop(currentV);
    for (int j = 0; j < currentV.size; j ++) {
      if (!currentV[j].IsEqualToZero() && this->selInducing.selected[j]) {
        this->generatorsSmallSub.RemoveLastObject();
        this->selSmallParSel.AddSelectionAppendNewIndex(i);
        break;
      }
    }
  }
  this->NilModPreNil.SetSize(0);
  this->nilradicalSmall.SetSize(0);
  this->nilradicalLarge.SetSize(0);
  this->weightsNilradicalLarge.SetSize(0);
  this->weightsNilradicalSmall.SetSize(0);
  this->weightsNilModPreNil.SetSize(0);
  this->indicesNilradicalLarge.SetSize(0);
  this->indicesNilradicalSmall.SetSize(0);
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  WeylGroupData& theLargeWeyl = this->theHmm.theRange().theWeyl;
  WeylGroupData& theSmallWeyl = this->theHmm.theDomain().theWeyl;
  int numB3NegGenerators = this->theHmm.theRange().GetNumPosRoots();
  int numG2NegGenerators = this->theHmm.theDomain().GetNumPosRoots();
  for (int i = 0; i < numB3NegGenerators; i ++) {
    const Vector<Rational>& currentWeight = theLargeWeyl.RootSystem[i];
    bool isInNilradical = false;
    for (int k = 0; k < this->selInducing.CardinalitySelection; k ++) {
      if (!currentWeight[this->selInducing.elements[k]].IsEqualToZero()) {
        isInNilradical = true;
        break;
      }
    }
    if (isInNilradical) {
      this->weightsNilradicalLarge.AddOnTop(currentWeight);
      tempElt.MakeGenerator(i, this->theHmm.theRange());
      this->nilradicalLarge.AddOnTop(tempElt);
      this->indicesNilradicalLarge.AddOnTop(i);
    }
  }
  for (int i = 0; i < numG2NegGenerators; i ++) {
    const Vector<Rational>& currentWeight = theSmallWeyl.RootSystem[i];
    bool isInNilradical = false;
    for (int k = 0; k < this->selSmallParSel.CardinalitySelection; k ++) {
      if (!currentWeight[this->selSmallParSel.elements[k]].IsEqualToZero()) {
        isInNilradical = true;
        break;
      }
    }
    if (isInNilradical) {
      this->weightsNilradicalSmall.AddOnTop(currentWeight);
      tempElt.MakeGenerator(i, this->theHmm.theDomain());
      this->nilradicalSmall.AddOnTop(tempElt);
      this->indicesNilradicalSmall.AddOnTop(i);
    }
  }
  this->NilModPreNil = this->nilradicalLarge;
  this->weightsNilModPreNil = this->weightsNilradicalLarge;
  Vector<Rational> proj;
  for (int i = 0; i < this->nilradicalSmall.size; i ++) {
    ElementSemisimpleLieAlgebra<Rational>& eltImage =
    this->theHmm.imagesAllChevalleyGenerators[this->indicesNilradicalSmall[i]];
    int theIndex = this->NilModPreNil.GetIndex(eltImage);
    if (theIndex != - 1) {
      this->NilModPreNil.RemoveIndexSwapWithLast(theIndex);
      this->weightsNilModPreNil.RemoveIndexSwapWithLast(theIndex);
      continue;
    }
    bool isGood = false;
    for (int j = 0; j < this->weightsNilModPreNil.size; j ++) {
      proj = this->ProjectWeight(this->weightsNilModPreNil[j]);
      if (proj == this->weightsNilradicalSmall[i]) {
        isGood = true;
        this->NilModPreNil.RemoveIndexSwapWithLast(j);
        this->weightsNilModPreNil.RemoveIndexSwapWithLast(j);
        break;
      }
    }
    if (!isGood) {
      global.fatal << "This is either a programming error, or Lemma 3.3, T. Milev, P. Somberg, \"On branching...\""
      << " is wrong. The question is, which is the more desirable case... The bad apple is element "
      << this->nilradicalSmall[i].ToString() << " of weight "
      << this->weightsNilradicalSmall[i].ToString() << ". " << global.fatal;
    }
  }
}

branchingData::branchingData() {
  this->flagUseNilWeightGeneratorOrder = false;
  this->flagAscendingGeneratorOrder = false;
}

void branchingData::initAssumingParSelAndHmmInittedPart2Subgroups() {
  List<Vectors<Rational> > emptyList;
  this->WeylFDSmallAsSubInLarge.ComputeSubGroupFromGeneratingReflections(&this->generatorsSmallSub, &emptyList, 1000, true);
  this->WeylFDSmall.MakeParabolicFromSelectionSimpleRoots(
    *this->WeylFDSmall.AmbientWeyl, this->selSmallParSel, 1000
  );
  this->WeylFD.MakeParabolicFromSelectionSimpleRoots(this->theHmm.theRange().theWeyl, this->selInducing, 1000);
  this->WeylFD.ComputeRootSubsystem();
  this->WeylFDSmallAsSubInLarge.ComputeRootSubsystem();
  this->WeylFDSmall.ComputeRootSubsystem();
}

std::string branchingData::GetStringCasimirProjector(int theIndex, const Rational& additionalMultiple) {
  Vector<RationalFunctionOld> weightDifference;
  std::stringstream formulaStream1;
  HashedList<Vector<RationalFunctionOld> > accountedDiffs;
  accountedDiffs.SetExpectedSize(this->g2Weights.size);
  bool found = false;
  for (int i = 0; i < this->g2Weights.size; i ++) {
    weightDifference = this->g2Weights[i] - this->g2Weights[theIndex];
    if (weightDifference.IsPositive() && !accountedDiffs.Contains(weightDifference)) {
      accountedDiffs.AddOnTop(weightDifference);
      if (additionalMultiple != 1) {
        formulaStream1 << additionalMultiple.ToString(&this->theFormat);
      }
      formulaStream1 << "(i(\\bar c) - (" << this->theChars[i].ToString(&this->theFormat) << "))";
      found = true;
    }
  }
  if (!found) {
    formulaStream1 << "id";
  }
  return formulaStream1.str();
}

bool LittelmannPath::IsAdaptedString(MonomialTensor<int, MathRoutines::IntUnsignIdentity>& theString) {
  LittelmannPath tempPath = *this;
  LittelmannPath tempPath2;
  for (int i = 0; i < theString.generatorsIndices.size; i ++) {
    for (int k = 0; k < theString.Powers[i]; k ++) {
      tempPath.ActByEalpha(- theString.generatorsIndices[i] - 1);
    }
    if (tempPath.IsEqualToZero()) {
      return false;
    }
    tempPath2 = tempPath;
    tempPath2.ActByEalpha(- theString.generatorsIndices[i] - 1);
    if (!tempPath2.IsEqualToZero()) {
      return false;
    }
  }
  return true;
}

void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::GetGroupElementsIndexedAsAmbientGroup(
  List<ElementWeylGroup>& output
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::GetGroupElementsIndexedAsAmbientGroup");
  if (this->ExternalAutomorphisms.size > 0) {
    global.fatal << "This is  a programming error: a function meant for subgroups that are "
    << "Weyl groups of Levi parts of parabolics "
    << "is called on a subgroup that is not of that type. "
    << global.fatal;
  }
  output.Reserve(this->allElements.size);
  output.SetSize(0);
  ElementWeylGroup currentOutput;
  currentOutput.owner = this->AmbientWeyl;
  Vector<int> indexShifts;
  indexShifts.SetSize(this->simpleRootsInner.size);
  for (int i = 0; i < this->simpleRootsInner.size; i ++) {
    indexShifts[i] = this->simpleRootsInner[i].GetIndexFirstNonZeroCoordinate();
  }
  for (int i = 0; i < this->allElements.size; i ++) {
    const ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& other = this->allElements[i];
    currentOutput.generatorsLastAppliedFirst.SetSize(other.generatorsLastAppliedFirst.size);
    for (int j = 0; j < currentOutput.generatorsLastAppliedFirst.size; j ++) {
      currentOutput.generatorsLastAppliedFirst[j].index = indexShifts[other.generatorsLastAppliedFirst[j].index];
    }
    output.AddOnTop(currentOutput);
  }
}

std::string LittelmannPath::ToString(bool useSimpleCoords, bool useArrows, bool includeDominance) const {
  if (this->Waypoints.size == 0) {
    return "0";
  }
  std::stringstream out;
  for (int i = 0; i < this->Waypoints.size; i ++) {
    if (useSimpleCoords) {
      out << this->Waypoints[i].ToString();
    } else {
      out << this->owner->GetFundamentalCoordinatesFromSimple(this->Waypoints[i]).ToString();
    }
    if (i != this->Waypoints.size - 1) {
      if (useArrows) {
        out << "->";
      } else {
        out << ",";
      }
    }
  }
  if (includeDominance) {
    out << " ";
    for (int i = 0; i < this->owner->GetDim(); i ++) {
      LittelmannPath tempP = *this;
      tempP.ActByEFDisplayIndex(i + 1);
      if (!tempP.IsEqualToZero()) {
        out << "e_{" << i + 1 << "}";
      }
      tempP = *this;
      tempP.ActByEFDisplayIndex(- i - 1);
      if (!tempP.IsEqualToZero()) {
        out << "e_{" << - i - 1 << "},";
      }
    }
  }
  return out.str();
}
