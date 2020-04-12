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
    Rational power = currentMonomial.TotalDegree();
    LargeInteger powerInteger;
    if (!power.IsInteger(&powerInteger)) {
      if (comments != nullptr) {
        *comments << "Monomial has non-integer power " << power << ". ";
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
bool Polynomial<coefficient>::isSquareFreeAndUnivariate(
  const ElementZmodP& one,
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
  Polynomial<coefficient> divisor;
  Polynomial::greatestCommonDivisor(*this, derivative, divisor, one, comments);
  return true;
}

template <>
bool Polynomial<Rational>::factorMe(
  List<Polynomial<Rational> >& outputFactors,
  std::stringstream* comments
) const {
  MacroRegisterFunctionWithName("Polynomial::factorMe");
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
    if (!currentFactor.factorMeOutputIsADivisor(divisor, comments)) {
      return false;
    }
    if (currentFactor.IsEqualToOne()) {
      outputFactors.AddOnTop(divisor);
      continue;
    }
    Rational tempRat = divisor.scaleNormalizeLeadingMonomial();
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
  myCopy.scaleNormalizeLeadingMonomial();
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
      tempV[0].AssignNumeratorAndDenominator(divisorsS[j], divisorsH[i]);
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
void Polynomial<Rational>::getValuesLagrangeInterpolands(
  Vector<Rational>& pointsOfInterpolation,
  Vector<Rational>& pointsOfEvaluation,
  Vectors<Rational>& outputValuesInterpolands
) {
  outputValuesInterpolands.SetSize(pointsOfInterpolation.size);
  for (int i = 0; i < pointsOfInterpolation.size; i ++) {
    Vector<Rational>& currentInterpoland = outputValuesInterpolands[i];
    currentInterpoland.SetSize(pointsOfEvaluation.size);
    for (int j = 0; j < pointsOfInterpolation.size; j ++) {
      currentInterpoland[j] = (i == j) ? 1 : 0;
    }
    for (int j = pointsOfInterpolation.size; j < pointsOfEvaluation.size; j ++) {
      currentInterpoland[j] = 1;
      for (int k = 0; k < pointsOfInterpolation.size; k ++) {
        if (i == k) {
          continue;
        }
        currentInterpoland[j] *= pointsOfEvaluation[j] - pointsOfInterpolation[k];
        currentInterpoland[j] /= pointsOfInterpolation[i] - pointsOfInterpolation[k];
      }
    }
  }
}

template <class coefficient>
void Polynomial<coefficient>::Interpolate(
  const Vector<coefficient>& thePoints, const Vector<coefficient>& ValuesAtThePoints
) {
  Polynomial<coefficient> theLagrangeInterpolator, accumulator;
  this->MakeZero();
  for (int i = 0; i < thePoints.size; i ++) {
    theLagrangeInterpolator.MakeConst(1, 1);
    for (int j = 0; j < thePoints.size; j ++) {
      if (i == j) {
        continue;
      }
      accumulator.MakeDegreeOne(1, 0, 1, - thePoints[j]);
      accumulator /= thePoints[i] - thePoints[j];
      theLagrangeInterpolator *= accumulator;
    }
    theLagrangeInterpolator *= ValuesAtThePoints[i];
    *this += theLagrangeInterpolator;
  }
}

template <>
bool Polynomial<Rational>::factorMeOutputIsADivisor(
  Polynomial<Rational>& output, std::stringstream* comments
) {
  MacroRegisterFunctionWithName("Polynomial::factorMeOutputIsADivisor");
  if (this->GetMinimalNumberOfVariables() > 1) {
    return false;
  }
  if (this->GetMinimalNumberOfVariables() == 0) {
    return true;
  }
  Polynomial<Rational> thePoly = *this;
  thePoly.scaleNormalizeLeadingMonomial();
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
    if (i > degreeLeft) {
      continue;
    }
    theValuesAtPoints[i].IsInteger(&tempLI);
    if (!tempLI.value.FactorReturnFalseIfFactorizationIncomplete(
      thePrimeFactorsAtPoints[i], thePrimeFactorsMults[i], 0, comments
    )) {
      if (comments != nullptr) {
        *comments
        << "<br>Aborting polynomial factorization: "
        << "failed to factor the integer "
        << theValuesAtPoints[i].toString()
        << " (most probably the integer is too large).";
      }
      return false;
    }
    thePrimeFactorsAtPoints[i].AddOnTop(- 1);
    thePrimeFactorsMults[i].AddOnTop(1);
  }
  Incrementable<Incrementable<SelectionOneItem> > divisorSelection;
  Polynomial<Rational> quotient, remainder;
  Vectors<Rational> valuesLeftInterpolands;
  Vector<Rational> PointsOfInterpolationLeft;
  PointsOfInterpolationLeft.Reserve(degreeLeft + 1);
  Rational currentPrimePowerContribution, currentPointContribution;
  for (int i = 0; i <= degreeLeft; i ++) {
    PointsOfInterpolationLeft.AddOnTop(AllPointsOfEvaluation[i]);
  }
  divisorSelection.initFromMults(thePrimeFactorsMults);
  valuesLeftInterpolands.SetSize(degreeLeft + 1);
  this->getValuesLagrangeInterpolands(
    PointsOfInterpolationLeft, AllPointsOfEvaluation, valuesLeftInterpolands
  );
  ProgressReport theReport(1000, GlobalVariables::Response::ReportType::general);
  LargeInteger total = divisorSelection.totalCombinations();
  do {
    if (theReport.TickAndWantReport()) {
      std::stringstream report;
      report << "Trying divisor combination "
      << theReport.ticks << " out of " << total.toString();
      theReport.Report(report.str());
    }
    theValuesAtPointsLeft.MakeZero(theValuesAtPoints.size);
    Rational firstValue;
    bool isGood = false; //<-we shall first check if the divisor is constant.
    for (int j = 0; j < divisorSelection.theElements.size; j ++) {
      currentPointContribution = 1;
      for (int k = 0; k < divisorSelection[j].theElements.size; k ++) {
        currentPrimePowerContribution = thePrimeFactorsAtPoints[j][k];
        currentPrimePowerContribution.RaiseToPower(divisorSelection[j][k].SelectedMult);
        currentPointContribution *= currentPrimePowerContribution;
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
      theValuesAtPointsLeft += valuesLeftInterpolands[j] * currentPointContribution;
    }
    if (!isGood) {
      continue;
    }
    for (int j = 0; j < AllPointsOfEvaluation.size; j ++) {
      if (theValuesAtPointsLeft[j].IsEqualToZero()) {
        isGood = false;
        break;
      }
      currentPointContribution = theValuesAtPoints[j] / theValuesAtPointsLeft[j];
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
  } while (divisorSelection.IncrementReturnFalseIfPastLast());
  output = *this;
  this->MakeOne(1);
  return true;
}

template <>
bool Polynomial<Rational>::factorMeNormalizedFactors(
  Rational& outputCoefficient,
  List<Polynomial<Rational> >& outputFactors,
  std::stringstream* comments
) const {
  MacroRegisterFunctionWithName("Polynomial::factorMeNormalizedFactors");
  List<Polynomial<Rational> > factorsToBeProcessed;
  outputFactors.SetSize(0);
  factorsToBeProcessed.AddOnTop(*this);
  outputCoefficient = factorsToBeProcessed.LastObject()->scaleNormalizeLeadingMonomial();
  Polynomial<Rational> currentFactor, divisor;
  while (factorsToBeProcessed.size > 0) {
    currentFactor = factorsToBeProcessed.PopLastObject();
    if (!currentFactor.factorMeOutputIsADivisor(divisor, comments)) {
      return false;
    }
    if (currentFactor.IsEqualToOne()) {
      outputFactors.AddOnTop(divisor);
      continue;
    }
    factorsToBeProcessed.AddOnTop(divisor);
    factorsToBeProcessed.AddOnTop(currentFactor);
  }
  outputFactors.QuickSortAscending();
  Polynomial<Rational> checkComputations;
  checkComputations.MakeOne();
  for (int i = 0; i < outputFactors.size; i ++) {
    checkComputations *= outputFactors[i];
  }
  checkComputations *= outputCoefficient;
  if (!checkComputations.IsEqualTo(*this)) {
    global.fatal << "Error in polynomial factorization function." << global.fatal;
  }
  return true;
}
