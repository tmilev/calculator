// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "math_general_implementation.h"
#include "math_extra_polynomial_factorization.h"

template < >
void PolynomialConversions::convertToPolynomial(
  const PolynomialUnivariateModular& input,
  Polynomial<ElementZmodP>& output
) {
  output.makeZero();
  ElementZmodP coefficient;
  MonomialPolynomial monomial;
  for (int i = 0; i < input.coefficients.size; i ++) {
    coefficient.makeFrom(
      input.modulusData->modulus, input.coefficients[i]
    );
    monomial.makeEi(0, i);
    output.addMonomial(monomial, coefficient);
  }
}

template <class Coefficient>
bool Polynomial<Coefficient>::isSquareFreeAndUnivariate(
  const Coefficient& one, std::stringstream* comments
) const {
  int numberOfVariables = this->minimalNumberOfVariables();
  if (numberOfVariables > 1) {
    if (comments != nullptr) {
      *comments
      << "Not univariate: polynomial has "
      << numberOfVariables
      << " variables. ";
    }
    return false;
  }
  return this->isSquareFree(one, comments);
}

template < >
bool Polynomial<Rational>::findOneVariableRationalRoots(
  List<Rational>& output
) {
  STACK_TRACE("Polynomial::findOneVariableRationalRoots");
  List<MonomialPolynomial>::Comparator* monomialOrder =
  &MonomialPolynomial::orderDefault();
  if (this->minimalNumberOfVariables() > 1) {
    return false;
  }
  output.setSize(0);
  if (this->minimalNumberOfVariables() == 0 || this->isEqualToZero()) {
    return true;
  }
  Polynomial<Rational> myCopy;
  myCopy = *this;
  myCopy.scaleNormalizeLeadingMonomial(
    &MonomialPolynomial::orderDefault()
  );
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
  if (
    !highestCoefficient.isSmallInteger() || !lowestTerm.isSmallInteger()
  ) {
    return false;
  }
  Vector<Rational> tempV;
  Rational val;
  tempV.setSize(1);
  List<int> divisorsH, divisorsS;
  LargeInteger hT, lT;
  hT = highestCoefficient.getNumerator();
  lT = lowestTerm.getNumerator();
  if (
    !hT.getDivisors(divisorsH, false) || !lT.getDivisors(divisorsS, true)
  ) {
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

template < >
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
    for (
      int j = pointsOfInterpolation.size; j < pointsOfEvaluation.size; j ++
    ) {
      currentInterpoland[j] = 1;
      for (int k = 0; k < pointsOfInterpolation.size; k ++) {
        if (i == k) {
          continue;
        }
        currentInterpoland[j] *=
        pointsOfEvaluation[j] - pointsOfInterpolation[k];
        currentInterpoland[j] /=
        pointsOfInterpolation[i] - pointsOfInterpolation[k];
      }
    }
  }
}

bool PolynomialFactorizationKronecker::oneFactor(
  std::stringstream* comments, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Polynomial::oneFactor");
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
  List<List<int> > primeFactorsMults;
  Vector<Rational> valuesAtPoints, valuesAtRemainingPoints;
  allPointsOfEvaluation.setSize(degree + 1);
  primeFactorsAtPoints.setSize(degreeLeft + 1);
  primeFactorsMults.setSize(degreeLeft + 1);
  allPointsOfEvaluation[0] = 0;
  for (int i = 1; i < allPointsOfEvaluation.size; i ++) {
    allPointsOfEvaluation[i] = (i % 2 == 1) ? i / 2 + 1 : -(i / 2);
  }
  Vector<Rational> argument;
  argument.setSize(1);
  valuesAtPoints.setSize(allPointsOfEvaluation.size);
  LargeInteger tempLI;
  for (int i = 0; i < allPointsOfEvaluation.size; i ++) {
    argument[0] = allPointsOfEvaluation[i];
    valuesAtPoints[i] = this->current.evaluate(argument);
    if (valuesAtPoints[i].isEqualToZero()) {
      Polynomial<Rational> incomingFactor;
      incomingFactor.makeDegreeOne(1, 0, 1, - argument[0]);
      this->output->accountReducedFactor(incomingFactor, true);
      return true;
    }
    if (i > degreeLeft) {
      continue;
    }
    valuesAtPoints[i].isInteger(&tempLI);
    if (
      !tempLI.value.factor(
        primeFactorsAtPoints[i],
        primeFactorsMults[i],
        0,
        3,
        commentsOnFailure
      )
    ) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "<br>Aborting polynomial factorization: "
        << "failed to factor the integer "
        << valuesAtPoints[i].toString()
        << " (most probably the integer is too large).";
      }
      return false;
    }
    primeFactorsAtPoints[i].addOnTop(- 1);
    primeFactorsMults[i].addOnTop(1);
  }
  Incrementable<Incrementable<SelectionOneItem> > divisorSelection;
  Vectors<Rational> valuesLeftInterpolands;
  Vector<Rational> pointsOfInterpolationLeft;
  pointsOfInterpolationLeft.reserve(degreeLeft + 1);
  Rational currentPrimePowerContribution, currentPointContribution;
  for (int i = 0; i <= degreeLeft; i ++) {
    pointsOfInterpolationLeft.addOnTop(allPointsOfEvaluation[i]);
  }
  divisorSelection.initializeFromMultiplicities(primeFactorsMults);
  valuesLeftInterpolands.setSize(degreeLeft + 1);
  Polynomial<Rational>::getValuesLagrangeInterpolands(
    pointsOfInterpolationLeft, allPointsOfEvaluation, valuesLeftInterpolands
  );
  ProgressReport report(
    1000, GlobalVariables::Response::ReportType::general
  );
  LargeInteger total = divisorSelection.totalCombinations();
  do {
    if (report.tickAndWantReport()) {
      std::stringstream reportStream;
      reportStream
      << "Trying divisor combination "
      << report.ticks
      << " out of "
      << total.toString();
      report.report(reportStream.str());
    }
    this->output->computations ++;
    if (
      this->output->maximumComputations > 0 &&
      this->output->computations > this->output->maximumComputations
    ) {
      // We've exceeded our computational budget.
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Kronecker factorization ran too "
        << "many combinations: maximum allowed was: "
        << this->output->maximumComputations
        << ", but the total computations was "
        << total.toString()
        << ". ";
      }
      return false;
    }
    valuesAtRemainingPoints.makeZero(valuesAtPoints.size);
    Rational firstValue;
    bool isGood = false;
    // <-we shall first check if the divisor is constant.
    for (int j = 0; j < divisorSelection.elements.size; j ++) {
      currentPointContribution = 1;
      for (int k = 0; k < divisorSelection[j].elements.size; k ++) {
        currentPrimePowerContribution = primeFactorsAtPoints[j][k];
        currentPrimePowerContribution.raiseToPower(
          divisorSelection[j][k].amount
        );
        currentPointContribution *= currentPrimePowerContribution;
      }
      if (!isGood) {
        if (j == 0) {
          firstValue = currentPointContribution;
        } else {
          if (firstValue != currentPointContribution) {
            isGood = true;
            // <- the divisor has takes two different values, hence is
            // non-constant.
          }
        }
      }
      valuesAtRemainingPoints +=
      valuesLeftInterpolands[j] * currentPointContribution;
    }
    if (!isGood) {
      continue;
    }
    for (int j = 0; j < allPointsOfEvaluation.size; j ++) {
      if (valuesAtRemainingPoints[j].isEqualToZero()) {
        isGood = false;
        break;
      }
      currentPointContribution = valuesAtPoints[j] /
      valuesAtRemainingPoints[j];
      if (!currentPointContribution.isInteger()) {
        isGood = false;
        break;
      }
    }
    if (!isGood) {
      continue;
    }
    Polynomial<Rational> incoming;
    incoming.interpolate(pointsOfInterpolationLeft, valuesAtRemainingPoints);
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
  STACK_TRACE("PolynomialFactorizationKronecker::solvePolynomial");
  PolynomialFactorizationUnivariate<Rational> factorization;
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
  PolynomialFactorizationFiniteFields algorithm;
  if (!factorization.factor(input, algorithm, nullptr, nullptr)) {
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
        *commentsOnFailure
        << "Degree of factor "
        << factor.toString()
        << " is not 1 or 2. ";
      }
      return false;
    }
    if (factor.totalDegree() == 1) {
      output.addOnTop(- c / b);
    }
    if (factor.totalDegree() == 2) {
      AlgebraicNumber squareRootOfDiscriminant;
      Rational discriminant = factor.getDiscriminant();
      if (
        !squareRootOfDiscriminant.assignRationalQuadraticRadical(
          discriminant, closure, nullptr
        )
      ) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "Failed to extract the square root of the discriminant: "
          << discriminant.toString()
          << ".";
        }
        return false;
      }
      AlgebraicNumber xOne = (squareRootOfDiscriminant - b) / (2* a);
      AlgebraicNumber xTwo = (squareRootOfDiscriminant *(- 1) - b) / (2* a);
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
  this->millisecondsCantorZassenhaus = 0;
  this->millisecondsLift = 0;
  this->millisecondsFactorizationFromLift = 0;
  this->millisecondsTotal = 0;
  this->millisecondsSquareFree = 0;
  this->numberOfRunsOfFactor = 0;
}

bool PolynomialFactorizationFiniteFields::factorWithTiming(
  std::stringstream* comments, std::stringstream* commentsOnFailure
) {
  int64_t startMilliseconds = global.getElapsedMilliseconds();
  this->millisecondsLift = 0;
  this->millisecondsTotal = 0;
  this->millisecondsSquareFree = 0;
  this->millisecondsCantorZassenhaus = 0;
  this->millisecondsFactorizationFromLift = 0;
  this->numberOfRunsOfFactor ++;
  bool result = this->factor(comments, commentsOnFailure);
  int64_t ellapsedMillseconds =
  global.getElapsedMilliseconds() - startMilliseconds;
  if (this->millisecondsTotal == 0) {
    this->millisecondsTotal = ellapsedMillseconds;
  } else {
    global
    << Logger::red
    << "Second run of factorWithTiming; input: "
    << this->output->original.toString()
    << ". Milliseconds: "
    << ellapsedMillseconds
    << " ms. "
    << Logger::endL;
  }
  return result;
}

bool PolynomialFactorizationFiniteFields::hasSquareFactor(
  std::stringstream* comments, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("PolynomialFactorizationFiniteFields::hasSquareFactor");
  (void) comments;
  List<int> quickCheckPrimes = List<int>({101, 103, 107, 109, 113});
  for (int i = 0; i < quickCheckPrimes.size; i ++) {
    PolynomialUnivariateModular converted;
    PolynomialUnivariateModular derivative;
    PolynomialUnivariateModular greatestCommonDivisorConvertedAndDerivative;
    IntegerModulusSmall modulus;
    modulus.initializeModulusData(quickCheckPrimes[i]);
    // The current polynomial is supposed to have integer coefficients,
    // hence the modular conversion cannot fail due to division by
    // zero.
    converted.makeFromPolynomialAndModulusNoFailure(
      &modulus, this->current
    );
    converted.derivative(derivative);
    converted.greatestCommonDivisor(
      converted,
      derivative,
      greatestCommonDivisorConvertedAndDerivative,
      nullptr
    );
    if (greatestCommonDivisorConvertedAndDerivative.isConstant()) {
      // The polynomial and its derivative have no
      // common factor over a small prime.
      // Therefore the pollynomial is square-free over that
      // small prime. Therefore it's square-free over the rationals.
      return false;
    }
  }
  // The polynomial is not square-free mod the primes above.
  // It could still be square free over the rationals if we are unlucky.
  // For example, x^2+101*103*107*109*113 is square free over the rationals,
  // but equals x^2 mod 101, 103, 107, 109 and 113.
  Vector<Polynomial<Rational> > derivative;
  if (!this->current.differential(derivative, commentsOnFailure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to compute the differential.";
    }
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
  return false;
}

bool PolynomialFactorizationFiniteFields::factor(
  std::stringstream* comments, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("PolynomialFactorizationFiniteFields::factor");
  this->current = this->output->current;
  if (this->current.minimalNumberOfVariables() > 1) {
    return false;
  }
  if (this->current.minimalNumberOfVariables() == 0) {
    return true;
  }
  this->degree = 0;
  if (!this->current.totalDegree().isSmallInteger(&this->degree)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Total degree is not small integer.";
    }
    return false;
  }
  int64_t squareFreeStart = global.getElapsedMilliseconds();
  if (this->hasSquareFactor(comments, commentsOnFailure)) {
    return true;
  }
  this->millisecondsSquareFree =
  global.getElapsedMilliseconds() - squareFreeStart;
  const List<unsigned int>& primeFactors =
  LargeIntegerUnsigned::allPrimesSmallerThan15Bits();
  this->leadingCoefficient =
  this->current.getLeadingCoefficient(&MonomialPolynomial::orderDefault()).
  getNumerator();
  for (int i = 1; i < primeFactors.size; i ++) {
    if (this->leadingCoefficient.value % primeFactors[i] == 0) {
      // Leading coefficient is divisible by prime.
      continue;
    }
    this->oneModular.makeOne(primeFactors[i]);
    this->smallModulus.initializeModulusData(primeFactors[i]);
    ElementZmodP::convertModuloIntegerAfterScalingToIntegral(
      this->current, this->modularization, primeFactors[i]
    );
    if (
      !this->modularization.isSquareFree(this->oneModular, nullptr) ||
      modularization.totalDegreeInt() != degree
    ) {
      continue;
    }
    this->modularization.scaleNormalizeLeadingMonomial(
      &MonomialPolynomial::orderDefault()
    );
    return this->oneFactorFromModularization(comments, commentsOnFailure);
  }
  return this->output->accountNonReducedFactorTemplate(this->current);
}

bool PolynomialFactorizationFiniteFields::oneFactorFromModularization(
  std::stringstream* comments, std::stringstream* commentsOnFailure
) {
  STACK_TRACE(
    "PolynomialFactorizationFiniteFields::oneFactorFromModularization"
  );
  this->format.flagSuppressModP = true;
  int64_t startCantorZassenhaus = global.getElapsedMilliseconds();
  PolynomialFactorizationUnivariate<ElementZmodP> factorizationModular;
  factorizationModular.format.flagSuppressModP = true;
  factorizationModular.format.makeAlphabetXYZUW();
  PolynomialFactorizationCantorZassenhaus<
    PolynomialModuloPolynomialModuloInteger,
    PolynomialUnivariateModular,
    PolynomialUnivariateModularAsModulus
  > algorithm;
  PolynomialUnivariateModular zero;
  zero.makeZero(&this->smallModulus);
  algorithm.initialize(zero);
  if (
    !factorizationModular.factor(
      this->modularization, algorithm, comments, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to factor modular polynomial: "
      << this->modularization.toString();
    }
    return false;
  }
  this->millisecondsCantorZassenhaus =
  global.getElapsedMilliseconds() - startCantorZassenhaus;
  this->factorsOverPrime = factorizationModular.reduced;
  //  this->computeCoefficientBoundsElementary();
  this->computeCoefficientBoundsGelfond();
  int64_t startHenselLift = global.getElapsedMilliseconds();
  this->henselLift(comments);
  this->millisecondsLift = global.getElapsedMilliseconds() - startHenselLift;
  if (comments != nullptr) {
    *comments << "Hensel lift took: " << this->millisecondsLift << "ms. ";
  }
  int64_t startFactorizationFromHenselLift = global.getElapsedMilliseconds();
  bool result = this->factorizationFromHenselLift(comments, commentsOnFailure);
  this->millisecondsFactorizationFromLift =
  global.getElapsedMilliseconds() - startFactorizationFromHenselLift;
  return result;
}

void PolynomialFactorizationFiniteFields::computeCoefficientBoundsElementary()
{
  STACK_TRACE(
    "PolynomialFactorizationFiniteFields::computeCoefficientBoundsElementary"
  );
  this->largestCoefficient = 0;
  for (int i = 0; i < this->current.size(); i ++) {
    this->largestCoefficient =
    MathRoutines::maximum(
      LargeInteger(
        this->current.coefficients[i].getNumerator().value
      ),
      this->largestCoefficient
    );
  }
  // 2 times the largest coefficient gives an upper bound for the absolute
  // value
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
  // Therefore |a_k| <= leadingCoefficient * (n choose k) *
  // upperBoundAbsoluteValueRoot^k
  // Since the largest binomial coefficient is (n choose floor(n/2)), this is
  // bounded above by
  // leadingCoefficient * (n choose floor(n/2)) * upperBoundAbsoluteValueRoot^n
  this->coefficientBound = this->upperBoundAbsoluteValueRoot;
  this->coefficientBound.raiseToPower(this->degree);
  this->coefficientBound *= this->largestCoefficient;
  this->coefficientBound *=
  MathRoutines::nChooseK(this->degree, this->degree / 2);
}

void PolynomialFactorizationFiniteFields::computeCoefficientBoundsGelfond() {
  STACK_TRACE(
    "PolynomialFactorizationFiniteFields::computeCoefficientBoundsGelfond"
  );
  this->largestCoefficient = 0;
  for (int i = 0; i < this->current.size(); i ++) {
    this->largestCoefficient =
    MathRoutines::maximum(
      LargeInteger(
        this->current.coefficients[i].getNumerator().value
      ),
      this->largestCoefficient
    );
  }
  this->coefficientBound = 2;
  MathRoutines::raiseToPower(
    this->coefficientBound, this->degree, LargeInteger(1)
  );
  this->coefficientBound *= this->largestCoefficient;
  this->coefficientBound *= this->degree;
}

void PolynomialFactorizationFiniteFields::henselLift(
  std::stringstream* comments
) {
  STACK_TRACE("PolynomialFactorizationFiniteFields::henselLift");
  SylvesterMatrix<ElementZmodP>::sylvesterMatrixProduct(
    this->factorsOverPrime, this->sylvesterMatrix, comments
  );
  this->sylvesterMatrixInverted = this->sylvesterMatrix;
  bool mustBeTrue = this->sylvesterMatrixInverted.invert();
  if (!mustBeTrue) {
    global.fatal
    << "Sylvester product matrix is supposed to be invertible. "
    << global.fatal;
  }
  if (comments != nullptr) {
    this->format.flagSuppressModP = true;
    this->format.makeAlphabetXYZUW();
    *comments
    << "Sylvester matrix: "
    << "\\("
    << this->sylvesterMatrix.toStringLatex(&this->format)
    << "\\)"
    << ", inverted: \\("
    << this->sylvesterMatrixInverted.toStringLatex(&this->format)
    << "\\)";
    this->factorNames.clear();
    std::stringstream factorStream;
    for (int i = 0; i < this->factorsOverPrime.size; i ++) {
      if (i < 5) {
        std::string letterString;
        letterString.push_back('a' + static_cast<char>(i));
        this->factorNames.addOnTop(letterString);
      } else {
        std::stringstream out;
        out << "{a_{" << i + 1 << "}}";
        this->factorNames.addOnTop(out.str());
      }
      factorStream
      << "\\left("
      << this->factorsOverPrime[i].toString(&this->format)
      << "\\right)";
    }
    *comments
    << "\\(\\begin{array}{rcll}"
    << "f_1&\\equiv&"
    << factorStream.str()
    << "& \\mod "
    << this->oneModular.modulus
    << "\\\\";
    for (int i = 0; i < this->factorsOverPrime.size; i ++) {
      *comments
      << this->factorNames[i]
      << "_1"
      << "&\\equiv&"
      << this->factorsOverPrime[i].toString(&this->format)
      << "& \\mod "
      << this->oneModular.modulus
      << "\\\\\n";
    }
    *comments << "\\end{array}\\)";
    *comments << "<hr>";
  }
  this->factorsLifted = this->factorsOverPrime;
  this->modulusHenselLift = this->oneModular.modulus;
  if (this->coefficientBound <= this->modulusHenselLift) {
    this->coefficientBound = this->modulusHenselLift + 1;
  }
  int liftStep = 1;
  while (this->modulusHenselLift < this->coefficientBound) {
    LargeIntegerUnsigned oldModulus = this->modulusHenselLift;
    this->modulusHenselLift *= this->oneModular.modulus;
    liftStep ++;
    this->henselLiftOnce(oldModulus, liftStep, comments);
  }
}

std::string PolynomialFactorizationFiniteFields::toLatexLiftedFactors() {
  std::stringstream out;
  out << "\\(";
  for (int i = 0; i < this->factorsLifted.size; i ++) {
    out << this->factorsLifted[i].toString(&this->format);
    if (i != this->factorsLifted.size - 1) {
      out << ", ";
    }
  }
  out << "\\)";
  return out.str();
}

void PolynomialFactorizationFiniteFields::henselLiftOnce(
  const LargeIntegerUnsigned& oldModulus,
  int stepCount,
  std::stringstream* comments
) {
  STACK_TRACE("PolynomialFactorizationFiniteFields::henselLiftOnce");
  std::stringstream explainer;
  ElementZmodP::convertModuloIntegerAfterScalingToIntegral(
    this->current,
    this->desiredLiftWithoutRescaling,
    this->modulusHenselLift
  );
  this->desiredLift = this->desiredLiftWithoutRescaling;
  this->scaleProductLift =
  this->desiredLift.scaleNormalizeLeadingMonomial(
    &MonomialPolynomial::orderDefault()
  );
  if (comments != nullptr) {
    explainer << "\\(\\begin{array}{rcllllll}";
    bool rescalingNeeded =
    !this->desiredLiftWithoutRescaling.isEqualTo(this->desiredLift);
    if (rescalingNeeded) {
      explainer
      << "h_{"
      << stepCount
      << "}&\\equiv&"
      << this->desiredLiftWithoutRescaling.toString(&this->format)
      << "&&&&"
      << "\\mod "
      << this->modulusHenselLift
      << "\\\\";
    }
    explainer << "f_{" << stepCount << "}";
    if (rescalingNeeded) {
      explainer
      << "=\\frac{h_{"
      << stepCount
      << "}}{"
      << this->desiredLiftWithoutRescaling.getLeadingCoefficient(nullptr).
      toString(&this->format)
      << "}";
    }
    explainer
    << "&\\equiv&"
    << this->desiredLift.toString(&this->format)
    << "&&&&"
    << "\\mod "
    << this->modulusHenselLift
    << "\\\\";
  }
  for (int i = 0; i < this->factorsLifted.size; i ++) {
    ElementZmodP::convertLiftPolynomialModular(
      this->factorsLifted[i],
      this->factorsLifted[i],
      this->modulusHenselLift
    );
  }
  Polynomial<ElementZmodP> newProduct;
  ElementZmodP one;
  one.makeOne(this->modulusHenselLift);
  newProduct.makeConstant(one);
  for (int i = 0; i < this->factorsLifted.size; i ++) {
    newProduct *= this->factorsLifted[i];
  }
  if (comments != nullptr) {
    *comments
    << "<br>Product without correction: \\("
    << newProduct.toString(&this->format)
    << "\\mod "
    << this->modulusHenselLift
    << "\\)";
  }
  Polynomial<ElementZmodP> correction = this->desiredLift;
  correction -= newProduct;
  Polynomial<ElementZmodP> correctionOverPrime;
  for (int i = 0; i < correction.size(); i ++) {
    ElementZmodP rescaledCoefficient = this->oneModular.zero();
    rescaledCoefficient.value = correction.coefficients[i].value / oldModulus;
    correctionOverPrime.addMonomial(
      correction.monomials[i], rescaledCoefficient
    );
  }
  if (comments != nullptr) {
    explainer
    << "g_{"
    << stepCount
    << "}&\\equiv&f_{"
    << stepCount
    << "}-"
    << "f_{"
    << stepCount - 1
    << "}&="
    << this->desiredLift.toString(&this->format)
    << "-";
    for (int i = 0; i < this->factorsLifted.size; i ++) {
      explainer
      << "\\left("
      << this->factorsLifted[i].toString(&this->format)
      << "\\right)";
    }
    explainer
    << "&\\equiv&"
    << correction.toString(&this->format)
    << "&\\mod "
    << this->modulusHenselLift
    << "\\\\";
    explainer
    << "\\frac{g_{"
    << stepCount
    << "}}{"
    << oldModulus
    << "}"
    << "&\\equiv&"
    << correctionOverPrime.toString(&this->format)
    << "&&&&\\mod "
    << this->oneModular.modulus
    << "\\\\\n";
;
    *comments
    << "Modulus: "
    << this->modulusHenselLift
    << ". "
    << "Lifted factors without correction: "
    << this->toLatexLiftedFactors();
    *comments
    << "<br>Desired lift: \\("
    << desiredLift.toString(&this->format)
    << " \\mod "
    << this->modulusHenselLift
    << " \\)";
    *comments
    << "<br>Desired lift - non-corrected: \\("
    << correction.toString(&this->format)
    << "\\mod "
    << this->modulusHenselLift
    << "\\)";
    *comments
    << "<br>(Desired lift - non-corrected)/"
    << oldModulus
    << ": \\("
    << correctionOverPrime.toString(&this->format)
    << "\\mod "
    << this->oneModular.modulus
    << "\\)";
  }
  Vector<ElementZmodP> coefficientsCorrection;
  coefficientsCorrection.makeZero(
    this->sylvesterMatrixInverted.numberOfColumns,
    this->oneModular.zero()
  );
  for (int i = 0; i < correctionOverPrime.size(); i ++) {
    int index =
    this->sylvesterMatrixInverted.numberOfColumns -
    correctionOverPrime.monomials[i].totalDegreeInt() -
    1;
    coefficientsCorrection[index] = correctionOverPrime.coefficients[i];
  }
  Vector<ElementZmodP> solution;
  this->sylvesterMatrixInverted.actOnVectorColumn(
    coefficientsCorrection, solution, this->oneModular.zero()
  );
  if (comments != nullptr) {
    Matrix<ElementZmodP> transposedCorrection;
    Matrix<ElementZmodP> transposedSolution;
    transposedCorrection.assignVectorColumn(coefficientsCorrection);
    transposedSolution.assignVectorColumn(solution);
    *comments
    << "<br>Lift coefficients: \\("
    << this->sylvesterMatrixInverted.toStringLatex(&this->format)
    << transposedCorrection.toStringLatex(&this->format)
    << "="
    << transposedSolution.toStringLatex(&this->format)
    << "\\)";
    explainer
    << "S^{-1}\\cdot \\frac{g_{"
    << stepCount
    << "}}{"
    << oldModulus
    << "}&=&"
    << this->sylvesterMatrixInverted.toStringLatex(&this->format)
    << transposedCorrection.toStringLatex(&this->format)
    << "&&\\equiv&"
    << transposedSolution.toStringLatex(&this->format)
    << "&\\mod "
    << this->oneModular.modulus
    << "\\\\\n";
  }
  int offset = 0;
  List<Polynomial<ElementZmodP> > allCorrections;
  List<Polynomial<ElementZmodP> > oldFactors;
  for (int i = 0; i < this->factorsLifted.size; i ++) {
    if (comments != nullptr) {
      Polynomial<ElementZmodP> zero;
      allCorrections.addOnTop(zero);
      oldFactors.addOnTop(this->factorsLifted[i]);
    }
    int summandsCurrentFactor = this->factorsLifted[i].totalDegreeInt();
    for (int j = 0; j < summandsCurrentFactor; j ++) {
      ElementZmodP incoming;
      incoming.modulus = this->modulusHenselLift;
      int index = offset + summandsCurrentFactor - 1 - j;
      incoming.value = solution[index].value * oldModulus;
      MonomialPolynomial monomial(0, j);
      this->factorsLifted[i].addMonomial(monomial, incoming);
      if (comments != nullptr) {
        allCorrections[i].addMonomial(monomial, solution[index]);
      }
    }
    offset += summandsCurrentFactor;
  }
  if (comments != nullptr) {
    *comments << "<br>Lifted factors: \\(\\begin{array}{rclll}";
    for (int i = 0; i < allCorrections.size; i ++) {
      *comments
      << this->factorsLifted[i].toString(&this->format)
      << "&=&\\left("
      << oldFactors[i].toString(&this->format)
      << "\\right) &+ "
      << oldModulus
      << "\\cdot \\left("
      << allCorrections[i].toString(&this->format)
      << "\\right)"
      << "&"
      << "\\mod "
      << this->modulusHenselLift
      << "\\\\";
      explainer
      << this->factorNames[i]
      << "_{"
      << stepCount
      << "}"
      << "&\\equiv&"
      << oldFactors[i].toString(&this->format)
      << "&+"
      << oldModulus
      << "\\cdot\\left("
      << allCorrections[i].toString(&this->format)
      << "\\right)&\\equiv&"
      << this->factorsLifted[i].toString(&this->format)
      << "&\\mod "
      << this->modulusHenselLift
      << "\\\\\n";
    }
    *comments << "\\end{array}\\)";
  }
  Polynomial<ElementZmodP> productLift;
  productLift.makeConstant(one);
  for (int i = 0; i < this->factorsLifted.size; i ++) {
    productLift *= this->factorsLifted[i];
  }
  if (comments != nullptr) {
    *comments
    << "<br>Lifted product, corrected, before rescaling: "
    << productLift.toString(&this->format);
  }
  productLift /= this->scaleProductLift;
  if (comments != nullptr) {
    explainer << "\\end{array} \\)";
    *comments
    << "<br>Lifted product, corrected: "
    << productLift.toString(&this->format);
    Polynomial<Rational> negativesIncluded;
    ElementZmodP::convertPolynomialModularToPolynomialIntegral(
      productLift, negativesIncluded, true
    );
    *comments
    << "<br>Lifted product, over Z: "
    << negativesIncluded.toString(&this->format)
    << "<br>"
    << explainer.str()
    << "<hr>";
    this->latexExplainers.addOnTop(explainer.str());
  }
}

bool PolynomialFactorizationFiniteFields::tryFactor(
  SelectionFixedRank& selection
) {
  STACK_TRACE("PolynomialFactorizationFiniteFields::tryFactor");
  Polynomial<ElementZmodP> product;
  ElementZmodP start;
  start.makeOne(this->modulusHenselLift);
  start /= this->scaleProductLift;
  product.makeConstant(start);
  for (int i = 0; i < selection.selection.cardinalitySelection; i ++) {
    product *= this->factorsLifted[selection.selection.elements[i]];
  }
  Polynomial<Rational> candidate;
  ElementZmodP::convertPolynomialModularToPolynomialIntegral(
    product, candidate, true
  );
  if (!this->output->accountReducedFactor(candidate, false)) {
    return false;
  }
  selection.selection.invertSelection();
  List<Polynomial<ElementZmodP> > factorsLiftedTrimmed;
  for (int i = 0; i < selection.selection.cardinalitySelection; i ++) {
    factorsLiftedTrimmed.addOnTop(
      this->factorsLifted[selection.selection.elements[i]]
    );
  }
  this->factorsLifted = factorsLiftedTrimmed;
  return true;
}

bool PolynomialFactorizationFiniteFields::factorizationFromHenselLift(
  std::stringstream* comments, std::stringstream* commentsOnFailure
) {
  STACK_TRACE(
    "PolynomialFactorizationFiniteFields::factorizationFromHenselLift"
  );
  this->maximumFactorsLiftedTries = 1000;
  while (this->factorsLifted.size > 0) {
    if (
      !this->factorizationFromHenselLiftOnce(comments, commentsOnFailure)
    ) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Failed to carry out hensel lift. ";
      }
      return false;
    }
  }
  return true;
}

bool PolynomialFactorizationFiniteFields::factorizationFromHenselLiftOnce(
  std::stringstream* comments, std::stringstream* commentsOnFailure
) {
  STACK_TRACE(
    "PolynomialFactorizationFiniteFields::factorizationFromHenselLiftOnce"
  );
  (void) comments;
  SelectionFixedRank selection;
  // global.comments << "And the factors are: " <<
  // this->factorsLifted.toStringCommaDelimited();
  int maximumSubsetsToTry = this->factorsLifted.size / 2;
  LargeInteger twoPowerN = 2;
  twoPowerN.raiseToPower(maximumSubsetsToTry);
  for (int i = 1; i <= maximumSubsetsToTry; i ++) {
    selection.setNumberOfItemsAndDesiredSubsetSize(
      i, this->factorsLifted.size
    );
    do {
      this->factorsLiftedTries ++;
      if (this->factorsLiftedTries > this->maximumFactorsLiftedTries) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "The maximum allowed Hensel lift combinations is: "
          << this->maximumFactorsLiftedTries
          << ". Total possibilities: "
          << twoPowerN;
        }
        return false;
      }
      if (this->tryFactor(selection)) {
        return true;
      }
    } while (selection.incrementReturnFalseIfPastLast());
  }
  selection.setNumberOfItemsAndDesiredSubsetSize(
    this->factorsLifted.size, this->factorsLifted.size
  );
  if (!this->tryFactor(selection)) {
    // The product of all lifts should be equal to the
    // original polynomial, which is not the case.
    // We've made an algorithmic or programming error.
    global.fatal
    << "This code section should not be reachable. "
    << global.fatal;
  }
  return true;
}

PolynomialUnivariateModular::PolynomialUnivariateModular():
modulusData(nullptr) {}

PolynomialUnivariateModular::PolynomialUnivariateModular(
  const PolynomialUnivariateModular& other
) {
  this->modulusData = nullptr;
  *this = other;
}

PolynomialUnivariateModular::PolynomialUnivariateModular(
  IntegerModulusSmall* inputModulus
):
modulusData(inputModulus) {}

bool PolynomialUnivariateModular::greatestCommonDivisor(
  const PolynomialUnivariateModular& left,
  const PolynomialUnivariateModular& right,
  PolynomialUnivariateModular& output,
  const ElementZmodP& unused,
  std::stringstream* commentsOnFailure
) {
  (void) unused;
  return
  PolynomialUnivariateModular::greatestCommonDivisor(
    left, right, output, commentsOnFailure
  );
}

bool PolynomialUnivariateModular::greatestCommonDivisor(
  const PolynomialUnivariateModular& left,
  const PolynomialUnivariateModular& right,
  PolynomialUnivariateModular& output,
  std::stringstream* commentsOnFailure
) {
  if (&output == &left || &output == &right) {
    PolynomialUnivariateModular leftCopy = left;
    PolynomialUnivariateModular rightCopy = right;
    return
    PolynomialUnivariateModular::greatestCommonDivisor(
      leftCopy, rightCopy, output, commentsOnFailure
    );
  }
  output = left;
  PolynomialUnivariateModular divisor = right;
  output.rescaleSoLeadingCoefficientIsOne();
  divisor.rescaleSoLeadingCoefficientIsOne();
  PolynomialUnivariateModular remainder;
  PolynomialUnivariateModular quotient;
  while (!divisor.isEqualToZero()) {
    output.divideBy(divisor, quotient, remainder);
    output = divisor;
    divisor = remainder;
  }
  return true;
}

void PolynomialUnivariateModular::ensureCoefficientLength(int desiredLength) {
  int oldLength = this->coefficients.size;
  if (desiredLength > oldLength) {
    this->coefficients.setSize(desiredLength);
  }
  for (int i = oldLength; i < this->coefficients.size; i ++) {
    this->coefficients[i] = 0;
  }
}

void PolynomialUnivariateModular::trimTrailingZeroes() {
  for (int i = this->coefficients.size - 1; i >= 0; i --) {
    if (this->coefficients[i] != 0) {
      return;
    }
    this->coefficients.size --;
  }
}

void PolynomialUnivariateModular::addAnotherTimesConstant(
  const PolynomialUnivariateModular& other, int coefficient
) {
  coefficient = this->reduceIntModP(coefficient);
  this->ensureCoefficientLength(other.coefficients.size);
  int modulus = this->getModulus();
  for (int i = 0; i < other.coefficients.size; i ++) {
    this->coefficients[i] += other.coefficients[i] * coefficient;
    this->coefficients[i] %= modulus;
  }
  this->trimTrailingZeroes();
}

void PolynomialUnivariateModular::addTerm(int coefficient, int termPower) {
  this->ensureCoefficientLength(termPower + 1);
  this->coefficients[termPower] += coefficient;
  int modulus = this->getModulus();
  this->coefficients[termPower] %= modulus;
  if (this->coefficients[termPower] < 0) {
    this->coefficients[termPower] += modulus;
  }
  this->trimTrailingZeroes();
}

void PolynomialUnivariateModular::addAnotherTimesTerm(
  const PolynomialUnivariateModular& other, int coefficient, int termPower
) {
  coefficient = this->reduceIntModP(coefficient);
  this->ensureCoefficientLength(other.coefficients.size + termPower);
  int modulus = this->getModulus();
  for (int i = 0; i < other.coefficients.size; i ++) {
    this->coefficients[i + termPower] = (
      this->coefficients[i + termPower] + other.coefficients[i] * coefficient
    ) %
    modulus;
  }
  this->trimTrailingZeroes();
}

int PolynomialUnivariateModular::getLeadingCoefficient() const {
  if (this->coefficients.size == 0) {
    return 0;
  }
  return *this->coefficients.lastObject();
}

void PolynomialUnivariateModular::divideBy(
  const PolynomialUnivariateModular& divisor,
  PolynomialUnivariateModular& outputQuotient,
  PolynomialUnivariateModular& outputRemainder
) const {
  if (
    this == &outputQuotient ||
    &divisor == &outputQuotient ||
    this == &outputRemainder ||
    &divisor == &outputRemainder
  ) {
    PolynomialUnivariateModular thisCopy = *this;
    PolynomialUnivariateModular divisorCopy = divisor;
    thisCopy.divideBy(divisorCopy, outputQuotient, outputRemainder);
    return;
  }
  int divisorInvertedLeadingCoefficient =
  this->modulusData->inverses[divisor.getLeadingCoefficient()];
  outputRemainder = *this;
  outputQuotient.makeZero(this->modulusData);
  int modulus = this->getModulus();
  if (divisor.isEqualToZero()) {
    global.fatal << "Dividing by zero is not allowed. " << global.fatal;
  }
  while (
    outputRemainder.coefficients.size >= divisor.coefficients.size
  ) {
    int coefficient = (
      outputRemainder.getLeadingCoefficient() *
      divisorInvertedLeadingCoefficient
    ) %
    modulus;
    int termPower =
    outputRemainder.coefficients.size - divisor.coefficients.size;
    outputQuotient.addTerm(coefficient, termPower);
    int coefficientNegated = modulus - coefficient;
    outputRemainder.addAnotherTimesTerm(
      divisor, coefficientNegated, termPower
    );
  }
}

bool PolynomialUnivariateModular::makeFromPolynomialAndModulus(
  IntegerModulusSmall* modulus, const Polynomial<Rational>& input
) {
  this->makeZero(modulus);
  for (int i = 0; i < input.size(); i ++) {
    ElementZmodP element;
    element.modulus = modulus->modulus;
    if (!element.assignRational(input.coefficients[i])) {
      return false;
    }
    int coefficient = 0;
    element.value.isIntegerFittingInInt(&coefficient);
    this->addTerm(coefficient, input.monomials[i].totalDegreeInt());
  }
  return true;
}

void PolynomialUnivariateModular::makeFromPolynomialAndModulusNoFailure(
  IntegerModulusSmall* modulus, const Polynomial<Rational>& input
) {
  bool mustBeTrue = this->makeFromPolynomialAndModulus(modulus, input);
  if (!mustBeTrue) {
    global.fatal
    << "Failed to create modular polynomial form "
    << input.toString()
    << " and modulus: "
    << modulus->modulus
    << global.fatal;
  }
}

int PolynomialUnivariateModular::reduceModP(const LargeInteger& input) {
  LargeInteger reduced;
  reduced = input;
  reduced %= this->getModulus();
  int result = 0;
  reduced.isIntegerFittingInInt(&result);
  return result;
}

void PolynomialUnivariateModular::makeZero(IntegerModulusSmall* inputModulus) {
  this->modulusData = inputModulus;
  this->coefficients.clear();
}

void PolynomialUnivariateModular::operator*=(int other) {
  if (other == 0) {
    this->makeZero(this->modulusData);
    return;
  }
  int modulus = this->getModulus();
  other = this->reduceIntModP(other);
  for (int i = 0; i < this->coefficients.size; i ++) {
    this->coefficients[i] *= other;
    this->coefficients[i] %= modulus;
  }
}

void PolynomialUnivariateModular::rescaleSoLeadingCoefficientIsOne() {
  if (this->coefficients.size == 0) {
    return;
  }
  int invertedLeadingCoefficient =
  this->modulusData->inverses[*this->coefficients.lastObject()];
  *this *= invertedLeadingCoefficient;
}

bool PolynomialUnivariateModular::checkInitialization() const {
  if (this->modulusData == nullptr) {
    global.fatal << "Uninitialized modular polynomial. " << global.fatal;
  }
  return true;
}

void PolynomialUnivariateModular::toPolynomialNonDense(
  Polynomial<ElementZmodP>& output
) const {
  this->checkInitialization();
  output.makeZero();
  MonomialPolynomial monomial;
  ElementZmodP coefficientModP;
  for (int i = 0; i < this->coefficients.size; i ++) {
    monomial.setVariable(0, i);
    coefficientModP.makeFrom(
      this->modulusData->modulus, this->coefficients[i]
    );
    output.addMonomial(monomial, coefficientModP);
  }
}

std::string PolynomialUnivariateModular::toString(FormatExpressions* format)
const {
  if (this->modulusData == nullptr) {
    return "[uninitialized]";
  }
  Polynomial<LargeInteger> converter;
  MonomialPolynomial monomial;
  MemorySaving<FormatExpressions> backupFormat;
  if (format == nullptr) {
    format = &backupFormat.getElement();
    format->polynomialAlphabet.addOnTop("x");
  }
  for (int i = 0; i < this->coefficients.size; i ++) {
    LargeInteger coefficient = this->coefficients[i];
    monomial.makeEi(0, i);
    converter.addMonomial(monomial, coefficient);
  }
  std::stringstream out;
  out
  << converter.toString(format)
  << " \\mod "
  << this->modulusData->modulus
  << "";
  return out.str();
}

void PolynomialUnivariateModular::operator=(
  const PolynomialUnivariateModular& other
) {
  other.checkInitialization();
  this->modulusData = other.modulusData;
  this->coefficients = other.coefficients;
}

void PolynomialUnivariateModular::operator=(const ElementZmodP& other) {
  if (other.modulus != this->getModulus()) {
    global.fatal
    << "Attempt to assign "
    << other.toString()
    << " to polynomial with modulus: "
    << this->getModulus()
    << global.fatal;
  }
  this->coefficients.clear();
  LargeInteger otherReduced = other.value;
  otherReduced %= this->getModulus();
  int incoming = 0;
  otherReduced.isIntegerFittingInInt(&incoming);
  this->coefficients.addOnTop(incoming);
  this->trimTrailingZeroes();
}

void PolynomialUnivariateModular::operator=(
  const Polynomial<ElementZmodP>& other
) {
  STACK_TRACE("PolynomialUnivariateModular::operator=");
  this->checkInitialization();
  this->makeZero(this->modulusData);
  if (other.minimalNumberOfVariables() > 1) {
    global.fatal
    << "Attempt to assign "
    << other.toString()
    << " to polynomial with one variable. "
    << global.fatal;
  }
  for (int i = 0; i < other.size(); i ++) {
    LargeInteger otherReduced = other.coefficients[i].value;
    otherReduced %= this->getModulus();
    int incoming = 0;
    otherReduced.isIntegerFittingInInt(&incoming);
    this->addTerm(incoming, other.monomials[i].totalDegreeInt());
  }
}

void PolynomialUnivariateModular::operator-=(
  const PolynomialUnivariateModular& other
) {
  this->addAnotherTimesConstant(other, - 1);
}

void PolynomialUnivariateModular::derivative(
  PolynomialUnivariateModular& output
) const {
  if (this == &output) {
    PolynomialUnivariateModular thisCopy = *this;
    thisCopy.derivative(output);
    return;
  }
  output.makeZero(this->modulusData);
  for (int i = this->coefficients.size - 1; i > 0; i --) {
    output.addTerm(this->coefficients[i] * i, i - 1);
  }
}

bool PolynomialUnivariateModular::isEqualToZero() const {
  return this->coefficients.size == 0;
}

int PolynomialUnivariateModular::totalDegreeInt() const {
  return this->coefficients.size - 1;
}

bool PolynomialUnivariateModular::isConstant() const {
  return this->coefficients.size <= 1;
}

ElementZmodP PolynomialUnivariateModular::evaluate(
  const ElementZmodP& input, const ElementZmodP& unused
) const {
  (void) unused;
  int inputSmall = 0;
  input.value.isIntegerFittingInInt(&inputSmall);
  int result = 0;
  for (int i = this->coefficients.size - 1; i >= 0; i --) {
    result *= inputSmall;
    result += this->coefficients[i];
    result %= this->modulusData->modulus;
  }
  ElementZmodP resultModP;
  resultModP.makeFrom(this->modulusData->modulus, result);
  return resultModP;
}

void PolynomialUnivariateModularAsModulus::operator=(
  const PolynomialUnivariateModular& inputModulus
) {
  STACK_TRACE("PolynomialUnivariateModularAsModulus::operator=");
  this->modulus = inputModulus;
  this->computeFromModulus();
}

void PolynomialUnivariateModularAsModulus::computeFromModulus() {
  STACK_TRACE("PolynomialUnivariateModularAsModulus::computeFromModulus");
  this->modulus.checkInitialization();
  int totalModulusDegree = this->modulus.totalDegreeInt();
  if (totalModulusDegree < 0) {
    global.fatal << "Zero modulus not allowed. " << global.fatal;
  }
  this->imagesPowersOfX.setSize(totalModulusDegree);
  List<int> startingReduction;
  startingReduction.initializeFillInObject(totalModulusDegree, 0);
  *startingReduction.lastObject() = 1;
  List<int>* previousReduction = &startingReduction;
  PolynomialUnivariateModular imageLargestXPower;
  imageLargestXPower = this->modulus;
  imageLargestXPower.rescaleSoLeadingCoefficientIsOne();
  imageLargestXPower.addTerm(- 1, totalModulusDegree);
  imageLargestXPower *= - 1;
  imageLargestXPower.ensureCoefficientLength(totalModulusDegree);
  this->imageXToTheNth = imageLargestXPower.coefficients;
  for (int i = 0; i < totalModulusDegree; i ++) {
    List<int>& reduction = this->imagesPowersOfX[i];
    this->computeOneReductionRow(*previousReduction, reduction);
    previousReduction = &reduction;
  }
}

std::string PolynomialUnivariateModularAsModulus::toStringFull(
  FormatExpressions* format
) const {
  return this->modulus.toString(format) + this->toStringImagesOfX();
}

std::string PolynomialUnivariateModularAsModulus::toString(
  FormatExpressions* format
) const {
  return this->modulus.toString(format);
}

std::string PolynomialUnivariateModularAsModulus::toStringImagesOfX() const {
  std::stringstream out;
  out << this->imageXToTheNth.toStringCommaDelimited();
  int degree = this->imageXToTheNth.size;
  for (int i = 0; i < this->imagesPowersOfX.size; i ++) {
    out
    << "\nx^"
    << degree + i
    << " -> "
    << this->imagesPowersOfX[i].toStringCommaDelimited();
  }
  return out.str();
}

bool PolynomialUnivariateModularAsModulus::checkInitialization() const {
  this->modulus.checkInitialization();
  return true;
}

void PolynomialUnivariateModularAsModulus::computeOneReductionRow(
  const List<int>& previous, List<int>& output
) {
  STACK_TRACE("PolynomialUnivariateModularAsModulus::computeOneReductionRow");
  int totalModulusDegree = this->modulus.totalDegreeInt();
  output.setSize(totalModulusDegree);
  int modulusInteger = this->modulus.modulusData->modulus;
  int lastCoefficient = *previous.lastObject();
  output[0] = this->imageXToTheNth[0] * lastCoefficient;
  output[0] %= modulusInteger;
  for (int i = 1; i < output.size; i ++) {
    output[i] = previous[i - 1] + this->imageXToTheNth[i] * lastCoefficient;
    output[i] %= modulusInteger;
  }
}

void PolynomialUnivariateModular::operator*=(
  const PolynomialUnivariateModular& other
) {
  this->checkInitialization();
  PolynomialUnivariateModular output;
  output.makeZero(this->modulusData);
  output.ensureCoefficientLength(
    other.coefficients.size + this->coefficients.size - 1
  );
  int modulus = this->getModulus();
  for (int i = 0; i < this->coefficients.size; i ++) {
    int thisCoefficient = this->coefficients[i];
    for (int j = 0; j < other.coefficients.size; j ++) {
      output.coefficients[i + j] = (
        output.coefficients[i + j] + thisCoefficient * other.coefficients[j]
      ) %
      modulus;
    }
  }
  *this = output;
}

void PolynomialModuloPolynomialModuloInteger::makeFromModulusAndValue(
  PolynomialUnivariateModularAsModulus* inputModulus,
  const Polynomial<ElementZmodP>& inputValue
) {
  STACK_TRACE(
    "PolynomialModuloPolynomialModuloInteger::makeFromModulusAndValue"
  );
  inputModulus->checkInitialization();
  this->modulusContainer = inputModulus;
  this->value.modulusData = this->modulusContainer->modulus.modulusData;
  this->value = inputValue;
  this->reduce();
  this->checkInitialization();
}

void PolynomialModuloPolynomialModuloInteger::operator+=(
  const ElementZmodP& other
) {
  this->value.addTerm(this->value.reduceModP(other.value), 0);
}

void PolynomialModuloPolynomialModuloInteger::operator-=(
  const ElementZmodP& other
) {
  LargeInteger negative = other.value;
  negative.negate();
  this->value.addTerm(this->value.reduceModP(negative), 0);
}

void PolynomialModuloPolynomialModuloInteger::operator-=(
  const PolynomialModuloPolynomialModuloInteger& other
) {
  this->value -= other.value;
  this->reduce();
}

void PolynomialModuloPolynomialModuloInteger::operator*=(
  const PolynomialModuloPolynomialModuloInteger& other
) {
  this->checkInitialization();
  this->value *= other.value;
  this->reduce();
}

void PolynomialModuloPolynomialModuloInteger::reduce() {
  int totalModulusDegree = this->modulus()->modulus.totalDegreeInt();
  if (this->value.totalDegreeInt() >= totalModulusDegree * 2) {
    global.fatal
    << "Reduction of polynomial of too-large degree: "
    << this->value.toString()
    << " mod: "
    << this->modulus()->modulus.toString()
    << global.fatal;
  }
  List<List<int> >& reductions = this->modulus()->imagesPowersOfX;
  for (
    int i = totalModulusDegree; i < this->value.coefficients.size; i ++
  ) {
    int coefficient = this->value.coefficients[i];
    this->value.coefficients[i] = 0;
    List<int>& currentReduction = reductions[i - totalModulusDegree];
    for (int j = 0; j < totalModulusDegree; j ++) {
      this->value.addTerm(coefficient * currentReduction[j], j);
    }
  }
  this->value.trimTrailingZeroes();
}

bool PolynomialModuloPolynomialModuloInteger::isEqualToZero() const {
  return this->value.isEqualToZero();
}

bool PolynomialModuloPolynomialModuloInteger::checkInitialization() const {
  if (this->modulusContainer == nullptr) {
    global.fatal
    << "Uninitialized PolynomialModuloPolynomialModuloInteger. "
    << global.fatal;
  }
  this->modulusContainer->checkInitialization();
  this->value.checkInitialization();
  return true;
}

std::string PolynomialModuloPolynomialModuloInteger::toString(
  FormatExpressions* format
) const {
  return this->toStringFull(format);
}

std::string PolynomialModuloPolynomialModuloInteger::toStringFull(
  FormatExpressions* format
) const {
  std::stringstream out;
  out
  << this->value.toString(format)
  << " \\mod ("
  << this->modulusContainer->toString(format)
  << ")";
  return out.str();
}

void IntegerModulusSmall::initializeModulusData(int inputModulus) {
  this->modulus = inputModulus;
  this->inverses.initializeFillInObject(inputModulus, 0);
  for (int i = 1; i < inputModulus; i ++) {
    if (
      !MathRoutines::invertXModN(i, inputModulus, this->inverses[i])
    ) {
      global.fatal
      << "Modulus "
      << inputModulus
      << " is not prime. "
      << global.fatal;
    }
  }
}
