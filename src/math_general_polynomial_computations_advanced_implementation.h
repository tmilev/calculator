// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfImplementationHeaderPolyComputationsAdvanced_already_included
#define vpfImplementationHeaderPolyComputationsAdvanced_already_included

#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_extra_algebraic_numbers.h"

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::wrapUpGroebnerOnExceedingComputationLimit(
  List<Polynomial<Coefficient> >& output
) {
  this->getBasis(output);
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
  output.setSize(this->theBasis.size);
  for (int i = 0; i < this->theBasis.size; i ++) {
    output[i] = this->theBasis[i].element;
  }
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::transformToReducedBasis(
  List<Polynomial<Coefficient> >& inputOutpuT, int upperLimitPolyComputations
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::transformToReducedBasis");
  this->initializeForGroebnerComputation();
  this->basisCandidates = inputOutpuT;
  ProgressReport report;
  if (this->flagDoProgressReport) {
    std::stringstream reportStream;
    reportStream << "Transforming to reduced (NON-Groebner) basis a system of "
    << this->basisCandidates.size << " elements (at the start). ";
    report.report(reportStream.str());
  }
  int oldMaxNumGBCompos = this->maximumPolynomialComputations;
  this->maximumPolynomialComputations = upperLimitPolyComputations;
  this->addAndReducePolynomials();
  this->maximumPolynomialComputations = oldMaxNumGBCompos;
  if (upperLimitPolyComputations > 0) {
    if (this->numberPolynomialDivisions > upperLimitPolyComputations) {
      return this->wrapUpGroebnerOnExceedingComputationLimit(inputOutpuT);
    }
  }
  return this->wrapUpOnGroebnerBasisSuccess(inputOutpuT);
}

template <class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::toStringStatusGroebnerBasisTransformation() {
  std::stringstream out;
  out << "Transforming to Groebner basis a system of "
  << this->basisCandidates.size << " elements: "
  << this->basisCandidates.toStringCommaDelimited(&this->theFormat);
  return out.str();
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::generateOneSymmetricDifferenceCandidate(
  GroebnerBasisComputation::BasisElement& left,
  GroebnerBasisComputation::BasisElement& right
) {
  int numberOfVariables = MathRoutines::maximum(
    left.leadingMonomial.minimalNumberOfVariables(),
    right.leadingMonomial.minimalNumberOfVariables()
  );
  MonomialP leftShift;
  MonomialP rightShift;
  for (int k = 0; k < numberOfVariables; k ++) {
    if (left.leadingMonomial(k) > right.leadingMonomial(k)) {
      rightShift.setVariable(k, left.leadingMonomial(k) - right.leadingMonomial(k));
     leftShift.setVariable(k, 0);
    } else {
      leftShift.setVariable(k, right.leadingMonomial(k) - left.leadingMonomial(k));
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
void GroebnerBasisComputation<Coefficient>::generateSymmetricDifferenceCandidates() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::generateSymmetricDifferenceCandidates");
  ProgressReport reportProgress("Groebner basis report");
  this->numberOfSymmetricDifferenceRounds ++;
  for (int i = 0; i < this->theBasis.size; i ++) {
    for (int j = i + 1; j < this->theBasis.size && i < this->theBasis.size; j ++) {
      if (reportProgress.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream << "Computing S-poly of depth " << this->numberOfSymmetricDifferenceRounds
        << ". Taking s-difference of indices " << i + 1 << " and " << j + 1
        << " out of " << this->theBasis.size
        << ".<br>Before proceding to adjoin candidates, "
        << "I have " << this->basisCandidates.size
        << " candidates and basis of size "
        << this->theBasis.size << ".";
        reportProgress.report(reportStream.str());
      }
      this->generateOneSymmetricDifferenceCandidate(
        this->theBasis[i], this->theBasis[j]
      );
    }
  }
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::transformToReducedGroebnerBasis(
  List<Polynomial<Coefficient> >& inputOutput
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::transformToReducedGroebnerBasis");
  this->initializeForGroebnerComputation();
  this->basisCandidates = inputOutput;
  ProgressReport reportStart("Groebner basis start");
  if (this->flagDoProgressReport) {
    reportStart.report(this->toStringStatusGroebnerBasisTransformation());
  }
  this->addAndReducePolynomials();
  if (this->limitsExceeded()) {
    return this->wrapUpGroebnerOnExceedingComputationLimit(inputOutput);
  }
  if (this->theBasis.size == 1) {
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

template<class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::toStringPolynomialBasisStatus() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::toStringPolynomialBasisStatus");
  std::stringstream out;
  out << "There are " << this->basisCandidates.size
  << " candidates left to adjoin. <br>The current polynomial basis has "
  << theBasis.size << " elements. "
  << "<br>Number of polynomial divisions: "
  << this->numberPolynomialDivisions
  << " with a limit of: " << this->maximumPolynomialComputations << " computations. "
  << "The basis follows.";
  for (int i = 0; i < this->theBasis.size; i ++) {
    out << "<br>" << this->theBasis[i].toString(&this->theFormat);
  }
  out << "<br>There are " << this->basisCandidates.size << " candidates. ";
  for (int i = 0; i < this->basisCandidates.size; i ++) {
    out << "<br>" << this->basisCandidates[i].toString(&this->theFormat);
  }
  return out.str();
}

template<class Coefficient>
bool GroebnerBasisComputation<Coefficient>::limitsExceeded() const {
  if (this->maximumPolynomialComputations <= 0) {
    return false;
  }
  return this->numberPolynomialDivisions > this->maximumPolynomialComputations;
}

template<class Coefficient>
bool GroebnerBasisComputation<Coefficient>::addAndReduceOnePolynomial() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::addAndReduceOnePolynomial");
  if (this->basisCandidates.size == 0) {
    return true;
  }
  this->remainderDivisionByBasis(
    *this->basisCandidates.lastObject(), this->remainderDivision
  );
  if (this->limitsExceeded()) {
    return false;
  }
  ProgressReport theReport;
  if (this->flagDoProgressReport) {
    std::stringstream reportStream;
    reportStream << "Adding remainder: "
    << this->remainderDivision.toString(&this->theFormat);
    theReport.report(reportStream.str());
  }
  this->basisCandidates.removeLastObject();
  return this->addRemainderToBasis();
}

template<class Coefficient>
bool GroebnerBasisComputation<Coefficient>::addAndReducePolynomials() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::addAndReducePolynomials");
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
  MacroRegisterFunctionWithName("GroebnerBasisComputation::minimalNumberOfVariables");
  int result = 0;
  for (int i = 0; i < this->theBasis.size; i ++) {
    Polynomial<Coefficient>& current = this->theBasis[i].element;
    MathRoutines::maximum(current.minimalNumberOfVariables(), result);
  }
  return result;
}

template <class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::toStringLetterOrder(bool addDollars) const {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::toStringLetterOrder");
  std::stringstream out;
  int numVars = this->minimalNumberOfVariables();
  List<MonomialP> variables;
  out << "Variable name(s), ascending order: ";
  variables.setSize(numVars);
  for (int i = 0; i < variables.size; i ++) {
    variables[i].makeEi(i, 1);
  }
  variables.quickSortAscending(&this->thePolynomialOrder.monomialOrder);
  FormatExpressions tempFormat = this->theFormat;
  if (addDollars) {
    out << "$";
  }
  for (int i = 0; i < numVars; i ++) {
    out << variables[i].toString(&tempFormat);
    if (i != numVars - 1) {
      out << ", ";
    }
  }
  if (addDollars) {
    out << "$";
  }
  if (numVars > 1) {
    out << ". The implied lexicographic order runs in the opposite direction: ";
    if (addDollars) {
      out << "$";
    }
    for (int i = 0; i < numVars; i ++) {
      out << variables[i].toString(&tempFormat);
      if (i != numVars - 1) {
        out << " > ";
      }
    }
    if (addDollars) {
      out << "$";
    }
  }
  return out.str();
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::oneDivisonSubStepWithBasis(
  Polynomial<Coefficient>& remainder,
  const MonomialP& leadingMonomial,
  const Coefficient& leadingCoefficient,
  int index,
  ProgressReport* theReport
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::oneDivisonSubStepWithBasis");
  const MonomialP& leadingMonomialBasis = this->theBasis[index].leadingMonomial;
  const Coefficient& leadingCoefficientBasis = this->theBasis[index].leadingCoefficient;
  MonomialP quotientMonomial = leadingMonomial;
  quotientMonomial /= leadingMonomialBasis;
  if (!quotientMonomial.hasPositiveOrZeroExponents()) {
    global.fatal << "This is a programming error: the pivot monomial "
    << "in the polynomial division algorithm has negative exponent(s). "
    << "This is not allowed. " << global.fatal;
  }
  if (this->flagDoLogDivision) {
    this->divisionReport.getElement().intermediateHighestMonDivHighestMon.addOnTop(quotientMonomial);
    this->divisionReport.getElement().intermediateSelectedDivisors.addOnTop(index);
  }
  Polynomial<Coefficient> buffer;
  buffer = this->theBasis[index].element;
  Coefficient quotientCoefficient = leadingCoefficient;
  quotientCoefficient /= leadingCoefficientBasis;
  if (this->flagDoLogDivision) {
    this->divisionReport.getElement().intermediateCoeffs.addOnTop(quotientCoefficient);
  }
  buffer.multiplyBy(quotientMonomial, quotientCoefficient);
  if (this->flagStoreQuotients) {
    this->theQuotients[index].addMonomial(quotientMonomial, quotientCoefficient);
  }
  if (this->flagDoLogDivision) {
    this->divisionReport.getElement().intermediateSubtractands.addOnTop(buffer);
  }
  if (this->flagDoProgressReport && theReport != nullptr) {
    std::stringstream out;
    out << "Monomial operation count: " << this->numberMonomialOperations;
    if (this->maximumPolynomialComputations > 0) {
      out << ", with a limit of " << this->maximumPolynomialComputations;
    }
    out << ".\n";
    out << "<br>Number of intermediate remainders: "
    << this->numberOfIntermediateRemainders << "\n<br>Highest remainder monomial: "
    << leadingMonomial.toString()
    << ".\n<br>"
    << "Dividing working remainder monomial " << leadingMonomialBasis.toString()
    << " by the leading monomial " << leadingMonomialBasis.toString()
    << " of basis element: " << index + 1
    << " out of " << this->theBasis.size << "\n<br>"
    << remainder.size() << " monomials in current remainder.";
    theReport->report(out.str());
  }
  remainder -= buffer;
  this->numberMonomialOperations += buffer.size();
  if (this->flagDoLogDivision) {
    this->divisionReport.getElement().intermediateRemainders.addOnTop(remainder);
    List<MonomialP> empty;
    this->divisionReport.getElement().intermediateHighlightedMons.addOnTop(empty);
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
  MacroRegisterFunctionWithName("GroebnerBasisComputation::oneDivisonStepWithBasis");
  MonomialP highestMonomial;
  Coefficient leadingCoefficient;
  int indexLeadingMonomial = currentRemainder.getIndexLeadingMonomial(
    &highestMonomial,
    &leadingCoefficient,
    &this->thePolynomialOrder.monomialOrder
  );
  if (indexLeadingMonomial == - 1) {
    // Remainder is zero.
    return false;
  }
  for (int i = 0; i < this->theBasis.size; i ++) {
    if (i == basisIndexToIgnore) {
      continue;
    }
    if (!highestMonomial.isDivisibleBy(this->theBasis[i].leadingMonomial)) {
      continue;
    }
    this->numberOfIntermediateRemainders ++;
    this->oneDivisonSubStepWithBasis(
      currentRemainder,
      highestMonomial,
      leadingCoefficient,
      i,
      report
    );
    return true;
  }
  remainderResult.addMonomial(highestMonomial, leadingCoefficient);
  if (this->flagDoLogDivision) {
    (*this->divisionReport.getElement().intermediateHighlightedMons.lastObject()).addOnTop(highestMonomial);
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
  MacroRegisterFunctionWithName("GroebnerBasisComputation::remainderDivisionByBasis");
  if (this->flagDoLogDivision) {
    this->divisionReport.getElement().owner = this;
  }
  this->numberPolynomialDivisions ++;
  if (this->flagStoreQuotients) {
    this->theQuotients.setSize(this->theBasis.size);
    for (int i = 0; i < this->theQuotients.size; i ++) {
      this->theQuotients[i].makeZero();
    }
  }
  ProgressReport reportHeader;
  ProgressReport reportMain;
  ProgressReport reportRemainderSoFar;
  reportRemainderSoFar.ticksPerReport = 50;
  if (this->flagDoProgressReport) {
    std::stringstream reportStream;
    reportStream << "Computing remainder of division of "
    << input.toString(&this->theFormat)
    << " by " << this->theBasis.toString(&this->theFormat);
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

    this->divisionReport.getElement().intermediateRemainders.addOnTop(currentRemainder);
    this->divisionReport.getElement().intermediateHighlightedMons.setSize(1);
    this->divisionReport.getElement().intermediateHighlightedMons.lastObject()->setSize(0);
  }
  while (!currentRemainder.isEqualToZero()) {
    this->numberOfIntermediateRemainders = 0;
    while (this->oneDivisonStepWithBasis(
      currentRemainder,
      outputRemainder,
      basisIndexToIgnore,
      &reportMain
    )) {
    }
    if (this->flagDoProgressReport) {
      std::stringstream out;
      out << "Last extracted remainder monomial took: "
      << this->numberOfIntermediateRemainders << " polynomial divisions.\n"
      << "<br>" << currentRemainder.size()
      << " monomials in the working remainder.";
      out << "<br>" << outputRemainder.size()
      << " monomials in the final output remainder.";
      out << "<br>Current basis: "
      << this->theBasis.toStringCommaDelimited(&this->theFormat);
      out << "<br>Current remainder: "
      << currentRemainder.toString(&this->theFormat);
      reportRemainderSoFar.report(out.str());
    }
  }
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::addRemainderToBasis() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::addRemainderToBasis");
  if (this->remainderDivision.isEqualToZero()) {
    return true;
  }
  this->flagFoundNewBasisElements = true;
  this->remainderDivision.scaleNormalizeLeadingMonomial(
    &this->thePolynomialOrder.monomialOrder
  );
  MonomialP newLeadingMonomial;
  this->remainderDivision.getIndexLeadingMonomial(
    &newLeadingMonomial,
    nullptr,
    &this->thePolynomialOrder.monomialOrder
  );
  int indexToAddAt = 0;
  for (; indexToAddAt < this->theBasis.size; indexToAddAt ++) {
    MonomialP& otherLeadingMonomial = this->theBasis[indexToAddAt].leadingMonomial;
    if (this->thePolynomialOrder.monomialOrder.greaterThan(
      otherLeadingMonomial, newLeadingMonomial
    )) {
      break;
    }
  }
  for (int i = this->theBasis.size - 1; i >= indexToAddAt; i --) {
    this->basisCandidates.addOnTop(this->theBasis[i].element);
  }
  this->theBasis.setSize(indexToAddAt);
  this->addBasisElementNoReduction(this->remainderDivision);
  return true;
}

template <class Coefficient>
PolynomialDivisionReport<Coefficient>::PolynomialDivisionReport() {
  this->highlightAllMonsFinalRemainder = - 1;
  this->firstIndexLatexSlide = - 1;
  this->owner = nullptr;
}

template<class Coefficient>
PolynomialSystem<Coefficient>::PolynomialSystem() {
  this->theAlgebraicClosure = nullptr;
  this->maximumSerreSystemComputationsPreferred = 0;
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

  this->maximumPolynomialComputations = 0;
  this->maximumBasisReductionComputations = 10000;

  this->flagDoLogDivision = false;
  this->flagStoreQuotients = false;

  this->flagFoundNewBasisElements = false;
  this->numberOfSymmetricDifferenceRounds = 0;
}

template <class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::BasisElement::toString(
  FormatExpressions* theFormat
) const {
  std::stringstream out;
  out << "[" << this->leadingMonomial.toString(theFormat) << "] "
  << this->element.toString(theFormat);
  return out.str();
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::addBasisElementNoReduction(
  const Polynomial<Coefficient>& input
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::addBasisElementNoReduction");
  if (this->thePolynomialOrder.monomialOrder.leftGreaterThanRight == nullptr) {
    global.fatal << "Uninitialized monomial order. " << global.fatal;
  }
  if (input.isEqualToZero()) {
    return;
  }
  this->theBasis.setSize(this->theBasis.size + 1);
  GroebnerBasisComputation<Coefficient>::BasisElement& last = *this->theBasis.lastObject();
  last.element = input;
  last.element.getIndexLeadingMonomial(
    &last.leadingMonomial,
    &last.leadingCoefficient,
    &this->thePolynomialOrder.monomialOrder
  );
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::initializeForGroebnerComputation() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::initializeForGroebnerComputation");
  this->basisCandidates.setSize(0);
  this->theBasis.setSize(0);
  this->numberPolynomialDivisions = 0;
  this->numberMonomialOperations = 0;
}

template<class Coefficient>
void GroebnerBasisComputation<Coefficient>::checkConsistency() {
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::getSubstitutionFromPartialSolutionSerreLikeSystem(
  PolynomialSubstitution<Coefficient>& outputSub
) {
  MacroRegisterFunctionWithName("PolynomialSystem::getSubstitutionFromPartialSolutionSerreLikeSystem");
  outputSub.makeIdentitySubstitution(this->systemSolution.size);
  for (int i = 0; i < this->solutionsFound.cardinalitySelection; i ++) {
    outputSub[this->solutionsFound.elements[i]] = this->systemSolution[
      this->solutionsFound.elements[i]
    ];
  }
}

template <class Coefficient>
bool PolynomialSystem<Coefficient>::getOneVariablePolynomialSolution(
  const Polynomial<Coefficient>& thePoly, Coefficient& outputSolution
) {
  AlgebraicNumber number;
  if (this->theAlgebraicClosure == 0) {
    return false;
  }
  if (!number.constructFromMinimalPolynomial(
    thePoly, *this->theAlgebraicClosure, nullptr
  )) {
    return false;
  }
  outputSolution = number;
  return true;
}

template <class Coefficient>
bool PolynomialSystem<Coefficient>::hasImpliedSubstitutions(
  List<Polynomial<Coefficient> >& inputSystem, PolynomialSubstitution<Coefficient>& outputSub
) {
  int numVars = this->systemSolution.size;
  MonomialP tempM;
  Polynomial<Coefficient> tempP;
  Coefficient theCF;
  for (int i = 0; i < inputSystem.size; i ++) {
    tempP = inputSystem[i];
    for (int j = 0; j < numVars; j ++) {
      tempM.makeEi(j, 1, numVars);
      int indexTempM = tempP.monomials.getIndex(tempM);
      if (indexTempM == - 1) {
        continue;
      }
      theCF = tempP.coefficients[indexTempM];
      tempP.subtractMonomial(tempM, theCF);
      bool isGood = true;
      for (int k = 0; k < tempP.size(); k ++) {
        if (!(tempP[k](j) == 0)) {
          isGood = false;
          tempP.addMonomial(tempM, theCF);
          break;
        }
      }
      if (!isGood) {
        continue;
      }
      outputSub.makeIdentitySubstitution(numVars);
      outputSub[j] = tempP;
      theCF *= - 1;
      outputSub[j] /= theCF;
      return true;
    }
    int oneVarIndex;
    if (tempP.isOneVariableNonConstantPolynomial(&oneVarIndex)) {
      if (this->flagUsingAlgebraicClosure && this->theAlgebraicClosure != 0) {
        if (this->getOneVariablePolynomialSolution(tempP, theCF)) {
          outputSub.makeIdentitySubstitution(numVars);
          outputSub[oneVarIndex].makeConstant(theCF);
          //check our work:
          tempP.substitution(outputSub);
          if (!tempP.isEqualToZero()) {
            global.fatal << "I was solving the polynomial equation " << inputSystem[i].toString()
            << ", which resulted in the substitution " << outputSub.toString()
            << ". However, after carrying out the substitution in the polynomial, I got " << tempP.toString() << ". "
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
int GroebnerBasisComputation<Coefficient>::getNumberOfEquationsThatWouldBeLinearIfISubstitutedVariable(
  int theVarIndex, List<Polynomial<Coefficient> >& input
) {
  int result = 0;
  for (int i = 0; i < input.size; i ++) {
    Rational degExcludingVar = 0;
    for (int j = 0; j < input[i].size(); j ++) {
      degExcludingVar = MathRoutines::maximum(input[i][j].totalDegree() - input[i][j](theVarIndex), degExcludingVar);
    }
    if (degExcludingVar < 2) {
      result ++;
    }
  }
  return result;
}

template <class Coefficient>
int PolynomialSystem<Coefficient>::getPreferredSerreSystemSubstitutionIndex(
  List<Polynomial<Coefficient> >& inputSystem
) {
  Selection varSel;
  this->getVariablesToSolveFor(inputSystem, varSel);
  if (varSel.cardinalitySelection == 0) {
    return - 1;
  }
  int championIndex = varSel.elements[0];
  int championImprovement = GroebnerBasisComputation<Coefficient>::getNumberOfEquationsThatWouldBeLinearIfISubstitutedVariable(
    championIndex, inputSystem
  );
  for (int i = 1; i < varSel.cardinalitySelection; i ++) {
    if (
      GroebnerBasisComputation<Coefficient>::getNumberOfEquationsThatWouldBeLinearIfISubstitutedVariable(
        varSel.elements[i], inputSystem
      ) > championImprovement
    ) {
      championIndex = varSel.elements[i];
      championImprovement = GroebnerBasisComputation<Coefficient>::getNumberOfEquationsThatWouldBeLinearIfISubstitutedVariable(
        varSel.elements[i], inputSystem
      );
    }
  }
  return championIndex;
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::backSubstituteIntoSinglePolynomial(
  Polynomial<Coefficient>& thePoly, int theIndex, PolynomialSubstitution<Coefficient>& theFinalSub
) {
  MacroRegisterFunctionWithName("PolynomialSystem::backSubstituteIntoSinglePolynomial");
  Polynomial<Coefficient> tempP;
  tempP.makeMonomial(theIndex, 1, 1);
  if (thePoly == tempP) {
    return;
  }
  thePoly.substitution(theFinalSub);
  bool changed = false;
  for (int i = 0; i < thePoly.size(); i ++) {
    for (int j = 0; j < thePoly[i].minimalNumberOfVariables(); j ++) {
      if (thePoly[i](j) != 0) {
        if (!this->solutionsFound.selected[j]) {
          this->setSerreLikeSolutionIndex(j, 0);
        } else {
          if (this->systemSolution[j] != 0) {
            global.fatal << "This is a programming error: variable index " << j + 1
            << " is supposed to be a free parameter, i.e., be set to zero, but "
            << "instead it has a non-zero value. " << global.fatal;
          }
        }
        theFinalSub[j] = 0;
        changed = true;
      }
    }
  }
  if (changed) {
    thePoly.substitution(theFinalSub);
  }
  Coefficient tempCF;
  if (!thePoly.isConstant(&tempCF)) {
    global.fatal << "\n<br>\nThis is a programming error: after carrying all implied substitutions "
    << "the polynomial is not a constant, rather equals "
    << thePoly.toString() << ". " << global.fatal;
  }
  theFinalSub[theIndex] = tempCF;
  this->setSerreLikeSolutionIndex(theIndex, tempCF);
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::backSubstituteIntoPolynomialSystem(
  List<PolynomialSubstitution<Coefficient> >& theImpliedSubs
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::backSubstituteIntoPolynomialSystem");
  PolynomialSubstitution<Coefficient> FinalSub;
  this->getSubstitutionFromPartialSolutionSerreLikeSystem(FinalSub);
  for (int i = theImpliedSubs.size - 1; i >= 0; i --) {
    for (int j = 0; j < theImpliedSubs[i].size; j ++) {
      this->backSubstituteIntoSinglePolynomial(theImpliedSubs[i][j], j, FinalSub);
    }
  }
}

template <class Coefficient>
int PolynomialSystem<Coefficient>::getNumberOfVariablesToSolveFor(const List<Polynomial<Coefficient> >& input) {
  Selection theSel;
  PolynomialSystem<Coefficient>::getVariablesToSolveFor(input, theSel);
  return theSel.cardinalitySelection;
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::getVariablesToSolveFor(const List<Polynomial<Coefficient> >& input, Selection& output) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::getVariablesToSolveFor");
  int NumVars = 0;
  for (int i = 0; i < input.size; i ++) {
    NumVars = MathRoutines::maximum(NumVars, input[i].minimalNumberOfVariables());
  }
  output.initialize(NumVars);
  for (int i = 0; i < input.size && output.cardinalitySelection < output.maximumSize; i ++) {
    for (int j = 0; j < input[i].size() && output.cardinalitySelection < output.maximumSize; j ++) {
      for (int k = 0; k < input[i][j].minimalNumberOfVariables() && output.cardinalitySelection < output.maximumSize; k ++) {
        if (input[i][j](k) != 0) {
          output.addSelectionAppendNewIndex(k);
        }
      }
    }
  }
}

template <class Coefficient>
bool PolynomialSystem<Coefficient>::isContradictoryReducedSystem(const List<Polynomial<Coefficient> >& input) {
  if (input.size == 1) {
    if (input[0].isConstant()) {
      return true;
    }
  }
  return false;
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::polynomialSystemSolutionSimplificationPhase(
  List<Polynomial<Coefficient> >& inputSystem
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::polynomialSystemSolutionSimplificationPhase");
  ProgressReport theReport1, theReport2, theReport3;
  if (this->groebner.flagDoProgressReport) {
    std::stringstream reportStream;
    reportStream << "Reducing input system without use of heuristics.";
    theReport1.report(reportStream.str());
  }
  bool changed = true;
  PolynomialSubstitution<Coefficient> theSub;
  this->impliedSubstitutions.setSize(0);
  this->impliedSubstitutions.reserve(inputSystem.size);
//  int startingMaxNumSerreSystemComputations = this->MaxNumSerreSystemComputations;
  while (changed) {
    this->groebner.numberPolynomialDivisions = 0;
    List<Polynomial<Coefficient> > oldSystem = inputSystem;
    bool success = this->groebner.transformToReducedBasis(inputSystem, this->groebner.maximumBasisReductionComputations);
    if (!success) {
      inputSystem = oldSystem;
    } else {
      oldSystem = inputSystem;
    }
    if (this->groebner.flagDoProgressReport) {
      std::stringstream reportStream;
      reportStream << "Attempting to transform system to a groebner basis... ";
      theReport2.report(reportStream.str());
    }
    if (success && inputSystem.size > 0) {
      this->groebner.numberPolynomialDivisions = 0;
      success = this->groebner.transformToReducedGroebnerBasis(inputSystem);
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
        reportStream << "done, basis has " << inputSystem.size << " elements. ";
      } else {
        reportStream << "not successful: computation is too large. In the process I transformed the "
        << "starting system to one with " << inputSystem.size << " elements.";
      }
      theReport2.report(reportStream.str());
    }
    this->numberOfSerreSystemComputations += this->groebner.numberPolynomialDivisions;
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
          this->backSubstituteIntoPolynomialSystem(this->impliedSubstitutions);
          return;
        }
      }
    }
    changed = this->hasImpliedSubstitutions(inputSystem, theSub);
    if (changed) {
      if (this->groebner.flagDoProgressReport) {
        std::stringstream reportStream;
        reportStream << "Found implied substitutions.<br>"
        << this->toStringImpliedSubstitutions();
        theReport3.report(reportStream.str());
      }
      this->impliedSubstitutions.addOnTop(theSub);
      for (int i = 0; i < inputSystem.size; i ++) {
        inputSystem[i].substitution(theSub);
      }
    }
  }
}

template <class Coefficient>
bool PolynomialSystem<Coefficient>::shouldReport() {
  return this->groebner.flagDoProgressReport;
}

template <class Coefficient>
FormatExpressions& PolynomialSystem<Coefficient>::format() {
  return this->groebner.theFormat;
}

template <class Coefficient>
std::string PolynomialSystem<Coefficient>::toStringImpliedSubstitutions() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::toStringImpliedSubstitutions");
  if (this->impliedSubstitutions.size == 0) {
    return "";
  }
  std::stringstream out;
  out << "Implied subs: ";
  for (int i = 0; i < this->impliedSubstitutions.size; i ++) {
    for (int j = 0; j < this->impliedSubstitutions[i].size; j ++) {
      int letterIndex = - 1;
      if (this->impliedSubstitutions[i][j].isOneLetterFirstDegree(&letterIndex)) {
        if (letterIndex == j) {
          continue;
        }
      }
      out << "<br>" << (MonomialP(j)).toString(&this->format()) << "="
      << this->impliedSubstitutions[i][j].toString(&this->format()) << "; ";
    }
  }
  return out.str();
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::initializeForSystemSolution() {
  MacroRegisterFunctionWithName("PolynomialSystem::initializeForSystemSolution");
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
  MacroRegisterFunctionWithName("GroebnerBasisComputation::setUpRecursiveComputation");
  toBeModified.initializeForSystemSolution();
  toBeModified.recursionCounterSerreLikeSystem = this->recursionCounterSerreLikeSystem;
  toBeModified.maximumSerreSystemComputationsPreferred = this->maximumSerreSystemComputationsPreferred;
  toBeModified.systemSolution = this->systemSolution;
  toBeModified.solutionsFound = this->solutionsFound;
  toBeModified.flagUseTheMonomialBranchingOptimization = this->flagUseTheMonomialBranchingOptimization;
  toBeModified.flagUsingAlgebraicClosure = this->flagUsingAlgebraicClosure;
  toBeModified.theAlgebraicClosure = this->theAlgebraicClosure;
  toBeModified.groebner.maximumPolynomialComputations = this->groebner.maximumPolynomialComputations;
  toBeModified.groebner.maximumBasisReductionComputations = this->groebner.maximumBasisReductionComputations;
  toBeModified.groebner.theFormat = this->groebner.theFormat;
  toBeModified.groebner.thePolynomialOrder = this->groebner.thePolynomialOrder;
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::processSolvedSubcaseIfSolvedOrProvenToHaveSolution(
  PolynomialSystem<Coefficient>& potentiallySolvedCase
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::processSolvedSubcaseIfSolvedOrProvenToHaveSolution");
  if (potentiallySolvedCase.flagSystemSolvedOverBaseField) {
    potentiallySolvedCase.numberOfSerreSystemComputations = this->numberOfSerreSystemComputations;
    this->solutionsFound = potentiallySolvedCase.solutionsFound;
    this->systemSolution = potentiallySolvedCase.systemSolution;
    this->flagSystemProvenToHaveNoSolution = potentiallySolvedCase.flagSystemProvenToHaveNoSolution;
    this->flagSystemProvenToHaveSolution = potentiallySolvedCase.flagSystemProvenToHaveSolution;
    this->flagSystemSolvedOverBaseField = potentiallySolvedCase.flagSystemSolvedOverBaseField;
    this->backSubstituteIntoPolynomialSystem(this->impliedSubstitutions);
  }
  if (potentiallySolvedCase.flagSystemProvenToHaveSolution) {
    this->flagSystemProvenToHaveSolution = true;
  }
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::trySettingValueToVariable(
  List<Polynomial<Coefficient> >& inputSystem, const Rational& aValueToTryOnPreferredVariable
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::trySettingValueToVariable");
  ProgressReport theReport1;
  PolynomialSystem<Coefficient>& theHeuristicAttempt = this->computationUsedInRecursiveCalls.getElement();
  this->setUpRecursiveComputation(theHeuristicAttempt);
  int theVarIndex = this->getPreferredSerreSystemSubstitutionIndex(inputSystem);
  if (theVarIndex == - 1) {
    global.fatal << "This is a programming error: preferred substitution variable index is - 1. "
    << "Input system in calculator-input format: <br>"
    << this->toStringCalculatorInputFromSystem(inputSystem) << "<br>" << global.fatal;
  }
  PolynomialSubstitution<Coefficient> theSub;
  theSub.makeIdentitySubstitution(this->systemSolution.size);
  theSub[theVarIndex] = aValueToTryOnPreferredVariable;
  if (this->groebner.flagDoProgressReport) {
    std::stringstream out;
    MonomialP theMon(theVarIndex);
    out << this->toStringImpliedSubstitutions() << "Attempting an (a priori random) substitution:<br>"
    << theMon.toString(&this->format()) << "=" << aValueToTryOnPreferredVariable << ";";
    theReport1.report(out.str());
  }
  theHeuristicAttempt.setSerreLikeSolutionIndex(theVarIndex, aValueToTryOnPreferredVariable);
  for (int i = 0; i < inputSystem.size; i ++) {
    inputSystem[i].substitution(theSub);
  }
  theHeuristicAttempt.solveSerreLikeSystemRecursively(inputSystem);
  this->numberOfSerreSystemComputations += theHeuristicAttempt.numberOfSerreSystemComputations;
  this->processSolvedSubcaseIfSolvedOrProvenToHaveSolution(theHeuristicAttempt);
}

template <class Coefficient>
bool PolynomialSystem<Coefficient>::hasSingleMonomialEquation(
  const List<Polynomial<Coefficient> >& inputSystem, MonomialP& outputMon
) {
  MacroRegisterFunctionWithName("PolynomialSystem::hasSingleMonomialEquation");
  bool result = false;
  int minimalNumberOfNonZeroMonomialEntries = this->numberOfVariablesToSolveForStart * 2 + 1;
  for (int i = 0; i < inputSystem.size; i ++) {
    if (inputSystem[i].size() == 1) {
      result = true;
      int currentNumNonZeroMonEntries = 0;
      for (int j = 0; j < inputSystem[i][0].minimalNumberOfVariables(); j ++) {
        if (!(inputSystem[i][0](j) == 0)) {
          currentNumNonZeroMonEntries ++;
        }
      }
      if (currentNumNonZeroMonEntries < minimalNumberOfNonZeroMonomialEntries) {
        outputMon = inputSystem[i][0];
      }
    }
  }
  return result;
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::solveWhenSystemHasSingleMonomial(
  List<Polynomial<Coefficient> >& inputSystem, const MonomialP& theMon
) {
  MacroRegisterFunctionWithName("PolynomialSystem::solveWhenSystemHasSingleMonomial");
  ProgressReport theReport1;
  List<Polynomial<Coefficient> > inputSystemCopy = inputSystem;
  bool allProvenToHaveNoSolution = true;
  for (int i = 0; i < theMon.minimalNumberOfVariables(); i ++) {
    if (theMon(i) != 0) {
      if (this->shouldReport()) {
        std::stringstream out;
        MonomialP tempMon(i);
        out << "The system has the single monomial: " << theMon.toString(&this->format())
        << "<br>Trying case:<br>" << tempMon.toString(&this->format()) << "= 0;";
        theReport1.report(out.str());
      }
      PolynomialSubstitution<Coefficient> theSub;
      theSub.makeIdentitySubstitution(this->systemSolution.size);
      theSub[i] = 0;
      PolynomialSystem<Coefficient>& theCase = this->computationUsedInRecursiveCalls.getElement();
      this->setUpRecursiveComputation(theCase);
      theCase.setSerreLikeSolutionIndex(i, 0);
      inputSystem = inputSystemCopy;
      for (int i = 0; i < inputSystem.size; i ++) {
        inputSystem[i].substitution(theSub);
      }
      theCase.solveSerreLikeSystemRecursively(inputSystem);
      this->processSolvedSubcaseIfSolvedOrProvenToHaveSolution(theCase);
      if (!theCase.flagSystemProvenToHaveNoSolution) {
        allProvenToHaveNoSolution = false;
      }
      if (this->flagSystemSolvedOverBaseField) {
        return;
      }
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
  MacroRegisterFunctionWithName("GroebnerBasisComputation::solveSerreLikeSystemRecursively");
  RecursionDepthCounter theCounter(&this->recursionCounterSerreLikeSystem);
  ProgressReport theReport1, theReport2, theReport3;
  List<Polynomial<Coefficient> > startingSystemNoModifications = inputSystem;
  this->numberOfVariablesToSolveForStart = this->getNumberOfVariablesToSolveFor(inputSystem);
  if (this->groebner.flagDoProgressReport) {
    std::stringstream out;
    out << "Solving Serre-like polynomial system with " << this->numberOfVariablesToSolveForStart
    << " variables at recursion depth: " << this->recursionCounterSerreLikeSystem << ". ";
    theReport1.report(out.str());
  }
  this->polynomialSystemSolutionSimplificationPhase(inputSystem);
  if (this->flagSystemProvenToHaveNoSolution || this->flagSystemSolvedOverBaseField) {
    return;
  }
  this->numberOfVariablesToSolveForAfterReduction = this->getNumberOfVariablesToSolveFor(inputSystem);
  if (this->groebner.flagDoProgressReport) {
    std::stringstream out;
    out << "Without using heuristics, I managed to reduce "
    << this->numberOfVariablesToSolveForStart - this->numberOfVariablesToSolveForAfterReduction
    << " variables. ";
    if (this->numberOfVariablesToSolveForStart - this->numberOfVariablesToSolveForAfterReduction != 0) {
      out << "I did this via the substitutions " << this->toStringImpliedSubstitutions();
    }
    out << "<br>Number of remaining variables to solve for: "
    << this->numberOfVariablesToSolveForAfterReduction;
    theReport2.report(out.str());
  }
  List<Polynomial<Coefficient> > systemBeforeHeuristics = inputSystem;
  MonomialP singleMonEquation;
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
      MonomialP monomial(this->getPreferredSerreSystemSubstitutionIndex(inputSystem));
      reportStreamHeuristics << "<br>The substitution  "
      << monomial.toString(&this->groebner.theFormat) << "=" << randomValueItry << ";"
      << " did not produce a solution over the base field ";
      if (this->computationUsedInRecursiveCalls.getElement().flagSystemProvenToHaveNoSolution) {
        reportStreamHeuristics << " as it resulted in a system which has no solution. ";
      } else {
        reportStreamHeuristics << " as it resulted in a system which exceeded the computation limits. ";
      }
      theReport3.report(reportStreamHeuristics.str());
    }
  }
  inputSystem = startingSystemNoModifications;
}

template <class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::toStringDivision(Polynomial<Coefficient>& toBeDivided) {
  std::stringstream out;
  out << "Dividing: " << toBeDivided.toString() << "<br>by:<br>" << this->theBasis.toStringCommaDelimited();
  return out.str();
}

template <class Coefficient>
std::string PolynomialSystem<Coefficient>::toStringCalculatorInputFromSystem(
  const List<Polynomial<Coefficient> >& inputSystem
) {
  std::stringstream out;
  out << "findOneSolutionSerreLikePolynomialSystem";
  if (this->flagTryDirectlySolutionOverAlgebraicClosure) {
    out << "Algebraic";
  }
  out << "UpperLimit{}(" << this->maximumSerreSystemComputationsPreferred << ", ";
  for (int j = 0; j < inputSystem.size; j ++) {
    out << inputSystem[j].toString(&this->groebner.theFormat);
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
  MacroRegisterFunctionWithName("GroebnerBasisComputation::solveSerreLikeSystem");
  this->flagSystemProvenToHaveNoSolution = false;
  this->flagSystemSolvedOverBaseField = false;
  this->flagSystemProvenToHaveSolution = false;
  this->recursionCounterSerreLikeSystem = 0;
  this->groebner.numberPolynomialDivisions = 0;
  this->numberOfSerreSystemComputations = 0;
  int numVars = 0;
  List<Polynomial<Coefficient> > workingSystem = inputSystem;
  for (int i = 0; i < workingSystem.size; i ++) {
    numVars = MathRoutines::maximum(numVars, workingSystem[i].minimalNumberOfVariables());
  }
  this->systemSolution.initializeFillInObject(numVars, 0);
  this->solutionsFound.initialize(numVars);
  ProgressReport theReport;
  std::stringstream reportStream;
  if (this->groebner.flagDoProgressReport) {
    reportStream << "Solving system " << this->toStringCalculatorInputFromSystem(inputSystem);
    theReport.report(reportStream.str());
  }
  this->numberOfSerreVariablesOneGenerator = workingSystem[0].minimalNumberOfVariables() / 2;
  if (this->theAlgebraicClosure == 0) {
    this->flagTryDirectlySolutionOverAlgebraicClosure = false;
  }
  if (!this->flagTryDirectlySolutionOverAlgebraicClosure) {
    this->flagUsingAlgebraicClosure = false;
    this->solveSerreLikeSystemRecursively(workingSystem);
  }
  if (this->theAlgebraicClosure != 0) {
    if (!this->flagSystemSolvedOverBaseField && !this->flagSystemProvenToHaveNoSolution) {
      if (this->groebner.flagDoProgressReport) {
        if (!this->flagTryDirectlySolutionOverAlgebraicClosure) {
          reportStream << "<br><b>Failed to solve system over the rationals... </b>";
        }
        reportStream << "<br><b>Attempting to solve allowing algebraic extensions.</b> ";
        theReport.report(reportStream.str());
      }
      this->flagUsingAlgebraicClosure = true;
      this->solveSerreLikeSystemRecursively(workingSystem);
    }
  }
  if (this->flagSystemSolvedOverBaseField) {
    if (this->solutionsFound.cardinalitySelection != this->solutionsFound.maximumSize) {
      for (int i = 0; i < this->solutionsFound.maximumSize; i ++) {
        if (!this->solutionsFound.selected[i]) {
          this->setSerreLikeSolutionIndex(i, 0);
        }
      }
    }
    PolynomialSubstitution<Coefficient> theSub;
    this->getSubstitutionFromPartialSolutionSerreLikeSystem(theSub);
    workingSystem = inputSystem;
    for (int i = 0; i < workingSystem.size; i ++) {
      workingSystem[i].substitution(theSub);
      if (!workingSystem[i].isEqualToZero()) {
        global.fatal << "<br>This is a programming error. "
        << "Function solveSerreLikeSystem reports to have found a solution over the base field, "
        << "but substituting the solution back to the original "
        << "system does not yield a zero system of equations. More precisely, "
        << "the reported solution was " << this->toStringSerreLikeSolution()
        << " but substitution in equation " << inputSystem[i].toString()
        << " yields " << workingSystem[i].toString() << ". Calculator input: <br>"
        << this->toStringCalculatorInputFromSystem(inputSystem) << " <br>"
        << global.fatal;
      }
    }
  }
}

template <class Coefficient>
std::string PolynomialSystem<Coefficient>::toStringSerreLikeSolution() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::toStringSerreLikeSolution");
  std::stringstream out;
  Polynomial<Rational> theMon;
  for (int i = 0; i < this->systemSolution.size; i ++) {
    if (this->solutionsFound.selected[i]) {
      theMon.makeMonomial(i, 1, 1);
      out << " " << theMon.toString(&this->groebner.theFormat)
      << " = " << this->systemSolution[i] << ";";
    }
  }
  return out.str();
}

template <class Coefficient>
void PolynomialSystem<Coefficient>::setSerreLikeSolutionIndex(
  int theIndex, const Coefficient& theConst
) {
  this->systemSolution[theIndex] = theConst;
  if (this->solutionsFound.selected[theIndex]) {
    global.fatal << "This a programming error: attempting to set "
    << "value to a variable whose value has already been computed. " << global.fatal;
  }
  this->solutionsFound.addSelectionAppendNewIndex(theIndex);
}

template<class Coefficient>
bool Polynomial<Coefficient>::leastCommonMultiple(
  const Polynomial<Coefficient>& left,
  const Polynomial<Coefficient>& right,
  Polynomial<Coefficient>& output,
  const Coefficient& one,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Polynomial::leastCommonMultiple");
  if (left.isEqualToZero() || right.isEqualToZero()) {
    global.fatal << "Least common multiple of zero polynomials is not allowed. " << global.fatal;
  }
  Polynomial<Coefficient> leftTemp, rightTemp, oneMinusT;
  List<Polynomial<Coefficient> > theBasis;
  leftTemp = left;
  rightTemp = right;
  int numberOfVariables = MathRoutines::maximum(
    left.minimalNumberOfVariables(), right.minimalNumberOfVariables()
  );
  leftTemp.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  rightTemp.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  oneMinusT.makeMonomial(numberOfVariables, 1, one);
  leftTemp *= oneMinusT;
  oneMinusT *= - 1;
  oneMinusT += one;
  rightTemp *= oneMinusT;
  theBasis.setSize(0);
  theBasis.addOnTop(leftTemp);
  theBasis.addOnTop(rightTemp);
  GroebnerBasisComputation<Coefficient> computation;
  computation.theFormat.flagSuppressModP = true;
  computation.theFormat.polynomialAlphabet.addOnTop("x");
  computation.theFormat.polynomialAlphabet.addOnTop("y");
  computation.theFormat.polynomialAlphabet.addOnTop("z");
  computation.theFormat.polynomialAlphabet.addOnTop("w");
  computation.thePolynomialOrder.monomialOrder = MonomialP::orderForGreatestCommonDivisor();
  computation.maximumPolynomialComputations = - 1;
  if (!computation.transformToReducedGroebnerBasis(theBasis)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Unexpected: failed to transform to reduced "
      << "Groebner basis. ";
    }
    return false;
  }
  int maximalMonomialNoTIndex = - 1;
  Rational maximaltotalDegree;
  MonomialP currentLeading;
  for (int i = theBasis.size - 1; i >= 0; i --) {
    theBasis[i].getIndexLeadingMonomial(
      &currentLeading, nullptr, &computation.thePolynomialOrder.monomialOrder
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
    << left.toString(&computation.theFormat)
    << " and " << right.toString(&computation.theFormat) << ". "
    << "The list of polynomials is: ";
    for (int i = 0; i < theBasis.size; i ++) {
      global.fatal << theBasis[i].toString(&computation.theFormat) << ", ";
    }
    global.fatal << global.fatal;
  }
  output = theBasis[maximalMonomialNoTIndex];
  output.setNumberOfVariablesSubstituteDeletedByOne(numberOfVariables);
  output.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  return true;
}

template<class Coefficient>
bool Polynomial<Coefficient>::greatestCommonDivisor(
  const Polynomial<Coefficient>& left,
  const Polynomial<Coefficient>& right,
  Polynomial<Coefficient>& output,
  const Coefficient& one,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("Polynomial::greatestCommonDivisor");
  if (left.isEqualToZero() || right.isEqualToZero()) {
    global.fatal << "Greatest common divisor including zeroes not allowed. " << global.fatal;
  }
  Polynomial<Coefficient> leastCommonMultipleBuffer;
  Polynomial<Coefficient> productBuffer;
  Polynomial<Coefficient> remainderBuffer;
  if (!Polynomial<Coefficient>::leastCommonMultiple(
    left, right, leastCommonMultipleBuffer, one, commentsOnFailure
  )) {
    return false;
  }
  productBuffer = left;
  productBuffer *= right;
  productBuffer.divideBy(
    leastCommonMultipleBuffer,
    output,
    remainderBuffer,
    &MonomialP::orderForGreatestCommonDivisor()
  );
  if (!remainderBuffer.isEqualToZero() || output.isEqualToZero()) {
    FormatExpressions theFormat;
    theFormat.polynomialAlphabet.addOnTop("x");
    theFormat.polynomialAlphabet.addOnTop("y");
    global.fatal
    << "This is a programming error."
    << "<br>While computing the gcd of left = "
    << left.toString(&theFormat) << "<br>and right = "
    << right.toString(&theFormat) << "<br>I got that left * right = "
    << productBuffer.toString(&theFormat)
    << "<br>, and that lcm(left, right) = "
    << leastCommonMultipleBuffer.toString(&theFormat)
    << "<br>but at the same time "
    << "right * left divided by lcm (left, right) equals<br>"
    << output.toString(&theFormat)
    << "<br>with remainder " << remainderBuffer.toString(&theFormat)
    << ", which is imposible."
    << global.fatal;
  }
  output.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  return true;
}

template <class Coefficient, class OneFactorFinder>
bool PolynomialFactorization<Coefficient, OneFactorFinder>::factor(
  const Polynomial<Coefficient>& input,
  std::stringstream* comments,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("PolynomialFactorization::factor");
  this->original = input;
  if (this->original.isConstant(&this->constantFactor)) {
    return true;
  }
  this->current = this->original;
  this->constantFactor = this->current.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  this->constantFactor.invert();
  this->nonReduced.addOnTop(this->current);
  OneFactorFinder algorithm;
  algorithm.output = this;
  while (this->nonReduced.size > 0) {
    this->current = this->nonReduced.popLastObject();
    // The algorithm is allowed to store state between factorization attempts.
    if (!algorithm.oneFactor(comments, commentsOnFailure)) {
      return false;
    }
  }
  this->nonReduced.quickSortAscending();
  this->reduced.quickSortAscending();
  this->checkFactorization();
  return true;
}

template <class Coefficient, class OneFactorFinder>
bool PolynomialFactorization<Coefficient, OneFactorFinder>::accountNonReducedFactor(
  Polynomial<Coefficient>& incoming
) {
  MacroRegisterFunctionWithName("PolynomialFactorization::accountNonReducedFactor");
  incoming.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  Polynomial<Coefficient> quotient, remainder;
  this->current.divideBy(incoming, quotient, remainder, &MonomialP::orderDefault());
  if (!remainder.isEqualToZero()) {
    return false;
  }
  this->nonReduced.addOnTop(incoming);
  if (quotient.isConstant()) {
    global.fatal
    << "Accounting non-reduced factor must result is degree drop. "
    << global.fatal;
  }
  this->nonReduced.addOnTop(quotient);
  return true;
}

template <class Coefficient, class OneFactorFinder>
bool PolynomialFactorization<Coefficient, OneFactorFinder>::accountReducedFactor(
  Polynomial<Coefficient>& incoming
) {
  MacroRegisterFunctionWithName("PolynomialFactorization::accountReducedFactor");
  if (incoming.isEqualToZero()) {
    global.fatal << "zero is not a valid factor. " << global.fatal;
  }
  incoming.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  Polynomial<Coefficient> quotient, remainder;
  this->current.divideBy(
    incoming, quotient, remainder, &MonomialP::orderDefault()
  );
  if (!remainder.isEqualToZero()) {
    return false;
  }
  this->reduced.addOnTop(incoming);
  Coefficient extraFactor;
  if (quotient.isConstant(&extraFactor)) {
    this->constantFactor *= extraFactor;
  } else {
    this->nonReduced.addOnTop(quotient);
  }
  return true;
}

template <class Coefficient, class OneFactorFinder>
bool PolynomialFactorization<Coefficient, OneFactorFinder>::checkFactorization() const {
  MacroRegisterFunctionWithName("Polynomial::checkFactorization");
  Polynomial<Coefficient> checkComputations;
  checkComputations.makeConstant(this->constantFactor);
  for (int i = 0; i < this->nonReduced.size; i ++) {
    checkComputations *= nonReduced[i];
  }
  for (int i = 0; i < this->reduced.size; i ++) {
    checkComputations *= this->reduced[i];
  }
  if (!checkComputations.isEqualTo(this->original)) {
    global.fatal << "Error in polynomial factorization function."
    << "Product of factorization: " << this->toStringResult(nullptr)
    << " equals " << checkComputations
    << global.fatal;
  }
  return true;
}

template <class Coefficient, class OneFactorFinder>
std::string PolynomialFactorization<Coefficient, OneFactorFinder>::toStringResult(
  FormatExpressions *theFormat
) const {
  std::stringstream out;
  std::string constantFactorString = this->constantFactor.toString(theFormat);
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
    out << "(" << this->reduced[i].toString(theFormat) << ")";
  }
  if (this->nonReduced.size > 0) {
    out << "[[";
  }
  for (int i = 0; i < this->nonReduced.size; i ++) {
    out << "(" << this->nonReduced[i].toString(theFormat) << ")";
  }
  if (this->nonReduced.size > 0) {
    out << "]]";
  }
  return out.str();
}
#endif
