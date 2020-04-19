// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderMathVectors_h_already_included
#define vpfHeaderMathVectors_h_already_included

#include "general_logging_global_variables.h"
#include "math_large_integers.h"

class SelectionWithMultiplicities;
class Selection;

template <class coefficient>
class Vector: public List<coefficient> {
  friend Vector<coefficient> operator-(const Vector<coefficient>& input) {
    Vector<coefficient> result;
    result.setSize(input.size);
    for (int i = 0; i < input.size; i ++) {
      result[i] = - input[i];
    }
    return result;
  }
  friend std::ostream& operator<< <coefficient>(std::ostream& output, const Vector<coefficient>& theVector);
public:
  Vector(){}
  Vector(const Vector<coefficient>& other): List<coefficient>(other) {
    *this = other;
  }
  template <class otherCoeff>
  Vector(const Vector<otherCoeff>& other) {
    *this = other;
  }
  Vector(const Selection& other) {
    *this = other;
  }
  std::string toString(FormatExpressions* theFormat = nullptr) const {
    std::stringstream out;
    out.precision(5);
    out << "(";
    for (int i = 0; i < this->size; i ++) {
      out << (*this)[i].toString(theFormat);
      if (i != this->size - 1) {
        out << ", ";
      }
    }
    out << ")";
    return out.str();
  }
  void ToListStringsBasicType(List<std::string>& output) const {
    output.setSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      std::stringstream out;
      out.precision(5);
      out << (*this)[i];
      output[i] = out.str();
    }
  }
  std::string ToStringSquareBrackets(FormatExpressions* theFormat = nullptr) const {
    std::stringstream out;
    out.precision(5);
    out << "[";
    for (int i = 0; i < this->size; i ++) {
      out << (*this)[i].toString(theFormat);
      if (i != this->size - 1) {
        out << ", ";
      }
    }
    out << "]";
    return out.str();
  }
  std::string ToStringSquareBracketsBasicType(FormatExpressions* theFormat = nullptr) const {
    (void) theFormat;
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
  std::string ToStringLetterFormat(
    const std::string& inputLetter, FormatExpressions* theFormat = nullptr, bool DontIncludeLastVar = false
  ) const;
  std::string ToStringEpsilonFormat(FormatExpressions* theFormat = nullptr) const {
    return this->ToStringLetterFormat("\\varepsilon", theFormat);
  }
  template <class otherType>
  static void ScalarProduct(
    const Vector<coefficient>& r1,
    const Vector<coefficient>& r2,
    const Matrix<otherType>& TheBilinearForm,
    coefficient& result
  ) {
    if (r1.size != TheBilinearForm.NumRows || r1.size != r2.size || r1.size != TheBilinearForm.NumCols) {
      global.fatal << "This is a programming error: attempting to take "
      << "a bilinear form represented by matrix with " << TheBilinearForm.NumRows
      << " rows and " << TheBilinearForm.NumCols << " columns "
      << " of vectors of dimension " << r1.size << " and " << r2.size << ". "
      << global.fatal;
    }
    coefficient tempRat, accumRow;
    result = 0;
    for (int i = 0; i < TheBilinearForm.NumRows; i ++) {
      accumRow = 0;
      for (int j = 0; j < TheBilinearForm.NumCols; j ++) {
        tempRat = r2[j];
        tempRat *= TheBilinearForm.elements[i][j];
        accumRow += tempRat;
      }
      accumRow *= r1[i];
      result += accumRow;
    }
  }
  template <class otherType>
  static coefficient ScalarProduct(
    const Vector<coefficient>& r1, const Vector<coefficient>& r2, const Matrix<otherType>& TheBilinearForm
  ) {
    coefficient result;
    Vector<coefficient>::ScalarProduct(r1, r2, TheBilinearForm, result);
    return result;
  }
  void PerturbNoZeroScalarProductWithMe(const List<Vector<coefficient> >& inputVectors);
  void PerturbNormalRelativeToVectorsInGeneralPosition(
    const Vectors<Rational>& NonStrictConeNonPositiveScalar,
    const List<Vector<Rational> >& VectorsToBeInGeneralPosition
  );
  coefficient ScalarProduct (const Vector<coefficient>& r2, const Matrix<coefficient>& form) const {
    return ScalarProduct(*this, r2, form);
  }
  bool IsPositive() {
    return this->IsPositiveOrZero() && !this->IsEqualToZero();
  }
  bool IsNegative() {
    return this->IsNegativeOrZero() && !this->IsEqualToZero();
  }
  Vector<Rational> GetVectorRational() const {
    Vector<Rational> result;
    result.setSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      result[i] = (*this)[i].RationalValue();
    }
    return result;
  }
  Vector<double> GetVectorDouble() const {
    Vector<double> result;
    result.setSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      result[i] = this->theObjects[i].GetDoubleValue();
    }
    return result;
  }
  bool IsIntegral() {
    for (int i = 0; i < this->size; i ++) {
      if (!this->theObjects[i].IsInteger()) {
        return false;
      }
    }
    return true;
  }
  static Vector<coefficient> GetEi(int theDimension, int theIndex) {
    Vector<coefficient> output;
    output.MakeEi(theDimension, theIndex);
    return output;
  }
  bool IsNegativeOrZero() {
    for (int i = 0; i < this->size; i ++) {
      if (this->theObjects[i] > 0) {
        return false;
      }
    }
    return true;
  }
  bool IsPositiveOrZero() const {
    for (int i = 0; i < this->size; i ++) {
      if (this->theObjects[i] < 0) {
        return false;
      }
    }
    return true;
  }
  coefficient SumCoords() const {
    coefficient result = 0;
    for (int i = 0; i < this->size; i ++) {
      result += this->theObjects[i];
    }
    return result;
  }
  template <class otherType>
  otherType ScalarEuclidean(const Vector<otherType>& other) const {
    otherType output;
    this->ScalarEuclidean(other, output);
    return output;
  }
  template <class otherType>
  void ScalarEuclidean(const Vector<otherType>& other, otherType& output) const {
    if (this->size != other.size) {
      global.fatal << "This is a programming error: taking scalar product of elements of different dimensions: "
      << *this << " and " << other << ". ";
      global.fatal << global.fatal;
    }
    otherType tempElt;
    output = 0;
    for (int i = 0; i < this->size; i ++) {
      tempElt = other[i];
      tempElt *= this->theObjects[i];
      output += tempElt;
    }
  }
  static void ProjectOntoHyperPlane(
    Vector<coefficient>& input,
    Vector<coefficient>& normal,
    Vector<coefficient>& ProjectionDirection,
    Vector<coefficient>& output
  ) {
    coefficient t;
    coefficient tempRat;
    input.ScalarEuclidean(normal, t);
    ProjectionDirection.ScalarEuclidean(normal, tempRat);
    t /= tempRat;
    t.Minus();
    Vector<coefficient>::VectorPlusVectorTimesScalar(input, ProjectionDirection, t, output);
  }
  static void VectorPlusVectorTimesScalar(
    const Vector<coefficient>& r1,
    const Vector<coefficient>& r2,
    const coefficient& theCoeff,
    Vector<coefficient>& output
  ) {
    coefficient tempRat;
    if (r1.size != r2.size) {
      global.fatal << "Adding vectors of different dimensions. " << global.fatal;
    }
    output = r1;
    for (int i = 0; i < r1.size; i ++) {
      tempRat = r2[i];
      tempRat *= theCoeff;
      output.theObjects[i] += (tempRat);
    }
  }
  void Minus() {
    for (int i = 0; i < this->size; i ++) {
      this->theObjects[i] *= - 1;
    }
  }
  void MakeEi(int DesiredDimension, int NonZeroIndex) {
    this->makeZero(DesiredDimension);
    (*this)[NonZeroIndex] = 1;
  }
  inline static unsigned int hashFunction(const Vector<coefficient>& input) {
    return input.hashFunction();
  }
  unsigned int hashFunction() const {
    unsigned int result = 0;
    int theSize = MathRoutines::Minimum(this->size, SomeRandomPrimesSize);
    for (int i = 0; i < theSize; i ++) {
      result += this->theObjects[i].hashFunction() * ::SomeRandomPrimes[i];
    }
    return result;
  }
  void ScaleToFirstNonZeroCoordinatePositive();
  // Returns the number by which the vector was multiplied.
  void ScaleNormalizeFirstNonZero();
  void MakeAffineUsingLastCoordinate() {
    coefficient theElt;
    theElt = *this->LastObject();
    this->size --;
    this->operator/=(theElt);
  }
  bool AssignMatDetectRowOrColumn(const Matrix<coefficient>& input) {
    if (input.NumCols == 1) {
      this->setSize(input.NumRows);
      for (int i = 0; i < this->size; i ++) {
        this->theObjects[i] = input.elements[i][0];
      }
      return true;
    }
    if (input.NumRows == 1) {
      this->setSize(input.NumCols);
      for (int i = 0; i < this->size; i ++) {
        this->theObjects[i] = input.elements[0][i];
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
  int GetNumNonZeroCoords() const {
    int result = 0;
    for (int i = 0; i < this->size; i ++) {
      if (!this->theObjects[i].IsEqualToZero()) {
        result ++;
      }
    }
    return result;
  }
  bool IsProportionalTo(const Vector<coefficient>& other, coefficient& TimesMeEqualsOther) const;
  bool IsProportionalTo(const Vector<coefficient>& other) const {
    coefficient TimesMeEqualsOther;
    return this->IsProportionalTo(other, TimesMeEqualsOther);
  }
  int FindLCMDenominatorsTruncateToInt();
  void FindLCMDenominators(LargeIntegerUnsigned& output);
  inline Vector<coefficient> GetShiftToTheLeftOnePosition() {
    return this->GetShiftToTheLeft(1);
  }
  void MakeNormalInProjectivizationFromPointAndNormal(Vector<coefficient>& point, Vector<coefficient>& normal) {
    //the extra dimension is going to be the last dimension
    int newDimension = normal.size + 1;
    this->setSize(newDimension);
    this->RootScalarEuclideanRoot(normal, point, this->theObjects[newDimension - 1]);
    this->theObjects[newDimension - 1].Minus();
    for (int j = 0; j < newDimension - 1; j ++) {
      this->theObjects[j] = normal[j];
    }
  }
  bool ProjectToAffineSpace(Vector<coefficient> &output) {
    if (this->theObjects[this->size - 1].IsEqualToZero()) {
      return false;
    }
    output.setSize(this->size - 1);
    for (int i = 0; i < this->size - 1; i ++) {
      output[i] = this->theObjects[i];
    }
    output /= this->theObjects[this->size - 1];
    return true;
  }
  bool MakeAffineProjectionFromNormal(affineHyperplane<Rational>& output);
  int GetIndexFirstNonZeroCoordinate() const {
    for (int i = 0; i < this->size; i ++) {
      if (!this->theObjects[i].IsEqualToZero()) {
        return i;
      }
    }
    return - 1;
  }
  int getIndexLastNonZeroCoordinate() {
    for (int i = this->size - 1; i >= 0; i --) {
      if (!this->theObjects[i].IsEqualToZero()) {
        return i;
      }
    }
    return - 1;
  }
  Vector<coefficient> GetShiftToTheLeft(int numPositions) {
    Vector<coefficient> result;
    if (numPositions > this->size) {
      global.fatal << "You requested a shift of "
      << numPositions << " positions in a vector with "
      << this->size << "elements" << global.fatal;
    }
    result.setSize(this->size - numPositions);
    for (int i = 0; i < result.size; i ++) {
      result[i] = this->theObjects[i + numPositions];
    }
    return result;
  }
  inline void ShiftToTheLeftOnePos() {
    this->ShiftToTheLeft(1);
  }
  void ShiftToTheLeft(int numPositions) {
    if (numPositions > this->size) {
      global.fatal << "Bad vector shift. " << global.fatal;
    }
    for (int i = 0; i < this->size - numPositions; i ++) {
      this->theObjects[i] = this->theObjects[i + numPositions];
    }
    this->size -= numPositions;
  }
  void ShiftToTheRightInsertZeroes(int numPositions, const coefficient& theRingZero) {
    if (numPositions < 0) {
      global.fatal << "Bad vector shift, cannot fill with zeroes. " << global.fatal;
    }
    this->setSize(this->size + numPositions);
    for (int i = this->size - 1; i >= numPositions; i --) {
      this->theObjects[i] = this->theObjects[i - numPositions];
    }
    for (int i = 0; i < numPositions; i ++) {
      this->theObjects[i] = theRingZero;
    }
  }
  void SetDimInsertZeroes(int newDim) {
    int oldDim = this->size;
    this->setSize(newDim);
    for (int i = oldDim; i < newDim; i ++) {
      (*this)[i] = 0;
    }
  }
  bool GetIntegralCoordsInBasisIfTheyExist(
    const Vectors<coefficient>& inputBasis,
    Vector<coefficient>& output,
    Matrix<coefficient>& bufferMatGaussianEliminationCC,
    Matrix<coefficient>& bufferMatGaussianElimination,
    const coefficient& theRingUnit,
    const coefficient& theRingMinusUnit,
    const coefficient& theRingZero
  );
  void GetVectorsPerpendicularTo(
    Vectors<coefficient>& output
  ) {
    int Pivot = - 1;
    if (!this->FindIndexFirstNonZeroCoordinateFromTheLeft(Pivot)) {
      output.MakeEiBasis(this->size);
      return;
    }
    output.setSize(this->size - 1);
    for (int i = 0; i < this->size; i ++) {
      if (i != Pivot) {
        Vector<coefficient>& current = output.theObjects[i];
        current.MakeEi(this->size, i);
        current.theObjects[Pivot] -= this->theObjects[i];
      }
    }
  }
  bool FindIndexFirstNonZeroCoordinateFromTheLeft(int& theIndex) {
    theIndex = - 1;
    for (int i = 0; i < this->size; i ++) {
      if (!this->theObjects[i].IsEqualToZero()) {
        theIndex = i;
        return true;
      }
    }
    return false;
  }
  bool GetCoordsInBasiS(const Vectors<coefficient>& inputBasis, Vector<coefficient>& output) const;
  Vector<coefficient> GetProjectivizedNormal(Vector<coefficient>& affinePoint);
  Vector<coefficient> operator*(const coefficient& other) const {
    Vector<coefficient> result;
    result.setSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      result[i] = this->theObjects[i];
      result[i] *= other;
    }
    return result;
  }
  Vector<coefficient> operator/(const coefficient& other) const {
    if (other.IsEqualToZero()) {
      global.fatal << "This is a programming error: division by zero. "
      << "Division by zero error are supposed to be handled at an earlier level. " << global.fatal;
    }
    Vector<coefficient> result;
    result.setSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      result[i] = (*this)[i];
      result[i] /= other;
    }
    return result;
  }
  void operator*=(const coefficient& other) {
    for (int i = 0; i < this->size; i ++) {
      this->theObjects[i] *= other;
    }
  }
  void operator/=(const coefficient& other) {
    for (int i = 0; i < this->size; i ++) {
      this->theObjects[i] /= other;
    }
  }
  template <class otherType>
  void operator+=(const Vector<otherType>& other) {
    for (int i = 0; i < this->size; i ++) {
      (*this)[i] += other[i];
    }
  }
  inline bool operator<(const Vector<coefficient>& other) const {
    return other > *this;
  }
  bool IsGreaterThanLexicographic(const Vector<coefficient>& other) const {
    return this->::List<coefficient>::operator>(other);
  }
  bool operator>(const Vector<coefficient>& other) const {
    if (this->size != other.size) {
      global.fatal << "This is a programming error: comparing Vectors with different number of coordinates, namely, "
      << this->toString() << " and " << other.toString() << ". " << global.fatal;
    }
    coefficient c1 = 0, c2 = 0;
    for (int i = 0; i < this->size; i ++) {
      c1 += this->theObjects[i];
      c2 += other.theObjects[i];
    }
    if (c1 > c2) {
      return true;
    }
    if (c2 > c1) {
      return false;
    }
    for (int i = this->size - 1; i >= 0; i --) {
      if (this->theObjects[i] > other.theObjects[i]) {
        return true;
      }
      if (other.theObjects[i] > this->theObjects[i]) {
        return false;
      }
    }
    return false;
  }
  template <class otherType>
  void operator-=(const Vector<otherType>& other) {
    if (this->size != other.size) {
      global.fatal << "This is a programming error: subtracting vectors with different dimensions. " << global.fatal;
    }
    for (int i = 0; i < this->size; i ++) {
      this->theObjects[i] -= other[i];
    }
  }
  Vector<coefficient> operator+(const Vector<coefficient>& right) const {
    Vector<coefficient> result = *this;
    result += right;
    return result;
  }
  bool AssignString(const std::string& input);

  void operator=(const std::string& input) {
    this->AssignString(input);
  }
  // The following function is required else
  // some compilers generate warning:
  // 'implicitly declared Vector<Rational>& Vector<Rational>::operator=(...) is deprecated'
  Vector<coefficient>& operator=(const Vector<coefficient>& other) {
    if (this == &other) {
      return *this;
    }
    this->setSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->theObjects[i] = other[i];
    }
    return *this;
  }
  template <class otherType>
  Vector<coefficient>& operator=(const Vector<otherType>& other) {
    if (this == reinterpret_cast<const Vector<coefficient>*>(&other)) {
      return *this;
    }
    this->setSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->theObjects[i] = other[i];
    }
    return *this;
  }
  template <class otherType>
  Vector<coefficient>& operator=(const List<otherType>& other) {
    this->setSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->theObjects[i] = other[i];
    }
    return *this;
  }
  void operator=(const Selection& other);
  void operator=(const SelectionWithMultiplicities& other);
  bool operator!=(const Vector<coefficient>& other) const {
    return !(*this == other);
  }
  bool IsEqualToZero() const {
    for (int i = 0; i < this->size; i ++) {
      if (!this->theObjects[i].IsEqualToZero()) {
        return false;
      }
    }
    return true;
  }
  Vector<coefficient> operator-(const Vector<coefficient>& other) const {
    Vector<coefficient> result;
    result = *this;
    result -= other;
    return result;
  }
  void ReadFromFile(std::fstream& input) {
    std::string tempS;
    input >> tempS;
    if (tempS != "root_dim:") {
      global.fatal << "Failed to read vector from file. " << global.fatal;
    }
    int tempI;
    input >> tempI;
    this->setSize(tempI);
    for (int i = 0; i < this->size; i ++) {
      this->theObjects[i].ReadFromFile(input);
    }
  }
  void WriteToFile(std::fstream& output) {
    output << "root_dim: " << this->size << " ";
    for (int i = 0; i < this->size; i ++) {
      this->theObjects[i].WriteToFile(output);
      output << " ";
    }
  }
};

template <class coefficient>
void Vector<coefficient>::ScaleNormalizeFirstNonZero() {
  int index = this->GetIndexFirstNonZeroCoordinate();
  if (index < 0) {
    return;
  }
  coefficient::scaleNormalizeIndex(*this, index);
}

template <class coefficient>
void Vector<coefficient>::ScaleToFirstNonZeroCoordinatePositive() {
  for (int i = 0; i < this->size; i ++) {
    if ((*this)[i].IsPositive()) {
      return;
    }
    if ((*this)[i].IsNegative()) {
      this->Minus();
      return;
    }
  }
}

template <class coefficient>
bool Vector<coefficient>::IsProportionalTo(
  const Vector<coefficient>& input,
  coefficient& outputTimesMeEqualsInput
) const {
  if (this->size != input.size) {
    return false;
  }
  int IndexFirstNonZero = - 1;
  for (int i = 0; i < this->size; i ++) {
    if (!this->theObjects[i].IsEqualToZero()) {
      IndexFirstNonZero = i;
      break;
    }
  }
  if (IndexFirstNonZero == - 1) {
    if (input.IsEqualToZero()) {
      outputTimesMeEqualsInput.makeZero();
      return true;
    }
    return false;
  }
  Vector<Rational> tempRoot = *this;
  outputTimesMeEqualsInput = input[IndexFirstNonZero];
  outputTimesMeEqualsInput /= (*this)[IndexFirstNonZero];
  tempRoot *= outputTimesMeEqualsInput;
  return tempRoot == input;
}

template <class coefficient>
void Vector<coefficient>::FindLCMDenominators(LargeIntegerUnsigned& output) {
  LargeIntegerUnsigned tempI, tempI2;
  output.makeOne();
  for (int i = 0; i < this->size; i ++) {
    this->theObjects[i].GetDenominator(tempI2);
    LargeIntegerUnsigned::gcd(output, tempI2, tempI);
    output.MultiplyBy(tempI2);
    output.DivPositive(tempI, output, tempI2);
  }
}

template <class coefficient>
int Vector<coefficient>::FindLCMDenominatorsTruncateToInt() {
  int result = 1;
  for (int i = 0; i < this->size; i ++) {
    result = MathRoutines::lcm(result, this->theObjects[i].DenShort);
    if ((*this)[i].Extended != 0) {
      global.fatal << "Coefficient is large rational at a place where that is not allowed. " << global.fatal;
    }
  }
  return result;
}

template <class coefficient>
class Vectors: public List<Vector<coefficient> > {
  public:
  std::string ElementToStringEpsilonForm(bool useLatex, bool useHtml, bool makeTable) {
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
      tempS = this->theObjects[i].ToStringEpsilonFormat();
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
  std::string ElementsToInequalitiesString(
    bool useLatex, bool useHtml, bool LastVarIsConstant, FormatExpressions& theFormat
  ) const;
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  bool LinearAlgebraForVertexComputation(
    Selection& theSelection, Vector<coefficient>& output, Matrix<coefficient>& buffer, Selection& NonPivotPointsBuffer
  );
  void GetVectorsDouble(Vectors<double>& output) const {
    output.setSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      output[i] = this->theObjects[i].GetVectorDouble();
    }
  }
  void AssignListListCoefficientType(const List<List<coefficient> >& input) {
    this->setSize(input.size);
    for (int i = 0; i < input.size; i ++) {
      this->theObjects[i] = input[i];
    }
  }
  void AssignListList(const List<Vectors<coefficient> >& input) {
    int count = 0;
    for (int i = 0; i < input.size; i ++) {
      count += input[i].size;
    }
    this->setSize(0);
    this->Reserve(count);
    for (int i = 0; i < input.size; i ++) {
      for (int j = 0; j < input[i].size; j ++) {
        this->addOnTop(input[i][j]);
      }
    }
  }
  std::string ElementToLinearCombinationString() {
    std::stringstream out;
    std::string tempS;
    for (int i = 0; i < this->size; i ++) {
      this->theObjects[i].toString(tempS);
      out << tempS;
      if (i != this->size - 1) {
        out << " + ";
      }
    }
    return out.str();
  }
  int ArrangeFirstVectorsBeOfMaxPossibleRank(Matrix<coefficient>& bufferMat, Selection& bufferSel);
  static unsigned int hashFunction(const Vectors<coefficient>& input) {
    return input.hashFunction();
  }
  unsigned int hashFunction() const {
    return this->::List<Vector<coefficient> >::hashFunction();
  }
  bool HasAnElementWithPositiveScalarProduct(const Vector<coefficient>& input) const {
    for (int i = 0; i < this->size; i ++) {
      if (input.ScalarEuclidean(this->theObjects[i]).IsPositive()) {
        return true;
      }
    }
    return false;
  }
  bool HasAnElementWithNegativeScalarProduct(const Vector<coefficient>& input) const {
    for (int i = 0; i < this->size; i ++) {
      if (input.ScalarEuclidean(this->theObjects[i]).IsNegative()) {
        return true;
      }
    }
    return false;
  }
  bool HasAnElementPerpendicularTo(const Vector<coefficient>& input) const {
    for (int i = 0; i < this->size; i ++) {
      if (input.ScalarEuclidean(this->theObjects[i]).IsEqualToZero()) {
        return true;
      }
    }
    return false;
  }
  void SelectionToMatrix(Selection& theSelection, int OutputDimension, Matrix<coefficient>& output);
  void SelectionToMatrixAppend(Selection& theSelection, int OutputDimension, Matrix<coefficient>& output, int StartRowIndex);
  void SelectionToMatrix(Selection& theSelection, int OutputDimension, Matrix<coefficient>& output, int StartRowIndex);
  void GetGramMatrix(Matrix<coefficient>& output, const Matrix<Rational>* theBilinearForm = nullptr) const;
  void GetMatrixRootsToRows(Matrix<Rational>& output) const;
  void GetOrthogonalComplement(Vectors<coefficient>& output, Matrix<Rational>* theBilinearForm = nullptr);
  bool LinSpanContainsVector(
    const Vector<coefficient>& input, Matrix<coefficient>& bufferMatrix, Selection& bufferSelection
  ) const;
  void MakeEiBasis(int theDimension) {
    this->setSize(theDimension);
    for (int i = 0; i < this->size; i ++) {
      this->theObjects[i].MakeEi(theDimension, i);
    }
  }
  bool LinSpanContainsVector(const Vector<coefficient>& input) const;
  static void SelectABasisInSubspace(
    const List<Vector<coefficient> >& input, List<Vector<coefficient> >& output, Selection& outputSelectedPivots
  );
  int GetRankOfSpanOfElements(Matrix<coefficient>* buffer = 0, Selection* bufferSelection = nullptr) const;
  static bool ConesIntersect(
    List<Vector<Rational> >& StrictCone,
    List<Vector<Rational> >& NonStrictCone,
    Vector<Rational>* outputLinearCombo = nullptr,
    Vector<Rational>* outputSplittingNormal = nullptr
  );
  static bool GetNormalSeparatingCones(
    List<Vector<coefficient> >& coneStrictlyPositiveCoeffs,
    List<Vector<coefficient> >& coneNonNegativeCoeffs,
    Vector<coefficient>& outputNormal
  );
  void average(Vector<coefficient>& output, int theDimension) {
    this->sum(output, theDimension);
    if (this->size == 0) {
      return;
    }
    output /= this->size;
  }
  void sum(Vector<coefficient>& output, int resultDim) const {
    output.makeZero(resultDim);
    for (int i = 0; i < this->size; i ++) {
      output += this->theObjects[i];
    }
  }
  bool GetCoordsInBasis(
    const Vectors<coefficient>& inputBasis, Vectors<coefficient>& outputCoords
  ) const;
  bool GetIntegralCoordsInBasisIfTheyExist(
    const Vectors<coefficient>& inputBasis,
    Vectors<coefficient>& output,
    const coefficient& theRingUnit,
    const coefficient& theRingMinusUnit,
    const coefficient& theRingZero
  ) const {
    Matrix<coefficient> bufferMatGaussianEliminationCC, bufferMatGaussianElimination;
    bool result = true;
    output.setSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      if (!this->theObjects[i].GetIntegralCoordsInBasisIfTheyExist(
        inputBasis,
        output.theObjects[i],
        bufferMatGaussianEliminationCC,
        bufferMatGaussianElimination,
        theRingUnit,
        theRingMinusUnit,
        theRingZero
      )) {
        result = false;
      }
    }
    return result;
  }
  bool GetIntegralCoordsInBasisIfTheyExist(
    const Vectors<coefficient>& inputBasis,
    Vectors<coefficient>& output,
    Matrix<coefficient>& bufferMatGaussianEliminationCC,
    Matrix<coefficient>& bufferMatGaussianElimination,
    const coefficient& theRingUnit,
    const coefficient& theRingMinusUnit,
    const coefficient& theRingZero
  ) const {
    bool result = true;
    output.setSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      if (!this->theObjects[i].GetIntegralCoordsInBasisIfTheyExist(
        inputBasis,
        output.theObjects[i],
        bufferMatGaussianEliminationCC,
        bufferMatGaussianElimination,
        theRingUnit,
        theRingMinusUnit,
        theRingZero
      )) {
        result = false;
      }
    }
    return result;
  }
  std::string ElementToStringLetterFormat(const std::string& inputLetter, bool useLatex) {
    std::stringstream out;
    for (int i = 0; i < this->size; i ++) {
      out << this->theObjects[i].ElementToStringLetterFormat(inputLetter, useLatex);
      if (i != this->size - 1) {
        out << ",";
      }
    }
    return out.str();
  }
  bool ComputeNormalExcludingIndex(Vector<coefficient>& output, int index, Matrix<coefficient>& bufferMatrix);
  bool ComputeNormalFromSelection(
    Vector<coefficient>& output,
    Selection& theSelection,
    Matrix<coefficient>& bufferMatrix,
    int theDimension
  ) const;
  bool ComputeNormalFromSelectionAndExtraRoot(
    Vector<coefficient>& output,
    Vector<coefficient>& ExtraRoot,
    Selection& theSelection,
    Matrix<coefficient>& bufferMatrix,
    Selection& bufferSel
  );
  bool ComputeNormalFromSelectionAndTwoExtraRoots(
    Vector<coefficient>& output,
    Vector<coefficient>& ExtraRoot1,
    Vector<coefficient>& ExtraRoot2,
    Selection& theSelection,
    Matrix<coefficient>& bufferMat,
    Selection& bufferSel
  );
  bool ComputeNormal(Vector<coefficient>& output, int inputDim);
  void GaussianEliminationForNormalComputation(
    Matrix<coefficient>& inputMatrix, Selection& outputNonPivotPoints, int theDimension
  ) const;
  //the below function returns a n row 1 column matrix with the coefficients in the obvious order
  bool GetLinearDependence(Matrix<coefficient>& outputTheLinearCombination);
  void GetLinearDependenceRunTheLinearAlgebra(Matrix<coefficient>& outputTheSystem, Selection& outputNonPivotPoints);
  bool ContainsARootNonPerpendicularTo(const Vector<coefficient>& input, const Matrix<coefficient>& theBilinearForm);
  bool ContainsOppositeRoots() {
    if (this->size < 10) {
      Vector<Rational> tempRoot;
      for (int i = 0; i < this->size; i ++) {
        tempRoot = this->theObjects[i];
        tempRoot.Minus();
        for (int j = i + 1; j < this->size; j ++) {
          if (this->theObjects[j].IsEqualTo(tempRoot)) {
            return true;
          }
        }
      }
      return false;
    }
    HashedList<Vector<coefficient> > tempList;
    tempList.setExpectedSize(this->size);
    tempList = *this;
    for (int i = 0; i < this->size; i ++) {
      if (tempList.Contains(- this->theObjects[i])) {
        return true;
      }
    }
    return false;
  }
  void AssignMatrixColumns(Matrix<coefficient>& mat) {
    Vector<coefficient> tempRoot;
    this->setSize(mat.NumCols);
    tempRoot.setSize(mat.NumRows);
    for (int i = 0; i < mat.NumCols; i ++) {
      for (int j = 0; j < mat.NumRows; j ++) {
        tempRoot[j] = mat.elements[j][i];
      }
      this->theObjects[i] = tempRoot;
    }
  }
  void AssignMatrixRows(const Matrix<coefficient>& mat) {
    this->size = 0;
    this->setSize(mat.NumRows);
    for (int i = 0; i < mat.NumRows; i ++) {
      this->theObjects[i].setSize(mat.NumCols);
      for (int j = 0; j < mat.NumCols; j ++) {
        this->theObjects[i].theObjects[j] = mat.elements[i][j];
      }
    }
  }
  int GetDim() const {
    if (this->size > 0) {
      return this->theObjects[0].size;
    }
    return - 1;
  }
  void BeefUpWithEiToLinearlyIndependentBasis(int theDim);
  void ChooseABasis();
  static void IntersectTwoLinSpaces(
    const List<Vector<coefficient> >& firstSpace,
    const List<Vector<coefficient> >& secondSpace,
    List<Vector<coefficient> >& output
  );
  void operator=(const List<List<coefficient> >& other) {
    this->setSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->theObjects[i] = other.theObjects[i];
    }
  }
  template <class otherType>
  void operator=(const List<Vector<otherType> >& other) {
    this->setSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->theObjects[i] = other[i];
    }
  }
};

template <class coefficient>
bool Vector<coefficient>::AssignString(const std::string& input) {
  MacroRegisterFunctionWithName("Vector::AssignString");
  this->setSize(0);
  int currentDigit = - 1;
  int sign = 1;
  for (unsigned i = 0; i < input.size(); i ++) {
    char previous = (i == 0) ? '(' : input[i - 1];
    if (MathRoutines::isADigit(input[i], &currentDigit)) {
      if (!MathRoutines::isADigit(previous)) {
        this->addOnTop(coefficient(0));
      }
      *this->LastObject() *= 10;
      *this->LastObject() += currentDigit * sign;
    } else if (input[i] == '-') {
      sign = - 1;
    } else if (input[i] == ',') {
      sign = 1;
    }
  }
  return true;
}

template <class coefficient>
bool Vector<coefficient>::GetCoordsInBasiS(const Vectors<coefficient>& inputBasis, Vector<coefficient>& output) const {
  if (inputBasis.size == 0) {
    return false;
  }
  MacroRegisterFunctionWithName("Vector::GetCoordsInBasiS");
  Vectors<coefficient> bufferVectors;
  Matrix<coefficient> bufferMat;
  if (this->size != inputBasis[0].size) {
    global.fatal << "This is a programming error: asking to get coordinates of vector of "
    << this->size << " coordinates using a basis whose first vector has "
    << inputBasis[0].size << " coordinates." << global.fatal;
  }
  bufferVectors.Reserve(inputBasis.size + 1);
  bufferVectors.addListOnTop(inputBasis);
  bufferVectors.addOnTop(*this);
  if (!bufferVectors.GetLinearDependence(bufferMat)) {
    return false;
  }
  coefficient tempCF = bufferMat(bufferMat.NumRows - 1, 0);
  bufferMat /= tempCF;
  output.setSize(bufferMat.NumRows - 1);
  for (int i = 0; i < bufferMat.NumRows - 1; i ++) {
    bufferMat(i, 0).Minus();
    output[i] = bufferMat(i, 0);
  }
  return true;
}

template <class coefficient>
void Vectors<coefficient>::GetLinearDependenceRunTheLinearAlgebra(
  Matrix<coefficient>& outputTheSystem, Selection& outputNonPivotPoints
) {
  MacroRegisterFunctionWithName("Vectors::GetLinearDependenceRunTheLinearAlgebra");
  if (this->size == 0) {
    return;
  }
  int Dimension = (*this)[0].size;
  outputTheSystem.init(Dimension, this->size);
  for (int i = 0; i < this->size; i ++) {
    for (int j = 0; j < Dimension; j ++) {
      outputTheSystem(j, i) = (*this)[i][j];
    }
  }
  outputTheSystem.GaussianEliminationByRows(0, &outputNonPivotPoints);
}

template <class coefficient>
bool Vectors<coefficient>::LinSpanContainsVector(
  const Vector<coefficient>& input, Matrix<coefficient>& bufferMatrix, Selection& bufferSelection
) const {
  Vectors<coefficient> tempVectors;
  tempVectors = (*this);
  tempVectors.addOnTop(input);
  return this->GetRankOfSpanOfElements(&bufferMatrix, &bufferSelection) ==
  tempVectors.GetRankOfSpanOfElements(&bufferMatrix, &bufferSelection);
}

template <class coefficient>
bool Vectors<coefficient>::GetCoordsInBasis(
  const Vectors<coefficient>& inputBasis, Vectors<coefficient>& outputCoords
) const {
  MacroRegisterFunctionWithName("Vectors::GetCoordsInBasis");
  //if (this == 0 || &outputCoords == 0 || this == &outputCoords)
  //  global.fatal << "This is a programming error: input and output addresses are zero or coincide. this address: "
  //  << (unsigned long) this << "; output address: " << (unsigned long)(&outputCoords) << global.fatal;
  outputCoords.setSize(this->size);
  for (int i = 0; i < this->size; i ++) {
    if (!(this->operator[](i).GetCoordsInBasiS(inputBasis, outputCoords[i]))) {
      return false;
    }
  }
  return true;
}

template <class coefficient>
bool Vector<coefficient>::GetIntegralCoordsInBasisIfTheyExist(
  const Vectors<coefficient>& inputBasis,
  Vector<coefficient>& output,
  Matrix<coefficient>& bufferMatGaussianEliminationCC,
  Matrix<coefficient>& bufferMatGaussianElimination,
  const coefficient& theRingUnit,
  const coefficient& theRingMinusUnit,
  const coefficient& theRingZero
) {
  int theDim = this->size;
  bufferMatGaussianElimination.init(inputBasis.size, theDim);
  for (int i = 0; i < inputBasis.size; i ++) {
    for (int j = 0; j < theDim; j ++) {
      bufferMatGaussianElimination.elements[i][j] = inputBasis[i][j];
    }
  }
  bufferMatGaussianEliminationCC.MakeIdMatrix(bufferMatGaussianElimination.NumRows, theRingUnit, theRingZero);
  bufferMatGaussianElimination.GaussianEliminationEuclideanDomain(
    &bufferMatGaussianEliminationCC, theRingMinusUnit, theRingUnit
  );
  Vector<coefficient> tempRoot, theCombination;
  if (this == &output) {
    global.fatal << "Output not allowed to coincide with this object" << global.fatal;
  }
  output.makeZero(inputBasis.size);
  theCombination = *this;
  int col = 0;
  for (int i = 0; i < inputBasis.size; i ++) {
    for (; col < theDim; col ++) {
      if (!bufferMatGaussianElimination.elements[i][col].IsEqualToZero()) {
        break;
      }
    }
    if (col >= theDim) {
       break;
    }
    bufferMatGaussianElimination.GetVectorFromRow(i, tempRoot);
    output[i] = this->theObjects[col];
    output[i] /= bufferMatGaussianElimination.elements[i][col];
    tempRoot *= output[i];
    theCombination -= tempRoot;
  }
  if (!theCombination.IsEqualToZero()) {
    return false;
  }
  bufferMatGaussianEliminationCC.ActMultiplyVectorRowOnTheRight(output);
  return true;
}

template<class coefficient>
void Vectors<coefficient>::GetGramMatrix(Matrix<coefficient>& output, const Matrix<Rational>* theBilinearForm) const {
  output.Resize(this->size, this->size, false);
  for (int i = 0; i < this->size; i ++) {
    for (int j = i; j < this->size; j ++) {
      if (theBilinearForm != nullptr) {
        Vector<coefficient>::ScalarProduct(this->theObjects[i], this->theObjects[j], *theBilinearForm, output.elements[i][j]);
      } else {
        output(i, j) = (*this)[i].ScalarEuclidean((*this)[j]);
      }
      if (i != j) {
        output(j, i) = output(i, j);
      }
    }
  }
}

template<class coefficient>
bool Vectors<coefficient>::ContainsARootNonPerpendicularTo(
  const Vector<coefficient>& input, const Matrix<coefficient>& theBilinearForm
) {
  for (int i = 0; i < this->size; i ++) {
    if (!Vector<coefficient>::ScalarProduct(this->theObjects[i], input, theBilinearForm).IsEqualToZero()) {
      return true;
    }
  }
  return false;
}

template<class coefficient>
int Vectors<coefficient>::ArrangeFirstVectorsBeOfMaxPossibleRank(Matrix<coefficient>& bufferMat, Selection& bufferSel) {
  if (this->size == 0) {
    return 0;
  }
  int theDimension = this->GetDimensionOfElements();
  Vectors<Rational> tempRoots;
  int oldRank = 0;
  for (int i = 0; i < this->size; i ++) {
    tempRoots.addOnTop(this->theObjects[i]);
    int newRank = tempRoots.GetRankOfSpanOfElements(bufferMat, bufferSel);
    if (newRank == oldRank) {
      tempRoots.RemoveIndexSwapWithLast(tempRoots.size - 1);
    } else {
      this->SwapTwoIndices(oldRank, i);
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

template <class coefficient>
class affineHyperplane {
public:
  Vector<coefficient> affinePoint;
  Vector<coefficient> normal;
  void toString(std::string& output);
  //void InduceFromFacet(Facet& input);
  //the below returns false if the projection is not of full dimension
  unsigned int hashFunction() const;
  static inline unsigned int hashFunction(const affineHyperplane& input) {
    return input.hashFunction();
  }
  //  bool ProjectFromFacet(Facet& input);
  bool ProjectFromFacetNormal(Vector<coefficient>& input);
  Vector<coefficient> ProjectOnMe(Vector<coefficient>& input) const;
  bool ContainsPoint(Vector<coefficient> & thePoint);
  void MakeFromNormalAndPoint(Vector<coefficient>& inputPoint, Vector<coefficient>& inputNormal);
  bool HasACommonPointWithPositiveTwoToTheNth_ant();
  bool operator==(const affineHyperplane& right);
};

template <class coefficient>
bool affineHyperplane<coefficient>::operator==(const affineHyperplane& right) {
  Vector<Rational> tempRoot1, tempRoot2;
  tempRoot1 = this->normal;
  tempRoot2 = right.normal;
  tempRoot1.ScaleNormalizeFirstNonZero();
  tempRoot2.ScaleNormalizeFirstNonZero();
  if (!(tempRoot1 == tempRoot2)) {
    return false;
  }
  Rational tempRat1, tempRat2;
  tempRoot1.ScalarEuclidean(this->affinePoint, tempRat1);
  tempRoot1.ScalarEuclidean(right.affinePoint, tempRat2);
  return tempRat1.IsEqualTo(tempRat2);
}

template <class coefficient>
Vector<coefficient> affineHyperplane<coefficient>::ProjectOnMe(Vector<coefficient>& input) const {
  //output = input +x*normal  and <input +x*normal, normal>= 0 =>
  //x = -<input, normal>/<normal,normal>
  coefficient theNormalCoeff = - input.ScalarEuclidean(this->normal) / this->normal.ScalarEuclidean(this->normal);
  Vector<coefficient> output;
  output = input + this->normal * theNormalCoeff;
  return output;
}

template <class coefficient>
bool affineHyperplane<coefficient>::ProjectFromFacetNormal(Vector<coefficient>& input) {
  int tempI = input.GetIndexFirstNonZeroCoordinate();
  if (tempI == - 1) {
    global.fatal << "No non-zero coordinate found. " << global.fatal;
  }
  if (tempI == input.size - 1) {
    return false;
  }
  this->affinePoint.makeZero(input.size);
  this->affinePoint.setSize(input.size - 1);
  this->affinePoint[tempI] = input[input.size - 1];
  this->affinePoint[tempI].Minus();
  this->affinePoint[tempI].DivideBy(input[tempI]);
  this->normal = input;
  this->normal.setSize(input.size - 1);
  return true;
}

template <class coefficient>
bool affineHyperplane<coefficient>::ContainsPoint(Vector<coefficient>& thePoint) {
  Rational tempRat1, tempRat2;
  tempRat1 = this->normal.ScalarEuclidean(thePoint);
  tempRat2 = this->normal.ScalarEuclidean(this->affinePoint);
  return tempRat2.IsEqualTo(tempRat1);
}

template <class coefficient>
bool affineHyperplane<coefficient>::HasACommonPointWithPositiveTwoToTheNth_ant() {
  Rational tempRat;
  tempRat = this->normal.ScalarEuclidean(this->affinePoint);
  if (tempRat.IsEqualToZero()) {
    return true;
  }
  for (int i = 0; i < this->normal.size; i ++) {
    Rational& tempRat2 = this->normal[i];
    if (tempRat.IsNegative() && tempRat2.IsNegative()) {
      return true;
    }
    if (tempRat.IsPositive() && tempRat2.IsPositive()) {
      return true;
    }
  }
  return false;
}

template <class coefficient>
void affineHyperplane<coefficient>::MakeFromNormalAndPoint(Vector<coefficient>& inputPoint, Vector<coefficient>& inputNormal) {
  this->affinePoint = inputPoint;
  this->normal = inputNormal;
}

template <class coefficient>
void affineHyperplane<coefficient>::toString(std::string& output) {
  std::stringstream out;
  out << "point: " << this->affinePoint << ", normal: " << this->normal;
  output = out.str();
}

template <class coefficient>
unsigned int affineHyperplane<coefficient>::hashFunction() const {
  // warning: if normal gets streched, the hashfunction should not change!
  Vector<Rational> tempNormal;
  tempNormal = this->normal;
  tempNormal.ScaleNormalizeFirstNonZero();
  Rational tempRat = this->normal.ScalarEuclidean(this->affinePoint);
  return this->normal.hashFunction() + tempRat.hashFunction();
}

class affineHyperplanes: public List<affineHyperplane<Rational> > {
public:
  std::string DebugString;
  void toString(std::string& output);
  void ComputeDebugString() {
    this->toString(this->DebugString);
  }
};

class affineCone {
public:
  affineHyperplanes theWalls;
  unsigned int hashFunction() const;
  inline static unsigned int hashFunction(const affineCone& input) {
    return input.hashFunction();
  }
  int GetDimension();
  void SuperimposeAffineCones(affineCones& theOtherComplex);
  //void induceFromCombinatorialChamber(CombinatorialChamber& input);
  bool WallIsInternalInCone(affineHyperplane<Rational>& theKillerCandidate);
  //The below function returns true if the system of homogeneous linear inequalities Ax<=b
  //has a solution, false otherwise, where A is a matrix and x and b are column vectors.
  //  bool SystemLinearInequalitiesHasSolution
  //    (Matrix<Rational> & matA, Matrix<Rational> & matb, Matrix<Rational> & outputPoint);
  bool SplitByAffineHyperplane(affineHyperplane<Rational>& theKillerPlane, affineCones& output);
};

class affineCones: public HashedList<affineCone> {
public:
  void SuperimposeAffineCones(affineCones& theOtherComplex);
};


#endif
