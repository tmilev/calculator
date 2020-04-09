// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_extra_algebraic_numbers.h"
#include "math_subsets_selections.h"
#include "math_general_implementation.h"

std::string MonomialVector::toString(FormatExpressions* theFormat) const {
  if (theFormat != nullptr) {
    if (this->theIndex < theFormat->vectorSpaceEiBasisNames.size && this->theIndex >= 0) {
      return theFormat->vectorSpaceEiBasisNames[this->theIndex];
    }
  }
  std::stringstream out;
  out << "e_{" << this->theIndex + 1 << "}";
  return out.str();
}

int AlgebraicClosureRationals::GetDimensionOverTheRationals() const {
  return this->latestBasis.size;
}

bool AlgebraicClosureRationals::CheckConsistency() const {
  return true;
}

void AlgebraicClosureRationals::GetMultiplicativeOperatorFromRadicalSelection(
  const Selection& theSel, MatrixTensor<Rational>& outputOp
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::GetMultiplicativeOperatorFromRadicalSelection");
  outputOp.MakeZero();
  Selection vectorActedOnSel, resultVectorSel;
  vectorActedOnSel.init(this->theQuadraticRadicals.size);
  resultVectorSel.init(this->theQuadraticRadicals.size);
  Rational theCoeff;
  MonomialMatrix tempM;
  do {
    theCoeff = 1;
    for (int i = 0; i < this->theQuadraticRadicals.size; i ++) {
      if (vectorActedOnSel.selected[i] && theSel.selected[i]) {
        resultVectorSel.selected[i] = false;
        theCoeff *= this->theQuadraticRadicals[i];
      } else if (!vectorActedOnSel.selected[i] && !theSel.selected[i]) {
        resultVectorSel.selected[i] = false;
      } else {
        resultVectorSel.selected[i] = true;
      }
    }
    resultVectorSel.ComputeIndicesFromSelection();
    tempM.MakeEij(this->GetIndexFromRadicalSelection(resultVectorSel), this->GetIndexFromRadicalSelection(vectorActedOnSel));
    outputOp.AddMonomial(tempM, theCoeff);
  } while (vectorActedOnSel.IncrementReturnFalseIfPastLast());
}

void AlgebraicClosureRationals::ComputeDisplayStringsFromRadicals() {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::ComputeDisplayStringsFromRadicals");
  if (!this->flagIsQuadraticRadicalExtensionRationals) {
    return;
  }
  this->DisplayNamesBasisElements.SetSize(this->latestBasis.size);
  Selection theSel;
  theSel.init(this->theQuadraticRadicals.size);
  do {
    std::stringstream out;
    LargeInteger theRad = 1;
    for (int i = 0; i < theSel.CardinalitySelection; i ++) {
      theRad *= this->theQuadraticRadicals[theSel.elements[i]];
    }
    if (theRad != 1) {
      out << "\\sqrt{" << theRad.toString() << "}";
    }
    this->DisplayNamesBasisElements[this->GetIndexFromRadicalSelection(theSel)] = out.str();
  } while (theSel.IncrementReturnFalseIfPastLast());
}

bool AlgebraicClosureRationals::GetRadicalSelectionFromIndex(int inputIndex, Selection& theSel) {
  if (!this->flagIsQuadraticRadicalExtensionRationals) {
    return false;
  }
  theSel.init(this->theQuadraticRadicals.size);
  int counter = 0;
  while (inputIndex > 0) {
    if (inputIndex % 2 == 1) {
      theSel.AddSelectionAppendNewIndex(counter);
    }
    inputIndex /= 2;
    counter ++;
  }
  return true;
}

int AlgebraicClosureRationals::GetIndexFromRadicalSelection(const Selection& theSel) {
  if (theSel.MaxSize > 30) {
    global.fatal
    << "This is a programming error: the algebraic extension "
    << "is too large to be handled by the current data structures. "
    << global.fatal;
  }
  int result = 0;
  for (int i = theSel.MaxSize - 1; i >= 0; i --) {
    result *= 2;
    if (theSel.selected[i]) {
      result += 1;
    }
  }
  return result;
}

bool AlgebraicClosureRationals::MergeRadicals(const List<LargeInteger>& theRadicals) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::MergeTwoQuadraticRadicalExtensions");
  if (!this->flagIsQuadraticRadicalExtensionRationals) {
    global.fatal << "This is a programming error: AlgebraicClosureRationals::MergeTwoQuadraticRadicalExtensions "
    << "with at least one of two arguments that is not a quadratic radical extension of the rationals. "
    << global.fatal;
  }
  //refine factors:
  HashedList<LargeInteger> radicalsNew = this->theQuadraticRadicals;
  radicalsNew.AddOnTopNoRepetition(theRadicals);
  bool found = true;
  LargeIntegerUnsigned candidateGCD, leftQuotient, rightQuotient;
  while (found) {
    found = false;
    for (int i = 0; i < radicalsNew.size; i ++) {
      for (int j = i + 1; j < radicalsNew.size; j ++) {
        if (radicalsNew[i] == - 1 || radicalsNew[j] == - 1) {
          continue;
        }
        LargeIntegerUnsigned::gcd(radicalsNew[i].value, radicalsNew[j].value, candidateGCD);
        if (candidateGCD > 1) {
          leftQuotient = radicalsNew[i].value / candidateGCD;
          rightQuotient = radicalsNew[j].value / candidateGCD;
          radicalsNew.RemoveIndexSwapWithLast(j);
          radicalsNew.RemoveIndexSwapWithLast(i);
          if (leftQuotient > 1) {
            radicalsNew.AddOnTopNoRepetition(LargeInteger(leftQuotient));
          }
          if (rightQuotient > 1) {
            radicalsNew.AddOnTopNoRepetition(LargeInteger(rightQuotient));
          }
          radicalsNew.AddOnTopNoRepetition(LargeInteger(candidateGCD));
          found = true;
        }
      }
    }
  }
  radicalsNew.QuickSortAscending();
  if (radicalsNew.size > 16) {
    global.Comments << "Computing with fields whose dimension over the "
    << "rationals is greater than 2^16 is not allowed. "
    << "Such computations are too large for the current "
    << "implementation of algberaic extensions of the rationals. "
    << GlobalVariables::Crasher::GetStackTraceEtcErrorMessageHTML();
    return (false);
  }
  this->latestBasis.SetSize(MathRoutines::TwoToTheNth(radicalsNew.size));
  MatrixTensor<Rational> currentInjection;
  currentInjection.MakeZero();
  Selection largerFieldSel, smallerFieldSel;
  largerFieldSel.init(radicalsNew.size);
  smallerFieldSel.init(this->theQuadraticRadicals.size);
  do {
    largerFieldSel.initNoMemoryAllocation();
    for (int j = 0; j < this->theQuadraticRadicals.size; j ++) {
      if (smallerFieldSel.selected[j]) {
        if (this->theQuadraticRadicals[j] == - 1) {
          largerFieldSel.AddSelectionAppendNewIndex(radicalsNew.GetIndex(- 1));
          continue;
        }
        for (int k = 0; k < radicalsNew.size; k ++) {
          if (this->theQuadraticRadicals[j] % radicalsNew[k] == 0 && radicalsNew[k] != - 1) {
            largerFieldSel.AddSelectionAppendNewIndex(k);
          }
        }
      }
    }
    currentInjection.AddMonomial(MonomialMatrix(
        this->GetIndexFromRadicalSelection(largerFieldSel),
        this->GetIndexFromRadicalSelection(smallerFieldSel)
      ), 1
    );
  } while (smallerFieldSel.IncrementReturnFalseIfPastLast());
  this->theQuadraticRadicals = radicalsNew;
  largerFieldSel.init(radicalsNew.size);
  do {
    this->GetMultiplicativeOperatorFromRadicalSelection(
      largerFieldSel,
      this->latestBasis[this->GetIndexFromRadicalSelection(largerFieldSel)]
    );
  } while (largerFieldSel.IncrementReturnFalseIfPastLast());
  this->InjectOldBases(&currentInjection);
  this->AppendAdditiveEiBasis();
  this->ComputeDisplayStringsFromRadicals();
  return true;
}

void AlgebraicClosureRationals::AssignDefaultBasisDisplayNames() {
  this->DisplayNamesBasisElements.SetSize(this->latestBasis.size);
  int extensionNumber = this->basisInjections.size;
  for (int i = 1; i < this->latestBasis.size; i ++) {
    std::stringstream basisVector;
    basisVector << "e_{" << extensionNumber << ", " << i << "}";
    this->DisplayNamesBasisElements[i] = basisVector.str();
  }
}

void AlgebraicClosureRationals::InjectOldBases(
  const MatrixTensor<Rational>* injectionNullForIdentity
) {
  if (injectionNullForIdentity == nullptr) {
    return;
  }
  for (int j = 0; j < this->basisInjections.size; j ++) {
    for (int i = 0; i < this->basisInjections[j].size; i ++) {
      injectionNullForIdentity->ActOnVectorColumn(this->basisInjections[j][i]);
    }
  }
}
void AlgebraicClosureRationals::AppendAdditiveEiBasis() {
  this->basisInjections.SetSize(this->basisInjections.size + 1);
  this->basisInjections.LastObject()->SetSize(this->latestBasis.size);
  for (int i = 0; i < this->latestBasis.size; i ++) {
    (*this->basisInjections.LastObject())[i].MaKeEi(i);
  }
}

bool AlgebraicClosureRationals::ChooseGeneratingElement(
  int attemptsLimitZeroForNone, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::ChooseGeneratingElement");
  SelectionPositiveIntegers theSel;
  int DimensionOverRationals = this->latestBasis.size;
  theSel.init(DimensionOverRationals);
  this->theGeneratingElementPowersBasis.SetSize(0);
  Vector<Rational> currentVect;
  this->GeneratingElemenT.owner = this;
  this->GeneratingElemenT.basisIndex = this->basisInjections.size - 1;
  int attemptsSoFar = 0;
  if (this->basisInjections.size > 1) {
    int indexToSkipFirst = this->basisInjections[this->basisInjections.size - 2].size - 1;
    if (indexToSkipFirst < DimensionOverRationals) {
      theSel.theInts.MakeEi(DimensionOverRationals, indexToSkipFirst);
    }
  }
  for (theSel.IncrementReturnFalseIfPastLast(); ; theSel.IncrementReturnFalseIfPastLast()) {
    attemptsSoFar ++;
    if (attemptsLimitZeroForNone > 0 && attemptsSoFar > attemptsLimitZeroForNone) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Choice of generating element is taking too long: made "
        << attemptsSoFar << " attempts so far, limit: " << attemptsLimitZeroForNone << ". ";
      }
      global.Comments << "Choice of generating element is taking too long: made "
      << attemptsSoFar << " attempts so far, limit: " << attemptsLimitZeroForNone << ". ";
      return false;
    }
    this->GeneratingElemenT.element.MakeZero();
    for (int i = 0; i < theSel.theInts.size; i ++) {
      MonomialVector tempV;
      tempV.MakeEi(i);
      this->GeneratingElemenT.element.AddMonomial(tempV, theSel.theInts[i]);
    }
    this->GetMultiplicationBy(this->GeneratingElemenT, this->GeneratingElementTensorForm);
    this->GeneratingElementTensorForm.GetMatrix(this->GeneratingElementMatForm, DimensionOverRationals);
    this->theGeneratingElementPowersBasis.SetSize(0);
    currentVect.MakeEi(DimensionOverRationals, 0);
    this->theGeneratingElementPowersBasis.AddOnTop(currentVect);
    do {
      this->GeneratingElementMatForm.ActOnVectorColumn(currentVect);
      this->theGeneratingElementPowersBasis.AddOnTop(currentVect);
      if (
        this->theGeneratingElementPowersBasis.size >
        this->theGeneratingElementPowersBasis.GetRankOfSpanOfElements()
      ) {
        this->theGeneratingElementPowersBasis.SetSize(this->theGeneratingElementPowersBasis.size - 1);
        break;
      }
    } while (true);
    if (this->theGeneratingElementPowersBasis.size == DimensionOverRationals) {
      break;
    }
  }
  return true;
}

void AlgebraicClosureRationals::ContractBasesIfRedundant(
  AlgebraicClosureRationals& previousCopy,
  AlgebraicNumber* outputImageGenerator
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::ContractBasesIfRedundant");
  if (
    previousCopy.latestBasis.size != this->latestBasis.size ||
    this->basisInjections.size < 3
  ) {
    return;
  }
  if (outputImageGenerator == nullptr) {
    *this = previousCopy;
    return;
  }
  this->GetAdditionTo(*outputImageGenerator, outputImageGenerator->element);
  MatrixTensor<Rational> reverseMap;

  reverseMap.AssignVectorsToColumns(this->basisInjections[this->basisInjections.size - 3]);
  reverseMap.Invert();
  reverseMap.ActOnVectorColumn(outputImageGenerator->element);
  outputImageGenerator->basisIndex = this->basisInjections.size - 3;
  *this = previousCopy;
}

bool AlgebraicClosureRationals::ReduceMe(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::ReduceMe");
  if (!this->ChooseGeneratingElement(1000, commentsOnFailure)) {
    return false;
  }
  if (this->flagIsQuadraticRadicalExtensionRationals) {
    return true;
  }
  Polynomial<Rational> theMinPoly, smallestFactor;
  theMinPoly.AssignMinPoly(this->GeneratingElementMatForm);
  int theDim = this->latestBasis.size;
  List<Polynomial<Rational> > theFactors;
  bool mustBeTrue = theMinPoly.factorMe(theFactors, nullptr);
  if (!mustBeTrue) {
    global.fatal
    << "This is a programming error: failed to factor polynomial "
    << theMinPoly.toString() << global.fatal;
  }
  smallestFactor = theFactors[0];
  if (smallestFactor.TotalDegreeInt() == theDim) {
    return true;
  }
  MatrixTensor<Rational> generatorPowers, generatorPowersInverse;
  generatorPowers.AssignVectorsToColumns(this->theGeneratingElementPowersBasis);
  generatorPowersInverse = generatorPowers;
  generatorPowersInverse.Invert();
  Polynomial<Rational> zToTheNth, remainderAfterReduction, tempP;
  MatrixTensor<Rational> projectionGeneratorCoordinates;
  int smallestFactorDegree = - 1;
  if (!smallestFactor.TotalDegree().IsSmallInteger(&smallestFactorDegree)) {
    global.fatal
    << "This is a programming error: "
    << smallestFactor.toString()
    << " has non-integral exponent, which should be "
    << "impossible in the current context. "
    << global.fatal;
  }
  projectionGeneratorCoordinates.MakeZero();
  for (int i = 0; i < smallestFactorDegree; i ++) {
    projectionGeneratorCoordinates.AddMonomial(MonomialMatrix(i, i), 1);
  }
  for (int i = smallestFactorDegree; i < theDim; i ++) {
    zToTheNth.MakeMonomiaL(0, i, 1, 1);
    zToTheNth.DivideBy(smallestFactor, tempP, remainderAfterReduction, &MonomialP::orderDefault());
    for (int j = 0; j < remainderAfterReduction.size(); j ++) {
      int theIndex = - 1;
      remainderAfterReduction[j](0).IsSmallInteger(&theIndex);
      projectionGeneratorCoordinates.AddMonomial(
        MonomialMatrix(theIndex, i),
        remainderAfterReduction.coefficients[j]
      );
    }
  }
  List<MatrixTensor<Rational> > newBasis;
  MatrixTensor<Rational> generatorProjected;
  Rational leadingCoefficient = smallestFactor.GetLeadingCoefficient(nullptr);
  for (int i = 0; i < smallestFactorDegree; i ++) {
    MonomialMatrix termBelowMainDiagonal, termInLastColumn;
    if (i + 1 < smallestFactorDegree) {
      termBelowMainDiagonal.MakeEij(i + 1, i);
      generatorProjected.AddMonomial(termBelowMainDiagonal, 1);
    }
    termInLastColumn.MakeEij(i, smallestFactorDegree - 1);
    Rational coefficientLastColumn = - smallestFactor.GetMonomialCoefficient(MonomialP(0, i));
    coefficientLastColumn /= leadingCoefficient;
    generatorProjected.AddMonomial(termInLastColumn, coefficientLastColumn);
    if (coefficientLastColumn != 0 && generatorProjected.IsEqualToZero()) {
      global.fatal << "We shouldn't have zero generator. " << global.fatal;
    }
  }
  newBasis.SetSize(smallestFactorDegree);
  newBasis[0].MakeId(smallestFactorDegree);
  newBasis[1] = generatorProjected;
  for (int i = 2; i < smallestFactorDegree; i ++) {
    newBasis[i] = generatorProjected;
    newBasis[i].RaiseToPower(i);
  }
  this->latestBasis = newBasis;
  MatrixTensor<Rational> injection;
  injection = projectionGeneratorCoordinates;
  injection *= generatorPowersInverse;
  this->InjectOldBases(&injection);
  this->AppendAdditiveEiBasis();
  this->AssignDefaultBasisDisplayNames();
  if (!this->ChooseGeneratingElement(1000, commentsOnFailure)) {
    return false;
  }
  this->GetMultiplicationBy(
    this->GeneratingElemenT, this->GeneratingElementTensorForm
  );
  this->GeneratingElementTensorForm.GetMatrix(
    this->GeneratingElementMatForm, this->latestBasis.size
  );
  return true;
}

void AlgebraicClosureRationals::GetAdditionTo(
  const AlgebraicNumber& input, VectorSparse<Rational>& output
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::GetAdditionTo");
  if (&output == &input.element) {
    AlgebraicNumber anCopy = input;
    this->GetAdditionTo(anCopy, output);
    return;
  }
  if (input.owner == nullptr) {
    if (input.element.size() > 0) {
      output.MaKeEi(0, input.element.coefficients[0]);
    }
    return;
  }
  if (
    input.basisIndex < 0 ||
    input.basisIndex >= this->basisInjections.size
  ) {
    global.fatal
    << "This is a programming error: element has out-of-range basis index "
    << input.basisIndex << ". "
    << global.fatal;
  }
  // The basis is already the latest one.
  // The map in the last element of oldBasesInjectedIntoLatest
  // is the identity as it describes the map of the latest basis
  // into itself.
  if (input.basisIndex == this->basisInjections.size - 1) {
    output = input.element;
    return;
  }
  output.MakeZero();
  for (int i = 0; i < input.element.size(); i ++) {
    int currentIndex = input.element[i].theIndex;
    if (
      currentIndex < 0 ||
      currentIndex >= this->basisInjections[input.basisIndex].size
    ) {
      global.fatal
      << "This is a programming error: I am getting basis index "
      << input.basisIndex
      << " with current index " << currentIndex
      << ". A printout of the algebraic closure follows. "
      << this->toString() << global.fatal;
    }
    output.AddOtherTimesConst(
      this->basisInjections[input.basisIndex][currentIndex],
      input.element.coefficients[i]
    );
  }
}

void AlgebraicClosureRationals::GetMultiplicationBy(
  const AlgebraicNumber& input, MatrixTensor<Rational>& output
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::GetMultiplicationBy");
  output.MakeZero();
  VectorSparse<Rational> inputAdditiveForm;
  this->GetAdditionTo(input, inputAdditiveForm);
  MatrixTensor<Rational> currentMat;
  for (int i = 0; i < inputAdditiveForm.size(); i ++) {
    if (
      inputAdditiveForm[i].theIndex < 0 ||
      inputAdditiveForm[i].theIndex >= this->latestBasis.size
    ) {
      global.fatal << "This is a programming error: element " << input.toString()
      << " has bad index, namely, " << inputAdditiveForm[i].theIndex
      << ". The algebraic closure is: " << this->toString() << ". " << global.fatal;
    }
    currentMat = this->latestBasis[inputAdditiveForm[i].theIndex];
    currentMat *= inputAdditiveForm.coefficients[i];
    output += currentMat;
  }
}

bool AlgebraicNumber::AssignCosRationalTimesPi(const Rational& input, AlgebraicClosureRationals& inputOwner) {
  MacroRegisterFunctionWithName("AlgebraicNumber::AssignCosRationalTimesPi");
  Rational fracPart = input;
  fracPart.AssignFracValue();
  Rational halfIntegerPart = input * 2;
  halfIntegerPart.AssignFloor();
  LargeInteger halfIntegerPartTimesTwo;
  if (!halfIntegerPart.IsInteger(&halfIntegerPartTimesTwo)) {
    global.fatal << "something went wrong: floor function returns non-integer" << global.fatal;
  }
  halfIntegerPart /= 2;
  Rational halfFracPart = input - halfIntegerPart;
  Rational half(1, 2);
  halfIntegerPartTimesTwo %= 4;
  int sign = 1;
  if (halfIntegerPartTimesTwo == 1) {
    sign = - 1;
    halfFracPart = half - halfFracPart;
  }
  if (halfIntegerPartTimesTwo == 2) {
    sign = - 1;
  }
  if (halfIntegerPartTimesTwo == 3) {
    halfFracPart = half - halfFracPart;
  }
  if (halfFracPart == Rational(1, 12)) {
    AlgebraicNumber sqrt6, sqrt2;
    sqrt6.AssignRationalQuadraticRadical(6, inputOwner, nullptr);
    sqrt2.AssignRationalQuadraticRadical(2, inputOwner, nullptr);
    *this = sqrt6 + sqrt2;
    *this /= 4 * sign;
    return true;
  }
  if (halfFracPart == Rational(5, 12)) {
    AlgebraicNumber sqrt6, sqrt2;
    sqrt6.AssignRationalQuadraticRadical(6, inputOwner, nullptr);
    sqrt2.AssignRationalQuadraticRadical(2, inputOwner, nullptr);
    *this = sqrt6 - sqrt2;
    *this /= 4 * sign;
    return true;
  }
  if (halfFracPart == Rational(1, 6)) {
    this->AssignRationalQuadraticRadical(3, inputOwner, nullptr);
    *this /= 2;
    *this *= sign;
    return true;
  }
  if (halfFracPart == Rational(1, 4)) {
    this->AssignRationalQuadraticRadical(2, inputOwner, nullptr);
    *this /= 2;
    *this *= sign;
    return true;
  }
  if (halfFracPart == Rational(1, 3)) {
    *this = half * sign;
    return true;
  }
  if (halfFracPart == half) {
    *this = 0;
    return true;
  }
  if (halfFracPart == 0) {
    *this = sign;
    return true;
  }
  return false;
}


unsigned int AlgebraicNumber::HashFunction() const {
  //global.fatal << global.fatal;
  //WARNING. Algebraic numbers, as they are recorded in memory at the moment,
  //do not have unique presentations, so we return 0 as their hash function.
  //Computing a hash function can be done, for example, by picking the hash function of the minimal polynomial
  //over the rationals. However, such computations appear to be too heavy, and will make sense only if we need to deal
  //with large sets of algebraic numbers without repetition.
  return 0;
}

bool AlgebraicNumber::operator==(const Rational& other) const {
  if (this->element.IsEqualToZero()) {
    return other == 0;
  }
  if (this->element.size() != 1) {
    return false;
  }
  if (this->element[0].theIndex != 0) {
    return false;
  }
  return this->element.coefficients[0] == other;
}

bool AlgebraicNumber::NeedsParenthesisForMultiplication(
  FormatExpressions* unused
) const {
  (void) unused;
  if (this->owner == nullptr) {
    return false;
  }
  VectorSparse<Rational> additiveForm;
  this->owner->GetAdditionTo(*this, additiveForm);
  return (additiveForm.size() > 1);
}

bool AlgebraicNumber::NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost() const {
  if (this->owner == nullptr) {
    Rational tempRat;
    if (this->IsRational(&tempRat)) {
      return tempRat.NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost();
    }
    return false;
  }
  VectorSparse<Rational> additiveForm;
  this->owner->GetAdditionTo(*this, additiveForm);
  if (additiveForm.size() != 1) {
    return true;
  }
  return additiveForm.coefficients[0].NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost();
}

bool AlgebraicNumber::CheckNonZeroOwner() const {
  if (this->owner == nullptr) {
    global.fatal << "This is a programming error: algebraic number with "
    << "non-initialized owner not permitted in the current context." << global.fatal;
  }
  return true;
}

bool AlgebraicNumber::ConstructFromMinPoly(
  const Polynomial<AlgebraicNumber>& thePoly,
  AlgebraicClosureRationals& inputOwner,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("AlgebraicNumber::ConstructFromMinPoly");
  return inputOwner.AdjoinRootMinimalPolynomial(
    thePoly, *this, commentsOnFailure
  );
}

void AlgebraicClosureRationals::reset() {
  this->flagIsQuadraticRadicalExtensionRationals = true;
  this->latestBasis.SetSize(1);
  this->latestBasis[0].MakeId(1);

  this->theGeneratingElementPowersBasis.SetSize(1);
  this->theGeneratingElementPowersBasis[0].MakeEi(1, 0);
  this->basisInjections.SetSize(1);
  this->basisInjections[0].SetSize(1);
  this->basisInjections[0][0].MaKeEi(0);
  this->theQuadraticRadicals.Clear();
  this->DisplayNamesBasisElements.SetSize(1);
  this->DisplayNamesBasisElements[0] = "";
  this->GeneratingElementTensorForm.MakeId(1);
  this->GeneratingElementMatForm.MakeIdMatrix(1);
  this->GeneratingElemenT.owner = this;
  this->GeneratingElemenT.element.MaKeEi(0);
}

bool AlgebraicClosureRationals::AdjoinRootQuadraticPolyToQuadraticRadicalExtension(
  const Polynomial<AlgebraicNumber>& thePoly,
  AlgebraicNumber& outputRoot,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::AdjoinRootQuadraticPolyToQuadraticRadicalExtension");
  if (thePoly.TotalDegree() != 2 || !this->flagIsQuadraticRadicalExtensionRationals) {
    return false;
  }
  Polynomial<AlgebraicNumber> algNumPoly;
  this->ConvertPolyDependingOneVariableToPolyDependingOnFirstVariableNoFail(thePoly, algNumPoly);
  Polynomial<Rational> minPoly;
  minPoly.MakeZero();
  Rational currentCF, theLinearTermCFdividedByTwo, theConstTermShifted;
  for (int i = 0; i < algNumPoly.size(); i ++) {
    if (!algNumPoly.coefficients[i].IsRational(&currentCF)) {
      return false;
    } else {
      minPoly.AddMonomial(algNumPoly[i], currentCF);
    }
  }
  List<MonomialP>::Comparator* monomialOrder = &MonomialP::orderDefault();
  minPoly /= minPoly.GetLeadingCoefficient(monomialOrder);
  minPoly.GetCoeffInFrontOfLinearTermVariableIndex(0, theLinearTermCFdividedByTwo);
  theLinearTermCFdividedByTwo /= 2;
  minPoly.GetConstantTerm(theConstTermShifted);
  theConstTermShifted -= theLinearTermCFdividedByTwo*theLinearTermCFdividedByTwo;
  theConstTermShifted *= - 1;
  if (!outputRoot.AssignRationalQuadraticRadical(
    theConstTermShifted, *this, commentsOnFailure
  )) {
    return false;
  }
  outputRoot -= theLinearTermCFdividedByTwo;
  //Check our work:
  PolynomialSubstitution<AlgebraicNumber> checkSub;
  checkSub.SetSize(1);
  checkSub[0].MakeConst(outputRoot);
  algNumPoly.Substitution(checkSub);
  if (!algNumPoly.IsEqualToZero()) {
    global.fatal << "This is a programming error. The number z = " << outputRoot.toString()
    << " was just adjoined to a quadratic radical extension of the rationals; z "
    << "was given by requesting that it has minimial polynomial " << algNumPoly.toString()
    << ", however, substituting z back in to the minimal polynomial "
    << "does not yield zero, rather yields " << algNumPoly.toString() << ". " << global.fatal;
  }
  return true;
}

void AlgebraicClosureRationals::ConvertPolyDependingOneVariableToPolyDependingOnFirstVariableNoFail(
  const Polynomial<AlgebraicNumber>& input, Polynomial<AlgebraicNumber>& output
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::ConvertPolyDependingOneVariableToPolyDependingOnFirstVariableNoFail");
  int indexVar = - 1;
  if (!input.IsOneVariableNonConstPoly(&indexVar)) {
    global.fatal << "This is a programming error: "
    << "I am being asked convert to a one-variable polynomial a polynomial "
    << "depending on more than one variables. "
    << "The input poly is: " << input.toString() << global.fatal;
  }
  PolynomialSubstitution<AlgebraicNumber> theSub;
  theSub.MakeIdSubstitution(indexVar + 1);
  theSub[indexVar].MakeMonomiaL(0, 1, 1);
  output = input;
  output.Substitution(theSub);
}

bool AlgebraicClosureRationals::AdjoinRootMinimalPolynomial(
  const Polynomial<AlgebraicNumber>& thePoly,
  AlgebraicNumber& outputRoot,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::AdjoinRootMinPoly");
  if (this->AdjoinRootQuadraticPolyToQuadraticRadicalExtension(
    thePoly, outputRoot, commentsOnFailure
  )) {
    return true;
  }
  Polynomial<AlgebraicNumber> minPoly;
  this->ConvertPolyDependingOneVariableToPolyDependingOnFirstVariableNoFail(thePoly, minPoly);
  List<MonomialP>::Comparator* monomialOrder = &MonomialP::orderDefault();
  AlgebraicNumber leadingCoefficient = minPoly.GetLeadingCoefficient(monomialOrder);
  minPoly /= leadingCoefficient;
  AlgebraicClosureRationals backUpCopy;
  backUpCopy = *this;
  MatrixTensor<Rational> theGenMat;
  int degreeMinPoly = minPoly.TotalDegreeInt();
  int startingDimension = this->latestBasis.size;
  if (
    degreeMinPoly * startingDimension > 10000 ||
    startingDimension > 10000 ||
    degreeMinPoly > 10000
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "<hr>Adjoining root of minimial polynomial failed: "
      << "the current field extension dimension over the rationals is "
      << startingDimension << ", the degree of the minimial polynomial is "
      << degreeMinPoly
      << ", yielding expected final dimension "
      << startingDimension << "*" << degreeMinPoly << " = "
      << startingDimension * degreeMinPoly
      << " over the rationals. The calculator is hard-coded "
      << "to accept dimension over the rationals no larger than 10000 "
      << "- multiplication in such a field corresponds to a "
      << "10000x10000 matrix "
      << "(100 000 000 entries, expected in general to be dense). "
      << "If you do indeed want to carry out "
      << "such large computations, you need to "
      << "compile the calculator on your own, modifying file "
      << __FILE__ << ", line " << __LINE__ << ".";
    }
    return false;
  }
  theGenMat.MakeZero();
  for (int i = 0; i < degreeMinPoly - 1; i ++) {
    for (int j = 0; j < startingDimension; j ++) {
      theGenMat.AddMonomial(MonomialMatrix((i + 1) * startingDimension + j, i * startingDimension + j), 1);
    }
  }
  Polynomial<AlgebraicNumber> minusMinimalPolynomialMinusMaximalMonomial = minPoly;
  MonomialP leadingMonomial;
  AlgebraicNumber leadingCoefficientModified;
  minusMinimalPolynomialMinusMaximalMonomial.GetIndexLeadingMonomial(
    &leadingMonomial, &leadingCoefficientModified, monomialOrder
  );
  minusMinimalPolynomialMinusMaximalMonomial.SubtractMonomial(leadingMonomial, leadingCoefficientModified);
  minusMinimalPolynomialMinusMaximalMonomial /= leadingCoefficientModified;
  minusMinimalPolynomialMinusMaximalMonomial *= - 1;
  MatrixTensor<Rational> currentCoeffMatForm;
  for (int i = 0; i < minusMinimalPolynomialMinusMaximalMonomial.size(); i ++) {
    AlgebraicNumber& currentCoeff = minusMinimalPolynomialMinusMaximalMonomial.coefficients[i];
    const MonomialP& currentMon = minusMinimalPolynomialMinusMaximalMonomial[i];
    this->GetMultiplicationBy(currentCoeff, currentCoeffMatForm);
    for (int j = 0; j < currentCoeffMatForm.size(); j ++) {
      int relRowIndex = currentCoeffMatForm[j].vIndex;
      int relColIndex = currentCoeffMatForm[j].dualIndex;
      if (relRowIndex == - 1 || relColIndex == - 1) {
        global.fatal << "This is a programming error: non initialized monomial. " << global.fatal;
      }
      theGenMat.AddMonomial(
        MonomialMatrix(
          currentMon.TotalDegreeInt() * startingDimension + relRowIndex,
          startingDimension * (degreeMinPoly - 1) + relColIndex
        ),
        currentCoeffMatForm.coefficients[j]
      );
    }
  }
  int finalDimension = degreeMinPoly * startingDimension;
  List<MatrixTensor<Rational> > finalBasis;
  finalBasis.SetSize(this->latestBasis.size);
  MatrixTensor<Rational> theGenMatPower;
  theGenMatPower.MakeId(degreeMinPoly);
  for (int i = 0; i < startingDimension; i ++) {
    finalBasis[i].AssignTensorProduct(theGenMatPower, this->latestBasis[i]);
  }
  this->latestBasis = finalBasis;
  this->latestBasis.SetSize(finalDimension);
  theGenMatPower = theGenMat;
  for (int i = 1; i < degreeMinPoly; i ++) {
    for (int j = 0; j < startingDimension; j ++) {
      this->latestBasis[i * startingDimension + j] = theGenMatPower;
      this->latestBasis[i * startingDimension + j] *= finalBasis[j];
    }
    if (i != degreeMinPoly - 1) {
      theGenMatPower *= theGenMat;
    }
  }
  this->AppendAdditiveEiBasis();
  outputRoot.owner = this;
  outputRoot.element.MaKeEi(startingDimension);
  outputRoot.basisIndex = this->basisInjections.size - 1;
  this->flagIsQuadraticRadicalExtensionRationals = false;
  if (!this->ReduceMe(commentsOnFailure)) {
    *this = backUpCopy;
    return false;
  }
  // Suppose the minimal polynomial we are adjoining is
  // a_0 x^n + a_1 x^{n-1} + ...
  // If, after reduction, the field is contracted to the original,
  // this means that the polynomial was really not minimal
  // and had a linear factor over the base field. In that
  // case, we may declare the base field root of that polynomial to be
  // the solution of our polynomial and so we want to know the image of
  // x as that will give us the solution in question.
  this->ContractBasesIfRedundant(backUpCopy, &outputRoot);


  // Sanity check code here:
  PolynomialSubstitution<AlgebraicNumber> theSub;
  theSub.SetSize(1);
  theSub[0].MakeConst(outputRoot);
  Polynomial<AlgebraicNumber> substitutedMinPoly;
  substitutedMinPoly = minPoly;
  substitutedMinPoly.Substitution(theSub);
  if (!substitutedMinPoly.IsEqualToZero()) {
    global.fatal << "This is a programming error. The number z = "
    << outputRoot.toString() << " was just adjoined to the base field; z "
    << "was given by requesting that it has minimial polynomial "
    << minPoly.toString()
    << ", however, substituting z back in to the minimal polynomial "
    << "does not yield zero, rather yields "
    << substitutedMinPoly.toString() << ". "
    << "The algebraic closure printout follows. "
    << this->ToStringFull() << global.fatal;
  }
  return true;
}

void AlgebraicNumber::Invert() {
  MacroRegisterFunctionWithName("AlgebraicNumber::Invert");
  if (this->owner == nullptr) {
    if (this->element.IsEqualToZero()) {
      global.fatal << "This is a programming error: division by zero. " << global.fatal;
    }
    bool isGood = (this->element.size() == 1);
    if (isGood) {
      isGood = (this->element[0].theIndex == 0);
    }
    if (!isGood) {
      global.fatal << "This is a programming error: Algebraic number has no owner, "
      << "so it must be rational, but it appears to be not. "
      << "Its theElt vector is: " << this->element.toString() << global.fatal;
    }
    this->element.coefficients[0].Invert();
    return;
  }
  MatrixTensor<Rational> theInverted;
  Matrix<Rational> tempMat2;
  this->owner->GetMultiplicationBy(*this, theInverted);
  theInverted.GetMatrix(tempMat2, this->owner->latestBasis.size);
  tempMat2.Invert();
  theInverted = tempMat2;
  this->element.MaKeEi(0);
  theInverted.ActOnVectorColumn(this->element);
  this->basisIndex = this->owner->basisInjections.size - 1;
}

void AlgebraicNumber::operator/=(const AlgebraicNumber& other) {
  MacroRegisterFunctionWithName("AlgebraicNumber::operator/=");
  AlgebraicNumber otherCopy = other;
  otherCopy.Invert();
  *this *= otherCopy;
}

void AlgebraicNumber::operator*=(const Rational& other) {
  this->element *= other;
}

bool AlgebraicNumber::CheckCommonOwner(const AlgebraicNumber& other) const {
  if (this->owner == nullptr || other.owner == nullptr) {
    return true;
  }
  if (this->owner != other.owner) {
    global.fatal << "This is a programming error. Two algebraic numbers have "
    << "different algebraic closures when they shouldn't. " << global.fatal;
    return false;
  }
  return true;
}

void AlgebraicNumber::operator-=(const AlgebraicNumber& other) {
  MacroRegisterFunctionWithName("AlgebraicNumber::operator=");
  this->CheckCommonOwner(other);
  if (this->basisIndex == other.basisIndex) {
    this->element -= other.element;
    return;
  }
  AlgebraicClosureRationals* theOwner = this->owner;
  if (theOwner == nullptr) {
    theOwner = other.owner;
  }
  if (theOwner == nullptr && this->basisIndex != other.basisIndex) {
    global.fatal << "This is a programming error: algebraic numbers "
    << "with zero owners but different basis indices. " << global.fatal;
  }
  VectorSparse<Rational> AdditiveFormOther;
  theOwner->GetAdditionTo(*this, this->element);
  theOwner->GetAdditionTo(other, AdditiveFormOther);
  this->owner = theOwner;
  this->basisIndex = theOwner->basisInjections.size - 1;
  this->element -= AdditiveFormOther;
}

void AlgebraicNumber::operator+=(const AlgebraicNumber& other) {
  MacroRegisterFunctionWithName("AlgebraicNumber::operator+=");
  this->CheckConsistency();
  other.CheckConsistency();
  if (this == &other) {
    AlgebraicNumber otherCopy = other;
    *this += otherCopy;
    this->CheckConsistency();
    return;
  }
  this->CheckCommonOwner(other);
  AlgebraicClosureRationals* theOwner = this->owner;
  if (theOwner == nullptr) {
    theOwner = other.owner;
  }
  if (theOwner == nullptr && this->basisIndex != other.basisIndex) {
    global.fatal << "This is a programming error: algebraic numbers: "
    << this->toString() << " and " << other.toString()
    << " have with zero owners but different basis indices. " << global.fatal;
  }
  if (this->basisIndex == other.basisIndex) {
    this->owner = theOwner;
    this->CheckConsistency();
    other.CheckConsistency();
    this->element += other.element;
    this->CheckConsistency();
    return;
  }
  VectorSparse<Rational> AdditiveFormOther;
  theOwner->GetAdditionTo(*this, this->element);
  theOwner->GetAdditionTo(other, AdditiveFormOther);
  this->owner = theOwner;
  this->basisIndex = theOwner->basisInjections.size - 1;
  this->element += AdditiveFormOther;
  this->CheckConsistency();
}

bool AlgebraicNumber::CheckConsistency() const {
  if (this->flagDeallocated) {
    global.fatal << "This is a programming error: use after free of AlgebraicNumber. " << global.fatal;
  }
  if (this->owner == nullptr) {
    if (!this->IsRational()) {
      for (int i = 0; i < this->element.size(); i ++) {
        global.Comments << "<br>index: " << this->element[i].theIndex << ", coefficient: "
        << this->element.coefficients[i];
      }
      global.fatal << "Detected non-rational algebraic number with zero owner. " << global.fatal;
    }
    if (this->basisIndex != 0) {
      global.fatal << "Algebraic number with non-zero basis and zero owner. " << global.fatal;
    }
  }
  return true;
}

void AlgebraicNumber::operator*=(const AlgebraicNumber& other) {
  MacroRegisterFunctionWithName("AlgebraicNumber::operator*=");
  if (this == &other) {
    AlgebraicNumber otherCopy = other;
    *this *= otherCopy;
    return;
  }
  this->CheckConsistency();
  if (other.owner == nullptr) {
    if (other.IsEqualToZero()) {
      this->element.MakeZero();
      return;
    }
    this->element *= other.element.coefficients[0];
    return;
  }
  if (this->owner == nullptr) {
    if (this->element.IsEqualToZero()) {
      return;
    }
    Rational tempRat = this->element.GetMonomialCoefficient(MonomialVector(0));
    *this = other;
    *this *= tempRat;
    return;
  }
  bool doReport = (this->element.size() * other.element.size() > 100);
  ProgressReport theReport;
  if (doReport) {
    std::stringstream reportStream;
    reportStream << "Large multiplication:<br>"
    << this->toString() << "\n<br>by<br>\n" << other.toString()
    << "\n<br>...";
    theReport.Report(reportStream.str());
  }
  this->CheckCommonOwner(other);
  MatrixTensor<Rational> leftMat, rightMat;
  this->owner->GetMultiplicationBy(*this, leftMat);
  this->owner->GetMultiplicationBy(other, rightMat);
  leftMat.CheckConsistencyGrandMaster();
  rightMat.CheckConsistencyGrandMaster();
  leftMat *= rightMat;
  this->basisIndex = this->owner->basisInjections.size - 1;
  this->element.MaKeEi(0);
  leftMat.ActOnVectorColumn(this->element);
}

void AlgebraicNumber::SqrtMeDefault(std::stringstream* commentsOnError) {
  this->RadicalMeDefault(2, commentsOnError);
}

bool AlgebraicNumber::operator>(const AlgebraicNumber& other) const {
  Rational left, right;
  if (this->IsRational(&left) && other.IsRational(&right)) {
    return left > right;
  }
  this->CheckCommonOwner(other);
  return this->element > other.element;
}

void AlgebraicNumber::AssignRational(const Rational& input, AlgebraicClosureRationals& inputOwner) {
  this->basisIndex = 0;
  this->owner = &inputOwner;
  this->element.MaKeEi(0, input);
}

AlgebraicNumber AlgebraicNumber::zero() {
  AlgebraicNumber result;
  return result;
}

bool AlgebraicNumber::IsExpressedViaLatestBasis() const {
  if (this->owner == nullptr) {
    return true;
  }
  return this->basisIndex == this->owner->basisInjections.size - 1;
}

void AlgebraicNumber::ExpressViaLatestBasis() {
  if (this->owner == nullptr) {
    return;
  }
  if (this->basisIndex == this->owner->basisInjections.size - 1) {
    return;
  }
  this->owner->GetAdditionTo(*this, this->element);
  this->basisIndex = this->owner->basisInjections.size - 1;
}

bool AlgebraicNumber::EvaluatesToDouble(double* outputWhichDouble) const {
  MacroRegisterFunctionWithName("AlgebraicNumber::EvaluatesToDouble");
  if (!this->IsExpressedViaLatestBasis()) {
    AlgebraicNumber thisCopy = *this;
    thisCopy.ExpressViaLatestBasis();
    return thisCopy.EvaluatesToDouble(outputWhichDouble);
  }
  Rational ratValue;
  if (this->IsRational(&ratValue)) {
    if (outputWhichDouble != nullptr) {
      *outputWhichDouble = ratValue.GetDoubleValue();
    }
    return true;
  }
  if (this->owner == nullptr) {
    global.fatal << "Owner is zero but algebraic number is not rational. " << global.fatal;
  }
  if (!this->owner->flagIsQuadraticRadicalExtensionRationals) {
    return false;
  }
  if (outputWhichDouble != nullptr) {
    *outputWhichDouble = 0;
  }
  Selection currentRadicalSelection;
  double currentMultiplicand = 0;
  for (int i = 0; i < this->element.size(); i ++) {
    this->owner->GetRadicalSelectionFromIndex(this->element[i].theIndex, currentRadicalSelection);
    if (outputWhichDouble != nullptr) {
      currentMultiplicand = this->element.coefficients[i].GetDoubleValue();
    }
    for (int j = 0; j < currentRadicalSelection.CardinalitySelection; j ++) {
      if (this->owner->theQuadraticRadicals[currentRadicalSelection.elements[j]] < 0) {
        return false;
      } else {
        if (outputWhichDouble != nullptr) {
          currentMultiplicand *= FloatingPoint::Sqrt(
            this->owner->theQuadraticRadicals[currentRadicalSelection.elements[j]].GetDoubleValue()
          );
        }
      }
    }
    if (outputWhichDouble != nullptr) {
      *outputWhichDouble += currentMultiplicand;
    }
  }
  return true;
}

bool AlgebraicNumber::AssignRationalQuadraticRadical(
  const Rational& inpuT,
  AlgebraicClosureRationals& inputOwner,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("AlgebraicNumber::AssignRationalRadical");
  this->CheckConsistency();
  if (inpuT == 0) {
    return false;
  }
  if (!inputOwner.flagIsQuadraticRadicalExtensionRationals) {
    Polynomial<AlgebraicNumber> minPoly;
    minPoly.MakeMonomiaL(0, 2);
    minPoly -= inpuT;
    bool result = this->ConstructFromMinPoly(minPoly, inputOwner, commentsOnFailure);
    if (result) {
      this->CheckConsistency();
    }
    return result;
  }
  List<LargeInteger> theFactors;
  Rational absoluteInput = inpuT;
  if (absoluteInput < 0) {
    theFactors.AddOnTop(- 1);
    absoluteInput *= - 1;
  }
  LargeInteger squareFreeInput = absoluteInput.GetNumerator();
  squareFreeInput *= absoluteInput.GetDenominator();
  List<LargeInteger> primeFactors;
  List<int> theMults;
  if (!squareFreeInput.value.FactorReturnFalseIfFactorizationIncomplete(
    primeFactors, theMults, 0, nullptr
  )) {
    return false;
  }
  squareFreeInput.value = 1;
  Rational squareRootRationalPart = 1;
  squareRootRationalPart /= absoluteInput.GetDenominator();
  for (int i = 0; i < primeFactors.size; i ++) {
    if (theMults[i] % 2 == 1) {
      squareFreeInput *= primeFactors[i];
    }
    primeFactors[i].RaiseToPower(theMults[i] / 2);
    squareRootRationalPart *= primeFactors[i];
  }
  if (!squareFreeInput.IsEqualToOne()) {
    theFactors.AddOnTop(squareFreeInput);
  }
  if (theFactors.size == 0) {
    this->AssignRational(squareRootRationalPart, inputOwner);
    this->CheckConsistency();
    return true;
  }
  if (!inputOwner.MergeRadicals(theFactors)) {
    return false;
  }
  Selection FactorSel;
  FactorSel.init(inputOwner.theQuadraticRadicals.size);
  for (int i = 0; i < theFactors.size; i ++) {
    if (theFactors[i] == - 1) {
      FactorSel.AddSelectionAppendNewIndex(inputOwner.theQuadraticRadicals.GetIndex(- 1));
    } else {
      for (int j = 0; j < inputOwner.theQuadraticRadicals.size; j ++) {
        if (inputOwner.theQuadraticRadicals[j] != - 1) {
          if (theFactors[i] % inputOwner.theQuadraticRadicals[j] == 0) {
            FactorSel.AddSelectionAppendNewIndex(j);
          }
        }
      }
    }
  }
  this->owner = &inputOwner;
  this->basisIndex = this->owner->basisInjections.size - 1;
  this->element.MaKeEi(this->owner->GetIndexFromRadicalSelection(FactorSel));
  this->element *= squareRootRationalPart;
  this->CheckConsistency();
  return true;
}

bool AlgebraicNumber::RadicalMeDefault(
  int radical, std::stringstream* commentsOnError
) {
  MacroRegisterFunctionWithName("AlgebraicNumber::RadicalMeDefault");
  if (this->owner == nullptr) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Failed to extract radical: algebraic closure is missing. ";
    }
    return false;
  }
  AlgebraicNumber result, one, minusOne;
  one.AssignRational(1, *this->owner);
  minusOne.AssignRational(- 1, *this->owner);
  Polynomial<AlgebraicNumber> thePolynomial;
  thePolynomial.AddConstant(*this * minusOne);
  MonomialP leadingMonomial;
  leadingMonomial.MakeEi(0, radical);
  thePolynomial.AddMonomial(leadingMonomial, one);
  if (!this->owner->AdjoinRootMinimalPolynomial(
    thePolynomial, result, commentsOnError
  )) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Failed to extend algebraic closure field. ";
    }
    return false;
  }
  *this = result;
  return true;
}

std::string AlgebraicClosureRationals::ToStringQuadraticRadical(
  FormatExpressions *theFormat
) const {
  (void) theFormat;
  std::stringstream out;
  out << "\\mathbb Q[";
  for (int i = 0; i < this->theQuadraticRadicals.size; i ++) {
    out << "\\sqrt{" << this->theQuadraticRadicals[i].toString() << "}";
    if (i != this->theQuadraticRadicals.size - 1) {
      out << ", ";
    }
  }
  out << "]";
  return HtmlRoutines::GetMathSpanPure(out.str());
}

std::string AlgebraicClosureRationals::ToStringFull(FormatExpressions* theFormat) const {
  std::stringstream out;
  out << "Dimension over the rationals: "
  << this->latestBasis.size << ". Multiplicative basis follows. ";
  for (int i = 0; i < this->latestBasis.size; i ++) {
    out << "<br>";
    std::stringstream theFlaStream;
    if (i < this->DisplayNamesBasisElements.size) {
      if (this->DisplayNamesBasisElements[i] == "") {
        theFlaStream << "1";
      } else {
        theFlaStream << this->DisplayNamesBasisElements[i];
      }
    } else {
      theFlaStream << " e_{" << i + 1 << "}";
    }
    theFlaStream << "=" << this->latestBasis[i].ToStringMatrixForm(theFormat);
    out << HtmlRoutines::GetMathSpanPure(theFlaStream.str());
    if (i != this->latestBasis.size - 1) {
      out << ",  ";
    }
  }
  if (this->flagIsQuadraticRadicalExtensionRationals) {
    out << "<br>Generating element not selected. ";
  } else {
    out << "<br>Generating element: "
    << HtmlRoutines::GetMathSpanPure(this->GeneratingElementMatForm.toString(theFormat));
  }
  out << "<br>There are " << this->basisInjections.size << " registered old bases. ";
  for (int i = 0; i < this->basisInjections.size; i ++) {
    out << "<hr>Basis " << i + 1 << " has " << this->basisInjections[i].size << " elements: ";
    for (int j = 0; j < this->basisInjections[i].size; j ++) {
      out << HtmlRoutines::GetMathSpanPure(this->basisInjections[i][j].toString(theFormat));
      if (j != this->basisInjections[i].size - 1) {
        out << ", ";
      }
    }
  }
  out << "<br>";
  return out.str();
}

AlgebraicNumber AlgebraicNumber::scaleNormalizeIndex(
  List<AlgebraicNumber>& output, int indexNonZeroElement
) {
  List<Rational> allCoefficients;
  int indexNonZeroRational = - 1;
  for (int i = 0; i < output.size; i ++) {
    List<Rational>& current = output[i].element.coefficients;
    if (i == indexNonZeroElement) {
      indexNonZeroRational = allCoefficients.size - 1;
      for (int j = 0; j < current.size; j ++) {
        indexNonZeroRational ++;
        if (!current[j].IsEqualToZero()) {
          break;
        }
      }
    }
    allCoefficients.AddListOnTop(output[i].element.coefficients);
  }
  Rational scale = Rational::scaleNormalizeIndex(allCoefficients, indexNonZeroRational);
  for (int i = 0; i < output.size; i ++){
    output[i] *= scale;
  }
  return scale;
}

AlgebraicNumber AlgebraicClosureRationals::One() {
  return this->fromRational(1);
}

AlgebraicNumber AlgebraicClosureRationals::fromRational(const Rational& input) {
  AlgebraicNumber result(input);
  result.owner = this;
  return result;
}

std::string AlgebraicClosureRationals::toString(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::toString");
  (void) theFormat;
  std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML = false;
  tempFormat.flagUseLatex = true;
  if (this->latestBasis.size == 1) {
    out << HtmlRoutines::GetMathSpanPure("\\mathbb Q");
    return out.str();
  }
  if (this->flagIsQuadraticRadicalExtensionRationals) {
    return this->ToStringQuadraticRadical(&tempFormat);
  }
  return this->ToStringFull(&tempFormat);
}

bool AlgebraicNumber::IsRational(Rational* whichRational) const {
  if (this->IsEqualToZero()) {
    if (whichRational != nullptr) {
      *whichRational = 0;
    }
    return true;
  }
  for (int i = 0; i < this->element.size(); i ++) {
    if (this->element[i].theIndex != 0) {
      return false;
    } else if (whichRational != nullptr) {
      *whichRational = this->element.coefficients[i];
    }
  }
  return true;
}

bool AlgebraicNumber::IsEqualToZero() const {
  return this->element.IsEqualToZero();
}

std::string AlgebraicNumber::toString(FormatExpressions* theFormat) const {
  if (this->owner == nullptr) {
    if (this->element.IsEqualToZero()) {
      return "0";
    }
    return this->element.coefficients[0].toString(theFormat);
  }
  std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.vectorSpaceEiBasisNames = this->owner->DisplayNamesBasisElements;
  if (theFormat != nullptr) {
    tempFormat.flagUseFrac = theFormat->flagUseFrac;
  }
  VectorSparse<Rational> theAdditiveVector;
  this->owner->GetAdditionTo(*this, theAdditiveVector);
  out << theAdditiveVector.toString(&tempFormat); //<< "~ in~ the~ field~ " << this->owner->toString();
  if (this->basisIndex < this->owner->basisInjections.size - 1 && global.UserDebugFlagOn()) {
    out << "[=" << this->ToStringNonInjected() << "]";
  }
  return out.str();
}

std::string AlgebraicNumber::ToStringNonInjected(FormatExpressions* theFormat) const {
  if (this->owner == nullptr) {
    if (this->element.IsEqualToZero()) {
      return "0";
    }
    return this->element.coefficients[0].toString(theFormat);
  }
  std::stringstream out;
  FormatExpressions tempFormat;
  int dimension = this->owner->basisInjections[this->basisIndex].size;
  tempFormat.vectorSpaceEiBasisNames.SetSize(dimension);

  for (int i = 0; i < dimension; i ++) {
    std::stringstream letter;
    letter << "e_{" << this->basisIndex + 1 << ", " << i+1 << "}";
    tempFormat.vectorSpaceEiBasisNames[i] = letter.str();
  }
  if (theFormat != nullptr) {
    tempFormat.flagUseFrac = theFormat->flagUseFrac;
  }
  out << this->element.toString(&tempFormat); //<< "~ in~ the~ field~ " << this->owner->toString();
  return out.str();
}


bool AlgebraicNumber::operator==(const AlgebraicNumber& other) const {
  Rational ratValue;
  this->CheckConsistency();
  other.CheckConsistency();
  if (this->IsRational(&ratValue)) {
    return other == ratValue;
  }
  if (other.IsRational(&ratValue)) {
    return *this == ratValue;
  }
  if (this->owner != other.owner) {
    global.fatal << "This might or might not be a programming error: "
    << "comparing two algebraic number that do not have the same owner. "
    << "The numbers have owners of respective addresses "
    << this->owner << " and " << other.owner << ". The numbers are: "
    << this->toString() << " and " << other.toString() << ". Crashing to let you know. ";
    global.fatal << global.fatal;
  }
  this->CheckNonZeroOwner();
  if (this->basisIndex == other.basisIndex) {
    return this->element == other.element;
  }
  VectorSparse<Rational> leftAdditive, rightAdditive;
  this->owner->GetAdditionTo(*this, leftAdditive);
  this->owner->GetAdditionTo(other, rightAdditive);
  return leftAdditive == rightAdditive;
}

void AlgebraicNumber::operator=(const Polynomial<AlgebraicNumber>& other) {
  if (!other.IsConstant(this)) {
    global.fatal << "This is a programming error: attempting to assign non-constant "
    << "polynomial to a Rational number is not allowed. " << global.fatal;
  }
}

void AlgebraicNumber::operator=(const Rational& other) {
  this->owner = nullptr;
  this->element.MaKeEi(0, other);
  this->basisIndex = 0;
}

bool ElementZmodP::NeedsParenthesisForMultiplication(FormatExpressions* theFormat) const {
  if (theFormat == nullptr) {
    return true;
  }
  return !theFormat->flagSuppressModP;
}

std::string ElementZmodP::toStringModP() const {
  return ElementZmodP::toStringModP(this->theModulus);
}

std::string ElementZmodP::toStringModP(const LargeIntegerUnsigned& modulus) {
  std::stringstream out;
  out << "mod(" << modulus.toString() << ")";
  return out.str();
}

std::string ElementZmodP::toString(FormatExpressions* theFormat) const {
  bool suppressModulus = false;
  if (theFormat != nullptr) {
    suppressModulus = theFormat->flagSuppressModP;
  }
  std::stringstream out;
  if (suppressModulus) {
    out << this->theValue.toString();
  } else {
    out << "(" << this->theValue.toString() << " "
    << this->toStringModP() << ")";
  }
  return out.str();
}

bool ElementZmodP::operator*=(const Rational& other) {
  ElementZmodP otherElement;
  otherElement.theModulus = this->theModulus;
  if (!otherElement.AssignRational(other)) {
    return false;
  }
  *this *= otherElement;
  return true;
}

ElementZmodP ElementZmodP::operator*(const Rational& other) const {
  ElementZmodP otherElt;
  otherElt.theModulus = this->theModulus;
  otherElt.AssignRational(other);
  ElementZmodP result = *this;
  result *= otherElt;
  return result;
}

unsigned int ElementZmodP::HashFunction() const {
  if (this->theValue.IsEqualToZero()) {
    return 0;
  }
  return this->theValue.HashFunction() * SomeRandomPrimes[0] +
  this->theModulus.HashFunction() * SomeRandomPrimes[1];
}

void ElementZmodP::CheckIamInitialized() const {
  if (this->theModulus.IsEqualToZero()) {
    global.fatal << "Programming error: computing with non-initialized "
    << "element the ring Z mod p (the number p has not been initialized!)."
    << global.fatal;
  }
  if (this->flagDeallocated) {
    global.fatal
    << "Programming error: use after free of element z mod p. " << global.fatal;
  }
}

void ElementZmodP::operator=(const LargeInteger& other) {
  this->CheckIamInitialized();
  this->theValue = other.value;
  this->theValue %= this->theModulus;
  if (other.sign == - 1) {
    ElementZmodP mOne;
    mOne.MakeMOne(this->theModulus);
    *this *= mOne;
  }
}

bool ElementZmodP::operator>(const ElementZmodP& other) const {
  if (this->theModulus > other.theModulus) {
    return true;
  }
  if (other.theModulus > this->theModulus) {
    return false;
  }
  return this->theValue > other.theValue;
}

ElementZmodP ElementZmodP::zero() {
  ElementZmodP result;
  result.theModulus = 1;
  return result;
}

void ElementZmodP::ScaleToIntegralMinimalHeightAndGetPoly(
  const Polynomial<Rational>& input,
  Polynomial<ElementZmodP>& output,
  const LargeIntegerUnsigned& newModulo
) {
  Polynomial<Rational> rescaled;
  rescaled = input;
  rescaled.scaleNormalizeLeadingMonomial();
  output.SetExpectedSize(input.size());
  ElementZmodP theCF;
  theCF.theModulus = newModulo;
  output.MakeZero();
  for (int i = 0; i < input.size(); i ++) {
    theCF = input.coefficients[i];
    output.AddMonomial(input[i], theCF);
  }
}

void ElementZmodP::operator=(const ElementZmodP& other) {
  this->theModulus = other.theModulus;
  this->theValue = other.theValue;
}

void ElementZmodP::operator=(const LargeIntegerUnsigned& other) {
  this->CheckIamInitialized();
  this->theValue = other;
  this->theValue %= this->theModulus;
}

void ElementZmodP::MakeOne(const LargeIntegerUnsigned& newModulo) {
  this->theModulus = newModulo;
  this->theValue = 1;
}

void ElementZmodP::MakeMOne(const LargeIntegerUnsigned& newModulo) {
  this->theModulus = newModulo;
  this->theValue = newModulo;
  this->theValue --;
}

void ElementZmodP::CheckEqualModuli(const ElementZmodP& other) {
  if (this->theModulus != other.theModulus) {
    global.fatal << "This is a programming error: attempting to make an operation "
    << "with two elemetns of Z mod P with different moduli, "
    << this->theModulus.toString() << " and "
    << other.theModulus.toString() << ". " << global.fatal;
  }
}

ElementZmodP ElementZmodP::operator*(const ElementZmodP& other) const {
  ElementZmodP result = *this;
  result *= other;
  return result;
}

ElementZmodP ElementZmodP::scaleNormalizeIndex(
  List<ElementZmodP>& toBeScaled, int indexNonZeroElement
) {
  ElementZmodP scale = toBeScaled[indexNonZeroElement];
  ElementZmodP one;
  one.MakeOne(scale.theModulus);
  one /= scale;
  for (int i = 0; i < toBeScaled.size; i ++) {
    toBeScaled[i] *= scale;
  }
  return scale;
}

ElementZmodP ElementZmodP::operator+(const Rational& other) const {
  ElementZmodP result = *this;
  result += other;
  return result;
}

ElementZmodP ElementZmodP::operator-(const ElementZmodP& other) const {
  ElementZmodP result = *this;
  result -= other;
  return result;
}

ElementZmodP ElementZmodP::operator/(const ElementZmodP& other) const {
  ElementZmodP result = *this;
  result /= other;
  return result;
}

ElementZmodP operator*(int left, const ElementZmodP& right) {
  ElementZmodP result;
  result = right;
  result *= left;
  return result;
}

void ElementZmodP::operator*=(int other) {
  *this *= LargeInteger(other);
}

void ElementZmodP::operator*=(const LargeInteger& other) {
  this->theValue *= other.value;
   if (other.IsNegative()) {
     this->theValue *= this->theModulus - 1;
   }
   this->theValue %= this->theModulus;
 }

bool ElementZmodP::operator+=(const Rational& other) {
  ElementZmodP otherElt;
  otherElt.theModulus = this->theModulus;
  if (!otherElt.AssignRational(other)) {
    return false;
  }
  *this += otherElt;
  return true;
}

void ElementZmodP::operator+=(const ElementZmodP& other) {
  if (this == &other) {
    ElementZmodP other = *this;
    *this += other;
    return;
  }
  this->CheckEqualModuli(other);
  this->theValue += other.theValue;
  this->theValue %= this->theModulus;
}

void ElementZmodP::operator-=(const ElementZmodP& other) {
  if (this == &other) {
    ElementZmodP other = *this;
    *this += other;
    return;
  }
  this->CheckEqualModuli(other);
  ElementZmodP otherTimesMinusOne = other;
  otherTimesMinusOne *= -1;
  *this += otherTimesMinusOne;
}

void ElementZmodP::operator=(const Rational& other) {
  bool tempB = this->AssignRational(other);
  if (!tempB) {
    global.fatal << "This is a programming error: "
    << "using ElementZmodP::operator= to assign a Rational number failed. "
    << "Operator = does not allow failure. " << global.fatal;
  }
}

void ElementZmodP::operator=(const int other) {
  LargeInteger otherLI = other;
  *this = otherLI;
}

void ElementZmodP::operator-=(const LargeIntegerUnsigned& other) {
  ElementZmodP otherElt;
  otherElt.theModulus = this->theModulus;
  otherElt = other;
  (*this) -= otherElt;
}

bool ElementZmodP::AssignRational(const Rational& other) {
  this->CheckIamInitialized();
  *this = other.GetNumerator();
  ElementZmodP denominator;
  denominator.theModulus = this->theModulus;
  denominator.theValue = other.GetDenominator();
  if (denominator.IsEqualToZero()) {
    return false;
  }
  return (*this /= denominator);
}

bool ElementZmodP::operator/=(const LargeInteger& other) {
  ElementZmodP divisor;
  divisor.theModulus = this->theModulus;
  if (!divisor.AssignRational(Rational(other))) {
    return false;
  }
  bool result = ((*this) /= divisor);
  return result;
}

bool ElementZmodP::operator/=(const ElementZmodP& other) {
  this->CheckIamInitialized();
  this->CheckEqualModuli(other);
  if (other.IsEqualToZero()) {
    global.fatal << "Division by zero in ElementZmodP." << global.fatal;
  }
  LargeInteger theInverted, otherValue, theMod;
  theMod = this->theModulus;
  otherValue = other.theValue;
  if (!MathRoutines::InvertXModN(otherValue, theMod, theInverted)) {
    return false;
  }
  *this *= theInverted;
  return true;
}
