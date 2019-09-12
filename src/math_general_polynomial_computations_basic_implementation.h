//The current file is licensed under the license terms found in the main header file "calculator.h".
//For additional information refer to the file "calculator.h".
#ifndef vpfImplementationHeaderPolynomialComputationsBasic_already_included
#define vpfImplementationHeaderPolynomialComputationsBasic_already_included

#include "math_general.h"
static ProjectInformationInstance ProjectInfovpfImplementationHeaderPolynomialComputationsBasic(__FILE__, "Implementation header, basic polynomial computations. ");

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
        crash << "This is a programming error. Attempting to carry out a substitution in the monomial "
        << this->ToString()
        << " which does have non-zero exponent of variable x_{" << i + 1 << "}; however, the input substitution has "
        << TheSubstitution.size
        << " variable images. More precisely, the input substitution is:  "
        << TheSubstitution.ToString() << ". " << crash;
      }
      int theExponent = 0;
      if (!this->monBody[i].IsSmallInteger(&theExponent) || this->monBody[i] < 0) {
        if (TheSubstitution[i].IsMonomialCoeffOne()) {
          MonomialP tempMon = TheSubstitution[i][0];
          tempMon.RaiseToPower(this->monBody[i]);
          output *= tempMon;
          continue;
        }
        stOutput << "This may or may not be a programming error. "
        << "I cannot carry out a substitution in a monomial that has exponent "
        << "which is not a small integer: it is " << this->monBody[i]
        << " instead. " << Crasher::GetStackTraceEtcErrorMessageHTML();
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
bool Polynomial<coefficient>::IsOneVariablePoly(int* whichVariable) const {
  int tempInt;
  if (whichVariable == nullptr) {
    whichVariable = &tempInt;
  }
  *whichVariable = - 1;
  for (int i = 0; i < this->size(); i ++) {
    for (int j = 0; j < (*this)[i].GetMinNumVars(); j ++) {
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
void Polynomial<coefficient>::MakeDeterminantFromSquareMatrix(const Matrix<Polynomial<coefficient> >& theMat) {
  if (theMat.NumCols != theMat.NumRows) {
    crash << "Cannot compute determinant: matrix has "
    << theMat.NumRows << " rows and " << theMat.NumCols << " columns. " << crash;
  }
  permutation thePerm;
  thePerm.initPermutation(theMat.NumRows);
  int numCycles = thePerm.GetNumPermutations();
  //stOutput << "<hr>" << numCycles << " total cycles";
  List<int> permutationIndices;
  thePerm.GetPermutationLthElementIsTheImageofLthIndex(permutationIndices);
  Polynomial<coefficient> result, theMonomial;
  result.MakeZero();
  result.SetExpectedSize(numCycles);
  for (int i = 0; i < numCycles; i ++, thePerm.incrementAndGetPermutation(permutationIndices)) {
    theMonomial.MakeOne();
    for (int j = 0; j < permutationIndices.size; j ++) {
      theMonomial *= theMat(j, permutationIndices[j]);
    }
    //the following can be made much faster, but no need right now as it won't be a bottleneck.
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

template<class coefficient>
void Polynomial<coefficient>::ScaleToIntegralNoGCDCoeffs() {
  if (this->size() == 0) {
    return;
  }
  int indexHighestMon = 0;
  LargeIntUnsigned tempInt1, accumNum, accumDen;
  LargeInt tempInt2;
  accumDen.MakeOne();
  accumNum = this->theCoeffs[0].GetNumerator().value;
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].IsGEQLexicographicLastVariableStrongest((*this)[indexHighestMon])) {
      indexHighestMon = i;
    }
    Rational& tempRat = this->theCoeffs[i];
    tempInt1 = tempRat.GetDenominator();
    tempInt2 = tempRat.GetNumerator();
    LargeIntUnsigned::lcm(tempInt1, accumDen, accumDen);
    LargeIntUnsigned::gcd(tempInt2.value, accumNum, accumNum);
  }
  Rational theMultiple;
  theMultiple.MakeOne();
  if (this->theCoeffs[indexHighestMon].IsNegative()) {
    theMultiple.MakeMOne();
  }
  theMultiple.MultiplyByLargeIntUnsigned(accumDen);
  Rational tempRat2;
  LargeInt tempInt3;
  tempInt3.AssignLargeIntUnsigned(accumNum);
  tempRat2.AssignLargeInteger(tempInt3);
  theMultiple.DivideBy(tempRat2);
  *this *= theMultiple;
}

template <class coefficient>
int Polynomial<coefficient>::TotalDegreeInt() const {
  int result = - 1;
  if (!this->TotalDegree().IsSmallInteger(&result)) {
    crash << "This is a programming error: requested total degree of a "
    << "polynomial in int formal, but the degree of the polynomial is not a small integer. " << crash;
  }
  return result;
}

template <class coefficient>
bool Polynomial<coefficient>::Substitution(const List<Polynomial<coefficient> >& TheSubstitution) {
  MacroRegisterFunctionWithName("Polynomial::Substitution");
  Polynomial<coefficient> Accum, TempPoly;
//  int commentGrandMasterCheckWhenDone;
//  this->GrandMasterConsistencyCheck();
  for (int i = 0; i < this->size(); i ++) {
    if (!(*this)[i].SubstitutioN(TheSubstitution, TempPoly)) {
      return false;
    }
    TempPoly *= this->theCoeffs[i];
    Accum += TempPoly;
  }
  *this = Accum;
  return true;
}

template <class coefficient>
void Polynomial<coefficient>::MakeOne(int ExpectedNumVars) {
  this->MakeConst(1, ExpectedNumVars);
}

template <class coefficient>
void Polynomial<coefficient>::MakeDegreeOne(int NVar, int NonZeroIndex, const coefficient& coeff) {
  this->MakeZero();
  MonomialP tempM;
  tempM.MakeEi(NonZeroIndex, 1, NVar);
  this->AddMonomial(tempM, coeff);
}

template <class coefficient>
void Polynomial<coefficient>::MakeDegreeOne(
  int NVar, int NonZeroIndex1, int NonZeroIndex2, const coefficient& coeff1, const coefficient& coeff2
) {
  (void) NVar;
  this->MakeZero();
  MonomialP tempM;
  tempM.MakeEi(NonZeroIndex1);
  this->AddMonomial(tempM, coeff1);
  tempM.MakeEi(NonZeroIndex2);
  this->AddMonomial(tempM, coeff2);
}

template <class coefficient>
void Polynomial<coefficient>::MakeDegreeOne(
  int NVar, int NonZeroIndex, const coefficient& coeff1, const coefficient& ConstantTerm
) {
  this->MakeDegreeOne(NVar, NonZeroIndex, coeff1);
  *this += ConstantTerm;
}

template <class coefficient>
coefficient Polynomial<coefficient>::Evaluate(const Vector<coefficient>& input) {
  MacroRegisterFunctionWithName("Polynomial::Evaluate");
  coefficient output = 0;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialP& currentMon = (*this)[i];
    coefficient accum = this->theCoeffs[i];
    coefficient tempElt;
    for (int j = 0; j < currentMon.GetMinNumVars(); j ++) {
      int numCycles = 0;
      if (!(*this)[i](j).IsSmallInteger(&numCycles)) {
        crash << "This is a programming error. Attempting to evaluate a polynomial whose "
        <<  i + 1 << "^{th} variable is raised to the power "
        << (*this)[i](j).ToString()
        << ". Raising variables to power is allowed only if the power is a small integer. "
        << "If the user has requested such an operation, "
        << "it *must* be intercepted at an earlier level (and the user must be informed)." << crash;
      }
      bool isPositive = numCycles > 0;
      if (numCycles < 0) {
        numCycles = - numCycles;
      }
      tempElt = input[j];
      MathRoutines::RaiseToPower(tempElt, numCycles, static_cast<coefficient>(1));
      if (!isPositive) {
        tempElt.Invert();
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
  if (newNumVars >= this->GetMinNumVars()) {
    return;
  }
  if (newNumVars < 0) {
    crash << "This is a programming error. "
    << "Requesting negative number of variables (more precisely, "
    << newNumVars << ") is not allowed. " << crash;
  }
  Polynomial<coefficient> Accum;
  Accum.MakeZero();
  Accum.SetExpectedSize(this->size());
  MonomialP tempM;
  for (int i = 0; i < this->size(); i ++) {
    tempM.MakeOne(newNumVars);
    for (int j = 0; j < newNumVars; j ++) {
      tempM[j] = (*this)[i](j);
    }
    Accum.AddMonomial(tempM, this->theCoeffs[i]);
  }
  this->operator=(Accum);
}

template <class coefficient>
void Polynomial<coefficient>::ShiftVariableIndicesToTheRight(int VarIndexShift) {
  if (VarIndexShift < 0) {
    crash << "This is a programming error. Requesting negative variable shift (more precisely, "
    << VarIndexShift << ") not allowed. " << crash;
  }
  if (VarIndexShift == 0) {
    return;
  }
  int oldNumVars = this->GetMinNumVars();
  int newNumVars = oldNumVars + VarIndexShift;
  Polynomial<coefficient> Accum;
  Accum.MakeZero();
  Accum.SetExpectedSize(this->size());
  MonomialP tempM;
  for (int i = 0; i < this->size(); i ++) {
    tempM.MakeOne(newNumVars);
    for (int j = 0; j < oldNumVars; j ++) {
      tempM[j + VarIndexShift] = (*this)[i](j);
    }
    Accum.AddMonomial(tempM, this->theCoeffs[i]);
  }
  *this = Accum;
}

template <class coefficient>
Matrix<coefficient> Polynomial<coefficient>::EvaluateUnivariatePoly(const Matrix<coefficient>& input) {
  //for univariate polynomials only
  MacroRegisterFunctionWithName("Polynomial::EvaluateUnivariatePoly");
  Matrix<coefficient> output, tempElt, idMat;
  idMat.MakeIdMatrix(input.NumCols);
  output.MakeZeroMatrix(input.NumCols);
  for (int i = 0; i < this->size; i ++) {
    const MonomialP& currentMon = (*this)[i];
    int numCycles = 0;
    if (!currentMon(0).IsSmallInteger(&numCycles) ) {
      crash << "This is a programming error. Attempting to evaluate a polynomial whose "
      <<  i + 1 << "^{th} variable is raised to the power "
      << currentMon(0).ToString()
      << ". Raising variables to power is allowed only if the power is a small integer. "
      << "If the user has requested such an operation, "
      << "it *must* be intercepted at an earlier level (and the user must be informed)."
      << crash;
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
    tempElt *= this->theCoeffs[i];
    output += tempElt;
  }
  return output;
}

template <class coefficient>
void Polynomial<coefficient>::ScaleToPositiveMonomials(MonomialP& outputScale) {
  int numVars = this->GetMinNumVars();
  outputScale.MakeOne(numVars);
  for (int i = 0; i < numVars; i ++) {
    for (int j = 0; j < this->size(); j ++) {
      outputScale[i] = MathRoutines::Minimum(outputScale(i), (*this)[j](i));
    }
  }
  outputScale.Invert();
  this->MultiplyBy(outputScale, 1);
}

template <class coefficient>
bool Polynomial<coefficient>::IsProportionalTo(
  const Polynomial<coefficient>& other, coefficient& TimesMeEqualsOther, const coefficient& theRingUnit
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
  TimesMeEqualsOther = other.theCoeffs[indexInOther];
  TimesMeEqualsOther /= this->theCoeffs[0];
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
  Polynomial<coefficient>& outputRemainder
) const {
  MacroRegisterFunctionWithName("Polynomial::DivideBy");
  if (
    &outputRemainder == this ||
    &outputQuotient == this ||
    &outputRemainder == &inputDivisor ||
    &outputQuotient == &inputDivisor
  ) {
    Polynomial<coefficient> newQuot, newRemaind;
    this->DivideBy(inputDivisor, newQuot, newRemaind);
    outputQuotient = newQuot;
    outputRemainder = newRemaind;
    return;
  }
  outputRemainder = *this;
  MonomialP scaleRemainder;
  MonomialP scaleInput;
  Polynomial tempInput;
  tempInput = inputDivisor;
  outputRemainder.ScaleToPositiveMonomials(scaleRemainder);
  tempInput.ScaleToPositiveMonomials(scaleInput);
  int remainderMaxMonomial = outputRemainder.GetIndexMaxMonomialLexicographicLastVariableStrongest();
  int inputMaxMonomial = tempInput.GetIndexMaxMonomialLexicographicLastVariableStrongest();
  outputQuotient.MakeZero();
  if (remainderMaxMonomial == - 1) {
    return;
  }
  outputQuotient.SetExpectedSize(this->size());
  MonomialP tempMon;
  int numVars = MathRoutines::Maximum(this->GetMinNumVars(), inputDivisor.GetMinNumVars());
  tempMon.MakeOne(numVars);
  Polynomial<coefficient> tempP;
  tempP.SetExpectedSize(this->size());
  //if (this->flagAnErrorHasOccuredTimeToPanic)
  //{ this->ComputeDebugString();
   // tempInput.ComputeDebugString();
  //}
  if (remainderMaxMonomial >= outputRemainder.size()) {
    crash << "Remainder max monomial too large. " << crash;
  }
  if (inputMaxMonomial >= tempInput.size() || inputMaxMonomial < 0) {
    crash << "This is a programming error: the index of the maximal input monomial is "
    << inputMaxMonomial << " while the polynomial has "
    << tempInput.size() << "  monomials. I am attempting to divide "
    << this->ToString() << " by " << inputDivisor.ToString() << ". " << crash;
  }
  while (outputRemainder[remainderMaxMonomial].IsGEQLexicographicLastVariableStrongest(tempInput[inputMaxMonomial])) {
    if (remainderMaxMonomial >= outputRemainder.size()) {
      crash << "Remainder max monomial too large. " << crash;
    }
    tempMon = outputRemainder[remainderMaxMonomial];
    tempMon /= tempInput[inputMaxMonomial];
    if (!tempMon.HasPositiveOrZeroExponents()) {
      break;
    }
    coefficient tempCoeff = outputRemainder.theCoeffs[remainderMaxMonomial];
    tempCoeff /= tempInput.theCoeffs[inputMaxMonomial] ;
    outputQuotient.AddMonomial(tempMon, tempCoeff);
    tempP = tempInput;
    tempP.MultiplyBy(tempMon, tempCoeff);
    outputRemainder -= tempP;
    remainderMaxMonomial = outputRemainder.GetIndexMaxMonomialLexicographicLastVariableStrongest();
    if (remainderMaxMonomial == - 1) {
      break;
    }
  }
  scaleInput.Invert();
  outputQuotient.MultiplyBy(scaleInput, 1);
  outputQuotient.MultiplyBy(scaleRemainder, 1);
  outputRemainder.MultiplyBy(scaleRemainder, 1);
}

template <class coefficient>
void Polynomial<coefficient>::DivideByConstant(const coefficient& r) {
  for (int i = 0; i < this->size; i ++) {
    this->TheObjects[i].Coefficient /= r;
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
    crash << "Programming error: requesting the minimimal polynomial of a non-square matrix. " << crash;
  }
  int n = input.NumCols;
  if (n == 0) {
    crash << "At present, the characteristic polyomial of a 0x0 matrix is not defined. "
    << "Crashing to let you know. If you think this should be changed, document why and "
    << "modify the present assertion. " << crash;
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
    crash << "Programming error: requesting the minimimal polynomial of a non-square matrix. " << crash;
  }
  int theDim = input.NumCols;
  this->MakeOne(1);
  Vectors<coefficient> theBasis;
  Vector<coefficient> theVectorPowers;
  Vector<coefficient> firstDependentPower;
  Polynomial<coefficient> currentFactor;
  MonomialP tempM;
  for (int col = 0; col < theDim; col ++) {
    theVectorPowers.MakeEi(theDim,col);
    theBasis.SetSize(0);
    theBasis.AddOnTop(theVectorPowers);
    for (int i = 0; i < theDim; i ++) {
      input.ActOnVectorColumn(*theBasis.LastObject(), theVectorPowers);
      if (theBasis.LinSpanContainsVector(theVectorPowers)) {
        break;
      }
      theBasis.AddOnTop(theVectorPowers);
    }
    theVectorPowers.GetCoordsInBasiS(theBasis, firstDependentPower);
    currentFactor.SetExpectedSize(theBasis.size + 1);
    currentFactor.MakeZero();
    for (int i = 0; i < theBasis.size; i ++) {
      tempM.MakeEi(0, i, 1);
      currentFactor.AddMonomial(tempM, - firstDependentPower[i]);
    }
    tempM.MakeEi(0, theBasis.size, 1);
    currentFactor.AddMonomial(tempM, 1);
    *this = MathRoutines::lcm(*this, currentFactor);
  }
  this->ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
}

template <class coefficient>
int Polynomial<coefficient>::GetMaxPowerOfVariableIndex(int VariableIndex) {
  int result = 0;
  for (int i = 0; i < this->size(); i ++) {
    result = MathRoutines::Maximum(result, (*this)[i](VariableIndex).NumShort);
    if (!(*this)[i](VariableIndex).IsSmallInteger()) {
      crash << " This is a programming error: GetMaxPowerOfVariableIndex is called on a polynomial whose monomials"
      << " have degrees that are not small integers. This needs to be fixed! " << crash;
    }
  }
  return result;
}

template <class coefficient>
void Polynomial<coefficient>::GetConstantTerm(coefficient& output, const coefficient& theRingZero) const {
  MonomialP tempM;
  tempM.MakeOne();
  int i = this->theMonomials.GetIndex(tempM);
  if (i == - 1) {
    output = theRingZero;
  } else {
    output = this->theCoeffs[i];
  }
}

template <class coefficient>
void Polynomial<coefficient>::GetCoeffInFrontOfLinearTermVariableIndex(int index, coefficient& output) {
  MonomialP tempM;
  tempM.MakeEi(index);
  int i = this->theMonomials.GetIndex(tempM);
  if (i == - 1) {
    output = 0;
  } else {
    output = this->theCoeffs[i];
  }
}

template<class coefficient>
bool Polynomial<coefficient>::FindOneVarRatRoots(List<Rational>& output) {
  MacroRegisterFunctionWithName("Polynomial::FindOneVarRatRoots");
  if (this->GetMinNumVars() > 1) {
    return false;
  }
  output.SetSize(0);
  if (this->GetMinNumVars() == 0 || this->IsEqualToZero()) {
    return true;
  }
  Polynomial<coefficient> myCopy;
  myCopy = *this;
  myCopy.ScaleToIntegralMinHeightOverTheRationalsReturnsWhatIWasMultipliedBy();
  Rational lowestTerm, highestTerm;
  this->GetConstantTerm(lowestTerm);
  if (lowestTerm == 0) {
    Polynomial<Rational> x1, tempP;
    x1.MakeMonomiaL(0, 1, 1);
    myCopy.DivideBy(x1, myCopy, tempP);
    List<Rational> tempList;
    bool result = myCopy.FindOneVarRatRoots(tempList);
    output.AddOnTop(0);
    output.AddListOnTop(tempList);
    return result;
  }
  if (this->IsConstant()) {
    return true;
  }
  int indexHighest = this->GetIndexMaxMonomialLexicographicLastVariableStrongest();
  highestTerm = this->theCoeffs[indexHighest];
  if (!highestTerm.IsSmallInteger() || !lowestTerm.IsSmallInteger()) {
    return false;
  }
  Vector<Rational> tempV;
  Rational val;
  tempV.SetSize(1);
  List<int> divisorsH, divisorsS;
  LargeInt hT, lT;
  hT = highestTerm.GetNumerator();
  lT = lowestTerm.GetNumerator();
  if (!hT.GetDivisors(divisorsH, false) || !lT.GetDivisors(divisorsS, true)) {
    return false;
  }
  for (int i = 0; i < divisorsH.size; i ++) {
    for (int j = 0; j < divisorsS.size; j ++) {
      tempV[0].AssignNumeratorAndDenominator(divisorsS[j],divisorsH[i]);
      val = myCopy.Evaluate(tempV);
      if (val == 0) {
        Polynomial<Rational> divisor, tempP;
        divisor.MakeDegreeOne(1, 0, 1, - tempV[0]);
        myCopy.DivideBy(divisor, myCopy, tempP);
        output.AddOnTop(tempV[0]);
        List<Rational> tempList;
        bool result = myCopy.FindOneVarRatRoots(tempList);
        output.AddListOnTop(tempList);
        return result;
      }
    }
  }
  return true;
}

template <class coefficient>
bool PolynomialOrder<coefficient>::CompareLeftGreaterThanRight(
  const Polynomial<coefficient>& left, const Polynomial<coefficient>& right
) const {
  MacroRegisterFunctionWithName("PolynomialOrder::CompareLeftGreaterThanRight");
  Polynomial<coefficient> difference = left;
  difference -= right;
  if (difference.IsEqualToZero()) {
    return false;
  }
  if (difference.theCoeffs[difference.GetIndexMaxMonomial(this->theMonOrder)] > 0) {
    return true;
  }
  return false;
}
#endif
