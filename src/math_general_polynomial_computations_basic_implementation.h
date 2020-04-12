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
bool Polynomial<coefficient>::IsOneVariablePoly(int* whichVariable) const {
  int tempInt;
  if (whichVariable == nullptr) {
    whichVariable = &tempInt;
  }
  *whichVariable = - 1;
  for (int i = 0; i < this->size(); i ++) {
    for (int j = 0; j < (*this)[i].GetMinimalNumberOfVariables(); j ++) {
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
    global.fatal << "Cannot compute determinant: matrix has "
    << theMat.NumRows << " rows and " << theMat.NumCols << " columns. " << global.fatal;
  }
  permutation thePerm;
  thePerm.initPermutation(theMat.NumRows);
  int numCycles = thePerm.GetNumPermutations();
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

template<class coefficient>
void Polynomial<coefficient>::ScaleToIntegralNoGCDCoeffs() {
  if (this->size() == 0) {
    return;
  }
  int indexHighestMonomial = 0;
  LargeIntegerUnsigned tempInt1, accumNum, accumDen;
  LargeInteger tempInt2;
  accumDen.MakeOne();
  accumNum = this->coefficients[0].GetNumerator().value;
  for (int i = 0; i < this->size(); i ++) {
    if (MonomialP::orderDefault().greaterThan(
      (*this)[i],
      (*this)[indexHighestMonomial]
    )) {
      indexHighestMonomial = i;
    }
    Rational& tempRat = this->coefficients[i];
    tempInt1 = tempRat.GetDenominator();
    tempInt2 = tempRat.GetNumerator();
    LargeIntegerUnsigned::lcm(tempInt1, accumDen, accumDen);
    LargeIntegerUnsigned::gcd(tempInt2.value, accumNum, accumNum);
  }
  Rational theMultiple;
  theMultiple.MakeOne();
  if (this->coefficients[indexHighestMonomial].IsNegative()) {
    theMultiple.MakeMOne();
  }
  theMultiple.MultiplyByLargeIntUnsigned(accumDen);
  Rational tempRat2;
  LargeInteger tempInt3;
  tempInt3.AssignLargeIntUnsigned(accumNum);
  tempRat2.AssignLargeInteger(tempInt3);
  theMultiple.DivideBy(tempRat2);
  *this *= theMultiple;
}

template <class coefficient>
int Polynomial<coefficient>::TotalDegreeInt() const {
  int result = - 1;
  if (!this->TotalDegree().IsSmallInteger(&result)) {
    global.fatal << "This is a programming error: requested total degree of a "
    << "polynomial in int formal, but the "
    << "degree of the polynomial is not a small integer. "
    << global.fatal;
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
    TempPoly *= this->coefficients[i];
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
  int NVar,
  int NonZeroIndex1,
  int NonZeroIndex2,
  const coefficient& coeff1,
  const coefficient& coeff2
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
    coefficient accum = this->coefficients[i];
    coefficient tempElt;
    for (int j = 0; j < currentMon.GetMinimalNumberOfVariables(); j ++) {
      int numCycles = 0;
      if (!(*this)[i](j).IsSmallInteger(&numCycles)) {
        global.fatal << "This is a programming error. Attempting to evaluate a polynomial whose "
        <<  i + 1 << "^{th} variable is raised to the power "
        << (*this)[i](j).toString()
        << ". Raising variables to power is allowed only if the power is a small integer. "
        << "If the user has requested such an operation, "
        << "it *must* be intercepted at an earlier level (and the user must be informed)." << global.fatal;
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
  if (newNumVars >= this->GetMinimalNumberOfVariables()) {
    return;
  }
  if (newNumVars < 0) {
    global.fatal << "This is a programming error. "
    << "Requesting negative number of variables (more precisely, "
    << newNumVars << ") is not allowed. " << global.fatal;
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
    Accum.AddMonomial(tempM, this->coefficients[i]);
  }
  this->operator=(Accum);
}

template <class coefficient>
void Polynomial<coefficient>::ShiftVariableIndicesToTheRight(int VarIndexShift) {
  if (VarIndexShift < 0) {
    global.fatal << "This is a programming error. Requesting negative variable shift (more precisely, "
    << VarIndexShift << ") not allowed. " << global.fatal;
  }
  if (VarIndexShift == 0) {
    return;
  }
  int oldNumVars = this->GetMinimalNumberOfVariables();
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
    Accum.AddMonomial(tempM, this->coefficients[i]);
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
      global.fatal << "This is a programming error. Attempting to evaluate a polynomial whose "
      <<  i + 1 << "^{th} variable is raised to the power "
      << currentMon(0).toString()
      << ". Raising variables to power is allowed only if the power is a small integer. "
      << "If the user has requested such an operation, "
      << "it *must* be intercepted at an earlier level (and the user must be informed)."
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
void Polynomial<coefficient>::ScaleToPositiveMonomialExponents(MonomialP& outputScale) {
  int numVars = this->GetMinimalNumberOfVariables();
  outputScale.MakeOne(numVars);
  for (int i = 0; i < numVars; i ++) {
    for (int j = 0; j < this->size(); j ++) {
      outputScale[i] = MathRoutines::Minimum(outputScale(i), (*this)[j](i));
    }
  }
  outputScale.Invert();
  this->MultiplyBy(outputScale);
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
  outputQuotient.MakeZero();
  if (remainderLeadingIndex == - 1) {
    return;
  }
  outputQuotient.SetExpectedSize(this->size());
  MonomialP quotientMonomial;
  Polynomial<coefficient> subtracand;
  subtracand.SetExpectedSize(this->size());
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
  tempM.MakeOne();
  int i = this->theMonomials.GetIndex(tempM);
  if (i == - 1) {
    output = theRingZero;
  } else {
    output = this->coefficients[i];
  }
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
  this->allMonomials.AddOnTopNoRepetition(this->startingPoly.GetElement().theMonomials);
  for (int i = 0; i < theRemainders.size; i ++) {
    this->allMonomials.AddOnTopNoRepetition(theRemainders[i].theMonomials);
  }
  for (int i = 0; i < theSubtracands.size; i ++) {
    this->allMonomials.AddOnTopNoRepetition(theSubtracands[i].theMonomials);
  }
  //List<std::string> basisColorStyles;
  //basisColorStyles.SetSize(this->theBasiS.size);
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
  this->allMonomials.AddOnTopNoRepetition(this->startingPoly.GetElement().theMonomials);
  for (int i = 0; i < theRemainders.size; i ++) {
    this->allMonomials.AddOnTopNoRepetition(theRemainders[i].theMonomials);
    if (i < theSubtracands.size) {
      this->allMonomials.AddOnTopNoRepetition(theSubtracands[i].theMonomials);
    }
  }
  //List<std::string> basisColorStyles;
  //basisColorStyles.SetSize(this->theBasiS.size);
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
#endif
