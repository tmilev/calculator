#include "general_lists.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_lie_theory_extras.h"
#include "math_extra_modules_semisimple_lie_algebras.h" // IWYU pragma: keep: breaks the build.
#include "math_extra_semisimple_lie_algebras.h"
#include "math_extra_universal_enveloping.h"
#include "math_general.h"
#include "math_vector_partition_functions.h"

void SemisimpleLieAlgebra::getChevalleyGeneratorAsLieBracketsSimpleGenerators(
  int generatorIndex,
  List<int>& outputIndicesFormatAd0Ad1Ad2etc,
  Rational& outputMultiplyLieBracketsToGetGenerator
) {
  STACK_TRACE(
    "SemisimpleLieAlgebra::getChevalleyGeneratorAsLieBracketsSimpleGenerators"
  );
  outputIndicesFormatAd0Ad1Ad2etc.size = 0;
  if (this->isGeneratorFromCartan(generatorIndex)) {
    int simpleIndex = generatorIndex - this->getNumberOfPositiveRoots();
    outputIndicesFormatAd0Ad1Ad2etc.addOnTop(generatorIndex + this->getRank());
    outputIndicesFormatAd0Ad1Ad2etc.addOnTop(
      2 * this->getNumberOfPositiveRoots() - 1 - generatorIndex
    );
    outputMultiplyLieBracketsToGetGenerator =
    this->weylGroup.cartanSymmetric.elements[simpleIndex][simpleIndex] /
    2;
    return;
  }
  Vector<Rational> weight = this->getWeightOfGenerator(generatorIndex);
  outputMultiplyLieBracketsToGetGenerator = 1;
  Vector<Rational> genWeight;
  Vector<Rational> newWeight;
  while (!weight.isEqualToZero()) {
    for (int i = 0; i < this->getRank(); i ++) {
      genWeight.makeEi(this->getRank(), i);
      if (weight.isPositive()) {
        genWeight.negate();
      }
      newWeight = weight + genWeight;
      if (newWeight.isEqualToZero() || this->weylGroup.isARoot(newWeight)) {
        weight = newWeight;
        int index = this->getGeneratorIndexFromRoot(- genWeight);
        outputIndicesFormatAd0Ad1Ad2etc.addOnTop(index);
        if (!weight.isEqualToZero()) {
          int currentIndex = this->weylGroup.rootSystem.getIndex(weight);
          index = this->getRootIndexFromGenerator(index);
          if (!this->computedChevalleyConstants.elements[index][currentIndex]) {
            global.fatal
            << "For some reason I am not computed. Here is me: "
            << this->toString()
            << global.fatal;
          }
          outputMultiplyLieBracketsToGetGenerator /=
          this->chevalleyConstants.elements[index][currentIndex];
        }
        break;
      }
    }
  }
}

bool PartialFractions::argumentsAllowed(
  Vectors<Rational>& arguments, std::string& outputWhatWentWrong
) {
  if (arguments.size < 1) {
    return false;
  }
  Cone cone;
  bool result = cone.createFromVertices(arguments);
  if (cone.isEntireSpace()) {
    outputWhatWentWrong =
    "Error: the vectors you gave as input span the entire space.";
    return false;
  }
  for (int i = 0; i < cone.vertices.size; i ++) {
    if (cone.isInCone(cone.vertices[i]) && cone.isInCone(- cone.vertices[i])) {
      std::stringstream out;
      out
      << "Error: the Q_{>0} span of vectors you gave as "
      << "input contains zero (as it contains the vector "
      << cone.vertices[i].toString()
      << " as well as its opposite vector "
      << (- cone.vertices[i]).toString()
      << "), hence the vector partition function is "
      << "can only take values infinity or zero. ";
      outputWhatWentWrong = out.str();
      return false;
    }
  }
  return result;
}

void Lattice::intersectWithLineGivenBy(
  Vector<Rational>& inputLine, Vector<Rational>& outputGenerator
) {
  Vectors<Rational> roots;
  roots.addOnTop(inputLine);
  this->intersectWithLinearSubspaceSpannedBy(roots);
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
    global.fatal
    << "LS path without initialized owner is begin acted upon. "
    << global.fatal;
  }
  if (displayIndex > 0) {
    this->actByEAlpha(displayIndex - 1);
  } else {
    this->actByFAlpha(- displayIndex - 1);
  }
}

void LittelmannPath::actByEAlpha(int indexAlpha) {
  if (this->owner == nullptr) {
    global.fatal
    << "LS path without initialized owner is begin acted upon. "
    << global.fatal;
  }
  if (indexAlpha < 0 || indexAlpha >= this->owner->getDimension()) {
    global.fatal
    << "Index of Littelmann root operator out of range. "
    << global.fatal;
  }
  if (this->waypoints.size == 0) {
    return;
  }
  Rational minimalScalarProduct = 0;
  int indexMinimalScalarProduct = - 1;
  if (this->owner == nullptr) {
    global.fatal << "zero owner not allowed here. " << global.fatal;
  }
  WeylGroupData& weylGroup = *this->owner;
  weylGroup.computeRho(true);
  Vector<Rational>& alpha = weylGroup.rootsOfBorel[indexAlpha];
  Rational lengthAlpha = weylGroup.rootScalarCartanRoot(alpha, alpha);
  Vector<Rational> alphaScaled = alpha * 2 / lengthAlpha;
  for (int i = 0; i < this->waypoints.size; i ++) {
    Rational scalarProduct =
    this->owner->rootScalarCartanRoot(this->waypoints[i], alphaScaled);
    if (scalarProduct <= minimalScalarProduct) {
      minimalScalarProduct = scalarProduct;
      indexMinimalScalarProduct = i;
    }
  }
  if (indexMinimalScalarProduct <= 0 || minimalScalarProduct > - 1) {
    this->waypoints.size = 0;
    return;
  }
  int precedingIndex = 0;
  for (int i = 0; i <= indexMinimalScalarProduct; i ++) {
    Rational scalar =
    weylGroup.rootScalarCartanRoot(this->waypoints[i], alphaScaled);
    if (scalar >= minimalScalarProduct + 1) {
      precedingIndex = i;
    }
    if (scalar < minimalScalarProduct + 1) {
      break;
    }
  }
  Rational s2 =
  this->owner->rootScalarCartanRoot(
    this->waypoints[precedingIndex], alphaScaled
  );
  if (!this->minimaAreIntegral()) {
    global.comments << "<br>Something is wrong: starting path is BAD!";
  }
  if (s2 > minimalScalarProduct + 1) {
    this->waypoints.setSize(this->waypoints.size + 1);
    for (int i = this->waypoints.size - 1; i >= precedingIndex + 2; i --) {
      this->waypoints[i] = this->waypoints[i - 1];
    }
    precedingIndex ++;
    indexMinimalScalarProduct ++;
    Vector<Rational>& r1 = this->waypoints[precedingIndex];
    Vector<Rational>& r2 = this->waypoints[precedingIndex - 1];
    Rational s1 = weylGroup.rootScalarCartanRoot(r1, alphaScaled);
    Rational x = (minimalScalarProduct + 1 - s2) / (s1 - s2);
    this->waypoints[precedingIndex] = (r1 - r2) * x + r2;
  }
  Vectors<Rational> differences;
  differences.setSize(indexMinimalScalarProduct - precedingIndex);
  Rational currentDistance = 0;
  Rational minimalDistance = 0;
  for (int i = 0; i < differences.size; i ++) {
    differences[i] =
    this->waypoints[i + precedingIndex + 1] -
    this->waypoints[i + precedingIndex];
    currentDistance +=
    weylGroup.rootScalarCartanRoot(differences[i], alphaScaled);
    if (currentDistance < minimalDistance) {
      weylGroup.reflectSimple(indexAlpha, differences[i]);
      minimalDistance = currentDistance;
    }
  }
  for (int i = 0; i < differences.size; i ++) {
    this->waypoints[i + precedingIndex + 1] =
    this->waypoints[i + precedingIndex] + differences[i];
  }
  for (int i = indexMinimalScalarProduct + 1; i < this->waypoints.size; i ++) {
    this->waypoints[i] += alpha;
  }
  this->simplify();
}

void LittelmannPath::actByFAlpha(int indexAlpha) {
  if (this->waypoints.size == 0) {
    return;
  }
  if (this->owner == nullptr) {
    global.fatal
    << "LS path without initialized owner is begin acted upon. "
    << global.fatal;
  }
  if (indexAlpha < 0 || indexAlpha >= this->owner->getDimension()) {
    global.fatal
    << "Index of Littelmann root operator out of range. "
    << global.fatal;
  }
  Rational minimalScalarProduct = 0;
  int indexMinimalScalarProduct = - 1;
  WeylGroupData& weylGroup = *this->owner;
  Vector<Rational>& alpha = weylGroup.rootsOfBorel[indexAlpha];
  Rational lengthAlpha = this->owner->rootScalarCartanRoot(alpha, alpha);
  Vector<Rational> alphaScaled = alpha * 2 / lengthAlpha;
  for (int i = 0; i < this->waypoints.size; i ++) {
    Rational scalarProduct =
    this->owner->rootScalarCartanRoot(this->waypoints[i], alphaScaled);
    if (scalarProduct <= minimalScalarProduct) {
      minimalScalarProduct = scalarProduct;
      indexMinimalScalarProduct = i;
    }
  }
  Rational lastScalar =
  this->owner->rootScalarCartanRoot(
    *this->waypoints.lastObject(), alphaScaled
  );
  if (indexMinimalScalarProduct < 0 || lastScalar - minimalScalarProduct < 1) {
    this->waypoints.size = 0;
    return;
  }
  int succeedingIndex = 0;
  for (
    int i = this->waypoints.size - 1; i >= indexMinimalScalarProduct; i --
  ) {
    Rational scalar =
    weylGroup.rootScalarCartanRoot(alphaScaled, this->waypoints[i]);
    if (scalar >= minimalScalarProduct + 1) {
      succeedingIndex = i;
    }
    if (scalar < minimalScalarProduct + 1) {
      break;
    }
  }
  Rational s1 =
  this->owner->rootScalarCartanRoot(
    this->waypoints[succeedingIndex], alphaScaled
  );
  if (s1 > minimalScalarProduct + 1) {
    this->waypoints.setSize(this->waypoints.size + 1);
    for (int i = this->waypoints.size - 1; i >= succeedingIndex + 1; i --) {
      this->waypoints[i] = this->waypoints[i - 1];
    }
    // Rational scalarNext =
    // weylGroup.rootScalarCartanRoot(this->waypoints[succeedingIndex],
    // alphaScaled);
    Vector<Rational>& r1 = this->waypoints[succeedingIndex];
    Vector<Rational>& r2 = this->waypoints[succeedingIndex - 1];
    Rational s2 = weylGroup.rootScalarCartanRoot(r2, alphaScaled);
    Rational x = (minimalScalarProduct + 1 - s2) / (s1 - s2);
    this->waypoints[succeedingIndex] = (r1 - r2) * x + r2;
  }
  Vector<Rational> difference;
  Vector<Rational> oldWayPoint;
  oldWayPoint = this->waypoints[indexMinimalScalarProduct];
  Rational currentDist = 0;
  for (int i = 0; i < succeedingIndex - indexMinimalScalarProduct; i ++) {
    difference =
    this->waypoints[i + indexMinimalScalarProduct + 1] - oldWayPoint;
    currentDist += weylGroup.rootScalarCartanRoot(difference, alphaScaled);
    if (currentDist > 0) {
      weylGroup.reflectSimple(indexAlpha, difference);
      currentDist = 0;
    }
    oldWayPoint = this->waypoints[i + indexMinimalScalarProduct + 1];
    this->waypoints[i + indexMinimalScalarProduct + 1] =
    this->waypoints[i + indexMinimalScalarProduct] + difference;
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
  Vector<Rational> d1;
  Vector<Rational> d2;
  Rational d11;
  Rational d12;
  Rational d22;
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
  List<Rational> minima;
  WeylGroupData& weyl = *this->owner;
  int dimension = weyl.getDimension();
  minima.setSize(dimension);
  for (int i = 0; i < dimension; i ++) {
    minima[i] = weyl.getScalarProductSimpleRoot(this->waypoints[0], i) * 2 /
    weyl.cartanSymmetric.elements[i][i];
  }
  for (int i = 1; i < this->waypoints.size; i ++) {
    for (int j = 0; j < dimension; j ++) {
      minima[j] =
      MathRoutines::minimum(
        weyl.getScalarProductSimpleRoot(this->waypoints[i], j) * 2 /
        weyl.cartanSymmetric.elements[j][j],
        minima[j]
      );
    }
  }
  for (int i = 0; i < dimension; i ++) {
    if (!minima[i].isSmallInteger()) {
      return false;
    }
  }
  return true;
}

void LittelmannPath::makeFromweightInSimpleCoordinates(
  const Vector<Rational>& weightInSimpleCoordinates, WeylGroupData& inputOwner
) {
  this->owner = &inputOwner;
  this->waypoints.setSize(2);
  this->waypoints[0].makeZero(inputOwner.getDimension());
  this->waypoints[1] = weightInSimpleCoordinates;
  this->simplify();
}

std::string LittelmannPath::toStringIndicesToCalculatorOutput(
  LittelmannPath& inputStartingPath, List<int>& input
) {
  std::stringstream out;
  for (int i = input.size - 1; i >= 0; i --) {
    int displayIndex = input[i];
    if (displayIndex >= 0) {
      displayIndex ++;
    }
    out << "eAlpha(" << displayIndex << ", ";
  }
  out
  << "littelmann"
  << inputStartingPath.owner->getFundamentalCoordinatesFromSimple(
    *inputStartingPath.waypoints.lastObject()
  ).toString();
  for (int i = 0; i < input.size; i ++) {
    out << " ) ";
  }
  return out.str();
}

bool LittelmannPath::generateOrbit(
  List<LittelmannPath>& output,
  List<List<int> >& outputOperators,
  int upperBoundNumberOfElements,
  Selection* parabolicNonSelectedAreInLeviPart
) {
  HashedList<LittelmannPath> hashedOutput;
  hashedOutput.addOnTop(*this);
  int dimension = this->owner->getDimension();
  outputOperators.setSize(1);
  outputOperators[0].setSize(0);
  List<int> currentSequence;
  if (upperBoundNumberOfElements > 0) {
    currentSequence.reserve(upperBoundNumberOfElements);
  }
  LittelmannPath currentPath;
  bool result = true;
  Selection parabolicSelectionSelectedAreInLeviPart;
  parabolicSelectionSelectedAreInLeviPart.initialize(dimension);
  if (parabolicNonSelectedAreInLeviPart != nullptr) {
    parabolicSelectionSelectedAreInLeviPart =
    *parabolicNonSelectedAreInLeviPart;
    parabolicSelectionSelectedAreInLeviPart.invertSelection();
  } else {
    parabolicSelectionSelectedAreInLeviPart.makeFullSelection();
  }
  for (
    int lowestNonExplored = 0; lowestNonExplored < hashedOutput.size;
    lowestNonExplored ++
  ) {
    if (
      upperBoundNumberOfElements > 0 &&
      upperBoundNumberOfElements < hashedOutput.size
    ) {
      result = false;
      break;
    } else {
      for (
        int j = 0; j < parabolicSelectionSelectedAreInLeviPart.
        cardinalitySelection; j ++
      ) {
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

std::string LittelmannPath::toStringOperatorSequenceStartingOnMe(
  List<int>& input
) {
  MonomialTensor<Rational> monomial;
  monomial = input;
  monomial.generatorsIndices.reverseElements();
  monomial.powers.reverseElements();
  return monomial.toString();
}

template <class Coefficient>
bool MonomialUniversalEnvelopingOrdered<Coefficient>::
modOutFDRelationsExperimental(
  const Vector<Rational>& highestWeightSimpleCoordinates,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  WeylGroupData& weyl = this->owner->ownerSemisimpleLieAlgebra->weylGroup;
  Vector<Rational> highestWeightSimpleCoordinatesTrue =
  highestWeightSimpleCoordinates;
  weyl.raiseToDominantWeight(highestWeightSimpleCoordinatesTrue);
  Vector<Rational> highestWeightDualCoordinates =
  weyl.getDualCoordinatesFromFundamental(
    weyl.getFundamentalCoordinatesFromSimple(
      highestWeightSimpleCoordinatesTrue
    )
  );
  List<Coefficient> substitution;
  substitution.setSize(highestWeightDualCoordinates.size);
  for (int i = 0; i < highestWeightDualCoordinates.size; i ++) {
    substitution[i] = highestWeightDualCoordinates[i];
  }
  this->modOutVermaRelations(&substitution, ringUnit, ringZero);
  int numberOfPositiveRoots =
  this->owner->ownerSemisimpleLieAlgebra->getNumberOfPositiveRoots();
  Vector<Rational> currentWeight = highestWeightSimpleCoordinatesTrue;
  Vector<Rational> testWeight;
  for (int k = this->generatorsIndices.size - 1; k >= 0; k --) {
    int indexCurrentGenerator = this->generatorsIndices[k];
    if (indexCurrentGenerator >= numberOfPositiveRoots) {
      return false;
    }
    ElementSemisimpleLieAlgebra<Rational>& currentElement =
    this->owner->elementOrder[indexCurrentGenerator];
    if (
      !currentElement.getCartanPart().isEqualToZero() ||
      currentElement.size() > 1
    ) {
      return false;
    }
    int power = 0;
    if (!this->powers[k].isSmallInteger(power)) {
      return false;
    }
    int rootIndex =
    this->owner->ownerSemisimpleLieAlgebra->getRootIndexFromGenerator(
      currentElement[0].generatorIndex
    );
    const Vector<Rational>& currentRoot = weyl.rootSystem[rootIndex];
    for (int j = 0; j < power; j ++) {
      currentWeight += currentRoot;
      testWeight = currentWeight;
      weyl.raiseToDominantWeight(testWeight);
      if (
        !(highestWeightSimpleCoordinatesTrue - testWeight).isPositiveOrZero()
      ) {
        this->makeZero(ringZero, *this->owner);
        return true;
      }
    }
  }
  return true;
}

template <class Coefficient>
bool ElementUniversalEnvelopingOrdered<Coefficient>::
modOutFDRelationsExperimental(
  const Vector<Rational>& highestWeightSimpleCoordinates,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  MonomialUniversalEnvelopingOrdered<Coefficient> monomial;
  ElementUniversalEnvelopingOrdered<Coefficient> output;
  output.makeZero(*this->owner);
  bool result = true;
  for (int i = 0; i < this->size; i ++) {
    monomial = this->objects[i];
    if (
      !monomial.modOutFDRelationsExperimental(
        highestWeightSimpleCoordinates, ringUnit, ringZero
      )
    ) {
      result = false;
    }
    output.addMonomial(monomial);
  }
  this->operator=(output);
  return result;
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::getCoordinatesInBasis(
  List<ElementUniversalEnveloping<Coefficient> >& basis,
  Vector<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) const {
  List<ElementUniversalEnveloping<Coefficient> > workingBasis;
  List<ElementUniversalEnveloping<Coefficient> > elements;
  workingBasis = basis;
  workingBasis.addOnTop(*this);
  Vectors<Coefficient> coordinates;
  if (
    !this->getBasisFromSpanOfElements(
      workingBasis, coordinates, elements, ringUnit, ringZero
    )
  ) {
    return false;
  }
  Vector<Coefficient> root;
  root = *coordinates.lastObject();
  coordinates.setSize(basis.size);
  return root.getCoordinatesInBasis(coordinates, output);
}

template <class Coefficient>
template <class CoefficientTypeQuotientField>
bool ElementUniversalEnveloping<Coefficient>::getBasisFromSpanOfElements(
  List<ElementUniversalEnveloping<Coefficient> >& elements,
  Vectors<CoefficientTypeQuotientField>& outputCoordinates,
  List<ElementUniversalEnveloping<Coefficient> >& outputBasis,
  const CoefficientTypeQuotientField& fieldUnit,
  const CoefficientTypeQuotientField& fieldZero
) {
  if (elements.size == 0) {
    return false;
  }
  ElementUniversalEnveloping<Coefficient> outputCorrespondingMonomials;
  outputCorrespondingMonomials.makeZero(*elements[0].owner);
  Vectors<CoefficientTypeQuotientField> outputCoordinatesBeforeReduction;
  for (int i = 0; i < elements.size; i ++) {
    for (int j = 0; j < elements[i].size; j ++) {
      outputCorrespondingMonomials.addOnTopNoRepetition(elements[i][j]);
    }
  }
  outputCoordinatesBeforeReduction.setSize(elements.size);
  for (int i = 0; i < elements.size; i ++) {
    Vector<CoefficientTypeQuotientField>& currentList =
    outputCoordinatesBeforeReduction[i];
    currentList.makeZero(outputCorrespondingMonomials.size);
    ElementUniversalEnveloping<Coefficient>& currentElement = elements[i];
    for (int j = 0; j < currentElement.size; j ++) {
      MonomialUniversalEnveloping<Coefficient>& monomial = currentElement[j];
      currentList[outputCorrespondingMonomials.getIndex(monomial)] =
      monomial.coefficient;
    }
  }
  outputBasis.size = 0;
  outputBasis.reserve(elements.size);
  Vectors<CoefficientTypeQuotientField> basisCoordinateForm;
  basisCoordinateForm.reserve(elements.size);
  Selection selectedBasis;
  outputCoordinatesBeforeReduction.SelectABasis(
    basisCoordinateForm, fieldZero, selectedBasis
  );
  for (int i = 0; i < selectedBasis.cardinalitySelection; i ++) {
    outputBasis.addOnTop(elements.objects[selectedBasis.elements[i]]);
  }
  Matrix<Coefficient> bufferMatrix;
  Vectors<Coefficient> bufferVectors;
  outputCoordinatesBeforeReduction.getCoordinatesInBasis(
    basisCoordinateForm,
    outputCoordinates,
    bufferVectors,
    bufferMatrix,
    fieldUnit,
    fieldZero
  );
  return true;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::modToMinDegreeFormFDRels(
  const Vector<Rational>& highestWeightInSimpleCoordinates,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  ElementUniversalEnveloping<Coefficient> result;
  result.makeZero(*this->owner);
  bool found = true;
  int numberOfPositiveRoots = this->owner->getNumberOfPositiveRoots();
  while (found) {
    found = false;
    for (int j = numberOfPositiveRoots - 1; j >= 0; j --) {
      this->owner->universalEnvelopingGeneratorOrder.swapTwoIndices(
        j, numberOfPositiveRoots - 1
      );
      this->simplify(ringUnit);
      this->owner->universalEnvelopingGeneratorOrder.swapTwoIndices(
        j, numberOfPositiveRoots - 1
      );
      if (
        this->modOutFDRelationsExperimental(
          highestWeightInSimpleCoordinates, ringUnit, ringZero
        )
      ) {
        found = true;
      }
    }
  }
  this->simplify(ringUnit);
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::applyMinusTransposeAutoOnMe() {
  MonomialUniversalEnveloping<Coefficient> monomial;
  ElementUniversalEnveloping<Coefficient> result;
  result.makeZero(*this->owner);
  int numberOfPositiveRoots = this->getOwner().getNumberOfPositiveRoots();
  int rank = this->getOwner().getRank();
  Coefficient coefficient;
  for (int i = 0; i < this->size; i ++) {
    MonomialUniversalEnveloping<Coefficient>& workingMonomial =
    this->objects[i];
    coefficient = this->coefficients[i];
    monomial.owner = workingMonomial.owner;
    monomial.powers.size = 0;
    monomial.generatorsIndices.size = 0;
    for (int j = 0; j < workingMonomial.powers.size; j ++) {
      int power;
      if (!workingMonomial.powers[j].isSmallInteger(&power)) {
        return false;
      }
      int generator = workingMonomial.generatorsIndices[j];
      if (generator < numberOfPositiveRoots) {
        generator = 2 * numberOfPositiveRoots + rank - 1 - generator;
      } else if (generator >= numberOfPositiveRoots + rank) {
        generator = - generator + 2 * numberOfPositiveRoots + rank - 1;
      }
      monomial.multiplyByGeneratorPowerOnTheRight(
        generator, workingMonomial.powers[j]
      );
      if (power % 2 == 1) {
        coefficient *= - 1;
      }
    }
    result.addMonomial(monomial, coefficient);
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
  ElementUniversalEnveloping<Coefficient> minusTransposeRight;
  minusTransposeRight = right;
  if (!minusTransposeRight.applyMinusTransposeAutoOnMe()) {
    return false;
  }
  ElementUniversalEnveloping<Coefficient> accumulator;
  ElementUniversalEnveloping<Coefficient> intermediateAccumulator;
  ElementUniversalEnveloping<Coefficient> element;
  accumulator.makeZero(*this->owners, this->indexInOwners);
  MonomialUniversalEnveloping<Coefficient> constantMonomial;
  constantMonomial.makeConstant();
  if (logStream != nullptr) {
    *logStream
    << "backtraced elt: "
    << minusTransposeRight.toString(&global.defaultFormat.getElement())
    << "<br>";
    *logStream
    << "this element: "
    << this->toString(&global.defaultFormat.getElement())
    << "<br>";
  }
  for (int j = 0; j < right.size; j ++) {
    intermediateAccumulator = *this;
    intermediateAccumulator.simplify(global, ringUnit, ringZero);
    if (logStream != nullptr) {
      *logStream
      << "intermediate after simplification: "
      << intermediateAccumulator.toString(&global.defaultFormat.getElement())
      << "<br>";
    }
    intermediateAccumulator.modOutVermaRelations(
      &global, substitutionHiGoesToIthElement, ringUnit, ringZero
    );
    MonomialUniversalEnveloping<Coefficient>& rightMonomial =
    minusTransposeRight[j];
    Coefficient& rightMonomialCoefficient =
    minusTransposeRight.coefficients[j];
    int power;
    for (int i = rightMonomial.powers.size - 1; i >= 0; i --) {
      if (rightMonomial.powers[i].isSmallInteger(&power)) {
        for (int k = 0; k < power; k ++) {
          element.makeOneGenerator(
            rightMonomial.generatorsIndices[i],
            *this->owners,
            this->indexInOwners,
            ringUnit
          );
          MathRoutines::swap(element, intermediateAccumulator);
          if (logStream != nullptr) {
            *logStream
            << "element before mult: "
            << element.toString(&global.defaultFormat)
            << "<br>";
            *logStream
            << "intermediate before mult: "
            << intermediateAccumulator.toString(
              &global.defaultFormat.getElement()
            )
            << "<br>";
          }
          intermediateAccumulator *= (element);
          if (logStream != nullptr) {
            *logStream
            << "intermediate before simplification: "
            << intermediateAccumulator.toString(
              &global.defaultFormat.getElement()
            )
            << "<br>";
          }
          intermediateAccumulator.simplify(ringUnit);
          if (logStream != nullptr) {
            *logStream
            << "intermediate after simplification: "
            << intermediateAccumulator.toString(
              &global.defaultFormat.getElement()
            )
            << "<br>";
          }
          intermediateAccumulator.modOutVermaRelations(
            substitutionHiGoesToIthElement, ringUnit, ringZero
          );
          if (logStream != nullptr) {
            *logStream
            << "intermediate after Verma rels: "
            << intermediateAccumulator.toString(
              &global.defaultFormat.getElement()
            )
            << "<br>";
          }
        }
      } else {
        return false;
      }
    }
    intermediateAccumulator *= rightMonomialCoefficient;
    accumulator += intermediateAccumulator;
    int index = intermediateAccumulator.getIndex(constantMonomial);
    if (index != - 1) {
      output += intermediateAccumulator.coefficients[index];
    }
  }
  if (logStream != nullptr) {
    *logStream
    << "final UE element: "
    << accumulator.toString(&global.defaultFormat.getElement());
  }
  return true;
}

template <class Coefficient>
std::string ElementUniversalEnveloping<Coefficient>::isInProperSubmodule(
  const Vector<Coefficient>* substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  std::stringstream out;
  List<ElementUniversalEnveloping<Coefficient> > orbit;
  orbit.reserve(1000);
  ElementUniversalEnveloping<Coefficient> element;
  int dimension = this->getOwner().getRank();
  int totalPositiveRoots = this->getOwner().getNumberOfPositiveRoots();
  orbit.addOnTop(*this);
  for (int i = 0; i < orbit.size; i ++) {
    for (int j = 0; j < dimension; j ++) {
      element.makeOneGenerator(
        j + totalPositiveRoots + dimension, *this->owner, ringUnit
      );
      element *= orbit[i];
      element.simplify(ringUnit);
      element.modOutVermaRelations(
        substitutionHiGoesToIthElement, ringUnit, ringZero
      );
      if (!element.isEqualToZero()) {
        orbit.addOnTop(element);
      }
    }
  }
  for (int i = 0; i < orbit.size; i ++) {
    ElementUniversalEnveloping<Coefficient>& current = orbit[i];
    out << "<br>" << current.toString(&global.defaultFormat.getElement());
  }
  return out.str();
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::convertToRationalCoefficient(
  ElementUniversalEnveloping<Rational>& output
) {
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<Rational> workingMonomial;
  Rational coefficient;
  for (int i = 0; i < this->size; i ++) {
    MonomialUniversalEnveloping<Coefficient>& monomial = this->objects[i];
    workingMonomial.makeOne(*this->owner);
    if (!this->coefficients[i].isConstant(coefficient)) {
      return false;
    }
    for (int j = 0; j < monomial.powers.size; j ++) {
      Rational constantPower;
      if (!monomial.powers[j].isConstant(constantPower)) {
        return false;
      }
      workingMonomial.multiplyByGeneratorPowerOnTheRight(
        monomial.generatorsIndices[j], constantPower
      );
    }
    output.addMonomial(workingMonomial, Coefficient(1));
  }
  return true;
}

void BranchingData::initializePart1NoSubgroups() {
  STACK_TRACE(
    "BranchingData::initAssumingParSelAndHmmInittedPart1NoSubgroups"
  );
  this->weylGroupFiniteDimensionalSmallAsSubgroupInLarge.ambientWeyl =
  &this->homomorphism.coDomainAlgebra().weylGroup;
  this->weylGroupFiniteDimensionalSmall.ambientWeyl =
  &this->homomorphism.domainAlgebra().weylGroup;
  this->weylGroupFiniteDimensional.ambientWeyl =
  &this->homomorphism.coDomainAlgebra().weylGroup;
  this->smallParabolicSelection.initialize(
    weylGroupFiniteDimensionalSmall.ambientWeyl->getDimension()
  );
  for (int i = 0; i < this->homomorphism.imagesCartanDomain.size; i ++) {
    Vector<Rational>& currentVector =
    this->homomorphism.imagesCartanDomain[i];
    this->generatorsSmallSub.addOnTop(currentVector);
    for (int j = 0; j < currentVector.size; j ++) {
      if (!currentVector[j].isEqualToZero() && this->inducing.selected[j]) {
        this->generatorsSmallSub.removeLastObject();
        this->smallParabolicSelection.addSelectionAppendNewIndex(i);
        break;
      }
    }
  }
  this->nilradicalModuloPreimageNilradical.setSize(0);
  this->nilradicalSmall.setSize(0);
  this->nilradicalLarge.setSize(0);
  this->weightsNilradicalLarge.setSize(0);
  this->weightsNilradicalSmall.setSize(0);
  this->weightsNilModPreNil.setSize(0);
  this->indicesNilradicalLarge.setSize(0);
  this->indicesNilradicalSmall.setSize(0);
  ElementSemisimpleLieAlgebra<Rational> element;
  WeylGroupData& largeWeylGroup =
  this->homomorphism.coDomainAlgebra().weylGroup;
  WeylGroupData& smallWeylGroup = this->homomorphism.domainAlgebra().weylGroup;
  int numberOfB3NegativeGenerators =
  this->homomorphism.coDomainAlgebra().getNumberOfPositiveRoots();
  int numberOfG2NegativeGenerators =
  this->homomorphism.domainAlgebra().getNumberOfPositiveRoots();
  for (int i = 0; i < numberOfB3NegativeGenerators; i ++) {
    const Vector<Rational>& currentWeight = largeWeylGroup.rootSystem[i];
    bool isInNilradical = false;
    for (int k = 0; k < this->inducing.cardinalitySelection; k ++) {
      if (!currentWeight[this->inducing.elements[k]].isEqualToZero()) {
        isInNilradical = true;
        break;
      }
    }
    if (isInNilradical) {
      this->weightsNilradicalLarge.addOnTop(currentWeight);
      element.makeGenerator(i, this->homomorphism.coDomainAlgebra());
      this->nilradicalLarge.addOnTop(element);
      this->indicesNilradicalLarge.addOnTop(i);
    }
  }
  for (int i = 0; i < numberOfG2NegativeGenerators; i ++) {
    const Vector<Rational>& currentWeight = smallWeylGroup.rootSystem[i];
    bool isInNilradical = false;
    for (
      int k = 0; k < this->smallParabolicSelection.cardinalitySelection; k ++
    ) {
      if (
        !currentWeight[this->smallParabolicSelection.elements[k]].isEqualToZero
        ()
      ) {
        isInNilradical = true;
        break;
      }
    }
    if (isInNilradical) {
      this->weightsNilradicalSmall.addOnTop(currentWeight);
      element.makeGenerator(i, this->homomorphism.domainAlgebra());
      this->nilradicalSmall.addOnTop(element);
      this->indicesNilradicalSmall.addOnTop(i);
    }
  }
  this->nilradicalModuloPreimageNilradical = this->nilradicalLarge;
  this->weightsNilModPreNil = this->weightsNilradicalLarge;
  Vector<Rational> projection;
  for (int i = 0; i < this->nilradicalSmall.size; i ++) {
    ElementSemisimpleLieAlgebra<Rational>& elementImage =
    this->homomorphism.imagesAllChevalleyGenerators[
      this->indicesNilradicalSmall[i]
    ];
    int index =
    this->nilradicalModuloPreimageNilradical.getIndex(elementImage);
    if (index != - 1) {
      this->nilradicalModuloPreimageNilradical.removeIndexSwapWithLast(index);
      this->weightsNilModPreNil.removeIndexSwapWithLast(index);
      continue;
    }
    bool isGood = false;
    for (int j = 0; j < this->weightsNilModPreNil.size; j ++) {
      projection = this->projectWeight(this->weightsNilModPreNil[j]);
      if (projection == this->weightsNilradicalSmall[i]) {
        isGood = true;
        this->nilradicalModuloPreimageNilradical.removeIndexSwapWithLast(j);
        this->weightsNilModPreNil.removeIndexSwapWithLast(j);
        break;
      }
    }
    if (!isGood) {
      global.fatal
      <<
      "This is either a programming error, or Lemma 3.3, T. Milev, P. Somberg, \"On branching...\""
      <<
      " is wrong. The question is, which is the more desirable case... The bad apple is element "
      << this->nilradicalSmall[i].toString()
      << " of weight "
      << this->weightsNilradicalSmall[i].toString()
      << ". "
      << global.fatal;
    }
  }
}

BranchingData::BranchingData() {
  this->flagUseNilWeightGeneratorOrder = false;
  this->flagAscendingGeneratorOrder = false;
}

void BranchingData::initializePart2NoSubgroups() {
  List<Vectors<Rational> > emptyList;
  this->weylGroupFiniteDimensionalSmallAsSubgroupInLarge.
  computeSubGroupFromGeneratingReflections(
    &this->generatorsSmallSub, &emptyList, 1000, true
  );
  this->weylGroupFiniteDimensionalSmall.makeParabolicFromSelectionSimpleRoots(
    *this->weylGroupFiniteDimensionalSmall.ambientWeyl,
    this->smallParabolicSelection,
    1000
  );
  this->weylGroupFiniteDimensional.makeParabolicFromSelectionSimpleRoots(
    this->homomorphism.coDomainAlgebra().weylGroup, this->inducing, 1000
  );
  this->weylGroupFiniteDimensional.computeRootSubsystem();
  this->weylGroupFiniteDimensionalSmallAsSubgroupInLarge.computeRootSubsystem(
  );
  this->weylGroupFiniteDimensionalSmall.computeRootSubsystem();
}

std::string BranchingData::getStringCasimirProjector(
  int index, const Rational& additionalMultiple
) {
  Vector<RationalFraction<Rational> > weightDifference;
  std::stringstream formulaStream1;
  HashedList<Vector<RationalFraction<Rational> > > accountedDifferences;
  accountedDifferences.setExpectedSize(this->g2Weights.size);
  bool found = false;
  for (int i = 0; i < this->g2Weights.size; i ++) {
    weightDifference = this->g2Weights[i] - this->g2Weights[index];
    if (
      weightDifference.isPositive() &&
      !accountedDifferences.contains(weightDifference)
    ) {
      accountedDifferences.addOnTop(weightDifference);
      if (additionalMultiple != 1) {
        formulaStream1 << additionalMultiple.toString(&this->format);
      }
      formulaStream1
      << "(i(\\bar c) - ("
      << this->allCharacters[i].toString(&this->format)
      << "))";
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

bool LittelmannPath::isAdaptedString(
  MonomialTensor<int, HashFunctions::hashFunction>& inputString
) {
  LittelmannPath path1 = *this;
  LittelmannPath path2;
  for (int i = 0; i < inputString.generatorsIndices.size; i ++) {
    for (int k = 0; k < inputString.powers[i]; k ++) {
      path1.actByEAlpha(- inputString.generatorsIndices[i] - 1);
    }
    if (path1.isEqualToZero()) {
      return false;
    }
    path2 = path1;
    path2.actByEAlpha(- inputString.generatorsIndices[i] - 1);
    if (!path2.isEqualToZero()) {
      return false;
    }
  }
  return true;
}

void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::
getGroupElementsIndexedAsAmbientGroup(List<ElementWeylGroup>& output) {
  STACK_TRACE(
    "SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms"
    "::getGroupElementsIndexedAsAmbientGroup"
  );
  if (this->externalAutomorphisms.size > 0) {
    global.fatal
    << "This is  a programming error: a function meant for subgroups that are "
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
    indexShifts[i] =
    this->simpleRootsInner[i].getIndexFirstNonZeroCoordinate();
  }
  for (int i = 0; i < this->allElements.size; i ++) {
    const ElementSubgroupWeylGroupAutomorphisms& other = this->allElements[i];
    currentOutput.generatorsLastAppliedFirst.setSize(
      other.generatorsLastAppliedFirst.size
    );
    for (int j = 0; j < currentOutput.generatorsLastAppliedFirst.size; j ++) {
      currentOutput.generatorsLastAppliedFirst[j].index =
      indexShifts[other.generatorsLastAppliedFirst[j].index];
    }
    output.addOnTop(currentOutput);
  }
}

std::string LittelmannPath::toString(
  bool useSimpleCoordinates, bool useArrows, bool includeDominance
) const {
  if (this->waypoints.size == 0) {
    return "0";
  }
  std::stringstream out;
  for (int i = 0; i < this->waypoints.size; i ++) {
    if (useSimpleCoordinates) {
      out << this->waypoints[i].toString();
    } else {
      out
      << this->owner->getFundamentalCoordinatesFromSimple(this->waypoints[i]).
      toString();
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
      LittelmannPath path = *this;
      path.actByEFDisplayIndex(i + 1);
      if (!path.isEqualToZero()) {
        out << "e_{" << i + 1 << "}";
      }
      path = *this;
      path.actByEFDisplayIndex(- i - 1);
      if (!path.isEqualToZero()) {
        out << "e_{" << - i - 1 << "},";
      }
    }
  }
  return out.str();
}

template < >
unsigned int HashFunctions::hashFunction(const std::string& input) {
  size_t numberOfCycles = input.size();
  unsigned int result = 0;
  int hashCounter = 0;
  for (unsigned i = 0; i < numberOfCycles; i ++) {
    result += static_cast<unsigned>(input[i]) *
    HashConstants::getConstantIncrementCounter(hashCounter);
  }
  return result;
}

template < >
unsigned int HashFunctions::hashFunction(const bool& input) {
  return static_cast<unsigned int>(input);
}

template < >
unsigned int HashFunctions::hashFunction(const int& input) {
  return static_cast<unsigned>(input);
}

template < >
unsigned int HashFunctions::hashFunction(const unsigned int& input) {
  return input;
}

template < >
unsigned int HashFunctions::hashFunction(const char& input) {
  return static_cast<unsigned int>(input);
}

template < >
unsigned int HashFunctions::hashFunction(const unsigned char& input) {
  return static_cast<unsigned int>(input);
}

template < >
unsigned int HashFunctions::hashFunction(const List<unsigned char>& input) {
  unsigned int result = 0;
  int j = 0;
  for (int i = 0; i < input.size; i ++) {
    result +=
    HashConstants::getConstantIncrementCounter(j) * static_cast<unsigned>(
      input[i]
    );
  }
  return result;
}
