// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "math_general_implementation.h"

// The Cantor-Zassenhaus algorithm:
// https://en.wikipedia.org/wiki/Cantor%E2%80%93Zassenhaus_algorithm
PolynomialFactorizationCantorZassenhaus::PolynomialFactorizationCantorZassenhaus() {
  this->output = nullptr;
}

bool PolynomialFactorizationCantorZassenhaus::oneFactor(
  std::stringstream* comments,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("PolynomialFactorizationCantorZassenhaus::oneFactor");
  this->output->format.flagSuppressModP = true;
  this->current = this->output->current;

  this->one.makeOne(this->current.coefficients[0].modulus);
  if (this->one.modulus == 2) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Only odd primes allowed. ";
    }
    return false;
  }
  ProgressReport report;
  std::stringstream out;
  out << "Find one factor of " << this->current.toString(&this->output->format);
  out << "Factorization so far: " << this->output->toStringResult(&this->output->format);
  report.report(out.str());
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
  return this->oneFactorGo(comments, commentsOnFailure);
}

bool PolynomialFactorizationCantorZassenhaus::hasFactorsOfDifferentDegree(std::stringstream* comments) {
  MacroRegisterFunctionWithName("PolynomialFactorizationCantorZassenhaus::hasFactorsOfDifferentDegree");
  (void) comments;
  int degree = this->current.totalDegreeInt();
  PolynomialModuloPolynomial<ElementZmodP> currentPower;
  PolynomialModuloPolynomial<ElementZmodP> currentPowerMinusBaseLetter;
  currentPower = this->baseLetter;
  Polynomial<ElementZmodP> candidateDivisor;
  for (int i = 0; i < degree; i ++) {
    MathRoutines::raiseToPower(currentPower, this->one.modulus, this->oneQuotientRing);
    currentPowerMinusBaseLetter = currentPower;
    currentPowerMinusBaseLetter -= this->baseLetter;
    if (currentPowerMinusBaseLetter.isEqualToZero()) {
      continue;
    }
    bool expectedToBeTrue =
    Polynomial<ElementZmodP>::greatestCommonDivisor(
      this->current, currentPowerMinusBaseLetter.value, candidateDivisor, this->one, nullptr
    );
    if (!expectedToBeTrue) {
      global.fatal << "Unexpected failure to compute greatest common divisor of "
      << this->current << " and " << currentPowerMinusBaseLetter.value
      << ". " << global.fatal;
    }
    if (comments != nullptr) {
      std::string xLetter = this->output->format.getPolynomialLetter(0);
      *comments << "<br>GCD: "
      << candidateDivisor.toString(&this->output->format)
      << " of: \\(" << xLetter
      << "^{" << this->one.modulus << "^{" << degree << "}} - "
      << xLetter << "\\) "
      << "and \\(" << this->current.toString(&this->output->format) << "\\)";
    }
    if (
      candidateDivisor.totalDegree() > 0 &&
      candidateDivisor.totalDegree() < this->current.totalDegree()
    ) {
      if (comments != nullptr) {
        std::string xLetter = this->output->format.getPolynomialLetter(0);
        *comments << "<br>Found factor: "
        << candidateDivisor.toString(&this->output->format)
        << " by gcd between \\(" << xLetter
        << "^{" << this->one.modulus << "^{" << degree << "}} - "
        << xLetter << "\\) "
        << "and \\(" << this->current.toString(&this->output->format) << "\\)";
      }
      this->output->accountNonReducedFactor(candidateDivisor);
      return true;
    }
  }
  return false;
}

bool PolynomialFactorizationCantorZassenhaus::divisorFromCandidate(
  const Polynomial<ElementZmodP>& candidate,
  const std::string& candidateDisplayName,
  std::stringstream* comments
) {
  if (candidate.isEqualToZero()) {
    return false;
  }
  Polynomial<ElementZmodP> divisor;
  bool mustBeTrue = candidate.greatestCommonDivisor(
    candidate, this->current, divisor, this->one, comments
  );
  if (!mustBeTrue) {
    global.fatal << "Failure to find gcd not allowed here. " << global.fatal;
  }
  if (divisor.totalDegree() <= 0 || divisor.totalDegree() >= this->current.totalDegree()) {
    return false;
  }
  if (comments != nullptr) {
    *comments << "Found divisor \\("
    << this->one.toStringPolynomial(divisor, &this->output->format)
    << "\\) by taking gcd with \\(" << candidateDisplayName << "\\). ";
  }
  this->output->accountNonReducedFactor(divisor);
  return true;
}

bool PolynomialFactorizationCantorZassenhaus::handlePrimeDegreeSeparatedFactor(
  Polynomial<ElementZmodP>& input
) {
  MacroRegisterFunctionWithName("PolynomialFactorizationCantorZassenhaus::handlePrimeDegreeSeparatedFactor");
  int linearTermsToTry = 500;
  if (this->one.modulus < linearTermsToTry) {
    this->one.modulus.isIntegerFittingInInt(&linearTermsToTry);
  }
  List<ElementZmodP> foundRoots;
  Vector<ElementZmodP> rootCandidate;
  rootCandidate.addOnTop(this->one);
  for (int i = 0; i < linearTermsToTry; i ++) {
    rootCandidate[0].value = static_cast<unsigned>(i);
    ElementZmodP value = input.evaluate(rootCandidate, this->one.zero());
    if (value.isEqualToZero()) {
      foundRoots.addOnTop(rootCandidate[0]);
    }
  }
  if (foundRoots.size == 0) {
    if (this->one.modulus <= linearTermsToTry) {
      // No linear factors; the current factor is irreducible
      // as it is of prime degree and all of its sub-factors
      // are of equal degree.
      return this->output->accountReducedFactor(input, true);
    }
    // It is possible that the current factor has
    // linear factors we haven't checked for.
    return false;
  }
  for (int i = 0; i < foundRoots.size; i ++) {
    Polynomial<ElementZmodP> linearFactor;
    linearFactor.makeMonomial(0, 1, this->one);
    linearFactor -= foundRoots[i];
    this->output->accountReducedFactor(linearFactor, false);
  }
  if (this->current.totalDegree() > 0) {
    this->output->accountNonReducedFactor(this->current);
  }
  return true;
}

bool PolynomialFactorizationCantorZassenhaus::oneFactorGo(
  std::stringstream* comments,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("PolynomialFactorizationCantorZassenhaus::oneFactorGo");
  (void) commentsOnFailure;
  this->baseLetter.modulus = this->output->current;
  this->baseLetter.value.makeDegreeOne(1, 0, this->one, this->one.zero());
  this->oneQuotientRing.modulus = this->current;
  this->oneQuotientRing.value.makeConstant(this->one);

  if (this->hasFactorsOfDifferentDegree(comments)) {
    return true;
  }
  if (comments != nullptr) {
    *comments << "<br>All divisors of " << this->current.toString(&this->output->format)
    << " are of equal degree. ";
  }
  if (this->current.totalDegree().getNumerator().value.isPossiblyPrime(0, true, comments)) {
    if (this->handlePrimeDegreeSeparatedFactor(this->current)) {
      return true;
    }
  }
  if (
    this->current.totalDegree() == 1
  ) {
    return this->output->accountReducedFactor(this->current, true);
  }
  int maximumDivisors = 10;
  if (this->one.modulus < maximumDivisors) {
    this->one.modulus.isIntegerFittingInInt(&maximumDivisors);
  }
  ProgressReport report;
  for (unsigned i = 0; i < static_cast<unsigned>(maximumDivisors); i ++) {
    std::stringstream reportStream;
    reportStream << "Looking for factors round " << i + 1 << " out of " << maximumDivisors;
    report.report(reportStream.str());
    if (this->oneFactorProbabilityHalf(i, comments, commentsOnFailure)) {
      return true;
    }
  }
  return this->output->accountReducedFactor(this->current, true);
}

bool PolynomialFactorizationCantorZassenhaus::oneFactorProbabilityHalf(
  unsigned int constant,
  std::stringstream *comments,
  std::stringstream *commentsOnFailure
) {
  (void) commentsOnFailure;
  LargeIntegerUnsigned modulus = this->one.modulus;
  int degree = this->current.totalDegreeInt();
  PolynomialModuloPolynomial<ElementZmodP> startingPolynomial;
  startingPolynomial = this->baseLetter;
  ElementZmodP constantModular;
  constantModular.modulus = modulus;
  constantModular.value = constant;
  startingPolynomial.value += constantModular;
  PolynomialModuloPolynomial<ElementZmodP> currentPolynomial;
  for (int i = 0; i < degree; i ++) {
    currentPolynomial = startingPolynomial;
    LargeInteger power = modulus;
    power.raiseToPower(i + 1);
    power -= 1;
    power /= 2;
    MathRoutines::raiseToPower(currentPolynomial, power, oneQuotientRing);
    if (comments != nullptr) {
      *comments << "<br>\\(A=x^{\\frac{" << modulus << "^{" << i + 1 << "} - 1}{2}} \\)=\\( "
      << this->one.toStringPolynomial(currentPolynomial.value, &this->output->format) << "\\)";
    }
    if (this->divisorFromCandidate(currentPolynomial.value, "A", comments)) {
      return true;
    }
    if (this->divisorFromCandidate(currentPolynomial.value + oneQuotientRing.value, "A + 1", comments)) {
      return true;
    }
    if (this->divisorFromCandidate(currentPolynomial.value + oneQuotientRing.value, "A - 1", comments)) {
      return true;
    }
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
  List<MonomialPolynomial>::Comparator* monomialOrder = &MonomialPolynomial::orderDefault();
  if (this->minimalNumberOfVariables() > 1) {
    return false;
  }
  output.setSize(0);
  if (this->minimalNumberOfVariables() == 0 || this->isEqualToZero()) {
    return true;
  }
  Polynomial<Rational> myCopy;
  myCopy = *this;
  myCopy.scaleNormalizeLeadingMonomial(&MonomialPolynomial::orderDefault());
  Rational lowestTerm, highestCoefficient;
  this->getConstantTerm(lowestTerm);
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
  highestCoefficient = this->getLeadingCoefficient(monomialOrder);
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
  if (!hT.getDivisors(divisorsH, false) || !lT.getDivisors(divisorsS, true)) {
    return false;
  }
  for (int i = 0; i < divisorsH.size; i ++) {
    for (int j = 0; j < divisorsS.size; j ++) {
      tempV[0].assignNumeratorAndDenominator(divisorsS[j], divisorsH[i]);
      val = myCopy.evaluate(tempV);
      if (val == 0) {
        Polynomial<Rational> divisor, remainder;
        divisor.makeDegreeOne(1, 0, 1, - tempV[0]);
        myCopy.divideBy(divisor, myCopy, remainder, monomialOrder);
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

bool PolynomialFactorizationKronecker::oneFactor(
  std::stringstream* comments,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Polynomial::oneFactor");
  (void) comments;
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
  Vector<Rational> allPointsOfEvaluation;
  List<List<LargeInteger> > primeFactorsAtPoints;
  List<List<int> > thePrimeFactorsMults;
  Vector<Rational> theValuesAtPoints, valuesAtRemainingPoints;
  allPointsOfEvaluation.setSize(degree + 1);
  primeFactorsAtPoints.setSize(degreeLeft + 1);
  thePrimeFactorsMults.setSize(degreeLeft + 1);
  allPointsOfEvaluation[0] = 0;
  for (int i = 1; i < allPointsOfEvaluation.size; i ++) {
    allPointsOfEvaluation[i] = (i % 2 == 1) ? i / 2 + 1 : - (i / 2);
  }
  Vector<Rational> theArgument;
  theArgument.setSize(1);
  theValuesAtPoints.setSize(allPointsOfEvaluation.size);
  LargeInteger tempLI;
  for (int i = 0; i < allPointsOfEvaluation.size; i ++) {
    theArgument[0] = allPointsOfEvaluation[i];
    theValuesAtPoints[i] = this->current.evaluate(theArgument);
    if (theValuesAtPoints[i].isEqualToZero()) {
      Polynomial<Rational> incomingFactor;
      incomingFactor.makeDegreeOne(1, 0, 1, - theArgument[0]);
      this->output->accountReducedFactor(incomingFactor, true);
      return true;
    }
    if (i > degreeLeft) {
      continue;
    }
    theValuesAtPoints[i].isInteger(&tempLI);
    if (!tempLI.value.factor(
      primeFactorsAtPoints[i], thePrimeFactorsMults[i], 0, 3, commentsOnFailure
    )) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "<br>Aborting polynomial factorization: "
        << "failed to factor the integer "
        << theValuesAtPoints[i].toString()
        << " (most probably the integer is too large).";
      }
      return false;
    }
    primeFactorsAtPoints[i].addOnTop(- 1);
    thePrimeFactorsMults[i].addOnTop(1);
  }
  Incrementable<Incrementable<SelectionOneItem> > divisorSelection;
  Vectors<Rational> valuesLeftInterpolands;
  Vector<Rational> PointsOfInterpolationLeft;
  PointsOfInterpolationLeft.reserve(degreeLeft + 1);
  Rational currentPrimePowerContribution, currentPointContribution;
  for (int i = 0; i <= degreeLeft; i ++) {
    PointsOfInterpolationLeft.addOnTop(allPointsOfEvaluation[i]);
  }
  divisorSelection.initFromMults(thePrimeFactorsMults);
  valuesLeftInterpolands.setSize(degreeLeft + 1);
  Polynomial<Rational>::getValuesLagrangeInterpolands(
    PointsOfInterpolationLeft, allPointsOfEvaluation, valuesLeftInterpolands
  );
  ProgressReport report(1000, GlobalVariables::Response::ReportType::general);
  LargeInteger total = divisorSelection.totalCombinations();
  do {
    if (report.tickAndWantReport()) {
      std::stringstream reportStream;
      reportStream << "Trying divisor combination "
      << report.ticks << " out of " << total.toString();
      report.report(reportStream.str());
    }
    this->output->computations ++;
    if (
      this->output->maximumComputations > 0 &&
      this->output->computations > this->output->maximumComputations
    ) {
      // We've exceeded our computational budget.
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Kronecker factorization ran too "
        << "many combinations: maximum allowed was: "
        << this->output->maximumComputations
        << ", but the total computations was "
        << total.toString() << ". ";
      }
      return false;
    }
    valuesAtRemainingPoints.makeZero(theValuesAtPoints.size);
    Rational firstValue;
    bool isGood = false; //<-we shall first check if the divisor is constant.
    for (int j = 0; j < divisorSelection.elements.size; j ++) {
      currentPointContribution = 1;
      for (int k = 0; k < divisorSelection[j].elements.size; k ++) {
        currentPrimePowerContribution = primeFactorsAtPoints[j][k];
        currentPrimePowerContribution.raiseToPower(divisorSelection[j][k].amount);
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
      valuesAtRemainingPoints += valuesLeftInterpolands[j] * currentPointContribution;
    }
    if (!isGood) {
      continue;
    }
    for (int j = 0; j < allPointsOfEvaluation.size; j ++) {
      if (valuesAtRemainingPoints[j].isEqualToZero()) {
        isGood = false;
        break;
      }
      currentPointContribution = theValuesAtPoints[j] / valuesAtRemainingPoints[j];
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
      valuesAtRemainingPoints
    );
    if (this->output->accountNonReducedFactor(incoming)) {
      return true;
    }
  } while (divisorSelection.incrementReturnFalseIfPastLast());
  return this->output->accountReducedFactor(this->current, true);
}

bool PolynomialFactorizationKronecker::solvePolynomial(
  const Polynomial<Rational>& input,
  List<AlgebraicNumber>& output,
  AlgebraicClosureRationals& closure,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("PolynomialFactorizationKronecker::solvePolynomial");
  PolynomialFactorizationUnivariate<Rational, PolynomialFactorizationKronecker> factorization;
  if (input.totalDegree() < 1) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Polynomial is of degree below 1. ";
    }
    return false;
  }
  if (input.minimalNumberOfVariables() > 1) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Polynomial is not univariate. ";
    }
    return false;
  }
  if (!factorization.factor(input, nullptr, nullptr)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to factor polynomial. ";
    }
    return false;
  }
  MonomialPolynomial x(0, 1);
  MonomialPolynomial xSquared(0, 2);
  Rational a, b, c;
  for (int i = 0; i < factorization.reduced.size; i ++) {
    Polynomial<Rational>& factor = factorization.reduced[i];
    c = factor.getConstantTerm(0);
    b = factor.getCoefficientOf(x);
    a = factor.getCoefficientOf(xSquared);
    if (factor.totalDegree() > 2 || factor.totalDegree() < 1) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Degree of factor "
        << factor.toString() << " is not 1 or 2. ";
      }
      return false;
    }
    if (factor.totalDegree() == 1) {
      output.addOnTop(- c / b);
    }
    if (factor.totalDegree() == 2) {
      AlgebraicNumber squareRootOfDiscriminant;
      Rational discriminant = factor.getDiscriminant();
      if (!squareRootOfDiscriminant.assignRationalQuadraticRadical(
        discriminant, closure, nullptr
      )) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "Failed to extract the square root of the discriminant: "
          << discriminant.toString() << ".";
        }
        return false;
      }
      AlgebraicNumber xOne = (squareRootOfDiscriminant - b) / (2 * a);
      AlgebraicNumber xTwo = (squareRootOfDiscriminant * (- 1) - b) / (2 * a);
      output.addOnTop(xOne);
      output.addOnTop(xTwo);
    }
  }
  return true;
}

PolynomialFactorizationFiniteFields::PolynomialFactorizationFiniteFields() {
  this->output = nullptr;
  this->degree = 0;
  this->factorsLiftedTries = 0;
  this->maximumFactorsLiftedTries = 50000;
}

bool PolynomialFactorizationFiniteFields::oneFactor(
  std::stringstream* comments, std::stringstream* commentsOnFailure
) {
  this->current = this->output->current;
  if (this->current.minimalNumberOfVariables() > 1) {
    return false;
  }
  if (this->current.minimalNumberOfVariables() == 0) {
    return true;
  }
  this->degree = 0;
  if (!this->current.totalDegree().isSmallInteger(&this->degree)) {
    return false;
  }
  Vector<Polynomial<Rational> > derivative;
  if (!this->current.differential(derivative, commentsOnFailure)) {
    return false;
  }
  Polynomial<Rational> greatestCommonDivisor;
  derivative[0].greatestCommonDivisor(
    derivative[0],
    this->current,
    greatestCommonDivisor,
    Rational::one(),
    commentsOnFailure
  );
  if (!greatestCommonDivisor.isConstant()) {
    this->output->accountNonReducedFactor(greatestCommonDivisor);
    return true;
  }
  unsigned int maximumPrimeToTry = 10000;
  List<unsigned int> primeFactors;
  LargeIntegerUnsigned::getPrimesEratosthenesSieve(maximumPrimeToTry, primeFactors);
  this->leadingCoefficient = this->current.getLeadingCoefficient(
    &MonomialPolynomial::orderDefault()
  ).getNumerator();
  for (int i = 1; i < primeFactors.size; i ++) {
    if (this->leadingCoefficient.value % primeFactors[i] == 0) {
      // Leading coefficient is divisible by prime.
      continue;
    }
    this->oneModular.makeOne(primeFactors[i]);
    ElementZmodP::convertModuloIntegerAfterScalingToIntegral(
      this->current, this->modularization, primeFactors[i]
    );
    if (
      !this->modularization.isSquareFree(this->oneModular, nullptr) ||
      modularization.totalDegreeInt() != degree
    ) {
      continue;
    }
    this->modularization.scaleNormalizeLeadingMonomial(&MonomialPolynomial::orderDefault());
    return this->oneFactorFromModularization(comments, commentsOnFailure);
  }
  return false;
}

bool PolynomialFactorizationFiniteFields::oneFactorFromModularization(
  std::stringstream* comments, std::stringstream* commentsOnFailure
) {
  PolynomialFactorizationUnivariate<ElementZmodP, PolynomialFactorizationCantorZassenhaus> factorizationModular;
  if (!factorizationModular.factor(this->modularization, comments, commentsOnFailure)) {
    return false;
  }
  this->factorsOverPrime = factorizationModular.reduced;
  this->format.flagSuppressModP = true;
  this->computeCoefficientBounds();
  this->henselLift(comments);
  return this->factorizationFromHenselLift(comments, commentsOnFailure);
}

void PolynomialFactorizationFiniteFields::computeCoefficientBounds() {
  MacroRegisterFunctionWithName("PolynomialFactorizationFiniteFields::computeCoefficientBounds");
  this->largestCoefficient = 0;
  for (int i = 0; i < this->current.size(); i ++) {
    this->largestCoefficient = MathRoutines::maximum(
      LargeInteger(this->current.coefficients[i].getNumerator().value),
      this->largestCoefficient
    );
  }
  // 2 times the largest coefficient gives an upper bound for the absolute value
  // of the complex roots of the polynomial.
  this->upperBoundAbsoluteValueRoot = this->largestCoefficient * 2;
  // Let n be the polynomial degree.
  // Let a_k be the coefficient of degree k.
  // Vieta's formulas imply that
  // a_k = +/- leadingCoefficient * \sum_{i_1,..., i_k} x_{i_1}... x_{i_k},
  // where x_{1}, ..., x_{n} are the complex roots of the polynomial and
  // i_1, ..., i_k run over the different roots of the polynomial.
  // There are (n choose k) summands and each summand can be bounded above
  // by replacing x_j with upperBoundAbsoluteValueRoot.
  // Therefore |a_k| <= leadingCoefficient * (n choose k) * upperBoundAbsoluteValueRoot^k
  // Since the largest binomial coefficient is (n choose floor(n/2)), this is
  // bounded above by
  // leadingCoefficient * (n choose floor(n/2)) * upperBoundAbsoluteValueRoot^n
  this->coefficientBound = this->upperBoundAbsoluteValueRoot;
  this->coefficientBound.raiseToPower(this->degree);
  this->coefficientBound *= this->largestCoefficient;
  this->coefficientBound *= MathRoutines::nChooseK(this->degree, this->degree / 2);
}

void PolynomialFactorizationFiniteFields::henselLift(std::stringstream* comments) {
  MacroRegisterFunctionWithName("PolynomialFactorizationFiniteFields::henselLift");
  SylvesterMatrix<ElementZmodP>::sylvesterMatrixProduct(this->factorsOverPrime, this->sylvesterMatrix, comments);

  if (comments != nullptr) {
    *comments << "Sylvester matrix: " << this->sylvesterMatrix.toString(&this->format);
  }
  this->sylvesterMatrixInverted = this->sylvesterMatrix;
  bool mustBeTrue = this->sylvesterMatrixInverted.invert();
  if (!mustBeTrue) {
    global.fatal << "Sylvester product matrix is supposed to be invertible. " << global.fatal;
  }
  this->factorsLifted = this->factorsOverPrime;
  this->modulusHenselLift = this->oneModular.modulus;
  while (this->modulusHenselLift < this->coefficientBound) {
    LargeIntegerUnsigned oldModulus = this->modulusHenselLift;
    this->modulusHenselLift *= this->oneModular.modulus;
    this->henselLiftOnce(oldModulus, comments);
  }
}

void PolynomialFactorizationFiniteFields::henselLiftOnce(
  const LargeIntegerUnsigned& oldModulus,
  std::stringstream* comments
) {
  MacroRegisterFunctionWithName("PolynomialFactorizationFiniteFields::henselLiftOnce");
  for (int i = 0; i < this->factorsLifted.size; i ++) {
    ElementZmodP::convertLiftPolynomialModular(
      this->factorsLifted[i], this->factorsLifted[i], this->modulusHenselLift
    );
  }
  if (comments != nullptr) {
    *comments << "<hr>"
    << "Modulus: " << this->modulusHenselLift << ". "
    << "Lifted factors without correction: "
    << this->factorsLifted.toStringCommaDelimited(&this->format);

  }
  ElementZmodP::convertModuloIntegerAfterScalingToIntegral(
    this->current, this->desiredLift, this->modulusHenselLift
  );
  this->scaleProductLift = this->desiredLift.scaleNormalizeLeadingMonomial(&MonomialPolynomial::orderDefault());
  Polynomial<ElementZmodP> newProduct;
  ElementZmodP one;
  one.makeOne(this->modulusHenselLift);
  newProduct.makeConstant(one);
  for (int i = 0; i < this->factorsLifted.size; i ++) {
    newProduct *= this->factorsLifted[i];
  }
  if (comments != nullptr) {
    *comments << "<br>Lifted product without correction: "
    << newProduct.toString(&this->format);
  }
  newProduct -= this->desiredLift;
  if (comments != nullptr) {
    *comments << "<br>Desired lift: " << desiredLift.toString(&this->format);
    *comments << "<br>To be corrected: " << newProduct.toString(&this->format);
  }
  Vector<ElementZmodP> coefficientsCorrection;
  coefficientsCorrection.makeZero(
    this->sylvesterMatrixInverted.numberOfColumns,
    this->oneModular.zero()
  );
  ElementZmodP minusOne;
  minusOne.makeMinusOne(this->oneModular.modulus);
  for (int i = 0; i < newProduct.size(); i ++) {
    int index =
    this->sylvesterMatrixInverted.numberOfColumns -
    newProduct.monomials[i].totalDegreeInt() - 1;
    coefficientsCorrection[index].value =
    newProduct.coefficients[i].value / oldModulus;
    coefficientsCorrection[index] *= minusOne;
  }
  if (comments != nullptr) {
    *comments << "<br>Z-vector: " << coefficientsCorrection.toString(&this->format);
  }
  this->sylvesterMatrixInverted.actOnVectorColumn(
    coefficientsCorrection, this->oneModular.zero()
  );
  if (comments != nullptr) {
    *comments << "<br>solution vector: " << coefficientsCorrection.toString(&this->format);
  }
  int offset = 0;
  for (int i = 0; i < this->factorsLifted.size; i ++) {
    int summandsCurrentFactor =
    this->factorsLifted[i].totalDegreeInt();
    for (int j = 0; j < summandsCurrentFactor; j ++) {
      ElementZmodP incoming;
      incoming.modulus = this->modulusHenselLift;
      int index = offset + summandsCurrentFactor - 1 - j;
      incoming.value = coefficientsCorrection[index].value * oldModulus;
      this->factorsLifted[i].addMonomial(MonomialPolynomial(0, j), incoming);
    }
    offset += summandsCurrentFactor;
  }
  if (comments != nullptr) {
    *comments << "<br>Lifted factors, modulus: "
    << this->factorsLifted.toStringCommaDelimited(&this->format);
  }
  Polynomial<ElementZmodP> productLift;
  productLift.makeConstant(one);
  for (int i = 0; i < this->factorsLifted.size; i ++) {
    productLift *= this->factorsLifted[i];
  }
  if (comments != nullptr) {
    *comments << "<br>Lifted product, corrected, before rescaling: "
    << productLift.toString(&this->format);
  }
  productLift /= scaleProductLift;
  if (comments != nullptr) {
    *comments << "<br>Lifted product, corrected: "
    << productLift.toString(&this->format);
    Polynomial<Rational> negativesIncluded;
    ElementZmodP::convertPolynomialModularToPolynomialIntegral(productLift, negativesIncluded, true);
    *comments << "<br>Lifted product, over Z: " << negativesIncluded.toString(&this->format) << "<hr>";
  }
}

bool PolynomialFactorizationFiniteFields::tryFactor(SelectionFixedRank& selection) {
  MacroRegisterFunctionWithName("PolynomialFactorizationFiniteFields::tryFactor");
  Polynomial<ElementZmodP> product;
  ElementZmodP start;
  start.makeOne(this->modulusHenselLift);
  start /= this->scaleProductLift;
  product.makeConstant(start);
  for (int i = 0; i < selection.selection.cardinalitySelection; i ++) {
    product *= this->factorsLifted[selection.selection.elements[i]];
  }
  Polynomial<Rational> candidate;
  ElementZmodP::convertPolynomialModularToPolynomialIntegral(product, candidate, true);
  if (!this->output->accountReducedFactor(candidate, false)) {
    return false;
  }
  selection.selection.invertSelection();
  List<Polynomial<ElementZmodP> > factorsLiftedTrimmed;
  for (int i = 0; i < selection.selection.cardinalitySelection; i ++) {
    factorsLiftedTrimmed.addOnTop(this->factorsLifted[selection.selection.elements[i]]);
  }
  this->factorsLifted = factorsLiftedTrimmed;
  return true;
}

bool PolynomialFactorizationFiniteFields::factorizationFromHenselLift(
  std::stringstream *comments, std::stringstream *commentsOnFailure
) {
  MacroRegisterFunctionWithName("PolynomialFactorizationFiniteFields::factorizationFromHenselLift");
  this->maximumFactorsLiftedTries = 100;
  while (this->factorsLifted.size > 0) {
    if (!this->factorizationFromHenselLiftOnce(comments, commentsOnFailure)) {
      return false;
    }
  }
  return true;
}

bool PolynomialFactorizationFiniteFields::factorizationFromHenselLiftOnce(
  std::stringstream* comments, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("PolynomialFactorizationFiniteFields::factorizationFromHenselLiftOnce");
  (void) comments;
  SelectionFixedRank selection;
  // global.comments << "And the factors are: " << this->factorsLifted.toStringCommaDelimited();
  for (int i = 1; i <= this->factorsLifted.size; i ++) {
    selection.setNumberOfItemsAndDesiredSubsetSize(i, this->factorsLifted.size);
    do {
      this->factorsLiftedTries ++;
      if (this->factorsLiftedTries > this->maximumFactorsLiftedTries) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "The maximum allowed Hensel lift combinations is: "
          << this->maximumFactorsLiftedTries << ". ";
        }
        return false;
      }
      if (this->tryFactor(selection)) {
        return true;
      }
    } while (selection.incrementReturnFalseIfPastLast());
  }
  // Since the product of all lifts should be equal to the
  // original polynomial, in the worst case scenario,
  // the loop above should return on
  // its final iteration.
  // Therefore if we reach this piece of code we've made
  // an algorithmic or programming error.
  global.fatal << "This code section should not be reachable. " << global.fatal;
  return true;
}
