//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderMathVectors_h_already_included
#define vpfHeaderMathVectors_h_already_included

#include "vpfHeader2Math01_LargeIntArithmetic.h"
static ProjectInformationInstance vpfHeader2Math_02Vectors(__FILE__, "Header, vectors and related machinery. ");
class SelectionWithMultiplicities;
class Selection;

template <class coefficient>
class Vector: public List<coefficient> {
  friend Vector<coefficient> operator-(const Vector<coefficient>& input) {
    Vector<coefficient> result;
    result.SetSize(input.size);
    for (int i = 0; i < input.size; i ++)
      result[i] = - input[i];
    return result;
  }
  friend std::ostream& operator<< <coefficient>(std::ostream& output, const Vector<coefficient>& theVector);
public:
  Vector(){}
  Vector(const Vector<coefficient>& other):List<coefficient>(other) {
    *this = other;
  }
  template <class otherCoeff>
  Vector(const Vector<otherCoeff>& other) {
    *this = other;
  }
  Vector(const Selection& other) {
    *this = other;
  }
  inline static std::string GetXMLClassName() {
    std::string result = "Vector_" + coefficient::GetXMLClassName();
    return result;
  }
  std::string ToString(FormatExpressions* theFormat = 0) const {
    std::stringstream out;
    out.precision(5);
    out << "(";
    for (int i = 0; i < this->size; i ++) {
      out << (*this)[i].ToString(theFormat);
      if (i != this->size - 1) {
        out << ", ";
      }
    }
    out << ")";
    return out.str();
  }
  void ToListStringsBasicType(List<std::string>& output) const {
    output.SetSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      std::stringstream out;
      out.precision(5);
      out << (*this)[i];
      output[i] = out.str();
    }
  }
  std::string ToStringSquareBrackets(FormatExpressions* theFormat = 0) const {
    std::stringstream out;
    out.precision(5);
    out << "[";
    for (int i = 0; i < this->size; i ++) {
      out << (*this)[i].ToString(theFormat);
      if (i != this->size - 1) {
        out << ", ";
      }
    }
    out << "]";
    return out.str();
  }
  std::string ToStringSquareBracketsBasicType(FormatExpressions* theFormat = 0) const {
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
    const std::string& inputLetter, FormatExpressions* theFormat = 0, bool DontIncludeLastVar = false
  ) const;
  std::string ToStringEpsilonFormat(FormatExpressions* theFormat = 0) const {
    return this->ToStringLetterFormat("\\varepsilon", theFormat);
  }
  template <class otherType>
  static void ScalarProduct(const Vector<coefficient>& r1, const Vector<coefficient>& r2, const Matrix<otherType>& TheBilinearForm, coefficient& result) {
    if (r1.size != TheBilinearForm.NumRows || r1.size != r2.size || r1.size != TheBilinearForm.NumCols) {
      crash << "This is a programming error: attempting to take "
      << "a bilinear form represented by matrix with " << TheBilinearForm.NumRows
      << " rows and " << TheBilinearForm.NumCols << " columns "
      << " of vectors of dimension " << r1.size << " and " << r2.size << ". "
      << crash;
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
  static coefficient ScalarProduct(const Vector<coefficient>& r1, const Vector<coefficient>& r2, const Matrix<otherType>& TheBilinearForm) {
    coefficient result;
    Vector<coefficient>::ScalarProduct(r1, r2, TheBilinearForm, result);
    return result;
  }
  void PerturbNoZeroScalarProductWithMe(const List<Vector<coefficient> >& inputVectors);
  void PerturbNormalRelativeToVectorsInGeneralPosition(const Vectors<Rational>& NonStrictConeNonPositiveScalar, const List<Vector<Rational> >& VectorsToBeInGeneralPosition);
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
    result.SetSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      result[i] = (*this)[i].RationalValue();
    }
    return result;
  }
  Vector<double> GetVectorDouble() const {
    Vector<double> result;
    result.SetSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      result[i] = this->TheObjects[i].GetDoubleValue();
    }
    return result;
  }
  bool IsIntegral() {
    for (int i = 0; i < this->size; i ++) {
      if (!this->TheObjects[i].IsInteger()) {
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
      if (this->TheObjects[i] > 0) {
        return false;
      }
    }
    return true;
  }
  bool IsPositiveOrZero() const {
    for (int i = 0; i < this->size; i ++) {
      if (this->TheObjects[i] < 0) {
        return false;
      }
    }
    return true;
  }
  coefficient SumCoords() const {
    coefficient result = 0;
    for (int i = 0; i < this->size; i ++)
      result += this->TheObjects[i];
    return result;
  }
  template <class otherType>
  inline otherType ScalarEuclidean(const Vector<otherType>& other) const {
    otherType output;
    this->ScalarEuclidean(other, output);
    return output;
  }
  template <class otherType>
  void ScalarEuclidean(const Vector<otherType>& other, otherType& output) const {
    if (this->size != other.size) {
      crash.theCrashReport << "This is a programming error: taking scalar product of elements of different dimensions: "
      << *this << " and " << other << ". ";
      crash << crash;
    }
    otherType tempElt;
    output = 0;
    for (int i = 0; i < this->size; i ++) {
      tempElt = other[i];
      tempElt *= this->TheObjects[i];
      output += tempElt;
    }
  }
  static void ProjectOntoHyperPlane(Vector<coefficient>& input, Vector<coefficient>& normal, Vector<coefficient>& ProjectionDirection, Vector<coefficient>& output) {
    coefficient t;
    coefficient tempRat;
    Vector<coefficient>::ScalarEuclidean(input, normal, t);
    Vector<coefficient>::ScalarEuclidean(ProjectionDirection, normal, tempRat);
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
    if (r1.size != r2.size)
      crash << "Adding vectors of different dimensions. " << crash;
    output = r1;
    for (int i = 0; i < r1.size; i ++) {
      tempRat = r2[i];
      tempRat *= theCoeff;
      output.TheObjects[i] += (tempRat);
    }
  }
  void Minus() {
    for (int i = 0; i < this->size; i ++) {
      this->TheObjects[i] *= - 1;
    }
  }
  void MakeEi(int DesiredDimension, int NonZeroIndex) {
    this->MakeZero(DesiredDimension);
    (*this)[NonZeroIndex] = 1;
  }
  inline static unsigned int HashFunction(const Vector<coefficient>& input) {
    return input.HashFunction();
  }
  unsigned int HashFunction() const {
    unsigned int result = 0;
    int theSize = MathRoutines::Minimum(this->size, SomeRandomPrimesSize);
    for (int i = 0; i < theSize; i ++) {
      result += this->TheObjects[i].HashFunction() * ::SomeRandomPrimes[i];
    }
    return result;
  }
  void ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive() {
    this->ScaleByPositiveRationalToIntegralMinHeight();
    this->ScaleToFirstNonZeroCoordinatePositive();
  }
  void ScaleToFirstNonZeroCoordinatePositive();
  void MakeAffineUsingLastCoordinate() {
    coefficient theElt;
    theElt = *this->LastObject();
    this->size --;
    this->operator/=(theElt);
  }
  void ScaleByPositiveRationalToIntegralMinHeight();
  bool AssignMatDetectRowOrColumn(const Matrix<coefficient>& input) {
    if (input.NumCols == 1) {
      this->SetSize(input.NumRows);
      for (int i = 0; i < this->size; i ++)
        this->TheObjects[i] = input.elements[i][0];
      return true;
    }
    if (input.NumRows == 1) {
      this->SetSize(input.NumCols);
      for (int i = 0; i < this->size; i ++)
        this->TheObjects[i] = input.elements[0][i];
      return true;
    }
    return false;
  }
  void MakeZero(int theDim) {
    this->SetSize(theDim);
    for (int i = 0; i < theDim; i ++)
      (*this)[i] = 0;
  }
  int GetNumNonZeroCoords() const {
    int result = 0;
    for (int i = 0; i < this->size; i ++)
      if (!this->TheObjects[i].IsEqualToZero())
        result ++;
    return result;
  }
  bool IsProportionalTo(const Vector<coefficient>& other, coefficient& TimesMeEqualsOther) const;
  bool IsProportionalTo(const Vector<coefficient>& other) const {
    coefficient TimesMeEqualsOther;
    return this->IsProportionalTo(other, TimesMeEqualsOther);
  }
  int FindLCMDenominatorsTruncateToInt();
  void FindLCMDenominators(LargeIntUnsigned& output);
  inline Vector<coefficient> GetShiftToTheLeftOnePosition() {
    return this->GetShiftToTheLeft(1);
  }
  void MakeNormalInProjectivizationFromPointAndNormal(Vector<coefficient>& point, Vector<coefficient>& normal) {
    //the extra dimension is going to be the last dimension
    int newDimension = normal.size + 1;
    this->SetSize(newDimension);
    this->RootScalarEuclideanRoot(normal, point, this->TheObjects[newDimension - 1]);
    this->TheObjects[newDimension - 1].Minus();
    for (int j = 0; j < newDimension - 1; j ++)
      this->TheObjects[j] = normal[j];
  }
  bool ProjectToAffineSpace(Vector<coefficient> &output) {
    if (this->TheObjects[this->size - 1].IsEqualToZero())
      return false;
    output.SetSize(this->size - 1);
    for (int i = 0; i < this->size - 1; i ++)
      output[i] = this->TheObjects[i];
    output /= this->TheObjects[this->size - 1];
    return true;
  }
  bool MakeAffineProjectionFromNormal(affineHyperplane<Rational>& output);
/*  { int tempI= this->GetIndexFirstNonZeroCoordinate();
    if (tempI == this->size- 1)
      return false;
    output.affinePoint.MakeZero(this->size- 1);
    output.normal.SetSize(this->size- 1);
    output.affinePoint[tempI].Assign(*this->LastObject());
    output.affinePoint[tempI].Minus();
    output.affinePoint[tempI].DivideBy(this->TheObjects[tempI]);
    for (int i = 0; i < this->size- 1; i ++)
      output.normal[i] = this->TheObjects[i];
    return true;
  }*/
  int GetIndexFirstNonZeroCoordinate() {
    for (int i = 0; i < this->size; i ++) {
      if (!this->TheObjects[i].IsEqualToZero()) {
        return i;
      }
    }
    return - 1;
  }
  int getIndexLastNonZeroCoordinate() {
    for (int i = this->size - 1; i >= 0; i --) {
      if (!this->TheObjects[i].IsEqualToZero()) {
        return i;
      }
    }
    return - 1;
  }
  Vector<coefficient> GetShiftToTheLeft(int numPositions) {
    Vector<coefficient> result;
    if (numPositions > this->size)
      crash << "You requested a shift of " << numPositions << " positions in a vector with "
      << this->size << "elements" << crash;
    result.SetSize(this->size - numPositions);
    for (int i = 0; i < result.size; i ++)
      result[i] = this->TheObjects[i + numPositions];
    return result;
  }
  inline void ShiftToTheLeftOnePos() {
    this->ShiftToTheLeft(1);
  }
  void ShiftToTheLeft(int numPositions) {
    if (numPositions > this->size)
      crash << crash;
    for (int i = 0; i < this->size - numPositions; i ++)
      this->TheObjects[i] = this->TheObjects[i + numPositions];
    this->size -= numPositions;
  }
  void ShiftToTheRightInsertZeroes(int numPositions, const coefficient& theRingZero) {
    if (numPositions < 0)
      crash << crash;
    this->SetSize(this->size + numPositions);
    for (int i = this->size - 1; i >= numPositions; i --)
      this->TheObjects[i] = this->TheObjects[i - numPositions];
    for (int i = 0; i < numPositions; i ++)
      this->TheObjects[i] = theRingZero;
  }
  void SetDimInsertZeroes(int newDim) {
    int oldDim = this->size;
    this->SetSize(newDim);
    for (int i = oldDim; i < newDim; i ++)
      (*this)[i] = 0;
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
/*  template<class OtherType>
  void operator=(const Vector<OtherType>& other) {
    this->SetSize(other.size);
    for (int i = 0; i <other.size; i ++)
      this->TheObjects[i] = other.TheObjects[i];
  }*/
  void GetVectorsPerpendicularTo(
    Vectors<coefficient>& output, const coefficient& theRingUnit, const coefficient& theRingZero
  ) {
    int Pivot = - 1;
    if (!this->FindIndexFirstNonZeroCoordinateFromTheLeft(Pivot)) {
      output.MakeEiBasis(this->size, theRingUnit, theRingZero);
      return;
    }
    output.SetSize(this->size - 1);
    for (int i = 0; i < this->size; i ++) {
      if (i != Pivot) {
        Vector<coefficient>& current = output.TheObjects[i];
        current.MakeEi(this->size, i, theRingUnit, theRingZero);
        current.TheObjects[Pivot] -= this->TheObjects[i];
      }
    }
  }
  bool FindIndexFirstNonZeroCoordinateFromTheLeft(int& theIndex) {
    theIndex = - 1;
    for (int i = 0; i < this->size; i ++) {
      if (!this->TheObjects[i].IsEqualToZero()) {
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
    result.SetSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      result[i] = this->TheObjects[i];
      result[i] *= other;
    }
    return result;
  }
  Vector<coefficient> operator/(const coefficient& other) const {
    if (other.IsEqualToZero())
      crash << "This is a programming error: division by zero. Division by zero error are supposed to be handled at an earlier level. " << crash;
    Vector<coefficient> result;
    result.SetSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      result[i] = (*this)[i];
      result[i] /= other;
    }
    return result;
  }
  void operator*=(const coefficient& other) {
    for (int i = 0; i < this->size; i ++) {
      this->TheObjects[i] *= other;
    }
  }
  void operator/=(const coefficient& other) {
    for (int i = 0; i < this->size; i ++) {
      this->TheObjects[i] /= other;
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
    if (this->size != other.size)
      crash << "This is a programming error: comparing Vectors with different number of coordinates, namely, "
      << this->ToString() << " and " << other.ToString() << ". " << crash;
    coefficient c1 = 0, c2 = 0;
    for (int i = 0; i < this->size; i ++) {
      c1 += this->TheObjects[i];
      c2 += other.TheObjects[i];
    }
    if (c1 > c2)
      return true;
    if (c2 > c1)
      return false;
    for (int i = this->size - 1; i >= 0; i --) {
      if (this->TheObjects[i] > other.TheObjects[i])
        return true;
      if (other.TheObjects[i] > this->TheObjects[i])
        return false;
    }
    return false;
  }
  template <class otherType>
  void operator-=(const Vector<otherType>& other) {
    if (this->size != other.size)
      crash << "This is a programming error: subtracting vectors with different dimensions. " << crash;
    for (int i = 0; i < this->size; i ++)
      this->TheObjects[i] -= other[i];
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
  template <class otherType>
  void operator=(const Vector<otherType>& other) {
    if (this == (Vector<coefficient>*) &other)
      return;
    this->SetSize(other.size);
    for (int i = 0; i < other.size; i ++)
      this->TheObjects[i] = other[i];
  }
  template <class otherType>
  void operator=(const List<otherType>& other) {
    this->SetSize(other.size);
    for (int i = 0; i < other.size; i ++)
      this->TheObjects[i] = other[i];
  }
  void operator=(const Selection& other);
  void operator=(const SelectionWithMultiplicities& other);
  bool operator!=(const Vector<coefficient>& other) const {
    return !(*this == other);
  }
  bool IsEqualToZero() const {
    for (int i = 0; i < this->size; i ++) {
      if (!this->TheObjects[i].IsEqualToZero()) {
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
  void ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables){this->ReadFromFile(input);}
  void ReadFromFile(std::fstream& input) {
    std::string tempS;
    input >> tempS;
    if (tempS != "root_dim:")
      crash << crash;
    int tempI;
    input >> tempI;
    this->SetSize(tempI);
    for (int i = 0; i < this->size; i ++)
      this->TheObjects[i].ReadFromFile(input);
  }
  inline void WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables) {
    this->WriteToFile(output);
  }
  void WriteToFile(std::fstream& output) {
    output << "root_dim: " << this->size << " ";
    for (int i = 0; i < this->size; i ++) {
      this->TheObjects[i].WriteToFile(output);
      output << " ";
    }
  }
};

template <class coefficient>
void Vector<coefficient>::ScaleByPositiveRationalToIntegralMinHeight() {
  LargeInt numGCD, tempUI;
  bool foundNonZero = false;
  for (int i = 0; i < this->size; i ++) {
    if (!this->TheObjects[i].IsEqualToZero()) {
      if (foundNonZero) {
        if (!numGCD.IsEqualToOne()) {
          tempUI = (*this)[i].GetNumerator();
          LargeIntUnsigned::gcd(numGCD.value, tempUI.value, numGCD.value);
        }
      } else {
        numGCD = (*this)[i].GetNumerator();
        numGCD.sign = 1;
        foundNonZero = true;
      }
      tempUI = (*this)[i].GetDenominator();
      if (!tempUI.IsEqualToOne()) {
        (*this) *= (coefficient) tempUI;
      }
    }
  }
  if (foundNonZero) {
    if (!numGCD.IsEqualToOne()) {
      *this /= (coefficient)(numGCD);
    }
  }
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
  const Vector<coefficient>& input, coefficient& outputTimesMeEqualsInput
) const {
  if (this->size != input.size)
    return false;
  int IndexFirstNonZero = - 1;
  for (int i = 0; i < this->size; i ++) {
    if (!this->TheObjects[i].IsEqualToZero()) {
      IndexFirstNonZero = i;
      break;
    }
  }
  if (IndexFirstNonZero == - 1) {
    if (input.IsEqualToZero()) {
      outputTimesMeEqualsInput.MakeZero();
      return true;
    }
    return false;
  }
  Vector<Rational> tempRoot = *this;
  outputTimesMeEqualsInput = input.TheObjects[IndexFirstNonZero];
  outputTimesMeEqualsInput /= this->TheObjects[IndexFirstNonZero];
  tempRoot *= outputTimesMeEqualsInput;
  return tempRoot == input;
}

template <class coefficient>
void Vector<coefficient>::FindLCMDenominators(LargeIntUnsigned& output) {
  LargeIntUnsigned tempI, tempI2;
  output.MakeOne();
  for (int i = 0; i < this->size; i ++) {
    this->TheObjects[i].GetDenominator(tempI2);
    LargeIntUnsigned::gcd(output, tempI2, tempI);
    output.MultiplyBy(tempI2);
    output.DivPositive(tempI, output, tempI2);
  }
}

template <class coefficient>
int Vector<coefficient>::FindLCMDenominatorsTruncateToInt() {
  int result = 1;
  for (int i = 0; i < this->size; i ++) {
    result = MathRoutines::lcm(result, this->TheObjects[i].DenShort);
    if ((*this)[i].Extended != 0) {
      crash << "Coefficient is large rational at a place where that is not allowed. " << crash;
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
    if (useLatex)
      useHtml = false;
    if (useHtml && makeTable)
      out << "<table>";
    if (useLatex && makeTable)
      out << "\\begin{array}{l}";
    for (int i = 0; i < this->size; i ++) {
      tempS = this->TheObjects[i].ToStringEpsilonFormat();
      if (useHtml && makeTable)
        out << "<tr><td>";
      if (!useLatex && useHtml)
        out << "(";
      out << tempS;
      if (!useLatex && useHtml)
        out << ")";
      if (useLatex && makeTable)
        out << "\\\\";
      if (useHtml && makeTable)
        out << "</td></tr>";
      if (!makeTable) {
        if (i != this->size - 1) {
          out << ", ";
        }
      } else {
        out << "\n";
      }
    }
    if (useLatex && makeTable)
      out << "\\end{array}";
    if (useHtml && makeTable)
      out << "</table>";
    return out.str();
  }
  std::string ElementsToInequalitiesString(bool useLatex, bool useHtml, bool LastVarIsConstant, FormatExpressions& theFormat) const;
  std::string ToString(FormatExpressions* theFormat = 0) const;
  bool LinearAlgebraForVertexComputation(Selection& theSelection, Vector<coefficient>& output, Matrix<coefficient>& buffer, Selection& NonPivotPointsBuffer);
  void GetVectorsDouble(Vectors<double>& output) {
    output.SetSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      output[i] = this->TheObjects[i].GetVectorDouble();
    }
  }
  void AssignListListCoefficientType(const List<List<coefficient> >& input) {
    this->SetSize(input.size);
    for (int i = 0; i < input.size; i ++) {
      this->TheObjects[i] = input[i];
    }
  }
  void AssignListList(const List<Vectors<coefficient> >& input) {
    int count = 0;
    for (int i = 0; i < input.size; i ++)
      count += input[i].size;
    this->SetSize(0);
    this->Reserve(count);
    for (int i = 0; i < input.size; i ++) {
      for (int j = 0; j < input[i].size; j ++) {
        this->AddOnTop(input[i][j]);
      }
    }
  }
  std::string ElementToLinearCombinationString() {
    std::stringstream out;
    std::string tempS;
    for (int i = 0; i < this->size; i ++) {
      this->TheObjects[i].ToString(tempS);
      out << tempS;
      if (i != this->size - 1) {
        out << " + ";
      }
    }
    return out.str();
  }
  int ArrangeFirstVectorsBeOfMaxPossibleRank(Matrix<coefficient>& bufferMat, Selection& bufferSel);
  static unsigned int HashFunction(const Vectors<coefficient>& input) {
    return input.HashFunction();
  }
  unsigned int HashFunction() const {
    return this->::List<Vector<coefficient> >::HashFunction();
  }
  bool HasAnElementWithPositiveScalarProduct(const Vector<coefficient>& input) const {
    for (int i = 0; i < this->size; i ++) {
      if (input.ScalarEuclidean(this->TheObjects[i]).IsPositive()) {
        return true;
      }
    }
    return false;
  }
  bool HasAnElementWithNegativeScalarProduct(const Vector<coefficient>& input) const {
    for (int i = 0; i < this->size; i ++) {
      if (input.ScalarEuclidean(this->TheObjects[i]).IsNegative()) {
        return true;
      }
    }
    return false;
  }
  bool HasAnElementPerpendicularTo(const Vector<coefficient>& input) const {
    for (int i = 0; i < this->size; i ++) {
      if (input.ScalarEuclidean(this->TheObjects[i]).IsEqualToZero()) {
        return true;
      }
    }
    return false;
  }
  bool PerturbVectorToRegular(Vector<coefficient>& inputOutput);
  void SelectionToMatrix(Selection& theSelection, int OutputDimension, Matrix<coefficient>& output);
  void SelectionToMatrixAppend(Selection& theSelection, int OutputDimension, Matrix<coefficient>& output, int StartRowIndex);
  void SelectionToMatrix(Selection& theSelection, int OutputDimension, Matrix<coefficient>& output, int StartRowIndex);
  void GetGramMatrix(Matrix<coefficient>& output, const Matrix<Rational>* theBilinearForm= 0) const;
  void GetMatrixRootsToRows(Matrix<Rational>& output) const;
  void GetOrthogonalComplement(Vectors<coefficient>& output, Matrix<Rational>* theBilinearForm= 0);
  bool LinSpanContainsVector(const Vector<coefficient>& input, Matrix<coefficient>& bufferMatrix, Selection& bufferSelection) const;
  void MakeEiBasis(int theDimension) {
    this->SetSize(theDimension);
    for (int i = 0; i < this->size; i ++)
      this->TheObjects[i].MakeEi(theDimension, i);
  }
  bool LinSpanContainsVector(const Vector<coefficient>& input) const {
    Matrix<coefficient> buffer;
    Selection bufferSelection;
    return this->LinSpanContainsVector(input, buffer, bufferSelection);
  }
  static void SelectABasisInSubspace
  (const List<Vector<coefficient> >& input, List<Vector<coefficient> >& output, Selection& outputSelectedPivots);
  int GetRankOfSpanOfElements(Matrix<coefficient>* buffer = 0, Selection* bufferSelection = 0) const;
  static bool ConesIntersect(
    List<Vector<Rational> >& StrictCone,
    List<Vector<Rational> >& NonStrictCone,
    Vector<Rational>* outputLinearCombo = 0,
    Vector<Rational>* outputSplittingNormal = 0
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
    output.MakeZero(resultDim);
    for (int i = 0; i < this->size; i ++) {
      output += this->TheObjects[i];
    }
  }
  bool GetCoordsInBasis(
    const Vectors<coefficient>& inputBasis, Vectors<coefficient>& outputCoords
  );
  bool GetIntegralCoordsInBasisIfTheyExist(
    const Vectors<coefficient>& inputBasis,
    Vectors<coefficient>& output,
    const coefficient& theRingUnit,
    const coefficient& theRingMinusUnit,
    const coefficient& theRingZero
  ) const {
    Matrix<coefficient> bufferMatGaussianEliminationCC, bufferMatGaussianElimination;
    bool result = true;
    output.SetSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      if (!this->TheObjects[i].GetIntegralCoordsInBasisIfTheyExist(
        inputBasis,
        output.TheObjects[i],
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
    output.SetSize(this->size);
    for (int i = 0; i < this->size; i ++) {
      if (!this->TheObjects[i].GetIntegralCoordsInBasisIfTheyExist(
        inputBasis,
        output.TheObjects[i],
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
      out << this->TheObjects[i].ElementToStringLetterFormat(inputLetter, useLatex);
      if (i != this->size - 1) {
        out << ",";
      }
    }
    return out.str();
  }
  bool ComputeNormalExcludingIndex(Vector<coefficient>& output, int index, Matrix<coefficient>& bufferMatrix);
  bool ComputeNormalFromSelection(Vector<coefficient>& output, Selection& theSelection, Matrix<coefficient>& bufferMatrix, int theDimension);
  bool ComputeNormalFromSelectionAndExtraRoot(Vector<coefficient>& output, Vector<coefficient>& ExtraRoot, Selection& theSelection, Matrix<coefficient>& bufferMatrix, Selection& bufferSel);
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
        tempRoot = this->TheObjects[i];
        tempRoot.Minus();
        for (int j = i + 1; j < this->size; j ++) {
          if (this->TheObjects[j].IsEqualTo(tempRoot)) {
            return true;
          }
        }
      }
      return false;
    }
    HashedList<Vector<coefficient> > tempList;
    tempList.SetExpectedSize(this->size);
    tempList = *this;
    for (int i = 0; i < this->size; i ++) {
      if (tempList.Contains(- this->TheObjects[i])) {
        return true;
      }
    }
    return false;
  }
  void AssignMatrixColumns(Matrix<coefficient>& mat) {
    Vector<coefficient> tempRoot;
    this->SetSize(mat.NumCols);
    tempRoot.SetSize(mat.NumRows);
    for (int i = 0; i < mat.NumCols; i ++) {
      for (int j = 0; j < mat.NumRows; j ++) {
        tempRoot[j] = mat.elements[j][i];
      }
      this->TheObjects[i] = tempRoot;
    }
  }
  void AssignMatrixRows(const Matrix<coefficient>& mat) {
    this->size = 0;
    this->SetSize(mat.NumRows);
    for (int i = 0; i < mat.NumRows; i ++) {
      this->TheObjects[i].SetSize(mat.NumCols);
      for (int j = 0; j < mat.NumCols; j ++) {
        this->TheObjects[i].TheObjects[j] = mat.elements[i][j];
      }
    }
  }
  int GetDim() const {
    if (this->size > 0)
      return this->TheObjects[0].size;
    return - 1;
  }
  void BeefUpWithEiToLinearlyIndependentBasis(int theDim, const coefficient& ringUnit =(coefficient) 1, const coefficient& ringZero =(coefficient) 0) {
    Selection BufferSel;
    Matrix<coefficient> Buffer;
    if (this->size != 0 && theDim != this->GetDim())
      crash << crash;
    int currentRank = this->GetRankOfSpanOfElements(Buffer, BufferSel);
    if (currentRank == theDim)
      return;
    Vector<coefficient> theVect;
    for (int i = 0; i < theDim && currentRank < theDim; i ++) {
      theVect.MakeEi(theDim, i);
      this->AddOnTop(theVect);
      int candidateRank = this->GetRankOfSpanOfElements(Buffer, BufferSel);
      if (candidateRank > currentRank)
        currentRank = candidateRank;
      else
        this->size --;
    }
    if (currentRank != theDim)
      crash << crash;
  }
  void ChooseABasis() {
    Vectors<Rational> output;
    Matrix<coefficient> tempMat;
    Selection tempSel;
    for (int i = 0; i < this->size; i ++) {
      output.AddOnTop(this->TheObjects[i]);
      if (output.GetRankOfSpanOfElements(&tempMat, &tempSel) < output.size)
        output.RemoveLastObject();
    }
    this->operator=(output);
  }
  static void IntersectTwoLinSpaces(
    const List<Vector<coefficient> >& firstSpace,
    const List<Vector<coefficient> >& secondSpace,
    List<Vector<coefficient> >& output
  );
  bool IsRegular(Vector<coefficient>& r, Vector<coefficient>& outputFailingNormal, int theDimension);
  void operator=(const List<List<coefficient> >& other) {
    this->SetSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->TheObjects[i] = other.TheObjects[i];
    }
  }
  template <class otherType>
  void operator=(const List<Vector<otherType> >& other) {
    this->SetSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->TheObjects[i] = other[i];
    }
  }
};

template <class coefficient>
bool Vector<coefficient>::AssignString(const std::string& input) {
  MacroRegisterFunctionWithName("Vector::AssignString");
  this->SetSize(0);
  int currentDigit = - 1;
  int sign = 1;
  for (int i = 0; i < (signed) input.size(); i ++) {
    char previous = (i == 0) ? '(' : input[i - 1];
    if (MathRoutines::isADigit(input[i], &currentDigit)) {
      if (!MathRoutines::isADigit(previous)) {
        this->AddOnTop( (coefficient) 0);
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
  if (inputBasis.size == 0)
    return false;
  MacroRegisterFunctionWithName("Vector::GetCoordsInBasiS");
  Vectors<coefficient> bufferVectors;
  Matrix<coefficient> bufferMat;
  if (this->size != inputBasis[0].size)
    crash << "This is a programming error: asking to get coordinates of vector of "
    << this->size << " coordinates using a basis whose first vector has "
    << inputBasis[0].size << " coordinates." << crash;
  bufferVectors.Reserve(inputBasis.size + 1);
  bufferVectors.AddListOnTop(inputBasis);
  bufferVectors.AddOnTop(*this);
  if (!bufferVectors.GetLinearDependence(bufferMat))
    return false;
  coefficient tempCF = bufferMat(bufferMat.NumRows - 1, 0);
  bufferMat /= tempCF;
  output.SetSize(bufferMat.NumRows - 1);
  for (int i = 0; i < bufferMat.NumRows - 1; i ++) {
    bufferMat(i, 0).Minus();
    output[i] = bufferMat(i, 0);
  }
  return true;
}

template <class coefficient>
void Vectors<coefficient>::GetLinearDependenceRunTheLinearAlgebra(Matrix<coefficient>& outputTheSystem, Selection& outputNonPivotPoints) {
  MacroRegisterFunctionWithName("Vectors::GetLinearDependenceRunTheLinearAlgebra");
  if (this->size == 0)
    return;
  int Dimension = (int) (*this)[0].size;
  outputTheSystem.init(Dimension, (int)this->size);
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
  tempVectors.AddOnTop(input);
  return this->GetRankOfSpanOfElements(&bufferMatrix, &bufferSelection) == tempVectors.GetRankOfSpanOfElements(&bufferMatrix, &bufferSelection);
}

template <class coefficient>
bool Vectors<coefficient>::GetCoordsInBasis(
  const Vectors<coefficient>& inputBasis, Vectors<coefficient>& outputCoords
) {
  MacroRegisterFunctionWithName("Vectors::GetCoordsInBasis");
  //if (this == 0 || &outputCoords == 0 || this == &outputCoords)
  //  crash << "This is a programming error: input and output addresses are zero or coincide. this address: "
  //  << (unsigned long) this << "; output address: " << (unsigned long)(&outputCoords) << crash;
  outputCoords.SetSize(this->size);
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
    crash << "Output not allowed to coincide with this object" << crash;
  }
  output.MakeZero(inputBasis.size);
  theCombination = *this;
  int col = 0;
//  stOutput << "<br>vector whose coords we wanna find: " << this->ToString();
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
    output[i] = this->TheObjects[col];
    output[i] /= bufferMatGaussianElimination.elements[i][col];
    tempRoot *= output[i];
    theCombination -= tempRoot;
  }
  if (!theCombination.IsEqualToZero())
    return false;
  bufferMatGaussianEliminationCC.ActMultiplyVectorRowOnTheRight(output);
  return true;
}

template<class coefficient>
void Vectors<coefficient>::GetGramMatrix(Matrix<coefficient>& output, const Matrix<Rational>* theBilinearForm) const {
  output.Resize(this->size, this->size, false);
  for (int i = 0; i < this->size; i ++) {
    for (int j = i; j < this->size; j ++) {
      if (theBilinearForm != 0) {
        Vector<coefficient>::ScalarProduct(this->TheObjects[i], this->TheObjects[j], *theBilinearForm, output.elements[i][j]);
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
bool Vectors<coefficient>::ContainsARootNonPerpendicularTo(const Vector<coefficient>& input, const Matrix<coefficient>& theBilinearForm) {
  for (int i = 0; i < this->size; i ++) {
    if (!Vector<coefficient>::ScalarProduct(this->TheObjects[i], input, theBilinearForm).IsEqualToZero()) {
      return true;
    }
  }
  return false;
}

template<class coefficient>
int Vectors<coefficient>::ArrangeFirstVectorsBeOfMaxPossibleRank(Matrix<coefficient>& bufferMat, Selection& bufferSel) {
  if (this->size == 0)
    return 0;
  int theDimension = this->GetDimensionOfElements();
  Vectors<Rational> tempRoots;
  int oldRank = 0;
  for (int i = 0; i < this->size; i ++) {
    tempRoots.AddOnTop(this->TheObjects[i]);
    int newRank = tempRoots.GetRankOfSpanOfElements(bufferMat, bufferSel);
    if (newRank == oldRank) {
      tempRoots.RemoveIndexSwapWithLast(tempRoots.size - 1);
    } else {
      this->SwapTwoIndices(oldRank, i);
      if (oldRank + 1 != newRank)
        crash << "Old rank plus one must equal new rank." << crash;
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
  void ToString(std::string& output);
  //void InduceFromFacet(Facet& input);
  //the below returns false if the projection is not of full dimension
  unsigned int HashFunction() const;
  static inline unsigned int HashFunction(const affineHyperplane& input) {
    return input.HashFunction();
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
  tempRoot1.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
  tempRoot2.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
  if (!(tempRoot1 == tempRoot2))
    return false;
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
  if (tempI == - 1)
    crash << crash;
  if (tempI == input.size - 1)
    return false;
  this->affinePoint.MakeZero(input.size);
  this->affinePoint.SetSize(input.size - 1);
  this->affinePoint[tempI] = input[input.size - 1];
  this->affinePoint[tempI].Minus();
  this->affinePoint[tempI].DivideBy(input[tempI]);
  this->normal = input;
  this->normal.SetSize(input.size - 1);
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
  if (tempRat.IsEqualToZero())
    return true;
  for (int i = 0; i < this->normal.size; i ++) {
    Rational& tempRat2 = this->normal[i];
    if (tempRat.IsNegative() && tempRat2.IsNegative())
      return true;
    if (tempRat.IsPositive() && tempRat2.IsPositive())
      return true;
  }
  return false;
}

template <class coefficient>
void affineHyperplane<coefficient>::MakeFromNormalAndPoint(Vector<coefficient>& inputPoint, Vector<coefficient>& inputNormal) {
  this->affinePoint = inputPoint;
  this->normal = inputNormal;
}

template <class coefficient>
void affineHyperplane<coefficient>::ToString(std::string& output) {
  std::stringstream out;
  out << "point: " << this->affinePoint << ", normal: " << this->normal;
  output = out.str();
}

template <class coefficient>
unsigned int affineHyperplane<coefficient>::HashFunction() const {
  //warning: if normal gets streched, the hashfunction should not change!
  Vector<Rational> tempNormal;
  tempNormal = this->normal;
  tempNormal.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
  Rational tempRat = this->normal.ScalarEuclidean(this->affinePoint);
  return this->normal.HashFunction() + tempRat.HashFunction();
}

class affineHyperplanes: public List<affineHyperplane<Rational> > {
public:
  std::string DebugString;
  void ToString(std::string& output);
  void ComputeDebugString() {
    this->ToString(this->DebugString);
  }
};

class affineCone {
public:
  affineHyperplanes theWalls;
  unsigned int HashFunction() const;
  inline static unsigned int HashFunction(const affineCone& input) {
    return input.HashFunction();
  }
  inline int GetDimension();
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
