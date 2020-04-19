// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader1_3_h_already_included
#define vpfHeader1_3_h_already_included

#include "math_general.h"

class AlgebraicClosureRationals;
class AlgebraicNumber {
  friend std::ostream& operator<<(std::ostream& output, const AlgebraicNumber& theNumber) {
    output << theNumber.toString();
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
  VectorSparse<Rational> element;
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
  AlgebraicNumber(AlgebraicClosureRationals* inputOwner) {
    this->owner = inputOwner;
    this->basisIndex = 0;
    this->flagDeallocated = false;
  }
  static AlgebraicNumber zero();
  bool IsExpressedViaLatestBasis() const;
  void ExpressViaLatestBasis();
  bool AssignCosRationalTimesPi(const Rational& input, AlgebraicClosureRationals& inputOwner);
  bool AssignSinRationalTimesPi(const Rational& input, AlgebraicClosureRationals& inputOwner) {
    Rational rHalf(1, 2);
    return this->AssignCosRationalTimesPi(rHalf - input, inputOwner);
  }
  bool NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost() const;
  bool NeedsParenthesisForMultiplication(FormatExpressions* unused) const;
  bool CheckConsistency() const;
  bool CheckNonZeroOwner() const;
  bool CheckCommonOwner(const AlgebraicNumber& other) const;
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const AlgebraicNumber& input) {
    return input.hashFunction();
  }
  // Returns the number by which the vector was multiplied.
  static AlgebraicNumber scaleNormalizeIndex(
    List<AlgebraicNumber>& output, int indexNonZeroElement
  );
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
    this->element = other.element;
  }
  void operator=(const Rational& other);
  void operator=(int other) {
    *this = Rational(other);
  }
  bool ConstructFromMinPoly(
    const Polynomial<AlgebraicNumber>& thePoly,
    AlgebraicClosureRationals& inputOwner,
    std::stringstream* commentsOnFailure
  );
  bool ConstructFromMinPoly(
    const Polynomial<Rational>& thePoly,
    AlgebraicClosureRationals& inputOwner,
    std::stringstream* commentsOnFailure
  ) {
    Polynomial<AlgebraicNumber> polyConverted;
    polyConverted = thePoly;
    return this->ConstructFromMinPoly(polyConverted, inputOwner, commentsOnFailure);
  }
  bool AssignRationalQuadraticRadical(
    const Rational& inpuT,
    AlgebraicClosureRationals& inputOwner,
    std::stringstream* commentsOnFailure
  );
  void AssignRational(const Rational& input, AlgebraicClosureRationals& inputOwner);
  void SqrtMeDefault(std::stringstream* commentsOnError);
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
  bool RadicalMeDefault(int radical, std::stringstream* commentsOnError);
  void invert();
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
    this->element *= - 1;
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
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringNonInjected(FormatExpressions* theFormat = nullptr) const;
};

// The algebraic closure of the rationals is
// represented as a series of nested extensions of the rationals:
// Rationals = Q = A_0 <= A_1 <= A_2 <= ...
// Each extension A_i comes with a basis e_{i, 1}, e_{i, 2}, ...
class AlgebraicClosureRationals {
public:
  // Basis over the rationals of the latest algebraic extension.
  List<MatrixTensor<Rational> > latestBasis;

  // Injections of earlier rational number bases
  // into the latest basis.
  // The [i][j]-th element refers to the image basis element e_{i, j},
  // i.e., the j^th basis element of the i^th [extension A_i].
  // More precisely, the [i][j]-th element of the map below describes the image of
  // e_{i, j} when injected down to the latest extension A_n, with
  // coordinates relative to the latest basis e_{n, 1}, ..., e_{n, n}.
  List<List<VectorSparse<Rational> > > basisInjections;

  MatrixTensor<Rational> GeneratingElementTensorForm;
  Matrix<Rational> GeneratingElementMatForm;
  AlgebraicNumber GeneratingElemenT;
  Vectors<Rational> theGeneratingElementPowersBasis;

  bool flagIsQuadraticRadicalExtensionRationals;
  HashedList<LargeInteger> theQuadraticRadicals;
  List<std::string> DisplayNamesBasisElements;
  void InjectOldBases(const MatrixTensor<Rational>* injectionNullForIdentity);
  void AppendAdditiveEiBasis();
  void AssignDefaultBasisDisplayNames();
  void reset();
  bool CheckConsistency() const;
  AlgebraicClosureRationals() {
    this->reset();
  }
  bool MergeRadicals(const List<LargeInteger>& theRadicals);
  bool ChooseGeneratingElement(int attemptsLimitZeroForNone, std::stringstream* commentsOnFailure);
  bool ReduceMe(std::stringstream* commentsOnFailure);
  void ContractBasesIfRedundant(
    AlgebraicClosureRationals& previousCopy,
    AlgebraicNumber* outputImageGenerator
  );
  void ComputeDisplayStringsFromRadicals();
  bool GetRadicalSelectionFromIndex(int inputIndex, Selection& theSel);
  int getDimensionOverTheRationals() const;
  static int GetIndexFromRadicalSelection(const Selection& theSel);
  void GetMultiplicativeOperatorFromRadicalSelection(
    const Selection& theSel, MatrixTensor<Rational>& outputOp
  );
  void GetMultiplicationBy(const AlgebraicNumber& input, MatrixTensor<Rational>& output);
  void GetAdditionTo(const AlgebraicNumber& input, VectorSparse<Rational>& output);
  void ConvertPolyDependingOneVariableToPolyDependingOnFirstVariableNoFail(
    const Polynomial<AlgebraicNumber>& input, Polynomial<AlgebraicNumber>& output
  );
  bool AdjoinRootMinimalPolynomial(
    const Polynomial<AlgebraicNumber>& thePoly,
    AlgebraicNumber& outputRoot,
    std::stringstream* commentsOnFailure
  );
  bool AdjoinRootQuadraticPolyToQuadraticRadicalExtension(
    const Polynomial<AlgebraicNumber>& thePoly,
    AlgebraicNumber& outputRoot,
    std::stringstream* commentsOnFailure
  );
  std::string ToStringQuadraticRadical(FormatExpressions* theFormat = nullptr) const;
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringFull(FormatExpressions* theFormat = nullptr) const;
  bool splitToPartialFractionsOverRealAlgebraicNumbers(
    RationalFunction& inputRF,
    List<Polynomial<AlgebraicNumber> >& outputNumerators,
    List<Polynomial<AlgebraicNumber> >& outputDenominators,
    std::stringstream* reportStream = nullptr
  );
  AlgebraicNumber One();
  AlgebraicNumber Zero();
  AlgebraicNumber fromRational(const Rational& input);
};

class ElementZmodP {
public:
  friend std::ostream& operator<<(std::ostream& output, const ElementZmodP& input) {
    output << input.toString();
    return output;
  }
  LargeIntegerUnsigned theModulus;
  LargeIntegerUnsigned theValue;
  bool flagDeallocated;
  friend ElementZmodP operator*(int left, const ElementZmodP& right);
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const ElementZmodP& input) {
    return input.hashFunction();
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
  void invert();
  bool IsEqualToZero() const {
    return this->theValue.IsEqualToZero();
  }
  // Returns the number by which the vector was multiplied.
  static ElementZmodP scaleNormalizeIndex(
    List<ElementZmodP>& toBeScaled, int indexNonZeroElement
  );
  void operator=(const ElementZmodP& other);
  void operator=(const LargeIntegerUnsigned& other);
  void makeOne(const LargeIntegerUnsigned& newModulo);
  void MakeMOne(const LargeIntegerUnsigned& newModulo);
  void CheckEqualModuli(const ElementZmodP& other);
  bool operator==(int other) const;
  bool operator==(const ElementZmodP& other) const;
  void operator*=(const ElementZmodP& other);
  void operator*=(int other);
  void operator*=(const LargeInteger& other);
  bool operator*=(const Rational& other);
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
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  std::string toStringModP() const;
  static std::string toStringModP(const LargeIntegerUnsigned& modulus);
  void operator=(const LargeInteger& other);
  bool AssignRational(const Rational& other);
  void operator=(const Rational& other);
  bool operator/=(const ElementZmodP& den);
  bool operator/=(const LargeInteger& den);
  bool NeedsParenthesisForMultiplication(FormatExpressions* theFormat = nullptr) const;
  static void convertModuloIntegerAfterScalingToIntegral(
    const Polynomial<Rational>& input,
    Polynomial<ElementZmodP>& output,
    const LargeIntegerUnsigned& newModulo
  );
  static ElementZmodP zero();
  bool operator>(const ElementZmodP& other) const;
  class Test {
  public:
    static bool all();
    static bool basicOperations();
  };
};

class PolynomialFactorizationCantorZassenhaus {
public:
  ElementZmodP one;
  int degree;
  int degreeUnknownFactor;
  PolynomialFactorization<ElementZmodP, PolynomialFactorizationCantorZassenhaus>* output;
  Polynomial<ElementZmodP> current;
  bool oneFactor(
    std::stringstream* commentsOnFailure
  );
  bool oneFactorGo(std::stringstream* commentsOnFailure);
  PolynomialFactorizationCantorZassenhaus() {
    this->output = nullptr;
  }
};

#endif
