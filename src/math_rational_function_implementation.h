// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderRationalFunctionImplementation_already_included
#define vpfHeaderRationalFunctionImplementation_already_included

#include "math_rational_function.h"


template<class Coefficient>
bool RationalFunction<Coefficient>::convertToType(int theType) {
  if (theType < this->expressionType) {
    return false;
  }
  if (theType == this->expressionType) {
    return true;
  }
  if (this->expressionType == this->typeRational && this->expressionType < theType) {
    this->expressionType = this->typePoly;
    this->Numerator.getElement().makeConstant(this->ratValue);
  }
  if (this->expressionType == this->typePoly && this->expressionType < theType) {
    this->expressionType = this->typeRationalFunction;
    this->Denominator.getElement().makeConstant(1);
  }
  return true;
}

template<class Coefficient>
void RationalFunction<Coefficient>::invert() {
  if (!this->checkConsistency()) {
    global.fatal << "Inconsistent rational functoin. " << global.fatal;
  }
  if (this->expressionType == this->typeRational) {
    if (this->ratValue.isEqualToZero()) {
      global.fatal << "Division by zero. "
      << "Division by zero errors must be caught earlier in the program and "
      << "handled gracefully. Crashing ungracefully. " << global.fatal;
    }
    this->ratValue.invert();
    return;
  }
  if (this->expressionType == this->typePoly) {
    this->convertToType(this->typeRationalFunction);
  }
  if (this->Numerator.getElement().isEqualToZero()) {
    global.fatal << "Cannot invert rational function with zero numerator. " << global.fatal;
  }
  MathRoutines::swap(this->Numerator.getElement(), this->Denominator.getElement());
  this->expressionType = this->typeRationalFunction;
  this->reduceMemory();
  this->simplifyLeadingCoefficientOnly();
  if (!this->checkConsistency()) {
    global.fatal << "Consistency check failed. " << global.fatal;
  }
}

template<class Coefficient>
bool RationalFunction<Coefficient>::checkConsistency() const {
  if (this->expressionType == this->typePoly) {
    if (this->Numerator.isZeroPointer()) {
      global.fatal << "A rational function is flagged as being a "
      << "non-constant polynomial, but the numerator pointer is zero. "
      << global.fatal;
      return false;
    }
    if (this->Numerator.getElementConst().isConstant()) {
      global.fatal << "This is a programming error: a rational function is flagged as "
      << "having a non-constant numerator, but the numerator is constant. " << global.fatal;
      return false;
    }
  }
  if (this->expressionType == this->typeRationalFunction) {
    if (this->Numerator.isZeroPointer() || this->Denominator.isZeroPointer()) {
      global.fatal << "This is a programming error: a rational function is flagged as "
      << "having non-constant denominator, but either the numerator or the denominator pointer is zero. "
      << global.fatal;
      return false;
    }
    if (this->Denominator.getElementConst().isConstant()) {
      global.fatal << "This is a programming error: a rational function is flagged as "
      << "having non-constant denominator, but the denominator is constant. " << global.fatal;
      return false;
    }
  }
  if (
    this->expressionType != this->typeRational &&
    this->expressionType != this->typePoly &&
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
void RationalFunction<Coefficient>::minus() {
  this->operator*= (Rational(- 1));
  if (!this->checkConsistency()) {
    global.fatal << "Failed to take the negative sign of a rational function. " << global.fatal;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator-=(const RationalFunction<Rational>& other) {
  if (!this->checkConsistency()) {
    global.fatal << "Corrupt rational function in operator -=. " << global.fatal;
  }
  if (!other.checkConsistency()) {
    global.fatal << "Corrupt other rational function in operator -=. " << global.fatal;
  }
  RationalFunction tempRF;
  tempRF = other;
  tempRF.minus();
  this->operator+=(tempRF);
  if (!this->checkConsistency()) {
    global.fatal << "Corrupt output in rational function operator -=." << global.fatal;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator-=(const Rational& other) {
  if (!this->checkConsistency()) {
    global.fatal << "Corrupt rational function in operator-=(Rational). " << global.fatal;
  }
  RationalFunction tempRF;
  tempRF.makeConstant(other);
  tempRF.minus();
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
  this->expressionType = this->typeRational;
  this->ratValue.makeZero();
  this->Numerator.freeMemory();
  this->Denominator.freeMemory();
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
Rational RationalFunction<Coefficient>::RationalValue() const {
  switch(this->expressionType) {
    case RationalFunction::typeRational:
      return this->ratValue;
    case RationalFunction::typeError:
      return 0;
    default:
      return this->Numerator.getElementConst().GetConstantTerm();
  }
}

template<class Coefficient>
RationalFunction<Coefficient>::RationalFunction() {
  this->expressionType = this->typeError;
  this->ratValue.makeZero();
}

template<class Coefficient>
RationalFunction<Coefficient>::RationalFunction(int other) {
  this->expressionType = this->typeRational;
  this->operator=(other);
}

template<class Coefficient>
RationalFunction<Coefficient>::RationalFunction(const Rational& other) {
  this->expressionType = this->typeRational;
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
  if (this->expressionType == this->typeRational) {
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
    case RationalFunction::typeRational:
      return false;
    case RationalFunction::typePoly:
      return this->Numerator.getElementConst().needsParenthesisForMultiplication();
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
  if (this->expressionType == this->typeRational) {
    return this->ratValue.toString();
  }
  if (this->expressionType == this->typePoly) {
    return this->Numerator.getElementConst().toString(theFormat);
  }
  std::stringstream out;
  bool useFrac = theFormat == nullptr ? false : theFormat->flagUseFrac;
  bool needParenthesis = false;
  if (!useFrac) {
    needParenthesis = this->Numerator.getElementConst().needsParenthesisForMultiplication();
  }
  if (useFrac) {
    out << "\\frac{";
  }
  if (needParenthesis) {
    out << "(";
  }
  out << this->Numerator.getElementConst().toString(theFormat);
  if (needParenthesis) {
    out << ")";
  }
  if (useFrac) {
    out << "}{";
  } else {
    out << "/(";
  }
  out << this->Denominator.getElementConst().toString(theFormat);
  if (useFrac) {
    out << "}";
  } else {
    out << ")";
  }
  //out << " Num vars: " << this->minimalNumberOfVariables();
  return out.str();
}

template<class Coefficient>
void RationalFunction<Coefficient>::gcd(
  const Polynomial<Rational>& left,
  const Polynomial<Rational>& right,
  Polynomial<Rational>& output
) {
  if (RationalFunction<Coefficient>::gcdQuick(left, right, output)) {
    return;
  }
  MacroRegisterFunctionWithName("RationalFunctionOld::gcd");
  Polynomial<Rational> leastCommonMultipleBuffer, productBuffer, remainderBuffer;
  RationalFunction<Coefficient>::lcm(left, right, leastCommonMultipleBuffer);
  productBuffer = left;
  productBuffer *= right;
  productBuffer.DivideBy(
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
  this->expressionType = this->typePoly;
  ExpectedNumVars = MathRoutines::maximum(theIndex + 1, ExpectedNumVars);
  this->Numerator.getElement().makeDegreeOne(ExpectedNumVars, theIndex, theCoeff);
}

template<class Coefficient>
void RationalFunction<Coefficient>::makeMonomial(int letterIndex, const Rational& power, const Rational& coefficient) {
  if (letterIndex < 0) {
    global.fatal << "I am asked to create Monomial which has a variable of negative index "
    << letterIndex << ". " << global.fatal;
  }
  this->expressionType = this->typePoly;
  this->Numerator.getElement().makeMonomial(letterIndex, power, coefficient);
}

template<class Coefficient>
void RationalFunction<Coefficient>::setNumberOfVariablesSubstituteDeletedByOne(int newNumVars) {
  int oldNumVars = this->minimalNumberOfVariables();
  this->Numerator.getElement().setNumberOfVariablesSubstituteDeletedByOne(newNumVars);
  this->Denominator.getElement().setNumberOfVariablesSubstituteDeletedByOne(newNumVars);
  if (newNumVars < oldNumVars) {
    this->simplify();
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator=(const RationalFunction<Rational>& other) {
  this->expressionType = other.expressionType;
  switch (this->expressionType) {
    case RationalFunction::typeRational:
      this->ratValue = other.ratValue;
      break;
    case RationalFunction::typePoly:
      if (other.Numerator.isZeroPointer()) {
        global.fatal << "zero pointer in numerator of other. " << global.fatal;
      }
      this->Numerator.getElement() = other.Numerator.getElementConst();
      break;
    case RationalFunction::typeRationalFunction:
      if (other.Numerator.isZeroPointer() || other.Denominator.isZeroPointer()) {
        global.fatal << "zero pointer in numerator or denominator of other. " << global.fatal;
      }
      this->Numerator.getElement() = other.Numerator.getElementConst();
      this->Denominator.getElement() = other.Denominator.getElementConst();
      break;
    default:
      break;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::lcm(
  const Polynomial<Rational>& left,
  const Polynomial<Rational>& right,
  Polynomial<Rational>& output
) {
  std::stringstream commentsOnFailure;
  bool success = Polynomial<Rational>::leastCommonMultiple(
    left, right, output, Rational::one(), &commentsOnFailure
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
void RationalFunction<Coefficient>::operator*=(const Polynomial<Rational>& other) {
  if (other.isEqualToZero()) {
    this->makeZero();
    return;
  }
  if (this->expressionType == this->typeRational) {
    this->convertToType(this->typePoly);
  }
  if (this->expressionType == this->typePoly) {
    this->Numerator.getElement() *= other;
    this->reduceMemory();
    return;
  }
  Polynomial<Rational> theGCD, theResult, tempP;
  ProgressReport theReport;
  if (theReport.tickAndWantReport()) {
    std::stringstream out;
    out << "Multiplying " << this->toString(&global.theDefaultFormat.getElement()) << " by "
    << other.toString(&global.theDefaultFormat.getElement());
    theReport.report(out.str());
  }
  RationalFunction<Coefficient>::gcd(this->Denominator.getElement(), other, theGCD);
  this->Numerator.getElement() *= other;
  List<MonomialP>::Comparator* monomialOrder = &MonomialP::orderForGreatestCommonDivisor();
  this->Numerator.getElement().DivideBy(theGCD, theResult, tempP, monomialOrder);
  if (!tempP.isEqualToZero()) {
    global.fatal << "Polynomial equal to zero not allowed here. " << global.fatal;
  }
  this->Numerator.getElement() = theResult;
  this->Denominator.getElement().DivideBy(theGCD, theResult, tempP, monomialOrder);
  if (!tempP.isEqualToZero()) {
    global.fatal << "Polynomial not equal to zero. " << global.fatal;
  }
  this->Denominator.getElement() = theResult;
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
void RationalFunction<Coefficient>::operator/=(const RationalFunction<Rational>& other) {
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
void RationalFunction<Coefficient>::operator*=(const Rational& other) {
  if (other.isEqualToZero()) {
    this->makeZero();
    return;
  }
  switch(this->expressionType) {
    case RationalFunction::typeRational:
      this->ratValue *= other;
      return;
    case RationalFunction::typePoly:
      this->Numerator.getElement() *= other;
      return;
    case RationalFunction::typeRationalFunction:
      this->Numerator.getElement() *= other;
      this->simplifyLeadingCoefficientOnly();
      return;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator*=(const RationalFunction<Rational>& other) {
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
  if (other.expressionType == this->typeRational) {
    this->operator*=(other.ratValue);
    return;
  }
  if (other.expressionType == this->typePoly) {
    this->operator*=(other.Numerator.getElementConst());
    return;
  }
  if (this->expressionType == this->typeRational) {
    Rational tempRat;
    tempRat = this->ratValue;
    this->operator=(other);
    this->operator*=(tempRat);
    return;
  }
  if (this->expressionType == this->typePoly) {
    Polynomial<Rational>  tempP;
    tempP = this->Numerator.getElement();
    this->operator=(other);
    this->operator*=(tempP);
    return;
  }
  Polynomial<Rational> theGCD1, theGCD2, tempP1, tempP2;
  ProgressReport theReport;
  if (theReport.tickAndWantReport()) {
    std::stringstream out;
    out << "Multiplying " << this->toString() << " by " << other.toString();
    theReport.report(out.str());
  }
  RationalFunction<Coefficient>::gcd(other.Denominator.getElementConst(), this->Numerator.getElement(), theGCD1);
  RationalFunction<Coefficient>::gcd(this->Denominator.getElement(), other.Numerator.getElementConst(), theGCD2);
  List<MonomialP>::Comparator* monomialOrder = &MonomialP::orderForGreatestCommonDivisor();
  this->Numerator.getElement().DivideBy(theGCD1, tempP1, tempP2, monomialOrder);
  this->Numerator.getElement() = tempP1;
  if (!tempP2.isEqualToZero()) {
    global.fatal << "Polynomial equal to zero not allowed here. " << global.fatal;
  }
  other.Denominator.getElementConst().DivideBy(theGCD1, tempP1, tempP2, monomialOrder);
  if (!tempP2.isEqualToZero()) {
    global.fatal << "Polynomial must not be zero here. " << global.fatal;
  }
  this->Denominator.getElement() *= tempP1;
  this->Denominator.getElement().DivideBy(theGCD2, tempP1, tempP2, monomialOrder);
  if (!tempP2.isEqualToZero()) {
    global.fatal << "Polynomial must not be zero here. " << global.fatal;
  }
  this->Denominator.getElement() = tempP1;
  other.Numerator.getElementConst().DivideBy(theGCD2, tempP1, tempP2, monomialOrder);
  if (!tempP2.isEqualToZero()) {
    global.fatal << "Polynomial must not be zero here. " << global.fatal;
  }
  this->Numerator.getElement() *= tempP1;
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
void RationalFunction<Coefficient>::operator+=(const RationalFunction<Rational>& other) {
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
  MacroRegisterFunctionWithName("RationalFunctionOld::simplify");
  List<MonomialP>::Comparator* monomialOrder = &MonomialP::orderForGreatestCommonDivisor();
  if (this->expressionType == this->typeRationalFunction) {
    if (!this->Numerator.getElement().isEqualToZero()) {
      Polynomial<Rational> theGCD, tempP, tempP2;
      this->gcd(this->Numerator.getElement(), this->Denominator.getElement(), theGCD);
      if (theGCD.isEqualToZero()) {
        global.fatal << "This is a programing error: "
        << " while fetching the gcd of " << this->Numerator.getElement().toString()
        << " and " << this->Denominator.getElement().toString()
        << " I got 0, which is impossible. " << global.fatal;
      }
      this->Numerator.getElement().DivideBy(theGCD, tempP, tempP2, monomialOrder);
      this->Numerator.getElement() = tempP;
      this->Denominator.getElement().DivideBy(theGCD, tempP, tempP2, monomialOrder);
      this->Denominator.getElement() = tempP;
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
  if (this->expressionType == this->typeRational) {
    Rational result = this->ratValue;
    result.invert();
    this->ratValue.makeOne();
    return result;
  }
  if (this->expressionType == this->typePoly) {
    return this->Numerator.getElement().scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  }
  if (this->expressionType != this->typeRationalFunction) {
    return Rational::one();
  }
  Rational result;
  result = this->Numerator.getElement().scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  result /= this->Denominator.getElement().scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  return result;
}

template<class Coefficient>
void RationalFunction<Coefficient>::simplifyLeadingCoefficientOnly() {
  if (this->expressionType != this->typeRationalFunction) {
    return;
  }
  Rational scaleNumerator = this->Numerator.getElement().scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  Rational scaleDenominator = this->Denominator.getElement().scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  Rational scale = scaleDenominator / scaleNumerator;
  this->Denominator.getElement() *= scale.getDenominator();
  this->Numerator.getElement() *= scale.getNumerator();
}

template<class Coefficient>
bool RationalFunction<Coefficient>::isConstant(Rational* whichConstant) const {
  if (this->expressionType != this->typeRational) {
    return false;
  }
  if (whichConstant != nullptr) {
    *whichConstant = this->ratValue;
  }
  return true;
}

template<class Coefficient>
bool RationalFunction<Coefficient>::isInteger() const {
  return this->expressionType == this->typeRational && this->ratValue.isInteger();
}

template<class Coefficient>
bool RationalFunction<Coefficient>::isSmallInteger(int* whichInteger) const {
  return this->expressionType == this->typeRational &&
  this->ratValue.isSmallInteger(whichInteger);
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
    numeratorContentGreatestCommonDivisor = LargeIntegerUnsigned::lcm(
      scales[i].getNumerator().value,
      numeratorContentGreatestCommonDivisor
    );
    denominatorContentLeastCommonMultiple = LargeIntegerUnsigned::gcd(
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
bool RationalFunction<Coefficient>::gcdQuick(
  const Polynomial<Rational>& left,
  const Polynomial<Rational>& right,
  Polynomial<Rational>& output
) {
  if (left.totalDegree() > 1 && right.totalDegree() > 1) {
    return false;
  }
  Polynomial<Rational> quotient, remainder;
  List<MonomialP>::Comparator* monomialOrder = &MonomialP::orderDefault();
  if (left.totalDegree() > right.totalDegree()) {
    left.DivideBy(right, quotient, remainder, monomialOrder);
    if (remainder.isEqualToZero()) {
      output = right;
    } else {
      output.makeOne();
    }
  } else {
    right.DivideBy(left, quotient, remainder, monomialOrder);
    if (remainder.isEqualToZero()) {
      output = left;
    } else {
      output.makeOne();
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
    case RationalFunction::typeRational:
      this->ratValue.raiseToPower(thePower);
      break;
    case RationalFunction::typePoly:
      this->Numerator.getElement().raiseToPower(thePower, 1);
      break;
    case RationalFunction::typeRationalFunction:
      this->Numerator.getElement().raiseToPower(thePower, 1);
      this->Denominator.getElement().raiseToPower(thePower, 1);
      break;
  }
  this->checkConsistency();
}

template<class Coefficient>
void RationalFunction<Coefficient>::clearDenominators(RationalFunction<Rational>& outputWasMultipliedBy) {
  //outputWasMultipliedBy.makeConstant(this->NumVars, (Rational) 1, this->context);
  Rational tempRat;
  switch(this->expressionType) {
    case RationalFunction::typeRational:
      tempRat = this->ratValue.getDenominator();
      outputWasMultipliedBy.makeConstant(tempRat);
      this->ratValue *= tempRat;
    break;
    case RationalFunction::typePoly:
      this->Numerator.getElement().clearDenominators(tempRat);
      outputWasMultipliedBy.makeConstant(tempRat);
    break;
    case RationalFunction::typeRationalFunction:
      RationalFunction tempRF;
      outputWasMultipliedBy.operator=(this->Denominator.getElement());
      *this *= outputWasMultipliedBy;
      this->clearDenominators(tempRF);
      outputWasMultipliedBy *= tempRF;
      break;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::addSameTypes(const RationalFunction<Rational>& other) {
  switch (this->expressionType) {
    case RationalFunction::typeRational:
      this->ratValue += other.ratValue;
      break;
    case RationalFunction::typePoly:
      this->Numerator.getElement() += other.Numerator.getElementConst();
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
void RationalFunction<Coefficient>::ReducePolynomialToRational() {
  if (this->expressionType == this->typePoly) {
    if (this->Numerator.getElement().isConstant()) {
      this->expressionType = this->typeRational;
      if (this->Numerator.getElement().isEqualToZero()) {
        this->ratValue.makeZero();
      } else {
        this->ratValue = this->Numerator.getElement().coefficients[0];
      }
      this->Numerator.freeMemory();
    }
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::addHonestRationalFunction(const RationalFunction<Rational>& other) {
  MacroRegisterFunctionWithName("RationalFunctionOld::addHonestRationalFunction");
  Rational tempRat;
  if (!this->Denominator.getElement().isProportionalTo(other.Denominator.getElementConst(), tempRat, Rational(1))) {
    Polynomial<Rational> buffer;
    buffer = this->Denominator.getElement();
    this->Numerator.getElement() *= other.Denominator.getElementConst();
    buffer *= other.Numerator.getElementConst();
    this->Numerator.getElement() += buffer;
    this->Denominator.getElement() *= other.Denominator.getElementConst();
    if (this->Denominator.getElement().isEqualToZero())
      global.fatal << "Denominator of rational function is zero." << global.fatal;
    this->simplify();
  } else {
    this->Numerator.getElement() *= tempRat;
    this->Denominator.getElement() *= tempRat;
    this->Numerator.getElement() += other.Numerator.getElementConst();
    this->reduceMemory();
    this->simplifyLeadingCoefficientOnly();
  }
  if (!this->checkConsistency()) {
    global.fatal << "Inconsistent rational function. " << global.fatal;
  }
}

template<class Coefficient>
bool RationalFunction<Coefficient>::isEqualTo(const RationalFunction<Rational>& other) const {
  if (this->expressionType != other.expressionType) {
    return false;
  }
  switch (this->expressionType) {
    case RationalFunction::typeRationalFunction:
      return this->Numerator.getElementConst().isEqualTo(other.Numerator.getElementConst()) &&
      this->Denominator.getElementConst().isEqualTo(other.Denominator.getElementConst());
    case RationalFunction::typePoly:
      return this->Numerator.getElementConst().isEqualTo(other.Numerator.getElementConst());
    case RationalFunction::typeRational:
      return this->ratValue == other.ratValue;
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
  this->ReducePolynomialToRational();
  if (!this->checkConsistency()) {
    global.fatal << "Corrupt rational function. " << global.fatal;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator=(const Polynomial<Rational>& other) {
  this->expressionType = this->typePoly;
  this->Numerator.getElement() = other;
  this->reduceMemory();
}

template<class Coefficient>
unsigned int RationalFunction<Coefficient>::hashFunction() const {
  switch (this->expressionType) {
    case RationalFunction::typeRational:
      return this->ratValue.hashFunction();
    case RationalFunction::typePoly:
      return this->Numerator.getElementConst().hashFunction();
    case RationalFunction::typeRationalFunction:
      return this->Numerator.getElementConst().hashFunction() * someRandomPrimes[0] +
      this->Denominator.getElementConst().hashFunction() * someRandomPrimes[1];
    default:
      return static_cast<unsigned int>(- 1);
  }
}

template<class Coefficient>
unsigned int RationalFunction<Coefficient>::hashFunction(const RationalFunction<Rational>& input) {
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
void RationalFunction<Coefficient>::setNumberOfVariables(int GoalNumVars) {
  this->setNumberOfVariablesSubstituteDeletedByOne(GoalNumVars);
}

template<class Coefficient>
void RationalFunction<Coefficient>::getNumerator(Polynomial<Rational>& output) const {
  switch (this->expressionType) {
    case RationalFunction::typeRational:
      output.makeConstant(this->ratValue);
      return;
    default:
      output = this->Numerator.getElementConst();
      return;
  }
}

template<class Coefficient>
bool RationalFunction<Coefficient>::isNegative() {
  if (this->expressionType == this->typeRational) {
    return this->ratValue.isNegative();
  }
  return false;
}

template<class Coefficient>
void RationalFunction<Coefficient>::getDenominator(Polynomial<Rational>& output) const {
  switch (this->expressionType) {
    case RationalFunction::typeRationalFunction:
      if (this->Denominator.isZeroPointer()) {
        global.fatal << "This is a programming error: the rational function is "
        << "supposed to be honest, but the denominator pointer is zero. " << global.fatal;
      }
      output = this->Denominator.getElementConst();
      return;
    default:
      output.makeConstant(Rational(1));
      return;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::operator+=(const Polynomial<Rational>& other) {
  RationalFunction tempOther;
  tempOther = other;
  *this += tempOther;
}

template<class Coefficient>
bool RationalFunction<Coefficient>::operator==(int other) const {
  if (other == 0) {
    return this->isEqualToZero();
  } else {
    return this->expressionType == this->typeRational && (this->ratValue == other);
  }
}

template<class Coefficient>
bool RationalFunction<Coefficient>::operator!=(int other) {
  return !(this->operator==(other));
}

template<class Coefficient>
bool RationalFunction<Coefficient>::operator==(const RationalFunction<Rational>& other) const {
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
    case RationalFunction::typeRational:
      return this->ratValue > other.ratValue;
    case RationalFunction::typePoly:
      return this->Numerator.getElementConst() > other.Numerator.getElementConst();
    case RationalFunction::typeRationalFunction:
      if (other.Denominator.getElementConst() > this->Denominator.getElementConst()) {
        return true;
      }
      if (this->Denominator.getElementConst() > other.Denominator.getElementConst()) {
        return false;
      }
      return this->Numerator.getElementConst() > other.Numerator.getElementConst();
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
void RationalFunction<Coefficient>::operator/=(const Polynomial<Rational>& other) {
  RationalFunction tempRF;
  tempRF = other;
  tempRF.invert();
  *this *= tempRF;
  if (!this->checkConsistency()) {
    global.fatal << "Bad rational function." << global.fatal;
  }
}

template<class Coefficient>
void RationalFunction<Coefficient>::reduceRationalFunctionToPolynomial() {
  if (this->expressionType != this->typeRationalFunction) {
    return;
  }
  if (this->Denominator.getElement().isConstant()) {
    this->Numerator.getElement() /= this->Denominator.getElement().coefficients[0];
    this->Denominator.freeMemory();
    this->expressionType = this->typePoly;
  }
  if (this->Numerator.getElement().isEqualToZero()) {
    this->makeZero();
  }
}

template<class Coefficient>
bool RationalFunction<Coefficient>::substitution(const PolynomialSubstitution<Rational>& theSub) {
  MacroRegisterFunctionWithName("RationalFunctionOld::substitution");
  if (theSub.size < 1) {
    return false;
  }
  switch(this->expressionType) {
    case RationalFunction::typeRational:
      return true;
    case RationalFunction::typePoly:
      if (!this->Numerator.getElement().substitution(theSub)) {
        return false;
      }
      this->simplify();
      return true;
    case RationalFunction::typeRationalFunction:
      if (!this->Numerator.getElement().substitution(theSub)) {
        return false;
      }
      if (!this->Denominator.getElement().substitution(theSub)) {
        return false;
      }
      if (this->Denominator.getElement().isEqualToZero()) {
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
    case RationalFunction::typeRational:
      return 0;
    case RationalFunction::typePoly:
      return this->Numerator.getElementConst().minimalNumberOfVariables();
    case RationalFunction::typeRationalFunction:
      return MathRoutines::maximum(
        this->Numerator.getElementConst().minimalNumberOfVariables(), this->Denominator.getElementConst().minimalNumberOfVariables()
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
      if (currentPoly.GetMaxPowerOfVariableIndex(j) > 0) {
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
