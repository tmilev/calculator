//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#ifndef vpfHeader1_2_h_already_included
#define vpfHeader1_2_h_already_included

#include "general_logging_global_variables.h"
#include "general_strings.h"
#include "math_large_integers.h"
#include "math_vectors.h"
#include "math_subsets_selections.h"
#include "general_file_operations_encodings.h"
#include "html_snippets.h"

static ProjectInformationInstance ProjectInfoVpfHeader1_2(__FILE__, "Header, math routines. ");

// We are wrapping the math.h c++ functions for portability reasons
// (if for some reason we want to change from math.h to a better floating point
// library, we only need to change the implementation of the FloatingPoint class.
class FloatingPoint {
public:
  static double abs(double argument);
  static double sin(double argument);
  static double cos(double argument);
  static double arccos(double argument);
  static double arctan(double argument);
  static double arcsin(double argument);
  static double sqrt(double argument);
  static double power(double base, double exponent);
  static double log(double argument);
  static double floor(double argument);
  static std::string DoubleToString(double input);
};

class WeylGroupData;
class AlgebraicClosureRationals;
template <class coefficient>
class MatrixTensor;

class ChevalleyGenerator {
public:
  SemisimpleLieAlgebra* owner;
  int theGeneratorIndex;
  ChevalleyGenerator(): owner(nullptr), theGeneratorIndex(- 1) {
  }
  friend std::ostream& operator << (std::ostream& output, const ChevalleyGenerator& theGen) {
    output << theGen.ToString();
    return output;
  }
  bool CheckInitialization() const;
  static bool IsMonEqualToZero() {
    return false;
  }
  static unsigned int HashFunction(const ChevalleyGenerator& input) {
    return static_cast<unsigned>(input.theGeneratorIndex);
  }
  unsigned int HashFunction() const {
    return this->HashFunction(*this);
  }
  void MakeGenerator(SemisimpleLieAlgebra& inputOwner, int inputGeneratorIndex) {
    this->owner = &inputOwner;
    this->theGeneratorIndex = inputGeneratorIndex;
  }
  void operator=(const ChevalleyGenerator& other) {
    this->owner = other.owner;
    this->theGeneratorIndex = other.theGeneratorIndex;
  }
  bool operator>(const ChevalleyGenerator& other) const;
  std::string ToString(FormatExpressions* inputFormat = nullptr) const;
  void CheckConsistencyWithOther(const ChevalleyGenerator& other) const;
  bool operator==(const ChevalleyGenerator& other) const {
    this->CheckConsistencyWithOther(other);
    return this->theGeneratorIndex == other.theGeneratorIndex;
  }
};

template <class coefficient, unsigned int inputHashFunction(const coefficient&)= coefficient::HashFunction>
class MonomialTensor {
  friend std::ostream& operator << (std::ostream& output, const MonomialTensor<coefficient, inputHashFunction>& theMon) {
    return output << theMon.ToString();
  }
private:
public:
  List<int> generatorsIndices;
  List<coefficient> Powers;
  bool flagDeallocated;
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
  bool IsEqualToOne() const {
    return this->generatorsIndices.size == 0;
  }
  bool IsMonEqualToZero() const {
    return false;
  }
  void operator=(List<int>& other) {
    this->generatorsIndices.Reserve(other.size);
    this->Powers.Reserve(other.size);
    this->MakeConst();
    for (int i = 0; i < other.size; i ++) {
      this->MultiplyByGeneratorPowerOnTheRight(other[i], 1);
    }
  }
  bool CheckConsistency() {
    if (this->flagDeallocated) {
      crash << "Programming error: use after free of MonomialTensor." << crash;
    }
    return true;
  }
  void operator=(const MonomialTensor<coefficient, inputHashFunction>& other) {
    this->generatorsIndices = other.generatorsIndices;
    this->Powers = other.Powers;
  }
  MonomialTensor() {
    this->flagDeallocated = false;
  }
  ~MonomialTensor() {
    this->flagDeallocated = true;
  }
  int GetMinNumVars() {
    int result = 0;
    for (int i = 0; i < this->Powers.size; i ++) {
      result = MathRoutines::Maximum(result, this->Powers[i].GetMinNumVars());
    }
    return result;
  }
  template<class otherType>
  void operator=(const MonomialTensor<otherType>& other) {
    this->generatorsIndices = other.generatorsIndices;
    this->Powers.SetSize(other.Powers.size);
    for (int i = 0; i < other.Powers.size; i ++) {
      this->Powers[i] = other.Powers[i];
    }
  }
  bool SimplifyEqualConsecutiveGenerators(int lowestNonReducedIndex);
  void MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, const coefficient& thePower);
  void MultiplyByGeneratorPowerOnTheLeft(int theGeneratorIndexStandsToTheLeft, const coefficient& thePower);
  unsigned int HashFunction() const {
    int top = MathRoutines::Minimum(SomeRandomPrimesSize, this->generatorsIndices.size);
    unsigned int result = 0;
    for (int i = 0; i < top; i ++) {
      result += SomeRandomPrimes[i] * this->generatorsIndices[i] +
      SomeRandomPrimes[top - 1 - i] * inputHashFunction(this->Powers[i]);
    }
    return result;
  }
  static unsigned int HashFunction(const MonomialTensor<coefficient, inputHashFunction>& input) {
    return input.HashFunction();
  }
  void MakeConst() {
    this->generatorsIndices.size = 0;
    this->Powers.size = 0;
  }
  bool operator>(const MonomialTensor<coefficient, inputHashFunction>& other) const {
    if (other.generatorsIndices.size > this->generatorsIndices.size) {
      return false;
    }
    if (other.generatorsIndices.size < this->generatorsIndices.size) {
      return true;
    }
    for (int i = 0; i < this->generatorsIndices.size; i ++) {
      if (other.generatorsIndices[i] > this->generatorsIndices[i]) {
        return false;
      }
      if (other.generatorsIndices[i] < this->generatorsIndices[i]) {
        return true;
      }
      if (other.Powers[i] > this->Powers[i]) {
        return false;
      }
      if (this->Powers[i] > other.Powers[i]) {
        return true;
      }
    }
    return false;
  }
  bool operator==(const MonomialTensor<coefficient, inputHashFunction>& other) const {
    return this->Powers == other.Powers && this->generatorsIndices == other.generatorsIndices;
  }
  bool operator<(const MonomialTensor<coefficient, inputHashFunction>& right) const {
    coefficient leftrank = 0;
    for (int i = 0; i < this->Powers.size; i ++) {
      leftrank += this->Powers[i];
    }
    coefficient rightrank = 0;
    for (int i = 0; i < right.Powers.size; i ++) {
      rightrank += right.Powers[i];
    }
    if (leftrank < rightrank) {
      return true;
    }
    if (rightrank < leftrank) {
      return false;
    }
    // this does not crash for coefficient = int
    for (int i = 0; i < this->generatorsIndices.size; i ++) {
      if (this->generatorsIndices[i] < right.generatorsIndices[i]) {
        return false;
      }
      if (right.generatorsIndices[i] < this->generatorsIndices[i]) {
        return true;
      }
      if (this->Powers[i] < right.Powers[i]) {
        return false;
      }
      if (right.Powers[i] < this->Powers[i]) {
        return true;
      }
    }
    return false;
  }
  void operator*=(const MonomialTensor<coefficient, inputHashFunction>& standsOnTheRight) {
    if (standsOnTheRight.generatorsIndices.size == 0) {
      return;
    }
    if (this == &standsOnTheRight) {
      MonomialTensor<coefficient, inputHashFunction> tempMon;
      tempMon = standsOnTheRight;
      (*this) *= (tempMon);
      return;
    }
    this->generatorsIndices.SetExpectedSize(standsOnTheRight.generatorsIndices.size + this->generatorsIndices.size);
    this->Powers.SetExpectedSize(standsOnTheRight.generatorsIndices.size + this->generatorsIndices.size);
    int firstIndex = standsOnTheRight.generatorsIndices[0];
    int i = 0;
    if (this->generatorsIndices.size > 0) {
      if (firstIndex == (*this->generatorsIndices.LastObject())) {
        *this->Powers.LastObject() += standsOnTheRight.Powers[0];
        i = 1;
      }
    }
    for (; i <standsOnTheRight.generatorsIndices.size; i ++) {
      this->Powers.AddOnTop(standsOnTheRight.Powers[i]);
      this->generatorsIndices.AddOnTop(standsOnTheRight.generatorsIndices[i]);
    }
  }
};

template<typename theType, unsigned int hashFunction(const theType&) = theType::HashFunction>
class MonomialWrapper {
  public:
  theType theObject;
  MonomialWrapper() {
  }
  MonomialWrapper(const theType& input) {
    this->theObject = input;
  }
  std::string ToString(FormatExpressions* theFormat = nullptr) const {
    (void) theFormat;
    std::stringstream out;
    out << "(" << this->theObject << ")";
    return out.str();
  }
  bool operator>(const MonomialWrapper& other) const {
    return this->theObject > other.theObject;
  }
  friend std::ostream& operator << (std::ostream& output, const MonomialWrapper& theMon) {
    output << theMon.theObject;
    return output;
  }
  static unsigned int HashFunction(const MonomialWrapper& input) {
    return hashFunction(input.theObject);
  }
  bool IsMonEqualToZero() const {
    return false;
  }
  bool operator==(const MonomialWrapper& other) const {
    return this->theObject == other.theObject;
  }
};

class MonomialP {
private:
  // monbody contains the exponents of the variables.
  // IMPORTANT. The monBody of a monomial is NOT unique.
  // Two monomials whose non-zero entries coincide
  // (but otherwise one monomial might have more entries filled with zeroes)
  // are considered to be equal.
  // Therefore special attention must be paid when performing operations with
  // MonomialP's, especially with operator== and MonomialP::HashFunction!
  // Please modify this class in accordance with what was just explained.
  // Note that by the above token I decided to declare operator[] as non-const
  // function and operator() as a const function but returning a copy of the
  // underlying element, rather than a reference to the element.
  //
  // IMPORTANT. The default monomial order, implemented by operator>, is the graded lexicographic
  // last variable strongest order. Other monomial orders are not referred by operator>, but
  // by their explicit names.
  // Note that the MonomialCollection::ToString method uses the FormatExpressions::thePolyMonOrder
  // to sort monomials when displaying polynomials to the screen.
  List<Rational> monBody;
public:
  MonomialP(int letterIndex) {
    this->monBody.initializeFillInObject(letterIndex + 1, 0);
    this->monBody[letterIndex] = 1;
  }
  MonomialP(int letterIndex, int power) {
    this->monBody.initializeFillInObject(letterIndex + 1, 0);
    this->monBody[letterIndex] = power;
  }
  MonomialP(const MonomialP& other) {
    *this = other;
  }
  MonomialP() {
  }
  friend std::ostream& operator << (std::ostream& output, const MonomialP& theMon) {
    output << theMon.ToString();
    return output;
  }
  Rational& operator[](int i) {
    if (i < 0) {
      crash << "This is a programming error: requested exponent of monomial variable with index "
      << i << " which is negative. " << crash;
    }
    if (i >= this->monBody.size) {
      this->SetNumVariablesSubDeletedVarsByOne(i + 1);
    }
    return this->monBody[i];
  }
  Rational operator()(int i) const {
    if (i < 0) {
      crash << "This is a programming error: requested exponent of monomial variable "
      << " with index " << i << " which is negative. " << crash;
    }
    if (i >= this->monBody.size) {
      return 0;
    }
    return this->monBody[i];
  }
  static std::string GetXMLClassName() {
    return "MonomialP";
  }
  unsigned  int HashFunction() const {
    return this->monBody.HashFunction();
  }
  bool HasPositiveOrZeroExponents() const {
    for (int i = 0; i < this->monBody.size; i ++) {
      if (this->monBody[i].IsNegative()) {
        return false;
      }
    }
    return true;
  }
  static bool IsMonEqualToZero() {
    return false;
  }
  void ExponentMeBy(const Rational& theExp);
  //Warning: HashFunction must return the same result
  //for equal monomials represented by different monBodies.
  // Two such different representation may differ by extra entries filled in with zeroes.
  static unsigned int HashFunction(const MonomialP& input) {
    unsigned int result = 0;
    int numCycles = MathRoutines::Minimum(input.monBody.size, SomeRandomPrimesSize);
    for (int i = 0; i < numCycles; i ++) {
      result += input.monBody[i].HashFunction();
    }
    return result;
  }
  std::string ToString(FormatExpressions* PolyFormat = nullptr) const;
  void MakeOne(int ExpectedNumVars = 0) {
    this->monBody.initializeFillInObject(ExpectedNumVars, static_cast<Rational>(0));
  }
  bool operator>(const MonomialP& other) const;
  bool IsDivisibleBy(const MonomialP& other) const;
  int TotalDegreeInt() const {
    int result = - 1;
    if (!this->TotalDegree().IsSmallInteger(&result)) {
      crash << "This is a programming error: total degree of monomial must be "
      << "a small integer to call this function. " << crash;
    }
    return result;
  }
  Rational TotalDegree() const {
    Rational result = 0;
    for (int i = 0; i < this->monBody.size; i ++) {
      result += this->monBody[i];
    }
    return result;
  }
  void MultiplyBy(const MonomialP& other) {
    this->operator*=(other);
  }
  void DivideBy(const MonomialP& other) {
    this->operator/=(other);
  }
  bool IsLinear() const {
    return this->IsConstant() || this->IsLinearNoConstantTerm();
  }
  bool IsLinearNoConstantTerm(int* whichLetter = nullptr) const {
    return this->IsOneLetterFirstDegree(whichLetter);
  }
  bool IsOneLetterFirstDegree(int* whichLetter = nullptr) const {
    Rational whichDegree;
    if (!this->IsOneLetterNthDegree(whichLetter, &whichDegree)) {
      return false;
    }
    return whichDegree == 1;
  }
  bool IsOneLetterNthDegree(int* whichLetter = nullptr, Rational* whichDegree = nullptr) const {
    int tempI1;
    if (whichLetter == nullptr) {
      whichLetter = &tempI1;
    }
    *whichLetter = - 1;
    for (int i = 0; i < this->monBody.size; i ++) {
      if (this->monBody[i] != 0) {
        if (whichDegree != nullptr) {
          *whichDegree = this->monBody[i];
        }
        if ((*whichLetter) == - 1) {
          *whichLetter = i;
        } else {
          return false;
        }
      }
    }
    return (*whichLetter) != - 1;
  }
  bool CheckConsistency() const {
    return true;
  }
  template <class coefficient>
  bool SubstitutioN(const List<Polynomial<coefficient> >& TheSubstitution, Polynomial<coefficient>& output) const;
  void MakeEi(int LetterIndex, int Power = 1, int ExpectedNumVars = 0);
  int GetHighestIndexSuchThatHigherIndexVarsDontParticipate() {
    for (int i = this->monBody.size - 1; i >= 0; i --) {
      if (this->monBody[i] != 0) {
        return i;
      }
    }
    return - 1;
  }
  bool IsGEQpartialOrder(MonomialP& m);
  static bool LeftIsGEQLexicographicLastVariableStrongest(const MonomialP& left, const MonomialP& right) {
    return left.IsGEQLexicographicLastVariableStrongest(right);
  }
  static bool LeftGreaterThanLexicographicLastVariableStrongest(const MonomialP& left, const MonomialP& right) {
    if (left == right) {
      return false;
    }
    return left.IsGEQLexicographicLastVariableStrongest(right);
  }
  static bool LeftGreaterThanLexicographicLastVariableWeakest(const MonomialP& left, const MonomialP& right) {
    if (left == right) {
      return false;
    }
    return left.IsGEQLexicographicLastVariableWeakest(right);
  }
  static bool LeftIsGEQLexicographicLastVariableWeakest(const MonomialP& left, const MonomialP& right) {
    return left.IsGEQLexicographicLastVariableWeakest(right);
  }
  static bool LeftGreaterThanTotalDegThenLexicographicLastVariableStrongest(const MonomialP& left, const MonomialP& right) {
    if (left == right) {
      return false;
    }
    return left.IsGEQTotalDegThenLexicographicLastVariableStrongest(right);
  }
  static bool LeftGreaterThanTotalDegThenLexicographicLastVariableWeakest(const MonomialP& left, const MonomialP& right) {
    if (left == right) {
      return false;
    }
    if (left.TotalDegree() > right.TotalDegree()) {
      return true;
    }
    if (left.TotalDegree() < right.TotalDegree()) {
      return false;
    }
    return left.IsGEQLexicographicLastVariableWeakest(right);
  }
  static bool LeftIsGEQTotalDegThenLexicographicLastVariableStrongest(const MonomialP& left, const MonomialP& right) {
    return left.IsGEQTotalDegThenLexicographicLastVariableStrongest(right);
  }
  bool IsGEQLexicographicLastVariableStrongest(const MonomialP& other) const;
  bool IsGEQLexicographicLastVariableWeakest(const MonomialP& other) const;
  bool IsGEQTotalDegThenLexicographicLastVariableStrongest(const MonomialP& other) const;
  void SetNumVariablesSubDeletedVarsByOne(int newNumVars);
  bool IsConstant() const {
    for (int i = 0; i < this->monBody.size; i ++) {
      if (!this->monBody[i].IsEqualToZero()) {
        return false;
      }
    }
    return true;
  }
  int GetMinNumVars() const {
    return this->monBody.size;
  }
  void Invert() {
    for (int i = 0; i < this->monBody.size; i ++) {
      this->monBody[i].Minus();
    }
  }
  void DrawElement(DrawElementInputOutput& theDrawData);
  void RaiseToPower(const Rational& thePower) {
    for (int i = 0; i < this->monBody.size; i ++) {
      this->monBody[i] *= thePower;
    }
  }
  void operator*=(const MonomialP& other);
  void operator/=(const MonomialP& other);
  bool operator==(const MonomialP& other) const;
  template <class coefficient>
  void operator=(const Vector<coefficient>& other) {
    this->monBody = other;
  }
  void operator=(const MonomialP& other) {
    this->monBody = other.monBody;
  }
  void ReadFromFile(std::fstream& input);
  void WriteToFile(std::fstream& output) const {
    this->monBody.WriteToFile(output);
  }
};

template <typename coefficient>
std::ostream& operator<<  (std::ostream& output, const Matrix<coefficient>& theMat);

template <typename coefficient>
class Matrix {
  //friend std::iostream& operator<< <coefficient>(std::iostream& output, const Matrix<coefficient>& theMat);
  friend std::ostream& operator<<  <coefficient>(std::ostream& output, const Matrix<coefficient>& theMat);
  //friend std::iostream& operator>> <coefficient>(std::iostream& input, Matrix<coefficient>& theMat);
public:
  int NumRows; int ActualNumRows;
  int NumCols; int ActualNumCols;
  coefficient** elements;
  static bool flagComputingDebugInfo;
  bool flagDeallocated;
  void init(int r, int c);
  void ReleaseMemory();
  bool IsPositiveDefinite();
  bool IsNonNegativeAllEntries() {
    for (int i = 0; i < this->NumRows; i ++) {
      for (int j = 0; j < this->NumCols; j ++) {
        if (this->elements[i][j] < 0) {
          return false;
        }
      }
    }
    return true;
  }
  void Resize(int r, int c, bool PReserveValues, const coefficient* TheRingZero = nullptr);
  static std::string GetXMLClassName() {
    std::string result = "Matrix_";
    result.append(coefficient::GetXMLClassName());
    return result;
  }
  Matrix(): NumRows(0), ActualNumRows(0), NumCols(0), ActualNumCols(0), elements(nullptr), flagDeallocated(false) {
  }
  Matrix(const Matrix<coefficient>& other):
    NumRows(0), ActualNumRows(0), NumCols(0), ActualNumCols(0), elements(0), flagDeallocated(false) {
    *this = other;
  }
  ~Matrix() {
    this->ReleaseMemory();
    this->flagDeallocated = true;
  }
  void Transpose() {
    if (this->NumCols == this->NumRows) {
      for (int i = 0; i < this->NumRows; i ++) {
        for (int j = i + 1; j < this->NumCols; j ++) {
          MathRoutines::swap<coefficient>(this->elements[j][i], this->elements[i][j]);
        }
      }
      return;
    }
    Matrix<coefficient> tempMat;
    tempMat.init(this->NumCols, this->NumRows);
    for (int i = 0; i < this->NumRows; i ++) {
      for (int j = 0; j < this->NumCols; j ++) {
        tempMat.elements[j][i] = this->elements[i][j];
      }
    }
    *this = tempMat;
  }
  void AppendMatrixOnTheRight(const Matrix<coefficient>& standsOnTheRight) {
    if (&standsOnTheRight == this) {
      Matrix<coefficient> copyThis = *this;
      this->AppendMatrixOnTheRight(copyThis);
      return;
    }
    if (standsOnTheRight.NumRows < this->NumRows) {
      coefficient theZero;
      theZero = 0;
      Matrix<coefficient> standsOnTheRightNew = standsOnTheRight;
      standsOnTheRightNew.Resize(this->NumRows, standsOnTheRight.NumCols, true, &theZero);
      this->AppendMatrixOnTheRight(standsOnTheRightNew);
      return;
    }
    if (this->NumRows < standsOnTheRight.NumRows) {
      coefficient theZero;
      theZero = 0;
      this->Resize(standsOnTheRight.NumRows, this->NumCols, true, &theZero);
    }
    int oldNumCols = this->NumCols;
    this->Resize(this->NumRows, standsOnTheRight.NumCols + oldNumCols, true);
    for (int i = 0; i < this->NumRows; i ++) {
      for (int j = oldNumCols; j < this->NumCols; j ++) {
        this->elements[i][j] = standsOnTheRight.elements[i][j - oldNumCols];
      }
    }
  }
  void AppendMatrixToTheBottom(const Matrix<coefficient>& standsBelow) {
    if (&standsBelow == this) {
      Matrix<coefficient> copyThis = *this;
      this->AppendMatrixToTheBottom(copyThis);
      return;
    }
    if (standsBelow.NumCols < this->NumCols) {
      coefficient theZero;
      theZero = 0;
      Matrix<coefficient> standsBelowNew = standsBelow;
      standsBelowNew.Resize(standsBelow.NumRows, this->NumCols, true, &theZero);
      this->AppendMatrixOnTheRight(standsBelowNew);
      return;
    }
    if (this->NumCols < standsBelow.NumCols) {
      coefficient theZero;
      theZero = 0;
      this->Resize(this->NumRows, standsBelow.NumCols, true, &theZero);
    }
    //So far, we have guaranteed that this and &standsBelow have the same number of columns and
    // are different objects.
    int oldNumRows = this->NumRows;
    this->Resize(this->NumRows + standsBelow.NumRows, this->NumCols, true);
    for (int i = oldNumRows; i < this->NumRows; i ++) {
      for (int j = 0; j < this->NumCols; j ++) {
        this->elements[i][j] = standsBelow(i - oldNumRows, j);
      }
    }
  }
  bool CheckConsistency() const {
    if (this->flagDeallocated) {
      crash << "Programming error: use after free of Matrix. " << crash;
    }
    return true;
  }
  static void MatrixInBasis(
    const Matrix<coefficient>& input,
    Matrix<coefficient>& output,
    const List<Vector<coefficient> >& basis,
    const Matrix<coefficient>& gramMatrixInverted
  ) {
    int d = basis.size;
    output.init(d, d);
    Vector<Rational> tempV;
    for (int j = 0; j < d; j ++) {
      input.ActOnVectorColumn(basis[j], tempV);
      for (int i = 0; i < d; i ++) {
        output.elements[i][j] = basis[i].ScalarEuclidean(tempV);
      }
    }
    output.MultiplyOnTheLeft(gramMatrixInverted);
  }
  void ComputeDeterminantOverwriteMatrix(
    coefficient& output,
    const coefficient& theRingOne = 1,
    const coefficient& theRingZero = 0
  );
  void ActOnVectorROWSOnTheLeft(List<Vector<coefficient> >& standOnTheRightAsVectorRow) const {
    List<Vector<coefficient> > output;
    this->ActOnVectorROWSOnTheLeft(standOnTheRightAsVectorRow, output);
    standOnTheRightAsVectorRow = output;
  }
  void ActOnVectorROWSOnTheLeft(
    List<Vector<coefficient> >& standOnTheRightAsVectorRow,
    List<Vector<coefficient> >& output
  ) const {
    if (this->NumCols != standOnTheRightAsVectorRow.size) {
      crash << "This is a programming error: attempting to multiply a matrix, standing on the left, with "
      << this->NumCols << " columns, by a matrix, standing on the right, with "
      << standOnTheRightAsVectorRow.size << " rows. "
      << crash;
    }
    output.SetSize(this->NumRows);
    for (int i = 0; i < this->NumRows; i ++) {
      output[i].MakeZero(standOnTheRightAsVectorRow[0].size);
      for (int j = 0; j < this->NumCols; j ++) {
        output[i] += standOnTheRightAsVectorRow[j] * (*this)(i, j);
      }
    }
  }
  void ActMultiplyVectorRowOnTheRight(Vector<coefficient>& standsOnTheLeft) const {
    Vector<coefficient> output;
    this->ActMultiplyVectorRowOnTheRight(standsOnTheLeft, output);
    standsOnTheLeft = output;
  }
  void ActMultiplyVectorRowOnTheRight(const Vector<coefficient>& standsOnTheLeft, Vector<coefficient>& output) const {
    if (&standsOnTheLeft == &output) {
      crash << "In ActMultiplyVectorRowOnTheRight: output not allowed to equal input. " << crash;
    }
    if (this->NumRows != standsOnTheLeft.size) {
      crash << "Number of rows of matrix vector number of elements. " << crash;
    }
    output.MakeZero(this->NumCols);
    coefficient tempElt;
    for (int i = 0; i < this->NumCols; i ++) {
      for (int j = 0; j < this->NumRows; j ++) {
        tempElt = this->elements[j][i];
        tempElt *= standsOnTheLeft[j];
        output[i] += tempElt;
      }
    }
  }
  void GetNSquaredVectorForm(Vector<coefficient>& output) {
    output.SetSize(this->NumRows*this->NumCols);
    for (int i = 0; i < this->NumRows; i ++) {
      for (int j = 0; j < this->NumCols; j ++) {
        output.TheObjects[i * this->NumRows + j] = this->elements[i][j];
      }
    }
  }
  template <class otherType>
  void ActOnVectorColumn(const Vector<otherType>& input, Vector<otherType>& output, const otherType& TheRingZero = 0) const {
    if (&input == &output) {
      Vector<otherType> inputNew = input;
      this->ActOnVectorColumn(inputNew, output, TheRingZero);
      return;
    }
    if (this->NumCols != input.size) {
      crash << "This is a programming error: attempting to multiply a matrix with "
      << this->NumCols << " columns with a vector(column) of "
      << " dimension " << input.size << ". " << crash;
    }
    output.MakeZero(this->NumRows);
    otherType tempElt;
    for (int i = 0; i < this->NumRows; i ++) {
      for (int j = 0; j < this->NumCols; j ++) {
        tempElt = this->elements[i][j];
        tempElt *= input[j];
        output[i] += tempElt;
      }
    }
  }
  unsigned int HashFunction() const {
    return this->HashFunction(*this);
  }
  static unsigned int HashFunction(const Matrix<coefficient>& input) {
    unsigned int result = 0;
    int counter = 0;
    for (int i = 0; i < input.NumRows; i ++, counter ++) {
      for (int j = 0; j < input.NumCols; j ++, counter ++) {
        if (counter < SomeRandomPrimesSize) {
          result += input.elements[i][j].HashFunction() * SomeRandomPrimes[counter];
        } else {
          result += input.elements[i][j].HashFunction() * (i + 1) + j;
        }
      }
    }
    return result;
  }
  template <class otherType>
  void ActOnVectorsColumn(
    const Vectors<otherType>& input, Vectors<otherType>& output, const otherType& TheRingZero = 0
  ) const {
    if (&input == &output) {
      crash << "In ActOnVectorsColumn: input not allowed to equal output. " << crash;
    }
    if (input.size == 0) {
      return;
    }
    if (this->NumCols != input.GetDim()) {
      crash << "This is a programming error: attempting to act by "
      << this->ToString() << "(an " << this->NumRows << " x "
      << this->NumCols << " matrix) on a column vector "
      << input.ToString() << "(dimension " << input.size << ")." << crash;
    }
    output.SetSize(input.size);
    for (int i = 0; i < input.size; i ++) {
      this->ActOnVectorColumn(input[i], output[i], TheRingZero);
    }
  }
  template <class otherType>
  void ActOnVectorColumn(Vector<otherType>& inputOutput, const otherType& TheRingZero = static_cast<otherType>(0)) const {
    Vector<otherType> buffer;
    this->ActOnVectorColumn(inputOutput, buffer, TheRingZero);
    inputOutput = buffer;
  }
  template <class otherType>
  void ActOnVectorsColumn(Vectors<otherType>& inputOutput, const otherType& TheRingZero = static_cast<otherType>(0)) const {
    for (int i = 0; i < inputOutput.size; i ++) {
      this->ActOnVectorColumn(inputOutput[i], TheRingZero);
    }
  }
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringLatex(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringSystemLatex(Matrix<coefficient>* constTerms = 0, FormatExpressions* theFormat = nullptr) const;
  std::string ToStringPlainText(bool jsonFormat = false) const;
  std::string ToStringOneLine(bool jsonFormat = false) const;
  std::string ElementToStringWithBlocks(List<int>& theBlocks);
  void MakeIdMatrix(int theDimension, const coefficient& theRingUnit = 1, const coefficient& theRingZero = 0) {
    this->init(theDimension, theDimension);
    for (int i = 0; i < theDimension; i ++) {
      for (int j = 0; j < theDimension; j ++) {
        if (j != i) {
          this->elements[i][j] = theRingZero;
        } else {
          this->elements[i][j] = theRingUnit;
        }
      }
    }
  }
  void MakeZeroMatrix(int theDimension, const coefficient& theRingZero = 0) {
    this->init(theDimension, theDimension);
    for (int i = 0; i < theDimension; i ++) {
      for (int j = 0; j < theDimension; j ++) {
        this->elements[i][j] = theRingZero;
      }
    }
  }
  void ActOnMonomialAsDifferentialOperator(const MonomialP& input, Polynomial<Rational>& output);
  void SwitchTwoRows(int row1, int row2);
  void SwitchTwoRowsWithCarbonCopy(int row1, int row2, Matrix<coefficient>* theCarbonCopy) {
    this->SwitchTwoRows(row1, row2);
    if (theCarbonCopy != 0) {
      theCarbonCopy->SwitchTwoRows(row1, row2);
    }
  }
  void SetNumVariables(int GoalNumVars) {
    for (int i = 0; i < this->NumRows; i ++) {
      for (int j = 0; j < this->NumCols; j ++) {
        this->elements[i][j].SetNumVariables(GoalNumVars);
      }
    }
  }
  void Substitution(const PolynomialSubstitution<Rational>& theSub);
  coefficient ScalarProduct(const Vector<coefficient>& left, const Vector<coefficient>& right) {
    return this->ScalarProduct(left, right, static_cast<coefficient>(0));
  }
  coefficient ScalarProduct(
    const Vector<coefficient>& left, const Vector<coefficient>& right, const coefficient& theRingZero
  ) {
    if (left.size != this->NumCols || right.size != this->NumRows) {
      crash << "Scalar product using matrix: dimensions of vectors don't match. " << crash;
    }
    coefficient Result, tempElt;
    Result = theRingZero;
    for (int i = 0; i < this->NumRows; i ++) {
      for (int j = 0; j < this->NumCols; j ++) {
        tempElt = left[i];
        tempElt *= this->elements[i][j];
        tempElt *= right[j];
        Result += tempElt;
      }
    }
    return Result;
  }
  inline coefficient& operator()(unsigned i, unsigned j) const {
    return (*this)(static_cast<signed>(i), static_cast<signed>(j));
  }
  coefficient& operator()(int i, int j) const {
    if (i < 0 || i >= this->NumRows || j < 0 || j >= this->NumCols) {
      crash << "This is a programming error: requesting row, column indexed by "
      << i << " and " << j << " but I am a matrix with "
      << this->NumRows << " rows and " << this->NumCols << " colums. " << crash;
    }
    return this->elements[i][j];
  }
  bool IsSquare() const {
    return this->NumCols == this->NumRows;
  }
  bool IsIdMatrix() const {
    if (this->NumRows != this->NumCols || this->NumRows <= 0) {
      return false;
    }
    for (int i = 0; i < this->NumRows; i ++) {
      for (int j = 0; j < this->NumCols; j ++) {
        if (i == j) {
          if (!this->elements[i][j].IsEqualToOne()) {
            return false;
          }
        } else {
          if (!this->elements[i][j].IsEqualToZero()) {
            return false;
          }
        }
      }
    }
    return true;
  }
  bool IsID() const {
    return IsIdMatrix();
  } // for FiniteGroup compat
  void GetVectorFromColumn(int colIndex, Vector<coefficient>& output) const;
  void GetVectorFromRow(int rowIndex, Vector<coefficient>& output) const;
  int FindPivot(int columnIndex, int RowStartIndex);
  bool FindFirstNonZeroElementSearchEntireRow(coefficient& output) {
    for (int i = 0; i < this->NumCols; i ++) {
      for (int j = 0; j < this->NumRows; j ++) {
        if (!this->elements[i][j].IsEqualToZero()) {
          output = this->elements[i][j];
          return true;
        }
      }
    }
    return false;
  }
  void MakeLinearOperatorFromDomainAndRange(Vectors<coefficient>& domain, Vectors<coefficient>& range) {
    Matrix<coefficient> A;
    Matrix<coefficient> B;
    A.AssignVectorsToRows(domain);
    B.AssignVectorsToRows(range);
    A.Invert();
    (*this) = A * B;
    this->Transpose();
  }
  void RowTimesScalar(int rowIndex, const coefficient& scalar);
  void RowTimesScalarWithCarbonCopy(int rowIndex, const coefficient& scalar, Matrix<coefficient>* theCarbonCopy) {
    this->RowTimesScalar(rowIndex, scalar);
    if (theCarbonCopy != 0) {
      theCarbonCopy->RowTimesScalar(rowIndex, scalar);
    }
  }
  void AddTwoRows(int fromRowIndex, int ToRowIndex, int StartColIndex, const coefficient& scalar);
  void AddTwoRowsWithCarbonCopy(
    int fromRowIndex, int ToRowIndex, int StartColIndex, const coefficient& scalar, Matrix<coefficient>* theCarbonCopy
  ) {
    this->AddTwoRows(fromRowIndex, ToRowIndex, StartColIndex, scalar);
    if (theCarbonCopy != 0) {
      theCarbonCopy->AddTwoRows(fromRowIndex, ToRowIndex, StartColIndex, scalar);
    }
  }
  void SubtractRows(int indexRowWeSubtractFrom, int indexSubtracted, int StartColIndex, const coefficient& scalar);
  void SubtractRowsWithCarbonCopy(
    int indexRowWeSubtractFrom,
    int indexSubtracted,
    int StartColIndex,
    const coefficient& scalar,
    Matrix<coefficient>* theCarbonCopy
  ) {
    this->SubtractRows(indexRowWeSubtractFrom, indexSubtracted, StartColIndex, scalar);
    if (theCarbonCopy != 0) {
      theCarbonCopy->SubtractRows(indexRowWeSubtractFrom, indexSubtracted, StartColIndex, scalar);
    }
  }
  void MultiplyOnTheLeft(
    const Matrix<coefficient>& standsOnTheLeft, Matrix<coefficient>& output, const coefficient& theRingZero = 0
  ) const;
  void MultiplyOnTheLeft(const Matrix<coefficient>& standsOnTheLeft, const coefficient& theRingZero = 0);
  void MultiplyOnTheRight(const Matrix<coefficient>& standsOnTheRight) {
    Matrix<coefficient> temp = *this;
    *this = standsOnTheRight;
    this->MultiplyOnTheLeft(temp);
  }
  void NonPivotPointsToEigenVectorMatrixForm(Selection& TheNonPivotPoints, Matrix<coefficient>& output);
  void GetVectorsFromRows(List<Vector<coefficient> >& output) {
    output.SetSize(this->NumRows);
    for (int i = 0; i < this->NumRows; i ++) {
      output[i].SetSize(this->NumCols);
      for (int j = 0; j < this->NumCols; j ++) {
        output[i][j] = this->elements[i][j];
      }
    }
  }
  void NonPivotPointsToEigenVector(
    Selection& TheNonPivotPoints,
    Vector<coefficient>& output,
    const coefficient& theRingUnit = 1,
    const coefficient& theRingZero = 0
  );
  bool Invert();
  Matrix<coefficient> Inverse() {
    Matrix<coefficient> copy = *this;
    if (!copy.Invert()) {
      crash << "Request to invert " << copy.ToString() << " failed. " << crash;
    }
    return copy;
  }
  Matrix<coefficient> operator^(const Matrix<coefficient>& right) const;
  static void ConjugationAction(
    const Matrix<coefficient>& conjugateMe, const Matrix<coefficient>& conjugateBy, Matrix<coefficient>& out
  );
  void MakeZero(const coefficient& theRingZero = 0);
  void MakeID(const Matrix<coefficient>& prototype, const coefficient& theRingZero = 0, const coefficient& theRingOne = 1);
  //if m1 corresponds to a linear operator from V1 to V2 and
  // m2 to a linear operator from W1 to W2, then the result of the below function
  //corresponds to the linear operator from V1+W1 to V2+W2 (direct sum)
  //this means you write the matrix m1 in the upper left corner m2 in the lower right
  // and everything else you fill with zeros
  void AssignDirectSum(Matrix<coefficient>& m1,  Matrix<coefficient>& m2);
  // if S and T are endomorphisms of V and W, build the matrix of SⓧT that acts on
  // VⓧW with basis (v1ⓧw1,v1ⓧw2,...,v2ⓧw1,v2ⓧw2,...vnⓧwn)
  void AssignTensorProduct(const Matrix<coefficient>& left, const Matrix<coefficient>& right);
  void AssignVectorsToRows(const List<Vector<coefficient> >& input) {
    int numCols = - 1;
    if (input.size > 0) {
      numCols = input[0].size;
    }
    this->init(input.size, numCols);
    for (int i = 0; i < input.size; i ++) {
      for (int j = 0; j < numCols; j ++) {
        this->elements[i][j] = input[i][j];
      }
    }
  }
  void AssignVectorsToColumns(const Vectors<coefficient>& input) {
    this->init(input[0].size, input.size);
    for (int i = 0; i < this->NumRows; i ++) {
      for (int j = 0; j < this->NumCols; j ++) {
        (*this)(i, j) = input[j][i];
      }
    }
  }
  void AssignVectorColumn(const Vector<coefficient>& input) {
    this->init(input.size, 1);
    for (int i = 0; i < input.size; i ++) {
      this->elements[i][0] = input[i];
    }
  }
  void AssignVectorToRowKeepOtherRowsIntactNoInit(int rowIndex, const Vector<coefficient>& input) {
    if (input.size != this->NumCols || rowIndex >= this->NumRows || rowIndex < 0) {
      crash << "Error: attempting to assign vector " << input.ToString()
      << " (" << input.size << " coordinates) "
      << " to row with index " << rowIndex << " in a matrix with "
      << this->NumRows << " rows and " << this->NumCols << " columns. " << crash;
    }
    for (int i = 0; i < this->NumCols; i ++) {
      this->elements[rowIndex][i] = input[i];
    }
  }
  void AssignVectorToColumnKeepOtherColsIntactNoInit(int colIndex, const Vector<coefficient>& input) {
    if (input.size != this->NumRows || colIndex >= this->NumCols || colIndex < 0) {
      crash << "In AssignVectorToColumnKeepOtherColsIntactNoInit: bad vector/matrix dimensions. "  << crash;
    }
    for (int i = 0; i < this->NumRows; i ++) {
      this->elements[i][colIndex] = input[i];
    }
  }
  void AssignBlock(Matrix<coefficient>& block, int starti, int startj) {
    if (starti + block.NumRows > this->NumRows || startj + block.NumCols > this->NumCols) {
      crash << "In AssignBlock: bad indices. " << crash;
    }
    for (int i = 0; i < block.NumRows; i ++) {
      for (int j = 0; j < block.NumCols; j ++) {
        this->elements[starti + i][startj + j] = block.elements[i][j];
      }
    }
  }
  void AssignVectorRow(const Vector<coefficient>& input) {
    this->init(1, input.size);
    for (int i = 0; i < input.size; i ++) {
      this->elements[0][i] = input[i];
    }
  }
  void DirectSumWith(const Matrix<coefficient>& m2, const coefficient& theRingZero = 0);
  bool operator!=(const Matrix<coefficient>& other) const {
    return !((*this) == other);
  }
  bool operator==(const Matrix<coefficient>& other) const {
    if (this->NumRows != other.NumRows || this->NumCols != other.NumCols) {
      return false;
    }
    for (int i = 0; i < this->NumRows; i ++) {
      for (int j = 0; j < this->NumCols; j ++) {
        if (!(this->elements[i][j] == other.elements[i][j])) {
          return false;
        }
      }
    }
    return true;
  }
  bool IsEqualToZero() const {
    for (int i = 0; i < this->NumRows; i ++) {
      for (int j = 0; j < this->NumCols; j ++) {
        if (this->elements[i][j] != 0) {
          return false;
        }
      }
    }
    return true;
  }
  bool IsProportionalTo(const Matrix<coefficient>& input, coefficient& outputTimesMeEqualsInput) {
    if (input.NumCols != this->NumCols || input.NumRows != this->NumRows) {
      return false;
    }
    bool found = false;
    for (int i = 0; i < this->NumRows && !found; i ++) {
      for (int j = 0; j < this->NumCols; j ++) {
        if (!this->elements[i][j].IsEqualToZero()) {
          found = true;
          outputTimesMeEqualsInput = input.elements[i][j];
          outputTimesMeEqualsInput /= this->elements[i][j];
          break;
        }
      }
    }
    if (!found) {
      return input.IsEqualToZero();
    }
    Matrix<coefficient> tempMat;
    tempMat = *this;
    tempMat *= outputTimesMeEqualsInput;
    tempMat -= input;
    return tempMat.IsEqualToZero();
  }
  //returns true if the system has a solution, false otherwise
  bool RowEchelonFormToLinearSystemSolution(
    Selection& inputPivotPoints, Matrix<coefficient>& inputRightHandSide, Matrix<coefficient>& outputSolution
  );
  void operator+=(const Matrix<coefficient>& right) {
    if (this->NumRows != right.NumRows || this->NumCols != right.NumCols) {
      crash << "This is a programming error: attempting to add matrix with "
      << this->NumRows << " rows and " << this->NumCols
      << " columns to a matrix with " << right.NumRows << " rows and "
      << right.NumCols << " columns. " << crash;
    }
    for (int i = 0; i < this->NumRows; i ++) {
      for (int j = 0; j < this->NumCols; j ++) {
        this->elements[i][j] += right.elements[i][j];
      }
    }
  }
  LargeIntUnsigned FindPositiveLCMCoefficientDenominators();
  void operator-=(const Matrix<coefficient>& right) {
    if (this->NumRows != right.NumRows || this->NumCols != right.NumCols) {
      crash << "This is a programming error: attempting to subtract from matrix with "
      << this->NumRows << " rows and " << this->NumCols
      << " columns a matrix with " << right.NumRows << " rows and " << right.NumCols << " columns. " << crash;
    }
    for (int i = 0; i < this->NumRows; i ++) {
      for (int j = 0; j < this->NumCols; j ++) {
        (*this)(i, j) -= right(i, j);
      }
    }
  }
  void WriteToFile(std::fstream& output);
  bool ReadFromFile(std::fstream& input);
  void operator/=(const coefficient& input) {
    for (int j = 0; j < this->NumRows; j ++) {
      for (int i = 0; i < this->NumCols; i ++) {
        this->elements[j][i] /= input;
      }
    }
  }
  void operator*=(const coefficient& input) {
    for (int j = 0; j < this->NumRows; j ++) {
      for (int i = 0; i < this->NumCols; i ++) {
        this->elements[j][i] *= input;
      }
    }
  }
  void operator*=(const Matrix<coefficient>& input) {
    this->MultiplyOnTheRight(input);
  }
  void LieBracketWith(Matrix<coefficient>& right) {
    Matrix<coefficient> tempMat;
    this->LieBracket(*this, right, tempMat);
    *this = tempMat;
  }
  static void LieBracket(const Matrix<coefficient>& left, const Matrix<coefficient>& right, Matrix<coefficient>& output) {
    if (left.NumCols != left.NumRows || right.NumCols != right.NumRows || left.NumCols != right.NumCols) {
      crash << "In LieBracket: bad dimensions of matrices. " << crash;
    }
    Matrix<coefficient> tempPlus, tempMinus;
    tempPlus = (right);
    tempPlus.MultiplyOnTheLeft(left);
    tempMinus = (left);
    tempMinus.MultiplyOnTheLeft(right);
    tempPlus -= (tempMinus);
    output = tempPlus;
  }
  //The Gaussian elimination below brings the matrix to a row-echelon form,
  //that is, makes the matrix be something like (example is 4x5):
  //1 0 3 0 0
  //0 1 0 0 0
  //0 0 0 1 0
  //0 0 0 0 0
  //In the Gaussian elimination below, we define non-pivot points to be indices of the columns
  // that do not have a pivot 1 in them.
  // In the above example, the third (index 2) and fifth (index 4) columns do not have a pivot 1 in them.
  void GaussianEliminationByRows(
    Matrix<coefficient>* carbonCopyMat = 0,
    Selection* outputNonPivotColumns = nullptr,
    Selection* outputPivotColumns = nullptr,
    std::stringstream* humanReadableReport = nullptr,
    FormatExpressions* theFormat = nullptr
  );
  void GaussianEliminationByRowsNoRowSwapPivotPointsByRows(
    int firstNonProcessedRow,
    List<int>& outputPivotPointCols,
    Selection* outputNonPivotPoints__WarningSelectionNotInitialized
  );
  void GaussianEliminationEuclideanDomain(
    Matrix<coefficient>* otherMatrix = 0,
    const coefficient& theRingMinusUnit = - 1,
    const coefficient& theRingUnit = 1,
    bool (*comparisonGEQFunction) (const coefficient& left, const coefficient& right) = 0
  );
  static bool Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(
    Matrix<coefficient>& A, Matrix<coefficient>& b, Matrix<coefficient>& output
  );
  coefficient GetDeterminant();
  coefficient GetTrace() const;
  void AssignMatrixIntWithDen(Matrix<LargeInt>& theMat, const LargeIntUnsigned& Den);
  void ScaleToIntegralForMinRationalHeightNoSignChange();
  void GetMatrixIntWithDen(Matrix<LargeInt>& outputMat, LargeIntUnsigned& outputDen);
  void LieBracketWith(const Matrix<coefficient>& right);
  void ApproximateLargestEigenSpace(
    Vectors<Rational>& outputBasis,
    const Rational& DesiredError,
    int SuggestedOrder,
    int numIterations
  );
  void MatrixToRoot(Vector<Rational>& output);
  bool GetEigenspacesProvidedAllAreIntegralWithEigenValueSmallerThanDim(List<Vectors<coefficient> >& output) const;
  void GetZeroEigenSpace(List<Vector<coefficient> >& output) const {
    Matrix<coefficient> tempMat = *this;
    tempMat.GetZeroEigenSpaceModifyMe(output);
  }
  void GetZeroEigenSpaceModifyMe(List<Vector<coefficient> >& output);
  void GetEigenspaceModifyMe(const coefficient &inputEigenValue, List<Vector<coefficient> >& outputEigenspace) {
    for (int i = 0; i < this->NumCols; i ++) {
      this->elements[i][i] -= inputEigenValue;
    }
    this->GetZeroEigenSpaceModifyMe(outputEigenspace);
  }
  static bool SystemLinearInequalitiesHasSolution(
    Matrix<coefficient>& matA,
    Matrix<coefficient>& matb,
    Matrix<coefficient>& outputPoint
  );
  static bool SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(
    Matrix<coefficient>& matA,
    Matrix<coefficient>& matb,
    Vector<coefficient>* outputSolution = 0
  );
  static void ComputePotentialChangeGradient(
    Matrix<coefficient>& matA,
    Selection& BaseVariables,
    int NumTrueVariables,
    int ColumnIndex,
    Rational& outputChangeGradient,
    bool& hasAPotentialLeavingVariable
  );
  static void GetMaxMovementAndLeavingVariableRow(
    Rational& maxMovement,
    int& LeavingVariableRow,
    int EnteringVariable,
    Matrix<coefficient>& tempMatA,
    Vector<coefficient>& inputVectorX,
    Selection& BaseVariables
  );
  int FindPositiveLCMCoefficientDenominatorsTruncated();
  int FindPositiveGCDCoefficientNumeratorsTruncated();
  Matrix<coefficient> operator-(const Matrix<coefficient>& right) const {
    Matrix<coefficient> tempMat =(*this);
    tempMat -= right;
    return tempMat;
  }
  Matrix<coefficient> operator*(const Matrix<coefficient>& right) const;
  Vector<coefficient> operator*(const Vector<coefficient>& v) const;

  void operator=(const Matrix<coefficient>& other);
  template<class otherType>
  void operator=(const Matrix<otherType>& other) {
    this->Resize(other.NumRows, other.NumCols, false);
    for (int i = 0; i < this->NumRows; i ++) {
      for (int j = 0; j < this->NumCols; j ++) {
        (*this)(i, j) = other(i, j);
      }
    }
  }
  // The following are for compatibility with the FiniteGroup class
  void GetCharacteristicPolyStandardRepresentation(Polynomial<coefficient>& out) {
    out.AssignCharPoly(*this);
  }
  bool HasDifferentConjugacyInvariantsFrom(Matrix<coefficient>& other) {
    Polynomial<coefficient> p, q;
    this->GetCharacteristicPolyStandardRepresentation(p);
    other.GetCharacteristicPolyStandardRepresentation(q);
    return !(p == q);
  }
  bool operator>(const Matrix<coefficient>& right) const {
    if (this->NumRows != right.NumRows || this->NumCols != right.NumCols) {
      crash << "An attempt was just made to compare two matrices of different dimensions; "
      << "most likely something is wrong in some FiniteGroup, see the frames above. "
      << crash;
    }
    for (int i = 0; i < this->NumRows; i ++) {
      for (int j = 0; j < this->NumCols; j ++) {
        if (!(this->elements[i][j] > right.elements[i][j])) {
          return false;
        }
      }
    }
    return true;
  }
};

template <typename Element>
void Matrix<Element>::Resize(int r, int c, bool PReserveValues, const Element* const TheRingZero) {
  if (r < 0) {
    r = 0;
  }
  if (c < 0) {
    c = 0;
  }
  if (r == this->NumRows && c == this->NumCols) {
    return;
  }
  if (r == 0 || c == 0) {
    this->NumRows = r;
    this->NumCols = c;
    return;
  }
  Element** newElements = nullptr;
  int newActualNumCols = MathRoutines::Maximum(this->ActualNumCols, c);
  int newActualNumRows = MathRoutines::Maximum(this->ActualNumRows, r);
  if (r > this->ActualNumRows || c > this->ActualNumCols) {
    newElements  = new Element * [newActualNumRows];
#ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter += newActualNumRows;
  ParallelComputing::CheckPointerCounters();
#endif
    for (int i = 0; i < newActualNumRows; i ++) {
      newElements[i] = new Element[newActualNumCols];
#ifdef AllocationLimitsSafeguard
  ParallelComputing::GlobalPointerCounter += newActualNumCols;
  ParallelComputing::CheckPointerCounters();
#endif
    }
  }
  int firstInvalidRow = MathRoutines::Minimum(this->NumRows, r);
  int firstInvalidCol = MathRoutines::Minimum(this->NumCols, c);
  if (PReserveValues && newElements != nullptr) {
    for (int j = 0; j < firstInvalidRow; j ++) {
      for (int i = 0; i < firstInvalidCol; i ++) {
        newElements[j][i] = this->elements[j][i];
      }
    }
  }
  if (TheRingZero != nullptr) {
    if (!PReserveValues) {
      firstInvalidRow = 0;
      firstInvalidCol = 0;
    }
    for (int i = 0; i <r; i ++) {
      int colStart = (i < firstInvalidRow) ? firstInvalidCol : 0;
      for (int j = colStart; j < c; j ++) {
        newElements[i][j] = *TheRingZero;
      }
    }
  }
  if (newElements != nullptr) {
    this->ReleaseMemory();
    this->elements = newElements;
    this->ActualNumCols = newActualNumCols;
    this->ActualNumRows = newActualNumRows;
  }
  this->NumCols = c;
  this->NumRows = r;
}

template <typename coefficient>
void Vectors<coefficient>::GetMatrixRootsToRows(Matrix<Rational>& output) const {
  int tempNumCols = 0;
  if (this->size != 0) {
    tempNumCols = static_cast<int>(this->TheObjects[0].size);
  }
  output.init(static_cast<int>(this->size), tempNumCols);
  for (int i = 0; i < this->size; i ++) {
    for (int j = 0; j < tempNumCols; j ++) {
      output.elements[i][j] = this->TheObjects[i][j];
    }
  }
}

template <typename coefficient>
void Vectors<coefficient>::GetOrthogonalComplement(Vectors<coefficient>& output, Matrix<Rational>* theBilinearForm) {
  if (this->size == 0) {
    if (theBilinearForm != nullptr) {
      output.MakeEiBasis(theBilinearForm->NumRows);
      return;
    }
    crash << "Finding orthogonal complement of zero vectors without specifying a bilinear form is "
    << " forbidden: I can't determine the dimension of the ambient vector space. " << crash;
  }
  Matrix<coefficient> theMatrix;
  theMatrix.AssignVectorsToRows(*this);
  if (theBilinearForm != nullptr) {
    theMatrix *= *theBilinearForm;
  }
  theMatrix.GetZeroEigenSpaceModifyMe(output);
}

template <typename coefficient>
bool Vectors<coefficient>::ComputeNormal(Vector<coefficient>& output, int inputDim) {
  if (this->size == 0) {
    if (inputDim == 1) {
      output.MakeEi(1, 0);
      return true;
    }
    return false;
  }
  int theDimension = this->TheObjects[0].size;
  Matrix<coefficient> tempMatrix;
  Selection NonPivotPoints;
  NonPivotPoints.init(theDimension);
  output.SetSize(theDimension);
  this->GaussianEliminationForNormalComputation(tempMatrix, NonPivotPoints, theDimension);
  if (NonPivotPoints.CardinalitySelection != 1) {
    return false;
  }
  tempMatrix.NonPivotPointsToEigenVector(NonPivotPoints, output);
  return true;
}

template <typename Element>
void Matrix<Element>::operator=(const Matrix<Element>& m) {
  if (this == &m) {
    return;
  }
  this->Resize(m.NumRows, m.NumCols, false);
  for (int i = 0; i < this->NumRows; i ++) {
    for (int j = 0; j < this->NumCols; j ++) {
      this->elements[i][j] = m.elements[i][j];
    }
  }
}

template <typename Element>
void Matrix<Element>::ReleaseMemory() {
  for (int i = 0; i < this->ActualNumRows; i ++) {
    delete [] this->elements[i];
  }
  delete [] this->elements;
#ifdef AllocationLimitsSafeguard
ParallelComputing::GlobalPointerCounter -= this->ActualNumRows * this->ActualNumCols + this->ActualNumRows;
  ParallelComputing::CheckPointerCounters();
#endif
  this->elements = nullptr;
  this->NumCols = 0;
  this->NumRows = 0;
  this->ActualNumRows = 0;
  this->ActualNumCols = 0;
}

template <typename Element>
void Matrix<Element>::WriteToFile(std::fstream& output) {
  output << XML::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  output << "r: " << this->NumRows << " c: " << this->NumCols << "\n";
  for (int i = 0; i < this->NumRows; i ++) {
    for (int j = 0; j < this->NumCols; j ++) {
      this->elements[i][j].WriteToFile(output);
      output << " ";
    }
    output << "\n";
  }
  output << XML::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName());
}

template <typename Element>
bool Matrix<Element>::ReadFromFile(std::fstream& input) {
  int r, c;
  std::string tempS;
  int NumReadWords;
  XML::ReadThroughFirstOpenTag(input, NumReadWords, this->GetXMLClassName());
  if (NumReadWords != 0) {
    crash << "While reading matrix, encountered fatal error. " << crash;
  }
  input >> tempS >> r >> tempS >> c;
  if (tempS != "c:") {
    crash << "Bad matrix file. " << crash;
    return false;
  }
  this->init(r, c);
  for (int i = 0; i < this->NumRows; i ++) {
    for (int j = 0; j < this->NumCols; j ++) {
      this->elements[i][j].ReadFromFile(input);
    }
  }
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, NumReadWords, this->GetXMLClassName());
  if (NumReadWords != 0) {
    crash << "Bad matrix file: not enough words read. " << crash;
  }
  return true;
}

template <typename coefficient>
bool Matrix<coefficient>::Invert() {
  if (this->NumCols != this->NumRows) {
    crash << "This is a programming error: requesting to invert a non-square matrix of "
    << this->NumRows << " rows and "
    << this->NumCols << " columns. " << crash;
  }
  MacroRegisterFunctionWithName("Matrix::Invert");
//  if (this->flagComputingDebugInfo)
//    this->ComputeDebugString();
  Matrix theInverse;
  theInverse.MakeIdMatrix(this->NumRows);
  Selection NonPivotCols;
  this->GaussianEliminationByRows(&theInverse, &NonPivotCols, 0);
  if (NonPivotCols.CardinalitySelection != 0) {
    return false;
  }
  *this = theInverse;
  return true;
}

template <typename coefficient>
Matrix<coefficient> Matrix<coefficient>::operator^(const Matrix<coefficient>& right) const {
  Matrix<coefficient> conjugateInverse = right;
  conjugateInverse.Invert();
  Matrix<coefficient> tmp;
  this->MultiplyOnTheLeft(conjugateInverse,tmp);
  Matrix<coefficient> out;
  right.MultiplyOnTheLeft(tmp, out);
  return out;
}

template <typename coefficient>
void Matrix<coefficient>::ConjugationAction(
  const Matrix<coefficient>& conjugateWith,
  const Matrix<coefficient>& conjugateOn,
  Matrix<coefficient>& out
) {
  out = conjugateOn ^ conjugateWith;
}

template <typename coefficient>
void Matrix<coefficient>::MultiplyOnTheLeft(const Matrix<coefficient>& standsOnTheLeft, const coefficient& theRingZero) {
  Matrix<coefficient> tempMat;
  this->MultiplyOnTheLeft(standsOnTheLeft, tempMat, theRingZero);
  this->operator=(tempMat);
}

template <typename coefficient>
void Matrix<coefficient>::MultiplyOnTheLeft(
  const Matrix<coefficient>& standsOnTheLeft,
  Matrix<coefficient>& output,
  const coefficient& theRingZero
) const {
  if (&output == this || &output == &standsOnTheLeft) {
    Matrix<coefficient> thisCopy = *this;
    Matrix<coefficient> standsOnTheLeftCopy = standsOnTheLeft;
    thisCopy.MultiplyOnTheLeft(standsOnTheLeftCopy, output, theRingZero);
    return;
  }
  if (this->NumRows != standsOnTheLeft.NumCols) {
    crash << "This is a programming error: attempting to multiply a matrix with " << standsOnTheLeft.NumCols
    << " columns by a matrix with " << this->NumRows << "rows. " << crash;
  }
  coefficient tempEl;
  output.init(standsOnTheLeft.NumRows, this->NumCols);
  for (int i = 0; i < standsOnTheLeft.NumRows; i ++) {
    for (int j = 0; j < this->NumCols; j ++) {
      output.elements[i][j] = theRingZero;
      for (int k = 0; k < this->NumRows; k ++) {
        tempEl = standsOnTheLeft(i, k);
        tempEl *= this->elements[k][j];
        output.elements[i][j] += (tempEl);
      }
    }
  }
}

template <typename Element>
std::string Matrix<Element>::ElementToStringWithBlocks(List<int>& theBlocks) {
  std::stringstream out;
  std::string tempS;
  out << "\\left(\\begin{array}{";
  int offset = 0;
  int blockIndex = 0;
  for (int j = 0; j < this->NumCols; j ++) {
    out << "c";
    offset ++;
    if (offset == theBlocks.TheObjects[blockIndex]) {
      offset = 0;
      blockIndex ++;
      if (j != this->NumCols - 1) {
        out << "|";
      }
    }
  }
  out << "}";
  offset = 0;
  blockIndex = 0;
  for (int i = 0; i < this->NumRows; i ++) {
    for (int j = 0; j < this->NumCols; j ++) {
      tempS = this->elements[i][j].ToString();
      out << tempS;
      if (j != this->NumCols - 1) {
        out << " & ";
      }
    }
    out << "\\\\\n";
    offset ++;
    if (offset == theBlocks.TheObjects[blockIndex]) {
      offset = 0;
      blockIndex ++;
      if (i != this->NumCols - 1) {
        out << "\\hline";
      }
    }
  }
  out << "\\end{array}\\right)";
  return out.str();
}

template<typename Element>
void Matrix<Element>::AssignDirectSum(Matrix<Element>& m1, Matrix<Element>& m2) {
  if (this == &m1 || this == &m2) {
    crash << "In AssignDirectSum: output not allowed to equal any of the inputs. " << crash;
  }
  this->Resize(m1.NumRows + m2.NumRows, m1.NumCols + m2.NumCols, false);
  this->MakeZero();
  for (int i = 0; i < m1.NumRows; i ++) {
    for (int j = 0; j < m1.NumCols; j ++) {
      this->elements[i][j] = m1.elements[i][j];
    }
  }
  for (int i = 0; i < m2.NumRows; i ++) {
    for (int j = 0; j < m2.NumCols; j ++) {
      this->elements[i + m1.NumRows][j + m1.NumCols] = m2.elements[i][j];
    }
  }
}

template<typename Element>
void Matrix<Element>::AssignTensorProduct(const Matrix<Element>& left, const Matrix<Element>& right) {
  //handle lazy programmers:
  if (this == &left || this == &right) {
    Matrix<Element> leftCopy = left;
    Matrix<Element> rightCopy = right;
    this->AssignTensorProduct(leftCopy, rightCopy);
    return;
  }
  this->Resize(left.NumRows*right.NumRows, left.NumCols*right.NumCols, false);
  int sr = right.NumRows;
  int sc = right.NumCols;
  //The basis of the tensor product vector space  MUST be in the SAME order as the one used by MatrixTensor::AssignTensorProduct.
  //indexing. Let the first vector space have basis v_1, ..., v_k, the second: w_1, ..., w_m.
  //Then the basis of the tensor product is given in the order: v_1\otimes w_1, ..., v_1\otimes w_m,
  //..., v_k\otimes w_1, ..., v_k\otimes w_m
  for (int iv = 0; iv < left.NumRows; iv ++) {
    for (int iw = 0; iw < right.NumRows; iw ++) {
      for (int jv = 0; jv < left.NumCols; jv ++) {
        for (int jw = 0; jw < right.NumCols; jw ++) {
          this->elements[iv * sr + iw][jv * sc + jw] = left.elements[iv][jv] * right.elements[iw][jw];
        }
      }
    }
  }
}

template<typename Element>
void Matrix<Element>::DirectSumWith(const Matrix<Element>& m2, const Element& theRingZero) {
  int oldNumRows = this->NumRows;
  int oldNumCols = this->NumCols;
  this->Resize(this->NumRows + m2.NumRows, this->NumCols + m2.NumCols, true);
  for (int i = 0; i < m2.NumRows; i ++) {
    for (int j = 0; j < m2.NumCols; j ++) {
      this->elements[i + oldNumRows][j + oldNumCols] = m2.elements[i][j];
    }
    for (int j = 0; j < oldNumCols; j ++) {
      this->elements[i + oldNumRows][j] = theRingZero;
    }
  }
  for (int j = 0; j < oldNumRows; j ++) {
    for (int i = oldNumCols; i < this->NumCols; i ++) {
      this->elements[j][i] = theRingZero;
    }
  }
}

template <typename Element>
int Matrix<Element>::FindPivot(int columnIndex, int RowStartIndex) {
  for (int i = RowStartIndex; i < this->NumRows; i ++) {
    if (!this->elements[i][columnIndex].IsEqualToZero()) {
      return i;
    }
  }
  return - 1;
}

template <typename Element>
void Matrix<Element>::SubtractRows(
  int indexRowWeSubtractFrom, int indexSubtracted, int StartColIndex, const Element& scalar
) {
  Element tempElement;
  for (int i = StartColIndex; i < this->NumCols; i ++) {
    tempElement = this->elements[indexSubtracted][i];
    tempElement *= scalar;
    this->elements[indexRowWeSubtractFrom][i] -= tempElement;
  }
}

template <typename Element>
void Matrix<Element>::RowTimesScalar(int rowIndex, const Element& scalar) {
  for (int i = 0; i < this->NumCols; i ++) {
    this->elements[rowIndex][i] *= scalar;
  }
}

template <typename Element>
void Matrix<Element>::SwitchTwoRows(int row1, int row2) {
  if (row1 == row2) {
    return;
  }
  Element* tmp = this->elements[row1];
  this->elements[row1] = this->elements[row2];
  this->elements[row2] = tmp;
}

template <typename Element>
bool Matrix<Element>::Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(
  Matrix<Element>& A, Matrix<Element>& b, Matrix<Element>& output
) {
  if (A.NumRows != b.NumRows) {
    crash << "Number of matrix rows does not match number of vector entries. " << crash;
  }
  Selection thePivotPoints;
  A.GaussianEliminationByRows(&b, 0, &thePivotPoints);
  return A.RowEchelonFormToLinearSystemSolution(thePivotPoints, b, output);
}

template <typename Element>
bool Matrix<Element>::RowEchelonFormToLinearSystemSolution(
  Selection& inputPivotPoints, Matrix<Element>& inputRightHandSide, Matrix<Element>& outputSolution
) {
  if (
    inputPivotPoints.MaxSize != this->NumCols ||
    inputRightHandSide.NumCols != 1 ||
    inputRightHandSide.NumRows != this->NumRows
  ) {
    crash << "In RowEchelonFormToLinearSystemSolution: bad matrix dimensions. " << crash;
  }
  outputSolution.init(this->NumCols, 1);
  int NumPivots = 0;
  for (int i = 0; i < this->NumCols; i ++) {
    if (inputPivotPoints.selected[i]) {
      outputSolution(i, 0) = inputRightHandSide(NumPivots, 0);
      NumPivots ++;
    } else {
      outputSolution(i, 0).MakeZero();
    }
  }
  for (int i = NumPivots; i < this->NumRows; i ++) {
    if (!inputRightHandSide.elements[i][0].IsEqualToZero()) {
      return false;
    }
  }
  return true;
}

template <typename Element>
void Matrix<Element>::GaussianEliminationByRowsNoRowSwapPivotPointsByRows(
  int firstNonProcessedRow,
  List<int>& outputPivotPointCols,
  Selection* outputNonPivotPoints__WarningSelectionNotInitialized
) {
  outputPivotPointCols.SetSize(this->NumRows);
  Element tempElement;
  for (int i = firstNonProcessedRow; i < this->numRows; i ++) {
    int currentPivotCol = - 1;
    for (int j = 0; j < this->NumCols; j ++) {
      if (!this->elements[i][j].IsEqualToZero()) {
        currentPivotCol = j;
        break;
      }
    }
    outputPivotPointCols.TheObjects[i] = currentPivotCol;
    if (currentPivotCol != - 1) {
      tempElement = this->elements[i][currentPivotCol];
      tempElement.Invert();
      this->RowTimesScalar(i, tempElement);
      for (int j = 0; j < this->NumRows; j ++) {
        if (i != j) {
          if (!this->elements[j][i].IsEqualToZero()) {
            tempElement.Assign(this->elements[j][i]);
            this->SubtractRows(j, i, 0, tempElement);
          }
        }
      }
    }
  }
  if (outputNonPivotPoints__WarningSelectionNotInitialized != nullptr) {
    for (int i = 0; i < this->NumCols; i ++) {
      outputNonPivotPoints__WarningSelectionNotInitialized->selected[i] = true;
    }
    for (int i = 0; i < this->NumRows; i ++) {
      if (outputPivotPointCols.TheObjects[i] != - 1) {
        outputNonPivotPoints__WarningSelectionNotInitialized->selected[outputPivotPointCols.TheObjects[i]] = false;
      }
    }
    outputNonPivotPoints__WarningSelectionNotInitialized->ComputeIndicesFromSelection();
  }
}

template <typename Element>
void Matrix<Element>::MakeZero(const Element& theRingZero) {
  for (int i = 0; i < this->NumRows; i ++) {
    for (int j = 0; j < this->NumCols; j ++) {
      this->elements[i][j] = theRingZero;
    }
  }
}

template <typename Element>
void Matrix<Element>::MakeID(const Matrix<Element>& prototype, const Element& theRingZero, const Element& theRingOne) {
  this->init(prototype.NumRows, prototype.NumCols);
  for (int i = 0; i < this->NumRows; i ++) {
    for (int j = 0; j < this->NumCols; j ++) {
      this->elements[i][j] = i == j ? theRingOne : theRingZero;
    }
  }
}

template <typename Element>
void Matrix<Element>::init(int r, int c) {
  this->Resize(r, c, false);
}

class FormatExpressions {
public:
  //alphabetBases must contain at least two elements
  std::string chevalleyGgeneratorLetter;
  std::string chevalleyHgeneratorLetter;
  std::string fundamentalWeightLetter;
  std::string polyDefaultLetter;
  std::string WeylAlgebraDefaultLetter;
  std::string CustomPlusSign;
  std::string CustomCoeffMonSeparator;
  std::string FDrepLetter;
  std::string simpleRootLetter;
  List<std::string> polyAlphabeT;
  List<std::string> weylAlgebraLetters;
  List<std::string> vectorSpaceEiBasisNames;
  Rational AmbientCartanSymmetricInverseScale;
  int ExtraLinesCounterLatex;
  int NumAmpersandsPerNewLineForLaTeX;
  int MaxRecursionDepthPerExpression;
  int MaxLineLength;
  int MaxMatrixLineLength;
  int MaxMatrixDisplayedRows;
  int MaxLinesPerPage;
  int MatrixColumnVerticalLineIndex;
  //  bool flagMatrixStarted;
  //  bool flagMatrixRowStarted;
  bool flagPassCustomCoeffMonSeparatorToCoeffs;
  bool flagMakingExpressionTableWithLatex;
  bool flagUseLatex;
  bool flagUsePmatrix;
  bool flagUsePNG;
  bool flagUseHTML;
  bool flagUseHtmlAndStoreToHD;
  bool flagUseMathSpanPureVsMouseHover;
  bool flagDynkinTypeDontUsePlusAndExponent;
  bool flagUseCalculatorFormatForUEOrdered;
  bool flagQuasiDiffOpCombineWeylPart;
  bool flagExpressionIsFinal;
  bool flagIncludeMutableInformation;
  bool flagExpressionNewLineAllowed;
  bool flagIncludeExtraHtmlDescriptionsInPlots;
  bool flagIncludeLieAlgebraTypes;
  bool flagUseReflectionNotation;
  bool flagCandidateSubalgebraShortReportOnly;
  bool flagSupressDynkinIndexOne;
  bool flagFormatWeightAsVectorSpaceIndex;
  bool flagFormatMatrixAsLinearSystem;
  bool flagUseFrac;
  bool flagSuppressOneIn1overXtimesY;
  bool flagSuppressLongMatrices;
  bool flagLatexDetailsInHtml;
  bool flagUseQuotes;
  char AmbientWeylLetter;
  List<MonomialP>::OrderLeftGreaterThanRight thePolyMonOrder;
  template <typename templateMonomial>
  typename List<templateMonomial>::OrderLeftGreaterThanRight GetMonOrder();
  std::string GetPolyLetter(int index) const;
  FormatExpressions();
};

class MonomialWeylAlgebra {
  public:
  MonomialP polynomialPart;
  MonomialP differentialPart;
  friend std::ostream& operator << (std::ostream& output, const MonomialWeylAlgebra& theMon) {
    output << theMon.ToString();
    return output;
  }
  static bool IsMonEqualToZero() {
    return false;
  }
  bool IsConstant() const {
    return this->polynomialPart.IsConstant() && this->differentialPart.IsConstant();
  }
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
  static unsigned int HashFunction(const MonomialWeylAlgebra& input) {
    return
    input.polynomialPart.HashFunction() +
    input.differentialPart.HashFunction() * SomeRandomPrimes[0];
  }
  unsigned int HashFunction() const {
    return this->HashFunction(*this);
  }
  int GetMinNumVars() const {
    return MathRoutines::Maximum(this->polynomialPart.GetMinNumVars(), this->differentialPart.GetMinNumVars());
  }
  bool operator==(const MonomialWeylAlgebra& other) const {
    return this->polynomialPart == other.polynomialPart &&
    this->differentialPart == other.differentialPart;
  }
  bool operator>(const MonomialWeylAlgebra& other) const {
    if (this->differentialPart > other.differentialPart) {
      return true;
    }
    if (other.differentialPart > this->differentialPart) {
      return false;
    }
    if (this->polynomialPart > other.polynomialPart) {
      return true;
    }
    if (other.polynomialPart > this->polynomialPart) {
      return false;
    }
    return false;
  }
  bool HasNonSmallPositiveIntegerDerivation() const {
    for (int i = 0; i < this->differentialPart.GetMinNumVars(); i ++) {
      if (!this->differentialPart(i).IsSmallInteger()) {
        return true;
      }
    }
    return false;
  }
  void MakeOne(int ExpectedNumVars = 0) {
    this->polynomialPart.MakeOne(ExpectedNumVars);
    this->differentialPart.MakeOne(ExpectedNumVars);
  }
};

template <class coefficient>
std::iostream& operator <<(std::iostream& output, const Polynomial<coefficient>& input) {
  output << input.ToString();
  return output;
}

template <class templateMonomial, class coefficient>
std::ostream& operator<<(std::ostream& output, const MonomialCollection<templateMonomial, coefficient>& theCollection);

template <class templateMonomial, class coefficient>
class MonomialCollection {
private:
  void AddOnTop(const MonomialP& tempP);//<-to guard the now unsafe base class method
  void Clear();//<-to guard the now unsafe base class method
  friend std::ostream& operator<< <templateMonomial, coefficient>(
    std::ostream& output, const MonomialCollection<templateMonomial, coefficient>& theCollection
  );
public:
  HashedList<templateMonomial> theMonomials;
  List<coefficient> theCoeffs;
  bool flagDeallocated;
  MonomialCollection() {
    this->flagDeallocated = false;
  }
  MonomialCollection(const MonomialCollection& other) {
    this->flagDeallocated = false;
    this->operator=(other);
  }
  void operator=(const MonomialCollection& other) {
    this->theMonomials = other.theMonomials;
    this->theCoeffs = other.theCoeffs;
  }
  ~MonomialCollection() {
    this->flagDeallocated = true;
  }
  bool NeedsParenthesisForMultiplication() const {
    return this->size() > 1;
  }
  static std::string GetXMLClassName() {
    std::string result = "MonomialCollection_";
    result.append(templateMonomial::GetXMLClassName());
    return result;
  }
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
  int size() const {
    return this->theMonomials.size;
  }
  //BIG FAT WARNING
  //the hash function of Monomial collection must return the same value for
  // monomial collections whose monomials are permuted!
  static unsigned int HashFunction(const MonomialCollection<templateMonomial, coefficient>& input) {
    unsigned int result = 0;
    for (int i = 0; i < input.size(); i ++) {
      result += input.theCoeffs[i].HashFunction() * input[i].HashFunction();
    }
    return result;
  }
  void QuickSortAscending(typename List<templateMonomial>::OrderLeftGreaterThanRight theOrder = nullptr) {
    List<templateMonomial> theSortedMons = this->theMonomials;
    theSortedMons.QuickSortAscending(theOrder, &this->theCoeffs);
    this->theMonomials = theSortedMons;
  }
  void QuickSortDescending(typename List<templateMonomial>::OrderLeftGreaterThanRight theOrder = nullptr) {
    List<templateMonomial> theSortedMons = this->theMonomials;
    theSortedMons.QuickSortDescending(theOrder, &this->theCoeffs);
    this->theMonomials = theSortedMons;
  }
  void GetVectorMonsAscending(Vector<coefficient>& result);
  void GetVectorMonsDescending(Vector<coefficient>& result);
  void PopMonomial(int index) {
    this->theMonomials.RemoveIndexSwapWithLast(index);
    this->theCoeffs.RemoveIndexSwapWithLast(index);
  }
  void AddOtherTimesConst(MonomialCollection<templateMonomial, coefficient>& other, const coefficient& theConst);
  void PopMonomial(int index, templateMonomial& outputMon, coefficient& outputCoeff) {
    outputMon = (*this)[index];
    outputCoeff = this->theCoeffs[index];
    this->theMonomials.RemoveIndexSwapWithLast(index);
    this->theCoeffs.RemoveIndexSwapWithLast(index);
  }
  unsigned int HashFunction() const {
    return this->HashFunction(*this);
  }
  coefficient GetCoefficientsSum() const {
    coefficient result = 0;
    for (int i = 0; i < this->theCoeffs.size; i ++) {
      result += this->theCoeffs[i];
    }
    return result;
  }
  void AddMonomial(const templateMonomial& inputMon, const coefficient& inputCoeff) {
    this->CleanupMonIndex(this->AddMonomialNoCoeffCleanUpReturnsCoeffIndex(inputMon, inputCoeff));
  }
  void GetMinMonomial(templateMonomial& outputMon, coefficient& outputCF) const {
    if (this->IsEqualToZero()) {
      crash << "This is a programming error: calling GetMinMon on a zero monomial collection is forbidden. " << crash;
    }
    outputMon = (*this)[0];
    outputCF = this->theCoeffs[0];
    for (int i = 1; i < this->size(); i ++) {
      if (outputMon > (*this)[i]) {
        outputMon = (*this)[i];
        outputCF = this->theCoeffs[i];
      }
    }
  }
  void GetMaxMonomial(templateMonomial& outputMon, coefficient& outputCF) const {
    if (this->IsEqualToZero()) {
      crash << "This is a programming error: calling GetMinMon on a zero monomial collection is forbidden. " << crash;
    }
    outputMon = (*this)[0];
    outputCF = this->theCoeffs[0];
    for (int i = 1; i < this->size(); i ++) {
      if ((*this)[i] > outputMon) {
        outputMon = (*this)[i];
        outputCF = this->theCoeffs[i];
      }
    }
  }
  coefficient GetLeadingCoefficient() const {
    if (this->IsEqualToZero()) {
      return 0;
    }
    templateMonomial tempM;
    coefficient result;
    this->GetMaxMonomial(tempM, result);
    return result;
  }
  bool CleanupMonIndex(int theIndex) {
    if (theIndex != - 1) {
      if (this->theCoeffs[theIndex] == 0) {
        if (this->flagDeallocated) {
          crash << "Use after free or race condition on monomial collection. " << crash;
        }
        bool oldFlagDeallocated = this->flagDeallocated;
        this->flagDeallocated = true;
        this->theMonomials.RemoveIndexSwapWithLast(theIndex);
        this->theCoeffs.RemoveIndexSwapWithLast(theIndex);
        this->flagDeallocated = oldFlagDeallocated;
        return true;
      }
    }
    return false;
  }
  int AddMonomialNoCoeffCleanUpReturnsCoeffIndex(const templateMonomial& inputMon, const coefficient& inputCoeff);
  template <class MonomialCollectionTemplate>
  static void GaussianEliminationByRows(
    List<MonomialCollectionTemplate>& theList,
    bool *IHaveMadeARowSwitch = nullptr,
    HashedList<templateMonomial>* seedMonomials = nullptr,
    Matrix<coefficient>* carbonCopyMatrix = nullptr,
    List<MonomialCollectionTemplate>* carbonCopyList = nullptr
  );
  template <class MonomialCollectionTemplate>
  static void IntersectVectorSpaces(
    const List<MonomialCollectionTemplate>& vectorSpace1,
    const List<MonomialCollectionTemplate>& vectorSpace2,
    List<MonomialCollectionTemplate>& outputIntersection, HashedList<templateMonomial>* seedMonomials = nullptr
  );
  template <class MonomialCollectionTemplate>
  static int GetRankIntersectionVectorSpaces(
    List<MonomialCollectionTemplate>& vectorSpace1,
    List<MonomialCollectionTemplate>& vectorSpace2,
    HashedList<templateMonomial>* seedMonomials = nullptr
  ) {
    List<MonomialCollectionTemplate> listCopy = vectorSpace1;
    listCopy.AddListOnTop(vectorSpace2);
    return vectorSpace1.size + vectorSpace2.size -
    MonomialCollection<templateMonomial, coefficient>::GetRankOfSpanOfElements(listCopy, seedMonomials);
  }
  template <class MonomialCollectionTemplate>
  static bool VectorSpacesIntersectionIsNonTrivial(
    List<MonomialCollectionTemplate>& vectorSpace1,
    List<MonomialCollectionTemplate>& vectorSpace2,
    HashedList<templateMonomial>* seedMonomials = nullptr
  ) {
    return 0 != MonomialCollection<templateMonomial, coefficient>::GetRankIntersectionVectorSpaces(
      vectorSpace1, vectorSpace2, seedMonomials
    );
  }
  template <class MonomialCollectionTemplate>
  static bool LinSpanContains(
    const List<MonomialCollectionTemplate>& theList,
    const MonomialCollectionTemplate& input,
    HashedList<templateMonomial>* seedMonomials = nullptr
  ) {
    List<MonomialCollectionTemplate> listCopy = theList;
    MonomialCollection<templateMonomial, coefficient>::GaussianEliminationByRowsDeleteZeroRows(listCopy, 0, seedMonomials);
    int startSpanSize = listCopy.size;
    listCopy.AddOnTop(input);
    MonomialCollection<templateMonomial, coefficient>::GaussianEliminationByRowsDeleteZeroRows(listCopy, 0, seedMonomials);
    return listCopy.size == startSpanSize;
  }
  template <class MonomialCollectionTemplate>
  static bool LinSpanContainsGetFirstLinearCombination(
    const List<MonomialCollectionTemplate>& theList,
    const MonomialCollectionTemplate& input,
    Vector<coefficient>& outputFirstLinearCombination,
    HashedList<templateMonomial>* seedMonomials = nullptr
  );
  bool HasRationalCoeffs(MonomialCollection<templateMonomial, Rational>* outputConversionToRationals = nullptr) {
    Rational tempRat;
    Rational* theCF = nullptr;
    if (outputConversionToRationals != nullptr) {
      theCF = &tempRat;
      outputConversionToRationals->MakeZero();
    }
    for (int i = 0; i < this->size(); i ++) {
      if (!this->theCoeffs[i].IsRational(theCF)) {
        return false;
      } else {
        if (outputConversionToRationals != nullptr) {
          outputConversionToRationals->AddMonomial((*this)[i], *theCF);
        }
      }
    }
    return true;
  }
  template <class MonomialCollectionTemplate>
  static int GetRankOfSpanOfElements(
    List<MonomialCollectionTemplate>& theList, HashedList<templateMonomial>* seedMonomials = nullptr
  ) {
    List<MonomialCollectionTemplate> listCopy = theList;
    MonomialCollection<templateMonomial, coefficient>::GaussianEliminationByRowsDeleteZeroRows(listCopy, 0, seedMonomials);
    return listCopy.size;
  }
  template <class MonomialCollectionTemplate>
  static void GaussianEliminationByRowsDeleteZeroRows(
    List<MonomialCollectionTemplate >& theList,
    bool *IvemadeARowSwitch = nullptr,
    HashedList<templateMonomial>* seedMonomials = nullptr
  ) {
    MonomialCollectionTemplate::GaussianEliminationByRows(theList, IvemadeARowSwitch, seedMonomials);
    for (int j = theList.size - 1; j >= 0; j --) {
      if (theList[j].IsEqualToZero()) {
        theList.size --;
      }
    }
  }
  int SubtractMonomialNoCoeffCleanUpReturnsCoeffIndex(const templateMonomial& inputMon, const coefficient& inputCoeff);
  void CleanUpZeroCoeffs() {
    for (int i = 0; i < this->size; i ++) {
      if (this->CleanupMonIndex(i)) {
        i --;
      }
    }
  }
  template<class baseType>
  void SubstitutionCoefficients(const List<Polynomial<baseType> >& theSub) {
    MacroRegisterFunctionWithName("Polynomial::SubstitutionCoefficients");
    coefficient newCoeff;
    for (int i = 0; i < this->size(); i ++) {
      newCoeff = this->theCoeffs[i];
      newCoeff.Substitution(theSub);
      if (newCoeff.IsEqualToZero()) {
        this->PopMonomial(i);
        i --;
        continue;
      }
      this->theCoeffs[i] = newCoeff;
    }
  }
  void SubtractMonomial(const templateMonomial& inputMon, const coefficient& inputCoeff) {
    this->CleanupMonIndex(this->SubtractMonomialNoCoeffCleanUpReturnsCoeffIndex(inputMon, inputCoeff));
  }
  coefficient GetMonomialCoefficient(const templateMonomial& inputMon) const {
    int theIndex = this->theMonomials.GetIndex(inputMon);
    if (theIndex == - 1) {
      return 0;
    }
    return this->theCoeffs[theIndex];
  }
  bool operator>(const MonomialCollection<templateMonomial, coefficient>& other) const {
    if (this->size() > other.size()) {
      return true;
    }
    if (this->size() < other.size()) {
      return false;
    }
    MonomialCollection<templateMonomial, coefficient> leftCopy = *this;
    MonomialCollection<templateMonomial, coefficient> rightCopy = other;
    leftCopy.QuickSortAscending();
    rightCopy.QuickSortAscending();
    for (int i = leftCopy.size() - 1; i >= 0; i --) {
      if (leftCopy[i] > rightCopy[i]) {
        return true;
      }
      if (rightCopy[i] > leftCopy[i]) {
        return false;
      }
      if (leftCopy.theCoeffs[i] > rightCopy.theCoeffs[i]) {
        return true;
      }
      if (rightCopy.theCoeffs[i] > leftCopy.theCoeffs[i]) {
        return false;
      }
    }
    return false;
  }
  int TotalDegree();
  void CheckFlagDeallocated() const {
    if (
      this->theMonomials.flagDeallocated ||
      this->theCoeffs.flagDeallocated ||
      this->flagDeallocated
    ) {
      crash << "Use after free of monomial collection. " << crash;
    }
  }
  void CheckConsistencyGrandMaster() const {
    this->CheckConsistency();
    this->theMonomials.GrandMasterConsistencyCheck();
    for (int i = 0; i < this->size(); i ++) {
      this->theCoeffs[i].checkConsistency();
    }
  }
  void CheckConsistency() const {
    this->CheckFlagDeallocated();
    this->CheckNumCoeffsConsistency();
  }
  template <class baseRing>
  baseRing FindGCDCoefficientNumerators() const {
    if (this->size() == 0) {
      return 1;
    }
    baseRing result, tempCF;
    this->theCoeffs[0].GetNumerator(result);
    for (int i = 1; i < this->size(); i ++) {
      this->theCoeffs[i].GetNumerator(tempCF);
      coefficient::gcd(result, tempCF, result);
    }
    return result;
  }
  Rational FindGCDCoefficientNumeratorsOverRationals() const {
    if (this->size() == 0) {
      return 1;
    }
    LargeInt result, tempUI;
    result = this->theCoeffs[0].GetNumeratorRationalPart();
    result.sign = 1;
    for (int i = 1; i < this->size(); i ++) {
      tempUI = this->theCoeffs[i].GetNumeratorRationalPart();
      LargeIntUnsigned::gcd(result.value, tempUI.value, result.value);
    }
    return result;
  }
  Rational FindLCMCoefficientDenominatorsOverRationals() const {
    LargeInt result, tempUI;
    result = 1;
    for (int i = 0; i < this->size(); i ++) {
      tempUI = this->theCoeffs[i].GetDenominator();
      LargeIntUnsigned::lcm(result.value, tempUI.value, result.value);
    }
    return result;
  }
  Rational ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedByLeadingCoefficientPositive() {
    Rational result = this->ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
    if (this->GetLeadingCoefficient() < 0) {
      (*this) *= - 1;
      result *= - 1;
    }
    return result;
  }
  Rational ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy() {
    if (this->size() == 0) {
      return 1;
    }
    Rational result = 1;
    Rational tempRat;
    for (int i = 0; i < this->size(); i ++) {
      tempRat = this->theCoeffs[i].GetDenominatorRationalPart();
      *this *= tempRat;
      result *= tempRat;
    }
    LargeInt theGCD, tempUI;
    theGCD = this->theCoeffs[0].GetNumeratorRationalPart().GetNumerator();
    theGCD.sign = 1;
    for (int i = 0; i < this->size(); i ++) {
      tempUI = this->theCoeffs[i].GetNumeratorRationalPart().GetNumerator();
      LargeIntUnsigned::gcd(theGCD.value, tempUI.value, theGCD.value);
    }
    tempRat = theGCD;
    *this /= tempRat;
    result /= tempRat;
    return result;
  }

  static std::string GetBlendCoeffAndMon(
    const templateMonomial& inputMon, coefficient& inputCoeff, bool addPlusToFront, FormatExpressions* theFormat = nullptr
  );
  void CheckNumCoeffsConsistency() const {
    if (this->theCoeffs.size != this->theMonomials.size) {
      crash << "This is a programming error: a monomial collection has "
      << this->theMonomials.size << " monomials but "
      << this->theCoeffs.size << " coefficients. " << crash;
    }
  }
  bool IsEqualToZero() const;
  int FindMaxPowerOfVariableIndex(int VariableIndex);
  void MakeZero() {
    this->theMonomials.Clear();
    this->theCoeffs.SetSize(0);
  }
  int GetMinNumVars() const {
    int result = 0;
    for (int i = 0; i < this->size(); i ++) {
      result = MathRoutines::Maximum(result, (*this)[i].GetMinNumVars());
    }
    return result;
  }
  bool IsSmallInteger(int* whichInteger = nullptr) const;
  bool IsInteger(LargeInt* whichInteger = nullptr) const;
  void SetExpectedSize(int theSize) {
    this->theMonomials.SetExpectedSize(theSize);
    this->theCoeffs.SetExpectedSize(theSize);
  }
  bool HasGEQMonomial(templateMonomial& m, int& WhichIndex);
  void WriteToFile(std::fstream& output);
  void ClearDenominators(Rational& output) {
    output.MakeOne();
    Rational tempRat;
    for (int i = 0; i < this->size(); i ++) {
      if (!this->theCoeffs[i].IsInteger()) {
        tempRat = this->theCoeffs[i].GetDenominator();
        *this *= tempRat;
        output *= tempRat;
      }
    }
  }
  bool ReadFromFile(std::fstream& input);
  bool operator!=(const MonomialCollection<templateMonomial, coefficient>& other) const {
    return !(*this == other);
  }
  bool operator==(const MonomialCollection<templateMonomial, coefficient>& other) const;
  bool operator==(int x) const;
  void operator+=(const MonomialCollection<templateMonomial, coefficient>& other);
  MonomialCollection<templateMonomial, coefficient> operator*(const coefficient& other) const {
    MonomialCollection<templateMonomial, coefficient> result = *this;
    result *= other;
    return result;
  }
//  void operator+=(const templateMonomial& m)
//  { this->AddMonomial(m, 1);
//  }
  MonomialCollection<templateMonomial, coefficient> operator-(
    const MonomialCollection<templateMonomial, coefficient>& other
  ) const {
    MonomialCollection<templateMonomial, coefficient> output = *this;
    output -= other;
    return output;
  }
  MonomialCollection<templateMonomial, coefficient> operator+(
    const MonomialCollection<templateMonomial, coefficient>& other
  ) const {
    MonomialCollection<templateMonomial, coefficient> output = *this;
    output += other;
    return output;
  }
  void MakeConst(const coefficient& coeff) {
    templateMonomial tempM;
    tempM.MakeOne();
    this->AddMonomial(tempM, coeff);
  }
  void MakeOne() {
    this->MakeConst(1);
  }
  void operator+=(const int& other) {
    templateMonomial tempM;
    coefficient theCF = other;
    tempM.MakeOne();
    this->AddMonomial(tempM, theCF);
  }

  void operator+=(const coefficient& other) {
    templateMonomial tempM;
    tempM.MakeOne();
    this->AddMonomial(tempM, other);
  }
  void operator+=(const templateMonomial& other) {
    this->AddMonomial(other, 1);
  }
  void operator-=(const templateMonomial& other) {
    this->SubtractMonomial(other, 1);
  }
  void operator-=(const coefficient& other) {
    templateMonomial tempM;
    tempM.MakeOne();
    this->SubtractMonomial(tempM, other);
  }
  template <class OtherType>
  void AssignOtherType(const MonomialCollection<templateMonomial, coefficient>& other) {
    this->::HashedList<templateMonomial>::operator=(other);
    this->theCoeffs.SetSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->theCoeffs[i] = other.theCoeffs[i];
    }
  }
  void operator-=(const MonomialCollection<templateMonomial, coefficient>& other) {
    this->SubtractOtherTimesCoeff(other);
  }
  void SubtractOtherTimesCoeff(const MonomialCollection<templateMonomial, coefficient>& other, coefficient* inputcf = nullptr);
  template <class otherType>
  void operator/=(const otherType& other) {
    if (other == 0) {
      crash << "This is a programming error. A MonomialCollection division "
      << "by zero has been requested: division by zero error should"
      << " be handled before calling operator/=. " << crash;
      return;
    }
    for (int i = 0; i < this->theCoeffs.size; i ++) {
      this->theCoeffs[i] /= other;
    }
  }
  template <class otherType>
  void operator*=(const otherType& other) {
    for (int i = 0; i < this->theCoeffs.size; i ++) {
      this->theCoeffs[i] *= other;
      if (i == 0) {
        //to avoid implicit conversion problems, we make the zero check at the first cycle
        if (this->theCoeffs[i].IsEqualToZero()) {
          this->MakeZero();
          return;
        }
      }
    }
  }
  const templateMonomial& operator[](int theIndex) const {
    return this->theMonomials[theIndex];
  }
  void operator=(const templateMonomial& other) {
    templateMonomial otherCopy = other; //in case other is contained as a monomial in me and gets destroyed when I call
    //this->MakeZero(). This shouldn't yield a measurable slowdown.
    //Further this function is to be used for non-time critical operations.
    this->MakeZero();
    this->AddMonomial(otherCopy, 1);
  }
  template<class otherType>
  void operator=(const MonomialCollection<templateMonomial, otherType>& other) {
    this->theCoeffs = other.theCoeffs;
    this->theMonomials = other.theMonomials;
  }
};

class MonomialVector {
  friend std::ostream& operator << (std::ostream& output, const MonomialVector& theMon) {
    output << theMon.ToString();
    return output;
  }
  public:
  int theIndex;
  MonomialVector(): theIndex(- 1) {
  }
  MonomialVector(int inputIndex): theIndex(inputIndex) {
  }
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
  unsigned int HashFunction() const {
    return static_cast<unsigned int>(this->theIndex);
  }
  static unsigned int HashFunction(const MonomialVector& input) {
    return input.HashFunction();
  }
  bool operator==(const MonomialVector& other) const {
    return this->theIndex == other.theIndex;
  }
  void MakeEi(int inputIndex) {
    this->theIndex = inputIndex;
  }
  static bool IsMonEqualToZero() {
    return false;
  }
  bool operator>(const MonomialVector& other) const {
    return this->theIndex > other.theIndex;
  }
};

template <class coefficient>
class VectorSparse : public MonomialCollection<MonomialVector, coefficient> {
  public:
  void MaKeEi(int NonZeroIndex, const coefficient& theCoeff = 1) {
    this->MakeZero();
    MonomialVector theMon;
    theMon.MakeEi(NonZeroIndex);
    this->AddMonomial(theMon, theCoeff);
  }
  int GetLargestParticipatingBasisIndex() {
    int result = - 1;
    for (int i = 0; i < this->size(); i ++) {
      result = MathRoutines::Maximum(result, (*this)[i].theIndex);
    }
    return result;
  }
  bool operator>(const VectorSparse<coefficient>& other) const {
    return this->::MonomialCollection<MonomialVector, coefficient>::operator>(other);
  }
};

template <class templateMonomial, class coefficient>
class ElementMonomialAlgebra: public MonomialCollection<templateMonomial, coefficient> {
  public:
  void MultiplyBy(
    const ElementMonomialAlgebra<templateMonomial, coefficient>& other,
    ElementMonomialAlgebra<templateMonomial, coefficient>& output,
    ElementMonomialAlgebra<templateMonomial, coefficient>& bufferPoly,
    templateMonomial& bufferMon
  ) const;
  void MultiplyBy(
    const templateMonomial& other,
    const coefficient& theCoeff,
    ElementMonomialAlgebra<templateMonomial, coefficient>& output,
    ElementMonomialAlgebra<templateMonomial, coefficient>& bufferPoly,
    templateMonomial& bufferMon
  ) const;
  void MultiplyOnTheLeft(const ElementMonomialAlgebra<templateMonomial, coefficient>& standsOnTheLeft) {
    ElementMonomialAlgebra<templateMonomial, coefficient> tempMat, bufferPoly;
    templateMonomial bufferMon;
    standsOnTheLeft.MultiplyBy(*this, tempMat, bufferPoly, bufferMon);
    *this = tempMat;
  }
  void operator*=(const ElementMonomialAlgebra<templateMonomial, coefficient>& other) {
    ElementMonomialAlgebra<templateMonomial, coefficient> bufferPoly;
    templateMonomial bufferMon;
    this->MultiplyBy(other, *this, bufferPoly, bufferMon);
  }
  void operator*=(const coefficient& other) {
    this->::MonomialCollection<templateMonomial, coefficient>::operator*=(other);
  }
  void RaiseToPower(int d, ElementMonomialAlgebra<templateMonomial, coefficient>& output, const coefficient& theRingUniT);
  void RaiseToPower(int d) {
    if (d == 1) {
      return;
    }
    ElementMonomialAlgebra<templateMonomial, coefficient> theOne;
    theOne.MakeConst(1);
    MathRoutines::RaiseToPower(*this, d, theOne);
  }
};

template<class coefficient>
class PolynomialOrder {
  public:
  List<MonomialP>::OrderLeftGreaterThanRight theMonOrder;
  bool CompareLeftGreaterThanRight(const Polynomial<coefficient>& left, const Polynomial<coefficient>& right) const;
};

template<class coefficient>
class Polynomial: public ElementMonomialAlgebra<MonomialP, coefficient> {
public:
  friend std::iostream& operator<< <coefficient>(std::iostream& output, const Polynomial<coefficient>& input);
  Polynomial(int x) {
    this->MakeConst(x, 0);
  }
  Polynomial(){}
  Polynomial(const Polynomial<coefficient>& other) {
    this->operator=(other);
  }
  static bool flagAnErrorHasOccuredTimeToPanic;
  //to avoid code::blocks parsing problems (remove when code::blocks learns to parse parent classes:
  void AddMonomial(const MonomialP& inputMon, const coefficient& inputCoeff) {
    this->::MonomialCollection<MonomialP, coefficient>::AddMonomial(inputMon, inputCoeff);
  }
  unsigned int HashFunction() const {
    return this->::MonomialCollection<MonomialP, coefficient>::HashFunction();
  }
  static unsigned int HashFunction(const Polynomial<coefficient>& input) {
    return input.HashFunction();
  }
  void MultiplyBy(const MonomialP& other, const coefficient& theCoeff) {
    Polynomial<coefficient> output;
    output.MakeZero();
    output.AddMonomial(other, theCoeff);
    *this *= output;
  }
  void MultiplyBy(const MonomialP& other, const coefficient& theCoeff, Polynomial<coefficient>& output) const {
    output = *this;
    output.MultiplyBy(other, theCoeff);
  }
  //////////////////////////////////////////////
  void AssignFloor() const {
  }
  void AssignMinPoly(const Matrix<coefficient>& input);
  void AssignCharPoly(const Matrix<coefficient>& input);
  void AssignMonomialWithExponent(Vector<Rational>& r, const coefficient& theCoeff = 1) {
    this->MakeZero();
    this->AddMonomial(r, theCoeff);
  }
  void GetConstantTerm(coefficient& output, const coefficient& theRingZero = 0) const;
  coefficient GetConstantTerm(const coefficient& theRingZero = 0) const {
    coefficient result;
    this->GetConstantTerm(result, theRingZero);
    return result;
  }
  static bool IsMonEqualToZero() {
    return false;
  }
  static void GetValuesLagrangeInterpolandsAtConsecutivePoints(
    Vector<Rational>& inputConsecutivePointsOfInterpolation,
    Vector<Rational>& inputPointsOfEvaluation,
    Vectors<Rational>& outputValuesInterpolands
  );
  bool FactorMeOutputIsADivisor(Polynomial<Rational>& output, std::stringstream* comments);
  bool FactorMeNormalizedFactors(
    Rational& outputCoeff, List<Polynomial<Rational> >& outputFactors, std::stringstream* comments
  ) const;
  bool FactorMe(List<Polynomial<Rational> >& outputFactors, std::stringstream* comments) const;
  void Interpolate(const Vector<coefficient>& thePoints, const Vector<coefficient>& ValuesAtThePoints);
  bool FindOneVarRatRoots(List<Rational>& output);
  coefficient GetDiscriminant();
  void GetCoeffInFrontOfLinearTermVariableIndex(int index, coefficient& output);
  void MakeMonomiaL(int LetterIndex, const Rational& Power, const coefficient& Coeff = 1, int ExpectedNumVars = 0) {
    if (LetterIndex < 0) {
      crash << "This is a programming error: the index"
      << LetterIndex + 1 << " is  non-positive which is not allowed. " << crash;
    }
    int numVars = MathRoutines::Maximum(LetterIndex + 1, ExpectedNumVars);
    this->MakeZero();
    MonomialP tempM;
    tempM.MakeOne(numVars);
    tempM[LetterIndex] = Power;
    this->AddMonomial(tempM, Coeff);
  }
  void MakeDegreeOne(int NVar, int NonZeroIndex, const coefficient& coeff);
  void MakeDegreeOne(
    int NVar,
    int NonZeroIndex1,
    int NonZeroIndex2,
    const coefficient& coeff1,
    const coefficient& coeff2
  );
  void MakeDegreeOne(int NVar, int NonZeroIndex, const coefficient& coeff1, const coefficient& ConstantTerm);
  void MakeLinPolyFromRootNoConstantTerm(const Vector<Rational> & r);
  void MakeLinPolyFromRootLastCoordConst(const Vector<Rational> & input);
  void MakePolyFromDirectionAndNormal(
    Vector<coefficient>& direction, Vector<coefficient>& normal, coefficient& Correction
  );
  static bool GetLinearSystemFromLinearPolys(
    const List<Polynomial<coefficient> >& theLinPolys,
    Matrix<coefficient>& homogenousPart,
    Matrix<coefficient>& constTerms
  );
  bool IsOneVariablePoly(int* whichVariable = nullptr) const;
  bool IsOneVariableNonConstPoly(int* whichVariable = nullptr) const {
    int tempInt;
    if (whichVariable == nullptr) {
      whichVariable = &tempInt;
    }
    if (!this->IsOneVariablePoly(whichVariable)) {
      return false;
    }
    return *whichVariable != - 1;
  }
  Polynomial<coefficient> GetOne() const {
    Polynomial<coefficient> result;
    result.MakeOne();
    return result;
  }
  Polynomial<coefficient> GetZero() const {
    Polynomial<coefficient> result;
    result.MakeZero();
    return result;
  }
  Rational RationalValue() {
    Rational result;
    this->GetConstantTerm(result, 0);
    return result;
  }
  void MakeDeterminantFromSquareMatrix(const Matrix<Polynomial<coefficient> >& theMat);
  void MakeConst(const coefficient& theConst, int ExpectedNumVars = 0) {
    this->MakeZero();
    MonomialP theConstMon;
    theConstMon.MakeOne(ExpectedNumVars);
    this->AddMonomial(theConstMon, theConst);
  }
  void MakeOne(int ExpectedNumVars = 0);
  Rational ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy() {
    if (this->IsEqualToZero()) {
      return 1;
    }
    Rational result = this->ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
    if (this->theCoeffs[this->GetIndexMaxMonomialLexicographicLastVariableStrongest()].IsNegative()) {
      *this *= - 1;
      result *= - 1;
    }
    return result;
  }
  void GetPolyWithPolyCoeff(
    Selection& theNonCoefficientVariables, Polynomial<Polynomial<coefficient> >& output
  ) const;
  void GetPolyUnivariateWithPolyCoeffs(
    int theVar,
    Polynomial<Polynomial<coefficient> >& output
  ) const;
  void ScaleToIntegralNoGCDCoeffs();
  void TimesInteger(int a);
  void DivideBy(
    const Polynomial<coefficient>& inputDivisor,
    Polynomial<coefficient>& outputQuotient,
    Polynomial<coefficient>& outputRemainder
  ) const;
  void DivideByConstant(const coefficient& r);
  void AddConstant(const coefficient& theConst) {
    MonomialP tempMon;
    tempMon.MakeOne();
    this->AddMonomial(tempMon, theConst);
  }
  void ShiftVariableIndicesToTheRight(int VarIndexShift);
  void SetNumVariablesSubDeletedVarsByOne(int newNumVars);
  void SetDynamicSubtype(int newNumVars) {
    this->SetNumVariablesSubDeletedVarsByOne(newNumVars);
  }
  int GetHighestIndexSuchThatHigherIndexVarsDontParticipate() {
    int result = - 1;
    for (int i = 0; i < this->size; i ++) {
      result = MathRoutines::Maximum(result, this->TheObjects[i].GetHighestIndexSuchThatHigherIndexVarsDontParticipate());
    }
    return result;
  }
  void ScaleToPositiveMonomials(MonomialP& outputScale);
  void DecreaseNumVariables(int increment, Polynomial<coefficient>& output);
  bool Substitution(const List<Polynomial<coefficient> >& TheSubstitution);
  Rational TotalDegree() const{
    Rational result = 0;
    for (int i = 0; i < this->size(); i ++) {
      result = MathRoutines::Maximum((*this)[i].TotalDegree(), result);
    }
    return result;
  }
  int TotalDegreeInt() const;
  bool IsEqualToOne() const {
    coefficient tempC;
    if (this->IsConstant(&tempC)) {
      return tempC.IsEqualToOne();
    }
    return false;
  }
  bool IsMonomialCoeffOne() const {
    if (this->size() != 1) {
      return false;
    }
    return this->theCoeffs[0].IsEqualToOne();
  }
  bool IsOneLetterFirstDegree(int* whichLetter = nullptr) const {
    if (this->size() != 1) {
      return false;
    }
    return (*this)[0].IsOneLetterFirstDegree(whichLetter);
  }
  bool IsConstant(coefficient* whichConstant = nullptr) const {
    if (this->size() > 1) {
      return false;
    }
    if (this->size() == 0) {
      if (whichConstant != nullptr) {
        *whichConstant = 0;
      }
      return true;
    }
    if (whichConstant != nullptr) {
      *whichConstant = this->theCoeffs[0];
    }
    const MonomialP& theMon = (*this)[0];
    return theMon.IsConstant();
  }
  bool IsNegative() const {
    coefficient tempC;
    if (!this->IsConstant(&tempC)) {
      return false;
    }
    return tempC.IsNegative();
  }
  bool IsLinearNoConstantTerm() {
    for (int i = 0; i < this->size; i ++) {
      if (!this->TheObjects[i].IsLinearNoConstantTerm()) {
        return false;
      }
    }
    return true;
  }
  bool IsLinear() {
    for (int i = 0; i < this->size(); i ++) {
      if (!(*this)[i].IsLinear()) {
        return false;
      }
    }
    return true;
  }
  bool IsLinearGetRootConstantTermLastCoordinate(Vector<coefficient>& outputRoot) {
    outputRoot.MakeZero(this->GetMinNumVars() + 1);
    int index;
    for (int i = 0; i < this->size(); i ++) {
      if ((*this)[i].IsConstant()) {
        *outputRoot.LastObject() = this->theCoeffs[i];
      } else {
        if ((*this)[i].IsOneLetterFirstDegree(&index)) {
          outputRoot[index] = this->theCoeffs[i];
        } else {
          return false;
        }
      }
    }
    return true;
  }
  void RaiseToPower(int d) {
    if (d == 1) {
      return;
    }
    if (d < 0) {
      crash << "This is a programming error: attempting to raise the polynomial "
      << this->ToString() << " to the negative power "
      << d << ". " << crash;
    }
    Polynomial<coefficient> theOne;
    theOne.MakeOne(this->GetMinNumVars());
    MathRoutines::RaiseToPower(*this, d, theOne);
  }
  bool GetRootFromLinPolyConstTermLastVariable(Vector<coefficient>& outputRoot) {
    return this->IsLinearGetRootConstantTermLastCoordinate(outputRoot);
  }
  Matrix<coefficient> EvaluateUnivariatePoly(const Matrix<coefficient>& input);//<-for univariate polynomials only
  coefficient Evaluate(const Vector<coefficient>& input);
  bool IsProportionalTo(
    const Polynomial<coefficient>& other, coefficient& TimesMeEqualsOther, const coefficient& theRingUnit
  ) const;
  void DrawElement(DrawElementInputOutput& theDrawData, FormatExpressions& PolyFormatLocal);
  const MonomialP& GetMaxMonomial(List<MonomialP>::OrderLeftGreaterThanRight theMonOrder = nullptr) const {
    return (*this)[this->GetIndexMaxMonomial(theMonOrder)];
  }
  int GetIndexMaxMonomial(List<MonomialP>::OrderLeftGreaterThanRight theMonOrder = nullptr) const {
    if (this->size() == 0) {
      return - 1;
    }
    int result = 0;
    for (int i = 1; i < this->size(); i ++) {
      if (theMonOrder != nullptr) {
        if (theMonOrder((*this)[i], (*this)[result])) {
          result = i;
        }
      } else {
        if ((*this)[i] > (*this)[result]) {
          result = i;
        }
      }
    }
    return result;
  }
  int GetIndexMaxMonomialLexicographicLastVariableStrongest() const {
    return this->GetIndexMaxMonomial(MonomialP::LeftGreaterThanLexicographicLastVariableStrongest);
  }
//  void ComponentInFrontOfVariableToPower(int VariableIndex, ListPointers<Polynomial<coefficient> >& output, int UpToPower);
  int GetMaxPowerOfVariableIndex(int VariableIndex);
  bool operator<=(const coefficient& other) const {
    coefficient constME;
    if (!this->IsConstant(&constME)) {
      crash << "This may or may not be a programming error: "
      << "attempting to compare a non-constant polynomial to "
      << "a constant. I cannot judge at the moment whether allowing that is a good decision. "
      << "In any case, crashing to let you know. "
      << crash;
      return false;
    }
    return constME <= other;
  }
  bool operator<(const coefficient& other) const {
    coefficient constME;
    if (!this->IsConstant(&constME)) {
      crash << "This may or may not be a programming error: "
      << "attempting to compare a non-constant polynomial to "
      << " a constant. I cannot judge at the moment whether allowing "
      << "that is a good decision. In any case, crashing to let you know. "
      << crash;
      return false;
    }
    return constME<other;
  }

  bool operator>(const Polynomial<coefficient>& other) const {
    if (other.size() == 0) {
      if (this->size() == 0) {
        return false;
      } else {
        return true;
      }
    }
    if (this->size() == 0) {
      return false;
    }
    if (this->TotalDegree() > other.TotalDegree()) {
      return true;
    }
    if (this->TotalDegree() < other.TotalDegree()) {
      return false;
    }
    int thisMaxMonIndex = this->GetIndexMaxMonomialLexicographicLastVariableStrongest();
    int otherMaxMonIndex = other.GetIndexMaxMonomialLexicographicLastVariableStrongest();
    if ((*this)[thisMaxMonIndex] > other[otherMaxMonIndex]) {
      return true;
    }
    if (other[otherMaxMonIndex] > (*this)[thisMaxMonIndex]) {
      return false;
    }
    if (this->theCoeffs[thisMaxMonIndex] > other.theCoeffs[otherMaxMonIndex]) {
      return true;
    }
    if (other.theCoeffs[otherMaxMonIndex] > this->theCoeffs[thisMaxMonIndex]) {
      return false;
    }
    return false;
  }
  bool operator<=(const Polynomial<coefficient>& other) const {
    return !(*this > other);
  }
  //has to be rewritten please don't use!
  bool IsGreaterThanZeroLexicographicOrder();
  static bool IsGEQcompareByTopMonomialTotalDegThenLexicographic(
    const Polynomial<coefficient>& left, const Polynomial<coefficient>& right
  ) {
    if (left.IsEqualToZero()) {
      return right.IsEqualToZero();
    }
    if (right.IsEqualToZero()) {
      return true;
    }
    return left[left.GetIndexMaxMonomialTotalDegThenLexicographic()].MonomialP::IsGEQTotalDegThenLexicographic(
      right[right.GetIndexMaxMonomialTotalDegThenLexicographic()]
    );
  }
  static bool IsGEQcompareByTopMonomialLexicographicLastVarStrongest(
    const Polynomial<coefficient>& left, const Polynomial<coefficient>& right
  ) {
    if (left.IsEqualToZero()) {
      return right.IsEqualToZero();
    }
    if (right.IsEqualToZero()) {
      return true;
    }
    int indexMaxLeft = left.GetIndexMaxMonomialLexicographicLastVariableStrongest();
    int indexMaxRight = right.GetIndexMaxMonomialLexicographicLastVariableStrongest();
    return left[indexMaxLeft].MonomialP::IsGEQLexicographicLastVariableStrongest(right[indexMaxRight]);
  }
  bool IsEqualTo(const Polynomial<coefficient>& p) const {
    return *this == p;
  }
  void operator-=(int x) {
    MonomialP tempMon;
    tempMon.MakeOne();
    this->SubtractMonomial(tempMon, x);
  }
  void operator-=(const coefficient& other) {
    MonomialP tempMon;
    tempMon.MakeOne();
    this->SubtractMonomial(tempMon, other);
  }
  void operator-=(const Polynomial<coefficient>& other) {
    this->::MonomialCollection<MonomialP, coefficient>::operator-=(other);
  }
  void operator*=(const MonomialP& other) {
    Polynomial<coefficient> otherP;
    otherP.MakeZero();
    otherP.AddMonomial(other, 1);
    (*this) *= otherP;
  }
  void operator*=(const Polynomial<coefficient>& other) {
    this->::ElementMonomialAlgebra<MonomialP, coefficient>::operator*=(other);
  }
  Polynomial<coefficient> operator%(const Polynomial<coefficient>& other) {
    Polynomial<coefficient> temp;
    Polynomial<coefficient> result;
    this->DivideBy(other, temp, result);
    return result;
  }
  void operator/=(const Polynomial<coefficient>& other) {
    Polynomial<coefficient> tempMe = *this;
    Polynomial<coefficient> tempRemainder;
    tempMe.DivideBy(other, *this, tempRemainder);
  }
  void operator/=(int other) {
    this->::MonomialCollection<MonomialP, coefficient>::operator/= (other);
  }
  void operator/=(const coefficient& other) {
    this->::MonomialCollection<MonomialP, coefficient>::operator/= (other);
  }
  template <class otherType>
  void operator*=(const otherType& other) {
    this->::MonomialCollection<MonomialP, coefficient>::operator*= (other);
  }
  void operator=(const Polynomial<coefficient>& other) {
    this->::MonomialCollection<MonomialP, coefficient>::operator=(other);
  }
  template<class otherType>
  void operator=(const Polynomial<otherType>& other) {
    this->::MonomialCollection<MonomialP, coefficient>::operator=(other);
  }
  void operator=(const coefficient& other) {
    this->MakeConst(other);
  }
  void operator=(int other) {
    coefficient tempCF;
    tempCF = other;
    this->MakeConst(tempCF);
  }
  template <class otherType>
  void AssignOtherType(const Polynomial<otherType>& other) {
    this->NumVars = other.NumVars;
    this->::MonomialCollection<MonomialP, coefficient>::AssignOtherType(other);
  }
};

template <class coefficient>
class PolynomialSubstitution: public List<Polynomial<coefficient> > {
  public:
  //One of the main purposes of this class is to be used for carrying out substitutions.
  //the general format of the substitution is:
  // the i^th element denotes the image of x_i,
  // For example, if polynomials is the array
  // x_1+x_2 (poly in 3 variables)
  // x_1x_3+2 (poly in 3 variables)
  // this mens that it can be applied to polynomials in two variables
  // like this: x_1-> x_1+x_2
  // x_2-> (x_1x_3+2)
  // to produce a polynomial in three variables
  void MakeIdSubstitution(int numVars, const coefficient& theRingUnit = 1);
  void MakeIdLikeInjectionSub(int numStartingVars, int numTargetVarsMustBeLargerOrEqual, const coefficient& theRingUnit);
  //In the following function we have that:
  //the format of the linear substitution is:
  //theSub is a  whose number of rows minus 1 must equal the # number of
  //target variables and whose number of columns must equal the number of variables in
  //the current polynomial (this->NumVariables).
  //The first row denotes the constant term in the substitution of the respective variable!
  //An element in the x-th row and y-th column
  //is defined as element [x][y] !
  void MakeExponentSubstitution(Matrix<LargeInt>& theSub);
  void PrintPolys(std::string& output);
  void MakeSubstitutionLastVariableToEndPoint(int numVars, Polynomial<coefficient>& EndPoint);
  void AddConstant(coefficient& theConst);
  void TimesConstant(coefficient& r);
  void DivideByConstant(coefficient& r);
  int GetHighestIndexSuchThatHigherIndexVarsDontParticipate() {
    int result = - 1;
    for (int i = 0; i < this->size; i ++) {
      result = MathRoutines::Maximum(
        result, this->TheObjects[i].GetHighestIndexSuchThatHigherIndexVarsDontParticipate()
      );
    }
    return result;
  }
  void MakeZero(int NumVars) {
    for (int i = 0; i < this->size; i ++) {
      this->TheObjects[i].MakeZero(NumVars);
    }
  }
  std::string ToString(int numDisplayedEltsMinus1ForAll = - 1) const {
    std::string tempS;
    this->ToString(tempS, numDisplayedEltsMinus1ForAll);
    return tempS;
  }
  void ComputeDebugString();
  bool operator==(const PolynomialSubstitution& right);
  void Substitution(PolynomialSubstitution<coefficient>& theSub, int NumVarsTarget) {
    Polynomial<Rational>  tempP;
    for (int i = 0; i < this->size; i ++) {
      this->TheObjects[i].Substitution(theSub, tempP, NumVarsTarget, static_cast<Rational>(1));
      this->TheObjects[i] = tempP;
    }
  }
  void MakeOneParameterSubFromDirection(Vector<Rational>& direction) {
    MonomialP tempM;
    tempM.MakeOne(1);
    tempM[0] = 1;
    this->SetSize(direction.size);
    for (int i = 0; i < this->size; i ++) {
      this->TheObjects[i].MakeZero();
      this->TheObjects[i].AddMonomial(tempM,direction.TheObjects[i]);
    }
  }
  void MakeSubFromMatrixIntAndDen(Matrix<LargeInt>& theMat, LargeIntUnsigned& Den) {
    Matrix<Rational> tempMat;
    tempMat.AssignMatrixIntWithDen(theMat, Den);
    this->MakeLinearSubConstTermsLastRow(tempMat);
  }
  void ToString(std::string& output, int numDisplayedEltsMinus1ForAll) const {
    std::stringstream out;
    output.clear();
    FormatExpressions PolyFormatLocal;
    if (numDisplayedEltsMinus1ForAll == - 1) {
      numDisplayedEltsMinus1ForAll = this->size;
    }
    for (int i = 0; i < this->size; i ++) {
      out << "x_{" << i + 1 << "} \\mapsto " << this->TheObjects[i].ToString(&PolyFormatLocal);
      if (i != this->size - 1) {
        out << ", ";
      }
    }
    out << ": ";
    output = out.str();
  }
  void ComputeB(Polynomial<Rational> & output, int cutOffIndex, int theDimension) {
    PolynomialSubstitution EpsForm;
    Polynomial<Rational>  tempP;
    EpsForm.SetSize(theDimension);
    EpsForm.TheObjects[0] = this->TheObjects[0];
    for (int i = 1; i < theDimension; i ++) {
      EpsForm.TheObjects[i] = this->TheObjects[i];
      tempP = this->TheObjects[i - 1];
      tempP *= - 1;
      EpsForm.TheObjects[i] += tempP;
    }
    EpsForm.TheObjects[3] += this->TheObjects[4];
    for (int j = cutOffIndex + 1; j < theDimension; j ++) {
      if (!EpsForm.TheObjects[j].IsGreaterThanZeroLexicographicOrder()) {
        EpsForm.TheObjects[j] *= - 1;
      }
    }
    output.MakeZero();
    for (int i = 0; i < theDimension; i ++) {
      output += EpsForm.TheObjects[i];
    }
  }
  void MakeLinearSubConstTermsLastRow(Matrix<coefficient>& theMat);
  void MakeConstantShiftCoordinatesAreAdded(Vector<Rational>& shiftPerVariable);
  void MakeLinearSubbedVarsCorrespondToMatRows(Matrix<Rational>& theMat, Vector<Rational>& theConstants);
  void ComputeDiscreteIntegrationUpTo(int d);
  void MakeLinearSubOnLastVariable(int NumVars, Polynomial<Rational>& LastVarSub) {
    this->SetSize(NumVars);
    for (int i = 0; i < NumVars - 1; i ++) {
      this->TheObjects[i].MakeDegreeOne(NumVars, i, 1);
    }
    this->TheObjects[NumVars - 1] = LastVarSub;
  }
  void MakeSubNVarForOtherChamber(Vector<Rational>& direction, Vector<Rational>& normal, Rational& Correction) {
    this->SetSize(direction.size);
    Polynomial<Rational> TempPoly;
    TempPoly.MakePolyFromDirectionAndNormal(direction, normal, Correction);
    for (int i = 0; i < direction.size; i ++) {
      Polynomial<Rational> TempPoly2;
      TempPoly2 = TempPoly;
      TempPoly2 *= direction[i];
      TempPoly2 *= - 1;
      this->TheObjects[i].MakeDegreeOne(direction.size, i, 1);
      this->TheObjects[i] += TempPoly2;
    }
  }
  void MakeSubAddExtraVarForIntegration(Vector<Rational>& direction) {
    this->SetSize(direction.size);
    for (int i = 0; i < direction.size; i ++) {
      Rational tempRat;
      tempRat.Assign(direction[i]);
      tempRat.Minus();
      this->TheObjects[i].MakeDegreeOne(direction.size + 1, i, direction.size, 1, tempRat);
    }
  }
};

//std::iostream& operator<<(std::iostream& output, const RationalFunctionOld& theRF);
//std::iostream& operator>>(std::iostream& input, RationalFunctionOld& theRF);

template<class coefficient>
class GroebnerBasisComputation {
  public:
  PolynomialOrder<coefficient> thePolynomialOrder;
  Polynomial<coefficient> SoPolyBuf;
  Polynomial<coefficient> remainderDivision;
  Polynomial<coefficient> bufPoly;
  Polynomial<coefficient> bufPolyForGaussianElimination;
  MonomialP SoPolyLeftShift;
  MonomialP SoPolyRightShift;
  MonomialP bufferMoN1;
  List<Polynomial<coefficient> > theQuotients;
  List<Polynomial<coefficient> > theBasiS;
  List<Polynomial<coefficient> > basisCandidates;
  List<MonomialP> leadingMons;
  List<coefficient> leadingCoeffs;
  int RecursionCounterSerreLikeSystem;
  int NumVarsToSolveForStarT;
  int NumVariablesToSolveForAfterReduction;
  int NumberSerreSystemComputations;
  int NumberSerreVariablesOneGenerator;
  int NumberGBComputations;

  int MaxNumSerreSystemComputationsPreferred;
  int MaxNumGBComputations;
  int MaxNumBasisReductionComputations;
  int firstIndexLatexSlide;
  //bool flagBasisGuaranteedToGenerateIdeal;
  bool flagUseTheMonomialBranchingOptimization;
  bool flagDoProgressReport;
  bool flagDoSortBasis;
  bool flagDoLogDivision;
  bool flagStoreQuotients;
  bool flagSystemProvenToHaveNoSolution;
  bool flagSystemProvenToHaveSolution;
  bool flagSystemSolvedOverBaseField;
  bool flagUsingAlgebraicClosuRe;
  bool flagTryDirectlySolutionOverAlgebraicClosure;
  AlgebraicClosureRationals* theAlgebraicClosurE;
  HashedList<MonomialP> allMonomials;
  List<List<List<int> > > highlightMonsQuotients;
  List<List<List<int> > > highlightMonsRemainders;
  List<List<List<int> > > highlightMonsSubtracands;
  List<List<List<int> > > highlightMonsDivisors;
  List<List<int> > fcAnswerMonsQuotients;
  List<List<int> > fcAnswerMonsRemainders;
  List<List<int> > fcAnswerMonsSubtracands;
  List<List<int> > fcAnswerMonsDivisors;
  List<int> uncoverAllMonsQuotients;
  List<int> uncoverAllMonsRemainders;
  List<int> uncoverAllMonsSubtracands;
  List<int> uncoverAllMonsDivisors;
  List<int> uncoverMonsFinalRemainder;
  int highlightAllMonsFinalRemainder;
  List<int> additionalHighlightFinalRemainder;
  List<List<int> > additionalHighlightRemainders;
  List<int> firstNonZeroIndicesPerIntermediateSubtracand;
  std::stringstream longDivisionLog;
  MemorySaving<GroebnerBasisComputation<coefficient> > ComputationUsedInRecursiveCalls;
  MemorySaving<List<Polynomial<coefficient> > > intermediateRemainders;
  MemorySaving<List<List<MonomialP> > > intermediateHighlightedMons;
  MemorySaving<List<MonomialP> > intermediateHighestMonDivHighestMon;
  MemorySaving<List<coefficient> > intermediateCoeffs;
  MemorySaving<List<Polynomial<coefficient> > > intermediateSubtractands;
  MemorySaving<List<int> > intermediateSelectedDivisors;
  MemorySaving<Polynomial<coefficient> > startingPoly;
  MemorySaving<List<coefficient> > systemSolution;
  MemorySaving<Selection> solutionsFound;
  List<PolynomialSubstitution<coefficient> > theImpliedSubS;
  FormatExpressions theFormat;
  void SetSerreLikeSolutionIndex(int theIndex, const coefficient& theConst);
  void GetSubFromPartialSolutionSerreLikeSystem(PolynomialSubstitution<coefficient>& outputSub);
  std::string ToStringSerreLikeSolution();
  std::string GetPolynomialStringSpacedMonomialsHtml(
    const Polynomial<coefficient>& thePoly,
    const std::string& extraStyle,
    List<MonomialP>* theHighLightedMons = nullptr
  );
  std::string GetPolynomialStringSpacedMonomialsLaTeX(
    const Polynomial<coefficient>& thePoly,
    std::string* highlightColor = nullptr,
    List<MonomialP>* theHighLightedMons = nullptr,
    int* firstNonZeroIndex = nullptr
  );
  std::string GetSpacedMonomialsWithHighlightLaTeX(
    const Polynomial<coefficient>& thePoly,
    List<List<int> >* slidesToHighlightMon,
    List<int>* slidesToFcAnswer,
    List<int>* slidesToUncover,
    List<int>* slidesAdditionalHighlight,
    int slidesToUncoverAllMons,
    bool useColumnSeparator
  );
  void ComputeHighLightsFromRemainder(int remainderIndex, int& currentSlideNumber);
  std::string GetDivisionStringHtml();
  std::string GetDivisionStringLaTeX();
  std::string GetDivisionLaTeXSlide();

  int SelectPolyIndexToAddNext();
  bool AddPolysAndReduceBasis();
  bool TransformToReducedBasis(
    List<Polynomial<coefficient> >& inputOutpuT, int upperLimitPolyComputations
  );
  bool TransformToReducedGroebnerBasis(List<Polynomial<coefficient> >& inputOutpuT);
  bool TransformToReducedGroebnerBasisImprovedAlgorithm(
    List<Polynomial<coefficient> >& inputOutpuT, int upperComputationBound = - 1
  );
  void TrySettingValueToVariable(
    List<Polynomial<coefficient> >& inputSystem, const Rational& aValueToTryOnPreferredVariable
  );
  bool WrapUpGroebnerOnExceedingComputationLimit(List<Polynomial<coefficient> >& inputOutpuT);
  bool WrapUpOnGroebnerBasisSuccess(List<Polynomial<coefficient> >& inputOutpuT);
  GroebnerBasisComputation();
  void MakeMinimalBasis();
  int GetNumVars() const;
  std::string ToStringLetterOrder(bool addDollars) const;
  std::string ToStringPolynomialBasisStatus() const;
  std::string ToStringImpliedSubs();
  static int GetNumEquationsThatWouldBeLinearIfIsubbedVar(int theVarIndex, List<Polynomial<coefficient> >& input);
  static int GetNumVarsToSolveFor(const List<Polynomial<coefficient> >& input);
  static void GetVarsToSolveFor(const List<Polynomial<coefficient> >& input, Selection& output);
  static bool IsContradictoryReducedSystem(const List<Polynomial<coefficient> >& input);
  void RemainderDivisionWithRespectToBasis(
    Polynomial<coefficient>& inputOutput,
    Polynomial<coefficient>* outputRemainder = 0,
    int basisIndexToIgnore = - 1
  );
  std::string ToStringCalculatorInputFromSystem(const List<Polynomial<coefficient> >& inputSystem);
  void SolveSerreLikeSystem(List<Polynomial<coefficient> >& inputSystem);
  bool HasImpliedSubstitutions(
    List<Polynomial<coefficient> >& inputSystem,
    PolynomialSubstitution<coefficient>& outputSub
  );
  bool HasSingleMonomialEquation(const List<Polynomial<coefficient> >& inputSystem, MonomialP& outputMon);
  void SetUpRecursiveComputation(GroebnerBasisComputation& toBeModified);
  void ProcessSolvedSubcaseIfSolvedOrProvenToHaveSolution(GroebnerBasisComputation& potentiallySolvedCase);
  void SolveWhenSystemHasSingleMonomial(List<Polynomial<coefficient> >& inputSystem, const MonomialP& theMon);
  int GetPreferredSerreSystemSubIndex(List<Polynomial<coefficient> >& inputSystem);
  void SolveSerreLikeSystemRecursively(List<Polynomial<coefficient> >& inputSystem);
  void PolySystemSolutionSimplificationPhase(List<Polynomial<coefficient> >& inputSystem);
  void BackSubstituteIntoPolySystem(List<PolynomialSubstitution<coefficient> >& theImpliedSubs);
  void BackSubstituteIntoSinglePoly(
    Polynomial<coefficient>& thePoly, int theIndex, PolynomialSubstitution<coefficient>& theFinalSub
  );
  bool AddRemainderToBasis();
  bool GetOneVarPolySolution(const Polynomial<coefficient>& thePoly, coefficient& outputSolution);
   //Criterion from Cox, Little, O'Shea:
  static bool CriterionCLOsh(
    HashedListSpecialized<Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity> >& thePairs,
    List<MonomialP>& theLeadingMons,
    MonomialP& leadingTermLCM
  );
  void CheckConsistency();
  void initForSystemSolution();
  void initForGroebnerComputation(int expectedNumInputPolys);
  void initForDivisionAlone(List<Polynomial<coefficient> >& inputOutpuT);
};

class RationalFunctionOld {
private:
  void AddSameTypes(const RationalFunctionOld& other) {
    switch (this->expressionType) {
      case RationalFunctionOld::typeRational:
        this->ratValue += other.ratValue;
        break;
      case RationalFunctionOld::typePoly:
        this->Numerator.GetElement() += other.Numerator.GetElementConst();
        break;
      case RationalFunctionOld::typeRationalFunction:
        this->AddHonestRF(other);
        break;
    }
    this->ReduceMemory();
    if (!this->checkConsistency()) {
      crash << "Bad rational function. " << crash;
    }
  }
  void AddHonestRF(const RationalFunctionOld& other);
  void ReduceRFToPoly();
  void ReducePolyToRational() {
    if (this->expressionType == this->typePoly) {
      if (this->Numerator.GetElement().IsConstant()) {
        this->expressionType = this->typeRational;
        if (this->Numerator.GetElement().IsEqualToZero()) {
          this->ratValue.MakeZero();
        } else {
          this->ratValue = this->Numerator.GetElement().theCoeffs[0];
        }
        this->Numerator.FreeMemory();
      }
    }
//    int commentMEWhenDone;
//    if (!this->checkConsistency())
//      crash << crash;
  }
  bool ConvertToType(int theType);
public:
  friend std::ostream& operator<< (std::ostream& output, const RationalFunctionOld& theRF) {
    output << theRF.ToString();
    return output;
  }
  friend std::istream& operator>> (std::istream& input, RationalFunctionOld& theRF);

  MemorySaving<Polynomial<Rational> > Numerator;
  MemorySaving<Polynomial<Rational> > Denominator;
  Rational ratValue;
  int expressionType;
  enum typeExpression{ typeRational = 0, typePoly = 1, typeRationalFunction = 2, typeError = 3};
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
  bool NeedsParenthesisForMultiplication() const {
    switch(this->expressionType) {
      case RationalFunctionOld::typeRational:
        return false;
      case RationalFunctionOld::typePoly:
        return this->Numerator.GetElementConst().NeedsParenthesisForMultiplication();
      case RationalFunctionOld::typeRationalFunction:
        return false;
    }
    return false;
  }
  bool FindOneVarRatRoots(List<Rational>& output);
  Rational GetDenominatorRationalPart() {
    Polynomial<Rational> Num;
    this->GetNumerator(Num);
    return Num.FindLCMCoefficientDenominatorsOverRationals();
  }
  Rational GetNumeratorRationalPart() {
    Polynomial<Rational> Num;
    this->GetNumerator(Num);
    return Num.FindGCDCoefficientNumeratorsOverRationals();
  }
  RationalFunctionOld GetOne() const;
  RationalFunctionOld GetZero() const;
  int GetMinNumVars() const;
  bool Substitution(const PolynomialSubstitution<Rational>& theSub);
  RationalFunctionOld(const RationalFunctionOld& other);
  RationalFunctionOld();
  RationalFunctionOld(int other);
  RationalFunctionOld(const Rational& other);
  Rational RationalValue() const;
  void RaiseToPower(int thePower);
  void ReduceMemory() {
    this->ReduceRFToPoly();
    this->ReducePolyToRational();
    if (!this->checkConsistency()) {
      crash << "Corrupt rational function. " << crash;
    }
  }
  void operator=(const Polynomial<Rational>& other) {
    this->expressionType = this->typePoly;
    this->Numerator.GetElement() = other;
    this->ReduceMemory();
  }
  unsigned int HashFunction() const {
    switch (this->expressionType) {
      case RationalFunctionOld::typeRational:
        return this->ratValue.HashFunction();
      case RationalFunctionOld::typePoly:
        return this->Numerator.GetElementConst().HashFunction();
      case RationalFunctionOld::typeRationalFunction:
        return this->Numerator.GetElementConst().HashFunction() * SomeRandomPrimes[0] +
        this->Denominator.GetElementConst().HashFunction() * SomeRandomPrimes[1];
      default:
        return static_cast<unsigned int>(- 1);
    }
  }
  static unsigned int HashFunction(const RationalFunctionOld& input) {
    return input.HashFunction();
  }
  void operator=(int other) {
    this->MakeConst(other);
  }
  void operator=(const Rational& other) {
    this->MakeConst(other);
  }
  void operator=(const RationalFunctionOld& other);
  bool checkConsistency() const;
  void SetNumVariables(int GoalNumVars) {
    this->SetNumVariablesSubDeletedVarsByOne(GoalNumVars);
  }
  void SetNumVariablesSubDeletedVarsByOne(int newNumVars);
  void MakeOneLetterMoN(int theIndex, const Rational& theCoeff, int ExpectedNumVars = 0);
  void GetNumerator(Polynomial<Rational>& output) const {
    switch (this->expressionType) {
      case RationalFunctionOld::typeRational:
        output.MakeConst(this->ratValue);
        return;
      default:
        output = this->Numerator.GetElementConst();
        return;
    }
  }
  bool IsNegative() {
    if (this->expressionType == this->typeRational) {
      return this->ratValue.IsNegative();
    }
    return false;
  }
  void GetDenominator(Polynomial<Rational>& output) const {
    switch (this->expressionType) {
      case RationalFunctionOld::typeRationalFunction:
        if (this->Denominator.IsZeroPointer()) {
          crash << "This is a programming error: the rational function is "
          << "supposed to be honest, but the denominator pointer is zero. " << crash;
        }
        output = this->Denominator.GetElementConst();
        return;
      default:
        output.MakeConst(Rational(1));
        return;
    }
  }
  void ClearDenominators(RationalFunctionOld& outputWasMultipliedBy);
  void operator+=(const Polynomial<Rational>& other) {
    RationalFunctionOld tempOther;
    tempOther = other;
    *this += tempOther;
  }
  void operator+=(const RationalFunctionOld& other);
  bool operator==(int other) const {
    if (other == 0) {
      return this->IsEqualToZero();
    } else {
      return this->expressionType == this->typeRational && (this->ratValue == other);
    }
  }
  bool operator!=(int other) {
    return !(this->operator==(other));
  }
  bool operator==(const RationalFunctionOld& other) const {
    return this->IsEqualTo(other);
  }
  void Simplify();
  void SimplifyLeadingCoefficientOnly();
  void operator+=(int theConstant);
  void operator*=(const RationalFunctionOld& other);
  void operator*=(const Polynomial<Rational>& other);
  void operator*=(const MonomialP& other);
  void operator*=(const Rational& other);
  void operator*=(int other) {
    *this *= Rational(other);
  }
  bool operator<(const RationalFunctionOld& other) const {
    return other > *this;
  }
  bool operator<=(const RationalFunctionOld& other) const {
    if (*this == other) {
      return true;
    }
    return other > *this;
  }
  bool operator>(const RationalFunctionOld& other) const {
    if (this->expressionType < other.expressionType) {
      return false;
    }
    if (this->expressionType > other.expressionType) {
      return true;
    }
    switch (this->expressionType) {
      case RationalFunctionOld::typeRational:
        return this->ratValue > other.ratValue;
      case RationalFunctionOld::typePoly:
        return this->Numerator.GetElementConst() > other.Numerator.GetElementConst();
      case RationalFunctionOld::typeRationalFunction:
        if (other.Denominator.GetElementConst() > this->Denominator.GetElementConst()) {
          return true;
        }
        if (this->Denominator.GetElementConst() > other.Denominator.GetElementConst()) {
          return false;
        }
        return this->Numerator.GetElementConst() > other.Numerator.GetElementConst();
      default:
        return false;
    }
  }
  void TimesConstant(const Rational& theConst) {
    this->operator*=(theConst);
  }
  void Invert();
  void MakeOne();
  void MakeZero();
  void MakeMonomiaL(int LetterIndex, const Rational& Power, const Rational& Coeff=1, int ExpectedNumVars = 0);
  void MakeConst(const Rational& theCoeff) {
    this->MakeZero();
    this->ratValue = theCoeff;
  }
  bool IsConstant(Rational* whichConstant = nullptr) const {
    if (this->expressionType != this->typeRational) {
      return false;
    }
    if (whichConstant != nullptr) {
      *whichConstant = this->ratValue;
    }
    return true;
  }
  bool IsInteger() const {
    return this->expressionType == this->typeRational && this->ratValue.IsInteger();
  }
  bool IsSmallInteger(int* whichInteger = nullptr) const {
    return this->expressionType == this->typeRational &&
    this->ratValue.IsSmallInteger(whichInteger);
  }
  bool IsEqualToZero() const {
    return this->expressionType == this->typeRational && this->ratValue.IsEqualToZero();
  }
  bool IsEqualToOne() const {
    return this->expressionType == this->typeRational && this->ratValue.IsEqualToOne();
  }
  bool IsEqualTo(const RationalFunctionOld& other) const {
    if (this->expressionType != other.expressionType) {
      return false;
    }
    switch (this->expressionType) {
      case RationalFunctionOld::typeRationalFunction:
        return this->Numerator.GetElementConst().IsEqualTo(other.Numerator.GetElementConst()) &&
        this->Denominator.GetElementConst().IsEqualTo(other.Denominator.GetElementConst());
      case RationalFunctionOld::typePoly:
        return this->Numerator.GetElementConst().IsEqualTo(other.Numerator.GetElementConst());
      case RationalFunctionOld::typeRational:
        return this->ratValue == other.ratValue;
    }
    crash << "This line of code is supposed to be unreachable. " << crash;
    return false;
  }
  static bool GetRelations(
    const List<Polynomial<Rational> >& inputElements,
    List<Polynomial<Rational> >& outputGeneratorLabels,
    List<Polynomial<Rational> >& outputRelations,
    std::stringstream& comments
  );
  static bool gcdQuicK(
    const Polynomial<Rational>& left, const Polynomial<Rational>& right, Polynomial<Rational>& output
  );
  static void ScaleClearDenominator(List<RationalFunctionOld>& input, Vector<Polynomial<Rational> >& output);
  static void gcd(const Polynomial<Rational>& left, const Polynomial<Rational>& right, Polynomial<Rational>& output);
  static void lcm(const Polynomial<Rational>& left, const Polynomial<Rational>& right,
  Polynomial<Rational>& output);
  void operator-=(int other) {
    *this -= static_cast<Rational>(other);
  }
  void operator-=(const Rational& other);
  void operator-=(const RationalFunctionOld& other);
  void operator/=(int other);
  void operator/=(const Polynomial<Rational>& other);
  void operator/=(const RationalFunctionOld& other);
  void Minus();
};

template <class templateMonomial, class coefficient>
bool MonomialCollection<templateMonomial, coefficient>::operator==(int x) const {
  if (x == 0) {
    return this->size() == 0;
  }
  crash << "This is either a programming error, or an unforeseen use of operator==. "
  << "If the second is the case, an audit/careful "
  << "proofreading of the code calling this function is needed; "
  << "I am crashing just in case. " << crash;
  return false;
}

template <class templateMonomial, class coefficient>
bool MonomialCollection<templateMonomial, coefficient>::operator==(
  const MonomialCollection<templateMonomial, coefficient>& other
) const {
  if (this->theCoeffs.size != other.theCoeffs.size) {
    return false;
  }
  for (int i = 0; i < this->size(); i ++) {
    int indexInOther = other.theMonomials.GetIndex((*this)[i]);
    if (indexInOther == - 1) {
      return false;
    }
    if (!(this->theCoeffs[i] == other.theCoeffs[indexInOther])) {
      return false;
    }
  }
  return true;
}

template <class templateMonomial, class coefficient>
void MonomialCollection<templateMonomial, coefficient>::WriteToFile(std::fstream& output) {
  output << XML::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  output << " numMons: " << this->size();
  for (int i = 0; i < this->size(); i ++) {
    output << " ";
    if (i > 0) {
      if (!this->theCoeffs[i].BeginsWithMinus()) {
        output << "+ ";
      }
    }
    this->theCoeffs[i].WriteToFile(output);
    const templateMonomial& tempM = (*this)[i];
    tempM.WriteToFile(output);
  }
  output << XML::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName()) << "\n";
}

template <class templateMonomial, class coefficient>
bool MonomialCollection<templateMonomial, coefficient>::ReadFromFile(std::fstream& input) {
  int numReadWords, targetSize;
  XML::ReadThroughFirstOpenTag(input, numReadWords, this->GetXMLClassName());
  std::string ReaderString;
  bool result = true;
  input >> ReaderString >> targetSize;
  if (ReaderString != "numMons:" ) {
    crash << "Bad monomial collection in file. " << crash;
    return false;
  }
  this->MakeZero();
  templateMonomial tempM;
  this->SetExpectedSize(targetSize);
  input.ignore();
  coefficient theCoeff;
  for (int i = 0; i < targetSize; i ++) {
    if (input.peek() == '+') {
      input >> ReaderString;
    }
    theCoeff.ReadFromFile(input);
    tempM.ReadFromFile(input);
    if (!result) {
      break;
    }
    this->AddMonomial(tempM, theCoeff);
  }
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, this->GetXMLClassName());
  if (numReadWords != 0) {
    crash << "Trailing words while reading monomial collection. " << crash;
  }
  return result;
}

template <class templateMonomial, class coefficient>
bool MonomialCollection<templateMonomial, coefficient>::IsEqualToZero() const {
  return this->size() == 0;
}

template <class templateMonomial, class coefficient>
bool MonomialCollection<templateMonomial, coefficient>::IsInteger(LargeInt* whichInteger) const {
  if (this->size() > 1) {
    return false;
  }
  if (this->size() == 0) {
    if (whichInteger != nullptr) {
      *whichInteger = 0;
    }
    return true;
  }
  bool result = (*this)[0].IsConstant();
  if (result) {
    result = this->theCoeffs[0].IsInteger(whichInteger);
  }
  return result;
}

template <class templateMonomial, class coefficient>
bool MonomialCollection<templateMonomial, coefficient>::IsSmallInteger(int* whichInteger) const {
  if (this->size() > 1) {
    return false;
  }
  if (this->size() == 0) {
    if (whichInteger != nullptr) {
      *whichInteger = 0;
    }
    return true;
  }
  bool result = (*this)[0].IsConstant();
  if (result) {
    result = this->theCoeffs[0].IsSmallInteger(whichInteger);
  }
  return result;
}

template <class templateMonomial, class coefficient>
void MonomialCollection<templateMonomial, coefficient>::GetVectorMonsAscending(Vector<coefficient>& result) {
  this->QuickSortAscending();
  result = this->theCoeffs;
}

template <class templateMonomial, class coefficient>
void MonomialCollection<templateMonomial, coefficient>::GetVectorMonsDescending(Vector<coefficient>& result) {
  this->QuickSortDescending();
  result = this->theCoeffs;
}

template <class coefficient>
void Polynomial<coefficient>::MakeLinPolyFromRootLastCoordConst(const Vector<Rational>& input) {
  this->MakeZero();
  MonomialP tempM;
  for (int i = 0; i < input.size - 1; i ++) {
    tempM.MakeEi(i);
    this->AddMonomial(tempM, input[i]);
  }
  this->operator+=(*input.LastObject());
}

template <class coefficient>
void Polynomial<coefficient>::MakeLinPolyFromRootNoConstantTerm(const Vector<Rational>& r) {
  this->MakeZero();
  MonomialP tempM;
  for (int i = 0; i < r.size; i ++) {
    tempM.MakeEi(i);
    if (!r[i].IsInteger()) {
      crash << "This may or may not be a programming error: requesting a monomial with non-integer exponent. "
      << "It appears this operation should be allowed, however "
      << "this requires further consideration. I am crashing just in case. "
      << crash;
    }
    this->AddMonomial(tempM, r[i].GetNumerator());
  }
}

template <class templateMonomial, class coefficient>
void MonomialCollection<templateMonomial, coefficient>::SubtractOtherTimesCoeff(
  const MonomialCollection<templateMonomial, coefficient>& other, coefficient* inputcf
) {
  if (this == &other) {
    if (inputcf == nullptr) {
      this->MakeZero();
      return;
    }
    MonomialCollection<templateMonomial, coefficient> otherNew = other;
    this->SubtractOtherTimesCoeff(otherNew, inputcf);
    return;
  }
  this->SetExpectedSize(other.size() + this->size());
  coefficient tempCF;
  for (int i = 0; i < other.size(); i ++) {
    ParallelComputing::SafePointDontCallMeFromDestructors();
    tempCF = other.theCoeffs[i];
    if (inputcf != nullptr) {
      tempCF *= *inputcf;
    }
    this->SubtractMonomial(other[i], tempCF);
  }
}

template <class templateMonomial, class coefficient>
void MonomialCollection<templateMonomial, coefficient>::AddOtherTimesConst(
  MonomialCollection<templateMonomial, coefficient>& other, const coefficient& theConst
) {
  this->SetExpectedSize(other.size() + this->size());
  coefficient tempCF;
  for (int i = 0; i < other.size(); i ++) {
    ParallelComputing::SafePointDontCallMeFromDestructors();
    tempCF = theConst;
    tempCF *= other.theCoeffs[i];
    this->AddMonomial(other[i], tempCF);
  }
}

template <class templateMonomial, class coefficient>
void MonomialCollection<templateMonomial, coefficient>::operator+=(
  const MonomialCollection<templateMonomial, coefficient>& other
) {
  this->SetExpectedSize(other.size() + this->size());
  for (int i = 0; i < other.size(); i ++) {
    ParallelComputing::SafePointDontCallMeFromDestructors();
    this->AddMonomial(other[i], other.theCoeffs[i]);
  }
}

template <class templateMonomial, class coefficient>
bool MonomialCollection<templateMonomial, coefficient>::HasGEQMonomial(templateMonomial& m, int& WhichIndex) {
  for (int i = 0; i < this->size; i ++) {
    if (this->TheObjects[i].IsGEQpartialOrder(m)) {
      WhichIndex = i;
      return true;
    }
  }
  WhichIndex = - 1;
  return false;
}

template <class templateMonomial, class coefficient>
template <class MonomialCollectionTemplate>
bool MonomialCollection<templateMonomial, coefficient>::LinSpanContainsGetFirstLinearCombination(
  const List<MonomialCollectionTemplate>& theList,
  const MonomialCollectionTemplate& input,
  Vector<coefficient>& outputFirstLinearCombination,
  HashedList<templateMonomial>* seedMonomials
) {
  List<MonomialCollectionTemplate> listCopy = theList;
  Matrix<coefficient> theRowOperations;
  theRowOperations.MakeIdMatrix(theList.size);
  MonomialCollection<templateMonomial, coefficient>::GaussianEliminationByRows(listCopy, 0, seedMonomials, &theRowOperations);
  MonomialCollectionTemplate remainderFromInput = input;
  templateMonomial currentMon;
  coefficient CFminMon, CFinRemainder;
  outputFirstLinearCombination.MakeZero(listCopy.size);
  for (int i = 0; i < listCopy.size; i ++) {
    if (listCopy[i].IsEqualToZero()) {
      break;
    }
    listCopy[i].GetMinMonomial(currentMon, CFminMon);
    CFinRemainder = remainderFromInput.GetMonomialCoefficient(currentMon);
    outputFirstLinearCombination[i] = CFinRemainder;
    outputFirstLinearCombination[i] /= CFminMon;
    remainderFromInput -= listCopy[i] * outputFirstLinearCombination[i];
  }
  if (!remainderFromInput.IsEqualToZero()) {
    return false;
  }
  theRowOperations.ActMultiplyVectorRowOnTheRight(outputFirstLinearCombination);
  return true;
}

template <class templateMonomial, class coefficient>
template <class MonomialCollectionTemplate>
void MonomialCollection<templateMonomial, coefficient>::GaussianEliminationByRows(
  List<MonomialCollectionTemplate>& theList,
  bool *IHaveMadeARowSwitch,
  HashedList<templateMonomial>* seedMonomials,
  Matrix<coefficient>* carbonCopyMatrix,
  List<MonomialCollectionTemplate>* carbonCopyList
) {
  MacroRegisterFunctionWithName("MonomialCollection::GaussianEliminationByRows");
  if (carbonCopyMatrix != 0) {
    if (carbonCopyMatrix->NumRows != theList.size) {
      crash << "This is a programming error: carbon copy matrix has "
      << carbonCopyMatrix->NumRows << " rows, while the gaussian-eliminated list has " << theList.size
      << " elements; the two numbers must be the same!" << crash;
    }
  }
  if (carbonCopyList != 0) {
    if (carbonCopyList->size != theList.size) {
      crash << "This is a programming error: carbon copy list has "
      << carbonCopyList->size << " elements, while the gaussian-eliminated list has "
      << theList.size << " elements; the two numbers must be the same!" << crash;
    }
  }
  MemorySaving<HashedList<templateMonomial> > bufferMons;
  HashedList<templateMonomial>& allMons = seedMonomials == 0 ? bufferMons.GetElement() : *seedMonomials;
  if (seedMonomials == 0) {
    int topBoundNumMons = 0;
    for (int i = 0; i < theList.size; i ++) {
      topBoundNumMons += theList[i].size();
    }
    allMons.SetExpectedSize(topBoundNumMons);
  }
  for (int i = 0; i < theList.size; i ++) {
    allMons.AddOnTopNoRepetition(theList[i].theMonomials);
  }
  allMons.QuickSortAscending();
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML = true;
  tempFormat.flagUseLatex = true;
  if (IHaveMadeARowSwitch != nullptr) {
    *IHaveMadeARowSwitch = false;
  }
  int currentRowIndex = 0;
  coefficient tempCF, tempCF2;
  for (int i = 0; i < allMons.size && currentRowIndex < theList.size; i ++) {
    const templateMonomial& currentMon = allMons[i];
    int goodRow = currentRowIndex;
    for (; goodRow < theList.size; goodRow ++) {
      if (theList[goodRow].theMonomials.Contains(currentMon)) {
        break;
      }
    }
    if (goodRow >= theList.size) {
      continue;
    }
    if (currentRowIndex != goodRow) {
      theList.SwapTwoIndices(currentRowIndex, goodRow);
      if (carbonCopyList != 0) {
        carbonCopyList->SwapTwoIndices(currentRowIndex, goodRow);
      }
      if (carbonCopyMatrix != 0) {
        carbonCopyMatrix->SwitchTwoRows(currentRowIndex, goodRow);
      }
      if (IHaveMadeARowSwitch != nullptr) {
        *IHaveMadeARowSwitch = true;
      }
    }
    MonomialCollection<templateMonomial, coefficient>& currentPivot = theList[currentRowIndex];
    int colIndex = currentPivot.theMonomials.GetIndex(currentMon);
    if (colIndex == - 1) {
      crash << "This is a programming error. An internal check at the "
      << "Gaussian elimination method for monomial collections fails. "
      << "Something is wrong. Here is the List you wanted to perform Gaussian elimination upon. "
      << theList.ToString() << ". " << crash;
    }
    tempCF = currentPivot.theCoeffs[colIndex];
    tempCF.Invert();
    currentPivot *= tempCF;
    if (carbonCopyMatrix != 0) {
      carbonCopyMatrix->RowTimesScalar(currentRowIndex, tempCF);
    }
    if (carbonCopyList != 0) {
      (*carbonCopyList)[currentRowIndex] *= tempCF;
    }
    for (int j = 0; j < theList.size; j ++) {
      if (j != currentRowIndex) {
        MonomialCollection<templateMonomial, coefficient>& currentOther = theList[j];
        int otherColIndex = currentOther.theMonomials.GetIndex(currentMon);
        if (otherColIndex != - 1) {
          tempCF = currentOther.theCoeffs[otherColIndex];
          currentOther.SubtractOtherTimesCoeff(currentPivot, &tempCF);
          if (carbonCopyList != 0) {
            (*carbonCopyList)[j].SubtractOtherTimesCoeff((*carbonCopyList)[currentRowIndex], &tempCF);
          }
          if (carbonCopyMatrix != 0) {
            tempCF2 = tempCF;
            tempCF2 *= - 1;
            carbonCopyMatrix->AddTwoRows(currentRowIndex, j, 0, tempCF2);
          }
        }
      }
    }
    currentRowIndex ++;
  }
}

template <class templateMonomial, class coefficient>
int MonomialCollection<templateMonomial, coefficient>::AddMonomialNoCoeffCleanUpReturnsCoeffIndex(
  const templateMonomial& inputMon, const coefficient& inputCoeff
) {
  this->CheckConsistency();
  if (inputCoeff == 0 || inputMon.IsMonEqualToZero()) {
    return - 1;
  }
  if (this->flagDeallocated) {
    crash << "Use after free or race condition on monomial collection. " << crash;
  }
  bool oldFlagDeallocated = this->flagDeallocated;
  this->flagDeallocated = true;
  int j = this->theMonomials.GetIndex(inputMon);
  if (j >= this->size()) {
    crash << "Error: function GetIndex evaluated on "
    << inputMon << " with hash function "
    << inputMon.HashFunction(inputMon)
    << " returns index " << j << " but I have only "
    << this->size() << " elements. ";
    crash << crash;
  }
  if (j == - 1) {
    this->theMonomials.AddOnTop(inputMon);
    this->theCoeffs.AddOnTop(inputCoeff);
    j = this->size() - 1;
  } else {
    if (j >= this->theCoeffs.size) {
      crash << "This is a programming error. "
      << "Looking for coefficient index " << j
      << " when number of coefficients is "
      << this->theCoeffs.size <<  ". " << crash;
    }
    this->theCoeffs[j] += inputCoeff;
  }
  this->flagDeallocated = oldFlagDeallocated;
  return j;
}

template <class templateMonomial, class coefficient>
int MonomialCollection<templateMonomial, coefficient>::SubtractMonomialNoCoeffCleanUpReturnsCoeffIndex(
  const templateMonomial& inputMon, const coefficient& inputCoeff
) {
  if (inputCoeff.IsEqualToZero() || inputMon.IsMonEqualToZero()) {
    return - 1;
  }
  int j = this->theMonomials.GetIndex(inputMon);
  if (j == - 1) {
    this->theMonomials.AddOnTop(inputMon);
    this->theCoeffs.AddOnTop(inputCoeff);
    *this->theCoeffs.LastObject() *= - 1;
    j = this->theCoeffs.size - 1;
  } else {
    this->theCoeffs[j] -= inputCoeff;
  }
  return j;
}

template <class templateMonomial, class coefficient>
void ElementMonomialAlgebra<templateMonomial, coefficient>::RaiseToPower(
  int d, ElementMonomialAlgebra<templateMonomial, coefficient>& output, const coefficient& theRingUniT
) {
  ElementMonomialAlgebra<templateMonomial, coefficient> tempOne;
  tempOne.MakeConst(theRingUniT);
  output = *this;
  MathRoutines::RaiseToPower(output, d, tempOne);
}

template <class coefficient>
void PolynomialSubstitution<coefficient>::PrintPolys(std::string& output) {
  std::stringstream out;
  FormatExpressions PolyFormatLocal;
  for (int i = 0; i < this->size; i ++) {
    std::string tempS;
    out << i << ". ";
    this->TheObjects[i]->StringPrintOutAppend(tempS, PolyFormatLocal);
    out << tempS << "\r\n";
  }
  output = out.str();
}

template <class coefficient>
void PolynomialSubstitution<coefficient>::MakeIdSubstitution(int numVars, const coefficient& theRingUnit) {
  this->MakeIdLikeInjectionSub(numVars, numVars, theRingUnit);
}

template <class coefficient>
void PolynomialSubstitution<coefficient>::MakeIdLikeInjectionSub(
  int numStartingVars, int numTargetVarsMustBeLargerOrEqual, const coefficient& theRingUnit
) {
  if (numStartingVars > numTargetVarsMustBeLargerOrEqual) {
    crash << "Bad number of variables. " << crash;
  }
  this->SetSize(numStartingVars);
  for (int i = 0; i < this->size; i ++) {
    Polynomial<coefficient>& currentPoly = this->TheObjects[i];
    currentPoly.MakeDegreeOne(numTargetVarsMustBeLargerOrEqual, i, theRingUnit);
  }
}

template <class coefficient>
void PolynomialSubstitution<coefficient>::MakeExponentSubstitution(Matrix<LargeInt>& theSub) {
  Polynomial<coefficient> tempP;
  MonomialP tempM;
  tempM.MakeOne(theSub.NumRows);
  this->size = 0;
  this->SetSize(theSub.NumCols);
  for (int i = 0; i < theSub.NumCols; i ++) {
    for (int j = 0; j < theSub.NumRows; j ++) {
      tempM[j] = Rational(theSub(j, i));
    }
    tempP.MakeZero();
    tempP.AddMonomial(tempM, 1);
    this->TheObjects[i] = tempP;
  }
}

template <class coefficient>
void PolynomialSubstitution<coefficient>::MakeSubstitutionLastVariableToEndPoint(
  int numVars, Polynomial<coefficient>& EndPoint
) {
  this->SetSize(numVars);
  for (int i = 0; i < numVars - 1; i ++) {
    this->TheObjects[i].MakeDegreeOne(numVars, i, coefficient::TheRingUnit);
  }
  this->TheObjects[numVars - 1].CopyFromPoly(EndPoint);
}

class PartFractions;

class oneFracWithMultiplicitiesAndElongations {
public:
  ListLight<int> Multiplicities;
  ListLight<int> Elongations;
  void AddMultiplicity(int MultiplicityIncrement, int Elongation);
  int IndexLargestElongation();
  int GetLargestElongation();
  void GetPolyDenominator(Polynomial<LargeInt>& output, int MultiplicityIndex, Vector<Rational>& theExponent);
  int GetMultiplicityLargestElongation();
  int GetLCMElongations();
  int GetTotalMultiplicity() const;
  void invert();
  void init();
  static Vector<Rational> GetCheckSumRoot(int NumVars);
  unsigned int HashFunction() const;
  static unsigned int HashFunction(const oneFracWithMultiplicitiesAndElongations& input) {
    return input.HashFunction();
  }
  void ComputeOneCheckSum(Rational& output, const Vector<Rational>& theExp, int theDimension);
  bool IsHigherThan(oneFracWithMultiplicitiesAndElongations& f);
  void operator=(oneFracWithMultiplicitiesAndElongations& right);
  bool operator==(oneFracWithMultiplicitiesAndElongations& right);
  std::string ToString();
  void OneFracToStringBasisChange(
    PartFractions& owner,
    int indexElongation,
    Matrix<LargeInt>& VarChange,
    bool UsingVarChange,
    std::string& output,
    bool LatexFormat,
    int indexInFraction,
    int theDimension,
    FormatExpressions& PolyFormatLocal
  );
};

class LaTeXProcedures {
public:
  static const int ScaleFactor = 40;
  static const int FigureSizeX = 10; //in centimeters
  static const int FigureSizeY = 10; //in centimeters
  static const int FigureCenterCoordSystemX = 4; //in centimeters
  static const int FigureCenterCoordSystemY = 8; //in centimeters
  static const int TextPrintCenteringAdjustmentX = 3;
  static const int TextPrintCenteringAdjustmentY = 3;
  static void drawline(
    double X1, double Y1, double X2, double Y2,
    uint32_t thePenStyle, int ColorIndex, std::fstream& output, DrawingVariables& drawInput
  );
  static void drawTextDirectly(double X1, double Y1, const std::string& theText, int ColorIndex, std::fstream& output);
  static void beginDocument(std::fstream& output);
  static void endLatexDocument(std::fstream& output);
  static void beginPSTricks(std::fstream& output);
  static void endPSTricks(std::fstream& output);
  static void GetStringFromColorIndex(int ColorIndex, std::string &output, DrawingVariables& drawInput);
};

class permutation: public SelectionWithDifferentMaxMultiplicities {
public:
  void initPermutation(int n);
  void initPermutation(List<int>& disjointSubsets, int TotalNumElements);
  void incrementAndGetPermutation(List<int>& output);
  void GetPermutationLthElementIsTheImageofLthIndex(List<int>& output);
  int GetNumPermutations() {
    return this->TotalNumSubsetsMustBeSmalInt();
  }
};

template<typename coefficient>
void Matrix<coefficient>::NonPivotPointsToEigenVectorMatrixForm(
  Selection& TheNonPivotPoints, Matrix<coefficient>& output
) {
  int RowCounter = 0;
  output.init(this->NumCols, 1);
  for (int i = 0; i < this->NumCols; i ++) {
    if (!TheNonPivotPoints.selected[i]) {
      output(i, 0) = 0;
      for (int j = 0; j < TheNonPivotPoints.CardinalitySelection; j ++) {
        output(i, 0) -= this->elements[RowCounter][TheNonPivotPoints.elements[j]];
      }
      RowCounter ++;
    } else {
      output(i, 0) = 1;
    }
  }
}

template<typename coefficient>
void Matrix<coefficient>::NonPivotPointsToEigenVector(
  Selection& TheNonPivotPoints,
  Vector<coefficient>& output,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  int RowCounter = 0;
  output.SetSize(this->NumCols);
  for (int i = 0; i < this->NumCols; i ++) {
    if (!TheNonPivotPoints.selected[i]) {
      output[i] = theRingZero;
      for (int j = 0; j < TheNonPivotPoints.CardinalitySelection; j ++) {
        output[i] -= this->elements[RowCounter][TheNonPivotPoints.elements[j]];
      }
      RowCounter ++;
    } else {
      output[i] = theRingUnit;
    }
  }
}

template <class Object>
void List<Object>::WriteToFile(std::fstream& output, int UpperLimitForDebugPurposes) const {
  int NumWritten = this->size;
  if (UpperLimitForDebugPurposes > 0 && UpperLimitForDebugPurposes < NumWritten) {
    NumWritten = UpperLimitForDebugPurposes;
  }
  output << XML::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  output << "size: " << NumWritten << "\n";
  for (int i = 0; i < NumWritten; i ++) {
    this->TheObjects[i].WriteToFile(output);
    output << "\n";
  }
  output << XML::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName()) << "\n";
}

template<class coefficient>
class CompleX {
  static bool flagEqualityIsApproximate;
  static double EqualityPrecision;
  public:
  coefficient Im;
  coefficient Re;
  std::string ToString(FormatExpressions* theFormat = nullptr) {
    (void) theFormat; //taking care of unused parameters
    std::stringstream tempStream;
    tempStream << *this;
    return tempStream.str();
  }
  friend std::iostream& operator<<(std::iostream& output, const CompleX<coefficient>& input) {
    output << input.Re << "+ i" << input.Im;
    return output;
  }
  void operator*=(const CompleX<coefficient>& other) {
    CompleX Accum;
    Accum.Re = this->Re * other.Re - this->Im * other.Im;
    Accum.Im = this->Re * other.Im + this->Im * other.Re;
    this->operator=(Accum);
  }
  void operator=(const CompleX<coefficient>& other) {
    this->Re = other.Re;
    this->Im = other.Im;
  }
  void operator+=(const CompleX<coefficient>& other) {
    this->Re += other.Re;
    this->Im += other.Im;
  }
  void operator-=(const CompleX<coefficient>& other) {
    this->Re -= other.Re;
    this->Im -= other.Im;
  }
  void operator=(int other) {
    this->Re = other;
    this->Im = 0;
  }
  bool operator==(int other) const {
    CompleX<coefficient> otherComplex;
    otherComplex = other;
    return *this == otherComplex;
  }
  bool operator==(CompleX<coefficient>& other) const {
    CompleX<coefficient> difference;
    difference = *this;
    difference -= other;
    return difference.IsEqualToZero();
  }
  void operator=(double other) {
    this->Re = other;
    this->Im = 0;
  }
  void Invert() {
    coefficient numerator;
    numerator = this->Re * this->Re + this->Im * this->Im;
    this->Re /= numerator;
    numerator *= - 1;
    this->Im /= numerator;
  }
  bool IsEqualToZero() const {
    if (!CompleX<coefficient>::flagEqualityIsApproximate) {
      return this->Im == 0 && this->Re == 0;
    } else {
      return
      this->Im<CompleX<coefficient>::EqualityPrecision && - this->Im<CompleX<coefficient>::EqualityPrecision &&
      this->Re<CompleX<coefficient>::EqualityPrecision && - this->Re<CompleX<coefficient>::EqualityPrecision;
    }
  }
  void Minus() {
    this->Im = - this->Im;
    this->Re = - this->Re;
  }
  bool NeedsParenthesisForMultiplication() {
    if (this->Re == 0 && this->Im >= 0) {
      return false;
    }
    if (this->Im == 0 && this->Re >= 0) {
      return false;
    }
    return true;
  }
  CompleX(){}
  CompleX(int other) {
    this->operator=(other);
  }
  CompleX(double other) {
    this->operator=(other);
  }
};

template<class coefficient>
class ElementSemisimpleLieAlgebra : public MonomialCollection<ChevalleyGenerator, coefficient> {
public:
  bool CheckConsistency() const {
    if (this->size() == 0) {
      return true;
    }
    SemisimpleLieAlgebra* owner = (*this)[0].owner;
    for (int i = 1; i < this->size(); i ++) {
      if (owner != (*this)[i].owner) {
        crash << "This is a programming error: ElementSemisimpleLieAlgebra "
        << "contains Chevalley generators with different owners. " << crash;
      }
    }
    return true;
  }
  bool NeedsParenthesisForMultiplication() const;
  Vector<coefficient> GetCartanPart() const;
  void MakeGGenerator(const Vector<Rational>& theRoot, SemisimpleLieAlgebra& inputOwner);
  bool IsElementCartan() const;
  void MakeHgenerator(const Vector<coefficient>& theH, SemisimpleLieAlgebra& inputOwners);
  void MakeGenerator(int generatorIndex, SemisimpleLieAlgebra& inputOwner);
  void ElementToVectorNegativeRootSpacesFirst(Vector<coefficient>& output) const;
  void AssignVectorNegRootSpacesCartanPosRootSpaces(const Vector<coefficient>& input, SemisimpleLieAlgebra& owner);
  bool GetCoordsInBasis(
    const List<ElementSemisimpleLieAlgebra<coefficient> >& theBasis, Vector<coefficient>& output
  ) const;
  SemisimpleLieAlgebra* GetOwner() const {
    this->CheckConsistency();
    if (this->size() == 0) {
      return nullptr;
    }
    return (*this)[0].owner;
  }
  bool GetCoordsInBasis(
    const List<ElementSemisimpleLieAlgebra>& theBasis, Vector<RationalFunctionOld>& output
  ) const {
    Vector<Rational> tempVect;
    if (!this->GetCoordsInBasis(theBasis, tempVect)) {
      return false;
    }
    output.SetSize(tempVect.size);
    return true;
  }
  void MultiplyByRational(SemisimpleLieAlgebra& owner, const Rational& theNumber);
  static void GetBasisFromSpanOfElements(
    List<ElementSemisimpleLieAlgebra>& theElements,
    List<ElementSemisimpleLieAlgebra>& outputTheBasis
  );
  void ActOnMe(const ElementSemisimpleLieAlgebra& theElt, ElementSemisimpleLieAlgebra& output, SemisimpleLieAlgebra& owner);
  void ActOnMe(
    const ElementSemisimpleLieAlgebra& theElt,
    ElementSemisimpleLieAlgebra& output,
    SemisimpleLieAlgebra& owner,
    const RationalFunctionOld& theRingUnit,
    const RationalFunctionOld& theRingZero
  );
  bool IsACoeffOneChevalleyGenerator();
  bool IsProportionalTo(const ElementSemisimpleLieAlgebra& other) const {
    Vector<Rational> tempRoot1, tempRoot2;
    this->ElementToVectorNegativeRootSpacesFirst(tempRoot1);
    other.ElementToVectorNegativeRootSpacesFirst(tempRoot2);
    return tempRoot1.IsProportionalTo(tempRoot2);
  }
  bool IsProportionalTo(const ElementSemisimpleLieAlgebra& other, Rational& outputTimesMeEqualsInput) const {
    Vector<Rational> tempRoot1, tempRoot2;
    this->ElementToVectorNegativeRootSpacesFirst(tempRoot1);
    other.ElementToVectorNegativeRootSpacesFirst(tempRoot2);
    return tempRoot1.IsProportionalTo(tempRoot2, outputTimesMeEqualsInput);
  }
  bool MustUseBracketsWhenDisplayingMeRaisedToPower();
  unsigned int HashFunction() const {
    return this->indexOfOwnerAlgebra * SomeRandomPrimes[0] +
    this->::MonomialCollection<ChevalleyGenerator, Rational>::HashFunction() * SomeRandomPrimes[1];
  }
  static unsigned int HashFunction(const ElementSemisimpleLieAlgebra& input) {
    return input.HashFunction();
  }
  template<class otherElement>
  void operator=(const otherElement& other) {
    this->::MonomialCollection<ChevalleyGenerator, coefficient>::operator=(other);
  }
  Vector<Rational> GetRootIMustBeWeight() const;
};

template <class coefficient>
int Matrix<coefficient>::FindPositiveLCMCoefficientDenominatorsTruncated() {
  int result = 1;
  for (int i = 0; i < this->NumRows; i ++) {
    for (int j = 0; j < this->NumCols; j ++) {
      result *= this->elements[i][j].DenShort / Rational::gcdSigned(result, this->elements[i][j].DenShort);
    }
  }
  return result;
}

template <class coefficient>
LargeIntUnsigned Matrix<coefficient>::FindPositiveLCMCoefficientDenominators() {
  LargeIntUnsigned result = 1;
  for (int i = 0; i < this->NumRows; i ++) {
    for (int j = 0; j < this->NumCols; j ++) {
      result = LargeIntUnsigned::lcm(result, (*this)(i, j).GetDenominator());
    }
  }
  return result;
}

template <class coefficient>
void Matrix<coefficient>::GetMatrixIntWithDen(Matrix<LargeInt>& outputMat, LargeIntUnsigned& outputDen) {
  outputDen = this->FindPositiveLCMCoefficientDenominators();
  outputMat.init(this->NumRows, this->NumCols);
  Rational tempRat;
  for (int i = 0; i < this->NumRows; i ++) {
    for (int j = 0; j < this->NumCols; j ++) {
      tempRat = this->elements[i][j] * outputDen;
      outputMat(i, j) = tempRat.GetDenominator();
    }
  }
}

template <class coefficient>
int Matrix<coefficient>::FindPositiveGCDCoefficientNumeratorsTruncated() {
  int result = 1;
  for (int i = 0; i < this->NumRows; i ++) {
    for (int j = 0; j < this->NumCols; j ++) {
      if (this->elements[i][j].NumShort != 0) {
        result = Rational::gcdSigned(result, this->elements[i][j].NumShort);
      }
    }
  }
  if (result == 0) {
    crash << "Crash in Matrix::FindPositiveGCDCoefficientNumeratorsTruncated" << crash;
  }
  if (result < 0) {
    result = - result;
  }
  return result;
}

template <class coefficient>
void Matrix<coefficient>::ScaleToIntegralForMinRationalHeightNoSignChange() {
  Rational tempRat;
  tempRat.AssignNumeratorAndDenominator(
    this->FindPositiveLCMCoefficientDenominatorsTruncated(), this->FindPositiveGCDCoefficientNumeratorsTruncated()
  );
  *this *= tempRat;
}

template <class coefficient>
coefficient Matrix<coefficient> ::GetDeterminant() {
  Matrix<coefficient> tempMat = *this;
  coefficient result;
  tempMat.ComputeDeterminantOverwriteMatrix(result);
  return result;
}

template <class coefficient>
coefficient Matrix<coefficient>::GetTrace() const {
  if (this->NumCols != this->NumRows) {
    crash << "This is either programming error, a mathematical error, "
    << "or requires a more general definition of trace. Requesting the trace of "
    << "a non-square matrix of " << this->NumRows << " rows and "
    << this->NumCols << " columns is not allowed. "
    << crash;
  }
  coefficient acc = 0;
  for (int i = 0; i < this->NumCols; i ++) {
    acc += this->elements[i][i];
  }
  return acc;
}

template <class coefficient>
Matrix<coefficient> Matrix<coefficient>::operator*(const Matrix<coefficient>& right) const {
  Matrix<coefficient> tempMat;
  tempMat = right;
  tempMat.MultiplyOnTheLeft(*this);
  return tempMat;
}

template <class coefficient>
Vector<coefficient> Matrix<coefficient>::operator*(const Vector<coefficient>& v) const {
  if (v.size != NumCols) {
    crash << "matrix application mismatch: matrix with" << NumCols
    << "columns attempted to multiply vector of length" << v.size << crash;
  }
  Vector<coefficient> out;
  out.MakeZero(NumRows);
  for (int i = 0; i < NumRows; i ++) {
    for (int j = 0; j < NumCols; j ++) {
      out[i] += elements[i][j] * v[j];
    }
  }
  return out;
}

template <class coefficient>
void Matrix<coefficient>::AssignMatrixIntWithDen(Matrix<LargeInt>& theMat, const LargeIntUnsigned& Den) {
  this->init(theMat.NumRows, theMat.NumCols);
  for (int i = 0; i < this->NumRows; i ++) {
    for (int j = 0; j < this->NumCols; j ++) {
      this->elements[i][j] = theMat.elements[i][j];
      this->elements[i][j] /= Den;
    }
  }
}

template <class coefficient>
void Polynomial<coefficient>::MakePolyFromDirectionAndNormal(
  Vector<coefficient>& direction, Vector<coefficient>& normal, coefficient& Correction
) {
  Rational tempRat2 = Vector<coefficient>::ScalarEuclidean(direction, normal);
  this->MakeZero();
  MonomialP tempM;
  for (int i = 0; i < direction.size; i ++) {
    tempM.MakeEi(i);
    this->AddMonomial(tempM, normal.TheObjects[i] / tempRat2);
  }
  *this += Correction;
}

template <class coefficient>
bool Vectors<coefficient>::GetNormalSeparatingCones(
  List<Vector<coefficient> >& coneStrictlyPositiveCoeffs,
  List<Vector<coefficient> >& coneNonNegativeCoeffs,
  Vector<coefficient>& outputNormal
) {
  Matrix<Rational> matA;
  Matrix<Rational> matb;
  Vector<Rational> matX;
  int theDimension = coneStrictlyPositiveCoeffs[0].size;
  if (coneStrictlyPositiveCoeffs.size == 0) {
    if (coneNonNegativeCoeffs.size > 0) {
      outputNormal.MakeZero(coneNonNegativeCoeffs[0].size);
    }
    return true;
  }
  int numRows = coneStrictlyPositiveCoeffs.size + coneNonNegativeCoeffs.size;
  matA.init(numRows, theDimension * 2 + numRows);
  matA.MakeZero();
  matb.init(numRows, 1);
  matb.MakeZero();
  for (int i = 0; i < coneStrictlyPositiveCoeffs.size; i ++) {
    for (int k = 0; k < theDimension; k ++) {
      matA.elements[i][k].Assign(coneStrictlyPositiveCoeffs.TheObjects[i].TheObjects[k]);
      matA.elements[i][k + theDimension].Assign(matA.elements[i][k]);
      matA.elements[i][k + theDimension].Minus();
    }
    matb.elements[i][0].MakeOne();
    matA.elements[i][theDimension * 2 + i].MakeMOne();
  }
  for (int i = 0; i < coneNonNegativeCoeffs.size; i ++) {
    int currentRow = i + coneStrictlyPositiveCoeffs.size;
    for (int k = 0; k < theDimension; k ++) {
      matA.elements[currentRow][k].Assign(coneNonNegativeCoeffs.TheObjects[i].TheObjects[k]);
      matA.elements[currentRow][k + theDimension].Assign(matA.elements[currentRow][k]);
      matA.elements[currentRow][k + theDimension].Minus();
    }
    matA.elements[currentRow][2 * theDimension + currentRow].MakeOne();
  }
  //matA.ComputeDebugString();
  //matb.ComputeDebugString();
  //matX.ComputeDebugString();
  bool result = Matrix<Rational>::SystemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(
    matA, matb, &matX
  );
  //matA.ComputeDebugString();
  //matb.ComputeDebugString();
  //matX.ComputeDebugString();
  outputNormal.MakeZero(theDimension);
  for (int i = 0; i < theDimension; i ++) {
    outputNormal[i] = matX[i] - matX[i + theDimension];
  }
  if (result) {
    Rational tempRat;
    for (int i = 0; i < coneStrictlyPositiveCoeffs.size; i ++) {
      coneStrictlyPositiveCoeffs[i].ScalarEuclidean(outputNormal, tempRat);
      if (!tempRat.IsPositive()) {
        crash << "Unexpected non-positive value. " << crash;
      }
    }
    for (int i = 0; i < coneNonNegativeCoeffs.size; i ++) {
      coneNonNegativeCoeffs[i].ScalarEuclidean(outputNormal, tempRat);
      if (!tempRat.IsNonPositive()) {
        crash << "Unexpected positive value" << crash;
      }
    }
  }
//  outputNormal.ComputeDebugString();
  return result;
}

template<class coefficient>
void Matrix<coefficient>::ComputePotentialChangeGradient(
  Matrix<coefficient>& matA,
  Selection& BaseVariables,
  int NumTrueVariables,
  int ColumnIndex,
  Rational& outputChangeGradient,
  bool& hasAPotentialLeavingVariable
) {
  hasAPotentialLeavingVariable = false;
  outputChangeGradient.MakeZero();
  for (int j = 0; j < matA.NumRows; j ++) {
    if (BaseVariables.elements[j] >= NumTrueVariables) {
      outputChangeGradient += matA.elements[j][ColumnIndex];
    }
    hasAPotentialLeavingVariable = hasAPotentialLeavingVariable || matA.elements[j][ColumnIndex].IsPositive();
  }
  if (ColumnIndex >= NumTrueVariables) {
    outputChangeGradient -= 1;
  }
}

template<class coefficient>
void Matrix<coefficient>::GetMaxMovementAndLeavingVariableRow(
  Rational& maxMovement,
  int& LeavingVariableRow,
  int EnteringVariable,
  Matrix<coefficient>& tempMatA,
  Vector<coefficient>& inputVectorX,
  Selection& BaseVariables
) {
  LeavingVariableRow = - 1;
  maxMovement.MakeZero();
  for (int i = 0; i < tempMatA.NumRows; i ++) {
    Rational tempRat;
    tempRat.Assign(tempMatA.elements[i][EnteringVariable]);
    if (tempRat.IsPositive()) {
      tempRat.Invert();
      tempRat.MultiplyBy(inputVectorX[BaseVariables.elements[i]]);
      if (maxMovement.IsGreaterThan(tempRat)|| (LeavingVariableRow == - 1)) {
        maxMovement.Assign(tempRat);
        LeavingVariableRow = i;
      }
    }
  }
}

template <typename coefficient>
void Matrix<coefficient>::ActOnMonomialAsDifferentialOperator(
  const MonomialP& input, Polynomial<Rational>& output
) {
  if (this->NumRows != this->NumCols) {
    crash << "Matrix not square as expected. " << crash;
  }
  MonomialP tempMon;
  output.MakeZero();
  Rational coeff;
  for (int i = 0; i < this->NumRows; i ++) {
    for (int j = 0; j < this->NumCols; j ++) {
      tempMon = input;
      coeff = tempMon(j);
      coeff *= this->elements[i][j];
      tempMon[j] -= 1;
      tempMon[i] += 1;
      output.AddMonomial(tempMon, coeff);
    }
  }
}

template <typename coefficient>
void Matrix<coefficient>::GetZeroEigenSpaceModifyMe(List<Vector<coefficient> >& output) {
  if (this->NumRows == 0) {
    output.SetSize(this->NumCols);
    for (int i = 0; i < this->NumCols; i ++) {
      output[i].MakeEi(this->NumCols, i);
    }
    return;
  }
  Selection nonPivotPts;
  this->GaussianEliminationByRows(0, &nonPivotPts);
  output.SetSize(nonPivotPts.CardinalitySelection);
  for (int i = 0; i < nonPivotPts.CardinalitySelection; i ++) {
    int currentPivotIndex = nonPivotPts.elements[i];
    output[i].MakeEi(this->NumCols, currentPivotIndex);
    int rowCounter = 0;
    for (int j = 0; j < this->NumCols; j ++) {
      if (!nonPivotPts.selected[j]) {
        output[i][j] -= this->elements[rowCounter][currentPivotIndex];
        rowCounter ++;
      }
    }
  }
}

template <class coefficient>
std::string Vectors<coefficient>::ToString(FormatExpressions* theFormat) const {
  std::stringstream out;
  std::string tempS;
  bool useLaTeX = false;
  bool useHtml = false;
  bool makeTable = false;
  if (theFormat != nullptr) {
    useLaTeX = theFormat->flagUseLatex;
    useHtml = theFormat->flagUseHTML;
  }
  if (!useLaTeX && !useHtml) {
    out << this->size << " vectors: \n";
  }
  if (useLaTeX && makeTable) {
    out << "\\begin{tabular}{c}";
  }
  if (useHtml && makeTable) {
    out << "<table>";
  }
  for (int i = 0; i < this->size; i ++) {
    tempS = this->TheObjects[i].ToString(theFormat);
    if (useHtml && makeTable) {
      out << "<tr><td>";
    }
    out << tempS;
    if (!makeTable && i != this->size - 1) {
      out << ", ";
    }
    if (useLaTeX && makeTable) {
      out << "\\\\\n";
    }
    if (useHtml && makeTable) {
      out << "</td></tr>";
    }
  }
  if (useHtml && makeTable) {
    out << "</table>";
  }
  if (useLaTeX && makeTable) {
    out << "\\end{tabular}";
  }
  return out.str();
}

template <class Object>
void List<Object>::SubSelection(const Selection& theSelection, List<Object>& output) {
  if (&output == this) {
    List<Object> thisCopy = *this;
    thisCopy.SubSelection(theSelection, output);
    return;
  }
  output.SetSize(theSelection.CardinalitySelection);
  for (int i = 0; i < theSelection.CardinalitySelection; i ++) {
    output[i] = (*this)[theSelection.elements[i]];
  }
}

template <class Object>
void List<Object>::IntersectWith(const List<Object>& other, List<Object>& output) const {
  if (&output == &other || this == &output) {
    List<Object> l1 = *this;
    List<Object> l2 = other;
    l1.IntersectWith(l2, output);
    return;
  }
  HashedList<Object> tempList;
  tempList = *this;
  output.SetSize(0);
  for (int i = 0; i < other.size; i ++) {
    if (tempList.Contains(other[i])) {
      output.AddOnTop(other[i]);
    }
  }
}

template <class coefficient>
std::string Vector<coefficient>::ToStringLetterFormat(
  const std::string& inputLetter, FormatExpressions* theFormat, bool DontIncludeLastVar
) const {
  if (this->IsEqualToZero()) {
    return "0";
  }
  std::stringstream out;
  std::string tempS;
  bool found = false;
  int NumVars = DontIncludeLastVar ? this->size - 1 : this->size;
  for (int i = 0; i < NumVars; i ++) {
    if (!this->TheObjects[i].IsEqualToZero()) {
      tempS = this->TheObjects[i].ToString(theFormat);
      if (this->TheObjects[i].NeedsParenthesisForMultiplication()) {
        tempS = "(" + tempS + ")";
      }
      if (tempS == "1") {
        tempS = "";
      }
      if (tempS == "- 1" || tempS == "-1")
        tempS = "-";
      if (found) {
        if (tempS.size() > 0) {
          if (tempS[0] != '-') {
            out << "+";
          }
        } else {
          out << "+";
        }
      }
      found = true;
      out << tempS;
      if (theFormat != nullptr) {
        if (theFormat->vectorSpaceEiBasisNames.size > i) {
          out << theFormat->vectorSpaceEiBasisNames[i];
          continue;
        }
      }
      out << inputLetter << "_{" << i + 1 << "}";
    }
  }
  return out.str();
}

template <class coefficient, unsigned int inputHashFunction(const coefficient&)>
void MonomialTensor<coefficient, inputHashFunction>::MultiplyByGeneratorPowerOnTheLeft(
  int theGeneratorIndexStandsOnTheLeft, const coefficient& thePower
) {
  if (thePower == 0) {
    return;
  }
  List<int> newGeneratorIndices;
  List<coefficient> newPowers;
  newGeneratorIndices.SetExpectedSize(this->generatorsIndices.size + 1);
  newPowers.SetExpectedSize(this->generatorsIndices.size + 1);
  newGeneratorIndices.AddOnTop(theGeneratorIndexStandsOnTheLeft);
  newPowers.AddOnTop(thePower);
  newGeneratorIndices.AddListOnTop(this->generatorsIndices);
  newPowers.AddListOnTop(this->Powers);
  this->generatorsIndices = newGeneratorIndices;
  this->Powers = newPowers;
  this->SimplifyEqualConsecutiveGenerators(0);
}

template <class coefficient, unsigned int inputHashFunction(const coefficient&)>
void MonomialTensor<coefficient, inputHashFunction>::MultiplyByGeneratorPowerOnTheRight(
  int theGeneratorIndex, const coefficient& thePower
) {
  if (thePower == 0) {
    return;
  }
  if (this->generatorsIndices.size > 0) {
    if (*this->generatorsIndices.LastObject() == theGeneratorIndex) {
      (*this->Powers.LastObject()) += thePower;
      return;
    }
  }
  this->Powers.AddOnTop(thePower);
  this->generatorsIndices.AddOnTop(theGeneratorIndex);
}

template <class coefficient, unsigned int inputHashFunction(const coefficient&)>
std::string MonomialTensor<coefficient, inputHashFunction>::ToString(FormatExpressions* theFormat) const {
  if (this->generatorsIndices.size == 0) {
    return "1";
  }
  std::string theLetter = theFormat == nullptr ?  "g" : theFormat->chevalleyGgeneratorLetter;
  std::string letters = "abcdefghijklmnopqrstuvwxyz";
  std::string exponents[10] = {"⁰", "¹", "²", "³", "⁴", "⁵", "⁶", "⁷", "⁸", "⁹"};
  std::stringstream out;
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    if (static_cast<unsigned>(generatorsIndices[i]) < letters.size()) {
      out << letters[this->generatorsIndices[i]];
    } else {
      out << theLetter << "_{" << this->generatorsIndices[i] << "}";
    }
    if (!(this->Powers[i] == 1)) {
      if (this->Powers[i] == 2) {
        out << exponents[2];
      } else if (this->Powers[i] == 3) {
        out << exponents[3];
      } else if (this->Powers[i] == 4) {
        out << exponents[4];
      } else if (this->Powers[i] == 5) {
        out << exponents[5];
      } else if (this->Powers[i] == 6) {
        out << exponents[6];
      } else if (this->Powers[i] == 7) {
        out << exponents[7];
      } else if (this->Powers[i] == 8) {
        out << exponents[8];
      } else if (this->Powers[i] == 9) {
        out << exponents[9];
      } else {
        out << "^{" << this->Powers[i] << "}";
      }
    }
  }
  return out.str();
}

template <class coefficient, unsigned int inputHashFunction(const coefficient&)>
bool MonomialTensor<coefficient, inputHashFunction>::SimplifyEqualConsecutiveGenerators(int lowestNonReducedIndex) {
  if (this->generatorsIndices.size < 1) {
    return false;
  }
  if (lowestNonReducedIndex < 0) {
    lowestNonReducedIndex = 0;
  }
  bool result = false;
  for (int next = lowestNonReducedIndex + 1; next < this->generatorsIndices.size; next ++) {
    if (this->generatorsIndices[lowestNonReducedIndex] == this->generatorsIndices[next]) {
      result = true;
      this->Powers[lowestNonReducedIndex] += this->Powers[next];
    } else {
      lowestNonReducedIndex ++;
      this->Powers[lowestNonReducedIndex] = this->Powers[next];
      this->generatorsIndices[lowestNonReducedIndex] = this->generatorsIndices[next];
    }
  }
  this->generatorsIndices.SetSize(lowestNonReducedIndex + 1);
  this->Powers.SetSize(lowestNonReducedIndex + 1);
  return result;
}

template <typename coefficient>
std::string Matrix<coefficient>::ToStringLatex(FormatExpressions* theFormat) const {
  FormatExpressions formatCopy;
  if (theFormat != nullptr) {
    formatCopy = *theFormat;
  }
  formatCopy.flagUseLatex = true;
  formatCopy.flagUseHTML = false;
  return this->ToString(&formatCopy);
}

template <typename coefficient>
std::string Matrix<coefficient>::ToStringSystemLatex(
  Matrix<coefficient>* constTerms, FormatExpressions* theFormat
) const {
  std::stringstream out;
  bool constTermsAreGood =
  (constTerms == 0) ? false : (constTerms->NumRows == this->NumRows && constTerms->NumCols > 0);
  out << "\\begin{array}{l";
  for (int j = 0; j < this->NumCols; j ++) {
    out << "l";
  }
  out << "}";
  std::string currentEntry;
  Polynomial<coefficient> theMon;
  for (int i = 0; i < this->NumRows; i ++) {
    bool foundNonZeroEntry = false;
    for (int j = 0; j < this->NumCols; j ++) {
      if (!((*this)(i, j) == 0)) {
        theMon.MakeMonomiaL(j, 1, (*this)(i, j));
        currentEntry = theMon.ToString(theFormat);
        if (currentEntry == "") {
          crash << "Empty strings not allowed as result of ToString() function call. " << crash;
        }
        if (currentEntry[0] != '-' && foundNonZeroEntry) {
          out << "+";
        }
        out << currentEntry;
        foundNonZeroEntry = true;
      }
      if (j == this->NumCols - 1 && !foundNonZeroEntry) {
        out << "0";
      }
      out << " & ";
    }
    out << "=";
    if (!constTermsAreGood) {
      out << "0";
    } else {
      out << (*constTerms)(i, 0).ToString(theFormat);
    }
    out << "\\\\";
  }
  out << "\\end{array}";
  return out.str();
}

template <typename coefficient>
std::string Matrix<coefficient>::ToString(FormatExpressions* theFormat) const {
  std::stringstream out;
  std::string tempS;
  bool useHtml    = (theFormat == nullptr) ? true : theFormat->flagUseHTML;
  bool useLatex   = (theFormat == nullptr) ? false : theFormat->flagUseLatex;
  bool usePmatrix = (theFormat == nullptr) ? true : theFormat->flagUsePmatrix;
  if (useHtml) {
    out << "<table>";
  }
  if (useLatex) {
    int verticalLineIndex = theFormat == nullptr ? - 1 : theFormat->MatrixColumnVerticalLineIndex;
    if (usePmatrix) {
      out << "\\begin{pmatrix}";
    } else {
      out << "\\left(";
      out << "\\begin{array}{";
      for (int j = 0; j < this->NumCols; j ++) {
        out << "c";
        if (verticalLineIndex == j) {
          out << "|";
        }
      }
      out << "}";
    }
  }
  for (int i = 0; i < this->NumRows; i ++) {
    if (useHtml) {
      out << "<tr>";
    }
    for (int j = 0; j < this->NumCols; j ++) {
      tempS = (*this)(i, j).ToString(theFormat);
      if (useHtml) {
        out << "<td>";
      }
      out << tempS;
      if (useLatex) {
        if (j != this->NumCols - 1) {
          out << " & ";
        } else {
          out << "\\\\";
        }
      }
      if (useHtml) {
        out << "</td>";
      }
      if (!useHtml && !useLatex) {
        out << ", \t ";
      }
    }
    if (useHtml) {
      out << "</tr>";
    }
    out << "\n";
  }
  if (useHtml) {
    out << "</table>";
  }
  if (useLatex) {
    if (usePmatrix) {
      out << "\\end{pmatrix}";
    } else {
      out << "\\end{array}";
      out << "\\right)";
    }
  }
  return out.str();
}

template <typename coefficient>
std::string Matrix<coefficient>::ToStringOneLine(bool jsonFormat) const {
  std::stringstream out;
  out << "[";
  for (int i = 0; i < this->NumRows; i ++) {
    if (jsonFormat) {
      out << "[";
    }
    for (int j = 0; j < this->NumCols; j ++) {
      out << this->elements[i][j];
      if (j != this->NumCols - 1) {
        out << ", ";
      }
    }
    if (jsonFormat) {
      out << "]";
    }
    if (i != this->NumRows - 1) {
      if (jsonFormat) {
        out << ", ";
      } else {
        out << "; ";
      }
    }
  }
  out << "]";
  return out.str();
}

template <typename coefficient>
std::string Matrix<coefficient>::ToStringPlainText(bool jsonFormat) const {
  // I'm pretty sure C++ supports having a block of memory with
  // multiple indices into it.  oh well.
  List<List<std::string> > element_strings;
  element_strings.SetSize(this->NumRows);
  int cols_per_elt = 0;
  for (int i = 0; i < this->NumRows; i ++) {
    element_strings[i].SetSize(this->NumCols);
    for (int j = 0; j < this->NumCols; j ++) {
      std::stringstream ss;
      ss << this->elements[i][j];
      element_strings[i][j] = ss.str();
      int sl = element_strings[i][j].length();
      if (sl > cols_per_elt) {
        cols_per_elt = sl;
      }
    }
  }
  std::stringstream out;
  for (int i = 0; i < this->NumRows; i ++) {
    if (jsonFormat) {
      if (i == 0) {
        out << '[';
      } else {
        out << ' ';
      }
    }
    out << '[';
    for (int j = 0; j < this->NumCols; j ++) {
      int sl = element_strings[i][j].length();
      int pad = cols_per_elt - sl;
      for (int pi = 0; pi < pad; pi ++) {
        out << ' ';
      }
      out << element_strings[i][j];
      if (j != this->NumCols - 1) {
        if (jsonFormat) {
          out << ',';
        }
        out << ' ';
      }
    }
    out << ']';
    if (i != this->NumRows - 1) {
      out << '\n';
    } else {
      if (jsonFormat) {
        out << ']';
      }
    }
  }
  return out.str();
}

template <class templateMonomial, class coefficient>
std::string MonomialCollection<templateMonomial, coefficient>::GetBlendCoeffAndMon(
  const templateMonomial& inputMon,
  coefficient& inputCoeff,
  bool addPlusToFront,
  FormatExpressions* theFormat
) {
  std::stringstream out;
  std::string coeffStr = inputCoeff.ToString(theFormat);
  if (inputMon.IsConstant()) {
    if (coeffStr[0] != '-' && addPlusToFront) {
      out << "+" << coeffStr;
    } else {
      out << coeffStr;
    }
    return out.str();
  }
  std::string monString = inputMon.ToString(theFormat);
  if (coeffStr == "1") {
    if (addPlusToFront) {
      out << "+";
    }
    out << monString;
    return out.str();
  }
  if (coeffStr == "- 1" || coeffStr == "-1") {
    out << "-" << monString;
    return out.str();
  }
  if (coeffStr[0] != '-' && addPlusToFront) {
    out << "+" << coeffStr << monString;
  } else {
    out << coeffStr << monString;
  }
  return out.str();
}

template <class templateMonomial, class coefficient>
std::string MonomialCollection<templateMonomial, coefficient>::ToString(FormatExpressions* theFormat) const {
  if (this->size() == 0) {
    return "0";
  }
  MacroRegisterFunctionWithName("MonomialCollection::ToString");
  std::stringstream out;
  std::string tempS1, tempS2;
  List<templateMonomial> sortedMons;
  sortedMons = this->theMonomials;
  //If this line fails to link, you must do the following.
  //You need to implement FormatExpressions::GetMonOrder<templateMonomial>()
  // and make it return 0 (or a pointer to a monomial order, should you
  //wish to use a custom one.
  typename List<templateMonomial>::OrderLeftGreaterThanRight
  theOrder = (theFormat == nullptr) ? 0 : theFormat->GetMonOrder<templateMonomial>();
  sortedMons.QuickSortDescending(theOrder);
//  out << "(hash: " << this->HashFunction() << ")";
  int cutOffCounter = 0;
  bool useCustomPlus = false;
  bool useCustomTimes = false;
  int MaxLineLength = theFormat == nullptr ? 200 : theFormat->MaxLineLength;
  int NumAmpersandsPerNewLineForLaTeX = (theFormat == nullptr) ? 1 : theFormat->NumAmpersandsPerNewLineForLaTeX;
  bool flagUseLaTeX = (theFormat == nullptr) ? false : theFormat->flagUseLatex;
  bool flagUseHTML = (theFormat == nullptr) ? false : theFormat->flagUseHTML;
  std::string oldCustomTimes = "";
  if (theFormat != nullptr) {
    useCustomPlus = (theFormat->CustomPlusSign != "");
    useCustomTimes = (theFormat->CustomCoeffMonSeparator != "");
    if (theFormat->flagPassCustomCoeffMonSeparatorToCoeffs == false) {
      oldCustomTimes = theFormat->CustomCoeffMonSeparator;
      theFormat->CustomCoeffMonSeparator = "";
    }
  }
  for (int i = 0; i < sortedMons.size; i ++) {
    templateMonomial& currentMon = sortedMons[i];
    coefficient& currentCoeff = this->theCoeffs[this->theMonomials.GetIndex(currentMon)];
    if (currentCoeff.NeedsParenthesisForMultiplication()) {
      tempS1 = "(" + currentCoeff.ToString(theFormat) + ")";
    } else {
      tempS1 = currentCoeff.ToString(theFormat);
    }
    tempS2 = currentMon.ToString(theFormat);
    if (tempS2 != "") {
      bool useFracSpecial = false;
      if (theFormat != nullptr) {
        if (theFormat->flagUseFrac && theFormat->flagSuppressOneIn1overXtimesY) {
          useFracSpecial = true;
        }
      }
      if (useFracSpecial) {
        std::string tempS3;
        if (StringRoutines::StringBeginsWith(tempS1, "\\frac{1}", &tempS3)) {
          tempS1 = "\\frac{" + tempS2 + "}" + tempS3;
        } else if (StringRoutines::StringBeginsWith(tempS1, "-\\frac{1}", &tempS3)) {
          tempS1 = "-\\frac{" + tempS2 + "}" + tempS3;
        } else {
          useFracSpecial = false;
        }
      }
      if (!useFracSpecial) {
        if (!useCustomTimes) {
          if (tempS1 == "1" && tempS2 != "1") {
            tempS1 = "";
          }
          if ((tempS1 == "- 1" || tempS1 == "-1") && tempS2 != "1") {
            tempS1 = "-";
          }
          if (tempS2 != "1") {
            tempS1 += tempS2;
          }
        } else {
          tempS1 += oldCustomTimes;
          tempS1 += tempS2;
        }
      }
    }
    if (i > 0) {
      if (!useCustomPlus) {
        if (tempS1.size() > 0) {
          if (tempS1[0] != '-') {
            out << "+";
            cutOffCounter += 1;
          }
        } else {
          out << "+";
          cutOffCounter += 1;
        }
      } else {
        out << theFormat->CustomPlusSign;
      }
    }
    out << tempS1;
    cutOffCounter += tempS1.size();
    if (MaxLineLength > 0) {
      if (cutOffCounter > MaxLineLength) {
        cutOffCounter = 0;
        if (flagUseLaTeX && i != sortedMons.size - 1) {
          out << " \\\\";
          for (int k = 0; k < NumAmpersandsPerNewLineForLaTeX; k ++) {
            out << "&";
          }
          out << " ";
        }
        if (flagUseHTML && !flagUseLaTeX && (i != sortedMons.size - 1)) {
          out << " <br>";
        }
      }
    }
  }
  if (theFormat != nullptr) {
    theFormat->CustomCoeffMonSeparator = oldCustomTimes;
  }
  return out.str();
}

class Lattice {
  void TestGaussianEliminationEuclideanDomainRationals(Matrix<Rational>& output);
public:
  static const std::string GetXMLClassName() {
    return "Lattice";
  }
  Matrix<Rational> basisRationalForm;
  Matrix<LargeInt> basis;
  LargeIntUnsigned Den;
  int GetDim() const {
    return this->basis.NumCols;
  }
  int GetRank() const {
    return this->basis.NumRows;
  }
  void IntersectWith(const Lattice& other);
  bool FindOnePreimageInLatticeOf(
    const Matrix<Rational>& theLinearMap, const Vectors<Rational>& input, Vectors<Rational>& output
  );
  void IntersectWithPreimageOfLattice(
    const Matrix<Rational> & theLinearMap, const Lattice& other
  );
  void IntersectWithLineGivenBy(Vector<Rational>& inputLine, Vector<Rational>& outputGenerator);
  static bool GetClosestPointInDirectionOfTheNormalToAffineWallMovingIntegralStepsInDirection(
    Vector<Rational>& startingPoint,
    Vector<Rational>& theAffineHyperplane,
    Vector<Rational>& theDirection,
    Vector<Rational>& outputPoint
  );
  void GetDefaultFundamentalDomainInternalPoint(Vector<Rational>& output);
  bool GetInternalPointInConeForSomeFundamentalDomain(
    Vector<Rational>& output, Cone& coneContainingOutputPoint
  );
  void GetRootOnLatticeSmallestPositiveProportionalTo(
    Vector<Rational>& input, Vector<Rational>& output
  );
  void GetRougherLatticeFromAffineHyperplaneDirectionAndLattice(
    const Vector<Rational>& theDirection,
    Vector<Rational>& outputDirectionMultipleOnLattice,
    Vector<Rational>& theShift,
    Vector<Rational>& theAffineHyperplane,
    Vectors<Rational>& outputRepresentatives,
    Vectors<Rational>& movementInDirectionPerRepresentative,
    Lattice& outputRougherLattice
  );
  void ApplyLinearMap(Matrix<Rational>& theMap, Lattice& output);
  void IntersectWithBothOfMaxRank(const Lattice& other);
  void GetDualLattice(Lattice& output) const;
  bool IsInLattice(const Vector<Rational>& theVector) const {
    Vector<Rational> tempVect = theVector;
    if (!this->ReduceVector(tempVect)) {
      return false;
    }
    return tempVect.IsEqualToZero();
  }
  //returns false if the vector is not in the vector space spanned by the lattice
  bool ReduceVector(Vector<Rational>& theVector) const;
  //In the following two functions, the format of the matrix theSub of the substitution is as follows.
  //Let the ambient dimension be n, and the coordinates be x_1,..., x_n.
  //Let the new vector space be of dimension m, with coordinates y_1,..., y_m.
  //Then theSub is an n by m matrix, where the i^th row of the matrix gives the expression of x_i via the y_j's.
  //In addition, we require that n>=m (otherwise, in general, we do not expect to get a lattice).
  //For example, if we want to carry out the substitution
  //x_1=y_1+y_2, x_2=y_1-y_2, x_3=y_1, then
  //theSub should be initialized as:
  //1  1
  //1 - 1
  //1  0
  bool SubstitutionHomogeneous(const Matrix<Rational>& theSub);
  bool SubstitutionHomogeneous(const PolynomialSubstitution<Rational>& theSub);
  //the following function follows the same convention as the preceding except that we allow n<m. However,
  // in order to assure that the preimage of the lattice is a lattice,
  //we provide as input an ambient lattice in the new vector space of dimension m
  bool SubstitutionHomogeneous(const Matrix<Rational>& theSub, Lattice& resultIsSubsetOf);
  void Reduce();
  void IntersectWithLinearSubspaceSpannedBy(const Vectors<Rational>& theSubspaceBasis);
  void IntersectWithLinearSubspaceGivenByNormals(const Vectors<Rational>& theSubspaceNormals);
  void IntersectWithLinearSubspaceGivenByNormal(const Vector<Rational>& theNormal);
  static bool GetHomogeneousSubMatFromSubIgnoreConstantTerms(
    const PolynomialSubstitution<Rational>& theSub, Matrix<Rational>& output
  );
  //returning false means that the lattice given as rougher is not actually rougher than the current lattice
  //or that there are too many representatives
  bool GetAllRepresentatives(const Lattice& rougherLattice, Vectors<Rational>& output) const;
  bool GetAllRepresentativesProjectingDownTo(
    const Lattice& rougherLattice, Vectors<Rational>& startingShifts, Vectors<Rational>& output
  ) const;
  std::string ToString() const;
  bool operator==(const Lattice& other) {
    return this->basisRationalForm == other.basisRationalForm;
  }
  void operator=(const Lattice& other) {
    this->basis = other.basis;
    this->Den = other.Den;
    this->basisRationalForm = other.basisRationalForm;
  }
  void WriteToFile(std::fstream& output);
  bool ReadFromFile(std::fstream& input);
  void MakeZn(int theDim);
  void RefineByOtherLattice(const Lattice& other);
  void MakeFromRoots(const Vectors<Rational>& input);
  Lattice() {
  }
  Lattice(const Lattice& other) {
    this->operator=(other);
  }
  void MakeFromMat(const Matrix<Rational>& input);
};

class QuasiPolynomial {
public:
  static std::string GetXMLClassName() {
    return "Quasipolynomial";
  }
  int GetNumVars() const {
    return this->AmbientLatticeReduced.basis.NumRows;
  }
  Lattice AmbientLatticeReduced;
  Vectors<Rational> LatticeShifts;
  std::string DebugString;
  List<Polynomial<Rational> > valueOnEachLatticeShift;
  std::string ToString(bool useHtml, bool useLatex) {
    return this->ToString(useHtml, useLatex, nullptr);
  }
  std::string ToString(bool useHtml, bool useLatex, FormatExpressions* thePolyFormat);
  void ComputeDebugString() {
    this->DebugString = this->ToString(false, false);
  }
  Rational Evaluate(const Vector<Rational>& input);
  void AddLatticeShift(const Polynomial<Rational>& input, const Vector<Rational>& inputShift);
  void AddAssumingLatticeIsSame(const QuasiPolynomial& other);
  void MakeRougherLattice(const Lattice& latticeToRoughenBy);
  void MakeFromPolyShiftAndLattice(
    const Polynomial<Rational>& inputPoly, const MonomialP& theShift, const Lattice& theLattice
  );
  void MakeZeroLatTiceZn(int theDim);
  void MakeZeroOverLattice(Lattice& theLattice);
  bool IsEqualToZero() const {
    return this->valueOnEachLatticeShift.size == 0;
  }
  void Substitution(
    const Matrix<Rational>& mapFromNewSpaceToOldSpace,
    const Vector<Rational>& inputTranslationSubtractedFromArgument,
    const Lattice& ambientLatticeNewSpace,
    QuasiPolynomial& output
  );
  void Substitution(
    const Matrix<Rational>& mapFromNewSpaceToOldSpace,
    const Lattice& ambientLatticeNewSpace,
    QuasiPolynomial& output
  );
  void Substitution(
    const Vector<Rational>& inputTranslationSubtractedFromArgument,
    QuasiPolynomial& output
  );
  bool SubstitutionLessVariables(
    const PolynomialSubstitution<Rational>& theSub, QuasiPolynomial& output
  ) const;
  void operator+=(const QuasiPolynomial& other);
  QuasiPolynomial(){}
  void WriteToFile(std::fstream& output);
  bool ReadFromFile(std::fstream& input);
  QuasiPolynomial(const QuasiPolynomial& other) {
    this->operator=(other);
  }
  void operator*=(const Rational& theConst);
  void operator=(const QuasiPolynomial& other) {
    this->AmbientLatticeReduced = other.AmbientLatticeReduced;
    this->LatticeShifts = other.LatticeShifts;
    this->valueOnEachLatticeShift = other.valueOnEachLatticeShift;
  }
};

class PartFraction: ListLight<oneFracWithMultiplicitiesAndElongations> {
private:
  void findPivot();
  void findInitialPivot();
  //void intRootToString(std::stringstream& out, int* TheRoot, bool MinusInExponent);
  bool rootIsInFractionCone (PartFractions& owner, Vector<Rational>* theRoot) const;
  friend class PartFractions;
  friend class partFractionPolynomialSubstitution;
public:
  std::string DebugString;
  int LastDistinguishedIndex;
  int FileStoragePosition;
  bool PowerSeriesCoefficientIsComputed;
  bool IsIrrelevant;
  bool RelevanceIsComputed;
  List<int> IndicesNonZeroMults;
  friend std::ostream& operator << (std::ostream& output, const PartFraction& input) {
    (void) input;//avoid unused parameter warning, portable
    crash << " Not implemented, please fix. " << crash;
    return output;
  }
  static bool IsMonEqualToZero() {
    return false;
  }
  bool RemoveRedundantShortRootsClassicalRootSystem(
    PartFractions& owner, Vector<Rational>* Indicator, Polynomial<LargeInt>& buffer1, int theDimension
  );
  bool RemoveRedundantShortRoots(PartFractions& owner, Vector<Rational>* Indicator, int theDimension);
  bool AlreadyAccountedForInGUIDisplay;
  static bool flagAnErrorHasOccurredTimeToPanic;
  //static  bool flagUsingPrecomputedOrlikSolomonBases;
  static std::fstream TheBigDump;
  static bool UseGlobalCollector;
  static bool MakingConsistencyCheck;
  static Rational CheckSum, CheckSum2;
  static Vector<Rational> theVectorToBePartitioned;
  void ComputePolyCorrespondingToOneMonomial(
    QuasiPolynomial& outputQP,
    const MonomialP& theMon,
    Vectors<Rational>& normals,
    Lattice& theLattice
  ) const;
  static void EvaluateIntPoly(
    const Polynomial<LargeInt>& input, const Vector<Rational>& values, Rational& output
  );
  static void MakePolynomialFromOneNormal(
    Vector<Rational>& normal,
    const MonomialP& shiftRational,
    int theMult,
    Polynomial<Rational>& output
  );
  void ComputeNormals(
    PartFractions& owner, Vectors<Rational>& output, int theDimension, Matrix<Rational>& buffer
  );
  int ComputeGainingMultiplicityIndexInLinearRelation(
    bool flagUsingOrlikSolomon, Matrix<Rational>& theLinearRelation
  );
  void GetRootsFromDenominator(
    PartFractions& owner, Vectors<Rational>& output
  ) const;
  void GetVectorPartitionFunction(
    PartFractions& owner, Polynomial<LargeInt>& theCoeff, QuasiPolynomial& output
  ) const;
  LargeInt EvaluateIntPolyAtOne(Polynomial<LargeInt>& input);
  //void InsertNewRootIndex(int index);
  //void MultiplyMinusShiftBy (int* theRoot, int Multiplicity);
  void MultiplyCoeffBy(Rational& r);
  void decomposeAMinusNB(
    int indexA,
    int indexB,
    int n,
    int indexAminusNB,
    MonomialCollection<PartFraction, Polynomial<LargeInt> >& output,
    PartFractions& owner
  );
  bool DecomposeFromLinRelation(
    Matrix<Rational>& theLinearRelation,
    MonomialCollection<PartFraction, Polynomial<LargeInt> >& output,
    bool flagUsingOSbasis,
    List<Vector<Rational> >& startingVectors
  );
  void ComputeOneCheckSuM(
    PartFractions& owner, Rational& output, int theDimension
  ) const;
  bool ReduceMeOnce(
    const Polynomial<LargeInt>& myCoeff,
    Polynomial<LargeInt>& outputCoeff,
    Vectors<Rational>& startingVectors
  );
  void ReduceMonomialByMonomial(PartFractions& owner, int myIndex, Vector<Rational>* Indicator);
  void ApplySzenesVergneFormulA(
    List<Vector<Rational> >& startingVectors,
    List<int>& theSelectedIndices,
    List<int>& theElongations,
    int GainingMultiplicityIndex,
    int ElongationGainingMultiplicityIndex,
    MonomialCollection<PartFraction, Polynomial<LargeInt> >& output
  );
  void ApplyGeneralizedSzenesVergneFormulA(
    List<int>& theSelectedIndices,
    List<int>& theGreatestElongations,
    List<int>& theCoefficients,
    int GainingMultiplicityIndex,
    int ElongationGainingMultiplicityIndex,
    MonomialCollection<PartFraction,
    Polynomial<LargeInt> >& output,
    List<Vector<Rational> >& startingVectors
  );
  bool CheckForOrlikSolomonAdmissibility(List<int>& theSelectedIndices);
  bool reduceOnceTotalOrderMethod(
    MonomialCollection<PartFraction, Polynomial<LargeInt> >& output, PartFractions& owner
  );
  bool reduceOnceGeneralMethodNoOSBasis(
    PartFractions& owner,
    MonomialCollection<PartFraction,
    Polynomial<LargeInt> >& output,
    Vectors<Rational>& bufferVectors,
    Matrix<Rational>& bufferMat
  );
  bool ReduceOnceGeneralMethod(
    PartFractions& owner,
    MonomialCollection<PartFraction,
    Polynomial<LargeInt> >& output,
    Vectors<Rational>& bufferVectors,
    Matrix<Rational>& bufferMat
  );
  bool AreEqual(PartFraction& p);
  bool IsReduced();
  unsigned int HashFunction() const;
  static unsigned int HashFunction(const PartFraction& input) {
    return input.HashFunction();
  }
  int MultiplyByOneFrac(oneFracWithMultiplicitiesAndElongations& f);
  void init(int numRoots);
  void ComputeIndicesNonZeroMults();
  bool DecreasePowerOneFrac(int index, int increment);
  void PrepareFraction(
    int indexA,
    int indexB,
    int AminusNBindex,
    bool indexAisNullified,
    PartFraction& output,
    Polynomial<LargeInt>& AminusNbetaPoly,
    Polynomial<LargeInt>& outputCommonCoeff
  );
  void Assign(const PartFraction& p);
  void AssignDenominatorOnly(const PartFraction& p);
  void AssignNoIndicesNonZeroMults(PartFraction& p);
  int getSmallestNonZeroIndexGreaterThanOrEqualTo(PartFractions& owner, int minIndex);
  int ControlLineSizeFracs(std::string& output, FormatExpressions& PolyFormatLocal);
  int ControlLineSizeStringPolys(std::string& output, FormatExpressions& PolyFormatLocal);
  //void swap(int indexA, int indexB);
  PartFraction();
  ~PartFraction();
  void GetPolyReduceMonomialByMonomial(
    PartFractions& owner,
    Vector<Rational>& theExponent,
    int StartMonomialPower,
    int DenPowerReduction,
    int startDenominatorPower,
    Polynomial<LargeInt>& output
  );
  void ReduceMonomialByMonomialModifyOneMonomial(
    PartFractions& Accum,
    SelectionWithDifferentMaxMultiplicities& thePowers,
    List<int>& thePowersSigned,
    MonomialP& input,
    LargeInt& inputCoeff
  );
  void GetAlphaMinusNBetaPoly(PartFractions& owner, int indexA, int indexB, int n, Polynomial<LargeInt>& output);
  void GetNElongationPolyWithMonomialContribution(
    List<Vector<Rational> >& startingVectors,
    List<int>& theSelectedIndices,
    List<int>& theCoefficients,
    List<int>& theGreatestElongations,
    int theIndex,
    Polynomial<LargeInt>& output,
    int theDimension
  );
  void GetNElongationPoly(
    List<Vector<Rational> >& startingVectors,
    int index,
    int baseElongation,
    int LengthOfGeometricSeries,
    Polynomial<LargeInt>& output,
    int theDimension
  );
  static void GetNElongationPoly(Vector<Rational>& exponent, int n, Polynomial<LargeInt>& output, int theDimension);
  int GetNumProportionalVectorsClassicalRootSystems(PartFractions& owner);
  bool operator==(const PartFraction& right) const;
  void operator=(const PartFraction& right);
  bool initFromRoots(PartFractions& owner, Vectors<Rational>& input);
  std::string ToString(bool LatexFormat, FormatExpressions& PolyFormatLocal, int& NumLinesUsed);
  void ReadFromFile(PartFractions& owner, std::fstream& input);
  void WriteToFile(std::fstream& output) const;
  int SizeWithoutDebugString() const;
};

class Cone {
  void ComputeVerticesFromNormalsNoFakeVertices();
  bool EliminateFakeNormalsUsingVertices(int numAddedFakeWalls);
  friend std::ostream& operator << (std::ostream& output, const Cone& theCone) {
    output << theCone.ToString();
    return output;
  }
public:
  static const std::string GetXMLClassName() {
    return "Cone";
  }
  bool flagIsTheZeroCone;
  Vectors<Rational> Vertices;
  Vectors<Rational> Normals;
  int LowestIndexNotCheckedForChopping;
  int LowestIndexNotCheckedForSlicingInDirection;
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
  void TransformToWeylProjective(ConeComplex& owner);
  std::string DrawMeToHtmlProjective(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat);
  std::string DrawMeToHtmlLastCoordAffine(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat);
  void GetLinesContainedInCone(Vectors<Rational>& output);
  void TranslateMeMyLastCoordinateAffinization(Vector<Rational>& theTranslationVector);
  bool IsAnHonest1DEdgeAffine(const Vector<Rational>& vertex1, const Vector<Rational>& vertex2) const {
    int numCommonWalls = 0;
    for (int i = 0; i < this->Normals.size; i ++) {
      if (
        vertex1.ScalarEuclidean(this->Normals[i]).IsEqualToZero() &&
        vertex2.ScalarEuclidean(this->Normals[i]).IsEqualToZero()
      ) {
        numCommonWalls ++;
        if (numCommonWalls == this->GetDim() - 2) {
          return true;
        }
      }
    }
    return false;
  }
  bool IsTheEntireSpace() {
    return this->Normals.size == 0 && this->flagIsTheZeroCone;
  }
  bool IsAnHonest1DEdgeAffine(int vertexIndex1, int vertexIndex2) const {
    Vector<Rational>& vertex1 = this->Vertices[vertexIndex1];
    Vector<Rational>& vertex2 = this->Vertices[vertexIndex2];
    return this->IsAnHonest1DEdgeAffine(vertex1, vertex2);
  }
  bool DrawMeLastCoordAffine(
    bool InitDrawVars,
    DrawingVariables& theDrawingVariables,
    FormatExpressions& theFormat,
    const std::string& ChamberWallColor = nullptr
  ) const;
  bool DrawMeProjective(
    Vector<Rational>* coordCenterTranslation,
    bool initTheDrawVars,
    DrawingVariables& theDrawingVariables,
    FormatExpressions& theFormat
  ) const;
  bool IsInCone(const Vector<Rational>& point) const;
  bool IsInCone(const Vectors<Rational>& vertices) const {
    for (int i = 0; i < vertices.size; i ++) {
      if (!this->IsInCone(vertices[i])) {
        return false;
      }
    }
    return true;
  }
  bool GetLatticePointsInCone(
    Lattice& theLattice,
    Vector<Rational>& theShift,
    int upperBoundPointsInEachDim,
    bool lastCoordinateIsOne,
    Vectors<Rational>& outputPoints,
    Vector<Rational>* shiftAllPointsBy
  ) const;
  bool MakeConvexHullOfMeAnd(const Cone& other);
  void ChangeBasis(Matrix<Rational>& theLinearMap);
  std::string DebugString;
  int GetDim() const {
    if (this->Normals.size == 0) {
      return 0;
    }
    return this->Normals[0].size;
  }
  void SliceInDirection(Vector<Rational>& theDirection, ConeComplex& output);
  bool CreateFromNormalS(
    Vectors<Rational>& inputNormals,
    bool UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices
  );
  //returns false if the cone is non-proper, i.e. when either
  //1) the cone is empty or is of smaller dimension than it should be
  //2) the resulting cone is the entire space
  bool CreateFromNormals(Vectors<Rational>& inputNormals) {
    return this->CreateFromNormalS(inputNormals, false);
  }
  bool CreateFromVertices(const Vectors<Rational>& inputVertices);
  void GetInternalPoint(Vector<Rational>& output) const {
    if (this->Vertices.size <= 0) {
      return;
    }
    this->Vertices.sum(output, this->Vertices[0].size);
  }
  Vector<Rational> GetInternalPoint() const {
    Vector<Rational> result;
    this->GetInternalPoint(result);
    return result;
  }
  unsigned int HashFunction() const {
    return this->Vertices.HashFunction();
  }
  static unsigned int HashFunction(const Cone& input) {
    return input.HashFunction();
  }
  bool ProduceNormalFromTwoNormalsAndSlicingDirection(
    Vector<Rational>& SlicingDirection,
    Vector<Rational>& normal1,
    Vector<Rational>& normal2,
    Vector<Rational>& output
  );
  bool ReadFromFile(std::fstream& output);
  void WriteToFile(std::fstream& output);
  void operator=(const Cone& other) {
    //this->flagHasSufficientlyManyVertices = other.flagHasSufficientlyManyVertices;
    this->flagIsTheZeroCone = other.flagIsTheZeroCone;
    this->Vertices = other.Vertices;
    this->Normals = other.Normals;
    this->LowestIndexNotCheckedForSlicingInDirection = other.LowestIndexNotCheckedForSlicingInDirection;
    this->LowestIndexNotCheckedForChopping = other.LowestIndexNotCheckedForChopping;
  }
  Cone(const Cone& other) {
    this->operator=(other);
  }
  Cone() {
    this->LowestIndexNotCheckedForSlicingInDirection = 0;
    this->LowestIndexNotCheckedForChopping = 0;
    this->flagIsTheZeroCone = true;
    //this->flagHasSufficientlyManyVertices = true;
  }
  void IntersectAHyperplane(Vector<Rational>& theNormal, Cone& outputConeLowerDim);
  bool GetRootFromLPolyConstantTermGoesToLastVariable(Polynomial<Rational>& inputLPoly, Vector<Rational>& output);
  bool SolveLPolyEqualsZeroIAmProjective(Polynomial<Rational>& inputLPoly, Cone& outputCone);
  bool SolveLQuasiPolyEqualsZeroIAmProjective(QuasiPolynomial& inputLQP, List<Cone>& outputConesOverEachLatticeShift);
  bool operator>(const Cone& other) const {
    return this->Normals > other.Normals;
  }
  bool operator==(const Cone& other) const {
    return this->flagIsTheZeroCone == other.flagIsTheZeroCone && this->Normals == other.Normals;
  }
};

class ConeLatticeAndShift {
  public:
  static std::string GetXMLClassName() {
    return "ConeLatticeShift";
  }
  Cone theProjectivizedCone;
  Lattice theLattice;
  Vector<Rational> theShift;
  void FindExtremaInDirectionOverLatticeOneNonParam(
    Vector<Rational>& theLPToMaximizeAffine,
    Vectors<Rational>& outputAppendLPToMaximizeAffine,
    List<ConeLatticeAndShift>& outputAppend
  );
  void operator=(const ConeLatticeAndShift& other) {
    this->theProjectivizedCone = other.theProjectivizedCone;
    this->theLattice = other.theLattice;
    this->theShift = other.theShift;
  }
  void WriteToFile(std::fstream& output);
  void FindExtremaInDirectionOverLatticeOneNonParamDegenerateCase(
    Vector<Rational>& theLPToMaximizeAffine,
    Vectors<Rational>& outputAppendLPToMaximizeAffine,
    List<ConeLatticeAndShift>& outputAppend,
    Matrix<Rational>& theProjectionLatticeLevel
  );
  bool ReadFromFile(std::fstream& input);
  std::string ToString(FormatExpressions& theFormat);
  int GetDimProjectivized() {
    return this->theProjectivizedCone.GetDim();
  }
  int GetDimAffine() {
    return this->theProjectivizedCone.GetDim() - 1;
  }
};

class ConeComplex : public HashedList<Cone> {
public:
  bool flagChambersHaveTooFewVertices;
  bool flagIsRefined;
//  List<int> NonRefinedChambers;
  int indexLowestNonRefinedChamber;
  HashedList<Vector<Rational> > splittingNormals;
  Vectors<Rational> slicingDirections;
  Cone ConvexHull;
  std::string DebugString;
  void RefineOneStep();
  void Refine();
  void RefineMakeCommonRefinement(const ConeComplex& other);
  void Sort();
  void RefineAndSort();
  void FindMaxmumOverNonDisjointChambers(Vectors<Rational>& theMaximaOverEachChamber, Vectors<Rational>& outputMaxima);
  void MakeAffineAndTransformToProjectiveDimPlusOne(Vector<Rational>& affinePoint, ConeComplex& output);
  void TransformToWeylProjective();
  int GetDim() {
    if (this->size <= 0) {
      return - 1;
    }
    return this->TheObjects[0].GetDim();
  }
  bool AddNonRefinedChamberOnTopNoRepetition(const Cone& newCone);
  void PopChamberSwapWithLast(int index);
  void GetAllWallsConesNoOrientationNoRepetitionNoSplittingNormals(Vectors<Rational>& output) const;
  bool DrawMeLastCoordAffine(bool InitDrawVars, DrawingVariables& theDrawingVariables, FormatExpressions& theFormat);
  void TranslateMeMyLastCoordinateAffinization(Vector<Rational> & theTranslationVector);
  void InitFromDirectionsAndRefine(Vectors<Rational>& inputVectors);
  void InitFromAffineDirectionsAndRefine(Vectors<Rational>& inputDirections, Vectors<Rational>& inputAffinePoints);
  std::string DrawMeToHtmlLastCoordAffine(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat);
  bool DrawMeProjective(
    Vector<Rational>* coordCenterTranslation,
    bool InitDrawVars,
    DrawingVariables& theDrawingVariables,
    FormatExpressions& theFormat
  );
  std::string DrawMeToHtmlProjective(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat);
  std::string ToString(bool useHtml = false);
  void ComputeDebugString() {
    this->DebugString = this->ToString();
  }
  int GetLowestIndexchamberContaining(const Vector<Rational>& theRoot) const {
    for (int i = 0; i < this->size; i ++) {
      if (this->TheObjects[i].IsInCone(theRoot)) {
        return i;
      }
    }
    return - 1;
  }
  bool findMaxLFOverConeProjective(
    const Cone& input, List<Polynomial<Rational> >& inputLinPolys, List<int>& outputMaximumOverEeachSubChamber
  );
  bool findMaxLFOverConeProjective(
    const Cone& input, Vectors<Rational>& inputLFsLastCoordConst, List<int>& outputMaximumOverEeachSubChamber
  );
  void initFromCones(
    List<Vectors<Rational> >& NormalsOfCones,
    bool UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices
  );
  void initFromCones(
    List<Cone>& NormalsOfCones, bool UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices
  );
  bool SplitChamber(
    int indexChamberBeingRefined, bool weAreChopping, const Vector<Rational>& killerNormal
  );
  void GetNewVerticesAppend(
    Cone& myDyingCone, const Vector<Rational>& killerNormal, HashedList<Vector<Rational> >& outputVertices
  );
  void init() {
    this->splittingNormals.Clear();
    this->slicingDirections.size = 0;
    this->Clear();
    this->indexLowestNonRefinedChamber = 0;
    this->ConvexHull.Normals.size = 0;
    this->ConvexHull.Vertices.size = 0;
    this->ConvexHull.flagIsTheZeroCone = true;
  }
  ConeComplex(const ConeComplex& other):HashedList<Cone>() {
    this->operator=(other);
  }
  ConeComplex() {
    this->flagChambersHaveTooFewVertices = false;
    this->flagIsRefined = false;
  }
  void WriteToFile(std::fstream& output, int UpperLimit = - 1);
  bool ReadFromFile(std::fstream& input, int UpperLimitDebugPurposes = - 1);
  void operator=(const ConeComplex& other) {
    this->::HashedList<Cone>::operator=(other);
    this->splittingNormals = other.splittingNormals;
    this->slicingDirections = other.slicingDirections;
    this->indexLowestNonRefinedChamber = other.indexLowestNonRefinedChamber;
    this->flagIsRefined = other.flagIsRefined;
    this->flagChambersHaveTooFewVertices = other.flagChambersHaveTooFewVertices;
  }
};

class PartFractions: public MonomialCollection<PartFraction, Polynomial<LargeInt> > {
  bool splitPartial();
  void initCommon();
  PartFractions(const PartFractions& other);
public:
  int AmbientDimension;
  int IndexLowestNonProcessed;
  int IndexCurrentlyProcessed;
  int HighestIndex;
  int NumberIrrelevantFractions;
  int NumberRelevantReducedFractions;
  int NumMonomialsInTheNumerators;
  int NumGeneratorsInTheNumerators;
  int NumRelevantNonReducedFractions;
  //int NumRelevantMonomialsInNonReducedFractions;
  //int NumRelevantGeneratorsInNonReducedFractions;
  static int NumMonomialsInNumeratorsRelevantFractions;
  int NumGeneratorsRelevenatFractions;
  int NumMonomialsInNumeratorsIrrelevantFractions;
  int NumGeneratorsIrrelevantFractions;
  int NumTotalReduced;
  int NumProcessedForVPFfractions;
  int NumRunsReduceMonomialByMonomial;
  //GlobalVariables theGlobalVariables;
  static int NumProcessedForVPFMonomialsTotal;
  static std::fstream ComputedContributionsList;
  static const int MaxReadFileBufferSize = 33554432; //= 32 MB of read buffer size
  std::string DebugString;
  Rational StartCheckSum;
  Rational EndCheckSum;
  static Rational CheckSum;
  bool flagDiscardingFractions;
  bool flagUsingOrlikSolomonBasis;
  bool flagInitialized;
  int LimitSplittingSteps;
  int SplitStepsCounter;
  ConeComplex theChambers;
  static bool flagSplitTestModeNoNumerators;
  static bool flagAnErrorHasOccurredTimeToPanic;
  static bool flagMakingProgressReport;
  static bool flagUsingCheckSum;
  static int flagMaxNumStringOutputLines;

  HashedList<Vector<Rational> > startingVectors;
  Matrix<int> TableAllowedAminusB;
  Matrix<int> TableAllowedAminus2B;
  Selection IndicesRedundantShortRoots;
  List<int> IndicesDoublesOfRedundantShortRoots;
  int NumNonRedundantShortRoots;
  Vector<Rational> weights;
  void initFromRoots(Vectors<Rational>& theAlgorithmBasis, Vector<Rational>* theWeights);
  int AddRootAndSort(Vector<Rational>& theRoot);
  int AddRootPReserveOrder(Vector<Rational>& theRoot);
  int getIndex(const Vector<Rational>& TheRoot);
  int getIndexDoubleOfARoot(const Vector<Rational>& TheRoot);
  void ComputeTable(int theDimension);
  void PrepareCheckSums();
  std::string DoTheFullComputationReturnLatexFileString(
    Vectors<Rational>& toBePartitioned, FormatExpressions& theFormat, std::string* outputHtml
  );
  bool ArgumentsAllowed(Vectors<Rational>& theArguments, std::string& outputWhatWentWrong);
  bool AssureIndicatorRegularity(Vector<Rational> & theIndicator);
  void CompareCheckSums();
  void ComputeDebugString();
  void ComputeDebugStringNoNumerator();
  void ComputeDebugStringWithVPfunction();
  void ComputePolyCorrespondingToOneMonomial(
    QuasiPolynomial& outputQP, int monomialIndex, Vectors<Rational>& normals, Lattice& theLattice
  );
  void ComputeDebugStringBasisChange();
  bool initFromRoots(Vectors<Rational>& input);
  void initAndSplit(Vectors<Rational>& input);
  void Run(Vectors<Rational>& input);
  //row index is the index of the Vector<Rational> ; column(second) index is the coordinate index
  void RemoveRedundantShortRootsClassicalRootSystem(Vector<Rational>* Indicator);
  void RemoveRedundantShortRoots(Vector<Rational>* Indicator);
  bool RemoveRedundantShortRootsIndex(int theIndex, Vector<Rational>* Indicator);
  bool splitClassicalRootSystem(bool ShouldElongate, Vector<Rational>* Indicator);
  bool split(Vector<Rational>* Indicator);
  void ComputeKostantFunctionFromWeylGroup(
    char WeylGroupLetter, int WeylGroupNumber, QuasiPolynomial& output, Vector<Rational>* ChamberIndicator
  );
  bool IsHigherThanWRTWeight(
    const Vector<Rational>& left, const Vector<Rational>& r, const Vector<Rational>& theWeights
  );
  void ComputeSupport(List<Vectors<Rational> >& output);
  void ComputeOneCheckSum(Rational& output);
  void AccountPartFractionInternals(int sign, int index, Vector<Rational>* Indicator);
  void PopIndexHashChooseSwapByLowestNonProcessedAndAccount(int index, Vector<Rational>* Indicator);
  void PopIndexSwapLastHashAndAccount(int index, Vector<Rational>* Indicator);
  void PrepareIndicatorVariables();
  void initFromOtherPartFractions(PartFractions& input);
  void IncreaseHighestIndex(int increment);
  std::string ToString(FormatExpressions& theFormat) {
    std::string tempS;
    this->ToString(tempS, theFormat);
    return tempS;
  }
  void ToString(std::string& output, FormatExpressions& theFormat);
  int ToString(std::string& output, bool LatexFormat, FormatExpressions& theFormat);
  int ElementToStringBasisChange(
    std::string& output, bool LatexFormat, FormatExpressions& PolyFormatLocal
  );
  int ElementToStringOutputToFile(std::fstream& output, bool LatexFormat);
  int ElementToStringBasisChangeOutputToFile(std::fstream& output, bool LatexFormat);
  bool GetVectorPartitionFunction(QuasiPolynomial& output, Vector<Rational>& newIndicator);
  bool VerifyFileComputedContributions();
  void WriteToFileComputedContributions(std::fstream& output);
  int ReadFromFileComputedContributions(std::fstream& input);
  void WriteToFile(std::fstream& output);
  void ReadFromFile(std::fstream& input);
  void ResetRelevanceIsComputed() {
    crash << "This is not implemented yet. " << crash;
  }
  PartFractions();
  int SizeWithoutDebugString();
  bool CheckForMinimalityDecompositionWithRespectToRoot(Vector<Rational>  *theRoot);
  void MakeProgressReportSplittingMainPart();
  void MakeProgressReportRemovingRedundantRoots();
  void MakeProgressReportUncoveringBrackets();
  void MakeProgressVPFcomputation();
};

class DynkinSimpleType {
  friend std::ostream& operator << (std::ostream& output, const DynkinSimpleType& theMon) {
    output << theMon.ToString();
    return output;
  }
  public:
  char theLetter;
  int theRank;
  Rational CartanSymmetricInverseScale;
  DynkinSimpleType(): theLetter('X'), theRank(- 1), CartanSymmetricInverseScale(0) {
  }
  DynkinSimpleType(char inputChar, int inputRank, const Rational& inputScale = 1) :
    theLetter(inputChar), theRank(inputRank), CartanSymmetricInverseScale(inputScale) {

  }
  void MakeAone() {
    this->theLetter = 'A';
    this->theRank = 1;
    this->CartanSymmetricInverseScale = 1;
  }
  int GetRootSystemSize() const;
  int GetLieAlgebraDimension() const {
    return this->GetRootSystemSize() + this->theRank;
  }
  int GetSSAlgDim() const {
    return this->GetRootSystemSize() + this->theRank;
  }
  void MakeArbitrary(char inputLetter, int inputRank, Rational inputLengthFirstCorRootSquared) {
    if ((
        inputLetter != 'A' && inputLetter != 'B' && inputLetter != 'C' && inputLetter != 'D' &&
        inputLetter != 'E' && inputLetter != 'F' && inputLetter != 'G'
      ) || inputRank <= 0
    ) {
      crash << "This is a programming error. Requested to create a simple Dynkin type of type "
      << inputLetter << " and rank "
      << inputRank << ". This is not allowed: I only accept types A, B, C, D, E, F and G and non-negative ranks. "
      << crash;
    }
    if (inputLetter == 'G') {
      inputRank = 2;
    }
    if (inputLetter == 'F') {
      inputRank = 4;
    }
    if (inputLetter == 'E' && inputRank > 8) {
      inputRank = 8;
    }
    if (inputRank == 1) {
      inputLetter = 'A';
    }
    this->theRank = inputRank;
    this->theLetter = inputLetter;
    this->CartanSymmetricInverseScale = inputLengthFirstCorRootSquared;
  }
  Rational GetPrincipalSlTwoCSInverseScale() const;
  void GetCoCartanSymmetric(Matrix<Rational>& output) const;
  void GetCartanSymmetric(Matrix<Rational>& output) const;
  void GetAn(int n, Matrix<Rational>& output) const;
  void GetBn(int n, Matrix<Rational>& output) const;
  void GetCn(int n, Matrix<Rational>& output) const;
  void GetDn(int n, Matrix<Rational>& output) const;
  void GetEn(int n, Matrix<Rational>& output) const;
  void GetF4(Matrix<Rational>& output) const;
  void GetG2(Matrix<Rational>& output) const;
  void Grow(List<DynkinSimpleType>& output, List<List<int> >* outputPermutationRoots) const;
  void operator=(const DynkinSimpleType& other) {
    this->theLetter = other.theLetter;
    this->theRank = other.theRank;
    this->CartanSymmetricInverseScale = other.CartanSymmetricInverseScale;
  }
  bool operator==(const DynkinSimpleType& other) const {
    return
    this->theLetter == other.theLetter && this->theRank == other.theRank &&
    this->CartanSymmetricInverseScale == other.CartanSymmetricInverseScale;
  }
  static unsigned int HashFunction(const DynkinSimpleType& input) {
    return static_cast<unsigned int>(input.theLetter) * 2 +
    static_cast<unsigned int>(input.theRank) + SomeRandomPrimes[0] * input.CartanSymmetricInverseScale.HashFunction();
  }
  unsigned int HashFunction() const {
    return this->HashFunction(*this);
  }
  bool IsMonEqualToZero() const {
    return false;
  }
  void GetAutomorphismActingOnVectorColumn(MatrixTensor<Rational>& output, int AutoIndex) const;
  Rational GetDefaultCoRootLengthSquared(int rootIndex) const;
  Rational GetDefaultRootLengthSquared(int rootIndex) const;
  Rational GetDefaultLongRootLengthSquared() const;
  Rational GetEpsilonRealizationLongRootLengthSquared() const;
  static Rational GetDefaultLongRootLengthSquared(char theInputType) {
    DynkinSimpleType theType(theInputType, 2);
    return theType.GetDefaultLongRootLengthSquared();
  }
  Rational GetLongRootLengthSquared() const;
  Rational GetRatioRootSquaredToFirstSquared(int rootIndex) const;
  static Rational GetRatioLongRootToFirst(char inputWeylLetter, int inputRank);
  static Rational GetDynkinIndexParabolicallyInducingSubalgebra(char inputType);
  bool CanBeExtendedParabolicallyTo(const DynkinSimpleType& otherType) const;
  bool CanBeExtendedParabolicallyOrIsEqualTo(const DynkinSimpleType& otherType) const {
    if (*this == otherType) {
      return true;
    }
    return this->CanBeExtendedParabolicallyTo(otherType);
  }
  bool HasPrecomputedSubalgebras() const;
  Rational GetRatioLongRootToFirst() const {
    return this->GetRatioLongRootToFirst(this->theLetter, this->theRank);
  }
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringNonTechnicalName(FormatExpressions* theFormat = nullptr) const;
  void operator++(int);
  bool operator>(const DynkinSimpleType& other) const;
  static void GetEpsilonMatrix(char WeylLetter, int WeylRank, Matrix<Rational>& output);
  bool operator<(const DynkinSimpleType& other) const {
    return other > *this;
  }
  bool operator<(int otherRank) const;
};

//this class may need a modification: perhaps it should not inherit monomial collection,
//but rather have a monomial collection as a private member.
//Many important operations with Dynkin types require fixed order of monomials,
//which may impose this reorganization.
class DynkinType: public MonomialCollection<DynkinSimpleType, Rational> {
public:
  void GetLettersTypesMults(
    List<char>* outputLetters = nullptr,
    List<int>* outputRanks = nullptr,
    List<int>* outputMults = nullptr,
    List<Rational>* outputFirstCoRootLengthsSquared = nullptr
  ) const;
  void GetTypesWithMults(List<DynkinSimpleType>& output) const;
  bool IsOfSimpleType(char inputType, int inputRank) const {
    char currentType;
    int currentRank;
    if (!this->IsSimple(&currentType, &currentRank)) {
      return false;
    }
    return currentType == inputType && currentRank == inputRank;
  }
  static void GetOuterAutosGeneratorsOneTypeActOnVectorColumn(
    List<MatrixTensor<Rational> >& output, const DynkinSimpleType& theType, int multiplicity
  );
  unsigned int HashFunction() const {
    return this->MonomialCollection<DynkinSimpleType, Rational>::HashFunction();
  }
  static unsigned int HashFunction(const DynkinType& input) {
    return MonomialCollection<DynkinSimpleType, Rational>::HashFunction(input);
  }
  void GetOuterAutosGeneratorsActOnVectorColumn(List<MatrixTensor<Rational> >& output);
  bool IsSimple(char* outputtype = nullptr, int* outputRank = nullptr, Rational* outputLength = nullptr) const;
  void GetSortedDynkinTypes(List<DynkinSimpleType>& output) const;
  Rational GetPrincipalSlTwoCSInverseScale() const;
  void SortTheDynkinTypes();
  bool Grow(
    const List<Rational>& allowedInverseScales,
    int AmbientWeylDim,
    List<DynkinType>& output,
    List<List<int> >* outputPermutationRoots
  ) const;
  bool HasPrecomputedSubalgebras() const;
  std::string ToStringVirtualNameFolder() const;
  bool ContainsType(char theTypeLetter) const;
  void GetDynkinTypeWithDefaultScales(DynkinType& output) const;
  static void GetPrecomputedDynkinTypes(List<DynkinType>& output);
  DynkinSimpleType GetGreatestSimpleType() const;
  DynkinSimpleType GetSmallestSimpleType() const;
  LargeInt GetWeylGroupSizeByFormula() const;
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
  void ScaleFirstCoRootSquaredLength(const Rational& multiplyCoRootSquaredLengthBy);
  int GetMult(int SimpleTypeIdentifier) const {
    int result = 0;
    if (!this->theCoeffs[SimpleTypeIdentifier].IsSmallInteger(&result)) {
      crash << "This is a programming error: Dynkin type has multiplicity that is not a small integer " << crash;
    }
    return result;
  }
  int GetNumSimpleComponentsOfGivenRank(int desiredRank) const;
  int GetNumSimpleComponents() const;
  Rational GetRankRational() const;
  int GetRank() const;
  int GetRootSystemSize() const {
    Rational result = 0;
    for (int i = 0; i < this->size(); i ++) {
      result += this->theCoeffs[i] * (*this)[i].GetRootSystemSize();
    }
    int intResult = 0;
    if (!result.IsSmallInteger(&intResult)) {
      crash << "This is a programming error: multiplicity of simple type is not a small integer. " << crash;
    }
    return intResult;
  }
  int GetLieAlgebraDimension() const {
    Rational result = 0;
    for (int i = 0; i < this->size(); i ++) {
      result += this->theCoeffs[i] * (*this)[i].GetLieAlgebraDimension();
    }
    int intResult = 0;
    if (!result.IsSmallInteger(&intResult)) {
      crash << "This is a programming error: multiplicity of simple type is not a small integer. " << crash;
    }
    return intResult;
  }
  bool IsTypeA_1() const;
  static int GetIndexPreimageFromRootInjection(int inputIndex, const List<int>& inputRootInjection);
  bool CanBeExtendedParabolicallyTo(const DynkinType& other) const;
  bool CanBeExtendedParabolicallyOrIsEqualTo(const DynkinType& other) const;
  void MakeSimpleType(char type, int rank, const Rational* inputFirstCoRootSqLength = nullptr);
  void GetEpsilonMatrix(Matrix<Rational>& output) const;
  void GetCoCartanSymmetric(Matrix<Rational>& output) const;
  void GetCartanSymmetric(Matrix<Rational>& output) const;
  void GetCartanSymmetricDefaultLengthKeepComponentOrder(Matrix<Rational>& output) const;
  int GetCoxeterEdgeWeight(int v, int w);
  std::string GetWeylGroupName(FormatExpressions* theFormat = nullptr) const;
  static void GetDynkinIndicesSl2SubalgebrasSimpleType(
    const DynkinSimpleType& theType,
    List<List<Rational> >& precomputedDynkinIndicesSl2subalgebrasSimpleTypes,
    HashedList<DynkinSimpleType>& dynkinSimpleTypesWithComputedSl2Subalgebras,
    HashedList<Rational>& outputDynkinIndices
  );
  void GetDynkinIndicesSl2Subalgebras(
    List<List<Rational> >& precomputedDynkinIndicesSl2subalgebrasSimpleTypes,
    HashedList<DynkinSimpleType>& dynkinSimpleTypesWithComputedSl2Subalgebras,
    HashedList<Rational>& outputDynkinIndices
  );
  bool HasExceptionalComponent() const;
  bool operator>(const DynkinType& other) const;
  void operator=(const MonomialCollection<DynkinSimpleType, Rational>& other) {
    this->::MonomialCollection<DynkinSimpleType, Rational>::operator=(other);
  }
  DynkinType operator-(const MonomialCollection<DynkinSimpleType, Rational>& other) {
    DynkinType result = *this;
    result -= other;
    return result;
  }
};

class DynkinDiagramRootSubalgebra {
public:
  Vectors<Rational> AmbientRootSystem;
  Matrix<Rational> AmbientBilinearForm;
  List<Vectors<Rational> > SimpleBasesConnectedComponents;
  //to each connected component of the simple bases corresponds
  //its dynkin string with the same index
  List<DynkinSimpleType> SimpleComponentTypes;
  List<int> indicesThreeNodes;
  List<List<int> > indicesEnds;
  List<List<int> > sameTypeComponents;
  List<int> indexUniComponent;
  List<int> indexInUniComponent;
  bool CheckInitialization() const;
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
  int RankTotal();
  int NumRootsGeneratedByDiagram();
  void Sort();
  void GetDynkinType(DynkinType& output) const;
  void SwapDynkinStrings(int i, int j);
  Rational GetSquareLengthLongestRootLinkedTo(const Vector<Rational>& inputVector);
  Rational GetSquareLengthShortestRootLinkedTo(const Vector<Rational>& inputVector);
  bool LetterIsDynkinGreaterThanLetter(char letter1, char letter2);
  // The function below takes as an input a set of roots and
  // computes the corredponding Dynkin diagram of the
  // root subsystem. Note: the simleBasisInput is required to be a set of simple roots.
  // The procedure calls a
  // transformation to simple basis on the simpleBasisInput,
  // so your input will get changed if it wasn't
  // simple as required.
  void ComputeDiagramTypeModifyInput(Vectors<Rational>& inputRoots, WeylGroupData& theWeyl);
  //the below function is just as the above but doesn't modify simpleBasisInput
  void ComputeDiagramInputIsSimple(const Vectors<Rational>& simpleBasisInput);
  void ComputeDiagramTypeModifyInputRelative(
    Vectors<Rational>& inputOutputSimpleWeightSystem,
    const HashedList<Vector<Rational> >& weightSystem,
    const Matrix<Rational>& theBilinearForm
  );
  void ComputeDynkinStrings();
  void ComputeDynkinString(int indexComponent);
  int numberOfThreeValencyNodes(int indexComponent);
  bool operator==(const DynkinDiagramRootSubalgebra& right) const;
  bool IsGreaterThan(DynkinDiagramRootSubalgebra& right);
  Rational GetSizeCorrespondingWeylGroupByFormula();
  void GetAutomorphism(List<List<int> >& output, int index);
  void GetAutomorphisms(List<List<List<int> > >& output);
  void GetMapFromPermutation(
    Vectors<Rational>& domain,
    Vectors<Rational>& range,
    List<int>& thePerm,
    List<List<List<int> > >& theAutos,
    SelectionWithDifferentMaxMultiplicities& theAutosPerm,
    DynkinDiagramRootSubalgebra& right
  );
};

class VectorPartition {
public:
  Vectors<Rational> PartitioningRoots;
  Vector<Rational> goalVector;
  List<int> currentPartition;
  Vector<Rational> currentPartitionSum;
  // Format: each element of thePartitions gives an array whose entries give
  // the multiplicity of the weights. I.e. if PartitioningRoots has 2 elements, then thePartitions[0]
  // would have 2 elements: the first giving the multiplicity of PartitioningRoots[0]
  // and the second - the multiplicity of
  // PartitioningRoots[1]
  List<List<int> > thePartitions;
  bool flagStoreAllPartitions;
  VectorPartition() {
    this->flagStoreAllPartitions = false;
  }
  Vector<Rational> GetPartitionSum();
  void BeefUpPartition();
  bool NudgePartition();
  std::string ToStringPartitioningVectors();
  std::string ToStringOnePartition(const List<int>& currentPartition);
  std::string ToStringAllPartitions(bool useHtml);
  bool init(const Vectors<Rational>& inputPartitioningRoots, const Vector<Rational>& inputRoot);
  bool IncrementReturnFalseIfPastLast();
};

template <class coefficient>
class ElementWeylAlgebra : public MonomialCollection<MonomialWeylAlgebra, coefficient> {
public:
  unsigned int HashFunction() const {
    return this->::MonomialCollection<MonomialWeylAlgebra, coefficient>::HashFunction();
  }
  static unsigned int HashFunction(const ElementWeylAlgebra& input) {
    return input.HashFunction();
  }
  void MakeGEpsPlusEpsInTypeD(int i, int j, int NumVars);
  void MakeGEpsMinusEpsInTypeD(int i, int j, int NumVars);
  void MakeGMinusEpsMinusEpsInTypeD(int i, int j, int NumVars);
  void Makexixj(int i, int j, int NumVars);
  void Makexi(int i, int NumVars);
  void Makedi(int i, int NumVars);
  void Makedidj(int i, int j, int NumVars);
  void Makexidj(int i, int j, int NumVars);
  static void GetStandardOrderDiffOperatorCorrespondingToNraisedTo(
    const Rational& inputRationalPower,
    int indexVar,
    ElementWeylAlgebra& outputDO,
    Polynomial<Rational>& outputDenominator
  );
  void FourierTransform (ElementWeylAlgebra<coefficient>& output) const;
  bool ActOnPolynomial(Polynomial<Rational>& thePoly) const;
  void SetNumVariables(int newNumVars);
  void MultiplyOnTheLeft(const ElementWeylAlgebra& standsOnTheLeft);
  static void LieBracket(const ElementWeylAlgebra& left, const ElementWeylAlgebra& right, ElementWeylAlgebra& output);
  void LieBracketOnTheLeft(const ElementWeylAlgebra& standsOnTheLeft);
  void LieBracketOnTheLeftMakeReport(const ElementWeylAlgebra& standsOnTheLeft);
  void LieBracketOnTheRightMakeReport(const ElementWeylAlgebra& standsOnTheRight);
  void LieBracketOnTheRight(const ElementWeylAlgebra& standsOnTheRight);
  bool Substitution(
    const PolynomialSubstitution<Rational>& SubPolyPart,
    const PolynomialSubstitution<Rational>& SubDiffPArt
  );
  void MakeOne(int ExpectedNumVars = 0) {
    MonomialWeylAlgebra tempMon;
    tempMon.polynomialPart.MakeOne(ExpectedNumVars);
    tempMon.differentialPart.MakeOne(ExpectedNumVars);
    this->MakeZero();
    this->AddMonomial(tempMon, 1);
  }
  bool IsPolynomial(Polynomial<coefficient>* whichPoly = 0) const;
  bool HasNonSmallPositiveIntegerDerivation() const;
  void RaiseToPower(int thePower);
  void MultiplyTwoMonomials(
    const MonomialWeylAlgebra& left, const MonomialWeylAlgebra& right, ElementWeylAlgebra& output
  ) const;
  void AssignPolynomial(const Polynomial<Rational>& input) {
    this->MakeZero();
    MonomialWeylAlgebra tempM;
    for (int i = 0; i < input.size(); i ++) {
      tempM.polynomialPart = input[i];
      this->AddMonomial(tempM, input.theCoeffs[i]);
    }
  }
  void operator=(const std::string& input);
  void operator*=(const coefficient& other) {
    this->MonomialCollection<MonomialWeylAlgebra, coefficient>::operator*=(other);
  }
  void operator*=(const ElementWeylAlgebra& other);
  bool IsLetter(char theLetter);
  bool IsIndex(char theIndex);
  bool IsNumber(char theNumber);
};

class LittelmannPath {
public:
  WeylGroupData* owner;
  Vectors<Rational> Waypoints;
  void MakeFromWeightInSimpleCoords(const Vector<Rational>& weightInSimpleCoords, WeylGroupData& theOwner);
  void MakeFromWaypoints(Vectors<Rational>& weightsInSimpleCoords, WeylGroupData& theOwner) {
    this->owner = &theOwner;
    this->Waypoints = weightsInSimpleCoords;
    this->Simplify();
  }
  void ActByFalpha(int indexAlpha);
  void ActByEalpha(int indexAlpha);
  void ActByEFDisplayIndex(int displayIndex);
  void operator+=(const LittelmannPath& other) {
    this->Waypoints.Reserve(this->Waypoints.size + other.Waypoints.size);
    Vector<Rational> endPoint = *this->Waypoints.LastObject();
    for (int i = 0; i < other.Waypoints.size; i ++) {
      this->Waypoints.AddOnTop(other.Waypoints[i] + endPoint);
    }
  }
  bool IsAdaptedString(MonomialTensor<int, MathRoutines::IntUnsignIdentity>& theString);
  std::string ElementToStringIndicesToCalculatorOutput(LittelmannPath& inputStartingPath, List<int>& input);
  std::string ElementToStringOperatorSequenceStartingOnMe(List<int>& input);
  bool GenerateOrbit(
    List<LittelmannPath>& output,
    List<List<int> >& outputOperators,
    int UpperBoundNumElts,
    Selection* parabolicNonSelectedAreInLeviPart = nullptr
  );
  std::string GenerateOrbitAndAnimate();
  bool MinimaAreIntegral();
  std::string ToString(bool useSimpleCoords = true, bool useArrows = true, bool includeDominance = false) const;
  void Simplify();
  unsigned int HashFunction() const {
    return this->Waypoints.HashFunction();
  }
  static unsigned int HashFunction(const LittelmannPath& input) {
    return input.HashFunction();
  }
  bool IsEqualToZero() const {
    return this->Waypoints.size == 0;
  }
  void operator=(const LittelmannPath& other) {
    this->Waypoints = other.Waypoints;
    this->owner = other.owner;
  }
  bool operator==(const LittelmannPath& other) const {
    return this->Waypoints == other.Waypoints;
  }
};

class ConeLatticeAndShiftMaxComputation {
public:
  static const std::string GetXMLClassName() {
    return "ConeLatticeAndShiftMaxComputation";
  }
  int numNonParaM;
  int numProcessedNonParam;
  List<ConeComplex> complexStartingPerRepresentative;
  List<ConeComplex> complexRefinedPerRepresentative;
  List<List<Vectors<Rational> > > theMaximaCandidates;
  List<Vectors<Rational> > startingLPtoMaximize;
  List<Vectors<Rational> > finalMaxima;
  Lattice theStartingLattice;
  Lattice theFinalRougherLattice;
  Vector<Rational> theStartingRepresentative;
  Vectors<Rational> theFinalRepresentatives;
  List<ConeLatticeAndShift> theConesLargerDim;
  List<ConeLatticeAndShift> theConesSmallerDim;
  List<List<ConeComplex> > finalMaximaChambers;
  List<List<List<int> > > finalMaximaChambersIndicesMaxFunctions;
  List<bool> IsInfinity;
  Vectors<Rational> LPtoMaximizeLargerDim;
  Vectors<Rational> LPtoMaximizeSmallerDim;

  std::string ToString(FormatExpressions* theFormat = nullptr);
  void init(Vector<Rational>& theNEq, Cone& startingCone, Lattice& startingLattice, Vector<Rational>& startingShift);
  void FindExtremaParametricStep1(PauseThread& thePauseController);
  void FindExtremaParametricStep2TrimChamberForMultOne(PauseThread& thePauseController);
  void FindExtremaParametricStep3();
  void FindExtremaParametricStep4();
  void FindExtremaParametricStep5();
  void WriteToFile(std::fstream& output);
  bool ReadFromFile(std::fstream& input, int UpperLimitDebugPurposes);
};

class PiecewiseQuasipolynomial {
  public:
  // Let the piecewise quasipolynomial be in n variables.
  // Then the theProjectivizedComplex is an n + 1-dimensional complex,
  // which is the projectivization of the n-dim affine complex representing the
  // domain of the piecewise quasipoly.
  ConeComplex theProjectivizedComplex;
  List<QuasiPolynomial> theQPs;
  int NumVariables;
  std::string ToString(bool useLatex, bool useHtml);
  void DrawMe(
    DrawingVariables& theDrawingVars,
    int numLatticePointsPerDim,
    Cone* RestrictingChamber = nullptr,
    Vector<Rational>* distinguishedPoint = nullptr
  );
  int GetNumVars() {
    return this->NumVariables;
  }
  void MakeCommonRefinement(const PiecewiseQuasipolynomial& other) {
    this->MakeCommonRefinement(other.theProjectivizedComplex);
  }
  bool CheckConsistency() const {
    return true;
  }
  void MakeCommonRefinement(const ConeComplex& other);
  void TranslateArgument(Vector<Rational>& translateToBeAddedToArgument);
  bool MakeVPF(Vectors<Rational>& theRoots, std::string& outputstring);
  Rational Evaluate(const Vector<Rational>& thePoint);
  Rational EvaluateInputProjectivized(const Vector<Rational>& thePoint);
  void MakeZero(int numVars) {
    this->NumVariables = numVars;
    this->theProjectivizedComplex.init();
    this->theQPs.size = 0;
  }
  void operator+=(const PiecewiseQuasipolynomial& other);
  void operator*=(const Rational& other);
};

class MonomialMatrix {
  friend std::ostream& operator<<(std::ostream& output, const MonomialMatrix& theMon) {
    output << theMon.ToString();
    return output;
  }
  public:
  int vIndex;
  int dualIndex;
  bool IsId;
  MonomialMatrix(const MonomialMatrix& other) {
    this->operator=(other);
  }
  MonomialMatrix(): vIndex(- 1), dualIndex(- 1), IsId(false) {
  }
  MonomialMatrix(int i, int j): vIndex(i), dualIndex(j), IsId(false) {
  }
  void operator=(const MonomialMatrix& other) {
    this->vIndex = other.vIndex;
    this->dualIndex = other.dualIndex;
    this->IsId = other.IsId;
  }
  void MakeEij(int i, int j) {
    this->dualIndex = j;
    this->vIndex = i;
    this->IsId = false;
  }
  void MakeOne() {
    this->MakeIdSpecial();
  }
  void Transpose() {
    MathRoutines::swap(this->vIndex, this->dualIndex);
  }
  void MakeZero() {
    this->IsId = false;
    this->vIndex = - 1;
    this->dualIndex = - 1;
  }
  bool CheckConsistency() const {
    return true;
  }
  bool IsMonEqualToZero() const {
    return !this->IsId && this->vIndex == - 1 && this->dualIndex == - 1;
  }
  bool operator==(const MonomialMatrix& other) const {
    return this->vIndex == other.vIndex && this->dualIndex == other.dualIndex && this->IsId == other.IsId;
  }
  static unsigned int HashFunction(const MonomialMatrix& input) {
    return static_cast<unsigned int>(input.vIndex) * SomeRandomPrimes[0] +
    static_cast<unsigned int>(input.dualIndex) * SomeRandomPrimes[1] + input.IsId;
  }
  unsigned int HashFunction() const {
    return HashFunction(*this);
  }
  bool operator>(const MonomialMatrix& other) const {
    if (this->IsId != other.IsId) {
      return this->IsId > other.IsId;
    }
    if (this->vIndex == other.vIndex) {
      return this->dualIndex > other.dualIndex;
    }
    return this->vIndex > other.vIndex;
  }
  void MakeIdSpecial() {
    this->vIndex = - 1;
    this->dualIndex = - 1;
    this->IsId = true;
  }
  std::string ToString(FormatExpressions* theFormat = nullptr) const {
    (void) theFormat;//avoid unused parameter warning, portable
    std::stringstream out;
    if (!this->IsId) {
      out << "m_{" << this->vIndex + 1 << "}\\otimes " << "m^*_{" << this->dualIndex + 1 << "}";
    } else {
      out << "id";
    }
    return out.str();
  }
  void operator*=(const MonomialMatrix& other) {
    MacroRegisterFunctionWithName("MonomialMatrix::operator*=");
    if (this == &other) {
      MonomialMatrix otherCopy;
      otherCopy = other;
      *this *= otherCopy;
      return;
    }
    if (other.IsId) {
      return;
    }
    if (this->IsId) {
      *this = other;
      return;
    }
    if (this->dualIndex == other.vIndex) {
      this->dualIndex = other.dualIndex;
    } else {
      this->MakeZero();
    }
  }
};

template <class coefficient>
class MatrixTensor: public ElementMonomialAlgebra<MonomialMatrix, coefficient> {
public:
  void MakeIdSpecial() {
    this->MakeZero();
    MonomialMatrix theMon;
    theMon.MakeIdSpecial();
    this->AddMonomial(theMon, 1);
  }
  void MakeId(int numVars) {
    this->MakeZero();
    MonomialMatrix theMon;
    for (int i = 0; i < numVars; i ++) {
      theMon.dualIndex = i;
      theMon.vIndex = i;
      this->AddMonomial(theMon, 1);
    }
  }
  void Invert();
  int GetMinNumRows() const {
    int result = - 1;
    for (int i = 0; i < this->size(); i ++) {
      result = MathRoutines::Maximum(result, (*this)[i].vIndex);
    }
    return result + 1;
  }
  int GetMinNumCols() const {
    int result = - 1;
    for (int i = 0; i < this->size(); i ++) {
      result = MathRoutines::Maximum(result, (*this)[i].dualIndex);
    }
    return result + 1;
  }
  int GetMinNumColsNumRows() const {
    int result = - 1;
    for (int i = 0; i < this->size(); i ++) {
      result = MathRoutines::Maximum(result, (*this)[i].dualIndex);
      result = MathRoutines::Maximum(result, (*this)[i].vIndex);
    }
    return result + 1;
  }
  coefficient GetDeterminant() const {
    Matrix<coefficient> theMat;
    this->GetMatrix(theMat, this->GetMinNumColsNumRows());
    return theMat.GetDeterminant();
  }
  void DirectSumWith(const MatrixTensor<coefficient>& other);
  void GetVectorsSparseFromRowsIncludeZeroRows(List<VectorSparse<coefficient> >& output, int MinNumRows = - 1);
  bool IsID() const {
    int theDim = this->GetMinNumColsNumRows();
    Selection theSel;
    theSel.init(theDim);
    for (int i = 0; i < this->size(); i ++) {
      if ((*this)[i].vIndex != (*this)[i].dualIndex) {
        return false;
      }
      if (this->theCoeffs[i] != 1) {
        return false;
      }
      theSel.AddSelectionAppendNewIndex((*this)[i].vIndex);
    }
    return theSel.CardinalitySelection == theDim;
  }
  bool IsPositiveDefinite() {
    Matrix<coefficient> other;
    this->GetMatrix(other, this->GetMinNumColsNumRows());
    return other.IsPositiveDefinite();
  }
  void operator=(const Matrix<coefficient>& other) {
    this->MakeZero();
    MonomialMatrix theMon;
    for (int i = 0; i < other.NumRows; i ++) {
      for (int j = 0; j < other.NumCols; j ++) {
        if (!other.elements[i][j].IsEqualToZero()) {
          theMon.dualIndex = j;
          theMon.vIndex = i;
          this->AddMonomial(theMon, other.elements[i][j]);
        }
      }
    }
  }
  void AssignTensorProduct(const MatrixTensor<coefficient>& left, const MatrixTensor<coefficient>& right) {
    //handle lazy programmers:
    if (this == &left || this == &right) {
      MatrixTensor<coefficient> leftCopy = left;
      MatrixTensor<coefficient> rightCopy = right;
      this->AssignTensorProduct(leftCopy, rightCopy);
      return;
    }
    //The basis of the tensor product vector space MUST be in the SAME order as the one used by Matrix::AssignTensorProduct.
    //int leftDomainDim= left.GetMinNumCols();
    int rightDomainDim = right.GetMinNumCols();
    //int leftRangeDim= left.GetMinNumRows();
    int rightRangeDim = right.GetMinNumRows();
    MonomialMatrix tempM;
    this->MakeZero();
    coefficient tempCF;
    for (int i = 0; i < left.size(); i ++) {
      for (int j = 0; j < right.size(); j ++) {
        tempM.dualIndex = left[i].dualIndex * rightDomainDim + right[j].dualIndex;
        tempM.vIndex = left[i].vIndex * rightRangeDim + right[j].vIndex;
        tempCF = left.theCoeffs[i];
        tempCF *= right.theCoeffs[j];
        this->AddMonomial(tempM, tempCF);
      }
    }
  }
  void Substitution(const PolynomialSubstitution<Rational>& theSub) {
    MatrixTensor<coefficient> thisCopy = *this;
    this->MakeZero();
    coefficient tempCF;
    for (int i = 0; i < thisCopy.size(); i ++) {
      tempCF = thisCopy.theCoeffs[i];
      tempCF.Substitution(theSub);
      this->AddMonomial(thisCopy[i], tempCF);
    }
  }
  void Transpose() {
    MatrixTensor<coefficient> output;
    MonomialMatrix theMon;
    output.MakeZero();
    for (int i = 0; i < this->size(); i ++) {
      theMon = (*this)[i];
      theMon.Transpose();
      output.AddMonomial(theMon, this->theCoeffs[i]);
    }
    *this = output;
  }
  void SetNumVariables(int newNumVars) {
    MatrixTensor<coefficient> thisCopy = *this;
    this->MakeZero();
    coefficient tempCF;
    for (int i = 0; i < thisCopy.size; i ++) {
      tempCF = thisCopy.theCoeffs[i];
      tempCF.SetNumVariables(newNumVars);
      this->AddMonomial(thisCopy[i], tempCF);
    }
  }
  void LieBracketOnTheLeft(const MatrixTensor<coefficient>& standsOnTheLeft) {
    MacroRegisterFunctionWithName("MatrixTensor<coefficient>::LieBracketOnTheLeft");
    MatrixTensor<coefficient> output;
    MonomialMatrix theMon;
    output.MakeZero();
    output.SetExpectedSize(this->size() * standsOnTheLeft.size() * 2);
    coefficient tempCF;
    for (int i = 0; i < this->size(); i ++) {
      for (int j = 0; j < standsOnTheLeft.size(); j ++) {
        tempCF = this->theCoeffs[i];
        tempCF *= standsOnTheLeft.theCoeffs[j];
        theMon = standsOnTheLeft[j];
        theMon *= (*this)[i];
        output.AddMonomial(theMon, tempCF);
        tempCF *= - 1;
        theMon = (*this)[i];
        theMon *= standsOnTheLeft[j];
        output.AddMonomial(theMon, tempCF);
      }
    }
    *this = output;
  }
  std::string ToStringMatForm(FormatExpressions* theFormat = nullptr) const {
    if (this->IsEqualToZero()) {
      return "(0)";
    }
    Matrix<coefficient> tempMat;
    this->GetMatrix(tempMat, this->GetMinNumColsNumRows());
    return tempMat.ToString(theFormat);
  }
  void GetMatrix(Matrix<coefficient>& output, int theDim) const {
    theDim = MathRoutines::Maximum(theDim, this->GetMinNumColsNumRows());
    output.init(theDim, theDim);
    output.MakeZero();
    for (int i = 0; i < this->size(); i ++) {
      if ((*this)[i].IsId) {
        for (int j = 0; j < theDim; j ++) {
          output(j, j) += this->theCoeffs[i];
        }
      } else {
        output((*this)[i].vIndex, (*this)[i].dualIndex) += this->theCoeffs[i];
      }
    }
  }
  template <class otherType>
  void AssignVectorsToColumns(const Vectors<otherType>& inputVectors) {
    this->MakeZero();
    for (int i = 0; i < inputVectors.size; i ++) {
      for (int j = 0; j < inputVectors[i].size; j ++) {
        this->AddMonomial(MonomialMatrix(j, i), inputVectors[i][j]);
      }
    }
  }
  template <class otherType>
  void AssignVectorsToRows(const List<VectorSparse<otherType> >& inputVectors) {
    this->MakeZero();
    for (int i = 0; i < inputVectors.size; i ++) {
      for (int j = 0; j < inputVectors[i].size(); j ++) {
        this->AddMonomial(MonomialMatrix(i, inputVectors[i][j].theIndex), inputVectors[i].theCoeffs[j]);
      }
    }
  }
  bool IsNilpotent() const {
    MatrixTensor<coefficient> theMat;
    theMat = *this;
    for (int theDim = this->GetMinNumColsNumRows() + 1; theDim > 0; theDim /= 2) {
      theMat *= theMat;
      if (theMat.IsEqualToZero()) {
        return true;
      }
    }
    return false;
  }
  void GaussianEliminationByRowsMatrix(MatrixTensor<coefficient>* carbonCopyMat = 0);
  template <class otherType>
  void ActOnVectorColumn(const Vector<otherType>& input, Vector<otherType>& output) const {
    if (&input == &output) {
      Vector<otherType> inputCopy = input;
      this->ActOnVectorColumn(inputCopy, output);
      return;
    }
    output.MakeZero(this->GetMinNumRows());
    otherType currentCF;
    for (int i = 0; i < this->size(); i ++) {
      //note that, at the cost of one extra implicit conversion below, we pReserve the order of multiplication:
      //first is matrix element, then vector coordinate. The code should work as-is for non-commutative fields.
      //(think in the generality of quaternion matrix acting on quaternion-coefficient polynomials!)
      currentCF = this->theCoeffs[i];
      currentCF *= input[(*this)[i].dualIndex];
      output[(*this)[i].vIndex] += currentCF;
    }
  }
  void ActOnVectorROWSOnTheLeft(
    const List<Vector<coefficient> >& inputStandToTheLeftAsVectorRows, List<Vector<coefficient> >& output
  ) const {
    if (&inputStandToTheLeftAsVectorRows == &output) {
      List<Vector<coefficient> > inputCopy = inputStandToTheLeftAsVectorRows;
      this->ActOnVectorROWSOnTheLeft(inputCopy, output);
      return;
    }
    output.SetSize(this->GetMinNumRows());
    int numColsTarget = inputStandToTheLeftAsVectorRows[0].size;
    if (this->GetMinNumCols() != inputStandToTheLeftAsVectorRows.size) {
      crash << "This is a programming error: attemtping to act by matrix "
      << this->ToString() << " (" << this->GetMinNumCols() << " columns) "
      << " on the " << inputStandToTheLeftAsVectorRows.size << " vector-rows: "
      << inputStandToTheLeftAsVectorRows.ToString() << ". "
      << crash;
    }
    for (int i = 0; i < inputStandToTheLeftAsVectorRows.size; i ++) {
      output[i].MakeZero(numColsTarget);
    }
    for (int i = 0; i < this->size(); i ++) {
      for (int j = 0; j < numColsTarget; j ++) {
        output[(*this)[i].vIndex][j] += inputStandToTheLeftAsVectorRows[(*this)[i].dualIndex][j] * this->theCoeffs[i];
      }
    }
  }
  template <class otherType>
  void ActOnVectorsColumn(Vectors<otherType>& inputOutput) const {
    for (int i = 0; i < inputOutput.size; i ++) {
      this->ActOnVectorColumn(inputOutput[i], inputOutput[i]);
    }
  }
  template <class otherType>
  void ActOnVectorColumn(VectorSparse<otherType>& inputOutput) const {
    VectorSparse<otherType> output;
    output.MakeZero();
    otherType tempElt;
    MonomialVector tempVM;
    for (int i = 0; i < this->size(); i ++) {
      for (int j = 0; j < inputOutput.size(); j ++) {
        if ((*this)[i].dualIndex == inputOutput[j].theIndex) {
          tempVM.theIndex = (*this)[i].vIndex;
          tempElt = this->theCoeffs[i];
          tempElt *= inputOutput.theCoeffs[j];
          output.AddMonomial(tempVM, tempElt);
        }
      }
    }
    inputOutput = output;
  }
  unsigned int HashFunction() const {
    return this->::MonomialCollection<MonomialMatrix, coefficient>::HashFunction();
  }
  static unsigned int HashFunction(const MatrixTensor<coefficient>& input) {
    return input.HashFunction();
  }
  template<class otherClass>
  void operator=(const otherClass& other) {
    this->::MonomialCollection<MonomialMatrix, coefficient>::operator=(other);
  }
  MatrixTensor<coefficient> operator+(const MatrixTensor<coefficient>& rightM) const {
    MatrixTensor output;
    output = *this;
    output += rightM;
    return output;
  }
};

template <class coefficient>
void MatrixTensor<coefficient>::GetVectorsSparseFromRowsIncludeZeroRows(
  List<VectorSparse<coefficient> >& output, int MinNumRows
) {
  MinNumRows = MathRoutines::Maximum(MinNumRows, this->GetMinNumRows());
  output.SetSize(MinNumRows);
  for (int i = 0; i < output.size; i ++) {
    output[i].MakeZero();
  }
  for (int i = 0; i < this->size(); i ++) {
    int rowIndex = (*this)[i].vIndex;
    int colIndex = (*this)[i].dualIndex;
    output[rowIndex].AddMonomial(MonomialVector(colIndex), this->theCoeffs[i]);
  }
}

template <class coefficient>
void MatrixTensor<coefficient>::GaussianEliminationByRowsMatrix(MatrixTensor<coefficient>* carbonCopyMat) {
  List<VectorSparse<coefficient> > theRows, theCarbonCopyRows;
  int numRows = this->GetMinNumRows();
  if (carbonCopyMat != 0) {
    numRows = MathRoutines::Maximum(numRows, carbonCopyMat->GetMinNumRows());
    carbonCopyMat->GetVectorsSparseFromRowsIncludeZeroRows(theCarbonCopyRows, numRows);
  }
  this->GetVectorsSparseFromRowsIncludeZeroRows(theRows, numRows);
  List<VectorSparse<coefficient> >* theCarbonCopyPointer = carbonCopyMat == 0 ? 0 : &theCarbonCopyRows;
  VectorSparse<coefficient>::GaussianEliminationByRows(theRows, 0, 0, 0, theCarbonCopyPointer);
  this->AssignVectorsToRows(theRows);
  if (carbonCopyMat != 0) {
    carbonCopyMat->AssignVectorsToRows(theCarbonCopyRows);
  }
}

template <class coefficient>
void MatrixTensor<coefficient>::DirectSumWith(const MatrixTensor<coefficient>& other) {
  if (&other == this) {
    MatrixTensor<coefficient> otherCopy = other;
    this->DirectSumWith(otherCopy);
    return;
  }
  int indexShift = this->GetMinNumColsNumRows();
  this->SetExpectedSize(this->size() + other.size());
  MonomialMatrix currentM;
  for (int i = 0; i < other.size(); i ++) {
    currentM.vIndex = other[i].vIndex + indexShift;
    currentM.dualIndex = other[i].dualIndex + indexShift;
    this->AddMonomial(currentM, other.theCoeffs[i]);
  }
}

template <class coefficient>
void MatrixTensor<coefficient>::Invert() {
  MatrixTensor<coefficient> theId;
  theId.MakeId(this->GetMinNumColsNumRows());
  MatrixTensor<coefficient> result = theId;
  this->GaussianEliminationByRowsMatrix(&result);
  if (*this != theId) {
    crash << "This is a programming error: attempting to invert a "
    << "non-invertable matrix tensor. After Gaussian elimination, the matrix equals "
    << this->ToStringMatForm() << " but should instead be equal to " << theId.ToStringMatForm() << crash;
  }
  *this = result;
}

template<class coefficient>
class MonomialGeneralizedVerma {
  public:
  ModuleSSalgebra<coefficient>* owner;
  MonomialUniversalEnveloping<coefficient> theMonCoeffOne;
  int indexFDVector;
  MonomialGeneralizedVerma(): owner(nullptr), indexFDVector(- 1) {
  }
  friend std::ostream& operator << (std::ostream& output, const MonomialGeneralizedVerma<coefficient>& theGen) {
    output << theGen.ToString();
    return output;
  }
  void MultiplyMeByUEEltOnTheLefT(
    const ElementUniversalEnveloping<coefficient>& theUE,
    ElementSumGeneralizedVermas<coefficient>& output
  ) const;
  void operator=(const MonomialGeneralizedVerma<coefficient>& other) {
    this->owner = other.owner;
    this->indexFDVector = other.indexFDVector;
    this->theMonCoeffOne = other.theMonCoeffOne;
  }

  std::string ToString(FormatExpressions* theFormat = nullptr, bool includeV = true) const;
  static bool IsMonEqualToZero() {
    return false;
  }
  bool operator==(const MonomialGeneralizedVerma<coefficient>& other) const {
    if (this->indexFDVector == other.indexFDVector && this->owner == other.owner) {
      return this->theMonCoeffOne == other.theMonCoeffOne;
    }
    return false;
  }
  void SetNumVariables(int GoalNumVars) {
    if (this->owner->size <= this->indexInOwner) {
      crash << "Crash in SetNumVariables: bad number of variables. " << crash;
    }
    this->theMonCoeffOne.SetNumVariables(GoalNumVars);
    this->owner->TheObjects[this->indexInOwner].SetNumVariables(GoalNumVars);
  }
  void Substitution(
    const PolynomialSubstitution<Rational>& theSub, ListReferences<ModuleSSalgebra<coefficient> >& theMods
  );
  unsigned int HashFunction() const {
    return this->indexFDVector * SomeRandomPrimes[0] + (static_cast<unsigned int>(reinterpret_cast<uintptr_t>(this->owner))) * SomeRandomPrimes[1];
  }
  static unsigned int HashFunction(const MonomialGeneralizedVerma<coefficient>& input) {
    return input.HashFunction();
  }
  bool operator>(const MonomialGeneralizedVerma<coefficient>& other) {
    if (this->owner != other.owner) {
      // use of ulong is correct on i386, amd64, and a number of other popular platforms
      // uintptr_t is only available in c++ 0x
      return reinterpret_cast<unsigned long>(this->owner) > reinterpret_cast<unsigned long>(other.owner);
    }
    if (this->indexFDVector != other.indexFDVector) {
      return this->indexFDVector > other.indexFDVector;
    }
    return this->theMonCoeffOne > other.theMonCoeffOne;
  }
  void ReduceMe(ElementSumGeneralizedVermas<coefficient>& output) const;
  bool IsHWV() const {
    if (!this->theMonCoeffOne.IsEqualToOne()) {
      return false;
    }
    return this->GetOwner().GetDim() - 1 == this->indexFDVector;
  }
  void MakeConst(ModuleSSalgebra<coefficient>& inputOwner) {
    this->owner = &inputOwner;
    this->theMonCoeffOne.MakeOne(*inputOwner.owner);
  }
  ModuleSSalgebra<coefficient>& GetOwner() const {
    return *this->owner;
  }
};

template<class coefficient>
class ElementSumGeneralizedVermas : public MonomialCollection<MonomialGeneralizedVerma<coefficient>, coefficient> {
public:
  void MultiplyMeByUEEltOnTheLeft(const ElementUniversalEnveloping<coefficient>& theUE);
  unsigned int HashFunction() const {
    return this->MonomialCollection<MonomialGeneralizedVerma<coefficient>, coefficient >::HashFunction();
  }
  static unsigned int HashFunction(const ElementSumGeneralizedVermas<coefficient>& input) {
    return input.HashFunction();
  }
  ElementSumGeneralizedVermas() {
  }
  void MakeHWV(ModuleSSalgebra<coefficient>& theOwner, const coefficient& theRingUnit);
  int GetNumVars() {
    if (this->owner == nullptr) {
      return - 1;
    }
    if (this->owner->size == 0) {
      return - 1;
    }
    int theAnswer = this->owner->TheObjects[0].GetNumVars();
    for (int i = 1; i < this->owner->size; i ++) {
      if (theAnswer != this->owner->TheObjects[i].GetNumVars()) {
        return - 1;
      }
    }
    return theAnswer;
  }
  bool ExtractElementUE(ElementUniversalEnveloping<coefficient>& output, SemisimpleLieAlgebra& theOwner);
  void operator=(const ElementSumGeneralizedVermas<coefficient>& other) {
    this->::MonomialCollection<MonomialGeneralizedVerma<coefficient>, coefficient>::operator=(other);
  }
};

template <class coefficient>
class MonomialTensorGeneralizedVermas {
public:
  List<MonomialGeneralizedVerma<coefficient> > theMons;
  void SimplifyNormalizeCoeffs();
  friend std::ostream& operator << (std::ostream& output, const MonomialTensorGeneralizedVermas<coefficient>& input) {
    output << input.ToString();
    return output;
  }
  static bool IsMonEqualToZero() {
    return false;
  }
  int GetNumVars() {
    return this->Coefficient.GetNumVars();
  }
  void operator*=(const MonomialTensorGeneralizedVermas<coefficient>& other) {
    if (this == &other) {
      MonomialTensorGeneralizedVermas<coefficient> tempMon1;
      tempMon1 = other;
      *this *= tempMon1;
      return;
    }
    this->theMons.AddListOnTop(other.theMons);
  }
  void operator*=(const MonomialGeneralizedVerma<coefficient>& other) {
    this->theMons.AddOnTop(other);
  }
  unsigned int HashFunction() const {
    int numCycles = MathRoutines::Minimum(SomeRandomPrimesSize, this->theMons.size);
    unsigned int result = 0;
    for (int i = 0; i < numCycles; i ++) {
      result += SomeRandomPrimes[i] * this->theMons[i].HashFunction();
    }
    return result;
  }
  static unsigned int HashFunction(const MonomialTensorGeneralizedVermas<coefficient>& input) {
    return input.HashFunction();
  }
  void SetNumVariables(int GoalNumVars) {
    for (int i = 0; i < this->theMons.size; i ++) {
      this->theMons[i].SetNumVariables(GoalNumVars);
    }
  }
  void Substitution(
    const PolynomialSubstitution<Rational>& theSub,
    ListReferences<ModuleSSalgebra<coefficient> >& theMods
  ) {
    for (int i = 0; i < this->theMons.size; i ++) {
      this->theMons[i].Substitution(theSub, theMods);
    }
  }
  std::string ToString(FormatExpressions* theFormat = nullptr, bool includeV = true) const;
  MonomialTensorGeneralizedVermas() {
  }
  void operator=(const MonomialTensorGeneralizedVermas<coefficient>& other) {
    this->theMons = other.theMons;
  }
  void operator=(const MonomialGeneralizedVerma<coefficient>& other);
  bool operator==(const MonomialTensorGeneralizedVermas<coefficient>& other) const {
    if (this->theMons.size != other.theMons.size) {
      return false;
    }
    for (int i = 0; i < this->theMons.size; i ++) {
      if (!(this->theMons[i] == other.theMons[i])) {
        return false;
      }
    }
    return true;
  }
  bool IsHWV() const {
    if (this->theMons.size != 1) {
      return false;
    }
    return this->theMons[0].IsHWV();
  }
  bool operator>(const MonomialTensorGeneralizedVermas<coefficient>& other) const {
    if (this->theMons.size > other.theMons.size) {
      return true;
    }
    if (other.theMons.size > this->theMons.size) {
      return false;
    }
    ///This might need a rewrite. As it is, it will cause monomials to be sorted according to the
    ///alphabetical order of their human-readable strings. If I have time, I will make a better scheme for
    ///comparison.
    return this->ToString() > other.ToString();
  }
};

template <class coefficient>
void MonomialTensorGeneralizedVermas <coefficient>::SimplifyNormalizeCoeffs() {
  if (this->Coefficient.IsEqualToZero()) {
    this->MakeZero(this->Coefficient.GetZero());
    return;
  }
  for (int i = 0; i < this->theMons.size; i ++) {
    coefficient& currentCF = this->theMons[i].Coefficient;
    if (currentCF.IsEqualToZero()) {
      this->MakeZero(this->Coefficient.GetZero());
      return;
    }
    this->Coefficient *= currentCF;
    currentCF = currentCF.GetOne();
  }
}

class PolynomialOverModule;

class slTwoInSlN {
  int GetModuleIndexFromHighestWeightVector(const Matrix<Rational>& input) {
    Rational tempRat;
    for (int i = 0; i < this->theHighestWeightVectors.size; i ++) {
      if (this->theHighestWeightVectors.TheObjects[i].IsProportionalTo(input, tempRat)) {
        return i;
      }
    }
    return - 1;
  }
public:
  int theDimension;
  Matrix<Rational> theH;
  Matrix<Rational> theE;
  Matrix<Rational> theF;
  List<int> thePartition;
  List<Matrix<Rational> > theProjectors;
  List<Matrix<Rational> > theHighestWeightVectors;
  List<List<Matrix<Rational> > > theGmodKModules;
  List<List<List<int> > > PairingTable;
  void GetIsPlusKIndexingFrom(int input, int& s, int& k);
  std::string ElementMatrixToTensorString(const Matrix<Rational> & input, bool useHtml);
  std::string initFromModuleDecomposition(List<int>& decompositionDimensions, bool useHtml, bool computePairingTable);
  std::string initPairingTable(bool useHtml);
  std::string ElementModuleIndexToString(int input, bool useHtml);
  std::string GetNotationString(bool useHtml);
  bool ComputeInvariantsOfDegree(
    List<int>& decompositionDimensions, int theDegree, List<Polynomial<Rational> >& output, std::string& outputError
  );
  std::string PairTwoIndices(List<int>& output, int leftIndex, int rightIndex, bool useHtml);
  void ExtractHighestWeightVectorsFromVector(
    Matrix<Rational>& input,
    List<Matrix<Rational> >& outputDecompositionOfInput,
    List<Matrix<Rational> >& outputTheHWVectors
  );
  void ClimbDownFromHighestWeightAlongSl2String(
    Matrix<Rational>& input, Matrix<Rational>& output, Rational& outputCoeff, int generatorPower
  );
  void ClimbUpFromVector(
    Matrix<Rational>& input, Matrix<Rational>& outputLastNonZero, int& largestPowerNotKillingInput
  );
};

template<typename coefficient>
void Matrix<coefficient>::GetVectorFromRow(int rowIndex, Vector<coefficient>& output) const {
  output.SetSize(this->NumCols);
  for (int i = 0; i < this->NumCols; i ++) {
    output.TheObjects[i] = this->elements[rowIndex][i];
  }
}

template<typename coefficient>
void Matrix<coefficient>::GetVectorFromColumn(int colIndex, Vector<coefficient>& output) const {
  output.SetSize(this->NumRows);
  for (int i = 0; i < this->NumRows; i ++) {
    output[i] = this->elements[i][colIndex];
  }
}

class KLpolys: public HashedList<Vector<Rational> > {
public:
  WeylGroupData* TheWeylGroup;
  List<int> TheMultiplicities;
  List<bool> Explored;
  int NextToExplore;
  int LowestNonExplored;
  List<List<int> > BruhatOrder;
  List<List<int> > SimpleReflectionsActionList;
  List<List<int> > InverseBruhatOrder;
  std::string DebugString;
  List<List<Polynomial<Rational> > > theKLPolys;
  List<List<Polynomial<Rational> > > theRPolys;
  List<List<Rational> > theKLcoeffs;
  void KLcoeffsToString(List<int>& theKLCoeffs, std::string& output);
  void FindNextToExplore();
  int FindMinimalBruhatNonExplored(List<bool>& theExplored);
  int FindMaximalBruhatNonExplored(List<bool>& theExplored);
  void initTheMults();
  void Compute(int x);
  void Check();
  //returns true if computation succeeded, false otherwise
  bool ComputeRxy(int x, int y, int SimpleReflectionIndex);
  void ComputeKLxy(int x, int y);
  bool IsMaxNonEplored(int index);
  bool IndexGEQIndex(int a, int b);
  bool IndexGreaterThanIndex(int a, int b);
  void ComputeDebugString();
  std::string ToString(FormatExpressions* theFormat = nullptr);
  void MergeBruhatLists(int fromList, int toList);
  std::string KLPolysToString(FormatExpressions* theFormat = nullptr);
  void ComputeKLcoefficients();
  int ChamberIndicatorToIndex(Vector<Rational>& ChamberIndicator);
  std::string RPolysToString(FormatExpressions* theFormat = nullptr);
  bool ComputeKLPolys(WeylGroupData* theWeylGroup);
  void ComputeRPolys();
  int ComputeProductfromSimpleReflectionsActionList(int x, int y);
  void WriteKLCoeffsToFile(std::fstream& output, List<int>& KLcoeff, int TopIndex);
  //returns the TopIndex of the KL coefficients
  int ReadKLCoeffsFromFile(std::fstream& input, List<int>& output);
  KLpolys() {
    this->TheWeylGroup = nullptr;
  }
  void GeneratePartialBruhatOrder();
  void ExtendOrder();
  void ComputeFullBruhatOrder();
  void initFromWeyl(WeylGroupData* theWeylGroup);
};


template<class coefficient>
void Matrix<coefficient>::Substitution(const PolynomialSubstitution<Rational>& theSub) {
  for (int i = 0; i < this->NumRows; i ++) {
    for (int j = 0; j < this->NumCols; j ++) {
      this->elements[i][j].Substitution(theSub);
    }
  }
}

template <class coefficient>
std::string MonomialTensorGeneralizedVermas<coefficient>::ToString(FormatExpressions* theFormat, bool includeV) const {
  std::stringstream out;
  if (this->theMons.size > 1) {
    for (int i = 0; i < this->theMons.size; i ++) {
      bool ishwv = this->theMons[i].IsHWV();
      if (!ishwv) {
        out << "(";
      }
      out << this->theMons[i].ToString(theFormat, includeV);
      if (!ishwv) {
        out  << ")";
      }
      if (i != this->theMons.size - 1) {
        out << "\\otimes ";
      }
    }
  } else {
    out << this->theMons[0].ToString(theFormat, includeV);
  }
  return out.str();
}

template <class coefficient>
std::string MonomialGeneralizedVerma<coefficient>::ToString(FormatExpressions* theFormat, bool includeV) const {
  if (this->owner == nullptr) {
    crash << "This is a programming error: non-initialized generalized Verma monomial (owner is 0)." << crash;
  }
  ModuleSSalgebra<coefficient>& theMod = *this->owner;
  std::string tempS;
  if (tempS == "1") {
    tempS = "";
  }
  if (tempS == "- 1" || tempS == "-1") {
    tempS = "-";
  }
  tempS += this->theMonCoeffOne.ToString(theFormat);
  if (tempS == "1") {
    tempS = "";
  }
  if (tempS == "- 1" || tempS == "-1") {
    tempS = "-";
  }
  bool needsCdot = (tempS != "1" && tempS != "-" && tempS != "");
  std::stringstream out;
  out << tempS;
  tempS = theMod.theGeneratingWordsNonReduced[this->indexFDVector].ToString(theFormat);
  if (tempS != "1") {
    out << tempS;
  }
  needsCdot = needsCdot || (tempS != "1" && tempS != "-");
  if (needsCdot) {
    out << "\\cdot ";
  }
  if (includeV) {
    out << theMod.ElementToStringHWV(theFormat);
  }
  return out.str();
}

template <class templateMonomial, class coefficient>
std::ostream& operator<<(std::ostream& output, const MonomialCollection<templateMonomial, coefficient>& theCollection) {
  if (theCollection.size() == 0) {
    output << "0";
    return output;
  }
  std::string tempS1, tempS2;
  List<templateMonomial> sortedMons;
  sortedMons = theCollection.theMonomials;
  sortedMons.QuickSortDescending();
  int cutOffCounter = 0;
  for (int i = 0; i < sortedMons.size; i ++) {
    templateMonomial& currentMon = sortedMons[i];
    std::stringstream tempStream;
    coefficient& currentCoeff = theCollection.theCoeffs[theCollection.theMonomials.GetIndex(currentMon)];
    tempStream << currentCoeff;
    tempS1 = tempStream.str();
    tempS2 = currentMon.ToString();
    if (tempS1 == "1" && tempS2 != "1") {
      tempS1 = "";
    }
    if ((tempS1 == "- 1" || tempS1 == "-1") && tempS2 != "1") {
      tempS1 = "-";
    }
    if (tempS2 != "1") {
      tempS1 += tempS2;
    }
    if (i > 0) {
      if (tempS1.size() > 0) {
        if (tempS1[0] != '-') {
          output << "+";
          cutOffCounter += 1;
        }
      } else {
        output << "+";
        cutOffCounter += 1;
      }
    }
    output << tempS1;
    cutOffCounter += tempS1.size();
  }
  return output;
}

template <class coefficient>
void PolynomialSubstitution<coefficient>::MakeLinearSubConstTermsLastRow(Matrix<coefficient>& theMat) {
  this->SetSize(theMat.NumCols);
  MonomialP tempM;
  for (int i = 0; i < this->size; i ++) {
    this->TheObjects[i].MakeZero();
    for (int j = 0; j < theMat.NumRows - 1; j ++) {
      tempM.MakeOne(theMat.NumRows - 1);
      tempM[j] = 1;
      this->TheObjects[i].AddMonomial(tempM, theMat.elements[j][i]);
    }
    this->TheObjects[i] += theMat.elements[theMat.NumRows - 1][i];
  }
}

template <class coefficient>
void MonomialGeneralizedVerma<coefficient>::Substitution(
  const PolynomialSubstitution<Rational>& theSub, ListReferences<ModuleSSalgebra<coefficient> >& theMods
) {
  this->theMonCoeffOne.Substitution(theSub);
  ModuleSSalgebra<coefficient> newOwner;
  newOwner = *this->owner;
  newOwner.Substitution(theSub);
  int newModIndex = theMods.AddNoRepetitionOrReturnIndexFirst(newOwner);
  this->owner = &theMods[newModIndex];
}

template <class coefficient>
void MonomialTensorGeneralizedVermas<coefficient>::operator=(const MonomialGeneralizedVerma<coefficient>& other) {
  this->theMons.SetSize(1);
  this->theMons[0] = other;
}

template <class coefficient>
void MonomialGeneralizedVerma<coefficient>::MultiplyMeByUEEltOnTheLefT(
  const ElementUniversalEnveloping<coefficient>& theUE,
  ElementSumGeneralizedVermas<coefficient>& output
) const {
  MacroRegisterFunctionWithName("MonomialGeneralizedVerma<coefficient>::MultiplyMeByUEEltOnTheLefT");
  MonomialGeneralizedVerma<coefficient> currentMon;
  output.MakeZero();
  ElementSumGeneralizedVermas<coefficient> buffer;
  ProgressReport theReport;
  if (!this->GetOwner().owner->flagHasNilradicalOrder) {
    crash << "Calling generalized verma module simplification requires nilradical order on the generators. "
    << crash;
  }
  for (int j = 0; j < theUE.size(); j ++) {
    currentMon.theMonCoeffOne = theUE[j];
    currentMon.theMonCoeffOne *= this->theMonCoeffOne;
    currentMon.owner = this->owner;
    currentMon.indexFDVector = this->indexFDVector;
    currentMon.owner = this->owner;
    std::stringstream reportStream;
    reportStream << "reducing mon: " << currentMon.ToString()
    << ", index" << j + 1 << " out of " << theUE.size() << "...";
    theReport.Report(reportStream.str());
    currentMon.ReduceMe(buffer);
    reportStream << " done.";
    theReport.Report(reportStream.str());
    buffer *= theUE.theCoeffs[j];
    output += buffer;
  }
}

template <class coefficient>
void ElementSumGeneralizedVermas<coefficient>::MultiplyMeByUEEltOnTheLeft(
  const ElementUniversalEnveloping<coefficient>& theUE
) {
  MacroRegisterFunctionWithName("ElementSumGeneralizedVermas<coefficient>::MultiplyMeByUEEltOnTheLeft");
  ElementSumGeneralizedVermas<coefficient> buffer, Accum;
  Accum.MakeZero();
  for (int i = 0; i < this->size(); i ++) {
    (*this)[i].MultiplyMeByUEEltOnTheLefT(theUE, buffer);
    buffer *= this->theCoeffs[i];
    Accum += buffer;
  }
  *this = Accum;
}

template <class coefficient>
void MonomialGeneralizedVerma<coefficient>::ReduceMe(
  ElementSumGeneralizedVermas<coefficient>& output
) const {
  MacroRegisterFunctionWithName("MonomialGeneralizedVerma::ReduceMe");
  ModuleSSalgebra<coefficient>& theMod = *this->owner;
  output.MakeZero();
  MonomialUniversalEnveloping<coefficient> tempMon;
  tempMon = this->theMonCoeffOne;
  tempMon *= theMod.theGeneratingWordsNonReduced[this->indexFDVector];
  int indexCheck = theMod.theGeneratingWordsNonReduced.GetIndex(tempMon);
  if (!this->owner->owner->flagHasNilradicalOrder) {
    crash << "Owner needs nilradical order!!!" << crash;
  }
  if (indexCheck != - 1) {
    MonomialGeneralizedVerma<coefficient> basisMon;
    basisMon.MakeConst(*this->owner);
    basisMon.indexFDVector = indexCheck;
    output.AddMonomial(basisMon, 1);
    return;
  }
  ElementUniversalEnveloping<coefficient> theUEelt;
  theUEelt.MakeZero(*this->GetOwner().owner);
  theUEelt.AddMonomial(this->theMonCoeffOne, 1);
  theUEelt.Simplify();

  MonomialUniversalEnveloping<coefficient> currentMon;
  MonomialGeneralizedVerma<coefficient> newMon;
  MatrixTensor<coefficient> tempMat1, tempMat2;
  ProgressReport theReport;
  coefficient theCF;
  for (int l = 0; l < theUEelt.size(); l ++) {
    currentMon = theUEelt[l];
    tempMat1.MakeIdSpecial();
    for (int k = currentMon.Powers.size - 1; k >= 0; k --) {
      std::stringstream reportStream;
      reportStream << "accounting monomial " << currentMon.ToString() << " of index "
      << l + 1 << " out of "
      << theUEelt.size() << " and letter index " << currentMon.Powers.size - k
      << " out of " << currentMon.Powers.size << "...";
      theReport.Report(reportStream.str());
      int thePower = - 1;
      if (!currentMon.Powers[k].IsSmallInteger(&thePower)) {
        break;
      }
      int theIndex = currentMon.generatorsIndices[k];
      if (theMod.HasFreeAction(theIndex)) {
        break;
      }
      tempMat2 = tempMat1;
      tempMat1 = theMod.GetActionGeneratorIndeX(theIndex);
      tempMat1.RaiseToPower(thePower);
      tempMat1 *= tempMat2;
      currentMon.Powers.size --;
      currentMon.generatorsIndices.size --;
      reportStream << "done!";
      theReport.Report(reportStream.str());
    }
    newMon.owner = this->owner;
    for (int i = 0; i < tempMat1.size(); i ++) {
      int otherIndex = - 1;
      if (tempMat1[i].dualIndex == this->indexFDVector) {
        otherIndex = tempMat1[i].vIndex;
      }
      if (tempMat1[i].IsId) {
        otherIndex = this->indexFDVector;
      }
      if (otherIndex != - 1) {
        newMon.theMonCoeffOne = currentMon;
        newMon.indexFDVector = otherIndex;
        theCF = theUEelt.theCoeffs[l];
        theCF *= tempMat1.theCoeffs[i];
        output.AddMonomial(newMon, theCF);
      }
    }
  }
  theMod.GetOwner().OrderStandardAscending();
}

template<class coefficient>
void Vectors<coefficient>::IntersectTwoLinSpaces(
  const List<Vector<coefficient> >& firstSpace,
  const List<Vector<coefficient> >& secondSpace,
  List<Vector<coefficient> >& output
) {
  Vectors<coefficient> firstReduced, secondReduced;
  Selection tempSel;
  Vectors<coefficient>::SelectABasisInSubspace(firstSpace, firstReduced, tempSel);
  Vectors<coefficient>::SelectABasisInSubspace(secondSpace, secondReduced, tempSel);
  if (firstReduced.size == 0 || secondReduced.size == 0) {
    output.size = 0;
    return;
  }
  int theDim = firstReduced[0].size;
  Matrix<coefficient> theMat;
  theMat.init(theDim, firstReduced.size + secondReduced.size);
  for (int i = 0; i < theDim; i ++) {
    for (int j = 0; j < firstReduced.size; j ++) {
      theMat(i, j) = firstReduced[j][i];
    }
    for (int j = 0; j < secondReduced.size; j ++) {
      theMat(i, firstReduced.size + j) = 0;
      theMat(i, firstReduced.size + j) -= secondReduced[j][i];
    }
  }
  theMat.GaussianEliminationByRows(0, &tempSel);
  output.Reserve(tempSel.CardinalitySelection);
  output.size = 0;
  Vector<coefficient> nextIntersection;
  for (int i = 0; i < tempSel.CardinalitySelection; i ++) {
    int currentIndex = tempSel.elements[i];
    if (currentIndex < firstReduced.size) {
      crash << "Unexpected condition in Vectors::IntersectTwoLinSpaces. " << crash;
    }
    nextIntersection.MakeZero(theDim);
    for (int j = 0; j < firstReduced.size; j ++) {
      if (!tempSel.selected[j]) {
        nextIntersection += firstReduced[j] * theMat.elements[j][currentIndex];
      }
    }
    output.AddOnTop(nextIntersection);
  }
}

template <class coefficient>
void ElementSumGeneralizedVermas<coefficient>::MakeHWV(
  ModuleSSalgebra<coefficient>& theOwner, const coefficient& theRingUnit
) {
  this->MakeZero();
  MonomialGeneralizedVerma<coefficient> theMon;
  theMon.indexFDVector = theOwner.theGeneratingWordsNonReduced.size - 1;
  theMon.theMonCoeffOne.MakeOne(theOwner.GetOwner());
  theMon.owner = &theOwner;
  this->AddMonomial(theMon, theRingUnit);
}

template<class coefficient>
bool Matrix<coefficient>::IsPositiveDefinite() {
  if (this->NumRows != this->NumCols) {
    crash << "This is a programming error: attempting to evaluate whether a matrix "
    << "is positive definite, but the matrix is not square. " << crash;
  }
  coefficient det;
  Matrix<coefficient> tempMat;
  for (int i = 0; i < this->NumRows; i ++) {
    tempMat.init(i + 1, i + 1);
    for (int j = 0; j < tempMat.NumCols; j ++) {
      for (int k = 0; k < tempMat.NumCols; k ++) {
        tempMat.elements[j][k] = this->elements[j][k];
      }
    }
    det = tempMat.GetDeterminant();
    if (det <= 0) {
      return false;
    }
  }
  return true;
}

template <class coefficient>
bool ElementSemisimpleLieAlgebra<coefficient>::GetCoordsInBasis(
  const List<ElementSemisimpleLieAlgebra>& theBasis, Vector<coefficient>& output
) const {
  if (theBasis.size == 0) {
    return false;
  }
  if (this->IsEqualToZero()) {
    output.MakeZero(theBasis.size);
    return true;
  }
  MacroRegisterFunctionWithName("ElementSemisimpleLieAlgebra::GetCoordsInBasis");
  Vectors<coefficient> tempBasis;
  Vector<coefficient> tempRoot;
  tempBasis.SetSize(theBasis.size);
  for (int i = 0 ; i < theBasis.size; i ++) {
    theBasis[i].ElementToVectorNegativeRootSpacesFirst(tempBasis[i]);
  }
  this->ElementToVectorNegativeRootSpacesFirst(tempRoot);
  return tempRoot.GetCoordsInBasiS(tempBasis, output);
}

template <class coefficient>
void ElementSemisimpleLieAlgebra<coefficient>::MakeGenerator(int generatorIndex, SemisimpleLieAlgebra& inputOwner) {
  this->MakeZero();
  ChevalleyGenerator tempGenerator;
  tempGenerator.MakeGenerator(inputOwner, generatorIndex);
  this->AddMonomial(tempGenerator, 1);
}

template <class coefficient>
std::string Vectors<coefficient>::ElementsToInequalitiesString(
  bool useLatex, bool useHtml, bool LastVarIsConstant, FormatExpressions& theFormat
) const {
  std::stringstream out;
  std::string tempS;
  if (useLatex) {
    out << "\\begin{array}{l}";
  }
  for (int i = 0; i < this->size; i ++) {
    Vector<Rational>& current = (*this)[i];
    tempS = current.ToStringLetterFormat(theFormat.polyDefaultLetter, &theFormat, LastVarIsConstant);
    if (tempS == "") {
      out << "(0";
    }
    out << tempS;
    if (!LastVarIsConstant) {
      if (useLatex) {
        out << "\\geq 0\\\\";
      } else {
        out << "=>0\n";
      }
    } else {
      if (useLatex) {
        out << "\\geq " << (- (*current.LastObject())).ToString() << "\\\\";
      } else {
        out << "=>" <<  (- (*current.LastObject())).ToString();
      }
    }
    if (tempS == "") {
      out << ")";
    }
    if (useHtml) {
      out << "<br>";
    } else {
      out << "\n";
    }
  }
  if (useLatex) {
    out << "\\end{array}";
  }
  return out.str();
}
#endif
