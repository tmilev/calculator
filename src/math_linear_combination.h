#ifndef header_math_linear_combination_ALREADY_INCLUDED
#define header_math_linear_combination_ALREADY_INCLUDED

#include "general_logging_global_variables.h"
#include "general_strings.h"
#include "math_basics.h"
#include "math_general.h"
#include "math_large_integers.h"
#include "math_mathml.h"
#include "math_subsets_selections.h"
#include "math_vectors.h"
#include "progress_report.h"

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
  std::string termToString(
    const Coefficient& coefficient,
    const TemplateMonomial& monomial,
    const FormatExpressions* format,
    const std::string& customCoefficientMonomialSeparator
  ) const;
  void termToMathML(
    const Coefficient& coefficient,
    const TemplateMonomial& monomial,
    const FormatExpressions* format,
    const std::string& customCoefficientMonomialSeparator,
    List<std::string>& rowOutputs,
    MathExpressionFormattingProperties& outputProperties
  ) const;
  friend LinearCombination<TemplateMonomial, Coefficient> operator*(
    const Coefficient& left,
    const LinearCombination<TemplateMonomial, Coefficient>& right
  ) {
    LinearCombination<TemplateMonomial, Coefficient> result;
    Coefficient productCoefficient;
    for (int i = 0; i < right.monomials.size(); i ++) {
      productCoefficient = left;
      productCoefficient *= right.coefficients[i];
      result.addMonomial(right.monomials[i], productCoefficient);
    }
    return result;
  }
public:
  HashedList<TemplateMonomial> monomials;
  List<Coefficient> coefficients;
  bool flagDeallocated;
  LinearCombination() {
    this->flagDeallocated = false;
  }
  LinearCombination(const LinearCombination& other) {
    this->flagDeallocated = false;
    this->monomials = other.monomials;
    this->coefficients = other.coefficients;
  }
  void operator=(const LinearCombination& other) {
    this->monomials = other.monomials;
    this->coefficients = other.coefficients;
  }
  ~LinearCombination() {
    this->flagDeallocated = true;
  }
  void computeFormattingProperties(
    const FormatExpressions* format = nullptr,
    MathExpressionFormattingProperties* outputProperties = nullptr
  ) const {
    if (outputProperties == nullptr) {
      return;
    }
    outputProperties->needsParenthesesForMultiplicationOnTheRight =
    this->needsParenthesisForMultiplication(format);
  }
  bool needsParenthesisForMultiplication(
    const FormatExpressions* unused = nullptr
  ) const {
    (void) unused;
    return this->size() > 1;
  }
  // Same as toString but, if the polynomial is too long and a line break is
  // needed,
  // it will be enclosed in a \begin{array} ... \end{array} pair.
  std::string toStringWithPossibleLineBreak(
    const FormatExpressions* format = nullptr,
    int* outputNumberOfLines = nullptr
  ) const;
  std::string toString(
    const FormatExpressions* format = nullptr,
    int* outputNumberOfLines = nullptr
  ) const;
  std::string toStringIfShort(const FormatExpressions* format = nullptr) const;
  // Same as toString but uses a default alphabet "x", "y", ...
  // for the first few variables.
  std::string toStringPretty() const {
    FormatExpressions format;
    format.makeAlphabetXYZUW();
    return this->toString(&format);
  }
  std::string toMathML(
    const FormatExpressions* format,
    MathExpressionFormattingProperties* outputProperties = nullptr
  ) const;
  std::string toMathMLFinal(FormatExpressions* format) const;
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
    const LinearCombination<TemplateMonomial, Coefficient>& other,
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
  template <typename OtherType>
  LinearCombination<TemplateMonomial, Coefficient> operator*(
    const OtherType& other
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
  LinearCombination<TemplateMonomial, Coefficient> operator-() const {
    LinearCombination<TemplateMonomial, Coefficient> output;
    output.makeZero();
    output -= *this;
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
  void assignVector(const Vector<Coefficient>& other);
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
  template <typename OtherType>
  ElementMonomialAlgebra<TemplateMonomial, Coefficient> operator*(
    const OtherType& other
  ) const {
    ElementMonomialAlgebra<TemplateMonomial, Coefficient> result = *this;
    result *= other;
    return result;
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

template <class TemplateMonomial, class Coefficient>
void ElementMonomialAlgebra<TemplateMonomial, Coefficient>::raiseToPower(
  int d,
  ElementMonomialAlgebra<TemplateMonomial, Coefficient>& output,
  const Coefficient& ringUnit
) {
  ElementMonomialAlgebra<TemplateMonomial, Coefficient> monomialOne;
  monomialOne.makeConstant(ringUnit);
  output = *this;
  MathRoutines::raiseToPower(output, d, monomialOne);
}

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
  const LinearCombination<TemplateMonomial, Coefficient>& other,
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
    listCopy[i] *= outputFirstLinearCombination[i];
    remainderFromInput -= listCopy[i];
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
    int i = 0; i < allMonomials.size &&
    currentRowIndex < toBeEliminated.size; i ++
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
template <class LinearCombinationTemplate>
void LinearCombination<TemplateMonomial, Coefficient>::intersectVectorSpaces(
  const List<LinearCombinationTemplate>& vectorSpace1,
  const List<LinearCombinationTemplate>& vectorSpace2,
  List<LinearCombinationTemplate>& outputIntersection,
  HashedList<TemplateMonomial>* seedMonomials
) {
  STACK_TRACE("LinearCombination::intersectVectorSpaces");
  List<LinearCombinationTemplate> workingSpace = vectorSpace1;
  List<LinearCombinationTemplate> vectorSpace2Eliminated = vectorSpace2;
  LinearCombination<TemplateMonomial, Coefficient>::
  gaussianEliminationByRowsDeleteZeroRows(
    vectorSpace2Eliminated, nullptr, seedMonomials
  );
  LinearCombination<TemplateMonomial, Coefficient>::
  gaussianEliminationByRowsDeleteZeroRows(
    workingSpace, nullptr, seedMonomials
  );
  Matrix<Coefficient> linearCombinationMatrix;
  int dimensionFirstSpace = workingSpace.size;
  linearCombinationMatrix.makeIdentityMatrix(
    workingSpace.size + vectorSpace2Eliminated.size
  );
  workingSpace.addListOnTop(vectorSpace2Eliminated);
  vectorSpace2Eliminated = workingSpace;
  LinearCombination<TemplateMonomial, Coefficient>::gaussianEliminationByRows(
    workingSpace, nullptr, seedMonomials, &linearCombinationMatrix
  );
  int resultDimension = 0;
  for (int i = workingSpace.size - 1; i >= 0; i --) {
    if (workingSpace[i].isEqualToZero()) {
      resultDimension ++;
    } else {
      break;
    }
  }
  outputIntersection.setSize(resultDimension);
  int counter = - 1;
  LinearCombinationTemplate current;
  for (int i = workingSpace.size - 1; i >= 0; i --) {
    if (!workingSpace[i].isEqualToZero()) {
      break;
    }
    counter ++;
    outputIntersection[counter].makeZero();
    for (int j = 0; j < dimensionFirstSpace; j ++) {
      current = vectorSpace2Eliminated[j];
      current *= linearCombinationMatrix(i, j);
      outputIntersection[counter] += current;
    }
  }
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
std::string LinearCombination<TemplateMonomial, Coefficient>::termToString(
  const Coefficient& coefficient,
  const TemplateMonomial& monomial,
  const FormatExpressions* format,
  const std::string& customCoefficientMonomialSeparator
) const {
  std::string coefficientString;
  MathExpressionFormattingProperties coefficientProperties;
  coefficient.computeFormattingProperties(format, &coefficientProperties);
  std::string monomialString;
  if (coefficientProperties.needsParenthesesForMultiplicationOnTheRight) {
    coefficientString = "\\left(" + coefficient.toString(format) + "\\right)";
  } else {
    coefficientString = coefficient.toString(format);
  }
  monomialString = monomial.toString(format);
  if (monomialString == "") {
    return coefficientString;
  }
  if (
    format != nullptr &&
    format->flagUseFrac &&
    format->flagSuppressOneIn1overXtimesY
  ) {
    int sign = 0;
    std::string denominatorString;
    if (
      FormatExpressions::toStringIsFractionLikeWithUnitNumerator(
        coefficient, format, sign, denominatorString
      )
    ) {
      if (sign == 1) {
        return "\\frac{" + monomialString + "}{" + denominatorString + "}";
      } else {
        return "-\\frac{" + monomialString + "}{" + denominatorString + "}";
      }
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
void LinearCombination<TemplateMonomial, Coefficient>::termToMathML(
  const Coefficient& coefficient,
  const TemplateMonomial& monomial,
  const FormatExpressions* format,
  const std::string& customCoefficientMonomialSeparator,
  List<std::string>& rowOutputs,
  MathExpressionFormattingProperties& outputProperties
) const {
  STACK_TRACE("LinearCombination::termToMathML");
  MathExpressionFormattingProperties coefficientProperites;
  MathExpressionFormattingProperties monomialProperties;
  rowOutputs.clear();
  std::string coefficientMathML =
  coefficient.toMathML(format, &coefficientProperites);
  if (coefficientProperites.needsParenthesesForMultiplicationOnTheRight) {
    coefficientMathML =
    MathML::leftParenthesis + coefficientMathML + MathML::rightParenthesis;
  }
  std::string monomialMathML = monomial.toMathML(format, &monomialProperties);
  if (customCoefficientMonomialSeparator != "") {
    rowOutputs.addOnTop(coefficientMathML);
    rowOutputs.addOnTop(customCoefficientMonomialSeparator);
    rowOutputs.addOnTop(monomialMathML);
    return;
  }
  if (
    format != nullptr &&
    format->flagUseFrac &&
    format->flagSuppressOneIn1overXtimesY
  ) {
    int sign = 0;
    std::string denominatorString;
    if (
      FormatExpressions::toMathMLIsFractionLikeWithUnitNumerator(
        coefficient, format, sign, denominatorString
      )
    ) {
      std::string element =
      "<mfrac>" + monomialMathML + denominatorString + "</mfrac>";
      if (sign != 1) {
        rowOutputs.addOnTop(MathML::negativeSign);
        outputProperties.startsWithMinus = true;
      }
      rowOutputs.addOnTop(element);
      return;
    }
  }
  if (monomialProperties.isOne) {
    rowOutputs.addOnTop(coefficientMathML);
    outputProperties = coefficientProperites;
    return;
  }
  if (coefficientProperites.isOne) {
    rowOutputs.addOnTop(monomialMathML);
    outputProperties = monomialProperties;
    return;
  }
  if (coefficientProperites.isNegativeOne) {
    rowOutputs.addOnTop(MathML::negativeSign);
    rowOutputs.addOnTop(monomialMathML);
    outputProperties.startsWithMinus = true;
    if (monomialProperties.isOne) {
      outputProperties.isNegativeOne = true;
    }
    return;
  }
  rowOutputs.addOnTop(coefficientMathML);
  rowOutputs.addOnTop(monomialMathML);
  outputProperties.startsWithMinus = coefficientProperites.startsWithMinus;
}

template <class TemplateMonomial, class Coefficient>
std::string LinearCombination<TemplateMonomial, Coefficient>::
toStringWithPossibleLineBreak(
  const FormatExpressions* format, int* outputNumberOfLines
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
std::string LinearCombination<TemplateMonomial, Coefficient>::toMathMLFinal(
  FormatExpressions* format
) const {
  std::string latex = this->toString(format);
  std::string mathML = this->toMathML(format);
  return MathML::toMathMLFinal(mathML, latex, format);
}

template <class TemplateMonomial, class Coefficient>
std::string LinearCombination<TemplateMonomial, Coefficient>::toMathML(
  const FormatExpressions* format,
  MathExpressionFormattingProperties* outputProperties
) const {
  STACK_TRACE("LinearCombination::toMathML");
  if (this->size() == 0) {
    return "<mn>0</mn>";
  }
  std::stringstream out;
  List<TemplateMonomial> sortedMonomials;
  sortedMonomials = this->monomials;
  // If this line fails to link, you must do the following.
  // You need to implement
  // FormatExpressions::getMonomialOrder<TemplateMonomial>()
  // and make it return 0 (or a pointer to a monomial order, should you
  // wish to use a custom one.
  const typename List<TemplateMonomial>::Comparator* monomialOrder = (
    format == nullptr
  ) ?
  nullptr :
  format->getMonomialOrder<TemplateMonomial>();
  sortedMonomials.quickSortDescending(monomialOrder);
  bool useCustomPlus = false;
  std::string customTimes = "";
  MemorySaving<FormatExpressions> formatHolder;
  FormatExpressions* formatForTerm = format ==
  nullptr ? nullptr : &formatHolder.getElement();
  if (format != nullptr) {
    *formatForTerm = *format;
    formatForTerm->customCoefficientMonomialSeparator = "";
    formatForTerm->flagCanOmitHorizontalRowWrapper = true;
    useCustomPlus = (format->customPlusSign.mathML != "");
  }
  out << "<mrow>";
  for (int i = 0; i < sortedMonomials.size; i ++) {
    TemplateMonomial& monomial = sortedMonomials[i];
    int coefficientIndex = this->monomials.getIndex(monomial);
    Coefficient& coefficient = this->coefficients[coefficientIndex];
    List<std::string> termStrings;
    MathExpressionFormattingProperties termProperties;
    this->termToMathML(
      coefficient,
      monomial,
      formatForTerm,
      customTimes,
      termStrings,
      termProperties
    );
    if (i > 0) {
      if (useCustomPlus) {
        out << format->customPlusSign.mathML;
      } else if (!termProperties.startsWithMinus) {
        out << "<mo>+</mo>";
      }
    }
    for (const std::string& termString : termStrings) {
      out << termString;
    }
    if (i == 0 && outputProperties != nullptr && this->size() == 1) {
      *outputProperties = termProperties;
    }
  }
  out << "</mrow>";
  if (outputProperties != nullptr && this->size() > 1) {
    outputProperties->needsParenthesesForMultiplicationOnTheRight = true;
    outputProperties->needsParenthesesWhenLastAndMultipliedOnTheLeft = true;
  }
  return out.str();
}

template <class TemplateMonomial, class Coefficient>
std::string LinearCombination<TemplateMonomial, Coefficient>::toStringIfShort(
  const FormatExpressions* format
) const {
  STACK_TRACE("LinearCombination::toStringIfShort");
  if (this->size() < 25) {
    return this->toString(format);
  }
  std::stringstream out;
  TemplateMonomial leadingMonomial;
  Coefficient leadingCoefficient;
  this->getIndexLeadingMonomial(&leadingMonomial, &leadingCoefficient);
  out
  << this->termToString(leadingCoefficient, leadingMonomial, format, "")
  << "+ ... [large linear combination with "
  << this->size()
  << " monomials]. ";
  return out.str();
}

template <class TemplateMonomial, class Coefficient>
std::string LinearCombination<TemplateMonomial, Coefficient>::toString(
  const FormatExpressions* format, int* outputNumberOfLines
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
  const typename List<TemplateMonomial>::Comparator* monomialOrder = (
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
  bool isInNumerator = (format == nullptr) ? false : format->flagIsInNumerator;
  std::string customTimes = "";
  MemorySaving<FormatExpressions> formatHolder;
  FormatExpressions* formatForTerms = format ==
  nullptr ? nullptr : &formatHolder.getElement();
  if (format != nullptr) {
    useCustomPlus = (format->customPlusSign.latex != "");
    *formatForTerms = *format;
    customTimes = format->customCoefficientMonomialSeparator;
    if (
      format->flagPassCustomCoefficientMonomialSeparatorToCoefficients == false
    ) {
      formatForTerms->customCoefficientMonomialSeparator = "";
    }
  }
  for (int i = 0; i < sortedMonomials.size; i ++) {
    TemplateMonomial& monomial = sortedMonomials[i];
    int coefficientIndex = this->monomials.getIndex(monomial);
    Coefficient& coefficient = this->coefficients[coefficientIndex];
    std::string termString =
    this->termToString(coefficient, monomial, formatForTerms, customTimes);
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
        out << format->customPlusSign.latex;
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
      }
    }
  }
  return out.str();
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
    STACK_TRACE("MatrixTensor::lieBracketOnTheLeft");
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
  std::string toStringMatrixForm(const FormatExpressions* format = nullptr)
  const {
    if (this->isEqualToZero()) {
      return "(0)";
    }
    Matrix<Coefficient> matrix;
    this->getMatrix(matrix, this->getMinimumNumberOfColumnsNumberOfRows());
    return matrix.toString(format);
  }
  std::string toMathMLMatrixForm(
    const FormatExpressions* format = nullptr,
    MathExpressionFormattingProperties* outputProperties = nullptr
  ) const {
    if (this->isEqualToZero()) {
      return "<mrow><mo>(</mo><mn>0</mn><mo>)</mo></mrow>";
    }
    Matrix<Coefficient> matrix;
    this->getMatrix(matrix, this->getMinimumNumberOfColumnsNumberOfRows());
    return matrix.toMathML(format, outputProperties);
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
  void assignVectorsToRows(
    const List<VectorSparse<otherType> >& inputVectors
  ) {
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
  template <class OtherType>
  void actOnVectorColumn(
    const Vector<OtherType>& input, Vector<OtherType>& output
  ) const;
  template <typename BuiltInType>
  void actOnVectorColumnBuiltIn(
    const Vector<BuiltInType>& input, Vector<BuiltInType>& output
  ) const {
    if (&input == &output) {
      Vector<BuiltInType> inputCopy = input;
      this->actOnVectorColumn(inputCopy, output);
      return;
    }
    output.makeZero(this->getMinimalNumberOfRows());
    BuiltInType currentCoefficient;
    for (int i = 0; i < this->size(); i ++) {
      // Note that, at the cost of one extra implicit conversion below, we
      // preserve the order of multiplication:
      // first is matrix element, then vector coordinate.
      // The code should work
      // as-is for non-commutative fields, such as
      // quaternion matrix acting on
      // quaternion-coefficient polynomials.
      this->coefficients[i].toBuiltInType(currentCoefficient);
      currentCoefficient *= input[(*this)[i].dualIndex];
      output[(*this)[i].vIndex] += currentCoefficient;
    }
  }
  void actOnVectorRowsOnTheLeft(
    const List<Vector<Coefficient> >& inputStandToTheLeftAsVectorRows,
    List<Vector<Coefficient> >& output
  ) const {
    if (&inputStandToTheLeftAsVectorRows == &output) {
      List<Vector<Coefficient> > inputCopy = inputStandToTheLeftAsVectorRows;
      this->actOnVectorRowsOnTheLeft(inputCopy, output);
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

template < >
template < >
void MatrixTensor<Rational>::actOnVectorColumn(
  const Vector<char>& input, Vector<char>& output
) const;

template <class Coefficient>
template <class OtherType>
void MatrixTensor<Coefficient>::actOnVectorColumn(
  const Vector<OtherType>& input, Vector<OtherType>& output
) const {
  if (&input == &output) {
    Vector<OtherType> inputCopy = input;
    this->actOnVectorColumn(inputCopy, output);
    return;
  }
  output.makeZero(this->getMinimalNumberOfRows());
  OtherType currentCoefficient;
  for (int i = 0; i < this->size(); i ++) {
    // Note that, at the cost of one extra implicit conversion below, we
    // preserve the order of multiplication:
    // first is matrix element, then vector coordinate.
    // The code should work
    // as-is for non-commutative fields, such as
    // quaternion matrix acting on
    // quaternion-coefficient polynomials.
    currentCoefficient = this->coefficients[i];
    currentCoefficient *= input[(*this)[i].dualIndex];
    output[(*this)[i].vIndex] += currentCoefficient;
  }
}

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

template <class TemplateMonomial, class Coefficient>
void ElementMonomialAlgebra<TemplateMonomial, Coefficient>::multiplyBy(
  const ElementMonomialAlgebra<TemplateMonomial, Coefficient>& other,
  ElementMonomialAlgebra<TemplateMonomial, Coefficient>& output,
  ElementMonomialAlgebra<TemplateMonomial, Coefficient>& bufferPolynomial,
  TemplateMonomial& bufferMonomial
) const {
  STACK_TRACE("ElementMonomialAlgebra::multiplyBy");
  if (other.isEqualToZero()) {
    output.makeZero();
    return;
  }
  if (&bufferPolynomial == this || &bufferPolynomial == &other) {
    global.fatal
    << "Bad buffer in ElementMonomialAlgebra::multiplyBy."
    << global.fatal;
  }
  this->checkConsistency();
  other.checkConsistency();
  int maximumMonomials = this->size() * other.size();
  if (maximumMonomials > 2000000) {
    maximumMonomials = 2000000;
  }
  int totalMonomialPairs = 0;
  ProgressReport report1;
  ProgressReport report2(400);
  if (report1.tickAndWantReport() && maximumMonomials > 400) {
    totalMonomialPairs = other.size() * this->size();
    std::stringstream reportStream;
    reportStream
    << "Large polynomial computation: "
    << this->size()
    << " times "
    << other.size()
    << "="
    << totalMonomialPairs
    << " monomials:\n<br>\n"
    << this->toString()
    << " times "
    << other.toString();
    report1.report(reportStream.str());
  }
  bufferPolynomial.makeZero();
  bufferPolynomial.setExpectedSize(maximumMonomials);
  bufferPolynomial.checkConsistency();
  bufferMonomial.checkConsistency();
  Coefficient current;
  for (int i = 0; i < other.size(); i ++) {
    for (int j = 0; j < this->size(); j ++) {
      if (report2.tickAndWantReport()) {
        std::stringstream reportStream2;
        reportStream2
        << "Multiplying monomials: "
        << i + 1
        << " out of "
        << other.size()
        << " by "
        << j + 1
        << " out of "
        << this->size()
        << ". ";
        report2.report(reportStream2.str());
      }
      bufferMonomial = (*this)[j];
      bufferMonomial *= other[i];
      current = this->coefficients[j];
      current *= other.coefficients[i];
      bufferPolynomial.addMonomial(bufferMonomial, current);
    }
  }
  output = bufferPolynomial;
}

template <class TemplateMonomial, class Coefficient>
void ElementMonomialAlgebra<TemplateMonomial, Coefficient>::multiplyBy(
  const TemplateMonomial& other,
  ElementMonomialAlgebra<TemplateMonomial, Coefficient>& output
) const {
  if (this == &output) {
    ElementMonomialAlgebra<TemplateMonomial, Coefficient> thisCopy = *this;
    thisCopy.multiplyBy(other, output);
    return;
  }
  output.makeZero();
  TemplateMonomial monomial;
  for (int i = 0; i < this->size(); i ++) {
    monomial = this->monomials[i];
    monomial *= other;
    output.addMonomial(monomial, this->coefficients[i]);
  }
}

template <class Coefficient>
void MatrixTensor<Coefficient>::raiseToPower(int power) {
  if (power <= 0) {
    global.fatal
    << "MatrixTensor::raiseToPower is currently "
    << "implemented for positive integer power only. "
    << global.fatal;
  }
  MatrixTensor<Coefficient> id;
  id.makeIdentitySpecial();
  MathRoutines::raiseToPower(*this, power, id);
}

template <class Coefficient>
void MatrixTensor<Coefficient>::operator*=(
  const MatrixTensor<Coefficient>& other
) {
  STACK_TRACE("MatrixTensor::operator*=");
  if (other.isEqualToZero()) {
    this->makeZero();
    return;
  }
  this->checkConsistency();
  other.checkConsistency();
  int maximumMonomialsFinal = this->size() * other.size();
  if (maximumMonomialsFinal > 2000000) {
    maximumMonomialsFinal = 2000000;
  }
  int totalMonomialPairs = 0;
  ProgressReport report1;
  ProgressReport report2(400);
  if (report1.tickAndWantReport()) {
    totalMonomialPairs = other.size() * this->size();
    std::stringstream reportStream;
    reportStream
    << "Large matrix monomial computation: "
    << this->size()
    << " x "
    << other.size()
    << "="
    << totalMonomialPairs
    << " monomials:\n<br>\n"
    << this->toString()
    << " times "
    << other.toString();
    report1.report(reportStream.str());
  }
  MatrixTensor<Coefficient> result;
  result.setExpectedSize(maximumMonomialsFinal);
  Coefficient currentCoefficient;
  MonomialMatrix monomial;
  for (int i = 0; i < other.size(); i ++) {
    for (int j = 0; j < this->size(); j ++) {
      if (report2.tickAndWantReport()) {
        std::stringstream reportStream2;
        reportStream2
        << "Multiplying monomials: "
        << i + 1
        << " out of "
        << other.size()
        << " by "
        << j + 1
        << " out of "
        << this->size()
        << ". ";
        report2.report(reportStream2.str());
      }
      monomial = (*this)[j];
      monomial *= other[i];
      if (monomial.isZeroMonomial()) {
        continue;
      }
      currentCoefficient = this->coefficients[j];
      currentCoefficient *= other.coefficients[i];
      result.addMonomial(monomial, currentCoefficient);
    }
  }
  *this = result;
}

template <class Coefficient>
void VectorSparse<Coefficient>::assignVector(
  const Vector<Coefficient>& other
) {
  this->makeZero();
  for (int i = 0; i < other.size; i ++) {
    VectorSparse<Coefficient> monomial;
    monomial.makeEi(i, other[i]);
    *this += monomial;
  }
}

template <class Coefficient>
void Vector<Coefficient>::makeVectorSpaceBasisFormat(
  const MathMLAndLatex& inputLetter, FormatExpressions& output
) const {
  output.vectorSpaceEiBasisNames.clear();
  for (int i = 0; i < this->size; i ++) {
    std::stringstream currentMathML;
    std::stringstream currentLaTeX;
    currentMathML
    << "<msub>"
    << inputLetter.mathML
    << "<mn>"
    << i + 1
    << "</mn></msub>";
    currentLaTeX << inputLetter.latex << "_{" << i + 1 << "}";
    output.vectorSpaceEiBasisNames.addOnTop(
      MathMLAndLatex(currentLaTeX.str(), currentMathML.str())
    );
  }
}

template <class Coefficient>
std::string Vector<Coefficient>::toMathMLLetterFormat(
  const MathMLAndLatex& inputLetter, const FormatExpressions* format
) const {
  VectorSparse<Coefficient> converted;
  converted.assignVector(*this);
  FormatExpressions sparseVectorFormat;
  if (format != nullptr) {
    sparseVectorFormat = *format;
  }
  this->makeVectorSpaceBasisFormat(inputLetter, sparseVectorFormat);
  return converted.toMathML(&sparseVectorFormat);
}

#endif // header_math_linear_combination_ALREADY_INCLUDED
