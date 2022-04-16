// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef header_math_extra_universal_enveloping_implementation_ALREADY_INCLUDED
#define header_math_extra_universal_enveloping_implementation_ALREADY_INCLUDED

#include "math_extra_universal_enveloping.h"

template <class Coefficient>
Coefficient SemisimpleLieAlgebra::getKillingFormProductWRTLevi(
  const ElementSemisimpleLieAlgebra<Coefficient>& left,
  const ElementSemisimpleLieAlgebra<Coefficient>& right,
  const Selection& rootsNotInLevi
) {
  STACK_TRACE("SemisimpleLieAlgebra::getKillingFormProductWRTLevi");
  Coefficient result = 0;
  ElementSemisimpleLieAlgebra<Coefficient> adadAppliedToMon, element;
  ChevalleyGenerator baseGen;
  Vector<Rational> rootsNotInLeviVectorForm = rootsNotInLevi;
  Vector<Rational> weight;
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    weight = this->getWeightOfGenerator(i);
    if (weight.scalarEuclidean(rootsNotInLeviVectorForm) != 0) {
      continue;
    }
    baseGen.makeGenerator(*this, i);
    adadAppliedToMon.makeZero();
    adadAppliedToMon.addMonomial(baseGen, 1);
    this->lieBracket(right, adadAppliedToMon, element);
    this->lieBracket(left, element, adadAppliedToMon);
    result += adadAppliedToMon.getCoefficientOf(baseGen);
  }
  return result;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::makeCasimirWRTLeviParabolic(
  SemisimpleLieAlgebra& owner, const Selection& leviRoots
) {
  STACK_TRACE("ElementUniversalEnveloping::makeCasimirWRTLeviParabolic");
  if (leviRoots.cardinalitySelection == 0) {
    this->makeZero(owner);
    return;
  }
  Coefficient result = 0;
  ElementSemisimpleLieAlgebra<Rational> leftE, rightE;
  ChevalleyGenerator baseGen;
  Selection rootsNotInLEvi = leviRoots;
  rootsNotInLEvi.invertSelection();
  Vector<Rational> rootsNotInLeviVectorForm = rootsNotInLEvi;
  Vector<Rational> weightLeft, weightRight;
  this->makeZero(owner);
  MonomialUniversalEnveloping<Coefficient> monomial;
  Rational coefficient;
  // Coefficient coefficientconverted;
  for (int i = 0; i < owner.getNumberOfGenerators(); i ++) {
    weightLeft = owner.getWeightOfGenerator(i);
    if (weightLeft.scalarEuclidean(rootsNotInLeviVectorForm) != 0) {
      continue;
    }
    if (weightLeft.isEqualToZero()) {
      continue;
    }
    monomial.makeOne(owner);
    int indexOpposite = owner.getGeneratorIndexFromRoot(- weightLeft);
    monomial.generatorsIndices.addOnTop(i);
    monomial.generatorsIndices.addOnTop(indexOpposite);
    monomial.powers.addOnTop(1);
    monomial.powers.addOnTop(1);
    leftE.makeGenerator(i, owner);
    rightE.makeGenerator(indexOpposite, owner);
    coefficient =
    owner.getKillingFormProductWRTLevi(leftE, rightE, rootsNotInLEvi);
    coefficient.invert();
    this->addMonomial(monomial, coefficient);
  }
  Matrix<Rational> killingRestrictedToCartan;
  killingRestrictedToCartan.initialize(
    leviRoots.cardinalitySelection, leviRoots.cardinalitySelection
  );
  for (int i = 0; i < leviRoots.cardinalitySelection; i ++) {
    for (int j = i; j < leviRoots.cardinalitySelection; j ++) {
      weightLeft.makeEi(owner.getRank(), leviRoots.elements[i]);
      weightRight.makeEi(owner.getRank(), leviRoots.elements[j]);
      leftE.makeCartanGenerator(weightLeft, owner);
      rightE.makeCartanGenerator(weightRight, owner);
      killingRestrictedToCartan(i, j) =
      owner.getKillingFormProductWRTLevi(leftE, rightE, rootsNotInLEvi);
      killingRestrictedToCartan(j, i) = killingRestrictedToCartan(i, j);
    }
  }
  killingRestrictedToCartan.invert();
  ElementUniversalEnveloping<Coefficient> leftUE, rightUE;
  Vector<Rational> currentEj;
  for (int i = 0; i < leviRoots.cardinalitySelection; i ++) {
    weightLeft.makeEi(owner.getRank(), leviRoots.elements[i]);
    weightRight.makeZero(owner.getRank());
    for (int j = 0; j < leviRoots.cardinalitySelection; j ++) {
      currentEj.makeEi(owner.getRank(), leviRoots.elements[j]);
      weightRight += currentEj * killingRestrictedToCartan(i, j);
    }
    leftUE.makeCartanGenerator(weightLeft, owner);
    rightUE.makeCartanGenerator(weightRight, owner);
    leftUE *= rightUE;
    *this += leftUE;
  }
  this->simplify();
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::modOutVermaRelations(
  const Vector<Coefficient>* substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  MonomialUniversalEnveloping<Coefficient> monomial;
  ElementUniversalEnveloping<Coefficient> output;
  output.makeZero(*this->owner);
  Coefficient acquiredCoefficient;
  for (int i = 0; i < this->size(); i ++) {
    monomial = (*this)[i];
    monomial.modOutVermaRelations(
      acquiredCoefficient,
      substitutionHiGoesToIthElement,
      ringUnit,
      ringZero
    );
    acquiredCoefficient *= this->coefficients[i];
    output.addMonomial(monomial, acquiredCoefficient);
  }
  this->operator=(output);
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::lieBracketOnTheLeft(
  const ElementSemisimpleLieAlgebra<Rational>& left
) {
  if (this->isEqualToZero()) {
    this->makeZero(*this->owner);
    return;
  }
  ElementUniversalEnveloping<Coefficient> element1, element2;
  element1.assignElementLieAlgebra(
    left, *this->owner, this->coefficients[0].one()
  );
  element2 = *this;
  element2.lieBracketOnTheRight(element1, *this);
}

template <class Coefficient>
bool MonomialUniversalEnveloping<Coefficient>::adjointRepresentationAction(
  const ElementUniversalEnveloping<Coefficient>& input,
  ElementUniversalEnveloping<Coefficient>& output
) const {
  output.makeZero(*this->owner);
  ElementSemisimpleLieAlgebra<Rational> element;
  output = input;
  for (int i = this->generatorsIndices.size - 1; i >= 0; i --) {
    int nextCycleSize;
    if (!this->powers[i].isSmallInteger(&nextCycleSize)) {
      return false;
    }
    element.makeGenerator(this->generatorsIndices[i], *this->owner);
    for (int j = 0; j < nextCycleSize; j ++) {
      output.lieBracketOnTheLeft(element);
    }
    output.simplify();
  }
  return true;
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::adjointRepresentationAction(
  const ElementUniversalEnveloping<Coefficient>& input,
  ElementUniversalEnveloping<Coefficient>& output
) const {
  if (&input == &output) {
    global.fatal
    << "Input not allowed to coincide with output. "
    << global.fatal;
  }
  output.makeZero(*this->owner);
  ElementUniversalEnveloping<Coefficient> summand;
  for (int i = 0; i < this->size(); i ++) {
    if (!(*this)[i].adjointRepresentationAction(input, summand)) {
      return false;
    }
    summand *= this->coefficients[i];
    output += summand;
  }
  return true;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::simplify(
  const Coefficient& ringUnit
) {
  ElementUniversalEnveloping<Coefficient> buffer;
  ElementUniversalEnveloping<Coefficient> outpuT;
  MonomialUniversalEnveloping<Coefficient> monomial;
  Coefficient currentCoefficient;
  outpuT.makeZero(*this->owner);
  for (; this->size() > 0;) {
    // FormatExpressions tempFormat;
    // tempFormat.MakeAlphabetArbitraryWithIndex("g", "h");
    this->popMonomial(this->size() - 1, monomial, currentCoefficient);
    bool reductionOccurred = false;
    for (int i = 0; i < monomial.generatorsIndices.size - 1; i ++) {
      if (
        !this->getOwner().areOrderedProperly(
          monomial.generatorsIndices[i],
          monomial.generatorsIndices[i + 1]
        )
      ) {
        if (monomial.switchConsecutiveIndicesIfTheyCommute(i)) {
          this->addMonomial(monomial, currentCoefficient);
          reductionOccurred = true;
          break;
        }
        if (
          monomial.commutingAnBtoBAnPlusLowerOrderAllowed(
            monomial.powers[i],
            monomial.generatorsIndices[i],
            monomial.powers[i + 1],
            monomial.generatorsIndices[i + 1]
          )
        ) {
          monomial.commuteAnBtoBAnPlusLowerOrder(i, buffer, ringUnit);
          buffer *= currentCoefficient;
          *this += buffer;
          reductionOccurred = true;
          break;
        }
        if (
          monomial.commutingABntoBnAPlusLowerOrderAllowed(
            monomial.powers[i],
            monomial.generatorsIndices[i],
            monomial.powers[i + 1],
            monomial.generatorsIndices[i + 1]
          )
        ) {
          monomial.commuteABntoBnAPlusLowerOrder(i, buffer, ringUnit);
          buffer *= currentCoefficient;
          *this += buffer;
          reductionOccurred = true;
          break;
        }
      }
    }
    if (!reductionOccurred) {
      outpuT.addMonomial(monomial, currentCoefficient);
    }
  }
  *this = outpuT;
}

template <class Coefficient>
bool MonomialUniversalEnveloping<Coefficient>::
commutingAnBtoBAnPlusLowerOrderAllowed(
  Coefficient& leftPower,
  int leftGeneratorIndex,
  Coefficient& rightPower,
  int rightGeneratorIndex
) {
  return
  this->commutingABntoBnAPlusLowerOrderAllowed(
    rightPower, rightGeneratorIndex, leftPower, leftGeneratorIndex
  );
}

template <class Coefficient>
bool MonomialUniversalEnveloping<Coefficient>::
commutingABntoBnAPlusLowerOrderAllowed(
  Coefficient& leftPower,
  int leftGeneratorIndex,
  Coefficient& rightPower,
  int rightGeneratorIndex
) {
  int leftPowerSmall, rightPowerSmall;
  if (!leftPower.isSmallInteger(&leftPowerSmall)) {
    return false;
  }
  if (leftPowerSmall < 0) {
    return false;
  }
  if (rightPower.isSmallInteger(&rightPowerSmall)) {
    if (rightPowerSmall >= 0) {
      return true;
    }
  }
  int numPosRoots = this->getOwner().weylGroup.rootsOfBorel.size;
  int dimension = this->getOwner().weylGroup.cartanSymmetric.numberOfRows;
  if (
    rightGeneratorIndex >= numPosRoots &&
    rightGeneratorIndex < numPosRoots + dimension
  ) {
    return
    this->getOwner().lieBrackets.elements[leftGeneratorIndex][
      rightGeneratorIndex
    ].isEqualToZero();
  }
  return true;
}

template <class Coefficient>
bool MonomialUniversalEnveloping<Coefficient>::
switchConsecutiveIndicesIfTheyCommute(int leftIndex) {
  if (leftIndex >= this->generatorsIndices.size - 1) {
    return false;
  }
  int leftGenerator = this->generatorsIndices[leftIndex];
  int rightGenerator = this->generatorsIndices[leftIndex + 1];
  if (
    !this->getOwner().lieBrackets.elements[leftGenerator][rightGenerator].
    isEqualToZero()
  ) {
    return false;
  }
  this->generatorsIndices.swapTwoIndices(leftIndex, leftIndex + 1);
  this->powers.swapTwoIndices(leftIndex, leftIndex + 1);
  this->simplifyEqualConsecutiveGenerators(leftIndex - 1);
  return true;
}

template <class Coefficient>
void MonomialUniversalEnveloping<Coefficient>::commuteAnBtoBAnPlusLowerOrder(
  int indexA,
  ElementUniversalEnveloping<Coefficient>& output,
  const Coefficient& ringUnit
) {
  if (indexA == this->generatorsIndices.size - 1) {
    return;
  }
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<Coefficient> monomial;
  monomial.makeOne(*this->owner);
  monomial.powers.setExpectedSize(this->generatorsIndices.size + 2);
  monomial.generatorsIndices.setExpectedSize(
    this->generatorsIndices.size + 2
  );
  monomial.powers.size = 0;
  monomial.generatorsIndices.size = 0;
  int rightGeneratorIndeX = this->generatorsIndices[indexA + 1];
  int leftGeneratorIndeX = this->generatorsIndices[indexA];
  Coefficient rightPower, leftPower;
  rightPower = this->powers[indexA + 1];
  leftPower = this->powers[indexA];
  rightPower -= 1;
  int powerDroP = 0;
  Coefficient acquiredCoefficienT, incomingAcquiredCoefficienT;
  acquiredCoefficienT = ringUnit;
  for (int i = 0; i < indexA; i ++) {
    monomial.multiplyByGeneratorPowerOnTheRight(
      this->generatorsIndices[i], this->powers[i]
    );
  }
  MonomialUniversalEnveloping<Coefficient> startMon;
  startMon = monomial;
  ElementSemisimpleLieAlgebra<Rational> adAToTheIthOfB, aElt;
  adAToTheIthOfB.makeGenerator(rightGeneratorIndeX, *this->owner);
  aElt.makeGenerator(leftGeneratorIndeX, *this->owner);
  // Formula realized:
  // a^n b =\sum_{i = 0}^\infty \binom{n}{i} (\ad a)^i (b)a^{n-i}
  // Proof (Dixmier): let L_x stand for left multiplication by x and R_x stand
  // for right multiplication.
  // Then L_x and R_x commute and L_x-R_x =ad_x, i.e.
  // (L_a)^n =(R_a +ad_a)^n.
  do {
    for (int i = 0; i < adAToTheIthOfB.size(); i ++) {
      int newGeneratorIndex = adAToTheIthOfB[i].generatorIndex;
      monomial = startMon;
      incomingAcquiredCoefficienT = acquiredCoefficienT;
      incomingAcquiredCoefficienT *= adAToTheIthOfB.coefficients[i];
      monomial.multiplyByGeneratorPowerOnTheRight(newGeneratorIndex, ringUnit);
      monomial.multiplyByGeneratorPowerOnTheRight(
        leftGeneratorIndeX, leftPower
      );
      monomial.multiplyByGeneratorPowerOnTheRight(
        rightGeneratorIndeX, rightPower
      );
      for (int i = indexA + 2; i < this->generatorsIndices.size; i ++) {
        monomial.multiplyByGeneratorPowerOnTheRight(
          this->generatorsIndices[i], this->powers[i]
        );
      }
      output.addMonomial(monomial, incomingAcquiredCoefficienT);
    }
    powerDroP ++;
    acquiredCoefficienT *= leftPower;
    acquiredCoefficienT /= powerDroP;
    leftPower -= 1;
    this->getOwner().lieBracket(aElt, adAToTheIthOfB, adAToTheIthOfB);
  } while (
    !adAToTheIthOfB.isEqualToZero() && !acquiredCoefficienT.isEqualToZero()
  );
}

template <class Coefficient>
void MonomialUniversalEnveloping<Coefficient>::commuteABntoBnAPlusLowerOrder(
  int index,
  ElementUniversalEnveloping<Coefficient>& output,
  const Coefficient& ringUnit
) {
  if (index == this->generatorsIndices.size - 1) {
    return;
  }
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<Coefficient> monomial;
  monomial.makeOne(*this->owner);
  monomial.powers.setExpectedSize(this->generatorsIndices.size + 2);
  monomial.generatorsIndices.setExpectedSize(
    this->generatorsIndices.size + 2
  );
  monomial.powers.size = 0;
  monomial.generatorsIndices.size = 0;
  int rightGeneratorIndex = this->generatorsIndices[index + 1];
  int leftGeneratorIndex = this->generatorsIndices[index];
  Coefficient rightPower, leftPower;
  rightPower = this->powers[index + 1];
  leftPower = this->powers[index];
  leftPower -= 1;
  int powerDrop = 0;
  Coefficient acquiredCoefficient, incomingAcquiredCoefficient;
  acquiredCoefficient = ringUnit;
  for (int i = 0; i < index; i ++) {
    monomial.multiplyByGeneratorPowerOnTheRight(
      this->generatorsIndices[i], this->powers[i]
    );
  }
  monomial.multiplyByGeneratorPowerOnTheRight(
    this->generatorsIndices[index], leftPower
  );
  MonomialUniversalEnveloping<Coefficient> startMon;
  startMon = monomial;
  ElementSemisimpleLieAlgebra<Rational> adResult, element, rightGeneratorElt;
  adResult.makeGenerator(leftGeneratorIndex, *this->owner);
  rightGeneratorElt.makeGenerator(rightGeneratorIndex, *this->owner);
  // Formula realized:
  // a b^n  &= &\sum_{i = 0}^\infty b^{n-i}(-\ad b)^i (a) \binom{n}{i} .
  // Proof (Dixmier): let L_x stand for left multiplication by x and R_x stand
  // for right multiplication.
  // Then L_x and R_x commute and L_x-R_x =ad_x, i.e.
  // (L_b-ad_b)^n =R_b^n.
  do {
    for (int i = 0; i < adResult.size(); i ++) {
      int newGeneratorIndex = adResult[i].generatorIndex;
      monomial = startMon;
      monomial.multiplyByGeneratorPowerOnTheRight(
        rightGeneratorIndex, rightPower
      );
      incomingAcquiredCoefficient = acquiredCoefficient;
      incomingAcquiredCoefficient *= adResult.coefficients[i];
      monomial.multiplyByGeneratorPowerOnTheRight(newGeneratorIndex, ringUnit);
      for (int i = index + 2; i < this->generatorsIndices.size; i ++) {
        monomial.multiplyByGeneratorPowerOnTheRight(
          this->generatorsIndices[i], this->powers[i]
        );
      }
      output.addMonomial(monomial, incomingAcquiredCoefficient);
    }
    acquiredCoefficient *= rightPower;
    rightPower -= 1;
    this->getOwner().lieBracket(adResult, rightGeneratorElt, element);
    adResult = element;
    powerDrop ++;
    acquiredCoefficient /= powerDrop;
  } while (
    !adResult.isEqualToZero() && !acquiredCoefficient.isEqualToZero()
  );
}

template <class Coefficient>
bool MonomialUniversalEnveloping<Coefficient>::
highestWeightTransposeAntiAutomorphismBilinearForm(
  const MonomialUniversalEnveloping<Coefficient>& right,
  Coefficient& output,
  const Vector<Coefficient>* substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero,
  std::stringstream* logStream
) {
  ElementUniversalEnveloping<Coefficient> element1, element2;
  element1.makeZero(*this->owner);
  element1.addMonomial(*this, ringUnit);
  element2.makeZero(*this->owner);
  element2.addMonomial(right, ringUnit);
  return
  element1.highestWeightTransposeAntiAutomorphismBilinearForm(
    element2,
    output,
    substitutionHiGoesToIthElement,
    ringUnit,
    ringZero,
    logStream
  );
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::applyTransposeAntiAutoOnMe() {
  MonomialUniversalEnveloping<Coefficient> tempMon;
  ElementUniversalEnveloping<Coefficient> result;
  result.makeZero(*this->owner);
  int numPosRoots = this->getOwner().getNumberOfPositiveRoots();
  int rank = this->getOwner().getRank();
  Coefficient coefficient;
  this->checkNumberOfCoefficientsConsistency();
  for (int i = 0; i < this->size(); i ++) {
    const MonomialUniversalEnveloping<Coefficient>& currentMon = (*this)[i];
    coefficient = this->coefficients[i];
    tempMon.owner = currentMon.owner;
    tempMon.powers.size = 0;
    tempMon.generatorsIndices.size = 0;
    for (int j = currentMon.powers.size - 1; j >= 0; j --) {
      int power;
      if (!currentMon.powers[j].isSmallInteger(&power)) {
        return false;
      }
      int generatorIndex = currentMon.generatorsIndices[j];
      if (generatorIndex < numPosRoots) {
        generatorIndex = 2 * numPosRoots + rank - 1 - generatorIndex;
      } else if (generatorIndex >= numPosRoots + rank) {
        generatorIndex = - generatorIndex + 2 * numPosRoots + rank - 1;
      }
      tempMon.multiplyByGeneratorPowerOnTheRight(
        generatorIndex, currentMon.powers[j]
      );
    }
    result.addMonomial(tempMon, coefficient);
  }
  *this = result;
  return true;
}

template <typename Coefficient>
bool SemisimpleLieAlgebra::getElementAdjointRepresentation(
  const ElementUniversalEnveloping<Coefficient>& element,
  Matrix<Coefficient>& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("SemisimpleLieAlgebra::getElementAdjointRepresentation");
  int numberOfGenerators = this->getNumberOfGenerators();
  output.makeZeroMatrix(numberOfGenerators, 0);
  Coefficient one;
  one = 1;
  ElementUniversalEnveloping<Coefficient>
  basisElement,
  actionOnBasisElementUniversalEnveloping;
  ElementSemisimpleLieAlgebra<Coefficient> actionOnBasisElement;
  for (int i = 0; i < numberOfGenerators; i ++) {
    basisElement.makeOneGenerator(i, *this, one);
    if (
      !element.adjointRepresentationAction(
        basisElement, actionOnBasisElementUniversalEnveloping
      )
    ) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Failed to extract action of "
        << element.toString()
        << " on "
        << basisElement.toString();
      }
      return false;
    }
    if (
      !actionOnBasisElementUniversalEnveloping.isLieAlgebraElement(
        actionOnBasisElement
      )
    ) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "<hr>While computing the ad-action of "
        << element.toString()
        << ": element "
        << actionOnBasisElementUniversalEnveloping.toString()
        << " is not in the semisimple Lie algebra.";
      }
      return false;
    }
    for (int j = 0; j < actionOnBasisElement.size(); j ++) {
      int basisIndex = actionOnBasisElement.monomials[j].generatorIndex;
      output(basisIndex, i) = actionOnBasisElement.coefficients[j];
    }
  }
  return true;
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::
highestWeightTransposeAntiAutomorphismBilinearForm(
  const ElementUniversalEnveloping<Coefficient>& right,
  Coefficient& output,
  const Vector<Coefficient>* substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero,
  std::stringstream* logStream
) const {
  output = ringZero;
  ElementUniversalEnveloping<Coefficient> taLeft;
  taLeft = *this;
  if (!taLeft.applyTransposeAntiAutoOnMe()) {
    return false;
  }
  ElementUniversalEnveloping<Coefficient>
  accumulator,
  intermediate,
  element,
  startingElt;
  List<int> oldOrder = this->getOwner().universalEnvelopingGeneratorOrder;
  int numberOfPositiveRoots = this->getOwner().getNumberOfPositiveRoots();
  for (int i = 0; i < numberOfPositiveRoots; i ++) {
    this->getOwner().universalEnvelopingGeneratorOrder[i] = - 1;
  }
  accumulator.makeZero(this->getOwner());
  MonomialUniversalEnveloping<Coefficient> constMon;
  constMon.makeOne(this->getOwner());
  if (logStream != nullptr) {
    *logStream
    << "left eltement transposed: "
    << taLeft.toString(&global.defaultFormat.getElement())
    << "<br>";
    *logStream
    << "right element: "
    << right.toString(&global.defaultFormat.getElement())
    << "<br>";
  }
  startingElt = right;
  startingElt.simplify(ringUnit);
  if (logStream != nullptr) {
    *logStream
    << "right element after simplification: "
    << startingElt.toString(&global.defaultFormat.getElement())
    << "<br>";
  }
  startingElt.modOutVermaRelations(substitutionHiGoesToIthElement, ringUnit);
  if (logStream != nullptr) {
    *logStream
    << "right element after Verma rels: "
    << startingElt.toString(&global.defaultFormat.getElement())
    << "<br>";
  }
  Coefficient leftMonCoeff;
  for (int j = 0; j < taLeft.size(); j ++) {
    intermediate = startingElt;
    const MonomialUniversalEnveloping<Coefficient>& leftMon = taLeft[j];
    leftMonCoeff = taLeft.coefficients[j];
    int power;
    for (int i = leftMon.powers.size - 1; i >= 0; i --) {
      if (leftMon.powers[i].isSmallInteger(&power)) {
        for (int k = 0; k < power; k ++) {
          element.makeOneGenerator(
            leftMon.generatorsIndices[i], this->getOwner(), ringUnit
          );
          MathRoutines::swap(element, intermediate);
          if (logStream != nullptr) {
            // *logStream << "element before mult: " <<
            // element.toString(global,
            // tempFormat) << "<br>";
            *logStream
            << "intermediate before mult: "
            << intermediate.toString(&global.defaultFormat.getElement())
            << "<br>";
          }
          intermediate *= element;
          if (logStream != nullptr) {
            *logStream
            << "intermediate before simplification: "
            << intermediate.toString(&global.defaultFormat.getElement())
            << "<br>";
          }
          intermediate.simplify(ringUnit);
          if (logStream != nullptr) {
            *logStream
            << "intermediate after simplification: "
            << intermediate.toString(&global.defaultFormat.getElement())
            << "<br>";
          }
          intermediate.modOutVermaRelations(
            substitutionHiGoesToIthElement, ringUnit, ringZero
          );
          if (logStream != nullptr) {
            *logStream
            << "intermediate after Verma rels: "
            << intermediate.toString(&global.defaultFormat.getElement())
            << "<br>";
          }
        }
      } else {
        this->getOwner().universalEnvelopingGeneratorOrder = oldOrder;
        return false;
      }
    }
    intermediate *= leftMonCoeff;
    accumulator += intermediate;
    int index = intermediate.monomials.getIndex(constMon);
    if (index != - 1) {
      output += intermediate.coefficients[index];
    }
  }
  if (logStream != nullptr) {
    *logStream
    << "final UE element: "
    << accumulator.toString(&global.defaultFormat.getElement());
  }
  this->getOwner().universalEnvelopingGeneratorOrder = oldOrder;
  return true;
}

template <class Coefficient>
void MonomialUniversalEnveloping<Coefficient>::modOutVermaRelations(
  Coefficient& outputCoeff,
  const Vector<Coefficient>* substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  int numPosRoots = this->getOwner().getNumberOfPositiveRoots();
  int dimension = this->getOwner().getRank();
  outputCoeff = ringUnit;
  for (int i = this->generatorsIndices.size - 1; i >= 0; i --) {
    int indexCurrentGenerator = this->generatorsIndices[i];
    if (indexCurrentGenerator >= numPosRoots + dimension) {
      this->makeOne(*this->owner);
      outputCoeff = ringZero;
      return;
    }
    if (indexCurrentGenerator < numPosRoots) {
      return;
    }
    if (
      indexCurrentGenerator >= numPosRoots &&
      indexCurrentGenerator < numPosRoots + dimension
    ) {
      if (substitutionHiGoesToIthElement == 0) {
        this->makeOne(*this->owner);
        outputCoeff = ringZero;
        return;
      }
      int degree = 0;
      if (!this->powers[i].isSmallInteger(&degree)) {
        return;
      }
      int hIndex = indexCurrentGenerator - numPosRoots;
      Coefficient substitutedH;
      substitutedH = (*substitutionHiGoesToIthElement)[hIndex];
      MathRoutines::raiseToPower(substitutedH, degree, ringUnit);
      outputCoeff *= substitutedH;
      this->generatorsIndices.size --;
      this->powers.size --;
    }
  }
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::substitution(
  const PolynomialSubstitution<Rational>& polynomialSubstitution
) {
  ElementUniversalEnveloping<Coefficient> output;
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<Coefficient> monomial;
  Coefficient tempCF;
  for (int i = 0; i < this->size(); i ++) {
    monomial = (*this)[i];
    monomial.substitution(polynomialSubstitution);
    tempCF = this->coefficients[i];
    tempCF.substitution(polynomialSubstitution, 1, nullptr);
    output.addMonomial(monomial, tempCF);
  }
  *this = output;
}

template <class Coefficient>
void MonomialUniversalEnveloping<Coefficient>::substitution(
  const PolynomialSubstitution<Rational>& substitution
) {
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    this->powers[i].substitution(substitution, 1, nullptr);
  }
  this->simplifyEqualConsecutiveGenerators(0);
}

template <class Coefficient>
void MonomialUniversalEnveloping<Coefficient>::setNumberOfVariables(
  int newNumberOfVariables
) {
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    this->powers.objects[i].setNumberOfVariablesSubstituteDeletedByOne(
      newNumberOfVariables
    );
  }
}

template <class Coefficient>
std::string MonomialUniversalEnveloping<Coefficient>::toString(
  FormatExpressions* format
) const {
  std::stringstream out;
  std::string tempS;
  if (this->owner == nullptr) {
    return "(Error:Programming:NonInitializedMonomial)";
  }
  if (this->generatorsIndices.size == 0) {
    return "1";
  }
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    Coefficient& power = this->powers[i];
    int index = this->generatorsIndices[i];
    tempS = this->getOwner().getStringFromChevalleyGenerator(index, format);
    out << tempS;
    if (!power.isEqualToOne()) {
      out << "^";
      out << "{";
      out << power.toString(format);
      out << "}";
    }
  }
  return out.str();
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::makeCasimir(
  SemisimpleLieAlgebra& inputOwner
) {
  // std::stringstream out;
  this->makeZero(inputOwner);
  WeylGroupData& weylGroup = this->getOwner().weylGroup;
  int dimension = weylGroup.cartanSymmetric.numberOfRows;
  Vector<Rational> root1;
  Vector<Rational> root2;
  //  Matrix<Rational> killingForm;
  //  killingForm.initialize(dimension, dimension);
  //  for (int i = 0; i < dimension; i ++)
  //  { root1.makeEi(dimension, i);
  //    for (int j = 0; j < dimension; j ++)
  //    { killingForm.elements[i][j] = 0;
  //      root2.makeEi(dimension, j);
  //      for (int k = 0; k < weylGroup.RootSystem.size; k++)
  // killingForm.elements[i][j] += weylGroup.rootScalarCartanRoot(root1,
  // weylGroup.RootSystem.objects[k])* weylGroup.rootScalarCartanRoot(root2,
  // weylGroup.RootSystem.objects[k]);
  //    }
  //  }
  //  killingForm.invert(global);
  //  killingForm.ComputeDebugString();
  //  out << killingForm.toString(true, false);
  ElementUniversalEnveloping<Coefficient> element1, element2;
  // this code is to check a math formula:
  //  ElementUniversalEnveloping checkElement;
  //  checkElement.makeZero(owner);
  Matrix<Rational> invertedSymCartan;
  invertedSymCartan = weylGroup.cartanSymmetric;
  invertedSymCartan.invert();
  for (int i = 0; i < dimension; i ++) {
    root1.makeEi(dimension, i);
    // implementation without the ninja formula:
    //    killingForm.actOnVectorColumn(root1, root2);
    //    element1.makeCartanGenerator(root1, numVars, owner);
    //    element2.makeCartanGenerator(root2, numVars, owner);
    //    element1*= element2;
    //    *this+= element1;
    // Alternative implementation using a ninja formula I cooked up after
    // looking at the printouts:
    invertedSymCartan.actOnVectorColumn(root1, root2);
    element1.makeCartanGenerator(root1, inputOwner);
    element2.makeCartanGenerator(root2, inputOwner);
    element1 *= element2;
    *this += element1;
  }
  // Rational rational;
  for (int i = 0; i < weylGroup.rootSystem.size; i ++) {
    // Implementation without the ninja formula:
    //    rational = 0;
    //    Vector<Rational> & root = weylGroup.RootSystem.objects[i];
    //    int indexOfOpposite = weylGroup.RootSystem.getIndex(-root);
    // Vector<Rational> & opposite =
    // weylGroup.RootSystem.objects[indexOfOpposite];
    //    for (int j = 0; j < weylGroup.RootSystem.size; j ++)
    //    { Vector<Rational> & current = weylGroup.RootSystem.objects[j];
    //      if (current == opposite)
    //        rational +=2;
    //       else
    //       { int indexOfSum= weylGroup.RootSystem.getIndex(current +root);
    //         if (indexOfSum!= - 1)
    // rational
    // +=(owner.ChevalleyConstants.elements[i][j]*owner.ChevalleyConstants.elements[indexOfOpposite][indexOfSum]);
    //       }
    //     }
    //     rational +=2;
    //     rational = 1/rational;
    //     element2.makeOneGeneratorCoefficientOne(opposite, numVars, owner);
    //     element1.makeOneGeneratorCoefficientOne(root, numVars, owner);
    //     element2*= element1;
    //
    //     element2*= rational;
    //     *this+= element2;
    // The ninja formula alternative implementation:
    const Vector<Rational>& root =
    weylGroup.rootSystem[i];
    element2.makeOneGeneratorCoefficientOne(- root, inputOwner);
    element1.makeOneGeneratorCoefficientOne(root, inputOwner);
    element2 *= element1;
    element2 *=
    weylGroup.rootScalarCartanRoot(
      weylGroup.rootSystem[i], weylGroup.rootSystem[i]
    ) /
    2;
    *this += element2;
  }
  *this /= weylGroup.getKillingDividedByTraceRatio();
  // check that the ninja formula is correct:
  //  FormatExpressions tempPolyFormat;
  //  tempPolyFormat.MakeAlphabetArbitraryWithIndex("g", "h");
  // this->DebugString= out.str();
  //  Vector<Rational> root;
  //  for (int i = 0; i < dimension; i ++)
  //  { root.makeEi(dimension, i);
  //    if (!length1Explored)
  //    { length1= weylGroup.rootScalarCartanRoot(root, root);
  //      length1Explored = true;
  //      coefficient1= 0;
  //      for (int j = 0; j < weylGroup.rootsOfBorel.size; j ++)
  // { coefficient1+= weylGroup.rootScalarCartanRoot(root,
  // weylGroup.rootsOfBorel.objects[j])*weylGroup.rootScalarCartanRoot(root,
  // weylGroup.rootsOfBorel.objects[j]);
  //        coef
  //      }
  //    }
  //  }
  this->simplify(0);
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::substitutionCoefficients(
  PolynomialSubstitution<Rational>& polynomialSubstitution,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  ElementUniversalEnveloping<Coefficient> endResult;
  MonomialUniversalEnveloping<Coefficient> currentMon;
  endResult.makeZero(*this->owner);
  Coefficient tempCF;
  for (int i = 0; i < this->size; i ++) {
    currentMon = this->objects[i];
    this->coefficients[i].substitution(polynomialSubstitution);
    endResult.addMonomial(currentMon, tempCF);
  }
  endResult.simplify(ringUnit, ringZero);
  this->operator=(endResult);
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::lieBracketOnTheRight(
  const ElementUniversalEnveloping<Coefficient>& right,
  ElementUniversalEnveloping<Coefficient>& output
) const {
  ElementUniversalEnveloping<Coefficient> element, element2;
  element = *this;
  element *= right;
  element2 = right;
  element2 *= *this;
  output = element;
  output -= element2;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::cleanUpZeroCoefficient() {
  for (int i = 0; i < this->size; i ++) {
    if ((*this)[i].coefficient.isEqualToZero()) {
      this->removeIndexSwapWithLast(i);
      i --;
    }
  }
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::makeOneGenerator(
  int index, SemisimpleLieAlgebra& inputOwner, const Coefficient& ringUnit
) {
  this->makeZero(inputOwner);
  MonomialUniversalEnveloping<Coefficient> result;
  result.makeOne(inputOwner);
  result.multiplyByGeneratorPowerOnTheRight(index, ringUnit);
  this->addMonomial(result, ringUnit);
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::makeOneGeneratorCoefficientOne(
  int index, SemisimpleLieAlgebra& inputOwner, const Coefficient& ringUnit
) {
  this->makeZero(inputOwner);
  MonomialUniversalEnveloping<Coefficient> result;
  result.makeOne(inputOwner);
  result.multiplyByGeneratorPowerOnTheRight(index, ringUnit);
  this->addMonomial(result, ringUnit);
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::isLieAlgebraElement(
  ElementSemisimpleLieAlgebra<Coefficient>& whichElement
) const {
  whichElement.makeZero();
  Coefficient exponent;
  ChevalleyGenerator chevalleyGenerator;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialUniversalEnveloping<Coefficient>& monomial = (*this)[i];
    monomial.getDegree(exponent);
    if (!exponent.isEqualToOne()) {
      return false;
    }
    chevalleyGenerator.makeGenerator(
      *this->owner, monomial.generatorsIndices[0]
    );
    whichElement.addMonomial(chevalleyGenerator, this->coefficients[i]);
  }
  return true;
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::isLieAlgebraElementRational(
  ElementSemisimpleLieAlgebra<Rational>& whichElement
) const {
  whichElement.makeZero();
  Coefficient exponent;
  ChevalleyGenerator chevalleyGenerator;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialUniversalEnveloping<Coefficient>& monomial = (*this)[i];
    monomial.getDegree(exponent);
    if (!exponent.isEqualToOne()) {
      return false;
    }
    Rational coefficient;
    if (!this->coefficients[i].isConstant(&coefficient)) {
      return false;
    }
    chevalleyGenerator.makeGenerator(
      *this->owner, monomial.generatorsIndices[0]
    );
    whichElement.addMonomial(chevalleyGenerator, coefficient);
  }
  return true;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::assignElementLieAlgebra(
  const ElementSemisimpleLieAlgebra<Rational>& input,
  SemisimpleLieAlgebra& inputOwner,
  const Coefficient& ringUnit
) {
  this->makeZero(inputOwner);
  MonomialUniversalEnveloping<Coefficient> tempMon;
  tempMon.makeOne(inputOwner);
  tempMon.generatorsIndices.setSize(1);
  tempMon.powers.setSize(1);
  tempMon.powers[0] = ringUnit;
  Coefficient tempCF;
  for (int i = 0; i < input.size(); i ++) {
    tempCF = ringUnit;
    // <- to facilitate implicit type conversion: ringUnit does not have to be
    // of type Rational
    tempCF *= input.coefficients[i];
    // <- to facilitate implicit type conversion: ringUnit does not have to be
    // of type Rational
    tempMon.generatorsIndices[0] = input[i].generatorIndex;
    this->addMonomial(tempMon, tempCF);
  }
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::getCoordinateFormOfSpanOfElements
(
  List<ElementUniversalEnveloping<Coefficient> >& elements,
  Vectors<Coefficient>& outputCoordinates,
  ElementUniversalEnveloping<Coefficient>& outputCorrespondingMonomials
) {
  if (elements.size == 0) {
    return;
  }
  outputCorrespondingMonomials.makeZero(
    *elements[0].owners, elements[0].indexInOwners
  );
  for (int i = 0; i < elements.size; i ++) {
    for (int j = 0; j < elements[i].size; j ++) {
      outputCorrespondingMonomials.addOnTopNoRepetition(elements[i][j]);
    }
  }
  outputCoordinates.setSize(elements.size);
  Polynomial<Rational> ZeroPoly;
  ZeroPoly.makeZero();
  for (int i = 0; i < elements.size; i ++) {
    Vector<Coefficient>& current = outputCoordinates[i];
    current.initializeFillInObject(
      outputCorrespondingMonomials.size, ZeroPoly
    );
    ElementUniversalEnveloping& currentElt = elements[i];
    for (int j = 0; j < currentElt.size; j ++) {
      MonomialUniversalEnveloping<Coefficient>& currentMon = currentElt[j];
      current[outputCorrespondingMonomials.getIndex(currentMon)] =
      currentElt.coefficients[j];
    }
  }
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::makeCartanGenerator(
  const Vector<Rational>& input, SemisimpleLieAlgebra& inputOwner
) {
  MonomialUniversalEnveloping<Coefficient> tempMon;
  this->makeZero(inputOwner);
  tempMon.makeOne(inputOwner);
  int dimension = this->getOwner().weylGroup.cartanSymmetric.numberOfRows;
  int numPosRoots = this->getOwner().weylGroup.rootsOfBorel.size;
  tempMon.generatorsIndices.setSize(1);
  tempMon.powers.setSize(1);
  Coefficient tempCF;
  for (int i = 0; i < dimension; i ++) {
    if (!input[i].isEqualToZero()) {
      (*tempMon.generatorsIndices.lastObject()) = i + numPosRoots;
      *tempMon.powers.lastObject() = 1;
      tempCF = 1;
      // <- to facilitate type conversion
      tempCF *= input[i];
      // <- to facilitate type conversion we call extra assignment
      this->addMonomial(tempMon, tempCF);
    }
  }
}

template <class Coefficient>
void MonomialUniversalEnveloping<Coefficient>::simplify(
  ElementUniversalEnveloping<Coefficient>& output,
  const Coefficient& ringUnit
) {
  output.makeZero(*this->owner);
  output.addOnTop(*this);
  this->simplifyAccumulateInOutputNoOutputInit(output, ringUnit);
}

template <class Coefficient>
bool ElementUniversalEnvelopingOrdered<Coefficient>::
getElementUniversalEnveloping(
  ElementUniversalEnveloping<Coefficient>& output,
  SemisimpleLieAlgebra& inputOwner
) {
  ElementUniversalEnveloping<Coefficient> Accum, element;
  Accum.makeZero(inputOwner);
  for (int i = 0; i < this->size; i ++) {
    if (
      !this->objects[i].getElementUniversalEnveloping(element, owner)
    ) {
      return false;
    } else {
      Accum += element;
    }
  }
  output = Accum;
  return true;
}

template <class Coefficient>
bool MonomialUniversalEnvelopingOrdered<Coefficient>::
getElementUniversalEnveloping(
  ElementUniversalEnveloping<Coefficient>& output,
  SemisimpleLieAlgebraOrdered& inputOwner
) {
  ElementUniversalEnveloping<Coefficient> accumulator;
  ElementUniversalEnveloping<Coefficient> tempMon;
  int index;
  int degree = 0;
  accumulator.makeConstant(this->Coefficient, inputOwner);
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    if (this->powers[i].isSmallInteger(&degree)) {
      tempMon.assignElementLieAlgebra(
        this->owner->elementOrder[this->generatorsIndices[i]],
        inputOwner,
        this->Coefficient.getOne(),
        this->Coefficient.GetZero()
      );
      tempMon.raiseToPower(degree);
      accumulator *= tempMon;
    } else {
      if (
        this->owner->elementOrder[this->generatorsIndices[i]].
        isCoefficientOneChevalleyGenerator()
      ) {
        tempMon.makeOneGeneratorCoefficientOne(
          index,
          inputOwner,
          this->Coefficient.getOne(),
          this->Coefficient.GetZero()
        );
        tempMon[0].powers[0] = this->powers[i];
        accumulator *= tempMon;
      } else {
        return false;
      }
    }
  }
  output.operator=(accumulator);
  return true;
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::setNumberOfVariables(
  int newNumberOfVariables
) {
  this->Coefficient.setNumberOfVariablesSubstituteDeletedByOne(
    newNumberOfVariables
  );
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    this->powers[i].setNumberOfVariablesSubstituteDeletedByOne(
      newNumberOfVariables
    );
  }
}

template <class Coefficient>
bool ElementUniversalEnvelopingOrdered<Coefficient>::getCoordinatesInBasis(
  List<ElementUniversalEnvelopingOrdered<Coefficient> >& basis,
  Vector<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) const {
  List<ElementUniversalEnvelopingOrdered<Coefficient> > tempBasis, elements;
  tempBasis = basis;
  tempBasis.addOnTop(*this);
  Vectors<Coefficient> tempCoords;
  this->getBasisFromSpanOfElements(
    tempBasis, tempCoords, elements, ringUnit, ringZero
  );
  Vector<Coefficient> lastRoot;
  lastRoot = *tempCoords.lastObject();
  tempCoords.setSize(basis.size);
  return
  lastRoot.getCoordinatesInBasis(tempCoords, output, ringUnit, ringZero);
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::
getCoordinateFormOfSpanOfElements(
  List<ElementUniversalEnvelopingOrdered>& elements,
  Vectors<Polynomial<Coefficient> >& outputCoordinates,
  ElementUniversalEnvelopingOrdered& outputCorrespondingMonomials
) {
  if (elements.size == 0) {
    return;
  }
  outputCorrespondingMonomials.makeZero(*elements[0].owner);
  for (int i = 0; i < elements.size; i ++) {
    for (int j = 0; j < elements[i].size; j ++) {
      outputCorrespondingMonomials.addOnTopNoRepetition(elements[i][j]);
    }
  }
  outputCoordinates.setSize(elements.size);
  Polynomial<Rational> ZeroPoly;
  ZeroPoly.makeZero();
  for (int i = 0; i < elements.size; i ++) {
    Vector<Polynomial<Coefficient> >& current = outputCoordinates[i];
    current.initializeFillInObject(
      outputCorrespondingMonomials.size, ZeroPoly
    );
    ElementUniversalEnvelopingOrdered& currentElt = elements[i];
    for (int j = 0; j < currentElt.size; j ++) {
      MonomialUniversalEnvelopingOrdered<Coefficient>& currentMon =
      currentElt[j];
      current.objects[outputCorrespondingMonomials.getIndex(currentMon)] =
      currentMon.coefficient;
    }
  }
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::substitutionCoefficients(
  PolynomialSubstitution<Rational>& substitution
) {
  if (substitution.size < 1) {
    return;
  }
  this->Coefficient.substitution(substitution);
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::substitutionCoefficients(
  PolynomialSubstitution<Rational>& substitution
) {
  ElementUniversalEnvelopingOrdered<Coefficient> endResult;
  MonomialUniversalEnvelopingOrdered<Coefficient> currentMon;
  endResult.makeZero(*this->owner);
  for (int i = 0; i < this->size; i ++) {
    currentMon = (*this)[i];
    currentMon.substitutionCoefficients(substitution);
    endResult.addMonomial(currentMon);
  }
  this->operator=(endResult);
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::makeZero(
  SemisimpleLieAlgebra& inputOwner
) {
  this->::LinearCombination<
    MonomialUniversalEnveloping<Coefficient>, Coefficient
  >::makeZero();
  this->owner = &inputOwner;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::multiplyBy(
  const MonomialUniversalEnveloping<Coefficient>& standsOnTheRight,
  const Coefficient& coefficient
) {
  if (standsOnTheRight.generatorsIndices.size == 0) {
    return;
  }
  ElementUniversalEnveloping<Coefficient> output;
  output.setExpectedSize(this->size);
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<Coefficient> tempMon;
  Coefficient newCoeff;
  for (int i = 0; i < this->size; i ++) {
    tempMon = (*this)[i];
    tempMon *= standsOnTheRight;
    newCoeff = this->coefficients[i];
    newCoeff *= coefficient;
    output.addMonomial(tempMon, newCoeff);
  }
  *this = output;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::operator*=(
  const ElementUniversalEnveloping& standsOnTheRight
) {
  this->::ElementMonomialAlgebra<
    MonomialUniversalEnveloping<Coefficient>, Coefficient
  >::operator*=(
    static_cast<
      ElementMonomialAlgebra<
        MonomialUniversalEnveloping<Coefficient>, Coefficient
      >
    >(standsOnTheRight)
  );
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::raiseToPower(int power) {
  ElementUniversalEnveloping<Coefficient> buffer;
  buffer = *this;
  if (this->size() == 0) {
    return;
  }
  this->makeConstant(this->coefficients[0].one(), *this->owner);
  for (int i = 0; i < power; i ++) {
    this->operator*=(buffer);
  }
}

template <class Coefficient>
void ElementVermaModuleOrdered<Coefficient>::getBasisFromSpanOfElements(
  List<ElementVermaModuleOrdered>& elements,
  Vectors<RationalFraction<Rational> >& outputCoordinates,
  List<ElementVermaModuleOrdered>& outputTheBasis,
  const RationalFraction<Rational>& rationalFractionOne,
  const RationalFraction<Rational>& rationalFractionZero
) {
  List<ElementUniversalEnvelopingOrdered<Coefficient> > elementsUEform;
  elementsUEform.setSize(elements.size);
  for (int i = 0; i < elements.size; i ++) {
    elementsUEform[i] = elements[i].elementInternal;
  }
  List<ElementUniversalEnvelopingOrdered<Coefficient> >
  basisUniversalEnvelopingForm;
  ElementUniversalEnvelopingOrdered<Coefficient>::getBasisFromSpanOfElements(
    elementsUEform,
    outputCoordinates,
    basisUniversalEnvelopingForm,
    rationalFractionOne,
    rationalFractionZero,
    global
  );
  outputTheBasis.setSize(basisUniversalEnvelopingForm.size);
  for (int i = 0; i < basisUniversalEnvelopingForm.size; i ++) {
    outputTheBasis[i].elementInternal = basisUniversalEnvelopingForm[i];
    outputTheBasis[i].substitutionNthElementIsImageNthCoordinateSimpleBasis =
    elements[0].substitutionNthElementIsImageNthCoordinateSimpleBasis;
  }
}

template <class Coefficient>
bool ElementVermaModuleOrdered<Coefficient>::getCoordinatesInBasis(
  const List<ElementVermaModuleOrdered<Coefficient> >& basis,
  Vector<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) const {
  List<ElementUniversalEnvelopingOrdered<Coefficient> > elementsUEform;
  elementsUEform.setSize(basis.size);
  for (int i = 0; i < basis.size; i ++) {
    elementsUEform.objects[i] = basis.objects[i].elementInternal;
  }
  return
  this->elementInternal.getCoordinatesInBasis(
    elementsUEform, output, ringUnit, ringZero
  );
}

template <class Coefficient>
bool ElementVermaModuleOrdered<Coefficient>::needsParenthesisForMultiplication(
) const {
  return this->elementInternal.needsParenthesisForMultiplication();
}

template <class Coefficient>
void ElementVermaModuleOrdered<Coefficient>::
assignElementUniversalEnvelopingOrderedTimesHighestWeightVector(
  ElementUniversalEnvelopingOrdered<Coefficient>& input,
  const ElementVermaModuleOrdered<Coefficient>& ringZero,
  const Coefficient& ringUnit
) {
  this->elementInternal.operator=(input);
  if (
    ringZero.substitutionNthElementIsImageNthCoordinateSimpleBasis.size != 3
  ) {
    global.fatal
    << "ringZero.substitutionNthElementIsImageNthCoordinateSimpleBasis.size "
    << "is not equal to 3 as expected. "
    << global.fatal;
  }
  this->substitutionNthElementIsImageNthCoordinateSimpleBasis =
  ringZero.substitutionNthElementIsImageNthCoordinateSimpleBasis;
  this->elementInternal.modOutVermaRelationsOld(
    false,
    this->substitutionNthElementIsImageNthCoordinateSimpleBasis,
    ringUnit
  );
}

template <class Coefficient>
void ElementVermaModuleOrdered<Coefficient>::multiplyOnTheLeft(
  const ElementSemisimpleLieAlgebra<Rational>& other,
  ElementVermaModuleOrdered<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  ElementUniversalEnvelopingOrdered<Coefficient> element;
  element.assignElementLieAlgebra(
    other, ringUnit, ringZero, *this->elementInternal.owner
  );
  element *= this->elementInternal;
  output.elementInternal = element;
  output.elementInternal.simplify(ringUnit, ringZero);
  output.elementInternal.modOutVermaRelationsOld(
    false,
    this->substitutionNthElementIsImageNthCoordinateSimpleBasis,
    ringUnit
  );
}

template <class Coefficient>
std::string ElementVermaModuleOrdered<Coefficient>::toString(
  const FormatExpressions& format
) const {
  std::stringstream out;
  std::string tempS =
  MathRoutines::toStringBrackets(this->elementInternal, format);
  if (tempS.size() > 1) {
    out << "(";
  }
  if (tempS != "1") {
    out << tempS;
  }
  if (tempS.size() > 1) {
    out << ")";
  }
  if (tempS != "0") {
    out << " v";
  }
  return out.str();
}

template <class Coefficient>
void ElementVermaModuleOrdered<Coefficient>::actOnMe(
  const ElementSemisimpleLieAlgebra<Rational>& actingElt,
  ElementVermaModuleOrdered<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) const {
  ElementUniversalEnvelopingOrdered<Coefficient> element;
  element.assignElementLieAlgebra(
    actingElt, ringUnit, ringZero, *this->elementInternal.owner
  );
  element.lieBracketOnTheRight(
    this->elementInternal, output.elementInternal
  );
  output.elementInternal.simplify(ringUnit, ringZero);
  output.substitutionNthElementIsImageNthCoordinateSimpleBasis =
  this->substitutionNthElementIsImageNthCoordinateSimpleBasis;
  output.elementInternal.modOutVermaRelationsOld(
    false,
    this->substitutionNthElementIsImageNthCoordinateSimpleBasis,
    ringUnit
  );
}

template <class Coefficient>
template <class CoefficientTypeQuotientField>
void ElementUniversalEnvelopingOrdered<Coefficient>::getBasisFromSpanOfElements
(
  List<ElementUniversalEnvelopingOrdered<Coefficient> >& elements,
  Vectors<CoefficientTypeQuotientField>& outputCoords,
  List<ElementUniversalEnvelopingOrdered<Coefficient> >& outputTheBasis,
  const CoefficientTypeQuotientField& fieldUnit,
  const CoefficientTypeQuotientField& fieldZero
) {
  if (elements.size == 0) {
    return;
  }
  ElementUniversalEnvelopingOrdered<Coefficient> outputCorrespondingMonomials;
  outputCorrespondingMonomials.makeZero(*elements[0].owner);
  Vectors<CoefficientTypeQuotientField> outputCoordsBeforeReduction;
  for (int i = 0; i < elements.size; i ++) {
    for (int j = 0; j < elements[i].size; j ++) {
      outputCorrespondingMonomials.addOnTopNoRepetition(elements[i][j]);
    }
  }
  outputCoordsBeforeReduction.setSize(elements.size);
  for (int i = 0; i < elements.size; i ++) {
    Vector<CoefficientTypeQuotientField>& currentList =
    outputCoordsBeforeReduction[i];
    currentList.makeZero(outputCorrespondingMonomials.size, fieldZero);
    ElementUniversalEnvelopingOrdered<Coefficient>& currentElt = elements[i];
    for (int j = 0; j < currentElt.size; j ++) {
      MonomialUniversalEnvelopingOrdered<Coefficient>& currentMon =
      currentElt[j];
      currentList.objects[
        outputCorrespondingMonomials.getIndex(currentMon)
      ] =
      currentMon.coefficient;
    }
  }
  outputTheBasis.size = 0;
  outputTheBasis.reserve(elements.size);
  Vectors<CoefficientTypeQuotientField> basisCoordForm;
  basisCoordForm.reserve(elements.size);
  Selection selectedBasis;
  outputCoordsBeforeReduction.ComputeDebugString();
  outputCoordsBeforeReduction.SelectABasis(
    basisCoordForm, fieldZero, selectedBasis
  );
  for (int i = 0; i < selectedBasis.cardinalitySelection; i ++) {
    outputTheBasis.addOnTop(elements[selectedBasis.elements[i]]);
  }
  Matrix<Coefficient> bufferMat;
  Vectors<Coefficient> bufferVectors;
  outputCoordsBeforeReduction.getCoordinatesInBasis(
    basisCoordForm,
    outputCoords,
    bufferVectors,
    bufferMat,
    fieldUnit,
    fieldZero
  );
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::simplify(
  const Coefficient& ringUnit, const Coefficient& ringZero
) {
  ElementUniversalEnvelopingOrdered buffer;
  ElementUniversalEnvelopingOrdered output;
  output.makeZero(*this->owner);
  for (int i = 0; i < this->size; i ++) {
    this->objects[i].simplify(buffer, ringUnit, ringZero);
    output += buffer;
  }
  *this = output;
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::makeZero(
  SemisimpleLieAlgebraOrdered& inputOwner
) {
  this->clear();
  this->owner = &inputOwner;
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::simplify(
  ElementUniversalEnvelopingOrdered<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  output.makeZero(*this->owner);
  output.addOnTop(*this);
  this->simplifyAccumulateInOutputNoOutputInit(output, ringUnit, ringZero);
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::cleanUpZeroCoefficient() {
  for (int i = 0; i < this->size; i ++) {
    if (this->objects[i].coefficient.isEqualToZero()) {
      this->removeIndexSwapWithLast(i);
      i --;
    }
  }
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::operator+=(
  const ElementUniversalEnvelopingOrdered<Coefficient>& other
) {
  this->reserve(this->size + other.size);
  for (int i = 0; i < other.size; i ++) {
    this->addMonomialNoCleanUpZeroCoefficient(other[i]);
  }
  this->cleanUpZeroCoefficient();
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::operator-=(
  const ElementUniversalEnvelopingOrdered<Coefficient>& other
) {
  this->reserve(this->size + other.size);
  MonomialUniversalEnvelopingOrdered<Coefficient> tempMon;
  for (int i = 0; i < other.size; i ++) {
    tempMon = other.objects[i];
    tempMon.coefficient *= - 1;
    this->addMonomialNoCleanUpZeroCoefficient(tempMon);
  }
  this->cleanUpZeroCoefficient();
}

template <class Coefficient>
template <class otherType>
void ElementUniversalEnvelopingOrdered<Coefficient>::operator*=(
  const otherType& other
) {
  if (other.isEqualToZero()) {
    this->makeZero(*this->owner);
    return;
  }
  for (int i = 0; i < this->size; i ++) {
    this->objects[i].coefficient.operator*=(other);
  }
}

template <class Coefficient>
template <class SecondType>
void ElementUniversalEnvelopingOrdered<Coefficient>::operator/=(
  const SecondType& other
) {
  for (int i = 0; i < this->size; i ++) {
    this->objects[i].coefficient.operator/=(other);
  }
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::multiplyByNoSimplify(
  const MonomialUniversalEnvelopingOrdered& other
) {
  if (this == &other) {
    global.fatal
    << "Output not allowed to be equal to this object. "
    << global.fatal;
  }
  this->generatorsIndices.reserve(
    other.generatorsIndices.size + this->generatorsIndices.size
  );
  this->powers.reserve(
    other.generatorsIndices.size + this->generatorsIndices.size
  );
  this->Coefficient.multiplyBy(other.coefficient);
  if (other.generatorsIndices.size == 0) {
    return;
  }
  int firstIndex = other.generatorsIndices[0];
  int i = 0;
  if (this->generatorsIndices.size > 0) {
    if (firstIndex == (*this->generatorsIndices.lastObject())) {
      *this->powers.lastObject() += other.powers[0];
      i = 1;
    }
  }
  for (; i < other.generatorsIndices.size; i ++) {
    this->powers.addOnTop(other.powers[i]);
    this->generatorsIndices.addOnTop(other.generatorsIndices[i]);
  }
}

template <class Coefficient>
bool ElementUniversalEnvelopingOrdered<Coefficient>::isProportionalTo(
  const ElementUniversalEnvelopingOrdered<Coefficient>& other,
  Coefficient& outputTimesMeEqualsOther,
  const Coefficient& ringZero
) const {
  if (this->isEqualToZero()) {
    if (other.isEqualToZero()) {
      return true;
    }
    return false;
  }
  if (other.isEqualToZero()) {
    outputTimesMeEqualsOther = ringZero;
    return true;
  }
  if (other.size != this->size) {
    return false;
  }
  MonomialUniversalEnvelopingOrdered<Coefficient>& monomial = (*this)[0];
  int index = other.getIndex(monomial);
  if (index == - 1) {
    return false;
  }
  MonomialUniversalEnvelopingOrdered<Coefficient>& otherMon = other[index];
  monomial.ComputeDebugString();
  otherMon.ComputeDebugString();
  outputTimesMeEqualsOther = otherMon.coefficient;
  outputTimesMeEqualsOther /= monomial.coefficient;
  ElementUniversalEnvelopingOrdered<Coefficient> element;
  element = *this;
  element *= outputTimesMeEqualsOther;
  element -= other;
  return element.isEqualToZero();
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::operator*=(
  const MonomialUniversalEnvelopingOrdered& other
) {
  if (this == &other) {
    global.fatal << "Output and input are the same. " << global.fatal;
  }
  this->Coefficient *= other.coefficient;
  for (int i = 0; i < other.generatorsIndices.size; i ++) {
    this->multiplyByGeneratorPowerOnTheRight(
      other.generatorsIndices[i], other.powers[i]
    );
  }
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::operator*=(
  const ElementUniversalEnvelopingOrdered<Coefficient>& other
) {
  ElementUniversalEnvelopingOrdered output;
  output.makeZero(*this->owner);
  output.reserve(this->size * other.size);
  MonomialUniversalEnvelopingOrdered<Coefficient> tempMon;
  for (int i = 0; i < this->size; i ++) {
    for (int j = 0; j < other.size; j ++) {
      tempMon = (*this)[i];
      tempMon *= other[j];
      output.addMonomial(tempMon);
    }
  }
  *this = output;
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::
addMonomialNoCleanUpZeroCoefficient(
  const MonomialUniversalEnvelopingOrdered<Coefficient>& input
) {
  int index = this->getIndex(input);
  if (index == - 1) {
    this->addOnTop(input);
  } else {
    this->objects[index].coefficient += input.coefficient;
  }
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::
simplifyAccumulateInOutputNoOutputInit(
  ElementUniversalEnvelopingOrdered<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  int indexlowestNonSimplified = 0;
  ElementUniversalEnvelopingOrdered<Coefficient> buffer2;
  MonomialUniversalEnvelopingOrdered<Coefficient> tempMon;
  // simplified order is descending order
  while (indexlowestNonSimplified < output.size) {
    bool reductionOccurred = false;
    if (output[indexlowestNonSimplified].coefficient.isEqualToZero()) {
      reductionOccurred = true;
    } else {
      for (
        int i = 0; i < output[indexlowestNonSimplified].generatorsIndices.size
        -
        1; i ++
      ) {
        if (
          output[indexlowestNonSimplified].generatorsIndices[i] >
          output[indexlowestNonSimplified].generatorsIndices[i + 1]
        ) {
          if (
            output[indexlowestNonSimplified].
            switchConsecutiveIndicesIfTheyCommute(i, tempMon, ringZero)
          ) {
            output.addMonomialNoCleanUpZeroCoefficient(tempMon);
            reductionOccurred = true;
            break;
          }
          if (
            this->commutingRightIndexAroundLeftIndexAllowed(
              output[indexlowestNonSimplified].powers[i],
              output[indexlowestNonSimplified].generatorsIndices[i],
              output[indexlowestNonSimplified].powers[i + 1],
              output[indexlowestNonSimplified].generatorsIndices[i + 1]
            )
          ) {
            output[indexlowestNonSimplified].
            commuteConsecutiveIndicesRightIndexAroundLeft(
              i, buffer2, ringUnit, ringZero
            );
            for (int j = 0; j < buffer2.size; j ++) {
              output.addMonomialNoCleanUpZeroCoefficient(buffer2[j]);
            }
            reductionOccurred = true;
            break;
          }
          if (
            this->commutingLeftIndexAroundRightIndexAllowed(
              output[indexlowestNonSimplified].powers[i],
              output[indexlowestNonSimplified].generatorsIndices[i],
              output[indexlowestNonSimplified].powers[i + 1],
              output[indexlowestNonSimplified].generatorsIndices[i + 1]
            )
          ) {
            output[indexlowestNonSimplified].
            commuteConsecutiveIndicesLeftIndexAroundRight(
              i, buffer2, ringUnit, ringZero
            );
            for (int j = 0; j < buffer2.size; j ++) {
              output.addMonomialNoCleanUpZeroCoefficient(buffer2[j]);
            }
            reductionOccurred = true;
            break;
          }
        }
      }
    }
    if (reductionOccurred) {
      output.removeIndexSwapWithLast(indexlowestNonSimplified);
    } else {
      indexlowestNonSimplified ++;
    }
  }
  output.cleanUpZeroCoefficient();
}

template <class Coefficient>
bool MonomialUniversalEnvelopingOrdered<Coefficient>::
switchConsecutiveIndicesIfTheyCommute(
  int leftIndex,
  MonomialUniversalEnvelopingOrdered<Coefficient>& output,
  const Coefficient& ringZero
) {
  if (leftIndex >= this->generatorsIndices.size - 1) {
    return false;
  }
  int leftGeneratorIndex = this->generatorsIndices[leftIndex];
  int rightGeneratorIndex = this->generatorsIndices[leftIndex + 1];
  ElementSemisimpleLieAlgebra<Rational> element;
  this->owner->ownerSemisimpleLieAlgebra->lieBracket(
    this->owner->elementOrder[leftGeneratorIndex],
    this->owner->elementOrder[rightGeneratorIndex],
    element
  );
  if (element.isEqualToZero()) {
    output.generatorsIndices.reserve(this->generatorsIndices.size);
    output.powers.reserve(this->generatorsIndices.size);
    output.makeZero(ringZero, *this->owner);
    output.coefficient = this->Coefficient;
    // output.ComputeDebugString();
    for (int i = 0; i < leftIndex; i ++) {
      output.multiplyByGeneratorPowerOnTheRight(
        this->generatorsIndices[i], this->powers[i]
      );
    }
    output.multiplyByGeneratorPowerOnTheRight(
      this->generatorsIndices[leftIndex + 1],
      this->powers[leftIndex + 1]
    );
    output.multiplyByGeneratorPowerOnTheRight(
      this->generatorsIndices[leftIndex], this->powers[leftIndex]
    );
    for (int i = leftIndex + 2; i < this->generatorsIndices.size; i ++) {
      output.multiplyByGeneratorPowerOnTheRight(
        this->generatorsIndices[i], this->powers[i]
      );
    }
    return true;
  }
  return false;
}

template <class Coefficient>
bool MonomialUniversalEnvelopingOrdered<Coefficient>::
commutingRightIndexAroundLeftIndexAllowed(
  Coefficient& leftPower,
  int leftGeneratorIndex,
  Coefficient& rightPower,
  int rightGeneratorIndex
) {
  return
  this->commutingLeftIndexAroundRightIndexAllowed(
    rightPower, rightGeneratorIndex, leftPower, leftGeneratorIndex
  );
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::
commuteConsecutiveIndicesRightIndexAroundLeft(
  int index,
  ElementUniversalEnvelopingOrdered<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  if (index == this->generatorsIndices.size - 1) {
    return;
  }
  output.makeZero(*this->owner);
  MonomialUniversalEnvelopingOrdered tempMon;
  tempMon.makeZero(ringZero, *this->owner);
  tempMon.powers.reserve(this->generatorsIndices.size + 2);
  tempMon.generatorsIndices.reserve(this->generatorsIndices.size + 2);
  tempMon.powers.size = 0;
  tempMon.generatorsIndices.size = 0;
  int rightGeneratorIndeX = this->generatorsIndices.objects[index + 1];
  int leftGeneratorIndeX = this->generatorsIndices.objects[index];
  Coefficient rightPower, leftPower;
  rightPower = this->powers.objects[index + 1];
  leftPower = this->powers.objects[index];
  rightPower -= 1;
  int powerDroP = 0;
  //  if (this->flagAnErrorHasOccurredTimeToPanic)
  // if (this->toString() == "2f_{5}f_{-5}f_{-4}" || this->toString() ==
  // "2f_{11}f_{-4}")
  //  { this->flagAnErrorHasOccurredTimeToPanic = true;
  //  }
  Coefficient acquiredCoefficienT;
  acquiredCoefficienT = this->Coefficient;
  tempMon.coefficient = this->Coefficient;
  for (int i = 0; i < index; i ++) {
    tempMon.multiplyByGeneratorPowerOnTheRight(
      this->generatorsIndices[i], this->powers[i]
    );
  }
  MonomialUniversalEnvelopingOrdered startMon;
  startMon = tempMon;
  ElementSemisimpleLieAlgebra<Rational> adResulT, element, leftElement;
  this->owner->assignGeneratorCoefficientOne(rightGeneratorIndeX, adResulT);
  this->owner->assignGeneratorCoefficientOne(leftGeneratorIndeX, leftElement);
  // tempLefttElt.ComputeDebugString(*this->owner, false, false);
  Vector<Rational> coefficients;
  do {
    this->owner->getLinearCombinationFrom(adResulT, coefficients);
    for (int i = 0; i < coefficients.size; i ++) {
      if (coefficients[i] != 0) {
        int newGeneratorIndex = i;
        tempMon = startMon;
        if (this->flagAnErrorHasOccurredTimeToPanic) {
          tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.coefficient = acquiredCoefficienT;
        if (this->flagAnErrorHasOccurredTimeToPanic) {
          tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.coefficient *= coefficients[i];
        if (this->flagAnErrorHasOccurredTimeToPanic) {
          tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.multiplyByGeneratorPowerOnTheRight(
          newGeneratorIndex, ringUnit
        );
        tempMon.multiplyByGeneratorPowerOnTheRight(
          leftGeneratorIndeX, leftPower
        );
        tempMon.multiplyByGeneratorPowerOnTheRight(
          rightGeneratorIndeX, rightPower
        );
        for (int i = index + 2; i < this->generatorsIndices.size; i ++) {
          tempMon.multiplyByGeneratorPowerOnTheRight(
            this->generatorsIndices.objects[i],
            this->powers.objects[i]
          );
        }
        if (this->flagAnErrorHasOccurredTimeToPanic) {
          tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        output.addOnTop(tempMon);
      }
    }
    acquiredCoefficienT *= leftPower;
    leftPower -= 1;
    this->owner->ownerSemisimpleLieAlgebra->lieBracket(
      leftElement, adResulT, element
    );
    adResulT = element;
    powerDroP ++;
    acquiredCoefficienT /= powerDroP;
  } while (
    !adResulT.isEqualToZero() && !acquiredCoefficienT.isEqualToZero()
  );
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::
commuteConsecutiveIndicesLeftIndexAroundRight(
  int index,
  ElementUniversalEnvelopingOrdered<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  if (index == this->generatorsIndices.size - 1) {
    return;
  }
  output.makeZero(*this->owner);
  MonomialUniversalEnvelopingOrdered tempMon;
  tempMon.makeZero(ringZero, *this->owner);
  tempMon.powers.setExpectedSize(this->generatorsIndices.size + 2);
  tempMon.generatorsIndices.setExpectedSize(
    this->generatorsIndices.size + 2
  );
  tempMon.powers.size = 0;
  tempMon.generatorsIndices.size = 0;
  int rightGeneratorIndex = this->generatorsIndices.objects[index + 1];
  int leftGeneratorIndex = this->generatorsIndices.objects[index];
  Coefficient rightPower, leftPower;
  rightPower = this->powers.objects[index + 1];
  leftPower = this->powers.objects[index];
  leftPower -= 1;
  int powerDrop = 0;
  Coefficient acquiredCoefficient;
  acquiredCoefficient = this->Coefficient;
  tempMon.coefficient = this->Coefficient;
  for (int i = 0; i < index; i ++) {
    tempMon.multiplyByGeneratorPowerOnTheRight(
      this->generatorsIndices.objects[i],
      this->powers.objects[i]
    );
  }
  tempMon.multiplyByGeneratorPowerOnTheRight(
    this->generatorsIndices.objects[index], leftPower
  );
  MonomialUniversalEnvelopingOrdered startMon, tempMon2;
  startMon = tempMon;
  ElementSemisimpleLieAlgebra<Rational> adResult, element, tempRightElt;
  this->owner->assignGeneratorCoefficientOne(leftGeneratorIndex, adResult);
  this->owner->assignGeneratorCoefficientOne(
    rightGeneratorIndex, tempRightElt
  );
  //  tempRightElt.ComputeDebugString(*this->owner, false, false);
  Vector<Rational> coefficients;
  do {
    this->owner->getLinearCombinationFrom(adResult, coefficients);
    for (int i = 0; i < coefficients.size; i ++) {
      if (coefficients[i] != 0) {
        int newGeneratorIndex = i;
        tempMon = startMon;
        tempMon.multiplyByGeneratorPowerOnTheRight(
          rightGeneratorIndex, rightPower
        );
        tempMon.coefficient = acquiredCoefficient;
        tempMon.coefficient *= coefficients[i];
        tempMon.multiplyByGeneratorPowerOnTheRight(
          newGeneratorIndex, ringUnit
        );
        for (int i = index + 2; i < this->generatorsIndices.size; i ++) {
          tempMon.multiplyByGeneratorPowerOnTheRight(
            this->generatorsIndices[i], this->powers[i]
          );
        }
        output.addOnTop(tempMon);
      }
    }
    acquiredCoefficient *= rightPower;
    rightPower -= 1;
    this->owner->ownerSemisimpleLieAlgebra->lieBracket(
      adResult, tempRightElt, element
    );
    adResult = element;
    powerDrop ++;
    acquiredCoefficient /= powerDrop;
    // adResult.ComputeDebugString(*this->owner, false, false);
  } while (
    !adResult.isEqualToZero() && !acquiredCoefficient.isEqualToZero()
  );
}

template <class Coefficient>
bool MonomialUniversalEnvelopingOrdered<Coefficient>::
commutingLeftIndexAroundRightIndexAllowed(
  Coefficient& leftPower,
  int leftGeneratorIndex,
  Coefficient& rightPower,
  int rightGeneratorIndex
) {
  int tempInt;
  if (leftPower.isSmallInteger(&tempInt)) {
    if (rightPower.isSmallInteger(&tempInt)) {
      return true;
    }
    int numPosRoots =
    this->owner->ownerSemisimpleLieAlgebra->weylGroup.rootsOfBorel.size;
    int dimension =
    this->owner->ownerSemisimpleLieAlgebra->weylGroup.cartanSymmetric.
    numberOfRows;
    if (
      rightGeneratorIndex >= numPosRoots &&
      rightGeneratorIndex < numPosRoots + dimension
    ) {
      ElementSemisimpleLieAlgebra<Rational> element;
      this->owner->ownerSemisimpleLieAlgebra->lieBracket(
        this->owner->elementOrder[leftGeneratorIndex],
        this->owner->elementOrder[rightGeneratorIndex],
        element
      );
      if (element.isEqualToZero()) {
        return true;
      } else {
        return false;
      }
    } else {
      return true;
    }
  }
  return false;
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::makeZero(
  int numberOfVariables, SemisimpleLieAlgebraOrdered& inputOwner
) {
  this->Coefficient.makeZero(numberOfVariables);
  this->owner = &inputOwner;
  this->generatorsIndices.size = 0;
  this->powers.size = 0;
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::makeZero(
  const Coefficient& ringZero, SemisimpleLieAlgebraOrdered& inputOwner
) {
  this->coefficient = ringZero;
  this->owner = &inputOwner;
  this->generatorsIndices.size = 0;
  this->powers.size = 0;
}

template <class Coefficient>
unsigned int MonomialUniversalEnvelopingOrdered<Coefficient>::hashFunction()
const {
  unsigned int result = 0;
  int j = 0;
  for (int i = 0; i < this->powers.size; i ++) {
    result +=
    HashConstants::getConstantIncrementCounter(j) *
    this->generatorsIndices.objects[i];
  }
  return result;
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::
multiplyByGeneratorPowerOnTheRight(int generatorIndex, int power) {
  if (power == 0) {
    return;
  }
  Polynomial<Rational> tempP;
  tempP.makeConstant(power);
  this->multiplyByGeneratorPowerOnTheRight(generatorIndex, tempP);
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::
multiplyByGeneratorPowerOnTheRight(
  int generatorIndex, const Coefficient& power
) {
  if (power.isEqualToZero()) {
    return;
  }
  if (this->generatorsIndices.size > 0) {
    if (*this->generatorsIndices.lastObject() == generatorIndex) {
      (*this->powers.lastObject()) += power;
      return;
    }
  }
  this->powers.addOnTop(power);
  this->generatorsIndices.addOnTop(generatorIndex);
}

template <class Coefficient>
std::string MonomialUniversalEnvelopingOrdered<Coefficient>::toString(
  FormatExpressions* format
) const {
  if (this->owner == nullptr) {
    return "Faulty monomial: non-initialized owner. ";
  }
  if (this->isEqualToZero()) {
    return "0";
  }
  std::stringstream out;
  std::string tempS;
  if (this->generatorsIndices.size > 0) {
    tempS = MathRoutines::toStringBrackets(this->Coefficient, format);
    if (tempS == "1") {
      tempS = "";
    }
    if (tempS == "- 1" || tempS == "-1") {
      tempS = "-";
    }
  } else {
    tempS = this->Coefficient.toString(format);
  }
  out << tempS;
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    Coefficient& power = this->powers[i];
    int index = this->generatorsIndices[i];
    bool usebrackets = false;
    tempS =
    this->owner->ownerSemisimpleLieAlgebra->getStringFromChevalleyGenerator(
      index, format
    );
    if (usebrackets) {
      out << "(";
    }
    out << tempS;
    if (usebrackets) {
      out << ")";
    }
    tempS = power.toString(format);
    if (tempS != "1") {
      out << "^";
      // if (useLatex)
      out << "{";
      out << tempS;
      // if (useLatex)
      out << "}";
    }
    // if (power>1)
    //  out << ")";
  }
  return out.str();
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::toString(
  std::string& output, FormatExpressions* PolyFormatLocal
) const {
  std::stringstream out;
  std::string tempS;
  if (this->size == 0) {
    out << "0";
  }
  int indexCharacterAtLastLineBreak = 0;
  for (int i = 0; i < this->size; i ++) {
    MonomialUniversalEnvelopingOrdered<Coefficient>& current =
    this->objects[i];
    tempS = current.toString(PolyFormatLocal);
    if (i != 0) {
      if (tempS.size() > 0) {
        if (tempS[0] != '-') {
          out << '+';
        }
      }
    }
    out << tempS;
    if ((static_cast<int>(out.tellp())) - indexCharacterAtLastLineBreak > 150
    ) {
      indexCharacterAtLastLineBreak = out.tellp();
      out << "\\\\&&";
    }
  }
  output = out.str();
}

template <class Coefficient>
bool ElementUniversalEnvelopingOrdered<Coefficient>::
assignElementUniversalEnveloping(
  ElementUniversalEnveloping<Coefficient>& input,
  SemisimpleLieAlgebraOrdered& owner,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  ElementUniversalEnvelopingOrdered<Coefficient> element;
  this->makeZero(owner);
  for (int i = 0; i < input.size; i ++) {
    if (
      !element.assignMonomialUniversalEnveloping(
        input.objects[i],
        input.coefficients[i],
        owner,
        ringUnit,
        ringZero
      )
    ) {
      return false;
    }
    this->operator+=(element);
  }
  this->simplify(ringUnit, ringZero);
  return true;
}

template <class Coefficient>
bool ElementUniversalEnvelopingOrdered<Coefficient>::
assignMonomialUniversalEnveloping(
  MonomialUniversalEnveloping<Coefficient>& input,
  const Coefficient& inputCoeff,
  SemisimpleLieAlgebraOrdered& owner,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  ElementUniversalEnvelopingOrdered monomial;
  ElementSemisimpleLieAlgebra<Rational> element;
  Coefficient coefficient;
  coefficient = inputCoeff;
  this->makeConstant(coefficient, owner);
  for (int i = 0; i < input.generatorsIndices.size; i ++) {
    int power;
    bool isASmallInt = input.powers.objects[i].isSmallInteger(&power);
    if (isASmallInt) {
      element.makeGenerator(
        input.generatorsIndices.objects[i],
        *input.owners,
        input.indexInOwners
      );
      monomial.assignElementLieAlgebra(element, ringUnit, ringZero, owner);
      monomial.raiseToPower(power, ringUnit);
    } else {
      return false;
    }
    this->multiplyBy(monomial);
  }
  return true;
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::assignElementLieAlgebra(
  const ElementSemisimpleLieAlgebra<Rational>& input,
  const Coefficient& ringUnit,
  const Coefficient& ringZero,
  SemisimpleLieAlgebraOrdered& inputOwner
) {
  this->makeZero(inputOwner);
  Vector<Rational> elementRootForm;
  input.toVectorNegativeRootSpacesFirst(elementRootForm);
  inputOwner.chevalleyGeneratorsInCurrentCoordinates.actOnVectorColumn(
    elementRootForm, ringZero
  );
  MonomialUniversalEnvelopingOrdered<Coefficient> tempMon;
  tempMon.makeZero(ringZero, inputOwner);
  tempMon.generatorsIndices.setSize(1);
  tempMon.powers.setSize(1);
  tempMon.powers.objects[0] = ringUnit;
  for (int index = 0; index < elementRootForm.size; index ++) {
    if (elementRootForm.objects[index] != 0) {
      tempMon.coefficient = ringUnit;
      tempMon.coefficient *= elementRootForm.objects[index];
      tempMon.generatorsIndices[0] = index;
      this->addOnTop(tempMon);
    }
  }
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::raiseToPower(
  int power, const Coefficient& ringUnit
) {
  if (this->size == 0) {
    return;
  }
  ElementUniversalEnvelopingOrdered<Coefficient> buffer;
  buffer.operator=(*this);
  this->makeConstant(ringUnit, *this->owner);
  for (int i = 0; i < power; i ++) {
    this->operator*=(buffer);
  }
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::lieBracketOnTheRight(
  const ElementSemisimpleLieAlgebra<Rational>& right,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  ElementUniversalEnvelopingOrdered<Coefficient> element;
  element.assignElementLieAlgebra(
    right, ringUnit, ringZero, *this->owner
  );
  this->lieBracketOnTheRight(element, *this);
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::lieBracketOnTheRight(
  const ElementUniversalEnvelopingOrdered& right,
  ElementUniversalEnvelopingOrdered& output
) {
  ElementUniversalEnvelopingOrdered element, element2;
  element = *this;
  element *= right;
  element2 = right;
  element2 *= *this;
  output = element;
  output -= element2;
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::addMonomial(
  const MonomialUniversalEnvelopingOrdered<Coefficient>& input
) {
  if (input.isEqualToZero()) {
    return;
  }
  int index = this->getIndex(input);
  if (index == - 1) {
    this->addOnTop(input);
  } else {
    this->objects[index].coefficient += input.coefficient;
    if (this->objects[index].coefficient.isEqualToZero()) {
      this->removeIndexSwapWithLast(index);
    }
  }
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::modOutVermaRelationsOld(
  bool substitutionHighestWeightWithZeroes,
  const PolynomialSubstitution<Rational>& highestWeightSub,
  const Coefficient& ringUnit
) {
  MonomialUniversalEnvelopingOrdered<Coefficient> tempMon;
  ElementUniversalEnvelopingOrdered<Coefficient> output;
  output.makeZero(*this->owner);
  for (int i = 0; i < this->size; i ++) {
    tempMon = this->objects[i];
    tempMon.modOutVermaRelationsOld(
      substitutionHighestWeightWithZeroes, highestWeightSub, ringUnit
    );
    output.addMonomial(tempMon);
  }
  this->operator=(output);
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::modOutVermaRelations(
  const List<Coefficient>* substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  MonomialUniversalEnvelopingOrdered<Coefficient> tempMon;
  ElementUniversalEnvelopingOrdered<Coefficient> output;
  output.makeZero(*this->owner);
  for (int i = 0; i < this->size; i ++) {
    tempMon = this->objects[i];
    tempMon.modOutVermaRelations(
      substitutionHiGoesToIthElement, ringUnit, ringZero
    );
    output.addMonomial(tempMon);
  }
  this->operator=(output);
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::modOutVermaRelations(
  const List<Coefficient>* substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  int numPosRoots =
  this->owner->ownerSemisimpleLieAlgebra->getNumberOfPositiveRoots();
  int dimension = this->owner->ownerSemisimpleLieAlgebra->getRank();
  for (int i = this->generatorsIndices.size - 1; i >= 0; i --) {
    int indexCurrentGenerator = this->generatorsIndices[i];
    if (indexCurrentGenerator >= numPosRoots + dimension) {
      this->makeZero(ringZero, *this->owner);
      return;
    }
    if (indexCurrentGenerator < numPosRoots) {
      return;
    }
    if (
      indexCurrentGenerator >= numPosRoots &&
      indexCurrentGenerator < numPosRoots + dimension
    ) {
      int degree = 0;
      if (!this->powers[i].isSmallInteger(degree)) {
        return;
      }
      if (substitutionHiGoesToIthElement == 0) {
        this->makeZero(ringZero, *this->owner);
        return;
      }
      Coefficient substitutedH = ringZero;
      Vector<Rational> currentH =
      this->owner->elementOrder[indexCurrentGenerator].getCartanPart();
      for (int j = 0; j < currentH.size; j ++) {
        substitutedH += (*substitutionHiGoesToIthElement)[j] * currentH[j];
      }
      MathRoutines::raiseToPower(substitutedH, degree, ringUnit);
      this->coefficient *= substitutedH;
      this->generatorsIndices.size --;
      this->powers.size --;
    }
  }
}

template <class Coefficient>
Coefficient ElementUniversalEnveloping<Coefficient>::getKillingFormProduct(
  const ElementUniversalEnveloping<Coefficient>& right
) const {
  STACK_TRACE("ElementUniversalEnveloping::getKillingFormProduct");
  if (this->isEqualToZero()) {
    return 0;
  }
  Coefficient result = 0;
  ElementUniversalEnveloping<Coefficient> adadAppliedToMon, element;
  SemisimpleLieAlgebra* owner = &this->getOwner();
  MonomialUniversalEnveloping<Coefficient> baseGen;
  for (int i = 0; i < owner->getNumberOfGenerators(); i ++) {
    baseGen.makeGenerator(i, *owner);
    adadAppliedToMon.makeZero(*owner);
    adadAppliedToMon.addMonomial(baseGen, 1);
    right.adjointRepresentationAction(adadAppliedToMon, element);
    element.simplify(Coefficient::one());
    this->adjointRepresentationAction(element, adadAppliedToMon);
    adadAppliedToMon.simplify(Coefficient::one());
    result += adadAppliedToMon.getCoefficientOf(baseGen);
  }
  return result;
}

#endif // header_math_extra_universal_enveloping_implementation_ALREADY_INCLUDED
