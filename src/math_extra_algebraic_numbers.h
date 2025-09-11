#ifndef header_math_extra_algebraic_numbers_ALREADY_INCLUDED
#define header_math_extra_algebraic_numbers_ALREADY_INCLUDED

#include "math_extra_polynomial_factorization.h"
#include "math_general.h"
#include "math_modular_arithmetic.h"

class Calculator;
class AlgebraicClosureRationals;

class AlgebraicNumber {
  friend std::ostream& operator<<(
    std::ostream& output, const AlgebraicNumber& number
  ) {
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
  AlgebraicNumber(const Rational& other):
  owner(nullptr),
  basisIndex(0),
  flagDeallocated(false) {
    this->operator=(other);
  }
  AlgebraicNumber(int other):
  owner(nullptr),
  basisIndex(0),
  flagDeallocated(false) {
    this->operator=(Rational(other));
  }
  AlgebraicNumber(const AlgebraicNumber& other):
  owner(nullptr),
  basisIndex(0),
  flagDeallocated(false) {
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
  bool assignCosRationalTimesPi(
    const Rational& input, AlgebraicClosureRationals& inputOwner
  );
  bool assignSinRationalTimesPi(
    const Rational& input, AlgebraicClosureRationals& inputOwner
  ) {
    Rational half(1, 2);
    return this->assignCosRationalTimesPi(half - input, inputOwner);
  }
  // For the purposes fo rescaling rational functions with algebraic number
  // coefficients.
  AlgebraicNumber getNumerator() const;
  // For the purposes fo rescaling rational functions with algebraic number
  // coefficients.
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
  // Whether the algebraic number is guaranteed to be real and negative.
  // When it returns true, the number is known to be real and negative.
  // When it returns false, the number may or may not be real.
  bool isNegativeRealGuaranteed() const;
  bool isRealGuaranteed(double* outputApproximateRealValue) const;
  bool isEqualToZero() const;
  bool isEqualToOne() const {
    return (*this) == 1;
  }
  void operator=(const AlgebraicNumber& other);
  void operator=(const Rational& other);
  void operator=(int other);
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
  void assignRational(
    const Rational& input, AlgebraicClosureRationals* inputOwner
  );
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
  bool evaluatesToComplex(Complex<double>* outputApproximateValue) const;
  bool evaluatesToDouble(double* outputWhichDouble) const;
  void operator/=(const AlgebraicNumber& other);
  bool operator==(const AlgebraicNumber& other) const;
  bool operator==(const Rational& other) const;
  bool operator==(int other) const;
  bool operator!=(const AlgebraicNumber& other) const;
  bool operator!=(int other) const;
  void negate();
  void operator=(const Polynomial<AlgebraicNumber>& other);
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
  void operator*=(int other) {
    this->operator*=(Rational(other));
  }
  bool operator>(const AlgebraicNumber& other) const;
  std::string toString(FormatExpressions* format = nullptr) const;
  std::string toMathML(
    FormatExpressions* format = nullptr,
    MathExpressionProperties* outputProperties = nullptr
  ) const;
  std::string toStringNonInjected(FormatExpressions* format = nullptr) const;
  class Test {
  public:
    // Constructs an algebraic number from string.
    // Owned by an instance of the Calculator.
    static bool fromString(
      const std::string& input, Calculator& inputOwner, AlgebraicNumber& output
    );
    // Same as fromString but will crash if the input
    // cannot be parsed.
    static AlgebraicNumber fromStringWithoutFailure(
      const std::string& input, Calculator& inputOwner
    );
    static bool all();
    static bool constantValues();
    static bool evaluatesToComplex();
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
  // Injections of earlier rational number bases into the latest basis.
  // The [i][j]-th element refers to the image basis element e_{i, j},
  // i.e., the j^th basis element of the i^th [extension A_i].
  // More precisely, the [i][j]-th element of the map below describes the image
  // of e_{i, j} when injected down to the latest extension A_n, with
  // coordinates relative to the latest basis e_{n, 1}, ..., e_{n, n}.
  List<List<VectorSparse<Rational> > > basisInjections;
  MatrixTensor<Rational> generatingElementTensorForm;
  Matrix<Rational> generatingElementMatrixForm;
  AlgebraicNumber generatingElement;
  Vectors<Rational> generatingElementPowersBasis;
  bool flagIsQuadraticRadicalExtensionRationals;
  HashedList<LargeInteger> quadraticRadicals;
  List<LargeInteger> quadraticRadicalsCorrespondingToBasisElements;
  List<VariableLetter> displayNamesBasisElements;
  void injectOldBases(const MatrixTensor<Rational>* injectionNullForIdentity);
  void appendAdditiveEiBasis();
  void assignDefaultBasisDisplayNames();
  void reset();
  bool checkElement(const AlgebraicNumber& input) const;
  bool checkConsistency() const;
  AlgebraicClosureRationals() {
    this->reset();
  }
  bool mergeRadicals(const List<LargeInteger>& radicals);
  bool chooseGeneratingElement(
    int attemptsLimitZeroForNone, std::stringstream* commentsOnFailure
  );
  bool reduceMe(std::stringstream* commentsOnFailure);
  void contractBasesIfRedundant(
    AlgebraicClosureRationals& previousCopy,
    AlgebraicNumber* outputImageGenerator
  );
  void computeQuadraticRadicals();
  bool getRadicalSelectionFromIndex(int inputIndex, Selection& selection);
  int getDimensionOverRationals() const;
  static int getIndexFromRadicalSelection(const Selection& selection);
  void getMultiplicativeOperatorFromRadicalSelection(
    const Selection& selection, MatrixTensor<Rational>& output
  );
  void getMultiplicationBy(
    const AlgebraicNumber& input, MatrixTensor<Rational>& output
  );
  void getAdditionTo(
    const AlgebraicNumber& input, VectorSparse<Rational>& output
  );
  void convertPolynomialOneVariableToPolynomialFirstVariable(
    const Polynomial<AlgebraicNumber>& input,
    Polynomial<AlgebraicNumber>& output
  );
  bool adjoinRootMinimalRationalPolynomial(
    const Polynomial<Rational>& polynomial,
    AlgebraicNumber& outputRoot,
    std::stringstream* commentsOnFailure
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
  std::string toStringQuadraticRadical(FormatExpressions* format = nullptr)
  const;
  std::string toMathMLQuadraticRadical(FormatExpressions* format = nullptr)
  const;
  std::string toString(FormatExpressions* format = nullptr) const;
  std::string toMathML(FormatExpressions* format = nullptr) const;
  std::string toMathMLFinal(FormatExpressions* format = nullptr) const;
  std::string toStringFull(FormatExpressions* format = nullptr) const;
  AlgebraicNumber one();
  AlgebraicNumber zero();
  AlgebraicNumber imaginaryUnit();
  AlgebraicNumber fromRational(const Rational& input);
};

template <
  class PolynomialModuloPolynomialImplementation,
  class PolynomialImplementation,
  class PolynomialModulusImplementation
>
class PolynomialFactorizationCantorZassenhaus {
public:
  static const int maximumDegreeDefault = 512;
  ElementZmodP one;
  int degree;
  int degreeUnknownFactor;
  PolynomialFactorizationUnivariate<ElementZmodP>* output;
  PolynomialModuloPolynomialImplementation baseLetter;
  PolynomialModuloPolynomialImplementation oneQuotientRing;
  // The modular polynomial currently factored.
  PolynomialImplementation current;
  // A conversion of the current polynomial to a
  // data structure that plays the role of Q in the
  // expression a mod Q. In the case of
  // PolynomialUnivariateModular implementation of our polynomial,
  // the modulus will be implemented as
  // the data structure PolynomialUnivariateModularAsModulus,
  // which contains precomputed computational shortcuts
  // to quickly reduce mod Q.
  PolynomialModulusImplementation modulus;
  List<Polynomial<ElementZmodP> > factorCandidatesPreviousRuns;
  void initialize(const PolynomialImplementation& modulusInitializer);
  void initializeOneFactorComputation();
  bool oneFactor(
    std::stringstream* comments, std::stringstream* commentsOnFailure
  );
  // Input is either irreuducible polynomial of prime degree, or a product
  // of linear polynomials.
  bool handlePrimeDegreeSeparatedFactor(PolynomialImplementation& input);
  bool oneFactorGo(
    std::stringstream* comments, std::stringstream* commentsOnFailure
  );
  bool hasFactorsOfDifferentDegree(std::stringstream* comments);
  bool handleFactorsOfSameDegree(
    std::stringstream* comments, std::stringstream* commentsOnFailure
  );
  bool oneFactorProbabilityHalf(
    unsigned int constant,
    std::stringstream* comments,
    std::stringstream* commentsOnFailure
  );
  void computeStartingPolynomial(
    unsigned int constant, PolynomialModuloPolynomialImplementation& output
  );
  bool divisorFromCandidate(
    const PolynomialImplementation& candidate,
    const std::string& candidateDisplayName,
    std::stringstream* comments
  );
  bool checkInitialization() const;
  PolynomialFactorizationCantorZassenhaus();
  static std::string name() {
    return "Cantor-Zassenhaus";
  }
  class Test {
  public:
    static bool all();
    static bool constructStartingPolynomial();
    static bool testOneStartingPolynomial(
      int modulus, int constant, int currentDegree, const std::string& expected
    );
    static bool testOnce(
      int modulus, const std::string& input, const std::string& expected
    );
  };
};

class PolynomialFactorizationFiniteFields {
public:
  static const int maximumDegreeDefault = 100;
  PolynomialFactorizationUnivariate<Rational>* output;
  Polynomial<Rational> current;
  Polynomial<ElementZmodP> modularization;
  IntegerModulusSmall smallModulus;
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
  Polynomial<ElementZmodP> desiredLiftWithoutRescaling;
  Matrix<ElementZmodP> sylvesterMatrix;
  Matrix<ElementZmodP> sylvesterMatrixInverted;
  List<std::string> latexExplainers;
  List<std::string> factorNames;
  int64_t millisecondsCantorZassenhaus;
  int64_t millisecondsSquareFree;
  int64_t millisecondsLift;
  int64_t millisecondsFactorizationFromLift;
  int64_t millisecondsTotal;
  int numberOfRunsOfFactor;
  std::string toLatexLiftedFactors();
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
    int stepCount,
    std::stringstream* comments
  );
  // Same as factor.
  // The name oneFactor is
  // required to allow use of this class with
  // the PolynomialFactorizationUnivariate template,
  // as shown in the example below.
  // PolynomialFactorizationUnivariate<
  //   Rational,
  //   PolynomialFactorizationFiniteFields
  // > factorization;
  // factorization.factor();
  //
  // There's no computational penalty for using
  // factorization.factor() as above vs. using the
  // naked factor() method below, so please use this
  // class through the wrapper
  // PolynomialFactorizationUnivariate.
  // The wrapper has convenience functions which are
  // shared with for use with other factorization methods.
  // The nature of the Cantor-Zassenhaus algorithm is
  // such that finding one factor is about as complicated
  // as finding all factors, which is not assumed to
  // be the case for all methods for factorization.
  bool oneFactor(
    std::stringstream* comments, std::stringstream* commentsOnFailure
  ) {
    return this->factorWithTiming(comments, commentsOnFailure);
  }
  // Factors the polynomial fully; same as factor but in addition
  // records factorization speed information.
  bool factorWithTiming(
    std::stringstream* comments, std::stringstream* commentsOnFailure
  );
  // Factors the polynomial fully.
  bool factor(
    std::stringstream* comments, std::stringstream* commentsOnFailure
  );
  bool hasSquareFactor(
    std::stringstream* comments, std::stringstream* commentsOnFailure
  );
  // An elementary method for computing coefficient
  // bounds of the individual factors.
  // To be replaced by the next method.
  // I was trying to find proof for the Gelfond's inequality below,
  // and was using the present function as a fallback
  // until I am sure the Gelfond inequality holds.
  void computeCoefficientBoundsElementary();
  // A method for computing tighter coefficient bounds.
  // Drop in replacement for the previous method.
  // Reference (discovered recently; please read carefully and
  // make sure it's correct):
  // K. Mahler, On some inequalities for polynomials
  // in several variables, Journal London Mathematical Society 37,
  // 1962, pages 341-344.
  // Notation: \prod f_i = f, polynomials in one variable
  // (the article allows multivariable polynomials as well).
  // Let H_i = absolute value of largest coefficient of f_i.
  // Let H = absolute value of largest coefficient of f.
  // Then inequality (II), taken for one-variable polynomials, becomes:
  // \prod H_i <= 2^{\deg f-1} sqrt(n+1) H
  // Each of the H_i's can be 1, so in the worst case scenario, the
  // inequality becomes
  // H_i <= 2^{\deg f-1} sqrt(n+1) H.
  // To avoid computing with square roots,
  // we strengthen the inequality by
  // H_i < 2^{\deg f-1} (n+1) H.
  // In view of the preceding discussion,
  // the function below returns the quantity 2^{\deg f-1} (n+1) H.
  void computeCoefficientBoundsGelfond();
  bool oneFactorFromModularization(
    std::stringstream* comments, std::stringstream* commentsOnFailure
  );
  static std::string name() {
    return "finite field factorization";
  }
  PolynomialFactorizationFiniteFields();
  class Test {
  public:
    class TestCase {
    public:
      std::string toBeFactored;
      std::string desiredFactorization;
      Polynomial<Rational>::Test parser;
      bool run();
    };

    static bool all();
    static bool test(
      const std::string& toFactor, const std::string& desiredResult
    );
    static bool gelfondBound(
      const std::string& inputPolynomial, const std::string& desiredBound
    );
  };
};

// A class that (attempts to) determine whether the polynomial has
// quadratic radical roots and find them if they exist.
// As of writing, the algorithms used allow for failure.
class PolynomialQuadraticRootFinder {
  AlgebraicClosureRationals* algebraicClosure;
  bool addRootsOfLinearFactor(const Polynomial<AlgebraicNumber>& factor);
  bool addRootsOfQuadraticFactor(const Polynomial<AlgebraicNumber>& factor);
public:
  Polynomial<AlgebraicNumber> polynomial;
  PolynomialFactorizationUnivariate<AlgebraicNumber> factorization;
  List<AlgebraicNumber> roots;
  // A list of suspected polynomial roots.
  // The numbers must be roots of the polynomial.
  // We expect to have a mathematical reason for expecting these
  // to be roots. An example of when that can happen is when
  // the polynomial was obtained as minimal polynomial of a matrix
  // for which we happen to know an eigenvalue.
  List<AlgebraicNumber> rootHints;
  PolynomialQuadraticRootFinder();
  void initialize(AlgebraicClosureRationals* inputAlgebraicClosure);
  bool findRoots(Polynomial<AlgebraicNumber>& input);
  bool factorInput();
  bool factorRationalInput(const Polynomial<Rational>& input);
  bool hasRationalPolynomial(Polynomial<Rational>& whichPolynomial);
  std::string toString(FormatExpressions* format = nullptr) const;
};

// Attemtps to find a sufficiently simple factorization of the
// polynomial over a quadratic extension of the current
// maximal field held in the algebraicClosure.
class PolynomialFactorizationSimpleQuadraticRational {
public:
  List<Rational> expectedRationalRoots;
};

// Finds eigenvalues of a matrix.
// Currently only works only for special cases.
// The most powerful supported case is rational matrix with
// the eigenvalues that lie in a
// quadratic radical extension of the rationals.
// Some other special cases work too, this is work in progress.
// With sufficient algorithmic work, this could be made to work
// in full generality.
class MatrixEigenvalueFinder {
  AlgebraicClosureRationals* algebraicClosure;
  PolynomialQuadraticRootFinder eigenvalueFinder;
public:
  Matrix<AlgebraicNumber> matrix;
  Polynomial<AlgebraicNumber> characteristicPolynomial;
  HashedList<AlgebraicNumber> eigenValuesWithoutMultiplicity;
  List<Vectors<AlgebraicNumber> > eigenvectors;
  MatrixEigenvalueFinder();
  void initialize(AlgebraicClosureRationals* inputAlgebraicClosure);
  bool findEigenValuesAndEigenspaces(
    const Matrix<Rational>& input, std::stringstream* commentsOnFailure
  );
  bool findEigenValuesAndEigenspaces(
    const Matrix<AlgebraicNumber>& input, std::stringstream* commentsOnFailure
  );
  int numberOfEigenVectors() const;
  void eigenVectorsFromEigenvalue(
    const AlgebraicNumber& eigenValue, Vectors<AlgebraicNumber>& output
  );
  void checkDiagonalElementForEigenvalueCandidacy(int index);
  const PolynomialFactorizationUnivariate<AlgebraicNumber>&
  factorizationMinimalPolynomial() const;
  void extractPossibleEigenvalues();
};

#endif // header_math_extra_algebraic_numbers_ALREADY_INCLUDED
