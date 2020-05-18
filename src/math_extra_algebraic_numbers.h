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
    argument.checkConsistency();
    AlgebraicNumber result = argument;
    result.checkConsistency();
    result *= - 1;
    result.checkConsistency();
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
  static AlgebraicNumber zeroStatic();
  bool isExpressedViaLatestBasis() const;
  void expressViaLatestBasis();
  bool assignCosRationalTimesPi(const Rational& input, AlgebraicClosureRationals& inputOwner);
  bool assignSinRationalTimesPi(const Rational& input, AlgebraicClosureRationals& inputOwner) {
    Rational half(1, 2);
    return this->assignCosRationalTimesPi(half - input, inputOwner);
  }
  bool needsParenthesisForMultiplicationWhenSittingOnTheRightMost() const;
  bool needsParenthesisForMultiplication(FormatExpressions* unused) const;
  bool checkConsistency() const;
  bool checkNonZeroOwner() const;
  bool checkCommonOwner(const AlgebraicNumber& other) const;
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const AlgebraicNumber& input) {
    return input.hashFunction();
  }
  // Returns the number by which the vector was multiplied.
  static AlgebraicNumber scaleNormalizeIndex(
    List<AlgebraicNumber>& output, int indexNonZeroElement
  );
  bool isPositive() {
    Rational ratPart;
    if (this->isRational(&ratPart)) {
      return ratPart.isPositive();
    }
    return false;
  }
  bool isRational(Rational* whichRational = nullptr) const;
  bool isNegative() const {
    Rational therationalValue;
    if (this->isRational(&therationalValue)) {
      return therationalValue.isNegative();
    }
    return false;
  }
  bool isEqualToZero() const;
  bool isEqualToOne() const {
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
  bool constructFromMinimalPolynomial(
    const Polynomial<AlgebraicNumber>& thePoly,
    AlgebraicClosureRationals& inputOwner,
    std::stringstream* commentsOnFailure
  );
  bool constructFromMinimalPolynomial(
    const Polynomial<Rational>& thePoly,
    AlgebraicClosureRationals& inputOwner,
    std::stringstream* commentsOnFailure
  );
  AlgebraicNumber one();
  bool assignRationalQuadraticRadical(
    const Rational& input,
    AlgebraicClosureRationals& inputOwner,
    std::stringstream* commentsOnFailure
  );
  void assignRational(const Rational& input, AlgebraicClosureRationals& inputOwner);
  void squareRootDefault(std::stringstream* commentsOnError);
  bool isSmallInteger(int* whichInteger) const {
    Rational theRat;
    if (!this->isRational(&theRat)) {
      return false;
    }
    return theRat.isSmallInteger(whichInteger);
  }
  bool isInteger(LargeInteger* whichInteger) const {
    Rational theRat;
    if (!this->isRational(&theRat)) {
      return false;
    }
    return theRat.isInteger(whichInteger);
  }
  bool radicalMeDefault(int radical, std::stringstream* commentsOnError);
  void invert();
  bool evaluatesToDouble(double* outputWhichDouble) const;
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
  void minus() {
    this->element *= - 1;
  }
  void operator= (const Polynomial<AlgebraicNumber>& other);
  AlgebraicNumber operator+(const AlgebraicNumber& other) const {
    this->checkConsistency();
    AlgebraicNumber result = *this;
    result.checkConsistency();
    other.checkConsistency();
    result += other;
    result.checkConsistency();
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
    this->checkConsistency();
    other.checkConsistency();
    AlgebraicNumber result = *this;

    result /= other;
    return result;
  }
  void operator+=(const AlgebraicNumber& other);
  void operator-=(const AlgebraicNumber& other);
  void operator*=(const AlgebraicNumber& other);
  void operator*=(const Rational& other);
  void operator*=(LargeInteger other) {
    this->operator*=(Rational(other));
  }
  void operator*= (int other) {
    this->operator*=(Rational(other));
  }
  bool operator>(const AlgebraicNumber& other) const;
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  std::string toStringNonInjected(FormatExpressions* theFormat = nullptr) const;
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

  MatrixTensor<Rational> generatingElementTensorForm;
  Matrix<Rational> generatingElementMatrixForm;
  AlgebraicNumber generatingElement;
  Vectors<Rational> theGeneratingElementPowersBasis;

  bool flagIsQuadraticRadicalExtensionRationals;
  HashedList<LargeInteger> theQuadraticRadicals;
  List<std::string> displayNamesBasisElements;
  void injectOldBases(const MatrixTensor<Rational>* injectionNullForIdentity);
  void appendAdditiveEiBasis();
  void assignDefaultBasisDisplayNames();
  void reset();
  bool checkConsistency() const;
  AlgebraicClosureRationals() {
    this->reset();
  }
  bool mergeRadicals(const List<LargeInteger>& theRadicals);
  bool chooseGeneratingElement(int attemptsLimitZeroForNone, std::stringstream* commentsOnFailure);
  bool reduceMe(std::stringstream* commentsOnFailure);
  void contractBasesIfRedundant(
    AlgebraicClosureRationals& previousCopy,
    AlgebraicNumber* outputImageGenerator
  );
  void computeDisplayStringsFromRadicals();
  bool getRadicalSelectionFromIndex(int inputIndex, Selection& theSel);
  int getDimensionOverTheRationals() const;
  static int getIndexFromRadicalSelection(const Selection& theSel);
  void getMultiplicativeOperatorFromRadicalSelection(
    const Selection& theSel, MatrixTensor<Rational>& outputOp
  );
  void getMultiplicationBy(const AlgebraicNumber& input, MatrixTensor<Rational>& output);
  void getAdditionTo(const AlgebraicNumber& input, VectorSparse<Rational>& output);
  void convertPolyDependingOneVariableToPolyDependingOnFirstVariableNoFail(
    const Polynomial<AlgebraicNumber>& input, Polynomial<AlgebraicNumber>& output
  );
  bool adjoinRootMinimalPolynomial(
    const Polynomial<AlgebraicNumber>& thePoly,
    AlgebraicNumber& outputRoot,
    std::stringstream* commentsOnFailure
  );
  bool adjoinRootQuadraticPolynomialToQuadraticRadicalExtension(
    const Polynomial<AlgebraicNumber>& thePoly,
    AlgebraicNumber& outputRoot,
    std::stringstream* commentsOnFailure
  );
  std::string toStringQuadraticRadical(FormatExpressions* theFormat = nullptr) const;
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  std::string toStringFull(FormatExpressions* theFormat = nullptr) const;
  bool splitToPartialFractionsOverRealAlgebraicNumbers(
    RationalFunction<Rational>& inputRF,
    List<Polynomial<AlgebraicNumber> >& outputNumerators,
    List<Polynomial<AlgebraicNumber> >& outputDenominators,
    std::stringstream* reportStream = nullptr
  );
  AlgebraicNumber one();
  AlgebraicNumber zero();
  AlgebraicNumber fromRational(const Rational& input);
};

class ElementZmodP {
public:
  friend std::ostream& operator<<(std::ostream& output, const ElementZmodP& input) {
    output << input.toString();
    return output;
  }
  static const int maximumModulusForUserFacingPolynomialDivision;
  LargeIntegerUnsigned modulus;
  LargeIntegerUnsigned value;
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
  void checkIamInitialized() const;
  bool invert();
  bool isEqualToZero() const {
    return this->value.isEqualToZero();
  }
  // Returns the number by which the vector was multiplied.
  static ElementZmodP scaleNormalizeIndex(
    List<ElementZmodP>& toBeScaled, int indexNonZeroElement
  );
  // Required by RationalFunction.
  // Returns a copy of the number.
  ElementZmodP getNumerator() const;
  // Required by RationalFunction.
  // Returns one.
  ElementZmodP getDenominator() const;
  void operator=(const ElementZmodP& other);
  void operator=(const LargeIntegerUnsigned& other);
  void makeZero();
  void makeOne(const LargeIntegerUnsigned& newModulo);
  void makeMinusOne(const LargeIntegerUnsigned& newModulo);
  void checkEqualModuli(const ElementZmodP& other);

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
  bool assignRational(const Rational& other);
  void operator=(const Rational& other);
  bool operator/=(const ElementZmodP& den);
  bool operator/=(const LargeInteger& den);
  bool needsParenthesisForMultiplication(FormatExpressions* theFormat = nullptr) const;
  static void convertModuloIntegerAfterScalingToIntegral(
    const Polynomial<Rational>& input,
    Polynomial<ElementZmodP>& output,
    const LargeIntegerUnsigned& newModulo
  );
  // Format a polynomial with modular coefficients.
  // The modulo information will not be repeated for all coefficients, but will be put
  // at the end of the expression.
  std::string toStringPolynomial(const Polynomial<ElementZmodP>& input, FormatExpressions* format) const;
  ElementZmodP zero() const;
  static ElementZmodP zeroStatic();
  ElementZmodP one() const;
  bool operator>(const ElementZmodP& other) const;
  class Test {
  public:
    static bool all();
    static bool basicOperations();
    static bool scale();
  };
};

class PolynomialFactorizationCantorZassenhaus {
public:
  ElementZmodP one;
  int degree;
  int degreeUnknownFactor;
  static const int maximumtotalDegree;
  PolynomialFactorization<ElementZmodP, PolynomialFactorizationCantorZassenhaus>* output;
  PolynomialModuloPolynomial<ElementZmodP> baseLetter;
  PolynomialModuloPolynomial<ElementZmodP> oneQuotientRing;

  Polynomial<ElementZmodP> current;
  List<Polynomial<ElementZmodP> > factorCandidatesPreviousRuns;
  bool oneFactor(
    std::stringstream* comments,
    std::stringstream* commentsOnFailure
  );
  bool oneFactorGo(std::stringstream* comments, std::stringstream* commentsOnFailure);
  bool hasFactorsOfDifferentDegree(std::stringstream* comments);
  PolynomialFactorizationCantorZassenhaus();
};

#endif
