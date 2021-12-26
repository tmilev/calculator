// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderMathVectors_h_already_included
#define vpfHeaderMathVectors_h_already_included

#include "general_logging_global_variables.h"
#include "math_large_integers.h"

class SelectionWithMultiplicities;
class Selection;

template <class Coefficient>
class Vector: public List<Coefficient> {
  friend Vector<Coefficient> operator-(const Vector<Coefficient>& input) {
    Vector<Coefficient> result;
    result.setSize(input.size);
    for (int i = 0; i < input.size; i ++) {
      result[i] = - input[i];
    }
    return result;
  }
  friend std::ostream& operator<< <Coefficient>(std::ostream& output, const Vector<Coefficient>& toBeWritten);
public:
  Vector(){}
  Vector(const Vector<Coefficient>& other): List<Coefficient>(other) {
    *this = other;
  }
  template <class otherCoeff>
  Vector(const Vector<otherCoeff>& other) {
    *this = other;
  }
  Vector(const Selection& other) {
    *this = other;
  }
  std::string toString(FormatExpressions* format = nullptr) const {
    std::stringstream out;
    out.precision(5);
    out << "(";
    for (int i = 0; i < this->size; i ++) {
      out << (*this)[i].toString(format);
      if (i != this->size - 1) {
        out << ", ";
      }
    }
    out << ")";
    return out.str();
  }
  void toListStringsBasicType(List<std::string>& output) const {
    output.setSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      std::stringstream out;
      out.precision(5);
      out << (*this)[i];
      output[i] = out.str();
    }
  }
  std::string toStringSquareBrackets(FormatExpressions* format = nullptr) const {
    std::stringstream out;
    out.precision(5);
    out << "[";
    for (int i = 0; i < this->size; i ++) {
      out << (*this)[i].toString(format);
      if (i != this->size - 1) {
        out << ", ";
      }
    }
    out << "]";
    return out.str();
  }
  std::string toStringSquareBracketsBasicType(FormatExpressions* format = nullptr) const {
    (void) format;
    std::stringstream out;
    out.precision(5);
    out << "[";
    for (int i = 0; i < this->size; i ++) {
      out << std::to_string((*this)[i]);
      if (i != this->size - 1) {
        out << ", ";
      }
    }
    out << "]";
    return out.str();
  }
  std::string toStringLetterFormat(
    const std::string& inputLetter, FormatExpressions* format = nullptr, bool dontIncludeLastVar = false
  ) const;
  std::string toStringEpsilonFormat(FormatExpressions* format = nullptr) const {
    return this->toStringLetterFormat("\\varepsilon", format);
  }
  template <class otherType>
  static void scalarProduct(
    const Vector<Coefficient>& r1,
    const Vector<Coefficient>& r2,
    const Matrix<otherType>& bilinearForm,
    Coefficient& result
  ) {
    if (r1.size != bilinearForm.numberOfRows || r1.size != r2.size || r1.size != bilinearForm.numberOfColumns) {
      global.fatal << "Attempt to take "
      << "a bilinear form represented by matrix with " << bilinearForm.numberOfRows
      << " rows and " << bilinearForm.numberOfColumns << " columns "
      << "of vectors of dimension " << r1.size << " and " << r2.size << ". "
      << global.fatal;
    }
    Coefficient summand;
    Coefficient rowAccumulator;
    result = 0;
    for (int i = 0; i < bilinearForm.numberOfRows; i ++) {
      rowAccumulator = 0;
      for (int j = 0; j < bilinearForm.numberOfColumns; j ++) {
        summand = r2[j];
        summand *= bilinearForm.elements[i][j];
        rowAccumulator += summand;
      }
      rowAccumulator *= r1[i];
      result += rowAccumulator;
    }
  }
  template <class otherType>
  static Coefficient scalarProduct(
    const Vector<Coefficient>& r1, const Vector<Coefficient>& r2, const Matrix<otherType>& bilinearForm
  ) {
    Coefficient result;
    Vector<Coefficient>::scalarProduct(r1, r2, bilinearForm, result);
    return result;
  }
  void perturbNoZeroScalarProductWithMe(const List<Vector<Coefficient> >& inputVectors);
  void perturbNormalRelativeToVectorsInGeneralPosition(
    const Vectors<Rational>& nonStrictConeNonPositiveScalar,
    const List<Vector<Rational> >& vectorsToBeInGeneralPosition
  );
  Coefficient scalarProduct (const Vector<Coefficient>& r2, const Matrix<Coefficient>& form) const {
    return scalarProduct(*this, r2, form);
  }
  bool isPositive() {
    return this->isPositiveOrZero() && !this->isEqualToZero();
  }
  bool isNegative() {
    return this->isNegativeOrZero() && !this->isEqualToZero();
  }
  Vector<Rational> getVectorRational() const {
    Vector<Rational> result;
    result.setSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      result[i] = (*this)[i].rationalValue();
    }
    return result;
  }
  Vector<double> getVectorDouble() const {
    Vector<double> result;
    result.setSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      result[i] = this->objects[i].getDoubleValue();
    }
    return result;
  }
  bool IsIntegral() {
    for (int i = 0; i < this->size; i ++) {
      if (!this->objects[i].isInteger()) {
        return false;
      }
    }
    return true;
  }
  static Vector<Coefficient> getEi(int dimension, int index) {
    Vector<Coefficient> output;
    output.makeEi(dimension, index);
    return output;
  }
  bool isNegativeOrZero() {
    for (int i = 0; i < this->size; i ++) {
      if (this->objects[i] > 0) {
        return false;
      }
    }
    return true;
  }
  bool isPositiveOrZero() const {
    for (int i = 0; i < this->size; i ++) {
      if (this->objects[i] < 0) {
        return false;
      }
    }
    return true;
  }
  Coefficient sumCoordinates() const {
    Coefficient result = 0;
    for (int i = 0; i < this->size; i ++) {
      result += this->objects[i];
    }
    return result;
  }
  template <class Type>
  Type scalarEuclidean(const Vector<Type>& other) const {
    Type output;
    this->scalarEuclidean(other, output);
    return output;
  }
  template <class Type>
  void scalarEuclidean(const Vector<Type>& other, Type& output) const {
    if (this->size != other.size) {
      global.fatal << "Attempt to take scalar product of elements of different dimensions: "
      << *this << " and " << other << ". "
      << global.fatal;
    }
    Type element;
    output = 0;
    for (int i = 0; i < this->size; i ++) {
      element = other[i];
      element *= this->objects[i];
      output += element;
    }
  }
  static void projectOntoHyperPlane(
    Vector<Coefficient>& input,
    Vector<Coefficient>& normal,
    Vector<Coefficient>& projectionDirection,
    Vector<Coefficient>& output
  ) {
    Coefficient t;
    Coefficient scalar;
    input.scalarEuclidean(normal, t);
    projectionDirection.scalarEuclidean(normal, scalar);
    t /= scalar;
    t.negate();
    Vector<Coefficient>::vectorPlusVectorTimesScalar(input, projectionDirection, t, output);
  }
  static void vectorPlusVectorTimesScalar(
    const Vector<Coefficient>& r1,
    const Vector<Coefficient>& r2,
    const Coefficient& coefficient,
    Vector<Coefficient>& output
  ) {
    Coefficient scalar;
    if (r1.size != r2.size) {
      global.fatal << "Adding vectors of different dimensions. " << global.fatal;
    }
    output = r1;
    for (int i = 0; i < r1.size; i ++) {
      scalar = r2[i];
      scalar *= coefficient;
      output.objects[i] += scalar;
    }
  }
  void negate() {
    for (int i = 0; i < this->size; i ++) {
      this->objects[i] *= - 1;
    }
  }
  void makeEi(int desiredDimension, int nonZeroIndex) {
    this->makeZero(desiredDimension);
    (*this)[nonZeroIndex] = 1;
  }
  inline static unsigned int hashFunction(const Vector<Coefficient>& input) {
    return input.::List<Coefficient>::hashFunction();
  }
  unsigned int hashFunction() const {
    return this->::List<Coefficient>::hashFunction();
  }
  void scaleToFirstNonZeroCoordinatePositive();
  // Returns the number by which the vector was multiplied.
  void scaleNormalizeFirstNonZero();
  void makeAffineUsingLastCoordinate() {
    Coefficient element;
    element = *this->lastObject();
    this->size --;
    this->operator/=(element);
  }
  bool assignMatrixDetectRowOrColumn(const Matrix<Coefficient>& input) {
    if (input.numberOfColumns == 1) {
      this->setSize(input.numberOfRows);
      for (int i = 0; i < this->size; i ++) {
        this->objects[i] = input.elements[i][0];
      }
      return true;
    }
    if (input.numberOfRows == 1) {
      this->setSize(input.numberOfColumns);
      for (int i = 0; i < this->size; i ++) {
        this->objects[i] = input.elements[0][i];
      }
      return true;
    }
    return false;
  }
  void makeZero(int dimension) {
    this->setSize(dimension);
    for (int i = 0; i < dimension; i ++) {
      (*this)[i] = 0;
    }
  }
  void makeZero(int dimension, const Coefficient& zeroCoefficient) {
    this->setSize(dimension);
    for (int i = 0; i < dimension; i ++) {
      (*this)[i] = zeroCoefficient;
    }
  }
  int getNumberOfNonZeroCoordinates() const {
    int result = 0;
    for (int i = 0; i < this->size; i ++) {
      if (!this->objects[i].isEqualToZero()) {
        result ++;
      }
    }
    return result;
  }
  bool isProportionalTo(const Vector<Coefficient>& other, Coefficient& outputTimesMeEqualsOther) const;
  bool isProportionalTo(const Vector<Coefficient>& other) const {
    Coefficient outputTimesMeEqualsOther;
    return this->isProportionalTo(other, outputTimesMeEqualsOther);
  }
  int findLeastCommonMultipleDenominatorsTruncateToInt();
  void findLeastCommonMultipleDenominators(LargeIntegerUnsigned& output);
  inline Vector<Coefficient> getshiftToTheLeftOnePositionition() {
    return this->getshiftToTheLeft(1);
  }
  void makeNormalInProjectivizationFromPointAndNormal(Vector<Coefficient>& point, Vector<Coefficient>& normal) {
    //the extra dimension is going to be the last dimension
    int newDimension = normal.size + 1;
    this->setSize(newDimension);
    this->RootScalarEuclideanRoot(normal, point, this->objects[newDimension - 1]);
    this->objects[newDimension - 1].negate();
    for (int j = 0; j < newDimension - 1; j ++) {
      this->objects[j] = normal[j];
    }
  }
  bool projectToAffineSpace(Vector<Coefficient>& output) {
    if (this->objects[this->size - 1].isEqualToZero()) {
      return false;
    }
    output.setSize(this->size - 1);
    for (int i = 0; i < this->size - 1; i ++) {
      output[i] = this->objects[i];
    }
    output /= this->objects[this->size - 1];
    return true;
  }
  bool makeAffineProjectionFromNormal(AffineHyperplane<Rational>& output);
  int getIndexFirstNonZeroCoordinate() const {
    for (int i = 0; i < this->size; i ++) {
      if (!this->objects[i].isEqualToZero()) {
        return i;
      }
    }
    return - 1;
  }
  int getIndexLastNonZeroCoordinate() {
    for (int i = this->size - 1; i >= 0; i --) {
      if (!this->objects[i].isEqualToZero()) {
        return i;
      }
    }
    return - 1;
  }
  Vector<Coefficient> getshiftToTheLeft(int numberOfPositions) {
    Vector<Coefficient> result;
    if (numberOfPositions > this->size) {
      global.fatal << "You requested a shift of "
      << numberOfPositions << " positions in a vector with "
      << this->size << "elements" << global.fatal;
    }
    result.setSize(this->size - numberOfPositions);
    for (int i = 0; i < result.size; i ++) {
      result[i] = this->objects[i + numberOfPositions];
    }
    return result;
  }
  inline void shiftToTheLeftOnePosition() {
    this->shiftToTheLeft(1);
  }
  void shiftToTheLeft(int numberOfPositions) {
    if (numberOfPositions > this->size) {
      global.fatal << "Bad vector shift. " << global.fatal;
    }
    for (int i = 0; i < this->size - numberOfPositions; i ++) {
      this->objects[i] = this->objects[i + numberOfPositions];
    }
    this->size -= numberOfPositions;
  }
  void shiftToTheRightInsertZeroes(int numberOfPositions, const Coefficient& ringZero) {
    if (numberOfPositions < 0) {
      global.fatal << "Bad vector shift, cannot fill with zeroes. " << global.fatal;
    }
    this->setSize(this->size + numberOfPositions);
    for (int i = this->size - 1; i >= numberOfPositions; i --) {
      this->objects[i] = this->objects[i - numberOfPositions];
    }
    for (int i = 0; i < numberOfPositions; i ++) {
      this->objects[i] = ringZero;
    }
  }
  void setDimensionInsertZeroes(int newDimension) {
    int oldDimension = this->size;
    this->setSize(newDimension);
    for (int i = oldDimension; i < newDimension; i ++) {
      (*this)[i] = 0;
    }
  }
  bool getIntegralCoordsInBasisIfTheyExist(
    const Vectors<Coefficient>& inputBasis,
    Vector<Coefficient>& output,
    Matrix<Coefficient>& bufferMatGaussianEliminationCC,
    Matrix<Coefficient>& bufferMatGaussianElimination,
    const Coefficient& ringUnit,
    const Coefficient& negativeOne,
    const Coefficient& ringZero
  );
  void GetVectorsPerpendicularTo(
    Vectors<Coefficient>& output
  ) {
    int Pivot = - 1;
    if (!this->findIndexFirstNonZeroCoordinateFromTheLeft(Pivot)) {
      output.makeEiBasis(this->size);
      return;
    }
    output.setSize(this->size - 1);
    for (int i = 0; i < this->size; i ++) {
      if (i != Pivot) {
        Vector<Coefficient>& current = output.objects[i];
        current.makeEi(this->size, i);
        current.objects[Pivot] -= this->objects[i];
      }
    }
  }
  bool findIndexFirstNonZeroCoordinateFromTheLeft(int& index) {
    index = - 1;
    for (int i = 0; i < this->size; i ++) {
      if (!this->objects[i].isEqualToZero()) {
        index = i;
        return true;
      }
    }
    return false;
  }
  bool getCoordinatesInBasis(const Vectors<Coefficient>& inputBasis, Vector<Coefficient>& output) const;
  Vector<Coefficient> GetProjectivizedNormal(Vector<Coefficient>& affinePoint);
  Vector<Coefficient> operator*(const Coefficient& other) const {
    Vector<Coefficient> result;
    result.setSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      result[i] = this->objects[i];
      result[i] *= other;
    }
    return result;
  }
  Vector<Coefficient> operator/(const Coefficient& other) const {
    if (other.isEqualToZero()) {
      global.fatal << "Division by zero. "
      << "Division by zero error are supposed to be handled at an earlier level. " << global.fatal;
    }
    Vector<Coefficient> result;
    result.setSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      result[i] = (*this)[i];
      result[i] /= other;
    }
    return result;
  }
  void operator*=(const Coefficient& other) {
    for (int i = 0; i < this->size; i ++) {
      this->objects[i] *= other;
    }
  }
  void operator/=(const Coefficient& other) {
    for (int i = 0; i < this->size; i ++) {
      this->objects[i] /= other;
    }
  }
  template <class otherType>
  void operator+=(const Vector<otherType>& other) {
    for (int i = 0; i < this->size; i ++) {
      (*this)[i] += other[i];
    }
  }
  inline bool operator<(const Vector<Coefficient>& other) const {
    return other > *this;
  }
  bool isGreaterThanLexicographic(const Vector<Coefficient>& other) const {
    return this->::List<Coefficient>::operator>(other);
  }
  bool operator>(const Vector<Coefficient>& other) const {
    if (this->size != other.size) {
      global.fatal << "Comparing vectors with different number of coordinates, namely, "
      << this->toString() << " and " << other.toString() << ". " << global.fatal;
    }
    Coefficient c1 = 0, c2 = 0;
    for (int i = 0; i < this->size; i ++) {
      c1 += this->objects[i];
      c2 += other.objects[i];
    }
    if (c1 > c2) {
      return true;
    }
    if (c2 > c1) {
      return false;
    }
    for (int i = this->size - 1; i >= 0; i --) {
      if (this->objects[i] > other.objects[i]) {
        return true;
      }
      if (other.objects[i] > this->objects[i]) {
        return false;
      }
    }
    return false;
  }
  template <class otherType>
  void operator-=(const Vector<otherType>& other) {
    if (this->size != other.size) {
      global.fatal << "Subtracting vectors with different dimensions. " << global.fatal;
    }
    for (int i = 0; i < this->size; i ++) {
      this->objects[i] -= other[i];
    }
  }
  Vector<Coefficient> operator+(const Vector<Coefficient>& right) const {
    Vector<Coefficient> result = *this;
    result += right;
    return result;
  }
  bool assignString(const std::string& input);

  void operator=(const std::string& input) {
    this->assignString(input);
  }
  // The following function is required else
  // some compilers generate warning:
  // 'implicitly declared Vector<Rational>& Vector<Rational>::operator=(...) is deprecated'
  Vector<Coefficient>& operator=(const Vector<Coefficient>& other) {
    if (this == &other) {
      return *this;
    }
    this->setSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->objects[i] = other[i];
    }
    return *this;
  }
  template <class otherType>
  Vector<Coefficient>& operator=(const Vector<otherType>& other) {
    if (this == reinterpret_cast<const Vector<Coefficient>*>(&other)) {
      return *this;
    }
    this->setSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->objects[i] = other[i];
    }
    return *this;
  }
  template <class otherType>
  Vector<Coefficient>& operator=(const List<otherType>& other) {
    this->setSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->objects[i] = other[i];
    }
    return *this;
  }
  void operator=(const Selection& other);
  void operator=(const SelectionWithMultiplicities& other);
  bool operator!=(const Vector<Coefficient>& other) const {
    return !(*this == other);
  }
  bool isEqualToZero() const {
    for (int i = 0; i < this->size; i ++) {
      if (!this->objects[i].isEqualToZero()) {
        return false;
      }
    }
    return true;
  }
  Vector<Coefficient> operator-(const Vector<Coefficient>& other) const {
    Vector<Coefficient> result;
    result = *this;
    result -= other;
    return result;
  }
};

template <class Coefficient>
void Vector<Coefficient>::scaleNormalizeFirstNonZero() {
  int index = this->getIndexFirstNonZeroCoordinate();
  if (index < 0) {
    return;
  }
  Coefficient::scaleNormalizeIndex(*this, index);
}

template <class Coefficient>
void Vector<Coefficient>::scaleToFirstNonZeroCoordinatePositive() {
  for (int i = 0; i < this->size; i ++) {
    if ((*this)[i].isPositive()) {
      return;
    }
    if ((*this)[i].isNegative()) {
      this->negate();
      return;
    }
  }
}

template <class Coefficient>
bool Vector<Coefficient>::isProportionalTo(
  const Vector<Coefficient>& input,
  Coefficient& outputTimesMeEqualsInput
) const {
  if (this->size != input.size) {
    return false;
  }
  int indexFirstNonZero = - 1;
  for (int i = 0; i < this->size; i ++) {
    if (!this->objects[i].isEqualToZero()) {
      indexFirstNonZero = i;
      break;
    }
  }
  if (indexFirstNonZero == - 1) {
    if (input.isEqualToZero()) {
      outputTimesMeEqualsInput.makeZero();
      return true;
    }
    return false;
  }
  Vector<Rational> tempRoot = *this;
  outputTimesMeEqualsInput = input[indexFirstNonZero];
  outputTimesMeEqualsInput /= (*this)[indexFirstNonZero];
  tempRoot *= outputTimesMeEqualsInput;
  return tempRoot == input;
}

template <class Coefficient>
void Vector<Coefficient>::findLeastCommonMultipleDenominators(LargeIntegerUnsigned& output) {
  LargeIntegerUnsigned quotient;
  LargeIntegerUnsigned remainder;
  output.makeOne();
  for (int i = 0; i < this->size; i ++) {
    this->objects[i].getDenominator(remainder);
    LargeIntegerUnsigned::greatestCommonDivisor(output, remainder, quotient);
    output.multiplyBy(remainder);
    output.dividePositive(quotient, output, remainder);
  }
}

template <class Coefficient>
int Vector<Coefficient>::findLeastCommonMultipleDenominatorsTruncateToInt() {
  int result = 1;
  for (int i = 0; i < this->size; i ++) {
    result = MathRoutines::leastCommonMultiple(result, this->objects[i].denominatorShort);
    if ((*this)[i].extended != 0) {
      global.fatal
      << "Coefficient is large rational at a "
      << "place where that is not allowed. "
      << global.fatal;
    }
  }
  return result;
}

template <class Coefficient>
class Vectors: public List<Vector<Coefficient> > {
  public:
  std::string toStringEpsilonForm(bool useLatex, bool useHtml, bool makeTable) {
    std::string tempS;
    std::stringstream out;
    if (useLatex) {
      useHtml = false;
    }
    if (useHtml && makeTable) {
      out << "<table>";
    }
    if (useLatex && makeTable) {
      out << "\\begin{array}{l}";
    }
    for (int i = 0; i < this->size; i ++) {
      tempS = this->objects[i].toStringEpsilonFormat();
      if (useHtml && makeTable) {
        out << "<tr><td>";
      }
      if (!useLatex && useHtml) {
        out << "(";
      }
      out << tempS;
      if (!useLatex && useHtml) {
        out << ")";
      }
      if (useLatex && makeTable) {
        out << "\\\\";
      }
      if (useHtml && makeTable) {
        out << "</td></tr>";
      }
      if (!makeTable) {
        if (i != this->size - 1) {
          out << ", ";
        }
      } else {
        out << "\n";
      }
    }
    if (useLatex && makeTable) {
      out << "\\end{array}";
    }
    if (useHtml && makeTable) {
      out << "</table>";
    }
    return out.str();
  }
  std::string toInequalitiesString(
    bool useLatex, bool useHtml, bool lastVariableIsConstant, FormatExpressions& format
  ) const;
  std::string toString(FormatExpressions* format = nullptr) const;
  bool linearAlgebraForVertexComputation(
    Selection& selection,
    Vector<Coefficient>& output,
    Matrix<Coefficient>& buffer,
    Selection& nonPivotPointsBuffer
  );
  void getVectorsDouble(Vectors<double>& output) const {
    output.setSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      output[i] = this->objects[i].getVectorDouble();
    }
  }
  void assignListListCoefficientType(const List<List<Coefficient> >& input) {
    this->setSize(input.size);
    for (int i = 0; i < input.size; i ++) {
      this->objects[i] = input[i];
    }
  }
  void assignListList(const List<Vectors<Coefficient> >& input) {
    int count = 0;
    for (int i = 0; i < input.size; i ++) {
      count += input[i].size;
    }
    this->setSize(0);
    this->reserve(count);
    for (int i = 0; i < input.size; i ++) {
      for (int j = 0; j < input[i].size; j ++) {
        this->addOnTop(input[i][j]);
      }
    }
  }
  std::string toLinearCombinationString() {
    std::stringstream out;
    std::string tempS;
    for (int i = 0; i < this->size; i ++) {
      this->objects[i].toString(tempS);
      out << tempS;
      if (i != this->size - 1) {
        out << " + ";
      }
    }
    return out.str();
  }
  int arrangeFirstVectorsBeOfMaxPossibleRank(Matrix<Coefficient>& bufferMatrix, Selection& bufferSelection);
  static unsigned int hashFunction(const Vectors<Coefficient>& input) {
    return input.hashFunction();
  }
  unsigned int hashFunction() const {
    return this->::List<Vector<Coefficient> >::hashFunction();
  }
  bool hasAnElementWithPositiveScalarProduct(const Vector<Coefficient>& input) const {
    for (int i = 0; i < this->size; i ++) {
      if (input.scalarEuclidean(this->objects[i]).isPositive()) {
        return true;
      }
    }
    return false;
  }
  bool hasAnElementWithNegativeScalarProduct(const Vector<Coefficient>& input) const {
    for (int i = 0; i < this->size; i ++) {
      if (input.scalarEuclidean(this->objects[i]).isNegative()) {
        return true;
      }
    }
    return false;
  }
  bool hasAnElementPerpendicularTo(const Vector<Coefficient>& input) const {
    for (int i = 0; i < this->size; i ++) {
      if (input.scalarEuclidean(this->objects[i]).isEqualToZero()) {
        return true;
      }
    }
    return false;
  }
  void selectionToMatrix(Selection& selection, int outputDimension, Matrix<Coefficient>& output);
  void selectionToMatrixAppend(Selection& selection, int outputDimension, Matrix<Coefficient>& output, int startRowIndex);
  void selectionToMatrix(Selection& selection, int outputDimension, Matrix<Coefficient>& output, int startRowIndex);
  void getGramMatrix(Matrix<Coefficient>& output, const Matrix<Rational>* bilinearForm = nullptr) const;
  void getMatrixRootsToRows(Matrix<Rational>& output) const;
  void getOrthogonalComplement(Vectors<Coefficient>& output, Matrix<Coefficient>* bilinearForm = nullptr);
  bool linearSpanContainsVector(
    const Vector<Coefficient>& input, Matrix<Coefficient>& bufferMatrix, Selection& bufferSelection
  ) const;
  void makeEiBasis(int dimension) {
    this->setSize(dimension);
    for (int i = 0; i < this->size; i ++) {
      this->objects[i].makeEi(dimension, i);
    }
  }
  bool linearSpanContainsVector(const Vector<Coefficient>& input) const;
  static void selectBasisInSubspace(
    const List<Vector<Coefficient> >& input, List<Vector<Coefficient> >& output, Selection& outputSelectedPivots
  );
  int getRankElementSpan(Matrix<Coefficient>* buffer = 0, Selection* bufferSelection = nullptr) const;
  static bool conesIntersect(
    List<Vector<Rational> >& strictCone,
    List<Vector<Rational> >& nonStrictCone,
    Vector<Rational>* outputLinearCombo = nullptr,
    Vector<Rational>* outputSplittingNormal = nullptr
  );
  static bool getNormalSeparatingCones(
    List<Vector<Coefficient> >& coneStrictlyPositiveCoeffs,
    List<Vector<Coefficient> >& coneNonNegativeCoeffs,
    Vector<Coefficient>& outputNormal
  );
  void average(Vector<Coefficient>& output, int dimension) {
    this->sum(output, dimension);
    if (this->size == 0) {
      return;
    }
    output /= this->size;
  }
  void sum(Vector<Coefficient>& output, int resultDimension) const {
    output.makeZero(resultDimension);
    for (int i = 0; i < this->size; i ++) {
      output += this->objects[i];
    }
  }
  bool getCoordinatesInBasis(
    const Vectors<Coefficient>& inputBasis, Vectors<Coefficient>& outputCoords
  ) const;
  bool getIntegralCoordsInBasisIfTheyExist(
    const Vectors<Coefficient>& inputBasis,
    Vectors<Coefficient>& output,
    const Coefficient& ringUnit,
    const Coefficient& negativeOne,
    const Coefficient& ringZero
  ) const {
    Matrix<Coefficient> bufferMatGaussianEliminationCC, bufferMatGaussianElimination;
    bool result = true;
    output.setSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      if (!this->objects[i].getIntegralCoordsInBasisIfTheyExist(
        inputBasis,
        output.objects[i],
        bufferMatGaussianEliminationCC,
        bufferMatGaussianElimination,
        ringUnit,
        negativeOne,
        ringZero
      )) {
        result = false;
      }
    }
    return result;
  }
  bool getIntegralCoordsInBasisIfTheyExist(
    const Vectors<Coefficient>& inputBasis,
    Vectors<Coefficient>& output,
    Matrix<Coefficient>& bufferMatGaussianEliminationCC,
    Matrix<Coefficient>& bufferMatGaussianElimination,
    const Coefficient& ringUnit,
    const Coefficient& negativeOne,
    const Coefficient& ringZero
  ) const {
    bool result = true;
    output.setSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      if (!this->objects[i].getIntegralCoordsInBasisIfTheyExist(
        inputBasis,
        output.objects[i],
        bufferMatGaussianEliminationCC,
        bufferMatGaussianElimination,
        ringUnit,
        negativeOne,
        ringZero
      )) {
        result = false;
      }
    }
    return result;
  }
  std::string toStringLetterFormat(const std::string& inputLetter, bool useLatex) {
    std::stringstream out;
    for (int i = 0; i < this->size; i ++) {
      out << this->objects[i].toStringLetterFormat(inputLetter, useLatex);
      if (i != this->size - 1) {
        out << ",";
      }
    }
    return out.str();
  }
  bool computeNormalExcludingIndex(
    Vector<Coefficient>& output, int index, Matrix<Coefficient>& bufferMatrix
  );
  bool computeNormalFromSelection(
    Vector<Coefficient>& output,
    Selection& selection,
    Matrix<Coefficient>& bufferMatrix,
    int dimension
  ) const;
  bool computeNormalFromSelectionAndExtraRoot(
    Vector<Coefficient>& output,
    Vector<Coefficient>& extraRoot,
    Selection& selection,
    Matrix<Coefficient>& bufferMatrix,
    Selection& bufferSelection
  );
  bool computeNormalFromSelectionAndTwoExtraRoots(
    Vector<Coefficient>& output,
    Vector<Coefficient>& extraRoot1,
    Vector<Coefficient>& extraRoot2,
    Selection& selection,
    Matrix<Coefficient>& bufferMatrix,
    Selection& bufferSelection
  );
  bool computeNormal(Vector<Coefficient>& output, int inputDimension);
  void gaussianEliminationForNormalComputation(
    Matrix<Coefficient>& inputMatrix, Selection& outputNonPivotPoints, int dimension
  ) const;
  //the below function returns a n row 1 column matrix with the coefficients in the obvious order
  bool getLinearDependence(Matrix<Coefficient>& outputLinearCombination);
  void getLinearDependenceCompute(Matrix<Coefficient>& outputSystem, Selection& outputNonPivotPoints);
  bool containsVectorNonPerpendicularTo(const Vector<Coefficient>& input, const Matrix<Coefficient>& bilinearForm);
  bool containsOppositeRoots() {
    if (this->size < 10) {
      Vector<Rational> tempRoot;
      for (int i = 0; i < this->size; i ++) {
        tempRoot = this->objects[i];
        tempRoot.negate();
        for (int j = i + 1; j < this->size; j ++) {
          if (this->objects[j].isEqualTo(tempRoot)) {
            return true;
          }
        }
      }
      return false;
    }
    HashedList<Vector<Coefficient> > tempList;
    tempList.setExpectedSize(this->size);
    tempList = *this;
    for (int i = 0; i < this->size; i ++) {
      if (tempList.contains(- this->objects[i])) {
        return true;
      }
    }
    return false;
  }
  void assignMatrixColumns(Matrix<Coefficient>& matrix) {
    Vector<Coefficient> tempRoot;
    this->setSize(matrix.numberOfColumns);
    tempRoot.setSize(matrix.numberOfRows);
    for (int i = 0; i < matrix.numberOfColumns; i ++) {
      for (int j = 0; j < matrix.numberOfRows; j ++) {
        tempRoot[j] = matrix.elements[j][i];
      }
      this->objects[i] = tempRoot;
    }
  }
  void assignMatrixRows(const Matrix<Coefficient>& input) {
    this->size = 0;
    this->setSize(input.numberOfRows);
    for (int i = 0; i < input.numberOfRows; i ++) {
      this->objects[i].setSize(input.numberOfColumns);
      for (int j = 0; j < input.numberOfColumns; j ++) {
        this->objects[i].objects[j] = input.elements[i][j];
      }
    }
  }
  int getDimension() const {
    if (this->size > 0) {
      return this->objects[0].size;
    }
    return - 1;
  }
  void beefUpWithEiToLinearlyIndependentBasis(int dimension);
  void chooseABasis();
  static void intersectTwoLinearSpaces(
    const List<Vector<Coefficient> >& firstSpace,
    const List<Vector<Coefficient> >& secondSpace,
    List<Vector<Coefficient> >& output
  );
  void operator=(const List<List<Coefficient> >& other) {
    this->setSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->objects[i] = other.objects[i];
    }
  }
  template <class otherType>
  void operator=(const List<Vector<otherType> >& other) {
    this->setSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->objects[i] = other[i];
    }
  }
};

template <class Coefficient>
bool Vector<Coefficient>::assignString(const std::string& input) {
  MacroRegisterFunctionWithName("Vector::assignString");
  this->setSize(0);
  int currentDigit = - 1;
  int sign = 1;
  for (unsigned i = 0; i < input.size(); i ++) {
    char previous = (i == 0) ? '(' : input[i - 1];
    if (MathRoutines::isADigit(input[i], &currentDigit)) {
      if (!MathRoutines::isADigit(previous)) {
        this->addOnTop(Coefficient(0));
      }
      *this->lastObject() *= 10;
      *this->lastObject() += currentDigit * sign;
    } else if (input[i] == '-') {
      sign = - 1;
    } else if (input[i] == ',') {
      sign = 1;
    }
  }
  return true;
}

template <class Coefficient>
bool Vector<Coefficient>::getCoordinatesInBasis(const Vectors<Coefficient>& inputBasis, Vector<Coefficient>& output) const {
  if (inputBasis.size == 0) {
    return false;
  }
  MacroRegisterFunctionWithName("Vector::getCoordinatesInBasis");
  Vectors<Coefficient> bufferVectors;
  Matrix<Coefficient> bufferMatrix;
  if (this->size != inputBasis[0].size) {
    global.fatal << "Attempt to get coordinates of vector of "
    << this->size << " coordinates using a basis whose first vector has "
    << inputBasis[0].size << " coordinates." << global.fatal;
  }
  bufferVectors.reserve(inputBasis.size + 1);
  bufferVectors.addListOnTop(inputBasis);
  bufferVectors.addOnTop(*this);
  if (!bufferVectors.getLinearDependence(bufferMatrix)) {
    return false;
  }
  Coefficient temporaryCoefficient = bufferMatrix(bufferMatrix.numberOfRows - 1, 0);
  bufferMatrix /= temporaryCoefficient;
  output.setSize(bufferMatrix.numberOfRows - 1);
  for (int i = 0; i < bufferMatrix.numberOfRows - 1; i ++) {
    bufferMatrix(i, 0).negate();
    output[i] = bufferMatrix(i, 0);
  }
  return true;
}

template <class Coefficient>
void Vectors<Coefficient>::getLinearDependenceCompute(
  Matrix<Coefficient>& outputSystem, Selection& outputNonPivotPoints
) {
  MacroRegisterFunctionWithName("Vectors::getLinearDependenceCompute");
  if (this->size == 0) {
    return;
  }
  int dimension = (*this)[0].size;
  outputSystem.initialize(dimension, this->size);
  for (int i = 0; i < this->size; i ++) {
    for (int j = 0; j < dimension; j ++) {
      outputSystem(j, i) = (*this)[i][j];
    }
  }
  outputSystem.gaussianEliminationByRows(0, &outputNonPivotPoints);
}

template <class Coefficient>
bool Vectors<Coefficient>::linearSpanContainsVector(
  const Vector<Coefficient>& input, Matrix<Coefficient>& bufferMatrix, Selection& bufferSelection
) const {
  MacroRegisterFunctionWithName("Vectors::linearSpanContainsVector");
  Vectors<Coefficient> tempVectors;
  tempVectors = *this;
  tempVectors.addOnTop(input);
  return this->getRankElementSpan(&bufferMatrix, &bufferSelection) ==
  tempVectors.getRankElementSpan(&bufferMatrix, &bufferSelection);
}

template <class Coefficient>
bool Vectors<Coefficient>::getCoordinatesInBasis(
  const Vectors<Coefficient>& inputBasis, Vectors<Coefficient>& outputCoords
) const {
  MacroRegisterFunctionWithName("Vectors::getCoordinatesInBasis");
  outputCoords.setSize(this->size);
  for (int i = 0; i < this->size; i ++) {
    if (!(this->operator[](i).getCoordinatesInBasis(inputBasis, outputCoords[i]))) {
      return false;
    }
  }
  return true;
}

template <class Coefficient>
bool Vector<Coefficient>::getIntegralCoordsInBasisIfTheyExist(
  const Vectors<Coefficient>& inputBasis,
  Vector<Coefficient>& output,
  Matrix<Coefficient>& bufferMatGaussianEliminationCC,
  Matrix<Coefficient>& bufferMatGaussianElimination,
  const Coefficient& ringUnit,
  const Coefficient& negativeOne,
  const Coefficient& ringZero
) {
  int dimension = this->size;
  bufferMatGaussianElimination.initialize(inputBasis.size, dimension);
  for (int i = 0; i < inputBasis.size; i ++) {
    for (int j = 0; j < dimension; j ++) {
      bufferMatGaussianElimination.elements[i][j] = inputBasis[i][j];
    }
  }
  bufferMatGaussianEliminationCC.makeIdentityMatrix(bufferMatGaussianElimination.numberOfRows, ringUnit, ringZero);
  bufferMatGaussianElimination.gaussianEliminationEuclideanDomain(
    &bufferMatGaussianEliminationCC, negativeOne, ringUnit
  );
  Vector<Coefficient> tempRoot, combination;
  if (this == &output) {
    global.fatal << "Output not allowed to coincide with this object." << global.fatal;
  }
  output.makeZero(inputBasis.size);
  combination = *this;
  int column = 0;
  for (int i = 0; i < inputBasis.size; i ++) {
    for (; column < dimension; column ++) {
      if (!bufferMatGaussianElimination.elements[i][column].isEqualToZero()) {
        break;
      }
    }
    if (column >= dimension) {
      break;
    }
    bufferMatGaussianElimination.getVectorFromRow(i, tempRoot);
    output[i] = this->objects[column];
    output[i] /= bufferMatGaussianElimination.elements[i][column];
    tempRoot *= output[i];
    combination -= tempRoot;
  }
  if (!combination.isEqualToZero()) {
    return false;
  }
  bufferMatGaussianEliminationCC.actMultiplyVectorRowOnTheRight(output);
  return true;
}

template<class Coefficient>
void Vectors<Coefficient>::getGramMatrix(Matrix<Coefficient>& output, const Matrix<Rational>* bilinearForm) const {
  output.resize(this->size, this->size, false);
  for (int i = 0; i < this->size; i ++) {
    for (int j = i; j < this->size; j ++) {
      if (bilinearForm != nullptr) {
        Vector<Coefficient>::scalarProduct(this->objects[i], this->objects[j], *bilinearForm, output.elements[i][j]);
      } else {
        output(i, j) = (*this)[i].scalarEuclidean((*this)[j]);
      }
      if (i != j) {
        output(j, i) = output(i, j);
      }
    }
  }
}

template<class Coefficient>
bool Vectors<Coefficient>::containsVectorNonPerpendicularTo(
  const Vector<Coefficient>& input, const Matrix<Coefficient>& bilinearForm
) {
  for (int i = 0; i < this->size; i ++) {
    if (!Vector<Coefficient>::scalarProduct(this->objects[i], input, bilinearForm).isEqualToZero()) {
      return true;
    }
  }
  return false;
}

template<class Coefficient>
int Vectors<Coefficient>::arrangeFirstVectorsBeOfMaxPossibleRank(
  Matrix<Coefficient>& bufferMatrix, Selection& bufferSelection
) {
  if (this->size == 0) {
    return 0;
  }
  int dimension = this->getDimensionOfElements();
  Vectors<Rational> tempRoots;
  int oldRank = 0;
  for (int i = 0; i < this->size; i ++) {
    tempRoots.addOnTop(this->objects[i]);
    int newRank = tempRoots.getRankElementSpan(bufferMatrix, bufferSelection);
    if (newRank == oldRank) {
      tempRoots.removeIndexSwapWithLast(tempRoots.size - 1);
    } else {
      this->swapTwoIndices(oldRank, i);
      if (oldRank + 1 != newRank) {
        global.fatal << "Old rank plus one must equal new rank. " << global.fatal;
      }
      oldRank = newRank;
    }
    if (oldRank == dimension) {
      return dimension;
    }
  }
  return oldRank;
}

template <class Coefficient>
class AffineHyperplane {
public:
  Vector<Coefficient> affinePoint;
  Vector<Coefficient> normal;
  void toString(std::string& output);
  // Returns false if the projection is not of full dimension.
  unsigned int hashFunction() const;
  static inline unsigned int hashFunction(const AffineHyperplane& input) {
    return input.hashFunction();
  }
  bool projectFromFacetNormal(Vector<Coefficient>& input);
  Vector<Coefficient> projectOnMe(Vector<Coefficient>& input) const;
  bool containsPoint(Vector<Coefficient>& point);
  void makeFromNormalAndPoint(Vector<Coefficient>& inputPoint, Vector<Coefficient>& inputNormal);
  bool hasCommonPointWithPositiveTwoToTheNthQuadrant();
  bool operator==(const AffineHyperplane& right);
};

template <class Coefficient>
bool AffineHyperplane<Coefficient>::operator==(const AffineHyperplane& right) {
  Vector<Rational> leftNormal;
  Vector<Rational> rightNormal;
  leftNormal = this->normal;
  rightNormal = right.normal;
  leftNormal.scaleNormalizeFirstNonZero();
  rightNormal.scaleNormalizeFirstNonZero();
  if (!(leftNormal == rightNormal)) {
    return false;
  }
  Rational scalarProductLeft;
  Rational scalarProductRight;
  leftNormal.scalarEuclidean(this->affinePoint, scalarProductLeft);
  leftNormal.scalarEuclidean(right.affinePoint, scalarProductRight);
  return scalarProductLeft.isEqualTo(scalarProductRight);
}

template <class Coefficient>
Vector<Coefficient> AffineHyperplane<Coefficient>::projectOnMe(Vector<Coefficient>& input) const {
  // output = input + x * normal  and <input + x * normal, normal> = 0 =>
  // x = -<input, normal> / <normal, normal>
  Coefficient normalCoefficient = - input.scalarEuclidean(this->normal) / this->normal.scalarEuclidean(this->normal);
  Vector<Coefficient> output;
  output = input + this->normal * normalCoefficient;
  return output;
}

template <class Coefficient>
bool AffineHyperplane<Coefficient>::projectFromFacetNormal(Vector<Coefficient>& input) {
  int index = input.getIndexFirstNonZeroCoordinate();
  if (index == - 1) {
    global.fatal << "No non-zero coordinate found. " << global.fatal;
  }
  if (index == input.size - 1) {
    return false;
  }
  this->affinePoint.makeZero(input.size);
  this->affinePoint.setSize(input.size - 1);
  this->affinePoint[index] = input[input.size - 1];
  this->affinePoint[index].negate();
  this->affinePoint[index].divideBy(input[index]);
  this->normal = input;
  this->normal.setSize(input.size - 1);
  return true;
}

template <class Coefficient>
bool AffineHyperplane<Coefficient>::containsPoint(Vector<Coefficient>& point) {
  Rational scalarProductPoint, scalarProductInternalPoint;
  scalarProductPoint = this->normal.scalarEuclidean(point);
  scalarProductInternalPoint = this->normal.scalarEuclidean(this->affinePoint);
  return scalarProductInternalPoint.isEqualTo(scalarProductPoint);
}

template <class Coefficient>
bool AffineHyperplane<Coefficient>::hasCommonPointWithPositiveTwoToTheNthQuadrant() {
  Rational scalarProductWithInternal;
  scalarProductWithInternal = this->normal.scalarEuclidean(this->affinePoint);
  if (scalarProductWithInternal.isEqualToZero()) {
    return true;
  }
  for (int i = 0; i < this->normal.size; i ++) {
    Rational& scalarProduct = this->normal[i];
    if (scalarProductWithInternal.isNegative() && scalarProduct.isNegative()) {
      return true;
    }
    if (scalarProductWithInternal.isPositive() && scalarProduct.isPositive()) {
      return true;
    }
  }
  return false;
}

template <class Coefficient>
void AffineHyperplane<Coefficient>::makeFromNormalAndPoint(
  Vector<Coefficient>& inputPoint, Vector<Coefficient>& inputNormal
) {
  this->affinePoint = inputPoint;
  this->normal = inputNormal;
}

template <class Coefficient>
void AffineHyperplane<Coefficient>::toString(std::string& output) {
  std::stringstream out;
  out << "point: " << this->affinePoint << ", normal: " << this->normal;
  output = out.str();
}

template <class Coefficient>
unsigned int AffineHyperplane<Coefficient>::hashFunction() const {
  // warning: if normal gets streched, the hashfunction should not change!
  Vector<Rational> rescaledNormal;
  rescaledNormal = this->normal;
  rescaledNormal.scaleNormalizeFirstNonZero();
  Rational scalarProduct = this->normal.scalarEuclidean(this->affinePoint);
  return this->normal.hashFunction() + scalarProduct.hashFunction();
}

class AffineHyperplanes: public List<AffineHyperplane<Rational> > {
public:
  void toString(std::string& output);
};

class AffineCone {
public:
  AffineHyperplanes walls;
  unsigned int hashFunction() const;
  inline static unsigned int hashFunction(const AffineCone& input) {
    return input.hashFunction();
  }
  int getDimension();
  void superimposeAffineCones(AffineCones& otherComplex);
  bool wallIsInternalInCone(AffineHyperplane<Rational>& killerCandidate);
  // The below function returns true if the system of homogeneous linear inequalities Ax<=b
  // has a solution, false otherwise, where A is a matrix and x and b are column vectors.
  // bool systemLinearInequalitiesHasSolution
  //    (Matrix<Rational> & matA, Matrix<Rational> & matb, Matrix<Rational> & outputPoint);
  bool splitByAffineHyperplane(AffineHyperplane<Rational>& killerPlane, AffineCones& output);
};

class AffineCones: public HashedList<AffineCone> {
public:
  void superimposeAffineCones(AffineCones& theOtherComplex);
};

#endif
