// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfImplementationHeaderPolyComputationsAdvanced_already_included
#define vpfImplementationHeaderPolyComputationsAdvanced_already_included

#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_extra_algebraic_numbers.h"

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::wrapUpGroebnerOnExceedingComputationLimit(
  List<Polynomial<Coefficient> >& inputOutpuT
) {
  inputOutpuT.reserve(this->theBasis.size + this->basisCandidates.size);
  inputOutpuT = this->theBasis;
  inputOutpuT.addListOnTop(this->basisCandidates);
  return false;
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::wrapUpOnGroebnerBasisSuccess(
  List<Polynomial<Coefficient> >& inputOutpuT
) {
  inputOutpuT = this->theBasis;
  return true;
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::transformToReducedBasis(
  List<Polynomial<Coefficient> >& inputOutpuT, int upperLimitPolyComputations
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::transformToReducedBasis");
  this->initForGroebnerComputation(inputOutpuT.size);
  this->basisCandidates = inputOutpuT;
  ProgressReport theReport1;
  if (this->flagDoProgressReport) {
    std::stringstream reportStream;
    reportStream << "<br>Transforming to reduced (NON-Groebner) basis a system of "
    << this->basisCandidates.size << " elements (at the start).";
    theReport1.report(reportStream.str());
  }
  int oldMaxNumGBCompos = this->maximumPolynomialComputations;
  this->maximumPolynomialComputations = upperLimitPolyComputations;
  this->addAndReducePolynomials();
  this->maximumPolynomialComputations = oldMaxNumGBCompos;
  if (upperLimitPolyComputations > 0) {
    if (this->numberPolynomialComputations > upperLimitPolyComputations) {
      return this->wrapUpGroebnerOnExceedingComputationLimit(inputOutpuT);
    }
  }
  return this->wrapUpOnGroebnerBasisSuccess(inputOutpuT);
}

template <class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::toStringStatusGroebnerBasisTransformation() {
  std::stringstream out;
  out << "<br>Transforming to Groebner basis a system of "
  << this->basisCandidates.size << " elements: "
  << this->basisCandidates.toStringCommaDelimited(&this->theFormat);
  return out.str();
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::transformToReducedGroebnerBasis(
  List<Polynomial<Coefficient> >& inputOutput
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::transformToReducedGroebnerBasis");
  this->initForGroebnerComputation(inputOutput.size);
  this->basisCandidates = inputOutput;
  ProgressReport reportStart("Groebner basis start");
  ProgressReport reportProgress("Groebner basis report");
  if (this->flagDoProgressReport) {
    reportStart.report(this->toStringStatusGroebnerBasisTransformation());
    reportProgress.report("Adding initial polynomials to basis. ");
  }
  this->addAndReducePolynomials();
  if (this->flagDoProgressReport) {
    reportProgress.report("Adding initial polynomials to basis done. ");
  }
  if (this->maximumPolynomialComputations > 0) {
    if (this->numberPolynomialComputations > this->maximumPolynomialComputations) {
      return this->wrapUpGroebnerOnExceedingComputationLimit(inputOutput);
    }
  }
  //this->flagBasisGuaranteedToGenerateIdeal = true;
  if (this->theBasis.size == 1) {
    return this->wrapUpOnGroebnerBasisSuccess(inputOutput);
  }
  bool changed = true;
  int sPolyDepth = 0;
  MonomialP leftHighestMonomial, rightHighestMonomial;
  Coefficient leftHighestCoefficient, rightHighestCoefficient;
  while (changed) {
    sPolyDepth ++;
    changed = false;
    for (int i = 0; i < this->theBasis.size; i ++) {
      for (int j = i + 1; j < this->theBasis.size && i < this->theBasis.size; j ++) {
        Polynomial<Coefficient>& currentLeft = this->theBasis[i];
        Polynomial<Coefficient>& currentRight = this->theBasis[j];
        if (reportProgress.tickAndWantReport()) {
          std::stringstream reportStream;
          reportStream << "<br>Computing S-poly of depth " << sPolyDepth
          << ". Taking s-difference of indices " << i + 1 << " and " << j + 1
          << " out of " << this->theBasis.size
          << ".<br>Before proceding to adjoin candidates, "
          << "I have " << this->basisCandidates.size
          << " candidates and basis of size "
          << this->theBasis.size << ".";
          reportProgress.report(reportStream.str());
        }
        currentLeft.getIndexLeadingMonomial(
          &leftHighestMonomial,
          &leftHighestCoefficient,
          &this->thePolynomialOrder.monomialOrder
        );
        currentRight.getIndexLeadingMonomial(
          &rightHighestMonomial,
          &rightHighestCoefficient,
          &this->thePolynomialOrder.monomialOrder
        );
        int numberOfVariables = MathRoutines::Maximum(
          leftHighestMonomial.minimalNumberOfVariables(),
          rightHighestMonomial.minimalNumberOfVariables()
        );
        this->soPolyLeftShift.makeOne(numberOfVariables);
        this->soPolyRightShift.makeOne(numberOfVariables);
        for (int k = 0; k < numberOfVariables; k ++) {
          if (leftHighestMonomial(k) > rightHighestMonomial(k)) {
            this->soPolyRightShift.setVariable(k, leftHighestMonomial(k) - rightHighestMonomial(k));
            this->soPolyLeftShift.setVariable(k, 0);
          } else {
            this->soPolyLeftShift.setVariable(k, rightHighestMonomial(k) - leftHighestMonomial(k));
            this->soPolyRightShift.setVariable(k, 0);
          }
        }
        this->bufPoly = currentLeft;
        this->bufPoly.multiplyBy(this->soPolyLeftShift, rightHighestCoefficient);
        this->SoPolyBuf = currentRight;
        this->SoPolyBuf.multiplyBy(this->soPolyRightShift, leftHighestCoefficient);
        this->SoPolyBuf -= this->bufPoly;
        this->basisCandidates.addOnTop(this->SoPolyBuf);
        this->numberPolynomialComputations ++;
        if (this->maximumPolynomialComputations > 0) {
          if (this->numberPolynomialComputations > this->maximumPolynomialComputations) {
            return this->wrapUpGroebnerOnExceedingComputationLimit(inputOutput);
          }
        }
      }
    }
    if (this->addAndReducePolynomials()) {
      changed = true;
    }
    if (this->maximumPolynomialComputations > 0) {
      if (this->numberPolynomialComputations > this->maximumPolynomialComputations) {
        return this->wrapUpGroebnerOnExceedingComputationLimit(inputOutput);
      }
    }
  }
  this->theBasis.quickSortAscendingCustom(this->thePolynomialOrder);
  return this->wrapUpOnGroebnerBasisSuccess(inputOutput);
}

template<class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::toStringPolynomialBasisStatus() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::toStringPolynomialBasisStatus");
  std::stringstream out;
  out << "There are " << this->basisCandidates.size
  << " candidates left to adjoin. <br>The current polynomial basis has "
  << theBasis.size << " elements. "
  << "<br>Number of Groebner-basis polynomial computations: "
  << this->numberPolynomialComputations
  << " with a limit of: " << this->maximumPolynomialComputations << " computations. "
  << "The basis follows.<br>"
  << this->theBasis.toStringCommaDelimited(&this->theFormat);
  return out.str();
}

template<class Coefficient>
bool GroebnerBasisComputation<Coefficient>::addAndReduceOnePolynomial() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::addAndReduceOnePolynomial");
  if (this->basisCandidates.size == 0) {
    return false;
  }
  this->remainderDivisionByBasis(
    *this->basisCandidates.lastObject(), &this->remainderDivision
  );
  this->numberPolynomialComputations ++;
  this->basisCandidates.removeLastObject();
  return this->addRemainderToBasis();
}

template<class Coefficient>
bool GroebnerBasisComputation<Coefficient>::addAndReducePolynomials() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::addAndReducePolynomials");
  bool changed = false;
  ProgressReport report("Add polynomial");
  while (this->basisCandidates.size > 0) {
    if (this->flagDoProgressReport) {
      report.report(this->toStringPolynomialBasisStatus());
    }
    changed |= this->addAndReduceOnePolynomial();
  }
  return changed;
}

template<class Coefficient>
void GroebnerBasisComputation<Coefficient>::MakeMinimalBasis() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::MakeMinimalBasis");
  for (int i = 0; i < this->theBasis.size; i ++) {
    for (int j = 0; j < this->leadingMons.size; j ++) {
      if (i != j) {
        if (this->leadingMons[i].isDivisibleBy(this->leadingMons[j])) {
          this->leadingMons.removeIndexSwapWithLast(i);
          this->theBasis.removeIndexSwapWithLast(i);
          i --;
          break;
        }
      }
    }
  }
}

template<class Coefficient>
bool GroebnerBasisComputation<Coefficient>::CriterionCLOsh(
  HashedListSpecialized<Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity> >&thePairs,
  List<MonomialP>& theLeadingMons,
  MonomialP& leadingTermLCM
) {
  // page 107, Cox, Little, O'Shea,
  // Ideals, Varieties, algorithms
  Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity>&
  lastPair = *thePairs.lastObject();
  Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity>
  pairBeingTested1, pairBeingTested2;
  for (int k = 0; k < theLeadingMons.size; k ++) {
    if (k != lastPair.Object1 && k != lastPair.Object2) {
      if (leadingTermLCM.isDivisibleBy(theLeadingMons[k])) {
        pairBeingTested1.Object1 = MathRoutines::Minimum(lastPair.Object1, k);
        pairBeingTested1.Object2 = MathRoutines::Maximum(lastPair.Object1, k);
        pairBeingTested2.Object1 = MathRoutines::Minimum(lastPair.Object2, k);
        pairBeingTested2.Object2 = MathRoutines::Maximum(lastPair.Object2, k);
        if (!thePairs.contains(pairBeingTested1) && !thePairs.contains(pairBeingTested2)) {
          return true;
        }
      }
    }
  }
  return false;
}

template<class Coefficient>
bool GroebnerBasisComputation<Coefficient>::transformToReducedGroebnerBasisImprovedAlgorithm(
  List<Polynomial<Coefficient> >& inputOutpuT, int upperComputationBound
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::transformToReducedGroebnerBasisImprovedAlgorithm");
  (void) upperComputationBound;
  //This is an implementation of the algorithm on page 106, Cox, Little, O'Shea,
  //Ideals, Varieties, algorithms
  this->initForGroebnerComputation(inputOutpuT);
  this->theBasis = inputOutpuT;
  HashedListSpecialized<PairInts> indexPairs;
//  Pair<int, int> currentPair;
  indexPairs.setExpectedSize(this->theBasis.size * this->theBasis.size);
  this->leadingMons.setExpectedSize(this->theBasis.size * 2);
  for (int i = 0; i < this->theBasis.size; i ++) {
    for (int j = i + 1; j < this->theBasis.size; j ++) {
      indexPairs.addOnTop(PairInts (i, j));
    }
    this->theBasis[i].ScaleNormalizeLeadingMonomial();
    int theIndex = this->theBasis[i].GetIndexMaxMonomial(this->monomialOrder);
    this->leadingMons.addOnTop(this->theBasis[i][theIndex]);
    this->leadingCoeffs.addOnTop(this->theBasis[i].coefficients[theIndex]);
  }
  if (this->theBasis.size <= 0) {
    global.fatal << "This is a programming error: transforming to Groebner basis not allowed for empty basis. " << global.fatal;
  }
  MonomialP leftShift, rightShift, monLCM;
  Polynomial<Rational> leftBuf, rightBuf;
  Polynomial<Rational>& outputRemainder = rightBuf; //to save some RAM
  ProgressReport reportOuter;
  while (indexPairs.size > 0) {
    PairInts& lastPair = *indexPairs.lastObject();
    int currentPairIndex = indexPairs.size - 1;
    bool isGood = false;
    Polynomial<Rational>& currentLeft = this->theBasis[lastPair.Object1];
    Polynomial<Rational>& currentRight = this->theBasis[lastPair.Object2];
    MonomialP& leftHighestMon = this->leadingMons[lastPair.Object1];
    MonomialP& rightHighestMon = this->leadingMons[lastPair.Object2];
    if (this->flagDoProgressReport) {
      std::stringstream out;
      out << "Basis size: " << this->theBasis.size << ".\n<br>Remaining cases current round: " << indexPairs.size;
      reportOuter.report(out.str());
    }
    int numVars = MathRoutines::Maximum(
      leftHighestMon.minimalNumberOfVariables(),
      rightHighestMon.minimalNumberOfVariables()
    );
    leftShift.makeOne(numVars);
    rightShift.makeOne(numVars);
    for (int k = 0; k < numVars; k ++) {
      if (leftHighestMon[k] > 0 && rightHighestMon[k] > 0) {
        isGood = true;
      }
      if (leftHighestMon[k] > rightHighestMon[k]) {
        rightShift.setVariable(k, leftHighestMon[k] - rightHighestMon[k]);
        leftShift.setVariable(k, 0);
      } else {
        leftShift.setVariable(k, rightHighestMon[k] - leftHighestMon[k]);
        rightShift.setVariable(k, 0);
      }
    }
    monLCM = leftHighestMon;
    monLCM *= leftShift;
    if (isGood) {
      if (!this->CriterionCLOsh(indexPairs, this->leadingMons, monLCM)) {
        leftBuf = currentLeft;
        rightBuf = currentRight;
        leftBuf.multiplyBy(leftShift, this->leadingCoeffs[lastPair.Object2]);
        rightBuf.multiplyBy(rightShift, this->leadingCoeffs[lastPair.Object1]);
        leftBuf -= rightBuf;
        this->remainderDivisionByBasis(leftBuf, &outputRemainder, global);
        if (this->flagDoProgressReport) {
          std::stringstream out;
          out << "Basis size: " << this->theBasis.size << ".\n<br>Remaining cases current round: " << indexPairs.size;
          reportOuter.report(out.str());
        }
        if (!outputRemainder.isEqualToZero()) {
          outputRemainder.scaleNormalizeLeadingMonomial(this->monomialOrder);
          this->theBasis.addOnTop(outputRemainder);
          int theIndexMaxMon = this->theBasis.lastObject()->GetIndexMaxMonomial(this->monomialOrder);
          this->leadingMons.addOnTop((*this->theBasis.lastObject())[theIndexMaxMon]);
          this->leadingCoeffs.addOnTop(this->theBasis.lastObject()->coefficients[theIndexMaxMon]);
          for (int i = 0; i < this->theBasis.size - 1; i ++) {
            indexPairs.addOnTop(PairInts(i, this->theBasis.size - 1));
          }
        }
      }
    }
    indexPairs.removeIndexSwapWithLast(currentPairIndex);
  }
  this->MakeMinimalBasis();
  inputOutpuT = this->theBasis;
  return true;
}

template <class Coefficient>
int GroebnerBasisComputation<Coefficient>::getNumberOfVariables() const {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::getNumberOfVariables");
  int result = 0;
  for (int i = 0; i < this->theBasis.size; i ++) {
    for (int j = 0; j < this->theBasis[i].size(); j ++) {
      const MonomialP& currentMon = this->theBasis[i][j];
      result = MathRoutines::Maximum(currentMon.minimalNumberOfVariables(), result);
    }
  }
  return result;
}

template <class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::toStringLetterOrder(bool addDollars) const {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::toStringLetterOrder");
  std::stringstream out;
  int numVars = this->getNumberOfVariables();
  List<MonomialP> theVars;
  out << "Variable name(s), ascending order: ";
  theVars.setSize(numVars);
  for (int i = 0; i < theVars.size; i ++) {
    theVars[i].makeEi(i, 1);
  }
  theVars.quickSortAscending(&this->thePolynomialOrder.monomialOrder);
  FormatExpressions tempFormat = this->theFormat;
  if (addDollars) {
    out << "$";
  }
  for (int i = 0; i < numVars; i ++) {
    out << theVars[i].toString(&tempFormat);
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
      out << theVars[i].toString(&tempFormat);
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
  const MonomialP& leadingMonomialBasis = this->leadingMons[index];
  const Coefficient& leadingCoefficientBasis = this->leadingCoeffs[index];
  MonomialP quotientMonomial = leadingMonomial;
  quotientMonomial /= leadingMonomialBasis;
  if (!quotientMonomial.HasPositiveOrZeroExponents()) {
    global.fatal << "This is a programming error: the pivot monomial "
    << "in the polynomial division algorithm has negative exponent(s). "
    << "This is not allowed. " << global.fatal;
  }
  if (this->flagDoLogDivision) {
    this->intermediateHighestMonDivHighestMon.getElement().addOnTop(quotientMonomial);
    this->intermediateSelectedDivisors.getElement().addOnTop(index);
  }
  this->bufPoly = this->theBasis[index];
  Coefficient quotientCoefficient = leadingCoefficient;
  quotientCoefficient /= leadingCoefficientBasis;
  if (this->flagDoLogDivision) {
    this->intermediateCoeffs.getElement().addOnTop(quotientCoefficient);
  }
  this->bufPoly.multiplyBy(quotientMonomial, quotientCoefficient);
  if (this->flagStoreQuotients) {
    this->theQuotients[index].addMonomial(quotientMonomial, quotientCoefficient);
  }
  if (this->flagDoLogDivision) {
    this->intermediateSubtractands.getElement().addOnTop(this->bufPoly);
  }
  if (this->flagDoProgressReport && theReport != nullptr) {
    std::stringstream out;
    out << "Polynomial operation count: " << this->numberPolynomialComputations;
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
  remainder -= this->bufPoly;
  if (this->flagDoLogDivision) {
    this->intermediateRemainders.getElement().addOnTop(remainder);
    List<MonomialP> empty;
    this->intermediateHighlightedMons.getElement().addOnTop(empty);
  }
  this->numberPolynomialComputations ++;
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::oneDivisonStepWithBasis(
  Polynomial<Coefficient>& currentRemainder,
  Polynomial<Coefficient>* remainderResult,
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
    if (!highestMonomial.isDivisibleBy(this->leadingMons[i])) {
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
  if (remainderResult != nullptr) {
    remainderResult->addMonomial(highestMonomial, leadingCoefficient);
  }
  if (this->flagDoLogDivision) {
    (*this->intermediateHighlightedMons.getElement().lastObject()).addOnTop(highestMonomial);
  }
  currentRemainder.popMonomial(indexLeadingMonomial);
  this->numberPolynomialComputations ++;
  return false;
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::remainderDivisionByBasis(
  Polynomial<Coefficient>& inputOutput,
  Polynomial<Coefficient>* outputRemainder,
  int basisIndexToIgnore
) {
  // Reference: Cox, Little, O'Shea, Ideals, Varieties and Algorithms, page 62.
  MacroRegisterFunctionWithName("GroebnerBasisComputation::remainderDivisionByBasis");
  if (
    &inputOutput == outputRemainder ||
    &inputOutput == &this->bufPoly ||
    outputRemainder == &this->bufPoly
  ) {
    global.fatal << "This is a programming error: the input, the output and "
    << "the buffer member object must be pairwise distinct when carrying out "
    << "multi-polynomial division. " << global.fatal;
  }
  if (this->flagStoreQuotients) {
    this->theQuotients.setSize(this->theBasis.size);
    for (int i = 0; i < this->theQuotients.size; i ++) {
      this->theQuotients[i].makeZero();
    }
  }
  MemorySaving<Polynomial<Coefficient> > tempPoly;
  if (outputRemainder == 0) {
    outputRemainder = &tempPoly.getElement();
  }
  ProgressReport reportHeader;
  ProgressReport reportMain;
  ProgressReport reportRemainderSoFar;
  reportRemainderSoFar.ticksPerReport = 50;
  if (this->flagDoProgressReport) {
    std::stringstream reportStream;
    reportStream << "Computing remainder of division of "
    << inputOutput.toString(&this->theFormat)
    << " by " << this->theBasis.toString(&this->theFormat);
    reportHeader.report(reportStream.str());
  }
  outputRemainder->makeZero();
  Polynomial<Coefficient>& currentRemainder = inputOutput;
  if (this->flagDoLogDivision) {
    this->startingPoly.getElement() = currentRemainder;
  }
  if (this->flagDoLogDivision) {
    this->intermediateCoeffs.getElement().size = 0;
    this->intermediateRemainders.getElement().size = 0;
    this->intermediateSubtractands.getElement().size = 0;

    this->intermediateRemainders.getElement().addOnTop(currentRemainder);
    this->intermediateHighlightedMons.getElement().setSize(1);
    this->intermediateHighlightedMons.getElement().lastObject()->setSize(0);
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
      if (outputRemainder != nullptr) {
        out << "<br>" << outputRemainder->size()
        << " monomials in the final output remainder.";
      }
      out << "<br>Current basis: " << this->theBasis.toStringCommaDelimited(&this->theFormat);
      out << "<br>Current remainder: " << currentRemainder.toString(&this->theFormat);
      reportRemainderSoFar.report(out.str());
    }
  }
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::addRemainderToBasis() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::addRemainderToBasis");
  if (this->leadingMons.size != this->theBasis.size) {
    global.fatal << "This is a programming error: "
    << "the number of leading monomials does not equal "
    << "the number of polynomials. " << global.fatal;
  }
  if (this->remainderDivision.isEqualToZero()) {
    return false;
  }
  this->remainderDivision.scaleNormalizeLeadingMonomial(&this->thePolynomialOrder.monomialOrder);
  MonomialP newLeadingMonomial;
  Coefficient remainderLeadingCoefficient;
  this->remainderDivision.getIndexLeadingMonomial(
    &newLeadingMonomial,
    &remainderLeadingCoefficient,
    &this->thePolynomialOrder.monomialOrder
  );
  int indexToAddAt = 0;
  for (; indexToAddAt < this->theBasis.size; indexToAddAt ++) {
    MonomialP otherLeadingMonomial = this->theBasis[indexToAddAt].GetLeadingMonomial(
      &this->thePolynomialOrder.monomialOrder
    );
    if (this->thePolynomialOrder.monomialOrder.greaterThan(
      newLeadingMonomial, otherLeadingMonomial
    )) {
      break;
    }
  }
  for (int i = indexToAddAt; i < this->theBasis.size; i ++) {
    this->basisCandidates.addOnTop(this->theBasis[i]);
  }
  this->theBasis.setSize(indexToAddAt + 1);
  this->leadingMons.setSize(indexToAddAt + 1);
  this->leadingCoeffs.setSize(indexToAddAt + 1);
  this->theBasis[indexToAddAt] = this->remainderDivision;
  this->leadingMons[indexToAddAt] = newLeadingMonomial;
  this->leadingCoeffs[indexToAddAt] = remainderLeadingCoefficient;
  return true;
}

template <class Coefficient>
GroebnerBasisComputation<Coefficient>::GroebnerBasisComputation() {
  // "Graded reverse lexicographic" order.
  this->thePolynomialOrder.monomialOrder.leftGreaterThanRight = MonomialP::greaterThan_totalDegree_rightSmallerWins;

  this->RecursionCounterSerreLikeSystem = 0;
  this->NumVarsToSolveForStarT = - 1;
  this->NumVariablesToSolveForAfterReduction = 0;
  this->NumberSerreSystemComputations = 0;
  this->NumberSerreVariablesOneGenerator = - 1;
  this->numberPolynomialComputations = 0;
  this->numberOfIntermediateRemainders = 0;

  this->MaxNumSerreSystemComputationsPreferred = 0;
  this->maximumPolynomialComputations = 0;
  this->MaxNumBasisReductionComputations = 10000;
  this->firstIndexLatexSlide = - 1;

  this->flagDoLogDivision = false;
  this->flagStoreQuotients = false;
  this->flagSystemProvenToHaveNoSolution = false;
  this->flagSystemProvenToHaveSolution = false;
  this->flagSystemSolvedOverBaseField = false;
  this->flagUsingAlgebraicClosuRe = false;
  this->flagTryDirectlySolutionOverAlgebraicClosure = false;
  this->flagUseTheMonomialBranchingOptimization = false;

  this->theAlgebraicClosurE = 0;
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::initializeForDivision(
  List<Polynomial<Coefficient> >& inputOutput
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::initializeForDivision");
  if (inputOutput.size <= 0) {
    global.fatal << "This is a programming error: "
    << "I cannot transform an empty list to a Groebner basis. "
    << global.fatal;
  }
  this->theBasis = inputOutput;
  this->leadingMons.setSize(inputOutput.size);
  this->leadingCoeffs.setSize(inputOutput.size);
  for (int i = 0; i < this->theBasis.size; i ++) {
    Polynomial<Coefficient>& curPoly = theBasis[i];
    int theIndex = curPoly.getIndexLeadingMonomial(
      &this->leadingMons[i],
      &this->leadingCoeffs[i],
      &this->thePolynomialOrder.monomialOrder
    );
    if (theIndex == - 1) {
      global.fatal << "This is a programming error: initialization for polynomial "
      << "division with respect to at least one zero polynomial. "
      << "If this is a bad user input, it should be handled at an earlier level. "
      << "Here is the current basis by which we need to divide. "
      << this->theBasis.toString() << global.fatal;
    }
  }
  this->numberPolynomialComputations = 0;
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::initForGroebnerComputation(int expectedNumInputPolys) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::initForGroebnerComputation");
  this->basisCandidates.setSize(0);
  this->theBasis.setSize(0);
  this->theBasis.reserve(expectedNumInputPolys);
  this->leadingMons.setSize(0);
  this->leadingMons.reserve(expectedNumInputPolys);
  this->leadingCoeffs.setSize(0);
  this->leadingCoeffs.reserve(expectedNumInputPolys);
  this->numberPolynomialComputations = 0;
}

template<class Coefficient>
void GroebnerBasisComputation<Coefficient>::checkConsistency() {
  //if (this->NumberOfComputations>this->MaxNumComputations+ 1000)
    //global.fatal << "This may or may not be a programming error. While handling computation excess limit, I got that NumberOfComputations is much larger than MaxNumComputations. "
    //<< " I have no explanation for this issue right now, so I am crashing to let you know something is fishy. "
    //<< global.fatal;
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::getSubstitutionFromPartialSolutionSerreLikeSystem(
  PolynomialSubstitution<Coefficient>& outputSub
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::getSubstitutionFromPartialSolutionSerreLikeSystem");
  outputSub.MakeIdSubstitution(this->systemSolution.getElement().size);
  for (int i = 0; i < this->solutionsFound.getElement().CardinalitySelection; i ++) {
    outputSub[this->solutionsFound.getElement().elements[i]] = this->systemSolution.getElement()[
      this->solutionsFound.getElement().elements[i]
    ];
  }
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::getOneVariablePolynomialSolution(
  const Polynomial<Coefficient>& thePoly, Coefficient& outputSolution
) {
  AlgebraicNumber theAN;
  if (this->theAlgebraicClosurE == 0) {
    return false;
  }
  if (!theAN.constructFromMinimalPolynomial(
    thePoly, *this->theAlgebraicClosurE, nullptr
  )) {
    return false;
  }
  outputSolution = theAN;
  return true;
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::HasImpliedSubstitutions(
  List<Polynomial<Coefficient> >& inputSystem, PolynomialSubstitution<Coefficient>& outputSub
) {
  int numVars = this->systemSolution.getElement().size;
  MonomialP tempM;
  Polynomial<Coefficient> tempP;
  Coefficient theCF;
  for (int i = 0; i < inputSystem.size; i ++) {
    tempP = inputSystem[i];
    for (int j = 0; j < numVars; j ++) {
      tempM.makeEi(j, 1, numVars);
      int indexTempM = tempP.theMonomials.getIndex(tempM);
      if (indexTempM == - 1) {
        continue;
      }
      theCF = tempP.coefficients[indexTempM];
      tempP.SubtractMonomial(tempM, theCF);
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
      outputSub.MakeIdSubstitution(numVars);
      outputSub[j] = tempP;
      theCF *= - 1;
      outputSub[j] /= theCF;
//      coefficient theConst;
//      global.Comments << "<hr>Output sub is: x_{" << j+ 1 << "}=" << outputSub[j].toString();
//      if (outputSub[j].IsConstant(&theConst))
//        this->SetSerreLikeSolutionIndex(j, theConst);
      //global.Comments << "<br>Current solution candidate is: " << this->systemSolution.getElement().toString();
      return true;
    }
    int oneVarIndex;
    if (tempP.IsOneVariableNonConstPoly(&oneVarIndex)) {
      if (this->flagUsingAlgebraicClosuRe && this->theAlgebraicClosurE != 0) {
        if (this->getOneVariablePolynomialSolution(tempP, theCF)) {
          outputSub.MakeIdSubstitution(numVars);
          outputSub[oneVarIndex].makeConstant(theCF);
          //check our work:
          tempP.substitution(outputSub);
          if (!tempP.isEqualToZero()) {
            global.fatal << "This is a programming error: I was solving the polynomial equation " << inputSystem[i].toString()
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
int GroebnerBasisComputation<Coefficient>::GetNumEquationsThatWouldBeLinearIfIsubbedVar(
  int theVarIndex, List<Polynomial<Coefficient> >& input
) {
  int result = 0;
  for (int i = 0; i < input.size; i ++) {
    Rational degExcludingVar = 0;
    for (int j = 0; j < input[i].size(); j ++) {
      degExcludingVar = MathRoutines::Maximum(input[i][j].TotalDegree() - input[i][j](theVarIndex), degExcludingVar);
    }
    if (degExcludingVar < 2) {
      result ++;
    }
  }
  return result;
}

template <class Coefficient>
int GroebnerBasisComputation<Coefficient>::GetPreferredSerreSystemSubIndex(List<Polynomial<Coefficient> >& inputSystem) {
  Selection varSel;
  this->GetVarsToSolveFor(inputSystem, varSel);
  if (varSel.CardinalitySelection == 0) {
    return - 1;
  }
  int championIndex = varSel.elements[0];
  int championImprovement = GroebnerBasisComputation<Coefficient>::GetNumEquationsThatWouldBeLinearIfIsubbedVar(
    championIndex, inputSystem
  );
  for (int i = 1; i < varSel.CardinalitySelection; i ++) {
    if (
      GroebnerBasisComputation<Coefficient>::GetNumEquationsThatWouldBeLinearIfIsubbedVar(
        varSel.elements[i], inputSystem
      ) > championImprovement
    ) {
      championIndex = varSel.elements[i];
      championImprovement = GroebnerBasisComputation<Coefficient>::GetNumEquationsThatWouldBeLinearIfIsubbedVar(
        varSel.elements[i], inputSystem
      );
    }
  }
  return championIndex;
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::BackSubstituteIntoSinglePoly(
  Polynomial<Coefficient>& thePoly, int theIndex, PolynomialSubstitution<Coefficient>& theFinalSub
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::BackSubstituteIntoSinglePoly");
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
        if (!this->solutionsFound.getElement().selected[j]) {
          this->SetSerreLikeSolutionIndex(j, 0);
        } else {
          if (this->systemSolution.getElement()[j] != 0) {
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
  if (!thePoly.IsConstant(&tempCF)) {
    global.fatal << "\n<br>\nThis is a programming error: after carrying all implied substitutions "
    << "the polynomial is not a constant, rather equals "
    << thePoly.toString() << ". " << global.fatal;
  }
  theFinalSub[theIndex] = tempCF;
  this->SetSerreLikeSolutionIndex(theIndex, tempCF);
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::BackSubstituteIntoPolySystem(
  List<PolynomialSubstitution<Coefficient> >& theImpliedSubs
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::BackSubstituteIntoPolySystem");
  PolynomialSubstitution<Coefficient> FinalSub;
  this->getSubstitutionFromPartialSolutionSerreLikeSystem(FinalSub);
  for (int i = theImpliedSubs.size - 1; i >= 0; i --) {
    for (int j = 0; j < theImpliedSubs[i].size; j ++) {
      this->BackSubstituteIntoSinglePoly(theImpliedSubs[i][j], j, FinalSub);
    }
  }
}

template <class Coefficient>
int GroebnerBasisComputation<Coefficient>::GetNumVarsToSolveFor(const List<Polynomial<Coefficient> >& input) {
  Selection theSel;
  GroebnerBasisComputation<Coefficient>::GetVarsToSolveFor(input, theSel);
  return theSel.CardinalitySelection;
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::GetVarsToSolveFor(const List<Polynomial<Coefficient> >& input, Selection& output) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::GetVarsToSolveFor");
  int NumVars = 0;
  for (int i = 0; i < input.size; i ++) {
    NumVars = MathRoutines::Maximum(NumVars, input[i].minimalNumberOfVariables());
  }
  output.initialize(NumVars);
  for (int i = 0; i < input.size && output.CardinalitySelection < output.MaxSize; i ++) {
    for (int j = 0; j < input[i].size() && output.CardinalitySelection < output.MaxSize; j ++) {
      for (int k = 0; k < input[i][j].minimalNumberOfVariables() && output.CardinalitySelection < output.MaxSize; k ++) {
        if (input[i][j](k) != 0) {
          output.AddSelectionAppendNewIndex(k);
        }
      }
    }
  }
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::IsContradictoryReducedSystem(const List<Polynomial<Coefficient> >& input) {
  if (input.size == 1) {
    if (input[0].IsConstant()) {
      return true;
    }
  }
  return false;
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::PolySystemSolutionSimplificationPhase(
  List<Polynomial<Coefficient> >& inputSystem
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::PolySystemSolutionSimplificationPhase");
  ProgressReport theReport1, theReport2, theReport3;
  if (this->flagDoProgressReport) {
    std::stringstream reportStream;
    reportStream << "Reducing input system without use of heuristics.";
    theReport1.report(reportStream.str());
  }
  bool changed = true;
  PolynomialSubstitution<Coefficient> theSub;
  this->theImpliedSubS.setSize(0);
  this->theImpliedSubS.reserve(inputSystem.size);
//  int startingMaxNumSerreSystemComputations = this->MaxNumSerreSystemComputations;
  while (changed) {
    this->numberPolynomialComputations = 0;
    List<Polynomial<Coefficient> > oldSystem = inputSystem;
    bool success = this->transformToReducedBasis(inputSystem, this->MaxNumBasisReductionComputations);
    if (!success) {
      inputSystem = oldSystem;
    } else {
      oldSystem = inputSystem;
    }
    if (this->flagDoProgressReport) {
      std::stringstream reportStream;
      reportStream << "Attempting to transform system to a groebner basis... ";
      theReport2.report(reportStream.str());
    }
    if (success && inputSystem.size > 0) {
      this->numberPolynomialComputations = 0;
      success = this->transformToReducedGroebnerBasis(inputSystem);
    }
    if (!success) {
      inputSystem = oldSystem;
    } else {
      oldSystem = inputSystem;
    }
    if (this->flagDoProgressReport) {
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
    this->NumberSerreSystemComputations += this->numberPolynomialComputations;
    if (success) {
      if (this->IsContradictoryReducedSystem(inputSystem)) {
        this->flagSystemProvenToHaveNoSolution = true;
        this->flagSystemSolvedOverBaseField = false;
        this->flagSystemProvenToHaveSolution = false;
        return;
      } else {
        this->flagSystemProvenToHaveSolution = true;
        if (inputSystem.size == 0) {
          this->flagSystemProvenToHaveNoSolution = false;
          this->flagSystemSolvedOverBaseField = true;
          this->BackSubstituteIntoPolySystem(this->theImpliedSubS);
          return;
        }
      }
    }
    changed = this->HasImpliedSubstitutions(inputSystem, theSub);
    if (changed) {
      if (this->flagDoProgressReport) {
        std::stringstream reportStream;
        reportStream << "Found implied substitutions.<br>"
        << this->ToStringImpliedSubs();
        theReport3.report(reportStream.str());
      }
      this->theImpliedSubS.addOnTop(theSub);
      for (int i = 0; i < inputSystem.size; i ++) {
        inputSystem[i].substitution(theSub);
      }
    }
  }
}

template <class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::ToStringImpliedSubs() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::ToStringImpliedSubs");
  if (this->theImpliedSubS.size == 0) {
    return "";
  }
  std::stringstream out;
  out << "Implied subs: ";
  for (int i = 0; i < this->theImpliedSubS.size; i ++) {
    for (int j = 0; j < this->theImpliedSubS[i].size; j ++) {
      int letterIndex = - 1;
      if (this->theImpliedSubS[i][j].IsOneLetterFirstDegree(&letterIndex)) {
        if (letterIndex == j) {
          continue;
        }
      }
      out << "<br>" << (MonomialP(j)).toString(&this->theFormat) << "="
      << this->theImpliedSubS[i][j].toString(&this->theFormat) << "; ";
    }
  }
  return out.str();
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::initForSystemSolution() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::initForSystemSolution");
  this->initForGroebnerComputation(0);
  this->theImpliedSubS.setSize(0);
  this->flagSystemProvenToHaveNoSolution = false;
  this->flagSystemSolvedOverBaseField = false;
  this->flagSystemProvenToHaveSolution = false;
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::SetUpRecursiveComputation(GroebnerBasisComputation& toBeModified) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::SetUpRecursiveComputation");
  toBeModified.initForSystemSolution();
  toBeModified.RecursionCounterSerreLikeSystem = this->RecursionCounterSerreLikeSystem;
  toBeModified.maximumPolynomialComputations = this->maximumPolynomialComputations;
  toBeModified.MaxNumSerreSystemComputationsPreferred = this->MaxNumSerreSystemComputationsPreferred;
  toBeModified.MaxNumBasisReductionComputations = this->MaxNumBasisReductionComputations;
  toBeModified.systemSolution = this->systemSolution;
  toBeModified.solutionsFound = this->solutionsFound;
  toBeModified.theFormat = this->theFormat;
  toBeModified.thePolynomialOrder = this->thePolynomialOrder;
  toBeModified.flagUseTheMonomialBranchingOptimization = this->flagUseTheMonomialBranchingOptimization;
  toBeModified.theAlgebraicClosurE = this->theAlgebraicClosurE;
  toBeModified.flagUsingAlgebraicClosuRe = this->flagUsingAlgebraicClosuRe;
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::ProcessSolvedSubcaseIfSolvedOrProvenToHaveSolution(
  GroebnerBasisComputation& potentiallySolvedCase
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::ProcessSolvedSubcaseIfSolvedOrProvenToHaveSolution");
  if (potentiallySolvedCase.flagSystemSolvedOverBaseField) {
    potentiallySolvedCase.NumberSerreSystemComputations = this->NumberSerreSystemComputations;
    this->solutionsFound = potentiallySolvedCase.solutionsFound;
    this->systemSolution = potentiallySolvedCase.systemSolution;
    this->flagSystemProvenToHaveNoSolution = potentiallySolvedCase.flagSystemProvenToHaveNoSolution;
    this->flagSystemProvenToHaveSolution = potentiallySolvedCase.flagSystemProvenToHaveSolution;
    this->flagSystemSolvedOverBaseField = potentiallySolvedCase.flagSystemSolvedOverBaseField;
    this->BackSubstituteIntoPolySystem(this->theImpliedSubS);
  }
  if (potentiallySolvedCase.flagSystemProvenToHaveSolution) {
    this->flagSystemProvenToHaveSolution = true;
  }
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::TrySettingValueToVariable(
  List<Polynomial<Coefficient> >& inputSystem, const Rational& aValueToTryOnPreferredVariable
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::TrySettingValueToVariable");
  ProgressReport theReport1;
  GroebnerBasisComputation& theHeuristicAttempt = this->ComputationUsedInRecursiveCalls.getElement();
  this->SetUpRecursiveComputation(theHeuristicAttempt);
  int theVarIndex = this->GetPreferredSerreSystemSubIndex(inputSystem);
  if (theVarIndex == - 1) {
    global.fatal << "This is a programming error: preferred substitution variable index is - 1. "
    << "Input system in calculator-input format: <br>"
    << this->ToStringCalculatorInputFromSystem(inputSystem) << "<br>" << global.fatal;
  }
  PolynomialSubstitution<Coefficient> theSub;
  theSub.MakeIdSubstitution(this->systemSolution.getElement().size);
  theSub[theVarIndex] = aValueToTryOnPreferredVariable;
  if (this->flagDoProgressReport) {
    std::stringstream out;
    MonomialP theMon(theVarIndex);
    out << this->ToStringImpliedSubs() << "<br>Attempting an (a priori random) substitution:<br>"
    << theMon.toString(&this->theFormat) << "=" << aValueToTryOnPreferredVariable << ";";
    theReport1.report(out.str());
  }
  theHeuristicAttempt.SetSerreLikeSolutionIndex(theVarIndex, aValueToTryOnPreferredVariable);
  for (int i = 0; i < inputSystem.size; i ++) {
    inputSystem[i].substitution(theSub);
  }
  theHeuristicAttempt.solveSerreLikeSystemRecursively(inputSystem);
  this->NumberSerreSystemComputations+= theHeuristicAttempt.NumberSerreSystemComputations;
  this->ProcessSolvedSubcaseIfSolvedOrProvenToHaveSolution(theHeuristicAttempt);
}

template <class Coefficient>
bool GroebnerBasisComputation<Coefficient>::HasSingleMonomialEquation(
  const List<Polynomial<Coefficient> >& inputSystem, MonomialP& outputMon
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::HasSingleMonomialEquation");
  bool result = false;
  int MinNumNonZeroMonEntries = this->NumVarsToSolveForStarT * 2 + 1;
  for (int i = 0; i < inputSystem.size; i ++) {
    if (inputSystem[i].size() == 1) {
      result = true;
      int currentNumNonZeroMonEntries = 0;
      for (int j = 0; j < inputSystem[i][0].minimalNumberOfVariables(); j ++) {
        if (!(inputSystem[i][0](j) == 0)) {
          currentNumNonZeroMonEntries ++;
        }
      }
      if (currentNumNonZeroMonEntries < MinNumNonZeroMonEntries) {
        outputMon = inputSystem[i][0];
      }
    }
  }
  return result;
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::SolveWhenSystemHasSingleMonomial(
  List<Polynomial<Coefficient> >& inputSystem, const MonomialP& theMon
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::SolveWhenSystemHasSingleMonomial");
  ProgressReport theReport1;
  List<Polynomial<Coefficient> > inputSystemCopy = inputSystem;
  bool allProvenToHaveNoSolution = true;
  for (int i = 0; i < theMon.minimalNumberOfVariables(); i ++) {
    if (theMon(i) != 0) {
      if (this->flagDoProgressReport) {
        std::stringstream out;
        MonomialP tempMon(i);
        out << "The system has the single monomial: " << theMon.toString(&this->theFormat)
        << "<br>Trying case:<br>" << tempMon.toString(&this->theFormat) << "= 0;";
        theReport1.report(out.str());
      }
      PolynomialSubstitution<Coefficient> theSub;
      theSub.MakeIdSubstitution(this->systemSolution.getElement().size);
      theSub[i] = 0;
      GroebnerBasisComputation& theCase = this->ComputationUsedInRecursiveCalls.getElement();
      this->SetUpRecursiveComputation(theCase);
      theCase.SetSerreLikeSolutionIndex(i, 0);
      inputSystem = inputSystemCopy;
      for (int i = 0; i < inputSystem.size; i ++) {
        inputSystem[i].substitution(theSub);
      }
      theCase.solveSerreLikeSystemRecursively(inputSystem);
      this->ProcessSolvedSubcaseIfSolvedOrProvenToHaveSolution(theCase);
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
void GroebnerBasisComputation<Coefficient>::solveSerreLikeSystemRecursively(List<Polynomial<Coefficient> >& inputSystem) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::solveSerreLikeSystemRecursively");
  RecursionDepthCounter theCounter(&this->RecursionCounterSerreLikeSystem);
  ProgressReport theReport1, theReport2, theReport3;
  List<Polynomial<Coefficient> > startingSystemNoModifications = inputSystem;
  this->NumVarsToSolveForStarT = this->GetNumVarsToSolveFor(inputSystem);
  if (this->flagDoProgressReport) {
    std::stringstream out;
    out << "<hr>Solving Serre-like polynomial system with " << this->NumVarsToSolveForStarT
    << " variables at recursion depth: " << this->RecursionCounterSerreLikeSystem << ". ";
    theReport1.report(out.str());
  }
  this->PolySystemSolutionSimplificationPhase(inputSystem);
  if (this->flagSystemProvenToHaveNoSolution || this->flagSystemSolvedOverBaseField) {
    return;
  }
  this->NumVariablesToSolveForAfterReduction = this->GetNumVarsToSolveFor(inputSystem);
  if (this->flagDoProgressReport) {
    std::stringstream out;
    out << "Without using heuristics, I managed to reduce "
    << this->NumVarsToSolveForStarT - this->NumVariablesToSolveForAfterReduction
    << " variables. ";
    if (this->NumVarsToSolveForStarT - this->NumVariablesToSolveForAfterReduction != 0) {
      out << "I did this via the substitutions " << this->ToStringImpliedSubs();
    }
    out << "<br>Number of remaining variables to solve for: "
    << this->NumVariablesToSolveForAfterReduction;
    theReport2.report(out.str());
  }
  List<Polynomial<Coefficient> > systemBeforeHeuristics = inputSystem;
  MonomialP singleMonEquation;
  if (this->flagUseTheMonomialBranchingOptimization) {
    if (this->HasSingleMonomialEquation(inputSystem, singleMonEquation)) {
      this->SolveWhenSystemHasSingleMonomial(inputSystem, singleMonEquation);
      return;
    }
  }
  std::stringstream reportStreamHeuristics;
  for (int randomValueItry = 0; randomValueItry < 2; randomValueItry ++) {
    this->TrySettingValueToVariable(inputSystem, randomValueItry);
    if (this->flagSystemSolvedOverBaseField) {
      return;
    }
    inputSystem = systemBeforeHeuristics;
    if (this->flagDoProgressReport) {
      MonomialP theMon(this->GetPreferredSerreSystemSubIndex(inputSystem));
      reportStreamHeuristics << "<br>The substitution  "
      << theMon.toString(&this->theFormat) << "=" << randomValueItry << ";"
      << " did not produce a solution over the base field ";
      if (this->ComputationUsedInRecursiveCalls.getElement().flagSystemProvenToHaveNoSolution) {
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
  return  out.str();
}

template <class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::ToStringCalculatorInputFromSystem(
  const List<Polynomial<Coefficient> >& inputSystem
) {
  std::stringstream out;
  out << "FindOneSolutionSerreLikePolynomialSystem";
  if (this->flagTryDirectlySolutionOverAlgebraicClosure) {
    out << "Algebraic";
  }
  out << "UpperLimit{}(" << this->MaxNumSerreSystemComputationsPreferred << ", ";
  for (int j = 0; j < inputSystem.size; j ++) {
    out << inputSystem[j].toString(&this->theFormat);
    if (j != inputSystem.size - 1) {
      out << ", ";
    }
  }
  out << ")";
  return out.str();
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::SolveSerreLikeSystem(List<Polynomial<Coefficient> >& inputSystem) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::SolveSerreLikeSystem");
  this->flagSystemProvenToHaveNoSolution = false;
  this->flagSystemSolvedOverBaseField = false;
  this->flagSystemProvenToHaveSolution = false;
  this->RecursionCounterSerreLikeSystem = 0;
  this->numberPolynomialComputations = 0;
  this->NumberSerreSystemComputations = 0;
  int numVars = 0;
  List<Polynomial<Coefficient> > workingSystem = inputSystem;
  for (int i = 0; i < workingSystem.size; i ++) {
    numVars = MathRoutines::Maximum(numVars, workingSystem[i].minimalNumberOfVariables());
  }
  this->systemSolution.getElement().initializeFillInObject(numVars, 0);
  this->solutionsFound.getElement().initialize(numVars);
  ProgressReport theReport;
  std::stringstream reportStream;
  if (this->flagDoProgressReport) {
    reportStream << "Solving system " << this->ToStringCalculatorInputFromSystem(inputSystem);
    theReport.report(reportStream.str());
  }
  this->NumberSerreVariablesOneGenerator = workingSystem[0].minimalNumberOfVariables() / 2;
  if (this->theAlgebraicClosurE == 0) {
    this->flagTryDirectlySolutionOverAlgebraicClosure = false;
  }
  if (!this->flagTryDirectlySolutionOverAlgebraicClosure) {
    this->flagUsingAlgebraicClosuRe = false;
    this->solveSerreLikeSystemRecursively(workingSystem);
  }
  if (this->theAlgebraicClosurE != 0) {
    if (!this->flagSystemSolvedOverBaseField && !this->flagSystemProvenToHaveNoSolution) {
      if (this->flagDoProgressReport) {
        if (!this->flagTryDirectlySolutionOverAlgebraicClosure) {
          reportStream << "<br><b>Failed to solve system over the rationals... </b>";
        }
        reportStream << "<br><b>Attempting to solve allowing algebraic extensions.</b> ";
        theReport.report(reportStream.str());
      }
      this->flagUsingAlgebraicClosuRe = true;
      this->solveSerreLikeSystemRecursively(workingSystem);
    }
  }
  if (this->flagSystemSolvedOverBaseField) {
    if (this->solutionsFound.getElement().CardinalitySelection != this->solutionsFound.getElement().MaxSize) {
      for (int i = 0; i < this->solutionsFound.getElement().MaxSize; i ++) {
        if (!this->solutionsFound.getElement().selected[i]) {
          this->SetSerreLikeSolutionIndex(i, 0);
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
        << "Function SolveSerreLikeSystem reports to have found a solution over the base field, "
        << "but substituting the solution back to the original "
        << "system does not yield a zero system of equations. More precisely, "
        << "the reported solution was " << this->ToStringSerreLikeSolution()
        << " but substitution in equation " << inputSystem[i].toString()
        << " yields " << workingSystem[i].toString() << ". Calculator input: <br>"
        << this->ToStringCalculatorInputFromSystem(inputSystem) << " <br>"
        << global.fatal;
      }
    }
  }
}

template <class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::ToStringSerreLikeSolution() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::ToStringSerreLikeSolution");
  std::stringstream out;
  Polynomial<Rational> theMon;
  for (int i = 0; i < this->systemSolution.getElement().size; i ++) {
    if (this->solutionsFound.getElement().selected[i]) {
      theMon.makeMonomial(i, 1, 1);
      out << " " << theMon.toString(&this->theFormat)
      << " = " << this->systemSolution.getElement()[i] << ";";
    }
  }
  return out.str();
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::SetSerreLikeSolutionIndex(
  int theIndex, const Coefficient& theConst
) {
  this->systemSolution.getElement()[theIndex] = theConst;
  if (this->solutionsFound.getElement().selected[theIndex]) {
    global.fatal << "This a programming error: attempting to set "
    << "value to a variable whose value has already been computed. " << global.fatal;
  }
  this->solutionsFound.getElement().AddSelectionAppendNewIndex(theIndex);
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
  Polynomial<Coefficient> leftTemp, rightTemp, oneMinusT;
  List<Polynomial<Coefficient> > theBasis;
  leftTemp = left;
  rightTemp = right;
  int theNumVars = MathRoutines::Maximum(
    left.minimalNumberOfVariables(), right.minimalNumberOfVariables()
  );
  leftTemp.SetNumVariablesSubDeletedVarsByOne(theNumVars + 1);
  rightTemp.SetNumVariablesSubDeletedVarsByOne(theNumVars + 1);
  leftTemp.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  rightTemp.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  oneMinusT.makeMonomial(theNumVars, 1, one, theNumVars + 1);
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
  computation.thePolynomialOrder.monomialOrder = MonomialP::orderForGreatestCommonDivisor();
  computation.maximumPolynomialComputations = - 1;
  if (!computation.transformToReducedGroebnerBasis(theBasis)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Unexpected: failed to transform to reduced "
      << "Groebner basis. ";
    }
    return false;
  }
  int maxMonNoTIndex = - 1;
  Rational maximalTotalDegree;
  MonomialP currentLeading;
  for (int i = 0; i < theBasis.size; i ++) {
    theBasis[i].getIndexLeadingMonomial(
      &currentLeading, nullptr, &computation.thePolynomialOrder.monomialOrder
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
  productBuffer.DivideBy(
    leastCommonMultipleBuffer,
    output,
    remainderBuffer,
    &MonomialP::orderForGreatestCommonDivisor()
  );
  if (!remainderBuffer.isEqualToZero() || output.isEqualToZero()) {
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
  output.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  return true;
}

template <class Coefficient, class oneFactorFinder>
bool PolynomialFactorization<Coefficient, oneFactorFinder>::factor(
  const Polynomial<Coefficient>& input,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("PolynomialFactorization::factor");
  this->original = input;
  if (this->original.IsConstant(&this->constantFactor)) {
    return true;
  }
  this->current = this->original;
  this->constantFactor = this->current.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  this->constantFactor.invert();
  this->nonReduced.addOnTop(this->current);
  while (this->nonReduced.size > 0) {
    this->current = this->nonReduced.popLastObject();
    oneFactorFinder algorithm;
    algorithm.output = this;
    if (!algorithm.oneFactor(commentsOnFailure)) {
      return false;
    }
  }
  this->nonReduced.quickSortAscending();
  this->reduced.quickSortAscending();
  this->checkFactorization();
  return true;
}

template <class Coefficient, class oneFactorFinder>
bool PolynomialFactorization<Coefficient, oneFactorFinder>::accountNonReducedFactor(
  Polynomial<Coefficient>& incoming
) {
  MacroRegisterFunctionWithName("PolynomialFactorization::accountNonReducedFactor");
  incoming.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  Polynomial<Coefficient> quotient, remainder;
  this->current.DivideBy(incoming, quotient, remainder, &MonomialP::orderDefault());
  if (!remainder.isEqualToZero()) {
    return false;
  }
  this->nonReduced.addOnTop(incoming);
  if (quotient.IsConstant()) {
    global.fatal
    << "Accounting non-reduced factor must result is degree drop. "
    << global.fatal;
  }
  this->nonReduced.addOnTop(quotient);
  return true;
}

template <class Coefficient, class oneFactorFinder>
bool PolynomialFactorization<Coefficient, oneFactorFinder>::accountReducedFactor(
  Polynomial<Coefficient>& incoming
) {
  MacroRegisterFunctionWithName("PolynomialFactorization::accountReducedFactor");
  if (incoming.isEqualToZero()) {
    global.fatal << "Zero is not a valid factor. " << global.fatal;
  }
  incoming.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  Polynomial<Coefficient> quotient, remainder;
  this->current.DivideBy(
    incoming, quotient, remainder, &MonomialP::orderDefault()
  );
  if (!remainder.isEqualToZero()) {
    return false;
  }
  this->reduced.addOnTop(incoming);
  Coefficient extraFactor;
  if (quotient.IsConstant(&extraFactor)) {
    this->constantFactor *= extraFactor;
  } else {
    this->nonReduced.addOnTop(quotient);
  }
  return true;
}

template <class Coefficient, class oneFactorFinder>
bool PolynomialFactorization<Coefficient, oneFactorFinder>::checkFactorization() const {
  MacroRegisterFunctionWithName("Polynomial::checkFactorization");
  Polynomial<Coefficient> checkComputations;
  checkComputations.makeConstant(this->constantFactor);
  for (int i = 0; i < this->nonReduced.size; i ++) {
    checkComputations *= nonReduced[i];
  }
  for (int i = 0; i < this->reduced.size; i ++) {
    checkComputations *= this->reduced[i];
  }
  if (!checkComputations.IsEqualTo(this->original)) {
    global.fatal << "Error in polynomial factorization function."
    << "Product of factorization: " << this->toStringResult(nullptr)
    << " equals " << checkComputations
    << global.fatal;
  }
  return true;
}

template <class Coefficient, class oneFactorFinder>
std::string PolynomialFactorization<Coefficient, oneFactorFinder>::toStringResult(
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
