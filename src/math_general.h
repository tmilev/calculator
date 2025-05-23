#ifndef header_math_general_ALREADY_INCLUDED
#define header_math_general_ALREADY_INCLUDED

#include "general_logging_global_variables.h"
#include "general_strings.h"
#include "math_large_integers.h"
#include "math_subsets_selections.h"
#include "math_vectors.h"

// An externally defined class used to plot Dynkin diagrams.
class Plot;
class ElementZmodP;
class WeylGroupData;
class AlgebraicClosureRationals;
template <class Coefficient>
class MatrixTensor;
class ProgressReport;

class ChevalleyGenerator {
public:
  SemisimpleLieAlgebra* owner;
  // generatorIndex = 0 stands for g_{- N} and all larger indices stand for the
  // remaining generators in the Lie algebra basis:
  // g_{- N}, ... g_{- 1}, h_1, ..., h_k, g_1, ..., g_N.
  // Here h_1, ..., h_k are elements of the Cartan subalgebra, the elements
  // g_{- N}, ..., g_{- 1} are the negative Chevalley generators,
  // and g_1, ..., g_N are the positive.
  // The generators are ordered in the standard order assumed in our system,
  // i.e., in the graded lexicographic order with
  // respect to the simple coordinates of the weights of the generators.
  int generatorIndex;
  ChevalleyGenerator(): owner(nullptr), generatorIndex(- 1) {}
  friend std::ostream& operator<<(
    std::ostream& output, const ChevalleyGenerator& generator
  ) {
    output << generator.toString();
    return output;
  }
  bool checkInitialization() const;
  static unsigned int hashFunction(const ChevalleyGenerator& input) {
    return static_cast<unsigned>(input.generatorIndex);
  }
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  void makeGenerator(
    SemisimpleLieAlgebra& inputOwner, int inputGeneratorIndex
  );
  void makeGeneratorRootSpace(
    SemisimpleLieAlgebra& inputOwner, const Vector<Rational>& root
  );
  void operator=(const ChevalleyGenerator& other) {
    this->owner = other.owner;
    this->generatorIndex = other.generatorIndex;
  }
  bool operator>(const ChevalleyGenerator& other) const;
  std::string toString(FormatExpressions* inputFormat = nullptr) const;
  void checkConsistencyWithOther(const ChevalleyGenerator& other) const;
  bool operator==(const ChevalleyGenerator& other) const {
    this->checkConsistencyWithOther(other);
    return this->generatorIndex == other.generatorIndex;
  }
  bool isInCartan(Vector<Rational>* whichElement) const;
  // Returns the root of the generator if the element
  // is not in the Cartan subalgebra.
  // If the element is in the Cartan subalgebra, returns the zero vector.
  bool isInRootSpace(Vector<Rational>* whichRoot) const;
  class Test {
  public:
    static bool all();
    static bool basic();
  };
};

template <
  class Coefficient,
  unsigned int inputHashFunction(const Coefficient&) = Coefficient::
  hashFunction
>
class MonomialTensor {
  friend std::ostream& operator<<(
    std::ostream& output,
    const MonomialTensor<Coefficient, inputHashFunction>& monomial
  ) {
    return output << monomial.toString();
  }
private:
public:
  List<int> generatorsIndices;
  List<Coefficient> powers;
  bool flagDeallocated;
  std::string toString(FormatExpressions* format = nullptr) const;
  bool isEqualToOne() const {
    return this->generatorsIndices.size == 0;
  }
  void operator=(List<int>& other) {
    this->generatorsIndices.reserve(other.size);
    this->powers.reserve(other.size);
    this->makeConstant();
    for (int i = 0; i < other.size; i ++) {
      this->multiplyByGeneratorPowerOnTheRight(other[i], 1);
    }
  }
  bool checkConsistency() {
    if (this->flagDeallocated) {
      global.fatal << "Use after free of MonomialTensor." << global.fatal;
    }
    return true;
  }
  void operator=(const MonomialTensor<Coefficient, inputHashFunction>& other) {
    this->generatorsIndices = other.generatorsIndices;
    this->powers = other.powers;
  }
  MonomialTensor() {
    this->flagDeallocated = false;
  }
  ~MonomialTensor() {
    this->flagDeallocated = true;
  }
  int minimalNumberOfVariables() {
    int result = 0;
    for (int i = 0; i < this->powers.size; i ++) {
      result =
      MathRoutines::maximum(
        result, this->powers[i].minimalNumberOfVariables()
      );
    }
    return result;
  }
  template <class otherType>
  void operator=(const MonomialTensor<otherType>& other) {
    this->generatorsIndices = other.generatorsIndices;
    this->powers.setSize(other.powers.size);
    for (int i = 0; i < other.powers.size; i ++) {
      this->powers[i] = other.powers[i];
    }
  }
  bool simplifyEqualConsecutiveGenerators(int lowestNonReducedIndex);
  void multiplyByGeneratorPowerOnTheRight(
    int generatorIndex, const Coefficient& power
  );
  void multiplyByGeneratorPowerOnTheLeft(
    int generatorIndexStandsToTheLeft, const Coefficient& power
  );
  unsigned int hashFunction() const {
    unsigned int result = 0;
    int j = 0;
    for (int i = 0; i < this->generatorsIndices.size; i ++) {
      result +=
      HashConstants::getConstantIncrementCounter(j) *
      this->generatorsIndices[i] +
      HashConstants::getConstantIncrementCounter(j) *
      inputHashFunction(this->powers[i]);
    }
    return result;
  }
  static unsigned int hashFunction(
    const MonomialTensor<Coefficient, inputHashFunction>& input
  ) {
    return input.hashFunction();
  }
  void makeConstant() {
    this->generatorsIndices.size = 0;
    this->powers.size = 0;
  }
  bool operator>(const MonomialTensor<Coefficient, inputHashFunction>& other)
  const {
    if (other.generatorsIndices.size > this->generatorsIndices.size) {
      return false;
    }
    if (other.generatorsIndices.size < this->generatorsIndices.size) {
      return true;
    }
    for (int i = 0; i < this->generatorsIndices.size; i ++) {
      if (other.generatorsIndices[i] > this->generatorsIndices[i]) {
        return false;
      }
      if (other.generatorsIndices[i] < this->generatorsIndices[i]) {
        return true;
      }
      if (other.powers[i] > this->powers[i]) {
        return false;
      }
      if (this->powers[i] > other.powers[i]) {
        return true;
      }
    }
    return false;
  }
  bool operator==(const MonomialTensor<Coefficient, inputHashFunction>& other)
  const {
    return
    this->powers == other.powers &&
    this->generatorsIndices == other.generatorsIndices;
  }
  bool operator<(const MonomialTensor<Coefficient, inputHashFunction>& right)
  const {
    Coefficient leftrank = 0;
    for (int i = 0; i < this->powers.size; i ++) {
      leftrank += this->powers[i];
    }
    Coefficient rightrank = 0;
    for (int i = 0; i < right.powers.size; i ++) {
      rightrank += right.powers[i];
    }
    if (leftrank < rightrank) {
      return true;
    }
    if (rightrank < leftrank) {
      return false;
    }
    // this does not crash for coefficient = int
    for (int i = 0; i < this->generatorsIndices.size; i ++) {
      if (this->generatorsIndices[i] < right.generatorsIndices[i]) {
        return false;
      }
      if (right.generatorsIndices[i] < this->generatorsIndices[i]) {
        return true;
      }
      if (this->powers[i] < right.powers[i]) {
        return false;
      }
      if (right.powers[i] < this->powers[i]) {
        return true;
      }
    }
    return false;
  }
  void operator*=(
    const MonomialTensor<Coefficient, inputHashFunction>& standsOnTheRight
  ) {
    if (standsOnTheRight.generatorsIndices.size == 0) {
      return;
    }
    if (this == &standsOnTheRight) {
      MonomialTensor<Coefficient, inputHashFunction> monomial;
      monomial = standsOnTheRight;
      (*this) *= (monomial);
      return;
    }
    this->generatorsIndices.setExpectedSize(
      standsOnTheRight.generatorsIndices.size + this->generatorsIndices.size
    );
    this->powers.setExpectedSize(
      standsOnTheRight.generatorsIndices.size + this->generatorsIndices.size
    );
    int firstIndex = standsOnTheRight.generatorsIndices[0];
    int i = 0;
    if (this->generatorsIndices.size > 0) {
      if (firstIndex == (*this->generatorsIndices.lastObject())) {
        *this->powers.lastObject() += standsOnTheRight.powers[0];
        i = 1;
      }
    }
    for (; i < standsOnTheRight.generatorsIndices.size; i ++) {
      this->powers.addOnTop(standsOnTheRight.powers[i]);
      this->generatorsIndices.addOnTop(standsOnTheRight.generatorsIndices[i]);
    }
  }
};

template <
  typename Type,
  unsigned int objectHashFunction(const Type&) = HashFunctions::hashFunction<
    Type
  >
>
class MonomialWrapper {
public:
  Type content;
  MonomialWrapper() {}
  MonomialWrapper(const Type& input) {
    this->content = input;
  }
  std::string toString(FormatExpressions* format = nullptr) const {
    (void) format;
    std::stringstream out;
    out << "(" << this->content << ")";
    return out.str();
  }
  bool operator>(const MonomialWrapper& other) const {
    return this->content > other.content;
  }
  friend std::ostream& operator<<(
    std::ostream& output, const MonomialWrapper& monomial
  ) {
    output << monomial.content;
    return output;
  }
  static unsigned int hashFunction(const MonomialWrapper& input) {
    return objectHashFunction(input.content);
  }
  bool operator==(const MonomialWrapper& other) const {
    return this->content == other.content;
  }
};

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

template <typename Coefficient>
std::ostream& operator<<(
  std::ostream& output, const Matrix<Coefficient>& matrix
);
template <typename Coefficient>
class Matrix {
  // friend std::iostream& operator<< <Coefficient>(std::iostream& output,
  // const
  // Matrix<Coefficient>& matrix);
  friend std::ostream& operator<<<Coefficient>(
    std::ostream& output, const Matrix<Coefficient>& matrix
  );
  // friend std::iostream& operator>> <Coefficient>(std::iostream& input,
  // Matrix<Coefficient>& matrix);
public:
  int numberOfRows;
  int actualNumberOfRows;
  int numberOfColumns;
  int actualNumberOfColumns;
  Coefficient** elements;
  bool flagDeallocated;
  void initialize(int desiredNumberOfRows, int desiredNumberOfColumns);
  void releaseMemory();
  bool isPositiveDefinite();
  bool isNonNegativeAllEntries() {
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        if (this->elements[i][j] < 0) {
          return false;
        }
      }
    }
    return true;
  }
  void resize(
    int desiredRows,
    int desiredColumns,
    bool preserveValues,
    const Coefficient* ringZero = nullptr
  );
  Matrix():
  numberOfRows(0),
  actualNumberOfRows(0),
  numberOfColumns(0),
  actualNumberOfColumns(0),
  elements(nullptr),
  flagDeallocated(false) {}
  Matrix(const Matrix<Coefficient>& other):
  numberOfRows(0),
  actualNumberOfRows(0),
  numberOfColumns(0),
  actualNumberOfColumns(0),
  elements(0),
  flagDeallocated(false) {
    *this = other;
  }
  ~Matrix() {
    this->releaseMemory();
    this->flagDeallocated = true;
  }
  void transpose() {
    if (this->numberOfColumns == this->numberOfRows) {
      for (int i = 0; i < this->numberOfRows; i ++) {
        for (int j = i + 1; j < this->numberOfColumns; j ++) {
          MathRoutines::swap<Coefficient>(
            this->elements[j][i], this->elements[i][j]
          );
        }
      }
      return;
    }
    Matrix<Coefficient> matrix;
    matrix.initialize(this->numberOfColumns, this->numberOfRows);
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        matrix.elements[j][i] = this->elements[i][j];
      }
    }
    *this = matrix;
  }
  void appendMatrixOnTheRight(const Matrix<Coefficient>& standsOnTheRight) {
    if (&standsOnTheRight == this) {
      Matrix<Coefficient> copyThis = *this;
      this->appendMatrixOnTheRight(copyThis);
      return;
    }
    if (standsOnTheRight.numberOfRows < this->numberOfRows) {
      Coefficient zero;
      zero = 0;
      Matrix<Coefficient> standsOnTheRightNew = standsOnTheRight;
      standsOnTheRightNew.resize(
        this->numberOfRows, standsOnTheRight.numberOfColumns, true, &zero
      );
      this->appendMatrixOnTheRight(standsOnTheRightNew);
      return;
    }
    if (this->numberOfRows < standsOnTheRight.numberOfRows) {
      Coefficient zero;
      zero = 0;
      this->resize(
        standsOnTheRight.numberOfRows, this->numberOfColumns, true, &zero
      );
    }
    int oldNumberOfColumns = this->numberOfColumns;
    this->resize(
      this->numberOfRows,
      standsOnTheRight.numberOfColumns + oldNumberOfColumns,
      true
    );
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = oldNumberOfColumns; j < this->numberOfColumns; j ++) {
        this->elements[i][j] =
        standsOnTheRight.elements[i][j - oldNumberOfColumns];
      }
    }
  }
  void appendRowToTheBottom(const Vector<Coefficient>& row) {
    Matrix<Coefficient> matrixRow;
    matrixRow.assignVectorRow(row);
    this->appendMatrixToTheBottom(matrixRow);
  }
  void appendMatrixToTheBottom(const Matrix<Coefficient>& standsBelow) {
    if (&standsBelow == this) {
      Matrix<Coefficient> copyThis = *this;
      this->appendMatrixToTheBottom(copyThis);
      return;
    }
    if (standsBelow.numberOfColumns < this->numberOfColumns) {
      Coefficient zero;
      zero = 0;
      Matrix<Coefficient> standsBelowNew = standsBelow;
      standsBelowNew.resize(
        standsBelow.numberOfRows, this->numberOfColumns, true, &zero
      );
      this->appendMatrixOnTheRight(standsBelowNew);
      return;
    }
    if (this->numberOfColumns < standsBelow.numberOfColumns) {
      Coefficient zero;
      zero = 0;
      this->resize(
        this->numberOfRows, standsBelow.numberOfColumns, true, &zero
      );
    }
    // So far, we have guaranteed that this and
    // &standsBelow have the same number of columns and
    // are different objects.
    int oldNumberOfRows = this->numberOfRows;
    this->resize(
      this->numberOfRows + standsBelow.numberOfRows,
      this->numberOfColumns,
      true
    );
    for (int i = oldNumberOfRows; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        this->elements[i][j] = standsBelow(i - oldNumberOfRows, j);
      }
    }
  }
  bool checkConsistency() const {
    if (this->flagDeallocated) {
      global.fatal << "Use after free of Matrix. " << global.fatal;
    }
    return true;
  }
  static void matrixInBasis(
    const Matrix<Coefficient>& input,
    Matrix<Coefficient>& output,
    const List<Vector<Coefficient> >& basis,
    const Matrix<Coefficient>& gramMatrixInverted
  ) {
    int d = basis.size;
    output.initialize(d, d);
    Vector<Rational> vectorColumn;
    for (int j = 0; j < d; j ++) {
      input.actOnVectorColumn(basis[j], vectorColumn);
      for (int i = 0; i < d; i ++) {
        output.elements[i][j] = basis[i].scalarEuclidean(vectorColumn);
      }
    }
    output.multiplyOnTheLeft(gramMatrixInverted);
  }
  void computeDeterminantOverwriteMatrix(
    Coefficient& output,
    const Coefficient& ringOne = 1,
    const Coefficient& ringZero = 0
  );
  void actOnVectorROWSOnTheLeft(
    List<Vector<Coefficient> >& standOnTheRightAsVectorRow
  ) const {
    List<Vector<Coefficient> > output;
    this->actOnVectorROWSOnTheLeft(standOnTheRightAsVectorRow, output);
    standOnTheRightAsVectorRow = output;
  }
  void actOnVectorROWSOnTheLeft(
    List<Vector<Coefficient> >& standOnTheRightAsVectorRow,
    List<Vector<Coefficient> >& output
  ) const {
    if (this->numberOfColumns != standOnTheRightAsVectorRow.size) {
      global.fatal
      << "Attempting to multiply a matrix, standing on the left, with "
      << this->numberOfColumns
      << " columns, by a matrix, standing on the right, with "
      << standOnTheRightAsVectorRow.size
      << " rows. "
      << global.fatal;
    }
    output.setSize(this->numberOfRows);
    for (int i = 0; i < this->numberOfRows; i ++) {
      output[i].makeZero(standOnTheRightAsVectorRow[0].size);
      for (int j = 0; j < this->numberOfColumns; j ++) {
        output[i] += standOnTheRightAsVectorRow[j] *(*this)(i, j);
      }
    }
  }
  void actMultiplyVectorRowOnTheRight(Vector<Coefficient>& standsOnTheLeft)
  const {
    Vector<Coefficient> output;
    this->actMultiplyVectorRowOnTheRight(standsOnTheLeft, output);
    standsOnTheLeft = output;
  }
  void actMultiplyVectorRowOnTheRight(
    const Vector<Coefficient>& standsOnTheLeft, Vector<Coefficient>& output
  ) const {
    if (&standsOnTheLeft == &output) {
      global.fatal
      << "In actMultiplyVectorRowOnTheRight: "
      << "output not allowed to equal input. "
      << global.fatal;
    }
    if (this->numberOfRows != standsOnTheLeft.size) {
      global.fatal
      << "Number of rows of matrix vector number of elements. "
      << global.fatal;
    }
    output.makeZero(this->numberOfColumns);
    Coefficient coefficient;
    for (int i = 0; i < this->numberOfColumns; i ++) {
      for (int j = 0; j < this->numberOfRows; j ++) {
        coefficient = this->elements[j][i];
        coefficient *= standsOnTheLeft[j];
        output[i] += coefficient;
      }
    }
  }
  void getNSquaredVectorForm(Vector<Coefficient>& output) {
    output.setSize(this->numberOfRows * this->numberOfColumns);
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        output.objects[i * this->numberOfRows + j] = this->elements[i][j];
      }
    }
  }
  template <class otherType>
  void actOnVectorColumn(
    const Vector<otherType>& input,
    Vector<otherType>& output,
    const otherType& ringZero = otherType::zero()
  ) const {
    if (&input == &output) {
      Vector<otherType> inputNew = input;
      this->actOnVectorColumn(inputNew, output, ringZero);
      return;
    }
    if (this->numberOfColumns != input.size) {
      global.fatal
      << "Attempt to multiply a matrix with "
      << this->numberOfColumns
      << " columns with a vector(column) of "
      << "dimension "
      << input.size
      << ". "
      << global.fatal;
    }
    output.makeZero(this->numberOfRows, ringZero);
    otherType product;
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        product = this->elements[i][j];
        product *= input[j];
        output[i] += product;
      }
    }
  }
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  static unsigned int hashFunction(const Matrix<Coefficient>& input) {
    unsigned int result = 0;
    int hashCounter = 0;
    for (int i = 0; i < input.numberOfRows; i ++) {
      for (int j = 0; j < input.numberOfColumns; j ++) {
        result +=
        input.elements[i][j].hashFunction() *
        HashConstants::getConstantIncrementCounter(hashCounter);
      }
    }
    return result;
  }
  template <class otherType>
  void actOnVectorsColumn(
    const Vectors<otherType>& input,
    Vectors<otherType>& output,
    const otherType& ringZero = 0
  ) const {
    if (&input == &output) {
      global.fatal
      << "In actOnVectorsColumn: input not allowed to equal output. "
      << global.fatal;
    }
    if (input.size == 0) {
      return;
    }
    if (this->numberOfColumns != input.getDimension()) {
      global.fatal
      << "Attempt to act by "
      << this->toString()
      << " (an "
      << this->numberOfRows
      << " x "
      << this->numberOfColumns
      << " matrix) on a column vector "
      << input.toString()
      << " (dimension "
      << input.size
      << "). "
      << global.fatal;
    }
    output.setSize(input.size);
    for (int i = 0; i < input.size; i ++) {
      this->actOnVectorColumn(input[i], output[i], ringZero);
    }
  }
  template <class otherType>
  void actOnVectorColumn(
    Vector<otherType>& inputOutput,
    const otherType& ringZero = otherType::zero()
  ) const {
    Vector<otherType> buffer;
    this->actOnVectorColumn(inputOutput, buffer, ringZero);
    inputOutput = buffer;
  }
  template <class otherType>
  void actOnVectorsColumn(
    Vectors<otherType>& inputOutput,
    const otherType& ringZero = otherType::zero()
  ) const {
    for (int i = 0; i < inputOutput.size; i ++) {
      this->actOnVectorColumn(inputOutput[i], ringZero);
    }
  }
  std::string toString(FormatExpressions* format = nullptr) const;
  std::string toStringLatex(FormatExpressions* format = nullptr) const;
  std::string toStringSystemLatex(
    Matrix<Coefficient>* constantTerms = 0, FormatExpressions* format = nullptr
  ) const;
  std::string toStringPlainText(bool jsonFormat = false) const;
  std::string toStringOneLine(bool jsonFormat = false) const;
  std::string toStringWithBlocks(List<int>& blocks);
  void makeIdentityMatrix(
    int dimension,
    const Coefficient& ringUnit = Coefficient::oneStatic(),
    const Coefficient& ringZero = Coefficient::zeroStatic()
  ) {
    this->initialize(dimension, dimension);
    for (int i = 0; i < dimension; i ++) {
      for (int j = 0; j < dimension; j ++) {
        if (j != i) {
          this->elements[i][j] = ringZero;
        } else {
          this->elements[i][j] = ringUnit;
        }
      }
    }
  }
  void makeZeroMatrix(int dimension, const Coefficient& ringZero = 0) {
    this->initialize(dimension, dimension);
    for (int i = 0; i < dimension; i ++) {
      for (int j = 0; j < dimension; j ++) {
        this->elements[i][j] = ringZero;
      }
    }
  }
  void actOnMonomialAsDifferentialOperator(
    const MonomialPolynomial& input, Polynomial<Rational>& output
  );
  void switchRows(int row1, int row2);
  void switchRowsWithCarbonCopy(
    int row1, int row2, Matrix<Coefficient>* carbonCopy
  ) {
    this->switchRows(row1, row2);
    if (carbonCopy != 0) {
      carbonCopy->switchRows(row1, row2);
    }
  }
  void setNumberOfVariables(int desiredNumberOfVariables) {
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        this->elements[i][j].setNumberOfVariables(desiredNumberOfVariables);
      }
    }
  }
  void substitute(const PolynomialSubstitution<Rational>& substitution);
  Coefficient scalarProduct(
    const Vector<Coefficient>& left, const Vector<Coefficient>& right
  ) const {
    return this->scalarProduct(left, right, static_cast<Coefficient>(0));
  }
  Coefficient scalarProduct(
    const Vector<Coefficient>& left,
    const Vector<Coefficient>& right,
    const Coefficient& ringZero
  ) const {
    if (left.size != this->numberOfColumns || right.size != this->numberOfRows)
    {
      global.fatal
      << "Scalar product using matrix: dimensions of vectors don't match. "
      << global.fatal;
    }
    Coefficient result;
    Coefficient contribution;
    result = ringZero;
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        contribution = left[i];
        contribution *= this->elements[i][j];
        contribution *= right[j];
        result += contribution;
      }
    }
    return result;
  }
  inline Coefficient& operator()(unsigned i, unsigned j) const {
    return (*this)(static_cast<signed>(i), static_cast<signed>(j));
  }
  Coefficient& operator()(int i, int j) const {
    if (i < 0 || i >= this->numberOfRows || j < 0 || j >= this->numberOfColumns)
    {
      global.fatal
      << "Request of (row, column) with index ("
      << i
      << ", "
      << j
      << "), but the present matrix has "
      << this->numberOfRows
      << " rows and "
      << this->numberOfColumns
      << " colums. "
      << global.fatal;
    }
    return this->elements[i][j];
  }
  bool isSquare() const {
    return this->numberOfColumns == this->numberOfRows;
  }
  bool isDiagonal() const {
    if (this->numberOfRows != this->numberOfColumns || this->numberOfRows <= 0)
    {
      return false;
    }
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        if (i == j) {
          continue;
        }
        if (!this->elements[i][j].isEqualToZero()) {
          return false;
        }
      }
    }
    return true;
  }
  bool isIdentity() const {
    if (this->numberOfRows != this->numberOfColumns || this->numberOfRows <= 0)
    {
      return false;
    }
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        if (i == j) {
          if (!this->elements[i][j].isEqualToOne()) {
            return false;
          }
        } else {
          if (!this->elements[i][j].isEqualToZero()) {
            return false;
          }
        }
      }
    }
    return true;
  }
  bool jordanNormalForm(
    Matrix<AlgebraicNumber>& outputLeft,
    Matrix<AlgebraicNumber>& outputDiagonalized,
    Matrix<AlgebraicNumber>& outputRight,
    AlgebraicClosureRationals& ownerField,
    std::stringstream* comments
  );
  void getVectorFromColumn(int columnIndex, Vector<Coefficient>& output) const;
  void getVectorFromRow(int rowIndex, Vector<Coefficient>& output) const;
  int findPivot(int columnIndex, int rowStartIndex);
  bool findFirstNonZeroElementSearchEntireRow(Coefficient& output) {
    for (int i = 0; i < this->numberOfColumns; i ++) {
      for (int j = 0; j < this->numberOfRows; j ++) {
        if (!this->elements[i][j].isEqualToZero()) {
          output = this->elements[i][j];
          return true;
        }
      }
    }
    return false;
  }
  bool makeInvertibleHomomorphismFromDomainAndRange(
    Vectors<Coefficient>& domain, Vectors<Coefficient>& range
  ) {
    Matrix<Coefficient> domainMatrix;
    Matrix<Coefficient> rangeMatrix;
    domainMatrix.assignVectorsToRows(domain);
    rangeMatrix.assignVectorsToRows(range);
    if (!domainMatrix.invert()) {
      return false;
    }
    (*this) = domainMatrix * rangeMatrix;
    this->transpose();
    return true;
  }
  void rowTimesScalar(int rowIndex, const Coefficient& scalar);
  void rowTimesScalarWithCarbonCopy(
    int rowIndex, const Coefficient& scalar, Matrix<Coefficient>* carbonCopy
  ) {
    this->rowTimesScalar(rowIndex, scalar);
    if (carbonCopy != 0) {
      carbonCopy->rowTimesScalar(rowIndex, scalar);
    }
  }
  void addTwoRows(
    int fromRowIndex,
    int toRowIndex,
    int startColumnIndex,
    const Coefficient& scalar
  );
  void addTwoRowsWithCarbonCopy(
    int fromRowIndex,
    int toRowIndex,
    int startColumnIndex,
    const Coefficient& scalar,
    Matrix<Coefficient>* carbonCopy
  ) {
    this->addTwoRows(fromRowIndex, toRowIndex, startColumnIndex, scalar);
    if (carbonCopy != 0) {
      carbonCopy->addTwoRows(
        fromRowIndex, toRowIndex, startColumnIndex, scalar
      );
    }
  }
  void subtractRows(
    int indexRowWeSubtractFrom,
    int indexSubtracted,
    int startColumnIndex,
    const Coefficient& scalar
  );
  void subtractRowsWithCarbonCopy(
    int indexRowWeSubtractFrom,
    int indexSubtracted,
    int startColumnIndex,
    const Coefficient& scalar,
    Matrix<Coefficient>* carbonCopy
  ) {
    this->subtractRows(
      indexRowWeSubtractFrom, indexSubtracted, startColumnIndex, scalar
    );
    if (carbonCopy != 0) {
      carbonCopy->subtractRows(
        indexRowWeSubtractFrom, indexSubtracted, startColumnIndex, scalar
      );
    }
  }
  void multiplyOnTheLeft(
    const Matrix<Coefficient>& standsOnTheLeft,
    Matrix<Coefficient>& output,
    const Coefficient& ringZero = 0
  ) const;
  void multiplyOnTheLeft(
    const Matrix<Coefficient>& standsOnTheLeft, const Coefficient& ringZero = 0
  );
  void multiplyOnTheRight(const Matrix<Coefficient>& standsOnTheRight) {
    Matrix<Coefficient> thisCopy = *this;
    *this = standsOnTheRight;
    this->multiplyOnTheLeft(thisCopy);
  }
  void getVectorsFromRows(List<Vector<Coefficient> >& output) const {
    output.setSize(this->numberOfRows);
    for (int i = 0; i < this->numberOfRows; i ++) {
      output[i].setSize(this->numberOfColumns);
      for (int j = 0; j < this->numberOfColumns; j ++) {
        output[i][j] = this->elements[i][j];
      }
    }
  }
  void nonPivotPointsToEigenVectorHomogeneous(
    Selection& nonPivotPoints,
    Vector<Coefficient>& output,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  void nonPivotPointsToEigenVectorLexicographic(
    Selection& nonPivotPoints,
    Vector<Coefficient>& output,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  bool invert();
  Matrix<Coefficient> inverse() {
    Matrix<Coefficient> copy = *this;
    if (!copy.invert()) {
      global.fatal
      << "Request to invert "
      << copy.toString()
      << " failed. "
      << global.fatal;
    }
    return copy;
  }
  Matrix<Coefficient> operator^(const Matrix<Coefficient>& right) const;
  static void conjugationAction(
    const Matrix<Coefficient>& conjugateMe,
    const Matrix<Coefficient>& conjugateBy,
    Matrix<Coefficient>& out
  );
  void makeZero(const Coefficient& ringZero = 0);
  void makeIdentity(
    const Matrix<Coefficient>& prototype,
    const Coefficient& ringZero = 0,
    const Coefficient& ringOne = 1
  );
  // If m1 corresponds to a linear operator from V1 to V2 and
  // m2 to a linear operator from W1 to W2, then the result of the below
  // function corresponds to the linear operator
  // from V1+W1 to V2+W2 (direct sum).
  // This means you write the matrix m1 in
  // the upper left corner m2 in the lower
  // right and everything else you fill with zeros.
  void assignDirectSum(Matrix<Coefficient>& m1, Matrix<Coefficient>& m2);
  // If S and T are endomorphisms of V and W, build the matrix of S \otimes T
  // that acts on
  // V \otimes W with basis
  // (v1 \otimes w1, v1 \otimes w2, ..., v2 \otimes w1, v2 \otimes w2, ..., vn
  // \otimes wn)
  void assignTensorProduct(
    const Matrix<Coefficient>& left, const Matrix<Coefficient>& right
  );
  void assignVectorsToRows(const List<Vector<Coefficient> >& input) {
    int desiredNumberOfColumns = 0;
    if (input.size > 0) {
      desiredNumberOfColumns = input[0].size;
    }
    this->initialize(input.size, desiredNumberOfColumns);
    for (int i = 0; i < input.size; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        this->elements[i][j] = input[i][j];
      }
    }
  }
  void assignVectorsToColumns(const Vectors<Coefficient>& input) {
    this->initialize(input[0].size, input.size);
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        (*this)(i, j) = input[j][i];
      }
    }
  }
  void assignVectorColumn(const Vector<Coefficient>& input) {
    this->initialize(input.size, 1);
    for (int i = 0; i < input.size; i ++) {
      this->elements[i][0] = input[i];
    }
  }
  void assignVectorToRowKeepOtherRowsIntactNoInitialization(
    int rowIndex, const Vector<Coefficient>& input
  ) {
    if (
      input.size != this->numberOfColumns ||
      rowIndex >= this->numberOfRows ||
      rowIndex < 0
    ) {
      global.fatal
      << "Attempt to assign vector "
      << input.toString()
      << " ("
      << input.size
      << " coordinates) "
      << "to row with index "
      << rowIndex
      << " in a matrix with "
      << this->numberOfRows
      << " rows and "
      << this->numberOfColumns
      << " columns. "
      << global.fatal;
    }
    for (int i = 0; i < this->numberOfColumns; i ++) {
      this->elements[rowIndex][i] = input[i];
    }
  }
  void assignVectorToColumnKeepOtherColsIntactNoInit(
    int columnIndex, const Vector<Coefficient>& input
  ) {
    if (
      input.size != this->numberOfRows ||
      columnIndex >= this->numberOfColumns ||
      columnIndex < 0
    ) {
      global.fatal
      << "In assignVectorToColumnKeepOtherColsIntactNoInit: "
      << "bad vector/matrix dimensions. "
      << global.fatal;
    }
    for (int i = 0; i < this->numberOfRows; i ++) {
      this->elements[i][columnIndex] = input[i];
    }
  }
  void assignBlock(Matrix<Coefficient>& block, int starti, int startj) {
    if (
      starti + block.numberOfRows > this->numberOfRows ||
      startj + block.numberOfColumns > this->numberOfColumns
    ) {
      global.fatal << "In assignBlock: bad indices. " << global.fatal;
    }
    for (int i = 0; i < block.numberOfRows; i ++) {
      for (int j = 0; j < block.numberOfColumns; j ++) {
        this->elements[starti + i][startj + j] = block.elements[i][j];
      }
    }
  }
  void assignVectorRow(const Vector<Coefficient>& input) {
    this->initialize(1, input.size);
    for (int i = 0; i < input.size; i ++) {
      this->elements[0][i] = input[i];
    }
  }
  void directSumWith(
    const Matrix<Coefficient>& m2, const Coefficient& ringZero = 0
  );
  bool operator!=(const Matrix<Coefficient>& other) const {
    return !((*this) == other);
  }
  bool operator==(const Matrix<Coefficient>& other) const {
    if (
      this->numberOfRows != other.numberOfRows ||
      this->numberOfColumns != other.numberOfColumns
    ) {
      return false;
    }
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        if (!(this->elements[i][j] == other.elements[i][j])) {
          return false;
        }
      }
    }
    return true;
  }
  bool isEqualToZero() const {
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        if (this->elements[i][j] != 0) {
          return false;
        }
      }
    }
    return true;
  }
  bool isProportionalTo(
    const Matrix<Coefficient>& input, Coefficient& outputTimesMeEqualsInput
  ) {
    if (
      input.numberOfColumns != this->numberOfColumns ||
      input.numberOfRows != this->numberOfRows
    ) {
      return false;
    }
    bool found = false;
    for (int i = 0; i < this->numberOfRows && !found; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        if (!this->elements[i][j].isEqualToZero()) {
          found = true;
          outputTimesMeEqualsInput = input.elements[i][j];
          outputTimesMeEqualsInput /= this->elements[i][j];
          break;
        }
      }
    }
    if (!found) {
      return input.isEqualToZero();
    }
    Matrix<Coefficient> mustBeZero;
    mustBeZero = *this;
    mustBeZero *= outputTimesMeEqualsInput;
    mustBeZero -= input;
    return mustBeZero.isEqualToZero();
  }
  // Returns true if the system has a solution, false otherwise.
  bool rowEchelonFormToLinearSystemSolution(
    Selection& inputPivotPoints,
    Matrix<Coefficient>& inputRightHandSide,
    Matrix<Coefficient>& outputSolution
  );
  void operator+=(const Matrix<Coefficient>& right) {
    if (
      this->numberOfRows != right.numberOfRows ||
      this->numberOfColumns != right.numberOfColumns
    ) {
      global.fatal
      << "Attempt to add matrix with "
      << this->numberOfRows
      << " rows and "
      << this->numberOfColumns
      << " columns to a matrix with "
      << right.numberOfRows
      << " rows and "
      << right.numberOfColumns
      << " columns. "
      << global.fatal;
    }
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        this->elements[i][j] += right.elements[i][j];
      }
    }
  }
  LargeIntegerUnsigned findPositiveLCMCoefficientDenominators();
  void operator-=(const Matrix<Coefficient>& right);
  void writeToFile(std::fstream& output);
  bool readFromFile(std::fstream& input);
  void operator/=(const Coefficient& input) {
    for (int j = 0; j < this->numberOfRows; j ++) {
      for (int i = 0; i < this->numberOfColumns; i ++) {
        this->elements[j][i] /= input;
      }
    }
  }
  void operator*=(const Coefficient& input) {
    for (int j = 0; j < this->numberOfRows; j ++) {
      for (int i = 0; i < this->numberOfColumns; i ++) {
        this->elements[j][i] *= input;
      }
    }
  }
  void operator*=(const Matrix<Coefficient>& input) {
    this->multiplyOnTheRight(input);
  }
  void lieBracketWith(Matrix<Coefficient>& right) {
    Matrix<Coefficient> result;
    this->lieBracket(*this, right, result);
    *this = result;
  }
  static void lieBracket(
    const Matrix<Coefficient>& left,
    const Matrix<Coefficient>& right,
    Matrix<Coefficient>& output
  ) {
    if (
      left.numberOfColumns != left.numberOfRows ||
      right.numberOfColumns != right.numberOfRows ||
      left.numberOfColumns != right.numberOfColumns
    ) {
      global.fatal
      << "In lieBracket: bad dimensions of matrices. "
      << global.fatal;
    }
    Matrix<Coefficient> firstSummand;
    Matrix<Coefficient> subtracand;
    firstSummand = (right);
    firstSummand.multiplyOnTheLeft(left);
    subtracand = (left);
    subtracand.multiplyOnTheLeft(right);
    firstSummand -= (subtracand);
    output = firstSummand;
  }
  // The Gaussian elimination below brings the matrix to a row-echelon form,
  // that is, makes the matrix be something like (example is 4x5):
  // 1 0 3 0 0
  // 0 1 0 0 0
  // 0 0 0 1 0
  // 0 0 0 0 0
  // In the Gaussian elimination below, we define
  // non-pivot points to be indices
  // of the columns
  // that do not have a pivot 1 in them.
  // In the above example, the third (index 2) and fifth (index 4) columns do
  // not have a pivot 1 in them.
  void gaussianEliminationByRows(
    Matrix<Coefficient>* carbonCopyMatrix = 0,
    Selection* outputNonPivotColumns = nullptr,
    Selection* outputPivotColumns = nullptr,
    std::stringstream* humanReadableReport = nullptr,
    FormatExpressions* format = nullptr
  );
  void gaussianEliminationByRowsNoRowSwapPivotPointsByRows(
    int firstNonProcessedRow,
    List<int>& outputPivotPointCols,
    Selection* outputNonPivotPoints__WarningSelectionNotInitialized
  );
  void gaussianEliminationEuclideanDomain(
    Matrix<Coefficient>* otherMatrix = 0,
    const Coefficient& negativeOne = - 1,
    const Coefficient& ringUnit = 1,
    bool(*comparisonGEQFunction)(
      const Coefficient& left, const Coefficient& right
    ) =
    0
  );
  static bool solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(
    Matrix<Coefficient>& A, Matrix<Coefficient>& b, Matrix<Coefficient>& output
  );
  Coefficient getDeterminant();
  Coefficient getTrace() const;
  void assignMatrixIntegerWithDenominator(
    Matrix<LargeInteger>& matrix, const LargeIntegerUnsigned& denominator
  );
  void scaleToIntegralForMinimalRationalHeightNoSignChange();
  void getMatrixIntegerWithDenominator(
    Matrix<LargeInteger>& outputMat, LargeIntegerUnsigned& outputDenominator
  );
  void lieBracketWith(const Matrix<Coefficient>& right);
  bool getEigenspacesProvidedAllAreIntegralWithEigenValueSmallerThanDimension(
    List<Vectors<Coefficient> >& output
  ) const;
  void getZeroEigenSpace(List<Vector<Coefficient> >& output) const {
    Matrix<Coefficient> matrixCopy = *this;
    matrixCopy.getZeroEigenSpaceModifyMe(output);
  }
  void getZeroEigenSpaceModifyMe(List<Vector<Coefficient> >& output);
  void getEigenspaceModifyMe(
    const Coefficient& inputEigenValue,
    List<Vector<Coefficient> >& outputEigenspace
  ) {
    for (int i = 0; i < this->numberOfColumns; i ++) {
      this->elements[i][i] -= inputEigenValue;
    }
    this->getZeroEigenSpaceModifyMe(outputEigenspace);
  }
  static bool
  systemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(
    Matrix<Coefficient>& matrixA,
    Matrix<Coefficient>& matrixb,
    Vector<Coefficient>* outputSolution = 0
  );
  static void computePotentialChangeGradient(
    Matrix<Coefficient>& matrixA,
    Selection& baseVariables,
    int numberOfTrueVariables,
    int columnIndex,
    Rational& outputChangeGradient,
    bool& hasAPotentialLeavingVariable
  );
  static void getMaxMovementAndLeavingVariableRow(
    Rational& maximumMovement,
    int& leavingVariableRow,
    int enteringVariable,
    Matrix<Coefficient>& matrixA,
    Vector<Coefficient>& inputVectorX,
    Selection& baseVariables
  );
  LargeInteger findPositiveGCDCoefficientNumerators();
  Matrix<Coefficient> operator-(const Matrix<Coefficient>& right) const {
    Matrix<Coefficient> result = (*this);
    result -= right;
    return result;
  }
  Matrix<Coefficient> operator*(const Matrix<Coefficient>& right) const;
  Vector<Coefficient> operator*(const Vector<Coefficient>& v) const;
  void operator=(const Matrix<Coefficient>& other);
  template <class otherType>
  void operator=(const Matrix<otherType>& other) {
    this->resize(other.numberOfRows, other.numberOfColumns, false);
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        (*this)(i, j) = other(i, j);
      }
    }
  }
  // The following are for compatibility with the FiniteGroup class.
  void getCharacteristicPolynomialStandardRepresentation(
    Polynomial<Coefficient>& out
  ) {
    out.assignCharacteristicPolynomial(*this);
  }
  bool hasDifferentConjugacyInvariantsFrom(Matrix<Coefficient>& other) {
    Polynomial<Coefficient> p;
    Polynomial<Coefficient> q;
    this->getCharacteristicPolynomialStandardRepresentation(p);
    other.getCharacteristicPolynomialStandardRepresentation(q);
    return !(p == q);
  }
  bool operator>(const Matrix<Coefficient>& right) const {
    if (
      this->numberOfRows != right.numberOfRows ||
      this->numberOfColumns != right.numberOfColumns
    ) {
      global.fatal
      << "An attempt was just made to compare "
      << "two matrices of different dimensions; "
      << "most likely something is wrong in some "
      << "FiniteGroup, see the frames above. "
      << global.fatal;
    }
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        if (!(this->elements[i][j] > right.elements[i][j])) {
          return false;
        }
      }
    }
    return true;
  }
  class Test {
  public:
    static bool all();
    static bool matrixIntegerWithDenominator();
    static bool oneMatrixIntegerWithDenominator(
      const std::string& input,
      const std::string& expectedMatrix,
      int expectedScale
    );
    // Makes a matrix from a string such as
    // ((2,3), (3,4), (4,5))
    // Converted to matrix using the calculator MakeMatrix function.
    static void matrixFromString(
      const std::string& inputString, Matrix<Rational>& output
    );
  };
};

template <class Coefficient>
bool Vectors<Coefficient>::conesIntersect(
  List<Vector<Rational> >& strictCone,
  List<Vector<Rational> >& nonStrictCone,
  Vector<Rational>* outputLinearCombo,
  Vector<Rational>* outputSplittingNormal
) {
  Matrix<Rational> matA;
  Matrix<Rational> matb;
  if (strictCone.size == 0) {
    if (outputSplittingNormal != nullptr) {
      if (nonStrictCone.size > 0) {
        outputSplittingNormal->makeZero(nonStrictCone[0].size);
      }
    }
    return false;
  }
  int dimension = strictCone[0].size;
  int numberOfColumns = strictCone.size + nonStrictCone.size;
  matA.initialize(dimension + 1, numberOfColumns);
  matb.initialize(dimension + 1, 1);
  matb.makeZero();
  matb.elements[dimension][0].makeOne();
  for (int i = 0; i < strictCone.size; i ++) {
    for (int k = 0; k < dimension; k ++) {
      matA.elements[k][i].assign(strictCone[i][k]);
    }
    matA.elements[dimension][i].makeOne();
  }
  for (int i = 0; i < nonStrictCone.size; i ++) {
    int currentColumn = i + strictCone.size;
    for (int k = 0; k < dimension; k ++) {
      matA.elements[k][currentColumn].assign(nonStrictCone[i][k]);
      matA.elements[k][currentColumn].negate();
    }
    matA.elements[dimension][currentColumn].makeZero();
  }
  if (
    !Matrix<Rational>::
    systemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution
    (matA, matb, outputLinearCombo)
  ) {
    if (outputSplittingNormal != nullptr) {
      bool tempBool = Vectors<Coefficient>::getNormalSeparatingCones(
        strictCone, nonStrictCone, *outputSplittingNormal
      );
      if (!tempBool) {
        global.fatal
        << "This is an algorithmic/mathematical "
        << "(hence also programming) error. "
        << "I get that two cones do not intersect, "
        << "yet there exists no plane separating them. "
        << "Something is wrong with the "
        << "implementation of the simplex algorithm. "
        << "The input which manifested the problem was: <br>StrictCone: <br>"
        << strictCone.toString()
        << "<br>Non-strict cone: <br>"
        << nonStrictCone.toString()
        << "<br>"
        << global.fatal;
      }
    }
    return false;
  }
  if (outputLinearCombo != nullptr) {
    for (int i = strictCone.size; i < outputLinearCombo->size; i ++) {
      (*outputLinearCombo)[i] *= - 1;
    }
  }
  return true;
}

template <typename Coefficient>
void Matrix<Coefficient>::resize(
  int r, int c, bool preserveValues, const Coefficient* const ringZero
) {
  if (r < 0) {
    r = 0;
  }
  if (c < 0) {
    c = 0;
  }
  if (r == this->numberOfRows && c == this->numberOfColumns) {
    return;
  }
  if (r == 0 || c == 0) {
    this->numberOfRows = r;
    this->numberOfColumns = c;
    return;
  }
  Coefficient** newElements = nullptr;
  int newActualNumberOfColumns =
  MathRoutines::maximum(this->actualNumberOfColumns, c);
  int newActualNumberOfRows =
  MathRoutines::maximum(this->actualNumberOfRows, r);
  if (r > this->actualNumberOfRows || c > this->actualNumberOfColumns) {
    newElements = new Coefficient*[newActualNumberOfRows];
#ifdef AllocationLimitsSafeguard
    GlobalStatistics::globalPointerCounter += newActualNumberOfRows;
    GlobalStatistics::checkPointerCounters();
#endif
    for (int i = 0; i < newActualNumberOfRows; i ++) {
      newElements[i] = new Coefficient[newActualNumberOfColumns];
#ifdef AllocationLimitsSafeguard
      GlobalStatistics::globalPointerCounter += newActualNumberOfColumns;
      GlobalStatistics::checkPointerCounters();
#endif
    }
  }
  int firstInvalidRow = MathRoutines::minimum(this->numberOfRows, r);
  int firstInvalidColumn = MathRoutines::minimum(this->numberOfColumns, c);
  if (preserveValues && newElements != nullptr) {
    for (int j = 0; j < firstInvalidRow; j ++) {
      for (int i = 0; i < firstInvalidColumn; i ++) {
        newElements[j][i] = this->elements[j][i];
      }
    }
  }
  if (ringZero != nullptr) {
    if (!preserveValues) {
      firstInvalidRow = 0;
      firstInvalidColumn = 0;
    }
    for (int i = 0; i < r; i ++) {
      int columnStart = (i < firstInvalidRow) ? firstInvalidColumn : 0;
      for (int j = columnStart; j < c; j ++) {
        newElements[i][j] = *ringZero;
      }
    }
  }
  if (newElements != nullptr) {
    this->releaseMemory();
    this->elements = newElements;
    this->actualNumberOfColumns = newActualNumberOfColumns;
    this->actualNumberOfRows = newActualNumberOfRows;
  }
  this->numberOfColumns = c;
  this->numberOfRows = r;
}

template <typename Coefficient>
void Vectors<Coefficient>::getMatrixRootsToRows(Matrix<Rational>& output) const {
  int numberOfColumns = 0;
  if (this->size != 0) {
    numberOfColumns = static_cast<int>(this->objects[0].size);
  }
  output.initialize(static_cast<int>(this->size), numberOfColumns);
  for (int i = 0; i < this->size; i ++) {
    for (int j = 0; j < numberOfColumns; j ++) {
      output.elements[i][j] = this->objects[i][j];
    }
  }
}

template <typename Coefficient>
void Vectors<Coefficient>::getOrthogonalComplement(
  Vectors<Coefficient>& output, Matrix<Coefficient>* bilinearForm
) {
  STACK_TRACE("Vectors::getOrthogonalComplement");
  if (this->size == 0) {
    if (bilinearForm != nullptr) {
      output.makeEiBasis(bilinearForm->numberOfRows);
      return;
    }
    global.fatal
    << "Finding orthogonal complement of zero "
    << "vectors without specifying a bilinear form is "
    << "forbidden: I can't determine the dimension "
    << "of the ambient vector space. "
    << global.fatal;
  }
  Matrix<Coefficient> matrix;
  matrix.assignVectorsToRows(*this);
  if (bilinearForm != nullptr) {
    matrix *= *bilinearForm;
  }
  matrix.getZeroEigenSpaceModifyMe(output);
}

template <typename Coefficient>
bool Vectors<Coefficient>::computeNormal(
  Vector<Coefficient>& output, int inputDimension
) {
  if (this->size == 0) {
    if (inputDimension == 1) {
      output.makeEi(1, 0);
      return true;
    }
    return false;
  }
  int dimension = this->objects[0].size;
  Matrix<Coefficient> eliminated;
  Selection nonPivotPoints;
  nonPivotPoints.initialize(dimension);
  output.setSize(dimension);
  this->gaussianEliminationForNormalComputation(
    eliminated, nonPivotPoints, dimension
  );
  if (nonPivotPoints.cardinalitySelection != 1) {
    return false;
  }
  eliminated.nonPivotPointsToEigenVectorHomogeneous(nonPivotPoints, output);
  return true;
}

template <typename Coefficient>
void Matrix<Coefficient>::operator=(const Matrix<Coefficient>& m) {
  if (this == &m) {
    return;
  }
  this->resize(m.numberOfRows, m.numberOfColumns, false);
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      this->elements[i][j] = m.elements[i][j];
    }
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::releaseMemory() {
  for (int i = 0; i < this->actualNumberOfRows; i ++) {
    delete[] this->elements[i];
  }
  delete[] this->elements;
#ifdef AllocationLimitsSafeguard
  GlobalStatistics::globalPointerCounter -=
  this->actualNumberOfRows * this->actualNumberOfColumns +
  this->actualNumberOfRows;
  GlobalStatistics::checkPointerCounters();
#endif
  this->elements = nullptr;
  this->numberOfColumns = 0;
  this->numberOfRows = 0;
  this->actualNumberOfRows = 0;
  this->actualNumberOfColumns = 0;
}

template <typename Coefficient>
bool Matrix<Coefficient>::invert() {
  STACK_TRACE("Matrix::invert");
  if (this->numberOfColumns != this->numberOfRows) {
    global.fatal
    << "Request to invert a non-square matrix of "
    << this->numberOfRows
    << " rows and "
    << this->numberOfColumns
    << " columns. "
    << global.fatal;
  }
  if (this->numberOfColumns == 0) {
    global.fatal << "0 by 0 matrix inversion is not allowed. " << global.fatal;
  }
  Matrix inverse;
  inverse.makeIdentityMatrix(
    this->numberOfRows, (*this)(0, 0).one(), (*this)(0, 0).zero()
  );
  Selection nonPivotColumns;
  this->gaussianEliminationByRows(&inverse, &nonPivotColumns, 0);
  if (nonPivotColumns.cardinalitySelection != 0) {
    return false;
  }
  *this = inverse;
  return true;
}

template <typename Coefficient>
Matrix<Coefficient> Matrix<Coefficient>::operator^(
  const Matrix<Coefficient>& right
) const {
  Matrix<Coefficient> conjugateInverse = right;
  conjugateInverse.invert();
  Matrix<Coefficient> buffer;
  this->multiplyOnTheLeft(conjugateInverse, buffer);
  Matrix<Coefficient> out;
  right.multiplyOnTheLeft(buffer, out);
  return out;
}

template <typename Coefficient>
void Matrix<Coefficient>::conjugationAction(
  const Matrix<Coefficient>& conjugateWith,
  const Matrix<Coefficient>& conjugateOn,
  Matrix<Coefficient>& out
) {
  out = conjugateOn ^ conjugateWith;
}

template <typename Coefficient>
void Matrix<Coefficient>::multiplyOnTheLeft(
  const Matrix<Coefficient>& standsOnTheLeft, const Coefficient& ringZero
) {
  Matrix<Coefficient> result;
  this->multiplyOnTheLeft(standsOnTheLeft, result, ringZero);
  this->operator=(result);
}

template <typename Coefficient>
void Matrix<Coefficient>::multiplyOnTheLeft(
  const Matrix<Coefficient>& standsOnTheLeft,
  Matrix<Coefficient>& output,
  const Coefficient& ringZero
) const {
  if (&output == this || &output == &standsOnTheLeft) {
    Matrix<Coefficient> thisCopy = *this;
    Matrix<Coefficient> standsOnTheLeftCopy = standsOnTheLeft;
    thisCopy.multiplyOnTheLeft(standsOnTheLeftCopy, output, ringZero);
    return;
  }
  if (this->numberOfRows != standsOnTheLeft.numberOfColumns) {
    global.fatal
    << "Attempt to multiply a matrix with "
    << standsOnTheLeft.numberOfColumns
    << " columns by a matrix with "
    << this->numberOfRows
    << " rows. "
    << global.fatal;
  }
  Coefficient coefficient;
  output.initialize(standsOnTheLeft.numberOfRows, this->numberOfColumns);
  for (int i = 0; i < standsOnTheLeft.numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      output.elements[i][j] = ringZero;
      for (int k = 0; k < this->numberOfRows; k ++) {
        coefficient = standsOnTheLeft(i, k);
        coefficient *= this->elements[k][j];
        output.elements[i][j] += coefficient;
      }
    }
  }
}

template <typename Coefficient>
std::string Matrix<Coefficient>::toStringWithBlocks(List<int>& blocks) {
  std::stringstream out;
  std::string currentString;
  out << "\\left(\\begin{array}{";
  int offset = 0;
  int blockIndex = 0;
  for (int j = 0; j < this->numberOfColumns; j ++) {
    out << "c";
    offset ++;
    if (offset == blocks.objects[blockIndex]) {
      offset = 0;
      blockIndex ++;
      if (j != this->numberOfColumns - 1) {
        out << "|";
      }
    }
  }
  out << "}";
  offset = 0;
  blockIndex = 0;
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      currentString = this->elements[i][j].toString();
      out << currentString;
      if (j != this->numberOfColumns - 1) {
        out << " & ";
      }
    }
    out << "\\\\\n";
    offset ++;
    if (offset == blocks.objects[blockIndex]) {
      offset = 0;
      blockIndex ++;
      if (i != this->numberOfColumns - 1) {
        out << "\\hline";
      }
    }
  }
  out << "\\end{array}\\right)";
  return out.str();
}

template <typename Coefficient>
void Matrix<Coefficient>::assignDirectSum(
  Matrix<Coefficient>& m1, Matrix<Coefficient>& m2
) {
  if (this == &m1 || this == &m2) {
    global.fatal
    << "In assignDirectSum: output not allowed "
    << "to equal any of the inputs. "
    << global.fatal;
  }
  this->resize(
    m1.numberOfRows + m2.numberOfRows,
    m1.numberOfColumns + m2.numberOfColumns,
    false
  );
  this->makeZero();
  for (int i = 0; i < m1.numberOfRows; i ++) {
    for (int j = 0; j < m1.numberOfColumns; j ++) {
      this->elements[i][j] = m1.elements[i][j];
    }
  }
  for (int i = 0; i < m2.numberOfRows; i ++) {
    for (int j = 0; j < m2.numberOfColumns; j ++) {
      this->elements[i + m1.numberOfRows][j + m1.numberOfColumns] =
      m2.elements[i][j];
    }
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::assignTensorProduct(
  const Matrix<Coefficient>& left, const Matrix<Coefficient>& right
) {
  // handle lazy programmers:
  if (this == &left || this == &right) {
    Matrix<Coefficient> leftCopy = left;
    Matrix<Coefficient> rightCopy = right;
    this->assignTensorProduct(leftCopy, rightCopy);
    return;
  }
  this->resize(
    left.numberOfRows * right.numberOfRows,
    left.numberOfColumns * right.numberOfColumns,
    false
  );
  int sr = right.numberOfRows;
  int sc = right.numberOfColumns;
  // The basis of the tensor product vector space MUST be
  // in the SAME order as the one used by MatrixTensor::assignTensorProduct.
  // indexing. Let the first vector space have basis
  // v_1, ..., v_k, the second: w_1, ..., w_m.
  // Then the basis of the tensor product is given
  // in the order: v_1\otimes w_1, ..., v_1\otimes w_m,
  // ..., v_k\otimes w_1, ..., v_k\otimes w_m
  for (int iv = 0; iv < left.numberOfRows; iv ++) {
    for (int iw = 0; iw < right.numberOfRows; iw ++) {
      for (int jv = 0; jv < left.numberOfColumns; jv ++) {
        for (int jw = 0; jw < right.numberOfColumns; jw ++) {
          this->elements[iv * sr + iw][jv * sc + jw] =
          left.elements[iv][jv] * right.elements[iw][jw];
        }
      }
    }
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::directSumWith(
  const Matrix<Coefficient>& m2, const Coefficient& ringZero
) {
  int oldNumberOfRows = this->numberOfRows;
  int oldNumberOfColumns = this->numberOfColumns;
  this->resize(
    this->numberOfRows + m2.numberOfRows,
    this->numberOfColumns + m2.numberOfColumns,
    true
  );
  for (int i = 0; i < m2.numberOfRows; i ++) {
    for (int j = 0; j < m2.numberOfColumns; j ++) {
      this->elements[i + oldNumberOfRows][j + oldNumberOfColumns] =
      m2.elements[i][j];
    }
    for (int j = 0; j < oldNumberOfColumns; j ++) {
      this->elements[i + oldNumberOfRows][j] = ringZero;
    }
  }
  for (int j = 0; j < oldNumberOfRows; j ++) {
    for (int i = oldNumberOfColumns; i < this->numberOfColumns; i ++) {
      this->elements[j][i] = ringZero;
    }
  }
}

template <typename Coefficient>
int Matrix<Coefficient>::findPivot(int columnIndex, int rowStartIndex) {
  for (int i = rowStartIndex; i < this->numberOfRows; i ++) {
    if (!this->elements[i][columnIndex].isEqualToZero()) {
      return i;
    }
  }
  return - 1;
}

template <typename Coefficient>
void Matrix<Coefficient>::subtractRows(
  int indexRowWeSubtractFrom,
  int indexSubtracted,
  int startColumnIndex,
  const Coefficient& scalar
) {
  Coefficient coefficient;
  for (int i = startColumnIndex; i < this->numberOfColumns; i ++) {
    coefficient = this->elements[indexSubtracted][i];
    coefficient *= scalar;
    this->elements[indexRowWeSubtractFrom][i] -= coefficient;
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::rowTimesScalar(
  int rowIndex, const Coefficient& scalar
) {
  for (int i = 0; i < this->numberOfColumns; i ++) {
    this->elements[rowIndex][i] *= scalar;
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::switchRows(int row1, int row2) {
  if (row1 == row2) {
    return;
  }
  Coefficient * swapper = this->elements[row1];
  this->elements[row1] = this->elements[row2];
  this->elements[row2] = swapper;
}

template <typename Coefficient>
bool Matrix<Coefficient>::
solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(
  Matrix<Coefficient>& A, Matrix<Coefficient>& b, Matrix<Coefficient>& output
) {
  if (A.numberOfRows != b.numberOfRows) {
    global.fatal
    << "Number of matrix rows does not match number of vector entries. "
    << global.fatal;
  }
  Selection pivotPoints;
  A.gaussianEliminationByRows(&b, 0, &pivotPoints);
  return A.rowEchelonFormToLinearSystemSolution(pivotPoints, b, output);
}

template <typename Coefficient>
bool Matrix<Coefficient>::rowEchelonFormToLinearSystemSolution(
  Selection& inputPivotPoints,
  Matrix<Coefficient>& inputRightHandSide,
  Matrix<Coefficient>& outputSolution
) {
  if (
    inputPivotPoints.numberOfElements != this->numberOfColumns ||
    inputRightHandSide.numberOfColumns != 1 ||
    inputRightHandSide.numberOfRows != this->numberOfRows
  ) {
    global.fatal
    << "In rowEchelonFormToLinearSystemSolution: bad matrix dimensions. "
    << global.fatal;
  }
  outputSolution.initialize(this->numberOfColumns, 1);
  int numberOfPivots = 0;
  for (int i = 0; i < this->numberOfColumns; i ++) {
    if (inputPivotPoints.selected[i]) {
      outputSolution(i, 0) = inputRightHandSide(numberOfPivots, 0);
      numberOfPivots ++;
    } else {
      outputSolution(i, 0).makeZero();
    }
  }
  for (int i = numberOfPivots; i < this->numberOfRows; i ++) {
    if (!inputRightHandSide.elements[i][0].isEqualToZero()) {
      return false;
    }
  }
  return true;
}

template <typename Coefficient>
void Matrix<Coefficient>::gaussianEliminationByRowsNoRowSwapPivotPointsByRows(
  int firstNonProcessedRow,
  List<int>& outputPivotPointCols,
  Selection* outputNonPivotPoints__WarningSelectionNotInitialized
) {
  outputPivotPointCols.setSize(this->numberOfRows);
  Coefficient coefficient;
  for (int i = firstNonProcessedRow; i < this->numberOfRows; i ++) {
    int currentPivotCol = - 1;
    for (int j = 0; j < this->numberOfColumns; j ++) {
      if (!this->elements[i][j].isEqualToZero()) {
        currentPivotCol = j;
        break;
      }
    }
    outputPivotPointCols.objects[i] = currentPivotCol;
    if (currentPivotCol != - 1) {
      coefficient = this->elements[i][currentPivotCol];
      coefficient.invert();
      this->rowTimesScalar(i, coefficient);
      for (int j = 0; j < this->numberOfRows; j ++) {
        if (i != j) {
          if (!this->elements[j][i].isEqualToZero()) {
            coefficient.assign(this->elements[j][i]);
            this->subtractRows(j, i, 0, coefficient);
          }
        }
      }
    }
  }
  if (outputNonPivotPoints__WarningSelectionNotInitialized != nullptr) {
    for (int i = 0; i < this->numberOfColumns; i ++) {
      outputNonPivotPoints__WarningSelectionNotInitialized->selected[i] = true;
    }
    for (int i = 0; i < this->numberOfRows; i ++) {
      if (outputPivotPointCols.objects[i] != - 1) {
        outputNonPivotPoints__WarningSelectionNotInitialized->selected[
          outputPivotPointCols.objects[i]
        ] =
        false;
      }
    }
    outputNonPivotPoints__WarningSelectionNotInitialized->
    computeIndicesFromSelection();
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::makeZero(const Coefficient& ringZero) {
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      this->elements[i][j] = ringZero;
    }
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::makeIdentity(
  const Matrix<Coefficient>& prototype,
  const Coefficient& ringZero,
  const Coefficient& ringOne
) {
  this->initialize(prototype.numberOfRows, prototype.numberOfColumns);
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      this->elements[i][j] = i == j ? ringOne : ringZero;
    }
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::initialize(
  int desiredNumberOfRows, int desiredNumberOfColumns
) {
  this->resize(desiredNumberOfRows, desiredNumberOfColumns, false);
}

class FormatExpressions {
public:
  // alphabetBases must contain at least two elements
  std::string chevalleyGGeneratorLetter;
  std::string chevalleyHGeneratorLetter;
  std::string fundamentalWeightLetter;
  std::string polynomialDefaultLetter;
  std::string weylAlgebraDefaultLetter;
  std::string customPlusSign;
  std::string customCoefficientMonomialSeparator;
  std::string finiteDimensionalRepresentationLetter;
  std::string simpleRootLetter;
  List<std::string> polynomialAlphabet;
  List<std::string> weylAlgebraLetters;
  List<std::string> vectorSpaceEiBasisNames;
  Rational ambientCartanSymmetricInverseScale;
  int extraLinesCounterLatex;
  int numberOfAmpersandsPerNewLineForLaTeX;
  int maximumLineLength;
  int maximumMatrixLineLength;
  int maximumMatrixDisplayedRows;
  int matrixColumnVerticalLineIndex;
  static const int expressionLineBreak = 50;
  bool flagDontCollalpseProductsByUnits;
  bool flagPassCustomCoefficientMonomialSeparatorToCoefficients;
  bool flagMakingExpressionTableWithLatex;
  bool flagUseLatex;
  bool flagUsePmatrix;
  bool flagUsePNG;
  bool flagUseHTML;
  bool flagDynkinTypeDontUsePlusAndExponent;
  bool flagUseCalculatorFormatForUEOrdered;
  bool flagQuasiDiffOpCombineWeylPart;
  bool flagExpressionIsFinal;
  bool flagIncludeMutableInformation;
  bool flagExpressionNewLineAllowed;
  bool flagIncludeExtraHtmlDescriptionsInPlots;
  bool flagIncludeLieAlgebraTypes;
  bool flagUseReflectionNotation;
  bool flagCandidateSubalgebraShortReportOnly;
  bool flagSupressDynkinIndexOne;
  bool flagFormatWeightAsVectorSpaceIndex;
  bool flagFormatMatrixAsLinearSystem;
  bool flagUseFrac;
  bool flagSuppressOneIn1overXtimesY;
  bool flagSuppressLongMatrices;
  bool flagLatexDetailsInHtml;
  bool flagUseQuotes;
  bool flagSuppressModP;
  bool flagIsInNumerator;
  std::string suffixLinearCombination;
  char ambientWeylLetter;
  List<MonomialPolynomial>::Comparator monomialOrder;
  template <typename TemplateMonomial>
  typename List<TemplateMonomial>::Comparator* getMonomialOrder();
  std::string getPolynomialLetter(int index) const;
  FormatExpressions();
  static FormatExpressions* defaultFormat();
  void makePolynomialAlphabetLetters(
    const std::string& inputDefaultLetter, int letterCount
  );
  void makeAlphabetXYZUW();
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

template <class SourceCoefficient, class TargetCoefficient>
class CoefficientConverter {
public:
  virtual bool convert(
    const SourceCoefficient& input, TargetCoefficient& output
  ) = 0;
};

template <class TemplateMonomial, class Coefficient>
std::ostream& operator<<(
  std::ostream& output,
  const LinearCombination<TemplateMonomial, Coefficient>& collection
);
template <class TemplateMonomial, class Coefficient>
class LinearCombination {
private:
  void addOnTop(const MonomialPolynomial& tempP);
  // <-to guard the now unsafe base class method
  void clear();
  // <-to guard the now unsafe base class method
  friend std::ostream& operator<<<TemplateMonomial, Coefficient>(
    std::ostream& output,
    const LinearCombination<TemplateMonomial, Coefficient>& collection
  );
  std::string getTermString(
    const Coefficient& coefficient,
    const TemplateMonomial& monomial,
    FormatExpressions* format,
    const std::string& customCoefficientMonomialSeparator
  ) const;
public:
  HashedList<TemplateMonomial> monomials;
  List<Coefficient> coefficients;
  bool flagDeallocated;
  LinearCombination() {
    this->flagDeallocated = false;
  }
  LinearCombination(const LinearCombination& other) {
    this->flagDeallocated = false;
    this->operator=(other);
  }
  void operator=(const LinearCombination& other) {
    this->monomials = other.monomials;
    this->coefficients = other.coefficients;
  }
  ~LinearCombination() {
    this->flagDeallocated = true;
  }
  bool needsParenthesisForMultiplication(FormatExpressions* unused = nullptr)
  const {
    (void) unused;
    return this->size() > 1;
  }
  // Same as toString but, if the polynomial is too long and a line break is
  // needed,
  // it will be enclosed in a \begin{array} ... \end{array} pair.
  std::string toStringWithPossibleLineBreak(
    FormatExpressions* format = nullptr, int* outputNumberOfLines = nullptr
  ) const;
  std::string toString(
    FormatExpressions* format = nullptr, int* outputNumberOfLines = nullptr
  ) const;
  // Same as toString but uses a default alphabet "x", "y", ...
  // for the first few variables.
  std::string toStringPretty() const {
    FormatExpressions format;
    format.makeAlphabetXYZUW();
    return this->toString(&format);
  }
  int size() const {
    return this->monomials.size;
  }
  // The hash function of Monomial collection must return the same value for
  // monomial collections whose monomials are permuted.
  static unsigned int hashFunction(
    const LinearCombination<TemplateMonomial, Coefficient>& input
  ) {
    unsigned int result = 0;
    for (int i = 0; i < input.size(); i ++) {
      result += input.coefficients[i].hashFunction() * input[i].hashFunction();
    }
    return result;
  }
  void quickSortAscending(
    typename List<TemplateMonomial>::Comparator* order = nullptr
  ) {
    List<TemplateMonomial> sortedMonomials = this->monomials;
    sortedMonomials.quickSortAscending(order, &this->coefficients);
    this->monomials = sortedMonomials;
  }
  void quickSortDescending(
    typename List<TemplateMonomial>::Comparator* order = nullptr
  ) {
    List<TemplateMonomial> sortedMonomials = this->monomials;
    sortedMonomials.quickSortDescending(order, &this->coefficients);
    this->monomials = sortedMonomials;
  }
  void getVectorMonomialsAscending(Vector<Coefficient>& result);
  void getVectorMonomialsDescending(Vector<Coefficient>& result);
  void popMonomial(int index) {
    this->monomials.removeIndexSwapWithLast(index);
    this->coefficients.removeIndexSwapWithLast(index);
  }
  void addOtherTimesConst(
    LinearCombination<TemplateMonomial, Coefficient>& other,
    const Coefficient& coefficient
  );
  void popMonomial(
    int index, TemplateMonomial& outputMonomial, Coefficient& outputCoefficient
  ) {
    outputMonomial = (*this)[index];
    outputCoefficient = this->coefficients[index];
    this->monomials.removeIndexSwapWithLast(index);
    this->coefficients.removeIndexSwapWithLast(index);
  }
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  Coefficient getCoefficientsSum() const {
    Coefficient result = 0;
    for (int i = 0; i < this->coefficients.size; i ++) {
      result += this->coefficients[i];
    }
    return result;
  }
  void addMonomial(
    const TemplateMonomial& monomial, const Coefficient& coefficient
  ) {
    this->cleanupMonomialIndex(
      this->addMonomialNoCoefficientCleanUpReturnIndex(monomial, coefficient)
    );
  }
  void getMinimalMonomial(
    TemplateMonomial& outputMonomial, Coefficient& outputCoefficient
  ) const {
    if (this->isEqualToZero()) {
      global.fatal
      << "Calling getMinimalMonomial "
      << "on a zero monomial collection is forbidden. "
      << global.fatal;
    }
    outputMonomial = (*this)[0];
    outputCoefficient = this->coefficients[0];
    for (int i = 1; i < this->size(); i ++) {
      if (outputMonomial > (*this)[i]) {
        outputMonomial = (*this)[i];
        outputCoefficient = this->coefficients[i];
      }
    }
  }
  Coefficient getLeadingCoefficient(
    const typename List<TemplateMonomial>::Comparator* monomialOrder
  ) {
    Coefficient result;
    this->getIndexLeadingMonomial(nullptr, &result, monomialOrder);
    return result;
  }
  TemplateMonomial getLeadingMonomial(
    const typename List<TemplateMonomial>::Comparator* monomialOrder
  ) {
    TemplateMonomial result;
    this->getIndexLeadingMonomial(&result, nullptr, monomialOrder);
    return result;
  }
  int getIndexLeadingMonomial(
    TemplateMonomial* outputMonomial,
    Coefficient* outputCoefficient,
    const typename List<TemplateMonomial>::Comparator* monomialOrder = nullptr
  ) const {
    if (this->size() == 0) {
      return - 1;
    }
    int result = 0;
    if (monomialOrder != nullptr) {
      if (monomialOrder->leftGreaterThanRight == nullptr) {
        monomialOrder = nullptr;
      }
    }
    for (int i = 1; i < this->size(); i ++) {
      if (monomialOrder != nullptr) {
        if (monomialOrder->greaterThan((*this)[i], this->monomials[result])) {
          result = i;
        }
      } else {
        if ((*this)[i] > this->monomials[result]) {
          result = i;
        }
      }
    }
    if (outputMonomial != nullptr) {
      *outputMonomial = this->monomials[result];
    }
    if (outputCoefficient != nullptr) {
      *outputCoefficient = this->coefficients[result];
    }
    return result;
  }
  bool cleanupMonomialIndex(int index) {
    if (index == - 1) {
      return false;
    }
    // Note: operator != is not required to be defined
    // for our coefficients; operator == is.
    bool coefficientIsZero = (this->coefficients[index] == 0);
    if (!coefficientIsZero) {
      return false;
    }
    if (this->flagDeallocated) {
      global.fatal
      << "Use after free or race condition on monomial collection. "
      << global.fatal;
    }
    bool oldFlagDeallocated = this->flagDeallocated;
    this->flagDeallocated = true;
    this->monomials.removeIndexSwapWithLast(index);
    this->coefficients.removeIndexSwapWithLast(index);
    this->flagDeallocated = oldFlagDeallocated;
    return true;
  }
  int addMonomialNoCoefficientCleanUpReturnIndex(
    const TemplateMonomial& inputMonomial, const Coefficient& inputCoefficient
  );
  // Carries out sparse gaussian elimination.
  // When outputAccumulateNumberOfMonomialOperations, the number of monomial
  // operations
  // carried out by the algorithm will be accumulated into that variable;
  // the variable will not be initialized.
  template <class LinearCombinationTemplate>
  static void gaussianEliminationByRows(
    List<LinearCombinationTemplate>& toBeEliminated,
    bool* madeARowSwitch = nullptr,
    HashedList<TemplateMonomial>* seedMonomials = nullptr,
    Matrix<Coefficient>* carbonCopyMatrix = nullptr,
    int* outputAccumulateNumberOfMonomialOperations = nullptr,
    List<LinearCombinationTemplate>* carbonCopyList = nullptr
  );
  template <class LinearCombinationTemplate>
  static void intersectVectorSpaces(
    const List<LinearCombinationTemplate>& vectorSpace1,
    const List<LinearCombinationTemplate>& vectorSpace2,
    List<LinearCombinationTemplate>& outputIntersection,
    HashedList<TemplateMonomial>* seedMonomials = nullptr
  );
  template <class LinearCombinationTemplate>
  static int getRankIntersectionVectorSpaces(
    const List<LinearCombinationTemplate>& left,
    const List<LinearCombinationTemplate>& right,
    HashedList<TemplateMonomial>* seedMonomials = nullptr
  ) {
    List<LinearCombinationTemplate> listCopy = left;
    listCopy.addListOnTop(right);
    int leftRank = LinearCombination::getRankElementSpan(left, seedMonomials);
    int rightRank =
    LinearCombination::getRankElementSpan(right, seedMonomials);
    return
    leftRank +
    rightRank -
    LinearCombination<TemplateMonomial, Coefficient>::getRankElementSpan(
      listCopy, seedMonomials
    );
  }
  template <class LinearCombinationTemplate>
  static bool vectorSpacesIntersectionIsNonTrivial(
    const List<LinearCombinationTemplate>& left,
    const List<LinearCombinationTemplate>& right,
    HashedList<TemplateMonomial>* seedMonomials = nullptr
  ) {
    int intersectionRank =
    LinearCombination<TemplateMonomial, Coefficient>::
    getRankIntersectionVectorSpaces(left, right, seedMonomials);
    return intersectionRank != 0;
  }
  template <class LinearCombinationTemplate>
  static bool linearSpanContains(
    const List<LinearCombinationTemplate>& inputList,
    const LinearCombinationTemplate& input,
    HashedList<TemplateMonomial>* seedMonomials = nullptr
  ) {
    List<LinearCombinationTemplate> listCopy = inputList;
    LinearCombination<TemplateMonomial, Coefficient>::
    gaussianEliminationByRowsDeleteZeroRows(listCopy, 0, seedMonomials);
    int startSpanSize = listCopy.size;
    listCopy.addOnTop(input);
    LinearCombination<TemplateMonomial, Coefficient>::
    gaussianEliminationByRowsDeleteZeroRows(listCopy, 0, seedMonomials);
    return listCopy.size == startSpanSize;
  }
  template <class LinearCombinationTemplate>
  static bool linearSpanContainsGetFirstLinearCombination(
    const List<LinearCombinationTemplate>& inputList,
    const LinearCombinationTemplate& input,
    Vector<Coefficient>& outputFirstLinearCombination,
    HashedList<TemplateMonomial>* seedMonomials = nullptr
  );
  bool hasRationalCoefficients(
    LinearCombination<TemplateMonomial, Rational>* outputConversionToRationals
    =
    nullptr
  ) {
    Rational unused;
    Rational* coefficient = nullptr;
    if (outputConversionToRationals != nullptr) {
      coefficient = &unused;
      outputConversionToRationals->makeZero();
    }
    for (int i = 0; i < this->size(); i ++) {
      if (!this->coefficients[i].isRational(coefficient)) {
        return false;
      } else {
        if (outputConversionToRationals != nullptr) {
          outputConversionToRationals->addMonomial((*this)[i], *coefficient);
        }
      }
    }
    return true;
  }
  template <class LinearCombinationTemplate>
  static int getRankElementSpan(
    const List<LinearCombinationTemplate>& inputList,
    HashedList<TemplateMonomial>* seedMonomials = nullptr
  ) {
    List<LinearCombinationTemplate> listCopy = inputList;
    LinearCombination<TemplateMonomial, Coefficient>::
    gaussianEliminationByRowsDeleteZeroRows(listCopy, 0, seedMonomials);
    return listCopy.size;
  }
  template <class LinearCombinationTemplate>
  static void gaussianEliminationByRowsDeleteZeroRows(
    List<LinearCombinationTemplate>& inputList,
    bool* madeARowSwitch = nullptr,
    HashedList<TemplateMonomial>* seedMonomials = nullptr
  ) {
    LinearCombinationTemplate::gaussianEliminationByRows(
      inputList, madeARowSwitch, seedMonomials
    );
    for (int j = inputList.size - 1; j >= 0; j --) {
      if (inputList[j].isEqualToZero()) {
        inputList.size --;
      }
    }
  }
  int subtractMonomialNoCoefficientCleanUpReturnIndex(
    const TemplateMonomial& inputMonomial, const Coefficient& inputCoefficient
  );
  void cleanUpZeroCoefficients() {
    for (int i = 0; i < this->size; i ++) {
      if (this->cleanupMonomialIndex(i)) {
        i --;
      }
    }
  }
  // Carries out an in-place substitution
  // in the coefficients of the linear combination.
  // The coefficient type is required to have a
  // substitute function that carries out a substitution in-place.
  template <class SubstitutionClass>
  void substituteInCoefficients(const SubstitutionClass& substitution) {
    STACK_TRACE("Polynomial::substituteInCoefficients");
    Coefficient newCoefficient;
    for (int i = 0; i < this->size(); i ++) {
      newCoefficient = this->coefficients[i];
      newCoefficient.substitute(substitution, 1);
      if (newCoefficient.isEqualToZero()) {
        this->popMonomial(i);
        i --;
        continue;
      }
      this->coefficients[i] = newCoefficient;
    }
  }
  void subtractMonomial(
    const TemplateMonomial& inputMonomial, const Coefficient& inputCoefficient
  ) {
    this->cleanupMonomialIndex(
      this->subtractMonomialNoCoefficientCleanUpReturnIndex(
        inputMonomial, inputCoefficient
      )
    );
  }
  Coefficient getCoefficientOf(const TemplateMonomial& inputMonomial) const {
    int index = this->monomials.getIndex(inputMonomial);
    if (index == - 1) {
      if (this->coefficients.size > 0) {
        return this->coefficients[0].zero();
      }
      return Coefficient::zeroStatic();
    }
    return this->coefficients[index];
  }
  bool operator>(const LinearCombination<TemplateMonomial, Coefficient>& other)
  const {
    if (this->size() > other.size()) {
      return true;
    }
    if (this->size() < other.size()) {
      return false;
    }
    LinearCombination<TemplateMonomial, Coefficient> leftCopy = *this;
    LinearCombination<TemplateMonomial, Coefficient> rightCopy = other;
    leftCopy.quickSortAscending();
    rightCopy.quickSortAscending();
    for (int i = leftCopy.size() - 1; i >= 0; i --) {
      if (leftCopy[i] > rightCopy[i]) {
        return true;
      }
      if (rightCopy[i] > leftCopy[i]) {
        return false;
      }
      if (leftCopy.coefficients[i] > rightCopy.coefficients[i]) {
        return true;
      }
      if (rightCopy.coefficients[i] > leftCopy.coefficients[i]) {
        return false;
      }
    }
    return false;
  }
  void checkFlagDeallocated() const {
    if (
      this->monomials.flagDeallocated ||
      this->coefficients.flagDeallocated ||
      this->flagDeallocated
    ) {
      global.fatal
      << "Use after free of monomial collection. "
      << global.fatal;
    }
  }
  void checkConsistencyGrandMaster() const {
    this->checkConsistency();
    this->monomials.grandMasterConsistencyCheck();
    for (int i = 0; i < this->size(); i ++) {
      this->coefficients[i].checkConsistency();
    }
  }
  void checkConsistency() const {
    this->checkFlagDeallocated();
    this->checkNumberOfCoefficientsConsistency();
  }
  // Returns the number by which the linear combination was multiplied.
  // Returns zero if the input is zero.
  Coefficient scaleNormalizeLeadingMonomial(
    const typename List<TemplateMonomial>::Comparator* monomialOrder
  ) {
    STACK_TRACE("Polynomial::scaleNormalizeLeadingMonomial");
    if (this->isEqualToZero()) {
      return Coefficient();
    }
    int indexLeadingMonomial =
    this->getIndexLeadingMonomial(nullptr, nullptr, monomialOrder);
    Coefficient result =
    Coefficient::scaleNormalizeIndex(this->coefficients, indexLeadingMonomial);
    return result;
  }
  static std::string getBlendCoefficientAndMonomial(
    const TemplateMonomial& inputMonomial,
    Coefficient& inputCoefficient,
    bool addPlusToFront,
    FormatExpressions* format = nullptr
  );
  void checkNumberOfCoefficientsConsistency() const {
    if (this->coefficients.size != this->monomials.size) {
      global.fatal
      << "A monomial collection has "
      << this->monomials.size
      << " monomials but "
      << this->coefficients.size
      << " coefficients. "
      << global.fatal;
    }
  }
  bool isEqualToZero() const;
  void makeZero() {
    this->monomials.clear();
    this->coefficients.setSize(0);
  }
  int minimalNumberOfVariables() const {
    int result = 0;
    for (int i = 0; i < this->size(); i ++) {
      result =
      MathRoutines::maximum(result, (*this)[i].minimalNumberOfVariables());
    }
    return result;
  }
  bool isSmallInteger(int* whichInteger = nullptr) const;
  bool isInteger(LargeInteger* whichInteger = nullptr) const;
  void setExpectedSize(int expectedSize) {
    this->monomials.setExpectedSize(expectedSize);
    this->coefficients.setExpectedSize(expectedSize);
  }
  bool hasGreaterThanOrEqualToMonomial(TemplateMonomial& m, int& whichIndex);
  void writeToFile(std::fstream& output);
  void clearDenominators(Rational& output) {
    output.makeOne();
    Rational scale;
    for (int i = 0; i < this->size(); i ++) {
      if (!this->coefficients[i].isInteger()) {
        scale = this->coefficients[i].getDenominator();
        *this *= scale;
        output *= scale;
      }
    }
  }
  bool readFromFile(std::fstream& input);
  bool operator!=(
    const LinearCombination<TemplateMonomial, Coefficient>& other
  ) const {
    return !(*this == other);
  }
  bool operator==(
    const LinearCombination<TemplateMonomial, Coefficient>& other
  ) const;
  bool operator==(int x) const;
  void operator+=(
    const LinearCombination<TemplateMonomial, Coefficient>& other
  );
  LinearCombination<TemplateMonomial, Coefficient> operator*(
    const Coefficient& other
  ) const {
    LinearCombination<TemplateMonomial, Coefficient> result = *this;
    result *= other;
    return result;
  }
  LinearCombination<TemplateMonomial, Coefficient> operator-(
    const LinearCombination<TemplateMonomial, Coefficient>& other
  ) const {
    LinearCombination<TemplateMonomial, Coefficient> output = *this;
    output -= other;
    return output;
  }
  LinearCombination<TemplateMonomial, Coefficient> operator+(
    const LinearCombination<TemplateMonomial, Coefficient>& other
  ) const {
    LinearCombination<TemplateMonomial, Coefficient> output = *this;
    output += other;
    return output;
  }
  void makeConstant(const Coefficient& coefficient) {
    TemplateMonomial monomial;
    monomial.makeOne();
    this->addMonomial(monomial, coefficient);
  }
  void makeOne() {
    this->makeConstant(1);
  }
  void operator+=(const Coefficient& other) {
    TemplateMonomial monomial;
    monomial.makeOne();
    this->addMonomial(monomial, other);
  }
  void operator-=(const Coefficient& other) {
    TemplateMonomial monomial;
    monomial.makeOne();
    this->subtractMonomial(monomial, other);
  }
  // Assigns a linear combination with one type of
  // coefficients to a linear combination with the same
  // monomials but a different coefficient type, using
  // a given conversion function.
  template <class OtherType>
  bool assignOtherType(
    const LinearCombination<TemplateMonomial, OtherType>& other,
    CoefficientConverter<OtherType, Coefficient>& converter
  ) {
    this->monomials = other.monomials;
    this->coefficients.setSize(other.size());
    for (int i = 0; i < other.size(); i ++) {
      if (!converter.convert(other.coefficients[i], this->coefficients[i])) {
        // Conversion failed.
        return false;
      }
    }
    return true;
  }
  void operator-=(
    const LinearCombination<TemplateMonomial, Coefficient>& other
  ) {
    this->subtractOtherTimesCoefficient(other);
  }
  void subtractOtherTimesCoefficient(
    const LinearCombination<TemplateMonomial, Coefficient>& other,
    Coefficient* inputCoefficient = nullptr
  );
  template <class otherType>
  void operator/=(const otherType& other) {
    if (other == 0) {
      global.fatal
      << "A LinearCombination division "
      << "by zero has been requested: division by zero error should "
      << "be handled before calling operator/=. "
      << global.fatal;
      return;
    }
    for (int i = 0; i < this->coefficients.size; i ++) {
      this->coefficients[i] /= other;
    }
  }
  template <class OtherType>
  void operator*=(const OtherType& other) {
    for (int i = 0; i < this->coefficients.size; i ++) {
      this->coefficients[i] *= other;
      if (i == 0) {
        // To avoid implicit conversion problems,
        // we make the zero check at the first cycle.
        if (this->coefficients[i].isEqualToZero()) {
          this->makeZero();
          return;
        }
      }
    }
  }
  const TemplateMonomial& operator[](int index) const {
    return this->monomials[index];
  }
  void operator=(const TemplateMonomial& other) {
    TemplateMonomial otherCopy = other;
    // <- In case other is contained as a
    // monomial in this LinearCombination
    // and gets destroyed when this->makeZero() is called.
    // This shouldn't yield a measurable slowdown.
    // Further this function is to be used for non-time critical operations.
    this->makeZero();
    this->addMonomial(otherCopy, 1);
  }
  template <class OtherType>
  void operator=(const LinearCombination<TemplateMonomial, OtherType>& other) {
    this->coefficients = other.coefficients;
    this->monomials = other.monomials;
  }
};

// Monomial used to implement sparse vectors.
class MonomialVector {
  friend std::ostream& operator<<(
    std::ostream& output, const MonomialVector& monomial
  ) {
    output << monomial.toString();
    return output;
  }
public:
  int monomialIndex;
  MonomialVector(): monomialIndex(- 1) {}
  MonomialVector(int inputIndex): monomialIndex(inputIndex) {}
  std::string toString(FormatExpressions* format = nullptr) const;
  unsigned int hashFunction() const {
    return static_cast<unsigned int>(this->monomialIndex);
  }
  static unsigned int hashFunction(const MonomialVector& input) {
    return input.hashFunction();
  }
  bool operator==(const MonomialVector& other) const {
    return this->monomialIndex == other.monomialIndex;
  }
  void makeEi(int inputIndex) {
    this->monomialIndex = inputIndex;
  }
  bool operator>(const MonomialVector& other) const {
    return this->monomialIndex > other.monomialIndex;
  }
};

template <class Coefficient>
class VectorSparse: public LinearCombination<MonomialVector, Coefficient> {
public:
  void makeEi(int nonZeroIndex, const Coefficient& coefficient = 1) {
    this->makeZero();
    MonomialVector monomial;
    monomial.makeEi(nonZeroIndex);
    this->addMonomial(monomial, coefficient);
  }
  int getLargestParticipatingBasisIndex() {
    int result = - 1;
    for (int i = 0; i < this->size(); i ++) {
      result = MathRoutines::maximum(result, (*this)[i].monomialIndex);
    }
    return result;
  }
  bool operator>(const VectorSparse<Coefficient>& other) const {
    return
    this->::LinearCombination<MonomialVector, Coefficient>::operator>(other);
  }
};

template <class TemplateMonomial, class Coefficient>
class ElementMonomialAlgebra: public LinearCombination<
  TemplateMonomial, Coefficient
> {
public:
  void multiplyBy(
    const ElementMonomialAlgebra<TemplateMonomial, Coefficient>& other,
    ElementMonomialAlgebra<TemplateMonomial, Coefficient>& output,
    ElementMonomialAlgebra<TemplateMonomial, Coefficient>& bufferPoly,
    TemplateMonomial& bufferMon
  ) const;
  void multiplyBy(
    const TemplateMonomial& other,
    const Coefficient& coefficient,
    ElementMonomialAlgebra<TemplateMonomial, Coefficient>& output,
    ElementMonomialAlgebra<TemplateMonomial, Coefficient>& bufferPoly,
    TemplateMonomial& bufferMon
  ) const;
  void multiplyBy(
    const TemplateMonomial& monomial,
    ElementMonomialAlgebra<TemplateMonomial, Coefficient>& output
  ) const;
  void multiplyBy(const MonomialPolynomial& other) {
    this->multiplyBy(other, *this);
  }
  void multiplyBy(
    const MonomialPolynomial& other, const Coefficient& coefficient
  ) {
    return this->multiplyBy(other, coefficient, *this);
  }
  void multiplyBy(
    const MonomialPolynomial& other,
    const Coefficient& coefficient,
    ElementMonomialAlgebra<TemplateMonomial, Coefficient>& output
  ) const {
    this->multiplyBy(other, output);
    output *= coefficient;
  }
  void multiplyOnTheLeft(
    const ElementMonomialAlgebra<TemplateMonomial, Coefficient>&
    standsOnTheLeft
  ) {
    ElementMonomialAlgebra<TemplateMonomial, Coefficient> matrix;
    ElementMonomialAlgebra<TemplateMonomial, Coefficient> bufferPolynomial;
    TemplateMonomial bufferMonomial;
    standsOnTheLeft.multiplyBy(
      *this, matrix, bufferPolynomial, bufferMonomial
    );
    *this = matrix;
  }
  void operator*=(
    const ElementMonomialAlgebra<TemplateMonomial, Coefficient>& other
  ) {
    ElementMonomialAlgebra<TemplateMonomial, Coefficient> bufferPolynomial;
    TemplateMonomial bufferMonomial;
    this->multiplyBy(other, *this, bufferPolynomial, bufferMonomial);
  }
  void operator*=(const Coefficient& other) {
    this->::LinearCombination<TemplateMonomial, Coefficient>::operator*=(
      other
    );
  }
  void raiseToPower(
    int d,
    ElementMonomialAlgebra<TemplateMonomial, Coefficient>& output,
    const Coefficient& ringUnit
  );
  void raiseToPower(int d) {
    if (d == 1) {
      return;
    }
    ElementMonomialAlgebra<TemplateMonomial, Coefficient> one;
    one.makeConstant(1);
    MathRoutines::raiseToPower(*this, d, one);
  }
};

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
  Coefficient constantTerm(const Coefficient& ringZero = 0) const {
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
    int numberOfVariables, const Coefficient& ringUnit = 1
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
  List<List<MonomialPolynomial> > intermediateHighlightedMons;
  List<MonomialPolynomial> intermediateHighestMonDivHighestMon;
  List<Coefficient> intermediateCoefficients;
  List<Polynomial<Coefficient> > intermediateSubtractands;
  Polynomial<Coefficient> startingPolynomial;
  int highlightAllMonsFinalRemainder;
  int firstIndexLatexSlide;
  List<List<int> > additionalHighlightRemainders;
  List<List<List<int> > > highlightMonsQuotients;
  List<List<List<int> > > highlightMonsRemainders;
  List<List<List<int> > > highlightMonsSubtrahends;
  List<List<List<int> > > highlightMonsDivisors;
  List<List<int> > fcAnswerMonsQuotients;
  List<List<int> > fcAnswerMonsRemainders;
  List<List<int> > fcAnswerMonsSubtrahends;
  List<List<int> > fcAnswerMonsDivisors;
  List<int> firstNonZeroIndicesPerIntermediateSubtracand;
  List<int> intermediateSelectedDivisors;
  List<int> uncoverAllMonsQuotients;
  List<int> uncoverAllMonsRemainders;
  List<int> uncoverAllMonsSubtrahends;
  List<int> uncoverAllMonsDivisors;
  List<int> uncoverMonsFinalRemainder;
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
    List<List<int> >* slidesToHighlightMon,
    List<int>* slidesToFcAnswer,
    List<int>* slidesToUncover,
    List<int>* slidesAdditionalHighlight,
    int slidesToUncoverAllMons,
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

class ArbitrarySubstitutionsProvider {
public:
  List<Rational> arbitrarySubstitutions;
  bool flagChooseSmallestIndexVariableFirst;
  int oneIsFirstArbitrarySubstitutionWhenRecursionDepthIsMultipleOf;
  ArbitrarySubstitutionsProvider();
  void computeArbitrarySubstitutions(int recursionDepth);
  int preferredVariableForArbitrarySubstitutionProvider(
    Selection& variablesToSolveFor, int bestHeursticCandidate
  );
};

template <class Coefficient>
class PolynomialSystem {
public:
  int numberOfSerreSystemComputations;
  int numberOfSerreVariablesOneGenerator;
  int recursionCounterSerreLikeSystem;
  int numberOfVariablesToSolveForStart;
  int numberOfVariablesToSolveForAfterReduction;
  bool flagTryDirectlySolutionOverAlgebraicClosure;
  bool flagUseMonomialBranchingOptimization;
  bool flagSystemProvenToHaveNoSolution;
  bool flagSystemProvenToHaveSolution;
  bool flagSystemSolvedOverBaseField;
  bool flagUsingAlgebraicClosure;
  List<Polynomial<Coefficient> > gaussianEliminatedSystem;
  AlgebraicClosureRationals* algebraicClosure;
  GroebnerBasisComputation<Coefficient> groebner;
  MemorySaving<PolynomialSystem<Coefficient> > computationUsedInRecursiveCalls;
  List<Coefficient> systemSolution;
  Selection solutionsFound;
  List<PolynomialSubstitution<Coefficient> > impliedSubstitutions;
  ArbitrarySubstitutionsProvider substitutionsProvider;
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
    const Rational& aValueToTryOnPreferredVariable
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

template <class TemplateMonomial, class Coefficient>
bool LinearCombination<TemplateMonomial, Coefficient>::operator==(int x) const {
  if (x == 0) {
    return this->size() == 0;
  }
  global.fatal
  << "This is either a programming error, "
  << "or an unforeseen use of operator==. "
  << "If the second is the case, an audit/careful "
  << "proofreading of the code calling this function is needed; "
  << "I am crashing just in case. "
  << global.fatal;
  return false;
}

template <class TemplateMonomial, class Coefficient>
bool LinearCombination<TemplateMonomial, Coefficient>::operator==(
  const LinearCombination<TemplateMonomial, Coefficient>& other
) const {
  if (this->coefficients.size != other.coefficients.size) {
    return false;
  }
  for (int i = 0; i < this->size(); i ++) {
    int indexInOther = other.monomials.getIndex((*this)[i]);
    if (indexInOther == - 1) {
      return false;
    }
    if (!(this->coefficients[i] == other.coefficients[indexInOther])) {
      return false;
    }
  }
  return true;
}

template <class TemplateMonomial, class Coefficient>
bool LinearCombination<TemplateMonomial, Coefficient>::isEqualToZero() const {
  return this->size() == 0;
}

template <class TemplateMonomial, class Coefficient>
bool LinearCombination<TemplateMonomial, Coefficient>::isInteger(
  LargeInteger* whichInteger
) const {
  if (this->size() > 1) {
    return false;
  }
  if (this->size() == 0) {
    if (whichInteger != nullptr) {
      *whichInteger = 0;
    }
    return true;
  }
  bool result = (*this)[0].isConstant();
  if (result) {
    result = this->coefficients[0].isInteger(whichInteger);
  }
  return result;
}

template <class TemplateMonomial, class Coefficient>
bool LinearCombination<TemplateMonomial, Coefficient>::isSmallInteger(
  int* whichInteger
) const {
  if (this->size() > 1) {
    return false;
  }
  if (this->size() == 0) {
    if (whichInteger != nullptr) {
      *whichInteger = 0;
    }
    return true;
  }
  bool result = (*this)[0].isConstant();
  if (result) {
    result = this->coefficients[0].isSmallInteger(whichInteger);
  }
  return result;
}

template <class TemplateMonomial, class Coefficient>
void LinearCombination<TemplateMonomial, Coefficient>::
getVectorMonomialsAscending(Vector<Coefficient>& result) {
  this->quickSortAscending();
  result = this->coefficients;
}

template <class TemplateMonomial, class Coefficient>
void LinearCombination<TemplateMonomial, Coefficient>::
getVectorMonomialsDescending(Vector<Coefficient>& result) {
  this->quickSortDescending();
  result = this->coefficients;
}

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

template <class TemplateMonomial, class Coefficient>
void LinearCombination<TemplateMonomial, Coefficient>::
subtractOtherTimesCoefficient(
  const LinearCombination<TemplateMonomial, Coefficient>& other,
  Coefficient* inputCoefficient
) {
  if (this == &other) {
    if (inputCoefficient == nullptr) {
      this->makeZero();
      return;
    }
    LinearCombination<TemplateMonomial, Coefficient> otherNew = other;
    this->subtractOtherTimesCoefficient(otherNew, inputCoefficient);
    return;
  }
  this->setExpectedSize(other.size() + this->size());
  Coefficient product;
  for (int i = 0; i < other.size(); i ++) {
    product = other.coefficients[i];
    if (inputCoefficient != nullptr) {
      product *= *inputCoefficient;
    }
    this->subtractMonomial(other[i], product);
  }
}

template <class TemplateMonomial, class Coefficient>
void LinearCombination<TemplateMonomial, Coefficient>::addOtherTimesConst(
  LinearCombination<TemplateMonomial, Coefficient>& other,
  const Coefficient& coefficient
) {
  this->setExpectedSize(other.size() + this->size());
  Coefficient product;
  for (int i = 0; i < other.size(); i ++) {
    product = coefficient;
    product *= other.coefficients[i];
    this->addMonomial(other[i], product);
  }
}

template <class TemplateMonomial, class Coefficient>
void LinearCombination<TemplateMonomial, Coefficient>::operator+=(
  const LinearCombination<TemplateMonomial, Coefficient>& other
) {
  this->setExpectedSize(other.size() + this->size());
  for (int i = 0; i < other.size(); i ++) {
    this->addMonomial(other[i], other.coefficients[i]);
  }
}

template <class TemplateMonomial, class Coefficient>
bool LinearCombination<TemplateMonomial, Coefficient>::
hasGreaterThanOrEqualToMonomial(TemplateMonomial& m, int& whichIndex) {
  for (int i = 0; i < this->size; i ++) {
    if (this->objects[i].IsGEQpartialOrder(m)) {
      whichIndex = i;
      return true;
    }
  }
  whichIndex = - 1;
  return false;
}

template <class TemplateMonomial, class Coefficient>
template <class LinearCombinationTemplate>
bool LinearCombination<TemplateMonomial, Coefficient>::
linearSpanContainsGetFirstLinearCombination(
  const List<LinearCombinationTemplate>& inputList,
  const LinearCombinationTemplate& input,
  Vector<Coefficient>& outputFirstLinearCombination,
  HashedList<TemplateMonomial>* seedMonomials
) {
  List<LinearCombinationTemplate> listCopy = inputList;
  Matrix<Coefficient> rowOperations;
  rowOperations.makeIdentityMatrix(inputList.size);
  LinearCombination<TemplateMonomial, Coefficient>::gaussianEliminationByRows(
    listCopy, 0, seedMonomials, &rowOperations
  );
  LinearCombinationTemplate remainderFromInput = input;
  TemplateMonomial monomial;
  Coefficient coefficientMinimialMonomial;
  Coefficient coefficientInRemainder;
  outputFirstLinearCombination.makeZero(listCopy.size);
  for (int i = 0; i < listCopy.size; i ++) {
    if (listCopy[i].isEqualToZero()) {
      break;
    }
    listCopy[i].getMinimalMonomial(monomial, coefficientMinimialMonomial);
    coefficientInRemainder = remainderFromInput.getCoefficientOf(monomial);
    outputFirstLinearCombination[i] = coefficientInRemainder;
    outputFirstLinearCombination[i] /= coefficientMinimialMonomial;
    remainderFromInput -= listCopy[i] * outputFirstLinearCombination[i];
  }
  if (!remainderFromInput.isEqualToZero()) {
    return false;
  }
  rowOperations.actMultiplyVectorRowOnTheRight(outputFirstLinearCombination);
  return true;
}

template <class TemplateMonomial, class Coefficient>
template <class LinearCombinationTemplate>
void LinearCombination<TemplateMonomial, Coefficient>::
gaussianEliminationByRows(
  List<LinearCombinationTemplate>& toBeEliminated,
  bool* madeARowSwitch,
  HashedList<TemplateMonomial>* seedMonomials,
  Matrix<Coefficient>* carbonCopyMatrix,
  int* outputAccumulateNumberOfMonomialOperations,
  List<LinearCombinationTemplate>* carbonCopyList
) {
  STACK_TRACE("LinearCombination::gaussianEliminationByRows");
  if (carbonCopyMatrix != 0) {
    if (carbonCopyMatrix->numberOfRows != toBeEliminated.size) {
      global.fatal
      << "Carbon copy matrix has "
      << carbonCopyMatrix->numberOfRows
      << " rows, while the gaussian-eliminated list has "
      << toBeEliminated.size
      << " elements; the two numbers must be the same!"
      << global.fatal;
    }
  }
  if (carbonCopyList != 0) {
    if (carbonCopyList->size != toBeEliminated.size) {
      global.fatal
      << "Carbon copy list has "
      << carbonCopyList->size
      << " elements, while the gaussian-eliminated list has "
      << toBeEliminated.size
      << " elements; the two numbers must be the same!"
      << global.fatal;
    }
  }
  MemorySaving<HashedList<TemplateMonomial> > bufferMonomials;
  HashedList<TemplateMonomial>& allMonomials = seedMonomials ==
  0 ? bufferMonomials.getElement() : *seedMonomials;
  if (seedMonomials == 0) {
    int topBoundNumberOfMonomials = 0;
    for (int i = 0; i < toBeEliminated.size; i ++) {
      topBoundNumberOfMonomials += toBeEliminated[i].size();
    }
    allMonomials.setExpectedSize(topBoundNumberOfMonomials);
  }
  for (int i = 0; i < toBeEliminated.size; i ++) {
    allMonomials.addOnTopNoRepetition(toBeEliminated[i].monomials);
  }
  allMonomials.quickSortAscending();
  FormatExpressions format;
  format.flagUseHTML = true;
  format.flagUseLatex = true;
  if (madeARowSwitch != nullptr) {
    *madeARowSwitch = false;
  }
  int currentRowIndex = 0;
  Coefficient accumulator;
  Coefficient negated;
  for (
    int i = 0; i < allMonomials.size && currentRowIndex < toBeEliminated.size;
    i ++
  ) {
    const TemplateMonomial& monomial = allMonomials[i];
    int goodRow = currentRowIndex;
    for (; goodRow < toBeEliminated.size; goodRow ++) {
      if (toBeEliminated[goodRow].monomials.contains(monomial)) {
        break;
      }
    }
    if (goodRow >= toBeEliminated.size) {
      continue;
    }
    if (currentRowIndex != goodRow) {
      toBeEliminated.swapTwoIndices(currentRowIndex, goodRow);
      if (carbonCopyList != 0) {
        carbonCopyList->swapTwoIndices(currentRowIndex, goodRow);
      }
      if (carbonCopyMatrix != 0) {
        carbonCopyMatrix->switchRows(currentRowIndex, goodRow);
      }
      if (madeARowSwitch != nullptr) {
        *madeARowSwitch = true;
      }
    }
    LinearCombination<TemplateMonomial, Coefficient>& currentPivot =
    toBeEliminated[currentRowIndex];
    int columnIndex = currentPivot.monomials.getIndex(monomial);
    if (columnIndex == - 1) {
      global.fatal
      << "An internal check at the "
      << "Gaussian elimination method for monomial collections fails. "
      << "Something is wrong. "
      << "Here is the list you wanted to perform Gaussian elimination upon. "
      << toBeEliminated.toString()
      << ". "
      << global.fatal;
    }
    accumulator = currentPivot.coefficients[columnIndex];
    accumulator.invert();
    currentPivot *= accumulator;
    if (carbonCopyMatrix != 0) {
      carbonCopyMatrix->rowTimesScalar(currentRowIndex, accumulator);
    }
    if (carbonCopyList != 0) {
      (*carbonCopyList)[currentRowIndex] *= accumulator;
    }
    for (int j = 0; j < toBeEliminated.size; j ++) {
      if (j == currentRowIndex) {
        continue;
      }
      LinearCombination<TemplateMonomial, Coefficient>& currentOther =
      toBeEliminated[j];
      int otherColumnIndex = currentOther.monomials.getIndex(monomial);
      if (otherColumnIndex == - 1) {
        continue;
      }
      accumulator = currentOther.coefficients[otherColumnIndex];
      currentOther.subtractOtherTimesCoefficient(currentPivot, &accumulator);
      if (outputAccumulateNumberOfMonomialOperations != nullptr) {
        *outputAccumulateNumberOfMonomialOperations += currentPivot.size();
      }
      if (carbonCopyList != 0) {
        (*carbonCopyList)[j].subtractOtherTimesCoefficient((*carbonCopyList)[
            currentRowIndex
          ],
          &accumulator
        );
      }
      if (carbonCopyMatrix != 0) {
        negated = accumulator;
        negated *= - 1;
        carbonCopyMatrix->addTwoRows(currentRowIndex, j, 0, negated);
      }
    }
    currentRowIndex ++;
  }
}

template <class TemplateMonomial, class Coefficient>
int LinearCombination<TemplateMonomial, Coefficient>::
addMonomialNoCoefficientCleanUpReturnIndex(
  const TemplateMonomial& inputMonomial, const Coefficient& inputCoefficient
) {
  this->checkConsistency();
  if (inputCoefficient == 0) {
    return - 1;
  }
  if (this->flagDeallocated) {
    global.fatal
    << "Use after free or race condition on monomial collection. "
    << global.fatal;
  }
  bool oldFlagDeallocated = this->flagDeallocated;
  this->flagDeallocated = true;
  int j = this->monomials.getIndex(inputMonomial);
  if (j >= this->size()) {
    global.fatal
    << "Error: function getIndex evaluated on "
    << inputMonomial
    << " with hash function "
    << inputMonomial.hashFunction(inputMonomial)
    << " returns index "
    << j
    << " but I have only "
    << this->size()
    << " elements. "
    << global.fatal;
  }
  if (j == - 1) {
    j = this->size();
    this->monomials.addOnTop(inputMonomial);
    this->coefficients.addOnTop(inputCoefficient);
  } else {
    if (j >= this->coefficients.size) {
      global.fatal
      << "Looking for coefficient index "
      << j
      << " when number of coefficients is "
      << this->coefficients.size
      << ". "
      << global.fatal;
    }
    this->coefficients[j] += inputCoefficient;
  }
  this->flagDeallocated = oldFlagDeallocated;
  return j;
}

template <class TemplateMonomial, class Coefficient>
int LinearCombination<TemplateMonomial, Coefficient>::
subtractMonomialNoCoefficientCleanUpReturnIndex(
  const TemplateMonomial& inputMonomial, const Coefficient& inputCoefficient
) {
  if (inputCoefficient.isEqualToZero()) {
    return - 1;
  }
  int j = this->monomials.getIndex(inputMonomial);
  if (j == - 1) {
    this->monomials.addOnTop(inputMonomial);
    this->coefficients.addOnTop(inputCoefficient);
    *this->coefficients.lastObject() *= - 1;
    j = this->coefficients.size - 1;
  } else {
    this->coefficients[j] -= inputCoefficient;
  }
  return j;
}

template <class TemplateMonomial, class Coefficient>
void ElementMonomialAlgebra<TemplateMonomial, Coefficient>::raiseToPower(
  int d,
  ElementMonomialAlgebra<TemplateMonomial, Coefficient>& output,
  const Coefficient& ringUnit
) {
  ElementMonomialAlgebra<TemplateMonomial, Coefficient> tempOne;
  tempOne.makeConstant(ringUnit);
  output = *this;
  MathRoutines::raiseToPower(output, d, tempOne);
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

class LaTeXProcedures {
public:
  static const int scaleFactor = 40;
  // In centimeters.
  static const int figureCenterCoordinateSystemX = 4;
  // In centimeters.
  static const int figureCenterCoordinateSystemY = 8;
  // In centimeters.
  static const int textPrintCenteringAdjustmentX = 3;
  static const int textPrintCenteringAdjustmentY = 3;
  static void drawline(
    double x1,
    double y1,
    double x2,
    double y2,
    uint32_t penStyle,
    int colorIndex,
    std::fstream& output,
    DrawingVariables& drawInput
  );
  static void drawTextDirectly(
    double x1,
    double y1,
    const std::string& text,
    int colorIndex,
    std::fstream& output
  );
  static void beginDocument(std::fstream& output);
  static void endLatexDocument(std::fstream& output);
  static void beginPSTricks(std::fstream& output);
  static void endPSTricks(std::fstream& output);
  static void getStringFromColorIndex(
    int colorIndex, std::string& output, DrawingVariables& drawInput
  );
};

class Permutation: public SelectionWithDifferentMaxMultiplicities {
public:
  void initPermutation(int n);
  void initPermutation(List<int>& disjointSubsets, int totalNumberOfElements);
  void incrementAndGetPermutation(List<int>& output);
  void getPermutationLthElementIsTheImageofLthIndex(List<int>& output);
  int getNumberOfPermutations() {
    return this->totalNumberSubsetsSmallInt();
  }
};

template <typename Coefficient>
void Matrix<Coefficient>::nonPivotPointsToEigenVectorHomogeneous(
  Selection& nonPivotPoints,
  Vector<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  int rowCounter = 0;
  output.setSize(this->numberOfColumns);
  for (int i = 0; i < this->numberOfColumns; i ++) {
    if (nonPivotPoints.selected[i]) {
      output[i] = ringUnit;
      continue;
    }
    output[i] = ringZero;
    for (int j = 0; j < nonPivotPoints.cardinalitySelection; j ++) {
      output[i] -= this->elements[rowCounter][nonPivotPoints.elements[j]];
    }
    rowCounter ++;
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::nonPivotPointsToEigenVectorLexicographic(
  Selection& nonPivotPoints,
  Vector<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  (void) ringZero;
  output.makeZero(this->numberOfColumns);
  if (nonPivotPoints.cardinalitySelection == 0) {
    return;
  }
  int firstNonPivotIndex = nonPivotPoints.elements[0];
  output[firstNonPivotIndex] = ringUnit;
  for (int i = firstNonPivotIndex - 1; i >= 0; i --) {
    output[i] = (*this)(i, firstNonPivotIndex);
    output[i].negate();
  }
}

template <class Coefficient>
class Complex {
  static bool flagEqualityIsApproximate;
  static double equalityPrecision;
public:
  Coefficient imaginaryPart;
  Coefficient realPart;
  std::string toString(FormatExpressions* unused = nullptr) const {
    (void) unused;
    std::stringstream currentStream;
    currentStream << *this;
    return currentStream.str();
  }
  friend std::iostream& operator<<(
    std::iostream& output, const Complex<Coefficient>& input
  ) {
    output << input.realPart << "+ i" << input.imaginaryPart;
    return output;
  }
  void operator*=(const Complex<Coefficient>& other) {
    Complex accumulator;
    accumulator.realPart =
    this->realPart *
    other.realPart - this->imaginaryPart * other.imaginaryPart;
    accumulator.imaginaryPart =
    this->realPart * other.imaginaryPart +
    this->imaginaryPart * other.realPart;
    this->operator=(accumulator);
  }
  void operator=(const Complex<Coefficient>& other) {
    this->realPart = other.realPart;
    this->imaginaryPart = other.imaginaryPart;
  }
  void operator+=(const Complex<Coefficient>& other) {
    this->realPart += other.realPart;
    this->imaginaryPart += other.imaginaryPart;
  }
  void operator-=(const Complex<Coefficient>& other) {
    this->realPart -= other.realPart;
    this->imaginaryPart -= other.imaginaryPart;
  }
  void operator=(int other) {
    this->realPart = other;
    this->imaginaryPart = 0;
  }
  bool operator==(int other) const {
    Complex<Coefficient> otherComplex;
    otherComplex = other;
    return *this == otherComplex;
  }
  bool operator==(Complex<Coefficient>& other) const {
    Complex<Coefficient> difference;
    difference = *this;
    difference -= other;
    return difference.isEqualToZero();
  }
  void operator=(double other) {
    this->realPart = other;
    this->imaginaryPart = 0;
  }
  void invert() {
    Coefficient numerator;
    numerator =
    this->realPart * this->realPart +
    this->imaginaryPart * this->imaginaryPart;
    this->realPart /= numerator;
    numerator *= - 1;
    this->imaginaryPart /= numerator;
  }
  bool isEqualToZero() const;
  void negate() {
    this->imaginaryPart = - this->imaginaryPart;
    this->realPart = - this->realPart;
  }
  bool needsParenthesisForMultiplication(FormatExpressions* unused) const {
    (void) unused;
    if (this->realPart == 0 && this->imaginaryPart >= 0) {
      return false;
    }
    if (this->imaginaryPart == 0 && this->realPart >= 0) {
      return false;
    }
    return true;
  }
  Complex() {}
  Complex(int other) {
    this->operator=(other);
  }
  Complex(double other) {
    this->operator=(other);
  }
};

template < >
bool Complex<double>::flagEqualityIsApproximate;
template < >
double Complex<double>::equalityPrecision;

template <class Coefficient>
bool Complex<Coefficient>::isEqualToZero() const {
  if (!Complex<Coefficient>::flagEqualityIsApproximate) {
    return this->imaginaryPart == 0 && this->realPart == 0;
  } else {
    return
    this->imaginaryPart < Complex<Coefficient>::equalityPrecision &&
    - this->imaginaryPart < Complex<Coefficient>::equalityPrecision &&
    this->realPart < Complex<Coefficient>::equalityPrecision &&
    - this->realPart < Complex<Coefficient>::equalityPrecision;
  }
}

template <class Coefficient>
void Matrix<Coefficient>::operator-=(const Matrix<Coefficient>& right) {
  if (
    this->numberOfRows != right.numberOfRows ||
    this->numberOfColumns != right.numberOfColumns
  ) {
    global.fatal
    << "Attempt to subtract from matrix with "
    << this->numberOfRows
    << " rows and "
    << this->numberOfColumns
    << " columns a matrix with "
    << right.numberOfRows
    << " rows and "
    << right.numberOfColumns
    << " columns. "
    << global.fatal;
  }
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      (*this)(i, j) -= right(i, j);
    }
  }
}

template <class Coefficient>
LargeIntegerUnsigned Matrix<Coefficient>::
findPositiveLCMCoefficientDenominators() {
  LargeIntegerUnsigned result = 1;
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      result =
      LargeIntegerUnsigned::leastCommonMultiple(
        result, (*this)(i, j).getDenominator()
      );
    }
  }
  return result;
}

template <class Coefficient>
void Matrix<Coefficient>::getMatrixIntegerWithDenominator(
  Matrix<LargeInteger>& outputMat, LargeIntegerUnsigned& outputDenominator
) {
  outputDenominator = this->findPositiveLCMCoefficientDenominators();
  outputMat.initialize(this->numberOfRows, this->numberOfColumns);
  Rational product;
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      product = this->elements[i][j] * outputDenominator;
      outputMat(i, j) = product.getNumerator();
    }
  }
}

template <class Coefficient>
LargeInteger Matrix<Coefficient>::findPositiveGCDCoefficientNumerators() {
  LargeIntegerUnsigned result = 0;
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      const Rational& current = (*this)(i, j);
      if (current.isEqualToZero()) {
        continue;
      }
      if (result.isEqualToZero()) {
        result = current.getNumerator().value;
        continue;
      }
      result =
      LargeIntegerUnsigned::greatestCommonDivisor(
        result, current.getNumerator().value
      );
    }
  }
  if (result == 0) {
    result = 1;
  }
  return result;
}

template <class Coefficient>
void Matrix<Coefficient>::scaleToIntegralForMinimalRationalHeightNoSignChange()
{
  Rational scale;
  scale = this->findPositiveLCMCoefficientDenominators();
  scale /= this->findPositiveGCDCoefficientNumerators();
  *this *= scale;
}

template <class Coefficient>
Coefficient Matrix<Coefficient>::getDeterminant() {
  Matrix<Coefficient> matrix = *this;
  Coefficient result;
  matrix.computeDeterminantOverwriteMatrix(result);
  return result;
}

template <class Coefficient>
Coefficient Matrix<Coefficient>::getTrace() const {
  if (this->numberOfColumns != this->numberOfRows) {
    global.fatal
    << "This is either programming error, a mathematical error, "
    << "or requires a more general definition of trace. "
    << "Requesting the trace of "
    << "a non-square matrix of "
    << this->numberOfRows
    << " rows and "
    << this->numberOfColumns
    << " columns is not allowed. "
    << global.fatal;
  }
  Coefficient sum = 0;
  for (int i = 0; i < this->numberOfColumns; i ++) {
    sum += this->elements[i][i];
  }
  return sum;
}

template <class Coefficient>
Matrix<Coefficient> Matrix<Coefficient>::operator*(
  const Matrix<Coefficient>& right
) const {
  Matrix<Coefficient> matrix;
  matrix = right;
  matrix.multiplyOnTheLeft(*this);
  return matrix;
}

template <class Coefficient>
Vector<Coefficient> Matrix<Coefficient>::operator*(
  const Vector<Coefficient>& v
) const {
  if (v.size != this->numberOfColumns) {
    global.fatal
    << "Matrix application mismatch: matrix with "
    << this->numberOfColumns
    << " columns attempted to multiply vector of length "
    << v.size
    << ". "
    << global.fatal;
  }
  Vector<Coefficient> out;
  out.makeZero(this->numberOfRows);
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      out[i] += this->elements[i][j] * v[j];
    }
  }
  return out;
}

template <class Coefficient>
void Matrix<Coefficient>::assignMatrixIntegerWithDenominator(
  Matrix<LargeInteger>& matrix, const LargeIntegerUnsigned& denominator
) {
  this->initialize(matrix.numberOfRows, matrix.numberOfColumns);
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      this->elements[i][j] = matrix.elements[i][j];
      this->elements[i][j] /= denominator;
    }
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

template <class Coefficient>
bool Vectors<Coefficient>::getNormalSeparatingCones(
  List<Vector<Coefficient> >& coneStrictlyPositiveCoefficients,
  List<Vector<Coefficient> >& coneNonNegativeCoefficients,
  Vector<Coefficient>& outputNormal
) {
  Matrix<Rational> matrixA;
  Matrix<Rational> matrixB;
  Vector<Rational> matrixX;
  int dimension = coneStrictlyPositiveCoefficients[0].size;
  if (coneStrictlyPositiveCoefficients.size == 0) {
    if (coneNonNegativeCoefficients.size > 0) {
      outputNormal.makeZero(coneNonNegativeCoefficients[0].size);
    }
    return true;
  }
  int numberOfRows =
  coneStrictlyPositiveCoefficients.size + coneNonNegativeCoefficients.size;
  matrixA.initialize(numberOfRows, dimension * 2 + numberOfRows);
  matrixA.makeZero();
  matrixB.initialize(numberOfRows, 1);
  matrixB.makeZero();
  for (int i = 0; i < coneStrictlyPositiveCoefficients.size; i ++) {
    for (int k = 0; k < dimension; k ++) {
      matrixA.elements[i][k].assign(
        coneStrictlyPositiveCoefficients.objects[i].objects[k]
      );
      matrixA.elements[i][k + dimension].assign(matrixA.elements[i][k]);
      matrixA.elements[i][k + dimension].negate();
    }
    matrixB.elements[i][0].makeOne();
    matrixA.elements[i][dimension * 2 + i].makeMinusOne();
  }
  for (int i = 0; i < coneNonNegativeCoefficients.size; i ++) {
    int currentRow = i + coneStrictlyPositiveCoefficients.size;
    for (int k = 0; k < dimension; k ++) {
      matrixA.elements[currentRow][k].assign(
        coneNonNegativeCoefficients.objects[i].objects[k]
      );
      matrixA.elements[currentRow][k + dimension].assign(
        matrixA.elements[currentRow][k]
      );
      matrixA.elements[currentRow][k + dimension].negate();
    }
    matrixA.elements[currentRow][2 * dimension + currentRow].makeOne();
  }
  bool result =
  Matrix<Rational>::
  systemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(
    matrixA, matrixB, &matrixX
  );
  outputNormal.makeZero(dimension);
  for (int i = 0; i < dimension; i ++) {
    outputNormal[i] = matrixX[i] - matrixX[i + dimension];
  }
  if (result) {
    Rational scalarProduct;
    for (int i = 0; i < coneStrictlyPositiveCoefficients.size; i ++) {
      coneStrictlyPositiveCoefficients[i].scalarEuclidean(
        outputNormal, scalarProduct
      );
      if (!scalarProduct.isPositive()) {
        global.fatal << "Unexpected non-positive value. " << global.fatal;
      }
    }
    for (int i = 0; i < coneNonNegativeCoefficients.size; i ++) {
      coneNonNegativeCoefficients[i].scalarEuclidean(
        outputNormal, scalarProduct
      );
      if (!scalarProduct.isNonPositive()) {
        global.fatal << "Unexpected positive value. " << global.fatal;
      }
    }
  }
  return result;
}

template <class Coefficient>
void Matrix<Coefficient>::computePotentialChangeGradient(
  Matrix<Coefficient>& matrixA,
  Selection& baseVariables,
  int numberOfTrueVariables,
  int columnIndex,
  Rational& outputChangeGradient,
  bool& hasAPotentialLeavingVariable
) {
  hasAPotentialLeavingVariable = false;
  outputChangeGradient.makeZero();
  for (int j = 0; j < matrixA.numberOfRows; j ++) {
    if (baseVariables.elements[j] >= numberOfTrueVariables) {
      outputChangeGradient += matrixA.elements[j][columnIndex];
    }
    hasAPotentialLeavingVariable = hasAPotentialLeavingVariable ||
    matrixA.elements[j][columnIndex].isPositive();
  }
  if (columnIndex >= numberOfTrueVariables) {
    outputChangeGradient -= 1;
  }
}

template <class Coefficient>
void Matrix<Coefficient>::getMaxMovementAndLeavingVariableRow(
  Rational& maximumMovement,
  int& leavingVariableRow,
  int enteringVariable,
  Matrix<Coefficient>& matrixA,
  Vector<Coefficient>& inputVectorX,
  Selection& baseVariables
) {
  leavingVariableRow = - 1;
  maximumMovement.makeZero();
  for (int i = 0; i < matrixA.numberOfRows; i ++) {
    Rational pivotCoefficient;
    pivotCoefficient.assign(matrixA.elements[i][enteringVariable]);
    if (pivotCoefficient.isPositive()) {
      pivotCoefficient.invert();
      pivotCoefficient.multiplyBy(inputVectorX[baseVariables.elements[i]]);
      if (
        maximumMovement.isGreaterThan(pivotCoefficient) || (
          leavingVariableRow == - 1
        )
      ) {
        maximumMovement.assign(pivotCoefficient);
        leavingVariableRow = i;
      }
    }
  }
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

template <typename Coefficient>
void Matrix<Coefficient>::getZeroEigenSpaceModifyMe(
  List<Vector<Coefficient> >& output
) {
  STACK_TRACE("Matrix::getZeroEigenSpaceModifyMe");
  if (this->numberOfRows == 0) {
    output.setSize(this->numberOfColumns);
    for (int i = 0; i < this->numberOfColumns; i ++) {
      output[i].makeEi(this->numberOfColumns, i);
    }
    return;
  }
  Selection nonPivotPoints;
  this->gaussianEliminationByRows(0, &nonPivotPoints);
  output.setSize(nonPivotPoints.cardinalitySelection);
  for (int i = 0; i < nonPivotPoints.cardinalitySelection; i ++) {
    int currentPivotIndex = nonPivotPoints.elements[i];
    output[i].makeEi(this->numberOfColumns, currentPivotIndex);
    int rowCounter = 0;
    for (int j = 0; j < this->numberOfColumns; j ++) {
      if (!nonPivotPoints.selected[j]) {
        output[i][j] -= this->elements[rowCounter][currentPivotIndex];
        rowCounter ++;
      }
    }
  }
}

template <class Coefficient>
std::string Vectors<Coefficient>::toString(FormatExpressions* format) const {
  std::stringstream out;
  std::string currentString;
  bool useLaTeX = false;
  bool useHtml = false;
  bool makeTable = false;
  if (format != nullptr) {
    useLaTeX = format->flagUseLatex;
    useHtml = format->flagUseHTML;
  }
  if (!useLaTeX && !useHtml) {
    out << this->size << " vectors: \n";
  }
  if (useLaTeX && makeTable) {
    out << "\\begin{tabular}{c}";
  }
  if (useHtml && makeTable) {
    out << "<table>";
  }
  for (int i = 0; i < this->size; i ++) {
    currentString = this->objects[i].toString(format);
    if (useHtml && makeTable) {
      out << "<tr><td>";
    }
    out << currentString;
    if (!makeTable && i != this->size - 1) {
      out << ", ";
    }
    if (useLaTeX && makeTable) {
      out << "\\\\\n";
    }
    if (useHtml && makeTable) {
      out << "</td></tr>";
    }
  }
  if (useHtml && makeTable) {
    out << "</table>";
  }
  if (useLaTeX && makeTable) {
    out << "\\end{tabular}";
  }
  return out.str();
}

template <class Object>
void List<Object>::subSelection(
  const Selection& selection, List<Object>& output
) {
  if (&output == this) {
    List<Object> thisCopy = *this;
    thisCopy.subSelection(selection, output);
    return;
  }
  output.setSize(selection.cardinalitySelection);
  for (int i = 0; i < selection.cardinalitySelection; i ++) {
    output[i] = (*this)[selection.elements[i]];
  }
}

template <class Object>
void List<Object>::intersectWith(
  const List<Object>& other, List<Object>& output
) const {
  if (&output == &other || this == &output) {
    List<Object> l1 = *this;
    List<Object> l2 = other;
    l1.intersectWith(l2, output);
    return;
  }
  HashedList<Object> intersection;
  intersection = *this;
  output.setSize(0);
  for (int i = 0; i < other.size; i ++) {
    if (intersection.contains(other[i])) {
      output.addOnTop(other[i]);
    }
  }
}

template <class Coefficient>
std::string Vector<Coefficient>::toStringLetterFormat(
  const std::string& inputLetter,
  FormatExpressions* format,
  bool dontIncludeLastVar
) const {
  if (this->isEqualToZero()) {
    return "0";
  }
  std::stringstream out;
  std::string term;
  bool found = false;
  int numberOfVariables = dontIncludeLastVar ? this->size - 1 : this->size;
  for (int i = 0; i < numberOfVariables; i ++) {
    if (!this->objects[i].isEqualToZero()) {
      term = (*this)[i].toString(format);
      if ((*this)[i].needsParenthesisForMultiplication(format)) {
        term = "(" + term + ")";
      }
      if (term == "1") {
        term = "";
      }
      if (term == "- 1" || term == "-1") {
        term = "-";
      }
      if (found) {
        if (term.size() > 0) {
          if (term[0] != '-') {
            out << "+";
          }
        } else {
          out << "+";
        }
      }
      found = true;
      out << term;
      if (format != nullptr) {
        if (format->vectorSpaceEiBasisNames.size > i) {
          out << format->vectorSpaceEiBasisNames[i];
          continue;
        }
      }
      out << inputLetter << "_{" << i + 1 << "}";
    }
  }
  return out.str();
}

template <
  class Coefficient, unsigned int inputHashFunction(const Coefficient&)
>
void MonomialTensor<Coefficient, inputHashFunction>::
multiplyByGeneratorPowerOnTheLeft(
  int generatorIndexStandsToTheLeft, const Coefficient& power
) {
  if (power == 0) {
    return;
  }
  List<int> newGeneratorIndices;
  List<Coefficient> newPowers;
  newGeneratorIndices.setExpectedSize(this->generatorsIndices.size + 1);
  newPowers.setExpectedSize(this->generatorsIndices.size + 1);
  newGeneratorIndices.addOnTop(generatorIndexStandsToTheLeft);
  newPowers.addOnTop(power);
  newGeneratorIndices.addListOnTop(this->generatorsIndices);
  newPowers.addListOnTop(this->powers);
  this->generatorsIndices = newGeneratorIndices;
  this->powers = newPowers;
  this->simplifyEqualConsecutiveGenerators(0);
}

template <
  class Coefficient, unsigned int inputHashFunction(const Coefficient&)
>
void MonomialTensor<Coefficient, inputHashFunction>::
multiplyByGeneratorPowerOnTheRight(
  int generatorIndex, const Coefficient& power
) {
  if (power == 0) {
    return;
  }
  if (this->generatorsIndices.size > 0) {
    if (*this->generatorsIndices.lastObject() == generatorIndex) {
      (*this->powers.lastObject()) += power;
      return;
    }
  }
  this->powers.addOnTop(power);
  this->generatorsIndices.addOnTop(generatorIndex);
}

template <
  class Coefficient, unsigned int inputHashFunction(const Coefficient&)
>
std::string MonomialTensor<Coefficient, inputHashFunction>::toString(
  FormatExpressions* format
) const {
  if (this->generatorsIndices.size == 0) {
    return "1";
  }
  std::string letter = format ==
  nullptr ? "g" : format->chevalleyGGeneratorLetter;
  std::string letters = "abcdefghijklmnopqrstuvwxyz";
  std::stringstream out;
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    if (static_cast<unsigned>(generatorsIndices[i]) < letters.size()) {
      out << letters[this->generatorsIndices[i]];
    } else {
      out << letter << "_{" << this->generatorsIndices[i] << "}";
    }
    out << "^{" << this->powers[i] << "}";
  }
  return out.str();
}

template <
  class Coefficient, unsigned int inputHashFunction(const Coefficient&)
>
bool MonomialTensor<Coefficient, inputHashFunction>::
simplifyEqualConsecutiveGenerators(int lowestNonReducedIndex) {
  if (this->generatorsIndices.size < 1) {
    return false;
  }
  if (lowestNonReducedIndex < 0) {
    lowestNonReducedIndex = 0;
  }
  bool result = false;
  for (
    int next = lowestNonReducedIndex + 1; next < this->generatorsIndices.size;
    next ++
  ) {
    if (
      this->generatorsIndices[lowestNonReducedIndex] ==
      this->generatorsIndices[next]
    ) {
      result = true;
      this->powers[lowestNonReducedIndex] += this->powers[next];
    } else {
      lowestNonReducedIndex ++;
      this->powers[lowestNonReducedIndex] = this->powers[next];
      this->generatorsIndices[lowestNonReducedIndex] =
      this->generatorsIndices[next];
    }
  }
  this->generatorsIndices.setSize(lowestNonReducedIndex + 1);
  this->powers.setSize(lowestNonReducedIndex + 1);
  return result;
}

template <typename Coefficient>
std::string Matrix<Coefficient>::toStringLatex(FormatExpressions* format) const {
  FormatExpressions formatCopy;
  if (format != nullptr) {
    formatCopy = *format;
  }
  formatCopy.flagUseLatex = true;
  formatCopy.flagUseHTML = false;
  return this->toString(&formatCopy);
}

template <typename Coefficient>
std::string Matrix<Coefficient>::toStringSystemLatex(
  Matrix<Coefficient>* constantTerms, FormatExpressions* format
) const {
  std::stringstream out;
  bool constTermsAreGood = (constantTerms == 0) ?
  false : (
    constantTerms->numberOfRows == this->numberOfRows &&
    constantTerms->numberOfColumns > 0
  );
  out << "\\begin{array}{l";
  for (int j = 0; j < this->numberOfColumns; j ++) {
    out << "l";
  }
  out << "}";
  std::string currentEntry;
  Polynomial<Coefficient> monomial;
  for (int i = 0; i < this->numberOfRows; i ++) {
    bool foundNonZeroEntry = false;
    for (int j = 0; j < this->numberOfColumns; j ++) {
      if (!((*this)(i, j) == 0)) {
        monomial.makeMonomial(j, 1, (*this)(i, j));
        currentEntry = monomial.toString(format);
        if (currentEntry == "") {
          global.fatal
          << "Empty strings not allowed "
          << "as result of toString() function call. "
          << global.fatal;
        }
        if (currentEntry[0] != '-' && foundNonZeroEntry) {
          out << "+";
        }
        out << currentEntry;
        foundNonZeroEntry = true;
      }
      if (j == this->numberOfColumns - 1 && !foundNonZeroEntry) {
        out << "0";
      }
      out << " & ";
    }
    out << "=";
    if (!constTermsAreGood) {
      out << "0";
    } else {
      out << (*constantTerms)(i, 0).toString(format);
    }
    out << "\\\\";
  }
  out << "\\end{array}";
  return out.str();
}

template <typename Coefficient>
std::string Matrix<Coefficient>::toString(FormatExpressions* format) const {
  std::stringstream out;
  std::string coefficientString;
  bool useHtml = (format == nullptr) ? true : format->flagUseHTML;
  bool useLatex = (format == nullptr) ? false : format->flagUseLatex;
  bool usePmatrix = (format == nullptr) ? true : format->flagUsePmatrix;
  if (useHtml) {
    out << "<table>";
  }
  if (useLatex) {
    int verticalLineIndex = format ==
    nullptr ? - 1 : format->matrixColumnVerticalLineIndex;
    if (usePmatrix) {
      out << "\\begin{pmatrix}";
    } else {
      out << "\\left(";
      out << "\\begin{array}{";
      for (int j = 0; j < this->numberOfColumns; j ++) {
        out << "c";
        if (verticalLineIndex == j) {
          out << "|";
        }
      }
      out << "}";
    }
  }
  for (int i = 0; i < this->numberOfRows; i ++) {
    if (useHtml) {
      out << "<tr>";
    }
    for (int j = 0; j < this->numberOfColumns; j ++) {
      coefficientString = (*this)(i, j).toString(format);
      if (useHtml) {
        out << "<td>";
      }
      out << coefficientString;
      if (useLatex) {
        if (j != this->numberOfColumns - 1) {
          out << " & ";
        } else {
          out << "\\\\";
        }
      }
      if (useHtml) {
        out << "</td>";
      }
      if (!useHtml && !useLatex) {
        out << ", ";
      }
    }
    if (useHtml) {
      out << "</tr>";
    }
    out << "\n";
  }
  if (useHtml) {
    out << "</table>";
  }
  if (useLatex) {
    if (usePmatrix) {
      out << "\\end{pmatrix}";
    } else {
      out << "\\end{array}";
      out << "\\right)";
    }
  }
  return out.str();
}

template <typename Coefficient>
std::string Matrix<Coefficient>::toStringOneLine(bool jsonFormat) const {
  std::stringstream out;
  out << "[";
  for (int i = 0; i < this->numberOfRows; i ++) {
    if (jsonFormat) {
      out << "[";
    }
    for (int j = 0; j < this->numberOfColumns; j ++) {
      out << this->elements[i][j];
      if (j != this->numberOfColumns - 1) {
        out << ", ";
      }
    }
    if (jsonFormat) {
      out << "]";
    }
    if (i != this->numberOfRows - 1) {
      if (jsonFormat) {
        out << ", ";
      } else {
        out << "; ";
      }
    }
  }
  out << "]";
  return out.str();
}

template <typename Coefficient>
std::string Matrix<Coefficient>::toStringPlainText(bool jsonFormat) const {
  List<List<std::string> > element_strings;
  element_strings.setSize(this->numberOfRows);
  int columnsPerElement = 0;
  for (int i = 0; i < this->numberOfRows; i ++) {
    element_strings[i].setSize(this->numberOfColumns);
    for (int j = 0; j < this->numberOfColumns; j ++) {
      std::stringstream ss;
      ss << this->elements[i][j];
      element_strings[i][j] = ss.str();
      int sl = element_strings[i][j].length();
      if (sl > columnsPerElement) {
        columnsPerElement = sl;
      }
    }
  }
  std::stringstream out;
  for (int i = 0; i < this->numberOfRows; i ++) {
    if (jsonFormat) {
      if (i == 0) {
        out << '[';
      } else {
        out << ' ';
      }
    }
    out << '[';
    for (int j = 0; j < this->numberOfColumns; j ++) {
      int sl = element_strings[i][j].length();
      int pad = columnsPerElement - sl;
      for (int pi = 0; pi < pad; pi ++) {
        out << ' ';
      }
      out << element_strings[i][j];
      if (j != this->numberOfColumns - 1) {
        if (jsonFormat) {
          out << ',';
        }
        out << ' ';
      }
    }
    out << ']';
    if (i != this->numberOfRows - 1) {
      out << '\n';
    } else {
      if (jsonFormat) {
        out << ']';
      }
    }
  }
  return out.str();
}

template <class TemplateMonomial, class Coefficient>
std::string LinearCombination<TemplateMonomial, Coefficient>::
getBlendCoefficientAndMonomial(
  const TemplateMonomial& inputMonomial,
  Coefficient& inputCoefficient,
  bool addPlusToFront,
  FormatExpressions* format
) {
  std::stringstream out;
  std::string coefficientString = inputCoefficient.toString(format);
  if (inputMonomial.isConstant()) {
    if (coefficientString[0] != '-' && addPlusToFront) {
      out << "+" << coefficientString;
    } else {
      out << coefficientString;
    }
    return out.str();
  }
  std::string monomialString = inputMonomial.toString(format);
  if (inputCoefficient.needsParenthesisForMultiplication(format)) {
    coefficientString = "\\left(" + coefficientString + "\\right)";
  }
  if (coefficientString == "1") {
    if (addPlusToFront) {
      out << "+";
    }
    out << monomialString;
    return out.str();
  }
  if (coefficientString == "- 1" || coefficientString == "-1") {
    out << "-" << monomialString;
    return out.str();
  }
  if (coefficientString[0] != '-' && addPlusToFront) {
    out << "+" << coefficientString << monomialString;
  } else {
    out << coefficientString << monomialString;
  }
  return out.str();
}

template <class TemplateMonomial, class Coefficient>
std::string LinearCombination<TemplateMonomial, Coefficient>::getTermString(
  const Coefficient& coefficient,
  const TemplateMonomial& monomial,
  FormatExpressions* format,
  const std::string& customCoefficientMonomialSeparator
) const {
  bool fracSpecialDesired = false;
  if (format != nullptr) {
    if (format->flagUseFrac && format->flagSuppressOneIn1overXtimesY) {
      fracSpecialDesired = true;
    }
  }
  std::string coefficientString;
  std::string monomialString;
  if (coefficient.needsParenthesisForMultiplication(format)) {
    coefficientString = "\\left(" + coefficient.toString(format) + "\\right)";
  } else {
    coefficientString = coefficient.toString(format);
  }
  monomialString = monomial.toString(format);
  if (monomialString == "") {
    return coefficientString;
  }
  if (fracSpecialDesired) {
    std::string denominatorString;
    if (
      StringRoutines::stringBeginsWith(
        coefficientString, "\\frac{1}", &denominatorString
      )
    ) {
      return "\\frac{" + monomialString + "}" + denominatorString;
    } else if (
      StringRoutines::stringBeginsWith(
        coefficientString, "-\\frac{1}", &denominatorString
      )
    ) {
      return "-\\frac{" + monomialString + "}" + denominatorString;
    }
  }
  if (customCoefficientMonomialSeparator != "") {
    return
    coefficientString +
    customCoefficientMonomialSeparator +
    monomialString;
  }
  if (monomialString == "1") {
    return coefficientString;
  }
  if (coefficientString == "1") {
    return monomialString;
  }
  if (coefficientString == "- 1" || coefficientString == "-1") {
    return "-" + monomialString;
  }
  return coefficientString + monomialString;
}

template <class TemplateMonomial, class Coefficient>
std::string LinearCombination<TemplateMonomial, Coefficient>::
toStringWithPossibleLineBreak(
  FormatExpressions* format, int* outputNumberOfLines
) const {
  int lines = 1;
  if (outputNumberOfLines == nullptr) {
    outputNumberOfLines = &lines;
  }
  std::string result = this->toString(format, outputNumberOfLines);
  if (*outputNumberOfLines <= 1) {
    return result;
  }
  std::stringstream out;
  out << "\\begin{array}{l}" << result << "\\end{array}";
  return out.str();
}

template <class TemplateMonomial, class Coefficient>
std::string LinearCombination<TemplateMonomial, Coefficient>::toString(
  FormatExpressions* format, int* outputNumberOfLines
) const {
  STACK_TRACE("LinearCombination::toString");
  if (outputNumberOfLines != nullptr) {
    *outputNumberOfLines = 1;
  }
  if (this->size() == 0) {
    return "0";
  }
  std::stringstream out;
  List<TemplateMonomial> sortedMonomials;
  sortedMonomials = this->monomials;
  // If this line fails to link, you must do the following.
  // You need to implement
  // FormatExpressions::getMonomialOrder<TemplateMonomial>()
  // and make it return 0 (or a pointer to a monomial order, should you
  // wish to use a custom one.
  typename List<TemplateMonomial>::Comparator* monomialOrder = (
    format == nullptr
  ) ?
  0 :
  format->getMonomialOrder<TemplateMonomial>();
  sortedMonomials.quickSortDescending(monomialOrder);
  int cutOffCounter = 0;
  bool useCustomPlus = false;
  int maximumLineLength = format == nullptr ? - 1 : format->maximumLineLength;
  int numberOfAmpersandsPerNewLineForLaTeX = (format == nullptr) ?
  1 :
  format->numberOfAmpersandsPerNewLineForLaTeX;
  bool flagUseLaTeX = (format == nullptr) ? false : format->flagUseLatex;
  bool flagUseHTML = (format == nullptr) ? false : format->flagUseHTML;
  bool isInNumerator = (format == nullptr) ? false : format->flagIsInNumerator;
  std::string customTimes = "";
  if (format != nullptr) {
    useCustomPlus = (format->customPlusSign != "");
    if (
      format->flagPassCustomCoefficientMonomialSeparatorToCoefficients == false
    ) {
      customTimes = format->customCoefficientMonomialSeparator;
      format->customCoefficientMonomialSeparator = "";
    }
  }
  for (int i = 0; i < sortedMonomials.size; i ++) {
    TemplateMonomial& monomial = sortedMonomials[i];
    int coefficientIndex = this->monomials.getIndex(monomial);
    Coefficient& coefficient = this->coefficients[coefficientIndex];
    std::string termString =
    this->getTermString(coefficient, monomial, format, customTimes);
    if (i > 0) {
      if (!useCustomPlus) {
        if (termString.size() > 0) {
          if (termString[0] != '-') {
            out << "+";
            cutOffCounter += 1;
          }
        } else {
          out << "+";
          cutOffCounter += 1;
        }
      } else {
        out << format->customPlusSign;
      }
    }
    out << termString;
    cutOffCounter += termString.size();
    if (maximumLineLength > 0) {
      if (cutOffCounter > maximumLineLength) {
        cutOffCounter = 0;
        if (flagUseLaTeX && i != sortedMonomials.size - 1) {
          if (outputNumberOfLines != nullptr) {
            (*outputNumberOfLines) ++;
          }
          out << " \\\\";
          if (isInNumerator) {
            out << " \\hline ";
          }
          for (int k = 0; k < numberOfAmpersandsPerNewLineForLaTeX; k ++) {
            out << "&";
          }
          out << " ";
        }
        if (flagUseHTML && !flagUseLaTeX && (i != sortedMonomials.size - 1)) {
          out << " <br>";
        }
      }
    }
  }
  if (format != nullptr) {
    format->customCoefficientMonomialSeparator = customTimes;
    if (format->suffixLinearCombination != "") {
      out << " " << format->suffixLinearCombination;
    }
  }
  return out.str();
}

// Wall of a cone: contains the normal of the wall as
// well as the id(s) of the cones on the other side of the wall.
// The number of ids on the other side of the wall can be:
// 1) zero if the cone is on the outside of cone complex;
// 2) 1 if the cone complex is normal (every wall is exactly between
// two chambers)
// 3) 2 or more if the cone complex is not normal, in which case
// we are likely trying to normalize it and are in the middle of computations.
class Wall {
  friend std::ostream& operator<<(std::ostream& output, const Wall& wall) {
    output << wall.toString();
    return output;
  }
public:
  List<int> neighbors;
  Vector<Rational> normal;
  bool operator==(const Wall& other) const;
  bool operator>(const Wall& other) const;
  bool operator!=(const Wall& other) const;
  // Is the input point on the "inside" half-space of my wall or on the wall?
  bool isInClosedHalfSpace(const Vector<Rational>& point) const;
  // Is the input point on the "inside" half-space of my wall without
  // being on the wall?
  bool isInOpenHalfSpace(const Vector<Rational>& point) const;
  // Are all the input point on the "inside" half-space of my wall without
  // being on the wall?
  bool hasAtLeastOnePointInOpenHalfSpace(const List<Vector<Rational> >& points)
  const;
  std::string toString() const;
  bool checkConsistency() const;
};

class DynkinSimpleType {
  friend std::ostream& operator<<(
    std::ostream& output, const DynkinSimpleType& monomial
  ) {
    output << monomial.toString();
    return output;
  }
public:
  char letter;
  int rank;
  Rational cartanSymmetricInverseScale;
  // Constants used to plot Dynkin diagrams.
  static const int radiusOfRootCircle = 1;
  static const int distanceBetweenRootCenters = 20;
  static const int labelDistance = 5;
  DynkinSimpleType(): letter('X'), rank(- 1), cartanSymmetricInverseScale(0) {}
  DynkinSimpleType(const DynkinSimpleType& other) {
    *this = other;
  }
  DynkinSimpleType(
    char inputLetter, int inputRank, const Rational& inputScale = 1
  ):
  letter(inputLetter),
  rank(inputRank),
  cartanSymmetricInverseScale(inputScale) {}
  int getRootSystemSize() const;
  int getLieAlgebraDimension() const {
    return this->getRootSystemSize() + this->rank;
  }
  void makeArbitrary(
    char inputLetter,
    int inputRank,
    const Rational& inputLengthFirstCorRootSquared
  );
  Rational getPrincipalSlTwoCartanSymmetricInverseScale() const;
  void getCoCartanSymmetric(Matrix<Rational>& output) const;
  void getCartanSymmetric(Matrix<Rational>& output) const;
  void getAn(int n, Matrix<Rational>& output) const;
  void getBn(int n, Matrix<Rational>& output) const;
  void getCn(int n, Matrix<Rational>& output) const;
  void getDn(int n, Matrix<Rational>& output) const;
  void getEn(int n, Matrix<Rational>& output) const;
  void getF4(Matrix<Rational>& output) const;
  void getG2(Matrix<Rational>& output) const;
  void grow(
    List<DynkinSimpleType>& output, List<List<int> >* outputPermutationRoots
  ) const;
  void operator=(const DynkinSimpleType& other) {
    this->letter = other.letter;
    this->rank = other.rank;
    this->cartanSymmetricInverseScale = other.cartanSymmetricInverseScale;
  }
  bool operator==(const DynkinSimpleType& other) const {
    return
    this->letter == other.letter &&
    this->rank == other.rank &&
    this->cartanSymmetricInverseScale == other.cartanSymmetricInverseScale;
  }
  static unsigned int hashFunction(const DynkinSimpleType& input) {
    return
    static_cast<unsigned int>(input.letter) * 2 + static_cast<unsigned int>(
      input.rank
    ) +
    HashConstants::constant0 *
    input.cartanSymmetricInverseScale.hashFunction();
  }
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  void getAutomorphismActingOnVectorColumn(
    MatrixTensor<Rational>& output, int autoIndex
  ) const;
  Rational getDefaultCoRootLengthSquared(int rootIndex) const;
  Rational getDefaultRootLengthSquared(int rootIndex) const;
  Rational getDefaultLongRootLengthSquared() const;
  Rational getEpsilonRealizationLongRootLengthSquared() const;
  static Rational getDefaultLongRootLengthSquared(char inputType) {
    DynkinSimpleType dynkinType(inputType, 2);
    return dynkinType.getDefaultLongRootLengthSquared();
  }
  Rational getLongRootLengthSquared() const;
  Rational getRatioRootSquaredToFirstSquared(int rootIndex) const;
  static Rational getRatioLongRootToFirst(char inputWeylLetter, int inputRank);
  static Rational getDynkinIndexParabolicallyInducingSubalgebra(
    char inputType
  );
  bool canBeExtendedParabolicallyTo(const DynkinSimpleType& otherType) const;
  bool canBeExtendedParabolicallyOrIsEqualTo(const DynkinSimpleType& otherType)
  const {
    if (*this == otherType) {
      return true;
    }
    return this->canBeExtendedParabolicallyTo(otherType);
  }
  bool hasPrecomputedSubalgebras() const;
  Rational getRatioLongRootToFirst() const {
    return this->getRatioLongRootToFirst(this->letter, this->rank);
  }
  std::string toString(FormatExpressions* format = nullptr) const;
  std::string toStringNonTechnicalName(FormatExpressions* format = nullptr)
  const;
  void operator++(int);
  bool operator>(const DynkinSimpleType& other) const;
  static void getEpsilonMatrix(
    char weylLetter, int weylRank, Matrix<Rational>& output
  );
  bool operator<(const DynkinSimpleType& other) const {
    return other > *this;
  }
  bool operator<(int otherRank) const;
  void plot(Plot& output, int verticalOffset) const;
  static void plotOneRoot(
    Plot& output, int index, bool filled, int verticalOffset
  );
  static void plotHorizontalChainOfRoots(
    Plot& output, int count, Selection* filledNodes
  );
  static void plotHorizontalChainOfRoots(
    Plot& output,
    int verticalOffset,
    int count,
    Selection* filledNodes,
    List<std::string>* labels
  );
  static void plotE6(Plot& output, int verticalOffset);
  static void plotE7(Plot& output, int verticalOffset);
  static void plotE8(Plot& output, int verticalOffset);
  static void plotF4(Plot& output, int verticalOffset);
  static void plotG2(Plot& output, int verticalOffset);
  static void appendOneSingleConnectedRootToTheRight(
    Plot& output,
    int segmentsSoFar,
    int verticalOffset,
    const std::string& label,
    bool filled
  );
  static void plotAn(
    Plot& output, int rank, Selection* filledRoots, int verticalOffset
  );
  static void plotBn(
    Plot& output, int rank, Selection* filledRoots, int verticalOffset
  );
  static void plotBC(
    Plot& output, int rank, Selection* filledRoots, int verticalOffset
  );
  static void plotCn(
    Plot& output, int rank, Selection* filledRoots, int verticalOffset
  );
  static void plotDn(
    Plot& output, int rank, Selection* filledRoots, int verticalOffset
  );
};

// This class may need a modification: perhaps it should not inherit monomial
// collection,
// but rather have a monomial collection as a private member.
// Many important operations with Dynkin types require
// fixed order of monomials,
// which may impose this reorganization.
class DynkinType: public LinearCombination<DynkinSimpleType, Rational> {
private:
public:
  void getLettersTypesMultiplicities(
    List<char>* outputLetters = nullptr,
    List<int>* outputRanks = nullptr,
    List<int>* outputMults = nullptr,
    List<Rational>* outputFirstCoRootLengthsSquared = nullptr
  ) const;
  void getTypesWithMults(List<DynkinSimpleType>& output) const;
  static void getOuterAutosGeneratorsOneTypeActOnVectorColumn(
    List<MatrixTensor<Rational> >& output,
    const DynkinSimpleType& dynkinType,
    int multiplicity
  );
  unsigned int hashFunction() const {
    return this->LinearCombination<DynkinSimpleType, Rational>::hashFunction();
  }
  static unsigned int hashFunction(const DynkinType& input) {
    return LinearCombination<DynkinSimpleType, Rational>::hashFunction(input);
  }
  void getOuterAutosGeneratorsActOnVectorColumn(
    List<MatrixTensor<Rational> >& output
  );
  bool isSimpleOfType(char desiredType, int desiredRank) const;
  bool isSimpleOfType(
    char desiredType,
    int* outputRank = nullptr,
    Rational* outputLength = nullptr
  ) const;
  bool isSimple(
    char* outputtype = nullptr,
    int* outputRank = nullptr,
    Rational* outputLength = nullptr
  ) const;
  void getSortedDynkinTypes(List<DynkinSimpleType>& output) const;
  Rational getPrincipalSlTwoCartanSymmetricInverseScale() const;
  void sortDynkinTypes();
  bool grow(
    const List<Rational>& allowedInverseScales,
    int ambientWeylDimension,
    List<DynkinType>& output,
    List<List<int> >* outputPermutationRoots
  ) const;
  bool hasPrecomputedSubalgebras() const;
  std::string toStringVirtualNameFolder() const;
  bool containsType(char typeLetter) const;
  void getDynkinTypeWithDefaultScales(DynkinType& output) const;
  static void getPrecomputedDynkinTypes(List<DynkinType>& output);
  DynkinSimpleType getGreatestSimpleType() const;
  DynkinSimpleType getSmallestSimpleType() const;
  LargeInteger getWeylGroupSizeByFormula() const;
  std::string toString(FormatExpressions* format = nullptr) const;
  void scaleFirstCoRootSquaredLength(
    const Rational& multiplyCoRootSquaredLengthBy
  );
  int getMultiplicity(int simpleTypeIdentifier) const;
  int getNumberOfSimpleComponentsOfGivenRank(int desiredRank) const;
  int getNumberOfSimpleComponents() const;
  Rational getRankRational() const;
  int getRank() const;
  int getRootSystemSize() const;
  int getLieAlgebraDimension() const;
  bool isTypeAOne() const;
  static int getIndexPreimageFromRootInjection(
    int inputIndex, const List<int>& inputRootInjection
  );
  bool canBeExtendedParabolicallyTo(const DynkinType& other) const;
  bool canBeExtendedParabolicallyOrIsEqualTo(const DynkinType& other) const;
  void makeSimpleType(
    char type, int rank, const Rational* inputFirstCoRootSquareLength = nullptr
  );
  void getEpsilonMatrix(Matrix<Rational>& output) const;
  void getCoCartanSymmetric(Matrix<Rational>& output) const;
  void getCartanSymmetric(Matrix<Rational>& output) const;
  void getCartanSymmetricDefaultLengthKeepComponentOrder(
    Matrix<Rational>& output
  ) const;
  int getCoxeterEdgeWeight(int v, int w);
  std::string getWeylGroupName(FormatExpressions* format = nullptr) const;
  static void getDynkinIndicesSl2SubalgebrasSimpleType(
    const DynkinSimpleType& desiredType,
    List<List<Rational> >& precomputedDynkinIndicesSl2subalgebrasSimpleTypes,
    HashedList<DynkinSimpleType>& dynkinSimpleTypesWithComputedSl2Subalgebras,
    HashedList<Rational>& outputDynkinIndices,
    AlgebraicClosureRationals* algebraicClosure
  );
  void getDynkinIndicesSl2Subalgebras(
    List<List<Rational> >& precomputedDynkinIndicesSl2subalgebrasSimpleTypes,
    HashedList<DynkinSimpleType>& dynkinSimpleTypesWithComputedSl2Subalgebras,
    HashedList<Rational>& outputDynkinIndices,
    AlgebraicClosureRationals* algebraicClosure
  );
  bool hasExceptionalComponent() const;
  bool operator>(const DynkinType& other) const;
  void operator=(const LinearCombination<DynkinSimpleType, Rational>& other);
  DynkinType operator-(
    const LinearCombination<DynkinSimpleType, Rational>& other
  );
  // These functions are used to plot dynkin diagrams and Vogan diagrams.
  void plot(Plot& output);
  static void plotInitialize(Plot& output);
};

class DynkinDiagramRootSubalgebra {
public:
  Vectors<Rational> ambientRootSystem;
  Matrix<Rational> ambientBilinearForm;
  List<Vectors<Rational> > simpleBasesConnectedComponents;
  // to each connected component of the simple bases corresponds
  // its dynkin string with the same index
  List<DynkinSimpleType> simpleComponentTypes;
  List<int> indicesThreeNodes;
  List<List<int> > indicesEnds;
  List<List<int> > sameTypeComponents;
  List<int> indexUniComponent;
  List<int> indexInUniComponent;
  bool checkInitialization() const;
  std::string toString(FormatExpressions* format = nullptr) const;
  int rankTotal();
  int numberRootsGeneratedByDiagram();
  void sort();
  void getDynkinType(DynkinType& output) const;
  void swapDynkinStrings(int i, int j);
  Rational getSquareLengthLongestRootLinkedTo(
    const Vector<Rational>& inputVector
  );
  Rational getSquareLengthShortestRootLinkedTo(
    const Vector<Rational>& inputVector
  );
  bool letterIsDynkinGreaterThanLetter(char letter1, char letter2);
  // The function below takes as an input a set of roots and
  // computes the corredponding Dynkin diagram of the
  // root subsystem. Note: the simleBasisInput
  // is required to be a set of simple
  // roots.
  // The procedure calls a
  // transformation to simple basis on the simpleBasisInput,
  // so your input will get changed if it wasn't
  // simple as required.
  void computeDiagramTypeModifyInput(
    Vectors<Rational>& inputRoots, WeylGroupData& weylGroup
  );
  // The following function is just as the above but doesn't modify
  // simpleBasisInput
  void computeDiagramInputIsSimple(const Vectors<Rational>& simpleBasisInput);
  void computeDiagramTypeModifyInputRelative(
    Vectors<Rational>& inputOutputSimpleWeightSystem,
    const HashedList<Vector<Rational> >& weightSystem,
    const Matrix<Rational>& bilinearForm
  );
  void computeDynkinStrings();
  void computeDynkinString(int indexComponent);
  int numberOfThreeValencyNodes(int indexComponent);
  bool operator==(const DynkinDiagramRootSubalgebra& right) const;
  bool isGreaterThan(DynkinDiagramRootSubalgebra& right);
  Rational getSizeCorrespondingWeylGroupByFormula();
  void getAutomorphism(List<List<int> >& output, int index);
  void getAutomorphisms(List<List<List<int> > >& output);
  void getMapFromPermutation(
    Vectors<Rational>& domain,
    Vectors<Rational>& range,
    List<int>& permutation,
    List<List<List<int> > >& automorphisms,
    SelectionWithDifferentMaxMultiplicities& autosPermutations,
    DynkinDiagramRootSubalgebra& right
  );
};

class VectorPartition {
public:
  Vectors<Rational> partitioningVectors;
  Vector<Rational> targetSum;
  List<int> currentPartition;
  Vector<Rational> currentPartitionSum;
  // Partitions stores all generated partitions, when
  // flagStoreAllPartitions is set.
  // Format: each element of partitions gives an array whose entries give
  // the multiplicity of the weights. In other words,
  // if partitioningVectors has 2 elements,
  // then partitions[0]
  // would have 2 elements: the first giving the multiplicity of
  // partitioningVectors[0]
  // and the second - the multiplicity of
  // partitioningVectors[1].
  List<List<int> > partitions;
  // Whether or not to use the partitions array.
  bool flagStoreAllPartitions;
  VectorPartition() {
    this->flagStoreAllPartitions = false;
  }
  bool currentPartitionSumExceedsGoal() const;
  bool addOneAtIndex(int atIndex);
  bool addOne();
  void addIndex(int index, int quantity);
  std::string toStringPartitioningVectors();
  std::string toStringOnePartition(const List<int>& currentPartition);
  std::string toStringAllPartitions(bool useHtml);
  bool initialize(
    const Vectors<Rational>& inputPartitioningVectors,
    const Vector<Rational>& inputTarget
  );
  bool incrementReturnFalseIfPastLast();
  int numberOfVectors() const {
    return this->partitioningVectors.size;
  }
  int getDimension() const {
    return this->targetSum.size;
  }
  int numberOfPartitionsByEnumeration();
};

class LittelmannPath {
public:
  friend std::ostream& operator<<(
    std::ostream& output, const LittelmannPath& unused
  ) {
    (void) unused;
    output << "A Littelmann Path.";
    return output;
  }
  WeylGroupData* owner;
  Vectors<Rational> waypoints;
  void makeFromweightInSimpleCoordinates(
    const Vector<Rational>& weightInSimpleCoordinates,
    WeylGroupData& inputOwner
  );
  void makeFromWaypoints(
    Vectors<Rational>& weightsinSimpleCoordinates, WeylGroupData& inputOwner
  ) {
    this->owner = &inputOwner;
    this->waypoints = weightsinSimpleCoordinates;
    this->simplify();
  }
  void actByFAlpha(int indexAlpha);
  void actByEAlpha(int indexAlpha);
  void actByEFDisplayIndex(int displayIndex);
  void operator+=(const LittelmannPath& other) {
    this->waypoints.reserve(this->waypoints.size + other.waypoints.size);
    Vector<Rational> endPoint = *this->waypoints.lastObject();
    for (int i = 0; i < other.waypoints.size; i ++) {
      this->waypoints.addOnTop(other.waypoints[i] + endPoint);
    }
  }
  bool isAdaptedString(
    MonomialTensor<int, HashFunctions::hashFunction>& inputString
  );
  std::string toStringIndicesToCalculatorOutput(
    LittelmannPath& inputStartingPath, List<int>& input
  );
  std::string toStringOperatorSequenceStartingOnMe(List<int>& input);
  bool generateOrbit(
    List<LittelmannPath>& output,
    List<List<int> >& outputOperators,
    int upperBoundNumberOfElements,
    Selection* parabolicNonSelectedAreInLeviPart = nullptr
  );
  std::string generateOrbitAndAnimate();
  bool minimaAreIntegral();
  std::string toString(
    bool useSimpleCoordinates = true,
    bool useArrows = true,
    bool includeDominance = false
  ) const;
  void simplify();
  unsigned int hashFunction() const {
    return this->waypoints.hashFunction();
  }
  static unsigned int hashFunction(const LittelmannPath& input) {
    return input.hashFunction();
  }
  bool isEqualToZero() const {
    return this->waypoints.size == 0;
  }
  LittelmannPath();
  LittelmannPath(const LittelmannPath& other);
  LittelmannPath& operator=(const LittelmannPath& other) {
    this->waypoints = other.waypoints;
    this->owner = other.owner;
    return *this;
  }
  bool operator==(const LittelmannPath& other) const {
    return this->waypoints == other.waypoints;
  }
};

class MonomialMatrix {
  friend std::ostream& operator<<(
    std::ostream& output, const MonomialMatrix& monomial
  ) {
    output << monomial.toString();
    return output;
  }
public:
  // The monomial matrix:
  // ( 0 1 )
  // ( 0 0 )
  // has vIndex    = row index    = 0
  // and dualIndex = column index = 1
  int vIndex;
  int dualIndex;
  bool isIdentity;
  MonomialMatrix(const MonomialMatrix& other) {
    this->operator=(other);
  }
  MonomialMatrix(): vIndex(- 1), dualIndex(- 1), isIdentity(false) {}
  MonomialMatrix(int i, int j): vIndex(i), dualIndex(j), isIdentity(false) {}
  void operator=(const MonomialMatrix& other) {
    this->vIndex = other.vIndex;
    this->dualIndex = other.dualIndex;
    this->isIdentity = other.isIdentity;
  }
  void makeEij(int i, int j) {
    this->dualIndex = j;
    this->vIndex = i;
    this->isIdentity = false;
  }
  void makeOne() {
    this->makeIdentitySpecial();
  }
  void transpose() {
    MathRoutines::swap(this->vIndex, this->dualIndex);
  }
  void makeZero() {
    this->isIdentity = false;
    this->vIndex = - 1;
    this->dualIndex = - 1;
  }
  bool checkConsistency() const {
    return true;
  }
  bool isZeroMonomial() const {
    return !this->isIdentity && this->vIndex == - 1 && this->dualIndex == - 1;
  }
  bool operator==(const MonomialMatrix& other) const {
    return
    this->vIndex == other.vIndex &&
    this->dualIndex == other.dualIndex &&
    this->isIdentity == other.isIdentity;
  }
  static unsigned int hashFunction(const MonomialMatrix& input) {
    return
    static_cast<unsigned int>(input.vIndex) * HashConstants::constant0 +
    static_cast<unsigned int>(input.dualIndex) *
    HashConstants::constant1 +
    input.isIdentity;
  }
  unsigned int hashFunction() const {
    return hashFunction(*this);
  }
  bool operator>(const MonomialMatrix& other) const {
    if (this->isIdentity != other.isIdentity) {
      return this->isIdentity > other.isIdentity;
    }
    if (this->vIndex == other.vIndex) {
      return this->dualIndex > other.dualIndex;
    }
    return this->vIndex > other.vIndex;
  }
  void makeIdentitySpecial() {
    this->vIndex = - 1;
    this->dualIndex = - 1;
    this->isIdentity = true;
  }
  std::string toString(FormatExpressions* format = nullptr) const {
    (void) format;
    std::stringstream out;
    if (!this->isIdentity) {
      out
      << "m_{"
      << (this->vIndex + 1)
      << "}\\otimes "
      << "m^*_{"
      << (this->dualIndex + 1)
      << "}";
    } else {
      out << "id";
    }
    return out.str();
  }
  void operator*=(const MonomialMatrix& other) {
    STACK_TRACE("MonomialMatrix::operator*=");
    if (this == &other) {
      MonomialMatrix otherCopy;
      otherCopy = other;
      *this *= otherCopy;
      return;
    }
    if (other.isIdentity) {
      return;
    }
    if (this->isIdentity) {
      *this = other;
      return;
    }
    if (this->dualIndex == other.vIndex) {
      this->dualIndex = other.dualIndex;
    } else {
      this->makeZero();
    }
  }
};

template <class Coefficient>
class MatrixTensor: public LinearCombination<MonomialMatrix, Coefficient> {
public:
  void makeIdentitySpecial() {
    this->makeZero();
    MonomialMatrix monomial;
    monomial.makeIdentitySpecial();
    this->addMonomial(monomial, 1);
  }
  void makeIdentity(int numberOfVariables) {
    this->makeZero();
    MonomialMatrix monomial;
    for (int i = 0; i < numberOfVariables; i ++) {
      monomial.dualIndex = i;
      monomial.vIndex = i;
      this->addMonomial(monomial, 1);
    }
  }
  void invert();
  int getMinimalNumberOfRows() const {
    int result = - 1;
    for (int i = 0; i < this->size(); i ++) {
      result = MathRoutines::maximum(result, (*this)[i].vIndex);
    }
    return result + 1;
  }
  int getMinimalNumberOfColumns() const {
    int result = - 1;
    for (int i = 0; i < this->size(); i ++) {
      result = MathRoutines::maximum(result, (*this)[i].dualIndex);
    }
    return result + 1;
  }
  int getMinimumNumberOfColumnsNumberOfRows() const {
    int result = - 1;
    for (int i = 0; i < this->size(); i ++) {
      result = MathRoutines::maximum(result, (*this)[i].dualIndex);
      result = MathRoutines::maximum(result, (*this)[i].vIndex);
    }
    return result + 1;
  }
  Coefficient getDeterminant() const {
    Matrix<Coefficient> matrix;
    this->getMatrix(matrix, this->getMinimumNumberOfColumnsNumberOfRows());
    return matrix.getDeterminant();
  }
  void directSumWith(const MatrixTensor<Coefficient>& other);
  void raiseToPower(int power);
  void operator*=(const MatrixTensor<Coefficient>& other);
  void operator*=(const Coefficient& other) {
    return
    this->::LinearCombination<MonomialMatrix, Coefficient>::operator*=(other);
  }
  void getVectorsSparseFromRowsIncludeZeroRows(
    List<VectorSparse<Coefficient> >& output, int minimalNumberOfRows = - 1
  );
  bool isIdentity() const {
    int dimension = this->getMinimumNumberOfColumnsNumberOfRows();
    Selection selection;
    selection.initialize(dimension);
    for (int i = 0; i < this->size(); i ++) {
      if ((*this)[i].vIndex != (*this)[i].dualIndex) {
        return false;
      }
      if (this->coefficients[i] != 1) {
        return false;
      }
      selection.addSelectionAppendNewIndex((*this)[i].vIndex);
    }
    return selection.cardinalitySelection == dimension;
  }
  bool isPositiveDefinite() {
    Matrix<Coefficient> other;
    this->getMatrix(other, this->getMinimumNumberOfColumnsNumberOfRows());
    return other.isPositiveDefinite();
  }
  void operator=(const Matrix<Coefficient>& other) {
    this->makeZero();
    MonomialMatrix monomial;
    for (int i = 0; i < other.numberOfRows; i ++) {
      for (int j = 0; j < other.numberOfColumns; j ++) {
        if (!other.elements[i][j].isEqualToZero()) {
          monomial.dualIndex = j;
          monomial.vIndex = i;
          this->addMonomial(monomial, other.elements[i][j]);
        }
      }
    }
  }
  void assignTensorProduct(
    const MatrixTensor<Coefficient>& left,
    const MatrixTensor<Coefficient>& right
  ) {
    // handle lazy programmers:
    if (this == &left || this == &right) {
      MatrixTensor<Coefficient> leftCopy = left;
      MatrixTensor<Coefficient> rightCopy = right;
      this->assignTensorProduct(leftCopy, rightCopy);
      return;
    }
    // The basis of the tensor product
    // vector space MUST be in the SAME order as
    // the one used by Matrix::assignTensorProduct.
    // int leftDomainDim= left.getMinimalNumberOfColumns();
    int rightDomainDim = right.getMinimalNumberOfColumns();
    // int leftRangeDim= left.getMinimalNumberOfRows();
    int rightRangeDim = right.getMinimalNumberOfRows();
    MonomialMatrix monomial;
    this->makeZero();
    Coefficient coefficient;
    for (int i = 0; i < left.size(); i ++) {
      for (int j = 0; j < right.size(); j ++) {
        monomial.dualIndex =
        left[i].dualIndex * rightDomainDim + right[j].dualIndex;
        monomial.vIndex = left[i].vIndex * rightRangeDim + right[j].vIndex;
        coefficient = left.coefficients[i];
        coefficient *= right.coefficients[j];
        this->addMonomial(monomial, coefficient);
      }
    }
  }
  void substitute(const PolynomialSubstitution<Rational>& substitution) {
    MatrixTensor<Coefficient> thisCopy = *this;
    this->makeZero();
    Coefficient coefficient;
    for (int i = 0; i < thisCopy.size(); i ++) {
      coefficient = thisCopy.coefficients[i];
      coefficient.substitute(substitution, Rational::one());
      this->addMonomial(thisCopy[i], coefficient);
    }
  }
  void transpose() {
    MatrixTensor<Coefficient> output;
    MonomialMatrix monomial;
    output.makeZero();
    for (int i = 0; i < this->size(); i ++) {
      monomial = (*this)[i];
      monomial.transpose();
      output.addMonomial(monomial, this->coefficients[i]);
    }
    *this = output;
  }
  void setNumberOfVariables(int newNumberOfVariables) {
    MatrixTensor<Coefficient> thisCopy = *this;
    this->makeZero();
    Coefficient tempCF;
    for (int i = 0; i < thisCopy.size; i ++) {
      tempCF = thisCopy.coefficients[i];
      tempCF.setNumberOfVariables(newNumberOfVariables);
      this->addMonomial(thisCopy[i], tempCF);
    }
  }
  void lieBracketOnTheLeft(const MatrixTensor<Coefficient>& standsOnTheLeft) {
    STACK_TRACE("MatrixTensor<Coefficient>::lieBracketOnTheLeft");
    MatrixTensor<Coefficient> output;
    MonomialMatrix monomial;
    output.makeZero();
    output.setExpectedSize(this->size() * standsOnTheLeft.size() * 2);
    Coefficient coefficient;
    for (int i = 0; i < this->size(); i ++) {
      for (int j = 0; j < standsOnTheLeft.size(); j ++) {
        coefficient = this->coefficients[i];
        coefficient *= standsOnTheLeft.coefficients[j];
        monomial = standsOnTheLeft[j];
        monomial *= (*this)[i];
        output.addMonomial(monomial, coefficient);
        coefficient *= - 1;
        monomial = (*this)[i];
        monomial *= standsOnTheLeft[j];
        output.addMonomial(monomial, coefficient);
      }
    }
    *this = output;
  }
  std::string toStringMatrixForm(FormatExpressions* format = nullptr) const {
    if (this->isEqualToZero()) {
      return "(0)";
    }
    Matrix<Coefficient> matrix;
    this->getMatrix(matrix, this->getMinimumNumberOfColumnsNumberOfRows());
    return matrix.toString(format);
  }
  void getMatrix(Matrix<Coefficient>& output, int dimension) const {
    dimension =
    MathRoutines::maximum(
      dimension, this->getMinimumNumberOfColumnsNumberOfRows()
    );
    output.initialize(dimension, dimension);
    output.makeZero();
    for (int i = 0; i < this->size(); i ++) {
      if ((*this)[i].isIdentity) {
        for (int j = 0; j < dimension; j ++) {
          output(j, j) += this->coefficients[i];
        }
      } else {
        output((*this)[i].vIndex, (*this)[i].dualIndex) +=
        this->coefficients[i];
      }
    }
  }
  template <class otherType>
  void assignVectorsToColumns(const Vectors<otherType>& inputVectors) {
    this->makeZero();
    for (int i = 0; i < inputVectors.size; i ++) {
      for (int j = 0; j < inputVectors[i].size; j ++) {
        this->addMonomial(MonomialMatrix(j, i), inputVectors[i][j]);
      }
    }
  }
  template <class otherType>
  void assignVectorsToColumns(
    const List<VectorSparse<otherType> >& inputVectors
  ) {
    this->makeZero();
    for (int i = 0; i < inputVectors.size; i ++) {
      for (int j = 0; j < inputVectors[i].size(); j ++) {
        this->addMonomial(
          MonomialMatrix(inputVectors[i][j].monomialIndex, i),
          inputVectors[i].coefficients[j]
        );
      }
    }
  }
  template <class otherType>
  void assignVectorsToRows(const List<VectorSparse<otherType> >& inputVectors)
  {
    this->makeZero();
    for (int i = 0; i < inputVectors.size; i ++) {
      for (int j = 0; j < inputVectors[i].size(); j ++) {
        this->addMonomial(
          MonomialMatrix(i, inputVectors[i][j].monomialIndex),
          inputVectors[i].coefficients[j]
        );
      }
    }
  }
  bool isNilpotent() const {
    MatrixTensor<Coefficient> element;
    element = *this;
    int dimensionPlusOne = this->getMinimumNumberOfColumnsNumberOfRows() + 1;
    for (int i = dimensionPlusOne; i > 0; i /= 2) {
      element *= element;
      if (element.isEqualToZero()) {
        return true;
      }
    }
    return false;
  }
  void gaussianEliminationByRowsMatrix(
    MatrixTensor<Coefficient>* carbonCopyMatrix = 0
  );
  template <class otherType>
  void actOnVectorColumn(
    const Vector<otherType>& input, Vector<otherType>& output
  ) const {
    if (&input == &output) {
      Vector<otherType> inputCopy = input;
      this->actOnVectorColumn(inputCopy, output);
      return;
    }
    output.makeZero(this->getMinimalNumberOfRows());
    otherType currentCoefficient;
    for (int i = 0; i < this->size(); i ++) {
      // note that, at the cost of one extra implicit conversion below, we
      // pReserve the order of multiplication:
      // first is matrix element, then vector coordinate. The code should work
      // as-is for non-commutative fields.
      // (think in the generality of quaternion matrix acting on
      // quaternion-coefficient polynomials!)
      currentCoefficient = this->coefficients[i];
      currentCoefficient *= input[(*this)[i].dualIndex];
      output[(*this)[i].vIndex] += currentCoefficient;
    }
  }
  void actOnVectorROWSOnTheLeft(
    const List<Vector<Coefficient> >& inputStandToTheLeftAsVectorRows,
    List<Vector<Coefficient> >& output
  ) const {
    if (&inputStandToTheLeftAsVectorRows == &output) {
      List<Vector<Coefficient> > inputCopy = inputStandToTheLeftAsVectorRows;
      this->actOnVectorROWSOnTheLeft(inputCopy, output);
      return;
    }
    output.setSize(this->getMinimalNumberOfRows());
    int numberOfColumnsTarget = inputStandToTheLeftAsVectorRows[0].size;
    if (
      this->getMinimalNumberOfColumns() != inputStandToTheLeftAsVectorRows.size
    ) {
      global.fatal
      << "Attemtping to act by matrix "
      << this->toString()
      << " ("
      << this->getMinimalNumberOfColumns()
      << " columns) "
      << " on the "
      << inputStandToTheLeftAsVectorRows.size
      << " vector-rows: "
      << inputStandToTheLeftAsVectorRows.toString()
      << ". "
      << global.fatal;
    }
    for (int i = 0; i < inputStandToTheLeftAsVectorRows.size; i ++) {
      output[i].makeZero(numberOfColumnsTarget);
    }
    for (int i = 0; i < this->size(); i ++) {
      for (int j = 0; j < numberOfColumnsTarget; j ++) {
        output[(*this)[i].vIndex][j] +=
        inputStandToTheLeftAsVectorRows[(*this)[i].dualIndex][j] *
        this->coefficients[i];
      }
    }
  }
  template <class otherType>
  void actOnVectorsColumn(Vectors<otherType>& inputOutput) const {
    for (int i = 0; i < inputOutput.size; i ++) {
      this->actOnVectorColumn(inputOutput[i], inputOutput[i]);
    }
  }
  template <class otherType>
  void actOnVectorColumn(VectorSparse<otherType>& inputOutput) const {
    VectorSparse<otherType> output;
    output.makeZero();
    otherType element;
    MonomialVector tempVM;
    for (int i = 0; i < this->size(); i ++) {
      for (int j = 0; j < inputOutput.size(); j ++) {
        if ((*this)[i].dualIndex == inputOutput[j].monomialIndex) {
          tempVM.monomialIndex = (*this)[i].vIndex;
          element = this->coefficients[i];
          element *= inputOutput.coefficients[j];
          output.addMonomial(tempVM, element);
        }
      }
    }
    inputOutput = output;
  }
  unsigned int hashFunction() const {
    return
    this->::LinearCombination<MonomialMatrix, Coefficient>::hashFunction();
  }
  static unsigned int hashFunction(const MatrixTensor<Coefficient>& input) {
    return input.hashFunction();
  }
  template <class otherClass>
  void operator=(const otherClass& other) {
    this->::LinearCombination<MonomialMatrix, Coefficient>::operator=(other);
  }
  MatrixTensor<Coefficient> operator+(const MatrixTensor<Coefficient>& rightM)
  const {
    MatrixTensor output;
    output = *this;
    output += rightM;
    return output;
  }
};

template <class Coefficient>
void MatrixTensor<Coefficient>::getVectorsSparseFromRowsIncludeZeroRows(
  List<VectorSparse<Coefficient> >& output, int minimalNumberOfRows
) {
  minimalNumberOfRows =
  MathRoutines::maximum(minimalNumberOfRows, this->getMinimalNumberOfRows());
  output.setSize(minimalNumberOfRows);
  for (int i = 0; i < output.size; i ++) {
    output[i].makeZero();
  }
  for (int i = 0; i < this->size(); i ++) {
    int rowIndex = (*this)[i].vIndex;
    int columnIndex = (*this)[i].dualIndex;
    output[rowIndex].addMonomial(
      MonomialVector(columnIndex), this->coefficients[i]
    );
  }
}

template <class Coefficient>
void MatrixTensor<Coefficient>::gaussianEliminationByRowsMatrix(
  MatrixTensor<Coefficient>* carbonCopyMatrix
) {
  List<VectorSparse<Coefficient> > rows;
  List<VectorSparse<Coefficient> > carbonCopyRows;
  int numberOfRows = this->getMinimalNumberOfRows();
  if (carbonCopyMatrix != 0) {
    numberOfRows =
    MathRoutines::maximum(
      numberOfRows, carbonCopyMatrix->getMinimalNumberOfRows()
    );
    carbonCopyMatrix->getVectorsSparseFromRowsIncludeZeroRows(
      carbonCopyRows, numberOfRows
    );
  }
  this->getVectorsSparseFromRowsIncludeZeroRows(rows, numberOfRows);
  List<VectorSparse<Coefficient> >* carbonCopyPointer = carbonCopyMatrix ==
  0 ? 0 : &carbonCopyRows;
  VectorSparse<Coefficient>::gaussianEliminationByRows(
    rows, nullptr, nullptr, nullptr, nullptr, carbonCopyPointer
  );
  this->assignVectorsToRows(rows);
  if (carbonCopyMatrix != 0) {
    carbonCopyMatrix->assignVectorsToRows(carbonCopyRows);
  }
}

template <class Coefficient>
void MatrixTensor<Coefficient>::directSumWith(
  const MatrixTensor<Coefficient>& other
) {
  if (&other == this) {
    MatrixTensor<Coefficient> otherCopy = other;
    this->directSumWith(otherCopy);
    return;
  }
  int indexShift = this->getMinimumNumberOfColumnsNumberOfRows();
  this->setExpectedSize(this->size() + other.size());
  MonomialMatrix currentM;
  for (int i = 0; i < other.size(); i ++) {
    currentM.vIndex = other[i].vIndex + indexShift;
    currentM.dualIndex = other[i].dualIndex + indexShift;
    this->addMonomial(currentM, other.coefficients[i]);
  }
}

template <class Coefficient>
void MatrixTensor<Coefficient>::invert() {
  MatrixTensor<Coefficient> identity;
  identity.makeIdentity(this->getMinimumNumberOfColumnsNumberOfRows());
  MatrixTensor<Coefficient> result = identity;
  this->gaussianEliminationByRowsMatrix(&result);
  if (*this != identity) {
    global.fatal
    << "Attempting to invert a "
    << "non-invertable matrix tensor. "
    << "After Gaussian elimination, the matrix equals "
    << this->toStringMatrixForm()
    << " but should instead be equal to "
    << identity.toStringMatrixForm()
    << global.fatal;
  }
  *this = result;
}

class PolynomialOverModule;

class SlTwoInSlN {
  int getModuleIndexFromHighestWeightVector(const Matrix<Rational>& input) {
    Rational scale;
    for (int i = 0; i < this->highestWeightVectors.size; i ++) {
      if (this->highestWeightVectors.objects[i].isProportionalTo(input, scale))
      {
        return i;
      }
    }
    return - 1;
  }
public:
  int dimension;
  Matrix<Rational> hElement;
  Matrix<Rational> eElement;
  Matrix<Rational> fElement;
  List<int> partition;
  List<Matrix<Rational> > projectors;
  List<Matrix<Rational> > highestWeightVectors;
  List<List<Matrix<Rational> > > gModKModules;
  List<List<List<int> > > pairingTable;
  void getIsPlusKIndexingFrom(int input, int& s, int& k);
  std::string elementMatrixToTensorString(
    const Matrix<Rational>& input, bool useHtml
  );
  std::string initFromModuleDecomposition(
    List<int>& decompositionDimensions, bool useHtml, bool computePairingTable
  );
  std::string initPairingTable(bool useHtml);
  std::string elementModuleIndexToString(int input, bool useHtml);
  std::string getNotationString(bool useHtml);
  bool computeInvariantsOfDegree(
    List<int>& decompositionDimensions,
    int degree,
    List<Polynomial<Rational> >& output,
    std::string& outputError
  );
  std::string pairTwoIndices(
    List<int>& output, int leftIndex, int rightIndex, bool useHtml
  );
  void extractHighestWeightVectorsFromVector(
    Matrix<Rational>& input,
    List<Matrix<Rational> >& outputDecompositionOfInput,
    List<Matrix<Rational> >& outputTheHWVectors
  );
  void climbDownFromHighestWeightAlongSl2String(
    Matrix<Rational>& input,
    Matrix<Rational>& output,
    Rational& outputCoefficients,
    int generatorPower
  );
  void climbUpFromVector(
    Matrix<Rational>& input,
    Matrix<Rational>& outputLastNonZero,
    int& largestPowerNotKillingInput
  );
};

template <typename Coefficient>
void Matrix<Coefficient>::getVectorFromRow(
  int rowIndex, Vector<Coefficient>& output
) const {
  output.setSize(this->numberOfColumns);
  for (int i = 0; i < this->numberOfColumns; i ++) {
    output.objects[i] = this->elements[rowIndex][i];
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::getVectorFromColumn(
  int columnIndex, Vector<Coefficient>& output
) const {
  output.setSize(this->numberOfRows);
  for (int i = 0; i < this->numberOfRows; i ++) {
    output[i] = this->elements[i][columnIndex];
  }
}

class KazhdanLusztigPolynomials: public HashedList<Vector<Rational> > {
public:
  WeylGroupData* weylGroup;
  List<int> multiplicities;
  List<bool> explored;
  int nextToExplore;
  int lowestNonExplored;
  List<List<int> > bruhatOrder;
  List<List<int> > simpleReflectionsActionList;
  List<List<int> > inverseBruhatOrder;
  List<List<Polynomial<Rational> > > kazhdanLuzstigPolynomials;
  List<List<Polynomial<Rational> > > rPolynomials;
  List<List<Rational> > kazhdanLuzstigCoefficients;
  void kazhdanLuzstigCoefficientsToString(
    List<int>& kazhdanLuzstigCoefficients, std::string& output
  );
  void findNextToExplore();
  int findMinimalBruhatNonExplored(List<bool>& explored);
  int findMaximalBruhatNonExplored(List<bool>& explored);
  void initTheMults();
  void compute(int x);
  void check();
  // returns true if computation succeeded, false otherwise
  bool computeRxy(int x, int y, int simpleReflectionIndex);
  void computeKLxy(int x, int y);
  bool isMaximalNonExplored(int index);
  bool indexGEQIndex(int a, int b);
  bool indexGreaterThanIndex(int a, int b);
  std::string toString(FormatExpressions* format = nullptr);
  void mergeBruhatLists(int fromList, int toList);
  std::string KLPolysToString(FormatExpressions* format = nullptr);
  void computeKLCoefficients();
  int chamberIndicatorToIndex(Vector<Rational>& chamberIndicator);
  std::string rPolysToString(FormatExpressions* format = nullptr);
  bool computeKLPolys(WeylGroupData* weylGroup);
  void computeRPolys();
  int computeProductfromSimpleReflectionsActionList(int x, int y);
  void writeKLCoeffsToFile(
    std::fstream& output, List<int>& KLcoeff, int topIndex
  );
  // returns the topIndex of the KL coefficients
  int readKLCoeffsFromFile(std::fstream& input, List<int>& output);
  KazhdanLusztigPolynomials() {
    this->weylGroup = nullptr;
  }
  void generatePartialBruhatOrder();
  void initFromWeyl(WeylGroupData* weylGroup);
};

template <class Coefficient>
void Matrix<Coefficient>::substitute(
  const PolynomialSubstitution<Rational>& substitution
) {
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      this->elements[i][j].substitute(substitution, 1);
    }
  }
}

template <class TemplateMonomial, class Coefficient>
std::ostream& operator<<(
  std::ostream& output,
  const LinearCombination<TemplateMonomial, Coefficient>& collection
) {
  if (collection.size() == 0) {
    output << "0";
    return output;
  }
  std::string coefficientString;
  std::string monomialString;
  List<TemplateMonomial> sortedMonomials;
  sortedMonomials = collection.monomials;
  sortedMonomials.quickSortDescending();
  for (int i = 0; i < sortedMonomials.size; i ++) {
    TemplateMonomial& monomial = sortedMonomials[i];
    std::stringstream termStream;
    Coefficient& coefficient =
    collection.coefficients[collection.monomials.getIndex(monomial)];
    termStream << coefficient;
    coefficientString = termStream.str();
    monomialString = monomial.toString();
    if (coefficientString == "1" && monomialString != "1") {
      coefficientString = "";
    }
    if ((coefficientString == "- 1" || coefficientString == "-1") &&
      monomialString != "1"
    ) {
      coefficientString = "-";
    }
    if (monomialString != "1") {
      coefficientString += monomialString;
    }
    if (i > 0) {
      if (coefficientString.size() > 0) {
        if (coefficientString[0] != '-') {
          output << "+";
        }
      } else {
        output << "+";
      }
    }
    output << coefficientString;
  }
  return output;
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

template <class Coefficient>
void Vectors<Coefficient>::intersectTwoLinearSpaces(
  const List<Vector<Coefficient> >& firstSpace,
  const List<Vector<Coefficient> >& secondSpace,
  List<Vector<Coefficient> >& output
) {
  Vectors<Coefficient> firstReduced, secondReduced;
  Selection selection;
  Vectors<Coefficient>::selectBasisInSubspace(
    firstSpace, firstReduced, selection
  );
  Vectors<Coefficient>::selectBasisInSubspace(
    secondSpace, secondReduced, selection
  );
  if (firstReduced.size == 0 || secondReduced.size == 0) {
    output.size = 0;
    return;
  }
  int dimension = firstReduced[0].size;
  Matrix<Coefficient> matrix;
  matrix.initialize(dimension, firstReduced.size + secondReduced.size);
  for (int i = 0; i < dimension; i ++) {
    for (int j = 0; j < firstReduced.size; j ++) {
      matrix(i, j) = firstReduced[j][i];
    }
    for (int j = 0; j < secondReduced.size; j ++) {
      matrix(i, firstReduced.size + j) = 0;
      matrix(i, firstReduced.size + j) -= secondReduced[j][i];
    }
  }
  matrix.gaussianEliminationByRows(0, &selection);
  output.reserve(selection.cardinalitySelection);
  output.size = 0;
  Vector<Coefficient> nextIntersection;
  for (int i = 0; i < selection.cardinalitySelection; i ++) {
    int currentIndex = selection.elements[i];
    if (currentIndex < firstReduced.size) {
      global.fatal
      << "Unexpected condition in Vectors::intersectTwoLinearSpaces. "
      << global.fatal;
    }
    nextIntersection.makeZero(dimension);
    for (int j = 0; j < firstReduced.size; j ++) {
      if (!selection.selected[j]) {
        nextIntersection +=
        firstReduced[j] * matrix.elements[j][currentIndex];
      }
    }
    output.addOnTop(nextIntersection);
  }
}

template <class Coefficient>
bool Matrix<Coefficient>::isPositiveDefinite() {
  if (this->numberOfRows != this->numberOfColumns) {
    global.fatal
    << "Attempt to evaluate whether a matrix "
    << "is positive definite, but the matrix is not square. "
    << global.fatal;
  }
  Coefficient determinant;
  Matrix<Coefficient> topLeftCorner;
  for (int i = 0; i < this->numberOfRows; i ++) {
    topLeftCorner.initialize(i + 1, i + 1);
    for (int j = 0; j < topLeftCorner.numberOfColumns; j ++) {
      for (int k = 0; k < topLeftCorner.numberOfColumns; k ++) {
        topLeftCorner.elements[j][k] = this->elements[j][k];
      }
    }
    determinant = topLeftCorner.getDeterminant();
    if (determinant <= 0) {
      return false;
    }
  }
  return true;
}

template <class Coefficient>
std::string Vectors<Coefficient>::toLatexInequalities(
  bool lastVariableIsConstant, FormatExpressions& format
) const {
  std::stringstream out;
  std::string polynomialPart;
  out << "\\begin{array}{rcl}";
  for (int i = 0; i < this->size; i ++) {
    Vector<Rational>& current = (*this)[i];
    polynomialPart =
    current.toStringLetterFormat(
      format.polynomialDefaultLetter, &format, lastVariableIsConstant
    );
    if (polynomialPart == "") {
      out << "(0";
    }
    out << polynomialPart;
    if (!lastVariableIsConstant) {
      out << "&\\geq& 0\\\\\n";
    } else {
      out << "\\geq " << (-(*current.lastObject())).toString() << "\\\\";
    }
    if (polynomialPart == "") {
      out << ")";
    }
  }
  out << "\\end{array}";
  return out.str();
}

#endif // header_math_general_ALREADY_INCLUDED
