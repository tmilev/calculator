// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_extra_finite_groups_implementation.h"
#include "general_lists.h"
#include "math_general.h"
#include "math_extra_universal_enveloping.h"

void SemisimpleLieAlgebra::getChevalleyGeneratorAsLieBracketsSimpleGenerators(
  int generatorIndex,
  List<int>& outputIndicesFormatAd0Ad1Ad2etc,
  Rational& outputMultiplyLieBracketsToGetGenerator
) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::getChevalleyGeneratorAsLieBracketsSimpleGenerators");
  outputIndicesFormatAd0Ad1Ad2etc.size = 0;
  if (this->isGeneratorFromCartan(generatorIndex)) {
    int simpleIndex = generatorIndex - this->getNumberOfPositiveRoots();
    outputIndicesFormatAd0Ad1Ad2etc.addOnTop(generatorIndex + this->getRank());
    outputIndicesFormatAd0Ad1Ad2etc.addOnTop(2 * this->getNumberOfPositiveRoots() - 1 - generatorIndex);
    outputMultiplyLieBracketsToGetGenerator = this->theWeyl.cartanSymmetric.elements[simpleIndex][simpleIndex] / 2;
    return;
  }
  Vector<Rational> theWeight = this->getWeightOfGenerator(generatorIndex);
  outputMultiplyLieBracketsToGetGenerator = 1;
  Vector<Rational> genWeight, newWeight;
  while (!theWeight.isEqualToZero()) {
    for (int i = 0; i < this->getRank(); i ++) {
      genWeight.makeEi(this->getRank(), i);
      if (theWeight.isPositive()) {
        genWeight.minus();
      }
      newWeight = theWeight + genWeight;
      if (newWeight.isEqualToZero() || this->theWeyl.isARoot(newWeight)) {
        theWeight = newWeight;
        int theIndex = this->getGeneratorFromRoot(- genWeight);
        outputIndicesFormatAd0Ad1Ad2etc.addOnTop(theIndex);
        if (!theWeight.isEqualToZero()) {
          int currentIndex = this->theWeyl.RootSystem.getIndex(theWeight);
          theIndex = this->getRootIndexFromGenerator(theIndex);
          if (!this->Computed.elements[theIndex][currentIndex]) {
            global.fatal << "This is a programming error. "
            << "For some reason I am not computed. Here is me: " << this->toString() << global.fatal;
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
      << tempCone.Vertices[i].toString() << " as well as its opposite vector "
      << (- tempCone.Vertices[i]).toString()
      << "), hence the vector partition function is " << "can only take values infinity or zero. ";
      outputWhatWentWrong = out.str();
      return false;
    }
  }
  return result;
}

void Lattice::IntersectWithLineGivenBy(Vector<Rational>& inputLine, Vector<Rational>& outputGenerator) {
  Vectors<Rational> tempRoots;
  tempRoots.addOnTop(inputLine);
  this->IntersectWithLinearSubspaceSpannedBy(tempRoots);
  if (this->basisRationalForm.numberOfRows > 1) {
    global.fatal << "This should not be possible. " << global.fatal;
  }
  if (this->basisRationalForm.numberOfRows == 0) {
    outputGenerator.makeZero(inputLine.size);
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
  if (indexAlpha < 0 || indexAlpha >= this->owner->getDimension()) {
    global.fatal << " This is a programming error: index of Littelmann root operator out of range. " << global.fatal;
  }
  if (this->Waypoints.size == 0) {
    return;
  }
  Rational theMin = 0;
  int minIndex = - 1;
  if (this->owner == nullptr) {
    global.fatal << "zero owner not allowed here. " << global.fatal;
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
    global.comments << "<br>Something is wrong: starting path is BAD!";
  }
  if (s2 > theMin + 1) {
    this->Waypoints.setSize(this->Waypoints.size + 1);
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
  differences.setSize(minIndex-precedingIndex);
  Rational currentDist = 0;
  Rational minDist = 0;
  for (int i = 0; i < differences.size; i ++) {
    differences[i] = this->Waypoints[i + precedingIndex + 1] - this->Waypoints[i + precedingIndex];
    currentDist += theWeyl.RootScalarCartanRoot(differences[i], alphaScaled);
    if (currentDist < minDist) {
      theWeyl.reflectSimple(indexAlpha, differences[i]);
      minDist = currentDist;
    }
  }
  for (int i = 0; i < differences.size; i ++) {
    this->Waypoints[i + precedingIndex + 1] = this->Waypoints[i + precedingIndex] + differences[i];
  }
  for (int i = minIndex + 1; i < this->Waypoints.size; i ++) {
    this->Waypoints[i] += alpha;
  }
  this->simplify();
}

void LittelmannPath::ActByFalpha(int indexAlpha) {
  if (this->Waypoints.size == 0) {
    return;
  }
  if (this->owner == nullptr) {
    global.fatal << " This is a programming error: LS path without initialized owner is begin acted upon. " << global.fatal;
  }
  if (indexAlpha < 0 || indexAlpha >= this->owner->getDimension()) {
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
  Rational lastScalar = this->owner->RootScalarCartanRoot(*this->Waypoints.lastObject(), alphaScaled);
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
    this->Waypoints.setSize(this->Waypoints.size + 1);
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
      theWeyl.reflectSimple(indexAlpha, diff);
      currentDist = 0;
    }
    oldWayPoint = this->Waypoints[i + minIndex + 1];
    this->Waypoints[i + minIndex + 1] = this->Waypoints[i + minIndex] + diff;
  }
  for (int i = succeedingIndex + 1; i < this->Waypoints.size; i ++) {
    this->Waypoints[i] -= alpha;
  }
  this->simplify();
}

void LittelmannPath::simplify() {
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
    bool isBad = ((d11 * d22 - d12 * d12).isEqualToZero() && (d12 <= 0));
    if (!isBad) {
      leftIndex ++;
      this->Waypoints[leftIndex] = middle;
    }
    rightIndex ++;
  }
  leftIndex ++;
  this->Waypoints[leftIndex] = *this->Waypoints.lastObject();
/*  if (leftIndex + 1< this->Waypoints.size) {
    this->Waypoints.setSize(leftIndex + 1);
    tempStream << " reduced to " << this->toString();
    global.Comments << tempStream.str();
  }*/
  this->Waypoints.setSize(leftIndex + 1);
}

bool LittelmannPath::MinimaAreIntegral() {
  if (this->Waypoints.size == 0) {
    return true;
  }
  List<Rational> theMinima;
  WeylGroupData& theWeyl = *this->owner;
  int theDim = theWeyl.getDimension();
  theMinima.setSize(theDim);
  for (int i = 0; i < theDim; i ++) {
    theMinima[i] = theWeyl.GetScalarProdSimpleRoot(this->Waypoints[0], i) * 2 / theWeyl.cartanSymmetric.elements[i][i];
  }
  for (int i = 1; i < this->Waypoints.size; i ++) {
    for (int j = 0; j < theDim; j ++) {
      theMinima[j] = MathRoutines::minimum(theWeyl.GetScalarProdSimpleRoot(
        this->Waypoints[i], j) * 2 / theWeyl.cartanSymmetric.elements[j][j], theMinima[j]
      );
    }
  }
  for (int i = 0; i < theDim; i ++) {
    if (!theMinima[i].isSmallInteger()) {
      return false;
    }
  }
  return true;
}

void LittelmannPath::MakeFromWeightInSimpleCoords(const Vector<Rational>& weightInSimpleCoords, WeylGroupData& theOwner) {
  this->owner = &theOwner;
  this->Waypoints.setSize(2);
  this->Waypoints[0].makeZero(theOwner.getDimension());
  this->Waypoints[1] = weightInSimpleCoords;
  this->simplify();
}

std::string LittelmannPath::ElementtoStringIndicesToCalculatorOutput(LittelmannPath& inputStartingPath, List<int>& input) {
  std::stringstream out;
  for (int i = input.size - 1; i >= 0; i --) {
    int displayIndex = input[i];
    if (displayIndex >= 0) {
      displayIndex ++;
    }
    out << "eAlpha(" << displayIndex << ", ";
  }
  out << "littelmann"
  << inputStartingPath.owner->getFundamentalCoordinatesFromSimple(*inputStartingPath.Waypoints.lastObject()).toString();
  for (int i = 0; i < input.size; i ++) {
    out << " ) ";
  }
  return out.str();
}

bool LittelmannPath::generateOrbit(
  List<LittelmannPath>& output,
  List<List<int> >& outputOperators,
  int UpperBoundNumElts,
  Selection* parabolicNonSelectedAreInLeviPart
) {
  HashedList<LittelmannPath> hashedOutput;
  hashedOutput.addOnTop(*this);
  int theDim = this->owner->getDimension();
  outputOperators.setSize(1);
  outputOperators[0].setSize(0);
  List<int> currentSequence;
  if (UpperBoundNumElts > 0) {
    currentSequence.reserve(UpperBoundNumElts);
  }
  LittelmannPath currentPath;
  bool result = true;
  Selection parabolicSelectionSelectedAreInLeviPart;
  parabolicSelectionSelectedAreInLeviPart.initialize(theDim);
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
      for (int j = 0; j < parabolicSelectionSelectedAreInLeviPart.cardinalitySelection; j ++) {
        bool found = true;
        currentPath = hashedOutput[lowestNonExplored];
        currentSequence = outputOperators[lowestNonExplored];
        int theIndex = parabolicSelectionSelectedAreInLeviPart.elements[j];
        while (found) {
          found = false;
          currentPath.ActByEalpha(theIndex);
          if (!currentPath.isEqualToZero()) {
            if (hashedOutput.addOnTopNoRepetition(currentPath)) {
              found = true;
              currentSequence.addOnTop(theIndex);
              outputOperators.addOnTop(currentSequence);
              if (!currentPath.MinimaAreIntegral()) {
                global.comments << "<hr>Found a bad path:<br> ";
                global.comments << " = " << currentPath.toString();
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
          if (!currentPath.isEqualToZero()) {
            if (hashedOutput.addOnTopNoRepetition(currentPath)) {
              found = true;
              currentSequence.addOnTop(- theIndex - 1);
              outputOperators.addOnTop(currentSequence);
              if (!currentPath.MinimaAreIntegral()) {
                global.comments << "<hr>Found a bad path:<br> ";
                global.comments << " = " << currentPath.toString();
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
  tempMon.generatorsIndices.reverseElements();
  tempMon.Powers.reverseElements();
  return tempMon.toString();
}

template <class Coefficient>
bool MonomialUniversalEnvelopingOrdered<Coefficient>::modOutFDRelationsExperimental(
  const Vector<Rational>& theHWsimpleCoords,
  const Coefficient& theRingUnit,
  const Coefficient& theRingZero
) {
  WeylGroupData& theWeyl = this->owner->theOwner->theWeyl;
  Vector<Rational> theHWsimpleCoordsTrue = theHWsimpleCoords;
  theWeyl.RaiseToDominantWeight(theHWsimpleCoordsTrue);
  Vector<Rational> theHWdualCoords = theWeyl.GetDualCoordinatesFromFundamental(
    theWeyl.getFundamentalCoordinatesFromSimple(theHWsimpleCoordsTrue)
  );
  List<Coefficient> theSub;
  theSub.setSize(theHWdualCoords.size);
  for (int i = 0; i < theHWdualCoords.size; i ++) {
    theSub[i] = theHWdualCoords[i];
  }
  this->modOutVermaRelations(&theSub, theRingUnit, theRingZero);
  int numPosRoots = this->owner->theOwner->getNumberOfPositiveRoots();
  Vector<Rational> currentWeight = theHWsimpleCoordsTrue;
  Vector<Rational> testWeight;
  for (int k = this->generatorsIndices.size - 1; k >= 0; k --) {
    int IndexCurrentGenerator = this->generatorsIndices[k];
    if (IndexCurrentGenerator >= numPosRoots) {
      return false;
    }
    ElementSemisimpleLieAlgebra<Rational>& currentElt = this->owner->theOrder[IndexCurrentGenerator];
    if (!currentElt.GetCartanPart().isEqualToZero() || currentElt.size() > 1) {
      return false;
    }
    int thePower = 0;
    if (!this->Powers[k].isSmallInteger(thePower)) {
      return false;
    }
    int rootIndex = this->owner->theOwner->getRootIndexFromGenerator(currentElt[0].theGeneratorIndex);
    const Vector<Rational>& currentRoot = theWeyl.RootSystem[rootIndex];
    for (int j = 0; j < thePower; j ++) {
      currentWeight += currentRoot;
      testWeight = currentWeight;
      theWeyl.RaiseToDominantWeight(testWeight);
      if (!(theHWsimpleCoordsTrue - testWeight).isPositiveOrZero()) {
        this->makeZero(theRingZero, *this->owner);
        return true;
      }
    }
  }
  return true;
}

template <class Coefficient>
bool ElementUniversalEnvelopingOrdered<Coefficient>::modOutFDRelationsExperimental(
  const Vector<Rational>& theHWsimpleCoords,
  const Coefficient& theRingUnit,
  const Coefficient& theRingZero
) {
  MonomialUniversalEnvelopingOrdered<Coefficient> tempMon;
  ElementUniversalEnvelopingOrdered<Coefficient> output;
  output.makeZero(*this->owner);
  bool result = true;
  for (int i = 0; i < this->size; i ++) {
    tempMon = this->theObjects[i];
    if (!tempMon.modOutFDRelationsExperimental(theHWsimpleCoords, theRingUnit, theRingZero)) {
      result = false;
    }
    output.addMonomial(tempMon);
  }
  this->operator=(output);
  return result;
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::getCoordinatesInBasis(
  List<ElementUniversalEnveloping<Coefficient> >& theBasis,
  Vector<Coefficient>& output,
  const Coefficient& theRingUnit,
  const Coefficient& theRingZero
) const {
  List<ElementUniversalEnveloping<Coefficient> > tempBasis, tempElts;
  tempBasis = theBasis;
  tempBasis.addOnTop(*this);
  Vectors<Coefficient> tempCoords;
  if (!this->getBasisFromSpanOfElements(tempBasis, tempCoords, tempElts, theRingUnit, theRingZero)) {
    return false;
  }
  Vector<Coefficient> tempRoot;
  tempRoot = *tempCoords.lastObject();
  tempCoords.setSize(theBasis.size);
  return tempRoot.getCoordinatesInBasis(tempCoords, output);
}

template<class Coefficient>
template<class CoefficientTypeQuotientField>
bool ElementUniversalEnveloping<Coefficient>::getBasisFromSpanOfElements(
  List<ElementUniversalEnveloping<Coefficient> >& theElements,
  Vectors<CoefficientTypeQuotientField>& outputCoords,
  List<ElementUniversalEnveloping<Coefficient> >& outputTheBasis,
  const CoefficientTypeQuotientField& theFieldUnit,
  const CoefficientTypeQuotientField& theFieldZero
) {
  if (theElements.size == 0) {
    return false;
  }
  ElementUniversalEnveloping<Coefficient> outputCorrespondingMonomials;
  outputCorrespondingMonomials.makeZero(*theElements[0].owner);
  Vectors<CoefficientTypeQuotientField> outputCoordsBeforeReduction;
  for (int i = 0; i < theElements.size; i ++) {
    for (int j = 0; j < theElements[i].size; j ++) {
      outputCorrespondingMonomials.addOnTopNoRepetition(theElements[i][j]);
    }
  }
  outputCoordsBeforeReduction.setSize(theElements.size);
  for (int i = 0; i < theElements.size; i ++) {
    Vector<CoefficientTypeQuotientField>& currentList = outputCoordsBeforeReduction[i];
    currentList.makeZero(outputCorrespondingMonomials.size);
    ElementUniversalEnveloping<Coefficient>& currentElt = theElements[i];
    for (int j = 0; j < currentElt.size; j ++) {
      MonomialUniversalEnveloping<Coefficient>& currentMon = currentElt[j];
      currentList[outputCorrespondingMonomials.getIndex(currentMon)] = currentMon.theCoefficient;
    }
  }
  outputTheBasis.size = 0;
  outputTheBasis.reserve(theElements.size);
  Vectors<CoefficientTypeQuotientField> basisCoordForm;
  basisCoordForm.reserve(theElements.size);
  Selection selectedBasis;
  outputCoordsBeforeReduction.SelectABasis(basisCoordForm, theFieldZero, selectedBasis);
  for (int i = 0; i < selectedBasis.cardinalitySelection; i ++) {
    outputTheBasis.addOnTop(theElements.theObjects[selectedBasis.elements[i]]);
  }
  Matrix<Coefficient> bufferMat;
  Vectors<Coefficient> bufferVectors;
  outputCoordsBeforeReduction.getCoordinatesInBasis(
    basisCoordForm, outputCoords, bufferVectors, bufferMat, theFieldUnit, theFieldZero
  );
  return true;
}

template<class Coefficient>
void ElementUniversalEnveloping<Coefficient>::modToMinDegreeFormFDRels(
  const Vector<Rational>& theHWinSimpleCoords,
  const Coefficient& theRingUnit,
  const Coefficient& theRingZero
) {
  ElementUniversalEnveloping<Coefficient> result;
  result.makeZero(*this->owner);
  bool Found = true;
  int numPosRoots = this->owner->getNumberOfPositiveRoots();
  while (Found) {
    Found = false;
    for (int j = numPosRoots - 1; j >= 0; j --) {
      this->owner->UEGeneratorOrderIncludingCartanElts.swapTwoIndices(j, numPosRoots - 1);
      this->simplify(theRingUnit);
      this->owner->UEGeneratorOrderIncludingCartanElts.swapTwoIndices(j, numPosRoots - 1);
      if (this->modOutFDRelationsExperimental(theHWinSimpleCoords, theRingUnit, theRingZero)) {
        Found = true;
      }
    }
  }
  this->simplify(theRingUnit);
}

template<class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::applyMinusTransposeAutoOnMe() {
  MonomialUniversalEnveloping<Coefficient> tempMon;
  ElementUniversalEnveloping<Coefficient> result;
  result.makeZero(*this->owner);
  int numPosRoots = this->getOwner().getNumberOfPositiveRoots();
  int theRank = this->getOwner().getRank();
  Coefficient theCoeff;
  for (int i = 0; i < this->size; i ++) {
    MonomialUniversalEnveloping<Coefficient>& currentMon = this->theObjects[i];
    theCoeff = this->coefficients[i];
    tempMon.owner = currentMon.owner;
    tempMon.Powers.size = 0;
    tempMon.generatorsIndices.size = 0;
    for (int j = 0; j < currentMon.Powers.size; j ++) {
      int thePower;
      if (!currentMon.Powers[j].isSmallInteger(&thePower)) {
        return false;
      }
      int theGenerator = currentMon.generatorsIndices[j];
      if (theGenerator < numPosRoots) {
        theGenerator = 2 * numPosRoots + theRank - 1 - theGenerator;
      } else if (theGenerator >= numPosRoots + theRank) {
        theGenerator = - theGenerator + 2 * numPosRoots + theRank - 1;
      }
      tempMon.multiplyByGeneratorPowerOnTheRight(theGenerator, currentMon.Powers[j]);
      if (thePower % 2 == 1) {
        theCoeff *= - 1;
      }
    }
    result.addMonomial(tempMon, theCoeff);
  }
  *this = result;
  return true;
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::HWMTAbilinearForm(
  const ElementUniversalEnveloping<Coefficient>& right,
  Coefficient& output,
  const Vector<Coefficient>* subHiGoesToIthElement,
  const Coefficient& theRingUnit,
  const Coefficient& theRingZero,
  std::stringstream* logStream
) {
  output = theRingZero;
  ElementUniversalEnveloping<Coefficient> MTright;
  MTright = right;
  if (!MTright.applyMinusTransposeAutoOnMe()) {
    return false;
  }
  ElementUniversalEnveloping<Coefficient> Accum, intermediateAccum, tempElt;
  Accum.makeZero(*this->owners, this->indexInOwners);
  MonomialUniversalEnveloping<Coefficient> constMon;
  constMon.makeConstant();
  if (logStream != nullptr) {
    *logStream << "backtraced elt: " << MTright.toString(&global.theDefaultFormat.getElement()) << "<br>";
    *logStream << "this element: " << this->toString(&global.theDefaultFormat.getElement()) << "<br>";
  }
  for (int j = 0; j < right.size; j ++) {
    intermediateAccum = *this;
    intermediateAccum.simplify(global, theRingUnit, theRingZero);
    if (logStream != nullptr) {
      *logStream << "intermediate after simplification: "
      << intermediateAccum.toString(&global.theDefaultFormat.getElement()) << "<br>";
    }
    intermediateAccum.modOutVermaRelations(&global, subHiGoesToIthElement, theRingUnit, theRingZero);
    MonomialUniversalEnveloping<Coefficient>& rightMon = MTright[j];
    Coefficient& rightMonCoeff = MTright.coefficients[j];
    int thePower;
    for (int i = rightMon.Powers.size - 1; i >= 0; i --) {
      if (rightMon.Powers[i].isSmallInteger(&thePower)) {
        for (int k = 0; k < thePower; k ++) {
          tempElt.MakeOneGenerator(rightMon.generatorsIndices[i], *this->owners, this->indexInOwners, theRingUnit);
          MathRoutines::swap(tempElt, intermediateAccum);
          if (logStream != nullptr) {
            *logStream << "tempElt before mult: " << tempElt.toString(&global.theDefaultFormat) << "<br>";
            *logStream << "intermediate before mult: "
            << intermediateAccum.toString(&global.theDefaultFormat.getElement()) << "<br>";
          }
          intermediateAccum *= (tempElt);
          if (logStream != nullptr) {
            *logStream << "intermediate before simplification: "
            << intermediateAccum.toString(&global.theDefaultFormat.getElement()) << "<br>";
          }
          intermediateAccum.simplify(theRingUnit);
          if (logStream != nullptr) {
            *logStream << "intermediate after simplification: "
            << intermediateAccum.toString(&global.theDefaultFormat.getElement()) << "<br>";
          }
          intermediateAccum.modOutVermaRelations(subHiGoesToIthElement, theRingUnit, theRingZero);
          if (logStream != nullptr) {
            *logStream << "intermediate after Verma rels: "
            << intermediateAccum.toString(&global.theDefaultFormat.getElement()) << "<br>";
          }
        }
      } else {
        return false;
      }
    }
    intermediateAccum *= rightMonCoeff;
    Accum += intermediateAccum;
    int theIndex = intermediateAccum.getIndex(constMon);
    if (theIndex != - 1) {
      output += intermediateAccum.coefficients[theIndex];
    }
  }
  if (logStream != nullptr) {
    *logStream << "final UE element: " << Accum.toString(&global.theDefaultFormat.getElement());
  }
  return true;
}

template <class Coefficient>
std::string ElementUniversalEnveloping<Coefficient>::isInProperSubmodule(
  const Vector<Coefficient>* subHiGoesToIthElement, const Coefficient& theRingUnit, const Coefficient& theRingZero
) {
  std::stringstream out;
  List<ElementUniversalEnveloping<Coefficient> > theOrbit;
  theOrbit.reserve(1000);
  ElementUniversalEnveloping<Coefficient> theElt;
  int theDim = this->getOwner().getRank();
  int numPosRoots = this->getOwner().getNumberOfPositiveRoots();
  theOrbit.addOnTop(*this);
  for (int i = 0; i < theOrbit.size; i ++) {
    for (int j = 0; j < theDim; j ++) {
      theElt.MakeOneGenerator(j + numPosRoots + theDim, *this->owner, theRingUnit);
      theElt *= theOrbit[i];
      theElt.simplify(theRingUnit);
      theElt.modOutVermaRelations(subHiGoesToIthElement, theRingUnit, theRingZero);
      if (!theElt.isEqualToZero()) {
        theOrbit.addOnTop(theElt);
      }
    }
  }
  for (int i = 0; i < theOrbit.size; i ++) {
    ElementUniversalEnveloping<Coefficient>& current = theOrbit[i];
    out << "<br>" << current.toString(&global.theDefaultFormat.getElement());
  }
  return out.str();
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::convertToRationalCoefficient(ElementUniversalEnveloping<Rational>& output) {
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<Rational> tempMon;
  Rational theCoeff;
  for (int i = 0; i < this->size; i ++) {
    MonomialUniversalEnveloping<Coefficient>& currentMon = this->theObjects[i];
    tempMon.makeOne(*this->owner);
    if (!this->coefficients[i].isConstant(theCoeff)) {
      return false;
    }
    for (int j = 0; j < currentMon.Powers.size; j ++) {
      Rational tempRat;
      if (!currentMon.Powers[j].isConstant(tempRat)) {
        return false;
      }
      tempMon.multiplyByGeneratorPowerOnTheRight(currentMon.generatorsIndices[j], tempRat);
    }
    output.addMonomial(tempMon, Coefficient(1));
  }
  return true;
}

void BranchingData::initAssumingParSelAndHmmInittedPart1NoSubgroups() {
  MacroRegisterFunctionWithName("BranchingData::initAssumingParSelAndHmmInittedPart1NoSubgroups");
  this->WeylFDSmallAsSubInLarge.AmbientWeyl = &this->theHmm.theRange().theWeyl;
  this->WeylFDSmall.AmbientWeyl = &this->theHmm.theDomain().theWeyl;
  this->WeylFD.AmbientWeyl = &this->theHmm.theRange().theWeyl;
  this->selSmallParSel.initialize(WeylFDSmall.AmbientWeyl->getDimension());
  for (int i = 0; i < this->theHmm.ImagesCartanDomain.size; i ++) {
    Vector<Rational>& currentV = this->theHmm.ImagesCartanDomain[i];
    this->generatorsSmallSub.addOnTop(currentV);
    for (int j = 0; j < currentV.size; j ++) {
      if (!currentV[j].isEqualToZero() && this->selInducing.selected[j]) {
        this->generatorsSmallSub.removeLastObject();
        this->selSmallParSel.addSelectionAppendNewIndex(i);
        break;
      }
    }
  }
  this->NilModPreNil.setSize(0);
  this->nilradicalSmall.setSize(0);
  this->nilradicalLarge.setSize(0);
  this->weightsNilradicalLarge.setSize(0);
  this->weightsNilradicalSmall.setSize(0);
  this->weightsNilModPreNil.setSize(0);
  this->indicesNilradicalLarge.setSize(0);
  this->indicesNilradicalSmall.setSize(0);
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  WeylGroupData& theLargeWeyl = this->theHmm.theRange().theWeyl;
  WeylGroupData& theSmallWeyl = this->theHmm.theDomain().theWeyl;
  int numB3NegGenerators = this->theHmm.theRange().getNumberOfPositiveRoots();
  int numG2NegGenerators = this->theHmm.theDomain().getNumberOfPositiveRoots();
  for (int i = 0; i < numB3NegGenerators; i ++) {
    const Vector<Rational>& currentWeight = theLargeWeyl.RootSystem[i];
    bool isInNilradical = false;
    for (int k = 0; k < this->selInducing.cardinalitySelection; k ++) {
      if (!currentWeight[this->selInducing.elements[k]].isEqualToZero()) {
        isInNilradical = true;
        break;
      }
    }
    if (isInNilradical) {
      this->weightsNilradicalLarge.addOnTop(currentWeight);
      tempElt.makeGenerator(i, this->theHmm.theRange());
      this->nilradicalLarge.addOnTop(tempElt);
      this->indicesNilradicalLarge.addOnTop(i);
    }
  }
  for (int i = 0; i < numG2NegGenerators; i ++) {
    const Vector<Rational>& currentWeight = theSmallWeyl.RootSystem[i];
    bool isInNilradical = false;
    for (int k = 0; k < this->selSmallParSel.cardinalitySelection; k ++) {
      if (!currentWeight[this->selSmallParSel.elements[k]].isEqualToZero()) {
        isInNilradical = true;
        break;
      }
    }
    if (isInNilradical) {
      this->weightsNilradicalSmall.addOnTop(currentWeight);
      tempElt.makeGenerator(i, this->theHmm.theDomain());
      this->nilradicalSmall.addOnTop(tempElt);
      this->indicesNilradicalSmall.addOnTop(i);
    }
  }
  this->NilModPreNil = this->nilradicalLarge;
  this->weightsNilModPreNil = this->weightsNilradicalLarge;
  Vector<Rational> proj;
  for (int i = 0; i < this->nilradicalSmall.size; i ++) {
    ElementSemisimpleLieAlgebra<Rational>& eltImage =
    this->theHmm.imagesAllChevalleyGenerators[this->indicesNilradicalSmall[i]];
    int theIndex = this->NilModPreNil.getIndex(eltImage);
    if (theIndex != - 1) {
      this->NilModPreNil.removeIndexSwapWithLast(theIndex);
      this->weightsNilModPreNil.removeIndexSwapWithLast(theIndex);
      continue;
    }
    bool isGood = false;
    for (int j = 0; j < this->weightsNilModPreNil.size; j ++) {
      proj = this->projectWeight(this->weightsNilModPreNil[j]);
      if (proj == this->weightsNilradicalSmall[i]) {
        isGood = true;
        this->NilModPreNil.removeIndexSwapWithLast(j);
        this->weightsNilModPreNil.removeIndexSwapWithLast(j);
        break;
      }
    }
    if (!isGood) {
      global.fatal << "This is either a programming error, or Lemma 3.3, T. Milev, P. Somberg, \"On branching...\""
      << " is wrong. The question is, which is the more desirable case... The bad apple is element "
      << this->nilradicalSmall[i].toString() << " of weight "
      << this->weightsNilradicalSmall[i].toString() << ". " << global.fatal;
    }
  }
}

BranchingData::BranchingData() {
  this->flagUseNilWeightGeneratorOrder = false;
  this->flagAscendingGeneratorOrder = false;
}

void BranchingData::initAssumingParSelAndHmmInittedPart2Subgroups() {
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

std::string BranchingData::GetStringCasimirProjector(int theIndex, const Rational& additionalMultiple) {
  Vector<RationalFunction> weightDifference;
  std::stringstream formulaStream1;
  HashedList<Vector<RationalFunction> > accountedDiffs;
  accountedDiffs.setExpectedSize(this->g2Weights.size);
  bool found = false;
  for (int i = 0; i < this->g2Weights.size; i ++) {
    weightDifference = this->g2Weights[i] - this->g2Weights[theIndex];
    if (weightDifference.isPositive() && !accountedDiffs.contains(weightDifference)) {
      accountedDiffs.addOnTop(weightDifference);
      if (additionalMultiple != 1) {
        formulaStream1 << additionalMultiple.toString(&this->theFormat);
      }
      formulaStream1 << "(i(\\bar c) - (" << this->theChars[i].toString(&this->theFormat) << "))";
      found = true;
    }
  }
  if (!found) {
    formulaStream1 << "id";
  }
  return formulaStream1.str();
}

LittelmannPath::LittelmannPath() {
  this->owner = nullptr;
}

LittelmannPath::LittelmannPath(const LittelmannPath& other) {
  *this = other;
}

bool LittelmannPath::IsAdaptedString(MonomialTensor<int, MathRoutines::IntUnsignIdentity>& theString) {
  LittelmannPath tempPath = *this;
  LittelmannPath tempPath2;
  for (int i = 0; i < theString.generatorsIndices.size; i ++) {
    for (int k = 0; k < theString.Powers[i]; k ++) {
      tempPath.ActByEalpha(- theString.generatorsIndices[i] - 1);
    }
    if (tempPath.isEqualToZero()) {
      return false;
    }
    tempPath2 = tempPath;
    tempPath2.ActByEalpha(- theString.generatorsIndices[i] - 1);
    if (!tempPath2.isEqualToZero()) {
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
  output.reserve(this->allElements.size);
  output.setSize(0);
  ElementWeylGroup currentOutput;
  currentOutput.owner = this->AmbientWeyl;
  Vector<int> indexShifts;
  indexShifts.setSize(this->simpleRootsInner.size);
  for (int i = 0; i < this->simpleRootsInner.size; i ++) {
    indexShifts[i] = this->simpleRootsInner[i].getIndexFirstNonZeroCoordinate();
  }
  for (int i = 0; i < this->allElements.size; i ++) {
    const ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& other = this->allElements[i];
    currentOutput.generatorsLastAppliedFirst.setSize(other.generatorsLastAppliedFirst.size);
    for (int j = 0; j < currentOutput.generatorsLastAppliedFirst.size; j ++) {
      currentOutput.generatorsLastAppliedFirst[j].index = indexShifts[other.generatorsLastAppliedFirst[j].index];
    }
    output.addOnTop(currentOutput);
  }
}

std::string LittelmannPath::toString(bool useSimpleCoords, bool useArrows, bool includeDominance) const {
  if (this->Waypoints.size == 0) {
    return "0";
  }
  std::stringstream out;
  for (int i = 0; i < this->Waypoints.size; i ++) {
    if (useSimpleCoords) {
      out << this->Waypoints[i].toString();
    } else {
      out << this->owner->getFundamentalCoordinatesFromSimple(this->Waypoints[i]).toString();
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
    for (int i = 0; i < this->owner->getDimension(); i ++) {
      LittelmannPath tempP = *this;
      tempP.ActByEFDisplayIndex(i + 1);
      if (!tempP.isEqualToZero()) {
        out << "e_{" << i + 1 << "}";
      }
      tempP = *this;
      tempP.ActByEFDisplayIndex(- i - 1);
      if (!tempP.isEqualToZero()) {
        out << "e_{" << - i - 1 << "},";
      }
    }
  }
  return out.str();
}
