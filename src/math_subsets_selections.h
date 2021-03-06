// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader1_2SubsetsSelections_already_included
#define vpfHeader1_2SubsetsSelections_already_included

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
  LargeInteger getNumberOfCombinationsFixedCardinality(int theCardinality) {
    return MathRoutines::nChooseK(this->numberOfElements, theCardinality);
  }
  void shrinkMaxSize();
  void makeSubSelection(Selection& theSelection, Selection& theSubSelection);
  void initSelectionFixedCardinality(int card);
  void incrementSelectionFixedCardinality(int card);
  void invertSelection() {
    for (int i = 0; i < this->numberOfElements; i ++) {
      this->selected[i] = !this->selected[i];
    }
    this->computeIndicesFromSelection();
  }
  void operator=(const Selection& right);
  void operator=(const Vector<Rational>& other);
  // void operator=(const std::string& other);
  // warning: to call the comparison operator sucessfully, cardinalitySelection must
  // be properly computed!
  bool operator==(const Selection& right) const {
    if (this->numberOfElements != right.numberOfElements || this->cardinalitySelection != right.cardinalitySelection) {
      return false;
    }
    for (int i = 0; i < this->cardinalitySelection; i ++) {
      if (this->selected[this->elements[i]] != right.selected[this->elements[i]]) {
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
  Selection(const Selection& other): numberOfElements(0), cardinalitySelection(0) {
    *this = other;
  }
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
      result += static_cast<unsigned int>(input.multiplicities[input.elements[i]]) * someRandomPrimes[input.elements[i]];
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
    return MathRoutines::kToTheNth(maximumMultiplicity, this->multiplicities.size);
  }
  bool incrementReturnFalseIfPastLast();
  void incrementSubset();
  void incrementSubsetFixedCardinality(int Cardinality);
  bool hasMultiplicitiesZeroAndOneOnly();
  int maximumCardinalityWithMultiplicities() {
    return this->maximumMultiplicity * this->multiplicities.size;
  }
  int cardinalitySelectionWithMultiplicities();
  static unsigned int hashFunction(const SelectionWithMaxMultiplicity& input) {
    return static_cast<unsigned int>(input.maximumMultiplicity) * someRandomPrimes[0] +
    input.::SelectionWithMultiplicities::hashFunction(input);
  }
};

class SelectionWithDifferentMaxMultiplicities {
public:
  List<int> elements;
  List<int> multiplicities;
  List<int> capacities;
  void initPart1(int numElements);
  void clearNoMaxMultiplicitiesChange();
  bool incrementReturnFalseIfPastLast();
  LargeInteger totalNumberOfSubsets();
  int totalNumberSubsetsSmallInt();
  int totalMultiplicity();
  int maximumTotalMultiplicity();
  void initFromInts(int* theMaxMults, int numberMaxMults);
  void initFromInts(const List<int>& theMaxMults);
  bool hasSameMaxMultiplicities(SelectionWithDifferentMaxMultiplicities& other) {
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
  SelectionOneItem(): maximumMultiplicity(0), amount(- 1) {
   
  }
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
  void initFromMults(int theMult) {
    this->maximumMultiplicity = theMult;
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
  List<Base> theElements;
  Base& operator[](int i) {
    return this->theElements[i];
  }
  LargeInteger totalCombinations() {
    LargeInteger result = 1;
    for (int i = 0; i < this->theElements.size; i ++) {
      result *= this->theElements[i].totalCombinations();
    }
    return result;
  }
  bool incrementReturnFalseIfPastLast() {
    for (int i = this->theElements.size - 1; i >= 0; i --) {
      if (this->theElements[i].incrementReturnFalseIfPastLast()) {
        return true;
      }
    }
    return false;
  }
  void initFromMults(int inputBase, int numElts) {
    this->theElements.setSize(numElts);
    for (int i = 0; i < this->theElements.size; i ++) {
      this->theElements[i].initFromMults(inputBase);
    }
  }
  template<class Element>
  void initFromMults(const List<Element>& input, int useOnlyNElementsOnly = 0) {
    if (useOnlyNElementsOnly > 0 && useOnlyNElementsOnly <= input.size) {
      this->theElements.setSize(useOnlyNElementsOnly);
    } else {
      this->theElements.setSize(input.size);
    }
    for (int i = 0; i < this->theElements.size; i ++) {
      this->theElements[i].initFromMults(input[i]);
    }
  }
  std::string toString() const {
    std::stringstream out;
    out << "(";
    for (int i = 0; i < this->theElements.size; i ++) {
      out << this->theElements[i].toString();
      if (i != this->theElements.size - 1) {
        out << ", ";
      }
    }
    out << ")";
    return out.str();
  }
};

class SelectionFixedRank {
public:
  Selection theSelection;
  int desiredSubsetSize;
  LargeInteger totalCombinations() {
    LargeInteger result;
    MathRoutines::nChooseK(theSelection.numberOfElements, desiredSubsetSize, result);
    return result;
  }
  void setNumberOfItemsAndDesiredSubsetSize(int inputDesiredSubsetSize, int inputNumItems) {
    if (inputDesiredSubsetSize < 0 || inputNumItems < 0) {
      global.fatal << "Request to initialize a selection of size "
      << inputDesiredSubsetSize << " out of "
      << inputNumItems << " elements, which does not make sense. " << global.fatal;
    }
    this->theSelection.initialize(inputNumItems);
    this->desiredSubsetSize = inputDesiredSubsetSize;
    if (this->desiredSubsetSize > 0) {
      this->theSelection.incrementSelectionFixedCardinality(this->desiredSubsetSize);
    }
  }
  std::string toString() const {
    return this->theSelection.toString();
  }
  bool incrementReturnFalseIfPastLast() {
    this->theSelection.incrementSelectionFixedCardinality(this->desiredSubsetSize);
    for (int i = 0; i < this->desiredSubsetSize; i ++) {
      if (!this->theSelection.selected[i]) {
        return true;
      }
    }
    return false;
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
  void initialize(int numIntegers) {
    this->integers.makeZero(numIntegers);
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

template<class Coefficient>
bool Vectors<Coefficient>::computeNormalFromSelectionAndTwoExtraRoots(Vector<Coefficient>& output,
  Vector<Coefficient>& extraRoot1,
  Vector<Coefficient>& extraRoot2,
  Selection& theSelection,
  Matrix<Coefficient>& bufferMat,
  Selection& bufferSel
) {
  Selection& nonPivotPoints = bufferSel;
  if (this->size == 0) {
    return false;
  }
  int theDimension = this->objects[0].size;
  output.setSize(theDimension);
  bufferMat.initialize(theSelection.cardinalitySelection + 2, theDimension);
  for (int j = 0; j < theDimension; j ++) {
    for (int i = 0; i < theSelection.cardinalitySelection; i ++) {
      bufferMat.elements[i][j].assign(this->objects[theSelection.elements[i]].objects[j]);
    }
    bufferMat.elements[theSelection.cardinalitySelection][j].assign(extraRoot1.objects[j]);
    bufferMat.elements[theSelection.cardinalitySelection + 1][j].assign(extraRoot2.objects[j]);
  }
  bufferMat.gaussianEliminationByRows(0, nonPivotPoints);
  if (nonPivotPoints.cardinalitySelection != 1) {
    return false;
  }
  bufferMat.nonPivotPointsToEigenVector(nonPivotPoints, output);
  return true;
}

template <typename Coefficient>
void Vectors<Coefficient>::selectionToMatrix(
  Selection& theSelection, int outputDimension, Matrix<Coefficient>& output
) {
  output.initialize(outputDimension, theSelection.cardinalitySelection);
  this->selectionToMatrix(theSelection, outputDimension, output, 0);
}

template <typename Coefficient>
void Vectors<Coefficient>::selectionToMatrixAppend(
  Selection& theSelection,
  int outputDimension,
  Matrix<Coefficient>& output,
  int startRowIndex
) {
  for (int i = 0; i < theSelection.cardinalitySelection; i ++) {
    Vector<Coefficient>& tempRoot = this->objects[theSelection.elements[i]];
    for (int j = 0; j < outputDimension; j ++) {
      output.elements[startRowIndex + i][j] = tempRoot[j];
    }
  }
}

template <typename Coefficient>
void Vectors<Coefficient>::selectionToMatrix(
  Selection& theSelection,
  int outputDimension,
  Matrix<Coefficient>& output,
  int startRowIndex
) {
  for (int i = 0; i < theSelection.cardinalitySelection; i ++) {
    Vector<Rational>& tempRoot = this->objects[theSelection.elements[i]];
    for (int j = 0; j < outputDimension; j ++) {
      output.elements[startRowIndex + i][j] = tempRoot[j];
    }
  }
}

template<class Coefficient>
bool Vectors<Coefficient>::computeNormalExcludingIndex(
  Vector<Coefficient>& output, int index, Matrix<Coefficient>& bufferMatrix
) {
  Selection nonPivotPoints;
  if (this->size == 0) {
    return false;
  }
  int theDimension = this->objects[0].size;
  output.setSize(theDimension);
  bufferMatrix.initialize(this->size - 1, theDimension);
  int k = - 1;
  for (int i = 0; i < this->size; i ++) {
    if (i != index) {
      k ++;
      for (int j = 0; j < theDimension; j ++) {
        bufferMatrix.elements[k][j] = (*this)[i][j];
      }
    }
  }
  bufferMatrix.gaussianEliminationByRows(0, &nonPivotPoints);
  if (nonPivotPoints.cardinalitySelection != 1) {
    return false;
  }
  bufferMatrix.nonPivotPointsToEigenVector(nonPivotPoints, output);
  return true;
}

template<class Coefficient>
bool Vectors<Coefficient>::computeNormalFromSelection(
  Vector<Coefficient>& output,
  Selection& theSelection,
  Matrix<Coefficient>& bufferMatrix,
  int theDimension
) const {
  Selection nonPivotPoints;
  output.setSize(theDimension);
  bufferMatrix.initialize(theSelection.cardinalitySelection, theDimension);
  for (int i = 0; i < theSelection.cardinalitySelection; i ++) {
    for (int j = 0; j < theDimension; j ++) {
      bufferMatrix.elements[i][j] = this->objects[theSelection.elements[i]].objects[j];
    }
  }
  bufferMatrix.gaussianEliminationByRows(0, &nonPivotPoints);
  if (nonPivotPoints.cardinalitySelection != 1) {
    return false;
  }
  bufferMatrix.nonPivotPointsToEigenVector(nonPivotPoints, output);
  return true;
}

template<class Coefficient>
bool Vectors<Coefficient>::computeNormalFromSelectionAndExtraRoot(
  Vector<Coefficient>& output,
  Vector<Coefficient>& ExtraRoot,
  Selection& theSelection,
  Matrix<Coefficient>& bufferMatrix,
  Selection& bufferSel
) {
  if (this->size == 0) {
    return false;
  }
  int theDimension = this->objects[0].size;
  output.setSize(theDimension);
  Matrix<Coefficient> matOutputEmpty;
  Selection& nonPivotPoints = bufferSel;
  bufferMatrix.initialize(theSelection.cardinalitySelection + 1, theDimension);
  matOutputEmpty.initialize(- 1, - 1);
  for (int j = 0; j < theDimension; j ++) {
    for (int i = 0; i < theSelection.cardinalitySelection; i ++) {
      bufferMatrix.elements[i][j].assign(this->objects[theSelection.elements[i]][j]);
    }
    bufferMatrix.elements[theSelection.cardinalitySelection][j].assign(ExtraRoot[j]);
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
  Matrix<Coefficient>& inputMatrix, Selection& outputNonPivotPoints, int theDimension
) const {
  inputMatrix.initialize(this->size, theDimension);
  outputNonPivotPoints.initialize(theDimension);
  for (int i = 0; i < this->size; i ++) {
    for (int j = 0; j < theDimension; j ++) {
      inputMatrix(i, j) = (*this)[i][j];
    }
  }
  inputMatrix.gaussianEliminationByRows(0, &outputNonPivotPoints);
}

template <class Coefficient>
int Vectors<Coefficient>::getRankElementSpan(Matrix<Coefficient>* buffer, Selection* bufferSelection) const {
  if (this->size == 0) {
    return 0;
  }
  int theDimension = this->objects[0].size;
  MemorySaving<Matrix<Coefficient> > emergencyMatBuf;
  MemorySaving<Selection> emergencySelBuf;
  if (buffer == nullptr) {
    buffer = &emergencyMatBuf.getElement();
  }
  if (bufferSelection == nullptr) {
    bufferSelection = &emergencySelBuf.getElement();
  }
  this->gaussianEliminationForNormalComputation(*buffer, *bufferSelection, theDimension);
  return (theDimension - bufferSelection->cardinalitySelection);
}

template <class Coefficient>
bool Vectors<Coefficient>::getLinearDependence(Matrix<Coefficient>& outputTheLinearCombination) {
  Matrix<Coefficient> tempMat;
  Selection nonPivotPoints;
  this->getLinearDependenceRunTheLinearAlgebra(tempMat, nonPivotPoints);
  if (nonPivotPoints.cardinalitySelection == 0) {
    return false;
  }
  tempMat.nonPivotPointsToEigenVectorMatrixForm(nonPivotPoints, outputTheLinearCombination);
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
  Selection& theSelection,
  Vector<Coefficient>& output,
  Matrix<Coefficient>& buffer,
  Selection& nonPivotPointsBuffer
) {
  if (this->size == 0) {
    return false;
  }
  int theDimension = this->objects[0].size;
  output.setSize(theDimension);
  if (theDimension - 1 != theSelection.cardinalitySelection) {
    global.fatal << "Dimensions don't match. " << global.fatal;
  }
  buffer.initialize(theDimension - 1, theDimension);
  for (int i = 0; i < theDimension - 1; i ++) {
    for (int j = 0; j < theDimension; j ++) {
      buffer.elements[i][j] = (this->externalWalls[theSelection.elements[i]].normal[j]);
    }
  }
  buffer.gaussianEliminationByRows(0, nonPivotPointsBuffer);
  if (nonPivotPointsBuffer.cardinalitySelection == 1) {
    buffer.nonPivotPointsToEigenVector(nonPivotPointsBuffer, output);
    return true;
  }
  return false;
}

#endif
