#ifndef header_math_general_ALREADY_INCLUDED
#define header_math_general_ALREADY_INCLUDED

#include "general_logging_global_variables.h"
#include "general_strings.h"
#include "math_large_integers.h"
#include "math_mathml.h"
#include "math_subsets_selections.h"
#include "math_vectors.h"

class AlgebraicClosureRationals;
template <class Coefficient>
class MatrixTensor;

template <
  class Coefficient,
  unsigned int inputHashFunction(const Coefficient&) = Coefficient::
  hashFunction
>
class MonomialTensor {
  friend std::ostream& operator<<(
    std::ostream& output,
    const MonomialTensor<Coefficient, inputHashFunction>& monomial
  ) {
    return output << monomial.toString();
  }
private:
public:
  List<int> generatorsIndices;
  List<Coefficient> powers;
  bool flagDeallocated;
  std::string toString(FormatExpressions* format = nullptr) const;
  bool isEqualToOne() const {
    return this->generatorsIndices.size == 0;
  }
  void operator=(List<int>& other) {
    this->generatorsIndices.reserve(other.size);
    this->powers.reserve(other.size);
    this->makeConstant();
    for (int i = 0; i < other.size; i ++) {
      this->multiplyByGeneratorPowerOnTheRight(other[i], 1);
    }
  }
  bool checkConsistency() {
    if (this->flagDeallocated) {
      global.fatal << "Use after free of MonomialTensor." << global.fatal;
    }
    return true;
  }
  void operator=(const MonomialTensor<Coefficient, inputHashFunction>& other) {
    this->generatorsIndices = other.generatorsIndices;
    this->powers = other.powers;
  }
  MonomialTensor() {
    this->flagDeallocated = false;
  }
  ~MonomialTensor() {
    this->flagDeallocated = true;
  }
  int minimalNumberOfVariables() {
    int result = 0;
    for (int i = 0; i < this->powers.size; i ++) {
      result =
      MathRoutines::maximum(
        result, this->powers[i].minimalNumberOfVariables()
      );
    }
    return result;
  }
  template <class otherType>
  void operator=(const MonomialTensor<otherType>& other) {
    this->generatorsIndices = other.generatorsIndices;
    this->powers.setSize(other.powers.size);
    for (int i = 0; i < other.powers.size; i ++) {
      this->powers[i] = other.powers[i];
    }
  }
  bool simplifyEqualConsecutiveGenerators(int lowestNonReducedIndex);
  void multiplyByGeneratorPowerOnTheRight(
    int generatorIndex, const Coefficient& power
  );
  void multiplyByGeneratorPowerOnTheLeft(
    int generatorIndexStandsToTheLeft, const Coefficient& power
  );
  unsigned int hashFunction() const {
    unsigned int result = 0;
    int j = 0;
    for (int i = 0; i < this->generatorsIndices.size; i ++) {
      result +=
      HashConstants::getConstantIncrementCounter(j) *
      this->generatorsIndices[i] +
      HashConstants::getConstantIncrementCounter(j) *
      inputHashFunction(this->powers[i]);
    }
    return result;
  }
  static unsigned int hashFunction(
    const MonomialTensor<Coefficient, inputHashFunction>& input
  ) {
    return input.hashFunction();
  }
  void makeConstant() {
    this->generatorsIndices.size = 0;
    this->powers.size = 0;
  }
  bool operator>(const MonomialTensor<Coefficient, inputHashFunction>& other)
  const {
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
      if (other.powers[i] > this->powers[i]) {
        return false;
      }
      if (this->powers[i] > other.powers[i]) {
        return true;
      }
    }
    return false;
  }
  bool operator==(const MonomialTensor<Coefficient, inputHashFunction>& other)
  const {
    return
    this->powers == other.powers &&
    this->generatorsIndices == other.generatorsIndices;
  }
  bool operator<(const MonomialTensor<Coefficient, inputHashFunction>& right)
  const {
    Coefficient leftrank = 0;
    for (int i = 0; i < this->powers.size; i ++) {
      leftrank += this->powers[i];
    }
    Coefficient rightrank = 0;
    for (int i = 0; i < right.powers.size; i ++) {
      rightrank += right.powers[i];
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
      if (this->powers[i] < right.powers[i]) {
        return false;
      }
      if (right.powers[i] < this->powers[i]) {
        return true;
      }
    }
    return false;
  }
  void operator*=(
    const MonomialTensor<Coefficient, inputHashFunction>& standsOnTheRight
  ) {
    if (standsOnTheRight.generatorsIndices.size == 0) {
      return;
    }
    if (this == &standsOnTheRight) {
      MonomialTensor<Coefficient, inputHashFunction> monomial;
      monomial = standsOnTheRight;
      (*this) *= (monomial);
      return;
    }
    this->generatorsIndices.setExpectedSize(
      standsOnTheRight.generatorsIndices.size + this->generatorsIndices.size
    );
    this->powers.setExpectedSize(
      standsOnTheRight.generatorsIndices.size + this->generatorsIndices.size
    );
    int firstIndex = standsOnTheRight.generatorsIndices[0];
    int i = 0;
    if (this->generatorsIndices.size > 0) {
      if (firstIndex == (*this->generatorsIndices.lastObject())) {
        *this->powers.lastObject() += standsOnTheRight.powers[0];
        i = 1;
      }
    }
    for (; i < standsOnTheRight.generatorsIndices.size; i ++) {
      this->powers.addOnTop(standsOnTheRight.powers[i]);
      this->generatorsIndices.addOnTop(standsOnTheRight.generatorsIndices[i]);
    }
  }
};

template <
  typename Type,
  unsigned int objectHashFunction(const Type&) = HashFunctions::hashFunction<
    Type
  >
>
class MonomialWrapper {
public:
  Type content;
  MonomialWrapper() {}
  MonomialWrapper(const Type& input) {
    this->content = input;
  }
  std::string toString(const FormatExpressions* format = nullptr) const {
    (void) format;
    std::stringstream out;
    out << "(" << this->content << ")";
    return out.str();
  }
  bool operator>(const MonomialWrapper& other) const {
    return this->content > other.content;
  }
  friend std::ostream& operator<<(
    std::ostream& output, const MonomialWrapper& monomial
  ) {
    output << monomial.content;
    return output;
  }
  static unsigned int hashFunction(const MonomialWrapper& input) {
    return objectHashFunction(input.content);
  }
  bool operator==(const MonomialWrapper& other) const {
    return this->content == other.content;
  }
};

template <typename Coefficient>
std::ostream& operator<<(
  std::ostream& output, const Matrix<Coefficient>& matrix
);
template <typename Coefficient>
class Matrix {
  // friend std::iostream& operator<< <Coefficient>(std::iostream& output,
  // const
  // Matrix<Coefficient>& matrix);
  friend std::ostream& operator<<<Coefficient>(
    std::ostream& output, const Matrix<Coefficient>& matrix
  );
  // friend std::iostream& operator>> <Coefficient>(std::iostream& input,
  // Matrix<Coefficient>& matrix);
public:
  int numberOfRows;
  int actualNumberOfRows;
  int numberOfColumns;
  int actualNumberOfColumns;
  Coefficient** elements;
  bool flagDeallocated;
  void initialize(int desiredNumberOfRows, int desiredNumberOfColumns);
  void releaseMemory();
  bool isPositiveDefinite();
  bool isNonNegativeAllEntries() {
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        if (this->elements[i][j] < 0) {
          return false;
        }
      }
    }
    return true;
  }
  void resize(
    int desiredRows,
    int desiredColumns,
    bool preserveValues,
    const Coefficient* ringZero = nullptr
  );
  Matrix():
  numberOfRows(0),
  actualNumberOfRows(0),
  numberOfColumns(0),
  actualNumberOfColumns(0),
  elements(nullptr),
  flagDeallocated(false) {}
  Matrix(const Matrix<Coefficient>& other):
  numberOfRows(0),
  actualNumberOfRows(0),
  numberOfColumns(0),
  actualNumberOfColumns(0),
  elements(0),
  flagDeallocated(false) {
    *this = other;
  }
  ~Matrix() {
    this->releaseMemory();
    this->flagDeallocated = true;
  }
  void transpose() {
    if (this->numberOfColumns == this->numberOfRows) {
      for (int i = 0; i < this->numberOfRows; i ++) {
        for (int j = i + 1; j < this->numberOfColumns; j ++) {
          MathRoutines::swap<Coefficient>(
            this->elements[j][i], this->elements[i][j]
          );
        }
      }
      return;
    }
    Matrix<Coefficient> matrix;
    matrix.initialize(this->numberOfColumns, this->numberOfRows);
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        matrix.elements[j][i] = this->elements[i][j];
      }
    }
    *this = matrix;
  }
  void appendMatrixOnTheRight(const Matrix<Coefficient>& standsOnTheRight) {
    if (&standsOnTheRight == this) {
      Matrix<Coefficient> copyThis = *this;
      this->appendMatrixOnTheRight(copyThis);
      return;
    }
    if (standsOnTheRight.numberOfRows < this->numberOfRows) {
      Coefficient zero;
      zero = 0;
      Matrix<Coefficient> standsOnTheRightNew = standsOnTheRight;
      standsOnTheRightNew.resize(
        this->numberOfRows, standsOnTheRight.numberOfColumns, true, &zero
      );
      this->appendMatrixOnTheRight(standsOnTheRightNew);
      return;
    }
    if (this->numberOfRows < standsOnTheRight.numberOfRows) {
      Coefficient zero;
      zero = 0;
      this->resize(
        standsOnTheRight.numberOfRows, this->numberOfColumns, true, &zero
      );
    }
    int oldNumberOfColumns = this->numberOfColumns;
    this->resize(
      this->numberOfRows,
      standsOnTheRight.numberOfColumns + oldNumberOfColumns,
      true
    );
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = oldNumberOfColumns; j < this->numberOfColumns; j ++) {
        this->elements[i][j] =
        standsOnTheRight.elements[i][j - oldNumberOfColumns];
      }
    }
  }
  void appendRowToTheBottom(const Vector<Coefficient>& row) {
    Matrix<Coefficient> matrixRow;
    matrixRow.assignVectorRow(row);
    this->appendMatrixToTheBottom(matrixRow);
  }
  void appendMatrixToTheBottom(const Matrix<Coefficient>& standsBelow) {
    if (&standsBelow == this) {
      Matrix<Coefficient> copyThis = *this;
      this->appendMatrixToTheBottom(copyThis);
      return;
    }
    if (standsBelow.numberOfColumns < this->numberOfColumns) {
      Coefficient zero;
      zero = 0;
      Matrix<Coefficient> standsBelowNew = standsBelow;
      standsBelowNew.resize(
        standsBelow.numberOfRows, this->numberOfColumns, true, &zero
      );
      this->appendMatrixOnTheRight(standsBelowNew);
      return;
    }
    if (this->numberOfColumns < standsBelow.numberOfColumns) {
      Coefficient zero;
      zero = 0;
      this->resize(
        this->numberOfRows, standsBelow.numberOfColumns, true, &zero
      );
    }
    // So far, we have guaranteed that this and
    // &standsBelow have the same number of columns and
    // are different objects.
    int oldNumberOfRows = this->numberOfRows;
    this->resize(
      this->numberOfRows + standsBelow.numberOfRows,
      this->numberOfColumns,
      true
    );
    for (int i = oldNumberOfRows; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        this->elements[i][j] = standsBelow(i - oldNumberOfRows, j);
      }
    }
  }
  bool checkConsistency() const {
    if (this->flagDeallocated) {
      global.fatal << "Use after free of Matrix. " << global.fatal;
    }
    return true;
  }
  // Interprets a square n by n matrix as a linear operator and makes a basis
  // change.
  // Let the matrix is M and the newBasis is the vector-columns v_1, ..., v_d
  // [the dimension d need not equal n].
  // Assume that M preserves the vector space generated by v_1, ..., v_d.
  // In other words, assume the linear span of
  // Mv_1, ..., Mv_d is contained in the
  // linear span of v_1, ..., v_d.
  // Let the output of this function be the d by d matrix B
  // with j^th vector column
  // b_{1,j}, ..., b_{d,j}.
  // Then Bv_j equals b_{1,j}v_1 + ... + b_{d, j}v_d.
  //
  // If the matrix M does not preserves the vector space generated by
  // v_1, ..., v_d, the function will return false.
  //
  // Math note: here's how we compute B.
  // Let A be the d x d matrix with columns A=[M v_1, ..., M v_d].
  // Let G be the gram matrix whose i,j^th entry equals the scalar product
  // <v_i, v_j>. Then B= G^{-1} A.
  static bool changeBasis(
    const Matrix<Coefficient>& linearOperator,
    const List<Vector<Coefficient> >& newBasis,
    Matrix<Coefficient>& output,
    std::stringstream* commentsOnFailure
  );
  // Interprets the matrix as a linear operator and makes a basis change.
  bool changeBasis(
    const List<Vector<Coefficient> >& newBasis,
    Matrix<Coefficient>& output,
    std::stringstream* commentsOnFailure
  ) const;
  void computeDeterminantOverwriteMatrix(
    Coefficient& output,
    const Coefficient& ringOne = 1,
    const Coefficient& ringZero = 0
  );
  void actOnVectorROWSOnTheLeft(
    List<Vector<Coefficient> >& standOnTheRightAsVectorRow
  ) const {
    List<Vector<Coefficient> > output;
    this->actOnVectorROWSOnTheLeft(standOnTheRightAsVectorRow, output);
    standOnTheRightAsVectorRow = output;
  }
  void actOnVectorROWSOnTheLeft(
    List<Vector<Coefficient> >& standOnTheRightAsVectorRow,
    List<Vector<Coefficient> >& output
  ) const {
    if (this->numberOfColumns != standOnTheRightAsVectorRow.size) {
      global.fatal
      << "Attempting to multiply a matrix, standing on the left, with "
      << this->numberOfColumns
      << " columns, by a matrix, standing on the right, with "
      << standOnTheRightAsVectorRow.size
      << " rows. "
      << global.fatal;
    }
    output.setSize(this->numberOfRows);
    for (int i = 0; i < this->numberOfRows; i ++) {
      output[i].makeZero(standOnTheRightAsVectorRow[0].size);
      for (int j = 0; j < this->numberOfColumns; j ++) {
        output[i] += standOnTheRightAsVectorRow[j] *(*this)(i, j);
      }
    }
  }
  void actMultiplyVectorRowOnTheRight(Vector<Coefficient>& standsOnTheLeft)
  const {
    Vector<Coefficient> output;
    this->actMultiplyVectorRowOnTheRight(standsOnTheLeft, output);
    standsOnTheLeft = output;
  }
  void actMultiplyVectorRowOnTheRight(
    const Vector<Coefficient>& standsOnTheLeft, Vector<Coefficient>& output
  ) const {
    if (&standsOnTheLeft == &output) {
      global.fatal
      << "In actMultiplyVectorRowOnTheRight: "
      << "output not allowed to equal input. "
      << global.fatal;
    }
    if (this->numberOfRows != standsOnTheLeft.size) {
      global.fatal
      << "Number of rows of matrix vector number of elements. "
      << global.fatal;
    }
    output.makeZero(this->numberOfColumns);
    Coefficient coefficient;
    for (int i = 0; i < this->numberOfColumns; i ++) {
      for (int j = 0; j < this->numberOfRows; j ++) {
        coefficient = this->elements[j][i];
        coefficient *= standsOnTheLeft[j];
        output[i] += coefficient;
      }
    }
  }
  void getNSquaredVectorForm(Vector<Coefficient>& output) {
    output.setSize(this->numberOfRows * this->numberOfColumns);
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        output.objects[i * this->numberOfRows + j] = this->elements[i][j];
      }
    }
  }
  template <class OtherType>
  void actOnVectorColumn(
    const Vector<OtherType>& input,
    Vector<OtherType>& output,
    const OtherType& ringZero = OtherType::zero()
  ) const {
    if (&input == &output) {
      Vector<OtherType> inputNew = input;
      this->actOnVectorColumn(inputNew, output, ringZero);
      return;
    }
    if (this->numberOfColumns != input.size) {
      global.fatal
      << "Attempt to multiply a matrix with "
      << this->numberOfColumns
      << " columns with a vector(column) of "
      << "dimension "
      << input.size
      << ". "
      << global.fatal;
    }
    output.makeZero(this->numberOfRows, ringZero);
    OtherType product;
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        product = this->elements[i][j];
        product *= input[j];
        output[i] += product;
      }
    }
  }
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  static unsigned int hashFunction(const Matrix<Coefficient>& input) {
    unsigned int result = 0;
    int hashCounter = 0;
    for (int i = 0; i < input.numberOfRows; i ++) {
      for (int j = 0; j < input.numberOfColumns; j ++) {
        result +=
        input.elements[i][j].hashFunction() *
        HashConstants::getConstantIncrementCounter(hashCounter);
      }
    }
    return result;
  }
  // Returns true if the vector space spanned by the input vectors
  // is mapped into itself by the matrix action.
  // Here, the vectors are regarded as vector-columns, and the
  // matrix action is multiplication of the columns on the right.
  bool preservesVectorSpace(const List<Vector<Coefficient> >& vectorSpaceBasis)
  const;
  template <class otherType>
  void actOnVectorsColumn(
    const Vectors<otherType>& input,
    Vectors<otherType>& output,
    const otherType& ringZero = 0
  ) const {
    if (&input == &output) {
      global.fatal
      << "In actOnVectorsColumn: input not allowed to equal output. "
      << global.fatal;
    }
    if (input.size == 0) {
      return;
    }
    if (this->numberOfColumns != input.getDimension()) {
      global.fatal
      << "Attempt to act by "
      << this->toString()
      << " (an "
      << this->numberOfRows
      << " x "
      << this->numberOfColumns
      << " matrix) on a column vector "
      << input.toString()
      << " (dimension "
      << input.size
      << "). "
      << global.fatal;
    }
    output.setSize(input.size);
    for (int i = 0; i < input.size; i ++) {
      this->actOnVectorColumn(input[i], output[i], ringZero);
    }
  }
  template <class otherType>
  void actOnVectorColumn(
    Vector<otherType>& inputOutput,
    const otherType& ringZero = otherType::zero()
  ) const {
    Vector<otherType> buffer;
    this->actOnVectorColumn(inputOutput, buffer, ringZero);
    inputOutput = buffer;
  }
  template <class otherType>
  void actOnVectorsColumn(
    Vectors<otherType>& inputOutput,
    const otherType& ringZero = otherType::zero()
  ) const {
    for (int i = 0; i < inputOutput.size; i ++) {
      this->actOnVectorColumn(inputOutput[i], ringZero);
    }
  }
  std::string toMathML(
    const FormatExpressions* format = nullptr,
    MathExpressionFormattingProperties* outputProperties = nullptr
  ) const;
  std::string toMathMLFinal(FormatExpressions* format = nullptr) const;
  std::string toString(const FormatExpressions* format = nullptr) const;
  std::string toStringLatex(const FormatExpressions* format = nullptr) const;
  std::string toStringSystemLatex(
    Matrix<Coefficient>* constantTerms = 0, FormatExpressions* format = nullptr
  ) const;
  std::string toStringPlainText(bool jsonFormat = false) const;
  std::string toStringOneLine(bool jsonFormat = false) const;
  std::string toStringWithBlocks(List<int>& blocks);
  void makeIdentityMatrix(
    int dimension,
    const Coefficient& ringUnit = Coefficient::oneStatic(),
    const Coefficient& ringZero = Coefficient::zeroStatic()
  ) {
    this->initialize(dimension, dimension);
    for (int i = 0; i < dimension; i ++) {
      for (int j = 0; j < dimension; j ++) {
        if (j != i) {
          this->elements[i][j] = ringZero;
        } else {
          this->elements[i][j] = ringUnit;
        }
      }
    }
  }
  void makeZeroMatrix(int dimension, const Coefficient& ringZero = 0) {
    this->initialize(dimension, dimension);
    for (int i = 0; i < dimension; i ++) {
      for (int j = 0; j < dimension; j ++) {
        this->elements[i][j] = ringZero;
      }
    }
  }
  void actOnMonomialAsDifferentialOperator(
    const MonomialPolynomial& input, Polynomial<Rational>& output
  );
  void switchRows(int row1, int row2);
  void switchRowsWithCarbonCopy(
    int row1, int row2, Matrix<Coefficient>* carbonCopy
  ) {
    this->switchRows(row1, row2);
    if (carbonCopy != 0) {
      carbonCopy->switchRows(row1, row2);
    }
  }
  void setNumberOfVariables(int desiredNumberOfVariables) {
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        this->elements[i][j].setNumberOfVariables(desiredNumberOfVariables);
      }
    }
  }
  void substitute(const PolynomialSubstitution<Rational>& substitution);
  Coefficient scalarProduct(
    const Vector<Coefficient>& left, const Vector<Coefficient>& right
  ) const {
    return this->scalarProduct(left, right, static_cast<Coefficient>(0));
  }
  Coefficient scalarProduct(
    const Vector<Coefficient>& left,
    const Vector<Coefficient>& right,
    const Coefficient& ringZero
  ) const {
    if (
      left.size != this->numberOfColumns || right.size != this->numberOfRows
    ) {
      global.fatal
      << "Scalar product using matrix: dimensions of vectors don't match. "
      << global.fatal;
    }
    Coefficient result;
    Coefficient contribution;
    result = ringZero;
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        contribution = left[i];
        contribution *= this->elements[i][j];
        contribution *= right[j];
        result += contribution;
      }
    }
    return result;
  }
  inline Coefficient& operator()(unsigned i, unsigned j) const {
    return (*this)(static_cast<signed>(i), static_cast<signed>(j));
  }
  Coefficient& operator()(int i, int j) const {
    if (
      i < 0 || i >= this->numberOfRows || j < 0 || j >= this->numberOfColumns
    ) {
      global.fatal
      << "Request of (row, column) with index ("
      << i
      << ", "
      << j
      << "), but the present matrix has "
      << this->numberOfRows
      << " rows and "
      << this->numberOfColumns
      << " colums. "
      << global.fatal;
    }
    return this->elements[i][j];
  }
  bool isSquare() const {
    return this->numberOfColumns == this->numberOfRows;
  }
  bool isDiagonal() const {
    if (
      this->numberOfRows != this->numberOfColumns || this->numberOfRows <= 0
    ) {
      return false;
    }
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        if (i == j) {
          continue;
        }
        if (!this->elements[i][j].isEqualToZero()) {
          return false;
        }
      }
    }
    return true;
  }
  bool isIdentity() const {
    if (
      this->numberOfRows != this->numberOfColumns || this->numberOfRows <= 0
    ) {
      return false;
    }
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        if (i == j) {
          if (!this->elements[i][j].isEqualToOne()) {
            return false;
          }
        } else {
          if (!this->elements[i][j].isEqualToZero()) {
            return false;
          }
        }
      }
    }
    return true;
  }
  bool jordanNormalForm(
    Matrix<AlgebraicNumber>& outputLeft,
    Matrix<AlgebraicNumber>& outputDiagonalized,
    Matrix<AlgebraicNumber>& outputRight,
    AlgebraicClosureRationals& ownerField,
    std::stringstream* comments
  );
  void getVectorFromColumn(int columnIndex, Vector<Coefficient>& output) const;
  void getVectorFromRow(int rowIndex, Vector<Coefficient>& output) const;
  int findPivot(int columnIndex, int rowStartIndex);
  bool findFirstNonZeroElementSearchEntireRow(Coefficient& output) {
    for (int i = 0; i < this->numberOfColumns; i ++) {
      for (int j = 0; j < this->numberOfRows; j ++) {
        if (!this->elements[i][j].isEqualToZero()) {
          output = this->elements[i][j];
          return true;
        }
      }
    }
    return false;
  }
  bool makeInvertibleHomomorphismFromDomainAndRange(
    Vectors<Coefficient>& domain, Vectors<Coefficient>& range
  ) {
    Matrix<Coefficient> domainMatrix;
    Matrix<Coefficient> rangeMatrix;
    domainMatrix.assignVectorsToRows(domain);
    rangeMatrix.assignVectorsToRows(range);
    if (!domainMatrix.invert()) {
      return false;
    }
    (*this) = domainMatrix * rangeMatrix;
    this->transpose();
    return true;
  }
  void rowTimesScalar(int rowIndex, const Coefficient& scalar);
  void rowTimesScalarWithCarbonCopy(
    int rowIndex, const Coefficient& scalar, Matrix<Coefficient>* carbonCopy
  ) {
    this->rowTimesScalar(rowIndex, scalar);
    if (carbonCopy != 0) {
      carbonCopy->rowTimesScalar(rowIndex, scalar);
    }
  }
  void addTwoRows(
    int fromRowIndex,
    int toRowIndex,
    int startColumnIndex,
    const Coefficient& scalar
  );
  void addTwoRowsWithCarbonCopy(
    int fromRowIndex,
    int toRowIndex,
    int startColumnIndex,
    const Coefficient& scalar,
    Matrix<Coefficient>* carbonCopy
  ) {
    this->addTwoRows(fromRowIndex, toRowIndex, startColumnIndex, scalar);
    if (carbonCopy != 0) {
      carbonCopy->addTwoRows(
        fromRowIndex, toRowIndex, startColumnIndex, scalar
      );
    }
  }
  void subtractRows(
    int indexRowWeSubtractFrom,
    int indexSubtracted,
    int startColumnIndex,
    const Coefficient& scalar
  );
  void subtractRowsWithCarbonCopy(
    int indexRowWeSubtractFrom,
    int indexSubtracted,
    int startColumnIndex,
    const Coefficient& scalar,
    Matrix<Coefficient>* carbonCopy
  ) {
    this->subtractRows(
      indexRowWeSubtractFrom, indexSubtracted, startColumnIndex, scalar
    );
    if (carbonCopy != 0) {
      carbonCopy->subtractRows(
        indexRowWeSubtractFrom, indexSubtracted, startColumnIndex, scalar
      );
    }
  }
  void multiplyOnTheLeft(
    const Matrix<Coefficient>& standsOnTheLeft,
    Matrix<Coefficient>& output,
    const Coefficient& ringZero = 0
  ) const;
  void multiplyOnTheLeft(
    const Matrix<Coefficient>& standsOnTheLeft, const Coefficient& ringZero = 0
  );
  void multiplyOnTheRight(const Matrix<Coefficient>& standsOnTheRight) {
    Matrix<Coefficient> thisCopy = *this;
    *this = standsOnTheRight;
    this->multiplyOnTheLeft(thisCopy);
  }
  void getVectorsFromRows(List<Vector<Coefficient> >& output) const {
    output.setSize(this->numberOfRows);
    for (int i = 0; i < this->numberOfRows; i ++) {
      output[i].setSize(this->numberOfColumns);
      for (int j = 0; j < this->numberOfColumns; j ++) {
        output[i][j] = this->elements[i][j];
      }
    }
  }
  void nonPivotPointsToEigenVectorHomogeneous(
    Selection& nonPivotPoints,
    Vector<Coefficient>& output,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  void nonPivotPointsToEigenVectorLexicographic(
    Selection& nonPivotPoints,
    Vector<Coefficient>& output,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  bool invert();
  Matrix<Coefficient> inverse() {
    Matrix<Coefficient> copy = *this;
    if (!copy.invert()) {
      global.fatal
      << "Request to invert "
      << copy.toString()
      << " failed. "
      << global.fatal;
    }
    return copy;
  }
  Matrix<Coefficient> operator^(const Matrix<Coefficient>& right) const;
  static void conjugationAction(
    const Matrix<Coefficient>& conjugateMe,
    const Matrix<Coefficient>& conjugateBy,
    Matrix<Coefficient>& out
  );
  void makeZero(const Coefficient& ringZero = 0);
  void makeIdentity(
    const Matrix<Coefficient>& prototype,
    const Coefficient& ringZero = 0,
    const Coefficient& ringOne = 1
  );
  // If m1 corresponds to a linear operator from V1 to V2 and
  // m2 to a linear operator from W1 to W2, then the result of the below
  // function corresponds to the linear operator
  // from V1+W1 to V2+W2 (direct sum).
  // This means you write the matrix m1 in
  // the upper left corner m2 in the lower
  // right and everything else you fill with zeros.
  void assignDirectSum(Matrix<Coefficient>& m1, Matrix<Coefficient>& m2);
  // If S and T are endomorphisms of V and W, build the matrix of S \otimes T
  // that acts on
  // V \otimes W with basis
  // (v1 \otimes w1, v1 \otimes w2, ..., v2 \otimes w1, v2 \otimes w2, ..., vn
  // \otimes wn)
  void assignTensorProduct(
    const Matrix<Coefficient>& left, const Matrix<Coefficient>& right
  );
  void assignVectorsToRows(const List<Vector<Coefficient> >& input) {
    int desiredNumberOfColumns = 0;
    if (input.size > 0) {
      desiredNumberOfColumns = input[0].size;
    }
    this->initialize(input.size, desiredNumberOfColumns);
    for (int i = 0; i < input.size; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        this->elements[i][j] = input[i][j];
      }
    }
  }
  void assignVectorsToColumns(const Vectors<Coefficient>& input) {
    this->initialize(input[0].size, input.size);
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        (*this)(i, j) = input[j][i];
      }
    }
  }
  void assignVectorColumn(const Vector<Coefficient>& input) {
    this->initialize(input.size, 1);
    for (int i = 0; i < input.size; i ++) {
      this->elements[i][0] = input[i];
    }
  }
  void assignVectorToRowKeepOtherRowsIntactNoInitialization(
    int rowIndex, const Vector<Coefficient>& input
  ) {
    if (
      input.size != this->numberOfColumns ||
      rowIndex >= this->numberOfRows ||
      rowIndex < 0
    ) {
      global.fatal
      << "Attempt to assign vector "
      << input.toString()
      << " ("
      << input.size
      << " coordinates) "
      << "to row with index "
      << rowIndex
      << " in a matrix with "
      << this->numberOfRows
      << " rows and "
      << this->numberOfColumns
      << " columns. "
      << global.fatal;
    }
    for (int i = 0; i < this->numberOfColumns; i ++) {
      this->elements[rowIndex][i] = input[i];
    }
  }
  void assignVectorToColumnKeepOtherColumnsIntact(
    int columnIndex, const Vector<Coefficient>& input
  ) {
    if (
      input.size != this->numberOfRows ||
      columnIndex >= this->numberOfColumns ||
      columnIndex < 0
    ) {
      global.fatal
      << "In assignVectorToColumnKeepOtherColumnsIntact: "
      << "bad vector/matrix dimensions. "
      << global.fatal;
    }
    for (int i = 0; i < this->numberOfRows; i ++) {
      this->elements[i][columnIndex] = input[i];
    }
  }
  void assignBlock(Matrix<Coefficient>& block, int starti, int startj) {
    if (
      starti + block.numberOfRows > this->numberOfRows ||
      startj + block.numberOfColumns > this->numberOfColumns
    ) {
      global.fatal << "In assignBlock: bad indices. " << global.fatal;
    }
    for (int i = 0; i < block.numberOfRows; i ++) {
      for (int j = 0; j < block.numberOfColumns; j ++) {
        this->elements[starti + i][startj + j] = block.elements[i][j];
      }
    }
  }
  void assignVectorRow(const Vector<Coefficient>& input) {
    this->initialize(1, input.size);
    for (int i = 0; i < input.size; i ++) {
      this->elements[0][i] = input[i];
    }
  }
  void directSumWith(
    const Matrix<Coefficient>& m2, const Coefficient& ringZero = 0
  );
  bool operator!=(const Matrix<Coefficient>& other) const {
    return !((*this) == other);
  }
  bool operator==(const Matrix<Coefficient>& other) const {
    if (
      this->numberOfRows != other.numberOfRows ||
      this->numberOfColumns != other.numberOfColumns
    ) {
      return false;
    }
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        if (!(this->elements[i][j] == other.elements[i][j])) {
          return false;
        }
      }
    }
    return true;
  }
  bool isEqualToZero() const {
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        if (this->elements[i][j] != 0) {
          return false;
        }
      }
    }
    return true;
  }
  bool isProportionalTo(
    const Matrix<Coefficient>& input, Coefficient& outputTimesMeEqualsInput
  ) {
    if (
      input.numberOfColumns != this->numberOfColumns ||
      input.numberOfRows != this->numberOfRows
    ) {
      return false;
    }
    bool found = false;
    for (int i = 0; i < this->numberOfRows && !found; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        if (!this->elements[i][j].isEqualToZero()) {
          found = true;
          outputTimesMeEqualsInput = input.elements[i][j];
          outputTimesMeEqualsInput /= this->elements[i][j];
          break;
        }
      }
    }
    if (!found) {
      return input.isEqualToZero();
    }
    Matrix<Coefficient> mustBeZero;
    mustBeZero = *this;
    mustBeZero *= outputTimesMeEqualsInput;
    mustBeZero -= input;
    return mustBeZero.isEqualToZero();
  }
  // Returns true if the system has a solution, false otherwise.
  bool rowEchelonFormToLinearSystemSolution(
    Selection& inputPivotPoints,
    Matrix<Coefficient>& inputRightHandSide,
    Matrix<Coefficient>& outputSolution
  );
  void operator+=(const Matrix<Coefficient>& right) {
    if (
      this->numberOfRows != right.numberOfRows ||
      this->numberOfColumns != right.numberOfColumns
    ) {
      global.fatal
      << "Attempt to add matrix with "
      << this->numberOfRows
      << " rows and "
      << this->numberOfColumns
      << " columns to a matrix with "
      << right.numberOfRows
      << " rows and "
      << right.numberOfColumns
      << " columns. "
      << global.fatal;
    }
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        this->elements[i][j] += right.elements[i][j];
      }
    }
  }
  LargeIntegerUnsigned findPositiveLCMCoefficientDenominators();
  void operator-=(const Matrix<Coefficient>& right);
  void writeToFile(std::fstream& output);
  bool readFromFile(std::fstream& input);
  void operator/=(const Coefficient& input) {
    for (int j = 0; j < this->numberOfRows; j ++) {
      for (int i = 0; i < this->numberOfColumns; i ++) {
        this->elements[j][i] /= input;
      }
    }
  }
  void operator*=(const Coefficient& input) {
    for (int j = 0; j < this->numberOfRows; j ++) {
      for (int i = 0; i < this->numberOfColumns; i ++) {
        this->elements[j][i] *= input;
      }
    }
  }
  void operator*=(const Matrix<Coefficient>& input) {
    this->multiplyOnTheRight(input);
  }
  void lieBracketWith(Matrix<Coefficient>& right) {
    Matrix<Coefficient> result;
    this->lieBracket(*this, right, result);
    *this = result;
  }
  static void lieBracket(
    const Matrix<Coefficient>& left,
    const Matrix<Coefficient>& right,
    Matrix<Coefficient>& output
  ) {
    if (
      left.numberOfColumns != left.numberOfRows ||
      right.numberOfColumns != right.numberOfRows ||
      left.numberOfColumns != right.numberOfColumns
    ) {
      global.fatal
      << "In lieBracket: bad dimensions of matrices. "
      << global.fatal;
    }
    Matrix<Coefficient> firstSummand;
    Matrix<Coefficient> subtracand;
    firstSummand = (right);
    firstSummand.multiplyOnTheLeft(left);
    subtracand = (left);
    subtracand.multiplyOnTheLeft(right);
    firstSummand -= (subtracand);
    output = firstSummand;
  }
  // The Gaussian elimination below brings the matrix to a row-echelon form,
  // that is, makes the matrix be something like (example is 4x5):
  // 1 0 3 0 0
  // 0 1 0 0 0
  // 0 0 0 1 0
  // 0 0 0 0 0
  // In the Gaussian elimination below, we define
  // non-pivot points to be indices
  // of the columns
  // that do not have a pivot 1 in them.
  // In the above example, the third (index 2) and fifth (index 4) columns do
  // not have a pivot 1 in them.
  void gaussianEliminationByRows(
    Matrix<Coefficient>* carbonCopyMatrix = 0,
    Selection* outputNonPivotColumns = nullptr,
    Selection* outputPivotColumns = nullptr,
    std::stringstream* humanReadableReport = nullptr,
    FormatExpressions* format = nullptr
  );
  void gaussianEliminationByRowsNoRowSwapPivotPointsByRows(
    int firstNonProcessedRow,
    List<int>& outputPivotPointColumns,
    Selection* outputNonPivotPoints__WarningSelectionNotInitialized
  );
  void gaussianEliminationEuclideanDomain(
    Matrix<Coefficient>* otherMatrix = 0,
    const Coefficient& negativeOne = - 1,
    const Coefficient& ringUnit = 1,
    bool(*comparisonGEQFunction)(
      const Coefficient& left, const Coefficient& right
    ) =
    0
  );
  static bool solveAxEqualsBModifyInputReturnFirstSolutionIfExists(
    Matrix<Coefficient>& A, Matrix<Coefficient>& b, Matrix<Coefficient>& output
  );
  Coefficient getDeterminant();
  Coefficient trace() const;
  void assignMatrixIntegerWithDenominator(
    Matrix<LargeInteger>& matrix, const LargeIntegerUnsigned& denominator
  );
  void scaleToIntegralForMinimalRationalHeightNoSignChange();
  void getMatrixIntegerWithDenominator(
    Matrix<LargeInteger>& outputMatrix, LargeIntegerUnsigned& outputDenominator
  );
  void lieBracketWith(const Matrix<Coefficient>& right);
  // A function that finds eigenspaces of a matrix provided the eigenvalues are
  // easy to guess. This function is likely no longer needed
  // and can be obsoleted cleanly by class [MatrixEigenvalueFinder].
  bool getEigenspacesProvidedAllAreIntegralWithEigenValueSmallerThanDimension(
    List<Vectors<Coefficient> >& output
  ) const;
  // Finds the eigenspaces of the matrix corresponding to the given
  // eigenvalues.
  // You can use this through the [MatrixEigenvalueFinder] class.
  void getEigenSpacesFromEigenvalues(
    List<Coefficient>& eigenvalues, List<Vectors<Coefficient> >& output
  ) const;
  void getZeroEigenSpace(List<Vector<Coefficient> >& output) const {
    Matrix<Coefficient> matrixCopy = *this;
    matrixCopy.getZeroEigenSpaceModifyMe(output);
  }
  void getZeroEigenSpaceModifyMe(List<Vector<Coefficient> >& output);
  void getEigenspaceModifyMe(
    const Coefficient& inputEigenValue,
    List<Vector<Coefficient> >& outputEigenspace
  );
  static bool
  systemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(
    Matrix<Coefficient>& matrixA,
    Matrix<Coefficient>& matrixb,
    Vector<Coefficient>* outputSolution = 0
  );
  static void computePotentialChangeGradient(
    Matrix<Coefficient>& matrixA,
    Selection& baseVariables,
    int numberOfTrueVariables,
    int columnIndex,
    Rational& outputChangeGradient,
    bool& hasAPotentialLeavingVariable
  );
  static void getMaxMovementAndLeavingVariableRow(
    Rational& maximumMovement,
    int& leavingVariableRow,
    int enteringVariable,
    Matrix<Coefficient>& matrixA,
    Vector<Coefficient>& inputVectorX,
    Selection& baseVariables
  );
  LargeInteger findPositiveGCDCoefficientNumerators();
  Matrix<Coefficient> operator-(const Matrix<Coefficient>& right) const {
    Matrix<Coefficient> result = (*this);
    result -= right;
    return result;
  }
  Matrix<Coefficient> operator*(const Matrix<Coefficient>& right) const;
  Vector<Coefficient> operator*(const Vector<Coefficient>& v) const;
  void operator=(const Matrix<Coefficient>& other);
  template <class otherType>
  void operator=(const Matrix<otherType>& other) {
    this->resize(other.numberOfRows, other.numberOfColumns, false);
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        (*this)(i, j) = other(i, j);
      }
    }
  }
  // The following are for compatibility with the FiniteGroup class.
  void getCharacteristicPolynomialStandardRepresentation(
    Polynomial<Coefficient>& out
  ) {
    out.assignCharacteristicPolynomial(*this);
  }
  bool hasDifferentConjugacyInvariantsFrom(Matrix<Coefficient>& other) {
    Polynomial<Coefficient> p;
    Polynomial<Coefficient> q;
    this->getCharacteristicPolynomialStandardRepresentation(p);
    other.getCharacteristicPolynomialStandardRepresentation(q);
    return !(p == q);
  }
  bool operator>(const Matrix<Coefficient>& right) const {
    if (
      this->numberOfRows != right.numberOfRows ||
      this->numberOfColumns != right.numberOfColumns
    ) {
      global.fatal
      << "An attempt was just made to compare "
      << "two matrices of different dimensions; "
      << "most likely something is wrong in some "
      << "FiniteGroup, see the frames above. "
      << global.fatal;
    }
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        if (!(this->elements[i][j] > right.elements[i][j])) {
          return false;
        }
      }
    }
    return true;
  }
  class Test {
  public:
    static bool all();
    static bool matrixIntegerWithDenominator();
    static bool oneMatrixIntegerWithDenominator(
      const std::string& input,
      const std::string& expectedMatrix,
      int expectedScale
    );
    // Makes a matrix from a string such as
    // ((2,3), (3,4), (4,5))
    // Converted to matrix using the calculator MakeMatrix function.
    static void matrixFromString(
      const std::string& inputString, Matrix<Rational>& output
    );
  };
};

template <class Coefficient>
bool Vectors<Coefficient>::conesIntersect(
  List<Vector<Rational> >& strictCone,
  List<Vector<Rational> >& nonStrictCone,
  Vector<Rational>* outputLinearCombo,
  Vector<Rational>* outputSplittingNormal
) {
  Matrix<Rational> A;
  Matrix<Rational> b;
  if (strictCone.size == 0) {
    if (outputSplittingNormal != nullptr) {
      if (nonStrictCone.size > 0) {
        outputSplittingNormal->makeZero(nonStrictCone[0].size);
      }
    }
    return false;
  }
  int dimension = strictCone[0].size;
  int numberOfColumns = strictCone.size + nonStrictCone.size;
  A.initialize(dimension + 1, numberOfColumns);
  b.initialize(dimension + 1, 1);
  b.makeZero();
  b.elements[dimension][0].makeOne();
  for (int i = 0; i < strictCone.size; i ++) {
    for (int k = 0; k < dimension; k ++) {
      A.elements[k][i].assign(strictCone[i][k]);
    }
    A.elements[dimension][i].makeOne();
  }
  for (int i = 0; i < nonStrictCone.size; i ++) {
    int currentColumn = i + strictCone.size;
    for (int k = 0; k < dimension; k ++) {
      A.elements[k][currentColumn].assign(nonStrictCone[i][k]);
      A.elements[k][currentColumn].negate();
    }
    A.elements[dimension][currentColumn].makeZero();
  }
  if (
    !Matrix<Rational>::
    systemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution
    (A, b, outputLinearCombo)
  ) {
    if (outputSplittingNormal != nullptr) {
      bool tempBool = Vectors<Coefficient>::getNormalSeparatingCones(
        strictCone, nonStrictCone, *outputSplittingNormal
      );
      if (!tempBool) {
        global.fatal
        << "This is an algorithmic/mathematical "
        << "(hence also programming) error. "
        << "I get that two cones do not intersect, "
        << "yet there exists no plane separating them. "
        << "Something is wrong with the "
        << "implementation of the simplex algorithm. "
        << "The input which manifested the problem was: <br>StrictCone: <br>"
        << strictCone.toString()
        << "<br>Non-strict cone: <br>"
        << nonStrictCone.toString()
        << "<br>"
        << global.fatal;
      }
    }
    return false;
  }
  if (outputLinearCombo != nullptr) {
    for (int i = strictCone.size; i < outputLinearCombo->size; i ++) {
      (*outputLinearCombo)[i] *= - 1;
    }
  }
  return true;
}

template <typename Coefficient>
void Matrix<Coefficient>::resize(
  int r, int c, bool preserveValues, const Coefficient* const ringZero
) {
  if (r < 0) {
    r = 0;
  }
  if (c < 0) {
    c = 0;
  }
  if (r == this->numberOfRows && c == this->numberOfColumns) {
    return;
  }
  if (r == 0 || c == 0) {
    this->numberOfRows = r;
    this->numberOfColumns = c;
    return;
  }
  Coefficient** newElements = nullptr;
  int newActualNumberOfColumns =
  MathRoutines::maximum(this->actualNumberOfColumns, c);
  int newActualNumberOfRows =
  MathRoutines::maximum(this->actualNumberOfRows, r);
  if (r > this->actualNumberOfRows || c > this->actualNumberOfColumns) {
    newElements = new Coefficient*[newActualNumberOfRows];
#ifdef AllocationLimitsSafeguard
    GlobalStatistics::globalPointerCounter += newActualNumberOfRows;
    GlobalStatistics::checkPointerCounters();
#endif
    for (int i = 0; i < newActualNumberOfRows; i ++) {
      newElements[i] = new Coefficient[newActualNumberOfColumns];
#ifdef AllocationLimitsSafeguard
      GlobalStatistics::globalPointerCounter += newActualNumberOfColumns;
      GlobalStatistics::checkPointerCounters();
#endif
    }
  }
  int firstInvalidRow = MathRoutines::minimum(this->numberOfRows, r);
  int firstInvalidColumn = MathRoutines::minimum(this->numberOfColumns, c);
  if (preserveValues && newElements != nullptr) {
    for (int j = 0; j < firstInvalidRow; j ++) {
      for (int i = 0; i < firstInvalidColumn; i ++) {
        newElements[j][i] = this->elements[j][i];
      }
    }
  }
  if (ringZero != nullptr) {
    if (!preserveValues) {
      firstInvalidRow = 0;
      firstInvalidColumn = 0;
    }
    for (int i = 0; i < r; i ++) {
      int columnStart = (i < firstInvalidRow) ? firstInvalidColumn : 0;
      for (int j = columnStart; j < c; j ++) {
        newElements[i][j] = *ringZero;
      }
    }
  }
  if (newElements != nullptr) {
    this->releaseMemory();
    this->elements = newElements;
    this->actualNumberOfColumns = newActualNumberOfColumns;
    this->actualNumberOfRows = newActualNumberOfRows;
  }
  this->numberOfColumns = c;
  this->numberOfRows = r;
}

template <typename Coefficient>
void Vectors<Coefficient>::orthogonalComplement(
  Vectors<Coefficient>& output, Matrix<Coefficient>* bilinearForm
) {
  STACK_TRACE("Vectors::orthogonalComplement");
  if (this->size == 0) {
    if (bilinearForm != nullptr) {
      output.makeEiBasis(bilinearForm->numberOfRows);
      return;
    }
    global.fatal
    << "Finding orthogonal complement of zero "
    << "vectors without specifying a bilinear form is "
    << "forbidden: I can't determine the dimension "
    << "of the ambient vector space. "
    << global.fatal;
  }
  Matrix<Coefficient> matrix;
  matrix.assignVectorsToRows(*this);
  if (bilinearForm != nullptr) {
    matrix *= *bilinearForm;
  }
  matrix.getZeroEigenSpaceModifyMe(output);
}

template <typename Coefficient>
bool Vectors<Coefficient>::computeNormal(
  Vector<Coefficient>& output, int inputDimension
) {
  if (this->size == 0) {
    if (inputDimension == 1) {
      output.makeEi(1, 0);
      return true;
    }
    return false;
  }
  int dimension = this->objects[0].size;
  Matrix<Coefficient> eliminated;
  Selection nonPivotPoints;
  nonPivotPoints.initialize(dimension);
  output.setSize(dimension);
  this->gaussianEliminationForNormalComputation(
    eliminated, nonPivotPoints, dimension
  );
  if (nonPivotPoints.cardinalitySelection != 1) {
    return false;
  }
  eliminated.nonPivotPointsToEigenVectorHomogeneous(nonPivotPoints, output);
  return true;
}

template <typename Coefficient>
void Matrix<Coefficient>::operator=(const Matrix<Coefficient>& m) {
  if (this == &m) {
    return;
  }
  this->resize(m.numberOfRows, m.numberOfColumns, false);
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      this->elements[i][j] = m.elements[i][j];
    }
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::releaseMemory() {
  for (int i = 0; i < this->actualNumberOfRows; i ++) {
    delete[] this->elements[i];
  }
  delete[] this->elements;
#ifdef AllocationLimitsSafeguard
  GlobalStatistics::globalPointerCounter -=
  this->actualNumberOfRows * this->actualNumberOfColumns +
  this->actualNumberOfRows;
  GlobalStatistics::checkPointerCounters();
#endif
  this->elements = nullptr;
  this->numberOfColumns = 0;
  this->numberOfRows = 0;
  this->actualNumberOfRows = 0;
  this->actualNumberOfColumns = 0;
}

template <typename Coefficient>
bool Matrix<Coefficient>::invert() {
  STACK_TRACE("Matrix::invert");
  if (this->numberOfColumns != this->numberOfRows) {
    global.fatal
    << "Request to invert a non-square matrix of "
    << this->numberOfRows
    << " rows and "
    << this->numberOfColumns
    << " columns. "
    << global.fatal;
  }
  if (this->numberOfColumns == 0) {
    global.fatal << "0 by 0 matrix inversion is not allowed. " << global.fatal;
  }
  Matrix inverse;
  inverse.makeIdentityMatrix(
    this->numberOfRows, (*this)(0, 0).one(), (*this)(0, 0).zero()
  );
  Selection nonPivotColumns;
  this->gaussianEliminationByRows(&inverse, &nonPivotColumns, 0);
  if (nonPivotColumns.cardinalitySelection != 0) {
    return false;
  }
  *this = inverse;
  return true;
}

template <typename Coefficient>
Matrix<Coefficient> Matrix<Coefficient>::operator^(
  const Matrix<Coefficient>& right
) const {
  Matrix<Coefficient> conjugateInverse = right;
  conjugateInverse.invert();
  Matrix<Coefficient> buffer;
  this->multiplyOnTheLeft(conjugateInverse, buffer);
  Matrix<Coefficient> out;
  right.multiplyOnTheLeft(buffer, out);
  return out;
}

template <typename Coefficient>
void Matrix<Coefficient>::conjugationAction(
  const Matrix<Coefficient>& conjugateWith,
  const Matrix<Coefficient>& conjugateOn,
  Matrix<Coefficient>& out
) {
  out = conjugateOn ^ conjugateWith;
}

template <typename Coefficient>
void Matrix<Coefficient>::multiplyOnTheLeft(
  const Matrix<Coefficient>& standsOnTheLeft, const Coefficient& ringZero
) {
  Matrix<Coefficient> result;
  this->multiplyOnTheLeft(standsOnTheLeft, result, ringZero);
  this->operator=(result);
}

template <typename Coefficient>
void Matrix<Coefficient>::multiplyOnTheLeft(
  const Matrix<Coefficient>& standsOnTheLeft,
  Matrix<Coefficient>& output,
  const Coefficient& ringZero
) const {
  if (&output == this || &output == &standsOnTheLeft) {
    Matrix<Coefficient> thisCopy = *this;
    Matrix<Coefficient> standsOnTheLeftCopy = standsOnTheLeft;
    thisCopy.multiplyOnTheLeft(standsOnTheLeftCopy, output, ringZero);
    return;
  }
  if (this->numberOfRows != standsOnTheLeft.numberOfColumns) {
    global.fatal
    << "Attempt to multiply a matrix with "
    << standsOnTheLeft.numberOfColumns
    << " columns by a matrix with "
    << this->numberOfRows
    << " rows. "
    << global.fatal;
  }
  Coefficient coefficient;
  output.initialize(standsOnTheLeft.numberOfRows, this->numberOfColumns);
  for (int i = 0; i < standsOnTheLeft.numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      output.elements[i][j] = ringZero;
      for (int k = 0; k < this->numberOfRows; k ++) {
        coefficient = standsOnTheLeft(i, k);
        coefficient *= this->elements[k][j];
        output.elements[i][j] += coefficient;
      }
    }
  }
}

template <typename Coefficient>
bool Matrix<Coefficient>::preservesVectorSpace(
  const List<Vector<Coefficient> >& vectorSpaceBasis
) const {
  Vectors<Coefficient> vectors = vectorSpaceBasis;
  Vectors<Coefficient> transformed;
  Vectors<Coefficient> intersection;
  this->actOnVectorsColumn(vectors, transformed);
  return vectors.linearSpanContainsVectors(transformed);
}

template <typename Coefficient>
std::string Matrix<Coefficient>::toStringWithBlocks(List<int>& blocks) {
  std::stringstream out;
  std::string currentString;
  out << "\\left(\\begin{array}{";
  int offset = 0;
  int blockIndex = 0;
  for (int j = 0; j < this->numberOfColumns; j ++) {
    out << "c";
    offset ++;
    if (offset == blocks.objects[blockIndex]) {
      offset = 0;
      blockIndex ++;
      if (j != this->numberOfColumns - 1) {
        out << "|";
      }
    }
  }
  out << "}";
  offset = 0;
  blockIndex = 0;
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      currentString = this->elements[i][j].toString();
      out << currentString;
      if (j != this->numberOfColumns - 1) {
        out << " & ";
      }
    }
    out << "\\\\\n";
    offset ++;
    if (offset == blocks.objects[blockIndex]) {
      offset = 0;
      blockIndex ++;
      if (i != this->numberOfColumns - 1) {
        out << "\\hline";
      }
    }
  }
  out << "\\end{array}\\right)";
  return out.str();
}

template <typename Coefficient>
void Matrix<Coefficient>::assignDirectSum(
  Matrix<Coefficient>& m1, Matrix<Coefficient>& m2
) {
  if (this == &m1 || this == &m2) {
    global.fatal
    << "In assignDirectSum: output not allowed "
    << "to equal any of the inputs. "
    << global.fatal;
  }
  this->resize(
    m1.numberOfRows + m2.numberOfRows,
    m1.numberOfColumns + m2.numberOfColumns,
    false
  );
  this->makeZero();
  for (int i = 0; i < m1.numberOfRows; i ++) {
    for (int j = 0; j < m1.numberOfColumns; j ++) {
      this->elements[i][j] = m1.elements[i][j];
    }
  }
  for (int i = 0; i < m2.numberOfRows; i ++) {
    for (int j = 0; j < m2.numberOfColumns; j ++) {
      this->elements[i + m1.numberOfRows][j + m1.numberOfColumns] =
      m2.elements[i][j];
    }
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::assignTensorProduct(
  const Matrix<Coefficient>& left, const Matrix<Coefficient>& right
) {
  // handle lazy programmers:
  if (this == &left || this == &right) {
    Matrix<Coefficient> leftCopy = left;
    Matrix<Coefficient> rightCopy = right;
    this->assignTensorProduct(leftCopy, rightCopy);
    return;
  }
  this->resize(
    left.numberOfRows * right.numberOfRows,
    left.numberOfColumns * right.numberOfColumns,
    false
  );
  int rightRows = right.numberOfRows;
  int rightColumns = right.numberOfColumns;
  // The basis of the tensor product vector space MUST be
  // in the SAME order as the one used by MatrixTensor::assignTensorProduct.
  // indexing. Let the first vector space have basis
  // v_1, ..., v_k, the second: w_1, ..., w_m.
  // Then the basis of the tensor product is given
  // in the order: v_1\otimes w_1, ..., v_1\otimes w_m,
  // ..., v_k\otimes w_1, ..., v_k\otimes w_m
  for (int iv = 0; iv < left.numberOfRows; iv ++) {
    for (int iw = 0; iw < right.numberOfRows; iw ++) {
      for (int jv = 0; jv < left.numberOfColumns; jv ++) {
        for (int jw = 0; jw < right.numberOfColumns; jw ++) {
          this->elements[iv * rightRows + iw][jv * rightColumns + jw] =
          left.elements[iv][jv] * right.elements[iw][jw];
        }
      }
    }
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::directSumWith(
  const Matrix<Coefficient>& m2, const Coefficient& ringZero
) {
  int oldNumberOfRows = this->numberOfRows;
  int oldNumberOfColumns = this->numberOfColumns;
  this->resize(
    this->numberOfRows + m2.numberOfRows,
    this->numberOfColumns + m2.numberOfColumns,
    true
  );
  for (int i = 0; i < m2.numberOfRows; i ++) {
    for (int j = 0; j < m2.numberOfColumns; j ++) {
      this->elements[i + oldNumberOfRows][j + oldNumberOfColumns] =
      m2.elements[i][j];
    }
    for (int j = 0; j < oldNumberOfColumns; j ++) {
      this->elements[i + oldNumberOfRows][j] = ringZero;
    }
  }
  for (int j = 0; j < oldNumberOfRows; j ++) {
    for (int i = oldNumberOfColumns; i < this->numberOfColumns; i ++) {
      this->elements[j][i] = ringZero;
    }
  }
}

template <typename Coefficient>
int Matrix<Coefficient>::findPivot(int columnIndex, int rowStartIndex) {
  for (int i = rowStartIndex; i < this->numberOfRows; i ++) {
    if (!this->elements[i][columnIndex].isEqualToZero()) {
      return i;
    }
  }
  return - 1;
}

template <typename Coefficient>
void Matrix<Coefficient>::subtractRows(
  int indexRowWeSubtractFrom,
  int indexSubtracted,
  int startColumnIndex,
  const Coefficient& scalar
) {
  Coefficient coefficient;
  for (int i = startColumnIndex; i < this->numberOfColumns; i ++) {
    coefficient = this->elements[indexSubtracted][i];
    coefficient *= scalar;
    this->elements[indexRowWeSubtractFrom][i] -= coefficient;
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::rowTimesScalar(
  int rowIndex, const Coefficient& scalar
) {
  for (int i = 0; i < this->numberOfColumns; i ++) {
    this->elements[rowIndex][i] *= scalar;
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::switchRows(int row1, int row2) {
  if (row1 == row2) {
    return;
  }
  Coefficient * swapper = this->elements[row1];
  this->elements[row1] = this->elements[row2];
  this->elements[row2] = swapper;
}

template <typename Coefficient>
bool Matrix<Coefficient>::solveAxEqualsBModifyInputReturnFirstSolutionIfExists(
  Matrix<Coefficient>& A, Matrix<Coefficient>& b, Matrix<Coefficient>& output
) {
  if (A.numberOfRows != b.numberOfRows) {
    global.fatal
    << "Number of matrix rows does not match number of vector entries. "
    << global.fatal;
  }
  Selection pivotPoints;
  A.gaussianEliminationByRows(&b, 0, &pivotPoints);
  return A.rowEchelonFormToLinearSystemSolution(pivotPoints, b, output);
}

template <typename Coefficient>
bool Matrix<Coefficient>::rowEchelonFormToLinearSystemSolution(
  Selection& inputPivotPoints,
  Matrix<Coefficient>& inputRightHandSide,
  Matrix<Coefficient>& outputSolution
) {
  if (
    inputPivotPoints.numberOfElements != this->numberOfColumns ||
    inputRightHandSide.numberOfColumns != 1 ||
    inputRightHandSide.numberOfRows != this->numberOfRows
  ) {
    global.fatal
    << "In rowEchelonFormToLinearSystemSolution: bad matrix dimensions. "
    << global.fatal;
  }
  outputSolution.initialize(this->numberOfColumns, 1);
  int numberOfPivots = 0;
  for (int i = 0; i < this->numberOfColumns; i ++) {
    if (inputPivotPoints.selected[i]) {
      outputSolution(i, 0) = inputRightHandSide(numberOfPivots, 0);
      numberOfPivots ++;
    } else {
      outputSolution(i, 0).makeZero();
    }
  }
  for (int i = numberOfPivots; i < this->numberOfRows; i ++) {
    if (!inputRightHandSide.elements[i][0].isEqualToZero()) {
      return false;
    }
  }
  return true;
}

template <typename Coefficient>
void Matrix<Coefficient>::gaussianEliminationByRowsNoRowSwapPivotPointsByRows(
  int firstNonProcessedRow,
  List<int>& outputPivotPointColumns,
  Selection* outputNonPivotPoints__WarningSelectionNotInitialized
) {
  outputPivotPointColumns.setSize(this->numberOfRows);
  Coefficient coefficient;
  for (int i = firstNonProcessedRow; i < this->numberOfRows; i ++) {
    int currentPivotCol = - 1;
    for (int j = 0; j < this->numberOfColumns; j ++) {
      if (!this->elements[i][j].isEqualToZero()) {
        currentPivotCol = j;
        break;
      }
    }
    outputPivotPointColumns.objects[i] = currentPivotCol;
    if (currentPivotCol != - 1) {
      coefficient = this->elements[i][currentPivotCol];
      coefficient.invert();
      this->rowTimesScalar(i, coefficient);
      for (int j = 0; j < this->numberOfRows; j ++) {
        if (i != j) {
          if (!this->elements[j][i].isEqualToZero()) {
            coefficient.assign(this->elements[j][i]);
            this->subtractRows(j, i, 0, coefficient);
          }
        }
      }
    }
  }
  if (outputNonPivotPoints__WarningSelectionNotInitialized != nullptr) {
    for (int i = 0; i < this->numberOfColumns; i ++) {
      outputNonPivotPoints__WarningSelectionNotInitialized->selected[i] = true;
    }
    for (int i = 0; i < this->numberOfRows; i ++) {
      if (outputPivotPointColumns.objects[i] != - 1) {
        outputNonPivotPoints__WarningSelectionNotInitialized->selected[
          outputPivotPointColumns.objects[i]
        ] =
        false;
      }
    }
    outputNonPivotPoints__WarningSelectionNotInitialized->
    computeIndicesFromSelection();
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::makeZero(const Coefficient& ringZero) {
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      this->elements[i][j] = ringZero;
    }
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::makeIdentity(
  const Matrix<Coefficient>& prototype,
  const Coefficient& ringZero,
  const Coefficient& ringOne
) {
  this->initialize(prototype.numberOfRows, prototype.numberOfColumns);
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      this->elements[i][j] = i == j ? ringOne : ringZero;
    }
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::initialize(
  int desiredNumberOfRows, int desiredNumberOfColumns
) {
  this->resize(desiredNumberOfRows, desiredNumberOfColumns, false);
}

struct MathMLAndLatex {
  friend std::ostream& operator<<(
    std::ostream& output, const MathMLAndLatex& element
  ) {
    return output << element.latex;
  }
  std::string latex;
  std::string mathML;
  MathMLAndLatex(
    const std::string& inputLatexLetter, const std::string& inputMathMLLetter
  ):
  latex(inputLatexLetter),
  mathML(inputMathMLLetter) {}
  MathMLAndLatex(const std::string& inputLetter = ""):
  latex(inputLetter),
  mathML(inputLetter) {}
  void clear() {
    this->mathML = "";
    this->latex = "";
  }
  bool operator==(const MathMLAndLatex& other) const;
  void operator=(const std::string& other);
};

class FormatExpressions {
public:
  // alphabetBases must contain at least two elements
  std::string chevalleyGGeneratorLetter;
  std::string chevalleyHGeneratorLetter;
  MathMLAndLatex fundamentalWeightLetter;
  std::string polynomialDefaultLetter;
  std::string weylAlgebraDefaultLetter;
  MathMLAndLatex customPlusSign;
  std::string customCoefficientMonomialSeparator;
  MathMLAndLatex finiteDimensionalRepresentationLetter;
  std::string simpleRootLetter;
  List<MathMLAndLatex> polynomialAlphabet;
  List<std::string> weylAlgebraLetters;
  List<MathMLAndLatex> vectorSpaceEiBasisNames;
  Rational ambientCartanSymmetricInverseScale;
  int extraLinesCounterLatex;
  int numberOfAmpersandsPerNewLineForLaTeX;
  int maximumLineLength;
  int maximumMatrixLineLength;
  int maximumMatrixDisplayedRows;
  int matrixColumnVerticalLineIndex;
  static const int expressionLineBreak = 50;
  bool flagDontCollalpseProductsByUnits;
  bool flagPassCustomCoefficientMonomialSeparatorToCoefficients;
  bool flagUseLatex;
  bool flagUsePmatrix;
  bool flagUsePNG;
  bool flagUseHTML;
  bool flagDynkinTypeDontUsePlusAndExponent;
  bool flagUseCalculatorFormatForUEOrdered;
  bool flagQuasiDiffOpCombineWeylPart;
  bool flagExpressionIsTopLevel;
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
  bool flagSuppressModP;
  bool flagIsInNumerator;
  MathBootstrapScriptType bootstrapScriptType;
  std::string suffixLinearCombination;
  char ambientWeylLetter;
  List<MonomialPolynomial>::Comparator monomialOrder;
  template <typename TemplateMonomial>
  const typename List<TemplateMonomial>::Comparator* getMonomialOrder() const;
  std::string polynomialLatexLetter(int index) const;
  std::string polynomialMathMLLetter(int index) const;
  FormatExpressions();
  static FormatExpressions* defaultFormat();
  void makePolynomialAlphabetLetters(
    const std::string& inputDefaultLetter, int letterCount
  );
  void makeAlphabetXYZUW();
  const MathMLAndLatex& fetchCustomPlusSign();
  // Returns true if a given object string/latex representation is
  // of the form 1/A or -1/A. Used to shorten an expressions such as
  // (1/A * T) to T/A. To use this method, override it for your
  // particular type.
  // `input` is the object we are trying to convert to a string.
  // `format` is the formatting options to use when computing the
  // string representation of the object.
  // `outputSign` will be modified to 1
  // if the input is of the form 1/A and to -1 if the input is of the form
  // -1/A.
  // `outputDenominatorString` is the string representation of the denominator
  // A.
  // This method may be overridden for a specific type, search for
  // its implementations please.
  template <typename Object>
  static bool toStringIsFractionLikeWithUnitNumerator(
    const Object& input,
    const FormatExpressions* format,
    int& outputSign,
    std::string& outputDenominatorString
  ) {
    (void) input;
    (void) format;
    (void) outputSign;
    (void) outputDenominatorString;
    return false;
  }
};

template <class SourceCoefficient, class TargetCoefficient>
class CoefficientConverter {
public:
  virtual bool convert(
    const SourceCoefficient& input, TargetCoefficient& output
  ) = 0;
};

// Monomial used to implement sparse vectors.
class MonomialVector {
  friend std::ostream& operator<<(
    std::ostream& output, const MonomialVector& monomial
  ) {
    output << monomial.toString();
    return output;
  }
public:
  int monomialIndex;
  MonomialVector(): monomialIndex(- 1) {}
  MonomialVector(int inputIndex): monomialIndex(inputIndex) {}
  std::string toString(const FormatExpressions* format = nullptr) const;
  std::string toMathML(
    const FormatExpressions* format = nullptr,
    MathExpressionFormattingProperties* outputProperties = nullptr
  ) const;
  unsigned int hashFunction() const {
    return static_cast<unsigned int>(this->monomialIndex);
  }
  static unsigned int hashFunction(const MonomialVector& input) {
    return input.hashFunction();
  }
  bool operator==(const MonomialVector& other) const {
    return this->monomialIndex == other.monomialIndex;
  }
  void makeEi(int inputIndex) {
    this->monomialIndex = inputIndex;
  }
  bool operator>(const MonomialVector& other) const {
    return this->monomialIndex > other.monomialIndex;
  }
};

class LaTeXProcedures {
public:
  static const int scaleFactor = 40;
  // In centimeters.
  static const int figureCenterCoordinateSystemX = 4;
  // In centimeters.
  static const int figureCenterCoordinateSystemY = 8;
  // In centimeters.
  static const int textPrintCenteringAdjustmentX = 3;
  static const int textPrintCenteringAdjustmentY = 3;
  static void drawline(
    double x1,
    double y1,
    double x2,
    double y2,
    uint32_t penStyle,
    int colorIndex,
    std::fstream& output,
    DrawingVariables& drawInput
  );
  static void drawTextDirectly(
    double x1,
    double y1,
    const std::string& text,
    int colorIndex,
    std::fstream& output
  );
  static void beginDocument(std::fstream& output);
  static void endLatexDocument(std::fstream& output);
  static void beginPSTricks(std::fstream& output);
  static void endPSTricks(std::fstream& output);
  static void getStringFromColorIndex(
    int colorIndex, std::string& output, DrawingVariables& drawInput
  );
};

class Permutation: public SelectionWithDifferentMaxMultiplicities {
public:
  void initPermutation(int n);
  void initPermutation(List<int>& disjointSubsets, int totalNumberOfElements);
  void incrementAndGetPermutation(List<int>& output);
  void getPermutationLthElementIsTheImageofLthIndex(List<int>& output);
  int getNumberOfPermutations() {
    return this->totalNumberSubsetsSmallInt();
  }
};

template <typename Coefficient>
void Matrix<Coefficient>::nonPivotPointsToEigenVectorHomogeneous(
  Selection& nonPivotPoints,
  Vector<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  int rowCounter = 0;
  output.setSize(this->numberOfColumns);
  for (int i = 0; i < this->numberOfColumns; i ++) {
    if (nonPivotPoints.selected[i]) {
      output[i] = ringUnit;
      continue;
    }
    output[i] = ringZero;
    for (int j = 0; j < nonPivotPoints.cardinalitySelection; j ++) {
      output[i] -= this->elements[rowCounter][nonPivotPoints.elements[j]];
    }
    rowCounter ++;
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::nonPivotPointsToEigenVectorLexicographic(
  Selection& nonPivotPoints,
  Vector<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  (void) ringZero;
  output.makeZero(this->numberOfColumns);
  if (nonPivotPoints.cardinalitySelection == 0) {
    return;
  }
  int firstNonPivotIndex = nonPivotPoints.elements[0];
  output[firstNonPivotIndex] = ringUnit;
  for (int i = firstNonPivotIndex - 1; i >= 0; i --) {
    output[i] = (*this)(i, firstNonPivotIndex);
    output[i].negate();
  }
}

template <class Coefficient>
class Complex {
  static bool flagEqualityIsApproximate;
  static double equalityPrecision;
public:
  Coefficient imaginaryPart;
  Coefficient realPart;
  std::string toString(const FormatExpressions* unused = nullptr) const {
    (void) unused;
    std::stringstream currentStream;
    currentStream << *this;
    return currentStream.str();
  }
  friend std::iostream& operator<<(
    std::iostream& output, const Complex<Coefficient>& input
  ) {
    output << input.realPart << "+ i" << input.imaginaryPart;
    return output;
  }
  void operator*=(const Complex<Coefficient>& other) {
    Complex accumulator;
    accumulator.realPart =
    this->realPart *
    other.realPart - this->imaginaryPart * other.imaginaryPart;
    accumulator.imaginaryPart =
    this->realPart * other.imaginaryPart +
    this->imaginaryPart * other.realPart;
    this->operator=(accumulator);
  }
  void operator=(const Complex<Coefficient>& other) {
    this->realPart = other.realPart;
    this->imaginaryPart = other.imaginaryPart;
  }
  void operator+=(const Complex<Coefficient>& other) {
    this->realPart += other.realPart;
    this->imaginaryPart += other.imaginaryPart;
  }
  void operator-=(const Complex<Coefficient>& other) {
    this->realPart -= other.realPart;
    this->imaginaryPart -= other.imaginaryPart;
  }
  void operator=(int other) {
    this->realPart = other;
    this->imaginaryPart = 0;
  }
  bool operator==(int other) const {
    Complex<Coefficient> otherComplex;
    otherComplex = other;
    return *this == otherComplex;
  }
  bool operator==(Complex<Coefficient>& other) const {
    Complex<Coefficient> difference;
    difference = *this;
    difference -= other;
    return difference.isEqualToZero();
  }
  void operator=(double other) {
    this->realPart = other;
    this->imaginaryPart = 0;
  }
  void invert() {
    Coefficient numerator;
    numerator =
    this->realPart * this->realPart +
    this->imaginaryPart * this->imaginaryPart;
    this->realPart /= numerator;
    numerator *= - 1;
    this->imaginaryPart /= numerator;
  }
  bool isEqualToZero() const;
  void negate() {
    this->imaginaryPart = - this->imaginaryPart;
    this->realPart = - this->realPart;
  }
  void computeFormattingProperties(
    const FormatExpressions* format = nullptr,
    MathExpressionFormattingProperties* outputProperties = nullptr
  ) const {
    (void) format;
    if (outputProperties == nullptr) {
      return;
    }
    outputProperties->needsParenthesesForMultiplicationOnTheRight =
    this->needsParenthesisForMultiplication(format);
  }
  bool needsParenthesisForMultiplication(const FormatExpressions* unused) const {
    (void) unused;
    if (this->realPart == 0 && this->imaginaryPart >= 0) {
      return false;
    }
    if (this->imaginaryPart == 0 && this->realPart >= 0) {
      return false;
    }
    return true;
  }
  Coefficient magnitudeSquared() const {
    return
    this->realPart * this->realPart +
    this->imaginaryPart * this->imaginaryPart;
  }
  Complex() {}
  Complex(int other) {
    this->operator=(other);
  }
  Complex(double other) {
    this->operator=(other);
  }
  Complex(const Complex<Coefficient>& other) {
    this->realPart = other.realPart;
    this->imaginaryPart = other.imaginaryPart;
  }
};

template < >
bool Complex<double>::flagEqualityIsApproximate;
template < >
double Complex<double>::equalityPrecision;

template <class Coefficient>
bool Complex<Coefficient>::isEqualToZero() const {
  if (!Complex<Coefficient>::flagEqualityIsApproximate) {
    return this->imaginaryPart == 0 && this->realPart == 0;
  } else {
    return
    this->imaginaryPart < Complex<Coefficient>::equalityPrecision &&
    - this->imaginaryPart < Complex<Coefficient>::equalityPrecision &&
    this->realPart < Complex<Coefficient>::equalityPrecision &&
    - this->realPart < Complex<Coefficient>::equalityPrecision;
  }
}

template <class Coefficient>
void Matrix<Coefficient>::operator-=(const Matrix<Coefficient>& right) {
  if (
    this->numberOfRows != right.numberOfRows ||
    this->numberOfColumns != right.numberOfColumns
  ) {
    global.fatal
    << "Attempt to subtract from matrix with "
    << this->numberOfRows
    << " rows and "
    << this->numberOfColumns
    << " columns a matrix with "
    << right.numberOfRows
    << " rows and "
    << right.numberOfColumns
    << " columns. "
    << global.fatal;
  }
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      (*this)(i, j) -= right(i, j);
    }
  }
}

template <class Coefficient>
LargeIntegerUnsigned Matrix<Coefficient>::
findPositiveLCMCoefficientDenominators() {
  LargeIntegerUnsigned result = 1;
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      result =
      LargeIntegerUnsigned::leastCommonMultiple(
        result, (*this)(i, j).getDenominator()
      );
    }
  }
  return result;
}

template <class Coefficient>
void Matrix<Coefficient>::getMatrixIntegerWithDenominator(
  Matrix<LargeInteger>& outputMatrix, LargeIntegerUnsigned& outputDenominator
) {
  outputDenominator = this->findPositiveLCMCoefficientDenominators();
  outputMatrix.initialize(this->numberOfRows, this->numberOfColumns);
  Rational product;
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      product = this->elements[i][j] * outputDenominator;
      outputMatrix(i, j) = product.getNumerator();
    }
  }
}

template <class Coefficient>
LargeInteger Matrix<Coefficient>::findPositiveGCDCoefficientNumerators() {
  LargeIntegerUnsigned result = 0;
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      const Rational& current = (*this)(i, j);
      if (current.isEqualToZero()) {
        continue;
      }
      if (result.isEqualToZero()) {
        result = current.getNumerator().value;
        continue;
      }
      result =
      LargeIntegerUnsigned::greatestCommonDivisor(
        result, current.getNumerator().value
      );
    }
  }
  if (result == 0) {
    result = 1;
  }
  return result;
}

template <class Coefficient>
void Matrix<Coefficient>::scaleToIntegralForMinimalRationalHeightNoSignChange()
{
  Rational scale;
  scale = this->findPositiveLCMCoefficientDenominators();
  scale /= this->findPositiveGCDCoefficientNumerators();
  *this *= scale;
}

template <class Coefficient>
Coefficient Matrix<Coefficient>::getDeterminant() {
  Matrix<Coefficient> matrix = *this;
  Coefficient result;
  matrix.computeDeterminantOverwriteMatrix(result);
  return result;
}

template <class Coefficient>
Coefficient Matrix<Coefficient>::trace() const {
  if (this->numberOfColumns != this->numberOfRows) {
    global.fatal
    << "This is either programming error, a mathematical error, "
    << "or requires a more general definition of trace. "
    << "Requesting the trace of "
    << "a non-square matrix of "
    << this->numberOfRows
    << " rows and "
    << this->numberOfColumns
    << " columns is not allowed. "
    << global.fatal;
  }
  Coefficient sum = 0;
  for (int i = 0; i < this->numberOfColumns; i ++) {
    sum += this->elements[i][i];
  }
  return sum;
}

template <class Coefficient>
Matrix<Coefficient> Matrix<Coefficient>::operator*(
  const Matrix<Coefficient>& right
) const {
  Matrix<Coefficient> matrix;
  matrix = right;
  matrix.multiplyOnTheLeft(*this);
  return matrix;
}

template <class Coefficient>
Vector<Coefficient> Matrix<Coefficient>::operator*(
  const Vector<Coefficient>& v
) const {
  if (v.size != this->numberOfColumns) {
    global.fatal
    << "Matrix application mismatch: matrix with "
    << this->numberOfColumns
    << " columns attempted to multiply vector of length "
    << v.size
    << ". "
    << global.fatal;
  }
  Vector<Coefficient> out;
  out.makeZero(this->numberOfRows);
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      out[i] += this->elements[i][j] * v[j];
    }
  }
  return out;
}

template <class Coefficient>
void Matrix<Coefficient>::assignMatrixIntegerWithDenominator(
  Matrix<LargeInteger>& matrix, const LargeIntegerUnsigned& denominator
) {
  this->initialize(matrix.numberOfRows, matrix.numberOfColumns);
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      this->elements[i][j] = matrix.elements[i][j];
      this->elements[i][j] /= denominator;
    }
  }
}

template <class Coefficient>
bool Vectors<Coefficient>::getNormalSeparatingCones(
  List<Vector<Coefficient> >& coneStrictlyPositiveCoefficients,
  List<Vector<Coefficient> >& coneNonNegativeCoefficients,
  Vector<Coefficient>& outputNormal
) {
  Matrix<Rational> matrixA;
  Matrix<Rational> matrixB;
  Vector<Rational> matrixX;
  int dimension = coneStrictlyPositiveCoefficients[0].size;
  if (coneStrictlyPositiveCoefficients.size == 0) {
    if (coneNonNegativeCoefficients.size > 0) {
      outputNormal.makeZero(coneNonNegativeCoefficients[0].size);
    }
    return true;
  }
  int numberOfRows =
  coneStrictlyPositiveCoefficients.size + coneNonNegativeCoefficients.size;
  matrixA.initialize(numberOfRows, dimension * 2 + numberOfRows);
  matrixA.makeZero();
  matrixB.initialize(numberOfRows, 1);
  matrixB.makeZero();
  for (int i = 0; i < coneStrictlyPositiveCoefficients.size; i ++) {
    for (int k = 0; k < dimension; k ++) {
      matrixA.elements[i][k].assign(
        coneStrictlyPositiveCoefficients.objects[i].objects[k]
      );
      matrixA.elements[i][k + dimension].assign(matrixA.elements[i][k]);
      matrixA.elements[i][k + dimension].negate();
    }
    matrixB.elements[i][0].makeOne();
    matrixA.elements[i][dimension * 2 + i].makeMinusOne();
  }
  for (int i = 0; i < coneNonNegativeCoefficients.size; i ++) {
    int currentRow = i + coneStrictlyPositiveCoefficients.size;
    for (int k = 0; k < dimension; k ++) {
      matrixA.elements[currentRow][k].assign(
        coneNonNegativeCoefficients.objects[i].objects[k]
      );
      matrixA.elements[currentRow][k + dimension].assign(
        matrixA.elements[currentRow][k]
      );
      matrixA.elements[currentRow][k + dimension].negate();
    }
    matrixA.elements[currentRow][2 * dimension + currentRow].makeOne();
  }
  bool result =
  Matrix<Rational>::
  systemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(
    matrixA, matrixB, &matrixX
  );
  outputNormal.makeZero(dimension);
  for (int i = 0; i < dimension; i ++) {
    outputNormal[i] = matrixX[i] - matrixX[i + dimension];
  }
  if (result) {
    Rational scalarProduct;
    for (int i = 0; i < coneStrictlyPositiveCoefficients.size; i ++) {
      coneStrictlyPositiveCoefficients[i].scalarEuclidean(
        outputNormal, scalarProduct
      );
      if (!scalarProduct.isPositive()) {
        global.fatal << "Unexpected non-positive value. " << global.fatal;
      }
    }
    for (int i = 0; i < coneNonNegativeCoefficients.size; i ++) {
      coneNonNegativeCoefficients[i].scalarEuclidean(
        outputNormal, scalarProduct
      );
      if (!scalarProduct.isNonPositive()) {
        global.fatal << "Unexpected positive value. " << global.fatal;
      }
    }
  }
  return result;
}

template <class Coefficient>
void Matrix<Coefficient>::computePotentialChangeGradient(
  Matrix<Coefficient>& matrixA,
  Selection& baseVariables,
  int numberOfTrueVariables,
  int columnIndex,
  Rational& outputChangeGradient,
  bool& hasAPotentialLeavingVariable
) {
  hasAPotentialLeavingVariable = false;
  outputChangeGradient.makeZero();
  for (int j = 0; j < matrixA.numberOfRows; j ++) {
    if (baseVariables.elements[j] >= numberOfTrueVariables) {
      outputChangeGradient += matrixA.elements[j][columnIndex];
    }
    hasAPotentialLeavingVariable = hasAPotentialLeavingVariable ||
    matrixA.elements[j][columnIndex].isPositive();
  }
  if (columnIndex >= numberOfTrueVariables) {
    outputChangeGradient -= 1;
  }
}

template <class Coefficient>
void Matrix<Coefficient>::getMaxMovementAndLeavingVariableRow(
  Rational& maximumMovement,
  int& leavingVariableRow,
  int enteringVariable,
  Matrix<Coefficient>& matrixA,
  Vector<Coefficient>& inputVectorX,
  Selection& baseVariables
) {
  leavingVariableRow = - 1;
  maximumMovement.makeZero();
  for (int i = 0; i < matrixA.numberOfRows; i ++) {
    Rational pivotCoefficient;
    pivotCoefficient.assign(matrixA.elements[i][enteringVariable]);
    if (pivotCoefficient.isPositive()) {
      pivotCoefficient.invert();
      pivotCoefficient.multiplyBy(inputVectorX[baseVariables.elements[i]]);
      if (
        maximumMovement.isGreaterThan(pivotCoefficient) || (
          leavingVariableRow == - 1
        )
      ) {
        maximumMovement.assign(pivotCoefficient);
        leavingVariableRow = i;
      }
    }
  }
}

template <class Coefficient>
void Matrix<Coefficient>::getEigenSpacesFromEigenvalues(
  List<Coefficient>& eigenvalues, List<Vectors<Coefficient> >& output
) const {
  output.setSize(0);
  Matrix<Coefficient> eigenspaceMatrix;
  output.setSize(eigenvalues.size);
  for (int i = 0; i < eigenvalues.size; i ++) {
    const Coefficient& eigenValue = eigenvalues[i];
    eigenspaceMatrix = *this;
    Vectors<Coefficient>& currentEigenspace = output[i];
    currentEigenspace.clear();
    eigenspaceMatrix.getEigenspaceModifyMe(eigenValue, currentEigenspace);
  }
}

template <class Coefficient>
bool Matrix<Coefficient>::
getEigenspacesProvidedAllAreIntegralWithEigenValueSmallerThanDimension(
  List<Vectors<Coefficient> >& output
) const {
  int upperLimitComputations = 100000;
  output.setSize(0);
  int found = 0;
  Polynomial<Coefficient> minimalPolynomial;
  minimalPolynomial.assignMinimalPolynomial(*this);
  Vector<Coefficient> eigenValueCandidate;
  eigenValueCandidate.setSize(1);
  Matrix<Rational> eigenSpaceMatrix;
  for (int ii = 0; ii < upperLimitComputations; ii ++) {
    int i = ((ii + 1) / 2) *(2 *(ii % 2) - 1);
    // 0, 1, - 1, 2, - 2, 3, - 3,...
    eigenValueCandidate[0] = i;
    if (minimalPolynomial.evaluate(eigenValueCandidate) == 0) {
      eigenSpaceMatrix = *this;
      output.setSize(output.size + 1);
      eigenSpaceMatrix.getEigenspaceModifyMe(
        eigenValueCandidate[0], *output.lastObject()
      );
      if (output.lastObject()->size == 0) {
        global.fatal
        << "This is a programming error: "
        << eigenValueCandidate[0].toString()
        << " is a zero of the minimal polynomial "
        << minimalPolynomial.toString()
        << " of the operator "
        << this->toString()
        << " but the corresponding eigenspace is empty. "
        << global.fatal;
      }
      found += output.lastObject()->size;
      if (found == this->numberOfColumns) {
        return true;
      }
    }
  }
  return false;
}

template <typename Coefficient>
void Matrix<Coefficient>::getEigenspaceModifyMe(
  const Coefficient& inputEigenValue,
  List<Vector<Coefficient> >& outputEigenspace
) {
  STACK_TRACE("Matrix::getEigenspaceModifyMe");
  for (int i = 0; i < this->numberOfColumns; i ++) {
    (*this)(i, i) -= inputEigenValue;
  }
  this->getZeroEigenSpaceModifyMe(outputEigenspace);
}

template <typename Coefficient>
void Matrix<Coefficient>::getZeroEigenSpaceModifyMe(
  List<Vector<Coefficient> >& output
) {
  STACK_TRACE("Matrix::getZeroEigenSpaceModifyMe");
  if (this->numberOfRows == 0) {
    output.setSize(this->numberOfColumns);
    for (int i = 0; i < this->numberOfColumns; i ++) {
      output[i].makeEi(this->numberOfColumns, i);
    }
    return;
  }
  Selection nonPivotPoints;
  this->gaussianEliminationByRows(0, &nonPivotPoints);
  output.setSize(nonPivotPoints.cardinalitySelection);
  for (int i = 0; i < nonPivotPoints.cardinalitySelection; i ++) {
    int currentPivotIndex = nonPivotPoints.elements[i];
    output[i].makeEi(this->numberOfColumns, currentPivotIndex);
    int rowCounter = 0;
    for (int j = 0; j < this->numberOfColumns; j ++) {
      if (nonPivotPoints.selected[j]) {
        continue;
      }
      output[i][j] -= this->elements[rowCounter][currentPivotIndex];
      rowCounter ++;
    }
  }
}

template <class Coefficient>
std::string Vectors<Coefficient>::toString(FormatExpressions* format) const {
  std::stringstream out;
  std::string currentString;
  bool useLaTeX = false;
  bool useHtml = false;
  bool makeTable = false;
  if (format != nullptr) {
    useLaTeX = format->flagUseLatex;
    useHtml = format->flagUseHTML;
  }
  if (!useLaTeX && !useHtml) {
    out << this->size << " vectors";
    if (this->size != 0) {
      out << ": \n";
    } else {
      out << ". ";
    }
  }
  if (useLaTeX && makeTable) {
    out << "\\begin{tabular}{c}";
  }
  if (useHtml && makeTable) {
    out << "<table>";
  }
  for (int i = 0; i < this->size; i ++) {
    currentString = this->objects[i].toString(format);
    if (useHtml && makeTable) {
      out << "<tr><td>";
    }
    out << currentString;
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

template <class Coefficient>
std::string Vector<Coefficient>::toMathMLLetterFormat(
  const std::string& inputLetter, const FormatExpressions* format
) const {
  if (this->isEqualToZero()) {
    return "<mn>0</mn>";
  }
  std::stringstream out;
  std::string term;
  bool found = false;
  out << "<mrow>";
  for (int i = 0; i < this->size; i ++) {
    if ((*this)[i].isEqualToZero()) {
      continue;
    }
    MathExpressionFormattingProperties properties;
    term = (*this)[i].toMathML(format, &properties);
    if ((*this)[i].needsParenthesisForMultiplication(format)) {
      term = MathML::leftParenthesis + term + MathML::rightParenthesis;
    }
    if (term == "<mn>1</mn>") {
      term = "";
    }
    if (term == "<mo>-</mo><mn>1</mn>") {
      term = "<mo>-</mo>";
    }
    if (found && !properties.startsWithMinus) {
      out << "<mo>+</mo>";
    }
    found = true;
    out << term;
    if (format != nullptr) {
      if (format->vectorSpaceEiBasisNames.size > i) {
        out << format->vectorSpaceEiBasisNames[i].mathML;
        continue;
      }
    }
    out
    << "<msub><mi>"
    << inputLetter
    << "</mi>"
    << "<mn>"
    << i + 1
    << "</mn></msub>";
  }
  out << "</mrow>";
  return out.str();
}

template <class Coefficient>
std::string Vector<Coefficient>::toStringLetterFormat(
  const std::string& inputLetter,
  const FormatExpressions* format,
  bool dontIncludeLastVar
) const {
  if (this->isEqualToZero()) {
    return "0";
  }
  std::stringstream out;
  std::string term;
  bool found = false;
  int numberOfVariables = dontIncludeLastVar ? this->size - 1 : this->size;
  for (int i = 0; i < numberOfVariables; i ++) {
    if (!this->objects[i].isEqualToZero()) {
      term = (*this)[i].toString(format);
      if ((*this)[i].needsParenthesisForMultiplication(format)) {
        term = "(" + term + ")";
      }
      if (term == "1") {
        term = "";
      }
      if (term == "- 1" || term == "-1") {
        term = "-";
      }
      if (found) {
        if (term.size() > 0) {
          if (term[0] != '-') {
            out << "+";
          }
        } else {
          out << "+";
        }
      }
      found = true;
      out << term;
      if (format != nullptr) {
        if (format->vectorSpaceEiBasisNames.size > i) {
          out << format->vectorSpaceEiBasisNames[i].latex;
          continue;
        }
      }
      out << inputLetter << "_{" << i + 1 << "}";
    }
  }
  return out.str();
}

template <
  class Coefficient, unsigned int inputHashFunction(const Coefficient&)
>
void MonomialTensor<Coefficient, inputHashFunction>::
multiplyByGeneratorPowerOnTheLeft(
  int generatorIndexStandsToTheLeft, const Coefficient& power
) {
  if (power == 0) {
    return;
  }
  List<int> newGeneratorIndices;
  List<Coefficient> newPowers;
  newGeneratorIndices.setExpectedSize(this->generatorsIndices.size + 1);
  newPowers.setExpectedSize(this->generatorsIndices.size + 1);
  newGeneratorIndices.addOnTop(generatorIndexStandsToTheLeft);
  newPowers.addOnTop(power);
  newGeneratorIndices.addListOnTop(this->generatorsIndices);
  newPowers.addListOnTop(this->powers);
  this->generatorsIndices = newGeneratorIndices;
  this->powers = newPowers;
  this->simplifyEqualConsecutiveGenerators(0);
}

template <
  class Coefficient, unsigned int inputHashFunction(const Coefficient&)
>
void MonomialTensor<Coefficient, inputHashFunction>::
multiplyByGeneratorPowerOnTheRight(
  int generatorIndex, const Coefficient& power
) {
  if (power == 0) {
    return;
  }
  if (this->generatorsIndices.size > 0) {
    if (*this->generatorsIndices.lastObject() == generatorIndex) {
      (*this->powers.lastObject()) += power;
      return;
    }
  }
  this->powers.addOnTop(power);
  this->generatorsIndices.addOnTop(generatorIndex);
}

template <
  class Coefficient, unsigned int inputHashFunction(const Coefficient&)
>
std::string MonomialTensor<Coefficient, inputHashFunction>::toString(
  FormatExpressions* format
) const {
  if (this->generatorsIndices.size == 0) {
    return "1";
  }
  std::string letter = format ==
  nullptr ? "g" : format->chevalleyGGeneratorLetter;
  std::string letters = "abcdefghijklmnopqrstuvwxyz";
  std::stringstream out;
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    if (static_cast<unsigned>(generatorsIndices[i]) < letters.size()) {
      out << letters[this->generatorsIndices[i]];
    } else {
      out << letter << "_{" << this->generatorsIndices[i] << "}";
    }
    out << "^{" << this->powers[i] << "}";
  }
  return out.str();
}

template <
  class Coefficient, unsigned int inputHashFunction(const Coefficient&)
>
bool MonomialTensor<Coefficient, inputHashFunction>::
simplifyEqualConsecutiveGenerators(int lowestNonReducedIndex) {
  if (this->generatorsIndices.size < 1) {
    return false;
  }
  if (lowestNonReducedIndex < 0) {
    lowestNonReducedIndex = 0;
  }
  bool result = false;
  for (
    int next = lowestNonReducedIndex + 1; next < this->generatorsIndices.
    size; next ++
  ) {
    if (
      this->generatorsIndices[lowestNonReducedIndex] ==
      this->generatorsIndices[next]
    ) {
      result = true;
      this->powers[lowestNonReducedIndex] += this->powers[next];
    } else {
      lowestNonReducedIndex ++;
      this->powers[lowestNonReducedIndex] = this->powers[next];
      this->generatorsIndices[lowestNonReducedIndex] =
      this->generatorsIndices[next];
    }
  }
  this->generatorsIndices.setSize(lowestNonReducedIndex + 1);
  this->powers.setSize(lowestNonReducedIndex + 1);
  return result;
}

template <typename Coefficient>
std::string Matrix<Coefficient>::toStringLatex(const FormatExpressions* format)
const {
  FormatExpressions formatCopy;
  if (format != nullptr) {
    formatCopy = *format;
  }
  formatCopy.flagUseLatex = true;
  formatCopy.flagUseHTML = false;
  return this->toString(&formatCopy);
}

template <typename Coefficient>
std::string Matrix<Coefficient>::toStringSystemLatex(
  Matrix<Coefficient>* constantTerms, FormatExpressions* format
) const {
  std::stringstream out;
  bool constTermsAreGood = (constantTerms == 0) ?
  false : (
    constantTerms->numberOfRows == this->numberOfRows &&
    constantTerms->numberOfColumns > 0
  );
  out << "\\begin{array}{l";
  for (int j = 0; j < this->numberOfColumns; j ++) {
    out << "l";
  }
  out << "}";
  std::string currentEntry;
  Polynomial<Coefficient> monomial;
  for (int i = 0; i < this->numberOfRows; i ++) {
    bool foundNonZeroEntry = false;
    for (int j = 0; j < this->numberOfColumns; j ++) {
      if (!((*this)(i, j) == 0)) {
        monomial.makeMonomial(j, 1, (*this)(i, j));
        currentEntry = monomial.toString(format);
        if (currentEntry == "") {
          global.fatal
          << "Empty strings not allowed "
          << "as result of toString() function call. "
          << global.fatal;
        }
        if (currentEntry[0] != '-' && foundNonZeroEntry) {
          out << "+";
        }
        out << currentEntry;
        foundNonZeroEntry = true;
      }
      if (j == this->numberOfColumns - 1 && !foundNonZeroEntry) {
        out << "0";
      }
      out << " & ";
    }
    out << "=";
    if (!constTermsAreGood) {
      out << "0";
    } else {
      out << (*constantTerms)(i, 0).toString(format);
    }
    out << "\\\\";
  }
  out << "\\end{array}";
  return out.str();
}

template <typename Coefficient>
std::string Matrix<Coefficient>::toMathML(
  const FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) const {
  (void) outputProperties;
  std::stringstream out;
  std::string coefficientString;
  int verticalLineIndex = format ==
  nullptr ? - 1 : format->matrixColumnVerticalLineIndex;
  out << "<mrow>" << MathML::leftParenthesis;
  if (verticalLineIndex == - 1) {
    out << "<mtable>";
  } else {
    out << "<mtable columnlines='";
    for (int i = 0; i < this->numberOfColumns; i ++) {
      if (i > 0) {
        out << " ";
      }
      if (i == verticalLineIndex) {
        out << "solid";
      } else {
        out << "none";
      }
    }
    out << "'>";
  }
  for (int i = 0; i < this->numberOfRows; i ++) {
    out << "<mtr>";
    for (int j = 0; j < this->numberOfColumns; j ++) {
      coefficientString = (*this)(i, j).toMathML(format);
      out << "<mtd>" << coefficientString << "</mtd>";
    }
    out << "</mtr>";
  }
  out << "</mtable>";
  out << MathML::rightParenthesis << "</mrow>";
  return out.str();
}

template <typename Coefficient>
std::string Matrix<Coefficient>::toMathMLFinal(FormatExpressions* format) const {
  return
  MathML::toMathMLFinal(
    this->toMathML(format), this->toStringLatex(format), format
  );
}

template <typename Coefficient>
std::string Matrix<Coefficient>::toString(const FormatExpressions* format)
const {
  std::stringstream out;
  std::string coefficientString;
  bool useHtml = (format == nullptr) ? true : format->flagUseHTML;
  bool useLatex = (format == nullptr) ? false : format->flagUseLatex;
  bool usePmatrix = (format == nullptr) ? true : format->flagUsePmatrix;
  if (useHtml) {
    out << "<table>";
  }
  if (useLatex) {
    int verticalLineIndex = format ==
    nullptr ? - 1 : format->matrixColumnVerticalLineIndex;
    if (usePmatrix) {
      out << "\\begin{pmatrix}";
    } else {
      out << "\\left(";
      out << "\\begin{array}{";
      for (int j = 0; j < this->numberOfColumns; j ++) {
        out << "c";
        if (verticalLineIndex == j) {
          out << "|";
        }
      }
      out << "}";
    }
  }
  for (int i = 0; i < this->numberOfRows; i ++) {
    if (useHtml) {
      out << "<tr>";
    }
    for (int j = 0; j < this->numberOfColumns; j ++) {
      coefficientString = (*this)(i, j).toString(format);
      if (useHtml) {
        out << "<td>";
      }
      out << coefficientString;
      if (useLatex) {
        if (j != this->numberOfColumns - 1) {
          out << " & ";
        } else {
          out << "\\\\";
        }
      }
      if (useHtml) {
        out << "</td>";
      }
      if (!useHtml && !useLatex) {
        out << ", ";
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

template <typename Coefficient>
std::string Matrix<Coefficient>::toStringOneLine(bool jsonFormat) const {
  std::stringstream out;
  out << "[";
  for (int i = 0; i < this->numberOfRows; i ++) {
    if (jsonFormat) {
      out << "[";
    }
    for (int j = 0; j < this->numberOfColumns; j ++) {
      out << this->elements[i][j];
      if (j != this->numberOfColumns - 1) {
        out << ", ";
      }
    }
    if (jsonFormat) {
      out << "]";
    }
    if (i != this->numberOfRows - 1) {
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

template <typename Coefficient>
std::string Matrix<Coefficient>::toStringPlainText(bool jsonFormat) const {
  List<List<std::string> > element_strings;
  element_strings.setSize(this->numberOfRows);
  int columnsPerElement = 0;
  for (int i = 0; i < this->numberOfRows; i ++) {
    element_strings[i].setSize(this->numberOfColumns);
    for (int j = 0; j < this->numberOfColumns; j ++) {
      std::stringstream ss;
      ss << this->elements[i][j];
      element_strings[i][j] = ss.str();
      int sl = element_strings[i][j].length();
      if (sl > columnsPerElement) {
        columnsPerElement = sl;
      }
    }
  }
  std::stringstream out;
  for (int i = 0; i < this->numberOfRows; i ++) {
    if (jsonFormat) {
      if (i == 0) {
        out << '[';
      } else {
        out << ' ';
      }
    }
    out << '[';
    for (int j = 0; j < this->numberOfColumns; j ++) {
      int sl = element_strings[i][j].length();
      int pad = columnsPerElement - sl;
      for (int pi = 0; pi < pad; pi ++) {
        out << ' ';
      }
      out << element_strings[i][j];
      if (j != this->numberOfColumns - 1) {
        if (jsonFormat) {
          out << ',';
        }
        out << ' ';
      }
    }
    out << ']';
    if (i != this->numberOfRows - 1) {
      out << '\n';
    } else {
      if (jsonFormat) {
        out << ']';
      }
    }
  }
  return out.str();
}

// Wall of a cone: contains the normal of the wall as
// well as the id(s) of the cones on the other side of the wall.
// The number of ids on the other side of the wall can be:
// 1) zero if the cone is on the outside of cone complex;
// 2) 1 if the cone complex is normal (every wall is exactly between
// two chambers)
// 3) 2 or more if the cone complex is not normal, in which case
// we are likely trying to normalize it and are in the middle of computations.
class Wall {
  friend std::ostream& operator<<(std::ostream& output, const Wall& wall) {
    output << wall.toString();
    return output;
  }
public:
  List<int> neighbors;
  Vector<Rational> normal;
  bool operator==(const Wall& other) const;
  bool operator>(const Wall& other) const;
  bool operator!=(const Wall& other) const;
  // Is the input point on the "inside" half-space of my wall or on the wall?
  bool isInClosedHalfSpace(const Vector<Rational>& point) const;
  // Is the input point on the "inside" half-space of my wall without
  // being on the wall?
  bool isInOpenHalfSpace(const Vector<Rational>& point) const;
  // Are all the input point on the "inside" half-space of my wall without
  // being on the wall?
  bool hasAtLeastOnePointInOpenHalfSpace(const List<Vector<Rational> >& points)
  const;
  std::string toString() const;
  bool checkConsistency() const;
};

class VectorPartition {
public:
  Vectors<Rational> partitioningVectors;
  Vector<Rational> targetSum;
  List<int> currentPartition;
  Vector<Rational> currentPartitionSum;
  // Partitions stores all generated partitions, when
  // flagStoreAllPartitions is set.
  // Format: each element of partitions gives an array whose entries give
  // the multiplicity of the weights. In other words,
  // if partitioningVectors has 2 elements,
  // then partitions[0]
  // would have 2 elements: the first giving the multiplicity of
  // partitioningVectors[0]
  // and the second - the multiplicity of
  // partitioningVectors[1].
  List<List<int> > partitions;
  // Whether or not to use the partitions array.
  bool flagStoreAllPartitions;
  VectorPartition() {
    this->flagStoreAllPartitions = false;
  }
  bool currentPartitionSumExceedsGoal() const;
  bool addOneAtIndex(int atIndex);
  bool addOne();
  void addIndex(int index, int quantity);
  std::string toStringPartitioningVectors();
  std::string toStringOnePartition(const List<int>& currentPartition);
  std::string toStringAllPartitions(bool useHtml);
  bool initialize(
    const Vectors<Rational>& inputPartitioningVectors,
    const Vector<Rational>& inputTarget
  );
  bool incrementReturnFalseIfPastLast();
  int numberOfVectors() const {
    return this->partitioningVectors.size;
  }
  int getDimension() const {
    return this->targetSum.size;
  }
  int numberOfPartitionsByEnumeration();
};

class MonomialMatrix {
  friend std::ostream& operator<<(
    std::ostream& output, const MonomialMatrix& monomial
  ) {
    output << monomial.toString();
    return output;
  }
public:
  // The monomial matrix:
  // ( 0 1 )
  // ( 0 0 )
  // has vIndex    = row index    = 0
  // and dualIndex = column index = 1
  int vIndex;
  int dualIndex;
  bool isIdentity;
  MonomialMatrix(const MonomialMatrix& other) {
    this->operator=(other);
  }
  MonomialMatrix(): vIndex(- 1), dualIndex(- 1), isIdentity(false) {}
  MonomialMatrix(int i, int j): vIndex(i), dualIndex(j), isIdentity(false) {}
  void operator=(const MonomialMatrix& other) {
    this->vIndex = other.vIndex;
    this->dualIndex = other.dualIndex;
    this->isIdentity = other.isIdentity;
  }
  void makeEij(int i, int j) {
    this->dualIndex = j;
    this->vIndex = i;
    this->isIdentity = false;
  }
  void makeOne() {
    this->makeIdentitySpecial();
  }
  void transpose() {
    MathRoutines::swap(this->vIndex, this->dualIndex);
  }
  void makeZero() {
    this->isIdentity = false;
    this->vIndex = - 1;
    this->dualIndex = - 1;
  }
  bool checkConsistency() const {
    return true;
  }
  bool isZeroMonomial() const {
    return !this->isIdentity && this->vIndex == - 1 && this->dualIndex == - 1;
  }
  bool operator==(const MonomialMatrix& other) const {
    return
    this->vIndex == other.vIndex &&
    this->dualIndex == other.dualIndex &&
    this->isIdentity == other.isIdentity;
  }
  static unsigned int hashFunction(const MonomialMatrix& input) {
    return
    static_cast<unsigned int>(input.vIndex) * HashConstants::constant0 +
    static_cast<unsigned int>(input.dualIndex) *
    HashConstants::constant1 +
    input.isIdentity;
  }
  unsigned int hashFunction() const {
    return hashFunction(*this);
  }
  bool operator>(const MonomialMatrix& other) const {
    if (this->isIdentity != other.isIdentity) {
      return this->isIdentity > other.isIdentity;
    }
    if (this->vIndex == other.vIndex) {
      return this->dualIndex > other.dualIndex;
    }
    return this->vIndex > other.vIndex;
  }
  void makeIdentitySpecial() {
    this->vIndex = - 1;
    this->dualIndex = - 1;
    this->isIdentity = true;
  }
  std::string toMathML(
    const FormatExpressions* format = nullptr,
    MathExpressionFormattingProperties* outputProperties = nullptr
  ) const {
    (void) format;
    (void) outputProperties;
    std::stringstream out;
    if (!this->isIdentity) {
      out
      << "<mrow><msub><mi>m</mi>"
      << "<mn>"
      << (this->vIndex + 1)
      << "</mn>"
      << "<mo>&otimes;</mo>"
      << "<msubsup><mi>m</mi>"
      << "<mn> "
      << (this->dualIndex + 1)
      << "</mn>"
      << "<mo>*</mo>"
      << "</mrow>";
    } else {
      out << "<mi>id</mi>";
    }
    return out.str();
  }
  std::string toString(const FormatExpressions* format = nullptr) const {
    (void) format;
    std::stringstream out;
    if (!this->isIdentity) {
      out
      << "m_{"
      << (this->vIndex + 1)
      << "}\\otimes "
      << "m^*_{"
      << (this->dualIndex + 1)
      << "}";
    } else {
      out << "id";
    }
    return out.str();
  }
  void operator*=(const MonomialMatrix& other) {
    STACK_TRACE("MonomialMatrix::operator*=");
    if (this == &other) {
      MonomialMatrix otherCopy;
      otherCopy = other;
      *this *= otherCopy;
      return;
    }
    if (other.isIdentity) {
      return;
    }
    if (this->isIdentity) {
      *this = other;
      return;
    }
    if (this->dualIndex == other.vIndex) {
      this->dualIndex = other.dualIndex;
    } else {
      this->makeZero();
    }
  }
};

class PolynomialOverModule;

template <typename Coefficient>
void Matrix<Coefficient>::getVectorFromRow(
  int rowIndex, Vector<Coefficient>& output
) const {
  output.setSize(this->numberOfColumns);
  for (int i = 0; i < this->numberOfColumns; i ++) {
    output.objects[i] = this->elements[rowIndex][i];
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::getVectorFromColumn(
  int columnIndex, Vector<Coefficient>& output
) const {
  output.setSize(this->numberOfRows);
  for (int i = 0; i < this->numberOfRows; i ++) {
    output[i] = this->elements[i][columnIndex];
  }
}

template <class Coefficient>
void Matrix<Coefficient>::substitute(
  const PolynomialSubstitution<Rational>& substitution
) {
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      this->elements[i][j].substitute(substitution, 1);
    }
  }
}

template <class Coefficient>
void Vectors<Coefficient>::intersectTwoLinearSpaces(
  const List<Vector<Coefficient> >& firstSpace,
  const List<Vector<Coefficient> >& secondSpace,
  List<Vector<Coefficient> >& output
) {
  Vectors<Coefficient> firstReduced, secondReduced;
  Selection selection;
  Vectors<Coefficient>::selectBasisInSubspace(
    firstSpace, firstReduced, selection
  );
  Vectors<Coefficient>::selectBasisInSubspace(
    secondSpace, secondReduced, selection
  );
  if (firstReduced.size == 0 || secondReduced.size == 0) {
    output.size = 0;
    return;
  }
  int dimension = firstReduced[0].size;
  Matrix<Coefficient> matrix;
  matrix.initialize(dimension, firstReduced.size + secondReduced.size);
  for (int i = 0; i < dimension; i ++) {
    for (int j = 0; j < firstReduced.size; j ++) {
      matrix(i, j) = firstReduced[j][i];
    }
    for (int j = 0; j < secondReduced.size; j ++) {
      matrix(i, firstReduced.size + j) = 0;
      matrix(i, firstReduced.size + j) -= secondReduced[j][i];
    }
  }
  matrix.gaussianEliminationByRows(0, &selection);
  output.reserve(selection.cardinalitySelection);
  output.size = 0;
  Vector<Coefficient> nextIntersection;
  for (int i = 0; i < selection.cardinalitySelection; i ++) {
    int currentIndex = selection.elements[i];
    if (currentIndex < firstReduced.size) {
      global.fatal
      << "Unexpected condition in Vectors::intersectTwoLinearSpaces. "
      << global.fatal;
    }
    nextIntersection.makeZero(dimension);
    for (int j = 0; j < firstReduced.size; j ++) {
      if (!selection.selected[j]) {
        nextIntersection +=
        firstReduced[j] * matrix.elements[j][currentIndex];
      }
    }
    output.addOnTop(nextIntersection);
  }
}

template <class Coefficient>
bool Matrix<Coefficient>::isPositiveDefinite() {
  if (this->numberOfRows != this->numberOfColumns) {
    global.fatal
    << "Attempt to evaluate whether a matrix "
    << "is positive definite, but the matrix is not square. "
    << global.fatal;
  }
  Coefficient determinant;
  Matrix<Coefficient> topLeftCorner;
  for (int i = 0; i < this->numberOfRows; i ++) {
    topLeftCorner.initialize(i + 1, i + 1);
    for (int j = 0; j < topLeftCorner.numberOfColumns; j ++) {
      for (int k = 0; k < topLeftCorner.numberOfColumns; k ++) {
        topLeftCorner.elements[j][k] = this->elements[j][k];
      }
    }
    determinant = topLeftCorner.getDeterminant();
    if (determinant <= 0) {
      return false;
    }
  }
  return true;
}

template <class Coefficient>
std::string Vectors<Coefficient>::toLatexInequalities(
  bool lastVariableIsConstant, FormatExpressions& format
) const {
  std::stringstream out;
  std::string polynomialPart;
  out << "\\begin{array}{rcl}";
  for (int i = 0; i < this->size; i ++) {
    Vector<Rational>& current = (*this)[i];
    polynomialPart =
    current.toStringLetterFormat(
      format.polynomialDefaultLetter, &format, lastVariableIsConstant
    );
    if (polynomialPart == "") {
      out << "(0";
    }
    out << polynomialPart;
    if (!lastVariableIsConstant) {
      out << "&\\geq& 0\\\\\n";
    } else {
      out << "\\geq " << (-(*current.lastObject())).toString() << "\\\\";
    }
    if (polynomialPart == "") {
      out << ")";
    }
  }
  out << "\\end{array}";
  return out.str();
}

#endif // header_math_general_ALREADY_INCLUDED
