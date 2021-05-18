// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_extra_finite_groups_implementation.h"
#include "general_lists.h"
#include "math_general.h"
#include "math_extra_universal_enveloping.h"
#include "math_rational_function_implementation.h"

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
    outputMultiplyLieBracketsToGetGenerator = this->weylGroup.cartanSymmetric.elements[simpleIndex][simpleIndex] / 2;
    return;
  }
  Vector<Rational> theWeight = this->getWeightOfGenerator(generatorIndex);
  outputMultiplyLieBracketsToGetGenerator = 1;
  Vector<Rational> genWeight, newWeight;
  while (!theWeight.isEqualToZero()) {
    for (int i = 0; i < this->getRank(); i ++) {
      genWeight.makeEi(this->getRank(), i);
      if (theWeight.isPositive()) {
        genWeight.negate();
      }
      newWeight = theWeight + genWeight;
      if (newWeight.isEqualToZero() || this->weylGroup.isARoot(newWeight)) {
        theWeight = newWeight;
        int index = this->getGeneratorIndexFromRoot(- genWeight);
        outputIndicesFormatAd0Ad1Ad2etc.addOnTop(index);
        if (!theWeight.isEqualToZero()) {
          int currentIndex = this->weylGroup.rootSystem.getIndex(theWeight);
          index = this->getRootIndexFromGenerator(index);
          if (!this->computedChevalleyConstants.elements[index][currentIndex]) {
            global.fatal
            << "For some reason I am not computed. Here is me: " << this->toString() << global.fatal;
          }
          outputMultiplyLieBracketsToGetGenerator /= this->chevalleyConstants.elements[index][currentIndex];
        }
        break;
      }
    }
  }
}

bool PartialFractions::argumentsAllowed(Vectors<Rational>& theArguments, std::string& outputWhatWentWrong) {
  if (theArguments.size < 1) {
    return false;
  }
  Cone tempCone;
  bool result = tempCone.createFromVertices(theArguments);
  if (tempCone.isTheEntireSpace()) {
    outputWhatWentWrong = "Error: the vectors you gave as input span the entire space.";
    return false;
  }
  for (int i = 0; i < tempCone.vertices.size; i ++) {
    if (tempCone.isInCone(tempCone.vertices[i]) && tempCone.isInCone(- tempCone.vertices[i])) {
      std::stringstream out;
      out << "Error: the Q_{>0} span of vectors you gave as input contains zero (as it contains the vector "
      << tempCone.vertices[i].toString() << " as well as its opposite vector "
      << (- tempCone.vertices[i]).toString()
      << "), hence the vector partition function is " << "can only take values infinity or zero. ";
      outputWhatWentWrong = out.str();
      return false;
    }
  }
  return result;
}

void Lattice::intersectWithLineGivenBy(Vector<Rational>& inputLine, Vector<Rational>& outputGenerator) {
  Vectors<Rational> tempRoots;
  tempRoots.addOnTop(inputLine);
  this->intersectWithLinearSubspaceSpannedBy(tempRoots);
  if (this->basisRationalForm.numberOfRows > 1) {
    global.fatal << "This should not be possible. " << global.fatal;
  }
  if (this->basisRationalForm.numberOfRows == 0) {
    outputGenerator.makeZero(inputLine.size);
  } else {
    this->basisRationalForm.getVectorFromRow(0, outputGenerator);
  }
}

void LittelmannPath::actByEFDisplayIndex(int displayIndex) {
  if (this->owner == nullptr) {
    global.fatal << "LS path without initialized owner is begin acted upon. " << global.fatal;
  }
  if (displayIndex > 0) {
    this->actByEAlpha(displayIndex - 1);
  } else {
    this->actByFAlpha(- displayIndex - 1);
  }
}

void LittelmannPath::actByEAlpha(int indexAlpha) {
  if (this->owner == nullptr) {
    global.fatal << "LS path without initialized owner is begin acted upon. " << global.fatal;
  }
  if (indexAlpha < 0 || indexAlpha >= this->owner->getDimension()) {
    global.fatal << "Index of Littelmann root operator out of range. " << global.fatal;
  }
  if (this->waypoints.size == 0) {
    return;
  }
  Rational theMin = 0;
  int minIndex = - 1;
  if (this->owner == nullptr) {
    global.fatal << "zero owner not allowed here. " << global.fatal;
  }
  WeylGroupData& theWeyl = *this->owner;
  theWeyl.computeRho(true);
  Vector<Rational>& alpha = theWeyl.rootsOfBorel[indexAlpha];
  Rational LengthAlpha = theWeyl.rootScalarCartanRoot(alpha, alpha);
  Vector<Rational> alphaScaled = alpha * 2 / LengthAlpha;
  for (int i = 0; i < this->waypoints.size; i ++) {
    Rational tempRat = this->owner->rootScalarCartanRoot(this->waypoints[i], alphaScaled);
    if (tempRat <= theMin) {
      theMin = tempRat;
      minIndex = i;
    }
  }
  if (minIndex <= 0 || theMin > - 1) {
    this->waypoints.size = 0;
    return;
  }
  int precedingIndex = 0;
  for (int i = 0; i <= minIndex; i ++) {
    Rational tempScalar = theWeyl.rootScalarCartanRoot(this->waypoints[i], alphaScaled);
    if (tempScalar >= theMin + 1) {
      precedingIndex = i;
    }
    if (tempScalar < theMin + 1) {
      break;
    }
  }
  Rational s2 = this->owner->rootScalarCartanRoot(this->waypoints[precedingIndex], alphaScaled);
  if (!this->minimaAreIntegral()) {
    global.comments << "<br>Something is wrong: starting path is BAD!";
  }
  if (s2 > theMin + 1) {
    this->waypoints.setSize(this->waypoints.size + 1);
    for (int i = this->waypoints.size - 1; i >= precedingIndex + 2; i --) {
      this->waypoints[i] = this->waypoints[i - 1];
    }
    precedingIndex ++;
    minIndex ++;
    Vector<Rational>& r1 = this->waypoints[precedingIndex];
    Vector<Rational>& r2 = this->waypoints[precedingIndex - 1];
    Rational s1 = theWeyl.rootScalarCartanRoot(r1, alphaScaled);
    Rational x = (theMin + 1 - s2) / (s1 - s2);
    this->waypoints[precedingIndex] = (r1 - r2) * x + r2;
  }
  Vectors<Rational> differences;
  differences.setSize(minIndex-precedingIndex);
  Rational currentDist = 0;
  Rational minDist = 0;
  for (int i = 0; i < differences.size; i ++) {
    differences[i] = this->waypoints[i + precedingIndex + 1] - this->waypoints[i + precedingIndex];
    currentDist += theWeyl.rootScalarCartanRoot(differences[i], alphaScaled);
    if (currentDist < minDist) {
      theWeyl.reflectSimple(indexAlpha, differences[i]);
      minDist = currentDist;
    }
  }
  for (int i = 0; i < differences.size; i ++) {
    this->waypoints[i + precedingIndex + 1] = this->waypoints[i + precedingIndex] + differences[i];
  }
  for (int i = minIndex + 1; i < this->waypoints.size; i ++) {
    this->waypoints[i] += alpha;
  }
  this->simplify();
}

void LittelmannPath::actByFAlpha(int indexAlpha) {
  if (this->waypoints.size == 0) {
    return;
  }
  if (this->owner == nullptr) {
    global.fatal << "LS path without initialized owner is begin acted upon. " << global.fatal;
  }
  if (indexAlpha < 0 || indexAlpha >= this->owner->getDimension()) {
    global.fatal << "Index of Littelmann root operator out of range. " << global.fatal;
  }
  Rational theMin = 0;
  int minIndex = - 1;
  WeylGroupData& theWeyl = *this->owner;
  Vector<Rational>& alpha = theWeyl.rootsOfBorel[indexAlpha];
  Rational LengthAlpha = this->owner->rootScalarCartanRoot(alpha, alpha);
  Vector<Rational> alphaScaled = alpha * 2 / LengthAlpha;
  for (int i = 0; i < this->waypoints.size; i ++) {
    Rational tempRat = this->owner->rootScalarCartanRoot(this->waypoints[i], alphaScaled);
    if (tempRat <= theMin) {
      theMin = tempRat;
      minIndex = i;
    }
  }
  Rational lastScalar = this->owner->rootScalarCartanRoot(*this->waypoints.lastObject(), alphaScaled);
  if (minIndex < 0 || lastScalar - theMin < 1) {
    this->waypoints.size = 0;
    return;
  }
  int succeedingIndex = 0;
  for (int i = this->waypoints.size - 1; i >= minIndex; i --) {
    Rational tempScalar = theWeyl.rootScalarCartanRoot(alphaScaled, this->waypoints[i]);
    if (tempScalar >= theMin + 1) {
      succeedingIndex = i;
    }
    if (tempScalar < theMin + 1) {
      break;
    }
  }
  Rational s1 = this->owner->rootScalarCartanRoot(this->waypoints[succeedingIndex], alphaScaled);
  if (s1 > theMin + 1) {
    this->waypoints.setSize(this->waypoints.size + 1);
    for (int i = this->waypoints.size - 1; i >= succeedingIndex + 1; i --) {
      this->waypoints[i] = this->waypoints[i - 1];
    }
    //Rational scalarNext = theWeyl.rootScalarCartanRoot(this->waypoints[succeedingIndex], alphaScaled);
    Vector<Rational>& r1 = this->waypoints[succeedingIndex];
    Vector<Rational>& r2 = this->waypoints[succeedingIndex - 1];
    Rational s2 = theWeyl.rootScalarCartanRoot(r2, alphaScaled);
    Rational x = (theMin + 1 - s2) / (s1 - s2);
    this->waypoints[succeedingIndex] = (r1 - r2) * x + r2;
  }
  Vector<Rational> diff, oldWayPoint;
  oldWayPoint = this->waypoints[minIndex];
  Rational currentDist = 0;
  for (int i = 0; i < succeedingIndex - minIndex; i ++) {
    diff = this->waypoints[i + minIndex + 1] - oldWayPoint;
    currentDist += theWeyl.rootScalarCartanRoot(diff, alphaScaled);
    if (currentDist > 0) {
      theWeyl.reflectSimple(indexAlpha, diff);
      currentDist = 0;
    }
    oldWayPoint = this->waypoints[i + minIndex + 1];
    this->waypoints[i + minIndex + 1] = this->waypoints[i + minIndex] + diff;
  }
  for (int i = succeedingIndex + 1; i < this->waypoints.size; i ++) {
    this->waypoints[i] -= alpha;
  }
  this->simplify();
}

void LittelmannPath::simplify() {
  if (this->waypoints.size == 0) {
    return;
  }
  Vector<Rational> d1, d2;
  Rational d11, d12, d22;
  int leftIndex = 0;
  int rightIndex = 2;
  while (rightIndex < this->waypoints.size) {
    Vector<Rational>& left = this->waypoints[leftIndex];
    Vector<Rational>& middle = this->waypoints[rightIndex - 1];
    Vector<Rational>& right = this->waypoints[rightIndex];
    d1 = left - middle;
    d2 = right - middle;
    d11 = d1.scalarEuclidean(d1);
    d12 = d1.scalarEuclidean(d2);
    d22 = d2.scalarEuclidean(d2);
    bool isBad = ((d11 * d22 - d12 * d12).isEqualToZero() && (d12 <= 0));
    if (!isBad) {
      leftIndex ++;
      this->waypoints[leftIndex] = middle;
    }
    rightIndex ++;
  }
  leftIndex ++;
  this->waypoints[leftIndex] = *this->waypoints.lastObject();
  this->waypoints.setSize(leftIndex + 1);
}

bool LittelmannPath::minimaAreIntegral() {
  if (this->waypoints.size == 0) {
    return true;
  }
  List<Rational> theMinima;
  WeylGroupData& theWeyl = *this->owner;
  int theDim = theWeyl.getDimension();
  theMinima.setSize(theDim);
  for (int i = 0; i < theDim; i ++) {
    theMinima[i] = theWeyl.getScalarProductSimpleRoot(this->waypoints[0], i) * 2 / theWeyl.cartanSymmetric.elements[i][i];
  }
  for (int i = 1; i < this->waypoints.size; i ++) {
    for (int j = 0; j < theDim; j ++) {
      theMinima[j] = MathRoutines::minimum(theWeyl.getScalarProductSimpleRoot(
        this->waypoints[i], j) * 2 / theWeyl.cartanSymmetric.elements[j][j], theMinima[j]
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

void LittelmannPath::makeFromWeightInSimpleCoords(const Vector<Rational>& weightInSimpleCoords, WeylGroupData& theOwner) {
  this->owner = &theOwner;
  this->waypoints.setSize(2);
  this->waypoints[0].makeZero(theOwner.getDimension());
  this->waypoints[1] = weightInSimpleCoords;
  this->simplify();
}

std::string LittelmannPath::toStringIndicesToCalculatorOutput(LittelmannPath& inputStartingPath, List<int>& input) {
  std::stringstream out;
  for (int i = input.size - 1; i >= 0; i --) {
    int displayIndex = input[i];
    if (displayIndex >= 0) {
      displayIndex ++;
    }
    out << "eAlpha(" << displayIndex << ", ";
  }
  out << "littelmann"
  << inputStartingPath.owner->getFundamentalCoordinatesFromSimple(*inputStartingPath.waypoints.lastObject()).toString();
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
    parabolicSelectionSelectedAreInLeviPart.invertSelection();
  } else {
    parabolicSelectionSelectedAreInLeviPart.makeFullSelection();
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
        int index = parabolicSelectionSelectedAreInLeviPart.elements[j];
        while (found) {
          found = false;
          currentPath.actByEAlpha(index);
          if (!currentPath.isEqualToZero()) {
            if (hashedOutput.addOnTopNoRepetition(currentPath)) {
              found = true;
              currentSequence.addOnTop(index);
              outputOperators.addOnTop(currentSequence);
              if (!currentPath.minimaAreIntegral()) {
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
          currentPath.actByFAlpha(index);
          if (!currentPath.isEqualToZero()) {
            if (hashedOutput.addOnTopNoRepetition(currentPath)) {
              found = true;
              currentSequence.addOnTop(- index - 1);
              outputOperators.addOnTop(currentSequence);
              if (!currentPath.minimaAreIntegral()) {
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

std::string LittelmannPath:: toStringOperatorSequenceStartingOnMe(List<int>& input) {
  MonomialTensor<Rational> tempMon;
  tempMon = input;
  tempMon.generatorsIndices.reverseElements();
  tempMon.powers.reverseElements();
  return tempMon.toString();
}

template <class Coefficient>
bool MonomialUniversalEnvelopingOrdered<Coefficient>::modOutFDRelationsExperimental(
  const Vector<Rational>& theHWsimpleCoords,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  WeylGroupData& theWeyl = this->owner->theOwner->weylGroup;
  Vector<Rational> theHWsimpleCoordsTrue = theHWsimpleCoords;
  theWeyl.raiseToDominantWeight(theHWsimpleCoordsTrue);
  Vector<Rational> theHWdualCoords = theWeyl.getDualCoordinatesFromFundamental(
    theWeyl.getFundamentalCoordinatesFromSimple(theHWsimpleCoordsTrue)
  );
  List<Coefficient> theSub;
  theSub.setSize(theHWdualCoords.size);
  for (int i = 0; i < theHWdualCoords.size; i ++) {
    theSub[i] = theHWdualCoords[i];
  }
  this->modOutVermaRelations(&theSub, ringUnit, ringZero);
  int numPosRoots = this->owner->theOwner->getNumberOfPositiveRoots();
  Vector<Rational> currentWeight = theHWsimpleCoordsTrue;
  Vector<Rational> testWeight;
  for (int k = this->generatorsIndices.size - 1; k >= 0; k --) {
    int indexCurrentGenerator = this->generatorsIndices[k];
    if (indexCurrentGenerator >= numPosRoots) {
      return false;
    }
    ElementSemisimpleLieAlgebra<Rational>& currentElt = this->owner->theOrder[indexCurrentGenerator];
    if (!currentElt.getCartanPart().isEqualToZero() || currentElt.size() > 1) {
      return false;
    }
    int power = 0;
    if (!this->powers[k].isSmallInteger(power)) {
      return false;
    }
    int rootIndex = this->owner->theOwner->getRootIndexFromGenerator(currentElt[0].generatorIndex);
    const Vector<Rational>& currentRoot = theWeyl.rootSystem[rootIndex];
    for (int j = 0; j < power; j ++) {
      currentWeight += currentRoot;
      testWeight = currentWeight;
      theWeyl.raiseToDominantWeight(testWeight);
      if (!(theHWsimpleCoordsTrue - testWeight).isPositiveOrZero()) {
        this->makeZero(ringZero, *this->owner);
        return true;
      }
    }
  }
  return true;
}

template <class Coefficient>
bool ElementUniversalEnvelopingOrdered<Coefficient>::modOutFDRelationsExperimental(
  const Vector<Rational>& theHWsimpleCoords,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  MonomialUniversalEnvelopingOrdered<Coefficient> tempMon;
  ElementUniversalEnvelopingOrdered<Coefficient> output;
  output.makeZero(*this->owner);
  bool result = true;
  for (int i = 0; i < this->size; i ++) {
    tempMon = this->objects[i];
    if (!tempMon.modOutFDRelationsExperimental(theHWsimpleCoords, ringUnit, ringZero)) {
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
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) const {
  List<ElementUniversalEnveloping<Coefficient> > tempBasis, elements;
  tempBasis = theBasis;
  tempBasis.addOnTop(*this);
  Vectors<Coefficient> tempCoords;
  if (!this->getBasisFromSpanOfElements(tempBasis, tempCoords, elements, ringUnit, ringZero)) {
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
    outputTheBasis.addOnTop(theElements.objects[selectedBasis.elements[i]]);
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
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  ElementUniversalEnveloping<Coefficient> result;
  result.makeZero(*this->owner);
  bool Found = true;
  int numPosRoots = this->owner->getNumberOfPositiveRoots();
  while (Found) {
    Found = false;
    for (int j = numPosRoots - 1; j >= 0; j --) {
      this->owner->UEGeneratorOrderIncludingCartanElts.swapTwoIndices(j, numPosRoots - 1);
      this->simplify(ringUnit);
      this->owner->UEGeneratorOrderIncludingCartanElts.swapTwoIndices(j, numPosRoots - 1);
      if (this->modOutFDRelationsExperimental(theHWinSimpleCoords, ringUnit, ringZero)) {
        Found = true;
      }
    }
  }
  this->simplify(ringUnit);
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
    MonomialUniversalEnveloping<Coefficient>& currentMon = this->objects[i];
    theCoeff = this->coefficients[i];
    tempMon.owner = currentMon.owner;
    tempMon.powers.size = 0;
    tempMon.generatorsIndices.size = 0;
    for (int j = 0; j < currentMon.powers.size; j ++) {
      int power;
      if (!currentMon.powers[j].isSmallInteger(&power)) {
        return false;
      }
      int theGenerator = currentMon.generatorsIndices[j];
      if (theGenerator < numPosRoots) {
        theGenerator = 2 * numPosRoots + theRank - 1 - theGenerator;
      } else if (theGenerator >= numPosRoots + theRank) {
        theGenerator = - theGenerator + 2 * numPosRoots + theRank - 1;
      }
      tempMon.multiplyByGeneratorPowerOnTheRight(theGenerator, currentMon.powers[j]);
      if (power % 2 == 1) {
        theCoeff *= - 1;
      }
    }
    result.addMonomial(tempMon, theCoeff);
  }
  *this = result;
  return true;
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::highestWeightMTAbilinearForm(
  const ElementUniversalEnveloping<Coefficient>& right,
  Coefficient& output,
  const Vector<Coefficient>* substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero,
  std::stringstream* logStream
) {
  output = ringZero;
  ElementUniversalEnveloping<Coefficient> MTright;
  MTright = right;
  if (!MTright.applyMinusTransposeAutoOnMe()) {
    return false;
  }
  ElementUniversalEnveloping<Coefficient> Accum, intermediateAccum, element;
  Accum.makeZero(*this->owners, this->indexInOwners);
  MonomialUniversalEnveloping<Coefficient> constMon;
  constMon.makeConstant();
  if (logStream != nullptr) {
    *logStream << "backtraced elt: " << MTright.toString(&global.defaultFormat.getElement()) << "<br>";
    *logStream << "this element: " << this->toString(&global.defaultFormat.getElement()) << "<br>";
  }
  for (int j = 0; j < right.size; j ++) {
    intermediateAccum = *this;
    intermediateAccum.simplify(global, ringUnit, ringZero);
    if (logStream != nullptr) {
      *logStream << "intermediate after simplification: "
      << intermediateAccum.toString(&global.defaultFormat.getElement()) << "<br>";
    }
    intermediateAccum.modOutVermaRelations(&global, substitutionHiGoesToIthElement, ringUnit, ringZero);
    MonomialUniversalEnveloping<Coefficient>& rightMon = MTright[j];
    Coefficient& rightMonCoeff = MTright.coefficients[j];
    int power;
    for (int i = rightMon.powers.size - 1; i >= 0; i --) {
      if (rightMon.powers[i].isSmallInteger(&power)) {
        for (int k = 0; k < power; k ++) {
          element.makeOneGenerator(rightMon.generatorsIndices[i], *this->owners, this->indexInOwners, ringUnit);
          MathRoutines::swap(element, intermediateAccum);
          if (logStream != nullptr) {
            *logStream << "element before mult: " << element.toString(&global.defaultFormat) << "<br>";
            *logStream << "intermediate before mult: "
            << intermediateAccum.toString(&global.defaultFormat.getElement()) << "<br>";
          }
          intermediateAccum *= (element);
          if (logStream != nullptr) {
            *logStream << "intermediate before simplification: "
            << intermediateAccum.toString(&global.defaultFormat.getElement()) << "<br>";
          }
          intermediateAccum.simplify(ringUnit);
          if (logStream != nullptr) {
            *logStream << "intermediate after simplification: "
            << intermediateAccum.toString(&global.defaultFormat.getElement()) << "<br>";
          }
          intermediateAccum.modOutVermaRelations(substitutionHiGoesToIthElement, ringUnit, ringZero);
          if (logStream != nullptr) {
            *logStream << "intermediate after Verma rels: "
            << intermediateAccum.toString(&global.defaultFormat.getElement()) << "<br>";
          }
        }
      } else {
        return false;
      }
    }
    intermediateAccum *= rightMonCoeff;
    Accum += intermediateAccum;
    int index = intermediateAccum.getIndex(constMon);
    if (index != - 1) {
      output += intermediateAccum.coefficients[index];
    }
  }
  if (logStream != nullptr) {
    *logStream << "final UE element: " << Accum.toString(&global.defaultFormat.getElement());
  }
  return true;
}

template <class Coefficient>
std::string ElementUniversalEnveloping<Coefficient>::isInProperSubmodule(
  const Vector<Coefficient>* substitutionHiGoesToIthElement, const Coefficient& ringUnit, const Coefficient& ringZero
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
      theElt.makeOneGenerator(j + numPosRoots + theDim, *this->owner, ringUnit);
      theElt *= theOrbit[i];
      theElt.simplify(ringUnit);
      theElt.modOutVermaRelations(substitutionHiGoesToIthElement, ringUnit, ringZero);
      if (!theElt.isEqualToZero()) {
        theOrbit.addOnTop(theElt);
      }
    }
  }
  for (int i = 0; i < theOrbit.size; i ++) {
    ElementUniversalEnveloping<Coefficient>& current = theOrbit[i];
    out << "<br>" << current.toString(&global.defaultFormat.getElement());
  }
  return out.str();
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::convertToRationalCoefficient(ElementUniversalEnveloping<Rational>& output) {
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<Rational> tempMon;
  Rational theCoeff;
  for (int i = 0; i < this->size; i ++) {
    MonomialUniversalEnveloping<Coefficient>& currentMon = this->objects[i];
    tempMon.makeOne(*this->owner);
    if (!this->coefficients[i].isConstant(theCoeff)) {
      return false;
    }
    for (int j = 0; j < currentMon.powers.size; j ++) {
      Rational tempRat;
      if (!currentMon.powers[j].isConstant(tempRat)) {
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
  this->WeylFDSmallAsSubInLarge.ambientWeyl = &this->theHmm.range().weylGroup;
  this->WeylFDSmall.ambientWeyl = &this->theHmm.domain().weylGroup;
  this->WeylFD.ambientWeyl = &this->theHmm.range().weylGroup;
  this->selSmallParSel.initialize(WeylFDSmall.ambientWeyl->getDimension());
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
  ElementSemisimpleLieAlgebra<Rational> element;
  WeylGroupData& theLargeWeyl = this->theHmm.range().weylGroup;
  WeylGroupData& theSmallWeyl = this->theHmm.domain().weylGroup;
  int numB3NegGenerators = this->theHmm.range().getNumberOfPositiveRoots();
  int numG2NegGenerators = this->theHmm.domain().getNumberOfPositiveRoots();
  for (int i = 0; i < numB3NegGenerators; i ++) {
    const Vector<Rational>& currentWeight = theLargeWeyl.rootSystem[i];
    bool isInNilradical = false;
    for (int k = 0; k < this->selInducing.cardinalitySelection; k ++) {
      if (!currentWeight[this->selInducing.elements[k]].isEqualToZero()) {
        isInNilradical = true;
        break;
      }
    }
    if (isInNilradical) {
      this->weightsNilradicalLarge.addOnTop(currentWeight);
      element.makeGenerator(i, this->theHmm.range());
      this->nilradicalLarge.addOnTop(element);
      this->indicesNilradicalLarge.addOnTop(i);
    }
  }
  for (int i = 0; i < numG2NegGenerators; i ++) {
    const Vector<Rational>& currentWeight = theSmallWeyl.rootSystem[i];
    bool isInNilradical = false;
    for (int k = 0; k < this->selSmallParSel.cardinalitySelection; k ++) {
      if (!currentWeight[this->selSmallParSel.elements[k]].isEqualToZero()) {
        isInNilradical = true;
        break;
      }
    }
    if (isInNilradical) {
      this->weightsNilradicalSmall.addOnTop(currentWeight);
      element.makeGenerator(i, this->theHmm.domain());
      this->nilradicalSmall.addOnTop(element);
      this->indicesNilradicalSmall.addOnTop(i);
    }
  }
  this->NilModPreNil = this->nilradicalLarge;
  this->weightsNilModPreNil = this->weightsNilradicalLarge;
  Vector<Rational> proj;
  for (int i = 0; i < this->nilradicalSmall.size; i ++) {
    ElementSemisimpleLieAlgebra<Rational>& eltImage =
    this->theHmm.imagesAllChevalleyGenerators[this->indicesNilradicalSmall[i]];
    int index = this->NilModPreNil.getIndex(eltImage);
    if (index != - 1) {
      this->NilModPreNil.removeIndexSwapWithLast(index);
      this->weightsNilModPreNil.removeIndexSwapWithLast(index);
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
  this->WeylFDSmallAsSubInLarge.computeSubGroupFromGeneratingReflections(&this->generatorsSmallSub, &emptyList, 1000, true);
  this->WeylFDSmall.makeParabolicFromSelectionSimpleRoots(
    *this->WeylFDSmall.ambientWeyl, this->selSmallParSel, 1000
  );
  this->WeylFD.makeParabolicFromSelectionSimpleRoots(this->theHmm.range().weylGroup, this->selInducing, 1000);
  this->WeylFD.computeRootSubsystem();
  this->WeylFDSmallAsSubInLarge.computeRootSubsystem();
  this->WeylFDSmall.computeRootSubsystem();
}

std::string BranchingData::getStringCasimirProjector(int index, const Rational& additionalMultiple) {
  Vector<RationalFraction<Rational> > weightDifference;
  std::stringstream formulaStream1;
  HashedList<Vector<RationalFraction<Rational> > > accountedDiffs;
  accountedDiffs.setExpectedSize(this->g2Weights.size);
  bool found = false;
  for (int i = 0; i < this->g2Weights.size; i ++) {
    weightDifference = this->g2Weights[i] - this->g2Weights[index];
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

bool LittelmannPath::isAdaptedString(MonomialTensor<int, HashFunctions::hashFunction>& theString) {
  LittelmannPath tempPath = *this;
  LittelmannPath tempPath2;
  for (int i = 0; i < theString.generatorsIndices.size; i ++) {
    for (int k = 0; k < theString.powers[i]; k ++) {
      tempPath.actByEAlpha(- theString.generatorsIndices[i] - 1);
    }
    if (tempPath.isEqualToZero()) {
      return false;
    }
    tempPath2 = tempPath;
    tempPath2.actByEAlpha(- theString.generatorsIndices[i] - 1);
    if (!tempPath2.isEqualToZero()) {
      return false;
    }
  }
  return true;
}

void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::getGroupElementsIndexedAsAmbientGroup(
  List<ElementWeylGroup>& output
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::getGroupElementsIndexedAsAmbientGroup");
  if (this->ExternalAutomorphisms.size > 0) {
    global.fatal << "This is  a programming error: a function meant for subgroups that are "
    << "Weyl groups of Levi parts of parabolics "
    << "is called on a subgroup that is not of that type. "
    << global.fatal;
  }
  output.reserve(this->allElements.size);
  output.setSize(0);
  ElementWeylGroup currentOutput;
  currentOutput.owner = this->ambientWeyl;
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
  if (this->waypoints.size == 0) {
    return "0";
  }
  std::stringstream out;
  for (int i = 0; i < this->waypoints.size; i ++) {
    if (useSimpleCoords) {
      out << this->waypoints[i].toString();
    } else {
      out << this->owner->getFundamentalCoordinatesFromSimple(this->waypoints[i]).toString();
    }
    if (i != this->waypoints.size - 1) {
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
      tempP.actByEFDisplayIndex(i + 1);
      if (!tempP.isEqualToZero()) {
        out << "e_{" << i + 1 << "}";
      }
      tempP = *this;
      tempP.actByEFDisplayIndex(- i - 1);
      if (!tempP.isEqualToZero()) {
        out << "e_{" << - i - 1 << "},";
      }
    }
  }
  return out.str();
}
