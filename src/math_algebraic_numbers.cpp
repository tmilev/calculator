#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "math_extra_algebraic_numbers.h"
#include "math_subsets_selections.h"
#include "math_general_implementation.h"

const int ElementZmodP::maximumModulusForUserFacingPolynomialDivision = 10000;

std::string MonomialVector::toString(FormatExpressions* format) const {
  if (format != nullptr) {
    if (
      this->monomialIndex < format->vectorSpaceEiBasisNames.size &&
      this->monomialIndex >= 0
    ) {
      return format->vectorSpaceEiBasisNames[this->monomialIndex];
    }
  }
  std::stringstream out;
  out << "e_{" << this->monomialIndex + 1 << "}";
  return out.str();
}

PolynomialSolverWithQuadraticRadicalsUnivariate::
PolynomialSolverWithQuadraticRadicalsUnivariate(
  AlgebraicClosureRationals& inputField
) {
  this->ownerField = &inputField;
}

bool PolynomialSolverWithQuadraticRadicalsUnivariate::
solvePolynomialWithRadicals(
  const Polynomial<Rational>& input,
  std::stringstream* commentsOnFailure
) {
  PolynomialFactorizationUnivariate<Rational> factorization;
  PolynomialFactorizationFiniteFields algorithm;
  if (
    !factorization.factor(input, algorithm, nullptr, commentsOnFailure)
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to factor the characteristic polynomial.";
    }
    return false;
  }
  this->solutions.clear();
  for (int i = 0; i < factorization.reduced.size; i ++) {
    Polynomial<Rational>& factor = factorization.reduced[i];
    if (factor.totalDegreeInt() > 2) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Characteristic polynomial has factor of degree larger than 2.";
      }
      return false;
    }
    if (factor.isLinear()) {
      this->solutions.addOnTop(
        - factor.getConstantTerm() /
        factor.getCoefficientInFrontOfLinearTermVariableIndex(0)
      );
    } else {
      Rational a = factor.getCoefficientOf(MonomialPolynomial(0, 2));
      Rational b = factor.getCoefficientOf(MonomialPolynomial(0, 1));
      Rational c = factor.getCoefficientOf(MonomialPolynomial(0, 0));
      Rational discriminant = b * b - 4 * a * c;
      AlgebraicNumber squareRootDiscriminant;
      if (
        !squareRootDiscriminant.assignRationalQuadraticRadical(
          discriminant, *this->ownerField, commentsOnFailure
        )
      ) {
        return false;
      }
      AlgebraicNumber leftRoot = (squareRootDiscriminant *(- 1) - b) / (2* a);
      AlgebraicNumber rightRoot = (squareRootDiscriminant - b) / (2* a);
      this->solutions.addOnTop(leftRoot);
      this->solutions.addOnTop(rightRoot);
    }
  }
  return true;
}

int AlgebraicClosureRationals::getDimensionOverRationals() const {
  return this->latestBasis.size;
}

bool AlgebraicClosureRationals::checkConsistency() const {
  return true;
}

void AlgebraicClosureRationals::getMultiplicativeOperatorFromRadicalSelection(
  const Selection& selection, MatrixTensor<Rational>& output
) {
  STACK_TRACE(
    "AlgebraicClosureRationals::getMultiplicativeOperatorFromRadicalSelection"
  );
  output.makeZero();
  Selection vectorActedOnSel, resultVectorSel;
  vectorActedOnSel.initialize(this->quadraticRadicals.size);
  resultVectorSel.initialize(this->quadraticRadicals.size);
  Rational coefficient;
  MonomialMatrix tempM;
  do {
    coefficient = 1;
    for (int i = 0; i < this->quadraticRadicals.size; i ++) {
      if (vectorActedOnSel.selected[i] && selection.selected[i]) {
        resultVectorSel.selected[i] = false;
        coefficient *= this->quadraticRadicals[i];
      } else if (
        !vectorActedOnSel.selected[i] && !selection.selected[i]
      ) {
        resultVectorSel.selected[i] = false;
      } else {
        resultVectorSel.selected[i] = true;
      }
    }
    resultVectorSel.computeIndicesFromSelection();
    tempM.makeEij(
      this->getIndexFromRadicalSelection(resultVectorSel),
      this->getIndexFromRadicalSelection(vectorActedOnSel)
    );
    output.addMonomial(tempM, coefficient);
  } while (vectorActedOnSel.incrementReturnFalseIfPastLast());
}

void AlgebraicClosureRationals::computeDisplayStringsFromRadicals() {
  STACK_TRACE("AlgebraicClosureRationals::computeDisplayStringsFromRadicals");
  if (!this->flagIsQuadraticRadicalExtensionRationals) {
    return;
  }
  this->displayNamesBasisElements.setSize(this->latestBasis.size);
  Selection selection;
  selection.initialize(this->quadraticRadicals.size);
  do {
    std::stringstream out;
    LargeInteger radical = 1;
    for (int i = 0; i < selection.cardinalitySelection; i ++) {
      radical *= this->quadraticRadicals[selection.elements[i]];
    }
    if (radical != 1) {
      out << "\\sqrt{" << radical.toString() << "}";
    }
    this->displayNamesBasisElements[
      this->getIndexFromRadicalSelection(selection)
    ] =
    out.str();
  } while (selection.incrementReturnFalseIfPastLast());
}

bool AlgebraicClosureRationals::getRadicalSelectionFromIndex(
  int inputIndex, Selection& selection
) {
  if (!this->flagIsQuadraticRadicalExtensionRationals) {
    return false;
  }
  selection.initialize(this->quadraticRadicals.size);
  int counter = 0;
  while (inputIndex > 0) {
    if (inputIndex % 2 == 1) {
      selection.addSelectionAppendNewIndex(counter);
    }
    inputIndex /= 2;
    counter ++;
  }
  return true;
}

int AlgebraicClosureRationals::getIndexFromRadicalSelection(
  const Selection& selection
) {
  if (selection.numberOfElements > 30) {
    global.fatal
    << "The algebraic extension is too large "
    << "to be handled by the current data structures. "
    << global.fatal;
  }
  int result = 0;
  for (int i = selection.numberOfElements - 1; i >= 0; i --) {
    result *= 2;
    if (selection.selected[i]) {
      result += 1;
    }
  }
  return result;
}

bool AlgebraicClosureRationals::mergeRadicals(
  const List<LargeInteger>& radicals
) {
  STACK_TRACE("AlgebraicClosureRationals::mergeRadicals");
  if (!this->flagIsQuadraticRadicalExtensionRationals) {
    global.fatal
    << "AlgebraicClosureRationals::MergeTwoQuadraticRadicalExtensions "
    << "with at least one of two arguments that is "
    << "not a quadratic radical extension of the rationals. "
    << global.fatal;
  }
  // refine factors:
  HashedList<LargeInteger> radicalsNew = this->quadraticRadicals;
  radicalsNew.addOnTopNoRepetition(radicals);
  bool found = true;
  LargeIntegerUnsigned candidateGCD, leftQuotient, rightQuotient;
  while (found) {
    found = false;
    for (int i = 0; i < radicalsNew.size; i ++) {
      for (int j = i + 1; j < radicalsNew.size; j ++) {
        if (radicalsNew[i] == - 1 || radicalsNew[j] == - 1) {
          continue;
        }
        LargeIntegerUnsigned::greatestCommonDivisor(
          radicalsNew[i].value, radicalsNew[j].value, candidateGCD
        );
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
    global.comments
    << "Computing with fields whose dimension over the "
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
  smallerFieldSel.initialize(this->quadraticRadicals.size);
  do {
    largerFieldSel.initialize(radicalsNew.size);
    for (int j = 0; j < this->quadraticRadicals.size; j ++) {
      if (smallerFieldSel.selected[j]) {
        if (this->quadraticRadicals[j] == - 1) {
          largerFieldSel.addSelectionAppendNewIndex(
            radicalsNew.getIndex(- 1)
          );
          continue;
        }
        for (int k = 0; k < radicalsNew.size; k ++) {
          if (
            this->quadraticRadicals[j] % radicalsNew[k] == 0 &&
            radicalsNew[k] != - 1
          ) {
            largerFieldSel.addSelectionAppendNewIndex(k);
          }
        }
      }
    }
    currentInjection.addMonomial(
      MonomialMatrix(
        this->getIndexFromRadicalSelection(largerFieldSel),
        this->getIndexFromRadicalSelection(smallerFieldSel)
      ),
      1
    );
  } while (smallerFieldSel.incrementReturnFalseIfPastLast());
  this->quadraticRadicals = radicalsNew;
  largerFieldSel.initialize(radicalsNew.size);
  do {
    this->getMultiplicativeOperatorFromRadicalSelection(
      largerFieldSel,
      this->latestBasis[
        this->getIndexFromRadicalSelection(largerFieldSel)
      ]
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
  STACK_TRACE("AlgebraicClosureRationals::injectOldBases");
  if (injectionNullForIdentity == nullptr) {
    return;
  }
  for (int j = 0; j < this->basisInjections.size; j ++) {
    for (int i = 0; i < this->basisInjections[j].size; i ++) {
      injectionNullForIdentity->actOnVectorColumn(
        this->basisInjections[j][i]
      );
    }
  }
}

void AlgebraicClosureRationals::appendAdditiveEiBasis() {
  STACK_TRACE("AlgebraicClosureRationals::appendAdditiveEiBasis");
  this->basisInjections.setSize(this->basisInjections.size + 1);
  this->basisInjections.lastObject()->setSize(this->latestBasis.size);
  for (int i = 0; i < this->latestBasis.size; i ++) {
    (*this->basisInjections.lastObject())[i].makeEi(i);
  }
}

bool AlgebraicClosureRationals::chooseGeneratingElement(
  int attemptsLimitZeroForNone, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("AlgebraicClosureRationals::chooseGeneratingElement");
  SelectionPositiveIntegers selection;
  int dimensionOverRationals = this->latestBasis.size;
  selection.initialize(dimensionOverRationals);
  this->generatingElementPowersBasis.setSize(0);
  Vector<Rational> currentVect;
  this->generatingElement.owner = this;
  this->generatingElement.basisIndex = this->basisInjections.size - 1;
  int attemptsSoFar = 0;
  if (this->basisInjections.size > 1) {
    int indexToSkipFirst =
    this->basisInjections[this->basisInjections.size - 2].size - 1;
    if (indexToSkipFirst < dimensionOverRationals) {
      selection.integers.makeEi(dimensionOverRationals, indexToSkipFirst);
    }
  }
  for (
    selection.incrementReturnFalseIfPastLast();; selection.incrementReturnFalseIfPastLast()
  ) {
    attemptsSoFar ++;
    if (
      attemptsLimitZeroForNone > 0 && attemptsSoFar > attemptsLimitZeroForNone
    ) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Choice of generating element is taking too long: made "
        << attemptsSoFar
        << " attempts so far, limit: "
        << attemptsLimitZeroForNone
        << ". ";
      }
      global.comments
      << "Choice of generating element is taking too long: made "
      << attemptsSoFar
      << " attempts so far, limit: "
      << attemptsLimitZeroForNone
      << ". ";
      return false;
    }
    this->generatingElement.element.makeZero();
    for (int i = 0; i < selection.integers.size; i ++) {
      MonomialVector tempV;
      tempV.makeEi(i);
      this->generatingElement.element.addMonomial(
        tempV, selection.integers[i]
      );
    }
    this->getMultiplicationBy(
      this->generatingElement, this->generatingElementTensorForm
    );
    this->generatingElementTensorForm.getMatrix(
      this->generatingElementMatrixForm, dimensionOverRationals
    );
    this->generatingElementPowersBasis.setSize(0);
    currentVect.makeEi(dimensionOverRationals, 0);
    this->generatingElementPowersBasis.addOnTop(currentVect);
    do {
      this->generatingElementMatrixForm.actOnVectorColumn(
        currentVect, Rational::zero()
      );
      this->generatingElementPowersBasis.addOnTop(currentVect);
      if (
        this->generatingElementPowersBasis.size >
        this->generatingElementPowersBasis.getRankElementSpan()
      ) {
        this->generatingElementPowersBasis.setSize(
          this->generatingElementPowersBasis.size - 1
        );
        break;
      }
    } while (true);
    if (this->generatingElementPowersBasis.size == dimensionOverRationals) {
      break;
    }
  }
  return true;
}

void AlgebraicClosureRationals::contractBasesIfRedundant(
  AlgebraicClosureRationals& previousCopy,
  AlgebraicNumber* outputImageGenerator
) {
  STACK_TRACE("AlgebraicClosureRationals::contractBasesIfRedundant");
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
  reverseMap.assignVectorsToColumns(
    this->basisInjections[this->basisInjections.size - 3]
  );
  reverseMap.invert();
  reverseMap.actOnVectorColumn(outputImageGenerator->element);
  outputImageGenerator->basisIndex = this->basisInjections.size - 3;
  *this = previousCopy;
}

bool AlgebraicClosureRationals::reduceMe(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("AlgebraicClosureRationals::reduceMe");
  if (!this->chooseGeneratingElement(1000, commentsOnFailure)) {
    return false;
  }
  if (this->flagIsQuadraticRadicalExtensionRationals) {
    return true;
  }
  Polynomial<Rational> minimalPolynomial, smallestFactor;
  minimalPolynomial.assignMinimalPolynomial(
    this->generatingElementMatrixForm
  );
  int dimension = this->latestBasis.size;
  PolynomialFactorizationUnivariate<Rational> factorization;
  PolynomialFactorizationFiniteFields algorithm;
  bool mustBeTrue =
  factorization.factor(minimalPolynomial, algorithm, nullptr, nullptr);
  if (!mustBeTrue) {
    global.fatal
    << "Failed to factor polynomial "
    << minimalPolynomial.toString()
    << ". "
    << global.fatal;
  }
  smallestFactor = factorization.reduced[0];
  if (smallestFactor.totalDegreeInt() == dimension) {
    return true;
  }
  MatrixTensor<Rational> generatorPowers, generatorPowersInverse;
  generatorPowers.assignVectorsToColumns(this->generatingElementPowersBasis);
  generatorPowersInverse = generatorPowers;
  generatorPowersInverse.invert();
  Polynomial<Rational> zToTheNth, remainderAfterReduction, tempP;
  MatrixTensor<Rational> projectionGeneratorCoordinates;
  int smallestFactorDegree = - 1;
  if (
    !smallestFactor.totalDegree().isSmallInteger(&smallestFactorDegree)
  ) {
    global.fatal
    << smallestFactor.toString()
    << " has non-integral exponent, which should be "
    << "impossible in the current context. "
    << global.fatal;
  }
  if (smallestFactorDegree == 1) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Your minimal polynomial has a linear factor: "
      << factorization.toStringResult();
    }
    return false;
  }
  projectionGeneratorCoordinates.makeZero();
  for (int i = 0; i < smallestFactorDegree; i ++) {
    projectionGeneratorCoordinates.addMonomial(MonomialMatrix(i, i), 1);
  }
  for (int i = smallestFactorDegree; i < dimension; i ++) {
    zToTheNth.makeMonomial(0, i, 1);
    zToTheNth.divideBy(
      smallestFactor,
      tempP,
      remainderAfterReduction,
      &MonomialPolynomial::orderDefault()
    );
    for (int j = 0; j < remainderAfterReduction.size(); j ++) {
      int index = - 1;
      remainderAfterReduction[j](0).isSmallInteger(&index);
      projectionGeneratorCoordinates.addMonomial(
        MonomialMatrix(index, i),
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
    Rational coefficientLastColumn =
    - smallestFactor.getCoefficientOf(MonomialPolynomial(0, i));
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

bool AlgebraicClosureRationals::checkElement(const AlgebraicNumber& input)
const {
  input.checkConsistency();
  if (input.owner != nullptr && input.owner != this) {
    global.fatal
    << "Element has a different algebraic closure owner."
    << global.fatal;
  }
  return true;
}

void AlgebraicClosureRationals::getAdditionTo(
  const AlgebraicNumber& input, VectorSparse<Rational>& output
) {
  STACK_TRACE("AlgebraicClosureRationals::getAdditionTo");
  if (&output == &input.element) {
    AlgebraicNumber copy = input;
    this->getAdditionTo(copy, output);
    return;
  }
  this->checkElement(input);
  if (input.owner == nullptr) {
    if (input.element.size() > 0) {
      output.makeEi(0, input.element.coefficients[0]);
    }
    return;
  }
  if (
    input.basisIndex < 0 || input.basisIndex >= this->basisInjections.size
  ) {
    global.fatal
    << "Element has out-of-range basis index "
    << input.basisIndex
    << ". "
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
    int currentIndex = input.element[i].monomialIndex;
    if (
      currentIndex < 0 ||
      currentIndex >= this->basisInjections[input.basisIndex].size
    ) {
      global.fatal
      << "I am getting basis index "
      << input.basisIndex
      << " with current index "
      << currentIndex
      << ". A printout of the algebraic closure follows. "
      << this->toString()
      << global.fatal;
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
  STACK_TRACE("AlgebraicClosureRationals::getMultiplicationBy");
  output.makeZero();
  VectorSparse<Rational> inputAdditiveForm;
  this->getAdditionTo(input, inputAdditiveForm);
  MatrixTensor<Rational> currentMat;
  for (int i = 0; i < inputAdditiveForm.size(); i ++) {
    if (
      inputAdditiveForm[i].monomialIndex < 0 ||
      inputAdditiveForm[i].monomialIndex >= this->latestBasis.size
    ) {
      global.fatal
      << "Element "
      << input.toString()
      << " has bad index, namely, "
      << inputAdditiveForm[i].monomialIndex
      << ". The algebraic closure is: "
      << this->toString()
      << ". "
      << global.fatal;
    }
    currentMat = this->latestBasis[inputAdditiveForm[i].monomialIndex];
    currentMat *= inputAdditiveForm.coefficients[i];
    output += currentMat;
  }
}

bool AlgebraicNumber::assignCosRationalTimesPi(
  const Rational& input, AlgebraicClosureRationals& inputOwner
) {
  STACK_TRACE("AlgebraicNumber::assignCosRationalTimesPi");
  Rational fractionalPart = input;
  fractionalPart.assignFractionalValue();
  Rational halfIntegerPart = input * 2;
  halfIntegerPart.assignFloor();
  LargeInteger halfIntegerPartTimesTwo;
  if (!halfIntegerPart.isInteger(&halfIntegerPartTimesTwo)) {
    global.fatal
    << "something went wrong: floor function returns non-integer"
    << global.fatal;
  }
  halfIntegerPart /= 2;
  Rational halfFractionalPart = input - halfIntegerPart;
  Rational half(1, 2);
  halfIntegerPartTimesTwo %= 4;
  int sign = 1;
  if (halfIntegerPartTimesTwo == 1) {
    sign = - 1;
    halfFractionalPart = half - halfFractionalPart;
  }
  if (halfIntegerPartTimesTwo == 2) {
    sign = - 1;
  }
  if (halfIntegerPartTimesTwo == 3) {
    halfFractionalPart = half - halfFractionalPart;
  }
  if (halfFractionalPart == Rational(1, 12)) {
    AlgebraicNumber sqrt6, sqrt2;
    sqrt6.assignRationalQuadraticRadical(6, inputOwner, nullptr);
    sqrt2.assignRationalQuadraticRadical(2, inputOwner, nullptr);
    *this = sqrt6 + sqrt2;
    *this /= 4 * sign;
    return true;
  }
  if (halfFractionalPart == Rational(5, 12)) {
    AlgebraicNumber sqrt6, sqrt2;
    sqrt6.assignRationalQuadraticRadical(6, inputOwner, nullptr);
    sqrt2.assignRationalQuadraticRadical(2, inputOwner, nullptr);
    *this = sqrt6 - sqrt2;
    *this /= 4 * sign;
    return true;
  }
  if (halfFractionalPart == Rational(1, 6)) {
    this->assignRationalQuadraticRadical(3, inputOwner, nullptr);
    *this /= 2;
    *this *= sign;
    return true;
  }
  if (halfFractionalPart == Rational(1, 4)) {
    this->assignRationalQuadraticRadical(2, inputOwner, nullptr);
    *this /= 2;
    *this *= sign;
    return true;
  }
  if (halfFractionalPart == Rational(1, 3)) {
    *this = half * sign;
    return true;
  }
  if (halfFractionalPart == half) {
    *this = 0;
    return true;
  }
  if (halfFractionalPart == 0) {
    *this = sign;
    return true;
  }
  return false;
}

unsigned int AlgebraicNumber::hashFunction() const {
  // WARNING. Algebraic numbers, as they are recorded in memory at the moment,
  // do not have unique reprentations in memory.
  // 1. Computing a hash function can be done by picking the hash
  // function of the minimal polynomial over the rationals.
  // However, such computations
  // appear to be too heavy, and will make sense only if we need to deal
  // with large sets of algebraic numbers without repetition.
  // 2. Currently, we choose to compute the hash function by computing the
  // rational part of the number, which should be unique.
  return this->element.getCoefficientOf(MonomialVector(0)).hashFunction();
}

bool AlgebraicNumber::operator==(const Rational& other) const {
  if (this->element.isEqualToZero()) {
    return other == 0;
  }
  if (this->element.size() != 1) {
    return false;
  }
  if (this->element[0].monomialIndex != 0) {
    return false;
  }
  return this->element.coefficients[0] == other;
}

AlgebraicNumber AlgebraicNumber::getNumerator() const {
  Rational value;
  AlgebraicNumber result;
  if (this->isRational(&value)) {
    result = value.getNumerator();
    return result;
  }
  result = *this;
  return result;
}

AlgebraicNumber AlgebraicNumber::getDenominator() const {
  Rational value;
  AlgebraicNumber result;
  if (this->isRational(&value)) {
    result = value.getDenominator();
    return result;
  }
  result.assignRational(1, this->owner);
  return result;
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

bool AlgebraicNumber::
needsParenthesisForMultiplicationWhenSittingOnTheRightMost() const {
  if (this->owner == nullptr) {
    Rational rationalValue;
    if (this->isRational(&rationalValue)) {
      return
      rationalValue.needsParenthesisForMultiplicationWhenSittingOnTheRightMost(
      );
    }
    return false;
  }
  VectorSparse<Rational> additiveForm;
  this->owner->getAdditionTo(*this, additiveForm);
  if (additiveForm.size() != 1) {
    return true;
  }
  return
  additiveForm.coefficients[0].
  needsParenthesisForMultiplicationWhenSittingOnTheRightMost();
}

bool AlgebraicNumber::checkNonZeroOwner() const {
  if (this->owner == nullptr) {
    global.fatal
    << "Algebraic number with "
    << "non-initialized owner not permitted in the current context."
    << global.fatal;
  }
  return true;
}

bool AlgebraicNumber::constructFromMinimalPolynomial(
  const Polynomial<AlgebraicNumber>& polynomial,
  AlgebraicClosureRationals& inputOwner,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("AlgebraicNumber::constructFromMinimalPolynomial");
  return
  inputOwner.adjoinRootMinimalPolynomial(
    polynomial, *this, commentsOnFailure
  );
}

void AlgebraicClosureRationals::reset() {
  this->flagIsQuadraticRadicalExtensionRationals = true;
  this->latestBasis.setSize(1);
  this->latestBasis[0].makeIdentity(1);
  this->generatingElementPowersBasis.setSize(1);
  this->generatingElementPowersBasis[0].makeEi(1, 0);
  this->basisInjections.setSize(1);
  this->basisInjections[0].setSize(1);
  this->basisInjections[0][0].makeEi(0);
  this->quadraticRadicals.clear();
  this->displayNamesBasisElements.setSize(1);
  this->displayNamesBasisElements[0] = "";
  this->generatingElementTensorForm.makeIdentity(1);
  this->generatingElementMatrixForm.makeIdentityMatrix(
    1, Rational::one(), Rational::zero()
  );
  this->generatingElement.owner = this;
  this->generatingElement.element.makeEi(0);
}

bool AlgebraicClosureRationals::
adjoinRootQuadraticPolynomialToQuadraticRadicalExtension(
  const Polynomial<AlgebraicNumber>& polynomial,
  AlgebraicNumber& outputRoot,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE(
    "AlgebraicClosureRationals::adjoinRootQuadraticPolynomialToQuadraticRadicalExtension"
  );
  if (
    polynomial.totalDegree() != 2 ||
    !this->flagIsQuadraticRadicalExtensionRationals
  ) {
    return false;
  }
  Polynomial<AlgebraicNumber> algebraicNumberPolynomial;
  this->convertPolynomialOneVariableToPolynomialFirstVariable(
    polynomial, algebraicNumberPolynomial
  );
  Polynomial<Rational> minimialPolynomial;
  minimialPolynomial.makeZero();
  Rational currentCF, linearTermCoefficientFDividedByTwo, constantTermShifted;
  for (int i = 0; i < algebraicNumberPolynomial.size(); i ++) {
    if (
      !algebraicNumberPolynomial.coefficients[i].isRational(&currentCF)
    ) {
      return false;
    } else {
      minimialPolynomial.addMonomial(
        algebraicNumberPolynomial[i], currentCF
      );
    }
  }
  List<MonomialPolynomial>::Comparator* monomialOrder =
  &MonomialPolynomial::orderDefault();
  minimialPolynomial /=
  minimialPolynomial.getLeadingCoefficient(monomialOrder);
  minimialPolynomial.getCoefficientInFrontOfLinearTermVariableIndex(
    0, linearTermCoefficientFDividedByTwo
  );
  linearTermCoefficientFDividedByTwo /= 2;
  minimialPolynomial.getConstantTerm(constantTermShifted);
  constantTermShifted -=
  linearTermCoefficientFDividedByTwo * linearTermCoefficientFDividedByTwo;
  constantTermShifted *= - 1;
  if (
    !outputRoot.assignRationalQuadraticRadical(
      constantTermShifted, *this, commentsOnFailure
    )
  ) {
    return false;
  }
  outputRoot -= linearTermCoefficientFDividedByTwo;
  // check our work:
  PolynomialSubstitution<AlgebraicNumber> checkSub;
  checkSub.setSize(1);
  checkSub[0].makeConstant(outputRoot);
  algebraicNumberPolynomial.substitute(checkSub, this->one());
  if (!algebraicNumberPolynomial.isEqualToZero()) {
    global.fatal
    << "The number z = "
    << outputRoot.toString()
    <<
    " was just adjoined to a quadratic radical extension of the rationals; z "
    << "was given by requesting that it has minimial polynomial "
    << algebraicNumberPolynomial.toString()
    << ", however, substituting z back in to the minimal polynomial "
    << "does not yield zero, rather yields "
    << algebraicNumberPolynomial.toString()
    << ". "
    << global.fatal;
  }
  return true;
}

void AlgebraicClosureRationals::
convertPolynomialOneVariableToPolynomialFirstVariable(
  const Polynomial<AlgebraicNumber>& input,
  Polynomial<AlgebraicNumber>& output
) {
  STACK_TRACE(
    "AlgebraicClosureRationals::"
    "convertPolynomialDependingOneVariableToPolynomialDependingOnFirstVariableNoFail"
  );
  int indexVariable = - 1;
  if (!input.isOneVariableNonConstantPolynomial(&indexVariable)) {
    global.fatal
    << "I am being asked convert to a one-variable polynomial a polynomial "
    << "depending on more than one variables. "
    << "The input polynomial is: "
    << input.toString()
    << global.fatal;
  }
  PolynomialSubstitution<AlgebraicNumber> substitution;
  substitution.makeIdentitySubstitution(indexVariable + 1);
  substitution[indexVariable].makeMonomial(0, 1, 1);
  output = input;
  output.substitute(substitution, this->one());
}

bool AlgebraicClosureRationals::adjoinRootMinimalPolynomial(
  const Polynomial<AlgebraicNumber>& polynomial,
  AlgebraicNumber& outputRoot,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("AlgebraicClosureRationals::adjoinRootMinimalPolynomial");
  if (
    this->adjoinRootQuadraticPolynomialToQuadraticRadicalExtension(
      polynomial, outputRoot, commentsOnFailure
    )
  ) {
    return true;
  }
  Polynomial<AlgebraicNumber> minimialPolynomial;
  this->convertPolynomialOneVariableToPolynomialFirstVariable(
    polynomial, minimialPolynomial
  );
  List<MonomialPolynomial>::Comparator* monomialOrder =
  &MonomialPolynomial::orderDefault();
  AlgebraicNumber leadingCoefficient =
  minimialPolynomial.getLeadingCoefficient(monomialOrder);
  minimialPolynomial /= leadingCoefficient;
  AlgebraicClosureRationals backUpCopy;
  backUpCopy = *this;
  MatrixTensor<Rational> generatorMatrix;
  int degreeMinimialPolynomial = minimialPolynomial.totalDegreeInt();
  int startingDimension = this->latestBasis.size;
  if (
    degreeMinimialPolynomial * startingDimension > 10000 ||
    startingDimension > 10000 ||
    degreeMinimialPolynomial > 10000
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "<hr>Adjoining root of minimial polynomial failed: "
      << "the current field extension dimension over the rationals is "
      << startingDimension
      << ", the degree of the minimial polynomial is "
      << degreeMinimialPolynomial
      << ", yielding expected final dimension "
      << startingDimension
      << "*"
      << degreeMinimialPolynomial
      << " = "
      << startingDimension * degreeMinimialPolynomial
      << " over the rationals. The calculator is hard-coded "
      << "to accept dimension over the rationals no larger than 10000 "
      << "- multiplication in such a field corresponds to a "
      << "10000x10000 matrix "
      << "(100 000 000 entries, expected in general to be dense). "
      << "If you do indeed want to carry out "
      << "such large computations, you need to "
      << "compile the calculator on your own, modifying file "
      << __FILE__
      << ", line "
      << __LINE__
      << ".";
    }
    return false;
  }
  if (degreeMinimialPolynomial <= 1) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "The degree of the minimal polynomial must be at least two.";
    }
    return false;
  }
  generatorMatrix.makeZero();
  for (int i = 0; i < degreeMinimialPolynomial - 1; i ++) {
    for (int j = 0; j < startingDimension; j ++) {
      generatorMatrix.addMonomial(
        MonomialMatrix((i + 1) * startingDimension + j, i * startingDimension +
          j
        ),
        1
      );
    }
  }
  Polynomial<AlgebraicNumber> minusMinimalPolynomialMinusMaximalMonomial =
  minimialPolynomial;
  MonomialPolynomial leadingMonomial;
  AlgebraicNumber leadingCoefficientModified;
  minusMinimalPolynomialMinusMaximalMonomial.getIndexLeadingMonomial(
    &leadingMonomial, &leadingCoefficientModified, monomialOrder
  );
  minusMinimalPolynomialMinusMaximalMonomial.subtractMonomial(
    leadingMonomial, leadingCoefficientModified
  );
  minusMinimalPolynomialMinusMaximalMonomial /= leadingCoefficientModified;
  minusMinimalPolynomialMinusMaximalMonomial *= - 1;
  MatrixTensor<Rational> currentCoefficientMatrixForm;
  for (
    int i = 0; i < minusMinimalPolynomialMinusMaximalMonomial.size(); i ++
  ) {
    AlgebraicNumber& currentCoefficient =
    minusMinimalPolynomialMinusMaximalMonomial.coefficients[i];
    const MonomialPolynomial& currentMonomial =
    minusMinimalPolynomialMinusMaximalMonomial[i];
    this->getMultiplicationBy(
      currentCoefficient, currentCoefficientMatrixForm
    );
    for (int j = 0; j < currentCoefficientMatrixForm.size(); j ++) {
      int relationRowIndex = currentCoefficientMatrixForm[j].vIndex;
      int relationColumnIndex = currentCoefficientMatrixForm[j].dualIndex;
      if (relationRowIndex == - 1 || relationColumnIndex == - 1) {
        global.fatal << "Non initialized monomial. " << global.fatal;
      }
      generatorMatrix.addMonomial(
        MonomialMatrix(
          currentMonomial.totalDegreeInt() * startingDimension +
          relationRowIndex,
          startingDimension *(degreeMinimialPolynomial - 1) +
          relationColumnIndex
        ),
        currentCoefficientMatrixForm.coefficients[j]
      );
    }
  }
  int finalDimension = degreeMinimialPolynomial * startingDimension;
  List<MatrixTensor<Rational> > finalBasis;
  finalBasis.setSize(this->latestBasis.size);
  MatrixTensor<Rational> generatorMatrixPower;
  generatorMatrixPower.makeIdentity(degreeMinimialPolynomial);
  for (int i = 0; i < startingDimension; i ++) {
    finalBasis[i].assignTensorProduct(
      generatorMatrixPower, this->latestBasis[i]
    );
  }
  this->latestBasis = finalBasis;
  this->latestBasis.setSize(finalDimension);
  generatorMatrixPower = generatorMatrix;
  for (int i = 1; i < degreeMinimialPolynomial; i ++) {
    for (int j = 0; j < startingDimension; j ++) {
      this->latestBasis[i * startingDimension + j] = generatorMatrixPower;
      this->latestBasis[i * startingDimension + j] *= finalBasis[j];
    }
    if (i != degreeMinimialPolynomial - 1) {
      generatorMatrixPower *= generatorMatrix;
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
  PolynomialSubstitution<AlgebraicNumber> substitution;
  substitution.setSize(1);
  substitution[0].makeConstant(outputRoot);
  Polynomial<AlgebraicNumber> substitutedMinimalPolynomial;
  substitutedMinimalPolynomial = minimialPolynomial;
  substitutedMinimalPolynomial.substitute(substitution, this->one());
  if (!substitutedMinimalPolynomial.isEqualToZero()) {
    global.fatal
    << "The number z = "
    << outputRoot.toString()
    << " was just adjoined to the base field; z "
    << "was given by requesting that it has minimial polynomial "
    << minimialPolynomial.toString()
    << ", however, substituting z back in to the minimal polynomial "
    << "does not yield zero, rather yields "
    << substitutedMinimalPolynomial.toString()
    << ". "
    << "The algebraic closure printout follows. "
    << this->toStringFull()
    << global.fatal;
  }
  return true;
}

void AlgebraicNumber::invert() {
  STACK_TRACE("AlgebraicNumber::invert");
  if (this->owner == nullptr) {
    if (this->element.isEqualToZero()) {
      global.fatal << "Division by zero. " << global.fatal;
    }
    bool isGood = (this->element.size() == 1);
    if (isGood) {
      isGood = (this->element[0].monomialIndex == 0);
    }
    if (!isGood) {
      global.fatal
      << "Algebraic number has no owner, "
      << "so it must be rational, but it appears to be not. "
      << "Its element vector is: "
      << this->element.toString()
      << global.fatal;
    }
    this->element.coefficients[0].invert();
    return;
  }
  MatrixTensor<Rational> inverted;
  Matrix<Rational> matrix;
  this->owner->getMultiplicationBy(*this, inverted);
  inverted.getMatrix(matrix, this->owner->latestBasis.size);
  matrix.invert();
  inverted = matrix;
  this->element.makeEi(0);
  inverted.actOnVectorColumn(this->element);
  this->basisIndex = this->owner->basisInjections.size - 1;
}

void AlgebraicNumber::operator/=(const AlgebraicNumber& other) {
  STACK_TRACE("AlgebraicNumber::operator/=");
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
    global.fatal
    << "Two algebraic numbers have "
    << "different algebraic closures when they shouldn't. "
    << global.fatal;
    return false;
  }
  return true;
}

void AlgebraicNumber::operator-=(const AlgebraicNumber& other) {
  STACK_TRACE("AlgebraicNumber::operator=");
  this->checkCommonOwner(other);
  if (this->basisIndex == other.basisIndex) {
    this->element -= other.element;
    return;
  }
  AlgebraicClosureRationals* correctedOwner = this->owner;
  if (correctedOwner == nullptr) {
    correctedOwner = other.owner;
  }
  if (correctedOwner == nullptr && this->basisIndex != other.basisIndex) {
    global.fatal
    << "Algebraic numbers "
    << "with zero owners but different basis indices. "
    << global.fatal;
  }
  VectorSparse<Rational> additiveFormOther;
  correctedOwner->getAdditionTo(*this, this->element);
  correctedOwner->getAdditionTo(other, additiveFormOther);
  this->owner = correctedOwner;
  this->basisIndex = correctedOwner->basisInjections.size - 1;
  this->element -= additiveFormOther;
}

void AlgebraicNumber::operator+=(const AlgebraicNumber& other) {
  STACK_TRACE("AlgebraicNumber::operator+=");
  this->checkConsistency();
  other.checkConsistency();
  if (this == &other) {
    AlgebraicNumber otherCopy = other;
    *this += otherCopy;
    this->checkConsistency();
    return;
  }
  this->checkCommonOwner(other);
  AlgebraicClosureRationals* correctedOwner = this->owner;
  if (correctedOwner == nullptr) {
    correctedOwner = other.owner;
  }
  if (correctedOwner == nullptr && this->basisIndex != other.basisIndex) {
    global.fatal
    << "Algebraic numbers: "
    << this->toString()
    << " and "
    << other.toString()
    << " have with zero owners but different basis indices. "
    << global.fatal;
  }
  if (this->basisIndex == other.basisIndex) {
    this->owner = correctedOwner;
    this->checkConsistency();
    other.checkConsistency();
    this->element += other.element;
    this->checkConsistency();
    return;
  }
  VectorSparse<Rational> additiveFormOther;
  correctedOwner->getAdditionTo(*this, this->element);
  correctedOwner->getAdditionTo(other, additiveFormOther);
  this->owner = correctedOwner;
  this->basisIndex = correctedOwner->basisInjections.size - 1;
  this->element += additiveFormOther;
  this->checkConsistency();
}

bool AlgebraicNumber::checkConsistency() const {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of AlgebraicNumber. " << global.fatal;
  }
  if (this->owner != nullptr) {
    if (
      this->basisIndex < 0 ||
      this->basisIndex >= this->owner->basisInjections.size
    ) {
      global.fatal
      << "Element has out-of-range basis index "
      << this->basisIndex
      << ". "
      << global.fatal;
    }
  }
  if (this->owner == nullptr) {
    if (!this->isRational()) {
      for (int i = 0; i < this->element.size(); i ++) {
        global.comments
        << "<br>index: "
        << this->element[i].monomialIndex
        << ", coefficient: "
        << this->element.coefficients[i];
      }
      global.fatal
      << "Detected non-rational algebraic number with zero owner. "
      << global.fatal;
    }
    if (this->basisIndex != 0) {
      global.fatal
      << "Algebraic number with non-zero basis and zero owner. "
      << global.fatal;
    }
  }
  return true;
}

void AlgebraicNumber::operator*=(const AlgebraicNumber& other) {
  STACK_TRACE("AlgebraicNumber::operator*=");
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
    Rational copy = this->element.getCoefficientOf(MonomialVector(0));
    *this = other;
    *this *= copy;
    return;
  }
  bool doReport = (
    this->element.size() * other.element.size() > 100
  );
  ProgressReport report;
  if (doReport) {
    std::stringstream reportStream;
    reportStream
    << "Large multiplication:<br>"
    << this->toString()
    << "\n<br>by<br>\n"
    << other.toString()
    << "\n<br>...";
    report.report(reportStream.str());
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
  if (doReport) {
    report.report("Large multiplication done.");
  }
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
  const Polynomial<Rational>& polynomial,
  AlgebraicClosureRationals& inputOwner,
  std::stringstream* commentsOnFailure
) {
  Polynomial<AlgebraicNumber> polyConverted;
  polyConverted = polynomial;
  return
  this->constructFromMinimalPolynomial(
    polyConverted, inputOwner, commentsOnFailure
  );
}

AlgebraicNumber AlgebraicNumber::oneStatic() {
  AlgebraicNumber result;
  result.assignRational(1, nullptr);
  return result;
}

AlgebraicNumber AlgebraicNumber::zeroStatic() {
  AlgebraicNumber result;
  result.assignRational(0, nullptr);
  return result;
}

AlgebraicNumber AlgebraicNumber::zero() const {
  AlgebraicNumber result;
  result.assignRational(0, this->owner);
  return result;
}

AlgebraicNumber AlgebraicNumber::one() const {
  AlgebraicNumber result;
  result.assignRational(1, this->owner);
  return result;
}

void AlgebraicNumber::makeZero() {
  this->assignRational(0, this->owner);
}

void AlgebraicNumber::assignRational(
  const Rational& input, AlgebraicClosureRationals* inputOwner
) {
  this->basisIndex = 0;
  this->owner = inputOwner;
  this->element.makeEi(0, input);
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
  STACK_TRACE("AlgebraicNumber::evaluatesToDouble");
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
    global.fatal
    << "Owner is zero but algebraic number is not rational. "
    << global.fatal;
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
    this->owner->getRadicalSelectionFromIndex(
      this->element[i].monomialIndex, currentRadicalSelection
    );
    if (outputWhichDouble != nullptr) {
      currentMultiplicand = this->element.coefficients[i].getDoubleValue();
    }
    for (int j = 0; j < currentRadicalSelection.cardinalitySelection; j ++) {
      if (
        this->owner->quadraticRadicals[
          currentRadicalSelection.elements[j]
        ] <
        0
      ) {
        return false;
      } else {
        if (outputWhichDouble != nullptr) {
          currentMultiplicand *=
          FloatingPoint::sqrtFloating(
            this->owner->quadraticRadicals[
              currentRadicalSelection.elements[j]
            ].getDoubleValue()
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
  STACK_TRACE("AlgebraicNumber::assignRationalQuadraticRadical");
  this->checkConsistency();
  if (input == 0) {
    return false;
  }
  if (!inputOwner.flagIsQuadraticRadicalExtensionRationals) {
    Polynomial<AlgebraicNumber> minPoly;
    minPoly.makeMonomial(0, 2);
    minPoly -= input;
    bool result =
    this->constructFromMinimalPolynomial(
      minPoly, inputOwner, commentsOnFailure
    );
    if (result) {
      this->checkConsistency();
    }
    return result;
  }
  List<LargeInteger> factors;
  Rational absoluteInput = input;
  if (absoluteInput < 0) {
    factors.addOnTop(- 1);
    absoluteInput *= - 1;
  }
  LargeInteger squareFreeInput = absoluteInput.getNumerator();
  squareFreeInput *= absoluteInput.getDenominator();
  List<LargeInteger> primeFactors;
  List<int> multiplicities;
  if (
    !squareFreeInput.value.factor(
      primeFactors, multiplicities, 0, 4, nullptr
    )
  ) {
    return false;
  }
  squareFreeInput.value = 1;
  Rational squareRootRationalPart = 1;
  squareRootRationalPart /= absoluteInput.getDenominator();
  for (int i = 0; i < primeFactors.size; i ++) {
    if (multiplicities[i] % 2 == 1) {
      squareFreeInput *= primeFactors[i];
    }
    primeFactors[i].raiseToPower(multiplicities[i] / 2);
    squareRootRationalPart *= primeFactors[i];
  }
  if (!squareFreeInput.isEqualToOne()) {
    factors.addOnTop(squareFreeInput);
  }
  if (factors.size == 0) {
    this->assignRational(squareRootRationalPart, &inputOwner);
    this->checkConsistency();
    return true;
  }
  if (!inputOwner.mergeRadicals(factors)) {
    return false;
  }
  Selection factorSelection;
  factorSelection.initialize(inputOwner.quadraticRadicals.size);
  for (int i = 0; i < factors.size; i ++) {
    if (factors[i] == - 1) {
      factorSelection.addSelectionAppendNewIndex(
        inputOwner.quadraticRadicals.getIndex(- 1)
      );
    } else {
      for (int j = 0; j < inputOwner.quadraticRadicals.size; j ++) {
        if (inputOwner.quadraticRadicals[j] != - 1) {
          if (factors[i] % inputOwner.quadraticRadicals[j] == 0) {
            factorSelection.addSelectionAppendNewIndex(j);
          }
        }
      }
    }
  }
  this->owner = &inputOwner;
  this->basisIndex = this->owner->basisInjections.size - 1;
  this->element.makeEi(
    this->owner->getIndexFromRadicalSelection(factorSelection)
  );
  this->element *= squareRootRationalPart;
  this->checkConsistency();
  return true;
}

bool AlgebraicNumber::radicalMeDefault(
  int radical, std::stringstream* commentsOnError
) {
  STACK_TRACE("AlgebraicNumber::radicalMeDefault");
  if (this->owner == nullptr) {
    if (commentsOnError != nullptr) {
      *commentsOnError
      << "Failed to extract radical: algebraic closure is missing. ";
    }
    return false;
  }
  AlgebraicNumber result, one, minusOne;
  one.assignRational(1, this->owner);
  minusOne.assignRational(- 1, this->owner);
  Polynomial<AlgebraicNumber> polynomial;
  polynomial.addConstant(*this * minusOne);
  MonomialPolynomial leadingMonomial;
  leadingMonomial.makeEi(0, radical);
  polynomial.addMonomial(leadingMonomial, one);
  if (
    !this->owner->adjoinRootMinimalPolynomial(
      polynomial, result, commentsOnError
    )
  ) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Failed to extend algebraic closure field. ";
    }
    return false;
  }
  *this = result;
  return true;
}

std::string AlgebraicClosureRationals::toStringQuadraticRadical(
  FormatExpressions* format
) const {
  (void) format;
  std::stringstream out;
  out << "\\mathbb Q[";
  for (int i = 0; i < this->quadraticRadicals.size; i ++) {
    out << "\\sqrt{" << this->quadraticRadicals[i].toString() << "}";
    if (i != this->quadraticRadicals.size - 1) {
      out << ", ";
    }
  }
  out << "]";
  return HtmlRoutines::getMathNoDisplay(out.str());
}

std::string AlgebraicClosureRationals::toStringFull(FormatExpressions* format)
const {
  std::stringstream out;
  out
  << "Dimension over the rationals: "
  << this->latestBasis.size
  << ". Multiplicative basis follows. ";
  for (int i = 0; i < this->latestBasis.size; i ++) {
    out << "<br>";
    std::stringstream formulaStream;
    if (i < this->displayNamesBasisElements.size) {
      if (this->displayNamesBasisElements[i] == "") {
        formulaStream << "1";
      } else {
        formulaStream << this->displayNamesBasisElements[i];
      }
    } else {
      formulaStream << " e_{" << i + 1 << "}";
    }
    formulaStream << "=" << this->latestBasis[i].toStringMatrixForm(format);
    out << HtmlRoutines::getMathNoDisplay(formulaStream.str());
    if (i != this->latestBasis.size - 1) {
      out << ",  ";
    }
  }
  if (this->flagIsQuadraticRadicalExtensionRationals) {
    out << "<br>Generating element not selected. ";
  } else {
    out
    << "<br>Generating element: "
    << HtmlRoutines::getMathNoDisplay(
      this->generatingElementMatrixForm.toString(format)
    );
  }
  out
  << "<br>There are "
  << this->basisInjections.size
  << " registered old bases. ";
  for (int i = 0; i < this->basisInjections.size; i ++) {
    out
    << "<hr>Basis "
    << i + 1
    << " has "
    << this->basisInjections[i].size
    << " elements: ";
    for (int j = 0; j < this->basisInjections[i].size; j ++) {
      out
      << HtmlRoutines::getMathNoDisplay(
        this->basisInjections[i][j].toString(format)
      );
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
  Rational scale =
  Rational::scaleNormalizeIndex(allCoefficients, indexNonZeroRational);
  for (int i = 0; i < output.size; i ++) {
    output[i] *= scale;
  }
  return scale;
}

AlgebraicNumber AlgebraicClosureRationals::one() {
  return this->fromRational(1);
}

AlgebraicNumber AlgebraicClosureRationals::zero() {
  return this->fromRational(0);
}

AlgebraicNumber AlgebraicClosureRationals::imaginaryUnit() {
  AlgebraicNumber result = - 1;
  result.owner = this;
  std::stringstream comments;
  result.squareRootDefault(&comments);
  return result;
}

AlgebraicNumber AlgebraicClosureRationals::fromRational(const Rational& input)
{
  AlgebraicNumber result(input);
  result.owner = this;
  return result;
}

std::string AlgebraicClosureRationals::toString(FormatExpressions* format)
const {
  STACK_TRACE("AlgebraicClosureRationals::toString");
  (void) format;
  std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.flagUseHTML = false;
  tempFormat.flagUseLatex = true;
  if (this->latestBasis.size == 1) {
    out << HtmlRoutines::getMathNoDisplay("\\mathbb Q");
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
    if (this->element[i].monomialIndex != 0) {
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

std::string AlgebraicNumber::toString(FormatExpressions* format) const {
  if (this->owner == nullptr) {
    if (this->element.isEqualToZero()) {
      return "0";
    }
    return this->element.coefficients[0].toString(format);
  }
  std::stringstream out;
  FormatExpressions tempFormat;
  tempFormat.vectorSpaceEiBasisNames = this->owner->displayNamesBasisElements;
  if (format != nullptr) {
    tempFormat.flagUseFrac = format->flagUseFrac;
  }
  tempFormat.flagUseHTML = false;
  VectorSparse<Rational> additiveVector;
  this->owner->getAdditionTo(*this, additiveVector);
  out << additiveVector.toString(&tempFormat);
  // << "~ in~ the~ field~ " << this->owner->toString();
  if (
    this->basisIndex < this->owner->basisInjections.size - 1 &&
    global.userDebugFlagOn()
  ) {
    out << "[=" << this->toStringNonInjected() << "]";
  }
  return out.str();
}

std::string AlgebraicNumber::toStringNonInjected(FormatExpressions* format)
const {
  if (this->owner == nullptr) {
    if (this->element.isEqualToZero()) {
      return "0";
    }
    return this->element.coefficients[0].toString(format);
  }
  std::stringstream out;
  FormatExpressions tempFormat;
  int dimension = this->owner->basisInjections[this->basisIndex].size;
  tempFormat.vectorSpaceEiBasisNames.setSize(dimension);
  for (int i = 0; i < dimension; i ++) {
    std::stringstream letter;
    letter << "e_{" << this->basisIndex + 1 << ", " << i + 1 << "}";
    tempFormat.vectorSpaceEiBasisNames[i] = letter.str();
  }
  if (format != nullptr) {
    tempFormat.flagUseFrac = format->flagUseFrac;
  }
  out << this->element.toString(&tempFormat);
  // << "~ in~ the~ field~ " << this->owner->toString();
  return out.str();
}

bool AlgebraicNumber::operator==(int other) const {
  return *this == Rational(other);
}

bool AlgebraicNumber::operator!=(const AlgebraicNumber& other) const {
  return !(*this == other);
}

bool AlgebraicNumber::operator!=(int other) const {
  return !(*this == other);
}

void AlgebraicNumber::negate() {
  this->element *= - 1;
}

bool AlgebraicNumber::operator==(const AlgebraicNumber& other) const {
  Rational rationalValue;
  this->checkConsistency();
  other.checkConsistency();
  if (this->isRational(&rationalValue)) {
    return other == rationalValue;
  }
  if (other.isRational(&rationalValue)) {
    return *this == rationalValue;
  }
  if (this->owner != other.owner) {
    global.fatal
    << "This might or might not be a programming error: "
    << "comparing two algebraic number that do not have the same owner. "
    << "The numbers have owners of respective addresses "
    << this->owner
    << " and "
    << other.owner
    << ". The numbers are: "
    << this->toString()
    << " and "
    << other.toString()
    << ". Crashing to let you know. "
    << global.fatal;
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
    << "Attempt to assign non-constant "
    << "polynomial to a Rational number is not allowed. "
    << global.fatal;
  }
}

void AlgebraicNumber::operator=(const Rational& other) {
  this->owner = nullptr;
  this->element.makeEi(0, other);
  this->basisIndex = 0;
}

void AlgebraicNumber::operator=(const AlgebraicNumber& other) {
  this->basisIndex = other.basisIndex;
  this->owner = other.owner;
  this->element = other.element;
}

void AlgebraicNumber::operator=(int other) {
  *this = Rational(other);
}

bool ElementZmodP::needsParenthesisForMultiplication(
  FormatExpressions* format
) const {
  if (format == nullptr) {
    return true;
  }
  return !format->flagSuppressModP;
}

std::string ElementZmodP::toStringModP() const {
  return ElementZmodP::toStringModP(this->modulus);
}

std::string ElementZmodP::toStringModP(const LargeIntegerUnsigned& modulus) {
  std::stringstream out;
  out << "mod (" << modulus.toString() << ")";
  return out.str();
}

std::string ElementZmodP::toStringPolynomiaL(
  const Polynomial<ElementZmodP>& input, FormatExpressions* format
) const {
  std::stringstream out;
  out << input.toString(format) << "\\mod " << this->modulus.toString();
  return out.str();
}

std::string ElementZmodP::toStringPolynomialCalculator(
  const Polynomial<ElementZmodP>& input, FormatExpressions* format
) const {
  std::stringstream out;
  out << "PolynomialModP{}(";
  out << input.toString(format);
  out << ", " << this->modulus << ")";
  return out.str();
}

std::string ElementZmodP::toString(FormatExpressions* format) const {
  bool suppressModulus = false;
  if (format != nullptr) {
    suppressModulus = format->flagSuppressModP;
  }
  std::stringstream out;
  if (suppressModulus) {
    out << this->value.toString();
  } else {
    out << "(" << this->value.toString() << " " << this->toStringModP() << ")";
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
  ElementZmodP otherElement;
  otherElement.modulus = this->modulus;
  otherElement.assignRational(other);
  ElementZmodP result = *this;
  result *= otherElement;
  return result;
}

unsigned int ElementZmodP::hashFunction() const {
  if (this->value.isEqualToZero()) {
    return 0;
  }
  return
  this->value.hashFunction() * HashConstants::constant0 +
  this->modulus.hashFunction() * HashConstants::constant1;
}

void ElementZmodP::checkIamInitialized() const {
  if (this->modulus.isEqualToZero()) {
    global.fatal
    << "Element of Z mod p has non-initialized modulus. "
    << global.fatal;
  }
  if (this->flagDeallocated) {
    global.fatal << "Use after free of element of Z mod p. " << global.fatal;
  }
}

void ElementZmodP::operator=(const LargeInteger& other) {
  this->checkIamInitialized();
  this->value = other.value;
  this->value %= this->modulus;
  if (other.sign == - 1) {
    ElementZmodP minusOne;
    minusOne.makeMinusOne(this->modulus);
    *this *= minusOne;
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

ElementZmodP ElementZmodP::oneStatic() {
  ElementZmodP result;
  result.modulus = 1;
  return result;
}

ElementZmodP ElementZmodP::zero() const {
  ElementZmodP result;
  result.modulus = this->modulus;
  if (this->modulus == 0) {
    global.fatal
    << "Extracting constant one from non-initialized element not allowed. "
    << global.fatal;
  }
  return result;
}

ElementZmodP ElementZmodP::one() const {
  ElementZmodP result;
  result.modulus = this->modulus;
  result.value = 1;
  if (this->modulus == 0) {
    global.fatal
    << "Extracting zero constant from non-initialized element not allowed. "
    << global.fatal;
  }
  return result;
}

void ElementZmodP::convertPolynomialModularToPolynomialIntegral(
  const Polynomial<ElementZmodP>& input,
  Polynomial<Rational>& output,
  bool useNegatives
) {
  STACK_TRACE("ElementZmodP::convertPolynomialModularToPolynomialIntegral");
  output.makeZero();
  if (input.size() == 0) {
    return;
  }
  Rational modulusDividedByTwo = input.coefficients[0].modulus;
  modulusDividedByTwo /= 2;
  for (int i = 0; i < input.size(); i ++) {
    Rational coefficient = input.coefficients[i].value;
    if (useNegatives) {
      if (coefficient > modulusDividedByTwo) {
        coefficient = input.coefficients[i].value;
        coefficient -= input.coefficients[i].modulus;
      }
    }
    output.addMonomial(input[i], coefficient);
  }
}

void ElementZmodP::convertLiftPolynomialModular(
  const Polynomial<ElementZmodP>& input,
  Polynomial<ElementZmodP>& output,
  const LargeIntegerUnsigned& newModulus
) {
  STACK_TRACE("ElementZmodP::convertLiftPolynomialModular");
  if (&input == &output) {
    Polynomial<ElementZmodP> inputCopy = input;
    ElementZmodP::convertLiftPolynomialModular(inputCopy, output, newModulus);
    return;
  }
  output.makeZero();
  if (input.size() == 0) {
    return;
  }
  for (int i = 0; i < input.size(); i ++) {
    ElementZmodP coefficient;
    coefficient.modulus = newModulus;
    coefficient.value = input.coefficients[i].value;
    output.addMonomial(input.monomials[i], coefficient);
  }
}

void ElementZmodP::convertModuloIntegerAfterScalingToIntegral(
  const Polynomial<Rational>& input,
  Polynomial<ElementZmodP>& output,
  const LargeIntegerUnsigned& newModulo
) {
  STACK_TRACE("ElementZmodP::convertModuloIntegerAfterScalingToIntegral");
  Polynomial<Rational> rescaled;
  rescaled = input;
  rescaled.scaleNormalizeLeadingMonomial(
    &MonomialPolynomial::orderDefault()
  );
  output.setExpectedSize(input.size());
  ElementZmodP coefficient;
  coefficient.modulus = newModulo;
  output.makeZero();
  for (int i = 0; i < input.size(); i ++) {
    coefficient = input.coefficients[i];
    output.addMonomial(input[i], coefficient);
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

void ElementZmodP::makeFrom(int inputModulus, int inputValue) {
  if (inputModulus <= 0) {
    global.fatal << "Non-positive modulus not allowed. " << global.fatal;
  }
  this->modulus = inputModulus;
  *this = inputValue;
}

void ElementZmodP::makeFrom(
  const LargeIntegerUnsigned& inputModulus, int inputValue
) {
  if (inputModulus <= 0) {
    global.fatal << "Non-positive modulus not allowed. " << global.fatal;
  }
  this->modulus = inputModulus;
  *this = inputValue;
}

void ElementZmodP::negate() {
  if (this->value.isEqualToZero()) {
    return;
  }
  this->value = this->modulus - this->value;
}

void ElementZmodP::checkEqualModuli(const ElementZmodP& other) {
  if (this->modulus != other.modulus) {
    global.fatal
    << "Attempt to make an operation "
    << "with two elemetns of Z mod P with different moduli, "
    << this->modulus.toString()
    << " and "
    << other.modulus.toString()
    << ". "
    << global.fatal;
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
  ElementZmodP otherElement;
  otherElement.modulus = this->modulus;
  if (!otherElement.assignRational(other)) {
    return false;
  }
  *this += otherElement;
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
  otherTimesMinusOne *= - 1;
  *this += otherTimesMinusOne;
}

void ElementZmodP::operator=(const Rational& other) {
  bool tempB = this->assignRational(other);
  if (!tempB) {
    global.fatal
    << "Using ElementZmodP::operator= to assign a Rational number failed. "
    << "Operator = does not allow failure. "
    << global.fatal;
  }
}

void ElementZmodP::operator=(const int other) {
  LargeInteger otherLargeInteger = other;
  *this = otherLargeInteger;
}

void ElementZmodP::operator-=(const LargeIntegerUnsigned& other) {
  ElementZmodP otherElement;
  otherElement.modulus = this->modulus;
  otherElement = other;
  (*this) -= otherElement;
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
  LargeInteger inverted, otherValue, incomingModulus;
  incomingModulus = this->modulus;
  otherValue = other.value;
  if (!MathRoutines::invertXModN(otherValue, incomingModulus, inverted)) {
    return false;
  }
  *this *= inverted;
  return true;
}
