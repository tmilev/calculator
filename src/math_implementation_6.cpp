// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "math_general_implementation.h"

// The Cantor-Zassenhaus algorithm:
// https://en.wikipedia.org/wiki/Cantor%E2%80%93Zassenhaus_algorithm
template <>
bool Polynomial<ElementZmodP>::factorMeCantorZassenhaus(
  List<Polynomial<ElementZmodP> >& outputFactors,
  std::stringstream* comments
) const {
  if (this->GetMinimalNumberOfVariables() > 1) {
    if (comments != nullptr) {
      *comments
      << "I haven't been taught how to factor polynomials "
      << "with more than 1 variable. ";
    }
    return false;
  }
  if (this->GetMinimalNumberOfVariables() == 0) {
    if (comments != nullptr) {
      *comments << "Factoring constant polynomials is not allowed. ";
    }
    return false;
  }
  if (this->IsEqualToZero()) {
    return false;
  }
  Polynomial<ElementZmodP> derivative;
  if (!this->computeDerivative(derivative, comments)) {
    return false;
  }
  Polynomial<ElementZmodP> greatestCommonDivisorWithDerivative;
  ElementZmodP one;
  one.MakeOne(this->coefficients[0].theModulus);
  if (!this->greatestCommonDivisor(
    *this,
    derivative,
    greatestCommonDivisorWithDerivative,
    one,
    comments
  )) {
    return false;
  }
  if (comments != nullptr) {
    *comments << "Not implemented yet.";
  }

  return false;
}

template<class coefficient>
bool Polynomial<coefficient>::computeDerivative(
  Polynomial<coefficient>& output,
  std::stringstream* comments
) const {
  if (this->GetMinimalNumberOfVariables() > 1) {
    if (comments != nullptr) {
      *comments
      << "Can only compute derivative of a one-variable polynomial. ";
    }
    return false;
  }
  if (&output == this) {
    Polynomial<coefficient> thisCopy = *this;
    return thisCopy.computeDerivative(output, comments);
  }
  output.MakeZero();
  int variableIndex = 0;
  for (int i = 0; i < this->size(); i ++) {
    coefficient newCoefficient;
    newCoefficient = this->coefficients[i];
    const MonomialP& currentMonomial = this->theMonomials[i];
    Rational power;
    currentMonomial.IsOneLetterNthDegree(&variableIndex, &power);
    LargeInteger powerInteger;
    if (!power.IsInteger(&powerInteger)) {
      if (comments != nullptr) {
        *comments << "Monomial has non-integer power. ";
      }
      return false;
    }
    newCoefficient *= power;
    MonomialP newMonomial;
    power -= 1;
    newMonomial[variableIndex] = power;
    output.AddMonomial(newMonomial, newCoefficient);
  }
  return true;
}

template<class coefficient>
bool Polynomial<coefficient>::leastCommonMultiple(
  const Polynomial<coefficient>& left,
  const Polynomial<coefficient>& right,
  Polynomial<coefficient>& output,
  coefficient one,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Polynomial::leastCommonMultiple");
  Polynomial<coefficient> leftTemp, rightTemp, tempP;
  List<Polynomial<coefficient> > theBasis;
  leftTemp = left;
  rightTemp = right;
  int theNumVars = MathRoutines::Maximum(left.GetMinimalNumberOfVariables(), right.GetMinimalNumberOfVariables());
  leftTemp.SetNumVariablesSubDeletedVarsByOne(theNumVars + 1);
  rightTemp.SetNumVariablesSubDeletedVarsByOne(theNumVars + 1);
  tempP.MakeMonomiaL(theNumVars, 1, one, theNumVars + 1);
  leftTemp *= tempP;
  tempP *= - 1;
  tempP += one;
  rightTemp *= tempP;
  theBasis.size = 0;
  theBasis.AddOnTop(leftTemp);
  theBasis.AddOnTop(rightTemp);
  GroebnerBasisComputation<coefficient> theComp;
  theComp.thePolynomialOrder.theMonOrder = MonomialP::orderForGCD();
  theComp.MaxNumGBComputations = - 1;
  if (!theComp.TransformToReducedGroebnerBasis(theBasis)) {
    global.fatal << "Transformation to reduced "
    << "Groebner basis is not allowed to fail in this function. " << global.fatal;
  }
  int maxMonNoTIndex = - 1;
  Rational maximalTotalDegree;
  MonomialP currentLeading;
  for (int i = 0; i < theBasis.size; i ++) {
    theBasis[i].GetIndexLeadingMonomial(
      &currentLeading, nullptr, &theComp.thePolynomialOrder.theMonOrder
    );
    if (currentLeading(theNumVars) == 0) {
      if (maxMonNoTIndex == - 1) {
        maximalTotalDegree = currentLeading.TotalDegree();
        maxMonNoTIndex = i;
      }
      if (maximalTotalDegree < currentLeading.TotalDegree()) {
        maximalTotalDegree = currentLeading.TotalDegree();
        maxMonNoTIndex = i;
      }
    }
  }
  if (maxMonNoTIndex == - 1) {
    global.fatal << "This is a programming error: failed to obtain "
    << "the least common multiple of two polynomials. The list of polynomials is: ";
    for (int i = 0; i < theBasis.size; i ++) {
      global.fatal << theBasis[i].toString() << ", ";
    }
    global.fatal << global.fatal;
  }
  output = theBasis[maxMonNoTIndex];
  output.SetNumVariablesSubDeletedVarsByOne(theNumVars);
  output.ScaleNormalizeLeadingMonomial();
  return true;
}

template<class coefficient>
bool Polynomial<coefficient>::greatestCommonDivisor(
  const Polynomial<coefficient>& left,
  const Polynomial<coefficient>& right,
  Polynomial<coefficient>& output,
  coefficient one,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Polynomial::greatestCommonDivisor");
  Polynomial<coefficient> leastCommonMultipleBuffer;
  Polynomial<coefficient> productBuffer;
  Polynomial<coefficient> remainderBuffer;
  if (!Polynomial<coefficient>::leastCommonMultiple(
    left, right, leastCommonMultipleBuffer, one, commentsOnFailure
  )) {
    return false;
  }
  productBuffer = left;
  productBuffer *= right;
  productBuffer.DivideBy(
    leastCommonMultipleBuffer,
    output,
    remainderBuffer,
    &MonomialP::orderForGCD()
  );
  if (!remainderBuffer.IsEqualToZero() || output.IsEqualToZero()) {
    global.fatal
    << "This is a programming error."
    << "<br>While computing the gcd of left = "
    << left.toString() << "<br>and right = "
    << right.toString() << "<br>I got that left * right = "
    << productBuffer.toString()
    << "<br>, and that lcm(left, right) = "
    << leastCommonMultipleBuffer.toString()
    << "<br>but at the same time "
    << "right * left divided by lcm (left, right) equals<br>"
    << output.toString()
    << "<br>with remainder " << remainderBuffer.toString()
    << ", which is imposible."
    << global.fatal;
  }
  output.ScaleNormalizeLeadingMonomial();
  return true;
}

/*
template<class coefficient>
bool Polynomial<coefficient>::isSquareFreeAndUnivariate(
  std::stringstream* comments
) const {
  if (this->GetMinimalNumberOfVariables() > 1) {
    if (comments != nullptr) {
      *comments
      << "I haven't been taught to compute square-freeness "
      << "with polynomials in more than one variable. ";
    }
    return false;
  }
  if (this->GetMinimalNumberOfVariables() == 0) {
    return true;
  }

  Polynomial<coefficient> derivative;
  if (!this->computeDerivative(derivative, comments)) {
    return false;
  }

  return true;
}
*/

template <>
bool Polynomial<Rational>::FactorMe(
  List<Polynomial<Rational> >& outputFactors,
  std::stringstream* comments
) const {
  MacroRegisterFunctionWithName("Polynomial::FactorMe");
  outputFactors.SetSize(0);
  if (this->IsEqualToZero() || this->IsConstant()) {
    outputFactors.AddOnTop(*this);
    return true;
  }
  List<Polynomial<Rational> > factorsToBeProcessed;
  factorsToBeProcessed.AddOnTop(*this);
  Polynomial<Rational> currentFactor, divisor;
  while (factorsToBeProcessed.size > 0) {
    currentFactor = factorsToBeProcessed.PopLastObject();
    if (!currentFactor.FactorMeOutputIsADivisor(divisor, comments)) {
      return false;
    }
    if (currentFactor.IsEqualToOne()) {
      outputFactors.AddOnTop(divisor);
      continue;
    }
    Rational tempRat = divisor.ScaleNormalizeLeadingMonomial();
    currentFactor /= tempRat;
    factorsToBeProcessed.AddOnTop(divisor);
    factorsToBeProcessed.AddOnTop(currentFactor);
  }
  outputFactors.QuickSortAscending();
  return true;
}

template<>
bool Polynomial<Rational>::FindOneVariableRationalRoots(List<Rational>& output) {
  MacroRegisterFunctionWithName("Polynomial::FindOneVariableRationalRoots");
  List<MonomialP>::Comparator* monomialOrder = &MonomialP::orderDefault();
  if (this->GetMinimalNumberOfVariables() > 1) {
    return false;
  }
  output.SetSize(0);
  if (this->GetMinimalNumberOfVariables() == 0 || this->IsEqualToZero()) {
    return true;
  }
  Polynomial<Rational> myCopy;
  myCopy = *this;
  myCopy.ScaleNormalizeLeadingMonomial();
  Rational lowestTerm, highestCoefficient;
  this->GetConstantTerm(lowestTerm);
  if (lowestTerm == 0) {
    Polynomial<Rational> x1, tempP;
    x1.MakeMonomiaL(0, 1, 1);
    myCopy.DivideBy(x1, myCopy, tempP, monomialOrder);
    List<Rational> tempList;
    bool result = myCopy.FindOneVariableRationalRoots(tempList);
    output.AddOnTop(0);
    output.AddListOnTop(tempList);
    return result;
  }
  if (this->IsConstant()) {
    return true;
  }
  highestCoefficient = this->GetLeadingCoefficient(monomialOrder);
  if (!highestCoefficient.IsSmallInteger() || !lowestTerm.IsSmallInteger()) {
    return false;
  }
  Vector<Rational> tempV;
  Rational val;
  tempV.SetSize(1);
  List<int> divisorsH, divisorsS;
  LargeInteger hT, lT;
  hT = highestCoefficient.GetNumerator();
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
        myCopy.DivideBy(divisor, myCopy, tempP, monomialOrder);
        output.AddOnTop(tempV[0]);
        List<Rational> tempList;
        bool result = myCopy.FindOneVariableRationalRoots(tempList);
        output.AddListOnTop(tempList);
        return result;
      }
    }
  }
  return true;
}

template <>
void Polynomial<Rational>::GetValuesLagrangeInterpolandsAtConsecutivePoints(
  Vector<Rational>& inputConsecutivePointsOfInterpolation,
  Vector<Rational>& inputPointsOfEvaluation,
  Vectors<Rational>& outputValuesInterpolands
) {
  outputValuesInterpolands.SetSize(inputConsecutivePointsOfInterpolation.size);
  for (int i = 0; i < inputConsecutivePointsOfInterpolation.size; i ++) {
    Vector<Rational>& currentInterpoland = outputValuesInterpolands[i];
    currentInterpoland.SetSize(inputPointsOfEvaluation.size);
    for (int j = 0; j < inputConsecutivePointsOfInterpolation.size; j ++) {
      currentInterpoland[j] = (i == j) ? 1 : 0;
    }
    for (int j = inputConsecutivePointsOfInterpolation.size; j < inputPointsOfEvaluation.size; j ++) {
      currentInterpoland[j] = 1;
      for (int k = 0; k < inputConsecutivePointsOfInterpolation.size; k ++) {
        if (i != k) {
          currentInterpoland[j] *= inputPointsOfEvaluation[j] - inputConsecutivePointsOfInterpolation[k];
          currentInterpoland[j] /= inputConsecutivePointsOfInterpolation[i] - inputConsecutivePointsOfInterpolation[k];
        }
      }
    }
  }
}

template <>
bool Polynomial<Rational>::FactorMeOutputIsADivisor(Polynomial<Rational>& output, std::stringstream* comments) {
  MacroRegisterFunctionWithName("Polynomial_CoefficientType::FactorMeOutputIsADivisor");
  if (this->GetMinimalNumberOfVariables() > 1) {
    return false;
  }
  if (this->GetMinimalNumberOfVariables() == 0) {
    return true;
  }
  Polynomial<Rational> thePoly = *this;
  thePoly.ScaleNormalizeLeadingMonomial();
  int degree = 0;
  if (!thePoly.TotalDegree().IsSmallInteger(&degree)) {
    return false;
  }
  int degreeLeft = degree / 2;
  Vector<Rational> AllPointsOfEvaluation;
  List<List<LargeInteger> > thePrimeFactorsAtPoints;
  List<List<int> > thePrimeFactorsMults;
  Vector<Rational> theValuesAtPoints, theValuesAtPointsLeft;
  AllPointsOfEvaluation.SetSize(degree + 1);
  thePrimeFactorsAtPoints.SetSize(degreeLeft + 1);
  thePrimeFactorsMults.SetSize(degreeLeft + 1);
  AllPointsOfEvaluation[0] = 0;
  for (int i = 1; i < AllPointsOfEvaluation.size; i ++) {
    AllPointsOfEvaluation[i] = (i % 2 == 1) ? i / 2 + 1 : - (i / 2);
  }
  Vector<Rational> theArgument;
  theArgument.SetSize(1);
  theValuesAtPoints.SetSize(AllPointsOfEvaluation.size);
  LargeInteger tempLI;
  for (int i = 0; i < AllPointsOfEvaluation.size; i ++) {
    theArgument[0] = AllPointsOfEvaluation[i];
    theValuesAtPoints[i] = thePoly.Evaluate(theArgument);
    if (theValuesAtPoints[i].IsEqualToZero()) {
      output.MakeDegreeOne(1, 0, 1, - theArgument[0]);
      *this /= output;
      return true;
    }
    if (i < degreeLeft + 1) {
      theValuesAtPoints[i].IsInteger(&tempLI);
      if (!tempLI.value.FactorReturnFalseIfFactorizationIncomplete(
        thePrimeFactorsAtPoints[i], thePrimeFactorsMults[i], 0, comments
      )) {
        if (comments != nullptr) {
          *comments << "<br>Aborting polynomial factorization: failed to factor the integer "
          << theValuesAtPoints[i].toString() << " (most probably the integer is too large).";
        }
        return false;
      }
    }
  }
  Incrementable<Incrementable<SelectionOneItem> > theDivisorSel;
  Incrementable<SelectionOneItem> signSel;
  Polynomial<Rational> quotient, remainder;
  Vectors<Rational> valuesLeftInterpolands;
  Vector<Rational> PointsOfInterpolationLeft;
  PointsOfInterpolationLeft.Reserve(degreeLeft + 1);
  Rational currentPrimePowerContribution, currentPointContribution;
  for (int i = 0; i <= degreeLeft; i ++) {
    PointsOfInterpolationLeft.AddOnTop(AllPointsOfEvaluation[i]);
  }
  theDivisorSel.initFromMults(thePrimeFactorsMults);
  signSel.initFromMults(1, degreeLeft + 1);
  valuesLeftInterpolands.SetSize(degreeLeft + 1);
  this->GetValuesLagrangeInterpolandsAtConsecutivePoints(
    PointsOfInterpolationLeft, AllPointsOfEvaluation, valuesLeftInterpolands
  );
  do {
    do {
      theValuesAtPointsLeft.MakeZero(theValuesAtPoints.size);
      Rational firstValue;
      bool isGood = false;//<-we shall first check if the divisor is constant.
      for (int j = 0; j < theDivisorSel.theElements.size; j ++) {
        currentPointContribution = 1;
        for (int k = 0; k < theDivisorSel[j].theElements.size; k ++) {
          currentPrimePowerContribution = thePrimeFactorsAtPoints[j][k];
          currentPrimePowerContribution.RaiseToPower(theDivisorSel[j][k].SelectedMult);
          currentPointContribution *= currentPrimePowerContribution;
        }
        if (signSel[j].SelectedMult == 1) {
          currentPointContribution *= - 1;
        }
        if (!isGood) {
          if (j == 0) {
            firstValue = currentPointContribution;
          } else {
            if (firstValue != currentPointContribution) {
              isGood = true; //<- the divisor has takes two different values, hence is non-constant.
            }
          }
        }
        theValuesAtPointsLeft += valuesLeftInterpolands[j]*currentPointContribution;
      }
      if (!isGood) {
        continue;
      }
      for (int j = 0; j < AllPointsOfEvaluation.size; j ++) {
        if (theValuesAtPointsLeft[j].IsEqualToZero()) {
          isGood = false;
          break;
        }
        currentPointContribution = (theValuesAtPoints[j]) / theValuesAtPointsLeft[j];
        if (!currentPointContribution.IsInteger()) {
          isGood = false;
          break;
        }
      }
      if (!isGood) {
        continue;
      }
      output.Interpolate(Vector<Rational>(PointsOfInterpolationLeft), Vector<Rational>(theValuesAtPointsLeft));
      this->DivideBy(output, quotient, remainder, &MonomialP::orderDefault());
      if (!remainder.IsEqualToZero()) {
        continue;
      }
      *this = quotient;
      return true;
    } while (theDivisorSel.IncrementReturnFalseIfPastLast());
  } while (signSel.IncrementReturnFalseIfPastLast());
  output = *this;
  this->MakeOne(1);
  return true;
}

template <>
bool Polynomial<Rational>::FactorMeNormalizedFactors(
  Rational& outputCoeff,
  List<Polynomial<Rational> >& outputFactors,
  std::stringstream* comments
) const {
  MacroRegisterFunctionWithName("Polynomial::FactorMeNormalizedFactors");
  List<Polynomial<Rational> > factorsToBeProcessed;
  outputFactors.SetSize(0);
  factorsToBeProcessed.AddOnTop(*this);
  factorsToBeProcessed.LastObject()->ScaleNormalizeLeadingMonomial();
  Polynomial<Rational> currentFactor, divisor;
  while (factorsToBeProcessed.size > 0) {
    currentFactor = factorsToBeProcessed.PopLastObject();
    if (!currentFactor.FactorMeOutputIsADivisor(divisor, comments)) {
      return false;
    }
    if (currentFactor.IsEqualToOne()) {
      divisor.ScaleNormalizeLeadingMonomial();
      outputFactors.AddOnTop(divisor);
      continue;
    }
    divisor.ScaleNormalizeLeadingMonomial();
    factorsToBeProcessed.AddOnTop(divisor);
    factorsToBeProcessed.AddOnTop(currentFactor);
  }
  outputFactors.QuickSortAscending();
  Polynomial<Rational> checkComputations;
  checkComputations.MakeOne();
  for (int i = 0; i < outputFactors.size; i ++) {
    checkComputations *= outputFactors[i];
  }
  if (!checkComputations.IsProportionalTo(*this, outputCoeff, 1)) {
    global.fatal << "Error in polynomial factorization function." << global.fatal;
  }
  return true;
}
