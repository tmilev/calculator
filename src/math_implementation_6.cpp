// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "math_general_implementation.h"
#include "math_extra_polynomial_factorization.h"

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
  List<List<int> > primeFactorsMults;
  Vector<Rational> valuesAtPoints, valuesAtRemainingPoints;
  allPointsOfEvaluation.setSize(degree + 1);
  primeFactorsAtPoints.setSize(degreeLeft + 1);
  primeFactorsMults.setSize(degreeLeft + 1);
  allPointsOfEvaluation[0] = 0;
  for (int i = 1; i < allPointsOfEvaluation.size; i ++) {
    allPointsOfEvaluation[i] = (i % 2 == 1) ? i / 2 + 1 : - (i / 2);
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
    if (!tempLI.value.factor(
      primeFactorsAtPoints[i], primeFactorsMults[i], 0, 3, commentsOnFailure
    )) {
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
    valuesAtRemainingPoints.makeZero(valuesAtPoints.size);
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
      currentPointContribution = valuesAtPoints[j] / valuesAtRemainingPoints[j];
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
      pointsOfInterpolationLeft,
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

bool PolynomialFactorizationFiniteFields::factor(
  std::stringstream* comments,
  std::stringstream* commentsOnFailure
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
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Total degree is not small integer.";
    }
    return false;
  }
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
  const List<unsigned int>& primeFactors = LargeIntegerUnsigned::allPrimesSmallerThan15Bits();
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
  return this->output->accountReducedFactor(this->current, false);
}

bool PolynomialFactorizationFiniteFields::oneFactorFromModularization(
  std::stringstream* comments, std::stringstream* commentsOnFailure
) {
  PolynomialFactorizationUnivariate<ElementZmodP> factorizationModular;
  PolynomialFactorizationCantorZassenhaus<PolynomialModuloPolynomialModuloInteger> algorithm;

  if (!factorizationModular.factor(
    this->modularization,
    algorithm,
    comments,
    commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to factor modular polynomial: "
      << this->modularization.toString();
    }
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
  this->maximumFactorsLiftedTries = 1000;
  while (this->factorsLifted.size > 0) {
    if (!this->factorizationFromHenselLiftOnce(comments, commentsOnFailure)) {
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
  MacroRegisterFunctionWithName("PolynomialFactorizationFiniteFields::factorizationFromHenselLiftOnce");
  (void) comments;
  SelectionFixedRank selection;
  // global.comments << "And the factors are: " << this->factorsLifted.toStringCommaDelimited();
  int maximumSubsetsToTry = this->factorsLifted.size / 2;
  LargeInteger twoPowerN = 2;
  twoPowerN.raiseToPower(maximumSubsetsToTry);
  for (int i = 1; i <= maximumSubsetsToTry; i ++) {
    selection.setNumberOfItemsAndDesiredSubsetSize(i, this->factorsLifted.size);
    do {
      this->factorsLiftedTries ++;
      if (this->factorsLiftedTries > this->maximumFactorsLiftedTries) {
        if (commentsOnFailure != nullptr) {
          *commentsOnFailure
          << "The maximum allowed Hensel lift combinations is: "
          << this->maximumFactorsLiftedTries << ". Total possibilities: " << twoPowerN;
        }
        return false;
      }
      if (this->tryFactor(selection)) {
        return true;
      }
    } while (selection.incrementReturnFalseIfPastLast());
  }
  selection.setNumberOfItemsAndDesiredSubsetSize(this->factorsLifted.size, this->factorsLifted.size);
  if (!this->tryFactor(selection)) {
    // The product of all lifts should be equal to the
    // original polynomial, which is not the case.
    // We've made an algorithmic or programming error.
    global.fatal << "This code section should not be reachable. " << global.fatal;
  }
  return true;
}

PolynomialUnivariateModular::PolynomialUnivariateModular() : modulus(nullptr) {
}

PolynomialUnivariateModular::PolynomialUnivariateModular(
  IntegerModulusSmall* inputModulus
) : modulus(inputModulus) {
}
