// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "math_extra_algebraic_numbers.h"
#include "math_subsets_selections.h"
#include "math_general_implementation.h"

const int ElementZmodP::maximumModulusForUserFacingPolynomialDivision = 10000;

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

int AlgebraicClosureRationals::getDimensionOverTheRationals() const {
  return this->latestBasis.size;
}

bool AlgebraicClosureRationals::checkConsistency() const {
  return true;
}

void AlgebraicClosureRationals::getMultiplicativeOperatorFromRadicalSelection(
  const Selection& theSel, MatrixTensor<Rational>& outputOp
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::getMultiplicativeOperatorFromRadicalSelection");
  outputOp.makeZero();
  Selection vectorActedOnSel, resultVectorSel;
  vectorActedOnSel.initialize(this->theQuadraticRadicals.size);
  resultVectorSel.initialize(this->theQuadraticRadicals.size);
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
    resultVectorSel.computeIndicesFromSelection();
    tempM.makeEij(this->getIndexFromRadicalSelection(resultVectorSel), this->getIndexFromRadicalSelection(vectorActedOnSel));
    outputOp.addMonomial(tempM, theCoeff);
  } while (vectorActedOnSel.incrementReturnFalseIfPastLast());
}

void AlgebraicClosureRationals::computeDisplayStringsFromRadicals() {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::computeDisplayStringsFromRadicals");
  if (!this->flagIsQuadraticRadicalExtensionRationals) {
    return;
  }
  this->displayNamesBasisElements.setSize(this->latestBasis.size);
  Selection theSel;
  theSel.initialize(this->theQuadraticRadicals.size);
  do {
    std::stringstream out;
    LargeInteger theRad = 1;
    for (int i = 0; i < theSel.cardinalitySelection; i ++) {
      theRad *= this->theQuadraticRadicals[theSel.elements[i]];
    }
    if (theRad != 1) {
      out << "\\sqrt{" << theRad.toString() << "}";
    }
    this->displayNamesBasisElements[this->getIndexFromRadicalSelection(theSel)] = out.str();
  } while (theSel.incrementReturnFalseIfPastLast());
}

bool AlgebraicClosureRationals::getRadicalSelectionFromIndex(int inputIndex, Selection& theSel) {
  if (!this->flagIsQuadraticRadicalExtensionRationals) {
    return false;
  }
  theSel.initialize(this->theQuadraticRadicals.size);
  int counter = 0;
  while (inputIndex > 0) {
    if (inputIndex % 2 == 1) {
      theSel.addSelectionAppendNewIndex(counter);
    }
    inputIndex /= 2;
    counter ++;
  }
  return true;
}

int AlgebraicClosureRationals::getIndexFromRadicalSelection(const Selection& theSel) {
  if (theSel.maximumSize > 30) {
    global.fatal
    << "This is a programming error: the algebraic extension "
    << "is too large to be handled by the current data structures. "
    << global.fatal;
  }
  int result = 0;
  for (int i = theSel.maximumSize - 1; i >= 0; i --) {
    result *= 2;
    if (theSel.selected[i]) {
      result += 1;
    }
  }
  return result;
}

bool AlgebraicClosureRationals::mergeRadicals(const List<LargeInteger>& theRadicals) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::MergeTwoQuadraticRadicalExtensions");
  if (!this->flagIsQuadraticRadicalExtensionRationals) {
    global.fatal << "This is a programming error: AlgebraicClosureRationals::MergeTwoQuadraticRadicalExtensions "
    << "with at least one of two arguments that is not a quadratic radical extension of the rationals. "
    << global.fatal;
  }
  //refine factors:
  HashedList<LargeInteger> radicalsNew = this->theQuadraticRadicals;
  radicalsNew.addOnTopNoRepetition(theRadicals);
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
          radicalsNew.removeIndexSwapWithLast(j);
          radicalsNew.removeIndexSwapWithLast(i);
          if (leftQuotient > 1) {
            radicalsNew.addOnTopNoRepetition(LargeInteger(leftQuotient));
          }
          if (rightQuotient > 1) {
            radicalsNew.addOnTopNoRepetition(LargeInteger(rightQuotient));
          }
          radicalsNew.addOnTopNoRepetition(LargeInteger(candidateGCD));
          found = true;
        }
      }
    }
  }
  radicalsNew.quickSortAscending();
  if (radicalsNew.size > 16) {
    global.comments << "Computing with fields whose dimension over the "
    << "rationals is greater than 2^16 is not allowed. "
    << "Such computations are too large for the current "
    << "implementation of algberaic extensions of the rationals. "
    << GlobalVariables::Crasher::getStackTraceEtcErrorMessageHTML();
    return (false);
  }
  this->latestBasis.setSize(MathRoutines::twoToTheNth(radicalsNew.size));
  MatrixTensor<Rational> currentInjection;
  currentInjection.makeZero();
  Selection largerFieldSel, smallerFieldSel;
  largerFieldSel.initialize(radicalsNew.size);
  smallerFieldSel.initialize(this->theQuadraticRadicals.size);
  do {
    largerFieldSel.initNoMemoryAllocation();
    for (int j = 0; j < this->theQuadraticRadicals.size; j ++) {
      if (smallerFieldSel.selected[j]) {
        if (this->theQuadraticRadicals[j] == - 1) {
          largerFieldSel.addSelectionAppendNewIndex(radicalsNew.getIndex(- 1));
          continue;
        }
        for (int k = 0; k < radicalsNew.size; k ++) {
          if (this->theQuadraticRadicals[j] % radicalsNew[k] == 0 && radicalsNew[k] != - 1) {
            largerFieldSel.addSelectionAppendNewIndex(k);
          }
        }
      }
    }
    currentInjection.addMonomial(MonomialMatrix(
        this->getIndexFromRadicalSelection(largerFieldSel),
        this->getIndexFromRadicalSelection(smallerFieldSel)
      ), 1
    );
  } while (smallerFieldSel.incrementReturnFalseIfPastLast());
  this->theQuadraticRadicals = radicalsNew;
  largerFieldSel.initialize(radicalsNew.size);
  do {
    this->getMultiplicativeOperatorFromRadicalSelection(
      largerFieldSel,
      this->latestBasis[this->getIndexFromRadicalSelection(largerFieldSel)]
    );
  } while (largerFieldSel.incrementReturnFalseIfPastLast());
  this->injectOldBases(&currentInjection);
  this->appendAdditiveEiBasis();
  this->computeDisplayStringsFromRadicals();
  return true;
}

void AlgebraicClosureRationals::assignDefaultBasisDisplayNames() {
  this->displayNamesBasisElements.setSize(this->latestBasis.size);
  int extensionNumber = this->basisInjections.size;
  for (int i = 1; i < this->latestBasis.size; i ++) {
    std::stringstream basisVector;
    basisVector << "e_{" << extensionNumber << ", " << i << "}";
    this->displayNamesBasisElements[i] = basisVector.str();
  }
}

void AlgebraicClosureRationals::injectOldBases(
  const MatrixTensor<Rational>* injectionNullForIdentity
) {
  if (injectionNullForIdentity == nullptr) {
    return;
  }
  for (int j = 0; j < this->basisInjections.size; j ++) {
    for (int i = 0; i < this->basisInjections[j].size; i ++) {
      injectionNullForIdentity->actOnVectorColumn(this->basisInjections[j][i]);
    }
  }
}
void AlgebraicClosureRationals::appendAdditiveEiBasis() {
  this->basisInjections.setSize(this->basisInjections.size + 1);
  this->basisInjections.lastObject()->setSize(this->latestBasis.size);
  for (int i = 0; i < this->latestBasis.size; i ++) {
    (*this->basisInjections.lastObject())[i].makeEi(i);
  }
}

bool AlgebraicClosureRationals::chooseGeneratingElement(
  int attemptsLimitZeroForNone, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::chooseGeneratingElement");
  SelectionPositiveIntegers theSel;
  int DimensionOverRationals = this->latestBasis.size;
  theSel.initialize(DimensionOverRationals);
  this->theGeneratingElementPowersBasis.setSize(0);
  Vector<Rational> currentVect;
  this->generatingElement.owner = this;
  this->generatingElement.basisIndex = this->basisInjections.size - 1;
  int attemptsSoFar = 0;
  if (this->basisInjections.size > 1) {
    int indexToSkipFirst = this->basisInjections[this->basisInjections.size - 2].size - 1;
    if (indexToSkipFirst < DimensionOverRationals) {
      theSel.theInts.makeEi(DimensionOverRationals, indexToSkipFirst);
    }
  }
  for (theSel.incrementReturnFalseIfPastLast(); ; theSel.incrementReturnFalseIfPastLast()) {
    attemptsSoFar ++;
    if (attemptsLimitZeroForNone > 0 && attemptsSoFar > attemptsLimitZeroForNone) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Choice of generating element is taking too long: made "
        << attemptsSoFar << " attempts so far, limit: " << attemptsLimitZeroForNone << ". ";
      }
      global.comments << "Choice of generating element is taking too long: made "
      << attemptsSoFar << " attempts so far, limit: " << attemptsLimitZeroForNone << ". ";
      return false;
    }
    this->generatingElement.element.makeZero();
    for (int i = 0; i < theSel.theInts.size; i ++) {
      MonomialVector tempV;
      tempV.makeEi(i);
      this->generatingElement.element.addMonomial(tempV, theSel.theInts[i]);
    }
    this->getMultiplicationBy(this->generatingElement, this->generatingElementTensorForm);
    this->generatingElementTensorForm.getMatrix(this->generatingElementMatrixForm, DimensionOverRationals);
    this->theGeneratingElementPowersBasis.setSize(0);
    currentVect.makeEi(DimensionOverRationals, 0);
    this->theGeneratingElementPowersBasis.addOnTop(currentVect);
    do {
      this->generatingElementMatrixForm.actOnVectorColumn(currentVect);
      this->theGeneratingElementPowersBasis.addOnTop(currentVect);
      if (
        this->theGeneratingElementPowersBasis.size >
        this->theGeneratingElementPowersBasis.getRankOfSpanOfElements()
      ) {
        this->theGeneratingElementPowersBasis.setSize(this->theGeneratingElementPowersBasis.size - 1);
        break;
      }
    } while (true);
    if (this->theGeneratingElementPowersBasis.size == DimensionOverRationals) {
      break;
    }
  }
  return true;
}

void AlgebraicClosureRationals::contractBasesIfRedundant(
  AlgebraicClosureRationals& previousCopy,
  AlgebraicNumber* outputImageGenerator
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::contractBasesIfRedundant");
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
  this->getAdditionTo(*outputImageGenerator, outputImageGenerator->element);
  MatrixTensor<Rational> reverseMap;

  reverseMap.assignVectorsToColumns(this->basisInjections[this->basisInjections.size - 3]);
  reverseMap.invert();
  reverseMap.actOnVectorColumn(outputImageGenerator->element);
  outputImageGenerator->basisIndex = this->basisInjections.size - 3;
  *this = previousCopy;
}

bool AlgebraicClosureRationals::reduceMe(
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::reduceMe");
  if (!this->chooseGeneratingElement(1000, commentsOnFailure)) {
    return false;
  }
  if (this->flagIsQuadraticRadicalExtensionRationals) {
    return true;
  }
  Polynomial<Rational> theMinPoly, smallestFactor;
  theMinPoly.assignMinPoly(this->generatingElementMatrixForm);
  int theDim = this->latestBasis.size;
  PolynomialFactorization<Rational, PolynomialFactorizationKronecker> factorization;
  bool mustBeTrue = factorization.factor(
    theMinPoly,
    nullptr,
    nullptr
  );
  if (!mustBeTrue) {
    global.fatal
    << "This is a programming error: failed to factor polynomial "
    << theMinPoly.toString() << ". "
    << global.fatal;
  }
  smallestFactor = factorization.reduced[0];
  if (smallestFactor.totalDegreeInt() == theDim) {
    return true;
  }
  MatrixTensor<Rational> generatorPowers, generatorPowersInverse;
  generatorPowers.assignVectorsToColumns(this->theGeneratingElementPowersBasis);
  generatorPowersInverse = generatorPowers;
  generatorPowersInverse.invert();
  Polynomial<Rational> zToTheNth, remainderAfterReduction, tempP;
  MatrixTensor<Rational> projectionGeneratorCoordinates;
  int smallestFactorDegree = - 1;
  if (!smallestFactor.totalDegree().isSmallInteger(&smallestFactorDegree)) {
    global.fatal
    << "This is a programming error: "
    << smallestFactor.toString()
    << " has non-integral exponent, which should be "
    << "impossible in the current context. "
    << global.fatal;
  }
  projectionGeneratorCoordinates.makeZero();
  for (int i = 0; i < smallestFactorDegree; i ++) {
    projectionGeneratorCoordinates.addMonomial(MonomialMatrix(i, i), 1);
  }
  for (int i = smallestFactorDegree; i < theDim; i ++) {
    zToTheNth.makeMonomial(0, i, 1);
    zToTheNth.divideBy(smallestFactor, tempP, remainderAfterReduction, &MonomialP::orderDefault());
    for (int j = 0; j < remainderAfterReduction.size(); j ++) {
      int theIndex = - 1;
      remainderAfterReduction[j](0).isSmallInteger(&theIndex);
      projectionGeneratorCoordinates.addMonomial(
        MonomialMatrix(theIndex, i),
        remainderAfterReduction.coefficients[j]
      );
    }
  }
  List<MatrixTensor<Rational> > newBasis;
  MatrixTensor<Rational> generatorProjected;
  Rational leadingCoefficient = smallestFactor.getLeadingCoefficient(nullptr);
  for (int i = 0; i < smallestFactorDegree; i ++) {
    MonomialMatrix termBelowMainDiagonal, termInLastColumn;
    if (i + 1 < smallestFactorDegree) {
      termBelowMainDiagonal.makeEij(i + 1, i);
      generatorProjected.addMonomial(termBelowMainDiagonal, 1);
    }
    termInLastColumn.makeEij(i, smallestFactorDegree - 1);
    Rational coefficientLastColumn = - smallestFactor.getCoefficientOf(MonomialP(0, i));
    coefficientLastColumn /= leadingCoefficient;
    generatorProjected.addMonomial(termInLastColumn, coefficientLastColumn);
    if (coefficientLastColumn != 0 && generatorProjected.isEqualToZero()) {
      global.fatal << "We shouldn't have zero generator. " << global.fatal;
    }
  }
  newBasis.setSize(smallestFactorDegree);
  newBasis[0].makeIdentity(smallestFactorDegree);
  newBasis[1] = generatorProjected;
  for (int i = 2; i < smallestFactorDegree; i ++) {
    newBasis[i] = generatorProjected;
    newBasis[i].raiseToPower(i);
  }
  this->latestBasis = newBasis;
  MatrixTensor<Rational> injection;
  injection = projectionGeneratorCoordinates;
  injection *= generatorPowersInverse;
  this->injectOldBases(&injection);
  this->appendAdditiveEiBasis();
  this->assignDefaultBasisDisplayNames();
  if (!this->chooseGeneratingElement(1000, commentsOnFailure)) {
    return false;
  }
  this->getMultiplicationBy(
    this->generatingElement, this->generatingElementTensorForm
  );
  this->generatingElementTensorForm.getMatrix(
    this->generatingElementMatrixForm, this->latestBasis.size
  );
  return true;
}

void AlgebraicClosureRationals::getAdditionTo(
  const AlgebraicNumber& input, VectorSparse<Rational>& output
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::getAdditionTo");
  if (&output == &input.element) {
    AlgebraicNumber anCopy = input;
    this->getAdditionTo(anCopy, output);
    return;
  }
  if (input.owner == nullptr) {
    if (input.element.size() > 0) {
      output.makeEi(0, input.element.coefficients[0]);
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
  output.makeZero();
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
    output.addOtherTimesConst(
      this->basisInjections[input.basisIndex][currentIndex],
      input.element.coefficients[i]
    );
  }
}

void AlgebraicClosureRationals::getMultiplicationBy(
  const AlgebraicNumber& input, MatrixTensor<Rational>& output
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::getMultiplicationBy");
  output.makeZero();
  VectorSparse<Rational> inputAdditiveForm;
  this->getAdditionTo(input, inputAdditiveForm);
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

bool AlgebraicNumber::assignCosRationalTimesPi(const Rational& input, AlgebraicClosureRationals& inputOwner) {
  MacroRegisterFunctionWithName("AlgebraicNumber::assignCosRationalTimesPi");
  Rational fracPart = input;
  fracPart.assignFractionalValue();
  Rational halfIntegerPart = input * 2;
  halfIntegerPart.assignFloor();
  LargeInteger halfIntegerPartTimesTwo;
  if (!halfIntegerPart.isInteger(&halfIntegerPartTimesTwo)) {
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
    sqrt6.assignRationalQuadraticRadical(6, inputOwner, nullptr);
    sqrt2.assignRationalQuadraticRadical(2, inputOwner, nullptr);
    *this = sqrt6 + sqrt2;
    *this /= 4 * sign;
    return true;
  }
  if (halfFracPart == Rational(5, 12)) {
    AlgebraicNumber sqrt6, sqrt2;
    sqrt6.assignRationalQuadraticRadical(6, inputOwner, nullptr);
    sqrt2.assignRationalQuadraticRadical(2, inputOwner, nullptr);
    *this = sqrt6 - sqrt2;
    *this /= 4 * sign;
    return true;
  }
  if (halfFracPart == Rational(1, 6)) {
    this->assignRationalQuadraticRadical(3, inputOwner, nullptr);
    *this /= 2;
    *this *= sign;
    return true;
  }
  if (halfFracPart == Rational(1, 4)) {
    this->assignRationalQuadraticRadical(2, inputOwner, nullptr);
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


unsigned int AlgebraicNumber::hashFunction() const {
  //global.fatal << global.fatal;
  //WARNING. Algebraic numbers, as they are recorded in memory at the moment,
  //do not have unique presentations, so we return 0 as their hash function.
  //Computing a hash function can be done, for example, by picking the hash function of the minimal polynomial
  //over the rationals. However, such computations appear to be too heavy, and will make sense only if we need to deal
  //with large sets of algebraic numbers without repetition.
  return 0;
}

bool AlgebraicNumber::operator==(const Rational& other) const {
  if (this->element.isEqualToZero()) {
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

bool AlgebraicNumber::needsParenthesisForMultiplication(
  FormatExpressions* unused
) const {
  (void) unused;
  if (this->owner == nullptr) {
    return false;
  }
  VectorSparse<Rational> additiveForm;
  this->owner->getAdditionTo(*this, additiveForm);
  return (additiveForm.size() > 1);
}

bool AlgebraicNumber::needsParenthesisForMultiplicationWhenSittingOnTheRightMost() const {
  if (this->owner == nullptr) {
    Rational tempRat;
    if (this->isRational(&tempRat)) {
      return tempRat.needsParenthesisForMultiplicationWhenSittingOnTheRightMost();
    }
    return false;
  }
  VectorSparse<Rational> additiveForm;
  this->owner->getAdditionTo(*this, additiveForm);
  if (additiveForm.size() != 1) {
    return true;
  }
  return additiveForm.coefficients[0].needsParenthesisForMultiplicationWhenSittingOnTheRightMost();
}

bool AlgebraicNumber::checkNonZeroOwner() const {
  if (this->owner == nullptr) {
    global.fatal << "This is a programming error: algebraic number with "
    << "non-initialized owner not permitted in the current context." << global.fatal;
  }
  return true;
}

bool AlgebraicNumber::constructFromMinimalPolynomial(
  const Polynomial<AlgebraicNumber>& thePoly,
  AlgebraicClosureRationals& inputOwner,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("AlgebraicNumber::constructFromMinimalPolynomial");
  return inputOwner.adjoinRootMinimalPolynomial(
    thePoly, *this, commentsOnFailure
  );
}

void AlgebraicClosureRationals::reset() {
  this->flagIsQuadraticRadicalExtensionRationals = true;
  this->latestBasis.setSize(1);
  this->latestBasis[0].makeIdentity(1);

  this->theGeneratingElementPowersBasis.setSize(1);
  this->theGeneratingElementPowersBasis[0].makeEi(1, 0);
  this->basisInjections.setSize(1);
  this->basisInjections[0].setSize(1);
  this->basisInjections[0][0].makeEi(0);
  this->theQuadraticRadicals.clear();
  this->displayNamesBasisElements.setSize(1);
  this->displayNamesBasisElements[0] = "";
  this->generatingElementTensorForm.makeIdentity(1);
  this->generatingElementMatrixForm.makeIdentityMatrix(1);
  this->generatingElement.owner = this;
  this->generatingElement.element.makeEi(0);
}

bool AlgebraicClosureRationals::adjoinRootQuadraticPolynomialToQuadraticRadicalExtension(
  const Polynomial<AlgebraicNumber>& thePoly,
  AlgebraicNumber& outputRoot,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::adjoinRootQuadraticPolynomialToQuadraticRadicalExtension");
  if (thePoly.totalDegree() != 2 || !this->flagIsQuadraticRadicalExtensionRationals) {
    return false;
  }
  Polynomial<AlgebraicNumber> algNumPoly;
  this->convertPolyDependingOneVariableToPolyDependingOnFirstVariableNoFail(thePoly, algNumPoly);
  Polynomial<Rational> minPoly;
  minPoly.makeZero();
  Rational currentCF, theLinearTermCFdividedByTwo, theConstTermShifted;
  for (int i = 0; i < algNumPoly.size(); i ++) {
    if (!algNumPoly.coefficients[i].isRational(&currentCF)) {
      return false;
    } else {
      minPoly.addMonomial(algNumPoly[i], currentCF);
    }
  }
  List<MonomialP>::Comparator* monomialOrder = &MonomialP::orderDefault();
  minPoly /= minPoly.getLeadingCoefficient(monomialOrder);
  minPoly.getCoefficientInFrontOfLinearTermVariableIndex(0, theLinearTermCFdividedByTwo);
  theLinearTermCFdividedByTwo /= 2;
  minPoly.getConstantTerm(theConstTermShifted);
  theConstTermShifted -= theLinearTermCFdividedByTwo*theLinearTermCFdividedByTwo;
  theConstTermShifted *= - 1;
  if (!outputRoot.assignRationalQuadraticRadical(
    theConstTermShifted, *this, commentsOnFailure
  )) {
    return false;
  }
  outputRoot -= theLinearTermCFdividedByTwo;
  //check our work:
  PolynomialSubstitution<AlgebraicNumber> checkSub;
  checkSub.setSize(1);
  checkSub[0].makeConstant(outputRoot);
  algNumPoly.substitution(checkSub);
  if (!algNumPoly.isEqualToZero()) {
    global.fatal << "This is a programming error. The number z = " << outputRoot.toString()
    << " was just adjoined to a quadratic radical extension of the rationals; z "
    << "was given by requesting that it has minimial polynomial " << algNumPoly.toString()
    << ", however, substituting z back in to the minimal polynomial "
    << "does not yield zero, rather yields " << algNumPoly.toString() << ". " << global.fatal;
  }
  return true;
}

void AlgebraicClosureRationals::convertPolyDependingOneVariableToPolyDependingOnFirstVariableNoFail(
  const Polynomial<AlgebraicNumber>& input, Polynomial<AlgebraicNumber>& output
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::convertPolyDependingOneVariableToPolyDependingOnFirstVariableNoFail");
  int indexVar = - 1;
  if (!input.isOneVariableNonConstantPolynomial(&indexVar)) {
    global.fatal << "This is a programming error: "
    << "I am being asked convert to a one-variable polynomial a polynomial "
    << "depending on more than one variables. "
    << "The input poly is: " << input.toString() << global.fatal;
  }
  PolynomialSubstitution<AlgebraicNumber> theSub;
  theSub.makeIdentitySubstitution(indexVar + 1);
  theSub[indexVar].makeMonomial(0, 1, 1);
  output = input;
  output.substitution(theSub);
}

bool AlgebraicClosureRationals::adjoinRootMinimalPolynomial(
  const Polynomial<AlgebraicNumber>& thePoly,
  AlgebraicNumber& outputRoot,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("AlgebraicClosureRationals::AdjoinRootMinPoly");
  if (this->adjoinRootQuadraticPolynomialToQuadraticRadicalExtension(
    thePoly, outputRoot, commentsOnFailure
  )) {
    return true;
  }
  Polynomial<AlgebraicNumber> minPoly;
  this->convertPolyDependingOneVariableToPolyDependingOnFirstVariableNoFail(thePoly, minPoly);
  List<MonomialP>::Comparator* monomialOrder = &MonomialP::orderDefault();
  AlgebraicNumber leadingCoefficient = minPoly.getLeadingCoefficient(monomialOrder);
  minPoly /= leadingCoefficient;
  AlgebraicClosureRationals backUpCopy;
  backUpCopy = *this;
  MatrixTensor<Rational> theGenMat;
  int degreeMinPoly = minPoly.totalDegreeInt();
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
  theGenMat.makeZero();
  for (int i = 0; i < degreeMinPoly - 1; i ++) {
    for (int j = 0; j < startingDimension; j ++) {
      theGenMat.addMonomial(MonomialMatrix((i + 1) * startingDimension + j, i * startingDimension + j), 1);
    }
  }
  Polynomial<AlgebraicNumber> minusMinimalPolynomialMinusMaximalMonomial = minPoly;
  MonomialP leadingMonomial;
  AlgebraicNumber leadingCoefficientModified;
  minusMinimalPolynomialMinusMaximalMonomial.getIndexLeadingMonomial(
    &leadingMonomial, &leadingCoefficientModified, monomialOrder
  );
  minusMinimalPolynomialMinusMaximalMonomial.subtractMonomial(leadingMonomial, leadingCoefficientModified);
  minusMinimalPolynomialMinusMaximalMonomial /= leadingCoefficientModified;
  minusMinimalPolynomialMinusMaximalMonomial *= - 1;
  MatrixTensor<Rational> currentCoeffMatForm;
  for (int i = 0; i < minusMinimalPolynomialMinusMaximalMonomial.size(); i ++) {
    AlgebraicNumber& currentCoeff = minusMinimalPolynomialMinusMaximalMonomial.coefficients[i];
    const MonomialP& currentMon = minusMinimalPolynomialMinusMaximalMonomial[i];
    this->getMultiplicationBy(currentCoeff, currentCoeffMatForm);
    for (int j = 0; j < currentCoeffMatForm.size(); j ++) {
      int relRowIndex = currentCoeffMatForm[j].vIndex;
      int relColIndex = currentCoeffMatForm[j].dualIndex;
      if (relRowIndex == - 1 || relColIndex == - 1) {
        global.fatal << "This is a programming error: non initialized monomial. " << global.fatal;
      }
      theGenMat.addMonomial(
        MonomialMatrix(
          currentMon.totalDegreeInt() * startingDimension + relRowIndex,
          startingDimension * (degreeMinPoly - 1) + relColIndex
        ),
        currentCoeffMatForm.coefficients[j]
      );
    }
  }
  int finalDimension = degreeMinPoly * startingDimension;
  List<MatrixTensor<Rational> > finalBasis;
  finalBasis.setSize(this->latestBasis.size);
  MatrixTensor<Rational> theGenMatPower;
  theGenMatPower.makeIdentity(degreeMinPoly);
  for (int i = 0; i < startingDimension; i ++) {
    finalBasis[i].assignTensorProduct(theGenMatPower, this->latestBasis[i]);
  }
  this->latestBasis = finalBasis;
  this->latestBasis.setSize(finalDimension);
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
  this->appendAdditiveEiBasis();
  outputRoot.owner = this;
  outputRoot.element.makeEi(startingDimension);
  outputRoot.basisIndex = this->basisInjections.size - 1;
  this->flagIsQuadraticRadicalExtensionRationals = false;
  if (!this->reduceMe(commentsOnFailure)) {
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
  this->contractBasesIfRedundant(backUpCopy, &outputRoot);


  // Sanity check code here:
  PolynomialSubstitution<AlgebraicNumber> theSub;
  theSub.setSize(1);
  theSub[0].makeConstant(outputRoot);
  Polynomial<AlgebraicNumber> substitutedMinPoly;
  substitutedMinPoly = minPoly;
  substitutedMinPoly.substitution(theSub);
  if (!substitutedMinPoly.isEqualToZero()) {
    global.fatal << "This is a programming error. The number z = "
    << outputRoot.toString() << " was just adjoined to the base field; z "
    << "was given by requesting that it has minimial polynomial "
    << minPoly.toString()
    << ", however, substituting z back in to the minimal polynomial "
    << "does not yield zero, rather yields "
    << substitutedMinPoly.toString() << ". "
    << "The algebraic closure printout follows. "
    << this->toStringFull() << global.fatal;
  }
  return true;
}

void AlgebraicNumber::invert() {
  MacroRegisterFunctionWithName("AlgebraicNumber::invert");
  if (this->owner == nullptr) {
    if (this->element.isEqualToZero()) {
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
    this->element.coefficients[0].invert();
    return;
  }
  MatrixTensor<Rational> theInverted;
  Matrix<Rational> tempMat2;
  this->owner->getMultiplicationBy(*this, theInverted);
  theInverted.getMatrix(tempMat2, this->owner->latestBasis.size);
  tempMat2.invert();
  theInverted = tempMat2;
  this->element.makeEi(0);
  theInverted.actOnVectorColumn(this->element);
  this->basisIndex = this->owner->basisInjections.size - 1;
}

void AlgebraicNumber::operator/=(const AlgebraicNumber& other) {
  MacroRegisterFunctionWithName("AlgebraicNumber::operator/=");
  AlgebraicNumber otherCopy = other;
  otherCopy.invert();
  *this *= otherCopy;
}

void AlgebraicNumber::operator*=(const Rational& other) {
  this->element *= other;
}

bool AlgebraicNumber::checkCommonOwner(const AlgebraicNumber& other) const {
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
  this->checkCommonOwner(other);
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
  theOwner->getAdditionTo(*this, this->element);
  theOwner->getAdditionTo(other, AdditiveFormOther);
  this->owner = theOwner;
  this->basisIndex = theOwner->basisInjections.size - 1;
  this->element -= AdditiveFormOther;
}

void AlgebraicNumber::operator+=(const AlgebraicNumber& other) {
  MacroRegisterFunctionWithName("AlgebraicNumber::operator+=");
  this->checkConsistency();
  other.checkConsistency();
  if (this == &other) {
    AlgebraicNumber otherCopy = other;
    *this += otherCopy;
    this->checkConsistency();
    return;
  }
  this->checkCommonOwner(other);
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
    this->checkConsistency();
    other.checkConsistency();
    this->element += other.element;
    this->checkConsistency();
    return;
  }
  VectorSparse<Rational> AdditiveFormOther;
  theOwner->getAdditionTo(*this, this->element);
  theOwner->getAdditionTo(other, AdditiveFormOther);
  this->owner = theOwner;
  this->basisIndex = theOwner->basisInjections.size - 1;
  this->element += AdditiveFormOther;
  this->checkConsistency();
}

bool AlgebraicNumber::checkConsistency() const {
  if (this->flagDeallocated) {
    global.fatal << "This is a programming error: use after free of AlgebraicNumber. " << global.fatal;
  }
  if (this->owner == nullptr) {
    if (!this->isRational()) {
      for (int i = 0; i < this->element.size(); i ++) {
        global.comments << "<br>index: " << this->element[i].theIndex << ", coefficient: "
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
  this->checkConsistency();
  if (other.owner == nullptr) {
    if (other.isEqualToZero()) {
      this->element.makeZero();
      return;
    }
    this->element *= other.element.coefficients[0];
    return;
  }
  if (this->owner == nullptr) {
    if (this->element.isEqualToZero()) {
      return;
    }
    Rational tempRat = this->element.getCoefficientOf(MonomialVector(0));
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
    theReport.report(reportStream.str());
  }
  this->checkCommonOwner(other);
  MatrixTensor<Rational> leftMatrix, rightMatrix;
  this->owner->getMultiplicationBy(*this, leftMatrix);
  this->owner->getMultiplicationBy(other, rightMatrix);
  leftMatrix.checkConsistencyGrandMaster();
  rightMatrix.checkConsistencyGrandMaster();
  leftMatrix *= rightMatrix;
  this->basisIndex = this->owner->basisInjections.size - 1;
  this->element.makeEi(0);
  leftMatrix.actOnVectorColumn(this->element);
}

void AlgebraicNumber::squareRootDefault(std::stringstream* commentsOnError) {
  this->radicalMeDefault(2, commentsOnError);
}

bool AlgebraicNumber::operator>(const AlgebraicNumber& other) const {
  Rational left, right;
  if (this->isRational(&left) && other.isRational(&right)) {
    return left > right;
  }
  this->checkCommonOwner(other);
  return this->element > other.element;
}

bool AlgebraicNumber::constructFromMinimalPolynomial(
  const Polynomial<Rational>& thePoly,
  AlgebraicClosureRationals& inputOwner,
  std::stringstream* commentsOnFailure
) {
  Polynomial<AlgebraicNumber> polyConverted;
  polyConverted = thePoly;
  return this->constructFromMinimalPolynomial(polyConverted, inputOwner, commentsOnFailure);
}

AlgebraicNumber AlgebraicNumber::one() {
  AlgebraicNumber result;
  result.assignRational(1, *this->owner);
  return result;
}

void AlgebraicNumber::assignRational(const Rational& input, AlgebraicClosureRationals& inputOwner) {
  this->basisIndex = 0;
  this->owner = &inputOwner;
  this->element.makeEi(0, input);
}

AlgebraicNumber AlgebraicNumber::zero() {
  return AlgebraicNumber::zeroStatic();
}

AlgebraicNumber AlgebraicNumber::zeroStatic() {
  AlgebraicNumber result;
  return result;
}

bool AlgebraicNumber::isExpressedViaLatestBasis() const {
  if (this->owner == nullptr) {
    return true;
  }
  return this->basisIndex == this->owner->basisInjections.size - 1;
}

void AlgebraicNumber::expressViaLatestBasis() {
  if (this->owner == nullptr) {
    return;
  }
  if (this->basisIndex == this->owner->basisInjections.size - 1) {
    return;
  }
  this->owner->getAdditionTo(*this, this->element);
  this->basisIndex = this->owner->basisInjections.size - 1;
}

bool AlgebraicNumber::evaluatesToDouble(double* outputWhichDouble) const {
  MacroRegisterFunctionWithName("AlgebraicNumber::evaluatesToDouble");
  if (!this->isExpressedViaLatestBasis()) {
    AlgebraicNumber thisCopy = *this;
    thisCopy.expressViaLatestBasis();
    return thisCopy.evaluatesToDouble(outputWhichDouble);
  }
  Rational ratValue;
  if (this->isRational(&ratValue)) {
    if (outputWhichDouble != nullptr) {
      *outputWhichDouble = ratValue.getDoubleValue();
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
    this->owner->getRadicalSelectionFromIndex(this->element[i].theIndex, currentRadicalSelection);
    if (outputWhichDouble != nullptr) {
      currentMultiplicand = this->element.coefficients[i].getDoubleValue();
    }
    for (int j = 0; j < currentRadicalSelection.cardinalitySelection; j ++) {
      if (this->owner->theQuadraticRadicals[currentRadicalSelection.elements[j]] < 0) {
        return false;
      } else {
        if (outputWhichDouble != nullptr) {
          currentMultiplicand *= FloatingPoint::sqrtFloating(
            this->owner->theQuadraticRadicals[currentRadicalSelection.elements[j]].getDoubleValue()
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

bool AlgebraicNumber::assignRationalQuadraticRadical(
  const Rational& input,
  AlgebraicClosureRationals& inputOwner,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("AlgebraicNumber::AssignRationalRadical");
  this->checkConsistency();
  if (input == 0) {
    return false;
  }
  if (!inputOwner.flagIsQuadraticRadicalExtensionRationals) {
    Polynomial<AlgebraicNumber> minPoly;
    minPoly.makeMonomial(0, 2);
    minPoly -= input;
    bool result = this->constructFromMinimalPolynomial(minPoly, inputOwner, commentsOnFailure);
    if (result) {
      this->checkConsistency();
    }
    return result;
  }
  List<LargeInteger> theFactors;
  Rational absoluteInput = input;
  if (absoluteInput < 0) {
    theFactors.addOnTop(- 1);
    absoluteInput *= - 1;
  }
  LargeInteger squareFreeInput = absoluteInput.getNumerator();
  squareFreeInput *= absoluteInput.getDenominator();
  List<LargeInteger> primeFactors;
  List<int> theMults;
  if (!squareFreeInput.value.factor(
    primeFactors, theMults, 0, 4, nullptr
  )) {
    return false;
  }
  squareFreeInput.value = 1;
  Rational squareRootRationalPart = 1;
  squareRootRationalPart /= absoluteInput.getDenominator();
  for (int i = 0; i < primeFactors.size; i ++) {
    if (theMults[i] % 2 == 1) {
      squareFreeInput *= primeFactors[i];
    }
    primeFactors[i].raiseToPower(theMults[i] / 2);
    squareRootRationalPart *= primeFactors[i];
  }
  if (!squareFreeInput.isEqualToOne()) {
    theFactors.addOnTop(squareFreeInput);
  }
  if (theFactors.size == 0) {
    this->assignRational(squareRootRationalPart, inputOwner);
    this->checkConsistency();
    return true;
  }
  if (!inputOwner.mergeRadicals(theFactors)) {
    return false;
  }
  Selection FactorSel;
  FactorSel.initialize(inputOwner.theQuadraticRadicals.size);
  for (int i = 0; i < theFactors.size; i ++) {
    if (theFactors[i] == - 1) {
      FactorSel.addSelectionAppendNewIndex(inputOwner.theQuadraticRadicals.getIndex(- 1));
    } else {
      for (int j = 0; j < inputOwner.theQuadraticRadicals.size; j ++) {
        if (inputOwner.theQuadraticRadicals[j] != - 1) {
          if (theFactors[i] % inputOwner.theQuadraticRadicals[j] == 0) {
            FactorSel.addSelectionAppendNewIndex(j);
          }
        }
      }
    }
  }
  this->owner = &inputOwner;
  this->basisIndex = this->owner->basisInjections.size - 1;
  this->element.makeEi(this->owner->getIndexFromRadicalSelection(FactorSel));
  this->element *= squareRootRationalPart;
  this->checkConsistency();
  return true;
}

bool AlgebraicNumber::radicalMeDefault(
  int radical, std::stringstream* commentsOnError
) {
  MacroRegisterFunctionWithName("AlgebraicNumber::radicalMeDefault");
  if (this->owner == nullptr) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Failed to extract radical: algebraic closure is missing. ";
    }
    return false;
  }
  AlgebraicNumber result, one, minusOne;
  one.assignRational(1, *this->owner);
  minusOne.assignRational(- 1, *this->owner);
  Polynomial<AlgebraicNumber> thePolynomial;
  thePolynomial.addConstant(*this * minusOne);
  MonomialP leadingMonomial;
  leadingMonomial.makeEi(0, radical);
  thePolynomial.addMonomial(leadingMonomial, one);
  if (!this->owner->adjoinRootMinimalPolynomial(
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

std::string AlgebraicClosureRationals::toStringQuadraticRadical(
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
  return HtmlRoutines::getMathSpanPure(out.str());
}

std::string AlgebraicClosureRationals::toStringFull(FormatExpressions* theFormat) const {
  std::stringstream out;
  out << "Dimension over the rationals: "
  << this->latestBasis.size << ". Multiplicative basis follows. ";
  for (int i = 0; i < this->latestBasis.size; i ++) {
    out << "<br>";
    std::stringstream theFlaStream;
    if (i < this->displayNamesBasisElements.size) {
      if (this->displayNamesBasisElements[i] == "") {
        theFlaStream << "1";
      } else {
        theFlaStream << this->displayNamesBasisElements[i];
      }
    } else {
      theFlaStream << " e_{" << i + 1 << "}";
    }
    theFlaStream << "=" << this->latestBasis[i].toStringMatrixForm(theFormat);
    out << HtmlRoutines::getMathSpanPure(theFlaStream.str());
    if (i != this->latestBasis.size - 1) {
      out << ",  ";
    }
  }
  if (this->flagIsQuadraticRadicalExtensionRationals) {
    out << "<br>Generating element not selected. ";
  } else {
    out << "<br>Generating element: "
    << HtmlRoutines::getMathSpanPure(this->generatingElementMatrixForm.toString(theFormat));
  }
  out << "<br>There are " << this->basisInjections.size << " registered old bases. ";
  for (int i = 0; i < this->basisInjections.size; i ++) {
    out << "<hr>Basis " << i + 1 << " has " << this->basisInjections[i].size << " elements: ";
    for (int j = 0; j < this->basisInjections[i].size; j ++) {
      out << HtmlRoutines::getMathSpanPure(this->basisInjections[i][j].toString(theFormat));
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
        if (!current[j].isEqualToZero()) {
          break;
        }
      }
    }
    allCoefficients.addListOnTop(output[i].element.coefficients);
  }
  Rational scale = Rational::scaleNormalizeIndex(allCoefficients, indexNonZeroRational);
  for (int i = 0; i < output.size; i ++){
    output[i] *= scale;
  }
  return scale;
}

AlgebraicNumber AlgebraicClosureRationals::one() {
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
    out << HtmlRoutines::getMathSpanPure("\\mathbb Q");
    return out.str();
  }
  if (this->flagIsQuadraticRadicalExtensionRationals) {
    return this->toStringQuadraticRadical(&tempFormat);
  }
  return this->toStringFull(&tempFormat);
}

bool AlgebraicNumber::isRational(Rational* whichRational) const {
  if (this->isEqualToZero()) {
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

bool AlgebraicNumber::isEqualToZero() const {
  return this->element.isEqualToZero();
}

std::string AlgebraicNumber::toString(FormatExpressions* theFormat) const {
  if (this->owner == nullptr) {
    if (this->element.isEqualToZero()) {
      return "0";
    }
    return this->element.coefficients[0].toString(theFormat);
  }
  std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.vectorSpaceEiBasisNames = this->owner->displayNamesBasisElements;
  if (theFormat != nullptr) {
    tempFormat.flagUseFrac = theFormat->flagUseFrac;
  }
  VectorSparse<Rational> theAdditiveVector;
  this->owner->getAdditionTo(*this, theAdditiveVector);
  out << theAdditiveVector.toString(&tempFormat); //<< "~ in~ the~ field~ " << this->owner->toString();
  if (this->basisIndex < this->owner->basisInjections.size - 1 && global.userDebugFlagOn()) {
    out << "[=" << this->toStringNonInjected() << "]";
  }
  return out.str();
}

std::string AlgebraicNumber::toStringNonInjected(FormatExpressions* theFormat) const {
  if (this->owner == nullptr) {
    if (this->element.isEqualToZero()) {
      return "0";
    }
    return this->element.coefficients[0].toString(theFormat);
  }
  std::stringstream out;
  FormatExpressions tempFormat;
  int dimension = this->owner->basisInjections[this->basisIndex].size;
  tempFormat.vectorSpaceEiBasisNames.setSize(dimension);

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
  this->checkConsistency();
  other.checkConsistency();
  if (this->isRational(&ratValue)) {
    return other == ratValue;
  }
  if (other.isRational(&ratValue)) {
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
  this->checkNonZeroOwner();
  if (this->basisIndex == other.basisIndex) {
    return this->element == other.element;
  }
  VectorSparse<Rational> leftAdditive, rightAdditive;
  this->owner->getAdditionTo(*this, leftAdditive);
  this->owner->getAdditionTo(other, rightAdditive);
  return leftAdditive == rightAdditive;
}

void AlgebraicNumber::operator=(const Polynomial<AlgebraicNumber>& other) {
  if (!other.isConstant(this)) {
    global.fatal
    << "This is a programming error: attempting to assign non-constant "
    << "polynomial to a Rational number is not allowed. " << global.fatal;
  }
}

void AlgebraicNumber::operator=(const Rational& other) {
  this->owner = nullptr;
  this->element.makeEi(0, other);
  this->basisIndex = 0;
}

bool ElementZmodP::needsParenthesisForMultiplication(
  FormatExpressions* theFormat
) const {
  if (theFormat == nullptr) {
    return true;
  }
  return !theFormat->flagSuppressModP;
}

std::string ElementZmodP::toStringModP() const {
  return ElementZmodP::toStringModP(this->modulus);
}

std::string ElementZmodP::toStringModP(const LargeIntegerUnsigned& modulus) {
  std::stringstream out;
  out << "mod (" << modulus.toString() << ")";
  return out.str();
}

std::string ElementZmodP::toStringPolynomial(
  const Polynomial<ElementZmodP>& input, FormatExpressions* format
) const {
  std::stringstream out;
  out << "PolynomialModP{}(" << input.toString(format) << ", " << this->modulus << ")";
  return out.str();
}

std::string ElementZmodP::toString(FormatExpressions* theFormat) const {
  bool suppressModulus = false;
  if (theFormat != nullptr) {
    suppressModulus = theFormat->flagSuppressModP;
  }
  std::stringstream out;
  if (suppressModulus) {
    out << this->value.toString();
  } else {
    out << "(" << this->value.toString() << " "
    << this->toStringModP() << ")";
  }
  return out.str();
}

bool ElementZmodP::operator*=(const Rational& other) {
  ElementZmodP otherElement;
  otherElement.modulus = this->modulus;
  if (!otherElement.assignRational(other)) {
    return false;
  }
  *this *= otherElement;
  return true;
}

ElementZmodP ElementZmodP::operator*(const Rational& other) const {
  ElementZmodP otherElt;
  otherElt.modulus = this->modulus;
  otherElt.assignRational(other);
  ElementZmodP result = *this;
  result *= otherElt;
  return result;
}

unsigned int ElementZmodP::hashFunction() const {
  if (this->value.isEqualToZero()) {
    return 0;
  }
  return this->value.hashFunction() * someRandomPrimes[0] +
  this->modulus.hashFunction() * someRandomPrimes[1];
}

void ElementZmodP::checkIamInitialized() const {
  if (this->modulus.isEqualToZero()) {
    global.fatal << "Element of Z mod p has non-initialized modulus. "
    << global.fatal;
  }
  if (this->flagDeallocated) {
    global.fatal
    << "Use after free of element of Z mod p. " << global.fatal;
  }
}

void ElementZmodP::operator=(const LargeInteger& other) {
  this->checkIamInitialized();
  this->value = other.value;
  this->value %= this->modulus;
  if (other.sign == - 1) {
    ElementZmodP mOne;
    mOne.makeMinusOne(this->modulus);
    *this *= mOne;
  }
}

bool ElementZmodP::operator>(const ElementZmodP& other) const {
  if (this->modulus > other.modulus) {
    return true;
  }
  if (other.modulus > this->modulus) {
    return false;
  }
  return this->value > other.value;
}

ElementZmodP ElementZmodP::zeroStatic() {
  ElementZmodP result;
  result.modulus = 1;
  return result;
}

ElementZmodP ElementZmodP::zero() const {
  ElementZmodP result;
  result.modulus = this->modulus;
  if (this->modulus == 0) {
    global.fatal << "Extracting constant one from non-initialized element not allowed. " << global.fatal;
  }
  return result;
}

ElementZmodP ElementZmodP::one() const {
  ElementZmodP result;
  result.modulus = this->modulus;
  result.value = 1;
  if (this->modulus == 0) {
    global.fatal << "Extracting zero constant from non-initialized element not allowed. " << global.fatal;
  }
  return result;
}

void ElementZmodP::convertModuloIntegerAfterScalingToIntegral(
  const Polynomial<Rational>& input,
  Polynomial<ElementZmodP>& output,
  const LargeIntegerUnsigned& newModulo
) {
  Polynomial<Rational> rescaled;
  rescaled = input;
  rescaled.scaleNormalizeLeadingMonomial(&MonomialP::orderDefault());
  output.setExpectedSize(input.size());
  ElementZmodP theCF;
  theCF.modulus = newModulo;
  output.makeZero();
  for (int i = 0; i < input.size(); i ++) {
    theCF = input.coefficients[i];
    output.addMonomial(input[i], theCF);
  }
}

void ElementZmodP::operator=(const ElementZmodP& other) {
  this->modulus = other.modulus;
  this->value = other.value;
}

void ElementZmodP::operator=(const LargeIntegerUnsigned& other) {
  this->checkIamInitialized();
  this->value = other;
  this->value %= this->modulus;
}

bool ElementZmodP::invert() {
  ElementZmodP copy = *this;
  this->makeOne(this->modulus);
  bool result = (*this /= copy);
  return result;
}

void ElementZmodP::makeZero() {
  this->value = 0;
  this->checkIamInitialized();
}

ElementZmodP ElementZmodP::getNumerator() const {
  return *this;
}

ElementZmodP ElementZmodP::getDenominator() const {
  return this->one();
}

void ElementZmodP::makeOne(const LargeIntegerUnsigned& newModulo) {
  this->modulus = newModulo;
  this->value = 1;
}

void ElementZmodP::makeMinusOne(const LargeIntegerUnsigned& newModulo) {
  this->modulus = newModulo;
  this->value = newModulo;
  this->value --;
}

void ElementZmodP::checkEqualModuli(const ElementZmodP& other) {
  if (this->modulus != other.modulus) {
    global.fatal << "This is a programming error: attempting to make an operation "
    << "with two elemetns of Z mod P with different moduli, "
    << this->modulus.toString() << " and "
    << other.modulus.toString() << ". " << global.fatal;
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
  scale.invert();
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
  this->value *= other.value;
   if (other.isNegative()) {
     this->value *= this->modulus - 1;
   }
   this->value %= this->modulus;
 }

bool ElementZmodP::operator+=(const Rational& other) {
  ElementZmodP otherElt;
  otherElt.modulus = this->modulus;
  if (!otherElt.assignRational(other)) {
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
  this->checkEqualModuli(other);
  this->value += other.value;
  this->value %= this->modulus;
}

void ElementZmodP::operator-=(const ElementZmodP& other) {
  if (this == &other) {
    ElementZmodP other = *this;
    *this += other;
    return;
  }
  this->checkEqualModuli(other);
  ElementZmodP otherTimesMinusOne = other;
  otherTimesMinusOne *= -1;
  *this += otherTimesMinusOne;
}

void ElementZmodP::operator=(const Rational& other) {
  bool tempB = this->assignRational(other);
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
  otherElt.modulus = this->modulus;
  otherElt = other;
  (*this) -= otherElt;
}

bool ElementZmodP::assignRational(const Rational& other) {
  this->checkIamInitialized();
  *this = other.getNumerator();
  ElementZmodP denominator;
  denominator.modulus = this->modulus;
  denominator.value = other.getDenominator();
  if (denominator.isEqualToZero()) {
    return false;
  }
  return (*this /= denominator);
}

bool ElementZmodP::operator/=(const LargeInteger& other) {
  ElementZmodP divisor;
  divisor.modulus = this->modulus;
  if (!divisor.assignRational(Rational(other))) {
    return false;
  }
  bool result = ((*this) /= divisor);
  return result;
}

bool ElementZmodP::operator/=(const ElementZmodP& other) {
  this->checkIamInitialized();
  this->checkEqualModuli(other);
  if (other.isEqualToZero()) {
    global.fatal << "Division by zero in ElementZmodP." << global.fatal;
  }
  LargeInteger theInverted, otherValue, theMod;
  theMod = this->modulus;
  otherValue = other.value;
  if (!MathRoutines::invertXModN(otherValue, theMod, theInverted)) {
    return false;
  }
  *this *= theInverted;
  return true;
}
