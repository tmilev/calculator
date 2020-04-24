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
  if (this->IsConstant()) {
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
      if (theSubstitution[i].IsMonomialCoeffOne()) {
        MonomialP tempMon = theSubstitution[i][0];
        tempMon.raiseToPower(this->monBody[i]);
        output *= tempMon;
        continue;
      }
      global.comments << "This may or may not be a programming error. "
      << "I cannot carry out a substitution in a monomial that has exponent "
      << "which is not a small integer: it is " << this->monBody[i]
      << " instead. " << GlobalVariables::Crasher::GetStackTraceEtcErrorMessageHTML();
      return false;
    }
    tempPoly = theSubstitution[i];
    tempPoly.raiseToPower(theExponent, 1);
    output *= tempPoly;
  }
  return true;
}

template<class Coefficient>
bool Polynomial<Coefficient>::IsOneVariableNonConstPoly(int* whichVariable) const {
  int tempInt;
  if (whichVariable == nullptr) {
    whichVariable = &tempInt;
  }
  if (!this->IsOneVariablePoly(whichVariable)) {
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
Rational Polynomial<Coefficient>::RationalValue() {
  Rational result;
  this->GetConstantTerm(result, 0);
  return result;
}

template<class Coefficient>
bool Polynomial<Coefficient>::IsOneVariablePoly(int* whichVariable) const {
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
void Polynomial<Coefficient>::MakeDeterminantFromSquareMatrix(
  const Matrix<Polynomial<Coefficient> >& theMat
) {
  if (theMat.numberOfColumns != theMat.numberOfRows) {
    global.fatal << "Cannot compute determinant: matrix has "
    << theMat.numberOfRows << " rows and " << theMat.numberOfColumns
    << " columns. " << global.fatal;
  }
  Permutation thePerm;
  thePerm.initPermutation(theMat.numberOfRows);
  int numCycles = thePerm.GetNumPermutations();
  List<int> permutationIndices;
  thePerm.GetPermutationLthElementIsTheImageofLthIndex(permutationIndices);
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
int Polynomial<Coefficient>::TotalDegreeInt() const {
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
    result = MathRoutines::Maximum((*this)[i].TotalDegree(), result);
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
void Polynomial<Coefficient>::makeOne(int ExpectedNumVars) {
  this->makeConstant(1, ExpectedNumVars);
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
Coefficient Polynomial<Coefficient>::Evaluate(const Vector<Coefficient>& input) {
  MacroRegisterFunctionWithName("Polynomial::Evaluate");
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
void Polynomial<Coefficient>::SetNumVariablesSubDeletedVarsByOne(int newNumVars) {
  MacroRegisterFunctionWithName("Polynomial_CoefficientType::SetNumVariablesSubDeletedVarsByOne");
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
    tempM.makeOne(newNumVars);
    for (int j = 0; j < newNumVars; j ++) {
      tempM.setVariable(j, (*this)[i](j));
    }
    Accum.addMonomial(tempM, this->coefficients[i]);
  }
  this->operator=(Accum);
}

template <class Coefficient>
bool Polynomial<Coefficient>::hasSmallIntegralPositivePowers(
  int* whichTotalDegree
) const {
  int whichTotalDegreeContainer = 0;
  for (int i = 0; i < this->size(); i ++) {
    if (!this->theMonomials[i].hasSmallIntegralPositivePowers(&whichTotalDegreeContainer)) {
      return false;
    }
  }
  if (whichTotalDegree != nullptr) {
    *whichTotalDegree = whichTotalDegreeContainer;
  }
  return true;
}

template <class Coefficient>
void Polynomial<Coefficient>::ShiftVariableIndicesToTheRight(int VarIndexShift) {
  if (VarIndexShift < 0) {
    global.fatal << "This is a programming error. "
    << "Requesting negative variable shift (more precisely, "
    << VarIndexShift << ") not allowed. " << global.fatal;
  }
  if (VarIndexShift == 0) {
    return;
  }
  int oldNumVars = this->minimalNumberOfVariables();
  int newNumVars = oldNumVars + VarIndexShift;
  Polynomial<Coefficient> Accum;
  Accum.makeZero();
  Accum.setExpectedSize(this->size());
  MonomialP tempM;
  for (int i = 0; i < this->size(); i ++) {
    tempM.makeOne(newNumVars);
    for (int j = 0; j < oldNumVars; j ++) {
      tempM[j + VarIndexShift] = (*this)[i](j);
    }
    Accum.addMonomial(tempM, this->coefficients[i]);
  }
  *this = Accum;
}

template <class Coefficient>
bool Polynomial<Coefficient>::IsEqualToOne() const {
  Coefficient tempC;
  if (this->IsConstant(&tempC)) {
    return tempC.IsEqualToOne();
  }
  return false;
}

template <class Coefficient>
bool Polynomial<Coefficient>::IsMonomialCoeffOne() const {
  if (this->size() != 1) {
    return false;
  }
  return this->coefficients[0].IsEqualToOne();
}

template <class Coefficient>
bool Polynomial<Coefficient>::IsOneLetterFirstDegree(int* whichLetter) const {
  if (this->size() != 1) {
    return false;
  }
  return (*this)[0].IsOneLetterFirstDegree(whichLetter);
}

template <class Coefficient>
bool Polynomial<Coefficient>::IsConstant(Coefficient* whichConstant) const {
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
  return theMon.IsConstant();
}

template <class Coefficient>
bool Polynomial<Coefficient>::isNegative() const {
  Coefficient tempC;
  if (!this->IsConstant(&tempC)) {
    return false;
  }
  return tempC.isNegative();
}

template <class Coefficient>
bool Polynomial<Coefficient>::IsLinearNoConstantTerm() {
  for (int i = 0; i < this->size; i ++) {
    if (!this->theObjects[i].IsLinearNoConstantTerm()) {
      return false;
    }
  }
  return true;
}

template <class Coefficient>
bool Polynomial<Coefficient>::IsLinear() {
  for (int i = 0; i < this->size(); i ++) {
    if (!(*this)[i].IsLinear()) {
      return false;
    }
  }
  return true;
}

template <class Coefficient>
bool Polynomial<Coefficient>::IsLinearGetRootConstantTermLastCoordinate(Vector<Coefficient>& outputRoot) {
  outputRoot.makeZero(this->minimalNumberOfVariables() + 1);
  int index;
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].IsConstant()) {
      *outputRoot.lastObject() = this->coefficients[i];
    } else {
      if ((*this)[i].IsOneLetterFirstDegree(&index)) {
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
bool Polynomial<Coefficient>::GetRootFromLinPolyConstTermLastVariable(Vector<Coefficient>& outputRoot) {
  return this->IsLinearGetRootConstantTermLastCoordinate(outputRoot);
}

template <class Coefficient>
Matrix<Coefficient> Polynomial<Coefficient>::EvaluateUnivariatePoly(
  const Matrix<Coefficient>& input
) {
  // for univariate polynomials only
  MacroRegisterFunctionWithName("Polynomial::EvaluateUnivariatePoly");
  Matrix<Coefficient> output, tempElt, idMat;
  idMat.MakeIdMatrix(input.numberOfColumns);
  output.MakeZeroMatrix(input.numberOfColumns);
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
int Polynomial<Coefficient>::GetHighestIndexSuchThatHigherIndexVarsDontParticipate() {
  int result = - 1;
  for (int i = 0; i < this->size; i ++) {
    result = MathRoutines::Maximum(
      result,
      this->theObjects[i].GetHighestIndexSuchThatHigherIndexVarsDontParticipate()
    );
  }
  return result;
}

template <class Coefficient>
void Polynomial<Coefficient>::ScaleToPositiveMonomialExponents(MonomialP& outputScale) {
  int numVars = this->minimalNumberOfVariables();
  outputScale.makeOne(numVars);
  for (int i = 0; i < numVars; i ++) {
    for (int j = 0; j < this->size(); j ++) {
      const MonomialP& currentMonomial = (*this)[j];
      Rational currentScale = MathRoutines::Minimum(outputScale(i), currentMonomial(i));
      outputScale.setVariable(i, currentScale);
    }
  }
  outputScale.invert();
  this->multiplyBy(outputScale);
}

template <class Coefficient>
bool Polynomial<Coefficient>::operator<=(const Coefficient& other) const {
  Coefficient constME;
  if (!this->IsConstant(&constME)) {
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
  if (!this->IsConstant(&constME)) {
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
  int thisMaxMonIndex = this->GetIndexLeadingMonomial(&thisMaximalMonomial, nullptr, &monomialOrder);
  int otherMaxMonIndex = other.GetIndexLeadingMonomial(&otherMaximalMonomial, nullptr, &monomialOrder);
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
bool Polynomial<Coefficient>::IsGEQcompareByTopMonomialTotalDegThenLexicographic(
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
bool Polynomial<Coefficient>::IsGEQcompareByTopMonomialLexicographicLastVarStrongest(
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
bool Polynomial<Coefficient>::IsEqualTo(const Polynomial<Coefficient>& p) const {
  return *this == p;
}

template <class Coefficient>
void Polynomial<Coefficient>::operator-=(int x) {
  MonomialP tempMon;
  tempMon.makeOne();
  this->SubtractMonomial(tempMon, x);
}

template <class Coefficient>
void Polynomial<Coefficient>::operator-=(const Coefficient& other) {
  MonomialP tempMon;
  tempMon.makeOne();
  this->SubtractMonomial(tempMon, other);
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
  this->DivideBy(other, temp, result, &MonomialP::orderDefault());
  return result;
}

template <class Coefficient>
void Polynomial<Coefficient>::operator/=(const Polynomial<Coefficient>& other) {
  Polynomial<Coefficient> tempMe = *this;
  Polynomial<Coefficient> tempRemainder;
  tempMe.DivideBy(other, *this, tempRemainder, &MonomialP::orderDefault());
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
void Polynomial<Coefficient>::AssignOtherType(const Polynomial<otherType>& other) {
  this->NumVars = other.NumVars;
  this->::LinearCombination<MonomialP, Coefficient>::AssignOtherType(other);
}

template <class Coefficient>
bool Polynomial<Coefficient>::operator<=(const Polynomial<Coefficient>& other) const {
  return !(*this > other);
}

template <class Coefficient>
bool Polynomial<Coefficient>::IsProportionalTo(
  const Polynomial<Coefficient>& other,
  Coefficient& TimesMeEqualsOther,
  const Coefficient& theRingUnit
) const {
  if (this->size() != other.size()) {
    return false;
  }
  if (other.size() == 0) {
    TimesMeEqualsOther = theRingUnit;
    return true;
  }
  const MonomialP& firstMon = (*this)[0];
  int indexInOther = other.theMonomials.getIndex(firstMon);
  if (indexInOther == - 1) {
    return false;
  }
  TimesMeEqualsOther = other.coefficients[indexInOther];
  TimesMeEqualsOther /= this->coefficients[0];
  Polynomial<Coefficient> tempP;
  tempP = *this;
  tempP *= TimesMeEqualsOther;
  tempP -= other;
  return tempP.isEqualToZero();
}

template <class Coefficient>
void Polynomial<Coefficient>::DivideBy(
  const Polynomial<Coefficient>& inputDivisor,
  Polynomial<Coefficient>& outputQuotient,
  Polynomial<Coefficient>& outputRemainder,
  typename List<MonomialP>::Comparator* monomialOrder
) const {
  MacroRegisterFunctionWithName("Polynomial::DivideBy");
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
    this->DivideBy(inputDivisor, newQuotient, newRemainder, monomialOrder);
    outputQuotient = newQuotient;
    outputRemainder = newRemainder;
    return;
  }
  outputRemainder = *this;
  MonomialP scaleRemainder, scaleInput;
  Polynomial<Coefficient> divisorShiftedExponents = inputDivisor;
  outputRemainder.ScaleToPositiveMonomialExponents(scaleRemainder);
  divisorShiftedExponents.ScaleToPositiveMonomialExponents(scaleInput);
  MonomialP remainderLeadingMonomial;
  Coefficient remainderLeadingCoefficient;
  int remainderLeadingIndex = outputRemainder.GetIndexLeadingMonomial(
    &remainderLeadingMonomial, &remainderLeadingCoefficient, monomialOrder
  );
  MonomialP leadingMonomialShiftedDivisor;
  Coefficient leadingCoefficientShiftedDivisor;
  divisorShiftedExponents.GetIndexLeadingMonomial(
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
    if (!quotientMonomial.HasPositiveOrZeroExponents()) {
      break;
    }
    Coefficient quotientCoefficient = remainderLeadingCoefficient;
    quotientCoefficient /= leadingCoefficientShiftedDivisor;
    outputQuotient.addMonomial(quotientMonomial, quotientCoefficient);
    subtracand = divisorShiftedExponents;
    subtracand.multiplyBy(quotientMonomial, quotientCoefficient);
    outputRemainder -= subtracand;
    int remainderIndex = outputRemainder.GetIndexLeadingMonomial(
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
void Polynomial<Coefficient>::TimesInteger(int a) {
  Rational r;
  r.AssignInteger(a);
  this->TimesRational(r);
}

template <class Coefficient>
void Polynomial<Coefficient>::AssignCharPoly(const Matrix<Coefficient>& input) {
  MacroRegisterFunctionWithName("Polynomial::AssignCharPoly");
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
    currenCF = - acc.GetTrace() / i;
    this->addMonomial(MonomialP(0, i), currenCF);
    for (int j = 0; j < n; j ++) {
      acc(j, j) += currenCF;
    }
    acc.multiplyOnTheLeft(input);
  }
  this->addMonomial(MonomialP(0, n), - acc.GetTrace() / n);
}

template <class Coefficient>
void Polynomial<Coefficient>::AssignMinPoly(const Matrix<Coefficient>& input) {
  MacroRegisterFunctionWithName("Polynomial::AssignMinPoly");
  if (input.numberOfColumns != input.numberOfRows) {
    global.fatal << "Programming error: requesting the "
    << "minimimal polynomial of a non-square matrix. "
    << global.fatal;
  }
  int theDim = input.numberOfColumns;
  this->makeOne(1);
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
      if (theBasis.LinSpanContainsVector(theVectorPowers)) {
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
    *this = MathRoutines::lcm(*this, currentFactor);
  }
  this->scaleNormalizeLeadingMonomial();
}

template <class Coefficient>
int Polynomial<Coefficient>::GetMaxPowerOfVariableIndex(int VariableIndex) {
  int result = 0;
  for (int i = 0; i < this->size(); i ++) {
    result = MathRoutines::Maximum(result, (*this)[i](VariableIndex).numeratorShort);
    if (!(*this)[i](VariableIndex).isSmallInteger()) {
      global.fatal << " This is a programming error: "
      << "GetMaxPowerOfVariableIndex called on a polynomial whose monomials "
      << "have degrees that are not small integers. "
      << "This needs to be fixed! " << global.fatal;
    }
  }
  return result;
}

template <class Coefficient>
void Polynomial<Coefficient>::GetConstantTerm(Coefficient& output, const Coefficient& theRingZero) const {
  MonomialP tempM;
  tempM.makeOne();
  int i = this->theMonomials.getIndex(tempM);
  if (i == - 1) {
    output = theRingZero;
  } else {
    output = this->coefficients[i];
  }
}

template <class Coefficient>
void Polynomial<Coefficient>::makeMonomial(
  int letterIndex,
  const Rational& power,
  const Coefficient& inputCoefficient,
  int ExpectedNumVars
) {
  if (letterIndex < 0) {
    global.fatal << "Negative variable index: "
    << letterIndex << " is not allowed. "
    << global.fatal;
  }
  int numVars = MathRoutines::Maximum(letterIndex + 1, ExpectedNumVars);
  this->makeZero();
  MonomialP tempM;
  tempM.makeOne(numVars);
  tempM.setVariable(letterIndex, power);
  this->addMonomial(tempM, inputCoefficient);
}

template <class Coefficient>
void Polynomial<Coefficient>::GetCoeffInFrontOfLinearTermVariableIndex(
  int index, Coefficient& output
) {
  MonomialP tempM;
  tempM.makeEi(index);
  int i = this->theMonomials.getIndex(tempM);
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
  if (this->IsConstant()) {
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
    const MonomialP& currentMonomial = this->theMonomials[i];
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
bool PolynomialOrder<Coefficient>::CompareLeftGreaterThanRight(
  const Polynomial<Coefficient>& left, const Polynomial<Coefficient>& right
) const {
  MacroRegisterFunctionWithName("PolynomialOrder::CompareLeftGreaterThanRight");
  List<MonomialP> sortedLeft = left.theMonomials;
  List<MonomialP> sortedRight = right.theMonomials;
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
    Coefficient leftCoefficient = left.GetMonomialCoefficient(leftMonomial);
    Coefficient rightCoefficient = right.GetMonomialCoefficient(rightMonomial);
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
std::string GroebnerBasisComputation<Coefficient>::GetPolynomialStringSpacedMonomialsLaTeX(
  const Polynomial<Coefficient>& thePoly,
  std::string* highlightColor,
  List<MonomialP>* theHighLightedMons,
  int* firstNonZeroIndex
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::GetPolynomialStringSpacedMonomialsLaTeX");
  std::stringstream out;
  bool found = false;
  int countMons = 0;
  if (firstNonZeroIndex != nullptr) {
    *firstNonZeroIndex = - 1;
  }
  for (int i = 0; i < this->allMonomials.size; i ++) {
    int theIndex = thePoly.theMonomials.getIndex(this->allMonomials[i]);
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
      &this->theFormat
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
std::string GroebnerBasisComputation<Coefficient>::GetDivisionStringLaTeX() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::GetDivisionStringLaTeX");
  std::stringstream out;
  List<Polynomial<Coefficient> >& theRemainders = this->intermediateRemainders.getElement();
  List<Polynomial<Coefficient> >& theSubtracands = this->intermediateSubtractands.getElement();
  this->theFormat.monomialOrder = this->thePolynomialOrder.monomialOrder;
  std::string HighlightedColor = "red";
  this->allMonomials.addOnTopNoRepetition(this->startingPoly.getElement().theMonomials);
  for (int i = 0; i < theRemainders.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theRemainders[i].theMonomials);
  }
  for (int i = 0; i < theSubtracands.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theSubtracands[i].theMonomials);
  }
  //List<std::string> basisColorStyles;
  //basisColorStyles.setSize(this->theBasiS.size);
  this->allMonomials.quickSortDescending(&this->thePolynomialOrder.monomialOrder);
  this->theFormat.flagUseLatex = true;
  out << this->ToStringLetterOrder(true);
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
  out << this->GetPolynomialStringSpacedMonomialsLaTeX(
    this->remainderDivision, &HighlightedColor, &this->remainderDivision.theMonomials
  ) << "\\\\\\hline";
  out << "\\textbf{Divisor(s)} &" << "\\multicolumn{"
  << this->allMonomials.size << "}{|c|}{\\textbf{Quotient(s)}}"
  << "\\\\";
  for (int i = 0; i < this->theBasiS.size; i ++) {
    out << "$";
    out << this->theBasiS[i].toString(&this->theFormat);
    out << "$";
    out << "& \\multicolumn{" << this->allMonomials.size << "}{|l|}{";
    out << "$" << this->theQuotients[i].toString(&this->theFormat) << "$" << "}\\\\\\hline\\hline";
  }
  for (int i = 0; i < theRemainders.size; i ++) {
    if (i < theRemainders.size - 1) {
      out << "$\\underline{~}$";
    }
    out << "&"
    << this->GetPolynomialStringSpacedMonomialsLaTeX
    (theRemainders[i], &HighlightedColor, &this->intermediateHighlightedMons.getElement()[i])
    << "\\\\\n";
    if (i < theSubtracands.size) {
      out << "&";
      out << this->GetPolynomialStringSpacedMonomialsLaTeX
      (theSubtracands[i], &HighlightedColor)
      << "\\\\\\cline{2-" << this->allMonomials.size + 1 << "}";
    }
  }
  out << "\\hline";
  out << "\\end{longtable}";
  return out.str();
}

template<class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::GetPolynomialStringSpacedMonomialsHtml(
  const Polynomial<Coefficient>& thePoly,
  const std::string& extraStyle,
  List<MonomialP>* theHighLightedMons
) {
  std::stringstream out;
  bool found = false;
  int countMons = 0;
  for (int i = 0; i < this->allMonomials.size; i ++) {
    int theIndex = thePoly.theMonomials.getIndex(this->allMonomials[i]);
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
    if (this->theFormat.flagUseLatex) {
      std::string monomialWithCoefficient = Polynomial<Coefficient>::getBlendCoefficientAndMonomial(
        thePoly[theIndex],
        thePoly.coefficients[theIndex],
        found,
        &this->theFormat
      );

      out << HtmlRoutines::GetMathSpanPure(monomialWithCoefficient);
    } else {
      out << Polynomial<Coefficient>::getBlendCoefficientAndMonomial(
        thePoly[theIndex],
        thePoly.coefficients[theIndex],
        found,
        &this->theFormat
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
std::string GroebnerBasisComputation<Coefficient>::GetDivisionStringHtml() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::GetDivisionStringHtml");
  std::stringstream out;
  List<Polynomial<Coefficient> >& theRemainders = this->intermediateRemainders.getElement();
  List<Polynomial<Coefficient> >& theSubtracands = this->intermediateSubtractands.getElement();
  this->theFormat.monomialOrder = this->thePolynomialOrder.monomialOrder;
  std::string underlineStyle = " style ='white-space: nowrap; border-bottom:1px solid black;'";
  this->allMonomials.clear();
  this->allMonomials.addOnTopNoRepetition(this->startingPoly.getElement().theMonomials);
  for (int i = 0; i < theRemainders.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theRemainders[i].theMonomials);
    if (i < theSubtracands.size) {
      this->allMonomials.addOnTopNoRepetition(theSubtracands[i].theMonomials);
    }
  }
  //List<std::string> basisColorStyles;
  //basisColorStyles.setSize(this->theBasiS.size);
  this->allMonomials.quickSortDescending(&this->thePolynomialOrder.monomialOrder);
  out << this->ToStringLetterOrder(false);
  out << "<br>";
  out << theRemainders.size << " division steps total.<br>";
  out << "<table style ='white-space: nowrap; border:1px solid black;'>";
  out << "<tr><td " << underlineStyle << "><b>Remainder:</b></td>";
  out << this->GetPolynomialStringSpacedMonomialsHtml(
    this->remainderDivision, underlineStyle, &this->remainderDivision.theMonomials
  )
  << "</td></tr>";
  out << "<tr><td style ='border-right:1px solid black;'><b>Divisor(s)</b></td><td colspan ='"
  << this->allMonomials.size + 1 << "'><b>Quotient(s) </b></td>"
  << "</tr>";
  for (int i = 0; i < this->theBasiS.size; i ++) {
    out << "<tr>";
    out << "<td style ='border-right:1px solid black; border-bottom: 1px solid gray;'>";
    if (this->theFormat.flagUseLatex) {
      out << HtmlRoutines::GetMathSpanPure(this->theBasiS[i].toString(&this->theFormat), - 1);
    } else {
      out << this->theBasiS[i].toString(&this->theFormat);
    }
    out << "</td>";
    out << "<td style ='border-bottom:1px solid gray;' colspan ='"
    << this->allMonomials.size + 1 << "'>";
    out << HtmlRoutines::GetMathSpanPure(this->theQuotients[i].toString(&this->theFormat));
    out << "</td></tr>";
  }
  out << "</tr>";
  if (theRemainders.size != this->intermediateHighlightedMons.getElement().size) {
    global.fatal << "Should have as many remainders: " << theRemainders.size
    << " as intermediate highlighted mons: "
    << this->intermediateHighlightedMons.getElement().size << global.fatal;
  }
  if (theRemainders.size != theSubtracands.size + 1) {
    global.fatal << "Remainders should equal subtracands plus one. " << global.fatal;
  }
  for (int i = 0; i < theRemainders.size; i ++) {
    out << "<tr><td></td>"
    << this->GetPolynomialStringSpacedMonomialsHtml(
      theRemainders[i], "", &this->intermediateHighlightedMons.getElement()[i]
    ) << "</tr>";
    if (i < theSubtracands.size) {
      out << "<tr><td>-</td></tr>";
      out << "<tr><td></td>" << this->GetPolynomialStringSpacedMonomialsHtml(theSubtracands[i], underlineStyle)
      << "</tr>";
    }
  }
  out << "</table>";
  return out.str();
}

template<class Coefficient>
void PolynomialModuloPolynomial<Coefficient>::operator*=(
  PolynomialModuloPolynomial& other
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
  this->value.DivideBy(
    this->modulus,
    unusedQuotient,
    this->value,
    &MonomialP::orderDefault()
  );
}

#endif
