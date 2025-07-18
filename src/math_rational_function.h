#ifndef header_math_rational_function_ALREADY_INCLUDED
#define header_math_rational_function_ALREADY_INCLUDED

#include "math_general.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "progress_report.h"

template <class Coefficient>
class RationalFraction {
private:
  void addSameTypes(const RationalFraction<Coefficient>& other);
  void addHonestRationalFunction(const RationalFraction<Coefficient>& other);
  void reduceRationalFunctionToPolynomial();
  void reducePolynomialToRational();
  bool convertToType(int inputExpressionType);
  static bool greatestCommonDivisorQuick(
    const Polynomial<Coefficient>& left,
    const Polynomial<Coefficient>& right,
    Polynomial<Coefficient>& output
  );
public:
  friend std::ostream& operator<<(
    std::ostream& output, const RationalFraction<Coefficient>& rationalFraction
  ) {
    output << rationalFraction.toString();
    return output;
  }
  MemorySaving<Polynomial<Coefficient> > numerator;
  MemorySaving<Polynomial<Coefficient> > denominator;
  Coefficient constantValue;
  int expressionType;
  enum TypeExpression {
    typeConstant = 0,
    typePolynomial = 1,
    typeRationalFraction = 2,
    typeError = 3
  };
  std::string toString(FormatExpressions* format = nullptr) const;
  bool needsParenthesisForMultiplication(FormatExpressions* unused = nullptr)
  const;
  bool findOneVariableRationalRoots(List<Rational>& output);
  static RationalFraction one();
  static RationalFraction zero();
  static RationalFraction zeroStatic();
  int minimalNumberOfVariables() const;
  bool substitute(
    const PolynomialSubstitution<Rational>& substitution,
    const Coefficient& one
  );
  bool substitute(
    const PolynomialSubstitution<Coefficient>& substitution,
    const Coefficient& one,
    std::stringstream* commentsOnFailure
  );
  RationalFraction(const RationalFraction<Rational>& other);
  RationalFraction();
  RationalFraction(int other);
  RationalFraction(const Coefficient& other);
  Rational rationalValue() const;
  void raiseToPower(int power);
  void reduceMemory();
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const RationalFraction<Coefficient>& input);
  void operator=(int other);
  void operator=(const Coefficient& other);
  void operator=(const RationalFraction<Rational>& other);
  void operator=(const Polynomial<Coefficient>& other);
  bool checkConsistency() const;
  void setNumberOfVariables(int goalNumberOfVariables);
  void setNumberOfVariablesSubstituteDeletedByOne(int newNumberOfVariables);
  void makeOneLetterMonomial(
    int index,
    const Coefficient& coefficient,
    int expectedNumberOfVariables = 0
  );
  void getNumerator(Polynomial<Coefficient>& output) const;
  void getDenominator(Polynomial<Coefficient>& output) const;
  bool isNegative();
  void clearDenominators(RationalFraction<Coefficient>& outputWasMultipliedBy);
  void operator+=(int coefficient);
  void operator+=(const Polynomial<Coefficient>& other);
  void operator+=(const RationalFraction<Coefficient>& other);
  bool operator==(int other) const;
  bool operator!=(int other);
  bool operator==(const RationalFraction<Coefficient>& other) const;
  void simplify();
  void reduceRationalFraction();
  // Scales the numerator and denominator simultaneously
  // so all coefficients are integers,
  // the leading monomial of the denominator has positive coefficient,
  // and either the denominator or the numerator
  // has coefficients that are relatively prime.
  void simplifyLeadingCoefficientOnly();
  // Scales the numerator and denominator so that both
  // have positive leading coefficients, all coefficients are integers,
  // and the denominator and numerator have relatively prime coefficients.
  // Returns the number by which the element was multiplied.
  Rational scaleToIntegral();
  void operator*=(const RationalFraction<Coefficient>& other);
  void operator*=(const Polynomial<Coefficient>& other);
  void operator*=(const MonomialPolynomial& other);
  void operator*=(const Coefficient& other);
  void operator*=(int other);
  bool operator<(const RationalFraction<Rational>& other) const;
  bool operator<=(const RationalFraction<Rational>& other) const;
  bool operator>(const RationalFraction<Rational>& other) const;
  void multiplyByConstant(const Rational& inputConstant);
  void invert();
  void makeOne();
  void makeZero();
  void makeMonomial(
    int letterIndex, const Rational& power, const Rational& coefficient = 1
  );
  void makeConstant(const Coefficient& coefficient) {
    this->makeZero();
    this->constantValue = coefficient;
  }
  bool isPolynomial(Polynomial<Coefficient>* whichPolynomial) const {
    if (
      this->expressionType != TypeExpression::typePolynomial &&
      this->expressionType == TypeExpression::typeConstant
    ) {
      return false;
    }
    if (whichPolynomial != nullptr) {
      this->getNumerator(*whichPolynomial);
    }
    return true;
  }
  bool isConstant(Rational* whichConstant = nullptr) const;
  bool isInteger() const;
  bool isSmallInteger(int* whichInteger = nullptr) const;
  bool isEqualToZero() const {
    return
    this->expressionType == TypeExpression::typeConstant &&
    this->constantValue.isEqualToZero();
  }
  bool isEqualToOne() const {
    return
    this->expressionType == TypeExpression::typeConstant &&
    this->constantValue.isEqualToOne();
  }
  bool isEqualTo(const RationalFraction<Coefficient>& other) const;
  static bool getRelations(
    const List<Polynomial<Rational> >& inputElements,
    List<Polynomial<Rational> >& outputGeneratorLabels,
    List<Polynomial<Rational> >& outputRelations,
    std::stringstream& comments
  );
  static RationalFraction scaleNormalizeIndex(
    List<RationalFraction<Rational> >& input, int indexNonZeroElement
  );
  static void greatestCommonDivisor(
    const Polynomial<Coefficient>& left,
    const Polynomial<Coefficient>& right,
    Polynomial<Coefficient>& output
  );
  static void leastCommonMultiple(
    const Polynomial<Coefficient>& left,
    const Polynomial<Coefficient>& right,
    Polynomial<Coefficient>& output
  );
  void operator-=(int other) {
    *this -= static_cast<Rational>(other);
  }
  void operator-=(const Coefficient& other);
  void operator-=(const RationalFraction<Coefficient>& other);
  void operator/=(int other);
  void operator/=(const Polynomial<Coefficient>& other);
  void operator/=(const RationalFraction<Coefficient>& other);
  void negate();
  static RationalFraction<Rational> oneRational() {
    return RationalFraction(Rational::one());
  }
  static RationalFraction<Rational> zeroRational() {
    return RationalFraction(Rational::zero());
  }
  class Test {
  public:
    static bool all();
    static bool scaleNormalizeIndex();
    static RationalFraction<Rational> fromString(const std::string& input);
    static bool fromStringTest();
  };
};

template < >
bool RationalFraction<Rational>::Test::fromStringTest();
template < >
bool RationalFraction<Rational>::Test::scaleNormalizeIndex();

template <class Coefficient>
bool RationalFraction<Coefficient>::convertToType(int inputExpressionType) {
  STACK_TRACE("RationalFraction::convertToType");
  if (inputExpressionType < this->expressionType) {
    return false;
  }
  if (inputExpressionType == this->expressionType) {
    return true;
  }
  if (
    this->expressionType == TypeExpression::typeConstant &&
    this->expressionType < inputExpressionType
  ) {
    this->expressionType = TypeExpression::typePolynomial;
    this->numerator.getElement().makeConstant(this->constantValue);
  }
  if (
    this->expressionType == TypeExpression::typePolynomial &&
    this->expressionType < inputExpressionType
  ) {
    this->expressionType = TypeExpression::typeRationalFraction;
    this->denominator.getElement().makeConstant(this->constantValue.one());
  }
  return true;
}

template <class Coefficient>
void RationalFraction<Coefficient>::invert() {
  STACK_TRACE("RationalFraction::invert");
  if (!this->checkConsistency()) {
    global.fatal << "Inconsistent rational function. " << global.fatal;
  }
  if (this->expressionType == TypeExpression::typeConstant) {
    if (this->constantValue.isEqualToZero()) {
      global.fatal
      << "Division by zero. "
      << "Division by zero errors must be caught earlier in the program and "
      << "handled gracefully. Crashing ungracefully. "
      << global.fatal;
    }
    this->constantValue.invert();
    return;
  }
  if (this->expressionType == TypeExpression::typePolynomial) {
    this->convertToType(TypeExpression::typeRationalFraction);
  }
  if (this->numerator.getElement().isEqualToZero()) {
    global.fatal
    << "Cannot invert rational function with zero numerator. "
    << global.fatal;
  }
  MathRoutines::swap(
    this->numerator.getElement(), this->denominator.getElement()
  );
  this->expressionType = TypeExpression::typeRationalFraction;
  this->reduceMemory();
  this->simplifyLeadingCoefficientOnly();
  if (!this->checkConsistency()) {
    global.fatal << "Consistency check failed. " << global.fatal;
  }
}

template <class Coefficient>
bool RationalFraction<Coefficient>::checkConsistency() const {
  if (this->expressionType == TypeExpression::typePolynomial) {
    if (this->numerator.isZeroPointer()) {
      global.fatal
      << "A rational function is flagged as being a "
      << "non-constant polynomial, but the numerator pointer is zero. "
      << global.fatal;
      return false;
    }
    if (this->numerator.getElementConst().isConstant()) {
      global.fatal
      << "A rational function is flagged as "
      << "having a non-constant numerator, but the numerator is constant. "
      << global.fatal;
      return false;
    }
  }
  if (this->expressionType == TypeExpression::typeRationalFraction) {
    if (this->numerator.isZeroPointer() || this->denominator.isZeroPointer()) {
      global.fatal
      << "A rational function is flagged as "
      << "having non-constant denominator, but "
      << "either the numerator or the denominator pointer is zero. "
      << global.fatal;
      return false;
    }
    if (this->denominator.getElementConst().isConstant()) {
      global.fatal
      << "A rational function is flagged as "
      << "having non-constant denominator, but the denominator is constant. "
      << global.fatal;
      return false;
    }
  }
  if (
    this->expressionType != TypeExpression::typeConstant &&
    this->expressionType != TypeExpression::typePolynomial &&
    this->expressionType != TypeExpression::typeRationalFraction
  ) {
    global.fatal
    << "A rational function is not initialized properly: its type is "
    << this->expressionType
    << " which is not allowed. "
    << global.fatal;
    return false;
  }
  return true;
}

template <class Coefficient>
void RationalFraction<Coefficient>::operator/=(int other) {
  RationalFraction coefficient;
  coefficient.makeConstant(other);
  *this /= coefficient;
}

template <class Coefficient>
void RationalFraction<Coefficient>::negate() {
  this->operator*=(Rational(- 1));
  if (!this->checkConsistency()) {
    global.fatal
    << "Failed to take the negative sign of a rational function. "
    << global.fatal;
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::operator-=(
  const RationalFraction<Coefficient>& other
) {
  if (!this->checkConsistency()) {
    global.fatal
    << "Corrupt rational function in operator -=. "
    << global.fatal;
  }
  if (!other.checkConsistency()) {
    global.fatal
    << "Corrupt other rational function in operator -=. "
    << global.fatal;
  }
  RationalFraction<Coefficient> negativeCoefficient;
  negativeCoefficient = other;
  negativeCoefficient.negate();
  this->operator+=(negativeCoefficient);
  if (!this->checkConsistency()) {
    global.fatal
    << "Corrupt output in rational function operator -=."
    << global.fatal;
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::operator-=(const Coefficient& other) {
  if (!this->checkConsistency()) {
    global.fatal
    << "Corrupt rational function in operator-=(Rational). "
    << global.fatal;
  }
  RationalFraction negated;
  negated.makeConstant(other);
  negated.negate();
  this->operator+=(negated);
  if (!(this->checkConsistency())) {
    global.fatal
    << "Corrupt output in rational function operator-=(Rational)."
    << global.fatal;
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::makeOne() {
  this->makeConstant(Coefficient::oneStatic());
}

template <class Coefficient>
void RationalFraction<Coefficient>::makeZero() {
  this->expressionType = TypeExpression::typeConstant;
  this->constantValue.makeZero();
  this->numerator.freeMemory();
  this->denominator.freeMemory();
  if (!this->checkConsistency()) {
    global.fatal
    << "makeZero produced corrupt output in rational function old. "
    << global.fatal;
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::operator+=(int coefficient) {
  RationalFraction constantTerm;
  constantTerm.makeConstant(Rational(coefficient));
  (*this) += constantTerm;
}

template <class Coefficient>
Rational RationalFraction<Coefficient>::rationalValue() const {
  switch (this->expressionType) {
  case RationalFraction::typeConstant:
    return this->constantValue;
  case RationalFraction::typeError:
    return 0;
  default:
    return this->numerator.getElementConst().constantTerm(0);
  }
}

template <class Coefficient>
RationalFraction<Coefficient>::RationalFraction() {
  this->expressionType = this->typeError;
}

template <class Coefficient>
RationalFraction<Coefficient>::RationalFraction(const Coefficient& other) {
  this->expressionType = TypeExpression::typeConstant;
  this->operator=(other);
}

template <class Coefficient>
RationalFraction<Coefficient>::RationalFraction(int other) {
  this->expressionType = TypeExpression::typeConstant;
  this->operator=(other);
}

template <class Coefficient>
RationalFraction<Coefficient>::RationalFraction(
  const RationalFraction<Rational>& other
):
expressionType(RationalFraction::typeError) {
  this->operator=(other);
}

template <class Coefficient>
RationalFraction<Coefficient> RationalFraction<Coefficient>::zero() {
  return RationalFraction<Coefficient>::zeroStatic();
}

template <class Coefficient>
RationalFraction<Coefficient> RationalFraction<Coefficient>::zeroStatic() {
  RationalFraction result;
  result.makeZero();
  return result;
}

template <class Coefficient>
RationalFraction<Coefficient> RationalFraction<Coefficient>::one() {
  RationalFraction result;
  result.makeConstant(1);
  return result;
}

template <class Coefficient>
bool RationalFraction<Coefficient>::findOneVariableRationalRoots(
  List<Rational>& output
) {
  if (this->expressionType == TypeExpression::typeConstant) {
    output.setSize(0);
    return true;
  }
  Polynomial<Rational> polynomial;
  this->getNumerator(polynomial);
  return polynomial.findOneVariableRationalRoots(output);
}

template <class Coefficient>
bool RationalFraction<Coefficient>::needsParenthesisForMultiplication(
  FormatExpressions* unused
) const {
  (void) unused;
  switch (this->expressionType) {
  case RationalFraction::typeConstant:
    return false;
  case RationalFraction::typePolynomial:
    return
    this->numerator.getElementConst().needsParenthesisForMultiplication();
  case RationalFraction::typeRationalFraction:
    return false;
  }
  return false;
}

template <class Coefficient>
std::string RationalFraction<Coefficient>::toString(FormatExpressions* format)
const {
  if (this->expressionType == this->typeError) {
    return "[error]";
  }
  if (this->expressionType == TypeExpression::typeConstant) {
    return this->constantValue.toString();
  }
  if (this->expressionType == TypeExpression::typePolynomial) {
    return this->numerator.getElementConst().toString(format);
  }
  std::stringstream out;
  bool useFrac = format == nullptr ? false : format->flagUseFrac;
  bool needParenthesis = false;
  if (!useFrac) {
    needParenthesis =
    this->numerator.getElementConst().needsParenthesisForMultiplication();
  }
  if (useFrac) {
    out << "\\frac{";
  }
  if (needParenthesis) {
    out << "(";
  }
  out << this->numerator.getElementConst().toString(format);
  if (needParenthesis) {
    out << ")";
  }
  if (useFrac) {
    out << "}{";
  } else {
    out << "/(";
  }
  out << this->denominator.getElementConst().toString(format);
  if (useFrac) {
    out << "}";
  } else {
    out << ")";
  }
  // out << " Num vars: " << this->minimalNumberOfVariables();
  return out.str();
}

template <class Coefficient>
void RationalFraction<Coefficient>::greatestCommonDivisor(
  const Polynomial<Coefficient>& left,
  const Polynomial<Coefficient>& right,
  Polynomial<Coefficient>& output
) {
  if (
    RationalFraction<Coefficient>::greatestCommonDivisorQuick(
      left, right, output
    )
  ) {
    return;
  }
  STACK_TRACE("RationalFunctionOld::gcd");
  Polynomial<Coefficient> leastCommonMultipleBuffer;
  Polynomial<Coefficient> productBuffer;
  Polynomial<Coefficient> remainderBuffer;
  RationalFraction<Coefficient>::leastCommonMultiple(
    left, right, leastCommonMultipleBuffer
  );
  productBuffer = left;
  productBuffer *= right;
  productBuffer.divideBy(
    leastCommonMultipleBuffer,
    output,
    remainderBuffer,
    &MonomialPolynomial::orderForGreatestCommonDivisor()
  );
  if (!remainderBuffer.isEqualToZero() || output.isEqualToZero()) {
    global.fatal
    << "While computing the gcd of left = "
    << left.toString()
    << " <br>and right = "
    << right.toString()
    << " <br>I got that left * right = "
    << productBuffer.toString()
    << "<br>and that lcm(left, right) = "
    << leastCommonMultipleBuffer.toString()
    << " <br>but at the same time right * left "
    << "divided by lcm (left, right) equals<br>"
    << output.toString()
    << "<br> with remainder "
    << remainderBuffer.toString()
    << ", which is imposible."
    << global.fatal;
  }
  output.scaleNormalizeLeadingMonomial(&MonomialPolynomial::orderDefault());
}

template <class Coefficient>
void RationalFraction<Coefficient>::makeOneLetterMonomial(
  int index, const Coefficient& coefficient, int expectedNumberOfVariables
) {
  if (index < 0) {
    global.fatal
    << "I am asked to create "
    << "Monomial which has a variable of negative index "
    << index
    << ". "
    << global.fatal;
  }
  this->expressionType = TypeExpression::typePolynomial;
  expectedNumberOfVariables =
  MathRoutines::maximum(index + 1, expectedNumberOfVariables);
  this->numerator.getElement().makeDegreeOne(
    expectedNumberOfVariables, index, coefficient
  );
}

template <class Coefficient>
void RationalFraction<Coefficient>::makeMonomial(
  int letterIndex, const Rational& power, const Rational& coefficient
) {
  if (letterIndex < 0) {
    global.fatal
    << "I am asked to create Monomial which has a variable of negative index "
    << letterIndex
    << ". "
    << global.fatal;
  }
  this->expressionType = TypeExpression::typePolynomial;
  this->numerator.getElement().makeMonomial(letterIndex, power, coefficient);
}

template <class Coefficient>
void RationalFraction<Coefficient>::setNumberOfVariablesSubstituteDeletedByOne(
  int newNumberOfVariables
) {
  int oldNumberOfVariables = this->minimalNumberOfVariables();
  this->numerator.getElement().setNumberOfVariablesSubstituteDeletedByOne(
    newNumberOfVariables
  );
  this->denominator.getElement().setNumberOfVariablesSubstituteDeletedByOne(
    newNumberOfVariables
  );
  if (newNumberOfVariables < oldNumberOfVariables) {
    this->simplify();
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::operator=(
  const RationalFraction<Rational>& other
) {
  this->expressionType = other.expressionType;
  switch (this->expressionType) {
  case RationalFraction::typeConstant:
    this->constantValue = other.constantValue;
    break;
  case RationalFraction::typePolynomial:
    if (other.numerator.isZeroPointer()) {
      global.fatal << "zero pointer in numerator of other. " << global.fatal;
    }
    this->numerator.getElement() = other.numerator.getElementConst();
    break;
  case RationalFraction::typeRationalFraction:
    if (other.numerator.isZeroPointer() || other.denominator.isZeroPointer()) {
      global.fatal
      << "zero pointer in numerator or denominator of other. "
      << global.fatal;
    }
    this->numerator.getElement() = other.numerator.getElementConst();
    this->denominator.getElement() = other.denominator.getElementConst();
    break;
  default:
    break;
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::leastCommonMultiple(
  const Polynomial<Coefficient>& left,
  const Polynomial<Coefficient>& right,
  Polynomial<Coefficient>& output
) {
  if (left.isEqualToZero() || right.isEqualToZero()) {
    global.fatal
    << "Least common multiple allowing zero not allowed. "
    << global.fatal;
  }
  std::stringstream commentsOnFailure;
  bool success = Polynomial<Coefficient>::leastCommonMultiple(
    left, right, output, left.coefficients[0].one(), &commentsOnFailure
  );
  if (!success) {
    global.fatal
    << "Failure of least common multiple "
    << "computation not allowed here. "
    << global.fatal;
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::operator*=(const MonomialPolynomial& other)
{
  Polynomial<Rational> converted;
  converted.makeZero();
  converted.addMonomial(other, 1);
  *this *= converted;
}

template <class Coefficient>
void RationalFraction<Coefficient>::operator*=(
  const Polynomial<Coefficient>& other
) {
  if (other.isEqualToZero()) {
    this->makeZero();
    return;
  }
  if (this->expressionType == TypeExpression::typeConstant) {
    this->convertToType(TypeExpression::typePolynomial);
  }
  if (this->expressionType == TypeExpression::typePolynomial) {
    this->numerator.getElement() *= other;
    this->reduceMemory();
    return;
  }
  Polynomial<Coefficient> commonDivisor;
  Polynomial<Coefficient> result;
  Polynomial<Coefficient> divisor;
  ProgressReport report;
  if (report.tickAndWantReport()) {
    std::stringstream out;
    out
    << "Multiplying "
    << this->toString(&global.defaultFormat.getElement())
    << " by "
    << other.toString(&global.defaultFormat.getElement());
    report.report(out.str());
  }
  RationalFraction<Coefficient>::greatestCommonDivisor(
    this->denominator.getElement(), other, commonDivisor
  );
  this->numerator.getElement() *= other;
  List<MonomialPolynomial>::Comparator* monomialOrder =
  &MonomialPolynomial::orderForGreatestCommonDivisor();
  this->numerator.getElement().divideBy(
    commonDivisor, result, divisor, monomialOrder
  );
  if (!divisor.isEqualToZero()) {
    global.fatal
    << "Polynomial equal to zero not allowed here. "
    << global.fatal;
  }
  this->numerator.getElement() = result;
  this->denominator.getElement().divideBy(
    commonDivisor, result, divisor, monomialOrder
  );
  if (!divisor.isEqualToZero()) {
    global.fatal << "Polynomial not equal to zero. " << global.fatal;
  }
  this->denominator.getElement() = result;
  this->reduceMemory();
  this->simplifyLeadingCoefficientOnly();
  if (report.tickAndWantReport()) {
    std::stringstream out;
    out
    << "Multiplying "
    << this->toString(&global.defaultFormat.getElement())
    << " by "
    << other.toString(&global.defaultFormat.getElement());
    out << " and the result is:\n" << this->toString();
    report.report(out.str());
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::operator/=(
  const RationalFraction<Coefficient>& other
) {
  this->checkConsistency();
  RationalFraction inverted;
  inverted = other;
  inverted.checkConsistency();
  inverted.invert();
  inverted.checkConsistency();
  *this *= inverted;
  if (!this->checkConsistency()) {
    global.fatal << "Incosistent rational function. " << global.fatal;
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::operator*=(const Coefficient& other) {
  if (other.isEqualToZero()) {
    this->makeZero();
    return;
  }
  switch (this->expressionType) {
  case RationalFraction::typeConstant:
    this->constantValue *= other;
    return;
  case RationalFraction::typePolynomial:
    this->numerator.getElement() *= other;
    return;
  case RationalFraction::typeRationalFraction:
    this->numerator.getElement() *= other;
    this->simplifyLeadingCoefficientOnly();
    return;
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::operator*=(
  const RationalFraction<Coefficient>& other
) {
  this->checkConsistency();
  other.checkConsistency();
  if (this == &other) {
    this->raiseToPower(2);
    return;
  }
  if (other.isEqualToZero() || this->isEqualToZero()) {
    this->makeZero();
    return;
  }
  if (other.expressionType == TypeExpression::typeConstant) {
    this->operator*=(other.constantValue);
    return;
  }
  if (other.expressionType == TypeExpression::typePolynomial) {
    this->operator*=(other.numerator.getElementConst());
    return;
  }
  if (this->expressionType == TypeExpression::typeConstant) {
    Coefficient constant;
    constant = this->constantValue;
    this->operator=(other);
    this->operator*=(constant);
    return;
  }
  if (this->expressionType == TypeExpression::typePolynomial) {
    Polynomial<Coefficient> polynomial;
    polynomial = this->numerator.getElement();
    this->operator=(other);
    this->operator*=(polynomial);
    return;
  }
  Polynomial<Coefficient> greatestCommonDivisor1;
  Polynomial<Coefficient> greatestCommonDivisor2;
  Polynomial<Coefficient> polynomial1;
  Polynomial<Coefficient> polynomial2;
  ProgressReport report;
  if (report.tickAndWantReport()) {
    std::stringstream out;
    out << "Multiplying " << this->toString() << " by " << other.toString();
    report.report(out.str());
  }
  RationalFraction<Coefficient>::greatestCommonDivisor(
    other.denominator.getElementConst(),
    this->numerator.getElement(),
    greatestCommonDivisor1
  );
  RationalFraction<Coefficient>::greatestCommonDivisor(
    this->denominator.getElement(),
    other.numerator.getElementConst(),
    greatestCommonDivisor2
  );
  List<MonomialPolynomial>::Comparator* monomialOrder =
  &MonomialPolynomial::orderForGreatestCommonDivisor();
  this->numerator.getElement().divideBy(
    greatestCommonDivisor1, polynomial1, polynomial2, monomialOrder
  );
  this->numerator.getElement() = polynomial1;
  if (!polynomial2.isEqualToZero()) {
    global.fatal
    << "Polynomial equal to zero not allowed here. "
    << global.fatal;
  }
  other.denominator.getElementConst().divideBy(
    greatestCommonDivisor1, polynomial1, polynomial2, monomialOrder
  );
  if (!polynomial2.isEqualToZero()) {
    global.fatal << "Polynomial must not be zero here. " << global.fatal;
  }
  this->denominator.getElement() *= polynomial1;
  this->denominator.getElement().divideBy(
    greatestCommonDivisor2, polynomial1, polynomial2, monomialOrder
  );
  if (!polynomial2.isEqualToZero()) {
    global.fatal << "Polynomial must not be zero here. " << global.fatal;
  }
  this->denominator.getElement() = polynomial1;
  other.numerator.getElementConst().divideBy(
    greatestCommonDivisor2, polynomial1, polynomial2, monomialOrder
  );
  if (!polynomial2.isEqualToZero()) {
    global.fatal << "Polynomial must not be zero here. " << global.fatal;
  }
  this->numerator.getElement() *= polynomial1;
  this->reduceMemory();
  this->simplifyLeadingCoefficientOnly();
  if (report.tickAndWantReport()) {
    std::stringstream out;
    out << "Multiplying " << this->toString() << " by " << other.toString();
    out << " and the result is:\n" << this->toString();
    report.report(out.str());
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::operator+=(
  const RationalFraction<Coefficient>& other
) {
  if (this == &other) {
    *this *= Rational(2);
    return;
  }
  if (!this->checkConsistency()) {
    global.fatal << "Corrupt rational function. " << global.fatal;
  }
  if (!other.checkConsistency()) {
    global.fatal << "Corrupt other rational function. " << global.fatal;
  }
  if (other.expressionType < this->expressionType) {
    RationalFraction<Coefficient> converter;
    converter = other;
    converter.convertToType(this->expressionType);
    this->addSameTypes(converter);
    if (!this->checkConsistency()) {
      global.fatal
      << "Output of rational function addition is corrupt. "
      << global.fatal;
    }
    return;
  }
  if (this->expressionType == other.expressionType) {
    this->addSameTypes(other);
    if (!this->checkConsistency()) {
      global.fatal
      << "Output of rational function addition is corrupt "
      << "for same type of rational functions."
      << global.fatal;
    }
    return;
  }
  if (this->expressionType < other.expressionType) {
    this->convertToType(other.expressionType);
    this->addSameTypes(other);
    if (!this->checkConsistency()) {
      global.fatal
      << "Corrupt outputs of rational "
      << "function addition, different expression types. "
      << global.fatal;
    }
  }
  if (!this->checkConsistency()) {
    global.fatal
    << "Corrupt final output of rational function addition. "
    << global.fatal;
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::reduceRationalFraction() {
  if (this->expressionType != TypeExpression::typeRationalFraction) {
    return;
  }
  if (this->numerator.getElement().isEqualToZero()) {
    return;
  }
  Polynomial<Coefficient>& simplifiedNumerator = this->numerator.getElement();
  Polynomial<Coefficient>& simplifiedDenominator =
  this->denominator.getElement();
  MonomialPolynomial scale;
  simplifiedNumerator.scaleToPositiveMonomialExponents(scale);
  simplifiedDenominator *= scale;
  simplifiedDenominator.scaleToPositiveMonomialExponents(scale);
  simplifiedNumerator *= scale;
  List<MonomialPolynomial>::Comparator* monomialOrder =
  &MonomialPolynomial::orderForGreatestCommonDivisor();
  Polynomial<Coefficient> greatestCommonDivisor;
  Polynomial<Coefficient> quotientByGreatestCommonDivisor;
  Polynomial<Coefficient> remainderByGCD;
  this->greatestCommonDivisor(
    simplifiedNumerator, simplifiedDenominator, greatestCommonDivisor
  );
  if (greatestCommonDivisor.isEqualToZero()) {
    global.fatal
    << "This is a programing error: "
    << " while fetching the gcd of "
    << simplifiedNumerator.toString()
    << " and "
    << simplifiedDenominator.toString()
    << " I got 0, which is impossible. "
    << global.fatal;
  }
  this->numerator.getElement().divideBy(
    greatestCommonDivisor,
    quotientByGreatestCommonDivisor,
    remainderByGCD,
    monomialOrder
  );
  simplifiedNumerator = quotientByGreatestCommonDivisor;
  simplifiedDenominator.divideBy(
    greatestCommonDivisor,
    quotientByGreatestCommonDivisor,
    remainderByGCD,
    monomialOrder
  );
  this->denominator.getElement() = quotientByGreatestCommonDivisor;
}

template <class Coefficient>
void RationalFraction<Coefficient>::simplify() {
  STACK_TRACE("RationalFraction::simplify");
  this->reduceRationalFraction();
  this->reduceMemory();
  this->simplifyLeadingCoefficientOnly();
}

template <class Coefficient>
Rational RationalFraction<Coefficient>::scaleToIntegral() {
  if (this->isEqualToZero()) {
    return Rational::one();
  }
  if (this->expressionType == TypeExpression::typeConstant) {
    Rational result = this->constantValue;
    result.invert();
    this->constantValue.makeOne();
    return result;
  }
  if (this->expressionType == TypeExpression::typePolynomial) {
    return
    this->numerator.getElement().scaleNormalizeLeadingMonomial(
      &MonomialPolynomial::orderDefault()
    );
  }
  if (this->expressionType != TypeExpression::typeRationalFraction) {
    return Rational::one();
  }
  Rational result;
  result =
  this->numerator.getElement().scaleNormalizeLeadingMonomial(
    &MonomialPolynomial::orderDefault()
  );
  result /=
  this->denominator.getElement().scaleNormalizeLeadingMonomial(
    &MonomialPolynomial::orderDefault()
  );
  return result;
}

template <class Coefficient>
void RationalFraction<Coefficient>::simplifyLeadingCoefficientOnly() {
  if (this->expressionType != TypeExpression::typeRationalFraction) {
    return;
  }
  Coefficient scaleNumerator =
  this->numerator.getElement().scaleNormalizeLeadingMonomial(
    &MonomialPolynomial::orderDefault()
  );
  Coefficient scaleDenominator =
  this->denominator.getElement().scaleNormalizeLeadingMonomial(
    &MonomialPolynomial::orderDefault()
  );
  Coefficient scale = scaleDenominator / scaleNumerator;
  this->denominator.getElement() *= scale.getDenominator();
  this->numerator.getElement() *= scale.getNumerator();
}

template <class Coefficient>
bool RationalFraction<Coefficient>::isConstant(Rational* whichConstant) const {
  if (this->expressionType != TypeExpression::typeConstant) {
    return false;
  }
  if (whichConstant != nullptr) {
    *whichConstant = this->constantValue;
  }
  return true;
}

template <class Coefficient>
bool RationalFraction<Coefficient>::isInteger() const {
  return
  this->expressionType == TypeExpression::typeConstant &&
  this->constantValue.isInteger();
}

template <class Coefficient>
bool RationalFraction<Coefficient>::isSmallInteger(int* whichInteger) const {
  return
  this->expressionType == TypeExpression::typeConstant &&
  this->constantValue.isSmallInteger(whichInteger);
}

template <class Coefficient>
RationalFraction<Coefficient> RationalFraction<Coefficient>::
scaleNormalizeIndex(
  List<RationalFraction<Rational> >& input, int indexNonZeroElement
) {
  if (input.size == 0) {
    return 1;
  }
  List<Rational> scales;
  Rational scale;
  Polynomial<Rational> currentNumerator;
  Polynomial<Rational> currentDenominator;
  for (int i = 0; i < input.size; i ++) {
    RationalFraction<Rational>& current = input[i];
    current.getNumerator(currentNumerator);
    current.getDenominator(currentDenominator);
    scale =
    currentNumerator.scaleNormalizeLeadingMonomial(
      &MonomialPolynomial::orderDefault()
    );
    scale /=
    currentDenominator.scaleNormalizeLeadingMonomial(
      &MonomialPolynomial::orderDefault()
    );
    scales.addOnTop(scale);
  }
  LargeIntegerUnsigned numeratorContentGreatestCommonDivisor =
  scales[0].getNumerator().value;
  LargeIntegerUnsigned denominatorContentLeastCommonMultiple =
  scales[0].getDenominator();
  for (int i = 1; i < scales.size; i ++) {
    numeratorContentGreatestCommonDivisor =
    LargeIntegerUnsigned::leastCommonMultiple(
      scales[i].getNumerator().value, numeratorContentGreatestCommonDivisor
    );
    denominatorContentLeastCommonMultiple =
    LargeIntegerUnsigned::greatestCommonDivisor(
      denominatorContentLeastCommonMultiple, scales[i].getDenominator()
    );
  }
  scale = numeratorContentGreatestCommonDivisor;
  scale /= denominatorContentLeastCommonMultiple;
  if (scales[indexNonZeroElement] < 0) {
    scale *= - 1;
  }
  for (int i = 0; i < input.size; i ++) {
    input[i] *= scale;
  }
  return scale;
}

template <class Coefficient>
bool RationalFraction<Coefficient>::greatestCommonDivisorQuick(
  const Polynomial<Coefficient>& left,
  const Polynomial<Coefficient>& right,
  Polynomial<Coefficient>& output
) {
  if (left.totalDegree() > 1 && right.totalDegree() > 1) {
    return false;
  }
  if (left.isEqualToZero()) {
    global.fatal
    << "Greatest common divisor involving zero not allowed. "
    << global.fatal;
  }
  Polynomial<Coefficient> quotient;
  Polynomial<Coefficient> remainder;
  List<MonomialPolynomial>::Comparator* monomialOrder =
  &MonomialPolynomial::orderDefault();
  if (left.totalDegree() > right.totalDegree()) {
    left.divideBy(right, quotient, remainder, monomialOrder);
    if (remainder.isEqualToZero()) {
      output = right;
    } else {
      output.makeConstant(left.coefficients[0].one());
    }
  } else {
    right.divideBy(left, quotient, remainder, monomialOrder);
    if (remainder.isEqualToZero()) {
      output = left;
    } else {
      output.makeConstant(left.coefficients[0].one());
    }
  }
  return true;
}

template <class Coefficient>
void RationalFraction<Coefficient>::raiseToPower(int power) {
  STACK_TRACE("RationalFraction::raiseToPower");
  this->checkConsistency();
  if (power < 0) {
    this->invert();
    power = - power;
  }
  if (power == 0) {
    this->makeOne();
    return;
  }
  switch (this->expressionType) {
  case RationalFraction::typeConstant:
    MathRoutines::raiseToPower(
      this->constantValue, power, this->constantValue.one()
    );
    break;
  case RationalFraction::typePolynomial:
    this->numerator.getElement().raiseToPower(
      power, this->constantValue.one()
    );
    break;
  case RationalFraction::typeRationalFraction:
    this->numerator.getElement().raiseToPower(
      power, this->constantValue.one()
    );
    this->denominator.getElement().raiseToPower(
      power, this->constantValue.one()
    );
    break;
  }
  this->checkConsistency();
}

template <class Coefficient>
void RationalFraction<Coefficient>::clearDenominators(
  RationalFraction<Coefficient>& outputWasMultipliedBy
) {
  Rational scalar;
  switch (this->expressionType) {
  case RationalFraction::typeConstant:
    scalar = this->constantValue.getDenominator();
    outputWasMultipliedBy.makeConstant(scalar);
    this->constantValue *= scalar;
    break;
  case RationalFraction::typePolynomial:
    this->numerator.getElement().clearDenominators(scalar);
    outputWasMultipliedBy.makeConstant(scalar);
    break;
  case RationalFraction::typeRationalFraction:
    RationalFraction converter;
    outputWasMultipliedBy.operator=(this->denominator.getElement());
    *this *= outputWasMultipliedBy;
    this->clearDenominators(converter);
    outputWasMultipliedBy *= converter;
    break;
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::addSameTypes(
  const RationalFraction<Coefficient>& other
) {
  switch (this->expressionType) {
  case RationalFraction::typeConstant:
    this->constantValue += other.constantValue;
    break;
  case RationalFraction::typePolynomial:
    this->numerator.getElement() += other.numerator.getElementConst();
    break;
  case RationalFraction::typeRationalFraction:
    this->addHonestRationalFunction(other);
    break;
  }
  this->reduceMemory();
  if (!this->checkConsistency()) {
    global.fatal << "Bad rational function. " << global.fatal;
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::reducePolynomialToRational() {
  if (this->expressionType != TypeExpression::typePolynomial) {
    return;
  }
  if (!this->numerator.getElement().isConstant()) {
    return;
  }
  this->expressionType = TypeExpression::typeConstant;
  if (this->numerator.getElement().isEqualToZero()) {
    this->constantValue.makeZero();
  } else {
    this->constantValue = this->numerator.getElement().coefficients[0];
  }
  this->numerator.freeMemory();
}

template <class Coefficient>
void RationalFraction<Coefficient>::addHonestRationalFunction(
  const RationalFraction<Coefficient>& other
) {
  STACK_TRACE("RationalFraction::addHonestRationalFunction");
  Coefficient proportionalityCoefficient;
  if (
    !this->denominator.getElement().isProportionalTo(
      other.denominator.getElementConst(),
      proportionalityCoefficient,
      Coefficient::oneStatic()
    )
  ) {
    Polynomial<Coefficient> buffer;
    buffer = this->denominator.getElement();
    this->numerator.getElement() *= other.denominator.getElementConst();
    buffer *= other.numerator.getElementConst();
    this->numerator.getElement() += buffer;
    this->denominator.getElement() *= other.denominator.getElementConst();
    if (this->denominator.getElement().isEqualToZero()) {
      global.fatal
      << "Denominator of rational function is zero."
      << global.fatal;
    }
    this->simplify();
  } else {
    this->numerator.getElement() *= proportionalityCoefficient;
    this->denominator.getElement() *= proportionalityCoefficient;
    this->numerator.getElement() += other.numerator.getElementConst();
    this->reduceMemory();
    this->simplifyLeadingCoefficientOnly();
  }
  if (!this->checkConsistency()) {
    global.fatal << "Inconsistent rational function. " << global.fatal;
  }
}

template <class Coefficient>
bool RationalFraction<Coefficient>::isEqualTo(
  const RationalFraction<Coefficient>& other
) const {
  if (this->expressionType != other.expressionType) {
    return false;
  }
  switch (this->expressionType) {
  case RationalFraction::typeRationalFraction:
    return
    this->numerator.getElementConst().isEqualTo(
      other.numerator.getElementConst()
    ) &&
    this->denominator.getElementConst().isEqualTo(
      other.denominator.getElementConst()
    );
  case RationalFraction::typePolynomial:
    return
    this->numerator.getElementConst().isEqualTo(
      other.numerator.getElementConst()
    );
  case RationalFraction::typeConstant:
    return this->constantValue == other.constantValue;
  }
  global.fatal
  << "This line of code is supposed to be unreachable. "
  << global.fatal;
  return false;
}

template <class Coefficient>
void RationalFraction<Coefficient>::multiplyByConstant(
  const Rational& inputConstant
) {
  this->operator*=(inputConstant);
}

template <class Coefficient>
void RationalFraction<Coefficient>::reduceMemory() {
  this->reduceRationalFunctionToPolynomial();
  this->reducePolynomialToRational();
  if (!this->checkConsistency()) {
    global.fatal << "Corrupt rational function. " << global.fatal;
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::operator=(
  const Polynomial<Coefficient>& other
) {
  this->expressionType = TypeExpression::typePolynomial;
  this->numerator.getElement() = other;
  if (!other.isEqualToZero()) {
    // Coefficients of types such as ElementZModP may have
    // their zero constants not known at compile time.
    // We therefore initialize a zero constant here.
    this->constantValue = other.coefficients[0].zero();
  }
  this->reduceMemory();
}

template <class Coefficient>
unsigned int RationalFraction<Coefficient>::hashFunction() const {
  switch (this->expressionType) {
  case RationalFraction::typeConstant:
    return this->constantValue.hashFunction();
  case RationalFraction::typePolynomial:
    return this->numerator.getElementConst().hashFunction();
  case RationalFraction::typeRationalFraction:
    return
    this->numerator.getElementConst().hashFunction() * HashConstants::constant0
    +
    this->denominator.getElementConst().hashFunction() *
    HashConstants::constant1;
  default:
    return static_cast<unsigned int>(- 1);
  }
}

template <class Coefficient>
unsigned int RationalFraction<Coefficient>::hashFunction(
  const RationalFraction<Coefficient>& input
) {
  return input.hashFunction();
}

template <class Coefficient>
void RationalFraction<Coefficient>::operator=(int other) {
  this->makeConstant(other);
}

template <class Coefficient>
void RationalFraction<Coefficient>::operator=(const Coefficient& other) {
  this->makeConstant(other);
}

template <class Coefficient>
void RationalFraction<Coefficient>::setNumberOfVariables(
  int goalNumberOfVariables
) {
  this->setNumberOfVariablesSubstituteDeletedByOne(goalNumberOfVariables);
}

template <class Coefficient>
void RationalFraction<Coefficient>::getNumerator(
  Polynomial<Coefficient>& output
) const {
  switch (this->expressionType) {
  case RationalFraction::typeConstant:
    output.makeConstant(this->constantValue);
    return;
  default:
    output = this->numerator.getElementConst();
    return;
  }
}

template <class Coefficient>
bool RationalFraction<Coefficient>::isNegative() {
  if (this->expressionType == TypeExpression::typeConstant) {
    return this->constantValue.isNegative();
  }
  return false;
}

template <class Coefficient>
void RationalFraction<Coefficient>::getDenominator(
  Polynomial<Coefficient>& output
) const {
  switch (this->expressionType) {
  case RationalFraction::typeRationalFraction:
    if (this->denominator.isZeroPointer()) {
      global.fatal
      << "The rational function is "
      << "supposed to be honest, but the denominator pointer is zero. "
      << global.fatal;
    }
    output = this->denominator.getElementConst();
    return;
  default:
    output.makeConstant(Rational(1));
    return;
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::operator+=(
  const Polynomial<Coefficient>& other
) {
  RationalFraction<Coefficient> converter;
  converter = other;
  *this += converter;
}

template <class Coefficient>
bool RationalFraction<Coefficient>::operator==(int other) const {
  if (other == 0) {
    return this->isEqualToZero();
  } else {
    return
    this->expressionType == TypeExpression::typeConstant && (
      this->constantValue == other
    );
  }
}

template <class Coefficient>
bool RationalFraction<Coefficient>::operator!=(int other) {
  return !(this->operator==(other));
}

template <class Coefficient>
bool RationalFraction<Coefficient>::operator==(
  const RationalFraction<Coefficient>& other
) const {
  return this->isEqualTo(other);
}

template <class Coefficient>
bool RationalFraction<Coefficient>::operator>(
  const RationalFraction<Rational>& other
) const {
  if (this->expressionType < other.expressionType) {
    return false;
  }
  if (this->expressionType > other.expressionType) {
    return true;
  }
  switch (this->expressionType) {
  case RationalFraction::typeConstant:
    return this->constantValue > other.constantValue;
  case RationalFraction::typePolynomial:
    return
    this->numerator.getElementConst() >
    other.numerator.getElementConst();
  case RationalFraction::typeRationalFraction:
    if (
      other.denominator.getElementConst() > this->denominator.getElementConst()
    ) {
      return true;
    }
    if (
      this->denominator.getElementConst() > other.denominator.getElementConst()
    ) {
      return false;
    }
    return
    this->numerator.getElementConst() >
    other.numerator.getElementConst();
  default:
    return false;
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::operator*=(int other) {
  *this *= Rational(other);
}

template <class Coefficient>
bool RationalFraction<Coefficient>::operator<(
  const RationalFraction<Rational>& other
) const {
  return other > *this;
}

template <class Coefficient>
bool RationalFraction<Coefficient>::operator<=(
  const RationalFraction<Rational>& other
) const {
  if (*this == other) {
    return true;
  }
  return other > *this;
}

template <class Coefficient>
void RationalFraction<Coefficient>::operator/=(
  const Polynomial<Coefficient>& other
) {
  RationalFraction<Coefficient> otherInverted;
  otherInverted = other;
  otherInverted.invert();
  *this *= otherInverted;
  if (!this->checkConsistency()) {
    global.fatal << "Bad rational function. " << global.fatal;
  }
}

template <class Coefficient>
void RationalFraction<Coefficient>::reduceRationalFunctionToPolynomial() {
  if (this->expressionType != TypeExpression::typeRationalFraction) {
    return;
  }
  if (this->denominator.getElement().isConstant()) {
    this->numerator.getElement() /=
    this->denominator.getElement().coefficients[0];
    this->denominator.freeMemory();
    this->expressionType = TypeExpression::typePolynomial;
  }
  if (this->numerator.getElement().isEqualToZero()) {
    this->makeZero();
  }
}

template <class Coefficient>
bool RationalFraction<Coefficient>::substitute(
  const PolynomialSubstitution<Rational>& substitution, const Coefficient& one
) {
  return this->substitute(substitution, one, nullptr);
}

template <class Coefficient>
bool RationalFraction<Coefficient>::substitute(
  const PolynomialSubstitution<Coefficient>& substitution,
  const Coefficient& one,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("RationalFraction::substitution");
  switch (this->expressionType) {
  case RationalFraction::typeConstant:
    return true;
  case RationalFraction::typePolynomial:
    if (!this->numerator.getElement().substitute(substitution, one)) {
      return false;
    }
    this->simplify();
    return true;
  case RationalFraction::typeRationalFraction:
    if (!this->numerator.getElement().substitute(substitution, one)) {
      return false;
    }
    if (!this->denominator.getElement().substitute(substitution, one)) {
      return false;
    }
    if (this->denominator.getElement().isEqualToZero()) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Substitution leads to division by zero. ";
      }
      return false;
    }
    this->simplify();
    return true;
  default:
    global.fatal
    << "Default case not allowed in "
    << "RationalFraction::substitution. Rational function: "
    << this->toString()
    << "."
    << global.fatal;
    break;
  }
  return false;
}

template <class Coefficient>
int RationalFraction<Coefficient>::minimalNumberOfVariables() const {
  switch (this->expressionType) {
  case RationalFraction::typeConstant:
    return 0;
  case RationalFraction::typePolynomial:
    return this->numerator.getElementConst().minimalNumberOfVariables();
  case RationalFraction::typeRationalFraction:
    return
    MathRoutines::maximum(
      this->numerator.getElementConst().minimalNumberOfVariables(),
      this->denominator.getElementConst().minimalNumberOfVariables()
    );
  default:
    // this should never happen! maybe global.fatal << global.fatal here...
    return - 1;
  }
}

template <class Coefficient>
bool RationalFraction<Coefficient>::getRelations(
  const List<Polynomial<Rational> >& inputElements,
  List<Polynomial<Rational> >& outputGeneratorLabels,
  List<Polynomial<Rational> >& outputRelations,
  std::stringstream& comments
) {
  STACK_TRACE("RationalFunctionOld::GetRelationsGetRelations");
  outputGeneratorLabels.setSize(inputElements.size);
  outputRelations.setSize(0);
  if (inputElements.size == 0) {
    return true;
  }
  List<Polynomial<Rational> > groebnerBasis;
  groebnerBasis = inputElements;
  int startingGenerators = inputElements.size;
  int totalStartingVariables = 0;
  for (int i = 0; i < inputElements.size; i ++) {
    totalStartingVariables =
    MathRoutines::maximum(
      totalStartingVariables, inputElements[0].minimalNumberOfVariables()
    );
  }
  Polynomial<Rational> currentGenerator;
  for (int i = 0; i < startingGenerators; i ++) {
    Polynomial<Rational>& currentPoly = groebnerBasis[i];
    currentPoly.setNumberOfVariablesSubstituteDeletedByOne(
      totalStartingVariables + startingGenerators
    );
    currentGenerator.makeDegreeOne(
      totalStartingVariables + startingGenerators,
      i + totalStartingVariables,
      1
    );
    outputGeneratorLabels[i] = currentGenerator;
    currentPoly -= currentGenerator;
  }
  GroebnerBasisComputation<Rational> computation;
  computation.polynomialOrder.monomialOrder.setComparison(
    MonomialPolynomial::greaterThan_leftLargerWins
  );
  if (!computation.transformToReducedGroebnerBasis(groebnerBasis, false)) {
    comments << "Failed to find Groebner basis";
    return false;
  }
  outputRelations.reserve(groebnerBasis.size);
  outputRelations.setSize(0);
  for (int i = 0; i < groebnerBasis.size; i ++) {
    Polynomial<Rational>& currentPolynomial = groebnerBasis[i];
    bool bad = false;
    for (int j = 0; j < totalStartingVariables; j ++) {
      if (currentPolynomial.maximumPowerOfVariableIndex(j) > 0) {
        bad = true;
        break;
      }
    }
    if (!bad) {
      outputRelations.addOnTop(currentPolynomial);
    }
  }
  return true;
}

#endif // header_math_rational_function_ALREADY_INCLUDED
