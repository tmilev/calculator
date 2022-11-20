
#ifndef header_math_subsets_selections_ALREADY_INCLUDED
#define header_math_subsets_selections_ALREADY_INCLUDED

#include "general_lists.h"
#include "math_vectors.h"
#include "general_memory_saving.h"

class Selection {
public:
  List<int> elements;
  List<bool> selected;
  int numberOfElements;
  int cardinalitySelection;
  void addSelectionAppendNewIndex(int index);
  void removeLastSelection();
  void removeSelection(int index) {
    this->selected[index] = false;
    this->computeIndicesFromSelection();
  }
  void makeFullSelection(int inputMaxSize) {
    this->initialize(inputMaxSize);
    this->makeFullSelection();
  }
  void makeFullSelection() {
    for (int i = 0; i < this->numberOfElements; i ++) {
      this->elements[i] = i;
      this->selected[i] = true;
    }
    this->cardinalitySelection = this->numberOfElements;
  }
  bool isSubset(const Selection& other) const {
    if (this->numberOfElements != other.numberOfElements) {
      return false;
    }
    for (int i = 0; i < this->cardinalitySelection; i ++) {
      if (!other.selected[this->elements[i]]) {
        return false;
      }
    }
    return true;
  }
  void initialize(int inputNumberOfElements);
  void computeIndicesFromSelection();
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const Selection& input) {
    return input.hashFunction();
  }
  std::string toString() const;
  void incrementSelection();
  bool incrementReturnFalseIfPastLast() {
    this->incrementSelection();
    if (this->cardinalitySelection == 0) {
      return false;
    }
    return true;
  }
  int selectionToIndex();
  void expandMaxSize();
  LargeInteger getNumberOfCombinationsFixedCardinality(int cardinality) {
    return MathRoutines::nChooseK(this->numberOfElements, cardinality);
  }
  void shrinkMaxSize();
  void makeSubSelection(Selection& selection, Selection& subSelection);
  void initSelectionFixedCardinality(int cardinality);
  bool incrementSelectionFixedCardinalityReturnFalseIfPastLast(
    int cardinality
  );
  void invertSelection() {
    for (int i = 0; i < this->numberOfElements; i ++) {
      this->selected[i] = !this->selected[i];
    }
    this->computeIndicesFromSelection();
  }
  void operator=(const Selection& right);
  void operator=(const Vector<Rational>& other);
  // void operator=(const std::string& other);
  // warning: to call the comparison operator sucessfully, cardinalitySelection
  // must
  // be properly computed!
  bool operator==(const Selection& right) const {
    if (
      this->numberOfElements != right.numberOfElements ||
      this->cardinalitySelection != right.cardinalitySelection
    ) {
      return false;
    }
    for (int i = 0; i < this->cardinalitySelection; i ++) {
      if (
        this->selected[this->elements[i]] !=
        right.selected[this->elements[i]]
      ) {
        return false;
      }
    }
    return true;
  }
  Selection();
  Selection(int m);
  Selection(const Vector<Rational>& other) {
    this->operator=(other);
  }
  Selection(const Selection& other):
  numberOfElements(0),
  cardinalitySelection(0) {
    *this = other;
  }
  bool checkInitialization() const;
  class Test {
  public:
    static bool all();
    static bool testNElements(int n);
  };
};

class SelectionWithMultiplicities {
public:
  void toString(std::string& output);
  std::string toString() {
    std::string tempS;
    this->toString(tempS);
    return tempS;
  }
  List<int> elements;
  List<int> multiplicities;
  static unsigned int hashFunction(const SelectionWithMultiplicities& input) {
    unsigned int result = 0;
    for (int i = 0; i < input.elements.size; i ++) {
      int j = input.elements[i];
      result += static_cast<unsigned int>(
        input.multiplicities[input.elements[i]]
      ) *
      HashConstants::getConstantIncrementCounter(j);
    }
    return result;
  }
  int cardinalitySelectionWithoutMultiplicities();
  void initWithMultiplicities(int numberOfElements);
  void computeElements();
};

class SelectionWithMaxMultiplicity: public SelectionWithMultiplicities {
public:
  int maximumMultiplicity;
  void initMaxMultiplicity(int numberOfElements, int maximulMultiplicity);
  LargeInteger numberOfCombinationsOfCardinality(int cardinality);
  LargeInteger getTotalCombinationCount() const;
  int numberOfSelectionsTotal() {
    return
    MathRoutines::kToTheNth(
      maximumMultiplicity, this->multiplicities.size
    );
  }
  bool incrementReturnFalseIfPastLast();
  void incrementSubset();
  void incrementSubsetFixedCardinality(int cardinality);
  bool hasMultiplicitiesZeroAndOneOnly();
  int maximumCardinalityWithMultiplicities() {
    return this->maximumMultiplicity * this->multiplicities.size;
  }
  int cardinalitySelectionWithMultiplicities();
  static unsigned int hashFunction(const SelectionWithMaxMultiplicity& input) {
    return
    static_cast<unsigned int>(input.maximumMultiplicity) *
    HashConstants::constant0 +
    input.::SelectionWithMultiplicities::hashFunction(input);
  }
};

class SelectionWithDifferentMaxMultiplicities {
public:
  List<int> elements;
  List<int> multiplicities;
  List<int> capacities;
  void initializePart1(int numberOfElements);
  void clearNoMaxMultiplicitiesChange();
  bool incrementReturnFalseIfPastLast();
  LargeInteger totalNumberOfSubsets();
  int totalNumberSubsetsSmallInt();
  int totalMultiplicity();
  int maximumTotalMultiplicity();
  void initializeFromIntegers(
    int* maximalMultiplicities, int numberOfMaximalMultiplicities
  );
  void initializeFromIntegers(const List<int>& maximalMultiplicities);
  bool hasSameMaxMultiplicities(
    SelectionWithDifferentMaxMultiplicities& other
  ) {
    return this->capacities.isEqualTo(other.capacities);
  }
  void operator=(const SelectionWithDifferentMaxMultiplicities& right) {
    this->multiplicities = right.multiplicities;
    this->capacities = right.capacities;
    this->elements = right.elements;
  }
};

class SelectionOneItem {
public:
  int maximumMultiplicity;
  int amount;
  SelectionOneItem(): maximumMultiplicity(0), amount(- 1) {}
  bool incrementReturnFalseIfPastLast() {
    if (this->maximumMultiplicity == 0) {
      return false;
    }
    this->amount ++;
    if (this->amount > this->maximumMultiplicity) {
      this->amount = 0;
    }
    return this->amount != 0;
  }
  void initializeFromMultiplicities(int inputMultiplicity) {
    this->maximumMultiplicity = inputMultiplicity;
    this->amount = 0;
  }
  std::string toString() const {
    std::stringstream out;
    out << this->amount << " out of " << this->maximumMultiplicity;
    return out.str();
  }
  LargeInteger totalCombinations() {
    return this->maximumMultiplicity + 1;
  }
};

template <class Base>
class Incrementable {
public:
  List<Base> elements;
  Base& operator[](int i) {
    return this->elements[i];
  }
  LargeInteger totalCombinations() {
    LargeInteger result = 1;
    for (int i = 0; i < this->elements.size; i ++) {
      result *= this->elements[i].totalCombinations();
    }
    return result;
  }
  bool incrementReturnFalseIfPastLast() {
    for (int i = this->elements.size - 1; i >= 0; i --) {
      if (this->elements[i].incrementReturnFalseIfPastLast()) {
        return true;
      }
    }
    return false;
  }
  void initializeFromMultiplicities(int inputBase, int numberOfElements) {
    this->elements.setSize(numberOfElements);
    for (int i = 0; i < this->elements.size; i ++) {
      this->elements[i].initializeFromMultiplicities(inputBase);
    }
  }
  template <class Element>
  void initializeFromMultiplicities(
    const List<Element>& input, int useOnlyNElementsOnly = 0
  ) {
    if (useOnlyNElementsOnly > 0 && useOnlyNElementsOnly <= input.size) {
      this->elements.setSize(useOnlyNElementsOnly);
    } else {
      this->elements.setSize(input.size);
    }
    for (int i = 0; i < this->elements.size; i ++) {
      this->elements[i].initializeFromMultiplicities(input[i]);
    }
  }
  std::string toString() const {
    std::stringstream out;
    out << "(";
    for (int i = 0; i < this->elements.size; i ++) {
      out << this->elements[i].toString();
      if (i != this->elements.size - 1) {
        out << ", ";
      }
    }
    out << ")";
    return out.str();
  }
};

class SelectionFixedRank {
public:
  Selection selection;
  int desiredSubsetSize;
  LargeInteger totalCombinations() {
    LargeInteger result;
    MathRoutines::nChooseK(
      selection.numberOfElements, desiredSubsetSize, result
    );
    return result;
  }
  void setNumberOfItemsAndDesiredSubsetSize(
    int inputDesiredSubsetSize, int inputNumberOfItems
  ) {
    if (inputDesiredSubsetSize < 0 || inputNumberOfItems < 0) {
      global.fatal
      << "Request to initialize a selection of size "
      << inputDesiredSubsetSize
      << " out of "
      << inputNumberOfItems
      << " elements, which does not make sense. "
      << global.fatal;
    }
    if (inputDesiredSubsetSize > inputNumberOfItems) {
      global.fatal
      << "Subset size not allowed to exceed the number of elements. "
      << global.fatal;
    }
    this->selection.initialize(inputNumberOfItems);
    this->desiredSubsetSize = inputDesiredSubsetSize;
    if (this->desiredSubsetSize > 0) {
      this->selection.incrementSelectionFixedCardinalityReturnFalseIfPastLast(
        this->desiredSubsetSize
      );
    }
  }
  std::string toString() const {
    return this->selection.toString();
  }
  bool incrementReturnFalseIfPastLast() {
    return
    this->selection.incrementSelectionFixedCardinalityReturnFalseIfPastLast(
      this->desiredSubsetSize
    );
  }
};

class SelectionPositiveIntegers {
public:
  Vector<LargeIntegerUnsigned> integers;
  std::string toString(FormatExpressions* format = nullptr) {
    (void) format;
    return this->integers.toString();
  }
  LargeIntegerUnsigned getGrading() {
    return this->integers.sumCoordinates();
  }
  void initialize(int numberOfIntegers) {
    this->integers.makeZero(numberOfIntegers);
  }
  void setFirstInGradeLevel(const LargeIntegerUnsigned& inputGradingLevel) {
    this->integers[0] = inputGradingLevel;
    for (int i = 1; i < this->integers.size; i ++) {
      this->integers[i] = 0;
    }
  }
  bool incrementReturnFalseIfPastLast() {
    for (int i = this->integers.size - 2; i >= 0; i --) {
      if (this->integers[i] > 0) {
        this->integers[i] --;
        this->integers[i + 1] = *this->integers.lastObject();
        this->integers[i + 1] ++;
        if (i != this->integers.size - 2) {
          *this->integers.lastObject() = 0;
        }
        return true;
      }
    }
    this->setFirstInGradeLevel(this->getGrading() + 1);
    return true;
  }
};

template <class Coefficient>
bool Vectors<Coefficient>::computeNormalFromSelectionAndTwoExtraRoots(
  Vector<Coefficient>& output,
  Vector<Coefficient>& extraRoot1,
  Vector<Coefficient>& extraRoot2,
  Selection& selection,
  Matrix<Coefficient>& bufferMatrix,
  Selection& bufferSelection
) {
  Selection& nonPivotPoints = bufferSelection;
  if (this->size == 0) {
    return false;
  }
  int dimension = this->objects[0].size;
  output.setSize(dimension);
  bufferMatrix.initialize(selection.cardinalitySelection + 2, dimension);
  for (int j = 0; j < dimension; j ++) {
    for (int i = 0; i < selection.cardinalitySelection; i ++) {
      bufferMatrix.elements[i][j].assign(
        this->objects[selection.elements[i]].objects[j]
      );
    }
    bufferMatrix.elements[selection.cardinalitySelection][j].assign(
      extraRoot1.objects[j]
    );
    bufferMatrix.elements[selection.cardinalitySelection + 1][j].assign(
      extraRoot2.objects[j]
    );
  }
  bufferMatrix.gaussianEliminationByRows(0, nonPivotPoints);
  if (nonPivotPoints.cardinalitySelection != 1) {
    return false;
  }
  bufferMatrix.nonPivotPointsToEigenVector(nonPivotPoints, output);
  return true;
}

template <typename Coefficient>
void Vectors<Coefficient>::selectionToMatrix(
  Selection& selection, int outputDimension, Matrix<Coefficient>& output
) {
  output.initialize(outputDimension, selection.cardinalitySelection);
  this->selectionToMatrix(selection, outputDimension, output, 0);
}

template <typename Coefficient>
void Vectors<Coefficient>::selectionToMatrixAppend(
  Selection& selection,
  int outputDimension,
  Matrix<Coefficient>& output,
  int startRowIndex
) {
  for (int i = 0; i < selection.cardinalitySelection; i ++) {
    Vector<Coefficient>& root = this->objects[selection.elements[i]];
    for (int j = 0; j < outputDimension; j ++) {
      output.elements[startRowIndex + i][j] = root[j];
    }
  }
}

template <typename Coefficient>
void Vectors<Coefficient>::selectionToMatrix(
  Selection& selection,
  int outputDimension,
  Matrix<Coefficient>& output,
  int startRowIndex
) {
  for (int i = 0; i < selection.cardinalitySelection; i ++) {
    Vector<Rational>& root = this->objects[selection.elements[i]];
    for (int j = 0; j < outputDimension; j ++) {
      output.elements[startRowIndex + i][j] = root[j];
    }
  }
}

template <class Coefficient>
bool Vectors<Coefficient>::computeNormalExcludingIndex(
  Vector<Coefficient>& output,
  int index,
  Matrix<Coefficient>& bufferMatrix
) {
  Selection nonPivotPoints;
  if (this->size == 0) {
    return false;
  }
  int dimension = this->objects[0].size;
  output.setSize(dimension);
  bufferMatrix.initialize(this->size - 1, dimension);
  int k = - 1;
  for (int i = 0; i < this->size; i ++) {
    if (i == index) {
      continue;
    }
    k ++;
    for (int j = 0; j < dimension; j ++) {
      bufferMatrix.elements[k][j] = (*this)[i][j];
    }
  }
  bufferMatrix.gaussianEliminationByRows(0, &nonPivotPoints);
  if (nonPivotPoints.cardinalitySelection != 1) {
    return false;
  }
  bufferMatrix.nonPivotPointsToEigenVectorHomogeneous(nonPivotPoints, output);
  return true;
}

template <class Coefficient>
bool Vectors<Coefficient>::computeNormalFromSelection(
  Vector<Coefficient>& output,
  Selection& selection,
  Matrix<Coefficient>& bufferMatrix,
  int dimension
) const {
  Selection nonPivotPoints;
  output.setSize(dimension);
  bufferMatrix.initialize(selection.cardinalitySelection, dimension);
  for (int i = 0; i < selection.cardinalitySelection; i ++) {
    for (int j = 0; j < dimension; j ++) {
      bufferMatrix.elements[i][j] =
      this->objects[selection.elements[i]].objects[j];
    }
  }
  bufferMatrix.gaussianEliminationByRows(0, &nonPivotPoints);
  if (nonPivotPoints.cardinalitySelection != 1) {
    return false;
  }
  bufferMatrix.nonPivotPointsToEigenVectorHomogeneous(nonPivotPoints, output);
  return true;
}

template <class Coefficient>
bool Vectors<Coefficient>::computeNormalFromSelectionAndExtraRoot(
  Vector<Coefficient>& output,
  Vector<Coefficient>& extraRoot,
  Selection& selection,
  Matrix<Coefficient>& bufferMatrix,
  Selection& bufferSelection
) {
  if (this->size == 0) {
    return false;
  }
  int dimension = this->objects[0].size;
  output.setSize(dimension);
  Matrix<Coefficient> matOutputEmpty;
  Selection& nonPivotPoints = bufferSelection;
  bufferMatrix.initialize(selection.cardinalitySelection + 1, dimension);
  matOutputEmpty.initialize(- 1, - 1);
  for (int j = 0; j < dimension; j ++) {
    for (int i = 0; i < selection.cardinalitySelection; i ++) {
      bufferMatrix.elements[i][j].assign(
        this->objects[selection.elements[i]][j]
      );
    }
    bufferMatrix.elements[selection.cardinalitySelection][j].assign(
      extraRoot[j]
    );
  }
  bufferMatrix.gaussianEliminationByRows(matOutputEmpty, nonPivotPoints);
  if (nonPivotPoints.cardinalitySelection != 1) {
    return false;
  }
  bufferMatrix.nonPivotPointsToEigenVector(nonPivotPoints, output);
  return true;
}

template <class Coefficient>
void Vectors<Coefficient>::gaussianEliminationForNormalComputation(
  Matrix<Coefficient>& inputMatrix,
  Selection& outputNonPivotPoints,
  int dimension
) const {
  inputMatrix.initialize(this->size, dimension);
  outputNonPivotPoints.initialize(dimension);
  for (int i = 0; i < this->size; i ++) {
    for (int j = 0; j < dimension; j ++) {
      inputMatrix(i, j) = (*this)[i][j];
    }
  }
  inputMatrix.gaussianEliminationByRows(0, &outputNonPivotPoints);
}

template <class Coefficient>
int Vectors<Coefficient>::getRankElementSpan(
  Matrix<Coefficient>* buffer, Selection* bufferSelection
) const {
  if (this->size == 0) {
    return 0;
  }
  int dimension = this->objects[0].size;
  MemorySaving<Matrix<Coefficient> > emergencyMatrixBuffer;
  MemorySaving<Selection> emergencySelectionBuffer;
  if (buffer == nullptr) {
    buffer = &emergencyMatrixBuffer.getElement();
  }
  if (bufferSelection == nullptr) {
    bufferSelection = &emergencySelectionBuffer.getElement();
  }
  this->gaussianEliminationForNormalComputation(
    *buffer, *bufferSelection, dimension
  );
  return (dimension - bufferSelection->cardinalitySelection);
}

template <class Coefficient>
bool Vectors<Coefficient>::getLinearDependence(
  Vector<Coefficient>& outputLinearCombination,
  bool homogeneous,
  const Coefficient& one,
  const Coefficient& zero
) {
  Matrix<Coefficient> eliminated;
  Selection nonPivotPoints;
  this->getLinearDependenceCompute(eliminated, nonPivotPoints);
  if (nonPivotPoints.cardinalitySelection == 0) {
    return false;
  }
  if (homogeneous) {
    eliminated.nonPivotPointsToEigenVectorHomogeneous(
      nonPivotPoints, outputLinearCombination, one, zero
    );
  } else {
    eliminated.nonPivotPointsToEigenVectorLexicographic(
      nonPivotPoints, outputLinearCombination, one, zero
    );
  }
  return true;
}

template <class Coefficient>
void Vector<Coefficient>::operator=(const Selection& other) {
  if (other.numberOfElements < 0) {
    this->setSize(0);
    return;
  }
  this->setSize(other.numberOfElements);
  for (int i = 0; i < other.numberOfElements; i ++) {
    if (other.selected[i]) {
      this->objects[i] = 1;
    } else {
      this->objects[i] = 0;
    }
  }
}

template <class Coefficient>
void Vector<Coefficient>::operator=(const SelectionWithMultiplicities& other) {
  this->setSize(other.multiplicities.size);
  for (int i = 0; i < other.multiplicities.size; i ++) {
    this->objects[i] = other.multiplicities[i];
  }
}

template <class Coefficient>
bool Vectors<Coefficient>::linearAlgebraForVertexComputation(
  Selection& selection,
  Vector<Coefficient>& output,
  Matrix<Coefficient>& buffer,
  Selection& nonPivotPointsBuffer
) {
  if (this->size == 0) {
    return false;
  }
  int dimension = this->objects[0].size;
  output.setSize(dimension);
  if (dimension - 1 != selection.cardinalitySelection) {
    global.fatal << "Dimensions don't match. " << global.fatal;
  }
  buffer.initialize(dimension - 1, dimension);
  for (int i = 0; i < dimension - 1; i ++) {
    for (int j = 0; j < dimension; j ++) {
      buffer.elements[i][j] =
      this->externalWalls[selection.elements[i]].normal[j];
    }
  }
  buffer.gaussianEliminationByRows(0, nonPivotPointsBuffer);
  if (nonPivotPointsBuffer.cardinalitySelection == 1) {
    buffer.nonPivotPointsToEigenVector(nonPivotPointsBuffer, output);
    return true;
  }
  return false;
}

#endif // header_math_subsets_selections_ALREADY_INCLUDED
