// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfImplementationHeaderPolynomialComputationsBasic_already_included
#define vpfImplementationHeaderPolynomialComputationsBasic_already_included

#include "math_general.h"

template <class Coefficient>
bool MonomialP::substitution(
  const List<Polynomial<Coefficient> >& theSubstitution,
  Polynomial<Coefficient>& output
) const {
  MacroRegisterFunctionWithName("MonomialP::substitution");
  output.makeConstant(1);
  if (this->isConstant()) {
    return true;
  }
  Polynomial<Coefficient> tempPoly;
  for (int i = 0; i < this->monBody.size; i ++) {
    if (this->monBody[i] == 0) {
      continue;
    }
    if (i >= theSubstitution.size) {
      global.fatal << "This is a programming error. Attempting to carry out a substitution in the monomial "
      << this->toString()
      << " which does have non-zero exponent of variable x_{" << i + 1 << "}; however, the input substitution has "
      << theSubstitution.size
      << " variable images. More precisely, the input substitution is:  "
      << theSubstitution.toString() << ". " << global.fatal;
    }
    int theExponent = 0;
    if (!this->monBody[i].isSmallInteger(&theExponent) || this->monBody[i] < 0) {
      if (theSubstitution[i].isMonomialCoefficientOne()) {
        MonomialP tempMon = theSubstitution[i][0];
        tempMon.raiseToPower(this->monBody[i]);
        output *= tempMon;
        continue;
      }
      global.comments << "This may or may not be a programming error. "
      << "I cannot carry out a substitution in a monomial that has exponent "
      << "which is not a small integer: it is " << this->monBody[i]
      << " instead. " << GlobalVariables::Crasher::getStackTraceEtcErrorMessageHTML();
      return false;
    }
    tempPoly = theSubstitution[i];
    tempPoly.raiseToPower(theExponent, 1);
    output *= tempPoly;
  }
  return true;
}

template<class Coefficient>
bool Polynomial<Coefficient>::isOneVariableNonConstantPolynomial(int* whichVariable) const {
  int tempInt;
  if (whichVariable == nullptr) {
    whichVariable = &tempInt;
  }
  if (!this->isOneVariablePolynomial(whichVariable)) {
    return false;
  }
  return *whichVariable != - 1;
}

template<class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::one() const {
  Polynomial<Coefficient> result;
  result.makeOne();
  return result;
}

template<class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::zero() {
  Polynomial<Coefficient> result;
  result.makeZero();
  return result;
}

template<class Coefficient>
Rational Polynomial<Coefficient>::rationalValue() {
  Rational result;
  this->getConstantTerm(result, 0);
  return result;
}

template<class Coefficient>
bool Polynomial<Coefficient>::isOneVariablePolynomial(int* whichVariable) const {
  int tempInt;
  if (whichVariable == nullptr) {
    whichVariable = &tempInt;
  }
  *whichVariable = - 1;
  for (int i = 0; i < this->size(); i ++) {
    for (int j = 0; j < (*this)[i].minimalNumberOfVariables(); j ++) {
      if ((*this)[i](j) != 0) {
        if (*whichVariable == - 1) {
          *whichVariable = j;
        } else if (*whichVariable != j) {
          return false;
        }
      }
    }
  }
  return true;
}

template <class Coefficient>
void Polynomial<Coefficient>::makeDeterminantFromSquareMatrix(
  const Matrix<Polynomial<Coefficient> >& theMat
) {
  if (theMat.numberOfColumns != theMat.numberOfRows) {
    global.fatal << "Cannot compute determinant: matrix has "
    << theMat.numberOfRows << " rows and " << theMat.numberOfColumns
    << " columns. " << global.fatal;
  }
  Permutation thePerm;
  thePerm.initPermutation(theMat.numberOfRows);
  int numCycles = thePerm.getNumberOfPermutations();
  List<int> permutationIndices;
  thePerm.getPermutationLthElementIsTheImageofLthIndex(permutationIndices);
  Polynomial<Coefficient> result, theMonomial;
  result.makeZero();
  result.setExpectedSize(numCycles);
  for (int i = 0; i < numCycles; i ++, thePerm.incrementAndGetPermutation(permutationIndices)) {
    theMonomial.makeOne();
    for (int j = 0; j < permutationIndices.size; j ++) {
      theMonomial *= theMat(j, permutationIndices[j]);
    }
    // The following can be made much faster, but no need right now as it won't be a bottleneck.
    int sign = 1;
    for (int j = 0; j < permutationIndices.size; j ++) {
      for (int k = j + 1; k < permutationIndices.size; k ++) {
        if (permutationIndices[k] < permutationIndices[j]) {
          sign *= - 1;
        }
      }
    }
    theMonomial *= sign;
    result += theMonomial;
  }
  *this = result;
}

template <class Coefficient>
int Polynomial<Coefficient>::totalDegreeInt() const {
  int result = - 1;
  if (!this->totalDegree().isSmallInteger(&result)) {
    global.fatal
    << "This is a programming error: requested total degree of a "
    << "polynomial in int formal, but the "
    << "degree of the polynomial is not a small integer. "
    << global.fatal;
  }
  return result;
}

template <class Coefficient>
Rational Polynomial<Coefficient>::totalDegree() const {
  Rational result = 0;
  for (int i = 0; i < this->size(); i ++) {
    result = MathRoutines::maximum((*this)[i].totalDegree(), result);
  }
  return result;
}

template <class Coefficient>
bool Polynomial<Coefficient>::substitution(
  const List<Polynomial<Coefficient> >& TheSubstitution
) {
  MacroRegisterFunctionWithName("Polynomial::substitution");
  Polynomial<Coefficient> Accum, TempPoly;
  for (int i = 0; i < this->size(); i ++) {
    if (!(*this)[i].substitution(TheSubstitution, TempPoly)) {
      return false;
    }
    TempPoly *= this->coefficients[i];
    Accum += TempPoly;
  }
  *this = Accum;
  return true;
}

template <class Coefficient>
void Polynomial<Coefficient>::makeOne() {
  this->makeConstant(1);
}

template <class Coefficient>
void Polynomial<Coefficient>::makeDegreeOne(
  int NVar, int NonZeroIndex, const Coefficient& coeff
) {
  this->makeZero();
  MonomialP tempM;
  tempM.makeEi(NonZeroIndex, 1, NVar);
  this->addMonomial(tempM, coeff);
}

template <class Coefficient>
void Polynomial<Coefficient>::makeDegreeOne(
  int NVar,
  int NonZeroIndex1,
  int NonZeroIndex2,
  const Coefficient& coeff1,
  const Coefficient& coeff2
) {
  (void) NVar;
  this->makeZero();
  MonomialP tempM;
  tempM.makeEi(NonZeroIndex1);
  this->addMonomial(tempM, coeff1);
  tempM.makeEi(NonZeroIndex2);
  this->addMonomial(tempM, coeff2);
}

template <class Coefficient>
void Polynomial<Coefficient>::makeDegreeOne(
  int NVar,
  int NonZeroIndex,
  const Coefficient& coeff1,
  const Coefficient& ConstantTerm
) {
  this->makeDegreeOne(NVar, NonZeroIndex, coeff1);
  *this += ConstantTerm;
}

template <class Coefficient>
Coefficient Polynomial<Coefficient>::evaluate(const Vector<Coefficient>& input) {
  MacroRegisterFunctionWithName("Polynomial::evaluate");
  Coefficient output = 0;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialP& currentMon = (*this)[i];
    Coefficient accum = this->coefficients[i];
    Coefficient tempElt;
    for (int j = 0; j < currentMon.minimalNumberOfVariables(); j ++) {
      int numCycles = 0;
      if (!(*this)[i](j).isSmallInteger(&numCycles)) {
        global.fatal << "This is a programming error. "
        << "Attempting to evaluate a polynomial whose "
        << i + 1 << "^{th} variable is raised to the power "
        << (*this)[i](j).toString()
        << ". Raising variables to power is allowed "
        << "only if the power is a small integer. "
        << "If the user has requested such an operation, "
        << "it *must* be intercepted at an earlier level "
        << "(and the user must be informed)." << global.fatal;
      }
      bool isPositive = numCycles > 0;
      if (numCycles < 0) {
        numCycles = - numCycles;
      }
      tempElt = input[j];
      MathRoutines::raiseToPower(tempElt, numCycles, static_cast<Coefficient>(1));
      if (!isPositive) {
        tempElt.invert();
      }
      accum *= tempElt;
    }
    output += accum;
  }
  return output;
}

template <class Coefficient>
void Polynomial<Coefficient>::setNumberOfVariablesSubstituteDeletedByOne(int newNumVars) {
  MacroRegisterFunctionWithName("Polynomial_CoefficientType::setNumberOfVariablesSubstituteDeletedByOne");
  if (newNumVars >= this->minimalNumberOfVariables()) {
    return;
  }
  if (newNumVars < 0) {
    global.fatal << "This is a programming error. "
    << "Requesting negative number of variables (more precisely, "
    << newNumVars << ") is not allowed. " << global.fatal;
  }
  Polynomial<Coefficient> Accum;
  Accum.makeZero();
  Accum.setExpectedSize(this->size());
  MonomialP tempM;
  for (int i = 0; i < this->size(); i ++) {
    tempM.makeOne();
    for (int j = 0; j < newNumVars; j ++) {
      tempM.setVariable(j, (*this)[i](j));
    }
    Accum.addMonomial(tempM, this->coefficients[i]);
  }
  this->operator=(Accum);
}

template <class Coefficient>
bool Polynomial<Coefficient>::hasSmallIntegralPositivePowers(
  int* whichtotalDegree
) const {
  int whichtotalDegreeContainer = 0;
  for (int i = 0; i < this->size(); i ++) {
    if (!this->monomials[i].hasSmallIntegralPositivePowers(&whichtotalDegreeContainer)) {
      return false;
    }
  }
  if (whichtotalDegree != nullptr) {
    *whichtotalDegree = whichtotalDegreeContainer;
  }
  return true;
}

template <class Coefficient>
void Polynomial<Coefficient>::shiftVariableIndicesToTheRight(int VarIndexShift) {
  if (VarIndexShift < 0) {
    global.fatal << "This is a programming error. "
    << "Requesting negative variable shift (more precisely, "
    << VarIndexShift << ") not allowed. " << global.fatal;
  }
  if (VarIndexShift == 0) {
    return;
  }
  int oldNumVars = this->minimalNumberOfVariables();
  Polynomial<Coefficient> Accum;
  Accum.makeZero();
  Accum.setExpectedSize(this->size());
  MonomialP tempM;
  for (int i = 0; i < this->size(); i ++) {
    tempM.makeOne();
    for (int j = 0; j < oldNumVars; j ++) {
      tempM.setVariable(j + VarIndexShift, (*this)[i](j));
    }
    Accum.addMonomial(tempM, this->coefficients[i]);
  }
  *this = Accum;
}

template <class Coefficient>
bool Polynomial<Coefficient>::isEqualToOne() const {
  Coefficient tempC;
  if (this->isConstant(&tempC)) {
    return tempC.isEqualToOne();
  }
  return false;
}

template <class Coefficient>
bool Polynomial<Coefficient>::isMonomialCoefficientOne() const {
  if (this->size() != 1) {
    return false;
  }
  return this->coefficients[0].isEqualToOne();
}

template <class Coefficient>
bool Polynomial<Coefficient>::isOneLetterFirstDegree(int* whichLetter) const {
  if (this->size() != 1) {
    return false;
  }
  return (*this)[0].isOneLetterFirstDegree(whichLetter);
}

template <class Coefficient>
bool Polynomial<Coefficient>::isConstant(Coefficient* whichConstant) const {
  if (this->size() > 1) {
    return false;
  }
  if (this->size() == 0) {
    if (whichConstant != nullptr) {
      *whichConstant = 0;
    }
    return true;
  }
  if (whichConstant != nullptr) {
    *whichConstant = this->coefficients[0];
  }
  const MonomialP& theMon = (*this)[0];
  return theMon.isConstant();
}

template <class Coefficient>
bool Polynomial<Coefficient>::isNegative() const {
  Coefficient tempC;
  if (!this->isConstant(&tempC)) {
    return false;
  }
  return tempC.isNegative();
}

template <class Coefficient>
bool Polynomial<Coefficient>::isLinearNoConstantTerm() {
  for (int i = 0; i < this->size; i ++) {
    if (!this->objects[i].isLinearNoConstantTerm()) {
      return false;
    }
  }
  return true;
}

template <class Coefficient>
bool Polynomial<Coefficient>::isLinear() {
  for (int i = 0; i < this->size(); i ++) {
    if (!(*this)[i].isLinear()) {
      return false;
    }
  }
  return true;
}

template <class Coefficient>
bool Polynomial<Coefficient>::isLinearGetRootConstantTermLastCoordinate(Vector<Coefficient>& outputRoot) {
  outputRoot.makeZero(this->minimalNumberOfVariables() + 1);
  int index;
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].isConstant()) {
      *outputRoot.lastObject() = this->coefficients[i];
    } else {
      if ((*this)[i].isOneLetterFirstDegree(&index)) {
        outputRoot[index] = this->coefficients[i];
      } else {
        return false;
      }
    }
  }
  return true;
}

template <class Coefficient>
void Polynomial<Coefficient>::raiseToPower(
  int d, const Coefficient& one
) {
  if (d == 1) {
    return;
  }
  if (d < 0) {
    global.fatal << "This is a programming error: attempting to raise the polynomial "
    << this->toString() << " to the negative power "
    << d << ". " << global.fatal;
  }
  Polynomial<Coefficient> onePolynomial;
  onePolynomial.makeConstant(one);
  MathRoutines::raiseToPower(*this, d, onePolynomial);
}

template <class Coefficient>
bool Polynomial<Coefficient>::getRootFromLinearPolynomialConstantTermLastVariable(Vector<Coefficient>& outputRoot) {
  return this->isLinearGetRootConstantTermLastCoordinate(outputRoot);
}

template <class Coefficient>
Matrix<Coefficient> Polynomial<Coefficient>::evaluateUnivariatePolynomial(
  const Matrix<Coefficient>& input
) {
  // for univariate polynomials only
  MacroRegisterFunctionWithName("Polynomial::evaluateUnivariatePolynomial");
  Matrix<Coefficient> output, tempElt, idMat;
  idMat.makeIdentityMatrix(input.numberOfColumns);
  output.makeZeroMatrix(input.numberOfColumns);
  for (int i = 0; i < this->size; i ++) {
    const MonomialP& currentMon = (*this)[i];
    int numCycles = 0;
    if (!currentMon(0).isSmallInteger(&numCycles) ) {
      global.fatal
      << "This is a programming error. "
      << "Attempting to evaluate a polynomial whose "
      << i + 1 << "^{th} variable is raised to the power "
      << currentMon(0).toString()
      << ". Raising variables to power is allowed "
      << "only if the power is a small integer. "
      << "If the user has requested such an operation, "
      << "it *must* be intercepted at an earlier level "
      << "(and the user must be informed)."
      << global.fatal;
    }
    bool isPositive = (numCycles > 0);
    if (numCycles < 0) {
      numCycles = - numCycles;
    }
    tempElt = input;
    MathRoutines::raiseToPower(tempElt, numCycles, idMat);
    if (!isPositive) {
      tempElt.invert();
    }
    tempElt *= this->coefficients[i];
    output += tempElt;
  }
  return output;
}

template <class Coefficient>
int Polynomial<Coefficient>::getHighestIndexSuchThatHigherIndexVariablesDontParticipate() {
  int result = - 1;
  for (int i = 0; i < this->size; i ++) {
    result = MathRoutines::maximum(
      result,
      this->objects[i].getHighestIndexSuchThatHigherIndexVariablesDontParticipate()
    );
  }
  return result;
}

template <class Coefficient>
void Polynomial<Coefficient>::scaleToPositiveMonomialExponents(MonomialP& outputScale) {
  int numVars = this->minimalNumberOfVariables();
  outputScale.makeOne();
  for (int i = 0; i < numVars; i ++) {
    for (int j = 0; j < this->size(); j ++) {
      const MonomialP& currentMonomial = (*this)[j];
      Rational currentScale = MathRoutines::minimum(outputScale(i), currentMonomial(i));
      outputScale.setVariable(i, currentScale);
    }
  }
  outputScale.invert();
  this->multiplyBy(outputScale);
}

template <class Coefficient>
bool Polynomial<Coefficient>::operator<=(const Coefficient& other) const {
  Coefficient constME;
  if (!this->isConstant(&constME)) {
    global.fatal << "This may or may not be a programming error: "
    << "attempting to compare a non-constant polynomial to "
    << "a constant. I cannot judge at the moment whether "
    << "allowing that is a good decision. "
    << "In any case, crashing to let you know. "
    << global.fatal;
    return false;
  }
  return constME <= other;
}

template <class Coefficient>
bool Polynomial<Coefficient>::operator<(const Coefficient& other) const {
  Coefficient constME;
  if (!this->isConstant(&constME)) {
    global.fatal << "This may or may not be a programming error: "
    << "attempting to compare a non-constant polynomial to "
    << "a constant. I cannot judge at the moment whether allowing "
    << "that is a good decision. In any case, crashing to let you know. "
    << global.fatal;
    return false;
  }
  return constME<other;
}

template <class Coefficient>
bool Polynomial<Coefficient>::operator>(const Polynomial<Coefficient>& other) const {
  if (other.size() == 0) {
    if (this->size() == 0) {
      return false;
    } else {
      return true;
    }
  }
  if (this->size() == 0) {
    return false;
  }
  if (this->totalDegree() > other.totalDegree()) {
    return true;
  }
  if (this->totalDegree() < other.totalDegree()) {
    return false;
  }
  MonomialP thisMaximalMonomial, otherMaximalMonomial;
  List<MonomialP>::Comparator& monomialOrder = MonomialP::orderDefault();
  int thisMaxMonIndex = this->getIndexLeadingMonomial(&thisMaximalMonomial, nullptr, &monomialOrder);
  int otherMaxMonIndex = other.getIndexLeadingMonomial(&otherMaximalMonomial, nullptr, &monomialOrder);
  if (thisMaximalMonomial > otherMaximalMonomial) {
    return true;
  }
  if (otherMaximalMonomial > thisMaximalMonomial) {
    return false;
  }
  if (this->coefficients[thisMaxMonIndex] > other.coefficients[otherMaxMonIndex]) {
    return true;
  }
  if (other.coefficients[otherMaxMonIndex] > this->coefficients[thisMaxMonIndex]) {
    return false;
  }
  return false;
}

template <class Coefficient>
bool Polynomial<Coefficient>::isGEQCompareByTopMonomialTotalDegThenLexicographic(
  const Polynomial<Coefficient>& left, const Polynomial<Coefficient>& right
) {
  if (left.isEqualToZero()) {
    return right.isEqualToZero();
  }
  if (right.isEqualToZero()) {
    return true;
  }
  return left[left.GetIndexMaxMonomialTotalDegThenLexicographic()].MonomialP::IsGEQTotalDegThenLexicographic(
    right[right.GetIndexMaxMonomialTotalDegThenLexicographic()]
  );
}

template <class Coefficient>
bool Polynomial<Coefficient>::isGEQCompareByTopMonomialLexicographicLastVarStrongest(
  const Polynomial<Coefficient>& left, const Polynomial<Coefficient>& right
) {
  if (left.isEqualToZero()) {
    return right.isEqualToZero();
  }
  if (right.isEqualToZero()) {
    return true;
  }
  int indexMaxLeft = left.GetIndexMaxMonomialLexicographicLastVariableStrongest();
  int indexMaxRight = right.GetIndexMaxMonomialLexicographicLastVariableStrongest();
  return left[indexMaxLeft].MonomialP::IsGEQLexicographicLastVariableStrongest(right[indexMaxRight]);
}

template <class Coefficient>
bool Polynomial<Coefficient>::isEqualTo(const Polynomial<Coefficient>& p) const {
  return *this == p;
}

template <class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::operator+(const Polynomial<Coefficient>& other) const {
  Polynomial<Coefficient> copy = *this;
  copy += other;
  return copy;
}

template <class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::operator-(const Polynomial<Coefficient>& other) const {
  Polynomial<Coefficient> copy = *this;
  copy -= other;
  return copy;
}

template <class Coefficient>
void Polynomial<Coefficient>::operator-=(int x) {
  MonomialP tempMon;
  tempMon.makeOne();
  this->subtractMonomial(tempMon, x);
}

template <class Coefficient>
void Polynomial<Coefficient>::operator-=(const Coefficient& other) {
  MonomialP tempMon;
  tempMon.makeOne();
  this->subtractMonomial(tempMon, other);
}

template <class Coefficient>
void Polynomial<Coefficient>::operator-=(const Polynomial<Coefficient>& other) {
  this->::LinearCombination<MonomialP, Coefficient>::operator-=(other);
}

template <class Coefficient>
void Polynomial<Coefficient>::operator*=(const MonomialP& other) {
  this->multiplyBy(other, *this);
}

template <class Coefficient>
void Polynomial<Coefficient>::operator*=(const Polynomial<Coefficient>& other) {
  this->::ElementMonomialAlgebra<MonomialP, Coefficient>::operator*=(other);
}

template <class Coefficient>
Polynomial<Coefficient> Polynomial<Coefficient>::operator%(const Polynomial<Coefficient>& other) {
  Polynomial<Coefficient> temp;
  Polynomial<Coefficient> result;
  this->divideBy(other, temp, result, &MonomialP::orderDefault());
  return result;
}

template <class Coefficient>
void Polynomial<Coefficient>::operator/=(const Polynomial<Coefficient>& other) {
  Polynomial<Coefficient> tempMe = *this;
  Polynomial<Coefficient> tempRemainder;
  tempMe.divideBy(other, *this, tempRemainder, &MonomialP::orderDefault());
}

template <class Coefficient>
void Polynomial<Coefficient>::operator/=(int other) {
  this->::LinearCombination<MonomialP, Coefficient>::operator/=(other);
}

template <class Coefficient>
void Polynomial<Coefficient>::operator/=(const Coefficient& other) {
  this->::LinearCombination<MonomialP, Coefficient>::operator/=(other);
}

template <class Coefficient>
template <class otherType>
void Polynomial<Coefficient>::operator*=(const otherType& other) {
  this->::LinearCombination<MonomialP, Coefficient>::operator*= (other);
}

template <class Coefficient>
void Polynomial<Coefficient>::operator=(const Polynomial<Coefficient>& other) {
  this->::LinearCombination<MonomialP, Coefficient>::operator=(other);
}

template <class Coefficient>
template<class otherType>
void Polynomial<Coefficient>::operator=(const Polynomial<otherType>& other) {
  this->::LinearCombination<MonomialP, Coefficient>::operator=(other);
}

template <class Coefficient>
void Polynomial<Coefficient>::operator=(const Coefficient& other) {
  this->makeConstant(other);
}

template <class Coefficient>
void Polynomial<Coefficient>::operator=(int other) {
  Coefficient tempCF;
  tempCF = other;
  this->makeConstant(tempCF);
}

template <class Coefficient>
template <class otherType>
void Polynomial<Coefficient>::assignOtherType(const Polynomial<otherType>& other) {
  this->NumVars = other.NumVars;
  this->::LinearCombination<MonomialP, Coefficient>::assignOtherType(other);
}

template <class Coefficient>
bool Polynomial<Coefficient>::operator<=(const Polynomial<Coefficient>& other) const {
  return !(*this > other);
}

template <class Coefficient>
bool Polynomial<Coefficient>::isProportionalTo(
  const Polynomial<Coefficient>& other,
  Coefficient& outputTimesMeEqualsOther,
  const Coefficient& ringUnit
) const {
  if (this->size() != other.size()) {
    return false;
  }
  if (other.size() == 0) {
    outputTimesMeEqualsOther = ringUnit;
    return true;
  }
  const MonomialP& firstMon = (*this)[0];
  int indexInOther = other.monomials.getIndex(firstMon);
  if (indexInOther == - 1) {
    return false;
  }
  outputTimesMeEqualsOther = other.coefficients[indexInOther];
  outputTimesMeEqualsOther /= this->coefficients[0];
  Polynomial<Coefficient> tempP;
  tempP = *this;
  tempP *= outputTimesMeEqualsOther;
  tempP -= other;
  return tempP.isEqualToZero();
}

template <class Coefficient>
void Polynomial<Coefficient>::divideBy(
  const Polynomial<Coefficient>& inputDivisor,
  Polynomial<Coefficient>& outputQuotient,
  Polynomial<Coefficient>& outputRemainder,
  typename List<MonomialP>::Comparator* monomialOrder
) const {
  MacroRegisterFunctionWithName("Polynomial::divideBy");
  if (monomialOrder == nullptr) {
    global.fatal << "Non-initialized monomial pointer not allowed. " << global.fatal;
  }
  if (&outputQuotient == &outputRemainder) {
    global.fatal << "Input and output addresses not allowed to coincide. " << global.fatal;
  }
  if (inputDivisor.isEqualToZero()) {
    global.fatal << "Division by zero polynomial. " << global.fatal;
  }
  if (
    &outputRemainder == this ||
    &outputQuotient == this ||
    &outputRemainder == &inputDivisor ||
    &outputQuotient == &inputDivisor
  ) {
    Polynomial<Coefficient> newQuotient, newRemainder;
    this->divideBy(inputDivisor, newQuotient, newRemainder, monomialOrder);
    outputQuotient = newQuotient;
    outputRemainder = newRemainder;
    return;
  }
  outputRemainder = *this;
  MonomialP scaleRemainder, scaleInput;
  Polynomial<Coefficient> divisorShiftedExponents = inputDivisor;
  outputRemainder.scaleToPositiveMonomialExponents(scaleRemainder);
  divisorShiftedExponents.scaleToPositiveMonomialExponents(scaleInput);
  MonomialP remainderLeadingMonomial;
  Coefficient remainderLeadingCoefficient;
  int remainderLeadingIndex = outputRemainder.getIndexLeadingMonomial(
    &remainderLeadingMonomial, &remainderLeadingCoefficient, monomialOrder
  );
  MonomialP leadingMonomialShiftedDivisor;
  Coefficient leadingCoefficientShiftedDivisor;
  divisorShiftedExponents.getIndexLeadingMonomial(
    &leadingMonomialShiftedDivisor,
    &leadingCoefficientShiftedDivisor,
    monomialOrder
  );
  outputQuotient.makeZero();
  if (remainderLeadingIndex == - 1) {
    return;
  }
  outputQuotient.setExpectedSize(this->size());
  MonomialP quotientMonomial;
  Polynomial<Coefficient> subtracand;
  subtracand.setExpectedSize(this->size());
  while (monomialOrder->greaterThanOrEqualTo(remainderLeadingMonomial, leadingMonomialShiftedDivisor)) {
    quotientMonomial = remainderLeadingMonomial;
    quotientMonomial /= leadingMonomialShiftedDivisor;
    if (!quotientMonomial.hasPositiveOrZeroExponents()) {
      break;
    }
    Coefficient quotientCoefficient = remainderLeadingCoefficient;
    quotientCoefficient /= leadingCoefficientShiftedDivisor;
    outputQuotient.addMonomial(quotientMonomial, quotientCoefficient);
    subtracand = divisorShiftedExponents;
    subtracand.multiplyBy(quotientMonomial, quotientCoefficient);
    outputRemainder -= subtracand;
    int remainderIndex = outputRemainder.getIndexLeadingMonomial(
      &remainderLeadingMonomial,
      &remainderLeadingCoefficient,
      monomialOrder
    );
    if (remainderIndex == - 1) {
      break;
    }
  }
  scaleInput.invert();
  outputQuotient.multiplyBy(scaleInput);
  outputQuotient.multiplyBy(scaleRemainder);
  outputRemainder.multiplyBy(scaleRemainder);
}

template <class Coefficient>
void Polynomial<Coefficient>::assignCharacteristicPoly(const Matrix<Coefficient>& input) {
  MacroRegisterFunctionWithName("Polynomial::assignCharacteristicPoly");
  if (input.numberOfColumns != input.numberOfRows) {
    global.fatal << "Programming error: requesting the "
    << "minimimal polynomial of a non-square matrix. "
    << global.fatal;
  }
  int n = input.numberOfColumns;
  if (n == 0) {
    global.fatal
    << "At present, the characteristic polynomial of "
    << "a 0x0 matrix is not defined. "
    << "Crashing to let you know. If you think this "
    << "should be changed, document why and "
    << "modify the present assertion. " << global.fatal;
  }
  this->makeConstant(1);
  Matrix<Coefficient> acc = input;
  Coefficient currenCF;
  for (int i = 1; i < n; i ++) {
    currenCF = - acc.getTrace() / i;
    this->addMonomial(MonomialP(0, i), currenCF);
    for (int j = 0; j < n; j ++) {
      acc(j, j) += currenCF;
    }
    acc.multiplyOnTheLeft(input);
  }
  this->addMonomial(MonomialP(0, n), - acc.getTrace() / n);
}

template <class Coefficient>
void Polynomial<Coefficient>::assignMinPoly(const Matrix<Coefficient>& input) {
  MacroRegisterFunctionWithName("Polynomial::assignMinPoly");
  if (input.numberOfColumns != input.numberOfRows) {
    global.fatal << "Programming error: requesting the "
    << "minimimal polynomial of a non-square matrix. "
    << global.fatal;
  }
  int theDim = input.numberOfColumns;
  this->makeOne();
  Vectors<Coefficient> theBasis;
  Vector<Coefficient> theVectorPowers;
  Vector<Coefficient> firstDependentPower;
  Polynomial<Coefficient> currentFactor;
  MonomialP tempM;
  for (int col = 0; col < theDim; col ++) {
    theVectorPowers.makeEi(theDim,col);
    theBasis.setSize(0);
    theBasis.addOnTop(theVectorPowers);
    for (int i = 0; i < theDim; i ++) {
      input.actOnVectorColumn(*theBasis.lastObject(), theVectorPowers);
      if (theBasis.linearSpanContainsVector(theVectorPowers)) {
        break;
      }
      theBasis.addOnTop(theVectorPowers);
    }
    theVectorPowers.getCoordinatesInBasis(theBasis, firstDependentPower);
    currentFactor.setExpectedSize(theBasis.size + 1);
    currentFactor.makeZero();
    for (int i = 0; i < theBasis.size; i ++) {
      tempM.makeEi(0, i, 1);
      currentFactor.addMonomial(tempM, - firstDependentPower[i]);
    }
    tempM.makeEi(0, theBasis.size, 1);
    currentFactor.addMonomial(tempM, 1);
    *this = MathRoutines::leastCommonMultiple(*this, currentFactor);
  }
  this->scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
}

template <class Coefficient>
int Polynomial<Coefficient>::getMaximumPowerOfVariableIndex(int VariableIndex) {
  int result = 0;
  for (int i = 0; i < this->size(); i ++) {
    result = MathRoutines::maximum(result, (*this)[i](VariableIndex).numeratorShort);
    if (!(*this)[i](VariableIndex).isSmallInteger()) {
      global.fatal << " This is a programming error: "
      << "getMaximumPowerOfVariableIndex called on a polynomial whose monomials "
      << "have degrees that are not small integers. "
      << "This needs to be fixed! " << global.fatal;
    }
  }
  return result;
}

template <class Coefficient>
void Polynomial<Coefficient>::interpolate(
  const Vector<Coefficient>& thePoints, const Vector<Coefficient>& ValuesAtThePoints
) {
  Polynomial<Coefficient> theLagrangeInterpolator, accumulator;
  this->makeZero();
  for (int i = 0; i < thePoints.size; i ++) {
    theLagrangeInterpolator.makeConstant(1);
    for (int j = 0; j < thePoints.size; j ++) {
      if (i == j) {
        continue;
      }
      accumulator.makeDegreeOne(1, 0, 1, - thePoints[j]);
      accumulator /= thePoints[i] - thePoints[j];
      theLagrangeInterpolator *= accumulator;
    }
    theLagrangeInterpolator *= ValuesAtThePoints[i];
    *this += theLagrangeInterpolator;
  }
}

template <class Coefficient>
void Polynomial<Coefficient>::getConstantTerm(Coefficient& output, const Coefficient& ringZero) const {
  MonomialP tempM;
  tempM.makeOne();
  int i = this->monomials.getIndex(tempM);
  if (i == - 1) {
    output = ringZero;
  } else {
    output = this->coefficients[i];
  }
}

template <class Coefficient>
void Polynomial<Coefficient>::makeMonomial(
  int letterIndex,
  const Rational& power,
  const Coefficient& inputCoefficient
) {
  if (letterIndex < 0) {
    global.fatal << "Negative variable index: "
    << letterIndex << " is not allowed. "
    << global.fatal;
  }
  this->makeZero();
  MonomialP variable;
  variable.makeOne();
  variable.setVariable(letterIndex, power);
  this->addMonomial(variable, inputCoefficient);
}

template <class Coefficient>
void Polynomial<Coefficient>::getCoefficientInFrontOfLinearTermVariableIndex(
  int index, Coefficient& output
) {
  MonomialP tempM;
  tempM.makeEi(index);
  int i = this->monomials.getIndex(tempM);
  if (i == - 1) {
    output = 0;
  } else {
    output = this->coefficients[i];
  }
}

template<class Coefficient>
bool Polynomial<Coefficient>::isSquareFree(
  const Coefficient& one, std::stringstream* comments
) const {
  MacroRegisterFunctionWithName("Polynomial::isSquareFree");
  Vector<Polynomial<Coefficient> > differentials;
  if (this->isConstant()) {
    return false;
  }
  if (!this->differential(differentials, comments)) {
    return false;
  }
  Polynomial<Coefficient> divisor = *this;
  for (int i = 0; i < differentials.size; i ++) {
    if (differentials[i].isEqualToZero()) {
      continue;
    }
    Polynomial::greatestCommonDivisor(divisor, differentials[i], divisor, one, comments);
  }
  return divisor.totalDegree() == 0;
}

template<class Coefficient>
bool Polynomial<Coefficient>::differential(
  Vector<Polynomial<Coefficient> >& output, std::stringstream* comments
) const {
  MacroRegisterFunctionWithName("Polynomial::differential");
  int numberOfVariables = this->minimalNumberOfVariables();
  output.setSize(numberOfVariables);
  for (int i = 0; i < this->size(); i ++) {
    const MonomialP& currentMonomial = this->monomials[i];
    const Coefficient& currentCoefficient = this->coefficients[i];
    int currentNumberOfVariables = currentMonomial.minimalNumberOfVariables();
    for (int j = 0; j < currentNumberOfVariables; j ++) {
      Coefficient newCoefficient = currentCoefficient;
      Rational power = currentMonomial(j);
      LargeInteger powerInteger;
      if (!power.isInteger(&powerInteger)) {
        if (comments != nullptr) {
          *comments << "Monomial has non-integer power " << power << ". ";
        }
        return false;
      }
      newCoefficient *= power;
      MonomialP newMonomial = currentMonomial;
      power -= 1;
      newMonomial.setVariable(j, power);
      output[j].addMonomial(newMonomial, newCoefficient);
    }
  }
  return true;
}

template<class Coefficient>
bool Polynomial<Coefficient>::differential(
  Polynomial<Coefficient>& output,
  std::stringstream* comments
) const {
  MacroRegisterFunctionWithName("Polynomial::differential");
  if (&output == this) {
    Polynomial<Coefficient> thisCopy = *this;
    return thisCopy.differential(output, comments);
  }
  Vector<Polynomial<Coefficient> > differentials;
  if (!this->differential(differentials, comments)) {
    return false;
  }
  output.makeZero();
  Polynomial<Coefficient> nextSummand;
  for (int i = 0; i < differentials.size; i ++) {
    nextSummand = differentials[i];
    MonomialP differentialVariable(differentials.size + i, 1);
    nextSummand *= differentialVariable;
    output += nextSummand;
  }
  return true;
}

template <class Coefficient>
bool PolynomialOrder<Coefficient>::compareLeftGreaterThanRight(
  const Polynomial<Coefficient>& left, const Polynomial<Coefficient>& right
) const {
  MacroRegisterFunctionWithName("PolynomialOrder::compareLeftGreaterThanRight");
  List<MonomialP> sortedLeft = left.monomials;
  List<MonomialP> sortedRight = right.monomials;
  sortedLeft.quickSortAscending(&this->monomialOrder);
  sortedRight.quickSortAscending(&this->monomialOrder);
  int leftIndex = sortedLeft.size - 1;
  int rightIndex = sortedRight.size - 1;
  for (; leftIndex >= 0 && rightIndex >= 0; leftIndex --, rightIndex--) {
    MonomialP& leftMonomial = sortedLeft[leftIndex];
    MonomialP& rightMonomial = sortedRight[rightIndex];
    if (leftMonomial > rightMonomial) {
      return true;
    }
    if (rightMonomial > leftMonomial) {
      return false;
    }
    Coefficient leftCoefficient = left.getCoefficientOf(leftMonomial);
    Coefficient rightCoefficient = right.getCoefficientOf(rightMonomial);
    if (leftCoefficient > rightCoefficient) {
      return true;
    }
    if (rightCoefficient > leftCoefficient) {
      return false;
    }
  }
  if (left.size() > right.size()) {
    return true;
  }
  if (right.size() > left.size()) {
    return false;
  }
  return false;
}

template<class Coefficient>
std::string PolynomialDivisionReport<Coefficient>::getPolynomialStringSpacedMonomialsLaTeX(
  const Polynomial<Coefficient>& thePoly,
  std::string* highlightColor,
  List<MonomialP>* theHighLightedMons,
  int* firstNonZeroIndex
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::getPolynomialStringSpacedMonomialsLaTeX");
  std::stringstream out;
  bool found = false;
  int countMons = 0;
  if (firstNonZeroIndex != nullptr) {
    *firstNonZeroIndex = - 1;
  }
  for (int i = 0; i < this->allMonomials.size; i ++) {
    int theIndex = thePoly.monomials.getIndex(this->allMonomials[i]);
    if (theIndex == - 1) {
      if (i != this->allMonomials.size - 1) {
        out << "&";
      }
      continue;
    }
    if (firstNonZeroIndex != nullptr) {
      if (*firstNonZeroIndex == - 1) {
        *firstNonZeroIndex = i;
      }
    }
    countMons ++;
    bool useHighlightStyle = false;
    if (highlightColor != nullptr) {
      if (theHighLightedMons != nullptr) {
        if (theHighLightedMons->contains(this->allMonomials[i])) {
          useHighlightStyle = true;
        }
      }
    }
    out << "$";
    if (useHighlightStyle) {
      out << "\\color{" << *highlightColor << "}{";
    }
    out << Polynomial<Coefficient>::getBlendCoefficientAndMonomial(
      thePoly[theIndex],
      thePoly.coefficients[theIndex],
      found,
      &this->owner->theFormat
    );
    found = true;
    if (useHighlightStyle) {
      out << "}\\color{black}";
    }
    out << "$ ";
    if (i != this->allMonomials.size - 1) {
      out << "& ";
    }
  }
  if (countMons != thePoly.size()) {
    out << " Programming ERROR!";
  }
  return out.str();
}

template <class Coefficient>
std::string PolynomialDivisionReport<Coefficient>::getDivisionStringLaTeX() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::getDivisionStringLaTeX");
  this->checkInitialization();
  std::stringstream out;
  List<Polynomial<Coefficient> >& theRemainders = this->intermediateRemainders;
  List<Polynomial<Coefficient> >& theSubtracands = this->intermediateSubtractands;
  this->owner->theFormat.monomialOrder = this->owner->thePolynomialOrder.monomialOrder;
  std::string highlightedColor = "red";
  this->allMonomials.addOnTopNoRepetition(this->startingPolynomial.monomials);
  for (int i = 0; i < theRemainders.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theRemainders[i].monomials);
  }
  for (int i = 0; i < theSubtracands.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theSubtracands[i].monomials);
  }
  //List<std::string> basisColorStyles;
  //basisColorStyles.setSize(this->theBasis.size);
  this->allMonomials.quickSortDescending(&this->owner->thePolynomialOrder.monomialOrder);
  this->owner->theFormat.flagUseLatex = true;
  out << this->owner->toStringLetterOrder(true);
  out << theRemainders.size << " division steps total.";
  out << "\\renewcommand{\\arraystretch}{1.2}";
  out << "\\begin{longtable}{|c";
  for (int i = 0; i < this->allMonomials.size; i ++) {
    out << "c";
  }
  out << "|} \\hline";
  out << "&" <<  "\\multicolumn{" << this->allMonomials.size
  << "}{|c|}{\\textbf{Remainder}}" << "\\\\";
  out << "\\multicolumn{1}{|c|}{} & ";
  Polynomial<Coefficient>& remainder = this->owner->remainderDivision;
  out << this->getPolynomialStringSpacedMonomialsLaTeX(
    remainder, &highlightedColor, &remainder.monomials
  ) << "\\\\\\hline";
  out << "\\textbf{Divisor(s)} &" << "\\multicolumn{"
  << this->allMonomials.size << "}{|c|}{\\textbf{Quotient(s)}}"
  << "\\\\";
  for (int i = 0; i < this->owner->theBasis.size; i ++) {
    out << "$";
    out << this->owner->theBasis[i].element.toString(&this->owner->theFormat);
    out << "$";
    out << "& \\multicolumn{" << this->allMonomials.size << "}{|l|}{";
    out << "$" << this->owner->theQuotients[i].toString(&this->owner->theFormat) << "$" << "}\\\\\\hline\\hline";
  }
  for (int i = 0; i < theRemainders.size; i ++) {
    if (i < theRemainders.size - 1) {
      out << "$\\underline{~}$";
    }
    out << "&"
    << this->getPolynomialStringSpacedMonomialsLaTeX(
      theRemainders[i], &highlightedColor, &this->intermediateHighlightedMons[i]
    ) << "\\\\\n";
    if (i < theSubtracands.size) {
      out << "&";
      out << this->getPolynomialStringSpacedMonomialsLaTeX(
        theSubtracands[i], &highlightedColor
      ) << "\\\\\\cline{2-" << this->allMonomials.size + 1 << "}";
    }
  }
  out << "\\hline";
  out << "\\end{longtable}";
  return out.str();
}

template<class Coefficient>
std::string PolynomialDivisionReport<Coefficient>::getPolynomialStringSpacedMonomialsHtml(
  const Polynomial<Coefficient>& thePoly,
  const std::string& extraStyle,
  List<MonomialP>* theHighLightedMons
) {
  std::stringstream out;
  bool found = false;
  int countMons = 0;
  for (int i = 0; i < this->allMonomials.size; i ++) {
    int theIndex = thePoly.monomials.getIndex(this->allMonomials[i]);
    if (theIndex == - 1) {
      out << "<td" << extraStyle << ">" << "</td>";
      continue;
    }
    countMons ++;
    bool useHighlightStyle = false;
    if (theHighLightedMons != nullptr) {
      if (theHighLightedMons->contains(this->allMonomials[i])) {
        useHighlightStyle = true;
      }
    }
    out << "<td" << extraStyle << ">";
    if (useHighlightStyle) {
      out << "<span style ='color:red'>";
    }
    if (this->owner->theFormat.flagUseLatex) {
      std::string monomialWithCoefficient = Polynomial<Coefficient>::getBlendCoefficientAndMonomial(
        thePoly[theIndex],
        thePoly.coefficients[theIndex],
        found,
        &this->owner->theFormat
      );

      out << HtmlRoutines::getMathSpanPure(monomialWithCoefficient);
    } else {
      out << Polynomial<Coefficient>::getBlendCoefficientAndMonomial(
        thePoly[theIndex],
        thePoly.coefficients[theIndex],
        found,
        &this->owner->theFormat
      );
    }
    found = true;
    if (useHighlightStyle) {
      out << "</span>";
    }
    out << "</td>";
  }
  if (countMons != thePoly.size()) {
    out << "<td><b>Programming ERROR!</b></td>";
  }
  return out.str();
}

template <class Coefficient>
bool PolynomialDivisionReport<Coefficient>::checkInitialization() {
  if (this->owner == nullptr) {
    global.fatal << "PolynomialDivisionReport has zero owner." << global.fatal;
  }
  return false;
}

template <class Coefficient>
std::string PolynomialDivisionReport<Coefficient>::getDivisionStringHtml() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::getDivisionStringHtml");
  this->checkInitialization();
  std::stringstream out;
  List<Polynomial<Coefficient> >& theRemainders = this->intermediateRemainders;
  List<Polynomial<Coefficient> >& theSubtracands = this->intermediateSubtractands;
  this->owner->theFormat.monomialOrder = this->owner->thePolynomialOrder.monomialOrder;
  std::string underlineStyle = " style ='white-space: nowrap; border-bottom:1px solid black;'";
  this->allMonomials.clear();
  this->allMonomials.addOnTopNoRepetition(this->startingPolynomial.monomials);
  for (int i = 0; i < theRemainders.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theRemainders[i].monomials);
    if (i < theSubtracands.size) {
      this->allMonomials.addOnTopNoRepetition(theSubtracands[i].monomials);
    }
  }
  //List<std::string> basisColorStyles;
  //basisColorStyles.setSize(this->theBasis.size);
  this->allMonomials.quickSortDescending(&this->owner->thePolynomialOrder.monomialOrder);
  out << this->owner->toStringLetterOrder(false);
  out << "<br>";
  out << theRemainders.size << " division steps total.<br>";
  out << "<table style ='white-space: nowrap; border:1px solid black;'>";
  out << "<tr><td " << underlineStyle << "><b>Remainder:</b></td>";
  out << this->getPolynomialStringSpacedMonomialsHtml(
    this->owner->remainderDivision, underlineStyle, &this->owner->remainderDivision.monomials
  )
  << "</td></tr>";
  out << "<tr><td style ='border-right:1px solid black;'><b>Divisor(s)</b></td><td colspan ='"
  << this->allMonomials.size + 1 << "'><b>Quotient(s) </b></td>"
  << "</tr>";
  FormatExpressions& format = this->owner->theFormat;
  for (int i = 0; i < this->owner->theBasis.size; i ++) {
    out << "<tr>";
    out << "<td style ='border-right:1px solid black; border-bottom: 1px solid gray;'>";
    if (this->owner->theFormat.flagUseLatex) {
      out << HtmlRoutines::getMathSpanPure(this->owner->theBasis[i].element.toString(&format), - 1);
    } else {
      out << this->owner->theBasis[i].element.toString(&format);
    }
    out << "</td>";
    out << "<td style ='border-bottom:1px solid gray;' colspan ='"
    << this->allMonomials.size + 1 << "'>";
    out << HtmlRoutines::getMathSpanPure(this->owner->theQuotients[i].toString(&format));
    out << "</td></tr>";
  }
  out << "</tr>";
  if (theRemainders.size != this->intermediateHighlightedMons.size) {
    global.fatal << "Should have as many remainders: " << theRemainders.size
    << " as intermediate highlighted mons: "
    << this->intermediateHighlightedMons.size << global.fatal;
  }
  if (theRemainders.size != theSubtracands.size + 1) {
    global.fatal << "Remainders should equal subtracands plus one. " << global.fatal;
  }
  for (int i = 0; i < theRemainders.size; i ++) {
    out << "<tr><td></td>"
    << this->getPolynomialStringSpacedMonomialsHtml(
      theRemainders[i], "", &this->intermediateHighlightedMons[i]
    ) << "</tr>";
    if (i < theSubtracands.size) {
      out << "<tr><td>-</td></tr>";
      out << "<tr><td></td>" << this->getPolynomialStringSpacedMonomialsHtml(theSubtracands[i], underlineStyle)
      << "</tr>";
    }
  }
  out << "</table>";
  return out.str();
}

template<class Coefficient>
void PolynomialModuloPolynomial<Coefficient>::operator*=(
  const PolynomialModuloPolynomial& other
) {
  if (other.modulus != this->modulus) {
    global.fatal << "Not allowed to multiply quotient-ring "
    << "elements of different rings. [This modulus, other modulus]: "
    << this->modulus << ", " << other.modulus << global.fatal;
  }
  this->value *= other.value;
  this->reduce();
}

template<class Coefficient>
void PolynomialModuloPolynomial<Coefficient>::operator+=(
  const PolynomialModuloPolynomial& other
) {
  if (other.modulus != this->modulus) {
    global.fatal << "Not allowed to multiply quotient-ring "
    << "elements of different rings. [This modulus, other modulus]: "
    << this->modulus << ", " << other.modulus << global.fatal;
  }
  this->value += other.value;
  this->reduce();
}

template<class Coefficient>
unsigned int PolynomialModuloPolynomial<Coefficient>::hashFunction() const {
  return
  this->value.hashFunction() * someRandomPrimes[0] +
  this->modulus.hashFunction() * someRandomPrimes[1];
}

template<class Coefficient>
bool PolynomialModuloPolynomial<Coefficient>::operator==(
  const PolynomialModuloPolynomial<Coefficient>& other
) const {
  return this->modulus == other.modulus && this->value == other.value;
}

template<class Coefficient>
unsigned int PolynomialModuloPolynomial<Coefficient>::hashFunction(
  const PolynomialModuloPolynomial<Coefficient>& input
) {
  return input.hashFunction();
}

template<class Coefficient>
std::string PolynomialModuloPolynomial<Coefficient>::toString(
  FormatExpressions* theFormat
) {
  std::stringstream out;
  out << this->value.toString(theFormat) << "(mod (" << this->modulus.toString(theFormat) << "))";
  return out.str();
}

template<class Coefficient>
PolynomialModuloPolynomial<Coefficient> PolynomialModuloPolynomial<Coefficient>::one() {
  PolynomialModuloPolynomial<Coefficient> result;
  result.modulus = this->modulus;
  result.value.makeConstant(this->modulus.coefficients[0].one());
  return result;
}

template<class Coefficient>
bool PolynomialModuloPolynomial<Coefficient>::isEqualToZero() const {
  return this->value.isEqualToZero();
}

template<class Coefficient>
void PolynomialModuloPolynomial<Coefficient>::reduce() {
  Polynomial<Coefficient> unusedQuotient;
  this->value.divideBy(
    this->modulus,
    unusedQuotient,
    this->value,
    &MonomialP::orderDefault()
  );
}

#endif
