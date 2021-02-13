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
  friend std::ostream& operator<< <Coefficient>(std::ostream& output, const Vector<Coefficient>& theVector);
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
    const std::string& inputLetter, FormatExpressions* format = nullptr, bool DontIncludeLastVar = false
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
    Coefficient tempRat, accumRow;
    result = 0;
    for (int i = 0; i < bilinearForm.numberOfRows; i ++) {
      accumRow = 0;
      for (int j = 0; j < bilinearForm.numberOfColumns; j ++) {
        tempRat = r2[j];
        tempRat *= bilinearForm.elements[i][j];
        accumRow += tempRat;
      }
      accumRow *= r1[i];
      result += accumRow;
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
  static Vector<Coefficient> getEi(int theDimension, int theIndex) {
    Vector<Coefficient> output;
    output.makeEi(theDimension, theIndex);
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
  template <class otherType>
  otherType scalarEuclidean(const Vector<otherType>& other) const {
    otherType output;
    this->scalarEuclidean(other, output);
    return output;
  }
  template <class otherType>
  void scalarEuclidean(const Vector<otherType>& other, otherType& output) const {
    if (this->size != other.size) {
      global.fatal << "Attempt to take scalar product of elements of different dimensions: "
      << *this << " and " << other << ". "
      << global.fatal;
    }
    otherType tempElt;
    output = 0;
    for (int i = 0; i < this->size; i ++) {
      tempElt = other[i];
      tempElt *= this->objects[i];
      output += tempElt;
    }
  }
  static void projectOntoHyperPlane(
    Vector<Coefficient>& input,
    Vector<Coefficient>& normal,
    Vector<Coefficient>& projectionDirection,
    Vector<Coefficient>& output
  ) {
    Coefficient t;
    Coefficient tempRat;
    input.scalarEuclidean(normal, t);
    projectionDirection.scalarEuclidean(normal, tempRat);
    t /= tempRat;
    t.negate();
    Vector<Coefficient>::vectorPlusVectorTimesScalar(input, projectionDirection, t, output);
  }
  static void vectorPlusVectorTimesScalar(
    const Vector<Coefficient>& r1,
    const Vector<Coefficient>& r2,
    const Coefficient& theCoeff,
    Vector<Coefficient>& output
  ) {
    Coefficient tempRat;
    if (r1.size != r2.size) {
      global.fatal << "Adding vectors of different dimensions. " << global.fatal;
    }
    output = r1;
    for (int i = 0; i < r1.size; i ++) {
      tempRat = r2[i];
      tempRat *= theCoeff;
      output.objects[i] += (tempRat);
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
    return input.hashFunction();
  }
  unsigned int hashFunction() const {
    unsigned int result = 0;
    int theSize = MathRoutines::minimum(this->size, someRandomPrimesSize);
    for (int i = 0; i < theSize; i ++) {
      result += this->objects[i].hashFunction() * ::someRandomPrimes[i];
    }
    return result;
  }
  void scaleToFirstNonZeroCoordinatePositive();
  // Returns the number by which the vector was multiplied.
  void scaleNormalizeFirstNonZero();
  void makeAffineUsingLastCoordinate() {
    Coefficient theElt;
    theElt = *this->lastObject();
    this->size --;
    this->operator/=(theElt);
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
  void makeZero(int theDim) {
    this->setSize(theDim);
    for (int i = 0; i < theDim; i ++) {
      (*this)[i] = 0;
    }
  }
  void makeZero(int theDim, const Coefficient& zeroCoefficient) {
    this->setSize(theDim);
    for (int i = 0; i < theDim; i ++) {
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
  bool projectToAffineSpace(Vector<Coefficient> &output) {
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
  Vector<Coefficient> getshiftToTheLeft(int numPositions) {
    Vector<Coefficient> result;
    if (numPositions > this->size) {
      global.fatal << "You requested a shift of "
      << numPositions << " positions in a vector with "
      << this->size << "elements" << global.fatal;
    }
    result.setSize(this->size - numPositions);
    for (int i = 0; i < result.size; i ++) {
      result[i] = this->objects[i + numPositions];
    }
    return result;
  }
  inline void shiftToTheLeftOnePosition() {
    this->shiftToTheLeft(1);
  }
  void shiftToTheLeft(int numPositions) {
    if (numPositions > this->size) {
      global.fatal << "Bad vector shift. " << global.fatal;
    }
    for (int i = 0; i < this->size - numPositions; i ++) {
      this->objects[i] = this->objects[i + numPositions];
    }
    this->size -= numPositions;
  }
  void shiftToTheRightInsertZeroes(int numPositions, const Coefficient& ringZero) {
    if (numPositions < 0) {
      global.fatal << "Bad vector shift, cannot fill with zeroes. " << global.fatal;
    }
    this->setSize(this->size + numPositions);
    for (int i = this->size - 1; i >= numPositions; i --) {
      this->objects[i] = this->objects[i - numPositions];
    }
    for (int i = 0; i < numPositions; i ++) {
      this->objects[i] = ringZero;
    }
  }
  void setDimensionInsertZeroes(int newDim) {
    int oldDim = this->size;
    this->setSize(newDim);
    for (int i = oldDim; i < newDim; i ++) {
      (*this)[i] = 0;
    }
  }
  bool getIntegralCoordsInBasisIfTheyExist(
    const Vectors<Coefficient>& inputBasis,
    Vector<Coefficient>& output,
    Matrix<Coefficient>& bufferMatGaussianEliminationCC,
    Matrix<Coefficient>& bufferMatGaussianElimination,
    const Coefficient& ringUnit,
    const Coefficient& theRingMinusUnit,
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
  bool findIndexFirstNonZeroCoordinateFromTheLeft(int& theIndex) {
    theIndex = - 1;
    for (int i = 0; i < this->size; i ++) {
      if (!this->objects[i].isEqualToZero()) {
        theIndex = i;
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
  bool IsGreaterThanLexicographic(const Vector<Coefficient>& other) const {
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
  LargeIntegerUnsigned tempI, tempI2;
  output.makeOne();
  for (int i = 0; i < this->size; i ++) {
    this->objects[i].getDenominator(tempI2);
    LargeIntegerUnsigned::greatestCommonDivisor(output, tempI2, tempI);
    output.multiplyBy(tempI2);
    output.divPositive(tempI, output, tempI2);
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
    bool useLatex, bool useHtml, bool LastVarIsConstant, FormatExpressions& theFormat
  ) const;
  std::string toString(FormatExpressions* format = nullptr) const;
  bool linearAlgebraForVertexComputation(
    Selection& theSelection,
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
  int arrangeFirstVectorsBeOfMaxPossibleRank(Matrix<Coefficient>& bufferMat, Selection& bufferSel);
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
  void selectionToMatrix(Selection& theSelection, int outputDimension, Matrix<Coefficient>& output);
  void selectionToMatrixAppend(Selection& theSelection, int outputDimension, Matrix<Coefficient>& output, int startRowIndex);
  void selectionToMatrix(Selection& theSelection, int outputDimension, Matrix<Coefficient>& output, int startRowIndex);
  void getGramMatrix(Matrix<Coefficient>& output, const Matrix<Rational>* theBilinearForm = nullptr) const;
  void getMatrixRootsToRows(Matrix<Rational>& output) const;
  void getOrthogonalComplement(Vectors<Coefficient>& output, Matrix<Rational>* bilinearForm = nullptr);
  bool linearSpanContainsVector(
    const Vector<Coefficient>& input, Matrix<Coefficient>& bufferMatrix, Selection& bufferSelection
  ) const;
  void makeEiBasis(int theDimension) {
    this->setSize(theDimension);
    for (int i = 0; i < this->size; i ++) {
      this->objects[i].makeEi(theDimension, i);
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
  void average(Vector<Coefficient>& output, int theDimension) {
    this->sum(output, theDimension);
    if (this->size == 0) {
      return;
    }
    output /= this->size;
  }
  void sum(Vector<Coefficient>& output, int resultDim) const {
    output.makeZero(resultDim);
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
    const Coefficient& theRingMinusUnit,
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
        theRingMinusUnit,
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
    const Coefficient& theRingMinusUnit,
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
        theRingMinusUnit,
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
    Selection& theSelection,
    Matrix<Coefficient>& bufferMatrix,
    int theDimension
  ) const;
  bool computeNormalFromSelectionAndExtraRoot(
    Vector<Coefficient>& output,
    Vector<Coefficient>& ExtraRoot,
    Selection& theSelection,
    Matrix<Coefficient>& bufferMatrix,
    Selection& bufferSel
  );
  bool computeNormalFromSelectionAndTwoExtraRoots(
    Vector<Coefficient>& output,
    Vector<Coefficient>& extraRoot1,
    Vector<Coefficient>& extraRoot2,
    Selection& theSelection,
    Matrix<Coefficient>& bufferMat,
    Selection& bufferSel
  );
  bool computeNormal(Vector<Coefficient>& output, int inputDimension);
  void gaussianEliminationForNormalComputation(
    Matrix<Coefficient>& inputMatrix, Selection& outputNonPivotPoints, int theDimension
  ) const;
  //the below function returns a n row 1 column matrix with the coefficients in the obvious order
  bool getLinearDependence(Matrix<Coefficient>& outputTheLinearCombination);
  void getLinearDependenceRunTheLinearAlgebra(Matrix<Coefficient>& outputTheSystem, Selection& outputNonPivotPoints);
  bool containsVectorNonPerpendicularTo(const Vector<Coefficient>& input, const Matrix<Coefficient>& theBilinearForm);
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
  void assignMatrixColumns(Matrix<Coefficient>& mat) {
    Vector<Coefficient> tempRoot;
    this->setSize(mat.numberOfColumns);
    tempRoot.setSize(mat.numberOfRows);
    for (int i = 0; i < mat.numberOfColumns; i ++) {
      for (int j = 0; j < mat.numberOfRows; j ++) {
        tempRoot[j] = mat.elements[j][i];
      }
      this->objects[i] = tempRoot;
    }
  }
  void assignMatrixRows(const Matrix<Coefficient>& mat) {
    this->size = 0;
    this->setSize(mat.numberOfRows);
    for (int i = 0; i < mat.numberOfRows; i ++) {
      this->objects[i].setSize(mat.numberOfColumns);
      for (int j = 0; j < mat.numberOfColumns; j ++) {
        this->objects[i].objects[j] = mat.elements[i][j];
      }
    }
  }
  int getDimension() const {
    if (this->size > 0) {
      return this->objects[0].size;
    }
    return - 1;
  }
  void beefUpWithEiToLinearlyIndependentBasis(int theDim);
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
  Matrix<Coefficient> bufferMat;
  if (this->size != inputBasis[0].size) {
    global.fatal << "Attempt to get coordinates of vector of "
    << this->size << " coordinates using a basis whose first vector has "
    << inputBasis[0].size << " coordinates." << global.fatal;
  }
  bufferVectors.reserve(inputBasis.size + 1);
  bufferVectors.addListOnTop(inputBasis);
  bufferVectors.addOnTop(*this);
  if (!bufferVectors.getLinearDependence(bufferMat)) {
    return false;
  }
  Coefficient tempCF = bufferMat(bufferMat.numberOfRows - 1, 0);
  bufferMat /= tempCF;
  output.setSize(bufferMat.numberOfRows - 1);
  for (int i = 0; i < bufferMat.numberOfRows - 1; i ++) {
    bufferMat(i, 0).negate();
    output[i] = bufferMat(i, 0);
  }
  return true;
}

template <class Coefficient>
void Vectors<Coefficient>::getLinearDependenceRunTheLinearAlgebra(
  Matrix<Coefficient>& outputTheSystem, Selection& outputNonPivotPoints
) {
  MacroRegisterFunctionWithName("Vectors::getLinearDependenceRunTheLinearAlgebra");
  if (this->size == 0) {
    return;
  }
  int dimension = (*this)[0].size;
  outputTheSystem.initialize(dimension, this->size);
  for (int i = 0; i < this->size; i ++) {
    for (int j = 0; j < dimension; j ++) {
      outputTheSystem(j, i) = (*this)[i][j];
    }
  }
  outputTheSystem.gaussianEliminationByRows(0, &outputNonPivotPoints);
}

template <class Coefficient>
bool Vectors<Coefficient>::linearSpanContainsVector(
  const Vector<Coefficient>& input, Matrix<Coefficient>& bufferMatrix, Selection& bufferSelection
) const {
  Vectors<Coefficient> tempVectors;
  tempVectors = (*this);
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
  const Coefficient& theRingMinusUnit,
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
    &bufferMatGaussianEliminationCC, theRingMinusUnit, ringUnit
  );
  Vector<Coefficient> tempRoot, theCombination;
  if (this == &output) {
    global.fatal << "Output not allowed to coincide with this object." << global.fatal;
  }
  output.makeZero(inputBasis.size);
  theCombination = *this;
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
    theCombination -= tempRoot;
  }
  if (!theCombination.isEqualToZero()) {
    return false;
  }
  bufferMatGaussianEliminationCC.actMultiplyVectorRowOnTheRight(output);
  return true;
}

template<class Coefficient>
void Vectors<Coefficient>::getGramMatrix(Matrix<Coefficient>& output, const Matrix<Rational>* theBilinearForm) const {
  output.resize(this->size, this->size, false);
  for (int i = 0; i < this->size; i ++) {
    for (int j = i; j < this->size; j ++) {
      if (theBilinearForm != nullptr) {
        Vector<Coefficient>::scalarProduct(this->objects[i], this->objects[j], *theBilinearForm, output.elements[i][j]);
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
  const Vector<Coefficient>& input, const Matrix<Coefficient>& theBilinearForm
) {
  for (int i = 0; i < this->size; i ++) {
    if (!Vector<Coefficient>::scalarProduct(this->objects[i], input, theBilinearForm).isEqualToZero()) {
      return true;
    }
  }
  return false;
}

template<class Coefficient>
int Vectors<Coefficient>::arrangeFirstVectorsBeOfMaxPossibleRank(Matrix<Coefficient>& bufferMat, Selection& bufferSel) {
  if (this->size == 0) {
    return 0;
  }
  int theDimension = this->getDimensionOfElements();
  Vectors<Rational> tempRoots;
  int oldRank = 0;
  for (int i = 0; i < this->size; i ++) {
    tempRoots.addOnTop(this->objects[i]);
    int newRank = tempRoots.getRankElementSpan(bufferMat, bufferSel);
    if (newRank == oldRank) {
      tempRoots.removeIndexSwapWithLast(tempRoots.size - 1);
    } else {
      this->swapTwoIndices(oldRank, i);
      if (oldRank + 1 != newRank) {
        global.fatal << "Old rank plus one must equal new rank. " << global.fatal;
      }
      oldRank = newRank;
    }
    if (oldRank == theDimension) {
      return theDimension;
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
  bool containsPoint(Vector<Coefficient> & thePoint);
  void makeFromNormalAndPoint(Vector<Coefficient>& inputPoint, Vector<Coefficient>& inputNormal);
  bool hasCommonPointWithPositiveTwoToTheNthQuadrant();
  bool operator==(const AffineHyperplane& right);
};

template <class Coefficient>
bool AffineHyperplane<Coefficient>::operator==(const AffineHyperplane& right) {
  Vector<Rational> tempRoot1, tempRoot2;
  tempRoot1 = this->normal;
  tempRoot2 = right.normal;
  tempRoot1.scaleNormalizeFirstNonZero();
  tempRoot2.scaleNormalizeFirstNonZero();
  if (!(tempRoot1 == tempRoot2)) {
    return false;
  }
  Rational tempRat1, tempRat2;
  tempRoot1.scalarEuclidean(this->affinePoint, tempRat1);
  tempRoot1.scalarEuclidean(right.affinePoint, tempRat2);
  return tempRat1.isEqualTo(tempRat2);
}

template <class Coefficient>
Vector<Coefficient> AffineHyperplane<Coefficient>::projectOnMe(Vector<Coefficient>& input) const {
  // output = input + x * normal  and <input + x * normal, normal> = 0 =>
  // x = -<input, normal> / <normal, normal>
  Coefficient theNormalCoeff = - input.scalarEuclidean(this->normal) / this->normal.scalarEuclidean(this->normal);
  Vector<Coefficient> output;
  output = input + this->normal * theNormalCoeff;
  return output;
}

template <class Coefficient>
bool AffineHyperplane<Coefficient>::projectFromFacetNormal(Vector<Coefficient>& input) {
  int tempI = input.getIndexFirstNonZeroCoordinate();
  if (tempI == - 1) {
    global.fatal << "No non-zero coordinate found. " << global.fatal;
  }
  if (tempI == input.size - 1) {
    return false;
  }
  this->affinePoint.makeZero(input.size);
  this->affinePoint.setSize(input.size - 1);
  this->affinePoint[tempI] = input[input.size - 1];
  this->affinePoint[tempI].negate();
  this->affinePoint[tempI].divideBy(input[tempI]);
  this->normal = input;
  this->normal.setSize(input.size - 1);
  return true;
}

template <class Coefficient>
bool AffineHyperplane<Coefficient>::containsPoint(Vector<Coefficient>& thePoint) {
  Rational tempRat1, tempRat2;
  tempRat1 = this->normal.scalarEuclidean(thePoint);
  tempRat2 = this->normal.scalarEuclidean(this->affinePoint);
  return tempRat2.isEqualTo(tempRat1);
}

template <class Coefficient>
bool AffineHyperplane<Coefficient>::hasCommonPointWithPositiveTwoToTheNthQuadrant() {
  Rational tempRat;
  tempRat = this->normal.scalarEuclidean(this->affinePoint);
  if (tempRat.isEqualToZero()) {
    return true;
  }
  for (int i = 0; i < this->normal.size; i ++) {
    Rational& tempRat2 = this->normal[i];
    if (tempRat.isNegative() && tempRat2.isNegative()) {
      return true;
    }
    if (tempRat.isPositive() && tempRat2.isPositive()) {
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
  Vector<Rational> tempNormal;
  tempNormal = this->normal;
  tempNormal.scaleNormalizeFirstNonZero();
  Rational tempRat = this->normal.scalarEuclidean(this->affinePoint);
  return this->normal.hashFunction() + tempRat.hashFunction();
}

class AffineHyperplanes: public List<AffineHyperplane<Rational> > {
public:
  void toString(std::string& output);
};

class AffineCone {
public:
  AffineHyperplanes theWalls;
  unsigned int hashFunction() const;
  inline static unsigned int hashFunction(const AffineCone& input) {
    return input.hashFunction();
  }
  int getDimension();
  void superimposeAffineCones(AffineCones& theOtherComplex);
  bool wallIsInternalInCone(AffineHyperplane<Rational>& theKillerCandidate);
  // The below function returns true if the system of homogeneous linear inequalities Ax<=b
  // has a solution, false otherwise, where A is a matrix and x and b are column vectors.
  // bool systemLinearInequalitiesHasSolution
  //    (Matrix<Rational> & matA, Matrix<Rational> & matb, Matrix<Rational> & outputPoint);
  bool splitByAffineHyperplane(AffineHyperplane<Rational>& theKillerPlane, AffineCones& output);
};

class AffineCones: public HashedList<AffineCone> {
public:
  void superimposeAffineCones(AffineCones& theOtherComplex);
};


#endif
