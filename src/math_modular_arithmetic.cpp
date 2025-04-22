#include "html_routines.h"
#include "math_extra_algebraic_numbers.h"
#include "math_general.h"
#include "math_general_implementation.h" // IWYU pragma: keep: breaks g++ -02 optimization build.
#include "math_general_polynomial_computations_advanced_implementation.h" // IWYU pragma: keep: breaks web assembly build.
#include "math_general_polynomial_computations_basic_implementation.h" // IWYU pragma: keep: breaks g++ -02 optimization build.
#include "math_subsets_selections.h"
#include "progress_report.h"

const int ElementZmodP::maximumModulusForUserFacingPolynomialDivision = 10000;



bool ElementZmodP::needsParenthesisForMultiplication(
  FormatExpressions* format
) const {
  if (format == nullptr) {
    return true;
  }
  return !format->flagSuppressModP;
}

std::string ElementZmodP::toStringModP() const {
  return ElementZmodP::toStringModP(this->modulus);
}

std::string ElementZmodP::toStringModP(const LargeIntegerUnsigned& modulus) {
  std::stringstream out;
  out << "mod (" << modulus.toString() << ")";
  return out.str();
}

std::string ElementZmodP::toStringPolynomial(
  const Polynomial<ElementZmodP>& input, FormatExpressions* format
) const {
  std::stringstream out;
  out << input.toString(format) << "\\mod " << this->modulus.toString();
  return out.str();
}

std::string ElementZmodP::toStringPolynomialCalculator(
  const Polynomial<ElementZmodP>& input, FormatExpressions* format
) const {
  std::stringstream out;
  out << "PolynomialModP{}(";
  out << input.toString(format);
  out << ", " << this->modulus << ")";
  return out.str();
}

std::string ElementZmodP::toString(FormatExpressions* format) const {
  bool suppressModulus = false;
  if (format != nullptr) {
    suppressModulus = format->flagSuppressModP;
  }
  std::stringstream out;
  if (suppressModulus) {
    out << this->value.toString();
  } else {
    out << "(" << this->value.toString() << " " << this->toStringModP() << ")";
  }
  return out.str();
}

bool ElementZmodP::operator*=(const Rational& other) {
  ElementZmodP otherElement;
  otherElement.modulus = this->modulus;
  if (!otherElement.assignRational(other)) {
    return false;
  }
  *this *= otherElement;
  return true;
}

ElementZmodP ElementZmodP::operator*(const Rational& other) const {
  ElementZmodP otherElement;
  otherElement.modulus = this->modulus;
  otherElement.assignRational(other);
  ElementZmodP result = *this;
  result *= otherElement;
  return result;
}

unsigned int ElementZmodP::hashFunction() const {
  if (this->value.isEqualToZero()) {
    return 0;
  }
  return
  this->value.hashFunction() * HashConstants::constant0 +
  this->modulus.hashFunction() * HashConstants::constant1;
}

void ElementZmodP::checkIamInitialized() const {
  if (this->modulus.isEqualToZero()) {
    global.fatal
    << "Element of Z mod p has non-initialized modulus. "
    << global.fatal;
  }
  if (this->flagDeallocated) {
    global.fatal << "Use after free of element of Z mod p. " << global.fatal;
  }
}

void ElementZmodP::operator=(const LargeInteger& other) {
  this->checkIamInitialized();
  this->value = other.value;
  this->value %= this->modulus;
  if (other.sign == - 1) {
    ElementZmodP minusOne;
    minusOne.makeMinusOne(this->modulus);
    *this *= minusOne;
  }
}

bool ElementZmodP::operator>(const ElementZmodP& other) const {
  if (this->modulus > other.modulus) {
    return true;
  }
  if (other.modulus > this->modulus) {
    return false;
  }
  return this->value > other.value;
}

ElementZmodP ElementZmodP::zeroStatic() {
  ElementZmodP result;
  result.modulus = 1;
  return result;
}

ElementZmodP ElementZmodP::oneStatic() {
  ElementZmodP result;
  result.modulus = 1;
  return result;
}

ElementZmodP ElementZmodP::zero() const {
  ElementZmodP result;
  result.modulus = this->modulus;
  if (this->modulus == 0) {
    global.fatal
    << "Extracting constant one from non-initialized element not allowed. "
    << global.fatal;
  }
  return result;
}

ElementZmodP ElementZmodP::one() const {
  ElementZmodP result;
  result.modulus = this->modulus;
  result.value = 1;
  if (this->modulus == 0) {
    global.fatal
    << "Extracting zero constant from non-initialized element not allowed. "
    << global.fatal;
  }
  return result;
}

void ElementZmodP::convertPolynomialModularToPolynomialIntegral(
  const Polynomial<ElementZmodP>& input,
  Polynomial<Rational>& output,
  bool useNegatives
) {
  STACK_TRACE("ElementZmodP::convertPolynomialModularToPolynomialIntegral");
  output.makeZero();
  if (input.size() == 0) {
    return;
  }
  Rational modulusDividedByTwo = input.coefficients[0].modulus;
  modulusDividedByTwo /= 2;
  for (int i = 0; i < input.size(); i ++) {
    Rational coefficient = input.coefficients[i].value;
    if (useNegatives) {
      if (coefficient > modulusDividedByTwo) {
        coefficient = input.coefficients[i].value;
        coefficient -= input.coefficients[i].modulus;
      }
    }
    output.addMonomial(input[i], coefficient);
  }
}

void ElementZmodP::convertLiftPolynomialModular(
  const Polynomial<ElementZmodP>& input,
  Polynomial<ElementZmodP>& output,
  const LargeIntegerUnsigned& newModulus
) {
  STACK_TRACE("ElementZmodP::convertLiftPolynomialModular");
  if (&input == &output) {
    Polynomial<ElementZmodP> inputCopy = input;
    ElementZmodP::convertLiftPolynomialModular(inputCopy, output, newModulus);
    return;
  }
  output.makeZero();
  if (input.size() == 0) {
    return;
  }
  for (int i = 0; i < input.size(); i ++) {
    ElementZmodP coefficient;
    coefficient.modulus = newModulus;
    coefficient.value = input.coefficients[i].value;
    output.addMonomial(input.monomials[i], coefficient);
  }
}

void ElementZmodP::convertPolynomialsRationalToModular(      const List<Polynomial<Rational>>& input,
                                                       List<Polynomial<ElementZmodP>>& output,
                                                       const LargeIntegerUnsigned& newModulus
){
  STACK_TRACE("ElementZmodP::convertPolynomialsRationalToModular");
  for (const Polynomial<Rational>& polynomial: input){
    Polynomial<ElementZmodP> converted;
    ElementZmodP:: convertPolynomialRationalToModular(polynomial, converted, newModulus);
output.addOnTop(converted);
  }

}

void ElementZmodP::convertPolynomialRationalToModular(const Polynomial<Rational>& input,
  Polynomial<ElementZmodP>& output,
  const LargeIntegerUnsigned& newModulus
) {
  STACK_TRACE("ElementZmodP::convertPolynomialModuloIntegerAfterScalingToIntegral");
  Polynomial<Rational> rescaled;
  rescaled = input;
  rescaled.scaleNormalizeLeadingMonomial(&MonomialPolynomial::orderDefault());
  output.setExpectedSize(input.size());
  ElementZmodP coefficient;
  coefficient.modulus = newModulus;
  output.makeZero();
  for (int i = 0; i < input.size(); i ++) {
    coefficient = input.coefficients[i];
    output.addMonomial(input[i], coefficient);
  }
}

void ElementZmodP::operator=(const ElementZmodP& other) {
  this->modulus = other.modulus;
  this->value = other.value;
}

void ElementZmodP::operator=(const LargeIntegerUnsigned& other) {
  this->checkIamInitialized();
  this->value = other;
  this->value %= this->modulus;
}

bool ElementZmodP::invert() {
  ElementZmodP copy = *this;
  this->makeOne(this->modulus);
  bool result = (*this /= copy);
  return result;
}

void ElementZmodP::makeZero() {
  this->value = 0;
  this->checkIamInitialized();
}

ElementZmodP ElementZmodP::getNumerator() const {
  return *this;
}

ElementZmodP ElementZmodP::getDenominator() const {
  return this->one();
}

void ElementZmodP::makeOne(const LargeIntegerUnsigned& newModulo) {
  this->modulus = newModulo;
  this->value = 1;
}

void ElementZmodP::makeMinusOne(const LargeIntegerUnsigned& newModulo) {
  this->modulus = newModulo;
  this->value = newModulo;
  this->value --;
}

void ElementZmodP::makeFrom(int inputModulus, int inputValue) {
  if (inputModulus <= 0) {
    global.fatal << "Non-positive modulus not allowed. " << global.fatal;
  }
  this->modulus = inputModulus;
  *this = inputValue;
}

void ElementZmodP::makeFrom(
  const LargeIntegerUnsigned& inputModulus, int inputValue
) {
  if (inputModulus <= 0) {
    global.fatal << "Non-positive modulus not allowed. " << global.fatal;
  }
  this->modulus = inputModulus;
  *this = inputValue;
}

void ElementZmodP::negate() {
  if (this->value.isEqualToZero()) {
    return;
  }
  this->value = this->modulus - this->value;
}

void ElementZmodP::checkEqualModuli(const ElementZmodP& other) {
  if (this->modulus != other.modulus) {
    global.fatal
    << "Attempt to make an operation "
    << "with two elemetns of Z mod P with different moduli, "
    << this->modulus.toString()
    << " and "
    << other.modulus.toString()
    << ". "
    << global.fatal;
  }
}

ElementZmodP ElementZmodP::operator*(const ElementZmodP& other) const {
  ElementZmodP result = *this;
  result *= other;
  return result;
}

ElementZmodP ElementZmodP::scaleNormalizeIndex(
  List<ElementZmodP>& toBeScaled, int indexNonZeroElement
) {
  ElementZmodP scale = toBeScaled[indexNonZeroElement];
  scale.invert();
  for (int i = 0; i < toBeScaled.size; i ++) {
    toBeScaled[i] *= scale;
  }
  return scale;
}

ElementZmodP ElementZmodP::operator+(const Rational& other) const {
  ElementZmodP result = *this;
  result += other;
  return result;
}

ElementZmodP ElementZmodP::operator-(const ElementZmodP& other) const {
  ElementZmodP result = *this;
  result -= other;
  return result;
}

ElementZmodP ElementZmodP::operator/(const ElementZmodP& other) const {
  ElementZmodP result = *this;
  result /= other;
  return result;
}

ElementZmodP operator*(int left, const ElementZmodP& right) {
  ElementZmodP result;
  result = right;
  result *= left;
  return result;
}

void ElementZmodP::operator*=(int other) {
  *this *= LargeInteger(other);
}

void ElementZmodP::operator*=(const LargeInteger& other) {
  this->value *= other.value;
  if (other.isNegative()) {
    this->value *= this->modulus - 1;
  }
  this->value %= this->modulus;
}

bool ElementZmodP::operator+=(const Rational& other) {
  ElementZmodP otherElement;
  otherElement.modulus = this->modulus;
  if (!otherElement.assignRational(other)) {
    return false;
  }
  *this += otherElement;
  return true;
}

void ElementZmodP::operator+=(const ElementZmodP& other) {
  if (this == &other) {
    ElementZmodP other = *this;
    *this += other;
    return;
  }
  this->checkEqualModuli(other);
  this->value += other.value;
  this->value %= this->modulus;
}

void ElementZmodP::operator-=(const ElementZmodP& other) {
  if (this == &other) {
    ElementZmodP other = *this;
    *this += other;
    return;
  }
  this->checkEqualModuli(other);
  ElementZmodP otherTimesMinusOne = other;
  otherTimesMinusOne *= - 1;
  *this += otherTimesMinusOne;
}

void ElementZmodP::operator=(const Rational& other) {
  bool mustBeTrue = this->assignRational(other);
  if (!mustBeTrue) {
    global.fatal
    << "Using ElementZmodP::operator= to assign a Rational number failed. "
    << "Operator = does not allow failure. "
    << global.fatal;
  }
}

void ElementZmodP::operator=(const int other) {
  LargeInteger otherLargeInteger = other;
  *this = otherLargeInteger;
}

void ElementZmodP::operator-=(const LargeIntegerUnsigned& other) {
  ElementZmodP otherElement;
  otherElement.modulus = this->modulus;
  otherElement = other;
  (*this) -= otherElement;
}

bool ElementZmodP::assignRational(const Rational& other) {
  this->checkIamInitialized();
  *this = other.getNumerator();
  ElementZmodP denominator;
  denominator.modulus = this->modulus;
  denominator.value = other.getDenominator();
  if (denominator.isEqualToZero()) {
    return false;
  }
  return (*this /= denominator);
}

bool ElementZmodP::operator/=(const LargeInteger& other) {
  ElementZmodP divisor;
  divisor.modulus = this->modulus;
  if (!divisor.assignRational(Rational(other))) {
    return false;
  }
  bool result = ((*this) /= divisor);
  return result;
}

bool ElementZmodP::operator/=(const ElementZmodP& other) {
  this->checkIamInitialized();
  this->checkEqualModuli(other);
  if (other.isEqualToZero()) {
    global.fatal << "Division by zero in ElementZmodP." << global.fatal;
  }
  LargeInteger inverted;
  LargeInteger otherValue;
  LargeInteger incomingModulus;
  incomingModulus = this->modulus;
  otherValue = other.value;
  if (!MathRoutines::invertXModN(otherValue, incomingModulus, inverted)) {
    return false;
  }
  *this *= inverted;
  return true;
}

