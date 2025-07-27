#ifndef header_math_polynomials_ALREADY_INCLUDED
#define header_math_polynomials_ALREADY_INCLUDED

#include "general_logging_global_variables.h"
#include "general_strings.h"
#include "math_general.h"
#include "math_large_integers.h"
#include "math_subsets_selections.h"
#include "math_vectors.h"

class ProgressReport;

class MonomialPolynomial {
private:
  // monbody contains the exponents of the variables.
  // IMPORTANT. The monBody of a monomial may not be unique.
  // Two monomials whose non-zero entries coincide
  // (but otherwise one monomial might have more entries filled with zeroes)
  // are considered to be equal.
  // Therefore special attention must be paid when performing operations with
  // MonomialPolynomial's, especially with operator== and
  // MonomialPolynomial::hashFunction!
  // Please modify this class in accordance with what was just explained.
  // Note that by the above token I decided to declare operator[] as non-const
  // function and operator() as a const function but returning a copy of the
  // underlying element, rather than a reference to the element.
  //
  // IMPORTANT. The default monomial order, implemented by operator>, is the
  // graded lexicographic
  // last variable strongest order. Other monomial orders are not referred by
  // operator>, but
  // by their explicit names.
  // Note that the LinearCombination::toString method uses the
  // FormatExpressions::monomialOrder
  // to sort monomials when displaying polynomials to the screen.
  List<Rational> monomialBody;
  void setSize(int variableCount);
public:
  struct Order {
    // Lexicographic order, see documentation below.
    static const int lexicographic = 0;
    // Lexicographic with opposite letter order.
    static const int lexicographicOpposite = 1;
    // Graded lexicographic order, ties resolved with lexicographic order.
    static const int gradedLexicographic = 2;
    // Graded reverse order, ties resolved with right-to-left-smaller-wins.
    static const int gradedReverseLexicographic = 3;
  };

  MonomialPolynomial(int letterIndex) {
    this->setVariable(letterIndex, Rational::one());
  }
  MonomialPolynomial(int letterIndex, int power) {
    this->setVariable(letterIndex, power);
  }
  MonomialPolynomial(const MonomialPolynomial& other) {
    *this = other;
  }
  MonomialPolynomial() {}
  MonomialPolynomial(const Vector<Rational>& exponents);
  friend std::ostream& operator<<(
    std::ostream& output, const MonomialPolynomial& monomial
  ) {
    output << monomial.toString();
    return output;
  }
  void multiplyByVariable(int variableIndex, const Rational& variablePower);
  const Rational& operator[](int i) const;
  Rational operator()(int i) const;
  unsigned int hashFunction() const {
    return this->monomialBody.hashFunction();
  }
  bool hasPositiveOrZeroExponents() const;
  bool hasNonNegativeIntegralExponents() const;
  // Warning: hashFunction must return the same result
  // for equal monomials represented by different monBodies.
  // Two such different representation may differ by extra entries filled in
  // with zeroes.
  static unsigned int hashFunction(const MonomialPolynomial& input) {
    unsigned int result = 0;
    int j = 0;
    for (int i = 0; i < input.monomialBody.size; i ++) {
      result +=
      input.monomialBody[i].hashFunction() *
      HashConstants::getConstantIncrementCounter(j);
    }
    return result;
  }
  std::string toString(FormatExpressions* polynomialFormat = nullptr) const;
  void makeFromPowers(const Vector<Rational>& inputMonomialBody);
  void makeOne() {
    this->monomialBody.setSize(0);
  }
  void makeEi(
    int letterIndex, int power = 1, int expectedNumberOfVariables = 0
  );
  void setVariable(int variableIndex, const Rational& power);
  void trimTrailingZeroes();
  bool operator>(const MonomialPolynomial& other) const;
  bool isDivisibleBy(const MonomialPolynomial& other) const;
  int totalDegreeInt() const {
    int result = - 1;
    if (!this->totalDegree().isSmallInteger(&result)) {
      global.fatal
      << "The total degree of monomial must be "
      << "a small integer to call this function. "
      << global.fatal;
    }
    return result;
  }
  Rational totalDegree() const {
    Rational result = 0;
    for (int i = 0; i < this->monomialBody.size; i ++) {
      result += this->monomialBody[i];
    }
    return result;
  }
  void multiplyBy(const MonomialPolynomial& other) {
    this->operator*=(other);
  }
  void divideBy(const MonomialPolynomial& other) {
    this->operator/=(other);
  }
  bool isLinear() const {
    return this->isConstant() || this->isLinearNoConstantTerm();
  }
  bool isLinearNoConstantTerm(int* whichLetter = nullptr) const {
    return this->isOneLetterFirstDegree(whichLetter);
  }
  bool isOneLetterFirstDegree(int* whichLetter = nullptr) const {
    Rational whichDegree;
    if (!this->isOneLetterNonConstant(whichLetter, &whichDegree)) {
      return false;
    }
    return whichDegree == 1;
  }
  bool isOneLetterNonConstant(
    int* whichLetter = nullptr, Rational* whichDegree = nullptr
  ) const {
    int indexBuffer;
    if (whichLetter == nullptr) {
      whichLetter = &indexBuffer;
    }
    *whichLetter = - 1;
    for (int i = 0; i < this->monomialBody.size; i ++) {
      if (this->monomialBody[i] == 0) {
        continue;
      }
      if (whichDegree != nullptr) {
        *whichDegree = this->monomialBody[i];
      }
      if ((*whichLetter) == - 1) {
        *whichLetter = i;
      } else {
        return false;
      }
    }
    return (*whichLetter) != - 1;
  }
  bool checkConsistency() const {
    return true;
  }
  template <class Coefficient>
  bool substitute(
    const List<Polynomial<Coefficient> >& substitution,
    Polynomial<Coefficient>& output,
    const Coefficient& one
  ) const;
  static List<MonomialPolynomial>::Comparator& orderDefault();
  static List<MonomialPolynomial>::Comparator& orderForGreatestCommonDivisor();
  static List<MonomialPolynomial>::Comparator& orderDegreeThenLeftLargerWins();
  // "Graded reverse lexicographic" order.
  // We compare by total degree.
  // If tied by total degree, compare right to left.
  // The first unequal power from the right breaks the tie.
  // The monomial with **smaller** power is declared **larger**.
  static bool greaterThan_totalDegree_rightSmallerWins(
    const MonomialPolynomial& left, const MonomialPolynomial& right
  );
  // "Graded lexicographic" order.
  static bool greaterThan_totalDegree_leftLargerWins(
    const MonomialPolynomial& left, const MonomialPolynomial& right
  );
  // "Lexicographic" order.
  // If computing with n variables, the "lexicographic order"
  // coincides with the infinite-alphabet dictionary of all words with length
  // n,
  // where
  // each letter is given by the variable exponent.
  // In other words, the lexicographic order for x^2 y^0 z^3 is the
  // "lexicographic" order
  // of the triple [2,0,3].
  // This is confusing, as it is completely different from the "dictionary
  // order" in which
  // the monomial x^2 y^0 z^3 = x x z z z would be placed. For example,
  // x^3 y^3 z > x y^2 z^3 because [3, 3, 0] > [1, 2, 3].
  // However, the word formed by the letters in the monomial are
  // "dictionary-ordered"
  // in the opposite order.
  // x x x y y y z <  x y y z z z.
  // This has lead to lots of confusion in previous version, so we have dropped
  // the
  // term "lexicographic" in all places except the end-user facing calculator
  // commands.
  bool greaterThan_leftLargerWins(const MonomialPolynomial& other) const;
  static bool greaterThan_leftLargerWins(
    const MonomialPolynomial& left, const MonomialPolynomial& right
  ) {
    return left.greaterThan_leftLargerWins(right);
  }
  // Lexicographic order but with variables ordered in the opposite direction.
  bool greaterThan_rightLargerWins(const MonomialPolynomial& other) const;
  static bool greaterThan_rightLargerWins(
    const MonomialPolynomial& left, const MonomialPolynomial& right
  ) {
    return left.greaterThan_rightLargerWins(right);
  }
  bool isConstant() const {
    for (int i = 0; i < this->monomialBody.size; i ++) {
      if (!this->monomialBody[i].isEqualToZero()) {
        return false;
      }
    }
    return true;
  }
  int minimalNumberOfVariables() const {
    return this->monomialBody.size;
  }
  // Evaluates the monomial at given points.
  template <class Coefficient>
  void evaluate(const Vector<Coefficient>& input, Coefficient& output) const {
    output = output.one();
    this->evaluateAccumulate(input, output);
  }
  // Same as evaluate but starts off with a given monomial coefficient.
  template <class Coefficient>
  void evaluateAccumulate(
    const Vector<Coefficient>& input, Coefficient& inputOutputCoefficient
  ) const {
    Coefficient converted;
    for (int j = 0; j < this->minimalNumberOfVariables(); j ++) {
      int numberOfCycles = 0;
      if (!this->monomialBody[j].isSmallInteger(&numberOfCycles)) {
        global.fatal
        << "Attempting to evaluate a monomial whose "
        << j + 1
        << "^{th} variable is raised to the power "
        << this->monomialBody[j].toString()
        << ". Raising variables to power is allowed "
        << "only if the power is a small integer. "
        << "If the user has requested such an operation, "
        << "it *must* be intercepted at an earlier level "
        << "(and the user must be informed)."
        << global.fatal;
      }
      bool isPositive = numberOfCycles > 0;
      if (numberOfCycles < 0) {
        numberOfCycles = - numberOfCycles;
      }
      converted = input[j];
      MathRoutines::raiseToPower(
        converted, numberOfCycles, inputOutputCoefficient.one()
      );
      if (!isPositive) {
        converted.invert();
      }
      inputOutputCoefficient *= converted;
    }
  }
  void invert() {
    for (int i = 0; i < this->monomialBody.size; i ++) {
      this->monomialBody[i].negate();
    }
  }
  bool hasSmallIntegralPositivePowers(int* whichtotalDegree) const;
  void raiseToPower(const Rational& power);
  void operator*=(const MonomialPolynomial& other);
  void operator/=(const MonomialPolynomial& other);
  bool operator==(const MonomialPolynomial& other) const;
  template <class Coefficient>
  void operator=(const Vector<Coefficient>& other) {
    this->monomialBody = other;
  }
  void operator=(const MonomialPolynomial& other) {
    this->monomialBody = other.monomialBody;
  }
  class Test {
  public:
    static bool all();
    static bool testMonomialOrdersSatisfyTheDefinitionOne(
      const MonomialPolynomial& mustBeSmaller,
      const MonomialPolynomial& mustBeLarger,
      List<MonomialPolynomial>::Comparator& order
    );
    static bool testMonomialOrdersSatisfyTheDefinition();
  };
};

class MonomialWeylAlgebra {
public:
  MonomialPolynomial polynomialPart;
  MonomialPolynomial differentialPart;
  friend std::ostream& operator<<(
    std::ostream& output, const MonomialWeylAlgebra& monomial
  ) {
    output << monomial.toString();
    return output;
  }
  bool isConstant() const {
    return
    this->polynomialPart.isConstant() &&
    this->differentialPart.isConstant();
  }
  std::string toString(FormatExpressions* format = nullptr) const;
  static unsigned int hashFunction(const MonomialWeylAlgebra& input) {
    return
    input.polynomialPart.hashFunction() +
    input.differentialPart.hashFunction() * HashConstants::constant0;
  }
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  int minimalNumberOfVariables() const {
    return
    MathRoutines::maximum(
      this->polynomialPart.minimalNumberOfVariables(),
      this->differentialPart.minimalNumberOfVariables()
    );
  }
  bool operator==(const MonomialWeylAlgebra& other) const {
    return
    this->polynomialPart == other.polynomialPart &&
    this->differentialPart == other.differentialPart;
  }
  bool operator>(const MonomialWeylAlgebra& other) const {
    if (this->differentialPart > other.differentialPart) {
      return true;
    }
    if (other.differentialPart > this->differentialPart) {
      return false;
    }
    if (this->polynomialPart > other.polynomialPart) {
      return true;
    }
    if (other.polynomialPart > this->polynomialPart) {
      return false;
    }
    return false;
  }
  bool hasNonSmallPositiveIntegerDerivation() const {
    for (
      int i = 0; i < this->differentialPart.minimalNumberOfVariables(); i ++
    ) {
      if (!this->differentialPart(i).isSmallInteger()) {
        return true;
      }
    }
    return false;
  }
  void makeOne() {
    this->polynomialPart.makeOne();
    this->differentialPart.makeOne();
  }
};

template <class Coefficient>
std::iostream& operator<<(
  std::iostream& output, const Polynomial<Coefficient>& input
) {
  output << input.toString();
  return output;
}

class PolynomialOrder {
public:
  List<MonomialPolynomial>::Comparator monomialOrder;
  template <class Coefficient>
  bool compareLeftGreaterThanRight(
    const Polynomial<Coefficient>& left, const Polynomial<Coefficient>& right
  ) const;
};

template <class Coefficient>
class Polynomial: public ElementMonomialAlgebra<
  MonomialPolynomial, Coefficient
> {
private:
public:
  friend std::iostream& operator<<<Coefficient>(
    std::iostream& output, const Polynomial<Coefficient>& input
  );
  Polynomial(int x) {
    this->makeConstant(x);
  }
  Polynomial() {}
  Polynomial(const Polynomial<Coefficient>& other) {
    this->operator=(other);
  }
  unsigned int hashFunction() const {
    return
    this->::LinearCombination<MonomialPolynomial, Coefficient>::hashFunction();
  }
  static unsigned int hashFunction(const Polynomial<Coefficient>& input) {
    return input.hashFunction();
  }
  void assignFloor() const {}
  void assignMinimalPolynomial(const Matrix<Coefficient>& input);
  void assignCharacteristicPolynomial(const Matrix<Coefficient>& input);
  void assignMonomialWithExponent(
    Vector<Rational>& r, const Coefficient& coefficient = 1
  ) {
    this->makeZero();
    this->addMonomial(r, coefficient);
  }
  void constantTerm(Coefficient& output, const Coefficient& ringZero = 0)
  const;
  // Gets the constant term of a polynomial.
  // Pass the coefficient zero as input.
  Coefficient constantTerm(
    const Coefficient& ringZero = Coefficient::zeroStatic()
  ) const {
    Coefficient result;
    this->constantTerm(result, ringZero);
    return result;
  }
  static void getValuesLagrangeInterpolands(
    Vector<Rational>& pointsOfInterpolation,
    Vector<Rational>& pointsOfEvaluation,
    Vectors<Rational>& outputValuesInterpolands
  );
  bool isSquareFree(const Coefficient& one, std::stringstream* comments) const;
  bool isSquareFreeAndUnivariate(
    const Coefficient& one, std::stringstream* comments
  ) const;
  // Computes the derivative polynomial d/dx p; crashes if the polynomial
  // is of two or more variables. Will also crash if the polynomial
  // has monomial powers that are too large.
  void derivative(Polynomial<Coefficient>& output) const;
  // Given polynomial p, returns d/dx_1 p dx_1 + ... + d/dx_n p dx_n.
  // The differentials dx_1, ..., dx_n are introduced as additional variables.
  bool differential(
    Polynomial<Coefficient>& output, std::stringstream* comments
  ) const;
  // Similar to the preceding function but returns a vector.
  bool differential(
    Vector<Polynomial<Coefficient> >& output, std::stringstream* comments
  ) const;
  void interpolate(
    const Vector<Coefficient>& points,
    const Vector<Coefficient>& valuesAtPoints
  );
  bool findOneVariableRationalRoots(List<Rational>& output);
  Coefficient discriminant();
  void coefficientInFrontOfLinearTermVariableIndex(
    int index, Coefficient& output
  );
  Coefficient coefficientInFrontOfLinearTermVariableIndex(int index) {
    Coefficient result;
    this->coefficientInFrontOfLinearTermVariableIndex(index, result);
    return result;
  }
  void makeMonomial(
    int letterIndex,
    const Rational& power,
    const Coefficient& inputCoefficient = 1
  );
  void makeDegreeOne(
    int numberOfVariables, int nonZeroIndex, const Coefficient& coefficient
  );
  void makeDegreeOne(
    int numberOfVariables,
    int nonZeroIndex1,
    int nonZeroIndex2,
    const Coefficient& coefficient1,
    const Coefficient& coefficient2
  );
  void makeDegreeOne(
    int numberOfVariables,
    int nonZeroIndex,
    const Coefficient& coefficient1,
    const Coefficient& constantTerm
  );
  void makeLinearNoConstant(const Vector<Rational>& inputCoefficients);
  void makeLinearWithConstantTerm(
    const Vector<Rational>& inputLastCoordinateConstantTerm
  );
  void makePolynomialFromDirectionAndNormal(
    Vector<Coefficient>& direction,
    Vector<Coefficient>& normal,
    Coefficient& correction
  );
  // Constructs a linear system from a system of linear polynomials equal to
  // zero.
  // Returns false if the polynomials are not linear.
  static bool getLinearSystemFromLinearPolynomials(
    const List<Polynomial<Coefficient> >& linearPolynomials,
    Matrix<Coefficient>& outputHomogenousPart,
    Matrix<Coefficient>& outputConstantTerms
  );
  bool isOneVariablePolynomial(int* whichVariable = nullptr) const;
  bool isOneVariableNonConstantPolynomial(int* whichVariable = nullptr) const;
  Polynomial<Coefficient> one() const;
  static Polynomial<Coefficient> zero();
  Rational rationalValue();
  void makeDeterminantFromSquareMatrix(
    const Matrix<Polynomial<Coefficient> >& matrix
  );
  void makeConstant(const Coefficient& constant) {
    this->makeZero();
    MonomialPolynomial one;
    one.makeOne();
    this->addMonomial(one, constant);
  }
  void makeOne();
  void polynomialWithPolynomialCoefficient(
    Selection& nonCoefficientVariables,
    Polynomial<Polynomial<Coefficient> >& output
  ) const;
  void polynomialUnivariateWithPolynomialCoefficients(
    int variableIndex, Polynomial<Polynomial<Coefficient> >& output
  ) const;
  // Returns the polynomial coefficient in front of the variable
  // x_{variableIndex}^variablePower.
  void coefficientPolynomialOfXPowerK(
    int variableIndex, int variablePower, Polynomial<Coefficient>& output
  );
  // Multivariable polynomial division with remainder.
  // Can be done using the multi-divisor polynomial division algorithm
  // in GroebnerBasisComputation by passing a single basis element.
  // However, since that data structure is somewhat heavy,
  // we provide an alternative independent implementation here.
  void divideBy(
    const Polynomial<Coefficient>& inputDivisor,
    Polynomial<Coefficient>& outputQuotient,
    Polynomial<Coefficient>& outputRemainder,
    List<MonomialPolynomial>::Comparator* monomialOrder
  ) const;
  void addConstant(const Coefficient& constantTerm) {
    MonomialPolynomial monomial;
    monomial.makeOne();
    this->addMonomial(monomial, constantTerm);
  }
  void shiftVariableIndicesToTheRight(int variableIndexShift);
  void setNumberOfVariablesSubstituteDeletedByOne(int newNumberOfVariables);
  int highestIndexSuchThatHigherIndexVariablesDontParticipate();
  void scaleToPositiveMonomialExponents(
    MonomialPolynomial& outputIWasMultipliedBy
  );
  // Substitutes in the given polynomial and writes the end result
  // in-place.
  bool substitute(
    const PolynomialSubstitution<Coefficient>& substitution,
    const Coefficient& one
  );
  // Substitutes in a polynomial.
  // The n^th polynomial in the substitution list gives the
  // image under the substitution of the n^th variable.
  // If the substitution has fewer variable images than
  // number of participating variables in some monomial,
  // the computation should crash fatally.
  bool substituteWriteOutput(
    const PolynomialSubstitution<Coefficient>& substitution,
    const Coefficient& one,
    Polynomial<Coefficient>& output
  ) const;
  Rational totalDegree() const;
  int totalDegreeInt() const;
  bool isEqualToOne() const;
  bool isMonomialCoefficientOne() const;
  bool isOneLetterFirstDegree(int* whichLetter = nullptr) const {
    if (this->size() != 1) {
      return false;
    }
    return (*this)[0].isOneLetterFirstDegree(whichLetter);
  }
  bool isConstant(Coefficient* whichConstant = nullptr) const;
  bool isNegative() const;
  bool isLinearNoConstantTerm();
  bool isLinear();
  bool isLinearGetRootConstantTermLastCoordinate(
    Vector<Coefficient>& outputRoot
  );
  void raiseToPower(int d, const Coefficient& one);
  bool rootFromLinearPolynomialConstantTermLastVariable(
    Vector<Coefficient>& outputRoot
  );
  Matrix<Coefficient> evaluateUnivariatePolynomial(
    const Matrix<Coefficient>& input
  );
  // <-for univariate polynomials only
  Coefficient evaluate(
    const Vector<Coefficient>& input, const Coefficient& zero = 0
  );
  Coefficient evaluate(const Coefficient& input, const Coefficient& zero = 0) {
    Vector<Coefficient> vectorInput;
    vectorInput.addOnTop(input);
    return this->evaluate(vectorInput, zero);
  }
  bool isProportionalTo(
    const Polynomial<Coefficient>& other,
    Coefficient& outputTimesMeEqualsOther,
    const Coefficient& ringUnit
  ) const;
  int maximumPowerOfVariableIndex(int variableIndex);
  bool operator<=(const Coefficient& other) const;
  bool operator<(const Coefficient& other) const;
  bool operator>(const Polynomial<Coefficient>& other) const {
    return
    this->::LinearCombination<MonomialPolynomial, Coefficient>::operator>(
      other
    );
  }
  bool operator<=(const Polynomial<Coefficient>& other) const;
  static bool isGEQCompareByTopMonomialTotalDegThenLexicographic(
    const Polynomial<Coefficient>& left, const Polynomial<Coefficient>& right
  );
  static bool isGEQCompareByTopMonomialLexicographicLastVarStrongest(
    const Polynomial<Coefficient>& left, const Polynomial<Coefficient>& right
  );
  bool isEqualTo(const Polynomial<Coefficient>& p) const {
    return *this == p;
  }
  Polynomial<Coefficient> operator+(const Polynomial<Coefficient>& other)
  const;
  Polynomial<Coefficient> operator-(const Polynomial<Coefficient>& other)
  const;
  void operator-=(int x);
  void operator-=(const Coefficient& other);
  void operator-=(const Polynomial<Coefficient>& other);
  void operator*=(const MonomialPolynomial& other);
  void operator*=(const Polynomial<Coefficient>& other);
  Polynomial<Coefficient> operator*(const Coefficient& other) const;
  Polynomial<Coefficient> operator*(const Polynomial<Coefficient>& other)
  const;
  Polynomial<Coefficient> operator%(const Polynomial<Coefficient>& other);
  void operator/=(const Polynomial<Coefficient>& other);
  void operator/=(int other);
  void operator/=(const Coefficient& other);
  template <class otherType>
  void operator*=(const otherType& other);
  void operator=(const Polynomial<Coefficient>& other) {
    this->::LinearCombination<MonomialPolynomial, Coefficient>::operator=(
      other
    );
  }
  template <class OtherType>
  void operator=(const Polynomial<OtherType>& other) {
    this->::LinearCombination<MonomialPolynomial, Coefficient>::operator=(
      other
    );
  }
  void operator=(const Coefficient& other);
  void operator=(int other);
  template <class OtherType>
  bool assignOtherType(
    const Polynomial<OtherType>& other,
    CoefficientConverter<OtherType, Coefficient>& converter
  ) {
    return
    this->::LinearCombination<MonomialPolynomial, Coefficient>::assignOtherType
    (other, converter);
  }
  // Returns the coefficient a of a monomial of the form ax^k.
  Coefficient coefficientOfXPowerK(int variableIndex, int variablePower) {
    MonomialPolynomial monomial;
    monomial.setVariable(variableIndex, variablePower);
    return this->getCoefficientOf(monomial);
  }
  // Default implementation of gcd using the Euclidean algorithm;
  // can be sped up particular coefficient types; for example
  // rational gcd can be sped up using modular arithmetic.
  static bool greatestCommonDivisorOneVariableDefault(
    const Polynomial<Coefficient>& left,
    const Polynomial<Coefficient>& right,
    Polynomial<Coefficient>& output,
    std::stringstream* commentsOnFailure
  );
  // The default implementation is overriden for
  // Polynomial<Rational>::greatestCommonDivisorOneVariable.
  static bool greatestCommonDivisorOneVariable(
    const Polynomial<Coefficient>& left,
    const Polynomial<Coefficient>& right,
    Polynomial<Coefficient>& output,
    std::stringstream* commentsOnFailure
  ) {
    return
    greatestCommonDivisorOneVariableDefault(
      left, right, output, commentsOnFailure
    );
  }
  static bool greatestCommonDivisor(
    const Polynomial<Coefficient>& left,
    const Polynomial<Coefficient>& right,
    Polynomial<Coefficient>& output,
    const Coefficient& one,
    std::stringstream* commentsOnFailure
  );
  static bool leastCommonMultipleOneVariable(
    const Polynomial<Coefficient>& left,
    const Polynomial<Coefficient>& right,
    Polynomial<Coefficient>& output,
    std::stringstream* commentsOnFailure
  );
  static bool leastCommonMultiple(
    const Polynomial<Coefficient>& left,
    const Polynomial<Coefficient>& right,
    Polynomial<Coefficient>& output,
    const Coefficient& one,
    std::stringstream* commentsOnFailure
  );
  bool hasSmallIntegralPositivePowers(int* whichTotalDegree) const;
  bool hasNonNegativeIntegralExponents() const;
  static Polynomial<Coefficient> zeroStatic() {
    Polynomial<Coefficient> zero;
    return zero;
  }
  bool checkInitialization() const {
    return true;
  }
  class Test {
  public:
    FormatExpressions format;
    FormatExpressions formatDifferentials;
    static bool all();
    void initialize();
    bool oneLeastCommonMultiple(
      const std::string& left,
      const std::string& right,
      const std::string& expected
    );
    bool leastCommonMultiple();
    bool oneFactorizationKronecker(
      const std::string& input, const std::string& expectedFactors
    );
    bool factorizationKronecker();
    bool oneDifferential(
      const std::string& input, const std::string& expected
    );
    bool differential();
    static Polynomial<Coefficient> fromString(const std::string& input);
    Vector<Polynomial<Coefficient> > fromStringCommonContext(
      const std::string& first, const std::string& second
    );
    Vector<Polynomial<Coefficient> > fromStringCommonContext(
      const List<std::string>& input
    );
    bool fromStringTest();
    bool fromStringCommonContextTest();
  };
};

template < >
bool Polynomial<Rational>::greatestCommonDivisorOneVariable(
  const Polynomial<Rational>& left,
  const Polynomial<Rational>& right,
  Polynomial<Rational>& output,
  std::stringstream* commentsOnFailure
);
template <class Coefficient>
class SylvesterMatrix {
  static void fillSylvesterMatrix(
    const Polynomial<Coefficient>& polynomial,
    int columnOffset,
    Matrix<Coefficient>& output
  );
  // Generalization of sylvester matrix for multiple polynomials.
  // Similar to the Sylvester matrix
  // but with coefficient parallelogram for each polynomial.
  static bool sylvesterMatrixMultiple(
    const List<Polynomial<Coefficient> >& polynomials,
    int dimension,
    Matrix<Coefficient>& output
  );
public:
  // For polynomials A = a_0 x^n + ..., B = b_0 x^m + ...,
  // we define the Sylvester matrix as the matrix:
  // ( a_0  0  0 ...  0  b_0  0  0 ... 0   )
  // ( a_1 a_0 0 ...  0  b_1 b_0 0 ... 0   )
  // (               ...               ... )
  // ( a_n           a_0               b_0 )
  // ( 0             ... b_m               )
  // ( ...           ... ...               )
  // ( 0    0  0 ... a_n  0   0  0 ... b_m ).
  // See https://en.wikipedia.org/wiki/Resultant.
  static bool sylvesterMatrix(
    const Polynomial<Coefficient>& left,
    const Polynomial<Coefficient>& right,
    Matrix<Coefficient>& output,
    std::stringstream* commentsOnFailure
  );
  // Let a_1, ..., a_k be the polynomials whose
  // Sylvester product matrix we want to compute.
  // Let t_j be the product of the a_i's with a_j omitted,
  // i.e., t_j = a_1 ... a_k / a_j.
  // Then we define the Sylvester product matrix to be
  // the sylvester multi-matrix of the t_j's.
  // In the case of k = 2 this coincides with the usual
  // Sylvester matrix (depending on the defintion, that may be
  // with reversed order of the polynomials).
  // The Sylvester product matrix is used when computing
  // Hensel lifts of multiple polynomial products.
  static bool sylvesterMatrixProduct(
    const List<Polynomial<Coefficient> >& polynomials,
    Matrix<Coefficient>& output,
    std::stringstream* commentsOnFailure
  );
};

template < >
bool Polynomial<Rational>::Test::differential();
template < >
bool Polynomial<Rational>::Test::oneDifferential(
  const std::string& input, const std::string& expected
);
template < >
bool Polynomial<Rational>::Test::oneLeastCommonMultiple(
  const std::string& left,
  const std::string& right,
  const std::string& expected
);
template < >
Vector<Polynomial<Rational> > Polynomial<Rational>::Test::
fromStringCommonContext(const std::string& first, const std::string& second);
template < >
Vector<Polynomial<Rational> > Polynomial<Rational>::Test::
fromStringCommonContext(const List<std::string>& input);
template < >
bool Polynomial<Rational>::Test::fromStringCommonContextTest();
template < >
void Polynomial<Rational>::Test::initialize();
template < >
bool Polynomial<Rational>::Test::leastCommonMultiple();
template < >
bool Polynomial<Rational>::Test::all();
template < >
Polynomial<Rational> Polynomial<Rational>::Test::fromString(
  const std::string& input
);
template < >
bool Polynomial<Rational>::Test::fromStringTest();
template < >
bool Polynomial<Rational>::findOneVariableRationalRoots(
  List<Rational>& output
);
class PolynomialConversions {
public:
  template <class PolynomialTemplate, class Coefficient>
  static void convertToPolynomial(
    const PolynomialTemplate& input, Polynomial<Coefficient>& output
  );
  template <class Coefficient>
  static void convertToPolynomial(
    const Polynomial<Coefficient>& input, Polynomial<Coefficient>& output
  ) {
    output = input;
  }
  class Test {
  public:
    static bool all();
    static bool univariateModularToDense();
    static bool oneUnivariateModularToDense(
      int modulus, const std::string& input, const std::string& expected
    );
  };
};

template <class Coefficient>
class PolynomialFactorizationUnivariate {
public:
  Polynomial<Coefficient> original;
  Polynomial<Coefficient> current;
  Coefficient constantFactor;
  List<Polynomial<Coefficient> > reduced;
  List<Polynomial<Coefficient> > nonReduced;
  std::string algorithm;
  int maximumDegree;
  // An integer that measures the amount of
  // computations carried out by the OneFactorFinder algorithm.
  // The integer should be a reasonably good measure of computation
  // and need not be a precise measure of CPU cycles.
  // The details on what measure of computation is stored by
  // the OneFactorFinder here remain unspecified.
  int64_t computations;
  // Upper limit for the number of computations to carry out before giving up.
  // Set to non-positive for no computation limits.
  int64_t maximumComputations;
  FormatExpressions format;
  bool basicChecks(std::stringstream* commentsOnFailure);
  template <class OneFactorFinder>
  bool factor(
    const Polynomial<Coefficient>& input,
    OneFactorFinder& algorithm,
    std::stringstream* comments,
    std::stringstream* commentsOnFailure
  );
  template <class PolynomialTemplate>
  bool accountNonReducedFactorTemplate(const PolynomialTemplate& incoming) {
    Polynomial<Coefficient> converted;
    PolynomialConversions::convertToPolynomial(incoming, converted);
    return this->accountNonReducedFactor(converted);
  }
  bool accountNonReducedFactor(Polynomial<Coefficient>& incoming);
  template <class PolynomialTemplate>
  bool accountReducedFactorTemplate(
    const PolynomialTemplate& incoming, bool accountQuotientAsNonReduced
  ) {
    Polynomial<Coefficient> converted;
    PolynomialConversions::convertToPolynomial(incoming, converted);
    return this->accountReducedFactor(converted, accountQuotientAsNonReduced);
  }
  bool accountReducedFactor(
    Polynomial<Coefficient>& incoming, bool accountQuotientAsNonReduced
  );
  bool checkFactorization() const;
  std::string toStringResult(FormatExpressions* format = nullptr) const;
  PolynomialFactorizationUnivariate();
};

template <class Coefficient>
class PolynomialSubstitution: public List<Polynomial<Coefficient> > {
public:
  // This class is used for carrying out substitutions.
  // The substitution format is:
  // the i^th element denotes the image of x_i,
  // For example, if polynomials is the array
  // x_1+x_2 (polynomial in 3 variables)
  // x_1x_3+2 (polynomial in 3 variables)
  // this means that it can be applied to polynomials in two variables
  // like this: x_1-> x_1+x_2
  // x_2-> (x_1x_3+2)
  // to produce a polynomial in three variables.
  void makeIdentitySubstitution(
    int numberOfVariables, const Coefficient& ringUnit
  );
  void makeIdentityLikeInjectionSubstitution(
    int numberOfStartingVariables,
    int numberOfTargetVariablesMustBeLargerOrEqual,
    const Coefficient& ringUnit
  );
  // In the following function we have that:
  // the format of the linear substitution is:
  // substitution is a matrix whose number of rows minus 1 must equal the #
  // number of
  // target variables and whose number of columns must equal the number of
  // variables in
  // the current polynomial (this->totalVariables).
  // The first row denotes the constant term in the substitution of the
  // respective variable.
  // An element in the x-th row and y-th column
  // is defined as element [x][y].
  void makeExponentSubstitution(Matrix<LargeInteger>& substitution);
  void multiplyByConstant(Coefficient& r);
  int getHighestIndexSuchThatHigherIndexVariablesDontParticipate() {
    int result = - 1;
    for (int i = 0; i < this->size; i ++) {
      result =
      MathRoutines::maximum(
        result,
        this->objects[i].
        getHighestIndexSuchThatHigherIndexVariablesDontParticipate()
      );
    }
    return result;
  }
  void makeZero(int numberOfVariables) {
    for (int i = 0; i < this->size; i ++) {
      this->objects[i].makeZero(numberOfVariables);
    }
  }
  std::string toString(FormatExpressions* format = nullptr) const {
    std::string result;
    this->toString(result, format);
    return result;
  }
  bool operator==(const PolynomialSubstitution& right);
  void makeSubstitutionFromMatrixIntegerAndDenominator(
    Matrix<LargeInteger>& matrix, LargeIntegerUnsigned& denominator
  ) {
    Matrix<Rational> rescaled;
    rescaled.assignMatrixIntegerWithDenominator(matrix, denominator);
    this->makeLinearSubstitutionConstantTermsLastRow(rescaled);
  }
  void toString(std::string& output, FormatExpressions* format) const {
    std::stringstream out;
    output.clear();
    for (int i = 0; i < this->size; i ++) {
      if (format != nullptr) {
        out << format->getPolynomialLetter(i);
      } else {
        out << "x_{" << i + 1 << "}";
      }
      out << "\\mapsto " << this->objects[i].toString(format);
      if (i != this->size - 1) {
        out << ", ";
      }
    }
    out << ": ";
    output = out.str();
  }
  void makeLinearSubstitutionConstantTermsLastRow(Matrix<Coefficient>& matrix);
  // Interprets the polynomials in the substitution as a system of linear
  // polynomial equations
  // and converts them to a linear system.
  // Will crash if the polynomials are not linear.
  void getLinearSystemFromLinearPolynomials(
    Matrix<Coefficient>& outputHomogenousPart,
    Matrix<Coefficient>& outputConstantTerms
  );
};

template <class Coefficient>
class PolynomialDivisionReport {
public:
  HashedList<MonomialPolynomial> allMonomials;
  List<Polynomial<Coefficient> > intermediateRemainders;
  List<List<MonomialPolynomial> > intermediateHighlightedMonomials;
  List<MonomialPolynomial> intermediateHighestMonomialDividedByHighestMonomial;
  List<Coefficient> intermediateCoefficients;
  List<Polynomial<Coefficient> > intermediateSubtractands;
  Polynomial<Coefficient> startingPolynomial;
  int highlightAllMonomialsFinalRemainder;
  int firstIndexLatexSlide;
  List<List<int> > additionalHighlightRemainders;
  List<List<List<int> > > highlightMonomialsQuotients;
  List<List<List<int> > > highlightMonomialsRemainders;
  List<List<List<int> > > highlightMonomialsSubtrahends;
  List<List<List<int> > > highlightMonomialsDivisors;
  List<List<int> > fcAnswerMonomialsQuotients;
  List<List<int> > fcAnswerMonomialsRemainders;
  List<List<int> > fcAnswerMonomialsSubtrahends;
  List<List<int> > fcAnswerMonomialsDivisors;
  List<int> firstNonZeroIndicesPerIntermediateSubtracand;
  List<int> intermediateSelectedDivisors;
  List<int> uncoverAllMonomialsQuotients;
  List<int> uncoverAllMonomialsRemainders;
  List<int> uncoverAllMonomialsSubtrahends;
  List<int> uncoverAllMonomialsDivisors;
  List<int> uncoverMonomialsFinalRemainder;
  List<int> additionalHighlightFinalRemainder;
  std::string divisionLog;
  GroebnerBasisComputation<Coefficient>* owner;
  bool checkInitialization();
  std::string getDivisionStringHtml();
  std::string getDivisionStringLaTeX();
  std::string getDivisionLaTeXSlide();
  std::string getPolynomialStringSpacedMonomialsLaTeX(
    const Polynomial<Coefficient>& polynomial,
    std::string* highlightColor = nullptr,
    List<MonomialPolynomial>* highlightedMonomials = nullptr,
    int* firstNonZeroIndex = nullptr
  );
  std::string getSpacedMonomialsWithHighlightLaTeX(
    const Polynomial<Coefficient>& polynomial,
    List<List<int> >* slidesToHighlightMonomials,
    List<int>* slidesToFcAnswer,
    List<int>* slidesToUncover,
    List<int>* slidesAdditionalHighlight,
    int slidesToUncoverAllMonomials,
    bool useColumnSeparator
  );
  std::string getPolynomialStringSpacedMonomialsHtml(
    const Polynomial<Coefficient>& polynomial,
    const std::string& extraStyle,
    List<MonomialPolynomial>* highlightedMonomials = nullptr
  );
  void computeHighLightsFromRemainder(
    int remainderIndex, int& currentSlideNumber
  );
  PolynomialDivisionReport();
};

template <class Coefficient>
class GroebnerBasisComputation {
public:
  PolynomialOrder polynomialOrder;
  Polynomial<Coefficient> remainderDivision;
  List<Polynomial<Coefficient> > quotients;
  List<Polynomial<Coefficient> > basisCandidates;
  class BasisElement {
  public:
    Polynomial<Coefficient> element;
    MonomialPolynomial leadingMonomial;
    Coefficient leadingCoefficient;
    std::string toString(FormatExpressions* format) const;
  };

  List<BasisElement> basis;
  int numberPolynomialDivisions;
  int numberMonomialOperations;
  int maximumMonomialOperations;
  int maximumPolynomialDivisions;
  int numberOfIntermediateRemainders;
  int numberOfSymmetricDifferenceRounds;
  bool flagFoundNewBasisElements;
  bool flagDoProgressReport;
  bool flagDoLogDivision;
  bool flagStoreQuotients;
  MemorySaving<PolynomialDivisionReport<Coefficient> > divisionReport;
  FormatExpressions format;
  void addBasisElementNoReduction(const Polynomial<Coefficient>& input);
  bool limitsExceeded() const;
  bool addAndReducePolynomials();
  bool addAndReduceOnePolynomial();
  bool addRemainderToBasis();
  bool transformToReducedBasis(List<Polynomial<Coefficient> >& inputOutput);
  bool transformToReducedGroebnerBasis(
    List<Polynomial<Coefficient> >& inputOutput, bool rescaleLeadingMonomials
  );
  void generateSymmetricDifferenceCandidates();
  void generateOneSymmetricDifferenceCandidate(
    GroebnerBasisComputation<Coefficient>::BasisElement& left,
    GroebnerBasisComputation<Coefficient>::BasisElement& right
  );
  void getBasis(List<Polynomial<Coefficient> >& output);
  bool wrapUpGroebnerOnExceedingComputationLimit(
    List<Polynomial<Coefficient> >& output
  );
  bool wrapUpOnGroebnerBasisSuccess(List<Polynomial<Coefficient> >& output);
  GroebnerBasisComputation();
  int minimalNumberOfVariables() const;
  static int getNumberOfEquationsThatWouldBeLinearIfISubstitutedVariable(
    int variableIndex, List<Polynomial<Coefficient> >& input
  );
  // Divides a multivariable polynomial simultaneously by a
  // list of polynomial divisors; the order of the divisors matters.
  void remainderDivisionByBasis(
    const Polynomial<Coefficient>& input,
    Polynomial<Coefficient>& outputRemainder,
    int basisIndexToIgnore = - 1
  );
  // Same as remainderDivisionByBasis but failure due to exceeding
  // computational limits is allowed.
  bool remainderDivisionByBasisFailureAllowed(
    const Polynomial<Coefficient>& input,
    Polynomial<Coefficient>& outputRemainder,
    int basisIndexToIgnore = - 1,
    bool failureAllowed = false
  );
  bool oneDivisonStepWithBasis(
    Polynomial<Coefficient>& currentRemainder,
    Polynomial<Coefficient>& remainderResult,
    int basisIndexToIgnore,
    ProgressReport* report
  );
  void oneDivisonSubStepWithBasis(
    Polynomial<Coefficient>& remainder,
    const MonomialPolynomial& leadingMonomial,
    const Coefficient& leadingCoefficient,
    int index,
    ProgressReport* report
  );
  void checkConsistency();
  void initializeForGroebnerComputation();
  std::string toStringLetterOrder() const;
  std::string toStringPolynomialBasisStatusLong();
  std::string toStringPolynomialBasisStatusShort();
  std::string toStringLimits() const;
  std::string toStringDivision(Polynomial<Coefficient>& toBeDivided);
  std::string toStringStatusGroebnerBasisTransformation();
};

template <class Coefficient>
class ArbitrarySubstitutionsProvider {
public:
  List<Coefficient> arbitrarySubstitutions;
  bool flagChooseSmallestIndexVariableFirst;
  int oneIsFirstWhenRecursionDepthIsMultipleOf;
  Coefficient sampleCoefficient;
  ArbitrarySubstitutionsProvider();
  void(*computeArbitrarySubstitutionsOverride)(
    ArbitrarySubstitutionsProvider& object, int recurstionDepth
  );
  void computeArbitrarySubstitutions(int recursionDepth);
  int preferredVariableForArbitrarySubstitutionProvider(
    Selection& variablesToSolveFor, int bestHeursticCandidate
  );
};

template <class Coefficient>
class PolynomialSystem {
public:
  int numberOfSerreSystemComputations;
  int recursionCounterSerreLikeSystem;
  int numberOfVariablesToSolveForStart;
  int numberOfVariablesToSolveForAfterReduction;
  bool flagTryDirectlySolutionOverAlgebraicClosure;
  bool flagUseMonomialBranchingOptimization;
  bool flagSystemProvenToHaveNoSolution;
  bool flagSystemProvenToHaveSolution;
  bool flagSystemSolvedOverBaseField;
  bool flagUsingAlgebraicClosure;
  // For fields that are dynamically generated such as
  // modular arithmetic elements (ElementZModP), use this element as
  // factory to generate contstants.
  Coefficient sampleCoefficient;
  List<Polynomial<Coefficient> > gaussianEliminatedSystem;
  AlgebraicClosureRationals* algebraicClosure;
  GroebnerBasisComputation<Coefficient> groebner;
  MemorySaving<PolynomialSystem<Coefficient> > computationUsedInRecursiveCalls;
  List<Coefficient> systemSolution;
  Selection solutionsFound;
  List<PolynomialSubstitution<Coefficient> > impliedSubstitutions;
  ArbitrarySubstitutionsProvider<Coefficient> substitutionsProvider;
  PolynomialSystem();
  void solveSerreLikeSystem(List<Polynomial<Coefficient> >& inputSystem);
  std::string toStringCalculatorInputFromSystem(
    const List<Polynomial<Coefficient> >& inputSystem
  ) const;
  bool isImpliedLinearSubstitution(
    const Polynomial<Coefficient>& inputPolynomial,
    PolynomialSubstitution<Coefficient>& outputSubstitution,
    Polynomial<Coefficient>& outputPolynomial
  );
  static bool leftIsBetterSubstitutionThanRight(
    const Polynomial<Coefficient>& left, const Polynomial<Coefficient>& right
  );
  bool isSolutionToPolynomialInOneVariable(
    Polynomial<Coefficient>& polynomial,
    PolynomialSubstitution<Coefficient>& outputSubstitution
  );
  bool findAndApplyImpliedSubstitutions(
    List<Polynomial<Coefficient> >& inputSystem, ProgressReport& progressReport
  );
  bool hasImpliedSubstitutions(
    List<Polynomial<Coefficient> >& inputSystem,
    PolynomialSubstitution<Coefficient>& outputSubstitution
  );
  bool hasSingleMonomialEquation(
    const List<Polynomial<Coefficient> >& inputSystem,
    MonomialPolynomial& outputMonomial
  );
  void setUpRecursiveComputation(PolynomialSystem<Coefficient>& toBeModified);
  void processSolvedSubcaseIfSolvedOrProvenToHaveSolution(
    PolynomialSystem<Coefficient>& potentiallySolvedCase
  );
  void solveWhenSystemHasSingleMonomial(
    List<Polynomial<Coefficient> >& inputOutputSystem,
    const MonomialPolynomial& monomial
  );
  int getPreferredSerreSystemSubstitutionIndex(
    List<Polynomial<Coefficient> >& inputSystem
  );
  void solveSerreLikeSystemRecursively(
    List<Polynomial<Coefficient> >& inputSystem
  );
  void polynomialSystemSolutionSimplificationPhase(
    List<Polynomial<Coefficient> >& inputOutputSystem
  );
  bool gaussianEliminationReturnFalseIfSystemIsContradictory(
    List<Polynomial<Coefficient> >& inputOutputSystem
  );
  // Carries out one polynomial system simplification step.
  bool oneSimplificationStepReturnTrueIfMoreSimplificationNeeded(
    List<Polynomial<Coefficient> >& inputOutputSystem,
    ProgressReport& simplificationProgressReport,
    ProgressReport& substitutionsProgressReport
  );
  void backSubstituteIntoPolynomialSystem(
    List<PolynomialSubstitution<Coefficient> >& impliedSubstitutions
  );
  void backSubstituteIntoSinglePolynomial(
    Polynomial<Coefficient>& toBeSubstitutedIn,
    int index,
    PolynomialSubstitution<Coefficient>& finalSubstitution
  );
  bool getOneVariablePolynomialSolution(
    const Polynomial<Coefficient>& polynomial, Coefficient& outputSolution
  );
  void setSerreLikeSolutionIndex(int index, const Coefficient& inputConstant);
  void getSubstitutionFromPartialSolutionSerreLikeSystem(
    PolynomialSubstitution<Coefficient>& outputSubstitution
  );
  std::string toStringSerreLikeSolution();
  static int getNumberOfVariablesToSolveFor(
    const List<Polynomial<Coefficient> >& input
  );
  static void getVariablesToSolveFor(
    const List<Polynomial<Coefficient> >& input, Selection& output
  );
  void trySettingValueToVariable(
    List<Polynomial<Coefficient> >& inputSystem,
    const Coefficient& aValueToTryOnPreferredVariable
  );
  static bool isContradictoryReducedSystem(
    const List<Polynomial<Coefficient> >& input
  );
  std::string toStringImpliedSubstitutions();
  void initializeForSystemSolution();
  bool shouldReport();
  FormatExpressions& format();
};

// Solves polynomial equations in one variable, using shortcut
// methods and avoiding use of algebraic number machinery.
class PolynomialSolverWithQuadraticRadicalsUnivariate {
public:
  List<AlgebraicNumber> solutions;
  AlgebraicClosureRationals* ownerField;
  PolynomialSolverWithQuadraticRadicalsUnivariate(
    AlgebraicClosureRationals& inputField
  );
  bool solvePolynomialWithRadicals(
    const Polynomial<Rational>& input, std::stringstream* commentsOnFailure
  );
};

template <class Coefficient>
bool Polynomial<Coefficient>::getLinearSystemFromLinearPolynomials(
  const List<Polynomial<Coefficient> >& linearPolynomials,
  Matrix<Coefficient>& outputHomogenousPart,
  Matrix<Coefficient>& outputConstantTerms
) {
  STACK_TRACE("Polynomial::getLinearSystemFromLinearPolynomials");
  int letter = 0;
  int numberOfVariables = 0;
  for (int i = 0; i < linearPolynomials.size; i ++) {
    numberOfVariables =
    MathRoutines::maximum(
      linearPolynomials[i].minimalNumberOfVariables(), numberOfVariables
    );
  }
  outputHomogenousPart.initialize(linearPolynomials.size, numberOfVariables);
  outputHomogenousPart.makeZero();
  outputConstantTerms.initialize(linearPolynomials.size, 1);
  outputConstantTerms.makeZero();
  for (int i = 0; i < linearPolynomials.size; i ++) {
    for (int j = 0; j < linearPolynomials[i].size(); j ++) {
      if (linearPolynomials[i][j].isLinearNoConstantTerm(&letter)) {
        outputHomogenousPart(i, letter) =
        linearPolynomials[i].coefficients[j];
      } else if (linearPolynomials[i][j].isConstant()) {
        outputConstantTerms(i, 0) = linearPolynomials[i].coefficients[j];
        outputConstantTerms(i, 0) *= - 1;
      } else {
        return false;
      }
    }
  }
  return true;
}

template <class Coefficient>
void Polynomial<Coefficient>::makeLinearWithConstantTerm(
  const Vector<Rational>& inputLastCoordinateConstantTerm
) {
  this->makeZero();
  MonomialPolynomial monomial;
  for (int i = 0; i < inputLastCoordinateConstantTerm.size - 1; i ++) {
    monomial.makeEi(i);
    this->addMonomial(monomial, inputLastCoordinateConstantTerm[i]);
  }
  this->operator+=(*inputLastCoordinateConstantTerm.lastObject());
}

template <class Coefficient>
void Polynomial<Coefficient>::makeLinearNoConstant(
  const Vector<Rational>& inputCoefficients
) {
  STACK_TRACE("Polynomial::makeLinearNoConstant");
  this->makeZero();
  MonomialPolynomial monomial;
  for (int i = 0; i < inputCoefficients.size; i ++) {
    monomial.makeEi(i);
    this->addMonomial(monomial, inputCoefficients[i]);
  }
}

template <class Coefficient>
void PolynomialSubstitution<Coefficient>::makeIdentitySubstitution(
  int numberOfVariables, const Coefficient& ringUnit
) {
  this->makeIdentityLikeInjectionSubstitution(
    numberOfVariables, numberOfVariables, ringUnit
  );
}

template <class Coefficient>
void PolynomialSubstitution<Coefficient>::makeIdentityLikeInjectionSubstitution
(
  int numberOfStartingVariables,
  int numberOfTargetVariablesMustBeLargerOrEqual,
  const Coefficient& ringUnit
) {
  if (numberOfStartingVariables > numberOfTargetVariablesMustBeLargerOrEqual) {
    global.fatal << "Bad number of variables. " << global.fatal;
  }
  this->setSize(numberOfStartingVariables);
  for (int i = 0; i < this->size; i ++) {
    Polynomial<Coefficient>& currentPolynomial = this->objects[i];
    currentPolynomial.makeDegreeOne(
      numberOfTargetVariablesMustBeLargerOrEqual, i, ringUnit
    );
  }
}

template <class Coefficient>
void PolynomialSubstitution<Coefficient>::makeExponentSubstitution(
  Matrix<LargeInteger>& substitution
) {
  Polynomial<Coefficient> polynomial;
  MonomialPolynomial monomial;
  monomial.makeOne();
  this->size = 0;
  this->setSize(substitution.numberOfColumns);
  for (int i = 0; i < substitution.numberOfColumns; i ++) {
    for (int j = 0; j < substitution.numberOfRows; j ++) {
      monomial.setVariable(j, substitution(j, i));
    }
    polynomial.makeZero();
    polynomial.addMonomial(monomial, 1);
    this->objects[i] = polynomial;
  }
}

template <class Coefficient>
void Polynomial<Coefficient>::makePolynomialFromDirectionAndNormal(
  Vector<Coefficient>& direction,
  Vector<Coefficient>& normal,
  Coefficient& correction
) {
  Rational scalarProduct = Vector<Coefficient>::scalarEuclidean(
    direction, normal
  );
  this->makeZero();
  MonomialPolynomial monomial;
  for (int i = 0; i < direction.size; i ++) {
    monomial.makeEi(i);
    this->addMonomial(monomial, normal.objects[i] / scalarProduct);
  }
  *this += correction;
}

template <typename Coefficient>
void Matrix<Coefficient>::actOnMonomialAsDifferentialOperator(
  const MonomialPolynomial& input, Polynomial<Rational>& output
) {
  if (this->numberOfRows != this->numberOfColumns) {
    global.fatal << "Matrix not square as expected. " << global.fatal;
  }
  MonomialPolynomial monomial;
  output.makeZero();
  Rational coefficient;
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      monomial = input;
      coefficient = monomial(j);
      coefficient *= this->elements[i][j];
      monomial.multiplyByVariable(j, - 1);
      monomial.multiplyByVariable(i, 1);
      output.addMonomial(monomial, coefficient);
    }
  }
}

template <class Coefficient>
void PolynomialSubstitution<Coefficient>::
makeLinearSubstitutionConstantTermsLastRow(Matrix<Coefficient>& matrix) {
  this->setSize(matrix.numberOfColumns);
  MonomialPolynomial monomial;
  for (int i = 0; i < this->size; i ++) {
    this->objects[i].makeZero();
    for (int j = 0; j < matrix.numberOfRows - 1; j ++) {
      monomial.makeOne();
      monomial.setVariable(j, 1);
      this->objects[i].addMonomial(monomial, matrix.elements[j][i]);
    }
    this->objects[i] += matrix.elements[matrix.numberOfRows - 1][i];
  }
}

template <class Coefficient>
void PolynomialSubstitution<Coefficient>::getLinearSystemFromLinearPolynomials(
  Matrix<Coefficient>& outputHomogenousPart,
  Matrix<Coefficient>& outputConstantTerms
) {
  bool mustBeTrue =
  Polynomial<Coefficient>::getLinearSystemFromLinearPolynomials(
    *this, outputHomogenousPart, outputConstantTerms
  );
  if (!mustBeTrue) {
    global.fatal
    << "Function PolynomialSubstitution::"
    << "getLinearSystemFromLinearPolynomialsSystem "
    << "was called with non-linear inputs: "
    << this->toString()
    << "."
    << global.fatal;
  }
}

#endif // header_math_polynomials_ALREADY_INCLUDED
