// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfImplementationHeaderPolynomialComputationsBasic_already_included
#define vpfImplementationHeaderPolynomialComputationsBasic_already_included

#include "math_general.h"

template <class coefficient>
bool MonomialP::SubstitutioN(const List<Polynomial<coefficient> >& TheSubstitution, Polynomial<coefficient>& output) const {
  MacroRegisterFunctionWithName("MonomialP::Substitution");
  output.MakeConst(1);
  if (this->IsConstant()) {
    return true;
  }
  Polynomial<coefficient> tempPoly;
  for (int i = 0; i < this->monBody.size; i ++) {
    if (this->monBody[i] != 0) {
      if (i >= TheSubstitution.size) {
        global.fatal << "This is a programming error. Attempting to carry out a substitution in the monomial "
        << this->toString()
        << " which does have non-zero exponent of variable x_{" << i + 1 << "}; however, the input substitution has "
        << TheSubstitution.size
        << " variable images. More precisely, the input substitution is:  "
        << TheSubstitution.toString() << ". " << global.fatal;
      }
      int theExponent = 0;
      if (!this->monBody[i].IsSmallInteger(&theExponent) || this->monBody[i] < 0) {
        if (TheSubstitution[i].IsMonomialCoeffOne()) {
          MonomialP tempMon = TheSubstitution[i][0];
          tempMon.RaiseToPower(this->monBody[i]);
          output *= tempMon;
          continue;
        }
        global.Comments << "This may or may not be a programming error. "
        << "I cannot carry out a substitution in a monomial that has exponent "
        << "which is not a small integer: it is " << this->monBody[i]
        << " instead. " << GlobalVariables::Crasher::GetStackTraceEtcErrorMessageHTML();
        return false;
      }
      tempPoly = TheSubstitution[i];
      tempPoly.RaiseToPower(theExponent);
      output *= tempPoly;
    }
  }
  return true;
}

template<class coefficient>
bool Polynomial<coefficient>::IsOneVariableNonConstPoly(int* whichVariable) const {
  int tempInt;
  if (whichVariable == nullptr) {
    whichVariable = &tempInt;
  }
  if (!this->IsOneVariablePoly(whichVariable)) {
    return false;
  }
  return *whichVariable != - 1;
}

template<class coefficient>
Polynomial<coefficient> Polynomial<coefficient>::one() const {
  Polynomial<coefficient> result;
  result.makeOne();
  return result;
}

template<class coefficient>
Polynomial<coefficient> Polynomial<coefficient>::zero() {
  Polynomial<coefficient> result;
  result.makeZero();
  return result;
}

template<class coefficient>
Rational Polynomial<coefficient>::RationalValue() {
  Rational result;
  this->GetConstantTerm(result, 0);
  return result;
}

template<class coefficient>
bool Polynomial<coefficient>::IsOneVariablePoly(int* whichVariable) const {
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

template <class coefficient>
void Polynomial<coefficient>::MakeDeterminantFromSquareMatrix(
  const Matrix<Polynomial<coefficient> >& theMat
) {
  if (theMat.NumCols != theMat.NumRows) {
    global.fatal << "Cannot compute determinant: matrix has "
    << theMat.NumRows << " rows and " << theMat.NumCols
    << " columns. " << global.fatal;
  }
  permutation thePerm;
  thePerm.initPermutation(theMat.NumRows);
  int numCycles = thePerm.GetNumPermutations();
  List<int> permutationIndices;
  thePerm.GetPermutationLthElementIsTheImageofLthIndex(permutationIndices);
  Polynomial<coefficient> result, theMonomial;
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

template <class coefficient>
int Polynomial<coefficient>::TotalDegreeInt() const {
  int result = - 1;
  if (!this->totalDegree().IsSmallInteger(&result)) {
    global.fatal
    << "This is a programming error: requested total degree of a "
    << "polynomial in int formal, but the "
    << "degree of the polynomial is not a small integer. "
    << global.fatal;
  }
  return result;
}

template <class coefficient>
Rational Polynomial<coefficient>::totalDegree() const {
  Rational result = 0;
  for (int i = 0; i < this->size(); i ++) {
    result = MathRoutines::Maximum((*this)[i].TotalDegree(), result);
  }
  return result;
}

template <class coefficient>
bool Polynomial<coefficient>::Substitution(
  const List<Polynomial<coefficient> >& TheSubstitution
) {
  MacroRegisterFunctionWithName("Polynomial::Substitution");
  Polynomial<coefficient> Accum, TempPoly;
  for (int i = 0; i < this->size(); i ++) {
    if (!(*this)[i].SubstitutioN(TheSubstitution, TempPoly)) {
      return false;
    }
    TempPoly *= this->coefficients[i];
    Accum += TempPoly;
  }
  *this = Accum;
  return true;
}

template <class coefficient>
void Polynomial<coefficient>::makeOne(int ExpectedNumVars) {
  this->MakeConst(1, ExpectedNumVars);
}

template <class coefficient>
void Polynomial<coefficient>::makeDegreeOne(
  int NVar, int NonZeroIndex, const coefficient& coeff
) {
  this->makeZero();
  MonomialP tempM;
  tempM.MakeEi(NonZeroIndex, 1, NVar);
  this->AddMonomial(tempM, coeff);
}

template <class coefficient>
void Polynomial<coefficient>::makeDegreeOne(
  int NVar,
  int NonZeroIndex1,
  int NonZeroIndex2,
  const coefficient& coeff1,
  const coefficient& coeff2
) {
  (void) NVar;
  this->makeZero();
  MonomialP tempM;
  tempM.MakeEi(NonZeroIndex1);
  this->AddMonomial(tempM, coeff1);
  tempM.MakeEi(NonZeroIndex2);
  this->AddMonomial(tempM, coeff2);
}

template <class coefficient>
void Polynomial<coefficient>::makeDegreeOne(
  int NVar,
  int NonZeroIndex,
  const coefficient& coeff1,
  const coefficient& ConstantTerm
) {
  this->makeDegreeOne(NVar, NonZeroIndex, coeff1);
  *this += ConstantTerm;
}

template <class coefficient>
coefficient Polynomial<coefficient>::Evaluate(const Vector<coefficient>& input) {
  MacroRegisterFunctionWithName("Polynomial::Evaluate");
  coefficient output = 0;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialP& currentMon = (*this)[i];
    coefficient accum = this->coefficients[i];
    coefficient tempElt;
    for (int j = 0; j < currentMon.minimalNumberOfVariables(); j ++) {
      int numCycles = 0;
      if (!(*this)[i](j).IsSmallInteger(&numCycles)) {
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
      MathRoutines::RaiseToPower(tempElt, numCycles, static_cast<coefficient>(1));
      if (!isPositive) {
        tempElt.invert();
      }
      accum *= tempElt;
    }
    output += accum;
  }
  return output;
}

template <class coefficient>
void Polynomial<coefficient>::SetNumVariablesSubDeletedVarsByOne(int newNumVars) {
  MacroRegisterFunctionWithName("Polynomial_CoefficientType::SetNumVariablesSubDeletedVarsByOne");
  if (newNumVars >= this->minimalNumberOfVariables()) {
    return;
  }
  if (newNumVars < 0) {
    global.fatal << "This is a programming error. "
    << "Requesting negative number of variables (more precisely, "
    << newNumVars << ") is not allowed. " << global.fatal;
  }
  Polynomial<coefficient> Accum;
  Accum.makeZero();
  Accum.setExpectedSize(this->size());
  MonomialP tempM;
  for (int i = 0; i < this->size(); i ++) {
    tempM.makeOne(newNumVars);
    for (int j = 0; j < newNumVars; j ++) {
      tempM.setVariable(j, (*this)[i](j));
    }
    Accum.AddMonomial(tempM, this->coefficients[i]);
  }
  this->operator=(Accum);
}

template <class coefficient>
bool Polynomial<coefficient>::hasSmallIntegralPositivePowers(
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

template <class coefficient>
void Polynomial<coefficient>::ShiftVariableIndicesToTheRight(int VarIndexShift) {
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
  Polynomial<coefficient> Accum;
  Accum.makeZero();
  Accum.setExpectedSize(this->size());
  MonomialP tempM;
  for (int i = 0; i < this->size(); i ++) {
    tempM.makeOne(newNumVars);
    for (int j = 0; j < oldNumVars; j ++) {
      tempM[j + VarIndexShift] = (*this)[i](j);
    }
    Accum.AddMonomial(tempM, this->coefficients[i]);
  }
  *this = Accum;
}

template <class coefficient>
bool Polynomial<coefficient>::IsEqualToOne() const {
  coefficient tempC;
  if (this->IsConstant(&tempC)) {
    return tempC.IsEqualToOne();
  }
  return false;
}

template <class coefficient>
bool Polynomial<coefficient>::IsMonomialCoeffOne() const {
  if (this->size() != 1) {
    return false;
  }
  return this->coefficients[0].IsEqualToOne();
}

template <class coefficient>
bool Polynomial<coefficient>::IsOneLetterFirstDegree(int* whichLetter) const {
  if (this->size() != 1) {
    return false;
  }
  return (*this)[0].IsOneLetterFirstDegree(whichLetter);
}

template <class coefficient>
bool Polynomial<coefficient>::IsConstant(coefficient* whichConstant) const {
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

template <class coefficient>
bool Polynomial<coefficient>::IsNegative() const {
  coefficient tempC;
  if (!this->IsConstant(&tempC)) {
    return false;
  }
  return tempC.IsNegative();
}

template <class coefficient>
bool Polynomial<coefficient>::IsLinearNoConstantTerm() {
  for (int i = 0; i < this->size; i ++) {
    if (!this->theObjects[i].IsLinearNoConstantTerm()) {
      return false;
    }
  }
  return true;
}

template <class coefficient>
bool Polynomial<coefficient>::IsLinear() {
  for (int i = 0; i < this->size(); i ++) {
    if (!(*this)[i].IsLinear()) {
      return false;
    }
  }
  return true;
}

template <class coefficient>
bool Polynomial<coefficient>::IsLinearGetRootConstantTermLastCoordinate(Vector<coefficient>& outputRoot) {
  outputRoot.makeZero(this->minimalNumberOfVariables() + 1);
  int index;
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].IsConstant()) {
      *outputRoot.LastObject() = this->coefficients[i];
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

template <class coefficient>
void Polynomial<coefficient>::RaiseToPower(int d) {
  if (d == 1) {
    return;
  }
  if (d < 0) {
    global.fatal << "This is a programming error: attempting to raise the polynomial "
    << this->toString() << " to the negative power "
    << d << ". " << global.fatal;
  }
  Polynomial<coefficient> theOne;
  theOne.makeOne(this->minimalNumberOfVariables());
  MathRoutines::RaiseToPower(*this, d, theOne);
}

template <class coefficient>
bool Polynomial<coefficient>::GetRootFromLinPolyConstTermLastVariable(Vector<coefficient>& outputRoot) {
  return this->IsLinearGetRootConstantTermLastCoordinate(outputRoot);
}

template <class coefficient>
Matrix<coefficient> Polynomial<coefficient>::EvaluateUnivariatePoly(
  const Matrix<coefficient>& input
) {
  // for univariate polynomials only
  MacroRegisterFunctionWithName("Polynomial::EvaluateUnivariatePoly");
  Matrix<coefficient> output, tempElt, idMat;
  idMat.MakeIdMatrix(input.NumCols);
  output.MakeZeroMatrix(input.NumCols);
  for (int i = 0; i < this->size; i ++) {
    const MonomialP& currentMon = (*this)[i];
    int numCycles = 0;
    if (!currentMon(0).IsSmallInteger(&numCycles) ) {
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
    MathRoutines::RaiseToPower(tempElt, numCycles, idMat);
    if (!isPositive) {
      tempElt.Invert();
    }
    tempElt *= this->coefficients[i];
    output += tempElt;
  }
  return output;
}

template <class coefficient>
int Polynomial<coefficient>::GetHighestIndexSuchThatHigherIndexVarsDontParticipate() {
  int result = - 1;
  for (int i = 0; i < this->size; i ++) {
    result = MathRoutines::Maximum(
      result,
      this->theObjects[i].GetHighestIndexSuchThatHigherIndexVarsDontParticipate()
    );
  }
  return result;
}

template <class coefficient>
void Polynomial<coefficient>::ScaleToPositiveMonomialExponents(MonomialP& outputScale) {
  int numVars = this->minimalNumberOfVariables();
  outputScale.makeOne(numVars);
  for (int i = 0; i < numVars; i ++) {
    for (int j = 0; j < this->size(); j ++) {
      const MonomialP& currentMonomial = (*this)[j];
      Rational currentScale = MathRoutines::Minimum(outputScale(i), currentMonomial(i));
      outputScale.setVariable(i, currentScale);
    }
  }
  outputScale.Invert();
  this->MultiplyBy(outputScale);
}

template <class coefficient>
bool Polynomial<coefficient>::operator<=(const coefficient& other) const {
  coefficient constME;
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

template <class coefficient>
bool Polynomial<coefficient>::operator<(const coefficient& other) const {
  coefficient constME;
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

template <class coefficient>
bool Polynomial<coefficient>::operator>(const Polynomial<coefficient>& other) const {
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

template <class coefficient>
bool Polynomial<coefficient>::IsGEQcompareByTopMonomialTotalDegThenLexicographic(
  const Polynomial<coefficient>& left, const Polynomial<coefficient>& right
) {
  if (left.IsEqualToZero()) {
    return right.IsEqualToZero();
  }
  if (right.IsEqualToZero()) {
    return true;
  }
  return left[left.GetIndexMaxMonomialTotalDegThenLexicographic()].MonomialP::IsGEQTotalDegThenLexicographic(
    right[right.GetIndexMaxMonomialTotalDegThenLexicographic()]
  );
}

template <class coefficient>
bool Polynomial<coefficient>::IsGEQcompareByTopMonomialLexicographicLastVarStrongest(
  const Polynomial<coefficient>& left, const Polynomial<coefficient>& right
) {
  if (left.IsEqualToZero()) {
    return right.IsEqualToZero();
  }
  if (right.IsEqualToZero()) {
    return true;
  }
  int indexMaxLeft = left.GetIndexMaxMonomialLexicographicLastVariableStrongest();
  int indexMaxRight = right.GetIndexMaxMonomialLexicographicLastVariableStrongest();
  return left[indexMaxLeft].MonomialP::IsGEQLexicographicLastVariableStrongest(right[indexMaxRight]);
}

template <class coefficient>
bool Polynomial<coefficient>::IsEqualTo(const Polynomial<coefficient>& p) const {
  return *this == p;
}

template <class coefficient>
void Polynomial<coefficient>::operator-=(int x) {
  MonomialP tempMon;
  tempMon.makeOne();
  this->SubtractMonomial(tempMon, x);
}

template <class coefficient>
void Polynomial<coefficient>::operator-=(const coefficient& other) {
  MonomialP tempMon;
  tempMon.makeOne();
  this->SubtractMonomial(tempMon, other);
}

template <class coefficient>
void Polynomial<coefficient>::operator-=(const Polynomial<coefficient>& other) {
  this->::LinearCombination<MonomialP, coefficient>::operator-=(other);
}

template <class coefficient>
void Polynomial<coefficient>::operator*=(const MonomialP& other) {
  this->MultiplyBy(other, *this);
}

template <class coefficient>
void Polynomial<coefficient>::operator*=(const Polynomial<coefficient>& other) {
  this->::ElementMonomialAlgebra<MonomialP, coefficient>::operator*=(other);
}

template <class coefficient>
Polynomial<coefficient> Polynomial<coefficient>::operator%(const Polynomial<coefficient>& other) {
  Polynomial<coefficient> temp;
  Polynomial<coefficient> result;
  this->DivideBy(other, temp, result, &MonomialP::orderDefault());
  return result;
}

template <class coefficient>
void Polynomial<coefficient>::operator/=(const Polynomial<coefficient>& other) {
  Polynomial<coefficient> tempMe = *this;
  Polynomial<coefficient> tempRemainder;
  tempMe.DivideBy(other, *this, tempRemainder, &MonomialP::orderDefault());
}

template <class coefficient>
void Polynomial<coefficient>::operator/=(int other) {
  this->::LinearCombination<MonomialP, coefficient>::operator/=(other);
}

template <class coefficient>
void Polynomial<coefficient>::operator/=(const coefficient& other) {
  this->::LinearCombination<MonomialP, coefficient>::operator/=(other);
}

template <class coefficient>
template <class otherType>
void Polynomial<coefficient>::operator*=(const otherType& other) {
  this->::LinearCombination<MonomialP, coefficient>::operator*= (other);
}

template <class coefficient>
void Polynomial<coefficient>::operator=(const Polynomial<coefficient>& other) {
  this->::LinearCombination<MonomialP, coefficient>::operator=(other);
}

template <class coefficient>
template<class otherType>
void Polynomial<coefficient>::operator=(const Polynomial<otherType>& other) {
  this->::LinearCombination<MonomialP, coefficient>::operator=(other);
}

template <class coefficient>
void Polynomial<coefficient>::operator=(const coefficient& other) {
  this->MakeConst(other);
}

template <class coefficient>
void Polynomial<coefficient>::operator=(int other) {
  coefficient tempCF;
  tempCF = other;
  this->MakeConst(tempCF);
}

template <class coefficient>
template <class otherType>
void Polynomial<coefficient>::AssignOtherType(const Polynomial<otherType>& other) {
  this->NumVars = other.NumVars;
  this->::LinearCombination<MonomialP, coefficient>::AssignOtherType(other);
}

template <class coefficient>
bool Polynomial<coefficient>::operator<=(const Polynomial<coefficient>& other) const {
  return !(*this > other);
}

template <class coefficient>
bool Polynomial<coefficient>::IsProportionalTo(
  const Polynomial<coefficient>& other,
  coefficient& TimesMeEqualsOther,
  const coefficient& theRingUnit
) const {
  if (this->size() != other.size()) {
    return false;
  }
  if (other.size() == 0) {
    TimesMeEqualsOther = theRingUnit;
    return true;
  }
  const MonomialP& firstMon = (*this)[0];
  int indexInOther = other.theMonomials.GetIndex(firstMon);
  if (indexInOther == - 1) {
    return false;
  }
  TimesMeEqualsOther = other.coefficients[indexInOther];
  TimesMeEqualsOther /= this->coefficients[0];
  Polynomial<coefficient> tempP;
  tempP = *this;
  tempP *= TimesMeEqualsOther;
  tempP -= other;
  return tempP.IsEqualToZero();
}

template <class coefficient>
void Polynomial<coefficient>::DivideBy(
  const Polynomial<coefficient>& inputDivisor,
  Polynomial<coefficient>& outputQuotient,
  Polynomial<coefficient>& outputRemainder,
  typename List<MonomialP>::Comparator* monomialOrder
) const {
  MacroRegisterFunctionWithName("Polynomial::DivideBy");
  if (monomialOrder == nullptr) {
    global.fatal << "Non-initialized monomial pointer not allowed. " << global.fatal;
  }
  if (&outputQuotient == &outputRemainder) {
    global.fatal << "Input and output addresses not allowed to coincide. " << global.fatal;
  }
  if (inputDivisor.IsEqualToZero()) {
    global.fatal << "Division by zero polynomial. " << global.fatal;
  }
  if (
    &outputRemainder == this ||
    &outputQuotient == this ||
    &outputRemainder == &inputDivisor ||
    &outputQuotient == &inputDivisor
  ) {
    Polynomial<coefficient> newQuotient, newRemainder;
    this->DivideBy(inputDivisor, newQuotient, newRemainder, monomialOrder);
    outputQuotient = newQuotient;
    outputRemainder = newRemainder;
    return;
  }
  outputRemainder = *this;
  MonomialP scaleRemainder, scaleInput;
  Polynomial<coefficient> divisorShiftedExponents = inputDivisor;
  outputRemainder.ScaleToPositiveMonomialExponents(scaleRemainder);
  divisorShiftedExponents.ScaleToPositiveMonomialExponents(scaleInput);
  MonomialP remainderLeadingMonomial;
  coefficient remainderLeadingCoefficient;
  int remainderLeadingIndex = outputRemainder.GetIndexLeadingMonomial(
    &remainderLeadingMonomial, &remainderLeadingCoefficient, monomialOrder
  );
  MonomialP leadingMonomialShiftedDivisor;
  coefficient leadingCoefficientShiftedDivisor;
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
  Polynomial<coefficient> subtracand;
  subtracand.setExpectedSize(this->size());
  while (monomialOrder->greaterThanOrEqualTo(remainderLeadingMonomial, leadingMonomialShiftedDivisor)) {
    quotientMonomial = remainderLeadingMonomial;
    quotientMonomial /= leadingMonomialShiftedDivisor;
    if (!quotientMonomial.HasPositiveOrZeroExponents()) {
      break;
    }
    coefficient quotientCoefficient = remainderLeadingCoefficient;
    quotientCoefficient /= leadingCoefficientShiftedDivisor;
    outputQuotient.AddMonomial(quotientMonomial, quotientCoefficient);
    subtracand = divisorShiftedExponents;
    subtracand.MultiplyBy(quotientMonomial, quotientCoefficient);
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
  scaleInput.Invert();
  outputQuotient.MultiplyBy(scaleInput);
  outputQuotient.MultiplyBy(scaleRemainder);
  outputRemainder.MultiplyBy(scaleRemainder);
}

template <class coefficient>
void Polynomial<coefficient>::DivideByConstant(const coefficient& r) {
  for (int i = 0; i < this->size; i ++) {
    this->theObjects[i].Coefficient /= r;
  }
}

template <class coefficient>
void Polynomial<coefficient>::TimesInteger(int a) {
  Rational r;
  r.AssignInteger(a);
  this->TimesRational(r);
}

template <class coefficient>
void Polynomial<coefficient>::AssignCharPoly(const Matrix<coefficient>& input) {
  MacroRegisterFunctionWithName("Polynomial::AssignCharPoly");
  if (input.NumCols != input.NumRows) {
    global.fatal << "Programming error: requesting the "
    << "minimimal polynomial of a non-square matrix. "
    << global.fatal;
  }
  int n = input.NumCols;
  if (n == 0) {
    global.fatal
    << "At present, the characteristic polynomial of "
    << "a 0x0 matrix is not defined. "
    << "Crashing to let you know. If you think this "
    << "should be changed, document why and "
    << "modify the present assertion. " << global.fatal;
  }
  this->MakeConst(1);
  Matrix<coefficient> acc = input;
  coefficient currenCF;
  for (int i = 1; i < n; i ++) {
    currenCF = - acc.GetTrace() / i;
    this->AddMonomial(MonomialP(0, i), currenCF);
    for (int j = 0; j < n; j ++) {
      acc(j, j) += currenCF;
    }
    acc.MultiplyOnTheLeft(input);
  }
  this->AddMonomial(MonomialP(0, n), - acc.GetTrace() / n);
}

template <class coefficient>
void Polynomial<coefficient>::AssignMinPoly(const Matrix<coefficient>& input) {
  MacroRegisterFunctionWithName("Polynomial::AssignMinPoly");
  if (input.NumCols != input.NumRows) {
    global.fatal << "Programming error: requesting the "
    << "minimimal polynomial of a non-square matrix. "
    << global.fatal;
  }
  int theDim = input.NumCols;
  this->makeOne(1);
  Vectors<coefficient> theBasis;
  Vector<coefficient> theVectorPowers;
  Vector<coefficient> firstDependentPower;
  Polynomial<coefficient> currentFactor;
  MonomialP tempM;
  for (int col = 0; col < theDim; col ++) {
    theVectorPowers.MakeEi(theDim,col);
    theBasis.setSize(0);
    theBasis.addOnTop(theVectorPowers);
    for (int i = 0; i < theDim; i ++) {
      input.ActOnVectorColumn(*theBasis.LastObject(), theVectorPowers);
      if (theBasis.LinSpanContainsVector(theVectorPowers)) {
        break;
      }
      theBasis.addOnTop(theVectorPowers);
    }
    theVectorPowers.GetCoordsInBasiS(theBasis, firstDependentPower);
    currentFactor.setExpectedSize(theBasis.size + 1);
    currentFactor.makeZero();
    for (int i = 0; i < theBasis.size; i ++) {
      tempM.MakeEi(0, i, 1);
      currentFactor.AddMonomial(tempM, - firstDependentPower[i]);
    }
    tempM.MakeEi(0, theBasis.size, 1);
    currentFactor.AddMonomial(tempM, 1);
    *this = MathRoutines::lcm(*this, currentFactor);
  }
  this->scaleNormalizeLeadingMonomial();
}

template <class coefficient>
int Polynomial<coefficient>::GetMaxPowerOfVariableIndex(int VariableIndex) {
  int result = 0;
  for (int i = 0; i < this->size(); i ++) {
    result = MathRoutines::Maximum(result, (*this)[i](VariableIndex).NumShort);
    if (!(*this)[i](VariableIndex).IsSmallInteger()) {
      global.fatal << " This is a programming error: "
      << "GetMaxPowerOfVariableIndex called on a polynomial whose monomials "
      << "have degrees that are not small integers. "
      << "This needs to be fixed! " << global.fatal;
    }
  }
  return result;
}

template <class coefficient>
void Polynomial<coefficient>::GetConstantTerm(coefficient& output, const coefficient& theRingZero) const {
  MonomialP tempM;
  tempM.makeOne();
  int i = this->theMonomials.GetIndex(tempM);
  if (i == - 1) {
    output = theRingZero;
  } else {
    output = this->coefficients[i];
  }
}

template <class coefficient>
void Polynomial<coefficient>::makeMonomial(
  int letterIndex,
  const Rational& power,
  const coefficient& inputCoefficient,
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
  this->AddMonomial(tempM, inputCoefficient);
}

template <class coefficient>
void Polynomial<coefficient>::GetCoeffInFrontOfLinearTermVariableIndex(
  int index, coefficient& output
) {
  MonomialP tempM;
  tempM.MakeEi(index);
  int i = this->theMonomials.GetIndex(tempM);
  if (i == - 1) {
    output = 0;
  } else {
    output = this->coefficients[i];
  }
}

template<class coefficient>
bool Polynomial<coefficient>::isSquareFree(
  const coefficient& one, std::stringstream* comments
) const {
  MacroRegisterFunctionWithName("Polynomial::isSquareFree");
  Vector<Polynomial<coefficient> > differentials;
  if (this->IsConstant()) {
    return false;
  }
  if (!this->differential(differentials, comments)) {
    return false;
  }
  Polynomial<coefficient> divisor = *this;
  for (int i = 0; i < differentials.size; i ++) {
    if (differentials[i].IsEqualToZero()) {
      continue;
    }
    Polynomial::greatestCommonDivisor(divisor, differentials[i], divisor, one, comments);
  }
  return divisor.totalDegree() == 0;
}

template<class coefficient>
bool Polynomial<coefficient>::differential(
  Vector<Polynomial<coefficient> >& output, std::stringstream* comments
) const {
  MacroRegisterFunctionWithName("Polynomial::differential");
  int numberOfVariables = this->minimalNumberOfVariables();
  output.setSize(numberOfVariables);
  for (int i = 0; i < this->size(); i ++) {
    const MonomialP& currentMonomial = this->theMonomials[i];
    const coefficient& currentCoefficient = this->coefficients[i];
    int currentNumberOfVariables = currentMonomial.minimalNumberOfVariables();
    for (int j = 0; j < currentNumberOfVariables; j ++) {
      coefficient newCoefficient = currentCoefficient;
      Rational power = currentMonomial(j);
      LargeInteger powerInteger;
      if (!power.IsInteger(&powerInteger)) {
        if (comments != nullptr) {
          *comments << "Monomial has non-integer power " << power << ". ";
        }
        return false;
      }
      newCoefficient *= power;
      MonomialP newMonomial = currentMonomial;
      power -= 1;
      newMonomial.setVariable(j, power);
      output[j].AddMonomial(newMonomial, newCoefficient);
    }
  }
  return true;
}

template<class coefficient>
bool Polynomial<coefficient>::differential(
  Polynomial<coefficient>& output,
  std::stringstream* comments
) const {
  MacroRegisterFunctionWithName("Polynomial::differential");
  if (&output == this) {
    Polynomial<coefficient> thisCopy = *this;
    return thisCopy.differential(output, comments);
  }
  Vector<Polynomial<coefficient> > differentials;
  if (!this->differential(differentials, comments)) {
    return false;
  }
  output.makeZero();
  Polynomial<coefficient> nextSummand;
  for (int i = 0; i < differentials.size; i ++) {
    nextSummand = differentials[i];
    MonomialP differentialVariable(differentials.size + i, 1);
    nextSummand *= differentialVariable;
    output += nextSummand;
  }
  return true;
}

template <class coefficient>
bool PolynomialOrder<coefficient>::CompareLeftGreaterThanRight(
  const Polynomial<coefficient>& left, const Polynomial<coefficient>& right
) const {
  MacroRegisterFunctionWithName("PolynomialOrder::CompareLeftGreaterThanRight");
  List<MonomialP> sortedLeft = left.theMonomials;
  List<MonomialP> sortedRight = right.theMonomials;
  sortedLeft.QuickSortAscending(&this->theMonOrder);
  sortedRight.QuickSortAscending(&this->theMonOrder);
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
    coefficient leftCoefficient = left.GetMonomialCoefficient(leftMonomial);
    coefficient rightCoefficient = right.GetMonomialCoefficient(rightMonomial);
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

template<class coefficient>
std::string GroebnerBasisComputation<coefficient>::GetPolynomialStringSpacedMonomialsLaTeX(
  const Polynomial<coefficient>& thePoly,
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
    int theIndex = thePoly.theMonomials.GetIndex(this->allMonomials[i]);
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
        if (theHighLightedMons->Contains(this->allMonomials[i])) {
          useHighlightStyle = true;
        }
      }
    }
    out << "$";
    if (useHighlightStyle) {
      out << "\\color{" << *highlightColor << "}{";
    }
    out << Polynomial<coefficient>::GetBlendCoeffAndMon(
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

template <class coefficient>
std::string GroebnerBasisComputation<coefficient>::GetDivisionStringLaTeX() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::GetDivisionStringLaTeX");
  std::stringstream out;
  List<Polynomial<coefficient> >& theRemainders = this->intermediateRemainders.GetElement();
  List<Polynomial<coefficient> >& theSubtracands = this->intermediateSubtractands.GetElement();
  this->theFormat.monomialOrder = this->thePolynomialOrder.theMonOrder;
  std::string HighlightedColor = "red";
  this->allMonomials.addOnTopNoRepetition(this->startingPoly.GetElement().theMonomials);
  for (int i = 0; i < theRemainders.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theRemainders[i].theMonomials);
  }
  for (int i = 0; i < theSubtracands.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theSubtracands[i].theMonomials);
  }
  //List<std::string> basisColorStyles;
  //basisColorStyles.setSize(this->theBasiS.size);
  this->allMonomials.QuickSortDescending(&this->thePolynomialOrder.theMonOrder);
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
    (theRemainders[i], &HighlightedColor, &this->intermediateHighlightedMons.GetElement()[i])
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

template<class coefficient>
std::string GroebnerBasisComputation<coefficient>::GetPolynomialStringSpacedMonomialsHtml(
  const Polynomial<coefficient>& thePoly,
  const std::string& extraStyle,
  List<MonomialP>* theHighLightedMons
) {
  std::stringstream out;
  bool found = false;
  int countMons = 0;
  for (int i = 0; i < this->allMonomials.size; i ++) {
    int theIndex = thePoly.theMonomials.GetIndex(this->allMonomials[i]);
    if (theIndex == - 1) {
      out << "<td" << extraStyle << ">" << "</td>";
      continue;
    }
    countMons ++;
    bool useHighlightStyle = false;
    if (theHighLightedMons != nullptr) {
      if (theHighLightedMons->Contains(this->allMonomials[i])) {
        useHighlightStyle = true;
      }
    }
    out << "<td" << extraStyle << ">";
    if (useHighlightStyle) {
      out << "<span style ='color:red'>";
    }
    if (this->theFormat.flagUseLatex) {
      std::string monomialWithCoefficient = Polynomial<coefficient>::GetBlendCoeffAndMon(
        thePoly[theIndex],
        thePoly.coefficients[theIndex],
        found,
        &this->theFormat
      );

      out << HtmlRoutines::GetMathSpanPure(monomialWithCoefficient);
    } else {
      out << Polynomial<coefficient>::GetBlendCoeffAndMon(
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

template <class coefficient>
std::string GroebnerBasisComputation<coefficient>::GetDivisionStringHtml() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::GetDivisionStringHtml");
  std::stringstream out;
  List<Polynomial<coefficient> >& theRemainders = this->intermediateRemainders.GetElement();
  List<Polynomial<coefficient> >& theSubtracands = this->intermediateSubtractands.GetElement();
  this->theFormat.monomialOrder = this->thePolynomialOrder.theMonOrder;
  std::string underlineStyle = " style ='white-space: nowrap; border-bottom:1px solid black;'";
  this->allMonomials.Clear();
  this->allMonomials.addOnTopNoRepetition(this->startingPoly.GetElement().theMonomials);
  for (int i = 0; i < theRemainders.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theRemainders[i].theMonomials);
    if (i < theSubtracands.size) {
      this->allMonomials.addOnTopNoRepetition(theSubtracands[i].theMonomials);
    }
  }
  //List<std::string> basisColorStyles;
  //basisColorStyles.setSize(this->theBasiS.size);
  this->allMonomials.QuickSortDescending(&this->thePolynomialOrder.theMonOrder);
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
  if (theRemainders.size != this->intermediateHighlightedMons.GetElement().size) {
    global.fatal << "Should have as many remainders: " << theRemainders.size
    << " as intermediate highlighted mons: "
    << this->intermediateHighlightedMons.GetElement().size << global.fatal;
  }
  if (theRemainders.size != theSubtracands.size + 1) {
    global.fatal << "Remainders should equal subtracands plus one. " << global.fatal;
  }
  for (int i = 0; i < theRemainders.size; i ++) {
    out << "<tr><td></td>"
    << this->GetPolynomialStringSpacedMonomialsHtml(
      theRemainders[i], "", &this->intermediateHighlightedMons.GetElement()[i]
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

template<class coefficient>
void ElementOneVariablePolynomialQuotientRing<coefficient>::operator*=(
  ElementOneVariablePolynomialQuotientRing& other
) {
  if (other.modulus != this->modulus) {
    global.fatal << "Not allowed to multiply quotient-ring "
    << "elements of different rings. [This modulus, other modulus]: "
    << this->modulus << ", " << other.modulus << global.fatal;
  }
  this->value *= other.value;
  this->reduce();
}

template<class coefficient>
unsigned int ElementOneVariablePolynomialQuotientRing<coefficient>::hashFunction(
) {
  return
  this->value.hashFunction() * SomeRandomPrimes[0] +
  this->value.hashFunction() * SomeRandomPrimes[1];
}

template<class coefficient>
unsigned int ElementOneVariablePolynomialQuotientRing<coefficient>::hashFunction(
  const ElementOneVariablePolynomialQuotientRing<coefficient>& input
) {
  return input.hashFunction();
}

template<class coefficient>
std::string ElementOneVariablePolynomialQuotientRing<coefficient>::toString(
  FormatExpressions* theFormat
) {
  std::stringstream out;
  out << this->value.toString(theFormat) << " mod " << this->modulus.toString(theFormat);
  return out.str();
}

template<class coefficient>
void ElementOneVariablePolynomialQuotientRing<coefficient>::reduce() {
  Polynomial<coefficient> unusedQuotient;
  this->value.DivideBy(
    this->modulus,
    unusedQuotient,
    this->value,
    &MonomialP::orderDefault()
  );
}

#endif
