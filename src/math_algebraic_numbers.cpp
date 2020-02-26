// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_extra_algebraic_numbers.h"
#include "math_subsets_selections.h"
#include "math_general_implementation.h"

std::string MonomialVector::ToString(FormatExpressions* theFormat) const {
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
      out << "\\sqrt{" << theRad.ToString() << "}";
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
  int DimOverRationals = this->latestBasis.size;
  theSel.init(DimOverRationals);
  this->theGeneratingElementPowersBasis.SetSize(0);
  Vector<Rational> currentVect;
  this->GeneratingElemenT.owner = this;
  this->GeneratingElemenT.basisIndex = this->basisInjections.size - 1;
  int attemptsSoFar = 0;
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
    this->GeneratingElemenT.theElT.MakeZero();
    for (int i = 0; i < theSel.theInts.size; i ++) {
      MonomialVector tempV;
      tempV.MakeEi(i);
      this->GeneratingElemenT.theElT.AddMonomial(tempV, theSel.theInts[i]);
    }
    this->GetMultiplicationBy(this->GeneratingElemenT, this->GeneratingElementTensorForm);
    this->GeneratingElementTensorForm.GetMatrix(this->GeneratingElementMatForm, DimOverRationals);
    this->theGeneratingElementPowersBasis.SetSize(0);
    currentVect.MakeEi(DimOverRationals, 0);
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
    if (this->theGeneratingElementPowersBasis.size == DimOverRationals) {
      break;
    }
  }
  return true;
}

bool AlgebraicClosureRationals::ReduceMe(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::ReduceMe");
  if (!this->ChooseGeneratingElement(1000, commentsOnFailure)) {
    return false;
  }
  if (this->flagIsQuadraticRadicalExtensionRationals) {
    return true;
  }
  global.Comments
  << "DEBUG: and the chosen generator is: z = "
  << this->GeneratingElemenT.ToString() << "<br>";
  Polynomial<Rational> theMinPoly, smallestFactor;
  theMinPoly.AssignMinPoly(this->GeneratingElementMatForm);
  int theDim = this->latestBasis.size;
  List<Polynomial<Rational> > theFactors;
  bool mustBeTrue = theMinPoly.FactorMe(theFactors, nullptr);
  if (!mustBeTrue) {
    global.fatal
    << "This is a programming error: failed to factor polynomial "
    << theMinPoly.ToString() << global.fatal;
  }
  smallestFactor = theFactors[0];
  if (smallestFactor.TotalDegreeInt() == theDim) {
    return true;
  }
  MatrixTensor<Rational> generatorPowers, generatorPowersInverse;
  generatorPowers.AssignVectorsToColumns(this->theGeneratingElementPowersBasis);
  generatorPowersInverse = generatorPowers;
  generatorPowersInverse.Invert();
  global.Comments << "DEBUG: field at reduction start: " << this->ToStringFull();
  global.Comments << "DEBUG: generating element powers: " << this->theGeneratingElementPowersBasis.ToString() << "<br>";
  global.Comments << "DEBUG: basis change matrix: " << generatorPowers.ToStringMatrixForm();
  global.Comments << "DEBUG: inverse basis change matrix: " << generatorPowersInverse.ToStringMatrixForm();


  Polynomial<Rational> zToTheNth, remainderAfterReduction, tempP;
  MatrixTensor<Rational> projectionGeneratorCoordinates;
  int smallestFactorDegree = - 1;
  if (!smallestFactor.TotalDegree().IsSmallInteger(&smallestFactorDegree)) {
    global.fatal
    << "This is a programming error: "
    << smallestFactor.ToString()
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
    zToTheNth.DivideBy(smallestFactor, tempP, remainderAfterReduction);
    for (int j = 0; j < remainderAfterReduction.size(); j ++) {
      int theIndex = - 1;
      remainderAfterReduction[j](0).IsSmallInteger(&theIndex);
      projectionGeneratorCoordinates.AddMonomial(
        MonomialMatrix(theIndex, i),
        remainderAfterReduction.coefficients[j]
      );
    }
  }
  global.Comments << "DEBUG: The projection in generator coordinates: "
  << projectionGeneratorCoordinates.ToStringMatrixForm() << "<br>"
  << "Generator min poly factors: "
  << theFactors.ToStringCommaDelimited() << "<br>";  
  List<MatrixTensor<Rational> > newBasis;
  MatrixTensor<Rational> generatorProjected;
  Rational leadingCoefficient = smallestFactor.GetLeadingCoefficient();
  global.Comments << "DEBUG: smallest factor: " << smallestFactor.ToString() << ". <br>";
  for (int i = 0; i < smallestFactorDegree - 1; i ++) {
    MonomialMatrix termBelowMainDiagonal, termInLastColumn;
    termBelowMainDiagonal.MakeEij(i + 1, i);
    generatorProjected.AddMonomial(termBelowMainDiagonal, 1);
    termInLastColumn.MakeEij(i, smallestFactorDegree - 1);
    Rational coefficientLastColumn = smallestFactor.GetMonomialCoefficient(MonomialP(0, i));
    coefficientLastColumn /= leadingCoefficient;
    global.Comments << "DEBUG: coefficient " << i << ": " << coefficientLastColumn
    << ", monomial: " << termInLastColumn << ", monomial is zero: " << termInLastColumn.IsZeroMonomial() << ". <br>";
    generatorProjected.AddMonomial(termInLastColumn, coefficientLastColumn);
    if (coefficientLastColumn != 0 && generatorProjected.IsEqualToZero()) {
      global.fatal << " zero generator shouldnt happen. "<< global.fatal;
    }
    global.Comments << "generator projected: " << generatorProjected.ToString() << ". <br>";
  }
  global.Comments << "DEBUG: and the generator is: " << generatorProjected.ToStringMatrixForm() << ". <br>";
  newBasis.SetSize(smallestFactorDegree);
  newBasis[0].MakeId(smallestFactorDegree);
  newBasis[1] = generatorProjected;
  for (int i = 2; i < smallestFactorDegree; i ++) {
    newBasis[i] = generatorProjected;
    newBasis[i].RaiseToPower(i);
  }
  this->latestBasis = newBasis;
  this->AssignDefaultBasisDisplayNames();
  MatrixTensor<Rational> injection;
  injection = projectionGeneratorCoordinates;
  injection *= generatorPowersInverse;
  global.Comments << "DEBUG: and the injection is: " << injection.ToStringMatrixForm();
  this->basisInjections.SetSize(this->basisInjections.size - 1);
  this->InjectOldBases(&injection);
  this->AppendAdditiveEiBasis();

  global.Comments << "DEBUG: got to here! ";
  global.Comments << "<hr> So far the field is: " << this->ToStringFull();

  if (!this->ChooseGeneratingElement(1000, commentsOnFailure)) {
    return false;
  }
  this->GetMultiplicationBy(
    this->GeneratingElemenT, this->GeneratingElementTensorForm
  );
  this->GeneratingElementTensorForm.GetMatrix(
    this->GeneratingElementMatForm, this->latestBasis.size
  );
  global.Comments << this->ToString();
  return true;
}

void AlgebraicClosureRationals::GetAdditionTo(
  const AlgebraicNumber& input, VectorSparse<Rational>& output
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::GetAdditionTo");
  if (&output == &input.theElT) {
    AlgebraicNumber anCopy = input;
    this->GetAdditionTo(anCopy, output);
    return;
  }
  if (input.owner == nullptr) {
    if (input.theElT.size() > 0) {
      output.MaKeEi(0, input.theElT.coefficients[0]);
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
  if (input.basisIndex == this->latestBasis.size - 1) {
    output = input.theElT;
    return;
  }
  output.MakeZero();
  for (int i = 0; i < input.theElT.size(); i ++) {
    int currentIndex = input.theElT[i].theIndex;
    if (
      currentIndex < 0 ||
      currentIndex >= this->basisInjections[input.basisIndex].size
    ) {
      global.fatal
      << "This is a programming error: I am getting basis index "
      << input.basisIndex
      << " with current index " << currentIndex
      << ". A printout of the algebraic closure follows. "
      << this->ToString() << global.fatal;
    }
    output.AddOtherTimesConst(
      this->basisInjections[input.basisIndex][currentIndex],
      input.theElT.coefficients[i]
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
      global.fatal << "This is a programming error: element " << input.ToString()
      << " has bad index, namely, " << inputAdditiveForm[i].theIndex
      << ". The algebraic closure is: " << this->ToString() << ". " << global.fatal;
    }
    currentMat = this->latestBasis[inputAdditiveForm[i].theIndex];
    currentMat *= inputAdditiveForm.coefficients[i];
    output += currentMat;
  }
}

Rational AlgebraicNumber::GetDenominatorRationalPart() const {
  if (this->owner == nullptr) {
    if (this->theElT.IsEqualToZero()) {
      return 1;
    }
    return this->theElT.coefficients[0].GetDenominator();
  }
  VectorSparse<Rational> theEltAdditive;
  this->owner->GetAdditionTo(*this, theEltAdditive);
  LargeIntegerUnsigned resultLCM = 1;
  for (int i = 0; i < theEltAdditive.size(); i ++) {
    resultLCM = LargeIntegerUnsigned::lcm(resultLCM, theEltAdditive.coefficients[i].GetDenominator());
  }
  return resultLCM;
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

Rational AlgebraicNumber::GetNumeratorRationalPart() const {
  if (this->owner == nullptr) {
    if (this->theElT.IsEqualToZero()) {
      return 0;
    }
    return this->theElT.coefficients[0].GetNumerator();
  }
  VectorSparse<Rational> theEltAdditive;
  this->owner->GetAdditionTo(*this, theEltAdditive);
  LargeInteger resultGCD = 1;
  LargeIntegerUnsigned tempR;
  if (theEltAdditive.size() > 0) {
    resultGCD = theEltAdditive.coefficients[0].GetNumerator();
  }
  for (int i = 1; i < theEltAdditive.size(); i ++) {
    tempR = resultGCD.value;
    LargeIntegerUnsigned::gcd(tempR, theEltAdditive.coefficients[i].GetNumerator().value, resultGCD.value);
  }
  return resultGCD;
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
  if (this->theElT.IsEqualToZero()) {
    return other == 0;
  }
  if (this->theElT.size() != 1) {
    return false;
  }
  if (this->theElT[0].theIndex != 0) {
    return false;
  }
  return this->theElT.coefficients[0] == other;
}

bool AlgebraicNumber::NeedsParenthesisForMultiplication() const {
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
  return inputOwner.AdjoinRootMinPoly(thePoly, *this, commentsOnFailure);
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
  this->GeneratingElemenT.theElT.MaKeEi(0);
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
  minPoly /= minPoly.GetMonomialCoefficient(minPoly.GetMaxMonomial());
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
    global.fatal << "This is a programming error. The number z=" << outputRoot.ToString()
    << " was just adjoined to the base field; z "
    << "was given by requesting that it has minimial polynomial " << algNumPoly.ToString()
    << ", however, substituting z back in to the minimal polynomial "
    << "does not yield zero, rather yields " << algNumPoly.ToString() << ". " << global.fatal;
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
    << "The input poly is: " << input.ToString() << global.fatal;
  }
  PolynomialSubstitution<AlgebraicNumber> theSub;
  theSub.MakeIdSubstitution(indexVar + 1);
  theSub[indexVar].MakeMonomiaL(0, 1, 1);
  output = input;
  output.Substitution(theSub);
}

bool AlgebraicClosureRationals::AdjoinRootMinPoly(
  const Polynomial<AlgebraicNumber>& thePoly,
  AlgebraicNumber& outputRoot,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::AdjoinRootMinPoly");
  if (this->AdjoinRootQuadraticPolyToQuadraticRadicalExtension(thePoly, outputRoot, commentsOnFailure)) {
    return true;
  }
  Polynomial<AlgebraicNumber> minPoly;
  this->ConvertPolyDependingOneVariableToPolyDependingOnFirstVariableNoFail(thePoly, minPoly);
  int indexMaxMonMinPoly = minPoly.GetIndexMaxMonomial();
  AlgebraicNumber leadingCF = minPoly.coefficients[indexMaxMonMinPoly];
  minPoly /= leadingCF;
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
  global.Comments << "DEBUG: Adjoin roots of: " << minPoly.ToString() << "<hr>";
  theGenMat.MakeZero();
  for (int i = 0; i < degreeMinPoly - 1; i ++) {
    for (int j = 0; j < startingDimension; j ++) {
      theGenMat.AddMonomial(MonomialMatrix((i + 1) * startingDimension + j, i * startingDimension + j), 1);
    }
  }
  Polynomial<AlgebraicNumber> minusMinPolyMinusMaxMon = minPoly;
  int indexMaxMon = minusMinPolyMinusMaxMon.GetIndexMaxMonomial(
    MonomialP::LeftIsGEQTotalDegThenLexicographicLastVariableStrongest
  );
  const MonomialP maxMon = minusMinPolyMinusMaxMon[indexMaxMon];
  AlgebraicNumber maxMonCoeff = minusMinPolyMinusMaxMon.coefficients[indexMaxMon];
  minusMinPolyMinusMaxMon.SubtractMonomial(maxMon, maxMonCoeff);
  minusMinPolyMinusMaxMon /= maxMonCoeff;
  minusMinPolyMinusMaxMon /= - 1;
  MatrixTensor<Rational> currentCoeffMatForm;
  for (int i = 0; i < minusMinPolyMinusMaxMon.size(); i ++) {
    AlgebraicNumber& currentCoeff = minusMinPolyMinusMaxMon.coefficients[i];
    const MonomialP& currentMon = minusMinPolyMinusMaxMon[i];
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
    // global.Comments << "DEBUG: final basis " << i + 1 << ": " << finalBasis[i].ToStringMatrixForm() << "<br>";
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
  outputRoot.theElT.MaKeEi(startingDimension);
  outputRoot.basisIndex = this->basisInjections.size - 1;
  this->flagIsQuadraticRadicalExtensionRationals = false;
  if (!this->ReduceMe(commentsOnFailure)) {
    *this = backUpCopy;
    return false;
  }
  // Sanity check code here:
  PolynomialSubstitution<AlgebraicNumber> theSub;
  theSub.SetSize(1);
  theSub[0].MakeConst(outputRoot);
  Polynomial<AlgebraicNumber> substitutedMinPoly;
  substitutedMinPoly = minPoly;
  substitutedMinPoly.Substitution(theSub);
  if (!substitutedMinPoly.IsEqualToZero()) {
    global.fatal << "This is a programming error. The number z = "
    << outputRoot.ToString() << " was just adjoined to the base field; z "
    << "was given by requesting that it has minimial polynomial "
    << minPoly.ToString()
    << ", however, substituting z back in to the minimal polynomial "
    << "does not yield zero, rather yields "
    << substitutedMinPoly.ToString() << ". "
    << "The algebraic closure printout follows. "
    << this->ToStringFull() << global.fatal;
  }
  return true;
}

void AlgebraicNumber::Invert() {
  MacroRegisterFunctionWithName("AlgebraicNumber::Invert");
  if (this->owner == nullptr) {
    if (this->theElT.IsEqualToZero()) {
      global.fatal << "This is a programming error: division by zero. " << global.fatal;
    }
    bool isGood = (this->theElT.size() == 1);
    if (isGood) {
      isGood = (this->theElT[0].theIndex == 0);
    }
    if (!isGood) {
      global.fatal << "This is a programming error: Algebraic number has no owner, "
      << "so it must be rational, but it appears to be not. "
      << "Its theElt vector is: " << this->theElT.ToString() << global.fatal;
    }
    this->theElT.coefficients[0].Invert();
    return;
  }
  MatrixTensor<Rational> theInverted;
  Matrix<Rational> tempMat2;
  this->owner->GetMultiplicationBy(*this, theInverted);
  theInverted.GetMatrix(tempMat2, this->owner->latestBasis.size);
  tempMat2.Invert();
  theInverted = tempMat2;
  this->theElT.MaKeEi(0);
  theInverted.ActOnVectorColumn(this->theElT);
  this->basisIndex = this->owner->basisInjections.size - 1;
}

void AlgebraicNumber::operator/=(const AlgebraicNumber& other) {
  MacroRegisterFunctionWithName("AlgebraicNumber::operator/=");
  AlgebraicNumber otherCopy = other;
  otherCopy.Invert();
  *this *= otherCopy;
}

void AlgebraicNumber::operator*=(const Rational& other) {
  this->theElT *= other;
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
    this->theElT -= other.theElT;
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
  theOwner->GetAdditionTo(*this, this->theElT);
  theOwner->GetAdditionTo(other, AdditiveFormOther);
  this->owner = theOwner;
  this->basisIndex = theOwner->basisInjections.size - 1;
  this->theElT -= AdditiveFormOther;
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
    << this->ToString() << " and " << other.ToString()
    << " have with zero owners but different basis indices. " << global.fatal;
  }
  if (this->basisIndex == other.basisIndex) {
    this->owner = theOwner;
    this->CheckConsistency();
    other.CheckConsistency();
    this->theElT += other.theElT;
    this->CheckConsistency();
    return;
  }
  VectorSparse<Rational> AdditiveFormOther;
  theOwner->GetAdditionTo(*this, this->theElT);
  theOwner->GetAdditionTo(other, AdditiveFormOther);
  this->owner = theOwner;
  this->basisIndex = theOwner->basisInjections.size - 1;
  this->theElT += AdditiveFormOther;
  this->CheckConsistency();
}

bool AlgebraicNumber::CheckConsistency() const {
  if (this->flagDeallocated) {
    global.fatal << "This is a programming error: use after free of AlgebraicNumber. " << global.fatal;
  }
  if (this->owner == nullptr) {
    if (!this->IsRational()) {
      for (int i = 0; i < this->theElT.size(); i ++) {
        global.Comments << "<br>index: " << this->theElT[i].theIndex << ", coefficient: "
        << this->theElT.coefficients[i];
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
      this->theElT.MakeZero();
      return;
    }
    this->theElT *= other.theElT.coefficients[0];
    return;
  }
  if (this->owner == nullptr) {
    if (this->theElT.IsEqualToZero()) {
      return;
    }
    Rational tempRat = this->theElT.GetMonomialCoefficient(MonomialVector(0));
    *this = other;
    *this *= tempRat;
    return;
  }
  bool doReport = (this->theElT.size() * other.theElT.size() > 100);
  ProgressReport theReport;
  if (doReport) {
    std::stringstream reportStream;
    reportStream << "Large multiplication:<br>"
    << this->ToString() << "\n<br>by<br>\n" << other.ToString()
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
  this->theElT.MaKeEi(0);
  leftMat.ActOnVectorColumn(this->theElT);
}

void AlgebraicNumber::SqrtMeDefault() {
  this->RadicalMeDefault(2);
}

bool AlgebraicNumber::operator>(const AlgebraicNumber& other) const {
  Rational left, right;
  if (this->IsRational(&left) && other.IsRational(&right)) {
    return left > right;
  }
  this->CheckCommonOwner(other);
  return this->theElT > other.theElT;
}

void AlgebraicNumber::AssignRational(const Rational& input, AlgebraicClosureRationals& inputOwner) {
  this->basisIndex = 0;
  this->owner = &inputOwner;
  this->theElT.MaKeEi(0, input);
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
  this->owner->GetAdditionTo(*this, this->theElT);
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
  for (int i = 0; i < this->theElT.size(); i ++) {
    this->owner->GetRadicalSelectionFromIndex(this->theElT[i].theIndex, currentRadicalSelection);
    if (outputWhichDouble != nullptr) {
      currentMultiplicand = this->theElT.coefficients[i].GetDoubleValue();
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
  this->theElT.MaKeEi(this->owner->GetIndexFromRadicalSelection(FactorSel));
  this->theElT *= squareRootRationalPart;
  this->CheckConsistency();
  return true;
}

void AlgebraicNumber::RadicalMeDefault(int theRad) {
  (void) theRad;
  global.fatal << "Not implemented yet!" << global.fatal;
/*  MatrixTensor<Rational> theRadicalOp;
  theRadicalOp.MakeZero();
  MonomialTensor tempM;
  for (int i = 0; i < this->GetMinPoly().size; i ++) {
    tempM= this->GetMinPoly()[i];
    tempM.ExponentMeBy(theRad);
    newMinPoly.AddMonomial(tempM, this->GetMinPoly().theCoeffs[i]);
  }
  this->rootIndex = 0;
  this->minPolyIndex = this->theRegistry->theMinPolys.AddNoRepetitionOrReturnIndexFirst(newMinPoly);*/
}

std::string AlgebraicClosureRationals::ToStringQuadraticRadical(FormatExpressions *theFormat) const {
  (void) theFormat;
  std::stringstream out;
  out << "\\mathbb Q[";
  for (int i = 0; i < this->theQuadraticRadicals.size; i ++) {
    out << "\\sqrt{" << this->theQuadraticRadicals[i].ToString() << "}";
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
    << HtmlRoutines::GetMathSpanPure(this->GeneratingElementMatForm.ToString(theFormat));
  }
  out << "<br>There are " << this->basisInjections.size << " registered old bases. ";
  for (int i = 0; i < this->basisInjections.size; i ++) {
    out << "<hr>Basis " << i + 1 << " has " << this->basisInjections[i].size << " elements: ";
    for (int j = 0; j < this->basisInjections[i].size; j ++) {
      out << HtmlRoutines::GetMathSpanPure(this->basisInjections[i][j].ToString(theFormat));
      if (j != this->basisInjections[i].size - 1) {
        out << ", ";
      }
    }
  }
  out << "<br>";
  return out.str();
}

std::string AlgebraicClosureRationals::ToString(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::ToStringQuadraticRadical");
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
  for (int i = 0; i < this->theElT.size(); i ++) {
    if (this->theElT[i].theIndex != 0) {
      return false;
    } else if (whichRational != nullptr) {
      *whichRational = this->theElT.coefficients[i];
    }
  }
  return true;
}

bool AlgebraicNumber::IsEqualToZero() const {
  return this->theElT.IsEqualToZero();
}

std::string AlgebraicNumber::ToString(FormatExpressions* theFormat) const {
  if (this->owner == nullptr) {
    if (this->theElT.IsEqualToZero()) {
      return "0";
    }
    return this->theElT.coefficients[0].ToString(theFormat);
  }
  std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.vectorSpaceEiBasisNames = this->owner->DisplayNamesBasisElements;
  if (theFormat != nullptr) {
    tempFormat.flagUseFrac = theFormat->flagUseFrac;
  }
  VectorSparse<Rational> theAdditiveVector;
  this->owner->GetAdditionTo(*this, theAdditiveVector);
  out << theAdditiveVector.ToString(&tempFormat); //<< "~ in~ the~ field~ " << this->owner->ToString();
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
    << this->ToString() << " and " << other.ToString() << ". Crashing to let you know. ";
    global.fatal << global.fatal;
  }
  this->CheckNonZeroOwner();
  if (this->basisIndex == other.basisIndex) {
    return this->theElT == other.theElT;
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
  this->theElT.MaKeEi(0, other);
  this->basisIndex = 0;
}

std::string ElementZmodP::ToString(FormatExpressions* theFormat) const {
  (void) theFormat; //remove unused parameter warnign portable.
  std::stringstream out;
  out << "(" << this->theValue.ToString() << " ~mod~ " << this->theModulus.ToString() << ")";
  return out.str();
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
  return this->theValue.HashFunction() * SomeRandomPrimes[0] + this->theModulus.HashFunction() * SomeRandomPrimes[1];
}

void ElementZmodP::CheckIamInitialized() const {
  if (this->theModulus.IsEqualToZero()) {
    global.fatal << "This is a programming error: computing with non-initialized "
    << "element the ring Z mod p (the number p has not been initialized!)."
    << global.fatal;
  }
  if (this->flagDeallocated) {
    global.fatal << "This is a programming error: use after free of element z mod p. " << global.fatal;
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

void ElementZmodP::ScaleToIntegralMinHeightAndGetPoly(
  const Polynomial<Rational>& input, Polynomial<ElementZmodP>& output, const LargeIntegerUnsigned& newModulo
) {
  Polynomial<Rational> rescaled;
  rescaled = input;
  rescaled.ScaleToIntegralMinHeightFirstCoeffPosReturnsWhatIWasMultipliedBy();
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
    << this->theModulus.ToString() << " and " << other.theModulus.ToString() << ". " << global.fatal;
  }
}

ElementZmodP ElementZmodP::operator*(const ElementZmodP& other) const {
  ElementZmodP result = *this;
  result *= other;
  return result;
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

void ElementZmodP::operator*=(const LargeInteger& other) {
  this->theValue *= other.value;
  if (other.IsNegative()) {
    this->theValue *= this->theModulus - 1;
    this->theValue %= this->theModulus;
  }
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
    global.fatal << "This is a programming error: using ElementZmodP::operator= to assign a Rational number failed. "
    << " Operator = does not allow failure. " << global.fatal;
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
  ElementZmodP den;
  den.theModulus = this->theModulus;
  den = other.GetDenominator();
  if (den.IsEqualToZero()) {
    return false;
  }
  return (*this /= den);
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
  LargeInteger theInverted, otherValue, theMod;
  theMod = this->theModulus;
  otherValue = other.theValue;
  if (!MathRoutines::InvertXModN(otherValue, theMod, theInverted)) {
    return false;
  }
  *this *= theInverted;
  return true;
}
