#ifndef header_math_general_polynomial_computations_advanced_implementation_ALREADY_INCLUDED
#define header_math_general_polynomial_computations_advanced_implementation_ALREADY_INCLUDED

#include "math_extra_algebraic_numbers.h"
#include "math_general_implementation.h"
#include "math_general_polynomial_computations_basic_implementation.h" // IWYU pragma: keep: breaks g++ -02 optimization build.

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::
wrapUpGroebnerOnExceedingComputationLimit(
  List<Polynomial<Coefficient> >& output
) {
  this->getBasis(output);
  for (int i = 0; i < this->basisCandidates.size; i ++) {
    output.addOnTop(this->basisCandidates[i]);
  }
  return false;
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::wrapUpOnGroebnerBasisSuccess(
  List<Polynomial<Coefficient> >& output
) {
  this->getBasis(output);
  return true;
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::getBasis(
  List<Polynomial<Coefficient> >& output
) {
  output.setSize(this->basis.size);
  for (int i = 0; i < this->basis.size; i ++) {
    output[i] = this->basis[i].element;
  }
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::transformToReducedBasis(
  List<Polynomial<Coefficient> >& inputOutput
) {
  STACK_TRACE("GroebnerBasisComputation::transformToReducedBasis");
  this->initializeForGroebnerComputation();
  this->basisCandidates = inputOutput;
  ProgressReport report;
  if (this->flagDoProgressReport) {
    std::stringstream reportStream;
    reportStream
    << "Transforming to reduced (NON-Groebner) basis a system of "
    << this->basisCandidates.size
    << " elements (at the start). ";
    report.report(reportStream.str());
  }
  this->addAndReducePolynomials();
  if (this->maximumPolynomialDivisions > 0) {
    if (this->numberPolynomialDivisions > this->maximumPolynomialDivisions) {
      return this->wrapUpGroebnerOnExceedingComputationLimit(inputOutput);
    }
  }
  return this->wrapUpOnGroebnerBasisSuccess(inputOutput);
}

template <class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::
toStringStatusGroebnerBasisTransformation() {
  std::stringstream out;
  out
  << "Transforming to Groebner basis a system of "
  << this->basisCandidates.size
  << " elements: "
  << this->basisCandidates.toStringCommaDelimited(&this->format);
  return out.str();
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::
generateOneSymmetricDifferenceCandidate(
  GroebnerBasisComputation::BasisElement& left,
  GroebnerBasisComputation::BasisElement& right
) {
  int numberOfVariables =
  MathRoutines::maximum(
    left.leadingMonomial.minimalNumberOfVariables(),
    right.leadingMonomial.minimalNumberOfVariables()
  );
  MonomialPolynomial leftShift;
  MonomialPolynomial rightShift;
  for (int k = 0; k < numberOfVariables; k ++) {
    if (left.leadingMonomial(k) > right.leadingMonomial(k)) {
      rightShift.setVariable(
        k, left.leadingMonomial(k) - right.leadingMonomial(k)
      );
      leftShift.setVariable(k, 0);
    } else {
      leftShift.setVariable(
        k, right.leadingMonomial(k) - left.leadingMonomial(k)
      );
      rightShift.setVariable(k, 0);
    }
  }
  Polynomial<Coefficient> symmetricDifference = left.element;
  symmetricDifference.multiplyBy(leftShift, right.leadingCoefficient);
  Polynomial<Coefficient> rightScaled = right.element;
  rightScaled.multiplyBy(rightShift, left.leadingCoefficient);
  symmetricDifference -= rightScaled;
  this->basisCandidates.addOnTop(symmetricDifference);
  this->numberPolynomialDivisions ++;
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::
generateSymmetricDifferenceCandidates() {
  STACK_TRACE(
    "GroebnerBasisComputation::generateSymmetricDifferenceCandidates"
  );
  ProgressReport reportProgress("Groebner basis report");
  this->numberOfSymmetricDifferenceRounds ++;
  for (int i = 0; i < this->basis.size; i ++) {
    for (
      int j = i + 1; j < this->basis.size && i < this->basis.size; j ++
    ) {
      if (reportProgress.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream
        << "Computing S-poly of depth "
        << this->numberOfSymmetricDifferenceRounds
        << ". Taking s-difference of indices "
        << i + 1
        << " and "
        << j + 1
        << " out of "
        << this->basis.size
        << ".<br>Before proceding to adjoin candidates, "
        << "I have "
        << this->basisCandidates.size
        << " candidates and basis of size "
        << this->basis.size
        << ".";
        reportProgress.report(reportStream.str());
      }
      this->generateOneSymmetricDifferenceCandidate(
        this->basis[i], this->basis[j]
      );
    }
  }
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::transformToReducedGroebnerBasis(
  List<Polynomial<Coefficient> >& inputOutput,
  bool rescaleLeadingMonomials
) {
  STACK_TRACE("GroebnerBasisComputation::transformToReducedGroebnerBasis");
  this->initializeForGroebnerComputation();
  this->basisCandidates = inputOutput;
  if (rescaleLeadingMonomials) {
    for (int i = 0; i < this->basisCandidates.size; i ++) {
      this->basisCandidates[i].scaleNormalizeLeadingMonomial(
        &MonomialPolynomial::orderDefault()
      );
    }
  }
  ProgressReport reportStart("Groebner basis start");
  if (this->flagDoProgressReport) {
    reportStart.report(this->toStringStatusGroebnerBasisTransformation());
  }
  this->addAndReducePolynomials();
  if (this->limitsExceeded()) {
    return this->wrapUpGroebnerOnExceedingComputationLimit(inputOutput);
  }
  if (this->basis.size == 1) {
    return this->wrapUpOnGroebnerBasisSuccess(inputOutput);
  }
  do {
    this->generateSymmetricDifferenceCandidates();
    if (!this->addAndReducePolynomials()) {
      return this->wrapUpGroebnerOnExceedingComputationLimit(inputOutput);
    }
  } while (this->flagFoundNewBasisElements);
  bool result = this->wrapUpOnGroebnerBasisSuccess(inputOutput);
  return result;
}

template <class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::
toStringPolynomialBasisStatus() {
  STACK_TRACE("GroebnerBasisComputation::toStringPolynomialBasisStatus");
  std::stringstream out;
  out
  << "There are "
  << this->basisCandidates.size
  << " candidates left to adjoin. "
  << "<br>The current polynomial basis has "
  << this->basis.size
  << " elements. "
  << "<br>Number of polynomial divisions: "
  << this->numberPolynomialDivisions
  << " with a limit of: "
  << this->maximumPolynomialDivisions
  << " computations. ";
  out
  << "<br>Number of monomial operations: "
  << this->numberMonomialOperations
  << ", limit: "
  << this->maximumMonomialOperations
  << ". ";
  out << "The basis follows.";
  for (int i = 0; i < this->basis.size; i ++) {
    out << "<br>" << this->basis[i].toString(&this->format);
  }
  out << "<br>There are " << this->basisCandidates.size << " candidates. ";
  for (int i = 0; i < this->basisCandidates.size; i ++) {
    out << "<br>" << this->basisCandidates[i].toString(&this->format);
  }
  return out.str();
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::limitsExceeded() const {
  if (
    this->maximumMonomialOperations > 0 &&
    this->maximumMonomialOperations < this->numberMonomialOperations
  ) {
    return true;
  }
  if (
    this->maximumPolynomialDivisions > 0 &&
    this->maximumPolynomialDivisions < this->numberPolynomialDivisions
  ) {
    return true;
  }
  return false;
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::addAndReduceOnePolynomial() {
  STACK_TRACE("GroebnerBasisComputation::addAndReduceOnePolynomial");
  if (this->basisCandidates.size == 0) {
    return true;
  }
  this->remainderDivisionByBasis(
    *this->basisCandidates.lastObject(), this->remainderDivision
  );
  if (this->limitsExceeded()) {
    return false;
  }
  ProgressReport report;
  if (this->flagDoProgressReport) {
    std::stringstream reportStream;
    reportStream
    << "Adding remainder: "
    << this->remainderDivision.toString(&this->format);
    report.report(reportStream.str());
  }
  this->basisCandidates.removeLastObject();
  return this->addRemainderToBasis();
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::addAndReducePolynomials() {
  STACK_TRACE("GroebnerBasisComputation::addAndReducePolynomials");
  ProgressReport report("Add polynomial");
  this->flagFoundNewBasisElements = false;
  while (this->basisCandidates.size > 0) {
    if (this->flagDoProgressReport) {
      report.report(this->toStringPolynomialBasisStatus());
    }
    if (!this->addAndReduceOnePolynomial()) {
      return false;
    }
    if (this->limitsExceeded()) {
      return false;
    }
  }
  return true;
}

template <class Coefficient>
int GroebnerBasisComputation<Coefficient>::minimalNumberOfVariables() const {
  STACK_TRACE("GroebnerBasisComputation::minimalNumberOfVariables");
  int result = 0;
  for (int i = 0; i < this->basis.size; i ++) {
    Polynomial<Coefficient>& current = this->basis[i].element;
    result =
    MathRoutines::maximum(current.minimalNumberOfVariables(), result);
  }
  for (int i = 0; i < this->basisCandidates.size; i ++) {
    Polynomial<Coefficient>& current = this->basisCandidates[i];
    result =
    MathRoutines::maximum(current.minimalNumberOfVariables(), result);
  }
  return result;
}

template <class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::toStringLetterOrder() const {
  STACK_TRACE("GroebnerBasisComputation::toStringLetterOrder");
  std::stringstream out;
  int numberOfVariables = this->minimalNumberOfVariables();
  List<MonomialPolynomial> variables;
  out << "Variable name(s). Listed in lexicographic order. ";
  variables.setSize(numberOfVariables);
  for (int i = 0; i < variables.size; i ++) {
    variables[i].makeEi(i, 1);
  }
  variables.quickSortAscending(&this->polynomialOrder.monomialOrder);
  FormatExpressions currentFormat = this->format;
  out << "\\(";
  for (int i = numberOfVariables - 1; i >= 0; i --) {
    out << variables[i].toString(&currentFormat);
    if (i != 0) {
      out << " > ";
    }
  }
  out << "\\). ";
  return out.str();
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::oneDivisonSubStepWithBasis(
  Polynomial<Coefficient>& remainder,
  const MonomialPolynomial& leadingMonomial,
  const Coefficient& leadingCoefficient,
  int index,
  ProgressReport* report
) {
  STACK_TRACE("GroebnerBasisComputation::oneDivisonSubStepWithBasis");
  const MonomialPolynomial& leadingMonomialBasis =
  this->basis[index].leadingMonomial;
  const Coefficient& leadingCoefficientBasis =
  this->basis[index].leadingCoefficient;
  MonomialPolynomial quotientMonomial = leadingMonomial;
  quotientMonomial /= leadingMonomialBasis;
  if (!quotientMonomial.hasPositiveOrZeroExponents()) {
    global.fatal
    << "The pivot monomial "
    << "in the polynomial division algorithm has negative exponent(s). "
    << "This is not allowed. "
    << global.fatal;
  }
  if (this->flagDoLogDivision) {
    this->divisionReport.getElement().intermediateHighestMonDivHighestMon.
    addOnTop(quotientMonomial);
    this->divisionReport.getElement().intermediateSelectedDivisors.addOnTop(
      index
    );
  }
  Polynomial<Coefficient> buffer;
  buffer = this->basis[index].element;
  Coefficient quotientCoefficient = leadingCoefficient;
  quotientCoefficient /= leadingCoefficientBasis;
  if (this->flagDoLogDivision) {
    this->divisionReport.getElement().intermediateCoeffs.addOnTop(
      quotientCoefficient
    );
  }
  buffer.multiplyBy(quotientMonomial, quotientCoefficient);
  if (this->flagStoreQuotients) {
    this->quotients[index].addMonomial(quotientMonomial, quotientCoefficient);
  }
  if (this->flagDoLogDivision) {
    this->divisionReport.getElement().intermediateSubtractands.addOnTop(
      buffer
    );
  }
  if (this->flagDoProgressReport && report != nullptr) {
    std::stringstream out;
    out << "Monomial operation count: " << this->numberMonomialOperations;
    if (this->maximumMonomialOperations > 0) {
      out << ", with a limit of " << this->maximumMonomialOperations;
    }
    out << ". Polynomial divisions: " << this->numberPolynomialDivisions;
    if (this->maximumPolynomialDivisions > 0) {
      out << ", with a limit of " << this->maximumPolynomialDivisions;
    }
    out << ".\n";
    out
    << "<br>Number of intermediate remainders: "
    << this->numberOfIntermediateRemainders
    << "\n<br>Highest remainder monomial: "
    << leadingMonomial.toString()
    << ".\n<br>"
    << "Dividing working remainder monomial "
    << leadingMonomialBasis.toString()
    << " by the leading monomial "
    << leadingMonomialBasis.toString()
    << " of basis element: "
    << index + 1
    << " out of "
    << this->basis.size
    << "\n<br>"
    << remainder.size()
    << " monomials in current remainder.";
    report->report(out.str());
  }
  remainder -= buffer;
  this->numberMonomialOperations += buffer.size();
  if (this->flagDoLogDivision) {
    this->divisionReport.getElement().intermediateRemainders.addOnTop(
      remainder
    );
    List<MonomialPolynomial> empty;
    this->divisionReport.getElement().intermediateHighlightedMons.addOnTop(
      empty
    );
  }
  this->numberMonomialOperations ++;
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::oneDivisonStepWithBasis(
  Polynomial<Coefficient>& currentRemainder,
  Polynomial<Coefficient>& remainderResult,
  int basisIndexToIgnore,
  ProgressReport* report
) {
  STACK_TRACE("GroebnerBasisComputation::oneDivisonStepWithBasis");
  MonomialPolynomial highestMonomial;
  Coefficient leadingCoefficient;
  int indexLeadingMonomial =
  currentRemainder.getIndexLeadingMonomial(
    &highestMonomial,
    &leadingCoefficient,
    &this->polynomialOrder.monomialOrder
  );
  if (indexLeadingMonomial == - 1) {
    // Remainder is zero.
    return false;
  }
  for (int i = 0; i < this->basis.size; i ++) {
    if (i == basisIndexToIgnore) {
      continue;
    }
    if (
      !highestMonomial.isDivisibleBy(this->basis[i].leadingMonomial)
    ) {
      continue;
    }
    this->numberOfIntermediateRemainders ++;
    this->oneDivisonSubStepWithBasis(
      currentRemainder, highestMonomial, leadingCoefficient, i, report
    );
    return true;
  }
  remainderResult.addMonomial(highestMonomial, leadingCoefficient);
  if (this->flagDoLogDivision) {
    (
      *this->divisionReport.getElement().intermediateHighlightedMons.lastObject
      ()
    ).addOnTop(highestMonomial);
  }
  currentRemainder.popMonomial(indexLeadingMonomial);
  this->numberMonomialOperations ++;
  return false;
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::remainderDivisionByBasis(
  const Polynomial<Coefficient>& input,
  Polynomial<Coefficient>& outputRemainder,
  int basisIndexToIgnore
) {
  // Reference: Cox, Little, O'Shea, Ideals, Varieties and Algorithms, page 62.
  STACK_TRACE("GroebnerBasisComputation::remainderDivisionByBasis");
  if (this->flagDoLogDivision) {
    this->divisionReport.getElement().owner = this;
  }
  this->numberPolynomialDivisions ++;
  if (this->flagStoreQuotients) {
    this->quotients.setSize(this->basis.size);
    for (int i = 0; i < this->quotients.size; i ++) {
      this->quotients[i].makeZero();
    }
  }
  ProgressReport reportHeader;
  ProgressReport reportMain;
  ProgressReport reportRemainderSoFar;
  reportRemainderSoFar.ticksPerReport = 50;
  if (this->flagDoProgressReport) {
    std::stringstream reportStream;
    reportStream
    << "Computing remainder of division of "
    << input.toString(&this->format)
    << " by "
    << this->basis.toString(&this->format);
    reportHeader.report(reportStream.str());
  }
  Polynomial<Coefficient> currentRemainder = input;
  outputRemainder.makeZero();
  if (this->flagDoLogDivision) {
    this->divisionReport.getElement().startingPolynomial = currentRemainder;
  }
  if (this->flagDoLogDivision) {
    this->divisionReport.getElement().intermediateCoeffs.size = 0;
    this->divisionReport.getElement().intermediateRemainders.size = 0;
    this->divisionReport.getElement().intermediateSubtractands.size = 0;
    this->divisionReport.getElement().intermediateRemainders.addOnTop(
      currentRemainder
    );
    this->divisionReport.getElement().intermediateHighlightedMons.setSize(1);
    this->divisionReport.getElement().intermediateHighlightedMons.lastObject()
    ->setSize(0);
  }
  while (!currentRemainder.isEqualToZero()) {
    this->numberOfIntermediateRemainders = 0;
    while (
      this->oneDivisonStepWithBasis(
        currentRemainder, outputRemainder, basisIndexToIgnore, &reportMain
      )
    ) {}
    if (this->flagDoProgressReport) {
      std::stringstream out;
      out
      << "Last extracted remainder monomial took: "
      << this->numberOfIntermediateRemainders
      << " polynomial divisions.\n"
      << "<br>"
      << currentRemainder.size()
      << " monomials in the working remainder.";
      out
      << "<br>"
      << outputRemainder.size()
      << " monomials in the final output remainder.";
      out
      << "<br>Current basis: "
      << this->basis.toStringCommaDelimited(&this->format);
      out
      << "<br>Current remainder: "
      << currentRemainder.toString(&this->format);
      reportRemainderSoFar.report(out.str());
    }
  }
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::addRemainderToBasis() {
  STACK_TRACE("GroebnerBasisComputation::addRemainderToBasis");
  if (this->remainderDivision.isEqualToZero()) {
    return true;
  }
  this->flagFoundNewBasisElements = true;
  this->remainderDivision.scaleNormalizeLeadingMonomial(
    &this->polynomialOrder.monomialOrder
  );
  MonomialPolynomial newLeadingMonomial;
  this->remainderDivision.getIndexLeadingMonomial(
    &newLeadingMonomial, nullptr, &this->polynomialOrder.monomialOrder
  );
  int indexToAddAt = 0;
  for (; indexToAddAt < this->basis.size; indexToAddAt ++) {
    MonomialPolynomial& otherLeadingMonomial =
    this->basis[indexToAddAt].leadingMonomial;
    if (
      this->polynomialOrder.monomialOrder.greaterThan(
        otherLeadingMonomial, newLeadingMonomial
      )
    ) {
      break;
    }
  }
  for (int i = this->basis.size - 1; i >= indexToAddAt; i --) {
    this->basisCandidates.addOnTop(this->basis[i].element);
  }
  this->basis.setSize(indexToAddAt);
  this->addBasisElementNoReduction(this->remainderDivision);
  return true;
}

template <class Coefficient>
PolynomialDivisionReport<Coefficient>::PolynomialDivisionReport() {
  this->highlightAllMonsFinalRemainder = - 1;
  this->firstIndexLatexSlide = - 1;
  this->owner = nullptr;
}

template <class Coefficient>
PolynomialSystem<Coefficient>::PolynomialSystem() {
  this->algebraicClosure = nullptr;
  this->numberOfSerreSystemComputations = 0;
  this->numberOfSerreVariablesOneGenerator = - 1;
  this->recursionCounterSerreLikeSystem = 0;
  this->numberOfVariablesToSolveForStart = - 1;
  this->numberOfVariablesToSolveForAfterReduction = 0;
  this->flagTryDirectlySolutionOverAlgebraicClosure = false;
  this->flagUseTheMonomialBranchingOptimization = false;
  this->flagSystemProvenToHaveNoSolution = false;
  this->flagSystemProvenToHaveSolution = false;
  this->flagSystemSolvedOverBaseField = false;
  this->flagUsingAlgebraicClosure = false;
}

template <class Coefficient>
GroebnerBasisComputation<Coefficient>::GroebnerBasisComputation() {
  this->numberPolynomialDivisions = 0;
  this->numberOfIntermediateRemainders = 0;
  this->maximumMonomialOperations = 0;
  this->maximumPolynomialDivisions = 10000;
  this->flagDoLogDivision = false;
  this->flagStoreQuotients = false;
  this->flagFoundNewBasisElements = false;
  this->numberOfSymmetricDifferenceRounds = 0;
}

template <class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::BasisElement::toString(
  FormatExpressions* format
) const {
  std::stringstream out;
  out
  << "[["
  << this->leadingMonomial.toString(format)
  << "]] "
  << this->element.toString(format);
  return out.str();
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::addBasisElementNoReduction(
  const Polynomial<Coefficient>& input
) {
  STACK_TRACE("GroebnerBasisComputation::addBasisElementNoReduction");
  if (this->polynomialOrder.monomialOrder.leftGreaterThanRight == nullptr) {
    global.fatal << "Uninitialized monomial order. " << global.fatal;
  }
  if (input.isEqualToZero()) {
    return;
  }
  this->basis.setSize(this->basis.size + 1);
  GroebnerBasisComputation<Coefficient>::BasisElement& last =
  *this->basis.lastObject();
  last.element = input;
  last.element.getIndexLeadingMonomial(
    &last.leadingMonomial,
    &last.leadingCoefficient,
    &this->polynomialOrder.monomialOrder
  );
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::initializeForGroebnerComputation()
{
  STACK_TRACE("GroebnerBasisComputation::initializeForGroebnerComputation");
  this->basisCandidates.setSize(0);
  this->basis.setSize(0);
  this->numberPolynomialDivisions = 0;
  this->numberMonomialOperations = 0;
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::checkConsistency() {}

template <class Coefficient>
void PolynomialSystem<Coefficient>::
getSubstitutionFromPartialSolutionSerreLikeSystem(
  PolynomialSubstitution<Coefficient>& outputSubstitution
) {
  STACK_TRACE(
    "PolynomialSystem::getSubstitutionFromPartialSolutionSerreLikeSystem"
  );
  outputSubstitution.makeIdentitySubstitution(this->systemSolution.size);
  for (int i = 0; i < this->solutionsFound.cardinalitySelection; i ++) {
    outputSubstitution[this->solutionsFound.elements[i]] =
    this->systemSolution[this->solutionsFound.elements[i]];
  }
}

template <class Coefficient>
bool PolynomialSystem<Coefficient>::getOneVariablePolynomialSolution(
  const Polynomial<Coefficient>& polynomial, Coefficient& outputSolution
) {
  AlgebraicNumber number;
  if (this->algebraicClosure == 0) {
    return false;
  }
  if (
    !number.constructFromMinimalPolynomial(
      polynomial, *this->algebraicClosure, nullptr
    )
  ) {
    return false;
  }
  outputSolution = number;
  return true;
}

template <class Coefficient>
bool PolynomialSystem<Coefficient>::hasImpliedSubstitutions(
  List<Polynomial<Coefficient> >& inputSystem,
  PolynomialSubstitution<Coefficient>& outputSub
) {
  int numberOfVariables = this->systemSolution.size;
  MonomialPolynomial monomial;
  Polynomial<Coefficient> polynomial;
  Coefficient coefficient;
  for (int i = 0; i < inputSystem.size; i ++) {
    polynomial = inputSystem[i];
    for (int j = 0; j < numberOfVariables; j ++) {
      monomial.makeEi(j, 1, numberOfVariables);
      int indexOfMonomial = polynomial.monomials.getIndex(monomial);
      if (indexOfMonomial == - 1) {
        continue;
      }
      coefficient = polynomial.coefficients[indexOfMonomial];
      polynomial.subtractMonomial(monomial, coefficient);
      bool isGood = true;
      for (int k = 0; k < polynomial.size(); k ++) {
        if (!(polynomial[k](j) == 0)) {
          isGood = false;
          polynomial.addMonomial(monomial, coefficient);
          break;
        }
      }
      if (!isGood) {
        continue;
      }
      outputSub.makeIdentitySubstitution(numberOfVariables);
      outputSub[j] = polynomial;
      coefficient *= - 1;
      outputSub[j] /= coefficient;
      return true;
    }
    int oneVarIndex;
    if (polynomial.isOneVariableNonConstantPolynomial(&oneVarIndex)) {
      if (this->flagUsingAlgebraicClosure && this->algebraicClosure != 0) {
        if (
          this->getOneVariablePolynomialSolution(polynomial, coefficient)
        ) {
          outputSub.makeIdentitySubstitution(numberOfVariables);
          outputSub[oneVarIndex].makeConstant(coefficient);
          // check our work:
          polynomial.substitute(outputSub, 1);
          if (!polynomial.isEqualToZero()) {
            global.fatal
            << "I was solving the polynomial equation "
            << inputSystem[i].toString()
            << ", which resulted in the substitution "
            << outputSub.toString()
            << ". However, after carrying out the "
            << "substitution in the polynomial, I got "
            << polynomial.toString()
            << ". "
            << global.fatal;
          }
          //
          return true;
        }
      }
    }
  }
  return false;
}

template <class Coefficient>
int GroebnerBasisComputation<Coefficient>::
getNumberOfEquationsThatWouldBeLinearIfISubstitutedVariable(
  int variableIndex, List<Polynomial<Coefficient> >& input
) {
  int result = 0;
  for (int i = 0; i < input.size; i ++) {
    Rational degreeExcludingVariable = 0;
    for (int j = 0; j < input[i].size(); j ++) {
      degreeExcludingVariable =
      MathRoutines::maximum(
        input[i][j].totalDegree() - input[i][j](variableIndex),
        degreeExcludingVariable
      );
    }
    if (degreeExcludingVariable < 2) {
      result ++;
    }
  }
  return result;
}

template <class Coefficient>
int PolynomialSystem<Coefficient>::getPreferredSerreSystemSubstitutionIndex(
  List<Polynomial<Coefficient> >& inputSystem
) {
  Selection variableSelection;
  this->getVariablesToSolveFor(inputSystem, variableSelection);
  if (variableSelection.cardinalitySelection == 0) {
    return - 1;
  }
  int championIndex = variableSelection.elements[0];
  int championImprovement =
  GroebnerBasisComputation<Coefficient>::
  getNumberOfEquationsThatWouldBeLinearIfISubstitutedVariable(
    championIndex, inputSystem
  );
  for (int i = 1; i < variableSelection.cardinalitySelection; i ++) {
    if (
      GroebnerBasisComputation<Coefficient>::
      getNumberOfEquationsThatWouldBeLinearIfISubstitutedVariable(
        variableSelection.elements[i], inputSystem
      ) >
      championImprovement
    ) {
      championIndex = variableSelection.elements[i];
      championImprovement =
      GroebnerBasisComputation<Coefficient>::
      getNumberOfEquationsThatWouldBeLinearIfISubstitutedVariable(
        variableSelection.elements[i], inputSystem
      );
    }
  }
  return championIndex;
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::backSubstituteIntoSinglePolynomial(
  Polynomial<Coefficient>& substitution,
  int index,
  PolynomialSubstitution<Coefficient>& finalSubstitution
) {
  STACK_TRACE("PolynomialSystem::backSubstituteIntoSinglePolynomial");
  Polynomial<Coefficient> polynomial;
  polynomial.makeMonomial(index, 1, 1);
  if (substitution == polynomial) {
    return;
  }
  substitution.substitute(finalSubstitution, 1);
  bool changed = false;
  for (int i = 0; i < substitution.size(); i ++) {
    for (
      int j = 0; j < substitution[i].minimalNumberOfVariables(); j ++
    ) {
      if (substitution[i](j) != 0) {
        if (!this->solutionsFound.selected[j]) {
          this->setSerreLikeSolutionIndex(j, 0);
        } else {
          if (this->systemSolution[j] != 0) {
            global.fatal
            << "Variable index "
            << j + 1
            << " is supposed to be a free parameter, "
            << "i.e., be set to zero, but "
            << "instead it has a non-zero value. "
            << global.fatal;
          }
        }
        finalSubstitution[j] = 0;
        changed = true;
      }
    }
  }
  if (changed) {
    substitution.substitute(finalSubstitution, 1);
  }
  Coefficient mustBeConstant;
  if (!substitution.isConstant(&mustBeConstant)) {
    global.fatal
    << "After carrying all implied substitutions "
    << "the polynomial is not a constant, rather equals "
    << substitution.toString()
    << ". "
    << global.fatal;
  }
  finalSubstitution[index] = mustBeConstant;
  this->setSerreLikeSolutionIndex(index, mustBeConstant);
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::backSubstituteIntoPolynomialSystem(
  List<PolynomialSubstitution<Coefficient> >& impliedSubstitutions
) {
  STACK_TRACE("PolynomialSystem::backSubstituteIntoPolynomialSystem");
  PolynomialSubstitution<Coefficient> finalSubstitution;
  this->getSubstitutionFromPartialSolutionSerreLikeSystem(finalSubstitution);
  for (int i = impliedSubstitutions.size - 1; i >= 0; i --) {
    for (int j = 0; j < impliedSubstitutions[i].size; j ++) {
      this->backSubstituteIntoSinglePolynomial(
        impliedSubstitutions[i][j], j, finalSubstitution
      );
    }
  }
}

template <class Coefficient>
int PolynomialSystem<Coefficient>::getNumberOfVariablesToSolveFor(
  const List<Polynomial<Coefficient> >& input
) {
  Selection selection;
  PolynomialSystem<Coefficient>::getVariablesToSolveFor(input, selection);
  return selection.cardinalitySelection;
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::getVariablesToSolveFor(
  const List<Polynomial<Coefficient> >& input, Selection& output
) {
  STACK_TRACE("PolynomialSystem::getVariablesToSolveFor");
  int numberOfVariables = 0;
  for (int i = 0; i < input.size; i ++) {
    numberOfVariables =
    MathRoutines::maximum(
      numberOfVariables, input[i].minimalNumberOfVariables()
    );
  }
  output.initialize(numberOfVariables);
  for (
    int i = 0; i < input.size &&
    output.cardinalitySelection < output.numberOfElements; i ++
  ) {
    for (
      int j = 0; j < input[i].size() &&
      output.cardinalitySelection < output.numberOfElements; j ++
    ) {
      for (
        int k = 0; k < input[i][j].minimalNumberOfVariables() &&
        output.cardinalitySelection < output.numberOfElements; k ++
      ) {
        if (input[i][j](k) != 0) {
          output.addSelectionAppendNewIndex(k);
        }
      }
    }
  }
}

template <class Coefficient>
bool PolynomialSystem<Coefficient>::isContradictoryReducedSystem(
  const List<Polynomial<Coefficient> >& input
) {
  if (input.size == 1) {
    if (input[0].isConstant()) {
      return true;
    }
  }
  return false;
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::polynomialSystemSolutionSimplificationPhase
(List<Polynomial<Coefficient> >& inputSystem) {
  STACK_TRACE("PolynomialSystem::polynomialSystemSolutionSimplificationPhase");
  ProgressReport report1;
  ProgressReport report2;
  ProgressReport report3;
  if (this->groebner.flagDoProgressReport) {
    std::stringstream reportStream;
    reportStream
    << "Reducing input system without use of heuristics with "
    << this->groebner.maximumPolynomialDivisions
    << " maximum polynomial divisions "
    << " and "
    << this->groebner.maximumMonomialOperations
    << " maximum monomial operations.";
    report1.report(reportStream.str());
  }
  bool changed = true;
  PolynomialSubstitution<Coefficient> substitution;
  this->impliedSubstitutions.setSize(0);
  this->impliedSubstitutions.reserve(inputSystem.size);
  while (changed) {
    this->groebner.numberPolynomialDivisions = 0;
    List<Polynomial<Coefficient> > oldSystem = inputSystem;
    bool success = this->groebner.transformToReducedBasis(inputSystem);
    if (!success) {
      inputSystem = oldSystem;
    } else {
      oldSystem = inputSystem;
    }
    if (this->groebner.flagDoProgressReport) {
      std::stringstream reportStream;
      reportStream << "Attempting to transform system to a groebner basis... ";
      report2.report(reportStream.str());
    }
    if (success && inputSystem.size > 0) {
      this->groebner.numberPolynomialDivisions = 0;
      success =
      this->groebner.transformToReducedGroebnerBasis(inputSystem, false);
    }
    if (!success) {
      inputSystem = oldSystem;
    } else {
      oldSystem = inputSystem;
    }
    if (this->groebner.flagDoProgressReport) {
      std::stringstream reportStream;
      reportStream << "Transforming system to a groebner basis... ";
      if (success) {
        reportStream
        << "done, basis has "
        << inputSystem.size
        << " elements. ";
      } else {
        reportStream
        << "not successful: computation is too large. "
        << "In the process I transformed the "
        << "starting system to one with "
        << inputSystem.size
        << " elements.";
      }
      report2.report(reportStream.str());
    }
    this->numberOfSerreSystemComputations +=
    this->groebner.numberPolynomialDivisions;
    if (success) {
      if (this->isContradictoryReducedSystem(inputSystem)) {
        this->flagSystemProvenToHaveNoSolution = true;
        this->flagSystemSolvedOverBaseField = false;
        this->flagSystemProvenToHaveSolution = false;
        return;
      } else {
        this->flagSystemProvenToHaveSolution = true;
        if (inputSystem.size == 0) {
          this->flagSystemProvenToHaveNoSolution = false;
          this->flagSystemSolvedOverBaseField = true;
          this->backSubstituteIntoPolynomialSystem(
            this->impliedSubstitutions
          );
          return;
        }
      }
    }
    changed = this->hasImpliedSubstitutions(inputSystem, substitution);
    if (changed) {
      if (this->groebner.flagDoProgressReport) {
        std::stringstream reportStream;
        reportStream
        << "Found implied substitutions.<br>"
        << this->toStringImpliedSubstitutions();
        report3.report(reportStream.str());
      }
      this->impliedSubstitutions.addOnTop(substitution);
      for (int i = 0; i < inputSystem.size; i ++) {
        inputSystem[i].substitute(substitution, 1);
      }
    }
  }
}

template <class Coefficient>
bool PolynomialSystem<Coefficient>::shouldReport() {
  return this->groebner.flagDoProgressReport;
}

template <class Coefficient>
FormatExpressions&PolynomialSystem<Coefficient>::format() {
  return this->groebner.format;
}

template <class Coefficient>
std::string PolynomialSystem<Coefficient>::toStringImpliedSubstitutions() {
  STACK_TRACE("PolynomialSystem::toStringImpliedSubstitutions");
  if (this->impliedSubstitutions.size == 0) {
    return "";
  }
  std::stringstream out;
  out << "Implied subs: ";
  for (int i = 0; i < this->impliedSubstitutions.size; i ++) {
    for (int j = 0; j < this->impliedSubstitutions[i].size; j ++) {
      int letterIndex = - 1;
      if (
        this->impliedSubstitutions[i][j].isOneLetterFirstDegree(&letterIndex)
      ) {
        if (letterIndex == j) {
          continue;
        }
      }
      out
      << "<br>"
      << (MonomialPolynomial(j)).toString(&this->format())
      << "="
      << this->impliedSubstitutions[i][j].toString(&this->format())
      << "; ";
    }
  }
  return out.str();
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::initializeForSystemSolution() {
  STACK_TRACE("PolynomialSystem::initializeForSystemSolution");
  this->groebner.initializeForGroebnerComputation();
  this->impliedSubstitutions.setSize(0);
  this->flagSystemProvenToHaveNoSolution = false;
  this->flagSystemSolvedOverBaseField = false;
  this->flagSystemProvenToHaveSolution = false;
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::setUpRecursiveComputation(
  PolynomialSystem<Coefficient>& toBeModified
) {
  STACK_TRACE("PolynomialSystem::setUpRecursiveComputation");
  toBeModified.initializeForSystemSolution();
  toBeModified.recursionCounterSerreLikeSystem =
  this->recursionCounterSerreLikeSystem;
  toBeModified.systemSolution = this->systemSolution;
  toBeModified.solutionsFound = this->solutionsFound;
  toBeModified.flagUseTheMonomialBranchingOptimization =
  this->flagUseTheMonomialBranchingOptimization;
  toBeModified.flagUsingAlgebraicClosure = this->flagUsingAlgebraicClosure;
  toBeModified.algebraicClosure = this->algebraicClosure;
  toBeModified.groebner.maximumMonomialOperations =
  this->groebner.maximumMonomialOperations;
  toBeModified.groebner.maximumPolynomialDivisions =
  this->groebner.maximumPolynomialDivisions;
  toBeModified.groebner.format = this->groebner.format;
  toBeModified.groebner.polynomialOrder = this->groebner.polynomialOrder;
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::
processSolvedSubcaseIfSolvedOrProvenToHaveSolution(
  PolynomialSystem<Coefficient>& potentiallySolvedCase
) {
  STACK_TRACE(
    "PolynomialSystem::processSolvedSubcaseIfSolvedOrProvenToHaveSolution"
  );
  if (potentiallySolvedCase.flagSystemSolvedOverBaseField) {
    potentiallySolvedCase.numberOfSerreSystemComputations =
    this->numberOfSerreSystemComputations;
    this->solutionsFound = potentiallySolvedCase.solutionsFound;
    this->systemSolution = potentiallySolvedCase.systemSolution;
    this->flagSystemProvenToHaveNoSolution =
    potentiallySolvedCase.flagSystemProvenToHaveNoSolution;
    this->flagSystemProvenToHaveSolution =
    potentiallySolvedCase.flagSystemProvenToHaveSolution;
    this->flagSystemSolvedOverBaseField =
    potentiallySolvedCase.flagSystemSolvedOverBaseField;
    this->backSubstituteIntoPolynomialSystem(this->impliedSubstitutions);
  }
  if (potentiallySolvedCase.flagSystemProvenToHaveSolution) {
    this->flagSystemProvenToHaveSolution = true;
  }
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::trySettingValueToVariable(
  List<Polynomial<Coefficient> >& inputSystem,
  const Rational& aValueToTryOnPreferredVariable
) {
  STACK_TRACE("GroebnerBasisComputation::trySettingValueToVariable");
  ProgressReport report1;
  PolynomialSystem<Coefficient>& heuristicAttempt =
  this->computationUsedInRecursiveCalls.getElement();
  this->setUpRecursiveComputation(heuristicAttempt);
  int variableIndex =
  this->getPreferredSerreSystemSubstitutionIndex(inputSystem);
  if (variableIndex == - 1) {
    global.fatal
    << "Preferred substitution variable index is - 1. "
    << "Input system in calculator-input format: <br>"
    << this->toStringCalculatorInputFromSystem(inputSystem)
    << "<br>"
    << global.fatal;
  }
  PolynomialSubstitution<Coefficient> substitution;
  substitution.makeIdentitySubstitution(this->systemSolution.size);
  substitution[variableIndex] = aValueToTryOnPreferredVariable;
  if (this->groebner.flagDoProgressReport) {
    std::stringstream out;
    MonomialPolynomial monomial(variableIndex);
    out
    << this->toStringImpliedSubstitutions()
    << "Attempting an (a priori random) substitution:<br>"
    << monomial.toString(&this->format())
    << "="
    << aValueToTryOnPreferredVariable
    << ";";
    report1.report(out.str());
  }
  heuristicAttempt.setSerreLikeSolutionIndex(
    variableIndex, aValueToTryOnPreferredVariable
  );
  for (int i = 0; i < inputSystem.size; i ++) {
    inputSystem[i].substitute(substitution, 1);
  }
  heuristicAttempt.solveSerreLikeSystemRecursively(inputSystem);
  this->numberOfSerreSystemComputations +=
  heuristicAttempt.numberOfSerreSystemComputations;
  this->processSolvedSubcaseIfSolvedOrProvenToHaveSolution(heuristicAttempt);
}

template <class Coefficient>
bool PolynomialSystem<Coefficient>::hasSingleMonomialEquation(
  const List<Polynomial<Coefficient> >& inputSystem,
  MonomialPolynomial& outputMonomial
) {
  STACK_TRACE("PolynomialSystem::hasSingleMonomialEquation");
  bool result = false;
  int minimalNumberOfNonZeroMonomialEntries =
  this->numberOfVariablesToSolveForStart * 2 + 1;
  for (int i = 0; i < inputSystem.size; i ++) {
    if (inputSystem[i].size() == 1) {
      result = true;
      int currentNumberNonZeroMonomialEntries = 0;
      for (
        int j = 0; j < inputSystem[i][0].minimalNumberOfVariables(); j ++
      ) {
        if (!(inputSystem[i][0](j) == 0)) {
          currentNumberNonZeroMonomialEntries ++;
        }
      }
      if (
        currentNumberNonZeroMonomialEntries <
        minimalNumberOfNonZeroMonomialEntries
      ) {
        outputMonomial = inputSystem[i][0];
      }
    }
  }
  return result;
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::solveWhenSystemHasSingleMonomial(
  List<Polynomial<Coefficient> >& inputSystem,
  const MonomialPolynomial& monomial
) {
  STACK_TRACE("PolynomialSystem::solveWhenSystemHasSingleMonomial");
  ProgressReport report1;
  List<Polynomial<Coefficient> > inputSystemCopy = inputSystem;
  bool allProvenToHaveNoSolution = true;
  for (int i = 0; i < monomial.minimalNumberOfVariables(); i ++) {
    if (monomial(i) == 0) {
      continue;
    }
    if (this->shouldReport()) {
      std::stringstream out;
      MonomialPolynomial monomial(i);
      out
      << "The system has the single monomial: "
      << monomial.toString(&this->format())
      << "<br>Trying case:<br>"
      << monomial.toString(&this->format())
      << "= 0;";
      report1.report(out.str());
    }
    PolynomialSubstitution<Coefficient> substitution;
    substitution.makeIdentitySubstitution(this->systemSolution.size);
    substitution[i] = 0;
    PolynomialSystem<Coefficient>& oneCase =
    this->computationUsedInRecursiveCalls.getElement();
    this->setUpRecursiveComputation(oneCase);
    oneCase.setSerreLikeSolutionIndex(i, 0);
    inputSystem = inputSystemCopy;
    for (int i = 0; i < inputSystem.size; i ++) {
      inputSystem[i].substitute(substitution, 1);
    }
    oneCase.solveSerreLikeSystemRecursively(inputSystem);
    this->processSolvedSubcaseIfSolvedOrProvenToHaveSolution(oneCase);
    if (!oneCase.flagSystemProvenToHaveNoSolution) {
      allProvenToHaveNoSolution = false;
    }
    if (this->flagSystemSolvedOverBaseField) {
      return;
    }
  }
  if (allProvenToHaveNoSolution) {
    this->flagSystemProvenToHaveNoSolution = true;
  }
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::solveSerreLikeSystemRecursively(
  List<Polynomial<Coefficient> >& inputSystem
) {
  STACK_TRACE("PolynomialSystem::solveSerreLikeSystemRecursively");
  RecursionDepthCounter counter(&this->recursionCounterSerreLikeSystem);
  ProgressReport report1, report2, report3;
  List<Polynomial<Coefficient> > startingSystemNoModifications = inputSystem;
  this->numberOfVariablesToSolveForStart =
  this->getNumberOfVariablesToSolveFor(inputSystem);
  if (this->groebner.flagDoProgressReport) {
    std::stringstream out;
    out
    << "Solving Serre-like polynomial system with "
    << this->numberOfVariablesToSolveForStart
    << " variables at recursion depth: "
    << this->recursionCounterSerreLikeSystem
    << ". ";
    report1.report(out.str());
  }
  this->polynomialSystemSolutionSimplificationPhase(inputSystem);
  if (
    this->flagSystemProvenToHaveNoSolution ||
    this->flagSystemSolvedOverBaseField
  ) {
    return;
  }
  this->numberOfVariablesToSolveForAfterReduction =
  this->getNumberOfVariablesToSolveFor(inputSystem);
  if (this->groebner.flagDoProgressReport) {
    std::stringstream out;
    out
    << "Without using heuristics, I managed to reduce "
    << this->numberOfVariablesToSolveForStart -
    this->numberOfVariablesToSolveForAfterReduction
    << " variables. ";
    if (
      this->numberOfVariablesToSolveForStart -
      this->numberOfVariablesToSolveForAfterReduction !=
      0
    ) {
      out
      << "I did this via the substitutions "
      << this->toStringImpliedSubstitutions();
    }
    out
    << "<br>Number of remaining variables to solve for: "
    << this->numberOfVariablesToSolveForAfterReduction;
    report2.report(out.str());
  }
  List<Polynomial<Coefficient> > systemBeforeHeuristics = inputSystem;
  MonomialPolynomial singleMonEquation;
  if (this->flagUseTheMonomialBranchingOptimization) {
    if (this->hasSingleMonomialEquation(inputSystem, singleMonEquation)) {
      this->solveWhenSystemHasSingleMonomial(inputSystem, singleMonEquation);
      return;
    }
  }
  std::stringstream reportStreamHeuristics;
  for (int randomValueItry = 0; randomValueItry < 2; randomValueItry ++) {
    this->trySettingValueToVariable(inputSystem, randomValueItry);
    if (this->flagSystemSolvedOverBaseField) {
      return;
    }
    inputSystem = systemBeforeHeuristics;
    if (this->groebner.flagDoProgressReport) {
      MonomialPolynomial monomial(
        this->getPreferredSerreSystemSubstitutionIndex(inputSystem)
      );
      reportStreamHeuristics
      << "<br>The substitution  "
      << monomial.toString(&this->groebner.format)
      << "="
      << randomValueItry
      << ";"
      << " did not produce a solution over the base field ";
      if (
        this->computationUsedInRecursiveCalls.getElement().
        flagSystemProvenToHaveNoSolution
      ) {
        reportStreamHeuristics
        << " as it resulted in a system which has no solution. ";
      } else {
        reportStreamHeuristics
        << " as it resulted in a system which "
        << "exceeded the computation limits. ";
      }
      report3.report(reportStreamHeuristics.str());
    }
  }
  inputSystem = startingSystemNoModifications;
}

template <class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::toStringDivision(
  Polynomial<Coefficient>& toBeDivided
) {
  std::stringstream out;
  out
  << "Dividing: "
  << toBeDivided.toString()
  << "<br>by:<br>"
  << this->basis.toStringCommaDelimited();
  return out.str();
}

template <class Coefficient>
std::string PolynomialSystem<Coefficient>::toStringCalculatorInputFromSystem(
  const List<Polynomial<Coefficient> >& inputSystem
) {
  std::stringstream out;
  out << "FindOneSolutionSerreLikePolynomialSystem";
  if (this->flagTryDirectlySolutionOverAlgebraicClosure) {
    out << "Algebraic";
  }
  out
  << "UpperLimit{}("
  << this->groebner.maximumPolynomialDivisions
  << ", "
  << this->groebner.maximumMonomialOperations
  << ", ";
  for (int j = 0; j < inputSystem.size; j ++) {
    out << inputSystem[j].toString(&this->groebner.format);
    if (j != inputSystem.size - 1) {
      out << ", ";
    }
  }
  out << ")";
  return out.str();
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::solveSerreLikeSystem(
  List<Polynomial<Coefficient> >& inputSystem
) {
  STACK_TRACE("PolynomialSystem::solveSerreLikeSystem");
  this->flagSystemProvenToHaveNoSolution = false;
  this->flagSystemSolvedOverBaseField = false;
  this->flagSystemProvenToHaveSolution = false;
  this->recursionCounterSerreLikeSystem = 0;
  this->groebner.numberPolynomialDivisions = 0;
  this->numberOfSerreSystemComputations = 0;
  int numberOfVariables = 0;
  List<Polynomial<Coefficient> > workingSystem = inputSystem;
  for (int i = 0; i < workingSystem.size; i ++) {
    numberOfVariables =
    MathRoutines::maximum(
      numberOfVariables, workingSystem[i].minimalNumberOfVariables()
    );
  }
  this->systemSolution.initializeFillInObject(numberOfVariables, 0);
  this->solutionsFound.initialize(numberOfVariables);
  ProgressReport report;
  std::stringstream reportStream;
  if (this->groebner.flagDoProgressReport) {
    reportStream
    << "Solving system "
    << this->toStringCalculatorInputFromSystem(inputSystem);
    report.report(reportStream.str());
  }
  this->numberOfSerreVariablesOneGenerator =
  workingSystem[0].minimalNumberOfVariables() /
  2;
  if (this->algebraicClosure == 0) {
    this->flagTryDirectlySolutionOverAlgebraicClosure = false;
  }
  if (!this->flagTryDirectlySolutionOverAlgebraicClosure) {
    this->flagUsingAlgebraicClosure = false;
    this->solveSerreLikeSystemRecursively(workingSystem);
  }
  if (this->algebraicClosure != 0) {
    if (
      !this->flagSystemSolvedOverBaseField &&
      !this->flagSystemProvenToHaveNoSolution
    ) {
      if (this->groebner.flagDoProgressReport) {
        if (!this->flagTryDirectlySolutionOverAlgebraicClosure) {
          reportStream
          << "<br><b>Failed to solve system over the rationals... </b>";
        }
        reportStream
        << "<br><b>Attempting to solve allowing algebraic extensions.</b> ";
        report.report(reportStream.str());
      }
      this->flagUsingAlgebraicClosure = true;
      this->solveSerreLikeSystemRecursively(workingSystem);
    }
  }
  if (this->flagSystemSolvedOverBaseField) {
    if (
      this->solutionsFound.cardinalitySelection !=
      this->solutionsFound.numberOfElements
    ) {
      for (int i = 0; i < this->solutionsFound.numberOfElements; i ++) {
        if (!this->solutionsFound.selected[i]) {
          this->setSerreLikeSolutionIndex(i, 0);
        }
      }
    }
    PolynomialSubstitution<Coefficient> substitution;
    this->getSubstitutionFromPartialSolutionSerreLikeSystem(substitution);
    workingSystem = inputSystem;
    for (int i = 0; i < workingSystem.size; i ++) {
      workingSystem[i].substitute(substitution, 1);
      if (!workingSystem[i].isEqualToZero()) {
        global.fatal
        << "<br>"
        << "Function solveSerreLikeSystem reports to "
        << "have found a solution over the base field, "
        << "but substituting the solution back to the original "
        << "system does not yield a zero system of equations. More precisely, "
        << "the reported solution was "
        << this->toStringSerreLikeSolution()
        << " but substitution in equation "
        << inputSystem[i].toString()
        << " yields "
        << workingSystem[i].toString()
        << ". Calculator input: <br>"
        << this->toStringCalculatorInputFromSystem(inputSystem)
        << " <br>"
        << global.fatal;
      }
    }
  }
}

template <class Coefficient>
std::string PolynomialSystem<Coefficient>::toStringSerreLikeSolution() {
  STACK_TRACE("GroebnerBasisComputation::toStringSerreLikeSolution");
  std::stringstream out;
  Polynomial<Rational> monomial;
  for (int i = 0; i < this->systemSolution.size; i ++) {
    if (this->solutionsFound.selected[i]) {
      monomial.makeMonomial(i, 1, 1);
      out
      << " "
      << monomial.toString(&this->groebner.format)
      << " = "
      << this->systemSolution[i]
      << ";";
    }
  }
  return out.str();
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::setSerreLikeSolutionIndex(
  int index, const Coefficient& inputConstant
) {
  this->systemSolution[index] = inputConstant;
  if (this->solutionsFound.selected[index]) {
    global.fatal
    << "Attempting to set "
    << "value to a variable whose value has already been computed. "
    << global.fatal;
  }
  this->solutionsFound.addSelectionAppendNewIndex(index);
}

template <class Coefficient>
bool Polynomial<Coefficient>::leastCommonMultipleOneVariable(
  const Polynomial<Coefficient>& left,
  const Polynomial<Coefficient>& right,
  Polynomial<Coefficient>& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Polynomial::leastCommonMultipleOneVariable");
  Polynomial<Coefficient> divisor;
  if (
    !Polynomial<Coefficient>::greatestCommonDivisorOneVariable(
      left, right, divisor, commentsOnFailure
    )
  ) {
    return false;
  }
  Polynomial<Coefficient> product = left;
  Polynomial<Coefficient> remainder;
  product *= right;
  product.divideBy(
    divisor,
    output,
    remainder,
    &MonomialPolynomial::orderDegreeThenLeftLargerWins()
  );
  if (!remainder.isEqualToZero()) {
    global.fatal
    << "In least common multiple computation: "
    << "remainder when dividing by greatest common divisor not zero."
    << global.fatal;
  }
  output.scaleNormalizeLeadingMonomial(nullptr);
  return true;
}

template <class Coefficient>
bool Polynomial<Coefficient>::leastCommonMultiple(
  const Polynomial<Coefficient>& left,
  const Polynomial<Coefficient>& right,
  Polynomial<Coefficient>& output,
  const Coefficient& one,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Polynomial::leastCommonMultiple");
  if (left.isEqualToZero() || right.isEqualToZero()) {
    global.fatal
    << "Least common multiple of zero polynomials is not allowed. "
    << global.fatal;
  }
  int numberOfVariables =
  MathRoutines::maximum(
    left.minimalNumberOfVariables(), right.minimalNumberOfVariables()
  );
  if (numberOfVariables <= 1) {
    return
    Polynomial<Coefficient>::leastCommonMultipleOneVariable(
      left, right, output, commentsOnFailure
    );
  }
  Polynomial<Coefficient> leftTemp, rightTemp;
  leftTemp = left;
  rightTemp = right;
  Polynomial<Coefficient> oneMinusT;
  List<Polynomial<Coefficient> > basis;
  leftTemp.scaleNormalizeLeadingMonomial(
    &MonomialPolynomial::orderDefault()
  );
  rightTemp.scaleNormalizeLeadingMonomial(
    &MonomialPolynomial::orderDefault()
  );
  oneMinusT.makeMonomial(numberOfVariables, 1, one);
  leftTemp *= oneMinusT;
  oneMinusT *= - 1;
  oneMinusT += one;
  rightTemp *= oneMinusT;
  basis.setSize(0);
  basis.addOnTop(leftTemp);
  basis.addOnTop(rightTemp);
  GroebnerBasisComputation<Coefficient> computation;
  computation.format.flagSuppressModP = true;
  computation.format.polynomialAlphabet.addOnTop("x");
  computation.format.polynomialAlphabet.addOnTop("y");
  computation.format.polynomialAlphabet.addOnTop("z");
  computation.format.polynomialAlphabet.addOnTop("w");
  computation.polynomialOrder.monomialOrder =
  MonomialPolynomial::orderForGreatestCommonDivisor();
  computation.maximumMonomialOperations = - 1;
  if (!computation.transformToReducedGroebnerBasis(basis, false)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Unexpected: failed to transform to reduced "
      << "Groebner basis. ";
    }
    return false;
  }
  int maximalMonomialNoTIndex = - 1;
  Rational maximaltotalDegree;
  MonomialPolynomial currentLeading;
  for (int i = basis.size - 1; i >= 0; i --) {
    basis[i].getIndexLeadingMonomial(
      &currentLeading,
      nullptr,
      &computation.polynomialOrder.monomialOrder
    );
    if (currentLeading(numberOfVariables) == 0) {
      if (maximalMonomialNoTIndex == - 1) {
        maximaltotalDegree = currentLeading.totalDegree();
        maximalMonomialNoTIndex = i;
      }
      if (maximaltotalDegree < currentLeading.totalDegree()) {
        maximaltotalDegree = currentLeading.totalDegree();
        maximalMonomialNoTIndex = i;
      }
    }
  }
  if (maximalMonomialNoTIndex == - 1) {
    global.fatal
    << "Failed to obtain the least common multiple of: "
    << left.toString(&computation.format)
    << " and "
    << right.toString(&computation.format)
    << ". "
    << "The list of polynomials is: ";
    for (int i = 0; i < basis.size; i ++) {
      global.fatal << basis[i].toString(&computation.format) << ", ";
    }
    global.fatal << global.fatal;
  }
  output = basis[maximalMonomialNoTIndex];
  output.setNumberOfVariablesSubstituteDeletedByOne(numberOfVariables);
  output.scaleNormalizeLeadingMonomial(
    &MonomialPolynomial::orderDefault()
  );
  return true;
}

template <class Coefficient>
bool Polynomial<Coefficient>::greatestCommonDivisorOneVariableDefault(
  const Polynomial<Coefficient>& left,
  const Polynomial<Coefficient>& right,
  Polynomial<Coefficient>& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Polynomial::greatestCommonDivisorOneVariableDefault");
  Polynomial<Coefficient> leftCopy = left;
  Polynomial<Coefficient> rightCopy = right;
  Polynomial<Coefficient> quotient;
  Polynomial<Coefficient> remainder;
  if (
    !leftCopy.hasNonNegativeIntegralExponents() ||
    !rightCopy.hasNonNegativeIntegralExponents()
  ) {
    global.fatal
    << "Your inputs are expected to have "
    << "non-negative integral powers. Instead, they are: "
    << leftCopy.toString(FormatExpressions::defaultFormat())
    << " and "
    << rightCopy.toString(FormatExpressions::defaultFormat())
    << global.fatal;
  }
  if (
    left.minimalNumberOfVariables() > 1 || right.minimalNumberOfVariables() > 1
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "This greatest common divisor computation "
      << "method requires one variable.";
    }
    return false;
  }
  leftCopy.scaleNormalizeLeadingMonomial(nullptr);
  rightCopy.scaleNormalizeLeadingMonomial(nullptr);
  // We are re-implementing MathRoutines::greatestCommonDivisor,
  // as this may be a performance-sensitive function.
  while (!rightCopy.isEqualToZero()) {
    leftCopy.divideBy(
      rightCopy,
      quotient,
      remainder,
      &MonomialPolynomial::orderDegreeThenLeftLargerWins()
    );
    if (
      !remainder.isEqualToZero() &&
      remainder.totalDegree() >= rightCopy.totalDegree()
    ) {
      global.fatal
      << "Univariate polynomial division of "
      << leftCopy.toString(FormatExpressions::defaultFormat())
      << " by "
      << rightCopy.toString(FormatExpressions::defaultFormat())
      << " yields remainder "
      << remainder.toString(FormatExpressions::defaultFormat())
      << " which does not reduce the degree. "
      << global.fatal;
    }
    leftCopy = rightCopy;
    rightCopy = remainder;
  }
  output = leftCopy;
  output.scaleNormalizeLeadingMonomial(nullptr);
  return true;
}

template <class Coefficient>
bool Polynomial<Coefficient>::greatestCommonDivisor(
  const Polynomial<Coefficient>& left,
  const Polynomial<Coefficient>& right,
  Polynomial<Coefficient>& output,
  const Coefficient& one,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("Polynomial::greatestCommonDivisor");
  if (left.isEqualToZero() || right.isEqualToZero()) {
    global.fatal
    << "Greatest common divisor including zeroes not allowed. "
    << global.fatal;
  }
  if (
    left.minimalNumberOfVariables() <= 1 &&
    right.minimalNumberOfVariables() <= 1
  ) {
    return
    Polynomial<Coefficient>::greatestCommonDivisorOneVariable(
      left, right, output, commentsOnFailure
    );
  }
  Polynomial<Coefficient> leastCommonMultipleBuffer;
  Polynomial<Coefficient> productBuffer;
  Polynomial<Coefficient> remainderBuffer;
  if (
    !Polynomial<Coefficient>::leastCommonMultiple(
      left, right, leastCommonMultipleBuffer, one, commentsOnFailure
    )
  ) {
    return false;
  }
  productBuffer = left;
  productBuffer *= right;
  productBuffer.divideBy(
    leastCommonMultipleBuffer,
    output,
    remainderBuffer,
    &MonomialPolynomial::orderForGreatestCommonDivisor()
  );
  if (!remainderBuffer.isEqualToZero() || output.isEqualToZero()) {
    FormatExpressions format;
    format.polynomialAlphabet.addOnTop("x");
    format.polynomialAlphabet.addOnTop("y");
    global.fatal
    << "<br>While computing the gcd of left = "
    << left.toString(&format)
    << "<br>and right = "
    << right.toString(&format)
    << "<br>I got that left * right = "
    << productBuffer.toString(&format)
    << "<br>, and that lcm(left, right) = "
    << leastCommonMultipleBuffer.toString(&format)
    << "<br>but at the same time "
    << "right * left divided by lcm (left, right) equals<br>"
    << output.toString(&format)
    << "<br>with remainder "
    << remainderBuffer.toString(&format)
    << ", which is imposible."
    << global.fatal;
  }
  output.scaleNormalizeLeadingMonomial(
    &MonomialPolynomial::orderDefault()
  );
  return true;
}

template <class Coefficient>
PolynomialFactorizationUnivariate<Coefficient>::
PolynomialFactorizationUnivariate() {
  this->computations = 0;
  this->maximumComputations = - 1;
  this->maximumDegree = - 1;
  this->algorithm = "[unknown]";
}

template <class Coefficient>
bool PolynomialFactorizationUnivariate<Coefficient>::basicChecks(
  std::stringstream* commentsOnFailure
) {
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
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Factoring zero is not allowed. ";
    }
    return false;
  }
  if (!this->current.hasSmallIntegralPositivePowers(nullptr)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Polynomial is expected to have sufficiently "
      << "small non-negative integer powers. ";
    }
    return false;
  }
  if (this->current.totalDegree() > this->maximumDegree) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "The degree of the input is larger than the "
      << "maximum allowed for the "
      << this->algorithm
      << " algorithm: "
      << this->maximumDegree
      << ". ";
    }
    return false;
  }
  return true;
}

template <class Coefficient>
template <class OneFactorFinder>
bool PolynomialFactorizationUnivariate<Coefficient>::factor(
  const Polynomial<Coefficient>& input,
  OneFactorFinder& algorithm,
  std::stringstream* comments,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("PolynomialFactorizationUnivariate::factor");
  this->original = input;
  if (this->original.isConstant(&this->constantFactor)) {
    return true;
  }
  this->current = this->original;
  this->constantFactor =
  this->current.scaleNormalizeLeadingMonomial(
    &MonomialPolynomial::orderDefault()
  );
  this->constantFactor.invert();
  this->nonReduced.addOnTop(this->current);
  this->maximumDegree = algorithm.maximumDegreeDefault;
  this->algorithm = algorithm.name();
  if (!this->basicChecks(commentsOnFailure)) {
    return false;
  }
  algorithm.output = this;
  while (this->nonReduced.size > 0) {
    this->current = this->nonReduced.popLastObject();
    // The algorithm is allowed to store state between factorization attempts.
    if (!algorithm.oneFactor(comments, commentsOnFailure)) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Failed to factor: " << this->current.toString();
      }
      return false;
    }
  }
  this->nonReduced.quickSortAscending();
  this->reduced.quickSortAscending();
  this->checkFactorization();
  return true;
}

template <class Coefficient>
bool PolynomialFactorizationUnivariate<Coefficient>::accountNonReducedFactor(
  Polynomial<Coefficient>& incoming
) {
  STACK_TRACE("PolynomialFactorizationUnivariate::accountNonReducedFactor");
  incoming.scaleNormalizeLeadingMonomial(
    &MonomialPolynomial::orderDefault()
  );
  Polynomial<Coefficient> quotient;
  Polynomial<Coefficient> remainder;
  this->current.divideBy(
    incoming, quotient, remainder, &MonomialPolynomial::orderDefault()
  );
  if (!remainder.isEqualToZero()) {
    return false;
  }
  this->nonReduced.addOnTop(incoming);
  if (quotient.isConstant()) {
    FormatExpressions format;
    format.flagSuppressModP = true;
    format.polynomialAlphabet.addOnTop("x");
    global.fatal
    << "While factoring: "
    << this->original.toString(&format)
    << "\nSub-factor: "
    << this->current.toString(&format)
    << "\nAccounting non-reduced factor:\n"
    << incoming.toString(&format)
    << "\nof:\n"
    << this->current.toString(&format)
    << "\nmust result in degree drop. "
    << global.fatal;
  }
  this->nonReduced.addOnTop(quotient);
  return true;
}

template <class Coefficient>
bool PolynomialFactorizationUnivariate<Coefficient>::accountReducedFactor(
  Polynomial<Coefficient>& incoming, bool accountQuotientAsNonReduced
) {
  STACK_TRACE("PolynomialFactorizationUnivariate::accountReducedFactor");
  if (incoming.isConstant()) {
    global.fatal
    << "Constant factors are not to be accounted. "
    << global.fatal;
  }
  incoming.scaleNormalizeLeadingMonomial(
    &MonomialPolynomial::orderDefault()
  );
  Polynomial<Coefficient> quotient;
  Polynomial<Coefficient> remainder;
  this->current.divideBy(
    incoming, quotient, remainder, &MonomialPolynomial::orderDefault()
  );
  if (!remainder.isEqualToZero()) {
    return false;
  }
  this->reduced.addOnTop(incoming);
  if (accountQuotientAsNonReduced || quotient.isConstant()) {
    Coefficient extraFactor;
    if (quotient.isConstant(&extraFactor)) {
      this->constantFactor *= extraFactor;
    } else {
      this->nonReduced.addOnTop(quotient);
    }
  }
  this->current = quotient;
  return true;
}

template <class Coefficient>
bool PolynomialFactorizationUnivariate<Coefficient>::checkFactorization() const {
  STACK_TRACE("PolynomialFactorizationUnivariate::checkFactorization");
  Polynomial<Coefficient> checkComputations;
  checkComputations.makeConstant(this->constantFactor);
  for (int i = 0; i < this->nonReduced.size; i ++) {
    checkComputations *= this->nonReduced[i];
  }
  for (int i = 0; i < this->reduced.size; i ++) {
    checkComputations *= this->reduced[i];
  }
  if (!checkComputations.isEqualTo(this->original)) {
    FormatExpressions format;
    format.flagSuppressModP = true;
    format.polynomialAlphabet.addOnTop("x");
    global.fatal
    << "Error in polynomial factorization function. "
    << "Product of factorization: "
    << this->toStringResult(nullptr)
    << " equals "
    << checkComputations.toString(&format)
    << "; expected: "
    << this->original.toString(&format)
    << global.fatal;
  }
  return true;
}

template <class Coefficient>
std::string PolynomialFactorizationUnivariate<Coefficient>::toStringResult(
  FormatExpressions* format
) const {
  std::stringstream out;
  MemorySaving<FormatExpressions> backup;
  if (format == nullptr) {
    format = &backup.getElement();
    format->flagSuppressModP = true;
    format->polynomialAlphabet.addOnTop("x");
  }
  std::string constantFactorString = this->constantFactor.toString(format);
  if (this->nonReduced.size + this->reduced.size == 0) {
    return constantFactorString;
  }
  if (constantFactorString == "1") {
    constantFactorString = "";
  }
  if (constantFactorString == "-1" || constantFactorString == "- 1") {
    constantFactorString = "-";
  }
  out << constantFactorString;
  for (int i = 0; i < this->reduced.size; i ++) {
    out << "(" << this->reduced[i].toString(format) << ")";
  }
  if (this->nonReduced.size > 0) {
    out << "[[";
  }
  for (int i = 0; i < this->nonReduced.size; i ++) {
    out << "(" << this->nonReduced[i].toString(format) << ")";
  }
  if (this->nonReduced.size > 0) {
    out << "]]";
  }
  return out.str();
}

// The Cantor-Zassenhaus algorithm:
// https://en.wikipedia.org/wiki/Cantor%E2%80%93Zassenhaus_algorithm
template <
  class PolynomialModuloPolynomialImplementation,
  class PolynomialImplementation,
  class PolynomialModulusImplementation
>
PolynomialFactorizationCantorZassenhaus<
  PolynomialModuloPolynomialImplementation,
  PolynomialImplementation,
  PolynomialModulusImplementation
>::PolynomialFactorizationCantorZassenhaus() {
  this->output = nullptr;
}

template <
  class PolynomialModuloPolynomialImplementation,
  class PolynomialImplementation,
  class PolynomialModulusImplementation
>
bool PolynomialFactorizationCantorZassenhaus<
  PolynomialModuloPolynomialImplementation,
  PolynomialImplementation,
  PolynomialModulusImplementation
>::checkInitialization() const {
  if (this->output == nullptr) {
    global.fatal << "Null output pointer not allowed. " << global.fatal;
  }
  return true;
}

template <
  class PolynomialModuloPolynomialImplementation,
  class PolynomialImplementation,
  class PolynomialModulusImplementation
>
void PolynomialFactorizationCantorZassenhaus<
  PolynomialModuloPolynomialImplementation,
  PolynomialImplementation,
  PolynomialModulusImplementation
>::initialize(const PolynomialImplementation& modulusInitializer) {
  this->current = modulusInitializer;
}

template <
  class PolynomialModuloPolynomialImplementation,
  class PolynomialImplementation,
  class PolynomialModulusImplementation
>
bool PolynomialFactorizationCantorZassenhaus<
  PolynomialModuloPolynomialImplementation,
  PolynomialImplementation,
  PolynomialModulusImplementation
>::oneFactor(
  std::stringstream* comments, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("PolynomialFactorizationCantorZassenhaus::oneFactor");
  this->checkInitialization();
  this->output->format.flagSuppressModP = true;
  this->current = this->output->current;
  this->one.makeOne(this->output->current.coefficients[0].modulus);
  if (this->one.modulus == 2) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Only odd primes allowed. ";
    }
    return false;
  }
  ProgressReport report;
  std::stringstream out;
  out
  << "Find one factor of "
  << this->current.toString(&this->output->format);
  out
  << "Factorization so far: "
  << this->output->toStringResult(&this->output->format);
  report.report(out.str());
  PolynomialImplementation derivative;
  this->current.derivative(derivative);
  PolynomialImplementation candidate;
  if (
    !derivative.greatestCommonDivisor(
      derivative,
      this->current,
      candidate,
      this->one,
      commentsOnFailure
    )
  ) {
    return false;
  }
  if (!candidate.isConstant()) {
    this->output->accountNonReducedFactorTemplate(candidate);
    return true;
  }
  return this->oneFactorGo(comments, commentsOnFailure);
}

template <
  class PolynomialModuloPolynomialImplementation,
  class PolynomialImplementation,
  class PolynomialModulusImplementation
>
bool PolynomialFactorizationCantorZassenhaus<
  PolynomialModuloPolynomialImplementation,
  PolynomialImplementation,
  PolynomialModulusImplementation
>::hasFactorsOfDifferentDegree(std::stringstream* comments) {
  STACK_TRACE(
    "PolynomialFactorizationCantorZassenhaus::hasFactorsOfDifferentDegree"
  );
  (void) comments;
  int degree = this->current.totalDegreeInt();
  PolynomialModuloPolynomialImplementation currentPower;
  PolynomialModuloPolynomialImplementation currentPowerMinusBaseLetter;
  currentPower = this->baseLetter;
  PolynomialImplementation candidateDivisor;
  currentPower.checkInitialization();
  for (int i = 0; i < degree; i ++) {
    MathRoutines::raiseToPower(
      currentPower, this->one.modulus, this->oneQuotientRing
    );
    currentPowerMinusBaseLetter = currentPower;
    currentPowerMinusBaseLetter -= this->baseLetter;
    if (currentPowerMinusBaseLetter.isEqualToZero()) {
      continue;
    }
    bool expectedToBeTrue =
    this->current.greatestCommonDivisor(
      this->current,
      currentPowerMinusBaseLetter.value,
      candidateDivisor,
      this->one,
      nullptr
    );
    if (!expectedToBeTrue) {
      global.fatal
      << "Unexpected failure to compute greatest common divisor of "
      << this->current
      << " and "
      << currentPowerMinusBaseLetter.value
      << ". "
      << global.fatal;
    }
    if (comments != nullptr) {
      std::string xLetter = this->output->format.getPolynomialLetter(0);
      *comments
      << "<br>GCD: "
      << candidateDivisor.toString(&this->output->format)
      << " of: \\("
      << xLetter
      << "^{"
      << this->one.modulus
      << "^{"
      << degree
      << "}} - "
      << xLetter
      << "\\) "
      << "and \\("
      << this->current.toString(&this->output->format)
      << "\\)";
    }
    if (
      candidateDivisor.totalDegreeInt() > 0 &&
      candidateDivisor.totalDegreeInt() < this->current.totalDegreeInt()
    ) {
      if (comments != nullptr) {
        std::string xLetter = this->output->format.getPolynomialLetter(0);
        *comments
        << "<br>Found factor: "
        << candidateDivisor.toString(&this->output->format)
        << " by gcd between \\("
        << xLetter
        << "^{"
        << this->one.modulus
        << "^{"
        << degree
        << "}} - "
        << xLetter
        << "\\) "
        << "and \\("
        << this->current.toString(&this->output->format)
        << "\\)";
      }
      this->output->accountNonReducedFactorTemplate(candidateDivisor);
      return true;
    }
  }
  return false;
}

template <
  class PolynomialModuloPolynomialImplementation,
  class PolynomialImplementation,
  class PolynomialModulusImplementation
>
bool PolynomialFactorizationCantorZassenhaus<
  PolynomialModuloPolynomialImplementation,
  PolynomialImplementation,
  PolynomialModulusImplementation
>::divisorFromCandidate(
  const PolynomialImplementation& candidate,
  const std::string& candidateDisplayName,
  std::stringstream* comments
) {
  if (candidate.isEqualToZero()) {
    return false;
  }
  PolynomialImplementation divisor;
  bool mustBeTrue =
  candidate.greatestCommonDivisor(
    candidate, this->current, divisor, this->one, comments
  );
  if (!mustBeTrue) {
    global.fatal << "Failure to find gcd not allowed here. " << global.fatal;
  }
  if (
    divisor.totalDegreeInt() <= 0 ||
    divisor.totalDegreeInt() >= this->current.totalDegreeInt()
  ) {
    return false;
  }
  if (comments != nullptr) {
    Polynomial<ElementZmodP> divisorConverted;
    PolynomialConversions::convertToPolynomial(divisor, divisorConverted);
    *comments
    << "Found divisor \\("
    << this->one.toStringPolynomial(
      divisorConverted, &this->output->format
    )
    << "\\) by taking gcd with \\("
    << candidateDisplayName
    << "\\). ";
  }
  this->output->accountNonReducedFactorTemplate(divisor);
  return true;
}

template <
  class PolynomialModuloPolynomialImplementation,
  class PolynomialImplementation,
  class PolynomialModulusImplementation
>
bool PolynomialFactorizationCantorZassenhaus<
  PolynomialModuloPolynomialImplementation,
  PolynomialImplementation,
  PolynomialModulusImplementation
>::handlePrimeDegreeSeparatedFactor(PolynomialImplementation& input) {
  STACK_TRACE(
    "PolynomialFactorizationCantorZassenhaus::"
    "handlePrimeDegreeSeparatedFactor"
  );
  this->checkInitialization();
  int linearTermsToTry = 500;
  if (this->one.modulus < linearTermsToTry) {
    this->one.modulus.isIntegerFittingInInt(&linearTermsToTry);
  }
  List<ElementZmodP> foundRoots;
  ElementZmodP rootCandidate;
  rootCandidate = this->one;
  for (int i = 0; i < linearTermsToTry; i ++) {
    rootCandidate.value = static_cast<unsigned>(i);
    ElementZmodP value = input.evaluate(rootCandidate, this->one.zero());
    if (value.isEqualToZero()) {
      foundRoots.addOnTop(rootCandidate);
    }
  }
  if (foundRoots.size == 0) {
    if (this->one.modulus <= linearTermsToTry) {
      // No linear factors; the current factor is irreducible
      // as it is of prime degree and all of its sub-factors
      // are of equal degree.
      return this->output->accountReducedFactorTemplate(input, true);
    }
    // It is possible that the current factor has
    // linear factors we haven't checked for.
    return false;
  }
  for (int i = 0; i < foundRoots.size; i ++) {
    Polynomial<ElementZmodP> linearFactor;
    linearFactor.makeMonomial(0, 1, this->one);
    linearFactor -= foundRoots[i];
    bool shouldAccountQuotientAsNonReduced = false;
    if (i == foundRoots.size - 1) {
      shouldAccountQuotientAsNonReduced = true;
    }
    if (
      !this->output->accountReducedFactor(
        linearFactor, shouldAccountQuotientAsNonReduced
      )
    ) {
      global.fatal
      << "Failed to account for reduced factor: "
      << linearFactor.toString()
      << global.fatal;
    }
  }
  return true;
}

template <
  class PolynomialModuloPolynomialImplementation,
  class PolynomialImplementation,
  class PolynomialModulusImplementation
>
void PolynomialFactorizationCantorZassenhaus<
  PolynomialModuloPolynomialImplementation,
  PolynomialImplementation,
  PolynomialModulusImplementation
>::initializeOneFactorComputation() {
  STACK_TRACE(
    "PolynomialFactorizationCantorZassenhaus::initializeOneFactorComputation"
  );
  Polynomial<ElementZmodP> polynomial;
  polynomial.makeDegreeOne(1, 0, this->one, this->one.zero());
  this->modulus = this->current;
  this->baseLetter.makeFromModulusAndValue(&this->modulus, polynomial);
  polynomial.makeConstant(this->one);
  this->oneQuotientRing.makeFromModulusAndValue(&this->modulus, polynomial);
  this->oneQuotientRing.checkInitialization();
  this->baseLetter.checkInitialization();
}

template <
  class PolynomialModuloPolynomialImplementation,
  class PolynomialImplementation,
  class PolynomialModulusImplementation
>
bool PolynomialFactorizationCantorZassenhaus<
  PolynomialModuloPolynomialImplementation,
  PolynomialImplementation,
  PolynomialModulusImplementation
>::oneFactorGo(
  std::stringstream* comments, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("PolynomialFactorizationCantorZassenhaus::oneFactorGo");
  (void) commentsOnFailure;
  this->initializeOneFactorComputation();
  if (this->hasFactorsOfDifferentDegree(comments)) {
    return true;
  }
  if (comments != nullptr) {
    *comments
    << "<br>All divisors of "
    << this->current.toString(&this->output->format)
    << " are of equal degree. ";
  }
  return this->handleFactorsOfSameDegree(comments, commentsOnFailure);
}

template <
  class PolynomialModuloPolynomialImplementation,
  class PolynomialImplementation,
  class PolynomialModulusImplementation
>
bool PolynomialFactorizationCantorZassenhaus<
  PolynomialModuloPolynomialImplementation,
  PolynomialImplementation,
  PolynomialModulusImplementation
>::handleFactorsOfSameDegree(
  std::stringstream* comments, std::stringstream* commentsOnFailure
) {
  LargeInteger degree = this->current.totalDegreeInt();
  if (degree.value.isPossiblyPrime(0, true, comments)) {
    if (this->handlePrimeDegreeSeparatedFactor(this->current)) {
      return true;
    }
  }
  this->current.checkInitialization();
  if (this->current.totalDegreeInt() == 1) {
    return this->output->accountReducedFactorTemplate(this->current, true);
  }
  int maximumDivisors = 10;
  if (this->one.modulus < maximumDivisors) {
    this->one.modulus.isIntegerFittingInInt(&maximumDivisors);
  }
  ProgressReport report;
  for (
    unsigned i = 0; i < static_cast<unsigned>(maximumDivisors); i ++
  ) {
    std::stringstream reportStream;
    reportStream
    << "Looking for factors round "
    << i + 1
    << " out of "
    << maximumDivisors;
    report.report(reportStream.str());
    if (this->oneFactorProbabilityHalf(i, comments, commentsOnFailure)) {
      return true;
    }
  }
  return this->output->accountReducedFactorTemplate(this->current, true);
}

template <
  class PolynomialModuloPolynomialImplementation,
  class PolynomialImplementation,
  class PolynomialModulusImplementation
>
void PolynomialFactorizationCantorZassenhaus<
  PolynomialModuloPolynomialImplementation,
  PolynomialImplementation,
  PolynomialModulusImplementation
>::computeStartingPolynomial(
  unsigned int constant, PolynomialModuloPolynomialImplementation& output
) {
  STACK_TRACE(
    "PolynomialFactorizationCantorZassenhaus::computeStartingPolynomial"
  );
  Polynomial<ElementZmodP> constructed;
  int power = 0;
  int constantTerm = 0;
  if (constant < this->one.modulus) {
    constantTerm = constant;
    power = 1;
  } else {
    constantTerm = 1 + (constant - 1) % (
      this->one.modulus.getUnsignedIntValueTruncated() - 1
    );
    power = 1 + (constant - 1) / (
      this->one.modulus.getUnsignedIntValueTruncated() - 1
    );
    if (power >= this->current.totalDegreeInt()) {
      power = 1;
    }
  }
  ElementZmodP constantModular;
  constantModular.makeFrom(this->one.modulus, constantTerm);
  constructed.makeMonomial(0, power, this->one);
  constructed += constantModular;
  output = this->baseLetter;
  output.setValue(constructed);
}

template <
  class PolynomialModuloPolynomialImplementation,
  class PolynomialImplementation,
  class PolynomialModulusImplementation
>
bool PolynomialFactorizationCantorZassenhaus<
  PolynomialModuloPolynomialImplementation,
  PolynomialImplementation,
  PolynomialModulusImplementation
>::oneFactorProbabilityHalf(
  unsigned int constant,
  std::stringstream* comments,
  std::stringstream* commentsOnFailure
) {
  (void) commentsOnFailure;
  LargeIntegerUnsigned modulus = this->one.modulus;
  int degree = this->current.totalDegreeInt();
  PolynomialModuloPolynomialImplementation startingPolynomial;
  this->computeStartingPolynomial(constant, startingPolynomial);
  PolynomialModuloPolynomialImplementation currentPolynomial;
  PolynomialModuloPolynomialImplementation currentPolynomialPlusOne;
  PolynomialModuloPolynomialImplementation currentPolynomialMinusOne;
  for (int i = 0; i < degree; i ++) {
    currentPolynomial = startingPolynomial;
    LargeInteger power = modulus;
    power.raiseToPower(i + 1);
    power -= 1;
    power /= 2;
    MathRoutines::raiseToPower(
      currentPolynomial, power, this->oneQuotientRing
    );
    if (comments != nullptr) {
      Polynomial<ElementZmodP> converted;
      PolynomialConversions::convertToPolynomial(
        currentPolynomial.getValue(), converted
      );
      *comments
      << "<br>\\(A=x^{\\frac{"
      << modulus
      << "^{"
      << i + 1
      << "} - 1}{2}} = "
      << this->one.toStringPolynomial(converted, &this->output->format)
      << "\\mod "
      << currentPolynomial.modulus()->toString(&this->output->format)
      << "\\)";
    }
    if (
      this->divisorFromCandidate(
        currentPolynomial.getValue(), "A", comments
      )
    ) {
      return true;
    }
    currentPolynomialPlusOne = currentPolynomial;
    currentPolynomialPlusOne += this->one;
    if (
      this->divisorFromCandidate(
        currentPolynomialPlusOne.getValue(), "A + 1", comments
      )
    ) {
      return true;
    }
    currentPolynomialMinusOne = currentPolynomial;
    currentPolynomialMinusOne -= this->one;
    if (
      this->divisorFromCandidate(
        currentPolynomialMinusOne.getValue(), "A - 1", comments
      )
    ) {
      return true;
    }
  }
  return false;
}

template <class Coefficient>
std::string PolynomialDivisionReport<Coefficient>::getDivisionLaTeXSlide() {
  STACK_TRACE("PolynomialDivisionReport::getDivisionLaTeXSlide");
  this->checkInitialization();
  std::stringstream out;
  List<Polynomial<Coefficient> >& remainders = this->intermediateRemainders;
  List<Polynomial<Coefficient> >& subtrahends = this->intermediateSubtractands;
  this->owner->format.monomialOrder =
  this->owner->polynomialOrder.monomialOrder;
  bool oneDivisor = (this->owner->basis.size == 1);
  this->allMonomials.clear();
  this->allMonomials.addOnTopNoRepetition(
    this->startingPolynomial.monomials
  );
  for (int i = 0; i < remainders.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(remainders[i].monomials);
  }
  for (int i = 0; i < subtrahends.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(subtrahends[i].monomials);
  }
  auto& basis = this->owner->basis;
  for (int i = 0; i < basis.size; i ++) {
    Polynomial<Coefficient>& current = basis[i].element;
    this->allMonomials.addOnTopNoRepetition(current.monomials);
  }
  for (int i = 0; i < this->owner->quotients.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(
      this->owner->quotients[i].monomials
    );
  }
  if (this->owner->remainderDivision.isEqualToZero()) {
    MonomialPolynomial constMon;
    constMon.makeOne();
    this->allMonomials.addOnTopNoRepetition(constMon);
  }
  this->allMonomials.quickSortDescending(
    &this->owner->polynomialOrder.monomialOrder
  );
  List<List<int> > dummyListList;
  List<int> dummyList;
  dummyListList.setSize(this->allMonomials.size);
  dummyList.initializeFillInObject(this->allMonomials.size, - 1);
  this->firstNonZeroIndicesPerIntermediateSubtracand.initializeFillInObject(
    subtrahends.size, 0
  );
  this->highlightMonsRemainders.initializeFillInObject(
    remainders.size, dummyListList
  );
  this->highlightMonsSubtrahends.initializeFillInObject(
    subtrahends.size, dummyListList
  );
  this->highlightMonsQuotients.initializeFillInObject(
    basis.size, dummyListList
  );
  this->highlightMonsDivisors.initializeFillInObject(
    basis.size, dummyListList
  );
  this->fcAnswerMonsRemainders.initializeFillInObject(
    remainders.size, dummyList
  );
  this->fcAnswerMonsSubtrahends.initializeFillInObject(
    subtrahends.size, dummyList
  );
  this->fcAnswerMonsQuotients.initializeFillInObject(basis.size, dummyList);
  this->fcAnswerMonsDivisors.initializeFillInObject(basis.size, dummyList);
  this->uncoverAllMonsRemainders.initializeFillInObject(remainders.size, 1);
  this->uncoverAllMonsSubtrahends.initializeFillInObject(
    subtrahends.size, 1
  );
  this->uncoverAllMonsQuotients.initializeFillInObject(basis.size, 1);
  this->uncoverAllMonsDivisors.initializeFillInObject(basis.size, 1);
  this->uncoverMonsFinalRemainder.initializeFillInObject(
    this->allMonomials.size, - 1
  );
  this->additionalHighlightFinalRemainder.initializeFillInObject(
    this->allMonomials.size, - 1
  );
  this->additionalHighlightRemainders.initializeFillInObject(
    this->allMonomials.size, dummyList
  );
  this->highlightAllMonsFinalRemainder = - 1;
  int currentSlideNumer = this->firstIndexLatexSlide + 1;
  for (int i = 0; i < remainders.size; i ++) {
    this->computeHighLightsFromRemainder(i, currentSlideNumer);
  }
  for (int i = 0; i < subtrahends.size; i ++) {
    this->firstNonZeroIndicesPerIntermediateSubtracand[i] =
    subtrahends[i].getIndexLeadingMonomial(
      nullptr, nullptr, &this->owner->polynomialOrder.monomialOrder
    );
  }
  this->owner->format.flagUseLatex = true;
  out << "\\renewcommand{\\arraystretch}{1.2}";
  out << "\\begin{longtable}{@{}c";
  for (int i = 0; i < this->allMonomials.size * 2; i ++) {
    out << "@{}c";
  }
  out << "}";
  if (!oneDivisor) {
    out
    << "{\\color{orange}\\textbf{Remainder:} }&"
    << this->getSpacedMonomialsWithHighlightLaTeX(
      this->owner->remainderDivision,
      nullptr,
      nullptr,
      &this->uncoverMonsFinalRemainder,
      &this->additionalHighlightFinalRemainder,
      - 1,
      true
    )
    << "\\\\\\hline";
  }
  if (!oneDivisor) {
    out
    << "\\textbf{Divisor(s)} &"
    << "\\multicolumn{"
    << this->allMonomials.size * 2
    << "}{c}{"
    << "\\alertNoH{"
    << this->uncoverAllMonsQuotients[0]
    << "}{"
    << "\\textbf{Quotient(s)}"
    << "}"
    << "}"
    << "\\\\";
  }
  for (int i = 0; i < this->owner->basis.size; i ++) {
    if (!oneDivisor) {
      out
      << this->getSpacedMonomialsWithHighlightLaTeX(
        this->owner->basis[i].element,
        &this->highlightMonsDivisors[i],
        &this->fcAnswerMonsDivisors[i],
        nullptr,
        nullptr,
        this->uncoverAllMonsDivisors[i],
        false
      );
    } else {
      out
      << "\\uncover<"
      << this->uncoverAllMonsQuotients[0]
      << "->{"
      << "\\alertNoH{"
      << this->uncoverAllMonsQuotients[0]
      << "}{"
      << "\\textbf{Quotient: }"
      << "}"
      << "}";
    }
    out << "&";
    out << "\\multicolumn{" << this->allMonomials.size * 2 << "}{c}{";
    out
    << this->getSpacedMonomialsWithHighlightLaTeX(
      this->owner->quotients[i],
      &this->highlightMonsQuotients[i],
      &this->fcAnswerMonsQuotients[i],
      nullptr,
      nullptr,
      this->uncoverAllMonsQuotients[i],
      false
    );
    out << "}";
    out << "\\\\";
    if (!oneDivisor) {
      out << "\\hline";
    }
  }
  out
  << " \\cline{2-"
  << this->allMonomials.size * 2 + 1
  << "}"
  << " \\cline{2-"
  << this->allMonomials.size * 2 + 1
  << "}";
  for (int i = 0; i < remainders.size; i ++) {
    if (i == 0) {
      if (oneDivisor) {
        out
        << "\\multicolumn{1}{c|}{"
        << this->getSpacedMonomialsWithHighlightLaTeX(
          this->owner->basis[0].element,
          &this->highlightMonsDivisors[0],
          &this->fcAnswerMonsDivisors[i],
          nullptr,
          nullptr,
          0,
          false
        )
        << "}";
      }
    } else {
      out << "\\uncover<" << this->uncoverAllMonsRemainders[i] << "->{";
    }
    if (i == remainders.size - 1 && i != 0 && oneDivisor) {
      out
      << "\\uncover<"
      << this->highlightAllMonsFinalRemainder
      << "->{"
      << "\\textbf{\\color{orange}Remainder: }"
      << "}";
    }
    out << "&";
    out
    << this->getSpacedMonomialsWithHighlightLaTeX(
      remainders[i],
      &this->highlightMonsRemainders[i],
      &this->fcAnswerMonsRemainders[i],
      nullptr,
      &this->additionalHighlightRemainders[i],
      this->uncoverAllMonsRemainders[i],
      true
    )
    << "\\\\";
    if (i < subtrahends.size) {
      out << "\\uncover<" << this->uncoverAllMonsSubtrahends[i] << "->{";
      out
      << "\\uncover<"
      << this->uncoverAllMonsSubtrahends[i] + 2
      << "->{\\alertNoH{"
      << this->uncoverAllMonsSubtrahends[i] + 2
      << ", "
      << this->uncoverAllMonsSubtrahends[i] + 3
      << "}{"
      << "$\\overline{\\phantom{A}}$"
      << "}}";
      out << "&";
      out
      << this->getSpacedMonomialsWithHighlightLaTeX(
        subtrahends[i],
        &this->highlightMonsSubtrahends[i],
        &this->fcAnswerMonsSubtrahends[i],
        nullptr,
        nullptr,
        this->uncoverAllMonsSubtrahends[i],
        true
      );
      out
      << "\\\\\\cline{"
      << this->firstNonZeroIndicesPerIntermediateSubtracand[i] * 2 + 2
      << "-"
      << this->allMonomials.size * 2 + 1
      << "}";
      out << "}";
    }
    if (i != 0) {
      out << "}";
    }
  }
  out << "\\end{longtable}";
  out << "\r\n";
  out << this->divisionLog.str();
  return out.str();
}

template <class Coefficient>
std::string PolynomialDivisionReport<Coefficient>::
getSpacedMonomialsWithHighlightLaTeX(
  const Polynomial<Coefficient>& polynomial,
  List<List<int> >* slidesToHighlightMon,
  List<int>* slidesToFcAnswer,
  List<int>* slidesToUncover,
  List<int>* slidesAdditionalHighlight,
  int slidesToUncoverAllMons,
  bool useColumnSeparator
) {
  STACK_TRACE(
    "PolynomialDivisionReport::"
    "getSpacedMonomialsWithHighlightLaTeX"
  );
  (void) slidesToUncoverAllMons;
  std::stringstream out;
  bool found = false;
  int countMons = 0;
  if (polynomial.isEqualToZero()) {
    if (useColumnSeparator) {
      for (int i = 0; i < this->allMonomials.size * 2 - 1; i ++) {
        out << "&";
      }
    }
    std::stringstream highlightHeadStream;
    std::stringstream highlightTailStream;
    MonomialPolynomial monomial;
    monomial.makeOne();
    int monIndex = this->allMonomials.getIndex(monomial);
    if (slidesAdditionalHighlight != nullptr && monIndex != - 1) {
      if ((*slidesAdditionalHighlight)[monIndex] > 0) {
        highlightHeadStream
        << "{ \\only<"
        << (*slidesAdditionalHighlight)[monIndex]
        << "->{\\color{orange}}";
        highlightTailStream << "}";
      }
    }
    if (slidesToFcAnswer != nullptr && monIndex != - 1) {
      if ((*slidesToFcAnswer)[monIndex] > 1) {
        highlightHeadStream
        << "\\fcAnswer{"
        << (*slidesToFcAnswer)[monIndex]
        << "}{";
        highlightTailStream << "}";
      }
    }
    out
    << "$ "
    << highlightHeadStream.str()
    << "0"
    << highlightTailStream.str()
    << "$";
    return out.str();
  }
  for (int i = 0; i < this->allMonomials.size; i ++) {
    int index = polynomial.monomials.getIndex(this->allMonomials[i]);
    if (index == - 1) {
      if (useColumnSeparator) {
        if (i != this->allMonomials.size - 1) {
          out << "&&";
        }
      }
      continue;
    }
    std::string highlightHead;
    std::string highlightTail;
    int fcAnswerSlide = - 1;
    if (slidesToFcAnswer != nullptr) {
      if (i < (*slidesToFcAnswer).size) {
        if ((*slidesToFcAnswer)[i] > 1) {
          fcAnswerSlide = (*slidesToFcAnswer)[i];
          if (slidesToHighlightMon != nullptr) {
            (*slidesToHighlightMon)[i].addOnTop(fcAnswerSlide);
          }
        }
      }
    }
    if (slidesToUncover != nullptr) {
      if ((*slidesToUncover)[i] > 1) {
        std::stringstream highlightStream;
        highlightStream << "\\uncover<" << (*slidesToUncover)[i] << "->{";
        highlightHead += highlightStream.str();
        highlightTail = "}" + highlightTail;
      }
    }
    if (slidesToHighlightMon != nullptr) {
      if ((*slidesToHighlightMon)[i].size > 0) {
        highlightHead +=
        "\\alertNoH{" + (*slidesToHighlightMon)[i].toStringCommaDelimited() +
        "}{";
        highlightTail = "}" + highlightTail;
      }
    }
    if (slidesAdditionalHighlight != nullptr) {
      if ((*slidesAdditionalHighlight)[i] > 0) {
        std::stringstream highlightStream;
        highlightStream
        << "{\\only<"
        << (*slidesAdditionalHighlight)[i]
        << "->{\\color{orange}}";
        highlightHead += highlightStream.str();
        highlightTail = "}" + highlightTail;
      }
    }
    countMons ++;
    std::string monWithSign =
    Polynomial<Coefficient>::getBlendCoefficientAndMonomial(
      polynomial[index],
      polynomial.coefficients[index],
      true,
      &this->owner->format
    );
    std::string sign = monWithSign.substr(0, 1);
    std::string monNoSign = monWithSign.substr(1);
    if (sign == "-" || found) {
      if (fcAnswerSlide != - 1) {
        out << "\\uncover<" << fcAnswerSlide << "->{";
      }
      out << highlightHead;
      out << "$" << sign << "$";
      out << highlightTail;
      if (fcAnswerSlide != - 1) {
        out << "}";
      }
    }
    found = true;
    if (useColumnSeparator) {
      out << "&";
    } else {
      out << " ";
    }
    if (fcAnswerSlide != - 1) {
      out << "\\fcAnswer{" << fcAnswerSlide << "}{";
    }
    out << highlightHead << "$" << monNoSign << "$" << highlightTail;
    if (fcAnswerSlide != - 1) {
      out << "}";
    }
    if (!useColumnSeparator) {
      out << " ";
    }
    if (useColumnSeparator) {
      if (i != this->allMonomials.size - 1) {
        out << "&";
      }
    }
  }
  if (countMons != polynomial.size()) {
    out << " Programming ERROR!";
  }
  return out.str();
}

template <class Coefficient>
void PolynomialDivisionReport<Coefficient>::computeHighLightsFromRemainder(
  int remainderIndex, int& currentSlideNumber
) {
  STACK_TRACE("PolynomialDivisionReport::computeHighLightsFromRemainder");
  this->checkInitialization();
  auto& basis = this->owner->basis;
  if (remainderIndex == 0) {
    for (int i = 0; i < this->allMonomials.size; i ++) {
      this->highlightMonsRemainders[remainderIndex][i].addOnTop(
        currentSlideNumber
      );
    }
    currentSlideNumber ++;
    for (int j = 0; j < basis.size; j ++) {
      for (int i = 0; i < this->allMonomials.size; i ++) {
        this->highlightMonsDivisors[j][i].addOnTop(currentSlideNumber);
      }
      currentSlideNumber ++;
    }
  }
  for (
    int i = 0; i < this->intermediateHighlightedMons[remainderIndex].size; i ++
  ) {
    int monomialIndex =
    this->allMonomials.getIndex(
      this->intermediateHighlightedMons[remainderIndex][i]
    );
    this->additionalHighlightRemainders[remainderIndex][monomialIndex] =
    currentSlideNumber;
    this->additionalHighlightFinalRemainder[monomialIndex] =
    currentSlideNumber;
    this->uncoverMonsFinalRemainder[monomialIndex] = currentSlideNumber;
    currentSlideNumber ++;
  }
  MonomialPolynomial constMon;
  constMon.makeOne();
  int zeroMonIndex = this->allMonomials.getIndex(constMon);
  if (this->intermediateRemainders[remainderIndex].isEqualToZero()) {
    this->additionalHighlightRemainders[remainderIndex][zeroMonIndex] =
    currentSlideNumber;
    this->additionalHighlightFinalRemainder[zeroMonIndex] = currentSlideNumber;
    currentSlideNumber ++;
  }
  if (remainderIndex == this->intermediateRemainders.size - 1) {
    for (int i = 0; i < basis.size; i ++) {
      this->uncoverAllMonsQuotients[i] = currentSlideNumber;
      for (int j = 0; j < this->allMonomials.size; j ++) {
        this->highlightMonsQuotients[i][j].addOnTop(currentSlideNumber);
      }
    }
    currentSlideNumber ++;
    this->highlightAllMonsFinalRemainder = currentSlideNumber;
  }
  if (remainderIndex >= this->intermediateSelectedDivisors.size) {
    return;
  }
  Polynomial<Coefficient>& currentRemainder =
  this->intermediateRemainders[remainderIndex];
  int indexCurrentDivisor = this->intermediateSelectedDivisors[remainderIndex];
  Polynomial<Coefficient>& currentDivisor = basis[indexCurrentDivisor].element;
  MonomialPolynomial divisorLeadingMonomial;
  int indexCurrentDivisorLeadingMoN =
  currentDivisor.getIndexLeadingMonomial(
    &divisorLeadingMonomial,
    nullptr,
    &this->owner->polynomialOrder.monomialOrder
  );
  int indexCurrentDivisorLeadingMonInAllMons =
  this->allMonomials.getIndex(divisorLeadingMonomial);
  MonomialPolynomial maxMonCurrentRemainder;
  Coefficient leadingCFCurrentRemainder;
  currentRemainder.getIndexLeadingMonomial(
    &maxMonCurrentRemainder,
    &leadingCFCurrentRemainder,
    &this->owner->polynomialOrder.monomialOrder
  );
  int indexCurrentRemainderLeadingMonInAllMons =
  this->allMonomials.getIndex(maxMonCurrentRemainder);
  this->highlightMonsDivisors[indexCurrentDivisor][
    indexCurrentDivisorLeadingMonInAllMons
  ].addOnTop(currentSlideNumber);
  this->highlightMonsRemainders[remainderIndex][
    indexCurrentRemainderLeadingMonInAllMons
  ].addOnTop(currentSlideNumber);
  if (remainderIndex == 0) {
    this->divisionLog << "$\\vphantom" << "{\\frac{x^1}{x^1}}$";
  }
  FormatExpressions& format = this->owner->format;
  this->divisionLog
  << "\\only<"
  << currentSlideNumber
  << ","
  << currentSlideNumber + 1
  << "| handout:0>{Divide "
  << "\\alertNoH{"
  << currentSlideNumber
  << ","
  << currentSlideNumber + 1
  << "}{"
  << "$"
  << currentRemainder.getBlendCoefficientAndMonomial(
    maxMonCurrentRemainder, leadingCFCurrentRemainder, false, &format
  )
  << "$ "
  << "}"
  << " by "
  << "\\alertNoH{"
  << currentSlideNumber
  << ","
  << currentSlideNumber + 1
  << "}{"
  << "$"
  << currentRemainder.getBlendCoefficientAndMonomial(
    currentDivisor.monomials[indexCurrentDivisorLeadingMoN],
    currentDivisor.coefficients[indexCurrentDivisorLeadingMoN],
    false,
    &format
  )
  << "$"
  << "}."
  << "}";
  currentSlideNumber ++;
  this->highlightMonsDivisors[indexCurrentDivisor][
    indexCurrentDivisorLeadingMonInAllMons
  ].addOnTop(currentSlideNumber);
  this->highlightMonsRemainders[remainderIndex][
    indexCurrentRemainderLeadingMonInAllMons
  ].addOnTop(currentSlideNumber);
  int indexCurrentQuotientMonInAllMons =
  this->allMonomials.getIndex(
    this->intermediateHighestMonDivHighestMon[remainderIndex]
  );
  Polynomial<Coefficient>& currentQuotient =
  this->owner->quotients[indexCurrentDivisor];
  int indexCurrentQuotientMoN =
  currentQuotient.monomials.getIndex(
    this->intermediateHighestMonDivHighestMon[remainderIndex]
  );
  this->fcAnswerMonsQuotients[indexCurrentDivisor][
    indexCurrentQuotientMonInAllMons
  ] =
  currentSlideNumber;
  currentSlideNumber ++;
  this->highlightMonsQuotients[indexCurrentDivisor][
    indexCurrentQuotientMonInAllMons
  ].addOnTop(currentSlideNumber);
  for (int i = 0; i < currentDivisor.size(); i ++) {
    this->highlightMonsDivisors[indexCurrentDivisor][
      this->allMonomials.getIndex(currentDivisor[i])
    ].addOnTop(currentSlideNumber);
  }
  this->uncoverAllMonsSubtrahends[remainderIndex] = currentSlideNumber;
  this->divisionLog
  << "\\only<"
  << currentSlideNumber
  << ", "
  << currentSlideNumber + 1
  << "| handout:0>{Multiply "
  << "\\alertNoH{"
  << currentSlideNumber
  << ", "
  << currentSlideNumber + 1
  << "}{$"
  << currentQuotient.getBlendCoefficientAndMonomial(
    currentQuotient.monomials[indexCurrentQuotientMoN],
    currentQuotient.coefficients[indexCurrentQuotientMoN],
    false,
    &format
  )
  << "$}"
  << " by divisor. "
  << "}";
  currentSlideNumber ++;
  this->highlightMonsQuotients[indexCurrentDivisor][
    indexCurrentQuotientMonInAllMons
  ].addOnTop(currentSlideNumber);
  for (int i = 0; i < currentDivisor.size(); i ++) {
    this->highlightMonsDivisors[indexCurrentDivisor][
      this->allMonomials.getIndex(currentDivisor[i])
    ].addOnTop(currentSlideNumber);
  }
  if (
    this->fcAnswerMonsSubtrahends[remainderIndex].size !=
    this->allMonomials.size
  ) {
    this->fcAnswerMonsSubtrahends[remainderIndex].initializeFillInObject(
      this->allMonomials.size, - 1
    );
  }
  for (
    int i = 0; i < this->intermediateSubtractands[remainderIndex].size(); i ++
  ) {
    this->fcAnswerMonsSubtrahends[remainderIndex][
      this->allMonomials.getIndex(
        this->intermediateSubtractands[remainderIndex][i]
      )
    ] =
    currentSlideNumber;
  }
  currentSlideNumber ++;
  for (
    int i = 0; i < this->intermediateRemainders[remainderIndex].size(); i ++
  ) {
    this->highlightMonsRemainders[remainderIndex][
      this->allMonomials.getIndex(
        this->intermediateRemainders[remainderIndex][i]
      )
    ].addOnTop(currentSlideNumber);
  }
  for (
    int i = 0; i < this->intermediateSubtractands[remainderIndex].size(); i ++
  ) {
    this->highlightMonsSubtrahends[remainderIndex][
      this->allMonomials.getIndex(
        this->intermediateSubtractands[remainderIndex][i]
      )
    ].addOnTop(currentSlideNumber);
  }
  this->uncoverAllMonsRemainders[remainderIndex + 1] = currentSlideNumber;
  this->divisionLog
  << "\\only<"
  << currentSlideNumber
  << ", "
  << currentSlideNumber + 1
  << "| handout:0>{subtract last two polynomials.}";
  currentSlideNumber ++;
  for (
    int i = 0; i < this->intermediateRemainders[remainderIndex].size(); i ++
  ) {
    this->highlightMonsRemainders[remainderIndex][
      this->allMonomials.getIndex(
        this->intermediateRemainders[remainderIndex][i]
      )
    ].addOnTop(currentSlideNumber);
  }
  for (
    int i = 0; i < this->intermediateSubtractands[remainderIndex].size(); i ++
  ) {
    this->highlightMonsSubtrahends[remainderIndex][
      this->allMonomials.getIndex(
        this->intermediateSubtractands[remainderIndex][i]
      )
    ].addOnTop(currentSlideNumber);
  }
  if (remainderIndex + 1 >= this->intermediateRemainders.size) {
    global.fatal
    << "Something is wrong: not enough intermediate remainders. "
    << global.fatal;
  }
  for (
    int i = 0; i < this->intermediateRemainders[remainderIndex + 1].size(); i
    ++
  ) {
    this->fcAnswerMonsRemainders[remainderIndex + 1][
      this->allMonomials.getIndex(
        this->intermediateRemainders[remainderIndex + 1][i]
      )
    ] =
    currentSlideNumber;
  }
  if (this->intermediateRemainders[remainderIndex + 1].isEqualToZero()) {
    this->fcAnswerMonsRemainders[remainderIndex + 1][zeroMonIndex] =
    currentSlideNumber;
  }
  currentSlideNumber ++;
}

#endif // header_math_general_polynomial_computations_advanced_implementation_ALREADY_INCLUDED
