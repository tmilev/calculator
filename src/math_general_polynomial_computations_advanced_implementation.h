// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfImplementationHeaderPolyComputationsAdvanced_already_included
#define vpfImplementationHeaderPolyComputationsAdvanced_already_included

#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_extra_algebraic_numbers.h"

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::WrapUpGroebnerOnExceedingComputationLimit(
  List<Polynomial<coefficient> >& inputOutpuT
) {
  inputOutpuT.Reserve(this->theBasiS.size + this->basisCandidates.size);
  inputOutpuT = this->theBasiS;
  inputOutpuT.AddListOnTop(this->basisCandidates);
  return false;
}

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::WrapUpOnGroebnerBasisSuccess(
  List<Polynomial<coefficient> >& inputOutpuT
) {
  inputOutpuT = this->theBasiS;
  return true;
}

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::TransformToReducedBasis(
  List<Polynomial<coefficient> >& inputOutpuT, int upperLimitPolyComputations
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::TransformToReducedBasis");
  this->initForGroebnerComputation(inputOutpuT.size);
  this->basisCandidates = inputOutpuT;
  ProgressReport theReport1;
  if (this->flagDoProgressReport) {
    std::stringstream reportStream;
    reportStream << "<br>Transforming to reduced (NON-Groebner) basis a system of "
    << this->basisCandidates.size << " elements (at the start).";
    theReport1.Report(reportStream.str());
  }
  int oldMaxNumGBCompos = this->MaxNumGBComputations;
  this->MaxNumGBComputations = upperLimitPolyComputations;
  this->AddPolysAndReduceBasis();
  this->MaxNumGBComputations = oldMaxNumGBCompos;
  if (upperLimitPolyComputations > 0) {
    if (this->NumberGBComputations > upperLimitPolyComputations) {
      return this->WrapUpGroebnerOnExceedingComputationLimit(inputOutpuT);
    }
  }
  return this->WrapUpOnGroebnerBasisSuccess(inputOutpuT);
}

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::TransformToReducedGroebnerBasis(
  List<Polynomial<coefficient> >& inputOutput
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::TransformToReducedGroebnerBasis");
  this->initForGroebnerComputation(inputOutput.size);
  this->basisCandidates = inputOutput;
  ProgressReport theReport1, theReport2;
  if (this->flagDoProgressReport) {
    std::stringstream reportStream;
    reportStream << "<br>Transforming to Groebner basis a system of "
    << this->basisCandidates.size << " elements (at the start).";
    theReport1.Report(reportStream.str());
  }
  this->AddPolysAndReduceBasis();
  if (this->MaxNumGBComputations > 0) {
    if (this->NumberGBComputations > this->MaxNumGBComputations) {
      return this->WrapUpGroebnerOnExceedingComputationLimit(inputOutput);
    }
  }
  //this->flagBasisGuaranteedToGenerateIdeal = true;
  if (this->theBasiS.size == 1) {
    return this->WrapUpOnGroebnerBasisSuccess(inputOutput);
  }
  bool changed = true;
  int SpolyDepth = 0;
  MonomialP leftHighestMonomial, rightHighestMonomial;
  coefficient leftHighestCoefficient, rightHighestCoefficient;
  while (changed) {
    SpolyDepth ++;
    changed = false;
    for (int i = 0; i < this->theBasiS.size; i ++) {
      for (int j = i + 1; j < this->theBasiS.size && i < this->theBasiS.size; j ++) {
        Polynomial<coefficient>& currentLeft = this->theBasiS[i];
        Polynomial<coefficient>& currentRight = this->theBasiS[j];
        currentLeft.GetIndexLeadingMonomial(
          &leftHighestMonomial,
          &leftHighestCoefficient,
          &this->thePolynomialOrder.theMonOrder
        );
        currentRight.GetIndexLeadingMonomial(
          &rightHighestMonomial,
          &rightHighestCoefficient,
          &this->thePolynomialOrder.theMonOrder
        );
        int numVars = MathRoutines::Maximum(
          leftHighestMonomial.minimalNumberOfVariables(),
          rightHighestMonomial.minimalNumberOfVariables()
        );
        this->SoPolyLeftShift.MakeOne(numVars);
        this->SoPolyRightShift.MakeOne(numVars);
        for (int k = 0; k < numVars; k ++) {
          if (leftHighestMonomial(k) > rightHighestMonomial(k)) {
            this->SoPolyRightShift[k] = leftHighestMonomial(k) - rightHighestMonomial(k);
            this->SoPolyLeftShift[k] = 0;
          } else {
            this->SoPolyLeftShift[k] = rightHighestMonomial(k) - leftHighestMonomial(k);
            this->SoPolyRightShift[k] = 0;
          }
        }
        if (this->flagDoProgressReport) {
          std::stringstream reportStream;
          reportStream << "<br>Computing S-poly of depth " << SpolyDepth
          << ". Taking s-difference of indices " << i + 1 << " and " << j + 1
          << " out of " << this->theBasiS.size
          << ".<br>Before proceding to adjoin candidates, "
          << "I have " << this->basisCandidates.size
          << " candidates and basis of size "
          << this->theBasiS.size << ".";
          theReport2.Report(reportStream.str());
        }
        this->bufPoly = currentLeft;
        this->bufPoly.MultiplyBy(this->SoPolyLeftShift, rightHighestCoefficient);
        this->SoPolyBuf= currentRight;
        this->SoPolyBuf.MultiplyBy(this->SoPolyRightShift, leftHighestCoefficient);
        this->SoPolyBuf -= this->bufPoly;
        this->basisCandidates.AddOnTop(this->SoPolyBuf);
        this->NumberGBComputations ++;
        if (this->MaxNumGBComputations > 0) {
          if (this->NumberGBComputations > this->MaxNumGBComputations) {
            return this->WrapUpGroebnerOnExceedingComputationLimit(inputOutput);
          }
        }
      }
    }
    if (this->AddPolysAndReduceBasis()) {
      changed = true;
    }
    if (this->MaxNumGBComputations > 0) {
      if (this->NumberGBComputations > this->MaxNumGBComputations) {
        return this->WrapUpGroebnerOnExceedingComputationLimit(inputOutput);
      }
    }
  }
  this->theBasiS.QuickSortAscendingCustom(this->thePolynomialOrder);
  return this->WrapUpOnGroebnerBasisSuccess(inputOutput);
}

template<class coefficient>
std::string GroebnerBasisComputation<coefficient>::ToStringPolynomialBasisStatus() const {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::ToStringPolynomialBasisStatus");
  std::stringstream out;
  out << "There are " << this->basisCandidates.size
  << " candidates left to adjoin. <br>The current polynomial basis has "
  << theBasiS.size << " elements. "
  << "<br>Number of Groebner-basis polynomial computations: "
  << this->NumberGBComputations
  << " with a limit of: " << this->MaxNumGBComputations << " computations. ";
  return out.str();
}

template <class coefficient>
int GroebnerBasisComputation<coefficient>::SelectPolyIndexToAddNext() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::SelectPolyIndexToAddNext");
  int result = this->basisCandidates.size - 1;
  MonomialP left, right;
  for (int i = this->basisCandidates.size - 2; i >= 0; i --) {
    if (this->basisCandidates[result].size() > this->basisCandidates[i].size()) {
      result = i;
    } else if (
      this->basisCandidates[result].size() == this->basisCandidates[i].size()) {
      left = this->basisCandidates[i].GetLeadingMonomial(&this->thePolynomialOrder.theMonOrder);
      right = this->basisCandidates[result].GetLeadingMonomial(&this->thePolynomialOrder.theMonOrder);
      if (left > right) {
        result = i;
      }
    }
  }
  return result;
}

template<class coefficient>
bool GroebnerBasisComputation<coefficient>::AddPolysAndReduceBasis() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::AddPolyAndReduceBasis");
  bool changed = false;
  ProgressReport theReport1;
  if (this->flagDoProgressReport) {
    theReport1.Report(this->ToStringPolynomialBasisStatus());
  }
  while (this->basisCandidates.size > 0) {
    bool addedNew = false;
    for (int i = this->basisCandidates.size - 1; i >= 0; i --) {
      if (this->basisCandidates[i].IsEqualToZero()) {
        this->basisCandidates.RemoveIndexSwapWithLast(i);
      }
    }
    int oldBasisSize = this->basisCandidates.size;
    while (this->basisCandidates.size > 0) {
      int selectedIndex = this->SelectPolyIndexToAddNext();
      this->RemainderDivisionByBasis(
        this->basisCandidates[selectedIndex], &this->remainderDivision
      );
      this->basisCandidates.RemoveIndexSwapWithLast(selectedIndex);
      if (this->AddRemainderToBasis()) {
        changed = true;
        addedNew = true;
        if (this->basisCandidates.size > oldBasisSize * 2) {
          break;
        }
      }
      this->NumberGBComputations ++;
      if (this->MaxNumGBComputations > 0) {
        if (this->NumberGBComputations > this->MaxNumGBComputations) {
          this->CheckConsistency();
          return true;
        }
      }
      if (this->flagDoProgressReport) {
        theReport1.Report(this->ToStringPolynomialBasisStatus());
      }
    }
    if (!addedNew) {
      break;
    }
    for (int i = 0; i < this->theBasiS.size; i ++) {
      this->bufPolyForGaussianElimination = this->theBasiS[i];
      if (this->flagDoProgressReport) {
        std::stringstream reportStream;
        reportStream << this->ToStringPolynomialBasisStatus()
        << "<br>Using poly division, verifying " << i + 1 << " out of "
        << theBasiS.size << " basis elements.";
        theReport1.Report(reportStream.str());
      }
      this->NumberGBComputations ++;
      this->RemainderDivisionByBasis(
        this->bufPolyForGaussianElimination,
        &this->remainderDivision,
        i
      );
      this->remainderDivision.scaleNormalizeLeadingMonomial();
      if (this->MaxNumGBComputations > 0) {
        if (this->NumberGBComputations > this->MaxNumGBComputations) {
          this->CheckConsistency();
          return true;
        }
      }
      if (!(this->remainderDivision == this->theBasiS[i])) {
        this->basisCandidates.AddOnTop(this->remainderDivision);
        this->leadingMons.RemoveIndexShiftDown(i);
        this->leadingCoeffs.RemoveIndexShiftDown(i);
        this->theBasiS.RemoveIndexShiftDown(i);
        i --;
        changed = true;
      }
    }
  }
  return changed;
}

template<class coefficient>
void GroebnerBasisComputation<coefficient>::MakeMinimalBasis() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::MakeMinimalBasis");
  for (int i = 0; i < this->theBasiS.size; i ++) {
    for (int j = 0; j < this->leadingMons.size; j ++) {
      if (i != j) {
        if (this->leadingMons[i].IsDivisibleBy(this->leadingMons[j])) {
          this->leadingMons.RemoveIndexSwapWithLast(i);
          this->theBasiS.RemoveIndexSwapWithLast(i);
          i --;
          break;
        }
      }
    }
  }
}

template<class coefficient>
bool GroebnerBasisComputation<coefficient>::CriterionCLOsh(
  HashedListSpecialized<Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity> >&thePairs,
  List<MonomialP>& theLeadingMons,
  MonomialP& leadingTermLCM
) {
  // page 107, Cox, Little, O'Shea,
  // Ideals, Varieties, algorithms
  Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity>&
  lastPair = *thePairs.LastObject();
  Pair<int, int, MathRoutines::IntUnsignIdentity, MathRoutines::IntUnsignIdentity>
  pairBeingTested1, pairBeingTested2;
  for (int k = 0; k < theLeadingMons.size; k ++) {
    if (k != lastPair.Object1 && k != lastPair.Object2) {
      if (leadingTermLCM.IsDivisibleBy(theLeadingMons[k])) {
        pairBeingTested1.Object1 = MathRoutines::Minimum(lastPair.Object1, k);
        pairBeingTested1.Object2 = MathRoutines::Maximum(lastPair.Object1, k);
        pairBeingTested2.Object1 = MathRoutines::Minimum(lastPair.Object2, k);
        pairBeingTested2.Object2 = MathRoutines::Maximum(lastPair.Object2, k);
        if (!thePairs.Contains(pairBeingTested1) && !thePairs.Contains(pairBeingTested2)) {
          return true;
        }
      }
    }
  }
  return false;
}

template<class coefficient>
bool GroebnerBasisComputation<coefficient>::TransformToReducedGroebnerBasisImprovedAlgorithm(
  List<Polynomial<coefficient> >& inputOutpuT, int upperComputationBound
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::TransformToReducedGroebnerBasisImprovedAlgorithm");
  (void) upperComputationBound;
  //This is an implementation of the algorithm on page 106, Cox, Little, O'Shea,
  //Ideals, Varieties, algorithms
  this->initForGroebnerComputation(inputOutpuT, global);
  this->theBasiS = inputOutpuT;
  HashedListSpecialized<PairInts> indexPairs;
//  Pair<int, int> currentPair;
  indexPairs.SetExpectedSize(this->theBasiS.size * this->theBasiS.size);
  this->leadingMons.SetExpectedSize(this->theBasiS.size * 2);
  for (int i = 0; i < this->theBasiS.size; i ++) {
    for (int j = i + 1; j < this->theBasiS.size; j ++) {
      indexPairs.AddOnTop(PairInts (i, j));
    }
    this->theBasiS[i].ScaleNormalizeLeadingMonomial();
    int theIndex = this->theBasiS[i].GetIndexMaxMonomial(this->theMonOrdeR);
    this->leadingMons.AddOnTop(this->theBasiS[i][theIndex]);
    this->leadingCoeffs.AddOnTop(this->theBasiS[i].coefficients[theIndex]);
  }
  if (this->theBasiS.size <= 0) {
    global.fatal << "This is a programming error: transforming to Groebner basis not allowed for empty basis. " << global.fatal;
  }
  MonomialP leftShift, rightShift, monLCM;
  Polynomial<Rational> leftBuf, rightBuf;
  Polynomial<Rational>& outputRemainder = rightBuf; //to save some RAM
  ProgressReport reportOuter;
  while (indexPairs.size > 0) {
    PairInts& lastPair = *indexPairs.LastObject();
    int currentPairIndex = indexPairs.size - 1;
    bool isGood = false;
    Polynomial<Rational>& currentLeft = this->theBasiS[lastPair.Object1];
    Polynomial<Rational>& currentRight = this->theBasiS[lastPair.Object2];
    MonomialP& leftHighestMon = this->leadingMons[lastPair.Object1];
    MonomialP& rightHighestMon = this->leadingMons[lastPair.Object2];
    if (this->flagDoProgressReport) {
      std::stringstream out;
      out << "Basis size: " << this->theBasiS.size << ".\n<br>Remaining cases current round: " << indexPairs.size;
      reportOuter.Report(out.str());
    }
    int numVars = MathRoutines::Maximum(
      leftHighestMon.minimalNumberOfVariables(),
      rightHighestMon.minimalNumberOfVariables()
    );
    leftShift.MakeOne(numVars);
    rightShift.MakeOne(numVars);
    for (int k = 0; k < numVars; k ++) {
      if (leftHighestMon[k] > 0 && rightHighestMon[k] > 0)
        isGood = true;
      if (leftHighestMon[k] > rightHighestMon[k]) {
        rightShift[k] = leftHighestMon[k] - rightHighestMon[k];
        leftShift[k] = 0;
      } else {
        leftShift[k] = rightHighestMon[k] - leftHighestMon[k] ;
        rightShift[k] = 0;
      }
    }
    monLCM = leftHighestMon;
    monLCM *= leftShift;
    if (isGood) {
      if (!this->CriterionCLOsh(indexPairs, this->leadingMons, monLCM)) {
        leftBuf = currentLeft;
        rightBuf = currentRight;
        leftBuf.MultiplyBy(leftShift, this->leadingCoeffs[lastPair.Object2]);
        rightBuf.MultiplyBy(rightShift, this->leadingCoeffs[lastPair.Object1]);
        leftBuf -= rightBuf;
        this->RemainderDivisionByBasis(leftBuf, &outputRemainder, global);
        if (this->flagDoProgressReport) {
          std::stringstream out;
          out << "Basis size: " << this->theBasiS.size << ".\n<br>Remaining cases current round: " << indexPairs.size;
          reportOuter.Report(out.str());
        }
        if (!outputRemainder.IsEqualToZero()) {
          outputRemainder.scaleNormalizeLeadingMonomial();
          this->theBasiS.AddOnTop(outputRemainder);
          int theIndexMaxMon = this->theBasiS.LastObject()->GetIndexMaxMonomial(this->theMonOrdeR);
          this->leadingMons.AddOnTop((*this->theBasiS.LastObject())[theIndexMaxMon]);
          this->leadingCoeffs.AddOnTop(this->theBasiS.LastObject()->coefficients[theIndexMaxMon]);
          for (int i = 0; i < this->theBasiS.size - 1; i ++) {
            indexPairs.AddOnTop(PairInts(i, this->theBasiS.size - 1));
          }
        }
      }
    }
    indexPairs.RemoveIndexSwapWithLast(currentPairIndex);
  }
  this->MakeMinimalBasis();
  inputOutpuT = this->theBasiS;
  return true;
}

template <class coefficient>
int GroebnerBasisComputation<coefficient>::GetNumVars() const {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::GetNumVars");
  int result = 0;
  for (int i = 0; i < this->theBasiS.size; i ++) {
    for (int j = 0; j < this->theBasiS[i].size(); j ++) {
      const MonomialP& currentMon = this->theBasiS[i][j];
      result = MathRoutines::Maximum(currentMon.minimalNumberOfVariables(), result);
    }
  }
  return result;
}

template <class coefficient>
std::string GroebnerBasisComputation<coefficient>::ToStringLetterOrder(bool addDollars) const {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::ToStringLetterOrder");
  std::stringstream out;
  int numVars = this->GetNumVars();
  List<MonomialP> theVars;
  out << "Variable name(s), ascending order: ";
  theVars.SetSize(numVars);
  for (int i = 0; i < theVars.size; i ++) {
    theVars[i].MakeEi(i, 1);
  }
  theVars.QuickSortAscending(&this->thePolynomialOrder.theMonOrder);
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

template <class coefficient>
void GroebnerBasisComputation<coefficient>::OneDivisonSubStepWithBasis(
  Polynomial<coefficient>& remainder,
  const MonomialP& leadingMonomial,
  const coefficient& leadingCoefficient,
  int index,
  ProgressReport* theReport
) {
  const MonomialP& leadingMonomialBasis = this->leadingMons[index];
  const coefficient& leadingCoefficientBasis = this->leadingCoeffs[index];
  MonomialP quotientMonomial = leadingMonomial;
  quotientMonomial /= leadingMonomialBasis;
  if (!quotientMonomial.HasPositiveOrZeroExponents()) {
    global.fatal << "This is a programming error: the pivot monomial "
    << "in the polynomial division algorithm has negative exponent(s). "
    << "This is not allowed. " << global.fatal;
  }
  if (this->flagDoLogDivision) {
    this->intermediateHighestMonDivHighestMon.GetElement().AddOnTop(quotientMonomial);
    this->intermediateSelectedDivisors.GetElement().AddOnTop(index);
  }
  this->bufPoly = this->theBasiS[index];
  coefficient quotientCoefficient = leadingCoefficient;
  quotientCoefficient /= leadingCoefficientBasis;
  if (this->flagDoLogDivision) {
    this->intermediateCoeffs.GetElement().AddOnTop(quotientCoefficient);
  }
  this->bufPoly.MultiplyBy(quotientMonomial, quotientCoefficient);
  if (this->flagStoreQuotients) {
    this->theQuotients[index].AddMonomial(quotientMonomial, quotientCoefficient);
  }
  if (this->flagDoLogDivision) {
    this->intermediateSubtractands.GetElement().AddOnTop(this->bufPoly);
  }
  if (this->flagDoProgressReport && theReport != nullptr) {
    std::stringstream out;
    out << "Polynomial operation count: " << this->NumberGBComputations;
    if (this->MaxNumGBComputations > 0) {
      out << ", with a limit of " << this->MaxNumGBComputations;
    }
    out << ".\n";
    out << "<br>Number of intermediate remainders: "
    << this->numberOfIntermediateRemainders << "\n<br>Highest remainder monomial: "
    << leadingMonomial.toString()
    << ".\n<br>"
    << "Dividing working remainder monomial " << leadingMonomialBasis.toString()
    << " by the leading monomial " << leadingMonomialBasis.toString()
    << " of basis element: " << index + 1
    << " out of " << this->theBasiS.size << "\n<br>"
    << remainder.size() << " monomials in current remainder.";
    theReport->Report(out.str());
  }
  remainder -= this->bufPoly;
  if (this->flagDoLogDivision) {
    this->intermediateRemainders.GetElement().AddOnTop(remainder);
    List<MonomialP> empty;
    this->intermediateHighlightedMons.GetElement().AddOnTop(empty);
  }
  this->NumberGBComputations++;
}

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::OneDivisonStepWithBasis(
  Polynomial<coefficient>& currentRemainder,
  Polynomial<coefficient>* remainderResult,
  int basisIndexToIgnore,
  ProgressReport* report
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::OneDivisonStepWithBasis");
  MonomialP highestMonomial;
  coefficient leadingCoefficient;
  int indexLeadingMonomial = currentRemainder.GetIndexLeadingMonomial(
    &highestMonomial,
    &leadingCoefficient,
    &this->thePolynomialOrder.theMonOrder
  );
  if (indexLeadingMonomial == -1) {
    return false;
  }
  for (int i = 0; i < this->theBasiS.size; i ++) {
    if (i == basisIndexToIgnore) {
      continue;
    }
    if (!highestMonomial.IsDivisibleBy(this->leadingMons[i])) {
      continue;
    }
    this->numberOfIntermediateRemainders ++;
    this->OneDivisonSubStepWithBasis(
      currentRemainder,
      highestMonomial,
      leadingCoefficient,
      i,
      report
    );
    return true;
  }
  if (remainderResult != nullptr) {
    remainderResult->AddMonomial(highestMonomial, leadingCoefficient);
  }
  if (this->flagDoLogDivision) {
    (*this->intermediateHighlightedMons.GetElement().LastObject()).AddOnTop(highestMonomial);
  }
  currentRemainder.PopMonomial(indexLeadingMonomial);
  this->NumberGBComputations ++;
  return false;
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::RemainderDivisionByBasis(
  Polynomial<coefficient>& inputOutput,
  Polynomial<coefficient>* outputRemainder,
  int basisIndexToIgnore
) {
  //Reference: Cox, Little, O'Shea, Ideals, Varieties and Algorithms, page 62.
  MacroRegisterFunctionWithName("GroebnerBasisComputation::RemainderDivisionByBasis");
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
    this->theQuotients.SetSize(this->theBasiS.size);
    for (int i = 0; i < this->theQuotients.size; i ++) {
      this->theQuotients[i].MakeZero();
    }
  }
  MemorySaving<Polynomial<coefficient> > tempPoly;
  if (outputRemainder == 0) {
    outputRemainder = &tempPoly.GetElement();
  }
  ProgressReport reportHeader;
  ProgressReport reportMain;
  ProgressReport reportRemainderSoFar;
  if (this->flagDoProgressReport) {
    reportHeader.Report("Computing remainder division");
  }
  outputRemainder->MakeZero();
  Polynomial<coefficient>& currentRemainder = inputOutput;
  if (this->flagDoLogDivision) {
    this->startingPoly.GetElement() = currentRemainder;
  }
  coefficient leadingMonCoeff;
  if (this->flagDoLogDivision) {
    this->intermediateCoeffs.GetElement().size = 0;
    this->intermediateRemainders.GetElement().size = 0;
    this->intermediateSubtractands.GetElement().size = 0;

    this->intermediateRemainders.GetElement().AddOnTop(currentRemainder);
    this->intermediateHighlightedMons.GetElement().SetSize(1);
    this->intermediateHighlightedMons.GetElement().LastObject()->SetSize(0);
  }
  while (!currentRemainder.IsEqualToZero()) {
    this->numberOfIntermediateRemainders = 0;
    while (this->OneDivisonStepWithBasis(
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
      reportRemainderSoFar.Report(out.str());
    }
  }
}

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::AddRemainderToBasis() {
  if (this->leadingMons.size != this->theBasiS.size) {
    global.fatal << "This is a programming error: "
    << "the number of leading monomials does not equal "
    << "the number of polynomials. " << global.fatal;
  }
  MacroRegisterFunctionWithName("GroebnerBasisComputation::AddPolyToBasis");
  if (this->remainderDivision.IsEqualToZero()) {
    return false;
  }
  this->remainderDivision.scaleNormalizeLeadingMonomial();
  MonomialP theNewLeadingMon;
  coefficient remainderLeadingCoefficient;
  this->remainderDivision.GetIndexLeadingMonomial(
    &theNewLeadingMon,
    &remainderLeadingCoefficient,
    &this->thePolynomialOrder.theMonOrder
  );
  if (this->flagDoSortBasis) {
    this->theBasiS.SetSize(this->theBasiS.size + 1);
    this->leadingMons.SetSize(this->theBasiS.size);
    this->leadingCoeffs.SetSize(this->theBasiS.size);
    for (int i = theBasiS.size - 1; i >= 0; i --) {
      bool shouldAddHere = true;
      if (i > 0) {
        shouldAddHere = (theNewLeadingMon > this->theBasiS[i - 1].GetLeadingMonomial(
          &this->thePolynomialOrder.theMonOrder
        ));
      }
      if (shouldAddHere) {
        this->theBasiS[i] = this->remainderDivision;
        this->leadingMons[i] = theNewLeadingMon;
        this->leadingCoeffs[i] = remainderLeadingCoefficient;
        break;
      } else {
        this->theBasiS[i] = this->theBasiS[i - 1];
        this->leadingMons[i] = this->leadingMons[i - 1];
        this->leadingCoeffs[i] = this->leadingCoeffs[i - 1];
      }
    }
  } else {
    this->theBasiS.AddOnTop(this->remainderDivision);
    this->leadingMons.AddOnTop(theNewLeadingMon);
    this->leadingCoeffs.AddOnTop(remainderLeadingCoefficient);
  }
  return true;
}

template <class coefficient>
GroebnerBasisComputation<coefficient>::GroebnerBasisComputation() {
  // "Graded reverse lexicographic" order.
  this->thePolynomialOrder.theMonOrder.leftGreaterThanRight = MonomialP::greaterThan_totalDegree_rightSmallerWins;

  this->RecursionCounterSerreLikeSystem = 0;
  this->NumVarsToSolveForStarT = - 1;
  this->NumVariablesToSolveForAfterReduction = 0;
  this->NumberSerreSystemComputations = 0;
  this->NumberSerreVariablesOneGenerator = - 1;
  this->NumberGBComputations = 0;
  this->numberOfIntermediateRemainders = 0;

  this->MaxNumSerreSystemComputationsPreferred = 0;
  this->MaxNumGBComputations = 0;
  this->MaxNumBasisReductionComputations = 10000;
  this->firstIndexLatexSlide = - 1;

  this->flagDoProgressReport = true;
  this->flagDoSortBasis = true;
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

template <class coefficient>
void GroebnerBasisComputation<coefficient>::initializeForDivision(
  List<Polynomial<coefficient> >& inputOutput
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::initializeForDivision");
  if (inputOutput.size <= 0) {
    global.fatal << "This is a programming error: "
    << "I cannot transform an empty list to a Groebner basis. "
    << global.fatal;
  }
  this->theBasiS = inputOutput;
  this->leadingMons.SetSize(inputOutput.size);
  this->leadingCoeffs.SetSize(inputOutput.size);
  for (int i = 0; i < this->theBasiS.size; i ++) {
    Polynomial<coefficient>& curPoly = theBasiS[i];
    int theIndex = curPoly.GetIndexLeadingMonomial(
      &this->leadingMons[i],
      &this->leadingCoeffs[i],
      &this->thePolynomialOrder.theMonOrder
    );
    if (theIndex == - 1) {
      global.fatal << "This is a programming error: initialization for polynomial "
      << "division with respect to at least one zero polynomial. "
      << "If this is a bad user input, it should be handled at an earlier level. "
      << "Here is the current basis by which we need to divide. "
      << this->theBasiS.toString() << global.fatal;
    }
  }
  this->NumberGBComputations = 0;
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::initForGroebnerComputation(int expectedNumInputPolys) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::initForGroebnerComputation");
  this->basisCandidates.SetSize(0);
  this->theBasiS.SetSize(0);
  this->theBasiS.Reserve(expectedNumInputPolys);
  this->leadingMons.SetSize(0);
  this->leadingMons.Reserve(expectedNumInputPolys);
  this->leadingCoeffs.SetSize(0);
  this->leadingCoeffs.Reserve(expectedNumInputPolys);
  this->NumberGBComputations = 0;
}

template<class coefficient>
void GroebnerBasisComputation<coefficient>::CheckConsistency() {
  //if (this->NumberOfComputations>this->MaxNumComputations+ 1000)
    //global.fatal << "This may or may not be a programming error. While handling computation excess limit, I got that NumberOfComputations is much larger than MaxNumComputations. "
    //<< " I have no explanation for this issue right now, so I am crashing to let you know something is fishy. "
    //<< global.fatal;
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::GetSubFromPartialSolutionSerreLikeSystem(
  PolynomialSubstitution<coefficient>& outputSub
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::GetSubFromPartialSolutionSerreLikeSystem");
  outputSub.MakeIdSubstitution(this->systemSolution.GetElement().size);
  for (int i = 0; i < this->solutionsFound.GetElement().CardinalitySelection; i ++) {
    outputSub[this->solutionsFound.GetElement().elements[i]] = this->systemSolution.GetElement()[
      this->solutionsFound.GetElement().elements[i]
    ];
  }
}

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::GetOneVarPolySolution(
  const Polynomial<coefficient>& thePoly, coefficient& outputSolution
) {
  AlgebraicNumber theAN;
  if (this->theAlgebraicClosurE == 0) {
    return false;
  }
  if (!theAN.ConstructFromMinPoly(
    thePoly, *this->theAlgebraicClosurE, nullptr
  )) {
    return false;
  }
  outputSolution = theAN;
  return true;
}

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::HasImpliedSubstitutions(
  List<Polynomial<coefficient> >& inputSystem, PolynomialSubstitution<coefficient>& outputSub
) {
  int numVars = this->systemSolution.GetElement().size;
  MonomialP tempM;
  Polynomial<coefficient> tempP;
  coefficient theCF;
//  if (theAlgebraicClosure != 0)
//  { global.Comments << "running with non-zero algebraic closure";
//  }
  for (int i = 0; i < inputSystem.size; i ++) {
    tempP = inputSystem[i];
    for (int j = 0; j < numVars; j ++) {
      tempM.MakeEi(j, 1, numVars);
      int indexTempM = tempP.theMonomials.GetIndex(tempM);
      if (indexTempM == - 1) {
        continue;
      }
      theCF = tempP.coefficients[indexTempM];
      tempP.SubtractMonomial(tempM, theCF);
      bool isGood = true;
      for (int k = 0; k < tempP.size(); k ++) {
        if (!(tempP[k](j) == 0)) {
          isGood = false;
          tempP.AddMonomial(tempM, theCF);
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
      //global.Comments << "<br>Current solution candidate is: " << this->systemSolution.GetElement().toString();
      return true;
    }
    int oneVarIndex;
    if (tempP.IsOneVariableNonConstPoly(&oneVarIndex)) {
      if (this->flagUsingAlgebraicClosuRe && this->theAlgebraicClosurE != 0) {
        if (this->GetOneVarPolySolution(tempP, theCF)) {
          outputSub.MakeIdSubstitution(numVars);
          outputSub[oneVarIndex].MakeConst(theCF);
          //check our work:
          tempP.Substitution(outputSub);
          if (!tempP.IsEqualToZero()) {
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

template <class coefficient>
int GroebnerBasisComputation<coefficient>::GetNumEquationsThatWouldBeLinearIfIsubbedVar(
  int theVarIndex, List<Polynomial<coefficient> >& input
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

template <class coefficient>
int GroebnerBasisComputation<coefficient>::GetPreferredSerreSystemSubIndex(List<Polynomial<coefficient> >& inputSystem) {
  Selection varSel;
  this->GetVarsToSolveFor(inputSystem, varSel);
  if (varSel.CardinalitySelection == 0) {
    return - 1;
  }
  int championIndex = varSel.elements[0];
  int championImprovement = GroebnerBasisComputation<coefficient>::GetNumEquationsThatWouldBeLinearIfIsubbedVar(
    championIndex, inputSystem
  );
  for (int i = 1; i < varSel.CardinalitySelection; i ++) {
    if (
      GroebnerBasisComputation<coefficient>::GetNumEquationsThatWouldBeLinearIfIsubbedVar(
        varSel.elements[i], inputSystem
      ) > championImprovement
    ) {
      championIndex = varSel.elements[i];
      championImprovement = GroebnerBasisComputation<coefficient>::GetNumEquationsThatWouldBeLinearIfIsubbedVar(
        varSel.elements[i], inputSystem
      );
    }
  }
  return championIndex;
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::BackSubstituteIntoSinglePoly(
  Polynomial<coefficient>& thePoly, int theIndex, PolynomialSubstitution<coefficient>& theFinalSub
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::BackSubstituteIntoSinglePoly");
  Polynomial<coefficient> tempP;
  tempP.makeMonomial(theIndex, 1, 1);
  if (thePoly == tempP) {
    return;
  }
  thePoly.Substitution(theFinalSub);
  bool changed = false;
  for (int i = 0; i < thePoly.size(); i ++) {
    for (int j = 0; j < thePoly[i].minimalNumberOfVariables(); j ++) {
      if (thePoly[i](j) != 0) {
        if (!this->solutionsFound.GetElement().selected[j]) {
          this->SetSerreLikeSolutionIndex(j, 0);
        } else {
          if (this->systemSolution.GetElement()[j] != 0) {
            global.fatal << "This is a programming error: variable index " << j + 1 << " is supposed to be a free parameter, i.e., be set to zero, but "
            << "instead it has a non-zero value. " << global.fatal;
          }
        }
        theFinalSub[j] = 0;
        changed = true;
      }
    }
  }
  if (changed) {
    thePoly.Substitution(theFinalSub);
  }
  coefficient tempCF;
  if (!thePoly.IsConstant(&tempCF)) {
    global.fatal << "\n<br>\nThis is a programming error: after carrying all implied substitutions "
    << "the polynomial is not a constant, rather equals "
    << thePoly.toString() << ". " << global.fatal;
  }
  theFinalSub[theIndex] = tempCF;
  this->SetSerreLikeSolutionIndex(theIndex, tempCF);
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::BackSubstituteIntoPolySystem(
  List<PolynomialSubstitution<coefficient> >& theImpliedSubs
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::BackSubstituteIntoPolySystem");
  PolynomialSubstitution<coefficient> FinalSub;
  this->GetSubFromPartialSolutionSerreLikeSystem(FinalSub);
  for (int i = theImpliedSubs.size - 1; i >= 0; i --) {
    for (int j = 0; j < theImpliedSubs[i].size; j ++) {
      this->BackSubstituteIntoSinglePoly(theImpliedSubs[i][j], j, FinalSub);
    }
  }
}

template <class coefficient>
int GroebnerBasisComputation<coefficient>::GetNumVarsToSolveFor(const List<Polynomial<coefficient> >& input) {
  Selection theSel;
  GroebnerBasisComputation<coefficient>::GetVarsToSolveFor(input, theSel);
  return theSel.CardinalitySelection;
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::GetVarsToSolveFor(const List<Polynomial<coefficient> >& input, Selection& output) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::GetVarsToSolveFor");
  int NumVars = 0;
  for (int i = 0; i < input.size; i ++) {
    NumVars = MathRoutines::Maximum(NumVars, input[i].minimalNumberOfVariables());
  }
  output.init(NumVars);
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

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::IsContradictoryReducedSystem(const List<Polynomial<coefficient> >& input) {
  if (input.size == 1) {
    if (input[0].IsConstant()) {
      return true;
    }
  }
  return false;
}


template <class coefficient>
void GroebnerBasisComputation<coefficient>::PolySystemSolutionSimplificationPhase(
  List<Polynomial<coefficient> >& inputSystem
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::PolySystemSolutionSimplificationPhase");
  ProgressReport theReport1, theReport2, theReport3;
  if (this->flagDoProgressReport) {
    std::stringstream reportStream;
    reportStream << "Reducing input system without use of heuristics.";
    theReport1.Report(reportStream.str());
  }
  bool changed = true;
  PolynomialSubstitution<coefficient> theSub;
  this->theImpliedSubS.SetSize(0);
  this->theImpliedSubS.Reserve(inputSystem.size);
//  int startingMaxNumSerreSystemComputations = this->MaxNumSerreSystemComputations;
  while (changed) {
    this->NumberGBComputations = 0;
    List<Polynomial<coefficient> > oldSystem = inputSystem;
    bool success = this->TransformToReducedBasis(inputSystem, this->MaxNumBasisReductionComputations);
    if (!success) {
      inputSystem = oldSystem;
    } else {
      oldSystem = inputSystem;
    }
    if (this->flagDoProgressReport) {
      std::stringstream reportStream;
      reportStream << "Attempting to transform system to a groebner basis... ";
      theReport2.Report(reportStream.str());
    }
    if (success && inputSystem.size > 0) {
      this->NumberGBComputations = 0;
      success = this->TransformToReducedGroebnerBasis(inputSystem);
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
        << " starting system to one with " << inputSystem.size << " elements.";
      }
      theReport2.Report(reportStream.str());
    }
    this->NumberSerreSystemComputations += this->NumberGBComputations;
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
        theReport3.Report(reportStream.str());
      }
      this->theImpliedSubS.AddOnTop(theSub);
      for (int i = 0; i < inputSystem.size; i ++) {
        inputSystem[i].Substitution(theSub);
      }
    }
  }
}

template <class coefficient>
std::string GroebnerBasisComputation<coefficient>::ToStringImpliedSubs() {
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

template <class coefficient>
void GroebnerBasisComputation<coefficient>::initForSystemSolution() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::initForSystemSolution");
  this->initForGroebnerComputation(0);
  this->theImpliedSubS.SetSize(0);
  this->flagSystemProvenToHaveNoSolution = false;
  this->flagSystemSolvedOverBaseField = false;
  this->flagSystemProvenToHaveSolution = false;
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::SetUpRecursiveComputation(GroebnerBasisComputation& toBeModified) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::SetUpRecursiveComputation");
  toBeModified.initForSystemSolution();
  toBeModified.RecursionCounterSerreLikeSystem = this->RecursionCounterSerreLikeSystem;
  toBeModified.MaxNumGBComputations = this->MaxNumGBComputations;
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

template <class coefficient>
void GroebnerBasisComputation<coefficient>::ProcessSolvedSubcaseIfSolvedOrProvenToHaveSolution(
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

template <class coefficient>
void GroebnerBasisComputation<coefficient>::TrySettingValueToVariable(
  List<Polynomial<coefficient> >& inputSystem, const Rational& aValueToTryOnPreferredVariable
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::TrySettingValueToVariable");
  ProgressReport theReport1;
  GroebnerBasisComputation& theHeuristicAttempt = this->ComputationUsedInRecursiveCalls.GetElement();
  this->SetUpRecursiveComputation(theHeuristicAttempt);
  int theVarIndex = this->GetPreferredSerreSystemSubIndex(inputSystem);
  if (theVarIndex == - 1) {
    global.fatal << "This is a programming error: preferred substitution variable index is - 1. "
    << "Input system in calculator-input format: <br>"
    << this->ToStringCalculatorInputFromSystem(inputSystem) << "<br>" << global.fatal;
  }
  PolynomialSubstitution<coefficient> theSub;
  theSub.MakeIdSubstitution(this->systemSolution.GetElement().size);
  theSub[theVarIndex] = aValueToTryOnPreferredVariable;
  if (this->flagDoProgressReport) {
    std::stringstream out;
    MonomialP theMon(theVarIndex);
    out << this->ToStringImpliedSubs() << "<br>Attempting an (a priori random) substitution:<br>"
    << theMon.toString(&this->theFormat) << "=" << aValueToTryOnPreferredVariable << ";";
    theReport1.Report(out.str());
  }
  theHeuristicAttempt.SetSerreLikeSolutionIndex(theVarIndex, aValueToTryOnPreferredVariable);
  for (int i = 0; i < inputSystem.size; i ++) {
    inputSystem[i].Substitution(theSub);
  }
  theHeuristicAttempt.SolveSerreLikeSystemRecursively(inputSystem);
  this->NumberSerreSystemComputations+= theHeuristicAttempt.NumberSerreSystemComputations;
  this->ProcessSolvedSubcaseIfSolvedOrProvenToHaveSolution(theHeuristicAttempt);
}

template <class coefficient>
bool GroebnerBasisComputation<coefficient>::HasSingleMonomialEquation(
  const List<Polynomial<coefficient> >& inputSystem, MonomialP& outputMon
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

template <class coefficient>
void GroebnerBasisComputation<coefficient>::SolveWhenSystemHasSingleMonomial(
  List<Polynomial<coefficient> >& inputSystem, const MonomialP& theMon
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::SolveWhenSystemHasSingleMonomial");
  ProgressReport theReport1;
  List<Polynomial<coefficient> > inputSystemCopy = inputSystem;
  bool allProvenToHaveNoSolution = true;
  for (int i = 0; i < theMon.minimalNumberOfVariables(); i ++) {
    if (theMon(i) != 0) {
      if (this->flagDoProgressReport) {
        std::stringstream out;
        MonomialP tempMon(i);
        out << "The system has the single monomial: " << theMon.toString(&this->theFormat)
        << "<br>Trying case:<br>" << tempMon.toString(&this->theFormat) << "= 0;";
        theReport1.Report(out.str());
      }
      PolynomialSubstitution<coefficient> theSub;
      theSub.MakeIdSubstitution(this->systemSolution.GetElement().size);
      theSub[i] = 0;
      GroebnerBasisComputation& theCase = this->ComputationUsedInRecursiveCalls.GetElement();
      this->SetUpRecursiveComputation(theCase);
      theCase.SetSerreLikeSolutionIndex(i, 0);
      inputSystem = inputSystemCopy;
      for (int i = 0; i < inputSystem.size; i ++) {
        inputSystem[i].Substitution(theSub);
      }
      theCase.SolveSerreLikeSystemRecursively(inputSystem);
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

template <class coefficient>
void GroebnerBasisComputation<coefficient>::SolveSerreLikeSystemRecursively(List<Polynomial<coefficient> >& inputSystem) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::SolveSerreLikeSystemRecursively");
  RecursionDepthCounter theCounter(&this->RecursionCounterSerreLikeSystem);
  ProgressReport theReport1, theReport2, theReport3;
  List<Polynomial<coefficient> > startingSystemNoModifications = inputSystem;
  this->NumVarsToSolveForStarT = this->GetNumVarsToSolveFor(inputSystem);
  if (this->flagDoProgressReport) {
    std::stringstream out;
    out << "<hr>Solving Serre-like polynomial system with " << this->NumVarsToSolveForStarT
    << " variables at recursion depth: " << this->RecursionCounterSerreLikeSystem << ". ";
    theReport1.Report(out.str());
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
    theReport2.Report(out.str());
  }
  List<Polynomial<coefficient> > systemBeforeHeuristics = inputSystem;
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
      if (this->ComputationUsedInRecursiveCalls.GetElement().flagSystemProvenToHaveNoSolution) {
        reportStreamHeuristics << " as it resulted in a system which has no solution. ";
      } else {
        reportStreamHeuristics << " as it resulted in a system which exceeded the computation limits. ";
      }
      theReport3.Report(reportStreamHeuristics.str());
    }
  }
  inputSystem = startingSystemNoModifications;
}

template <class coefficient>
std::string GroebnerBasisComputation<coefficient>::toStringDivision(Polynomial<coefficient>& toBeDivided) {
  std::stringstream out;
  out << "Dividing: " << toBeDivided.toString() << "<br>by:<br>" << this->theBasiS.ToStringCommaDelimited();
  return  out.str();
}

template <class coefficient>
std::string GroebnerBasisComputation<coefficient>::ToStringCalculatorInputFromSystem(
  const List<Polynomial<coefficient> >& inputSystem
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

template <class coefficient>
void GroebnerBasisComputation<coefficient>::SolveSerreLikeSystem(List<Polynomial<coefficient> >& inputSystem) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::SolveSerreLikeSystem");
  this->flagSystemProvenToHaveNoSolution = false;
  this->flagSystemSolvedOverBaseField = false;
  this->flagSystemProvenToHaveSolution = false;
  this->RecursionCounterSerreLikeSystem = 0;
  this->NumberGBComputations = 0;
  this->NumberSerreSystemComputations = 0;
  int numVars = 0;
  List<Polynomial<coefficient> > workingSystem = inputSystem;
  for (int i = 0; i < workingSystem.size; i ++) {
    numVars = MathRoutines::Maximum(numVars, workingSystem[i].minimalNumberOfVariables());
  }
  this->systemSolution.GetElement().initializeFillInObject(numVars, 0);
  this->solutionsFound.GetElement().init(numVars);
  ProgressReport theReport;
  std::stringstream reportStream;
  if (this->flagDoProgressReport) {
    reportStream << "Solving system " << this->ToStringCalculatorInputFromSystem(inputSystem);
    theReport.Report(reportStream.str());
  }
  this->NumberSerreVariablesOneGenerator = workingSystem[0].minimalNumberOfVariables() / 2;
  if (this->theAlgebraicClosurE == 0) {
    this->flagTryDirectlySolutionOverAlgebraicClosure = false;
  }
  if (!this->flagTryDirectlySolutionOverAlgebraicClosure) {
    this->flagUsingAlgebraicClosuRe = false;
    this->SolveSerreLikeSystemRecursively(workingSystem);
  }
  if (this->theAlgebraicClosurE != 0) {
    if (!this->flagSystemSolvedOverBaseField && !this->flagSystemProvenToHaveNoSolution) {
      if (this->flagDoProgressReport) {
        if (!this->flagTryDirectlySolutionOverAlgebraicClosure) {
          reportStream << "<br><b>Failed to solve system over the rationals... </b>";
        }
        reportStream << "<br><b>Attempting to solve allowing algebraic extensions.</b> ";
        theReport.Report(reportStream.str());
      }
      this->flagUsingAlgebraicClosuRe = true;
      this->SolveSerreLikeSystemRecursively(workingSystem);
    }
  }
  if (this->flagSystemSolvedOverBaseField) {
    if (this->solutionsFound.GetElement().CardinalitySelection != this->solutionsFound.GetElement().MaxSize) {
      for (int i = 0; i < this->solutionsFound.GetElement().MaxSize; i ++) {
        if (!this->solutionsFound.GetElement().selected[i]) {
          this->SetSerreLikeSolutionIndex(i, 0);
        }
      }
    }
    PolynomialSubstitution<coefficient> theSub;
    this->GetSubFromPartialSolutionSerreLikeSystem(theSub);
    workingSystem = inputSystem;
    for (int i = 0; i < workingSystem.size; i ++) {
      workingSystem[i].Substitution(theSub);
      if (!workingSystem[i].IsEqualToZero()) {
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

template <class coefficient>
std::string GroebnerBasisComputation<coefficient>::ToStringSerreLikeSolution() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::ToStringSerreLikeSolution");
  std::stringstream out;
  Polynomial<Rational> theMon;
  for (int i = 0; i < this->systemSolution.GetElement().size; i ++) {
    if (this->solutionsFound.GetElement().selected[i]) {
      theMon.makeMonomial(i, 1, 1);
      out << " " << theMon.toString(&this->theFormat)
      << " = " << this->systemSolution.GetElement()[i] << ";";
    }
  }
  return out.str();
}

template <class coefficient>
void GroebnerBasisComputation<coefficient>::SetSerreLikeSolutionIndex(
  int theIndex, const coefficient& theConst
) {
  this->systemSolution.GetElement()[theIndex] = theConst;
  if (this->solutionsFound.GetElement().selected[theIndex]) {
    global.fatal << "This a programming error: attempting to set "
    << "value to a variable whose value has already been computed. " << global.fatal;
  }
  this->solutionsFound.GetElement().AddSelectionAppendNewIndex(theIndex);
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
  Polynomial<coefficient> leftTemp, rightTemp, oneMinusT;
  List<Polynomial<coefficient> > theBasis;
  leftTemp = left;
  rightTemp = right;
  int theNumVars = MathRoutines::Maximum(
    left.minimalNumberOfVariables(), right.minimalNumberOfVariables()
  );
  leftTemp.SetNumVariablesSubDeletedVarsByOne(theNumVars + 1);
  rightTemp.SetNumVariablesSubDeletedVarsByOne(theNumVars + 1);
  leftTemp.scaleNormalizeLeadingMonomial();
  rightTemp.scaleNormalizeLeadingMonomial();
  oneMinusT.makeMonomial(theNumVars, 1, one, theNumVars + 1);
  leftTemp *= oneMinusT;
  oneMinusT *= - 1;
  oneMinusT += one;
  rightTemp *= oneMinusT;
  theBasis.size = 0;
  theBasis.AddOnTop(leftTemp);
  theBasis.AddOnTop(rightTemp);
  GroebnerBasisComputation<coefficient> theComp;
  theComp.thePolynomialOrder.theMonOrder = MonomialP::orderForGCD();
  theComp.MaxNumGBComputations = - 1;
  if (!theComp.TransformToReducedGroebnerBasis(theBasis)) {
    global.fatal << "Transformation to reduced "
    << "Groebner basis is not allowed to fail in this function. "
    << global.fatal;
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
  output.scaleNormalizeLeadingMonomial();
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
  output.scaleNormalizeLeadingMonomial();
  return true;
}
#endif
