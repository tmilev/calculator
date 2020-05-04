// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "math_general_implementation.h"

// The Cantor-Zassenhaus algorithm:
// https://en.wikipedia.org/wiki/Cantor%E2%80%93Zassenhaus_algorithm
bool PolynomialFactorizationCantorZassenhaus::oneFactor(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("PolynomialFactorizationCantorZassenhaus::oneFactor");
  this->output->format.flagSuppressModP = true;
  this->current = this->output->current;
  if (this->current.minimalNumberOfVariables() > 1) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "I haven't been taught how to factor polynomials "
      << "with more than 1 variable. ";
    }
    return false;
  }
  if (this->current.minimalNumberOfVariables() == 0) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Factoring constant polynomials is not allowed. ";
    }
    return false;
  }
  if (this->current.isEqualToZero()) {
    return false;
  }
  if (!this->current.hasSmallIntegralPositivePowers(nullptr)) {
    return false;
  }
  if (this->current.totalDegree() > this->maximumTotalDegree) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "The degree of the input is larger than the "
      << "maximum allowed for the Cantor-Zassenhaus algorithm: "
      << this->maximumTotalDegree << ". ";
    }
    return false;
  }
  this->one.makeOne(this->current.coefficients[0].theModulus);
  if (this->one.theModulus == 2) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Only odd primes allowed. ";
    }
    return false;
  }
  Vector<Polynomial<ElementZmodP> > derivative;
  if (!this->current.differential(derivative, commentsOnFailure)) {
    return false;
  }
  Polynomial<ElementZmodP> greatestCommonDivisor;
  derivative[0].greatestCommonDivisor(
    derivative[0],
    this->current,
    greatestCommonDivisor,
    this->one,
    commentsOnFailure
  );
  if (!greatestCommonDivisor.isConstant()) {
    this->output->accountNonReducedFactor(greatestCommonDivisor);
    return true;
  }
  return this->oneFactorGo(commentsOnFailure);
}

const int PolynomialFactorizationCantorZassenhaus::maximumTotalDegree = 64;

bool PolynomialFactorizationCantorZassenhaus::oneFactorGo(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("PolynomialFactorizationCantorZassenhaus::oneFactorGo");
  PolynomialModuloPolynomial<ElementZmodP> x;
  x.modulus = this->output->current;
  x.value.makeDegreeOne(1, 0, this->one, this->one.zero());
  PolynomialModuloPolynomial<ElementZmodP> oneQuotientRing;
  oneQuotientRing.modulus = this->current;
  oneQuotientRing.value.makeConstant(this->one);
  LargeInteger modulus = this->one.theModulus;
  int degree = this->current.totalDegreeInt();
  PolynomialModuloPolynomial<ElementZmodP> xPower;
  std::stringstream commentsGeneral;
  HashedList<PolynomialModuloPolynomial<ElementZmodP> > powers;
  powers.addOnTop(xPower);
  for (int i = 0; i < degree; i ++) {
    xPower = x;
    LargeInteger power = modulus;

    power.raiseToPower(i + 1);
    power -= 1;
    power /= 2;
    MathRoutines::raiseToPower(xPower, power, oneQuotientRing);
    powers.addOnTop(xPower);
    commentsGeneral << "\\(x^{\\frac{" << modulus << "^{" << i + 1 << "} - 1}{2}} \\)=\\( "
    << xPower.toString(&this->output->format) << "\\)";
    Polynomial<ElementZmodP> gcd, gcdWithPlusOne, gcdWithMinusOne;
    xPower.value.greatestCommonDivisor(xPower.value, xPower.modulus, gcd, this->one, commentsOnFailure);
    Polynomial<ElementZmodP> valuePlusOne = xPower.value + oneQuotientRing.value;
    Polynomial<ElementZmodP> valueMinusOne = xPower.value - oneQuotientRing.value;
    if (!valuePlusOne.isEqualToZero()) {
      xPower.value.greatestCommonDivisor(valuePlusOne, xPower.modulus, gcdWithPlusOne, this->one, commentsOnFailure);
    }
    if (!valueMinusOne.isEqualToZero()) {
      xPower.value.greatestCommonDivisor(valueMinusOne, xPower.modulus, gcdWithMinusOne, this->one, commentsOnFailure);
    }
    commentsGeneral
    << ", gcd: " << gcd.toString(&this->output->format)
    << ", gcdWithPlusOne: " << gcdWithPlusOne.toString(&this->output->format)
    << ", gcdWithMinusOne: " << gcdWithMinusOne.toString(&this->output->format)
    << "<br>";
  }
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << commentsGeneral.str();
  }

  return false;
}

template<class Coefficient>
bool Polynomial<Coefficient>::isSquareFreeAndUnivariate(
  const Coefficient& one,
  std::stringstream* comments
) const {
  int numberOfVariables = this->minimalNumberOfVariables();
  if (numberOfVariables > 1) {
    if (comments != nullptr) {
      *comments << "Not univariate: polynomial has "
      << numberOfVariables << " variables. ";
    }
    return false;
  }
  return this->isSquareFree(one, comments);
}

template<>
bool Polynomial<Rational>::findOneVariableRationalRoots(List<Rational>& output) {
  MacroRegisterFunctionWithName("Polynomial::findOneVariableRationalRoots");
  List<MonomialP>::Comparator* monomialOrder = &MonomialP::orderDefault();
  if (this->minimalNumberOfVariables() > 1) {
    return false;
  }
  output.setSize(0);
  if (this->minimalNumberOfVariables() == 0 || this->isEqualToZero()) {
    return true;
  }
  Polynomial<Rational> myCopy;
  myCopy = *this;
  myCopy.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  Rational lowestTerm, highestCoefficient;
  this->GetConstantTerm(lowestTerm);
  if (lowestTerm == 0) {
    Polynomial<Rational> x1, tempP;
    x1.makeMonomial(0, 1, 1);
    myCopy.divideBy(x1, myCopy, tempP, monomialOrder);
    List<Rational> tempList;
    bool result = myCopy.findOneVariableRationalRoots(tempList);
    output.addOnTop(0);
    output.addListOnTop(tempList);
    return result;
  }
  if (this->isConstant()) {
    return true;
  }
  highestCoefficient = this->GetLeadingCoefficient(monomialOrder);
  if (!highestCoefficient.isSmallInteger() || !lowestTerm.isSmallInteger()) {
    return false;
  }
  Vector<Rational> tempV;
  Rational val;
  tempV.setSize(1);
  List<int> divisorsH, divisorsS;
  LargeInteger hT, lT;
  hT = highestCoefficient.getNumerator();
  lT = lowestTerm.getNumerator();
  if (!hT.GetDivisors(divisorsH, false) || !lT.GetDivisors(divisorsS, true)) {
    return false;
  }
  for (int i = 0; i < divisorsH.size; i ++) {
    for (int j = 0; j < divisorsS.size; j ++) {
      tempV[0].AssignNumeratorAndDenominator(divisorsS[j], divisorsH[i]);
      val = myCopy.evaluate(tempV);
      if (val == 0) {
        Polynomial<Rational> divisor, tempP;
        divisor.makeDegreeOne(1, 0, 1, - tempV[0]);
        myCopy.divideBy(divisor, myCopy, tempP, monomialOrder);
        output.addOnTop(tempV[0]);
        List<Rational> tempList;
        bool result = myCopy.findOneVariableRationalRoots(tempList);
        output.addListOnTop(tempList);
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
  outputValuesInterpolands.setSize(pointsOfInterpolation.size);
  for (int i = 0; i < pointsOfInterpolation.size; i ++) {
    Vector<Rational>& currentInterpoland = outputValuesInterpolands[i];
    currentInterpoland.setSize(pointsOfEvaluation.size);
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

bool PolynomialFactorizationKronecker::oneFactor(
  std::stringstream* comments
) {
  MacroRegisterFunctionWithName("Polynomial::oneFactor");
  this->current = this->output->current;
  if (this->current.minimalNumberOfVariables() > 1) {
    return false;
  }
  if (this->current.minimalNumberOfVariables() == 0) {
    return true;
  }
  int degree = 0;
  if (!this->current.totalDegree().isSmallInteger(&degree)) {
    return false;
  }
  int degreeLeft = degree / 2;
  Vector<Rational> AllPointsOfEvaluation;
  List<List<LargeInteger> > thePrimeFactorsAtPoints;
  List<List<int> > thePrimeFactorsMults;
  Vector<Rational> theValuesAtPoints, theValuesAtPointsLeft;
  AllPointsOfEvaluation.setSize(degree + 1);
  thePrimeFactorsAtPoints.setSize(degreeLeft + 1);
  thePrimeFactorsMults.setSize(degreeLeft + 1);
  AllPointsOfEvaluation[0] = 0;
  for (int i = 1; i < AllPointsOfEvaluation.size; i ++) {
    AllPointsOfEvaluation[i] = (i % 2 == 1) ? i / 2 + 1 : - (i / 2);
  }
  Vector<Rational> theArgument;
  theArgument.setSize(1);
  theValuesAtPoints.setSize(AllPointsOfEvaluation.size);
  LargeInteger tempLI;
  for (int i = 0; i < AllPointsOfEvaluation.size; i ++) {
    theArgument[0] = AllPointsOfEvaluation[i];
    theValuesAtPoints[i] = this->current.evaluate(theArgument);
    if (theValuesAtPoints[i].isEqualToZero()) {
      Polynomial<Rational> incomingFactor;
      incomingFactor.makeDegreeOne(1, 0, 1, - theArgument[0]);
      this->output->accountReducedFactor(incomingFactor);
      return true;
    }
    if (i > degreeLeft) {
      continue;
    }
    theValuesAtPoints[i].isInteger(&tempLI);
    if (!tempLI.value.factor(
      thePrimeFactorsAtPoints[i], thePrimeFactorsMults[i], 0, 3, comments
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
    thePrimeFactorsAtPoints[i].addOnTop(- 1);
    thePrimeFactorsMults[i].addOnTop(1);
  }
  Incrementable<Incrementable<SelectionOneItem> > divisorSelection;
  Vectors<Rational> valuesLeftInterpolands;
  Vector<Rational> PointsOfInterpolationLeft;
  PointsOfInterpolationLeft.reserve(degreeLeft + 1);
  Rational currentPrimePowerContribution, currentPointContribution;
  for (int i = 0; i <= degreeLeft; i ++) {
    PointsOfInterpolationLeft.addOnTop(AllPointsOfEvaluation[i]);
  }
  divisorSelection.initFromMults(thePrimeFactorsMults);
  valuesLeftInterpolands.setSize(degreeLeft + 1);
  Polynomial<Rational>::getValuesLagrangeInterpolands(
    PointsOfInterpolationLeft, AllPointsOfEvaluation, valuesLeftInterpolands
  );
  ProgressReport theReport(1000, GlobalVariables::Response::ReportType::general);
  LargeInteger total = divisorSelection.totalCombinations();
  do {
    if (theReport.tickAndWantReport()) {
      std::stringstream report;
      report << "Trying divisor combination "
      << theReport.ticks << " out of " << total.toString();
      theReport.report(report.str());
    }
    theValuesAtPointsLeft.makeZero(theValuesAtPoints.size);
    Rational firstValue;
    bool isGood = false; //<-we shall first check if the divisor is constant.
    for (int j = 0; j < divisorSelection.theElements.size; j ++) {
      currentPointContribution = 1;
      for (int k = 0; k < divisorSelection[j].theElements.size; k ++) {
        currentPrimePowerContribution = thePrimeFactorsAtPoints[j][k];
        currentPrimePowerContribution.raiseToPower(divisorSelection[j][k].SelectedMult);
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
      if (theValuesAtPointsLeft[j].isEqualToZero()) {
        isGood = false;
        break;
      }
      currentPointContribution = theValuesAtPoints[j] / theValuesAtPointsLeft[j];
      if (!currentPointContribution.isInteger()) {
        isGood = false;
        break;
      }
    }
    if (!isGood) {
      continue;
    }
    Polynomial<Rational> incoming;
    incoming.interpolate(
      PointsOfInterpolationLeft,
      theValuesAtPointsLeft
    );
    if (this->output->accountNonReducedFactor(incoming)) {
      return true;
    }
  } while (divisorSelection.incrementReturnFalseIfPastLast());
  return this->output->accountReducedFactor(this->current);
}
