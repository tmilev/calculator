// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderRationalFunctionImplementation_already_included
#define vpfHeaderRationalFunctionImplementation_already_included

#include "math_rational_function.h"


template<class Coefficient>
bool RationalFunction<Coefficient>::convertToType(int theType) {
  MacroRegisterFunctionWithName("RationalFunction::convertToType");
  if (theType < this->expressionType) {
    return false;
  }
  if (theType == this->expressionType) {
    return true;
  }
  if (this->expressionType == this->typeConstant && this->expressionType < theType) {
    this->expressionType = this->typePolynomial;
    this->numerator.getElement().makeConstant(this->constantValue);
  }
  if (this->expressionType == this->typePolynomial && this->expressionType < theType) {
    this->expressionType = this->typeRationalFunction;
    this->denominator.getElement().makeConstant(this->constantValue.one());
  }
  return true;
}

template<class Coefficient>
void RationalFunction<Coefficient>::invert() {
  MacroRegisterFunctionWithName("RationalFunction::invert");
  if (!this->checkConsistency()) {
    global.fatal << "Inconsistent rational function. " << global.fatal;
  }
  if (this->expressionType == this->typeConstant) {
    if (this->constantValue.isEqualToZero()) {
      global.fatal << "Division by zero. "
      << "Division by zero errors must be caught earlier in the program and "
      << "handled gracefully. Crashing ungracefully. " << global.fatal;
    }
    this->constantValue.invert();
    return;
  }
  if (this->expressionType == this->typePolynomial) {
    this->convertToType(this->typeRationalFunction);
  }
  if (this->numerator.getElement().isEqualToZero()) {
    global.fatal << "Cannot invert rational function with zero numerator. " << global.fatal;
  }
  MathRoutines::swap(this->numerator.getElement(), this->denominator.getElement());
  this->expressionType = this->typeRationalFunction;
  this->reduceMemory();
  this->simplifyLeadingCoefficientOnly();
  if (!this->checkConsistency()) {
    global.fatal << "Consistency check failed. " << global.fatal;
  }
}

template<class Coefficient>
bool RationalFunction<Coefficient>::checkConsistency() const {
  if (this->expressionType == this->typePolynomial) {
    if (this->numerator.isZeroPointer()) {
      global.fatal << "A rational function is flagged as being a "
      << "non-constant polynomial, but the numerator pointer is zero. "
      << global.fatal;
      return false;
    }
    if (this->numerator.getElementConst().isConstant()) {
      global.fatal << "This is a programming error: a rational function is flagged as "
      << "having a non-constant numerator, but the numerator is constant. " << global.fatal;
      return false;
    }
  }
  if (this->expressionType == this->typeRationalFunction) {
    if (this->numerator.isZeroPointer() || this->denominator.isZeroPointer()) {
      global.fatal << "This is a programming error: a rational function is flagged as "
      << "having non-constant denominator, but either the numerator or the denominator pointer is zero. "
      << global.fatal;
      return false;
    }
    if (this->denominator.getElementConst().isConstant()) {
      global.fatal << "This is a programming error: a rational function is flagged as "
      << "having non-constant denominator, but the denominator is constant. " << global.fatal;
      return false;
    }
  }
  if (
    this->expressionType != this->typeConstant &&
    this->expressionType != this->typePolynomial &&
    this->expressionType != this->typeRationalFunction
  ) {
    global.fatal << "This is a programming error: a rational function is not initialized properly: its type is "
    << this->expressionType
    << " which is not allowed. " << global.fatal;
    return false;
  }
  return true;
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator/=(int other) {
  RationalFunction tempRF;
  tempRF.makeConstant(other);
  *this /= tempRF;
}

template<class Coefficient>
void RationalFunction<Coefficient>::negate() {
  this->operator*= (Rational(- 1));
  if (!this->checkConsistency()) {
    global.fatal << "Failed to take the negative sign of a rational function. " << global.fatal;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator-=(const RationalFunction<Coefficient>& other) {
  if (!this->checkConsistency()) {
    global.fatal << "Corrupt rational function in operator -=. " << global.fatal;
  }
  if (!other.checkConsistency()) {
    global.fatal << "Corrupt other rational function in operator -=. " << global.fatal;
  }
  RationalFunction<Coefficient> tempRF;
  tempRF = other;
  tempRF.negate();
  this->operator+=(tempRF);
  if (!this->checkConsistency()) {
    global.fatal << "Corrupt output in rational function operator -=." << global.fatal;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator-=(const Coefficient& other) {
  if (!this->checkConsistency()) {
    global.fatal << "Corrupt rational function in operator-=(Rational). " << global.fatal;
  }
  RationalFunction tempRF;
  tempRF.makeConstant(other);
  tempRF.negate();
  this->operator+=(tempRF);
  if (!(this->checkConsistency())) {
    global.fatal << "Corrupt output in rational function operator-=(Rational)." << global.fatal;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::makeOne() {
  this->makeConstant(1);
}

template<class Coefficient>
void RationalFunction<Coefficient>::makeZero() {
  this->expressionType = this->typeConstant;
  this->constantValue.makeZero();
  this->numerator.freeMemory();
  this->denominator.freeMemory();
  if (!this->checkConsistency()) {
    global.fatal << "makeZero produced corrupt output in rational function old. " << global.fatal;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator+=(int theConstant) {
  RationalFunction tempRF;
  tempRF.makeConstant(Rational(theConstant));
  (*this) += tempRF;
}

template<class Coefficient>
Rational RationalFunction<Coefficient>::rationalValue() const {
  switch(this->expressionType) {
    case RationalFunction::typeConstant:
      return this->constantValue;
    case RationalFunction::typeError:
      return 0;
    default:
      return this->numerator.getElementConst().getConstantTerm();
  }
}

template<class Coefficient>
RationalFunction<Coefficient>::RationalFunction() {
  this->expressionType = this->typeError;
}

template<class Coefficient>
RationalFunction<Coefficient>::RationalFunction(int other) {
  this->expressionType = this->typeConstant;
  this->operator=(other);
}

template<class Coefficient>
RationalFunction<Coefficient>::RationalFunction(const Rational& other) {
  this->expressionType = this->typeConstant;
  this->operator=(other);
}

template<class Coefficient>
RationalFunction<Coefficient>::RationalFunction(const RationalFunction<Rational>& other): expressionType(RationalFunction::typeError) {
  this->operator=(other);
}

template<class Coefficient>
RationalFunction<Coefficient> RationalFunction<Coefficient>::zero() {
  return RationalFunction<Coefficient>::zeroStatic();
}

template<class Coefficient>
RationalFunction<Coefficient> RationalFunction<Coefficient>::zeroStatic() {
  RationalFunction tempRat;
  tempRat.makeZero();
  return tempRat;
}

template<class Coefficient>
RationalFunction<Coefficient> RationalFunction<Coefficient>::one() {
  RationalFunction tempRat;
  tempRat.makeConstant(1);
  return tempRat;
}

template<class Coefficient>
bool RationalFunction<Coefficient>::findOneVariableRationalRoots(List<Rational>& output) {
  if (this->expressionType == this->typeConstant) {
    output.setSize(0);
    return true;
  }
  Polynomial<Rational> tempP;
  this->getNumerator(tempP);
  return tempP.findOneVariableRationalRoots(output);
}

template<class Coefficient>
bool RationalFunction<Coefficient>::needsParenthesisForMultiplication(FormatExpressions* unused) const {
  (void) unused;
  switch (this->expressionType) {
    case RationalFunction::typeConstant:
      return false;
    case RationalFunction::typePolynomial:
      return this->numerator.getElementConst().needsParenthesisForMultiplication();
    case RationalFunction::typeRationalFunction:
      return false;
  }
  return false;
}

template<class Coefficient>
std::string RationalFunction<Coefficient>::toString(FormatExpressions* theFormat) const {
  if (this->expressionType == this->typeError) {
    return "[error]";
  }
  if (this->expressionType == this->typeConstant) {
    return this->constantValue.toString();
  }
  if (this->expressionType == this->typePolynomial) {
    return this->numerator.getElementConst().toString(theFormat);
  }
  std::stringstream out;
  bool useFrac = theFormat == nullptr ? false : theFormat->flagUseFrac;
  bool needParenthesis = false;
  if (!useFrac) {
    needParenthesis = this->numerator.getElementConst().needsParenthesisForMultiplication();
  }
  if (useFrac) {
    out << "\\frac{";
  }
  if (needParenthesis) {
    out << "(";
  }
  out << this->numerator.getElementConst().toString(theFormat);
  if (needParenthesis) {
    out << ")";
  }
  if (useFrac) {
    out << "}{";
  } else {
    out << "/(";
  }
  out << this->denominator.getElementConst().toString(theFormat);
  if (useFrac) {
    out << "}";
  } else {
    out << ")";
  }
  //out << " Num vars: " << this->minimalNumberOfVariables();
  return out.str();
}

template<class Coefficient>
void RationalFunction<Coefficient>::greatestCommonDivisor(
  const Polynomial<Coefficient>& left,
  const Polynomial<Coefficient>& right,
  Polynomial<Coefficient>& output
) {
  if (RationalFunction<Coefficient>::greatestCommonDivisorQuick(left, right, output)) {
    return;
  }
  MacroRegisterFunctionWithName("RationalFunctionOld::gcd");
  Polynomial<Coefficient> leastCommonMultipleBuffer, productBuffer, remainderBuffer;
  RationalFunction<Coefficient>::leastCommonMultiple(left, right, leastCommonMultipleBuffer);
  productBuffer = left;
  productBuffer *= right;
  productBuffer.divideBy(
    leastCommonMultipleBuffer,
    output,
    remainderBuffer,
    &MonomialP::orderForGreatestCommonDivisor()
  );
  if (!remainderBuffer.isEqualToZero() || output.isEqualToZero()) {
    global.fatal
    << "This is a programming error. <br>While computing the gcd of left = "
    << left.toString() << " <br>and right = "
    << right.toString() << " <br>I got that left * right = "
    << productBuffer.toString()
    << "<br>and that lcm(left, right) = "
    << leastCommonMultipleBuffer.toString()
    << " <br>but at the same time right * left divided by lcm (left, right) equals<br>"
    << output.toString()
    << "<br> with remainder " << remainderBuffer.toString()
    << ", which is imposible."
    << global.fatal;
  }
  output.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
}

template<class Coefficient>
void RationalFunction<Coefficient>::makeOneLetterMonomial(
  int theIndex, const Rational& theCoeff, int ExpectedNumVars
) {
  if (theIndex < 0) {
    global.fatal << "This is a programming error: I am asked to create "
    << "Monomial which has a variable of negative index " << theIndex << ". " << global.fatal;
  }
  this->expressionType = this->typePolynomial;
  ExpectedNumVars = MathRoutines::maximum(theIndex + 1, ExpectedNumVars);
  this->numerator.getElement().makeDegreeOne(ExpectedNumVars, theIndex, theCoeff);
}

template<class Coefficient>
void RationalFunction<Coefficient>::makeMonomial(int letterIndex, const Rational& power, const Rational& coefficient) {
  if (letterIndex < 0) {
    global.fatal << "I am asked to create Monomial which has a variable of negative index "
    << letterIndex << ". " << global.fatal;
  }
  this->expressionType = this->typePolynomial;
  this->numerator.getElement().makeMonomial(letterIndex, power, coefficient);
}

template<class Coefficient>
void RationalFunction<Coefficient>::setNumberOfVariablesSubstituteDeletedByOne(int newNumVars) {
  int oldNumVars = this->minimalNumberOfVariables();
  this->numerator.getElement().setNumberOfVariablesSubstituteDeletedByOne(newNumVars);
  this->denominator.getElement().setNumberOfVariablesSubstituteDeletedByOne(newNumVars);
  if (newNumVars < oldNumVars) {
    this->simplify();
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator=(const RationalFunction<Rational>& other) {
  this->expressionType = other.expressionType;
  switch (this->expressionType) {
    case RationalFunction::typeConstant:
      this->constantValue = other.constantValue;
      break;
    case RationalFunction::typePolynomial:
      if (other.numerator.isZeroPointer()) {
        global.fatal << "zero pointer in numerator of other. " << global.fatal;
      }
      this->numerator.getElement() = other.numerator.getElementConst();
      break;
    case RationalFunction::typeRationalFunction:
      if (other.numerator.isZeroPointer() || other.denominator.isZeroPointer()) {
        global.fatal << "zero pointer in numerator or denominator of other. " << global.fatal;
      }
      this->numerator.getElement() = other.numerator.getElementConst();
      this->denominator.getElement() = other.denominator.getElementConst();
      break;
    default:
      break;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::leastCommonMultiple(
  const Polynomial<Coefficient>& left,
  const Polynomial<Coefficient>& right,
  Polynomial<Coefficient>& output
) {
  if (left.isEqualToZero() || right.isEqualToZero()) {
    global.fatal << "Least common multiple allowing zero not allowed. " << global.fatal;
  }
  std::stringstream commentsOnFailure;
  bool success = Polynomial<Coefficient>::leastCommonMultiple(
    left, right, output, left.coefficients[0].one(), &commentsOnFailure
  );
  if (!success) {
    global.fatal
    << "Failure of least common multiple "
    << "computation not allowed here. " << global.fatal;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator*=(const MonomialP& other) {
  Polynomial<Rational> otherP;
  otherP.makeZero();
  otherP.addMonomial(other, 1);
  *this *= otherP;
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator*=(const Polynomial<Coefficient>& other) {
  if (other.isEqualToZero()) {
    this->makeZero();
    return;
  }
  if (this->expressionType == this->typeConstant) {
    this->convertToType(this->typePolynomial);
  }
  if (this->expressionType == this->typePolynomial) {
    this->numerator.getElement() *= other;
    this->reduceMemory();
    return;
  }
  Polynomial<Coefficient> commonDivisor, theResult, tempP;
  ProgressReport theReport;
  if (theReport.tickAndWantReport()) {
    std::stringstream out;
    out << "Multiplying " << this->toString(&global.theDefaultFormat.getElement()) << " by "
    << other.toString(&global.theDefaultFormat.getElement());
    theReport.report(out.str());
  }
  RationalFunction<Coefficient>::greatestCommonDivisor(this->denominator.getElement(), other, commonDivisor);
  this->numerator.getElement() *= other;
  List<MonomialP>::Comparator* monomialOrder = &MonomialP::orderForGreatestCommonDivisor();
  this->numerator.getElement().divideBy(commonDivisor, theResult, tempP, monomialOrder);
  if (!tempP.isEqualToZero()) {
    global.fatal << "Polynomial equal to zero not allowed here. " << global.fatal;
  }
  this->numerator.getElement() = theResult;
  this->denominator.getElement().divideBy(commonDivisor, theResult, tempP, monomialOrder);
  if (!tempP.isEqualToZero()) {
    global.fatal << "Polynomial not equal to zero. " << global.fatal;
  }
  this->denominator.getElement() = theResult;
  this->reduceMemory();
  this->simplifyLeadingCoefficientOnly();
  if (theReport.tickAndWantReport()) {
    std::stringstream out;
    out << "Multiplying " << this->toString(&global.theDefaultFormat.getElement()) << " by "
    << other.toString(&global.theDefaultFormat.getElement());
    out << " and the result is:\n" << this->toString();
    theReport.report(out.str());
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator/=(const RationalFunction<Coefficient>& other) {
  this->checkConsistency();
  RationalFunction tempRF;
  tempRF = other;
  tempRF.checkConsistency();
  tempRF.invert();
  tempRF.checkConsistency();
  *this *= tempRF;
  if (!this->checkConsistency()) {
    global.fatal << "Incosistent rational function. " << global.fatal;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator*=(const Coefficient& other) {
  if (other.isEqualToZero()) {
    this->makeZero();
    return;
  }
  switch(this->expressionType) {
    case RationalFunction::typeConstant:
      this->constantValue *= other;
      return;
    case RationalFunction::typePolynomial:
      this->numerator.getElement() *= other;
      return;
    case RationalFunction::typeRationalFunction:
      this->numerator.getElement() *= other;
      this->simplifyLeadingCoefficientOnly();
      return;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator*=(const RationalFunction<Coefficient>& other) {
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
  if (other.expressionType == this->typeConstant) {
    this->operator*=(other.constantValue);
    return;
  }
  if (other.expressionType == this->typePolynomial) {
    this->operator*=(other.numerator.getElementConst());
    return;
  }
  if (this->expressionType == this->typeConstant) {
    Coefficient constant;
    constant = this->constantValue;
    this->operator=(other);
    this->operator*=(constant);
    return;
  }
  if (this->expressionType == this->typePolynomial) {
    Polynomial<Coefficient> tempP;
    tempP = this->numerator.getElement();
    this->operator=(other);
    this->operator*=(tempP);
    return;
  }
  Polynomial<Coefficient> theGCD1, theGCD2, tempP1, tempP2;
  ProgressReport theReport;
  if (theReport.tickAndWantReport()) {
    std::stringstream out;
    out << "Multiplying " << this->toString() << " by " << other.toString();
    theReport.report(out.str());
  }
  RationalFunction<Coefficient>::greatestCommonDivisor(other.denominator.getElementConst(), this->numerator.getElement(), theGCD1);
  RationalFunction<Coefficient>::greatestCommonDivisor(this->denominator.getElement(), other.numerator.getElementConst(), theGCD2);
  List<MonomialP>::Comparator* monomialOrder = &MonomialP::orderForGreatestCommonDivisor();
  this->numerator.getElement().divideBy(theGCD1, tempP1, tempP2, monomialOrder);
  this->numerator.getElement() = tempP1;
  if (!tempP2.isEqualToZero()) {
    global.fatal << "Polynomial equal to zero not allowed here. " << global.fatal;
  }
  other.denominator.getElementConst().divideBy(theGCD1, tempP1, tempP2, monomialOrder);
  if (!tempP2.isEqualToZero()) {
    global.fatal << "Polynomial must not be zero here. " << global.fatal;
  }
  this->denominator.getElement() *= tempP1;
  this->denominator.getElement().divideBy(theGCD2, tempP1, tempP2, monomialOrder);
  if (!tempP2.isEqualToZero()) {
    global.fatal << "Polynomial must not be zero here. " << global.fatal;
  }
  this->denominator.getElement() = tempP1;
  other.numerator.getElementConst().divideBy(theGCD2, tempP1, tempP2, monomialOrder);
  if (!tempP2.isEqualToZero()) {
    global.fatal << "Polynomial must not be zero here. " << global.fatal;
  }
  this->numerator.getElement() *= tempP1;
  this->reduceMemory();
  this->simplifyLeadingCoefficientOnly();
  if (theReport.tickAndWantReport()) {
    std::stringstream out;
    out << "Multiplying " << this->toString() << " by " << other.toString();
    out << " and the result is:\n" << this->toString();
    theReport.report(out.str());
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator+=(const RationalFunction<Coefficient>& other) {
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
    RationalFunction tempRF;
    tempRF = other;
    tempRF.convertToType(this->expressionType);
    this->addSameTypes(tempRF);
    if (!this->checkConsistency()) {
      global.fatal << "Output of rational function addition is corrupt. " << global.fatal;
    }
    return;
  }
  if (this->expressionType == other.expressionType) {
    this->addSameTypes(other);
    if (!this->checkConsistency()) {
      global.fatal << "Output of rational function addition is corrupt "
      << "for same type of rational functions." << global.fatal;
    }
    return;
  }
  if (this->expressionType < other.expressionType) {
    this->convertToType(other.expressionType);
    this->addSameTypes(other);
    if (!this->checkConsistency()) {
      global.fatal << "Corrupt outputs of rational "
      << "function addition, different expression types. "
      << global.fatal;
    }
  }
  if (!this->checkConsistency()) {
    global.fatal <<  "Corrupt final output of rational function addition. " << global.fatal;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::simplify() {
  MacroRegisterFunctionWithName("RationalFunction::simplify");
  List<MonomialP>::Comparator* monomialOrder = &MonomialP::orderForGreatestCommonDivisor();
  if (this->expressionType == this->typeRationalFunction) {
    if (!this->numerator.getElement().isEqualToZero()) {
      Polynomial<Coefficient> theGCD, tempP, tempP2;
      this->greatestCommonDivisor(this->numerator.getElement(), this->denominator.getElement(), theGCD);
      if (theGCD.isEqualToZero()) {
        global.fatal << "This is a programing error: "
        << " while fetching the gcd of " << this->numerator.getElement().toString()
        << " and " << this->denominator.getElement().toString()
        << " I got 0, which is impossible. " << global.fatal;
      }
      this->numerator.getElement().divideBy(theGCD, tempP, tempP2, monomialOrder);
      this->numerator.getElement() = tempP;
      this->denominator.getElement().divideBy(theGCD, tempP, tempP2, monomialOrder);
      this->denominator.getElement() = tempP;
    }
  }
  this->reduceMemory();
  this->simplifyLeadingCoefficientOnly();
}

template<class Coefficient>
Rational RationalFunction<Coefficient>::scaleToIntegral() {
  if (this->isEqualToZero()) {
    return Rational::one();
  }
  if (this->expressionType == this->typeConstant) {
    Rational result = this->constantValue;
    result.invert();
    this->constantValue.makeOne();
    return result;
  }
  if (this->expressionType == this->typePolynomial) {
    return this->numerator.getElement().scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  }
  if (this->expressionType != this->typeRationalFunction) {
    return Rational::one();
  }
  Rational result;
  result = this->numerator.getElement().scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  result /= this->denominator.getElement().scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  return result;
}

template<class Coefficient>
void RationalFunction<Coefficient>::simplifyLeadingCoefficientOnly() {
  if (this->expressionType != this->typeRationalFunction) {
    return;
  }
  Coefficient scaleNumerator = this->numerator.getElement().scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  Coefficient scaleDenominator = this->denominator.getElement().scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  Coefficient scale = scaleDenominator / scaleNumerator;
  this->denominator.getElement() *= scale.getDenominator();
  this->numerator.getElement() *= scale.getNumerator();
}

template<class Coefficient>
bool RationalFunction<Coefficient>::isConstant(Rational* whichConstant) const {
  if (this->expressionType != this->typeConstant) {
    return false;
  }
  if (whichConstant != nullptr) {
    *whichConstant = this->constantValue;
  }
  return true;
}

template<class Coefficient>
bool RationalFunction<Coefficient>::isInteger() const {
  return this->expressionType == this->typeConstant && this->constantValue.isInteger();
}

template<class Coefficient>
bool RationalFunction<Coefficient>::isSmallInteger(int* whichInteger) const {
  return this->expressionType == this->typeConstant &&
  this->constantValue.isSmallInteger(whichInteger);
}

template<class Coefficient>
RationalFunction<Coefficient> RationalFunction<Coefficient>::scaleNormalizeIndex(
  List<RationalFunction<Rational> >& input, int indexNonZeroElement
) {
  if (input.size == 0) {
    return 1;
  }
  List<Rational> scales;
  Rational scale;
  Polynomial<Rational> currentNumerator, currentDenominator;
  for (int i = 0; i < input.size; i ++) {
    RationalFunction<Rational>& current = input[i];
    current.getNumerator(currentNumerator);
    current.getDenominator(currentDenominator);
    scale = currentNumerator.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
    scale /= currentDenominator.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
    scales.addOnTop(scale);
  }
  LargeIntegerUnsigned numeratorContentGreatestCommonDivisor = scales[0].getNumerator().value;
  LargeIntegerUnsigned denominatorContentLeastCommonMultiple = scales[0].getDenominator();
  for (int i = 1; i < scales.size; i ++) {
    numeratorContentGreatestCommonDivisor = LargeIntegerUnsigned::leastCommonMultiple(
      scales[i].getNumerator().value,
      numeratorContentGreatestCommonDivisor
    );
    denominatorContentLeastCommonMultiple = LargeIntegerUnsigned::greatestCommonDivisor(
      denominatorContentLeastCommonMultiple,
      scales[i].getDenominator()
    );
  }
  scale = numeratorContentGreatestCommonDivisor;
  scale /= denominatorContentLeastCommonMultiple;
  if (scales[indexNonZeroElement] < 0) {
    scale *= -1;
  }
  for (int i = 0; i < input.size; i ++) {
    input[i] *= scale;
  }
  return scale;
}

template<class Coefficient>
bool RationalFunction<Coefficient>::greatestCommonDivisorQuick(
  const Polynomial<Coefficient>& left,
  const Polynomial<Coefficient>& right,
  Polynomial<Coefficient>& output
) {
  if (left.totalDegree() > 1 && right.totalDegree() > 1) {
    return false;
  }
  if (left.isEqualToZero()) {
    global.fatal << "Greatest common divisor involving zero not allowed. " << global.fatal;
  }
  Polynomial<Coefficient> quotient, remainder;
  List<MonomialP>::Comparator* monomialOrder = &MonomialP::orderDefault();
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

template<class Coefficient>
void RationalFunction<Coefficient>::raiseToPower(int thePower) {
  MacroRegisterFunctionWithName("RationalFunctionOld::raiseToPower");
  this->checkConsistency();
  if (thePower < 0) {
    this->invert();
    thePower = - thePower;
  }
  if (thePower == 0) {
    this->makeOne();
    return;
  }
  switch (this->expressionType) {
    case RationalFunction::typeConstant:
      MathRoutines::raiseToPower(this->constantValue, thePower, this->constantValue.one());
      break;
    case RationalFunction::typePolynomial:
      this->numerator.getElement().raiseToPower(thePower, this->constantValue.one());
      break;
    case RationalFunction::typeRationalFunction:
      this->numerator.getElement().raiseToPower(thePower, this->constantValue.one());
      this->denominator.getElement().raiseToPower(thePower, this->constantValue.one());
      break;
  }
  this->checkConsistency();
}

template<class Coefficient>
void RationalFunction<Coefficient>::clearDenominators(RationalFunction<Coefficient>& outputWasMultipliedBy) {
  //outputWasMultipliedBy.makeConstant(this->NumVars, (Rational) 1, this->context);
  Rational tempRat;
  switch(this->expressionType) {
    case RationalFunction::typeConstant:
      tempRat = this->constantValue.getDenominator();
      outputWasMultipliedBy.makeConstant(tempRat);
      this->constantValue *= tempRat;
    break;
    case RationalFunction::typePolynomial:
      this->numerator.getElement().clearDenominators(tempRat);
      outputWasMultipliedBy.makeConstant(tempRat);
    break;
    case RationalFunction::typeRationalFunction:
      RationalFunction tempRF;
      outputWasMultipliedBy.operator=(this->denominator.getElement());
      *this *= outputWasMultipliedBy;
      this->clearDenominators(tempRF);
      outputWasMultipliedBy *= tempRF;
      break;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::addSameTypes(const RationalFunction<Rational>& other) {
  switch (this->expressionType) {
    case RationalFunction::typeConstant:
      this->constantValue += other.constantValue;
      break;
    case RationalFunction::typePolynomial:
      this->numerator.getElement() += other.numerator.getElementConst();
      break;
    case RationalFunction::typeRationalFunction:
      this->addHonestRationalFunction(other);
      break;
  }
  this->reduceMemory();
  if (!this->checkConsistency()) {
    global.fatal << "Bad rational function. " << global.fatal;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::reducePolynomialToRational() {
  if (this->expressionType == this->typePolynomial) {
    if (this->numerator.getElement().isConstant()) {
      this->expressionType = this->typeConstant;
      if (this->numerator.getElement().isEqualToZero()) {
        this->constantValue.makeZero();
      } else {
        this->constantValue = this->numerator.getElement().coefficients[0];
      }
      this->numerator.freeMemory();
    }
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::addHonestRationalFunction(const RationalFunction<Rational>& other) {
  MacroRegisterFunctionWithName("RationalFunctionOld::addHonestRationalFunction");
  Rational tempRat;
  if (!this->denominator.getElement().isProportionalTo(other.denominator.getElementConst(), tempRat, Rational(1))) {
    Polynomial<Rational> buffer;
    buffer = this->denominator.getElement();
    this->numerator.getElement() *= other.denominator.getElementConst();
    buffer *= other.numerator.getElementConst();
    this->numerator.getElement() += buffer;
    this->denominator.getElement() *= other.denominator.getElementConst();
    if (this->denominator.getElement().isEqualToZero())
      global.fatal << "Denominator of rational function is zero." << global.fatal;
    this->simplify();
  } else {
    this->numerator.getElement() *= tempRat;
    this->denominator.getElement() *= tempRat;
    this->numerator.getElement() += other.numerator.getElementConst();
    this->reduceMemory();
    this->simplifyLeadingCoefficientOnly();
  }
  if (!this->checkConsistency()) {
    global.fatal << "Inconsistent rational function. " << global.fatal;
  }
}

template<class Coefficient>
bool RationalFunction<Coefficient>::isEqualTo(const RationalFunction<Coefficient>& other) const {
  if (this->expressionType != other.expressionType) {
    return false;
  }
  switch (this->expressionType) {
    case RationalFunction::typeRationalFunction:
      return this->numerator.getElementConst().isEqualTo(other.numerator.getElementConst()) &&
      this->denominator.getElementConst().isEqualTo(other.denominator.getElementConst());
    case RationalFunction::typePolynomial:
      return this->numerator.getElementConst().isEqualTo(other.numerator.getElementConst());
    case RationalFunction::typeConstant:
      return this->constantValue == other.constantValue;
  }
  global.fatal << "This line of code is supposed to be unreachable. " << global.fatal;
  return false;
}

template<class Coefficient>
void RationalFunction<Coefficient>::multiplyByConstant(const Rational& theConst) {
  this->operator*=(theConst);
}

template<class Coefficient>
void RationalFunction<Coefficient>::reduceMemory() {
  this->reduceRationalFunctionToPolynomial();
  this->reducePolynomialToRational();
  if (!this->checkConsistency()) {
    global.fatal << "Corrupt rational function. " << global.fatal;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator=(const Polynomial<Coefficient>& other) {
  this->expressionType = this->typePolynomial;
  this->numerator.getElement() = other;
  if (!other.isEqualToZero()) {
    // Coefficients of types such as ElementZModP may have
    // their zero constants not known at compile time.
    // We therefore initialize a zero constant here.
    this->constantValue = other.coefficients[0].zero();
  }
  this->reduceMemory();
}

template<class Coefficient>
unsigned int RationalFunction<Coefficient>::hashFunction() const {
  switch (this->expressionType) {
    case RationalFunction::typeConstant:
      return this->constantValue.hashFunction();
    case RationalFunction::typePolynomial:
      return this->numerator.getElementConst().hashFunction();
    case RationalFunction::typeRationalFunction:
      return this->numerator.getElementConst().hashFunction() * someRandomPrimes[0] +
      this->denominator.getElementConst().hashFunction() * someRandomPrimes[1];
    default:
      return static_cast<unsigned int>(- 1);
  }
}

template<class Coefficient>
unsigned int RationalFunction<Coefficient>::hashFunction(const RationalFunction<Coefficient>& input) {
  return input.hashFunction();
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator=(int other) {
  this->makeConstant(other);
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator=(const Rational& other) {
  this->makeConstant(other);
}

template<class Coefficient>
void RationalFunction<Coefficient>::setNumberOfVariables(int goalNumVars) {
  this->setNumberOfVariablesSubstituteDeletedByOne(goalNumVars);
}

template<class Coefficient>
void RationalFunction<Coefficient>::getNumerator(Polynomial<Coefficient>& output) const {
  switch (this->expressionType) {
    case RationalFunction::typeConstant:
      output.makeConstant(this->constantValue);
      return;
    default:
      output = this->numerator.getElementConst();
      return;
  }
}

template<class Coefficient>
bool RationalFunction<Coefficient>::isNegative() {
  if (this->expressionType == this->typeConstant) {
    return this->constantValue.isNegative();
  }
  return false;
}

template<class Coefficient>
void RationalFunction<Coefficient>::getDenominator(Polynomial<Coefficient>& output) const {
  switch (this->expressionType) {
    case RationalFunction::typeRationalFunction:
      if (this->denominator.isZeroPointer()) {
        global.fatal << "The rational function is "
        << "supposed to be honest, but the denominator pointer is zero. " << global.fatal;
      }
      output = this->denominator.getElementConst();
      return;
    default:
      output.makeConstant(Rational(1));
      return;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator+=(const Polynomial<Coefficient>& other) {
  RationalFunction tempOther;
  tempOther = other;
  *this += tempOther;
}

template<class Coefficient>
bool RationalFunction<Coefficient>::operator==(int other) const {
  if (other == 0) {
    return this->isEqualToZero();
  } else {
    return this->expressionType == this->typeConstant && (this->constantValue == other);
  }
}

template<class Coefficient>
bool RationalFunction<Coefficient>::operator!=(int other) {
  return !(this->operator==(other));
}

template<class Coefficient>
bool RationalFunction<Coefficient>::operator==(const RationalFunction<Coefficient>& other) const {
  return this->isEqualTo(other);
}

template<class Coefficient>
bool RationalFunction<Coefficient>::operator>(const RationalFunction<Rational>& other) const {
  if (this->expressionType < other.expressionType) {
    return false;
  }
  if (this->expressionType > other.expressionType) {
    return true;
  }
  switch (this->expressionType) {
    case RationalFunction::typeConstant:
      return this->constantValue > other.constantValue;
    case RationalFunction::typePolynomial:
      return this->numerator.getElementConst() > other.numerator.getElementConst();
    case RationalFunction::typeRationalFunction:
      if (other.denominator.getElementConst() > this->denominator.getElementConst()) {
        return true;
      }
      if (this->denominator.getElementConst() > other.denominator.getElementConst()) {
        return false;
      }
      return this->numerator.getElementConst() > other.numerator.getElementConst();
    default:
      return false;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator*=(int other) {
  *this *= Rational(other);
}

template<class Coefficient>
bool RationalFunction<Coefficient>::operator<(const RationalFunction<Rational>& other) const {
  return other > *this;
}

template<class Coefficient>
bool RationalFunction<Coefficient>::operator<=(const RationalFunction<Rational>& other) const {
  if (*this == other) {
    return true;
  }
  return other > *this;
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator/=(const Polynomial<Coefficient>& other) {
  RationalFunction<Coefficient> otherInverted;
  otherInverted = other;
  otherInverted.invert();
  *this *= otherInverted;
  if (!this->checkConsistency()) {
    global.fatal << "Bad rational function. " << global.fatal;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::reduceRationalFunctionToPolynomial() {
  if (this->expressionType != this->typeRationalFunction) {
    return;
  }
  if (this->denominator.getElement().isConstant()) {
    this->numerator.getElement() /= this->denominator.getElement().coefficients[0];
    this->denominator.freeMemory();
    this->expressionType = this->typePolynomial;
  }
  if (this->numerator.getElement().isEqualToZero()) {
    this->makeZero();
  }
}

template<class Coefficient>
bool RationalFunction<Coefficient>::substitution(
  const PolynomialSubstitution<Rational>& substitution,
  const Coefficient& one
) {
  return this->substitution(substitution, one, nullptr);
}

template<class Coefficient>
bool RationalFunction<Coefficient>::substitution(
  const PolynomialSubstitution<Rational>& substitution,
  const Coefficient& one,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("RationalFunction::substitution");
  switch(this->expressionType) {
    case RationalFunction::typeConstant:
      return true;
    case RationalFunction::typePolynomial:
      if (!this->numerator.getElement().substitution(substitution, one)) {
        return false;
      }
      this->simplify();
      return true;
    case RationalFunction::typeRationalFunction:
      if (!this->numerator.getElement().substitution(substitution, one)) {
        return false;
      }
      if (!this->denominator.getElement().substitution(substitution, one)) {
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
      global.fatal << "Default case not allowed. " << global.fatal;
      break;
  }
  return false;
}

template<class Coefficient>
int RationalFunction<Coefficient>::minimalNumberOfVariables() const {
  switch (this->expressionType) {
    case RationalFunction::typeConstant:
      return 0;
    case RationalFunction::typePolynomial:
      return this->numerator.getElementConst().minimalNumberOfVariables();
    case RationalFunction::typeRationalFunction:
      return MathRoutines::maximum(
        this->numerator.getElementConst().minimalNumberOfVariables(),
        this->denominator.getElementConst().minimalNumberOfVariables()
      );
    default: //this should never happen! maybe global.fatal << global.fatal here...
      return - 1;
  }
}

template<class Coefficient>
bool RationalFunction<Coefficient>::getRelations(
  const List<Polynomial<Rational> >& inputElements,
  List<Polynomial<Rational> >& outputGeneratorLabels,
  List<Polynomial<Rational> >& outputRelations,
  std::stringstream& comments
) {
  MacroRegisterFunctionWithName("RationalFunctionOld::GetRelationsGetRelations");
  outputGeneratorLabels.setSize(inputElements.size);
  outputRelations.setSize(0);
  if (inputElements.size == 0) {
    return true;
  }
  List<Polynomial<Rational> > theGroebnerBasis;
  theGroebnerBasis = inputElements;
  int numStartingGenerators = inputElements.size;
  int numStartingVariables = 0;
  for (int i = 0; i < inputElements.size; i ++) {
    numStartingVariables = MathRoutines::maximum(numStartingVariables, inputElements[0].minimalNumberOfVariables());
  }
  Polynomial<Rational> currentGenerator;
  for (int i = 0; i < numStartingGenerators; i ++) {
    Polynomial<Rational>& currentPoly = theGroebnerBasis[i];
    currentPoly.setNumberOfVariablesSubstituteDeletedByOne(numStartingVariables + numStartingGenerators);
    currentGenerator.makeDegreeOne(numStartingVariables + numStartingGenerators, i + numStartingVariables, 1);
    outputGeneratorLabels[i] = currentGenerator;
    currentPoly -= currentGenerator;
  }
  GroebnerBasisComputation<Rational> theComputation;
  theComputation.thePolynomialOrder.monomialOrder.setComparison(
    MonomialP::greaterThan_leftLargerWins
  );
  if (!theComputation.transformToReducedGroebnerBasis(theGroebnerBasis)) {
    comments << "Failed to find Groebner basis";
    return false;
  }
  outputRelations.reserve(theGroebnerBasis.size);
  outputRelations.setSize(0);
  for (int i = 0; i < theGroebnerBasis.size; i ++) {
    Polynomial<Rational>& currentPoly = theGroebnerBasis[i];
    bool bad = false;
    for (int j = 0; j < numStartingVariables; j ++) {
      if (currentPoly.getMaximumPowerOfVariableIndex(j) > 0) {
        bad = true;
        break;
      }
    }
    if (!bad) {
      outputRelations.addOnTop(currentPoly);
    }
  }
  return true;
}
#endif
