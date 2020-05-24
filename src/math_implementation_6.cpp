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
  if (
    this->current.totalDegree().getNumerator().value.isPossiblyPrime(0, true, comments) ||
    this->current.totalDegree() == 1
  ) {
    return this->output->accountReducedFactor(this->current);
  }
  int maximumDivisors = 10;
  if (this->one.modulus < maximumDivisors) {
    this->one.modulus.isIntegerFittingInInt(&maximumDivisors);
  }
  ProgressReport report;
  for (int i = 0; i < maximumDivisors; i ++) {
    std::stringstream reportStream;
    reportStream << "Looking for factors round " << i + 1 << " out of " << maximumDivisors;
    report.report(reportStream.str());
    if (this->oneFactorProbabilityHalf(i, comments, commentsOnFailure)) {
      return true;
    }
  }
  return this->output->accountReducedFactor(this->current);
}

bool PolynomialFactorizationCantorZassenhaus::oneFactorProbabilityHalf(
  int constant,
  std::stringstream *comments,
  std::stringstream *commentsOnFailure
) {
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
      thePrimeFactorsAtPoints[i], thePrimeFactorsMults[i], 0, 3, commentsOnFailure
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
  MonomialP x(0, 1);
  MonomialP xSquared(0, 2);
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
  LargeIntegerUnsigned leadingCoefficient = this->current.getLeadingCoefficient(
    &MonomialP::orderDefault()
  ).getNumerator().value;
  for (int i = 1; i < primeFactors.size; i ++) {
    if (leadingCoefficient % primeFactors[i] == 0) {
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
    this->modularization.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
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
  this->format.flagSuppressModP = true;
  if (commentsOnFailure != nullptr) {
    *commentsOnFailure << "DEBUG Factorization mod " << this->oneModular.modulus.toString()
    << ": " << factorizationModular.toStringResult(&this->format);
  }
  return false;
}

void PolynomialFactorizationFiniteFields::computeCoefficientBounds() {
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
  this->coefficientBound = this->largestCoefficient;
  this->coefficientBound *= MathRoutines::nChooseK(this->degree, this->degree / 2);
}
