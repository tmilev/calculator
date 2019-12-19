// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader1_2SubsetsSelections_already_included
#define vpfHeader1_2SubsetsSelections_already_included

#include "general_lists.h"
#include "math_vectors.h"
#include "general_memory_saving.h"
static ProjectInformationInstance ProjectInfoVpfHeader1_5(__FILE__, "Header, selections, subsets, etc. ");

class Selection {
public:
  List<int> elements;
  List<bool> selected;
  int MaxSize;
  int CardinalitySelection;
  static std::string GetXMLClassName() {
    return "Selection";
  }
  void AddSelectionAppendNewIndex(int index);
  void RemoveLastSelection();
  void RemoveSelection(int index) {
    this->selected[index] = false;
    this->ComputeIndicesFromSelection();
  }
  void MakeFullSelection(int inputMaxSize) {
    this->init(inputMaxSize);
    this->MakeFullSelection();
  }
  void MakeFullSelection() {
    for (int i = 0; i < this->MaxSize; i ++) {
      this->elements[i] = i;
      this->selected[i] = true;
    }
    this->CardinalitySelection = this->MaxSize;
  }
  bool IsSubset(const Selection& other) const {
    if (this->MaxSize != other.MaxSize) {
      return false;
    }
    for (int i = 0; i < this->CardinalitySelection; i ++) {
      if (!other.selected[this->elements[i]]) {
        return false;
      }
    }
    return true;
  }
  void init(int maxNumElements);
  void ComputeIndicesFromSelection();
  void initNoMemoryAllocation();
  unsigned int HashFunction() const;
  static unsigned int HashFunction(const Selection& input) {
    return input.HashFunction();
  }
  std::string ToString() const;
  void incrementSelection();
  bool IncrementReturnFalseIfPastLast() {
    this->incrementSelection();
    if (this->CardinalitySelection == 0) {
      return false;
    }
    return true;
  }
  int SelectionToIndex();
  void ExpandMaxSize();
  int GetNumCombinationsFixedCardinality(int theCardinality) {
    return MathRoutines::NChooseK(this->MaxSize, theCardinality);
  }
  void ShrinkMaxSize();
  void MakeSubSelection(Selection& theSelection, Selection& theSubSelection);
  void initSelectionFixedCardinality(int card);
  void incrementSelectionFixedCardinality(int card);
  void WriteToFile(std::fstream& output);
  void ReadFromFile(std::fstream& input);
  void InvertSelection() {
    for (int i = 0; i < this->MaxSize; i ++) {
      this->selected[i] = !this->selected[i];
    }
    this->ComputeIndicesFromSelection();
  }
  void operator=(const Selection& right);
  void operator=(const Vector<Rational>& other);
//  void operator=(const std::string& other);
  //warning: to call the comparison operator sucessfully, cardinalitySelection must
  //be properly computed!
  bool operator==(const Selection& right) const {
    if (this->MaxSize != right.MaxSize || this->CardinalitySelection != right.CardinalitySelection) {
      return false;
    }
    for (int i = 0; i < this->CardinalitySelection; i ++) {
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
  Selection(const Selection& other): MaxSize(0), CardinalitySelection(0) {
    *this = other;
  }
};

class SelectionWithMultiplicities {
public:
  std::string DebugString;
  void ComputeDebugString() {
    this->ToString(this->DebugString);
  }
  void ToString(std::string& output);
  std::string ToString() {
    std::string tempS;
    this->ToString(tempS);
    return tempS;
  }
  List<int> elements;
  List<int> Multiplicities;
  static unsigned int HashFunction(const SelectionWithMultiplicities& input) {
    unsigned int result = 0;
    for (int i = 0; i < input.elements.size; i ++) {
      result += static_cast<unsigned int>(input.Multiplicities[input.elements[i]]) * SomeRandomPrimes[input.elements[i]];
    }
    return result;
  }
  int CardinalitySelectionWithoutMultiplicities();
  void initWithMultiplicities(int NumElements);
  void ComputeElements();
};

class SelectionWithMaxMultiplicity: public SelectionWithMultiplicities {
  void init(int NumElements);
  void InitMe(int NumElements);
  void initWithMultiplicities(int NumElements);
public:
  int MaxMultiplicity;
  void initMaxMultiplicity(int NumElements, int MaxMult);
  int NumCombinationsOfCardinality(int cardinality);
  LargeInteger GetNumTotalCombinations() const;
  int NumSelectionsTotal() {
    return MathRoutines::KToTheNth(MaxMultiplicity, this->Multiplicities.size);
  }
  bool IncrementReturnFalseIfPastLast();
  void IncrementSubset();
  void IncrementSubsetFixedCardinality(int Cardinality);
  bool HasMultiplicitiesZeroAndOneOnly();
  int MaxCardinalityWithMultiplicities() {
    return this->MaxMultiplicity*this->Multiplicities.size;
  }
  int CardinalitySelectionWithMultiplicities();
  static unsigned int HashFunction(const SelectionWithMaxMultiplicity& input) {
    return static_cast<unsigned int>(input.MaxMultiplicity) * SomeRandomPrimes[0] +
    input.::SelectionWithMultiplicities::HashFunction(input);
  }
};

class SelectionWithDifferentMaxMultiplicities {
public:
  List<int> elements;
  List<int> Multiplicities;
  List<int> MaxMultiplicities;
  void initPart1(int NumElements);
  void clearNoMaxMultiplicitiesChange();
  bool IncrementReturnFalseIfPastLast();
  LargeInteger TotalNumSubsets();
  int TotalNumSubsetsMustBeSmalInt();
  int TotalMultiplicity();
  int MaxTotalMultiplicity();
  void initFromInts(int* theMaxMults, int NumberMaxMults);
  void initFromInts(const List<int>& theMaxMults);
  bool HasSameMaxMultiplicities(SelectionWithDifferentMaxMultiplicities& other) {
    return this->MaxMultiplicities.IsEqualTo(other.MaxMultiplicities);
  }
  void operator=(const SelectionWithDifferentMaxMultiplicities& right) {
    this->Multiplicities = right.Multiplicities;
    this->MaxMultiplicities = right.MaxMultiplicities;
    this->elements = right.elements;
  }
};

class SelectionOneItem {
  public:
  int MaxMultiplicity;
  int SelectedMult;
  SelectionOneItem(): MaxMultiplicity(0), SelectedMult(- 1) {
   
  }
  bool IncrementReturnFalseIfPastLast() {
    if (this->MaxMultiplicity == 0) {
      return false;
    }
    this->SelectedMult ++;
    if (this->SelectedMult > this->MaxMultiplicity) {
      this->SelectedMult = 0;
    }
    return this->SelectedMult != 0;
  }
  void initFromMults(int theMult) {
    this->MaxMultiplicity = theMult;
    this->SelectedMult = 0;
  }
  std::string ToString() const {
    std::stringstream out;
    out << this->SelectedMult << " out of " << this->MaxMultiplicity;
    return out.str();
  }
};

template <class Base>
class Incrementable {
public:
  List<Base> theElements;
  Base& operator[](int i) {
    return this->theElements[i];
  }
  LargeInteger GetNumTotalCombinations() {
    LargeInteger result = 1;
    for (int i = 0; i < this->theElements.size; i ++) {
      result *= this->theElements[i].GetNumTotalCombinations();
    }
    return result;
  }
  bool IncrementReturnFalseIfPastLast() {
    for (int i = this->theElements.size - 1; i >= 0; i --) {
      if (this->theElements[i].IncrementReturnFalseIfPastLast()) {
        return true;
      }
    }
    return false;
  }
  void initFromMults(int inputBase, int numElts) {
    this->theElements.SetSize(numElts);
    for (int i = 0; i < this->theElements.size; i ++) {
      this->theElements[i].initFromMults(inputBase);
    }
  }
  template<class Element>
  void initFromMults(const List<Element>& input, int useOnlyNElementsOnly = 0) {
    if (useOnlyNElementsOnly > 0 && useOnlyNElementsOnly <= input.size) {
      this->theElements.SetSize(useOnlyNElementsOnly);
    } else {
      this->theElements.SetSize(input.size);
    }
    for (int i = 0; i < this->theElements.size; i ++) {
      this->theElements[i].initFromMults(input[i]);
    }
  }
  std::string ToString() const {
    std::stringstream out;
    out << "(";
    for (int i = 0; i < this->theElements.size; i ++) {
      out << this->theElements[i].ToString();
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
  int DesiredSubsetSize;
  LargeInteger GetNumTotalCombinations() {
    LargeInteger result;
    MathRoutines::NChooseK(theSelection.MaxSize, DesiredSubsetSize, result);
    return result;
  }
  void SetNumItemsAndDesiredSubsetSize(int inputDesiredSubsetSize, int inputNumItems) {
    if (inputDesiredSubsetSize < 0 || inputNumItems < 0) {
      global.fatal << "This is a programming error: requesting to initialize a selection of size "
      << inputDesiredSubsetSize << " out of "
      << inputNumItems << " elements, which does not make sense. " << global.fatal;
    }
    this->theSelection.init(inputNumItems);
    this->DesiredSubsetSize = inputDesiredSubsetSize;
    if (this->DesiredSubsetSize > 0) {
      this->theSelection.incrementSelectionFixedCardinality(this->DesiredSubsetSize);
    }
  }
  std::string ToString() const {
    return this->theSelection.ToString();
  }
  bool IncrementReturnFalseIfPastLast() {
    this->theSelection.incrementSelectionFixedCardinality(this->DesiredSubsetSize);
    for (int i = 0; i < this->DesiredSubsetSize; i ++) {
      if (!this->theSelection.selected[i]) {
        return true;
      }
    }
    return false;
  }
};

class SelectionPositiveIntegers {
  public:
  Vector<LargeIntegerUnsigned> theInts;
  std::string ToString(FormatExpressions* theFormat = nullptr) {
    (void) theFormat;//avoid unused parameter warning, portable
    return this->theInts.ToString();
  }
  LargeIntegerUnsigned GetGrading() {
    return this->theInts.SumCoords();
  }
  void init(int numIntegers) {
    this->theInts.MakeZero(numIntegers);
  }
  void SetFirstInGradeLevel(const LargeIntegerUnsigned& inputGradingLevel) {
    this->theInts[0] = inputGradingLevel;
    for (int i = 1; i < this->theInts.size; i ++) {
      this->theInts[i] = 0;
    }
  }
  bool IncrementReturnFalseIfPastLast() {
    for (int i = this->theInts.size - 2; i >= 0; i --) {
      if (this->theInts[i] > 0) {
        this->theInts[i] --;
        this->theInts[i + 1] = *this->theInts.LastObject();
        this->theInts[i + 1] ++;
        if (i != this->theInts.size - 2) {
          *this->theInts.LastObject() = 0;
        }
        return true;
      }
    }
    this->SetFirstInGradeLevel(this->GetGrading() + 1);
    return true;
  }
};

template<class coefficient>
bool Vectors<coefficient>::IsRegular(Vector<coefficient>& r, Vector<coefficient>& outputFailingNormal, int theDimension) {
  //this function needs a complete rewrite
  Selection WallSelection;
  WallSelection.init(this->size);
  int x = MathRoutines::NChooseK(this->size, theDimension - 1);
  Matrix<coefficient> bufferMat;
  Vector<coefficient> tempRoot;
  coefficient theScalarProduct;
  for (int i = 0; i < x; i ++) {
    WallSelection.incrementSelectionFixedCardinality(theDimension - 1);
    if (this->ComputeNormalFromSelection(tempRoot, WallSelection, bufferMat, theDimension)) {
      tempRoot.ScalarEuclidean(r, theScalarProduct);
      if (theScalarProduct.IsEqualToZero()) {
        outputFailingNormal = tempRoot;
        return false;
      }
    }
  }
  return true;
}

template<class coefficient>
bool Vectors<coefficient>::ComputeNormalFromSelectionAndTwoExtraRoots(
  Vector<coefficient>& output,
  Vector<coefficient>& ExtraRoot1,
  Vector<coefficient>& ExtraRoot2,
  Selection& theSelection,
  Matrix<coefficient>& bufferMat,
  Selection& bufferSel
) {
  Selection& NonPivotPoints = bufferSel;
  if (this->size == 0) {
    return false;
  }
  int theDimension = this->TheObjects[0].size;
  output.SetSize(theDimension);
  bufferMat.init(theSelection.CardinalitySelection + 2, theDimension);
  for (int j = 0; j < theDimension; j ++) {
    for (int i = 0; i < theSelection.CardinalitySelection; i ++) {
      bufferMat.elements[i][j].Assign(this->TheObjects[theSelection.elements[i]].TheObjects[j]);
    }
    bufferMat.elements[theSelection.CardinalitySelection][j].Assign(ExtraRoot1.TheObjects[j]);
    bufferMat.elements[theSelection.CardinalitySelection + 1][j].Assign(ExtraRoot2.TheObjects[j]);
  }
  bufferMat.GaussianEliminationByRows(0, NonPivotPoints);
  if (NonPivotPoints.CardinalitySelection != 1) {
    return false;
  }
  bufferMat.NonPivotPointsToEigenVector(NonPivotPoints, output);
  return true;
}

template <typename coefficient>
void Vectors<coefficient>::SelectionToMatrix(
  Selection& theSelection, int OutputDimension, Matrix<coefficient>& output
) {
  output.init(OutputDimension, theSelection.CardinalitySelection);
  this->SelectionToMatrix(theSelection, OutputDimension, output, 0);
}

template <typename coefficient>
void Vectors<coefficient>::SelectionToMatrixAppend(
  Selection& theSelection, int OutputDimension, Matrix<coefficient>& output, int StartRowIndex
) {
  for (int i = 0; i < theSelection.CardinalitySelection; i ++) {
    Vector<coefficient>& tempRoot = this->TheObjects[theSelection.elements[i]];
    for (int j = 0; j < OutputDimension; j ++) {
      output.elements[StartRowIndex + i][j] = tempRoot[j];
    }
  }
}

template <typename coefficient>
void Vectors<coefficient>::SelectionToMatrix(
  Selection& theSelection, int OutputDimension, Matrix<coefficient>& output, int StartRowIndex
) {
  for (int i = 0; i < theSelection.CardinalitySelection; i ++) {
    Vector<Rational>& tempRoot = this->TheObjects[theSelection.elements[i]];
    for (int j = 0; j < OutputDimension; j ++) {
      output.elements[StartRowIndex + i][j] = tempRoot[j];
    }
  }
}

template<class coefficient>
bool Vectors<coefficient>::ComputeNormalExcludingIndex(
  Vector<coefficient>& output, int index, Matrix<coefficient>& bufferMatrix
) {
  Selection NonPivotPoints;
  if (this->size == 0) {
    return false;
  }
  int theDimension = this->TheObjects[0].size;
  output.SetSize(theDimension);
  bufferMatrix.init(this->size - 1, theDimension);
  int k = - 1;
  for (int i = 0; i < this->size; i ++) {
    if (i != index) {
      k ++;
      for (int j = 0; j < theDimension; j ++) {
        bufferMatrix.elements[k][j] = (*this)[i][j];
      }
    }
  }
  bufferMatrix.GaussianEliminationByRows(0, &NonPivotPoints);
  if (NonPivotPoints.CardinalitySelection != 1) {
    return false;
  }
  bufferMatrix.NonPivotPointsToEigenVector(NonPivotPoints, output);
  return true;
}

template<class coefficient>
bool Vectors<coefficient>::ComputeNormalFromSelection(
  Vector<coefficient>& output, Selection& theSelection, Matrix<coefficient>& bufferMatrix, int theDimension
) {
  Selection NonPivotPoints;
  output.SetSize(theDimension);
  bufferMatrix.init(theSelection.CardinalitySelection, theDimension);
  for (int i = 0; i < theSelection.CardinalitySelection; i ++) {
    for (int j = 0; j < theDimension; j ++) {
      bufferMatrix.elements[i][j] = this->TheObjects[theSelection.elements[i]].TheObjects[j];
    }
  }
  bufferMatrix.GaussianEliminationByRows(0, &NonPivotPoints);
  if (NonPivotPoints.CardinalitySelection != 1) {
    return false;
  }
  bufferMatrix.NonPivotPointsToEigenVector(NonPivotPoints, output);
  return true;
}

template<class coefficient>
bool Vectors<coefficient>::ComputeNormalFromSelectionAndExtraRoot(
  Vector<coefficient>& output,
  Vector<coefficient>& ExtraRoot,
  Selection& theSelection,
  Matrix<coefficient>& bufferMatrix,
  Selection& bufferSel
) {
  if (this->size == 0) {
    return false;
  }
  int theDimension = this->TheObjects[0].size;
  output.SetSize(theDimension);
  Matrix<coefficient> matOutputEmpty;
  Selection& NonPivotPoints = bufferSel;
  bufferMatrix.init(theSelection.CardinalitySelection + 1, theDimension);
  matOutputEmpty.init(- 1, - 1);
  for (int j = 0; j < theDimension; j ++) {
    for (int i = 0; i < theSelection.CardinalitySelection; i ++) {
      bufferMatrix.elements[i][j].Assign(this->TheObjects[theSelection.elements[i]][j]);
    }
    bufferMatrix.elements[theSelection.CardinalitySelection][j].Assign(ExtraRoot[j]);
  }
  bufferMatrix.GaussianEliminationByRows(matOutputEmpty, NonPivotPoints);
  if (NonPivotPoints.CardinalitySelection != 1) {
    return false;
  }
  bufferMatrix.NonPivotPointsToEigenVector(NonPivotPoints, output);
  return true;
}

template <class coefficient>
void Vectors<coefficient>::GaussianEliminationForNormalComputation(
  Matrix<coefficient>& inputMatrix, Selection& outputNonPivotPoints, int theDimension
) const {
  inputMatrix.init(this->size, theDimension);
  outputNonPivotPoints.init(theDimension);
  for (int i = 0; i < this->size; i ++) {
    for (int j = 0; j < theDimension; j ++) {
      inputMatrix(i, j) = (*this)[i][j];
    }
  }
  inputMatrix.GaussianEliminationByRows(0, &outputNonPivotPoints);
}

template <class coefficient>
int Vectors<coefficient>::GetRankOfSpanOfElements(Matrix<coefficient>* buffer, Selection* bufferSelection) const {
  if (this->size == 0) {
    return 0;
  }
  int theDimension = this->TheObjects[0].size;
  MemorySaving<Matrix<coefficient> > emergencyMatBuf;
  MemorySaving<Selection> emergencySelBuf;
  if (buffer == nullptr) {
    buffer = &emergencyMatBuf.GetElement();
  }
  if (bufferSelection == nullptr) {
    bufferSelection = &emergencySelBuf.GetElement();
  }
  this->GaussianEliminationForNormalComputation(*buffer, *bufferSelection, theDimension);
  return (theDimension - bufferSelection->CardinalitySelection);
}

template <class coefficient>
bool Vectors<coefficient>::GetLinearDependence(Matrix<coefficient>& outputTheLinearCombination) {
  Matrix<coefficient> tempMat;
  Selection nonPivotPoints;
  this->GetLinearDependenceRunTheLinearAlgebra(tempMat, nonPivotPoints);
  if (nonPivotPoints.CardinalitySelection == 0) {
    return false;
  }
  tempMat.NonPivotPointsToEigenVectorMatrixForm(nonPivotPoints, outputTheLinearCombination);
  return true;
}

template <class coefficient>
void Vector<coefficient>::operator=(const Selection& other) {
  if (other.MaxSize < 0) {
    this->SetSize(0);
    return;
  }
  this->SetSize(other.MaxSize);
  for (int i = 0; i < other.MaxSize; i ++) {
    if (other.selected[i]) {
      this->TheObjects[i] = 1;
    } else {
      this->TheObjects[i] = 0;
    }
  }
}

template <class coefficient>
void Vector<coefficient>::operator=(const SelectionWithMultiplicities& other) {
  this->SetSize(other.Multiplicities.size);
  for (int i = 0; i < other.Multiplicities.size; i ++) {
    this->TheObjects[i] = other.Multiplicities[i];
  }
}

template <class coefficient>
bool Vectors<coefficient>::LinearAlgebraForVertexComputation(
  Selection& theSelection, Vector<coefficient>& output, Matrix<coefficient>& buffer, Selection& NonPivotPointsBuffer
) {
  if (this->size == 0) {
    return false;
  }
  int theDimension = this->TheObjects[0].size;
  output.SetSize(theDimension);
  if (theDimension - 1 != theSelection.CardinalitySelection) {
    global.fatal << "Dimensions don't match. " << global.fatal;
  }
  buffer.init(theDimension - 1, theDimension);
  for (int i = 0; i < theDimension - 1; i ++) {
    for (int j = 0; j < theDimension; j ++) {
      buffer.elements[i][j] = (this->Externalwalls[theSelection.elements[i]].normal[j]);
    }
  }
  buffer.GaussianEliminationByRows(0, NonPivotPointsBuffer);
  if (NonPivotPointsBuffer.CardinalitySelection == 1) {
    buffer.NonPivotPointsToEigenVector(NonPivotPointsBuffer, output);
    return true;
  }
  return false;
}

#endif
