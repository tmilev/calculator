// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader1_2_h_already_included
#define vpfHeader1_2_h_already_included

#include "general_logging_global_variables.h"
#include "general_strings.h"
#include "math_large_integers.h"
#include "math_vectors.h"
#include "math_subsets_selections.h"
#include "general_file_operations_encodings.h"
#include "web_api.h"

class ElementZmodP;
class WeylGroupData;
class AlgebraicClosureRationals;
template <class Coefficient>
class MatrixTensor;

// We are wrapping the math.h c++ functions for portability reasons
// (if for some reason we want to change from math.h to a better floating point
// library, we only need to change the implementation of the FloatingPoint class.
// Due to conflicts with namespaces, we use function names different from the
// built-in ones.
class FloatingPoint {
public:
  static double absFloating(double argument);
  static double sinFloating(double argument);
  static double cosFloating(double argument);
  static double arccos(double argument);
  static double arctan(double argument);
  static double arcsin(double argument);
  static double sqrtFloating(double argument);
  static double power(double base, double exponent);
  static double logFloating(double argument);
  static double floorFloating(double argument);
  static std::string doubleToString(double input);
};

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
  ChevalleyGenerator(): owner(nullptr), generatorIndex(- 1) {
  }
  friend std::ostream& operator << (std::ostream& output, const ChevalleyGenerator& generator) {
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
  void makeGenerator(SemisimpleLieAlgebra& inputOwner, int inputGeneratorIndex) {
    this->owner = &inputOwner;
    this->generatorIndex = inputGeneratorIndex;
  }
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

template <class Coefficient, unsigned int inputHashFunction(const Coefficient&) = Coefficient::hashFunction>
class MonomialTensor {
  friend std::ostream& operator << (std::ostream& output, const MonomialTensor<Coefficient, inputHashFunction>& monomial) {
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
      result = MathRoutines::maximum(result, this->powers[i].minimalNumberOfVariables());
    }
    return result;
  }
  template<class otherType>
  void operator=(const MonomialTensor<otherType>& other) {
    this->generatorsIndices = other.generatorsIndices;
    this->powers.setSize(other.powers.size);
    for (int i = 0; i < other.powers.size; i ++) {
      this->powers[i] = other.powers[i];
    }
  }
  bool simplifyEqualConsecutiveGenerators(int lowestNonReducedIndex);
  void multiplyByGeneratorPowerOnTheRight(int generatorIndex, const Coefficient& power);
  void multiplyByGeneratorPowerOnTheLeft(int generatorIndexStandsToTheLeft, const Coefficient& power);
  unsigned int hashFunction() const {
    int top = MathRoutines::minimum(someRandomPrimesSize, this->generatorsIndices.size);
    unsigned int result = 0;
    for (int i = 0; i < top; i ++) {
      result += someRandomPrimes[i] * this->generatorsIndices[i] +
      someRandomPrimes[top - 1 - i] * inputHashFunction(this->powers[i]);
    }
    return result;
  }
  static unsigned int hashFunction(const MonomialTensor<Coefficient, inputHashFunction>& input) {
    return input.hashFunction();
  }
  void makeConstant() {
    this->generatorsIndices.size = 0;
    this->powers.size = 0;
  }
  bool operator>(const MonomialTensor<Coefficient, inputHashFunction>& other) const {
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
  bool operator==(const MonomialTensor<Coefficient, inputHashFunction>& other) const {
    return this->powers == other.powers && this->generatorsIndices == other.generatorsIndices;
  }
  bool operator<(const MonomialTensor<Coefficient, inputHashFunction>& right) const {
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
  void operator*=(const MonomialTensor<Coefficient, inputHashFunction>& standsOnTheRight) {
    if (standsOnTheRight.generatorsIndices.size == 0) {
      return;
    }
    if (this == &standsOnTheRight) {
      MonomialTensor<Coefficient, inputHashFunction> tempMon;
      tempMon = standsOnTheRight;
      (*this) *= (tempMon);
      return;
    }
    this->generatorsIndices.setExpectedSize(standsOnTheRight.generatorsIndices.size + this->generatorsIndices.size);
    this->powers.setExpectedSize(standsOnTheRight.generatorsIndices.size + this->generatorsIndices.size);
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

template<typename theType, unsigned int objectHashFunction(const theType&) = theType::hashFunction>
class MonomialWrapper {
  public:
  theType theObject;
  MonomialWrapper() {
  }
  MonomialWrapper(const theType& input) {
    this->theObject = input;
  }
  std::string toString(FormatExpressions* format = nullptr) const {
    (void) format;
    std::stringstream out;
    out << "(" << this->theObject << ")";
    return out.str();
  }
  bool operator>(const MonomialWrapper& other) const {
    return this->theObject > other.theObject;
  }
  friend std::ostream& operator << (std::ostream& output, const MonomialWrapper& theMon) {
    output << theMon.theObject;
    return output;
  }
  static unsigned int hashFunction(const MonomialWrapper& input) {
    return objectHashFunction(input.theObject);
  }
  bool operator==(const MonomialWrapper& other) const {
    return this->theObject == other.theObject;
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
  // MonomialPolynomial's, especially with operator== and MonomialPolynomial::hashFunction!
  // Please modify this class in accordance with what was just explained.
  // Note that by the above token I decided to declare operator[] as non-const
  // function and operator() as a const function but returning a copy of the
  // underlying element, rather than a reference to the element.
  //
  // IMPORTANT. The default monomial order, implemented by operator>, is the graded lexicographic
  // last variable strongest order. Other monomial orders are not referred by operator>, but
  // by their explicit names.
  // Note that the LinearCombination::toString method uses the FormatExpressions::monomialOrder
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
  MonomialPolynomial() {
  }
  friend std::ostream& operator<<(std::ostream& output, const MonomialPolynomial& theMon) {
    output << theMon.toString();
    return output;
  }
  void multiplyByVariable(int variableIndex, const Rational& variablePower);
  const Rational& operator[](int i) const;
  Rational operator()(int i) const;
  unsigned int hashFunction() const {
    return this->monomialBody.hashFunction();
  }
  bool hasPositiveOrZeroExponents() const;
  void exponentMeBy(const Rational& theExp);
  // Warning: hashFunction must return the same result
  // for equal monomials represented by different monBodies.
  // Two such different representation may differ by extra entries filled in with zeroes.
  static unsigned int hashFunction(const MonomialPolynomial& input) {
    unsigned int result = 0;
    int numCycles = MathRoutines::minimum(input.monomialBody.size, someRandomPrimesSize);
    for (int i = 0; i < numCycles; i ++) {
      result += input.monomialBody[i].hashFunction();
    }
    return result;
  }
  std::string toString(FormatExpressions* PolyFormat = nullptr) const;
  void makeOne() {
    this->monomialBody.setSize(0);
  }
  void makeEi(int LetterIndex, int Power = 1, int ExpectedNumVars = 0);
  void setVariable(int variableIndex, const Rational& power);
  void trimTrailingZeroes();
  bool operator>(const MonomialPolynomial& other) const;
  bool isDivisibleBy(const MonomialPolynomial& other) const;
  int totalDegreeInt() const {
    int result = - 1;
    if (!this->totalDegree().isSmallInteger(&result)) {
      global.fatal
      << "The total degree of monomial must be "
      << "a small integer to call this function. " << global.fatal;
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
    int tempI1;
    if (whichLetter == nullptr) {
      whichLetter = &tempI1;
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
  bool substitution(
    const List<Polynomial<Coefficient> >& theSubstitution,
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
  static bool greaterThan_totalDegree_leftLargerWins(const MonomialPolynomial& left, const MonomialPolynomial& right);

  // "Lexicographic" order.
  // If computing with n variables, the "lexicographic order"
  // coincides with the infinite-alphabet dictionary of all words with length n, where
  // each letter is given by the variable exponent.
  // In other words, the lexicographic order for x^2 y^0 z^3 is the "lexicographic" order
  // of the triple [2,0,3].
  // This is confusing, as it is completely different from the "dictionary order" in which
  // the monomial x^2 y^0 z^3 = x x z z z would be placed. For example,
  // x^3 y^3 z > x y^2 z^3 because [3, 3, 0] > [1, 2, 3].
  // However, the word formed by the letters in the monomial are "dictionary-ordered"
  // in the opposite order.
  // x x x y y y z <  x y y z z z.
  // This has lead to lots of confusion in previous version, so we have dropped the
  // term "lexicographic" in all places except the end-user facing calculator commands.
  bool greaterThan_leftLargerWins(const MonomialPolynomial& other) const;
  static bool greaterThan_leftLargerWins(const MonomialPolynomial& left, const MonomialPolynomial& right) {
    return left.greaterThan_leftLargerWins(right);
  }
  // Lexicographic order but with variables ordered in the opposite direction.
  bool greaterThan_rightLargerWins(const MonomialPolynomial& other) const;
  static bool greaterThan_rightLargerWins(const MonomialPolynomial& left, const MonomialPolynomial& right) {
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
  void invert() {
    for (int i = 0; i < this->monomialBody.size; i ++) {
      this->monomialBody[i].negate();
    }
  }
  bool hasSmallIntegralPositivePowers(int* whichtotalDegree) const;
  void raiseToPower(const Rational& thePower);
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
std::ostream& operator<<(std::ostream& output, const Matrix<Coefficient>& theMat);

template <typename Coefficient>
class Matrix {
  //friend std::iostream& operator<< <Coefficient>(std::iostream& output, const Matrix<Coefficient>& theMat);
  friend std::ostream& operator<< <Coefficient>(std::ostream& output, const Matrix<Coefficient>& theMat);
  //friend std::iostream& operator>> <Coefficient>(std::iostream& input, Matrix<Coefficient>& theMat);
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
  void resize(int desiredRows, int desiredColumns, bool preserveValues, const Coefficient* ringZero = nullptr);
  Matrix(): numberOfRows(0), actualNumberOfRows(0), numberOfColumns(0), actualNumberOfColumns(0), elements(nullptr), flagDeallocated(false) {
  }
  Matrix(const Matrix<Coefficient>& other):
    numberOfRows(0), actualNumberOfRows(0), numberOfColumns(0), actualNumberOfColumns(0), elements(0), flagDeallocated(false) {
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
          MathRoutines::swap<Coefficient>(this->elements[j][i], this->elements[i][j]);
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
      standsOnTheRightNew.resize(this->numberOfRows, standsOnTheRight.numberOfColumns, true, &zero);
      this->appendMatrixOnTheRight(standsOnTheRightNew);
      return;
    }
    if (this->numberOfRows < standsOnTheRight.numberOfRows) {
      Coefficient zero;
      zero = 0;
      this->resize(standsOnTheRight.numberOfRows, this->numberOfColumns, true, &zero);
    }
    int oldNumberOfColumns = this->numberOfColumns;
    this->resize(this->numberOfRows, standsOnTheRight.numberOfColumns + oldNumberOfColumns, true);
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = oldNumberOfColumns; j < this->numberOfColumns; j ++) {
        this->elements[i][j] = standsOnTheRight.elements[i][j - oldNumberOfColumns];
      }
    }
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
      standsBelowNew.resize(standsBelow.numberOfRows, this->numberOfColumns, true, &zero);
      this->appendMatrixOnTheRight(standsBelowNew);
      return;
    }
    if (this->numberOfColumns < standsBelow.numberOfColumns) {
      Coefficient theZero;
      theZero = 0;
      this->resize(this->numberOfRows, standsBelow.numberOfColumns, true, &theZero);
    }
    // So far, we have guaranteed that this and
    // &standsBelow have the same number of columns and
    // are different objects.
    int oldNumRows = this->numberOfRows;
    this->resize(this->numberOfRows + standsBelow.numberOfRows, this->numberOfColumns, true);
    for (int i = oldNumRows; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        this->elements[i][j] = standsBelow(i - oldNumRows, j);
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
  void actOnVectorROWSOnTheLeft(List<Vector<Coefficient> >& standOnTheRightAsVectorRow) const {
    List<Vector<Coefficient> > output;
    this->actOnVectorROWSOnTheLeft(standOnTheRightAsVectorRow, output);
    standOnTheRightAsVectorRow = output;
  }
  void actOnVectorROWSOnTheLeft(
    List<Vector<Coefficient> >& standOnTheRightAsVectorRow,
    List<Vector<Coefficient> >& output
  ) const {
    if (this->numberOfColumns != standOnTheRightAsVectorRow.size) {
      global.fatal << "Attempting to multiply a matrix, standing on the left, with "
      << this->numberOfColumns << " columns, by a matrix, standing on the right, with "
      << standOnTheRightAsVectorRow.size << " rows. "
      << global.fatal;
    }
    output.setSize(this->numberOfRows);
    for (int i = 0; i < this->numberOfRows; i ++) {
      output[i].makeZero(standOnTheRightAsVectorRow[0].size);
      for (int j = 0; j < this->numberOfColumns; j ++) {
        output[i] += standOnTheRightAsVectorRow[j] * (*this)(i, j);
      }
    }
  }
  void actMultiplyVectorRowOnTheRight(Vector<Coefficient>& standsOnTheLeft) const {
    Vector<Coefficient> output;
    this->actMultiplyVectorRowOnTheRight(standsOnTheLeft, output);
    standsOnTheLeft = output;
  }
  void actMultiplyVectorRowOnTheRight(const Vector<Coefficient>& standsOnTheLeft, Vector<Coefficient>& output) const {
    if (&standsOnTheLeft == &output) {
      global.fatal << "In actMultiplyVectorRowOnTheRight: output not allowed to equal input. " << global.fatal;
    }
    if (this->numberOfRows != standsOnTheLeft.size) {
      global.fatal << "Number of rows of matrix vector number of elements. " << global.fatal;
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
    output.setSize(this->numberOfRows*this->numberOfColumns);
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
      global.fatal << "Attempt to multiply a matrix with "
      << this->numberOfColumns << " columns with a vector(column) of "
      << " dimension " << input.size << ". " << global.fatal;
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
    int counter = 0;
    for (int i = 0; i < input.numberOfRows; i ++, counter ++) {
      for (int j = 0; j < input.numberOfColumns; j ++, counter ++) {
        if (counter < someRandomPrimesSize) {
          result += input.elements[i][j].hashFunction() * someRandomPrimes[counter];
        } else {
          result += input.elements[i][j].hashFunction() * (i + 1) + j;
        }
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
      global.fatal << "In actOnVectorsColumn: input not allowed to equal output. " << global.fatal;
    }
    if (input.size == 0) {
      return;
    }
    if (this->numberOfColumns != input.getDimension()) {
      global.fatal << "Attempt to act by "
      << this->toString() << " (an " << this->numberOfRows << " x "
      << this->numberOfColumns << " matrix) on a column vector "
      << input.toString() << " (dimension " << input.size << "). " << global.fatal;
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
    Matrix<Coefficient>* constantTerms = 0,
    FormatExpressions* format = nullptr
  ) const;
  std::string toStringPlainText(bool jsonFormat = false) const;
  std::string toStringOneLine(bool jsonFormat = false) const;
  std::string toStringWithBlocks(List<int>& theBlocks);
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
  void makeZeroMatrix(int theDimension, const Coefficient& ringZero = 0) {
    this->initialize(theDimension, theDimension);
    for (int i = 0; i < theDimension; i ++) {
      for (int j = 0; j < theDimension; j ++) {
        this->elements[i][j] = ringZero;
      }
    }
  }
  void actOnMonomialAsDifferentialOperator(const MonomialPolynomial& input, Polynomial<Rational>& output);
  void switchRows(int row1, int row2);
  void switchRowsWithCarbonCopy(int row1, int row2, Matrix<Coefficient>* carbonCopy) {
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
  void substitution(const PolynomialSubstitution<Rational>& substitution);
  Coefficient scalarProduct(const Vector<Coefficient>& left, const Vector<Coefficient>& right) {
    return this->scalarProduct(left, right, static_cast<Coefficient>(0));
  }
  Coefficient scalarProduct(
    const Vector<Coefficient>& left, const Vector<Coefficient>& right, const Coefficient& ringZero
  ) {
    if (left.size != this->numberOfColumns || right.size != this->numberOfRows) {
      global.fatal << "Scalar product using matrix: dimensions of vectors don't match. " << global.fatal;
    }
    Coefficient result, contribution;
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
    if (i < 0 || i >= this->numberOfRows || j < 0 || j >= this->numberOfColumns) {
      global.fatal
      << "Request of (row, column) with index ("
      << i << ", " << j << "), but the present matrix has "
      << this->numberOfRows << " rows and " << this->numberOfColumns
      << " colums. " << global.fatal;
    }
    return this->elements[i][j];
  }
  bool isSquare() const {
    return this->numberOfColumns == this->numberOfRows;
  }
  bool isIdentity() const {
    if (this->numberOfRows != this->numberOfColumns || this->numberOfRows <= 0) {
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
  void makeLinearOperatorFromDomainAndRange(Vectors<Coefficient>& domain, Vectors<Coefficient>& range) {
    Matrix<Coefficient> domainMatrix;
    Matrix<Coefficient> codomainMatrix;
    domainMatrix.assignVectorsToRows(domain);
    codomainMatrix.assignVectorsToRows(range);
    domainMatrix.invert();
    (*this) = domainMatrix * codomainMatrix;
    this->transpose();
  }
  void rowTimesScalar(int rowIndex, const Coefficient& scalar);
  void rowTimesScalarWithCarbonCopy(int rowIndex, const Coefficient& scalar, Matrix<Coefficient>* carbonCopy) {
    this->rowTimesScalar(rowIndex, scalar);
    if (carbonCopy != 0) {
      carbonCopy->rowTimesScalar(rowIndex, scalar);
    }
  }
  void addTwoRows(int fromRowIndex, int toRowIndex, int startColumnIndex, const Coefficient& scalar);
  void addTwoRowsWithCarbonCopy(
    int fromRowIndex,
    int toRowIndex,
    int startColumnIndex,
    const Coefficient& scalar,
    Matrix<Coefficient>* carbonCopy
  ) {
    this->addTwoRows(fromRowIndex, toRowIndex, startColumnIndex, scalar);
    if (carbonCopy != 0) {
      carbonCopy->addTwoRows(fromRowIndex, toRowIndex, startColumnIndex, scalar);
    }
  }
  void subtractRows(int indexRowWeSubtractFrom, int indexSubtracted, int startColumnIndex, const Coefficient& scalar);
  void subtractRowsWithCarbonCopy(
    int indexRowWeSubtractFrom,
    int indexSubtracted,
    int startColumnIndex,
    const Coefficient& scalar,
    Matrix<Coefficient>* carbonCopy
  ) {
    this->subtractRows(indexRowWeSubtractFrom, indexSubtracted, startColumnIndex, scalar);
    if (carbonCopy != 0) {
      carbonCopy->subtractRows(indexRowWeSubtractFrom, indexSubtracted, startColumnIndex, scalar);
    }
  }
  void multiplyOnTheLeft(
    const Matrix<Coefficient>& standsOnTheLeft,
    Matrix<Coefficient>& output,
    const Coefficient& ringZero = 0
  ) const;
  void multiplyOnTheLeft(const Matrix<Coefficient>& standsOnTheLeft, const Coefficient& ringZero = 0);
  void multiplyOnTheRight(const Matrix<Coefficient>& standsOnTheRight) {
    Matrix<Coefficient> temp = *this;
    *this = standsOnTheRight;
    this->multiplyOnTheLeft(temp);
  }
  void nonPivotPointsToEigenVectorMatrixForm(Selection& nonPivotPoints, Matrix<Coefficient>& output);
  void getVectorsFromRows(List<Vector<Coefficient> >& output) {
    output.setSize(this->numberOfRows);
    for (int i = 0; i < this->numberOfRows; i ++) {
      output[i].setSize(this->numberOfColumns);
      for (int j = 0; j < this->numberOfColumns; j ++) {
        output[i][j] = this->elements[i][j];
      }
    }
  }
  void nonPivotPointsToEigenVector(
    Selection& nonPivotPoints,
    Vector<Coefficient>& output,
    const Coefficient& ringUnit = 1,
    const Coefficient& ringZero = 0
  );
  bool invert();
  Matrix<Coefficient> inverse() {
    Matrix<Coefficient> copy = *this;
    if (!copy.invert()) {
      global.fatal << "Request to invert " << copy.toString() << " failed. " << global.fatal;
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
    const Coefficient& theRingOne = 1
  );
  // If m1 corresponds to a linear operator from V1 to V2 and
  // m2 to a linear operator from W1 to W2, then the result of the below function
  // corresponds to the linear operator from V1+W1 to V2+W2 (direct sum)
  // this means you write the matrix m1 in the upper left corner m2 in the lower right
  // and everything else you fill with zeros
  void assignDirectSum(Matrix<Coefficient>& m1, Matrix<Coefficient>& m2);
  // if S and T are endomorphisms of V and W, build the matrix of SⓧT that acts on
  // VⓧW with basis (v1ⓧw1,v1ⓧw2,...,v2ⓧw1,v2ⓧw2,...vnⓧwn)
  void assignTensorProduct(const Matrix<Coefficient>& left, const Matrix<Coefficient>& right);
  void assignVectorsToRows(const List<Vector<Coefficient> >& input) {
    int numCols = - 1;
    if (input.size > 0) {
      numCols = input[0].size;
    }
    this->initialize(input.size, numCols);
    for (int i = 0; i < input.size; i ++) {
      for (int j = 0; j < numCols; j ++) {
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
  void assignVectorToRowKeepOtherRowsIntactNoInitialization(int rowIndex, const Vector<Coefficient>& input) {
    if (input.size != this->numberOfColumns || rowIndex >= this->numberOfRows || rowIndex < 0) {
      global.fatal << "Attempt to assign vector " << input.toString()
      << " (" << input.size << " coordinates) "
      << " to row with index " << rowIndex
      << " in a matrix with "
      << this->numberOfRows << " rows and "
      << this->numberOfColumns << " columns. " << global.fatal;
    }
    for (int i = 0; i < this->numberOfColumns; i ++) {
      this->elements[rowIndex][i] = input[i];
    }
  }
  void assignVectorToColumnKeepOtherColsIntactNoInit(int colIndex, const Vector<Coefficient>& input) {
    if (input.size != this->numberOfRows || colIndex >= this->numberOfColumns || colIndex < 0) {
      global.fatal << "In assignVectorToColumnKeepOtherColsIntactNoInit: bad vector/matrix dimensions. "  << global.fatal;
    }
    for (int i = 0; i < this->numberOfRows; i ++) {
      this->elements[i][colIndex] = input[i];
    }
  }
  void assignBlock(Matrix<Coefficient>& block, int starti, int startj) {
    if (starti + block.numberOfRows > this->numberOfRows || startj + block.numberOfColumns > this->numberOfColumns) {
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
  void directSumWith(const Matrix<Coefficient>& m2, const Coefficient& ringZero = 0);
  bool operator!=(const Matrix<Coefficient>& other) const {
    return !((*this) == other);
  }
  bool operator==(const Matrix<Coefficient>& other) const {
    if (this->numberOfRows != other.numberOfRows || this->numberOfColumns != other.numberOfColumns) {
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
  bool isProportionalTo(const Matrix<Coefficient>& input, Coefficient& outputTimesMeEqualsInput) {
    if (input.numberOfColumns != this->numberOfColumns || input.numberOfRows != this->numberOfRows) {
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
    Matrix<Coefficient> tempMat;
    tempMat = *this;
    tempMat *= outputTimesMeEqualsInput;
    tempMat -= input;
    return tempMat.isEqualToZero();
  }
  //returns true if the system has a solution, false otherwise
  bool rowEchelonFormToLinearSystemSolution(
    Selection& inputPivotPoints,
    Matrix<Coefficient>& inputRightHandSide,
    Matrix<Coefficient>& outputSolution
  );
  void operator+=(const Matrix<Coefficient>& right) {
    if (this->numberOfRows != right.numberOfRows || this->numberOfColumns != right.numberOfColumns) {
      global.fatal << "Attempt to add matrix with "
      << this->numberOfRows << " rows and " << this->numberOfColumns
      << " columns to a matrix with " << right.numberOfRows << " rows and "
      << right.numberOfColumns << " columns. " << global.fatal;
    }
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        this->elements[i][j] += right.elements[i][j];
      }
    }
  }
  LargeIntegerUnsigned findPositiveLCMCoefficientDenominators();
  void operator-=(const Matrix<Coefficient>& right) {
    if (this->numberOfRows != right.numberOfRows || this->numberOfColumns != right.numberOfColumns) {
      global.fatal << "Attempt to subtract from matrix with "
      << this->numberOfRows << " rows and " << this->numberOfColumns
      << " columns a matrix with " << right.numberOfRows << " rows and "
      << right.numberOfColumns << " columns. " << global.fatal;
    }
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        (*this)(i, j) -= right(i, j);
      }
    }
  }
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
    Matrix<Coefficient> tempMat;
    this->lieBracket(*this, right, tempMat);
    *this = tempMat;
  }
  static void lieBracket(const Matrix<Coefficient>& left, const Matrix<Coefficient>& right, Matrix<Coefficient>& output) {
    if (
      left.numberOfColumns != left.numberOfRows ||
      right.numberOfColumns != right.numberOfRows ||
      left.numberOfColumns != right.numberOfColumns
    ) {
      global.fatal << "In lieBracket: bad dimensions of matrices. " << global.fatal;
    }
    Matrix<Coefficient> tempPlus, tempMinus;
    tempPlus = (right);
    tempPlus.multiplyOnTheLeft(left);
    tempMinus = (left);
    tempMinus.multiplyOnTheLeft(right);
    tempPlus -= (tempMinus);
    output = tempPlus;
  }
  // The Gaussian elimination below brings the matrix to a row-echelon form,
  // that is, makes the matrix be something like (example is 4x5):
  // 1 0 3 0 0
  // 0 1 0 0 0
  // 0 0 0 1 0
  // 0 0 0 0 0
  // In the Gaussian elimination below, we define non-pivot points to be indices of the columns
  // that do not have a pivot 1 in them.
  // In the above example, the third (index 2) and fifth (index 4) columns do not have a pivot 1 in them.
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
    const Coefficient& theRingMinusUnit = - 1,
    const Coefficient& ringUnit = 1,
    bool (*comparisonGEQFunction) (const Coefficient& left, const Coefficient& right) = 0
  );
  static bool solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(
    Matrix<Coefficient>& A, Matrix<Coefficient>& b, Matrix<Coefficient>& output
  );
  Coefficient getDeterminant();
  Coefficient getTrace() const;
  void assignMatrixIntegerWithDenominator(Matrix<LargeInteger>& theMat, const LargeIntegerUnsigned& Den);
  void scaleToIntegralForMinimalRationalHeightNoSignChange();
  void getMatrixIntegerWithDenominator(Matrix<LargeInteger>& outputMat, LargeIntegerUnsigned& outputDen);
  void lieBracketWith(const Matrix<Coefficient>& right);
  bool getEigenspacesProvidedAllAreIntegralWithEigenValueSmallerThanDimension(List<Vectors<Coefficient> >& output) const;
  void getZeroEigenSpace(List<Vector<Coefficient> >& output) const {
    Matrix<Coefficient> matrixCopy = *this;
    matrixCopy.getZeroEigenSpaceModifyMe(output);
  }
  void getZeroEigenSpaceModifyMe(List<Vector<Coefficient> >& output);
  void getEigenspaceModifyMe(const Coefficient &inputEigenValue, List<Vector<Coefficient> >& outputEigenspace) {
    for (int i = 0; i < this->numberOfColumns; i ++) {
      this->elements[i][i] -= inputEigenValue;
    }
    this->getZeroEigenSpaceModifyMe(outputEigenspace);
  }
  static bool systemLinearInequalitiesHasSolution(
    Matrix<Coefficient>& matA,
    Matrix<Coefficient>& matb,
    Matrix<Coefficient>& outputPoint
  );
  static bool systemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(
    Matrix<Coefficient>& matA,
    Matrix<Coefficient>& matb,
    Vector<Coefficient>* outputSolution = 0
  );
  static void computePotentialChangeGradient(Matrix<Coefficient>& matA,
    Selection& baseVariables,
    int numberOfTrueVariables,
    int columnIndex,
    Rational& outputChangeGradient,
    bool& hasAPotentialLeavingVariable
  );
  static void getMaxMovementAndLeavingVariableRow(Rational& maxMovement,
    int& leavingVariableRow,
    int enteringVariable,
    Matrix<Coefficient>& tempMatA,
    Vector<Coefficient>& inputVectorX,
    Selection& BaseVariables
  );
  int findPositiveLCMCoefficientDenominatorsTruncated();
  int findPositiveGCDCoefficientNumeratorsTruncated();
  Matrix<Coefficient> operator-(const Matrix<Coefficient>& right) const {
    Matrix<Coefficient> tempMat =(*this);
    tempMat -= right;
    return tempMat;
  }
  Matrix<Coefficient> operator*(const Matrix<Coefficient>& right) const;
  Vector<Coefficient> operator*(const Vector<Coefficient>& v) const;

  void operator=(const Matrix<Coefficient>& other);
  template<class otherType>
  void operator=(const Matrix<otherType>& other) {
    this->resize(other.numberOfRows, other.numberOfColumns, false);
    for (int i = 0; i < this->numberOfRows; i ++) {
      for (int j = 0; j < this->numberOfColumns; j ++) {
        (*this)(i, j) = other(i, j);
      }
    }
  }
  // The following are for compatibility with the FiniteGroup class
  void getCharacteristicPolynomialStandardRepresentation(Polynomial<Coefficient>& out) {
    out.assignCharacteristicPolynomial(*this);
  }
  bool hasDifferentConjugacyInvariantsFrom(Matrix<Coefficient>& other) {
    Polynomial<Coefficient> p, q;
    this->getCharacteristicPolynomialStandardRepresentation(p);
    other.getCharacteristicPolynomialStandardRepresentation(q);
    return !(p == q);
  }
  bool operator>(const Matrix<Coefficient>& right) const {
    if (this->numberOfRows != right.numberOfRows || this->numberOfColumns != right.numberOfColumns) {
      global.fatal << "An attempt was just made to compare two matrices of different dimensions; "
      << "most likely something is wrong in some FiniteGroup, see the frames above. "
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
  int theDimension = strictCone[0].size;
  int numCols = strictCone.size + nonStrictCone.size;
  matA.initialize(theDimension + 1, numCols);
  matb.initialize(theDimension + 1, 1);
  matb.makeZero(); matb.elements[theDimension][0].makeOne();
  for (int i = 0; i < strictCone.size; i ++) {
    for (int k = 0; k < theDimension; k ++) {
      matA.elements[k][i].assign(strictCone[i][k]);
    }
    matA.elements[theDimension][i].makeOne();
  }
  for (int i = 0; i < nonStrictCone.size; i ++) {
    int currentCol = i + strictCone.size;
    for (int k = 0; k < theDimension; k ++) {
      matA.elements[k][currentCol].assign(nonStrictCone[i][k]);
      matA.elements[k][currentCol].negate();
    }
    matA.elements[theDimension][currentCol].makeZero();
  }
  if (!Matrix<Rational>::systemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(
    matA, matb, outputLinearCombo
  )) {
    if (outputSplittingNormal != nullptr) {
      bool tempBool = Vectors<Coefficient>::getNormalSeparatingCones(strictCone, nonStrictCone, *outputSplittingNormal);
      if (!tempBool) {
        global.fatal << "This is an algorithmic/mathematical (hence also programming) error. "
        << "I get that two cones do not intersect, yet there exists no plane separating them. "
        << "Something is wrong with the implementation of the simplex algorithm. "
        << "The input which manifested the problem was: <br>StrictCone: <br>"
        << strictCone.toString() << "<br>Non-strict cone: <br>"
        << nonStrictCone.toString() << "<br>" << global.fatal;
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

template <typename Element>
void Matrix<Element>::resize(int r, int c, bool preserveValues, const Element* const ringZero) {
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
  Element** newElements = nullptr;
  int newActualNumCols = MathRoutines::maximum(this->actualNumberOfColumns, c);
  int newActualNumRows = MathRoutines::maximum(this->actualNumberOfRows, r);
  if (r > this->actualNumberOfRows || c > this->actualNumberOfColumns) {
    newElements  = new Element * [newActualNumRows];
#ifdef AllocationLimitsSafeguard
  GlobalStatistics::globalPointerCounter += newActualNumRows;
  GlobalStatistics::checkPointerCounters();
#endif
    for (int i = 0; i < newActualNumRows; i ++) {
      newElements[i] = new Element[newActualNumCols];
#ifdef AllocationLimitsSafeguard
  GlobalStatistics::globalPointerCounter += newActualNumCols;
  GlobalStatistics::checkPointerCounters();
#endif
    }
  }
  int firstInvalidRow = MathRoutines::minimum(this->numberOfRows, r);
  int firstInvalidCol = MathRoutines::minimum(this->numberOfColumns, c);
  if (preserveValues && newElements != nullptr) {
    for (int j = 0; j < firstInvalidRow; j ++) {
      for (int i = 0; i < firstInvalidCol; i ++) {
        newElements[j][i] = this->elements[j][i];
      }
    }
  }
  if (ringZero != nullptr) {
    if (!preserveValues) {
      firstInvalidRow = 0;
      firstInvalidCol = 0;
    }
    for (int i = 0; i < r; i ++) {
      int colStart = (i < firstInvalidRow) ? firstInvalidCol : 0;
      for (int j = colStart; j < c; j ++) {
        newElements[i][j] = *ringZero;
      }
    }
  }
  if (newElements != nullptr) {
    this->releaseMemory();
    this->elements = newElements;
    this->actualNumberOfColumns = newActualNumCols;
    this->actualNumberOfRows = newActualNumRows;
  }
  this->numberOfColumns = c;
  this->numberOfRows = r;
}

template <typename Coefficient>
void Vectors<Coefficient>::getMatrixRootsToRows(Matrix<Rational>& output) const {
  int tempNumCols = 0;
  if (this->size != 0) {
    tempNumCols = static_cast<int>(this->objects[0].size);
  }
  output.initialize(static_cast<int>(this->size), tempNumCols);
  for (int i = 0; i < this->size; i ++) {
    for (int j = 0; j < tempNumCols; j ++) {
      output.elements[i][j] = this->objects[i][j];
    }
  }
}

template <typename Coefficient>
void Vectors<Coefficient>::getOrthogonalComplement(Vectors<Coefficient>& output, Matrix<Rational>* bilinearForm) {
  if (this->size == 0) {
    if (bilinearForm != nullptr) {
      output.makeEiBasis(bilinearForm->numberOfRows);
      return;
    }
    global.fatal << "Finding orthogonal complement of zero vectors without specifying a bilinear form is "
    << " forbidden: I can't determine the dimension of the ambient vector space. " << global.fatal;
  }
  Matrix<Coefficient> theMatrix;
  theMatrix.assignVectorsToRows(*this);
  if (bilinearForm != nullptr) {
    theMatrix *= *bilinearForm;
  }
  theMatrix.getZeroEigenSpaceModifyMe(output);
}

template <typename Coefficient>
bool Vectors<Coefficient>::computeNormal(Vector<Coefficient>& output, int inputDimension) {
  if (this->size == 0) {
    if (inputDimension == 1) {
      output.makeEi(1, 0);
      return true;
    }
    return false;
  }
  int theDimension = this->objects[0].size;
  Matrix<Coefficient> tempMatrix;
  Selection nonPivotPoints;
  nonPivotPoints.initialize(theDimension);
  output.setSize(theDimension);
  this->gaussianEliminationForNormalComputation(tempMatrix, nonPivotPoints, theDimension);
  if (nonPivotPoints.cardinalitySelection != 1) {
    return false;
  }
  tempMatrix.nonPivotPointsToEigenVector(nonPivotPoints, output);
  return true;
}

template <typename Element>
void Matrix<Element>::operator=(const Matrix<Element>& m) {
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

template <typename Element>
void Matrix<Element>::releaseMemory() {
  for (int i = 0; i < this->actualNumberOfRows; i ++) {
    delete [] this->elements[i];
  }
  delete [] this->elements;
#ifdef AllocationLimitsSafeguard
GlobalStatistics::globalPointerCounter -= this->actualNumberOfRows * this->actualNumberOfColumns + this->actualNumberOfRows;
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
  MacroRegisterFunctionWithName("Matrix::invert");
  if (this->numberOfColumns != this->numberOfRows) {
    global.fatal << "Request to invert a non-square matrix of "
    << this->numberOfRows << " rows and "
    << this->numberOfColumns << " columns. " << global.fatal;
  }
  if (this->numberOfColumns == 0) {
    global.fatal << "0 by 0 matrix inversion is not allowed. " << global.fatal;
  }
  Matrix theInverse;
  theInverse.makeIdentityMatrix(
    this->numberOfRows,
    (*this)(0, 0).one(),
    (*this)(0, 0).zero()
  );
  Selection nonPivotColumns;
  this->gaussianEliminationByRows(&theInverse, &nonPivotColumns, 0);
  if (nonPivotColumns.cardinalitySelection != 0) {
    return false;
  }
  *this = theInverse;
  return true;
}

template <typename Coefficient>
Matrix<Coefficient> Matrix<Coefficient>::operator^(const Matrix<Coefficient>& right) const {
  Matrix<Coefficient> conjugateInverse = right;
  conjugateInverse.invert();
  Matrix<Coefficient> tmp;
  this->multiplyOnTheLeft(conjugateInverse,tmp);
  Matrix<Coefficient> out;
  right.multiplyOnTheLeft(tmp, out);
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
void Matrix<Coefficient>::multiplyOnTheLeft(const Matrix<Coefficient>& standsOnTheLeft, const Coefficient& ringZero) {
  Matrix<Coefficient> tempMat;
  this->multiplyOnTheLeft(standsOnTheLeft, tempMat, ringZero);
  this->operator=(tempMat);
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
    global.fatal << "Attempt to multiply a matrix with "
    << standsOnTheLeft.numberOfColumns
    << " columns by a matrix with " << this->numberOfRows << "rows. " << global.fatal;
  }
  Coefficient tempEl;
  output.initialize(standsOnTheLeft.numberOfRows, this->numberOfColumns);
  for (int i = 0; i < standsOnTheLeft.numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      output.elements[i][j] = ringZero;
      for (int k = 0; k < this->numberOfRows; k ++) {
        tempEl = standsOnTheLeft(i, k);
        tempEl *= this->elements[k][j];
        output.elements[i][j] += (tempEl);
      }
    }
  }
}

template <typename Element>
std::string Matrix<Element>::toStringWithBlocks(List<int>& theBlocks) {
  std::stringstream out;
  std::string tempS;
  out << "\\left(\\begin{array}{";
  int offset = 0;
  int blockIndex = 0;
  for (int j = 0; j < this->numberOfColumns; j ++) {
    out << "c";
    offset ++;
    if (offset == theBlocks.objects[blockIndex]) {
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
      tempS = this->elements[i][j].toString();
      out << tempS;
      if (j != this->numberOfColumns - 1) {
        out << " & ";
      }
    }
    out << "\\\\\n";
    offset ++;
    if (offset == theBlocks.objects[blockIndex]) {
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

template<typename Element>
void Matrix<Element>::assignDirectSum(Matrix<Element>& m1, Matrix<Element>& m2) {
  if (this == &m1 || this == &m2) {
    global.fatal << "In assignDirectSum: output not allowed to equal any of the inputs. " << global.fatal;
  }
  this->resize(m1.numberOfRows + m2.numberOfRows, m1.numberOfColumns + m2.numberOfColumns, false);
  this->makeZero();
  for (int i = 0; i < m1.numberOfRows; i ++) {
    for (int j = 0; j < m1.numberOfColumns; j ++) {
      this->elements[i][j] = m1.elements[i][j];
    }
  }
  for (int i = 0; i < m2.numberOfRows; i ++) {
    for (int j = 0; j < m2.numberOfColumns; j ++) {
      this->elements[i + m1.numberOfRows][j + m1.numberOfColumns] = m2.elements[i][j];
    }
  }
}

template<typename Element>
void Matrix<Element>::assignTensorProduct(const Matrix<Element>& left, const Matrix<Element>& right) {
  //handle lazy programmers:
  if (this == &left || this == &right) {
    Matrix<Element> leftCopy = left;
    Matrix<Element> rightCopy = right;
    this->assignTensorProduct(leftCopy, rightCopy);
    return;
  }
  this->resize(left.numberOfRows*right.numberOfRows, left.numberOfColumns*right.numberOfColumns, false);
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
          this->elements[iv * sr + iw][jv * sc + jw] = left.elements[iv][jv] * right.elements[iw][jw];
        }
      }
    }
  }
}

template<typename Element>
void Matrix<Element>::directSumWith(const Matrix<Element>& m2, const Element& ringZero) {
  int oldNumRows = this->numberOfRows;
  int oldNumCols = this->numberOfColumns;
  this->resize(this->numberOfRows + m2.numberOfRows, this->numberOfColumns + m2.numberOfColumns, true);
  for (int i = 0; i < m2.numberOfRows; i ++) {
    for (int j = 0; j < m2.numberOfColumns; j ++) {
      this->elements[i + oldNumRows][j + oldNumCols] = m2.elements[i][j];
    }
    for (int j = 0; j < oldNumCols; j ++) {
      this->elements[i + oldNumRows][j] = ringZero;
    }
  }
  for (int j = 0; j < oldNumRows; j ++) {
    for (int i = oldNumCols; i < this->numberOfColumns; i ++) {
      this->elements[j][i] = ringZero;
    }
  }
}

template <typename Element>
int Matrix<Element>::findPivot(int columnIndex, int rowStartIndex) {
  for (int i = rowStartIndex; i < this->numberOfRows; i ++) {
    if (!this->elements[i][columnIndex].isEqualToZero()) {
      return i;
    }
  }
  return - 1;
}

template <typename Element>
void Matrix<Element>::subtractRows(
  int indexRowWeSubtractFrom,
  int indexSubtracted,
  int startColumnIndex,
  const Element& scalar
) {
  Element tempElement;
  for (int i = startColumnIndex; i < this->numberOfColumns; i ++) {
    tempElement = this->elements[indexSubtracted][i];
    tempElement *= scalar;
    this->elements[indexRowWeSubtractFrom][i] -= tempElement;
  }
}

template <typename Element>
void Matrix<Element>::rowTimesScalar(int rowIndex, const Element& scalar) {
  for (int i = 0; i < this->numberOfColumns; i ++) {
    this->elements[rowIndex][i] *= scalar;
  }
}

template <typename Element>
void Matrix<Element>::switchRows(int row1, int row2) {
  if (row1 == row2) {
    return;
  }
  Element* tmp = this->elements[row1];
  this->elements[row1] = this->elements[row2];
  this->elements[row2] = tmp;
}

template <typename Element>
bool Matrix<Element>::solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(
  Matrix<Element>& A, Matrix<Element>& b, Matrix<Element>& output
) {
  if (A.numberOfRows != b.numberOfRows) {
    global.fatal << "Number of matrix rows does not match number of vector entries. " << global.fatal;
  }
  Selection thePivotPoints;
  A.gaussianEliminationByRows(&b, 0, &thePivotPoints);
  return A.rowEchelonFormToLinearSystemSolution(thePivotPoints, b, output);
}

template <typename Element>
bool Matrix<Element>::rowEchelonFormToLinearSystemSolution(
  Selection& inputPivotPoints, Matrix<Element>& inputRightHandSide, Matrix<Element>& outputSolution
) {
  if (
    inputPivotPoints.numberOfElements != this->numberOfColumns ||
    inputRightHandSide.numberOfColumns != 1 ||
    inputRightHandSide.numberOfRows != this->numberOfRows
  ) {
    global.fatal << "In rowEchelonFormToLinearSystemSolution: bad matrix dimensions. " << global.fatal;
  }
  outputSolution.initialize(this->numberOfColumns, 1);
  int numPivots = 0;
  for (int i = 0; i < this->numberOfColumns; i ++) {
    if (inputPivotPoints.selected[i]) {
      outputSolution(i, 0) = inputRightHandSide(numPivots, 0);
      numPivots ++;
    } else {
      outputSolution(i, 0).makeZero();
    }
  }
  for (int i = numPivots; i < this->numberOfRows; i ++) {
    if (!inputRightHandSide.elements[i][0].isEqualToZero()) {
      return false;
    }
  }
  return true;
}

template <typename Element>
void Matrix<Element>::gaussianEliminationByRowsNoRowSwapPivotPointsByRows(
  int firstNonProcessedRow,
  List<int>& outputPivotPointCols,
  Selection* outputNonPivotPoints__WarningSelectionNotInitialized
) {
  outputPivotPointCols.setSize(this->numberOfRows);
  Element tempElement;
  for (int i = firstNonProcessedRow; i < this->numRows; i ++) {
    int currentPivotCol = - 1;
    for (int j = 0; j < this->numberOfColumns; j ++) {
      if (!this->elements[i][j].isEqualToZero()) {
        currentPivotCol = j;
        break;
      }
    }
    outputPivotPointCols.objects[i] = currentPivotCol;
    if (currentPivotCol != - 1) {
      tempElement = this->elements[i][currentPivotCol];
      tempElement.invert();
      this->rowTimesScalar(i, tempElement);
      for (int j = 0; j < this->numberOfRows; j ++) {
        if (i != j) {
          if (!this->elements[j][i].isEqualToZero()) {
            tempElement.assign(this->elements[j][i]);
            this->subtractRows(j, i, 0, tempElement);
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
        outputNonPivotPoints__WarningSelectionNotInitialized->selected[outputPivotPointCols.objects[i]] = false;
      }
    }
    outputNonPivotPoints__WarningSelectionNotInitialized->computeIndicesFromSelection();
  }
}

template <typename Element>
void Matrix<Element>::makeZero(const Element& ringZero) {
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      this->elements[i][j] = ringZero;
    }
  }
}

template <typename Element>
void Matrix<Element>::makeIdentity(
  const Matrix<Element>& prototype,
  const Element& ringZero,
  const Element& theRingOne
) {
  this->initialize(prototype.numberOfRows, prototype.numberOfColumns);
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      this->elements[i][j] = i == j ? theRingOne : ringZero;
    }
  }
}

template <typename Element>
void Matrix<Element>::initialize(
  int desiredNumberOfRows, int desiredNumberOfColumns
) {
  this->resize(desiredNumberOfRows, desiredNumberOfColumns, false);
}

class FormatExpressions {
public:
  //alphabetBases must contain at least two elements
  std::string chevalleyGgeneratorLetter;
  std::string chevalleyHgeneratorLetter;
  std::string fundamentalWeightLetter;
  std::string polyDefaultLetter;
  std::string weylAlgebraDefaultLetter;
  std::string customPlusSign;
  std::string customCoefficientMonomialSeparator;
  std::string FDrepLetter;
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
  static const int ExpressionLineBreak = 50;
  bool flagDontCollalpseProductsByUnits;
  bool flagPassCustomCoeffMonSeparatorToCoeffs;
  bool flagMakingExpressionTableWithLatex;
  bool flagUseLatex;
  bool flagUsePmatrix;
  bool flagUsePNG;
  bool flagUseHTML;
  bool flagUseHtmlAndStoreToHD;
  bool flagUseMathSpanPureVsMouseHover;
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
  std::string suffixLinearCombination;
  char ambientWeylLetter;
  List<MonomialPolynomial>::Comparator monomialOrder;
  template <typename TemplateMonomial>
  typename List<TemplateMonomial>::Comparator* getMonomialOrder();
  std::string getPolynomialLetter(int index) const;
  FormatExpressions();
};

class MonomialWeylAlgebra {
  public:
  MonomialPolynomial polynomialPart;
  MonomialPolynomial differentialPart;
  friend std::ostream& operator << (std::ostream& output, const MonomialWeylAlgebra& theMon) {
    output << theMon.toString();
    return output;
  }
  bool isConstant() const {
    return this->polynomialPart.isConstant() && this->differentialPart.isConstant();
  }
  std::string toString(FormatExpressions* format = nullptr) const;
  static unsigned int hashFunction(const MonomialWeylAlgebra& input) {
    return
    input.polynomialPart.hashFunction() +
    input.differentialPart.hashFunction() * someRandomPrimes[0];
  }
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  int minimalNumberOfVariables() const {
    return MathRoutines::maximum(
      this->polynomialPart.minimalNumberOfVariables(),
      this->differentialPart.minimalNumberOfVariables()
    );
  }
  bool operator==(const MonomialWeylAlgebra& other) const {
    return this->polynomialPart == other.polynomialPart &&
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
      int i = 0;
      i < this->differentialPart.minimalNumberOfVariables();
      i ++
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
std::iostream& operator<<(std::iostream& output, const Polynomial<Coefficient>& input) {
  output << input.toString();
  return output;
}

template <class TemplateMonomial, class Coefficient>
std::ostream& operator<<(
  std::ostream& output,
  const LinearCombination<TemplateMonomial, Coefficient>& theCollection
);

template <class TemplateMonomial, class Coefficient>
class LinearCombination {
private:
  void addOnTop(const MonomialPolynomial& tempP);//<-to guard the now unsafe base class method
  void clear();//<-to guard the now unsafe base class method
  friend std::ostream& operator<< <TemplateMonomial, Coefficient>(
    std::ostream& output, const LinearCombination<TemplateMonomial, Coefficient>& theCollection
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
  bool needsParenthesisForMultiplication(FormatExpressions* unused = nullptr) const {
    (void) unused;
    return this->size() > 1;
  }
  std::string toString(FormatExpressions* format = nullptr) const;
  int size() const {
    return this->monomials.size;
  }
  // The hash function of Monomial collection must return the same value for
  // monomial collections whose monomials are permuted.
  static unsigned int hashFunction(const LinearCombination<TemplateMonomial, Coefficient>& input) {
    unsigned int result = 0;
    for (int i = 0; i < input.size(); i ++) {
      result += input.coefficients[i].hashFunction() * input[i].hashFunction();
    }
    return result;
  }
  void quickSortAscending(typename List<TemplateMonomial>::Comparator* theOrder = nullptr) {
    List<TemplateMonomial> theSortedMons = this->monomials;
    theSortedMons.quickSortAscending(theOrder, &this->coefficients);
    this->monomials = theSortedMons;
  }
  void quickSortDescending(typename List<TemplateMonomial>::Comparator* theOrder = nullptr) {
    List<TemplateMonomial> theSortedMons = this->monomials;
    theSortedMons.quickSortDescending(theOrder, &this->coefficients);
    this->monomials = theSortedMons;
  }
  void getVectorMonomialsAscending(Vector<Coefficient>& result);
  void getVectorMonomialsDescending(Vector<Coefficient>& result);
  void popMonomial(int index) {
    this->monomials.removeIndexSwapWithLast(index);
    this->coefficients.removeIndexSwapWithLast(index);
  }
  void addOtherTimesConst(
    LinearCombination<TemplateMonomial, Coefficient>& other,
    const Coefficient& theConst
  );
  void popMonomial(int index, TemplateMonomial& outputMon, Coefficient& outputCoeff) {
    outputMon = (*this)[index];
    outputCoeff = this->coefficients[index];
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
  void getMinimalMonomial(TemplateMonomial& outputMon, Coefficient& outputCF) const {
    if (this->isEqualToZero()) {
      global.fatal << "Calling getMinimalMonomial "
      << "on a zero monomial collection is forbidden. " << global.fatal;
    }
    outputMon = (*this)[0];
    outputCF = this->coefficients[0];
    for (int i = 1; i < this->size(); i ++) {
      if (outputMon > (*this)[i]) {
        outputMon = (*this)[i];
        outputCF = this->coefficients[i];
      }
    }
  }

  Coefficient getLeadingCoefficient(const typename List<TemplateMonomial>::Comparator* monomialOrder) {
    Coefficient result;
    this->getIndexLeadingMonomial(nullptr, &result, monomialOrder);
    return result;
  }

  TemplateMonomial getLeadingMonomial(const typename List<TemplateMonomial>::Comparator* monomialOrder) {
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
  bool cleanupMonomialIndex(int theIndex) {
    if (theIndex == - 1) {
      return false;
    }
    // Note: operator != is not required to be defined
    // for our coefficients; operator == is.
    bool coefficientIsZero = (this->coefficients[theIndex] == 0);
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
    this->monomials.removeIndexSwapWithLast(theIndex);
    this->coefficients.removeIndexSwapWithLast(theIndex);
    this->flagDeallocated = oldFlagDeallocated;
    return true;
  }
  int addMonomialNoCoefficientCleanUpReturnIndex(
    const TemplateMonomial& inputMonomial, const Coefficient& inputCoefficient
  );
  template <class LinearCombinationTemplate>
  static void gaussianEliminationByRows(
    List<LinearCombinationTemplate>& toBeEliminated,
    bool *madeARowSwitch = nullptr,
    HashedList<TemplateMonomial>* seedMonomials = nullptr,
    Matrix<Coefficient>* carbonCopyMatrix = nullptr,
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
    int rightRank = LinearCombination::getRankElementSpan(right, seedMonomials);
    return leftRank + rightRank -
    LinearCombination<TemplateMonomial, Coefficient>::getRankElementSpan(listCopy, seedMonomials);
  }
  template <class LinearCombinationTemplate>
  static bool vectorSpacesIntersectionIsNonTrivial(
    const List<LinearCombinationTemplate>& left,
    const List<LinearCombinationTemplate>& right,
    HashedList<TemplateMonomial>* seedMonomials = nullptr
  ) {
    int intersectionRank = LinearCombination<TemplateMonomial, Coefficient>::getRankIntersectionVectorSpaces(
      left, right, seedMonomials
    );
    return intersectionRank != 0;
  }
  template <class LinearCombinationTemplate>
  static bool linearSpanContains(
    const List<LinearCombinationTemplate>& theList,
    const LinearCombinationTemplate& input,
    HashedList<TemplateMonomial>* seedMonomials = nullptr
  ) {
    List<LinearCombinationTemplate> listCopy = theList;
    LinearCombination<TemplateMonomial, Coefficient>::gaussianEliminationByRowsDeleteZeroRows(
      listCopy, 0, seedMonomials
    );
    int startSpanSize = listCopy.size;
    listCopy.addOnTop(input);
    LinearCombination<TemplateMonomial, Coefficient>::gaussianEliminationByRowsDeleteZeroRows(
      listCopy, 0, seedMonomials
    );
    return listCopy.size == startSpanSize;
  }
  template <class LinearCombinationTemplate>
  static bool linearSpanContainsGetFirstLinearCombination(
    const List<LinearCombinationTemplate>& theList,
    const LinearCombinationTemplate& input,
    Vector<Coefficient>& outputFirstLinearCombination,
    HashedList<TemplateMonomial>* seedMonomials = nullptr
  );
  bool hasRationalCoefficients(
    LinearCombination<TemplateMonomial, Rational>* outputConversionToRationals = nullptr
  ) {
    Rational tempRat;
    Rational* theCF = nullptr;
    if (outputConversionToRationals != nullptr) {
      theCF = &tempRat;
      outputConversionToRationals->makeZero();
    }
    for (int i = 0; i < this->size(); i ++) {
      if (!this->coefficients[i].isRational(theCF)) {
        return false;
      } else {
        if (outputConversionToRationals != nullptr) {
          outputConversionToRationals->addMonomial((*this)[i], *theCF);
        }
      }
    }
    return true;
  }
  template <class LinearCombinationTemplate>
  static int getRankElementSpan(
    const List<LinearCombinationTemplate>& theList,
    HashedList<TemplateMonomial>* seedMonomials = nullptr
  ) {
    List<LinearCombinationTemplate> listCopy = theList;
    LinearCombination<TemplateMonomial, Coefficient>::gaussianEliminationByRowsDeleteZeroRows(listCopy, 0, seedMonomials);
    return listCopy.size;
  }
  template <class LinearCombinationTemplate>
  static void gaussianEliminationByRowsDeleteZeroRows(
    List<LinearCombinationTemplate >& theList,
    bool *IvemadeARowSwitch = nullptr,
    HashedList<TemplateMonomial>* seedMonomials = nullptr
  ) {
    LinearCombinationTemplate::gaussianEliminationByRows(
      theList, IvemadeARowSwitch, seedMonomials
    );
    for (int j = theList.size - 1; j >= 0; j --) {
      if (theList[j].isEqualToZero()) {
        theList.size --;
      }
    }
  }
  int subtractMonomialNoCoefficientCleanUpReturnIndex(
    const TemplateMonomial& inputMon, const Coefficient& inputCoeff
  );
  void cleanUpZeroCoefficients() {
    for (int i = 0; i < this->size; i ++) {
      if (this->cleanupMonomialIndex(i)) {
        i --;
      }
    }
  }
  template<class baseType>
  void substitutionCoefficients(const List<Polynomial<baseType> >& theSub) {
    MacroRegisterFunctionWithName("Polynomial::substitutionCoefficients");
    Coefficient newCoeff;
    for (int i = 0; i < this->size(); i ++) {
      newCoeff = this->coefficients[i];
      newCoeff.substitution(theSub, 1);
      if (newCoeff.isEqualToZero()) {
        this->popMonomial(i);
        i --;
        continue;
      }
      this->coefficients[i] = newCoeff;
    }
  }
  void subtractMonomial(const TemplateMonomial& inputMon, const Coefficient& inputCoeff) {
    this->cleanupMonomialIndex(this->subtractMonomialNoCoefficientCleanUpReturnIndex(inputMon, inputCoeff));
  }
  Coefficient getCoefficientOf(const TemplateMonomial& inputMon) const {
    int theIndex = this->monomials.getIndex(inputMon);
    if (theIndex == - 1) {
      if (this->coefficients.size > 0) {
        return this->coefficients[0].zero();
      }
      return Coefficient::zeroStatic();
    }
    return this->coefficients[theIndex];
  }
  bool operator>(const LinearCombination<TemplateMonomial, Coefficient>& other) const {
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
      global.fatal << "Use after free of monomial collection. " << global.fatal;
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
  Coefficient scaleNormalizeLeadingMonomial(const typename List<TemplateMonomial>::Comparator* monomialOrder) {
    MacroRegisterFunctionWithName("Polynomial::scaleNormalizeLeadingMonomial");
    if (this->isEqualToZero()) {
      return Coefficient();
    }
    int indexLeadingMonomial = this->getIndexLeadingMonomial(
      nullptr, nullptr, monomialOrder
    );
    Coefficient result = Coefficient::scaleNormalizeIndex(
      this->coefficients, indexLeadingMonomial
    );
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
      global.fatal << "A monomial collection has "
      << this->monomials.size << " monomials but "
      << this->coefficients.size << " coefficients. " << global.fatal;
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
      result = MathRoutines::maximum(result, (*this)[i].minimalNumberOfVariables());
    }
    return result;
  }
  bool isSmallInteger(int* whichInteger = nullptr) const;
  bool isInteger(LargeInteger* whichInteger = nullptr) const;
  void setExpectedSize(int theSize) {
    this->monomials.setExpectedSize(theSize);
    this->coefficients.setExpectedSize(theSize);
  }
  bool hasGreaterThanOrEqualToMonomial(TemplateMonomial& m, int& whichIndex);
  void writeToFile(std::fstream& output);
  void clearDenominators(Rational& output) {
    output.makeOne();
    Rational tempRat;
    for (int i = 0; i < this->size(); i ++) {
      if (!this->coefficients[i].isInteger()) {
        tempRat = this->coefficients[i].getDenominator();
        *this *= tempRat;
        output *= tempRat;
      }
    }
  }
  bool readFromFile(std::fstream& input);
  bool operator!=(const LinearCombination<TemplateMonomial, Coefficient>& other) const {
    return !(*this == other);
  }
  bool operator==(const LinearCombination<TemplateMonomial, Coefficient>& other) const;
  bool operator==(int x) const;
  void operator+=(const LinearCombination<TemplateMonomial, Coefficient>& other);
  LinearCombination<TemplateMonomial, Coefficient> operator*(const Coefficient& other) const {
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
  void makeConstant(const Coefficient& coeff) {
    TemplateMonomial tempM;
    tempM.makeOne();
    this->addMonomial(tempM, coeff);
  }
  void makeOne() {
    this->makeConstant(1);
  }
  void operator+=(const Coefficient& other) {
    TemplateMonomial tempM;
    tempM.makeOne();
    this->addMonomial(tempM, other);
  }
  void operator-=(const Coefficient& other) {
    TemplateMonomial tempM;
    tempM.makeOne();
    this->subtractMonomial(tempM, other);
  }
  template <class OtherType>
  void assignOtherType(const LinearCombination<TemplateMonomial, Coefficient>& other) {
    this->::HashedList<TemplateMonomial>::operator=(other);
    this->coefficients.setSize(other.size);
    for (int i = 0; i < other.size; i ++) {
      this->coefficients[i] = other.coefficients[i];
    }
  }
  void operator-=(const LinearCombination<TemplateMonomial, Coefficient>& other) {
    this->subtractOtherTimesCoefficient(other);
  }
  void subtractOtherTimesCoefficient(
    const LinearCombination<TemplateMonomial, Coefficient>& other,
    Coefficient* inputCoefficient = nullptr
  );
  template <class otherType>
  void operator/=(const otherType& other) {
    if (other == 0) {
      global.fatal << "A LinearCombination division "
      << "by zero has been requested: division by zero error should "
      << "be handled before calling operator/=. " << global.fatal;
      return;
    }
    for (int i = 0; i < this->coefficients.size; i ++) {
      this->coefficients[i] /= other;
    }
  }
  template <class otherType>
  void operator*=(const otherType& other) {
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
  const TemplateMonomial& operator[](int theIndex) const {
    return this->monomials[theIndex];
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
  template<class otherType>
  void operator=(const LinearCombination<TemplateMonomial, otherType>& other) {
    this->coefficients = other.coefficients;
    this->monomials = other.monomials;
  }
};

class MonomialVector {
  friend std::ostream& operator << (std::ostream& output, const MonomialVector& monomial) {
    output << monomial.toString();
    return output;
  }
  public:
  int theIndex;
  MonomialVector(): theIndex(- 1) {
  }
  MonomialVector(int inputIndex): theIndex(inputIndex) {
  }
  std::string toString(FormatExpressions* format = nullptr) const;
  unsigned int hashFunction() const {
    return static_cast<unsigned int>(this->theIndex);
  }
  static unsigned int hashFunction(const MonomialVector& input) {
    return input.hashFunction();
  }
  bool operator==(const MonomialVector& other) const {
    return this->theIndex == other.theIndex;
  }
  void makeEi(int inputIndex) {
    this->theIndex = inputIndex;
  }
  bool operator>(const MonomialVector& other) const {
    return this->theIndex > other.theIndex;
  }
};

template <class Coefficient>
class VectorSparse : public LinearCombination<MonomialVector, Coefficient> {
  public:
  void makeEi(int nonZeroIndex, const Coefficient& coefficient = 1) {
    this->makeZero();
    MonomialVector theMon;
    theMon.makeEi(nonZeroIndex);
    this->addMonomial(theMon, coefficient);
  }
  int getLargestParticipatingBasisIndex() {
    int result = - 1;
    for (int i = 0; i < this->size(); i ++) {
      result = MathRoutines::maximum(result, (*this)[i].theIndex);
    }
    return result;
  }
  bool operator>(const VectorSparse<Coefficient>& other) const {
    return this->::LinearCombination<MonomialVector, Coefficient>::operator>(other);
  }
};

template <class TemplateMonomial, class Coefficient>
class ElementMonomialAlgebra: public LinearCombination<TemplateMonomial, Coefficient> {
  public:
  void multiplyBy(
    const ElementMonomialAlgebra<TemplateMonomial, Coefficient>& other,
    ElementMonomialAlgebra<TemplateMonomial, Coefficient>& output,
    ElementMonomialAlgebra<TemplateMonomial, Coefficient>& bufferPoly,
    TemplateMonomial& bufferMon
  ) const;
  void multiplyBy(
    const TemplateMonomial& other,
    const Coefficient& theCoeff,
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
  void multiplyBy(const MonomialPolynomial& other, const Coefficient& theCoeff) {
    return this->multiplyBy(other, theCoeff, *this);
  }
  void multiplyBy(
    const MonomialPolynomial& other,
    const Coefficient& theCoeff,
    ElementMonomialAlgebra<TemplateMonomial, Coefficient>& output
  ) const {
    this->multiplyBy(other, output);
    output *= theCoeff;
  }

  void multiplyOnTheLeft(
    const ElementMonomialAlgebra<TemplateMonomial, Coefficient>& standsOnTheLeft
  ) {
    ElementMonomialAlgebra<TemplateMonomial, Coefficient> tempMat, bufferPoly;
    TemplateMonomial bufferMon;
    standsOnTheLeft.multiplyBy(*this, tempMat, bufferPoly, bufferMon);
    *this = tempMat;
  }
  void operator*=(const ElementMonomialAlgebra<TemplateMonomial, Coefficient>& other) {
    ElementMonomialAlgebra<TemplateMonomial, Coefficient> bufferPoly;
    TemplateMonomial bufferMon;
    this->multiplyBy(other, *this, bufferPoly, bufferMon);
  }
  void operator*=(const Coefficient& other) {
    this->::LinearCombination<TemplateMonomial, Coefficient>::operator*=(other);
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
    ElementMonomialAlgebra<TemplateMonomial, Coefficient> theOne;
    theOne.makeConstant(1);
    MathRoutines::raiseToPower(*this, d, theOne);
  }
};

template<class Coefficient>
class PolynomialOrder {
  public:
  List<MonomialPolynomial>::Comparator monomialOrder;
  bool compareLeftGreaterThanRight(const Polynomial<Coefficient>& left, const Polynomial<Coefficient>& right) const;
};

template<class Coefficient>
class Polynomial: public ElementMonomialAlgebra<MonomialPolynomial, Coefficient> {
private:
public:
  friend std::iostream& operator<< <Coefficient>(std::iostream& output, const Polynomial<Coefficient>& input);
  Polynomial(int x) {
    this->makeConstant(x);
  }
  Polynomial(){}
  Polynomial(const Polynomial<Coefficient>& other) {
    this->operator=(other);
  }
  unsigned int hashFunction() const {
    return this->::LinearCombination<MonomialPolynomial, Coefficient>::hashFunction();
  }
  static unsigned int hashFunction(const Polynomial<Coefficient>& input) {
    return input.hashFunction();
  }
  //////////////////////////////////////////////
  void assignFloor() const {
  }
  void assignMinimalPolynomial(const Matrix<Coefficient>& input);
  void assignCharacteristicPolynomial(const Matrix<Coefficient>& input);
  void assignMonomialWithExponent(Vector<Rational>& r, const Coefficient& theCoeff = 1) {
    this->makeZero();
    this->addMonomial(r, theCoeff);
  }
  void getConstantTerm(Coefficient& output, const Coefficient& ringZero = 0) const;
  Coefficient getConstantTerm(const Coefficient& ringZero = 0) const {
    Coefficient result;
    this->getConstantTerm(result, ringZero);
    return result;
  }
  static void getValuesLagrangeInterpolands(
    Vector<Rational>& pointsOfInterpolation,
    Vector<Rational>& pointsOfEvaluation,
    Vectors<Rational>& outputValuesInterpolands
  );
  bool isSquareFree(
    const Coefficient& one,
    std::stringstream* comments
  ) const;
  bool isSquareFreeAndUnivariate(
    const Coefficient& one,
    std::stringstream* comments
  ) const;
  // Given polynomial p, returns d/dx_1 p dx_1 + ... + d/dx_n p dx_n.
  // The differentials dx_1, ..., dx_n are introduced as additional variables.
  bool differential(
    Polynomial<Coefficient>& output, std::stringstream* comments
  ) const;
  // Similar to the preceding function but returns a vector.
  bool differential(
    Vector<Polynomial<Coefficient> >& output, std::stringstream* comments
  ) const;
  void interpolate(const Vector<Coefficient>& thePoints, const Vector<Coefficient>& valuesAtThePoints);
  bool findOneVariableRationalRoots(List<Rational>& output);
  Coefficient getDiscriminant();
  void getCoefficientInFrontOfLinearTermVariableIndex(int index, Coefficient& output);
  void makeMonomial(
    int letterIndex,
    const Rational& power,
    const Coefficient& inputCoefficient = 1
  );
  void makeDegreeOne(int numberOfVariables, int nonZeroIndex, const Coefficient& coefficient);
  void makeDegreeOne(
    int numberOfVariables,
    int nonZeroIndex1,
    int nonZeroIndex2,
    const Coefficient& coefficient1,
    const Coefficient& coefficient2
  );
  void makeDegreeOne(int numberOfVariables, int nonZeroIndex, const Coefficient& coefficient1, const Coefficient& constantTerm);
  void makeLinearNoConstant(const Vector<Rational>& inputCoefficients);
  void makeLinearWithConstantTerm(const Vector<Rational>& inputLastCoordinateConstantTerm);
  void makePolynomialFromDirectionAndNormal(
    Vector<Coefficient>& direction,
    Vector<Coefficient>& normal,
    Coefficient& Correction
  );
  static bool getLinearSystemFromLinearPolynomials(
    const List<Polynomial<Coefficient> >& linearPolynomials,
    Matrix<Coefficient>& homogenousPart,
    Matrix<Coefficient>& constTerms
  );
  bool isOneVariablePolynomial(int* whichVariable = nullptr) const;
  bool isOneVariableNonConstantPolynomial(int* whichVariable = nullptr) const;
  Polynomial<Coefficient> one() const;
  static Polynomial<Coefficient> zero();
  Rational rationalValue();
  void makeDeterminantFromSquareMatrix(const Matrix<Polynomial<Coefficient> >& theMat);
  void makeConstant(const Coefficient& constant) {
    this->makeZero();
    MonomialPolynomial one;
    one.makeOne();
    this->addMonomial(one, constant);
  }
  void makeOne();
  void getPolynomialWithPolynomialCoefficient(
    Selection& theNonCoefficientVariables, Polynomial<Polynomial<Coefficient> >& output
  ) const;
  void getPolynomialUnivariateWithPolynomialCoefficients(
    int variableIndex,
    Polynomial<Polynomial<Coefficient> >& output
  ) const;
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
  void addConstant(const Coefficient& theConst) {
    MonomialPolynomial tempMon;
    tempMon.makeOne();
    this->addMonomial(tempMon, theConst);
  }
  void shiftVariableIndicesToTheRight(int VarIndexShift);
  void setNumberOfVariablesSubstituteDeletedByOne(int newNumVars);
  int getHighestIndexSuchThatHigherIndexVariablesDontParticipate();
  void scaleToPositiveMonomialExponents(MonomialPolynomial& outputScale);
  bool substitution(const List<Polynomial<Coefficient> >& substitution, const Coefficient& one);
  Rational totalDegree() const;
  int totalDegreeInt() const;
  bool isEqualToOne() const;
  bool isMonomialCoefficientOne() const;
  bool isOneLetterFirstDegree(int* whichLetter = nullptr) const;
  bool isConstant(Coefficient* whichConstant = nullptr) const;
  bool isNegative() const;
  bool isLinearNoConstantTerm();
  bool isLinear();
  bool isLinearGetRootConstantTermLastCoordinate(Vector<Coefficient>& outputRoot);
  void raiseToPower(int d, const Coefficient& one);
  bool getRootFromLinearPolynomialConstantTermLastVariable(Vector<Coefficient>& outputRoot);
  Matrix<Coefficient> evaluateUnivariatePolynomial(const Matrix<Coefficient>& input);//<-for univariate polynomials only
  Coefficient evaluate(const Vector<Coefficient>& input, const Coefficient& zero = 0);
  bool isProportionalTo(
    const Polynomial<Coefficient>& other, Coefficient& outputTimesMeEqualsOther, const Coefficient& ringUnit
  ) const;
  void drawElement(DrawElementInputOutput& theDrawData, FormatExpressions& PolyFormatLocal);

  // void ComponentInFrontOfVariableToPower(int VariableIndex, ListPointers<Polynomial<Coefficient> >& output, int UpToPower);
  int getMaximumPowerOfVariableIndex(int VariableIndex);
  bool operator<=(const Coefficient& other) const;
  bool operator<(const Coefficient& other) const;
  bool operator>(const Polynomial<Coefficient>& other) const;
  bool operator<=(const Polynomial<Coefficient>& other) const;
  static bool isGEQCompareByTopMonomialTotalDegThenLexicographic(
    const Polynomial<Coefficient>& left, const Polynomial<Coefficient>& right
  );
  static bool isGEQCompareByTopMonomialLexicographicLastVarStrongest(
    const Polynomial<Coefficient>& left, const Polynomial<Coefficient>& right
  );
  bool isEqualTo(const Polynomial<Coefficient>& p) const;
  Polynomial<Coefficient> operator+(const Polynomial<Coefficient>& other) const;
  Polynomial<Coefficient> operator-(const Polynomial<Coefficient>& other) const;
  void operator-=(int x);
  void operator-=(const Coefficient& other);
  void operator-=(const Polynomial<Coefficient>& other);
  void operator*=(const MonomialPolynomial& other);
  void operator*=(const Polynomial<Coefficient>& other);
  Polynomial<Coefficient> operator%(const Polynomial<Coefficient>& other);
  void operator/=(const Polynomial<Coefficient>& other);
  void operator/=(int other);
  void operator/=(const Coefficient& other);
  template <class otherType>
  void operator*=(const otherType& other);
  void operator=(const Polynomial<Coefficient>& other);
  template<class otherType>
  void operator=(const Polynomial<otherType>& other);
  void operator=(const Coefficient& other);
  void operator=(int other);
  template <class otherType>
  void assignOtherType(const Polynomial<otherType>& other);
  static bool greatestCommonDivisorOneVariable(
    const Polynomial<Coefficient>& left,
    const Polynomial<Coefficient>& right,
    Polynomial<Coefficient>& output,
    std::stringstream* commentsOnFailure
  );
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
    bool oneDifferential(const std::string& input, const std::string& expected);
    bool differential();
    Polynomial<Coefficient> fromString(const std::string& input);
    Vector<Polynomial<Coefficient> > fromStringCommonContext(const std::string& first, const std::string& second);
    Vector<Polynomial<Coefficient> > fromStringCommonContext(const List<std::string>& input);
    bool fromStringTest();
    bool fromStringCommonContextTest();
  };
};

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
  static bool sylvesterMatrixMulti(
    const List<Polynomial<Coefficient> >& polynomials,
    int dimension,
    Matrix<Coefficient>& output
  );
public:
  // For polynomials A = a_0 x^n + ..., B = b_0 x^m + ...,
  // we define the Sylvester matrix as the matrix:
  // ( a_0  0  0 ...  0  b_1  0  0 ... 0   )
  // ( a_1 a_0 0 ...  0  b_2 b_1 0 ... 0   )
  // (               ...               ... )
  // ( a_n           a_0               b_1 )
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
  // Sylvester matrix (depending on the, defintion
  // with reversed order of the polynomials).
  // The Sylvester product matrix is used when computing
  // Henself lifts of multiple polynomial products.
  static bool sylvesterMatrixProduct(
    const List<Polynomial<Coefficient> >& polynomials,
    Matrix<Coefficient>& output,
    std::stringstream* commentsOnFailure
  );
};

template <>
bool Polynomial<Rational>::Test::differential();
template <>
bool Polynomial<Rational>::Test::oneDifferential(const std::string& input, const std::string& expected);
template <>
bool Polynomial<Rational>::Test::oneLeastCommonMultiple(
  const std::string& left,
  const std::string& right,
  const std::string& expected
);
template <>
Vector<Polynomial<Rational> > Polynomial<Rational>::Test::fromStringCommonContext(
  const std::string& first, const std::string& second
);
template <>
Vector<Polynomial<Rational> > Polynomial<Rational>::Test::fromStringCommonContext(
  const List<std::string>& input
);
template <>
bool Polynomial<Rational>::Test::fromStringCommonContextTest();
template <>
void Polynomial<Rational>::Test::initialize();
template <>
bool Polynomial<Rational>::Test::leastCommonMultiple();
template <>
bool Polynomial<Rational>::Test::all();
template <>
Polynomial<Rational> Polynomial<Rational>::Test::fromString(const std::string& input);
template <>
bool Polynomial<Rational>::Test::fromStringTest();

template<>
bool Polynomial<Rational>::findOneVariableRationalRoots(List<Rational>& output);

template <class Coefficient>
class PolynomialModuloPolynomial {
public:
  Polynomial<Coefficient> modulus;
  Polynomial<Coefficient> value;
  void reduce();
  void operator*=(const PolynomialModuloPolynomial<Coefficient>& other);
  void operator+=(const PolynomialModuloPolynomial<Coefficient>& other);
  void operator-=(const PolynomialModuloPolynomial<Coefficient>& other);
  std::string toString(FormatExpressions* format = nullptr);
  PolynomialModuloPolynomial<Coefficient> one();
  static unsigned int hashFunction(const PolynomialModuloPolynomial<Coefficient>& input);
  unsigned int hashFunction() const;
  bool operator==(const PolynomialModuloPolynomial<Coefficient>& other)const;
  bool isEqualToZero() const;
};

template <class Coefficient, class OneFactorFinder>
class PolynomialFactorizationUnivariate {
public:
  Polynomial<Coefficient> original;
  Polynomial<Coefficient> current;
  Coefficient constantFactor;
  List<Polynomial<Coefficient> > reduced;
  List<Polynomial<Coefficient> > nonReduced;
  int maximumDegree;
  // An integer that measures the amount of
  // computations carried out by the OneFactorFinder algorithm.
  // The integer should be a reasonably good measure of computation
  // and need not be a precise measure of CPU cycles.
  // The details on what measure of computation is stored by
  // the OneFactorFinder here remain unspecified.
  int64_t computations;
  // Upper limit for the number of computations to carry out before giving up.
  // Set to non-posiive for no computation limits.
  int64_t maximumComputations;
  FormatExpressions format;
  bool basicChecks(
    std::stringstream* commentsOnFailure
  );
  bool factor(
    const Polynomial<Coefficient>& input,
    std::stringstream* comments,
    std::stringstream* commentsOnFailure
  );
  bool accountNonReducedFactor(Polynomial<Coefficient>& incoming);
  bool accountReducedFactor(
    Polynomial<Coefficient>& incoming, bool accountQuotientAsNonReduced
  );
  bool checkFactorization() const;
  std::string toStringResult(FormatExpressions* format) const;
  PolynomialFactorizationUnivariate();
};

class PolynomialFactorizationKronecker {
  public:
  static const int maximumDegreeDefault = 16;
  PolynomialFactorizationUnivariate<Rational, PolynomialFactorizationKronecker>* output;
  Polynomial<Rational> current;
  bool oneFactor(
    std::stringstream* comments,
    std::stringstream* commentsOnFailure
  );
  PolynomialFactorizationKronecker() {
    this->output = nullptr;
  }
  static bool solvePolynomial(
    const Polynomial<Rational>& input,
    List<AlgebraicNumber>& output,
    AlgebraicClosureRationals& closure,
    std::stringstream* commentsOnFailure
  );
  static std::string name() {
    return "Kronecker";
  }
};

template <class Coefficient>
class PolynomialSubstitution: public List<Polynomial<Coefficient> > {
  public:
  // This class is used for carrying out substitutions.
  // The substitution format is:
  // the i^th element denotes the image of x_i,
  // For example, if polynomials is the array
  // x_1+x_2 (poly in 3 variables)
  // x_1x_3+2 (poly in 3 variables)
  // this mens that it can be applied to polynomials in two variables
  // like this: x_1-> x_1+x_2
  // x_2-> (x_1x_3+2)
  // to produce a polynomial in three variables.
  void makeIdentitySubstitution(int numVars, const Coefficient& ringUnit = 1);
  void makeIdentityLikeInjectionSubstitution(
    int numStartingVars,
    int numTargetVarsMustBeLargerOrEqual,
    const Coefficient& ringUnit
  );
  // In the following function we have that:
  // the format of the linear substitution is:
  // theSub is a matrix whose number of rows minus 1 must equal the # number of
  // target variables and whose number of columns must equal the number of variables in
  // the current polynomial (this->NumVariables).
  // The first row denotes the constant term in the substitution of the respective variable.
  // An element in the x-th row and y-th column
  // is defined as element [x][y].
  void makeExponentSubstitution(Matrix<LargeInteger>& substitution);
  void addConstant(Coefficient& theConst);
  void multiplyByConstant(Coefficient& r);
  int getHighestIndexSuchThatHigherIndexVariablesDontParticipate() {
    int result = - 1;
    for (int i = 0; i < this->size; i ++) {
      result = MathRoutines::maximum(
        result, this->objects[i].getHighestIndexSuchThatHigherIndexVariablesDontParticipate()
      );
    }
    return result;
  }
  void makeZero(int numberOfVariables) {
    for (int i = 0; i < this->size; i ++) {
      this->objects[i].makeZero(numberOfVariables);
    }
  }
  std::string toString(int numDisplayedEltsMinus1ForAll = - 1) const {
    std::string tempS;
    this->toString(tempS, numDisplayedEltsMinus1ForAll);
    return tempS;
  }
  bool operator==(const PolynomialSubstitution& right);
  void makeSubstitutionFromMatrixIntegerAndDenominator(Matrix<LargeInteger>& theMat, LargeIntegerUnsigned& Den) {
    Matrix<Rational> tempMat;
    tempMat.assignMatrixIntegerWithDenominator(theMat, Den);
    this->makeLinearSubstitutionConstantTermsLastRow(tempMat);
  }
  void toString(std::string& output, int numDisplayedEltsMinus1ForAll) const {
    std::stringstream out;
    output.clear();
    FormatExpressions PolyFormatLocal;
    if (numDisplayedEltsMinus1ForAll == - 1) {
      numDisplayedEltsMinus1ForAll = this->size;
    }
    for (int i = 0; i < this->size; i ++) {
      out << "x_{" << i + 1 << "} \\mapsto "
      << this->objects[i].toString(&PolyFormatLocal);
      if (i != this->size - 1) {
        out << ", ";
      }
    }
    out << ": ";
    output = out.str();
  }
  void makeLinearSubstitutionConstantTermsLastRow(Matrix<Coefficient>& matrix);
};

template<class Coefficient>
class PolynomialDivisionReport {
public:
  HashedList<MonomialPolynomial> allMonomials;
  List<Polynomial<Coefficient> > intermediateRemainders;
  List<List<MonomialPolynomial> > intermediateHighlightedMons;
  List<MonomialPolynomial> intermediateHighestMonDivHighestMon;
  List<Coefficient> intermediateCoeffs;
  List<Polynomial<Coefficient> > intermediateSubtractands;
  Polynomial<Coefficient> startingPolynomial;
  int highlightAllMonsFinalRemainder;
  int firstIndexLatexSlide;
  List<List<int> > additionalHighlightRemainders;
  List<List<List<int> > > highlightMonsQuotients;
  List<List<List<int> > > highlightMonsRemainders;
  List<List<List<int> > > highlightMonsSubtracands;
  List<List<List<int> > > highlightMonsDivisors;
  List<List<int> > fcAnswerMonsQuotients;
  List<List<int> > fcAnswerMonsRemainders;
  List<List<int> > fcAnswerMonsSubtracands;
  List<List<int> > fcAnswerMonsDivisors;
  List<int> firstNonZeroIndicesPerIntermediateSubtracand;
  List<int> intermediateSelectedDivisors;
  List<int> uncoverAllMonsQuotients;
  List<int> uncoverAllMonsRemainders;
  List<int> uncoverAllMonsSubtracands;
  List<int> uncoverAllMonsDivisors;
  List<int> uncoverMonsFinalRemainder;
  List<int> additionalHighlightFinalRemainder;
  std::stringstream divisionLog;

  GroebnerBasisComputation<Coefficient>* owner;
  bool checkInitialization();
  std::string getDivisionStringHtml();
  std::string getDivisionStringLaTeX();
  std::string getDivisionLaTeXSlide();
  std::string getPolynomialStringSpacedMonomialsLaTeX(
    const Polynomial<Coefficient>& thePoly,
    std::string* highlightColor = nullptr,
    List<MonomialPolynomial>* theHighLightedMons = nullptr,
    int* firstNonZeroIndex = nullptr
  );
  std::string getSpacedMonomialsWithHighlightLaTeX(
    const Polynomial<Coefficient>& thePoly,
    List<List<int> >* slidesToHighlightMon,
    List<int>* slidesToFcAnswer,
    List<int>* slidesToUncover,
    List<int>* slidesAdditionalHighlight,
    int slidesToUncoverAllMons,
    bool useColumnSeparator
  );
  std::string getPolynomialStringSpacedMonomialsHtml(
    const Polynomial<Coefficient>& thePoly,
    const std::string& extraStyle,
    List<MonomialPolynomial>* theHighLightedMons = nullptr
  );
  void computeHighLightsFromRemainder(int remainderIndex, int& currentSlideNumber);
  PolynomialDivisionReport();
};

template<class Coefficient>
class GroebnerBasisComputation {
  public:
  PolynomialOrder<Coefficient> polynomialOrder;
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
  bool transformToReducedBasis(
    List<Polynomial<Coefficient> >& inputOutput
  );
  bool transformToReducedGroebnerBasis(List<Polynomial<Coefficient> >& inputOutpuT);
  void generateSymmetricDifferenceCandidates();
  void generateOneSymmetricDifferenceCandidate(
    GroebnerBasisComputation<Coefficient>::BasisElement& left,
    GroebnerBasisComputation<Coefficient>::BasisElement& right
  );
  void getBasis(List<Polynomial<Coefficient> >& output);
  bool wrapUpGroebnerOnExceedingComputationLimit(List<Polynomial<Coefficient> >& output);
  bool wrapUpOnGroebnerBasisSuccess(List<Polynomial<Coefficient> >& output);
  GroebnerBasisComputation();
  int minimalNumberOfVariables() const;
  std::string toStringLetterOrder(bool addDollars) const;
  std::string toStringPolynomialBasisStatus();
  static int getNumberOfEquationsThatWouldBeLinearIfISubstitutedVariable(int variableIndex, List<Polynomial<Coefficient> >& input);
  void remainderDivisionByBasis(
    const Polynomial<Coefficient>& input,
    Polynomial<Coefficient>& outputRemainder,
    int basisIndexToIgnore = - 1
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
    ProgressReport* theReport
  );
  std::string toStringDivision(Polynomial<Coefficient>& toBeDivided);
  std::string toStringStatusGroebnerBasisTransformation();
  void checkConsistency();
  void initializeForGroebnerComputation();
};

template<class Coefficient>
class PolynomialSystem {
public:
  int numberOfSerreSystemComputations;
  int numberOfSerreVariablesOneGenerator;
  int recursionCounterSerreLikeSystem;
  int numberOfVariablesToSolveForStart;
  int numberOfVariablesToSolveForAfterReduction;
  bool flagTryDirectlySolutionOverAlgebraicClosure;
  bool flagUseTheMonomialBranchingOptimization;
  bool flagSystemProvenToHaveNoSolution;
  bool flagSystemProvenToHaveSolution;
  bool flagSystemSolvedOverBaseField;
  bool flagUsingAlgebraicClosure;

  AlgebraicClosureRationals* algebraicClosure;
  GroebnerBasisComputation<Coefficient> groebner;
  MemorySaving<PolynomialSystem<Coefficient> > computationUsedInRecursiveCalls;
  List<Coefficient> systemSolution;
  Selection solutionsFound;
  List<PolynomialSubstitution<Coefficient> > impliedSubstitutions;
  PolynomialSystem();
  void solveSerreLikeSystem(List<Polynomial<Coefficient> >& inputSystem);
  std::string toStringCalculatorInputFromSystem(const List<Polynomial<Coefficient> >& inputSystem);
  bool hasImpliedSubstitutions(
    List<Polynomial<Coefficient> >& inputSystem,
    PolynomialSubstitution<Coefficient>& outputSub
  );
  bool hasSingleMonomialEquation(const List<Polynomial<Coefficient> >& inputSystem, MonomialPolynomial& outputMon);
  void setUpRecursiveComputation(PolynomialSystem<Coefficient>& toBeModified);
  void processSolvedSubcaseIfSolvedOrProvenToHaveSolution(PolynomialSystem<Coefficient>& potentiallySolvedCase);
  void solveWhenSystemHasSingleMonomial(List<Polynomial<Coefficient> >& inputSystem, const MonomialPolynomial& theMon);
  int getPreferredSerreSystemSubstitutionIndex(List<Polynomial<Coefficient> >& inputSystem);
  void solveSerreLikeSystemRecursively(List<Polynomial<Coefficient> >& inputSystem);
  void polynomialSystemSolutionSimplificationPhase(List<Polynomial<Coefficient> >& inputSystem);
  void backSubstituteIntoPolynomialSystem(List<PolynomialSubstitution<Coefficient> >& impliedSubstitutions);
  void backSubstituteIntoSinglePolynomial(
    Polynomial<Coefficient>& substitution, int index, PolynomialSubstitution<Coefficient>& finalSubstitution
  );
  bool getOneVariablePolynomialSolution(const Polynomial<Coefficient>& thePoly, Coefficient& outputSolution);
  void setSerreLikeSolutionIndex(int theIndex, const Coefficient& theConst);
  void getSubstitutionFromPartialSolutionSerreLikeSystem(PolynomialSubstitution<Coefficient>& outputSub);
  std::string toStringSerreLikeSolution();
  static int getNumberOfVariablesToSolveFor(const List<Polynomial<Coefficient> >& input);
  static void getVariablesToSolveFor(const List<Polynomial<Coefficient> >& input, Selection& output);
  void trySettingValueToVariable(
    List<Polynomial<Coefficient> >& inputSystem, const Rational& aValueToTryOnPreferredVariable
  );
  static bool isContradictoryReducedSystem(const List<Polynomial<Coefficient> >& input);
  std::string toStringImpliedSubstitutions();
  void initializeForSystemSolution();
  bool shouldReport();
  FormatExpressions& format();
};


template <class TemplateMonomial, class Coefficient>
bool LinearCombination<TemplateMonomial, Coefficient>::operator==(int x) const {
  if (x == 0) {
    return this->size() == 0;
  }
  global.fatal << "This is either a programming error, or an unforeseen use of operator==. "
  << "If the second is the case, an audit/careful "
  << "proofreading of the code calling this function is needed; "
  << "I am crashing just in case. " << global.fatal;
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
bool LinearCombination<TemplateMonomial, Coefficient>::isInteger(LargeInteger* whichInteger) const {
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
bool LinearCombination<TemplateMonomial, Coefficient>::isSmallInteger(int* whichInteger) const {
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
void LinearCombination<TemplateMonomial, Coefficient>::getVectorMonomialsAscending(Vector<Coefficient>& result) {
  this->quickSortAscending();
  result = this->coefficients;
}

template <class TemplateMonomial, class Coefficient>
void LinearCombination<TemplateMonomial, Coefficient>::getVectorMonomialsDescending(Vector<Coefficient>& result) {
  this->quickSortDescending();
  result = this->coefficients;
}

template <class Coefficient>
void Polynomial<Coefficient>::makeLinearWithConstantTerm(
  const Vector<Rational>& inputLastCoordinateConstantTerm
) {
  this->makeZero();
  MonomialPolynomial tempM;
  for (int i = 0; i < inputLastCoordinateConstantTerm.size - 1; i ++) {
    tempM.makeEi(i);
    this->addMonomial(tempM, inputLastCoordinateConstantTerm[i]);
  }
  this->operator+=(*inputLastCoordinateConstantTerm.lastObject());
}

template <class Coefficient>
void Polynomial<Coefficient>::makeLinearNoConstant(
  const Vector<Rational>& inputCoefficients
) {
  this->makeZero();
  MonomialPolynomial tempM;
  for (int i = 0; i < inputCoefficients.size; i ++) {
    tempM.makeEi(i);
    if (!inputCoefficients[i].isInteger()) {
      global.fatal << "This may or may not be a programming error: "
      << "requesting a monomial with non-integer exponent. "
      << "It appears this operation should be allowed, however "
      << "this requires further consideration. I am crashing just in case. "
      << global.fatal;
    }
    this->addMonomial(tempM, inputCoefficients[i].getNumerator());
  }
}

template <class TemplateMonomial, class Coefficient>
void LinearCombination<TemplateMonomial, Coefficient>::subtractOtherTimesCoefficient(
  const LinearCombination<TemplateMonomial, Coefficient>& other, Coefficient* inputCoefficient
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
  LinearCombination<TemplateMonomial, Coefficient>& other, const Coefficient& theConst
) {
  this->setExpectedSize(other.size() + this->size());
  Coefficient product;
  for (int i = 0; i < other.size(); i ++) {
    product = theConst;
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
bool LinearCombination<TemplateMonomial, Coefficient>::hasGreaterThanOrEqualToMonomial(
  TemplateMonomial& m, int& whichIndex
) {
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
bool LinearCombination<TemplateMonomial, Coefficient>::linearSpanContainsGetFirstLinearCombination(
  const List<LinearCombinationTemplate>& theList,
  const LinearCombinationTemplate& input,
  Vector<Coefficient>& outputFirstLinearCombination,
  HashedList<TemplateMonomial>* seedMonomials
) {
  List<LinearCombinationTemplate> listCopy = theList;
  Matrix<Coefficient> theRowOperations;
  theRowOperations.makeIdentityMatrix(theList.size);
  LinearCombination<TemplateMonomial, Coefficient>::gaussianEliminationByRows(
    listCopy, 0, seedMonomials, &theRowOperations
  );
  LinearCombinationTemplate remainderFromInput = input;
  TemplateMonomial currentMon;
  Coefficient coefficientMinimialMonomial, coefficientInRemainder;
  outputFirstLinearCombination.makeZero(listCopy.size);
  for (int i = 0; i < listCopy.size; i ++) {
    if (listCopy[i].isEqualToZero()) {
      break;
    }
    listCopy[i].getMinimalMonomial(currentMon, coefficientMinimialMonomial);
    coefficientInRemainder = remainderFromInput.getCoefficientOf(currentMon);
    outputFirstLinearCombination[i] = coefficientInRemainder;
    outputFirstLinearCombination[i] /= coefficientMinimialMonomial;
    remainderFromInput -= listCopy[i] * outputFirstLinearCombination[i];
  }
  if (!remainderFromInput.isEqualToZero()) {
    return false;
  }
  theRowOperations.actMultiplyVectorRowOnTheRight(outputFirstLinearCombination);
  return true;
}

template <class TemplateMonomial, class Coefficient>
template <class LinearCombinationTemplate>
void LinearCombination<TemplateMonomial, Coefficient>::gaussianEliminationByRows(
  List<LinearCombinationTemplate>& toBeEliminated,
  bool *madeARowSwitch,
  HashedList<TemplateMonomial>* seedMonomials,
  Matrix<Coefficient>* carbonCopyMatrix,
  List<LinearCombinationTemplate>* carbonCopyList
) {
  MacroRegisterFunctionWithName("LinearCombination::gaussianEliminationByRows");
  if (carbonCopyMatrix != 0) {
    if (carbonCopyMatrix->numberOfRows != toBeEliminated.size) {
      global.fatal
      << "Carbon copy matrix has "
      << carbonCopyMatrix->numberOfRows
      << " rows, while the gaussian-eliminated list has " << toBeEliminated.size
      << " elements; the two numbers must be the same!" << global.fatal;
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
  MemorySaving<HashedList<TemplateMonomial> > bufferMons;
  HashedList<TemplateMonomial>& allMons = seedMonomials == 0 ? bufferMons.getElement() : *seedMonomials;
  if (seedMonomials == 0) {
    int topBoundNumMons = 0;
    for (int i = 0; i < toBeEliminated.size; i ++) {
      topBoundNumMons += toBeEliminated[i].size();
    }
    allMons.setExpectedSize(topBoundNumMons);
  }
  for (int i = 0; i < toBeEliminated.size; i ++) {
    allMons.addOnTopNoRepetition(toBeEliminated[i].monomials);
  }
  allMons.quickSortAscending();
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML = true;
  tempFormat.flagUseLatex = true;
  if (madeARowSwitch != nullptr) {
    *madeARowSwitch = false;
  }
  int currentRowIndex = 0;
  Coefficient accumulator, negated;
  for (int i = 0; i < allMons.size && currentRowIndex < toBeEliminated.size; i ++) {
    const TemplateMonomial& currentMon = allMons[i];
    int goodRow = currentRowIndex;
    for (; goodRow < toBeEliminated.size; goodRow ++) {
      if (toBeEliminated[goodRow].monomials.contains(currentMon)) {
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
    LinearCombination<TemplateMonomial, Coefficient>& currentPivot = toBeEliminated[currentRowIndex];
    int columnIndex = currentPivot.monomials.getIndex(currentMon);
    if (columnIndex == - 1) {
      global.fatal << "An internal check at the "
      << "Gaussian elimination method for monomial collections fails. "
      << "Something is wrong. Here is the List you wanted to perform Gaussian elimination upon. "
      << toBeEliminated.toString() << ". " << global.fatal;
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
      LinearCombination<TemplateMonomial, Coefficient>& currentOther = toBeEliminated[j];
      int otherColumnIndex = currentOther.monomials.getIndex(currentMon);
      if (otherColumnIndex != - 1) {
        accumulator = currentOther.coefficients[otherColumnIndex];
        currentOther.subtractOtherTimesCoefficient(currentPivot, &accumulator);
        if (carbonCopyList != 0) {
          (*carbonCopyList)[j].subtractOtherTimesCoefficient((*carbonCopyList)[currentRowIndex], &accumulator);
        }
        if (carbonCopyMatrix != 0) {
          negated = accumulator;
          negated *= - 1;
          carbonCopyMatrix->addTwoRows(currentRowIndex, j, 0, negated);
        }
      }
    }
    currentRowIndex ++;
  }
}

template <class TemplateMonomial, class Coefficient>
int LinearCombination<TemplateMonomial, Coefficient>::addMonomialNoCoefficientCleanUpReturnIndex(
  const TemplateMonomial& inputMonomial, const Coefficient& inputCoefficient
) {
  this->checkConsistency();
  if (inputCoefficient == 0) {
    return - 1;
  }
  if (this->flagDeallocated) {
    global.fatal << "Use after free or race condition on monomial collection. " << global.fatal;
  }
  bool oldFlagDeallocated = this->flagDeallocated;
  this->flagDeallocated = true;
  int j = this->monomials.getIndex(inputMonomial);
  if (j >= this->size()) {
    global.fatal << "Error: function getIndex evaluated on "
    << inputMonomial << " with hash function "
    << inputMonomial.hashFunction(inputMonomial)
    << " returns index " << j << " but I have only "
    << this->size() << " elements. " << global.fatal;
  }
  if (j == - 1) {
    j = this->size();
    this->monomials.addOnTop(inputMonomial);
    this->coefficients.addOnTop(inputCoefficient);
  } else {
    if (j >= this->coefficients.size) {
      global.fatal
      << "Looking for coefficient index " << j
      << " when number of coefficients is "
      << this->coefficients.size <<  ". " << global.fatal;
    }
    this->coefficients[j] += inputCoefficient;
  }
  this->flagDeallocated = oldFlagDeallocated;
  return j;
}

template <class TemplateMonomial, class Coefficient>
int LinearCombination<TemplateMonomial, Coefficient>::subtractMonomialNoCoefficientCleanUpReturnIndex(
  const TemplateMonomial& inputMon, const Coefficient& inputCoeff
) {
  if (inputCoeff.isEqualToZero()) {
    return - 1;
  }
  int j = this->monomials.getIndex(inputMon);
  if (j == - 1) {
    this->monomials.addOnTop(inputMon);
    this->coefficients.addOnTop(inputCoeff);
    *this->coefficients.lastObject() *= - 1;
    j = this->coefficients.size - 1;
  } else {
    this->coefficients[j] -= inputCoeff;
  }
  return j;
}

template <class TemplateMonomial, class Coefficient>
void ElementMonomialAlgebra<TemplateMonomial, Coefficient>::raiseToPower(
  int d, ElementMonomialAlgebra<TemplateMonomial, Coefficient>& output, const Coefficient& ringUnit
) {
  ElementMonomialAlgebra<TemplateMonomial, Coefficient> tempOne;
  tempOne.makeConstant(ringUnit);
  output = *this;
  MathRoutines::raiseToPower(output, d, tempOne);
}

template <class Coefficient>
void PolynomialSubstitution<Coefficient>::makeIdentitySubstitution(int numVars, const Coefficient& ringUnit) {
  this->makeIdentityLikeInjectionSubstitution(numVars, numVars, ringUnit);
}

template <class Coefficient>
void PolynomialSubstitution<Coefficient>::makeIdentityLikeInjectionSubstitution(
  int numStartingVars, int numTargetVarsMustBeLargerOrEqual, const Coefficient& ringUnit
) {
  if (numStartingVars > numTargetVarsMustBeLargerOrEqual) {
    global.fatal << "Bad number of variables. " << global.fatal;
  }
  this->setSize(numStartingVars);
  for (int i = 0; i < this->size; i ++) {
    Polynomial<Coefficient>& currentPoly = this->objects[i];
    currentPoly.makeDegreeOne(numTargetVarsMustBeLargerOrEqual, i, ringUnit);
  }
}

template <class Coefficient>
void PolynomialSubstitution<Coefficient>::makeExponentSubstitution(Matrix<LargeInteger>& substitution) {
  Polynomial<Coefficient> tempP;
  MonomialPolynomial tempM;
  tempM.makeOne();
  this->size = 0;
  this->setSize(substitution.numberOfColumns);
  for (int i = 0; i < substitution.numberOfColumns; i ++) {
    for (int j = 0; j < substitution.numberOfRows; j ++) {
      tempM.setVariable(j, substitution(j, i));
    }
    tempP.makeZero();
    tempP.addMonomial(tempM, 1);
    this->objects[i] = tempP;
  }
}

class PartialFractions;

class OnePartialFractionDenominator {
public:
  List<int> multiplicities;
  List<int> Elongations;
  void addMultiplicity(int MultiplicityIncrement, int Elongation);
  int indexLargestElongation();
  int getLargestElongation();
  void GetPolynomialDenominator(Polynomial<LargeInteger>& output, int MultiplicityIndex, Vector<Rational>& theExponent);
  int getMultiplicityLargestElongation();
  int getLeastCommonMultipleElongations();
  int getTotalMultiplicity() const;
  void invert();
  void initialize();
  static Vector<Rational> GetCheckSumRoot(int NumVars);
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const OnePartialFractionDenominator& input) {
    return input.hashFunction();
  }
  void computeOneCheckSum(Rational& output, const Vector<Rational>& theExp, int theDimension);
  void operator=(OnePartialFractionDenominator& right);
  bool operator==(OnePartialFractionDenominator& right);
  std::string toString();
  void oneFractionToStringBasisChange(
    PartialFractions& owner,
    int indexElongation,
    Matrix<LargeInteger>& VarChange,
    bool UsingVarChange,
    std::string& output,
    bool LatexFormat,
    int indexInFraction,
    int theDimension,
    FormatExpressions& PolyFormatLocal
  );
};

class LaTeXProcedures {
public:
  static const int ScaleFactor = 40;
  static const int FigureSizeX = 10; //in centimeters
  static const int FigureSizeY = 10; //in centimeters
  static const int FigureCenterCoordSystemX = 4; //in centimeters
  static const int FigureCenterCoordSystemY = 8; //in centimeters
  static const int TextPrintCenteringAdjustmentX = 3;
  static const int TextPrintCenteringAdjustmentY = 3;
  static void drawline(
    double X1,
    double Y1,
    double X2,
    double Y2,
    uint32_t thePenStyle,
    int ColorIndex,
    std::fstream& output,
    DrawingVariables& drawInput
  );
  static void drawTextDirectly(double X1, double Y1, const std::string& theText, int ColorIndex, std::fstream& output);
  static void beginDocument(std::fstream& output);
  static void endLatexDocument(std::fstream& output);
  static void beginPSTricks(std::fstream& output);
  static void endPSTricks(std::fstream& output);
  static void getStringFromColorIndex(int ColorIndex, std::string &output, DrawingVariables& drawInput);
};

class Permutation: public SelectionWithDifferentMaxMultiplicities {
public:
  void initPermutation(int n);
  void initPermutation(List<int>& disjointSubsets, int TotalNumElements);
  void incrementAndGetPermutation(List<int>& output);
  void getPermutationLthElementIsTheImageofLthIndex(List<int>& output);
  int getNumberOfPermutations() {
    return this->totalNumberSubsetsSmallInt();
  }
};

template<typename Coefficient>
void Matrix<Coefficient>::nonPivotPointsToEigenVectorMatrixForm(
  Selection& nonPivotPoints, Matrix<Coefficient>& output
) {
  int RowCounter = 0;
  output.initialize(this->numberOfColumns, 1);
  for (int i = 0; i < this->numberOfColumns; i ++) {
    if (!nonPivotPoints.selected[i]) {
      output(i, 0) = 0;
      for (int j = 0; j < nonPivotPoints.cardinalitySelection; j ++) {
        output(i, 0) -= this->elements[RowCounter][nonPivotPoints.elements[j]];
      }
      RowCounter ++;
    } else {
      output(i, 0) = 1;
    }
  }
}

template<typename Coefficient>
void Matrix<Coefficient>::nonPivotPointsToEigenVector(
  Selection& nonPivotPoints,
  Vector<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  int RowCounter = 0;
  output.setSize(this->numberOfColumns);
  for (int i = 0; i < this->numberOfColumns; i ++) {
    if (!nonPivotPoints.selected[i]) {
      output[i] = ringZero;
      for (int j = 0; j < nonPivotPoints.cardinalitySelection; j ++) {
        output[i] -= this->elements[RowCounter][nonPivotPoints.elements[j]];
      }
      RowCounter ++;
    } else {
      output[i] = ringUnit;
    }
  }
}

template<class Coefficient>
class Complex {
  static bool flagEqualityIsApproximate;
  static double equalityPrecision;
  public:
  Coefficient imaginaryPart;
  Coefficient realPart;
  std::string toString(FormatExpressions* unused = nullptr) const {
    (void) unused;
    std::stringstream tempStream;
    tempStream << *this;
    return tempStream.str();
  }
  friend std::iostream& operator<<(std::iostream& output, const Complex<Coefficient>& input) {
    output << input.realPart << "+ i" << input.imaginaryPart;
    return output;
  }
  void operator*=(const Complex<Coefficient>& other) {
    Complex Accum;
    Accum.realPart = this->realPart * other.realPart - this->imaginaryPart * other.imaginaryPart;
    Accum.imaginaryPart = this->realPart * other.imaginaryPart + this->imaginaryPart * other.realPart;
    this->operator=(Accum);
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
    numerator = this->realPart * this->realPart + this->imaginaryPart * this->imaginaryPart;
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

template < > bool Complex<double>::flagEqualityIsApproximate;
template < > double Complex<double>::equalityPrecision;

template <class Coefficient>
bool Complex<Coefficient>::isEqualToZero() const {
  if (!Complex<Coefficient>::flagEqualityIsApproximate) {
    return this->imaginaryPart == 0 && this->realPart == 0;
  } else {
    return
    this->imaginaryPart<Complex<Coefficient>::equalityPrecision && - this->imaginaryPart<Complex<Coefficient>::equalityPrecision &&
    this->realPart<Complex<Coefficient>::equalityPrecision && - this->realPart<Complex<Coefficient>::equalityPrecision;
  }
}

template<class Coefficient>
class ElementSemisimpleLieAlgebra : public LinearCombination<ChevalleyGenerator, Coefficient> {
public:
  bool checkConsistency() const {
    if (this->size() == 0) {
      return true;
    }
    SemisimpleLieAlgebra* owner = (*this)[0].owner;
    for (int i = 1; i < this->size(); i ++) {
      if (owner != (*this)[i].owner) {
        global.fatal << "ElementSemisimpleLieAlgebra "
        << "contains Chevalley generators with different owners. " << global.fatal;
      }
    }
    return true;
  }
  bool needsParenthesisForMultiplication() const;
  Vector<Coefficient> getCartanPart() const;
  void makeGGenerator(const Vector<Rational>& root, SemisimpleLieAlgebra& inputOwner);
  bool isElementCartan() const;
  void makeCartanGenerator(const Vector<Coefficient>& elementH, SemisimpleLieAlgebra& inputOwners);
  void makeGenerator(int generatorIndex, SemisimpleLieAlgebra& inputOwner);
  void elementToVectorNegativeRootSpacesFirst(Vector<Coefficient>& output) const;
  void assignVectorNegRootSpacesCartanPosRootSpaces(
    const Vector<Coefficient>& input,
    SemisimpleLieAlgebra& owner
  );
  bool getCoordinatesInBasis(
    const List<ElementSemisimpleLieAlgebra<Coefficient> >& basis,
    Vector<Coefficient>& output
  ) const;
  SemisimpleLieAlgebra* getOwner() const {
    this->checkConsistency();
    if (this->size() == 0) {
      return nullptr;
    }
    return (*this)[0].owner;
  }
  bool getCoordinatesInBasis(
    const List<ElementSemisimpleLieAlgebra>& basis,
    Vector<RationalFraction<Rational> >& output
  ) const {
    Vector<Rational> tempVect;
    if (!this->getCoordinatesInBasis(basis, tempVect)) {
      return false;
    }
    output.setSize(tempVect.size);
    return true;
  }
  static void getBasisFromSpanOfElements(
    List<ElementSemisimpleLieAlgebra>& elements,
    List<ElementSemisimpleLieAlgebra>& outputBasis
  );
  void actOnMe(
    const ElementSemisimpleLieAlgebra& element,
    ElementSemisimpleLieAlgebra& output,
    SemisimpleLieAlgebra& owner
  );
  void actOnMe(
    const ElementSemisimpleLieAlgebra& element,
    ElementSemisimpleLieAlgebra& output,
    SemisimpleLieAlgebra& owner,
    const RationalFraction<Rational>& ringUnit,
    const RationalFraction<Rational>& ringZero
  );
  bool isCoefficientOneChevalleyGenerator();
  bool isProportionalTo(const ElementSemisimpleLieAlgebra& other) const {
    Vector<Rational> left, right;
    this->elementToVectorNegativeRootSpacesFirst(left);
    other.elementToVectorNegativeRootSpacesFirst(right);
    return left.isProportionalTo(right);
  }
  bool isProportionalTo(const ElementSemisimpleLieAlgebra& other, Rational& outputTimesMeEqualsInput) const {
    Vector<Rational> tempRoot1, tempRoot2;
    this->elementToVectorNegativeRootSpacesFirst(tempRoot1);
    other.elementToVectorNegativeRootSpacesFirst(tempRoot2);
    return tempRoot1.isProportionalTo(tempRoot2, outputTimesMeEqualsInput);
  }
  unsigned int hashFunction() const {
    return this->indexOfOwnerAlgebra * someRandomPrimes[0] +
    this->::LinearCombination<ChevalleyGenerator, Rational>::hashFunction() * someRandomPrimes[1];
  }
  static unsigned int hashFunction(const ElementSemisimpleLieAlgebra& input) {
    return input.hashFunction();
  }
  template<class otherElement>
  void operator=(const otherElement& other) {
    this->::LinearCombination<ChevalleyGenerator, Coefficient>::operator=(other);
  }
  Vector<Rational> getRootIMustBeWeight() const;
};

template <class Coefficient>
int Matrix<Coefficient>::findPositiveLCMCoefficientDenominatorsTruncated() {
  int result = 1;
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      result *= this->elements[i][j].denominatorShort / Rational::gcdSigned(result, this->elements[i][j].denominatorShort);
    }
  }
  return result;
}

template <class Coefficient>
LargeIntegerUnsigned Matrix<Coefficient>::findPositiveLCMCoefficientDenominators() {
  LargeIntegerUnsigned result = 1;
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      result = LargeIntegerUnsigned::leastCommonMultiple(result, (*this)(i, j).getDenominator());
    }
  }
  return result;
}

template <class Coefficient>
void Matrix<Coefficient>::getMatrixIntegerWithDenominator(Matrix<LargeInteger>& outputMat, LargeIntegerUnsigned& outputDen) {
  outputDen = this->findPositiveLCMCoefficientDenominators();
  outputMat.initialize(this->numberOfRows, this->numberOfColumns);
  Rational tempRat;
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      tempRat = this->elements[i][j] * outputDen;
      outputMat(i, j) = tempRat.getDenominator();
    }
  }
}

template <class Coefficient>
int Matrix<Coefficient>::findPositiveGCDCoefficientNumeratorsTruncated() {
  int result = 1;
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      if (this->elements[i][j].numeratorShort != 0) {
        result = Rational::gcdSigned(result, this->elements[i][j].numeratorShort);
      }
    }
  }
  if (result == 0) {
    global.fatal << "Crash in Matrix::findPositiveGCDCoefficientNumeratorsTruncated. " << global.fatal;
  }
  if (result < 0) {
    result = - result;
  }
  return result;
}

template <class Coefficient>
void Matrix<Coefficient>::scaleToIntegralForMinimalRationalHeightNoSignChange() {
  Rational tempRat;
  tempRat.assignNumeratorAndDenominator(
    this->findPositiveLCMCoefficientDenominatorsTruncated(),
    this->findPositiveGCDCoefficientNumeratorsTruncated()
  );
  *this *= tempRat;
}

template <class Coefficient>
Coefficient Matrix<Coefficient> ::getDeterminant() {
  Matrix<Coefficient> tempMat = *this;
  Coefficient result;
  tempMat.computeDeterminantOverwriteMatrix(result);
  return result;
}

template <class Coefficient>
Coefficient Matrix<Coefficient>::getTrace() const {
  if (this->numberOfColumns != this->numberOfRows) {
    global.fatal << "This is either programming error, a mathematical error, "
    << "or requires a more general definition of trace. Requesting the trace of "
    << "a non-square matrix of " << this->numberOfRows << " rows and "
    << this->numberOfColumns << " columns is not allowed. "
    << global.fatal;
  }
  Coefficient sum = 0;
  for (int i = 0; i < this->numberOfColumns; i ++) {
    sum += this->elements[i][i];
  }
  return sum;
}

template <class Coefficient>
Matrix<Coefficient> Matrix<Coefficient>::operator*(const Matrix<Coefficient>& right) const {
  Matrix<Coefficient> matrix;
  matrix = right;
  matrix.multiplyOnTheLeft(*this);
  return matrix;
}

template <class Coefficient>
Vector<Coefficient> Matrix<Coefficient>::operator*(const Vector<Coefficient>& v) const {
  if (v.size != this->numberOfColumns) {
    global.fatal
    << "Matrix application mismatch: matrix with "
    << this->numberOfColumns
    << " columns attempted to multiply vector of length "
    << v.size << ". " << global.fatal;
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
  Vector<Coefficient>& direction, Vector<Coefficient>& normal, Coefficient& Correction
) {
  Rational tempRat2 = Vector<Coefficient>::scalarEuclidean(direction, normal);
  this->makeZero();
  MonomialPolynomial tempM;
  for (int i = 0; i < direction.size; i ++) {
    tempM.makeEi(i);
    this->addMonomial(tempM, normal.objects[i] / tempRat2);
  }
  *this += Correction;
}

template <class Coefficient>
bool Vectors<Coefficient>::getNormalSeparatingCones(
  List<Vector<Coefficient> >& coneStrictlyPositiveCoeffs,
  List<Vector<Coefficient> >& coneNonNegativeCoeffs,
  Vector<Coefficient>& outputNormal
) {
  Matrix<Rational> matA;
  Matrix<Rational> matb;
  Vector<Rational> matX;
  int theDimension = coneStrictlyPositiveCoeffs[0].size;
  if (coneStrictlyPositiveCoeffs.size == 0) {
    if (coneNonNegativeCoeffs.size > 0) {
      outputNormal.makeZero(coneNonNegativeCoeffs[0].size);
    }
    return true;
  }
  int numRows = coneStrictlyPositiveCoeffs.size + coneNonNegativeCoeffs.size;
  matA.initialize(numRows, theDimension * 2 + numRows);
  matA.makeZero();
  matb.initialize(numRows, 1);
  matb.makeZero();
  for (int i = 0; i < coneStrictlyPositiveCoeffs.size; i ++) {
    for (int k = 0; k < theDimension; k ++) {
      matA.elements[i][k].assign(coneStrictlyPositiveCoeffs.objects[i].objects[k]);
      matA.elements[i][k + theDimension].assign(matA.elements[i][k]);
      matA.elements[i][k + theDimension].negate();
    }
    matb.elements[i][0].makeOne();
    matA.elements[i][theDimension * 2 + i].makeMinusOne();
  }
  for (int i = 0; i < coneNonNegativeCoeffs.size; i ++) {
    int currentRow = i + coneStrictlyPositiveCoeffs.size;
    for (int k = 0; k < theDimension; k ++) {
      matA.elements[currentRow][k].assign(coneNonNegativeCoeffs.objects[i].objects[k]);
      matA.elements[currentRow][k + theDimension].assign(matA.elements[currentRow][k]);
      matA.elements[currentRow][k + theDimension].negate();
    }
    matA.elements[currentRow][2 * theDimension + currentRow].makeOne();
  }
  bool result = Matrix<Rational>::systemLinearEqualitiesWithPositiveColumnVectorHasNonNegativeNonZeroSolution(
    matA, matb, &matX
  );
  outputNormal.makeZero(theDimension);
  for (int i = 0; i < theDimension; i ++) {
    outputNormal[i] = matX[i] - matX[i + theDimension];
  }
  if (result) {
    Rational tempRat;
    for (int i = 0; i < coneStrictlyPositiveCoeffs.size; i ++) {
      coneStrictlyPositiveCoeffs[i].scalarEuclidean(outputNormal, tempRat);
      if (!tempRat.isPositive()) {
        global.fatal << "Unexpected non-positive value. " << global.fatal;
      }
    }
    for (int i = 0; i < coneNonNegativeCoeffs.size; i ++) {
      coneNonNegativeCoeffs[i].scalarEuclidean(outputNormal, tempRat);
      if (!tempRat.isNonPositive()) {
        global.fatal << "Unexpected positive value. " << global.fatal;
      }
    }
  }
  return result;
}

template<class Coefficient>
void Matrix<Coefficient>::computePotentialChangeGradient(
  Matrix<Coefficient>& matA,
  Selection& baseVariables,
  int numberOfTrueVariables,
  int columnIndex,
  Rational& outputChangeGradient,
  bool& hasAPotentialLeavingVariable
) {
  hasAPotentialLeavingVariable = false;
  outputChangeGradient.makeZero();
  for (int j = 0; j < matA.numberOfRows; j ++) {
    if (baseVariables.elements[j] >= numberOfTrueVariables) {
      outputChangeGradient += matA.elements[j][columnIndex];
    }
    hasAPotentialLeavingVariable = hasAPotentialLeavingVariable || matA.elements[j][columnIndex].isPositive();
  }
  if (columnIndex >= numberOfTrueVariables) {
    outputChangeGradient -= 1;
  }
}

template<class Coefficient>
void Matrix<Coefficient>::getMaxMovementAndLeavingVariableRow(
  Rational& maxMovement,
  int& leavingVariableRow,
  int enteringVariable,
  Matrix<Coefficient>& tempMatA,
  Vector<Coefficient>& inputVectorX,
  Selection& BaseVariables
) {
  leavingVariableRow = - 1;
  maxMovement.makeZero();
  for (int i = 0; i < tempMatA.numberOfRows; i ++) {
    Rational tempRat;
    tempRat.assign(tempMatA.elements[i][enteringVariable]);
    if (tempRat.isPositive()) {
      tempRat.invert();
      tempRat.multiplyBy(inputVectorX[BaseVariables.elements[i]]);
      if (maxMovement.isGreaterThan(tempRat) || (leavingVariableRow == - 1)) {
        maxMovement.assign(tempRat);
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
  MonomialPolynomial tempMon;
  output.makeZero();
  Rational coeff;
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      tempMon = input;
      coeff = tempMon(j);
      coeff *= this->elements[i][j];
      tempMon.multiplyByVariable(j, - 1);
      tempMon.multiplyByVariable(i, 1);
      output.addMonomial(tempMon, coeff);
    }
  }
}

template <typename Coefficient>
void Matrix<Coefficient>::getZeroEigenSpaceModifyMe(List<Vector<Coefficient> >& output) {
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
  std::string tempS;
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
    tempS = this->objects[i].toString(format);
    if (useHtml && makeTable) {
      out << "<tr><td>";
    }
    out << tempS;
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
void List<Object>::subSelection(const Selection& selection, List<Object>& output) {
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
void List<Object>::intersectWith(const List<Object>& other, List<Object>& output) const {
  if (&output == &other || this == &output) {
    List<Object> l1 = *this;
    List<Object> l2 = other;
    l1.intersectWith(l2, output);
    return;
  }
  HashedList<Object> tempList;
  tempList = *this;
  output.setSize(0);
  for (int i = 0; i < other.size; i ++) {
    if (tempList.contains(other[i])) {
      output.addOnTop(other[i]);
    }
  }
}

template <class Coefficient>
std::string Vector<Coefficient>::toStringLetterFormat(
  const std::string& inputLetter, FormatExpressions* format, bool dontIncludeLastVar
) const {
  if (this->isEqualToZero()) {
    return "0";
  }
  std::stringstream out;
  std::string tempS;
  bool found = false;
  int numberOfVariables = dontIncludeLastVar ? this->size - 1 : this->size;
  for (int i = 0; i < numberOfVariables; i ++) {
    if (!this->objects[i].isEqualToZero()) {
      tempS = (*this)[i].toString(format);
      if ((*this)[i].needsParenthesisForMultiplication(format)) {
        tempS = "(" + tempS + ")";
      }
      if (tempS == "1") {
        tempS = "";
      }
      if (tempS == "- 1" || tempS == "-1") {
        tempS = "-";
      }
      if (found) {
        if (tempS.size() > 0) {
          if (tempS[0] != '-') {
            out << "+";
          }
        } else {
          out << "+";
        }
      }
      found = true;
      out << tempS;
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

template <class Coefficient, unsigned int inputHashFunction(const Coefficient&)>
void MonomialTensor<Coefficient, inputHashFunction>::multiplyByGeneratorPowerOnTheLeft(
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

template <class Coefficient, unsigned int inputHashFunction(const Coefficient&)>
void MonomialTensor<Coefficient, inputHashFunction>::multiplyByGeneratorPowerOnTheRight(
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

template <class Coefficient, unsigned int inputHashFunction(const Coefficient&)>
std::string MonomialTensor<Coefficient, inputHashFunction>::toString(FormatExpressions* format) const {
  if (this->generatorsIndices.size == 0) {
    return "1";
  }
  std::string theLetter = format == nullptr ?  "g" : format->chevalleyGgeneratorLetter;
  std::string letters = "abcdefghijklmnopqrstuvwxyz";
  std::stringstream out;
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    if (static_cast<unsigned>(generatorsIndices[i]) < letters.size()) {
      out << letters[this->generatorsIndices[i]];
    } else {
      out << theLetter << "_{" << this->generatorsIndices[i] << "}";
    }
    out << "^{" << this->powers[i] << "}";
  }
  return out.str();
}

template <class Coefficient, unsigned int inputHashFunction(const Coefficient&)>
bool MonomialTensor<Coefficient, inputHashFunction>::simplifyEqualConsecutiveGenerators(int lowestNonReducedIndex) {
  if (this->generatorsIndices.size < 1) {
    return false;
  }
  if (lowestNonReducedIndex < 0) {
    lowestNonReducedIndex = 0;
  }
  bool result = false;
  for (int next = lowestNonReducedIndex + 1; next < this->generatorsIndices.size; next ++) {
    if (this->generatorsIndices[lowestNonReducedIndex] == this->generatorsIndices[next]) {
      result = true;
      this->powers[lowestNonReducedIndex] += this->powers[next];
    } else {
      lowestNonReducedIndex ++;
      this->powers[lowestNonReducedIndex] = this->powers[next];
      this->generatorsIndices[lowestNonReducedIndex] = this->generatorsIndices[next];
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
  bool constTermsAreGood =
  (constantTerms == 0) ? false : (constantTerms->numberOfRows == this->numberOfRows && constantTerms->numberOfColumns > 0);
  out << "\\begin{array}{l";
  for (int j = 0; j < this->numberOfColumns; j ++) {
    out << "l";
  }
  out << "}";
  std::string currentEntry;
  Polynomial<Coefficient> theMon;
  for (int i = 0; i < this->numberOfRows; i ++) {
    bool foundNonZeroEntry = false;
    for (int j = 0; j < this->numberOfColumns; j ++) {
      if (!((*this)(i, j) == 0)) {
        theMon.makeMonomial(j, 1, (*this)(i, j));
        currentEntry = theMon.toString(format);
        if (currentEntry == "") {
          global.fatal << "Empty strings not allowed as result of toString() function call. " << global.fatal;
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
  std::string tempS;
  bool useHtml    = (format == nullptr) ? true : format->flagUseHTML;
  bool useLatex   = (format == nullptr) ? false : format->flagUseLatex;
  bool usePmatrix = (format == nullptr) ? true : format->flagUsePmatrix;
  if (useHtml) {
    out << "<table>";
  }
  if (useLatex) {
    int verticalLineIndex = format == nullptr ? - 1 : format->matrixColumnVerticalLineIndex;
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
      tempS = (*this)(i, j).toString(format);
      if (useHtml) {
        out << "<td>";
      }
      out << tempS;
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
        out << ", \t ";
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
  int cols_per_elt = 0;
  for (int i = 0; i < this->numberOfRows; i ++) {
    element_strings[i].setSize(this->numberOfColumns);
    for (int j = 0; j < this->numberOfColumns; j ++) {
      std::stringstream ss;
      ss << this->elements[i][j];
      element_strings[i][j] = ss.str();
      int sl = element_strings[i][j].length();
      if (sl > cols_per_elt) {
        cols_per_elt = sl;
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
      int pad = cols_per_elt - sl;
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
std::string LinearCombination<TemplateMonomial, Coefficient>::getBlendCoefficientAndMonomial(
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
  std::string monString = inputMonomial.toString(format);
  if (inputCoefficient.needsParenthesisForMultiplication(format)) {
    coefficientString = "\\left(" + coefficientString + "\\right)";
  }
  if (coefficientString == "1") {
    if (addPlusToFront) {
      out << "+";
    }
    out << monString;
    return out.str();
  }
  if (coefficientString == "- 1" || coefficientString == "-1") {
    out << "-" << monString;
    return out.str();
  }
  if (coefficientString[0] != '-' && addPlusToFront) {
    out << "+" << coefficientString << monString;
  } else {
    out << coefficientString << monString;
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
  std::string coefficientString, monomialString;
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
    if (StringRoutines::stringBeginsWith(coefficientString, "\\frac{1}", &denominatorString)) {
      return "\\frac{" + monomialString + "}" + denominatorString;
    } else if (StringRoutines::stringBeginsWith(coefficientString, "-\\frac{1}", &denominatorString)) {
      return "-\\frac{" + monomialString + "}" + denominatorString;
    }
  }
  if (customCoefficientMonomialSeparator != "") {
    return coefficientString + customCoefficientMonomialSeparator + monomialString;
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
std::string LinearCombination<TemplateMonomial, Coefficient>::toString(
  FormatExpressions* format
) const {
  if (this->size() == 0) {
    return "0";
  }
  MacroRegisterFunctionWithName("LinearCombination::toString");
  std::stringstream out;
  List<TemplateMonomial> sortedMons;
  sortedMons = this->monomials;
  // If this line fails to link, you must do the following.
  // You need to implement FormatExpressions::getMonomialOrder<TemplateMonomial>()
  // and make it return 0 (or a pointer to a monomial order, should you
  // wish to use a custom one.
  typename List<TemplateMonomial>::Comparator*
  theOrder = (format == nullptr) ? 0 : format->getMonomialOrder<TemplateMonomial>();
  sortedMons.quickSortDescending(theOrder);
  int cutOffCounter = 0;
  bool useCustomPlus = false;
  int maximumLineLength = format == nullptr ? - 1 : format->maximumLineLength;
  int numberOfAmpersandsPerNewLineForLaTeX = (format == nullptr) ? 1 : format->numberOfAmpersandsPerNewLineForLaTeX;
  bool flagUseLaTeX = (format == nullptr) ? false : format->flagUseLatex;
  bool flagUseHTML = (format == nullptr) ? false : format->flagUseHTML;
  std::string customTimes = "";
  if (format != nullptr) {
    useCustomPlus = (format->customPlusSign != "");
    if (format->flagPassCustomCoeffMonSeparatorToCoeffs == false) {
      customTimes = format->customCoefficientMonomialSeparator;
      format->customCoefficientMonomialSeparator = "";
    }
  }
  for (int i = 0; i < sortedMons.size; i ++) {
    TemplateMonomial& monomial = sortedMons[i];
    Coefficient& coefficient = this->coefficients[this->monomials.getIndex(monomial)];
    std::string termString = this->getTermString(coefficient, monomial, format, customTimes);
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
        if (flagUseLaTeX && i != sortedMons.size - 1) {
          out << " \\\\";
          for (int k = 0; k < numberOfAmpersandsPerNewLineForLaTeX; k ++) {
            out << "&";
          }
          out << " ";
        }
        if (flagUseHTML && !flagUseLaTeX && (i != sortedMons.size - 1)) {
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

class Lattice {
  void testGaussianEliminationEuclideanDomainRationals(Matrix<Rational>& output);
public:
  Matrix<Rational> basisRationalForm;
  Matrix<LargeInteger> basis;
  LargeIntegerUnsigned denominator;
  int getDimension() const {
    return this->basis.numberOfColumns;
  }
  int getRank() const {
    return this->basis.numberOfRows;
  }
  void intersectWith(const Lattice& other);
  bool findOnePreimageInLatticeOf(
    const Matrix<Rational>& theLinearMap, const Vectors<Rational>& input, Vectors<Rational>& output
  );
  void intersectWithPreimageOfLattice(
    const Matrix<Rational> & theLinearMap, const Lattice& other
  );
  void intersectWithLineGivenBy(Vector<Rational>& inputLine, Vector<Rational>& outputGenerator);
  static bool getClosestPointInDirectionOfTheNormalToAffineWallMovingIntegralStepsInDirection(
    Vector<Rational>& startingPoint,
    Vector<Rational>& theAffineHyperplane,
    Vector<Rational>& theDirection,
    Vector<Rational>& outputPoint
  );
  void getDefaultFundamentalDomainInternalPoint(Vector<Rational>& output);
  bool getInternalPointInConeForSomeFundamentalDomain(
    Vector<Rational>& output, Cone& coneContainingOutputPoint
  );
  void getRootOnLatticeSmallestPositiveProportionalTo(
    Vector<Rational>& input, Vector<Rational>& output
  );
  void getRougherLatticeFromAffineHyperplaneDirectionAndLattice(
    const Vector<Rational>& direction,
    Vector<Rational>& outputDirectionMultipleOnLattice,
    Vector<Rational>& shift,
    Vector<Rational>& affineHyperplane,
    Vectors<Rational>& outputRepresentatives,
    Vectors<Rational>& movementInDirectionPerRepresentative,
    Lattice& outputRougherLattice
  );
  void applyLinearMap(Matrix<Rational>& linearMap, Lattice& output);
  void intersectWithBothOfMaximalRank(const Lattice& other);
  void getDualLattice(Lattice& output) const;
  bool isInLattice(const Vector<Rational>& theVector) const {
    Vector<Rational> tempVect = theVector;
    if (!this->reduceVector(tempVect)) {
      return false;
    }
    return tempVect.isEqualToZero();
  }
  // returns false if the vector is not in the vector space spanned by the lattice
  bool reduceVector(Vector<Rational>& theVector) const;
  // In the following two functions, the format of the matrix theSub of the substitution is as follows.
  // Let the ambient dimension be n, and the coordinates be x_1,..., x_n.
  // Let the new vector space be of dimension m, with coordinates y_1,..., y_m.
  // Then theSub is an n by m matrix, where the i^th row of the matrix gives the expression of x_i via the y_j's.
  // In addition, we require that n>=m (otherwise, in general, we do not expect to get a lattice).
  // For example, if we want to carry out the substitution
  // x_1=y_1+y_2, x_2=y_1-y_2, x_3=y_1, then
  // theSub should be initialized as:
  // 1  1
  // 1 - 1
  // 1  0
  bool substitutionHomogeneous(const Matrix<Rational>& theSub);
  bool substitutionHomogeneous(const PolynomialSubstitution<Rational>& theSub);
  // The following function follows the same convention
  // as the preceding except that we allow n < m. However,
  // in order to assure that the preimage of the lattice is a lattice,
  // we provide as input an ambient lattice
  // in the new vector space of dimension m.
  bool substitutionHomogeneous(const Matrix<Rational>& theSub, Lattice& resultIsSubsetOf);
  void reduce();
  void intersectWithLinearSubspaceSpannedBy(const Vectors<Rational>& theSubspaceBasis);
  void intersectWithLinearSubspaceGivenByNormals(const Vectors<Rational>& theSubspaceNormals);
  void intersectWithLinearSubspaceGivenByNormal(const Vector<Rational>& theNormal);
  static bool getHomogeneousSubstitutionMatrixFromSubstitutionIgnoreConstantTerms(
    const PolynomialSubstitution<Rational>& theSub, Matrix<Rational>& output
  );
  // Returning false means that the lattice given as rougher
  // is not actually rougher than the current one
  // or that there are too many representatives.
  bool getAllRepresentatives(const Lattice& rougherLattice, Vectors<Rational>& output) const;
  bool getAllRepresentativesProjectingDownTo(
    const Lattice& rougherLattice, Vectors<Rational>& startingShifts, Vectors<Rational>& output
  ) const;
  std::string toString() const;
  bool operator==(const Lattice& other) {
    return this->basisRationalForm == other.basisRationalForm;
  }
  void operator=(const Lattice& other) {
    this->basis = other.basis;
    this->denominator = other.denominator;
    this->basisRationalForm = other.basisRationalForm;
  }
  void writeToFile(std::fstream& output);
  bool readFromFile(std::fstream& input);
  void makeZn(int dimension);
  void refineByOtherLattice(const Lattice& other);
  void makeFromRoots(const Vectors<Rational>& input);
  Lattice() {
  }
  Lattice(const Lattice& other) {
    this->operator=(other);
  }
  void makeFromMatrix(const Matrix<Rational>& input);
};

class QuasiPolynomial {
public:
  int minimalNumberOfVariables() const {
    return this->ambientLatticeReduced.basis.numberOfRows;
  }
  Lattice ambientLatticeReduced;
  Vectors<Rational> latticeShifts;
  List<Polynomial<Rational> > valueOnEachLatticeShift;
  std::string toString(bool useHtml, bool useLatex) {
    return this->toString(useHtml, useLatex, nullptr);
  }
  std::string toString(bool useHtml, bool useLatex, FormatExpressions* format);
  Rational evaluate(const Vector<Rational>& input);
  void addLatticeShift(const Polynomial<Rational>& input, const Vector<Rational>& inputShift);
  void makeRougherLattice(const Lattice& latticeToRoughenBy);
  void makeFromPolynomialShiftAndLattice(
    const Polynomial<Rational>& inputPoly, const MonomialPolynomial& theShift, const Lattice& theLattice
  );
  void makeZeroLatticeZn(int theDim);
  void makeZeroOverLattice(Lattice& theLattice);
  bool isEqualToZero() const {
    return this->valueOnEachLatticeShift.size == 0;
  }
  void substitution(
    const Matrix<Rational>& mapFromNewSpaceToOldSpace,
    const Vector<Rational>& inputTranslationSubtractedFromArgument,
    const Lattice& ambientLatticeNewSpace,
    QuasiPolynomial& output
  );
  void substitution(
    const Matrix<Rational>& mapFromNewSpaceToOldSpace,
    const Lattice& ambientLatticeNewSpace,
    QuasiPolynomial& output
  );
  void substitution(
    const Vector<Rational>& inputTranslationSubtractedFromArgument,
    QuasiPolynomial& output
  );
  bool substitutionFewerVariables(
    const PolynomialSubstitution<Rational>& theSub, QuasiPolynomial& output
  ) const;
  void operator+=(const QuasiPolynomial& other);
  QuasiPolynomial(){}
  QuasiPolynomial(const QuasiPolynomial& other) {
    this->operator=(other);
  }
  void operator*=(const Rational& scalar);
  void operator=(const QuasiPolynomial& other) {
    this->ambientLatticeReduced = other.ambientLatticeReduced;
    this->latticeShifts = other.latticeShifts;
    this->valueOnEachLatticeShift = other.valueOnEachLatticeShift;
  }
};

class OnePartialFraction {
private:
  void findPivot();
  void findInitialPivot();
  bool rootIsInFractionCone (PartialFractions& owner, Vector<Rational>* theRoot) const;
  friend class PartialFractions;
  friend class partFractionPolynomialSubstitution;
public:
  std::string DebugString;
  int LastDistinguishedIndex;
  int FileStoragePosition;
  bool PowerSeriesCoefficientIsComputed;
  bool IsIrrelevant;
  bool RelevanceIsComputed;
  List<int> indicesNonZeroMultiplicities;
  List<OnePartialFractionDenominator> denominator;
  friend std::ostream& operator << (std::ostream& output, const OnePartialFraction& input) {
    (void) input;
    global.fatal << " Not implemented, please fix. " << global.fatal;
    return output;
  }
  bool removeRedundantShortRootsClassicalRootSystem(
    PartialFractions& owner, Vector<Rational>* Indicator, Polynomial<LargeInteger>& buffer1, int theDimension
  );
  bool removeRedundantShortRoots(PartialFractions& owner, Vector<Rational>* Indicator, int theDimension);
  bool AlreadyAccountedForInGUIDisplay;
  static bool flagAnErrorHasOccurredTimeToPanic;
  static std::fstream TheBigDump;
  static bool UseGlobalCollector;
  static bool MakingConsistencyCheck;
  static Rational CheckSum, CheckSum2;
  static Vector<Rational> theVectorToBePartitioned;
  void computePolynomialCorrespondingToOneMonomial(
    QuasiPolynomial& outputQP,
    const MonomialPolynomial& theMon,
    Vectors<Rational>& normals,
    Lattice& lattice
  ) const;
  static void evaluateIntegerPolynomial(
    const Polynomial<LargeInteger>& input, const Vector<Rational>& values, Rational& output
  );
  static void makePolynomialFromOneNormal(
    Vector<Rational>& normal,
    const MonomialPolynomial& shiftRational,
    int theMult,
    Polynomial<Rational>& output
  );
  void computeNormals(
    PartialFractions& owner, Vectors<Rational>& output, int theDimension, Matrix<Rational>& buffer
  );
  int computeGainingMultiplicityIndexInLinearRelation(
    bool flagUsingOrlikSolomon, Matrix<Rational>& theLinearRelation
  );
  void getRootsFromDenominator(
    PartialFractions& owner, Vectors<Rational>& output
  ) const;
  void getVectorPartitionFunction(
    PartialFractions& owner, Polynomial<LargeInteger>& theCoeff, QuasiPolynomial& output
  ) const;
  void decomposeAMinusNB(
    int indexA,
    int indexB,
    int n,
    int indexAminusNB,
    LinearCombination<OnePartialFraction, Polynomial<LargeInteger> >& output,
    PartialFractions& owner
  );
  bool decomposeFromLinearRelation(
    Matrix<Rational>& theLinearRelation,
    LinearCombination<OnePartialFraction, Polynomial<LargeInteger> >& output,
    bool flagUsingOSbasis,
    List<Vector<Rational> >& startingVectors
  );
  void computeOneCheckSum(
    PartialFractions& owner, Rational& output, int theDimension
  ) const;
  bool reduceMeOnce(
    const Polynomial<LargeInteger>& myCoeff,
    Polynomial<LargeInteger>& outputCoeff,
    Vectors<Rational>& startingVectors
  );
  void reduceMonomialByMonomial(PartialFractions& owner, int myIndex, Vector<Rational>* Indicator);
  void applySzenesVergneFormula(
    List<Vector<Rational> >& startingVectors,
    List<int>& theSelectedIndices,
    List<int>& theElongations,
    int GainingMultiplicityIndex,
    int ElongationGainingMultiplicityIndex,
    LinearCombination<OnePartialFraction, Polynomial<LargeInteger> >& output
  );
  void applyGeneralizedSzenesVergneFormula(
    List<int>& theSelectedIndices,
    List<int>& theGreatestElongations,
    List<int>& theCoefficients,
    int GainingMultiplicityIndex,
    int ElongationGainingMultiplicityIndex,
    LinearCombination<OnePartialFraction,
    Polynomial<LargeInteger> >& output,
    List<Vector<Rational> >& startingVectors
  );
  bool checkForOrlikSolomonAdmissibility(List<int>& theSelectedIndices);
  bool reduceOnceTotalOrderMethod(
    LinearCombination<OnePartialFraction, Polynomial<LargeInteger> >& output, PartialFractions& owner
  );
  bool reduceOnceGeneralMethodNoOSBasis(
    PartialFractions& owner,
    LinearCombination<OnePartialFraction,
    Polynomial<LargeInteger> >& output,
    Vectors<Rational>& bufferVectors,
    Matrix<Rational>& bufferMat
  );
  bool reduceOnceGeneralMethod(
    PartialFractions& owner,
    LinearCombination<OnePartialFraction,
    Polynomial<LargeInteger> >& output,
    Vectors<Rational>& bufferVectors,
    Matrix<Rational>& bufferMat
  );
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const OnePartialFraction& input) {
    return input.hashFunction();
  }
  void initialize(int numRoots);
  void computeIndicesNonZeroMultiplicities();
  bool decreasePowerOneFraction(int index, int increment);
  void prepareFraction(
    int indexA,
    int indexB,
    int AminusNBindex,
    bool indexAisNullified,
    OnePartialFraction& output,
    Polynomial<LargeInteger>& AminusNbetaPoly,
    Polynomial<LargeInteger>& outputCommonCoeff
  );
  void assign(const OnePartialFraction& p);
  void assignDenominatorOnly(const OnePartialFraction& p);
  void assignNoIndicesNonZeroMults(OnePartialFraction& p);
  int getSmallestNonZeroIndexGreaterThanOrEqualTo(PartialFractions& owner, int minIndex);
  int controlLineSizeFracs(std::string& output, FormatExpressions& PolyFormatLocal);
  int controlLineSizeStringPolys(std::string& output, FormatExpressions& PolyFormatLocal);
  //void swap(int indexA, int indexB);
  OnePartialFraction();
  ~OnePartialFraction();
  void getPolyReduceMonomialByMonomial(
    PartialFractions& owner,
    Vector<Rational>& theExponent,
    int StartMonomialPower,
    int DenPowerReduction,
    int startDenominatorPower,
    Polynomial<LargeInteger>& output
  );
  void reduceMonomialByMonomialModifyOneMonomial(
    PartialFractions& Accum,
    SelectionWithDifferentMaxMultiplicities& thePowers,
    List<int>& thePowersSigned,
    MonomialPolynomial& input,
    LargeInteger& inputCoeff
  );
  void getAlphaMinusNBetaPoly(PartialFractions& owner, int indexA, int indexB, int n, Polynomial<LargeInteger>& output);
  void getNElongationPolynomialWithMonomialContribution(
    List<Vector<Rational> >& startingVectors,
    List<int>& theSelectedIndices,
    List<int>& theCoefficients,
    List<int>& theGreatestElongations,
    int theIndex,
    Polynomial<LargeInteger>& output,
    int theDimension
  );
  void getNElongationPolynomial(
    List<Vector<Rational> >& startingVectors,
    int index,
    int baseElongation,
    int LengthOfGeometricSeries,
    Polynomial<LargeInteger>& output,
    int theDimension
  );
  static void getNElongationPolynomial(Vector<Rational>& exponent, int n, Polynomial<LargeInteger>& output, int theDimension);
  int getNumberProportionalVectorsClassicalRootSystems(PartialFractions& owner);
  bool operator==(const OnePartialFraction& right) const;
  void operator=(const OnePartialFraction& right);
  bool initFromRoots(PartialFractions& owner, Vectors<Rational>& input);
  std::string toString(bool LatexFormat, FormatExpressions& PolyFormatLocal, int& NumLinesUsed);
  void readFromFile(PartialFractions& owner, std::fstream& input);
  void writeToFile(std::fstream& output) const;
  int sizeWithoutDebugString() const;
};

class Cone {
  void computeVerticesFromNormalsNoFakeVertices();
  bool eliminateFakeNormalsUsingVertices(int numAddedFakeWalls);
  friend std::ostream& operator << (std::ostream& output, const Cone& theCone) {
    output << theCone.toString();
    return output;
  }
public:
  bool flagIsTheZeroCone;
  Vectors<Rational> vertices;
  Vectors<Rational> normals;
  int LowestIndexNotCheckedForChopping;
  int LowestIndexNotCheckedForSlicingInDirection;
  std::string toString(FormatExpressions* format = nullptr) const;
  void transformToWeylProjective(ConeComplex& owner);
  std::string drawMeToHtmlProjective(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat);
  std::string drawMeToHtmlLastCoordAffine(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat);
  void getLinesContainedInCone(Vectors<Rational>& output);
  void translateMeMyLastCoordinateAffinization(Vector<Rational>& theTranslationVector);
  bool isHonest1DEdgeAffine(const Vector<Rational>& vertex1, const Vector<Rational>& vertex2) const {
    int numCommonWalls = 0;
    for (int i = 0; i < this->normals.size; i ++) {
      if (
        vertex1.scalarEuclidean(this->normals[i]).isEqualToZero() &&
        vertex2.scalarEuclidean(this->normals[i]).isEqualToZero()
      ) {
        numCommonWalls ++;
        if (numCommonWalls == this->getDimension() - 2) {
          return true;
        }
      }
    }
    return false;
  }
  bool isTheEntireSpace() {
    return this->normals.size == 0 && this->flagIsTheZeroCone;
  }
  bool isHonest1DEdgeAffine(int vertexIndex1, int vertexIndex2) const {
    Vector<Rational>& vertex1 = this->vertices[vertexIndex1];
    Vector<Rational>& vertex2 = this->vertices[vertexIndex2];
    return this->isHonest1DEdgeAffine(vertex1, vertex2);
  }
  static bool isRegularToBasis(
    const Vectors<Rational>& basis,
    Vector<Rational>& r,
    Vector<Rational>& outputFailingNormal,
    int theDimension
  );
  static bool regularizeToBasis(
    const Vectors<Rational> &basis,
    Vector<Rational>& output
  );
  bool drawMeLastCoordinateAffine(
    bool InitDrawVars,
    DrawingVariables& theDrawingVariables,
    FormatExpressions& theFormat,
    const std::string& ChamberWallColor = nullptr
  ) const;
  bool drawMeProjective(
    Vector<Rational>* coordCenterTranslation,
    bool initTheDrawVars,
    DrawingVariables& theDrawingVariables,
    FormatExpressions& theFormat
  ) const;
  bool isInCone(const Vector<Rational>& point) const;
  bool isInCone(const Vectors<Rational>& vertices) const {
    for (int i = 0; i < vertices.size; i ++) {
      if (!this->isInCone(vertices[i])) {
        return false;
      }
    }
    return true;
  }
  bool getLatticePointsInCone(
    Lattice& theLattice,
    Vector<Rational>& theShift,
    int upperBoundPointsInEachDim,
    bool lastCoordinateIsOne,
    Vectors<Rational>& outputPoints,
    Vector<Rational>* shiftAllPointsBy
  ) const;
  bool makeConvexHullOfMeAnd(const Cone& other);
  void changeBasis(Matrix<Rational>& theLinearMap);
  std::string DebugString;
  int getDimension() const {
    if (this->normals.size == 0) {
      return 0;
    }
    return this->normals[0].size;
  }
  void sliceInDirection(Vector<Rational>& direction, ConeComplex& output);
  bool createFromNormals(
    Vectors<Rational>& inputNormals,
    bool useWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices
  );
  //returns false if the cone is non-proper, i.e. when either
  //1) the cone is empty or is of smaller dimension than it should be
  //2) the resulting cone is the entire space
  bool createFromNormals(Vectors<Rational>& inputNormals) {
    return this->createFromNormals(inputNormals, false);
  }
  bool createFromVertices(const Vectors<Rational>& inputVertices);
  static void scaleNormalizeByPositive(Vector<Rational>& toScale);
  void getInternalPoint(Vector<Rational>& output) const {
    if (this->vertices.size <= 0) {
      return;
    }
    this->vertices.sum(output, this->vertices[0].size);
  }
  Vector<Rational> getInternalPoint() const {
    Vector<Rational> result;
    this->getInternalPoint(result);
    return result;
  }
  unsigned int hashFunction() const {
    return this->vertices.hashFunction();
  }
  static unsigned int hashFunction(const Cone& input) {
    return input.hashFunction();
  }
  bool produceNormalFromTwoNormalsAndSlicingDirection(
    Vector<Rational>& SlicingDirection,
    Vector<Rational>& normal1,
    Vector<Rational>& normal2,
    Vector<Rational>& output
  );
  void operator=(const Cone& other) {
    //this->flagHasSufficientlyManyVertices = other.flagHasSufficientlyManyVertices;
    this->flagIsTheZeroCone = other.flagIsTheZeroCone;
    this->vertices = other.vertices;
    this->normals = other.normals;
    this->LowestIndexNotCheckedForSlicingInDirection = other.LowestIndexNotCheckedForSlicingInDirection;
    this->LowestIndexNotCheckedForChopping = other.LowestIndexNotCheckedForChopping;
  }
  Cone(const Cone& other) {
    this->operator=(other);
  }
  Cone() {
    this->LowestIndexNotCheckedForSlicingInDirection = 0;
    this->LowestIndexNotCheckedForChopping = 0;
    this->flagIsTheZeroCone = true;
    //this->flagHasSufficientlyManyVertices = true;
  }
  void intersectHyperplane(Vector<Rational>& theNormal, Cone& outputConeLowerDim);
  bool getRootFromLPolynomialConstantTermGoesToLastVariable(Polynomial<Rational>& inputLPoly, Vector<Rational>& output);
  bool solveLPolynomialEqualsZeroIAmProjective(Polynomial<Rational>& inputLPoly, Cone& outputCone);
  bool solveLQuasiPolyEqualsZeroIAmProjective(QuasiPolynomial& inputLQP, List<Cone>& outputConesOverEachLatticeShift);
  bool operator>(const Cone& other) const {
    return this->normals > other.normals;
  }
  bool operator==(const Cone& other) const {
    return this->flagIsTheZeroCone == other.flagIsTheZeroCone && this->normals == other.normals;
  }
};

class ConeLatticeAndShift {
  public:
  Cone theProjectivizedCone;
  Lattice theLattice;
  Vector<Rational> theShift;
  void findExtremaInDirectionOverLatticeOneNonParametric(
    Vector<Rational>& theLPToMaximizeAffine,
    Vectors<Rational>& outputAppendLPToMaximizeAffine,
    List<ConeLatticeAndShift>& outputAppend
  );
  void operator=(const ConeLatticeAndShift& other) {
    this->theProjectivizedCone = other.theProjectivizedCone;
    this->theLattice = other.theLattice;
    this->theShift = other.theShift;
  }
  void writeToFile(std::fstream& output);
  void findExtremaInDirectionOverLatticeOneNonParamDegenerateCase(
    Vector<Rational>& theLPToMaximizeAffine,
    Vectors<Rational>& outputAppendLPToMaximizeAffine,
    List<ConeLatticeAndShift>& outputAppend,
    Matrix<Rational>& theProjectionLatticeLevel
  );
  bool readFromFile(std::fstream& input);
  std::string toString(FormatExpressions& theFormat);
  int getDimensionProjectivized() {
    return this->theProjectivizedCone.getDimension();
  }
  int getDimensionAffine() {
    return this->theProjectivizedCone.getDimension() - 1;
  }
};

class ConeComplex : public HashedList<Cone> {
public:
  bool flagChambersHaveTooFewVertices;
  bool flagIsRefined;
//  List<int> NonRefinedChambers;
  int indexLowestNonRefinedChamber;
  HashedList<Vector<Rational> > splittingNormals;
  Vectors<Rational> slicingDirections;
  Cone ConvexHull;
  void refineOneStep();
  void refine();
  void refineMakeCommonRefinement(const ConeComplex& other);
  void sort();
  void refineAndSort();
  void findMaxmumOverNonDisjointChambers(Vectors<Rational>& theMaximaOverEachChamber, Vectors<Rational>& outputMaxima);
  void makeAffineAndTransformToProjectiveDimPlusOne(Vector<Rational>& affinePoint, ConeComplex& output);
  void transformToWeylProjective();
  int getDimension() {
    if (this->size <= 0) {
      return - 1;
    }
    return this->objects[0].getDimension();
  }
  bool addNonRefinedChamberOnTopNoRepetition(const Cone& newCone);
  void popChamberSwapWithLast(int index);
  void getAllWallsConesNoOrientationNoRepetitionNoSplittingNormals(Vectors<Rational>& output) const;
  bool drawMeLastCoordinateAffine(bool InitDrawVars, DrawingVariables& theDrawingVariables, FormatExpressions& theFormat);
  void translateMeMyLastCoordinateAffinization(Vector<Rational> & theTranslationVector);
  void initializeFromDirectionsAndRefine(Vectors<Rational>& inputVectors);
  void initializeFromAffineDirectionsAndRefine(Vectors<Rational>& inputDirections, Vectors<Rational>& inputAffinePoints);
  std::string drawMeToHtmlLastCoordAffine(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat);
  bool drawMeProjective(
    Vector<Rational>* coordCenterTranslation,
    bool InitDrawVars,
    DrawingVariables& theDrawingVariables,
    FormatExpressions& theFormat
  );
  std::string drawMeToHtmlProjective(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat);
  std::string toString(bool useHtml = false);
  int getLowestIndexChamberContaining(const Vector<Rational>& theRoot) const {
    for (int i = 0; i < this->size; i ++) {
      if (this->objects[i].isInCone(theRoot)) {
        return i;
      }
    }
    return - 1;
  }
  bool findMaxLFOverConeProjective(
    const Cone& input, List<Polynomial<Rational> >& inputLinPolys, List<int>& outputMaximumOverEeachSubChamber
  );
  bool findMaxLFOverConeProjective(
    const Cone& input, Vectors<Rational>& inputLFsLastCoordConst, List<int>& outputMaximumOverEeachSubChamber
  );
  void initFromCones(
    List<Vectors<Rational> >& normalsOfCones,
    bool useWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices
  );
  void initFromCones(
    List<Cone>& normalsOfCones, bool useWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices
  );
  bool splitChamber(
    int indexChamberBeingRefined, bool weAreChopping, const Vector<Rational>& killerNormal
  );
  void getNewVerticesAppend(
    Cone& myDyingCone, const Vector<Rational>& killerNormal, HashedList<Vector<Rational> >& outputVertices
  );
  void initialize() {
    this->splittingNormals.clear();
    this->slicingDirections.size = 0;
    this->clear();
    this->indexLowestNonRefinedChamber = 0;
    this->ConvexHull.normals.size = 0;
    this->ConvexHull.vertices.size = 0;
    this->ConvexHull.flagIsTheZeroCone = true;
  }
  ConeComplex(const ConeComplex& other):HashedList<Cone>() {
    this->operator=(other);
  }
  ConeComplex() {
    this->flagChambersHaveTooFewVertices = false;
    this->flagIsRefined = false;
  }
  void writeToFile(std::fstream& output, int UpperLimit = - 1);
  bool readFromFile(std::fstream& input, int UpperLimitDebugPurposes = - 1);
  void operator=(const ConeComplex& other) {
    this->::HashedList<Cone>::operator=(other);
    this->splittingNormals = other.splittingNormals;
    this->slicingDirections = other.slicingDirections;
    this->indexLowestNonRefinedChamber = other.indexLowestNonRefinedChamber;
    this->flagIsRefined = other.flagIsRefined;
    this->flagChambersHaveTooFewVertices = other.flagChambersHaveTooFewVertices;
  }
};

class PartialFractions: public LinearCombination<OnePartialFraction, Polynomial<LargeInteger> > {
  bool splitPartial();
  void initCommon();
public:
  int AmbientDimension;
  int IndexLowestNonProcessed;
  int IndexCurrentlyProcessed;
  int HighestIndex;
  int NumberIrrelevantFractions;
  int NumberRelevantReducedFractions;
  int numberOfMonomialsInTheNumerators;
  int NumGeneratorsInTheNumerators;
  int NumRelevantNonReducedFractions;
  //int NumRelevantMonomialsInNonReducedFractions;
  //int NumRelevantGeneratorsInNonReducedFractions;
  static int NumMonomialsInNumeratorsRelevantFractions;
  int NumGeneratorsRelevenatFractions;
  int NumMonomialsInNumeratorsIrrelevantFractions;
  int NumGeneratorsIrrelevantFractions;
  int NumTotalReduced;
  int NumProcessedForVPFfractions;
  int NumRunsReduceMonomialByMonomial;
  static int NumProcessedForVPFMonomialsTotal;
  static std::fstream ComputedContributionsList;
  static const int MaxReadFileBufferSize = 33554432; //= 32 MB of read buffer size
  Rational StartCheckSum;
  Rational EndCheckSum;
  static Rational CheckSum;
  bool flagDiscardingFractions;
  bool flagUsingOrlikSolomonBasis;
  bool flagInitialized;
  int LimitSplittingSteps;
  int SplitStepsCounter;
  ConeComplex theChambers;
  static bool flagSplitTestModeNoNumerators;
  static bool flagAnErrorHasOccurredTimeToPanic;
  static bool flagMakingProgressReport;
  static bool flagUsingCheckSum;
  static int flagMaxNumStringOutputLines;

  HashedList<Vector<Rational> > startingVectors;
  Matrix<int> TableAllowedAminusB;
  Matrix<int> TableAllowedAminus2B;
  Selection IndicesRedundantShortRoots;
  List<int> IndicesDoublesOfRedundantShortRoots;
  int NumNonRedundantShortRoots;
  Vector<Rational> weights;
  void initFromRoots(Vectors<Rational>& theAlgorithmBasis, Vector<Rational>* theWeights);
  int addRootAndSort(Vector<Rational>& theRoot);
  int getIndex(const Vector<Rational>& TheRoot);
  int getIndexDoubleOfARoot(const Vector<Rational>& TheRoot);
  void computeTable(int theDimension);
  void prepareCheckSums();
  std::string doTheFullComputationReturnLatexFileString(
    Vectors<Rational>& toBePartitioned, FormatExpressions& theFormat, std::string* outputHtml
  );
  bool argumentsAllowed(Vectors<Rational>& theArguments, std::string& outputWhatWentWrong);
  bool assureIndicatorRegularity(Vector<Rational> & theIndicator);
  void compareCheckSums();
  void computePolynomialCorrespondingToOneMonomial(
    QuasiPolynomial& outputQP,
    int monomialIndex,
    Vectors<Rational>& normals,
    Lattice& theLattice
  );
  bool initFromRoots(Vectors<Rational>& input);
  void initAndSplit(Vectors<Rational>& input);
  void run(Vectors<Rational>& input);
  //row index is the index of the Vector<Rational> ; column(second) index is the coordinate index
  void removeRedundantShortRootsClassicalRootSystem(Vector<Rational>* Indicator);
  void removeRedundantShortRoots(Vector<Rational>* Indicator);
  bool removeRedundantShortRootsIndex(int theIndex, Vector<Rational>* Indicator);
  bool splitClassicalRootSystem(bool ShouldElongate, Vector<Rational>* Indicator);
  bool split(Vector<Rational>* Indicator);
  void computeKostantFunctionFromWeylGroup(
    char WeylGroupLetter,
    int WeylGroupNumber,
    QuasiPolynomial& output,
    Vector<Rational>* ChamberIndicator
  );
  bool isHigherThanWithRespectToWeight(
    const Vector<Rational>& left, const Vector<Rational>& r, const Vector<Rational>& theWeights
  );
  void computeSupport(List<Vectors<Rational> >& output);
  void computeOneCheckSum(Rational& output);
  void popIndexHashChooseSwapByLowestNonProcessedAndAccount(int index, Vector<Rational>* Indicator);
  void prepareIndicatorVariables();
  void initFromOtherPartialFractions(PartialFractions& input);
  std::string toString(FormatExpressions& theFormat) {
    std::string tempS;
    this->toString(tempS, theFormat);
    return tempS;
  }
  void toString(std::string& output, FormatExpressions& theFormat);
  int toString(std::string& output, bool LatexFormat, FormatExpressions& theFormat);
  int toStringBasisChange(
    std::string& output, bool LatexFormat, FormatExpressions& PolyFormatLocal
  );
  int toFileOutput(std::fstream& output, bool LatexFormat);
  int toFileOutputBasisChange(std::fstream& output, bool LatexFormat);
  bool getVectorPartitionFunction(QuasiPolynomial& output, Vector<Rational>& newIndicator);
  bool verifyFileComputedContributions();
  void writeToFileComputedContributions(std::fstream& output);
  int readFromFileComputedContributions(std::fstream& input);
  void writeToFile(std::fstream& output);
  void readFromFile(std::fstream& input);
  void resetRelevanceIsComputed() {
    global.fatal << "This is not implemented yet. " << global.fatal;
  }
  PartialFractions();
  int sizeWithoutDebugString();
  bool checkForMinimalityDecompositionWithRespectToRoot(Vector<Rational>* theRoot);
  void makeProgressReportSplittingMainPart();
  void makeProgressVPFcomputation();
};

class DynkinSimpleType {
  friend std::ostream& operator << (std::ostream& output, const DynkinSimpleType& theMon) {
    output << theMon.toString();
    return output;
  }
  public:
  char letter;
  int rank;
  Rational cartanSymmetricInverseScale;
  DynkinSimpleType(): letter('X'), rank(- 1), cartanSymmetricInverseScale(0) {
  }
  DynkinSimpleType(const DynkinSimpleType& other) {
    *this = other;
  }
  DynkinSimpleType(char inputChar, int inputRank, const Rational& inputScale = 1) :
    letter(inputChar), rank(inputRank), cartanSymmetricInverseScale(inputScale) {

  }
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
  void grow(List<DynkinSimpleType>& output, List<List<int> >* outputPermutationRoots) const;
  void operator=(const DynkinSimpleType& other) {
    this->letter = other.letter;
    this->rank = other.rank;
    this->cartanSymmetricInverseScale = other.cartanSymmetricInverseScale;
  }
  bool operator==(const DynkinSimpleType& other) const {
    return
    this->letter == other.letter && this->rank == other.rank &&
    this->cartanSymmetricInverseScale == other.cartanSymmetricInverseScale;
  }
  static unsigned int hashFunction(const DynkinSimpleType& input) {
    return static_cast<unsigned int>(input.letter) * 2 +
    static_cast<unsigned int>(input.rank) + someRandomPrimes[0] * input.cartanSymmetricInverseScale.hashFunction();
  }
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  void getAutomorphismActingOnVectorColumn(MatrixTensor<Rational>& output, int AutoIndex) const;
  Rational getDefaultCoRootLengthSquared(int rootIndex) const;
  Rational getDefaultRootLengthSquared(int rootIndex) const;
  Rational getDefaultLongRootLengthSquared() const;
  Rational getEpsilonRealizationLongRootLengthSquared() const;
  static Rational getDefaultLongRootLengthSquared(char theInputType) {
    DynkinSimpleType theType(theInputType, 2);
    return theType.getDefaultLongRootLengthSquared();
  }
  Rational getLongRootLengthSquared() const;
  Rational getRatioRootSquaredToFirstSquared(int rootIndex) const;
  static Rational getRatioLongRootToFirst(char inputWeylLetter, int inputRank);
  static Rational getDynkinIndexParabolicallyInducingSubalgebra(char inputType);
  bool canBeExtendedParabolicallyTo(const DynkinSimpleType& otherType) const;
  bool canBeExtendedParabolicallyOrIsEqualTo(const DynkinSimpleType& otherType) const {
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
  std::string ToStringNonTechnicalName(FormatExpressions* format = nullptr) const;
  void operator++(int);
  bool operator>(const DynkinSimpleType& other) const;
  static void getEpsilonMatrix(char WeylLetter, int WeylRank, Matrix<Rational>& output);
  bool operator<(const DynkinSimpleType& other) const {
    return other > *this;
  }
  bool operator<(int otherRank) const;
};

//this class may need a modification: perhaps it should not inherit monomial collection,
//but rather have a monomial collection as a private member.
//Many important operations with Dynkin types require fixed order of monomials,
//which may impose this reorganization.
class DynkinType: public LinearCombination<DynkinSimpleType, Rational> {
public:
  void getLettersTypesMultiplicities(
    List<char>* outputLetters = nullptr,
    List<int>* outputRanks = nullptr,
    List<int>* outputMults = nullptr,
    List<Rational>* outputFirstCoRootLengthsSquared = nullptr
  ) const;
  void getTypesWithMults(List<DynkinSimpleType>& output) const;
  bool isOfSimpleType(char inputType, int inputRank) const {
    char currentType;
    int currentRank;
    if (!this->isSimple(&currentType, &currentRank)) {
      return false;
    }
    return currentType == inputType && currentRank == inputRank;
  }
  static void getOuterAutosGeneratorsOneTypeActOnVectorColumn(
    List<MatrixTensor<Rational> >& output, const DynkinSimpleType& theType, int multiplicity
  );
  unsigned int hashFunction() const {
    return this->LinearCombination<DynkinSimpleType, Rational>::hashFunction();
  }
  static unsigned int hashFunction(const DynkinType& input) {
    return LinearCombination<DynkinSimpleType, Rational>::hashFunction(input);
  }
  void getOuterAutosGeneratorsActOnVectorColumn(List<MatrixTensor<Rational> >& output);
  bool isSimple(char* outputtype = nullptr, int* outputRank = nullptr, Rational* outputLength = nullptr) const;
  void getSortedDynkinTypes(List<DynkinSimpleType>& output) const;
  Rational getPrincipalSlTwoCartanSymmetricInverseScale() const;
  void sortDynkinTypes();
  bool grow(
    const List<Rational>& allowedInverseScales,
    int AmbientWeylDim,
    List<DynkinType>& output,
    List<List<int> >* outputPermutationRoots
  ) const;
  bool hasPrecomputedSubalgebras() const;
  std::string toStringVirtualNameFolder() const;
  bool containsType(char theTypeLetter) const;
  void getDynkinTypeWithDefaultScales(DynkinType& output) const;
  static void getPrecomputedDynkinTypes(List<DynkinType>& output);
  DynkinSimpleType getGreatestSimpleType() const;
  DynkinSimpleType getSmallestSimpleType() const;
  LargeInteger getWeylGroupSizeByFormula() const;
  std::string toString(FormatExpressions* format = nullptr) const;
  void scaleFirstCoRootSquaredLength(const Rational& multiplyCoRootSquaredLengthBy);
  int getMultiplicity(int SimpleTypeIdentifier) const {
    int result = 0;
    if (!this->coefficients[SimpleTypeIdentifier].isSmallInteger(&result)) {
      global.fatal
      << "Dynkin type has multiplicity that is not a small integer. "
      << global.fatal;
    }
    return result;
  }
  int getNumberOfSimpleComponentsOfGivenRank(int desiredRank) const;
  int getNumberOfSimpleComponents() const;
  Rational getRankRational() const;
  int getRank() const;
  int getRootSystemSize() const {
    Rational result = 0;
    for (int i = 0; i < this->size(); i ++) {
      result += this->coefficients[i] * (*this)[i].getRootSystemSize();
    }
    int intResult = 0;
    if (!result.isSmallInteger(&intResult)) {
      global.fatal << "Multiplicity of simple type is not a small integer. " << global.fatal;
    }
    return intResult;
  }
  int getLieAlgebraDimension() const {
    Rational result = 0;
    for (int i = 0; i < this->size(); i ++) {
      result += this->coefficients[i] * (*this)[i].getLieAlgebraDimension();
    }
    int intResult = 0;
    if (!result.isSmallInteger(&intResult)) {
      global.fatal << "Multiplicity of simple type is not a small integer. " << global.fatal;
    }
    return intResult;
  }
  bool isTypeAOne() const;
  static int GetIndexPreimageFromRootInjection(int inputIndex, const List<int>& inputRootInjection);
  bool canBeExtendedParabolicallyTo(const DynkinType& other) const;
  bool canBeExtendedParabolicallyOrIsEqualTo(const DynkinType& other) const;
  void makeSimpleType(char type, int rank, const Rational* inputFirstCoRootSqLength = nullptr);
  void getEpsilonMatrix(Matrix<Rational>& output) const;
  void getCoCartanSymmetric(Matrix<Rational>& output) const;
  void getCartanSymmetric(Matrix<Rational>& output) const;
  void getCartanSymmetricDefaultLengthKeepComponentOrder(Matrix<Rational>& output) const;
  int getCoxeterEdgeWeight(int v, int w);
  std::string getWeylGroupName(FormatExpressions* format = nullptr) const;
  static void getDynkinIndicesSl2SubalgebrasSimpleType(
    const DynkinSimpleType& theType,
    List<List<Rational> >& precomputedDynkinIndicesSl2subalgebrasSimpleTypes,
    HashedList<DynkinSimpleType>& dynkinSimpleTypesWithComputedSl2Subalgebras,
    HashedList<Rational>& outputDynkinIndices
  );
  void getDynkinIndicesSl2Subalgebras(
    List<List<Rational> >& precomputedDynkinIndicesSl2subalgebrasSimpleTypes,
    HashedList<DynkinSimpleType>& dynkinSimpleTypesWithComputedSl2Subalgebras,
    HashedList<Rational>& outputDynkinIndices
  );
  bool hasExceptionalComponent() const;
  bool operator>(const DynkinType& other) const;
  void operator=(const LinearCombination<DynkinSimpleType, Rational>& other) {
    this->::LinearCombination<DynkinSimpleType, Rational>::operator=(other);
  }
  DynkinType operator-(const LinearCombination<DynkinSimpleType, Rational>& other) {
    DynkinType result = *this;
    result -= other;
    return result;
  }
};

class DynkinDiagramRootSubalgebra {
public:
  Vectors<Rational> ambientRootSystem;
  Matrix<Rational> ambientBilinearForm;
  List<Vectors<Rational> > simpleBasesConnectedComponents;
  //to each connected component of the simple bases corresponds
  //its dynkin string with the same index
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
  Rational getSquareLengthLongestRootLinkedTo(const Vector<Rational>& inputVector);
  Rational getSquareLengthShortestRootLinkedTo(const Vector<Rational>& inputVector);
  bool letterIsDynkinGreaterThanLetter(char letter1, char letter2);
  // The function below takes as an input a set of roots and
  // computes the corredponding Dynkin diagram of the
  // root subsystem. Note: the simleBasisInput is required to be a set of simple roots.
  // The procedure calls a
  // transformation to simple basis on the simpleBasisInput,
  // so your input will get changed if it wasn't
  // simple as required.
  void computeDiagramTypeModifyInput(Vectors<Rational>& inputRoots, WeylGroupData& theWeyl);
  //the below function is just as the above but doesn't modify simpleBasisInput
  void computeDiagramInputIsSimple(const Vectors<Rational>& simpleBasisInput);
  void computeDiagramTypeModifyInputRelative(
    Vectors<Rational>& inputOutputSimpleWeightSystem,
    const HashedList<Vector<Rational> >& weightSystem,
    const Matrix<Rational>& theBilinearForm
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
    List<int>& thePerm,
    List<List<List<int> > >& theAutos,
    SelectionWithDifferentMaxMultiplicities& theAutosPerm,
    DynkinDiagramRootSubalgebra& right
  );
};

class VectorPartition {
public:
  Vectors<Rational> partitioningRoots;
  Vector<Rational> goalVector;
  List<int> currentPartition;
  Vector<Rational> currentPartitionSum;
  // Format: each element of thePartitions gives an array whose entries give
  // the multiplicity of the weights. I.e. if PartitioningRoots has 2 elements, then thePartitions[0]
  // would have 2 elements: the first giving the multiplicity of PartitioningRoots[0]
  // and the second - the multiplicity of
  // PartitioningRoots[1]
  List<List<int> > thePartitions;
  bool flagStoreAllPartitions;
  VectorPartition() {
    this->flagStoreAllPartitions = false;
  }
  Vector<Rational> getPartitionSum();
  void beefUpPartition();
  bool nudgePartition();
  std::string toStringPartitioningVectors();
  std::string toStringOnePartition(const List<int>& currentPartition);
  std::string toStringAllPartitions(bool useHtml);
  bool initialize(const Vectors<Rational>& inputPartitioningRoots, const Vector<Rational>& inputRoot);
  bool incrementReturnFalseIfPastLast();
};

template <class Coefficient>
class ElementWeylAlgebra : public LinearCombination<MonomialWeylAlgebra, Coefficient> {
public:
  unsigned int hashFunction() const {
    return this->::LinearCombination<MonomialWeylAlgebra, Coefficient>::hashFunction();
  }
  static unsigned int hashFunction(const ElementWeylAlgebra& input) {
    return input.hashFunction();
  }
  void makeGEpsPlusEpsInTypeD(int i, int j, int NumVars);
  void makeGEpsMinusEpsInTypeD(int i, int j, int NumVars);
  void makeGMinusEpsMinusEpsInTypeD(int i, int j, int NumVars);
  void makexixj(int i, int j, int NumVars);
  void makexi(int i, int NumVars);
  void makedi(int i, int NumVars);
  void makedidj(int i, int j, int NumVars);
  void makexidj(int i, int j, int NumVars);
  static void getStandardOrderDifferentialOperatorCorrespondingToNRaisedTo(
    const Rational& inputRationalPower,
    int indexVar,
    ElementWeylAlgebra& outputDO,
    Polynomial<Rational>& outputDenominator
  );
  void fourierTransform(ElementWeylAlgebra<Coefficient>& output) const;
  bool actOnPolynomial(Polynomial<Rational>& thePoly) const;
  void setNumberOfVariables(int newNumVars);
  void multiplyOnTheLeft(const ElementWeylAlgebra& standsOnTheLeft);
  static void lieBracket(const ElementWeylAlgebra& left, const ElementWeylAlgebra& right, ElementWeylAlgebra& output);
  void lieBracketOnTheLeft(const ElementWeylAlgebra& standsOnTheLeft);
  void lieBracketOnTheLeftMakeReport(const ElementWeylAlgebra& standsOnTheLeft);
  void lieBracketOnTheRightMakeReport(const ElementWeylAlgebra& standsOnTheRight);
  void lieBracketOnTheRight(const ElementWeylAlgebra& standsOnTheRight);
  bool substitution(
    const PolynomialSubstitution<Rational>& substitutionPolynomialPart,
    const PolynomialSubstitution<Rational>& substitutionDifferentialPart
  );
  void makeOne() {
    MonomialWeylAlgebra tempMon;
    tempMon.polynomialPart.makeOne();
    tempMon.differentialPart.makeOne();
    this->makeZero();
    this->addMonomial(tempMon, 1);
  }
  bool isPolynomial(Polynomial<Coefficient>* whichPoly = 0) const;
  bool hasNonSmallPositiveIntegerDerivation() const;
  void raiseToPower(int thePower);
  void multiplyTwoMonomials(
    const MonomialWeylAlgebra& left, const MonomialWeylAlgebra& right, ElementWeylAlgebra& output
  ) const;
  void assignPolynomial(const Polynomial<Rational>& input) {
    this->makeZero();
    MonomialWeylAlgebra tempM;
    for (int i = 0; i < input.size(); i ++) {
      tempM.polynomialPart = input[i];
      this->addMonomial(tempM, input.coefficients[i]);
    }
  }
  void operator=(const std::string& input);
  void operator*=(const Coefficient& other) {
    this->LinearCombination<MonomialWeylAlgebra, Coefficient>::operator*=(other);
  }
  void operator*=(const ElementWeylAlgebra& other);
};

class LittelmannPath {
public:
  WeylGroupData* owner;
  Vectors<Rational> waypoints;
  void makeFromWeightInSimpleCoords(const Vector<Rational>& weightInSimpleCoords, WeylGroupData& theOwner);
  void makeFromWaypoints(Vectors<Rational>& weightsInSimpleCoords, WeylGroupData& theOwner) {
    this->owner = &theOwner;
    this->waypoints = weightsInSimpleCoords;
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
  bool isAdaptedString(MonomialTensor<int, HashFunctions::hashFunction>& theString);
  std::string toStringIndicesToCalculatorOutput(LittelmannPath& inputStartingPath, List<int>& input);
  std::string toStringOperatorSequenceStartingOnMe(List<int>& input);
  bool generateOrbit(
    List<LittelmannPath>& output,
    List<List<int> >& outputOperators,
    int UpperBoundNumElts,
    Selection* parabolicNonSelectedAreInLeviPart = nullptr
  );
  std::string generateOrbitAndAnimate();
  bool minimaAreIntegral();
  std::string toString(bool useSimpleCoords = true, bool useArrows = true, bool includeDominance = false) const;
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

class ConeLatticeAndShiftMaxComputation {
public:
  int numNonParaM;
  int numProcessedNonParam;
  List<ConeComplex> complexStartingPerRepresentative;
  List<ConeComplex> complexRefinedPerRepresentative;
  List<List<Vectors<Rational> > > theMaximaCandidates;
  List<Vectors<Rational> > startingLPtoMaximize;
  List<Vectors<Rational> > finalMaxima;
  Lattice theStartingLattice;
  Lattice theFinalRougherLattice;
  Vector<Rational> theStartingRepresentative;
  Vectors<Rational> theFinalRepresentatives;
  List<ConeLatticeAndShift> theConesLargerDim;
  List<ConeLatticeAndShift> theConesSmallerDim;
  List<List<ConeComplex> > finalMaximaChambers;
  List<List<List<int> > > finalMaximaChambersIndicesMaxFunctions;
  List<bool> isInfinity;
  Vectors<Rational> LPtoMaximizeLargerDim;
  Vectors<Rational> LPtoMaximizeSmallerDim;

  std::string toString(FormatExpressions* format = nullptr);
  void initialize(Vector<Rational>& theNEq, Cone& startingCone, Lattice& startingLattice, Vector<Rational>& startingShift);
  void findExtremaParametricStep1(PauseThread& thePauseController);
  void findExtremaParametricStep2TrimChamberForMultOne(PauseThread& thePauseController);
  void findExtremaParametricStep3();
  void findExtremaParametricStep4();
  void findExtremaParametricStep5();
};

class PiecewiseQuasipolynomial {
  public:
  // Let the piecewise quasipolynomial be in n variables.
  // Then the theProjectivizedComplex is an n + 1-dimensional complex,
  // which is the projectivization of the n-dim affine complex representing the
  // domain of the piecewise quasipoly.
  ConeComplex theProjectivizedComplex;
  List<QuasiPolynomial> theQPs;
  int NumVariables;
  std::string toString(bool useLatex, bool useHtml);
  void drawMe(
    DrawingVariables& theDrawingVars,
    int numLatticePointsPerDim,
    Cone* RestrictingChamber = nullptr,
    Vector<Rational>* distinguishedPoint = nullptr
  );
  int minimalNumberOfVariables() {
    return this->NumVariables;
  }
  void makeCommonRefinement(const PiecewiseQuasipolynomial& other) {
    this->makeCommonRefinement(other.theProjectivizedComplex);
  }
  bool checkConsistency() const {
    return true;
  }
  void makeCommonRefinement(const ConeComplex& other);
  void translateArgument(Vector<Rational>& translateToBeAddedToArgument);
  bool makeVPF(Vectors<Rational>& theRoots, std::string& outputstring);
  Rational evaluate(const Vector<Rational>& thePoint);
  Rational evaluateInputProjectivized(const Vector<Rational>& thePoint);
  void makeZero(int numVars) {
    this->NumVariables = numVars;
    this->theProjectivizedComplex.initialize();
    this->theQPs.size = 0;
  }
  void operator+=(const PiecewiseQuasipolynomial& other);
  void operator*=(const Rational& other);
};

class MonomialMatrix {
  friend std::ostream& operator<<(std::ostream& output, const MonomialMatrix& monomial) {
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
  MonomialMatrix(): vIndex(- 1), dualIndex(- 1), isIdentity(false) {
  }
  MonomialMatrix(int i, int j): vIndex(i), dualIndex(j), isIdentity(false) {
  }
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
    return this->vIndex == other.vIndex && this->dualIndex == other.dualIndex && this->isIdentity == other.isIdentity;
  }
  static unsigned int hashFunction(const MonomialMatrix& input) {
    return static_cast<unsigned int>(input.vIndex) * someRandomPrimes[0] +
    static_cast<unsigned int>(input.dualIndex) * someRandomPrimes[1] + input.isIdentity;
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
      out << "m_{" << this->vIndex + 1 << "}\\otimes " << "m^*_{" << this->dualIndex + 1 << "}";
    } else {
      out << "id";
    }
    return out.str();
  }
  void operator*=(const MonomialMatrix& other) {
    MacroRegisterFunctionWithName("MonomialMatrix::operator*=");
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
    MonomialMatrix theMon;
    theMon.makeIdentitySpecial();
    this->addMonomial(theMon, 1);
  }
  void makeIdentity(int numVars) {
    this->makeZero();
    MonomialMatrix theMon;
    for (int i = 0; i < numVars; i ++) {
      theMon.dualIndex = i;
      theMon.vIndex = i;
      this->addMonomial(theMon, 1);
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
    Matrix<Coefficient> theMat;
    this->getMatrix(theMat, this->getMinimumNumberOfColumnsNumberOfRows());
    return theMat.getDeterminant();
  }
  void directSumWith(const MatrixTensor<Coefficient>& other);
  void raiseToPower(int power);
  void operator*=(const MatrixTensor<Coefficient>& other);
  void operator*=(const Coefficient& other) {
    return this->::LinearCombination<MonomialMatrix, Coefficient>::operator*=(other);
  }
  void getVectorsSparseFromRowsIncludeZeroRows(
    List<VectorSparse<Coefficient> >& output, int MinNumRows = - 1
  );
  bool isIdentity() const {
    int theDim = this->getMinimumNumberOfColumnsNumberOfRows();
    Selection theSel;
    theSel.initialize(theDim);
    for (int i = 0; i < this->size(); i ++) {
      if ((*this)[i].vIndex != (*this)[i].dualIndex) {
        return false;
      }
      if (this->coefficients[i] != 1) {
        return false;
      }
      theSel.addSelectionAppendNewIndex((*this)[i].vIndex);
    }
    return theSel.cardinalitySelection == theDim;
  }
  bool isPositiveDefinite() {
    Matrix<Coefficient> other;
    this->getMatrix(other, this->getMinimumNumberOfColumnsNumberOfRows());
    return other.isPositiveDefinite();
  }
  void operator=(const Matrix<Coefficient>& other) {
    this->makeZero();
    MonomialMatrix theMon;
    for (int i = 0; i < other.numberOfRows; i ++) {
      for (int j = 0; j < other.numberOfColumns; j ++) {
        if (!other.elements[i][j].isEqualToZero()) {
          theMon.dualIndex = j;
          theMon.vIndex = i;
          this->addMonomial(theMon, other.elements[i][j]);
        }
      }
    }
  }
  void assignTensorProduct(const MatrixTensor<Coefficient>& left, const MatrixTensor<Coefficient>& right) {
    //handle lazy programmers:
    if (this == &left || this == &right) {
      MatrixTensor<Coefficient> leftCopy = left;
      MatrixTensor<Coefficient> rightCopy = right;
      this->assignTensorProduct(leftCopy, rightCopy);
      return;
    }
    //The basis of the tensor product vector space MUST be in the SAME order as the one used by Matrix::assignTensorProduct.
    //int leftDomainDim= left.getMinimalNumberOfColumns();
    int rightDomainDim = right.getMinimalNumberOfColumns();
    //int leftRangeDim= left.getMinimalNumberOfRows();
    int rightRangeDim = right.getMinimalNumberOfRows();
    MonomialMatrix tempM;
    this->makeZero();
    Coefficient tempCF;
    for (int i = 0; i < left.size(); i ++) {
      for (int j = 0; j < right.size(); j ++) {
        tempM.dualIndex = left[i].dualIndex * rightDomainDim + right[j].dualIndex;
        tempM.vIndex = left[i].vIndex * rightRangeDim + right[j].vIndex;
        tempCF = left.coefficients[i];
        tempCF *= right.coefficients[j];
        this->addMonomial(tempM, tempCF);
      }
    }
  }
  void substitution(const PolynomialSubstitution<Rational>& theSub) {
    MatrixTensor<Coefficient> thisCopy = *this;
    this->makeZero();
    Coefficient coefficient;
    for (int i = 0; i < thisCopy.size(); i ++) {
      coefficient = thisCopy.coefficients[i];
      coefficient.substitution(theSub, Rational::one());
      this->addMonomial(thisCopy[i], coefficient);
    }
  }
  void transpose() {
    MatrixTensor<Coefficient> output;
    MonomialMatrix theMon;
    output.makeZero();
    for (int i = 0; i < this->size(); i ++) {
      theMon = (*this)[i];
      theMon.transpose();
      output.addMonomial(theMon, this->coefficients[i]);
    }
    *this = output;
  }
  void setNumberOfVariables(int newNumVars) {
    MatrixTensor<Coefficient> thisCopy = *this;
    this->makeZero();
    Coefficient tempCF;
    for (int i = 0; i < thisCopy.size; i ++) {
      tempCF = thisCopy.coefficients[i];
      tempCF.setNumberOfVariables(newNumVars);
      this->addMonomial(thisCopy[i], tempCF);
    }
  }
  void lieBracketOnTheLeft(const MatrixTensor<Coefficient>& standsOnTheLeft) {
    MacroRegisterFunctionWithName("MatrixTensor<Coefficient>::lieBracketOnTheLeft");
    MatrixTensor<Coefficient> output;
    MonomialMatrix theMon;
    output.makeZero();
    output.setExpectedSize(this->size() * standsOnTheLeft.size() * 2);
    Coefficient tempCF;
    for (int i = 0; i < this->size(); i ++) {
      for (int j = 0; j < standsOnTheLeft.size(); j ++) {
        tempCF = this->coefficients[i];
        tempCF *= standsOnTheLeft.coefficients[j];
        theMon = standsOnTheLeft[j];
        theMon *= (*this)[i];
        output.addMonomial(theMon, tempCF);
        tempCF *= - 1;
        theMon = (*this)[i];
        theMon *= standsOnTheLeft[j];
        output.addMonomial(theMon, tempCF);
      }
    }
    *this = output;
  }
  std::string toStringMatrixForm(FormatExpressions* format = nullptr) const {
    if (this->isEqualToZero()) {
      return "(0)";
    }
    Matrix<Coefficient> tempMat;
    this->getMatrix(tempMat, this->getMinimumNumberOfColumnsNumberOfRows());
    return tempMat.toString(format);
  }
  void getMatrix(Matrix<Coefficient>& output, int theDim) const {
    theDim = MathRoutines::maximum(theDim, this->getMinimumNumberOfColumnsNumberOfRows());
    output.initialize(theDim, theDim);
    output.makeZero();
    for (int i = 0; i < this->size(); i ++) {
      if ((*this)[i].isIdentity) {
        for (int j = 0; j < theDim; j ++) {
          output(j, j) += this->coefficients[i];
        }
      } else {
        output((*this)[i].vIndex, (*this)[i].dualIndex) += this->coefficients[i];
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
  void assignVectorsToColumns(const List<VectorSparse<otherType> >& inputVectors) {
    this->makeZero();
    for (int i = 0; i < inputVectors.size; i ++) {
      for (int j = 0; j < inputVectors[i].size(); j ++) {
        this->addMonomial(MonomialMatrix(inputVectors[i][j].theIndex, i), inputVectors[i].coefficients[j]);
      }
    }
  }
  template <class otherType>
  void assignVectorsToRows(const List<VectorSparse<otherType> >& inputVectors) {
    this->makeZero();
    for (int i = 0; i < inputVectors.size; i ++) {
      for (int j = 0; j < inputVectors[i].size(); j ++) {
        this->addMonomial(MonomialMatrix(i, inputVectors[i][j].theIndex), inputVectors[i].coefficients[j]);
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
  void gaussianEliminationByRowsMatrix(MatrixTensor<Coefficient>* carbonCopyMat = 0);
  template <class otherType>
  void actOnVectorColumn(const Vector<otherType>& input, Vector<otherType>& output) const {
    if (&input == &output) {
      Vector<otherType> inputCopy = input;
      this->actOnVectorColumn(inputCopy, output);
      return;
    }
    output.makeZero(this->getMinimalNumberOfRows());
    otherType currentCF;
    for (int i = 0; i < this->size(); i ++) {
      //note that, at the cost of one extra implicit conversion below, we pReserve the order of multiplication:
      //first is matrix element, then vector coordinate. The code should work as-is for non-commutative fields.
      //(think in the generality of quaternion matrix acting on quaternion-coefficient polynomials!)
      currentCF = this->coefficients[i];
      currentCF *= input[(*this)[i].dualIndex];
      output[(*this)[i].vIndex] += currentCF;
    }
  }
  void actOnVectorROWSOnTheLeft(
    const List<Vector<Coefficient> >& inputStandToTheLeftAsVectorRows, List<Vector<Coefficient> >& output
  ) const {
    if (&inputStandToTheLeftAsVectorRows == &output) {
      List<Vector<Coefficient> > inputCopy = inputStandToTheLeftAsVectorRows;
      this->actOnVectorROWSOnTheLeft(inputCopy, output);
      return;
    }
    output.setSize(this->getMinimalNumberOfRows());
    int numColsTarget = inputStandToTheLeftAsVectorRows[0].size;
    if (this->getMinimalNumberOfColumns() != inputStandToTheLeftAsVectorRows.size) {
      global.fatal << "Attemtping to act by matrix "
      << this->toString() << " (" << this->getMinimalNumberOfColumns() << " columns) "
      << " on the " << inputStandToTheLeftAsVectorRows.size << " vector-rows: "
      << inputStandToTheLeftAsVectorRows.toString() << ". "
      << global.fatal;
    }
    for (int i = 0; i < inputStandToTheLeftAsVectorRows.size; i ++) {
      output[i].makeZero(numColsTarget);
    }
    for (int i = 0; i < this->size(); i ++) {
      for (int j = 0; j < numColsTarget; j ++) {
        output[(*this)[i].vIndex][j] += inputStandToTheLeftAsVectorRows[(*this)[i].dualIndex][j] * this->coefficients[i];
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
    otherType tempElt;
    MonomialVector tempVM;
    for (int i = 0; i < this->size(); i ++) {
      for (int j = 0; j < inputOutput.size(); j ++) {
        if ((*this)[i].dualIndex == inputOutput[j].theIndex) {
          tempVM.theIndex = (*this)[i].vIndex;
          tempElt = this->coefficients[i];
          tempElt *= inputOutput.coefficients[j];
          output.addMonomial(tempVM, tempElt);
        }
      }
    }
    inputOutput = output;
  }
  unsigned int hashFunction() const {
    return this->::LinearCombination<MonomialMatrix, Coefficient>::hashFunction();
  }
  static unsigned int hashFunction(const MatrixTensor<Coefficient>& input) {
    return input.hashFunction();
  }
  template<class otherClass>
  void operator=(const otherClass& other) {
    this->::LinearCombination<MonomialMatrix, Coefficient>::operator=(other);
  }
  MatrixTensor<Coefficient> operator+(const MatrixTensor<Coefficient>& rightM) const {
    MatrixTensor output;
    output = *this;
    output += rightM;
    return output;
  }
};

template <class Coefficient>
void MatrixTensor<Coefficient>::getVectorsSparseFromRowsIncludeZeroRows(
  List<VectorSparse<Coefficient> >& output, int MinNumRows
) {
  MinNumRows = MathRoutines::maximum(MinNumRows, this->getMinimalNumberOfRows());
  output.setSize(MinNumRows);
  for (int i = 0; i < output.size; i ++) {
    output[i].makeZero();
  }
  for (int i = 0; i < this->size(); i ++) {
    int rowIndex = (*this)[i].vIndex;
    int colIndex = (*this)[i].dualIndex;
    output[rowIndex].addMonomial(MonomialVector(colIndex), this->coefficients[i]);
  }
}

template <class Coefficient>
void MatrixTensor<Coefficient>::gaussianEliminationByRowsMatrix(MatrixTensor<Coefficient>* carbonCopyMat) {
  List<VectorSparse<Coefficient> > theRows, theCarbonCopyRows;
  int numRows = this->getMinimalNumberOfRows();
  if (carbonCopyMat != 0) {
    numRows = MathRoutines::maximum(numRows, carbonCopyMat->getMinimalNumberOfRows());
    carbonCopyMat->getVectorsSparseFromRowsIncludeZeroRows(theCarbonCopyRows, numRows);
  }
  this->getVectorsSparseFromRowsIncludeZeroRows(theRows, numRows);
  List<VectorSparse<Coefficient> >* theCarbonCopyPointer = carbonCopyMat == 0 ? 0 : &theCarbonCopyRows;
  VectorSparse<Coefficient>::gaussianEliminationByRows(theRows, 0, 0, 0, theCarbonCopyPointer);
  this->assignVectorsToRows(theRows);
  if (carbonCopyMat != 0) {
    carbonCopyMat->assignVectorsToRows(theCarbonCopyRows);
  }
}

template <class Coefficient>
void MatrixTensor<Coefficient>::directSumWith(const MatrixTensor<Coefficient>& other) {
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
  MatrixTensor<Coefficient> theId;
  theId.makeIdentity(this->getMinimumNumberOfColumnsNumberOfRows());
  MatrixTensor<Coefficient> result = theId;
  this->gaussianEliminationByRowsMatrix(&result);
  if (*this != theId) {
    global.fatal << "Attempting to invert a "
    << "non-invertable matrix tensor. After Gaussian elimination, the matrix equals "
    << this->toStringMatrixForm() << " but should instead be equal to " << theId.toStringMatrixForm() << global.fatal;
  }
  *this = result;
}

class PolynomialOverModule;

class SlTwoInSlN {
  int getModuleIndexFromHighestWeightVector(const Matrix<Rational>& input) {
    Rational tempRat;
    for (int i = 0; i < this->theHighestWeightVectors.size; i ++) {
      if (this->theHighestWeightVectors.objects[i].isProportionalTo(input, tempRat)) {
        return i;
      }
    }
    return - 1;
  }
public:
  int theDimension;
  Matrix<Rational> theH;
  Matrix<Rational> theE;
  Matrix<Rational> theF;
  List<int> thePartition;
  List<Matrix<Rational> > theProjectors;
  List<Matrix<Rational> > theHighestWeightVectors;
  List<List<Matrix<Rational> > > theGmodKModules;
  List<List<List<int> > > PairingTable;
  void getIsPlusKIndexingFrom(int input, int& s, int& k);
  std::string elementMatrixToTensorString(const Matrix<Rational> & input, bool useHtml);
  std::string initFromModuleDecomposition(List<int>& decompositionDimensions, bool useHtml, bool computePairingTable);
  std::string initPairingTable(bool useHtml);
  std::string ElementModuleIndexToString(int input, bool useHtml);
  std::string GetNotationString(bool useHtml);
  bool computeInvariantsOfDegree(
    List<int>& decompositionDimensions,
    int degree,
    List<Polynomial<Rational> >& output,
    std::string& outputError
  );
  std::string pairTwoIndices(List<int>& output, int leftIndex, int rightIndex, bool useHtml);
  void extractHighestWeightVectorsFromVector(
    Matrix<Rational>& input,
    List<Matrix<Rational> >& outputDecompositionOfInput,
    List<Matrix<Rational> >& outputTheHWVectors
  );
  void climbDownFromHighestWeightAlongSl2String(
    Matrix<Rational>& input, Matrix<Rational>& output, Rational& outputCoeff, int generatorPower
  );
  void climbUpFromVector(
    Matrix<Rational>& input, Matrix<Rational>& outputLastNonZero, int& largestPowerNotKillingInput
  );
};

template<typename Coefficient>
void Matrix<Coefficient>::getVectorFromRow(int rowIndex, Vector<Coefficient>& output) const {
  output.setSize(this->numberOfColumns);
  for (int i = 0; i < this->numberOfColumns; i ++) {
    output.objects[i] = this->elements[rowIndex][i];
  }
}

template<typename Coefficient>
void Matrix<Coefficient>::getVectorFromColumn(int columnIndex, Vector<Coefficient>& output) const {
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
  void kazhdanLuzstigCoefficientsToString(List<int>& kazhdanLuzstigCoefficients, std::string& output);
  void findNextToExplore();
  int findMinimalBruhatNonExplored(List<bool>& explored);
  int findMaximalBruhatNonExplored(List<bool>& explored);
  void initTheMults();
  void compute(int x);
  void check();
  //returns true if computation succeeded, false otherwise
  bool computeRxy(int x, int y, int SimpleReflectionIndex);
  void computeKLxy(int x, int y);
  bool isMaximalNonExplored(int index);
  bool indexGEQIndex(int a, int b);
  bool indexGreaterThanIndex(int a, int b);
  std::string toString(FormatExpressions* format = nullptr);
  void mergeBruhatLists(int fromList, int toList);
  std::string KLPolysToString(FormatExpressions* format = nullptr);
  void computeKLCoefficients();
  int chamberIndicatorToIndex(Vector<Rational>& ChamberIndicator);
  std::string rPolysToString(FormatExpressions* format = nullptr);
  bool computeKLPolys(WeylGroupData* theWeylGroup);
  void computeRPolys();
  int computeProductfromSimpleReflectionsActionList(int x, int y);
  void writeKLCoeffsToFile(std::fstream& output, List<int>& KLcoeff, int topIndex);
  //returns the topIndex of the KL coefficients
  int readKLCoeffsFromFile(std::fstream& input, List<int>& output);
  KazhdanLusztigPolynomials() {
    this->weylGroup = nullptr;
  }
  void generatePartialBruhatOrder();
  void initFromWeyl(WeylGroupData* theWeylGroup);
};


template<class Coefficient>
void Matrix<Coefficient>::substitution(const PolynomialSubstitution<Rational>& substitution) {
  for (int i = 0; i < this->numberOfRows; i ++) {
    for (int j = 0; j < this->numberOfColumns; j ++) {
      this->elements[i][j].substitution(substitution, 1);
    }
  }
}

template <class TemplateMonomial, class Coefficient>
std::ostream& operator<<(std::ostream& output, const LinearCombination<TemplateMonomial, Coefficient>& collection) {
  if (collection.size() == 0) {
    output << "0";
    return output;
  }
  std::string tempS1, tempS2;
  List<TemplateMonomial> sortedMons;
  sortedMons = collection.monomials;
  sortedMons.quickSortDescending();
  int cutOffCounter = 0;
  for (int i = 0; i < sortedMons.size; i ++) {
    TemplateMonomial& currentMon = sortedMons[i];
    std::stringstream tempStream;
    Coefficient& currentCoeff = collection.coefficients[collection.monomials.getIndex(currentMon)];
    tempStream << currentCoeff;
    tempS1 = tempStream.str();
    tempS2 = currentMon.toString();
    if (tempS1 == "1" && tempS2 != "1") {
      tempS1 = "";
    }
    if ((tempS1 == "- 1" || tempS1 == "-1") && tempS2 != "1") {
      tempS1 = "-";
    }
    if (tempS2 != "1") {
      tempS1 += tempS2;
    }
    if (i > 0) {
      if (tempS1.size() > 0) {
        if (tempS1[0] != '-') {
          output << "+";
          cutOffCounter += 1;
        }
      } else {
        output << "+";
        cutOffCounter += 1;
      }
    }
    output << tempS1;
    cutOffCounter += tempS1.size();
  }
  return output;
}

template <class Coefficient>
void PolynomialSubstitution<Coefficient>::makeLinearSubstitutionConstantTermsLastRow(
  Matrix<Coefficient>& matrix
) {
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

template<class Coefficient>
void Vectors<Coefficient>::intersectTwoLinearSpaces(
  const List<Vector<Coefficient> >& firstSpace,
  const List<Vector<Coefficient> >& secondSpace,
  List<Vector<Coefficient> >& output
) {
  Vectors<Coefficient> firstReduced, secondReduced;
  Selection tempSel;
  Vectors<Coefficient>::selectBasisInSubspace(firstSpace, firstReduced, tempSel);
  Vectors<Coefficient>::selectBasisInSubspace(secondSpace, secondReduced, tempSel);
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
  matrix.gaussianEliminationByRows(0, &tempSel);
  output.reserve(tempSel.cardinalitySelection);
  output.size = 0;
  Vector<Coefficient> nextIntersection;
  for (int i = 0; i < tempSel.cardinalitySelection; i ++) {
    int currentIndex = tempSel.elements[i];
    if (currentIndex < firstReduced.size) {
      global.fatal << "Unexpected condition in Vectors::intersectTwoLinearSpaces. " << global.fatal;
    }
    nextIntersection.makeZero(dimension);
    for (int j = 0; j < firstReduced.size; j ++) {
      if (!tempSel.selected[j]) {
        nextIntersection += firstReduced[j] * matrix.elements[j][currentIndex];
      }
    }
    output.addOnTop(nextIntersection);
  }
}

template<class Coefficient>
bool Matrix<Coefficient>::isPositiveDefinite() {
  if (this->numberOfRows != this->numberOfColumns) {
    global.fatal << "Attempting to evaluate whether a matrix "
    << "is positive definite, but the matrix is not square. " << global.fatal;
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
bool ElementSemisimpleLieAlgebra<Coefficient>::getCoordinatesInBasis(
  const List<ElementSemisimpleLieAlgebra>& basis, Vector<Coefficient>& output
) const {
  if (basis.size == 0) {
    return false;
  }
  if (this->isEqualToZero()) {
    output.makeZero(basis.size);
    return true;
  }
  MacroRegisterFunctionWithName("ElementSemisimpleLieAlgebra::getCoordinatesInBasis");
  Vectors<Coefficient> basisVector;
  Vector<Coefficient> element;
  basisVector.setSize(basis.size);
  for (int i = 0 ; i < basis.size; i ++) {
    basis[i].elementToVectorNegativeRootSpacesFirst(basisVector[i]);
  }
  this->elementToVectorNegativeRootSpacesFirst(element);
  return element.getCoordinatesInBasis(basisVector, output);
}

template <class Coefficient>
void ElementSemisimpleLieAlgebra<Coefficient>::makeGenerator(int generatorIndex, SemisimpleLieAlgebra& inputOwner) {
  this->makeZero();
  ChevalleyGenerator tempGenerator;
  tempGenerator.makeGenerator(inputOwner, generatorIndex);
  this->addMonomial(tempGenerator, 1);
}

template <class Coefficient>
std::string Vectors<Coefficient>::toInequalitiesString(
  bool useLatex, bool useHtml, bool lastVariableIsConstant, FormatExpressions& format
) const {
  std::stringstream out;
  std::string tempS;
  if (useLatex) {
    out << "\\begin{array}{l}";
  }
  for (int i = 0; i < this->size; i ++) {
    Vector<Rational>& current = (*this)[i];
    tempS = current.toStringLetterFormat(format.polyDefaultLetter, &format, lastVariableIsConstant);
    if (tempS == "") {
      out << "(0";
    }
    out << tempS;
    if (!lastVariableIsConstant) {
      if (useLatex) {
        out << "\\geq 0\\\\";
      } else {
        out << "=>0\n";
      }
    } else {
      if (useLatex) {
        out << "\\geq " << (- (*current.lastObject())).toString() << "\\\\";
      } else {
        out << "=>" <<  (- (*current.lastObject())).toString();
      }
    }
    if (tempS == "") {
      out << ")";
    }
    if (useHtml) {
      out << "<br>";
    } else {
      out << "\n";
    }
  }
  if (useLatex) {
    out << "\\end{array}";
  }
  return out.str();
}
#endif
