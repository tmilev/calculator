// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_extra_algebraic_numbers.h"
#include "math_subsets_selections.h"
#include "math_general_implementation.h"

static ProjectInformationInstance projectInfoAlgebraicNumbersCPP(__FILE__, "Algebraic numbers. ");

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
  return this->theBasisMultiplicative.size;
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
  this->DisplayNamesBasisElements.SetSize(this->theBasisMultiplicative.size);
  Selection theSel;
  theSel.init(this->theQuadraticRadicals.size);
  do {
    std::stringstream out;
    //for (int i = 0; i < theSel.CardinalitySelection; i ++)
    //{ const LargeInt& theRad = this->theQuadraticRadicals[theSel.elements[i]];
    //  out << "\\sqrt{" << theRad.ToString() << "}";
    //}
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
    crash
    << "This is a programming error: the algebraic extension "
    << "is too large to be handled by the current data structures. "
    << crash;
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
    crash << "This is a programming error: AlgebraicClosureRationals::MergeTwoQuadraticRadicalExtensions "
    << "with at least one of two arguments that is not a quadratic radical extension of the rationals. "
    << crash;
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
    << Crasher::GetStackTraceEtcErrorMessageHTML();
    return (false);
  }
  this->theBasisMultiplicative.SetSize(MathRoutines::TwoToTheNth(radicalsNew.size));
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
      this->theBasisMultiplicative[this->GetIndexFromRadicalSelection(largerFieldSel)]
    );
  } while (largerFieldSel.IncrementReturnFalseIfPastLast());
  this->RegisterNewBasis(currentInjection);
  this->ComputeDisplayStringsFromRadicals();
  return true;
}

void AlgebraicClosureRationals::RegisterNewBasis(const MatrixTensor<Rational>& theInjection) {
  //VectorSparse<Rational> eltVectorForm;
  for (int j = 0; j < this->theBasesAdditive.size; j ++) {
    for (int i = 0; i < this->theBasesAdditive[j].size; i ++) {
      theInjection.ActOnVectorColumn(this->theBasesAdditive[j][i]);
    }
  }
  this->theBasesAdditive.SetSize(this->theBasesAdditive.size + 1);
  this->theBasesAdditive.LastObject()->SetSize(this->theBasisMultiplicative.size);
  for (int i = 0; i < this->theBasisMultiplicative.size; i ++) {
    (*this->theBasesAdditive.LastObject())[i].MaKeEi(i);
  }
}

void AlgebraicClosureRationals::ChooseGeneratingElement() {
  MacroRegisterFunctionWithName("AlgebraicExtensionRationals::ChooseGeneratingElement");
  SelectionPositiveIntegers theSel;
  int DimOverRationals = this->theBasisMultiplicative.size;
  theSel.init(DimOverRationals);
  this->theGeneratingElementPowersBasis.SetSize(0);
  Vector<Rational> currentVect;
  this->GeneratingElemenT.owner = this;
  this->GeneratingElemenT.basisIndex = this->theBasesAdditive.size - 1;
  for (theSel.IncrementReturnFalseIfPastLast(); ; theSel.IncrementReturnFalseIfPastLast()) {
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
      if (this->theGeneratingElementPowersBasis.size >this->theGeneratingElementPowersBasis.GetRankOfSpanOfElements()) {
        this->theGeneratingElementPowersBasis.SetSize(this->theGeneratingElementPowersBasis.size - 1);
        break;
      }
    } while (true);
    if (this->theGeneratingElementPowersBasis.size == DimOverRationals) {
      break;
    }
  }
}

bool AlgebraicClosureRationals::ReduceMe() {
  MacroRegisterFunctionWithName("AlgebraicExtensionRationals::ReduceMe");
  this->ChooseGeneratingElement();
  if (this->flagIsQuadraticRadicalExtensionRationals) {
    return true;
  }
  Polynomial<Rational> theMinPoly, smallestFactor;
  theMinPoly.AssignMinPoly(this->GeneratingElementMatForm);
  int theDim = this->theBasisMultiplicative.size;
  List<Polynomial<Rational> > theFactors;
  bool mustBeTrue = theMinPoly.FactorMe(theFactors, nullptr);
  if (!mustBeTrue) {
    crash << "This is a programming error: failed to factor polynomial " << theMinPoly.ToString() << crash;
  }
  smallestFactor = theFactors[0];
  if (smallestFactor.TotalDegreeInt() == theDim) {
    return true;
  }
  MatrixTensor<Rational> theBasisChangeMat, theBasisChangeMatInverse;
  theBasisChangeMat.AssignVectorsToColumns(this->theGeneratingElementPowersBasis);
  theBasisChangeMatInverse = theBasisChangeMat;
  theBasisChangeMatInverse.Invert();
  Polynomial<Rational> zToTheNth, remainderAfterReduction, tempP;
  MatrixTensor<Rational> theProjection;
  int smallestFactorDegree = - 1;
  if (!smallestFactor.TotalDegree().IsSmallInteger(&smallestFactorDegree)) {
    crash << "This is a programming error: " << smallestFactor.ToString()
    << " has non-integral exponent, which should be impossible in the current context. " << crash;
  }
  theProjection.MakeZero();
  for (int i = 0; i < smallestFactorDegree; i ++) {
    theProjection.AddMonomial(MonomialMatrix(i, i), 1);
  }
  for (int i = smallestFactorDegree; i < theDim; i ++) {
    zToTheNth.MakeMonomiaL(0, i, 1, 1);
    zToTheNth.DivideBy(smallestFactor, tempP, remainderAfterReduction);
    for (int j = 0; j < remainderAfterReduction.size(); j ++) {
      int theIndex = - 1;
      remainderAfterReduction[j](0).IsSmallInteger(&theIndex);
      theProjection.AddMonomial(MonomialMatrix(theIndex, i),remainderAfterReduction.theCoeffs[j]);
    }
  }
  this->theBasisMultiplicative.SetSize(smallestFactorDegree);
  MonomialMatrix tempM;
  for (int i = 0; i < this->theBasisMultiplicative.size; i ++) {
    this->theBasisMultiplicative[i].MakeZero();
    for (int j = 0; j < this->theBasisMultiplicative.size; j ++) {
      zToTheNth.MakeMonomiaL(0, i + j, 1, 1);
      zToTheNth.DivideBy(smallestFactor, tempP, remainderAfterReduction);
      for (int k = 0; k < remainderAfterReduction.size(); k ++) {
        int theIndex = - 666;
        remainderAfterReduction[k](0).IsSmallInteger(&theIndex);
        tempM.vIndex = theIndex;
        tempM.dualIndex = j;
        this->theBasisMultiplicative[i].AddMonomial(tempM, remainderAfterReduction.theCoeffs[k]);
      }
    }
  }
  this->GeneratingElemenT.owner = this;
  if (this->theBasisMultiplicative.size > 1) {
    this->GeneratingElemenT.theElT.MaKeEi(1, 1);
  } else {
    this->GeneratingElemenT.theElT.MaKeEi(0, 1);
  }
  this->GetMultiplicationBy(this->GeneratingElemenT, this->GeneratingElementTensorForm);
  this->GeneratingElementTensorForm.GetMatrix(this->GeneratingElementMatForm, this->theBasisMultiplicative.size);
  this->RegisterNewBasis(theProjection);
  return true;
}

void AlgebraicClosureRationals::GetAdditionTo(const AlgebraicNumber& input, VectorSparse<Rational>& output) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::GetAdditionTo");
  if (&output == &input.theElT) {
    AlgebraicNumber anCopy = input;
    this->GetAdditionTo(anCopy, output);
    return;
  }
  if (input.owner == nullptr) {
    if (input.theElT.size() > 0) {
      output.MaKeEi(0, input.theElT.theCoeffs[0]);
    }
    return;
  }
  if (input.basisIndex < 0 || input.basisIndex >= this->theBasesAdditive.size) {
    global.Comments << "This is a programming error: element has basis index " << input.basisIndex << ". "
    << Crasher::GetStackTraceEtcErrorMessageHTML();
  }
  if (input.basisIndex == this->theBasesAdditive.size - 1) {
    output = input.theElT;
    return;
  }
  output.MakeZero();
  for (int i = 0; i < input.theElT.size(); i ++) {
    int currentIndex = input.theElT[i].theIndex;
    if (currentIndex < 0 || currentIndex >= this->theBasesAdditive[input.basisIndex].size) {
      crash << "This is a programming error: I am getting basis index "
      << input.basisIndex << " with current index " << currentIndex
      << ". A printout of the algebraic closure follows. " << this->ToString() << crash;
    }
    output.AddOtherTimesConst(this->theBasesAdditive[input.basisIndex][currentIndex], input.theElT.theCoeffs[i]);
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
    if (inputAdditiveForm[i].theIndex < 0 || inputAdditiveForm[i].theIndex >= this->theBasisMultiplicative.size) {
      crash << "This is a programming error: element " << input.ToString()
      << " has bad index, namely, " << inputAdditiveForm[i].theIndex
      << ". The algebraic closure is: " << this->ToString() << ". " << crash;
    }
    currentMat = this->theBasisMultiplicative[inputAdditiveForm[i].theIndex];
    currentMat *= inputAdditiveForm.theCoeffs[i];
    output += currentMat;
  }
}

Rational AlgebraicNumber::GetDenominatorRationalPart() const {
  if (this->owner == nullptr) {
    if (this->theElT.IsEqualToZero()) {
      return 1;
    }
    return this->theElT.theCoeffs[0].GetDenominator();
  }
  VectorSparse<Rational> theEltAdditive;
  this->owner->GetAdditionTo(*this, theEltAdditive);
  LargeIntegerUnsigned resultLCM = 1;
  for (int i = 0; i < theEltAdditive.size(); i ++) {
    resultLCM = LargeIntegerUnsigned::lcm(resultLCM, theEltAdditive.theCoeffs[i].GetDenominator());
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
    crash << "something went wrong: floor function returns non-integer" << crash;
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
    sqrt6.AssignRationalQuadraticRadical(6, inputOwner);
    sqrt2.AssignRationalQuadraticRadical(2, inputOwner);
    *this = sqrt6 + sqrt2;
    *this /= 4 * sign;
    return true;
  }
  if (halfFracPart == Rational(5, 12)) {
    AlgebraicNumber sqrt6, sqrt2;
    sqrt6.AssignRationalQuadraticRadical(6, inputOwner);
    sqrt2.AssignRationalQuadraticRadical(2, inputOwner);
    *this = sqrt6 - sqrt2;
    *this /= 4 * sign;
    return true;
  }
  if (halfFracPart == Rational(1, 6)) {
    this->AssignRationalQuadraticRadical(3, inputOwner);
    *this /= 2;
    *this *= sign;
    return true;
  }
  if (halfFracPart == Rational(1, 4)) {
    this->AssignRationalQuadraticRadical(2, inputOwner);
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
    return this->theElT.theCoeffs[0].GetNumerator();
  }
  VectorSparse<Rational> theEltAdditive;
  this->owner->GetAdditionTo(*this, theEltAdditive);
  LargeInteger resultGCD = 1;
  LargeIntegerUnsigned tempR;
  if (theEltAdditive.size() > 0) {
    resultGCD = theEltAdditive.theCoeffs[0].GetNumerator();
  }
  for (int i = 1; i < theEltAdditive.size(); i ++) {
    tempR = resultGCD.value;
    LargeIntegerUnsigned::gcd(tempR, theEltAdditive.theCoeffs[i].GetNumerator().value, resultGCD.value);
  }
  return resultGCD;
}

unsigned int AlgebraicNumber::HashFunction() const {
  //crash << crash;
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
  return this->theElT.theCoeffs[0] == other;
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
  return additiveForm.theCoeffs[0].NeedsParenthesisForMultiplicationWhenSittingOnTheRightMost();
}

bool AlgebraicNumber::CheckNonZeroOwner() const {
  if (this->owner == nullptr) {
    crash << "This is a programming error: algebraic number with "
    << "non-initialized owner not permitted in the current context." << crash;
  }
  return true;
}

bool AlgebraicNumber::ConstructFromMinPoly(const Polynomial<AlgebraicNumber>& thePoly, AlgebraicClosureRationals& inputOwner) {
  MacroRegisterFunctionWithName("AlgebraicNumber::ConstructFromMinPoly");
  return inputOwner.AdjoinRootMinPoly(thePoly, *this);
}

void AlgebraicClosureRationals::reset() {
  this->flagIsQuadraticRadicalExtensionRationals = true;
  this->theBasisMultiplicative.SetSize(1);
  this->theBasisMultiplicative[0].MakeId(1);

  this->theGeneratingElementPowersBasis.SetSize(1);
  this->theGeneratingElementPowersBasis[0].MakeEi(1, 0);
  this->theBasesAdditive.SetSize(1);
  this->theBasesAdditive[0].SetSize(1);
  this->theBasesAdditive[0][0].MaKeEi(0);
  this->theQuadraticRadicals.Clear();
  this->DisplayNamesBasisElements.SetSize(1);
  this->DisplayNamesBasisElements[0] = "";
  this->GeneratingElementTensorForm.MakeId(1);
  this->GeneratingElementMatForm.MakeIdMatrix(1);
  this->GeneratingElemenT.owner = this;
  this->GeneratingElemenT.theElT.MaKeEi(0);
}

bool AlgebraicClosureRationals::AdjoinRootQuadraticPolyToQuadraticRadicalExtension(
  const Polynomial<AlgebraicNumber>& thePoly, AlgebraicNumber& outputRoot
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
    if (!algNumPoly.theCoeffs[i].IsRational(&currentCF)) {
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
  if (!outputRoot.AssignRationalQuadraticRadical(theConstTermShifted, *this)) {
    return false;
  }
  outputRoot -= theLinearTermCFdividedByTwo;
  //Check our work:
  PolynomialSubstitution<AlgebraicNumber> checkSub;
  checkSub.SetSize(1);
  checkSub[0].MakeConst(outputRoot);
  algNumPoly.Substitution(checkSub);
  if (!algNumPoly.IsEqualToZero()) {
    crash << "This is a programming error. The number z=" << outputRoot.ToString()
    << " was just adjoined to the base field; z "
    << "was given by requesting that it has minimial polynomial " << algNumPoly.ToString()
    << ", however, substituting z back in to the minimal polynomial "
    << "does not yield zero, rather yields " << algNumPoly.ToString() << ". " << crash;
  }
  return true;
}

void AlgebraicClosureRationals::ConvertPolyDependingOneVariableToPolyDependingOnFirstVariableNoFail(
  const Polynomial<AlgebraicNumber>& input, Polynomial<AlgebraicNumber>& output
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::ConvertPolyDependingOneVariableToPolyDependingOnFirstVariableNoFail");
  int indexVar = - 1;
  if (!input.IsOneVariableNonConstPoly(&indexVar)) {
    crash << "This is a programming error: "
    << "I am being asked convert to a one-variable polynomial a polynomial "
    << "depending on more than one variables. "
    << "The input poly is: " << input.ToString() << crash;
  }
  PolynomialSubstitution<AlgebraicNumber> theSub;
  theSub.MakeIdSubstitution(indexVar + 1);
  theSub[indexVar].MakeMonomiaL(0, 1, 1);
  output = input;
  output.Substitution(theSub);
}

bool AlgebraicClosureRationals::AdjoinRootMinPoly(const Polynomial<AlgebraicNumber>& thePoly, AlgebraicNumber& outputRoot) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::AdjoinRootMinPoly");
  if (this->AdjoinRootQuadraticPolyToQuadraticRadicalExtension(thePoly, outputRoot)) {
    return true;
  }
  Polynomial<AlgebraicNumber> minPoly;
  this->ConvertPolyDependingOneVariableToPolyDependingOnFirstVariableNoFail(thePoly, minPoly);
  int indexMaxMonMinPoly = minPoly.GetIndexMaxMonomial();
  AlgebraicNumber leadingCF = minPoly.theCoeffs[indexMaxMonMinPoly];
  minPoly /= leadingCF;
  AlgebraicClosureRationals backUpCopy;
  backUpCopy = *this;
  MatrixTensor<Rational> theGenMat;
  int degreeMinPoly = minPoly.TotalDegreeInt();
  int startingDim = this->theBasisMultiplicative.size;
  if (degreeMinPoly * startingDim > 10000 || startingDim > 10000 || degreeMinPoly > 10000) {
    global.Comments << "<hr>Adjoining root of minimial polynomial failed: "
    << "the current field extension dimension over the rationals is "
    << startingDim << ", the degree of the minimial polynomial is " << degreeMinPoly
    << ", yielding expected final dimension "
    << startingDim << "*" << degreeMinPoly << " = " << startingDim * degreeMinPoly
    << " over the rationals. The calculator is hard-coded "
    << "to accept dimension over the rationals no larger than 10000 "
    << "- multiplication in such a field corresponds to a "
    << "10000x10000 matrix (100 000 000 entries). "
    << "If you do indeed want to carry out such large computations, you need to "
    << "compile the calculator on your own, modifying file " << __FILE__ << ", line " << __LINE__ << ".";
    return false;
  }
  theGenMat.MakeZero();
  for (int i = 0; i < degreeMinPoly - 1; i ++) {
    for (int j = 0; j < startingDim; j ++) {
      theGenMat.AddMonomial(MonomialMatrix((i + 1) * startingDim + j, i * startingDim + j), 1);
    }
  }
  Polynomial<AlgebraicNumber> minusMinPolyMinusMaxMon = minPoly;
  int indexMaxMon = minusMinPolyMinusMaxMon.GetIndexMaxMonomial(
    MonomialP::LeftIsGEQTotalDegThenLexicographicLastVariableStrongest
  );
  const MonomialP maxMon = minusMinPolyMinusMaxMon[indexMaxMon];
  AlgebraicNumber maxMonCoeff = minusMinPolyMinusMaxMon.theCoeffs[indexMaxMon];
  minusMinPolyMinusMaxMon.SubtractMonomial(maxMon, maxMonCoeff);
  minusMinPolyMinusMaxMon /= maxMonCoeff;
  minusMinPolyMinusMaxMon /= - 1;
  MatrixTensor<Rational> currentCoeffMatForm;
  for (int i = 0; i < minusMinPolyMinusMaxMon.size(); i ++) {
    AlgebraicNumber& currentCoeff = minusMinPolyMinusMaxMon.theCoeffs[i];
    const MonomialP& currentMon = minusMinPolyMinusMaxMon[i];
    this->GetMultiplicationBy(currentCoeff, currentCoeffMatForm);
    for (int j = 0; j < currentCoeffMatForm.size(); j ++) {
      int relRowIndex = currentCoeffMatForm[j].vIndex;
      int relColIndex = currentCoeffMatForm[j].dualIndex;
      if (relRowIndex == - 1 || relColIndex == - 1) {
        crash << "This is a programming error: non initialized monomial. " << crash;
      }
      theGenMat.AddMonomial(MonomialMatrix(
          currentMon.TotalDegreeInt() * startingDim + relRowIndex,
          startingDim * (degreeMinPoly - 1) + relColIndex
        ), currentCoeffMatForm.theCoeffs[j]
      );
    }
  }
  int finalDim = degreeMinPoly * startingDim;
  List<MatrixTensor<Rational> > finalBasis;
  finalBasis.SetSize(this->theBasisMultiplicative.size);
  MatrixTensor<Rational> theGenMatPower;
  theGenMatPower.MakeId(degreeMinPoly);
  for (int i = 0; i < startingDim; i ++) {
    finalBasis[i].AssignTensorProduct(theGenMatPower, this->theBasisMultiplicative[i]);
  }
  this->theBasisMultiplicative = finalBasis;
  theBasisMultiplicative.SetSize(finalDim);
  theGenMatPower = theGenMat;
  for (int i = 1; i < degreeMinPoly; i ++) {
    for (int j = 0; j < startingDim; j ++) {
      this->theBasisMultiplicative[i * startingDim + j] = theGenMatPower;
      this->theBasisMultiplicative[i * startingDim + j] *= finalBasis[j];
    }
    if (i != degreeMinPoly - 1) {
      theGenMatPower *= theGenMat;
    }
  }
  MatrixTensor<Rational> theInjection;
  theInjection.MakeId(startingDim);
  this->RegisterNewBasis(theInjection);
  outputRoot.owner = this;
  outputRoot.theElT.MaKeEi(startingDim);
  outputRoot.basisIndex = this->theBasesAdditive.size - 1;
  this->flagIsQuadraticRadicalExtensionRationals = false;
  if (!this->ReduceMe()) {
    *this = backUpCopy;
    return false;
  }
  //Sanity check code here:
  PolynomialSubstitution<AlgebraicNumber> theSub;
  theSub.SetSize(1);
  theSub[0].MakeConst(outputRoot);
  minPoly.Substitution(theSub);
  if (!minPoly.IsEqualToZero()) {
    crash << "This is a programming error. The number z="
    << outputRoot.ToString() << " was just adjoined to the base field; z "
    << "was given by requesting that it has minimial polynomial "
    << minPoly.ToString() << ", however, substituting z back in to the minimal polynomial "
    << "does not yield zero, rather yields " << minPoly.ToString() << ". " << crash;
  }
  return true;
}

void AlgebraicNumber::Invert() {
  MacroRegisterFunctionWithName("AlgebraicNumber::Invert");
  if (this->owner == nullptr) {
    if (this->theElT.IsEqualToZero()) {
      crash << "This is a programming error: division by zero. " << crash;
    }
    bool isGood = (this->theElT.size() == 1);
    if (isGood) {
      isGood = (this->theElT[0].theIndex == 0);
    }
    if (!isGood) {
      crash << "This is a programming error: Algebraic number has no owner, "
      << "so it must be rational, but it appears to be not. "
      << "Its theElt vector is: " << this->theElT.ToString() << crash;
    }
    this->theElT.theCoeffs[0].Invert();
    return;
  }
  MatrixTensor<Rational> theInverted;
  Matrix<Rational> tempMat2;
  this->owner->GetMultiplicationBy(*this, theInverted);
  theInverted.GetMatrix(tempMat2, this->owner->theBasisMultiplicative.size);
  tempMat2.Invert();
  theInverted = tempMat2;
  this->theElT.MaKeEi(0);
  theInverted.ActOnVectorColumn(this->theElT);
  this->basisIndex = this->owner->theBasesAdditive.size - 1;
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
    crash << "This is a programming error. Two algebraic numbers have "
    << "different algebraic closures when they shouldn't. " << crash;
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
    crash << "This is a programming error: algebraic numbers "
    << "with zero owners but different basis indices. " << crash;
  }
  VectorSparse<Rational> AdditiveFormOther;
  theOwner->GetAdditionTo(*this, this->theElT);
  theOwner->GetAdditionTo(other, AdditiveFormOther);
  this->owner = theOwner;
  this->basisIndex = theOwner->theBasesAdditive.size - 1;
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
    crash << "This is a programming error: algebraic numbers: "
    << this->ToString() << " and " << other.ToString()
    << " have with zero owners but different basis indices. " << crash;
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
  this->basisIndex = theOwner->theBasesAdditive.size - 1;
  this->theElT += AdditiveFormOther;
  this->CheckConsistency();
}

bool AlgebraicNumber::CheckConsistency() const {
  if (this->flagDeallocated) {
    crash << "This is a programming error: use after free of AlgebraicNumber. " << crash;
  }
  if (this->owner == nullptr) {
    if (!this->IsRational()) {
      for (int i = 0; i < this->theElT.size(); i ++) {
        global.Comments << "<br>index: " << this->theElT[i].theIndex << ", coefficient: "
        << this->theElT.theCoeffs[i];
      }
      crash << "Detected non-rational algebraic number with zero owner. " << crash;
    }
    if (this->basisIndex != 0) {
      crash << "Algebraic number with non-zero basis and zero owner. " << crash;
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
    this->theElT *= other.theElT.theCoeffs[0];
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
  this->basisIndex = this->owner->theBasesAdditive.size - 1;
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
  return this->basisIndex == this->owner->theBasesAdditive.size - 1;
}

void AlgebraicNumber::ExpressViaLatestBasis() {
  if (this->owner == nullptr) {
    return;
  }
  if (this->basisIndex == this->owner->theBasesAdditive.size - 1) {
    return;
  }
  this->owner->GetAdditionTo(*this, this->theElT);
  this->basisIndex = this->owner->theBasesAdditive.size - 1;
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
    crash << "Owner is zero but algebraic number is not rational. " << crash;
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
      currentMultiplicand = this->theElT.theCoeffs[i].GetDoubleValue();
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

bool AlgebraicNumber::AssignRationalQuadraticRadical(const Rational& inpuT, AlgebraicClosureRationals& inputOwner) {
  MacroRegisterFunctionWithName("AlgebraicNumber::AssignRationalRadical");
  this->CheckConsistency();
  if (inpuT == 0) {
    return false;
  }
  if (!inputOwner.flagIsQuadraticRadicalExtensionRationals) {
    Polynomial<AlgebraicNumber> minPoly;
    minPoly.MakeMonomiaL(0, 2);
    minPoly -= inpuT;
    bool result = this->ConstructFromMinPoly(minPoly, inputOwner);
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
  if (!squareFreeInput.value.FactorReturnFalseIfFactorizationIncomplete(primeFactors, theMults, 0, nullptr)) {
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
  this->basisIndex = this->owner->theBasesAdditive.size - 1;
  this->theElT.MaKeEi(this->owner->GetIndexFromRadicalSelection(FactorSel));
  this->theElT *= squareRootRationalPart;
  this->CheckConsistency();
  return true;
}

void AlgebraicNumber::RadicalMeDefault(int theRad) {
  (void) theRad;
  crash << "Not implemented yet!" << crash;
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

std::string AlgebraicClosureRationals::ToString(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::ToString");
  (void) theFormat;//remove unused parameter warning, portable.
  std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML = false;
  tempFormat.flagUseLatex = true;
  if (this->theBasisMultiplicative.size == 1) {
    out << HtmlRoutines::GetMathSpanPure("\\mathbb Q");
    return out.str();
  }
  if (this->flagIsQuadraticRadicalExtensionRationals) {
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
  out << "Dimension over the rationals: " << this->theBasisMultiplicative.size << ". Multiplicative basis follows. ";
  for (int i = 0; i < this->theBasisMultiplicative.size; i ++) {
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
    theFlaStream << "=" << this->theBasisMultiplicative[i].ToStringMatForm(&tempFormat);
    out << HtmlRoutines::GetMathSpanPure(theFlaStream.str());
    if (i != this->theBasisMultiplicative.size - 1) {
      out << ",  ";
    }
  }
  if (this->flagIsQuadraticRadicalExtensionRationals) {
    out << "<br>Generating element not selected. ";
  } else {
    out << "<br>Generating element: " << HtmlRoutines::GetMathSpanPure(this->GeneratingElementMatForm.ToString(&tempFormat));
  }
  out << "<br>There are " << this->theBasesAdditive.size << " registered old bases. ";
  for (int i = 0; i < this->theBasesAdditive.size; i ++) {
    out << "<hr>Basis " << i + 1 << " has " << this->theBasesAdditive[i].size << " elements: ";
    for (int j = 0; j < this->theBasesAdditive[i].size; j ++) {
      out << HtmlRoutines::GetMathSpanPure(this->theBasesAdditive[i][j].ToString(&tempFormat));
      if (j != this->theBasesAdditive[i].size - 1) {
        out << ", ";
      }
    }
  }
  return out.str();
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
      *whichRational = this->theElT.theCoeffs[i];
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
    return this->theElT.theCoeffs[0].ToString(theFormat);
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
    crash << "This might or might not be a programming error: "
    << "comparing two algebraic number that do not have the same owner. "
    << "The numbers have owners of respective addresses "
    << this->owner << " and " << other.owner << ". The numbers are: "
    << this->ToString() << " and " << other.ToString() << ". Crashing to let you know. ";
    crash << crash;
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
    crash << "This is a programming error: attempting to assign non-constant "
    << "polynomial to a Rational number is not allowed. " << crash;
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
    crash << "This is a programming error: computing with non-initialized "
    << "element the ring Z mod p (the number p has not been initialized!)."
    << crash;
  }
  if (this->flagDeallocated) {
    crash << "This is a programming error: use after free of element z mod p. " << crash;
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
    theCF = input.theCoeffs[i];
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
    crash << "This is a programming error: attempting to make an operation "
    << "with two elemetns of Z mod P with different moduli, "
    << this->theModulus.ToString() << " and " << other.theModulus.ToString() << ". " << crash;
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
    crash << "This is a programming error: using ElementZmodP::operator= to assign a Rational number failed. "
    << " Operator = does not allow failure. " << crash;
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
