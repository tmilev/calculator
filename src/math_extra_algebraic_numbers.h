// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader1_3_h_already_included
#define vpfHeader1_3_h_already_included

#include "math_general.h"

class AlgebraicClosureRationals;
class AlgebraicNumber {
  friend std::ostream& operator<<(std::ostream& output, const AlgebraicNumber& number) {
    output << number.toString();
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
    this->operator=(other);
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
  void makeZero();
  static AlgebraicNumber zeroStatic();
  static AlgebraicNumber oneStatic();
  AlgebraicNumber zero() const;
  AlgebraicNumber one() const;

  bool isExpressedViaLatestBasis() const;
  void expressViaLatestBasis();
  bool assignCosRationalTimesPi(const Rational& input, AlgebraicClosureRationals& inputOwner);
  bool assignSinRationalTimesPi(const Rational& input, AlgebraicClosureRationals& inputOwner) {
    Rational half(1, 2);
    return this->assignCosRationalTimesPi(half - input, inputOwner);
  }
  // For the purposes fo rescaling rational functions with algebraic number coefficients.
  AlgebraicNumber getNumerator() const;
  // For the purposes fo rescaling rational functions with algebraic number coefficients.
  AlgebraicNumber getDenominator() const;
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
    Rational rationalPart;
    if (this->isRational(&rationalPart)) {
      return rationalPart.isPositive();
    }
    return false;
  }
  bool isRational(Rational* whichRational = nullptr) const;
  bool isNegative() const {
    Rational rationalValue;
    if (this->isRational(&rationalValue)) {
      return rationalValue.isNegative();
    }
    return false;
  }
  bool isEqualToZero() const;
  bool isEqualToOne() const {
    return (*this) == 1;
  }
  void operator=(const AlgebraicNumber& other) {
    this->basisIndex = other.basisIndex;
    this->owner = other.owner;
    this->element = other.element;
  }
  void operator=(const Rational& other);
  void operator=(int other) {
    *this = Rational(other);
  }
  bool constructFromMinimalPolynomial(
    const Polynomial<AlgebraicNumber>& polynomial,
    AlgebraicClosureRationals& inputOwner,
    std::stringstream* commentsOnFailure
  );
  bool constructFromMinimalPolynomial(
    const Polynomial<Rational>& polynomial,
    AlgebraicClosureRationals& inputOwner,
    std::stringstream* commentsOnFailure
  );
  bool assignRationalQuadraticRadical(
    const Rational& input,
    AlgebraicClosureRationals& inputOwner,
    std::stringstream* commentsOnFailure
  );
  void assignRational(const Rational& input, AlgebraicClosureRationals* inputOwner);
  void squareRootDefault(std::stringstream* commentsOnError);
  bool isSmallInteger(int* whichInteger) const {
    Rational rational;
    if (!this->isRational(&rational)) {
      return false;
    }
    return rational.isSmallInteger(whichInteger);
  }
  bool isInteger(LargeInteger* whichInteger) const {
    Rational rational;
    if (!this->isRational(&rational)) {
      return false;
    }
    return rational.isInteger(whichInteger);
  }
  bool radicalMeDefault(int radical, std::stringstream* commentsOnError);
  void invert();
  bool evaluatesToDouble(double* outputWhichDouble) const;
  void operator/=(const AlgebraicNumber& other);
  bool operator==(const AlgebraicNumber& other) const;
  bool operator==(const Rational& other) const;
  bool operator==(int other) const;
  bool operator!= (const AlgebraicNumber& other) const;
  bool operator!= (int other) const;
  void negate();
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
  std::string toString(FormatExpressions* format = nullptr) const;
  std::string toStringNonInjected(FormatExpressions* format = nullptr) const;
  class Test{
  public:
    static bool all();
    static bool constantValues();
    static bool hashFunction();
  };
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
  Vectors<Rational> generatingElementPowersBasis;

  bool flagIsQuadraticRadicalExtensionRationals;
  HashedList<LargeInteger> quadraticRadicals;
  List<std::string> displayNamesBasisElements;
  void injectOldBases(const MatrixTensor<Rational>* injectionNullForIdentity);
  void appendAdditiveEiBasis();
  void assignDefaultBasisDisplayNames();
  void reset();
  bool checkConsistency() const;
  AlgebraicClosureRationals() {
    this->reset();
  }
  bool mergeRadicals(const List<LargeInteger>& radicals);
  bool chooseGeneratingElement(int attemptsLimitZeroForNone, std::stringstream* commentsOnFailure);
  bool reduceMe(std::stringstream* commentsOnFailure);
  void contractBasesIfRedundant(
    AlgebraicClosureRationals& previousCopy,
    AlgebraicNumber* outputImageGenerator
  );
  void computeDisplayStringsFromRadicals();
  bool getRadicalSelectionFromIndex(int inputIndex, Selection& selection);
  int getDimensionOverRationals() const;
  static int getIndexFromRadicalSelection(const Selection& selection);
  void getMultiplicativeOperatorFromRadicalSelection(
    const Selection& selection, MatrixTensor<Rational>& output
  );
  void getMultiplicationBy(const AlgebraicNumber& input, MatrixTensor<Rational>& output);
  void getAdditionTo(const AlgebraicNumber& input, VectorSparse<Rational>& output);
  void convertPolynomialDependingOneVariableToPolynomialDependingOnFirstVariableNoFail(
    const Polynomial<AlgebraicNumber>& input, Polynomial<AlgebraicNumber>& output
  );
  bool adjoinRootMinimalPolynomial(
    const Polynomial<AlgebraicNumber>& polynomial,
    AlgebraicNumber& outputRoot,
    std::stringstream* commentsOnFailure
  );
  bool adjoinRootQuadraticPolynomialToQuadraticRadicalExtension(
    const Polynomial<AlgebraicNumber>& polynomial,
    AlgebraicNumber& outputRoot,
    std::stringstream* commentsOnFailure
  );
  std::string toStringQuadraticRadical(FormatExpressions* format = nullptr) const;
  std::string toString(FormatExpressions* format = nullptr) const;
  std::string toStringFull(FormatExpressions* format = nullptr) const;
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
  bool isEqualToOne() const {
    return this->value.isEqualToOne();
  }
  // Returns the number by which the vector was multiplied.
  static ElementZmodP scaleNormalizeIndex(
    List<ElementZmodP>& toBeScaled, int indexNonZeroElement
  );
  // Required by RationalFunction when scaling simultaneously
  // numerator and denominator.
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
  void negate();
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
  std::string toString(FormatExpressions* format = nullptr) const;
  std::string toStringModP() const;
  static std::string toStringModP(const LargeIntegerUnsigned& modulus);
  void operator=(const LargeInteger& other);
  bool assignRational(const Rational& other);
  void operator=(const Rational& other);
  bool operator/=(const ElementZmodP& den);
  bool operator/=(const LargeInteger& den);
  bool needsParenthesisForMultiplication(FormatExpressions* format = nullptr) const;
  static void convertModuloIntegerAfterScalingToIntegral(
    const Polynomial<Rational>& input,
    Polynomial<ElementZmodP>& output,
    const LargeIntegerUnsigned& newModulo
  );
  static void convertPolynomialModularToPolynomialIntegral(
    const Polynomial<ElementZmodP>& input,
    Polynomial<Rational>& output,
    bool useNegatives
  );
  // Convert modular polynomial to another modular polynomial by copying all values
  // using the new modulus.
  static void convertLiftPolynomialModular(
    const Polynomial<ElementZmodP>& input,
    Polynomial<ElementZmodP>& output,
    const LargeIntegerUnsigned& newModulus
  );
  // Format a polynomial with modular coefficients.
  // The modulo information will not be repeated for all coefficients, but will be put
  // at the end of the expression.
  std::string toStringPolynomial(const Polynomial<ElementZmodP>& input, FormatExpressions* format) const;
  ElementZmodP zero() const;
  ElementZmodP one() const;
  static ElementZmodP zeroStatic();
  static ElementZmodP oneStatic();
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
  static const int maximumDegreeDefault = 512;
  ElementZmodP one;
  int degree;
  int degreeUnknownFactor;
  PolynomialFactorizationUnivariate<ElementZmodP, PolynomialFactorizationCantorZassenhaus>* output;
  PolynomialModuloPolynomial<ElementZmodP> baseLetter;
  PolynomialModuloPolynomial<ElementZmodP> oneQuotientRing;

  Polynomial<ElementZmodP> current;
  List<Polynomial<ElementZmodP> > factorCandidatesPreviousRuns;
  bool oneFactor(
    std::stringstream* comments,
    std::stringstream* commentsOnFailure
  );
  // Input is either irreuducible polynomial of prime degree, or a product
  // of linear polynomials.
  bool handlePrimeDegreeSeparatedFactor(Polynomial<ElementZmodP>& input);
  bool oneFactorGo(std::stringstream* comments, std::stringstream* commentsOnFailure);
  bool hasFactorsOfDifferentDegree(std::stringstream* comments);
  bool oneFactorProbabilityHalf(
    unsigned int constant,
    std::stringstream* comments,
    std::stringstream* commentsOnFailure
  );
  bool divisorFromCandidate(
    const Polynomial<ElementZmodP>& candidate,
    const std::string& candidateDisplayName,
    std::stringstream* comments
  );
  PolynomialFactorizationCantorZassenhaus();
  static std::string name() {
    return "Cantor-Zassenhaus";
  }
};

class PolynomialFactorizationFiniteFields {
public:
  static const int maximumDegreeDefault = 100;
  PolynomialFactorizationUnivariate<Rational, PolynomialFactorizationFiniteFields>* output;
  Polynomial<Rational> current;
  Polynomial<ElementZmodP> modularization;
  ElementZmodP oneModular;
  LargeIntegerUnsigned modulusHenselLift;
  LargeInteger leadingCoefficient;
  LargeInteger largestCoefficient;
  LargeInteger upperBoundAbsoluteValueRoot;
  LargeInteger coefficientBound;
  ElementZmodP scaleProductLift;
  int degree;
  int factorsLiftedTries;
  int maximumFactorsLiftedTries;
  FormatExpressions format;
  List<Polynomial<ElementZmodP> > factorsOverPrime;
  List<Polynomial<ElementZmodP> > factorsLifted;
  Polynomial<ElementZmodP> desiredLift;
  Matrix<ElementZmodP> sylvesterMatrix;
  Matrix<ElementZmodP> sylvesterMatrixInverted;
  bool factorizationFromHenselLift(
    std::stringstream* comments, std::stringstream* commentsOnFailure
  );
  bool factorizationFromHenselLiftOnce(
    std::stringstream* comments, std::stringstream* commentsOnFailure
  );
  bool tryFactor(SelectionFixedRank& selection);
  void henselLift(std::stringstream* comments);
  void henselLiftOnce(
    const LargeIntegerUnsigned& oldModulus,
    std::stringstream* comments
  );
  bool oneFactor(
    std::stringstream* comments,
    std::stringstream* commentsOnFailure
  );
  void computeCoefficientBounds();
  bool oneFactorFromModularization(
    std::stringstream* comments,
    std::stringstream* commentsOnFailure
  );
  static std::string name() {
    return "finite field factorization";
  }
  PolynomialFactorizationFiniteFields();
};
#endif
