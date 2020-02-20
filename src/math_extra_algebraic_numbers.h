// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader1_3_h_already_included
#define vpfHeader1_3_h_already_included

#include "math_general.h"

class AlgebraicClosureRationals;
class AlgebraicNumber {
  friend std::ostream& operator <<(std::ostream& output, const AlgebraicNumber& theNumber) {
    output << theNumber.ToString();
    return output;
  }
  friend AlgebraicNumber operator-(const AlgebraicNumber& argument) {
    argument.CheckConsistency();
    AlgebraicNumber result = argument;
    result.CheckConsistency();
    result *= - 1;
    result.CheckConsistency();
    return result;
  }
  public:
  AlgebraicClosureRationals* owner;
  int basisIndex;
  VectorSparse<Rational> theElT;
  bool flagDeallocated;
  AlgebraicNumber(): owner(nullptr), basisIndex(0), flagDeallocated(false) {}
  AlgebraicNumber(const Rational& other): owner(nullptr), basisIndex(0), flagDeallocated(false) {
    this->operator= (other);
  }
  AlgebraicNumber(int other): owner(nullptr), basisIndex(0), flagDeallocated(false) {
    this->operator=(Rational(other));
  }
  AlgebraicNumber(const AlgebraicNumber& other): owner(nullptr), basisIndex(0), flagDeallocated(false) {
    this->operator=(other);
  }
  bool IsExpressedViaLatestBasis() const;
  void ExpressViaLatestBasis();
  bool AssignCosRationalTimesPi(const Rational& input, AlgebraicClosureRationals& inputOwner);
  bool AssignSinRationalTimesPi(const Rational& input, AlgebraicClosureRationals& inputOwner) {
    Rational rHalf(1, 2);
    return this->AssignCosRationalTimesPi(rHalf - input, inputOwner);
  }
  bool NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost() const;
  bool NeedsParenthesisForMultiplication() const;
  bool CheckConsistency() const;
  bool CheckNonZeroOwner() const;
  bool CheckCommonOwner(const AlgebraicNumber& other) const;
  unsigned int HashFunction() const;
  static unsigned int HashFunction(const AlgebraicNumber& input) {
    return input.HashFunction();
  }
  LargeIntegerUnsigned GetNumerator() const {
    return this->theElT.FindGCDCoefficientNumeratorsOverRationals().GetNumerator().value;
  }
  LargeIntegerUnsigned GetDenominator() const {
    return this->GetDenominatorRationalPart().GetDenominator();
  }
  bool IsPositive() {
    Rational ratPart;
    if (this->IsRational(&ratPart)) {
      return ratPart.IsPositive();
    }
    return false;
  }
  bool IsRational(Rational* whichRational = nullptr) const;
  bool IsNegative() const {
    Rational theRationalValue;
    if (this->IsRational(&theRationalValue)) {
      return theRationalValue.IsNegative();
    }
    return false;
  }
  bool IsEqualToZero() const;
  bool IsEqualToOne() const {
    return (*this) == 1;
  }
  void operator= (const AlgebraicNumber& other) {
    this->basisIndex = other.basisIndex;
    this->owner = other.owner;
    this->theElT = other.theElT;
  }
  void operator=(const Rational& other);
  void operator=(int other) {
    *this = Rational(other);
  }
  bool ConstructFromMinPoly(const Polynomial<AlgebraicNumber>& thePoly, AlgebraicClosureRationals& inputOwner);
  bool ConstructFromMinPoly(const Polynomial<Rational>& thePoly, AlgebraicClosureRationals& inputOwner) {
    Polynomial<AlgebraicNumber> polyConverted;
    polyConverted = thePoly;
    return this->ConstructFromMinPoly(polyConverted, inputOwner);
  }
  bool AssignRationalQuadraticRadical(const Rational& inpuT, AlgebraicClosureRationals& inputOwner);
  void AssignRational(const Rational& input, AlgebraicClosureRationals& inputOwner);
  Rational GetDenominatorRationalPart() const;
  Rational GetNumeratorRationalPart() const;
  void SqrtMeDefault();
  bool IsSmallInteger(int* whichInteger) const {
    Rational theRat;
    if (!this->IsRational(&theRat)) {
      return false;
    }
    return theRat.IsSmallInteger(whichInteger);
  }
  bool IsInteger(LargeInteger* whichInteger) const {
    Rational theRat;
    if (!this->IsRational(&theRat)) {
      return false;
    }
    return theRat.IsInteger(whichInteger);
  }
  void RadicalMeDefault(int theRad);
  void Invert();
  bool EvaluatesToDouble(double* outputWhichDouble) const;
  void operator/=(const AlgebraicNumber& other);
  bool operator==(const AlgebraicNumber& other) const;
  bool operator==(const Rational& other) const;
  bool operator==(int other) const {
    return *this == Rational(other);
  }

  bool operator!= (const AlgebraicNumber& other) const {
    return !(*this == other);
  }
  bool operator!= (int other) const {
    return !(*this == other);
  }
  void Minus() {
    this->theElT *= - 1;
  }
  void operator= (const Polynomial<AlgebraicNumber>& other);
  AlgebraicNumber operator+(const AlgebraicNumber& other) const {
    this->CheckConsistency();
    AlgebraicNumber result = *this;
    result.CheckConsistency();
    other.CheckConsistency();
    result += other;
    result.CheckConsistency();
    return result;
  }
  AlgebraicNumber operator-(const AlgebraicNumber& other) const {
    AlgebraicNumber result = *this;
    result -= other;
    return result;
  }
  AlgebraicNumber operator*(const AlgebraicNumber& other) const {
    AlgebraicNumber result = *this;
    result *= other;
    return result;
  }
  AlgebraicNumber operator/(const AlgebraicNumber& other) const {
    this->CheckConsistency();
    other.CheckConsistency();
    AlgebraicNumber result = *this;

    result /= other;
    return result;
  }
  void operator+= (const AlgebraicNumber& other);
  void operator-= (const AlgebraicNumber& other);
  void operator*= (const AlgebraicNumber& other);
  void operator*= (const Rational& other);
  void operator*= (LargeInteger other) {
    this->operator*=(Rational(other));
  }
  void operator*= (int other) {
    this->operator*=(Rational(other));
  }
  bool operator>(const AlgebraicNumber& other) const;
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
};

// The algebraic closure of the rationals is
// represented as a series of nested extensions of the rationals:
// Rationals = Q = A_0 <= A_1 <= A_2 <= ...
// Each extension A_i comes with a basis e_{i, 1}, e_{i, 2}, ...
// Here, we explicitly allow the basis to be linearly dependent.
// In particular, we explicitly allow that there exist an extension for which
// the next extension has fewer basis vectors.
// The latter case automatically implies the
// basis of the preceding extension was redundant.
class AlgebraicClosureRationals {
public:
  List<MatrixTensor<Rational> > theBasisMultiplicative;

  List<List<VectorSparse<Rational> > > theBasesAdditive;

  MatrixTensor<Rational> GeneratingElementTensorForm;
  Matrix<Rational> GeneratingElementMatForm;
  AlgebraicNumber GeneratingElemenT;
  Vectors<Rational> theGeneratingElementPowersBasis;

  bool flagIsQuadraticRadicalExtensionRationals;
  HashedList<LargeInteger> theQuadraticRadicals;
  List<std::string> DisplayNamesBasisElements;
  void AddNewBasis();

  void RegisterNewBasis(const MatrixTensor<Rational>& theInjection);
  void reset();
  bool CheckConsistency() const;
  AlgebraicClosureRationals() {
    this->reset();
  }
  bool MergeRadicals(const List<LargeInteger>& theRadicals);
  void ChooseGeneratingElement();
  bool ReduceMe();
  void ComputeDisplayStringsFromRadicals();
  bool GetRadicalSelectionFromIndex(int inputIndex, Selection& theSel);
  int GetDimensionOverTheRationals() const;
  static int GetIndexFromRadicalSelection(const Selection& theSel);
  void GetMultiplicativeOperatorFromRadicalSelection(const Selection& theSel, MatrixTensor<Rational>& outputOp);
  void GetMultiplicationBy(const AlgebraicNumber& input, MatrixTensor<Rational>& output);
  void GetAdditionTo(const AlgebraicNumber& input, VectorSparse<Rational>& output);
  void ConvertPolyDependingOneVariableToPolyDependingOnFirstVariableNoFail(
    const Polynomial<AlgebraicNumber>& input, Polynomial<AlgebraicNumber>& output
  );
  bool AdjoinRootMinPoly(const Polynomial<AlgebraicNumber>& thePoly, AlgebraicNumber& outputRoot);
  bool AdjoinRootQuadraticPolyToQuadraticRadicalExtension(
    const Polynomial<AlgebraicNumber>& thePoly, AlgebraicNumber& outputRoot
  );
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
  bool splitToPartialFractionsOverRealAlgebraicNumbers(
    RationalFunctionOld& inputRF,
    List<Polynomial<AlgebraicNumber> >& outputNumerators,
    List<Polynomial<AlgebraicNumber> >& outputDenominators,
    std::stringstream* reportStream = nullptr
  );
};

class ElementZmodP {
public:
  LargeIntegerUnsigned theModulus;
  LargeIntegerUnsigned theValue;
  bool flagDeallocated;
  friend ElementZmodP operator*(int left, const ElementZmodP& right);
  unsigned int HashFunction() const;
  static unsigned int HashFunction(const ElementZmodP& input) {
    return input.HashFunction();
  }
  ElementZmodP() {
    this->flagDeallocated = false;
  }
  ElementZmodP(const ElementZmodP& other) {
    this->flagDeallocated = false;
    *this = other;
  }
  ~ElementZmodP() {
    this->flagDeallocated = true;
  }
  void CheckIamInitialized() const;
  std::string ToString(FormatExpressions* theFormat = nullptr) const;
  bool IsEqualToZero() const {
    return this->theValue.IsEqualToZero();
  }
  void operator=(const ElementZmodP& other);
  void operator=(const LargeIntegerUnsigned& other);
  void MakeOne(const LargeIntegerUnsigned& newModulo);
  void MakeMOne(const LargeIntegerUnsigned& newModulo);
  void CheckEqualModuli(const ElementZmodP& other);
  bool operator==(int other) const;
  bool operator==(const ElementZmodP& other) const;
  void operator*=(const ElementZmodP& other);
  void operator*=(const LargeInteger& other);
  void operator+=(const ElementZmodP& other);
  void operator-=(const ElementZmodP& other);
  void operator-=(const LargeIntegerUnsigned& other);
  bool operator+=(const Rational& other);
  void operator=(const int other);
  ElementZmodP operator+(const Rational& other) const;
  ElementZmodP operator-(const ElementZmodP& other) const;
  ElementZmodP operator/(const Rational& other) const;
  ElementZmodP operator/(const ElementZmodP& other) const;
  ElementZmodP operator*(const Rational& other) const;
  ElementZmodP operator*(const ElementZmodP& other) const;
  void operator=(const LargeInteger& other);
  bool AssignRational(const Rational& other);
  void operator=(const Rational& other);
  bool operator/=(const ElementZmodP& den);
  bool operator/=(const LargeInteger& den);
  void ScaleToIntegralMinHeightAndGetPoly(
    const Polynomial<Rational>& input, Polynomial<ElementZmodP>& output, const LargeIntegerUnsigned& newModulo
  );
};
#endif
